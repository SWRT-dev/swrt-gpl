/*
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
 *
 *   Copyright (C) 2012 Daniel Schwierzeck <daniel.schwierzeck@googlemail.com>
 */

#include <linux/module.h>
#include <linux/phy.h>

#define MII_MMDCTRL		0x0d
#define MII_MMDDATA		0x0e

#define MII_GCTRL		0x09
#define MII_GCTRL_MSPT		BIT(10)	/* Master/Slave Port Type */
#define MII_GCTRL_MS		BIT(11)	/* Master/Slave Config Value */
#define MII_GCTRL_MSEN		BIT(12)	/* Master/Slave Manual Configuration Enable */

#define MII_VR9_11G_IMASK	0x19	/* interrupt mask */
#define MII_VR9_11G_ISTAT	0x1a	/* interrupt status */
#define MII_PHY_FWV		0x1e	/* firmware version */

#define INT_VR9_11G_WOL		BIT(15)	/* Wake-On-LAN */
#define INT_VR9_11G_ANE		BIT(11)	/* Auto-Neg error */
#define INT_VR9_11G_ANC		BIT(10)	/* Auto-Neg complete */
#define INT_VR9_11G_ADSC	BIT(5)	/* Link auto-downspeed detect */
#define INT_VR9_11G_DXMC	BIT(2)	/* Duplex mode change */
#define INT_VR9_11G_LSPC	BIT(1)	/* Link speed change */
#define INT_VR9_11G_LSTC	BIT(0)	/* Link state change */
#define INT_VR9_11G_MASK	(INT_VR9_11G_LSTC | INT_VR9_11G_ADSC)

#define ADVERTISED_MPD		BIT(10)	/* Multi-port device */

#define MMD_DEVAD		0x1f
#define MMD_ACTYPE_SHIFT	14
#define MMD_ACTYPE_ADDRESS	(0 << MMD_ACTYPE_SHIFT)
#define MMD_ACTYPE_DATA		(1 << MMD_ACTYPE_SHIFT)
#define MMD_ACTYPE_DATA_PI	(2 << MMD_ACTYPE_SHIFT)
#define MMD_ACTYPE_DATA_PIWR	(3 << MMD_ACTYPE_SHIFT)

/* p31g aneg dev */
#define MMD_DEVANEG		0x07
#define P31G_ANEG_MGBT_AN_CTRL	0x20
#define CTRL_AB_2G5BT_BIT	BIT(7)
#define CTRL_AB_FR_2G5BT	BIT(5)
#define P31G_ANEG_MGBT_AN_STAT	0x21
#define STAT_AB_2G5BT_BIT	BIT(5)
#define STAT_AB_FR_2G5BT	BIT(3)
#define MMD_DEVVSPEC1			0x1E
#define VSPEC1_PM_CTRL			0x0C
#define VSPEC1_PM_CTRL_SYNCE_EN		BIT(4)
#define VSPEC1_PM_CTRL_SYNCE_CLK_MASK	GENMASK(5, 6)
#define VSPEC1_PM_CTRL_SYNCE_CLK_PSTN	(0 << 5) /* 8kHz clock */
#define VSPEC1_PM_CTRL_SYNCE_CLK_EEC1	(1 << 5) /* 2048kHz clock */
#define VSPEC1_PM_CTRL_SYNCE_CLK_EEC2	(2 << 5) /* 1544kHz clock */

static __maybe_unused int vr9_gphy_mmd_read(struct phy_device *phydev,
						u16 regnum)
{
	phy_write(phydev, MII_MMDCTRL, MMD_ACTYPE_ADDRESS | MMD_DEVAD);
	phy_write(phydev, MII_MMDDATA, regnum);
	phy_write(phydev, MII_MMDCTRL, MMD_ACTYPE_DATA | MMD_DEVAD);

	return phy_read(phydev, MII_MMDDATA);
}

static __maybe_unused int vr9_gphy_mmd_write(struct phy_device *phydev,
						u16 regnum, u16 val)
{
	phy_write(phydev, MII_MMDCTRL, MMD_ACTYPE_ADDRESS | MMD_DEVAD);
	phy_write(phydev, MII_MMDDATA, regnum);
	phy_write(phydev, MII_MMDCTRL, MMD_ACTYPE_DATA | MMD_DEVAD);
	phy_write(phydev, MII_MMDDATA, val);

	return 0;
}

