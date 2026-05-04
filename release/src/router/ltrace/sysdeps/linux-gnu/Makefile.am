# This file is part of ltrace.
# Copyright (C) 2014 Petr Machata, Red Hat, Inc.
# Copyright (C) 2010,2012 Marc Kleine-Budde, Pengutronix
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

DIST_SUBDIRS = aarch64 alpha arm cris ia64 m68k metag mips ppc s390	\
	       sparc x86 xtensa

SUBDIRS = \
	$(HOST_CPU)

noinst_LTLIBRARIES = \
	../libos.la

___libos_la_SOURCES = events.c trace.c proc.c breakpoint.c hooks.c

___libos_la_LIBADD = \
	libcpu.la

noinst_HEADERS = arch_syscallent.h signalent1.h syscallent1.h trace.h	\
	trace-defs.h events.h os.h

EXTRA_DIST = \
	arch_mksyscallent \
	mksignalent \
	mksyscallent \
	mksyscallent_s390

MAINTAINERCLEANFILES = \
	Makefile.in
