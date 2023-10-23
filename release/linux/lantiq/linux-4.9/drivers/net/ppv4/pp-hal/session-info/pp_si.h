/*
 * pp_si.h
 * Description: PP session information (SI) definitions
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2018-2020 Intel Corporation
 */

#ifndef __PP_SI_H__
#define __PP_SI_H__

#include <linux/types.h>
#include <linux/bitops.h>
#include <linux/pp_api.h>
#include "pp_fv.h"

#ifdef CONFIG_PPV4_LGM
/**
 * @brief SI UD region size in bytes
 */
#define PP_SI_TMPL_UD_SZ                (32)

/**
 * @brief SI DPU region size in bytes
 */
#define PP_SI_DPU_REGION_SZ             (16)

/**
 * @brief SI SCE region size in bytes
 *        the real size of the sce region is 64 bytes
 *        it is limited to 16 due to the way the SI looks
 */
#define PP_SI_SCE_REGION_SZ             (16)

/**
 * @brief SI BCE region size in bytes
 */
#define PP_SI_BCE_REGION_SZ             (160)

/**
 * @brief SI maximum new header size in bytes
 */
#define PP_SI_MAX_NEW_HDR_SZ            (160)

/**
 * @brief SI maximum new header size in bytes
 */
#define PP_SI_MAX_NEW_HDR_SZ            (160)

/**
 * @brief SI top size in 32bit words granularity
 */
#define PP_SI_TOP_WORDS_CNT             (8)
#define PP_SI_TOP_SZ                    (PP_SI_TOP_WORDS_CNT * sizeof(u32))

/**
 * @brief SI size in 32bit words granularity
 */
#define PP_SI_WORDS_CNT                 (64)

/**
 * @brief SI size in 32bit words granularity
 */
#define PP_DSI_WORDS_CNT                 (4)

/**
 * @brief SI UD region offset, if UD exist in the session than this is
 *        the offset he should be written to
 */
#define PP_SI_UD_OFFSET                 (0x20)

/**
 * @brief SI UD region size in bytes
 */
#define PP_UD_REGION_SZ                 (48)

/**
 * @brief SI trim L3 offset id value for specifying no trim is
 *        required
 */
#define SI_NO_TRIM_VALUE                (0x7)

/**
 * @brief SI protocol specific region size in bytes
 */
#define PP_PS_REGION_SZ                 (6)

/**
 * @brief Value defines an invalid value to specify the PP not
 *        to copy the UD from SI to the STW PS (protocol
 *        specific) region
 */
#define PP_INVALID_PS_OFF               (6)

/**
 * @brief SI fields enumeration
 */
enum pp_si_fld {
	SI_FLD_FIRST,
	SI_FLD_RECIPE_ID = SI_FLD_FIRST,
	SI_FLD_SESS_ID,
	SI_FLD_PLCY_BASE,
	SI_FLD_DF_MASK,
	SI_FLD_PS_OFF,
	SI_FLD_SI_UD_SZ,
	SI_FLD_BCE_EXT,
	SI_FLD_BCE_SZ,
	SI_FLD_BCE_START,
	SI_FLD_FV_SZ,
	SI_FLD_SCE_START,
	SI_FLD_DPU_START,
	SI_FLD_EGRS_PORT,
	SI_FLD_DST_QUEUE,
	SI_FLD_TDOX_FLOW,
	SI_FLD_COLOR,
	SI_FLD_PKT_LEN_DIFF,
	SI_FLD_EXT_REASSEMBLY,
	SI_FLD_INT_REASSEMBLY,
	SI_FLD_PS_COPY,
	SI_FLD_TMPL_UD_SZ,
	SI_FLD_PL2P,
	SI_FLD_CHKR_FLAGS,
	SI_FLD_TRIM_L3_OFF_ID,
	SI_FLD_SGC_BITMAP,
	SI_FLD_SGC1,
	SI_FLD_SGC2,
	SI_FLD_SGC3,
	SI_FLD_SGC4,
	SI_FLD_SGC5,
	SI_FLD_SGC6,
	SI_FLD_SGC7,
	SI_FLD_SGC8,
	SI_FLD_TBM_BITMAP,
	SI_FLD_TBM1,
	SI_FLD_TBM2,
	SI_FLD_TBM3,
	SI_FLD_TBM4,
	SI_FLD_TBM5,
	SI_FLD_PLICIES_BITMAP,
	SI_FLD_BSL_PRIO,
	SI_FLD_SEG_EN,
	SI_FLD_DPU_PPPOE_OFF,
	SI_FLD_DPU_NHDR_SZ,
	SI_FLD_DPU_NAT_SZ,
	SI_FLD_DPU_NHDR_L3_OFF,
	SI_FLD_DPU_LYR_FLD_OFF,
	SI_FLD_SCE_L4_CSUM_ZERO,
	SI_FLD_SCE_L4_CSUM,
	SI_FLD_SCE_L3_CSUM,
	SI_FLD_SCE_IP_LEN_DIFF,
	SI_FLD_SCE_DSCP,
	SI_FLD_SCE_NHDR_CSUM,
	SI_FLD_SCE_SRC_PORT,
	SI_FLD_SCE_DST_PORT,
	SI_FLD_SCE_TTL_DIFF,
	SI_FLD_SCE_PPPOE_DIFF,
	SI_FLD_SCE_L2_ORG_VAL,
	SI_FLD_LAST = SI_FLD_SCE_L2_ORG_VAL,
	SI_FLDS_NUM
};

