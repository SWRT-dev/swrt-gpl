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
	warp_wifi.c
*/
#include "warp_cfg.h"
#if defined(CONFIG_HW_NAT) || defined(CONFIG_RA_NAT_HW) || defined(CONFIG_NET_MEDIATEK_HNAT) || defined(CONFIG_NET_MEDIATEK_HNAT_MODULE)
#include <net/ra_nat.h>
#endif	/* CONFIG_HW_NAT || CONFIG_RA_NAT_HW || CONFIG_NET_MEDIATEK_HNAT || CONFIG_NET_MEDIATEK_HNAT_MODULE */
#include <net/ip.h>
#include <linux/tcp.h>
#include <linux/udp.h>
#include <linux/skbuff.h>
#include "warp_wifi.h"
#include "warp_utility.h"

/*Gloable function*/
#if defined(CONFIG_HW_NAT) || defined(CONFIG_RA_NAT_HW) || defined(CONFIG_NET_MEDIATEK_HNAT) || defined(CONFIG_NET_MEDIATEK_HNAT_MODULE)
#ifdef CONFIG_WARP_DBG_SUPPORT
/*
*
*/
static
void wifi_dump_skb(
	u8 idx,
	struct wlan_tx_info *info,
	struct sk_buff *skb)
{
	struct iphdr *hdr = ip_hdr(skb);

	warp_dbg(WARP_DBG_INF,
		 "%s(): add entry: wdma=%d,ringId=%d,wcid=%d,bssid=%d\n",
		 __func__,
		 idx,
		 info->ringidx,
		 info->wcid,
		 info->bssidx);

	if (hdr->version != 4)
		return;

	warp_dbg(WARP_DBG_INF,
		 "%s():src=%d.%d.%d.%d\n",
		 __func__,
		 (0xff & hdr->saddr),
		 (0xff00 & hdr->saddr) >> 8,
		 (0xff0000 & hdr->saddr) >> 16,
		 (0xff000000 & hdr->saddr) >> 24);

	warp_dbg(WARP_DBG_INF,
		 "%s(): dst=%d.%d.%d.%d\n",
		 __func__,
		 (0xff & hdr->daddr),
		 (0xff00 & hdr->daddr) >> 8,
		 (0xff0000 & hdr->daddr) >> 16,
		 (0xff000000 & hdr->daddr) >> 24);

	if (hdr->protocol == IPPROTO_TCP) {
		struct tcphdr *tcph = tcp_hdr(skb);
		warp_dbg(WARP_DBG_INF,
			 "%s(): protocol=TCP,sport=%d,dstport=%d\n",
			 __func__,
			 tcph->source,
			 tcph->dest);
	}

	if (hdr->protocol == IPPROTO_UDP) {
		struct udphdr *udph = udp_hdr(skb);
		warp_dbg(WARP_DBG_INF,
			 "%s(): protocol=UDP,sport=%d,dstport=%d\n",
			 __func__,
			 udph->source,
			 udph->dest);
	}
}
#endif /*CONFIG_WARP_DBG_SUPPORT*/

#define FOE_MAGIC_WED0 0x78
#define FOE_MAGIC_WED1 0x79
#define FOE_MAGIC_WED2 0x87

void
wifi_rx_tuple_add(struct wifi_entry *wifi, u8 idx, unsigned char *rx_info)
{
	struct wlan_rx_info r, *info =  &r;
	struct sk_buff *skb = NULL;

	memset(info, 0, sizeof(*info));
	wifi->ops->rxinfo_wrapper(rx_info, info);
	warp_dbg(WARP_DBG_INF, "%s(): WED IDX: %d,PPE Entry: %d, CSRN: %d\n",
		__func__,
		 idx, info->ppe_entry, info->csrn);

	skb = (struct sk_buff *)info->pkt;

	if (IS_SPACE_AVAILABLE_HEAD(skb)) {
		if (idx == 2)
			FOE_MAGIC_TAG(skb) = FOE_MAGIC_WED2;
		else if (idx == 1)
			FOE_MAGIC_TAG(skb) = FOE_MAGIC_WED1;
		else
			FOE_MAGIC_TAG(skb) = FOE_MAGIC_WED0;

		FOE_ENTRY_NUM(skb) = info->ppe_entry;
		FOE_AI(skb) = info->csrn;
	}
}

