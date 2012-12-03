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

#ifndef HB_LOGGER_H
#define HB_LOGGER_H

#include <string>
#include <cstdio>
#include <sstream>

#include <HBTime.h>
#include <HBReflection.h>

namespace Homer { namespace Base {

///////////////////////////////////////////////////////////////////////////////

inline std::string GetShortFileName(std::string pLongFileName)
{
	std::string tResult = "";

	if (pLongFileName.substr(pLongFileName.length() - 4, 4) == ".cpp")
        tResult = pLongFileName.substr(0, pLongFileName.length() - 4 /* ".cpp" */);
	else
        tResult = pLongFileName.substr(0, pLongFileName.length() - 2 /* ".h" */);

	int tPos = (int)tResult.rfind('/');
	if (tPos != (int)std::string::npos)
		tResult = tResult.substr(tPos + 1, tResult.length() - tPos - 1);

	return tResult;
}

template <typename T>
inline std::string toString(T const& value_)
{
    std::stringstream ss;
    ss << value_;
    return ss.str();
}

inline bool IsLetter(char *pChar)
{
    if (pChar == NULL)
        return false;

    return (((*pChar >= 'a') && (*pChar <= 'z')) || ((*pChar >= 'A') && (*pChar <= 'Z')));
}

///////////////////////////////////////////////////////////////////////////////

#define         LOG_OFF                         0
#define         LOG_ERROR                       1
#define         LOG_INFO                        2
#define         LOG_VERBOSE                     3

#define         LOGGER                          Logger::getInstance()
// standard logging macro
#define         LOG(Level, ...)                 LOGGER.AddMessage(Level, GetObjectNameStr(this).c_str(), __LINE__, __VA_ARGS__)
// remote logging with given source file and line number
#define         LOG_REMOTE(Level, Source, Line, ...)   LOGGER.AddMessage(Level, Source.c_str(), Line, __VA_ARGS__)
// static logging
#define         LOGEX(FromWhere, Level, ...)    LOGGER.AddMessage(Level, ("static:" + GetObjectNameStr(FromWhere) + ":" + GetShortFileName(__FILE__)).c_str(), __LINE__, __VA_ARGS__)

///////////////////////////////////////////////////////////////////////////////

class Logger
{
public:

    Logger();

    /// The destructor
    virtual ~Logger();

    static Logger sLogger;
    static Logger& getInstance();

    void Init(int pLevel);
    void Deinit();
    void AddMessage(int pLevel, const char *pSource, int pLine, const char* pFormat, ...);
    void SetLogLevel(int pLevel);
    int GetLogLevel();

private:
    int         mLogLevel;
    int         mLastMessageLogLevel;
    std::string mLastMessage;
    std::string mLastSource;
    int         mLastLine;
    int         mRepetitionCount;

    Time        mTime;
};

///////////////////////////////////////////////////////////////////////////////

}} // namespace

#endif
