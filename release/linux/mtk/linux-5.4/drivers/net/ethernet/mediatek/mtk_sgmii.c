// SPDX-License-Identifier: GPL-2.0
// Copyright (c) 2018-2019 MediaTek Inc.

/* A library for MediaTek SGMII circuit
 *
 * Author: Sean Wang <sean.wang@mediatek.com>
 *
 */

#include <linux/mfd/syscon.h>
#include <linux/of.h>
#include <linux/phylink.h>
#include <linux/regmap.h>

#include "mtk_eth_soc.h"

static struct mtk_sgmii_pcs *pcs_to_mtk_sgmii_pcs(struct phylink_pcs *pcs)
{
	return container_of(pcs, struct mtk_sgmii_pcs, pcs);
}

static int mtk_sgmii_xfi_pextp_init(struct mtk_sgmii *ss, struct device_node *r)
{
	struct device_node *np;
	int i;

	for (i = 0; i < MTK_MAX_DEVS; i++) {
		np = of_parse_phandle(r, "mediatek,xfi_pextp", i);
		if (!np)
			break;

		ss->pcs[i].regmap_pextp = syscon_node_to_regmap(np);
		if (IS_ERR(ss->pcs[i].regmap_pextp))
			return PTR_ERR(ss->pcs[i].regmap_pextp);

		of_node_put(np);
	}

	return 0;
}

static int mtk_sgmii_xfi_pll_init(struct mtk_sgmii *ss, struct device_node *r)
{
	struct device_node *np;

	np = of_parse_phandle(r, "mediatek,xfi_pll", 0);
	if (!np)
		return -1;

	ss->pll = syscon_node_to_regmap(np);
	if (IS_ERR(ss->pll))
		return PTR_ERR(ss->pll);

	of_node_put(np);

	return 0;
}

static int mtk_sgmii_xfi_pll_enable(struct mtk_sgmii *ss)
{
	u32 val = 0;

	if (!ss->pll)
		return -EINVAL;

	/* Add software workaround for USXGMII PLL TCL issue */
	regmap_write(ss->pll, XFI_PLL_ANA_GLB8, RG_XFI_PLL_ANA_SWWA);

	regmap_read(ss->pll, XFI_PLL_DIG_GLB8, &val);
	val |= RG_XFI_PLL_EN;
	regmap_write(ss->pll, XFI_PLL_DIG_GLB8, val);

	return 0;
}

int mtk_sgmii_link_status(struct mtk_sgmii_pcs *mpcs)
{
	unsigned int val;

	mutex_lock(&mpcs->reset_lock);

	regmap_read(mpcs->regmap, SGMSYS_PCS_CONTROL_1, &val);

	mutex_unlock(&mpcs->reset_lock);

	return FIELD_GET(SGMII_LINK_STATYS, val);
}

static void mtk_sgmii_get_state(struct mtk_sgmii_pcs *mpcs)
{
	struct phylink_link_state *state = &mpcs->state;
	unsigned int bm, adv, rgc3, sgm_mode;

	mutex_lock(&mpcs->reset_lock);

	state->interface = mpcs->interface;

	regmap_read(mpcs->regmap, SGMSYS_PCS_CONTROL_1, &bm);
	if (bm & SGMII_AN_ENABLE) {
		regmap_read(mpcs->regmap, SGMSYS_PCS_ADVERTISE, &adv);

		phylink_mii_c22_pcs_decode_state(state,
						 FIELD_GET(SGMII_BMSR, bm),
						 FIELD_GET(SGMII_LPA, adv));
	} else {
		state->link = !!(bm & SGMII_LINK_STATYS);

		regmap_read(mpcs->regmap, SGMSYS_SGMII_MODE, &sgm_mode);

		switch (sgm_mode & SGMII_SPEED_MASK) {
		case SGMII_SPEED_10:
			state->speed = SPEED_10;
			break;
		case SGMII_SPEED_100:
			state->speed = SPEED_100;
			break;
		case SGMII_SPEED_1000:
			regmap_read(mpcs->regmap, mpcs->ana_rgc3, &rgc3);
			rgc3 = FIELD_GET(RG_PHY_SPEED_3_125G, rgc3);
			state->speed = rgc3 ? SPEED_2500 : SPEED_1000;
			break;
		}

		if (sgm_mode & SGMII_DUPLEX_HALF)
			state->duplex = DUPLEX_HALF;
		else
			state->duplex = DUPLEX_FULL;
	}

	mutex_unlock(&mpcs->reset_lock);
}

void mtk_sgmii_reset(struct mtk_sgmii_pcs *mpcs)
{
	struct mtk_eth *eth = mpcs->eth;
	int id = mpcs->id;
	u32 val = 0;

	if (id >= MTK_MAX_DEVS || !eth->toprgu)
		return;

	mutex_lock(&mpcs->reset_lock);

	switch (id) {
	case 0:
		/* Enable software reset */
		regmap_read(eth->toprgu, TOPRGU_SWSYSRST_EN, &val);
		val |= SWSYSRST_XFI_PEXPT0_GRST |
		       SWSYSRST_SGMII0_GRST;
		regmap_write(eth->toprgu, TOPRGU_SWSYSRST_EN, val);

		/* Assert SGMII reset */
		regmap_read(eth->toprgu, TOPRGU_SWSYSRST, &val);
		val |= FIELD_PREP(SWSYSRST_UNLOCK_KEY, 0x88) |
		       SWSYSRST_XFI_PEXPT0_GRST |
		       SWSYSRST_SGMII0_GRST;
		regmap_write(eth->toprgu, TOPRGU_SWSYSRST, val);

		usleep_range(100, 500);

		/* De-assert SGMII reset */
		regmap_read(eth->toprgu, TOPRGU_SWSYSRST, &val);
		val |= FIELD_PREP(SWSYSRST_UNLOCK_KEY, 0x88);
		val &= ~(SWSYSRST_XFI_PEXPT0_GRST |
			 SWSYSRST_SGMII0_GRST);
		regmap_write(eth->toprgu, TOPRGU_SWSYSRST, val);

		/* Disable software reset */
		regmap_read(eth->toprgu, TOPRGU_SWSYSRST_EN, &val);
		val &= ~(SWSYSRST_XFI_PEXPT0_GRST |
			 SWSYSRST_SGMII0_GRST);
		regmap_write(eth->toprgu, TOPRGU_SWSYSRST_EN, val);
		break;
	case 1:
		/* Enable software reset */
		regmap_read(eth->toprgu, TOPRGU_SWSYSRST_EN, &val);
		val |= SWSYSRST_XFI_PEXPT1_GRST |
		       SWSYSRST_SGMII1_GRST;
		regmap_write(eth->toprgu, TOPRGU_SWSYSRST_EN, val);

		/* Assert SGMII reset */
		regmap_read(eth->toprgu, TOPRGU_SWSYSRST, &val);
		val |= FIELD_PREP(SWSYSRST_UNLOCK_KEY, 0x88) |
		       SWSYSRST_XFI_PEXPT1_GRST |
		       SWSYSRST_SGMII1_GRST;
		regmap_write(eth->toprgu, TOPRGU_SWSYSRST, val);

		usleep_range(100, 500);

		/* De-assert SGMII reset */
		regmap_read(eth->toprgu, TOPRGU_SWSYSRST, &val);
		val |= FIELD_PREP(SWSYSRST_UNLOCK_KEY, 0x88);
		val &= ~(SWSYSRST_XFI_PEXPT1_GRST |
			 SWSYSRST_SGMII1_GRST);
		regmap_write(eth->toprgu, TOPRGU_SWSYSRST, val);

		/* Disable software reset */
		regmap_read(eth->toprgu, TOPRGU_SWSYSRST_EN, &val);
		val &= ~(SWSYSRST_XFI_PEXPT1_GRST |
			 SWSYSRST_SGMII1_GRST);
		regmap_write(eth->toprgu, TOPRGU_SWSYSRST_EN, val);
		break;
	}

	mutex_unlock(&mpcs->reset_lock);

	usleep_range(10000, 11000);
}