/*
*
*/
void
wifi_tx_tuple_add(struct wifi_entry *wifi, u8 idx, u8 *tx_info, u32 wdma_rx_port)
{
	struct wlan_tx_info t, *info =  &t;

	memset(info, 0, sizeof(*info));
	wifi->ops->txinfo_wrapper(tx_info, info);

	if (ra_sw_nat_hook_tx) {
		struct sk_buff *skb = (struct sk_buff *)info->pkt;

		if ((FOE_AI(skb) == HIT_UNBIND_RATE_REACH) ||
		    (FOE_AI_TAIL(skb) == HIT_UNBIND_RATE_REACH)) {
			if (IS_SPACE_AVAILABLE_HEAD(skb)) {
				/*WDMA idx*/
				FOE_WDMA_ID(skb) = idx;
				/*Ring idx*/
				FOE_RX_ID(skb) = info->ringidx;
				/*wtable Idx*/
				FOE_WC_ID(skb) = info->wcid;
				/*Bssidx*/
				FOE_BSS_ID(skb) = info->bssidx;
			}
			if (IS_SPACE_AVAILABLE_TAIL(skb)) {
				/*WDMA idx*/
				FOE_WDMA_ID_TAIL(skb) = idx;
				/*Ring idx*/
				FOE_RX_ID_TAIL(skb) = info->ringidx;
				/*wtable Idx*/
				FOE_WC_ID_TAIL(skb) = info->wcid;
				/*Bssidx*/
				FOE_BSS_ID_TAIL(skb) = info->bssidx;
			}
		}
		/*use port for specify which hw_nat architecture*/
		if (ra_sw_nat_hook_tx) {
			if ((FOE_AI(skb) == HIT_UNBIND_RATE_REACH)||
			     (FOE_AI_TAIL(skb) == HIT_UNBIND_RATE_REACH)) {
				warp_dbg(WARP_DBG_INF, "%s(): WDMAID: %d,RingID: %d, Wcid: %d, Bssid: %d\n",
					__func__,
					 FOE_WDMA_ID(skb), FOE_RX_ID(skb), FOE_WC_ID(skb), FOE_BSS_ID(skb));
			}
			if (ra_sw_nat_hook_tx(skb, wdma_rx_port) != 1) {
				wifi->ops->txinfo_set_drop(tx_info);
			}
		}
#ifdef CONFIG_WARP_DBG_SUPPORT
		wifi_dump_skb(idx, info, skb);
#endif /*CONFIG_WARP_DBG_SUPPORT*/
	}
}
#endif	/* CONFIG_HW_NAT || CONFIG_RA_NAT_HW || CONFIG_NET_MEDIATEK_HNAT || CONFIG_NET_MEDIATEK_HNAT_MODULE */
/*
*
*/
bool
wifi_hw_tx_allow(struct wifi_entry *wifi, unsigned char *tx_info)
{
	if (wifi->ops->hw_tx_allow)
		return wifi->ops->hw_tx_allow(tx_info);
	return true;
}

/*
*
*/
void
wifi_tx_tuple_reset(struct wifi_entry *wifi)
{
	/* FoeTblClean(); */
}

/*
* Wifi function part
*/

/*
*
*/
void
wifi_tx_ring_info_dump(struct wifi_entry *wifi, unsigned char ring_id,
		       unsigned int idx)
{
	wifi->ops->tx_ring_info_dump(wifi, ring_id, idx);
}

/*
* wifi address translate control configure
*/
void
wifi_chip_atc_set(struct wifi_entry *wifi, bool enable)
{
	wifi->ops->config_atc(wifi->hw.priv, enable);
}

/*
*
*/
void
wifi_chip_reset(struct wifi_entry *wifi)
{
	wifi->ops->do_wifi_reset(wifi->hw.priv);
}

/*
*
*/
void
wifi_chip_update_wo_rxcnt(struct wifi_entry *wifi, void *wo_rxcnt)
{
	if (wifi->ops->update_wo_rxcnt)
		wifi->ops->update_wo_rxcnt(wifi->hw.priv, wo_rxcnt);
	else
		warp_dbg(WARP_DBG_OFF, "invalid ops, dismissed\n");
}

/*
* CHIP related setting
*/
void
wifi_chip_probe(struct wifi_entry *wifi, u32 irq, u8 warp_ver, u8 warp_sub_ver, int warp_hw_caps)
{
	/* assign base_addr for read/write cr purpose */
	wifi->base_addr = wifi->hw.base_addr;
	/*swap client irq information if needed*/
	wifi->ops->swap_irq(wifi->hw.priv, irq);
	/*always disable hw cr mirror first */
	wifi_chip_atc_set(wifi, false);

	wifi->ops->warp_ver_notify(wifi->hw.priv, warp_ver, warp_sub_ver, warp_hw_caps);
}

/*
*
*/
void
wifi_chip_remove(struct wifi_entry *wifi)
{
	wifi_chip_atc_set(wifi, true);
	wifi->hw.base_addr = 0;
	wifi->hw.p_int_mask = NULL;
}
