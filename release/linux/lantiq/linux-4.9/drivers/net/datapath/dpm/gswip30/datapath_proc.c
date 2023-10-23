/*
 * Copyright (C) Intel Corporation
 * Author: Shao Guohua <guohua.shao@intel.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 */
#include <linux/module.h>
#include <net/datapath_proc_api.h>	/*for proc api*/
#include <net/datapath_api.h>
#include <net/drv_tmu_ll.h>
#include <linux/list.h>
#include <net/lantiq_cbm_api.h>

#include <linux/list.h>

#include "../datapath.h"
#include "datapath_misc.h"

#define PROC_PARSER "parser"
#define PROC_COC "coc"
#define PROC_RMON_PORTS  "rmon"
#define PROC_EP "ep"	/*EP/port ID info */
#define PROC_DPORT "dport"	/*TMU dequeue port info */
#define DP_PROC_CBMLOOKUP "lookup"

#define DEBUGFS_RMON DP_DEBUGFS_PATH "/" PROC_RMON_PORTS
#define DEBUGFS_EP DP_DEBUGFS_PATH "/" PROC_EP

#define MAX_GSW_L_PMAC_PORT  7
#define MAX_GSW_R_PMAC_PORT  16
static GSW_RMON_Port_cnt_t gsw_l_rmon_mib[MAX_GSW_L_PMAC_PORT];
static GSW_RMON_Port_cnt_t gsw_r_rmon_mib[MAX_GSW_R_PMAC_PORT];
static GSW_RMON_Redirect_cnt_t gswr_rmon_redirect;

enum RMON_MIB_TYPE {
	RX_GOOD_PKTS = 0,
	RX_FILTER_PKTS,
	RX_DROP_PKTS,
	RX_OTHERS,

	TX_GOOD_PKTS,
	TX_ACM_PKTS,
	TX_DROP_PKTS,
	TX_OTHERS,

	REDIRECT_MIB,
	DP_DRV_MIB,

	/*last entry */
	RMON_MAX
};

static char f_q_mib_title_proc;
static int tmp_inst;

#define RMON_DASH_LINE 130
static void print_dash_line(struct seq_file *s)
{
	char buf[RMON_DASH_LINE + 4];

	memset(buf, '-', RMON_DASH_LINE);
	sprintf(buf + RMON_DASH_LINE, "\n");
	seq_puts(s, buf);
}

#define GSW_PORT_RMON_PRINT(title, var)  do { \
	seq_printf(s, \
		   "%-14s%10s %12u %12u %12u %12u %12u %12u %12u\n", \
		   title, "L(0-6)", \
		   gsw_l_rmon_mib[0].var, gsw_l_rmon_mib[1].var, \
		   gsw_l_rmon_mib[2].var, gsw_l_rmon_mib[3].var, \
		   gsw_l_rmon_mib[4].var, gsw_l_rmon_mib[5].var, \
		   gsw_l_rmon_mib[6].var); \
	seq_printf(s, \
		   "%-14s%10s %12u %12u %12u %12u %12u %12u %12u %12u\n", \
		   title, "R(0-6,15)", \
		   gsw_r_rmon_mib[0].var, gsw_r_rmon_mib[1].var, \
		   gsw_r_rmon_mib[2].var, gsw_r_rmon_mib[3].var, \
		   gsw_r_rmon_mib[4].var, gsw_r_rmon_mib[5].var, \
		   gsw_r_rmon_mib[6].var, gsw_r_rmon_mib[15].var); \
	seq_printf(s, \
		   "%-14s%10s %12u %12u %12u %12u %12u %12u %12u %12u\n", \
		   title, "R(7-14)", \
		   gsw_r_rmon_mib[7].var, gsw_r_rmon_mib[8].var, \
		   gsw_r_rmon_mib[9].var, gsw_r_rmon_mib[10].var, \
		   gsw_r_rmon_mib[11].var, gsw_r_rmon_mib[12].var, \
		   gsw_r_rmon_mib[13].var, gsw_r_rmon_mib[14].var); \
	print_dash_line(s); \
} while (0)

static void proc_parser_read(struct seq_file *s);
static ssize_t proc_parser_write(struct file *, const char *, size_t,
				 loff_t *);
static int proc_ep_dump(struct seq_file *s, int pos);
static ssize_t ep_port_write(struct file *, const char *, size_t, loff_t *);
static int proc_dport_dump(struct seq_file *s, int pos);
static int rmon_display_tmu_mib = 1;
static int rmon_display_port_full;

#ifndef CONFIG_LTQ_TMU
void tmu_qoct_read(const u32 qid,
		   u32 *wq,
		   u32 *qrth, u32 *qocc, u32 *qavg)
{
}

void tmu_qdct_read(const u32 qid, u32 *qdc)
{
}

char *get_dma_flags_str(u32 epn, char *buf, int buf_len)
{
	return NULL;
}

void tmu_equeue_link_get(const u32 qid, struct tmu_equeue_link *link)
{
}

u32 get_enq_counter(u32 index)
{
	return 0;
}

u32 get_deq_counter(u32 index)
{
	return 0;
}

#endif

static void proc_parser_read(struct seq_file *s)
{
	s8 cpu, mpe1, mpe2, mpe3;

	if (!capable(CAP_NET_ADMIN))
		return;
	dp_get_gsw_parser_30(&cpu, &mpe1, &mpe2, &mpe3);
	seq_printf(s, "cpu : %s with parser size =%d bytes\n",
		   parser_flag_str(cpu), parser_size_via_index(0));
	seq_printf(s, "mpe1: %s with parser size =%d bytes\n",
		   parser_flag_str(mpe1), parser_size_via_index(1));
	seq_printf(s, "mpe2: %s with parser size =%d bytes\n",
		   parser_flag_str(mpe2), parser_size_via_index(2));
	seq_printf(s, "mpe3: %s with parser size =%d bytes\n",
		   parser_flag_str(mpe3), parser_size_via_index(3));
}

static ssize_t proc_parser_write(struct file *file, const char *buf,
				 size_t count, loff_t *ppos)
{
	int len;
	char str[64];
	int num, i;
	char *param_list[20];
	s8 cpu = 0, mpe1 = 0, mpe2 = 0, mpe3 = 0, flag = 0;
	static int pce_rule_id;
	static GSW_PCE_rule_t pce;
	int inst = 0;
	struct core_ops *gsw_handle;
	struct tflow_ops *gsw_tflow;

	if (!capable(CAP_NET_ADMIN))
		return -EPERM;
	memset(&pce, 0, sizeof(pce));
	gsw_handle = dp_port_prop[inst].ops[GSWIP_R];
	gsw_tflow = &gsw_handle->gsw_tflow_ops;
	len = (sizeof(str) > count) ? count : sizeof(str) - 1;
	len -= copy_from_user(str, buf, len);
	str[len] = 0;
	num = dp_split_buffer(str, param_list, ARRAY_SIZE(param_list));

	if (dp_strncmpi(param_list[0], "enable", strlen("enable")) == 0) {
		for (i = 1; i < num; i++) {
			if (dp_strncmpi(param_list[i],
					"cpu",
					strlen("cpu"))
					== 0) {
				flag |= 0x1;
				cpu = 2;
			}

			if (dp_strncmpi(param_list[i],
					"mpe1",
					strlen("mpe1"))
					== 0) {
				flag |= 0x2;
				mpe1 = 2;
			}

			if (dp_strncmpi(param_list[i],
					"mpe2",
					strlen("mpe2"))
					== 0) {
				flag |= 0x4;
				mpe2 = 2;
			}

			if (dp_strncmpi(param_list[i],
					"mpe3",
					strlen("mpe3"))
					== 0) {
				flag |= 0x8;
				mpe3 = 2;
			}
		}

		if (!flag) {
			flag = 0x1 | 0x2 | 0x4 | 0x8;
			cpu = 2;
			mpe1 = 2;
			mpe2 = 2;
			mpe3 = 2;
		}

		DP_DEBUG(DP_DBG_FLAG_DBG,
			 "flag=0x%x mpe3/2/1/cpu=%d/%d/%d/%d\n", flag, mpe3,
			 mpe2, mpe1, cpu);
		dp_set_gsw_parser_30(flag, cpu, mpe1, mpe2, mpe3);
	} else if (dp_strncmpi(param_list[0],
				"disable",
				strlen("disable"))
				== 0) {
		for (i = 1; i < num; i++) {
			if (dp_strncmpi(param_list[i],
					"cpu",
					strlen("cpu"))
					== 0) {
				flag |= 0x1;
				cpu = 0;
			}

			if (dp_strncmpi(param_list[i],
					"mpe1",
					strlen("mpe1"))
					== 0) {
				flag |= 0x2;
				mpe1 = 0;
			}

			if (dp_strncmpi(param_list[i],
					"mpe2",
					strlen("mpe2"))
					== 0) {
				flag |= 0x4;
				mpe2 = 0;
			}

			if (dp_strncmpi(param_list[i],
					"mpe3",
					strlen("mpe3"))
					== 0) {
				flag |= 0x8;
				mpe3 = 0;
			}
		}

		if (!flag) {
			flag = 0x1 | 0x2 | 0x4 | 0x8;
			cpu = 0;
			mpe1 = 0;
			mpe2 = 0;
			mpe3 = 0;
		}

		DP_DEBUG(DP_DBG_FLAG_DBG,
			 "flag=0x%x mpe3/2/1/cpu=%d/%d/%d/%d\n", flag, mpe3,
			 mpe2, mpe1, cpu);
		dp_set_gsw_parser_30(flag, cpu, mpe1, mpe2, mpe3);
	} else if (dp_strncmpi(param_list[0], "refresh",
			       strlen("refresh")) == 0) {
		dp_get_gsw_parser_30(NULL, NULL, NULL, NULL);
		pr_info("value:cpu=%d mpe1=%d mpe2=%d mpe3=%d\n", pinfo[0].v,
			pinfo[1].v, pinfo[2].v, pinfo[3].v);
		pr_info("size :cpu=%d mpe1=%d mpe2=%d mpe3=%d\n",
			pinfo[0].size, pinfo[1].size, pinfo[2].size,
			pinfo[3].size);
		return count;
	} else if (dp_strncmpi(param_list[0], "mark", strlen("mark")) == 0) {
		int flag = dp_atoi(param_list[1]);

		if (flag < 0)
			flag = 0;
		else if (flag > 3)
			flag = 3;
		pr_info("eProcessPath_Action set to %d\n", flag);
		/*: All packets set to same mpe flag as specified */
		memset(&pce, 0, sizeof(pce));
		pce.pattern.nIndex = pce_rule_id;
		pce.pattern.bEnable = 1;

		pce.pattern.bParserFlagMSB_Enable = 1;
		pce.pattern.nParserFlagMSB_Mask = 0xffff;
		pce.pattern.bParserFlagLSB_Enable = 1;
		pce.pattern.nParserFlagLSB_Mask = 0xffff;
		pce.pattern.nDstIP_Mask = 0xffffffff;
		pce.pattern.bDstIP_Exclude = 0;

		pce.action.bRtDstPortMaskCmp_Action = 1;
		pce.action.bRtSrcPortMaskCmp_Action = 1;
		pce.action.bRtDstIpMaskCmp_Action = 1;
		pce.action.bRtSrcIpMaskCmp_Action = 1;

		pce.action.bRoutExtId_Action = 1;
		pce.action.nRoutExtId = 0; /*RT_EXTID_UDP; */
		pce.action.bRtAccelEna_Action = 1;
		pce.action.bRtCtrlEna_Action = 1;
		pce.action.eProcessPath_Action = flag;
		pce.action.bRMON_Action = 1;
		pce.action.nRMON_Id = 0;	/*RMON_UDP_CNTR; */

		if (gsw_tflow->TFLOW_PceRuleWrite(gsw_handle, &pce)) {
			pr_err("PCE rule add fail for GSW_PCE_RULE_WRITE\n");
			return count;
		}

	} else if (dp_strncmpi(param_list[0],
				     "unmark",
					 strlen("unmark"))
					 == 0) {
		/*: All packets set to same mpe flag as specified */
		memset(&pce, 0, sizeof(pce));
		pce.pattern.nIndex = pce_rule_id;
		pce.pattern.bEnable = 0;
		if (gsw_tflow->TFLOW_PceRuleWrite(gsw_handle, &pce)) {
			pr_err("PCE rule add fail for GSW_PCE_RULE_WRITE\n");
			return count;
		}
	} else {
		pr_info("Usage: echo %s > parser\n",
			"<enable/disable> [cpu] [mpe1] [mpe2] [mpe3]");
		pr_info("Usage: echo <refresh> parser\n");

		pr_info("Usage: echo %s > parser\n",
			" mark eProcessPath_Action_value(0~3)");
		pr_info("Usage: echo unmark > parser\n");
		return count;
	}

	return count;
}

