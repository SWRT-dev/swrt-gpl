include $(TOP)/.config
CFLAGS	+= -s -DBCMWPA2 -fpic -ffunction-sections -fdata-sections -Wl,--gc-sections
LDFLAGS	+= -L$(TOP)/nvram -lnvram -L$(TOP)/shared -lshared -ffunction-sections -fdata-sections -Wl,--gc-sections
ifeq ($(DHDAP),y)
CFLAGS += -D__CONFIG_DHDAP__
endif
OBJS := wlconf.c 
CFLAGS	+= -I. -I$(TOP)/shared -Wall -I$(SRCBASE)/
CFLAGS +=  -I$(LINUXDIR)/brcm/$(ARCH)/include -I$(LINUXDIR)/brcm/$(ARCH)/common/include  -I$(LINUXDIR)/brcm/$(ARCH)/shared/bcmwifi/include

all:
	$(CC) -o wlconf $(OBJS) $(CFLAGS) $(LDFLAGS)

clean:
	rm -f wlconf

install: all
	install -d $(INSTALLDIR)/usr/sbin
	install wlconf $(INSTALLDIR)/usr/sbin
	$(STRIP) $(INSTALLDIR)/usr/sbin/wlconf
