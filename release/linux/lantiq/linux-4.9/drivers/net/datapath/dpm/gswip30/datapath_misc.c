/*
 * Copyright (C) Intel Corporation
 * Author: Shao Guohua <guohua.shao@intel.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/version.h>
#include <linux/if_ether.h>
#include <linux/ethtool.h>
#include <linux/proc_fs.h>
#include <linux/delay.h>
#include <linux/init.h>
#include <linux/clk.h>
#include <linux/if_ether.h>
#include <linux/if_vlan.h>

#include <linux/clk.h>
#include <linux/ip.h>
#include <net/ip.h>
#define DATAPATH_HAL_LAYER   /*must put before include datapath_api.h in
			      *order to avoid include another platform's
			      *DMA descriptor and pmac header files
			      */
#include <net/datapath_api.h>
#include <net/datapath_api_gswip30.h>
#include "../datapath.h"
#include "datapath_proc.h"
#include "datapath_misc.h"
#include "datapath_mib.h"

#if IS_ENABLED(CONFIG_LTQ_TMU)
#include <net/drv_tmu_ll.h>
#endif

static void init_dma_desc_mask(void)
{
	/*mask 0: to remove the bit, 1 -- keep the bit */
	dma_rx_desc_mask1.all = 0xFFFFFFFF;
	dma_rx_desc_mask3.all = 0xFFFFFFFF;
	dma_rx_desc_mask3.field.own = 0;
	dma_rx_desc_mask3.field.c = 0;
	dma_rx_desc_mask3.field.sop = 0;
	dma_rx_desc_mask3.field.eop = 0;
	dma_rx_desc_mask3.field.dic = 0;
	dma_rx_desc_mask3.field.byte_offset = 0;
	dma_rx_desc_mask1.field.dec = 0;
	dma_rx_desc_mask1.field.enc = 0;
	dma_rx_desc_mask1.field.mpe2 = 0;
	dma_rx_desc_mask1.field.mpe1 = 0;
	/*mask to keep some value via 1
	 *set by top application all others set to 0
	 */
	dma_tx_desc_mask0.all = 0;
	dma_tx_desc_mask1.all = 0;
	dma_tx_desc_mask0.field.flow_id = 0xFF;
	dma_tx_desc_mask0.field.dest_sub_if_id = 0x7FFF;
	dma_tx_desc_mask1.field.mpe1 = 0x1;
	dma_tx_desc_mask1.field.color = 0x3;
	dma_tx_desc_mask1.field.ep = 0xF;
}

static void init_dma_pmac_template(int portid, u32 flags)
{
	int i;
	struct pmac_port_info *dp_info = get_dp_port_info(0, portid);

	/*Note:
	 * final tx_dma0 = (tx_dma0 & dma0_mask_template) | dma0_template
	 * final tx_dma1 = (tx_dma1 & dma1_mask_template) | dma1_template
	 * final tx_pmac = pmac_template
	 */
	memset(dp_info->pmac_template, 0, sizeof(dp_info->pmac_template));
	memset(dp_info->dma0_template, 0, sizeof(dp_info->dma0_template));
	memset(dp_info->dma1_template, 0, sizeof(dp_info->dma1_template));
	for (i = 0; i < MAX_TEMPLATE; i++) {
		dp_info->dma0_mask_template[i].all = 0xFFFFFFFF;
		dp_info->dma1_mask_template[i].all = 0xFFFFFFFF;
	}

	if (flags & DP_F_FAST_ETH_LAN) { /*always with pmac*/
		/*always with pmac */
		for (i = 0; i < MAX_TEMPLATE; i++) {
			dp_info->pmac_template[i].port_map_en = 1;
			dp_info->pmac_template[i].sppid = PMAC_CPU_ID;
			dp_info->pmac_template[i].redirect = 0;
			dp_info->pmac_template[i].class_en = 1;
			SET_PMAC_PORTMAP(&dp_info->pmac_template[i], portid);
		}
		/*for checksum for pmac_template[1]*/
		dp_info->pmac_template[TEMPL_CHECKSUM].tcp_chksum = 1;
	} else if (flags & DP_F_FAST_ETH_WAN) {/*always with pmac*/
		/*always with pmac */
		for (i = 0; i < MAX_TEMPLATE; i++) {
			dp_info->pmac_template[i].port_map_en = 1;
			dp_info->pmac_template[i].sppid = PMAC_CPU_ID;
			dp_info->pmac_template[i].redirect = 1;
			dp_info->pmac_template[i].class_en = 1;
			SET_PMAC_PORTMAP(&dp_info->pmac_template[i], portid);
		}
		/*for checksum for pmac_template[1]*/
		dp_info->pmac_template[TEMPL_CHECKSUM].tcp_chksum = 1;
	} else if (flags & DP_F_DIRECTLINK) { /*always with pmac*/
		/*normal dirctpath without checksum support
		 *but with pmac to Switch for accelerate
		 */
		dp_info->pmac_template[TEMPL_NORMAL].port_map_en = 0;
		dp_info->pmac_template[TEMPL_NORMAL].sppid = portid;
		dp_info->pmac_template[TEMPL_NORMAL].redirect = 0;
		dp_info->pmac_template[TEMPL_NORMAL].port_map = 0xff;
		dp_info->pmac_template[TEMPL_NORMAL].port_map2 = 0xff;
		dp_info->pmac_template[TEMPL_NORMAL].class_en = 1;
		SET_PMAC_PORTMAP(&dp_info->pmac_template[TEMPL_NORMAL], portid);
		dp_info->dma1_template[TEMPL_CHECKSUM].field.enc = 1;
		dp_info->dma1_template[TEMPL_CHECKSUM].field.dec = 1;
		dp_info->dma1_template[TEMPL_CHECKSUM].field.mpe2 = 0;
		dp_info->dma1_mask_template[TEMPL_CHECKSUM].field.enc = 0;
		dp_info->dma1_mask_template[TEMPL_CHECKSUM].field.dec = 0;
		dp_info->dma1_mask_template[TEMPL_CHECKSUM].field.mpe2 = 0;

		/*dirctpath with checksum support */
		dp_info->pmac_template[TEMPL_CHECKSUM].port_map_en = 1;
		dp_info->pmac_template[TEMPL_CHECKSUM].sppid = portid;
		dp_info->pmac_template[TEMPL_CHECKSUM].redirect = 1;
		dp_info->pmac_template[TEMPL_CHECKSUM].tcp_chksum = 1;
		dp_info->pmac_template[TEMPL_CHECKSUM].class_en = 1;
		SET_PMAC_PORTMAP(&dp_info->pmac_template[TEMPL_CHECKSUM],
				 portid);
		dp_info->dma1_template[TEMPL_CHECKSUM].field.enc = 1;
		dp_info->dma1_template[TEMPL_CHECKSUM].field.dec = 1;
		dp_info->dma1_template[TEMPL_CHECKSUM].field.mpe2 = 1;
		dp_info->dma1_mask_template[TEMPL_CHECKSUM].field.enc = 0;
		dp_info->dma1_mask_template[TEMPL_CHECKSUM].field.dec = 0;
		dp_info->dma1_mask_template[TEMPL_CHECKSUM].field.mpe2 = 0;

		/*dirctpath w/o checksum support but send packet to MPE DL FW*/
		dp_info->pmac_template[TEMPL_OTHERS].port_map_en = 1;
		dp_info->pmac_template[TEMPL_OTHERS].sppid = portid;
		dp_info->pmac_template[TEMPL_OTHERS].redirect = 1;
		dp_info->pmac_template[TEMPL_OTHERS].class_en = 1;
		SET_PMAC_PORTMAP(&dp_info->pmac_template[TEMPL_OTHERS], portid);
#if defined(CONFIG_ACCL_11AC_CS2) || defined(CONFIG_ACCL_11AC_CS2_MODULE)
		/* CPU traffic to PAE via cbm to apply PCE rule */
		dp_info->dma1_template[TEMPL_OTHERS].field.enc = 1;
		dp_info->dma1_template[TEMPL_OTHERS].field.dec = 1;
		dp_info->dma1_template[TEMPL_OTHERS].field.mpe2 = 0;
		dp_info->dma1_mask_template[TEMPL_OTHERS].field.enc = 0;
		dp_info->dma1_mask_template[TEMPL_OTHERS].field.dec = 0;
		dp_info->dma1_mask_template[TEMPL_OTHERS].field.mpe2 = 0;
#endif
	} else if (flags & DP_F_FAST_DSL) {
		/* For normal single DSL upstream, there is no pmac at all*/
		dp_info->dma1_template[TEMPL_NORMAL].field.dec = 1;
		dp_info->dma1_template[TEMPL_NORMAL].field.mpe2 = 1;
		dp_info->dma1_mask_template[TEMPL_NORMAL].field.enc = 0;
		dp_info->dma1_mask_template[TEMPL_NORMAL].field.dec = 0;
		dp_info->dma1_mask_template[TEMPL_NORMAL].field.mpe2 = 0;

		/*DSL  with checksum support */
		dp_info->pmac_template[TEMPL_CHECKSUM].port_map_en = 1;
		dp_info->pmac_template[TEMPL_CHECKSUM].sppid = portid;
		dp_info->pmac_template[TEMPL_CHECKSUM].redirect = 1;
		dp_info->pmac_template[TEMPL_CHECKSUM].tcp_chksum = 1;
		dp_info->pmac_template[TEMPL_CHECKSUM].class_en = 1;
		SET_PMAC_PORTMAP(&dp_info->pmac_template[TEMPL_CHECKSUM],
				 portid);
		dp_info->dma1_template[TEMPL_CHECKSUM].field.enc = 1;
		dp_info->dma1_template[TEMPL_CHECKSUM].field.dec = 1;
		dp_info->dma1_template[TEMPL_CHECKSUM].field.mpe2 = 1;
		dp_info->dma1_mask_template[TEMPL_CHECKSUM].field.enc = 0;
		dp_info->dma1_mask_template[TEMPL_CHECKSUM].field.dec = 0;
		dp_info->dma1_mask_template[TEMPL_CHECKSUM].field.mpe2 = 0;

		/*Bonding DSL  FCS Support via GSWIP */
		dp_info->pmac_template[TEMPL_OTHERS].port_map_en = 1;
		dp_info->pmac_template[TEMPL_OTHERS].sppid = portid;
		dp_info->pmac_template[TEMPL_OTHERS].redirect = 1;
		/*dp_info->pmac_template[TEMPL_OTHERS].tcp_chksum = 1; */
		dp_info->pmac_template[TEMPL_OTHERS].class_en = 1;
		SET_PMAC_PORTMAP(&dp_info->pmac_template[TEMPL_OTHERS],
				 portid);
		dp_info->dma1_template[TEMPL_OTHERS].field.enc = 1;
		dp_info->dma1_template[TEMPL_OTHERS].field.dec = 1;
		dp_info->dma1_template[TEMPL_OTHERS].field.mpe2 = 1;
		dp_info->dma1_mask_template[TEMPL_OTHERS].field.enc = 0;
		dp_info->dma1_mask_template[TEMPL_OTHERS].field.dec = 0;
		dp_info->dma1_mask_template[TEMPL_OTHERS].field.mpe2 = 0;
	} else if (flags & DP_F_FAST_WLAN) {
		/* WLAN block must put after DSL/DIRECTLINK block
		 * since all ACA device in GRX500 is using WLAN flag wrongly
		 * and here must make sure still be back-compatible for it.
		 * normal fast_wlan without pmac except checksum offload support
		 * So no need to configure pmac_tmplate[0]
		 */
		/*fast_wlan with checksum support */
		dp_info->pmac_template[TEMPL_CHECKSUM].port_map_en = 1;
		dp_info->pmac_template[TEMPL_CHECKSUM].sppid = portid;
		dp_info->pmac_template[TEMPL_CHECKSUM].redirect = 1;
		dp_info->pmac_template[TEMPL_CHECKSUM].tcp_chksum = 1;
		dp_info->pmac_template[TEMPL_CHECKSUM].class_en = 1;
		SET_PMAC_PORTMAP(&dp_info->pmac_template[TEMPL_CHECKSUM],
				 portid);
		dp_info->dma1_template[TEMPL_CHECKSUM].field.enc = 1;
		dp_info->dma1_template[TEMPL_CHECKSUM].field.dec = 1;
		dp_info->dma1_template[TEMPL_CHECKSUM].field.mpe2 = 1;
		dp_info->dma1_mask_template[TEMPL_CHECKSUM].field.enc = 0;
		dp_info->dma1_mask_template[TEMPL_CHECKSUM].field.dec = 0;
		dp_info->dma1_mask_template[TEMPL_CHECKSUM].field.mpe2 = 0;
	} else /*if(flags & DP_F_DIRECT ) */{/*always with pmac*/
		/*normal dirctpath without checksum support */
		dp_info->pmac_template[TEMPL_NORMAL].port_map_en = 0;
		dp_info->pmac_template[TEMPL_NORMAL].sppid = portid;
		dp_info->pmac_template[TEMPL_NORMAL].redirect = 0;
		dp_info->pmac_template[TEMPL_NORMAL].port_map = 0xff;
		dp_info->pmac_template[TEMPL_NORMAL].port_map2 = 0xff;
		dp_info->pmac_template[TEMPL_NORMAL].class_en = 1;
		dp_info->dma1_template[TEMPL_NORMAL].field.enc = 1;
		dp_info->dma1_template[TEMPL_NORMAL].field.dec = 1;
		dp_info->dma1_template[TEMPL_NORMAL].field.mpe2 = 0;
		dp_info->dma1_mask_template[TEMPL_NORMAL].field.enc = 0;
		dp_info->dma1_mask_template[TEMPL_NORMAL].field.dec = 0;
		dp_info->dma1_mask_template[TEMPL_NORMAL].field.mpe2 = 0;

		/* directpath with checksum support, used only for HW Litepath */
		dp_info->pmac_template[TEMPL_CHECKSUM].port_map_en = 0;
		dp_info->pmac_template[TEMPL_CHECKSUM].sppid = portid;
		dp_info->pmac_template[TEMPL_CHECKSUM].redirect = 0;
		dp_info->pmac_template[TEMPL_CHECKSUM].tcp_chksum = 1;
		dp_info->pmac_template[TEMPL_CHECKSUM].class_en = 1;
		dp_info->pmac_template[TEMPL_CHECKSUM].port_map = 0xff;
		dp_info->pmac_template[TEMPL_CHECKSUM].port_map2 = 0xff;
		RESET_PMAC_PORTMAP(&dp_info->pmac_template[TEMPL_CHECKSUM],
				 portid);
		dp_info->dma1_template[TEMPL_CHECKSUM].field.enc = 1;
		dp_info->dma1_template[TEMPL_CHECKSUM].field.dec = 1;
		dp_info->dma1_template[TEMPL_CHECKSUM].field.mpe2 = 1;
		dp_info->dma1_mask_template[TEMPL_CHECKSUM].field.enc = 0;
		dp_info->dma1_mask_template[TEMPL_CHECKSUM].field.dec = 0;
		dp_info->dma1_mask_template[TEMPL_CHECKSUM].field.mpe2 = 0;
	}
}

static void dump_rx_dma_desc(struct dma_rx_desc_0 *desc_0,
			     struct dma_rx_desc_1 *desc_1,
			     struct dma_rx_desc_2 *desc_2,
			     struct dma_rx_desc_3 *desc_3)
{
	if (!desc_0 || !desc_1 || !desc_2 || !desc_3) {
		pr_err("rx desc_0/1/2/3 NULL\n");
		return;
	}
	pr_info(" DMA Descripotr:D0=0x%08x D1=0x%08x D2=0x%08x D3=0x%08x\n",
		*(u32 *)desc_0, *(u32 *)desc_1,
		*(u32 *)desc_2, *(u32 *)desc_3);
	pr_info("  DW0:%s=%d tunl_id=%d flow_id=%d eth_type=%d subif=0x%04x\n",
		"resv0", desc_0->field.resv0,
		desc_0->field.tunnel_id,
		desc_0->field.flow_id, desc_0->field.eth_type,
		desc_0->field.dest_sub_if_id);
	pr_info("  DW1:sess=%d tcp_err=%d nat=%d dec=%d enc=%d mpe2/1=%d/%d\n",
		desc_1->field.session_id, desc_1->field.tcp_err,
		desc_1->field.nat, desc_1->field.dec, desc_1->field.enc,
		desc_1->field.mpe2, desc_1->field.mpe1);
	pr_info("      color=%02d ep=%02d resv1=%d classid=%02d\n",
		desc_1->field.color, desc_1->field.ep, desc_1->field.resv1,
		desc_1->field.classid);
	pr_info("  DW2:data_ptr=0x%08x\n", desc_2->field.data_ptr);
	pr_info("  DW3:own=%d c=%d sop=%d eop=%d dic=%d pdu_type=%d\n",
		desc_3->field.own, desc_3->field.c, desc_3->field.sop,
		desc_3->field.eop, desc_3->field.dic, desc_3->field.pdu_type);
	pr_info("      offset=%d atm_q=%d mpoa_pt=%d mpoa_mode=%d len=%d\n",
		desc_3->field.byte_offset, desc_3->field.qid,
		desc_3->field.mpoa_pt, desc_3->field.mpoa_mode,
		desc_3->field.data_len);
}