static inline bool pp_si_field_id_is_valid(enum pp_si_fld id)
{
	if (likely(id >= SI_FLD_FIRST &&
		   id <= SI_FLD_LAST))
		return true;

	pr_err("invalid SI field index %u, valid range is %u-%u\n",
	       id, SI_FLD_FIRST, SI_FLD_LAST);
	return false;
}

/**
 * @brief Session information fields masks
 */
#define SI_FLD_RECIPE_ID_LSB            (0)
#define SI_FLD_RECIPE_ID_MSB            (7)
#define SI_FLD_SESS_ID_LSB              (8)
#define SI_FLD_SESS_ID_MSB              (31)
#define SI_FLD_PLCY_BASE_LSB            (32)
#define SI_FLD_PLCY_BASE_MSB            (39)
#define SI_FLD_DF_MASK_LSB              (40)
#define SI_FLD_DF_MASK_MSB              (41)
#define SI_FLD_PS_OFF_LSB               (42)
#define SI_FLD_PS_OFF_MSB               (44)
#define SI_FLD_SI_UD_SZ_LSB             (45)
#define SI_FLD_SI_UD_SZ_MSB             (46)
#define SI_FLD_BCE_EXT_LSB              (47)
#define SI_FLD_BCE_EXT_MSB              (47)
#define SI_FLD_BCE_SZ_LSB               (48)
#define SI_FLD_BCE_SZ_MSB               (51)
#define SI_FLD_BCE_START_LSB            (52)
#define SI_FLD_BCE_START_MSB            (55)
#define SI_FLD_FV_SZ_LSB                (56)
#define SI_FLD_FV_SZ_MSB                (58)
#define SI_FLD_SCE_START_LSB            (59)
#define SI_FLD_SCE_START_MSB            (61)
#define SI_FLD_DPU_START_LSB            (62)
#define SI_FLD_DPU_START_MSB            (63)
#define SI_FLD_EGRS_PORT_LSB            (64)
#define SI_FLD_EGRS_PORT_MSB            (71)
#define SI_FLD_DST_QUEUE_LSB            (72)
#define SI_FLD_DST_QUEUE_MSB            (83)
#define SI_FLD_TDOX_FLOW_LSB            (84)
#define SI_FLD_TDOX_FLOW_MSB            (93)
#define SI_FLD_COLOR_LSB                (94)
#define SI_FLD_COLOR_MSB                (95)
#define SI_FLD_PKT_LEN_DIFF_LSB         (96)
#define SI_FLD_PKT_LEN_DIFF_MSB         (104)
#define SI_FLD_EXT_REASSEMBLY_LSB       (105)
#define SI_FLD_EXT_REASSEMBLY_MSB       (105)
#define SI_FLD_INT_REASSEMBLY_LSB       (106)
#define SI_FLD_INT_REASSEMBLY_MSB       (106)
#define SI_FLD_PS_COPY_LSB              (108)
#define SI_FLD_PS_COPY_MSB              (108)
#define SI_FLD_TMPL_UD_SZ_LSB           (109)
#define SI_FLD_TMPL_UD_SZ_MSB           (110)
#define SI_FLD_PL2P_LSB                 (111)
#define SI_FLD_PL2P_MSB                 (111)
#define SI_FLD_CHKR_FLAGS_LSB           (112)
#define SI_FLD_CHKR_FLAGS_MSB           (124)
#define SI_FLD_TRIM_L3_OFF_ID_LSB       (125)
#define SI_FLD_TRIM_L3_OFF_ID_MSB       (127)
#define SI_FLD_SGC_BITMAP_LSB           (128)
#define SI_FLD_SGC_BITMAP_MSB           (135)
#define SI_FLD_SGC1_LSB                 (136)
#define SI_FLD_SGC1_MSB                 (145)
#define SI_FLD_SGC2_LSB                 (146)
#define SI_FLD_SGC2_MSB                 (151)
#define SI_FLD_SGC3_LSB                 (152)
#define SI_FLD_SGC3_MSB                 (159)
#define SI_FLD_SGC4_LSB                 (160)
#define SI_FLD_SGC4_MSB                 (169)
#define SI_FLD_SGC5_LSB                 (170)
#define SI_FLD_SGC5_MSB                 (175)
#define SI_FLD_SGC6_LSB                 (176)
#define SI_FLD_SGC6_MSB                 (183)
#define SI_FLD_SGC7_LSB                 (184)
#define SI_FLD_SGC7_MSB                 (189)
#define SI_FLD_SGC8_LSB                 (190)
#define SI_FLD_SGC8_MSB                 (191)
#define SI_FLD_TBM_BITMAP_LSB           (192)
#define SI_FLD_TBM_BITMAP_MSB           (196)
#define SI_FLD_TBM1_LSB                 (200)
#define SI_FLD_TBM1_MSB                 (207)
#define SI_FLD_TBM2_LSB                 (208)
#define SI_FLD_TBM2_MSB                 (215)
#define SI_FLD_TBM3_LSB                 (216)
#define SI_FLD_TBM3_MSB                 (223)
#define SI_FLD_TBM4_LSB                 (224)
#define SI_FLD_TBM4_MSB                 (231)
#define SI_FLD_TBM5_LSB                 (232)
#define SI_FLD_TBM5_MSB                 (239)
#define SI_FLD_PLICIES_BITMAP_LSB       (248)
#define SI_FLD_PLICIES_BITMAP_MSB       (252)
#define SI_FLD_BSL_PRIO_LSB             (253)
#define SI_FLD_BSL_PRIO_MSB             (254)
#define SI_FLD_SEG_EN_LSB               (255)
#define SI_FLD_SEG_EN_MSB               (255)

#define SI_FLD_DPU_PPPOE_OFF_LSB        (8)
#define SI_FLD_DPU_PPPOE_OFF_MSB        (15)
#define SI_FLD_DPU_NHDR_SZ_LSB          (16)
#define SI_FLD_DPU_NHDR_SZ_MSB          (23)
#define SI_FLD_DPU_NAT_SZ_LSB           (24)
#define SI_FLD_DPU_NAT_SZ_MSB           (31)
#define SI_FLD_DPU_NHDR_L3_OFF_LSB      (32)
#define SI_FLD_DPU_NHDR_L3_OFF_MSB      (39)
#define SI_FLD_DPU_LYR_FLD_OFF_LSB      (40)
#define SI_FLD_DPU_LYR_FLD_OFF_MSB      (47)

/* all below fields are sce fields which are actually resides in
 * the dpu region due to lack of space in the sce region
 */
#define SI_FLD_SCE_L4_CSUM_ZERO_LSB     (112)
#define SI_FLD_SCE_L4_CSUM_ZERO_MSB     (127)

/* all sce start are at offset of 128 cause we want to include all
 * dpu fields in the sce mux
 */
#define SI_FLD_SCE_L4_CSUM_LSB          (128 + 0)
#define SI_FLD_SCE_L4_CSUM_MSB          (128 + 15)
#define SI_FLD_SCE_L3_CSUM_LSB          (128 + 16)
#define SI_FLD_SCE_L3_CSUM_MSB          (128 + 31)
#define SI_FLD_SCE_IP_LEN_DIFF_LSB      (128 + 32)
#define SI_FLD_SCE_IP_LEN_DIFF_MSB      (128 + 39)
#define SI_FLD_SCE_DSCP_LSB             (128 + 40)
#define SI_FLD_SCE_DSCP_MSB             (128 + 47)
#define SI_FLD_SCE_NHDR_CSUM_LSB        (128 + 48)
#define SI_FLD_SCE_NHDR_CSUM_MSB        (128 + 63)
#define SI_FLD_SCE_DST_PORT_LSB         (128 + 64)
#define SI_FLD_SCE_DST_PORT_MSB         (128 + 79)
#define SI_FLD_SCE_SRC_PORT_LSB         (128 + 80)
#define SI_FLD_SCE_SRC_PORT_MSB         (128 + 95)
#define SI_FLD_SCE_TTL_DIFF_LSB         (128 + 96)
#define SI_FLD_SCE_TTL_DIFF_MSB         (128 + 103)
#define SI_FLD_SCE_PPPOE_DIFF_LSB       (128 + 104)
#define SI_FLD_SCE_PPPOE_DIFF_MSB       (128 + 111)
#define SI_FLD_SCE_L2_ORG_VAL_LSB       (128 + 112)
#define SI_FLD_SCE_L2_ORG_VAL_MSB       (128 + 127)

