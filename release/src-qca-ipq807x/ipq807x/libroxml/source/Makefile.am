## Process this file with automake to generate Makefile.in

VERSION_INFO = -version-info $(SONAME_CURRENT):$(SONAME_REVISION):$(SONAME_AGE)

if DEBUG
  DEBUG_CFLAGS = -DDEBUG
endif

AUTOMAKE_OPTIONS = foreign nostdinc
ACLOCAL_AMFLAGS = -I m4
AM_CPPFLAGS = -I$(top_srcdir)/src -I$(top_builddir)/src
AM_LD = $(CC)
AM_CFLAGS = -Wall $(DEBUG_CFLAGS)
EXTRA_DIST =
CLEANFILES =

# Project headers
include_HEADERS = src/roxml.h
noinst_HEADERS = \
	src/roxml_defines.h \
	src/roxml_internal.h \
	src/roxml_parser.h \
	src/roxml_tune.h \
	src/roxml_types.h \
	src/roxml_buff.h \
	src/roxml_core.h \
	src/roxml_doxy.h \
	src/roxml_file.h \
	src/roxml.h \
	src/roxml_mem.h \
	src/roxml_xpath.h \
	src/roxml_utils.h \
	src/roxml_win32_native.h
EXTRA_DIST += src/roxml_doxy.h.in

# Libraries
lib_LTLIBRARIES = libroxml.la

# The libroxml library
libroxml_la_SOURCES = src/roxml_core.c \
			src/roxml_parser.c \
			src/roxml_buff.c \
			src/roxml_nav.c \
			src/roxml_content.c \
			src/roxml_mem.c \
			src/roxml_stub.c

libroxml_la_CFLAGS = -DIGNORE_EMPTY_TEXT_NODES \
			-DCONFIG_XML_CONTENT \
			-DCONFIG_XML_NAV \
			-DCONFIG_XML_BUFF

if VERBOSE_PARSING
libroxml_la_CFLAGS += -DVERBOSE_PARSING
endif
if CONFIG_XML_COMMIT
libroxml_la_SOURCES += src/roxml_commit.c
libroxml_la_CFLAGS += -DCONFIG_XML_COMMIT
endif
if CONFIG_XML_EDIT
libroxml_la_SOURCES += src/roxml_edit.c
libroxml_la_CFLAGS += -DCONFIG_XML_EDIT
endif
if CONFIG_XML_FILE
libroxml_la_SOURCES += src/roxml_file.c
libroxml_la_CFLAGS += -DCONFIG_XML_FILE
endif
if CONFIG_XML_XPATH
libroxml_la_SOURCES += src/roxml_xpath.c
libroxml_la_CFLAGS += -DCONFIG_XML_XPATH
endif

libroxml_la_LDFLAGS = $(VERSION_INFO)

bin_PROGRAMS =

# The roxml utility
if ENABLE_ROXML
  bin_PROGRAMS += roxml
  roxml_SOURCES = src/roxml_shell.c
  roxml_LDADD = libroxml.la
endif

# The rocat utility
if ENABLE_ROCAT
  bin_PROGRAMS += rocat
  rocat_SOURCES = src/roxml_cat.c
  rocat_LDADD = libroxml.la
endif

# mount.fuse.xml helper
if WITH_FUSE
  bin_PROGRAMS += mount.fuse.xml
  mount_fuse_xml_SOURCES = fuse.xml/xmlfs.c
  mount_fuse_xml_CFLAGS = -D_FILE_OFFSET_BITS=64 $(FUSE_CFLAGS)
  mount_fuse_xml_LDADD = libroxml.la $(FUSE_LIBS)
endif
EXTRA_DIST += fuse.xml/README

# pkg-config .pc file
pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libroxml.pc
EXTRA_DIST += libroxml.pc.in

# Additional project files
EXTRA_DIST += \
	License.txt \
	CMakeLists.txt

# Documentation
EXTRA_DIST += \
	data/html \
	data/icons \
	docs

if DOCS
docs: doxygen-docs
doxygen-docs: doxygen-html doxygen-man

doxygen-html:
	$(AM_V_GEN)$(DOXYGEN) docs/doxy.cfg >/dev/null 2>&1
	$(AM_V_GEN)cp $(top_srcdir)/data/icons/roxml.png docs/html/
	$(AM_V_GEN)cp $(top_srcdir)/data/icons/libroxml-ex.png docs/html/

doxygen-man:
	$(AM_V_GEN)$(DOXYGEN) docs/man.cfg >/dev/null 2>&1
	$(AM_V_GEN)chmod -R a+rw docs

clean-local: docs-clean
docs-clean:
	rm -Rf \
	docs/man \
	docs/html \
	docs/latex

