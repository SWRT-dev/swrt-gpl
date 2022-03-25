/*
 * Copyright (C) Intel Corporation
 * Author: Shao Guohua <guohua.shao@intel.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 */

#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/etherdevice.h>
#include <net/datapath_api.h>
#include "../datapath.h"
#include "datapath_misc.h"

#define BP_CFG(bp_cfg, _index, bflag, id) ({ \
	typeof(_index) (index) = (_index); \
	(bp_cfg)->bEgressSubMeteringEnable[(index)] = bflag; \
	(bp_cfg)->nEgressTrafficSubMeterId[(index)] = id; })

#define BR_CFG(br_cfg, _index, bflag, id) ({\
	typeof(_index) (index) = (_index); \
	(br_cfg)->bSubMeteringEnable[(index)] = bflag; \
	(br_cfg)->nTrafficSubMeterId[(index)] = id; })

#define METER_CIR(cir)  ((cir) / 1000)
#define METER_PIR(pir)  ((pir) / 1000)

struct ctp_assign {
	u32 flag; /*Datapath Device Flag */
	GSW_LogicalPortMode_t emode; /*mapped GSWIP CTP flag */
	u16 num; /*Max CTP allowed for that GSWIP logical port*/
	u16 num_subif; /*Max allowed number of subif for that logical port*/
	u32 vap_offset; /*VAP offset */
	u32 vap_mask;  /*VAP Mask after shift vap_offset bits */
	u32 lookup_mode; /*CQE lookup mode  */
	u16 swdev_enable; /* To enable or disable switchdev feature */
};

static struct ctp_assign ctp_assign_info[] = {
	/*note: multiple flags must put first */
	{DP_F_CPU, GSW_LOGICAL_PORT_8BIT_WLAN, 16, 16, 8, 0xF, CQE_LU_MODE0, 8},
	{DP_F_GPON, GSW_LOGICAL_PORT_GPON, 256, 256, 0, 0xFF, CQE_LU_MODE1, 1},
	{DP_F_EPON, GSW_LOGICAL_PORT_EPON, 64, 64, 0, 0xFF, CQE_LU_MODE1, 1},
	{DP_F_GINT, GSW_LOGICAL_PORT_GINT, 16, 16, 0, 0xFF, CQE_LU_MODE1, 1},
	{DP_F_FAST_ETH_WAN, GSW_LOGICAL_PORT_8BIT_WLAN, 8, 8, 8, 0xF,
		CQE_LU_MODE2, 1},
	{DP_F_FAST_ETH_LAN | DP_F_ALLOC_EXPLICIT_SUBIFID,
		GSW_LOGICAL_PORT_8BIT_WLAN, 8, 8, 8, 0xF, CQE_LU_MODE2, 1},
	{DP_F_FAST_ETH_LAN, GSW_LOGICAL_PORT_8BIT_WLAN, 4, 4, 8, 0xF,
		CQE_LU_MODE2, 1},
	{DP_F_VUNI, GSW_LOGICAL_PORT_8BIT_WLAN, 2, 2, 8, 0xF, CQE_LU_MODE2, 1},
	{DP_F_FAST_WLAN, GSW_LOGICAL_PORT_8BIT_WLAN, 16, 16, 8, 0xF,
		CQE_LU_MODE2, 1},
	{DP_F_FAST_DSL, GSW_LOGICAL_PORT_8BIT_WLAN, 1, 16, 8, 0xF,
		CQE_LU_MODE0, 0},
	{DP_F_FAST_WLAN_EXT, GSW_LOGICAL_PORT_9BIT_WLAN, 8, 8, 9, 0x7,
		CQE_LU_MODE2, 1}
};

static struct ctp_assign ctp_assign_def = {0, GSW_LOGICAL_PORT_8BIT_WLAN, 8, 8,
					   0xF, CQE_LU_MODE2, 1};

static struct gsw_itf itf_assign[PMAC_MAX_NUM] = {0};

static char *ctp_mode_string(GSW_LogicalPortMode_t type)
{
	if (type == GSW_LOGICAL_PORT_8BIT_WLAN)
		return "8BIT_WLAN";
	if (type == GSW_LOGICAL_PORT_9BIT_WLAN)
		return "9BIT_WLAN";
	if (type == GSW_LOGICAL_PORT_GPON)
		return "GPON";
	if (type == GSW_LOGICAL_PORT_EPON)
		return "EPON";
	if (type == GSW_LOGICAL_PORT_GINT)
		return "GINT";
	if (type == GSW_LOGICAL_PORT_OTHER)
		return "OTHER";
	return "Undef";
}

