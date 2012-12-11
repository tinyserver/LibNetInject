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
 * Purpose: ICMP ECHO packet handling
 * Author:  Thomas Volkert
 * Since:   2010-10-07
 */

#ifndef PACKET_ICMPECHO_H
#define PACKET_ICMPECHO_H

#include <PacketIp.h>

#include <string>

namespace LibNetInject {

///////////////////////////////////////////////////////////////////////////////

class PacketIcmpEcho:
    public PacketIp
{
public:
    PacketIcmpEcho();
    virtual ~PacketIcmpEcho();

    void SetIcmpEchoPayload(char *pData, unsigned int pDataSize); // TODO: need original IP header + 64 Bit Data
    void SetIcmpEchoSequenceNumber(u_int16_t pNumber);
    void SetIcmpEchoType(bool pReply = false);

    virtual void Reset(bool pRecursive = true);
    virtual int Send();

private:
    u_int8_t* mIcmpEchoPayloadData;
    u_int32_t mIcmpEchoPayloadSize;
    u_int16_t mIcmpEchoSequenceNumber;
    u_int8_t mIcmpEchoType;
};

///////////////////////////////////////////////////////////////////////////////


} // namespace

#endif