void mtk_sgmii_setup_phya_gen1(struct mtk_sgmii_pcs *mpcs)
{
	if (!mpcs->regmap_pextp)
		return;

	/* Setup operation mode */
	regmap_update_bits(mpcs->regmap_pextp, 0x9024, GENMASK(31, 0),
			   0x00D9071C);
	regmap_update_bits(mpcs->regmap_pextp, 0x2020, GENMASK(31, 0),
			   0xAA8585AA);
	regmap_update_bits(mpcs->regmap_pextp, 0x2030, GENMASK(31, 0),
			   0x0C020207);
	regmap_update_bits(mpcs->regmap_pextp, 0x2034, GENMASK(31, 0),
			   0x0E05050F);
	regmap_update_bits(mpcs->regmap_pextp, 0x2040, GENMASK(31, 0),
			   0x00200032);
	regmap_update_bits(mpcs->regmap_pextp, 0x50F0, GENMASK(31, 0),
			   0x00C014BA);
	regmap_update_bits(mpcs->regmap_pextp, 0x50E0, GENMASK(31, 0),
			   0x3777C12B);
	regmap_update_bits(mpcs->regmap_pextp, 0x506C, GENMASK(31, 0),
			   0x005F9CFF);
	regmap_update_bits(mpcs->regmap_pextp, 0x5070, GENMASK(31, 0),
			   0x9D9DFAFA);
	regmap_update_bits(mpcs->regmap_pextp, 0x5074, GENMASK(31, 0),
			   0x27273F3F);
	regmap_update_bits(mpcs->regmap_pextp, 0x5078, GENMASK(31, 0),
			   0xA7883C68);
	regmap_update_bits(mpcs->regmap_pextp, 0x507C, GENMASK(31, 0),
			   0x11661166);
	regmap_update_bits(mpcs->regmap_pextp, 0x5080, GENMASK(31, 0),
			   0x0E000EAF);
	regmap_update_bits(mpcs->regmap_pextp, 0x5084, GENMASK(31, 0),
			   0x08080E0D);
	regmap_update_bits(mpcs->regmap_pextp, 0x5088, GENMASK(31, 0),
			   0x02030B09);
	regmap_update_bits(mpcs->regmap_pextp, 0x50E4, GENMASK(31, 0),
			   0x0C0C0000);
	regmap_update_bits(mpcs->regmap_pextp, 0x50E8, GENMASK(31, 0),
			   0x04040000);
	regmap_update_bits(mpcs->regmap_pextp, 0x50EC, GENMASK(31, 0),
			   0x0F0F0606);
	regmap_update_bits(mpcs->regmap_pextp, 0x50A8, GENMASK(31, 0),
			   0x506E8C8C);
	regmap_update_bits(mpcs->regmap_pextp, 0x6004, GENMASK(31, 0),
			   0x18190000);
	regmap_update_bits(mpcs->regmap_pextp, 0x00F8, GENMASK(31, 0),
			   0x00FA32FA);
	/* Force SGDT_OUT off and select PCS */
	regmap_update_bits(mpcs->regmap_pextp, 0x00F4, GENMASK(31, 0),
			   0x80201F21);
	/* Force GLB_CKDET_OUT */
	regmap_update_bits(mpcs->regmap_pextp, 0x0030, GENMASK(31, 0),
			   0x00050C00);
	/* Force AEQ on */
	regmap_update_bits(mpcs->regmap_pextp, 0x0070, GENMASK(31, 0),
			   0x02002800);
	usleep_range(1, 5);
	regmap_update_bits(mpcs->regmap_pextp, 0x3040, GENMASK(31, 0),
			   0x20000000);
	/* Setup DA default value */
	regmap_update_bits(mpcs->regmap_pextp, 0x30B0, GENMASK(31, 0),
			   0x00000020);
	regmap_update_bits(mpcs->regmap_pextp, 0x3028, GENMASK(31, 0),
			   0x00008A01);
	regmap_update_bits(mpcs->regmap_pextp, 0x302C, GENMASK(31, 0),
			   0x0000A884);
	regmap_update_bits(mpcs->regmap_pextp, 0x3024, GENMASK(31, 0),
			   0x00083002);
	regmap_update_bits(mpcs->regmap_pextp, 0x3010, GENMASK(31, 0),
			   0x00011110);
	regmap_update_bits(mpcs->regmap_pextp, 0x3048, GENMASK(31, 0),
			   0x40704000);
	regmap_update_bits(mpcs->regmap_pextp, 0x3064, GENMASK(31, 0),
			   0x0000C000);
	regmap_update_bits(mpcs->regmap_pextp, 0x3050, GENMASK(31, 0),
			   0xA8000000);
	regmap_update_bits(mpcs->regmap_pextp, 0x3054, GENMASK(31, 0),
			   0x000000AA);
	regmap_update_bits(mpcs->regmap_pextp, 0x306C, GENMASK(31, 0),
			   0x20200F00);
	regmap_update_bits(mpcs->regmap_pextp, 0xA060, GENMASK(31, 0),
			   0x00050000);
	regmap_update_bits(mpcs->regmap_pextp, 0x90D0, GENMASK(31, 0),
			   0x00000007);
	/* Release reset */
	regmap_update_bits(mpcs->regmap_pextp, 0x0070, GENMASK(31, 0),
			   0x0200E800);
	usleep_range(150, 500);
	/* Switch to P0 */
	regmap_update_bits(mpcs->regmap_pextp, 0x0070, GENMASK(31, 0),
			   0x0200C111);
	usleep_range(1, 5);
	regmap_update_bits(mpcs->regmap_pextp, 0x0070, GENMASK(31, 0),
			   0x0200C101);
	usleep_range(15, 50);
	/* Switch to Gen2 */
	regmap_update_bits(mpcs->regmap_pextp, 0x0070, GENMASK(31, 0),
			   0x0201C111);
	usleep_range(1, 5);
	regmap_update_bits(mpcs->regmap_pextp, 0x0070, GENMASK(31, 0),
			   0x0201C101);
	usleep_range(100, 500);
	regmap_update_bits(mpcs->regmap_pextp, 0x30B0, GENMASK(31, 0),
			   0x00000030);
	regmap_update_bits(mpcs->regmap_pextp, 0x00F4, GENMASK(31, 0),
			   0x80201F01);
	regmap_update_bits(mpcs->regmap_pextp, 0x3040, GENMASK(31, 0),
			   0x30000000);
	usleep_range(400, 1000);
}

