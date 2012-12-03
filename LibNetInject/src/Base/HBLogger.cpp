/*****************************************************************************
 *
 *  Copyright (c) 2009 Thomas Volkert <thomas@homer-conferencing.com>
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

/*****************************************************************************
 * This file was copied from Homer-Conferencing (www.homer-conferencing.com).
 * It is hereby separately published under BSD license with the permission of
 * the original author.
 *****************************************************************************/

/*
 * Purpose: header file for logger
 * Author:  Thomas Volkert
 * Since:   2009-05-19
 */

#include <HBLogger.h>

#include <stdarg.h>
#include <string.h>
#include <time.h>

#include <Header_Windows.h>

namespace Homer { namespace Base {

#ifdef WIN32
#ifndef __MINGW32__
// use secured vsprintf of MS Windows
#define vsprintf vsprintf_s
#define strcpy strcpy_s
#endif
#endif

///////////////////////////////////////////////////////////////////////////////

using namespace std;

Logger Logger::sLogger;
bool sLoggerReady = false;

///////////////////////////////////////////////////////////////////////////////

Logger::Logger()
{
    mLogLevel = LOG_ERROR;
    mLastMessageLogLevel = LOG_ERROR;
    mLastSource = "";
    mLastLine = 0;
    mRepetitionCount = 0;
    sLoggerReady = true;
}

Logger::~Logger()
{
}

Logger& Logger::getInstance()
{
    return sLogger;
}

///////////////////////////////////////////////////////////////////////////////
void Logger::AddMessage(int pLevel, const char *pSource, int pLine, const char* pFormat, ...)
{
    if( !sLoggerReady)
    {
        printf("LOGGER: tried to log message from %s(%d) when logger instance isn't valid\n", pSource, pLine);
        return;
    }

    if (pLevel > mLogLevel /* we won't produce an output in the console */)
    {// nothing to do

        // return immediately
        return;
    }

    va_list tVArgs;
    char tMessageBuffer[4 * 1024];

    va_start(tVArgs, pFormat);
    vsprintf(tMessageBuffer, pFormat, tVArgs);
    va_end(tVArgs);

    string tFinalSource, tFinalTime, tFinalMessage;
    int tHour, tMin, tSec;
    Time::GetNow(0, 0, 0, &tHour, &tMin, &tSec);
    tFinalTime = (tHour < 10 ? "0" : "") + toString(tHour) + ":" + (tMin < 10 ? "0" : "") + toString(tMin) + "." + (tSec < 10 ? "0" : "") + toString(tSec);
    tFinalSource = toString(pSource);
    tFinalMessage = toString(tMessageBuffer);

    if ((mLastMessage != tFinalMessage) || (mLastSource != tFinalSource) || (mLastLine != pLine))
    {
        if (mRepetitionCount)
        {
            if (mLastMessageLogLevel <= mLogLevel)
                printf("        LAST MESSAGE WAS REPEATED %d TIME(S)\n", mRepetitionCount);

            mRepetitionCount = 0;
        }
        if ((pLevel <= mLogLevel) && (pLevel > LOG_OFF))
        {
            int tHour, tMin, tSec;
            mTime.GetNow(0, 0, 0, &tHour, &tMin, &tSec);
            //FILE *tFile = fopen("log.txt", "a");

            printf("(%02d:%02d.%02d)", tHour, tMin, tSec);
            //fprintf(tFile, "(%02d:%02d.%02d)", tHour, tMin, tSec);

            switch(pLevel)
            {
                case LOG_ERROR:
                            //fprintf(tFile, "ERROR:   %s(%d): %s\n", pFileName, pLine, tMessage);
                            printf(" ERROR:   %s(%d): %s\n", pSource, pLine, tMessageBuffer);
                            break;
                case LOG_INFO:
                            //fprintf(tFile, "INFO:    %s(%d): %s\n", pFileName, pLine, tMessage);
                            printf(" INFO:    %s(%d): %s\n", pSource, pLine, tMessageBuffer);
                            break;
                case LOG_VERBOSE:
                            //fprintf(tFile, "VERBOSE: %s(%d): %s\n", pFileName, pLine, tMessage);
                            printf(" VERBOSE: %s(%d): %s\n", pSource, pLine, tMessageBuffer);
                            break;
            }

            //fclose(tFile);
            mLastMessageLogLevel = pLevel;
            mLastSource = tFinalSource;
            mLastMessage = tFinalMessage;
            mLastLine = pLine;
        }
    }else
        mRepetitionCount++;
}

void Logger::Init(int pLevel)
{
    SetLogLevel(pLevel);
}

void Logger::Deinit()
{
}

void Logger::SetLogLevel(int pLevel)
{
    switch(pLevel)
    {
        case LOG_ERROR:
                mLogLevel = pLevel;
                break;
        case LOG_INFO:
                LOG(LOG_INFO, "Set log level to: INFO");
                mLogLevel = pLevel;
                break;
        case LOG_VERBOSE:
                LOG(LOG_INFO, "Set log level to: VERBOSE");
                mLogLevel = pLevel;
                break;
        default:
                mLogLevel = LOG_OFF;
                break;
    }
}

int Logger::GetLogLevel()
{
    if( !sLoggerReady)
    {
        printf("Tried to get log level when logger isn't available yet\n");
        return LOG_VERBOSE;
    }

    return mLogLevel;
}

///////////////////////////////////////////////////////////////////////////////

}} //namespace
