// SPDX-License-Identifier: GPL-2.0+
#include <linux/bitfield.h>
#include <linux/firmware.h>
#include <linux/module.h>
#include <linux/nvmem-consumer.h>
#include <linux/of_address.h>
#include <linux/of_platform.h>
#include <linux/pinctrl/consumer.h>
#include <linux/phy.h>
#include <linux/pm_domain.h>
#include <linux/pm_runtime.h>

#include "mtk.h"

#define MTK_2P5GPHY_ID_MT7988	(0x00339c11)

#define MT7988_2P5GE_PMB_FW		"mediatek/mt7988/i2p5ge-phy-pmb.bin"
#define MT7988_2P5GE_PMB_FW_SIZE	(0x20000)
#define MT7988_2P5GE_PMB_FW_BASE	(0x0f100000)
#define MT7988_2P5GE_PMB_FW_LEN		(0x20000)
#define MT7988_2P5GE_MD32_EN_CFG_BASE	(0x0f0f0018)
#define MT7988_2P5GE_MD32_EN_CFG_LEN	(0x20)
#define   MD32_EN			BIT(0)

#define BASE100T_STATUS_EXTEND		(0x10)
#define BASE1000T_STATUS_EXTEND		(0x11)
#define EXTEND_CTRL_AND_STATUS		(0x16)

#define PHY_AUX_CTRL_STATUS		(0x1d)
#define   PHY_AUX_DPX_MASK		GENMASK(5, 5)
#define   PHY_AUX_SPEED_MASK		GENMASK(4, 2)

#define MTK_PHY_LPI_PCS_DSP_CTRL		(0x121)
#define   MTK_PHY_LPI_SIG_EN_LO_THRESH100_MASK	GENMASK(12, 8)

/* Registers on Token Ring debug nodes */
/* ch_addr = 0x0, node_addr = 0xf, data_addr = 0x3c */
#define AUTO_NP_10XEN				BIT(6)

struct mtk_i2p5ge_phy_priv {
	bool fw_loaded;
};

enum {
	PHY_AUX_SPD_10 = 0,
	PHY_AUX_SPD_100,
	PHY_AUX_SPD_1000,
	PHY_AUX_SPD_2500,
};

static int mt798x_2p5ge_phy_load_fw(struct phy_device *phydev)
{
	struct mtk_i2p5ge_phy_priv *priv = phydev->priv;
	void __iomem *md32_en_cfg_base, *pmb_addr;
	struct device *dev = &phydev->mdio.dev;
	const struct firmware *fw;
	int ret, i;
	u16 reg;

	if (priv->fw_loaded)
		return 0;

	pmb_addr = ioremap(MT7988_2P5GE_PMB_FW_BASE, MT7988_2P5GE_PMB_FW_LEN);
	if (!pmb_addr)
		return -ENOMEM;
	md32_en_cfg_base = ioremap(MT7988_2P5GE_MD32_EN_CFG_BASE,
				   MT7988_2P5GE_MD32_EN_CFG_LEN);
	if (!md32_en_cfg_base) {
		ret = -ENOMEM;
		goto free_pmb;
	}

	ret = request_firmware(&fw, MT7988_2P5GE_PMB_FW, dev);
	if (ret) {
		dev_err(dev, "failed to load firmware: %s, ret: %d\n",
			MT7988_2P5GE_PMB_FW, ret);
		goto free;
	}

	if (fw->size != MT7988_2P5GE_PMB_FW_SIZE) {
		dev_err(dev, "Firmware size 0x%zx != 0x%x\n",
			fw->size, MT7988_2P5GE_PMB_FW_SIZE);
		ret = -EINVAL;
		goto release_fw;
	}

	reg = readw(md32_en_cfg_base);
	if (reg & MD32_EN) {
		phy_set_bits(phydev, MII_BMCR, BMCR_RESET);
		usleep_range(10000, 11000);
	}
	phy_set_bits(phydev, MII_BMCR, BMCR_PDOWN);

	/* Write magic number to safely stall MCU */
	phy_write_mmd(phydev, MDIO_MMD_VEND1, 0x800e, 0x1100);
	phy_write_mmd(phydev, MDIO_MMD_VEND1, 0x800f, 0x00df);

	for (i = 0; i < MT7988_2P5GE_PMB_FW_SIZE - 1; i += 4)
		writel(*((uint32_t *)(fw->data + i)), pmb_addr + i);
	dev_info(dev, "Firmware date code: %x/%x/%x, version: %x.%x\n",
		 be16_to_cpu(*((__be16 *)(fw->data +
					  MT7988_2P5GE_PMB_FW_SIZE - 8))),
		 *(fw->data + MT7988_2P5GE_PMB_FW_SIZE - 6),
		 *(fw->data + MT7988_2P5GE_PMB_FW_SIZE - 5),
		 *(fw->data + MT7988_2P5GE_PMB_FW_SIZE - 2),
		 *(fw->data + MT7988_2P5GE_PMB_FW_SIZE - 1));

	writew(reg & ~MD32_EN, md32_en_cfg_base);
	writew(reg | MD32_EN, md32_en_cfg_base);
	phy_set_bits(phydev, MII_BMCR, BMCR_RESET);
	/* We need a delay here to stabilize initialization of MCU */
	usleep_range(7000, 8000);
	dev_info(dev, "Firmware loading/trigger ok.\n");

	priv->fw_loaded = true;

release_fw:
	release_firmware(fw);
free:
	iounmap(md32_en_cfg_base);
free_pmb:
	iounmap(pmb_addr);

	return ret;
}

