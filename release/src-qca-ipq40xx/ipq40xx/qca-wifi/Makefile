# Convert swrt build environment variable to qsdk
include ../qsdk.config
include ../rules.mk
PKG_BUILD_DIR=$(shell pwd)/source

DRIVER_PATH:=$(PKG_BUILD_DIR)
# legacy header files.
FW_INCLUDE_PATH:=$(SRCBASE)/$(PLATFORM_ROUTER)/header/include
FW_COMMON_INCLUDE_PATH:=$(SRCBASE)/$(PLATFORM_ROUTER)/header/wlan/fwcommon/include
FW_INTERFACE_TOOLS_PATH:=$(SRCBASE)/$(PLATFORM_ROUTER)/header/wlan/fwcommon/fw_interface/tools/
FW_HDR_INCLUDE_PATH:=$(PKG_BUILD_DIR)/cmn_dev/fw_hdr
HALPHY_TOOLS_PATH:=$(SRCBASE)/$(PLATFORM_ROUTER)/header/wlan/halphy_tools

export BUILD_VARIANT="unified-perf"
export DRIVER_PATH
include $(DRIVER_PATH)/config.mk
ifeq ($(CONFIG_LINUX_5_4),y)
export QCA_WIFI_KERNEL_VERSION_5_4=1
endif
ifneq ($(CONFIG_TARGET_ipq)$(CONFIG_TARGET_ipq806x),)
	QCA_PLATFORM="ipq806x"
else
	QCA_PLATFORM=$(CONFIG_TARGET_ARCH_PACKAGES)
endif

QCAWLAN_MODULE_NAME:= $(notdir $(basename $(strip $(QCAWLAN_MODULE_LIST))))

ifeq ($(or $(CONFIG_GCC_USE_VERSION_7),$(CONFIG_GCC_USE_VERSION_8)),y)
export QCA_GCC_7_SUPPORT=1
endif

QCAWLAN_MAKEOPTS_EXTRAS:=SWRT_PATCH=y

