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

/*****************************************************************************
 * This file was copied from Homer-Conferencing (www.homer-conferencing.com).
 * It is hereby separately published under BSD license with the permission of
 * the original author.
 *****************************************************************************/

/*
 * Purpose: header for class reflection
 * Author:  Thomas Volkert
 * Since:   2011-03-10
 */

#ifndef HB_REFLECTION_H
#define HB_REFLECTION_H

#include <typeinfo>
#include <stdio.h>
#include <stdlib.h>

inline std::string ParseRawObjectName(std::string pRawName)
{
	std::string tResult = "";
	int tPos = 2; //ignore prefix "PN"
	int tRawNameLength = (int)pRawName.length();

	for(;;)
	{
		int tSize = 0;
	    std::string tSizeStr = "";
		while ((pRawName[tPos] >= (char)'0') && (pRawName[tPos] <= (char)'9') && (tPos < tRawNameLength))
		{
			tSizeStr += pRawName[tPos];
			//LOG(LOG_ERROR, "# %s", tSizeStr.c_str());
			tPos++;
		}
		tSize = atoi(tSizeStr.c_str());
		//LOG(LOG_ERROR, "Size %d", tSize);
		if (tSize == 0)
			return tResult;
		tResult += pRawName.substr(tPos, tSize);

		// go to next entry within pRawName
		tPos += tSize;
		//LOG(LOG_ERROR, "%d %d", tPos, tRawNameLength);

		// are we at the end? (ignore "E" at the end of such a typeid string)
		if (tPos >= tRawNameLength -1)
			return tResult;
		else
			tResult += "::";
	}
	return "";
}

#define GetObjectNameRawStr(x)  (toString(typeid(x).name()))
#define GetObjectNameStr(x) (ParseRawObjectName(GetObjectNameRawStr(x)))

namespace Homer { namespace Base {

///////////////////////////////////////////////////////////////////////////////

class Reflection
{
public:
    Reflection( );

    virtual ~Reflection( );

private:
};

///////////////////////////////////////////////////////////////////////////////

}} // namespaces

#endif
