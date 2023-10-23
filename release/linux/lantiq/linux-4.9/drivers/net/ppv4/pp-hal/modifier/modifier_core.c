/*
 * Description: PP Modifier Core Module
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2018-2019 Intel Corporation
 */

#define pr_fmt(fmt) "[PP_MODIFIER_CORE]: %s:%d: " fmt, __func__, __LINE__

#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/init.h>
#include <linux/errno.h>

#include "pp_regs.h"
#include "pp_common.h"
#include "modifier.h"
#include "modifier_regs.h"
#include "modifier_internal.h"

/**
 * @brief modifier HW base address
 */
u64 mod_base_addr;
u64 mod_ram_base_addr;

/**
 * @brief module database
 */
static struct mod_database *db;

/**
 * @brief get the execution unit current command address
 * @param rcp_idx current recipe index
 * @param type execution unit type
 * @return u32 command address
 */
static s32 __mod_eu_cmd_addr_get(u32 rcp_idx, s32 type, u32 cmd_idx,
				 unsigned long *addr)
{
	if (unlikely(ptr_is_null(addr)))
		return -EINVAL;

	if (unlikely(!mod_is_rcp_id_valid(rcp_idx)))
		return -EINVAL;

	if (unlikely(!mod_is_eu_type_valid(type)))
		return -EINVAL;

	if (unlikely(!mod_is_cmd_idx_valid(cmd_idx, db->eu[type].max_cmds)))
		return -EINVAL;

	*addr = db->eu[type].base_addr +
		(rcp_idx * db->eu[type].max_cmds * db->eu[type].cmd_sz) +
		(cmd_idx * db->eu[type].cmd_sz);

	pr_debug("eu type %d, cmd addr %#lx\n", type, *addr);

	return 0;
}

s32 mod_eu_cmd_wr(u32 rcp_idx, struct mod_eu_hw *cmd, s32 type, u32 cmd_idx)
{
	unsigned long cmd_addr;
	u32 i;

	if (unlikely(ptr_is_null(cmd)))
		return -EINVAL;

	if (unlikely(!mod_is_rcp_id_valid(rcp_idx)))
		return -EINVAL;

	if (unlikely(!mod_is_eu_type_valid(type)))
		return -EINVAL;

	if (unlikely(!mod_is_cmd_idx_valid(cmd_idx, db->eu[type].max_cmds)))
		return -EINVAL;

	pr_debug("eu type %d\n", type);

	if (unlikely(__mod_eu_cmd_addr_get(rcp_idx, type, cmd_idx, &cmd_addr)))
		return -EINVAL;

	MOD_FOR_EACH_EU_CMD_WORD(i, db->eu[type].cmd_sz) {
		pr_debug("cmd.addr %#lx: cmd.data[%u] = %#x\n",
			 cmd_addr, i, cmd->word[i]);
		PP_REG_WR32(cmd_addr, cmd->word[i]);
		cmd_addr += 4;
	}

	return 0;
}

s32 mod_eu_cmd_rd(u32 rcp_idx, struct mod_eu_hw *cmd, s32 type, u32 cmd_idx)
{
	unsigned long cmd_addr;
	u32 i;

	if (unlikely(ptr_is_null(cmd)))
		return -EINVAL;

	if (unlikely(!mod_is_rcp_id_valid(rcp_idx)))
		return -EINVAL;

	if (unlikely(!mod_is_eu_type_valid(type)))
		return -EINVAL;

	if (unlikely(!mod_is_cmd_idx_valid(cmd_idx, db->eu[type].max_cmds)))
		return -EINVAL;

	pr_debug("eu type %d\n", type);

	memset(cmd, 0, sizeof(*cmd));

	if (unlikely(__mod_eu_cmd_addr_get(rcp_idx, type, cmd_idx, &cmd_addr)))
		return -EINVAL;

	MOD_FOR_EACH_EU_CMD_WORD(i, db->eu[type].cmd_sz) {
		cmd->word[i] = PP_REG_RD32(cmd_addr);
		pr_debug("cmd.addr %#lx: cmd.data[%u] = %#x\n",
			 cmd_addr, i, cmd->word[i]);
		cmd_addr += 4;
	}

	return 0;
}

s32 mod_eu_cmd_word_set(u32 rcp_idx, u32 cmd_idx, u32 type, u32 word, u32 val)
{
	struct mod_eu_hw cmd_buf = { 0 };
	s32 ret;

	pr_debug("rcp %u, cmd %u, eu %u, word %u, val %u\n",
		 rcp_idx, cmd_idx, type, word, val);
	ret = mod_eu_cmd_rd(rcp_idx, &cmd_buf, type, cmd_idx);
	if (unlikely(ret))
		return ret;

	if (word >= db->eu[type].cmd_sz) {
		pr_err("invalid word %u index for eu %u command\n", word, type);
		return -EINVAL;
	}

	cmd_buf.word[word] = val;

	ret = mod_eu_cmd_wr(rcp_idx, &cmd_buf, type, cmd_idx);
	if (unlikely(ret))
		return ret;

	return 0;
}