PBBINS=$(wildcard prebuilt/$(BUILD_NAME)/*)
QCAWIFI_KMOD=$(filter %.ko,$(PBBINS))
QCAWIFI_LIBS=$(filter %.so,$(PBBINS))
QCAWIFI_XML=$(wildcard prebuilt/*.xml)
QCAWIFI_USR_SBIN=$(filter-out $(QCAWIFI_KMOD) $(QCAWIFI_LIBS) $(QCAWIFI_XML),$(PBBINS))

INSTALLKMODDIR:=$(INSTALLDIR)/lib/modules/$(LINUX_KERNEL)

N_DCTHREADS=$(shell grep -c '^processor' /proc/cpuinfo)

all:
ifneq ($(wildcard $(PKG_BUILD_DIR)/wow/*.c),)
	export DRIVER_PATH="$(DRIVER_PATH)"; \
	export FW_INCLUDE_PATH="$(FW_INCLUDE_PATH)"; \
	export FW_COMMON_INCLUDE_PATH="$(FW_COMMON_INCLUDE_PATH)"; \
	export FW_INTERFACE_TOOLS_PATH="$(FW_INTERFACE_TOOLS_PATH)"; \
	export FW_HDR_INCLUDE_PATH="$(FW_HDR_INCLUDE_PATH)"; \
	export HALPHY_TOOLS_PATH="$(HALPHY_TOOLS_PATH)"; \
	export LINUX_VERSION="$(LINUX_VERSION)"; \
	export LINUX_DIR="$(LINUXDIR)"; \
	export LINUX_SRC_DIR="$(LINUXDIR)"; \
	export BUILD_VARIANT="$(BUILD_VARIANT)"; \
	export TARGET_CROSS="$(CROSS_COMPILE)"; \
	export QCA_PLATFORM="$(QCA_PLATFORM)"; \
	export BIG_ENDIAN_HOST="$(BIG_ENDIAN_HOST)"; \
	export TARGET_CFLAGS="$(TARGET_CFLAGS) -DSWRT_PATCH -DRTCONFIG_MUSL_LIBC"; \
	export TARGET_LDFLAGS="$(TARGET_LDFLAGS) -lpthread -ldl"; \
	export QCA_NSS_WIFI_OFFLOAD_SUPPORT=0; \
	export CONFIG_TARGET_ipq_ipq807x_64_QSDK_Enterprise=n; \
	export CONFIG_TARGET_ipq_ipq807x_QSDK_Enterprise=n; \
	export BUILD_X86=n; \
	export LIMIT_RXBUF_LEN_4K=0; \
	export ATH_GEN_RANDOMNESS=0; \
	export DUMP_FW_RAM=0; \
	export ATH_SUPPORT_FW_RAM_DUMP_FOR_MIPS=0; \
	export QCA_TGT_FW_IRAM_DUMP_ENABLE=0; \
	export CONFIG_WIFI_TARGET_WIFI_2_0=y; \
	export CONFIG_WIFI_TARGET_WIFI_3_0=y; \
	export CONFIG_WIFI_MEM_DEBUG=n; \
	export CONFIG_WIFI_EMULATION_WIFI_3_0=n; \
	export CONFIG_WIFI_LOG_UTILS=n; \
	export CONFIG_PLD_STUB=n; \
	export ENDIAN="AH_LITTLE_ENDIAN"; \
	export CONFIG_64BIT="$(CONFIG_64BIT)"; \
	export QCAWLAN_MAKEOPTS_EXTRAS="$(QCAWLAN_MAKEOPTS_EXTRAS)"; \
	$(TARGET_MAKE) -C $(PKG_BUILD_DIR) src_prepare all_build N_DCTHREADS=$(N_DCTHREADS) $(TARGET_CONFIGURE_OPTS)
endif
	touch stamp-h1

install:
	@[ -d $(INSTALLDIR)/ini ] || install -d $(INSTALLDIR)/ini
	@[ -d $(INSTALLDIR)/sbin ] || install -d $(INSTALLDIR)/sbin
	@[ -d $(INSTALLDIR)/usr/sbin ] || install -d $(INSTALLDIR)/usr/sbin
	@[ -d $(INSTALLDIR)/usr/lib ] || install -d $(INSTALLDIR)/usr/lib
	@[ -d $(INSTALLDIR)/data/vendor ] || install -d $(INSTALLDIR)/data/vendor
	@[ -d $(INSTALLKMODDIR) ] || install -d $(INSTALLKMODDIR)
ifneq ($(wildcard $(PKG_BUILD_DIR)/wow/*.c),)
	$(MAKE) -C $(PKG_BUILD_DIR) QCAWLAN_TOOL_LIST="$(QCAWLAN_TOOL_LIST)" INSTALL_BIN_DEST="$(INSTALLDIR)/usr/sbin" CONFIG_WIFI_TARGET_WIFI_2_0="$(CONFIG_WIFI_TARGET_WIFI_2_0)" CONFIG_WIFI_TARGET_WIFI_3_0="$(CONFIG_WIFI_TARGET_WIFI_3_0)" \
		INSTALL_LIB_DEST="$(INSTALLDIR)/usr/lib" INSTALL_BIN="install -m0755" INSTALL_DATA="install -m0644" tools_installonly
	$(MAKE) -C $(PKG_BUILD_DIR) QCAWLAN_MODULE_LIST="$(QCAWLAN_MODULE_LIST)" INSTALL_ROOT_DRV="$(INSTALLKMODDIR)" driver_installonly
#	install -D qca-wifi-10.4/testmodule/tm.ko $(INSTALLDIR)/qca-wifi-10.4/lib/modules/$(LINUX_KERNEL)/tm.ko
#	install -D qca-wifi-10.4/os/linux/tools/wnm-app $(INSTALLDIR)/qca-wifi-10.4/usr/sbin/wnm-app
else
	@install $(foreach f,$(QCAWIFI_KMOD),$(f)) $(INSTALLKMODDIR)
	@install $(foreach f,$(QCAWIFI_USR_SBIN),$(f)) $(INSTALLDIR)/usr/sbin
	@install $(foreach f,$(QCAWIFI_LIBS),$(f)) $(INSTALLDIR)/usr/lib
endif
	@$(STRIPX) $(INSTALLKMODDIR)/*
	@$(STRIP) $(INSTALLDIR)/usr/lib/*
	@$(STRIP) $(INSTALLDIR)/usr/sbin/*

clean:

stage:
ifneq ($(wildcard $(PKG_BUILD_DIR)/wow/*.c),)
	[ -d $(STAGEDIR)/usr/include/qca-wifi ] || install -d $(STAGEDIR)/usr/include/qca-wifi
	$(MAKE) -C $(PKG_BUILD_DIR) INSTALL_DEST="$(STAGING_DIR)/usr/include/" install_headers
	$(CP) $(LINUXDIR)/include/uapi/linux/nl80211.h $(STAGING_DIR)/usr/include/nl80211_copy.h
endif





