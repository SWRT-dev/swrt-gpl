include ../.config

EXEC = networkmap
OBJS = JSON_checker.o function.o connections.o sm.o download.o shared_func.o protect_name.o networkmap.o

ifeq ($(HND_ROUTER),y)
CFLAGS += -Werror=return-type -Werror=uninitialized -Wno-date-time 
endif
CFLAGS  += -I. -I$(TOP)/shared -I$(TOP)/kernel_header/include -I$(SRCBASE)/include -I$(STAGEDIR)/usr/include/json-c
CFLAGS  += $(if $(RTCONFIG_DEBUG),,-s) -O2 -DNO_PARALLEL
CFLAGS  += -I$(TOP)/libasc
CFLAGS  += -I$(TOP)/curl/include

ifeq ($(RTCONFIG_RALINK),y)
CFLAGS += -I$(TOP)/shared/sysdeps/ralink -I$(TOP)/wireless_tools$(WTOOL_SUFFIX)
endif

ifeq ($(RTCONFIG_QCA),y)
CFLAGS += -I$(TOP)/shared/sysdeps/qca -I$(TOP)/wireless_tools$(WTOOL_SUFFIX)
CFLAGS += $(if $(RTCONFIG_SAMBA36X),-DSMP)
endif

ifeq ($(CONFIG_REALTEK),y)
CFLAGS += -I$(TOP)/shared/sysdeps/realtek
endif

ifeq ($(RTCONFIG_ALPINE),y)
CFLAGS += -DALPINE_WPS
CFLAGS += -I$(TOP)/httpd/sysdeps -I$(TOP)/shared/sysdeps/alpine -I$(TOP)/wireless_tools$(WTOOL_SUFFIX)
endif

ifeq ($(RTCONFIG_LANTIQ),y)
CFLAGS += -DLANTIQ_WPS
CFLAGS += -I$(TOP)/shared/sysdeps/lantiq -I$(TOP)/wireless_tools$(WTOOL_SUFFIX)
endif
ifeq ($(RTCONFIG_BCMARM),y)
CFLAGS += -I$(SRCBASE)/shared/bcmwifi/include -DTYPEDEF_FLOAT_T
CFLAGS += -I$(SRCBASE)/common/include
#LDFLAGS += -L$(TOP)/libbcm -lbcm
LDFLAGS += -lgcc_s

ifeq ($(HND_ROUTER_BE_4916), y)
CFLAGS+= -fcommon
endif
endif

LDFLAGS += -L$(TOP)/curl/lib/.libs -lcurl -L$(TOP)/
LDFLAGS += -L$(TOP)/libasc -lasc

ifeq ($(RTCONFIG_LANTIQ),y)
LDFLAGS += -L$(TOP)/fapi_wlan_common-05.04.00.131 -lfapiwlancommon -ldl -L$(TOP)/libhelper-1.4.0.2 -lhelper -L$(TOP)/libfapi-0.1 -lfapi
endif

ifeq ($(HND_ROUTER_AX), y)
CFLAGS += -I$(SRCBASE)/../components/proto/include -I$(SRCBASE)/../components/wlioctl/include -I$(SRCBASE)/wl -I$(SRCBASE)/wl/sys
endif

ifeq ($(RTCONFIG_BWDPI),y)
CFLAGS += -I$(TOP)/sqlite -I$(TOP)/httpd
CFLAGS += -I$(TOP)/bwdpi_source/src/tdts_udb_sample
CFLAGS += -I$(TOP)/bwdpi_source/src/tdts_udb_sample/linux
CFLAGS += -I$(TOP)/bwdpi_source/include
CFLAGS += -I$(TOP)/bwdpi_source/asus_include
endif
ifeq ($(RTCONFIG_BWDPI_DEP),y)
CFLAGS += -I$(TOP)/httpd
CFLAGS += -I$(TOP)/bwdpi_dep/bwdpi
endif

CFLAGS += -I$(TOP)/openssl/include
LDFLAGS += -lm -L$(TOP)/shared -lshared -L$(TOP_PLATFORM)/nvram${BCMEX} -lnvram -L$(STAGEDIR)/usr/lib -ljson-c
LDFLAGS += -L$(TOP)/shared -lshared -L$(TOP_PLATFORM)/nvram${BCMEX}${EX7} -lnvram 
LDFLAGS += -L$(TOP)/openssl -lcrypto -lssl
ifeq ($(RTCONFIG_OPENSSL3),y)
LDFLAGS += -latomic
endif

