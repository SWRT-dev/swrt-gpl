include ../qsdk.config
# Convert swrt build environment variable to qsdk
PKG_BUILD_DIR=$(shell pwd)/source
STAGING_DIR=$(STAGEDIR)
#TARGET_CROSS=$(shell basename $(CROSS_COMPILE))
TARGET_CROSS=$(CROSS_COMPILE)

EXTRA_CFLAGS+= -I$(STAGING_DIR)/usr/include/qca-nss-dp -I$(STAGING_DIR)/usr/include/qca-ssdk
ifeq (,$(CONFIG_TARGET_ipq95xx))
EXTRA_CFLAGS+= -I$(STAGING_DIR)/usr/include/qca-nss-gmac
endif
ifeq ($(CONFIG_KERNEL_IPQ_MEM_PROFILE),256)
EXTRA_CFLAGS+= -DNSS_MEM_PROFILE_LOW
LOW_MEM_PROFILE_MAKE_OPTS=y
DISABLE_DRV_MAKE_OPTS=y
else ifeq ($(CONFIG_LOWMEM_FLASH),y)
EXTRA_CFLAGS+= -DNSS_MEM_PROFILE_LOW
LOW_MEM_PROFILE_MAKE_OPTS=y
DISABLE_DRV_MAKE_OPTS=y
else ifeq ($(CONFIG_KERNEL_IPQ_MEM_PROFILE),512)
EXTRA_CFLAGS+= -DNSS_MEM_PROFILE_MEDIUM
endif

ifeq ($(CONFIG_KERNEL_SKB_FIXED_SIZE_2K),y)
EXTRA_CFLAGS+= -DNSS_SKB_FIXED_SIZE_2K
endif

ifeq (y,$(CONFIG_TARGET_ipq95xx))
POINT_OFFLOAD_MAKE_OPTS=y
DISABLE_DRV_MAKE_OPTS=y
endif

DRV_MAKE_OPTS:=
ifeq ($(DISABLE_DRV_MAKE_OPTS),y)
DRV_MAKE_OPTS+=NSS_DRV_C2C_ENABLE=n \
        NSS_DRV_CLMAP_ENABLE=n \
        NSS_DRV_GRE_ENABLE=n \
        NSS_DRV_GRE_REDIR_ENABLE=n \
        NSS_DRV_GRE_TUNNEL_ENABLE=n \
        NSS_DRV_IGS_ENABLE=n \
        NSS_DRV_IPSEC_ENABLE=n \
        NSS_DRV_LAG_ENABLE=n \
        NSS_DRV_L2TP_ENABLE=n \
        NSS_DRV_MAPT_ENABLE=n \
        NSS_DRV_OAM_ENABLE=n \
        NSS_DRV_PPTP_ENABLE=n \
        NSS_DRV_PORTID_ENABLE=n \
        NSS_DRV_PVXLAN_ENABLE=n \
        NSS_DRV_QRFS_ENABLE=n \
        NSS_DRV_QVPN_ENABLE=n \
        NSS_DRV_RMNET_ENABLE=n \
        NSS_DRV_SHAPER_ENABLE=n \
        NSS_DRV_SJACK_ENABLE=n \
        NSS_DRV_TLS_ENABLE=n \
        NSS_DRV_TSTAMP_ENABLE=n \
        NSS_DRV_TUN6RD_ENABLE=n \
        NSS_DRV_TUNIPIP6_ENABLE=n \
        NSS_DRV_VXLAN_ENABLE=n \
        NSS_DRV_MATCH_ENABLE=n \
        NSS_DRV_MIRROR_ENABLE=n \
        NSS_DRV_WIFI_EXT_VDEV_ENABLE=n \
        NSS_DRV_WIFI_MESH_ENABLE=n \
        NSS_DRV_IPV4_REASM_ENABLE=n \
        NSS_DRV_IPV6_REASM_ENABLE=n
