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
 * Purpose: wrapper for os independent time handling
 * Author:  Thomas Volkert
 * Since:   2010-09-23
 */

#ifndef HB_TIME_H
#define HB_TIME_H

#include <stdint.h>
#include <sys/types.h>
#include <time.h>

#ifdef M_LINUX
#include <sys/time.h>
#endif

#ifdef WIN32
#include <sys/timeb.h>
#endif

namespace Homer { namespace Base {

///////////////////////////////////////////////////////////////////////////////

class Time
{
public:
    Time( );

    virtual ~Time( );

    bool ValidTimeStamp();
    void InvalidateTimeStamp();
    int64_t UpdateTimeStamp(); // in µs
    int64_t TimeDiffInUSecs(Time *pTime);
    static int64_t GetTimeStamp(); // in µs
    static bool GetNow(int *pDay = NULL, int *pMonth = NULL, int *pYear = NULL, int *pHour = NULL, int *pMin = NULL, int *pSec = NULL);

    Time& operator=(const Time &pTime);

private:
    int64_t     mTimeStamp; // in µs
};

///////////////////////////////////////////////////////////////////////////////

}} // namespaces

#endif
