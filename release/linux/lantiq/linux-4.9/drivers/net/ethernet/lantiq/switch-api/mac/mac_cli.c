/******************************************************************************
 *                Copyright (c) 2016, 2017, 2018 Intel Corporation
 *
 *
 * For licensing information, see the file 'LICENSE' in the root folder of
 * this software module.
 *
 ******************************************************************************/

#include <xgmac.h>
#include <gswss_mac_api.h>
#include <lmac_api.h>
#include <xgmac_mdio.h>
#include <mac_tx_fifo.h>

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(arr)	(sizeof(arr)/sizeof((arr)[0]))
#endif

struct mac_api_cfg {
	u32 cmdType;
	int (*set_func_0)(void *);
	int (*set_func_1)(void *, u32);
	int (*set_func_2)(void *, u32, u32);
	int (*set_func_3)(void *, u32, u32, u32);
	int (*get_func)(void *);

};

static int cli_set_mtl_tx(void *pdev)
{
	struct mac_prv_data *priv_data = GET_MAC_PDATA(pdev);
	GSW_MAC_Cli_t *pdata = priv_data->mac_cli;

	xgmac_set_mtl_tx_mode(pdev, pdata->val[0]);
	xgmac_set_mtl_tx_thresh(pdev, pdata->val[1]);

	return 0;
}

static int cli_set_mtl_rx(void *pdev)
{
	struct mac_prv_data *priv_data = GET_MAC_PDATA(pdev);
	GSW_MAC_Cli_t *pdata = priv_data->mac_cli;

	xgmac_set_mtl_rx_mode(pdev, pdata->val[0]);
	xgmac_set_mtl_rx_thresh(pdev, pdata->val[1]);

	return 0;
}

static int cli_set_int(void *pdev)
{
	struct mac_prv_data *priv_data = GET_MAC_PDATA(pdev);
	GSW_MAC_Cli_t *pdata = priv_data->mac_cli;

	mac_int_enable(pdev);
	xgmac_set_mac_int(pdev, pdata->val[1], 1);
	xgmac_set_mtl_int(pdev, pdata->val[0]);

	return 0;
}

/* MAC REGISTER SETTINGS */
static int cli_set_mac_enable(void *pdev)
{
	struct mac_prv_data *priv_data = GET_MAC_PDATA(pdev);
	GSW_MAC_Cli_t *pdata = priv_data->mac_cli;

	if (pdata->val[0])
		xgmac_powerup(pdev);
	else
		xgmac_powerdown(pdev);

	return 0;
}

static int cli_set_mac_address(void *pdev)
{
	struct mac_prv_data *priv_data = GET_MAC_PDATA(pdev);
	GSW_MAC_Cli_t *pdata = priv_data->mac_cli;

	xgmac_set_mac_address(pdev, pdata->mac_addr);

	return 0;
}

static int cli_set_mac_rx_mode(void *pdev)
{
	struct mac_prv_data *priv_data = GET_MAC_PDATA(pdev);
	GSW_MAC_Cli_t *pdata = priv_data->mac_cli;

	xgmac_set_promiscuous_mode(pdev, pdata->val[0]);
	xgmac_set_all_multicast_mode(pdev, pdata->val[1]);

	return 0;
}

static int cli_mdio_wr(void *pdev)
{
	struct mac_prv_data *priv_data = GET_MAC_PDATA(pdev);
	GSW_MAC_Cli_t *pdata = priv_data->mac_cli;

	xgmac_mdio_single_wr(pdev, pdata->val[0], pdata->val[1], pdata->val[2],
			     pdata->val[3]);

	return 0;
}

static int cli_mdio_get_clause(void *pdev)
{
	struct mac_prv_data *priv_data = GET_MAC_PDATA(pdev);
	GSW_MAC_Cli_t *pdata = priv_data->mac_cli;

	return mdio_get_clause(pdev, pdata->val[1]);
}

