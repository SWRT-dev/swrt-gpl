# This file is part of ltrace.
# Copyright (C) 2012, 2013 Petr Machata, Red Hat Inc.
# Copyright (C) 2010 Marc Kleine-Budde, Pengutronix
# Copyright (C) 2010 Zachary T Welch, CodeSourcery
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; either version 2 of the
# License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
# 02110-1301 USA

ACLOCAL_AMFLAGS = -I config/m4

SUBDIRS = \
	sysdeps \
	. \
	testsuite

noinst_LTLIBRARIES = \
	libltrace.la

libltrace_la_SOURCES = bits.c breakpoints.c debug.c demangle.c dict.c	\
	ltrace-elf.c execute_program.c handle_event.c libltrace.c	\
	options.c output.c proc.c read_config_file.c summary.c		\
	library.c filter.c glob.c type.c value.c value_dict.c expr.c	\
	fetch.c vect.c param.c printf.c zero.c lens.c lens_default.c	\
	lens_enum.c memstream.c prototype.c
if HAVE_LIBDW
libltrace_la_SOURCES += dwarf_prototypes.c
endif

libltrace_la_LIBADD = \
	$(libelf_LIBS) \
	$(liberty_LIBS) \
	$(libsupcxx_LIBS) \
	$(libstdcxx_LIBS) \
	$(libdw_LIBS) \
	$(libunwind_LIBS) \
	sysdeps/libos.la

bin_PROGRAMS = \
	ltrace

ltrace_SOURCES = \
	main.c

ltrace_LDADD = \
	libltrace.la

noinst_HEADERS = bits.h backend.h breakpoint.h common.h debug.h		\
	defs.h demangle.h dict.h forward.h ltrace-elf.h ltrace.h	\
	options.h output.h proc.h read_config_file.h summary.h		\
	library.h filter.h glob.h vect.h type.h value.h value_dict.h	\
	callback.h expr.h fetch.h vect.h param.h printf.h zero.h	\
	lens.h lens_default.h lens_enum.h memstream.h prototype.h
if HAVE_LIBDW
noinst_HEADERS += dwarf_prototypes.h
endif

dist_man1_MANS = ltrace.1
dist_man5_MANS = ltrace.conf.5

dist_doc_DATA = COPYING CREDITS INSTALL README TODO

dist_pkgdata_DATA = etc/syscalls.conf etc/libc.so.conf	\
	etc/libm.so.conf etc/libacl.so.conf etc/libpthread.so.conf \
	etc/libpthread.so-types.conf etc/libc.so-types.conf

EXTRA_DIST = \
	debian/changelog \
	debian/compat \
	debian/control \
	debian/copyright \
	debian/rules

MAINTAINERCLEANFILES = \
	configure \
	Makefile.in \
	aclocal.m4 \
	config.h.in \
	config.h.in~ \
	config/autoconf/compile \
	config/autoconf/config.guess \
	config/autoconf/config.sub \
	config/autoconf/depcomp \
	config/autoconf/install-sh \
	config/autoconf/ltmain.sh \
	config/autoconf/mdate-sh \
	config/autoconf/missing \
	config/autoconf/texinfo.tex \
	libtool.m4 \
	ltoptions.m4 \
	ltsugar.m4 \
	ltversion.m4 \
	lt~obsolete.m4 \
	$(DIST_ARCHIVES)

maintainer-clean-local:
	-chmod -R a+rw $(distdir)
	-rm -fr $(distdir)
