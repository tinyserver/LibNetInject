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
 * Purpose: ICMP TIME EXCEED packet handling
 * Author:  Thomas Volkert
 * Since:   2010-10-08
 */

#include <PacketIcmpTimeExceed.h>
#include <PacketIp.h>
#include <HBLogger.h>

#include <string>

namespace LibNetInject {

using namespace std;
using namespace Homer::Base;

///////////////////////////////////////////////////////////////////////////////

PacketIcmpTimeExceed::PacketIcmpTimeExceed():
    PacketIp(PACKET_TYPE_ICMP)
{
    Reset(false);
}

PacketIcmpTimeExceed::~PacketIcmpTimeExceed()
{

}

///////////////////////////////////////////////////////////////////////////////

void PacketIcmpTimeExceed::SetIcmpTimeExceedPayload(char *pData, unsigned int pDataSize)
{
    mIcmpTimeExceedPayloadData = (u_int8_t*)pData;
    mIcmpTimeExceedPayloadSize = (u_int32_t)pDataSize;
}

void PacketIcmpTimeExceed::SetIcmpTimeExceedCode(bool pReassemblyFailed)
{
    if (pReassemblyFailed)
        mIcmpTimeExceedCode = ICMP_TIMXCEED_REASS;
    else
        mIcmpTimeExceedCode = ICMP_TIMXCEED_INTRANS;
}

void PacketIcmpTimeExceed::Reset(bool pRecursive)
{
    LOG(LOG_VERBOSE, "Going to reset header data structures");
    SetIcmpTimeExceedPayload(NULL, 0);
    SetIcmpTimeExceedCode(false);
    if (pRecursive)
        PacketIp::Reset();
}

int PacketIcmpTimeExceed::Send()
{
    libnet_ptag_t tProtocolTag = 0; //unused till now
    int tResult = -1;

    // ########################################
    // ### debug output
    // ########################################
    LOG(LOG_VERBOSE, "############################# ICMP TIME EXCEED #############################");
    LOG(LOG_VERBOSE, "TimeExceed-Code: %d", mIcmpTimeExceedCode);
    LOG(LOG_VERBOSE, "Payload-data: %p", mIcmpTimeExceedPayloadData);
    LOG(LOG_VERBOSE, "Payload-size: %d", mIcmpTimeExceedPayloadSize);

    // ########################################
    // ### build header
    // ########################################
    tProtocolTag = libnet_build_icmpv4_timeexceed(
                                        ICMP_TIMXCEED,
                                        mIcmpTimeExceedCode,
                                        0, /* checksum is auto. filled by libnet */
                                        mIcmpTimeExceedPayloadData,
                                        mIcmpTimeExceedPayloadSize,
                                        Packet::mSendContext,
                                        LIBNET_PTAG_INITIALIZER); // protocol tag to modify an existing header, 0 to build a new one
                                        // returns: protocol tag value on success, -1 on error
    if (tProtocolTag == -1)
    {
        LOG(LOG_ERROR, "libnet_build_icmpv4_timeexceed failed because of \"%s\"", libnet_geterror(Packet::mSendContext));
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