static void dump_tx_dma_desc(struct dma_tx_desc_0 *desc_0,
			     struct dma_tx_desc_1 *desc_1,
			     struct dma_tx_desc_2 *desc_2,
			     struct dma_tx_desc_3 *desc_3)
{
	int lookup;

	if (!desc_0 || !desc_1 || !desc_2 || !desc_3) {
		pr_err("tx desc_0/1/2/3 NULL\n");
		return;
	}
	pr_info(" DMA Descripotr:D0=0x%08x D1=0x%08x D2=0x%08x D3=0x%08x\n",
		*(u32 *)desc_0, *(u32 *)desc_1,
		*(u32 *)desc_2, *(u32 *)desc_3);
	pr_info("  DW0:%s=%d tunl_id=%d flow_id=%d eth_type=%d subif=0x%04x\n",
		"resv0", desc_0->field.resv0,
		desc_0->field.tunnel_id,
		desc_0->field.flow_id, desc_0->field.eth_type,
		desc_0->field.dest_sub_if_id);
	pr_info("  DW1:sess=%d tcp_err=%d nat=%d dec=%d enc=%d mpe2/1=%d/%d\n",
		desc_1->field.session_id, desc_1->field.tcp_err,
		desc_1->field.nat, desc_1->field.dec, desc_1->field.enc,
		desc_1->field.mpe2, desc_1->field.mpe1);
	pr_info("  color=%02d ep=%02d resv1=%d classid=%02d\n",
		desc_1->field.color, desc_1->field.ep, desc_1->field.resv1,
		desc_1->field.classid);
	pr_info("  DW2:data_ptr=0x%08x\n", desc_2->field.data_ptr);
	pr_info("  DW3:own=%d c=%d sop=%d eop=%d dic=%d pdu_type=%d\n",
		desc_3->field.own, desc_3->field.c, desc_3->field.sop,
		desc_3->field.eop, desc_3->field.dic, desc_3->field.pdu_type);
	pr_info("  offset=%d atm_qid=%d mpoa_pt=%d mpoa_mode=%d len=%d\n",
		desc_3->field.byte_offset, desc_3->field.qid,
		desc_3->field.mpoa_pt, desc_3->field.mpoa_mode,
		desc_3->field.data_len);
	lookup = ((desc_0->field.flow_id >> 6) << 12) |
		((desc_1->field.dec) << 11) |
		((desc_1->field.enc) << 10) |
		((desc_1->field.mpe2) << 9) |
		((desc_1->field.mpe1) << 8) |
		((desc_1->field.ep) << 4) |
		((desc_1->field.classid) << 0);
	pr_info("  lookup index=0x%x qid=%d\n", lookup,
		get_lookup_qid_via_index(lookup));
}