/**
 * @brief set execution unit command, convert the command to hw
 *        format
 * @param rcp current recipe
 * @param eu execution unit command
 * @return s32 0 on success, error code otherwise
 */
static s32 __mod_rcp_eu_cmd_set(struct mod_recipe *rcp, struct mod_eu_cmd *eu)
{
	struct mod_eu_hw cmd;

	if (unlikely(!(!ptr_is_null(rcp) &&
		       !ptr_is_null(eu))))
		return -EINVAL;

	memset(&cmd, 0, sizeof(cmd));
	pr_debug("set mod eu %d command\n", eu->type);
	pr_debug("stw_mux0 = %hhu\n", eu->stw_mux0);
	pr_debug("stw_mux1 = %hhu\n", eu->stw_mux1);
	pr_debug("si_mux0 = %hhu\n",  eu->si_mux0);
	pr_debug("si_mux1 = %hhu\n",  eu->si_mux1);
	pr_debug("jm_load0 = %d\n",   eu->jm_load0);
	pr_debug("jm_store = %d\n",   eu->jm_store);
	pr_debug("jm_chnk_sz = %d\n", eu->jm_chnk_sz);
	pr_debug("jm_load1 = %d\n",   eu->jm_load1);
	pr_debug("jm_op = %d\n",      eu->jm_op);
	pr_debug("sz_off = %hhu\n",   eu->sz_off);
	pr_debug("nop = %d\n",        eu->nop);
	pr_debug("imm = %d\n",        eu->imm);
	pr_debug("last = %d\n",       eu->last);
	pr_debug("stall = %d\n",      eu->stall);
	pr_debug("load0 = %hu\n",     eu->load0);
	pr_debug("store = %hu\n",     eu->store);
	pr_debug("load1 = %hu\n",     eu->load1);
	pr_debug("op0 = %hhu\n",      eu->op0);
	pr_debug("op1 = %hhu\n",      eu->op1);
	pr_debug("\n");

	cmd.word[MOD_EU_STW_MUX0_WORD]  |= PP_FIELD_PREP(MOD_EU_STW_MUX0_FLD,
							eu->stw_mux0);
	cmd.word[MOD_EU_STW_MUX1_WORD]  |= PP_FIELD_PREP(MOD_EU_STW_MUX1_FLD,
							eu->stw_mux1);
	cmd.word[MOD_EU_SI_MUX0_WORD]   |= PP_FIELD_PREP(MOD_EU_SI_MUX0_FLD,
							eu->si_mux0);
	cmd.word[MOD_EU_SI_MUX1_WORD]   |= PP_FIELD_PREP(MOD_EU_SI_MUX1_FLD,
							eu->si_mux1);
	cmd.word[MOD_EU_JM_OPCODE_WORD] |= PP_FIELD_PREP(MOD_EU_JM_OP_LD0_FLD,
							eu->jm_load0);
	cmd.word[MOD_EU_JM_OPCODE_WORD] |= PP_FIELD_PREP(MOD_EU_JM_OP_STR_FLD,
							eu->jm_store);
	cmd.word[MOD_EU_JM_OPCODE_WORD] |= PP_FIELD_PREP(MOD_EU_JM_OP_SZ_FLD,
							eu->jm_chnk_sz);
	cmd.word[MOD_EU_JM_OPCODE_WORD] |= PP_FIELD_PREP(MOD_EU_JM_OP_LD1_FLD,
							eu->jm_load1);
	cmd.word[MOD_EU_JM_OPCODE_WORD] |= PP_FIELD_PREP(MOD_EU_JM_OP_OP0_FLD,
							eu->jm_op);
	cmd.word[MOD_EU_SIZE_OFF_WORD]  |= PP_FIELD_PREP(MOD_EU_SIZE_OFF_FLD,
							eu->sz_off);
	cmd.word[MOD_EU_NOP_WORD]       |= PP_FIELD_PREP(MOD_EU_NOP_FLD,
							eu->nop);
	cmd.word[MOD_EU_IMM_WORD]       |= PP_FIELD_PREP(MOD_EU_IMM_FLD,
							eu->imm);
	cmd.word[MOD_EU_LAST_WORD]      |= PP_FIELD_PREP(MOD_EU_LAST_FLD,
							eu->last);
	cmd.word[MOD_EU_STALL_WORD]     |= PP_FIELD_PREP(MOD_EU_STALL_FLD,
							eu->stall);
	cmd.word[MOD_EU_LOAD0_WORD]     |= PP_FIELD_PREP(MOD_EU_LOAD0_FLD,
							 eu->load0);
	cmd.word[MOD_EU_STORE_WORD]     |= PP_FIELD_PREP(MOD_EU_STORE_FLD,
							eu->store);
	cmd.word[MOD_EU_LOAD1_WORD]     |= PP_FIELD_PREP(MOD_EU_LOAD1_FLD,
							eu->load1);
	cmd.word[MOD_EU_OPCODE0_WORD]   |= PP_FIELD_PREP(MOD_EU_OPCODE0_FLD,
							eu->op0);
	cmd.word[MOD_EU_OPCODE1_WORD]   |= PP_FIELD_PREP(MOD_EU_OPCODE1_FLD,
							eu->op1);

	return mod_eu_cmd_wr(rcp->index, &cmd, eu->type,
			     rcp->eu_cmd_cnt[eu->type] - 1);
}