static int mt798x_2p5ge_phy_config_init(struct phy_device *phydev)
{
	struct pinctrl *pinctrl;
	int ret;

	/* Check if PHY interface type is compatible */
	if (phydev->interface != PHY_INTERFACE_MODE_INTERNAL)
		return -ENODEV;

	ret = mt798x_2p5ge_phy_load_fw(phydev);
	if (ret < 0)
		return ret;

	/* Setup LED */
	phy_set_bits_mmd(phydev, MDIO_MMD_VEND2, MTK_PHY_LED0_ON_CTRL,
			 MTK_PHY_LED_ON_POLARITY | MTK_PHY_LED_ON_LINK10 |
			 MTK_PHY_LED_ON_LINK100 | MTK_PHY_LED_ON_LINK1000 |
			 MTK_PHY_LED_ON_LINK2500);
	phy_set_bits_mmd(phydev, MDIO_MMD_VEND2, MTK_PHY_LED1_ON_CTRL,
			 MTK_PHY_LED_ON_FDX | MTK_PHY_LED_ON_HDX);

	/* Switch pinctrl after setting polarity to avoid bogus blinking */
	pinctrl = devm_pinctrl_get_select(&phydev->mdio.dev, "i2p5gbe-led");
	if (IS_ERR(pinctrl))
		dev_err(&phydev->mdio.dev, "Fail to set LED pins!\n");

	phy_modify_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_LPI_PCS_DSP_CTRL,
		       MTK_PHY_LPI_SIG_EN_LO_THRESH100_MASK, 0);

	/* Enable 16-bit next page exchange bit if 1000-BT isn't advertising */
	mtk_tr_modify(phydev, 0x0, 0xf, 0x3c, AUTO_NP_10XEN,
		      FIELD_PREP(AUTO_NP_10XEN, 0x1));

	/* Enable HW auto downshift */
	phy_modify_paged(phydev, MTK_PHY_PAGE_EXTENDED_1,
			 MTK_PHY_AUX_CTRL_AND_STATUS,
			 0, MTK_PHY_ENABLE_DOWNSHIFT);

	return 0;
}

static int mt798x_2p5ge_phy_config_aneg(struct phy_device *phydev)
{
	bool changed = false;
	u32 adv;
	int ret;

	/* In fact, if we disable autoneg, we can't link up correctly:
	 * 2.5G/1G: Need AN to exchange master/slave information.
	 * 100M/10M: Without AN, link starts at half duplex (According to
	 *           IEEE 802.3-2018), which this phy doesn't support.
	 */
	if (phydev->autoneg == AUTONEG_DISABLE)
		return -EOPNOTSUPP;

	ret = genphy_c45_an_config_aneg(phydev);
	if (ret < 0)
		return ret;
	if (ret > 0)
		changed = true;

	/* Clause 45 doesn't define 1000BaseT support. Use Clause 22 instead in
	 * our design.
	 */
	adv = linkmode_adv_to_mii_ctrl1000_t(phydev->advertising);
	ret = phy_modify_changed(phydev, MII_CTRL1000, ADVERTISE_1000FULL, adv);
	if (ret < 0)
		return ret;
	if (ret > 0)
		changed = true;

	return genphy_c45_check_and_restart_aneg(phydev, changed);
}

static int mt798x_2p5ge_phy_get_features(struct phy_device *phydev)
{
	int ret;

	ret = genphy_c45_pma_read_abilities(phydev);
	if (ret)
		return ret;

	/* This phy can't handle collision, and neither can (XFI)MAC it's
	 * connected to. Although it can do HDX handshake, it doesn't support
	 * CSMA/CD that HDX requires.
	 */
	linkmode_clear_bit(ETHTOOL_LINK_MODE_100baseT_Half_BIT,
			   phydev->supported);

	return 0;
}

