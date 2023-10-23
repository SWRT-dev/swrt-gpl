/*
 * Description: Packet Processor Modifier Internal Definitions
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2018-2019 Intel Corporation
 */

#ifndef __MODIFIER_INTERNAL_H__
#define __MODIFIER_INTERNAL_H__

#include <linux/types.h>
#include <linux/bitops.h>

#include "modifier_regs.h"

extern u64 mod_base_addr;
extern u64 mod_ram_base_addr;
#define MOD_BASE_ADDR mod_base_addr
#define MOD_RAM_BASE_ADDR mod_ram_base_addr

#define MOD_STORE_CMD_MAX     (8)
#define MOD_STORE_CMD_SZ      (8)
#define MOD_STORE_RAM_BASE    (MOD_RAM_BASE_ADDR + 0x000000)
#define MOD_ALU_CMD_MAX       (8)
#define MOD_ALU_CMD_SZ        (16)
#define MOD_ALU0_RAM_BASE     (MOD_RAM_BASE_ADDR + 0x010000)
#define MOD_ALU1_RAM_BASE     (MOD_RAM_BASE_ADDR + 0x020000)
#define MOD_REPS_CMD_MAX      (8)
#define MOD_REPS_CMD_SZ       (8)
#define MOD_REPS0_RAM_BASE    (MOD_RAM_BASE_ADDR + 0x040000)
#define MOD_REPS1_RAM_BASE    (MOD_RAM_BASE_ADDR + 0x070000)
#define MOD_INSREP_CMD_MAX    (8)
#define MOD_INSREP_CMD_SZ     (16)
#define MOD_INSREP_RAM_BASE   (MOD_RAM_BASE_ADDR + 0x050000)
#define MOD_PAD_CMD_MAX       (8)
#define MOD_PAD_CMD_SZ        (8)
#define MOD_PAD_RAM_BASE      (MOD_RAM_BASE_ADDR + 0x060000)
#define MOD_STRIP_CMD_MAX     (8)
#define MOD_STRIP_CMD_SZ      (8)
#define MOD_STRIP_RAM_BASE    (MOD_RAM_BASE_ADDR + 0x080000)
#define MOD_SCE_CMD_MAX       (10)
#define MOD_SCE_CMD_SZ        (4)
#define MOD_SCE_RAM_BASE      (MOD_RAM_BASE_ADDR + 0x100000)

#define MOD_DPU_NUM_REGS      (16)

#define MOD_PKT_OFF           (0x0 << 8)
#define MOD_DPU_RCP_OFF       (0x1 << 8)
#define MOD_SRAM_OFF          (0x2 << 8)
#define MOD_DPU_GLB_OFF       (0x3 << 8)
#define MOD_SI_PKT_OFF        (0xFF)

#define MOD_DPU_OFF_MASK      GENMASK(15, 8)
#define MOD_DPU_ID_MASK       GENMASK(7, 0)

/**
 * @brief Modifier EU command fields
 */
#define MOD_EU_STW_MUX0_FLD   GENMASK(3, 0)
#define MOD_EU_STW_MUX0_WORD  (0)
#define MOD_EU_STW_MUX1_FLD   GENMASK(7, 4)
#define MOD_EU_STW_MUX1_WORD  (0)
#define MOD_EU_SI_MUX0_FLD    GENMASK(11, 8)
#define MOD_EU_SI_MUX0_WORD   (0)
#define MOD_EU_SI_MUX1_FLD    GENMASK(15, 12)
#define MOD_EU_SI_MUX1_WORD   (0)
#define MOD_EU_SIZE_OFF_FLD   GENMASK(19, 16)
#define MOD_EU_SIZE_OFF_WORD  (0)
#define MOD_EU_JM_OP_LD0_FLD  GENMASK(21, 20)
#define MOD_EU_JM_OP_STR_FLD  GENMASK(23, 22)
#define MOD_EU_JM_OP_SZ_FLD   GENMASK(27, 24)
#define MOD_EU_JM_OP_LD1_FLD  BIT(28)
#define MOD_EU_JM_OP_OP0_FLD  BIT(29)
#define MOD_EU_JM_OPCODE_WORD (0)
#define MOD_EU_NOP_FLD        BIT(30)
#define MOD_EU_NOP_WORD       (0)
#define MOD_EU_IMM_FLD        BIT(31)
#define MOD_EU_IMM_WORD       (0)
#define MOD_EU_LAST_FLD       BIT(0)
#define MOD_EU_LAST_WORD      (1)
#define MOD_EU_STALL_FLD      BIT(1)
#define MOD_EU_STALL_WORD     (1)
#define MOD_EU_LOAD0_FLD      GENMASK(11, 2)
#define MOD_EU_LOAD0_WORD     (1)
#define MOD_EU_STORE_FLD      GENMASK(21, 12)
#define MOD_EU_STORE_WORD     (1)
#define MOD_EU_LOAD1_FLD      GENMASK(31, 22)
#define MOD_EU_LOAD1_WORD     (1)
#define MOD_EU_OPCODE0_FLD    GENMASK(3, 0)
#define MOD_EU_OPCODE0_WORD   (2)
#define MOD_EU_OPCODE1_FLD    GENMASK(9, 4)
#define MOD_EU_OPCODE1_WORD   (2)

/**
 * @brief Modifier SCE command fields
 */
#define MOD_SCE_ADDR_FLD      GENMASK(5, 0)
#define MOD_SCE_MUX_SEL_FLD   GENMASK(12, 6)
#define MOD_SCE_CNT_FLD       BIT(13)
#define MOD_SCE_LAST_FLD      BIT(14)
#define MOD_SCE_CMD_WORD      (0)

#define MOD_FOR_EACH_EU_CMD_WORD(i, sz) for ((i) = 0; (i) < ((sz) >> 2); i++)
#define MOD_FOR_EACH_EU_CMD(i, max_cmd) for (i = 0; i < max_cmd; i++)
#define MOD_FOR_EACH_RCP(r)             for (r = 0; r <= MOD_RCP_LAST; r++)

/**
 * @enum modifier execution units type
 * @EU_STORE store execution unit
 * @EU_PAD pad execution unit
 * @EU_STRIP strip execution unit
 * @EU_REPS0 reps0 execution unit
 * @EU_REPS1 reps1 execution unit
 * @EU_INSREP insrep execution unit
 * @EU_ALU0 alu0 execution unit
 * @EU_ALU1 alu1 execution unit
 * @EU_SCE sce execution unit
 * @EU_MAX number of execution units
 */
enum mod_eu_type {
	EU_STORE,
	EU_PAD,
	EU_STRIP,
	EU_REPS0,
	EU_REPS1,
	EU_INSREP,
	EU_ALU0,
	EU_ALU1,
	EU_SCE,
	EU_MAX,
};

#define MOD_FOR_EACH_EU(eu) for (eu = 0; eu < EU_MAX; eu++)

/**
 * @enum Global dpu registers address
 * @DPU_GLB_REGX_BY global dpu register X byte Y
 */
