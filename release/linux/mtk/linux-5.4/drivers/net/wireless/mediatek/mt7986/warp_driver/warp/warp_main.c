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

	Module Name: warp
	warp.c
*/

/*Main module of WHNAT, use to for register hook function and probe for wifi chip*/

#include <linux/init.h>
#include <linux/module.h>

#include "warp.h"
#include "warp_hw.h"
#include "wed.h"
#include "warp_bm.h"

#include "warp_meminfo_list.h"
#include "warp_fwdl.h"


/*wo interface*/
#include "mcu/warp_woif.h"
#include "mcu/warp_wo.h"


#define WED0_DEV_NODE "mediatek,wed"
#define WED1_DEV_NODE "mediatek,wed2"

/*
* extern functions from other files
*/
extern int warp_entry_proc_init(struct warp_ctrl *warp_ctrl,
				struct warp_entry *warp);
extern void warp_entry_proc_exit(struct warp_ctrl *warp_ctrl,
				 struct warp_entry *warp);
extern int warp_ctrl_proc_init(struct warp_ctrl *warp_ctrl);
extern void warp_ctrl_proc_exit(struct warp_ctrl *warp_ctrl);
extern int wed_entry_proc_init(struct warp_entry *warp, struct wed_entry *wed);
extern void wed_entry_proc_exit(struct warp_entry *warp, struct wed_entry *wed);
extern int wdma_entry_proc_init(struct warp_entry *warp,
				struct wdma_entry *wdma);
extern void wdma_entry_proc_exit(struct warp_entry *warp,
				 struct wdma_entry *wdma);

extern int rxbm_proc_init(struct warp_entry *warp, struct wed_entry *wed);
extern void rxbm_proc_exit(struct warp_entry *warp, struct wed_entry *wed);
extern void wo_proc_exit(struct warp_entry *warp);
extern void wo_exep_proc_exit(struct warp_entry *warp);


/*
 *   Definition
*/
#define DRIVER_DESC "Register for MTK WIFI-to-Eth Offload Engine"
#define WHNAT_PLATFORM_DEV_NAME "warp_dev"
/* protection for attacking if hacker modify the DTS. */
#define WED_MAX_NUM 8

static struct warp_ctrl sys_wc;

/*
*
*/
static const struct of_device_id warp_of_ids[] = {
	{	.compatible = "mediatek,wed", },
	{	.compatible = "mediatek,wed2", },
	{	.compatible = "mediatek,wed3", },
	{ },
};

/*warp support list write here*/
static const u32 warp_support_list[] = {
	0x7615,
	0x7915,
	0x7986,
	0x7981,
	0x7906,
	0x7902,	//Bellwether
	0
};

/*local function*/
/*
*
*/
static u8
warp_cap_support(u32 chip_id)
{
	u8 i = 0;

	while (warp_support_list[i] != 0) {
		if (chip_id == warp_support_list[i]) {
			warp_dbg(WARP_DBG_OFF, "%s(): chip_id=%x is in WHNAT support list\n", __func__,
				 chip_id);
			return 1;
		}

		i++;
	}

	return 0;
}

static inline struct warp_ctrl *
warp_ctrl_get(void)
{
	return &sys_wc;
}

static inline struct warp_bus *
warp_bus_get(void)
{
	return &sys_wc.bus;
}

/*
*
*/
u8
warp_get_ref(void)
{
	struct warp_ctrl *warp_ctrl = warp_ctrl_get();
	return warp_ctrl->warp_ref;
}

/*
*
*/
static void
warp_increase_ref(void)
{
	struct warp_ctrl *warp_ctrl = warp_ctrl_get();

	spin_lock(&warp_ctrl->warp_lock);
	warp_ctrl->warp_ref++;
	spin_unlock(&warp_ctrl->warp_lock);
}

/*
*
*/
static void
warp_decrease_ref(void)
{
	struct warp_ctrl *warp_ctrl = warp_ctrl_get();

	spin_lock(&warp_ctrl->warp_lock);
	warp_ctrl->warp_ref--;
	spin_unlock(&warp_ctrl->warp_lock);
}

/*
*
*/
static struct wifi_hw *
warp_entry_acquire(u32 slot_id)
{
	int i;
	struct warp_ctrl *warp_ctrl = warp_ctrl_get();
	struct warp_entry *entry = NULL;

	/*check is acquired or not*/
	for (i = 0; i < warp_ctrl->warp_num; i++) {
		entry = &warp_ctrl->entry[i];
		if (entry->slot_id == slot_id) {
			warp_dbg(WARP_DBG_OFF,
				 "%s(): bus slot:%d, hook to WARP, entry id=%d, entry: %p\n", __func__, i,
				 entry->idx, entry);
			return &entry->wifi.hw;
		}
	}
	return NULL;
}

/*
*
*/
static void
warp_entry_release(struct warp_entry *warp)
{
	struct wifi_entry *wifi;
	wifi = &warp->wifi;
	/* wbsys irq return*/
	wifi->ops->swap_irq(wifi->hw.priv, warp->wifi.hw.irq);

	warp->wifi.hw.priv = NULL;
	memset(&warp->pdriver, 0, sizeof(warp->pdriver));
}

/*
*
*/
static struct warp_entry *
warp_entry_get_by_pdev(void *pdev)
{
	u8 i;
	struct warp_ctrl *warp_ctrl = warp_ctrl_get();
	struct warp_entry *entry;

	for (i = 0; i < warp_ctrl->warp_num; i++) {
		entry = &warp_ctrl->entry[i];

		if (entry->pdev == NULL && entry->pdriver.probe != NULL) {
			entry->pdev = pdev;
			warp_dbg(WARP_DBG_OFF, "%s(): return entry[%d] \n", __func__, i);
			return &warp_ctrl->entry[i];
		}
	}

	return NULL;
}

/*
*
*/
static struct warp_entry *
warp_entry_get_by_privdata(void *priv_data)
{
	unsigned char i;
	struct warp_ctrl *warp_ctrl = warp_ctrl_get();

	for (i = 0; i < warp_ctrl->warp_num; i++) {

		if (warp_ctrl->entry[i].wifi.hw.priv &&
		    warp_ctrl->entry[i].wifi.hw.priv == priv_data)
			return &warp_ctrl->entry[i];
	}

	return NULL;
}