/**
 * @brief set sce command, convert the command to hw format
 * @param rcp current recipe
 * @param sce sce command
 * @return s32 0 on success, error code otherwise
 */
static s32 __mod_rcp_sce_cmd_set(struct mod_recipe *rcp,
				 struct mod_sce_cmd *sce)
{
	struct mod_eu_hw cmd;

	if (unlikely(!(!ptr_is_null(rcp) &&
		       !ptr_is_null(sce))))
		return -EINVAL;

	pr_debug("eu type %d\n", EU_SCE);
	memset(&cmd, 0, sizeof(cmd));
	cmd.word[0] |= PP_FIELD_PREP(MOD_SCE_ADDR_FLD,    sce->addr);
	cmd.word[0] |= PP_FIELD_PREP(MOD_SCE_MUX_SEL_FLD, sce->mux_sel);
	cmd.word[0] |= PP_FIELD_PREP(MOD_SCE_CNT_FLD,     sce->cnt);
	cmd.word[0] |= PP_FIELD_PREP(MOD_SCE_LAST_FLD,    sce->last);

	return mod_eu_cmd_wr(rcp->index, &cmd, EU_SCE,
			     rcp->eu_cmd_cnt[EU_SCE] - 1);
}

/**
 * @brief allocate execution unit command
 * @param rcp current recipe
 * @param type execution unit type
 * @return s32 0 on success, error code otherwise
 */
static s32 __mod_eu_cmd_alloc(struct mod_recipe *rcp, s32 type)
{
	if (unlikely(ptr_is_null(rcp)))
		return -EINVAL;

	if (unlikely(!mod_is_eu_type_valid(type)))
		return -EINVAL;

	pr_debug("eu type %d\n", type);

	if (rcp->eu_cmd_cnt[type] >= db->eu[type].max_cmds) {
		pr_err("error: eu type %d command overflow\n", type);
		return -ENOSPC;
	}
	rcp->eu_cmd_cnt[type]++;

	return 0;
}

/**
 * @brief modify the last command by setting the 'last' bit
 * @param rcp current recipe
 * @param type execution unit type
 * @return s32 0 on success, error code otherwise
 */
static s32 __mod_eu_last_cmd_mod(struct mod_recipe *rcp, s32 type)
{
	s32 ret;
	struct mod_eu_hw cmd;

	if (unlikely(ptr_is_null(rcp)))
		return -EINVAL;

	if (unlikely(!mod_is_eu_type_valid(type)))
		return -EINVAL;

	pr_debug("eu type %d\n", type);

	/* read last command */
	ret = mod_eu_cmd_rd(rcp->index, &cmd, type, rcp->eu_cmd_cnt[type] - 1);

	if (unlikely(ret))
		goto done;

	/* set 'last' bit */
	if (type == EU_SCE) {
		cmd.word[MOD_SCE_CMD_WORD] =
			PP_FIELD_MOD(MOD_SCE_LAST_FLD, 1,
				     cmd.word[MOD_SCE_CMD_WORD]);
	} else {
		cmd.word[MOD_EU_LAST_WORD] =
			PP_FIELD_MOD(MOD_EU_LAST_FLD, 1,
				     cmd.word[MOD_EU_LAST_WORD]);
	}

	/* write back command */
	return mod_eu_cmd_wr(rcp->index, &cmd, type, rcp->eu_cmd_cnt[type] - 1);

done:
	return ret;
}

/**
 * @brief adding 'nop' command
 * @param rcp current recipe
 * @param type execution unit type
 * @return s32 0 on success, error code otherwise
 */
static s32 __mod_eu_nop_cmd_add(struct mod_recipe *rcp, s32 type)
{
	struct mod_eu_cmd  eu;
	struct mod_sce_cmd sce;
	s32 ret = 0;

	if (unlikely(ptr_is_null(rcp)))
		return -EINVAL;

	if (unlikely(!mod_is_eu_type_valid(type)))
		return -EINVAL;

	pr_debug("eu type %d\n", type);

	ret = __mod_eu_cmd_alloc(rcp, type);
	if (unlikely(ret))
		goto done;

	if (type == EU_SCE) {
		memset(&sce, 0, sizeof(sce));
		sce.addr = MOD_DPU_ID_MASK & DPU_RCP_REG15_B0_COND;
		sce.mux_sel = SCE_STW_SEL_ZERO;
		sce.last = true;
		ret = __mod_rcp_sce_cmd_set(rcp, &sce);
	} else {
		memset(&eu, 0, sizeof(eu));
		eu.type = type;
		eu.last = true;
		eu.nop = true;
		ret = __mod_rcp_eu_cmd_set(rcp, &eu);
	}

done:
	return ret;
}

