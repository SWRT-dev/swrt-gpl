#
# $Id: $
#

DRIVER_ROOT=..\..\..\..\wireless\driver
!include $(DRIVER_ROOT)\wpath.inc

TARGETTYPE = LIBRARY

TARGETNAME = BCL_LIB
TARGETPATH = $(DRIVER_ROOT)\$(MTLK_LIB_PATH)

# warning C4100: '...' : unreferenced formal parameter
# warning C4214: nonstandard extension used : bit field types other than int
# warning C4995: 'strcat' etc : name was marked as #pragma deprecated
MSC_WARNING_LEVEL = $(MSC_WARNING_LEVEL) /wd4100 /wd4214 /wd4995

SOURCES = ..\mt_bcl.c          \
          ..\mt_bcl_funcs.c    \
          ..\mt_util.c         \
          ..\mt_wapi.c         \
          ..\mt_wchacc.c       \
          ..\registry.c
