/*
 * Description: PP Parser Tables Management
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2018-2019 Intel Corporation
 */

#define pr_fmt(fmt) "[PP_PARSER_TBL]: %s:%d: " fmt, __func__, __LINE__

#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/errno.h>
#include <linux/err.h>
#include <linux/spinlock.h>
#include <linux/bitops.h>
#include <linux/bitmap.h>
#include <linux/string.h>
#include <linux/ctype.h>

#include "parser_internal.h"

/* inline functions */
/**
 * @brief Get a table entry reference
 * @param tbl table context
 * @param idx entry index
 * @return struct table_entry* entry reference on success, NULL otherwise
 */
static inline struct table_entry *__table_entry_get(struct table *tbl, u16 idx)
{
	if (unlikely(!table_entry_idx_is_valid(tbl, idx)))
		return NULL;

	return &tbl->entrs[idx];
}

const struct table_entry *table_entry_get(struct table *tbl, u16 idx)
{
	return __table_entry_get(tbl, idx);
}

bool table_entry_idx_is_valid(struct table *tbl, u16 ent_idx)
{
	if (unlikely(ptr_is_null(tbl)))
		return false;

	if (likely(ent_idx < tbl->n_ent))
		return true;

	prsr_err("Table %s: Invalid entry index %u\n", tbl->name, ent_idx);
	return false;
}

/**
 * @brief Table create helper which create a table context and
 *        initialize all table's info including all memory
 *        allocations
 * @param name table name
 * @param n_ent number of entries in the table
 * @param n_rsrv number of reserved entries in the table
 * @param rsrv_base reserved entries start index
 * @return struct table* table context on success, error code encoded
 *         as a pointer otherwise, use IS_ERR to determine if error
 *         did occur
 */
static struct table *__table_prepare(char *name, u32 n_ent,
				     u32 n_rsrv, u32 rsrv_base)
{
	struct table *tbl;
	u32 not_used;
	s32 ret;

	tbl = kzalloc(sizeof(*tbl), GFP_KERNEL);
	if (unlikely(!tbl)) {
		prsr_err("Failed to allocate table\n");
		return ERR_PTR(-ENOMEM);
	}
	/* allocate memory for the table entries */
	tbl->entrs = kcalloc(n_ent, sizeof(*tbl->entrs), GFP_KERNEL);
	if (unlikely(!tbl->entrs)) {
		prsr_err("Failed to allocate table entries memory\n");
		ret = -ENOMEM;
		goto fail;
	}
	/* allocate used entries (reserved and regular) bitmap */
	tbl->entrs_map = kcalloc(BITS_TO_LONGS(n_ent),
				 sizeof(*tbl->entrs_map),
				 GFP_KERNEL);
	if (unlikely(!tbl->entrs_map)) {
		prsr_err("Failed to allocate table used entries bitmap\n");
		ret = -ENOMEM;
		goto fail;
	}
	/* allocate used entries (reserved and regular) bitmap */
	tbl->reg.map = kcalloc(BITS_TO_LONGS(n_ent),
			       sizeof(*tbl->reg.map),
			       GFP_KERNEL);
	if (unlikely(!tbl->reg.map)) {
		prsr_err("Failed to allocate table regular entries bitmap\n");
		ret = -ENOMEM;
		goto fail;
	}
	/* allocate memory for the table reserved entries bitmap */
	tbl->rsrv.map = kcalloc(BITS_TO_LONGS(n_ent),
				sizeof(*tbl->rsrv.map),
				GFP_KERNEL);
	if (unlikely(!tbl->rsrv.map)) {
		prsr_err("Failed to allocate table reserved entries bitmap\n");
		ret = -ENOMEM;
		goto fail;
	}

	/* init table and entries info */
	spin_lock_init(&tbl->lock);
	tbl->valid       = true;
	tbl->n_ent       = n_ent;
	tbl->reg.n_free  = n_ent - n_rsrv;
	tbl->n_rsrv      = n_rsrv;
	tbl->rsrv.n_free = n_rsrv;
	not_used = strscpy(tbl->name, name ? name : "", sizeof(tbl->name));

	return tbl;
fail:
	table_destroy(tbl);
	return ERR_PTR(ret);
}

/**
 * @brief Table create helper which prepare all of the tables entries
 * @param tbl table context
 * @param rsrv_base reserved entries start index
 */
static void __entries_prepare(struct table *tbl, u32 rsrv_base)
{
	struct table_entry *ent;
	u32 i, not_used;

	/* init table entries */
	for (i = 0; i < tbl->n_ent; i++) {
		ent = &tbl->entrs[i];
		ent->idx      = i;
		ent->ent_rsrv = ((rsrv_base <= i) &&
				 (i < rsrv_base + tbl->n_rsrv));
		/* mark all reserved entries as used in the regular bitmap
		 * so they will not be allocated and mark the non-reserved
		 * entries as used in the reserved bitmap for same reason
		 */
		if (ent->ent_rsrv)
			set_bit(i, tbl->reg.map);
		else
			set_bit(i, tbl->rsrv.map);
		pr_buf(ent->name, sizeof(ent->name), not_used, " ");
	}
}

struct table *table_create(char *name, u32 n_ent, u32 n_rsrv, u32 rsrv_base)
{
	struct table *tbl;

	pr_debug("name '%s', n_ent %u, n_rsrv %u, rsrv_base %u\n",
		 name, n_ent, n_rsrv, rsrv_base);

	if (unlikely(n_ent == 0)) {
		prsr_err("cannot create table with 0 entries\n");
		return ERR_PTR(-EINVAL);
	}
	if (unlikely(n_ent < rsrv_base + n_rsrv)) {
		prsr_err("too many reserved entries\n");
		return ERR_PTR(-EINVAL);
	}
	if (unlikely(n_ent > TABLE_ENTRY_INVALID_IDX)) {
		prsr_err("table too big, max is %u\n",
			 TABLE_ENTRY_INVALID_IDX);
		return ERR_PTR(-EINVAL);
	}

	tbl = __table_prepare(name, n_ent, n_rsrv, rsrv_base);
	if (IS_ERR_OR_NULL(tbl))
		return tbl;

	__entries_prepare(tbl, rsrv_base);

	pr_debug("Table %s created: entr %p, umap %p, rumap %p\n",
		 tbl->name, tbl->entrs, tbl->reg.map, tbl->rsrv.map);

	return tbl;
}

s32 table_destroy(struct table *tbl)
{
	if (unlikely(ptr_is_null(tbl)))
		return -EINVAL;

	pr_debug("Destroying table %s, entrs %p, umap %p, regumap %p, rsrvumap %p\n",
		 tbl->name, tbl->entrs, tbl->entrs_map,
		 tbl->reg.map, tbl->rsrv.map);
	kfree(tbl->entrs);
	kfree(tbl->entrs_map);
	kfree(tbl->reg.map);
	kfree(tbl->rsrv.map);
	kfree(tbl);

	return 0;
}

