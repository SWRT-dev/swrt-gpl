include ../common.mak

all:

install:
	install -d $(INSTALLDIR)/sbin
	install -d $(INSTALLDIR)/usr/sbin
	install -d $(INSTALLDIR)/lib

ifeq ($(TCONFIG_USB),y)
# Optware perl symlink
	install -d $(INSTALLDIR)/usr/bin
	ln -sf /opt/bin/perl $(INSTALLDIR)/usr/bin/perl
endif
ifeq ($(RTCONFIG_BCMARM),y)
ifneq ($(RTCONFIG_HND_ROUTER),y)
#	install -m 0700 wlk $(INSTALLDIR)/usr/sbin
endif
endif
ifeq ($(RTCONFIG_ENTWARE),y)
	install -m 0755 entware.sh $(INSTALLDIR)/usr/sbin
endif
ifeq ($(K3),y)
	ln -sf /tmp/k3screenctrl $(INSTALLDIR)/lib/k3screenctrl
endif
# cron helper
	install -m 0700 cru $(INSTALLDIR)/usr/sbin

# playing with ipkg / from OpenWrt svn
#!!TB	install -m 0700 ipkg $(INSTALLDIR)/usr/sbin

# system info
	install -m 0700 sysinfo $(INSTALLDIR)/usr/sbin

# keep winscp happy
#	install -m 0700 groups $(INSTALLDIR)/usr/bin

# chpasswd script
	install -m 0700 chpasswd.sh $(INSTALLDIR)/usr/sbin

ifeq ($(RTCONFIG_RALINK),y)
# for adjust smp_affinity and rps_cpus
ifeq ($(RTCONFIG_RALINK_MT7621),y)
	install smp_openwr_4210_MT7621.sh $(INSTALLDIR)/sbin/smp.sh
endif
ifeq ($(RTCONFIG_RALINK_MT7629)$(RTCONFIG_RALINK_MT7622),y)
ifeq ($(RTACRH18)$(RT4GAC86U),y)
	install smp_openwr_4200.sh $(INSTALLDIR)/sbin/smp.sh
endif
endif
ifeq ($(RTCONFIG_MT798X),y)
	install smp_mt798x.sh $(INSTALLDIR)/sbin/smp.sh
endif
endif
clean:
