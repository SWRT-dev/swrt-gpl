include $(TOP)/common.mak
include $(SRCBASE)/.config
EXEC = mii_mgr
SRC=source/mii_mgr.c

CFLAGS += -Wall -Werror

#ifneq ($(CONFIG_SUPPORT_OPENWRT), y)
#ifeq ($(CONFIG_GLIBC_2_20),y)
#CFLAGS += -DCONFIG_GLIBC_2_20
#endif
#CFLAGS += -I$(TOP)/kernel_header/include -I./source
CFLAGS += -I./source
#ifeq ($(CONFIG_DEFAULTS_KERNEL_4_4),y)
#CFLAGS += -I$(ROOTDIR)/$(LINUXDIR)/drivers/net/ethernet/raeth
#else
#CFLAGS += -I$(ROOTDIR)/$(LINUXDIR)/drivers/net/raeth
#endif
#endif #CONFIG_SUPPORT_OPENWRT

all: $(EXEC)

mii_mgr: $(SRC)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(SRC)

install:
	install -D mii_mgr $(INSTALLDIR)/sbin/mii_mgr
	$(STRIP) $(INSTALLDIR)/sbin/mii_mgr
	cd $(INSTALLDIR)/sbin/ && ln -sf mii_mgr mii_mgr_cl45

clean:
	-rm -f $(EXEC) *.elf *.gdb *.o

