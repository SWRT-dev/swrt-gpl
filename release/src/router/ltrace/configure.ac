# -*- Autoconf -*-
# This file is part of ltrace.
# Copyright (C) 2010,2012,2013,2014 Petr Machata, Red Hat Inc.
# Copyright (C) 2010,2011 Joe Damato
# Copyright (C) 2010 Marc Kleine-Budde
# Copyright (C) 2010 Zachary T Welch
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

# Process this file with autoconf to produce a configure script.
AC_PREREQ([2.65])

AC_INIT([ltrace],[0.7.91],[ltrace-devel@lists.alioth.debian.org],
	[ltrace],[http://ltrace.alioth.debian.org/])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_SRCDIR(libltrace.c)
AC_CONFIG_MACRO_DIR([config/m4])
AC_CONFIG_AUX_DIR([config/autoconf])
AC_CANONICAL_BUILD
AC_CANONICAL_HOST

case "${host_os}" in
    linux-gnu*) HOST_OS="linux-gnu" ;;
    linux-uclibc*) HOST_OS="linux-gnu" ;;
    *)		AC_MSG_ERROR([unkown host-os ${host_os}]) ;;
esac
AC_SUBST(HOST_OS)

case "${host_cpu}" in
    arm*|sa110)		HOST_CPU="arm" ;;
    cris*)		HOST_CPU="cris" ;;
    mips*)		HOST_CPU="mips" ;;
    powerpc|powerpc64|powerpc64le)	HOST_CPU="ppc" ;;
    sun4u|sparc64)	HOST_CPU="sparc" ;;
    s390x)		HOST_CPU="s390" ;;
    i?86|x86_64)	HOST_CPU="x86" ;;
    xtensa*)		HOST_CPU="xtensa" ;;
    *)			HOST_CPU="${host_cpu}" ;;
esac
AC_SUBST(HOST_CPU)

# Checks for programs.
AC_PROG_CC
LT_INIT
# libtool-2:  LT_INIT()
AM_INIT_AUTOMAKE([foreign no-exeext dist-bzip2])
AM_MAINTAINER_MODE

#
# We use stat(2).  Even though we don't care about the file size or
# inode number, stat will fail with EOVERFLOW if either of these
# exceeds 32 bits.  We therefore ask for stat64 if available.  Do this
# test as soon as possible, as large file support may influence
# whether other headers are available.
#
AC_SYS_LARGEFILE

AC_ARG_WITH([libelf],
  AS_HELP_STRING([--with-libelf], [Prefix of libelf headers/library]),
  [case "${withval}" in
  (no)
    AC_MSG_ERROR([*** libelf is a required dependency])
    ;;
  (yes)
    AC_MSG_ERROR([*** --with-libelf requires you to specify a path])
    ;;
  (*)
    AM_CPPFLAGS="${AM_CPPFLAGS} -I${withval}/include"
    AM_LDFLAGS="${AM_LDFLAGS} -L${withval}/lib"
    libelf_LD_LIBRARY_PATH="${withval}/lib"
    ;;
esac],[])

# Checks for libraries.

saved_CPPFLAGS="${CPPFLAGS}"
saved_LDFLAGS="${LDFLAGS}"
CPPFLAGS="${CPPFLAGS} ${AM_CPPFLAGS}"
LDFLAGS="${LDFLAGS} ${AM_LDFLAGS}"
# libelf
AC_CHECK_HEADERS([elf.h gelf.h],,
	[AC_MSG_ERROR([*** libelf.h or gelf.h not found on your system])]
)
AC_CHECK_LIB([elf], [elf_begin], [libelf_LIBS="-lz -lzstd -lelf"],
	[AC_MSG_ERROR([*** libelf not found on your system])]
)
CPPFLAGS="${saved_CPPFLAGS}"
LDFLAGS="${saved_LDFLAGS}"
AC_SUBST(libelf_LIBS)


# HAVE_LIBIBERTY
AC_CHECK_LIB([iberty], [cplus_demangle], [
	AC_DEFINE([HAVE_LIBIBERTY], [1], [we have libiberty])
	liberty_LIBS="-liberty"], [
	liberty_LIBS=""])
AC_SUBST(liberty_LIBS)


# HAVE_LIBSUPC__
AC_CHECK_LIB([supc++], [__cxa_demangle], [
	AC_DEFINE([HAVE_LIBSUPC__], [1], [we have libsupc++])
	libsupcxx_LIBS="-lsupc++"], [
	libsupcxx_LIBS=""])
AC_SUBST(libsupcxx_LIBS)


