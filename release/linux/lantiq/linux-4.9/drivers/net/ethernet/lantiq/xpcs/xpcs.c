/*
 * Intel XPCS driver
 *
 * Copyright (C) 2018 Intel, Corp.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/err.h>
#include <linux/of.h>
#include <linux/phy/phy.h>
#include <linux/reset.h>
#include "xpcs.h"
#include <linux/netdevice.h>

#define MAX_BUSY_RETRY			2000
#define XPCS_IRQ_NAME			"xpcs_irq"
#define XPCS_RES_NAME			"xpcs_reg"
#define XPCS_MOD_NAME			"xpcs-name"
#define XPCS_CL73			"xpcs-cl73"
#define XPCS_CL37			"xpcs-cl37"
#define XPCS_CONN_TYPE			"xpcs-conn"
#define XPCS_MODE_NAME			"xpcs-mode"
#define XPCS_RESET_NAME			"xpcs_reset"
#define POWER_SAVE_MODE			"power-save"
#define XPCS_MAC_IDX			"mac_idx"
#define XPCS_NO_PHY_RX_AUTO_ADAPT	"xpcs-no-phy-rx-auto-adapt"
#define XPCS_AFE_EN			"xpcs-afe_en"
#define XPCS_DFE_EN			"xpcs-dfe_en"
#define XPCS_ADAPT_EN			"xpcs-cont_adapt_en"
#define XPCS_RX_ATTN_LVL		"xpcs-rx_attn_lvl"
#define XPCS_CTLE_BOOTS			"xpcs-rx_ctle_boost"
#define XPCS_CTLE_POLE			"xpcs-rx_ctle_pole"
#define XPCS_VGA2_GAIN			"xpcs-rx_vga2_gain"
#define XPCS_VGA1_GAIN			"xpcs-rx_vga1_gain"
#define XPCS_TX_EQ_PRE		"xpcs-tx_eq_pre"
#define XPCS_TX_EQ_POST		"xpcs-tx_eq_post"
#define XPCS_TX_EQ_MAIN		"xpcs-tx_eq_main"
#define XPCS_TX_IBOOST_LVL		"xpcs-tx_iboost_lvl"
#define XPCS_TX_VBOOST_LVL		"xpcs-tx_vboost_lvl"
#define XPCS_TX_VBOOST_EN		"xpcs-tx_vboost_en"
#define LINK_POLL_TIME			"link_poll_interval"
#define LINK_MODE_CFG			"xpcs_mode_cfg_seq"

static void xpcs_cl73_an(struct xpcs_prv_data *pdata);
static void xpcs_cl72_startup(struct xpcs_prv_data *pdata);
static int xpcs_vs_reset(struct xpcs_prv_data *pdata);
static int xpcs_oneg_xaui_mode(struct xpcs_prv_data *pdata);
static int xpcs_teng_xaui_mode(struct xpcs_prv_data *pdata);
static int xpcs_teng_kr_mode(struct xpcs_prv_data *pdata);
static int xpcs_2p5g_sgmii_mode(struct xpcs_prv_data *pdata);
static void xpcs_link_poll_work(struct work_struct *work);
static int xpcs_synphy_reset_sts(struct xpcs_prv_data *pdata);

static const u32 xpcs_serdes_afe_en[] = {
	XPCS_TENG_KR_AFE_EN,
	XPCS_TENG_XAUI_AFE_EN,
	XPCS_ONEG_AFE_EN,
	XPCS_TWOP5G_AFE_EN,
};

static const u32 xpcs_serdes_dfe_en[] = {
	XPCS_TENG_KR_DFE_EN,
	XPCS_TENG_XAUI_DFE_EN,
	XPCS_ONEG_DFE_EN,
	XPCS_TWOP5G_DFE_EN,
};

static const u32 xpcs_serdes_cont_adapt_0[] = {
	XPCS_TENG_KR_CONT_ADAPT_0,
	XPCS_TENG_XAUI_CONT_ADAPT_0,
	XPCS_ONEG_CONT_ADAPT_0,
	XPCS_TWOP5G_CONT_ADAPT_0,
};

static const u32 xpcs_serdes_rx_attn_ctrl[] = {
	XPCS_TENG_KR_PMA_RX_ATTN_CTRL,
	XPCS_TENG_XAUI_PMA_RX_ATTN_CTRL,
	XPCS_ONEG_PMA_RX_ATTN_CTRL,
	XPCS_TWOP5G_PMA_RX_ATTN_CTRL,
};

static const u32 xpcs_serdes_ctrl_boost_0[] = {
	XPCS_TENG_KR_PMA_RX_CTLE_BOOST_0,
	XPCS_TENG_XAUI_PMA_RX_CTLE_BOOST_0,
	XPCS_ONEG_PMA_RX_CTLE_BOOST_0,
	XPCS_TWOP5G_PMA_RX_CTLE_BOOST_0,
};

static const u32 xpcs_serdes_ctrl_pole_0[] = {
	XPCS_TENG_KR_PMA_RX_EQ_CTRL0,
	XPCS_TENG_XAUI_PMA_RX_EQ_CTRL0,
	XPCS_TWOP5G_PMA_RX_EQ_CTRL0,
	XPCS_ONEG_PMA_RX_EQ_CTRL0,
};

static const u32 xpcs_serdes_vga2_gain_0[] = {
	XPCS_TENG_KR_PMA_RX_VGA2_GAIN_0,
	XPCS_TENG_XAUI_PMA_RX_VGA2_GAIN_0,
	XPCS_TWOP5G_PMA_RX_VGA2_GAIN_0,
	XPCS_ONEG_PMA_RX_VGA2_GAIN_0,
};

static const u32 xpcs_serdes_vga1_gain_0[] = {
	XPCS_TENG_KR_PMA_RX_VGA1_GAIN_0,
	XPCS_TENG_XAUI_PMA_RX_VGA1_GAIN_0,
	XPCS_TWOP5G_PMA_RX_VGA1_GAIN_0,
	XPCS_ONEG_PMA_RX_VGA1_GAIN_0,
};

static const u32 xpcs_serdes_tx_eq_main[] = {
	XPCS_TENG_KR_PMA_TX_EQ_MAIN,
	XPCS_TENG_XAUI_PMA_TX_EQ_MAIN,
	XPCS_TWOP5G_PMA_TX_EQ_MAIN,
	XPCS_ONEG_PMA_TX_EQ_MAIN,
};

static const u32 xpcs_serdes_tx_eq_pre[] = {
	XPCS_TENG_KR_PMA_TX_EQ_PRE,
	XPCS_TENG_XAUI_PMA_TX_EQ_PRE,
	XPCS_TWOP5G_PMA_TX_EQ_PRE,
	XPCS_ONEG_PMA_TX_EQ_PRE,
};

static const u32 xpcs_serdes_tx_eq_post[] = {
	XPCS_TENG_KR_PMA_TX_EQ_POST,
	XPCS_TENG_XAUI_PMA_TX_EQ_POST,
	XPCS_TWOP5G_PMA_TX_EQ_POST,
	XPCS_ONEG_PMA_TX_EQ_POST,
};

static const u32 xpcs_serdes_tx_iboost_lvl[] = {
	XPCS_TENG_KR_PMA_TX_EQ_IBOOST_LVL,
	XPCS_TENG_XAUI_PMA_TX_EQ_IBOOST_LVL,
	XPCS_TWOP5G_PMA_TX_EQ_IBOOST_LVL,
	XPCS_ONEG_PMA_TX_EQ_IBOOST_LVL,
};

static const u32 xpcs_serdes_tx_vboost_lvl[] = {
	XPCS_TENG_KR_PMA_TX_EQ_VBOOST_LVL,
	XPCS_TENG_XAUI_PMA_TX_EQ_VBOOST_LVL,
	XPCS_TWOP5G_PMA_TX_EQ_VBOOST_LVL,
	XPCS_ONEG_PMA_TX_EQ_VBOOST_LVL,
};

static const u32 xpcs_serdes_vboost_en[] = {
	XPCS_TENG_KR_PMA_TX_EQ_VBOOST_EN,
	XPCS_TENG_XAUI_PMA_TX_EQ_VBOOST_EN,
	XPCS_TWOP5G_PMA_TX_EQ_VBOOST_EN,
	XPCS_ONEG_PMA_TX_EQ_VBOOST_EN,
};

struct xpcs_mode_cfg mode_cfg[MAX_XPCS_MODE] = {
	{
		.id = TENG_KR_MODE,
		.pma_type = PMA_TYPE_10G_BASE_KR,
		.pcs_type = PCS_TYPE_10G_BASE_R,
		.lane = LANE_1,
		.mplla_mult = 0x21,
		.mplla_bw = 0x7B,
		.vco_ld_val[0] = 0x549,
		.vco_ref_ld[0] = 0x29,
		.tx_rate[0] = 0,
		.rx_rate[0] = 0,
		.tx_width[0] = 2,	/* 2'b10 - 16 bit */
		.rx_width[0] = 2,	/* 2'b10 - 16 bit */
		.mplla_div165_clk_en = 1,
		.mplla_div10_clk_en = 0,
		.mplla_div8_clk_en = 1,
		.tx_eq_ovrride = 1,
		.ref_clk_ctrl = 0x171,
		.los_thr = 4,
		.rx_vref_ctrl = 0,
		.tx_iboost[0] = 0xF,
		.set_mode = xpcs_teng_kr_mode,
	},
	{
		.id = TENG_XAUI_MODE,
		.pma_type = PMA_TYPE_10G_BASE_KR,
		.pcs_type = PCS_TYPE_10G_BASE_X,
		.lane = LANE_1,
		.mplla_mult = 0x28,
		.mplla_bw = 0x56,
		.mpllb_mult = 0x28,
		.mpllb_bw = 0x56,
		.vco_ld_val[0] = 0x0550,
		.vco_ref_ld[0] = 0x22,
		.afe_en_31 = 0,
		.dfe_en_31 = 0,
		.tx_rate[0] = 2,	/* 2'b10 */
		.rx_rate[0] = 2,	/* 2'b10 */
		.tx_width[0] = 1,
		.rx_width[0] = 1,
		.mplla_div165_clk_en = 0,
		.mplla_div10_clk_en = 0,
		.mplla_div8_clk_en = 0,
		.mpllb_div_clk_en = 0,
		.mpllb_div10_clk_en = 1,
		.mpllb_div8_clk_en = 0,
		.tx_eq_ovrride = 1,
		.ref_clk_ctrl = 0x171,
		.los_thr = 1,
		.rx_vref_ctrl = 0xF,	/* Setting BIT12:8 = 5'd15 */
		.tx_iboost[0] = 0xF,
		.set_mode = xpcs_teng_xaui_mode,
	},
	{
		.id = ONEG_XAUI_MODE,
		.pma_type = PMA_TYPE_1G_BASE_KX,
		.pcs_type = PCS_TYPE_10G_BASE_X,
		.lane = LANE_1,
		.mplla_mult = 0x20,
		.mplla_bw = 0x6E,
		.vco_ld_val[0] = 0x0540,
		.vco_ref_ld[0] = 0x002a,
		.tx_rate[0] = 3,
		.rx_rate[0] = 3,
		.tx_width[0] = 1,	/* 2'b01 - 10 bit */
		.rx_width[0] = 1,	/* 2'b01 - 10 bit */
		.mplla_div165_clk_en = 0,
		.mplla_div10_clk_en = 1,
		.mplla_div8_clk_en = 0,
		.tx_eq_ovrride = 1,
		.los_thr = 0,
		.ref_clk_ctrl = 0x1F1,
		.los_thr = 4,
		.rx_vref_ctrl = 0xF,	/* Setting BIT12:8 = 5'd15 */
		.tx_iboost[0] = 0xF,
		.set_mode = xpcs_oneg_xaui_mode,
	},
	{
		.id = TWOP5G_SGMII_MODE,
		.pma_type = PMA_TYPE_1G_BASE_KX,
		.pcs_type = PCS_TYPE_10G_BASE_X,
		.lane = LANE_1,
		.mplla_mult = 0x28,
		.mplla_bw = 0x56,
		.mpllb_mult = 0x28,
		.mpllb_bw = 0x56,
		.vco_ld_val[0] = 0x550,
		.vco_ref_ld[0] = 0x22,
		.vco_ld_val[1] = 0x550,
		.afe_en_31 = 0,
		.dfe_en_31 = 0,
		.cont_adapt31 = 0,
		.tx_rate[0] = 2, /* 3'b010 : baud/4 */
		.rx_rate[0] = 2, /* 2'b10 : baud/4 */
		.tx_width[0] = 1, /* 2'b01: 10-bit; 2'b11: 20-bit */
		.rx_width[0] = 1, /* 2'b01: 10-bit; 2'b11: 20-bit */
		.mplla_div165_clk_en = 0,
		.mplla_div10_clk_en = 1,
		.mplla_div8_clk_en = 0,
		.mplla_div_clk_en = 0,
		.mpllb_div_clk_en = 0,
		.mpllb_div10_clk_en = 1,
		.mpllb_div8_clk_en = 0,
		.rx_div165_clk_en = 0,
		.tx_eq_ovrride = 1,
		.los_thr = 1,
		.ref_clk_ctrl = 0x1F1,
		.los_thr = 1,
		.rx_vref_ctrl = 0xF,	/* Setting BIT12:8 = 5'd15 */
		.tx_iboost[0] = 0xF,
		.set_mode = xpcs_2p5g_sgmii_mode,
	},
};

