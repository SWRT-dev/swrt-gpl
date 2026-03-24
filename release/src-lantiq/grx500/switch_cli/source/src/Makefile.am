#
#                              Copyright (c) 2010
#                           Lantiq Deutschland GmbH
#                     Am Campeon 3; 85579 Neubiberg, Germany
#
#  For licensing information, see the file 'LICENSE' in the root folder of
#  this software module.
#

bin_PROGRAMS = switch_cli
switch_cli_prgdir=../

#todo: include Makefile.inc

# nodist_switch_cli_SOURCES =

AM_CFLAGS = -Wall

switch_cli_SOURCES = @srcdir@/swapi_cli.c \
   @srcdir@/ltq_cli_lib.c \
   @srcdir@/gsw_cli_fkts.c \
   @srcdir@/gsw_cli_fkts_common.c \
   @srcdir@/gsw_tbl.c \
   @srcdir@/gsw_cli_fkts_flow.c  \
   @srcdir@/gsw_cli_mac.c \
   @srcdir@/gsw_cli_lmac.c \
   @srcdir@/gsw_cli_adap.c 


#if SWAPI_PAE_SUPPORT
#	switch_cli_SOURCES += @srcdir@/gsw_cli_pae.c
#	AM_CFLAGS += -DSWAPI_PAE_SUPPORT
#endif

if CLI_DUMP_CALL
   AM_CFLAGS += -DCLI_DUMP_CALL
   switch_cli_SOURCES += @srcdir@/ltq_cli_dump_call.c
endif

EXTRA_DIST = \
   @srcdir@/ltq_cli_lib.h \
   @srcdir@/gsw_cli_fkts.h \
   @srcdir@/gsw_tbl.h \
   @srcdir@/ltq_cli_lib.c \
   @srcdir@/ltq_cli_dump_call.c \
   @srcdir@/gsw_cli_fkts.c \
   @srcdir@/gsw_cli_mac.c \
   @srcdir@/gsw_cli_lmac.c \
   @srcdir@/gsw_cli_adap.c \
   @srcdir@/gsw_cli_fkts_common.c \
   @srcdir@/gsw_cli_fkts_flow.c \
   @srcdir@/gsw_tbl.c \
   @srcdir@/swapi_cli.c


INCLUDES = \
   -I@srcdir@ \
   -I@abs_top_builddir@ \
   -I@top_srcdir@ \
   -I@srcdir@/../include \
   -I@SYSTEM_INCL_PATH@

lint:
	@flint @srcdir@/../std_lx.lnt $(INCLUDES) \
	$(switch_cli_SOURCES)

rsm:
	rsm -fa -n -s -Ta $(switch_cli_SOURCES) $(EXTRA_DIST)

install-exec-hook: $(bin_PROGRAMS)
	@for p in $(bin_PROGRAMS); do \
		$(STRIP) $(DESTDIR)$(bindir)/$$p; \
		echo "  $(STRIP) $(DESTDIR)$(bindir)/$$p"; \
	done
