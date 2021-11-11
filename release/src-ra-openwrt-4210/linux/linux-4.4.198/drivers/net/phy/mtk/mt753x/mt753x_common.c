/*
 * Common part for MediaTek MT753x gigabit switch
 *
 * Copyright (C) 2018 MediaTek Inc. All Rights Reserved.
 *
 * Author: Weijie Gao <weijie.gao@mediatek.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <linux/kernel.h>
#include <linux/delay.h>

#include "mt753x.h"
#include "mt753x_regs.h"

void mt753x_irq_enable(struct gsw_mt753x *gsw)
{
	u32 val;
	int i;

	/* Record initial PHY link status */
	for (i = 0; i < MT753X_NUM_PHYS; i++) {
		val = gsw->mii_read(gsw, i, MII_BMSR);
		if (val & BMSR_LSTATUS)
			gsw->phy_link_sts |= BIT(i);
	}

	val = BIT(MT753X_NUM_PHYS) - 1;

	mt753x_reg_write(gsw, SYS_INT_EN, val);
}

static void display_port_link_status(struct gsw_mt753x *gsw, u32 port)
{
	u32 pmsr, speed_bits;
	const char *speed;

	pmsr = mt753x_reg_read(gsw, PMSR(port));

	speed_bits = (pmsr & MAC_SPD_STS_M) >> MAC_SPD_STS_S;

	switch (speed_bits) {
	case MAC_SPD_10:
		speed = "10Mbps";
		break;
	case MAC_SPD_100:
		speed = "100Mbps";
		break;
	case MAC_SPD_1000:
		speed = "1Gbps";
		break;
	case MAC_SPD_2500:
		speed = "2.5Gbps";
		break;
	}

	if (pmsr & MAC_LNK_STS) {
	dev_info(gsw->dev, "Port %d Link is Up - %s/%s\n",
		 port, speed, (pmsr & MAC_DPX_STS) ? "Full" : "Half");
	} else {
		dev_info(gsw->dev, "Port %d Link is Down\n", port);
	}
}

void mt753x_irq_worker(struct work_struct *work)
{
	struct gsw_mt753x *gsw;
	u32 sts, physts, laststs;
	int i;
#if defined(ASUS_EXT)
#if defined(CONFIG_MODEL_RTAX54)
	int led_en, led_iomode, led_gpio_mode, led_gpio_oe, led_value;
#endif
#endif

	gsw = container_of(work, struct gsw_mt753x, irq_worker);

	sts = mt753x_reg_read(gsw, SYS_INT_STS);

#if defined(ASUS_EXT)
#if defined(CONFIG_MODEL_RTAX54)
	led_en = mt753x_reg_read(gsw, 0x7d00);	//LED_EN: 1=enable 0=disable (default: 77777)
	led_iomode = mt753x_reg_read(gsw, 0x7d04);	//LED_IO_MODE: 0=GPIO 1=PHY mode (default: 77777)
	led_gpio_mode = mt753x_reg_read(gsw, 0x7d10); //GPIOMODE:1=output 0=input (default: 77777)
	led_gpio_oe = mt753x_reg_read(gsw, 0x7d14);	//GPIO_OE:1=output enable  0=output disable  (default: 0)
	led_value = mt753x_reg_read(gsw, 0x7d18);	//GPIO_DATA: 1=High 0=Low (This model is Low Active) (default: 77777)

	led_en &= 0x11111;
	led_iomode &= 0x66666;
	led_gpio_mode &= 0x11111;
	led_gpio_oe = 0x11111;
#endif
#endif
	/* Check for changed PHY link status */
	for (i = 0; i < MT753X_NUM_PHYS; i++) {
		if (!(sts & PHY_LC_INT(i)))
			continue;

		laststs = gsw->phy_link_sts & BIT(i);
		physts = !!(gsw->mii_read(gsw, i, MII_BMSR) & BMSR_LSTATUS);
		physts <<= i;

		if (physts ^ laststs) {
			gsw->phy_link_sts ^= BIT(i);
			display_port_link_status(gsw, i);
		}
	}

#if defined(ASUS_EXT)
#if defined(CONFIG_MODEL_RTAX54)
	if(gsw->phy_link_sts & 0xF)
		led_value &= ~(0x1 << 4);
	else
		led_value |= (0x1 << 4);

	if(gsw->phy_link_sts & 0x10)
		led_value &= ~0x1;
	else
		led_value |= 0x1;

	mt753x_reg_write(gsw, 0x7d00, led_en);
	mt753x_reg_write(gsw, 0x7d04, led_iomode);
	mt753x_reg_write(gsw, 0x7d10, led_gpio_mode);
	mt753x_reg_write(gsw, 0x7d14, led_gpio_oe);
	mt753x_reg_write(gsw, 0x7d18, led_value);
#endif
#endif

	mt753x_reg_write(gsw, SYS_INT_STS, sts);

	enable_irq(gsw->irq);
}