enum mod_dpu_glb_reg {
	DPU_GLB_REG_FIRST = MOD_DPU_GLB_OFF,
	DPU_GLB_REG0 = DPU_GLB_REG_FIRST,
	DPU_GLB_REG1,
	DPU_GLB_REG2,
	DPU_GLB_REG3,
	DPU_GLB_REG4,
	DPU_GLB_REG5,
	DPU_GLB_REG6,
	DPU_GLB_REG7,
	DPU_GLB_REG_LAST = DPU_GLB_REG7,
	DPU_GLB_REG_INVALID
};

#define MOD_GLB_EXT_IPV4_ID   DPU_GLB_REG0

/**
 * @enum recipe dpu registers address
 * @DPU_RCP_REGX_BY recipe dpu register X byte Y
 */
enum mod_dpu_rcp_reg {
	DPU_RCP_REG_FIRST = MOD_DPU_RCP_OFF,
	DPU_RCP_REG0_B0 = DPU_RCP_REG_FIRST,
	DPU_RCP_REG0_B1,
	DPU_RCP_REG0_B2,
	DPU_RCP_REG0_B3,
	DPU_RCP_REG1_B0,
	DPU_RCP_REG1_B1,
	DPU_RCP_REG1_B2,
	DPU_RCP_REG1_B3,
	DPU_RCP_REG2_B0,
	DPU_RCP_REG2_B1,
	DPU_RCP_REG2_B2,
	DPU_RCP_REG2_B3,
	DPU_RCP_REG3_B0,
	DPU_RCP_REG3_B1,
	DPU_RCP_REG3_B2,
	DPU_RCP_REG3_B3,
	DPU_RCP_REG4_B0,
	DPU_RCP_REG4_B1,
	DPU_RCP_REG4_B2,
	DPU_RCP_REG4_B3,
	DPU_RCP_REG5_B0,
	DPU_RCP_REG5_B1,
	DPU_RCP_REG5_B2,
	DPU_RCP_REG5_B3,
	DPU_RCP_REG6_B0,
	DPU_RCP_REG6_B1,
	DPU_RCP_REG6_B2,
	DPU_RCP_REG6_B3,
	DPU_RCP_REG7_B0,
	DPU_RCP_REG7_B1,
	DPU_RCP_REG7_B2,
	DPU_RCP_REG7_B3,
	DPU_RCP_REG8_B0,
	DPU_RCP_REG8_B1,
	DPU_RCP_REG8_B2,
	DPU_RCP_REG8_B3,
	DPU_RCP_REG9_B0,
	DPU_RCP_REG9_B1,
	DPU_RCP_REG9_B2,
	DPU_RCP_REG9_B3,
	DPU_RCP_REG10_B0,
	DPU_RCP_REG10_B1,
	DPU_RCP_REG10_B2,
	DPU_RCP_REG10_B3,
	DPU_RCP_REG11_B0,
	DPU_RCP_REG11_B1,
	DPU_RCP_REG11_B2,
	DPU_RCP_REG11_B3,
	/* NOTE: the below registers are by the modifier HW for
	 *       ALU operations with conditions as R2 and R3.
	 *       USE WITH CARE!
	 */
	DPU_RCP_REG12_B0_COND,
	DPU_RCP_REG12_B1_COND,
	DPU_RCP_REG12_B2_COND,
	DPU_RCP_REG12_B3_COND,
	DPU_RCP_REG13_B0_COND,
	DPU_RCP_REG13_B1_COND,
	DPU_RCP_REG13_B2_COND,
	DPU_RCP_REG13_B3_COND,
	DPU_RCP_REG14_B0_COND,
	DPU_RCP_REG14_B1_COND,
	DPU_RCP_REG14_B2_COND,
	DPU_RCP_REG14_B3_COND,
	DPU_RCP_REG15_B0_COND,
	DPU_RCP_REG15_B1_COND,
	DPU_RCP_REG15_B2_COND,
	DPU_RCP_REG15_B3_COND,
	DPU_RCP_REG_LAST = DPU_RCP_REG15_B3_COND,
	DPU_RCP_REG_INVALID
};

#define MOD_DPU_ID(d) (((d) & MOD_DPU_ID_MASK) >> 2)

/**
 * @enum bce start offset for insrep api
 */
enum mod_bce_offset {
	BCE_OFF_0,
	BCE_OFF_16,
	BCE_OFF_32,
	BCE_OFF_48,
	BCE_OFF_64,
	BCE_OFF_80,
	BCE_OFF_96,
	BCE_OFF_LAST = BCE_OFF_96,
};

/**
 * @struct execution unit
 * @max maximum commands per recipe
 * @addr ram base address
 * @cmd_sz command size
 */
struct mod_eu_info {
	u32 max_cmds;
	unsigned long base_addr;
	u32 cmd_sz;
};

/**
 * @struct module database
 * @next_recipe next avaliable recipe index
 * @eu execution units information
 */
struct mod_database {
	u32 next_recipe;
	struct mod_eu_info eu[EU_MAX];
};

/**
 * @struct modifier recipe
 * @index recipe index
 * @next_dpu next available dpu register
 * @eu_cmd_cnt number of commands per execution unit
 */
struct mod_recipe {
	u32 index;
	u32 next_dpu;
	u32 eu_cmd_cnt[EU_MAX];
};

/**
 * @enum execution unit status word mux select
 */
enum mod_eu_stw_mux_sel {
	EU_STW_SEL_ZERO    = 0x0,
	EU_STW_SEL_PS      = 0x1,
	EU_STW_SEL_COLOR,
	EU_STW_SEL_SRC_POOL,
	EU_STW_SEL_DSTQ,
	EU_STW_SEL_POLICY,
	EU_STW_SEL_PKT_LEN,
	EU_STW_SEL_L3_OFF5 = 0x8,
	EU_STW_SEL_L3_OFF4 = 0x9,
	EU_STW_SEL_L3_OFF3,
	EU_STW_SEL_L3_OFF2,
	EU_STW_SEL_L3_OFF1,
	EU_STW_SEL_L3_OFF0,
	EU_STW_SEL_PRSR_PKT_TYPE,
	EU_STW_SEL_PRE_L2_SIZE,
	EU_STW_SEL_LAST    = EU_STW_SEL_PRE_L2_SIZE,
};

#define STW_NEXT_LAYER(layer)   ((layer) - 1)
#define STW_PREV_LAYER(layer)   ((layer) + 1)

/**
 * @enum execution unit session information mux select
 */
enum mod_eu_si_mux_sel {
	EU_SI_SEL_ZERO = 0x0,
	EU_SI_SEL_PPPOE_OFF = 0x1,
	EU_SI_SEL_HDR_SZ,
	EU_SI_SEL_NAT_SZ,
	EU_SI_SEL_HDR_L3_OFF,
	EU_SI_SEL_LYR_FLD_OFF,
	EU_SI_SEL_LAST = EU_SI_SEL_LYR_FLD_OFF,
};

/**
 * @enum sce mux select
 */