static int cli_add_fifo(void *pdev)
{
	struct mac_prv_data *priv_data = GET_MAC_PDATA(pdev);
	GSW_MAC_Cli_t *pdata = priv_data->mac_cli;

	fifo_entry_add(pdev, pdata->val[0], pdata->val[1],
		       pdata->val[2], pdata->val[3], pdata->val[4],
		       pdata->val[5]);

	return 0;
}

static int cli_get_rmon(void *pdev)
{
	struct mac_prv_data *priv_data = GET_MAC_PDATA(pdev);
	GSW_MAC_Cli_t *pdata = priv_data->mac_cli;

	if (!pdata) {
		mac_printf("MAC Cli pointer is NULL\n");
		return -1;
	}

	xgmac_read_mmc_stats(pdev, &pdata->pstats);

	return 0;
}

static int cli_get_systime(void *pdev)
{
	struct mac_prv_data *priv_data = GET_MAC_PDATA(pdev);
	GSW_MAC_Cli_t *pdata = priv_data->mac_cli;

	if (!pdata) {
		mac_printf("MAC Cli pointer is NULL\n");
		return -1;
	}

	pdata->time = xgmac_get_systime(pdev);

	return 0;
}

static int cli_get_txtstamp(void *pdev)
{
	struct mac_prv_data *priv_data = GET_MAC_PDATA(pdev);
	GSW_MAC_Cli_t *pdata = priv_data->mac_cli;

	if (!pdata) {
		mac_printf("MAC Cli pointer is NULL\n");
		return -1;
	}

	pdata->time = xgmac_get_tx_tstamp(pdev);

	return 0;
}

static int cli_get_hwcap(void *pdev)
{
	struct mac_prv_data *priv_data = GET_MAC_PDATA(pdev);
	GSW_MAC_Cli_t *pdata = priv_data->mac_cli;

	if (!pdata) {
		mac_printf("MAC Cli pointer is NULL\n");
		return -1;
	}

	xgmac_get_hw_capability(pdev);

	memcpy(&pdata->hw_feat, &priv_data->hw_feat, sizeof(priv_data->hw_feat));

	return 0;
}

