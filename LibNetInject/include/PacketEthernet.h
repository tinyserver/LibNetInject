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
 * Purpose: Ethernet packet handling
 * Author:  Thomas Volkert
 * Since:   2010-10-08
 */

#ifndef PACKET_ETH_H
#define PACKET_ETH_H

#include <Packet.h>

#include <string>

namespace Frogger { namespace Net {

///////////////////////////////////////////////////////////////////////////////
#define HEADER_SIZE_ETHERNET    (sizeof(HeaderEthernet))
#define PAYLOAD_LIMIT_ETHERNET  1500
#define ADDRESS_SIZE_ETHERNET   6

struct HeaderEthernet
{
    u_int8_t  Destination[6];       // destination host address
    u_int8_t  Source[6];            // source host address
    u_int16_t Type;                 // IP, ARP, RARP etc
};

class PacketEthernet:
    public Packet
{
public:
    PacketEthernet();
    virtual ~PacketEthernet();

    /* sending */
    void SetEthernetSourceAdr(std::string pAddress);
    void SetEthernetDestinationAdr(std::string pAddress);
    void SetEthernetPayload(char *pData, unsigned int pDataSize);
    virtual void Reset(bool pRecursive = true); /* reset internal data structures to default values for a new packet */
    virtual int Send();/* send packet */

    /* receiving */
    bool GetEthernetSourceAdr(char *pData, unsigned int &pDataSize);
    bool GetEthernetDestinationAdr(char *pData, unsigned int &pDataSize);
    bool GetEthernetPayload(char *pData, unsigned int &pDataSize);

    /* header debugging */
    void PrintHeaderEthernet(HeaderEthernet* pHeader = NULL);
    u_int16_t GetLibnetLayer3Protocol();

protected:
    PacketEthernet(int pPacketType);

    HeaderEthernet*    mReceivedEthernetHeader;
private:
    u_int8_t *mSourceMac;
    u_int8_t *mDestinationMac;
    u_int8_t  mEthernetPayloadData[PAYLOAD_LIMIT_ETHERNET];
    u_int32_t mEthernetPayloadSize;
    std::string mDestinationMacStr;
    std::string mSourceMacStr;
};

///////////////////////////////////////////////////////////////////////////////


}} // namespace

#endif