/*
*
*/
static struct warp_entry *
warp_entry_get_for_pdev(void *pdev)
{
	u8 i;
	struct warp_ctrl *wc = warp_ctrl_get();
	struct warp_entry *entry;

	for (i = 0; i < wc->warp_num; i++) {
		entry = &wc->entry[i];

		if (entry->pdev == pdev)
			return &wc->entry[i];
	}

	return NULL;
}


/*
*
*/
struct warp_entry *
warp_entry_get_by_idx(u8 wed_idx)
{
	u8 i;
	struct warp_ctrl *wc = warp_ctrl_get();
	struct warp_entry *entry;

	for (i = 0; i < wc->warp_num; i++) {
		entry = &wc->entry[i];

		if (entry->idx == wed_idx)
			return &wc->entry[i];
	}

	return NULL;
}


/*
*
*/
#define  warp_entry_release_pdev(warp)\
	((struct warp_entry *) warp)->pdev = NULL

#ifdef WED_RX_D_SUPPORT
#ifdef CONFIG_WED_HW_RRO_SUPPORT
/*
*
*/
static int
warp_set_rrocfg(struct wed_entry *wed, u8 wed_idx)
{
	struct wed_rro_ctrl *rro_ctrl = &wed->res_ctrl.rx_ctrl.rro_ctrl;
	struct wo_cmd_wedcfg_para wedcfg_para = {0};

	struct warp_msg_cmd cmd = {0};

	warp_dbg(WARP_DBG_OFF, "set_rrocfg,%d\n", wed_idx);


	wedcfg_para.ring[WO_MIOD_RING].baseaddr = (unsigned long)
			rro_ctrl->miod_desc_base_mcu_view;

	wedcfg_para.ring[WO_MIOD_RING].maxcnt = rro_ctrl->miod_cnt;
	wedcfg_para.ring[WO_MIOD_RING].unit_sz = rro_ctrl->miod_entry_size;

	wedcfg_para.ring[WO_FBCMD_RING].baseaddr =
			(wedcfg_para.ring[WO_MIOD_RING].baseaddr +
			rro_ctrl->miod_cnt * rro_ctrl->miod_entry_size);

	wedcfg_para.ring[WO_FBCMD_RING].maxcnt = rro_ctrl->fdbk_cnt;
	wedcfg_para.ring[WO_FBCMD_RING].unit_sz = 4;
	wedcfg_para.wed_baseaddr = wed->base_addr;

	cmd.param.cmd_id = WO_CMD_WED_CFG;
	cmd.param.to_id = MODULE_ID_WO;
	cmd.param.wait_type = WARP_MSG_WAIT_TYPE_RSP_STATUS;
	cmd.param.timeout = 3000;
	cmd.msg = (u8 *)&wedcfg_para;
	cmd.msg_len = sizeof(struct wo_cmd_wedcfg_para);

	return warp_msg_send_cmd(wed_idx, &cmd);

}

#endif
#endif
/*
*
*/
static int
warp_probe(struct platform_device *pdev)
{
	int32_t ret = -1;
	int rc = 0;
	int i = 0;
	u32 heart_beat = 0;
	struct warp_entry *warp;
	struct wed_entry *wed;
	struct wdma_entry *wdma;
	struct wifi_entry *wifi;
	struct warp_bus *bus = warp_bus_get();
	struct warp_ctrl *warp_ctrl = warp_ctrl_get();

	warp = warp_entry_get_by_pdev(pdev);

	if (warp == NULL) {
		warp_dbg(WARP_DBG_ERR, "%s(): probe fail since warp is full!\n", __func__);
		return -1;
	}
	/*dma set to 32bit*/
	rc = dma_set_mask_and_coherent(&pdev->dev, DMA_BIT_MASK(32));

	if (rc != 0) {
		warp_dbg(WARP_DBG_ERR, "%s(): dma_set_mask_and_coherent() = %d is not 0\n", __func__, rc);
		return -1;
	}
	memset(&warp->wdma.res_ctrl, 0, sizeof(warp->wdma.res_ctrl));
	/* register warp msg */
	warp_msg_init(warp->idx);

	/*pre-allocate wifi information*/
	wifi = &warp->wifi;
	/*build sw base cr mirror mapping */
	warp_mtable_build_hw(warp);
	/*initial wed entry */
	wed = &warp->wed;
	wed->warp = warp;
	wed->sw_conf = &warp_ctrl->sw_conf[warp->idx];
	wed_init(pdev, warp->idx, wed);
	wed_entry_proc_init(warp, wed);
#ifdef WED_RX_D_SUPPORT
	rxbm_proc_init(warp, wed);
#endif
	/*initial wdma entry*/
	wdma = &warp->wdma;
	wdma->warp = warp;
	wdma->sw_conf = &warp_ctrl->sw_conf[warp->idx];
	wdma_init(pdev, warp->idx, wdma, wed->ver);
	wdma_entry_proc_init(warp, wdma);
#ifdef WED_INTER_AGENT_SUPPORT
	/*wifi chip related setting*/
	wifi_chip_probe(wifi, wed->irq, wed->ver, wed->sub_ver, wed->hw_cap);
	/*after wifi probe should write wpdma mask to wed*/
	warp_pdma_mask_set_hw(wed, *wifi->hw.p_int_mask);
	/*bus hw configuration*/
	warp_bus_set_hw(wed, bus, warp->idx, wifi->hw.msi_enable, wifi->hw.hif_type);
	/*wifi hw configuration*/
	warp_wifi_set_hw(wed, wifi);
#endif
	/*wo interface initial*/
#ifdef WED_RX_D_SUPPORT
	ret = woif_init(warp, bus);
#endif

	if (ret < 0) {
		warp_dbg(WARP_DBG_INF, "%s(): WO FM DL failed!\n", __func__);
		warp_fwdl_get_wo_heartbeat(&warp->woif.fwdl_ctrl, &heart_beat, warp->idx);
		warp_dbg(WARP_DBG_ERR, "%s()wo heartbeat: 0x%x!\n", __func__, heart_beat);
		/* dump WO PC LR log for 2 times*/
		warp_dbg(WARP_DBG_ERR, "%s()======WO PC LR dump======\n", __func__, heart_beat);
		for (i = 0; i < 2; i++) {
			warp_wo_pc_lr_cr_dump(warp->idx);
		}
		warp_dbg(WARP_DBG_ERR, "%s()======WO PC LR dump======\n", __func__, heart_beat);
	}
	warp_dbg(WARP_DBG_INF, "%s(): platform device probe is done\n", __func__);
	return 0;
}


/*
*
*/
static int
warp_remove(struct platform_device *pdev)
{
	struct warp_entry *warp;
	struct wed_entry *wed;
	struct wdma_entry *wdma;
	struct wifi_entry *wifi;

	warp = warp_entry_get_for_pdev(pdev);

	if (warp == NULL) {
		warp_dbg(WARP_DBG_ERR, "%s(): remove faild since can't find warp entry!\n",
			 __func__);
		return -1;
	}

	wed = &warp->wed;
	wdma = &warp->wdma;
	wifi = &warp->wifi;
	/*remove wifi related setting*/
	wifi_chip_remove(wifi);
	/*remove wdma related setting*/
	wdma_entry_proc_exit(warp, wdma);
	wdma_exit(pdev, wdma);
	/*remove wed related setting*/
#ifdef WED_RX_D_SUPPORT
	rxbm_proc_exit(warp, wed);
#endif
#ifdef CONFIG_WED_HW_RRO_SUPPORT
	wo_proc_exit(warp);
	wo_exep_proc_exit(warp);
#endif
	wed_entry_proc_exit(warp, wed);
	wed_exit(pdev, wed);
#ifdef WED_RX_D_SUPPORT
	woif_exit(&warp->woif);
#endif

	/*reomve warp entry from warp_ctrl*/
	warp_entry_release_pdev(warp);
	/* unregister warp msg */
	warp_msg_deinit(warp->idx);

	return 0;
}

/*
*
*/
static int
warp_entry_init(struct warp_entry *warp, u32 idx)
{
	struct warp_cfg *cfg = &warp->cfg;

	warp->idx = idx;
	warp->pdev = NULL;
	warp->slot_id = wed_get_slot_map(idx);
	memset(&warp->wifi, 0, sizeof(struct wifi_entry));
	memset(cfg, 0, sizeof(*cfg));
	cfg->hw_tx_en = CFG_HW_TX_SUPPORT;
	cfg->atc_en = CFG_ATC_SUPPORT;
	return 0;
}

/*
*
*/
static void
warp_entry_exit(struct warp_entry *warp)
{
	if (warp->wifi.hw.priv != NULL)
		warp_client_remove(warp->wifi.hw.priv);
}

/*
 *
 */
