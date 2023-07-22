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

WLU_OBJS := wlu.o wlu_cmd.o wlu_iov.o wlu_linux.o bcmutils.o wlu_client_shared.o wlu_pipe.o wlu_pipe_linux.o miniopt.o bcmxtlv.o bcm_app_utils.o wlu_rates_matrix.o wlc_ppr.o
WLU_OBJS += wluc_cac.o wluc_traffic_mgmt.o wluc_bmac.o wluc_ltecx.o wluc_phy.o wluc_wnm.o wluc_keymgmt.o wluc_interfere.o
WLU_OBJS += wluc_relmcast.o
WLU_OBJS += wluc_rrm.o
WLU_OBJS += wluc_wowl.o
WLU_OBJS += wluc_pkt_filter.o
WLU_OBJS += wluc_mfp.o
WLU_OBJS += wluc_ota_test.o
WLU_OBJS += wluc_bssload.o
WLU_OBJS += wluc_stf.o
WLU_OBJS += wluc_offloads.o
WLU_OBJS += wluc_tpc.o
WLU_OBJS += wluc_toe.o
WLU_OBJS += wluc_arpoe.o
WLU_OBJS += wluc_keep_alive.o
WLU_OBJS += wluc_ap.o
WLU_OBJS += wluc_ampdu.o
WLU_OBJS += wluc_ampdu_cmn.o
WLU_OBJS += wluc_bmac.o
WLU_OBJS += wluc_ht.o
WLU_OBJS += wluc_wds.o
WLU_OBJS += wluc_keymgmt.o
WLU_OBJS += wluc_scan.o
WLU_OBJS += wluc_obss.o
WLU_OBJS += wluc_prot_obss.o
WLU_OBJS += wluc_lq.o
WLU_OBJS += wluc_seq_cmds.o
WLU_OBJS += wluc_btcx.o
WLU_OBJS += wluc_led.o
WLU_OBJS += wluc_interfere.o
ifneq ($(wildcard $(LINUXDIR)/brcm/arm/wl/exe/wlu_common.c),)
WLU_OBJS += wlu_common.o
endif

ifeq ($(RWL),1)
WL_OBJS := $(WLU_OBJS)

WL_OBJS := $(WL_OBJS:%.o=wl_obj/%.o) 
WL_EXE  := wl
endif
UTILS := epi_ttcp
CFLAGS += -s -I$(LINUXDIR)/brcm/arm/include -I$(LINUXDIR)/brcm/arm/common/include
CFLAGS += $(if $(WLAN_ComponentIncPath),$(WLAN_ComponentIncPath),$(addprefix -I,$(wildcard $(LINUXDIR)/brcm/arm/shared/bcmwifi/include)))
ifeq ($(R7000P),y)
CFLAGS +=  -I$(LINUXDIR)/brcm/arm/wl/ppr/include
endif
ifeq ($(CONFIG_RTR_OPTIMIZE_SIZE),y)
CFLAGS  += -Os
else
CFLAGS  += -O2
endif
CFLAGS += -DBCMINTERNAL -DBCMDBG

CFLAGS += -DLINUX -I$(LINUXDIR)/brcm/arm/wl/exe

# Use newer 11ac ratespec for wl command line
CFLAGS += -DD11AC_IOTYPES -DPPR_API

#ifdef WLCNT
CFLAGS += -DWLCNT
#endif

ifeq ($(CONFIG_TRAFFIC_MGMT),y)
CFLAGS += -DTRAFFIC_MGMT
endif

vpath %.c $(SRCBASE)/tools/misc $(WLAN_ComponentSrcDirs) $(LINUXDIR)/brcm/arm/wl/ppr/src

UTILS += et
ifneq ($(wildcard $(LINUXDIR)/brcm/arm/et/linux/et.c),)
vpath %.c $(LINUXDIR)/brcm/arm/et/linux
endif
ifneq ($(wildcard $(LINUXDIR)/brcm/arm/wl/exe),)
CFLAGS += -I$(LINUXDIR)/brcm/arm/wl/exe
UTILS += wl
vpath %.c $(LINUXDIR)/brcm/arm/wl/exe $(LINUXDIR)/brcm/arm/shared
endif

CFLAGS += -I$(TOP)/shared
LDFLAGS += -L$(TOP)/nvram${BCMEX} -lnvram
LDFLAGS += -L$(TOP)/shared -L$(INSTALLDIR)/shared/usr/lib -lshared
LDFLAGS += -lgcc_s
ifeq ($(RTCONFIG_5301X),y)
CFLAGS += -DBCM5301X
endif
UTILS += robocfg


all: $(UTILS)

ifeq ($(wildcard $(LINUXDIR)/brcm/arm/wl/exe/*.c),)
wl:

else
wl: $(WLU_OBJS)
	$(CC) $^ -o $@ $(LDFLAGS)
endif

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