/*This API is only for GSWIP-R PMAC modification, not for GSWIP-L */
int dp_pmac_set_31(int inst, u32 port, dp_pmac_cfg_t *pmac_cfg)
{
	GSW_PMAC_Eg_Cfg_t egcfg;
	GSW_PMAC_Ig_Cfg_t igcfg;
	int i, j, k;
	u32 flag = 0;
	cbm_dq_port_res_t dqport;
	s32 ret;
	u32 pmacid;
	struct core_ops *gswr_r;

	if (!pmac_cfg || !port) {
		pr_err("dp_pmac_set:wrong parameter(pmac_cfg/port NULL)\n");
		return -1;
	}

	if (!pmac_cfg->ig_pmac_flags && !pmac_cfg->eg_pmac_flags)
		return 0;
	if (port == 2)
		pmacid = 1;
	else
		pmacid = 0;
	memset(&dqport, 0, sizeof(cbm_dq_port_res_t));

	/* Get GSWIP device handler */
	gswr_r = dp_port_prop[inst].ops[0];

	/*set ingress port via DMA tx channel */
	if (pmac_cfg->ig_pmac_flags) {
		/*Read back igcfg from gsw first */
		ret = cbm_dequeue_port_resources_get(port, &dqport, flag);

		if (ret == -1) {
			pr_err("cbm_dequeue_port_resources_get failed\n");
			return -1;
		}

		memset(&igcfg, 0, sizeof(GSW_PMAC_Ig_Cfg_t));

		for (i = 0; i < dqport.num_deq_ports; i++) {
			igcfg.nPmacId = pmacid;
			igcfg.nTxDmaChanId = dqport.deq_info[i].dma_tx_chan;
			if (igcfg.nTxDmaChanId == (u8)-1) {
				igcfg.nTxDmaChanId =
					pmac_cfg->ig_pmac.tx_dma_chan;
			}
			gswr_r->gsw_pmac_ops.Pmac_Ig_CfgGet(gswr_r, &igcfg);

			/*update igcfg and write back to gsw */
			if (pmac_cfg->ig_pmac_flags & IG_PMAC_F_ERR_DISC)
				igcfg.bErrPktsDisc =
					pmac_cfg->ig_pmac.err_disc;

			if (pmac_cfg->ig_pmac_flags & IG_PMAC_F_PRESENT)
				igcfg.bPmacPresent = pmac_cfg->ig_pmac.pmac;

			if (pmac_cfg->ig_pmac_flags & IG_PMAC_F_SUBIF)
				//igcfg.bSubIdDefault =
				igcfg.eSubId =
					pmac_cfg->ig_pmac.def_pmac_subifid;

			if (pmac_cfg->ig_pmac_flags & IG_PMAC_F_SPID)
				igcfg.bSpIdDefault =
					pmac_cfg->ig_pmac.def_pmac_src_port;

			if (pmac_cfg->ig_pmac_flags & IG_PMAC_F_CLASSENA)
				igcfg.bClassEna =
					pmac_cfg->ig_pmac.def_pmac_en_tc;

			if (pmac_cfg->ig_pmac_flags & IG_PMAC_F_CLASS)
				igcfg.bClassDefault =
					pmac_cfg->ig_pmac.def_pmac_tc;

			if (pmac_cfg->ig_pmac_flags & IG_PMAC_F_PMAPENA)
				igcfg.bPmapEna =
					pmac_cfg->ig_pmac.def_pmac_en_pmap;

			if (pmac_cfg->ig_pmac_flags & IG_PMAC_F_PMAP)
				igcfg.bPmapDefault =
					pmac_cfg->ig_pmac.def_pmac_pmap;

			if (pmac_cfg->ig_pmac_flags & IG_PMAC_F_PMACHDR1)
				igcfg.defPmacHdr[0] =
					pmac_cfg->ig_pmac.def_pmac_hdr[0];

			if (pmac_cfg->ig_pmac_flags & IG_PMAC_F_PMACHDR2)
				igcfg.defPmacHdr[1] =
					pmac_cfg->ig_pmac.def_pmac_hdr[1];

			if (pmac_cfg->ig_pmac_flags & IG_PMAC_F_PMACHDR3)
				igcfg.defPmacHdr[2] =
					pmac_cfg->ig_pmac.def_pmac_hdr[2];

			if (pmac_cfg->ig_pmac_flags & IG_PMAC_F_PMACHDR4)
				igcfg.defPmacHdr[3] =
					pmac_cfg->ig_pmac.def_pmac_hdr[3];

			if (pmac_cfg->ig_pmac_flags & IG_PMAC_F_PMACHDR5)
				igcfg.defPmacHdr[4] =
					pmac_cfg->ig_pmac.def_pmac_hdr[4];

			if (pmac_cfg->ig_pmac_flags & IG_PMAC_F_PMACHDR6)
				igcfg.defPmacHdr[5] =
					pmac_cfg->ig_pmac.def_pmac_hdr[5];

			if (pmac_cfg->ig_pmac_flags & IG_PMAC_F_PMACHDR7)
				igcfg.defPmacHdr[6] =
					pmac_cfg->ig_pmac.def_pmac_hdr[6];

			if (pmac_cfg->ig_pmac_flags & IG_PMAC_F_PMACHDR8)
				igcfg.defPmacHdr[7] =
					pmac_cfg->ig_pmac.def_pmac_hdr[7];

			DP_DEBUG(DP_DBG_FLAG_DBG,
				 "\nPMAC %d igcfg configuration:\n", port);
			DP_DEBUG(DP_DBG_FLAG_DBG, "igcfg.nPmacId=%d\n",
				 igcfg.nPmacId);
			DP_DEBUG(DP_DBG_FLAG_DBG, "igcfg.nTxDmaChanId=%d\n",
				 igcfg.nTxDmaChanId);
			DP_DEBUG(DP_DBG_FLAG_DBG, "igcfg.bErrPktsDisc=%d\n",
				 igcfg.bErrPktsDisc);
			DP_DEBUG(DP_DBG_FLAG_DBG, "igcfg.bPmapDefault=%d\n",
				 igcfg.bPmapDefault);
			DP_DEBUG(DP_DBG_FLAG_DBG, "igcfg.bPmapEna=%d\n",
				 igcfg.bPmapEna);
			DP_DEBUG(DP_DBG_FLAG_DBG, "igcfg.bClassDefault=%d\n",
				 igcfg.bClassDefault);
			DP_DEBUG(DP_DBG_FLAG_DBG, "igcfg.bClassEna=%d\n",
				 igcfg.bClassEna);
			DP_DEBUG(DP_DBG_FLAG_DBG, "igcfg.bSubIdDefault=%d\n",
				 igcfg.eSubId);
			DP_DEBUG(DP_DBG_FLAG_DBG, "igcfg.bSpIdDefault=%d\n",
				 igcfg.bSpIdDefault);
			DP_DEBUG(DP_DBG_FLAG_DBG, "igcfg.bPmacPresent=%d\n",
				 igcfg.bPmacPresent);
			DP_DEBUG(DP_DBG_FLAG_DBG, "igcfg.defPmacHdr=");

			for (k = 0;
					k <
					sizeof(igcfg.defPmacHdr) /
					sizeof(igcfg.defPmacHdr[0]); k++)
				DP_DEBUG(DP_DBG_FLAG_DBG, "0x%x ",
					 igcfg.defPmacHdr[k]);

			DP_DEBUG(DP_DBG_FLAG_DBG, "\n");

			gswr_r->gsw_pmac_ops.Pmac_Ig_CfgSet(gswr_r, &igcfg);
		}

			kfree(dqport.deq_info);
	}

	/*set egress port via pmac port id */
	if (!pmac_cfg->eg_pmac_flags)
		return 0;

	for (i = 0; i <= 15; i++) {	/*traffic class */
		for (j = 0; j <= 3; j++) {	/* flow */
			/*read back egcfg first from gsw */
			memset(&egcfg, 0, sizeof(GSW_PMAC_Eg_Cfg_t));
			egcfg.nPmacId = pmacid;
			egcfg.nDestPortId = port;
			egcfg.nTrafficClass = i;
			egcfg.nFlowIDMsb = j;
			egcfg.nBslTrafficClass = i;
			egcfg.bProcFlagsSelect = 1;
			DP_DEBUG(DP_DBG_FLAG_DBG, "bProcFlagsSelect=%u\n",
				 egcfg.bProcFlagsSelect);
			gswr_r->gsw_pmac_ops.Pmac_Eg_CfgGet(gswr_r, &egcfg);

			/*update egcfg and write back to gsw */
			if (pmac_cfg->eg_pmac_flags & EG_PMAC_F_FCS)
				egcfg.bFcsEna = pmac_cfg->eg_pmac.fcs;

			if (pmac_cfg->eg_pmac_flags & EG_PMAC_F_L2HDR_RM) {
				egcfg.bRemL2Hdr = pmac_cfg->eg_pmac.rm_l2hdr;
				egcfg.numBytesRem =
				    pmac_cfg->eg_pmac.num_l2hdr_bytes_rm;
			}

			if (pmac_cfg->eg_pmac_flags & EG_PMAC_F_PMAC)
				egcfg.bPmacEna = pmac_cfg->eg_pmac.pmac;

			if (pmac_cfg->eg_pmac_flags & EG_PMAC_F_RXID)
				egcfg.nRxDmaChanId =
				    pmac_cfg->eg_pmac.rx_dma_chan;

			if (pmac_cfg->eg_pmac_flags & EG_PMAC_F_REDIREN)
				egcfg.bRedirEnable = pmac_cfg->eg_pmac.redir;

			if (pmac_cfg->eg_pmac_flags & EG_PMAC_F_BSLSEG)
				egcfg.bBslSegmentDisable =
					pmac_cfg->eg_pmac.bsl_seg;

			if (pmac_cfg->eg_pmac_flags & EG_PMAC_F_RESDW1EN)
				egcfg.bResDW1Enable =
					pmac_cfg->eg_pmac.res_endw1;

			if (pmac_cfg->eg_pmac_flags & EG_PMAC_F_RESDW1)
				egcfg.nResDW1 = pmac_cfg->eg_pmac.res_dw1;

			if (pmac_cfg->eg_pmac_flags & EG_PMAC_F_RES1DW0EN)
				egcfg.bRes1DW0Enable =
					pmac_cfg->eg_pmac.res1_endw0;

			if (pmac_cfg->eg_pmac_flags & EG_PMAC_F_RES1DW0)
				egcfg.nRes1DW0 = pmac_cfg->eg_pmac.res1_dw0;

			if (pmac_cfg->eg_pmac_flags & EG_PMAC_F_RES2DW0EN)
				egcfg.bRes2DW0Enable =
					pmac_cfg->eg_pmac.res2_endw0;

			if (pmac_cfg->eg_pmac_flags & EG_PMAC_F_RES2DW0)
				egcfg.nRes2DW0 = pmac_cfg->eg_pmac.res2_dw0;

			/*if (pmac_cfg->eg_pmac_flags & EG_PMAC_F_TCENA)
			 *    egcfg.bTCEnable = pmac_cfg->eg_pmac.tc_enable;
			 */

			if (pmac_cfg->eg_pmac_flags & EG_PMAC_F_DECFLG)
				egcfg.bDecFlag = pmac_cfg->eg_pmac.dec_flag;

			if (pmac_cfg->eg_pmac_flags & EG_PMAC_F_ENCFLG)
				egcfg.bEncFlag = pmac_cfg->eg_pmac.enc_flag;

			if (pmac_cfg->eg_pmac_flags & EG_PMAC_F_MPE1FLG)
				egcfg.bMpe1Flag = pmac_cfg->eg_pmac.mpe1_flag;

			if (pmac_cfg->eg_pmac_flags & EG_PMAC_F_MPE2FLG)
				egcfg.bMpe2Flag = pmac_cfg->eg_pmac.mpe2_flag;
#if defined(CONFIG_INTEL_DATAPATH_DBG) && CONFIG_INTEL_DATAPATH_DBG
			if (dp_dbg_flag) {
				DP_DEBUG(DP_DBG_FLAG_DBG,
					 "\nPMAC %d egcfg configuration:\n",
					 port);
				DP_DEBUG(DP_DBG_FLAG_DBG,
					 "egcfg.nPmacId=%d\n",
					 egcfg.nPmacId);
				DP_DEBUG(DP_DBG_FLAG_DBG,
					 "egcfg.nRxDmaChanId=%d\n",
					 egcfg.nRxDmaChanId);
				DP_DEBUG(DP_DBG_FLAG_DBG,
					 "egcfg.bRemL2Hdr=%d\n",
					 egcfg.bRemL2Hdr);
				DP_DEBUG(DP_DBG_FLAG_DBG,
					 "egcfg.numBytesRem=%d\n",
					 egcfg.numBytesRem);
				DP_DEBUG(DP_DBG_FLAG_DBG,
					 "egcfg.bFcsEna=%d\n", egcfg.bFcsEna);
				DP_DEBUG(DP_DBG_FLAG_DBG,
					 "egcfg.bPmacEna=%d\n",
					 egcfg.bPmacEna);
				DP_DEBUG(DP_DBG_FLAG_DBG,
					 "egcfg.bResDW1Ena=%d\n",
					 egcfg.bResDW1Enable);
				DP_DEBUG(DP_DBG_FLAG_DBG,
					 "egcfg.nResDW1=%d\n", egcfg.nResDW1);
				DP_DEBUG(DP_DBG_FLAG_DBG,
					 "egcfg.bRes1DW0Ena=%d\n",
					 egcfg.bRes1DW0Enable);
				DP_DEBUG(DP_DBG_FLAG_DBG,
					 "egcfg.nRes1DW0=%d\n",
					 egcfg.nRes1DW0);
				DP_DEBUG(DP_DBG_FLAG_DBG,
					 "egcfg.bRes2DW0Ena=%d\n",
					 egcfg.bRes2DW0Enable);
				DP_DEBUG(DP_DBG_FLAG_DBG,
					 "egcfg.nRes2DW0=%d\n",
					 egcfg.nRes2DW0);
				DP_DEBUG(DP_DBG_FLAG_DBG,
					 "egcfg.nDestPortId=%d\n",
					 egcfg.nDestPortId);
				DP_DEBUG(DP_DBG_FLAG_DBG,
					 "egcfg.bTCEnable=%d\n",
					 egcfg.bTCEnable);
				DP_DEBUG(DP_DBG_FLAG_DBG,
					 "egcfg.nTrafficClass=%d\n",
					 egcfg.nTrafficClass);
				DP_DEBUG(DP_DBG_FLAG_DBG,
					 "egcfg.nBslTrafficClass=%d\n",
					 egcfg.nBslTrafficClass);
				DP_DEBUG(DP_DBG_FLAG_DBG,
					 "egcfg.nFlowIDMsb=%d\n",
					 egcfg.nFlowIDMsb);
				DP_DEBUG(DP_DBG_FLAG_DBG,
					 "egcfg.bDecFlag=%d\n",
					 egcfg.bDecFlag);
				DP_DEBUG(DP_DBG_FLAG_DBG,
					 "egcfg.bEncFlag=%d\n",
					 egcfg.bEncFlag);
				DP_DEBUG(DP_DBG_FLAG_DBG,
					 "egcfg.bMpe1Flag=%d\n",
					 egcfg.bMpe1Flag);
				DP_DEBUG(DP_DBG_FLAG_DBG,
					 "egcfg.bMpe2Flag=%d\n",
					 egcfg.bMpe2Flag);
			}
#endif
			gswr_r->gsw_pmac_ops.Pmac_Eg_CfgSet(gswr_r, &egcfg);
		}
	}

	return 0;
}