#define GSW_PORT_RMON64_PRINT(title, var)  do { \
	seq_printf(s, "%-14s%10s %12u %12u %12u %12u %12u %12u %12u\n", \
		   title "(H)", "L(0-6)", \
		   high_10dec(gsw_l_rmon_mib[0].var), \
		   high_10dec(gsw_l_rmon_mib[1].var), \
		   high_10dec(gsw_l_rmon_mib[2].var),  \
		   high_10dec(gsw_l_rmon_mib[3].var), \
		   high_10dec(gsw_l_rmon_mib[4].var),  \
		   high_10dec(gsw_l_rmon_mib[5].var), \
		   high_10dec(gsw_l_rmon_mib[6].var)); \
	seq_printf(s, "%-14s%10s %12u %12u %12u %12u %12u %12u %12u\n", \
		   title "(L)", "L(0-6)", \
		   low_10dec(gsw_l_rmon_mib[0].var), \
		   low_10dec(gsw_l_rmon_mib[1].var), \
		   low_10dec(gsw_l_rmon_mib[2].var), \
		   low_10dec(gsw_l_rmon_mib[3].var), \
		   low_10dec(gsw_l_rmon_mib[4].var), \
		   low_10dec(gsw_l_rmon_mib[5].var), \
		   low_10dec(gsw_l_rmon_mib[6].var)); \
	seq_printf(s, "%-14s%10s %12u %12u %12u %12u %12u %12u %12u %12u\n", \
		   title "(H)", "R(0-6,15)", \
		   high_10dec(gsw_r_rmon_mib[0].var), \
		   high_10dec(gsw_r_rmon_mib[1].var), \
		   high_10dec(gsw_r_rmon_mib[2].var), \
		   high_10dec(gsw_r_rmon_mib[3].var), \
		   high_10dec(gsw_r_rmon_mib[4].var), \
		   high_10dec(gsw_r_rmon_mib[5].var), \
		   high_10dec(gsw_r_rmon_mib[6].var), \
		   high_10dec(gsw_r_rmon_mib[15].var)); \
	seq_printf(s, "%-14s%10s %12u %12u %12u %12u %12u %12u %12u %12u\n", \
		   title "(L)", "R(0-6,15)", \
		   low_10dec(gsw_r_rmon_mib[0].var), \
		   low_10dec(gsw_r_rmon_mib[1].var), \
		   low_10dec(gsw_r_rmon_mib[2].var), \
		   low_10dec(gsw_r_rmon_mib[3].var), \
		   low_10dec(gsw_r_rmon_mib[4].var), \
		   low_10dec(gsw_r_rmon_mib[5].var), \
		   low_10dec(gsw_r_rmon_mib[6].var), \
		   low_10dec(gsw_r_rmon_mib[15].var)); \
	seq_printf(s, "%-14s%10s %12u %12u %12u %12u %12u %12u %12u %12u\n", \
		   title "(H)", "R(7-14)", \
		   high_10dec(gsw_r_rmon_mib[7].var), \
		   high_10dec(gsw_r_rmon_mib[8].var), \
		   high_10dec(gsw_r_rmon_mib[9].var),  \
		   high_10dec(gsw_r_rmon_mib[10].var), \
		   high_10dec(gsw_r_rmon_mib[11].var), \
		   high_10dec(gsw_r_rmon_mib[12].var), \
		   high_10dec(gsw_r_rmon_mib[13].var), \
		   high_10dec(gsw_r_rmon_mib[14].var)); \
	seq_printf(s, \
		   "%-14s%10s %12u %12u %12u %12u %12u %12u %12u %12u\n", \
		   title "(L)", "R(7-14)", \
		   low_10dec(gsw_r_rmon_mib[7].var), \
		   low_10dec(gsw_r_rmon_mib[8].var), \
		   low_10dec(gsw_r_rmon_mib[9].var), \
		   low_10dec(gsw_r_rmon_mib[10].var), \
		   low_10dec(gsw_r_rmon_mib[11].var), \
		   low_10dec(gsw_r_rmon_mib[12].var), \
		   low_10dec(gsw_r_rmon_mib[13].var), \
		   low_10dec(gsw_r_rmon_mib[14].var)); \
	print_dash_line(s); \
	} while (0)

typedef int (*ingress_pmac_set_callback_t) (dp_pmac_cfg_t *pmac_cfg,
					    u32 value);
typedef int (*egress_pmac_set_callback_t) (dp_pmac_cfg_t *pmac_cfg,
					   u32 value);
struct ingress_pmac_entry {
	char *name;
	ingress_pmac_set_callback_t ingress_callback;
};

struct egress_pmac_entry {
	char *name;
	egress_pmac_set_callback_t egress_callback;
};

static int ingress_err_disc_set(dp_pmac_cfg_t *pmac_cfg, u32 value)
{
	pmac_cfg->ig_pmac.err_disc = value;
	pmac_cfg->ig_pmac_flags = IG_PMAC_F_ERR_DISC;
	return 0;
}

static int ingress_pmac_set(dp_pmac_cfg_t *pmac_cfg, u32 value)
{
	pmac_cfg->ig_pmac.pmac = value;
	pmac_cfg->ig_pmac_flags = IG_PMAC_F_PRESENT;
	return 0;
}

static int ingress_pmac_pmap_set(dp_pmac_cfg_t *pmac_cfg, u32 value)
{
	pmac_cfg->ig_pmac.def_pmac_pmap = value;
	pmac_cfg->ig_pmac_flags = IG_PMAC_F_PMAP;
	return 0;
}

static int ingress_pmac_en_pmap_set(dp_pmac_cfg_t *pmac_cfg, u32 value)
{
	pmac_cfg->ig_pmac.def_pmac_en_pmap = value;
	pmac_cfg->ig_pmac_flags = IG_PMAC_F_PMAPENA;
	return 0;
}

static int ingress_pmac_tc_set(dp_pmac_cfg_t *pmac_cfg, u32 value)
{
	pmac_cfg->ig_pmac.def_pmac_tc = value;
	pmac_cfg->ig_pmac_flags = IG_PMAC_F_CLASS;
	return 0;
}

static int ingress_pmac_en_tc_set(dp_pmac_cfg_t *pmac_cfg, u32 value)
{
	pmac_cfg->ig_pmac.def_pmac_en_tc = value;
	pmac_cfg->ig_pmac_flags = IG_PMAC_F_CLASSENA;
	return 0;
}

