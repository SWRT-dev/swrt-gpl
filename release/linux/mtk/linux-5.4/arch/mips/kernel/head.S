/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 1994, 1995 Waldorf Electronics
 * Written by Ralf Baechle and Andreas Busse
 * Copyright (C) 1994 - 99, 2003, 06 Ralf Baechle
 * Copyright (C) 1996 Paul M. Antoine
 * Modified for DECStation and hence R3000 support by Paul M. Antoine
 * Further modifications by David S. Miller and Harald Koerfgen
 * Copyright (C) 1999 Silicon Graphics, Inc.
 * Kevin Kissell, kevink@mips.com and Carsten Langgaard, carstenl@mips.com
 * Copyright (C) 2000 MIPS Technologies, Inc.  All rights reserved.
 */
#include <linux/init.h>
#include <linux/threads.h>

#include <asm/addrspace.h>
#include <asm/asm.h>
#include <asm/asmmacro.h>
#include <asm/irqflags.h>
#include <asm/regdef.h>
#include <asm/mipsregs.h>
#include <asm/cacheops.h>
#include <asm/stackframe.h>

#include <kernel-entry-init.h>

	/*
	 * For the moment disable interrupts, mark the kernel mode and
	 * set ST0_KX so that the CPU does not spit fire when using
	 * 64-bit addresses.  A full initialization of the CPU's status
	 * register is done later in per_cpu_trap_init().
	 */
	.macro	setup_c0_status set clr
	.set	push
	mfc0	t0, CP0_STATUS
	or	t0, ST0_CU0|\set|0x1f|\clr
	xor	t0, 0x1f|\clr
	mtc0	t0, CP0_STATUS
	.set	noreorder
	sll	zero,3				# ehb
	.set	pop
	.endm

	.macro	setup_c0_status_pri
#ifdef CONFIG_64BIT
	setup_c0_status ST0_KX 0
#else
	setup_c0_status 0 0
#endif
	.endm

	.macro	setup_c0_status_sec
#ifdef CONFIG_64BIT
	setup_c0_status ST0_KX ST0_BEV
#else
	setup_c0_status 0 ST0_BEV
#endif
	.endm

#ifndef CONFIG_NO_EXCEPT_FILL
	/*
	 * Reserved space for exception handlers.
	 * Necessary for machines which link their kernels at KSEG0.
	 */
	.fill	0x400
#endif

EXPORT(_stext)

#ifdef CONFIG_BOOT_RAW
	/*
	 * Give us a fighting chance of running if execution beings at the
	 * kernel load address.	 This is needed because this platform does
	 * not have a ELF loader yet.
	 */
FEXPORT(__kernel_entry)
	j	kernel_entry
#endif /* CONFIG_BOOT_RAW */

#ifdef CONFIG_IMAGE_CMDLINE_HACK
	.ascii	"CMDLINE:"
EXPORT(__image_cmdline)
	.fill	0x400
#endif /* CONFIG_IMAGE_CMDLINE_HACK */

	__REF

NESTED(kernel_entry, 16, sp)			# kernel entry point

	kernel_entry_setup			# cpu specific setup

	setup_c0_status_pri

#ifdef CONFIG_MIPS_ER35_WORKAROUNDS
	/* Jump to KSEG1 so that we can perform cache related operations */
	PTR_LA	t0, 0f
	li	t1, 5
	ins	t0, t1, 29, 3
	jr	t0
	nop
0:

	/* Calculate L2 Cache size */
	MFC0	t0, CP0_CONFIG, 2
	ext	t1, t0, 4, 4
	li	t2, 2
	sllv	t1, t2, t1		/* Cache line size */

	ext	t2, t0, 8, 4
	li	t3, 64
	sllv	t2, t3, t2		/* Sets per way */

	ext	t3, t0, 0, 4
	addiu	t3, 1			/* Number of ways */

	mul	t2, t2, t3		/* Number of sets */


	/* Flush L2 Cache before setting CCA overrides */
	move	t3, zero
