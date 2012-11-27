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

#ifndef PACKET_UDP_H
#define PACKET_UDP_H

#include <PacketIp.h>

#include <string>

namespace Frogger { namespace Net {

///////////////////////////////////////////////////////////////////////////////
// UDP header
struct HeaderUdp
{
    u_int16_t sport;    ///< source port
    u_int16_t dport;    ///< destination port
    u_int16_t len;      ///< length of the entire datagram (in bytes)
    u_int16_t sum;      ///< checksum
};

// de/activate debugging of packets
//#define DEBUG_PACKETS_UDP

class PacketUdp:
    public PacketIp
{
public:
    PacketUdp();
    virtual ~PacketUdp();

    void SetUdpSourcePort(unsigned short int pPort);
    void SetUdpDestinationPort(unsigned short int pPort);
    void SetUdpPayload(char *pData, unsigned int pDataSize);

    /* reset internal data structures to default values for a new packet */
    virtual void Reset(bool pRecursive = true);
    /* send packet */
    virtual int Send();
    /* receive packet */
    virtual bool PrepareReceive(std::string pPacketFilter, int pTimeoutInMs = 0, bool pReceiveForeignPackets = true);
    virtual bool Receive();
    HeaderUdp *GetHeaderUdp();
    /* header debugging */
    void PrintHeaderUdp(bool pRecursive = true);

protected:
    HeaderUdp *mReceivedPacketHeaderUdp;

private:
    uint16_t mSourcePort;
    uint16_t mDestinationPort;
    u_int8_t* mUdpPayloadData;
    u_int32_t mUdpPayloadSize;
};

///////////////////////////////////////////////////////////////////////////////


}} // namespace

#endif /* PACKET_UDP_H */