static int ingress_pmac_subifid_set(dp_pmac_cfg_t *pmac_cfg, u32 value)
{
	pmac_cfg->ig_pmac.def_pmac_subifid = value;
	pmac_cfg->ig_pmac_flags = IG_PMAC_F_SUBIF;
	return 0;
}

static int ingress_pmac_srcport_set(dp_pmac_cfg_t *pmac_cfg, u32 value)
{
	pmac_cfg->ig_pmac.def_pmac_src_port = value;
	pmac_cfg->ig_pmac_flags = IG_PMAC_F_SPID;
	return 0;
}

static int ingress_pmac_hdr1_set(dp_pmac_cfg_t *pmac_cfg, u32 value)
{
	u8 hdr;

	hdr = (u8)value;
	pmac_cfg->ig_pmac.def_pmac_hdr[0] = hdr;
	pmac_cfg->ig_pmac_flags = IG_PMAC_F_PMACHDR1;
	return 0;
}

static int ingress_pmac_hdr2_set(dp_pmac_cfg_t *pmac_cfg, u32 value)
{
	u8 hdr;

	hdr = (u8)value;
	pmac_cfg->ig_pmac.def_pmac_hdr[1] = hdr;
	pmac_cfg->ig_pmac_flags = IG_PMAC_F_PMACHDR2;
	return 0;
}

static int ingress_pmac_hdr3_set(dp_pmac_cfg_t *pmac_cfg, u32 value)
{
	u8 hdr;

	hdr = (u8)value;
	pmac_cfg->ig_pmac.def_pmac_hdr[2] = hdr;
	pmac_cfg->ig_pmac_flags = IG_PMAC_F_PMACHDR3;
	return 0;
}

static int ingress_pmac_hdr4_set(dp_pmac_cfg_t *pmac_cfg, u32 value)
{
	u8 hdr;

	hdr = (u8)value;
	pmac_cfg->ig_pmac.def_pmac_hdr[3] = hdr;
	pmac_cfg->ig_pmac_flags = IG_PMAC_F_PMACHDR4;
	return 0;
}

static int ingress_pmac_hdr5_set(dp_pmac_cfg_t *pmac_cfg, u32 value)
{
	u8 hdr;

	hdr = (u8)value;
	pmac_cfg->ig_pmac.def_pmac_hdr[4] = hdr;
	pmac_cfg->ig_pmac_flags = IG_PMAC_F_PMACHDR5;
	return 0;
}

static int ingress_pmac_hdr6_set(dp_pmac_cfg_t *pmac_cfg, u32 value)
{
	u8 hdr;

	hdr = (u8)value;
	pmac_cfg->ig_pmac.def_pmac_hdr[5] = hdr;
	pmac_cfg->ig_pmac_flags = IG_PMAC_F_PMACHDR6;
	return 0;
}

static int ingress_pmac_hdr7_set(dp_pmac_cfg_t *pmac_cfg, u32 value)
{
	u8 hdr;

	hdr = (u8)value;
	pmac_cfg->ig_pmac.def_pmac_hdr[6] = hdr;
	pmac_cfg->ig_pmac_flags = IG_PMAC_F_PMACHDR7;
	return 0;
}

static int ingress_pmac_hdr8_set(dp_pmac_cfg_t *pmac_cfg, u32 value)
{
	u8 hdr;

	hdr = (u8)value;
	pmac_cfg->ig_pmac.def_pmac_hdr[7] = hdr;
	pmac_cfg->ig_pmac_flags = IG_PMAC_F_PMACHDR8;
	return 0;
}

static int egress_fcs_set(dp_pmac_cfg_t *pmac_cfg, u32 value)
{
	pmac_cfg->eg_pmac.fcs = value;
	pmac_cfg->eg_pmac_flags = EG_PMAC_F_FCS;
	return 0;
}

static int egress_l2hdr_bytes_set(dp_pmac_cfg_t *pmac_cfg, u32 value)
{
	pmac_cfg->eg_pmac.num_l2hdr_bytes_rm = value;
	pmac_cfg->eg_pmac_flags = EG_PMAC_F_L2HDR_RM;
	return 0;
}

static int egress_rx_dma_set(dp_pmac_cfg_t *pmac_cfg, u32 value)
{
	pmac_cfg->eg_pmac.rx_dma_chan = value;
	pmac_cfg->eg_pmac_flags = EG_PMAC_F_RXID;
	return 0;
}

static int egress_pmac_set(dp_pmac_cfg_t *pmac_cfg, u32 value)
{
	pmac_cfg->eg_pmac.pmac = value;
	pmac_cfg->eg_pmac_flags = EG_PMAC_F_PMAC;
	return 0;
}

static int egress_res_dw_set(dp_pmac_cfg_t *pmac_cfg, u32 value)
{
	pmac_cfg->eg_pmac.res_dw1 = value;
	pmac_cfg->eg_pmac_flags = EG_PMAC_F_RESDW1;
	return 0;
}

static int egress_res1_dw_set(dp_pmac_cfg_t *pmac_cfg, u32 value)
{
	pmac_cfg->eg_pmac.res1_dw0 = value;
	pmac_cfg->eg_pmac_flags = EG_PMAC_F_RES1DW0;
	return 0;
}

static int egress_res2_dw_set(dp_pmac_cfg_t *pmac_cfg, u32 value)
{
	pmac_cfg->eg_pmac.res2_dw0 = value;
	pmac_cfg->eg_pmac_flags = EG_PMAC_F_RES2DW0;
	return 0;
}

static int egress_tc_ena_set(dp_pmac_cfg_t *pmac_cfg, u32 value)
{
	pmac_cfg->eg_pmac.tc_enable = value;
	pmac_cfg->eg_pmac_flags = EG_PMAC_F_TCENA;
	return 0;
}

static int egress_dec_flag_set(dp_pmac_cfg_t *pmac_cfg, u32 value)
{
	pmac_cfg->eg_pmac.dec_flag = value;
	pmac_cfg->eg_pmac_flags = EG_PMAC_F_DECFLG;
	return 0;
}

static int egress_enc_flag_set(dp_pmac_cfg_t *pmac_cfg, u32 value)
{
	pmac_cfg->eg_pmac.enc_flag = value;
	pmac_cfg->eg_pmac_flags = EG_PMAC_F_ENCFLG;
	return 0;
}

