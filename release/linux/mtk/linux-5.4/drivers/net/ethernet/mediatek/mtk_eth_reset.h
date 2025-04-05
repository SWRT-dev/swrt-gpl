/* SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (c) 2022 MediaTek Inc.
 * Author: Henry Yen <henry.yen@mediatek.com>
 */

#ifndef MTK_ETH_RESET_H
#define MTK_ETH_RESET_H

/* Frame Engine Reset FSM */
#define MTK_FE_START_RESET	0x2000
#define MTK_FE_RESET_DONE	0x2001
#define MTK_WIFI_RESET_DONE	0x2002
#define MTK_WIFI_CHIP_ONLINE 	0x2003
#define MTK_WIFI_CHIP_OFFLINE 	0x2004
#define MTK_TOPS_DUMP_DONE	0x3001
#define MTK_FE_RESET_NAT_DONE	0x4001

#define MTK_FE_STOP_TRAFFIC	(0x2005)
#define MTK_FE_STOP_TRAFFIC_DONE	(0x2006)
#define MTK_FE_START_TRAFFIC	(0x2007)
#define MTK_FE_STOP_TRAFFIC_DONE_FAIL	(0x2008)
#define MTK_FE_START_RESET_INIT	(0x2009)

/*FE GDM Counter */
#define MTK_GDM_RX_FC	(0x24)

/* ADMA Rx Debug Monitor */
#define MTK_ADMA_RX_DBG0	(PDMA_BASE + 0x238)
#define MTK_ADMA_RX_DBG1	(PDMA_BASE + 0x23C)

/* PPE Configurations */
#define MTK_PPE_GLO_CFG(x)	(PPE_BASE(x) + 0x00)
#define MTK_PPE_TB_CFG(x)	(PPE_BASE(x) + 0x1C)
#define MTK_PPE_BIND_LMT_1(x)	(PPE_BASE(x) + 0x30)
#define MTK_PPE_KA(x)		(PPE_BASE(x) + 0x34)
#define MTK_PPE_KA_CFG_MASK	(0x3 << 12)
#define MTK_PPE_NTU_KA_MASK	(0xFF << 16)
#define MTK_PPE_KA_T_MASK	(0xFFFF << 0)
#define MTK_PPE_TCP_KA_MASK	(0xFF << 16)
#define MTK_PPE_UDP_KA_MASK	(0xFF << 24)
#define MTK_PPE_TICK_SEL_MASK	(0x1 << 24)
#define MTK_PPE_SCAN_MODE_MASK	(0x3 << 16)
#define MTK_PPE_BUSY		BIT(31)

#if defined(CONFIG_MEDIATEK_NETSYS_V3)
#define MTK_GDM_RX_BASE	(0x8)
#define MTK_GDM_CNT_OFFSET	(0x80)
#define MTK_GDM_TX_BASE	(0x48)
#else
#define MTK_GDM_RX_BASE	(0x8)
#define MTK_GDM_CNT_OFFSET	(0x40)
#define MTK_GDM_TX_BASE	(0x38)
#endif

enum mtk_reset_type {
	MTK_TYPE_COLD_RESET	= 0,
	MTK_TYPE_WARM_RESET,
};

enum mtk_reset_event_id {
	MTK_EVENT_FORCE		= 0,
	MTK_EVENT_WARM_CNT	= 1,
	MTK_EVENT_COLD_CNT	= 2,
	MTK_EVENT_TOTAL_CNT	= 3,
	MTK_EVENT_FQ_EMPTY	= 8,
	MTK_EVENT_TSO_FAIL	= 12,
	MTK_EVENT_TSO_ILLEGAL	= 13,
	MTK_EVENT_TSO_ALIGN	= 14,
	MTK_EVENT_RFIFO_OV	= 18,
	MTK_EVENT_RFIFO_UF	= 19,
};

int mtk_eth_netdevice_event(struct notifier_block *n, unsigned long event, void *ptr);
extern struct completion wait_ser_done;
extern struct completion wait_tops_done;
extern char* mtk_reset_event_name[32];
extern atomic_t reset_lock;
extern struct completion wait_nat_done;
extern u32 mtk_reset_flag;
extern bool mtk_stop_fail;

irqreturn_t mtk_handle_fe_irq(int irq, void *_eth);
u32 mtk_check_reset_event(struct mtk_eth *eth, u32 status);
int mtk_eth_cold_reset(struct mtk_eth *eth);
int mtk_eth_warm_reset(struct mtk_eth *eth);
void mtk_reset_event_update(struct mtk_eth *eth, u32 id);
void mtk_dump_netsys_info(void *_eth);
void mtk_hw_reset_monitor(struct mtk_eth *eth);
void mtk_save_qdma_cfg(struct mtk_eth *eth);
void mtk_restore_qdma_cfg(struct mtk_eth *eth);
void mtk_prepare_reset_fe(struct mtk_eth *eth);
void mtk_prepare_reset_ppe(struct mtk_eth *eth, u32 ppe_id);

void mtk_pse_set_port_link(struct mtk_eth *eth, u32 port, bool enable);
#endif		/* MTK_ETH_RESET_H */
