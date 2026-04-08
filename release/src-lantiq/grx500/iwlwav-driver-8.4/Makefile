# Convert swrt build environment variable to sdk
include ../sdk.config
include ../rules.mk
PKG_BUILD_DIR=$(shell pwd)/source

EXTRA_CFLAGS += -fno-pic -mno-abicalls
TARGET_CFLAGS += -fno-pic -mno-abicalls
TARGET_CXXFLAGS += -fno-pic -mno-abicalls
TARGET_ASFLAGS_DEFAULT += -fno-pic -mno-abicalls
TARGET_CPPFLAGS += -fno-pic -mno-abicalls

CONFIG_FILE:=GRX350_GRX550_WAVE600_CONFIG

IFX_CFLAGS += $(TARGET_CFLAGS) -O2 -D_FORTIFY_SOURCE=2 -Wformat -Wformat-security -Werror=format-security -Wall -fPIE -Wl,-strip-debug

CONFIG_SUFFIX:=
ifeq ($(DEBUG),y)
IFX_CFLAGS += -DCONFIG_WAVE_DEBUG
CONFIG_SUFFIX:=-debug
endif

INSTALLKMODDIR:=$(INSTALLDIR)/lib/modules/$(LINUX_KERNEL)


all:
	@echo "Config files use: $(CONFIG_FILE)"
	sed -i -e '1s/.*/BACKPORTS_VERSION="v$(LINUX_VERSION)"/' $(PKG_BUILD_DIR)/versions
	$(CP) $(CONFIG_FILE)$(CONFIG_SUFFIX) $(PKG_BUILD_DIR)/defconfigs/iwlwav
	$(MAKE) -C $(PKG_BUILD_DIR) CFLAGS="" defconfig-iwlwav KLIB_BUILD=$(LINUXDIR) KLIB=$(INSTALLKMODDIR) CC=gcc
	$(MAKE) $(PARALLEL_BUILD) -C $(PKG_BUILD_DIR) CFLAGS="$(IFX_CFLAGS)" CROSS_COMPILE=$(CROSS_COMPILE) ARCH=$(ARCH) KLIB_BUILD=$(LINUXDIR) KLIB=$(INSTALLKMODDIR) UGW_DRIVER_VERSION="6.1.0.c29bb528e5095314f555101810aa5d0763cef1c9"
	touch stamp-h1

install:
	@[ -d $(INSTALLKMODDIR) ] || install -d $(INSTALLKMODDIR)
	@install -D $(PKG_BUILD_DIR)/compat/compat.ko $(INSTALLKMODDIR)
	@install -D $(PKG_BUILD_DIR)/net/mac80211/mac80211.ko $(INSTALLKMODDIR)
	@install -D $(PKG_BUILD_DIR)/net/wireless/cfg80211.ko $(INSTALLKMODDIR)
	@install -D $(PKG_BUILD_DIR)/drivers/net/wireless/intel/iwlwav/mtlk.ko $(INSTALLKMODDIR)
	@install -D $(PKG_BUILD_DIR)/drivers/net/wireless/intel/iwlwav/tools/mtlkroot/mtlkroot.ko $(INSTALLKMODDIR)
	@$(STRIPX) $(INSTALLKMODDIR)/*.ko
	#echo -e "iwlwav_driver_hash=\"c29bb528e5095314f555101810aa5d0763cef1c9\"\niwlwav_driver_tag=\"c29bb528e5095314f555101810aa5d0763cef1c9\"" > $(INSTALLKMODDIR)/etc/iwlwav_driver.ver

clean:

stage:


