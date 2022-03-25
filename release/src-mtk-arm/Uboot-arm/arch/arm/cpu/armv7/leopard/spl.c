/*
 * Copyright 2010-2018 MediaTek, Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * Version 2 as published by the Free Software Foundation.
 */

#include <common.h>
#include <spl.h>
#include <config.h>
#include <malloc.h>
#include <asm/io.h>
#include <asm/arch/leopard.h>
#include <asm/arch/spm_mtcmos.h>
#include <asm/arch/wdt.h>
#include <asm/arch/timer.h>

DECLARE_GLOBAL_DATA_PTR;

static void arch_early_init(void);
static void platform_early_init(void);

extern char __image_copy_start[];
extern char __image_copy_end[];

/* Used for lzma decompression */
static size_t spl_malloc_base;

void *sys_lzma_malloc(size_t size)
{
	spl_malloc_base -= size;
	return (void *) spl_malloc_base;
}

void sys_lzma_free(void *address)
{
}

#ifdef ON_BOARD_SPI_NOR_FLASH_COMPONENT
#define LEOPARD_NOR_HEADER_SIZE			0x920

size_t spl_nor_uboot_base(void)
{
	return CONFIG_NOR_BASE + \
		((size_t) __image_copy_end - (size_t) __image_copy_start) + \
		LEOPARD_NOR_HEADER_SIZE;
}

size_t spl_nor_uboot_max_size(void)
{
	return CONFIG_NOR_BOOTLOADER_SIZE - \
		((size_t) __image_copy_end - (size_t) __image_copy_start) - \
		LEOPARD_NOR_HEADER_SIZE;
}

size_t spl_nor_uboot_alignment(void)
{
	return CONFIG_SPL_PAD_ALIGNMENT;
}
#endif /* ON_BOARD_SPI_NOR_FLASH_COMPONENT */

#if defined(ON_BOARD_NAND_FLASH_COMPONENT) || \
      defined(ON_BOARD_SPI_NAND_FLASH_COMPONENT)
size_t spl_nand_uboot_base(void)
{
	return \
		((size_t) __image_copy_end - (size_t) __image_copy_start);
}

size_t spl_nand_uboot_max_size(void)
{
	return CONFIG_MAX_BOOTLOADER_SIZE - \
		((size_t) __image_copy_end - (size_t) __image_copy_start);
}

size_t spl_nand_uboot_alignment(void)
{
	return CONFIG_SPL_PAD_ALIGNMENT;
}
#endif /* ON_BOARD_NAND_FLASH_COMPONENT || ON_BOARD_SPI_NAND_FLASH_COMPONENT */

void board_init_f(ulong dummy)
{

	/* Clear the BSS. */
	memset(__bss_start, 0, __bss_end - __bss_start);

	/* Set global data pointer. */
	gd = &gdata;

	arch_early_init();
	platform_early_init();

	board_init_r(NULL, 0);
}

/*
 * GIC settings must be done in secure-world.
 */
__weak int arm_gic_init(void)
{
	int i;
	int max_irq = 256; /* leopard max irq number */

	/*
	 * GIC Distributor initialization(for global setting).
	 * Set all global interrupts in non-security world access.
	 */
	for (i = 0; i < max_irq; i += 32) {
		writel(0x00000000, (SEC_POL_CTL_EN0 + i * 4 / 32));
		writel(0xFFFFFFFF,
			(GIC_DIST_BASE + GIC_DIST_SECURITY_SET + i * 4 / 32));
	}

	/*
	 * GIC CPU-interface initialization (for only each Cores),
	 * gic_cpu_init() has been done in lowlevel_init() function.
	 */

	return 0;
}

__weak void mt_pll_init(void)
{
	/* if > 900MHz, need to change CPU:CCI clock ratio to 1:2 (from 1:1),
	 * that is, set 0x10200640[4:0]=0x12
	 */
	writel((readl(ACLKEN_DIV) & ~0x1F) | 0x12, ACLKEN_DIV);
	gpt_busy_wait_us(10);
	writel((readl(MCU_BUS_MUX) & ~0x600) | 0x200, MCU_BUS_MUX);

	return;
}

__weak void mt_pll_post_init(void)
{
	return;
}

__weak void mt_set_emi(void)
{
	return;
}

__weak int mtk_nor_init(void)
{
	return 0;
}

static void print_chip_id(void)
{
	u32 hw_code, hw_subcode;
	u32 hw_version, sw_version;

	hw_code = readl(APHW_CODE);
	hw_subcode = readl(APHW_SUBCODE);
	hw_version = readl(APHW_VER);
	sw_version = readl(APSW_VER);

	printf("Hello, MT%x %s\n", hw_code,
			(sw_version == 0x00) ? "E1" : "E2");
	debug("hw_subcode = 0x%X, hw_version = 0x%X, sw_version = 0x%X.\n",
			hw_subcode, hw_version, sw_version);
	return;
}

/*
 * Data coherence initialization for different clusters.
 */
void cci400_snoop_init(void)
{
	/* Enable CA7 snoop function */
	writel(readl(MP0_AXI_CONFIG) & ~MP0_ACINACTM, MP0_AXI_CONFIG);

	/* Enable snoop requests and DVM message requests */
	writel(readl(CCI400_SI4_SNOOP_CONTROL) | (SNOOP_REQ | DVM_MSG_REQ),
		   CCI400_SI4_SNOOP_CONTROL);
	while (readl(CCI400_STATUS) & CHANGE_PENDING)
		;
}

/*
 * Essential misc setting initialization in the earliest phase.
 */
