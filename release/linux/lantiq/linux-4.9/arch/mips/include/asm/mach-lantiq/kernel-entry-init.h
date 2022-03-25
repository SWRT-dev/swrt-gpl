/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Chris Dearman (chris@mips.com)
 * Leonid Yegoshin (yegoshin@mips.com)
 * Copyright (C) 2012 Mips Technologies, Inc.
 */
#ifndef __ASM_MACH_LANTIQ_KERNEL_ENTRY_INIT_H
#define __ASM_MACH_LANTIQ_KERNEL_ENTRY_INIT_H

	/*
	 * Prepare segments for EVA boot:
	 *
	 * This is in case the processor boots in legacy configuration
	 * (SI_EVAReset is de-asserted and CONFIG5.K == 0)
	 *
	 * On entry, t1 is loaded with CP0_CONFIG
	 *
	 * ========================= Mappings =============================
	 * Virtual memory           Physical memory           Mapping
	 * 0x20000000 - 0x9fffffff  0x20000000 - 0x9ffffffff   MUSUK (kuseg)
	 *                          Flat 2GB physical memory
	 *
	 * 0x80000000 - 0x9fffffff  0x80000000 - 0x9ffffffff   MUSUK (kseg0)
	 * 0xa0000000 - 0xbf000000  0x00000000 - 0x1ffffffff   MUSUK (kseg1)
	 * 0xc0000000 - 0xdfffffff             -                 MK  (kseg2)
	 * 0xe0000000 - 0xffffffff  0xa0000000 - 0xbfffffff    UK    (kseg3)
	 *
	 *
	 * Lowmem is expanded to 2GB
	 * The last 64KB of physical memory are reserved for correct HIGHMEM
	 * macros arithmetics.
	 */

#ifdef CONFIG_LTQ_EVA_2GB
	.macro  platform_eva_init

	.set    push
	.set    reorder
	/*
	 * Get Config.K0 value and use it to program
	 * the segmentation registers
	 */
	mfc0    t1, CP0_CONFIG
	andi    t1, 0x7 /* CCA */
	move    t2, t1
	ins     t2, t1, 16, 3
	/* SegCtl0 */
	li      t0, ((MIPS_SEGCFG_UK << MIPS_SEGCFG_AM_SHIFT) |              \
		(5 << MIPS_SEGCFG_PA_SHIFT) | (2 << MIPS_SEGCFG_C_SHIFT) |   \
		(1 << MIPS_SEGCFG_EU_SHIFT)) |                               \
		(((MIPS_SEGCFG_MSK << MIPS_SEGCFG_AM_SHIFT) |                \
		(0 << MIPS_SEGCFG_PA_SHIFT) |                                \
		(1 << MIPS_SEGCFG_EU_SHIFT)) << 16)
	ins     t0, t1, 16, 3
	mtc0    t0, $5, 2

	/* SegCtl1 */
	li      t0, ((MIPS_SEGCFG_UK << MIPS_SEGCFG_AM_SHIFT) |             \
		(1 << MIPS_SEGCFG_PA_SHIFT) | (2 << MIPS_SEGCFG_C_SHIFT) |  \
		(1 << MIPS_SEGCFG_EU_SHIFT)) |                              \
		(((MIPS_SEGCFG_UK << MIPS_SEGCFG_AM_SHIFT) |                \
		(4 << MIPS_SEGCFG_PA_SHIFT) |                               \
		(1 << MIPS_SEGCFG_EU_SHIFT)) << 16)
	ins     t0, t1, 16, 3
	mtc0    t0, $5, 3

	/* SegCtl2 */
	li      t0, ((MIPS_SEGCFG_MUSUK << MIPS_SEGCFG_AM_SHIFT) |          \
		(2 << MIPS_SEGCFG_PA_SHIFT) |                               \
		(1 << MIPS_SEGCFG_EU_SHIFT)) |                              \
		(((MIPS_SEGCFG_MUSUK << MIPS_SEGCFG_AM_SHIFT) |             \
		(0 << MIPS_SEGCFG_PA_SHIFT) |                               \
		(1 << MIPS_SEGCFG_EU_SHIFT)) << 16)
	or      t0, t2
	mtc0    t0, $5, 4

	jal     mips_ihb
	mfc0    t0, $16, 5
	li      t2, 0x40000000      /* K bit */
	or      t0, t0, t2
	mtc0    t0, $16, 5
	sync
	jal     mips_ihb
	.set    pop
	.endm