# HAVE_LIBSTDC__
AC_CHECK_LIB([stdc++], [__cxa_demangle], [
	AC_DEFINE([HAVE_LIBSTDC__], [1], [we have libstdc++])
	libstdcxx_LIBS="-lstdc++"], [
	libstdcxx_LIBS=""])
AC_SUBST(libstdcxx_LIBS)


dnl Check security_get_boolean_active availability.
AC_CHECK_HEADERS(selinux/selinux.h)
AC_CHECK_LIB(selinux, security_get_boolean_active)

dnl Whether (and which) elfutils libdw.so to use for unwinding.
AC_ARG_WITH(elfutils,
  AS_HELP_STRING([--with-elfutils], [Use elfutils libdwfl unwinding support]),
  [case "${withval}" in
  (yes|no) enable_elfutils=$withval;;
  (*) enable_elfutils=yes
    AM_CPPFLAGS="${AM_CPPFLAGS} -I${withval}/include"
    AM_LDFLAGS="${AM_LDFLAGS} -L${withval}/lib"
    elfutils_LD_LIBRARY_PATH="${withval}/lib:${withval}/lib/elfutils"
    ;;
esac],[enable_elfutils=maybe])

dnl Check whether we have the elfutils libdwfl.h header installed.
saved_CPPFLAGS="${CPPFLAGS}"
CPPFLAGS="${CPPFLAGS} ${AM_CPPFLAGS}"
AC_CHECK_HEADERS([elfutils/libdwfl.h],[have_libdwfl_h=yes])
CPPFLAGS="${saved_CPPFLAGS}"

dnl And whether libdw.so provides the unwinding functions.
saved_LDFLAGS="${LDFLAGS}"
LDFLAGS="${LDFLAGS} ${AM_LDFLAGS}"
AC_CHECK_LIB([dw], [dwfl_getthread_frames], [have_libdw_dwfl_frames=yes])
LDFLAGS="${saved_LDFLAGS}"

AC_MSG_CHECKING([whether to use elfutils libdwfl unwinding support])
case "${enable_elfutils}" in
(yes|maybe)
  if test x$have_libdwfl_h = xyes -a x$have_libdw_dwfl_frames = xyes; then
    enable_elfutils=yes
  elif test $enable_elfutils = maybe; then
    enable_elfutils=no
  else
    AC_MSG_RESULT([$enable_elfutils])
    AC_MSG_ERROR([Missing elfutils/libdwfl.h or dwfl_getthread_frames not in libdw.so])
  fi
  ;;
(*) ;;
esac
AC_MSG_RESULT([$enable_elfutils])

if test x"$enable_elfutils" = xyes; then
  libdw_LIBS=-ldw
  AC_SUBST(libdw_LIBS)
  AC_DEFINE([HAVE_LIBDW], [1], [we have elfutils libdw])
fi

# HAVE_LIBUNWIND
AC_ARG_WITH(libunwind,
  AS_HELP_STRING([--with-libunwind], [Use libunwind frame unwinding support]),
  [case "${withval}" in
  (yes|no) enable_libunwind=$withval;;
  (*) enable_libunwind=yes
    AM_CPPFLAGS="${AM_CPPFLAGS} -I${withval}/include"
    AM_LDFLAGS="${AM_LDFLAGS} -L${withval}/lib"
    libunwind_LD_LIBRARY_PATH="${withval}/lib"
    ;;
esac],[enable_libunwind=maybe])

saved_CPPFLAGS="${CPPFLAGS}"
CPPFLAGS="${CPPFLAGS} ${AM_CPPFLAGS}"
AC_CHECK_HEADERS([libunwind.h], [have_libunwind_h=yes])
AC_CHECK_HEADERS([libunwind-ptrace.h], [have_libunwind_ptrace_h=yes])
CPPFLAGS="${saved_CPPFLAGS}"

AC_MSG_CHECKING([whether to use libunwind support])
case "${enable_libunwind}" in
(yes|maybe)
  if test x$have_libunwind_h = xyes -o x$have_libunwind_ptrace_h = xyes; then
    enable_libunwind=yes
  elif test $enable_libunwind = maybe; then
    enable_libunwind=no
  else
    AC_MSG_RESULT([$enable_libunwind])
    AC_MSG_ERROR([libunwind.h or libunwind-ptrace.h cannot be found])	
  fi
  ;;
(*) ;;
esac
AC_MSG_RESULT([$enable_libunwind])