/*flag: bit 0 for cpu
 * bit 1 for mpe1,bit 2 for mpe2, bit 3 for mpe3;
 */
#define GSW_L_BASE_ADDR        (0xBC000000)
#define GSW_R_BASE_ADDR        (0xBA000000)
#define FDMA_PASR_ADDR         (0xA47)

int dp_set_gsw_parser_31(u8 flag, u8 cpu, u8 mpe1,
			 u8 mpe2, u8 mpe3)
{
	GSW_CPU_PortCfg_t param = {0};
	GSW_register_t reg;
	struct core_ops *gsw_handle = dp_port_prop[0].ops[0];/*gswip o */

	if (gsw_handle->gsw_common_ops.CPU_PortCfgGet(gsw_handle, &param)) {
		pr_err("Failed GSW_CPU_PORT_CFG_GET\n");
		return -1;
	}
	DP_DEBUG(DP_DBG_FLAG_DBG, "old flag=0x%x cpu=%d mpe1/2/3=%d/%d/%d\n",
		 flag, param.eNoMPEParserCfg,
		 param.eMPE1ParserCfg, param.eMPE2ParserCfg,
		 param.eMPE1MPE2ParserCfg);
	DP_DEBUG(DP_DBG_FLAG_DBG, "new flag=0x%x cpu=%d mpe1/2/3=%d/%d/%d\n",
		 flag, cpu, mpe1, mpe2, mpe3);
	if (flag & F_MPE_NONE)
		param.eNoMPEParserCfg = cpu;

	if (flag & F_MPE1_ONLY)
		param.eMPE1ParserCfg = mpe1;

	if (flag & F_MPE2_ONLY)
		param.eMPE2ParserCfg = mpe2;

	if (flag & F_MPE1_MPE2)
		param.eMPE1MPE2ParserCfg = mpe3;

	if (gsw_handle->gsw_common_ops.CPU_PortCfgSet(gsw_handle, &param)) {
		pr_err("Failed GSW_CPU_PORT_CFG_SET\n");
		return -1;
	}
	memset(&reg, 0, sizeof(reg));
	reg.nRegAddr = 0x001; /**/
	gsw_handle->gsw_common_ops.RegisterGet(gsw_handle, &reg);
	reg.nData |= 1 << 0; /**/
	gsw_handle->gsw_common_ops.RegisterSet(gsw_handle, &reg);

	dp_parser_info_refresh(param.eNoMPEParserCfg,
			       param.eMPE1ParserCfg,
			       param.eMPE2ParserCfg,
			       param.eMPE1MPE2ParserCfg, 0);
	return 0;
}

int dp_get_gsw_parser_31(u8 *cpu, u8 *mpe1, u8 *mpe2,
			 u8 *mpe3)
{
	GSW_CPU_PortCfg_t param = {0};
	struct core_ops *gsw_handle = dp_port_prop[0].ops[0]; /*gswip 0*/

	if (gsw_handle->gsw_common_ops.CPU_PortCfgGet(gsw_handle, &param)) {
		pr_err("Failed GSW_CPU_PORT_CFG_GET\n");
		return -1;
	}
	dp_parser_info_refresh(param.eNoMPEParserCfg,
			       param.eMPE1ParserCfg,
			       param.eMPE2ParserCfg,
			       param.eMPE1MPE2ParserCfg, 1);

	if (cpu) {
		*cpu = param.eNoMPEParserCfg;
		DP_DEBUG(DP_DBG_FLAG_DBG, "  cpu=%d\n", *cpu);
	}

	if (mpe1) {
		*mpe1 = param.eMPE1ParserCfg;
		DP_DEBUG(DP_DBG_FLAG_DBG, "  mpe1=%d\n", *mpe1);
	}

	if (mpe2) {
		*mpe2 = param.eMPE2ParserCfg;
		DP_DEBUG(DP_DBG_FLAG_DBG, "  mpe2=%d\n", *mpe2);
	}

	if (mpe3) {
		*mpe3 = param.eMPE1MPE2ParserCfg;
		DP_DEBUG(DP_DBG_FLAG_DBG, "  mpe3=%d\n", *mpe3);
	}
	return 0;
}

int gsw_mib_reset_31(int dev, u32 flag)
{
	struct core_ops *gsw_handle;
	GSW_return_t ret;
	GSW_RMON_clear_t rmon_clear;

	gsw_handle = dp_port_prop[0].ops[0];
	rmon_clear.eRmonType = GSW_RMON_ALL_TYPE;
	ret = gsw_handle->gsw_rmon_ops.RMON_Clear(gsw_handle, &rmon_clear);

	if (ret != GSW_statusOk) {
		pr_err("R:GSW_RMON_CLEAR failed for GSW_RMON_ALL_TYPE\n");
		return -1;
	}
	return ret;
}