enum mod_sce_mux_sel {
	SCE_STW_SEL_ZERO         = 0x0,
	SCE_STW_SEL_DATA_OFF     = 0x4,
	SCE_STW_SEL_COLOR        = 0x6,
	SCE_STW_SEL_SRC_POOL,
	SCE_STW_SEL_PKT_PTR,
	SCE_STW_SEL_PORT         = 0xC,
	SCE_STW_SEL_POLICY,
	SCE_STW_SEL_PKT_LEN,
	SCE_STW_SEL_TCP_INFO     = 0x10,
	SCE_STW_SEL_EXT_L3_OFF   = 0x12,
	SCE_STW_SEL_INT_L3_OFF,
	SCE_STW_SEL_EXT_L4_OFF,
	SCE_STW_SEL_INT_L4_OFF,
	SCE_STW_SEL_EXT_FRAG_INFO_OFF,
	SCE_STW_SEL_INT_FRAG_INFO_OFF,
	SCE_STW_SEL_L3_OFF5,
	SCE_STW_SEL_L3_OFF4,
	SCE_STW_SEL_L3_OFF3,
	SCE_STW_SEL_L3_OFF2,
	SCE_STW_SEL_L3_OFF1,
	SCE_STW_SEL_L3_OFF0,
	SCE_STW_SEL_PRSR_PKT_TYPE,
	SCE_STW_SEL_L2_OFF,
	SCE_STW_SEL_SEL_LAST       = SCE_STW_SEL_L2_OFF,
	SCE_SI_SEL_ZERO            = 0x20,
	SCE_SI_SEL_PPPOE_OFF       = 0x21,
	SCE_SI_SEL_HDR_SZ          = 0x22,
	SCE_SI_SEL_NAT_SZ          = 0x23,
	SCE_SI_SEL_HDR_L3_OFF      = 0x24,
	SCE_SI_SEL_LYR_FLD_OFF     = 0x25,
	SCE_SI_SEL_L4_CSUM_ZERO    = 0x2E,
	SCE_SI_SEL_L4_CSUM_DELTA   = 0x30,
	SCE_SI_SEL_L3_CSUM_DELTA   = 0x32,
	SCE_SI_SEL_TOT_LEN_DIFF    = 0x34,
	SCE_SI_SEL_DSCP            = 0x35,
	SCE_SI_SEL_HDR_TMPLT_CSUM  = 0x36,
	SCE_SI_SEL_NEW_SRC_PORT    = 0x38,
	SCE_SI_SEL_NEW_DST_PORT    = 0x3A,
	SCE_SI_SEL_TTL             = 0x3C,
	SCE_SI_SEL_PPPOE_DIFF      = 0x3D,
	SCE_SI_SEL_L2_ORG_VAL      = 0x3E,
	SCE_SI_SEL_LAST            = SCE_SI_SEL_L2_ORG_VAL,
};

/**
 * @enum mod_cmd_fmt_load0_op
 * @EU_LOAD0_RAW take the raw load0 value
 * @EU_LOAD0_SI0 take the raw load0 value + SI0
 * @EU_LOAD0_SI0_SW0 take the raw load0 value + SI0 + STW0
 * @EU_LOAD0_SI0_SW1_NO_SW0 take the raw load0 value + SI0 +
 *                          STW1 - STW0
 */
enum mod_cmd_fmt_load0_op {
	EU_LOAD0_RAW,
	EU_LOAD0_SI0,
	EU_LOAD0_SI0_SW0,
	EU_LOAD0_SI0_SW1_NO_SW0,
};

/**
 * @enum mod_cmd_fmt_store_op
 * @EU_STORE_RAW take the raw store value
 * @EU_STORE_SI1 take the raw store value + SI1
 * @EU_STORE_SI1_SW1 take the raw store value + SI1 + STW1
 * @EU_STORE_SI1_SW1_NO_SW0 take the raw store value + SI1 +
 *                          STW1 - STW0
 */
enum mod_cmd_fmt_store_op {
	EU_STORE_RAW,
	EU_STORE_SI1,
	EU_STORE_SI1_SW1,
	EU_STORE_SI1_SW1_NO_SW0,
};

/**
 * @enum mod_cmd_fmt_chnk_sz_op
 * @EU_CHNK_SZ_RAW        RAW
 * @EU_CHNK_SZ_SI0        RAW + SI0
 * @EU_CHNK_SZ_SI1        RAW + SI1
 * @EU_CHNK_SZ_SW1_NO_SW0 RAW + SW1 - SW0
 * @EU_CHNK_SZ_SW0_NO_SW1 RAW + SW0 - SW1
 * @EU_SW0_NO_SW1         SW0 - SW1
 */
enum mod_cmd_fmt_chnk_sz_op {
	EU_CHNK_SZ_RAW,
	EU_CHNK_SZ_SI0,
	EU_CHNK_SZ_SI1,
	EU_CHNK_SZ_SW1_NO_SW0,
	EU_CHNK_SZ_SW0_NO_SW1,
	EU_SW0_NO_SW1,
};

/**
 * @enum mod_cmd_fmt_load1_op
 * @EU_LOAD1_RAW RAW
 * @EU_LOAD1_SI0 RAW + SI0
 */
enum mod_cmd_fmt_load1_op {
	EU_LOAD1_RAW,
	EU_LOAD1_SI0,
};

/**
 * @enum mod_cmd_fmt_op0_op
 * @EU_OP0_RAW        RAW
 * @EU_OP0_SW0_NO_SW1 SW0 - SW1
 */
enum mod_cmd_fmt_op0_op {
	EU_OP0_RAW,
	EU_OP0_SW0_NO_SW1,
};

/**
 * @enum mod_cmd_fmt_alu_cond_op
 * @EU_ALU_NO_COND      no alu conditions
 * @EU_ALU_COND_MET     condition met
 * @EU_ALU_COND_NO_MET  condition no met
 * @EU_ALU_COND_OP_RSRV reserve
 */
enum mod_cmd_fmt_alu_cond_op {
	EU_ALU_NO_COND,
	EU_ALU_COND_MET,
	EU_ALU_COND_NO_MET,
	EU_ALU_COND_OP_RSRV,
};

/**
 * @enum mod_cmd_fmt_alu_cond_type
 */
enum mod_cmd_fmt_alu_cond_type {
	EU_ALU_L0_BIG_R2,
	EU_ALU_L0_BIG_R3,
	EU_ALU_L0_EQ_R2,
	EU_ALU_L0_EQ_R3,
	EU_ALU_L0_SML_R2,
	EU_ALU_L0_SML_R3,
	EU_ALU_RES_BIG_R2,
	EU_ALU_RES_BIG_R3,
	EU_ALU_RES_EQ_R2,
	EU_ALU_RES_EQ_R3,
	EU_ALU_RES_SML_R2,
	EU_ALU_RES_SML_R3,
	EU_ALU_COND_TYPE_RES1,
	EU_ALU_COND_TYPE_RES2,
	EU_ALU_COND_TYPE_RES3,
	EU_ALU_COND_TYPE_RES4,
};

/**
 * @enum mod_cmd_fmt_alu_op
 */