static const char *mode_to_speed(u32 mode)
{
	switch (mode) {
	case ONEG_XAUI_MODE:
		return "1Gbps";
	case TWOP5G_SGMII_MODE:
		return "2.5Gbps";
	case TENG_KR_MODE:
	case TENG_XAUI_MODE:
		return "10Gbps";
	default:
		return "Unknown";
	}
}

static void xpcs_print_status(struct xpcs_prv_data *pdata)
{
	if (pdata->link)
		dev_dbg(pdata->dev, "Link is Up - %s/%s\n",
			 mode_to_speed(pdata->mode),
			 pdata->duplex == DUPLEX_FULL ? "Full" : "Half");
	else
		dev_dbg(pdata->dev, "Link is Down\n");
}

static int xpcs_rxtx_stable(struct xpcs_prv_data *pdata)
{
	int i = 0;
	u32 pseq_state = 0;

	do {
		pseq_state = XPCS_RGRD_VAL(pdata, PCS_DIG_STS, PSEQ_STATE);

		if (pseq_state == 4) {
			if (pdata->mode == TENG_KR_MODE)
				pr_debug("%s: Tx/Rx stable (Power_Good State) Speed: 10G\n",
					 pdata->name);
			else if (pdata->mode == ONEG_XAUI_MODE)
				pr_debug("%s: Tx/Rx stable (Power_Good State) Speed: 1G\n",
					 pdata->name);
			else if (pdata->mode == TWOP5G_SGMII_MODE)
				pr_debug("%s: Tx/Rx stable (Power_Good State) Speed: 2.5G\n",
					 pdata->name);

			break;
		}

		usleep_range(10, 20);
	} while (i++ < MAX_BUSY_RETRY);

	if (i >= MAX_BUSY_RETRY) {
		dev_info(pdata->dev, "%s: TX/RX Stable TIMEOUT\n",
			 pdata->name);
		return -1;
	}

	return XPCS_SUCCESS;
}

static int xpcs_byp_pwrupseq(struct xpcs_prv_data *pdata)
{
	int ret = XPCS_SUCCESS;

	XPCS_RGWR_VAL(pdata, PCS_DIG_CTRL1, BYP_PWRUP, 1);

	ret = xpcs_vs_reset(pdata);

	ret = xpcs_rxtx_stable(pdata);

	return ret;
}

static int xpcs_enable_pwrupseq(struct xpcs_prv_data *pdata)
{
	int ret = XPCS_SUCCESS;

	XPCS_RGWR_VAL(pdata, PCS_DIG_CTRL1, BYP_PWRUP, 0);

	ret = xpcs_vs_reset(pdata);

	/* This Synopsys PHY reset is selfcleared when PSEQ_STATE
	 * is equal to 3'b100, that is, Tx/Rx clocks are stable and in
	 * Power_Good state.
	 */
	if (xpcs_synphy_reset_sts(pdata) != XPCS_SUCCESS)
		return XPCS_FAILURE;

	return ret;
}

/* 10G Mode */
static int xpcs_teng_mode(struct xpcs_prv_data *pdata)
{
	u32 pma_ctrl = XPCS_RGRD(pdata, PMA_CTRL1);
	u32 pcs_ctrl = XPCS_RGRD(pdata, PCS_CTRL1);

	if (pdata->pcs_mode == BACKPL_ETH_PCS) {
		XPCS_SET_VAL(pma_ctrl, PMA_CTRL1, SS13, 1);
		XPCS_RGWR(pdata, PMA_CTRL1, pma_ctrl);
	} else {
		XPCS_SET_VAL(pcs_ctrl, PCS_CTRL1, SS13, 1);
		XPCS_RGWR(pdata, PCS_CTRL1, pcs_ctrl);
	}

	return XPCS_SUCCESS;
}

/* 1G Mode */
static int xpcs_oneg_mode(struct xpcs_prv_data *pdata)
{
	u32 pma_ctrl = XPCS_RGRD(pdata, PMA_CTRL1);
	u32 pcs_ctrl = XPCS_RGRD(pdata, PCS_CTRL1);

	if (pdata->pcs_mode == BACKPL_ETH_PCS) {
		XPCS_SET_VAL(pma_ctrl, PMA_CTRL1, SS13, 0);
		XPCS_RGWR(pdata, PMA_CTRL1, pma_ctrl);
	} else {
		XPCS_SET_VAL(pcs_ctrl, PCS_CTRL1, SS13, 0);
		XPCS_RGWR(pdata, PCS_CTRL1, pcs_ctrl);
	}

	return XPCS_SUCCESS;
}

/* 2.5G Mode */
static int xpcs_2p5g_mode(struct xpcs_prv_data *pdata)
{
	u32 pma_ctrl = XPCS_RGRD(pdata, PMA_CTRL1);
	u32 pcs_ctrl = XPCS_RGRD(pdata, PCS_CTRL1);

	if (pdata->pcs_mode == BACKPL_ETH_PCS) {
		XPCS_SET_VAL(pma_ctrl, PMA_CTRL1, SS13, 0);
		XPCS_RGWR(pdata, PMA_CTRL1, pma_ctrl);
		XPCS_SET_VAL(pcs_ctrl, PCS_CTRL1, SS13, 0);
		XPCS_RGWR(pdata, PCS_CTRL1, pcs_ctrl);
	}

	return XPCS_SUCCESS;
}

/* XPCS Vendor Specific Soft Reset
 * When the host sets this bit, the CSR block triggers the software reset
 * process in which all internal blocks are reset, except the Management
 * Interface block and CSR block.
 * When this bit is set, it also resets the PHY.
 * This bit is self cleared
 */
static int xpcs_vs_reset(struct xpcs_prv_data *pdata)
{
	u32 reset = 0;
	u32 idx = 0;

	XPCS_RGWR_VAL(pdata, PCS_DIG_CTRL1, VR_RST, 1);

	do {
		reset = XPCS_RGRD_VAL(pdata, PCS_DIG_CTRL1, VR_RST);

		if (reset == 0)
			break;

		idx++;

		/* To put a small delay and make sure previous operations
		 * are complete
		 */
		usleep_range(100, 200);
	} while (idx <= MAX_BUSY_RETRY);

	if (idx > MAX_BUSY_RETRY) {
		dev_info(pdata->dev,
			 "%s: XPCS Vendor Specific Soft Reset %d times\n",
			 pdata->name, MAX_BUSY_RETRY);
		return XPCS_FAILURE;
	}

	return XPCS_SUCCESS;
}

static int xpcs_synphy_reset_sts(struct xpcs_prv_data *pdata)
{
	u32 reset = 0;
	u32 idx = 0;

	/* Make sure synopys phy is out of reset */
	do {
		reset = XPCS_RGRD_VAL(pdata, PCS_CTRL1, RST);

		if (reset == 0)
			break;

		idx++;

		/* To put a small delay and make sure previous operations
		 * are complete
		 */
		usleep_range(10, 20);
	} while (idx <= MAX_BUSY_RETRY);

	if (idx > MAX_BUSY_RETRY) {
		dev_info(pdata->dev,
			 "%s: Synopsys Phy reset check reached %d times\n",
			 pdata->name, MAX_BUSY_RETRY);
		return XPCS_FAILURE;
	}

	return XPCS_SUCCESS;
}

