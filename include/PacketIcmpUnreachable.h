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
 * Purpose: ICMP UNREACHABLE packet handling
 * Author:  Thomas Volkert
 * Since:   2010-10-08
 */

#ifndef PACKET_ICMPUNREACHABLE_H
#define PACKET_ICMPUNREACHABLE_H

#include <PacketIp.h>

#include <string>

namespace Frogger { namespace Net {

#define     ICMP_UNREACHABLE_CODE_NET                0
#define     ICMP_UNREACHABLE_CODE_HOST               1
#define     ICMP_UNREACHABLE_CODE_PROTOCOL           2
#define     ICMP_UNREACHABLE_CODE_PORT               3
#define     ICMP_UNREACHABLE_CODE_NEEDFRAG           4
#define     ICMP_UNREACHABLE_CODE_SRCFAIL            5
#define     ICMP_UNREACHABLE_CODE_NET_UNKNOWN        6
#define     ICMP_UNREACHABLE_CODE_HOST_UNKNOWN       7
#define     ICMP_UNREACHABLE_CODE_ISOLATED           8
#define     ICMP_UNREACHABLE_CODE_NET_PROHIB         9
#define     ICMP_UNREACHABLE_CODE_HOST_PROHIB        10
#define     ICMP_UNREACHABLE_CODE_TOSNET             11
#define     ICMP_UNREACHABLE_CODE_TOSHOST            12
#define     ICMP_UNREACHABLE_CODE_FILTER_PROHIB      13
#define     ICMP_UNREACHABLE_CODE_HOST_PRECEDENCE    14
#define     ICMP_UNREACHABLE_CODE_PRECEDENCE_CUTOFF  15

///////////////////////////////////////////////////////////////////////////////

class PacketIcmpUnreachable:
    public PacketIp
{
public:
    PacketIcmpUnreachable();
    virtual ~PacketIcmpUnreachable();

    void SetIcmpUnreachablePayload(char *pData, unsigned int pDataSize); // TODO: need original IP header + 64 Bit Data
    void SetIcmpUnreachableCode(int pCode = ICMP_UNREACHABLE_CODE_HOST);

    virtual void Reset(bool pRecursive = true);
    virtual int Send();

private:
    u_int8_t* mIcmpUnreachablePayloadData;
    u_int32_t mIcmpUnreachablePayloadSize;
    u_int8_t mIcmpUnreachableCode;
};

///////////////////////////////////////////////////////////////////////////////


}} // namespace

#endif /* PACKET_ICMPUNREACHABLE_H */
