/*
 * Description: PP Parser Driver
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2018-2020 Intel Corporation
 */

#define pr_fmt(fmt) "[PP_PARSER]: %s:%d: " fmt, __func__, __LINE__

#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/errno.h>
#include <linux/spinlock.h>
#include <linux/bitops.h>
#include <linux/bitmap.h>
#include <linux/string.h>
#include <linux/pp_api.h>

#include "parser_protocols.h"
#include "pp_fv.h"
#include "parser.h"
#include "parser_internal.h"
#include "infra.h"

/* local struct/enum */
/**
 * @brief Parser database definition
 * @lock lock to use mainly when writing to hw registers,
 *       all tables entries allocation locking is done by
 *       the tables manager driver
 * @protos array of all protocols which are currently in use
 * @rsrv_protos_map reserved protocols bitmap which specify
 *                  which is used and which is not, reserved
 *                  protocols are used for dynamic configuration
 *                  of protocols
 * @X_tbl reference to a table create by the tables manager driver
 */
struct prsr_db {
	spinlock_t        lock;
	struct proto_db_e protos[PRSR_PROTOS_NUM];
	unsigned long    *rsrv_protos_map;
	struct table     *l2_tbl;
	struct table     *last_eth_tbl;
	struct table     *imem_tbl;
	struct table     *np_sel_tbl;
	struct table     *skip_tbl;
	struct table     *merge_tbl;
	struct table     *pit_tbl;
};

/* tables APIs shortcuts */
/**
 * @brief Test if an index is a valid layer 2 table index
 * @param idx index to test
 */
#define __prsr_l2_index_is_valid(idx) \
	table_entry_idx_is_valid(db->l2_tbl, idx)
/**
 * @brief Test if an index is a valid last ethertype table index
 * @param idx index to test
 */
#define __prsr_last_ethertype_index_is_valid(idx) \
	table_entry_idx_is_valid(db->last_eth_tbl, idx)
/**
 * @brief Test if an index is a valid imem table index
 * @param idx index to test
 */
#define __prsr_imem_index_is_valid(idx) \
	table_entry_idx_is_valid(db->imem_tbl, idx)
/**
 * @brief Test if an index is a valid np select table index
 * @param idx index to test
 */
#define __prsr_np_sel_index_is_valid(idx) \
	table_entry_idx_is_valid(db->np_sel_tbl, idx)
/**
 * @brief Test if an index is a valid skip table index
 * @param idx index to test
 */
#define __prsr_skip_index_is_valid(idx) \
	table_entry_idx_is_valid(db->skip_tbl, idx)
/**
 * @brief Test if an index is a valid merge table index
 * @param idx index to test
 */
#define __prsr_merge_index_is_valid(idx) \
	table_entry_idx_is_valid(db->merge_tbl, idx)
/**
 * @brief Test if an index is a valid pit table index
 * @param idx index to test
 */
#define __prsr_pit_index_is_valid(idx) \
	table_entry_idx_is_valid(db->pit_tbl, idx)

/* global definitions */
u64 prsr_base_addr;
static struct prsr_db *db;
static struct prsr_stats rstats; /* saved stats from last reset request */

/* ========================================================================== *
 *                      Misc Functions                                        *
 * ========================================================================== *
 */
/**
 * @brief Protocol internal ID validity test
 * @param id internal id
 * @return bool true if id is valid, false otherwise
 */
static inline bool __prsr_proto_id_is_valid(enum prsr_proto_id id)
{
	if (unlikely(id >= PRSR_PROTOS_NUM)) {
		prsr_err("invalid internal protocol id %u\n", id);
		return false;
	}
	return true;
}

/**
 * @brief Get parser table context
 * @param idx table index
 * @return struct table* table context on success, NULL otherwise
 */
struct table *prsr_get_tbl(u32 idx)
{
	switch (idx) {
	case PRSR_L2_TBL:
		return db->l2_tbl;
	case PRSR_LAST_ETHERTYPE_TBL:
		return db->last_eth_tbl;
	case PRSR_IMEM_TBL:
		return db->imem_tbl;
	case PRSR_NP_SEL_TBL:
		return db->np_sel_tbl;
	case PRSR_SKIP_TBL:
		return db->skip_tbl;
	case PRSR_MERGE_TBL:
		return db->merge_tbl;
	case PRSR_PIT_TBL:
		return db->pit_tbl;
	default:
		prsr_err("invalid parser table index %u\n", idx);
		return NULL;
	}
}

/**
 * @brief Layer 2 table compare operation opcode
 * @param op opcode
 * @return bool true if op is valid, false otherwise
 */
static inline bool __prsr_l2_cmp_op_is_valid(enum l2_cmp_op op)
{
	pr_debug("op %u\n", op);
	if (likely((op >= L2_CMP_OP_EQ) &&
		   (op < L2_CMP_OP_NUM)))
		return true;

	prsr_err("Invalid l2 compare opcode %u, max is %u\n",
		 op, L2_CMP_OP_NUM);
	return false;
}

/**
 * @brief IMEM table next protocol select logic validity test
 * @param logic
 * @return bool true if logic is valid, false otherwise
 */
static inline bool __prsr_np_logic_is_valid(enum np_logic logic)
{
	pr_debug("logic %u\n", logic);
	if (likely((logic >= NP_LOGIC_TUNN) &&
		   (logic < NP_LOGICS_NUM)))
		return true;

	prsr_err("Invalid imem np select logic %u, max is %u\n",
		 logic, NP_LOGICS_NUM);
	return false;
}

/**
 * @brief IMEM table next protocol select logic validity test
 * @param logic
 * @return bool true if logic is valid, false otherwise
 */
static inline bool __prsr_entry_type_is_valid(enum prsr_entry_type type)
{
	pr_debug("type %u\n", type);
	if (likely((type >= ETYPE_NONE) &&
		   (type < ETYPE_NUM)))
		return true;

	prsr_err("Invalid imem entry type %u, max is %u\n",
		 type, ETYPE_NUM);
	return false;
}

/**
 * @brief IMEM table skip logic validity test
 * @param logic
 * @return bool true if logic is valid, false otherwise
 */
static inline bool __prsr_skip_logic_is_valid(enum skip_logic logic)
{
	pr_debug("logic %u\n", logic);
	if (likely((logic >= SKIP_LOGIC_ALU) &&
		   (logic < SKIP_LOGICS_NUM)))
		return true;

	prsr_err("Invalid imem skip logic %u, max is %u\n",
		 logic, SKIP_LOGICS_NUM);
	return false;
}

/**
 * @brief IMEM table ALU opcode validity test
 * @param op
 * @return bool true if op is valid, false otherwise
 */
static inline bool __prsr_alu_opcode_is_valid(enum alu_op op)
{
	pr_debug("op %u\n", op);
	if (likely((op >= ALU_OP_IM) &&
		   (op < ALU_OPS_NUM)))
		return true;

	prsr_err("Invalid imem ALU opcode %u, max is %u\n",
		 op, ALU_OPS_NUM);
	return false;
}

/**
 * @brief Get parser protocol reference
 * @param id protocol internal id
 * @return struct proto_db_e* protocol reference on success, NULL
 *         otherwise
 */
static inline struct proto_db_e *__get_proto(enum prsr_proto_id id)
{
	if (unlikely(!__prsr_proto_id_is_valid(id)))
		return NULL;

	return &db->protos[id];
}

const struct proto_db_e *prsr_proto_info_get(enum prsr_proto_id id)
{
	return __get_proto(id);
}

s32 prsr_dflt_ethertype_set(enum prsr_proto_id id)
{
	struct proto_db_e *proto;
	u32 reg, mask;

	proto = __get_proto(id);
	if (unlikely(!proto))
		return -EINVAL;
	if (unlikely(!__prsr_imem_index_is_valid(proto->imem_idx))) {
		prsr_err("protocol %s(%u) has invalid imem index %u\n",
			 proto->name, id, proto->imem_idx);
		return -EINVAL;
	}

	spin_lock_bh(&db->lock);
	reg  = PP_REG_RD32(PP_PRSR_LAST_ETHERTYPE_MAP_REG);
	mask = PP_PRSR_LAST_ETHERTYPE_MAP_DEFAULT_PC_MSK;
	reg  = PP_FIELD_MOD(mask, proto->imem_idx, reg);
	pr_debug("id %u, imem_idx %u, reg %#x\n",
		 id, proto->imem_idx, reg);
	PP_REG_WR32(PP_PRSR_LAST_ETHERTYPE_MAP_REG, reg);
	spin_unlock_bh(&db->lock);

	return 0;
}

s32 prsr_dflt_ethertype_get(enum prsr_proto_id *id)
{
	const struct table_entry *ent;
	const struct proto_db_e  *proto;
	u32 reg, mask, dflt_imem;

	if (unlikely(ptr_is_null(id)))
		return -EINVAL;

	reg  = PP_REG_RD32(PP_PRSR_LAST_ETHERTYPE_MAP_REG);
	mask = PP_PRSR_LAST_ETHERTYPE_MAP_DEFAULT_PC_MSK;
	dflt_imem = PP_FIELD_GET(mask, reg);

	ent = table_entry_get(db->imem_tbl, dflt_imem);
	if (unlikely(!ent)) {
		prsr_err("failed to get default ethertype protocol at imem %u\n",
			 dflt_imem);
		return -ENOENT;
	}
	if (unlikely(!ent->data)) {
		prsr_err("no protocol in imem entry %u\n", dflt_imem);
		return -ENOENT;
	}

	proto = (const struct proto_db_e *)ent->data;
	*id   = proto->id;

	return 0;
}

/**
 * @brief Protocol header field extract info validity test
 * @param ext info
 * @return bool true if the entry is valid, false otherwise
 */
static bool __prsr_fld_ext_info_is_valid(const struct field_ext_info *fld)
{
	bool res = true;

	if (unlikely(ptr_is_null(fld)))
		return false;

	pr_debug("fv_idx %u, len %u, mask 0x%x, off %u\n",
		 fld->fv_idx, fld->len, fld->mask, fld->off);
	if (unlikely(sizeof(struct pp_fv) <= fld->fv_idx)) {
		prsr_err("fv_idx %u exceeds fv size %u\n", fld->fv_idx,
			 (u32)sizeof(struct pp_fv));
		res = false;
	}
	if (unlikely(!num_is_even(fld->fv_idx))) {
		prsr_err("fv_idx %u isn't an even number\n", fld->fv_idx);
		res = false;
	}
	if (unlikely(sizeof(struct pp_fv) <= fld->len)) {
		prsr_err("len %u exceeds fv size %u\n", fld->len,
			 (u32)sizeof(struct pp_fv));
		res = false;
	}

	return res;
}

/* ========================================================================== *
 *                      Parsing Information (PIT) Table                       *
 * ========================================================================== *
 */
/**
 * @brief Convert PIT entry to parser hw granularity
 * @param src pit entry to convert
 * @param dst pit entry to save the converted entry
 */
static void __prsr_fld_ext_to_pit_entry(const struct field_ext_info *fld,
					struct pit_entry *pit)
{
	if (unlikely(!(!ptr_is_null(fld) && !ptr_is_null(pit))))
		return;
	pr_debug("fld.off %u, fld.len %u, fld.fv_idx %u, fld.mask %#x\n",
		 fld->off, fld->len, fld->fv_idx, fld->mask);

	/* first, copy fld to pit */
	pit->fv_idx = fld->fv_idx;
	pit->off    = fld->off;
	pit->len    = fld->len;
	pit->mask   = fld->mask;
	/* adjust the values to fit the pit table restrictions */
	if (num_is_odd(pit->off)) {
		/* offset is odd, reduce it by 1 and increase the len by 1
		 * to copy also the previous byte
		 */
		pit->off -= 1;
		pit->len += 1;
		/* adjust to the mask to mask the extra byte */
		pit->mask = (pit->mask << 1) | (u32)BIT(0);
	}
	if (num_is_odd(pit->len)) {
		/* len is odd, increase it by 1 to copy also the next byte */
		pit->len  += 1;
		/* adjust to the mask to mask the extra byte */
		pit->mask |= (u32)(BIT(0) << (pit->len - 1));
	}

	/* now that all values are even, we can safely change the granularity */
	pit->fv_idx = PRSR_PIT_VAL_TO_HW_GRAN(pit->fv_idx);
	pit->len    = PRSR_PIT_VAL_TO_HW_GRAN(pit->len);
	pit->off    = PRSR_PIT_VAL_TO_HW_GRAN(pit->off);

	pr_debug("pit.off %u, pit.len %u, pit.fv_idx %u, pit.mask %#x\n",
		 pit->off, pit->len, pit->fv_idx, pit->mask);
}

/**
 * @brief PIT table info validity test
 * @param proto info
 * @return bool true if info is valid, false otherwise
 */
static bool __prsr_pit_info_is_valid(const struct pit_info *pit)
{
	bool res = true;

	if (unlikely(ptr_is_null(pit)))
		return false;

	pr_debug("base %u, cnt %u\n", pit->base, pit->cnt);
	if (unlikely(!__prsr_pit_index_is_valid(pit->base + pit->cnt)))
		res = false;

	return res;
}

/**
 * @brief PIT table entry fields validity test
 * @param ent info
 * @return bool true if the entry is valid, false otherwise
 */
static bool __prsr_pit_entry_is_valid(const struct pit_entry *ent)
{
	bool res = true;

	if (unlikely(ptr_is_null(ent)))
		return false;

	pr_debug("fv_idx %u, len %u, mask 0x%x, off %u\n",
		 ent->fv_idx, ent->len, ent->mask, ent->off);
	if (unlikely(!__pp_mask_value_valid(PP_PRSR_PIT_DATA_OFFSET_MSK,
					    ent->off))) {
		prsr_err("invalid offset\n");
		res = false;
	}
	if (unlikely(!__pp_mask_value_valid(PP_PRSR_PIT_DATA_LEN_MSK,
					    ent->len))) {
		prsr_err("invalid len\n");
		res = false;
	}
	if (unlikely(!__pp_mask_value_valid(PP_PRSR_PIT_TARGET_IDX_MSK,
					    ent->fv_idx))) {
		prsr_err("invalid fv index\n");
		res = false;
	}

	return res;
}

s32 prsr_pit_entry_set(u16 idx, const struct pit_entry *ent)
{
	struct pit_entry zero_ent = { 0 };
	u32 reg, mask;

	pr_debug("idx %u, ent %p\n", idx, ent);
	if (unlikely(!__prsr_pit_index_is_valid(idx)) ||
	    unlikely(ent && !__prsr_pit_entry_is_valid(ent)))
		return -EINVAL;

	ent = ent ? ent : &zero_ent;
	reg   = PP_FIELD_PREP(PP_PRSR_PIT_TARGET_IDX_MSK,  ent->fv_idx);
	reg  |= PP_FIELD_PREP(PP_PRSR_PIT_DATA_LEN_MSK,    ent->len);
	reg  |= PP_FIELD_PREP(PP_PRSR_PIT_DATA_OFFSET_MSK, ent->off);
	mask  = PP_FIELD_PREP(PP_PRSR_PIT_FV_MASK_MSK,     ent->mask);

	PP_REG_WR32(PP_PRSR_PIT_REG_IDX(idx),         reg);
	PP_REG_WR32(PP_PRSR_PIT_FV_MASK_REG_IDX(idx), mask);

	return 0;
}

s32 prsr_pit_entry_get(u16 idx, struct pit_entry *ent)
{
	u32 reg  = 0;
	u32 mask = 0;

	pr_debug("idx %u, ent %p\n", idx, ent);
	if (unlikely(ptr_is_null(ent)))
		return -EINVAL;
	if (unlikely(!__prsr_pit_index_is_valid(idx)))
		return -EINVAL;

	reg  = PP_REG_RD32(PP_PRSR_PIT_REG_IDX(idx));
	mask = PP_REG_RD32(PP_PRSR_PIT_FV_MASK_REG_IDX(idx));

	ent->fv_idx = PP_FIELD_GET(PP_PRSR_PIT_TARGET_IDX_MSK,  reg);
	ent->len    = PP_FIELD_GET(PP_PRSR_PIT_DATA_LEN_MSK,    reg);
	ent->off    = PP_FIELD_GET(PP_PRSR_PIT_DATA_OFFSET_MSK, reg);
	ent->mask   = PP_FIELD_GET(PP_PRSR_PIT_FV_MASK_MSK,     mask);

	return 0;
}

/**
 * @brief Check if a given pit table entry already exist in the table
 * @param subject the entry to search for
 * @param off offset to start the search from
 * @param idx the entry's index in case it exist
 * @return bool true if entry exist, false otherwise
 */
static bool __prsr_pit_entry_find(const struct pit_entry *subject,
				  u16 off, u16 *idx)
{
	struct pit_entry tmp;
	s32 ret;
	u16 _idx;

	/* when using memcmp on structures, must use memset b4 comparing
	 * cause the compiler may add alignment bytes and there is no
	 * guarantee that these will be set to zero
	 */
	memset(&tmp, 0, sizeof(tmp));

	_idx = table_get_next_entry(db->pit_tbl, off);
	while (_idx < db->pit_tbl->n_ent) {
		ret = prsr_pit_entry_get(_idx, &tmp);
		if (unlikely(ret)) {
			prsr_err("failed to get pit entry %u info\n", _idx);
			return false;
		}
		if (!memcmp(subject, &tmp, sizeof(*subject))) {
			*idx = _idx;
			return true;
		}

		_idx = table_get_next_entry(db->pit_tbl, _idx + 1);
	}
	return false;
}

/**
 * @brief Search for the biggest subset, starting from the first entry
 *        of @ext, of the specified extract info which exist in the
 *        PIT table.
 * @param ext the extract info array
 * @param n number of entries in 'ext'
 * @param sub_idx the index where the subset was found in case it
 *                exist
 * @param sub_sz subset size, number of extract info that were found
 * @return bool true if subset exist, false otherwise
 */
static bool __prsr_pit_entries_subset_find(const struct pit_entry *ext,
					   u16 n, u16 *sub_idx, u16 *sub_sz)
{
	u16 i, idx, ents_found = 0;

	pr_debug("ext %p, n %u\n", ext, n);
	if (unlikely(!(!ptr_is_null(ext)     &&
		       !ptr_is_null(sub_idx) &&
		       !ptr_is_null(sub_sz))))
		return false;

	if (unlikely(0 < n && PRSR_MAX_EXTRACT_ENTRIES < n))
		return false;

	*sub_sz = 0;
	/* try to find the first entry in the table */
	if (!__prsr_pit_entry_find(&ext[0], 0, &idx))
		/* first entry doesn't exist, subset not exist*/
		return false;
	/* first entry found, save the index */
	pr_debug("entry 0 found at index %u\n", idx);
	*sub_idx = idx;
	ents_found++;
	/* try to find following entries */
	for (i = 1; i < n; i++) {
		if (!__prsr_pit_entry_find(&ext[i], *sub_idx + i, &idx))
			break;
		pr_debug("entry %u found at index %u\n", i, idx);
		if (idx != *sub_idx + i)
			break;
		ents_found++;
	}
	*sub_sz = ents_found;

	return true;
}

/**
 * @brief Add new pit tables entries
 * @param data data pointer to attach to the entries
 * @param ext protocol extract info array
 * @param n number of entries
 * @param idx pit table index where the new entries was insert
 * @return s32 0 on success, error code otherwise
 */
static s32 __prsr_pit_entries_add(const char *name,
				  const struct pit_entry *ext,
				  u16 n, u16 *idx)
{
	s32 ret = 0;
	u16 i, n_found = 0, leftovers;

	if (unlikely(ptr_is_null(idx)))
		return -EINVAL;

	pr_debug("name %s, ext %p, n %u, idx %p\n", name, ext, n, idx);
	*idx = TABLE_ENTRY_INVALID_IDX;
	/* lock before searching to make sure nothing will be changed */
	spin_lock_bh(&db->lock);
	__prsr_pit_entries_subset_find(ext, n, idx, &n_found);
	pr_debug("found subset of %u at index %u\n", n_found, *idx);
	if (n_found == n)
		/* all needed pit entries found, done*/
		goto set_entries;
	else if (n_found == 0)
		/* no subset found, alloc all */
		goto alloc_all;

	/* allocate remaining entries */
	leftovers = *idx + n_found;
	ret = table_alloc_entries(db->pit_tbl, name, NULL, n - n_found,
				  &leftovers, false);

	/* we found a subset but failed to allocate the following entries,
	 * try to allocate all entries
	 */
	if (unlikely(ret)) {
alloc_all:
		*idx = TABLE_ENTRY_INVALID_IDX;
		n_found = 0;
		ret = table_alloc_entries(db->pit_tbl, name, NULL,
					  n, idx, false);
		if (ret)
			goto done;
	}

set_entries:
	if (unlikely(!__prsr_pit_index_is_valid(*idx))) {
		prsr_err("invalid pit idx %u -- entry alloc issue\n", *idx);
		ret = -ENOSPC;
		goto done;
	}
	pr_debug("PIT entries %u-%u was allocated\n", *idx, *idx + n - 1);

	for (i = 0; i < n; i++) {
		if (i < n_found) {
			/* append the new description to the existing entries */
			table_entry_desc_cat(db->pit_tbl, *idx + i, name, "/");
			table_entry_ref_inc(db->pit_tbl, *idx + i);
		} else {
			/* new allocated entry, set entry in hw */
			ret = prsr_pit_entry_set(*idx + i, &ext[i]);
			if (unlikely(ret))
				goto done;
		}
	}

done:
	spin_unlock_bh(&db->lock);
	return ret;
}

/**
 * @brief Delete existing pit table entries
 * @param name name to remove from the entries name in case the
 *             entries has other users
 * @param idx entry index
 * @param n number of entries to delete
 * @return s32 0 on success, error code otherwise
 */
static s32 __prsr_pit_entries_del(const char *name, u16 idx, u16 n)
{
	s32 ret;
	u32 i;

	for (i = 0; i < n; i++) {
		pr_debug("free entry %u\n", idx);
		/* clean protocol name from the entry name */
		table_entry_name_remove(db->pit_tbl, idx, name);
		/* free entry */
		ret = table_free_entry(db->pit_tbl, idx + i);
		switch (ret) {
		case 0:
			/* entry freed, invalidate in HW */
			prsr_pit_entry_set(idx + i, NULL);
		case -EEXIST:
			/* entry still is use by other protocols */
			continue;
		default:
			prsr_err("failed to free pit entry %u\n", idx + i);
			return ret;
		}
	}

	return 0;
}

/* ========================================================================== *
 *                      MAC Config                                            *
 * ========================================================================== *
 */
s32 prsr_mac_cfg_set(u8 len, struct pit_info *pit)
{
	u32 reg;

	pr_debug("\n");
	if (!__prsr_pit_info_is_valid(pit))
		return -EINVAL;

	reg  = PP_FIELD_PREP(PP_PRSR_MAC_CFG_MAC_LENGTH_MSK, len);
	reg |= PP_FIELD_PREP(PP_PRSR_MAC_CFG_PIT_CNT_MSK,    pit->cnt);
	reg |= PP_FIELD_PREP(PP_PRSR_MAC_CFG_PIT_IDX_MSK,    pit->base);
	pr_debug("len = %u, pit base %u, pit cnt %u --> reg %#x\n",
		 len, pit->base, pit->cnt, reg);

	PP_REG_WR32(PP_PRSR_MAC_CFG_REG, reg);

	return 0;
}

s32 prsr_mac_cfg_get(u16 *len, struct pit_info *pit)
{
	u32 reg = 0;

	if (unlikely(!(!ptr_is_null(pit) &&
		       !ptr_is_null(len))))
		return -EINVAL;

	reg       = PP_REG_RD32(PP_PRSR_MAC_CFG_REG);
	*len      = PP_FIELD_GET(PP_PRSR_MAC_CFG_MAC_LENGTH_MSK, reg);
	pit->cnt  = PP_FIELD_GET(PP_PRSR_MAC_CFG_PIT_CNT_MSK,    reg);
	pit->base = PP_FIELD_GET(PP_PRSR_MAC_CFG_PIT_IDX_MSK,    reg);

	pr_debug("len = %u, pit base %u, pit cnt %u\n",
		 *len, pit->base, pit->cnt);

	return 0;
}

s32 prsr_ethertype_cfg_set(struct pit_info *pit)
{
	u32 reg;

	pr_debug("\n");
	if (unlikely(!__prsr_pit_info_is_valid(pit)))
		return -EINVAL;

	spin_lock_bh(&db->lock);
	reg = PP_REG_RD32(PP_PRSR_LAST_ETHERTYPE_MAP_REG);
	reg = PP_FIELD_MOD(PP_PRSR_LAST_ETHERTYPE_MAP_PIT_VLD_MSK,
			   pit->cnt, reg);
	reg = PP_FIELD_MOD(PP_PRSR_LAST_ETHERTYPE_MAP_PIT_IDX_MSK,
			   pit->base, reg);
	pr_debug("pit base %u, pit cnt %u --> reg %#x\n",
		 pit->base, pit->cnt, reg);

	PP_REG_WR32(PP_PRSR_LAST_ETHERTYPE_MAP_REG, reg);
	spin_unlock_bh(&db->lock);

	return 0;
}

s32 prsr_ethertype_cfg_get(u16 *dflt_pc, struct pit_info *pit)
{
	u32 reg;

	if (unlikely(!(!ptr_is_null(dflt_pc) &&
		       !ptr_is_null(pit))))
		return -EINVAL;
	pr_debug("\n");

	spin_lock_bh(&db->lock);
	reg       = PP_REG_RD32(PP_PRSR_LAST_ETHERTYPE_MAP_REG);
	pit->cnt  = PP_FIELD_GET(PP_PRSR_LAST_ETHERTYPE_MAP_PIT_VLD_MSK, reg);
	pit->base = PP_FIELD_GET(PP_PRSR_LAST_ETHERTYPE_MAP_PIT_IDX_MSK, reg);
	*dflt_pc  = PP_FIELD_GET(PP_PRSR_LAST_ETHERTYPE_MAP_DEFAULT_PC_MSK,
				 reg);
	pr_debug("dflt_pc %u, pit base %u, pit cnt %u --> reg %#x\n",
		 *dflt_pc, pit->base, pit->cnt, reg);

	spin_unlock_bh(&db->lock);

	return 0;
}

void prsr_fv_swap_set(bool swap)
{
	u32 reg;

	spin_lock_bh(&db->lock);
	reg = PP_REG_RD32(PP_PRSR_OUTPUT_SWAP_REG);
	reg = PP_FIELD_MOD(PP_PRSR_OUTPUT_SWAP_SWAP_FV_MSK, swap, reg);
	PP_REG_WR32(PP_PRSR_OUTPUT_SWAP_REG, reg);
	spin_unlock_bh(&db->lock);
}

void prsr_fv_mask_swap_set(bool swap)
{
	u32 reg;

	spin_lock_bh(&db->lock);
	reg = PP_REG_RD32(PP_PRSR_OUTPUT_SWAP_REG);
	reg = PP_FIELD_MOD(PP_PRSR_OUTPUT_SWAP_SWAP_FVM_MSK, swap, reg);
	PP_REG_WR32(PP_PRSR_OUTPUT_SWAP_REG, reg);
	spin_unlock_bh(&db->lock);
}

void prsr_fv_blocks_swap_set(bool swap)
{
	u32 reg;

	spin_lock_bh(&db->lock);
	reg = PP_REG_RD32(PP_PRSR_OUTPUT_SWAP_REG);
	reg = PP_FIELD_MOD(PP_PRSR_OUTPUT_SWAP_SWAP_OP_FV_MSK, swap, reg);
	PP_REG_WR32(PP_PRSR_OUTPUT_SWAP_REG, reg);
	spin_unlock_bh(&db->lock);
}

bool prsr_fv_swap_get(void)
{
	u32 reg;

	reg = PP_REG_RD32(PP_PRSR_OUTPUT_SWAP_REG);
	return PP_FIELD_GET(PP_PRSR_OUTPUT_SWAP_SWAP_FV_MSK, reg);
}

bool prsr_fv_mask_swap_get(void)
{
	u32 reg;

	reg = PP_REG_RD32(PP_PRSR_OUTPUT_SWAP_REG);
	return PP_FIELD_GET(PP_PRSR_OUTPUT_SWAP_SWAP_FVM_MSK, reg);
}

bool prsr_fv_blocks_swap_get(void)
{
	u32 reg;

	reg = PP_REG_RD32(PP_PRSR_OUTPUT_SWAP_REG);
	return PP_FIELD_GET(PP_PRSR_OUTPUT_SWAP_SWAP_OP_FV_MSK, reg);
}

u64 prsr_imem_hit_map_get(void)
{
	u64 high, low;

	low  = (u64)PP_REG_RD32(PP_PRSR_LY3_PROTO_HIT_STICKY_0_31_REG);
	high = (u64)PP_REG_RD32(PP_PRSR_LY3_PROTO_HIT_STICKY_32_63_REG);

	return ((high << 32) | low);
}

s32 prsr_imem_last_pkt_flow_get(struct pkt_imem_flow *flow)
{
	u32 i, val;
	ulong mask;

	if (unlikely(ptr_is_null(flow)))
		return -EINVAL;

	memset(flow, 0, sizeof(*flow));

	for (i = 0; i < ARRAY_SIZE(flow->ent_idx); i++) {
		mask = PRSR_LY3_PROTO_SEQ_MASK(i);
		val  = PP_REG_RD32(PRSR_LY3_PROTO_SEQ_REG(i));
		flow->ent_idx[i] = PP_FIELD_GET(mask, val);
	}

	return 0;
}

u32 prsr_l2_table_hit_map_get(void)
{
	u32 reg;

	reg = PP_REG_RD32(PP_PRSR_LY2_ETYPE_HIT_STICKY_REG);

	return PP_FIELD_GET(PP_PRSR_LY2_ETYPE_HIT_STICKY_MSK, reg);
}

u32 prsr_l2_table_last_hit_map_get(void)
{
	u32 reg;

	reg = PP_REG_RD32(PP_PRSR_LY2_ETYPE_HIT_LAST_REG);

	return PP_FIELD_GET(PP_PRSR_LY2_ETYPE_HIT_LAST_MSK, reg);
}

void prsr_last_fv_get(struct pp_fv *fv, struct pp_fv *masked_fv)
{
	ulong mask[BITS_TO_LONGS(sizeof(struct pp_fv))];
	u32 i;

	if (fv)
		/* read the last fv */
		PP_REG_RD_REP(PP_PRSR_FIELDS_RECORD_REG, fv, sizeof(*fv));

	if (masked_fv) {
		/* read the last fv */
		PP_REG_RD_REP(PP_PRSR_FIELDS_RECORD_REG, masked_fv,
			      sizeof(*masked_fv));
		/* read the last mask */
		PP_REG_RD_REP(PP_PRSR_MASK_RECORD_REG, mask, sizeof(mask));
		/* mask the field vector */
		for_each_set_bit(i, mask, sizeof(struct pp_fv))
			((char *)masked_fv)[i] = 0;
	}
}

s32 prsr_last_stw_get(struct prsr_hw_stw *hw_stw)
{
	if (unlikely(ptr_is_null(hw_stw)))
		return -EINVAL;

	PP_REG_RD_REP(PP_PRSR_STATUS_RECORD_REG, hw_stw, sizeof(*hw_stw));

	return 0;
}

void prsr_stw_sticky_reset(void)
{
	u32 reg;

	reg = PP_REG_RD32(PP_PRSR_STATUS_RECORD_STATE_REG);
	reg = PP_FIELD_MOD(PP_PRSR_STATUS_RECORD_STATE_STICKY_ERROR_MSK,
			   0, reg);
	PP_REG_WR32(PP_PRSR_STATUS_RECORD_STATE_REG, reg);
}

/**
 * @brief Initialize parser output swap configuration. Configuring the
 *        parser to output the field vector by order, 1st byte goes
 *        first, 2nd goes second and so on. 128 bytes little endian.
 */
static void __prsr_fv_swap_init(void)
{
	/* set parser to output fv and mask by order: 1st byte
	 * goes first, 2nd goes second and so on ==> little endian on the
	 * entire field vector.
	 */
	prsr_fv_swap_set(false);
	prsr_fv_mask_swap_set(false);
	/* disable the parser from swapping the fv's 2 512 bits blocks */
	prsr_fv_blocks_swap_set(false);
}

s32 prsr_max_hdr_len_set(u32 hdr_len)
{
	u32 mask, reg = 0;

	mask = PP_PRSR_LY3_CFG_MAX_HEADER_LEN_MSK;
	if (unlikely(!__pp_mask_value_valid(mask, hdr_len))) {
		prsr_err("invalid maximum header length %u\n", hdr_len);
		return -EINVAL;
	}

	spin_lock_bh(&db->lock);
	reg = PP_REG_RD32(PP_PRSR_LY3_CFG_REG);
	reg = PP_FIELD_MOD(mask, hdr_len, reg);
	PP_REG_WR32(PP_PRSR_LY3_CFG_REG, reg);
	spin_unlock_bh(&db->lock);

	return 0;
}

u32 prsr_max_hdr_len_get(void)
{
	u32 mask, reg = 0;

	mask = PP_PRSR_LY3_CFG_MAX_HEADER_LEN_MSK;
	reg  = PP_REG_RD32(PP_PRSR_LY3_CFG_REG);

	return PP_FIELD_GET(mask, reg);
}

