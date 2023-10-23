/*
 * Description: Packet Processor Modifier Debug FS Interface
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2018-2019 Intel Corporation
 */

#define pr_fmt(fmt) "[PP_MODIFIER_DBG]: %s:%d: " fmt, __func__, __LINE__

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/debugfs.h>
#include <linux/parser.h>

#include "pp_debugfs_common.h"
#include "modifier.h"
#include "modifier_internal.h"
#include "pp_misc.h"

/**
 * @brief main modifier debugfs dir
 */
static struct dentry *dbgfs;
struct buf_fld_info mod_stw_flds[STW_MOD_FLDS_NUM];

struct pp_hw_stw_mod {
	u32 word[STW_MODIFIER_WORD_CNT];
};

struct pp_stw_mod_values {
	u32 val[STW_MOD_FLDS_NUM];
};

s32 pp_stw_mod_get(struct pp_stw_mod_values *stw_mod_vals)
{
	struct pp_hw_stw_mod stw_hw;
	u32 fld_idx;

	if (unlikely(ptr_is_null(stw_mod_vals)))
		return -EINVAL;

	PP_REG_RD_REP(PP_MOD_STW_STATUS_REG, &stw_hw, sizeof(stw_hw));

	for (fld_idx = 0 ; fld_idx < STW_MOD_FLDS_NUM; fld_idx++) {
		stw_mod_vals->val[fld_idx] =
			buf_fld_get(&mod_stw_flds[fld_idx], (void *)(&stw_hw));
	}

	return 0;
}

/**
 * @brief Dump status word
 * @param f seq_file
 * @param offset
 * @return int
 */
void __mod_stw_show(struct seq_file *f)
{
	struct pp_stw_mod_values  mod_vals;
	struct buf_fld_info      *fld;
	u32 fld_idx, val;
	s32 ret = 0;

	ret = pp_stw_mod_get(&mod_vals);
	if (unlikely(ret))
		return;

	seq_puts(f, " Modifier Status Word:\n");
	seq_puts(f, " =====================\n");
	for (fld_idx = 0 ; fld_idx < STW_MOD_FLDS_NUM ; fld_idx++) {
		fld = &mod_stw_flds[fld_idx];
		val = mod_vals.val[fld_idx];

		switch (fld_idx) {
		case STW_MOD_FLD_HDR_SZ_DIFF:
			seq_printf(f, " %-35s: %d [%#x]\n", fld->desc, (s32)val,
				   val);
			break;

		case STW_MOD_FLD_DROP_PKT:
			seq_printf(f, " %-35s: %s (%u)\n", fld->desc,
				   STW_DROP_PKT_STR(val), val);
			break;

		default:
			if (fld->msb == fld->lsb)
				seq_printf(f, " %-35s: %s\n", fld->desc,
					   BOOL2STR(val));
			else
				seq_printf(f, " %-35s: %u [%#x]\n", fld->desc,
					   val, val);
		}
	}
}

PP_DEFINE_DEBUGFS(stw_show, __mod_stw_show, NULL);

/* execution units names */
const char * const mod_eu_type_str[EU_MAX] = {
	"store",
	"pad",
	"strip",
	"reps0",
	"reps1",
	"insrep",
	"alu0",
	"alu1",
	"sce",
};

/**
 * @brief Debugfs help show API
 */
void mod_dbg_help_show(struct seq_file *f)
{
	seq_puts(f, "\n");
	seq_puts(f, " +-------------------------------------------------------------+\n");
	seq_puts(f, " |                    MODIFIER DEBUGFS HELP                    |\n");
	seq_puts(f, " +-------------------------------------------------------------+\n");
	seq_puts(f, " | print the modifier recipes  : cat recipes                   |\n");
	seq_puts(f, " +-------------------------------------------------------------+\n");
	seq_puts(f, " | print recipe verbose        : echo <rcp_id> > rcp_verbose   |\n");
	seq_puts(f, " +-------------------------------------------------------------+\n");
	seq_puts(f, " | print last pkt status word  : cat stw_show                  |\n");
	seq_puts(f, " +-------------------------------------------------------------+\n");
	seq_puts(f, " | goto custom recipe sub-menu : cd custom_rcp                 |\n");
	seq_puts(f, " +-------------------------------------------------------------+\n");
	seq_puts(f, "\n");
}

PP_DEFINE_DEBUGFS(help_show, mod_dbg_help_show, NULL);