static void mtk_misc_init(void)
{
	cci400_snoop_init();

	/* CKGEN: CLK_CFG_8 for GPT clock-select setting.
	 * bit16 - clk_10m_sel
	 * 0: clkxtal_40Mhz,
	 * 1: clkxtal_d4_10Mhz
	 */
	writel(0x00010000, (CKSYS_BASE + 0xC0));

	/* DeviceAPC: initial setting.
	 * Enable master Ethsys/WIFI/USB/PCIE transaction working in all secure-mode.
	 * regs w 0x10007500  4000 : Ethsys,
	 * regs w 0x10007500  0800 : WIFI,
	 * regs w 0x10007500  8000 : USB/PCIE,
	 */
	writel(0x00000000, (DEVICE_APC_BASE + 0x0F00));
	writel(0x0000C800, (DEVICE_APC_BASE + 0x0500));
}

static void arch_early_init(void)
{
	arch_cpu_init();
	/*arm_gic_init();*/
	preloader_console_init();
	print_chip_id();
#if !defined(MT7626_ASIC_BOARD)
	spm_mtcmos_cpu1_off();
#endif
}

static void platform_early_init(void)
{
	mtk_misc_init();
	mtk_timer_init();

#if !CFG_FPGA_PLATFORM
	/* init basic pll */
	mt_pll_init();
	mt_pll_post_init();
#endif
	/* init AP watchdog and set timeout to 10 secs */
	mtk_wdt_init();
}

static int spl_early_led_on(void)
{
	return 0;
}

void spl_board_init(void)
{
	u32 dram_size;

	spl_early_led_on();
#if CFG_FPGA_PLATFORM
	/* DramK On HAPS FPGA by CMM script */
#else
	mt_set_emi();
#endif

	/* Prepare allocatable memory for u-boot decompression */
#if CONFIG_CUSTOMIZE_DRAM_SIZE
	dram_size = CONFIG_SYS_SDRAM_SIZE;
#else
	if (!gd->ram_size)
		gd->ram_size = get_ram_size((long *) CONFIG_SYS_SDRAM_BASE, SZ_256M);
	dram_size = gd->ram_size;
#endif

	/* Set allocatable memory to the highest memory */
	spl_malloc_base = CONFIG_SYS_SDRAM_BASE + dram_size - 0x1000;

#if defined(CONFIG_SPL_RAM_DEVICE)
	return;
#elif defined(CONFIG_SPL_NOR_SUPPORT)
	/* mtk_nor_init(); */
#else
	/* nand_init();
	 * which has been done in spl_nand_load_image().
	 */
#endif
}

u32 spl_boot_device(void)
{
	int mode = -1;

#if defined(CONFIG_SPL_RAM_DEVICE)
	mode = BOOT_DEVICE_RAM;
#elif defined(CONFIG_SPL_NOR_SUPPORT)
	mode = BOOT_DEVICE_NOR;
#elif defined(CONFIG_SPL_NAND_SUPPORT)
	mode = BOOT_DEVICE_NAND;
#else
	mode = BOOT_DEVICE_NONE;
#endif
	if (mode == BOOT_DEVICE_NONE) {
		puts("Unsupported boot mode selected\n");
		hang();
	}

	return mode;
}

int spl_start_uboot(void)
{
#ifndef CONFIG_SPL_DECOMPRESS_UBOOT
	puts("SPL: Direct load U-Boot image!\n");
#else
	puts("SPL: Decompressing U-Boot image!\n");
#endif
	return 1;
}


typedef void __noreturn (*image_entry_noargs_t)(void);

/*
 * execute_will() function is moved to OnChip-SRAM to run.
 * This code cannot call any subfunction, and it must be a leaf function.
 * Also we can use the macro-definition substitued.
 */
void __noreturn execute_will(void)
{
	image_entry_noargs_t uboot_entry;

	/*
	 * Return shared-L2sram allocated on RomCode phase,
	 * this operation must be in secure world.
	 */
	isb();
	writel(0x00000100, L2C_CFG_MP0);
	isb();
#if 0 /* Always running in secure mode */
	/*
	 * Switch secure-mode to non-secure mode:
	 * 1. switch to monitor mode
	 * 2. force set SCR.NS=1
	 * 3. switch to original mode
	 */
	asm volatile (
		"mrs r1, cpsr\n"
		"bic r0, r1, #0x1f\n"
		"orr r0, r0, #0x16\n"
		"msr cpsr, r0\n"
		"mrc p15, 0, r2, c1, c1, 0\n"
		"orr r0, r2, #0x1\n"
		"mcr p15, 0, r0, c1, c1, 0\n"
		"msr cpsr, r1\n"
	);
#endif
	/*
	 * Jump u-boot entry got from sram data region,
	 * here we are in non-secure world now...
	 */
	isb();
	uboot_entry = (image_entry_noargs_t)(*(u32 *)JUMP_DADA_REGION);
	isb();
	uboot_entry();
}

/*
 * Here jump to OnChip-SRAM 0x102000 for transfer station.
 * SPL run on L2sram -> OnChip sram -> dram(u-boot).
 */
void __noreturn jump_to_image_no_args(struct spl_image_info *spl_image)
{
	image_entry_noargs_t jump_entry;
	u32 uboot_entry = spl_image->entry_point;

	printf("image entry point: 0x%X\n", spl_image->entry_point);

	memcpy((void *)JUMP_DADA_REGION, (void *)&uboot_entry, sizeof(u32));
	memcpy((void *)JUMP_CODE_REGION, (void *)execute_will, JUMP_CODE_SIZE);

	jump_entry = (image_entry_noargs_t)((void *)JUMP_CODE_REGION);
	isb();
	jump_entry();
}