/**
 * @brief SI fields enumeration
 * @note DSI looks a bit different when it is being read from the DDR
 */
enum pp_dsi_fld {
	DSI_FLD_FIRST,
	DSI_FLD_BYTES_CNT = DSI_FLD_FIRST,
	DSI_FLD_PKTS_CNT,
	DSI_FLD_DST_Q,
	DSI_FLD_DVRT,
	DSI_FLD_STALE,
	DSI_FLD_ACTIVE,
	DSI_FLD_VALID,
	DSI_FLD_LAST = DSI_FLD_VALID,
	DSI_FLDS_NUM
};

static inline bool pp_dsi_field_id_is_valid(enum pp_dsi_fld id)
{
	if (likely(id >= DSI_FLD_FIRST &&
		   id <= DSI_FLD_LAST))
		return true;

	pr_err("invalid DSI field index %u, valid range is %u-%u\n",
	       id, DSI_FLD_FIRST, DSI_FLD_LAST);
	return false;
}

#define DSI_FLD_BYTES_CNT_LSB           (0)
#define DSI_FLD_BYTES_CNT_MSB           (51)
#define DSI_FLD_PKTS_CNT_LSB            (64)
#define DSI_FLD_PKTS_CNT_MSB            (105)
#define DSI_FLD_DST_Q_LSB               (106)
#define DSI_FLD_DST_Q_MSB               (117)
#define DSI_FLD_DVRT_LSB                (118)
#define DSI_FLD_DVRT_MSB                (118)
#define DSI_FLD_STALE_LSB               (119)
#define DSI_FLD_STALE_MSB               (119)
#define DSI_FLD_ACTIVE_LSB              (120)
#define DSI_FLD_ACTIVE_MSB              (120)
#define DSI_FLD_VALID_LSB               (121)
#define DSI_FLD_VALID_MSB               (121)