if test x"$enable_libunwind" = xyes; then
  case "${host_cpu}" in
      arm*|sa110)	  UNWIND_ARCH="arm" ;;
      i?86)		  UNWIND_ARCH="x86" ;;
      powerpc)		  UNWIND_ARCH="ppc32" ;;
      powerpc64|powerpc64le)	UNWIND_ARCH="ppc64" ;;
      mips*)		  UNWIND_ARCH="mips" ;;
      *)		  UNWIND_ARCH="${host_cpu}" ;;
  esac

  saved_LDFLAGS="${LDFLAGS}"
  LDFLAGS="${LDFLAGS} ${AM_LDFLAGS}"
  AC_CHECK_LIB([unwind], [backtrace], [libunwind_LIBS=-lunwind],
	       [AC_MSG_ERROR([Couldn't find or use libunwind.])])

  AC_CHECK_LIB([unwind-${UNWIND_ARCH}], [_U${UNWIND_ARCH}_init_remote],
	       [libunwind_LIBS="-lunwind-${UNWIND_ARCH} $libunwind_LIBS"],
	       [AC_MSG_ERROR([Couldn't find or use libunwind-${UNWIND_ARCH}.])],
	       [$libunwind_LIBS])

  AC_CHECK_LIB([unwind-ptrace], [_UPT_create],
	       [libunwind_LIBS="-lunwind-ptrace $libunwind_LIBS"],
	       [AC_MSG_ERROR([Couldn't find or use libunwind-ptrace.])],
	       [$libunwind_LIBS])

  AC_SUBST(libunwind_LIBS)
  AC_DEFINE([HAVE_LIBUNWIND], [1], [we have libunwind])
  LDFLAGS="${saved_LDFLAGS}"
fi

if test x"$enable_elfutils" = xyes -a x"$enable_libunwind" = xyes; then
  AC_MSG_ERROR([Cannot enable both --with-libunwind and --with-elfutils])
fi

if test x"$enable_elfutils" = xyes -o x"$enable_libunwind" = xyes; then
  AC_DEFINE([HAVE_UNWINDER], [1], [we have an unwinder available])
fi

saved_CPPFLAGS="${CPPFLAGS}"
saved_LDFLAGS="${LDFLAGS}"
CPPFLAGS="${CPPFLAGS} ${AM_CPPFLAGS}"
LDFLAGS="${LDFLAGS} ${AM_LDFLAGS}"
# HAVE_ELF_C_READ_MMAP
AC_MSG_CHECKING([whether elf_begin accepts ELF_C_READ_MMAP])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include <gelf.h>]], [[
int main () {
	Elf *elf = elf_begin(4, ELF_C_READ_MMAP, 0);
	return 0;
}
	]])],[
	AC_DEFINE([HAVE_ELF_C_READ_MMAP], [1], [we have read mmap support])
	AC_MSG_RESULT([yes])],[
	AC_MSG_RESULT([no])])

saved_CFLAGS="${CFLAGS}"
CFLAGS="${CFLAGS} -Wall -Werror"
AC_MSG_CHECKING([whether elf_hash takes a char* argument])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include <libelf.h>]], [[
	(void) elf_hash("name");
	]])],
	[AC_DEFINE([ELF_HASH_TAKES_CHARP], [1],
		[elf_hash() takes char* (as opposed to unsigned char *)])
	 AC_MSG_RESULT([yes])],
	[AC_MSG_RESULT([no])])
CFLAGS="${saved_CFLAGS}"
CPPFLAGS="${saved_CPPFLAGS}"
LDFLAGS="${saved_LDFLAGS}"

AM_CPPFLAGS=" \
	-DSYSCONFDIR="'\"$(sysconfdir)\"'" \
	-DPKGDATADIR="'\"$(pkgdatadir)\"'" \
	${AM_CPPFLAGS} \
	-I\$(top_srcdir)/sysdeps/${HOST_OS}/${HOST_CPU} \
	-I\$(top_srcdir)/sysdeps/${HOST_OS} \
	-I\$(top_srcdir)/sysdeps \
	-I\$(top_srcdir) \
"

# Checks for header files.
AC_CHECK_HEADERS([ \
	fcntl.h \
	limits.h \
	stddef.h \
	stdint.h \
	stdlib.h \
	string.h \
	sys/ioctl.h \
	sys/param.h \
	sys/time.h \
	unistd.h \
])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_UID_T
AC_C_INLINE
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_CHECK_SIZEOF([long])


# Checks for library functions.
AC_FUNC_FORK
AC_CHECK_FUNCS([ \
	alarm \
	atexit \
	gettimeofday \
	memset \
	strchr \
	strdup \
	strerror \
	strsignal \
	strtol \
	strtoul \
])