static const struct mac_api_cfg xgmac_cfg[] = {
	{MAC_CLI_INIT, mac_init, 0, 0, 0, 0},
	{MAC_CLI_RESET, 0, mac_reset, 0, 0, 0},
	{MAC_CLI_MTL_TX, cli_set_mtl_tx, 0, 0, 0, 0},
	{MAC_CLI_MTL_RX, cli_set_mtl_rx, 0, 0, 0, 0},
	{MAC_CLI_FCTHR, 0, 0, xgmac_set_flow_control_threshold, 0, 0},
	{MAC_CLI_SET_TS_ADDEND, 0, xgmac_set_tstamp_addend, 0, 0, 0},
	{MAC_CLI_GET_SYSTEM_TIME, 0, 0, 0, 0, cli_get_systime},
	{MAC_CLI_GET_TX_TSTAMP, 0, 0, 0, 0, cli_get_txtstamp},
	{MAC_CLI_GET_TXTSTAMP_CNT, 0, 0, 0, 0, xgmac_get_txtstamp_cnt},
	{MAC_CLI_GET_TXTSTAMP_PKTID, 0, 0, 0, 0, xgmac_get_txtstamp_pktid},
	{MAC_CLI_SET_TSTAMP_EN, mac_enable_ts, 0, 0, 0, 0},
	{MAC_CLI_SET_TSTAMP_DIS, mac_disable_ts, 0, 0, 0, 0},
	{MAC_CLI_TXTSTAMP_MODE, 0, 0, 0, xgmac_ptp_txtstamp_mode, 0},
	{MAC_CLI_HW_TSTAMP, 0, 0, xgmac_set_hwtstamp_settings, 0, 0},
	{MAC_CLI_FLUSH_TX_Q, xgmac_flush_tx_queues, 0, 0, 0, 0},
	{MAC_CLI_ERR_PKT_FWD, 0, 0, xgmac_forward_fup_fep_pkt, 0, 0},
	{MAC_CLI_SET_INT, cli_set_int, 0, 0, 0, 0},
	{MAC_CLI_EN, cli_set_mac_enable, 0, 0, 0, 0},
	{MAC_CLI_SET_MAC_ADDR, cli_set_mac_address, 0, 0, 0, 0},
	{MAC_CLI_SET_RX_MODE, cli_set_mac_rx_mode, 0, 0, 0, 0},
	{MAC_CLI_SET_MTU, 0, mac_set_mtu, 0, 0, mac_get_mtu},
	{MAC_CLI_PFCTRL, 0, mac_set_flowctrl, 0, 0, 0},
	{MAC_CLI_SET_PF_FILTER, 0, xgmac_pause_frame_filtering, 0, 0, 0},
	{MAC_CLI_PAUSE_TX, xgmac_initiate_pause_tx, 0, 0, 0, 0},
	{MAC_CLI_SPEED, 0, mac_set_physpeed, 0, 0, mac_get_speed},
	{MAC_CLI_SET_DPX_MODE, 0, mac_set_duplex, 0, 0, 0},
	{MAC_CLI_CSUM_OFFLOAD, 0, xgmac_set_checksum_offload, 0, 0, 0},
	{MAC_CLI_LB, 0, mac_config_loopback, 0, 0, 0},
	{MAC_CLI_EEE, 0, 0, 0, mac_set_lpien, 0},
	{MAC_CLI_CRC_STRIP, 0, xgmac_set_crc_strip_type, 0, 0, 0},
	{MAC_CLI_CRC_STRIP_ACS, 0, xgmac_set_acs, 0, 0, 0},
	{MAC_CLI_IPG, 0, mac_config_ipg, 0, 0, 0},
	{MAC_CLI_MAGIC_PMT, 0, xgmac_set_magic_pmt, 0, 0, 0},
	{MAC_CLI_PMT_GUCAST, 0, xgmac_set_pmt_gucast, 0, 0, 0},
	{MAC_CLI_EXTCFG, 0, xgmac_set_extcfg, 0, 0, 0},
	{MAC_CLI_SET_RXTXCFG, 0, 0, xgmac_set_mac_rxtx, 0, 0},
	{MAC_CLI_SET_LINKSTS, 0, mac_set_linksts, 0, 0, 0},
	{MAC_CLI_SET_LPITX, 0, xgmac_set_mac_lpitx, 0, 0, 0},
	{MAC_CLI_SET_MDIO_CL, 0, 0, mdio_set_clause, 0, 0},
	{MAC_CLI_SET_MDIO_RD, 0, 0, 0, xgmac_mdio_single_rd, 0},
	{MAC_CLI_SET_MDIO_WR, cli_mdio_wr, 0, 0, 0, 0},
	{MAC_CLI_SET_MDIO_INT, 0, mdio_set_interrupt, 0, 0, 0},
	{MAC_CLI_SET_FCSGEN, 0, mac_set_fcs_gen, 0, 0, 0},
	{MAC_CLI_SET_GINT, 0, xgmac_set_gint, 0, 0, 0},
	{MAC_CLI_SET_RXCRC, 0, xgmac_set_rxcrc, 0, 0, 0},
	{MAC_CLI_GET_FIFO, 0, 0, 0, 0, print_fifo},
	{MAC_CLI_ADD_FIFO, cli_add_fifo, 0, 0, 0, 0},
	{MAC_CLI_DEL_FIFO, 0, fifo_entry_del, 0, 0, 0},
	{MAC_CLI_SET_EXT_TS_SRC, 0, xgmac_set_exttime_source, 0, 0, 0},
	{MAC_CLI_GET_RMON, 0, 0, 0, 0, cli_get_rmon},
	{MAC_CLI_CLEAR_RMON, xgmac_clear_rmon, 0, 0, 0, 0},
	{MAC_CLI_GET_CNTR_CFG, 0, 0, 0, 0, 0},
	{MAC_CLI_GET_HW_FEAT, 0, 0, 0, 0, cli_get_hwcap},
	{MAC_CLI_GET_MDIO_CL, 0, 0, 0, 0, cli_mdio_get_clause},
	{MAC_CLI_REG_RD, 0, xgmac_rd_reg, 0, 0, 0},
	{MAC_CLI_REG_WR, 0, 0, xgmac_wr_reg, 0, 0},
};

