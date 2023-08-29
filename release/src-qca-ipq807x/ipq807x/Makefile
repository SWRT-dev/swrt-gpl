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

pobj-y += libroxml
pobj-$(RTCONFIG_SOC_IPQ8074) += qca-ssdk qca-ssdk-shell
pobj-$(RTCONFIG_SOC_IPQ8074) += qca-wifi qca-cfg80211 libqcacommon
pobj-$(RTCONFIG_SOC_IPQ8074) += iw iwinfo qca-hostap
pobj-$(RTCONFIG_SOC_IPQ8074) += shortcut-fe libdaemon qca-mcs-lkm #nat46
pobj-$(RTCONFIG_SOC_IPQ8074) += qca-nss-drv qca-nss-dp qca-nss-ecm qca-hyfi-bridge emesh-sp
pobj-$(RTCONFIG_SOC_IPQ8074) += qca-nss-crypto qca-nss-cfi qca-nss-clients
pobj-$(RTCONFIG_SOC_IPQ8074) += qca-nss-fw-eip-hk qca-nss-fw-hk qca-nss-macsec
pobj-$(RTCONFIG_SOC_IPQ8074) += qca-mcs-apps 
pobj-$(RTCONFIG_SOC_IPQ8074) += qca-whc-lbd libwifisoncfg qca-wifison-ext-lib libieee1905
pobj-$(RTCONFIG_SOC_IPQ8074) += qca-thermald
pobj-$(RTCONFIG_WIFI_QCN5024_QCN5054) += athtestcmd-lith
pobj-$(RTCONFIG_WIFI_QCN5024_QCN5054) += qca-hk
pobj-$(RTCONFIG_WIFI_QCN5024_QCN5054) += qca-acfg
pobj-$(RTCONFIG_WIFI_QCN5024_QCN5054) += qca-cnssdiag qca-diag
pobj-$(RTCONFIG_WIFI_QCN5024_QCN5054) += ftm libtcmd qca-iot
#pobj-$(RTCONFIG_SWITCH_QCA8075_QCA8337_PHY_AQR107_AR8035_QCA8033) += mcproxy
pobj-$(RTCONFIG_WIFI_QCN5024_QCN5054) += qca-cnss-daemon qca-qmi-framework

# Wi-Fi SON related
#pobj-$(RTCONFIG_WIFI_QCN5024_QCN5054) += qca-hyfi-qdisc qca-hyfi-bridge qca-libhyfi-bridge
#ifeq ($(HIVE_LIB),y)
#pobj-y += libieee1905
#endif
#ifeq ($(HIVE_BIN),y)
#pobj-y += libevent-2.0.21 json-c
#pobj-y += qca-hyctl
#endif

# Bluetooth
pobj-y += btdiag

# Wigig
pobj-$(RTCONFIG_WIGIG) += qca-wigig-driver #mac80211-scripts
pobj-$(RTCONFIG_WIGIG) += wigig-firmware
#pobj-$(RTCONFIG_WIGIG) += qca-wigig-debug-tools $(PKGDEP_QCA_WIGIG_DEBUG_TOOLS)
pobj-$(RTCONFIG_WIGIG) += iw iwinfo

pobj-$(RTCONFIG_SWITCH_QCA8075_PHY_AQR107) += aq-fw-download
pobj-$(RTCONFIG_SWITCH_QCA8075_PHY_AQR111) += aq-fw-download
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