enum mod_cmd_fmt_alu_op {
	EU_ALU_INC,
	EU_ALU_DEC,
	EU_ALU_CS16_A,
	EU_ALU_CS16_B,
	EU_ALU_CS16_ADD,
	EU_ALU_ADD_2S,
	EU_ALU_DEC_2S_A,
	EU_ALU_DEC_2S_B,
	EU_ALU_SHFT_R,
	EU_ALU_SHFT_L,
	EU_ALU_BITW_OR,
	EU_ALU_BITW_AND,
	EU_ALU_BITW_XOR,
	EU_ALU_RAND,
	EU_ALU_OP_RSRV1,
	EU_ALU_OP_RSRV2,
};

#define MOD_PKT_OFF_L2_EXT EU_SI_SEL_ZERO
#define MOD_PKT_OFF_L3_EXT EU_SI_SEL_HDR_L3_OFF

/**
 * @define MOD_TO_HW_LOAD_SZ
 * @brief Convert number of bytes to modifier HW value
 */
#define MOD_TO_HW_LOAD_SZ(sz) ((sz) - 1)

/**
 * @enum execution unit hw command format
 */
struct mod_eu_hw {
	u32 word[4];
};

/**
 * @struct mod_eu_cmd execution unit command information
 */
struct mod_eu_cmd {
	enum mod_eu_type type;
	enum mod_cmd_fmt_load0_op jm_load0;
	enum mod_cmd_fmt_store_op jm_store;
	enum mod_cmd_fmt_chnk_sz_op jm_chnk_sz;
	enum mod_cmd_fmt_load1_op jm_load1;
	enum mod_cmd_fmt_op0_op jm_op;
	u16  load0;
	u8   load0_sz;
	u16  load1;
	u8   load1_sz;
	u16  store;
	u8   stw_mux0;
	u8   stw_mux1;
	u8   si_mux0;
	u8   si_mux1;
	u8   sz_off;
	u8   op0;
	u8   op1;
	bool nop;
	bool imm;
	bool last;
	bool stall;
};

/**
 * @struct mod_sce_cmd sce unit command information
 */
struct mod_sce_cmd {
	u8   addr;
	u8   mux_sel;
	bool cnt;
	bool last;
};

/**
 * @brief STW modifier fields enumaration
 * @STW_MOD_FLD_TTL_EXPIRED
 * @STW_MOD_FLD_IP_UNSUPP
 * @STW_MOD_FLD_EXT_DF
 * @STW_MOD_FLD_INT_DF
 * @STW_MOD_FLD_EXT_FRAG_TYPE
 * @STW_MOD_FLD_INT_FRAG_TYPE
 * @STW_MOD_FLD_TCP_FIN
 * @STW_MOD_FLD_TCP_SYN
 * @STW_MOD_FLD_TCP_RST
 * @STW_MOD_FLD_TCP_ACK
 * @STW_MOD_FLD_TCP_DATA_OFFSET
 * @STW_MOD_FLD_EXT_L3_OFFSET
 * @STW_MOD_FLD_INT_L3_OFFSET
 * @STW_MOD_FLD_EXT_L4_OFFSET
 * @STW_MOD_FLD_INT_L4_OFFSET
 * @STW_MOD_FLD_EXT_FRAG_INFO_OFFSET
 * @STW_MOD_FLD_INT_FRAG_INFO_OFFSET
 * @STW_MOD_FLD_L3_HDR_OFFSET_5
 * @STW_MOD_FLD_L3_HDR_OFFSET_4
 * @STW_MOD_FLD_L3_HDR_OFFSET_3
 * @STW_MOD_FLD_L3_HDR_OFFSET_2
 * @STW_MOD_FLD_L3_HDR_OFFSET_1
 * @STW_MOD_FLD_L3_HDR_OFFSET_0
 * @STW_MOD_FLD_UC_ADDED_BYTES
 * @STW_MOD_FLD_TUNNEL_INNER_OFFSET
 * @STW_MOD_FLD_PAYLOAD_OFFSET
 * @STW_MOD_FLD_ERROR_IND
 * @STW_MOD_FLD_RPB_CLID
 * @STW_MOD_FLD_DST_QUEUE
 * @STW_MOD_FLD_USER_DEFINE_LEN:
 *        16B granularity /number of UD words
 *        passed to modifier/uc
 *
 * @STW_MOD_FLD_HDR_SZ_DIFF:
 *        the diff between header size before
 *        modification and after modificatuion in twos
 *        compliment
 *
 * @STW_MOD_FLD_DROP_PKT:
 *        0x0 no drop
 *        0x1 dropped in HW parser to uC
 *        0x2 dropped in parser uC
 *        0x3 dropped in HW classifier to uC
 *        0x4 dropped in classifer uC
 *        0x5 dropped in PPRS
 *        0x6 dropped in checker
 *        0x7 dropped in parser
 * @STW_MOD_FLD_MOD_PBUFF:
 *        filed do no appear in UC interfaces
 *        indicate which buffer to write the RPB data
 */
enum pp_stw_modifier_fld {
	STW_MOD_FLD_FIRST = STW_COMMON_FLDS_NUM,
	STW_MOD_FLD_TTL_EXPIRED = STW_MOD_FLD_FIRST,
	STW_MOD_FLD_IP_UNSUPP,
	STW_MOD_FLD_EXT_DF,
	STW_MOD_FLD_INT_DF,
	STW_MOD_FLD_EXT_FRAG_TYPE,
	STW_MOD_FLD_INT_FRAG_TYPE,
	STW_MOD_FLD_TCP_FIN,
	STW_MOD_FLD_TCP_SYN,
	STW_MOD_FLD_TCP_RST,
	STW_MOD_FLD_TCP_ACK,
	STW_MOD_FLD_TCP_DATA_OFFSET,
	STW_MOD_FLD_EXT_L3_OFFSET,
	STW_MOD_FLD_INT_L3_OFFSET,
	STW_MOD_FLD_EXT_L4_OFFSET,
	STW_MOD_FLD_INT_L4_OFFSET,
	STW_MOD_FLD_EXT_FRAG_INFO_OFFSET,
	STW_MOD_FLD_INT_FRAG_INFO_OFFSET,
	STW_MOD_FLD_L3_HDR_OFFSET_5,
	STW_MOD_FLD_L3_HDR_OFFSET_4,
	STW_MOD_FLD_L3_HDR_OFFSET_3,
	STW_MOD_FLD_L3_HDR_OFFSET_2,
	STW_MOD_FLD_L3_HDR_OFFSET_1,
	STW_MOD_FLD_L3_HDR_OFFSET_0,
	STW_MOD_FLD_UC_ADDED_BYTES,
	STW_MOD_FLD_TUNN_INNER_OFFSET,
	STW_MOD_FLD_PAYLOAD_OFFSET,
	STW_MOD_FLD_ERROR_IND,
	STW_MOD_FLD_RPB_CLID,
	STW_MOD_FLD_DST_QUEUE,
	STW_MOD_FLD_USER_DEFINE_LEN,
	STW_MOD_FLD_HDR_SZ_DIFF,
	STW_MOD_FLD_DROP_PKT,
	STW_MOD_FLD_MOD_PBUFF,
	STW_MOD_FLD_LAST,
	STW_MOD_FLDS_NUM = STW_MOD_FLD_LAST
};

#define STW_MODIFIER_WORD_CNT                (6 + STW_COMMON_WORD_CNT)

/**
 * @brief STW modifier fields masks
 */
#define STW_MOD_FLD_TTL_EXPIRED_LSB          (0  + 4 * BITS_PER_U32)
#define STW_MOD_FLD_TTL_EXPIRED_MSB          (0  + 4 * BITS_PER_U32)
#define STW_MOD_FLD_IP_UNSUPP_LSB            (1  + 4 * BITS_PER_U32)
#define STW_MOD_FLD_IP_UNSUPP_MSB            (1  + 4 * BITS_PER_U32)
#define STW_MOD_FLD_EXT_DF_LSB               (2  + 4 * BITS_PER_U32)
#define STW_MOD_FLD_EXT_DF_MSB               (2  + 4 * BITS_PER_U32)
#define STW_MOD_FLD_INT_DF_LSB               (3  + 4 * BITS_PER_U32)
#define STW_MOD_FLD_INT_DF_MSB               (3  + 4 * BITS_PER_U32)
#define STW_MOD_FLD_EXT_FRAG_TYPE_LSB        (4  + 4 * BITS_PER_U32)
#define STW_MOD_FLD_EXT_FRAG_TYPE_MSB        (5  + 4 * BITS_PER_U32)
#define STW_MOD_FLD_INT_FRAG_TYPE_LSB        (6  + 4 * BITS_PER_U32)
#define STW_MOD_FLD_INT_FRAG_TYPE_MSB        (7  + 4 * BITS_PER_U32)
#define STW_MOD_FLD_TCP_FIN_LSB              (8  + 4 * BITS_PER_U32)
#define STW_MOD_FLD_TCP_FIN_MSB              (8  + 4 * BITS_PER_U32)
#define STW_MOD_FLD_TCP_SYN_LSB              (9  + 4 * BITS_PER_U32)
#define STW_MOD_FLD_TCP_SYN_MSB              (9  + 4 * BITS_PER_U32)
#define STW_MOD_FLD_TCP_RST_LSB              (10 + 4 * BITS_PER_U32)
#define STW_MOD_FLD_TCP_RST_MSB              (10 + 4 * BITS_PER_U32)
#define STW_MOD_FLD_TCP_ACK_LSB              (11 + 4 * BITS_PER_U32)
#define STW_MOD_FLD_TCP_ACK_MSB              (11 + 4 * BITS_PER_U32)
#define STW_MOD_FLD_TCP_DATA_OFFSET_LSB      (12 + 4 * BITS_PER_U32)
#define STW_MOD_FLD_TCP_DATA_OFFSET_MSB      (15 + 4 * BITS_PER_U32)
#define STW_MOD_FLD_EXT_L3_OFFSET_LSB        (16 + 4 * BITS_PER_U32)
#define STW_MOD_FLD_EXT_L3_OFFSET_MSB        (23 + 4 * BITS_PER_U32)
#define STW_MOD_FLD_INT_L3_OFFSET_LSB        (24 + 4 * BITS_PER_U32)
#define STW_MOD_FLD_INT_L3_OFFSET_MSB        (31 + 4 * BITS_PER_U32)
#define STW_MOD_FLD_EXT_L4_OFFSET_LSB        (0  + 5 * BITS_PER_U32)
#define STW_MOD_FLD_EXT_L4_OFFSET_MSB        (7  + 5 * BITS_PER_U32)
#define STW_MOD_FLD_INT_L4_OFFSET_LSB        (8  + 5 * BITS_PER_U32)
#define STW_MOD_FLD_INT_L4_OFFSET_MSB        (15 + 5 * BITS_PER_U32)
#define STW_MOD_FLD_EXT_FRAG_INFO_OFFSET_LSB (16 + 5 * BITS_PER_U32)
#define STW_MOD_FLD_EXT_FRAG_INFO_OFFSET_MSB (23 + 5 * BITS_PER_U32)
#define STW_MOD_FLD_INT_FRAG_INFO_OFFSET_LSB (24 + 5 * BITS_PER_U32)
#define STW_MOD_FLD_INT_FRAG_INFO_OFFSET_MSB (31 + 5 * BITS_PER_U32)
#define STW_MOD_FLD_L3_HDR_OFFSET_5_LSB      (0  + 6 * BITS_PER_U32)
#define STW_MOD_FLD_L3_HDR_OFFSET_5_MSB      (7  + 6 * BITS_PER_U32)
#define STW_MOD_FLD_L3_HDR_OFFSET_4_LSB      (8  + 6 * BITS_PER_U32)
#define STW_MOD_FLD_L3_HDR_OFFSET_4_MSB      (15 + 6 * BITS_PER_U32)
#define STW_MOD_FLD_L3_HDR_OFFSET_3_LSB      (16 + 6 * BITS_PER_U32)
#define STW_MOD_FLD_L3_HDR_OFFSET_3_MSB      (23 + 6 * BITS_PER_U32)
#define STW_MOD_FLD_L3_HDR_OFFSET_2_LSB      (24 + 6 * BITS_PER_U32)
#define STW_MOD_FLD_L3_HDR_OFFSET_2_MSB      (31 + 6 * BITS_PER_U32)
#define STW_MOD_FLD_L3_HDR_OFFSET_1_LSB      (0  + 7 * BITS_PER_U32)
#define STW_MOD_FLD_L3_HDR_OFFSET_1_MSB      (7  + 7 * BITS_PER_U32)
#define STW_MOD_FLD_L3_HDR_OFFSET_0_LSB      (8  + 7 * BITS_PER_U32)
#define STW_MOD_FLD_L3_HDR_OFFSET_0_MSB      (15 + 7 * BITS_PER_U32)
#define STW_MOD_FLD_UC_ADDED_BYTES_LSB       (16 + 7 * BITS_PER_U32)
#define STW_MOD_FLD_UC_ADDED_BYTES_MSB       (23 + 7 * BITS_PER_U32)
#define STW_MOD_FLD_TUNN_INNER_OFFSET_LSB    (24 + 7 * BITS_PER_U32)
#define STW_MOD_FLD_TUNN_INNER_OFFSET_MSB    (26 + 7 * BITS_PER_U32)
#define STW_MOD_FLD_PAYLOAD_OFFSET_LSB       (27 + 7 * BITS_PER_U32)
#define STW_MOD_FLD_PAYLOAD_OFFSET_MSB       (29 + 7 * BITS_PER_U32)
#define STW_MOD_FLD_ERROR_IND_LSB            (30 + 7 * BITS_PER_U32)
#define STW_MOD_FLD_ERROR_IND_MSB            (30 + 7 * BITS_PER_U32)
#define STW_MOD_FLD_RPB_CLID_LSB             (31 + 7 * BITS_PER_U32)
#define STW_MOD_FLD_RPB_CLID_MSB             (8  + 8 * BITS_PER_U32)
#define STW_MOD_FLD_DST_QUEUE_LSB            (9  + 8 * BITS_PER_U32)
#define STW_MOD_FLD_DST_QUEUE_MSB            (20 + 8 * BITS_PER_U32)
#define STW_MOD_FLD_USER_DEFINE_LEN_LSB      (21 + 8 * BITS_PER_U32)
#define STW_MOD_FLD_USER_DEFINE_LEN_MSB      (24 + 8 * BITS_PER_U32)
#define STW_MOD_FLD_HDR_SZ_DIFF_LSB          (25 + 8 * BITS_PER_U32)
#define STW_MOD_FLD_HDR_SZ_DIFF_MSB          (2  + 9 * BITS_PER_U32)
#define STW_MOD_FLD_DROP_PKT_LSB             (3  + 9 * BITS_PER_U32)
#define STW_MOD_FLD_DROP_PKT_MSB             (5  + 9 * BITS_PER_U32)
#define STW_MOD_FLD_MOD_PBUFF_LSB            (6  + 9 * BITS_PER_U32)
#define STW_MOD_FLD_MOD_PBUFF_MSB            (9  + 9 * BITS_PER_U32)

/**
 * @brief STW modifier fields description
 */
#define STW_MOD_FLD_TTL_EXPIRED_DESC            "TTL Expired"
#define STW_MOD_FLD_IP_UNSUPP_DESC              "IP Unsupported"
#define STW_MOD_FLD_EXT_DF_DESC                 "External Don't Frag"
#define STW_MOD_FLD_INT_DF_DESC                 "Internal Don't Frag"
#define STW_MOD_FLD_EXT_FRAG_TYPE_DESC          "External Frag Type"
#define STW_MOD_FLD_INT_FRAG_TYPE_DESC          "Internal Frag Type"
#define STW_MOD_FLD_TCP_FIN_DESC                "TCP Fin"
#define STW_MOD_FLD_TCP_SYN_DESC                "TCP Syn"
#define STW_MOD_FLD_TCP_RST_DESC                "TCP Reset"
#define STW_MOD_FLD_TCP_ACK_DESC                "TCP Ack"
#define STW_MOD_FLD_TCP_DATA_OFFSET_DESC        "TCP Data Offset"
#define STW_MOD_FLD_EXT_L3_OFFSET_DESC          "External L3 Offset"
#define STW_MOD_FLD_INT_L3_OFFSET_DESC          "Internal L3 Offset"
#define STW_MOD_FLD_EXT_L4_OFFSET_DESC          "External L4 Offset"
#define STW_MOD_FLD_INT_L4_OFFSET_DESC          "Internal L4 Offset"
#define STW_MOD_FLD_EXT_FRAG_INFO_OFFSET_DESC   "External Frag Info Offset"
#define STW_MOD_FLD_INT_FRAG_INFO_OFFSET_DESC   "Internal Frag Info Offset"
#define STW_MOD_FLD_L3_HDR_OFFSET_5_DESC        "L3 Header Offset 5"
#define STW_MOD_FLD_L3_HDR_OFFSET_4_DESC        "L3 Header Offset 4"
#define STW_MOD_FLD_L3_HDR_OFFSET_3_DESC        "L3 Header Offset 3"
#define STW_MOD_FLD_L3_HDR_OFFSET_2_DESC        "L3 Header Offset 2"
#define STW_MOD_FLD_L3_HDR_OFFSET_1_DESC        "L3 Header Offset 1"
#define STW_MOD_FLD_L3_HDR_OFFSET_0_DESC        "L3 Header Offset 0"
#define STW_MOD_FLD_UC_ADDED_BYTES_DESC         "UC Added Bytes"
#define STW_MOD_FLD_TUNN_INNER_OFFSET_DESC      "Tunnel Inner Offset"
#define STW_MOD_FLD_PAYLOAD_OFFSET_DESC         "Payload Offset"
#define STW_MOD_FLD_ERROR_IND_DESC              "Error"
#define STW_MOD_FLD_RPB_CLID_L_DESC             "RPB CLID Low"
#define STW_MOD_FLD_RPB_CLID_DESC               "RPB CLID"
#define STW_MOD_FLD_DST_QUEUE_DESC              "Destination Queue"
#define STW_MOD_FLD_USER_DEFINE_LEN_DESC        "UD Size"
#define STW_MOD_FLD_HDR_SZ_DIFF_DESC            "Header Size Diff"
#define STW_MOD_FLD_DROP_PKT_DESC               "Drop"
#define STW_MOD_FLD_MOD_PBUFF_DESC              "PBUFF"

static inline bool mod_is_rcp_id_valid(u32 idx)
{
	if (likely(idx < MOD_RCP_IDX_MAX))
		return true;

	pr_err("Invalid recipe index %u, valid values: 0..%u\n",
	       idx, MOD_RCP_IDX_MAX);
	return false;
}

static inline bool mod_is_eu_type_valid(u32 type)
{
	if (likely(type < EU_MAX))
		return true;

	pr_err("Invalid eu type %d, valid values: 0..%d\n", type, EU_MAX - 1);
	return false;
}

static inline bool mod_is_cmd_idx_valid(u32 idx, u32 max)
{
	if (likely(idx < max))
		return true;

	pr_err("Invalid cmd idx %u, valid values: 0..%u\n", idx, max - 1);
	return false;
}

static inline bool mod_is_bce_off_valid(u32 off)
{
	if (likely(off <= BCE_OFF_LAST))
		return true;

	pr_err("Invalid bce off %d, valid values: 0..%d\n", off, BCE_OFF_LAST);
	return false;
}

static inline bool mod_is_sce_mux_off_valid(u32 off)
{
	if (likely(off <= SCE_SI_SEL_LAST))
		return true;

	pr_err("Invalid sce mux off %d, valid values: 0..%d\n",
	       off, SCE_SI_SEL_LAST);
	return false;
}

static inline bool mod_is_op_size_valid(u32 sz)
{
	if (likely(sz <= 4))
		return true;

	pr_err("Invalid operation size %u, valid values: 0..4\n", sz);
	return false;
}

static inline bool mod_is_dpu_valid(u32 reg)
{
	if (reg >= DPU_RCP_REG_FIRST && reg <= DPU_RCP_REG_LAST)
		return true;

	if (reg >= DPU_GLB_REG_FIRST && reg <= DPU_GLB_REG_LAST)
		return true;

	pr_err("Invalid dpu reg %d, valid values: [%d..%d] [%d..%d]\n", reg,
	       DPU_RCP_REG_FIRST, DPU_RCP_REG_LAST,
	       DPU_GLB_REG_FIRST, DPU_GLB_REG_LAST);
	return false;
}

static inline bool mod_is_dpu_rcp_valid(u32 reg)
{
	if (reg >= DPU_RCP_REG_FIRST && reg <= DPU_RCP_REG_LAST)
		return true;

	pr_err("Invalid dpu rcp reg %d, valid values: [%d..%d]\n", reg,
	       DPU_RCP_REG_FIRST, DPU_RCP_REG_LAST);
	return false;
}

#define mod_is_layer_valid(off)         __mod_is_layer_valid(off, __func__)