static int
warp_get_swconf_from_dts(struct sw_conf_t *sw_conf, u8 wed_idx)
{
	struct device_node *node = NULL;
	u32 num = 0;
	const char *enable = NULL;

	if (wed_idx == 0)
		node = of_find_compatible_node(NULL, NULL, WED0_DEV_NODE);
	else
		node = of_find_compatible_node(NULL, NULL, WED1_DEV_NODE);

	if (!node) {
		warp_dbg(WARP_DBG_INF,
				 "%s: can't found node of %s from dts, use default!\n",
				 __func__, (wed_idx) ? WED1_DEV_NODE : WED0_DEV_NODE);
		return -1;
	}

	if (of_property_read_string(node, "dy_txbm_enable", &enable)) {
		warp_dbg(WARP_DBG_INF, "%s(): get dy_txbm_enable from DTS fail! remain:%s\n",
				 __func__, (sw_conf->txbm.enable) ? "true" : "false");
	} else {
		warp_dbg(WARP_DBG_INF, "%s(): get dy_txbm_enable = %s!!\n", __func__, enable);

		sw_conf->txbm.enable = (enable = "true") ? true : false;
	}

	if (of_property_read_u32_index(node, "dy_txbm_alt_quota", 0, &num)) {
		warp_dbg(WARP_DBG_INF, "%s(): get dy_txbm_alt_quota from DTS fail! remain:%d\n",
				 __func__, sw_conf->txbm.alt_quota);
	} else {
		warp_dbg(WARP_DBG_INF, "%s(): get dy_txbm_alt_quota = %d!!\n", __func__, num);

		sw_conf->txbm.alt_quota = num;
	}

	if (of_property_read_u32_index(node, "dy_txbm_low_thrd", 0, &num)) {
		warp_dbg(WARP_DBG_INF, "%s(): get dy_txbm_low_thrd from DTS fail!!\n", __func__);
	} else {
		warp_dbg(WARP_DBG_INF, "%s(): get dy_txbm_low_thrd = %d!!\n", __func__, num);

		sw_conf->txbm.buf_low = num;
	}

	if (of_property_read_u32_index(node, "dy_txbm_high_thrd", 0, &num)) {
		warp_dbg(WARP_DBG_INF, "%s(): get dy_txbm_high_thrd from DTS fail!!\n", __func__);
	} else {
		warp_dbg(WARP_DBG_INF, "%s(): get dy_txbm_high_thrd = %d!!\n", __func__, num);

		sw_conf->txbm.buf_high = num;
	}

	if (of_property_read_u32_index(node, "dy_txbm_budget", 0, &num)) {
		warp_dbg(WARP_DBG_INF, "%s(): get dy_txbm_budget from DTS fail!!\n", __func__);
	} else {
		warp_dbg(WARP_DBG_INF, "%s(): get dy_txbm_budget from = %d\n", __func__, num);

		sw_conf->txbm.budget_limit = num;
	}

	if (of_property_read_u32_index(node, "dy_txbm_budge_refill_wm", 0, &num)) {
		warp_dbg(WARP_DBG_INF, "%s(): get dy_txbm_budge_refill_wm from DTS fail! remain:%d\n",
				 __func__, sw_conf->txbm.budget_refill_watermark);
	} else {
		warp_dbg(WARP_DBG_INF, "%s(): get dy_txbm_budge_refill_wm = %d\n", __func__, num);

		sw_conf->txbm.budget_refill_watermark = num;
	}

	if (of_property_read_u32_index(node, "wdma_rx_ring_depth", 0, &num)) {
		warp_dbg(WARP_DBG_INF, "%s(): get wdma_rx_ring_depth from DTS fail! remain:%d\n",
				 __func__, sw_conf->rx_wdma_ring_depth);
	} else {
		warp_dbg(WARP_DBG_INF, "%s(): get wdma_rx_ring_depth = %d!!\n", __func__, num);

		sw_conf->rx_wdma_ring_depth = num;
	}

	if (of_property_read_u32_index(node, "txbm_init_sz", 0, &num)) {
		warp_dbg(WARP_DBG_INF, "%s(): get txbm_init_sz from DTS fail! remain:%d\n",
				 __func__, sw_conf->txbm.vld_group);
	} else {
		warp_dbg(WARP_DBG_INF, "%s(): get txbm_init_sz = %d!!\n", __func__, num);

		sw_conf->txbm.vld_group = num;
	}

	if (of_property_read_u32_index(node, "txbm_max_sz", 0, &num)) {
		warp_dbg(WARP_DBG_INF, "%s(): get txbm_max_sz from DTS fail! remain:%d\n",
				 __func__, sw_conf->txbm.max_group);
	} else {
		warp_dbg(WARP_DBG_INF, "%s(): get txbm_max_sz = %d!!\n", __func__, num);

		sw_conf->txbm.max_group = num;
	}

	if (of_property_read_string(node, "dy_rxbm_enable", &enable)) {
		warp_dbg(WARP_DBG_INF, "%s(): get dy_rxbm_enable from DTS fail! remain:%s\n",
				 __func__, (sw_conf->rxbm.enable) ? "true" : "false");
	} else {
		warp_dbg(WARP_DBG_INF, "%s(): get dy_rxbm_enable = %s!!\n", __func__, enable);

		sw_conf->rxbm.enable = (enable = "true") ? true : false;
	}

	if (of_property_read_u32_index(node, "dy_rxbm_alt_quota", 0, &num)) {
		warp_dbg(WARP_DBG_INF, "%s(): get dy_rxbm_alt_quota from DTS fail! remain:%d\n",
				 __func__, sw_conf->rxbm.alt_quota);
	} else {
		warp_dbg(WARP_DBG_INF, "%s(): get dy_rxbm_alt_quota = %d\n", __func__, num);

		sw_conf->rxbm.alt_quota = num;
	}

	if (of_property_read_u32_index(node, "dy_rxbm_low_thrd", 0, &num)) {
		warp_dbg(WARP_DBG_INF, "%s(): get dy_rxbm_low_thrd from DTS fail!!\n", __func__);
	} else {
		warp_dbg(WARP_DBG_INF, "%s(): get dy_rxbm_low_thrd = %d\n", __func__, num);

		sw_conf->rxbm.buf_low = num;
	}

	if (of_property_read_u32_index(node, "dy_rxbm_high_thrd", 0, &num)) {
		warp_dbg(WARP_DBG_INF, "%s(): get dy_rxbm_high_thrd from DTS fail!!\n", __func__);
	} else {
		warp_dbg(WARP_DBG_INF, "%s(): get dy_rxbm_high_thrd = %d\n", __func__, num);

		sw_conf->rxbm.buf_high = num;
	}

	if (of_property_read_u32_index(node, "dy_rxbm_budget", 0, &num)) {
		warp_dbg(WARP_DBG_INF, "%s(): get dy_rxbm_budget from DTS fail!!\n", __func__);
	} else {
		warp_dbg(WARP_DBG_INF, "%s(): get dy_rxbm_budget = %d\n", __func__, num);

		sw_conf->rxbm.budget_limit = num;
	}

	if (of_property_read_u32_index(node, "dy_rxbm_budge_refill_wm", 0, &num)) {
		warp_dbg(WARP_DBG_INF, "%s(): get dy_rxbm_budge_refill_wm from DTS fail! remain:%d\n",
				 __func__, sw_conf->rxbm.budget_refill_watermark);
	} else {
		warp_dbg(WARP_DBG_INF, "%s(): get dy_rxbm_budge_refill_wm = %d\n", __func__, num);

		sw_conf->rxbm.budget_refill_watermark = num;
	}

	if (of_property_read_u32_index(node, "wdma_tx_ring_depth", 0, &num)) {
		warp_dbg(WARP_DBG_INF, "%s(): get wdma_tx_ring_depth from DTS fail! remain:%d\n",
				 __func__, sw_conf->tx_wdma_ring_depth);
	} else {
		warp_dbg(WARP_DBG_INF, "%s(): get wdma_tx_ring_depth = %d\n", __func__, num);

		sw_conf->tx_wdma_ring_depth = num;
	}

	if (of_property_read_u32_index(node, "rxbm_init_sz", 0, &num)) {
		warp_dbg(WARP_DBG_INF, "%s(): get rxbm_init_sz from DTS fail! remain:%d\n",
				 __func__, sw_conf->rxbm.vld_group);
	} else {
		warp_dbg(WARP_DBG_INF, "%s(): get rxbm_init_sz = %d\n", __func__, num);

		sw_conf->rxbm.vld_group = num;
	}

	if (of_property_read_u32_index(node, "rxbm_max_sz", 0, &num)) {
		warp_dbg(WARP_DBG_INF, "%s(): get rxbm_max_sz from DTS fail! remain:%d\n",
				 __func__, sw_conf->rxbm.max_group);
	} else {
		warp_dbg(WARP_DBG_INF, "%s(): get rxbm_max_sz = %d\n", __func__, num);

		sw_conf->rxbm.max_group = num;
	}

	return 0;
}