s32 prsr_max_imem_proto_hdr_len_set(u32 hdr_len)
{
	u32 mask, reg = 0;

	mask = PP_PRSR_LY3_CFG_MAX_PROT_LEN_MSK;
	if (unlikely(!__pp_mask_value_valid(mask, hdr_len))) {
		prsr_err("invalid maximum imem proto header length %u\n",
			 hdr_len);
		return -EINVAL;
	}

	spin_lock_bh(&db->lock);
	reg = PP_REG_RD32(PP_PRSR_LY3_CFG_REG);
	reg = PP_FIELD_MOD(mask, hdr_len, reg);
	PP_REG_WR32(PP_PRSR_LY3_CFG_REG, reg);
	spin_unlock_bh(&db->lock);

	return 0;
}

u32 prsr_max_imem_proto_hdr_len_get(void)
{
	u32 mask, reg = 0;

	mask = PP_PRSR_LY3_CFG_MAX_PROT_LEN_MSK;
	reg  = PP_REG_RD32(PP_PRSR_LY3_CFG_REG);

	return PP_FIELD_GET(mask, reg);
}

s32 prsr_prefetch_hdr_len_set(u32 hdr_len)
{
	u32 mask, reg = 0;

	mask = PP_PRSR_RXA_MAX_HDR_LINES_MSK;
	if (unlikely(!__pp_mask_value_valid(mask, hdr_len))) {
		prsr_err("invalid max header len prefetch %u\n", hdr_len);
		return -EINVAL;
	}

	spin_lock_bh(&db->lock);
	reg = PP_REG_RD32(PP_PRSR_RXA_REG);
	reg = PP_FIELD_MOD(mask, hdr_len, reg);
	PP_REG_WR32(PP_PRSR_RXA_REG, reg);
	spin_unlock_bh(&db->lock);

	return 0;
}

u32 prsr_prefetch_hdr_len_get(void)
{
	u32 mask, reg = 0;

	mask = PP_PRSR_RXA_MAX_HDR_LINES_MSK;
	reg  = PP_REG_RD32(PP_PRSR_RXA_REG);

	return PP_FIELD_GET(mask, reg);
}

void prsr_hw_err_flags_set(u32 err_flags, bool overwrite)
{
	u32 reg = 0;

	if (!overwrite)
		reg = PP_REG_RD32(PP_PRSR_ABORT_CFG_REG);
	reg |= err_flags;
	PP_REG_WR32(PP_PRSR_ABORT_CFG_REG, reg);
}

u32 prsr_hw_err_flags_get(void)
{
	return PP_REG_RD32(PP_PRSR_ABORT_CFG_REG);
}

static void prsr_hw_drop_flags_init_astep(u32 *flags)
{
	/* don't drop packets if rbp mark it for discard */
	*flags |= PP_FIELD_PREP(PP_PRSR_ABORT_CFG_DROP_RPB_DISCARD_MSK, 0);
	/* don't drop packet if rbp mark it with error */
	*flags |= PP_FIELD_PREP(PP_PRSR_ABORT_CFG_DROP_RPB_ERROR_MSK, 0);
	/* don't drop zero len (reported by RPB in stw) packets */
	*flags |= PP_FIELD_PREP(PP_PRSR_ABORT_CFG_DROP_ZERO_LEN_MSK, 0);
	/* don't drop if l3 len exceeds min/max header (from imem) */
	*flags |= PP_FIELD_PREP(PP_PRSR_ABORT_CFG_DROP_PROTO_LENGTH_ERR_MSK, 0);
	/* don't drop in case l3 len exceeds max header len
	 * configured in ly3_config register
	 */
	*flags |= PP_FIELD_PREP(PP_PRSR_ABORT_CFG_DROP_MAX_HEADER_LEN_ERR_MSK,
				0);
	/* don't drop in case l3 len exceeds max imem header length
	 * configured in ly3_config register
	 */
	*flags |= PP_FIELD_PREP(PP_PRSR_ABORT_CFG_DROP_PROTO_OVERFLOW_ERR_MSK,
				0);
	/* don't drop if last ethertype wasn't identified */
	*flags |= PP_FIELD_PREP(PP_PRSR_ABORT_CFG_DROP_NO_LAST_ETH_TYPE_ERR_MSK,
				0);
	/* don't drop if l3 packet len doesn't match stw length */
	*flags |= PP_FIELD_PREP(PP_PRSR_ABORT_CFG_DROP_IP_LEN_ERR_MSK, 0);

	*flags |= PP_FIELD_PREP(PP_PRSR_ABORT_CFG_DROP_ABORT_CNT_ERR_MSK, 0);
	*flags |= PP_FIELD_PREP(PP_PRSR_ABORT_CFG_DROP_EOF_NO_PAYLOAD_ERR_MSK,
                                0);
}

static void prsr_hw_drop_flags_init(u32 *flags)
{
	/* drop packets if rbp mark it for discard */
	*flags |= PP_FIELD_PREP(PP_PRSR_ABORT_CFG_DROP_RPB_DISCARD_MSK, 1);
	/* drop packet if rbp mark it with error */
	*flags |= PP_FIELD_PREP(PP_PRSR_ABORT_CFG_DROP_RPB_ERROR_MSK, 1);
	/* drop zero len (reported by RPB in stw) packets */
	*flags |= PP_FIELD_PREP(PP_PRSR_ABORT_CFG_DROP_ZERO_LEN_MSK, 1);
	/* drop if l3 len exceeds min/max header (from imem) */
	*flags |= PP_FIELD_PREP(PP_PRSR_ABORT_CFG_DROP_PROTO_LENGTH_ERR_MSK, 1);
	/* drop in case l3 len exceeds max header len
	 * configured in ly3_config register
	 */
	*flags |= PP_FIELD_PREP(PP_PRSR_ABORT_CFG_DROP_MAX_HEADER_LEN_ERR_MSK,
				1);
	/* drop in case l3 len exceeds max imem header length
	 * configured in ly3_config register
	 */
	*flags |= PP_FIELD_PREP(PP_PRSR_ABORT_CFG_DROP_PROTO_OVERFLOW_ERR_MSK,
				1);
	/* don't drop if last ethertype wasn't identified */
	*flags |= PP_FIELD_PREP(PP_PRSR_ABORT_CFG_DROP_NO_LAST_ETH_TYPE_ERR_MSK,
				0);
	/* don't drop if l3 packet len doesn't match stw length */
	*flags |= PP_FIELD_PREP(PP_PRSR_ABORT_CFG_DROP_IP_LEN_ERR_MSK, 0);

	*flags |= PP_FIELD_PREP(PP_PRSR_ABORT_CFG_DROP_ABORT_CNT_ERR_MSK, 1);
	*flags |= PP_FIELD_PREP(PP_PRSR_ABORT_CFG_DROP_EOF_NO_PAYLOAD_ERR_MSK,
				1);
}

void prsr_hw_err_flags_init(void)
{
	u32 flags = 0;

	/* for A-step we must send all traffic through the parser uc
	 * 1. we must send also all the error packets to uc
	 * 2. drop packets will not go to the uc flow so we disable all drop
	 *    bits and the drop will be set by the uc on any error packet
	 */
	if (pp_silicon_step_get() == PP_SSTEP_A) {
		/* forward to FW in case of error */
		flags |=
			PP_FIELD_PREP(PP_PRSR_ABORT_CFG_EN_ERR_FALLBACK_MSK, 1);
		prsr_hw_drop_flags_init_astep(&flags);
	} else {
		/* don't forward to FW in case of error */
		flags |=
			PP_FIELD_PREP(PP_PRSR_ABORT_CFG_EN_ERR_FALLBACK_MSK, 0);
		prsr_hw_drop_flags_init(&flags);
	}

	/* set error flag for packets received with discard flag on */
	flags |= PP_FIELD_PREP(PP_PRSR_ABORT_CFG_ERROR_RPB_DISCARD_MSK, 1);
	/* set error flag for packets received with error flag on */
	flags |= PP_FIELD_PREP(PP_PRSR_ABORT_CFG_ERROR_RPB_ERROR_MSK, 1);
	/* set error flag for zero length (reported in status word) packets */
	flags |= PP_FIELD_PREP(PP_PRSR_ABORT_CFG_ERROR_ZERO_LEN_MSK, 1);
	/* do not set error flag in case last ethertype wasn't identified */
	flags |= PP_FIELD_PREP(PP_PRSR_ABORT_CFG_ERROR_NO_LAST_ETH_TYPE_ERR_MSK,
			       0);
	/* set error flag if l3 len exceeds min/max header (from imem) */
	flags |= PP_FIELD_PREP(PP_PRSR_ABORT_CFG_ERROR_PROTO_LENGTH_ERR_MSK, 1);
	/* set error flag if l3 len exceeds max header length
	 * configured in ly3_config register
	 */
	flags |= PP_FIELD_PREP(PP_PRSR_ABORT_CFG_ERROR_MAX_HEADER_LEN_ERR_MSK,
			       1);
	/* set error flag if l3 length exceeds max imem header length
	 * configured in ly3_config register
	 */
	flags |= PP_FIELD_PREP(PP_PRSR_ABORT_CFG_ERROR_PROTO_OVERFLOW_ERR_MSK,
			       1);
	/* don't set error flag if l3 packet length doesn't match stw len */
	flags |= PP_FIELD_PREP(PP_PRSR_ABORT_CFG_ERROR_IP_LEN_ERR_MSK, 0);

	/* do not abort if last ethertype wasn't identified */
	flags |= PP_FIELD_PREP(PP_PRSR_ABORT_CFG_NO_LAST_ETH_TYPE_ABORT_EN_MSK,
			       0);
	/* abort parsing if l3 length exceeds max header length
	 * configured in ly3_config register
	 */
	flags |= PP_FIELD_PREP(PP_PRSR_ABORT_CFG_MAX_HEADER_LEN_ABORT_EN_MSK,
			       1);
	/* abort parsing if l3 length exceeds max imem header length
	 * configured in ly3_config register
	 */
	flags |= PP_FIELD_PREP(PP_PRSR_ABORT_CFG_PROTO_OVERFLOW_ABORT_EN_MSK,
			       1);
	/* don't abort parsing if l3 packet length doesn't match stw len */
	flags |= PP_FIELD_PREP(PP_PRSR_ABORT_CFG_IP_LEN_ABORT_EN_MSK, 0);

	/* use reset values */
	flags |= PP_FIELD_PREP(PP_PRSR_ABORT_CFG_ERROR_ABORT_CNT_ERR_MSK,
			       PP_PRSR_ABORT_CFG_ERROR_ABORT_CNT_ERR_RST);
	flags |= PP_FIELD_PREP(PP_PRSR_ABORT_CFG_ERROR_EOF_NO_PAYLOAD_ERR_MSK,
			       PP_PRSR_ABORT_CFG_ERROR_EOF_NO_PAYLOAD_ERR_RST);
	flags |= PP_FIELD_PREP(PP_PRSR_ABORT_CFG_ABORT_CNT_ABORT_EN_MSK,
			       PP_PRSR_ABORT_CFG_ABORT_CNT_ABORT_EN_RST);
	flags |= PP_FIELD_PREP(PP_PRSR_ABORT_CFG_EOF_NO_PAYLOAD_ABORT_EN_MSK,
			       PP_PRSR_ABORT_CFG_EOF_NO_PAYLOAD_ABORT_EN_RST);

	prsr_hw_err_flags_set(flags, true);
}

void prsr_fv_mask_en_set(bool mask_en)
{
	u32 reg = 0;

	reg = PP_REG_RD32(PP_PRSR_FV_MASK_DIS_REG);
	reg = PP_FIELD_MOD(PP_PRSR_FV_MASK_DIS_MSK, !mask_en, reg);
	PP_REG_WR32(PP_PRSR_FV_MASK_DIS_REG, reg);
}

bool prsr_fv_mask_en_get(void)
{
	u32 reg;

	reg = PP_REG_RD32(PP_PRSR_FV_MASK_DIS_REG);
	return !(PP_FIELD_GET(PP_PRSR_FV_MASK_DIS_MSK, reg));
}

/**
 * @brief Set default imem program counters to: payload, idle
 * @return s32 0 on success, error code otherwise
 */
static s32 __prsr_set_dflt_pc(void)
{
	struct proto_db_e *payload, *idle;
	u32 reg;

	payload = __get_proto(PRSR_PROTO_PAYLOAD);
	idle    = __get_proto(PRSR_PROTO_IDLE);

	if (unlikely(!payload || !idle))
		return -EINVAL;

	if (unlikely(!__prsr_imem_index_is_valid(payload->imem_idx))) {
		prsr_err("Invalid imem index %u for payload protocol\n",
			 payload->imem_idx);
		return -EPERM;
	}
	if (unlikely(!__prsr_imem_index_is_valid(idle->imem_idx))) {
		prsr_err("Invalid imem index %u for idle protocol\n",
			 idle->imem_idx);
		return -EPERM;
	}

	reg = PP_REG_RD32(PP_PRSR_SET_PC_REG);
	reg = PP_FIELD_MOD(PP_PRSR_SET_PC_FILTER_PAYLOAD_PC_MSK,
			   payload->imem_idx, reg);
	reg = PP_FIELD_MOD(PP_PRSR_SET_PC_LY3_FRAG_NH_PC_MSK,
			   payload->imem_idx, reg);
	reg = PP_FIELD_MOD(PP_PRSR_SET_PC_ANA_STOP_ANALYZING_PC_MSK,
			   payload->imem_idx, reg);
	reg = PP_FIELD_MOD(PP_PRSR_SET_PC_IDLE_PC_MSK,
			   idle->imem_idx, reg);
	PP_REG_WR32(PP_PRSR_SET_PC_REG, reg);

	return 0;
}

s32 prsr_ipv6_nexthdr_support_set(u32 nexthdr, bool en)
{
	u32 reg;

	if (unlikely(nexthdr > NEXTHDR_MAX)) {
		prsr_err("Invalid IPv6 next header value %u\n", nexthdr);
		return -EINVAL;
	}

	spin_lock_bh(&db->lock);
	reg = PP_REG_RD32(PRSR_IPV6_NEXTHDR_TO_REG(nexthdr));
	reg = PP_FIELD_MOD(BIT(nexthdr), !en, reg);
	PP_REG_WR32(PRSR_IPV6_NEXTHDR_TO_REG(nexthdr), reg);
	spin_unlock_bh(&db->lock);

	return 0;
}

s32 prsr_ipv6_nexthdr_support_get(u32 nexthdr, bool *en)
{
	u32 reg;

	if (unlikely(ptr_is_null(en)))
		return -EINVAL;
	if (unlikely(nexthdr > NEXTHDR_MAX)) {
		prsr_err("Invalid IPv6 next header value %u\n", nexthdr);
		return -EINVAL;
	}

	reg = PP_REG_RD32(PRSR_IPV6_NEXTHDR_TO_REG(nexthdr));
	*en = !PP_FIELD_GET(BIT(nexthdr), reg);

	return 0;
}

s32 prsr_ipv6_nexthdr_support_get_all(unsigned long *nexthdr_map, u32 map_sz)
{
	u32 nexthdr;
	bool en;

	if (unlikely(ptr_is_null(nexthdr_map)))
		return -EINVAL;
	if (unlikely(map_sz != NEXTHDR_MAX))
		prsr_err("invalid nexthdr bitmap size %u\n", map_sz);

	bitmap_zero(nexthdr_map, map_sz);
	for (nexthdr = 0; nexthdr <= NEXTHDR_MAX; nexthdr++) {
		en = false;
		prsr_ipv6_nexthdr_support_get(nexthdr, &en);
		if (en)
			set_bit(nexthdr, nexthdr_map);
	}

	return 0;
}

/**
 * @brief Enable support for all IPv6 next headers
 */
static void __prsr_ipv6_nexthdr_support_init(void)
{
	u32 nexthdr;

	for (nexthdr = 0; nexthdr <= NEXTHDR_MAX; nexthdr++)
		prsr_ipv6_nexthdr_support_set(nexthdr, true);
}

/* ========================================================================== *
 *                      Layer 2 Table                                         *
 * ========================================================================== *
 */

/**
 * @brief Test whether l2 entry is valid or not
 * @param l2 entry to test
 * @return bool true in case the entry is valid, false otherwise
 */
static bool __prsr_l2_entry_is_valid(const struct l2_entry *l2)
{
	bool res = true;

	if (unlikely(ptr_is_null(l2)))
		return false;

	pr_debug("ethtype 0x%x, hdr_len %u, fallback %u, cmp_op %u\n",
		 l2->ethtype, l2->hdr_len, l2->fallback, l2->cmp_op);
	if (unlikely(!__prsr_l2_cmp_op_is_valid(l2->cmp_op)))
		res = false;
	if (unlikely(!__pp_mask_value_valid(PP_PRSR_LY2_TYPE_LEN_LY2_LEN_MSK,
					    l2->hdr_len))) {
		prsr_err("invalid l2 header length\n");
		res = false;
	}

	return res;
}

