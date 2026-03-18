# Convert swrt build environment variable to qsdk
include ../qsdk.config
include ../rules.mk
PKG_BUILD_DIR=$(shell pwd)/source
PKG_SOURCE_VERSION=95ca1c3b99376da2d0306919f2df4a8d3c9bb78b

TARGET_CFLAGS+= -Wall

# Define variables for nat46
INSTALLKMODDIR:=$(INSTALLDIR)/lib/modules/$(LINUX_VERSION)
NAT46_KMOD_LIST := nat46/modules/nat46.ko

NAT46_HDRS:=$(wildcard $(PKG_BUILD_DIR)/nat46/modules/*.h)

all:
	$(TARGET_MAKE) $(PARALLEL_BUILD) -C $(PKG_BUILD_DIR) $(TARGET_CONFIGURE_OPTS)
	$(KERNEL_MAKE) $(PARALLEL_BUILD) M="$(PKG_BUILD_DIR)/nat46/modules" MODFLAGS="-DMODULE -mlong-calls" \
		EXTRA_CFLAGS="-DNAT46_VERSION=\\\"$(PKG_SOURCE_VERSION)\\\"" modules
	touch stamp-h1

install:
	install -d $(INSTALLKMODDIR)
	install -m 755 $(foreach kmod,$(NAT46_KMOD_LIST),$(PKG_BUILD_DIR)/$(kmod)) $(INSTALLKMODDIR)
	$(KSTRIPX) $(INSTALLKMODDIR)/*.ko
	# Package/464xlat/install
#	install -d $(INSTALLDIR)/lib/netifd/proto
#	install -m 0755 package/files/464xlat.sh $(INSTALLDIR)/lib/netifd/proto/464xlat.sh
	install -d $(INSTALLDIR)/sbin
	install -m 0755 $(PKG_BUILD_DIR)/464xlatcfg $(INSTALLDIR)/sbin
	$(STRIP) $(INSTALLDIR)/sbin/*

clean:
	rm -f stage
	find $(PKG_BUILD_DIR) -type f -name ".*.o.cmd" \
		-o -name ".*.ko.cmd" \
		-o -name "*.o" \
		-o -name "modules.order" \
		-o -name "*.ko" \
		-o -name ".tmp_versions" \
		-o -name "Module.symvers" \
		-o -name "*.mod.c" | xargs rm -fr

stage: $(NAT46_HDRS)
	@mkdir -p $(STAGEDIR)/usr/include/nat46
	install -m 644 $(foreach h,$^,$(h)) $(STAGEDIR)/usr/include/nat46/
	@touch $@