/**
 * @brief Debugfs custom recipe help show API
 */
void mod_custom_dbg_help_show(struct seq_file *f)
{
	u32 eu;

	seq_puts(f, "\n");
	seq_puts(f, " +-------------------------------------------------------------+\n");
	seq_printf(f, " |             CUSTOM RECIPE (%03d) DEBUGFS HELP                |\n",
		   MOD_RCP_CUSTOM);
	seq_puts(f, " +-------------------------------------------------------------+\n");

	MOD_FOR_EACH_EU(eu) {
		seq_printf(f, " | set %-7s command : echo <cmd id> <4words cmd>  > %-7s |\n",
			   mod_eu_type_str[eu], mod_eu_type_str[eu]);
		seq_puts(f, " +-------------------------------------------------------------+\n");
	}

	seq_puts(f, "example: echo 0 0x00000000000000000000000140000000 > store\n\n");
}

PP_DEFINE_DEBUGFS(rcp_help_show, mod_custom_dbg_help_show, NULL);

/**
 * @brief Debugfs recipes description show API
 */
void mod_dbg_recipes_show(struct seq_file *f)
{
	u32 i, cnt;

	seq_puts(f, "\n");
	seq_puts(f, " +---------------------------------------------------------------------------------------------+\n");
	seq_puts(f, " |                      MODIFIER RECIPES                                                       |\n");
	seq_puts(f, " +---+---------------------------------------------------------+-------------------------------+\n");
	for (i = 0; i < MOD_RCP_IDX_MAX; i++) {
		cnt = PP_REG_RD32(PP_MOD_RECIPE_CNT_REG_IDX(i));
		/* print all the recipes that we configured */
		if (i <= MOD_RCP_LAST) {
			seq_printf(f, " |%-3u| %-55s | Modified Packets = %10u |\n",
			   i, mod_rcp_to_str(i), cnt);
			seq_puts(f, " +---+---------------------------------------------------------+-------------------------------+\n");
		} else if (cnt) {
			/* print only if the cnt isn't zero for all others */
			seq_printf(f, " |%-3u| %-55s | Modified Packets = %10u |\n",
			   i, mod_rcp_to_str(i), cnt);
			seq_puts(f, " +---+---------------------------------------------------------+-------------------------------+\n");
		}

	}
	seq_puts(f, "\n");
}

PP_DEFINE_DEBUGFS(recipes_show, mod_dbg_recipes_show, NULL);

/**
 * @brief show recipe commands
 * @param data unused
 * @param val recipe index
 * @return 0 on success
 */
static int mod_dbg_rcp_show(void *data, u64 val)
{
	u32 eu, cmd_idx, max_cmds;
	struct mod_eu_hw cmd;
	s32 ret;

	if (!capable(CAP_SYS_PACCT))
		return -EPERM;

	if (unlikely(!mod_is_rcp_id_valid((u32)val))) {
		pr_err("Invalid recipe %u\n", (u32)val);
		goto done;
	}

	if (unlikely((u32)val > MOD_RCP_LAST)) {
		pr_err("Recipe %u is not in used\n", (u32)val);
		goto done;
	}

	pr_info(" +--------------------------------------------------+\n");
	pr_info(" |                    RECIPE %3u                    |\n",
		(u32)val);
	pr_info(" +--------------------------------------------------+\n");

	MOD_FOR_EACH_EU(eu) {
		ret = mod_eu_cmd_max_get(eu, &max_cmds);
		if (unlikely(ret))
			goto done;

		pr_info(" | Execution unit: %-7s commands                 |\n",
			mod_eu_type_str[eu]);
		pr_info(" +--------------------------------------------------+\n");

		MOD_FOR_EACH_EU_CMD(cmd_idx, max_cmds) {
			ret = mod_eu_cmd_rd((u32)val, &cmd, eu, cmd_idx);
			if (unlikely(ret)) {
				pr_err("Failed to get eu %u command %u\n",
				       eu, cmd_idx);
				goto done;
			}
			if (!cmd.word[0] && !cmd.word[1] &&
			    !cmd.word[2] && !cmd.word[3])
				break;
			pr_info(" |    CMD%u:0x%08x%08x%08x%08x       |\n",
				cmd_idx, cmd.word[3], cmd.word[2],
				cmd.word[1], cmd.word[0]);
			pr_info(" +--------------------------------------------------+\n");
		}
	}
	pr_info("\n");

done:
	return 0;

}

