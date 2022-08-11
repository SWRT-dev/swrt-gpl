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

# Package dependence
PKGDEP_FTM		:= libnl libtcmd qca-diag qca-IOT
PKGDEP_IW		:= libnl-tiny
PKGDEP_IWINFO		:= iw libnl-tiny qca-wifi
PKGDEP_QCA_ACFG		:= libxml2 qca-wifi libnl qca-cfg80211 qca-hostap
PKGDEP_QCA_CFG80211	:= libxml2 libnl iw libnl-tiny
PKGDEP_QCA_CNSSDIAG	:= qca-diag
PKGDEP_QCA_CNSS_DAEMON	:= libnl qca-qmi-framework
PKGDEP_QCA_HOSTAP	:= qca-wifi libnl openssl qca-nss-macsec
PKGDEP_QCA_HYFI_BRIDGE	:= qca-ssdk
PKGDEP_QCA_LIBHYFI_BRIDGE := qca-hyfi-bridge
PKGDEP_QCA_MCS_APPS	:= qca-mcs-lkm qca-ssdk qca-wifi libqcacommon
PKGDEP_QCA_NSS_CFI	:= qca-nss-crypto
PKGDEP_QCA_NSS_CLIENT	:= qca-nss-drv qca-nss-crypto nat46
PKGDEP_QCA_NSS_CRYPTO	:= qca-nss-drv
PKGDEP_QCA_NSS_DP	:= qca-ssdk
PKGDEP_QCA_NSS_DRV	:= qca-nss-gmac qca-nss-dp
PKGDEP_QCA_NSS_ECM	:= qca-nss-drv shortcut-fe nat46 qca-mcs-lkm qca-hyfi-bridge
#PKGDEP_QCA_SHORTCUT_FE	:= libnl libdaemon
PKGDEP_QCA_WHC_LBD	:= libqcacommon jansson qca-wifi libwifisoncfg qca-wifison-ext-lib
PKGDEP_QCA_WIFI		:= qca-nss-drv iproute2-3.x libxml2 libnl qca-cfg80211
PKGDEP_QCA_WIGIG_DEBUG_TOOLS := libnl
PKGDEP_QCA_WIGIG_DRIVER	:= qca-nss-drv #mac80211-scripts

#
# software package selection
#
pobj-y:=

pobj-y += qca-nss-drv $(PKGDEP_QCA_NSS_DRV)
pobj-y += qca-nss-dp $(PKGDEP_QCA_NSS_DP)
pobj-y += qca-nss-ecm $(PKGDEP_QCA_NSS_ECM)
pobj-y += qca-nss-fw
pobj-y += qca-nss-crypto $(PKGDEP_QCA_NSS_CRYPTO)
pobj-y += qca-nss-cfi $(PKGDEP_QCA_NSS_CFI)
pobj-y += qca-nss-clients $(PKGDEP_QCA_NSS_CLIENT)
pobj-y += qca-nss-macsec
pobj-y += qca-ssdk
pobj-y += qca-mcs-lkm
pobj-y += qca-mcs-apps $(PKGDEP_QCA_MCS_APPS)
pobj-y += qca-ssdk-shell
pobj-y += shortcut-fe $(PKGDEP_QCA_SHORTCUT_FE)
pobj-y += qca-whc-lbd $(PKGDEP_QCA_WHC_LBD)
pobj-y += qca-thermald
#pobj-y += athtestcmd-lith
pobj-y += qca-wifi $(PKGDEP_QCA_WIFI)
pobj-y += qca-wifi-fw.ipq60xx
pobj-y += qca-acfg $(PKGDEP_QCA_ACFG)
pobj-y += qca-cfg80211 $(PKGDEP_QCA_CFG80211)
pobj-y += qca-cnssdiag $(PKGDEP_QCA_CNSSDIAG)
pobj-y += qca-hostap $(PKGDEP_QCA_HOSTAP)
pobj-y += ftm $(PKGDEP_FTM)
#pobj-$(RTCONFIG_SWITCH_QCA8075_QCA8337_PHY_AQR107_AR8035_QCA8033) += mcproxy
pobj-y += qca-cnss-daemon $(PKGDEP_QCA_CNSS_DAEMON)

# Bluetooth
pobj-y += btdiag
#

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
	echo "ok"
#ifeq ($(MUSL64),y)
#	ln -s qsdk64.config qsdk.config
#else ifeq ($(MUSL32),y)
#	ln -s qsdk32.config qsdk.config
#else
#	ln -s qsdk32.config qsdk.config
#endif

$(pobj-y) $(pobj-n) $(pobj-clean) $(pobj-install): dummy

.PHONY: all clean distclean mrproper install package image
.PHONY: conf mconf oldconf kconf kmconf config menuconfig oldconfig
.PHONY: dummy libnet libpcap


