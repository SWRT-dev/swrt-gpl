/*   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; version 2 of the License
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   Copyright (C) 2024 Frank-zj Lin <frank-zj.lin@mediatek.com>
 */
#include <linux/bits.h>

#include "../hnat_entry.h"

/* Initialize with default values of a statically bind entry */
static int hnat_bind_info_blk_init(struct hnat_bind_info_blk* blk)
{
	if (!blk)
		return -EINVAL;

	blk->pkt_type	= IPV4_HNAPT;
	blk->cah	= 0x1;
	blk->ttl	= 0x1;
	blk->state	= 0x2; /* bind */
	blk->sta	= 0x1; /* static entry */

	return 0;
}

/* PSE source port 0~15 */
static int hnat_bind_info_blk_sp_set(struct hnat_bind_info_blk* blk, u32 val)
{
	u32 _val = val & GENMASK(3, 0);

	if (!blk || _val != val)
		return -EINVAL;

	blk->sp = _val;

	return 0;
}

/* Initialize with default values of a statically bind entry */
static int hnat_info_blk2_init(struct hnat_info_blk2* blk)
{
	if (!blk)
		return -EINVAL;

	blk->mibf	= 0x1;
	blk->port_ag	= 0xF;

	return 0;
}

static int hnat_info_blk2_qid_set(struct hnat_info_blk2* blk, u32 val)
{
	u32 _val = val & GENMASK(6, 0);

	if (!blk || _val != val)
		return -EINVAL;

	blk->qid = _val;

	return 0;
}

/* PSE final port 0~15 */
static int hnat_info_blk2_fp_set(struct hnat_info_blk2* blk, u32 val)
{
	u32 _val = val & GENMASK(3, 0);

	if (!blk || _val != val)
		return -EINVAL;

	blk->dp = _val;

	return 0;
}

int hnat_ipv4_hnapt_init(struct hnat_ipv4_hnapt *entry)
{
	if (!entry)
		return -EINVAL;

	hnat_bind_info_blk_init(&entry->bfib1);
	hnat_info_blk2_init(&entry->iblk2);

	return 0;
}
EXPORT_SYMBOL(hnat_ipv4_hnapt_init);

int hnat_ipv4_hnapt_sp_set(struct hnat_ipv4_hnapt *entry, u32 val)
{
	return hnat_bind_info_blk_sp_set(&entry->bfib1, val);
}
EXPORT_SYMBOL(hnat_ipv4_hnapt_sp_set);

int hnat_ipv4_hnapt_qid_set(struct hnat_ipv4_hnapt *entry, u32 val)
{
	return hnat_info_blk2_qid_set(&entry->iblk2, val);
}
EXPORT_SYMBOL(hnat_ipv4_hnapt_qid_set);

int hnat_ipv4_hnapt_fp_set(struct hnat_ipv4_hnapt *entry, u32 val)
{
	return hnat_info_blk2_fp_set(&entry->iblk2, val);
}
EXPORT_SYMBOL(hnat_ipv4_hnapt_fp_set);

int hnat_ipv4_hnapt_sip_set(struct hnat_ipv4_hnapt *entry, __be32 val)
{
	if (!entry)
		return -EINVAL;

	entry->sip = ntohl(val);

	return 0;
}
EXPORT_SYMBOL(hnat_ipv4_hnapt_sip_set);

int hnat_ipv4_hnapt_dip_set(struct hnat_ipv4_hnapt *entry, __be32 val)
{
	if (!entry)
		return -EINVAL;

	entry->dip = ntohl(val);

	return 0;
}
EXPORT_SYMBOL(hnat_ipv4_hnapt_dip_set);

int hnat_ipv4_hnapt_dport_set(struct hnat_ipv4_hnapt *entry, u16 val)
{
	if (!entry)
		return -EINVAL;

	entry->dport = val;

	return 0;
}
EXPORT_SYMBOL(hnat_ipv4_hnapt_dport_set);