PP_DEFINE_DBGFS_ATT(rcp_verbose, NULL, mod_dbg_rcp_show);

/**
 * @brief set custom recipe command
 * @param f file pointer
 * @param ubuf the user space buffer to read from
 * @param len the maximum number of bytes to read
 * @param ppos the current position in the buffer
 * @return ssize_t On success, the number of bytes written is
 *         returned, or negative value is returned on error
 */
void mod_dbg_eu_cmd_set(char *cmd_buf, void *data)
{
	ssize_t ret;
	char *str, *cmd_idx_str, word_str[9], prefix[3];
	struct mod_eu_hw cmd;
	u32 eu, cmd_idx = 0;
	s32 i;

	eu = (unsigned long)data;
	if (unlikely(eu > EU_MAX)) {
		pr_err("invalid eu %u, abort\n", eu);
		return;
	}

	str = cmd_buf;
	cmd_idx_str = strsep(&str, " \t");
	if (!*str || !cmd_idx_str) {
		pr_err("invalid command\n");
		return;
	}

	ret = kstrtou32(cmd_idx_str, 0, &cmd_idx);
	if (ret) {
		pr_err("failed to parse command index '%s'\n", cmd_idx_str);
		return;
	}

	str = strim(str);
	ret = strscpy(prefix, str, sizeof(prefix));
	if (strcmp(prefix, "0x") == 0) {
		str += 2;
		if (!*str) {
			pr_err("invalid command\n");
			return;
		}
	}

	if (strlen(str) != (ARRAY_SIZE(cmd.word) * 8)) {
		pr_err("invalid command, all %u word of command must be filled\n",
		       (u32)ARRAY_SIZE(cmd.word));
		return;
	}

	for (i = ARRAY_SIZE(cmd.word) - 1; i >= 0; i--) {
		if (!*str) {
			pr_err("error null character\n");
			return;
		}
		ret = strscpy(word_str, str, sizeof(word_str));
		ret = kstrtou32(word_str, 16, &cmd.word[i]);
		if (ret) {
			pr_err("failed to parse '%s'\n", word_str);
			return;
		}
		str += strlen(word_str);
	}
	for (i = 0; i < ARRAY_SIZE(cmd.word); i++)
		pr_info("WORD[%d]: 0x%08x\n", i, cmd.word[i]);

	ret = mod_eu_cmd_wr(MOD_RCP_CUSTOM, &cmd, eu, cmd_idx);
	if (unlikely(ret))
		return;
}

PP_DEFINE_DEBUGFS(eu_cmd, NULL, mod_dbg_eu_cmd_set);

enum eu_cmd_set_opts {
	eu_cmd_set_help,
	eu_cmd_set_rcp_id,
	eu_cmd_set_eu,
	eu_cmd_set_cmd,
	eu_cmd_set_word_idx,
	eu_cmd_set_word_val,
};

static const match_table_t eu_cmd_set_tokens = {
	{eu_cmd_set_help,     "help"},
	{eu_cmd_set_rcp_id,   "rcp=%u"},
	{eu_cmd_set_eu,       "eu=%u"},
	{eu_cmd_set_cmd,      "cmd=%u"},
	{eu_cmd_set_word_idx, "word=%u"},
	{eu_cmd_set_word_val, "val=%u"},
};

/**
 * @brief Session delete help
 */
static void __mod_rcp_eu_cmd_set_help(void)
{
	u32 i;

	pr_info("\n");
	pr_info(" Brief: Change recipe command in one of the engine units\n");
	pr_info(" Usage: echo <option=value> [help]\n");
	pr_info(" Options:\n");
	pr_info("   help  - print this help function\n");
	pr_info("   rcp   - recipe id\n");
	pr_info("   cmd   - command id within the engin unit\n");
	pr_info("   word  - word index within the command\n");
	pr_info("   val   - word new value\n");
	pr_info("   eu    - engine unit id:\n");
	MOD_FOR_EACH_EU(i)
		pr_info("              %u: %s\n", i, mod_eu_type_str[i]);
	pr_info(" Examples:\n");
	pr_info("   set word 3 of command 2 for alu0 in recipe 5 to 0xff:\n");
	pr_info("     echo rcp=5 eu=6 cmd=2, word=3 val=0xff > ...\n\n");
}

/**
 * @brief Set session's destination queue
 */
