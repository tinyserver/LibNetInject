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
 * It is here separately published under BSD license with the permission of
 * the original author.
 *****************************************************************************/

/*
 * Purpose: wrapper for os independent time handling
 * Author:  Thomas Volkert
 * Since:   2010-09-23
 */

#include <HBLogger.h>

#include <errno.h>
#include <time.h>

#ifdef M_LINUX
#include <sys/time.h>
#endif

#ifdef WIN32
#include <sys/timeb.h>
#endif

namespace Homer { namespace Base {

using namespace std;

///////////////////////////////////////////////////////////////////////////////

Time::Time()
{
    mTimeStamp = 0;
}

Time::~Time()
{
}

///////////////////////////////////////////////////////////////////////////////

Time& Time::operator=(const Time &pTime)
{
	// only do assignment if pTime is a different object from this
	if (this != &pTime)
	{
		mTimeStamp = pTime.mTimeStamp;
	}
	return *this;
}

bool Time::ValidTimeStamp()
{
    return (mTimeStamp != 0);
}

void Time::InvalidateTimeStamp()
{
    mTimeStamp = 0;
}

int64_t Time::GetTimeStamp()
{
	int64_t tResult = 0;
	#ifdef M_LINUX
		struct timeval tTimeVal;
		gettimeofday(&tTimeVal, 0);
		tResult = (int64_t)1000 * 1000 * tTimeVal.tv_sec + tTimeVal.tv_usec;
	#endif

	#ifdef WIN32
		struct _timeb tTimeVal;
		#ifdef __MINGW32__
			_ftime(&tTimeVal);
		#else
			_ftime64_s(&tTimeVal);
		#endif
		tResult = (int64_t)1000 * 1000 * tTimeVal.time + tTimeVal.millitm * 1000;
	#endif

	return tResult;
}

int64_t Time::UpdateTimeStamp()
{
	mTimeStamp = GetTimeStamp();
    return mTimeStamp;
}

int64_t Time::TimeDiffInUSecs(Time *pTime)
{
    return ((int64_t)(mTimeStamp - pTime->mTimeStamp));
}

bool Time::GetNow(int *pDay, int *pMonth, int *pYear, int *pHour, int *pMin, int *pSec)
{
    bool tResult = false;
    time_t tTimeStamp = UpdateTimeStamp() / 1000000;
    struct tm tLocalTimeData;

    #if defined(LINUX) || defined(APPLE) || defined(BSD)
        struct tm* tLocalTime = localtime_r(&tTimeStamp, &tLocalTimeData);
        if (tLocalTime == NULL)
            LOGEX(Time, LOG_ERROR, "Error while determining current time");
        else
            tResult = true;
    #endif
    #ifdef WIN32
		#ifdef __MINGW32__
			struct tm* tTm = localtime(&tTimeStamp);
			tLocalTimeData.tm_mday = tTm->tm_mday;
			tLocalTimeData.tm_mon = tTm->tm_mon;
			tLocalTimeData.tm_year = tTm->tm_year;
			tLocalTimeData.tm_hour = tTm->tm_hour;
			tLocalTimeData.tm_min = tTm->tm_min;
			tLocalTimeData.tm_sec = tTm->tm_sec;
		#else
	        errno_t tErr = localtime_s(&tLocalTimeData, &tTimeStamp);
	        if (tErr != 0)
	            LOG(LOG_ERROR, "Error while determining current time");
	        else
	            tResult = true;
		#endif
    #endif
    if (pDay)
        *pDay = tLocalTimeData.tm_mday;
    if (pMonth)
        *pMonth = tLocalTimeData.tm_mon + 1;
    if (pYear)
        *pYear = tLocalTimeData.tm_year + 1900;
    if (pHour)
        *pHour = tLocalTimeData.tm_hour;
    if (pMin)
        *pMin = tLocalTimeData.tm_min;
    if (pSec)
        *pSec = tLocalTimeData.tm_sec;

    return tResult;
}

///////////////////////////////////////////////////////////////////////////////

}} //namespace