int hnat_ipv4_hnapt_sport_set(struct hnat_ipv4_hnapt *entry, u16 val)
{
	if (!entry)
		return -EINVAL;

	entry->sport = val;

	return 0;
}
EXPORT_SYMBOL(hnat_ipv4_hnapt_sport_set);

int hnat_ipv4_hnapt_new_sip_set(struct hnat_ipv4_hnapt *entry, __be32 val)
{
	if (!entry)
		return -EINVAL;

	entry->new_sip = ntohl(val);

	return 0;
}
EXPORT_SYMBOL(hnat_ipv4_hnapt_new_sip_set);

int hnat_ipv4_hnapt_new_dip_set(struct hnat_ipv4_hnapt *entry, __be32 val)
{
	if (!entry)
		return -EINVAL;

	entry->new_dip = ntohl(val);

	return 0;
}
EXPORT_SYMBOL(hnat_ipv4_hnapt_new_dip_set);

int hnat_ipv4_hnapt_new_dport_set(struct hnat_ipv4_hnapt *entry, u16 val)
{
	if (!entry)
		return -EINVAL;

	entry->new_dport = val;

	return 0;
}
EXPORT_SYMBOL(hnat_ipv4_hnapt_new_dport_set);

int hnat_ipv4_hnapt_new_sport_set(struct hnat_ipv4_hnapt *entry, u16 val)
{
	if (!entry)
		return -EINVAL;

	entry->new_sport = val;

	return 0;
}
EXPORT_SYMBOL(hnat_ipv4_hnapt_new_sport_set);

int hnat_ipv4_hnapt_cdrt_id_set(struct hnat_ipv4_hnapt *entry, u32 val)
{
	u32 _val = val & GENMASK(7, 0);

	if (!entry || _val != val)
		return -EINVAL;

	entry->cdrt_id = _val;

	return 0;
}
EXPORT_SYMBOL(hnat_ipv4_hnapt_cdrt_id_set);

u32 hnat_ipv4_hnapt_tops_entry_get(struct hnat_ipv4_hnapt *entry)
{
	return entry->tops_entry;
}
EXPORT_SYMBOL(hnat_ipv4_hnapt_tops_entry_get);

int hnat_ipv4_hnapt_tops_entry_set(struct hnat_ipv4_hnapt *entry, u32 val)
{
	u32 _val = val & GENMASK(5, 0);

	if (!entry || _val != val)
		return -EINVAL;

	entry->tops_entry = _val;

	return 0;
}
EXPORT_SYMBOL(hnat_ipv4_hnapt_tops_entry_set);

u32 hnat_ipv4_hnapt_tport_id_get(struct hnat_ipv4_hnapt *entry)
{
	return entry->tport_id;
}
EXPORT_SYMBOL(hnat_ipv4_hnapt_tport_id_get);

int hnat_ipv4_hnapt_tport_id_set(struct hnat_ipv4_hnapt *entry, u32 val)
{
	u32 _val = val & GENMASK(3, 0);

	if (!entry || _val != val)
		return -EINVAL;

	entry->tport_id = _val;

	return 0;
}
EXPORT_SYMBOL(hnat_ipv4_hnapt_tport_id_set);

int hnat_ipv4_hnapt_dmac_set(struct hnat_ipv4_hnapt *entry, u8 *mac)
{
	if (!entry || !mac)
		return -EINVAL;

	entry->dmac_hi = swab32(*((u32 *)mac));
	entry->dmac_lo = swab16(*((u16 *)&mac[4]));

	return 0;
}
EXPORT_SYMBOL(hnat_ipv4_hnapt_dmac_set);

int hnat_ipv4_hnapt_smac_set(struct hnat_ipv4_hnapt *entry, u8 *mac)
{
	if (!entry || !mac)
		return -EINVAL;

	entry->smac_hi = swab32(*((u32 *)mac));
	entry->smac_lo = swab16(*((u16 *)&mac[4]));

	return 0;
}
EXPORT_SYMBOL(hnat_ipv4_hnapt_smac_set);