s32 table_alloc_entries(struct table *tbl, const char *name, const void *data,
			u16 cnt, u16 *ent_idx, bool rsrv)
{
	unsigned long *bitmap, start;
	struct table_entry *ent;
	u32 i, not_used, *used, *free;
	bool static_alloc;
	s32 ret;

	pr_debug("Table %s, name %s, n %u, rsrv %u, ent_idx %u\n",
		 (tbl ? tbl->name : "null"), name, cnt, rsrv, *ent_idx);

	if (unlikely(!(!ptr_is_null(tbl) && !ptr_is_null(ent_idx))))
		return -EINVAL;

	static_alloc = *ent_idx != TABLE_ENTRY_INVALID_IDX;
	spin_lock_bh(&tbl->lock);
	/* if the caller ask for specific indexes, make sure they are valid */
	if ((static_alloc) &&
	    unlikely(!table_entry_idx_is_valid(tbl, *ent_idx + cnt - 1))) {
		ret = -EINVAL;
		goto fail;
	}

	/* choose allocation type */
	bitmap = rsrv ?  tbl->rsrv.map    :  tbl->reg.map;
	used   = rsrv ? &tbl->rsrv.n_used : &tbl->reg.n_used;
	free   = rsrv ? &tbl->rsrv.n_free : &tbl->reg.n_free;
	/* set starting index */
	start  = static_alloc ? *ent_idx : 0;

	/* try to allocate 'n' consequence free entries starting from 'start' */
	i = bitmap_find_next_zero_area(bitmap, tbl->n_ent, start, cnt, 0);

	/* if the first entry found is not what the caller asked for than
	 * the range he asked for isn't free
	 */
	if (static_alloc && i != *ent_idx) {
		pr_debug("%s: entries %u-%u are used\n",
			 tbl->name, *ent_idx, *ent_idx + cnt);
		ret = -EEXIST;
		goto fail;
	} else if (tbl->n_ent <= i) {
		prsr_err("Table %s: no %u consequent free%s entries\n",
			 tbl->name, cnt, rsrv ? " reserved" : "");
		ret = -ENOSPC;
		goto fail;
	}
	pr_debug("%u %s entries found at index %u\n",
		 cnt, rsrv ? " reserved" : "", i);
	/* found, mark entries as used */
	bitmap_set(bitmap, i, cnt);         /* mark in the specific bitmap */
	bitmap_set(tbl->entrs_map, i, cnt); /* mark in all entries bitmap  */
	/* update table counters */
	*used += cnt;
	*free -= cnt;
	/* set returned index */
	*ent_idx = i;
	/* init all entries */
	for (; i < *ent_idx + cnt; i++) {
		ent = &tbl->entrs[i];
		if (IS_ENABLED(CONFIG_DEBUG_FS) && name)
			not_used = strscpy(ent->name, name, sizeof(ent->name));

		ent->used    = true;
		ent->data    = data;
		ent->ref_cnt = 1;
		if (ent->ent_rsrv != rsrv)
			prsr_err("Allocated wrong entries type, rsrv %u, entry id %u rsrv %u\n",
				 rsrv, i, ent->ent_rsrv);
	}

	spin_unlock_bh(&tbl->lock);
	pr_debug("Table %s:%s indexes %u-%u allocated, n_used %u, n_free %u\n",
		 tbl->name, rsrv ? " reserved" : "", *ent_idx,
		 *ent_idx + cnt - 1, *used, *free);

	return 0;
fail:
	spin_unlock_bh(&tbl->lock);
	return ret;
}

s32 table_free_entry(struct table *tbl, u16 idx)
{
	struct table_entry *ent;

	pr_debug("Table %s: idx %u\n",
		 (tbl ? tbl->name : "null"), idx);

	if (unlikely(ptr_is_null(tbl)))
		return -EINVAL;

	spin_lock_bh(&tbl->lock);
	/* free entries */
	ent = __table_entry_get(tbl, idx);
	if (unlikely(!ent)) {
		spin_unlock_bh(&tbl->lock);
		return -EINVAL;
	}
	if (unlikely(!test_bit(idx, tbl->entrs_map))) {
		prsr_err("entry %u doesn't exist\n", idx);
		return -ENOENT;
	}
	/* if ref count > 0, nothing to do */
	if (--ent->ref_cnt) {
		spin_unlock_bh(&tbl->lock);
		return -EEXIST;
	}
	ent->used = false;
	ent->data = NULL;
	ent->name[0] = '\0';
	if (ent->ent_rsrv) {
		clear_bit(idx, tbl->rsrv.map);
		tbl->rsrv.n_free++;
		tbl->rsrv.n_used++;
	} else {
		clear_bit(idx, tbl->reg.map);
		tbl->reg.n_free++;
		tbl->reg.n_used++;
	}
	clear_bit(idx, tbl->entrs_map);

	spin_unlock_bh(&tbl->lock);
	pr_debug("Table %s: index %u freed, n_used %u, n_free %u, rsrv.n_used %u, reg.n_free %u\n",
		 tbl->name, idx, tbl->reg.n_used, tbl->reg.n_free,
		 tbl->rsrv.n_used, tbl->rsrv.n_free);

	return 0;
}

s32 table_entry_ref_inc(struct table *tbl, u16 ent_idx)
{
	struct table_entry *ent;
	s32 ref_cnt;

	ent = __table_entry_get(tbl, ent_idx);
	if (unlikely(!ent))
		return -EINVAL;

	spin_lock_bh(&tbl->lock);
	ref_cnt = (s32)(++ent->ref_cnt);
	spin_unlock_bh(&tbl->lock);

	return ref_cnt;
}