# Disable PPE bridge support for IPQ50xx.
ifneq (, $(CONFIG_TARGET_ipq50xx))
        DRV_MAKE_OPTS+=NSS_DRV_BRIDGE_ENABLE=n
endif
endif

ifeq ($(LOW_MEM_PROFILE_MAKE_OPTS),y)
DRV_MAKE_OPTS+=NSS_DRV_CAPWAP_ENABLE=n \
	NSS_DRV_TRUSTSEC_ENABLE=n \
	NSS_DRV_TRUSTSEC_RX_ENABLE=n \
	NSS_DRV_CRYPTO_ENABLE=n \
	NSS_DRV_DTLS_ENABLE=n
endif

ifeq ($(POINT_OFFLOAD_MAKE_OPTS),y)
DRV_MAKE_OPTS+=NSS_DRV_POINT_OFFLOAD=y \
	NSS_DRV_BRIDGE_ENABLE=n \
	NSS_DRV_ETH_RX_ENABLE=n \
	NSS_DRV_IPV4_ENABLE=n \
	NSS_DRV_IPV6_ENABLE=n \
	NSS_DRV_LSO_RX_ENABLE=n \
	NSS_DRV_PPPOE_ENABLE=n \
	NSS_DRV_UDP_ST_ENABLE=n \
	NSS_DRV_VLAN_ENABLE=n \
	NSS_DRV_WIFIOFFLOAD_ENABLE=n
endif

# Disable DRV IPv6 support when global IPv6 support is disabled.
#DRV_MAKE_OPTS+=NSS_DRV_IPV6_ENABLE=n
subtarget:=$(CONFIG_TARGET_BOARD)

INSTALLKMODDIR:=$(INSTALLDIR)/lib/modules/$(LINUX_KERNEL)
KBUILD_EXTRA_SYMBOLS += $(SDK_DIR_PLATFORM)/qca-nss-dp/Module.symvers

all:
ifneq ($(wildcard $(SDK_DIR_PLATFORM)/qca-nss-drv/*.c),)
	ln -sf $(SDK_DIR_PLATFORM)/qca-nss-drv source
	ln -sf $(PKG_BUILD_DIR)/exports/arch/nss_$(CONFIG_TARGET_BOARD)_64.h $(PKG_BUILD_DIR)/exports/nss_arch.h
	$(MAKE) -C "$(LINUXDIR)" $(strip $(DRV_MAKE_OPTS)) CROSS_COMPILE="$(TARGET_CROSS)" ARCH="$(ARCH)" M="$(PKG_BUILD_DIR)" \
		EXTRA_CFLAGS="$(EXTRA_CFLAGS)" SoC=$(subtarget) modules
else
	mkdir -p $(PKG_BUILD_DIR)
	cp prebuilt/qca-nss-drv.ko $(PKG_BUILD_DIR)
endif
	touch stamp-h1

install:
	@[ -d $(INSTALLKMODDIR) ] || install -d $(INSTALLKMODDIR)
	@install -D $(PKG_BUILD_DIR)/qca-nss-drv.ko $(INSTALLKMODDIR)
	@$(STRIPX) $(INSTALLKMODDIR)/qca-nss-drv.ko

clean:

stage:
ifneq ($(wildcard $(SDK_DIR_PLATFORM)/qca-nss-drv/*.c),)
	mkdir -p $(STAGING_DIR)/usr/include/qca-nss-drv
	mkdir -p $(STAGING_DIR)/usr/include/qca-nss-clients
	cp -rf $(PKG_BUILD_DIR)/exports/* $(STAGING_DIR)/usr/include/qca-nss-drv/
	rm $(STAGING_DIR)/usr/include/qca-nss-drv/nss_ipsecmgr.h
	cp -rf $(SDK_DIR_PLATFORM)/qca-nss-clients/exports/nss_ipsecmgr.h $(STAGING_DIR)/usr/include/qca-nss-clients/.
endif
