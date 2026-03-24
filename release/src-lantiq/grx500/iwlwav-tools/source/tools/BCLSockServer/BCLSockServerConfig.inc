#
# $Id: $
#

DRIVER_ROOT=..\..\..\wireless\driver
!include $(DRIVER_ROOT)\wpath.inc

# Settings
TARGETNAME      = BCLSockServer
TARGETDEFNAME   = $(TARGETNAME)
DEFFILE         = $(TARGETDEFNAME).def
TARGETPATH      = $(DRIVER_ROOT)\$(MTLK_TOOLS_BIN_PATH)
DEFFILE         = $(TARGETDEFNAME).def
TARGETTYPE      = PROGRAM
UMTYPE          = console
#USE_MSVCRT      = 1
USE_LIBCMT      = 1
USE_ATL         = 1

# Defines
C_DEFINES = $(C_DEFINES) /D_STANDALONE=1 /D_USE_STATIC_LIB

#
# Optimize and Level 4 warnings
#
MSC_WARNING_LEVEL = /W4

# Treat warnings as errors   
MSC_WARNING_LEVEL = $(MSC_WARNING_LEVEL) /WX

# 64-bit warnings
# /Wp64
#MSC_WARNING_LEVEL = /Od

INCLUDES = \
           ..\BCL;

SOURCES = ..\BCLSockServer.c
