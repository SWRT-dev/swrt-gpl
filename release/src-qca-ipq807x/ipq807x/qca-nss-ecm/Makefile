include ../qsdk.config
# Convert swrt build environment variable to qsdk
PKG_BUILD_DIR=$(shell pwd)/source
STAGING_DIR=$(STAGEDIR)
#TARGET_CROSS=$(shell basename $(CROSS_COMPILE))
TARGET_CROSS=$(CROSS_COMPILE)
subtarget:=$(CONFIG_TARGET_BOARD)

ifeq ($(CONFIG_QCA_NSS_ECM_EXAMPLES_PCC),y)
       FILES_EXAMPLES=$(PKG_BUILD_DIR)/examples/ecm_pcc_test.ko
endif

ifeq ($(CONFIG_QCA_NSS_ECM_EXAMPLES_MARK),y)
       FILES_EXAMPLES+=$(PKG_BUILD_DIR)/examples/ecm_mark_test.ko
endif

ifneq ($(CONFIG_PACKAGE_kmod-qca-ovsmgr),)
CONFIG_QCA_NSS_ECM_OVS=y
endif

EXTRA_CFLAGS+= \
	-I$(STAGING_DIR)/usr/include/qca-nss-drv \
	-I$(STAGING_DIR)/usr/include/qca-wifi \
	-I$(STAGING_DIR)/usr/include/qca-nss-sfe \
	-I$(STAGING_DIR)/usr/include/qca-nss-ppe \
	-I$(STAGING_DIR)/usr/include/nat46

ifeq ($(CONFIG_PACKAGE_kmod-qca-hyfi-bridge), y)
ECM_MAKE_OPTS:=ECM_CLASSIFIER_HYFI_ENABLE=y
EXTRA_CFLAGS+= -I$(STAGING_DIR)/usr/include/hyfibr
endif

ifneq ($(CONFIG_PACKAGE_kmod-qca-nss-ecm-wifi-plugin),)
CONFIG_QCA_NSS_ECM_WIFI_PLUGIN=y
ifneq ($(CONFIG_PACKAGE_kmod-emesh-sp),)
ECM_MAKE_OPTS+=ECM_CLASSIFIER_MSCS_SCS_ENABLE=y
EXTRA_CFLAGS+= -I$(STAGING_DIR)/usr/include/emesh-sp
endif
ECM_MAKE_OPTS+=ECM_CLASSIFIER_MSCS_ENABLE=y
ECM_MAKE_OPTS+=ECM_FRONT_END_FSE_ENABLE=y
endif

ifeq ($(CONFIG_PACKAGE_kmod-qca-mcs), y)
ECM_MAKE_OPTS+=ECM_MULTICAST_ENABLE=y
EXTRA_CFLAGS+= -I$(STAGING_DIR)/usr/include/qca-mcs
endif
ECM_MAKE_OPTS+=	ECM_INTERFACE_IPSEC_ENABLE=y \
		ECM_INTERFACE_PPTP_ENABLE=y \
		ECM_INTERFACE_L2TPV2_ENABLE=y \
		ECM_INTERFACE_GRE_TAP_ENABLE=y \
		ECM_INTERFACE_GRE_TUN_ENABLE=y \
		ECM_INTERFACE_SIT_ENABLE=y \
		ECM_INTERFACE_TUNIPIP6_ENABLE=y \
		ECM_INTERFACE_RAWIP_ENABLE=y

ECM_MAKE_OPTS+=ECM_INTERFACE_BOND_ENABLE=y

ifneq ($(CONFIG_KERNEL_IPQ_MEM_PROFILE),256)
ifneq ($(CONFIG_LOWMEM_FLASH),y)
ifneq ($(CONFIG_XFRM),)
ECM_MAKE_OPTS+=ECM_XFRM_ENABLE=y
endif
#ifeq ($(BUILD_VARIANT), nonss)
#ECM_MAKE_OPTS+=ECM_FRONT_END_SFE_ENABLE=y
#FILES_EXAMPLES+=$(PKG_BUILD_DIR)/examples/ecm_sfe_l2.ko
#endif
ECM_MAKE_OPTS+=ECM_NON_PORTED_SUPPORT_ENABLE=y
endif
endif

