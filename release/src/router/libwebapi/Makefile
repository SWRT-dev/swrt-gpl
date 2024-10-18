#***********************************************************************
#
# Makefile
#
#***********************************************************************
include $(LINUXDIR)/.config
include ../common.mak
include $(SRCBASE)/.config
include ../.config

ifneq ($(findstring CONFIG_BCM_KF_NETFILTER=, $(shell cat $(LINUXDIR)/.config)),)
ifneq ($(RTCONFIG_HND_ROUTER_AX_6756),y)
export CFLAGS += -DBCM_KF_NETFILTER
endif
endif

OBJS = webapi.o

OBJS += $(if $(wildcard priv_webapi.c), priv_webapi.o, prebuild/priv_webapi.o)

CFLAGS += $(if $(RTCONFIG_DEBUG),-g,-Os) -Wall $(EXTRACFLAGS) -fPIC
CFLAGS += -I. -I$(TOP)/shared -I$(SRCBASE)/include
CFLAGS += -I$(STAGEDIR)/usr/include/json-c

ifeq ($(RTCONFIG_BWDPI),y)
CFLAGS += -I$(TOP)/sqlite
CFLAGS += -I$(TOP)/bwdpi_source/src/tdts_udb_sample
CFLAGS += -I$(TOP)/bwdpi_source/src/tdts_udb_sample/linux
CFLAGS += -I$(TOP)/bwdpi_source/include
CFLAGS += -I$(TOP)/bwdpi_source/asus_include
endif
CFLAGS += -I$(TOP)/httpd

LDFLAGS += -L$(STAGEDIR)/usr/lib -ljson-c
LDFLAGS += -L$(TOP)/shared -lshared

ifeq ($(RTCONFIG_BWDPI),y)
LDFLAGS += -L$(TOP)/bwdpi_source/asus -lbwdpi
LDFLAGS += -L$(TOP)/bwdpi_source/asus_sql -lbwdpi_sql
LDFLAGS += -L$(TOP)/sqlite/.libs -lsqlite3
LDFLAGS += -lpthread
endif

ifeq ($(RTCONFIG_BCMARM),y)
CFLAGS += -I$(SRCBASE)/shared/bcmwifi/include -DTYPEDEF_FLOAT_T
endif

ifeq ($(RTCONFIG_CFGSYNC),y)
CFLAGS += -I$(TOP)/shared/sysdeps/amas
CFLAGS += -I$(TOP)/cfg_mnt
LIBS += -L$(TOP)/cfg_mnt -lcfgmnt
endif

ifeq ($(RTCONFIG_HTTPS),y)
CFLAGS += -I$(TOP)/mssl
CFLAGS += -I$(TOP)/openssl/include
LIBS += -L$(TOP)/mssl -lmssl
LIBS += -L$(TOP)/openssl -lssl -lcrypto -ldl
endif

ifeq ($(RTCONFIG_USB),y)
CFLAGS += -I$(TOP)/libdisk
LIBS += -L$(TOP)/libdisk -ldisk
ifeq ($(RTCONFIG_USB_SMS_MODEM),y)
CFLAGS += -I$(TOP)/smspdu
LIBS += -L$(TOP)/smspdu -lsmspdu
endif
endif

all: libwebapi.so

# handle prebuilt object here
ifneq ($(wildcard, ./prebuild/$(BUILD_NAME)),)
priv_webapi.o:
	@-cp -f ./prebuild/$(BUILD_NAME)/priv_webapi.o ./prebuild
	@-cp -f ./prebuild/$(BUILD_NAME)/priv_webapi.o .
endif

.c.o:
	$(CC) -c $*.c $(CFLAGS)

libwebapi.so: $(OBJS)
	$(CC) -shared $(OBJS) $(LDFLAGS) -o libwebapi.so

.PHONY: clean
clean:
	@echo LINUXDIR=$(LINUXDIR)
	-rm -rf libwebapi.so webapi.o $(OBJS)

