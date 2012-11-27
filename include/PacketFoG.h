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

// HINT: we assume Ethernet frames which encapsulate FoG packets
// HINT: we assume each source sends its data in the correct order
// HINT: we assume payload can be fragmented, in this case the first fragment is sent with explicit FoG header and every further fragment is sent without
// HINT: we use an FoG LLC header between Ethernet and FoG header to manage fragmentation

#ifndef PACKET_FOG_H
#define PACKET_FOG_H

#include <Header_Libnet.h>
#include <Packet.h>
#include <PacketEthernet.h>

#include <string>
#include <sys/types.h>

namespace Frogger { namespace Net {

///////////////////////////////////////////////////////////////////////////////
// FoG header
#define FOG_MARK_BACKWARD_ROUTE         0x0001
#define FOG_MARK_SIGNALING              0x0002
#define FOG_MARK_AUTHENTICATION         0x0004

struct HeaderFoG
{
    u_int16_t  HeaderSize;                      /* size of the FoG header */
    u_int16_t  MarkingReserved:13;              /* reserved space for future markings */
    u_int16_t  MarkingAuthentication:1;         /* authentication included in footer? */
    u_int16_t  MarkingSignaling:1;              /* does this packet has signaling data as payload? */
    u_int16_t  MarkingReverseRoute:1;           /* reverse route included in footer? */
    u_int16_t  ModificationCounter;             /* counter for route modifications */
    u_int32_t  PayloadSize;                     /* size of the entire payload */
    u_int32_t  RouteSize;                       /* size of the entire route */
    //u_int8_t   Route; //variable length       /* the route for the packet */
} __attribute__((__packed__));

struct HeaderFoGLLC
{
    u_int8_t    FirstFragment:1;                /* indicates if packet contains the first fragment of an appl. packet and has a FoG header */
    u_int8_t    LastFragment:1;                 /* indicates if packet contains the last fragment of an appl. packet */
    u_int8_t    Reserved:6;
} __attribute__((__packed__));

#define HEADER_SIZE_FOG     (sizeof(HeaderFoG))
#define HEADER_SIZE_FOG_LLC (sizeof(HeaderFoGLLC))
#define PAYLOAD_LIMIT_FOG   (PAYLOAD_LIMIT_ETHERNET - HEADER_SIZE_FOG_LLC)

// de/activate debugging of packets
//#define DEBUG_PACKETS_FOG

///////////////////////////////////////////////////////////////////////////////

class PacketFoG:
    public PacketEthernet
{
public:
    PacketFoG();
    virtual ~PacketFoG();

    /* send FoG packet */
    void SetFoGMarkingSignaling();
    void SetFoGMarkingFragment();
    void SetFoGPayload(char *pData, unsigned int pDataSize);
    void SetFoGRoute(char *pData, unsigned int pDataSize);
    void SetFoGReverseRoute(char *pData, unsigned int pDataSize);
    void SetFoGAuthentications(char *pData, unsigned int pDataSize);
    virtual void Reset(bool pRecursive = true); /* reset internal data structures to default values for a new packet */
    virtual int Send();

    /* receive FoG packet */
    virtual bool PrepareReceive(std::string pPacketFilter, int pTimeoutInMs = 0, bool pReceiveForeignPackets = true);
    bool GetFoGPayload(char *pData, unsigned int &pDataSize);
    bool IsLastFragment();
    virtual bool Receive();
    HeaderFoGLLC *GetReceivedHeaderFoGLLC();
    HeaderFoG *GetReceivedHeaderFoG();

    /* header debugging */
    void PrintFoGLLCHeader(HeaderFoGLLC *pHeader = NULL);
    void PrintFoGHeader(HeaderFoG *pHeader = NULL);
    void PrintFoGRoute(char* pRoute = NULL, unsigned int pRouteSize = 0);
    void PrintFoGPayload(char* pPayload = NULL, unsigned int pPayloadSize = 0);

protected:
    PacketFoG(int pPacketType);

    /* receiving */
    HeaderFoGLLC    *mReceivedFoGLLCHeader;
    HeaderFoG       *mReceivedFoGHeader;

private:
    void CalculateFoGHeaderForSending();
    void CalculateFoGLLCHeaderForSending();

    /* sending */
    u_int8_t        mEthernetPayload[PAYLOAD_LIMIT_ETHERNET];
    HeaderFoGLLC    mFoGLLCHeader;
    HeaderFoG       mFoGHeader;
    /* route */
    u_int8_t        mFoGRouteData[PAYLOAD_LIMIT_FOG];
    u_int32_t       mFoGRouteSize;
    /* reverse route */
    u_int8_t        mFoGReverseRouteData[PAYLOAD_LIMIT_FOG];
    u_int32_t       mFoGReverseRouteSize;
    /* authentications */
    u_int8_t        mFoGAuthenticationsData[PAYLOAD_LIMIT_FOG];
    u_int32_t       mFoGAuthenticationsSize;
    /* payload */
    u_int8_t        mFoGPayloadData[PAYLOAD_LIMIT_ETHERNET];
    u_int32_t       mFoGPayloadSize;
    /* markings */
    bool            mFoGMarkingSignaling;
    bool            mFoGMarkingAuthentication;
    bool            mFoGMarkingReverseRoute;
    bool            mFoGMarkingFragment;
};

///////////////////////////////////////////////////////////////////////////////


}} // namespace

#endif
