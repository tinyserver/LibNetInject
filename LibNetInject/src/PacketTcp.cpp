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
 * Purpose: TCP packet handling
 * Author:  Thomas Volkert
 * Since:   2010-10-07
 */

#include <PacketTcp.h>
#include <PacketIp.h>
#include <HBLogger.h>

#include <string>

namespace LibNetInject {

using namespace std;
using namespace Homer::Base;

///////////////////////////////////////////////////////////////////////////////

PacketTcp::PacketTcp():
    PacketIp(PACKET_TYPE_TCP)
{
    Reset(false);
}

PacketTcp::~PacketTcp()
{

}

///////////////////////////////////////////////////////////////////////////////

void PacketTcp::SetTcpSourcePort(unsigned short int pPort)
{
    mSourcePort = pPort;
}

void PacketTcp::SetTcpDestinationPort(unsigned short int pPort)
{
    mDestinationPort = pPort;
}

void PacketTcp::SetTcpPayload(char *pData, unsigned int pDataSize)
{
    mTcpPayloadData = (u_int8_t*)pData;
    mTcpPayloadSize = (u_int32_t)pDataSize;
}

void PacketTcp::SetTcpNumbers(unsigned int pSeuqnceNumber, unsigned int pAcknowledgeNumber)
{
    mSequenceNumber = pSeuqnceNumber;
    mAcknowledgeNumber = pAcknowledgeNumber;
}

void PacketTcp::SetTcpControlFlags(u_int8_t pFlags)
{
    mControlFlags = pFlags;
}

void PacketTcp::SetTcpWindowSize(u_int16_t pSize)
{
    mWindowSize = pSize;
}

void PacketTcp::SetTcpUrgentPointer(u_int16_t pPtr)
{
    mUrgentPointer = pPtr;
}

void PacketTcp::Reset(bool pRecursive)
{
    LOG(LOG_VERBOSE, "Going to reset header data structures");
    mReceivedPacketHeaderTcp = NULL;
    SetTcpSourcePort(0);
    SetTcpDestinationPort(0);
    SetTcpPayload(NULL, 0);
    SetTcpNumbers(0, 0); //TODO: geht das so per default?
    SetTcpControlFlags(0);
    SetTcpWindowSize(32); //TODO: ??
    SetTcpUrgentPointer(0); //TODO: ??
    if (pRecursive)
        PacketIp::Reset();
}

int PacketTcp::Send()
{
    libnet_ptag_t tProtocolTag = 0; //unused till now
    int tResult = -1;

    u_int32_t tSeqNr;
    if (mSequenceNumber != 0)
        tSeqNr = ++mSequenceNumber;
    else
        tSeqNr = libnet_get_prand(LIBNET_PRu32);

    u_int32_t tAckNr;
    if (mAcknowledgeNumber != 0)
        tAckNr = ++mAcknowledgeNumber;
    else
        tAckNr = libnet_get_prand(LIBNET_PRu32);

    u_int16_t tUrgentPtr;
    if (mUrgentPointer != 0)
        tUrgentPtr = mUrgentPointer;
    else
        tUrgentPtr = (u_int16_t)libnet_get_prand(LIBNET_PRu16);

    // ########################################
    // ### debug output
    // ########################################
    LOG(LOG_VERBOSE, "################################### TCP ##################################");
    LOG(LOG_VERBOSE, "Source: %hu", mSourcePort);
    LOG(LOG_VERBOSE, "Dest.: %hu", mDestinationPort);
    LOG(LOG_VERBOSE, "Sequence number: %d", tSeqNr);
    LOG(LOG_VERBOSE, "Acknowledge number: %d", tAckNr);
    LOG(LOG_VERBOSE, "Control flags: 0x%02X", mControlFlags);
    LOG(LOG_VERBOSE, "Window size: %hu", mWindowSize);
    LOG(LOG_VERBOSE, "Urgent pointer: %hu", mUrgentPointer);
    LOG(LOG_VERBOSE, "Payload-data: %p", mTcpPayloadData);
    LOG(LOG_VERBOSE, "Payload-size: %d", mTcpPayloadSize);

    // ########################################
    // ### build header
    // ########################################
    tProtocolTag = libnet_build_tcp(
                                        mSourcePort,
                                        mDestinationPort,
                                        tSeqNr,
                                        tAckNr,
                                        mControlFlags,
                                        mWindowSize,
                                        0, /* checksum is auto. filled by libnet */
                                        mUrgentPointer, /* 0 means libnet will fill this by some random value */
                                        LIBNET_TCP_H + mTcpPayloadSize,
                                        mTcpPayloadData,
                                        mTcpPayloadSize,
                                        Packet::mSendContext,
                                        LIBNET_PTAG_INITIALIZER); // protocol tag to modify an existing header, 0 to build a new one
                                        // returns: protocol tag value on success, -1 on error

    if (tProtocolTag == -1)
    {
        LOG(LOG_ERROR, "libnet_build_tcp failed because of \"%s\"", libnet_geterror(Packet::mSendContext));
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

bool PacketTcp::PrepareReceive(string pPacketFilter, int pTimeoutInMs, bool pReceiveForeignPackets)
{
    pPacketFilter = "tcp";
    return Packet::PrepareReceive(pPacketFilter, pTimeoutInMs, pReceiveForeignPackets);
}

bool PacketTcp::Receive()
{
    bool tResult = PacketIp::Receive();
    if (tResult)
    {
        mReceivedPacketHeaderTcp = (HeaderTcp*)(((char*)mReceivedPacketHeaderIp) + mReceivedPacketHeaderIp->ihl * 4);
        #ifdef DEBUG_PACKETS_TCP
            PrintHeaderTcp(false);
        #endif
    }
    return tResult;
}

void PacketTcp::PrintHeaderTcp(bool pRecursive)
{
    if (pRecursive)
        PrintHeaderIp(pRecursive);
    if ((mReceivedPacketHeaderTcp == NULL) || (mReceivedPacketHeaderIp == NULL))
        return;
        
    printf("################################### TCP ##################################\n");
    printf("Source: %hu\n", ntohs(mReceivedPacketHeaderTcp->sport));
    printf("Dest.: %hu\n", ntohs(mReceivedPacketHeaderTcp->dport));
    printf("Sequence number: %u\n", mReceivedPacketHeaderTcp->seq);
    printf("Acknowledge number: %u\n", mReceivedPacketHeaderTcp->ack);
    printf("Control flags: 0x%02X\n", mReceivedPacketHeaderTcp->ctrl);
    printf("Window size: %hu\n", ntohs(mReceivedPacketHeaderTcp->win));
    printf("Urgent pointer: %hu\n", ntohs(mReceivedPacketHeaderTcp->urp));
    printf("Header length: %hu bytes (%hu bytes header options)\n", (mReceivedPacketHeaderTcp->len >> 4) * 4, (mReceivedPacketHeaderTcp->len >> 4) * 4 - 20);
    printf("Payload length: %hu bytes\n", ntohs(mReceivedPacketHeaderIp->len) - (mReceivedPacketHeaderTcp->len >> 4) * 4 - mReceivedPacketHeaderIp->ihl * 4); // payload = total - tcp header - ip header
}

HeaderTcp *PacketTcp::GetHeaderTcp()
{
    return mReceivedPacketHeaderTcp;
}

///////////////////////////////////////////////////////////////////////////////

} //namespace
