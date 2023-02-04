#
# Miscellaneous utilities Makefile
#
# Copyright (C) 2009, Broadcom Corporation
# All Rights Reserved.
# 
# THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
# KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
# SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
# FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
#
# $Id: Makefile 382640 2013-02-03 04:35:49Z $
#

include $(SRCBASE)/router/.config


UTILS := epi_ttcp
CFLAGS += -s -I$(SRCBASE)/include -I$(SRCBASE)/common/include
CFLAGS += $(if $(WLAN_ComponentIncPath),$(WLAN_ComponentIncPath),$(addprefix -I,$(wildcard $(SRCBASE)/shared/bcmwifi/include)))
ifeq ($(CONFIG_RTR_OPTIMIZE_SIZE),y)
CFLAGS  += -Os
else
CFLAGS  += -O2
endif
CFLAGS += -DBCMINTERNAL -DBCMDBG

CFLAGS += -DLINUX -I$(SRCBASE)/wl/exe

# Use newer 11ac ratespec for wl command line
CFLAGS += -DD11AC_IOTYPES -DPPR_API

#ifdef WLCNT
CFLAGS += -DWLCNT
#endif

ifeq ($(CONFIG_TRAFFIC_MGMT),y)
CFLAGS += -DTRAFFIC_MGMT
endif

vpath %.c $(SRCBASE)/tools/misc $(WLAN_ComponentSrcDirs)

UTILS += et
UTILS += wl

ifeq ($(RTCONFIG_5301X),y)
CFLAGS += -DBCM5301X
endif
UTILS += robocfg


all: $(UTILS)

wl:


et:
	@echo $(CFLAGS)
	$(CC) $(CFLAGS) et.c -o $@ $(LDFLAGS)

robocfg: robocfg.c etc53xx.h

clean:
	rm -f *.o *~ $(UTILS)

install: all
	install -d $(INSTALLDIR)/usr/sbin
	install $(UTILS) $(INSTALLDIR)/usr/sbin
	$(STRIP) $(foreach file,$(UTILS),$(INSTALLDIR)/usr/sbin/$(file))
#	install -D -m 755 WlGetDriverCfg.sh $(INSTALLDIR)/usr/sbin/WlGetDriverCfg.sh
#	install -D -m 755 WlGetDriverStats.sh $(INSTALLDIR)/usr/sbin/WlGetDriverStats.sh