s32 mod_bce_ip_nat_cpy(struct mod_recipe *rcp, s32 bce_off, s32 dst_layer)
{
	struct mod_eu_cmd eu;
	s32 ret;

	if (unlikely(ptr_is_null(rcp)))
		return -EINVAL;

	if (unlikely(!mod_is_layer_valid(dst_layer)))
		return -EINVAL;

	if (unlikely(!mod_is_bce_off_valid(bce_off)))
		return -EINVAL;

	memset(&eu, 0, sizeof(eu));
	eu.type = EU_INSREP;

	ret = __mod_eu_cmd_alloc(rcp, eu.type);
	if (unlikely(ret))
		return ret;

	/* SI0 = SI[si_mux0] (nat template size) */
	eu.si_mux0 = EU_SI_SEL_NAT_SZ;
	/* SI1 = SI[si_mux1] (0) */
	eu.si_mux1 = 0;
	/* STW0 = L2 off (0) */
	eu.stw_mux0 = EU_STW_SEL_ZERO;
	/* STW1 = SW[stw_mux1] */
	eu.stw_mux1 = dst_layer;

	/* read from sram at offset bce_off x16 */
	eu.load0 = MOD_SRAM_OFF | bce_off;
	/* use load0 val as is */
	eu.jm_load0 = EU_LOAD0_RAW;

	eu.load1 = 0;
	/* load1 = load1(0) + SI0 (nat template size) */
	eu.jm_load1 = EU_LOAD1_SI0;

	/* ips offset in ipv4 header */
	eu.store = 12;
	/* store = store + SW1(L3 off) + SI1(0) - SW0(L2 off) */
	eu.jm_store = EU_STORE_SI1_SW1_NO_SW0;

	eu.sz_off = 0;
	/* chank_size = size_off(0) + SI0(nat_size) */
	eu.jm_chnk_sz = EU_CHNK_SZ_SI0;
	/* Trim size MS Nibble =  0 */
	eu.jm_op = EU_OP0_RAW;

	return __mod_rcp_eu_cmd_set(rcp, &eu);
}

s32 mod_reg_to_hdr_cpy(struct mod_recipe *rcp, s32 src, s32 layer, u8 off,
		       u8 sz)
{
	struct mod_eu_cmd eu;
	s32 ret;

	if (unlikely(ptr_is_null(rcp)))
		return -EINVAL;

	if (unlikely(!mod_is_dpu_valid(src)))
		return -EINVAL;

	if (unlikely(!mod_is_layer_valid(layer)))
		return -EINVAL;

	if (unlikely(!mod_is_op_size_valid(sz)))
		return -EINVAL;

	memset(&eu, 0, sizeof(eu));
	eu.type = EU_REPS0;

	ret = __mod_eu_cmd_alloc(rcp, eu.type);
	if (unlikely(ret))
		return ret;

	/* source dpu register */
	eu.load0      = src;
	/* use load0 value as is */
	eu.jm_load0   = EU_LOAD0_RAW;

	/* destination offset in straem */
	if (off == MOD_SI_PKT_OFF) {
		/* take dest offset from SI */
		eu.store   = MOD_PKT_OFF;
		eu.si_mux1 = EU_SI_SEL_LYR_FLD_OFF;
	} else {
		/* static dest offset */
		eu.store   = MOD_PKT_OFF | off;
		eu.si_mux1 = EU_SI_SEL_ZERO;
	}
	/* STW1 = layer offset from STW MUX */
	eu.stw_mux1 = layer;
	/* stw0 = L2 from STW MUX */
	eu.stw_mux0 = EU_STW_SEL_ZERO;
	eu.jm_store = EU_STORE_SI1_SW1_NO_SW0;

	/* size */
	eu.sz_off     = sz;
	eu.jm_chnk_sz = EU_CHNK_SZ_RAW;

	/* set to HW */
	return __mod_rcp_eu_cmd_set(rcp, &eu);
}

