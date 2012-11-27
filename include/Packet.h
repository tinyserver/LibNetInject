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
 * Purpose: general packet handling
 * Author:  Thomas Volkert
 * Since:   2010-10-06
 */

#ifndef PACKET_H
#define PACKET_H

#include <Header_Libnet.h>
#include <Header_Libpcap.h>
#include <string>

#define ETHERTYPE_FOG                     0x606 // self defined ether type for FoG
#define ETHERTYPE_FOG_STR                "0x606"

#define PACKET_TYPE_ANY              0xFFFFFFFFL
#define PACKET_TYPE_INVALID          0x00000000L
#define PACKET_LAYER2_MASK           0x000000FFL
#define PACKET_LAYER3_MASK           0x0000FF00L
#define PACKET_LAYER4_MASK           0x00FF0000L
#define PACKET_LAYER5_MASK           0xFF000000L

#define PACKET_TYPE_ETH              0x00000001L
#define PACKET_TYPE_IP               0x00000100L
#define PACKET_TYPE_FOG              0x00000200L
#define PACKET_TYPE_UDP             (0x00010000L | PACKET_TYPE_IP)
#define PACKET_TYPE_TCP             (0x00020000L | PACKET_TYPE_IP)
#define PACKET_TYPE_ICMP            (0x00030000L | PACKET_TYPE_IP)

namespace Frogger { namespace Net {

///////////////////////////////////////////////////////////////////////////////

class Packet
{

public:
    Packet(long int pPacketType = PACKET_TYPE_ANY);

    virtual ~Packet();

    /* handling of global setting of send and receive device */
    void SetSendDevice(std::string pDevice);
    std::string GetSendDevice();
    void SetReceiveDevice(std::string pDevice);
    std::string GetReceiveDevice();
    /* get packet type */
    long int GetType();
    /* reset internal data structures to default values for a new packet */
    virtual void Reset(bool pRecursive = true);
    /* send packet */
    virtual int Send();
    /* receive packet */
    virtual bool PrepareReceive(std::string pPacketFilter, int pTimeoutInMs = 0, bool pReceiveForeignPackets = true);
    virtual bool Receive();
    void GetRawPacket(char *pData, unsigned int &pDataSize);
    std::string GetDefaultDevice();

protected:
    libnet_t            *mSendContext;
    pcap_t              *mReceiveContext;
    struct bpf_program  mReceiveCompiledFilter;
    char                *mReceivedPacketData;
    unsigned int        mReceivedPacketSize;
    struct pcap_pkthdr  *mReceivedPacketDesc;
    bool                mReceiveIsPrepared;
    char                mPcapErrorBuffer[PCAP_ERRBUF_SIZE];
    char                mLibnetErrorBuffer[LIBNET_ERRBUF_SIZE];

private:
    void ResetReceiveContext();
    void ResetSendContext();

    char mReceiveErrorBuffer[PCAP_ERRBUF_SIZE];
    std::string mSendDevice;
    std::string mReceiveDevice;
    long int mPacketType;
};

///////////////////////////////////////////////////////////////////////////////

}} // namespace

#endif