ifeq ($(CONFIG_KERNEL_IPQ_MEM_PROFILE),256)
ECM_MAKE_OPTS+=ECM_FRONT_END_CONN_LIMIT_ENABLE=y
else ifeq ($(CONFIG_LOWMEM_FLASH),y)
ECM_MAKE_OPTS+=ECM_FRONT_END_CONN_LIMIT_ENABLE=y
endif

ifneq ($(CONFIG_PACKAGE_kmod-nat46),)
ECM_MAKE_OPTS+=ECM_INTERFACE_MAP_T_ENABLE=y
endif

ifneq ($(CONFIG_PACKAGE_kmod-qca-nss-drv-ovpn-link),)
ECM_MAKE_OPTS+=ECM_INTERFACE_OVPN_ENABLE=y
endif

ifneq ($(CONFIG_PACKAGE_kmod-qca-nss-drv-vxlanmgr),)
ECM_MAKE_OPTS+=ECM_INTERFACE_VXLAN_ENABLE=y
endif

ifneq ($(CONFIG_TARGET_ipq95xx),)
ECM_MAKE_OPTS+=ECM_INTERFACE_VXLAN_ENABLE=y
ECM_MAKE_OPTS+=ECM_FRONT_END_SFE_ENABLE=y
ECM_MAKE_OPTS+=ECM_FRONT_END_PPE_ENABLE=y
FILES_EXAMPLES+=$(PKG_BUILD_DIR)/examples/ecm_sfe_l2.ko
endif

ifneq ($(CONFIG_TARGET_ipq53xx),)
ECM_MAKE_OPTS+=ECM_INTERFACE_VXLAN_ENABLE=y
ECM_MAKE_OPTS+=ECM_FRONT_END_SFE_ENABLE=y
ECM_MAKE_OPTS+=ECM_FRONT_END_PPE_ENABLE=y
FILES_EXAMPLES+=$(PKG_BUILD_DIR)/examples/ecm_sfe_l2.ko
endif

ifneq ($(CONFIG_PACKAGE_kmod-qca-ovsmgr),)
ECM_MAKE_OPTS+=ECM_INTERFACE_OVS_BRIDGE_ENABLE=y \
		ECM_CLASSIFIER_OVS_ENABLE=y
EXTRA_CFLAGS+= -I$(STAGING_DIR)/usr/include/qca-ovsmgr
endif

ifneq ($(CONFIG_PACKAGE_kmod-qca-nss-drv-igs),)
ECM_MAKE_OPTS+=ECM_CLASSIFIER_DSCP_IGS=y
endif

ifneq ($(CONFIG_PACKAGE_kmod-emesh-sp),)
ECM_MAKE_OPTS+=ECM_CLASSIFIER_EMESH_ENABLE=y
EXTRA_CFLAGS+= -I$(STAGING_DIR)/usr/include/emesh-sp
endif

ifneq ($(CONFIG_PACKAGE_kmod-qca-nss-drv-mscs),)
ifneq ($(CONFIG_PACKAGE_kmod-emesh-sp),)
ECM_MAKE_OPTS+=ECM_CLASSIFIER_MSCS_SCS_ENABLE=y
EXTRA_CFLAGS+= -I$(STAGING_DIR)/usr/include/emesh-sp
endif
ECM_MAKE_OPTS+=ECM_CLASSIFIER_MSCS_ENABLE=y
endif

ifneq ($(CONFIG_PACKAGE_kmod-macvlan),)
ECM_MAKE_OPTS+=ECM_INTERFACE_MACVLAN_ENABLE=y
endif

# Disable ECM IPv6 support when global IPv6 support is disabled.
#ifneq ($(CONFIG_IPV6),)
ECM_MAKE_OPTS+=ECM_IPV6_ENABLE=y
#endif

# Disable ECM Bridge VLAN Filtering support when global kernel config is disabled
ifneq ($(CONFIG_BRIDGE_VLAN_FILTERING),)
ECM_MAKE_OPTS+=ECM_BRIDGE_VLAN_FILTERING_ENABLE=y
endif