s32 mod_reg_to_new_hdr_cpy(struct mod_recipe *rcp, s32 src, s32 layer, u32 off,
			   u32 sz)
{
	struct mod_eu_cmd eu;
	s32 ret;

	if (unlikely(ptr_is_null(rcp)))
		return -EINVAL;

	if (unlikely(!mod_is_dpu_valid(src)))
		return -EINVAL;

	if (unlikely(!mod_is_ext_layer_valid(layer)))
		return -EINVAL;

	if (unlikely(!mod_is_op_size_valid(sz)))
		return -EINVAL;

	memset(&eu, 0, sizeof(eu));
	eu.type = EU_REPS1;

	ret = __mod_eu_cmd_alloc(rcp, eu.type);
	if (unlikely(ret))
		return ret;

	/* source dpu register */
	eu.load0      = src;
	/* use load0 value as is */
	eu.jm_load0   = EU_LOAD0_RAW;

	/* destination offset in stream */
	eu.store      = MOD_PKT_OFF | off;
	/* SI1 = layer in new header */
	eu.si_mux1    = layer;
	/* stw1 = L2 from STW MUX */
	eu.stw_mux1   = EU_STW_SEL_ZERO;
	eu.jm_store   = EU_STORE_SI1_SW1;

	/* size */
	eu.sz_off     = sz;
	eu.jm_chnk_sz = EU_CHNK_SZ_RAW;

	/* set to HW */
	return __mod_rcp_eu_cmd_set(rcp, &eu);
}

/**
 * @brief Tells if a given ALU operation takes just one operand
 * @param op_code ALU operand to check
 * @return bool true if op_code takes only one parameter, false otherwise
 */
static inline
bool __mod_alu_is_single_op(enum mod_cmd_fmt_alu_op op_code)
{
	return (EU_ALU_INC == op_code || EU_ALU_DEC == op_code);
}

/**
 * @brief Tells if a given ALU operation takes just one operand
 * @param alu ALU engine, EU_ALU0 or EU_ALU1
 * @param dst Destination register
 * @return bool true if the destination is valid for the specified alu
 */
static inline
bool __mod_alu_is_dst_valid(enum mod_eu_type alu, s32 dst)
{
	if (alu == EU_ALU1 && dst >= DPU_GLB_REG_FIRST &&
	    dst <= DPU_GLB_REG_LAST) {
		pr_err("Error, ALU1 cannot write to global registers\n");
		return false;
	}
	return true;
}

s32 mod_alu_cmd_cond(enum mod_eu_type alu, struct mod_recipe *rcp, s32 op1,
		     s32 sz1, s32 op2, s32 sz2, s32 dst,
		     enum mod_cmd_fmt_alu_op op_code,
		     enum mod_cmd_fmt_alu_cond_type cond,
		     enum mod_cmd_fmt_alu_cond_op cond_op)
{
	struct mod_eu_cmd eu;
	s32 ret;

	if (unlikely(ptr_is_null(rcp)))
		return -EINVAL;

	if (unlikely(!mod_is_dpu_valid(op1)))
		return -EINVAL;

	if (unlikely(!mod_is_dpu_valid(dst)))
		return -EINVAL;

	if (unlikely(!mod_is_op_size_valid(sz1)))
		return -EINVAL;

	if (unlikely(!mod_is_alu_eu_type(alu)))
		return -EINVAL;

	if (unlikely(!__mod_alu_is_dst_valid(alu, dst)))
		return -EINVAL;

	if (!__mod_alu_is_single_op(op_code)) {
		if (!mod_is_dpu_valid(op2) || !mod_is_op_size_valid(sz2))
			return -EINVAL;
	}

	ret = __mod_eu_cmd_alloc(rcp, alu);
	if (unlikely(ret))
		return ret;

	memset(&eu, 0, sizeof(eu));
	eu.type     = alu;
	/* configure reading operand1 from dpu registers */
	eu.load0    = op1;
	eu.jm_load0 = EU_LOAD0_RAW; /* use load0 value only */
	eu.load0_sz = MOD_TO_HW_LOAD_SZ(sz1);

	/* configure reading the operand2 from dpu registers */
	if (!__mod_alu_is_single_op(op_code)) {
		eu.load1    = op2;
		eu.jm_load1 = EU_LOAD1_RAW; /* use load1 value only */
		eu.load1_sz = MOD_TO_HW_LOAD_SZ(sz2);
	}

	/* configure saving the result to dpu dest register*/
	eu.store    = dst;
	eu.jm_store = EU_STORE_RAW; /* use dest value only */

	/* configure the operation to execute */
	eu.op0      = op_code;
	eu.jm_op    = EU_OP0_RAW;
	/* configure the execution condition */
	eu.op1      = cond_op << 4 | cond;
	/* execute immediately */
	eu.imm      = true;

	/* encode the eu size */
	eu.sz_off     = eu.load1_sz << 2 | eu.load0_sz;
	/* size of csum is taken from SIZE field only */
	eu.jm_chnk_sz = EU_CHNK_SZ_RAW;

	/* set to HW */
	return __mod_rcp_eu_cmd_set(rcp, &eu);
}

