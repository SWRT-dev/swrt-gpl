#
# Makefile for memtester by Charles Cazabon.
#
# Copyright (C) 1999 Simon Kirby.
# Copyright (C) 1999-2020 Charles Cazabon.
# Licensed under the GNU General Public License version 2.  See the file
# COPYING for details.
#

# You don't need to edit these; change the contents of the conf-cc and conf-ld
# files if you need to change the compile/link commands.  See the README for
# more information.
CC			= $(CROSS_COMPILE)gcc
LD			= $(CROSS_COMPILE)gcc

SOURCES		= memtester.c tests.c output.c
OBJECTS		= $(SOURCES:.c=.o)
HEADERS		= memtester.h
TARGETS     = *.o compile load auto-ccld.sh find-systype make-compile make-load systype extra-libs
INSTALLPATH	= /usr/local
CFLAGS += -O2 -DPOSIX -D_POSIX_C_SOURCE=200809L -D_FILE_OFFSET_BITS=64 -DTEST_NARROW_WRITES -c
LDFLAGS += -s

#
# Targets
#
all: memtester

install: all
	mkdir -m 755 -p $(INSTALLPATH)/bin
	install -m 755 memtester $(INSTALLPATH)/bin/

clean:
	rm -f memtester $(TARGETS) $(OBJECTS) core


memtester: $(OBJECTS)
	$(CC) -o $@ $^ $(LDFLAGS)

