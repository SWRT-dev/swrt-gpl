#
# $Id$
#

DRIVER_ROOT = ..\..

!include $(DRIVER_ROOT)\wconfig.inc
!include $(DRIVER_ROOT)\wpath.inc

# warning C4214: nonstandard extension used : bit field types other than int
# warning C4995: 'strcat' etc : name was marked as #pragma deprecated
# warning C4310: cast truncates constant value
MSC_WARNING_LEVEL = $(MSC_WARNING_LEVEL) /wd4214 /wd4995 /wd4310

TARGETNAME = MtlkShared
TARGETPATH = $(DRIVER_ROOT)\$(MTLK_LIB_PATH)

INCLUDES = $(INCLUDES)                              \
           $(DRIVER_ROOT)\$(MTLK_DRV_CORE_PATH);    \
           $(DRIVER_ROOT)\$(MTLK_DRV_HW_PATH);      \
           $(DRIVER_ROOT)\$(MTLK_SHARED_INC);       \
           $(DRIVER_ROOT)\$(MTLK_SHARED_MAC);

SOURCES = ..\mtlk_sq.c      \
          ..\addba.c        \
          ..\eeprom.c       \
          ..\txmm.c         \
          ..\scan.c         \
          ..\cache.c        \
          ..\channels.c     \
          ..\frame.c        \
          ..\mtlkaux.c      \
          ..\rod.c          \
          ..\stadb.c        \
          ..\dfs.c          \
          ..\aocs.c         \
          ..\command.c      \
          ..\mtlkflctrl.c   \
          ..\hw_mmb.c       \
    	  ..\iperf_debug.c