static int mt798x_2p5ge_phy_read_status(struct phy_device *phydev)
{
	int ret;

	/* When MDIO_STAT1_LSTATUS is raised genphy_c45_read_link(), this phy
	 * actually hasn't finished AN. So use CL22's link update function
	 * instead.
	 */
	ret = genphy_update_link(phydev);
	if (ret)
		return ret;

	phydev->speed = SPEED_UNKNOWN;
	phydev->duplex = DUPLEX_UNKNOWN;
	phydev->pause = 0;
	phydev->asym_pause = 0;

	/* We'll read link speed through vendor specific registers down below.
	 * So remove phy_resolve_aneg_linkmode (AN on) & genphy_c45_read_pma
	 * (AN off).
	 */
	if (phydev->autoneg == AUTONEG_ENABLE && phydev->autoneg_complete) {
		ret = genphy_c45_read_lpa(phydev);
		if (ret < 0)
			return ret;

		/* Clause 45 doesn't define 1000BaseT support. Read the link
		 * partner's 1G advertisement via Clause 22.
		 */
		ret = phy_read(phydev, MII_STAT1000);
		if (ret < 0)
			return ret;
		mii_stat1000_mod_linkmode_lpa_t(phydev->lp_advertising, ret);
	} else if (phydev->autoneg == AUTONEG_DISABLE) {
		linkmode_zero(phydev->lp_advertising);
	}

	if (phydev->link) {
		ret = phy_read(phydev, PHY_AUX_CTRL_STATUS);
		if (ret < 0)
			return ret;

		switch (FIELD_GET(PHY_AUX_SPEED_MASK, ret)) {
		case PHY_AUX_SPD_10:
			phydev->speed = SPEED_10;
			break;
		case PHY_AUX_SPD_100:
			phydev->speed = SPEED_100;
			break;
		case PHY_AUX_SPD_1000:
			phydev->speed = SPEED_1000;
			break;
		case PHY_AUX_SPD_2500:
			phydev->speed = SPEED_2500;
			break;
		}

		phydev->duplex = DUPLEX_FULL;
		/* FIXME:
		 * The current firmware always enables rate adaptation mode.
		 */
		phydev->rate_matching = RATE_MATCH_PAUSE;
	}
#if 1 // force_down
	if (!phydev->link)
		phydev->speed = -1;
#endif

	return 0;
}

static int mt798x_2p5ge_phy_get_rate_matching(struct phy_device *phydev,
					      phy_interface_t iface)
{
	return RATE_MATCH_PAUSE;
}

static int mt798x_2p5ge_phy_probe(struct phy_device *phydev)
{
	struct mtk_i2p5ge_phy_priv *priv;

	priv = devm_kzalloc(&phydev->mdio.dev,
			    sizeof(struct mtk_i2p5ge_phy_priv), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	switch (phydev->drv->phy_id) {
	case MTK_2P5GPHY_ID_MT7988:
		/* The original hardware only sets MDIO_DEVS_PMAPMD */
		phydev->c45_ids.devices_in_package |= MDIO_DEVS_PCS |
						      MDIO_DEVS_AN |
						      MDIO_DEVS_VEND1 |
						      MDIO_DEVS_VEND2;
		break;
	default:
		return -EINVAL;
	}

	priv->fw_loaded = false;
	phydev->priv = priv;

	return 0;
}

static struct phy_driver mtk_2p5gephy_driver[] = {
	{
		PHY_ID_MATCH_MODEL(MTK_2P5GPHY_ID_MT7988),
		.name = "MediaTek MT7988 2.5GbE PHY",
		.probe = mt798x_2p5ge_phy_probe,
		.config_init = mt798x_2p5ge_phy_config_init,
		.config_aneg = mt798x_2p5ge_phy_config_aneg,
		.get_features = mt798x_2p5ge_phy_get_features,
		.read_status = mt798x_2p5ge_phy_read_status,
		.get_rate_matching = mt798x_2p5ge_phy_get_rate_matching,
		.suspend = genphy_suspend,
		.resume = genphy_resume,
		.read_page = mtk_phy_read_page,
		.write_page = mtk_phy_write_page,
	},
};

module_phy_driver(mtk_2p5gephy_driver);

static struct mdio_device_id __maybe_unused mtk_2p5ge_phy_tbl[] = {
	{ PHY_ID_MATCH_VENDOR(0x00339c00) },
	{ }
};

MODULE_DESCRIPTION("MediaTek 2.5Gb Ethernet PHY driver");
MODULE_AUTHOR("SkyLake Huang <SkyLake.Huang@mediatek.com>");
MODULE_LICENSE("GPL");

MODULE_DEVICE_TABLE(mdio, mtk_2p5ge_phy_tbl);
MODULE_FIRMWARE(MT7988_2P5GE_PMB_FW);
