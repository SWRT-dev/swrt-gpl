/*
 * evm.c
 *
 * Board functions for TI814x EVM
 *
 * Copyright (C) 2011, Texas Instruments, Incorporated - http://www.ti.com/
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR /PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <common.h>
#include <cpsw.h>
#include <errno.h>
#include <spl.h>
#include <asm/arch/cpu.h>
#include <asm/arch/hardware.h>
#include <asm/arch/omap.h>
#include <asm/arch/ddr_defs.h>
#include <asm/arch/clock.h>
#include <asm/arch/gpio.h>
#include <asm/arch/mmc_host_def.h>
#include <asm/arch/sys_proto.h>
#include <asm/io.h>
#include <asm/emif.h>
#include <asm/gpio.h>
#include "evm.h"

DECLARE_GLOBAL_DATA_PTR;

#ifdef CONFIG_SPL_BUILD
static struct wd_timer *wdtimer = (struct wd_timer *)WDT_BASE;
static struct uart_sys *uart_base = (struct uart_sys *)DEFAULT_UART_BASE;
#endif

static struct ctrl_dev *cdev = (struct ctrl_dev *)CTRL_DEVICE_BASE;

/* UART Defines */
#ifdef CONFIG_SPL_BUILD
#define UART_RESET		(0x1 << 1)
#define UART_CLK_RUNNING_MASK	0x1
#define UART_SMART_IDLE_EN	(0x1 << 0x3)

static void rtc32k_enable(void)
{
	struct rtc_regs *rtc = (struct rtc_regs *)RTC_BASE;

	/*
	 * Unlock the RTC's registers.  For more details please see the
	 * RTC_SS section of the TRM.  In order to unlock we need to
	 * write these specific values (keys) in this order.
	 */
	writel(0x83e70b13, &rtc->kick0r);
	writel(0x95a4f1e0, &rtc->kick1r);

	/* Enable the RTC 32K OSC by setting bits 3 and 6. */
	writel((1 << 3) | (1 << 6), &rtc->osc);
}

static void uart_enable(void)
{
	u32 regVal;

	/* UART softreset */
	regVal = readl(&uart_base->uartsyscfg);
	regVal |= UART_RESET;
	writel(regVal, &uart_base->uartsyscfg);
	while ((readl(&uart_base->uartsyssts) &
		UART_CLK_RUNNING_MASK) != UART_CLK_RUNNING_MASK)
		;

	/* Disable smart idle */
	regVal = readl(&uart_base->uartsyscfg);
	regVal |= UART_SMART_IDLE_EN;
	writel(regVal, &uart_base->uartsyscfg);
}

static void wdt_disable(void)
{
	writel(0xAAAA, &wdtimer->wdtwspr);
	while (readl(&wdtimer->wdtwwps) != 0x0)
		;
	writel(0x5555, &wdtimer->wdtwspr);
	while (readl(&wdtimer->wdtwwps) != 0x0)
		;
}

static const struct cmd_control evm_ddr2_cctrl_data = {
	.cmd0csratio	= 0x80,
	.cmd0dldiff	= 0x04,
	.cmd0iclkout	= 0x00,

	.cmd1csratio	= 0x80,
	.cmd1dldiff	= 0x04,
	.cmd1iclkout	= 0x00,

	.cmd2csratio	= 0x80,
	.cmd2dldiff	= 0x04,
	.cmd2iclkout	= 0x00,
};

static const struct emif_regs evm_ddr2_emif0_regs = {
	.sdram_config			= 0x40801ab2,
	.ref_ctrl			= 0x10000c30,
	.sdram_tim1			= 0x0aaaf552,
	.sdram_tim2			= 0x043631d2,
	.sdram_tim3			= 0x00000327,
	.emif_ddr_phy_ctlr_1		= 0x00000007
};

static const struct emif_regs evm_ddr2_emif1_regs = {
	.sdram_config			= 0x40801ab2,
	.ref_ctrl			= 0x10000c30,
	.sdram_tim1			= 0x0aaaf552,
	.sdram_tim2			= 0x043631d2,
	.sdram_tim3			= 0x00000327,
	.emif_ddr_phy_ctlr_1		= 0x00000007
};

const struct dmm_lisa_map_regs evm_lisa_map_regs = {
	.dmm_lisa_map_0			= 0x00000000,
	.dmm_lisa_map_1			= 0x00000000,
	.dmm_lisa_map_2			= 0x806c0300,
	.dmm_lisa_map_3			= 0x806c0300,
};