static int egress_mpe1_flag_set(dp_pmac_cfg_t *pmac_cfg, u32 value)
{
	pmac_cfg->eg_pmac.mpe1_flag = value;
	pmac_cfg->eg_pmac_flags = EG_PMAC_F_MPE1FLG;
	return 0;
}

static int egress_mpe2_flag_set(dp_pmac_cfg_t *pmac_cfg, u32 value)
{
	pmac_cfg->eg_pmac.mpe2_flag = value;
	pmac_cfg->eg_pmac_flags = EG_PMAC_F_MPE2FLG;
	return 0;
}

static struct ingress_pmac_entry ingress_entries[] = {
	{"errdisc", ingress_err_disc_set},
	{"pmac", ingress_pmac_set},
	{"pmac_pmap", ingress_pmac_pmap_set},
	{"pmac_en_pmap", ingress_pmac_en_pmap_set},
	{"pmac_tc", ingress_pmac_tc_set},
	{"pmac_en_tc", ingress_pmac_en_tc_set},
	{"pmac_subifid", ingress_pmac_subifid_set},
	{"pmac_srcport", ingress_pmac_srcport_set},
	{"pmac_hdr1", ingress_pmac_hdr1_set},
	{"pmac_hdr2", ingress_pmac_hdr2_set},
	{"pmac_hdr3", ingress_pmac_hdr3_set},
	{"pmac_hdr4", ingress_pmac_hdr4_set},
	{"pmac_hdr5", ingress_pmac_hdr5_set},
	{"pmac_hdr6", ingress_pmac_hdr6_set},
	{"pmac_hdr7", ingress_pmac_hdr7_set},
	{"pmac_hdr8", ingress_pmac_hdr8_set},
	{NULL, NULL}
};

static struct egress_pmac_entry egress_entries[] = {
	{"rx_dmachan", egress_rx_dma_set},
	{"rm_l2hdr", egress_l2hdr_bytes_set},
	{"fcs", egress_fcs_set},
	{"pmac", egress_pmac_set},
	{"res_dw1", egress_res_dw_set},
	{"res1_dw0", egress_res1_dw_set},
	{"res2_dw0", egress_res2_dw_set},
	{"tc_enable", egress_tc_ena_set},
	{"dec_flag", egress_dec_flag_set},
	{"enc_flag", egress_enc_flag_set},
	{"mpe1_flag", egress_mpe1_flag_set},
	{"mpe2_flag", egress_mpe2_flag_set},
	{NULL, NULL}
};

