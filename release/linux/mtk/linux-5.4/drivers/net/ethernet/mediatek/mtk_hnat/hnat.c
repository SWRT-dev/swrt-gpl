/*   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; version 2 of the License
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   Copyright (C) 2014-2016 Sean Wang <sean.wang@mediatek.com>
 *   Copyright (C) 2016-2017 John Crispin <blogic@openwrt.org>
 */

#include <linux/dma-mapping.h>
#include <linux/delay.h>
#include <linux/if.h>
#include <linux/io.h>
#include <linux/module.h>
#include <linux/of_device.h>
#include <linux/platform_device.h>
#include <linux/reset.h>
#include <linux/rtnetlink.h>
#include <net/netlink.h>

#include "nf_hnat_mtk.h"
#include "hnat.h"

#if 1 /* ASUS: skip specific VID */
extern int vlan_num[MTLAN_MAXINUM];
#endif

struct mtk_hnat *hnat_priv;
static struct socket *_hnat_roam_sock;
static struct work_struct _hnat_roam_work;
static struct delayed_work _hnat_flow_entry_teardown_work;

int (*ra_sw_nat_hook_rx)(struct sk_buff *skb) = NULL;
EXPORT_SYMBOL(ra_sw_nat_hook_rx);
int (*ra_sw_nat_hook_tx)(struct sk_buff *skb, int gmac_no) = NULL;
EXPORT_SYMBOL(ra_sw_nat_hook_tx);
void (*ra_sw_nat_clear_bind_entries)(void) = NULL;
EXPORT_SYMBOL(ra_sw_nat_clear_bind_entries);

int (*hnat_get_wdma_tx_port)(u32 wdma_idx) = NULL;
EXPORT_SYMBOL(hnat_get_wdma_tx_port);
int (*hnat_get_wdma_rx_port)(u32 wdma_idx) = NULL;
EXPORT_SYMBOL(hnat_get_wdma_rx_port);

int (*ppe_del_entry_by_mac)(unsigned char *mac) = NULL;
EXPORT_SYMBOL(ppe_del_entry_by_mac);
int (*ppe_del_entry_by_ip)(bool is_ipv4, void *addr) = NULL;
EXPORT_SYMBOL(ppe_del_entry_by_ip);
int (*ppe_del_entry_by_bssid_wcid)(u32 wdma_idx, u16 bssid, u16 wcid) = NULL;
EXPORT_SYMBOL(ppe_del_entry_by_bssid_wcid);

void (*ppe_dev_register_hook)(struct net_device *dev) = NULL;
EXPORT_SYMBOL(ppe_dev_register_hook);
void (*ppe_dev_unregister_hook)(struct net_device *dev) = NULL;
EXPORT_SYMBOL(ppe_dev_unregister_hook);
int (*mtk_tnl_encap_offload)(struct sk_buff *skb) = NULL;
EXPORT_SYMBOL(mtk_tnl_encap_offload);
int (*mtk_tnl_decap_offload)(struct sk_buff *skb) = NULL;
EXPORT_SYMBOL(mtk_tnl_decap_offload);
bool (*mtk_tnl_decap_offloadable)(struct sk_buff *skb) = NULL;
EXPORT_SYMBOL(mtk_tnl_decap_offloadable);
bool (*mtk_crypto_offloadable)(struct sk_buff *skb) = NULL;
EXPORT_SYMBOL(mtk_crypto_offloadable);

int (*hnat_set_wdma_pse_port_state)(u32 wdma_idx, bool up) = NULL;
EXPORT_SYMBOL(hnat_set_wdma_pse_port_state);

static void hnat_sma_build_entry(struct timer_list *t)
{
	int i;

	for (i = 0; i < CFG_PPE_NUM; i++)
		cr_set_field(hnat_priv->ppe_base[i] + PPE_TB_CFG,
			     SMA, SMA_FWD_CPU_BUILD_ENTRY);
}

struct foe_entry *hnat_get_foe_entry(u32 ppe_id, u32 index)
{
	if (index == 0x7fff || index >= hnat_priv->foe_etry_num ||
	    ppe_id >= CFG_PPE_NUM)
		return ERR_PTR(-EINVAL);

	return &hnat_priv->foe_table_cpu[ppe_id][index];
}
EXPORT_SYMBOL(hnat_get_foe_entry);

void hnat_cache_ebl(int enable)
{
	int i;

	for (i = 0; i < CFG_PPE_NUM; i++) {
		cr_set_field(hnat_priv->ppe_base[i] + PPE_CAH_CTRL, CAH_X_MODE, 1);
		cr_set_field(hnat_priv->ppe_base[i] + PPE_CAH_CTRL, CAH_X_MODE, 0);
		cr_set_field(hnat_priv->ppe_base[i] + PPE_CAH_CTRL, CAH_EN, enable);
	}
}
EXPORT_SYMBOL(hnat_cache_ebl);

static void hnat_reset_timestamp(struct timer_list *t)
{
	struct foe_entry *entry;
	int hash_index;

	hnat_cache_ebl(0);
	cr_set_field(hnat_priv->ppe_base[0] + PPE_TB_CFG, TCP_AGE, 0);
	cr_set_field(hnat_priv->ppe_base[0] + PPE_TB_CFG, UDP_AGE, 0);
	writel(0, hnat_priv->fe_base + 0x0010);

	for (hash_index = 0; hash_index < hnat_priv->foe_etry_num; hash_index++) {
		entry = hnat_priv->foe_table_cpu[0] + hash_index;
		if (entry->bfib1.state == BIND)
			entry->bfib1.time_stamp =
				readl(hnat_priv->fe_base + 0x0010) & (0xFFFF);
	}

	cr_set_field(hnat_priv->ppe_base[0] + PPE_TB_CFG, TCP_AGE, 1);
	cr_set_field(hnat_priv->ppe_base[0] + PPE_TB_CFG, UDP_AGE, 1);
	hnat_cache_ebl(1);

	mod_timer(&hnat_priv->hnat_reset_timestamp_timer, jiffies + 14400 * HZ);
}

static void cr_set_bits(void __iomem *reg, u32 bs)
{
	u32 val = readl(reg);

	val |= bs;
	writel(val, reg);
}

static void cr_clr_bits(void __iomem *reg, u32 bs)
{
	u32 val = readl(reg);

	val &= ~bs;
	writel(val, reg);
}

void cr_set_field(void __iomem *reg, u32 field, u32 val)
{
	unsigned int tv = readl(reg);

	tv &= ~field;
	tv |= ((val) << (ffs((unsigned int)field) - 1));
	writel(tv, reg);
}

/*boundary entry can't be used to accelerate data flow*/
void exclude_boundary_entry(struct foe_entry *foe_table_cpu)
{
	int entry_base = 0;
	int bad_entry, i, j;
	struct foe_entry *foe_entry;
	/*these entries are boundary every 128 entries*/
	int boundary_entry_offset[8] = { 12, 25, 38, 51, 76, 89, 102, 115};

	if (!foe_table_cpu)
		return;

	for (i = 0; entry_base < hnat_priv->foe_etry_num; i++) {
		/* set boundary entries as static*/
		for (j = 0; j < 8; j++) {
			bad_entry = entry_base + boundary_entry_offset[j];
			foe_entry = &foe_table_cpu[bad_entry];
			foe_entry->udib1.sta = 1;
		}
		entry_base = (i + 1) * 128;
	}
}

static int mtk_get_wdma_tx_port(u32 wdma_idx)
{
	if (wdma_idx == 0 || wdma_idx == 1 || wdma_idx == 2)
		return NR_PPE0_PORT;

	return -EINVAL;
}

static int mtk_get_wdma_rx_port(u32 wdma_idx)
{
	if (wdma_idx == 2)
		return NR_WDMA2_PORT;
	else if (wdma_idx == 1)
		return NR_WDMA1_PORT;
	else if (wdma_idx == 0)
		return NR_WDMA0_PORT;

	return -EINVAL;
}

static int mtk_set_wdma_pse_port_state(u32 wdma_idx, bool up)
{
	int port;

	port = mtk_get_wdma_rx_port(wdma_idx);
	if (port < 0)
		return -EINVAL;

	cr_set_field(hnat_priv->fe_base + MTK_FE_GLO_CFG(port),
		     MTK_FE_LINK_DOWN_P((u32)port), !up);

	return 0;
}

static int mtk_set_ppe_pse_port_state(u32 ppe_id, bool up)
{
	u32 port;

	if (ppe_id == 0)
		port = NR_PPE0_PORT;
	else if (ppe_id == 1)
		port = NR_PPE1_PORT;
	else if (ppe_id == 2)
		port = NR_PPE2_PORT;
	else
		return -EINVAL;

	cr_set_field(hnat_priv->fe_base + MTK_FE_GLO_CFG(port),
		     MTK_FE_LINK_DOWN_P(port), !up);

	return 0;
}

void set_gmac_ppe_fwd(int id, int enable)
{
	void __iomem *reg;
	u32 val;

	reg = hnat_priv->fe_base +
		((id == NR_GMAC1_PORT) ? GDMA1_FWD_CFG :
		 (id == NR_GMAC2_PORT) ? GDMA2_FWD_CFG : GDMA3_FWD_CFG);

	if (enable) {
#if defined(CONFIG_MEDIATEK_NETSYS_V2) || defined(CONFIG_MEDIATEK_NETSYS_V3)
		if (CFG_PPE_NUM >= 3 && id == NR_GMAC3_PORT)
			cr_set_field(reg, GDM_ALL_FRC_MASK, BITS_GDM_ALL_FRC_P_PPE2);
		else if (CFG_PPE_NUM >= 2 && id == NR_GMAC2_PORT)
			cr_set_field(reg, GDM_ALL_FRC_MASK, BITS_GDM_ALL_FRC_P_PPE1);
		else
			cr_set_field(reg, GDM_ALL_FRC_MASK, BITS_GDM_ALL_FRC_P_PPE);
#else
		cr_set_field(reg, GDM_ALL_FRC_MASK, BITS_GDM_ALL_FRC_P_PPE);
#endif
		return;
	}

	/*disabled */
	val = readl(reg);
#if defined(CONFIG_MEDIATEK_NETSYS_V2) || defined(CONFIG_MEDIATEK_NETSYS_V3)
	if ((CFG_PPE_NUM >= 2 &&
	    ((val & GDM_ALL_FRC_MASK) == BITS_GDM_ALL_FRC_P_PPE1 ||
	     (val & GDM_ALL_FRC_MASK) == BITS_GDM_ALL_FRC_P_PPE2)))
		cr_set_field(reg, GDM_ALL_FRC_MASK,
			     BITS_GDM_ALL_FRC_P_CPU_PDMA);
#endif

	if ((val & GDM_ALL_FRC_MASK) == BITS_GDM_ALL_FRC_P_PPE)
		cr_set_field(reg, GDM_ALL_FRC_MASK,
				 BITS_GDM_ALL_FRC_P_CPU_PDMA);

}

static int entry_mac_cmp(struct foe_entry *entry, u8 *mac)
{
	int ret = 0;

	if(IS_IPV4_GRP(entry)) {
		if(((swab32(entry->ipv4_hnapt.dmac_hi) == *(u32 *)mac) &&
			(swab16(entry->ipv4_hnapt.dmac_lo) == *(u16 *)&mac[4])) ||
			((swab32(entry->ipv4_hnapt.smac_hi) == *(u32 *)mac) &&
			(swab16(entry->ipv4_hnapt.smac_lo) == *(u16 *)&mac[4])))
			ret = 1;
	} else {
		if(((swab32(entry->ipv6_5t_route.dmac_hi) == *(u32 *)mac) &&
			(swab16(entry->ipv6_5t_route.dmac_lo) == *(u16 *)&mac[4])) ||
			((swab32(entry->ipv6_5t_route.smac_hi) == *(u32 *)mac) &&
			(swab16(entry->ipv6_5t_route.smac_lo) == *(u16 *)&mac[4])))
			ret = 1;
	}

	if (ret && debug_level >= 2)
		pr_info("mac=%pM\n", mac);

	return ret;
}

static int entry_ip_cmp(struct foe_entry *entry, bool is_ipv4, void *addr)
{
	struct in6_addr *tmp_ipv6;
	struct in6_addr ipv6 = {0};
	struct in6_addr foe_sipv6 = {0};
	struct in6_addr foe_dipv6 = {0};
	u32 *tmp_ipv4, ipv4;
	u32 *sipv6_0 = NULL;
	u32 *dipv6_0 = NULL;
	int ret = 0;

	if (is_ipv4) {
		tmp_ipv4 = (u32 *)addr;
		ipv4 = ntohl(*tmp_ipv4);

		switch ((int)entry->bfib1.pkt_type) {
		case IPV4_HNAPT:
		case IPV4_HNAT:
			if (entry->ipv4_hnapt.sip == ipv4 ||
			    entry->ipv4_hnapt.new_dip == ipv4)
				ret = 1;
			break;
		case IPV4_DSLITE:
		case IPV4_MAP_E:
			if (entry->ipv4_dslite.sip == ipv4 ||
			    entry->ipv4_dslite.dip == ipv4)
				ret = 1;
			break;
		default:
			break;
		}
	} else {
		memset(&foe_sipv6, 0, sizeof(struct in6_addr));
		memset(&foe_dipv6, 0, sizeof(struct in6_addr));
		memset(&ipv6, 0, sizeof(struct in6_addr));

		tmp_ipv6 = (struct in6_addr *)addr;
		ipv6.s6_addr32[0] = ntohl(tmp_ipv6->s6_addr32[0]);
		ipv6.s6_addr32[1] = ntohl(tmp_ipv6->s6_addr32[1]);
		ipv6.s6_addr32[2] = ntohl(tmp_ipv6->s6_addr32[2]);
		ipv6.s6_addr32[3] = ntohl(tmp_ipv6->s6_addr32[3]);

		switch ((int)entry->bfib1.pkt_type) {
		case IPV6_3T_ROUTE:
		case IPV6_5T_ROUTE:
		case IPV6_6RD:
			sipv6_0 = &(entry->ipv6_3t_route.ipv6_sip0);
			dipv6_0 = &(entry->ipv6_3t_route.ipv6_dip0);
			break;
#if defined(CONFIG_MEDIATEK_NETSYS_V3)
		case IPV6_HNAT:
		case IPV6_HNAPT:
			sipv6_0 = &(entry->ipv6_hnapt.ipv6_sip0);
			dipv6_0 = &(entry->ipv6_hnapt.new_ipv6_ip0);
			break;
#endif
		default:
			break;
		}

		if (sipv6_0 && dipv6_0) {
			memcpy(&foe_sipv6, sipv6_0, sizeof(struct in6_addr));
			memcpy(&foe_dipv6, dipv6_0, sizeof(struct in6_addr));
			if (!memcmp(&foe_sipv6, &ipv6, sizeof(struct in6_addr)) ||
			    !memcmp(&foe_dipv6, &ipv6, sizeof(struct in6_addr)))
				ret = 1;
		}
	}

	if (ret && debug_level >= 2) {
		if (is_ipv4)
			pr_info("ipv4=%pI4\n", tmp_ipv4);
		else
			pr_info("ipv6=%pI6\n", tmp_ipv6);
	}

	return ret;
}

int entry_delete_by_mac(u8 *mac)
{
	struct foe_entry *entry = NULL;
	int index, i, ret = 0;

	for (i = 0; i < CFG_PPE_NUM; i++) {
		entry = hnat_priv->foe_table_cpu[i];
		for (index = 0; index < DEF_ETRY_NUM; entry++, index++) {
			if(entry->bfib1.state == BIND && entry_mac_cmp(entry, mac)) {
				memset(entry, 0, sizeof(*entry));
				hnat_cache_ebl(1);
				if (debug_level >= 2)
					pr_info("delete entry idx = %d\n", index);
				ret++;
			}
		}
	}

	if(!ret && debug_level >= 2)
		pr_info("entry not found\n");

	return ret;
}

int entry_delete_by_ip(bool is_ipv4, void *addr)
{
	struct foe_entry *entry = NULL;
	int index, i, ret = 0;

	for (i = 0; i < CFG_PPE_NUM; i++) {
		entry = hnat_priv->foe_table_cpu[i];
		for (index = 0; index < DEF_ETRY_NUM; entry++, index++) {
			if (entry->bfib1.state == BIND && entry_ip_cmp(entry, is_ipv4, addr)) {
				memset(entry, 0, sizeof(*entry));
				hnat_cache_ebl(1);
				if (debug_level >= 2)
					pr_info("delete entry idx = %d\n", index);
				ret++;
			}
		}
	}

	if (!ret && debug_level >= 2)
		pr_info("entry not found\n");

	return ret;
}

static int entry_delete_by_bssid_wcid(u32 wdma_idx, u16 bssid, u16 wcid)
{
	struct foe_entry *entry = NULL;
	int index, i;
	int ret = 0;
	int port;

	port = mtk_get_wdma_rx_port(wdma_idx);

	if (port < 0)
		return -EINVAL;

	for (i = 0; i < CFG_PPE_NUM; i++) {
		entry = hnat_priv->foe_table_cpu[i];
		for (index = 0; index < DEF_ETRY_NUM; entry++, index++) {
			if (entry->bfib1.state != BIND)
				continue;

			if (IS_IPV4_GRP(entry)) {
				if (entry->ipv4_hnapt.winfo.bssid != bssid ||
				    entry->ipv4_hnapt.winfo.wcid != wcid ||
				    entry->ipv4_hnapt.iblk2.dp != port)
					continue;
			} else {
				if (entry->ipv6_5t_route.winfo.bssid != bssid ||
				    entry->ipv6_5t_route.winfo.wcid != wcid ||
				    entry->ipv6_5t_route.iblk2.dp != port)
					continue;
			}

			memset(entry, 0, sizeof(*entry));
			hnat_cache_ebl(1);
			if (debug_level >= 2)
				pr_info("delete entry idx = %d\n", index);
			ret++;
		}
	}
	return ret;
}

static void hnat_roam_handler(struct work_struct *work)
{
	struct kvec iov;
	struct msghdr msg;
	struct nlmsghdr *nlh;
	struct ndmsg *ndm;
	struct nlattr *nla;
	u8 rcv_buf[512];
	int len;

	if (!_hnat_roam_sock)
		return;

	iov.iov_base = rcv_buf;
	iov.iov_len = sizeof(rcv_buf);
	memset(&msg, 0, sizeof(msg));
	msg.msg_namelen = sizeof(struct sockaddr_nl);

	len = kernel_recvmsg(_hnat_roam_sock, &msg, &iov, 1, iov.iov_len, 0);
	if (len <= 0)
		goto out;

	nlh = (struct nlmsghdr*)rcv_buf;
	if (!NLMSG_OK(nlh, len) || nlh->nlmsg_type != RTM_NEWNEIGH)
		goto out;

	len = nlh->nlmsg_len - NLMSG_HDRLEN;
	ndm = (struct ndmsg *)NLMSG_DATA(nlh);
	if (ndm->ndm_family != PF_BRIDGE)
		goto out;

	nla = (struct nlattr *)((u8 *)ndm + sizeof(struct ndmsg));
	len -= NLMSG_LENGTH(sizeof(struct ndmsg));
	while (nla_ok(nla, len)) {
		if (nla_type(nla) == NDA_LLADDR) {
			entry_delete_by_mac(nla_data(nla));
		}
		nla = nla_next(nla, &len);
	}

out:
	schedule_work(&_hnat_roam_work);
}

static int hnat_roaming_enable(void)
{
	struct socket *sock = NULL;
	struct sockaddr_nl addr;
	int ret;

	INIT_WORK(&_hnat_roam_work, hnat_roam_handler);

	ret = sock_create_kern(&init_net, AF_NETLINK, SOCK_RAW, NETLINK_ROUTE, &sock);
	if (ret < 0)
		goto out;

	_hnat_roam_sock = sock;

	addr.nl_family = AF_NETLINK;
	addr.nl_pad = 0;
	addr.nl_pid = 65534;
	addr.nl_groups = 1 << (RTNLGRP_NEIGH - 1);
	ret = kernel_bind(sock, (struct sockaddr *)&addr, sizeof(addr));
	if (ret < 0)
		goto out;

	schedule_work(&_hnat_roam_work);
	pr_info("hnat roaming work enable\n");

	return 0;
out:
	if (sock)
		sock_release(sock);

	return ret;
}

static void hnat_roaming_disable(void)
{
	if (_hnat_roam_sock)
		sock_release(_hnat_roam_sock);
	_hnat_roam_sock = NULL;
	pr_info("hnat roaming work disable\n");
}

static void hnat_flow_entry_teardown_all(u32 ppe_id)
{
	struct hnat_flow_entry *flow_entry;
	struct hlist_head *head;
	struct hlist_node *n;
	int index;

	spin_lock_bh(&hnat_priv->flow_entry_lock);
	for (index = 0; index < DEF_ETRY_NUM / 4; index++) {
		head = &hnat_priv->foe_flow[ppe_id][index];
		hlist_for_each_entry_safe(flow_entry, n, head, list) {
			hnat_flow_entry_delete(flow_entry);
		}
	}
	spin_unlock_bh(&hnat_priv->flow_entry_lock);
}

static void hnat_flow_entry_teardown_handler(struct work_struct *work)
{
	struct hnat_flow_entry *flow_entry;
	struct hlist_head *head;
	struct hlist_node *n;
	unsigned long now = jiffies;
	int index, i;
	u32 cnt = 0;

	spin_lock_bh(&hnat_priv->flow_entry_lock);
	for (i = 0; i < CFG_PPE_NUM; i++) {
		for (index = 0; index < DEF_ETRY_NUM / 4; index++) {
			head = &hnat_priv->foe_flow[i][index];
			hlist_for_each_entry_safe(flow_entry, n, head, list) {
				/* If the entry is updated before 30 seconds, teardown it. */
				if (time_after(now, flow_entry->last_update + msecs_to_jiffies(30 * 1000))) {
					hnat_flow_entry_delete(flow_entry);
					cnt++;
				}
			}
		}
	}
	spin_unlock_bh(&hnat_priv->flow_entry_lock);

	if (debug_level >= 2 && cnt > 0)
		pr_info("[%s]: Teardown %d entries\n", __func__, cnt);

	schedule_delayed_work(&_hnat_flow_entry_teardown_work, msecs_to_jiffies(1000));
}

static void hnat_flow_entry_teardown_enable(void)
{
	INIT_DELAYED_WORK(&_hnat_flow_entry_teardown_work, hnat_flow_entry_teardown_handler);
	schedule_delayed_work(&_hnat_flow_entry_teardown_work, msecs_to_jiffies(1000));
}

static void hnat_flow_entry_teardown_disable(void)
{
	cancel_delayed_work(&_hnat_flow_entry_teardown_work);
}

static int hnat_hw_init(u32 ppe_id)
{
	if (ppe_id >= CFG_PPE_NUM)
		return -EINVAL;

#if 1 /* ASUS: skip specific VID */
	memset(vlan_num, 0, sizeof(vlan_num));
#endif

	/* setup hashing */
	cr_set_field(hnat_priv->ppe_base[ppe_id] + PPE_TB_CFG, TB_ETRY_NUM, hnat_priv->etry_num_cfg);
	cr_set_field(hnat_priv->ppe_base[ppe_id] + PPE_TB_CFG, HASH_MODE, HASH_MODE_1);
	writel(HASH_SEED_KEY, hnat_priv->ppe_base[ppe_id] + PPE_HASH_SEED);
	cr_set_field(hnat_priv->ppe_base[ppe_id] + PPE_TB_CFG, XMODE, 0);
	cr_set_field(hnat_priv->ppe_base[ppe_id] + PPE_TB_CFG, TB_ENTRY_SIZE,
		     (hnat_priv->data->version == MTK_HNAT_V3) ? ENTRY_128B :
		     (hnat_priv->data->version == MTK_HNAT_V2) ? ENTRY_96B :
								 ENTRY_80B);
	cr_set_field(hnat_priv->ppe_base[ppe_id] + PPE_TB_CFG, SMA, SMA_FWD_CPU_BUILD_ENTRY);

	/* set ip proto */
	writel(0xFFFFFFFF, hnat_priv->ppe_base[ppe_id] + PPE_IP_PROT_CHK);

	/* setup caching */
	hnat_cache_ebl(1);

	/* enable FOE */
	cr_set_bits(hnat_priv->ppe_base[ppe_id] + PPE_FLOW_CFG,
		    BIT_IPV4_NAT_EN | BIT_IPV4_NAPT_EN |
		    BIT_IPV4_NAT_FRAG_EN | BIT_IPV4_HASH_GREK |
		    BIT_IPV4_DSL_EN | BIT_IPV6_6RD_EN |
		    BIT_IPV6_3T_ROUTE_EN | BIT_IPV6_5T_ROUTE_EN);

	if (hnat_priv->data->version == MTK_HNAT_V2 ||
	    hnat_priv->data->version == MTK_HNAT_V3)
		cr_set_bits(hnat_priv->ppe_base[ppe_id] + PPE_FLOW_CFG,
			    BIT_IPV4_MAPE_EN | BIT_IPV4_MAPT_EN);

	if (hnat_priv->data->version == MTK_HNAT_V3)
		cr_set_bits(hnat_priv->ppe_base[ppe_id] + PPE_FLOW_CFG,
			    BIT_IPV6_NAT_EN | BIT_IPV6_NAPT_EN |
			    BIT_CS0_RM_ALL_IP6_IP_EN);

	/* setup FOE aging */
	cr_set_field(hnat_priv->ppe_base[ppe_id] + PPE_TB_CFG, NTU_AGE, 1);
	cr_set_field(hnat_priv->ppe_base[ppe_id] + PPE_TB_CFG, UNBD_AGE, 1);
	cr_set_field(hnat_priv->ppe_base[ppe_id] + PPE_UNB_AGE, UNB_MNP, 1000);
	cr_set_field(hnat_priv->ppe_base[ppe_id] + PPE_UNB_AGE, UNB_DLTA, 3);
	cr_set_field(hnat_priv->ppe_base[ppe_id] + PPE_TB_CFG, TCP_AGE, 1);
	cr_set_field(hnat_priv->ppe_base[ppe_id] + PPE_TB_CFG, UDP_AGE, 1);
	cr_set_field(hnat_priv->ppe_base[ppe_id] + PPE_TB_CFG, FIN_AGE, 1);
	cr_set_field(hnat_priv->ppe_base[ppe_id] + PPE_BND_AGE_0, UDP_DLTA, 12);
	cr_set_field(hnat_priv->ppe_base[ppe_id] + PPE_BND_AGE_0, NTU_DLTA, 1);
	cr_set_field(hnat_priv->ppe_base[ppe_id] + PPE_BND_AGE_1, FIN_DLTA, 1);
	cr_set_field(hnat_priv->ppe_base[ppe_id] + PPE_BND_AGE_1, TCP_DLTA, 7);

	/* setup FOE ka */
	cr_set_field(hnat_priv->ppe_base[ppe_id] + PPE_TB_CFG, KA_CFG, 0);
	cr_set_field(hnat_priv->ppe_base[ppe_id] + PPE_BIND_LMT_1, NTU_KA, 0);
	cr_set_field(hnat_priv->ppe_base[ppe_id] + PPE_KA, KA_T, 0);
	cr_set_field(hnat_priv->ppe_base[ppe_id] + PPE_KA, TCP_KA, 0);
	cr_set_field(hnat_priv->ppe_base[ppe_id] + PPE_KA, UDP_KA, 0);
	mdelay(10);

	cr_set_field(hnat_priv->ppe_base[ppe_id] + PPE_TB_CFG, SCAN_MODE, 2);
	cr_set_field(hnat_priv->ppe_base[ppe_id] + PPE_TB_CFG, KA_CFG, 3);
	cr_set_field(hnat_priv->ppe_base[ppe_id] + PPE_TB_CFG, TICK_SEL, 0);
	cr_set_field(hnat_priv->ppe_base[ppe_id] + PPE_KA, KA_T, 1);
	cr_set_field(hnat_priv->ppe_base[ppe_id] + PPE_KA, TCP_KA, 1);
	cr_set_field(hnat_priv->ppe_base[ppe_id] + PPE_KA, UDP_KA, 1);
	cr_set_field(hnat_priv->ppe_base[ppe_id] + PPE_BIND_LMT_1, NTU_KA, 1);

	/* setup FOE rate limit */
	cr_set_field(hnat_priv->ppe_base[ppe_id] + PPE_BIND_LMT_0, QURT_LMT, 16383);
	cr_set_field(hnat_priv->ppe_base[ppe_id] + PPE_BIND_LMT_0, HALF_LMT, 16383);
	cr_set_field(hnat_priv->ppe_base[ppe_id] + PPE_BIND_LMT_1, FULL_LMT, 16383);
	/* setup binding threshold as 30 packets per second */
	cr_set_field(hnat_priv->ppe_base[ppe_id] + PPE_BNDR, BIND_RATE, 0x1E);

	/* setup FOE cf gen */
	cr_set_field(hnat_priv->ppe_base[ppe_id] + PPE_GLO_CFG, PPE_EN, 1);
	writel(0, hnat_priv->ppe_base[ppe_id] + PPE_DFT_CPORT); /* pdma */
	/* writel(0x55555555, hnat_priv->ppe_base[ppe_id] + PPE_DFT_CPORT); */ /* qdma */
	cr_set_field(hnat_priv->ppe_base[ppe_id] + PPE_GLO_CFG, TTL0_DRP, 0);
	cr_set_field(hnat_priv->ppe_base[ppe_id] + PPE_GLO_CFG, MCAST_TB_EN, 1);

	if (hnat_priv->data->version == MTK_HNAT_V2 ||
	    hnat_priv->data->version == MTK_HNAT_V3) {
		writel(0xcb777, hnat_priv->ppe_base[ppe_id] + PPE_DFT_CPORT1);
		writel(0x7f, hnat_priv->ppe_base[ppe_id] + PPE_SBW_CTRL);
		cr_set_field(hnat_priv->ppe_base[ppe_id] + PPE_GLO_CFG, SP_CMP_EN, 1);
	}

	if (hnat_priv->data->version == MTK_HNAT_V3) {
		cr_set_field(hnat_priv->ppe_base[ppe_id] + PPE_SB_FIFO_DBG,
			     SB_MED_FULL_DRP_EN, 1);
		cr_set_bits(hnat_priv->ppe_base[ppe_id] + PPE_GLO_CFG,
			    NEW_IPV4_ID_INC_EN | TSID_EN);
	}

	/*enable ppe mib counter*/
	if (hnat_priv->data->per_flow_accounting) {
		cr_set_field(hnat_priv->ppe_base[ppe_id] + PPE_MIB_CFG, MIB_EN, 1);
		cr_set_field(hnat_priv->ppe_base[ppe_id] + PPE_MIB_CFG, MIB_READ_CLEAR, 1);
		cr_set_field(hnat_priv->ppe_base[ppe_id] + PPE_MIB_CAH_CTRL, MIB_CAH_EN, 1);
	}

	hnat_priv->g_ppdev = dev_get_by_name(&init_net, hnat_priv->ppd);
	hnat_priv->g_wandev = dev_get_by_name(&init_net, hnat_priv->wan);

	dev_info(hnat_priv->dev, "PPE%d hwnat start\n", ppe_id);

	spin_lock_init(&hnat_priv->entry_lock);
	spin_lock_init(&hnat_priv->flow_entry_lock);
	return 0;
}

