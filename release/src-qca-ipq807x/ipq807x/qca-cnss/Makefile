# Convert swrt build environment variable to qsdk
include ../qsdk.config
include ../rules.mk
PKG_BUILD_DIR=$(shell pwd)/source

TARGET_CONFIGURE_OPTS+= KERNEL_SRC=$(LINUXDIR)
EXTRA_CFLAGS+= -DSWRT_PATCH
KERNEL_MAKEOPTS+= EXTRA_CFLAGS="$(EXTRA_CFLAGS)"

INSTALLKMODDIR:=$(INSTALLDIR)/lib/modules/$(LINUX_KERNEL)

all:
ifneq ($(wildcard $(SDK_DIR_PLATFORM)/qca-cnss/*.c),)
	ln -sf $(SDK_DIR_PLATFORM)/qca-cnss source
	$(KERNEL_MAKE) $(PARALLEL_BUILD) M="$(PKG_BUILD_DIR)" CONFIG_BUILD_QSDK=y QCA_CNSS_STREAM_MOD=y modules
else
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) prebuilt/$(BUILD_NAME)/*.ko $(PKG_BUILD_DIR)
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
	$(CP) $(PKG_BUILD_DIR)/include/cnss2.h $(STAGING_DIR)/usr/include/qca-cnss/
endif

