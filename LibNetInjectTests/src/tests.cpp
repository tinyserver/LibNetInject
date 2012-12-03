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
 * Purpose: test program to show the basic usage of the library
 * Author:  Thomas Volkert
 * Since:   2010-10-07
 */

#include <PacketEthernet.h>
#include <PacketIp.h>
#include <PacketUdp.h>
#include <PacketTcp.h>
#include <PacketIcmpEcho.h>
#include <PacketIcmpTimeExceed.h>
#include <PacketIcmpUnreachable.h>
#include <HBLogger.h>
#include <HBTime.h>

#include <string.h>

using namespace Frogger::Net;
using namespace Homer::Base;
using namespace std;

int main(int pArgc, char* pArgv[])
{
    LOGGER.Init(LOG_ERROR);
    LOGEX(main, LOG_INFO, "Demo for LibNetInject\n");
    LOGEX(main, LOG_INFO, "\n======================= Sending ==========================");

    char tData[1400] = "This is a payload test 123!This is a payload test 123!This is a payload test 123!This is a payload test 123!This is a payload test 123!";

    //HINT: DO NOT FORGET TO SET THE CORRECT SEND/RECEIVE DEVICE HERE!
    // ..

    PacketIp tIpPacket;
    tIpPacket.SetIpSourceAdr("127.0.0.1");
    tIpPacket.SetIpDestinationAdr("127.0.1.1");
    tIpPacket.SetIpTimeToLive(8);
    tIpPacket.SetIpTypeofService(5);
    tIpPacket.SetIpPayload(tData, strlen(tData));
    tIpPacket.Send();

    PacketUdp tUdpPacket;
    tUdpPacket.SetIpSourceAdr("127.0.0.1");
    tUdpPacket.SetIpDestinationAdr("127.0.1.1");
    tUdpPacket.SetIpTimeToLive(8);
    tUdpPacket.SetIpTypeofService(5);
    tUdpPacket.SetUdpSourcePort(13);
    tUdpPacket.SetUdpDestinationPort(14);
    tUdpPacket.SetUdpPayload(tData, strlen(tData));
    tUdpPacket.Send();

    PacketTcp tTcpPacket;
    tTcpPacket.SetIpSourceAdr("127.0.0.1");
    tTcpPacket.SetIpDestinationAdr("127.0.1.1");
    tTcpPacket.SetIpTimeToLive(8);
    tTcpPacket.SetIpTypeofService(5);
    tTcpPacket.SetTcpSourcePort(13);
    tTcpPacket.SetTcpDestinationPort(14);
    tTcpPacket.SetTcpPayload(tData, strlen(tData));
    tTcpPacket.Send();

    PacketIcmpEcho tIcmpEchoPacket;
    tIcmpEchoPacket.SetIpSourceAdr("127.0.0.1");
    tIcmpEchoPacket.SetIpDestinationAdr("127.0.1.1");
    tIcmpEchoPacket.SetIpTimeToLive(8);
    tIcmpEchoPacket.SetIpTypeofService(5);
    tIcmpEchoPacket.SetIcmpEchoPayload(tData, strlen(tData));
    tIcmpEchoPacket.SetIcmpEchoType(false);
    tIcmpEchoPacket.Send();

    tIcmpEchoPacket.SetIcmpEchoType(true);
    tIcmpEchoPacket.Send();

    PacketEthernet tEthernetPacket;
    while(true)
    {
        int64_t tTime = Time::GetTimeStamp();
        tEthernetPacket.Reset();
        //printf("Reset: %ld\n", Time::GetTimeStamp() - tTime);
        tEthernetPacket.SetEthernetDestinationAdr("FF:FF:FF:FF:FF:FF");
        tEthernetPacket.SetEthernetSourceAdr("00:00:00:00:00:00");
        tEthernetPacket.SetEthernetPayload(tData, strlen(tData));
        //printf("Set: %ld\n", Time::GetTimeStamp() - tTime);
        tEthernetPacket.Send();
        //printf("Sent: %ld\n", Time::GetTimeStamp() - tTime);
    }

    PacketIcmpTimeExceed tIcmpTimeExceedPacket;
    tIcmpTimeExceedPacket.SetIpSourceAdr("127.0.0.1");
    tIcmpTimeExceedPacket.SetIpDestinationAdr("127.0.1.1");
    tIcmpTimeExceedPacket.SetIpTimeToLive(8);
    tIcmpTimeExceedPacket.SetIpTypeofService(5);
    tIcmpTimeExceedPacket.SetIcmpTimeExceedCode(false);
    tIcmpTimeExceedPacket.Send();

    PacketIcmpUnreachable tIcmpUnreachablePacket;
    tIcmpUnreachablePacket.SetIpSourceAdr("127.0.0.1");
    tIcmpUnreachablePacket.SetIpDestinationAdr("127.0.1.1");
    tIcmpUnreachablePacket.SetIpTimeToLive(8);
    tIcmpUnreachablePacket.SetIpTypeofService(5);
    tIcmpUnreachablePacket.SetIcmpUnreachableCode(ICMP_UNREACHABLE_CODE_HOST);
    tIcmpUnreachablePacket.Send();

    LOGEX(main, LOG_INFO, "\n======================= Receiving ==========================");

    PacketTcp tCapturedPacket;
    tCapturedPacket.PrepareReceive("", 3000);//wait for 3 seconds
    tCapturedPacket.Receive();
    tCapturedPacket.PrintHeaderTcp();

    PacketUdp tCapturedPacket1;
    tCapturedPacket1.PrepareReceive("", 3000);//wait for 3 seconds
    tCapturedPacket1.Receive();
    tCapturedPacket1.PrintHeaderUdp();
}