static void dump_rx_pmac(struct pmac_rx_hdr *pmac)
{
	int i, l;
	unsigned char *p = (char *)pmac;
	unsigned char buf[100];

	if (!pmac) {
		pr_err(" pmac NULL ??\n");
		return;
	}

	l = sprintf(buf, "PMAC at 0x%p: ", p);
	for (i = 0; i < 8; i++)
		l += sprintf(buf + l, "0x%02x ", p[i]);
	l += sprintf(buf + l, "\n");
	pr_info("%s", buf);

	/*byte 0 */
	pr_info("  byte 0:res=%d ver_done=%d ip_offset=%d\n", pmac->res1,
		pmac->ver_done, pmac->ip_offset);
	/*byte 1 */
	pr_info("  byte 1:tcp_h_offset=%d tcp_type=%d\n", pmac->tcp_h_offset,
		pmac->tcp_type);
	/*byte 2 */
	pr_info("  byte 2:ppid=%d class=%d\n", pmac->sppid, pmac->class);
	/*byte 3 */
	pr_info("  byte 3:res=%d pkt_type=%d\n", pmac->res2, pmac->pkt_type);
	/*byte 4 */
	pr_info("  byte 4:res=%d redirect=%d res2=%d src_sub_inf_id=%d\n",
		pmac->res3, pmac->redirect, pmac->res4, pmac->src_sub_inf_id);
	/*byte 5 */
	pr_info("  byte 5:src_sub_inf_id2=%d\n", pmac->src_sub_inf_id2);
	/*byte 6 */
	pr_info("  byte 6:port_map=%d\n", pmac->port_map);
	/*byte 7 */
	pr_info("  byte 7:port_map2=%d\n", pmac->port_map2);
}

static void dump_tx_pmac(struct pmac_tx_hdr *pmac)
{
	int i, l;
	unsigned char *p = (char *)pmac;
	unsigned char buf[100];

	if (!pmac) {
		pr_err("dump_tx_pmac pmac NULL ??\n");
		return;
	}

	l = sprintf(buf, "PMAC at 0x%p: ", p);
	for (i = 0; i < 8; i++)
		l += sprintf(buf + l, "0x%02x ", p[i]);
	sprintf(buf + l, "\n");
	pr_info("%s", buf);
	/*byte 0 */
	pr_info("  byte 0:tcp_chksum=%d res=%d ip_offset=%d\n",
		pmac->tcp_chksum, pmac->res1, pmac->ip_offset);
	/*byte 1 */
	pr_info("  byte 1:tcp_h_offset=%d tcp_type=%d\n", pmac->tcp_h_offset,
		pmac->tcp_type);
	/*byte 2 */
	pr_info("  byte 2:ppid=%d res=%d\n", pmac->sppid, pmac->res);
	/*byte 3 */
	pr_info("  byte 3:%s=%d %s=%d/%d time_dis=%d class_en=%d pkt_type=%d\n",
		"map_en", pmac->port_map_en,
		"res", pmac->res2, pmac->res3,
		pmac->time_dis, pmac->class_en,
		pmac->pkt_type);
	/*byte 4 */
	pr_info("  byte 4:fcs_ins_dis=%d redirect=%d time_stmp=%d subif=%d\n",
		pmac->fcs_ins_dis, pmac->redirect, pmac->time_stmp,
		pmac->src_sub_inf_id);
	/*byte 5 */
	pr_info("  byte 5:src_sub_inf_id2=%d\n", pmac->src_sub_inf_id2);
	/*byte 6 */
	pr_info("  byte 6:port_map=%d\n", pmac->port_map);
	/*byte 7 */
	pr_info("  byte 7:port_map2=%d\n", pmac->port_map2);
}

static void mib_init(u32 flag)
{
#if IS_ENABLED(CONFIG_INTEL_DATAPATH_MIB)
	dp_mib_init(0);
#endif
	gsw_mib_reset_30(0, 0); /* GSW L */
	gsw_mib_reset_30(1, 0); /* GSW R */
	cbm_counter_mode_set(0, 1); /*enqueue to byte */
	cbm_counter_mode_set(1, 1); /*dequeue to byte */
}

void dp_sys_mib_reset_30(u32 flag)
{
#if IS_ENABLED(CONFIG_INTEL_DATAPATH_MIB)
	dp_reset_sys_mib(0);
#else
	gsw_mib_reset_30(0, 0); /* GSW L */
	gsw_mib_reset_30(1, 0); /* GSW R */
	dp_clear_all_mib_inside(0);
#if IS_ENABLED(CONFIG_LTQ_TMU)
	tmu_reset_mib_all(flag);
#endif
#endif
}

static int dp_platform_set(int inst, u32 flag)
{
	if (!inst)/*only inst zero need DMA descriptor */
		init_dma_desc_mask();

	if (!dp_port_prop[inst].ops[0] ||
	    !dp_port_prop[inst].ops[1]) {
		pr_err("Why gswip handle zero?\n");
		return -1;
	}
		if (!inst)
			dp_sub_proc_install_30();
	if (!inst)
		mib_init(0);
	dp_get_gsw_parser_30(NULL, NULL, NULL, NULL);
#if IS_ENABLED(CONFIG_INTEL_DATAPATH_CPUFREQ)
	if (!inst) {
		dp_coc_cpufreq_init();
		if (flag == DP_PLATFORM_DE_INIT)
			dp_coc_cpufreq_exit();
	}
#endif

	return 0;
}

static int dev_platform_set(int inst, u8 ep, struct dp_dev_data *data,
			    u32 flags)
{
	return 0;
}

static int port_platform_set(int inst, u8 ep, struct dp_port_data *data,
			     u32 flags)
{
	int idx, i;
	u32 dma_chan, dma_ch_base;
	struct pmac_port_info *port_info = get_dp_port_info(inst, ep);

	port_info->ctp_max = MAX_SUBIF_PER_PORT;
	port_info->subif_max = MAX_SUBIF_PER_PORT;
	port_info->vap_offset = 8;
	port_info->vap_mask = 0xF;
	idx = port_info->deq_port_base;
	dma_chan =  port_info->dma_chan;
	dma_ch_base = port_info->dma_ch_base;
	for (i = 0; i < port_info->deq_port_num; i++) {
		dp_deq_port_tbl[inst][i + idx].dp_port = ep;

		/* For G.INT num_dma_chan 8 or 16, for other 1 */
		if (port_info->num_dma_chan > 1) {
			dp_deq_port_tbl[inst][i + idx].dma_chan = dma_chan++;
			dp_deq_port_tbl[inst][i + idx].dma_ch_offset =
								dma_ch_base + i;
		} else if (port_info->num_dma_chan == 1) {
			dp_deq_port_tbl[inst][i + idx].dma_chan = dma_chan;
			dp_deq_port_tbl[inst][i + idx].dma_ch_offset =
								dma_ch_base;
		}
		DP_DEBUG(DP_DBG_FLAG_DBG, "deq_port_tbl[%d][%d].dma_chan=%x\n",
			 inst, (i + idx), dma_chan);
	}
#if IS_ENABLED(CONFIG_INTEL_DATAPATH_MIB)
	if (flags & DP_F_DEREGISTER) {
		reset_gsw_itf(ep);
		return 0;
	}

	port_info->itf_info = get_free_itf(ep, flags);
	return 0;
#else
	return 0;
#endif
}

static int supported_logic_dev(int inst, struct net_device *dev,
			       char *subif_name)
{
	return is_vlan_dev(dev);
}

static int subif_hw_set(int inst, int portid, int subif_ix,
			struct subif_platform_data *data, u32 flags)
{
	int deq_port_idx = 0, cqe_deq, dma_ch_offset = 0;
	struct pmac_port_info *port_info;