static int hnat_start(u32 ppe_id)
{
	u32 foe_table_sz;
	u32 foe_mib_tb_sz;
	u32 foe_flow_sz;
	int etry_num_cfg;

	if (ppe_id >= CFG_PPE_NUM)
		return -EINVAL;

	/* mapp the FOE table */
	for (etry_num_cfg = DEF_ETRY_NUM_CFG ; etry_num_cfg >= 0 ;
	     etry_num_cfg--, hnat_priv->foe_etry_num /= 2) {
		foe_table_sz = hnat_priv->foe_etry_num * sizeof(struct foe_entry);
		hnat_priv->foe_table_cpu[ppe_id] = dma_alloc_coherent(
				hnat_priv->dev, foe_table_sz,
				&hnat_priv->foe_table_dev[ppe_id], GFP_KERNEL);
		if (hnat_priv->foe_table_cpu[ppe_id]) {
			foe_flow_sz = (hnat_priv->foe_etry_num / 4) * sizeof(struct hlist_head);
			hnat_priv->foe_flow[ppe_id] = devm_kzalloc(hnat_priv->dev, foe_flow_sz, GFP_KERNEL);
			if (!hnat_priv->foe_flow[ppe_id]) {
				dma_free_coherent(hnat_priv->dev, foe_table_sz,
						  hnat_priv->foe_table_cpu[ppe_id],
						  hnat_priv->foe_table_dev[ppe_id]);
				hnat_priv->foe_table_cpu[ppe_id] = NULL;
			} else {
				break;
			}
		}
	}

	if (!hnat_priv->foe_table_cpu[ppe_id] || !hnat_priv->foe_flow[ppe_id])
		return -1;
	dev_info(hnat_priv->dev, "PPE%d entry number = %d\n",
		 ppe_id, hnat_priv->foe_etry_num);

	writel(hnat_priv->foe_table_dev[ppe_id], hnat_priv->ppe_base[ppe_id] + PPE_TB_BASE);
	memset(hnat_priv->foe_table_cpu[ppe_id], 0, foe_table_sz);

	if (hnat_priv->data->version == MTK_HNAT_V1_1)
		exclude_boundary_entry(hnat_priv->foe_table_cpu[ppe_id]);

	if (hnat_priv->data->per_flow_accounting) {
		foe_mib_tb_sz = hnat_priv->foe_etry_num * sizeof(struct mib_entry);
		hnat_priv->foe_mib_cpu[ppe_id] =
			dma_alloc_coherent(hnat_priv->dev, foe_mib_tb_sz,
					   &hnat_priv->foe_mib_dev[ppe_id], GFP_KERNEL);
		if (!hnat_priv->foe_mib_cpu[ppe_id])
			return -1;
		writel(hnat_priv->foe_mib_dev[ppe_id],
		       hnat_priv->ppe_base[ppe_id] + PPE_MIB_TB_BASE);
		memset(hnat_priv->foe_mib_cpu[ppe_id], 0, foe_mib_tb_sz);

		hnat_priv->acct[ppe_id] =
			kzalloc(hnat_priv->foe_etry_num * sizeof(struct hnat_accounting),
				GFP_KERNEL);
		if (!hnat_priv->acct[ppe_id])
			return -1;
	}

	hnat_priv->etry_num_cfg = etry_num_cfg;
	hnat_hw_init(ppe_id);

	return 0;
}

static int ppe_busy_wait(u32 ppe_id)
{
	unsigned long t_start = jiffies;
	u32 r = 0;

	if (ppe_id >= CFG_PPE_NUM)
		return -EINVAL;

	while (1) {
		r = readl((hnat_priv->ppe_base[ppe_id] + 0x0));
		if (!(r & BIT(31)))
			return 0;
		if (time_after(jiffies, t_start + HZ))
			break;
		mdelay(10);
	}

	dev_notice(hnat_priv->dev, "ppe:%s timeout\n", __func__);

	return -1;
}