static void xpcs_cfg_table(struct xpcs_prv_data *pdata)
{
	struct xpcs_serdes_cfg *serdes_cfg = pdata->serdes_cfg;
	u32 mplla_ctrl2 = 0;
	u32 tx_eq0, tx_eq1;
	u32 val = 0, i = 0;

	if (pdata->mpllb) {
		XPCS_RGWR_VAL(pdata, PMA_MPLLB_C0, MPLLB_MULTIPLIER,
			      pdata->mode_cfg->mpllb_mult);

		XPCS_RGWR_VAL(pdata, PMA_MPLLB_C3, MPPLB_BANDWIDTH,
			      pdata->mode_cfg->mpllb_bw);
	} else {
		XPCS_RGWR_VAL(pdata, PMA_MPLLA_C0, MPLLA_MULTIPLIER,
			      pdata->mode_cfg->mplla_mult);

		XPCS_RGWR_VAL(pdata, PMA_MPLLA_C3, MPPLA_BANDWIDTH,
			      pdata->mode_cfg->mplla_bw);
	}

	/* In Power save mode below setting will save 10mW of Power */
	if (pdata->power_save) {
		for (i = 0; i < LANE_MAX - 1; i++)
			pdata->mode_cfg->tx_iboost[i] = 0xB;
	}

	switch (pdata->mode_cfg->lane) {
	case LANE_4:
		XPCS_RGWR_VAL(pdata, PMA_VCO_CAL_LD3, VCO_LD_VAL_3,
			      pdata->mode_cfg->vco_ld_val[3]);
		XPCS_RGWR_VAL(pdata, PMA_VCO_CAL_REF1, VCO_REF_LD_3,
			      pdata->mode_cfg->vco_ref_ld[3]);
		XPCS_RGWR_VAL(pdata, PMA_TX_RATE_CTRL, TX_RATE_3,
			      pdata->mode_cfg->tx_rate[3]);
		XPCS_RGWR_VAL(pdata, PMA_RX_RATE_CTRL, RX_RATE_3,
			      pdata->mode_cfg->rx_rate[3]);
		XPCS_RGWR_VAL(pdata, PMA_TX_GENCTRL2, TX_WIDTH_3,
			      pdata->mode_cfg->tx_width[3]);
		XPCS_RGWR_VAL(pdata, PMA_RX_GENCTRL2, RX_WIDTH_3,
			      pdata->mode_cfg->rx_width[3]);
		XPCS_RGWR_VAL(pdata, PMA_TX_BOOST_CTRL, TX3_IBOOST,
			      pdata->mode_cfg->tx_iboost[3]);

	case LANE_3:
		XPCS_RGWR_VAL(pdata, PMA_VCO_CAL_LD2, VCO_LD_VAL_2,
			      pdata->mode_cfg->vco_ld_val[2]);
		XPCS_RGWR_VAL(pdata, PMA_VCO_CAL_REF1, VCO_REF_LD_2,
			      pdata->mode_cfg->vco_ref_ld[2]);
		XPCS_RGWR_VAL(pdata, PMA_TX_RATE_CTRL, TX_RATE_2,
			      pdata->mode_cfg->tx_rate[2]);
		XPCS_RGWR_VAL(pdata, PMA_RX_RATE_CTRL, RX_RATE_2,
			      pdata->mode_cfg->rx_rate[2]);
		XPCS_RGWR_VAL(pdata, PMA_TX_GENCTRL2, TX_WIDTH_2,
			      pdata->mode_cfg->tx_width[2]);
		XPCS_RGWR_VAL(pdata, PMA_RX_GENCTRL2, RX_WIDTH_2,
			      pdata->mode_cfg->rx_width[2]);
		XPCS_RGWR_VAL(pdata, PMA_TX_BOOST_CTRL, TX2_IBOOST,
			      pdata->mode_cfg->tx_iboost[2]);

	case LANE_2:
		XPCS_RGWR_VAL(pdata, PMA_VCO_CAL_LD1, VCO_LD_VAL_1,
			      pdata->mode_cfg->vco_ld_val[1]);
		XPCS_RGWR_VAL(pdata, PMA_VCO_CAL_REF0, VCO_REF_LD_1,
			      pdata->mode_cfg->vco_ref_ld[1]);
		XPCS_RGWR_VAL(pdata, PMA_TX_RATE_CTRL, TX_RATE_1,
			      pdata->mode_cfg->tx_rate[1]);
		XPCS_RGWR_VAL(pdata, PMA_RX_RATE_CTRL, RX_RATE_1,
			      pdata->mode_cfg->rx_rate[1]);
		XPCS_RGWR_VAL(pdata, PMA_TX_GENCTRL2, TX_WIDTH_1,
			      pdata->mode_cfg->tx_width[1]);
		XPCS_RGWR_VAL(pdata, PMA_RX_GENCTRL2, RX_WIDTH_1,
			      pdata->mode_cfg->rx_width[1]);
		XPCS_RGWR_VAL(pdata, PMA_TX_BOOST_CTRL, TX1_IBOOST,
			      pdata->mode_cfg->tx_iboost[1]);

	case LANE_1:
		XPCS_RGWR_VAL(pdata, PMA_VCO_CAL_LD0, VCO_LD_VAL_0,
			      pdata->mode_cfg->vco_ld_val[0]);
		XPCS_RGWR_VAL(pdata, PMA_VCO_CAL_REF0, VCO_REF_LD_0,
			      pdata->mode_cfg->vco_ref_ld[0]);
		XPCS_RGWR_VAL(pdata, PMA_TX_RATE_CTRL, TX_RATE_0,
			      pdata->mode_cfg->tx_rate[0]);
		XPCS_RGWR_VAL(pdata, PMA_RX_RATE_CTRL, RX_RATE_0,
			      pdata->mode_cfg->rx_rate[0]);
		XPCS_RGWR_VAL(pdata, PMA_TX_GENCTRL2, TX_WIDTH_0,
			      pdata->mode_cfg->tx_width[0]);
		XPCS_RGWR_VAL(pdata, PMA_RX_GENCTRL2, RX_WIDTH_0,
			      pdata->mode_cfg->rx_width[0]);
		XPCS_RGWR_VAL(pdata, PMA_TX_BOOST_CTRL, TX0_IBOOST,
			      serdes_cfg->tx_iboost_lvl[pdata->mode]);

		break;

	default:
		break;
	}

	val = XPCS_RGRD(pdata, PMA_AFE_DFE_EN_CTRL);

	XPCS_SET_VAL(val, PMA_AFE_DFE_EN_CTRL, AFE_EN_0,
		     serdes_cfg->afe_en[pdata->mode]);
	XPCS_SET_VAL(val, PMA_AFE_DFE_EN_CTRL, AFE_EN_3_1,
		     pdata->mode_cfg->afe_en_31);
	XPCS_SET_VAL(val, PMA_AFE_DFE_EN_CTRL, DFE_EN_0,
		     serdes_cfg->dfe_en[pdata->mode]);
	XPCS_SET_VAL(val, PMA_AFE_DFE_EN_CTRL, DFE_EN_3_1,
		     pdata->mode_cfg->dfe_en_31);

	XPCS_RGWR(pdata, PMA_AFE_DFE_EN_CTRL, val);

	XPCS_RGWR_VAL(pdata, PMA_RX_EQ_CTRL4, CONT_ADAPT_0,
		      serdes_cfg->cont_adapt0[pdata->mode]);
	XPCS_RGWR_VAL(pdata, PMA_RX_EQ_CTRL4, CONT_ADAPT_3_1,
		      pdata->mode_cfg->cont_adapt31);

	if (pdata->mpllb) {
		mplla_ctrl2 = XPCS_RGRD(pdata, PMA_MPLLB_C2);

		XPCS_SET_VAL(mplla_ctrl2, PMA_MPLLB_C2, MPLLB_DIV_CLK_EN,
			     pdata->mode_cfg->mpllb_div_clk_en);
		XPCS_SET_VAL(mplla_ctrl2, PMA_MPLLB_C2, MPLLB_DIV10_CLK_EN,
			     pdata->mode_cfg->mpllb_div10_clk_en);
		XPCS_SET_VAL(mplla_ctrl2, PMA_MPLLB_C2, MPLLB_DIV8_CLK_EN,
			     pdata->mode_cfg->mpllb_div8_clk_en);

		XPCS_RGWR(pdata, PMA_MPLLB_C2, mplla_ctrl2);

		XPCS_RGWR_VAL(pdata, PMA_VCO_CAL_LD1, VCO_LD_VAL_1,
			      pdata->mode_cfg->vco_ld_val[1]);
	} else {
		mplla_ctrl2 = XPCS_RGRD(pdata, PMA_MPLLA_C2);

		XPCS_SET_VAL(mplla_ctrl2, PMA_MPLLA_C2, MPLLA_DIV16P5_CLK_EN,
			     pdata->mode_cfg->mplla_div165_clk_en);
		XPCS_SET_VAL(mplla_ctrl2, PMA_MPLLA_C2, MPLLA_DIV10_CLK_EN,
			     pdata->mode_cfg->mplla_div10_clk_en);
		XPCS_SET_VAL(mplla_ctrl2, PMA_MPLLA_C2, MPLLA_DIV8_CLK_EN,
			     pdata->mode_cfg->mplla_div8_clk_en);
		XPCS_SET_VAL(mplla_ctrl2, PMA_MPLLA_C2, MPLLA_DIV_CLK_EN,
			     pdata->mode_cfg->mplla_div_clk_en);

		XPCS_RGWR(pdata, PMA_MPLLA_C2, mplla_ctrl2);
	}

	tx_eq0 = XPCS_RGRD(pdata, PMA_TX_EQ_C0);

	XPCS_SET_VAL(tx_eq0, PMA_TX_EQ_C0, TX_EQ_MAIN,
		     serdes_cfg->tx_eq_main[pdata->mode]);
	XPCS_SET_VAL(tx_eq0, PMA_TX_EQ_C0, TX_EQ_PRE,
		     serdes_cfg->tx_eq_pre[pdata->mode]);

	XPCS_RGWR(pdata, PMA_TX_EQ_C0, tx_eq0);

	tx_eq1 = XPCS_RGRD(pdata, PMA_TX_EQ_C1);

	XPCS_SET_VAL(tx_eq1, PMA_TX_EQ_C1, TX_EQ_POST,
		     serdes_cfg->tx_eq_post[pdata->mode]);
	XPCS_SET_VAL(tx_eq1, PMA_TX_EQ_C1, TX_EQ_OVR_RIDE,
		     pdata->mode_cfg->tx_eq_ovrride);

	XPCS_RGWR(pdata, PMA_TX_EQ_C1, tx_eq1);

	/* Tx Voltage Boost Maximum level,
	 * maximum achievable Tx swing in the PHY
	 */
	XPCS_RGWR_VAL(pdata, PMA_TX_GENCTRL1, VBOOST_EN_0, 1);

	XPCS_RGWR_VAL(pdata, PMA_TX_GENCTRL1, VBOOST_LVL,
		      serdes_cfg->tx_vboost_lvl[pdata->mode]);
}

