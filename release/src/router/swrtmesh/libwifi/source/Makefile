CC ?= gcc
WIFI_TYPE ?= TEST

CFLAGS += -O2 -g -Wall -Werror
include $(TOP)/.config
CFLAGS += $(TARGET_CFLAGS) -Wall -I./
CFLAGS += -I$(STAGEDIR)/usr/include -I$(STAGEDIR)/usr/include/libnl3 -I$(STAGEDIR)/usr/include/openssl -I$(TOP)/shared -I$(SRCBASE)/include
LDFLAGS += -L$(STAGEDIR)/usr/lib
LDFLAGS += -L$(TOP)/shared -L$(TOP)/nvram -lshared -lnvram
ifeq ($(RTCONFIG_RALINK),y)
CFLAGS += -I$(TOP)/shared/sysdeps/ralink
else ifeq ($(RTCONFIG_QCA),y)
CFLAGS += -I$(TOP)/shared/sysdeps/qca
else ifeq ($(RTCONFIG_LANTIQ),y)
CFLAGS += -I$(TOP)/shared/sysdeps/lantiq
endif
CFLAGS += -I$(TOP)/wireless_tools$(WTOOL_SUFFIX)/ -DWE_NOLIBM

LIBWIFI_CFLAGS += -I.
LIBWIFI_LDFLAGS += -L.

LIBS += -lnl-3 -lnl-route-3 -lnl-genl-3
LIBS += -leasy

ver=$(shell ./genversion.sh)
maj=$(word 1,$(ver))
min=$(word 2,$(ver))
rev=$(word 3,$(ver))
version=$(maj).$(min).$(rev)

LIBWIFI_CFLAGS += -DLIBWIFI_MAJOR=$(maj) -DLIBWIFI_MINOR=$(min) -DLIBWIFI_REV=$(rev)

objs_lib = debug.o wifi.o chlist.o drivers.o wifiutils.o
objs_libutil = wifiutils.o
objs_dir =

ifneq  (,$(findstring BROADCOM,$(WIFI_TYPE)))
objs_dir += broadcom
LIBWIFI_CFLAGS += -Imodules/wpactrl -DWIFI_BROADCOM
objs_lib += modules/wpactrl/wpactrl_util.o
ifneq ($(filter -DCONFIG_BCM963138,$(CFLAGS)),)
objs_lib += modules/broadcom/bcm.o
else
LIBWIFI_CFLAGS += -Imodules/nlwifi -Imodules/wext -DWIFI_BROADCOM
objs_lib += modules/nlwifi/nlwifi.o \
	    modules/broadcom/brcm.o \
	    modules/broadcom/wlctrl.o
endif
endif

ifneq (,$(findstring MEDIATEK,$(WIFI_TYPE)))
objs_dir += mediatek
LIBWIFI_CFLAGS += -DWIFI_MEDIATEK
ifneq ($(filter -DMTWIFI,$(CFLAGS)),)
LIBWIFI_CFLAGS += -Imodules/wpactrl -Imodules/nlwifi -Imodules/wext
objs_dir += wpactrl nlwifi
objs_lib += modules/mediatek/mtwifi.o modules/wpactrl/wpactrl_util.o modules/nlwifi/nlwifi.o
else
objs_lib += modules/mediatek/mt.o
endif
endif

ifneq (,$(findstring QUALCOMM,$(WIFI_TYPE)))
objs_dir += qualcomm
LIBWIFI_CFLAGS += -DWIFI_QUALCOMM
objs_lib += modules/qualcomm/qca.o
endif

ifneq (,$(findstring QCA,$(WIFI_TYPE)))
objs_dir += qualcomm
LIBWIFI_CFLAGS += -DWIFI_QUALCOMM
objs_lib += modules/qualcomm/ipq.o
LIBWIFI_CFLAGS += -Imodules/wpactrl -Imodules/nlwifi -Imodules/wext
objs_dir += wpactrl nlwifi
objs_lib += modules/wpactrl/wpactrl_util.o modules/nlwifi/nlwifi.o
endif

ifneq (,$(findstring IPQ40XX,$(WIFI_TYPE)))
objs_dir += qualcomm
LIBWIFI_CFLAGS += -DWIFI_QUALCOMM
objs_lib += modules/qualcomm/ipq40xx.o
objs_lib += modules/qualcomm/wpactrl_util.o
endif

ifneq (,$(findstring INTEL,$(WIFI_TYPE)))
objs_dir += intel
LIBWIFI_CFLAGS += -Imodules/nlwifi -Imodules/wext -DWIFI_INTEL
objs_lib += modules/wext/wext.o \
	    modules/nlwifi/nlwifi.o \
	    modules/intel/intel.o
endif

ifneq (,$(findstring TEST,$(WIFI_TYPE)))
CFLAGS += -DWIFI_TEST -I/usr/include/libnl3 -DHAS_WIFI
objs_dir += test
objs_lib += modules/test/test.o
endif

ifneq (,$(findstring MAC80211,$(WIFI_TYPE)))
objs_dir += mac80211 wpactrl nlwifi
LIBWIFI_CFLAGS += -I/usr/include/libnl3 -Imodules/wpactrl -Imodules/nlwifi -Imodules/wext -DWIFI_MAC80211
objs_lib += modules/wpactrl/wpactrl_util.o \
	    modules/nlwifi/nlwifi.o \
	    modules/mac80211/mac80211.o
endif

ifneq (,$(findstring HAS_WIFI,$(CFLAGS)))
all: libwifiutils.so libwifi-7.so.$(version)
else
all: libwifiutils.so
endif

%.o: %.c
	$(CC) $(CFLAGS) $(LIBWIFI_CFLAGS) -fPIC -c -o $@ $<

libwifiutils.so: $(objs_libutil)
	$(CC) $(CFLAGS) $(LIBWIFI_CFLAGS) $(EXTRA_CFLAGS) \
		$(LDFLAGS) $(LIBWIFI_LDFLAGS) -shared \
		-Wl,-soname,libwifiutils.so -o $@ $^ $(LIBS)

libwifi-7.so.$(version): $(objs_lib)
	$(CC) $(CFLAGS) $(LIBWIFI_CFLAGS) $(EXTRA_CFLAGS) \
		$(VFLAGS) $(LDFLAGS) $(LIBWIFI_LDFLAGS) -shared \
		-Wl,-soname,libwifi-7.so.$(maj) -o $@ $^ $(LIBS)
	-ln -sf $@ libwifi-7.so.$(maj)
	-ln -sf $@ libwifi-7.so

ifneq (,$(findstring HAS_WIFI,$(CFLAGS)))
header-files=wifidefs.h wifiutils.h wifiops.h wifi.h
else
header-files=wifidefs.h wifiutils.h
library-files=libwifiutils.so
endif

-include Makefile.inc

install: install-headers install-libs

docs:
	cd docs; doxygen ./Doxyfile
	$(MAKE) -C docs/latex
	cp docs/latex/refman.pdf docs/libwifi.pdf

clean:
	for i in $(objs_dir); do rm -f modules/$$i/*.o; done
	rm -f *.o *.so*

.PHONY: all docs clean install
