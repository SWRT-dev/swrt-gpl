!IF 0

Module Name:
    makefile.

Id:
	$Id$

!ENDIF

!if "$(_TGTOS)"=="CE"

!IF "$(WINCEDEBUG)" == "debug"
C_DEFINES=$(C_DEFINES) -D_DEBUG=1 
!ENDIF

C_DEFINES=$(C_DEFINES) -D_WINCEOSVER=$(_WINCEOSVER)

!INCLUDE $(_MAKEENVROOT)\makefile.def

!else
!   message Unknown Target Environment: $(_TGTOS)
!endif