/**
 * @brief Layer 2 table entry set to HW registers WITHOUT any
 *        validation on the inputs, the caller MUST verify that the
 *        inputs are valid, for internal use only
 * @param idx table index
 * @param l2 entry info
 * @param pit entry pit table info
 * @note use NULL pointers to invalidate the entry in hw
 */
s32 prsr_l2_entry_set(u32 idx, const struct l2_entry *l2,
		      const struct pit_info *pit)
{
	struct l2_entry zero_l2  = { 0 };
	struct pit_info zero_pit = { 0 };
	u32 reg, bit;
	bool valid = true;

	pr_debug("idx %u, l2 %p, pit %p\n", idx, l2, pit);
	if (unlikely(!__prsr_l2_index_is_valid(idx))       ||
	    unlikely(l2  && !__prsr_l2_entry_is_valid(l2)) ||
	    unlikely(pit && !__prsr_pit_info_is_valid(pit)))
		return -EINVAL;

	if (unlikely(!l2 || !pit)) {
		valid = false;
		l2  = &zero_l2;
		pit = &zero_pit;
	}

	/* pit info, protocol id and length */
	reg  = PP_FIELD_PREP(PP_PRSR_LY2_TYPE_LEN_PIT_CNT_MSK, pit->cnt);
	reg |= PP_FIELD_PREP(PP_PRSR_LY2_TYPE_LEN_PIT_IDX_MSK, pit->base);
	reg |= PP_FIELD_PREP(PP_PRSR_LY2_TYPE_LEN_MSK,         l2->ethtype);
	reg |= PP_FIELD_PREP(PP_PRSR_LY2_TYPE_LEN_LY2_LEN_MSK, l2->hdr_len);
	PP_REG_WR32(PP_PRSR_LY2_TYPE_LEN_REG_IDX(idx), reg);

	spin_lock_bh(&db->lock);
	/* fallback */
	reg = PP_REG_RD32(PP_PRSR_LAST_ETHERTYPE_MAP_REG);
	bit = idx + PP_PRSR_LAST_ETHERTYPE_MAP_LY2_FALLBACK_OFF;
	reg = PP_FIELD_MOD(BIT(bit), l2->fallback, reg);
	PP_REG_WR32(PP_PRSR_LAST_ETHERTYPE_MAP_REG, reg);

	/* compare operation */
	reg = PP_REG_RD32(PP_PRSR_LY2_VALID_LEN_REG);
	bit = idx + PP_PRSR_LY2_VALID_LEN_LY2_IS_LEN_OFF;
	reg = PP_FIELD_MOD(BIT(bit), l2->cmp_op, reg);
	/* invalidate/validate */
	bit = idx + PP_PRSR_LY2_VALID_LEN_LY2_IS_VALID_OFF;
	reg = PP_FIELD_MOD(BIT(bit), valid, reg);
	PP_REG_WR32(PP_PRSR_LY2_VALID_LEN_REG, reg);
	spin_unlock_bh(&db->lock);

	return 0;
}

s32 prsr_l2_entry_get(u16 idx, bool *valid, struct l2_entry *l2,
		      struct pit_info *pit)
{
	u32 reg, bit;

	pr_debug("idx %u, l2 %p, pit %p\n", idx, l2, pit);
	if (unlikely(!__prsr_l2_index_is_valid(idx)))
		return -EINVAL;
	if (unlikely(!(!ptr_is_null(valid) &&
		       !ptr_is_null(l2)    &&
		       !ptr_is_null(pit))))
		return -EINVAL;

	/* pit info, protocol id and length */
	reg = PP_REG_RD32(PP_PRSR_LY2_TYPE_LEN_REG_IDX(idx));
	pit->cnt      = PP_FIELD_GET(PP_PRSR_LY2_TYPE_LEN_PIT_CNT_MSK, reg);
	pit->base   = PP_FIELD_GET(PP_PRSR_LY2_TYPE_LEN_PIT_IDX_MSK, reg);
	l2->ethtype = PP_FIELD_GET(PP_PRSR_LY2_TYPE_LEN_MSK,         reg);
	l2->hdr_len = PP_FIELD_GET(PP_PRSR_LY2_TYPE_LEN_LY2_LEN_MSK, reg);

	/* fallback */
	reg = PP_REG_RD32(PP_PRSR_LAST_ETHERTYPE_MAP_REG);
	bit = idx + PP_PRSR_LAST_ETHERTYPE_MAP_LY2_FALLBACK_OFF;
	l2->fallback = PP_FIELD_GET(BIT(bit), reg);

	/* compare operation */
	reg = PP_REG_RD32(PP_PRSR_LY2_VALID_LEN_REG);
	bit = idx + PP_PRSR_LY2_VALID_LEN_LY2_IS_LEN_OFF;
	l2->cmp_op = PP_FIELD_GET(BIT(bit), reg);

	/* invalidate/validate */
	bit = idx + PP_PRSR_LY2_VALID_LEN_LY2_IS_VALID_OFF;
	*valid = PP_FIELD_GET(BIT(bit), reg);

	return 0;
}

/**
 * @brief Add a new layer 2 table entry
 * @param data data pointer to attach the entry
 * @param l2 entry info
 * @param pit entry pit table info
 * @param l2_idx pointer to write the l2 idx
 * @return s32 0 on success, error code otherwise
 */
static s32 __prsr_l2_entry_add(const char *name, const struct l2_entry *l2,
			       const struct pit_info *pit, u16 *idx)
{
	s32 ret;

	ret = table_alloc_entries(db->l2_tbl, name, NULL, 1, idx, false);
	if (unlikely(ret))
		return ret;

	if (unlikely(!__prsr_l2_index_is_valid(*idx))) {
		prsr_err("invalid l2 idx %u -- entry alloc issue\n", *idx);
		return -ENOSPC;
	}

	prsr_l2_entry_set(*idx, l2, pit);

	return 0;
}

/**
 * @brief Delete layer 2 table entry
 * @param idx entry index to delete
 * @return s32 0 on success, error code otherwise
 */
static s32 __prsr_l2_entry_del(u16 idx)
{
	s32 ret;

	pr_debug("free entry %u\n", idx);
	ret = table_free_entry(db->l2_tbl, idx);
	if (unlikely(ret))
		return ret;

	prsr_l2_entry_set(idx, NULL, NULL);

	return 0;
}

/* ========================================================================== *
 *                      Instruction Memory (IMEM) Table                       *
 * ========================================================================== *
 */
/**
 * @brief Test whether imem entry is valid or not
 * @param idx entry index
 * @param ent entry to test
 * @return bool true in case the entry is valid, false otherwise
 */
static bool __prsr_imem_entry_is_valid(u16 idx, const struct imem_entry *ent)
{
	bool res = true;

	if (unlikely(ptr_is_null(ent)))
		return false;
	if (unlikely(!__prsr_pit_info_is_valid(&ent->pit)))
		res = false;
	if (unlikely(!__prsr_imem_index_is_valid(idx)))
		res = false;
	if (unlikely(!__pp_mask_value_valid(PP_PRSR_IMEM1_HEADER_LENGTH_MSK,
					    ent->hdr_len))) {
		prsr_err("invalid hdr_len\n");
		res = false;
	}
	/* next protocol */
	if (unlikely(!__pp_mask_value_valid(PP_PRSR_IMEM1_FLD0_SEL_MSK,
					    ent->np_off))) {
		prsr_err("invalid next protocol field offset\n");
		res = false;
	}
	if (unlikely(!__pp_mask_value_valid(PP_PRSR_IMEM1_FLD0_WIDTH_MSK,
					    ent->np_sz))) {
		prsr_err("invalid next protocol field length size\n");
		res = false;
	}
	if (unlikely(!__prsr_imem_index_is_valid(ent->np_dflt_im0))) {
		prsr_err("invalid next protocol default imem entry\n");
		res = false;
	}
	if (unlikely(!__prsr_np_logic_is_valid(ent->np_logic)))
		res = false;
	/* header length */
	if (unlikely(!__pp_mask_value_valid(PP_PRSR_IMEM0_FLD1_SEL_MSK,
					    ent->hdr_len_off))) {
		prsr_err("invalid header length field offset\n");
		res = false;
	}
	if (unlikely(!__pp_mask_value_valid(PP_PRSR_IMEM0_FLD1_WIDTH_MSK,
					    ent->hdr_len_sz))) {
		prsr_err("invalid header length field size\n");
		res = false;
	}
	if (unlikely(!__pp_mask_value_valid(PP_PRSR_IMEM0_IM1_MSK,
					    ent->hdr_len_im1))) {
		prsr_err("invalid header length immediate\n");
		res = false;
	}
	if (unlikely(!__prsr_entry_type_is_valid(ent->entry_type)))
		res = false;
	if (unlikely(!__prsr_skip_logic_is_valid(ent->skip_logic)))
		res = false;
	if (unlikely(!__prsr_alu_opcode_is_valid(ent->skip_op)))
		res = false;

	/* if header check is enable but max and min are wrong */
	if (unlikely(ent->hdr_len_chck) &&
	    unlikely(ent->hdr_len_max < ent->hdr_len_min)) {
		prsr_err("invalid hdr_len_min %u and hdr_len_max %u\n",
			 ent->hdr_len_min, ent->hdr_len_max);
		res = false;
	}
	/* replacement */
	if (unlikely(!ent->rep_valid)) {
		prsr_err("replacement must be valid\n");
		res = false;
	}
	if (unlikely(ent->rep_valid && ent->rep_idx != idx)) {
		prsr_err("replacement idx %u must be equal to entry index %u\n",
			 ent->rep_idx, idx);
		res = false;
	}

	return res;
}

s32 prsr_imem_entry_set(u16 idx, const struct imem_entry *ent)
{
	struct imem_entry zero_e = { 0 };
	u32 reg;

	pr_debug("idx %u, ent %p\n", idx, ent);
	if (ent && unlikely(!__prsr_imem_entry_is_valid(idx, ent)))
		return -EINVAL;

	ent = ent ? ent : &zero_e;
	/* next protocol info */
	reg  = PP_FIELD_PREP(PP_PRSR_IMEM1_HEADER_LENGTH_MSK, ent->hdr_len);
	reg |= PP_FIELD_PREP(PP_PRSR_IMEM1_FLD0_SEL_MSK,      ent->np_off);
	reg |= PP_FIELD_PREP(PP_PRSR_IMEM1_FLD0_WIDTH_MSK,    ent->np_sz);
	reg |= PP_FIELD_PREP(PP_PRSR_IMEM1_IM0_MSK,           ent->np_dflt_im0);
	PP_REG_WR32(PP_PRSR_IMEM1_REG_IDX(idx), reg);
	/* header length info */
	reg  = PP_FIELD_PREP(PP_PRSR_IMEM0_FLD1_SEL_MSK,    ent->hdr_len_off);
	reg |= PP_FIELD_PREP(PP_PRSR_IMEM0_FLD1_WIDTH_MSK,  ent->hdr_len_sz);
	reg |= PP_FIELD_PREP(PP_PRSR_IMEM0_SKIP_OPCODE_MSK, ent->skip_op);
	reg |= PP_FIELD_PREP(PP_PRSR_IMEM0_IM1_MSK,         ent->hdr_len_im1);
	reg |= PP_FIELD_PREP(PP_PRSR_IMEM0_NP_SEL_MSK,      ent->np_logic);
	reg |= PP_FIELD_PREP(PP_PRSR_IMEM0_SKIP_SEL_MSK,    ent->skip_logic);
	PP_REG_WR32(PP_PRSR_IMEM0_REG_IDX(idx), reg);
	/* header length min max values */
	reg = PP_FIELD_PREP(PP_PRSR_IMEM_PROT_LEN_CHCK_EN_MSK,
			    ent->hdr_len_chck);
	reg |= PP_FIELD_PREP(PP_PRSR_IMEM_PROT_LEN_MIN_LEN_MSK,
			     ent->hdr_len_min);
	reg |= PP_FIELD_PREP(PP_PRSR_IMEM_PROT_LEN_MAX_LEN_MSK,
			     ent->hdr_len_max);
	PP_REG_WR32(PP_PRSR_IMEM_PROT_LEN_REG_IDX(idx), reg);
	/* don't care */
	spin_lock_bh(&db->lock);
	reg = PP_REG_RD32(PRSR_IMEM_IDX_TO_RDC_REG(idx));
	reg = PP_FIELD_MOD(PRSR_IMEM_IDX_TO_RDC_BIT(idx), ent->dont_care, reg);
	PP_REG_WR32(PRSR_IMEM_IDX_TO_RDC_REG(idx), reg);
	spin_unlock_bh(&db->lock);
	/* fallback & pit & replacement */
	reg  = PP_FIELD_PREP(PP_PRSR_IMEM_ENTRY_REPLACEMENT_MSK, ent->rep_idx);
	reg |= PP_FIELD_PREP(PP_PRSR_IMEM_ENTRY_REPLACEMENT_VLD_MSK,
			     ent->rep_valid);
	reg |= PP_FIELD_PREP(PP_PRSR_IMEM_ENTRY_PIT_CNT_MSK,  ent->pit.cnt);
	reg |= PP_FIELD_PREP(PP_PRSR_IMEM_ENTRY_PIT_IDX_MSK,  ent->pit.base);
	reg |= PP_FIELD_PREP(PP_PRSR_IMEM_ENTRY_FALLBACK_MSK, ent->fallback);
	reg |= PP_FIELD_PREP(PP_PRSR_IMEM_ENTRY_ENTRY_TYPE_MSK,
			     ent->entry_type);
	PP_REG_WR32(PP_PRSR_IMEM_ENTRY_REG_IDX(idx), reg);

	return 0;
}

s32 prsr_imem_entry_get(u16 idx, struct imem_entry *ent)
{
	u32 reg;

	pr_debug("idx %u, ent %p\n", idx, ent);
	if (unlikely(ptr_is_null(ent)))
		return -EINVAL;
	if (unlikely(!__prsr_imem_index_is_valid(idx)))
		return -EINVAL;

	/* next protocol info */
	reg = PP_REG_RD32(PP_PRSR_IMEM1_REG_IDX(idx));
	ent->hdr_len = PP_FIELD_GET(PP_PRSR_IMEM1_HEADER_LENGTH_MSK, reg);
	ent->np_off  = PP_FIELD_GET(PP_PRSR_IMEM1_FLD0_SEL_MSK,      reg);
	ent->np_sz   = PP_FIELD_GET(PP_PRSR_IMEM1_FLD0_WIDTH_MSK,    reg);
	ent->np_dflt_im0 = PP_FIELD_GET(PP_PRSR_IMEM1_IM0_MSK,       reg);
	/* header length info */
	reg = PP_REG_RD32(PP_PRSR_IMEM0_REG_IDX(idx));
	ent->hdr_len_off = PP_FIELD_GET(PP_PRSR_IMEM0_FLD1_SEL_MSK,    reg);
	ent->hdr_len_sz  = PP_FIELD_GET(PP_PRSR_IMEM0_FLD1_WIDTH_MSK,  reg);
	ent->skip_op     = PP_FIELD_GET(PP_PRSR_IMEM0_SKIP_OPCODE_MSK, reg);
	ent->hdr_len_im1 = PP_FIELD_GET(PP_PRSR_IMEM0_IM1_MSK,         reg);
	ent->np_logic    = PP_FIELD_GET(PP_PRSR_IMEM0_NP_SEL_MSK,      reg);
	ent->skip_logic  = PP_FIELD_GET(PP_PRSR_IMEM0_SKIP_SEL_MSK,    reg);
	/* header length min max values */
	reg = PP_REG_RD32(PP_PRSR_IMEM_PROT_LEN_REG_IDX(idx));
	ent->hdr_len_chck = PP_FIELD_GET(PP_PRSR_IMEM_PROT_LEN_CHCK_EN_MSK,
					 reg);
	ent->hdr_len_min  = PP_FIELD_GET(PP_PRSR_IMEM_PROT_LEN_MIN_LEN_MSK,
					 reg);
	ent->hdr_len_max  = PP_FIELD_GET(PP_PRSR_IMEM_PROT_LEN_MAX_LEN_MSK,
					 reg);
	/* don't care */
	reg = PP_REG_RD32(PRSR_IMEM_IDX_TO_RDC_REG(idx));
	ent->dont_care = PP_FIELD_GET(PRSR_IMEM_IDX_TO_RDC_BIT(idx), reg);
	/* fallback & pit & replacement */
	reg = PP_REG_RD32(PP_PRSR_IMEM_ENTRY_REG_IDX(idx));
	ent->rep_idx    = PP_FIELD_GET(PP_PRSR_IMEM_ENTRY_REPLACEMENT_MSK, reg);
	ent->pit.cnt    = PP_FIELD_GET(PP_PRSR_IMEM_ENTRY_PIT_CNT_MSK, reg);
	ent->pit.base   = PP_FIELD_GET(PP_PRSR_IMEM_ENTRY_PIT_IDX_MSK, reg);
	ent->fallback   = PP_FIELD_GET(PP_PRSR_IMEM_ENTRY_FALLBACK_MSK, reg);
	ent->entry_type = PP_FIELD_GET(PP_PRSR_IMEM_ENTRY_ENTRY_TYPE_MSK, reg);
	ent->rep_valid  = PP_FIELD_GET(PP_PRSR_IMEM_ENTRY_REPLACEMENT_VLD_MSK,
				       reg);

	return 0;
}

