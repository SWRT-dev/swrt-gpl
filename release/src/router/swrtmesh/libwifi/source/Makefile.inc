CP ?= cp -fpPR
INSTALL ?= install
INSTALL_DIR = $(INSTALL) -d -m 755
INSTALL_PROGRAM = $(INSTALL) -m 755
INSTALL_DATA = $(INSTALL) -m 644
INSTALL_CONF = $(INSTALL) -m 600
INSTALL_LIB = $(CP)

INCDIR ?= /usr/include
LIBDIR ?= /usr/lib


header-files ?= $(wildcard *.h)
library-files ?= $(wildcard *.so*)


$(DESTDIR)/$(INCDIR)/%: %
	$(INSTALL_DATA) -D $< $@

$(DESTDIR)/$(LIBDIR)/%: %
	$(INSTALL_DIR) $(DESTDIR)/$(LIBDIR)
	$(INSTALL_LIB) $< $@


install-headers: $(addprefix $(DESTDIR)/$(INCDIR)/,$(header-files))

install-libs: $(addprefix $(DESTDIR)/$(LIBDIR)/,$(library-files))