int xgmac_cfg_main(GSW_MAC_Cli_t *params)
{
	struct mac_ops *ops = gsw_get_mac_ops(0, MAC_2);
	struct mac_prv_data *pdata = GET_MAC_PDATA(ops);
	int i = 0, ret = 0;
	int num_of_elem = ARRAY_SIZE(xgmac_cfg);

	for (i = 0; i < num_of_elem; i++) {
		if (xgmac_cfg[i].cmdType == params->cmdType) {
			break;
		}
	}

	ops = gsw_get_mac_ops(0, params->mac_idx);
	pdata = GET_MAC_PDATA(ops);
	pdata->set_all = 0;

	pdata->mac_cli = params;

#if 0

	for (k = 0; k < MAX_MAC_CLI_ARG; k++)
		pdata->val[k] = params->val[k];

	for (k = 0; k < 6; k++)
		pdata->mac_addr[k] = params->mac_addr[k];

#endif
#if 0

	printk("%x\n", pdata->mac_cli);
	printk("%x\n", pdata->mac_cli->pstats);
	printk("%x\n", pdata->mac_cli->hw_feat);

	for (k = 0; k < MAX_MAC_CLI_ARG; k++)
		printk("val %x\n", pdata->mac_cli->val[k]);

#endif

	if (params->get) {
		if (xgmac_cfg[i].get_func)
			ret = xgmac_cfg[i].get_func(ops);
		else if (xgmac_cfg[i].set_func_0)
			ret = xgmac_cfg[i].set_func_0(ops);
		else if (xgmac_cfg[i].set_func_1)
			ret = xgmac_cfg[i].set_func_1(ops, params->val[0]);
		else if (xgmac_cfg[i].set_func_2)
			ret = xgmac_cfg[i].set_func_2(ops,
						      params->val[0],
						      params->val[1]);
		else if (xgmac_cfg[i].set_func_3)
			ret = xgmac_cfg[i].set_func_3(ops,
						      params->val[0],
						      params->val[1],
						      params->val[2]);

	} else {
		if (xgmac_cfg[i].set_func_0)
			ret = xgmac_cfg[i].set_func_0(ops);
		else if (xgmac_cfg[i].set_func_1)
			ret = xgmac_cfg[i].set_func_1(ops, params->val[0]);
		else if (xgmac_cfg[i].set_func_2)
			ret = xgmac_cfg[i].set_func_2(ops,
						      params->val[0],
						      params->val[1]);
		else if (xgmac_cfg[i].set_func_3)
			ret = xgmac_cfg[i].set_func_3(ops,
						      params->val[0],
						      params->val[1],
						      params->val[2]);
	}


	return ret;
}

static int cli_set_cfg1588(void *pdev)
{
	struct adap_prv_data *prv_data = GET_ADAP_PDATA(pdev);
	GSW_MAC_Cli_t *pdata = prv_data->mac_cli;

	gswss_cfg0_1588(pdev,
			pdata->val[0], pdata->val[1],
			pdata->val[2], pdata->val[3]);
	return 0;

}

static int cli_set_txtstamp_fifo(void *pdev)
{
	struct mac_prv_data *priv_data = GET_MAC_PDATA(pdev);
	GSW_MAC_Cli_t *pdata = priv_data->mac_cli;

	struct mac_fifo_entry f_entry;

	f_entry.ttse = pdata->val[0];
	f_entry.ostc = pdata->val[1];
	f_entry.ostpa = pdata->val[2];
	f_entry.cic = pdata->val[3];
	f_entry.ttsl = pdata->val[4];
	f_entry.ttsh = pdata->val[5];
	f_entry.rec_id = pdata->val[6];

	gswss_set_txtstamp_fifo(pdev, &f_entry);
	return 0;
}

