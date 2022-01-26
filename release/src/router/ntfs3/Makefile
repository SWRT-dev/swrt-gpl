
PWD=$(shell pwd)

all:
	$(MAKE) -C $(LINUXDIR) CROSS_COMPILE=$(patsubst %-gcc,%-,$(KERNELCC)) EXTRA_CFLAGS="" SUBDIRS=$(PWD)/src modules

clean:
	$(RM) *.o *.ko src/*.o src/*.ko src/*.mod.*

.NOTPARALLEL:

install:
	$(MAKE) -C $(LINUXDIR) CROSS_COMPILE=$(patsubst %-gcc,%-,$(KERNELCC)) EXTRA_CFLAGS="" SUBDIRS=$(PWD)/src INSTALL_MOD_PATH=$(INSTALLDIR) modules_install
	@find $(INSTALLDIR) -name "modules.*" | xargs rm -f
	@find $(INSTALLDIR) -name "*.ko" | xargs $(STRIPX)

