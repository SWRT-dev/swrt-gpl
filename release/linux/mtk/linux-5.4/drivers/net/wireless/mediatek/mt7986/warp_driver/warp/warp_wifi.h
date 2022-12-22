/*
 ***************************************************************************
 * MediaTek Inc.
 *
 * All rights reserved. source code is an unpublished work and the
 * use of a copyright notice does not imply otherwise. This source code
 * contains confidential trade secret material of MediaTek. Any attemp
 * or participation in deciphering, decoding, reverse engineering or in any
 * way altering the source code is stricitly prohibited, unless the prior
 * written consent of MediaTek, Inc. is obtained.
 ***************************************************************************

	Module Name: wifi_offload
	warp_wifi.h
*/
#ifndef _WARP_WIFI_H_
#define _WARP_WIFI_H_
#include <net/ra_nat.h>

extern int (*ra_sw_nat_hook_tx)(struct sk_buff *skb, int gmac_no);

/*TX hook structure*/
struct wlan_tx_info {
	u8 *pkt;
	u32 bssidx;
	u32 wcid;
	u32 ringidx;
};

struct wlan_rx_info {
	u8 *pkt;
	u16 ppe_entry;
	u8 csrn;
};

struct ring_ctrl {
	u32 base;
	u32 cnt;
	u32 cidx;
	u32 didx;
};

struct wifi_hw {
	void *priv;
	void *hif_dev;
	u32 *p_int_mask;
	u32 chip_id;
	u32 tx_ring_num;
	u32 tx_token_nums;
	u32 sw_tx_token_nums;
	u32 hw_rx_token_num;
	unsigned long dma_offset;
	u32 int_sta;
	u32 int_mask;
	u32 tx_dma_glo_cfg;
	u32 ring_offset;
	u32 txd_size;
	u32 fbuf_size;
	u32 tx_pkt_size;
	u32 tx_ring_size;
	u32 rx_ring_size;
	u32 int_ser;
	u32 int_ser_value;
	struct ring_ctrl tx[2];
	struct ring_ctrl event;

	u32 rx_dma_glo_cfg;
	u32 rx_ring_num;
	u32 rx_data_ring_size;
	u32 rxd_size;
	u32 rx_pkt_size;
	u32 max_rxd_size;
	struct ring_ctrl rx[2];

	bool msi_enable;
	u32 hif_type;
	u32 irq;
	unsigned long wpdma_base;
	unsigned long base_phy_addr;
	unsigned long base_addr;

	u8 wfdma_tx_done_trig0_bit;
	u8 wfdma_tx_done_trig1_bit;
	u8 wfdma_tx_done_free_notify_trig_bit;
	u8 wfdma_rx_done_trig0_bit;
	u8 wfdma_rx_done_trig1_bit;
	bool dbdc_mode;
};

struct wifi_ops {
	void (*config_atc)(void *priv_data, bool enable);
	void (*swap_irq)(void *priv_data, u32 irq);
	void (*fbuf_init)(u8 *fbuf, u32 pkt_pa, u32 tkid);
	void (*txinfo_wrapper)(u8 *tx_info, struct wlan_tx_info *info);
	void (*txinfo_set_drop)(u8 *tx_info);
	bool (*hw_tx_allow)(u8 *tx_info);
	void (*tx_ring_info_dump)(void *priv_data, u8 ring_id, u32 idx);
	void (*warp_ver_notify)(void *priv_data, u8 ver, u8 warp_sub_ver, int warp_hw_caps);
#if 0
	int (*token_rx_dmad_init)(void *priv_data, void *pkt, unsigned long alloc_size,
				 void *alloc_va, dma_addr_t alloc_pa, u32 *token_id);
	u32 (*token_rx_dmad_deinit)(void *priv_data, u32 token_id);
#else
	u32 (*token_rx_dmad_init)(void *priv_data, void *pkt, unsigned long alloc_size,
				 void *alloc_va, dma_addr_t alloc_pa);
#endif
	int (*token_rx_dmad_lookup)(void *priv_data, u32 tkn_rx_id, void **pkt,
				    void **alloc_va, dma_addr_t *alloc_pa);
	void (*rxinfo_wrapper)(u8 *rx_info, struct wlan_rx_info *info);
	void (*do_wifi_reset)(void *priv_data);
	void (*update_wo_rxcnt)(void *priv_data, void *wo_rxcnt);
};

enum {
	BUS_TYPE_PCIE,
	BUS_TYPE_AXI,
	BUS_TYPE_MAX
};

struct wifi_entry {
	unsigned long base_addr;
	struct wifi_hw hw;
	struct wifi_ops *ops;
};

/*default SER status*/
#define WIFI_ERR_RECOV_NONE 			0x10
#define WIFI_ERR_RECOV_STOP_IDLE 		0
#define WIFI_ERR_RECOV_STOP_PDMA0 		1
#define WIFI_ERR_RECOV_RESET_PDMA0 		2
#define WIFI_ERR_RECOV_STOP_IDLE_DONE 		3

/*wifi related hal*/
void wifi_tx_tuple_add(struct wifi_entry *wifi, u8 idx, u8 *tx_info,
		       u32 wdma_rx_port);
void wifi_tx_tuple_reset(struct wifi_entry *wifi);
void wifi_tx_ring_info_dump(struct wifi_entry *wifi, u8 ring_id, u32 idx);
void wifi_chip_atc_set(struct wifi_entry *wifi, bool enable);
void wifi_chip_probe(struct wifi_entry *wifi, u32 irq, u8 warp_ver, u8 warp_sub_ver,
		     int warp_hw_caps);
void wifi_chip_remove(struct wifi_entry *wifi);
bool wifi_hw_tx_allow(struct wifi_entry *wifi, u8 *tx_info);
void wifi_rx_tuple_add(struct wifi_entry *wifi, u8 idx, u8 *rx_info);
void wifi_chip_reset(struct wifi_entry *wifi);
void wifi_chip_update_wo_rxcnt(struct wifi_entry *wifi, void *wo_rxcnt);


#define to_wifi_entry(_hw) container_of(_hw, struct wifi_entry, hw)

#endif /*_WARP_WIFI_H_*/
