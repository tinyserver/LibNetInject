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

#include <Header_Libnet.h>
#include <Header_Libpcap.h>
#include <Packet.h>
#include <HBLogger.h>

#include <string>
#include <errno.h>

namespace Frogger { namespace Net {

using namespace std;
using namespace Homer::Base;

///////////////////////////////////////////////////////////////////////////////

Packet::Packet(long int pPacketType)
{
    LOG(LOG_VERBOSE, "Send device: %s", mSendDevice.c_str());
    LOG(LOG_VERBOSE, "Receive device: %s", mReceiveDevice.c_str());

    mSendDevice = GetDefaultDevice();
    mReceiveDevice = GetDefaultDevice();
    mReceiveContext = NULL;
    mSendContext = NULL;
    mReceiveIsPrepared = false;
    mReceivedPacketSize = 0;
    mReceivedPacketData = NULL;
    mPacketType = pPacketType;

    ResetSendContext();
    ResetReceiveContext();
    Reset();
}

Packet::~Packet()
{
    if (mSendContext != NULL)
        libnet_destroy(mSendContext);
    if (mReceiveContext != NULL)
        pcap_close(mReceiveContext);

}

///////////////////////////////////////////////////////////////////////////////

void Packet::ResetReceiveContext()
{
    LOG(LOG_VERBOSE, "Resetting receive context");
    if (mReceiveContext != NULL)
    {
        pcap_close(mReceiveContext);
    }

    //################################################################
    //### Initialization for packet receiving
    //################################################################
    mReceiveContext = pcap_create(mReceiveDevice.c_str(), mPcapErrorBuffer);
    if (mReceiveContext == NULL)
        LOG(LOG_ERROR, "pcap_create() failed because of: \"%s\"", mPcapErrorBuffer);
}

void Packet::ResetSendContext()
{
    LOG(LOG_VERBOSE, "Resetting send context");
    if (mSendContext != NULL)
    {
        libnet_destroy(mSendContext);
    }

    //################################################################
    //### Initialization for packet sending
    //################################################################
    // is there a layer 3 protocol?
    if ((GetType() & PACKET_LAYER3_MASK) == PACKET_TYPE_IP)
    {// bind to raw socket on layer 3
        LOG(LOG_VERBOSE, "Binding to network layer (3)");
        mSendContext = libnet_init(LIBNET_RAW4_ADV, (char*)mSendDevice.c_str(), mLibnetErrorBuffer);
    }else
    {// bind to link socket on layer 2
        LOG(LOG_VERBOSE, "Binding to link layer (2)");
        mSendContext = libnet_init(LIBNET_LINK_ADV, (char*)mSendDevice.c_str(), mLibnetErrorBuffer);
    }

    if (mSendContext == NULL)
        LOG(LOG_ERROR, "libnet_init failed");

    if (libnet_seed_prand(mSendContext) == -1)
        LOG(LOG_ERROR, "libnet_seed_prand failed because of \"%s\"", libnet_geterror(mSendContext));
}

string Packet::GetDefaultDevice()
{
    string tResult = "";

    char *tDevice = pcap_lookupdev(mReceiveErrorBuffer);
    if (tDevice != NULL)
        tResult = string(tDevice);

    return tResult;
}

void Packet::SetSendDevice(std::string pDevice)
{
    if(mSendDevice != pDevice)
    {
        mSendDevice = pDevice;
        ResetSendContext();
    }
}

string Packet::GetSendDevice()
{
    return mSendDevice;
}

void Packet::SetReceiveDevice(std::string pDevice)
{
    if(mReceiveDevice != pDevice)
    {
        mReceiveDevice = pDevice;
        ResetReceiveContext();
    }
}

string Packet::GetReceiveDevice()
{
    return mReceiveDevice;
}

///////////////////////////////////////////////////////////////////////////////

long int Packet::GetType()
{
    return mPacketType;
}

void Packet::Reset(bool pRecursive)
{
    LOG(LOG_VERBOSE, "Going to reset header data structures");
    libnet_clear_packet(mSendContext);
}

int Packet::Send()
{
    int tWritten = -1;
    int tResult = -1;

    LOG(LOG_VERBOSE, "Going to send final packet");
    tResult = ((tWritten = libnet_write(mSendContext)) != -1) ? 0 : errno;
    if (tResult != 0)
        LOG(LOG_ERROR, "libnet_write failed because of \"%s\"", libnet_geterror(mSendContext));
    else
        LOG(LOG_VERBOSE, "..%d bytes sent", tWritten);

    return tResult;
}

bool Packet::PrepareReceive(string pPacketFilter, int pTimeoutInMs, bool pReceiveForeignPackets)
{
    bpf_u_int32 tReceiveDeviceIpNetAdr;
    bpf_u_int32 tReceiveDeviceIpNetMask;
    int tRes = 0;

    if (pcap_set_snaplen(mReceiveContext, 64*1024) < 0)
    {
        LOG(LOG_ERROR, "pcap_set_snaplen failed because of \"%s\"", pcap_geterr(mReceiveContext));
        return false;
    }

    if (pcap_set_timeout(mReceiveContext, pTimeoutInMs) < 0)
    {
        LOG(LOG_ERROR, "pcap_set_timeout failed because of \"%s\"", pcap_geterr(mReceiveContext));
        return false;
    }

    if (pcap_set_promisc(mReceiveContext, pReceiveForeignPackets) < 0)
    {
        LOG(LOG_ERROR, "pcap_set_promisc failed because of \"%s\"", pcap_geterr(mReceiveContext));
        return false;
    }

    if ((tRes = pcap_activate(mReceiveContext)) < 0)
    {
        LOG(LOG_ERROR, "pcap_activate failed because of \"%s\"(%d)", pcap_geterr(mReceiveContext), tRes);
        return false;
    }

    if ((tRes = pcap_lookupnet(mReceiveDevice.c_str(), &tReceiveDeviceIpNetAdr, &tReceiveDeviceIpNetMask, mPcapErrorBuffer)) < 0)
    {
        LOG(LOG_ERROR, "pcap_lookupnet failed because of \"%s\"(%d)", pcap_geterr(mReceiveContext), tRes);
        return false;
    }

//    if ((tRes = pcap_datalink(mReceiveContext)) != DLT_EN10MB)
//    {
//        LOG(LOG_ERROR, "pcap_datalink delivered incompatible data link layer type of %s(%d)", pcap_datalink_val_to_name(tRes), tRes);
//        return false;
//    }

    if ((tRes = pcap_compile(mReceiveContext, &mReceiveCompiledFilter, pPacketFilter.c_str(), 0, tReceiveDeviceIpNetAdr)) < 0)
    {
        LOG(LOG_ERROR, "pcap_compile failed because of \"%s\"(%d)", pcap_geterr(mReceiveContext), tRes);
        return false;
    }
    if ((tRes = pcap_setfilter(mReceiveContext, &mReceiveCompiledFilter)) < 0)
    {
        LOG(LOG_ERROR, "pcap_setfilter failed because of \"%s\"(%d)", pcap_geterr(mReceiveContext), tRes);
        return false;
    }
    mReceiveIsPrepared = true;
    return true;
}

bool Packet::Receive()
{
    bpf_u_int32 tPacketSize = 0;
    struct tm *tCapTime;
    int tRes = 0;

    if (!mReceiveIsPrepared)
    {
        LOG(LOG_ERROR, "Packet receiving is not prepared");
        return false;
    }

    if (mReceivedPacketData != NULL)
    {
        LOG(LOG_VERBOSE, "Reseting former packet memory from last receive call");
        mReceivedPacketData = NULL;
        mReceivedPacketSize = 0;
    }

    LOG(LOG_VERBOSE, "Going to receive a packet");
    //##############################################################
    /* pcap_next_ex
             result can be:
                 >1: okay
                 0: timeout
                -1: error
                -2: loop was broken because of call to pcap_breakloop()
     */
    if ((tRes = pcap_next_ex(mReceiveContext, &mReceivedPacketDesc, (const u_char**)&mReceivedPacketData)) < 1)
    {
        switch(tRes)
        {
            case 0:
                LOG(LOG_VERBOSE, "pcap_next_ex ended in timeout");
                break;
            default:
            case -1:
                LOG(LOG_ERROR, "pcap_next_ex failed because of \"%s\"", pcap_geterr(mReceiveContext));
                break;
            case -2:
                LOG(LOG_VERBOSE, "pcap_next_ex was broken out off because of a call to pcap_breakloop");
                break;
        }
        return false;
    }
    mReceivedPacketSize = mReceivedPacketDesc->caplen;
    tCapTime = localtime(&mReceivedPacketDesc->ts.tv_sec);
    LOG(LOG_VERBOSE, "..%lu bytes received at location %p and at time %02dh:%02dm.%02ds,%06ldus", mReceivedPacketSize, mReceivedPacketData, tCapTime->tm_hour, tCapTime->tm_min, tCapTime->tm_sec, mReceivedPacketDesc->ts.tv_usec);
    return true;
}

void Packet::GetRawPacket(char *pData, unsigned int &pDataSize)
{
    memcpy(pData, mReceivedPacketData, mReceivedPacketSize);
    pDataSize = mReceivedPacketSize;
}

}} //namespace