/**
 * @brief Add a new IMEM table entry
 * @param proto reference to the protocol which create the entry
 * @param name name to attach to the entry
 * @param ent entry to add
 * @param idx entry index, set to 'TABLE_ENTRY_INVALID_IDX' for
 *            dynamic index allocation
 * @return s32 0 on success, error code otherwise
 */
static s32 __prsr_imem_entry_add(const struct proto_db_e *proto,
				 const char *name, struct imem_entry *ent,
				 u16 *idx)
{
	s32 ret;

	ret = table_alloc_entries(db->imem_tbl, name, proto, 1, idx, false);
	if (unlikely(ret))
		return ret;

	if (unlikely(!__prsr_imem_index_is_valid(*idx))) {
		prsr_err("invalid imem idx %u -- entry alloc issue\n", *idx);
		return -ENOSPC;
	}

	/* according to silicon requirements, replacement must always be valid
	 * and replacement index must be equal to the entry index
	 */
	ent->rep_valid = true;
	ent->rep_idx   = *idx;

	return prsr_imem_entry_set(*idx, ent);
}

/**
 * @brief Delete IMEM table entry
 * @param idx entry index to delete
 * @return s32 0 on success, error code otherwise
 */
static s32 __prsr_imem_entry_del(u16 idx)
{
	s32 ret;

	pr_debug("free entry %u\n", idx);
	ret = table_free_entry(db->imem_tbl, idx);
	if (unlikely(ret))
		return ret;

	/* invalidate entry in HW */
	return prsr_imem_entry_set(idx, NULL);
}

/* ========================================================================== *
 *                      Last Ethertype Table                                  *
 * ========================================================================== *
 */

/**
 * @brief Test whether ethertype entry is valid or not
 * @param ent entry to test
 * @return bool true in case the entry is valid, false otherwise
 */
static bool
__prsr_last_ethertype_entry_is_valid(const struct last_ethertype_entry *ent)
{
	bool res = true;

	if (unlikely(ptr_is_null(ent)))
		return false;

	pr_debug("proto_id 0x%x, imem_idx %u\n", ent->proto_id, ent->imem_idx);
	if (unlikely(!__prsr_imem_index_is_valid(ent->imem_idx)))
		res = false;

	return res;
}

/**
 * @brief Set last ethertype table entry to HW registers WITHOUT any
 *        validation on the inputs, the caller MUST verify that the
 *        inputs are valid, for internal use only
 *
 * @param idx entry index
 * @param ent entry info, if NULL is provided, the entry will be
 *          initialized
 */
s32 prsr_last_ethertype_entry_set(u16 idx,
				  const struct last_ethertype_entry *ent)
{
	struct last_ethertype_entry zero_e = { 0 };
	bool valid = true;
	u32 reg;

	if (unlikely(!__prsr_last_ethertype_index_is_valid(idx)) ||
	    unlikely(ent && !__prsr_last_ethertype_entry_is_valid(ent)))
		return -EINVAL;

	if (!ent) {
		ent   = &zero_e;
		valid = false;
	}
	reg  = PP_FIELD_PREP(PP_PRSR_LAST_ETHERTYPE_CFG_ET_IS_VALID_MSK,
			     valid);
	reg |= PP_FIELD_PREP(PP_PRSR_LAST_ETHERTYPE_CFG_LY3_PC_MSK,
			     ent->imem_idx);
	reg |= PP_FIELD_PREP(PP_PRSR_LAST_ETHERTYPE_CFG_ET_VALUE_MSK,
			     ent->proto_id);
	PP_REG_WR32(PP_PRSR_LAST_ETHERTYPE_CFG_REG_IDX(idx), reg);

	return 0;
}

s32 prsr_last_ethertype_entry_get(u16 idx, bool *valid,
				  struct last_ethertype_entry *ent)
{
	u32 reg;

	if (unlikely(!(!ptr_is_null(ent) &&
		       !ptr_is_null(valid))))
		return -EINVAL;
	if (unlikely(!__prsr_last_ethertype_index_is_valid(idx)))
		return -EINVAL;

	reg = PP_REG_RD32(PP_PRSR_LAST_ETHERTYPE_CFG_REG_IDX(idx));
	*valid = PP_FIELD_GET(PP_PRSR_LAST_ETHERTYPE_CFG_ET_IS_VALID_MSK, reg);
	ent->imem_idx = PP_FIELD_GET(PP_PRSR_LAST_ETHERTYPE_CFG_LY3_PC_MSK,
				     reg);
	ent->proto_id = PP_FIELD_GET(PP_PRSR_LAST_ETHERTYPE_CFG_ET_VALUE_MSK,
				     reg);

	return 0;
}

/**
 * @brief Add a new last ethertype entry
 * @param data data pointer to attach the entry
 * @param ent entry info
 * @param idx buffer to store the index
 * @return s32 0 on success, error code otherwise
 */
static s32
__prsr_last_ethertype_entry_add(const struct last_ethertype_entry *ent,
				const char *name, u16 *idx)
{
	s32 ret;

	*idx = TABLE_ENTRY_INVALID_IDX;
	ret = table_alloc_entries(db->last_eth_tbl, name, NULL, 1, idx, false);
	if (unlikely(ret))
		return ret;

	if (unlikely(!__prsr_last_ethertype_index_is_valid(*idx))) {
		prsr_err("invalid last ethertype idx %u -- entry alloc issue\n",
			 *idx);
		return -ENOSPC;
	}
	prsr_last_ethertype_entry_set(*idx, ent);

	return 0;
}

/**
 * @brief Delete an existing last ethertype entry
 * @param idx entry index to delete
 * @return s32 0 on success, error code otherwise
 */
static s32 __prsr_last_ethertype_entry_del(u16 idx)
{
	s32 ret;

	pr_debug("free entry %u\n", idx);
	ret = table_free_entry(db->last_eth_tbl, idx);
	if (unlikely(ret))
		return ret;

	/* invalidate entry in HW */
	prsr_last_ethertype_entry_set(idx, NULL);

	return 0;
}

/*
 * ========================================================================== *
 *                      Next Protocol Select (NP_SEL) Table                   *
 * ========================================================================== *
 */
/**
 * @brief Test whether np select entry is valid or not
 * @param ent entry to test
 * @return bool true if the entry is valid, false otherwise
 */
static bool __prsr_np_sel_entry_is_valid(const struct np_sel_entry *ent)
{
	bool res = true;

	if (unlikely(ptr_is_null(ent)))
		return false;

	pr_debug("key %#x, imem_idx 0x%x\n",
		 ent->key, ent->imem_idx);
	if (unlikely(!__prsr_imem_index_is_valid(ent->imem_idx)))
		res = false;
	if (unlikely(!__pp_mask_value_valid(PP_PRSR_NP_SEL_KEY_MSK, ent->key)))
		res = false;

	return res;
}

s32 prsr_np_sel_entry_set(u16 idx, const struct np_sel_entry *ent)
{
	struct np_sel_entry zero_e = { 0 };
	bool valid = true;
	u32 reg;

	if (unlikely(!__prsr_np_sel_index_is_valid(idx)) ||
	    unlikely(ent && !__prsr_np_sel_entry_is_valid(ent)))
		return -EINVAL;

	if (!ent) {
		ent = &zero_e;
		valid = false;
	}
	reg  = PP_FIELD_PREP(PP_PRSR_NP_SEL_KEY_MSK,   ent->key);
	reg |= PP_FIELD_PREP(PP_PRSR_NP_SEL_VALUE_MSK, ent->imem_idx);
	reg |= PP_FIELD_PREP(PP_PRSR_NP_SEL_VALID_MSK, valid);
	PP_REG_WR32(PP_PRSR_NP_SEL_REG_IDX(idx), reg);

	return 0;
}

s32 prsr_np_sel_entry_get(u16 idx, bool *valid, struct np_sel_entry *ent)
{
	u32 reg;

	if (unlikely(!(!ptr_is_null(ent) &&
		       !ptr_is_null(valid))))
		return -EINVAL;
	if (unlikely(!__prsr_np_sel_index_is_valid(idx)))
		return -EINVAL;

	reg           = PP_REG_RD32(PP_PRSR_NP_SEL_REG_IDX(idx));
	ent->key      = PP_FIELD_GET(PP_PRSR_NP_SEL_KEY_MSK,   reg);
	ent->imem_idx = PP_FIELD_GET(PP_PRSR_NP_SEL_VALUE_MSK, reg);
	*valid        = PP_FIELD_GET(PP_PRSR_NP_SEL_VALID_MSK, reg);

	return 0;
}

/**
 * @brief Add a new next protocol entry
 * @param ent entry info
 * @param name string describing the entry
 * @param idx buffer to store the index
 * @return s32 0 on success, error code otherwise
 */
static s32 __prsr_np_sel_entry_add(const struct np_sel_entry *ent,
				   const char *name, u16 *idx)
{
	s32  ret;

	if (unlikely(!(!ptr_is_null(ent) &&
		       !ptr_is_null(idx))))
		return -EINVAL;

	*idx = TABLE_ENTRY_INVALID_IDX;
	ret = table_alloc_entries(db->np_sel_tbl, name, NULL, 1, idx, false);
	if (unlikely(ret))
		return ret;

	if (unlikely(!__prsr_np_sel_index_is_valid(*idx))) {
		prsr_err("invalid np select idx %u -- entry alloc issue\n",
			 *idx);
		return -ENOSPC;
	}
	prsr_np_sel_entry_set(*idx, ent);

	return 0;
}

/**
 * @brief Delete an existing np select table entry
 * @param idx entry index to delete
 * @return s32 0 on success, error code otherwise
 */
static s32 __prsr_np_sel_entry_del(u16 idx)
{
	s32 ret;

	pr_debug("free entry %u\n", idx);
	ret = table_free_entry(db->np_sel_tbl, idx);
	if (unlikely(ret))
		return ret;

	/* invalidate entry in HW */
	prsr_np_sel_entry_set(idx, NULL);

	return 0;
}

/* ========================================================================== *
 *                      Skip CAM Table                                        *
 * ========================================================================== *
 */
/**
 * @brief Test whether skip table entry is valid or not
 * @param ent entry to test
 * @return bool true if the entry is valid, false otherwise
 */
static bool __prsr_skip_entry_is_valid(const struct skip_entry *ent)
{
	bool res = true;

	if (unlikely(ptr_is_null(ent)))
		return false;

	pr_debug("key 0x%x, header len %u\n",
		 ent->key, ent->hdr_len);
	if (unlikely(!__pp_mask_value_valid(PP_PRSR_SKIP_LU_KEY_MSK, ent->key)))
		res = false;
	if (unlikely(!__pp_mask_value_valid(PP_PRSR_SKIP_LU_VALUE_MSK,
					    ent->hdr_len)))
		res = false;

	return res;
}

s32 prsr_skip_entry_set(u16 idx, const struct skip_entry *ent)
{
	struct skip_entry zero_e = { 0 };
	bool valid = true;
	u32 reg;

	if (unlikely(!__prsr_skip_index_is_valid(idx)) ||
	    unlikely(ent && !__prsr_skip_entry_is_valid(ent)))
		return -EINVAL;

	if (!ent) {
		ent   = &zero_e;
		valid = false;
	}
	reg  = PP_FIELD_PREP(PP_PRSR_SKIP_LU_KEY_MSK,   ent->key);
	reg |= PP_FIELD_PREP(PP_PRSR_SKIP_LU_VALUE_MSK, ent->hdr_len);
	reg |= PP_FIELD_PREP(PP_PRSR_SKIP_LU_VALID_MSK, valid);
	PP_REG_WR32(PP_PRSR_SKIP_LU_REG_IDX(idx), reg);

	return 0;
}

s32 prsr_skip_entry_get(u16 idx, bool *valid, struct skip_entry *ent)
{
	u32 reg;

	if (unlikely(!(!ptr_is_null(ent) &&
		       !ptr_is_null(valid))))
		return -EINVAL;
	if (unlikely(!__prsr_skip_index_is_valid(idx)))
		return -EINVAL;

	reg          = PP_REG_RD32(PP_PRSR_SKIP_LU_REG_IDX(idx));
	ent->key     = PP_FIELD_GET(PP_PRSR_SKIP_LU_KEY_MSK,   reg);
	ent->hdr_len = PP_FIELD_GET(PP_PRSR_SKIP_LU_VALUE_MSK, reg);
	*valid       = PP_FIELD_GET(PP_PRSR_SKIP_LU_VALID_MSK, reg);

	return 0;
}

/**
 * @brief Add a skip CAM (skip lu) entry
 * @param ent entry info
 * @param name string describing the entry
 * @param idx buffer to store the index
 * @return s32 0 on success, error code otherwise
 */
static s32 __prsr_skip_entry_add(const struct skip_entry *ent,
				 const char *name, u16 *idx)
{
	s32 ret;

	if (unlikely(ptr_is_null(ent)))
		return -EINVAL;

	*idx = TABLE_ENTRY_INVALID_IDX;
	ret = table_alloc_entries(db->skip_tbl, name, NULL, 1, idx, false);
	if (unlikely(ret))
		return ret;

	if (unlikely(!__prsr_skip_index_is_valid(*idx))) {
		prsr_err("invalid skip cam idx %u -- entry alloc issue\n",
			 *idx);
		return -ENOSPC;
	}
	prsr_skip_entry_set(*idx, ent);

	return 0;
}

/**
 * @brief Delete an existing skip tables entry
 * @param idx entry index to delete
 * @return s32 0 on success, error code otherwise
 */
static s32 __prsr_skip_entry_del(u16 idx)
{
	s32 ret;

	pr_debug("free entry %u\n", idx);
	ret = table_free_entry(db->skip_tbl, idx);
	if (unlikely(ret))
		return ret;

	/* invalidate entry in HW */
	prsr_skip_entry_set(idx, NULL);

	return 0;
}

/* ========================================================================== *
 *                      Merge CAM Table                                       *
 * ========================================================================== *
 */
/**
 * @brief Test whether merge entry is valid or not
 * @param ent entry to test
 * @return bool true if the entry is valid, false otherwise
 */
static bool __prsr_merge_entry_is_valid(const struct merge_entry *ent)
{
	bool res = true;

	if (unlikely(ptr_is_null(ent)))
		return false;

	pr_debug("previous %u, current %u, merged %u\n",
		 ent->prev, ent->curr, ent->merged);
	if (unlikely(!__prsr_imem_index_is_valid(ent->prev))) {
		prsr_err("invalid prev %u\n", ent->prev);
		res = false;
	}
	if (unlikely(!__prsr_imem_index_is_valid(ent->curr))) {
		prsr_err("invalid curr %u\n", ent->curr);
		res = false;
	}
	if (unlikely(!__prsr_imem_index_is_valid(ent->merged))) {
		prsr_err("invalid merged %u\n", ent->merged);
		res = false;
	}

	return res;
}

s32 prsr_merge_entry_set(u16 idx, const struct merge_entry *ent)
{
	struct merge_entry zero_e = { 0 };
	bool valid = true;
	u32 reg;

	if (unlikely(!__prsr_merge_index_is_valid(idx)) ||
	    unlikely(ent && !__prsr_merge_entry_is_valid(ent)))
		return -EINVAL;

	if (!ent) {
		ent = &zero_e;
		valid = false;
	}
	reg  = PP_FIELD_PREP(PP_PRSR_MERGE_PC_MSK,    ent->prev);
	reg |= PP_FIELD_PREP(PP_PRSR_MERGE_PC_CP_MSK, ent->curr);
	reg |= PP_FIELD_PREP(PP_PRSR_MERGE_PC_MP_MSK, ent->merged);
	reg |= PP_FIELD_PREP(PP_PRSR_MERGE_PC_VALID_MSK, valid);
	PP_REG_WR32(PP_PRSR_MERGE_PC_REG_IDX(idx), reg);

	return 0;
}

