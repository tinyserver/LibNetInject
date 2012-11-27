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
 * Purpose: IP packet handling
 * Author:  Thomas Volkert
 * Since:   2010-10-06
 */

#include <Packet.h>
#include <PacketIp.h>
#include <PacketEthernet.h>
#include <Header_Libnet.h>
#include <HBLogger.h>

#include <string>

namespace Frogger { namespace Net {

using namespace std;
using namespace Homer::Base;

///////////////////////////////////////////////////////////////////////////////

PacketIp::PacketIp():
    Packet(PACKET_TYPE_IP)
{
    Reset(false);
}

PacketIp::PacketIp(int pPacketType):
    Packet(pPacketType)
{
    Reset(false);
}

PacketIp::~PacketIp()
{

}

///////////////////////////////////////////////////////////////////////////////

void PacketIp::SetIpSourceAdr(std::string pAddress)
{
    mSourceIp = libnet_name2addr4(mSendContext, (char*)pAddress.c_str(), LIBNET_RESOLVE);
}

void PacketIp::SetIpDestinationAdr(std::string pAddress)
{
    mDestinationIp = libnet_name2addr4(mSendContext, (char*)pAddress.c_str(), LIBNET_RESOLVE);
}

string PacketIp::GetSourceIpAdr()
{
    return libnet_addr2name4(mSourceIp, LIBNET_DONT_RESOLVE);
}

string PacketIp::GetDestinationIpAdr()
{
    return libnet_addr2name4(mDestinationIp, LIBNET_DONT_RESOLVE);
}

void PacketIp::SetIpTypeofService(u_int8_t pTos)
{
    mTypeOfService = pTos;
}

void PacketIp::SetIpTimeToLive(uint8_t pTtl)
{
    mTimeToLive = pTtl;
}

void PacketIp::SetIpPayload(char *pData, unsigned int pDataSize)
{
    mIpPayloadData = (u_int8_t*)pData;
    mIpPayloadSize = (u_int32_t)pDataSize;
}

void PacketIp::Reset(bool pRecursive)
{
    LOG(LOG_VERBOSE, "Going to reset header data structures");
    mReceivedPacketHeaderIp = NULL;
    SetIpSourceAdr("127.0.0.1");
    SetIpDestinationAdr("127.0.0.1");
    SetIpTypeofService(0);
    SetIpTimeToLive(64); // fitting in most cases, in former times a value of 32 was used
    SetIpPayload(NULL, 0);
    if (pRecursive)
        Packet::Reset();
}

int PacketIp::Send()
{
    libnet_ptag_t tProtocolTag = 0; //unused till now
    u_int16_t tIpId = libnet_get_prand(LIBNET_PRu16);
    int tResult = -1;

    uint8_t *tPayloadData = NULL;
    uint32_t tPayloadSize = 0;

    // ########################################
    // ### debug output
    // ########################################
    LOG(LOG_VERBOSE, "################################### IP ##################################");
    LOG(LOG_VERBOSE, "Source: %s", GetSourceIpAdr().c_str());
    LOG(LOG_VERBOSE, "Dest.: %s", GetDestinationIpAdr().c_str());
    LOG(LOG_VERBOSE, "TOS field: %hu", mTypeOfService);
    LOG(LOG_VERBOSE, "Id number: %d", tIpId);
    LOG(LOG_VERBOSE, "TTL: %hu", mTimeToLive);
    LOG(LOG_VERBOSE, "Layer 4 protocol: %d", GetLibnetLayer4Protocol());
    // if there is a layer 4 protocol we have to ignore the IP payload
    if (GetLibnetLayer4Protocol() != IPPROTO_IP)
    {
        LOG(LOG_VERBOSE, "Payload-data: ignored but set to %p", mIpPayloadData);
        LOG(LOG_VERBOSE, "Payload-size: ignored but set to %d", mIpPayloadSize);
    }else
    {
        LOG(LOG_VERBOSE, "Payload-data: %p", mIpPayloadData);
        LOG(LOG_VERBOSE, "Payload-size: %d", mIpPayloadSize);
        tPayloadData = mIpPayloadData;
        tPayloadSize = mIpPayloadSize;
    }

    // ########################################
    // ### build header
    // ########################################
    tProtocolTag = libnet_build_ipv4(
                                        LIBNET_IPV4_H + mIpPayloadSize, // total length of the IP packet including all subsequent data
                                        mTypeOfService,
                                        tIpId,
                                        0, /*TODO: fragmentation bits and offset */
                                        mTimeToLive,
                                        GetLibnetLayer4Protocol(),
                                        0, /* checksum is auto. filled by libnet */
                                        mSourceIp,
                                        mDestinationIp,
                                        tPayloadData,
                                        tPayloadSize,
                                        mSendContext,
                                        LIBNET_PTAG_INITIALIZER); // protocol tag to modify an existing header, 0 to build a new one
                                        // returns: protocol tag value on success, -1 on error
    if (tProtocolTag == -1)
    {
        LOG(LOG_ERROR, "libnet_build_ipv4 failed because of \"%s\"", libnet_geterror(mSendContext));
    	tResult = errno;
    }else
    {
		// ########################################
		// ### call next layer
		// ########################################
		tResult = Packet::Send();
    }

    return tResult;
}

uint8_t PacketIp::GetLibnetLayer4Protocol()
{
    uint8_t tResult = IPPROTO_RAW;
    long int tProt = GetType() & (PACKET_LAYER4_MASK | PACKET_LAYER3_MASK);

    switch(tProt)
    {
        case PACKET_TYPE_UDP:
                tResult = IPPROTO_UDP;
                break;
        case PACKET_TYPE_TCP:
                tResult = IPPROTO_TCP;
                break;
        case PACKET_TYPE_ICMP:
                tResult = IPPROTO_ICMP;
                break;
        case PACKET_TYPE_IP:
                break;
        default:
                LOG(LOG_ERROR, "Unsupported layer 4 protocol %ld found", tProt);
                break;
    }
    return tResult;
}

string PacketIp::GetLayer4ProtocolStr(int pProtocol)
{
    string tResult = "";

    switch(pProtocol)
    {
        case IPPROTO_UDP:
                tResult = "udp";
                break;
        case IPPROTO_TCP:
                tResult = "tcp";
                break;
        case IPPROTO_ICMP:
                tResult = "icmp";
                break;
        default:
                tResult = "unsupported id " + toString(pProtocol);
                break;
    }
    return tResult;
}

bool PacketIp::PrepareReceive(string pPacketFilter, int pTimeoutInMs, bool pReceiveForeignPackets)
{
    pPacketFilter = "ip";
    return Packet::PrepareReceive(pPacketFilter, pTimeoutInMs, pReceiveForeignPackets);
}

bool PacketIp::Receive()
{
    bool tResult = Packet::Receive();
    if (tResult)
    {
        mReceivedPacketHeaderIp = (HeaderIp*)(mReceivedPacketData + HEADER_SIZE_ETHERNET);
        #ifdef DEBUG_PACKETS_IP
            PrintHeaderIp(false);
        #endif
    }
    return tResult;
}

void PacketIp::PrintHeaderIp(bool pRecursive)
{
    PacketEthernet tEthPacket;

    if (pRecursive)
        tEthPacket.PrintHeaderEthernet((HeaderEthernet*)mReceivedPacketData);
    if (mReceivedPacketHeaderIp == NULL)
        return;
        
    printf("################################### IP ##################################\n");
    printf("Source: %s\n", inet_ntoa(mReceivedPacketHeaderIp->src));
    printf("Dest.: %s\n", inet_ntoa(mReceivedPacketHeaderIp->dst));
    printf("TOS field: %hu\n", mReceivedPacketHeaderIp->tos);
    printf("Id number: %hu\n", ntohs(mReceivedPacketHeaderIp->id));
    printf("TTL: %hu\n", mReceivedPacketHeaderIp->ttl);
    printf("Layer 4 protocol: %s\n", GetLayer4ProtocolStr(mReceivedPacketHeaderIp->prot).c_str());
    printf("Header length: %hu bytes\n", mReceivedPacketHeaderIp->ihl * 4);
    printf("Total length of IP-packet: %hu bytes\n", ntohs(mReceivedPacketHeaderIp->len));
    printf("IP version: %hu\n", mReceivedPacketHeaderIp->ver);
}

HeaderIp *PacketIp::GetHeaderIp()
{
    return mReceivedPacketHeaderIp;
}

///////////////////////////////////////////////////////////////////////////////

}} //namespace
