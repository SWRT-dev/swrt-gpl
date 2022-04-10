PWD=$(shell pwd)

all:
	$(MAKE) -C $(LINUXDIR) EXTRA_CFLAGS=-I$(TOP)/shared SUBDIRS=$(PWD)/src modules

install:
	$(MAKE) -C $(LINUXDIR) EXTRA_CFLAGS=-I$(TOP)/shared SUBDIRS=$(PWD)/src INSTALL_MOD_PATH=$(INSTALLDIR) modules_install
	@find $(INSTALLDIR) -name "modules.*" | xargs rm -f
	@find $(INSTALLDIR) -name "*.ko" | xargs $(STRIPX)

clean:
	$(RM) *.o *.ko src/*.o src/*.ko src/*.mod.*