static int proc_gsw_port_rmon_dump(struct seq_file *s, int pos)
{
	int i;
	int ret = 0;
	struct core_ops *gsw_handle;
	struct rmon_ops *rmon;
	char flag_buf[20];

	if (!capable(CAP_NET_ADMIN))
		return -EPERM;
	if (pos == 0) {
		memset(gsw_r_rmon_mib, 0, sizeof(gsw_r_rmon_mib));
		memset(gsw_l_rmon_mib, 0, sizeof(gsw_l_rmon_mib));

		/*read gswip-r rmon counter */
		gsw_handle = dp_port_prop[0].ops[GSWIP_R];
		rmon = &gsw_handle->gsw_rmon_ops;
		for (i = 0; i < ARRAY_SIZE(gsw_r_rmon_mib); i++) {
			gsw_r_rmon_mib[i].nPortId = i;
			ret = rmon->RMON_Port_Get(gsw_handle,
						  &gsw_r_rmon_mib[i]);
			if (ret != GSW_statusOk) {
				pr_err("RMON_PORT_GET fail for Port %d\n", i);
				return -1;
			}
		}

		/*read pmac rmon redirect mib */
		memset(&gswr_rmon_redirect, 0, sizeof(gswr_rmon_redirect));
		ret = rmon->RMON_Redirect_Get(gsw_handle, &gswr_rmon_redirect);

		if (ret != GSW_statusOk) {
			pr_err("GSW_RMON_REDIRECT_GET fail for Port %d\n", i);
			return -1;
		}

		/*read gswip-l rmon counter */
		gsw_handle = dp_port_prop[0].ops[GSWIP_L];
		rmon = &gsw_handle->gsw_rmon_ops;
		for (i = 0; i < ARRAY_SIZE(gsw_l_rmon_mib); i++) {
			gsw_l_rmon_mib[i].nPortId = i;
			ret = rmon->RMON_Port_Get(gsw_handle,
						  &gsw_l_rmon_mib[i]);
			if (ret != GSW_statusOk) {
				pr_err("RMON_PORT_GET fail for Port %d\n", i);
				return -1;
			}
		}

		seq_printf(s, "%-24s %12u %12u %12u %12u %12u %12u %12u\n",
			   "GSWIP-L", 0, 1, 2, 3, 4, 5, 6);
		seq_printf(s, "%-24s %12u %12u %12u %12u %12u %12u %12u %12u\n",
			   "GSWIP-R(Fixed)", 0, 1, 2, 3, 4, 5, 6, 15);
		seq_printf(s, "%-24s %12u %12u %12u %12u %12u %12u %12u %12u\n",
			   "GSWIP-R(Dynamic)", 7, 8, 9, 10, 11, 12, 13, 14);
		print_dash_line(s);
	}
	if (pos == RX_GOOD_PKTS) {
		GSW_PORT_RMON_PRINT("RX_Good", nRxGoodPkts);
	} else if (pos == RX_FILTER_PKTS) {
		GSW_PORT_RMON_PRINT("RX_FILTER", nRxFilteredPkts);
	} else if (pos == RX_DROP_PKTS) {
		GSW_PORT_RMON_PRINT("RX_DROP", nRxDroppedPkts);
	} else if (pos == RX_OTHERS) {
		if (!rmon_display_port_full)
			goto NEXT;

		GSW_PORT_RMON_PRINT("RX_UNICAST", nRxUnicastPkts);
		GSW_PORT_RMON_PRINT("RX_BROADCAST", nRxBroadcastPkts);
		GSW_PORT_RMON_PRINT("RX_MULTICAST", nRxMulticastPkts);
		GSW_PORT_RMON_PRINT("RX_FCS_ERR", nRxFCSErrorPkts);
		GSW_PORT_RMON_PRINT("RX_UNDER_GOOD",
				    nRxUnderSizeGoodPkts);
		GSW_PORT_RMON_PRINT("RX_OVER_GOOD", nRxOversizeGoodPkts);
		GSW_PORT_RMON_PRINT("RX_UNDER_ERR",
				    nRxUnderSizeErrorPkts);
		GSW_PORT_RMON_PRINT("RX_OVER_ERR", nRxOversizeErrorPkts);
		GSW_PORT_RMON_PRINT("RX_ALIGN_ERR", nRxAlignErrorPkts);
		GSW_PORT_RMON_PRINT("RX_64B", nRx64BytePkts);
		GSW_PORT_RMON_PRINT("RX_127B", nRx127BytePkts);
		GSW_PORT_RMON_PRINT("RX_255B", nRx255BytePkts);
		GSW_PORT_RMON_PRINT("RX_511B", nRx511BytePkts);
		GSW_PORT_RMON_PRINT("RX_1023B", nRx1023BytePkts);
		GSW_PORT_RMON_PRINT("RX_MAXB", nRxMaxBytePkts);
		GSW_PORT_RMON64_PRINT("RX_BAD_b", nRxBadBytes);
	} else if (pos == TX_GOOD_PKTS) {
		GSW_PORT_RMON_PRINT("TX_Good", nTxGoodPkts);
	} else if (pos == TX_ACM_PKTS) {
		GSW_PORT_RMON_PRINT("TX_ACM_DROP", nTxAcmDroppedPkts);
	} else if (pos == TX_DROP_PKTS) {
		GSW_PORT_RMON_PRINT("TX_Drop", nTxDroppedPkts);
	} else if (pos == TX_OTHERS) {
		if (!rmon_display_port_full)
			goto NEXT;

		GSW_PORT_RMON_PRINT("TX_UNICAST", nTxUnicastPkts);
		GSW_PORT_RMON_PRINT("TX_BROADAST", nTxBroadcastPkts);
		GSW_PORT_RMON_PRINT("TX_MULTICAST", nTxMulticastPkts);
		GSW_PORT_RMON_PRINT("TX_SINGLE_COLL",
				    nTxSingleCollCount);
		GSW_PORT_RMON_PRINT("TX_MULT_COLL", nTxMultCollCount);
		GSW_PORT_RMON_PRINT("TX_LATE_COLL", nTxLateCollCount);
		GSW_PORT_RMON_PRINT("TX_EXCESS_COLL",
				    nTxExcessCollCount);
		GSW_PORT_RMON_PRINT("TX_COLL", nTxCollCount);
		GSW_PORT_RMON_PRINT("TX_PAUSET", nTxPauseCount);
		GSW_PORT_RMON_PRINT("TX_64B", nTx64BytePkts);
		GSW_PORT_RMON_PRINT("TX_127B", nTx127BytePkts);
		GSW_PORT_RMON_PRINT("TX_255B", nTx255BytePkts);
		GSW_PORT_RMON_PRINT("TX_511B", nTx511BytePkts);
		GSW_PORT_RMON_PRINT("TX_1023B", nTx1023BytePkts);
		GSW_PORT_RMON_PRINT("TX_MAX_B", nTxMaxBytePkts);
		GSW_PORT_RMON_PRINT("TX_UNICAST", nTxUnicastPkts);
		GSW_PORT_RMON_PRINT("TX_UNICAST", nTxUnicastPkts);
		GSW_PORT_RMON_PRINT("TX_UNICAST", nTxUnicastPkts);
		GSW_PORT_RMON64_PRINT("TX_GOOD_b", nTxGoodBytes);

	} else if (pos == REDIRECT_MIB) {
		/*GSWIP-R PMAC Redirect conter */
		seq_printf(s, "%-24s %12s %12s %12s %12s\n",
			   "GSW-R Redirect", "Rx_Pkts", "Tx_Pkts",
			   "Rx_DropsPkts", "Tx_DropsPkts");

		seq_printf(s, "%-24s %12d %12d %12d %12d\n", "",
			   gswr_rmon_redirect.nRxPktsCount,
			   gswr_rmon_redirect.nTxPktsCount,
			   gswr_rmon_redirect.nRxDiscPktsCount,
			   gswr_rmon_redirect.nTxDiscPktsCount);
		print_dash_line(s);
	} else if (pos == DP_DRV_MIB) {
		u64 eth0_rx = 0, eth0_tx = 0;
		u64 eth1_rx = 0, eth1_tx = 0;
		u64 dsl_rx = 0, dsl_tx = 0;
		u64 other_rx = 0, other_tx = 0;
		int i, j;
		struct pmac_port_info *port;
		struct dev_mib *mib;

		for (i = 1; i < PMAC_MAX_NUM; i++) {
			port = get_dp_port_info(tmp_inst, i);

			if (!port)
				continue;

			if (i < 6) {
				for (j = 0; j < 16; j++) {
					mib = get_dp_port_subif_mib(
						get_dp_port_subif(port, j));
					eth0_rx +=
					    STATS_GET(mib->rx_fn_rxif_pkt);
					eth0_rx +=
					    STATS_GET(mib->rx_fn_txif_pkt);
					eth0_tx +=
					    STATS_GET(mib->tx_cbm_pkt);
					eth0_tx +=
					    STATS_GET(mib->tx_tso_pkt);
				}
			} else if (i == 15) {
				for (j = 0; j < 16; j++) {
					mib = get_dp_port_subif_mib(
						get_dp_port_subif(port, j));
					eth1_rx +=
					    STATS_GET(mib->rx_fn_rxif_pkt);
					eth1_rx +=
					    STATS_GET(mib->rx_fn_txif_pkt);
					eth1_tx +=
					    STATS_GET(mib->tx_cbm_pkt);
					eth1_tx +=
					    STATS_GET(mib->tx_tso_pkt);
				}
			} else if (port->alloc_flags & DP_F_FAST_DSL) {
				for (j = 0; j < 16; j++) {
					mib = get_dp_port_subif_mib(
						get_dp_port_subif(port, j));
					dsl_rx +=
					    STATS_GET(mib->rx_fn_rxif_pkt);
					dsl_rx +=
					    STATS_GET(mib->rx_fn_txif_pkt);
					dsl_tx +=
					    STATS_GET(mib->tx_cbm_pkt);
					dsl_tx +=
					    STATS_GET(mib->tx_tso_pkt);
				}
			} else {
				for (j = 0; j < 16; j++) {
					mib = get_dp_port_subif_mib(
						get_dp_port_subif(port, j));
					other_rx +=
					    STATS_GET(mib->rx_fn_rxif_pkt);
					other_rx +=
					    STATS_GET(mib->rx_fn_txif_pkt);
					other_tx +=
					    STATS_GET(mib->tx_cbm_pkt);
					other_tx +=
					    STATS_GET(mib->tx_tso_pkt);
				}
			}
		}

		seq_printf(s, "%-15s %22s %22s %22s %22s\n", "DP Drv Mib",
			   "ETH_LAN", "ETH_WAN", "DSL", "Others");

		seq_printf(s, "%15s %22llu %22llu %22llu %22llu\n",
			   "Rx_Pkts", eth0_rx, eth1_rx, dsl_rx, other_rx);
		seq_printf(s, "%15s %22llu %22llu %22llu %22llu\n",
			   "Tx_Pkts", eth0_tx, eth1_tx, dsl_tx, other_tx);
		print_dash_line(s);
	} else if ((pos >= RMON_MAX) &&
		   (pos < (RMON_MAX + EGRESS_QUEUE_ID_MAX))) {
		u32 qdc[4], enq_c, deq_c, index;
		u32 wq, qrth, qocc, qavg;
		struct tmu_equeue_link equeue_link;
		char *flag_s;

		if (!rmon_display_tmu_mib)
			goto NEXT;

		index = pos - RMON_MAX;
		enq_c = get_enq_counter(index);
		deq_c = get_deq_counter(index);
		tmu_qdct_read(index, qdc);
		tmu_qoct_read(index, &wq, &qrth, &qocc, &qavg);
		tmu_equeue_link_get(index, &equeue_link);
		flag_s =
		    get_dma_flags_str(equeue_link.epn, flag_buf,
				      sizeof(flag_buf));

		if ((enq_c || deq_c || (qdc[0] + qdc[1] + qdc[2] + qdc[3])) ||
		    qocc || qavg) {
			if (!f_q_mib_title_proc) {
				seq_printf(s, "%-15s %10s %10s %10s (%10s %10s %10s %10s) %10s %10s %10s\n",
					   "TMU MIB     QID", "enq",
					   "deq", "drop", "No-Color",
					   "Green", "Yellow", "Red",
					   "qocc", "qavg", "  DMA  Flag");
				f_q_mib_title_proc = 1;
			}

			seq_printf(s, "%15d %10u %10u %10u (%10u %10u %10u %10u) %10u %10u %10s\n",
				   index, enq_c, deq_c,
				   (qdc[0] + qdc[1] + qdc[2] + qdc[3]),
				   qdc[0], qdc[1], qdc[2], qdc[3], qocc,
				   (qavg >> 8), flag_s ? flag_s : "");

		} else {
			goto NEXT;
		}
	} else {
		goto NEXT;
	}
NEXT:
	pos++;

	if (pos - RMON_MAX + 1 >= EGRESS_QUEUE_ID_MAX)
		return -1;

	return pos;
}

