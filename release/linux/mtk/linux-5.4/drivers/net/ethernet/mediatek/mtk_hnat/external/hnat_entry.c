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

#include "../hnat.h"
#include "hnat_entry.h"

static inline int ppe_id_get_by_sport(u32 sp)
{
        if (CFG_PPE_NUM == 3) {
                switch (sp) {
                case NR_GMAC1_PORT:
                        return 0;
                case NR_GMAC2_PORT:
                        return 1;
                case NR_GMAC3_PORT:
                        return 2;
                }
        }

        return 0;
}

void hnat_entry_add(struct foe_entry *foe)
{
        struct foe_entry *dst_foe;
        u32 ppe_id;
        int hash;
	int i;

        hash = hnat_get_ppe_hash(foe);
        ppe_id = ppe_id_get_by_sport(foe->ipv4_hnapt.bfib1.sp);

	/* collision policy: at most 4 entries with the same hash value */
	for (i = 0; i < 4; i++) {
		dst_foe = hnat_get_foe_entry(ppe_id,
					     ((hash + i) % hnat_priv->foe_etry_num));

	        if ((IS_ERR(dst_foe)))
	                return;

		if (dst_foe->ipv4_hnapt.bfib1.state == BIND)
			continue;

		break;
	}

        /* We must ensure all info has been updated before set to hw */
        wmb();
        memcpy(dst_foe, foe, sizeof(*foe));
}
EXPORT_SYMBOL(hnat_entry_add);