	if (!data || !data->subif_data) {
		pr_err("data NULL or subif_data NULL\n");
		return -1;
	}
	if (!dp_dma_chan_tbl[inst]) {
		pr_err("dp_dma_chan_tbl[%d] NULL\n", inst);
		return DP_FAILURE;
	}
	port_info = get_dp_port_info(inst, portid);
	if (data->subif_data)
		deq_port_idx = data->subif_data->deq_port_idx;
	if (port_info->deq_port_num < deq_port_idx + 1) {
		pr_err("Wrong deq_port_idx(%d), should < %d\n",
		       deq_port_idx, port_info->deq_port_num);
		return -1;
	}
	cqe_deq = port_info->deq_port_base + deq_port_idx;
	dma_ch_offset = dp_deq_port_tbl[inst][cqe_deq].dma_ch_offset;
	get_dp_port_subif(port_info, subif_ix)->cqm_deq_port[0] = cqe_deq;
	dp_deq_port_tbl[inst][cqe_deq].ref_cnt++;
	if (port_info->num_dma_chan)
		atomic_inc(&(dp_dma_chan_tbl[inst] + dma_ch_offset)->ref_cnt);
	DP_DEBUG(DP_DBG_FLAG_REG, "cbm[%d].ref_cnt=%d tx_dma_chan ref=%d\n",
		 cqe_deq,
		 dp_deq_port_tbl[inst][cqe_deq].ref_cnt,
		 atomic_read(&(dp_dma_chan_tbl[inst] +
			     dma_ch_offset)->ref_cnt));
	return 0;
}

static int subif_hw_reset(int inst, int portid, int subif_ix,
			  struct subif_platform_data *data, u32 flags)
{
	int deq_port_idx = 0, cqe_deq, dma_ch_offset;
	struct pmac_port_info *port_info;

	if (!data || !data->subif_data) {
		pr_err("data NULL or subif_data NULL\n");
		return -1;
	}
	if (!dp_dma_chan_tbl[inst]) {
		pr_err("dp_dma_chan_tbl[%d] NULL\n", inst);
		return DP_FAILURE;
	}
	port_info = get_dp_port_info(inst, portid);
	if (data->subif_data)
		deq_port_idx = data->subif_data->deq_port_idx;
	if (port_info->deq_port_num < deq_port_idx + 1) {
		pr_err("Wrong deq_port_idx(%d), should < %d\n",
		       deq_port_idx, port_info->deq_port_num);
		return -1;
	}
	cqe_deq = port_info->deq_port_base + deq_port_idx;
	dma_ch_offset = dp_deq_port_tbl[inst][cqe_deq].dma_ch_offset;
	if (!dp_deq_port_tbl[inst][cqe_deq].ref_cnt) {
		pr_err("Wrong cbm[%d].ref_cnt=%d\n",
		       cqe_deq,
		       dp_deq_port_tbl[inst][cqe_deq].ref_cnt);
		return -1;
	}
	dp_deq_port_tbl[inst][cqe_deq].ref_cnt--;
	if (port_info->num_dma_chan)
		atomic_dec(&(dp_dma_chan_tbl[inst] + dma_ch_offset)->ref_cnt);
	DP_DEBUG(DP_DBG_FLAG_REG, "cbm[%d].ref_cnt=%d tx_dma_chan ref=%d\n",
		 cqe_deq,
		 dp_deq_port_tbl[inst][cqe_deq].ref_cnt,
		 atomic_read(&(dp_dma_chan_tbl[inst] +
			     dma_ch_offset)->ref_cnt));
	return 0;
}

static int subif_platform_set(int inst, int portid, int subif_ix,
			      struct subif_platform_data *data, u32 flags)
{
	if (flags & DP_F_DEREGISTER)
		return subif_hw_reset(inst, portid, subif_ix, data, flags);
	return subif_hw_set(inst, portid, subif_ix, data, flags);
}

static int subif_platform_set_unexplicit(int inst, int port_id,
					 struct logic_dev *dev,
					 u32 flag)
{
	return 0;
}

static int not_valid_rx_ep(int ep)
{
	return (((ep >= 1) && (ep <= 6)) || (ep >= 15));
}

static void set_pmac_subif(struct pmac_tx_hdr *pmac, int32_t subif)
{
	pmac->src_sub_inf_id2 = subif & 0xff;
	pmac->src_sub_inf_id =  (subif >> 8) & 0x1f;
}

static void update_port_vap(int inst, u32 *ep, int *vap,
			    struct sk_buff *skb,
			    struct pmac_rx_hdr *pmac, char *decryp)
{
	struct pmac_port_info *pi;