/* Switch to 10G XAUI Mode in 12G gen5 PHY */
static int xpcs_teng_xaui_mode(struct xpcs_prv_data *pdata)
{
	struct xpcs_serdes_cfg *serdes_cfg = pdata->serdes_cfg;

	pdata->mpllb = 0;

	if (xpcs_byp_pwrupseq(pdata) != XPCS_SUCCESS)
		return XPCS_FAILURE;

	/* Use internal clock */
	XPCS_RGWR_VAL(pdata, PMA_REF_CLK_CTRL, REF_USE_PAD, 0);

	/* Setting SS13 bit PMA side */
	xpcs_teng_mode(pdata);

	XPCS_RGWR_VAL(pdata, PCS_XAUI_CTRL, XAUI_MODE, 0);

	XPCS_RGWR_VAL(pdata, PCS_CTRL2, PCS_TYPE_SEL,
		      pdata->mode_cfg->pcs_type);

	/* Setting SS13 bit of PCS side */
	XPCS_RGWR_VAL(pdata, PCS_CTRL1, SS13, 1);

	/* Select MPLLB for XAUI/10GBASE-KX4
	 * When this bit is set, PHY selects MPLLB to generate
	 * Tx analog clocks on lane 0
	 */
	XPCS_RGWR_VAL(pdata, PMA_MPLL_CMN_CTRL, MPLLB_SEL_3_1, 0);
	XPCS_RGWR_VAL(pdata, PMA_MPLL_CMN_CTRL, MPLLB_SEL_0, 0);

	xpcs_cfg_table(pdata);

	/* PHY LOS threshold register */
	XPCS_RGWR(pdata, PMA_RX_GENCTRL3, pdata->mode_cfg->los_thr);

	/* PHY RX attenuation */
	XPCS_RGWR(pdata, PMA_RX_ATTN_CTRL,
		  serdes_cfg->rx_attn_lvl[pdata->mode]);

	/* PHY ctle_pole and boost and gain register */
	XPCS_RGWR_VAL(pdata, PMA_RX_EQ_CTRL0, CTLE_BOOST_0,
		      serdes_cfg->rx_ctle_boost[pdata->mode]);
	XPCS_RGWR_VAL(pdata, PMA_RX_EQ_CTRL0, CTLE_POLE_0,
		      serdes_cfg->rx_ctle_pole[pdata->mode]);
	XPCS_RGWR_VAL(pdata, PMA_RX_EQ_CTRL0, VGA2_GAIN_0,
		      serdes_cfg->rx_vga2_gain[pdata->mode]);
	XPCS_RGWR_VAL(pdata, PMA_RX_EQ_CTRL0, VGA1_GAIN_0,
		      serdes_cfg->rx_vga1_gain[pdata->mode]);

	/* Rx Biasing Current Control for Rx analog front end */
	XPCS_RGWR_VAL(pdata, PMA_MISC_C0, RX_VREF_CTRL,
		      pdata->mode_cfg->rx_vref_ctrl);

	/* Link Up and Down, No Configuration reset */
	if (xpcs_vs_reset(pdata) != XPCS_SUCCESS)
		return XPCS_FAILURE;

	if (pdata->cl73 == CONN_TYPE_ANEG_DIS) {
		xpcs_disable_an(pdata);
	} else {
		xpcs_cl73_an(pdata);
	}

	return XPCS_SUCCESS;
}

/* Switch to 10G KR Mode in 12G gen5 PHY */
static int xpcs_teng_kr_mode(struct xpcs_prv_data *pdata)
{
	struct xpcs_serdes_cfg *serdes_cfg = pdata->serdes_cfg;

	pdata->mpllb = 0;

	if (xpcs_byp_pwrupseq(pdata) != XPCS_SUCCESS)
		return XPCS_FAILURE;

	/* Use internal clock */
	XPCS_RGWR_VAL(pdata, PMA_REF_CLK_CTRL, REF_USE_PAD, 0);

	/* Setting SS13 bit PMA side */
	xpcs_teng_mode(pdata);

	if (xpcs_enable_pwrupseq(pdata) != XPCS_SUCCESS)
		return XPCS_FAILURE;

	XPCS_RGWR_VAL(pdata, PCS_CTRL2, PCS_TYPE_SEL,
		      pdata->mode_cfg->pcs_type);

	/* 10GBASE-KR PMA or PMD type */
	XPCS_RGWR_VAL(pdata, PMA_CTRL2, PMA_TYPE,
		      pdata->mode_cfg->pma_type);

	xpcs_cfg_table(pdata);

	/* PHY LOS threshold register */
	XPCS_RGWR(pdata, PMA_RX_GENCTRL3, pdata->mode_cfg->los_thr);

	/* PHY RX attenuation level register */
	XPCS_RGWR(pdata, PMA_RX_ATTN_CTRL,
		  serdes_cfg->rx_attn_lvl[pdata->mode]);

	/* PHY ctle_pole and boost and gain register */
	XPCS_RGWR_VAL(pdata, PMA_RX_EQ_CTRL0, CTLE_BOOST_0,
		      serdes_cfg->rx_ctle_boost[pdata->mode]);
	XPCS_RGWR_VAL(pdata, PMA_RX_EQ_CTRL0, CTLE_POLE_0,
		      serdes_cfg->rx_ctle_pole[pdata->mode]);
	XPCS_RGWR_VAL(pdata, PMA_RX_EQ_CTRL0, VGA2_GAIN_0,
		      serdes_cfg->rx_vga2_gain[pdata->mode]);
	XPCS_RGWR_VAL(pdata, PMA_RX_EQ_CTRL0, VGA1_GAIN_0,
		      serdes_cfg->rx_vga1_gain[pdata->mode]);

	XPCS_RGWR(pdata, PMA_REF_CLK_CTRL, pdata->mode_cfg->ref_clk_ctrl);

	/* Link Up and Down, No Configuration reset */
	if (xpcs_vs_reset(pdata) != XPCS_SUCCESS)
		return XPCS_FAILURE;

	/* If connected to SFP module CL73 Auto-negotiation need to be
	 * disabled
	 */
	if (pdata->cl73 == CONN_TYPE_ANEG_DIS) {
		xpcs_disable_an(pdata);
	} else {
		xpcs_cl73_an(pdata);
	}

	return XPCS_SUCCESS;
}

/*
 * Speed     mode  SS6 SS13
 * 1000 Mbps SGMII 1   0
 * 100 Mbps  SGMII 0   1
 * 10 Mbps   SGMII 0   0
 */
/* 1G XAUI Mode */
static int xpcs_oneg_xaui_mode(struct xpcs_prv_data *pdata)
{
	struct xpcs_serdes_cfg *serdes_cfg = pdata->serdes_cfg;
	u32 spd;

	pdata->mpllb = 0;

	if (xpcs_byp_pwrupseq(pdata) != XPCS_SUCCESS)
		return XPCS_FAILURE;

	/* Use internal clock */
	XPCS_RGWR_VAL(pdata, PMA_REF_CLK_CTRL, REF_USE_PAD, 0);

	xpcs_oneg_mode(pdata);

	if (xpcs_enable_pwrupseq(pdata) != XPCS_SUCCESS)
		return XPCS_FAILURE;

	/* Set to XAUI mode */
	XPCS_RGWR_VAL(pdata, PCS_XAUI_CTRL, XAUI_MODE, 0);

	/* When this field is written with 2'b00, the KR
	 * speed mode is selected. When this field is written with a value other
	 * than 2'b00, the KR speed mode is de-selected and non-KR speed
	 * mode (KX4 or KX) is selected
	 */
	XPCS_RGWR_VAL(pdata, PCS_CTRL2, PCS_TYPE_SEL,
		      pdata->mode_cfg->pcs_type);

	/* PMA_TYPE_1G_BASE_KX */
	XPCS_RGWR_VAL(pdata, PMA_CTRL2, PMA_TYPE,
		      pdata->mode_cfg->pma_type);

	/* Ensure 2.5G mode is disabled */
	XPCS_RGWR_VAL(pdata, VR_MII_DIG_CTRL1, EN_2_5G_MODE, 0);

	/* Full duplex by default */
	XPCS_RGWR_VAL(pdata, SR_MII_CTRL, DUPLEX_MODE, 1);

	/* 1Gbps SGMII Mode or speed for Non-ANEG mode */
	/* 0 - 10M, 1 - 100M, 2 - 1G */
	spd = 2;
	if (pdata->cl37 == CONN_TYPE_ANEG_DIS) {
		if (pdata->speed == SPEED_10)
			spd = 0;
		else if (pdata->speed == SPEED_100)
			spd = 1;
	}
	XPCS_RGWR_VAL(pdata, SR_MII_CTRL, SS13, spd & BIT(0));
	XPCS_RGWR_VAL(pdata, SR_MII_CTRL, SS6, (spd >> 1) & BIT(0));

	xpcs_cfg_table(pdata);

	/* PHY LOS threshold register */
	XPCS_RGWR(pdata, PMA_RX_GENCTRL3, pdata->mode_cfg->los_thr);

	/* PHY RX attenuation level register */
	XPCS_RGWR(pdata, PMA_RX_ATTN_CTRL,
		  serdes_cfg->rx_attn_lvl[pdata->mode]);

	/* PHY ctle_pole and boost and gain register */
	XPCS_RGWR_VAL(pdata, PMA_RX_EQ_CTRL0, CTLE_BOOST_0,
		      serdes_cfg->rx_ctle_boost[pdata->mode]);
	XPCS_RGWR_VAL(pdata, PMA_RX_EQ_CTRL0, CTLE_POLE_0,
		      serdes_cfg->rx_ctle_pole[pdata->mode]);
	XPCS_RGWR_VAL(pdata, PMA_RX_EQ_CTRL0, VGA2_GAIN_0,
		      serdes_cfg->rx_vga2_gain[pdata->mode]);
	XPCS_RGWR_VAL(pdata, PMA_RX_EQ_CTRL0, VGA1_GAIN_0,
		      serdes_cfg->rx_vga1_gain[pdata->mode]);

	/* Rx Biasing Current Control for Rx analog front end */
	XPCS_RGWR_VAL(pdata, PMA_MISC_C0, RX_VREF_CTRL,
		      pdata->mode_cfg->rx_vref_ctrl);

	XPCS_RGWR(pdata, PMA_REF_CLK_CTRL, pdata->mode_cfg->ref_clk_ctrl);

	/* Link Up and Down, No Configuration reset */
	if (xpcs_vs_reset(pdata) != XPCS_SUCCESS)
		return XPCS_FAILURE;

	if (pdata->cl37 == CONN_TYPE_ANEG_DIS) {
		xpcs_disable_an(pdata);
	} else {
		/* Enable Clause 37 Auto-negotiation */
		xpcs_cl37_an(pdata);
	}

	return XPCS_SUCCESS;
}

/* 2.5G SGMII Mode */
static int xpcs_2p5g_sgmii_mode(struct xpcs_prv_data *pdata)
{
	struct xpcs_serdes_cfg *serdes_cfg = pdata->serdes_cfg;

	if (!strcmp(pdata->name, "wan_xpcs"))
		pdata->mpllb = 0;
	else
		pdata->mpllb = 1;

	if (xpcs_byp_pwrupseq(pdata) != XPCS_SUCCESS)
		return XPCS_FAILURE;

	/* Use internal clock */
	XPCS_RGWR_VAL(pdata, PMA_REF_CLK_CTRL, REF_USE_PAD, 0);

	xpcs_2p5g_mode(pdata);

	if (xpcs_enable_pwrupseq(pdata) != XPCS_SUCCESS)
		return XPCS_FAILURE;

	/* XAUI Mode to 0 to disable RXAUI Mode */
	XPCS_RGWR_VAL(pdata, PCS_XAUI_CTRL, XAUI_MODE, 0);

	/* Set PCS_Type=1 to disable KR Mode */
	XPCS_RGWR_VAL(pdata, PCS_CTRL2, PCS_TYPE_SEL,
		      pdata->mode_cfg->pcs_type);

	/* Set 1GBASE-KX PCS Type */
	XPCS_RGWR_VAL(pdata, PMA_CTRL2, PMA_TYPE,
		      pdata->mode_cfg->pma_type);

	/* 2.5G GMII Mode operation for 1000BaseX PCS Configuration */
	XPCS_RGWR_VAL(pdata, VR_MII_DIG_CTRL1, EN_2_5G_MODE, 1);

	/* Full duplex by default */
	XPCS_RGWR_VAL(pdata, SR_MII_CTRL, DUPLEX_MODE, 1);

	/* SGMII Mode */
	XPCS_RGWR_VAL(pdata, SR_MII_CTRL, SS13, 0);
	XPCS_RGWR_VAL(pdata, SR_MII_CTRL, SS6, 1);

	if (pdata->mpllb) {
		/* Select MPLLB for XAUI/10GBASE-KX4 */
		XPCS_RGWR_VAL(pdata, PMA_MPLL_CMN_CTRL, MPLLB_SEL_3_1, 0);
		XPCS_RGWR_VAL(pdata, PMA_MPLL_CMN_CTRL, MPLLB_SEL_0, 1);
	} else {
		/* De-Select MPLLB for XAUI/10GBASE-KX4 */
		XPCS_RGWR_VAL(pdata, PMA_MPLL_CMN_CTRL, MPLLB_SEL_3_1, 0);
		XPCS_RGWR_VAL(pdata, PMA_MPLL_CMN_CTRL, MPLLB_SEL_0, 0);
	}

	xpcs_cfg_table(pdata);

	/* Receiver Divide by 16.5 output clock on lane 0 */
	XPCS_RGWR_VAL(pdata, PMA_RX_GENCTRL1, RX_DIV16P5_CLK_EN,
		      pdata->mode_cfg->rx_div165_clk_en);

	/* PHY LOS threshold register */
	XPCS_RGWR(pdata, PMA_RX_GENCTRL3, pdata->mode_cfg->los_thr);

	/* PHY RX attenuation level register */
	XPCS_RGWR(pdata, PMA_RX_ATTN_CTRL,
		  serdes_cfg->rx_attn_lvl[pdata->mode]);

	/* PHY ctle_pole and boost and gain register */
	XPCS_RGWR_VAL(pdata, PMA_RX_EQ_CTRL0, CTLE_BOOST_0,
		      serdes_cfg->rx_ctle_boost[pdata->mode]);
	XPCS_RGWR_VAL(pdata, PMA_RX_EQ_CTRL0, CTLE_POLE_0,
		      serdes_cfg->rx_ctle_pole[pdata->mode]);
	XPCS_RGWR_VAL(pdata, PMA_RX_EQ_CTRL0, VGA2_GAIN_0,
		      serdes_cfg->rx_vga2_gain[pdata->mode]);
	XPCS_RGWR_VAL(pdata, PMA_RX_EQ_CTRL0, VGA1_GAIN_0,
		      serdes_cfg->rx_vga1_gain[pdata->mode]);

	/* Rx Biasing Current Control for Rx analog front end */
	XPCS_RGWR_VAL(pdata, PMA_MISC_C0, RX_VREF_CTRL,
		      pdata->mode_cfg->rx_vref_ctrl);

	/* Select CR Para Port This bit select the interface for accessing
	 * PHY registers: * 0 -JTAG * 1 -CR parallel port This bit should be
	 * changed only after disabling 'jtag_tck'to PHY.
	 */
	XPCS_RGWR_VAL(pdata, PMA_MISC_C0, CR_PARA_SEL, 1);

	XPCS_RGWR(pdata, PMA_REF_CLK_CTRL, pdata->mode_cfg->ref_clk_ctrl);

	/* Link Up and Down, No Configuration reset */
	if (xpcs_vs_reset(pdata) != XPCS_SUCCESS)
		return XPCS_FAILURE;

	if (pdata->cl37 == CONN_TYPE_ANEG_DIS) {
		xpcs_disable_an(pdata);
	} else {
		/* Enable Clause 37 Auto-negotiation */
		xpcs_cl37_an(pdata);
	}

	return XPCS_SUCCESS;
}

void xpcs_disable_an(struct xpcs_prv_data *pdata)
{
	/* Disable CL73 Autoneg */
	XPCS_RGWR_VAL(pdata, AN_CTRL, AN_EN, 0);
	/* Disable Backplane CL37 Autonegotiation */
	XPCS_RGWR_VAL(pdata, PCS_DIG_CTRL1, CL37_BP, 0);
	/* Disable CL37 Autoneg */
	XPCS_RGWR_VAL(pdata, SR_MII_CTRL, AN_ENABLE, 0);
	/* Disable Auto Speed Mode Change after CL37 AN */
	XPCS_RGWR_VAL(pdata, VR_MII_DIG_CTRL1, MAC_AUTO_SW, 0);
	/* Reset MII Ctrl register */
	XPCS_RGWR_VAL(pdata, VR_MII_AN_CTRL, PCS_MODE, 0);
	XPCS_RGWR_VAL(pdata, VR_MII_AN_CTRL, TX_CONFIG, 0);
	XPCS_RGWR_VAL(pdata, VR_MII_AN_CTRL, MII_AN_INTR_EN, 0);
}

