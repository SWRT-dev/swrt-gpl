/*
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 *
 *  Copyright (C) 2009~2015 Lantiq Deutschland GmbH
 *  Copyright (C) 2016 Intel Corporation.
 */

#ifndef _LANTIQ_ETH_DRV_H_
#define _LANTIQ_ETH_DRV_H_

#ifdef CONFIG_SW_ROUTING_MODE
    #define CONFIG_PMAC_DMA_ENABLE          1   /*g_pmac_dma */
    #define CONFIG_DMA_PMAC_ENABLE          1   /*g_dma_pmac*/
#else
    #define CONFIG_PMAC_DMA_ENABLE          0   /*g_pmac_dma */
    #define CONFIG_DMA_PMAC_ENABLE          0   /*g_dma_pmac*/
#endif

#if defined(CONFIG_PMAC_DMA_ENABLE) && CONFIG_PMAC_DMA_ENABLE
    #define NUM_ETH_INF                     7
#else
    #define NUM_ETH_INF                     1
#endif

struct xrx500_port {
	u8 num;
	u8 phy_addr;
	u16 flags;
	phy_interface_t phy_if;
	int link;
	int irq_num;
	struct phy_device *phydev;
	struct device_node *phy_node;
};

struct xrx500_thermal {
	struct thermal_cooling_device *cdev;

	unsigned int cur_state;
	unsigned int max_state;
};

struct xrx500_hw {
	struct net_device *devs[NUM_ETH_INF];
	int num_devs;
	int port_map[NUM_ETH_INF];
	unsigned short wan_map;
	struct xrx500_thermal thermal;
};

/**
 * This structure is used internal purpose
 */
struct ltq_eth_priv {
#if IS_ENABLED(CONFIG_INTEL_E160)
	void *macsec_data;
	struct mac_e160_ops e160_ops;
#endif
	/*!< network device interface Statistics */
	struct rtnl_link_stats64 stats;
	/*!< structure of dma device information */
	struct dma_device_info *dma_device;
	struct sk_buff *skb; /*!< skb buffer structure*/
	spinlock_t lock; /*!< spin lock */
	int phy_addr; /*!< interface mdio phy address*/
	int current_speed; /*!< interface current speed*/
	int full_duplex; /*!< duplex mode*/
	int current_duplex; /*!< current interface duplex mode*/
	void __iomem                *base_addr; /*!< Base address */
	unsigned int                flags;  /*!< flags */
	struct module *owner;
	dp_subif_t dp_subif;
	s32 dev_port; /*dev  instance */
	s32 f_dp;   /* status for register to datapath*/
	u32 dp_port_id;
	int xgmac_id;
	struct device_node *xpcs_node;
	struct xrx500_port port[NUM_ETH_INF];
	int num_port;
	struct xrx500_hw				*hw;
	unsigned short port_map;
	int id;
	int wan;
	int jumbo_enabled;
	int lct_en;
	int extra_subif;
	u8 bridge_domain;
	u8 lan_id;
	/*! min netdevices for extra subif/lct */
	int start;
	/*! max netdevices for extra subif/lct */
	int end;
	struct ltq_eth_priv *master;
	atomic_t powerup;
	atomic_t xgmac_rx;
	struct ethtool_cmd bkup_cmd;
	bool needs_recovery;
	#define FLAG_PAUSE_AUTO         0x00000001
	#define FLAG_FULL_DUPLEX        0x00000002
	#define FLAG_10_BASE_T          0x00000010
	#define FLAG_100_BASE_T         0x00000020
	#define FLAG_1000_BASE_T        0x00000040
	#define FLAG_TX_PAUSE           0x00000100
	#define FLAG_RX_PAUSE           0x00000200
	#define FLAG_FORCE_LINK         0x00000400
	#define FLAG_ADV_10HALF         0x00001000
	#define FLAG_ADV_10FULL         0x00002000
	#define FLAG_ADV_100HALF        0x00004000
	#define FLAG_ADV_100FULL        0x08008000
	#define FLAG_ADV_1000HALF       0x00010000
	#define FLAG_ADV_1000FULL       0x00020000
	#define FLAG_INTERNAL_PHY       0x00100000
	u32 ethtool_flags;
	#define ETHTOOL_FLAG_BP_CPU_ENABLE	BIT(0)
	bool eee_enabled;
	bool eee_active;
	bool tx_lpi_enabled;
	u32 tx_lpi_timer;
	/* The default timer value as per the
	 * XGMAC spec 1 sec(1000 ms)
	 */
	#define DEFAULT_LPI_TIMER 1000
};

/**
 * This structure is used internal purpose
 */
struct ltq_pmac_header_t {
	u32 tcp_checksum	    :1;  /*!< Reserved bits*/
	u32 ver_done	    :1;  /*!< IP Offset */
	u32 ip_offset	    :6; /*!< Destination Group and Port Map */
	u32 tcp_h_offset	    :5; /*!< Source Logical Port ID */
	u32 tcp_type	    :3; /*!< Reserved bits*/
	u32 sppid		    :4; /*!< Is Tagged */
	u32 class_id	    :4; /*!< Reserved bits*/
	u32 port_map_en	    :1; /*!< Reserved bits*/
	u32 port_map_sel	    :1; /*!< PPPoE Session Packet */
	u32 lrn_dis		    :1; /*!< IPv6 Packet */
	u32 class_en	    :1; /*!< IPv4 Packet */
	u32 reserved	    :2; /*!< Mirrored */
	u32 pkt_type	    :2; /*!< Reserved bits*/
	u32 crcgen_dis	    :1; /* Packet Length High Bits */
	u32 redirect	    :1; /* Packet Length Low Bits */
	u32 timestamp	    :1; /*!< Reserved bits*/
	u32 sub_if_sc_hi	    :5; /*!< Reserved bits*/
	u32 sub_if_sc_lo	    :8; /*!< Source Physical Port ID */
	u32 port_map_hi	    :8; /*!< Traffic Class */
	u32 port_map_lo	    :8; /*!< Traffic Class */
} ltq_pmac_header_t;

extern int g_xway_gphy_fw_loaded;

struct ltq_net_soc_data {
	bool need_defer;
	bool hw_checksum;
	unsigned int queue_num;
	u32 mtu_limit;
	int (*phy_connect_func)(struct net_device *, struct xrx500_port *);
};
#endif /* _LANTIQ_ETH_DRV_H_ */
