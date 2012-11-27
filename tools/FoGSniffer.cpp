/*****************************************************************************
 *
 *  Copyright (c) 2012 Thomas Volkert <thomas@homer-conferencing.com>
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
 * Purpose: sniffer for FoG packets
 * Author:  Thomas Volkert
 * Since:   2012-01-17
 */

#include <PacketFoG.h>
#include <HBLogger.h>

#include <string.h>

using namespace Frogger::Net;
using namespace Homer::Base;
using namespace std;


int main(int pArgc, char* pArgv[])
{
    LOGGER.Init(LOG_INFO);
    LOG(LOG_INFO, "======================= Waiting for FoG packets ==========================");
    printf("\n");

    PacketFoG tCapturedPacket;
    tCapturedPacket.SetSendDevice("eth0");
    tCapturedPacket.SetReceiveDevice("eth0");
    tCapturedPacket.PrepareReceive("", 3000, true);//wait for 3 seconds
    while(true)
    {
        if(tCapturedPacket.Receive())
        {
            tCapturedPacket.PrintHeaderEthernet();
            tCapturedPacket.PrintFoGLLCHeader();
            tCapturedPacket.PrintFoGHeader();
            tCapturedPacket.PrintFoGRoute();
            tCapturedPacket.PrintFoGPayload();
            printf("\n");
        }
    }
}