static int cli_set_macop(void *pdev)
{
	struct mac_prv_data *priv_data = GET_MAC_PDATA(pdev);
	GSW_MAC_Cli_t *pdata = priv_data->mac_cli;


	if (pdata->val[0] == RX) {
		if (pdata->val[1] == 0)
			gswss_set_mac_rxfcs_op(pdev, pdata->val[2]);
		else if (pdata->val[1] == 1)
			gswss_set_mac_rxsptag_op(pdev, pdata->val[2]);
		else if (pdata->val[1] == 2)
			gswss_set_mac_rxtime_op(pdev, pdata->val[2]);
	} else if (pdata->val[0] == TX) {
		if (pdata->val[1] == 0) {
			if (pdata->val[2] == 0 || pdata->val[2] == 1)
				gswss_set_mac_txfcs_ins_op(pdev,
							   pdata->val[2]);
			else if (pdata->val[2] == 2 || pdata->val[2] == 3)
				gswss_set_mac_txfcs_rm_op(pdev,
							  pdata->val[2] - 2);
		} else if (pdata->val[1] == 1)
			gswss_set_mac_txsptag_op(pdev, pdata->val[2]);
	}

	return 0;
}

static int cli_set_macif(void *pdev)
{
	struct mac_prv_data *priv_data = GET_MAC_PDATA(pdev);
	GSW_MAC_Cli_t *pdata = priv_data->mac_cli;

	if (pdata->val[0] == 0)
		gswss_set_1g_intf(pdev, pdata->val[1]);
	else if (pdata->val[0] == 1)
		gswss_set_fe_intf(pdev, pdata->val[1]);
	else if (pdata->val[0] == 2)
		gswss_set_2G5_intf(pdev, pdata->val[1]);

	return 0;
}

static int cli_mac_reset(void *pdev)
{
	struct mac_prv_data *priv_data = GET_MAC_PDATA(pdev);
	GSW_MAC_Cli_t *pdata = priv_data->mac_cli;

	if (pdata->val[0] == XGMAC)
		gswss_xgmac_reset(pdev, pdata->val[1]);
	else if (pdata->val[0] == LMAC)
		gswss_lmac_reset(pdev, pdata->val[1]);
	else if (pdata->val[0] == ADAP)
		gswss_adap_reset(pdev, pdata->val[1]);

	return 0;
}

static int cli_get_txtstamp_fifo(void *pdev)
{
	struct mac_prv_data *priv_data = GET_MAC_PDATA(pdev);
	GSW_MAC_Cli_t *pdata = priv_data->mac_cli;

	gswss_get_txtstamp_fifo(pdev, pdata->val[0], &pdata->f_entry);

	return 0;
}

static int cli_get_cfg0_1588(void *pdev)
{
	struct adap_prv_data *priv_data = GET_ADAP_PDATA(pdev);
	GSW_MAC_Cli_t *pdata = priv_data->mac_cli;
	gswss_get_cfg0_1588(pdev,
			    &pdata->val[0],
			    &pdata->val[1],
			    &pdata->val[2],
			    &pdata->val[3]);
	return 0;
}

static int cli_get_cfg1_1588(void *pdev)
{
	struct adap_prv_data *priv_data = GET_ADAP_PDATA(pdev);
	GSW_MAC_Cli_t *pdata = priv_data->mac_cli;

	gswss_get_cfg1_1588(pdev,
			    &pdata->val[0],
			    &pdata->val[1],
			    &pdata->val[2]);
	return 0;
}

static int cli_get_nco(void *pdev)
{
	struct adap_prv_data *priv_data = GET_ADAP_PDATA(pdev);
	GSW_MAC_Cli_t *pdata = priv_data->mac_cli;
	int nco;

	nco = gswss_get_nco(pdev, pdata->val[0]);

	return nco;
}