ifeq ($(RTCONFIG_BCM_502L07P2),y)
LDFLAGS += -L$(TOP_PLATFORM)/ethctl_lib -lethctl
endif
ifeq ($(HND_ROUTER),y)
LDFLAGS += -L$(TOP_PLATFORM)/wlcsm$(EXHND) -lwlcsm
endif

ifeq ($(RTCONFIG_BWDPI),y)
LDFLAGS += -L$(TOP)/bwdpi_source/asus -lbwdpi
endif
ifeq ($(RTCONFIG_BWDPI_DEP),y)
LDFLAGS += -L$(TOP)/bwdpi_dep/bwdpi -lbwdpi
endif

ifeq ($(RTCONFIG_NOTIFICATION_CENTER),y)
CFLAGS  += -I$(TOP)/sqlite
CFLAGS  += -I$(TOP)/nt_center/lib
CFLAGS  += -DASUSWRT_SDK
LDFLAGS += -L$(TOP)/nt_center/lib/ -lnt -lm
LDFLAGS += -L$(TOP)/sqlite/.libs -lsqlite3
LDFLAGS += -lpthread
endif

ifeq ($(RTCONFIG_PERMISSION_MANAGEMENT),y)
CFLAGS += -I$(TOP)/sqlCipher
CFLAGS += -I$(TOP)/PMS_DBapis
CFLAGS += -I$(TOP)/openssl/include/openssl
LDFLAGS += -L$(TOP)/openssl -lcrypto -lssl
LDFLAGS += -L $(TOP)/sqlCipher/.libs -lsqlcipher
LDFLAGS += -L$(TOP)/PMS_DBapis -lpms_sql
endif

ifeq ($(RTCONFIG_CFGSYNC),y)
CFLAGS += -I$(TOP)/cfg_mnt
LIBS += -L$(TOP)/cfg_mnt -lcfgmnt
endif

include $(SRCBASE)/.config

CFLAGS += -DNMP_DB
CFLAGS += -DBTN_SETUP
ifeq ($(HND_ROUTER),y)
CFLAGS += -DHND_ROUTER
endif

#ifeq ($(RTCONFIG_SWRTMESH),y)
#CFLAGS += -DHOSTAPD_SUPPORT
#OBJS += hostapd.o
#endif

all: copy_prebuild $(EXEC) asusdiscovery arpstorm

copy_prebuild:
ifneq ($(wildcard prebuild/$(BUILD_NAME)/*.o),)
	-cp -f prebuild/$(BUILD_NAME)/*.o ./
endif

$(EXEC): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS) $(CFLAGS)

arpstorm: arpstorm.o
	$(CC) -o $@ $^ $(LDFLAGS) $(CFLAGS)

asusdiscovery: packet.o ASUS_Discovery.o ASUS_Discovery_Debug.o
	$(CC) -o $@ $^ $(LDFLAGS) $(CFLAGS)

demo: demo.o
	$(CC) -o $@ $^ $(LDFLAGS) $(CFLAGS)

install:
	install -D $(EXEC) $(INSTALLDIR)/usr/sbin/$(EXEC)
	$(if $(RTCONFIG_DEBUG),,$(STRIP) $(INSTALLDIR)/usr/sbin/$(EXEC))
	install -D asusdiscovery $(INSTALLDIR)/usr/sbin/asusdiscovery
	$(if $(RTCONFIG_DEBUG),,$(STRIP) $(INSTALLDIR)/usr/sbin/asusdiscovery)
	install -D arpstorm $(INSTALLDIR)/usr/sbin/arpstorm
	$(if $(RTCONFIG_DEBUG),,$(STRIP) $(INSTALLDIR)/usr/sbin/arpstorm)
	install -d $(INSTALLDIR)/usr/networkmap
	cp -f db/networkmap.oui.js $(INSTALLDIR)/usr/networkmap/networkmap.oui.js
	cp -f db/nmp_conv_type.js $(INSTALLDIR)/usr/networkmap/nmp_conv_type.js
	cp -f db/nmp_vendor_type.js $(INSTALLDIR)/usr/networkmap/nmp_vendor_type.js
	cp -f db/nmp_bwdpi_type.js $(INSTALLDIR)/usr/networkmap/nmp_bwdpi_type.js

clean:
	rm -rf *.o $(EXEC) *~ asusdiscovery arpstorm