static const struct ddr_data evm_ddr2_data = {
	.datardsratio0		= ((0x35<<10) | (0x35<<0)),
	.datawdsratio0		= ((0x20<<10) | (0x20<<0)),
	.datawiratio0		= ((0<<10) | (0<<0)),
	.datagiratio0		= ((0<<10) | (0<<0)),
	.datafwsratio0		= ((0x90<<10) | (0x90<<0)),
	.datawrsratio0		= ((0x50<<10) | (0x50<<0)),
	.datauserank0delay	= 1,
	.datadldiff0		= 0x4,
};
#endif

/*
 * early system init of muxing and clocks.
 */
void s_init(void)
{
#ifdef CONFIG_SPL_BUILD
	/*
	 * Save the boot parameters passed from romcode.
	 * We cannot delay the saving further than this,
	 * to prevent overwrites.
	 */
#ifdef CONFIG_SPL_BUILD
	save_omap_boot_params();
#endif

	/* WDT1 is already running when the bootloader gets control
	 * Disable it to avoid "random" resets
	 */
	wdt_disable();

	/* Enable timer */
	timer_init();

	/* Setup the PLLs and the clocks for the peripherals */
	pll_init();

	/* Enable RTC32K clock */
	rtc32k_enable();

	/* Set UART pins */
	enable_uart0_pin_mux();

	/* Set MMC pins */
	enable_mmc1_pin_mux();

	/* Set Ethernet pins */
	enable_enet_pin_mux();

	/* Enable UART */
	uart_enable();

	gd = &gdata;

	preloader_console_init();

	config_dmm(&evm_lisa_map_regs);

	config_ddr(0, 0, &evm_ddr2_data, &evm_ddr2_cctrl_data,
		   &evm_ddr2_emif0_regs, 0);
	config_ddr(0, 0, &evm_ddr2_data, &evm_ddr2_cctrl_data,
		   &evm_ddr2_emif1_regs, 1);
#endif
}

/*
 * Basic board specific setup.  Pinmux has been handled already.
 */
int board_init(void)
{
	gd->bd->bi_boot_params = PHYS_DRAM_1 + 0x100;
	return 0;
}

#if !defined(CONFIG_SPL_BUILD) && defined(CONFIG_GENERIC_MMC)
int board_mmc_init(bd_t *bis)
{
	omap_mmc_init(1, 0, 0, -1, -1);

	return 0;
}
#endif

#ifdef CONFIG_DRIVER_TI_CPSW
static void cpsw_control(int enabled)
{
	/* VTP can be added here */

	return;
}

static struct cpsw_slave_data cpsw_slaves[] = {
	{
		.slave_reg_ofs	= 0x50,
		.sliver_reg_ofs	= 0x700,
		.phy_id		= 1,
	},
	{
		.slave_reg_ofs	= 0x90,
		.sliver_reg_ofs	= 0x740,
		.phy_id		= 0,
	},
};

static struct cpsw_platform_data cpsw_data = {
	.mdio_base		= CPSW_MDIO_BASE,
	.cpsw_base		= CPSW_BASE,
	.mdio_div		= 0xff,
	.channels		= 8,
	.cpdma_reg_ofs		= 0x100,
	.slaves			= 1,
	.slave_data		= cpsw_slaves,
	.ale_reg_ofs		= 0x600,
	.ale_entries		= 1024,
	.host_port_reg_ofs	= 0x28,
	.hw_stats_reg_ofs	= 0x400,
	.mac_control		= (1 << 5),
	.control		= cpsw_control,
	.host_port_num		= 0,
	.version		= CPSW_CTRL_VERSION_1,
};
#endif

int board_eth_init(bd_t *bis)
{
	uint8_t mac_addr[6];
	uint32_t mac_hi, mac_lo;

	if (!eth_getenv_enetaddr("ethaddr", mac_addr)) {
		printf("<ethaddr> not set. Reading from E-fuse\n");
		/* try reading mac address from efuse */
		mac_lo = readl(&cdev->macid0l);
		mac_hi = readl(&cdev->macid0h);
		mac_addr[0] = mac_hi & 0xFF;
		mac_addr[1] = (mac_hi & 0xFF00) >> 8;
		mac_addr[2] = (mac_hi & 0xFF0000) >> 16;
		mac_addr[3] = (mac_hi & 0xFF000000) >> 24;
		mac_addr[4] = mac_lo & 0xFF;
		mac_addr[5] = (mac_lo & 0xFF00) >> 8;

		if (is_valid_ether_addr(mac_addr))
			eth_setenv_enetaddr("ethaddr", mac_addr);
		else
			printf("Unable to read MAC address. Set <ethaddr>\n");
	}

	return cpsw_register(&cpsw_data);
}
