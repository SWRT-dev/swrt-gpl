/*
 *  This program is free software; you can distribute it and/or modify it
 *  under the terms of the GNU General Public License (Version 2) as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 *  for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  59 Temple Place - Suite 330, Boston MA 02111-1307, USA.
 *
 * Copyright (C) 2007 MIPS Technologies, Inc.
 *    Chris Dearman (chris@mips.com)
 */

#undef DEBUG

#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/smp.h>
#include <linux/cpumask.h>
#include <linux/interrupt.h>
#include <linux/compiler.h>
#include <linux/irqchip/mips-gic.h>

#include <linux/atomic.h>
#include <asm/cacheflush.h>
#include <asm/cpu.h>
#include <asm/processor.h>
#include <asm/hardirq.h>
#include <asm/mmu_context.h>
#include <asm/smp.h>
#include <asm/time.h>
#include <asm/mipsregs.h>
#include <asm/mipsmtregs.h>
#include <asm/mips_mt.h>
#include <asm/amon.h>
#ifdef CONFIG_LTQ_VMB
#include <asm/ltq_vmb.h>
#endif

#ifdef CONFIG_LTQ_ITC
#include <asm/ltq_itc.h>
#endif

static void cmp_init_secondary(void)
{
	struct cpuinfo_mips *c __maybe_unused = &current_cpu_data;

	/* Assume GIC is present */
	change_c0_status(ST0_IM, STATUSF_IP2 | STATUSF_IP3 | STATUSF_IP4 |
				 STATUSF_IP5 | STATUSF_IP6 | STATUSF_IP7);

	/* Enable per-cpu interrupts: platform specific */

#ifdef CONFIG_MIPS_MT_SMP
	if (cpu_has_mipsmt)
		c->vpe_id = (read_c0_tcbind() >> TCBIND_CURVPE_SHIFT) &
			TCBIND_CURVPE;
#endif

#ifdef CONFIG_LTQ_ITC
	itc_init();
#endif
}

static void cmp_smp_finish(void)
{
	pr_debug("SMPCMP: CPU%d: %s\n", smp_processor_id(), __func__);

	/* CDFIXME: remove this? */
	write_c0_compare(read_c0_count() + (8 * mips_hpt_frequency / HZ));

#ifdef CONFIG_MIPS_MT_FPAFF
	/* If we have an FPU, enroll ourselves in the FPU-full mask */
	if (cpu_has_fpu)
		cpumask_set_cpu(smp_processor_id(), &mt_fpu_cpumask);
#endif /* CONFIG_MIPS_MT_FPAFF */

	local_irq_enable();
}

void play_dead(void)
{
	unsigned int cpu;

	cpu = smp_processor_id();
	pr_info("CPU%d going offline\n", cpu);
}

/* Setup the PC, SP, and GP of a secondary processor and start it running
 * smp_bootstrap is the place to resume from
 * __KSTK_TOS(idle) is apparently the stack pointer
 * (unsigned long)idle->thread_info the gp
 */
static void cmp_boot_secondary(int cpu, struct task_struct *idle)
{
	struct thread_info *gp = task_thread_info(idle);
	unsigned long sp = __KSTK_TOS(idle);
	unsigned long pc = (unsigned long)&smp_bootstrap;
	unsigned long a0 = 0;
#ifdef CONFIG_LTQ_VMB
	int ret;
	struct CPU_launch_t cpu_launch;
#endif

	pr_debug("SMPCMP: CPU%d: %s cpu %d\n", smp_processor_id(),
		__func__, cpu);

#ifdef CONFIG_LTQ_VMB
	ret = vmb_cpu_alloc(cpu, "LINUX");
	if (ret == -VMB_EBUSY) {
		pr_err("VPE %d is Busy !!!\n", cpu);
		ret = vmb_cpu_alloc(MAX_CPU, "LINUX");
		pr_err("[%s]:[%d] CPU ret = %d\n",
		       __func__, __LINE__, ret);
		if (ret == -VMB_EBUSY) {
			pr_err("ALL the CPUs are Busy !\n");
			return;
		}
	}

	memset(&cpu_launch, 0, sizeof(struct CPU_launch_t));

#ifdef CONFIG_EVA
	cpu_launch.start_addr = CKSEG1ADDR(pc);
#else
	cpu_launch.start_addr = pc;
#endif
	cpu_launch.sp = sp;
	cpu_launch.gp = (unsigned long)gp;
	cpu_launch.a0 =  a0;

	ret = vmb_cpu_start(ret, cpu_launch, 0, 0, 0);
	if (ret == -VMB_ETIMEOUT || ret == -VMB_ENACK) {
		pr_err("[%s]:[%d] FW %s could not be launched on CPU %d.",
		       __func__, __LINE__, "LINUX", cpu);
		pr_err("The CPU has been force reset. Please use alloc and then start.\n");
		return;
	}
#else
	amon_cpu_start(cpu, pc, sp, (unsigned long)gp, a0);
#endif
}