#define DDR_DSI_FLD_BYTES_CNT_LSB       (0)
#define DDR_DSI_FLD_BYTES_CNT_MSB       (51)
#define DDR_DSI_FLD_PKTS_CNT_LSB        (62)
#define DDR_DSI_FLD_PKTS_CNT_MSB        (103)
#define DDR_DSI_FLD_DST_Q_LSB           (104)
#define DDR_DSI_FLD_DST_Q_MSB           (115)
#define DDR_DSI_FLD_DVRT_LSB            (116)
#define DDR_DSI_FLD_DVRT_MSB            (116)
#define DDR_DSI_FLD_STALE_LSB           (125)
#define DDR_DSI_FLD_STALE_MSB           (125)
#define DDR_DSI_FLD_ACTIVE_LSB          (126)
#define DDR_DSI_FLD_ACTIVE_MSB          (126)
#define DDR_DSI_FLD_VALID_LSB           (127)
#define DDR_DSI_FLD_VALID_MSB           (127)

/**
 * @brief PP HW session information definition
 *        it is impossible to map the si hw structure exactly so we
 *        just define an array of 64 32bit registers
 */
struct pp_hw_si {
	u32 word[PP_SI_WORDS_CNT];
};

/**
 * @brief PP HW session information definition
 *        it is impossible to map the si hw structure exactly so we
 *        just define an array of 64 32bit registers
 */
struct pp_hw_dsi {
	u32 word[PP_DSI_WORDS_CNT];
};

/**
 * @brief PP session information checker flags enumeration
 * @SEND2FW always send to fw micro controller
 * @DROP_SESS specify that the session is a drop session
 * @DYN_SESS specify that the session is dynamic
 * @DVRT_L3_OPTS divert the session to the host in case of IPv4
 *               options or IPv6 extensions are seen
 * @DVRT_TTL_EXP divert the session to the host in case TTL or HOP
 *               limit expired
 * @DVRT_TCP_CTRL divert the session to the host in case TCP control
 *                packet is received
 * @DVRT_FRAG divert session to the reassembly FW once fragments are
 *            seen
 * @DVRT_MTU_OVFL divert session to fragmentor FW once packet exceed
 *                egress port MTU
 * @DVRT_TCP_ACK divert session to turbodox FW once TCP ACK packet
 *               received
 * @DVRT_MULTICAST divert session to multicast FW before modification
 * @DVRT_EXCP specify whether to divert the session to the host upon
 *            exception, see checker developer guide for more info
 * @UPDT_SESS_CNT specify whether to update the session counters in
 *                the dynamic session info memory, applicable only
 *                when the session is dynamic
 */
enum si_chck_flags {
	SI_CHCK_FLAG_SEND2FW,
	SI_CHCK_FLAG_DROP_SESS,
	SI_CHCK_FLAG_DYN_SESS,
	SI_CHCK_FLAG_DVRT_L3_OPTS,
	SI_CHCK_FLAG_DVRT_TTL_EXP,
	SI_CHCK_FLAG_DVRT_TCP_CTRL,
	SI_CHCK_FLAG_DVRT_FRAG,
	SI_CHCK_FLAG_DVRT_MTU_OVFL,
	SI_CHCK_FLAG_DVRT_TCP_ACK,
	SI_CHCK_FLAG_DVRT_MULTICAST,
	SI_CHCK_FLAG_DVRT_EXCP,
	SI_CHCK_FLAG_UPDT_SESS_CNT,
	SI_CHCK_FLAGS_NUM,
};

/**
 * @brief SI DPU region definition
 * @valid specify if dpu info is valid
 * @nat_sz nat region size in bytes
 * @nhdr_sz size of the new header in the bce region
 * @nhdr_l3_off l3 offset from the start of the new header
 * @lyr_fld_off offset to a field inside a layer in the header, e.g.
 *              offset to checksum field in udp and tcp headers
 * @pppoe_off offset to the pppoe header, 0 for no pppoe
 */
struct pp_si_dpu {
	u8 nat_sz;
	u8 nhdr_sz;
	u8 nhdr_l3_off;
	u8 lyr_fld_off;
	u8 pppoe_off;
};

