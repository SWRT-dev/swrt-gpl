/*
 * Description: PP Session Information module
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2018-2020 Intel Corporation
 */

#define pr_fmt(fmt) "[PP_SI]:%s:%d: " fmt, __func__, __LINE__

#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/bitops.h>
#include "pp_si.h"
#include "pp_fv.h"
#include "checker.h"
#include "infra.h"
#include "pp_common.h"
#include "pp_regs.h"
#include "pp_port_mgr.h"


/**
 * @brief Shortcut macro to initialize SI field info with all
 *        parameters
 */
#define SI_FLD_INIT(flds, id)                                                  \
	BUF_FLD_INIT(flds, id, NULL, __##id##_IS_VALID, __##id##_TO_SI,        \
		     __##id##_FROM_SI, NULL)

/**
 * @brief Shortcut macro to initialize dynamic SI dpu field info
 */
#define SI_DPU_DFLD_INIT(flds, id)                                             \
	BUF_DFLD_INIT_SIMPLE(flds, id, __si_fld_dpu_start_bit_get)

/**
 * @brief Shortcut macro to initialize dynamic SI sce field info
 */
#define SI_SCE_DFLD_INIT(flds, id)                                             \
	BUF_DFLD_INIT_SIMPLE(flds, id, __si_fld_sce_start_bit_get)

/**
 * @brief Same as SI_DFLD_INIT but with the addition of to and from
 *        conversions
 */
#define SI_SCE_DFLD_INIT_LE16(flds, id)                                        \
	BUF_DFLD_INIT(flds, id, NULL, NULL, __si_fld_to_le16,                  \
		      __si_fld_from_le16, __si_fld_sce_start_bit_get)

/**
 * @brief SI fields encoding/decoding info
 */
static struct buf_fld_info si_flds[SI_FLDS_NUM];

/**
 * @brief Dynamic SI fields encoding/decoding info
 */
static struct buf_fld_info dsi_flds[DSI_FLDS_NUM];

static const char * const si_chck_flags_str[] = {
	"SEND2FW",
	"DROP",
	"DYNAMIC",
	"L3_OPTS",
	"TTL_EXP",
	"TCP_CTRL",
	"FRAG",
	"MTU_OVFL",
	"TCP_ACK",
	"MCAST",
	"EXCP",
	"UPDATE",
	"INVALID",
};

const char * const pp_si_fld2str(enum pp_si_fld fld)
{
	if (pp_si_field_id_is_valid(fld))
		return si_flds[fld].name;

	return NULL;
}

s32 pp_si_str2fld(const char * const name, enum pp_si_fld *fld)
{
	u32 idx;

	for (idx = 0; idx < SI_FLDS_NUM; idx++) {
		if (!strcmp(si_flds[idx].name, name)) {
			*fld = idx;
			return 0;
		}
	}

	return -EINVAL;
}

/**
 * @brief Convert value to SI SCE l4 checksum delta
 * @param val value to convert
 * @return s64 converted value
 */
static s64 __si_fld_to_le16(s64 val)
{
	return (s64)cpu_to_le16((s16)val);
}

/**
 * @brief Convert back SI SCE l4 checksum delta hw value
 * @param val value to convert
 * @return s64 converted value
 */
static s64 __si_fld_from_le16(s64 val)
{
	return (s64)le16_to_cpu((s16)val);
}

/**
 * @brief Get an SI field from an SI hardware buffer
 * @note for internal use only, doesn't verify input parameters
 * @param si hw SI to get the field from
 * @param id field id
 * @return s32 the field value
 */
static inline s32 __si_fld_get(const struct pp_hw_si *si, enum pp_si_fld id)
{
	return buf_fld_get(&si_flds[id], si->word);
}

/**
 * @brief Set an SI field into an SI hardware buffer
 * @note for internal use only, doesn't verify input parameters
 * @param si hw SI to set the field to
 * @param id field id
 * @return s32 0 on success, error code otherwise
 */
static inline s32 __si_fld_set(struct pp_hw_si *si, enum pp_si_fld id, s32 val)
{
	return buf_fld_set(&si_flds[id], si->word, val);
}

/**
 * @brief Get an DSI field from an DSI hardware buffer
 * @note for internal use only, doesn't verify input parameters
 * @param si hw DSI to get the field from
 * @param id field id
 * @return s32 the field value
 */
static inline s64 __dsi_fld_get(const struct pp_hw_dsi *dsi, enum pp_dsi_fld id)
{
	return buf_fld_get(&dsi_flds[id], dsi->word);
}

/**
 * @brief Set an DSI field into an DSI hardware buffer
 * @note for internal use only, doesn't verify input parameters
 * @param si hw DSI to set the field to
 * @param id field id
 * @return s32 0 on success, error code otherwise
 */
static inline s32 __dsi_fld_set(struct pp_hw_dsi *dsi, enum pp_dsi_fld id,
				s32 val)
{
	return buf_fld_set(&dsi_flds[id], dsi->word, val);
}

const char * const pp_si_chck_flag_to_str(u32 flag)
{
	if (unlikely(flag >= SI_CHCK_FLAGS_NUM)) {
		pr_err("invalid si checker flag %d\n", flag);
		return si_chck_flags_str[SI_CHCK_FLAGS_NUM];
	}

	return si_chck_flags_str[flag];
}

/* ========================================================================== */
/*                      Fields convert and valid APIs                         */
/* ========================================================================== */
/**
 * @brief Test whether the specified value is a valid SI dpu start
 *        offset or not
 * @param mask
 * @param val value to test
 * @return bool true in case 'val' is valid, false otherwise
 */
static bool __SI_FLD_DPU_START_IS_VALID(s64 val)
{
	switch (val) {
	case 0x20:
	case 0x30:
	case 0x40:
		return true;
	default:
		break;
	}
	return false;
}

/**
 * @brief Convert value to SI dpu start granularity
 * @param val value to convert
 * @return s64 converted value
 */
static s64 __SI_FLD_DPU_START_TO_SI(s64 val)
{
	return (val >> 4) - 1;
}

/**
 * @brief Convert back value from SI dpu start granularity
 * @param val value to convert
 * @return s64 converted value
 */
static s64 __SI_FLD_DPU_START_FROM_SI(s64 val)
{
	return ((val + 1) << 4);
}

/**
 * @brief Test whether the specified value is a valid SI sce start
 *        offset or not
 * @param mask
 * @param val value to test
 * @return bool true in case 'val' is valid, false otherwise
 */
static bool __SI_FLD_SCE_START_IS_VALID(s64 val)
{
	switch (val) {
	case 0x20:
	case 0x30:
	case 0x40:
	case 0x50:
	case 0x60:
	case 0x70:
	case 0x80:
		return true;
	default:
		break;
	}
	return false;
}

/**
 * @brief Convert value to SI sce start granularity
 * @param val value to convert
 * @return s64 converted value
 */
static s64 __SI_FLD_SCE_START_TO_SI(s64 val)
{
	return (val >> 4) - 1;
}

/**
 * @brief Convert back value from SI sce start granularity
 * @param val value to convert
 * @return s64 converted value
 */
static s64 __SI_FLD_SCE_START_FROM_SI(s64 val)
{
	return ((val + 1) << 4);
}

/**
 * @brief Test whether the specified value is a valid SI field vector
 *        or not
 * @param mask
 * @param val value to test
 * @return bool true in case 'val' is valid, false otherwise
 */
static bool __SI_FLD_FV_SZ_IS_VALID(s64 val)
{
	if (val >= SI_MIN_FV_SZ && val <= sizeof(struct pp_fv))
		return true;
	return false;
}

/**
 * @brief Convert value to SI field vector size granularity
 * @param val value to convert
 * @return s64 converted value
 */
static s64 __SI_FLD_FV_SZ_TO_SI(s64 val)
{
	/* field vactor */
	return (val >> 4) - 1;
}

/**
 * @brief Convert back value from SI field vector size granularity
 * @param val value to convert
 * @return s64 converted value
 */
static s64 __SI_FLD_FV_SZ_FROM_SI(s64 val)
{
	return ((val + 1) << 4);
}

/**
 * @brief Test whether the specified value is a valid SI bce start
 *        offset or not
 * @param mask
 * @param val value to test
 * @return bool true in case 'val' is valid, false otherwise
 */
static bool __SI_FLD_BCE_START_IS_VALID(s64 val)
{
	switch (val) {
	case 0x20:
	case 0x30:
	case 0x40:
	case 0x50:
	case 0x60:
	case 0x70:
	case 0x80:
	case 0x90:
	case 0xa0:
		return true;
	default:
		break;
	}
	return false;
}

/**
 * @brief Convert value to SI bce start granularity
 * @param val value to convert
 * @return s64 converted value
 */
static s64 __SI_FLD_BCE_START_TO_SI(s64 val)
{
	return (val >> 4);
}

/**
 * @brief Convert back value from SI bce start granularity
 * @param val value to convert
 * @return s64 converted value
 */
static s64 __SI_FLD_BCE_START_FROM_SI(s64 val)
{
	return (val << 4);
}

/**
 * @brief Test whether the specified value is a valid SI bce region
 *        size or not
 * @param mask
 * @param val value to test
 * @return bool true in case 'val' is valid, false otherwise
 */
static bool __SI_FLD_BCE_SZ_IS_VALID(s64 val)
{
	if (val <= PP_SI_BCE_REGION_SZ)
		return true;
	return false;
}

/**
 * @brief Convert value to SI bce region size granularity
 * @param val value to convert
 * @return s64 converted value
 */
static s64 __SI_FLD_BCE_SZ_TO_SI(s64 val)
{
	return (val >> 4);
}

/**
 * @brief Convert back value from SI bce region size granularity
 * @param val value to convert
 * @return s64 converted value
 */
static s64 __SI_FLD_BCE_SZ_FROM_SI(s64 val)
{
	return (val << 4);
}

/**
 * @brief Test whether the specified value is a valid SI UD region
 *        size or not
 * @param mask
 * @param val value to test
 * @return bool true in case 'val' is valid, false otherwise
 */
static bool __SI_FLD_SI_UD_SZ_IS_VALID(s64 val)
{
	if (val <= PP_UD_REGION_SZ)
		return true;
	return false;
}

/**
 * @brief Convert value to SI UD region size granularity
 * @param val value to convert
 * @return s64 converted value
 */
static s64 __SI_FLD_SI_UD_SZ_TO_SI(s64 val)
{
	return (val >> 4);
}

/**
 * @brief Convert back value from SI UD region size granularity
 * @param val value to convert
 * @return s64 converted value
 */
static s64 __SI_FLD_SI_UD_SZ_FROM_SI(s64 val)
{
	return (val << 4);
}

/**
 * @brief Test whether the specified value is a valid SI color or not
 * @param mask
 * @param val value to test
 * @return bool true in case 'val' is valid, false otherwise
 */
static bool __SI_FLD_COLOR_IS_VALID(s64 val)
{
	return __pp_is_color_valid((u8)val);
}

/**
 * @brief Test whether the specified value is a valid SI template UD
 *        size or not
 * @param mask
 * @param val value to test
 * @return bool true in case 'val' is valid, false otherwise
 */
static bool __SI_FLD_TMPL_UD_SZ_IS_VALID(s64 val)
{
	if (val <= PP_SI_TMPL_UD_SZ)
		return true;
	return false;
}

/**
 * @brief Convert value to SI template UD size granularity
 * @param val value to convert
 * @return s64 converted value
 */
static s64 __SI_FLD_TMPL_UD_SZ_TO_SI(s64 val)
{
	return (val >> 4);
}

/**
 * @brief Convert back value from SI template UD size granularity
 * @param val value to convert
 * @return s64 converted value
 */
static s64 __SI_FLD_TMPL_UD_SZ_FROM_SI(s64 val)
{
	return (val == 3 ? 0 : val << 4);
}

/**
 * @brief Convert SI packet length diff from 2's complement 9 bit
 *        number to 2's complement 32 bit number
 */
static s64 __SI_FLD_PKT_LEN_DIFF_FROM_SI(s64 val)
{
	/* extend val from 9bit sign to 64 bits sign */
	return sign_extend64(val, 8);
}

/**
 * @brief Convert SI packet length diff from 2's complement 32 bit
 *        number to 2's complement 9 bit number
 */
static s64 __SI_FLD_PKT_LEN_DIFF_TO_SI(s64 val)
{
	struct buf_fld_info *fld = &si_flds[SI_FLD_PKT_LEN_DIFF];
	ulong mask = GENMASK(fld->msb - fld->lsb + 1, 0);

	/* conversion means just mask the MSB bits */
	return PP_FIELD_GET(mask, val);
}

/**
 * @brief Test whether the specified value is a valid SI packet len
 *        diff value or not
 * @param mask
 * @param val value to test
 * @return bool true in case 'val' is valid, false otherwise
 */
bool __SI_FLD_PKT_LEN_DIFF_IS_VALID(s64 val)
{
	if (((s32)S8_MIN <= (s32)val) && ((s32)val <= (s32)U8_MAX))
		return true;
	return false;
}

/**
 * @brief Get dpu start bit
 * @param word hw si
 * @return u32 dpu start bit
 */
static inline u32 __si_fld_dpu_start_bit_get(const u32 *word)
{
	const struct pp_hw_si *si;

	si = (const struct pp_hw_si *)container_of(&word[0],
						   struct pp_hw_si, word[0]);
	return __si_fld_get(si, SI_FLD_DPU_START) * BITS_PER_BYTE;
}

/**
 * @brief Get sce start bit
 * @param word hw si
 * @return u32 sce start bit
 */
static inline u32 __si_fld_sce_start_bit_get(const u32 *word)
{
	const struct pp_hw_si *si;

	si = (const struct pp_hw_si *)container_of(&word[0],
						    struct pp_hw_si, word[0]);
	return __si_fld_get(si, SI_FLD_SCE_START) * BITS_PER_BYTE;
}

/* ========================================================================== */
/*                                  Getters                                   */
/* ========================================================================== */

s32 pp_si_fld_get(const struct pp_hw_si *si, enum pp_si_fld id, s32 *val)
{
	if (unlikely(!(!ptr_is_null(si) &&
		       !ptr_is_null(val))))
		return -EINVAL;

	if (unlikely(!pp_si_field_id_is_valid(id)))
		return -EINVAL;

	*val = __si_fld_get(si, id);

	return 0;
}

s32 pp_dsi_fld_get(const struct pp_hw_dsi *dsi, enum pp_dsi_fld id, s64 *val)
{
	if (unlikely(!(!ptr_is_null(dsi) &&
		       !ptr_is_null(val))))
		return -EINVAL;

	if (unlikely(!pp_dsi_field_id_is_valid(id)))
		return -EINVAL;

	*val = __dsi_fld_get(dsi, id);

	return 0;
}

/* ========================================================================== */
/*                                  Setters                                   */
/* ========================================================================== */
s32 pp_si_fld_set(struct pp_hw_si *si, enum pp_si_fld id, s32 val)
{
	if (unlikely(ptr_is_null(si)))
		return -EINVAL;
	if (unlikely(!pp_si_field_id_is_valid(id)))
		return -EINVAL;

	return __si_fld_set(si, id, val);
}

s32 pp_dsi_fld_set(struct pp_hw_dsi *dsi, enum pp_dsi_fld id, s32 val)
{
	if (unlikely(ptr_is_null(dsi)))
		return -EINVAL;
	if (unlikely(!pp_dsi_field_id_is_valid(id)))
		return -EINVAL;

	return __dsi_fld_set(dsi, id, val);
}

/* ========================================================================== */
/*                                  Encoding                                  */
/* ========================================================================== */
/**
 * @brief Encode SGC indexes into hw SI buffer
 */
static s32 __pp_si_sgc_encode(struct pp_hw_si *hw_si, struct pp_si *si)
{
	s32 ret = 0;
	u32 i;

	/* encode sgc IDs */
	for_each_set_bit(i, &si->sgc_en_map, PP_SI_SGC_MAX)
		ret |= __si_fld_set(hw_si, SI_FLD_SGC1 + i, si->sgc[i]);
	/* encode sgc bitmap */
	ret |= __si_fld_set(hw_si, SI_FLD_SGC_BITMAP, si->sgc_en_map);

	return ret;
}

/**
 * @brief Encode TBM indexes into hw SI buffer
 */
static s32 __pp_si_tbm_encode(struct pp_hw_si *hw_si, struct pp_si *si)
{
	s32 ret = 0;
	u32 i;

	/* encode tbm IDs */
	for_each_set_bit(i, &si->tbm_en_map, PP_SI_TBM_MAX)
		ret |= __si_fld_set(hw_si, SI_FLD_TBM1 + i, si->tbm[i]);
	/* encode tbm bitmap */
	ret |= __si_fld_set(hw_si, SI_FLD_TBM_BITMAP, si->tbm_en_map);

	return ret;
}

/**
 * @brief Encode SI UD into hw SI buffer
 */
static s32 __pp_si_ud_encode(struct pp_hw_si *hw_si, struct pp_si *si)
{
	u8 *dst = (u8 *)hw_si + PP_SI_UD_OFFSET;
	s32 ret = 0;

	if (!si->si_ud_sz && !si->si_ps_sz)
		return ret;

	/* SI UD Size (if UD present it is already includes the PS) */
	si->si_ud_sz = roundup(si->si_ud_sz, 16);
	ret |= __si_fld_set(hw_si, SI_FLD_SI_UD_SZ, si->si_ud_sz);

	memcpy(dst, si->ud, max_t(u8, si->si_ps_sz, si->si_ud_sz));
	pr_debug("ps_sz %u. ud_sz %u\n", si->si_ps_sz, si->si_ud_sz);

	return ret;
}

/**
 * @brief Encode SI dpu region into hw SI buffer
 */
static s32 __pp_si_dpu_encode(struct pp_hw_si *hw_si, struct pp_si *si)
{
	s32 ret = 0;

	if (!SI_IS_DPU_EXIST(si))
		return ret;

	si->dpu_start = PP_SI_UD_OFFSET +
		roundup(max_t(u8, si->si_ud_sz, si->si_ps_sz), 16);

	ret |= __si_fld_set(hw_si, SI_FLD_DPU_START,       si->dpu_start);
	ret |= __si_fld_set(hw_si, SI_FLD_DPU_NAT_SZ,      si->dpu.nat_sz);
	ret |= __si_fld_set(hw_si, SI_FLD_DPU_PPPOE_OFF,   si->dpu.pppoe_off);
	ret |= __si_fld_set(hw_si, SI_FLD_DPU_NHDR_SZ,     si->dpu.nhdr_sz);
	ret |= __si_fld_set(hw_si, SI_FLD_DPU_NHDR_L3_OFF, si->dpu.nhdr_l3_off);
	ret |= __si_fld_set(hw_si, SI_FLD_DPU_LYR_FLD_OFF, si->dpu.lyr_fld_off);

	return ret;
}

static s32 __pp_si_sce_encode(struct pp_hw_si *hw_si, struct pp_si *si)
{
	struct pp_si_sce *sce = &si->sce;
	s32 ret = 0;

	if (!SI_IS_SCE_EXIST(si))
		return ret;

	if (SI_IS_DPU_EXIST(si))
		/* if dpu exist, set the sce to include the dpu so
		 * the modifier will have access to the dpu fields as well
		 */
		si->sce_start = si->dpu_start;
	else
		si->sce_start = PP_SI_UD_OFFSET +
			roundup(max_t(u8, si->si_ud_sz, si->si_ps_sz), 16);

	ret |= __si_fld_set(hw_si, SI_FLD_SCE_START,        si->sce_start);
	ret |= __si_fld_set(hw_si, SI_FLD_SCE_L4_CSUM_ZERO, sce->l4_csum_zero);
	ret |= __si_fld_set(hw_si, SI_FLD_SCE_L4_CSUM,      sce->l4_csum_delta);
	ret |= __si_fld_set(hw_si, SI_FLD_SCE_L3_CSUM,      sce->l3_csum_delta);
	ret |= __si_fld_set(hw_si, SI_FLD_SCE_IP_LEN_DIFF,  sce->tot_len_diff);
	ret |= __si_fld_set(hw_si, SI_FLD_SCE_DSCP,         sce->dscp);
	ret |= __si_fld_set(hw_si, SI_FLD_SCE_NHDR_CSUM,    sce->nhdr_csum);
	ret |= __si_fld_set(hw_si, SI_FLD_SCE_SRC_PORT,     sce->new_src_port);
	ret |= __si_fld_set(hw_si, SI_FLD_SCE_DST_PORT,     sce->new_dst_port);
	ret |= __si_fld_set(hw_si, SI_FLD_SCE_TTL_DIFF,     sce->ttl_diff);
	ret |= __si_fld_set(hw_si, SI_FLD_SCE_PPPOE_DIFF,   sce->pppoe_diff);
	ret |= __si_fld_set(hw_si, SI_FLD_SCE_L2_ORG_VAL,   sce->l2_org_val);

	return ret;
}

/**
 * @brief Encode SI bce region into hw SI buffer
 */
static s32 __pp_si_bce_encode(struct pp_hw_si *hw_si, struct pp_si *si)
{
	s32 ret = 0;
	u8 nat_sz;
	u8 *dst;

	if (!SI_IS_BCE_EXIST(si))
		return ret;

	/* bce always start after the dpu */
	si->bce_start = si->dpu_start + PP_SI_DPU_REGION_SZ;
	/* if sce exist, the bce should follow */
	if (SI_IS_SCE_EXIST(si))
		si->bce_start += PP_SI_SCE_REGION_SZ;

	dst        = (u8 *)hw_si + si->bce_start;
	nat_sz     = roundup(si->dpu.nat_sz, 16);
	si->bce_sz = roundup(nat_sz + si->dpu.nhdr_sz, 16);

	/* copy NAT */
	if (si->dpu.nat_sz)
		memcpy(dst, &si->bce.nat, si->dpu.nat_sz);

	/* copy new header after the nat */
	if (si->dpu.nhdr_sz)
		memcpy(dst + nat_sz, si->bce.nhdr, si->dpu.nhdr_sz);

	ret |= __si_fld_set(hw_si, SI_FLD_BCE_START, si->bce_start);
	ret |= __si_fld_set(hw_si, SI_FLD_BCE_SZ,    si->bce_sz);
	ret |= __si_fld_set(hw_si, SI_FLD_BCE_EXT,   si->bce_ext);

	return ret;
}

/**
 * @brief Encode PP field vector into hw SI buffer
 *        Classifier HW expect fv to be written at the end of the si
 *        and reversed, meaning, the fv byte 0 (LSB) is written to the
 *        last byte of the si, byte 1 is written to the last byte - 1
 *        of the si and so on
 */
static s32 __pp_si_fv_encode(struct pp_hw_si *hw_si, struct pp_si *si)
{
	/* make sure fv doesn't overlap bce */
	if ((sizeof(*hw_si) - si->fv_sz) < (si->bce_start + si->bce_sz)) {
		pr_err("FV overlap bce region, fv sz %u, bce start %u, bce sz %u",
		       si->fv_sz, si->bce_start, si->bce_sz);
		return -EINVAL;
	}

	__buf_swap((char *)hw_si + sizeof(*hw_si) - si->fv_sz,
		   si->fv_sz, &si->fv, si->fv_sz);

	return __si_fld_set(hw_si, SI_FLD_FV_SZ, si->fv_sz);
}

s32 pp_si_encode_top(struct pp_hw_si *hw_si, struct pp_si *si)
{
	u8  df_mask;
	s32 ret = 0;

	df_mask = SI_DF_MASK_BUILD(si->ext_df_mask, si->int_df_mask);
	ret |= __si_fld_set(hw_si, SI_FLD_RECIPE_ID,      si->recipe_idx);
	ret |= __si_fld_set(hw_si, SI_FLD_SESS_ID,        si->sess_id);
	ret |= __si_fld_set(hw_si, SI_FLD_PS_OFF,         si->ps_off);
	ret |= __si_fld_set(hw_si, SI_FLD_PLCY_BASE,      si->base_policy);
	ret |= __si_fld_set(hw_si, SI_FLD_COLOR,          si->color);
	ret |= __si_fld_set(hw_si, SI_FLD_TDOX_FLOW,      si->tdox_flow);
	ret |= __si_fld_set(hw_si, SI_FLD_DST_QUEUE,      si->dst_q);
	ret |= __si_fld_set(hw_si, SI_FLD_EGRS_PORT,      si->eg_port);
	ret |= __si_fld_set(hw_si, SI_FLD_TRIM_L3_OFF_ID, si->trim_l3_id);
	ret |= __si_fld_set(hw_si, SI_FLD_CHKR_FLAGS,     si->chck_flags);
	ret |= __si_fld_set(hw_si, SI_FLD_PL2P,           si->pl2p);
	ret |= __si_fld_set(hw_si, SI_FLD_EXT_REASSEMBLY, si->ext_reassembly);
	ret |= __si_fld_set(hw_si, SI_FLD_INT_REASSEMBLY, si->int_reassembly);
	ret |= __si_fld_set(hw_si, SI_FLD_PS_COPY,        si->ps_copy);
	ret |= __si_fld_set(hw_si, SI_FLD_SEG_EN,         si->seg_en);
	ret |= __si_fld_set(hw_si, SI_FLD_BSL_PRIO,       si->fsqm_prio);
	ret |= __si_fld_set(hw_si, SI_FLD_PLICIES_BITMAP, si->policies_map);
	ret |= __si_fld_set(hw_si, SI_FLD_DF_MASK,        df_mask);
	ret |= __si_fld_set(hw_si, SI_FLD_PKT_LEN_DIFF,   si->pkt_len_diff);
	ret |= __si_fld_set(hw_si, SI_FLD_TMPL_UD_SZ,     si->tmpl_ud_sz);
	ret |= __pp_si_sgc_encode(hw_si, si);
	ret |= __pp_si_tbm_encode(hw_si, si);

	return ret;
}

s32 pp_si_encode(struct pp_hw_si *hw_si, struct pp_si *si)
{
	s32 ret = 0;

	if (unlikely(!(!ptr_is_null(si) &&
		       !ptr_is_null(hw_si))))
		return -EINVAL;

	ret |= pp_si_encode_top(hw_si, si);
	ret |= __pp_si_ud_encode(hw_si, si);
	ret |= __pp_si_dpu_encode(hw_si, si);
	ret |= __pp_si_sce_encode(hw_si, si);
	ret |= __pp_si_bce_encode(hw_si, si);
	ret |= __pp_si_fv_encode(hw_si, si);

	if (unlikely(ret))
		pr_err("Failed to encode si\n");
	return ret;
}

/* ========================================================================== */
/*                                  Decoding                                  */
/* ========================================================================== */
/**
 * @brief Decode SI SGC indexes from hw SI buffer
 */
static void __pp_si_sgc_decode(struct pp_si *si, const struct pp_hw_si *hw_si)
{
	u32 i;

	/* decode sgc bitmap */
	si->sgc_en_map = (ulong)__si_fld_get(hw_si, SI_FLD_SGC_BITMAP);

	/* decode sgc IDs */
	for (i = 0; i < ARRAY_SIZE(si->sgc); i++) {
		if (test_bit(i, &si->sgc_en_map))
			si->sgc[i] = (u16)__si_fld_get(hw_si, SI_FLD_SGC1 + i);
		else
			si->sgc[i] = PP_SGC_INVALID;
	}
}

/**
 * @brief Decode SI TBM indexes from hw SI buffer
 */
static void __pp_si_tbm_decode(struct pp_si *si, const struct pp_hw_si *hw_si)
{
	u32 i;

	/* decode tbm bitmap */
	si->tbm_en_map = (ulong)__si_fld_get(hw_si, SI_FLD_TBM_BITMAP);

	/* decode tbm IDs */
	for (i = 0; i < ARRAY_SIZE(si->tbm); i++) {
		if (test_bit(i, &si->tbm_en_map))
			si->tbm[i] = (u16)__si_fld_get(hw_si, SI_FLD_TBM1 + i);
		else
			si->tbm[i] = PP_TBM_INVALID;
	}
}

/**
 * @brief Decode SI UD region from hw SI buffer
 */
void __pp_si_ud_decode(struct pp_si *si, const struct pp_hw_si *hw_si)
{
	const u8 *src = (const u8 *)hw_si + PP_SI_UD_OFFSET;

	if (!si->si_ud_sz && (si->ps_off == PP_INVALID_PS_OFF))
		return;

	if (si->ps_off != PP_INVALID_PS_OFF)
		si->si_ps_sz = PP_PS_REGION_SZ - si->ps_off;

	memcpy(si->ud, src, max_t(u8, si->si_ps_sz, si->si_ud_sz));
}

/**
 * @brief Decode SI dpu region from hw SI buffer
 */
void __pp_si_dpu_decode(struct pp_si *si, const struct pp_hw_si *hw_si)
{
	u8 dpu_start = __si_fld_get(hw_si, SI_FLD_DPU_START);

	if (dpu_start == SI_INVALID_DPU_START)
		return;

	si->dpu_start       = dpu_start;
	si->dpu.nat_sz      = __si_fld_get(hw_si, SI_FLD_DPU_NAT_SZ);
	si->dpu.pppoe_off   = __si_fld_get(hw_si, SI_FLD_DPU_PPPOE_OFF);
	si->dpu.nhdr_sz     = __si_fld_get(hw_si, SI_FLD_DPU_NHDR_SZ);
	si->dpu.nhdr_l3_off = __si_fld_get(hw_si, SI_FLD_DPU_NHDR_L3_OFF);
	si->dpu.lyr_fld_off = __si_fld_get(hw_si, SI_FLD_DPU_LYR_FLD_OFF);
}

/**
 * @brief Decode SI sce region from hw SI buffer
 */
void __pp_si_sce_decode(struct pp_si *si, const struct pp_hw_si *hw_si)
{
	u8 sce_start = __si_fld_get(hw_si, SI_FLD_SCE_START);

	si->sce.valid = (sce_start != SI_INVALID_SCE_START);
	if (!SI_IS_SCE_EXIST(si))
		return;

	si->sce_start           = sce_start;
	si->sce.l4_csum_zero    = __si_fld_get(hw_si, SI_FLD_SCE_L4_CSUM_ZERO);
	si->sce.l4_csum_delta   = __si_fld_get(hw_si, SI_FLD_SCE_L4_CSUM);
	si->sce.l3_csum_delta   = __si_fld_get(hw_si, SI_FLD_SCE_L3_CSUM);
	si->sce.tot_len_diff    = __si_fld_get(hw_si, SI_FLD_SCE_IP_LEN_DIFF);
	si->sce.dscp            = __si_fld_get(hw_si, SI_FLD_SCE_DSCP);
	si->sce.nhdr_csum       = __si_fld_get(hw_si, SI_FLD_SCE_NHDR_CSUM);
	si->sce.new_src_port    = __si_fld_get(hw_si, SI_FLD_SCE_SRC_PORT);
	si->sce.new_dst_port    = __si_fld_get(hw_si, SI_FLD_SCE_DST_PORT);
	si->sce.ttl_diff        = __si_fld_get(hw_si, SI_FLD_SCE_TTL_DIFF);
	si->sce.pppoe_diff      = __si_fld_get(hw_si, SI_FLD_SCE_PPPOE_DIFF);
	si->sce.l2_org_val      = __si_fld_get(hw_si, SI_FLD_SCE_L2_ORG_VAL);
}

/**
 * @brief Decode SI bce region from hw SI buffer
 */
void __pp_si_bce_decode(struct pp_si *si, const struct pp_hw_si *hw_si)
{
	const char *src;
	u8 nat_sz;

	si->bce_start = __si_fld_get(hw_si, SI_FLD_BCE_START);
	si->bce_sz    = __si_fld_get(hw_si, SI_FLD_BCE_SZ);
	si->bce_ext   = __si_fld_get(hw_si, SI_FLD_BCE_EXT);

	src    = (const char *)hw_si + si->bce_start;
	nat_sz = roundup(si->dpu.nat_sz, 16);
	/* copy NAT */
	if (si->dpu.nat_sz)
		memcpy(&si->bce.nat, src, si->dpu.nat_sz);

	/* copy new header */
	if (si->dpu.nhdr_sz)
		memcpy(si->bce.nhdr, src + nat_sz, si->dpu.nhdr_sz);
}

/**
 * @brief Decode field vector from hw SI buffer
 */
void __pp_si_fv_decode(struct pp_si *si, const struct pp_hw_si *hw_si)
{
	si->fv_sz = __si_fld_get(hw_si, SI_FLD_FV_SZ);
	__buf_swap(&si->fv, sizeof(si->fv),
		   (const char *)hw_si + sizeof(*hw_si) - si->fv_sz, si->fv_sz);
}

s32 pp_si_decode_top(struct pp_si *si, const struct pp_hw_si *hw_si)
{
	u8 df_mask;

	si->recipe_idx     = __si_fld_get(hw_si, SI_FLD_RECIPE_ID);
	si->sess_id        = __si_fld_get(hw_si, SI_FLD_SESS_ID);
	si->si_ud_sz       = __si_fld_get(hw_si, SI_FLD_SI_UD_SZ);
	si->ps_off         = __si_fld_get(hw_si, SI_FLD_PS_OFF);
	si->base_policy    = __si_fld_get(hw_si, SI_FLD_PLCY_BASE);
	si->color          = __si_fld_get(hw_si, SI_FLD_COLOR);
	si->tdox_flow      = __si_fld_get(hw_si, SI_FLD_TDOX_FLOW);
	si->dst_q          = __si_fld_get(hw_si, SI_FLD_DST_QUEUE);
	si->eg_port        = __si_fld_get(hw_si, SI_FLD_EGRS_PORT);
	si->trim_l3_id     = __si_fld_get(hw_si, SI_FLD_TRIM_L3_OFF_ID);
	si->chck_flags     = __si_fld_get(hw_si, SI_FLD_CHKR_FLAGS);
	si->pl2p           = __si_fld_get(hw_si, SI_FLD_PL2P);
	si->tmpl_ud_sz     = __si_fld_get(hw_si, SI_FLD_TMPL_UD_SZ);
	si->ext_reassembly = __si_fld_get(hw_si, SI_FLD_EXT_REASSEMBLY);
	si->int_reassembly = __si_fld_get(hw_si, SI_FLD_INT_REASSEMBLY);
	si->ps_copy        = __si_fld_get(hw_si, SI_FLD_PS_COPY);
	si->seg_en         = __si_fld_get(hw_si, SI_FLD_SEG_EN);
	si->fsqm_prio      = __si_fld_get(hw_si, SI_FLD_BSL_PRIO);
	si->policies_map   = __si_fld_get(hw_si, SI_FLD_PLICIES_BITMAP);
	df_mask            = __si_fld_get(hw_si, SI_FLD_DF_MASK);
	si->pkt_len_diff   = __si_fld_get(hw_si, SI_FLD_PKT_LEN_DIFF);
	__pp_si_sgc_decode(si, hw_si);
	__pp_si_tbm_decode(si, hw_si);

	si->ext_df_mask = SI_DF_MASK_GET_EXT(df_mask);
	si->int_df_mask = SI_DF_MASK_GET_INT(df_mask);

	return 0;
}

s32 pp_si_decode(struct pp_si *si, const struct pp_hw_si *hw_si)
{
	if (unlikely(!(!ptr_is_null(si) &&
		       !ptr_is_null(hw_si))))
		return -EINVAL;

	pp_si_decode_top(si, hw_si);
	__pp_si_ud_decode(si, hw_si);
	__pp_si_dpu_decode(si, hw_si);
	__pp_si_sce_decode(si, hw_si);
	__pp_si_bce_decode(si, hw_si);
	__pp_si_fv_decode(si, hw_si);

	return 0;
}

s32 pp_dsi_decode(struct pp_dsi *dsi, const struct pp_hw_dsi *hw_dsi)
{
	if (unlikely(!(!ptr_is_null(dsi) &&
		       !ptr_is_null(hw_dsi))))
		return -EINVAL;

	dsi->bytes_cnt = __dsi_fld_get(hw_dsi, DSI_FLD_BYTES_CNT);
	dsi->pkts_cnt  = __dsi_fld_get(hw_dsi, DSI_FLD_PKTS_CNT);
	dsi->dst_q     = __dsi_fld_get(hw_dsi, DSI_FLD_DST_Q);
	dsi->divert    = __dsi_fld_get(hw_dsi, DSI_FLD_DVRT);
	dsi->stale     = __dsi_fld_get(hw_dsi, DSI_FLD_STALE);
	dsi->active    = __dsi_fld_get(hw_dsi, DSI_FLD_ACTIVE);
	dsi->valid     = __dsi_fld_get(hw_dsi, DSI_FLD_VALID);

	return 0;
}

s32 pp_si_port_info_set(u16 in_port, u16 eg_port, struct pp_si *si)
{
	s32 ret    = 0;
	u8  tx_map = 0;
	u8  rx_map = 0;
	u8  mask   = 0;

	if (unlikely(ptr_is_null(si)))
		return -EINVAL;

	si->eg_port = eg_port;
	ret = pp_port_tx_policy_info_get(eg_port, &si->base_policy, &tx_map);
	if (unlikely(ret)) {
		pr_err("failed to get port %u tx policy info\n", eg_port);
		return ret;
	}

	ret = pp_port_seg_en_get(eg_port, &si->seg_en);
	if (unlikely(ret)) {
		pr_err("failed to get port %u seg enable\n", eg_port);
		return ret;
	}

	ret = pp_port_prel2_en_get(eg_port, &si->pl2p);
	if (unlikely(ret)) {
		pr_err("failed to get port %u prel2 enable\n", eg_port);
		return ret;
	}

	/* For exception session there is no ingress port */
	if (likely(PP_IS_PORT_VALID(in_port))) {
		ret = pp_port_rx_policy_info_get(in_port, &rx_map);
		if (unlikely(ret)) {
			pr_err("failed to get port %u rx policy info\n",
			       in_port);
			return ret;
		}
	}

	pr_debug("port_rx_map %#x, port_tx_map %#x, port_tx_base_policy %u\n",
		 rx_map, tx_map, si->base_policy);

	/* build the session policies bitmap, the bitmap it built from rx & tx
	 * common policies and from all policies of the tx that are higher
	 * than the lowest common policy, this is only true in case rx policies
	 * exist or rx and tx has common policies, if they don't we just use
	 * the tx policies, from rx_dma perspective, it means that all packets
	 * received on the port will be copied to a buffer from one of
	 * the tx policies
	 * for example: tx policies = 0b1110, rx policies: 0b0100
	 *              ==> session policies: 0b1100
	 */
	if (!rx_map || !(rx_map & tx_map)) {
		si->policies_map = tx_map;
		goto done;
	}

	si->policies_map = rx_map & tx_map;
	mask = ~((u8)GENMASK(__ffs(si->policies_map) - 1, 0));
	si->policies_map |= tx_map & mask;
	si->base_policy += __ffs(si->policies_map) - __ffs(tx_map);

done:
	pr_debug("si->base_policy %u, si->policies_map %#x\n",
		 si->base_policy, si->policies_map);
	return ret;
}

s32 pp_si_sgc_info_set(u16 *sgc, u16 n_sgc, struct pp_si *si)
{
	u32 grp;

	if (unlikely(!(!ptr_is_null(si) &&
		       !ptr_is_null(sgc))))
		return -EINVAL;

	if (unlikely(!(n_sgc <= PP_SI_SGC_MAX))) {
		pr_info("Too much sgc entries %u, max allowed %u\n",
			n_sgc, PP_SI_SGC_MAX);
		return -EINVAL;
	}

	si->sgc_en_map = 0;
	for (grp = 0; grp < n_sgc; grp++) {
		/* ignore invalid entries */
		if (sgc[grp] == PP_SGC_INVALID)
			continue;
		if (unlikely(chk_sgc_session_attach(grp, sgc[grp]))) {
			pr_err("Failed to attach sgc [%u][%04u]\n",
			       grp, sgc[grp]);
			goto sgc_err;
		}
		si->sgc[grp] = sgc[grp];
		set_bit(grp, &si->sgc_en_map);
	}

	return 0;

sgc_err:
	while (grp--) {
		if (sgc[grp] == PP_SGC_INVALID)
			continue;
		chk_sgc_session_detach(grp, sgc[grp]);
	}

	return -EINVAL;
}

s32 pp_si_tbm_info_set(u16 *tbm, u16 n_tbm, struct pp_si *si)
{
	u32 i;

	if (unlikely(!(!ptr_is_null(si) &&
		       !ptr_is_null(tbm))))
		return -EINVAL;

	if (unlikely(!(n_tbm <= PP_SI_TBM_MAX))) {
		pr_info("Too much tbm entries %u, max allowed %u\n",
			n_tbm, PP_SI_TBM_MAX);
		return -EINVAL;
	}

	si->tbm_en_map = 0;
	for (i = 0; i < n_tbm; i++) {
		if (tbm[i] == PP_TBM_INVALID)
			continue;
		if (unlikely(chk_dual_tbm_session_attach(tbm[i]))) {
			pr_err("Failed to attach dual tbm %u\n", tbm[i]);
			goto tbm_err;
		}

		si->tbm[i] = tbm[i];
		set_bit(i, &si->tbm_en_map);
	}

	return 0;
tbm_err:
	/* clean up all the tbms that we attached */
	while (i--) {
		if (tbm[i] == PP_TBM_INVALID)
			continue;
		chk_dual_tbm_session_detach(tbm[i]);
	}

	return -EINVAL;
}

void pp_si_init(void)
{
	u32 idx;
	u32 newlen;
	char prefix[] = "SI_FLD_";
	u32 prefix_len = strlen(prefix);

	/* Init all SI simple static fields that doesn't require any
	 * validity check or any special conversions
	 */
	BUF_FLD_INIT_SIMPLE(si_flds, SI_FLD_RECIPE_ID);
	BUF_FLD_INIT_SIMPLE(si_flds, SI_FLD_SESS_ID);
	BUF_FLD_INIT_SIMPLE(si_flds, SI_FLD_PLCY_BASE);
	BUF_FLD_INIT_SIMPLE(si_flds, SI_FLD_DF_MASK);
	BUF_FLD_INIT_SIMPLE(si_flds, SI_FLD_PS_OFF);
	BUF_FLD_INIT_SIMPLE(si_flds, SI_FLD_BCE_EXT);
	BUF_FLD_INIT_SIMPLE(si_flds, SI_FLD_EGRS_PORT);
	BUF_FLD_INIT_SIMPLE(si_flds, SI_FLD_DST_QUEUE);
	BUF_FLD_INIT_SIMPLE(si_flds, SI_FLD_TDOX_FLOW);
	BUF_FLD_INIT_SIMPLE(si_flds, SI_FLD_EXT_REASSEMBLY);
	BUF_FLD_INIT_SIMPLE(si_flds, SI_FLD_INT_REASSEMBLY);
	BUF_FLD_INIT_SIMPLE(si_flds, SI_FLD_PS_COPY);
	BUF_FLD_INIT_SIMPLE(si_flds, SI_FLD_PL2P);
	BUF_FLD_INIT_SIMPLE(si_flds, SI_FLD_CHKR_FLAGS);
	BUF_FLD_INIT_SIMPLE(si_flds, SI_FLD_TRIM_L3_OFF_ID);
	BUF_FLD_INIT_SIMPLE(si_flds, SI_FLD_SGC_BITMAP);
	BUF_FLD_INIT_SIMPLE(si_flds, SI_FLD_SGC1);
	BUF_FLD_INIT_SIMPLE(si_flds, SI_FLD_SGC2);
	BUF_FLD_INIT_SIMPLE(si_flds, SI_FLD_SGC3);
	BUF_FLD_INIT_SIMPLE(si_flds, SI_FLD_SGC4);
	BUF_FLD_INIT_SIMPLE(si_flds, SI_FLD_SGC5);
	BUF_FLD_INIT_SIMPLE(si_flds, SI_FLD_SGC6);
	BUF_FLD_INIT_SIMPLE(si_flds, SI_FLD_SGC7);
	BUF_FLD_INIT_SIMPLE(si_flds, SI_FLD_SGC8);
	BUF_FLD_INIT_SIMPLE(si_flds, SI_FLD_TBM_BITMAP);
	BUF_FLD_INIT_SIMPLE(si_flds, SI_FLD_TBM1);
	BUF_FLD_INIT_SIMPLE(si_flds, SI_FLD_TBM2);
	BUF_FLD_INIT_SIMPLE(si_flds, SI_FLD_TBM3);
	BUF_FLD_INIT_SIMPLE(si_flds, SI_FLD_TBM4);
	BUF_FLD_INIT_SIMPLE(si_flds, SI_FLD_TBM5);
	BUF_FLD_INIT_SIMPLE(si_flds, SI_FLD_PLICIES_BITMAP);
	BUF_FLD_INIT_SIMPLE(si_flds, SI_FLD_BSL_PRIO);
	BUF_FLD_INIT_SIMPLE(si_flds, SI_FLD_SEG_EN);
	/* Init all SI static fields that requires only special
	 * validity check
	 */
	BUF_FLD_INIT_VALID(si_flds, SI_FLD_COLOR, __SI_FLD_COLOR_IS_VALID);
	/* Init all SI static fields that requires special validity
	 * check and granularity conversion
	 */
	SI_FLD_INIT(si_flds, SI_FLD_SI_UD_SZ);
	SI_FLD_INIT(si_flds, SI_FLD_DPU_START);
	SI_FLD_INIT(si_flds, SI_FLD_SCE_START);
	SI_FLD_INIT(si_flds, SI_FLD_FV_SZ);
	SI_FLD_INIT(si_flds, SI_FLD_BCE_SZ);
	SI_FLD_INIT(si_flds, SI_FLD_BCE_START);
	SI_FLD_INIT(si_flds, SI_FLD_TMPL_UD_SZ);
	SI_FLD_INIT(si_flds, SI_FLD_PKT_LEN_DIFF);
	/* Init all dpu fields */
	SI_DPU_DFLD_INIT(si_flds, SI_FLD_DPU_PPPOE_OFF);
	SI_DPU_DFLD_INIT(si_flds, SI_FLD_DPU_NAT_SZ);
	SI_DPU_DFLD_INIT(si_flds, SI_FLD_DPU_NHDR_SZ);
	SI_DPU_DFLD_INIT(si_flds, SI_FLD_DPU_NHDR_L3_OFF);
	SI_DPU_DFLD_INIT(si_flds, SI_FLD_DPU_LYR_FLD_OFF);
	/* Init all simple sce fields */
	SI_SCE_DFLD_INIT(si_flds, SI_FLD_SCE_IP_LEN_DIFF);
	SI_SCE_DFLD_INIT(si_flds, SI_FLD_SCE_DSCP);
	SI_SCE_DFLD_INIT(si_flds, SI_FLD_SCE_TTL_DIFF);
	SI_SCE_DFLD_INIT(si_flds, SI_FLD_SCE_PPPOE_DIFF);
	/* Init all sce fields which also need le16 conversion */
	SI_SCE_DFLD_INIT_LE16(si_flds, SI_FLD_SCE_SRC_PORT);
	SI_SCE_DFLD_INIT_LE16(si_flds, SI_FLD_SCE_DST_PORT);
	SI_SCE_DFLD_INIT_LE16(si_flds, SI_FLD_SCE_L4_CSUM_ZERO);
	SI_SCE_DFLD_INIT_LE16(si_flds, SI_FLD_SCE_L4_CSUM);
	SI_SCE_DFLD_INIT_LE16(si_flds, SI_FLD_SCE_L3_CSUM);
	SI_SCE_DFLD_INIT_LE16(si_flds, SI_FLD_SCE_NHDR_CSUM);
	SI_SCE_DFLD_INIT_LE16(si_flds, SI_FLD_SCE_L2_ORG_VAL);

	/* Init dynamic SI fields */
	if (pp_silicon_step_get() == PP_SSTEP_A) {
		BUF_FLD_INIT_PREFIX(dsi_flds, DSI_FLD_BYTES_CNT, DDR_);
		BUF_FLD_INIT_PREFIX(dsi_flds, DSI_FLD_PKTS_CNT, DDR_);
		BUF_FLD_INIT_PREFIX(dsi_flds, DSI_FLD_DST_Q, DDR_);
		BUF_FLD_INIT_PREFIX(dsi_flds, DSI_FLD_DVRT, DDR_);
		BUF_FLD_INIT_PREFIX(dsi_flds, DSI_FLD_STALE, DDR_);
		BUF_FLD_INIT_PREFIX(dsi_flds, DSI_FLD_ACTIVE, DDR_);
		BUF_FLD_INIT_PREFIX(dsi_flds, DSI_FLD_VALID, DDR_);
	} else {
		BUF_FLD_INIT_SIMPLE(dsi_flds, DSI_FLD_BYTES_CNT);
		BUF_FLD_INIT_SIMPLE(dsi_flds, DSI_FLD_PKTS_CNT);
		BUF_FLD_INIT_SIMPLE(dsi_flds, DSI_FLD_DST_Q);
		BUF_FLD_INIT_SIMPLE(dsi_flds, DSI_FLD_DVRT);
		BUF_FLD_INIT_SIMPLE(dsi_flds, DSI_FLD_STALE);
		BUF_FLD_INIT_SIMPLE(dsi_flds, DSI_FLD_ACTIVE);
		BUF_FLD_INIT_SIMPLE(dsi_flds, DSI_FLD_VALID);
	}

	/* Save fields names as lower case and without the SI_FLD_ prefix */
	for (idx = SI_FLD_FIRST; idx < SI_FLDS_NUM; idx++) {
		if (strlen(si_flds[idx].name) > prefix_len &&
		    strstr(si_flds[idx].name, prefix)) {
			newlen = strlen(si_flds[idx].name) - prefix_len;
			memmove(si_flds[idx].name,
				&si_flds[idx].name[prefix_len],
				newlen);
			si_flds[idx].name[newlen] = '\0';
			str_tolower(si_flds[idx].name);
		}
	}
}