static unsigned int core_vpe_count(unsigned int core)
{
		unsigned int cfg;

		if ((!IS_ENABLED(CONFIG_MIPS_MT_SMP) || !cpu_has_mipsmt) &&
		    (!IS_ENABLED(CONFIG_CPU_MIPSR6) || !cpu_has_vp))
			return 1;

		mips_cm_lock_other(core, 0);
		cfg = read_gcr_co_config() & CM_GCR_Cx_CONFIG_PVPE_MSK;
		mips_cm_unlock_other();
		return (cfg >> CM_GCR_Cx_CONFIG_PVPE_SHF) + 1;
}

/*Common setup before any secondaries are started
 */
void __init cmp_smp_setup(void)
{
	int i;
	int ncpu = 0;
	unsigned int ncores, nvpes, core_vpes;
	int c, v, v_min;

	pr_info("SMPCMP: CPU%d: %s\n", smp_processor_id(), __func__);

	/* Detect & record VPE topology */
	ncores = mips_cm_numcores();
	pr_info("%s topology ", cpu_has_mips_r6 ? "VP" : "VPE");

	for (c = nvpes = 0; c < ncores; c++) {
		core_vpes = core_vpe_count(c);
		pr_cont("%c%u", c ? ',' : '{', core_vpes);

	/* Use the number of VPEs in core 0 for smp_num_siblings */
		if (!c)
			smp_num_siblings = core_vpes;
		v_min = NR_CPUS - nvpes;
		v_min = min_t(int, core_vpes, v_min);
		for (v = 0; v < v_min; v++) {
			cpu_data[nvpes + v].core = c;
#if defined(CONFIG_MIPS_MT_SMP) || defined(CONFIG_CPU_MIPSR6)
			cpu_data[nvpes + v].vpe_id = v;
#endif
		}

		nvpes += core_vpes;
	}
	pr_cont("} total %u\n", nvpes);

#ifdef CONFIG_MIPS_MT_FPAFF
	/* If we have an FPU, enroll ourselves in the FPU-full mask */
	if (cpu_has_fpu)
		cpumask_set_cpu(0, &mt_fpu_cpumask);
#endif /* CONFIG_MIPS_MT_FPAFF */

	for (i = 1; i < NR_CPUS; i++) {
		if (amon_cpu_avail(i)) {
			set_cpu_possible(i, true);
			__cpu_number_map[i]	= ++ncpu;
			__cpu_logical_map[ncpu] = i;
		}
	}

	if (cpu_has_mipsmt) {
		unsigned int nvpe = 1;
#ifdef CONFIG_MIPS_MT_SMP
		unsigned int mvpconf0 = read_c0_mvpconf0();

		nvpe = ((mvpconf0 & MVPCONF0_PVPE) >> MVPCONF0_PVPE_SHIFT) + 1;
#endif
		smp_num_siblings = nvpe;
	}
	pr_info("Detected %i available secondary CPU(s)\n", ncpu);
}

void __init cmp_prepare_cpus(unsigned int max_cpus)
{
	unsigned int cca;
	bool cca_unsuitable;

	pr_debug("SMPCMP: CPU%d: %s max_cpus=%d\n",
		 smp_processor_id(), __func__, max_cpus);

	/* Detect whether the CCA is unsuited to multi-core SMP */
	cca = read_c0_config() & CONF_CM_CMASK;
	switch (cca) {
	case 0x4: /* CWBE */
	case 0x5: /* CWB */
				pr_info("CCA is coherent, multi-core is fine\n");
		/* The CCA is coherent, multi-core is fine */
		cca_unsuitable = false;
		break;

	default:
				pr_info("CCA is not coherent, multi-core is not usable\n");
		/* CCA is not coherent, multi-core is not usable */
		cca_unsuitable = true;
	}

#ifdef CONFIG_MIPS_MT
	/*
	 * FIXME: some of these options are per-system, some per-core and
	 * some per-cpu
	 */
	mips_mt_set_cpuoptions();
#endif

}

struct plat_smp_ops cmp_smp_ops = {
	.send_ipi_single	= mips_smp_send_ipi_single,
	.send_ipi_mask		= mips_smp_send_ipi_mask,
	.init_secondary		= cmp_init_secondary,
	.smp_finish		= cmp_smp_finish,
	.boot_secondary		= cmp_boot_secondary,
	.smp_setup		= cmp_smp_setup,
	.prepare_cpus		= cmp_prepare_cpus,
};
