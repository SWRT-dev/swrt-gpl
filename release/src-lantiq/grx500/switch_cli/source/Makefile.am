#
#                              Copyright (c) 2010
#                            Lantiq Deutschland GmbH
#                     Am Campeon 3; 85579 Neubiberg, Germany
#
#  For licensing information, see the file 'LICENSE' in the root folder of
#  this software module.
#


AUTOMAKE_OPTIONS = foreign 1.6 nostdinc
ACLOCAL_AMFLAGS = -I m4

SUBDIRS = src

# set distcheck options - see configure.in for list of modifications
DISTCHECK_CONFIGURE_FLAGS=@CONFIGURE_OPTIONS@

#switch_cli_docdir = ${prefix}/doc/switch_cli

#switch_cli_doc_DATA = \
#    ChangeLog \
#    LICENSE

#EXTRA_DIST = \
#    $(switch_cli_doc_DATA) \
#    autogen.sh

# Copy all the spec files. Of cource, only one is actually used.
dist-hook:
	for specfile in *.spec; do \
		if test -f $$specfile; then \
			cp -p $$specfile $(distdir); \
		fi \
	done

# Create self extracting linux distribution
distcheck-hook:
	@echo -n "Checking for the line ending style... "; \
	failure_list="`find $(distdir) -path '$(distdir)/doc' -prune -o -type f -exec grep -Pqsc $$'\r\n' {} \; -printf '\tDOS line ending style: %p\n'`"; \
	if test -z "$$failure_list"; then \
		echo "Ok"; \
	else \
		echo "Fail"; \
		echo "$$failure_list"; \
		exit -1; \
	fi
	makeself.sh --gzip --notemp $(distdir) switch_cli-${PACKAGE_VERSION}.sh \
	"${PACKAGE_NAME}"

lint:
	@target=lint; \
    list='$(SUBDIRS)'; \
    for subdir in $$list; do \
        echo "Making $$target in $$subdir"; \
        if test "$$subdir" = "."; then \
            dot_seen=yes; \
            local_target="$$target-am"; \
        else \
            local_target="$$target"; \
        fi; \
        (cd $$subdir && $(MAKE) $(AM_MAKEFLAGS) $$local_target) \
        || eval $$failcom; \
    done;

rsm:
	@target=rsm; \
    list='$(SUBDIRS)'; \
    for subdir in $$list; do \
        echo "Making $$target in $$subdir"; \
        if test "$$subdir" = "."; then \
            dot_seen=yes; \
            local_target="$$target-am"; \
        else \
            local_target="$$target"; \
        fi; \
        (cd $$subdir && $(MAKE) $(AM_MAKEFLAGS) $$local_target) \
        || eval $$failcom; \
    done;
