/*****************************************************************************
 *
 *  Copyright (c) 2011 Thomas Volkert <thomas@homer-conferencing.com>
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
 * Purpose: FoG packet handling
 * Author:  Thomas Volkert
 * Since:   2011-12-19
 */


#include <Packet.h>
#include <PacketFoG.h>
#include <PacketEthernet.h>
#include <Header_Libnet.h>
#include <HBLogger.h>

#include <string>

namespace Frogger { namespace Net {

using namespace std;
using namespace Homer::Base;

///////////////////////////////////////////////////////////////////////////////

// helper function to filter for valid ASCII chars
void printAscii(char pChar)
{
    char tChar = 0xFF & pChar;
    switch(tChar)
    {
        case '|':
        case ' ':
        case '(':
        case ')':
        case '[':
        case ']':
        case '<':
        case '>':
        case '&':
        case '=':
        case '+':
        case '*':
        case ',':
        case ':':
        case '.':
        case '-':
        case '\\':
        case '/':
        case '0'...'9':
        case 'a'...'z':
        case 'A'...'Z':
                printf(" %c ", tChar);
                break;
        default:
                printf("   ");
                break;
    }
}

// helper function to present buffer data human readable on screen
void printBufferData(__const void *pBuf, int  pBufSize)
{
    int jCount = 5;
    int i,j,k;

    if (pBufSize > 0)
    {
        printf("   Index:   ");
        for (j = 0; j < jCount; j++)
        {
            for (i = 0; i < 8; i++)
            {
                k = i + j * 8;
                if (k < pBufSize)
                    printf("%2d ", k);
            }
            printf("  ");
        }
        printf("\n");

        printf("   Value:   ");
        for (j = 0; j < jCount; j++)
        {
            for (i = 0; i < 8; i++)
            {
                k = i + j * 8;
                if (k < pBufSize)
                    printf("%02X ", 0xFF & ((char*)pBuf)[k]);
            }
            printf("  ");
        }
        printf("\n");

        printf("   ASCII:   ");
        for (j = 0; j < jCount; j++)
        {
            for (i = 0; i < 8; i++)
            {
                k = i + j * 8;
                if (k < pBufSize)
                {
                    printAscii(((char*)pBuf)[k]);
                }
            }
            printf("  ");
        }
        printf("\n");

    }
}

///////////////////////////////////////////////////////////////////////////////

PacketFoG::PacketFoG():
    PacketEthernet(PACKET_TYPE_FOG)
{
    Reset(false);
}

PacketFoG::PacketFoG(int pPacketType):
    PacketEthernet(pPacketType)
{
    Reset(false);
}

PacketFoG::~PacketFoG()
{

}

///////////////////////////////////////////////////////////////////////////////

void PacketFoG::SetFoGMarkingSignaling()
{
    mFoGMarkingSignaling = true;
}

void PacketFoG::SetFoGMarkingFragment()
{
    mFoGMarkingFragment = true;
}

void PacketFoG::SetFoGPayload(char *pData, unsigned int pDataSize)
{
    if(pDataSize > PAYLOAD_LIMIT_FOG)
    {
        LOG(LOG_ERROR, "Payload of size %d for FoG packets too big, limit is %d", pDataSize, PAYLOAD_LIMIT_FOG);
        mFoGPayloadSize = 0;
        return;
    }
    memcpy((void*)mFoGPayloadData, pData, pDataSize);
    mFoGPayloadSize = (u_int32_t)pDataSize;
}

void PacketFoG::SetFoGRoute(char *pData, unsigned int pDataSize)
{
    if(pDataSize > PAYLOAD_LIMIT_FOG)
    {
        LOG(LOG_ERROR, "Route of size %d for FoG packets too big, limit is %d", pDataSize, PAYLOAD_LIMIT_FOG);
        mFoGRouteSize = 0;
        return;
    }
    memcpy((void*)mFoGRouteData, pData, pDataSize);
    mFoGRouteSize = (u_int32_t)pDataSize;
}

void PacketFoG::SetFoGReverseRoute(char *pData, unsigned int pDataSize)
{
    if(pDataSize > PAYLOAD_LIMIT_FOG)
    {
        LOG(LOG_ERROR, "Reverse route of size %d for FoG packets too big, limit is %d", pDataSize, PAYLOAD_LIMIT_FOG);
        mFoGReverseRouteSize = 0;
        return;
    }
    memcpy((void*)mFoGReverseRouteData, pData, pDataSize);
    mFoGReverseRouteSize = (u_int32_t)pDataSize;
    mFoGMarkingReverseRoute = true;
}

void PacketFoG::SetFoGAuthentications(char *pData, unsigned int pDataSize)
{
    if(pDataSize > PAYLOAD_LIMIT_FOG)
    {
        LOG(LOG_ERROR, "Authentications of size %d for FoG packets too big, limit is %d", pDataSize, PAYLOAD_LIMIT_FOG);
        mFoGAuthenticationsSize = 0;
        return;
    }
    memcpy((void*)mFoGAuthenticationsData, pData, pDataSize);
    mFoGAuthenticationsSize = (u_int32_t)pDataSize;
    mFoGMarkingAuthentication = true;
}

void PacketFoG::Reset(bool pRecursive)
{
    LOG(LOG_VERBOSE, "Going to reset header data structures");
    mReceivedFoGHeader = NULL;

    LOG(LOG_VERBOSE, "..reset of payload data");
    memset((void*)mFoGPayloadData, 0, sizeof(mFoGPayloadData));
    mFoGPayloadSize = 0;

    LOG(LOG_VERBOSE, "..reset of route data");
    memset((void*)mFoGRouteData, 0, sizeof(mFoGRouteData));
    mFoGRouteSize = 0;

    LOG(LOG_VERBOSE, "..reset of reverse route data");
    memset((void*)mFoGReverseRouteData, 0, sizeof(mFoGReverseRouteData));
    mFoGReverseRouteSize = 0;

    LOG(LOG_VERBOSE, "..reset of auth. data");
    memset((void*)mFoGAuthenticationsData, 0, sizeof(mFoGAuthenticationsData));
    mFoGAuthenticationsSize = 0;

    mFoGMarkingSignaling = false;
    mFoGMarkingAuthentication = false;
    mFoGMarkingReverseRoute = false;
    mFoGMarkingFragment = false;

    if (pRecursive)
    {
        LOG(LOG_VERBOSE, "..call Reset() recursive");
        Packet::Reset();
    }
}

void PacketFoG::CalculateFoGLLCHeaderForSending()
{
    if(mFoGRouteSize > 0)
    {
        mFoGLLCHeader.FirstFragment = true;
        mFoGLLCHeader.LastFragment = !mFoGMarkingFragment;
    }else
    {
        mFoGLLCHeader.FirstFragment = false;
        mFoGLLCHeader.LastFragment = !mFoGMarkingFragment;
    }
}

void PacketFoG::CalculateFoGHeaderForSending()
{
    mFoGHeader.HeaderSize = sizeof(HeaderFoG);
    mFoGHeader.MarkingAuthentication = mFoGMarkingAuthentication;
    mFoGHeader.MarkingReverseRoute = mFoGMarkingReverseRoute;
    mFoGHeader.MarkingSignaling = mFoGMarkingSignaling;
    mFoGHeader.MarkingReserved = 0;
    mFoGHeader.ModificationCounter = 0;
    mFoGHeader.PayloadSize = mFoGPayloadSize;
    mFoGHeader.RouteSize = mFoGRouteSize;
}

int PacketFoG::Send()
{
    int tResult = -1;

    if(mFoGPayloadSize == 0)
    {
        LOG(LOG_ERROR, "Payload not set, sending of packet skipped");
        return tResult;
    }

    LOG(LOG_VERBOSE, "Going to send FoG packet");

    // ########################################
    // ### FoG LLC header
    // ########################################
    CalculateFoGLLCHeaderForSending();
    PrintFoGLLCHeader(&mFoGLLCHeader);
    memcpy(mEthernetPayload, &mFoGLLCHeader, HEADER_SIZE_FOG_LLC);

    if(mFoGRouteSize > 0)
    {
        /*
         * we send a completed FoG packet
         *      FoG LLC
         *      FoG header (including FoG route)
         *      FoG payload
         */

        // ########################################
        // ### FoG header
        // ########################################
        CalculateFoGHeaderForSending();
        PrintFoGHeader(&mFoGHeader);
        memcpy((void*)(mEthernetPayload + HEADER_SIZE_FOG_LLC), (const void*)&mFoGHeader, HEADER_SIZE_FOG);

        // ########################################
        // ### FoG route
        // ########################################
        PrintFoGRoute((char*)mFoGRouteData, mFoGRouteSize);
        memcpy((void*)(mEthernetPayload + HEADER_SIZE_FOG_LLC + HEADER_SIZE_FOG), (const void*)mFoGRouteData, mFoGRouteSize);

        // ########################################
        // ### FoG payload
        // ########################################
        if(mFoGPayloadSize > 0)
            memcpy((void*)(mEthernetPayload + HEADER_SIZE_FOG_LLC + HEADER_SIZE_FOG + mFoGHeader.RouteSize), mFoGPayloadData, mFoGPayloadSize);

        // ########################################
        // ### Ethernet payload
        // ########################################
        SetEthernetPayload((char*)mEthernetPayload, HEADER_SIZE_FOG_LLC + HEADER_SIZE_FOG + mFoGRouteSize + mFoGPayloadSize);
    }else
    {
        /*
         * we send a fragment of a FoG packet
         *      FoG LLC
         *      FoG payload
         */

        // ########################################
        // ### FoG payload
        // ########################################
        if(mFoGPayloadSize > 0)
            memcpy((void*)(mEthernetPayload + HEADER_SIZE_FOG_LLC), mFoGPayloadData, mFoGPayloadSize);

        // ########################################
        // ### Ethernet payload
        // ########################################
        SetEthernetPayload((char*)mEthernetPayload, HEADER_SIZE_FOG_LLC + mFoGPayloadSize);
    }

    // ########################################
    // ### SEND
    // ########################################
    tResult = PacketEthernet::Send();
    LOG(LOG_VERBOSE, "Have sent FoG packet");

    return tResult;
}

bool PacketFoG::PrepareReceive(string pPacketFilter, int pTimeoutInMs, bool pReceiveForeignPackets)
{
    pPacketFilter = "ether proto "ETHERTYPE_FOG_STR;
    return Packet::PrepareReceive(pPacketFilter, pTimeoutInMs, pReceiveForeignPackets);
}

bool PacketFoG::Receive()
{
    bool tResult = Packet::Receive();
    if (tResult)
    {
        mReceivedFoGLLCHeader = (HeaderFoGLLC*)(mReceivedPacketData + HEADER_SIZE_ETHERNET);
        mReceivedFoGHeader = (HeaderFoG*)(mReceivedPacketData + HEADER_SIZE_ETHERNET + HEADER_SIZE_FOG_LLC);
        #ifdef DEBUG_PACKETS_FOG
            PacketEthernet::PrintHeaderEthernet((HeaderEthernet*)mReceivedPacketData);
            PrintFoGHeader(mReceivedFoGHeader);
        #endif

        LOG(LOG_VERBOSE, "Have received a FoG packet");
    }

    return tResult;
}

void PacketFoG::PrintFoGLLCHeader(HeaderFoGLLC *pHeader)
{
    if (pHeader == NULL)
    {
        pHeader = mReceivedFoGLLCHeader;
    }
        
    LOG(LOG_INFO, "################################# FoG-LLC ################################");
    LOG(LOG_INFO, "Header size: %d", HEADER_SIZE_FOG_LLC);
    LOG(LOG_INFO, "First fragment: %s", pHeader->FirstFragment ? "yes" : "no");
    LOG(LOG_INFO, "Last fragment: %s", pHeader->LastFragment ? "yes" : "no");
}

void PacketFoG::PrintFoGHeader(HeaderFoG *pHeader)
{
    if (pHeader == NULL)
    {
        if(!mReceivedFoGLLCHeader->FirstFragment)
            return;

        pHeader = mReceivedFoGHeader;
    }

    LOG(LOG_INFO, "################################### FoG ##################################");
    LOG(LOG_INFO, "Header size: %hu", pHeader->HeaderSize);
    LOG(LOG_INFO, "Footer with auth.: %s", pHeader->MarkingAuthentication ? "yes" : "no");
    LOG(LOG_INFO, "Footer with reverse route: %s", pHeader->MarkingReverseRoute ? "yes" : "no");
    LOG(LOG_INFO, "Signaling data: %s", pHeader->MarkingSignaling ? "yes" : "no");
    LOG(LOG_INFO, "Modification counter: %hu", pHeader->ModificationCounter);
    LOG(LOG_INFO, "Payload size: %u", pHeader->PayloadSize);
    LOG(LOG_INFO, "Route size: %u", pHeader->RouteSize);
}

void PacketFoG::PrintFoGRoute(char* pRoute, unsigned int pRouteSize)
{
    if(pRoute == NULL)
    {
        if(!mReceivedFoGLLCHeader->FirstFragment)
        {
            return;
        }

        pRoute = (((char*)mReceivedFoGHeader) + HEADER_SIZE_FOG);
        pRouteSize = mReceivedFoGHeader->RouteSize;
    }
    LOG(LOG_INFO, "############################### FoG route ################################");
    LOG(LOG_INFO, "Size: %u", pRouteSize);
    LOG(LOG_INFO, "Data: ");
    if (LOGGER.getLogLevel() >= LOG_INFO)
    {
        printBufferData((const void*)pRoute, (int)pRouteSize);
    }
}

void PacketFoG::PrintFoGPayload(char *pPayload, unsigned int pPayloadSize)
{
    if(pPayload == NULL)
    {
        if(mReceivedFoGLLCHeader->FirstFragment)
        {
            pPayload = (((char*)mReceivedFoGHeader) + HEADER_SIZE_FOG + mReceivedFoGHeader->RouteSize);
            pPayloadSize = mReceivedFoGHeader->PayloadSize;
        }else{
            pPayload = (char*)(mReceivedPacketData + HEADER_SIZE_ETHERNET + HEADER_SIZE_FOG_LLC);
            pPayloadSize = mReceivedPacketSize - HEADER_SIZE_ETHERNET - HEADER_SIZE_FOG_LLC;
        }
    }
    LOG(LOG_INFO, "############################## FoG payload ###############################");
    LOG(LOG_INFO, "Size: %u", pPayloadSize);
    LOG(LOG_INFO, "Data: ");

    if (LOGGER.getLogLevel() >= LOG_INFO)
    {
        printBufferData((const void*)pPayload, (int)pPayloadSize);
    }
}

HeaderFoGLLC *PacketFoG::GetReceivedHeaderFoGLLC()
{
    return mReceivedFoGLLCHeader;
}

HeaderFoG *PacketFoG::GetReceivedHeaderFoG()
{
    return mReceivedFoGHeader;
}

bool PacketFoG::GetFoGPayload(char *pData, unsigned int &pDataSize)
{
    unsigned int tReceivedDataSize;

    if ((pData == NULL) || (mReceivedPacketData == NULL))
    {
        LOG(LOG_ERROR, "Data buffer invalid");
        return false;
    }

    if (mReceivedFoGLLCHeader->FirstFragment)
    {
        tReceivedDataSize = mReceivedFoGHeader->PayloadSize;

        if (tReceivedDataSize > pDataSize)
        {
            LOG(LOG_ERROR, "Given payload buffer is too small");
            return false;
        }

        pDataSize = tReceivedDataSize;
        memcpy(pData, (const void*)(((char*)mReceivedFoGHeader) + HEADER_SIZE_FOG + mReceivedFoGHeader->RouteSize), pDataSize);
    }else{
        tReceivedDataSize = mReceivedPacketSize - HEADER_SIZE_ETHERNET - HEADER_SIZE_FOG_LLC;

        if (tReceivedDataSize > pDataSize)
        {
            LOG(LOG_ERROR, "Given payload buffer is too small");
            return false;
        }

        pDataSize = tReceivedDataSize;
        memcpy(pData, (const void*)(mReceivedPacketData + HEADER_SIZE_ETHERNET + HEADER_SIZE_FOG_LLC), pDataSize);
    }

    #ifdef DEBUG_PACKETS_FOG
        PrintFoGPayload(pData, pDataSize);
    #endif

    return true;
}

bool PacketFoG::IsLastFragment()
{
    return mReceivedFoGLLCHeader->LastFragment;
}

///////////////////////////////////////////////////////////////////////////////

}} //namespace
