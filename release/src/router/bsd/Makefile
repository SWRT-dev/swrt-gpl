#
# Linux acsd Makefile
#
# $Copyright Broadcom Corporation$
#
# $Id: Makefile 401201 2013-05-09 03:06:42Z kenlo $
#

include $(SRCBASE)/router/.config

CFLAGS	+= -I. -I$(TOP)/shared -I$(SRCBASE)/include -I$(SRCBASE)/common/include -Wall 
CFLAGS	+= $(if $(WLAN_ComponentIncPath),$(WLAN_ComponentIncPath),$(addprefix -I,$(wildcard $(SRCBASE)/shared/bcmwifi/include)))
ifeq ($(HND_ROUTER_AX), y)
CFLAGS  += -I$(SRCBASE)/../components/math/include -I$(SRCBASE)/../components/proto/include -I$(SRCBASE)/../components/wlioctl/include
CFLAGS  += -I$(SRCBASE)/wl -I$(SRCBASE)/wl/sys
endif
#CFLAGS	+= -g -DDEBUG
CFLAGS	+= -s -O2
LDFLAGS += -L$(TOP_PLATFORM)/nvram$(BCMEX)$(EX7) -lnvram ${EXTRA_NV_LDFLAGS}
LDFLAGS += -L$(TOP)/shared -L$(INSTALLDIR)/shared/usr/lib -lshared
LDFLAGS += $(EXTRALDFLAGS)

#ifdef BCMINTERNAL
ifneq ($(NO_BCMINTERNAL), 1)
CFLAGS	+= -DBCMINTERNAL
endif
CFLAGS	+= -DBCMDBG
#endif

ifeq ($(RTCONFIG_AMASDB),y)
CFLAGS += -DAMASDB
endif

vpath %.c $(SRCBASE)/shared $(WLAN_ComponentSrcDirs)
vpath %.c $(SRCBASE)/shared/bcmwifi/src

ifeq ($(wildcard $(SRCBASE)/router/bsd/*.c),)
all:
ifeq ($(RTCONFIG_BCM7),y)
	-cp ./prebuilt/bsd_7 ./bsd
else ifeq ($(RTCONFIG_BCM_7114),y)
	-cp ./prebuilt/bsd_7114 ./bsd
else
	-cp ./prebuilt/bsd_6 ./bsd
endif
else
all: bsd
endif

OBJS = bsd_main.o bsd.o bsd_engine.o  bsd_util.o bsd_comm.o bcmwifi_channels.o bsd_default.o

install: all
	install -d $(INSTALLDIR)/usr/sbin/
	install bsd $(INSTALLDIR)/usr/sbin/
	$(STRIP) $(INSTALLDIR)/usr/sbin/bsd

clean:
	rm -f *.o *.a *.so bsd

${OBJS}: bsd.h

bsd: bcmutils.o ${OBJS}
	$(CC) $(CFLAGS) $^ -o $@  $(LDFLAGS)