s32 mod_alu_synch(enum mod_eu_type alu, struct mod_recipe *rcp, s32 layer,
		  u8 off, u8 sz)
{
	struct mod_eu_cmd eu;
	s32 ret;

	if (unlikely(ptr_is_null(rcp)))
		return -EINVAL;

	if (unlikely(!mod_is_layer_valid(layer)))
		return -EINVAL;

	if (unlikely(!mod_is_alu_eu_type(alu)))
		return -EINVAL;

	ret = __mod_eu_cmd_alloc(rcp, alu);
	if (unlikely(ret))
		return ret;

	memset(&eu, 0, sizeof(eu));
	eu.type = alu;
	/* we always sync to the last byte of the field, hence, sz - 1 */
	if (off == MOD_SI_PKT_OFF) {
		eu.load0   = MOD_PKT_OFF | (sz - 1);
		eu.si_mux0 = EU_SI_SEL_LYR_FLD_OFF;
	} else {
		eu.load0   = MOD_PKT_OFF | (off + sz - 1);
		eu.si_mux0 = EU_SI_SEL_ZERO;
	}
	eu.stw_mux1   = layer;/* SW1 = SW[stw_mux1] */
	eu.stw_mux0   = EU_STW_SEL_ZERO;/* SW0 = SW[stw_mux0] */
	/* use load0 + SI0 + SW1 - SW0 */
	eu.jm_load0   = EU_LOAD0_SI0_SW1_NO_SW0;
	eu.jm_chnk_sz = EU_CHNK_SZ_RAW;
	eu.nop = true;

	return __mod_rcp_eu_cmd_set(rcp, &eu);
}

s32 mod_pkt_to_reg_cpy(struct mod_recipe *rcp, s32 dst, s32 layer, u8 off,
		       u8 sz)
{
	struct mod_eu_cmd eu;
	s32 ret;

	if (unlikely(ptr_is_null(rcp)))
		return -EINVAL;

	if (unlikely(!mod_is_dpu_valid(dst)))
		return -EINVAL;

	if (unlikely(!mod_is_layer_valid(layer)))
		return -EINVAL;

	if (unlikely(!mod_is_op_size_valid(sz)))
		return -EINVAL;

	memset(&eu, 0, sizeof(eu));
	eu.type = EU_STORE;

	ret = __mod_eu_cmd_alloc(rcp, eu.type);
	if (unlikely(ret))
		return ret;

	if (off == MOD_SI_PKT_OFF) {
		/* use offset from si */
		eu.load0   = MOD_PKT_OFF;
		eu.si_mux0 = EU_SI_SEL_LYR_FLD_OFF;
	} else {
		eu.load0   = MOD_PKT_OFF | off;
		eu.si_mux0 = EU_SI_SEL_ZERO;
	}
	eu.stw_mux1 = layer;/* SW1 = SW[stw_mux1] */
	eu.stw_mux0 = EU_STW_SEL_ZERO;/* SW0 = SW[stw_mux0] */
	/* use load0 + SI0 + SW1 - SW0 */
	eu.jm_load0 = EU_LOAD0_SI0_SW1_NO_SW0;
	eu.store = dst;
	eu.jm_store = EU_STORE_RAW; /* store = raw store field */
	eu.sz_off = sz;
	eu.jm_chnk_sz = EU_CHNK_SZ_RAW; /* size = raw size field*/

	return __mod_rcp_eu_cmd_set(rcp, &eu);
}

s32 mod_sce_mux_to_reg_cpy(struct mod_recipe *rcp, s32 dst, s32 off)
{
	struct mod_sce_cmd sce;
	s32 ret;

	if (unlikely(ptr_is_null(rcp)))
		return -EINVAL;

	if (unlikely(!mod_is_dpu_rcp_valid(dst)))
		return -EINVAL;

	if (unlikely(dst & 0x3)) {
		pr_err("error: sce dst reg must be register start addr\n");
		return -EINVAL;
	}

	if (unlikely(!mod_is_sce_mux_off_valid(off)))
		return -EINVAL;

	ret = __mod_eu_cmd_alloc(rcp, EU_SCE);
	if (unlikely(ret))
		return ret;

	memset(&sce, 0, sizeof(sce));

	/* for sce address use register id */
	sce.addr = MOD_DPU_ID(dst);

	sce.mux_sel = off;
	/* TODO: sce.cnt = cnt; */

	return __mod_rcp_sce_cmd_set(rcp, &sce);
}