static inline bool __mod_is_layer_valid(enum mod_eu_stw_mux_sel off,
					const char *caller)
{
	if (likely((off >= EU_STW_SEL_L3_OFF5 && off <= EU_STW_SEL_L3_OFF0)))
		return true;

	pr_err("Invalid layer off %d, valid values: %d..%d, caller %s\n", off,
	       EU_STW_SEL_L3_OFF5, EU_STW_SEL_L3_OFF0, caller);
	return false;
}

static inline bool mod_is_ext_layer_valid(u32 off)
{
	if (likely(off <= EU_SI_SEL_LAST))
		return true;

	pr_err("Invalid ext layer off %d, valid values: 0..%d\n",
	       off, EU_SI_SEL_LAST);
	return false;
}

static inline bool mod_is_alu_eu_type(enum mod_eu_type eu_type)
{
	if (likely(eu_type == EU_ALU0 || eu_type == EU_ALU1))
		return true;

	pr_err("Invalid eu type %u, valid values: %d or %d\n", eu_type,
	       EU_ALU0, EU_ALU1);
	return false;
}

/* modifier core APIs */
/**
 * @brief Add a new ALU command to the recipe.
 * @param alu ALU engine, EU_ALU0 or EU_ALU1
 * @param rcp the recipe
 * @param op1 operand 1 register, this will used as load 0
 * @param sz1 size of op1
 * @param op2 operand 2 register, this will used as load 1
 * @param sz2 size of op2
 * @param dst destination register
 * @param op_code the ALU operation to execute
 * @param cond the condition
 * @param cond_op when to do the condition
 * @return s32 0 on success, error code otherwise
 */
s32 mod_alu_cmd_cond(enum mod_eu_type alu, struct mod_recipe *rcp, s32 op1,
		     s32 sz1, s32 op2, s32 sz2, s32 dst,
		     enum mod_cmd_fmt_alu_op op_code,
		     enum mod_cmd_fmt_alu_cond_type cond,
		     enum mod_cmd_fmt_alu_cond_op cond_op);

/**
 * @brief Add new ALU command w/o condition
 */
static inline s32 mod_alu_cmd(enum mod_eu_type alu, struct mod_recipe *rcp,
			      s32 op1, s32 sz1, s32 op2, s32 sz2, s32 dst,
			      enum mod_cmd_fmt_alu_op op_code)
{
	return mod_alu_cmd_cond(alu, rcp, op1, sz1, op2, sz2, dst, op_code,
				0, EU_ALU_NO_COND);
}

/**
 * @brief copy nat ipv4 from si-bce to packet, always use dst and src ips
 * @param rcp current recipe
 * @param bce_off bce offset in 16 bytes granularity
 * @param dst_layer destination layer in packet
 * @return s32 0 on success, error code otherwise
 */
s32 mod_bce_ip_nat_cpy(struct mod_recipe *rcp, s32 bce_off, s32 dst_layer);

/**
 * @brief copy field from dpu registers to packet header
 * @param rcp current recipe
 * @param src source dpu register
 * @param layer packet layer
 * @param off offset inside the layer
 * @param sz number of bytes to copy (up to 4 bytes)
 * @return s32 0 on success, error code otherwise
 */
s32 mod_reg_to_hdr_cpy(struct mod_recipe *rcp, s32 src, s32 layer, u8 off,
		       u8 sz);

/**
 * @brief copy field from dpu registers to packet new header
 * @param rcp current recipe
 * @param src source dpu register
 * @param layer new header packet layer
 * @param off offset inside the layer
 * @param sz number of bytes to copy (up to 4 bytes)
 * @return s32 0 on success, error code otherwise
 */
s32 mod_reg_to_new_hdr_cpy(struct mod_recipe *rcp, s32 src, s32 layer, u32 off,
			   u32 sz);

/**
 * @brief calculate 16bit checksum, works only on dpu registers
 *        the operation is done according to the specified condition
 * @param alu ALU engine, EU_ALU0 or EU_ALU1
 * @param rcp current recipe
 * @param new_val new value register
 * @param csum original checksum register
 * @param dst destination register
 * @param cond the condition
 * @param cond_op when to do the condition
 * @return s32 0 on success, error code otherwise
 */
static inline s32 mod_alu_csum16_add_cond(enum mod_eu_type alu,
					  struct mod_recipe *rcp, s32 new_val,
					  s32 csum, s32 dst,
					  enum mod_cmd_fmt_alu_cond_type cond,
					  enum mod_cmd_fmt_alu_cond_op cond_op)
{
	return mod_alu_cmd_cond(alu, rcp, csum, sizeof(u16), new_val,
				sizeof(u16), dst, EU_ALU_CS16_ADD, cond,
				cond_op);
}

/**
 * @brief calculate 16bit checksum, works only on dpu registers
 * @param alu ALU engine, EU_ALU0 or EU_ALU1
 * @param rcp current recipe
 * @param delta checksum delta register
 * @param csum original checksum register
 * @param dst destination register
 * @return s32 0 on success, error code otherwise
 */
static inline s32 mod_alu_csum16_add(enum mod_eu_type alu,
				     struct mod_recipe *rcp, s32 delta,
				     s32 csum, s32 dst)
{
	return mod_alu_csum16_add_cond(alu, rcp, delta, csum, dst, 0,
				       EU_ALU_NO_COND);
}

/**
 * @brief performs the 16bit checksum operation A, works only on dpu registers
 * This function can be used to recalculate the checksum if one of the fields
 * changes, the new checksum is calculated by solving the following operation
 * for the special case where old_val is zero:
 * new_csum = old_csum + old_val - new_val
 * the operation is done according to the specified condition
 * @param alu ALU engine, EU_ALU0 or EU_ALU1
 * @param rcp current recipe
 * @param new_val new value register
 * @param csum original checksum register
 * @param dst destination register
 * @param cond the condition
 * @param cond_op when to do the condition
 * @return s32 0 on success, error code otherwise
 */
static inline s32 mod_alu_csum16_sub_cond(enum mod_eu_type alu,
					  struct mod_recipe *rcp, s32 new_val,
					  s32 csum, s32 dst,
					  enum mod_cmd_fmt_alu_cond_type cond,
					  enum mod_cmd_fmt_alu_cond_op cond_op)
{
	return mod_alu_cmd_cond(alu, rcp, csum, sizeof(u16), new_val,
				sizeof(u16), dst, EU_ALU_CS16_A, cond,
				cond_op);
}

/**
 * @brief performs the 16bit checksum operation A, works only on dpu registers
 * This function can be used to recalculate the checksum if one of the fields
 * changes, the new checksum is calculated by solving the following operation
 * for the special case where old_val is zero:
 * new_csum = old_csum + old_val - new_val
 * @param alu ALU engine, EU_ALU0 or EU_ALU1
 * @param rcp current recipe
 * @param new_val new value register
 * @param csum original checksum register
 * @param dst destination register
 * @return s32 0 on success, error code otherwise
 */
static inline s32 mod_alu_csum16_sub(enum mod_eu_type alu,
				     struct mod_recipe *rcp, s32 new_val,
				     s32 csum, s32 dst)
{
	return mod_alu_csum16_sub_cond(alu, rcp, new_val, csum, dst, 0,
				       EU_ALU_NO_COND);
}