static void hnat_stop(u32 ppe_id)
{
	u32 foe_table_sz;
	u32 foe_mib_tb_sz;
	struct foe_entry *entry, *end;

	if (ppe_id >= CFG_PPE_NUM)
		return;

	/* send all traffic back to the DMA engine */
	set_gmac_ppe_fwd(NR_GMAC1_PORT, 0);
	set_gmac_ppe_fwd(NR_GMAC2_PORT, 0);
	set_gmac_ppe_fwd(NR_GMAC3_PORT, 0);

	dev_info(hnat_priv->dev, "hwnat stop\n");

	if (hnat_priv->foe_table_cpu[ppe_id]) {
		entry = hnat_priv->foe_table_cpu[ppe_id];
		end = hnat_priv->foe_table_cpu[ppe_id] + hnat_priv->foe_etry_num;
		while (entry < end) {
			entry->bfib1.state = INVALID;
			entry++;
		}
	}
	/* disable caching */
	hnat_cache_ebl(0);

	/* flush cache has to be ahead of hnat disable --*/
	cr_set_field(hnat_priv->ppe_base[ppe_id] + PPE_GLO_CFG, PPE_EN, 0);

	/* disable scan mode and keep-alive */
	cr_set_field(hnat_priv->ppe_base[ppe_id] + PPE_TB_CFG, SCAN_MODE, 0);
	cr_set_field(hnat_priv->ppe_base[ppe_id] + PPE_TB_CFG, KA_CFG, 0);

	ppe_busy_wait(ppe_id);

	/* disable FOE */
	cr_clr_bits(hnat_priv->ppe_base[ppe_id] + PPE_FLOW_CFG,
		    BIT_IPV4_NAPT_EN | BIT_IPV4_NAT_EN | BIT_IPV4_NAT_FRAG_EN |
		    BIT_IPV6_HASH_GREK | BIT_IPV4_DSL_EN |
		    BIT_IPV6_6RD_EN | BIT_IPV6_3T_ROUTE_EN |
		    BIT_IPV6_5T_ROUTE_EN | BIT_FUC_FOE | BIT_FMC_FOE);

	if (hnat_priv->data->version == MTK_HNAT_V2 ||
	    hnat_priv->data->version == MTK_HNAT_V3)
		cr_clr_bits(hnat_priv->ppe_base[ppe_id] + PPE_FLOW_CFG,
			    BIT_IPV4_MAPE_EN | BIT_IPV4_MAPT_EN);

	if (hnat_priv->data->version == MTK_HNAT_V3)
		cr_clr_bits(hnat_priv->ppe_base[ppe_id] + PPE_FLOW_CFG,
			    BIT_IPV6_NAT_EN | BIT_IPV6_NAPT_EN |
			    BIT_CS0_RM_ALL_IP6_IP_EN);

	/* disable FOE aging */
	cr_set_field(hnat_priv->ppe_base[ppe_id] + PPE_TB_CFG, NTU_AGE, 0);
	cr_set_field(hnat_priv->ppe_base[ppe_id] + PPE_TB_CFG, UNBD_AGE, 0);
	cr_set_field(hnat_priv->ppe_base[ppe_id] + PPE_TB_CFG, TCP_AGE, 0);
	cr_set_field(hnat_priv->ppe_base[ppe_id] + PPE_TB_CFG, UDP_AGE, 0);
	cr_set_field(hnat_priv->ppe_base[ppe_id] + PPE_TB_CFG, FIN_AGE, 0);

	/* free the FOE table */
	foe_table_sz = hnat_priv->foe_etry_num * sizeof(struct foe_entry);
	if (hnat_priv->foe_table_cpu[ppe_id])
		dma_free_coherent(hnat_priv->dev, foe_table_sz,
				  hnat_priv->foe_table_cpu[ppe_id],
				  hnat_priv->foe_table_dev[ppe_id]);
	hnat_priv->foe_table_cpu[ppe_id] = NULL;
	writel(0, hnat_priv->ppe_base[ppe_id] + PPE_TB_BASE);

	if (hnat_priv->data->per_flow_accounting) {
		foe_mib_tb_sz = hnat_priv->foe_etry_num * sizeof(struct mib_entry);
		if (hnat_priv->foe_mib_cpu[ppe_id])
			dma_free_coherent(hnat_priv->dev, foe_mib_tb_sz,
					  hnat_priv->foe_mib_cpu[ppe_id],
					  hnat_priv->foe_mib_dev[ppe_id]);
		writel(0, hnat_priv->ppe_base[ppe_id] + PPE_MIB_TB_BASE);
		kfree(hnat_priv->acct[ppe_id]);
	}

	/* Release the allocated hnat_flow_entry nodes */
	if (hnat_priv->foe_flow[ppe_id])
		hnat_flow_entry_teardown_all(ppe_id);
}

static void hnat_release_netdev(void)
{
	int i;
	struct extdev_entry *ext_entry;

	for (i = 0; i < MAX_EXT_DEVS && hnat_priv->ext_if[i]; i++) {
		ext_entry = hnat_priv->ext_if[i];
		if (ext_entry->dev)
			dev_put(ext_entry->dev);
		ext_if_del(ext_entry);
		kfree(ext_entry);
	}

	if (hnat_priv->g_ppdev)
		dev_put(hnat_priv->g_ppdev);

	if (hnat_priv->g_wandev)
		dev_put(hnat_priv->g_wandev);
}

static struct notifier_block nf_hnat_netdevice_nb __read_mostly = {
	.notifier_call = nf_hnat_netdevice_event,
};

static struct notifier_block nf_hnat_netevent_nb __read_mostly = {
	.notifier_call = nf_hnat_netevent_handler,
};

int hnat_enable_hook(void)
{
	/* register hook functions used by WHNAT module.
	 */
	if (hnat_priv->data->whnat) {
		ra_sw_nat_hook_rx =
			(hnat_priv->data->version == MTK_HNAT_V2 ||
			 hnat_priv->data->version == MTK_HNAT_V3) ?
			 mtk_sw_nat_hook_rx : NULL;
		ra_sw_nat_hook_tx = mtk_sw_nat_hook_tx;
		ppe_dev_register_hook = mtk_ppe_dev_register_hook;
		ppe_dev_unregister_hook = mtk_ppe_dev_unregister_hook;
		ra_sw_nat_clear_bind_entries = foe_clear_all_bind_entries;
		hnat_get_wdma_tx_port = mtk_get_wdma_tx_port;
		hnat_get_wdma_rx_port = mtk_get_wdma_rx_port;
		hnat_set_wdma_pse_port_state = mtk_set_wdma_pse_port_state;
	}
	eth_hook_tx = mtk_hnat_eth_hook_tx;

	if (hnat_register_nf_hooks())
		return -1;

	ppe_del_entry_by_mac = entry_delete_by_mac;
	ppe_del_entry_by_ip = entry_delete_by_ip;
	ppe_del_entry_by_bssid_wcid = entry_delete_by_bssid_wcid;
	hook_toggle = 1;

	return 0;
}

