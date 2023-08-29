include ../qsdk.config
# Convert swrt build environment variable to qsdk
PKG_BUILD_DIR=$(shell pwd)/source
STAGING_DIR=$(STAGEDIR)
#TARGET_CROSS=$(shell basename $(CROSS_COMPILE))
TARGET_CROSS=$(CROSS_COMPILE)
subtarget:=$(CONFIG_TARGET_BOARD)

ifneq (, $(findstring $(subtarget), "ipq807x" "ipq807x_64" "ipq60xx" "ipq60xx_64" "ipq50xx" "ipq50xx_64"))
# DTLS Manager v2.0 for IPQ807x/IPQ60xx/IPQ50xx
  DTLSMGR_DIR:=v2.0
# IPsec Manager v2.0 for IPQ807x/IPQ60xx/IPQ50xx
  IPSECMGR_DIR:=v2.0
else
# DTLS Manager v1.0 for IPQ806x
  DTLSMGR_DIR:=v1.0
# IPsec Manager v1.0 for IPQ806x
  IPSECMGR_DIR:=v1.0
endif

EXTRA_CFLAGS+= \
	-I$(STAGING_DIR)/usr/include/qca-nss-drv \
	-I$(STAGING_DIR)/usr/include/qca-nss-crypto \
	-I$(STAGING_DIR)/usr/include/qca-nss-cfi \
	-I$(STAGING_DIR)/usr/include/qca-nss-gmac \
	-I$(STAGING_DIR)/usr/include/qca-ssdk \
	-I$(STAGING_DIR)/usr/include/qca-ssdk/fal \
	-I$(STAGING_DIR)/usr/include/nat46

# Build individual packages if selected
ifneq ($(CONFIG_PACKAGE_kmod-qca-nss-drv-profile),)
MAKE_OPTS+=profile=y
endif

ifneq ($(CONFIG_PACKAGE_kmod-qca-nss-drv-capwapmgr),)
MAKE_OPTS+=capwapmgr=y
EXTRA_CFLAGS += -DNSS_CAPWAPMGR_ONE_NETDEV
endif

ifneq ($(CONFIG_PACKAGE_kmod-qca-nss-drv-tun6rd),)
MAKE_OPTS+=tun6rd=m
endif

ifneq ($(CONFIG_PACKAGE_kmod-qca-nss-drv-dtlsmgr),)
MAKE_OPTS+=dtlsmgr=y
endif

ifneq ($(CONFIG_PACKAGE_kmod-qca-nss-drv-tlsmgr),)
MAKE_OPTS+=tlsmgr=y
endif

ifneq ($(CONFIG_PACKAGE_kmod-qca-nss-drv-l2tpv2),)
MAKE_OPTS+=l2tpv2=y
EXTRA_CFLAGS += -DNSS_L2TPV2_ENABLED
endif

ifneq ($(CONFIG_PACKAGE_kmod-qca-nss-drv-pptp),)
MAKE_OPTS+=pptp=y
endif

ifneq ($(CONFIG_PACKAGE_kmod-qca-nss-drv-map-t),)
MAKE_OPTS+=map-t=y
endif

ifneq ($(CONFIG_PACKAGE_kmod-qca-nss-drv-tunipip6),)
MAKE_OPTS+=tunipip6=m
endif

ifneq ($(CONFIG_PACKAGE_kmod-qca-nss-drv-qdisc),)
MAKE_OPTS+=qdisc=y
endif

ifneq ($(CONFIG_PACKAGE_kmod-qca-nss-drv-igs),)
MAKE_OPTS+=igs=y
endif

ifneq ($(CONFIG_PACKAGE_kmod-qca-nss-drv-ipsecmgr),)
EXTRA_CFLAGS+= -I$(PKG_BUILD_DIR)/exports
MAKE_OPTS+=ipsecmgr=y
endif