static int
p31g_read_mmd_phyreg(struct phy_device *phydev, int regnum,
		     int devnum, int ptrad)
{
	return mdiobus_read(phydev->mdio.bus, phydev->mdio.addr,
				   MII_ADDR_C45 | (devnum << 16) | (regnum & 0xffff));
}

static void
p31g_write_mmd_phyreg(struct phy_device *phydev, int regnum,
		      int devnum, int ptrad, u32 val)
{
	regnum = MII_ADDR_C45 | ((devnum & 0x1f) << 16) | (regnum & 0xffff);
	mdiobus_write(phydev->mdio.bus, phydev->mdio.addr, regnum, val);
}

static int vr9_gphy_config_init(struct phy_device *phydev)
{
	int err;

	pr_debug("%s\n", __func__);

	/* Mask all interrupts */
	err = phy_write(phydev, MII_VR9_11G_IMASK, 0);
	if (err)
		return err;

	/* Clear all pending interrupts */
	phy_read(phydev, MII_VR9_11G_ISTAT);

	/* Fake clearing of interrupt */
	vr9_gphy_mmd_write(phydev, 0x707, 0x11);

	vr9_gphy_mmd_write(phydev, 0x1e0, 0xc5);
	vr9_gphy_mmd_write(phydev, 0x1e1, 0x67);
	vr9_gphy_mmd_write(phydev, 0x1e2, 0x42);
	vr9_gphy_mmd_write(phydev, 0x1e3, 0x10);
	vr9_gphy_mmd_write(phydev, 0x1e4, 0x70);
	vr9_gphy_mmd_write(phydev, 0x1e5, 0x03);
	vr9_gphy_mmd_write(phydev, 0x1e6, 0x20);
	vr9_gphy_mmd_write(phydev, 0x1e7, 0x00);
	vr9_gphy_mmd_write(phydev, 0x1e8, 0x40);
	vr9_gphy_mmd_write(phydev, 0x1e9, 0x20);

	return 0;
}

static int vr9_gphy_config_aneg(struct phy_device *phydev)
{
	int reg, err;

	/* Advertise as multi-port device */
	reg = phy_read(phydev, MII_CTRL1000);
	reg |= ADVERTISED_MPD;
	err = phy_write(phydev, MII_CTRL1000, reg);
	if (err)
		return err;

	return genphy_config_aneg(phydev);
}

static int vr9_gphy_ack_interrupt(struct phy_device *phydev)
{
	int reg;

	/*
	 * Possible IRQ numbers:
	 * - IM3_IRL18 for GPHY0
	 * - IM3_IRL17 for GPHY1
	 *
	 * Due to a silicon bug IRQ lines are not really independent from
	 * each other. Sometimes the two lines are driven at the same time
	 * if only one GPHY core raises the interrupt.
	 */

	reg = phy_read(phydev, MII_VR9_11G_ISTAT);

	/* Fake clearing of interrupt */
	vr9_gphy_mmd_write(phydev, 0x707, 0x11);

	return (reg < 0) ? reg : 0;
}

static int vr9_gphy_did_interrupt(struct phy_device *phydev)
{
	int reg;

	reg = phy_read(phydev, MII_VR9_11G_ISTAT);

	return reg > 0;
}

static int vr9_gphy_config_intr(struct phy_device *phydev)
{
	int err;

	if (phydev->interrupts == PHY_INTERRUPT_ENABLED) {
		/* Fake clearing of interrupt */
		vr9_gphy_mmd_write(phydev, 0x707, 0x11);
		err = phy_write(phydev, MII_VR9_11G_IMASK, INT_VR9_11G_MASK);
	} else {
		err = phy_write(phydev, MII_VR9_11G_IMASK, 0);
	}
	return err;
}

/* direct clause-45 read */
static __maybe_unused int p31g_gphy_mmd_read(struct phy_device *phydev,
					     u32 devad, u32 regnum)
{
	regnum = MII_ADDR_C45 | ((devad & 0x1f) << 16) |
		 (regnum & 0xffff);
	return phy_read(phydev, regnum);
}

/* direct clause-45 write */
static __maybe_unused int p31g_gphy_mmd_write(struct phy_device *phydev,
					      u32 devad, u32 regnum, u32 val)
{
	regnum = MII_ADDR_C45 | ((devad & 0x1f) << 16) |
		 (regnum & 0xffff);
	return phy_write(phydev, regnum, val);
}

static __maybe_unused int p31g_gphy_mmd_mask(struct phy_device *phydev,
					     u32 devad, u32 regnum,
					     u32 clear, u32 set)
{
	int val;

	val = p31g_gphy_mmd_read(phydev, devad, regnum);
	if (val < 0)
		return val;

	val &= ~(clear);
	val |= set;
	return p31g_gphy_mmd_write(phydev, devad, regnum, val);
}

static int p31g_gphy_config_init(struct phy_device *phydev)
{
	int err, val;

	vr9_gphy_config_init(phydev);

	/* Linux PHY framework does not have a way yet to define
	 * support for 2500baseT, therefore we set the bit directly here
	 */
	__set_bit(ETHTOOL_LINK_MODE_2500baseT_Full_BIT,
		  phydev->extended_supported);
	__set_bit(ETHTOOL_LINK_MODE_2500baseT_Full_BIT,
		  phydev->extended_advertising);

	dev_info(&phydev->mdio.dev, "fw version 0x%04X\n",
		 phy_read(phydev, MII_PHY_FWV));

	/* PRX300 only supports 2.048 MHz input clock, configure that */
	err = p31g_gphy_mmd_mask(phydev, MMD_DEVVSPEC1, VSPEC1_PM_CTRL,
				 VSPEC1_PM_CTRL_SYNCE_EN |
				 VSPEC1_PM_CTRL_SYNCE_CLK_MASK,
				 VSPEC1_PM_CTRL_SYNCE_EN |
				 VSPEC1_PM_CTRL_SYNCE_CLK_EEC1);
	if (err)
		return err;
	val = phy_read(phydev, MII_GCTRL);
	if (val < 0)
		return val;
	val |= MII_GCTRL_MS;
	val |= MII_GCTRL_MSEN;
	err = phy_write(phydev, MII_GCTRL, val);

	return err;
}

static int p31g_gphy_config_aneg(struct phy_device *phydev)
{
	int reg, old_reg, ret;
	bool support_2G5BT;

	/* configure 2.5G advertising */
	support_2G5BT = test_bit(ETHTOOL_LINK_MODE_2500baseT_Full_BIT,
				 phydev->extended_advertising);
	reg = p31g_gphy_mmd_read(phydev, MMD_DEVANEG, P31G_ANEG_MGBT_AN_CTRL);
	old_reg = reg;

	if (support_2G5BT)
		reg |= CTRL_AB_2G5BT_BIT | CTRL_AB_FR_2G5BT;
	else
		reg &= ~(CTRL_AB_2G5BT_BIT | CTRL_AB_FR_2G5BT);

	if (old_reg != reg)
		p31g_gphy_mmd_write(phydev, MMD_DEVANEG, P31G_ANEG_MGBT_AN_CTRL,
				    reg);

	ret = vr9_gphy_config_aneg(phydev);
	if (ret)
		return ret;

	/* restart aneg if 2.5G adv bit changes */
	if (phydev->autoneg == AUTONEG_ENABLE && (old_reg != reg))
		return genphy_restart_aneg(phydev);

	return 0;
}

static int p31g_gphy_read_status(struct phy_device *phydev)
{
	int ret, reg;

	ret = genphy_read_status(phydev);
	if (ret)
		return ret;

	/* check for 2.5G lp advertising and speed */
	reg = p31g_gphy_mmd_read(phydev, MMD_DEVANEG, P31G_ANEG_MGBT_AN_STAT);
	if (reg & (STAT_AB_2G5BT_BIT | STAT_AB_FR_2G5BT)) {
		__set_bit(ETHTOOL_LINK_MODE_2500baseT_Full_BIT,
			  phydev->extended_lp_advertising);

		reg = p31g_gphy_mmd_read(phydev, MMD_DEVANEG,
					 P31G_ANEG_MGBT_AN_CTRL);
		if (reg & (CTRL_AB_2G5BT_BIT | CTRL_AB_FR_2G5BT))
			phydev->speed = SPEED_2500;
	} else {
		__clear_bit(ETHTOOL_LINK_MODE_2500baseT_Full_BIT,
			    phydev->extended_lp_advertising);
	}

	return 0;
}

