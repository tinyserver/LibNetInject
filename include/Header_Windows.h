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
 * Purpose: Windows includes, compatibility definitions for VC
 * Author:  Thomas Volkert
 * Since:   2011-12-20
 */

// HINT: this abstractino of header includes is needed to have a defined order how we include the headers of Windows (esp. for WinSock2.h and Windows.h)
#ifndef HEADER_WINDOWS_H
#define HEADER_WINDOWS_H

///////////////////////////////////////////////////////////////////////////////

// use secured functions instead of old function for string handling
#ifdef _MSC_VER

// make sure the 64 bit version is marked as active
#ifndef WIN64
#define WIN64
#endif

#endif

// include all needed headers for 32 bit and 64 bit Windows
#if defined(WIN32) || defined(WIN64)

// activate some additional definitions within Windows includes
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <winsock2.h> // has to be included before Windows.h
#include <windows.h>
#include <ws2tcpip.h>
#include <io.h>
#include <BaseTsd.h>
#include <sys/timeb.h>
#include <stdio.h>
#include <Psapi.h>
#include <Tlhelp32.h>
#include <stdlib.h>
#include <shellapi.h>

// additional definitions for compatibility with gcc
#ifndef ssize_t
#define ssize_t SSIZE_T
#endif

#ifndef __const
#define __const
#endif

#endif

///////////////////////////////////////////////////////////////////////////////

#endif
