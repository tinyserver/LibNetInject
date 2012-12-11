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
 * Since:   2010-10-06
 */

#include <PacketIcmpEcho.h>
#include <PacketIp.h>
#include <HBLogger.h>

#include <string>

namespace LibNetInject {

using namespace std;
using namespace Homer::Base;

///////////////////////////////////////////////////////////////////////////////

PacketIcmpEcho::PacketIcmpEcho():
    PacketIp(PACKET_TYPE_ICMP)
{
    Reset(false);
}

PacketIcmpEcho::~PacketIcmpEcho()
{

}

///////////////////////////////////////////////////////////////////////////////

void PacketIcmpEcho::SetIcmpEchoPayload(char *pData, unsigned int pDataSize)
{
    mIcmpEchoPayloadData = (u_int8_t*)pData;
    mIcmpEchoPayloadSize = (u_int32_t)pDataSize;
}

void PacketIcmpEcho::SetIcmpEchoSequenceNumber(u_int16_t pNumber)
{
    mIcmpEchoSequenceNumber = pNumber;
}

void PacketIcmpEcho::SetIcmpEchoType(bool pReply)
{
    if (pReply)
        mIcmpEchoType = ICMP_ECHOREPLY;
    else
        mIcmpEchoType = ICMP_ECHO;
}

void PacketIcmpEcho::Reset(bool pRecursive)
{
    LOG(LOG_VERBOSE, "Going to reset header data structures");
    SetIcmpEchoPayload(NULL, 0);
    SetIcmpEchoSequenceNumber(0);
    SetIcmpEchoType(false);
    if (pRecursive)
        PacketIp::Reset();
}

int PacketIcmpEcho::Send()
{
    libnet_ptag_t tProtocolTag = 0; //unused till now
    u_int16_t tIcmpId = libnet_get_prand(LIBNET_PRu16);
    int tResult = -1;

    u_int16_t tSeqNr;
    if (mIcmpEchoSequenceNumber != 0)
        tSeqNr = ++mIcmpEchoSequenceNumber;
    else
        tSeqNr = (u_int16_t)libnet_get_prand(LIBNET_PRu16);

    // ########################################
    // ### debug output
    // ########################################
    LOG(LOG_VERBOSE, "################################# ICMP ECHO ################################");
    LOG(LOG_VERBOSE, "Echo-Type: %d", mIcmpEchoType);
    LOG(LOG_VERBOSE, "Id number: %hu", tIcmpId);
    LOG(LOG_VERBOSE, "Sequence number: %hu", tSeqNr);
    LOG(LOG_VERBOSE, "Payload-data: %p", mIcmpEchoPayloadData);
    LOG(LOG_VERBOSE, "Payload-size: %d", mIcmpEchoPayloadSize);

    // ########################################
    // ### build header
    // ########################################
    tProtocolTag = libnet_build_icmpv4_echo(
                                        mIcmpEchoType,
                                        0, /* code of ICMP packet (should be 0) */
                                        0, /* checksum is auto. filled by libnet */
                                        tIcmpId,
                                        tSeqNr,
                                        mIcmpEchoPayloadData,
                                        mIcmpEchoPayloadSize,
                                        mSendContext,
                                        LIBNET_PTAG_INITIALIZER); // protocol tag to modify an existing header, 0 to build a new one
                                        // returns: protocol tag value on success, -1 on error
    if (tProtocolTag == -1)
    {
        LOG(LOG_ERROR, "libnet_build_icmpv4_echo failed because of \"%s\"", libnet_geterror(mSendContext));
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

///////////////////////////////////////////////////////////////////////////////

} //namespace