int hnat_disable_hook(void)
{
	int i, hash_index;
	struct foe_entry *entry;

	ra_sw_nat_hook_tx = NULL;
	ra_sw_nat_hook_rx = NULL;
	ra_sw_nat_clear_bind_entries = NULL;
	hnat_get_wdma_tx_port = NULL;
	hnat_get_wdma_rx_port = NULL;
	hnat_set_wdma_pse_port_state = NULL;
	eth_hook_tx = NULL;
	hnat_unregister_nf_hooks();

	for (i = 0; i < CFG_PPE_NUM; i++) {
		cr_set_field(hnat_priv->ppe_base[i] + PPE_TB_CFG,
			     SMA, SMA_ONLY_FWD_CPU);

		for (hash_index = 0; hash_index < hnat_priv->foe_etry_num; hash_index++) {
			entry = hnat_priv->foe_table_cpu[i] + hash_index;
			if (entry->bfib1.state == BIND) {
				entry->ipv4_hnapt.udib1.state = INVALID;
				entry->ipv4_hnapt.udib1.time_stamp =
					readl((hnat_priv->fe_base + 0x0010)) & 0xFF;
			}
		}
	}

	/* clear HWNAT cache */
	hnat_cache_ebl(1);

	mod_timer(&hnat_priv->hnat_sma_build_entry_timer, jiffies + 3 * HZ);
	ppe_del_entry_by_mac = NULL;
	ppe_del_entry_by_ip = NULL;
	ppe_del_entry_by_bssid_wcid = NULL;
	hook_toggle = 0;

	return 0;
}

int hnat_warm_init(void)
{
	u32 foe_table_sz, foe_mib_tb_sz, ppe_id = 0;
	int i;

	unregister_netevent_notifier(&nf_hnat_netevent_nb);

	for (ppe_id = 0; ppe_id < CFG_PPE_NUM; ppe_id++) {
		foe_table_sz =
			hnat_priv->foe_etry_num * sizeof(struct foe_entry);
		writel(hnat_priv->foe_table_dev[ppe_id],
		       hnat_priv->ppe_base[ppe_id] + PPE_TB_BASE);
		memset(hnat_priv->foe_table_cpu[ppe_id], 0, foe_table_sz);

		if (hnat_priv->data->version == MTK_HNAT_V1_1)
			exclude_boundary_entry(hnat_priv->foe_table_cpu[ppe_id]);

		if (hnat_priv->data->per_flow_accounting) {
			foe_mib_tb_sz =
				hnat_priv->foe_etry_num * sizeof(struct mib_entry);
			writel(hnat_priv->foe_mib_dev[ppe_id],
			       hnat_priv->ppe_base[ppe_id] + PPE_MIB_TB_BASE);
			memset(hnat_priv->foe_mib_cpu[ppe_id], 0,
			       foe_mib_tb_sz);
		}

		hnat_hw_init(ppe_id);
	}

	/* The SER will enable all the PPE ports again,
	 * so we have to manually disable the unused ports one more.
	 */
	for (i = 0; i < MAX_PPE_NUM; i++) {
		if (i >= CFG_PPE_NUM)
			mtk_set_ppe_pse_port_state(i, false);
	}

	set_gmac_ppe_fwd(NR_GMAC1_PORT, 1);
	set_gmac_ppe_fwd(NR_GMAC2_PORT, 1);
	set_gmac_ppe_fwd(NR_GMAC3_PORT, 1);
	register_netevent_notifier(&nf_hnat_netevent_nb);

	return 0;
}

static struct packet_type mtk_pack_type __read_mostly = {
	.type   = HQOS_MAGIC_TAG,
	.func   = mtk_hqos_ptype_cb,
};

static int hnat_probe(struct platform_device *pdev)
{
	int i;
	int err = 0;
	int index = 0;
	struct resource *res;
	const char *name;
	struct device_node *np;
	unsigned int val;
	struct property *prop;
	struct extdev_entry *ext_entry;
	const struct of_device_id *match;

	hnat_priv = devm_kzalloc(&pdev->dev, sizeof(struct mtk_hnat), GFP_KERNEL);
	if (!hnat_priv) {
		err = -ENOMEM;
		goto err_out2;
	}

	hnat_priv->foe_etry_num = DEF_ETRY_NUM;

	match = of_match_device(of_hnat_match, &pdev->dev);
	if (unlikely(!match)) {
		err = -EINVAL;
		goto err_out2;
	}

	hnat_priv->data = (struct mtk_hnat_data *)match->data;

	hnat_priv->dev = &pdev->dev;
	np = hnat_priv->dev->of_node;

	err = of_property_read_string(np, "mtketh-wan", &name);
	if (err < 0) {
		err = -EINVAL;
		goto err_out2;
	}

	strncpy(hnat_priv->wan, (char *)name, IFNAMSIZ - 1);
	dev_info(&pdev->dev, "wan = %s\n", hnat_priv->wan);

	err = of_property_read_string(np, "mtketh-lan", &name);
	if (err < 0)
		strncpy(hnat_priv->lan, "eth0", IFNAMSIZ);
	else
		strncpy(hnat_priv->lan, (char *)name, IFNAMSIZ - 1);
	dev_info(&pdev->dev, "lan = %s\n", hnat_priv->lan);

	err = of_property_read_string(np, "mtketh-lan2", &name);
	if (err < 0)
		strncpy(hnat_priv->lan2, "NONE", IFNAMSIZ);
	else
		strncpy(hnat_priv->lan2, (char *)name, IFNAMSIZ - 1);
	dev_info(&pdev->dev, "lan2 = %s\n", hnat_priv->lan2);

	err = of_property_read_string(np, "mtketh-ppd", &name);
	if (err < 0)
		strncpy(hnat_priv->ppd, "eth0", IFNAMSIZ);
	else
		strncpy(hnat_priv->ppd, (char *)name, IFNAMSIZ - 1);
	dev_info(&pdev->dev, "ppd = %s\n", hnat_priv->ppd);

	/*get total gmac num in hnat*/
	err = of_property_read_u32_index(np, "mtketh-max-gmac", 0, &val);

	if (err < 0) {
		err = -EINVAL;
		goto err_out2;
	}

	hnat_priv->gmac_num = val;

	dev_info(&pdev->dev, "gmac num = %d\n", hnat_priv->gmac_num);

	err = of_property_read_u32_index(np, "mtkdsa-wan-port", 0, &val);

	if (err < 0) {
		hnat_priv->wan_dsa_port = NONE_DSA_PORT;
	} else {
		hnat_priv->wan_dsa_port = val;
		dev_info(&pdev->dev, "wan dsa port = %d\n", hnat_priv->wan_dsa_port);
	}

	err = of_property_read_u32_index(np, "mtketh-ppe-num", 0, &val);

	if (err < 0)
		hnat_priv->ppe_num = 1;
	else
		hnat_priv->ppe_num = val;

	dev_info(&pdev->dev, "ppe num = %d\n", hnat_priv->ppe_num);

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res) {
		err = -ENOENT;
		goto err_out2;
	}

	hnat_priv->fe_base = devm_ioremap_nocache(&pdev->dev, res->start,
					     res->end - res->start + 1);
	if (!hnat_priv->fe_base) {
		err = -EADDRNOTAVAIL;
		goto err_out2;
	}