void mtk_sgmii_setup_phya_gen2(struct mtk_sgmii_pcs *mpcs)
{
	if (!mpcs->regmap_pextp)
		return;

	/* Setup operation mode */
	regmap_update_bits(mpcs->regmap_pextp, 0x9024, GENMASK(31, 0),
			   0x00D9071C);
	regmap_update_bits(mpcs->regmap_pextp, 0x2020, GENMASK(31, 0),
			   0xAA8585AA);
	regmap_update_bits(mpcs->regmap_pextp, 0x2030, GENMASK(31, 0),
			   0x0C020707);
	regmap_update_bits(mpcs->regmap_pextp, 0x2034, GENMASK(31, 0),
			   0x0E050F0F);
	regmap_update_bits(mpcs->regmap_pextp, 0x2040, GENMASK(31, 0),
			   0x00140032);
	regmap_update_bits(mpcs->regmap_pextp, 0x50F0, GENMASK(31, 0),
			   0x00C014AA);
	regmap_update_bits(mpcs->regmap_pextp, 0x50E0, GENMASK(31, 0),
			   0x3777C12B);
	regmap_update_bits(mpcs->regmap_pextp, 0x506C, GENMASK(31, 0),
			   0x005F9CFF);
	regmap_update_bits(mpcs->regmap_pextp, 0x5070, GENMASK(31, 0),
			   0x9D9DFAFA);
	regmap_update_bits(mpcs->regmap_pextp, 0x5074, GENMASK(31, 0),
			   0x27273F3F);
	regmap_update_bits(mpcs->regmap_pextp, 0x5078, GENMASK(31, 0),
			   0xA7883C68);
	regmap_update_bits(mpcs->regmap_pextp, 0x507C, GENMASK(31, 0),
			   0x11661166);
	regmap_update_bits(mpcs->regmap_pextp, 0x5080, GENMASK(31, 0),
			   0x0E000AAF);
	regmap_update_bits(mpcs->regmap_pextp, 0x5084, GENMASK(31, 0),
			   0x08080D0D);
	regmap_update_bits(mpcs->regmap_pextp, 0x5088, GENMASK(31, 0),
			   0x02030909);
	regmap_update_bits(mpcs->regmap_pextp, 0x50E4, GENMASK(31, 0),
			   0x0C0C0000);
	regmap_update_bits(mpcs->regmap_pextp, 0x50E8, GENMASK(31, 0),
			   0x04040000);
	regmap_update_bits(mpcs->regmap_pextp, 0x50EC, GENMASK(31, 0),
			   0x0F0F0C06);
	regmap_update_bits(mpcs->regmap_pextp, 0x50A8, GENMASK(31, 0),
			   0x506E8C8C);
	regmap_update_bits(mpcs->regmap_pextp, 0x6004, GENMASK(31, 0),
			   0x18190000);
	regmap_update_bits(mpcs->regmap_pextp, 0x00F8, GENMASK(31, 0),
			   0x009C329C);
	/* Force SGDT_OUT off and select PCS */
	regmap_update_bits(mpcs->regmap_pextp, 0x00F4, GENMASK(31, 0),
			   0x80201F21);
	/* Force GLB_CKDET_OUT */
	regmap_update_bits(mpcs->regmap_pextp, 0x0030, GENMASK(31, 0),
			   0x00050C00);
	/* Force AEQ on */
	regmap_update_bits(mpcs->regmap_pextp, 0x0070, GENMASK(31, 0),
			   0x02002800);
	usleep_range(1, 5);
	regmap_update_bits(mpcs->regmap_pextp, 0x3040, GENMASK(31, 0),
			   0x20000000);
	/* Setup DA default value */
	regmap_update_bits(mpcs->regmap_pextp, 0x30B0, GENMASK(31, 0),
			   0x00000020);
	regmap_update_bits(mpcs->regmap_pextp, 0x3028, GENMASK(31, 0),
			   0x00008A01);
	regmap_update_bits(mpcs->regmap_pextp, 0x302C, GENMASK(31, 0),
			   0x0000A884);
	regmap_update_bits(mpcs->regmap_pextp, 0x3024, GENMASK(31, 0),
			   0x00083002);
	regmap_update_bits(mpcs->regmap_pextp, 0x3010, GENMASK(31, 0),
			   0x00011110);
	regmap_update_bits(mpcs->regmap_pextp, 0x3048, GENMASK(31, 0),
			   0x40704000);
	regmap_update_bits(mpcs->regmap_pextp, 0x3050, GENMASK(31, 0),
			   0xA8000000);
	regmap_update_bits(mpcs->regmap_pextp, 0x3054, GENMASK(31, 0),
			   0x000000AA);
	regmap_update_bits(mpcs->regmap_pextp, 0x306C, GENMASK(31, 0),
			   0x22000F00);
	regmap_update_bits(mpcs->regmap_pextp, 0xA060, GENMASK(31, 0),
			   0x00050000);
	regmap_update_bits(mpcs->regmap_pextp, 0x90D0, GENMASK(31, 0),
			   0x00000005);
	/* Release reset */
	regmap_update_bits(mpcs->regmap_pextp, 0x0070, GENMASK(31, 0),
			   0x0200E800);
	usleep_range(150, 500);
	/* Switch to P0 */
	regmap_update_bits(mpcs->regmap_pextp, 0x0070, GENMASK(31, 0),
			   0x0200C111);
	usleep_range(1, 5);
	regmap_update_bits(mpcs->regmap_pextp, 0x0070, GENMASK(31, 0),
			   0x0200C101);
	usleep_range(15, 50);
	/* Switch to Gen2 */
	regmap_update_bits(mpcs->regmap_pextp, 0x0070, GENMASK(31, 0),
			   0x0201C111);
	usleep_range(1, 5);
	regmap_update_bits(mpcs->regmap_pextp, 0x0070, GENMASK(31, 0),
			   0x0201C101);
	usleep_range(100, 500);
	regmap_update_bits(mpcs->regmap_pextp, 0x30B0, GENMASK(31, 0),
			   0x00000030);
	regmap_update_bits(mpcs->regmap_pextp, 0x00F4, GENMASK(31, 0),
			   0x80201F01);
	regmap_update_bits(mpcs->regmap_pextp, 0x3040, GENMASK(31, 0),
			   0x30000000);
	usleep_range(400, 1000);
}