/* Return allocated ctp number */
struct gsw_itf *ctp_port_assign(int inst, u8 ep, int bp_default,
				u32 flags, struct dp_dev_data *data)
{
	GSW_CTP_portAssignment_t ctp_assign;
	struct ctp_assign *assign = &ctp_assign_def;
	int i, alloc_flag;
	u16 num;
	struct core_ops *gsw_handle;
	struct ctp_ops *gsw_ctp;
	struct pmac_port_info *port_info = get_dp_port_info(inst, ep);

	memset(&ctp_assign, 0, sizeof(ctp_assign));
	gsw_handle = dp_port_prop[inst].ops[GSWIP_L];
	gsw_ctp = &gsw_handle->gsw_ctp_ops;
	alloc_flag = port_info->alloc_flags;

	if (flags & DP_F_DEREGISTER) {
		pr_err("Need to Free CTP Port here for ep=%d\n", ep);
		ctp_assign.nLogicalPortId = ep;
		ctp_assign.eMode = itf_assign[ep].mode;
		ctp_assign.nFirstCtpPortId = itf_assign[ep].start;
		ctp_assign.nNumberOfCtpPort = itf_assign[ep].n;
		if (gsw_ctp->CTP_PortAssignmentFree(gsw_handle, &ctp_assign)) {
			pr_err("Failed to allc CTP for ep=%d blk=%d mode=%d\n",
			       ep, assign->num, assign->emode);
			return NULL;
		}
		return NULL;
	}

	for (i = 0; i < ARRAY_SIZE(ctp_assign_info); i++) {
		if ((ctp_assign_info[i].flag & alloc_flag) ==
			ctp_assign_info[i].flag) {
			assign = &ctp_assign_info[i];
			break;
		}
	}
	if (data->max_ctp)
		num = data->max_ctp;
	else
		num = assign->num;
	ctp_assign.nLogicalPortId = ep;
	ctp_assign.eMode = assign->emode;
	ctp_assign.nBridgePortId = bp_default;
	ctp_assign.nFirstCtpPortId = 0;
	ctp_assign.nNumberOfCtpPort = num;
	if (gsw_ctp->CTP_PortAssignmentAlloc(gsw_handle, &ctp_assign)) {
		pr_err("Failed CTP Assignment for ep=%d blk size=%d mode=%s\n",
		       ep, num, ctp_mode_string(assign->emode));
		return NULL;
	}

	DP_DEBUG(DP_DBG_FLAG_DBG, "assign ep=%d with eMode=%d ctp_max:%d\n",
		 ep, assign->emode, ctp_assign.nNumberOfCtpPort);
	itf_assign[ep].mode = assign->emode;
	itf_assign[ep].n = ctp_assign.nNumberOfCtpPort;
	itf_assign[ep].start = ctp_assign.nFirstCtpPortId;
	itf_assign[ep].end = ctp_assign.nFirstCtpPortId +
		 ctp_assign.nNumberOfCtpPort - 1;
	itf_assign[ep].ep = ep;
	port_info->subif_max = assign->num_subif;
	port_info->ctp_max = ctp_assign.nNumberOfCtpPort;
	port_info->vap_offset = assign->vap_offset;
	port_info->vap_mask = assign->vap_mask;
#if IS_ENABLED(CONFIG_INTEL_DATAPATH_SWITCHDEV)
	port_info->swdev_en = assign->swdev_enable;
#endif
	return &itf_assign[ep];
}

int set_port_lookup_mode_31(int inst, u8 ep, u32 flags)
{
	int i, alloc_flag;
	struct ctp_assign *assign = &ctp_assign_def;
	struct pmac_port_info *port_info = get_dp_port_info(inst, ep);

	alloc_flag = port_info->alloc_flags;
	for (i = 0; i < ARRAY_SIZE(ctp_assign_info); i++) {
		if ((ctp_assign_info[i].flag & alloc_flag) ==
			ctp_assign_info[i].flag) {
			assign = &ctp_assign_info[i];
			break;
		}
	}
	port_info->cqe_lu_mode = assign->lookup_mode;
	port_info->gsw_mode = (u32)assign->emode;
	return 0;
}

/*Allocate a bridge port with specified FID and hardcoded CPU port member */
int alloc_bridge_port(int inst, int port_id, int subif_ix,
		      int fid, int bp_member)
{
	GSW_BRIDGE_portAlloc_t bp = {0};
	GSW_BRIDGE_portConfig_t bp_cfg = {0};
	struct core_ops *gsw_handle;
	struct brdgport_ops *gsw_bp;
	GSW_return_t ret;

	gsw_handle = dp_port_prop[inst].ops[GSWIP_L];
	gsw_bp = &gsw_handle->gsw_brdgport_ops;
	/*allocate a free bridge port */
	memset(&bp, 0, sizeof(bp));
	ret = gsw_bp->BridgePort_Alloc(gsw_handle, &bp);
	if ((ret != GSW_statusOk) ||
	    (bp.nBridgePortId < 0)) {
		pr_err("Failed to get a bridge port\n");
		return -1;
	}
	/*set this new bridge port with specified bridge ID(fid)
	 *and bridge port map
	 */
	bp_cfg.nBridgePortId = bp.nBridgePortId;
	bp_cfg.nDestLogicalPortId = port_id;
	bp_cfg.nDestSubIfIdGroup = subif_ix;
	/* By default Disable src mac learning for registered
	 * non CPU bridge port with DP
	 */
	if (get_dp_port_subif(get_dp_port_info(inst, port_id),
			      subif_ix)->mac_learn_dis == DP_MAC_LEARNING_DIS)
		bp_cfg.bSrcMacLearningDisable = 1;
	else
		bp_cfg.bSrcMacLearningDisable = 0;
	bp_cfg.eMask = GSW_BRIDGE_PORT_CONFIG_MASK_BRIDGE_ID |
		GSW_BRIDGE_PORT_CONFIG_MASK_BRIDGE_PORT_MAP |
		GSW_BRIDGE_PORT_CONFIG_MASK_MC_SRC_MAC_LEARNING |
		GSW_BRIDGE_PORT_CONFIG_MASK_EGRESS_CTP_MAPPING;
	bp_cfg.nBridgeId = fid;

	SET_BP_MAP(bp_cfg.nBridgePortMap, bp_member); /*CPU*/
	ret = gsw_bp->BridgePort_ConfigSet(gsw_handle, &bp_cfg);
	if (ret != GSW_statusOk) {
		pr_err("Failed to set bridge id(%d) and port map for bp= %d\n",
		       fid, bp_cfg.nBridgePortId);
		gsw_bp->BridgePort_Free(gsw_handle, &bp);
		return -1;
	}

	/*ADD this bridge port to CPU bridge port's member.
	 *Need read back first
	 */
	bp_cfg.nBridgePortId = bp_member;
	bp_cfg.eMask = GSW_BRIDGE_PORT_CONFIG_MASK_BRIDGE_PORT_MAP;
	ret = gsw_bp->BridgePort_ConfigGet(gsw_handle, &bp_cfg);
	if (ret != GSW_statusOk) {
		pr_err("Failed to get bridge port's member for bridgeport=%d\n",
		       bp_cfg.nBridgePortId);
		gsw_bp->BridgePort_Free(gsw_handle, &bp);
		return -1;
	}
	SET_BP_MAP(bp_cfg.nBridgePortMap, bp.nBridgePortId);
	ret = gsw_bp->BridgePort_ConfigSet(gsw_handle, &bp_cfg);
	if (ret != GSW_statusOk) {
		pr_err("Failed to set bridge port's member for bridgeport=%d\n",
		       bp_cfg.nBridgePortId);
		gsw_bp->BridgePort_Free(gsw_handle, &bp);
		return -1;
	}

	return bp.nBridgePortId;
}

/*Free one GSWIP bridge port
 *First read out its port member
 *according to this port memeber, from this deleing bridge port
 *from its member's member Free this bridge port
 */
int free_bridge_port(int inst, int bp)
{
	GSW_BRIDGE_portConfig_t *tmp = NULL, *tmp2 = NULL;
	int i, j;
	GSW_return_t ret;
	struct core_ops *gsw_handle;
	struct brdgport_ops *gsw_bp;

	gsw_handle = dp_port_prop[inst].ops[GSWIP_L];
	gsw_bp = &gsw_handle->gsw_brdgport_ops;
	if (bp == CPU_BP)
		return 0;

	tmp = kzalloc(sizeof(*tmp), GFP_KERNEL);
	tmp2 = kzalloc(sizeof(*tmp2), GFP_KERNEL);
	if (!tmp || !tmp2)
		goto FREE_EXIT;

	/*read out this delting bridge port's member*/
	tmp->nBridgePortId = bp;
	tmp->eMask = GSW_BRIDGE_PORT_CONFIG_MASK_BRIDGE_PORT_MAP;
	ret = gsw_bp->BridgePort_ConfigGet(gsw_handle, tmp);
	if (ret != GSW_statusOk) {
		pr_err("Failed GSW_BRIDGE_PORT_CONFIG_GET: %d\n", bp);
		goto EXIT;
	}
	/*remove this delting bridgeport from other bridge port's member*/
	for (i = 0; i < ARRAY_SIZE(tmp->nBridgePortMap); i++) {
		for (j = 0; j < 16 /*u16*/; j++) {
			if (!(tmp->nBridgePortMap[i] & (1 << j)))
				continue; /*not memmber bit set */
			memset(tmp2->nBridgePortMap, 0,
			       sizeof(tmp2->nBridgePortMap));
			tmp2->eMask =
				GSW_BRIDGE_PORT_CONFIG_MASK_BRIDGE_PORT_MAP;
			tmp2->nBridgePortId = i * 16 + j;
			ret = gsw_bp->BridgePort_ConfigGet(gsw_handle, tmp2);
			if (ret != GSW_statusOk) {
				pr_err("Failed GSW_BRIDGE_PORT_CONFIG_GET\n");
				goto EXIT;
			}
			UNSET_BP_MAP(tmp2->nBridgePortMap, bp);
			ret = gsw_bp->BridgePort_ConfigSet(gsw_handle, tmp2);
			if (ret != GSW_statusOk) {
				pr_err("Failed GSW_BRIDGE_PORT_CONFIG_SET\n");
				goto EXIT;
			}
		}
	}
EXIT:
	/*FRee thie bridge port */
	memset(tmp, 0, sizeof(tmp));
	tmp->nBridgePortId = bp;
	tmp->eMask = GSW_BRIDGE_PORT_CONFIG_MASK_BRIDGE_PORT_MAP;
	ret = gsw_bp->BridgePort_Free(gsw_handle,
				      (GSW_BRIDGE_portAlloc_t *)tmp);
	if (ret != GSW_statusOk)
		pr_err("Failed to GSW_BRIDGE_PORT_FREE:%d\n", bp);
FREE_EXIT:
	kfree(tmp);
	kfree(tmp2);
	return 0;
}

