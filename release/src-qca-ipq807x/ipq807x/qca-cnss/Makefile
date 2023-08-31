include ../qsdk.config
# Convert swrt build environment variable to qsdk
PKG_BUILD_DIR=$(shell pwd)/source
STAGING_DIR=$(STAGEDIR)
#TARGET_CROSS=$(shell basename $(CROSS_COMPILE))
TARGET_CROSS=$(CROSS_COMPILE)


DRV_MAKE_OPTS+=QCA_CNSS_STREAM_MOD=y

INSTALLKMODDIR:=$(INSTALLDIR)/lib/modules/$(LINUX_KERNEL)

all:
ifneq ($(wildcard $(SDK_DIR_PLATFORM)/qca-cnss/*.c),)
	ln -sf $(SDK_DIR_PLATFORM)/qca-cnss source
	export CONFIG_BUILD_QSDK=y; \
	$(MAKE) -C "$(LINUXDIR)" $(strip $(DRV_MAKE_OPTS)) CROSS_COMPILE="$(TARGET_CROSS)" ARCH="$(ARCH)" M="$(PKG_BUILD_DIR)" modules
else
	mkdir -p $(PKG_BUILD_DIR)
	cp -rf prebuilt/*.ko $(PKG_BUILD_DIR)
endif
	touch stamp-h1

install:
	@[ -d $(INSTALLKMODDIR) ] || install -d $(INSTALLKMODDIR)
	@install -D $(PKG_BUILD_DIR)/ipq_cnss2.ko $(INSTALLKMODDIR)
	@install -D $(PKG_BUILD_DIR)/ipq_cnss2_stream.ko $(INSTALLKMODDIR)
	@$(STRIPX) $(INSTALLKMODDIR)/*.ko

clean:

stage:
ifneq ($(wildcard $(SDK_DIR_PLATFORM)/qca-cnss/*.c),)
	mkdir -p $(STAGING_DIR)/usr/include/qca-cnss
	cp -rf $(PKG_BUILD_DIR)/include/cnss2.h $(STAGING_DIR)/usr/include/qca-cnss/
endif

