#
# Linux acsd Makefile
#
# Copyright (C) 2014, Broadcom Corporation
# All Rights Reserved.
# 
# This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
# the contents of this file may not be disclosed to third parties, copied
# or duplicated in any form, in whole or in part, without the prior
# written permission of Broadcom Corporation.
#
# $Id: Makefile 403251 2013-05-20 05:15:55Z $
#

CFLAGS	+= -I. -I$(TOP)/shared -I$(SRCBASE)/include ${WLAN_StdIncPathA} -I$(SRCBASE)/common/include -Wall
CFLAGS	+= $(if $(WLAN_ComponentIncPath),$(WLAN_ComponentIncPath),$(addprefix -I,$(wildcard $(SRCBASE)/shared/bcmwifi/include)))
#CFLAGS	+= -g -DDEBUG
#CFLAGS	+= -DACS_DEBUG
CFLAGS	+= -s -O2
LDFLAGS += -L$(TOP_PLATFORM)/nvram${BCMEX}${EX7} -L$(INSTALLDIR)/nvram/usr/lib -lnvram ${EXTRA_NV_LDFLAGS}
LDFLAGS += -L$(TOP)/shared -L$(INSTALLDIR)/shared/usr/lib -lshared
LDFLAGS += -lgcc_s

include ../.config

vpath %.c $(SRCBASE)/shared/ $(SRCBASE)/shared/bcmwifi/src

ifeq ($(wildcard $(SRCBASE)/router/acsd_arm/*.c),)
all:
ifeq ($(or $(RTAC3200),$(SBRAC3200P)),y)
	-cp ./prebuilt/acsd_7 ./acsd
else ifeq ($(or $(RTAC68U)),y)
	-cp ./prebuilt/acsd_6 ./acsd
else
	-cp ./prebuilt/acsd_7114 ./acsd
endif
else
all: acsd
endif

OBJS = bcmwifi_channels.o dcs.o acs.o acsd_utils.o acsd_main.o acsd_cmd.o acsd_chanim.o
OBJS += acs_dfsr.o chan_utils.o

install: all
	install -d $(INSTALLDIR)/usr/sbin/
	install acsd $(INSTALLDIR)/usr/sbin/
	#install acs_cli $(INSTALLDIR)/usr/sbin/
	$(STRIP) $(INSTALLDIR)/usr/sbin/acsd
	#$(STRIP) $(INSTALLDIR)/usr/sbin/acs_cli

clean:
	rm -f *.o *.a *.so acsd acs_cli

${OBJS}: acsd.h acsd_svr.h acs_dfsr.h

acsd: bcmutils.o bcmxtlv.o bcm_app_utils.o ${OBJS}
	$(CC) $(CFLAGS) $^ -o $@  $(LDFLAGS)

acs_cli: acsd_cli.o acsd_utils.o
	$(CC) $(CFLAGS) $^ -o $@  $(LDFLAGS)