/* Programming seq to enable Clause 37 Autonegotiation */
void xpcs_cl37_an(struct xpcs_prv_data *pdata)
{
	/* Disable CL73 Autoneg */
	XPCS_RGWR_VAL(pdata, AN_CTRL, AN_EN, 0);

	/* Enable Backplane CL37 Autonegotiation */
	XPCS_RGWR_VAL(pdata, PCS_DIG_CTRL1, CL37_BP, 1);

	/* Disable CL37 Autoneg */
	XPCS_RGWR_VAL(pdata, SR_MII_CTRL, AN_ENABLE, 0);

	if (pdata->cl37 == CONN_TYPE_ANEG_BX)
		/* PCS_Mode = 0 1000/2500-BaseX Mode */
		XPCS_RGWR_VAL(pdata, VR_MII_AN_CTRL, PCS_MODE, 0);
	else
		/* PCS_Mode = 2 SGMII Mode */
		XPCS_RGWR_VAL(pdata, VR_MII_AN_CTRL, PCS_MODE, 2);

	/* Enables the generation of Clause 37 autonegotiation complete
	 * interrupt output
	 */
	XPCS_RGWR_VAL(pdata, VR_MII_AN_CTRL, MII_AN_INTR_EN, 1);

	XPCS_RGWR_VAL(pdata, VR_MII_DIG_CTRL1, MAC_AUTO_SW, 0);

	if (pdata->cl37 == CONN_TYPE_ANEG_PHY) {
		/* PHY Side SGMII */
		XPCS_RGWR_VAL(pdata, VR_MII_AN_CTRL, TX_CONFIG, 1);
		/* Autonegotiation will advertise the values programmed to:
		 * SGMII_LINK_STS of VR_MII_AN_CTRL, SS13, SS6 of SR_MII_CTRL
		 * and FD of SR_MII_AN_ADV
		 */
		XPCS_RGWR_VAL(pdata, VR_MII_DIG_CTRL1, PHY_MODE_CTRL, 0);
		/* Enable SGMII Link */
		XPCS_RGWR_VAL(pdata, VR_MII_AN_CTRL, SGMII_LINK_STS, 1);
		/* SR_MII_AN_ADV must be written after Link Status and
		 * Link Speed according to Synopsis Notes
		 */
		/* 1 Full Duplex, 0 Half Duplex */
		XPCS_RGWR_VAL(pdata, SR_MII_AN_ADV, FD, 1);
		XPCS_RGWR_VAL(pdata, SR_MII_AN_ADV, HD, 0);
		XPCS_RGWR_VAL(pdata, SR_MII_AN_ADV, RF, 0);
		XPCS_RGWR_VAL(pdata, SR_MII_AN_ADV, PAUSE, 0);
	} else if (pdata->cl37 == CONN_TYPE_ANEG_MAC) {
		/* MAC Side SGMII */
		XPCS_RGWR_VAL(pdata, VR_MII_AN_CTRL, TX_CONFIG, 0);
		/* Enable SGMII Link */
		XPCS_RGWR_VAL(pdata, VR_MII_AN_CTRL, SGMII_LINK_STS, 1);
		/* MAC side SGMII Automatic Speed Mode Change after CL37 AN,
		 * xpcs automatically  switches to the negotiated
		 * SGMII/USXGMII/QSGMII(port0) speed, SGMII get ANEG Config
		 * from PHY about the link state change after the completion
		 * of Clause 37 auto-negotiation between PHY and Link Partner
		 */
		 XPCS_RGWR_VAL(pdata, VR_MII_DIG_CTRL1, MAC_AUTO_SW, 1);
	} else {
		/* 1 Full Duplex, 0 Half Duplex */
		XPCS_RGWR_VAL(pdata, SR_MII_AN_ADV, FD, 1);
		XPCS_RGWR_VAL(pdata, SR_MII_AN_ADV, HD, 0);
		XPCS_RGWR_VAL(pdata, SR_MII_AN_ADV, RF, 0);
		XPCS_RGWR_VAL(pdata, SR_MII_AN_ADV, PAUSE, 0);
	}

	/* 4-BIT MII Interface */
	XPCS_RGWR_VAL(pdata, VR_MII_AN_CTRL, MII_CONTROL, 0);

	/* 2.5G Mode */
	if (pdata->mode == TWOP5G_SGMII_MODE) {
		if (pdata->cl37 == CONN_TYPE_ANEG_BX)
			/* Link timer runs for 10 ms in BaseX mode */
			XPCS_RGWR(pdata, VR_MII_LINK_TIMER_CTRL, 0x2FAF);
		else
			/* Link timer runs for 1.6 ms in SGMII mode */
			XPCS_RGWR(pdata, VR_MII_LINK_TIMER_CTRL, 0x07A1);

		/* If this bit is set, the value programmed to
		 * VR MII MMD Link Timer Control Register will be used to
		 * compute the duration of Link Timer.
		 */
		XPCS_RGWR_VAL(pdata, VR_MII_DIG_CTRL1, CL37_TMR_OVR_RIDE, 1);
	}

	/* Enable CL37 Autoneg */
	XPCS_RGWR_VAL(pdata, SR_MII_CTRL, AN_ENABLE, 1);
}

/* Programming seq to enable Clause 73 Autonegotiation */
static void xpcs_cl73_an(struct xpcs_prv_data *pdata)
{
	/* Enable Autoneg */
	XPCS_RGWR_VAL(pdata, AN_CTRL, AN_EN, 1);

	/* Link Fail Inhibit/Autoneg Wait Timer Value for
	 * Clause 73 autonegotiation.
	 */
	XPCS_RGWR_VAL(pdata, AN_TIMER_CTRL1, INHBT_OR_WAIT_TIME, 0xF);

	/* Over-Ride Control for Clause 73 Auto-negotiation Timers. */
	XPCS_RGWR_VAL(pdata, AN_DIG_CTRL, CL73_TMR_OVR_RIDE, 1);

	/* Enable the AN Complete Interrupt, An Incompatible Link
	 * Interrupt, An page received Interrupt enable
	 */
	XPCS_RGWR(pdata, AN_INTR_MSK, 7);

	/* Restart Autoneg */
	XPCS_RGWR_VAL(pdata, AN_CTRL, RSTRT_AN, 1);

	/* Start Training */
	xpcs_cl72_startup(pdata);
}

/* If this bit is set before or during Clause 73 auto-negotiation, the
 * training starts after all pages have been exchanged during Clause
 * 73 auto-negotiation.
 */
static void xpcs_cl72_startup(struct xpcs_prv_data *pdata)
{
	/* Enable the 10G-Base-KR start up protocol */
	XPCS_RGWR_VAL(pdata, PMA_KR_PMD_CTRL, TR_EN, 1);
}

static int xpcs_get_serdes_cfg(struct xpcs_prv_data *pdata)
{
	struct xpcs_serdes_cfg *serdes_cfg;
	int ret;

	serdes_cfg = devm_kzalloc(pdata->dev, sizeof(*serdes_cfg), GFP_KERNEL);
	if (!serdes_cfg)
		return -ENOMEM;

	if (device_property_present(pdata->dev, XPCS_AFE_EN)) {
		ret = device_property_read_u32_array(pdata->dev,
						     XPCS_AFE_EN,
							 serdes_cfg->afe_en,
							 MAX_XPCS_MODE);
		if (ret) {
			dev_err(pdata->dev, "invalid %s property\n",
				XPCS_AFE_EN);
			return ret;
		}
	} else {
		memcpy(serdes_cfg->afe_en, xpcs_serdes_afe_en,
		       sizeof(serdes_cfg->afe_en));
	}

	if (device_property_present(pdata->dev, XPCS_DFE_EN)) {
		ret = device_property_read_u32_array(pdata->dev,
						     XPCS_DFE_EN,
							 serdes_cfg->dfe_en,
							 MAX_XPCS_MODE);
		if (ret) {
			dev_err(pdata->dev, "invalid %s property\n",
				XPCS_DFE_EN);
			return ret;
		}
	} else {
		memcpy(serdes_cfg->dfe_en, xpcs_serdes_dfe_en,
		       sizeof(serdes_cfg->dfe_en));
	}
	if (device_property_present(pdata->dev, XPCS_ADAPT_EN)) {
		ret = device_property_read_u32_array(pdata->dev,
						     XPCS_ADAPT_EN,
							 serdes_cfg->cont_adapt0,
							 MAX_XPCS_MODE);
		if (ret) {
			dev_err(pdata->dev, "invalid %s property\n",
				XPCS_ADAPT_EN);
			return ret;
		}
	} else {
		memcpy(serdes_cfg->cont_adapt0, xpcs_serdes_cont_adapt_0,
		       sizeof(serdes_cfg->cont_adapt0));
	}
	if (device_property_present(pdata->dev, XPCS_RX_ATTN_LVL)) {
		ret = device_property_read_u32_array(pdata->dev,
						     XPCS_RX_ATTN_LVL,
							 serdes_cfg->rx_attn_lvl,
							 MAX_XPCS_MODE);
		if (ret) {
			dev_err(pdata->dev, "invalid %s property\n",
				XPCS_RX_ATTN_LVL);
			return ret;
		}
	} else {
		memcpy(serdes_cfg->rx_attn_lvl, xpcs_serdes_rx_attn_ctrl,
		       sizeof(serdes_cfg->rx_attn_lvl));
	}
	if (device_property_present(pdata->dev, XPCS_CTLE_BOOTS)) {
		ret = device_property_read_u32_array(pdata->dev,
						     XPCS_CTLE_BOOTS,
							 serdes_cfg->rx_ctle_boost,
							 MAX_XPCS_MODE);
		if (ret) {
			dev_err(pdata->dev, "invalid %s property\n",
				XPCS_CTLE_BOOTS);
			return ret;
		}
	} else {
		memcpy(serdes_cfg->rx_ctle_boost, xpcs_serdes_ctrl_boost_0,
		       sizeof(serdes_cfg->rx_ctle_boost));
	}
	if (device_property_present(pdata->dev, XPCS_CTLE_POLE)) {
		ret = device_property_read_u32_array(pdata->dev,
						     XPCS_CTLE_POLE,
							 serdes_cfg->rx_ctle_pole,
							 MAX_XPCS_MODE);
		if (ret) {
			dev_err(pdata->dev, "invalid %s property\n",
				XPCS_CTLE_POLE);
			return ret;
		}
	} else {
		memcpy(serdes_cfg->rx_ctle_pole, xpcs_serdes_ctrl_pole_0,
		       sizeof(serdes_cfg->rx_ctle_pole));
	}
	if (device_property_present(pdata->dev, XPCS_VGA2_GAIN)) {
		ret = device_property_read_u32_array(pdata->dev,
						     XPCS_VGA2_GAIN,
							 serdes_cfg->rx_vga2_gain,
							 MAX_XPCS_MODE);
		if (ret) {
			dev_err(pdata->dev, "invalid %s property\n",
				XPCS_VGA2_GAIN);
			return ret;
		}
	} else {
		memcpy(serdes_cfg->rx_vga2_gain, xpcs_serdes_vga2_gain_0,
		       sizeof(serdes_cfg->rx_vga2_gain));
	}
	if (device_property_present(pdata->dev, XPCS_VGA1_GAIN)) {
		ret = device_property_read_u32_array(pdata->dev,
						     XPCS_VGA1_GAIN,
							 serdes_cfg->rx_vga1_gain,
							 MAX_XPCS_MODE);
		if (ret) {
			dev_err(pdata->dev, "invalid %s property\n",
				XPCS_VGA1_GAIN);
			return ret;
		}
	} else {
		memcpy(serdes_cfg->rx_vga1_gain, xpcs_serdes_vga1_gain_0,
		       sizeof(serdes_cfg->rx_vga1_gain));
	}
	if (device_property_present(pdata->dev, XPCS_TX_EQ_PRE)) {
		ret = device_property_read_u32_array(pdata->dev,
						     XPCS_TX_EQ_PRE,
							 serdes_cfg->tx_eq_pre,
							 MAX_XPCS_MODE);
		if (ret) {
			dev_err(pdata->dev, "invalid %s property\n",
				XPCS_TX_EQ_PRE);
			return ret;
		}
	} else {
		memcpy(serdes_cfg->tx_eq_pre, xpcs_serdes_tx_eq_pre,
		       sizeof(serdes_cfg->tx_eq_pre));
	}
	if (device_property_present(pdata->dev, XPCS_TX_EQ_POST)) {
		ret = device_property_read_u32_array(pdata->dev,
						     XPCS_TX_EQ_POST,
							 serdes_cfg->tx_eq_post,
							 MAX_XPCS_MODE);
		if (ret) {
			dev_err(pdata->dev, "invalid %s property\n",
				XPCS_TX_EQ_POST);
			return ret;
		}
	} else {
		memcpy(serdes_cfg->tx_eq_post, xpcs_serdes_tx_eq_post,
		       sizeof(serdes_cfg->tx_eq_post));
	}
	if (device_property_present(pdata->dev, XPCS_TX_EQ_MAIN)) {
		ret = device_property_read_u32_array(pdata->dev,
						     XPCS_TX_EQ_MAIN,
							 serdes_cfg->tx_eq_main,
							 MAX_XPCS_MODE);
		if (ret) {
			dev_err(pdata->dev, "invalid %s property\n",
				XPCS_TX_EQ_MAIN);
			return ret;
		}
	} else {
		memcpy(serdes_cfg->tx_eq_main, xpcs_serdes_tx_eq_main,
		       sizeof(serdes_cfg->tx_eq_main));
	}
	if (device_property_present(pdata->dev, XPCS_TX_IBOOST_LVL)) {
		ret = device_property_read_u32_array(pdata->dev,
						     XPCS_TX_IBOOST_LVL,
							 serdes_cfg->tx_iboost_lvl,
							 MAX_XPCS_MODE);
		if (ret) {
			dev_err(pdata->dev, "invalid %s property\n",
				XPCS_TX_IBOOST_LVL);
			return ret;
		}
	} else {
		memcpy(serdes_cfg->tx_iboost_lvl, xpcs_serdes_tx_iboost_lvl,
		       sizeof(serdes_cfg->tx_iboost_lvl));
	}
	if (device_property_present(pdata->dev, XPCS_TX_VBOOST_LVL)) {
		ret = device_property_read_u32_array(pdata->dev,
						     XPCS_TX_VBOOST_LVL,
							 serdes_cfg->tx_vboost_lvl,
							 MAX_XPCS_MODE);
		if (ret) {
			dev_err(pdata->dev, "invalid %s property\n",
				XPCS_TX_VBOOST_LVL);
			return ret;
		}
	} else {
		memcpy(serdes_cfg->tx_vboost_lvl, xpcs_serdes_tx_vboost_lvl,
		       sizeof(serdes_cfg->tx_vboost_lvl));
	}
	if (device_property_present(pdata->dev, XPCS_TX_VBOOST_EN)) {
		ret = device_property_read_u32_array(pdata->dev,
						     XPCS_TX_VBOOST_EN,
							 serdes_cfg->tx_vboost_en,
							 MAX_XPCS_MODE);
		if (ret) {
			dev_err(pdata->dev, "invalid %s property\n",
				XPCS_TX_VBOOST_EN);
			return ret;
		}
	} else {
		memcpy(serdes_cfg->tx_vboost_en, xpcs_serdes_vboost_en,
		       sizeof(serdes_cfg->tx_vboost_en));
	}
	serdes_cfg->no_phy_rx_auto_adapt = of_property_read_bool
				(pdata->dev->of_node,
				XPCS_NO_PHY_RX_AUTO_ADAPT);

	pdata->serdes_cfg = serdes_cfg;

	return 0;
}