int dp_gswip_mac_entry_add(int bport, int fid, int inst, u8 *addr)
{
	GSW_MAC_tableAdd_t tmp;
	GSW_return_t ret;
	struct core_ops *gsw_handle;

	gsw_handle = dp_port_prop[inst].ops[GSWIP_L];
	memset(&tmp, 0, sizeof(tmp));
	tmp.bStaticEntry = 1;
	tmp.nFId = fid;
	tmp.nPortId = bport;
	SET_BP_MAP(tmp.nPortMap, bport);
	tmp.nSubIfId = 0;
	memcpy(tmp.nMAC, addr, GSW_MAC_ADDR_LEN);
	ret = gsw_handle->gsw_swmac_ops.MAC_TableEntryAdd(gsw_handle, &tmp);
	if (ret != GSW_statusOk) {
		pr_err("fail in setting MAC table static add entry\r\n");
		return -1;
	}
	return 0;
}

int dp_gswip_mac_entry_del(int bport, int fid, int inst, u8 *addr)
{
	GSW_MAC_tableRemove_t tmp;
	GSW_MAC_tableQuery_t mac_query;
	GSW_return_t ret;
	struct core_ops *gsw_handle;
	struct swmac_ops *gsw_swmac;

	gsw_handle = dp_port_prop[inst].ops[GSWIP_L];
	gsw_swmac = &gsw_handle->gsw_swmac_ops;
	memset(&tmp, 0, sizeof(tmp));
	memset(&mac_query, 0, sizeof(mac_query));
	mac_query.nFId = fid;
	memcpy(mac_query.nMAC, addr, GSW_MAC_ADDR_LEN);
	ret = gsw_swmac->MAC_TableEntryQuery(gsw_handle, &mac_query);
	if (ret != GSW_statusOk) {
		pr_err("fail in getting MAC query entry\r\n");
		return -1;
	}
	tmp.nFId = fid;
	memcpy(tmp.nMAC, addr, GSW_MAC_ADDR_LEN);
	ret = gsw_swmac->MAC_TableEntryRemove(gsw_handle, &tmp);
	if (ret != GSW_statusOk) {
		pr_err("fail in setting MAC static entry remove\r\n");
		return -1;
	}
	return 0;
}

int cpu_vlan_mod_dis(int inst)
{
	GSW_QoS_portRemarkingCfg_t cfg = {0};
	struct core_ops *ops;
	int ret;

	ops = dp_port_prop[inst].ops[GSWIP_L];

	cfg.nPortId = 0;
	ret = ops->gsw_qos_ops.QoS_PortRemarkingCfgGet(ops, &cfg);
	if (ret != GSW_statusOk) {
		pr_err("QoS_PortRemarkingCfgGet failed\n");
		return -1;
	}

	cfg.bPCP_EgressRemarkingEnable = LTQ_FALSE;
	ret = ops->gsw_qos_ops.QoS_PortRemarkingCfgSet(ops, &cfg);
	if (ret != GSW_statusOk) {
		pr_err("QoS_PortRemarkingCfgSet failed\n");
		return -1;
	}

	return 0;
}

int dp_set_gsw_pmapper_31(int inst, int bport, int lport,
			  struct dp_pmapper *mapper, u32 flag)
{
	GSW_BRIDGE_portConfig_t bp_cfg = {0};
	struct core_ops *gsw_handle;
	struct brdgport_ops *gsw_bp;
	GSW_return_t ret;
	int i, index;
	int ctp;
	struct pmac_port_info *port_info = get_dp_port_info(inst, lport);

	gsw_handle = dp_port_prop[inst].ops[GSWIP_L];
	gsw_bp = &gsw_handle->gsw_brdgport_ops;
	DP_DEBUG(DP_DBG_FLAG_DBG,
		 "set pmapper bport %d inst %d lport %d\n",
		 bport, inst, lport);
	bp_cfg.nBridgePortId = bport;
	bp_cfg.nDestLogicalPortId = lport;
	bp_cfg.bPmapperEnable = 1;
	bp_cfg.ePmapperMappingMode = mapper->mode;

	/* copy the sub if information in all pmapper list*/
	if (mapper->def_ctp != DP_PMAPPER_DISCARD_CTP) {
		ctp = GET_VAP(mapper->def_ctp, port_info->vap_offset,
			      port_info->vap_mask);
	} else {
		ctp = PMAPPER_DISC_CTP;
	}
	bp_cfg.sPmapper.nDestSubIfIdGroup[0] = ctp;

	for (i = 0; i < DP_PMAP_PCP_NUM; i++) {
		if (mapper->pcp_map[i] != DP_PMAPPER_DISCARD_CTP) {
			ctp = GET_VAP(mapper->pcp_map[i],
				      port_info->vap_offset,
				      port_info->vap_mask);
		} else {
			ctp = PMAPPER_DISC_CTP;
		}
		bp_cfg.sPmapper.nDestSubIfIdGroup[1 + i] = ctp;
	}
	for (i = 0; i < DP_PMAP_DSCP_NUM; i++) {
		if (mapper->dscp_map[i] != DP_PMAPPER_DISCARD_CTP) {
			ctp = GET_VAP(mapper->dscp_map[i],
				      port_info->vap_offset,
				      port_info->vap_mask);
		} else {
			ctp = PMAPPER_DISC_CTP;
		}
		index = 1 + DP_PMAP_PCP_NUM + i;
		bp_cfg.sPmapper.nDestSubIfIdGroup[index] = ctp;
	}
	bp_cfg.eMask = GSW_BRIDGE_PORT_CONFIG_MASK_EGRESS_CTP_MAPPING;

	DP_DEBUG(DP_DBG_FLAG_DBG,
		 "call switch api mode %d enable %d eMask 0x%x\n",
		 bp_cfg.ePmapperMappingMode, bp_cfg.bPmapperEnable,
		 bp_cfg.eMask);
	ret = gsw_bp->BridgePort_ConfigSet(gsw_handle, &bp_cfg);
	if (ret != GSW_statusOk) {
		pr_err("fail in setting pmapper\r\n");
		return -1;
	}
	return 0;
}

int dp_get_gsw_pmapper_31(int inst, int bport, int lport,
			  struct dp_pmapper *mapper, u32 flag)
{
	GSW_BRIDGE_portConfig_t bp_cfg = {0};
	struct core_ops *gsw_handle;
	struct brdgport_ops *gsw_bp;
	GSW_return_t ret;
	int i, index;
	struct hal_priv *priv;
	u16 dest;
	struct pmac_port_info *info = get_dp_port_info(inst, lport);

	priv = (struct hal_priv *)dp_port_prop[inst].priv_hal;
	gsw_handle = dp_port_prop[inst].ops[GSWIP_L];
	gsw_bp = &gsw_handle->gsw_brdgport_ops;
	DP_DEBUG(DP_DBG_FLAG_DBG,
		 "get bport %d inst %d lport %d\n",
		 bport, inst, lport);
	bp_cfg.nBridgePortId = bport;
	bp_cfg.nDestLogicalPortId = lport;
	bp_cfg.eMask = GSW_BRIDGE_PORT_CONFIG_MASK_EGRESS_CTP_MAPPING;

	ret = gsw_bp->BridgePort_ConfigGet(gsw_handle, &bp_cfg);
	if (ret != GSW_statusOk) {
		pr_err("fail in getting pmapper\r\n");
		return -1;
	}

	DP_DEBUG(DP_DBG_FLAG_DBG,
		 "after call switch api mode %d enable %d\n",
		 bp_cfg.ePmapperMappingMode, bp_cfg.bPmapperEnable);

	if (!bp_cfg.bPmapperEnable) {
		pr_err("pmapper not enabled\r\n");
		return -1;
	}
	mapper->pmapper_id = bp_cfg.sPmapper.nPmapperId;
	mapper->mode = bp_cfg.ePmapperMappingMode;

