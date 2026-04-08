# Convert swrt build environment variable to sdk
include ../sdk.config
include ../rules.mk
PKG_BUILD_DIR=$(shell pwd)/source

EXTRA_CFLAGS += -fno-pic -mno-abicalls
TARGET_CFLAGS += -fno-pic -mno-abicalls
TARGET_CXXFLAGS += -fno-pic -mno-abicalls
TARGET_ASFLAGS_DEFAULT += -fno-pic -mno-abicalls
TARGET_CPPFLAGS += -fno-pic -mno-abicalls

CONFIG_FILE:=COMMON_WAVE600_CONFIG
ifeq ($(CONFIG_TARGET_intel_x86_lgm),y)
 INSMOD_PRMS:="interface_index=4,2,0,6"
endif

IFX_CFLAGS += $(TARGET_CFLAGS) -O2 -D_FORTIFY_SOURCE=2 -Wformat -Wformat-security -Werror=format-security -Wall -fPIE -Wl,-strip-debug -DSWRT_PATCH -D$(SWRT_NAME)

ifeq ($(CONFIG_USE_WAVE600_2_EMULATOR),y)
IFX_CFLAGS += -DCONFIG_WAVE_NO_TX_BSS_MONITOR
endif

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
	$(MAKE) $(PARALLEL_BUILD) -C $(PKG_BUILD_DIR) CFLAGS="$(IFX_CFLAGS)" CROSS_COMPILE=$(CROSS_COMPILE) ARCH=$(ARCH) KLIB_BUILD=$(LINUXDIR) KLIB=$(INSTALLKMODDIR) UGW_DRIVER_VERSION="6.1.0.b66c17415c5fa2eed37214a0e46bee7fea8eee5a"
	touch stamp-h1

install:
	@[ -d $(INSTALLKMODDIR) ] || install -d $(INSTALLKMODDIR)
	@install -D $(PKG_BUILD_DIR)/compat/compat.ko $(INSTALLKMODDIR)
	@install -D $(PKG_BUILD_DIR)/net/mac80211/mac80211.ko $(INSTALLKMODDIR)
	@install -D $(PKG_BUILD_DIR)/net/wireless/cfg80211.ko $(INSTALLKMODDIR)
	@install -D $(PKG_BUILD_DIR)/drivers/net/wireless/intel/iwlwav/mtlk.ko $(INSTALLKMODDIR)
	@install -D $(PKG_BUILD_DIR)/drivers/net/wireless/intel/iwlwav/tools/mtlkroot/mtlkroot.ko $(INSTALLKMODDIR)
	@$(STRIPX) $(INSTALLKMODDIR)/*.ko
	#echo -e "iwlwav_driver_hash=\"b66c17415c5fa2eed37214a0e46bee7fea8eee5a\"\niwlwav_driver_tag=\"b66c17415c5fa2eed37214a0e46bee7fea8eee5a\"" > $(INSTALLKMODDIR)/etc/iwlwav_driver.ver

clean:

stage:


