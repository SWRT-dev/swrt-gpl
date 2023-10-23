/*
 * pp_port_tests.c
 * Description: Packet Processor unit tests
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2018-2020 Intel Corporation
 */
#include <linux/types.h>
#include <linux/pp_api.h>
#include "pput.h"
#include "pp_common.h"
#include "pp_port_mgr.h"
#include "pp_buffer_mgr.h"
#include "pp_qos_utils.h"
#include "port_dist.h"
#include "rx_dma.h"
#include "rpb.h"

#ifdef pr_fmt
#undef pr_fmt
#define pr_fmt(fmt) "[PP_PORT_TEST] %s:%d: " fmt, __func__, __LINE__
#endif

static int port_manager_port_add_test(void *unused);
static int port_manager_port_del_test(void *unused);
static int port_manager_hostif_api_test(void *unused);
static int port_distributor_test(void *unused);
static int rx_dma_test(void *unused);
static int rpb_configuration_test(void *unused);
static int rpb_invalid_configuration_test(void *unused);

static struct pp_test_t pp_port_tests[] = {
	{
		.component    = PP_PORT_TESTS,
		.test_name    = "port_manager_port_add_test",
		.level_bitmap = PP_UT_LEVEL_FULL |
				PP_UT_LEVEL_MUST_PASS |
				PP_UT_LEVEL_MUST_RUN,
		.test_data    = NULL,
		.fn           = port_manager_port_add_test,
	},
	{
		.component    = PP_PORT_TESTS,
		.test_name    = "port_manager_hostif_api_test",
		.level_bitmap = PP_UT_LEVEL_FULL |
				PP_UT_LEVEL_MUST_PASS |
				PP_UT_LEVEL_MUST_RUN,
		.test_data    = NULL,
		.fn           = port_manager_hostif_api_test,
	},
	{
		.component    = PP_PORT_TESTS,
		.test_name    = "port_manager_port_del_test",
		.level_bitmap = PP_UT_LEVEL_FULL |
				PP_UT_LEVEL_MUST_PASS |
				PP_UT_LEVEL_MUST_RUN,
		.test_data    = NULL,
		.fn           = port_manager_port_del_test,
	},
	{
		.component    = PP_PORT_TESTS,
		.test_name    = "port_distributor_test",
		.level_bitmap = PP_UT_LEVEL_FULL |
				PP_UT_LEVEL_MUST_PASS |
				PP_UT_LEVEL_MUST_RUN,
		.test_data    = NULL,
		.fn           = port_distributor_test,
	},
	{
		.component    = PP_PORT_TESTS,
		.test_name    = "rx_dma_test",
		.level_bitmap = PP_UT_LEVEL_FULL |
				PP_UT_LEVEL_MUST_PASS |
				PP_UT_LEVEL_MUST_RUN,
		.test_data    = NULL,
		.fn           = rx_dma_test,
	},
	{
		.component    = PP_PORT_TESTS,
		.test_name    = "rpb_configuration_test",
		.level_bitmap = PP_UT_LEVEL_FULL |
				PP_UT_LEVEL_MUST_PASS |
				PP_UT_LEVEL_MUST_RUN,
		.test_data     = NULL,
		.fn           = rpb_configuration_test,
	},
	{
		.component    = PP_PORT_TESTS,
		.test_name    = "rpb_invalid_config_test",
		.level_bitmap = PP_UT_LEVEL_FULL |
				PP_UT_LEVEL_MUST_PASS |
				PP_UT_LEVEL_MUST_RUN,
		.test_data     = NULL,
		.fn           = rpb_invalid_configuration_test,
	},
};

/* TODO: use free port */
#define PP_TEST_PORT 254
/**
 * @brief port manager test, testing the add port API
 * @param unused
 * @return int PP_UT_PASS if test pass, PP_UT_FAIL otherwise
 */
static int port_manager_port_add_test(void *unused)
{
	struct pp_port_cfg port;

	memset(&port, 0, sizeof(struct pp_port_cfg));

	port.rx.cls.n_flds = 2;              /* 2 port clasification fields */
	port.rx.cls.cp[0].stw_off = 10;      /* fld0 status word off */
	port.rx.cls.cp[0].copy_size = 8;     /* fld0 copy size */
	port.rx.cls.cp[1].stw_off = 22;      /* fld1 status word off */
	port.rx.cls.cp[1].copy_size = 6;     /* fld1 copy size */
	port.rx.mem_port_en = 1;             /* memory port */
	port.rx.flow_ctrl_en = 1;            /* flow control */
	port.rx.parse_type = L2_PARSE;       /* full parsing */

	port.tx.max_pkt_size = 2000;         /* max 2000 */
	port.tx.headroom_size = 256;         /* buffers headroom */
	port.tx.tailroom_size = 16;          /* buffer tailroom */
	port.tx.min_pkt_len = PP_MIN_TX_PKT_LEN_NONE;/* 0 */
	port.tx.base_policy = 0;             /* set base policy to 0 */
	port.tx.policies_map = (u8)BIT(0);   /* set policy 0 bit */
	port.tx.pkt_only_en = false;         /* packet+descriptor */
	port.tx.seg_en = false;              /* no segmentation */

	if (unlikely(pmgr_port_add(PP_TEST_PORT, &port, false))) {
		pr_err("pp_port_add failed, port%u\n", PP_TEST_PORT);
		return PP_UT_FAIL;
	}

	pr_debug("pp_port_add done\n");
	return PP_UT_PASS;
}

/**
 * @brief port manager test, testing the del port API
 * @param unused
 * @return int PP_UT_PASS if test pass, PP_UT_FAIL otherwise
 */
static int port_manager_port_del_test(void *unused)
{
	if (unlikely(pmgr_port_del(PP_TEST_PORT, false))) {
		pr_err("pp_port_del failed, port%u\n", PP_TEST_PORT);
		return PP_UT_FAIL;
	}

	return PP_UT_PASS;
}

/**
 * @brief port manager test, testing the add/del hostif API
 * @param unused
 * @return int PP_UT_PASS if test pass, PP_UT_FAIL otherwise
 */
static int port_manager_hostif_api_test(void *unused)
{
	struct pp_qos_dev *qdev;
	struct pp_qos_queue_conf confq;
	struct pp_qos_port_conf conf;
	struct pp_hif_datapath dp, dflt_dp;
	struct pp_hostif_cfg hif;
	u32 i;
	s32 err, ret = PP_UT_PASS;
	u32 qos_queue[5];
	u32 qos_port;

	qdev = pp_qos_dev_open(PP_QOS_INSTANCE_ID);
	if (unlikely(ptr_is_null(qdev)))
		return PP_UT_FAIL;

	err = pp_qos_port_allocate(qdev, PP_TEST_PORT, &qos_port);
	if (err) {
		pr_err("pp_qos_port_allocate failed\n");
		return PP_UT_FAIL;
	}

	pp_qos_port_conf_set_default(&conf);
	conf.port_parent_prop.arbitration = PP_QOS_ARBITRATION_WRR;
	conf.ring_address = 1;
	conf.ring_size = 1;
	err = pp_qos_port_set(qdev, qos_port, &conf);
	if (err) {
		pr_err("pp_qos_port_set failed\n");
		return PP_UT_FAIL;
	}

	pp_qos_queue_conf_set_default(&confq);
	confq.queue_child_prop.parent = qos_port;
	for (i = 0 ; i < 5 ; i++) {
		err = pp_qos_queue_allocate(qdev, &qos_queue[i]);
		if (err) {
			pr_err("pp_qos_queue_allocate failed\n");
			return PP_UT_FAIL;
		}
		err = pp_qos_queue_set(qdev, qos_queue[i], &confq);
		if (err) {
			pr_err("pp_qos_queue_set failed\n");
			return PP_UT_FAIL;
		}
	}

	/* Save default to set it back later */
	err = pp_hostif_dflt_get(&dflt_dp);
	if (unlikely(err)) {
		pr_err("pp_hostif_dflt_get failed\n");
		return PP_UT_FAIL;
	}

	memset(&dp, 0, sizeof(struct pp_hif_datapath));

	/* Prepare the new default hostif dp */
	dp.eg[0].qos_q = qos_queue[4];
	dp.eg[0].pid = PP_TEST_PORT;
	for (i = 1; i < PP_HOSTIF_EG_MAX; i++) {
		dp.eg[i].qos_q = PP_QOS_INVALID_ID;
		dp.eg[i].pid = PP_PORT_INVALID;
	}
	dp.color = PP_COLOR_GREEN;
	for (i = 0; i < PP_SI_SGC_MAX; i++)
		dp.sgc[i] = PP_SGC_INVALID;
	for (i = 0; i < PP_SI_TBM_MAX; i++)
		dp.tbm[i] = PP_TBM_INVALID;
	/* Set the new default hostif dp */
	err = pmgr_hostif_dflt_set(&dp, false);
	if (unlikely(err)) {
		pr_err("pp_hostif_def_set failed\n");
		return PP_UT_FAIL;
	}

	/* Prepare the new hostif */
	hif.cls.port       = PP_TEST_PORT;
	hif.cls.tc_bitmap  = 0xF;
	hif.dp.eg[0].qos_q = qos_queue[0];
	hif.dp.eg[0].pid   = PP_TEST_PORT;
	hif.dp.eg[1].qos_q = qos_queue[1];
	hif.dp.eg[1].pid   = PP_TEST_PORT;
	hif.dp.eg[2].qos_q = qos_queue[2];
	hif.dp.eg[2].pid   = PP_TEST_PORT;
	hif.dp.eg[3].qos_q = qos_queue[3];
	hif.dp.eg[3].pid   = PP_TEST_PORT;
	hif.dp.color       = PP_COLOR_GREEN;
	for (i = 0; i < PP_SI_SGC_MAX; i++)
		hif.dp.sgc[i] = PP_SGC_INVALID;
	for (i = 0; i < PP_SI_TBM_MAX; i++)
		hif.dp.tbm[i] = PP_TBM_INVALID;

	/* Set the new hostif */
	err = pmgr_hostif_add(&hif, false);
	if (unlikely(err)) {
		pr_err("failed to add new pp to host interface\n");
		ret = PP_UT_FAIL;
		goto revert;
	}

	/* Update the hostif */
	memcpy(&dp, &hif.dp, sizeof(dp));
	dp.color = PP_COLOR_YELLOW;
	err = pmgr_hostif_update(&hif, &dp, false);
	if (unlikely(err)) {
		pr_err("failed to update pp to host interface\n");
		ret = PP_UT_FAIL;
	}

	/* try to delete the hostif with a different param,
	 * this operation should fail
	 */
	hif.dp.eg[3].qos_q = PP_QOS_INVALID_ID;
	err = pmgr_hostif_del(&hif, false);
	if (unlikely(!err)) {
		pr_err("pp_hostif_del wasn't failed with wrong parameters\n");
		ret = PP_UT_FAIL;
	}

	/* return back the configured host interface parameters */
	hif.dp.color = PP_COLOR_YELLOW;
	hif.dp.eg[3].qos_q = qos_queue[3];

	/* try to delete the hostif with the original parameters,
	 * this operation should pass
	 */
	err = pmgr_hostif_del(&hif, false);
	if (unlikely(err)) {
		pr_err("pp_hostif_del failed with the original parameters\n");
		ret = PP_UT_FAIL;
	}

revert:
	/* Set original default hostif dp */
	err = pmgr_hostif_dflt_set(&dflt_dp, false);
	if (unlikely(err)) {
		pr_err("pmgr_hostif_dflt_set failed\n");
		ret = PP_UT_FAIL;
	}

	for (i = 0 ; i < 5 ; i++) {
		err = pp_qos_queue_remove(qdev, qos_queue[i]);
		if (err) {
			pr_err("pp_qos_queue_remove failed\n");
			return PP_UT_FAIL;
		}
	}

	err = pp_qos_port_remove(qdev, qos_port);
	if (err) {
		pr_err("pp_qos_port_remove failed\n");
		return PP_UT_FAIL;
	}

	return ret;
}

/**
 * @brief port distributor test, testing the get/set port
 *        mapping API's
 * @param unused
 * @return int PP_UT_PASS if test pass, PP_UT_FAIL otherwise
 */
static int port_distributor_test(void *unused)
{
	struct port_dist_map map[PP_MAX_PORT];
	struct port_dist_map tmp_map;
	u32 i;

	/* Save current mapping */
	for (i = 0; i < PP_MAX_PORT; i++) {
		map[i].src_port = i;
		if (unlikely(port_dist_port_map_get(&map[i]))) {
			pr_err("port_dist_port_map_get failed, port %hu\n", i);
			return PP_UT_FAIL;
		}
	}
	/* Set default mapping */
	port_dist_default_set();
	/* Validate default mapping was set */
	for (i = 0; i < PP_MAX_PORT; i++) {
		tmp_map.src_port = i;
		if (unlikely(port_dist_port_map_get(&tmp_map))) {
			pr_err("port_dist_port_map_get failed, port %hu\n", i);
			return PP_UT_FAIL;
		}
		if (unlikely((tmp_map.rpb_port != PORT_DIST_DEFAULT_RPB) ||
			     (tmp_map.tc       != PORT_DIST_DEFAULT_TC))) {
			pr_err("port %hu default val error rpb %hu tc %hu\n",
			       i, tmp_map.rpb_port, tmp_map.tc);
			return PP_UT_FAIL;
		}
	}
	/* Return back original mapping */
	for (i = 0; i < PP_MAX_PORT; i++) {
		if (unlikely(port_dist_port_map_set(&map[i]))) {
			pr_err("port_dist_port_map_set failed, port %hu\n", i);
			return PP_UT_FAIL;
		}
	}
	/* Validate original mapping */
	for (i = 0; i < PP_MAX_PORT; i++) {
		tmp_map.src_port = i;
		if (unlikely(port_dist_port_map_get(&tmp_map))) {
			pr_err("port_dist_port_map_get failed, port %hu\n", i);
			return PP_UT_FAIL;
		}
		if (unlikely((tmp_map.rpb_port != map[i].rpb_port) ||
			     (tmp_map.tc       != map[i].tc))) {
			pr_err("port %hu original val error rpb %hu tc %hu\n",
			       i, map[i].rpb_port, map[i].tc);
			return PP_UT_FAIL;
		}
	}

	return PP_UT_PASS;
}

/**
 * @brief rx_dma test, testing the get/set port
 * @param unused
 * @return int PP_UT_PASS if test pass, PP_UT_FAIL otherwise
 */
static int rx_dma_test(void *unused)
{
	u32    idx;
	struct rx_dma_port_cfg old_port_cfg;
	struct rx_dma_port_cfg conf_pcfg;
	struct rx_dma_port_cfg recv_pcfg;

	conf_pcfg.headroom_size = 16;
	conf_pcfg.tailroom_size = 16;
	conf_pcfg.wr_desc = 1;
	conf_pcfg.pkt_only = 0;
	conf_pcfg.min_pkt_size = PP_MIN_TX_PKT_LEN_NONE;

	for (idx = 0 ; idx < PP_MAX_PORT ; idx++) {
		/* Store current port configuration */
		old_port_cfg.port_id = idx;
		if (rx_dma_port_get(&old_port_cfg)) {
			pr_err("rx_dma_get_port Failed");
			return PP_UT_FAIL;
		}

		/* Test set port */
		conf_pcfg.port_id = idx;
		if (rx_dma_port_set(&conf_pcfg)) {
			pr_err("rx_dma_get_port Failed\n");
			return PP_UT_FAIL;
		}

		/* Test get port */
		recv_pcfg.port_id = idx;
		if (rx_dma_port_get(&recv_pcfg)) {
			pr_err("rx_dma_get_port Failed\n");
			return PP_UT_FAIL;
		}

		/* Verify port configuration changed */
		if ((conf_pcfg.port_id != recv_pcfg.port_id) ||
		    (conf_pcfg.headroom_size != recv_pcfg.headroom_size) ||
		    (conf_pcfg.tailroom_size != recv_pcfg.tailroom_size) ||
		    (conf_pcfg.wr_desc != recv_pcfg.wr_desc) ||
		    (conf_pcfg.pkt_only != recv_pcfg.pkt_only) ||
		    (conf_pcfg.min_pkt_size != recv_pcfg.min_pkt_size)) {
			pr_err("Port configuration differs:\n");
			pr_err("+------------+------------+----------+\n");
			pr_err("|    Field   | Configured | Received |\n");
			pr_err("+------------+------------+----------+\n");
			pr_err("|  Port ID   | %10d | %8d |\n",
			       conf_pcfg.port_id,
			       recv_pcfg.port_id);
			pr_err("|  Headroom  | %10d | %8d |\n",
			       conf_pcfg.headroom_size,
			       recv_pcfg.headroom_size);
			pr_err("|  Tailroom  | %10d | %8d |\n",
			       conf_pcfg.tailroom_size,
			       recv_pcfg.tailroom_size);
			pr_err("|  Wr Desc   | %10d | %8d |\n",
			       conf_pcfg.wr_desc,
			       recv_pcfg.wr_desc);
			pr_err("|  Pkt Only  | %10d | %8d |\n",
			       conf_pcfg.pkt_only,
			       recv_pcfg.pkt_only);
			pr_err("|  Min Pkt   | %10d | %8d |\n",
			       conf_pcfg.min_pkt_size,
			       recv_pcfg.min_pkt_size);
			pr_err("+------------+------------+----------+\n");
			return PP_UT_FAIL;
		}

		/* Restore port configuration */
		if (rx_dma_port_set(&old_port_cfg)) {
			pr_err("rx_dma_get_port Failed\n");
			return PP_UT_FAIL;
		}
	}

	return PP_UT_PASS;
}

/**
 * @brief Test a valid RPB configuration change
 * @param unused
 * @return int PP_UT_PASS if test pass, PP_UT_FAIL otherwise
 */
static int rpb_configuration_test(void *unused)
{
	struct rpb_profile_cfg cfg, res_cfg;
	struct rpb_port_cfg *port, *res_port;
	struct rpb_tc_cfg *tc, *res_tc;
	u32 p, t, res = PP_UT_FAIL;
	s32 ret;

	memset(&cfg,      0, sizeof(cfg));
	memset(&res_cfg,  0, sizeof(res_cfg));

	/* set some configuration */
	cfg.high_thr     = 100000;
	cfg.low_thr      = 50000;
	cfg.pkt_high_thr = 1000;
	cfg.pkt_low_thr  = 500;
	RPB_FOR_EACH_PORT(p) {
		port = &cfg.port[p];
		port->pool_sz  = 20000 * (p+1) / 2;
		port->high_thr = 19000 * (p+1) / 2;
		port->low_thr  = 15000 * (p+1) / 2;
		port->flow_ctrl = true;
		port->is_mem = true;
		port->num_of_pp_ports = 0;
		RPB_FOR_EACH_TC(t) {
			tc = &port->tc[t];
			tc->dpool_sz       = port->pool_sz / 4;
			tc->dpool_high_thr = tc->dpool_sz  / 2;
			tc->dpool_low_thr  = tc->dpool_sz  / 4;
			tc->spool_high_thr = port->pool_sz / 2;
			tc->spool_low_thr  = port->pool_sz / 4;
			tc->hi_prio        = true;
		}
	}
	/* write the configuration */
	ret = rpb_custom_profile_set(&cfg);
	if (unlikely(ret)) {
		pr_err("Fail: Failed to set RPB configuration\n");
		return PP_UT_FAIL;
	}

	/* read the configuration and compare */
	ret = rpb_profile_get(RPB_PROFILE_COSTUM, &res_cfg);
	if (unlikely(ret)) {
		pr_err("Fail: Failed to get RPB configuration\n");
		goto test_fail;
	}

	if (!memcmp(&cfg, &res_cfg, sizeof(cfg))) {
		res = PP_UT_PASS;
		goto done;
	}

	if (cfg.high_thr != res_cfg.high_thr)
		pr_err("Fail: Global high threshold, expected %u, result %u\n",
			cfg.high_thr, res_cfg.high_thr);
	if (cfg.low_thr != res_cfg.low_thr)
		pr_err("Fail: Global low threshold, expected %u, result %u\n",
			cfg.low_thr, res_cfg.low_thr);
	if (cfg.pkt_high_thr != res_cfg.pkt_high_thr)
		pr_err("Fail: Global packets high threshold, expected %u, result %u\n",
			cfg.pkt_high_thr, res_cfg.pkt_high_thr);
	if (cfg.pkt_low_thr != res_cfg.pkt_low_thr)
		pr_err("Fail: Global packets low threshold, expected %u, result %u\n",
			cfg.pkt_low_thr, res_cfg.pkt_low_thr);

	RPB_FOR_EACH_PORT(p) {
		port     = &cfg.port[p];
		res_port = &res_cfg.port[p];
		if (port->high_thr != res_port->high_thr)
			pr_err("Fail: Port %u high threshold, expected %u, result %u\n",
				p, port->high_thr, res_port->high_thr);
		if (port->low_thr != res_port->low_thr)
			pr_err("Fail: Port %u low threshold, expected %u, result %u\n",
				p, port->low_thr, res_port->low_thr);
		if (port->pool_sz != res_port->pool_sz)
			pr_err("Fail: Port %u pool size, expected %u, result %u\n",
				p, port->pool_sz, res_port->pool_sz);

		RPB_FOR_EACH_TC(t) {
			tc     = &port->tc[t];
			res_tc = &res_port->tc[t];
			if (tc->dpool_sz != res_tc->dpool_sz)
				pr_err("Fail: Port %u tc %u pool size, expected %u, result %u\n",
					p, t, tc->dpool_sz, res_tc->dpool_sz);
			if (tc->dpool_high_thr != res_tc->dpool_high_thr)
				pr_err("Fail: Port %u tc %u high watermark, expected %u, result %u\n",
					p, t, tc->dpool_high_thr,
				       res_tc->dpool_high_thr);
			if (tc->dpool_high_thr != res_tc->dpool_high_thr)
				pr_err("Fail: Port %u tc %u high watermark, expected %u, result %u\n",
					p, t, tc->dpool_high_thr,
				       res_tc->dpool_high_thr);
			if (tc->dpool_low_thr != res_tc->dpool_low_thr)
				pr_err("Fail: Port %u tc %u low watermark, expected %u, result %u\n",
					p, t, tc->dpool_low_thr,
				       res_tc->dpool_low_thr);
			if (tc->spool_high_thr != res_tc->spool_high_thr)
				pr_err("Fail: Port %u tc %u high threshold, expected %u, result %u\n",
					p, t, tc->spool_high_thr,
				       res_tc->spool_high_thr);
			if (tc->spool_low_thr != res_tc->spool_low_thr)
				pr_err("Fail: Port %u tc %u low threshold, expected %u, result %u\n",
					p, t, tc->spool_low_thr,
				       res_tc->spool_low_thr);
			if (tc->hi_prio != res_tc->hi_prio)
				pr_err("Fail: Port %u tc %u high priority, expected %u, result %u\n",
					p, t, tc->hi_prio, res_tc->hi_prio);
			if (tc->flow_ctrl != res_tc->flow_ctrl)
				pr_err("Fail: Port %u tc %u flow control, expected %u, result %u\n",
					p, t, tc->flow_ctrl,
				       res_tc->flow_ctrl);
		}
	}

test_fail:
	res = PP_UT_FAIL;

done:
	/* restore configuration */
	ret = rpb_profile_set(RPB_PROFILE_DEFAULT);
	if (unlikely(ret)) {
		pr_err("Fail: Failed to restore RPB configuration\n");
		return PP_UT_FAIL;
	}
	return res;
}

/**
 * @brief Test invalid RPB configuration change
 * @param unused
 * @return int PP_UT_PASS if test pass, PP_UT_FAIL otherwise
 */
static int rpb_invalid_configuration_test(void *unused)
{
	struct rpb_profile_cfg cfg, curr_cfg;
	struct rpb_port_cfg *port;
	struct rpb_tc_cfg *tc;
	u32 ret, p, t, res;

	memset(&cfg,      0, sizeof(cfg));
	memset(&curr_cfg, 0, sizeof(curr_cfg));
	/* read current configuration */
	ret = rpb_profile_get(RPB_PROFILE_DEFAULT, &curr_cfg);
	if (unlikely(ret)) {
		pr_err("Fail: Failed to get default RPB configuration\n");
		return PP_UT_FAIL;
	}

	memcpy(&cfg, &curr_cfg, sizeof(cfg));

	/* set invalid configuration */
	RPB_FOR_EACH_PORT(p) {
		port = &cfg.port[p];
		port->high_thr = port->pool_sz + 1;
		RPB_FOR_EACH_TC(t) {
			tc = &port->tc[t];
			tc->dpool_low_thr = tc->dpool_high_thr + 1;
		}
	}

	/* write the configuration */
	ret = rpb_custom_profile_set(&cfg);
	if (unlikely(!ret)) {
		pr_err("Fail: Set invalid RPB configuration succeeded\n");
		res = PP_UT_FAIL;
		goto done;
	}
	res = PP_UT_PASS;

done:
	/* restore configuration */
	ret = rpb_profile_set(RPB_PROFILE_DEFAULT);
	if (unlikely(ret)) {
		pr_err("Fail: Failed to restore RPB configuration\n");
		return PP_UT_FAIL;
	}
	return res;
}

/**
 * Register all pp_port tests to pput
 */
void pp_port_tests_init(void)
{
	int test_idx;

	/* Add Tests */
	for (test_idx = 0 ; test_idx < ARRAY_SIZE(pp_port_tests); test_idx++)
		pp_register_test(&pp_port_tests[test_idx]);
}