/**
 * @brief SI single engine copy, AKA SCE, region
 * @valid specify if sce info is valid
 * @l4_csum_zero value which helps to avoid checksum calculations
 *               when the received packet has zero checksum
 * @l4_csum_delta l4 checksum delta due to modification in the L4
 *               which is not part of the new header
 * @l3_csum_delta l3 checksum delta due to modification in the L3
 *                which is not part of the new header
 * @new_hdr_csum new header l4 checksum
 * @dscp ipv4 tos field or ipv6 priority field
 * @tot_len_diff difference between outer to inner ip header fo tunnel
 *               encapsulations
 * @new_dst_port NAT's l4 new dest port
 * @new_src_port NAT's l4 new source port
 * @ttl_diff value to subtract from the packet's ttl/hop limit
 * @pppoe_diff length diff due to addition of pppoe header
 * @l2_org_val in cases where we don't have pppoe encapsulation
 *             we save the value from the offset of pppoe length
 *             field should be in case we had pppoe encapsulation
 *             we do it in order not to have a separate recipes
 *             for pppoe use cases
 */
struct pp_si_sce {
	bool valid;
	u16  l4_csum_zero;
	u16  l4_csum_delta;
	u16  l3_csum_delta;
	u16  nhdr_csum;
	u8   dscp;
	u8   tot_len_diff;
	u16  new_dst_port;
	u16  new_src_port;
	u8   ttl_diff;
	u8   pppoe_diff;
	u16  l2_org_val;
} __packed;

union pp_si_bce_nat {
	struct {
		__be32 saddr;
		__be32 daddr;
	} v4;
	struct {
		struct in6_addr saddr;
		struct in6_addr daddr;
	} v6;
};

/**
 * @brief SI burst engine copy, AKA BCE, region
 * @valid specify if bce info is valid
 * @nat nat region, usually should contain the new IP addresses
 * @nhdr new header region, the new header MUST be written in
 *       network endian byte order
 */
struct pp_si_bce {
	union pp_si_bce_nat nat;
	u8 nhdr[PP_SI_MAX_NEW_HDR_SZ];
} __packed;

/**
 * @brief PP session information definition, defining all fields
 *        required to encode an HW SI structure
 * @recipe_idx modification recipe index
 * @dpu_start offset to the start of the dpu region in the si, should
 *            not be set by the user, this is set by the si encode API
 * @sce_start offset to the start of the sce region in the si, should
 *            not be set by the user, this is set by the si encode API
 * @fv_sz size of the field vector
 * @bce_start offset to the start of the bce region in the si, should
 *            not be set by the user, this is set by the si encode API
 * @bce_sz size of the bce region in the si, should not be set by the
 *         user, this is set by the si encode API
 * @bce_ext set always to zero, not in use for PPv4
 * @si_ud_sz 16 bytes granularity size specifying number of
 *        bytes to copy from UD region in the SI to the
 *        status word by the checker
 * @ps_off specify the offset in the status word where the protocol
 *         specific should be copied
 * @ext_df_mask specify whether the checker should mask don't frag bit
 *              of external header in the status word
 * @int_df_mask specify whether the checker should mask don't frag bit
 *              of internal header in the status word
 * @base_policy egress port buffer manager base policy to use
 * @color session color for the qos algorithm
 * @tdox_flow tdox flow for tdox micro controller
 * @dst_q destination queue where the session packets should be sent
 *        to by the PP
 * @eg_port egress port where the session packets should be transmitted
 *          from
 * @trim_l3_off_id the parser is capable to save up to 6 protocols
 *                 offsets in an array in the status word, this id
 *                 specify the index of the protocol in that array
 *                 where the trim should start.
 *                 for example, for IPv4 UDP routed packet the MAC
 *                 header is replaced, the index should be the index
 *                 of the IPv4 in the array cause we trim only the MAC
 *                 header and in that case the index of the IPv4 in
 *                 the array in 0
 * @chck_flags flags for the checker
 * @pl2p specify if the egress port supports pre L2 info coping prior
 *       to the packet
 * @ps_copy specify whether status word protocol specific should be
 *          copied to the packet UD region
 * @tmpl_ud_sz 16 bytes granularity size specifying how much
 *             bytes the checker should copy from the status word UD
 *             to the packet UD region
 * @pkt_len_diff packet length difference due to packet modification
 * @sgc session group counters IDs array
 * @tbm token bucket meter IDs array
 * @fsqm_buff_alloc specify if buffers allocated for the session
 *                  should be fsqm buffers
 * @bsl_prio priority for fsqm buffer allocation
 * @policy_map bitmap specifying which policies can be used by
 *        the buffer manager for allocating buffers for the
 *        session
 * @ud user defined region, 48 bytes for user usage
 * @fv pp field vector
 */