void __mod_eu_cmd_set(char *args, void *data)
{
	substring_t substr[MAX_OPT_ARGS];
	enum eu_cmd_set_opts opt;
	char *tok;
	int val, ret;
	u32 rcp, eu, cmd, word, word_val;

	rcp      = MOD_RCPS_NUM;
	eu       = EU_MAX;
	cmd      = U32_MAX;
	word     = U32_MAX;
	word_val = 0;

	args = strim(args);
	/* iterate over user arguments */
	while ((tok = strsep(&args, " \t\n")) != NULL) {
		if (!*tok)
			continue;
		opt = match_token(tok, eu_cmd_set_tokens, substr);
		switch (opt) {
		case eu_cmd_set_help:
			__mod_rcp_eu_cmd_set_help();
			if (strcmp(tok, "help")) {
				pr_info("Error: Invalid option '%s'\n", tok);
				goto opt_parse_err;
			} else {
				return;
			}
		case eu_cmd_set_rcp_id:
			if (match_int(&substr[0], &val))
				goto opt_parse_err;
			rcp = (u32)val;
			break;
		case eu_cmd_set_eu:
			if (match_int(&substr[0], &val))
				goto opt_parse_err;
			eu = (u32)val;
			break;
		case eu_cmd_set_cmd:
			if (match_int(&substr[0], &val))
				goto opt_parse_err;
			cmd = (u32)val;
			break;
		case eu_cmd_set_word_idx:
			if (match_int(&substr[0], &val))
				goto opt_parse_err;
			word = (u32)val;
			break;
		case eu_cmd_set_word_val:
			if (match_int(&substr[0], &val))
				goto opt_parse_err;
			word_val = (u32)val;
			break;
		default:
			break;
		}
	}
	if (rcp == MOD_RCPS_NUM)
		return;

	ret = mod_eu_cmd_word_set(rcp, cmd, eu, word, word_val);
	if (unlikely(ret))
		pr_info("Failed to set eu command, ret %d\n", ret);

	return;

opt_parse_err:
	pr_err("Failed to parse input\n");
}

PP_DEFINE_DEBUGFS(eu_cmd_set, NULL, __mod_eu_cmd_set);

static struct debugfs_file dbg_files[] = {
	{ "rcp_verbose", &PP_DEBUGFS_FOPS(rcp_verbose) },
	{ "stw_show", &PP_DEBUGFS_FOPS(stw_show) },
	{ "help", &PP_DEBUGFS_FOPS(help_show) },
	{ "recipes", &PP_DEBUGFS_FOPS(recipes_show) },
	{ "eu_cmd_set", &PP_DEBUGFS_FOPS(eu_cmd_set) },
};