static int mtk_sgmii_pcs_config(struct phylink_pcs *pcs, unsigned int mode,
				phy_interface_t interface,
				const unsigned long *advertising,
				bool permit_pause_to_mac)
{
	struct mtk_sgmii_pcs *mpcs = pcs_to_mtk_sgmii_pcs(pcs);
	struct mtk_eth *eth = mpcs->eth;
	unsigned int rgc3, sgm_mode = 0, bmcr = 0, speed = 0;
	bool mode_changed = false, changed;
	int advertise, link_timer;

	advertise = phylink_mii_c22_pcs_encode_advertisement(interface,
							     advertising);
	if (advertise < 0)
		return advertise;

	/* Clearing IF_MODE_BIT0 switches the PCS to BASE-X mode, and
	 * we assume that fixes it's speed at bitrate = line rate (in
	 * other words, 1000Mbps or 2500Mbps).
	 */
	if (interface == PHY_INTERFACE_MODE_SGMII) {
		bmcr = SGMII_AN_ENABLE;
		sgm_mode = SGMII_IF_MODE_SGMII |
			   SGMII_REMOTE_FAULT_DIS |
			   SGMII_SPEED_DUPLEX_AN;
	} else if (interface == PHY_INTERFACE_MODE_2500BASEX) {
		/* HSGMII without autoneg */
		speed = SGMII_SPEED_1000;
		sgm_mode = SGMII_IF_MODE_SGMII;
	} else {
		/* 1000base-X with/without autoneg */
		bmcr = linkmode_test_bit(ETHTOOL_LINK_MODE_Autoneg_BIT,
					 advertising) ? SGMII_AN_ENABLE : 0;
		if (bmcr)
			sgm_mode = SGMII_SPEED_DUPLEX_AN;
		else
			speed = SGMII_SPEED_1000;
	}

	link_timer = phylink_get_link_timer_ns(interface);
	if (link_timer < 0)
		return link_timer;

	if (MTK_HAS_CAPS(eth->soc->caps, MTK_NETSYS_V3)) {
		mtk_sgmii_xfi_pll_enable(eth->sgmii);
		mtk_sgmii_reset(mpcs);
	}

	mutex_lock(&mpcs->regmap_lock);

	if (mode <= MLO_AN_INBAND && mpcs->interface != interface) {
		mpcs->interface = interface;
		mpcs->mode = mode;
		linkmode_copy(mpcs->advertising, advertising);
		mode_changed = true;
	}

	/* PHYA power down */
	regmap_update_bits(mpcs->regmap, SGMSYS_QPHY_PWR_STATE_CTRL,
			   SGMII_PHYA_PWD, SGMII_PHYA_PWD);

	/* Reset SGMII PCS state */
	regmap_update_bits(mpcs->regmap, SGMII_RESERVED_0,
			   SGMII_SW_RESET, SGMII_SW_RESET);

	/* Configure the interface polarity */
	regmap_update_bits(mpcs->regmap, SGMSYS_QPHY_WRAP_CTRL,
			   SGMII_PN_SWAP_MASK, mpcs->polarity);

	if (interface == PHY_INTERFACE_MODE_2500BASEX)
		rgc3 = RG_PHY_SPEED_3_125G;
	else
		rgc3 = 0;

	/* Configure the underlying interface speed */
	regmap_update_bits(mpcs->regmap, mpcs->ana_rgc3,
			   RG_PHY_SPEED_3_125G, rgc3);

	/* Setup the link timer */
	regmap_write(mpcs->regmap, SGMSYS_PCS_LINK_TIMER,
		     link_timer / 2 / 8);

	/* Update the advertisement, noting whether it has changed */
	regmap_update_bits_check(mpcs->regmap, SGMSYS_PCS_ADVERTISE,
				 SGMII_ADVERTISE, advertise, &changed);

	/* Update the sgmsys mode register */
	regmap_update_bits(mpcs->regmap, SGMSYS_SGMII_MODE,
			   SGMII_REMOTE_FAULT_DIS | SGMII_DUPLEX_HALF |
			   SGMII_SPEED_MASK | SGMII_SPEED_DUPLEX_AN |
			   SGMII_IF_MODE_SGMII, sgm_mode | speed);

	/* Update the BMCR */
	regmap_update_bits(mpcs->regmap, SGMSYS_PCS_CONTROL_1,
			   SGMII_AN_ENABLE, bmcr);

	/* Release PHYA power down state */
	usleep_range(50, 100);
	regmap_write(mpcs->regmap, SGMSYS_QPHY_PWR_STATE_CTRL, 0);

	if (MTK_HAS_CAPS(eth->soc->caps, MTK_NETSYS_V3)) {
		if (interface == PHY_INTERFACE_MODE_2500BASEX)
			mtk_sgmii_setup_phya_gen2(mpcs);
		else
			mtk_sgmii_setup_phya_gen1(mpcs);
	}

	mutex_unlock(&mpcs->regmap_lock);

	return changed || mode_changed;
}