s32 table_entry_ref_dec(struct table *tbl, u16 ent_idx)
{
	struct table_entry *ent;
	s32 ref_cnt;

	ent = __table_entry_get(tbl, ent_idx);
	if (unlikely(!ent))
		return -EINVAL;

	spin_lock_bh(&tbl->lock);
	ref_cnt = (s32)(--ent->ref_cnt);
	spin_unlock_bh(&tbl->lock);

	return ref_cnt;
}

const char *table_entry_name_get(struct table *tbl, u16 ent_idx)
{
	struct table_entry *ent = __table_entry_get(tbl, ent_idx);

	if (unlikely(!ent))
		return NULL;
	return ent->name;
}

#ifdef CONFIG_DEBUG_FS
s32 table_entry_name_append(struct table *tbl, u16 idx, const char *name,
			    char *sep, bool before)
{
	struct table_entry *ent = __table_entry_get(tbl, idx);
	char new_name[PRSR_MAX_NAME];
	s32 n;

	if (unlikely(!name))
		return 0;

	if (unlikely(!tbl || !ent))
		return -EINVAL;

	spin_lock_bh(&tbl->lock);
	if (before) {
		/* first, add the new name we got from the user*/
		n = strscpy(new_name, name, sizeof(new_name));
		/* concatenate the separator */
		n = strlcat(new_name, sep, sizeof(new_name));
		/* concatenate the entry name */
		n = strlcat(new_name, ent->name, sizeof(new_name));
		/* set the new name as the entry name */
		n = strscpy(ent->name, new_name, sizeof(ent->name));
	} else {
		n = strlcat(ent->name, sep,  sizeof(ent->name));
		n = strlcat(ent->name, name, sizeof(ent->name));
	}
	spin_unlock_bh(&tbl->lock);
	return 0;
}

s32 table_entry_name_remove(struct table *tbl, u16 idx, const char *name)
{
	struct table_entry *ent = __table_entry_get(tbl, idx);
	char *pos, *leftovers;
	s32 n;

	if (!name)
		return 0;

	if (!tbl || !ent)
		return -EINVAL;

	spin_lock_bh(&tbl->lock);
	/* find name in the entry name */
	pos = strnstr(ent->name, name, strlen(name));
	if (!pos) {
		/* name doesn't exist, nothing todo */
		goto done;
	}

	/* all that left after name */
	leftovers = pos + strlen(name);

	/* name found at pos, leftovers is all that left after name */
	/* overwrite name in entry name with the leftovers */
	n = strscpy(pos, leftovers, sizeof(ent->name));

done:
	spin_unlock_bh(&tbl->lock);
	return 0;
}
#endif

/**
 * @brief Get table's next free or used entry
 * @param tbl table context
 * @param off offset to start the search from
 * @param used specify if to get the next free or used entry
 * @return u16 next free or used entry on success, number of table
 *         entries in case such entry doesn't exist or invalid index
 *         in case 'tbl' is null
 */
static u16 __table_get_next_entry(struct table *tbl, u16 off, bool used)
{
	u16 idx;

	if (unlikely(ptr_is_null(tbl)))
		return TABLE_ENTRY_INVALID_IDX;

	spin_lock_bh(&tbl->lock);
	/* used or free entry */
	if (used)
		idx = find_next_bit(tbl->entrs_map, tbl->n_ent, off);
	else
		idx = find_next_zero_bit(tbl->entrs_map, tbl->n_ent, off);
	spin_unlock_bh(&tbl->lock);

	return idx;
}

u16 table_get_first_entry(struct table *tbl)
{
	return __table_get_next_entry(tbl, 0, true);
}

u16 table_get_next_entry(struct table *tbl, u16 off)
{
	return __table_get_next_entry(tbl, off, true);
}

u16 table_get_first_free_entry(struct table *tbl)
{
	return __table_get_next_entry(tbl, 0, false);
}

u16 table_get_next_free_entry(struct table *tbl, u16 off)
{
	return __table_get_next_entry(tbl, off, false);
}