man1_MANS = $(top_srcdir)/docs/roxml.1
nodist_man3_MANS = docs/man/man3/*.3

nodist_html_DATA = docs/html/*

$(man1_MANS): docs
$(nodist_man3_MANS): docs
$(nodist_html_DATA): docs

endif

# Unit tests

if ENABLE_TESTS

noinst_bin_PROGRAMS = \
	unittest/roxml.unitC \
	unittest/roxml.perf \
	unittest/roxml.mem \
	unittest/roxml.tiny.unitC
noinst_bindir = unittest

nodist_noinst_HEADERS = unittest/unittest.h

# Test libraries
noinst_LIBRARIES = \
		unittest/libroxml_test.a \
		unittest/libroxml_test_tiny.a

TEST_CFLAGS = -DUNITTEST -DREENTRANT -DLINUX -D__DEBUG -DIGNORE_EMPTY_TEXT_NODES -Wall -Wextra @AM_TEST_CFLAGS@ \
			-DCONFIG_XML_CONTENT \
			-DCONFIG_XML_NAV \
			-DCONFIG_XML_BUFF \
			-DCONFIG_XML_COMMIT \
			-DCONFIG_XML_EDIT \
			-DCONFIG_XML_FILE \
			-DCONFIG_XML_XPATH

# Test library libroxml_test.a
unittest_libroxml_test_a_SOURCES = \
	src/roxml_buff.c \
	src/roxml_commit.c \
	src/roxml_content.c \
	src/roxml_core.c \
	src/roxml_edit.c \
	src/roxml_file.c \
	src/roxml_mem.c \
	src/roxml_nav.c \
	src/roxml_parser.c \
	src/roxml_xpath.c \
	src/roxml_stub.c

unittest_libroxml_test_a_CFLAGS = $(TEST_CFLAGS)

# Test library libroxml_test_tiny.a
unittest_libroxml_test_tiny_a_SOURCES = \
	src/roxml_buff.c \
	src/roxml_content.c \
	src/roxml_core.c \
	src/roxml_file.c \
	src/roxml_mem.c \
	src/roxml_nav.c \
	src/roxml_parser.c \
	src/roxml_stub.c

unittest_libroxml_test_tiny_a_CFLAGS = \
	$(TEST_CFLAGS) \
	-UHAVE_CONFIG_H \
	-DCONFIG_XML_HEAP_BUFFERS=1 \
	-DCONFIG_XML_SMALL_BUFFER=1 \
	-DCONFIG_XML_SMALL_INPUT_FILE=1 \
	-DCONFIG_XML_THREAD_SAFE=0 \
	-UCONFIG_XML_COMMIT \
	-UCONFIG_XML_EDIT \
	-UCONFIG_XML_XPATH

# roxml.unitC
nodist_unittest_roxml_unitC_SOURCES = \
	unittest/unittest.c \
	unittest/roxml.test.c
unittest_roxml_unitC_LDADD = unittest/libroxml_test.a
unittest_roxml_unitC_CFLAGS = $(TEST_CFLAGS)

# roxml.perf
nodist_unittest_roxml_perf_SOURCES = unittest/roxml.perf.c
unittest_roxml_perf_LDADD = unittest/libroxml_test.a
unittest_roxml_perf_CFLAGS = -O3 -DREENTRANT -DLINUX -Wall

# roxml.mem
nodist_unittest_roxml_mem_SOURCES = unittest/roxml.mem.c
unittest_roxml_mem_LDADD = unittest/libroxml_test.a
unittest_roxml_mem_CFLAGS = $(TEST_CFLAGS)

# roxml.tiny.unitC
nodist_unittest_roxml_tiny_unitC_SOURCES = \
	unittest/unittest.c \
	unittest/roxml.test.c
unittest_roxml_tiny_unitC_LDADD = unittest/libroxml_test_tiny.a
unittest_roxml_tiny_unitC_CFLAGS = $(TEST_CFLAGS) \
	-UHAVE_CONFIG_H \
	-DCONFIG_XML_HEAP_BUFFERS=1 \
	-DCONFIG_XML_SMALL_BUFFER=1 \
	-DCONFIG_XML_SMALL_INPUT_FILE=1 \
	-DCONFIG_XML_THREAD_SAFE=0 \
	-UCONFIG_XML_COMMIT \
	-UCONFIG_XML_EDIT \
	-UCONFIG_XML_XPATH

CLEANFILES += \
		unittest/out.xml.copy \
		unittest/out.xml out.xml.human \
		unittest/out.xml.spec.copy \
		unittest/out.xml.valid.human \
		unittest/out.buf.xml \
		unittest/out.buf.xml.human \
		unittest/out.xml.ns.generated \
		unittest/out.xml.ns \
		unittest/out.xml.human.sub \
		unittest/out.xml.ns.del \
		unittest/out.xml.modattr \
		unittest/out.xml.modattr2 \
		unittest/out.xml.modattr3 \
		unittest/out.xml.modattr4 \
		unittest/out.xml.ns.generated2 \
		unittest/out.xml.valid.raw \
		unittest/out.xml.valid.raw2 \
		unittest/out.xml.valid.raw3 \
		unittest/out.xml.valid.raw4 \
		unittest/out.xml.valid.xpath \
		unittest/out.xml.valid.xpath2 \
		unittest/out.xml.valid.xpath3 \
		unittest/out.xml.valid.xpath4

endif