# Enable NSS frontend for all the platforms except ipq40xx, ipq95xx and ipq53xx
ifeq ($(CONFIG_TARGET_ipq_ipq40xx)$(CONFIG_TARGET_ipq40xx)$(CONFIG_TARGET_ipq95xx)$(CONFIG_TARGET_ipq53xx),)
#ifneq ($(BUILD_VARIANT), nonss)
ECM_MAKE_OPTS+=ECM_FRONT_END_NSS_ENABLE=y
#endif
endif

INSTALLKMODDIR:=$(INSTALLDIR)/lib/modules/$(LINUX_KERNEL)
PBBINS=$(wildcard prebuilt/*)
NSS_ECM_KMOD=$(filter %.ko,$(PBBINS))

KBUILD_EXTRA_SYMBOLS += $(SDK_DIR_PLATFORM)/qca-nss-drv/Module.symvers
KBUILD_EXTRA_SYMBOLS += $(SDK_DIR_PLATFORM)/qca-mcs/Module.symvers
KBUILD_EXTRA_SYMBOLS += $(SDK_DIR_PLATFORM)/emesh-sp/Module.symvers

all:
ifneq ($(wildcard $(SDK_DIR_PLATFORM)/qca-nss-ecm/*.c),)
	ln -sf $(SDK_DIR_PLATFORM)/qca-nss-ecm source
	$(MAKE) -C "$(LINUXDIR)" $(strip $(ECM_MAKE_OPTS)) CROSS_COMPILE="$(TARGET_CROSS)" ARCH="$(ARCH)" M="$(PKG_BUILD_DIR)" \
		EXTRA_CFLAGS="$(EXTRA_CFLAGS)" SoC="$(subtarget)" EXAMPLES_BUILD_PCC="$(CONFIG_QCA_NSS_ECM_EXAMPLES_PCC)" \
		EXAMPLES_BUILD_MARK="$(CONFIG_QCA_NSS_ECM_EXAMPLES_MARK)" EXAMPLES_BUILD_OVS="$(CONFIG_QCA_NSS_ECM_OVS)" \
		BUILD_ECM_WIFI_PLUGIN="$(CONFIG_QCA_NSS_ECM_WIFI_PLUGIN)" modules
endif
	touch stamp-h1

install:
	@[ -d $(INSTALLKMODDIR) ] || install -d $(INSTALLKMODDIR)
ifneq ($(wildcard $(SDK_DIR_PLATFORM)/qca-nss-ecm/*.c),)
ifeq ($(CONFIG_QCA_NSS_ECM_EXAMPLES_PCC),y)
	@install -D $(PKG_BUILD_DIR)/examples/ecm_pcc_test.ko $(INSTALLKMODDIR)
endif
ifeq ($(CONFIG_QCA_NSS_ECM_EXAMPLES_MARK),y)
	@install -D $(PKG_BUILD_DIR)/examples/ecm_mark_test.ko $(INSTALLKMODDIR)
endif
ifeq ($(CONFIG_QCA_NSS_ECM_OVS),y)
	@install -D $(PKG_BUILD_DIR)/examples/ecm_ovs.ko $(INSTALLKMODDIR)
endif
	@install -D $(PKG_BUILD_DIR)/examples/ecm_ae_select.ko $(INSTALLKMODDIR)
	@install -D $(PKG_BUILD_DIR)/ecm.ko $(INSTALLKMODDIR)
else
	@for i in $(NSS_ECM_KMOD); do install -D $$i $(INSTALLKMODDIR) ; done
endif
	@$(STRIPX) $(INSTALLKMODDIR)/*.ko


clean:

stage:
ifneq ($(wildcard $(SDK_DIR_PLATFORM)/qca-nss-ecm/*.c),)
	mkdir -p $(STAGING_DIR)/usr/include/qca-nss-ecm
	cp -rf $(PKG_BUILD_DIR)/exports/* $(STAGING_DIR)/usr/include/qca-nss-ecm/
endif