/*
*
*/
static int
warp_ctrl_init(struct warp_ctrl *warp_ctrl)
{
	u32 i;
	u32 size;

	warp_ctrl->warp_num = wed_get_entry_num();
	warp_ctrl->bus.warp_num = warp_ctrl->warp_num;

	if (warp_ctrl->warp_num > WED_MAX_NUM) {
		warp_dbg(WARP_DBG_ERR, "%s(): Error: wed_get_entry_num is more than %d\n", __func__, WED_MAX_NUM);
		return -1;
	}

	warp_ctrl->warp_driver_idx = 0;
	size = sizeof(struct warp_entry) * warp_ctrl->warp_num;
	warp_os_alloc_mem((unsigned char **)&warp_ctrl->entry, size, GFP_ATOMIC);

	if (!warp_ctrl->entry) {
		warp_dbg(WARP_DBG_ERR, "%s(): allocate warp control entry faild!\n", __func__);
		return -1;
	}

	memset(warp_ctrl->entry, 0, size);
	spin_lock(&warp_ctrl->warp_lock);
	warp_ctrl->warp_ref = 0;
	spin_unlock(&warp_ctrl->warp_lock);

	size = sizeof(struct sw_conf_t) * warp_ctrl->warp_num;
	warp_os_alloc_mem((unsigned char **)&warp_ctrl->sw_conf, size, GFP_ATOMIC);
	memset(warp_ctrl->sw_conf, 0, size);

	for (i = 0; i < warp_ctrl->warp_num; i++) {
		warp_entry_init(&warp_ctrl->entry[i], i);

		//warp_ctrl->sw_conf[i].txbm.enable = true;
		warp_ctrl->sw_conf[i].rx_wdma_ring_depth = WDMA_RX_BM_RING_SIZE;//WDMA_TX_BM_RING_SIZE;
		warp_ctrl->sw_conf[i].txbm.alt_quota = WED_DYNBM_LOW_GRP;
		warp_ctrl->sw_conf[i].txbm.buf_low = WED_DYNBM_LOW_GRP;
		warp_ctrl->sw_conf[i].txbm.buf_high = WED_DYNBM_HIGH_GRP;
		warp_ctrl->sw_conf[i].txbm.budget_limit = WED_DYNBM_LOW_GRP*4;
		warp_ctrl->sw_conf[i].txbm.budget_refill_watermark = WED_DYNBM_LOW_GRP*2;

		//warp_ctrl->sw_conf[i].rxbm.enable = true;
		//warp_ctrl->sw_conf[i].rxbm.vld_group = (12288/2)/128;
		warp_ctrl->sw_conf[i].tx_wdma_ring_depth = WDMA_RX_BM_RING_SIZE;//WDMA_TX_BM_RING_SIZE;
		warp_ctrl->sw_conf[i].rxbm.alt_quota = WED_DYNBM_LOW_GRP;
		//warp_ctrl->sw_conf[i].rxbm.buf_low = warp_ctrl->sw_conf[i].rxbm.vld_group - WED_DYNBM_LOW_GRP;
		//warp_ctrl->sw_conf[i].rxbm.buf_high = warp_ctrl->sw_conf[i].rxbm.vld_group - WED_DYNBM_HIGH_GRP;
		warp_ctrl->sw_conf[i].rxbm.budget_limit = WED_DYNBM_LOW_GRP*4;
		warp_ctrl->sw_conf[i].rxbm.budget_refill_watermark = WED_DYNBM_LOW_GRP*2;

		warp_get_swconf_from_dts(&warp_ctrl->sw_conf[i], i);
	}

	return 0;
}

/*
*
*/
static void
warp_ctrl_exit(struct warp_ctrl *warp_ctrl)
{
	u32 num = wed_get_entry_num();
	u32 i;

	if (warp_ctrl->warp_num != num)
		warp_dbg(WARP_DBG_ERR, "%s(): Error: wed_get_entry_num() = %d is diff. from warp_ctrl->warp_num = %d\n",
			__func__, num, warp_ctrl->warp_num);

	for (i = 0; i < warp_ctrl->warp_num; i++)
		warp_entry_exit(&warp_ctrl->entry[i]);

	warp_os_free_mem(warp_ctrl->entry);
	warp_os_free_mem(warp_ctrl->sw_conf);
}

/*
*
*/
static u32
warp_cr_search(struct warp_entry *entry, u32 cr)
{
	u32 i = 0;

	while (entry->mtbl[i].wifi_cr != 0) {
		if (entry->mtbl[i].wifi_cr == cr)
			return entry->mtbl[i].warp_cr;

		i++;
	}

	warp_dbg(WARP_DBG_ALL, "%s(): can't get warp cr from wifi cr %x\n", __func__,
		 cr);
	return 0;
}

/*
*
*/
static void
warp_wpdma_ring_init_hw(struct wed_entry *wed, struct wifi_entry *wifi)
{
#ifdef WED_TX_SUPPORT
	warp_tx_ring_init_hw(wed, wifi);
#endif /*WED_TX_SUPPORT*/
#ifdef WED_RX_SUPPORT
	warp_rx_ring_init_hw(wed, wifi);
#endif /*WED_RX_SUPPORT*/
#ifdef WED_HW_TX_SUPPORT
	/*initial buf mgmt setting*/
	warp_bfm_init_hw(wed);
#endif /*WED_HW_TX_SUPPORT*/

#ifdef WED_RX_D_SUPPORT
	warp_rx_data_ring_init_hw(wed, wifi);
	warp_rx_bm_init_hw(wed, wifi);
	warp_rx_rro_init_hw(wed, wifi);
	warp_rx_route_qm_init_hw(wed, wifi);
#endif
}

static inline bool
check_and_update_warp(struct warp_entry **warp, struct wed_entry **wed,
		      struct wdma_entry **wdma, struct wifi_entry **wifi, void *priv_data)
{
	struct warp_entry *_warp = warp_entry_get_by_privdata(priv_data);

	if (!_warp) {
		warp_dbg(WARP_DBG_ERR, "%s(): warp can't find for cookie:%p\n", __func__,
			 priv_data);
		return false;
	}
	*wed = &_warp->wed;
	*wdma = &_warp->wdma;
	*wifi = &_warp->wifi;
	*warp = _warp;
	return true;
}

