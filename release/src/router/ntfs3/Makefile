VERSION := $(shell cat $(LINUXDIR)/include/config/kernel.release 2> /dev/null)

Q ?= @

MODULE_CONFIG += CONFIG_NTFS3_FS=m
MODULE_CONFIG += CONFIG_NTFS3_LZX_XPRESS=y

sources += $(wildcard lib/*)
sources += $(wildcard *.h *.c)
sources += Makefile Kbuild Kconfig

ccflags-$(CONFIG_NTFS3_LZX_XPRESS) += -DCONFIG_NTFS3_LZX_XPRESS
ccflags-$(CONFIG_FS_POSIX_ACL) += -DCONFIG_NTFS3_FS_POSIX_ACL

destination = $(DESTDIR)/usr/src/$(DKMS_NAME)-$(VERSION)
PWD=$(shell pwd)

default: ntfs3.ko

ntfs3.ko: force
	#$(MAKE) -C $(KBUILD) M=$(CURDIR) modules $(MODULE_CONFIG) EXTRA_CFLAGS="$(CFLAGS_EXTRA)"
	$(MAKE) -C $(LINUXDIR) CROSS_COMPILE=$(patsubst %-gcc,%-,$(KERNELCC)) EXTRA_CFLAGS=-I$(TOP)/shared SUBDIRS=$(PWD) modules $(MODULE_CONFIG)

clean: force
	$(RM) *.o *.ko *.mod.*

.NOTPARALLEL:

install:
	#install -v -m 644 -D ntfs3.ko $(IDIR)/ntfs3.ko
	$(MAKE) -C $(LINUXDIR) CROSS_COMPILE=$(patsubst %-gcc,%-,$(KERNELCC)) EXTRA_CFLAGS=-I$(TOP)/shared SUBDIRS=$(PWD) INSTALL_MOD_PATH=$(INSTALLDIR) modules_install $(MODULE_CONFIG)
	@find $(INSTALLDIR) -name "modules.*" | xargs rm -f
	@find $(INSTALLDIR) -name "*.ko" | xargs $(STRIPX)

force: ;
