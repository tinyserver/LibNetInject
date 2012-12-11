/*****************************************************************************
 *
 *  Copyright (c) 2010 Thomas Volkert <thomas@homer-conferencing.com>
 *  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *****************************************************************************/

/*
 * Purpose: UDP packet handling
 * Author:  Thomas Volkert
 * Since:   2010-10-06
 */

#include <PacketUdp.h>
#include <PacketIp.h>
#include <HBLogger.h>

#include <string>

namespace LibNetInject {

using namespace std;
using namespace Homer::Base;

///////////////////////////////////////////////////////////////////////////////

PacketUdp::PacketUdp():
    PacketIp(PACKET_TYPE_UDP)
{
    Reset(false);
}

PacketUdp::~PacketUdp()
{

}

///////////////////////////////////////////////////////////////////////////////

void PacketUdp::SetUdpSourcePort(unsigned short int pPort)
{
    mSourcePort = pPort;
}

void PacketUdp::SetUdpDestinationPort(unsigned short int pPort)
{
    mDestinationPort = pPort;
}

void PacketUdp::SetUdpPayload(char *pData, unsigned int pDataSize)
{
    mUdpPayloadData = (u_int8_t*)pData;
    mUdpPayloadSize = (u_int32_t)pDataSize;
}

void PacketUdp::Reset(bool pRecursive)
{
    LOG(LOG_VERBOSE, "Going to reset header data structures");
    mReceivedPacketHeaderUdp = NULL;
    SetUdpSourcePort(0);
    SetUdpDestinationPort(0);
    SetUdpPayload(NULL, 0);
    if (pRecursive)
        PacketIp::Reset();
}

int PacketUdp::Send()
{
    libnet_ptag_t tProtocolTag = 0; //unused till now
    int tResult = -1;

    // ########################################
    // ### debug output
    // ########################################
    LOG(LOG_VERBOSE, "################################### UDP ##################################");
    LOG(LOG_VERBOSE, "Source: %hu", mSourcePort);
    LOG(LOG_VERBOSE, "Dest.: %hu", mDestinationPort);
    LOG(LOG_VERBOSE, "Payload-data: %p", mUdpPayloadData);
    LOG(LOG_VERBOSE, "Payload-size: %d", mUdpPayloadSize);

    // ########################################
    // ### build header
    // ########################################
    tProtocolTag = libnet_build_udp(
                                        mSourcePort,
                                        mDestinationPort,
                                        LIBNET_UDP_H + mUdpPayloadSize,
                                        0, /* checksum is auto. filled by libnet */
                                        mUdpPayloadData,
                                        mUdpPayloadSize,
                                        Packet::mSendContext,
                                        LIBNET_PTAG_INITIALIZER); // protocol tag to modify an existing header, 0 to build a new one
                                        // returns: protocol tag value on success, -1 on error
    if (tProtocolTag == -1)
    {
        LOG(LOG_ERROR, "libnet_build_udp failed because of \"%s\"", libnet_geterror(Packet::mSendContext));
    	tResult = errno;
    }else
    {
		// ########################################
		// ### call next layer
		// ########################################
		tResult = PacketIp::Send();
    }

    return tResult;
}

bool PacketUdp::PrepareReceive(string pPacketFilter, int pTimeoutInMs, bool pReceiveForeignPackets)
{
    pPacketFilter = "udp";
    return Packet::PrepareReceive(pPacketFilter, pTimeoutInMs, pReceiveForeignPackets);
}

bool PacketUdp::Receive()
{
    bool tResult = PacketIp::Receive();
    if (tResult)
    {
        mReceivedPacketHeaderUdp = (HeaderUdp*)(((char*)mReceivedPacketHeaderIp) + mReceivedPacketHeaderIp->ihl * 4);
        #ifdef DEBUG_PACKETS_UDP
            PrintHeaderUdp(false);
        #endif
    }
    return tResult;
}

void PacketUdp::PrintHeaderUdp(bool pRecursive)
{
    if (pRecursive)
        PrintHeaderIp(pRecursive);
    if (mReceivedPacketHeaderUdp == NULL)
        return;

    printf("################################### UDP #################################\n");
    printf("Source: %hu\n", ntohs(mReceivedPacketHeaderUdp->sport));
    printf("Dest.: %hu\n", ntohs(mReceivedPacketHeaderUdp->dport));
    printf("Header length: 8 bytes\n");
    printf("Payload length: %hu bytes\n", ntohs(mReceivedPacketHeaderUdp->len) - 8); // payload = datagram length - udp header
}

HeaderUdp *PacketUdp::GetHeaderUdp()
{
    return mReceivedPacketHeaderUdp;
}

///////////////////////////////////////////////////////////////////////////////

} //namespace
