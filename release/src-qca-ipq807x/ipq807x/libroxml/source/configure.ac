#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
m4_define([MAJOR_VERSION], 3)
m4_define([MINOR_VERSION], 0)
m4_define([MICRO_VERSION], 2)

AC_PREREQ(2.59)
AC_INIT([libroxml],
        [MAJOR_VERSION.MINOR_VERSION.MICRO_VERSION],
        [tristan.lelong@libroxml.net])
AC_CONFIG_AUX_DIR(aux_scripts)
AC_CONFIG_MACRO_DIR([m4])

# Values for SONAME. See -version-info for details.
AC_SUBST(SONAME_CURRENT, 0)
AC_SUBST(SONAME_REVISION, 0)
AC_SUBST(SONAME_AGE, 0)

# Checks for programs.
AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_LIBTOOL
AC_LIBTOOL_WIN32_DLL

AC_CONFIG_HEADERS([src/config.h])
AM_INIT_AUTOMAKE(subdir-objects)
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES])
AM_SILENT_RULES([yes])

# needed to handle 64-bit architecture
AC_CHECK_SIZEOF(int)
AC_CHECK_SIZEOF(long)
AC_CHECK_SIZEOF(size_t)

# tests some compiler flags
AC_MSG_CHECKING([whether CC supports -Wno-unused])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([])],
		[AC_MSG_RESULT([yes])]
		[AM_TEST_CFLAGS="$AM_TEST_CFLAGS -Wno-unused"],
		[AC_MSG_RESULT([no])])

AC_MSG_CHECKING([whether CC supports -Wunused-parameter])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([])],
		[AC_MSG_RESULT([yes])]
		[AM_TEST_CFLAGS="$AM_TEST_CFLAGS -Wno-unused-parameter"],
		[AC_MSG_RESULT([no])])

AC_MSG_CHECKING([whether CC supports -Wunused-variable])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([])],
		[AC_MSG_RESULT([yes])]
		[AM_TEST_CFLAGS="$AM_TEST_CFLAGS -Wno-unused-variable"],
		[AC_MSG_RESULT([no])])

AC_MSG_CHECKING([whether CC supports -Wunused-but-set-variable])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([])],
		[AC_MSG_RESULT([yes])]
		[AM_TEST_CFLAGS="$AM_TEST_CFLAGS -Wno-unused-but-set-variable"],
		[AC_MSG_RESULT([no])])

AC_SUBST(AM_TEST_CFLAGS)

AC_MSG_CHECKING([whether unittest sources are available])
if test -f unittest/unittest.c; then
	AC_MSG_RESULT([yes])
else
	AC_MSG_RESULT([no])
	enable_tests=no
fi

AC_MSG_NOTICE()
AC_MSG_NOTICE(Configuration is:)
AC_MSG_NOTICE()
AC_MSG_NOTICE(General:)

################################
# Build roxml binary
################################
AC_ARG_ENABLE(roxml,
              [AS_HELP_STRING([--disable-roxml],
              [Disable roxml binary build [default=yes]])],
              [],
              [enable_roxml=yes])
AM_CONDITIONAL(ENABLE_ROXML,test x$enable_roxml != xno)

AC_MSG_NOTICE(Roxml shell parser    ......... $enable_roxml)

################################
# Build rocat binary
################################

AC_ARG_ENABLE(rocat,
              [AS_HELP_STRING([--disable-rocat],
              [Disable rocat binary build [default=yes]])],
              [],
              [enable_rocat=yes])
AM_CONDITIONAL(ENABLE_ROCAT,test x$enable_rocat != xno)

AC_MSG_NOTICE(Rocat shell tool      ......... $enable_rocat)

################################
# Documentation
################################
AC_ARG_ENABLE(docs,
              [AS_HELP_STRING([--enable-docs],
              [Enable documentation build [default=no]])],
              [],
              [enable_docs=no])
AM_CONDITIONAL(DOCS,test x$enable_docs != xno)
if test x$enable_docs != xno; then
    AC_PATH_PROG([DOXYGEN], [doxygen])
    if test x$DOXYGEN = x; then
        AC_MSG_ERROR([doxygen not found: the docs cannot be built])
    fi
fi

AC_MSG_NOTICE(Documentation         ......... $enable_docs)

################################
# Build unit tests
################################
AC_ARG_ENABLE(tests,
	      [AS_HELP_STRING([--enable-tests],
	      [Enable unit tests build (only available from git repository)[default=no]])],
	      [],
	      [enable_tests=no])
AM_CONDITIONAL(ENABLE_TESTS,test x$enable_tests != xno)

AC_MSG_NOTICE(Unittests             ......... $enable_tests)

################################
# Fuse helper
################################
AC_ARG_WITH(fuse,
              [AS_HELP_STRING([--with-fuse],
              [Build fuse helper [default=no]])],
              [],
              [with_fuse=no])
