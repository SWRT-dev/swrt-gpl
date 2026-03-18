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
include $(SRCBASE)/.config

export CFLAGS+=$(EXTRACFLAGS)
export LDFLAGS+=$(EXTRALDFLAGS)

#
# software package selection
#
pobj-y:=

pobj-y += libroxml 464xlat nat46 libtcmd athtestcmd-lith libathtestcmd libtlv2 libtlvutil
#pobj-$(RTCONFIG_SOC_IPQ50XX) += breakpad breakpad-wrapper
pobj-$(RTCONFIG_SOC_IPQ50XX) += qca-ssdk qca-ssdk-shell
pobj-$(RTCONFIG_SOC_IPQ50XX) += qca-wifi qca-cfg80211 libqcacommon
pobj-$(RTCONFIG_SOC_IPQ50XX) += iw iwinfo qca-hostap
pobj-$(RTCONFIG_SOC_IPQ50XX) += libdaemon qca-mcs-lkm qca-nss-sfe
pobj-$(RTCONFIG_SOC_IPQ50XX) += qca-nss-drv qca-nss-dp qca-nss-ecm qca-hyfi-bridge emesh-sp
pobj-$(RTCONFIG_SOC_IPQ50XX) += qca-nss-crypto qca-nss-cfi qca-nss-clients qca-nss-udp-st qca-nss-udp-st-drv
pobj-$(RTCONFIG_SOC_IPQ50XX) += qca-nss-fw-mp qca-nss-macsec
pobj-$(RTCONFIG_SOC_IPQ50XX) += qca-mcs-apps 
pobj-$(RTCONFIG_SOC_IPQ50XX) += libieee1905
pobj-$(RTCONFIG_SOC_IPQ50XX) += qca-thermald
pobj-$(RTCONFIG_SOC_IPQ50XX) += myftm
pobj-$(RTCONFIG_SOC_IPQ50XX) += qca-wifi-fw
pobj-$(RTCONFIG_SOC_IPQ50XX) += qca-acfg
pobj-$(RTCONFIG_SOC_IPQ50XX) += qca-cnssdiag qca-diag
pobj-$(RTCONFIG_SOC_IPQ50XX) += libprovision ftm qca-iot
#pobj-$(RTCONFIG_SOC_IPQ50XX) += mcproxy
pobj-$(RTCONFIG_SOC_IPQ50XX) += qca-cnss qca-cnss-daemon qca-qmi-framework


# Bluetooth
pobj-y += btdiag

pobj-$(RTCONFIG_QCA_LBD) += qca-whc-lbd libwifisoncfg qca-wifison-ext-lib


pobj-clean := $(foreach pobj, $(pobj-y) $(pobj-n) $(pobj-), $(pobj)-clean)
pobj-install := $(foreach pobj,$(pobj-y),$(pobj)-install)

#
# Basic rules
#

all: qsdk.config $(pobj-y)

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

#
# configuration
#

#
# overrides and extra dependencies
#

#
# Generic rules
#

%: qsdk.config
	@[ ! -d $* ] || [ -f $*/Makefile ] || $(MAKE) $*-configure
	@[ ! -d $* ] || ( $(MAKE) -C $* )


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

qsdk.config:
	ln -sf qsdk64.config qsdk.config

$(pobj-y) $(pobj-n) $(pobj-clean) $(pobj-install): dummy

.PHONY: all clean distclean mrproper install package image
.PHONY: conf mconf oldconf kconf kmconf config menuconfig oldconfig
.PHONY: dummy libnet libpcap