static int proc_gsw_rmon_port_start(void)
{
	f_q_mib_title_proc = 0;
	tmp_inst = 0;
	return 0;
}

static ssize_t proc_gsw_rmon_write(struct file *file, const char *buf,
				   size_t count, loff_t *ppos)
{
	int len;
	char str[64];
	int num;
	char *param_list[10];

	if (!capable(CAP_NET_ADMIN))
		return -EPERM;
	len = (sizeof(str) > count) ? count : sizeof(str) - 1;
	len -= copy_from_user(str, buf, len);
	str[len] = 0;
	num = dp_split_buffer(str, param_list, ARRAY_SIZE(param_list));

	if (num < 1)
		goto help;

	if (dp_strncmpi(param_list[0], "clear", strlen("clear")) == 0 ||
	    dp_strncmpi(param_list[0], "c", 1) == 0 ||
	    dp_strncmpi(param_list[0], "rest", strlen("rest")) == 0 ||
	    dp_strncmpi(param_list[0], "r", 1) == 0) {
		dp_sys_mib_reset_30(0);
		goto EXIT_OK;
	}

	if (dp_strncmpi(param_list[0], "TMU", strlen("TMU")) == 0) {
		if (dp_strncmpi(param_list[1], "on", 2) == 0) {
			rmon_display_tmu_mib = 1;
			goto EXIT_OK;
		} else if (dp_strncmpi(param_list[1], "off", 3) == 0) {
			rmon_display_tmu_mib = 0;
			goto EXIT_OK;
		}
	}

	if (dp_strncmpi(param_list[0], "RMON", strlen("RMON")) == 0) {
		if (dp_strncmpi(param_list[1], "Full", strlen("Full")) == 0) {
			rmon_display_port_full = 1;
			goto EXIT_OK;
		} else if (dp_strncmpi(param_list[1],
				     "Basic",
					 strlen("Basic"))
					 == 0) {
			rmon_display_port_full = 0;
			goto EXIT_OK;
		}
	}

	/*unknown command */
	goto help;

EXIT_OK:
	return count;

help:
	pr_info("usage: echo clear > %s\n", DEBUGFS_RMON);
	pr_info("usage: echo TMU on > %s\n", DEBUGFS_RMON);
	pr_info("usage: echo TMU off > %s\n", DEBUGFS_RMON);
	pr_info("usage: echo RMON Full > %s\n", DEBUGFS_RMON);
	pr_info("usage: echo RMON Basic > %s\n", DEBUGFS_RMON);
	return count;
}

static int proc_dport_dump(struct seq_file *s, int pos)
{
	int i;
	cbm_dq_port_res_t res;
	u32 flag = 0;

	if (!capable(CAP_NET_ADMIN))
		return -EPERM;
	memset(&res, 0, sizeof(cbm_dq_port_res_t));
	if (cbm_dequeue_port_resources_get(pos, &res, flag) == 0) {
		seq_printf(s, "Dequeue port=%d free_base=0x%x\n", pos,
			   (u32)res.cbm_buf_free_base);

		for (i = 0; i < res.num_deq_ports; i++) {
			seq_printf(s,
				   "%d:deq_port_base=0x%x num_desc=%d port = %d tx chan %d\n",
				   i, (u32)res.deq_info[i].cbm_dq_port_base,
				   res.deq_info[i].num_desc,
				   res.deq_info[i].port_no,
				   res.deq_info[i].dma_tx_chan);
		}

		kfree(res.deq_info);
	}

	pos++;

	if (pos >= PMAC_MAX_NUM)
		pos = -1;	/*end of the loop */

	return pos;
}