static struct xpcs_mode_cfg *xpcs_get_cfg(u32 mode)
{
	int i = 0;

	for (i = 0; i < MAX_XPCS_MODE; i++) {
		if (mode_cfg[i].id == mode)
			return &mode_cfg[i];
	}

	return NULL;
}

static int xpcs_init(struct xpcs_prv_data *pdata)
{
	int ret = XPCS_SUCCESS;

	pdata->pcs_mode = BACKPL_ETH_PCS;

	pdata->mode_cfg = xpcs_get_cfg(pdata->mode);
	if (!pdata->mode_cfg) {
		dev_err(pdata->dev, "XPCS Init: Invalid Mode!\n");
		return -EINVAL;
	}

	ret = pdata->mode_cfg->set_mode(pdata);

	if (ret != XPCS_SUCCESS)
		dev_info(pdata->dev, "%s: %d :: Failed ::\n",
			 pdata->name, pdata->mode);

	return ret;
}

#ifdef CONFIG_OF
static int xpcs_parse_dts(struct platform_device *pdev,
			  struct xpcs_prv_data **pdata)
{
	struct device *dev = &pdev->dev;
	u32 prop = 0;

	(*pdata) = devm_kzalloc(dev, sizeof(**pdata), GFP_KERNEL);

	if (!(*pdata))
		return -ENOMEM;

	/* Retrieve the xpcs name */
	if (device_property_read_string(dev, XPCS_MOD_NAME, &(*pdata)->name)) {
		dev_err(dev, "Xpcs name: cannot get property\n");
		return -EINVAL;
	}

	/* Retrieve the xpcs mode */
	if (!device_property_read_u32(dev, XPCS_MODE_NAME, &prop)) {
		(*pdata)->mode = prop;

		if ((*pdata)->mode >= MAX_XPCS_MODE) {
			dev_err(dev, "Xpcs mode: %u is invalid\n",
				(*pdata)->mode);
			return -EINVAL;
		}
	} else {
		dev_err(dev, "Xpcs mode: cannot get property\n");
		return -EINVAL;
	}

	/* Retrieve the connection type */
	if (!device_property_read_u32(dev, XPCS_CONN_TYPE, &prop)) {
		if (prop >= CONN_TYPE_MAX) {
			dev_err(dev, "Xpcs conntype: %u is invalid\n", prop);
			return -EINVAL;
		}
		(*pdata)->cl73 = prop;
		(*pdata)->cl37 = prop;
	}

	if (!device_property_read_u32(dev, XPCS_CL73, &prop)) {
		if (prop >= CONN_TYPE_MAX) {
			dev_err(dev, "Xpcs CL73: %u is invalid\n", prop);
			return -EINVAL;
		}
		(*pdata)->cl73 = prop;
	}

	if (!device_property_read_u32(dev, XPCS_CL37, &prop)) {
		if (prop >= CONN_TYPE_MAX) {
			dev_err(dev, "Xpcs CL73: %u is invalid\n", prop);
			return -EINVAL;
		}
		(*pdata)->cl37 = prop;
	}

	if (!device_property_read_u32(dev, POWER_SAVE_MODE, &prop)) {
		(*pdata)->power_save = prop;
	} else {
		dev_err(dev, "Xpcs conn: cannot get property\n");
		return -EINVAL;
	}

	platform_set_drvdata(pdev, (void *)(*pdata));

	return XPCS_SUCCESS;
}

#else
static int xpcs_parse_dts(struct platform_device *pdev,
			  struct xpcs_prv_data *pdata,
			  struct resource *res)
{
	return -1;
}
#endif

static int xpcs_reset(struct device *dev)
{
	int err;
	struct xpcs_prv_data *pdata = dev_get_drvdata(dev);

	err = reset_control_assert(pdata->xpcs_rst);
	if (err)
		return err;
	udelay(1);
	err = reset_control_deassert(pdata->xpcs_rst);
	if (err)
		return err;

	return 0;
}

static int xpcs_power_off(struct device *dev)
{
	struct xpcs_prv_data *pdata;
	struct phy *phy;

	if (!dev)
		return -ENODEV;
	dev_dbg(dev, "%s\n", __func__);
	pdata = dev_get_drvdata(dev);
	phy = pdata->phy;
	if (!phy) {
		dev_err(dev, "XPCS Power Off: Cannot get phy!\n");
		return -EINVAL;
	}
	if (!pdata->state)
		return 0;

	pdata->state = 0;

	/* Xpcs reset assert */
	reset_control_assert(pdata->xpcs_rst);

	/* RCU reset PHY */
	phy_power_off(phy);

	/* Exit PHY */
	phy_exit(phy);

	return 0;
}

static int xpcs_power_on(struct device *dev)
{
	struct xpcs_prv_data *pdata;
	struct xpcs_serdes_cfg *serdes_cfg;
	struct phy *phy;
	int ret = 0;

	if (!dev)
		return -ENODEV;
	dev_dbg(dev, "%s\n", __func__);
	pdata = dev_get_drvdata(dev);
	serdes_cfg = pdata->serdes_cfg;
	phy = pdata->phy;
	if (!phy) {
		dev_err(dev, "XPCS Power On: Cannot get phy!\n");
		return -EINVAL;
	}
	if (pdata->state)
		return 0;

	/* Init PHY */
	ret = phy_init(phy);

	if (ret < 0) {
		dev_dbg(dev, "phy_init err %s.\n", pdata->name);
		return ret;
	}

	/* Power ON PHY */
	phy_power_on(phy);

	/* Xpcs reset deassert */
	reset_control_deassert(pdata->xpcs_rst);

	/* Power ON XPCS */
	ret = xpcs_init(pdata);

	if (ret < 0) {
		dev_dbg(dev, "xpcs_init err %s.\n", pdata->name);
		return ret;
	}

	pdata->state = 1;

	if (!serdes_cfg->no_phy_rx_auto_adapt)
		ret = phy_calibrate(phy);

	return ret;
}

int xpcs_reinit(struct device *dev, u32 mode)
{
	struct xpcs_prv_data *pdata;
	int ret;

	if (!dev)
		return -ENODEV;
	pdata = dev_get_drvdata(dev);
	ret = xpcs_power_off(dev);
	if (ret)
		return ret;

	/* Change mode to new mode */
	pdata->mode = mode;

	ret = xpcs_power_on(dev);

	return ret;
}

static int xpcs_ethtool_ksettings_get(struct device *dev,
				      struct ethtool_link_ksettings *cmd)
{
	struct xpcs_prv_data *pdata = dev_get_drvdata(dev);

