include ../common.mak
include $(SRCBASE)/.config

EXEC = mii_mgr mii_mgr_cl45
CFLAGS += -Wall -Werror

ifeq ($(RTCONFIG_RALINK_MT7629),y)
CFLAGS += -DRTCONFIG_RALINK_MT7629
endif

ifeq ($(RTCONFIG_RALINK_MT7622),y)
CFLAGS += -DRTCONFIG_RALINK_MT7622
endif

ifeq ($(RTCONFIG_WLMODULE_MT7915D_AP),y)
CFLAGS += -DRTCONFIG_WLMODULE_MT7915D_AP
endif

#ifneq ($(CONFIG_SUPPORT_OPENWRT), y)
#ifeq ($(CONFIG_GLIBC_2_20),y)
#CFLAGS += -DCONFIG_GLIBC_2_20
#endif
CFLAGS += -I$(ROOTDIR)/$(LINUXDIR)
#ifeq ($(CONFIG_DEFAULTS_KERNEL_4_4),y)
CFLAGS += -I$(ROOTDIR)/$(LINUXDIR)/drivers/net/ethernet/raeth
#else
#CFLAGS += -I$(ROOTDIR)/$(LINUXDIR)/drivers/net/raeth
#endif
#endif #CONFIG_SUPPORT_OPENWRT

all: $(EXEC)

mii_mgr: mii_mgr.o

	$(CC) $(LDFLAGS) -o $@ $^

mii_mgr_cl45: mii_mgr_cl45.o
	$(CC) $(LDFLAGS) -o $@ $^

romfs:
	$(ROMFSINST) /bin/mii_mgr
	$(ROMFSINST) /bin/mii_mgr_cl45

clean:
	-rm -f $(EXEC) *.elf *.gdb *.o

install:
	install -D $(EXEC) $(INSTALLDIR)/sbin/$(EXEC)
	$(STRIP) $(INSTALLDIR)/sbin/$(EXEC)
	chmod 0500 $(INSTALLDIR)/sbin/$(EXEC)

install:
	install -D mii_mgr $(INSTALLDIR)/sbin/mii_mgr
	$(STRIP) $(INSTALLDIR)/sbin/mii_mgr
	chmod 0500 $(INSTALLDIR)/sbin/mii_mgr
	install -D mii_mgr_cl45 $(INSTALLDIR)/sbin/mii_mgr_cl45
	$(STRIP) $(INSTALLDIR)/sbin/mii_mgr_cl45
	chmod 0500 $(INSTALLDIR)/sbin/mii_mgr_cl45

