# Copyright (C) 1992 - 2001, 2012, 2013, 2015 Free Software Foundation, Inc.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 1, or (at your option)
# any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#

EXTRA_DIST = branch_func.c branch_func.exp filters.exp hello-vfork.c	\
	hello-vfork.exp main.c main.exp main-internal.exp main-lib.c	\
	main-threaded.exp main-vfork.c main-vfork.exp			\
	parameters.c parameters.conf parameters.exp parameters-lib.c	\
	parameters2.exp parameters3.exp signals.c signals.exp		\
	system_calls.c system_calls.exp system_call_params.exp


CLEANFILES = *.o *.so *.log *.sum *.ltrace setval.tmp \
	main main-internal parameters signals system_calls

MAINTAINERCLEANFILES = Makefile.in


if HAVE_LIBDW
  EXTRA_DIST += dwarf.conf dwarf.exp dwarf.c dwarflib.c dwarflib.h dwarflib_cxx.cc11 dwarflib_cxx.hh
  CLEANFILES += dwarf
endif
