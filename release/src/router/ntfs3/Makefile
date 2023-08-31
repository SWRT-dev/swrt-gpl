include $(TOP)/.config
PWD=$(shell pwd)
ifeq ($(or $(RTCONFIG_MT798X),$(RTCONFIG_RALINK_EN7561),$(RTCONFIG_SOC_IPQ8074),$(RTCONFIG_SOC_IPQ50XX),$(RTCONFIG_SOC_IPQ60XX)),y)
SRC=src1
else
SRC=src
endif

all:
	$(MAKE) -C $(LINUXDIR) CROSS_COMPILE=$(patsubst %-gcc,%-,$(KERNELCC)) EXTRA_CFLAGS="" SUBDIRS=$(PWD)/$(SRC) modules

clean:
	$(RM) *.o *.ko $(SRC)/*.o $(SRC)/*.ko $(SRC)/*.mod.*

.NOTPARALLEL:

install:
	$(MAKE) -C $(LINUXDIR) CROSS_COMPILE=$(patsubst %-gcc,%-,$(KERNELCC)) EXTRA_CFLAGS="" SUBDIRS=$(PWD)/$(SRC) INSTALL_MOD_PATH=$(INSTALLDIR) modules_install
	@find $(INSTALLDIR) -name "modules.*" | xargs rm -f
	@find $(INSTALLDIR) -name "*.ko" | xargs $(STRIPX)