struct pp_si {
	u32    sess_id;
	u8     recipe_idx;
	u8     dpu_start;
	u8     sce_start;
	u8     fv_sz;
	u8     bce_start;
	u8     bce_sz;
	bool   bce_ext;
	u8     si_ud_sz;
	u8     si_ps_sz;
	u8     ps_off;
	bool   ext_df_mask;
	bool   int_df_mask;
	bool   ext_reassembly;
	bool   int_reassembly;
	u8     base_policy;
	u8     color;
	u16    tdox_flow;
	u16    dst_q;
	u16    eg_port;
	u8     trim_l3_id;
	u16    chck_flags;
	bool   pl2p;
	u8     tmpl_ud_sz;
	bool   ps_copy;
	s16    pkt_len_diff;
	u16    sgc[PP_SI_SGC_MAX];
	ulong  sgc_en_map;
	u16    tbm[PP_SI_TBM_MAX];
	ulong  tbm_en_map;
	bool   seg_en;
	u8     fsqm_prio;
	u8     policies_map;
	u8     ud[PP_UD_REGION_SZ];
	struct pp_si_dpu dpu;
	struct pp_si_sce sce;
	struct pp_si_bce bce;
	struct pp_fv fv;
};

/**
 * @brief PP dynamic session information definition, defining all
 *        fields required to encode an HW DSI structure
 * @bytes_cnt number of accelerated bytes
 * @pkts_cnt number of accelerated packets
 * @dst_q destination queue
 * @divert specify if the session should be diverted to the host
 * @stale specify if the session is staled
 * @active specify if the session is active
 * @valid specify if the session is valid
 */
struct pp_dsi {
	u64 bytes_cnt;
	u64 pkts_cnt;
	u16 dst_q;
	bool divert;
	bool stale;
	bool active;
	bool valid;
};

/**
 * DF MASK bits in HW SI means:
 * 0 - MASK the DF bit in packet (ignore DF)
 * 1 - don't MASK the DF bit in packet (use packet DF)
 */
#define SI_DF_MASK_BUILD(ext_mask, int_mask) \
	(PP_FIELD_PREP(BIT(0), !ext_mask) |  \
	 PP_FIELD_PREP(BIT(1), !int_mask))

#define SI_DF_MASK_GET_EXT(df_mask)          \
	(!PP_FIELD_GET(BIT(0), df_mask))

#define SI_DF_MASK_GET_INT(df_mask)          \
	(!PP_FIELD_GET(BIT(1), df_mask))

#define SI_IS_DPU_EXIST(si)             ((si)->dpu.nat_sz || (si)->dpu.nhdr_sz)
#define SI_IS_BCE_EXIST(si)             (SI_IS_DPU_EXIST(si))
#define SI_IS_SCE_EXIST(si)             ((si)->sce.valid)
#define SI_MIN_FV_SZ                    (16)
#define SI_INVALID_DPU_START            (16)
#define SI_INVALID_SCE_START            (16)

/**
 * @brief Get SI field value from SI buffer
 * @param si si buffer to get the field from
 * @param id field id
 * @param val si field value
 * @return s32 0 on success, error code otherwise
 */
s32 pp_si_fld_get(const struct pp_hw_si *si, enum pp_si_fld id, s32 *val);

/**
 * @brief Set value into an SI field
 * @param si si buffer to set the field into
 * @param id field id
 * @param val value to set
 */
s32 pp_si_fld_set(struct pp_hw_si *si, enum pp_si_fld id, s32 val);

/**
 * @brief Decode Top SI hw buffer to host SI form
 * @param si host si to store the host form
 * @param hw_si hw si to decode
 * @return s32 0 on success, error code otherwise
 */