static struct phy_driver lantiq_phy[] = {
	{
		.phy_id		= 0xd565a400,
		.phy_id_mask	= 0xffffffff,
		.name		= "Lantiq XWAY PEF7071",
		.features	= (PHY_GBIT_FEATURES | SUPPORTED_Pause),
		.flags		= 0, /*PHY_HAS_INTERRUPT,*/
		.config_init	= vr9_gphy_config_init,
		.config_aneg	= vr9_gphy_config_aneg,
		.read_status	= genphy_read_status,
		.ack_interrupt	= vr9_gphy_ack_interrupt,
		.did_interrupt	= vr9_gphy_did_interrupt,
		.config_intr	= vr9_gphy_config_intr,
	}, {
		.phy_id		= 0xd565a401,
		.phy_id_mask	= 0xffffffff,
		.name		= "Intel GPHY PEF7072",
		.features	= (PHY_GBIT_FEATURES | SUPPORTED_Pause),
		.config_init	= vr9_gphy_config_init,
		.config_aneg	= vr9_gphy_config_aneg,
		.read_status	= genphy_read_status,
		.ack_interrupt	= vr9_gphy_ack_interrupt,
		.did_interrupt	= vr9_gphy_did_interrupt,
	}, {
		.phy_id		= 0x030260D0,
		.phy_id_mask	= 0xfffffff0,
		.name		= "Lantiq XWAY VR9 GPHY 11G v1.3",
		.features	= (PHY_GBIT_FEATURES | SUPPORTED_Pause),
		.flags		= 0, /*PHY_HAS_INTERRUPT,*/
		.config_init	= vr9_gphy_config_init,
		.config_aneg	= vr9_gphy_config_aneg,
		.read_status	= genphy_read_status,
		.ack_interrupt	= vr9_gphy_ack_interrupt,
		.did_interrupt	= vr9_gphy_did_interrupt,
		.config_intr	= vr9_gphy_config_intr,
	}, {
		.phy_id		= 0xd565a408,
		.phy_id_mask	= 0xfffffff8,
		.name		= "Lantiq XWAY VR9 GPHY 11G v1.4",
		.features	= (PHY_GBIT_FEATURES | SUPPORTED_Pause),
		.flags		= /*0,*/ PHY_HAS_INTERRUPT,
		.config_init	= vr9_gphy_config_init,
		.config_aneg	= vr9_gphy_config_aneg,
		.read_status	= genphy_read_status,
		.ack_interrupt	= vr9_gphy_ack_interrupt,
		/*.did_interrupt	= vr9_gphy_did_interrupt,*/
		.config_intr	= vr9_gphy_config_intr,
	}, {
		.phy_id		= 0xd565a418,
		.phy_id_mask	= 0xfffffff8,
		.name		= "Lantiq XWAY XRX PHY22F v1.4",
		.features	= (PHY_BASIC_FEATURES | SUPPORTED_Pause),
		.flags		= 0, /*PHY_HAS_INTERRUPT,*/
		.config_init	= vr9_gphy_config_init,
		.config_aneg	= vr9_gphy_config_aneg,
		.read_status	= genphy_read_status,
		.ack_interrupt	= vr9_gphy_ack_interrupt,
		.did_interrupt	= vr9_gphy_did_interrupt,
		.config_intr	= vr9_gphy_config_intr,
	}, {
		.phy_id		= 0x67c9dc00,
		.phy_id_mask	= 0xfffffff0,
		.name		= "Intel GPHY 31G",
		.features	= (PHY_GBIT_FEATURES | SUPPORTED_Pause),
		.flags		= PHY_HAS_INTERRUPT,
		.config_init	= p31g_gphy_config_init,
		.config_aneg	= p31g_gphy_config_aneg,
		.read_status	= p31g_gphy_read_status,
		.ack_interrupt	= vr9_gphy_ack_interrupt,
		.config_intr	= vr9_gphy_config_intr,
		.read_mmd_indirect = p31g_read_mmd_phyreg,
		.write_mmd_indirect = p31g_write_mmd_phyreg,
	},
};

#if 0
static int __init ltq_phy_init(void)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(lantiq_phy); i++) {
		int err = phy_driver_register(&lantiq_phy[i]);
		if (err)
			pr_err("lantiq_phy: failed to load %s\n", lantiq_phy[i].name);
	}

	pr_info("%s success !\n", __func__);

	return 0;
}

static void __exit ltq_phy_exit(void)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(lantiq_phy); i++)
		phy_driver_unregister(&lantiq_phy[i]);
}
#endif

module_phy_driver(lantiq_phy);

MODULE_DESCRIPTION("Lantiq PHY drivers");
MODULE_AUTHOR("Daniel Schwierzeck <daniel.schwierzeck@googlemail.com>");
MODULE_LICENSE("GPL");