s32 mod_hdr_rplc(struct mod_recipe *rcp, u32 bce_off, s32 end_trim_layer)
{
	struct mod_eu_cmd eu;
	s32 ret;

	if (unlikely(ptr_is_null(rcp)))
		return -EINVAL;

	if (unlikely(!mod_is_layer_valid(end_trim_layer)))
		return -EINVAL;

	if (unlikely(!mod_is_bce_off_valid(bce_off)))
		return -EINVAL;

	memset(&eu, 0, sizeof(eu));
	eu.type = EU_INSREP;

	ret = __mod_eu_cmd_alloc(rcp, eu.type);
	if (unlikely(ret))
		return ret;

	pr_debug("eu type %d\n", eu.type);
	/* SI0 = header temlate size from SI MUX */
	eu.si_mux0    = EU_SI_SEL_HDR_SZ;
	/* SI1 = 0 */
	eu.si_mux1    = 0;
	/* STW1 = L2 offset from STW MUX */
	eu.stw_mux1   = EU_STW_SEL_ZERO;
	/* stw0 = end_trim_layer from STW MUX */
	eu.stw_mux0   = end_trim_layer;
	/* header starts in offset bce_off of bce SI region */
	eu.load0      = MOD_SRAM_OFF | bce_off;
	/* use load0 val as is */
	eu.jm_load0   = EU_LOAD0_RAW;
	/* load1 = 0 */
	eu.load1      = 0;
	/* load1 = load1 + SI0 (si template size) */
	eu.jm_load1   = EU_LOAD1_SI0;
	/* always start trim from the packet start */
	eu.store      = 0;
	/* store = store + SW1(L2 off) + SI1(0) */
	eu.jm_store   = EU_STORE_SI1_SW1;
	/* sz_off = 0 */
	eu.sz_off     = 0;
	/* cnk_sz[3..0] = SW0[3..0](L3 off) - SW1[3..0](L2 off) */
	eu.jm_chnk_sz = EU_SW0_NO_SW1;
	/* cnk_sz[7..4] = SW0[7..4](L3 off) - SW1[7..4](L2 off) */
	eu.jm_op      = EU_OP0_SW0_NO_SW1;

	/* set to HW */
	return __mod_rcp_eu_cmd_set(rcp, &eu);
}

s32 mod_rcp_complete(struct mod_recipe *rcp)
{
	s32 ret = 0;
	u32 i;

	if (unlikely(ptr_is_null(rcp)))
		return -EINVAL;

	for (i = 0; i < ARRAY_SIZE(rcp->eu_cmd_cnt); i++) {
		if (rcp->eu_cmd_cnt[i]) {
			/* eu is in used, set last command */
			ret = __mod_eu_last_cmd_mod(rcp, i);
			if (unlikely(ret))
				goto done;
		} else {
			/* unused eu, add nop as last command */
			ret = __mod_eu_nop_cmd_add(rcp, i);
			if (unlikely(ret))
				goto done;
		}
	}

done:
	return ret;
}

s32 mod_eu_cmd_max_get(s32 eu, u32 *max_cmds)
{
	if (unlikely(ptr_is_null(max_cmds)))
		return -EINVAL;

	if (unlikely(!mod_is_eu_type_valid(eu)))
		return -EINVAL;

	*max_cmds = db->eu[eu].max_cmds;

	return 0;
}

/**
 * @brief initialized module database
 * @return s32 0 on success, error code otherwise
 */
static s32 __mod_db_init(void)
{
	/* Allocate module database */
	db = kzalloc(sizeof(*db), GFP_KERNEL);
	if (unlikely(!db)) {
		pr_err("cannot allocate nodule database\n");
		return -ENOMEM;
	}

	db->eu[EU_STORE].max_cmds   = MOD_STORE_CMD_MAX;
	db->eu[EU_STORE].base_addr  = MOD_STORE_RAM_BASE;
	db->eu[EU_STORE].cmd_sz     = MOD_STORE_CMD_SZ;

	db->eu[EU_PAD].max_cmds     = MOD_PAD_CMD_MAX;
	db->eu[EU_PAD].base_addr    = MOD_PAD_RAM_BASE;
	db->eu[EU_PAD].cmd_sz       = MOD_PAD_CMD_SZ;

	db->eu[EU_STRIP].max_cmds   = MOD_STRIP_CMD_MAX;
	db->eu[EU_STRIP].base_addr  = MOD_STRIP_RAM_BASE;
	db->eu[EU_STRIP].cmd_sz     = MOD_STRIP_CMD_SZ;

	db->eu[EU_REPS0].max_cmds   = MOD_REPS_CMD_MAX;
	db->eu[EU_REPS0].base_addr  = MOD_REPS0_RAM_BASE;
	db->eu[EU_REPS0].cmd_sz     = MOD_REPS_CMD_SZ;

	db->eu[EU_REPS1].max_cmds   = MOD_REPS_CMD_MAX;
	db->eu[EU_REPS1].base_addr  = MOD_REPS1_RAM_BASE;
	db->eu[EU_REPS1].cmd_sz     = MOD_REPS_CMD_SZ;

	db->eu[EU_INSREP].max_cmds  = MOD_INSREP_CMD_MAX;
	db->eu[EU_INSREP].base_addr = MOD_INSREP_RAM_BASE;
	db->eu[EU_INSREP].cmd_sz    = MOD_INSREP_CMD_SZ;

	db->eu[EU_ALU0].max_cmds    = MOD_ALU_CMD_MAX;
	db->eu[EU_ALU0].base_addr   = MOD_ALU0_RAM_BASE;
	db->eu[EU_ALU0].cmd_sz      = MOD_ALU_CMD_SZ;

	db->eu[EU_ALU1].max_cmds    = MOD_ALU_CMD_MAX;
	db->eu[EU_ALU1].base_addr   = MOD_ALU1_RAM_BASE;
	db->eu[EU_ALU1].cmd_sz      = MOD_ALU_CMD_SZ;

	db->eu[EU_SCE].max_cmds     = MOD_SCE_CMD_MAX;
	db->eu[EU_SCE].base_addr    = MOD_SCE_RAM_BASE;
	db->eu[EU_SCE].cmd_sz       = MOD_SCE_CMD_SZ;

	return 0;
}

