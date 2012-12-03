###############################################################################
# 
#   Copyright (c) 2012 Thomas Volkert <thomas@homer-conferencing.com>
#   All rights reserved.
# 
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions
#  are met:
#  1. Redistributions of source code must retain the above copyright
#     notice, this list of conditions and the following disclaimer.
#  2. Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in the
#     documentation and/or other materials provided with the distribution.
# 
#  THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
#  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
#  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
#  ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
#  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
#  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
#  OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
#  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
#  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
#  OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
#  SUCH DAMAGE.
# 
###############################################################################

INCLUDE(${CMAKE_CURRENT_SOURCE_DIR}/../../LibNetInjectBuild/CMakeConfig.txt)

##############################################################
# Configuration
##############################################################

##############################################################
# include directories
SET (INCLUDE_DIRS
	../include
	../../LibNetInject/include
	../../LibNetInject/include/Base
)
	
##############################################################
# target directory for the executable
SET (TARGET_DIRECTORY
	${RELOCATION_DIR}
)

##############################################################
# compile flags
SET (FLAGS
	${FLAGS}
	-D_GLIBCXX_USE_WSTRING
)

##############################################################
# set rapth entries for non-default builds
IF (NOT (${BUILD} MATCHES "Default"))
	IF (APPLE)
		SET (LFLAGS
			${LFLAGS}
			-Wl,-rpath,.
			-Wl,-rpath,./lib
			-Wl,-rpath,../lib
			-Wl,-rpath,../../lib
			-Wl,-rpath,./bin
			-Wl,-rpath,./bin/lib
			-Wl,-rpath,../bin/lib
			-Wl,-rpath,../../bin/lib
			-Wl,-rpath,/usr/lib
			-Wl,-rpath,/usr/local/lib
		)
		IF (DEFINED INSTALL_LIBDIR)
			SET (LFLAGS
				${LFLAGS}
				-Wl,-rpath,${INSTALL_LIBDIR}
			)
		ENDIF()
	ELSE(APPLE)
		SET (LFLAGS
			${LFLAGS}
			-Wl,-R.
			-Wl,-R./lib
			-Wl,-R../lib
			-Wl,-R../../lib
			-Wl,-R./bin
			-Wl,-R./bin/lib
			-Wl,-R../bin/lib
			-Wl,-R../../bin/lib
			-Wl,-R/usr/lib
			-Wl,-R/usr/local/lib
		)
		IF (DEFINED INSTALL_LIBDIR)
			SET (LFLAGS
				${LFLAGS}
				-Wl,-R${INSTALL_LIBDIR}
			)
		ENDIF()
	ENDIF()
ENDIF()

IF (WIN32)
	IF (${BUILD} MATCHES "Debug")
		SET (LFLAGS	"${LFLAGS} -Wl,--subsystem,console")
	ELSE()
		SET (LFLAGS	"${LFLAGS} -Wl,--subsystem,windows")
	ENDIF ()
ENDIF (WIN32)

##############################################################
# SOURCES
SET (SOURCES
	../src/tests
)

##############################################################
# USED LIBRARIES for win32 environment
SET (LIBS_WINDOWS_INSTALL
    libgcc_s_dw2-1
    libstdc++-6
)
SET (LIBS_WINDOWS
	NetInject
)

# USED LIBRARIES for linux environment
set(LIBS_LINUX
	NetInject
)

# USED LIBRARIES for BSD environment
set(LIBS_BSD
	NetInject
)

# USED LIBRARIES for apple environment
set(LIBS_APPLE
	NetInject
)

##############################################################
SET (TARGET_PROGRAM_NAME
	tests
)

INCLUDE(${CMAKE_CURRENT_SOURCE_DIR}/../../LibNetInjectBuild/CMakeCore.txt)