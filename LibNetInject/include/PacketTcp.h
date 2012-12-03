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

#ifndef PACKET_TCP_H
#define PACKET_TCP_H

#include <PacketIp.h>

#include <string>

namespace Frogger { namespace Net {

///////////////////////////////////////////////////////////////////////////////
// TCP header
struct HeaderTcp
{
    u_int16_t sport;    ///< source port
    u_int16_t dport;    ///< destination port
    u_int32_t seq;      ///< sequence number
    u_int32_t ack;      ///< acknowledgment number
    u_int8_t  len;      ///< length of tcp header, some additional reserved bits
    u_int8_t  ctrl;     ///< control bits, 8 1-bit flags
    u_int16_t win;      ///< window
    u_int16_t sum;      ///< checksum
    u_int16_t urp;      ///< urgent pointer
};

// de/activate debugging of packets
//#define DEBUG_PACKETS_TCP

class PacketTcp:
    public PacketIp
{
public:
    PacketTcp();
    virtual ~PacketTcp();

    void SetTcpSourcePort(unsigned short int pPort);
    void SetTcpDestinationPort(unsigned short int pPort);
    void SetTcpPayload(char *pData, unsigned int pDataSize);
    void SetTcpNumbers(unsigned int pSeuqnceNumber, unsigned int pAcknowledgeNumber);
    void SetTcpControlFlags(u_int8_t pFlags);
    void SetTcpWindowSize(u_int16_t pSize);
    void SetTcpUrgentPointer(u_int16_t pPtr);

    /* reset internal data structures to default values for a new packet */
    virtual void Reset(bool pRecursive = true);
    /* send packet */
    virtual int Send();
    /* receive packet */
    virtual bool PrepareReceive(std::string pPacketFilter, int pTimeoutInMs = 0, bool pReceiveForeignPackets = true);
    virtual bool Receive();
    HeaderTcp *GetHeaderTcp();
    /* header debugging */
    void PrintHeaderTcp(bool pRecursive = true);

protected:
    HeaderTcp *mReceivedPacketHeaderTcp;

private:
    uint16_t mSourcePort;
    uint16_t mDestinationPort;
    u_int8_t* mTcpPayloadData;
    u_int32_t mTcpPayloadSize;
    u_int32_t mSequenceNumber;
    u_int32_t mAcknowledgeNumber;
    u_int8_t mControlFlags;
    u_int16_t mWindowSize;
    u_int16_t mUrgentPointer;
};

///////////////////////////////////////////////////////////////////////////////


}} // namespace

#endif /* PACKET_TCP_H */
