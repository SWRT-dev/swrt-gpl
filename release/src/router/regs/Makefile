EXEC = regs

all:
	- cp prebuild/$(BUILD_NAME)/regs regs

install: all
	install -D $(EXEC) $(INSTALLDIR)/bin/$(EXEC)
	$(STRIP) $(INSTALLDIR)/bin/$(EXEC)

clean:
	-rm -f $(EXEC) *.elf *.gdb *.o