s32 pp_si_decode_top(struct pp_si *si, const struct pp_hw_si *hw_si);

/**
 * @brief Decode SI hw buffer to host SI form
 * @param si host si to store the host form
 * @param hw_si hw si to decode
 * @return s32 0 on success, error code otherwise
 */
s32 pp_si_decode(struct pp_si *si, const struct pp_hw_si *hw_si);

/**
 * @brief Encode Top SI host structure to HW form
 * @param hw_si hw si buffer
 * @param si si to decode
 * @return s32 0 on success, error code otherwise
 */
s32 pp_si_encode_top(struct pp_hw_si *hw_si, struct pp_si *si);

/**
 * @brief Encode SI host structure to HW form
 * @param hw_si hw si buffer
 * @param si si to decode
 * @return s32 0 on success, error code otherwise
 */
s32 pp_si_encode(struct pp_hw_si *hw_si, struct pp_si *si);

/**
 * @brief Get DSI field value from DSI buffer
 * @param dsi dsi buffer to get the field from
 * @param id field id
 * @param val dsi field value
 * @return s64 0 on success, error code otherwise
 */
s32 pp_dsi_fld_get(const struct pp_hw_dsi *dsi, enum pp_dsi_fld id, s64 *val);

/**
 * @brief Set value into an DSI field
 * @param dsi dsi buffer to set the field into
 * @param id field id
 * @param val value to set
 */
s32 pp_dsi_fld_set(struct pp_hw_dsi *dsi, enum pp_dsi_fld id, s32 val);

/**
 * @brief Decode DSI hw buffer to host DSI form
 * @param dsi host dsi to store the host form
 * @param hw_dsi hw dsi to decode
 * @return s32 0 on success, error code otherwise
 */
s32 pp_dsi_decode(struct pp_dsi *dsi, const struct pp_hw_dsi *hw_dsi);

/**
 * @brief Fetch all SI's port info and set it into an si structure
 * @param in_port session ingress port, set to PP_PORT_INVALID to
 *                specify ingress port isn't exist for the session,
 *                e.g. exception session
 * @param eg_port session egress port
 * @param si si to save the port info to
 * @return s32 0 on success, error code otherwise
 */
s32 pp_si_port_info_set(u16 in_port, u16 eg_port, struct pp_si *si);

/**
 * @brief Set all session's si sgc info into an si structure
 * @param sgc sgc indexes array
 * @param n_sgc array size
 * @param si si to set the info to
 * @note entries with a value of 'PP_SGC_INVALID' will be ignored
 * @return s32 0 on success, error code otherwise
 */
s32 pp_si_sgc_info_set(u16 *sgc, u16 n_sgc, struct pp_si *si);

/**
 * @brief Set all session's si tbm info into an si structure
 * @param tbm tbm indexes array
 * @param n_tbm array size
 * @param si si to set the info to
 * @note entries with a value of 'PP_TBM_INVALID' will be ignored
 * @return s32 0 on success, error code otherwise
 */
s32 pp_si_tbm_info_set(u16 *tbm, u16 n_tbm, struct pp_si *si);

/**
 * @brief Get SI checker flag description
 * @param flag the flag
 * @return const char*const flag description, for invalid flag
 *         "invalid" string is returned
 */
const char * const pp_si_chck_flag_to_str(u32 flag);

/**
 * @brief converts si fields enum to str
 * @param fld enum value
 * @return enum string
 */
const char * const pp_si_fld2str(enum pp_si_fld fld);

/**
 * @brief converts si fields str to enum
 * @param name enum name
 * @param fld output contains the enum value, if exists
 * @return 0 if name exists in enum, error code otherwise
 */
s32 pp_si_str2fld(const char * const name, enum pp_si_fld *fld);

/**
 * @brief Initialize some stuff to make encoding and decoding SI
 *        easier
 */
void pp_si_init(void);
#else
static inline void pp_si_init(void)
{
}
#endif /* CONFIG_PPV4_LGM */
#endif /* __PP_SI_H__ */

