/*
 * (C) Copyright 2012 Stephen Warren
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

#include <asm/arch/spl.h>
#include <asm/arch/leopard.h>


.globl lowlevel_init
lowlevel_init:

#ifndef CONFIG_SPL_BUILD
	/* do nothing for u-boot */
	mov pc, lr
#else
	/*
	 * In secure mode for each core,
	 * GIC CPU-interface initialization(Must here).
	 */
	@ldr	r5, =GIC_CPU_BASE
	@mov	r6, #0x07 		   @ cpu interface enable
	@str	r6, [r5, #GIC_CPU_CTRL]
	@mov	r6, #0xF0		   @ interrupt Priority Mask
	@str	r6, [r5, #GIC_CPU_PRIMASK]

	/*
	 * Arch-timer :
	 * set CNTFRQ = 20Mhz = 0x1312D00,
	 * set CNTVOFF to zero.
	 */
	movw r0, #0x2D00
	movt r0, #0x131
	mcr p15, 0, r0, c14, c0, 0

	/*
	 * Enable SMP bit
	 */
	mrc	p15, 0, r0, c1, c0, 1
	orr	r0, r0, #0x40
	mcr	p15, 0, r0, c1, c0, 1
	/*
	 * If MPCore, handle secondary cores
	 */
	mrc	p15, 0, r0, c0, c0, 5
	ands	r1, r0, #0x40000000
	bne	letsgo           @ Go if Uni-Processor
	ands	r0, r0, #0x0f
	beq	letsgo           @ Go if core 0 on primary core tile
	b	secondary_core   @ Will not return

	/*
	 * Master CPU
	 */
letsgo:
	mov	pc, lr           @ back to my caller

	/*
	 * Slave CPUs
	 */
secondary_core:
	/*
	 * Read core number into r0 firstly
	 */
	mrc	p15, 0, r0, c0, c0, 5
	and	r0, r0, #0x0f

	/*
	 * Set WFI for slave cores
	 */
loop:
	dsb
	isb
	wfi
	b	loop
#endif