s32 prsr_merge_entry_get(u16 idx, bool *valid, struct merge_entry *ent)
{
	u32 reg;

	if (unlikely(!(!ptr_is_null(ent) &&
		       !ptr_is_null(valid))))
		return -EINVAL;
	if (unlikely(!__prsr_merge_index_is_valid(idx)))
		return -EINVAL;

	reg    = PP_REG_RD32(PP_PRSR_MERGE_PC_REG_IDX(idx));
	ent->prev   = PP_FIELD_GET(PP_PRSR_MERGE_PC_MSK,       reg);
	ent->curr   = PP_FIELD_GET(PP_PRSR_MERGE_PC_CP_MSK,    reg);
	ent->merged = PP_FIELD_GET(PP_PRSR_MERGE_PC_MP_MSK,    reg);
	*valid      = PP_FIELD_GET(PP_PRSR_MERGE_PC_VALID_MSK, reg);

	return 0;
}

/**
 * @brief Add a merge table entry
 * @param ent entry info
 * @param name string describing the entry
 * @param idx buffer to store the index
 * @return s32 0 on success, error code otherwise
 */
static s32 __prsr_merge_entry_add(const struct merge_entry *ent,
				  const char *name, u16 *idx)
{
	s32 ret;

	if (unlikely(!(!ptr_is_null(ent) &&
		       !ptr_is_null(idx))))
		return -EINVAL;

	*idx = TABLE_ENTRY_INVALID_IDX;
	ret = table_alloc_entries(db->merge_tbl, name, NULL, 1, idx, false);
	if (unlikely(ret))
		return ret;

	if (unlikely(!__prsr_merge_index_is_valid(*idx))) {
		prsr_err("invalid merge idx %u -- entry alloc issue\n",
			 *idx);
		return -ENOSPC;
	}
	prsr_merge_entry_set(*idx, ent);

	return 0;
}

/**
 * @brief Delete an existing merge table entry
 * @param idx entry index to delete
 * @return s32 0 on success, error code otherwise
 */
static s32 __prsr_merge_entry_del(u16 idx)
{
	s32 ret;

	pr_debug("free entry %u\n", idx);
	ret = table_free_entry(db->merge_tbl, idx);
	if (unlikely(ret))
		return ret;

	/* invalidate entry in HW */
	prsr_merge_entry_set(idx, NULL);

	return 0;
}

/* ========================================================================== *
 *                      Ports Configuration Table                             *
 * ========================================================================== *
 */
/**
 * @brief Port entry validaty test
 * @param ent the entry
 * @return bool true if entry is valid, false otherwise
 */
static bool prsr_port_entry_is_valid(struct port_cfg_entry *ent)
{
	struct port_clsf_fld *fld;
	bool res = true;
	u32 i, tot_len, max_len, min_off, max_off;

	if (unlikely(ptr_is_null(ent)))
		return -EINVAL;
	if (unlikely(!__prsr_imem_index_is_valid(ent->skip_l2_pc))) {
		prsr_err("invalid skip_l2_pc\n");
		res = false;
	}

	tot_len  = 0;
	max_len  = FIELD_SIZEOF(struct pp_fv_oob, stw_data) * BITS_PER_BYTE - 1;
	min_off  = offsetof(struct pp_fv_oob, stw_data)  * BITS_PER_BYTE;
	max_off  = min_off + max_len - 1;
	for (i = 0; i < ARRAY_SIZE(ent->fld); i++) {
		fld = &ent->fld[i];
		if (!fld->len)
			continue;

		if (unlikely(fld->stw_off > PP_STW_CLS_MAX_BIT)) {
			prsr_err("invalid status word offset %u at idx %u\n",
				 fld->stw_off, i);
			res = false;
		}
		if (unlikely(!(fld->fv_off >= min_off &&
			       fld->fv_off <= max_off))) {
			prsr_err("invalid field vector offset %u at idx %u\n",
				 fld->fv_off, i);
			res = false;
		}
		if (unlikely(fld->len > max_len)) {
			prsr_err("invalid length %u at idx %u\n", fld->len, i);
			res = false;
		}
		if (unlikely((fld->fv_off + fld->len) > max_off)) {
			prsr_err("fld %u, off %u, len %u, exceeds field vector max_off %u\n",
				 i, fld->fv_off, fld->len, max_off);
			res = false;
		}
		tot_len += fld->len;
	}
	/* make sure all field will not overwrite the l2 */
	if (unlikely(max_len < tot_len)) {
		prsr_err("all fields length %u exceeds FV STW data length of %u\n",
			 tot_len, max_len);
		res = false;
	}

	return res;
}

s32 prsr_port_cfg_entry_set(u16 port, struct port_cfg_entry *cfg)
{
	struct port_clsf_fld *fld;
	u32 reg, hack[2], cmd = 0;
	u32 i;

	if (unlikely(!__pp_is_port_valid(port) ||
		     !prsr_port_entry_is_valid(cfg)))
		return -EINVAL;

	/* fallback and no l2 parsing */
	reg  = PP_FIELD_PREP(PP_PRSR_PORT_CFG_PORT_FALLBACK_MSK, cfg->fallback);
	reg |= PP_FIELD_PREP(PP_PRSR_PORT_CFG_SKIP_LY2_PC_MSK, cfg->skip_l2_pc);

	memset(hack, 0, sizeof(hack));
	/* status word classification fields */
	for (i = 0; i < ARRAY_SIZE(cfg->fld); i++) {
		fld = &cfg->fld[i];
		hack[i >> 1] |=
			PP_FIELD_PREP(PP_PRSR_PORT_CLS_FLD_STW_OFF_MASK(i),
				      fld->stw_off);
		hack[i >> 1] |=
			PP_FIELD_PREP(PP_PRSR_PORT_CLS_FLD_FV_OFF_MASK(i),
				      fld->fv_off);
		hack[i >> 1] |=
			PP_FIELD_PREP(PP_PRSR_PORT_CLS_FLD_LEN_MASK(i),
				      fld->len);
	}

	cmd |= PP_FIELD_PREP(PP_PRSR_OOB_FV_HACK_REC_VAL2_FV_HACK_CMD_MSK,
			     PP_PRSR_PORT_CLS_WRITE_CMD);
	cmd |= PP_FIELD_PREP(PP_PRSR_OOB_FV_HACK_REC_VAL2_FV_HACK_PORT_NUM_MSK,
			     port);

	pr_debug("reg = %#x, hack0 %#x, hack1 %#x, cmd %#x\n",
		 reg, hack[0], hack[1], cmd);

	/* write everything */
	PP_REG_WR32(PP_PRSR_PORT_CFG_REG_IDX(port), reg);
	spin_lock_bh(&db->lock);
	PP_REG_WR32(PP_PRSR_OOB_FV_HACK_REC_VAL0_REG, hack[0]);
	PP_REG_WR32(PP_PRSR_OOB_FV_HACK_REC_VAL1_REG, hack[1]);
	PP_REG_WR32(PP_PRSR_OOB_FV_HACK_REC_VAL2_REG, cmd);
	spin_unlock_bh(&db->lock);

	return 0;
}

s32 prsr_port_cfg_entry_get(u16 port, struct port_cfg_entry *cfg)
{
	struct port_clsf_fld *fld;
	u32 reg, hack[2], cmd = 0;
	u32 i;

	if (unlikely(ptr_is_null(cfg)))
		return -EINVAL;
	if (unlikely(!__pp_is_port_valid(port)))
		return -EINVAL;

	reg = PP_REG_RD32(PP_PRSR_PORT_CFG_REG_IDX(port));
	cfg->fallback = PP_FIELD_GET(PP_PRSR_PORT_CFG_PORT_FALLBACK_MSK, reg);
	cfg->skip_l2_pc = PP_FIELD_GET(PP_PRSR_PORT_CFG_SKIP_LY2_PC_MSK, reg);

	/* send read command */
	cmd |= PP_FIELD_PREP(PP_PRSR_OOB_FV_HACK_REC_VAL2_FV_HACK_CMD_MSK,
			     PP_PRSR_PORT_CLS_READ_CMD);
	cmd |= PP_FIELD_PREP(PP_PRSR_OOB_FV_HACK_REC_VAL2_FV_HACK_PORT_NUM_MSK,
			     port);
	spin_lock_bh(&db->lock);
	PP_REG_WR32(PP_PRSR_OOB_FV_HACK_REC_VAL2_REG, cmd);
	/* wait for the registers to be ready */
	if (pp_reg_poll(PP_PRSR_OOB_FV_HACK_REC_VAL2_REG,
			PP_PRSR_OOB_FV_HACK_REC_VAL2_FV_HACK_RD_STAT_MSK, 1)) {
		prsr_err("Failed to read port %u classification config\n",
			 port);
		spin_unlock_bh(&db->lock);
		return -EFAULT;
	}
	hack[0] = PP_REG_RD32(PP_PRSR_OOB_FV_HACK_REC_VAL0_REG);
	hack[1] = PP_REG_RD32(PP_PRSR_OOB_FV_HACK_REC_VAL1_REG);
	spin_unlock_bh(&db->lock);

	pr_debug("reg = %#x, hack0 %#x, hack1 %#x, cmd %#x\n",
		 reg, hack[0], hack[1], cmd);

	/* status word classification fields */
	for (i = 0; i < ARRAY_SIZE(cfg->fld); i++) {
		fld = &cfg->fld[i];
		fld->stw_off =
			PP_FIELD_GET(PP_PRSR_PORT_CLS_FLD_STW_OFF_MASK(i),
				     hack[i >> 1]);
		fld->fv_off =
			PP_FIELD_GET(PP_PRSR_PORT_CLS_FLD_FV_OFF_MASK(i),
				     hack[i >> 1]);
		fld->len =
			PP_FIELD_GET(PP_PRSR_PORT_CLS_FLD_LEN_MASK(i),
				     hack[i >> 1]);
	}

	return 0;
}

s32 prsr_port_cfg_set(u16 port, struct pp_port_cls_info *cls,
		      enum pp_port_parsing parse_type)
{
	struct port_cfg_entry cfg;
	u32 i, fv_off;
	s32 ret;

	memset(&cfg, 0, sizeof(cfg));
	ret = prsr_port_cfg_entry_get(port, &cfg);
	if (unlikely(ret))
		goto fail;

	switch (parse_type) {
	case L2_PARSE:
		cfg.skip_l2_pc = PRSR_PORT_NO_L2_SKIP_VAL;
		break;
	case IP_PARSE:
		cfg.skip_l2_pc = __get_proto(PRSR_PROTO_IP_SEL)->imem_idx;
		break;
	case NO_PARSE:
		cfg.skip_l2_pc = __get_proto(PRSR_PROTO_NO_PARSE)->imem_idx;
		break;
	default:
		prsr_err("invalid parse type %u\n", parse_type);
		return -EINVAL;
	}

	fv_off = offsetof(struct pp_fv, oob.stw_data) * BITS_PER_BYTE;
	for (i = 0; i < cls->n_flds; i++) {
		cfg.fld[i].stw_off = cls->cp[i].stw_off;
		cfg.fld[i].len     = cls->cp[i].copy_size;
		cfg.fld[i].fv_off  = fv_off;
		fv_off += cfg.fld[i].len;
	}

	ret = prsr_port_cfg_entry_set(port, &cfg);
	if (unlikely(ret))
		goto fail;

	return 0;
fail:
	prsr_err("Failed to set port %u configuration\n", port);
	return ret;
}

s32 prsr_port_fallback_set(u16 port, bool fallback)
{
	struct port_cfg_entry cfg;
	s32 ret;

	ret = prsr_port_cfg_entry_get(port, &cfg);
	if (unlikely(ret))
		goto fail;

	if (cfg.fallback == fallback)
		return 0;

	cfg.fallback = fallback;
	ret = prsr_port_cfg_entry_set(port, &cfg);
	if (unlikely(ret))
		goto fail;

	return 0;
fail:
	prsr_err("Failed to set port %u fallback\n", port);
	return ret;
}

/* ========================================================================== *
 *                      MAC Protocol                                          *
 * ========================================================================== *
 */
/**
 * @brief Test whether a mac protocol parameters are valid or not
 * @param params parameters to test
 * @return bool true if 'params' are valid, false otherwise
 */
static bool
__prsr_mac_proto_is_valid(const struct prsr_mac_proto_params *params)
{
	const struct field_ext_info *fld;
	bool res = true;

	if (unlikely(ptr_is_null(params)))
		return false;

	pr_debug("%s: hdr_len %u\n", params->mac_name, params->hdr_len);
	fld = &params->mac_ext.fld[0];
	if (unlikely(!__prsr_fld_ext_info_is_valid(fld))) {
		prsr_err("invalid %s field extract info\n",
			 params->mac_name);
		res = false;
	}
	fld = &params->ethtype_ext.fld[0];
	if (unlikely(!__prsr_fld_ext_info_is_valid(fld))) {
		prsr_err("invalid %s field extract info\n",
			 params->ethtype_name);
		res = false;
	}

	return res;
}

/**
 * @brief Adds ethernet protocol to the PP parser <br>
 *        Caller must be familiar with and has access to 'struct
 *        pp_field_vector'
 * @param id protocol internal parser driver id
 * @param params protocol parameters
 * @return s32 0 on success, error code on failure
 */
static s32
__prsr_mac_proto_add(enum prsr_proto_id id,
		     const struct prsr_mac_proto_params *params)
{
	struct proto_db_e *proto;
	struct pit_info ethtype_pit;
	struct pit_entry mac_pit, eth_pit;
	s32 ret;
	u32 n;

	proto = __get_proto(id);
	if (unlikely(!proto))
		return -EINVAL;
	if (unlikely(!__prsr_mac_proto_is_valid(params)))
		return -EINVAL;

	/* set database entry */
	n = strscpy(proto->name, params->mac_name, sizeof(proto->name));
	proto->id       = id;
	proto->pit.cnt  = 1;
	proto->imem_idx = TABLE_ENTRY_INVALID_IDX;
	proto->l2_idx   = TABLE_ENTRY_INVALID_IDX;
	/* ------------- */
	/*      MAC      */
	/* ------------- */
	/* create PIT table entries */
	memset(&mac_pit, 0, sizeof(mac_pit));
	__prsr_fld_ext_to_pit_entry(&params->mac_ext.fld[0], &mac_pit);
	ret = __prsr_pit_entries_add(proto->name, &mac_pit,
				     proto->pit.cnt, &proto->pit.base);
	if (unlikely(ret))
		return ret;
	/* configure MAC parameters */
	ret = prsr_mac_cfg_set(params->hdr_len, &proto->pit);
	if (unlikely(ret))
		return ret;

	/* ------------------- */
	/*      Ethertype      */
	/* ------------------- */
	/* create PIT table entry */
	ethtype_pit.cnt = 1;
	memset(&eth_pit, 0, sizeof(eth_pit));
	__prsr_fld_ext_to_pit_entry(&params->ethtype_ext.fld[0], &eth_pit);
	ret = __prsr_pit_entries_add(params->ethtype_name, &eth_pit,
				     ethtype_pit.cnt,
				     &ethtype_pit.base);
	if (unlikely(ret))
		return ret;
	/* configure MAC parameters */
	ret = prsr_ethertype_cfg_set(&ethtype_pit);
	if (unlikely(ret))
		return ret;

	/* set the entry as valid in the db */
	proto->valid = true;
	pr_debug("done\n");

	return 0;
}

/* ========================================================================== *
 *                      Layer 2 protocol                                      *
 * ========================================================================== *
 */
/**
 * @brief Test whether layer 2 protocol parameters are valid or not
 * @param params parameters to test
 * @return bool true if the 'params' are valid, false otherwise
 */
static bool __prsr_l2_proto_is_valid(const struct prsr_l2_proto_params *params)
{
	const struct field_ext_info *fld;
	bool res = true;
	u32 i;

	if (unlikely(ptr_is_null(params)))
		return params;

	pr_debug("%s: n_fld %u\n", params->name, params->ext.n_fld);
	if (unlikely(!__prsr_l2_entry_is_valid(&params->hw)))
		res = false;

	if (unlikely(params->ext.n_fld > PRSR_MAX_EXTRACT_ENTRIES)) {
		prsr_err("too much extract entries %u\n", params->ext.n_fld);
		res = false;
	}
	for (i = 0; i < params->ext.n_fld; i++) {
		fld = &params->ext.fld[i];
		if (unlikely(!__prsr_fld_ext_info_is_valid(fld)))
			res = false;
	}
	if (unlikely(params->hw_idx != TABLE_ENTRY_INVALID_IDX &&
		     !__prsr_l2_index_is_valid(params->hw_idx)))
		res = false;

	return res;
}

