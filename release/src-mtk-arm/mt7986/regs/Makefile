EXEC = regs

all: $(EXEC)

$(EXEC): $(EXEC).c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $@.c $(LDLIBS)

install:
	install -D regs $(INSTALLDIR)/sbin/regs
	$(STRIP) $(INSTALLDIR)/sbin/regs

clean:
	-rm -f $(EXEC) *.elf *.gdb *.o

