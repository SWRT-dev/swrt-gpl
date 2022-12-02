
CFLAGS += -Os -Wall -Werror --std=gnu99 -g3 -Wmissing-declarations -D_GNU_SOURCE
CFLAGS += -I$(STAGEDIR)/usr/include/libnl-tiny -I$(TOP)/libubox -I$(TOP)/zlib
LDFLAGS += -L$(STAGEDIR)/usr/lib -lnl-tiny -L$(TOP)/libubox -lubox -L$(TOP)/zlib -lz

OBJS = client.c database.c neigh.c nfnetlink.c nlbwmon.c protocol.c socket.c subnets.c timing.c utils.c
EXEC = nlbwmon

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(LDFLAGS) $(CFLAGS) -o $@ $(OBJS)

%.o:
	$(CC) $(CFLAGS) -o $@ -c $<

install:
	install -d $(INSTALLDIR)/nlbwmon/usr/sbin
	install -d $(INSTALLDIR)/nlbwmon/usr/share/nlbwmon
	install $(EXEC) -D $(INSTALLDIR)/nlbwmon/usr/sbin/$(EXEC)
	install protocols.txt -D $(INSTALLDIR)/nlbwmon/usr/share/nlbwmon/protocols
	$(STRIP) $(INSTALLDIR)/nlbwmon/usr/sbin/$(EXEC)
	cd $(INSTALLDIR)/nlbwmon/usr/sbin/ && ln -sf $(EXEC) nlbw
	
clean:
	rm -f $(OBJS) $(EXEC)

.PHONY: all clean