	*ep = pmac->sppid; /*get the port_id from pmac's sppid */
	pi = get_dp_port_info(inst, *ep);
	if (pi->alloc_flags & DP_F_LOOPBACK) {
		*ep = GET_VAP((u32)pmac->src_sub_inf_id2 +
			(u32)(pmac->src_sub_inf_id << 8),
			pi->vap_offset, pi->vap_mask);
		*vap = 0;
		*decryp = 1;
	} else
		*vap = GET_VAP((u32)pmac->src_sub_inf_id2 +
			(u32)(pmac->src_sub_inf_id << 8),
			pi->vap_offset, pi->vap_mask);
}

static void get_dma_pmac_templ(int index, struct pmac_tx_hdr *pmac,
			       struct dma_tx_desc_0 *desc_0,
			       struct dma_tx_desc_1 *desc_1,
			       struct pmac_port_info *dp_info)
{
	if (likely(pmac))
		memcpy(pmac, &dp_info->pmac_template[index], sizeof(*pmac));
	desc_1->all = (desc_1->all & dp_info->dma1_mask_template[index].all) |
				dp_info->dma1_template[index].all;
}

static int check_csum_cap(void)
{
	return 1;
}

static int get_itf_start_end(struct gsw_itf *itf_info, u16 *start, u16 *end)
{
	if (!itf_info)
		return -1;
	if (start)
		*start = itf_info->start;
	if (end)
		*end = itf_info->end;

	return 0;
}

int register_dp_cap_gswip30(int flag)
{
	struct dp_hw_cap cap;

	memset(&cap, 0, sizeof(cap));
	cap.info.type = GSWIP30_TYPE;
	cap.info.ver = GSWIP30_VER;
	cap.info.dp_platform_set = dp_platform_set;
	cap.info.port_platform_set = port_platform_set;
	cap.info.dev_platform_set = dev_platform_set;
	cap.info.subif_platform_set_unexplicit = subif_platform_set_unexplicit;
	cap.info.proc_print_ctp_bp_info = NULL;
	cap.info.init_dma_pmac_template = init_dma_pmac_template;
	//dp.info.port_platform_set_unexplicit = port_platform_set_unexplicit;
	cap.info.subif_platform_set = subif_platform_set;
	cap.info.init_dma_pmac_template = init_dma_pmac_template;
	cap.info.not_valid_rx_ep = not_valid_rx_ep;
	cap.info.set_pmac_subif = set_pmac_subif;
	cap.info.update_port_vap = update_port_vap;
	cap.info.check_csum_cap = check_csum_cap;
	cap.info.get_dma_pmac_templ = get_dma_pmac_templ;
	cap.info.get_itf_start_end = get_itf_start_end;
	cap.info.dump_rx_dma_desc = dump_rx_dma_desc;
	cap.info.dump_tx_dma_desc = dump_tx_dma_desc;
	cap.info.dump_rx_pmac = dump_rx_pmac;
	cap.info.dump_tx_pmac = dump_tx_pmac;
	cap.info.supported_logic_dev = supported_logic_dev;
	cap.info.dp_pmac_set = dp_pmac_set_30;
	cap.info.dp_set_gsw_parser = dp_set_gsw_parser_30;
	cap.info.dp_get_gsw_parser = dp_get_gsw_parser_30;
	cap.info.dp_rx = dp_rx_30;
	cap.info.dp_tx = dp_xmit_30;
#if IS_ENABLED(CONFIG_INTEL_DATAPATH_HAL_GSWIP30_MIB)
	cap.info.dp_get_port_vap_mib = dp_get_port_vap_mib_30;
	cap.info.dp_clear_netif_mib = dp_clear_netif_mib_30;
#endif
#if IS_ENABLED(CONFIG_INTEL_DATAPATH_CPUFREQ)
	cap.info.dp_handle_cpufreq_event = dp_handle_cpufreq_event_30;
#endif
	cap.info.cap.tx_hw_chksum = 1;
	cap.info.cap.rx_hw_chksum = 1;
	cap.info.cap.hw_tso = 1;
	cap.info.cap.hw_gso = 1;
	strncpy(cap.info.cap.qos_eng_name, "TMU",
		sizeof(cap.info.cap.qos_eng_name));
	strncpy(cap.info.cap.pkt_eng_name, "PAE/MPE",
		sizeof(cap.info.cap.pkt_eng_name));
	cap.info.cap.max_num_queues = 128;
	cap.info.cap.max_num_scheds = 128;
	cap.info.cap.max_num_deq_ports = 24;
	cap.info.cap.max_num_qos_ports = 24;
	cap.info.cap.max_num_dp_ports = PMAC_MAX_NUM;
	cap.info.cap.max_num_subif_per_port = 16;
	cap.info.cap.max_num_subif = 256;
	cap.info.cap.max_num_bridge_port = 0;

	if (register_dp_hw_cap(&cap, flag)) {
		pr_err("Why register_dp_hw_cap fail\n");
		return -1;
	}

	return 0;
}