static void mtk_sgmii_pcs_link_poll(struct work_struct *work)
{
	struct mtk_sgmii_pcs *mpcs = container_of(work, struct mtk_sgmii_pcs,
						  link_poll.work);

	if (mpcs->interface != PHY_INTERFACE_MODE_SGMII &&
	    mpcs->interface != PHY_INTERFACE_MODE_1000BASEX &&
	    mpcs->interface != PHY_INTERFACE_MODE_2500BASEX)
		goto exit;

	if (!mtk_sgmii_link_status(mpcs))
		mtk_sgmii_pcs_config(&mpcs->pcs, UINT_MAX, mpcs->interface,
				     mpcs->advertising, false);

exit:
	if (mpcs->mode == MLO_AN_INBAND)
		mtk_sgmii_get_state(mpcs);
	else if (!delayed_work_pending(&mpcs->link_poll))
		schedule_delayed_work(&mpcs->link_poll, msecs_to_jiffies(1000));
}

static int mtk_sgmii_pcs_enable(struct phylink_pcs *pcs)
{
	struct mtk_sgmii_pcs *mpcs = pcs_to_mtk_sgmii_pcs(pcs);

	if (!delayed_work_pending(&mpcs->link_poll))
		schedule_delayed_work(&mpcs->link_poll, msecs_to_jiffies(1000));

	return 0;
}