/*
*
*/
static void
warp_wo_exit(struct warp_ctrl *warp_ctrl)
{
	u32 num = wed_get_entry_num();
	u32 i = 0;

	if (warp_ctrl->warp_num != num)
		warp_dbg(WARP_DBG_ERR, "%s(): Error: wed_get_entry_num() = %d is diff. from warp_ctrl->warp_num = %d\n", __func__, num, warp_ctrl->warp_num);

	for (i = 0; i < warp_ctrl->warp_num; i++) {
		struct warp_entry *entry = &warp_ctrl->entry[i];

		if (entry == NULL)
			warp_dbg(WARP_DBG_ERR, "%s(): warp_ctrl->entry is NULL!\n", __func__);

		if (entry->woif.wo_ctrl.cur_state != WO_STATE_UNDEFINED) {
			warp_whole_chip_wo_reset();
			warp_wo_set_apsrc_idle(entry->idx);
			break;
		}
	}
}

/*
* global function
*/

/*hook handler*/
/*
*
*/
u8
warp_get_wed_idx(void *priv_data)
{
	struct warp_entry *warp;

	warp = warp_entry_get_by_privdata(priv_data);

	if (!warp)
		return 0xff;

	return warp->idx;
}
EXPORT_SYMBOL(warp_get_wed_idx);

/*
* use for wifi client read/write CR mapping to wed cr:
* wifi driver-> wed cr -> wed hw -> wifi hw cr
*/
int
warp_proxy_read(
	void *priv_data,
	unsigned int addr,
	unsigned int *value)
{
	struct warp_entry *warp = warp_entry_get_by_privdata(priv_data);
	struct wed_entry *wed;
	u32 warp_cr = 0;

	if (!warp)
		return -1;

	wed = &warp->wed;
	warp_cr = warp_cr_search(warp, addr);

	if (warp_cr == 0)
		return -1;

	warp_io_read32(wed, warp_cr, value);
	warp_dbg(WARP_DBG_ALL, "%s(): Read addr (%x)=%x\n",
		 __func__,
		 addr,
		 *value);
	return 0;
}
EXPORT_SYMBOL(warp_proxy_read);

/*
*
*/
int
warp_proxy_write(
	void *priv_data,
	u32 addr,
	u32 value)
{
	struct warp_entry *warp = warp_entry_get_by_privdata(priv_data);
	struct wed_entry *wed;
	u32 warp_cr = 0;

	if (!warp)
		return -1;

	wed = &warp->wed;
	warp_cr = warp_cr_search(warp, addr);

	if (warp_cr == 0)
		return -1;


	warp_io_write32(wed, warp_cr, value);
	warp_dbg(WARP_DBG_ALL, "%s(): Write addr (%x)=%x\n",
		 __func__,
		 addr,
		 value);
	return 0;
}
EXPORT_SYMBOL(warp_proxy_write);

/*
* client request a wifi_entry structure and inital wifi_entry value
*/
struct wifi_hw *
warp_alloc_client(u32 chip_id, u8 bus_type, u8 wed_id, u32 slot_id, void *dev)
{
	if (!warp_cap_support(chip_id)) {
		warp_dbg(WARP_DBG_ERR, "%s(): chip = %x not support wifi hardware nat feature\n",
			 __func__, chip_id);
		return NULL;
	}

	if (bus_type == BUS_TYPE_PCIE) {
		slot_id = warp_get_pcie_slot((struct pci_dev *) dev);
		if (wed_id)
			slot_id = wed_id;
	}
	return warp_entry_acquire(slot_id);
}
EXPORT_SYMBOL(warp_alloc_client);

/*
* register client & using create a platform driver
*/
int
warp_register_client(struct wifi_hw *hw, struct wifi_ops *ops)
{
	int ret = 0;
	struct warp_ctrl *warp_ctrl = warp_ctrl_get();
	struct platform_driver *pdriver;
	struct wifi_entry *wifi = to_wifi_entry(hw);
	struct warp_entry *warp = to_warp_entry(wifi);
	static char name[MAX_NAME_SIZE] = "";

	if (!warp) {
		warp_dbg(WARP_DBG_OFF,
			 "%s(): can't acquire a new warp entry since full! priv_data: %p\n", __func__,
			 wifi->hw.priv);
		goto err;
	}
	/*assign wifi ops*/
	wifi->ops = ops;
	/*assign & create a pdriver*/
	ret = snprintf(name, sizeof(name), "%s%d", WHNAT_PLATFORM_DEV_NAME,
			warp_ctrl->warp_driver_idx++);
	if (ret < 0)
		goto err;

	pdriver = &warp->pdriver;
	pdriver->probe = warp_probe;
	pdriver->remove = warp_remove;
	pdriver->driver.name = name;
	pdriver->driver.owner = THIS_MODULE;
	pdriver->driver.of_match_table = warp_of_ids;
	warp_entry_proc_init(warp_ctrl, warp);
	warp_increase_ref();

	return platform_driver_register(pdriver);
err:
	return -1;
}
EXPORT_SYMBOL(warp_register_client);

/*
*
*/
void
warp_client_remove(void *priv_data)
{
	struct warp_entry *warp;
	struct warp_ctrl *warp_ctrl = warp_ctrl_get();

	warp = warp_entry_get_by_privdata(priv_data);

	if (!warp)
		return;

	platform_driver_unregister(&warp->pdriver);
	warp_entry_release(warp);
	warp_entry_proc_exit(warp_ctrl, warp);
	warp_dbg(WARP_DBG_OFF, "%s(): warp entry clear done!!\n", __func__);
}
EXPORT_SYMBOL(warp_client_remove);

