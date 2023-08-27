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

# Package dependence, sync with spf11.1/upb-platform_router.mak
PKGDEP_FTM		:= libnl libtcmd qca-diag qca-iot
PKGDEP_IW		:= libnl-tiny
PKGDEP_IWINFO		:= iw libnl-tiny qca-wifi
PKGDEP_LIBIEEE1905	:= libqcacommon
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
PKGDEP_QCA_SHORTCUT_FE	:= libnl libdaemon
PKGDEP_QCA_WHC_LBD	:= libqcacommon jansson-2.7 qca-wifi libwifisoncfg qca-wifison-ext-lib libieee1905
PKGDEP_QCA_WIFI		:= qca-nss-drv iproute2-3.x libxml2 libnl qca-cfg80211
PKGDEP_QCA_WIGIG_DEBUG_TOOLS := libnl
PKGDEP_QCA_WIGIG_DRIVER	:= qca-nss-drv #mac80211-scripts

#
# software package selection
#
pobj-y:=

# IPQ8074A.ILQ.10.0
pobj-$(RTCONFIG_SOC_IPQ8074) += qca-nss-drv $(PKGDEP_QCA_NSS_DRV)
pobj-$(RTCONFIG_SOC_IPQ8074) += qca-nss-dp $(PKGDEP_QCA_NSS_DP)
pobj-$(RTCONFIG_SOC_IPQ8074) += qca-nss-ecm $(PKGDEP_QCA_NSS_ECM)
pobj-$(RTCONFIG_SOC_IPQ8074) += qca-nss-fw-eip-hk
pobj-$(RTCONFIG_SOC_IPQ8074) += qca-nss-fw-hk
pobj-$(RTCONFIG_SOC_IPQ8074) += qca-nss-crypto $(PKGDEP_QCA_NSS_CRYPTO)
pobj-$(RTCONFIG_SOC_IPQ8074) += qca-nss-cfi $(PKGDEP_QCA_NSS_CFI)
pobj-$(RTCONFIG_SOC_IPQ8074) += qca-nss-clients $(PKGDEP_QCA_NSS_CLIENT)
pobj-$(RTCONFIG_SOC_IPQ8074) += qca-nss-macsec
pobj-$(RTCONFIG_SOC_IPQ8074) += qca-ssdk
pobj-$(RTCONFIG_SOC_IPQ8074) += qca-mcs-lkm
pobj-$(RTCONFIG_SOC_IPQ8074) += qca-mcs-apps $(PKGDEP_QCA_MCS_APPS)
pobj-$(RTCONFIG_SOC_IPQ8074) += qca-ssdk-shell
pobj-$(RTCONFIG_SOC_IPQ8074) += shortcut-fe $(PKGDEP_QCA_SHORTCUT_FE)
pobj-$(RTCONFIG_SOC_IPQ8074) += qca-whc-lbd $(PKGDEP_QCA_WHC_LBD)
pobj-$(RTCONFIG_SOC_IPQ8074) += qca-thermald
pobj-$(RTCONFIG_WIFI_QCN5024_QCN5054) += athtestcmd-lith
pobj-$(RTCONFIG_WIFI_QCN5024_QCN5054) += qca-wifi $(PKGDEP_QCA_WIFI)
pobj-$(RTCONFIG_WIFI_QCN5024_QCN5054) += qca-hk
pobj-$(RTCONFIG_WIFI_QCN5024_QCN5054) += qca-acfg $(PKGDEP_QCA_ACFG)
pobj-$(RTCONFIG_WIFI_QCN5024_QCN5054) += qca-cfg80211 $(PKGDEP_QCA_CFG80211)
pobj-$(RTCONFIG_WIFI_QCN5024_QCN5054) += qca-cnssdiag $(PKGDEP_QCA_CNSSDIAG)
pobj-$(RTCONFIG_WIFI_QCN5024_QCN5054) += qca-hostap $(PKGDEP_QCA_HOSTAP)
pobj-$(RTCONFIG_WIFI_QCN5024_QCN5054) += ftm $(PKGDEP_FTM)
#pobj-$(RTCONFIG_SWITCH_QCA8075_QCA8337_PHY_AQR107_AR8035_QCA8033) += mcproxy
pobj-$(RTCONFIG_WIFI_QCN5024_QCN5054) += qca-cnss-daemon $(PKGDEP_QCA_CNSS_DAEMON)

# Wi-Fi SON related
pobj-$(RTCONFIG_WIFI_QCN5024_QCN5054) += qca-hyfi-qdisc
pobj-$(RTCONFIG_WIFI_QCN5024_QCN5054) += qca-hyfi-bridge $(PKGDEP_QCA_HYFI_BRIDGE)
pobj-$(RTCONFIG_WIFI_QCN5024_QCN5054) += qca-libhyfi-bridge $(PKGDEP_QCA_LIBHYFI_BRIDGE)
ifeq ($(HIVE_LIB),y)
pobj-y += libieee1905
endif
ifeq ($(HIVE_BIN),y)
pobj-y += libevent-2.0.21 json-c
pobj-y += qca-hyctl
endif

# Bluetooth
pobj-y += btdiag

# Wigig
pobj-$(RTCONFIG_WIGIG) += qca-wigig-driver $(PKGDEP_QCA_WIGIG_DRIVER)
pobj-$(RTCONFIG_WIGIG) += wigig-firmware
#pobj-$(RTCONFIG_WIGIG) += qca-wigig-debug-tools $(PKGDEP_QCA_WIGIG_DEBUG_TOOLS)
pobj-$(RTCONFIG_WIGIG) += iw $(PKGDEP_IW)
pobj-$(RTCONFIG_WIGIG) += iwinfo $(PKGDEP_IWINFO)

pobj-$(RTCONFIG_SWITCH_QCA8075_PHY_AQR107) += aq-fw-download
pobj-$(RTCONFIG_SWITCH_QCA8075_QCA8337_PHY_AQR107_AR8035_QCA8033) += aq-fw-download

# U-Boot binaries
pobj-$(RTCONFIG_SWITCH_QCA8075_QCA8337_PHY_AQR107_AR8035_QCA8033) += uboot_bins

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
	ln -s qsdk64.config qsdk.config

$(pobj-y) $(pobj-n) $(pobj-clean) $(pobj-install): dummy

.PHONY: all clean distclean mrproper install package image
.PHONY: conf mconf oldconf kconf kmconf config menuconfig oldconfig
.PHONY: dummy libnet libpcap


