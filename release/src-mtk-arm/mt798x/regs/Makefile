# Convert swrt build environment variable to sdk
include ../rules.mk
PKG_BUILD_DIR=$(shell pwd)/source

EXEC = regs

all: $(EXEC)
	touch stamp-h1

$(EXEC): $(EXEC).c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $@.c $(LDLIBS)

install:
	install -D regs $(INSTALLDIR)/usr/sbin/regs
	$(STRIP) $(INSTALLDIR)/usr/sbin/regs

clean:
	-rm -f $(EXEC) *.elf *.gdb *.o