ifneq ($(CONFIG_PACKAGE_kmod-qca-nss-drv-ipsecmgr-klips),)
EXTRA_CFLAGS+= -I$(STAGING_DIR)/usr/include/qca-nss-ecm
MAKE_OPTS+=ipsecmgr-klips=m
# Enable netdev based binding from L2TP to IPsec if KLIPS is enabled
EXTRA_CFLAGS+= -DNSS_L2TP_IPSEC_BIND_BY_NETDEV
endif

ifneq ($(CONFIG_PACKAGE_kmod-qca-nss-drv-ipsecmgr-xfrm),)
EXTRA_CFLAGS+= -I$(STAGING_DIR)/usr/include/qca-nss-ecm
MAKE_OPTS+=ipsecmgr-xfrm=m
endif

ifneq ($(CONFIG_PACKAGE_kmod-qca-nss-drv-bridge-mgr),)
MAKE_OPTS+=bridge-mgr=y
#enable OVS bridge if ovsmgr is enabled
ifneq ($(CONFIG_PACKAGE_kmod-qca-ovsmgr),)
MAKE_OPTS+= NSS_BRIDGE_MGR_OVS_ENABLE=y
EXTRA_CFLAGS+= -I$(STAGING_DIR)/usr/include/qca-ovsmgr
endif
endif

ifneq ($(CONFIG_PACKAGE_kmod-qca-nss-drv-vlan-mgr),)
MAKE_OPTS+=vlan-mgr=y
endif

ifneq ($(CONFIG_PACKAGE_kmod-qca-nss-drv-lag-mgr),)
MAKE_OPTS+=lag-mgr=y
endif

ifneq ($(CONFIG_PACKAGE_kmod-qca-nss-drv-gre),)
EXTRA_CFLAGS+= -I$(PKG_BUILD_DIR)/exports
MAKE_OPTS+=gre=y
endif

ifneq ($(CONFIG_PACKAGE_kmod-qca-nss-drv-pppoe),)
MAKE_OPTS+=pppoe=y
endif

ifneq ($(CONFIG_PACKAGE_kmod-qca-nss-drv-netlink),)
MAKE_OPTS+=netlink=y

ifeq ($(CONFIG_KERNEL_IPQ_MEM_PROFILE),256)
EXTRA_CFLAGS+= -DNSS_NETLINK_UDP_ST_NO_RMNET_SUPPORT
else ifeq ($(CONFIG_LOWMEM_FLASH),y)
EXTRA_CFLAGS+= -DNSS_NETLINK_UDP_ST_NO_RMNET_SUPPORT
endif
endif

ifneq ($(CONFIG_PACKAGE_kmod-qca-nss-drv-ovpn-mgr),)
MAKE_OPTS+=ovpn-mgr=y
endif

ifneq ($(CONFIG_PACKAGE_kmod-qca-nss-drv-ovpn-link),)
MAKE_OPTS+=ovpn-link=y
endif

ifneq ($(CONFIG_PACKAGE_kmod-qca-nss-drv-pvxlanmgr),)
MAKE_OPTS+=pvxlanmgr=y
endif

ifneq ($(CONFIG_PACKAGE_kmod-qca-nss-drv-eogremgr),)
MAKE_OPTS+=eogremgr=y
endif

ifneq ($(CONFIG_PACKAGE_kmod-qca-nss-drv-clmapmgr),)
MAKE_OPTS+=clmapmgr=y
endif

ifneq ($(CONFIG_PACKAGE_kmod-qca-nss-drv-vxlanmgr),)
MAKE_OPTS+=vxlanmgr=y
EXTRA_CFLAGS += -DNSS_VXLAN_ENABLED
endif

ifneq ($(CONFIG_PACKAGE_kmod-qca-nss-drv-match),)
MAKE_OPTS+=match=y
endif

ifneq ($(CONFIG_PACKAGE_kmod-qca-nss-drv-mirror),)
MAKE_OPTS+=mirror=y
endif

ifneq ($(CONFIG_PACKAGE_kmod-qca-nss-drv-mscs),)
EXTRA_CFLAGS+= \
	-I$(STAGING_DIR)/usr/include/qca-wifi \
	-I$(STAGING_DIR)/usr/include/qca-nss-ecm