#elif defined(CONFIG_LTQ_EVA_1GB)
	.macro  platform_eva_init

	.set    push
	.set    reorder
	/*
	 * Get Config.K0 value and use it to program
	 * the segmentation registers
	 */
	mfc0    t1, CP0_CONFIG
	andi    t1, 0x7 /* CCA */
	move    t2, t1
	ins     t2, t1, 16, 3
	/* SegCtl0 */
	li      t0, ((MIPS_SEGCFG_UK << MIPS_SEGCFG_AM_SHIFT) |              \
		(5 << MIPS_SEGCFG_PA_SHIFT) | (2 << MIPS_SEGCFG_C_SHIFT) |   \
		(1 << MIPS_SEGCFG_EU_SHIFT)) |                               \
		(((MIPS_SEGCFG_MSK << MIPS_SEGCFG_AM_SHIFT) |                \
		(0 << MIPS_SEGCFG_PA_SHIFT) |                                \
		(1 << MIPS_SEGCFG_EU_SHIFT)) << 16)
	ins     t0, t1, 16, 3
	mtc0    t0, $5, 2

	/* SegCtl1 */
	li      t0, ((MIPS_SEGCFG_UK << MIPS_SEGCFG_AM_SHIFT) |              \
		(1 << MIPS_SEGCFG_PA_SHIFT) | (2 << MIPS_SEGCFG_C_SHIFT) |   \
		(1 << MIPS_SEGCFG_EU_SHIFT)) |                               \
		(((MIPS_SEGCFG_UK << MIPS_SEGCFG_AM_SHIFT) |                 \
		(2 << MIPS_SEGCFG_PA_SHIFT) |                                \
		(1 << MIPS_SEGCFG_EU_SHIFT)) << 16)
	ins     t0, t1, 16, 3
	mtc0    t0, $5, 3

	/* SegCtl2 */
	li      t0, ((MIPS_SEGCFG_MUSUK << MIPS_SEGCFG_AM_SHIFT) |           \
		(0 << MIPS_SEGCFG_PA_SHIFT) |                                \
		(1 << MIPS_SEGCFG_EU_SHIFT)) |                               \
		(((MIPS_SEGCFG_MUSK << MIPS_SEGCFG_AM_SHIFT) |               \
		(0 << MIPS_SEGCFG_PA_SHIFT) |                                \
		(1 << MIPS_SEGCFG_EU_SHIFT)) << 16)
	ins     t0, t1, 0, 3
	mtc0    t0, $5, 4

	jal     mips_ihb
	mfc0    t0, $16, 5
	li      t2, 0x40000000      /* K bit */
	or      t0, t0, t2
	mtc0    t0, $16, 5
	sync
	jal     mips_ihb

	.set    pop
	.endm
