#
# 802.1x Authenticator (Network Access Server) Embedded Linux Build Makefile
#
# Copyright (C) 2014, Broadcom Corporation
# All Rights Reserved.
#

include ../../.config

CFLAGS	+= -I.. -I$(SRCBASE)/include ${WLAN_StdIncPathA} -I$(SRCBASE)/common/include -I$(SRCBASE)/router/shared -Wall 
CFLAGS	+= $(WLAN_ComponentIncPath)
CFLAGS	+= -I$(SRCBASE)/shared/bcmwifi/include -I$(SRCBASE)/common/include/proto

#CFLAGS	+= -g -DDEBUG
CFLAGS += -DBCMDBG
CFLAGS	+= -s
CFLAGS  += -DNAS_WKSP_BUILD_NAS_AUTH
CFLAGS  += -DNAS_WKSP_BUILD_NAS_SUPPL
#CFLAGS  += -DNAS_GTK_PER_STA
CFLAGS  += -DNAS_RADIUS
CFLAGS  += -DNAS_WKSP_ON_DEMAND
ifeq ($(RTCONFIG_IPV6),y)
CFLAGS += -DNAS_IPV6
endif

CFLAGS	+= -DBCMSUPPL

# EAPD include path
CFLAGS	+= -I$(SRCBASE)/router/eapd${BCMEX}$(EX7)

LDFLAGS += -L$(TOP_PLATFORM)/nvram${BCMEX}${EX7} -L$(INSTALLDIR)/nvram/usr/lib -lnvram ${EXTRA_NV_LDFLAGS}
LDFLAGS += -L$(TOP)/shared -L$(INSTALLDIR)/shared/usr/lib -lshared
LDFLAGS += -L$(TOP)/libbcmcrypto -L$(INSTALLDIR)/libbcmcrypto/usr/lib -lbcmcrypto
LDFLAGS += -lgcc_s

ifeq ($(RTCONFIG_QTN),y)
LDFLAGS += -L$(TOP)/libqcsapi_client -lqcsapi_client
endif

vpath %.c $(SRCBASE)/router/nas${BCMEX}$(EX7) $(SRCBASE)/shared $(SRCBASE)/router/shared

# build target
NASOBJS = mppe.o wpa.o nas.o nas_linux.o nas_wl.o nas_wksp.o
NASOBJS += nas_radius.o nas_wksp_radius.o
UTLOBJS = bcmwpa.o bcmutils.o

ifeq ($(wildcard ../*.c), )
all:
ifeq ($(RTCONFIG_BCM_7114),y)
	-cp ./prebuilt/nas_7114 ./nas
else ifeq ($(RTCONFIG_BCM7),y)
	-cp ./prebuilt/nas_7 ./nas
else
	-cp ./prebuilt/nas_6 ./nas
endif
else
all: nas
endif

nas: $(NASOBJS) $(UTLOBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

# install to target
install: all
	install -D nas $(INSTALLDIR)/usr/sbin/nas
	$(STRIP) $(INSTALLDIR)/usr/sbin/nas
	cd $(INSTALLDIR)/usr/sbin && ln -sf nas nas4not && ln -sf nas nas4wds

# clean up mess
clean:
	rm -f *.o *~ nas
