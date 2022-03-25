#include <common.h>
#include <config.h>

#include <asm/arch/typedefs.h>
#include <asm/arch/timer.h>
#include <asm/arch/wdt.h>
#include <asm/arch/mt6735.h>

DECLARE_GLOBAL_DATA_PTR;

extern int rt2880_eth_initialize(bd_t *bis);
/*
 *  Iverson 20140326 : DRAM have been initialized in preloader.
 */

int dram_init(void)
{
    /*
     * UBoot support memory auto detection.
     * So now support both static declaration and auto detection for DRAM size
     */
#if CONFIG_CUSTOMIZE_DRAM_SIZE
    gd->ram_size = CONFIG_SYS_SDRAM_SIZE - SZ_16M;
    printf("static declaration g_total_rank_size = 0x%8X\n", (int)gd->ram_size);
#else
    gd->ram_size = get_ram_size((long *)CONFIG_SYS_SDRAM_BASE,0x80000000) - SZ_16M;
    printf("auto detection g_total_rank_size = 0x%8X\n", (int)gd->ram_size);
#endif

	return 0;
}

int board_init(void)
{
    mtk_timer_init();
    
#ifdef CONFIG_WATCHDOG_OFF
    mtk_wdt_disable();
#endif

    /* Nelson: address of boot parameters */
	gd->bd->bi_boot_params = CONFIG_SYS_SDRAM_BASE + 0x100;

    return 0;
}

int board_late_init (void)
{
    gd->env_valid = 1; //to load environment variable from persistent store
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