#elif defined(CONFIG_LTQ_EVA_LEGACY)
	.macro  platform_eva_init
	.set    push
	.set    reorder
	/*
	 * Get Config.K0 value and use it to program
	 * the segmentation registers
	 */
	mfc0    t1, CP0_CONFIG
	andi    t1, 0x7 /* CCA */
	move    t2, t1
	ins     t2, t1, 16, 3
	li      t0, ((MIPS_SEGCFG_UK << MIPS_SEGCFG_AM_SHIFT) |               \
		(5 << MIPS_SEGCFG_PA_SHIFT) | (2 << MIPS_SEGCFG_C_SHIFT) |    \
		(1 << MIPS_SEGCFG_EU_SHIFT)) |                                \
		(((MIPS_SEGCFG_MSK << MIPS_SEGCFG_AM_SHIFT) |                 \
		(0 << MIPS_SEGCFG_PA_SHIFT)/* | (5 << MIPS_SEGCFG_C_SHIFT)*/ |\
		(1 << MIPS_SEGCFG_EU_SHIFT)) << 16)
	ins     t0, t1, 16, 3
	mtc0    t0, $5, 2
	li      t0, ((MIPS_SEGCFG_UK << MIPS_SEGCFG_AM_SHIFT) |                \
		(1 << MIPS_SEGCFG_PA_SHIFT) | (2 << MIPS_SEGCFG_C_SHIFT) |     \
		(1 << MIPS_SEGCFG_EU_SHIFT)) |                                 \
		(((MIPS_SEGCFG_UK << MIPS_SEGCFG_AM_SHIFT) |                   \
		(1 << MIPS_SEGCFG_PA_SHIFT)/* | (5 << MIPS_SEGCFG_C_SHIFT) */ |\
		(1 << MIPS_SEGCFG_EU_SHIFT)) << 16)
	ins     t0, t1, 16, 3
	mtc0    t0, $5, 3
	li      t0, ((MIPS_SEGCFG_MUSK << MIPS_SEGCFG_AM_SHIFT) |              \
		(0 << MIPS_SEGCFG_PA_SHIFT)/* | (5 << MIPS_SEGCFG_C_SHIFT) */ |\
		(1 << MIPS_SEGCFG_EU_SHIFT)) |                                 \
		(((MIPS_SEGCFG_MUSK << MIPS_SEGCFG_AM_SHIFT) |                 \
		(0 << MIPS_SEGCFG_PA_SHIFT)/* | (5 << MIPS_SEGCFG_C_SHIFT) */ |\
		(1 << MIPS_SEGCFG_EU_SHIFT)) << 16)
	or      t0, t2
	mtc0    t0, $5, 4
	jal     mips_ihb

	mfc0    t0, $16, 5
	li      t2, 0x40000000      /* K bit */
	or      t0, t0, t2
	mtc0    t0, $16, 5
	sync
	jal	mips_ihb
	.set    pop
	.endm
#else
	/* Pure legacy + segment control for PCIE/MPE/Hwmemcpy and ToE */
	.macro  platform_eva_init
	.set    push
	.set    reorder
	/*
	 * Get Config.K0 value and use it to program
	 * the segmentation registers
	 */

	mfc0    t1, CP0_CONFIG
	andi    t1, 0x7 /* CCA */
	move    t2, t1
	ins     t2, t1, 16, 3
	li      t0, ((MIPS_SEGCFG_UK << MIPS_SEGCFG_AM_SHIFT) |               \
		(5 << MIPS_SEGCFG_PA_SHIFT) | (2 << MIPS_SEGCFG_C_SHIFT) |    \
		(1 << MIPS_SEGCFG_EU_SHIFT)) |                                \
		(((MIPS_SEGCFG_MK << MIPS_SEGCFG_AM_SHIFT) |                  \
		(0 << MIPS_SEGCFG_PA_SHIFT)/* | (5 << MIPS_SEGCFG_C_SHIFT)*/ |\
		(1 << MIPS_SEGCFG_EU_SHIFT)) << 16)
	ins     t0, t1, 16, 3
	mtc0    t0, $5, 2
	jal     mips_ihb
	.set    pop
	.endm
#endif /* Pure Legacy */

	.macro	kernel_entry_setup
	sync
	ehb
	platform_eva_init
	.endm
/*
 * Do SMP slave processor setup necessary before we can safely execute C code.
 */
	.macro	smp_slave_setup
	sync
	ehb
	platform_eva_init
	.endm

#endif /* __ASM_MACH_LANTIQ_KERNEL_ENTRY_INIT_H */