MAKE_OPTS+=mscs=y
endif

ifneq ($(CONFIG_PACKAGE_kmod-qca-nss-drv-wifi-meshmgr),)
MAKE_OPTS+=wifi-meshmgr=y
endif

INSTALLKMODDIR:=$(INSTALLDIR)/lib/modules/$(LINUX_KERNEL)
PBBINS=$(wildcard prebuilt/*)
NSS_CLIENTS_KMOD=$(filter %.ko,$(PBBINS))

all:
ifneq ($(wildcard $(SDK_DIR_PLATFORM)/qca-nss-clients/*.c),)
	ln -sf $(SDK_DIR_PLATFORM)/qca-nss-clients source
	cp $(SDK_DIR_PLATFORM)/qca-nss-drv/Module.symvers $(PKG_BUILD_DIR)
	$(MAKE) -C "$(LINUXDIR)" $(strip $(MAKE_OPTS)) CROSS_COMPILE="$(TARGET_CROSS)" ARCH="$(ARCH)" M="$(PKG_BUILD_DIR)" \
		EXTRA_CFLAGS="$(EXTRA_CFLAGS)" SoC="$(subtarget)" DTLSMGR_DIR="$(DTLSMGR_DIR)" IPSECMGR_DIR="$(IPSECMGR_DIR)" modules
endif

install:
	@[ -d $(INSTALLKMODDIR) ] || install -d $(INSTALLKMODDIR)
ifneq ($(wildcard $(SDK_DIR_PLATFORM)/qca-nss-clients/*.c),)
ifneq ($(CONFIG_PACKAGE_kmod-qca-nss-drv-wifi-meshmgr),)
	@install -D $(PKG_BUILD_DIR)/wifi_meshmgr/qca-nss-wifi-meshmgr.ko $(INSTALLKMODDIR)
endif
ifneq ($(CONFIG_PACKAGE_kmod-qca-nss-drv-pvxlanmgr),)
	@install -D $(PKG_BUILD_DIR)/pvxlanmgr/qca-nss-pvxlanmgr.ko $(INSTALLKMODDIR)
endif
ifneq ($(CONFIG_PACKAGE_kmod-qca-nss-drv-profile),)
	@install -D $(PKG_BUILD_DIR)/profiler/qca-nss-profile-drv.ko $(INSTALLKMODDIR)
endif
ifneq ($(CONFIG_PACKAGE_kmod-qca-nss-drv-tlsmgr),)
	@install -D $(PKG_BUILD_DIR)/tls/qca-nss-tlsmgr.ko $(INSTALLKMODDIR)
endif
ifneq ($(CONFIG_PACKAGE_kmod-qca-nss-drv-ipsecmgr-klips),)
	@install -D $(PKG_BUILD_DIR)/ipsecmgr/$(IPSECMGR_DIR)/plugins/klips/qca-nss-ipsec-klips.ko $(INSTALLKMODDIR)
endif
ifneq ($(CONFIG_PACKAGE_kmod-qca-nss-drv-eogremgr),)
	@install -D $(PKG_BUILD_DIR)/eogremgr/qca-nss-eogremgr.ko $(INSTALLKMODDIR)
endif
ifneq ($(CONFIG_PACKAGE_kmod-qca-nss-drv-dtlsmgr),)
	@install -D $(PKG_BUILD_DIR)/dtls/$(DTLSMGR_DIR)/qca-nss-dtlsmgr.ko $(INSTALLKMODDIR)
endif
ifneq ($(CONFIG_PACKAGE_kmod-qca-nss-drv-clmapmgr),)
	@install -D $(PKG_BUILD_DIR)/clmapmgr/qca-nss-clmapmgr.ko $(INSTALLKMODDIR)
endif
ifneq ($(CONFIG_PACKAGE_kmod-qca-nss-drv-capwapmgr),)
	@install -D $(PKG_BUILD_DIR)/capwapmgr/qca-nss-capwapmgr.ko $(INSTALLKMODDIR)
endif
	@install -D $(PKG_BUILD_DIR)/mscs/qca-nss-mscs.ko $(INSTALLKMODDIR)
	@install -D $(PKG_BUILD_DIR)/mirror/qca-nss-mirror.ko $(INSTALLKMODDIR)
	@install -D $(PKG_BUILD_DIR)/match/qca-nss-match.ko $(INSTALLKMODDIR)
	@install -D $(PKG_BUILD_DIR)/vxlanmgr/qca-nss-vxlanmgr.ko $(INSTALLKMODDIR)
	@install -D $(PKG_BUILD_DIR)/openvpn/plugins/qca-nss-ovpn-link.ko $(INSTALLKMODDIR)
	@install -D $(PKG_BUILD_DIR)/openvpn/src/qca-nss-ovpn-mgr.ko $(INSTALLKMODDIR)
	@install -D $(PKG_BUILD_DIR)/netlink/qca-nss-netlink.ko $(INSTALLKMODDIR)
	@install -D $(PKG_BUILD_DIR)/lag/qca-nss-lag-mgr.ko $(INSTALLKMODDIR)
	@install -D $(PKG_BUILD_DIR)/nss_qdisc/igs/act_nssmirred.ko $(INSTALLKMODDIR)
	@install -D $(PKG_BUILD_DIR)/nss_qdisc/qca-nss-qdisc.ko $(INSTALLKMODDIR)
	@install -D $(PKG_BUILD_DIR)/vlan/qca-nss-vlan.ko $(INSTALLKMODDIR)
	@install -D $(PKG_BUILD_DIR)/bridge/qca-nss-bridge-mgr.ko $(INSTALLKMODDIR)
	@install -D $(PKG_BUILD_DIR)/ipsecmgr/$(IPSECMGR_DIR)/plugins/xfrm/qca-nss-ipsec-xfrm.ko $(INSTALLKMODDIR)
	@install -D $(PKG_BUILD_DIR)/ipsecmgr/$(IPSECMGR_DIR)/qca-nss-ipsecmgr.ko $(INSTALLKMODDIR)
	@install -D $(PKG_BUILD_DIR)/tunipip6/qca-nss-tunipip6.ko $(INSTALLKMODDIR)
	@install -D $(PKG_BUILD_DIR)/gre/qca-nss-gre.ko $(PKG_BUILD_DIR)/gre/test/qca-nss-gre-test.ko $(INSTALLKMODDIR)
	@install -D $(PKG_BUILD_DIR)/map/map-t/qca-nss-map-t.ko $(INSTALLKMODDIR)
	@install -D $(PKG_BUILD_DIR)/pppoe/qca-nss-pppoe.ko $(INSTALLKMODDIR)
	@install -D $(PKG_BUILD_DIR)/pptp/qca-nss-pptp.ko $(INSTALLKMODDIR)
	@install -D $(PKG_BUILD_DIR)/l2tp/l2tpv2/qca-nss-l2tpv2.ko $(INSTALLKMODDIR)
	@install -D $(PKG_BUILD_DIR)/qca-nss-tun6rd.ko $(INSTALLKMODDIR)
else
	@for i in $(NSS_CLIENTS_KMOD); do install -D $$i $(INSTALLKMODDIR) ; done
endif
	@$(STRIPX) $(INSTALLKMODDIR)/*.ko

clean:

stage:
ifneq ($(wildcard $(SDK_DIR_PLATFORM)/qca-nss-clients/*.c),)
	mkdir -p $(STAGING_DIR)/usr/include/qca-nss-clients
	cp -rf $(PKG_BUILD_DIR)/netlink/include/* $(STAGING_DIR)/usr/include/qca-nss-clients/
	cp -rf $(PKG_BUILD_DIR)/exports/* $(STAGING_DIR)/usr/include/qca-nss-clients/
endif