/**
 * @brief Adds new low level protocol to be PP parser
 *        Caller must be familiar with and has access to
 *        'struct pp_field_vector'
 * @param id protocol internal parser driver id
 * @param params protocol parameters
 * @return s32 0 on success, error code on failure
 */
static s32 __prsr_l2_proto_add(enum prsr_proto_id id,
			       const struct prsr_l2_proto_params *params)
{
	struct proto_db_e *proto;
	struct pit_entry pit[PRSR_MAX_EXTRACT_ENTRIES];
	s32 ret, n, i;

	proto = __get_proto(id);
	if (unlikely(!proto))
		return -EINVAL;
	if (unlikely(!__prsr_l2_proto_is_valid(params)))
		return -EINVAL;

	proto->pit.cnt = params->ext.n_fld;
	/* create PIT table entries */
	if (proto->pit.cnt) {
		memset(pit, 0, sizeof(pit));
		for (i = 0; i < proto->pit.cnt; i++)
			__prsr_fld_ext_to_pit_entry(&params->ext.fld[i],
						    &pit[i]);

		ret = __prsr_pit_entries_add(params->name, pit,
					     proto->pit.cnt, &proto->pit.base);
		if (unlikely(ret))
			return ret;
	}
	/* create l2 table entry */
	proto->l2_idx = params->hw_idx;
	ret = __prsr_l2_entry_add(params->name, &params->hw,
				  &proto->pit, &proto->l2_idx);
	if (unlikely(ret))
		goto fail;

	/* init database entry */
	n = strscpy(proto->name, params->name, sizeof(proto->name));
	proto->valid    = true;
	proto->id       = id;
	proto->proto_id = params->hw.ethtype;
	proto->imem_idx = TABLE_ENTRY_INVALID_IDX;

	prsr_dbg_proto_add(id);
	pr_debug("done\n");
	return 0;
fail:
	prsr_proto_del(id);
	return ret;
}

/* ========================================================================== *
 *                      Upper Level protocol                                  *
 * ========================================================================== *
 */
/**
 * @brief Test whether an upper layer protocol parameters are valid or not
 * @param id protocol internal id
 * @param params parameters to test
 * @return bool true if the 'params' are valid, false otherwise
 */
static bool
__prsr_up_layer_proto_is_valid(enum prsr_proto_id id,
			       const struct prsr_up_layer_proto_params *params)
{
	const struct field_ext_info *fld;
	const struct pre_proto *pre_proto;
	struct proto_db_e *dflt_proto;
	bool res = true;
	u16 dflt_proto_imem;
	u32 i;

	if (unlikely(ptr_is_null(params)))
		return false;

	pr_debug("%s: n_fld %u\n", params->name, params->ext.n_fld);
	if (unlikely(!__prsr_proto_id_is_valid(id))) {
		prsr_err("invalid internal id %u\n", id);
		res = false;
	}
	/* extract info */
	if (unlikely(params->ext.n_fld > PRSR_MAX_EXTRACT_ENTRIES)) {
		prsr_err("too much extract entries %u\n", params->ext.n_fld);
		res = false;
	}
	for (i = 0; i < params->ext.n_fld; i++) {
		fld = &params->ext.fld[i];
		if (unlikely(!__prsr_fld_ext_info_is_valid(fld))) {
			prsr_err("invalid field %u pit entry\n", i);
			res = false;
		}
	}

	/* desired hw index */
	if (unlikely(params->hw_idx != TABLE_ENTRY_INVALID_IDX &&
		     !__prsr_imem_index_is_valid(params->hw_idx))) {
		prsr_err("invalid hw_idx %u\n", params->hw_idx);
		res = false;
	}
	/* preceding protocol tests:*
	 * 1. in case prior protocols exists and valids, np logic must be valid
	 * 2. default next protocol MUST be valid as MUST have
	 *    a valid imem entry
	 */
	for (i = 0; i < params->n_pre; i++) {
		pre_proto = &params->pre[i];
		if (unlikely(!__prsr_np_logic_is_valid(pre_proto->np_logic))) {
			prsr_err("invalid np_logic %u at index %u\n",
				 pre_proto->np_logic, i);
			res = false;
		}
	}
	if (unlikely(!params->n_pre != 0 &&
		     !__prsr_np_logic_is_valid(params->np.logic))) {
		prsr_err("invalid np logic %u\n", params->np.logic);
		res = false;
	}
	dflt_proto = __get_proto(params->np.dflt);
	if (likely(dflt_proto)) {
		/* don't check default protocol for protocols which use their
		 * own id as their default, ONLY to support last protocol
		 * in the table
		 */
		dflt_proto_imem = dflt_proto->imem_idx;
		if (id != params->np.dflt &&
		    unlikely(!__prsr_imem_index_is_valid(dflt_proto_imem))) {
			prsr_err("invalid imem index %u for protocol id %u\n",
				 dflt_proto_imem, params->np.dflt);
			res = false;
		}
	} else {
		prsr_err("invalid default proto_id %u\n", params->np.dflt);
		res = false;
	}

	/* skip logic */
	switch (params->skip.logic) {
	case SKIP_LOGIC_ALU:
		if (unlikely(!__prsr_alu_opcode_is_valid(params->skip.op)))
			res = false;
		break;
	case SKIP_LOGIC_LOOKUP:
	case SKIP_LOGIC_IM:
		break;
	default:
		prsr_err("invalid skip logic %u, max is %u\n",
			 params->skip.logic, SKIP_LOGICS_NUM);
		res = false;
		break;
	}

	return res;
}

/**
 * @brief Convert an upper layer protocol parameters to imem entry
 * @param params parameters to convert
 * @param ent 'param' in imem entry form
 */
static void
__prsr_up_params_to_imem_entry(const struct prsr_up_layer_proto_params *params,
			       struct imem_entry *ent)
{
	struct proto_db_e *dflt_proto;

	if (unlikely(!(!ptr_is_null(params) &&
		       !ptr_is_null(ent))))
		return;

	dflt_proto = __get_proto(params->np.dflt);
	if (unlikely(!dflt_proto)) {
		pr_crit("Failed to get default protocol\n");
		return;
	}

	ent->hdr_len     = params->hdr_len;
	ent->np_off      = params->np.off + params->np.len;
	ent->np_sz       = params->np.len;
	ent->np_dflt_im0 = dflt_proto->imem_idx;
	ent->np_logic    = params->np.logic;
	ent->hdr_len_off = params->skip.hdr_len_off + params->skip.hdr_len_sz;
	ent->hdr_len_sz  = params->skip.hdr_len_sz;
	ent->hdr_len_im1 = params->skip.hdr_len_im;
	ent->skip_logic  = params->skip.logic;
	ent->skip_op     = params->skip.op;
	if ((params->skip.hdr_len_min <= params->skip.hdr_len_max) &&
		(params->skip.hdr_len_min)) {
		ent->hdr_len_chck = true;
		ent->hdr_len_max  = params->skip.hdr_len_max;
		ent->hdr_len_min  = params->skip.hdr_len_min;
	}
	ent->dont_care  = params->ignore;
	ent->fallback   = params->fallback;
	ent->pit.cnt    = params->ext.n_fld;
	ent->entry_type = params->entry_type;
}

s32 prsr_up_layer_proto_pre_proto_add(enum prsr_proto_id id,
				      const struct pre_proto *pre, u16 pre_n)
{
	struct np_sel_entry np_e;
	struct proto_db_e *proto;
	char name[PRSR_MAX_NAME];
	u16 i, idx;
	s32 ret;
#ifdef CONFIG_DEBUG_FS
	s32 n;
#endif
	pr_debug("id %u, pre %p, pre_n %u\n", id, pre, pre_n);

	proto = __get_proto(id);
	if (unlikely(!proto))
		return -EINVAL;

	memset(&np_e, 0, sizeof(np_e));
	/* first check that all parameters are ok */
	for (i = 0; i < pre_n; i++) {
		if (unlikely(!__prsr_np_logic_is_valid(pre[i].np_logic))) {
			prsr_err("Invalid np logic %u at index %u\n",
				 pre[i].np_logic, i);
			return -EINVAL;
		}
	}
	/* iterate through the protocol's pre protos */
	for (i = 0; i < pre_n; i++) {
		// add next protocol select entry
		np_e.imem_idx = proto->imem_idx;
		np_e.key = PRSR_NP_SEL_KEY_BUILD(pre[i].val, pre[i].np_logic);
#ifdef CONFIG_DEBUG_FS
		pr_buf(name, sizeof(name), n, "%s -> %s",
		       np_logic_str[pre[i].np_logic], proto->name);
#else
		name[0] = '\0';
#endif
		ret = __prsr_np_sel_entry_add(&np_e, name, &idx);
		if (unlikely(ret))
			return ret;
		set_bit(idx, proto->np_map);
	}

	return 0;
}

s32 __prsr_up_layer_proto_add(enum prsr_proto_id id, bool dbgfs_add,
			      const struct prsr_up_layer_proto_params *params)
{
	struct last_ethertype_entry last_e;
	struct proto_db_e  *proto;
	struct imem_entry   imem_e;
	struct skip_entry   skip_e;
	struct pit_entry    pit[PRSR_MAX_EXTRACT_ENTRIES];
	s32 ret;
	u32 i, n;
	u16 idx;

	pr_debug("id %u, params %p\n", id, params);
	if (unlikely(!__prsr_up_layer_proto_is_valid(id, params)))
		return -EINVAL;

	memset(&imem_e, 0, sizeof(imem_e));
	memset(&last_e, 0, sizeof(last_e));

	proto = __get_proto(id);
	if (unlikely(!proto)) {
		pr_crit("Failed to get proto db entry\n");
		return -ENOENT;
	}
	/* init database entry */
	n = strscpy(proto->name, params->name, sizeof(proto->name));
	proto->id       = id;
	proto->proto_id = params->proto_id;
	proto->l2_idx   = TABLE_ENTRY_INVALID_IDX;
	/* add pit entries */
	proto->pit.cnt = params->ext.n_fld;
	/* create PIT table entries */
	if (proto->pit.cnt) {
		memset(pit, 0, sizeof(pit));
		for (i = 0; i < proto->pit.cnt; i++)
			__prsr_fld_ext_to_pit_entry(&params->ext.fld[i],
						    &pit[i]);
		ret = __prsr_pit_entries_add(proto->name, pit,
					     proto->pit.cnt, &proto->pit.base);
		if (unlikely(ret))
			return ret;
	}
	/* add IMEM entry */
	proto->imem_idx = params->hw_idx;
	__prsr_up_params_to_imem_entry(params, &imem_e);
	memcpy(&imem_e.pit, &proto->pit, sizeof(imem_e.pit));
	ret = __prsr_imem_entry_add(proto, proto->name, &imem_e,
				    &proto->imem_idx);
	if (unlikely(ret))
		goto fail;
	/* add last ethertype entry */
	for (i = 0; i < params->n_leth; i++) {
		last_e.proto_id = params->leth[i].ethtype;
		last_e.imem_idx = proto->imem_idx;
		ret = __prsr_last_ethertype_entry_add(&last_e,
						      params->leth[i].name,
						      &idx);
		if (unlikely(ret))
			goto fail;
		set_bit(idx, proto->leth_map);
	}
	/* add skip CAM entries */
	for (i = 0; i < params->skip.lut.n; i++) {
		skip_e.hdr_len = params->skip.lut.pairs[i].val;
		skip_e.key = PRSR_SKIP_KEY_BUILD(params->skip.lut.pairs[i].key,
						 params->skip.op);
		ret = __prsr_skip_entry_add(&skip_e, proto->name, &idx);
		if (unlikely(ret))
			goto fail;
		set_bit(idx, proto->skip_map);
	}

	/* finally, mark the protocol as valid in the db */
	proto->valid = true;
	if (dbgfs_add)
		prsr_dbg_proto_add(id);
	pr_debug("done\n");
	return 0;
fail:
	prsr_proto_del(id);
	pr_debug("fail\n");
	return ret;
}

s32 prsr_up_layer_proto_add(enum prsr_proto_id id,
			    const struct prsr_up_layer_proto_params *params)
{
	return __prsr_up_layer_proto_add(id, true, params);
}

s32 prsr_proto_del(enum prsr_proto_id id)
{
	return __prsr_proto_del(id, true);
}

s32 __prsr_proto_del(enum prsr_proto_id id, bool dbgfs_del)
{
	struct proto_db_e *proto;
	u32 bit;

	proto = __get_proto(id);
	if (unlikely(!proto))
		return -EINVAL;

	pr_debug("deleting proto %s(%u)\n", proto->name, proto->id);
	/* delete entries from all tables */
	if (proto->imem_idx != TABLE_ENTRY_INVALID_IDX)
		__prsr_imem_entry_del(proto->imem_idx);
	else if (proto->l2_idx != TABLE_ENTRY_INVALID_IDX)
		__prsr_l2_entry_del(proto->l2_idx);
	__prsr_pit_entries_del(proto->name, proto->pit.base, proto->pit.cnt);
	for_each_set_bit(bit, proto->leth_map, PRSR_NUM_LAST_ETHTYPE_ENTRIES)
		__prsr_last_ethertype_entry_del(bit);
	for_each_set_bit(bit, proto->np_map, PRSR_NUM_NP_SELECT_ENTRIES)
		__prsr_np_sel_entry_del(bit);
	for_each_set_bit(bit, proto->skip_map, PRSR_NUM_SKIP_ENTRIES)
		__prsr_skip_entry_del(bit);
	for_each_set_bit(bit, proto->merge_map, PRSR_NUM_MERGE_ENTRIES)
		__prsr_merge_entry_del(bit);
	/* clean database entry */
	memset(proto, 0, sizeof(*proto));
	proto->id       = PRSR_PROTO_INVALID;
	proto->imem_idx = TABLE_ENTRY_INVALID_IDX;
	proto->l2_idx   = TABLE_ENTRY_INVALID_IDX;
	/* clear the bit in the reserved protocol bit */
	clear_bit(id, db->rsrv_protos_map);
	/* delete debugfs files */
	if (dbgfs_del)
		prsr_dbg_proto_del(id);

	return 0;
}

s32 prsr_proto_id_alloc(enum prsr_proto_id *id)
{
	if (unlikely(ptr_is_null(id)))
		return -EINVAL;

	if (bitmap_full(db->rsrv_protos_map, PRSR_PROTOS_NUM))
		return -ENOSPC;

	*id = find_next_zero_bit(db->rsrv_protos_map, PRSR_PROTOS_NUM, 0);
	set_bit(*id, db->rsrv_protos_map);

	return 0;
}

/* ========================================================================== *
 *                      Module's APIs                                         *
 * ========================================================================== *
 */
/**
 * @brief Set parser's protocols according to the specified profile
 *        Configuring protocol which have same profile id as 'prof_id'
 * @param prof_id profile
 * @param protos protocols array
 * @return s32 0 on success, error code otherwise
 */
static s32 __prsr_profile_protos_set(enum prsr_prof_id prof_id,
				     struct protocols *protos)
{
	struct protocol_info *params;
	u32 i;
	s32 ret;

	/* Add protocols in HW and DB */
	for (i = 0; i < protos->n_proto; i++) {
		params = &protos->proto[i];
		/* protocol's profile match current profile ? */
		if (params->prof_id != PRSR_PROFILE_COMMON &&
		    params->prof_id != prof_id) {
			pr_debug("Protocol %s skipped\n", params->name);
			continue;
		}

		pr_debug("Adding protocol %s(%u)\n", params->name, params->id);
		if (params->id == PRSR_PROTO_MAC)
			ret = __prsr_mac_proto_add(params->id, &params->mac);
		else if (PRSR_IS_L2_PROTO(params->id))
			ret = __prsr_l2_proto_add(params->id, &params->l2);
		else if (PRSR_IS_UPPER_LAYER_PROTO(params->id))
			ret = prsr_up_layer_proto_add(params->id, &params->up);
		else
			ret = -EPROTONOSUPPORT;
		if (unlikely(ret))
			return ret;
	}

	return 0;
}

/**
 * @brief Set protocols preceding protocols, configure for each
 *        protocols which protocols precede him
 * @param prof_id specify on which profile this should be configured
 * @param protos protocols array
 * @return s32 0 on success
 */
static s32 __prsr_profile_pre_protos_set(enum prsr_prof_id prof_id,
					 struct protocols *protos)
{
	struct protocol_info *proto;
	u16 i;
	s32 ret;

	/* Add protocols relations */
	for (i = 0; i < protos->n_proto; i++) {
		proto = &protos->proto[i];
		/* protocol's profile match current profile ? */
		if (proto->prof_id != PRSR_PROFILE_COMMON &&
		    proto->prof_id != prof_id) {
			pr_debug("Protocol %s skipped\n", proto->name);
			continue;
		}
		if (!PRSR_IS_UPPER_LAYER_PROTO(proto->id))
			continue;
		ret = prsr_up_layer_proto_pre_proto_add(proto->id,
							proto->up.pre,
							proto->up.n_pre);
		if (unlikely(ret))
			return ret;
	}

