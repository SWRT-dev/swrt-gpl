ACLOCAL_AMFLAGS = -I m4

lib_LTLIBRARIES = libswpal.la

libswpal_la_SOURCES = uci_wrapper.c

libswpal_la_LDFLAGS = -luci

AM_CPPFLAGS = -Wall -Werror -I${top_srcdir}/include $(CPPFLAGS) -DPACKAGE_ID=\"WAVE_HAL\" -DLOGGING_ID -DLOG_LEVEL=6 -DLOG_TYPE=1 -DYOCTO \
                  -Werror=format-security -Wl,-strip-debug -ffunction-sections -fdata-sections -Wno-maybe-uninitialized
AM_LDFLAGS =  -Wl,--gc-sections

pkgconfigdir = $(libdir)/pkgconfig
include_HEADERS = $(srcdir)/include/uci_wrapper.h \
                  $(srcdir)/include/help_logging.h

bin_PROGRAMS = lite_nc
lite_nc_SOURCES = $(srcdir)/lite_nc/nc_help_lite.c \
                  $(srcdir)/lite_nc/nc_help_lite.h \
                  $(srcdir)/lite_nc/nc_lite.c