s32 mod_dbg_init(struct dentry *parent)
{
	struct dentry *dent           = NULL;
	struct dentry *custom_rcp_dir = NULL;
	u32 eu;
	s32 ret;

	if (unlikely(!debugfs_initialized())) {
		pr_err("Debugfs not initialized yet\n");
		return -EPERM;
	}

	/* modifier debugfs dir */
	ret = pp_debugfs_create(parent, "modifier", &dbgfs, dbg_files,
				ARRAY_SIZE(dbg_files), NULL);

	/* custom recipe debugfs dir */
	custom_rcp_dir = debugfs_create_dir("custom_rcp", dbgfs);
	if (unlikely(!custom_rcp_dir)) {
		pr_err("Failed to create the custom rcp debugfs dir\n");
		return -ENOMEM;
	}

	dent = debugfs_create_file("help", 0600, custom_rcp_dir, NULL,
				   &PP_DEBUGFS_FOPS(rcp_help_show));
	if (unlikely(IS_ERR_OR_NULL(dent)))
		return (s32)PTR_ERR(dent);

	MOD_FOR_EACH_EU(eu) {
		/* custom recipe eu commands debugfs */
		dent = debugfs_create_file(mod_eu_type_str[eu], 0600,
					   custom_rcp_dir,
					   (void *)(unsigned long)eu,
					   &PP_DEBUGFS_FOPS(eu_cmd));
		if (unlikely(IS_ERR_OR_NULL(dent))) {
			pr_err("Failed to create eu set debugfs file\n");
			return -ENOMEM;
		}
	}

	/* Init status word common fields */
	STW_INIT_COMMON_FLDS(mod_stw_flds);
	/* Init modifier specific status word fields */
	BUF_FLD_INIT_DESC(mod_stw_flds, STW_MOD_FLD_TTL_EXPIRED);
	BUF_FLD_INIT_DESC(mod_stw_flds, STW_MOD_FLD_IP_UNSUPP);
	BUF_FLD_INIT_DESC(mod_stw_flds, STW_MOD_FLD_EXT_DF);
	BUF_FLD_INIT_DESC(mod_stw_flds, STW_MOD_FLD_INT_DF);
	BUF_FLD_INIT_DESC(mod_stw_flds, STW_MOD_FLD_EXT_FRAG_TYPE);
	BUF_FLD_INIT_DESC(mod_stw_flds, STW_MOD_FLD_INT_FRAG_TYPE);
	BUF_FLD_INIT_DESC(mod_stw_flds, STW_MOD_FLD_TCP_FIN);
	BUF_FLD_INIT_DESC(mod_stw_flds, STW_MOD_FLD_TCP_SYN);
	BUF_FLD_INIT_DESC(mod_stw_flds, STW_MOD_FLD_TCP_RST);
	BUF_FLD_INIT_DESC(mod_stw_flds, STW_MOD_FLD_TCP_ACK);
	BUF_FLD_INIT_DESC(mod_stw_flds, STW_MOD_FLD_TCP_DATA_OFFSET);
	BUF_FLD_INIT_DESC(mod_stw_flds, STW_MOD_FLD_EXT_L3_OFFSET);
	BUF_FLD_INIT_DESC(mod_stw_flds, STW_MOD_FLD_INT_L3_OFFSET);
	BUF_FLD_INIT_DESC(mod_stw_flds, STW_MOD_FLD_EXT_L4_OFFSET);
	BUF_FLD_INIT_DESC(mod_stw_flds, STW_MOD_FLD_INT_L4_OFFSET);
	BUF_FLD_INIT_DESC(mod_stw_flds, STW_MOD_FLD_EXT_FRAG_INFO_OFFSET);
	BUF_FLD_INIT_DESC(mod_stw_flds, STW_MOD_FLD_INT_FRAG_INFO_OFFSET);
	BUF_FLD_INIT_DESC(mod_stw_flds, STW_MOD_FLD_L3_HDR_OFFSET_5);
	BUF_FLD_INIT_DESC(mod_stw_flds, STW_MOD_FLD_L3_HDR_OFFSET_4);
	BUF_FLD_INIT_DESC(mod_stw_flds, STW_MOD_FLD_L3_HDR_OFFSET_3);
	BUF_FLD_INIT_DESC(mod_stw_flds, STW_MOD_FLD_L3_HDR_OFFSET_2);
	BUF_FLD_INIT_DESC(mod_stw_flds, STW_MOD_FLD_L3_HDR_OFFSET_1);
	BUF_FLD_INIT_DESC(mod_stw_flds, STW_MOD_FLD_L3_HDR_OFFSET_0);
	BUF_FLD_INIT_DESC(mod_stw_flds, STW_MOD_FLD_UC_ADDED_BYTES);
	BUF_FLD_INIT_DESC(mod_stw_flds, STW_MOD_FLD_TUNN_INNER_OFFSET);
	BUF_FLD_INIT_DESC(mod_stw_flds, STW_MOD_FLD_PAYLOAD_OFFSET);
	BUF_FLD_INIT_DESC(mod_stw_flds, STW_MOD_FLD_ERROR_IND);
	BUF_FLD_INIT_DESC(mod_stw_flds, STW_MOD_FLD_RPB_CLID);
	BUF_FLD_INIT_DESC(mod_stw_flds, STW_MOD_FLD_DST_QUEUE);
	BUF_FLD_INIT_DESC(mod_stw_flds, STW_MOD_FLD_USER_DEFINE_LEN);
	BUF_FLD_INIT_DESC(mod_stw_flds, STW_MOD_FLD_HDR_SZ_DIFF);
	mod_stw_flds[STW_MOD_FLD_HDR_SZ_DIFF].from_hw = fld_diff_from_hw;
	BUF_FLD_INIT_DESC(mod_stw_flds, STW_MOD_FLD_DROP_PKT);
	BUF_FLD_INIT_DESC(mod_stw_flds, STW_MOD_FLD_MOD_PBUFF);

	return 0;
}

void mod_dbg_clean(void)
{
	debugfs_remove_recursive(dbgfs);
	dbgfs = NULL;
}