	dest = bp_cfg.sPmapper.nDestSubIfIdGroup[0];
	if (dest == PMAPPER_DISC_CTP)
		mapper->def_ctp = DP_PMAPPER_DISCARD_CTP;
	else
		mapper->def_ctp = SET_VAP(dest, info->vap_offset,
					  info->vap_mask);
	for (i = 0; i < DP_PMAP_PCP_NUM; i++) {
		dest = bp_cfg.sPmapper.nDestSubIfIdGroup[1 + i];
		if (dest == PMAPPER_DISC_CTP)
			mapper->pcp_map[i] = DP_PMAPPER_DISCARD_CTP;
		else
			mapper->pcp_map[i] = SET_VAP(dest, info->vap_offset,
						     info->vap_mask);
	}
	for (i = 0; i < DP_PMAP_DSCP_NUM; i++) {
		index = 1 + DP_PMAP_PCP_NUM + i;
		dest = bp_cfg.sPmapper.nDestSubIfIdGroup[index];
		if (dest == PMAPPER_DISC_CTP)
			mapper->dscp_map[i] = DP_PMAPPER_DISCARD_CTP;
		else
			mapper->dscp_map[i] = SET_VAP(dest, info->vap_offset,
						      info->vap_mask);
	}
	return 0;
}

int dp_meter_alloc_31(int inst, int *meterid, int flag)
{
	struct core_ops *gsw_handle;
	struct qos_ops *gsw_qos;
	GSW_QoS_meterCfg_t meter_cfg = {0};
	GSW_return_t ret;

	if (inst < 0) {
		pr_err("inst invalid\n");
		return -1;
	}
	if (!meterid) {
		pr_err("meterid NULL\n");
		return -1;
	}
	gsw_handle = dp_port_prop[inst].ops[GSWIP_L];
	if (!gsw_handle) {
		pr_err("gsw_handle NULL\n");
		return -1;
	}
	gsw_qos = &gsw_handle->gsw_qos_ops;
	if (flag == DP_F_DEREGISTER && *meterid >= 0) {
		meter_cfg.nMeterId = *meterid;
		ret = gsw_qos->QOS_MeterAlloc(gsw_handle, &meter_cfg);
		if (ret != GSW_statusOk) {
			pr_err("Meter dealloc failed: %d\n", ret);
			return -1;
		}
		return 0;
	}
	memset(&meter_cfg, 0, sizeof(meter_cfg));
	ret = gsw_qos->QOS_MeterAlloc(gsw_handle, &meter_cfg);
	if ((ret != GSW_statusOk) || (meter_cfg.nMeterId < 0)) {
		pr_err("Failed to get a meter alloc\n");
		*meterid = -1;
		return -1;
	}
	*meterid = meter_cfg.nMeterId;
	return 0;
}

static int dp_set_col_mark(struct net_device *dev, struct dp_meter_cfg  *meter,
			   int flag, struct dp_meter_subif *mtr_subif)
{
	struct core_ops *gsw_handle;
	struct ctp_ops *gsw_ctp;
	struct brdgport_ops *gsw_bp;
	GSW_BRIDGE_portConfig_t bp_cfg;
	GSW_CTP_portConfig_t ctp_cfg;
	GSW_return_t ret;
	struct pmac_port_info *port_info;

	if (!mtr_subif) {
		pr_err("mtr_subif NULL\n");
		return -1;
	}
	memset(&bp_cfg, 0, sizeof(GSW_BRIDGE_portConfig_t));
	memset(&ctp_cfg, 0, sizeof(GSW_CTP_portConfig_t));
	gsw_handle = dp_port_prop[mtr_subif->inst].ops[GSWIP_L];
	if (!gsw_handle)
		return -1;
	gsw_ctp = &gsw_handle->gsw_ctp_ops;
	gsw_bp = &gsw_handle->gsw_brdgport_ops;
	if (flag & DP_METER_ATTACH_CTP) {/* CTP port Flag */
		if (mtr_subif->subif.flag_pmapper) {
			pr_err("can't use CTP,pmapper is enable\n");
			return -1;
		}
		port_info = get_dp_port_info(mtr_subif->subif.inst,
					     mtr_subif->subif.port_id);
		ctp_cfg.nLogicalPortId = mtr_subif->subif.port_id;
		ctp_cfg.nSubIfIdGroup  = GET_VAP(mtr_subif->subif.subif,
						 port_info->vap_offset,
						 port_info->vap_mask);
		ret = gsw_ctp->CTP_PortConfigGet(gsw_handle, &ctp_cfg);
		if (ret != GSW_statusOk) {
			pr_err("PortConfigGet API failed :%d\n", ret);
			return -1;
		}
		if (meter->dir == DP_DIR_INGRESS) {
			ctp_cfg.eMask = GSW_CTP_PORT_CONFIG_INGRESS_MARKING;
			ctp_cfg.eIngressMarkingMode = meter->mode;
		} else if (meter->dir == DP_DIR_EGRESS) {
			if (meter->mode != DP_INTERNAL) {
				ctp_cfg.eMask =
				GSW_CTP_PORT_CONFIG_EGRESS_MARKING_OVERRIDE;
				ctp_cfg.bEgressMarkingOverrideEnable = 1;
				ctp_cfg.eEgressMarkingModeOverride =
					meter->mode;
			} else {
				DP_DEBUG(DP_DBG_FLAG_PAE,
					 "mode:internal invalid for egress\n");
			}
		} else {
			return -1;
		}
		ret = gsw_ctp->CTP_PortConfigSet(gsw_handle, &ctp_cfg);
		if (ret != GSW_statusOk) {
			pr_err("PortConfigSet API failed :%d\n", ret);
			return -1;
		}
	}
	if (flag & DP_METER_ATTACH_BRPORT) {/*BRIDGE port Flag*/
		if (!mtr_subif->subif.flag_bp) {
			pr_err("flag_bp value 0\n");
			return -1;
		}
		bp_cfg.nBridgePortId = mtr_subif->subif.bport;
		ret = gsw_bp->BridgePort_ConfigGet(gsw_handle, &bp_cfg);
		if (ret != GSW_statusOk) {
			pr_err("BridgePort_ConfigGet API failed :%d\n", ret);
			return -1;
		}
		if (meter->dir == DP_DIR_EGRESS) {
			pr_err("No Egress color marking for bridge port\n");
			return -1;
		} else if (meter->dir == DP_DIR_INGRESS) {
			bp_cfg.eMask =
				GSW_BRIDGE_PORT_CONFIG_MASK_INGRESS_MARKING;
			bp_cfg.eIngressMarkingMode = meter->mode;
		} else {
			pr_err(" invalid color mark dir\n");
			return -1;
		}
		ret = gsw_bp->BridgePort_ConfigSet(gsw_handle, &bp_cfg);
		if (ret != GSW_statusOk) {
			pr_err("BridgePort_ConfigSet API failed :%d\n", ret);
			return -1;
		}
	}
	return 0;
}