void mod_stats_reset(void)
{
	u32 reset_val = 0xFFFFFFFF;
	u32 i;

	PP_REG_WR32(PP_MOD_CHK_PKT_CNT_REG, reset_val);
	PP_REG_WR32(PP_MOD_UC_PKT_CNT_REG,  reset_val);

	MOD_FOR_EACH_RCP(i)
		PP_REG_WR32(PP_MOD_RECIPE_CNT_REG_IDX(i), 0);
}

s32 mod_stats_get(struct mod_stats *stats)
{
	u32 i, cnt;

	if (unlikely(!stats)) {
		pr_err("Invalid stats parameter\n");
		return -EINVAL;
	}

	memset(stats, 0, sizeof(struct mod_stats));

	stats->pkts_rcvd_from_chk = PP_REG_RD32(PP_MOD_CHK_PKT_CNT_REG);
	stats->pkts_rcvd_from_uc  = PP_REG_RD32(PP_MOD_UC_PKT_CNT_REG);

	MOD_FOR_EACH_RCP(i) {
		cnt = PP_REG_RD32(PP_MOD_RECIPE_CNT_REG_IDX(i));
		stats->pkts_modified += cnt;
	}

	return 0;
}

/**
 * @brief initialized HW
 * @return s32 0 on success, error code otherwise
 */
static s32 __mod_hw_init(void)
{
	s32 ret;
	u32 cmd_idx, eu, rcp, reg = 0;
	struct mod_eu_hw cmd;

	/* wait for job manager init done */
	ret = pp_reg_poll(PP_MOD_STATUS_REG, PP_MOD_STATUS_JOBM_STATUS_MSK, 0);
	if (ret) {
		pr_err("Failed polling on modifier status\n");
		return ret;
	}

	/* enable both clients, classifier and uc */
	reg |= PP_FIELD_PREP(PP_MOD_CTRL_CLIENT_EN_MSK,          0x3);
	/* DPU RR arbitration */
	reg |= PP_FIELD_PREP(PP_MOD_CTRL_JM_DPU_ARB_SCHEME_MSK,  0x1);
	/* job manager RR arbitration */
	reg |= PP_FIELD_PREP(PP_MOD_CTRL_JM_CLNT_ARB_SCHEME_MSK, 0x1);
	/* no swapping for bce */
	reg |= PP_FIELD_PREP(PP_MOD_CTRL_JOBM_BCE_ENDIAN_MSK,    0x1);
	/* do swapping for sce */
	reg |= PP_FIELD_PREP(PP_MOD_CTRL_JOBM_SCE_ENDIAN_MSK,    0x0);
	/* write register */
	PP_REG_WR32(PP_MOD_CTRL_REG, reg);

	/* reset recipes in HW */
	memset(&cmd, 0, sizeof(cmd));
	MOD_FOR_EACH_RCP(rcp) {
		MOD_FOR_EACH_EU(eu) {
			MOD_FOR_EACH_EU_CMD(cmd_idx, db->eu[eu].max_cmds) {
				ret = mod_eu_cmd_wr(rcp, &cmd, eu, cmd_idx);
				if (unlikely(ret)) {
					pr_err("failed to reset recipes\n");
					return ret;
				}
			}
		}
	}

	return 0;
}

s32 mod_init(struct pp_mod_init_param *init_param)
{
	s32 ret = 0;
	struct mod_recipe rcp;

	if (unlikely(ptr_is_null(init_param)))
		return -EINVAL;

	if (!init_param->valid)
		return -EINVAL;

	pr_debug("mod_base_addr %#llx\n", init_param->mod_base);

	/* set HW base address */
	mod_base_addr = init_param->mod_base;
	mod_ram_base_addr = init_param->mod_ram_base;

	/* init module db */
	ret = __mod_db_init();
	if (unlikely(ret))
		goto err;

	/* init debugfs */
	ret = mod_dbg_init(init_param->dbgfs);
	if (unlikely(ret))
		goto err;

	/* init HW */
	ret = __mod_hw_init();
	if (unlikely(ret))
		goto err;

	/* create NOP recipe */
	memset(&rcp, 0, sizeof(rcp));
	rcp.index = MOD_RCP_NOP;
	ret = mod_rcp_complete(&rcp);
	if (unlikely(ret))
		goto err;

	/* init all recipes */
	ret = mod_rcp_init();
	if (unlikely(ret))
		goto err;

	pr_debug("modifier initialized\n");

	return 0;

err:
	pr_err("failed to init modifier module\n");
	return ret;
}

void mod_exit(void)
{
	mod_dbg_clean();
	kfree(db);
	db = NULL;
}