#if defined(CONFIG_MEDIATEK_NETSYS_V2) || defined(CONFIG_MEDIATEK_NETSYS_V3)
	hnat_priv->ppe_base[0] = hnat_priv->fe_base + 0x2200;

	if (CFG_PPE_NUM > 1)
		hnat_priv->ppe_base[1] = hnat_priv->fe_base + 0x2600;

	if (CFG_PPE_NUM > 2)
		hnat_priv->ppe_base[2] = hnat_priv->fe_base + 0x2e00;
#else
	hnat_priv->ppe_base[0] = hnat_priv->fe_base + 0xe00;
#endif

	err = hnat_init_debugfs(hnat_priv);
	if (err)
		goto err_out2;

	prop = of_find_property(np, "ext-devices", NULL);
	for (name = of_prop_next_string(prop, NULL); name;
	     name = of_prop_next_string(prop, name), index++) {
		ext_entry = kzalloc(sizeof(*ext_entry), GFP_KERNEL);
		if (!ext_entry) {
			err = -ENOMEM;
			goto err_out1;
		}
		strncpy(ext_entry->name, (char *)name, IFNAMSIZ - 1);
		ext_if_add(ext_entry);
	}

	for (i = 0; i < MAX_EXT_DEVS && hnat_priv->ext_if[i]; i++) {
		ext_entry = hnat_priv->ext_if[i];
		dev_info(&pdev->dev, "ext devices = %s\n", ext_entry->name);
	}

	hnat_priv->lvid = 1;
	hnat_priv->wvid = 2;

	for (i = 0; i < CFG_PPE_NUM; i++) {
		err = hnat_start(i);
		if (err)
			goto err_out;
	}

	/* The PSE default enables all the PPE ports,
	 * so we need to manually disable the unused ports.
	 */
	for (i = 0; i < MAX_PPE_NUM; i++) {
		if (i >= CFG_PPE_NUM)
			mtk_set_ppe_pse_port_state(i, false);
		else
			mtk_set_ppe_pse_port_state(i, true);
	}

	if (hnat_priv->data->whnat) {
		err = whnat_adjust_nf_hooks();
		if (err)
			goto err_out;
	}

	err = hnat_enable_hook();
	if (err)
		goto err_out;

	register_netdevice_notifier(&nf_hnat_netdevice_nb);
	register_netevent_notifier(&nf_hnat_netevent_nb);

	if (hnat_priv->data->mcast) {
		for (i = 0; i < CFG_PPE_NUM; i++)
			hnat_mcast_enable(i);
	}

	timer_setup(&hnat_priv->hnat_sma_build_entry_timer, hnat_sma_build_entry, 0);
	if (hnat_priv->data->version == MTK_HNAT_V1_3) {
		timer_setup(&hnat_priv->hnat_reset_timestamp_timer, hnat_reset_timestamp, 0);
		hnat_priv->hnat_reset_timestamp_timer.expires = jiffies;
		add_timer(&hnat_priv->hnat_reset_timestamp_timer);
	}

	if (IS_HQOS_MODE && IS_GMAC1_MODE)
		dev_add_pack(&mtk_pack_type);

	err = hnat_roaming_enable();
	if (err)
		pr_info("hnat roaming work fail\n");

	hnat_flow_entry_teardown_enable();

	INIT_LIST_HEAD(&hnat_priv->xlat.map_list);

	return 0;

err_out:
	for (i = 0; i < CFG_PPE_NUM; i++)
		hnat_stop(i);
err_out1:
	hnat_deinit_debugfs(hnat_priv);
	for (i = 0; i < MAX_EXT_DEVS && hnat_priv->ext_if[i]; i++) {
		ext_entry = hnat_priv->ext_if[i];
		ext_if_del(ext_entry);
		kfree(ext_entry);
	}
err_out2:
	for (i = 0; i < MAX_PPE_NUM; i++)
		mtk_set_ppe_pse_port_state(i, false);
	return err;
}

static int hnat_remove(struct platform_device *pdev)
{
	int i;

	hnat_roaming_disable();
	hnat_flow_entry_teardown_disable();
	unregister_netdevice_notifier(&nf_hnat_netdevice_nb);
	unregister_netevent_notifier(&nf_hnat_netevent_nb);
	hnat_disable_hook();

	if (hnat_priv->data->mcast)
		hnat_mcast_disable();

	for (i = 0; i < CFG_PPE_NUM; i++)
		hnat_stop(i);

	for (i = 0; i < MAX_PPE_NUM; i++)
		mtk_set_ppe_pse_port_state(i, false);

	hnat_deinit_debugfs(hnat_priv);
	hnat_release_netdev();
	del_timer_sync(&hnat_priv->hnat_sma_build_entry_timer);
	if (hnat_priv->data->version == MTK_HNAT_V1_3)
		del_timer_sync(&hnat_priv->hnat_reset_timestamp_timer);

	if (IS_HQOS_MODE && IS_GMAC1_MODE)
		dev_remove_pack(&mtk_pack_type);

	return 0;
}

static const struct mtk_hnat_data hnat_data_v1 = {
	.num_of_sch = 2,
	.whnat = false,
	.per_flow_accounting = false,
	.mcast = false,
	.version = MTK_HNAT_V1_1,
};

static const struct mtk_hnat_data hnat_data_v2 = {
	.num_of_sch = 2,
	.whnat = true,
	.per_flow_accounting = true,
	.mcast = false,
	.version = MTK_HNAT_V1_2,
};

static const struct mtk_hnat_data hnat_data_v3 = {
	.num_of_sch = 4,
	.whnat = false,
	.per_flow_accounting = false,
	.mcast = false,
	.version = MTK_HNAT_V1_3,
};

static const struct mtk_hnat_data hnat_data_v4 = {
	.num_of_sch = 4,
	.whnat = true,
	.per_flow_accounting = true,
	.mcast = false,
	.version = MTK_HNAT_V2,
};

static const struct mtk_hnat_data hnat_data_v5 = {
	.num_of_sch = 4,
	.whnat = true,
	.per_flow_accounting = true,
	.mcast = false,
	.version = MTK_HNAT_V3,
};

const struct of_device_id of_hnat_match[] = {
	{ .compatible = "mediatek,mtk-hnat", .data = &hnat_data_v3 },
	{ .compatible = "mediatek,mtk-hnat_v1", .data = &hnat_data_v1 },
	{ .compatible = "mediatek,mtk-hnat_v2", .data = &hnat_data_v2 },
	{ .compatible = "mediatek,mtk-hnat_v3", .data = &hnat_data_v3 },
	{ .compatible = "mediatek,mtk-hnat_v4", .data = &hnat_data_v4 },
	{ .compatible = "mediatek,mtk-hnat_v5", .data = &hnat_data_v5 },
	{},
};
MODULE_DEVICE_TABLE(of, of_hnat_match);

static struct platform_driver hnat_driver = {
	.probe = hnat_probe,
	.remove = hnat_remove,
	.driver = {
		.name = "mediatek_soc_hnat",
		.of_match_table = of_hnat_match,
	},
};

module_platform_driver(hnat_driver);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Sean Wang <sean.wang@mediatek.com>");
MODULE_AUTHOR("John Crispin <john@phrozen.org>");
MODULE_DESCRIPTION("Mediatek Hardware NAT");
