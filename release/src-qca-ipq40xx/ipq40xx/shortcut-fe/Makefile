# Convert swrt build environment variable to qsdk
include ../qsdk.config
include ../rules.mk
PKG_BUILD_DIR=$(shell pwd)/source

EXTRA_CFLAGS += \
	-I$(PKG_BUILD_DIR)/shortcut-fe \
	-I$(PKG_BUILD_DIR)/fast-classifier \
	-DSFE_SUPPORT_IPV6 \

SFE_MAKEOPTS:=SFE_SUPPORT_IPV6=1 CONFIG_FAST_CLASSIFIER=m EXTRA_CFLAGS="$(EXTRA_CFLAGS)"

INSTALLKMODDIR:=$(INSTALLDIR)/lib/modules/$(LINUX_KERNEL)

all:
	$(KERNEL_MAKE) $(PARALLEL_BUILD) M=$(PKG_BUILD_DIR)/shortcut-fe $(SFE_MAKEOPTS) modules
	$(KERNEL_MAKE) $(PARALLEL_BUILD) M=$(PKG_BUILD_DIR)/fast-classifier $(SFE_MAKEOPTS) modules
	touch stamp-h1

install:
	@[ -d $(INSTALLKMODDIR) ] || install -d $(INSTALLKMODDIR)
	@install -D $(PKG_BUILD_DIR)/shortcut-fe/shortcut-fe.ko $(INSTALLKMODDIR)
	@install -D $(PKG_BUILD_DIR)/shortcut-fe/shortcut-fe-ipv6.ko $(INSTALLKMODDIR)
	@install -D $(PKG_BUILD_DIR)/shortcut-fe/shortcut-fe-cm.ko $(INSTALLKMODDIR)
	@install -D $(PKG_BUILD_DIR)/fast-classifier/fast-classifier.ko $(INSTALLKMODDIR)
	@$(STRIPX) $(INSTALLKMODDIR)/shortcut*.ko
	@$(STRIPX) $(INSTALLKMODDIR)/fast-classifier*.ko

clean:

stage:
	install -d $(STAGING_DIR)/usr/include/shortcut-fe
	cp -rf $(PKG_BUILD_DIR)/shortcut-fe/sfe.h $(STAGING_DIR)/usr/include/shortcut-fe