int dp_meter_add_31(struct net_device *dev,  struct dp_meter_cfg  *meter,
		    int flag, struct dp_meter_subif *mtr_subif)
{
	struct core_ops *gsw_handle;
	struct qos_ops *gsw_qos;
	struct tflow_ops *gsw_tflow;
	struct ctp_ops *gsw_ctp;
	struct brdgport_ops *gsw_bp;
	struct brdg_ops *gsw_brdg;
	GSW_QoS_meterCfg_t meter_cfg;
	GSW_BRIDGE_portConfig_t *bp_cfg = NULL;
	GSW_PCE_rule_t *pce_rule = NULL;
	GSW_CTP_portConfig_t *ctp_cfg = NULL;
	GSW_BRIDGE_config_t *br_cfg = NULL;
	GSW_return_t ret;
	int bret = 0;

	if (!mtr_subif) {
		pr_err("mtr_subif NULL\n");
		return -1;
	}
	gsw_handle = dp_port_prop[mtr_subif->inst].ops[GSWIP_L];
	if (!gsw_handle)
		return -1;
	gsw_qos = &gsw_handle->gsw_qos_ops;
	gsw_tflow = &gsw_handle->gsw_tflow_ops;
	gsw_ctp = &gsw_handle->gsw_ctp_ops;
	gsw_bp = &gsw_handle->gsw_brdgport_ops;
	gsw_brdg = &gsw_handle->gsw_brdg_ops;
	if (flag & DP_COL_MARKING)
		return dp_set_col_mark(dev, meter, flag, mtr_subif);
	memset(&meter_cfg, 0, sizeof(GSW_QoS_meterCfg_t));
	meter_cfg.nCbs = meter->cbs;
	meter_cfg.nRate = METER_CIR(meter->cir);
	meter_cfg.nEbs = meter->pbs;
	meter_cfg.nPiRate = METER_PIR(meter->pir);
	meter_cfg.bEnable = 1;
	if (meter->type == srTCM) {
		meter_cfg.eMtrType = GSW_QOS_Meter_srTCM;
	} else if (meter->type == trTCM) {
		meter_cfg.eMtrType = GSW_QOS_Meter_trTCM;
	} else {
		pr_err(" invalid meter type\n");
		return -1;
	}
	meter_cfg.nMeterId = meter->meter_id;
	meter_cfg.nColourBlindMode = meter->col_mode;
	ret = gsw_qos->QoS_MeterCfgSet(gsw_handle, &meter_cfg);
	if (ret != GSW_statusOk) {
		pr_err("MeterCfgSet API failed:%d\n", ret);
		return -1;
	}

	if (flag & DP_METER_ATTACH_PCE) {
		/* pattern setting */
		pce_rule = kzalloc(sizeof(GSW_PCE_rule_t), GFP_ATOMIC);
		if (!pce_rule) {
			pr_err("ctp_cfg alloc failed\n");
			bret = -1;
			goto err;
		}
		pce_rule->pattern.nIndex = meter->dp_pce.pce_idx;
		pce_rule->pattern.bEnable = 1;
		/* action setting */
		pce_rule->action.eMeterAction = GSW_PCE_ACTION_METER_1;
		pce_rule->action.nMeterId =  meter->meter_id;
		ret = gsw_tflow->TFLOW_PceRuleWrite(gsw_handle, pce_rule);
		if (ret != GSW_statusOk) {
			pr_err("PceRule Write API failed :%d\n", ret);
			goto err;
		}
	}
	if (flag & DP_METER_ATTACH_CTP) {/* CTP port Flag */
		struct pmac_port_info *port_info;

		if (mtr_subif->subif.flag_pmapper) {
			pr_err("can't use CTP,pmapper is enable\n");
			bret = -1;
			goto err;
		}
		ctp_cfg = kzalloc(sizeof(GSW_CTP_portConfig_t), GFP_ATOMIC);
		if (!ctp_cfg) {
			pr_err("ctp_cfg alloc failed\n");
			bret = -1;
			goto err;
		}
		port_info = get_dp_port_info(mtr_subif->subif.inst,
					     mtr_subif->subif.port_id);
		if (!port_info) {
			pr_err(" port_info is NULL\n");
			bret = -1;
			goto err;
		}
		ctp_cfg->nLogicalPortId = mtr_subif->subif.port_id;
		ctp_cfg->nSubIfIdGroup  = GET_VAP(mtr_subif->subif.subif,
						 port_info->vap_offset,
						 port_info->vap_mask);
		ret = gsw_ctp->CTP_PortConfigGet(gsw_handle, ctp_cfg);
		if (ret != GSW_statusOk) {
			pr_err("PortConfigGet API failed :%d\n", ret);
			bret = -1;
			goto err;
		}
		if (meter->dir == DP_DIR_EGRESS) {
			ctp_cfg->eMask = GSW_CTP_PORT_CONFIG_EGRESS_METER;
			ctp_cfg->bEgressMeteringEnable = 1;
			ctp_cfg->nEgressTrafficMeterId =  meter->meter_id;
		} else if (meter->dir == DP_DIR_INGRESS) {
			ctp_cfg->eMask = GSW_CTP_PORT_CONFIG_INGRESS_METER;
			ctp_cfg->bIngressMeteringEnable = 1;
			ctp_cfg->nIngressTrafficMeterId =  meter->meter_id;
		} else {
			pr_err(" invalid meter dir\n");
			return -1;
		}
		ret = gsw_ctp->CTP_PortConfigSet(gsw_handle, ctp_cfg);
		if (ret != GSW_statusOk) {
			pr_err("PortConfigSet API failed :%d\n", ret);
			bret = -1;
			goto err;
		}
	}
	if (flag & DP_METER_ATTACH_BRPORT) {/*BRIDGE port Flag*/
		if (!mtr_subif->subif.flag_bp) {
			pr_err("flag_bp value 0\n");
			bret = -1;
			goto err;
		}
		bp_cfg = kzalloc(sizeof(GSW_BRIDGE_portConfig_t), GFP_ATOMIC);
		if (!bp_cfg) {
			pr_err("bp_cfg alloc failed\n");
			bret = -1;
			goto err;
		}
		bp_cfg->nBridgePortId = mtr_subif->subif.bport;
		ret = gsw_bp->BridgePort_ConfigGet(gsw_handle, bp_cfg);
		if (ret != GSW_statusOk) {
			pr_err("BridgePort_ConfigGet API failed :%d\n", ret);
			bret = -1;
			goto err;
		}
		if (meter->dir == DP_DIR_EGRESS) {
			bp_cfg->eMask =
				GSW_BRIDGE_PORT_CONFIG_MASK_EGRESS_SUB_METER;
			if (meter->dp_pce.flow == DP_UKNOWN_UNICAST)
				BP_CFG(bp_cfg,
				       GSW_BRIDGE_PORT_EGRESS_METER_UNKNOWN_UC,
				       meter->meter_id, 1);
			else if (meter->dp_pce.flow == DP_MULTICAST)
				BP_CFG(bp_cfg,
				       GSW_BRIDGE_PORT_EGRESS_METER_MULTICAST,
				       meter->meter_id, 1);
			else if (meter->dp_pce.flow == DP_BROADCAST)
				BP_CFG(bp_cfg,
				       GSW_BRIDGE_PORT_EGRESS_METER_BROADCAST,
				       meter->meter_id, 1);
			else
				BP_CFG(bp_cfg,
				       GSW_BRIDGE_PORT_EGRESS_METER_OTHERS,
				       meter->meter_id, 1);
		} else if (meter->dir == DP_DIR_INGRESS) {
			bp_cfg->eMask =
				GSW_BRIDGE_PORT_CONFIG_MASK_INGRESS_METER;
			bp_cfg->bIngressMeteringEnable = 1;
			bp_cfg->nIngressTrafficMeterId = meter->meter_id;
		} else {
			pr_err(" invalid meter dir\n");
			return -1;
		}
		ret = gsw_bp->BridgePort_ConfigSet(gsw_handle, bp_cfg);
		if (ret != GSW_statusOk) {
			pr_err("BridgePort_ConfigSet API failed :%d\n", ret);
			bret = -1;
			goto err;
		}
	} else if (flag & DP_METER_ATTACH_BRIDGE) {
		br_cfg = kzalloc(sizeof(GSW_BRIDGE_config_t), GFP_ATOMIC);
		if (!br_cfg) {
			pr_err("br_cfg alloc failed\n");
			bret = -1;
			goto err;
		}
		br_cfg->nBridgeId = mtr_subif->fid;
		ret = gsw_brdg->Bridge_ConfigGet(gsw_handle, br_cfg);
		if (ret != GSW_statusOk) {
			pr_err("Bridge_ConfigGet API failed :%d\n", ret);
			bret = -1;
			goto err;
		}
		br_cfg->eMask = GSW_BRIDGE_CONFIG_MASK_SUB_METER;
		if (meter->dp_pce.flow == DP_UKNOWN_UNICAST)
			BR_CFG(br_cfg,
			       GSW_BRIDGE_PORT_EGRESS_METER_UNKNOWN_UC,
			       meter->meter_id, 1);
		else if (meter->dp_pce.flow == DP_MULTICAST)
			BR_CFG(br_cfg,
			       GSW_BRIDGE_PORT_EGRESS_METER_MULTICAST,
			       meter->meter_id, 1);
		else if (meter->dp_pce.flow == DP_BROADCAST)
			BR_CFG(br_cfg,
			       GSW_BRIDGE_PORT_EGRESS_METER_BROADCAST,
			       meter->meter_id, 1);
		else {
			pr_err("Meter flow invalid\n");
			bret = -1;
			goto err;
		}
		ret = gsw_brdg->Bridge_ConfigSet(gsw_handle, br_cfg);
		if (ret != GSW_statusOk) {
			pr_err("Bridge_ConfigSet API failed :%d\n", ret);
			bret = -1;
			goto err;
		}
	}
err:
	kfree(br_cfg);
	kfree(bp_cfg);
	kfree(ctp_cfg);
	kfree(pce_rule);
	return bret;
}