int proc_ep_dump(struct seq_file *s, int pos)
{
#if defined(NEW_CBM_API) && NEW_CBM_API
	u32 num;
	cbm_tmu_res_t *res = NULL;
	u32 flag = 0;
	int i;
	struct pmac_port_info *port = get_dp_port_info(0, pos);
#endif
	if (!capable(CAP_NET_ADMIN))
		return -EPERM;
#if defined(NEW_CBM_API) && NEW_CBM_API
	if (cbm_dp_port_resources_get
	    (&pos, &num, &res, port ? port->alloc_flags : flag) == 0) {
		for (i = 0; i < num; i++) {
			seq_printf(s, "ep=%d tmu_port=%d queue=%d sid=%d\n",
				   pos, res[i].tmu_port, res[i].tmu_q,
				   res[i].tmu_sched);
		}

		kfree(res);
	}
#endif
	pos++;

	if (pos >= PMAC_MAX_NUM)
		pos = -1;	/*end of the loop */

	return pos;
}

static void pmac_eg_cfg(char *param_list[], int num, dp_pmac_cfg_t *pmac_cfg)
{
	int i, j;
	u32 value;

	for (i = 2; i < num; i += 2) {
		for (j = 0; j < ARRAY_SIZE(egress_entries); j++) {
			if (dp_strncmpi(param_list[i],
					egress_entries[j].name,
					strlen(egress_entries[j].name)))
				continue;
			if (dp_strncmpi(egress_entries[j].name,
					"rm_l2hdr",
					strlen("rm_l2hdr")) == 0) {
				if (dp_atoi(param_list[i + 1]) > 0) {
					pmac_cfg->eg_pmac.rm_l2hdr = 1;
					value = dp_atoi(param_list[i + 1]);
					egress_entries[j].egress_callback(
							pmac_cfg, value);
					pr_info("egress pmac ep %s config ok\n",
						egress_entries[j].name);
					break;
				}
				pmac_cfg->eg_pmac.rm_l2hdr =
				    dp_atoi(param_list[i + 1]);
			} else {
				value = dp_atoi(param_list[i + 1]);
				egress_entries[j].egress_callback(pmac_cfg,
								value);
				pr_info("egress pmac ep %s configu ok\n",
					egress_entries[j].name);
				break;
			}
		}
	}
}

ssize_t ep_port_write(struct file *file, const char *buf, size_t count,
		      loff_t *ppos)
{
	int len;
	char str[64];
	int num, i, j, ret;
	u32 value;
	u32 port;
	char *param_list[10];
	dp_pmac_cfg_t pmac_cfg;
	int inst = 0;

	if (!capable(CAP_NET_ADMIN))
		return -EPERM;
	memset(&pmac_cfg, 0, sizeof(dp_pmac_cfg_t));
	len = (sizeof(str) > count) ? count : sizeof(str) - 1;
	len -= copy_from_user(str, buf, len);
	str[len] = 0;
	num = dp_split_buffer(str, param_list, ARRAY_SIZE(param_list));

	if (dp_strncmpi(param_list[0], "ingress", strlen("ingress")) == 0) {
		port = dp_atoi(param_list[1]);

		for (i = 2; i < num; i += 2) {
			for (j = 0; j < ARRAY_SIZE(ingress_entries); j++) {
				if (dp_strncmpi(param_list[i],
						ingress_entries[j].name,
						strlen(ingress_entries[j].name))
						== 0) {
					value = dp_atoi(param_list[i + 1]);
					ingress_entries[j].ingress_callback(
							&pmac_cfg, value);
					pr_info("ingress pmac ep %s configed\n",
						ingress_entries[j].name);
					break;
				}
			}
		}

		ret = dp_pmac_set_30(inst, port, &pmac_cfg);

		if (ret != 0) {
			pr_err("pmac set configuration failed\n");
			return -1;
		}
	} else if (dp_strncmpi(param_list[0], "egress", 6) == 0) {
		port = dp_atoi(param_list[1]);

		pmac_eg_cfg(param_list, num, &pmac_cfg);
		ret = dp_pmac_set_30(inst, port, &pmac_cfg);

		if (ret != 0) {
			pr_err("pmac set configuration failed\n");
			return -1;
		}
	} else {
		pr_info("wrong command\n");
		goto help;
	}

	return count;
help:
	pr_info("echo ingress/egress [ep_port] %s > %s\n",
		"['ingress/egress fields'] [value]", DEBUGFS_EP);
	pr_info("(eg) echo ingress 1 pmac 1 > %s\n", DEBUGFS_EP);
	pr_info("(eg) echo egress 1 rm_l2hdr 2 > %s\n", DEBUGFS_EP);
	pr_info("echo %s ['errdisc/pmac/pmac_pmap/pmac_en_pmap/pmac_tc",
		"ingress [ep_port] ");
	pr_info("                         %s > %s\n",
		"/pmac_en_tc/pmac_subifid/pmac_srcport'] [value]", DEBUGFS_EP);
	pr_info("echo  %s %s > %s\n", "egress [ep_port]",
		"['rx_dmachan/fcs/pmac/res_dw1/res1_dw0/res2_dw0] [value]",
		DEBUGFS_EP);
	pr_info("echo egress [ep_port] ['rm_l2hdr'] [value] > %s\n",
		DEBUGFS_EP);
	return count;
}

static struct dp_proc_entry dp_proc_entries[] = {
	/*name single_callback_t multi_callback_t/_start write_callback_t */
	{PROC_PARSER, proc_parser_read, NULL, NULL, proc_parser_write},
	{PROC_RMON_PORTS, NULL, proc_gsw_port_rmon_dump,
	 proc_gsw_rmon_port_start, proc_gsw_rmon_write},
#if IS_ENABLED(CONFIG_INTEL_DATAPATH_CPUFREQ)
	{PROC_COC, proc_coc_read_30, NULL, NULL, proc_coc_write_30},
#endif
	{PROC_EP, NULL, proc_ep_dump, NULL, ep_port_write},
	{PROC_DPORT, NULL, proc_dport_dump, NULL, NULL},
	{DP_PROC_CBMLOOKUP, NULL, lookup_dump30, lookup_start30,
	 proc_get_qid_via_index30},
	/*the last place holder */
	{NULL, NULL, NULL, NULL, NULL}
};

int dp_sub_proc_install_30(void)
{
	int i;

	if (!dp_proc_node) {
		pr_info("dp_sub_proc_install failed\n");
		return 0;
	}

	for (i = 0; i < ARRAY_SIZE(dp_proc_entries); i++)
		dp_proc_entry_create(dp_proc_node, &dp_proc_entries[i]);
	pr_info("dp_sub_proc_install ok\n");
	return 0;
}