#
# Define HAVE_OPEN_MEMSTREAM if open_memstream is available.  glibc
# before 2.10, eglibc and uClibc all need _GNU_SOURCE defined for
# open_memstream to become visible, so check for that as well.  If
# unavailable, require that tmpfile be present.  There's no
# HAVE_TMPFILE, as we plain require that to be present as a fallback.
#
AC_CHECK_FUNCS([open_memstream], [],
	[AC_MSG_CHECKING([for open_memstream with _GNU_SOURCE])
	 AC_LINK_IFELSE(
		[AC_LANG_PROGRAM([[#define _GNU_SOURCE 1
				   #include <stdio.h>]],
				 [[char *buf; size_t sz;
				   return open_memstream(&buf, &sz) != 0;]])],

		 [AC_MSG_RESULT([yes])
		  AC_DEFINE([HAVE_OPEN_MEMSTREAM], [1],
			[Define if open_memstream exists.])],

		 [AC_MSG_RESULT([no])
		  AC_CHECK_FUNC([tmpfile], [],
			[AC_MSG_ERROR(
			    [Either open_memstream or tmpfile required.])])])])

#
# Define HAVE_GETOPT_LONG if that is available.
#
AC_CHECK_HEADER([getopt.h], [AC_CHECK_FUNCS([getopt_long])])

#
# Debugging
#
AC_MSG_CHECKING([whether to enable debugging])
AC_ARG_ENABLE(debug,
    AS_HELP_STRING([--enable-debug], [enable debugging @<:@default=no@:>@]),
	[case "$enableval" in
	y | yes) CONFIG_DEBUG=yes ;;
        *) CONFIG_DEBUG=no ;;
    esac],
    [CONFIG_DEBUG=no])
AC_MSG_RESULT([${CONFIG_DEBUG}])
if test "${CONFIG_DEBUG}" = "yes"; then
    AC_DEFINE(DEBUG, 1, [debugging])
fi

# Ignore the compiler's warnings at your own risk.
AM_CFLAGS="${AM_CFLAGS} -Wall -Wsign-compare -Wfloat-equal -Wformat-security"
AC_ARG_ENABLE([werror],
    AS_HELP_STRING([--disable-werror], [disable use of -Werror]),
    [enable_werror=$enableval], [enable_werror=yes])
if test x$enable_werror = xyes; then
    AM_CFLAGS="${AM_CFLAGS} -Werror"
fi

AC_ARG_ENABLE([valgrind],
    AS_HELP_STRING([--enable-valgrind],[run all tests under valgrind]),
    [use_valgrind=$enableval], [use_valgrind=no])
if test x$use_valgrind = xyes; then
    AC_CHECK_PROG(HAVE_VALGRIND, valgrind, yes, no)
    if test x$HAVE_VALGRIND = xno; then
        AC_MSG_ERROR([valgrind not found])
    fi
fi
AM_CONDITIONAL(USE_VALGRIND, test "$use_valgrind" = yes)
AM_CONDITIONAL(HAVE_LIBDW, test x"$enable_elfutils" = xyes)

AC_SUBST(AM_CPPFLAGS)
AC_SUBST(AM_CFLAGS)
AC_SUBST(AM_LDFLAGS)
AC_SUBST(libelf_LD_LIBRARY_PATH)
AC_SUBST(elfutils_LD_LIBRARY_PATH)
AC_SUBST(libunwind_LD_LIBRARY_PATH)

AC_CONFIG_FILES([
	Makefile
	sysdeps/Makefile
	sysdeps/linux-gnu/Makefile
	sysdeps/linux-gnu/aarch64/Makefile
	sysdeps/linux-gnu/alpha/Makefile
	sysdeps/linux-gnu/arm/Makefile
	sysdeps/linux-gnu/cris/Makefile
	sysdeps/linux-gnu/ia64/Makefile
	sysdeps/linux-gnu/m68k/Makefile
	sysdeps/linux-gnu/metag/Makefile
	sysdeps/linux-gnu/mips/Makefile
	sysdeps/linux-gnu/ppc/Makefile
	sysdeps/linux-gnu/s390/Makefile
	sysdeps/linux-gnu/sparc/Makefile
	sysdeps/linux-gnu/x86/Makefile
	sysdeps/linux-gnu/xtensa/Makefile
	testsuite/Makefile
	testsuite/ltrace.main/Makefile
	testsuite/ltrace.minor/Makefile
	testsuite/ltrace.torture/Makefile
])
AC_OUTPUT