int dp_meter_del_31(struct net_device *dev,  struct dp_meter_cfg  *meter,
		    int flag, struct dp_meter_subif *mtr_subif)
{
	struct core_ops *gsw_handle;
	struct tflow_ops *gsw_tflow;
	struct ctp_ops *gsw_ctp;
	struct brdgport_ops *gsw_bp;
	struct brdg_ops *gsw_brdg;
	GSW_BRIDGE_portConfig_t *bp_cfg = NULL;
	GSW_PCE_rule_t *pce_rule = NULL;
	GSW_CTP_portConfig_t *ctp_cfg = NULL;
	GSW_BRIDGE_config_t *br_cfg = NULL;
	GSW_return_t ret;
	int bret = 0;

	if (!mtr_subif) {
		pr_err(" mtr_subif NULL\n");
		return -1;
	}
	gsw_handle = dp_port_prop[mtr_subif->inst].ops[GSWIP_L];
	if (!gsw_handle)
		return -1;
	gsw_tflow = &gsw_handle->gsw_tflow_ops;
	gsw_ctp = &gsw_handle->gsw_ctp_ops;
	gsw_bp = &gsw_handle->gsw_brdgport_ops;
	gsw_brdg = &gsw_handle->gsw_brdg_ops;
	if (meter->dir & DP_METER_ATTACH_PCE) {
		pce_rule = kzalloc(sizeof(GSW_PCE_rule_t), GFP_ATOMIC);
		if (!pce_rule) {
			pr_err("ctp_cfg alloc failed\n");
			bret = -1;
			goto err;
		}
		/* pattern setting */
		pce_rule->pattern.nIndex = meter->dp_pce.pce_idx;
		pce_rule->pattern.bEnable = 0;
		ret = gsw_tflow->TFLOW_PceRuleWrite(gsw_handle, pce_rule);
		if (ret != GSW_statusOk) {
			pr_err("PceRule Write API failed :%d\n", ret);
			bret = -1;
			goto err;
		}
	}
	if (flag & DP_METER_ATTACH_CTP) {
		struct pmac_port_info *port_info;

		if (mtr_subif->subif.flag_pmapper) {
			pr_err("flag_pmapper is set\n");
			bret = -1;
			goto err;
		}
		ctp_cfg = kzalloc(sizeof(GSW_CTP_portConfig_t), GFP_ATOMIC);
		if (!ctp_cfg) {
			pr_err("ctp_cfg alloc failed\n");
			bret = -1;
			goto err;
		}
		port_info = get_dp_port_info(mtr_subif->subif.inst,
					     mtr_subif->subif.port_id);
		if (!port_info) {
			pr_err(" port_info is NULL\n");
			bret = -1;
			goto err;
		}
		ctp_cfg->nLogicalPortId = mtr_subif->subif.port_id;
		ctp_cfg->nSubIfIdGroup = GET_VAP(mtr_subif->subif.subif,
							port_info->vap_offset,
							port_info->vap_mask);
		if (meter->dir == DP_DIR_EGRESS)
			ctp_cfg->nEgressTrafficMeterId =  meter->meter_id;
		else if (meter->dir == DP_DIR_INGRESS)
			ctp_cfg->nIngressTrafficMeterId =  meter->meter_id;
		ret = gsw_ctp->CTP_PortConfigGet(gsw_handle, ctp_cfg);
		if (ret != GSW_statusOk) {
			bret = -1;
			goto err;
		}
		if (meter->dir == DP_DIR_EGRESS) {
			ctp_cfg->eMask = GSW_CTP_PORT_CONFIG_EGRESS_METER;
			ctp_cfg->bEgressMeteringEnable = 0;
		} else if (meter->dir == DP_DIR_INGRESS) {
			ctp_cfg->eMask = GSW_CTP_PORT_CONFIG_INGRESS_METER;
			ctp_cfg->bIngressMeteringEnable = 0;
		}
		ret = gsw_ctp->CTP_PortConfigSet(gsw_handle, ctp_cfg);
		if (ret != GSW_statusOk) {
			bret = -1;
			goto err;
		}
	}
	if (flag & DP_METER_ATTACH_BRPORT) {
		if (!mtr_subif->subif.flag_bp) {
			pr_err("flag_bp is 0\n");
			bret = -1;
			goto err;
		}
		bp_cfg = kzalloc(sizeof(GSW_BRIDGE_portConfig_t), GFP_ATOMIC);
		if (!bp_cfg) {
			pr_err("bp_cfg alloc failed\n");
			bret = -1;
			goto err;
		}
		bp_cfg->nBridgePortId = mtr_subif->subif.bport;
		ret = gsw_bp->BridgePort_ConfigGet(gsw_handle, bp_cfg);
		if (ret != GSW_statusOk) {
			pr_err("BridgePort_ConfigGet API failed\n");
			bret = -1;
			goto err;
		}
		if (meter->dir == DP_DIR_EGRESS) {
			bp_cfg->eMask =
				GSW_BRIDGE_PORT_CONFIG_MASK_EGRESS_SUB_METER;
			if (meter->dp_pce.flow == DP_UKNOWN_UNICAST)
				BP_CFG(bp_cfg,
				       GSW_BRIDGE_PORT_EGRESS_METER_UNKNOWN_UC,
				       meter->meter_id, 0);
			else if (meter->dp_pce.flow == DP_MULTICAST)
				BP_CFG(bp_cfg,
				       GSW_BRIDGE_PORT_EGRESS_METER_MULTICAST,
				       meter->meter_id, 0);
			else if (meter->dp_pce.flow == DP_BROADCAST)
				BP_CFG(bp_cfg,
				       GSW_BRIDGE_PORT_EGRESS_METER_BROADCAST,
				       meter->meter_id, 0);
			else
				BP_CFG(bp_cfg,
				       GSW_BRIDGE_PORT_EGRESS_METER_OTHERS,
				       meter->meter_id, 0);
		} else {
			bp_cfg->eMask =
				GSW_BRIDGE_PORT_CONFIG_MASK_INGRESS_METER;
			bp_cfg->bIngressMeteringEnable = 0;
		}
		ret = gsw_bp->BridgePort_ConfigSet(gsw_handle, bp_cfg);
		if (ret != GSW_statusOk) {
			pr_err("BridgePort_ConfigSet API failed\n");
			bret = -1;
			goto err;
		}
	}
	if (flag & DP_METER_ATTACH_BRIDGE) {
		br_cfg = kzalloc(sizeof(GSW_BRIDGE_config_t), GFP_ATOMIC);
		if (!br_cfg) {
			pr_err("br_cfg alloc failed\n");
			bret = -1;
			goto err;
		}
		br_cfg->nBridgeId = mtr_subif->fid;
		ret = gsw_brdg->Bridge_ConfigGet(gsw_handle, br_cfg);
		if (ret != GSW_statusOk) {
			pr_err("Bridge_ConfigGet API failed :%d\n", ret);
			bret = -1;
			goto err;
		}
		br_cfg->eMask = GSW_BRIDGE_CONFIG_MASK_SUB_METER;
		if (meter->dp_pce.flow == DP_UKNOWN_UNICAST)
			BR_CFG(br_cfg,
			       GSW_BRIDGE_PORT_EGRESS_METER_UNKNOWN_UC,
			       meter->meter_id, 0);
		else if (meter->dp_pce.flow == DP_MULTICAST)
			BR_CFG(br_cfg,
			       GSW_BRIDGE_PORT_EGRESS_METER_MULTICAST,
			       meter->meter_id, 0);
		else if (meter->dp_pce.flow == DP_BROADCAST)
			BR_CFG(br_cfg,
			       GSW_BRIDGE_PORT_EGRESS_METER_BROADCAST,
			       meter->meter_id, 0);
		else {
			pr_err("Meter flow invalid\n");
			bret = -1;
			goto err;
		}
		ret = gsw_brdg->Bridge_ConfigSet(gsw_handle, br_cfg);
		if (ret != GSW_statusOk) {
			pr_err("Bridge_ConfigSet API failed :%d\n", ret);
			bret = -1;
			goto err;
		}
	}
err:
	kfree(br_cfg);
	kfree(bp_cfg);
	kfree(ctp_cfg);
	kfree(pce_rule);
	return bret;
}

int dp_set_bp_attr_31(struct dp_bp_attr *conf, int bport, uint32_t flag)
{
	GSW_return_t ret;
	GSW_BRIDGE_portConfig_t brportcfg;
	struct core_ops *ops = dp_port_prop[conf->inst].ops[0];

	memset(&brportcfg, 0, sizeof(GSW_BRIDGE_portConfig_t));

	brportcfg.nBridgePortId = bport;
	brportcfg.eMask = GSW_BRIDGE_PORT_CONFIG_MASK_BRIDGE_PORT_MAP;

	ret = ops->gsw_brdgport_ops.BridgePort_ConfigGet(ops, &brportcfg);
	if (ret != GSW_statusOk) {
		pr_err("fail in getting bridge port config\r\n");
		return DP_FAILURE;
	}

	if (conf->en)
		SET_BP_MAP(brportcfg.nBridgePortMap, CPU_PORT);
	else
		UNSET_BP_MAP(brportcfg.nBridgePortMap, CPU_PORT);

	ret = ops->gsw_brdgport_ops.BridgePort_ConfigSet(ops, &brportcfg);
	if (ret != GSW_statusOk) {
		pr_err("Fail in allocating/configuring bridge port\n");
		return DP_FAILURE;
	}

	return DP_SUCCESS;
}