1:
	cache	Index_Writeback_Inv_SD, 0(t3)
	sub	t2, 1
	add	t3, t3, t1
	bne	t2, zero, 1b
	nop

	sync

	/*
	 * Override bottom CONFIG_MIPS_ER35_RESERVED_SPACE of DDR to
	 * Uncached (which will be reserved as DMA zone)
	 */
	MFC0	t0, CP0_CMGCRBASE
	PTR_SLL	t0, t0, 4
	li	t1, 5
	ins	t0, t1, 29, 3

	/* GCR_REG2_MASK */
	lui	t1, (~((CONFIG_MIPS_ER35_RESERVED_SPACE - 1) >> 16)) & 0xffff
	ori	t1, t1, 0x0051
	sw	t1, 0xb8(t0)

	/* GCR_REG2_BASE */
	sw	zero, 0xb0(t0)

	/* Set default override to Write-through */
	lw	t1, 0x08(t0)
	li	t2, 0xffff8000
	and	t1, t1, t2
	ori	t1, 0x10
	sw	t1, 0x08(t0)
#endif

	/* We might not get launched at the address the kernel is linked to,
	   so we jump there.  */
	PTR_LA	t0, 0f
	jr	t0
0:

#ifdef CONFIG_USE_OF
#if defined(CONFIG_MIPS_RAW_APPENDED_DTB) || \
	defined(CONFIG_MIPS_ELF_APPENDED_DTB)

	PTR_LA		t2, __appended_dtb

#ifdef CONFIG_CPU_BIG_ENDIAN
	li		t1, 0xd00dfeed
#else  /* !CONFIG_CPU_BIG_ENDIAN */
	li		t1, 0xedfe0dd0
#endif /* !CONFIG_CPU_BIG_ENDIAN */
	lw		t0, (t2)
	beq		t0, t1, dtb_found
#endif /* CONFIG_MIPS_RAW_APPENDED_DTB || CONFIG_MIPS_ELF_APPENDED_DTB */
	li		t1, -2
	move		t2, a1
	beq		a0, t1, dtb_found

	li		t2, 0
dtb_found:
#endif /* CONFIG_USE_OF */
	PTR_LA		t0, __bss_start		# clear .bss
	LONG_S		zero, (t0)
	PTR_LA		t1, __bss_stop - LONGSIZE
1:
	PTR_ADDIU	t0, LONGSIZE
	LONG_S		zero, (t0)
	bne		t0, t1, 1b

	LONG_S		a0, fw_arg0		# firmware arguments
	LONG_S		a1, fw_arg1
	LONG_S		a2, fw_arg2
	LONG_S		a3, fw_arg3

#ifdef CONFIG_USE_OF
	LONG_S		t2, fw_passed_dtb
#endif

	MTC0		zero, CP0_CONTEXT	# clear context register
	PTR_LA		$28, init_thread_union
	/* Set the SP after an empty pt_regs.  */
	PTR_LI		sp, _THREAD_SIZE - 32 - PT_SIZE
	PTR_ADDU	sp, $28
	back_to_back_c0_hazard
	set_saved_sp	sp, t0, t1
	PTR_SUBU	sp, 4 * SZREG		# init stack pointer

#ifdef CONFIG_RELOCATABLE
	/* Copy kernel and apply the relocations */
	jal		relocate_kernel

	/* Repoint the sp into the new kernel image */
	PTR_LI		sp, _THREAD_SIZE - 32 - PT_SIZE
	PTR_ADDU	sp, $28
	set_saved_sp	sp, t0, t1
	PTR_SUBU	sp, 4 * SZREG		# init stack pointer

	/*
	 * relocate_kernel returns the entry point either
	 * in the relocated kernel or the original if for
	 * some reason relocation failed - jump there now
	 * with instruction hazard barrier because of the
	 * newly sync'd icache.
	 */
	jr.hb		v0
#else  /* !CONFIG_RELOCATABLE */
	j		start_kernel
#endif /* !CONFIG_RELOCATABLE */
	END(kernel_entry)

#ifdef CONFIG_SMP
/*
 * SMP slave cpus entry point.	Board specific code for bootstrap calls this
 * function after setting up the stack and gp registers.
 */
NESTED(smp_bootstrap, 16, sp)
	smp_slave_setup
	setup_c0_status_sec
	j	start_secondary
	END(smp_bootstrap)
#endif /* CONFIG_SMP */
