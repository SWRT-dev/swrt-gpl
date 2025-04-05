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

#ifndef _MTK_HNAT_EXTERNAL_HNAT_ENTRY_H_
#define _MTK_HNAT_EXTERNAL_HNAT_ENTRY_H_

#include <linux/debugfs.h>

#include "../hnat.h"

void hnat_entry_add(struct foe_entry *foe);

/* necessary apis to access struct hnapt_ipv4_hnapt from external kernel module */
int hnat_ipv4_hnapt_init(struct hnat_ipv4_hnapt *entry);
int hnat_ipv4_hnapt_sp_set(struct hnat_ipv4_hnapt *entry, u32 val);
int hnat_ipv4_hnapt_qid_set(struct hnat_ipv4_hnapt *entry, u32 val);
int hnat_ipv4_hnapt_fp_set(struct hnat_ipv4_hnapt *entry, u32 val);
int hnat_ipv4_hnapt_sip_set(struct hnat_ipv4_hnapt *entry, __be32 val);
int hnat_ipv4_hnapt_dip_set(struct hnat_ipv4_hnapt *entry, __be32 val);
int hnat_ipv4_hnapt_dport_set(struct hnat_ipv4_hnapt *entry, u16 val);
int hnat_ipv4_hnapt_sport_set(struct hnat_ipv4_hnapt *entry, u16 val);
int hnat_ipv4_hnapt_new_sip_set(struct hnat_ipv4_hnapt *entry, __be32 val);
int hnat_ipv4_hnapt_new_dip_set(struct hnat_ipv4_hnapt *entry, __be32 val);
int hnat_ipv4_hnapt_new_dport_set(struct hnat_ipv4_hnapt *entry, u16 val);
int hnat_ipv4_hnapt_new_sport_set(struct hnat_ipv4_hnapt *entry, u16 val);
int hnat_ipv4_hnapt_dmac_set(struct hnat_ipv4_hnapt *entry, u8 *mac);
int hnat_ipv4_hnapt_smac_set(struct hnat_ipv4_hnapt *entry, u8 *mac);
#if defined(CONFIG_MEDIATEK_NETSYS_V3)
int hnat_ipv4_hnapt_cdrt_id_set(struct hnat_ipv4_hnapt *entry, u32 val);
u32 hnat_ipv4_hnapt_tops_entry_get(struct hnat_ipv4_hnapt *entry);
int hnat_ipv4_hnapt_tops_entry_set(struct hnat_ipv4_hnapt *entry, u32 val);
u32 hnat_ipv4_hnapt_tport_id_get(struct hnat_ipv4_hnapt *entry);
int hnat_ipv4_hnapt_tport_id_set(struct hnat_ipv4_hnapt *entry, u32 val);
#endif /* (CONFIG_MEDIATEK_NETSYS_V3) */

#endif /* _MTK_HNAT_EXTERNAL_HNAT_ENTRY_H_ */