	if (!pdata) {
		dev_err(dev, "XPCS is not initialized\n");
		return -ENODEV;
	}

	ethtool_link_ksettings_zero_link_mode(cmd, supported);
	ethtool_link_ksettings_zero_link_mode(cmd, advertising);
	ethtool_link_ksettings_zero_link_mode(cmd, lp_advertising);

	ethtool_link_ksettings_add_link_mode(cmd, supported, 1000baseX_Full);
	ethtool_link_ksettings_add_link_mode(cmd, supported, 10000baseKR_Full);
	ethtool_link_ksettings_add_link_mode(cmd, supported, 2500baseX_Full);

	if (pdata->mode == TENG_KR_MODE)
		cmd->base.speed = SPEED_10000;
	else if (pdata->mode == ONEG_XAUI_MODE)
		cmd->base.speed = SPEED_1000;
	else if (pdata->mode == TWOP5G_SGMII_MODE)
		cmd->base.speed = SPEED_2500;
	/* USXGMII support only FULL duplex */
	cmd->base.duplex = DUPLEX_FULL;
	return 0;
}

static int xpcs_ethtool_ksettings_set(struct device *dev,
				      const struct ethtool_link_ksettings *cmd)
{
	u32 speed = cmd->base.speed;
	u32 mode;
	struct xpcs_prv_data *pdata = dev_get_drvdata(dev);

	if (!pdata) {
		dev_err(dev, "XPCS is not initialized\n");
		return -1;
	}

	/* Ignore settings with speed = 0, which can happen if settings were get
	 * before a valid interface speed was detected and then applied back.
	 */
	if (speed == 0)
		return 0;

	if (speed != SPEED_10000 &&
	    speed != SPEED_1000 &&
	    speed != SPEED_100 &&
	    speed != SPEED_10 &&
	    speed != SPEED_2500) {
		dev_err(dev, "XPCS invalid speed %d\n", speed);
		return -EINVAL;
	}

	if (speed == SPEED_1000 || speed == SPEED_100 || speed == SPEED_10)
		mode = ONEG_XAUI_MODE;
	else if (speed == SPEED_10000)
		mode = TENG_KR_MODE;
	else if (speed == SPEED_2500)
		mode = TWOP5G_SGMII_MODE;
	else
		return -EINVAL;

	/* Restart Xpcs & PHY if mode or speed changes */
	if (mode != pdata->mode || speed != pdata->speed) {
		pdata->speed = speed;
		xpcs_reinit(pdata->dev, mode);
	}

	return 0;
}

static void xpcs_get_link_sts(struct xpcs_prv_data *pdata)
{
	int ret;

	switch (pdata->mode) {
	case TENG_KR_MODE:
	case ONEG_XAUI_MODE:
		ret = XPCS_RGRD(pdata, PCS_STS1);
		ret = XPCS_RGRD(pdata, PCS_STS1);
		break;

	case TWOP5G_SGMII_MODE:
	/* FIXME: On SFP platform Sometimes PC_STS1 shows
	 * wrong link status for 2.5G Link partner.
	 */
		ret = XPCS_RGRD(pdata, SR_MII_STS);
		ret = XPCS_RGRD(pdata, SR_MII_STS);
		break;

	default:
		return;
	}

	if (ret < 0)
		return;
	if (ret & 0x0004)
		pdata->link = 1;
}

/* Poll the link state and reconfigure serdes */
static void xpcs_link_poll_work(struct work_struct *work)
{
	struct xpcs_prv_data *pdata = container_of(work, struct xpcs_prv_data,
						link_change_work.work);
	u32 mode = 0;

	/* Since the device can be in the wrong mode when a link is
	 * (re-)established (SFP connected after the interface is
	 * up), the link status may report no link. If there
	 * is no link, try switching modes and checking the status
	 * again
	 */
	 pdata->link = 0;
	 xpcs_get_link_sts(pdata);

	if (pdata->link) {
		pdata->link_mode = 0;
	} else {
		if (pdata->link_mode > (MAX_XPCS_MODE - 1))
			pdata->link_mode = 0;
		mode = pdata->link_change_seq[pdata->link_mode];
		xpcs_reinit(pdata->dev, mode);
		pdata->link_mode++;
	}

	if (pdata->old_link != pdata->link) {
		pdata->old_link = pdata->link;
		xpcs_print_status(pdata);
	}


	/* Reschedule work for every poll interval */
	queue_delayed_work(pdata->link_poll,
			   &pdata->link_change_work,
			   (HZ * pdata->poll_interval));
}

static int dynamic_link_detection(struct xpcs_prv_data *pdata)
{
	int err;

	if (device_property_present(pdata->dev, LINK_MODE_CFG)) {
		err = device_property_read_u32_array(pdata->dev,
						     LINK_MODE_CFG,
							 pdata->link_change_seq,
							 MAX_XPCS_MODE);
		if (err) {
			dev_err(pdata->dev, "invalid %s property\n",
				LINK_MODE_CFG);
			return err;
		}
	err = device_property_read_u32 (pdata->dev, LINK_POLL_TIME,
					&pdata->poll_interval);
	if (err) {
		dev_warn(pdata->dev, "using default poll interval\n");
		pdata->poll_interval = 2;
	}
	pdata->link_poll =
		create_singlethread_workqueue("xpcs_dynamic_link");
	if (!pdata->link_poll) {
		dev_err(pdata->dev, "Cannot create link change workqueue\n");
		return -ENOMEM;
	}

	INIT_DELAYED_WORK(&pdata->link_change_work,
			  xpcs_link_poll_work);
	queue_delayed_work(pdata->link_poll,
			   &pdata->link_change_work, (15 * HZ));
	}

	return 0;
}

static int xpcs_probe(struct platform_device *pdev)
{
	struct resource *res;
	struct device *dev = &pdev->dev;
	int ret = XPCS_SUCCESS;
	struct xpcs_prv_data *pdata;
	struct xpcs_serdes_cfg *serdes_cfg;

	if (dev->of_node) {
		if (xpcs_parse_dts(pdev, &pdata) != XPCS_SUCCESS) {
			dev_dbg(dev, "xpcs dt parse failed!\n");
			return -EINVAL;
		}
	} else { /* Read private data from end point */
		pdata = dev_get_drvdata(dev);

		if (!pdata) {
			dev_dbg(dev,
				"Get private data from end point failed!\n");
			return -EINVAL;
		}
	}

	pdata->id = pdev->id;
	pdata->dev = dev;

	pdata->ops.ethtool_ksettings_get = xpcs_ethtool_ksettings_get;
	pdata->ops.ethtool_ksettings_set = xpcs_ethtool_ksettings_set;
	pdata->ops.power_off = xpcs_power_off;
	pdata->ops.power_on = xpcs_power_on;

	pdata->irq_num = platform_get_irq_byname(pdev, XPCS_IRQ_NAME);

	if (pdata->irq_num <= 0) {
		dev_dbg(dev, "Cannot get xpcs irq !!\n");
		return -ENODEV;
	}

	res = platform_get_resource_byname(pdev, IORESOURCE_MEM, XPCS_RES_NAME);

	if (!res) {
		dev_err(dev, "Failed to get %s iomem res!\n", XPCS_RES_NAME);
		return -ENOENT;
	}

	dev_dbg(dev, "%s iomem: %pr !\n", res->name, res);

	pdata->phy = devm_phy_get(&pdev->dev, "phy");

	if (IS_ERR(pdata->phy)) {
		dev_dbg(dev, "No phy for %s\n", pdata->name);
		return PTR_ERR(pdata->phy);
	}

	ret = phy_init(pdata->phy);

	if (ret < 0) {
		dev_dbg(dev, "phy_init err %s.\n", pdata->name);
		return ret;
	}

	phy_power_on(pdata->phy);

	pdata->addr_base = devm_ioremap_resource(dev, res);

	if (IS_ERR(pdata->addr_base)) {
		dev_err(dev, "Failed to ioremap resource: %pr !\n", res);
		return -EINVAL;
	}

	pdata->xpcs_rst = devm_reset_control_get(dev, XPCS_RESET_NAME);

	if (IS_ERR(pdata->xpcs_rst))
		return -1;

	if (xpcs_reset(dev)) {
		dev_err(dev, "Failed to do %s reset:\n", pdata->name);
		return -EINVAL;
	}

	ret = xpcs_get_serdes_cfg(pdata);
	if (ret)
		return ret;

	serdes_cfg = pdata->serdes_cfg;

	/* Initialize XPCS */
	if (xpcs_init(pdata)) {
		dev_err(dev, "%s Initialization Failed!!\n", pdata->name);
		return -EINVAL;
	}

	pdata->state = 1;

	if (xpcs_sysfs_init(pdata)) {
		dev_dbg(dev, "%s: sysfs init failed!\n", pdata->name);
		return -EINVAL;
	}

	/* Schedule the link poll task if link is dynmically configurable*/
	ret = dynamic_link_detection(pdata);
	if (ret)
		return ret;

	/* Phy calibrate for RX Signal Adaption */
	if (!serdes_cfg->no_phy_rx_auto_adapt)
		ret = phy_calibrate(pdata->phy);

	return XPCS_SUCCESS;
}

static int xpcs_release(struct platform_device *pdev)
{
	struct xpcs_prv_data *priv = platform_get_drvdata(pdev);

	if (!IS_ERR(priv->phy)) {
		phy_power_off(priv->phy);
		phy_exit(priv->phy);
	}

	return 0;
}

static const struct of_device_id intel_xpcs_dev_match[] = {
	{ .compatible = "intel,xpcs" },
	{},
};
MODULE_DEVICE_TABLE(of, intel_xpcs_dev_match);

static struct platform_driver intel_xpcs_driver = {
	.probe = xpcs_probe,
	.remove = xpcs_release,
	.driver = {
		.name = XPCS_DEV_NAME,
		.of_match_table = of_match_ptr(intel_xpcs_dev_match),
	}
};

module_platform_driver(intel_xpcs_driver);

MODULE_AUTHOR("Joby Thampan");
MODULE_DESCRIPTION("Intel XPCS Device driver");
MODULE_LICENSE("GPL v2");