	return 0;
}

/**
 * @brief Test whether a merge info is valid or not
 * @param info info to test
 * @return s32 0 in case 'info' is valid, false otherwise
 */
static bool __prsr_merge_info_is_valid(const struct merge_info *info)
{
	struct proto_db_e *proto_prev, *proto_curr;
	bool res = true;
	u16 imem_idx;

	if (unlikely(ptr_is_null(info)))
		return false;

	pr_debug("prev %u, curr %u, merged %u\n",
		 info->prev, info->curr, info->merged);
	/* verify merged is either prev or curr */
	if (unlikely(info->merged != info->prev &&
		     info->merged != info->curr)) {
		prsr_err("merged must be either prev or curr\n");
		res = false;
	}
	/* verify all protocols ids are valid */
	proto_prev  = __get_proto(info->prev);
	if (unlikely(!proto_prev)) {
		prsr_err("invalid previous protocol\n");
		res = false;
	}
	proto_curr = __get_proto(info->curr);
	if (unlikely(!proto_curr)) {
		prsr_err("invalid current protocol\n");
		res = false;
	}
	if (proto_prev) {
		/* verify protocol exist in db */
		if (unlikely(!proto_prev->valid)) {
			prsr_err("previous protocol %u doesn't exist in db\n",
				 info->prev);
			res = false;
		}
		/* verify protocol has valid imem index */
		imem_idx = proto_prev->imem_idx;
		if (unlikely(!__prsr_imem_index_is_valid(imem_idx))) {
			prsr_err("previous protocol %u has invalid imem index %u\n",
				 info->prev, imem_idx);
			res = false;
		}
	}
	if (proto_curr) {
		/* verify protocol exist in db */
		if (unlikely(!proto_curr->valid)) {
			prsr_err("current protocol %u doesn't exist in db\n",
				 info->curr);
			res = false;
		}
		/* verify protocol has valid imem index */
		imem_idx = proto_curr->imem_idx;
		if (unlikely(!__prsr_imem_index_is_valid(imem_idx))) {
			prsr_err("current protocol %u has invalid imem index %u\n",
				 info->curr, imem_idx);
			res = false;
		}
	}

	return res;
}

s32 prsr_protocols_merge(const struct merge_info *info)
{
	struct proto_db_e *proto_prev, *proto_curr, *proto_merged;
	struct merge_entry  ent;
	char name[PRSR_MAX_NAME];
	s32 ret, n;
	u16 idx;

	if (unlikely(ptr_is_null(info)))
		return -EINVAL;

	if (unlikely(!__prsr_merge_info_is_valid(info))) {
		prsr_err("invalid merge info\n");
		return -EINVAL;
	}

	/* get protocols reference */
	proto_prev   = __get_proto(info->prev);
	proto_curr   = __get_proto(info->curr);
	proto_merged = __get_proto(info->merged);

	if (unlikely(!proto_prev || !proto_curr || !proto_merged)) {
		pr_crit("Failed to get prev, curr, and merged protocols\n");
		return -ENOENT;
	}

	/* convert the info into merge entry */
	ent.prev   = proto_prev->imem_idx;
	ent.curr   = proto_curr->imem_idx;
	ent.merged = proto_merged->imem_idx;
	pr_buf(name, sizeof(name), n, "[%s]+[%s] = [%s]",
	       proto_prev->name, proto_curr->name, proto_merged->name);
	/* create the entry */
	ret = __prsr_merge_entry_add(&ent, name, &idx);
	if (unlikely(ret))
		return ret;
	/* update the protocols with the new entry */
	set_bit(idx, proto_prev->merge_map);
	set_bit(idx, proto_curr->merge_map);

	return 0;
}

/**
 * @brief Configure all protocols merges
 * @param prof_id specify on which profile the merge should be
 *                configure
 * @param merges merge info array
 * @param n_merges number of infos
 * @return s32 0 on success, error code otherwise
 */
static s32 __prsr_profile_merge_set(enum prsr_prof_id prof_id,
				    struct merge_profile_info *merges,
				    u16 n_merges)
{
	struct merge_profile_info *info;
	s32 ret;
	u16 i;

	if (unlikely(ptr_is_null(merges)))
		return -EINVAL;

	for (i = 0; i < n_merges; i++) {
		info = &merges[i];
		if (info->prof_id != PRSR_PROFILE_COMMON &&
		    info->prof_id != prof_id) {
			pr_debug("Skipping merge info %u\n", i);
			continue;
		}
		ret = prsr_protocols_merge(&info->info);
		if (unlikely(ret)) {
			prsr_err("failed to set merge info %u\n", i);
			return ret;
		}
	}

	return 0;
}

/**
 * @brief Set parser profile, configure protocols that should be
 *        supported by the parser based on the specified profile.<br>
 * @param prof_id profile configure
 * @param protos protocols info
 * @return s32 0 on success, error code otherwise
 */
static s32 __prsr_profile_set(enum prsr_prof_id prof_id,
			      struct protocols *protos)
{
	s32 ret;

	if (unlikely(ptr_is_null(protos)))
		return -EINVAL;

	pr_debug("prof_id %u, protocols count %u, merges count %u\n",
		 prof_id, protos->n_proto, protos->n_merge);

	ret = __prsr_profile_protos_set(prof_id, protos);
	if (unlikely(ret))
		return ret;

	ret = __prsr_profile_pre_protos_set(prof_id, protos);
	if (unlikely(ret))
		return ret;

	ret = __prsr_profile_merge_set(prof_id, protos->merge, protos->n_merge);
	if (unlikely(ret))
		return ret;

	pr_debug("done\n");
	return 0;
}

/**
 * @brief Initialize driver database
 * @return s32 0 on success, error code otherwise
 */
static s32 __prsr_db_init(void)
{
	struct proto_db_e *proto;
	u32 i;

	/* allocate database memory */
	db = kzalloc(sizeof(*db), GFP_KERNEL);
	if (unlikely(!db)) {
		prsr_err("Failed to allocate DB memory\n");
		return -ENOMEM;
	}
	/* allocate reserved protocol ids bitmap for dynamic allocation */
	db->rsrv_protos_map = kcalloc(BITS_TO_LONGS(PRSR_PROTOS_NUM),
				      sizeof(*db->rsrv_protos_map),
				      GFP_KERNEL);
	if (unlikely(!db->rsrv_protos_map)) {
		prsr_err("Failed to allocate reserved protocols bitmap\n");
		return -ENOMEM;
	}
	spin_lock_init(&db->lock);
	for (i = 0; i < ARRAY_SIZE(db->protos); i++) {
		proto = __get_proto(i);
		proto->valid    = false;
		proto->id       = PRSR_PROTO_INVALID;
		proto->imem_idx = TABLE_ENTRY_INVALID_IDX;
		proto->l2_idx   = TABLE_ENTRY_INVALID_IDX;
	}

	return 0;
}

/**
 * @brief Initialize parser hardware registers to their default
 *        values.
 */
static void __prsr_hw_init(void)
{
	struct port_cfg_entry cfg;
	u32 i;

	/* init field vector swapping */
	__prsr_fv_swap_init();
	/* enable fv masking */
	prsr_fv_mask_en_set(true);
	/* set error/abort/drop flags */
	prsr_hw_err_flags_init();
	/* enable support for all IPv6 next headers */
	__prsr_ipv6_nexthdr_support_init();
	/* set some other defaults */
	/* init ports table */
	memset(&cfg, 0, sizeof(cfg));
	cfg.skip_l2_pc = PRSR_PORT_NO_L2_SKIP_VAL;
	/* for A-step, sending all ip packets to the ingress uc */
	if (pp_silicon_step_get() == PP_SSTEP_A)
		cfg.fallback = true;
	for (i = 0; i < PP_MAX_PORT; i++)
		prsr_port_cfg_entry_set(i, &cfg);
	/* init all tables entries */
	PRSR_FOR_EACH_L2_ENTRY(i)
		prsr_l2_entry_set(i, NULL, NULL);
	PRSR_FOR_EACH_LAST_ETHERTYPE_ENTRY(i)
		prsr_last_ethertype_entry_set(i, NULL);
	PRSR_FOR_EACH_IMEM_ENTRY(i)
		prsr_imem_entry_set(i, NULL);
	PRSR_FOR_EACH_NP_SELECT_ENTRY(i)
		prsr_np_sel_entry_set(i, NULL);
	PRSR_FOR_EACH_SKIP_ENTRY(i)
		prsr_skip_entry_set(i, NULL);
	PRSR_FOR_EACH_MERGE_ENTRY(i)
		prsr_merge_entry_set(i, NULL);
	PRSR_FOR_EACH_PIT_ENTRY(i)
		prsr_pit_entry_set(i, NULL);
}

/**
 * @brief Create an abstraction of all parser hw tables
 * @return s32 0 on success, error code otherwise
 */
static s32 __prsr_tables_create(void)
{
	/* L2 ethernet type table */
	db->l2_tbl = table_create("L2 ETH", PRSR_NUM_L2_ETH_TYPE_ENTRIES, 0, 0);
	if (unlikely(IS_ERR(db->l2_tbl)))
		return PTR_ERR(db->l2_tbl);
	/* IMEM table */
	db->imem_tbl = table_create("IMEM", PRSR_NUM_IMEM_ENTRIES, 0, 0);
	if (unlikely(IS_ERR(db->imem_tbl)))
		return PTR_ERR(db->imem_tbl);
	/* Last Ethernet table */
	db->last_eth_tbl = table_create("Last Ethertype",
					PRSR_NUM_LAST_ETHTYPE_ENTRIES, 0, 0);
	if (unlikely(IS_ERR(db->last_eth_tbl)))
		return PTR_ERR(db->last_eth_tbl);
	/* NP Select table */
	db->np_sel_tbl = table_create("NP Select",
				      PRSR_NUM_NP_SELECT_ENTRIES, 0, 0);
	if (unlikely(IS_ERR(db->np_sel_tbl)))
		return PTR_ERR(db->np_sel_tbl);
	/* Skip CAM table */
	db->skip_tbl = table_create("Skip", PRSR_NUM_SKIP_ENTRIES, 0, 0);
	if (unlikely(IS_ERR(db->skip_tbl)))
		return PTR_ERR(db->skip_tbl);
	/* Merge CAM table */
	db->merge_tbl = table_create("Merge", PRSR_NUM_MERGE_ENTRIES, 0, 0);
	if (unlikely(IS_ERR(db->merge_tbl)))
		return PTR_ERR(db->merge_tbl);
	/* PIT table */
	db->pit_tbl = table_create("PIT", PRSR_NUM_PIT_ENTRIES, 0, 0);
	if (unlikely(IS_ERR(db->pit_tbl)))
		return PTR_ERR(db->pit_tbl);

	return 0;
}

#define CTR_DIV_VAL (3)

static void __prsr_stats_get(struct prsr_stats *stats)
{
	memset(stats, 0, sizeof(struct prsr_stats));

	stats->rx_pkts        = PP_REG_RD32(PP_PRSR_PARSER_RPB_POP_CNT_REG);
	stats->rx_valid_pkts  = PP_REG_RD32(PP_PRSR_RPB_PARSER_POP_VLD_CNT_REG);
	stats->tx_pkts        = PP_REG_RD32(PP_PRSR_FV_CNT_REG);
	stats->tx_pkts_to_cls = PP_REG_RD32(PP_PRSR_FV2CLASS_CNT_REG);
	stats->tx_pkts_to_uc  = PP_REG_RD32(PP_PRSR_FV2UC_CNT_REG);
	stats->uc_discard     = PP_REG_RD32(PP_PRSR_PKT_RATE_STATUS_REG);
	stats->pprs_discard   = PP_REG_RD32(PP_PRSR_RPB_PPRS_TH_STATUS_REG);

	stats->rpb_discard    =
		PP_REG_RD32(PP_PRSR_RPB_DISCARD_CNT_REG) / CTR_DIV_VAL;
	stats->rpb_err        =
		PP_REG_RD32(PP_PRSR_RPB_ERROR_CNT_REG) / CTR_DIV_VAL;
	stats->zero_len       =
		PP_REG_RD32(PP_PRSR_ZERO_LEN_CNT_REG) / CTR_DIV_VAL;
	stats->proto_len_err  =
		PP_REG_RD32(PP_PRSR_PROTO_LENGTH_ERR_CNT_REG) / CTR_DIV_VAL;
	stats->hdr_len_err    =
		PP_REG_RD32(PP_PRSR_MAX_HEADER_LEN_ERR_CNT_REG) / CTR_DIV_VAL;
	stats->proto_overflow =
		PP_REG_RD32(PP_PRSR_PROTO_OVERFLOW_ERR_CNT_REG) / CTR_DIV_VAL;
	stats->ip_len_err     =
		PP_REG_RD32(PP_PRSR_IP_LEN_ERR_CNT_REG) / CTR_DIV_VAL;
	stats->payload_err    =
		PP_REG_RD32(PP_PRSR_EOF_NO_PAYLOAD_ERR_CNT_REG) / CTR_DIV_VAL;
	stats->aborted_pkts   =
		PP_REG_RD32(PP_PRSR_ABORT_CNT_ERR_CNT_REG) / CTR_DIV_VAL;
	stats->last_eth_err =
		PP_REG_RD32(PP_PRSR_NO_LAST_ETH_TYPE_ERR_CNT_REG) /
		CTR_DIV_VAL;
}

void prsr_stats_reset(void)
{
	__prsr_stats_get(&rstats);
}

s32 prsr_stats_get(struct prsr_stats *stats)
{
	if (unlikely(!stats)) {
		pr_err("Invalid stats parameter\n");
		return -EINVAL;
	}
	__prsr_stats_get(stats);
	U32_STRUCT_DIFF(&rstats, stats, stats);

	return 0;
}

/* module APIs */
s32 prsr_init(struct prsr_init_param *init_param)
{
	struct protocols *protocols = NULL;
	s32 ret = 0;

	if (!init_param->valid)
		return -EINVAL;

	pr_debug("base_addr 0x%llx, prof_id %u\n", init_param->parser_base,
		 init_param->parser_profile);
	prsr_base_addr = init_param->parser_base;

	/* db */
	ret = __prsr_db_init();
	if (unlikely(ret)) {
		prsr_err("Failed to initialize DB\n");
		goto done;
	}
	/* tables */
	ret = __prsr_tables_create();
	if (unlikely(ret)) {
		prsr_err("Failed to initialize tables\n");
		goto done;
	}
	/* HW regs */
	__prsr_hw_init();
	/* debugfs */
	ret = prsr_dbg_init(init_param->dbgfs);
	if (unlikely(ret)) {
		prsr_err("Failed to initialize parser debugfs\n");
		goto done;
	}
	/* protocols init */
	/* allocate protocols memory */
	protocols = kzalloc(sizeof(*protocols), GFP_KERNEL);
	if (unlikely(!protocols)) {
		prsr_err("Failed to allocate protocols memory\n");
		ret = -ENOMEM;
		goto done;
	}
	ret = __prsr_protocols_init(protocols);
	if (unlikely(ret)) {
		prsr_err("failed to initialize protocols\n");
		goto done;
	}
	/* profile set */
	ret = __prsr_profile_set(init_param->parser_profile, protocols);
	if (unlikely(ret)) {
		prsr_err("failed to set profile %u\n",
			 init_param->parser_profile);
		/* clean hw register to make parser will not create fv */
		__prsr_hw_init();
		goto done;
	}
	/* default ethertype IMEM entry */
	ret = prsr_dflt_ethertype_set(PRSR_PROTO_PAYLOAD);
	if (unlikely(ret)) {
		prsr_err("Failed to configure proto %u as default ethertype\n",
			 PRSR_PROTO_PAYLOAD);
		goto done;
	}
	/* set default imem program counters */
	ret = __prsr_set_dflt_pc();
	if (unlikely(ret)) {
		prsr_err("Failed to configure default imem pc\n");
		goto done;
	}

done:
	kfree(protocols);
	pr_debug("%s : ret %d\n", ret ? "fail" : "done", ret);
	return ret;
}

void prsr_exit(void)
{
	pr_debug("\n");
	prsr_dbg_clean();
	if (likely(db)) {
		table_destroy(db->l2_tbl);
		table_destroy(db->last_eth_tbl);
		table_destroy(db->imem_tbl);
		table_destroy(db->np_sel_tbl);
		table_destroy(db->skip_tbl);
		table_destroy(db->merge_tbl);
		table_destroy(db->pit_tbl);
		kfree(db->rsrv_protos_map);
		kfree(db);
		db = NULL;
	}
}
