#
# Platform specific router Makefile
#
# Copyright 2016, Broadcom
# All Rights Reserved.
#
# THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
# KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
# SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
# FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
#
#

include ../router/common.mak
include ../router/.config
include qsdk.config

PLATFORM_ROUTER_SRCBASE:=$(shell pwd)
export PLATFORM_ROUTER_SRCBASE
export INSTALL_BIN:=install -m0755
export INSTALL_DIR:=install -d -m0755
export INSTALL_DATA:=install -m0644
export INSTALL_CONF:=install -m0600

export CFLAGS+=$(EXTRACFLAGS)
export LDFLAGS+=$(EXTRALDFLAGS)

# Package dependence
PKGDEP_QCA_HOSTAP	:= qca-wifi-10.4 libnl openssl
PKGDEP_QCA_WIFI		:= iproute2-4.4.0 libxml2 libnl qca-wifi-fw-10.4
PKGDEP_QCA_EZMESH	:= libhyficommon libwifisoncfg libubus libubox libuci libwpa2 jansson bridge qti-autorole qca-libhyfi-bridge kmod-qca-hyfi-bridge qca-ssdk
PKGDEP_QCA_EZMESH	+= qca-wifi-10.4

#
# software package selection
#
pobj-y:=

pobj-y += qca-edma qca-swconfig.ipq40xx
#pobj-y += qcmbr-10.4 art2
pobj-y += qca-ssdk
pobj-y += qca-ssdk-shell
pobj-y += shortcut-fe
pobj-y += qca-wifi-10.4 $(PKGDEP_QCA_WIFI)
pobj-y += qca-wifi-fw.ipq40xx
ifeq ($(RTCONFIG_PCIE_QCA9984),y)
pobj-y += qca-wifi-fw.qca9984
endif
pobj-y += qca-hostap-10.4 $(PKGDEP_QCA_HOSTAP)
pobj-y += qca-wrapd-10.4
ifeq ($(RTCONFIG_EASYMESH),y)
pobj-y += qca-ezmesh $(PKGDEP_QCA_EZMESH)
endif
ifeq ($(RTCONFIG_BT_CONN),y)
pobj-y += btconfig
endif


pobj-clean := $(foreach pobj, $(pobj-y) $(pobj-n) $(pobj-), $(pobj)-clean)
pobj-install := $(foreach pobj,$(pobj-y),$(pobj)-install)

#
# Basic rules
#

all: $(pobj-y)

install: $(pobj-install) gen_target

gen_target:
	[ -d $(TARGETDIR) ] || install -d $(TARGETDIR)
	for dir in $(wildcard $(patsubst %,$(INSTALLDIR)/%,$(pobj-y))) ; do \
		(cd $${dir} && tar cpf - .) | (cd $(TARGETDIR) && tar xpf -) \
	done

clean: $(pobj-clean)

distclean: clean

#
# include rules for platform specific software packages
#
-include $(wildcard mak/*.mak)
-include $(wildcard rules/*.mak)

#
# configuration
#

#
# overrides and extra dependencies
#

#
# Generic rules
#

%:
	@[ ! -d $* ] || [ -f $*/Makefile ] || $(MAKE) $*-configure
	@[ ! -d $* ] || [ -d $*/prebuild ] || ( $(MAKE) -C $* )


%-clean:
	-@[ ! -d $* ] || $(MAKE) -C $* clean


%-install: %
	@echo $*
	@[ ! -d $* ] || $(MAKE) -C $* install INSTALLDIR=$(INSTALLDIR)/$*

%-stage:
	@echo $*
	@[ ! -d $* ] || $(MAKE) -C $* install DESTDIR=$(STAGEDIR) INSTALLDIR=$(INSTALLDIR)/$*

%-build:
	$(MAKE) $*-clean $*

%/Makefile:
	[ ! -d $* ] || $(MAKE) $*-configure

%-configure:
	@[ ! -d $* ] || ( cd $* ; \
		$(CONFIGURE) \
		--prefix=/usr \
		--bindir=/usr/sbin \
		--libdir=/usr/lib \
	)

$(pobj-y) $(pobj-n) $(pobj-clean) $(pobj-install): dummy

.PHONY: all clean distclean mrproper install package image
.PHONY: conf mconf oldconf kconf kmconf config menuconfig oldconfig
.PHONY: dummy libnet libpcap