struct mac_api_cfg ss_cfg[] = {
	{GSWSS_REG_WR, 0, 0, gswss_wr_reg, 0, 0},
	{GSWSS_REG_RD, 0, gswss_rd_reg, 0, 0, 0},
	{GSWSS_MAC_RESET, cli_mac_reset, 0, 0, 0, 0},
	{GSWSS_MAC_EN, 0, gswss_mac_enable, 0, 0, 0},
	{GSWSS_MAC_IF, cli_set_macif, 0, 0, 0, 0},
	{GSWSS_MAC_OP, cli_set_macop, 0, 0, 0, 0},
	{GSWSS_MAC_MTU, 0, gswss_set_mtu, 0, 0, gswss_get_mtu},
	{GSWSS_MAC_TXTSTAMP_FIFO, cli_set_txtstamp_fifo, 0, 0, 0, cli_get_txtstamp_fifo},
	{GSWSS_MAC_PHY2MODE, 0, 0, 0, 0, 0},
	{GSWSS_ADAP_INT, 0, 0, 0, gswss_set_interrupt, 0},
	{GSWSS_ADAP_CFG_1588, cli_set_cfg1588, 0, 0, 0, cli_get_cfg0_1588},
	{GSWSS_ADAP_NCO, 0, 0, gswss_set_nco, 0, cli_get_nco},
	{GSWSS_ADAP_MACSEC_RST, 0, gswss_macsec_reset, 0, 0, 0},
	{GSWSS_ADAP_SS_RST, gswss_switch_ss_reset, 0, 0, 0, 0},
	{GSWSS_ADAP_MACSEC_TO_MAC, 0, 0, gswss_set_macsec_to_mac, 0, 0},
	{GSWSS_ADAP_CORESE, 0, gswss_set_corese, 0, 0, gswss_get_corese},
	{GSWSS_ADAP_CLK_MD, 0, gswss_set_clkmode, 0, 0, gswss_get_clkmode},
	{GSWSS_ADAP_1588_CFG1, 0, 0, 0, gswss_cfg1_1588, cli_get_cfg1_1588},
};

int gswss_cfg_main(GSW_MAC_Cli_t *params)
{
	struct mac_ops *mac_ops;
	struct adap_ops *adap_ops = gsw_get_adap_ops(0);
	struct mac_prv_data *mac_pdata;
	struct adap_prv_data *adap_pdata = GET_ADAP_PDATA(adap_ops);
	void *ops = NULL;
	int i = 0, ret = 0;
	int num_of_elem = ARRAY_SIZE(ss_cfg);

	for (i = 0; i < num_of_elem; i++) {
		if (ss_cfg[i].cmdType == params->cmdType) {
			break;
		}
	}

	if (params->cmdType < GSWSS_MAX_MAC) {

		if (params->cmdType == GSWSS_REG_WR || params->cmdType == GSWSS_REG_RD) {
			params->mac_idx = MAC_2;
		}
		mac_ops = gsw_get_mac_ops(0, params->mac_idx);
		mac_pdata = GET_MAC_PDATA(mac_ops);

		mac_pdata->mac_cli = params;

		ops = mac_ops;

	} else if (params->cmdType > GSWSS_MAX_MAC) {

		adap_pdata->mac_cli = params;

		ops = adap_ops;
	}

	if (params->get) {
		if (ss_cfg[i].get_func)
			ret = ss_cfg[i].get_func(ops);
		else if (ss_cfg[i].set_func_0)
			ret = ss_cfg[i].set_func_0(ops);
		else if (ss_cfg[i].set_func_1)
			ret = ss_cfg[i].set_func_1(ops, params->val[0]);
		else if (ss_cfg[i].set_func_2)
			ret = ss_cfg[i].set_func_2(ops,
						   params->val[0],
						   params->val[1]);
		else if (ss_cfg[i].set_func_3)
			ret = ss_cfg[i].set_func_3(ops,
						   params->val[0],
						   params->val[1],
						   params->val[2]);

	} else {
		if (ss_cfg[i].set_func_0)
			ret = ss_cfg[i].set_func_0(ops);
		else if (ss_cfg[i].set_func_1)
			ret = ss_cfg[i].set_func_1(ops, params->val[0]);
		else if (ss_cfg[i].set_func_2)
			ret = ss_cfg[i].set_func_2(ops,
						   params->val[0],
						   params->val[1]);
		else if (ss_cfg[i].set_func_3)
			ret = ss_cfg[i].set_func_3(ops,
						   params->val[0],
						   params->val[1],
						   params->val[2]);
	}

	return ret;
}

