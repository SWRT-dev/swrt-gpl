/*
 * (C) Copyright 2018 MediaTek.Inc
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <common.h>
#include <config.h>

#include <asm/arch/timer.h>
#include <asm/arch/wdt.h>
#include <asm/arch/leopard.h>
#include <asm/arch/gpio.h>


DECLARE_GLOBAL_DATA_PTR;

extern int rt2880_eth_initialize(bd_t *bis);

/*
 *  Iverson 20140326 : DRAM have been initialized in SPL preloader.
 */
int dram_init(void)
{
    /*
     * UBoot support memory auto detection.
     * So now support both static declaration and auto detection for DRAM size
     */
#if CONFIG_CUSTOMIZE_DRAM_SIZE
    gd->ram_size = CONFIG_SYS_SDRAM_SIZE;
    printf("static declaration g_total_rank_size = 0x%8X\n", (int)gd->ram_size);
#else
    gd->ram_size = get_ram_size((long *)CONFIG_SYS_SDRAM_BASE, SZ_512M);
    printf("auto detection g_total_rank_size = 0x%8X\n", (int)gd->ram_size);
#endif

	return 0;
}

static void pin_mux_init(void)
{
	mt_set_gpio_mode(GPIO53, GPIO_MODE_06);
	mt_set_gpio_mode(GPIO54, GPIO_MODE_06);
	mt_set_gpio_mode(GPIO55, GPIO_MODE_06);
	mt_set_gpio_mode(GPIO56, GPIO_MODE_06);
	mt_set_gpio_mode(GPIO57, GPIO_MODE_06);
	mt_set_gpio_mode(GPIO58, GPIO_MODE_06);
	mt_set_gpio_mode(GPIO59, GPIO_MODE_06);
}

static void pcie_init(void)
{
	int val;

	/* reset pcie phy for low power mode*/
	val = READ_REGISTER_UINT32(PHY_RST_CTL);
	WRITE_REGISTER_UINT32(PHY_RST_CTL, val & ~(1<<3));
}

int board_init(void)
{
	/* mtk_timer_init(); */

#ifdef CONFIG_WATCHDOG_OFF
	mtk_wdt_disable();
#endif
	/* Dehui: set GPIO53~GPIO59 to AUX6 for N9 JTAG2 */
	pin_mux_init();

	/* Jianjun: reset pcie phy */
	pcie_init();

	/* Nelson: address of boot parameters */
	gd->bd->bi_boot_params = CONFIG_SYS_SDRAM_BASE + 0x100;

	return 0;
}

int board_late_init (void)
{
    /* To load environment variable from persistent store */
    gd->env_valid = 1;
    env_relocate();
    return 0;
}

/*
 *  Iverson todo
 */

void ft_board_setup(void *blob, bd_t *bd)
{
}

#ifndef CONFIG_SYS_DCACHE_OFF
void enable_caches(void)
{
    /* Enable D-cache. I-cache is already enabled in start.S */
    dcache_enable();
}
#endif

int board_eth_init(bd_t *bis)
{
#ifdef CONFIG_RT2880_ETH
    rt2880_eth_initialize(bis);
#endif
    return 0;
}