/*
*
*/
int
warp_ring_init(void *priv_data)
{
	struct warp_entry *warp = NULL;
	struct wed_entry *wed = NULL;
	struct wdma_entry *wdma = NULL;
	struct wifi_entry *wifi = NULL;
	struct warp_bus *bus = warp_bus_get();

	if (check_and_update_warp(&warp, &wed, &wdma, &wifi, priv_data) == false)
		goto end;

#ifdef WED_INTER_AGENT_SUPPORT
	warp_dbg(WARP_DBG_INF, "%s(): wed:%p, wdma:%p, wifi:%p\n", __func__, wed, wdma,
		 wifi);

	warp_dbg(WARP_DBG_INF, "%s(): set warp %p hw setting,idx=%d\n", __func__, warp,
		 warp->idx);
	/*initial wed*/
	warp_wed_init_hw(wed, wdma);
	/*initial wdma*/
	warp_wdma_init_hw(wed, wdma, warp->idx);
	/*hw setting for wdma enable*/
	warp_wdma_ring_init_hw(wed, wdma);
	/*initial wed_rx_bm*/
	wed_rx_bm_init(wed, &warp->wifi.hw);
#ifdef WED_DYNAMIC_RXBM_SUPPORT
	if (IS_WED_HW_CAP(wed, WED_HW_CAP_DYN_RXBM))
		regist_ul_dybm_task(wed);
#endif /* WED_DYNAMIC_RXBM_SUPPORT */
	/*hw setting for wed enable*/
	warp_wpdma_ring_init_hw(wed, wifi);
	/*enable wed interrupt*/
	warp_int_ctrl_hw(wed, wifi, wdma, ALL_INT_AGENT,
			true, bus->pcie_ints_offset, warp->idx);
	/*enable wed ext interrupt*/
	warp_eint_init_hw(wed);
	warp_eint_ctrl_hw(wed, true);

	/*enable HW CR mirror or not*/
	if (warp->cfg.atc_en) {
		wifi_chip_atc_set(wifi, true);
		warp_atc_set_hw(wifi, bus, warp->idx, true);
	}
#endif
#ifdef WED_RX_D_SUPPORT
#ifdef CONFIG_WED_HW_RRO_SUPPORT
	/* setup cfg to wo*/
	warp_set_rrocfg(wed, warp->idx);
#endif
#endif

#ifdef WED_PAO_SUPPORT
	warp_pao_init_hw(wed);
#endif
end:
	return 0;
}
EXPORT_SYMBOL(warp_ring_init);

/*
*
*/
void
warp_ring_exit(void *priv_data)
{
#ifdef WED_INTER_AGENT_SUPPORT
	struct warp_entry *warp;
	struct warp_bus *bus = warp_bus_get();
	struct wifi_entry *wifi;
	struct wed_entry *wed;
	struct wdma_entry *wdma;

	if (check_and_update_warp(&warp, &wed, &wdma, &wifi, priv_data) == false)
		goto end;

	if (warp->cfg.atc_en) {
		wifi_chip_atc_set(wifi, false);
		warp_atc_set_hw(wifi, bus, warp->idx, false);
	}

	warp_decrease_ref();
	/*flush all hw path*/
	wifi_tx_tuple_reset(wifi);
	/*Reset Ring and HW setting*/
	warp_dma_ctrl_hw(wed, WARP_DMA_DISABLE);
	warp_eint_ctrl_hw(wed, false);
	warp_int_ctrl_hw(wed, wifi, wdma, ALL_INT_AGENT,
			false, bus->pcie_ints_offset, warp->idx);
	warp_reset_hw(wed, WARP_RESET_INTERFACE);
	wed_rx_bm_exit(wed);
#ifdef WED_DYNAMIC_RXBM_SUPPORT
	if (IS_WED_HW_CAP(wed, WED_HW_CAP_DYN_RXBM))
		unregist_ul_dybm_task(wed);
#endif	/* WED_DYNAMIC_RXBM_SUPPORT */
#endif	/* WED_INTER_AGENT_SUPPORT */
end:
	return;
}
EXPORT_SYMBOL(warp_ring_exit);

/*
*
*/
void
warp_wlan_tx(void *priv_data, unsigned char *tx_info)
{
	struct warp_entry *warp;

	warp = warp_entry_get_by_privdata(priv_data);

	if (!warp)
		return;

	if (wifi_hw_tx_allow(&warp->wifi, tx_info) == false)
		return;

#ifdef WED_HW_TX_SUPPORT
	{
		u32 wdma_rx_port = warp->wdma.wdma_rx_port;

		if (warp->cfg.hw_tx_en)
			wifi_tx_tuple_add(&warp->wifi, warp->idx, tx_info, wdma_rx_port);
	}
#endif /*WED_HW_TX_SUPPORT*/
}
EXPORT_SYMBOL(warp_wlan_tx);

void
warp_wlan_rx(void *priv_data, unsigned char *rx_info)
{
#ifdef WED_RX_D_SUPPORT
	struct warp_entry *warp;

	warp = warp_entry_get_by_privdata(priv_data);

	if (!warp)
		return;

#ifdef WED_HW_RX_SUPPORT
	wifi_rx_tuple_add(&warp->wifi, warp->idx, rx_info);
#endif
#endif
}
EXPORT_SYMBOL(warp_wlan_rx);

/*
*
*/
void
warp_isr_handler(void *priv_data)
{
#ifdef WED_INTER_AGENT_SUPPORT
	struct warp_entry *warp;
	struct wed_entry *wed;
	unsigned int status = 0;

	warp = warp_entry_get_by_privdata(priv_data);

	if (!warp)
		return;

	wed = &warp->wed;
	warp_eint_ctrl_hw(wed, false);
	warp_eint_get_stat_hw(wed, &status);
	warp_eint_work_hw(wed, status);
	warp_eint_ctrl_hw(wed, true);
#endif
}
EXPORT_SYMBOL(warp_isr_handler);

/*
*
*/
void
warp_dma_handler(void *priv_data, int dma_cfg)
{
#ifdef WED_INTER_AGENT_SUPPORT
	struct warp_entry *warp;

	warp = warp_entry_get_by_privdata(priv_data);

	if (!warp)
		return;

	warp_dbg(WARP_DBG_LOU, "%s(): Set DMA =%d\n", __func__, dma_cfg);
	warp_dma_ctrl_hw(&warp->wed, dma_cfg);
#endif
}
EXPORT_SYMBOL(warp_dma_handler);