static int cli_lmac_get_rmon(void *pdev)
{
	struct mac_prv_data *priv_data = GET_MAC_PDATA(pdev);
	GSW_MAC_Cli_t *pdata = priv_data->mac_cli;

	if (!pdata) {
		mac_printf("MAC Cli pointer is NULL\n");
		return -1;
	}

	lmac_rmon_rd(pdev, &pdata->lpstats);

	return 0;
}

static const struct mac_api_cfg lmac_api_cfg[] = {
	{LMAC_REG_WR, 0, 0, lmac_wr_reg, 0, 0},
	{LMAC_REG_RD, 0, lmac_rd_reg, 0, 0, 0},
	{LMAC_RMON, 0, 0, 0, 0, cli_lmac_get_rmon},
	{LMAC_CLR_RMON, lmac_rmon_clr, 0, 0, 0, 0},
	{LMAC_IF_MODE, 0, lmac_set_intf_mode, 0, 0, 0},
	{LMAC_DPX, 0, lmac_set_duplex_mode, 0, 0, 0},
	{LMAC_XFCS, 0, lmac_set_txfcs, 0, 0, 0},
	{LMAC_FLOWCON, 0, lmac_set_flowcon_mode, 0, 0, 0},
	{LMAC_IPG, 0, lmac_set_ipg, 0, 0, 0},
	{LMAC_PREAMBLE, 0, lmac_set_preamble, 0, 0, 0},
	{LMAC_DEFERMODE, 0, lmac_set_defermode, 0, 0, 0},
	{LMAC_JPS, 0, 0, lmac_set_jps, 0, 0},
	{LMAC_LB, 0, lmac_set_loopback, 0, 0, 0},
	{LMAC_TXER, 0, lmac_set_txer, 0, 0, 0},
	{LMAC_LPIMONIT, 0, lmac_set_lpimonitor_mode, 0, 0, 0},
	{LMAC_PSTAT, 0, 0, 0, 0, 0},
	{LMAC_PISR, 0, 0, 0, 0, 0},
	{LMAC_PAUSE_SA_MODE, 0, lmac_set_pauseframe_samode, 0, 0, 0},
};

int lmac_cfg_main(GSW_MAC_Cli_t *params)
{
	struct mac_ops *mac_ops;
	struct mac_prv_data *mac_pdata;
	int i = 0, ret = 0;
	int num_of_elem = ARRAY_SIZE(lmac_api_cfg);

	for (i = 0; i < num_of_elem; i++) {
		if (lmac_api_cfg[i].cmdType == params->cmdType) {
			break;
		}
	}

	mac_ops = gsw_get_mac_ops(0, params->mac_idx);
	mac_pdata = GET_MAC_PDATA(mac_ops);

	mac_pdata->mac_cli = params;

	if (params->get) {
		if (lmac_api_cfg[i].get_func)
			ret = lmac_api_cfg[i].get_func(mac_ops);
		else if (lmac_api_cfg[i].set_func_1)
			ret = lmac_api_cfg[i].set_func_1(mac_ops, params->val[0]);
		else if (lmac_api_cfg[i].set_func_2)
			ret = lmac_api_cfg[i].set_func_2(mac_ops,
							 params->val[0],
							 params->val[1]);
		else if (lmac_api_cfg[i].set_func_3)
			ret = lmac_api_cfg[i].set_func_3(mac_ops,
							 params->val[0],
							 params->val[1],
							 params->val[2]);

	} else {
		if (lmac_api_cfg[i].set_func_1)
			ret = lmac_api_cfg[i].set_func_1(mac_ops, params->val[0]);
		else if (lmac_api_cfg[i].set_func_2)
			ret = lmac_api_cfg[i].set_func_2(mac_ops,
							 params->val[0],
							 params->val[1]);
		else if (lmac_api_cfg[i].set_func_3)
			ret = lmac_api_cfg[i].set_func_3(mac_ops,
							 params->val[0],
							 params->val[1],
							 params->val[2]);
	}

	return ret;
}
