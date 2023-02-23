include $(TOP)/common.mak
include $(SRCBASE)/.config
CFLAGS += -D_GNU_SOURCE
CFLAGS += -I$(TOP)/kernel_header/include -I$(STAGEDIR)/usr/include/ -I$(STAGEDIR)/usr/include/libnl-tiny -I./src

#CFLAGS += -I$(ROOTDIR)/$(LINUXDIR)/drivers/net/raeth -Werror
LDFLAGS += -L$(STAGEDIR)/usr/lib -lnl-tiny -lm
EXEC = switch

SRC=src/switch_fun.c src/switch_753x.c src/switch_ioctl.c src/switch_netlink.c

all: $(EXEC)

switch: $(SRC)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(SRC)

install:
	install -D $(EXEC) $(INSTALLDIR)/bin/$(EXEC)
	$(STRIP) $(INSTALLDIR)/bin/$(EXEC)

clean:
	-rm -f $(EXEC) *.elf *.gdb *.o