/*
*
*/
void
warp_ser_handler(void *priv_data, u32 ser_status)
{
	struct warp_entry *warp;
	struct warp_bus *bus = warp_bus_get();
	struct wifi_entry *wifi;
	struct wdma_entry *wdma;
	struct wed_entry *wed;

	if (check_and_update_warp(&warp, &wed, &wdma, &wifi, priv_data) == false)
		goto end;

	warp_dbg(WARP_DBG_OFF, "%s(): status=%d\n", __func__, ser_status);

	switch (ser_status) {
	case WIFI_ERR_RECOV_STOP_IDLE: {
		if (warp->cfg.atc_en) {
			wifi_chip_atc_set(wifi, false);
			warp_atc_set_hw(wifi, bus, warp->idx, false);
		}

		warp_eint_ctrl_hw(wed, false);
		warp_int_ctrl_hw(wed, wifi, wdma, WDMA_INT_AGENT,
				false, bus->pcie_ints_offset, warp->idx);
	}
	break;

	case WIFI_ERR_RECOV_STOP_IDLE_DONE:
		warp_int_ctrl_hw(wed, wifi, wdma, ALL_INT_AGENT,
				true, bus->pcie_ints_offset, warp->idx);
		break;

	case WIFI_ERR_RECOV_STOP_PDMA0: {
		/*flush all hw path*/
		wifi_tx_tuple_reset(wifi);
		wed_tx_ring_reset(wed);
		wdma_tx_ring_reset(wdma);

		if (warp_reset_hw(wed, WARP_RESET_IDX_ONLY) < 0) {
			warp_dbg(WARP_DBG_INF, "%s(): Reset index faild, change to reset module!\n",
				 __func__);
			warp_reset_hw(wed, WARP_RESET_IDX_MODULE);
		}
#ifdef WED_RX_D_SUPPORT
		wed_rx_ring_reset(wed);
		wed_rx_bm_exit(wed);
#endif
	}
	break;

	case WIFI_ERR_RECOV_RESET_PDMA0:
	default:
		warp_dbg(WARP_DBG_INF, "%s(): status=%d, do nothing\n", __func__, ser_status);
		break;
	}
end:
	return;
}
EXPORT_SYMBOL(warp_ser_handler);

/*
*
*/
void
warp_suspend_handler(void *priv_data)
{
#ifdef CONFIG_WED_HW_RRO_SUPPORT
	struct warp_entry *warp;

	warp_dbg(WARP_DBG_OFF, "%s()\n", __func__);

	warp = warp_entry_get_by_privdata(priv_data);

	if (!warp)
		return;

	warp_woctrl_enter_state(warp, WO_STATE_GATING);
#endif
}
EXPORT_SYMBOL(warp_suspend_handler);


/*
*
*/
void
warp_resume_handler(void *priv_data)
{
#ifdef CONFIG_WED_HW_RRO_SUPPORT
	struct warp_entry *warp;

	warp_dbg(WARP_DBG_OFF, "%s()\n", __func__);

	warp = warp_entry_get_by_privdata(priv_data);

	if (!warp)
		return;

	warp_woctrl_exit_state(warp, WO_STATE_GATING);
#endif
}
EXPORT_SYMBOL(warp_resume_handler);

/*
*
*/
void
warp_512_support_handler(void *priv_data, u8 *enable)
{
	struct warp_entry *warp;
	struct wed_entry *wed;
	struct wdma_entry *wdma;
	struct wifi_entry *wifi;

	if (check_and_update_warp(&warp, &wed, &wdma, &wifi, priv_data) == false)
		goto end;

	warp_wed_512_support_hw(wed, enable);

end:
	return;
}
EXPORT_SYMBOL(warp_512_support_handler);

/*
*
*/
void
warp_hb_chk_handler(void *priv_data, u8 *stop)
{
#ifdef CONFIG_WED_HW_RRO_SUPPORT
	struct warp_entry *warp = NULL;
	struct wed_ser_ctrl *ser_ctrl = NULL;

	warp = warp_entry_get_by_privdata(priv_data);
	if (!warp)
		return;

	ser_ctrl = &warp->wed.ser_ctrl;
	*stop = ser_ctrl->wo_no_response;

	if (*stop != 0)
		warp->woif.wo_ctrl.cur_state = WO_STATE_WF_RESET;
#endif
}
EXPORT_SYMBOL(warp_hb_chk_handler);

/*global function*/
/*
 *   global variable
 */
static int __init warp_module_init(void)
{
	struct warp_ctrl *warp_ctrl = warp_ctrl_get();
	u32 i = 0;
#ifdef MEM_LEAK_DBG
	MemInfoListInital();
#endif	/* MEM_LEAK_DBG */
	warp_dbg(WARP_DBG_OFF, "%s(): module init and register callback for warp\n",
		 __func__);
	/*initial global warp control*/
	memset(warp_ctrl, 0, sizeof(struct warp_ctrl));
	spin_lock_init(&warp_ctrl->warp_lock);
	/*register proc*/
	warp_ctrl_proc_init(warp_ctrl);
	/*initial warp driver and check */
	warp_ctrl_init(warp_ctrl);
	/*initial pci cr mirror cfg*/
	bus_config_init(&warp_ctrl->bus);

	for (i = 0; i < warp_ctrl->warp_num; i++) {
		struct warp_entry *entry = &warp_ctrl->entry[i];

		if (entry == NULL)
			warp_dbg(WARP_DBG_ERR, "%s(): warp_ctrl->entry is NULL!\n", __func__);

		warp_wo_set_apsrc_idle(entry->idx);
	}

	return 0;
}

static void __exit warp_module_exit(void)
{
	struct warp_ctrl *warp_ctrl = warp_ctrl_get();

	warp_dbg(WARP_DBG_OFF, "%s(): warp module exist\n", __func__);
	warp_wo_exit(warp_ctrl);
	bus_config_exit(&warp_ctrl->bus);
	warp_ctrl_exit(warp_ctrl);
	warp_ctrl_proc_exit(warp_ctrl);
#ifdef MEM_LEAK_DBG
	MemInfoListExit();
#endif	/* MEM_LEAK_DBG */
}

module_init(warp_module_init);
module_exit(warp_module_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_SUPPORTED_DEVICE("mt7615 within mt7622");

