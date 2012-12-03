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

// HINT: we assume Ethernet frames which encapsulate IP packets

#ifndef PACKET_IP_H
#define PACKET_IP_H

#include <Header_Libnet.h>
#include <Packet.h>
#include <PacketEthernet.h>

#include <string>
#include <sys/types.h>

namespace Frogger { namespace Net {

///////////////////////////////////////////////////////////////////////////////
// IPv4 header
struct HeaderIp
{
    u_int8_t  ihl:4;                ///< IHL(IP header length)
    u_int8_t  ver:4;                ///< ip version
    u_int8_t  tos;                  ///< type of service
    u_int16_t len;                  ///< total length
    u_int16_t id;                   ///< identification
    u_int16_t frag_off;             ///< fragment offset field
    u_int8_t  ttl;                  ///< time to live
    u_int8_t  prot;                 ///< protocol (e.g. \a IPPROTO_TCP, \a IPPROTO_UDP, \a IPPROTO_ICMP)
    u_int16_t sum;                  ///< checksum
    struct in_addr src, dst;        ///< source and dest address
};

// de/activate debugging of packets
//#define DEBUG_PACKETS_IP

class PacketIp:
    public Packet
{
public:
    PacketIp();
    virtual ~PacketIp();

    void SetIpSourceAdr(std::string pAddress);
    void SetIpDestinationAdr(std::string pAddress);
    void SetIpTypeofService(u_int8_t pTos);
    void SetIpTimeToLive(uint8_t pTtl);
    void SetIpPayload(char *pData, unsigned int pDataSize);

    /* reset internal data structures to default values for a new packet */
    virtual void Reset(bool pRecursive = true);
    /* send packet */
    virtual int Send();
    /* receive packet */
    virtual bool PrepareReceive(std::string pPacketFilter, int pTimeoutInMs = 0, bool pReceiveForeignPackets = true);
    virtual bool Receive();
    HeaderIp *GetHeaderIp();
    std::string GetSourceIpAdr();
    std::string GetDestinationIpAdr();
    /* header debugging */
    void PrintHeaderIp(bool pRecursive = true);

    std::string GetLayer4ProtocolStr(int pProtocol);
    uint8_t GetLibnetLayer4Protocol();

protected:
    PacketIp(int pPacketType);

    HeaderIp *mReceivedPacketHeaderIp;

private:
    u_int32_t mSourceIp; // source address as network byte ordered IPv4 address
    u_int32_t mDestinationIp; // destination address as network byte ordered IPv4 address
    u_int8_t mTypeOfService; // QoS class
    u_int8_t mTimeToLive; // TTL
    u_int8_t* mIpPayloadData;
    u_int32_t mIpPayloadSize;
};

///////////////////////////////////////////////////////////////////////////////


}} // namespace

#endif /* PACKET_IP_H */