static void mtk_sgmii_pcs_disable(struct phylink_pcs *pcs)
{
	struct mtk_sgmii_pcs *mpcs = pcs_to_mtk_sgmii_pcs(pcs);

	cancel_delayed_work_sync(&mpcs->link_poll);

	if (mpcs->mode == MLO_AN_INBAND)
		mtk_sgmii_get_state(mpcs);
}

static void mtk_sgmii_pcs_get_state(struct phylink_pcs *pcs,
				    struct phylink_link_state *state)
{
	struct mtk_sgmii_pcs *mpcs = pcs_to_mtk_sgmii_pcs(pcs);

	/* When the interface of the mpcs is not initialized,
	 * we should avoid overriding the state interface.
	 */
	if (mpcs->state.interface != PHY_INTERFACE_MODE_NA)
		state->interface = mpcs->state.interface;
	state->speed = mpcs->state.speed;
	state->duplex = mpcs->state.duplex;
	state->link = mpcs->state.link;

	/* Reconfiguring SGMII every second to ensure that PCS can
	 * link up with the Link Partner when a module is inserted.
	 */
	if (time_after(jiffies, mpcs->link_poll_expire) &&
	    !delayed_work_pending(&mpcs->link_poll)) {
		mpcs->link_poll_expire = jiffies + HZ;
		mpcs->state.an_enabled = state->an_enabled;
		schedule_delayed_work(&mpcs->link_poll, 0);
	}
}

void mtk_sgmii_pcs_restart_an(struct phylink_pcs *pcs)
{
	struct mtk_sgmii_pcs *mpcs = pcs_to_mtk_sgmii_pcs(pcs);
	unsigned int val = 0;

	if (!mpcs->regmap)
		return;

	regmap_read(mpcs->regmap, SGMSYS_PCS_CONTROL_1, &val);
	val |= SGMII_AN_RESTART;
	regmap_write(mpcs->regmap, SGMSYS_PCS_CONTROL_1, val);
}