AM_CONDITIONAL(WITH_FUSE,test x$with_fuse = xyes)
if test x$with_fuse = xyes; then
    AC_CHECK_HEADERS([fuse.h],
                     [
                     PKG_CHECK_MODULES([FUSE], [fuse], [], [
                         AC_SEARCH_LIBS([fuse_main], [fuse], [], [
                             AC_MSG_ERROR([unable to find the fuse library])
                         ])
                     ])
                     ],
                     AC_MSG_ERROR([fuse.h not found: the fuse xml helper cannot be built]),
                     [
#define _FILE_OFFSET_BITS 64
#include <fuse.h>
                     ]
    )
fi

AC_MSG_NOTICE(Fuse module           ......... $with_fuse)

################################
# Debugging options
################################
AC_ARG_ENABLE(debug,
              [AS_HELP_STRING([--enable-debug],
              [Turn on debugging [default=no]])],
              [],
              [enable_debug=no])
AM_CONDITIONAL(DEBUG,test x$enable_debug = xyes)

AC_MSG_NOTICE(Debug                 ......... $enable_debug)

################################
# Verbose XML checking
################################
AC_ARG_ENABLE(verbose,
              [AS_HELP_STRING([--enable-verbose],
              [Turn off verbose parsing on errors [default=no]])],
              [],
              [enable_verbose=no])
AM_CONDITIONAL(VERBOSE_PARSING,test x$enable_verbose = xyes)

AC_MSG_NOTICE(Verbose parsing      .......... $enable_verbose)

################################
# Tune options
################################
AC_MSG_NOTICE()
AC_MSG_NOTICE(Tuning:)

AC_DEFUN([AC_ROXML_TUNE],
[
    AC_ARG_ENABLE($1,
                  [AS_HELP_STRING([$3],
                  [$4 [default=$2]])],
                  [
                      if test x$enableval = xno; then
                          $1=no
                      else
                          $1=yes
                      fi
                  ],
                  [$1=$2]
    )
    if test x$$1 != xno; then
        roxml_tune=1
	AC_MSG_NOTICE([$6 ......... $7])
	AC_DEFINE_UNQUOTED([$5], [$roxml_tune], [$4 [default=$2]])
    else
        roxml_tune=0
	AC_MSG_NOTICE([$6 ......... $8])
    fi
    AM_CONDITIONAL([$5], [ test $roxml_tune -eq 1])
])

# CONFIG_XML_FLOAT
AC_ROXML_TUNE(float,
              [yes],
              [--disable-float],
              [Does the target library support float XML values.],
              [CONFIG_XML_FLOAT],
	      Support float        ,yes,no)

# CONFIG_XML_THREAD_SAFE
AC_ROXML_TUNE(threadsafe,
              [yes],
              [--disable-threadsafe],
              [Is the target library thread safe or not.],
              [CONFIG_XML_THREAD_SAFE],
	      Thread safety        ,yes,no)

# CONFIG_XML_HEAP_BUFFERS
AC_ROXML_TUNE(heapbuffers,
              [no],
              [--enable-heapbuffers],
              [Is the target library uses stack for internal buffers or heap. This is not compatible with thread safety.],
              [CONFIG_XML_HEAP_BUFFERS],
	      Buffers type         ,stack,heap)

# CONFIG_XML_SMALL_BUFFER
AC_ROXML_TUNE(smallbuffer,
              [no],
              [--enable-smallbuffer],
              [Define the size of internal buffer. For very small systems, large internal buffers can cause the systeml the behave strangely.],
              [CONFIG_XML_SMALL_BUFFER],
	      Internal buffers     ,small,standard)

# CONFIG_XML_SMALL_INPUT_FILE
AC_ROXML_TUNE(smallinputfile,
              [no],
              [--enable-smallinputfile],
              [Limit the size of input XML libroxml can handle to 64kb instead of 4Gb. Setting this option will reduce the size of RAM loaded tree.],
              [CONFIG_XML_SMALL_INPUT_FILE],
	      Input file           ,small (< 64kB),large (> 64kB))

# CONFIG_XML_FILE
AC_ROXML_TUNE(file,
              [yes],
              [--disable-file],
              [Can the target library parse file documents. When set to 0, this result in smaller binary.],
              [CONFIG_XML_FILE],
	      File parsing support ,yes,no)

# CONFIG_XML_EDIT
AC_ROXML_TUNE(edit,
              [yes],
              [--disable-edit],
              [Can the target library modify XML tree. When set to 0, this result in smaller binary (this saves about 3kb).],
              [CONFIG_XML_EDIT],
	      Write support        ,yes,no)

# CONFIG_XML_COMMIT
AC_ROXML_TUNE(commit,
              [yes],
              [--disable-commit],
              [Can the target library commit a tree. When set to 0, this result in smaller binary (this saves about 5kb).],
              [CONFIG_XML_COMMIT],
	      Commit support       ,yes,no)

# CONFIG_XML_XPATH
AC_ROXML_TUNE(xpath,
              [yes],
              [--disable-xpath],
              [Can the target library perform xpath queries. When set to 0, this result in smaller binary (this saves about 20kb).],
              [CONFIG_XML_XPATH],
	      Xpath support        ,yes,no)

################################
# Output files
################################
AC_MSG_NOTICE()
AC_CONFIG_FILES(Makefile libroxml.pc docs/doxy.cfg docs/man.cfg src/roxml_doxy.h)
AC_OUTPUT

