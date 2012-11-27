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

#include <Packet.h>
#include <PacketEthernet.h>
#include <Header_Libnet.h>
#include <HBLogger.h>

#include <string>

namespace Frogger { namespace Net {

using namespace std;
using namespace Homer::Base;

///////////////////////////////////////////////////////////////////////////////

PacketEthernet::PacketEthernet():
    Packet(PACKET_TYPE_ETH)
{
    mSourceMac = NULL;
    mDestinationMac = NULL;
    memset((void*)mEthernetPayloadData, 0, sizeof(mEthernetPayloadData));
    mEthernetPayloadSize = 0;
    Reset(false);
}

PacketEthernet::PacketEthernet(int pPacketType):
    Packet(pPacketType)
{
    mSourceMac = NULL;
    mDestinationMac = NULL;
    memset((void*)mEthernetPayloadData, 0, sizeof(mEthernetPayloadData));
    mEthernetPayloadSize = 0;
    Reset(false);
}

PacketEthernet::~PacketEthernet()
{

}

///////////////////////////////////////////////////////////////////////////////

void PacketEthernet::SetEthernetSourceAdr(std::string pAddress)
{
	int tResultSize = 0;
    if (mSourceMac != NULL)
        free(mSourceMac);
    mSourceMacStr = pAddress;
    mSourceMac = libnet_hex_aton(mSourceMacStr.c_str(), &tResultSize);
}

void PacketEthernet::SetEthernetDestinationAdr(std::string pAddress)
{
    int tResultSize = 0;
    if (mDestinationMac != NULL)
        free(mDestinationMac);
    mDestinationMacStr = pAddress;
    mDestinationMac = libnet_hex_aton(mDestinationMacStr.c_str(), &tResultSize);
}

void PacketEthernet::SetEthernetPayload(char *pData, unsigned int pDataSize)
{
    if(pDataSize > PAYLOAD_LIMIT_ETHERNET)
    {
        LOG(LOG_ERROR, "Payload of size %d for Ethernet packets too big, limit is %d", pDataSize, PAYLOAD_LIMIT_ETHERNET);
        return;
    }
    if (pDataSize > 0)
        memcpy((void*)mEthernetPayloadData, pData, pDataSize);
    mEthernetPayloadSize = (u_int32_t)pDataSize;
}

void PacketEthernet::Reset(bool pRecursive)
{
    LOG(LOG_VERBOSE, "Going to reset header data structures");
    SetEthernetSourceAdr("00:00:00:00:00:00");
    SetEthernetDestinationAdr("00:00:00:00:00:00");
    SetEthernetPayload(NULL, 0);
    if (pRecursive)
        Packet::Reset();
}

int PacketEthernet::Send()
{
    libnet_ptag_t tProtocolTag = 0; //unused till now
    int tResult = -1;

    uint8_t *tPayloadData = NULL;
    uint32_t tPayloadSize = 0;

    // ########################################
    // ### debug output
    // ########################################
    LOG(LOG_INFO, "############################## sending ETHERNET ##############################");
    LOG(LOG_INFO, "Dest.: %s", mDestinationMacStr.c_str());
    LOG(LOG_INFO, "Source: %s", mSourceMacStr.c_str());
    LOG(LOG_INFO, "Ether type: 0x%X", GetLibnetLayer3Protocol());
    LOG(LOG_INFO, "Payload-data: %p", mEthernetPayloadData);
    LOG(LOG_INFO, "Payload-size: %d", mEthernetPayloadSize);
    tPayloadData = mEthernetPayloadData;
    tPayloadSize = mEthernetPayloadSize;

    // ########################################
    // ### build header
    // ########################################
    tProtocolTag = libnet_build_ethernet(
                                        mDestinationMac,
                                        mSourceMac,
                                        GetLibnetLayer3Protocol(),
                                        tPayloadData,
                                        tPayloadSize,
                                        mSendContext,
                                        LIBNET_PTAG_INITIALIZER); // protocol tag to modify an existing header, 0 to build a new one
                                        // returns: protocol tag value on success, -1 on error
    if (tProtocolTag == -1)
    {
        LOG(LOG_ERROR, "libnet_build_ethernet failed because of \"%s\"", libnet_geterror(mSendContext));
    	tResult = errno;
    }else
    {
        // ########################################
        // ### call next layer
        // ########################################
       	tResult = Packet::Send();
    }

    return tResult;
}

u_int16_t PacketEthernet::GetLibnetLayer3Protocol()
{
    u_int16_t tResult = 0;
    long int tProt = GetType() & (PACKET_LAYER3_MASK);

    switch(tProt)
    {
        case PACKET_TYPE_IP:
                tResult = ETHERTYPE_IP;
                break;
        case PACKET_TYPE_FOG:
                tResult = ETHERTYPE_FOG;
                break;
        case 0:
                tResult = 0;
                break;
        default:
                LOG(LOG_ERROR, "Unsupported layer 3 protocol %ld found", tProt);
                break;
    }
    return tResult;
}

void PacketEthernet::PrintHeaderEthernet(HeaderEthernet* pHeader)
{
    if (pHeader == NULL)
    {
        pHeader = (HeaderEthernet*)mReceivedPacketData;
    }

    LOG(LOG_INFO, "################################ received ETHERNET ###############################");
    LOG(LOG_INFO, "Dest. : %02X:%02X:%02X:%02X:%02X:%02X", pHeader->Destination[0], pHeader->Destination[1], pHeader->Destination[2], pHeader->Destination[3], pHeader->Destination[4], pHeader->Destination[5]);
    LOG(LOG_INFO, "Source: %02X:%02X:%02X:%02X:%02X:%02X", pHeader->Source[0], pHeader->Source[1], pHeader->Source[2], pHeader->Source[3], pHeader->Source[4], pHeader->Source[5]);
    switch(ntohs(pHeader->Type))
    {
        case ETHERTYPE_FOG:
            LOG(LOG_INFO, "Layer 3 protocol: FoG (0x%04X)", ntohs(pHeader->Type));
            break;
        default:
            LOG(LOG_INFO, "Layer 3 protocol: 0x%04X", ntohs(pHeader->Type));
            break;
    }
}

bool PacketEthernet::GetEthernetSourceAdr(char *pData, unsigned int &pDataSize)
{
    if ((pData == NULL) || (mReceivedPacketData == NULL) || (ADDRESS_SIZE_ETHERNET > pDataSize))
    {
        LOG(LOG_ERROR, "Parameters of GetEthernetSourceAdr invalid");
        return false;
    }

    memcpy(pData, (const void*)(mReceivedPacketData + ADDRESS_SIZE_ETHERNET), ADDRESS_SIZE_ETHERNET);
    pDataSize = (unsigned int)ADDRESS_SIZE_ETHERNET;

    return true;
}

bool PacketEthernet::GetEthernetDestinationAdr(char *pData, unsigned int &pDataSize)
{
    if ((pData == NULL) || (mReceivedPacketData == NULL) || (ADDRESS_SIZE_ETHERNET > pDataSize))
    {
        LOG(LOG_ERROR, "Parameters of GetEthernetDestinationAdr invalid");
        return false;
    }

    memcpy(pData, (const void*)(mReceivedPacketData), ADDRESS_SIZE_ETHERNET);
    pDataSize = (unsigned int)ADDRESS_SIZE_ETHERNET;

    return true;
}

bool PacketEthernet::GetEthernetPayload(char *pData, unsigned int &pDataSize)
{
    if ((pData == NULL) || (mReceivedPacketData == NULL) || (mReceivedPacketSize - HEADER_SIZE_ETHERNET > pDataSize))
    {
        LOG(LOG_ERROR, "Parameters of GetEthernetPayload invalid");
        return false;
    }

    memcpy(pData, (const void*)(mReceivedPacketData + HEADER_SIZE_ETHERNET), mReceivedPacketSize - HEADER_SIZE_ETHERNET);
    pDataSize = (unsigned int)mReceivedPacketSize - HEADER_SIZE_ETHERNET;

    return true;
}

///////////////////////////////////////////////////////////////////////////////

}} //namespace