/**
 * @brief add 2 register fields, works only on dpu registers
 * @param alu ALU engine, EU_ALU0 or EU_ALU1
 * @param rcp current recipe
 * @param src source 1 register
 * @param s_sz source register size in bytes (up to 4)
 * @param val register holding the value to add
 * @param v_sz value register size in bytes (up to 4)
 * @param dst destination register
 * @return s32 0 on success, error code otherwise
 */
static inline s32 mod_alu_add(enum mod_eu_type alu, struct mod_recipe *rcp,
			      s32 src, s32 s_sz, s32 val, s32 v_sz, s32 dst)
{
	return mod_alu_cmd(alu, rcp, src, s_sz, val, v_sz, dst, EU_ALU_ADD_2S);
}

static inline s32 mod_alu_add_cond(enum mod_eu_type alu, struct mod_recipe *rcp,
				   s32 src, s32 s_sz, s32 val, s32 v_sz,
				   s32 dst, enum mod_cmd_fmt_alu_cond_type cond,
				   enum mod_cmd_fmt_alu_cond_op cond_op)
{
	return mod_alu_cmd_cond(alu, rcp, src, s_sz, val, v_sz, dst,
				EU_ALU_ADD_2S, cond, cond_op);
}

/**
 * @brief subtract register field by a value, works only on dpu
 *        register
 * @param alu ALU engine, EU_ALU0 or EU_ALU1
 * @param rcp current recipe
 * @param src source field register
 * @param val register holding the value to subtract
 * @param dst destination register to save the result
 * @param sz field size register (max size is 4 bytes)
 * @return s32 0 on success, error code otherwise
 */
static inline
s32 mod_alu_sub(enum mod_eu_type alu, struct mod_recipe *rcp, s32 src, s32 val,
		s32 dst, u32 sz)
{
	return mod_alu_cmd(alu, rcp, src, sz, val, sz, dst,
			   EU_ALU_DEC_2S_A);
}

/**
 * @brief decrement register field, works only on dpu register
 * @param alu ALU engine, EU_ALU0 or EU_ALU1
 * @param rcp current recipe
 * @param src source field register
 * @param dst destination register
 * @param sz field size register (max size is 4 bytes)
 * @return s32 0 on success, error code otherwise
 */
static inline
s32 mod_alu_dec(enum mod_eu_type alu, struct mod_recipe *rcp, s32 src,
		s32 dst, u32 sz)
{
	return mod_alu_cmd(alu, rcp, src, sz, DPU_RCP_REG_INVALID, sz,
			   dst, EU_ALU_DEC);
}

/**
 * @brief increment global register field, works only on dpu register
 * @param alu ALU engine, EU_ALU0 or EU_ALU1
 * @param rcp current recipe
 * @param src source field register
 * @param dst destination register
 * @return s32 0 on success, error code otherwise
 */
static inline
s32 mod_alu_inc(enum mod_eu_type alu, struct mod_recipe *rcp, s32 src, s32 dst)
{
	return mod_alu_cmd(alu, rcp, src, 4, DPU_RCP_REG_INVALID, 0,
			   dst, EU_ALU_INC);
}

/**
 * @brief synchronize the alu to a field inside a specific layer in
 *        the packet
 * @param alu ALU engine, EU_ALU0 or EU_ALU1
 * @param rcp current recipe
 * @param layer the layer
 * @param off the field offset within the layer
 * @param sz the field size in bytes
 * @return s32 0 on success, error code otherwise
 */
s32 mod_alu_synch(enum mod_eu_type alu, struct mod_recipe *rcp, s32 layer,
		  u8 off, u8 sz);

/**
 * @brief copy field from packet to dpu register (no register
 *        overlapping)
 * @param rcp current recipe
 * @param dst destination register
 * @param layer packet layer
 * @param off offset inside the layer, specify 0xff to take offset
 *            from si
 * @param sz field size (up to 4 bytes)
 * @return s32 0 on success, error code otherwise
 */
s32 mod_pkt_to_reg_cpy(struct mod_recipe *rcp, s32 dst, s32 layer, u8 off,
		       u8 sz);

/**
 * @brief copy 4 bytes from sce mux to dpu register
 * @param rcp current recipe
 * @param dst destination dpu register id
 * @param off sce mux offset
 * @return s32 0 on success, error code otherwise
 */
s32 mod_sce_mux_to_reg_cpy(struct mod_recipe *rcp, s32 dst, s32 off);

/**
 * @brief copy header from si-bce to the packet head
 * @param rcp current recipe
 * @param bce_off si-bce offset of new header
 * @param end_trim_layer until which layer to trim
 * @return s32 0 on success, error code otherwise
 */
s32 mod_hdr_rplc(struct mod_recipe *rcp, u32 bce_off, s32 end_trim_layer);

/**
 * @brief complete the recipe, set 'last' bit in last command of
 *        each used eu and set 'last'+'nop' as a new command for
 *        each unused eu
 * @param rcp current recipe
 * @return s32 0 on success, error code otherwise
 */
s32 mod_rcp_complete(struct mod_recipe *rcp);

/**
 * @brief get the maximum number of command for execution unit
 * @param eu execution unit id
 * @param max_cmds maximum commands
 * @return s32 0 on success, error code otherwise
 */
s32 mod_eu_cmd_max_get(s32 eu, u32 *max_cmds);

/**
 * @brief read eu command from recipe
 * @param rcp_idx recipe index
 * @param cmd command from recipe
 * @param type eu type
 * @param cmd_idx command index
 * @return s32 0 on success, error code otherwise
 */
s32 mod_eu_cmd_rd(u32 rcp_idx, struct mod_eu_hw *cmd, s32 type, u32 cmd_idx);

/**
 * @brief write eu command to recipe
 * @param rcp_idx recipe index
 * @param cmd command from recipe
 * @param type eu type
 * @param cmd_idx command index
 * @return s32 0 on success, error code otherwise
 */
s32 mod_eu_cmd_wr(u32 rcp_idx, struct mod_eu_hw *cmd, s32 type, u32 cmd_idx);

/**
 * @brief Modify specific command's word of a recipe
 * @param rcp_idx recipe index
 * @param cmd_idx command index
 * @param type eu engine
 * @param word word index to modify
 * @param val new value to set
 * @return s32 0 on success, error code otherwise
 */
s32 mod_eu_cmd_word_set(u32 rcp_idx, u32 cmd_idx, u32 type, u32 word, u32 val);

/**
 * @brief init all recipes
 * @return s32 0 on success, error code otherwise
 */
s32 mod_rcp_init(void);

#ifdef CONFIG_DEBUG_FS
/**
 * @brief Modifier debug init
 * @return s32 0 on success, error code otherwise
 */
s32 mod_dbg_init(struct dentry *parent);

/**
 * @brief Modifier debug cleanup
 */
void mod_dbg_clean(void);
#else /* !CONFIG_DEBUG_FS */
static inline s32 mod_dbg_init(struct dentry *parent)
{
	return 0;
}

static inline void mod_dbg_clean(void)
{
}
#endif /* CONFIG_DEBUG_FS */
#endif /* __MODIFIER_INTERNAL_H__ */
