# Convert swrt build environment variable to qsdk
include ../qsdk.config
include ../rules.mk
PKG_BUILD_DIR=$(shell pwd)/source

MAKE_OPTS:=

EXTRA_CFLAGS+= \
	-I$(PKG_BUILD_DIR)/exports

MAKE_OPTS+=SFE_SUPPORT_IPV6=y
MAKE_OPTS+=SFE_PROCESS_LOCAL_OUT=y
# Disable SFE Bridge VLAN Filtering support when global kernel config is disabled
ifneq ($(CONFIG_BRIDGE_VLAN_FILTERING),)
MAKE_OPTS+=SFE_BRIDGE_VLAN_FILTERING_ENABLE=y
endif

ifeq ($(CONFIG_KERNEL_IPQ_MEM_PROFILE),256)
EXTRA_CFLAGS+= -DSFE_MEM_PROFILE_LOW
else ifeq ($(CONFIG_LOWMEM_FLASH),y)
EXTRA_CFLAGS+= -DSFE_MEM_PROFILE_LOW
else ifeq ($(CONFIG_KERNEL_IPQ_MEM_PROFILE),512)
EXTRA_CFLAGS+= -DSFE_MEM_PROFILE_MEDIUM
endif

INSTALLKMODDIR:=$(INSTALLDIR)/lib/modules/$(LINUX_KERNEL)

all:
ifneq ($(wildcard $(SDK_DIR_PLATFORM)/qca-nss-sfe/Makefile),)
	ln -sf $(SDK_DIR_PLATFORM)/qca-nss-sfe source
	$(KERNEL_MAKE) $(strip $(MAKE_OPTS)) M="$(PKG_BUILD_DIR)" EXTRA_CFLAGS="$(EXTRA_CFLAGS)" \
		KBUILD_EXTRA_SYMBOLS=$(KBUILD_EXTRA_SYMBOLS) modules
else
	mkdir -p $(PKG_BUILD_DIR)
	cp prebuilt/qca-nss-sfe.ko $(PKG_BUILD_DIR)/
endif
	touch stamp-h1

install:
	@[ -d $(INSTALLKMODDIR) ] || install -d $(INSTALLKMODDIR)
	install -D $(PKG_BUILD_DIR)/qca-nss-sfe.ko $(INSTALLKMODDIR)
	@$(STRIPX) $(INSTALLKMODDIR)/*.ko

clean:

stage:
ifneq ($(wildcard $(SDK_DIR_PLATFORM)/qca-nss-sfe/Makefile),)
	mkdir -p $(STAGING_DIR)/usr/include/qca-nss-sfe
	cp -rf $(PKG_BUILD_DIR)/exports/* $(STAGING_DIR)/usr/include/qca-nss-sfe/
endif