static void mtk_sgmii_pcs_link_up(struct phylink_pcs *pcs, unsigned int mode,
				  phy_interface_t interface,
				  int speed, int duplex)
{
	struct mtk_sgmii_pcs *mpcs = pcs_to_mtk_sgmii_pcs(pcs);
	struct device *dev = mpcs->eth->dev;
	unsigned int sgm_mode, val;
	unsigned long t_start = jiffies;

	do {
		msleep(100);

		if (mtk_sgmii_link_status(mpcs))
			goto exit;

	} while (time_before(jiffies, t_start + msecs_to_jiffies(3000)));

	dev_warn(dev, "sgmii%d: wait link up timeout!\n", mpcs->id);
	return;

exit:
	/* If autoneg is enabled, the force speed and duplex
	 * are not useful, so don't go any further.
	 */
	regmap_read(mpcs->regmap, SGMSYS_PCS_CONTROL_1, &val);
	if (val & SGMII_AN_ENABLE)
		return;

	/* SGMII force speed and duplex setting */
	if (speed == SPEED_10)
		sgm_mode = SGMII_SPEED_10;
	else if (speed == SPEED_100)
		sgm_mode = SGMII_SPEED_100;
	else
		sgm_mode = SGMII_SPEED_1000;

	if (duplex != DUPLEX_FULL)
		sgm_mode |= SGMII_DUPLEX_HALF;

	regmap_update_bits(mpcs->regmap, SGMSYS_SGMII_MODE,
			   SGMII_DUPLEX_HALF | SGMII_SPEED_MASK,
			   sgm_mode);
}

static const struct phylink_pcs_ops mtk_sgmii_pcs_ops = {
	.pcs_config = mtk_sgmii_pcs_config,
	.pcs_enable = mtk_sgmii_pcs_enable,
	.pcs_disable = mtk_sgmii_pcs_disable,
	.pcs_get_state = mtk_sgmii_pcs_get_state,
	.pcs_an_restart = mtk_sgmii_pcs_restart_an,
	.pcs_link_up = mtk_sgmii_pcs_link_up,
};

int mtk_sgmii_init(struct mtk_eth *eth, struct device_node *r, u32 ana_rgc3)
{
	struct mtk_sgmii *ss = eth->sgmii;
	struct device_node *np;
	int ret, i;

	for (i = 0; i < MTK_MAX_DEVS; i++) {
		np = of_parse_phandle(r, "mediatek,sgmiisys", i);
		if (!np)
			break;

		ss->pcs[i].id = i;
		ss->pcs[i].eth = eth;
		ss->pcs[i].ana_rgc3 = ana_rgc3;

		ss->pcs[i].regmap = syscon_node_to_regmap(np);
		if (IS_ERR(ss->pcs[i].regmap))
			return PTR_ERR(ss->pcs[i].regmap);

		ss->pcs[i].polarity = 0;
		if (of_property_read_bool(np, "pn_swap"))
			ss->pcs[i].polarity |= SGMII_PN_SWAP_TX | SGMII_PN_SWAP_RX;
		else if (of_property_read_bool(np, "pn_swap_tx"))
			ss->pcs[i].polarity |= SGMII_PN_SWAP_TX;
		else if (of_property_read_bool(np, "pn_swap_rx"))
			ss->pcs[i].polarity |= SGMII_PN_SWAP_RX;

		ss->pcs[i].pcs.ops = &mtk_sgmii_pcs_ops;
		ss->pcs[i].pcs.poll = true;
		ss->pcs[i].interface = PHY_INTERFACE_MODE_NA;

		ss->pcs[i].state.link = 0;
		ss->pcs[i].state.duplex = DUPLEX_FULL;
		ss->pcs[i].state.speed = SPEED_1000;

		INIT_DELAYED_WORK(&ss->pcs[i].link_poll, mtk_sgmii_pcs_link_poll);

		mutex_init(&ss->pcs[i].regmap_lock);
		mutex_init(&ss->pcs[i].reset_lock);

		of_node_put(np);
	}

	if (MTK_HAS_CAPS(eth->soc->caps, MTK_NETSYS_V3)) {
		ret = mtk_sgmii_xfi_pextp_init(ss, r);
		if (ret)
			return ret;

		ret = mtk_sgmii_xfi_pll_init(ss, r);
		if (ret)
			return ret;
	}

	return 0;
}

struct phylink_pcs *mtk_sgmii_select_pcs(struct mtk_sgmii *ss, unsigned int id)
{
	if (id >= MTK_MAX_DEVS || !ss->pcs[id].regmap)
		return NULL;

	return &ss->pcs[id].pcs;
}
