/*
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 *
 *  Copyright (C) 2009~2015 Lantiq Deutschland GmbH
 *  Copyright (C) 2016 Intel Corporation.
 */
#include <linux/init.h>
#include <linux/string.h>
#include <linux/kernel.h>
#include <linux/cpu.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <asm/cacheflush.h>
#include <asm/smp-ops.h>
#include <asm/traps.h>
#include <asm/fw/fw.h>
#include <linux/of.h>
#include <linux/of_platform.h>
#include <linux/of_irq.h>
#include <linux/of_fdt.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/sched.h>
#include <linux/bootmem.h>
#include <asm/ltq_vmb.h>
#include <linux/irqchip/mips-gic.h>

#undef LINUX_SHM_DDR
#undef LTQ_VMB_DEBUG

/* Number of cores*/
struct VMB_core_t core_t[MAX_CORE];
static DEFINE_SPINLOCK(vmb_lock);
static int g_fw_vmb_irq[MAX_CPU];
static int g_fw_vmb_hwirq[MAX_CPU];
static int g_cpu_vmb_irq[MAX_CPU];
/* ITC SemIDs, Commom to all cores */
struct VMB_itc_t itc_t[MAX_ITC_SEMID];
static struct proc_dir_entry *vmb_proc, *vmb_proc_dir;

void vmb_itc_sem_free_pcpu(int8_t cpu)
{
	int i;

	for (i = 0; i < MAX_ITC_SEMID; i++) {
		if (itc_t[i].cpu_id == cpu)
			vmb_itc_sem_free(i);
	}
}

void vmb_itc_sem_free(int8_t semID)
{
	itc_t[semID].itc_status &= ~ITC_ACTIVE;
	itc_t[semID].itc_status |= ITC_INACTIVE;
	itc_t[semID].cpu_id = -1;
	itc_t[semID].tc_id = -1;
}
EXPORT_SYMBOL(vmb_itc_sem_free);

/* API to get the memory DDR for strutures VMB_fw_msg_t/struct FW_vmb_msg_t */
void *VMB_get_msg_addr(int cpu, int direction)
{
	void *msg_t =  (void *)(CPU_LAUNCH);

	/* VMB --> FW : VMB_fw_msg_t structure */
	if (direction == 0)
		msg_t = msg_t + (vmb_msg_size * cpu) +
			sizeof(struct FW_vmb_msg_t);
	else
		msg_t = msg_t + (vmb_msg_size * cpu);

	return msg_t;
}
EXPORT_SYMBOL(VMB_get_msg_addr);

static irqreturn_t fw_vmb_ipi1_hdlr(int irq, void *ptr)
{
	int cpu, cid = 0, vid = 0;
	struct VMB_vpe_t *vpet_t;
	struct FW_vmb_msg_t *fw_t;

	/* This handler is from FW_VMB_IPI1 so called from CPU1 */
	cpu = 1;
	cid = which_core(cpu);
	vid = vpe_in_core(cpu);

	vpet_t = &core_t[cid].vpe_t[vid];
	fw_t = (struct FW_vmb_msg_t *)VMB_get_msg_addr(cpu, 1);

	/*
	 * May lead to deadlock in case of FW_RESET as spinlock for
	 * that VPE is taken and then wait_event_called thus
	 *  commenting the locks if this doesnt work then need
	 * to use IBL_IN_ handler to wakeup this MPE handler
	 */

	memcpy(&vpet_t->fw_vmb, fw_t, sizeof(struct FW_vmb_msg_t));
	/* W.r.t FW copy the DDR section to Internal DB */
	smp_rmb();

	if (vpet_t->fw_vmb.status == FW_RESET) {
		vmb_cpu_free(cpu);
		vmb_tc_free(cpu, -1);

		if (vpet_t->vmb_callback_fn && vpet_t->vmb_callback_fn != NULL)
			vpet_t->vmb_callback_fn(vpet_t->fw_vmb.status);

/* Be cautious:TEST Throughly, VPE will go for a reset in deactivated STATE*/
		vpet_t->bl_status = IBL_INACTIVE;

		goto res1_ext;

	} else if (vpet_t->fw_vmb.status == IBL_IN_WAIT) {
		vmb_cpu_free(cpu);
		vmb_tc_free(cpu, -1);

		vpet_t->bl_status = IBL_ACTIVE;
	}

	/* Set the wakeup_vpe and wakeup the waitqueue */
	vpet_t->v_wq.wakeup_vpe = 1;
	wake_up_interruptible(&vpet_t->v_wq.vmb_wq);

res1_ext:
	return IRQ_HANDLED;
}

static irqreturn_t fw_vmb_ipi2_hdlr(int irq, void *ptr)
{
	int cpu, cid = 0, vid = 0;
	struct VMB_vpe_t *vpet_t;
	struct FW_vmb_msg_t *fw_t;

	/* This handler is from FW_VMB_IPI2 so called from CPU2 */
	cpu = 2;
	cid = which_core(cpu);
	vid = vpe_in_core(cpu);

	vpet_t = &core_t[cid].vpe_t[vid];
	fw_t = (struct FW_vmb_msg_t *)VMB_get_msg_addr(cpu, 1);

	/*
	 * May lead to deadlock in case of FW_RESET as spinlock
	 * for that VPE is taken and then wait_event_called thus
	 * commenting the locks if this doesnt work then need to use
	 * IBL_IN_ handler to wakeup this MPE handler
	 */

	memcpy(&vpet_t->fw_vmb, fw_t, sizeof(struct FW_vmb_msg_t));
	/* W.r.t FW copy the DDR section to Internal DB */
	smp_rmb();

	if (vpet_t->fw_vmb.status == FW_RESET) {
		vmb_cpu_free(cpu);
		vmb_tc_free(cpu, -1);

		if (vpet_t->vmb_callback_fn && vpet_t->vmb_callback_fn != NULL)
			vpet_t->vmb_callback_fn(vpet_t->fw_vmb.status);

/* Be cautious:TEST Throughly, VPE will go a reset and in deactivated STATE*/
		vpet_t->bl_status = IBL_INACTIVE;

		goto res2_ext;

	} else if (vpet_t->fw_vmb.status == IBL_IN_WAIT) {
		vmb_cpu_free(cpu);
		vmb_tc_free(cpu, -1);
		vpet_t->bl_status = IBL_ACTIVE;
	}

	/* Set the wakeup_vpe and wakeup the waitqueue */
	vpet_t->v_wq.wakeup_vpe = 1;
	wake_up_interruptible(&vpet_t->v_wq.vmb_wq);

res2_ext:
	return IRQ_HANDLED;
}

static irqreturn_t fw_vmb_ipi3_hdlr(int irq, void *ptr)
{
	int cpu, cid = 0, vid = 0;
	struct VMB_vpe_t *vpet_t;
	struct FW_vmb_msg_t *fw_t;

	/* This handler is from FW_VMB_IPI3 so called from CPU3 */
	cpu = 3;
	cid = which_core(cpu);
	vid = vpe_in_core(cpu);

	vpet_t = &core_t[cid].vpe_t[vid];
	fw_t = (struct FW_vmb_msg_t *)VMB_get_msg_addr(cpu, 1);

	/*
	 * May lead to deadlock in case of FW_RESET as spinlock
	 * for that VPE is taken and then wait_event_called thus
	 * commenting the locks if this doesnt work then need to use
	 * BL_IN_ handler to wakeup this MPE handler
	 */

	memcpy(&vpet_t->fw_vmb, fw_t, sizeof(struct FW_vmb_msg_t));
	/* W.r.t FW copy the DDR section to Internal DB */
	smp_rmb();

	if (vpet_t->fw_vmb.status == FW_RESET) {
		vmb_cpu_free(cpu);
		vmb_tc_free(cpu, -1);

		if (vpet_t->vmb_callback_fn && vpet_t->vmb_callback_fn != NULL)
			vpet_t->vmb_callback_fn(vpet_t->fw_vmb.status);

/* Be cautious:TEST Throughly, VPE will go for a reset and deactivated STATE*/
		vpet_t->bl_status = IBL_INACTIVE;

		goto res3_ext;

	} else if (vpet_t->fw_vmb.status == IBL_IN_WAIT) {
		vmb_cpu_free(cpu);
		vmb_tc_free(cpu, -1);

		vpet_t->bl_status = IBL_ACTIVE;
	}

	/* Set the wakeup_vpe and wakeup the waitqueue */
	vpet_t->v_wq.wakeup_vpe = 1;
	wake_up_interruptible(&vpet_t->v_wq.vmb_wq);

res3_ext:
	return IRQ_HANDLED;
}

/*
 * Update a dummy vmb_msg_t->status in DB as this is for Linux
 *  SMP which doesn't update DDR in Linux --> VMB direction
 */
static int linux_cpu_ipi_update(unsigned long cpu)
{
	unsigned long flags;
	int c_id = which_core(cpu);
	int v_id = vpe_in_core(cpu);
	struct VMB_vpe_t *vpet_t = &core_t[c_id].vpe_t[v_id];

#ifdef LINUX_SHM_DDR
	struct FW_vmb_msg_t *fw_t = (struct FW_vmb_msg_t *)VMB_get_msg_addr(cpu, 1);

	spin_lock_irqsave(&vpet_t->vpe_lock, flags);

	memcpy(&vpet_t->fw_vmb, fw_t, sizeof(struct FW_vmb_msg_t));
	/* W.r.t FW copy the DDR section to Internal DB */
	smp_rmb();

	/* Clear DDR section */
	memset(fw_t, 0, sizeof(struct FW_vmb_msg_t));

	/* Set the wakeup_vpe and wakeup the waitqueue */
	vpet_t->v_wq.wakeup_vpe = 1;
	wake_up_interruptible(&vpet_t->v_wq.vmb_wq);

	/* unlock */
	spin_unlock_irqrestore(&vpet_t->vpe_lock, flags);
#else
	spin_lock_irqsave(&vpet_t->vpe_lock, flags);

	vpet_t->fw_vmb.status = (uint32_t)FW_VMB_ACK;
	vpet_t->fw_vmb.priv_info = 0;

	/* Set the wakeup_vpe and wakeup the waitqueue */
	vpet_t->v_wq.wakeup_vpe = 1;
	wake_up_interruptible(&vpet_t->v_wq.vmb_wq);

	/* unlock */
	spin_unlock_irqrestore(&vpet_t->vpe_lock, flags);
#endif
	return 0;
}

static int __init vmb_cpu_active(unsigned int hcpu)
{
	if (hcpu != 0)
		linux_cpu_ipi_update((unsigned long)hcpu);

	return 0;
}

/* Lock --> Access and Copy DDR --> check for status and update Internal DB */
static void vmb_check_IBL_fw_msg(void)
{
	int i, j;
	struct VMB_core_t *ct = core_t;
	struct FW_vmb_msg_t *fw_msg_t;

	for (i = 0; i < MAX_CORE; i++) {
		struct VMB_vpe_t *vpet = ct[i].vpe_t;
		int cpu;

		for (j = 0; j < MAX_VPE; j++) {
			if ((i == 0) && (j == 0))
				continue;

			spin_lock(&vpet[j].vpe_lock);
			cpu = get_cpu_id(i, j);
			fw_msg_t = (struct FW_vmb_msg_t *)VMB_get_msg_addr(cpu, 1);
			memcpy(&vpet[j].fw_vmb, fw_msg_t,
			       sizeof(struct FW_vmb_msg_t));
			if (vpet[j].fw_vmb.status == IBL_IN_WAIT)
				vpet[j].bl_status = IBL_ACTIVE;
			else
				vpet[j].bl_status = IBL_INACTIVE;
			spin_unlock(&vpet[j].vpe_lock);
		}
	}
}

static int update_DB_from_DT(struct VMB_vpe_t *vt)
{
	struct device_node *np;
	char str1[16], *name;
	int ret;
	struct resource irqres;

	memset(str1, '\0', sizeof(str1));
	sprintf(str1, "%s%d", "/cpus/cpu@", vt->cpu_id);

	np = of_find_node_by_path(str1);
	if (!np)
		return -ENODEV;

	ret = of_property_read_string_index(np,
				"default-OS", 0, (const char **)&name);
	if (ret < 0 && ret != -EINVAL) {
		pr_info("ERROR : Property could be read from DT\n");
		return ret;
	}

	strlcpy(vt->name, name, sizeof(vt->name));
	vt->cpu_status |= CPU_BOOTUP_DT;

	/* for CPU 0, no need to get IRQ for VMB */
	if (vt->cpu_id == 0)
		return 0;

	ret = of_irq_to_resource_table(np, &irqres, 1);
	if (ret != 1) {
		pr_info("failed to get irq for vmb since ret = %d\n", ret);
		return -ENODEV;
	}
	/* get the interrupt numbers from DT */
	g_fw_vmb_irq[vt->cpu_id] = irqres.start;

	if (of_property_read_u32_index(np, "interrupts", 1,
				       &g_fw_vmb_hwirq[vt->cpu_id]) != 0) {
		pr_info("failed to get hwirq for vmb\n");
		return -ENODEV;
	}

	ret = of_property_read_u32(np, "vmb-fw-ipi",
				   &g_cpu_vmb_irq[vt->cpu_id]);
	if (ret < 0 && ret != -EINVAL) {
		pr_info("ERROR : Property could be read from DT\n");
		return ret;
	}

	return 0;
}

static void initialise_vmb_DB(void)
{
	struct VMB_core_t *coret = core_t;
	int i, j;

	coret[0].active |= CORE_ACTIVE;

	for (i = 0; i < MAX_CORE; i++) {
		struct VMB_tc_t *tct = coret[i].tc_t;
		struct VMB_vpe_t *vpet = coret[i].vpe_t;
		struct VMB_yr_t *yrit = coret[i].yr_t;

		for (j = 0; j < MAX_VPE; j++) {
			vpet[j].vpemt_grp = 0;
			vpet[j].core_id = (((i * 2) + j)/2);
			spin_lock_init(&vpet[j].vpe_lock);
			init_waitqueue_head(&vpet[j].v_wq.vmb_wq);
			vpet[j].v_wq.wakeup_vpe = 0;
			vpet[j].cpu_id = ((i * 2) + j);
			if ((i == 0) && (j == 0)) {
				/* Core0/VPE0 always active*/
				vpet[j].bl_status = IBL_ACTIVE;
				vpet[j].cpu_status = CPU_ACTIVE;
			} else {
				vpet[j].bl_status = IBL_INACTIVE;
				vpet[j].cpu_status = CPU_INACTIVE;
			}
			tct[j].vpe_id = j;
/* Get the information from DT for FW names also update the bootflag for cpu */
			update_DB_from_DT(&vpet[j]);

			if (is_linux_OS_vmb(vpet[j].cpu_id))
				set_cpu_present(vpet[j].cpu_id, true);
		}

		for (j = 0; j < MAX_TC; j++) {
			if (j >= 2) {
				tct[j].vpe_id = MAX_VPE;
				tct[j].tc_status = TC_INACTIVE;
			} else {
				tct[j].vpe_id = j;
				tct[j].tc_status = TC_ACTIVE;
			}
			tct[j].tcmt_grp = 0;
		}

		for (j = 0; j < MAX_YIELD_INTF; j++) {
			yrit[j].yr_status = YR_INACTIVE;
			yrit[j].cpu_id = -1;
		}

		for (j = 0; j < MAX_ITC_SEMID; j++) {
			itc_t[j].itc_status = ITC_INACTIVE;
			itc_t[j].cpu_id = -1;
			itc_t[j].tc_id = -1;
		}
	}
}

static void initialise_vmb_IRQhdlr(void)
{
	int err = 0;

	if (g_fw_vmb_irq[1]) {
		err = request_irq(g_fw_vmb_irq[1], fw_vmb_ipi1_hdlr,
				  0, "fw_vmb_ipi1", NULL);
		if (err)
			pr_info("request_irq for IRQ = %d failed err = %d\n",
				g_fw_vmb_irq[1], err);
	} else {
		pr_err("no IRQ was specified for CPU1 to VMB\n");
	}

	if (g_fw_vmb_irq[2]) {
		err = request_irq(g_fw_vmb_irq[2], fw_vmb_ipi2_hdlr,
				  0, "fw_vmb_ipi2", NULL);
		if (err)
			pr_info("request_irq for IRQ = %d failed err = %d\n",
				g_fw_vmb_irq[2], err);
	} else {
		pr_err("no IRQ was specified for CPU2 to VMB\n");
	}

	if (g_fw_vmb_irq[3]) {
		err = request_irq(g_fw_vmb_irq[3], fw_vmb_ipi3_hdlr,
				  0, "fw_vmb_ipi3", NULL);
		if (err)
			pr_info("request_irq for IRQ = %d failed err = %d\n",
				g_fw_vmb_irq[3], err);
	} else {
		pr_err("no IRQ was specified for CPU3 to VMB\n");
	}
}

/* Dump the tree */
static int dump_vmb_tree(struct seq_file *s, void *v)
{
	struct VMB_core_t *coret = core_t;
	int i, j, k, cpu_id;

	for (i = 0; i < MAX_CORE; i++) {
		struct VMB_tc_t *tct = coret[i].tc_t;
		struct VMB_vpe_t *vpet = coret[i].vpe_t;
		struct VMB_yr_t *yr = coret[i].yr_t;

		for (j = 0; j < MAX_VPE; j++) {
			spin_lock(&vpet[j].vpe_lock);
			cpu_id = get_cpu_id(i, j);
			seq_puts(s, "CORE ID ");
			seq_printf(s, "%d\n", i);
			seq_puts(s, "\t - VPE ID ");
			seq_printf(s, "%d\n", j);
			seq_puts(s, "\t \t - OS Name = ");
			seq_printf(s, "%s\n", vpet[j].name);
			seq_puts(s, "\t \t - InterAptiv-BL status = ");
			seq_printf(s, "%s\n", (vpet[j].bl_status == IBL_ACTIVE ?
					       "ACTIVE" : "INACTIVE"));
			seq_puts(s, "\t \t - CPU status = ");
			seq_printf(s, "%s\n",
				   ((vpet[j].cpu_status & CPU_ACTIVE) == CPU_ACTIVE ?
				   "ACTIVE" : "INACTIVE"));
			seq_puts(s, "\t \t - CPU ID = ");
			seq_printf(s, "%d\n", vpet[j].cpu_id);
			seq_puts(s, "\t \t - Allocated TC(s) =  ");
			for (k = 0; k < MAX_TC; k++) {
				if (tct[k].tc_status == TC_ACTIVE &&
				    cpu_id == get_cpu_id(i, tct[k].vpe_id)) {
					seq_puts(s, "TC");
					seq_printf(s, "%d", k);
					seq_puts(s, ", ");
				}
			}
			seq_puts(s, "\t \t\n");
			seq_puts(s, "\t \t - Allocated Yield Resource(s)=  ");
			for (k = 0; k < MAX_YIELD_INTF; k++) {
				if (yr[k].yr_status == YR_ACTIVE &&
				    cpu_id == yr[k].cpu_id) {
					seq_printf(s, "%d", k);
					seq_puts(s, ", ");
				}
			}
			seq_puts(s, "\t \t\n");
			seq_puts(s, "\t \t - Allocated ITC Resource(s) =  ");
			for (k = 0; k < MAX_ITC_SEMID; k++) {
				if (itc_t[k].itc_status == ITC_ACTIVE &&
				    cpu_id == itc_t[k].cpu_id) {
					seq_printf(s, "%d", k);
					seq_puts(s, ", ");
				}
			}
			seq_puts(s, "\t \t\n");
			seq_puts(s, "\t \t - FW_VMB_MSG Structure\n");
			seq_puts(s, "\t \t \t - status = ");
			seq_printf(s, "0X%x\n", vpet[j].fw_vmb.status);
			seq_puts(s, "\t \t \t - priv_info = ");
			seq_printf(s, "0X%x\n", vpet[j].fw_vmb.priv_info);

			spin_unlock(&vpet[j].vpe_lock);
		}
	}

	return 0;
}

static int vmb_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, dump_vmb_tree, NULL);
}

static const struct file_operations vmb_proc_fops = {
	.open	      = vmb_proc_open,
	.read	      = seq_read,
	.llseek	  = seq_lseek,
	.release		= single_release,
};

/* Initialise VMB structures and IRQ */
static int __init vmb_init(void)
{
	memset(core_t, 0, sizeof(struct VMB_core_t));

	/*
	 * CPU notifier for Linux SMP bootup indication notify (CPU_STARTING)
	 * from start_secondary
	 */
	cpuhp_setup_state(CPUHP_AP_INTEL_VMB_ONLINE,
			  "VMB_ON_LINE", vmb_cpu_active, NULL);

	/*
	 * Initialise a New core before vmbDB init as it will get time for
	 * dumping IBL_INWAIT in DDR which can be picked by vmb_check_IBL()
	 */

	/* Basic initialisation of the DB */
	initialise_vmb_DB();

	/* register IRQ handlers for VMB_FW_IRQx */
	initialise_vmb_IRQhdlr();

	/* Initialise the /proc/vm/debug to dump the Internal DB */
	vmb_proc_dir = proc_mkdir("vmb", NULL);
	vmb_proc = proc_create("status", 0644, vmb_proc_dir, &vmb_proc_fops);

	/*
	 * API to update th IBL status if the IBL_IN_WAIT IPI
	 * is missed during initialisation. Mainly a fail-safe Linux
	 * running VPE updates as there is no IPI mentioned
	 * to handle IBL_INWAIT for Linux
	 */
	vmb_check_IBL_fw_msg();

	spin_lock_init(&vmb_lock);

	return 0;
}

early_initcall(vmb_init);

/*
 * This function is needed to trigger GIC IPI Interrupt.
 * Needed as IPI IRQ number are not consecutive .
 *Currently we use 20,21 and 85 for CPU1,2 adnd 3 respectviely
 */

static void gic_trigger(int8_t cpu)
{
	switch (cpu) {
	case 1:
		gic_send_ipi_simple(g_cpu_vmb_irq[1], 0);
		break;
	case 2:
		gic_send_ipi_simple(g_cpu_vmb_irq[2], 0);
		break;
	case 3:
		gic_send_ipi_simple(g_cpu_vmb_irq[3], 0);
		break;
	default:
		break;
	}
}

/* Allocate Free CPU from the pool */
int8_t vmb_cpu_alloc(int8_t cpu, char *fw_name)
{
	int ret = -VMB_ERROR;
	struct VMB_core_t *coret = core_t;
	int i, j;
/*
 *CHECK : May be for alloc we need to have a global lock to
 * avoid a scenario where 2 FWs call at the same time race
 * condiand getting same cpu number
 */

	if (cpu != MAX_CPU) {
		int c_id = which_core(cpu);
		int v_id = vpe_in_core(cpu);
		struct VMB_vpe_t *vpet = &core_t[c_id].vpe_t[v_id];

		if ((vpet->cpu_status & CPU_BOOTUP_DT) == CPU_BOOTUP_DT) {
			int cmp_res = strncmp(vpet->name, fw_name,
					      sizeof(vpet->name));
			if (cmp_res != 0) {
				pr_info("Per DT Bootup %s not run on CPU %d\n",
					fw_name, cpu);
				pr_info("Please retry with MAX_CPUS !!!\n");
				ret = -VMB_EAVAIL;
				goto fin_alloc;
			}
		}

		if ((vpet->bl_status == IBL_ACTIVE) &&
		    ((vpet->cpu_status & CPU_INACTIVE) == CPU_INACTIVE)) {
			ret = cpu;
			vpet->cpu_status &= ~CPU_INACTIVE;
			vpet->cpu_status |= CPU_ACTIVE;
			pr_info("[%s]:[%d] CPU vpet.cpu_status = %x\n",
				__func__, __LINE__, vpet->cpu_status);
#ifdef CONFIG_LTQ_DYN_CPU_ALLOC
			if ((vpet->cpu_status & CPU_BOOTUP_DT) == CPU_BOOTUP_DT)
				vpet->cpu_status &= ~CPU_BOOTUP_DT;
			else
				strlcpy(vpet->name, fw_name,
					sizeof(vpet->name));
#endif
		} else {
			ret = -VMB_EBUSY;
		}
		return ret;
	}

	for (i = 0; i < MAX_CORE; i++) {
		struct VMB_vpe_t *vpet = coret[i].vpe_t;

		for (j = 0; j < MAX_VPE; j++) {
			if ((vpet[j].cpu_status & CPU_BOOTUP_DT) == CPU_BOOTUP_DT) {
				if (strncmp(vpet[j].name, fw_name, sizeof(vpet[j].name)) != 0) {
					ret = -VMB_EAVAIL;
					continue;
				}
			}

			if ((vpet[j].bl_status == IBL_ACTIVE) &&
			    ((vpet[j].cpu_status & CPU_INACTIVE) == CPU_INACTIVE)) {
				ret = get_cpu_id(i, j);
				vpet[j].cpu_status &= ~CPU_INACTIVE;
				vpet[j].cpu_status |= CPU_ACTIVE;
#ifdef CONFIG_LTQ_DYN_CPU_ALLOC
				if ((vpet[j].cpu_status & CPU_BOOTUP_DT) == CPU_BOOTUP_DT)
					vpet[j].cpu_status &= ~CPU_BOOTUP_DT;
				else
					strlcpy(vpet[j].name, fw_name,
						sizeof(vpet[j].name));
#endif
				goto fin_alloc;
			} else {
				ret = -VMB_EBUSY;
			}
		}
	}

fin_alloc:
	return ret;
}
EXPORT_SYMBOL(vmb_cpu_alloc);

void vmb_register_callback(uint8_t cpu, vmb_callback_func func)
{
	int c_id = which_core(cpu);
	int v_id = vpe_in_core(cpu);
	struct VMB_vpe_t *vpet = &core_t[c_id].vpe_t[v_id];

	vpet->vmb_callback_fn = func;
}
EXPORT_SYMBOL(vmb_register_callback);

/* Mark the VPE as free, Free all the allocated Yield resources to CPU/VPE */
int8_t vmb_cpu_free(int8_t cpu)
{
	int c_id = which_core(cpu);
	int v_id = vpe_in_core(cpu);
	struct VMB_vpe_t *vpet = &core_t[c_id].vpe_t[v_id];

	if (vpet->bl_status != IBL_ACTIVE)
		pr_info("WARNING : IBL is not active for CPU = %d !!!\n", cpu);

	vpet->cpu_status &= ~CPU_ACTIVE;
	vpet->cpu_status |= CPU_INACTIVE;

	/* Free Yield resorces allocated to this CPU */
	vmb_yr_free(cpu, -1);

	/* Free ITC SemIDs allocated to this CPU */
	vmb_itc_sem_free_pcpu(cpu);

	return VMB_SUCCESS;
}
EXPORT_SYMBOL(vmb_cpu_free);

int8_t vmb_cpu_force_stop(int8_t cpu)
{
	int c_id, v_id;
	struct VMB_vpe_t *vpet;
	int ret, ret1;

	c_id = which_core(cpu);
	v_id = vpe_in_core(cpu);
	vpet = &core_t[c_id].vpe_t[v_id];

	vpet->bl_status &= ~IBL_ACTIVE;
	vpet->bl_status |= IBL_INACTIVE;

	/* Generate a NMI for that VPE*/

	/* Wait for IBL_IN_WAIT or timeout */
	ret1 = wait_event_interruptible_timeout(vpet->v_wq.vmb_wq,
					(vpet->v_wq.wakeup_vpe == 1),
					QUEUE_TIMEOUT);
	if (ret1 <= 0) {
		pr_info("[%s]:[%d] wait_event timeout occured for CPU = %d\n",
			__func__, __LINE__, cpu);
		pr_info("Consider reseting the CPU using vmb_cpu_force_stop.\n");
		vpet->v_wq.wakeup_vpe = 0;

		ret = -VMB_ETIMEOUT;
		goto fin_fstop;
	}

	vpet->v_wq.wakeup_vpe = 0;

	if (vpet->fw_vmb.status == (uint32_t)IBL_IN_WAIT) {
		ret = VMB_SUCCESS;
		pr_err("[%s]:[%d] ret=%d\n", __func__, __LINE__, ret);
	} else {
		ret = -VMB_ENACK;
		pr_err("[%s]:[%d] -ENACK recieved from FW. Resetting\n",
		       __func__, __LINE__);
	}

fin_fstop:
	return ret;
}

int8_t vmb_cpu_stop(int8_t cpu)
{
	int c_id, v_id;
	struct VMB_vpe_t *vpet;
	int ret, ret1;
	struct VMB_fw_msg_t *vmb_fw_msg_t;

/*GLOBAL LOCK needed as "cpu" can for a simutaneous vmb_cpu_start from 2 FWs */

	c_id = which_core(cpu);
	v_id = vpe_in_core(cpu);
	vpet = &core_t[c_id].vpe_t[v_id];

/*GLOBAL UNLOCK needed as "cpu" can for a simutaneous vmb_cpu_start from 2 FWs*/

/*Per-VPE Lock and update the vmb_fw_msg_t struct and update DDR */

	spin_lock(&vpet->vpe_lock);

	if ((vpet->bl_status == IBL_ACTIVE) &&
	    ((vpet->cpu_status & CPU_ACTIVE) == CPU_ACTIVE)) {
		vmb_fw_msg_t = (struct VMB_fw_msg_t *)VMB_get_msg_addr(cpu, 0);
		memset(vmb_fw_msg_t, 0, sizeof(struct VMB_fw_msg_t));
		vmb_fw_msg_t->msg_id = VMB_CPU_STOP;
	} else {
		ret = -VMB_ERROR;
		spin_unlock(&vpet->vpe_lock);
		vmb_cpu_free(cpu);
		vmb_tc_free(cpu, -1);
		goto fin_stop;
	}

	spin_unlock(&vpet->vpe_lock);

	/* Generate an IPI */
	gic_trigger(cpu);
	mips_ihb();

	/* Wait for timeout */
	ret1 = wait_event_interruptible_timeout(vpet->v_wq.vmb_wq,
					(vpet->v_wq.wakeup_vpe == 1),
					QUEUE_TIMEOUT);
	if (ret1 <= 0) {
		pr_info("[%s]:[%d] wait_event timeout occured for CPU = %d\n",
			__func__, __LINE__, cpu);
		pr_info("Consider reseting the CPU using vmb_cpu_force_stop.\n");
		vpet->v_wq.wakeup_vpe = 0;
		memset(vmb_fw_msg_t, 0, sizeof(struct VMB_fw_msg_t));
		vmb_cpu_free(cpu);
		vmb_tc_free(cpu, -1);
		vmb_cpu_force_stop(cpu);
		ret = -VMB_ETIMEOUT;
		goto fin_stop;
	}

	pr_info("[%s]:[%d] OUTSIDE wakeup_vpe = %d vpet->fw_vmb.status = %d\n",
		__func__, __LINE__, vpet->v_wq.wakeup_vpe, vpet->fw_vmb.status);
	memset(vmb_fw_msg_t, 0, sizeof(struct VMB_fw_msg_t));
	vpet->v_wq.wakeup_vpe = 0;

	if ((vpet->fw_vmb.status == (uint32_t)FW_VMB_ACK) ||
	    (vpet->fw_vmb.status == (uint32_t)IBL_IN_WAIT)) {
		ret = VMB_SUCCESS;
		vmb_cpu_free(cpu);
		vmb_tc_free(cpu, -1);
		pr_err("[%s]:[%d]  ret = %d\n",
		       __func__, __LINE__, ret);
	} else {
		ret = -VMB_ENACK;
		pr_err("[%s]:[%d] -ENACK recieved from FW. Resetting CPU\n",
		       __func__, __LINE__);
		vmb_cpu_free(cpu);
		vmb_tc_free(cpu, -1);
		vmb_cpu_force_stop(cpu);
	}

fin_stop:
	return ret;
}
EXPORT_SYMBOL(vmb_cpu_stop);

int8_t vmb_cpu_start(int8_t cpu, struct CPU_launch_t cpu_launch,
		struct TC_launch_t tc_launch[], uint8_t num_tcs, uint8_t num_yr)
{
	int c_id, v_id;
	struct VMB_vpe_t *vpet;
	int ret, ret1, i;
	struct VMB_fw_msg_t *vmb_fw_msg_t;
	int32_t yieldres_t = 0;

/*
 * CHECK :  GLOBAL LOCK needed as "cpu" can for
 * a simutaneous vmb_cpu_start from 2 FWs
 */
	c_id = which_core(cpu);
	v_id = vpe_in_core(cpu);
	vpet = &core_t[c_id].vpe_t[v_id];

/*
 * CHECK : GLOBAL UNLOCK needed as "cpu" can for
 * a simutaneous vmb_cpu_start from 2 FWs
 */

/* After getting the cpu then we may use Per-VPE locks */

	if ((void *)&cpu_launch == NULL)
		return -VMB_ENOPRM;

	if (num_tcs > MAX_TCS) {
		pr_info("[%s]:[%d] num_tcs %d greater MAX_TCS (%d) reseting\n",
			__func__, __LINE__, num_tcs, MAX_TCS);
		num_tcs = MAX_TCS;
	}

/* Per-VPE Lock and update the vmb_fw_msg_t struct and update DDR */
	spin_lock(&vpet->vpe_lock);

/* Request num_yr should be a non-zero value */
	if (num_yr > 0) {
		yieldres_t = vmb_yr_get(cpu, num_yr);
		if (yieldres_t == -VMB_EBUSY) {
			spin_unlock(&vpet->vpe_lock);
			pr_err("[%s]:[%d] failed as %d Yield not free.\n",
			       __func__, __LINE__, num_yr);
			ret = -VMB_EBUSY;
			goto fin;
		}
	}

	vmb_fw_msg_t = (struct VMB_fw_msg_t *)VMB_get_msg_addr(cpu, 0);

	memset(vmb_fw_msg_t, 0, sizeof(struct VMB_fw_msg_t));

	vmb_fw_msg_t->msg_id = VMB_CPU_START;
	memcpy(&vmb_fw_msg_t->cpu_launch, &cpu_launch,
	       sizeof(vmb_fw_msg_t->cpu_launch));
	if ((void *)tc_launch != NULL) {
		for (i = 0; i < (num_tcs); i++) {
			memcpy(&vmb_fw_msg_t->tc_launch[i], &tc_launch[i],
			       sizeof(vmb_fw_msg_t->tc_launch[i]));
			vmb_fw_msg_t->tc_launch[i].mt_group = vpet->vpemt_grp;
		}
	}

	/* update the allocated yield resource bitmap */
	vmb_fw_msg_t->cpu_launch.yield_res = (uint32_t)yieldres_t;

	/* update tc_num to indicate number of TCs only in cpu and tc start */
	vmb_fw_msg_t->tc_num = num_tcs;

	/* Set the vpe mt_grp */
	vmb_fw_msg_t->cpu_launch.mt_group = vpet->vpemt_grp;
	spin_unlock(&vpet->vpe_lock);
	gic_trigger(cpu);
	mips_ihb();

	/* Wait for timeout */
	ret1 = wait_event_interruptible_timeout(vpet->v_wq.vmb_wq,
					(vpet->v_wq.wakeup_vpe == 1),
					QUEUE_TIMEOUT);
	if (ret1 <= 0) {
		pr_info("[%s]:[%d] wait_event timeout for CPU = %d FW = %s\n",
			__func__, __LINE__, cpu, vpet->name);
		pr_info("CPU not responding so going for a force cpu stop.\n");
		memset(vmb_fw_msg_t, 0, sizeof(struct VMB_fw_msg_t));
		vpet->v_wq.wakeup_vpe = 0;
		vmb_cpu_free(cpu);
		vmb_tc_free(cpu, -1);
		vmb_cpu_force_stop(cpu);
		ret = -VMB_ETIMEOUT;
		goto fin;
	}
	memset(vmb_fw_msg_t, 0, sizeof(struct VMB_fw_msg_t));
	vpet->v_wq.wakeup_vpe = 0;

	if (vpet->fw_vmb.status == (uint32_t)FW_VMB_ACK) {
		ret = VMB_SUCCESS;
	} else {
		ret = -VMB_ENACK;
		pr_err("[%s]:[%d] -ENACK recieved from FW ..\n",
		       __func__, __LINE__);
		pr_err("Consider reseting the CPU\n");
		vmb_cpu_free(cpu);
		vmb_tc_free(cpu, -1);
		vmb_cpu_force_stop(cpu);
	}

fin:
	return ret;
}
EXPORT_SYMBOL(vmb_cpu_start);

/********************************** VMB TC APIs **************************/

int8_t vmb_tc_alloc(uint8_t cpu)
{
	int ret = -VMB_ERROR;
	int i, c_id, v_id;
	struct VMB_core_t *coret = core_t;
	struct VMB_vpe_t *vpet;

	c_id = which_core(cpu);
	v_id = vpe_in_core(cpu);
	vpet = &core_t[c_id].vpe_t[v_id];

	if ((vpet->bl_status == IBL_ACTIVE) &&
	    ((vpet->cpu_status & CPU_ACTIVE) == CPU_ACTIVE)) {
		struct VMB_tc_t *tct = coret[c_id].tc_t;

		for (i = 2; i < MAX_TC; i++) {
			if ((tct[i].tc_status & TC_INACTIVE) == TC_INACTIVE) {
				tct[i].tc_status &= ~TC_INACTIVE;
				tct[i].tc_status |= TC_ACTIVE;
				ret = i;
				tct[i].vpe_id = v_id;
				goto fin_talloc;
			} else {
				ret = -VMB_EBUSY;
			}
		}
	}
fin_talloc:
		return ret;
}
EXPORT_SYMBOL(vmb_tc_alloc);

int8_t vmb_tc_free(int8_t cpu, int8_t tc_num)
{
	int ret = -VMB_ERROR;
	int i, c_id, v_id;
	struct VMB_core_t *coret = core_t;
	struct VMB_vpe_t *vpet;

	c_id = which_core(cpu);
	v_id = vpe_in_core(cpu);
	vpet = &core_t[c_id].vpe_t[v_id];

	if ((vpet->bl_status == IBL_ACTIVE) &&
	    ((vpet->cpu_status & CPU_ACTIVE) == CPU_ACTIVE))
		pr_info("VPE %d is not active !still freeing the TC %d\n",
			cpu, tc_num);

	if (tc_num == -1) {
		struct VMB_tc_t *tct = coret[c_id].tc_t;

		for (i = 2; i < MAX_TC; i++) {
			ret = get_cpu_id(c_id, tct[i].vpe_id);
			if (ret == cpu) {
				tct[i].tc_status &= ~TC_ACTIVE;
				tct[i].tc_status |= TC_INACTIVE;
			}
		}
		ret = VMB_SUCCESS;
	} else {
		struct VMB_tc_t *tct = &coret[c_id].tc_t[tc_num];

		ret = get_cpu_id(c_id, tct->vpe_id);

		if (ret == cpu) {
			tct->tc_status &= ~TC_ACTIVE;
			tct->tc_status |= TC_INACTIVE;
			ret = VMB_SUCCESS;
		} else {
			pr_info(" TC %d is not attached to CPU %d.\n",
				tc_num, cpu);
			ret = -VMB_EAVAIL;
		}
	}
	return ret;
}
EXPORT_SYMBOL(vmb_tc_free);

int8_t vmb_get_vpeid(uint8_t cpu, uint8_t tc_num)
{
	int ret = -VMB_ERROR;
	int c_id;
	struct VMB_core_t *coret = core_t;
	struct VMB_tc_t *tct;

	c_id = which_core(cpu);
	tct = &coret[c_id].tc_t[tc_num];
	ret = get_cpu_id(c_id, tct->vpe_id);

	return ret;
}
EXPORT_SYMBOL(vmb_get_vpeid);

int8_t vmb_tc_start(uint8_t cpu, struct TC_launch_t tc_launch[],
		    uint8_t num_tcs)
{
	int c_id, v_id;
	struct VMB_vpe_t *vpet;
	int ret, ret1, i;
	struct VMB_fw_msg_t *vmb_fw_msg_t;

	c_id = which_core(cpu);
	v_id = vpe_in_core(cpu);
	vpet = &core_t[c_id].vpe_t[v_id];

/* After getting the cpu then we may use Per-VPE locks */

	if ((void *)tc_launch == NULL)
		return -VMB_ENOPRM;

	if (num_tcs > MAX_TCS) {
		pr_info("num_tcs %d greater than MAX_TCS %d reseting num_tcs\n",
			num_tcs, MAX_TCS);
		num_tcs = MAX_TCS;
	}

/* Per-VPE Lock and update the vmb_fw_msg_t struct and update DDR */

	spin_lock(&vpet->vpe_lock);

	vmb_fw_msg_t = (struct VMB_fw_msg_t *)VMB_get_msg_addr(cpu, 0);
	memset(vmb_fw_msg_t, 0, sizeof(struct VMB_fw_msg_t));

	vmb_fw_msg_t->msg_id = VMB_TC_START;

	for (i = 0; i < (num_tcs); i++)
		memcpy(&vmb_fw_msg_t->tc_launch[i], &tc_launch[i],
		       sizeof(vmb_fw_msg_t->tc_launch[i]));

	/* update tc_num to indicate number of TCs only in cpu and tc start */
	vmb_fw_msg_t->tc_num = num_tcs;

	spin_unlock(&vpet->vpe_lock);

	/* Generate an IPI */
	gic_trigger(cpu);
	mips_ihb();

	/* Wait for timeout */
	ret1 = wait_event_interruptible_timeout(vpet->v_wq.vmb_wq,
					(vpet->v_wq.wakeup_vpe == 1),
					QUEUE_TIMEOUT);
	if (ret1 <= 0) {
		pr_info("[%s]:[%d] wait_event timeout occured for CPU = %d\n",
			__func__, __LINE__, cpu);
		pr_info("Consider reseting the CPU using vmb_cpu_force_stop\n");
		vpet->v_wq.wakeup_vpe = 0;
		memset(vmb_fw_msg_t, 0, sizeof(struct VMB_fw_msg_t));
		vmb_tc_free(cpu, -1);
		ret = -VMB_ETIMEOUT;
		goto fin_tcstart;
	}

	memset(vmb_fw_msg_t, 0, sizeof(struct VMB_fw_msg_t));
	vpet->v_wq.wakeup_vpe = 0;

	if (vpet->fw_vmb.status == (uint32_t)FW_VMB_ACK) {
		ret = VMB_SUCCESS;
	} else {
		ret = -VMB_ENACK;
		pr_err("[%s]:[%d] -ENACK recieved from FW. Resetting\n",
		       __func__, __LINE__);
		vmb_tc_free(cpu, -1);
	}

fin_tcstart:
	return ret;
}
EXPORT_SYMBOL(vmb_tc_start);


int8_t vmb_tc_stop(uint8_t cpu, uint8_t tc_num)
{
	int c_id, v_id;
	struct VMB_vpe_t *vpet;
	int ret, ret1;
	struct VMB_fw_msg_t *vmb_fw_msg_t;

	c_id = which_core(cpu);
	v_id = vpe_in_core(cpu);
	vpet = &core_t[c_id].vpe_t[v_id];

/* Per-VPE Lock and update the vmb_fw_msg_t struct and update DDR */

	spin_lock(&vpet->vpe_lock);

	vmb_fw_msg_t = (struct VMB_fw_msg_t *)VMB_get_msg_addr(cpu, 0);
	memset(vmb_fw_msg_t, 0, sizeof(struct VMB_fw_msg_t));

	vmb_fw_msg_t->msg_id = VMB_TC_STOP;
	vmb_fw_msg_t->tc_num = tc_num;

	spin_unlock(&vpet->vpe_lock);

	/* Generate an IPI */
	gic_trigger(cpu);
	mips_ihb();
	/* Wait for timeout */
	ret1 = wait_event_interruptible_timeout(vpet->v_wq.vmb_wq,
						(vpet->v_wq.wakeup_vpe == 1),
						QUEUE_TIMEOUT);
	if (ret1 <= 0) {
		pr_info("[%s]:[%d] wait_event timeout CPU = %d FW = %s\n",
			__func__, __LINE__, cpu, vpet->name);
		vpet->v_wq.wakeup_vpe = 0;
		memset(vmb_fw_msg_t, 0, sizeof(struct VMB_fw_msg_t));
		ret = -VMB_ETIMEOUT;
		goto fin_tcstop;
	}

	memset(vmb_fw_msg_t, 0, sizeof(struct VMB_fw_msg_t));
	vpet->v_wq.wakeup_vpe = 0;

	if (vpet->fw_vmb.status == (uint32_t)FW_VMB_ACK) {
		ret = VMB_SUCCESS;
	} else {
		ret = -VMB_ENACK;
		pr_err("[%s]:[%d] -ENACK recieved from FW. Resetting\n",
		       __func__, __LINE__);
	}

fin_tcstop:
	vmb_tc_free(cpu, tc_num);
	return ret;
}
EXPORT_SYMBOL(vmb_tc_stop);

int8_t vmb_tc_pause(uint8_t cpu, uint8_t tc_num)
{
	int c_id, v_id;
	struct VMB_vpe_t *vpet;
	int ret, ret1;
	struct VMB_fw_msg_t *vmb_fw_msg_t;

	c_id = which_core(cpu);
	v_id = vpe_in_core(cpu);
	vpet = &core_t[c_id].vpe_t[v_id];
/* Per-VPE Lock and update the vmb_fw_msg_t struct and update DDR */

	spin_lock(&vpet->vpe_lock);

	vmb_fw_msg_t = (struct VMB_fw_msg_t *)VMB_get_msg_addr(cpu, 0);
	memset(vmb_fw_msg_t, 0, sizeof(struct VMB_fw_msg_t));
	vmb_fw_msg_t->msg_id = VMB_TC_PAUSE;
	vmb_fw_msg_t->tc_num = tc_num;

	spin_unlock(&vpet->vpe_lock);

	/* Generate an IPI */
	gic_trigger(cpu);
	mips_ihb();
	pr_info("[%s]:[%d] WAITING FOR RESPONSE vpet->v_wq.wakeup_vpe = %d\n",
		__func__, __LINE__, vpet->v_wq.wakeup_vpe);

	/* Wait for timeout */
	ret1 = wait_event_interruptible_timeout(vpet->v_wq.vmb_wq,
					(vpet->v_wq.wakeup_vpe == 1),
					QUEUE_TIMEOUT);
	if (ret1 <= 0) {
		pr_info("[%s]:[%d] wait_event timeout CPU = %d FW = %s\n",
			__func__, __LINE__, cpu, vpet->name);
		pr_info("CPU could not be Paused !\n");
		vpet->v_wq.wakeup_vpe = 0;
		memset(vmb_fw_msg_t, 0, sizeof(struct VMB_fw_msg_t));
		ret = -VMB_ETIMEOUT;
		goto fin_tcpause;
	}

	pr_info("[%s]:[%d] OUTSIDE wakeup_vpe = %d vpet->fw_vmb.status = %d\n",
		__func__, __LINE__, vpet->v_wq.wakeup_vpe, vpet->fw_vmb.status);

	memset(vmb_fw_msg_t, 0, sizeof(struct VMB_fw_msg_t));
	vpet->v_wq.wakeup_vpe = 0;

	if (vpet->fw_vmb.status == (uint32_t)FW_VMB_ACK) {
		ret = VMB_SUCCESS;
		pr_err("[%s]:[%d]  ret = %d\n", __func__, __LINE__, ret);
	} else {
		ret = -VMB_ENACK;
		pr_err("[%s]:[%d] -ENACK recieved from FW . Resetting\n",
		       __func__, __LINE__);
	}

fin_tcpause:
	return ret;
}
EXPORT_SYMBOL(vmb_tc_pause);

int8_t vmb_tc_resume(uint8_t cpu, uint8_t tc_num)
{
	int c_id, v_id;
	struct VMB_vpe_t *vpet;
	int ret, ret1;
	struct VMB_fw_msg_t *vmb_fw_msg_t;

/*
 * CHECK :  GLOBAL LOCK needed as "cpu" can
 * for a simutaneous vmb_cpu_start from 2 FWs
 */

	c_id = which_core(cpu);
	v_id = vpe_in_core(cpu);
	vpet = &core_t[c_id].vpe_t[v_id];

/*
 *CHECK : GLOBAL UNLOCK needed as "cpu" can for a simutaneous
 *vmb_cpu_start from 2 FWs
 */
/*
 *Per-VPE Lock and update the vmb_fw_msg_t struct and update DDR
 */

	spin_lock(&vpet->vpe_lock);

	vmb_fw_msg_t = (struct VMB_fw_msg_t *)VMB_get_msg_addr(cpu, 0);
	memset(vmb_fw_msg_t, 0, sizeof(struct VMB_fw_msg_t));
	vmb_fw_msg_t->msg_id = VMB_TC_RESUME;
	vmb_fw_msg_t->tc_num = tc_num;

	spin_unlock(&vpet->vpe_lock);
	/* Generate an IPI */
	gic_trigger(cpu);
	mips_ihb();
	pr_info("[%s]:[%d] WAITING FOR RESPONSE vpet->v_wq.wakeup_vpe = %d !\n",
		__func__, __LINE__, vpet->v_wq.wakeup_vpe);

	/* Wait for timeout */
	ret1 = wait_event_interruptible_timeout(vpet->v_wq.vmb_wq,
					(vpet->v_wq.wakeup_vpe == 1),
					QUEUE_TIMEOUT);
	if (ret1 <= 0) {
		pr_err("[%s]:[%d] wait_event timeout occured for CPU = %d",
		       __func__, __LINE__, cpu);
		pr_err("starting FW = %sCPU could not be Resumed !!!....\n",
		       vpet->name);
		vpet->v_wq.wakeup_vpe = 0;
		memset(vmb_fw_msg_t, 0, sizeof(struct VMB_fw_msg_t));
		ret = -VMB_ETIMEOUT;
		goto fin_tcres;
	}

	pr_info("[%s]:[%d] OUTSIDE WAIT wakeup_vpe = %d fw_vmb.status = %d",
		__func__, __LINE__, vpet->v_wq.wakeup_vpe, vpet->fw_vmb.status);
	pr_info("ACK = %d cpu = %d ret1 = %d vpet = %p, smp id() = %d",
		FW_VMB_ACK, cpu, ret1, vpet, smp_processor_id());
	pr_info("vpet->fw_vmb = %p !!!!\n", &vpet->fw_vmb);

	memset(vmb_fw_msg_t, 0, sizeof(struct VMB_fw_msg_t));
	vpet->v_wq.wakeup_vpe = 0;

	if (vpet->fw_vmb.status == (uint32_t)FW_VMB_ACK) {
		ret = VMB_SUCCESS;
		pr_err("[%s]:[%d] ret = %d\n", __func__, __LINE__, ret);
	} else {
		ret = -VMB_ENACK;
		pr_err("CPU could not be Resumed !!!....\n");
		pr_err("[%s]:[%d] -ENACK recieved from FW ..\n",
		       __func__, __LINE__);
	}

fin_tcres:
	return ret;
}

int amon_cpu_avail(int cpu)
{
	return 1;
}
EXPORT_SYMBOL(vmb_tc_resume);

/*
 *Need this function to check if a CPU needs to run Linux or not.
 *Linux traverses the NR_CPU sequentially to bring the CPU up but will have
 *issues when we need to run Linux on Core0/VPE0 (CPU 0) and Core1/VPE0 (CPU 2).
 *We cannot set NR_CPU as 2 as again it means CPU0 and CPU1 and is referenced
 *as mentioned in whole kernel . So we cannot set bit 2 as CPU3 instead set
 *NR_CPUS=4 and selectively start the Linux based on DT
 */

int is_linux_OS_vmb(int cpu)
{
	int c_id, v_id;
	struct VMB_vpe_t *vpet;
	int ret = 0;

	c_id = which_core(cpu);
	v_id = vpe_in_core(cpu);
	vpet = &core_t[c_id].vpe_t[v_id];

	if (!strncmp(vpet->name, "LINUX", sizeof(vpet->name)))
		ret = 1;

	return ret;
}

int32_t vmb_yr_get(uint8_t cpu, uint16_t num_yr)
{
	int32_t ret = -VMB_ERROR;
	int i, c_id, n_yr;
	struct VMB_core_t *coret = core_t;
	struct VMB_yr_t *yri;
	int32_t yield_bits = 0x0;

	c_id = which_core(cpu);
	yri = coret[c_id].yr_t;
	n_yr = 0;

	/*
	 * Make sure requested # of Yield resources are available
	 * before proceeding to allocate. if not return -EBUSY.
	 */
	for (i = 0; i < MAX_YIELD_INTF; i++)
		if ((yri[i].yr_status & YR_INACTIVE) == YR_INACTIVE)
			n_yr++;

	if (n_yr < num_yr) {
		ret = -VMB_EBUSY;
		goto yralloc_failed;
	}

	n_yr = 0;
	for (i = 0; i < MAX_YIELD_INTF; i++) {
		if ((yri[i].yr_status & YR_INACTIVE) == YR_INACTIVE) {
			if (n_yr == num_yr)
				break;

			yri[i].yr_status &= ~YR_INACTIVE;
			yri[i].yr_status |= YR_ACTIVE;
			yield_bits |= 0x1 << i;
			yri[i].cpu_id = cpu; /* Add the CPU  */
			n_yr++;
			ret = yield_bits;
		}
	}

yralloc_failed:
	return ret;
}
EXPORT_SYMBOL(vmb_yr_get);

static inline void vmb_yr_free_error(int i, uint8_t cpu)
{
	pr_err("ield ID %d mismatch CPU%d\n", i, cpu);
}

void vmb_yr_free(uint8_t cpu, int16_t yr)
{
	int i, c_id;
	struct VMB_core_t *coret = core_t;
	struct VMB_yr_t *yri;
	int16_t n_yr;

	c_id = which_core(cpu);
	yri = coret[c_id].yr_t;
	i = 0;

	if (yr == -1) {
		for (i = 0; i < MAX_YIELD_INTF; i++) {
			if (yri[i].cpu_id == cpu) {
				yri[i].yr_status &= ~YR_ACTIVE;
				yri[i].yr_status |= YR_INACTIVE;
				yri[i].cpu_id = -1;
			}
		}
	} else {
		do {
			n_yr = (yr >> i) & 0x1;
			if (n_yr) {
				if (yri[i].cpu_id == cpu) {
					yri[i].yr_status &= ~YR_ACTIVE;
					yri[i].yr_status |= YR_INACTIVE;
					yri[i].cpu_id = -1;
				} else {
					vmb_yr_free_error(i, cpu);
				}
			}
			i++;
		} while (i < MAX_YIELD_INTF);
	}
}
EXPORT_SYMBOL(vmb_yr_free);

int32_t vmb_itc_sem_get(uint8_t cpu, uint8_t tc_num)
{
	int32_t ret = -VMB_EBUSY;
	int i;

	for (i = 0; i < MAX_ITC_SEMID; i++) {
		if ((itc_t[i].itc_status & ITC_INACTIVE) == ITC_INACTIVE) {
			ret = i;
			itc_t[i].itc_status &= ~ITC_INACTIVE;
			itc_t[i].itc_status |= ITC_ACTIVE;
			itc_t[i].cpu_id = cpu; /* Add the CPU number*/
			itc_t[i].tc_id = tc_num;
			break;
		}
	}

	return ret;
}
EXPORT_SYMBOL(vmb_itc_sem_get);

int32_t fw_vmb_get_irq(uint8_t cpu)
{
	int32_t ret = -VMB_EAVAIL;

	if (cpu < MAX_CPU)
		ret = g_fw_vmb_hwirq[cpu];

	return ret;
}
EXPORT_SYMBOL(fw_vmb_get_irq);

int vmb_run_tc(uint8_t vpe_num, struct TC_launch_t *tc_launch)
{
	u32 tc_num;

	if (!tc_launch)
		return -VMB_ENOPRM;

	tc_num = tc_launch->tc_num;

	/* VPC=1 EVP=0.*/
	write_c0_mvpcontrol((read_c0_mvpcontrol() & ~MVPCONTROL_EVP) |
			     MVPCONTROL_VPC);
	ehb();
	pr_debug("getmvpctrl = %x\n", (unsigned int)read_c0_mvpcontrol());

	/* set the target to tc_num */
	settc(tc_num);
	ehb();
	pr_debug("getvpectrl for target TC = %x\n",
		 (unsigned int)read_c0_vpecontrol());

	/* set DA bit so yields will work. */
	write_tc_c0_tcstatus(read_tc_c0_tcstatus() | TCSTATUS_DA);
	ehb();
	pr_debug("gettcstatus = %x\n", (unsigned int)read_tc_c0_tcstatus());

	/* Make sure tc is halted. (This done in h/w as part of reset.) */
	write_tc_c0_tchalt(TCHALT_H); /* H=1. */
	ehb();
	pr_debug("tc is halted = %x\n", (unsigned int)read_tc_c0_tchalt());

	/* Make sure tc is bound to vpe.*/
	write_tc_c0_tcbind((vpe_num << TCBIND_CURVPE_SHIFT) |
			    (tc_num << TCBIND_CURTC_SHIFT));
	ehb();
	pr_debug("gettcbind = %x\n", (unsigned int)read_tc_c0_tcbind());

	/* Set the stack pointer so we can call a c function */
	write_tc_gpr_sp(tc_launch->sp);
	ehb();
	pr_debug("stack top = %x\n", tc_launch->sp);

	/* Set the global pointer */
	write_tc_gpr_gp(tc_launch->gp);
	ehb();

	if (tc_launch->priv_info) {
		/* Pass the Arg0 */
		mttgpr(4, tc_launch->priv_info);
		ehb();
	}
	/* Point tc to code */
	write_tc_c0_tcrestart(tc_launch->start_addr);
	ehb();
	pr_debug("start address of TC = %x\n",
		 (unsigned int)read_tc_c0_tcrestart());

	write_tc_c0_tcstatus(read_tc_c0_tcstatus() |
			     (TCSTATUS_A | TCSTATUS_DA));
	ehb();
	pr_debug("current TC status = %x\n",
		 (unsigned int)read_tc_c0_tcstatus());

	/* Unhalt tc. */
	write_tc_c0_tchalt(0); /* H=0 (allow vpe_num/tc_num to execute).*/
	ehb();
	pr_debug("tc is unhalted = %x\n", (unsigned int)read_tc_c0_tchalt());

	/* Enable threading. TE=1.*/
	write_vpe_c0_vpecontrol(read_vpe_c0_vpecontrol() | VPECONTROL_TE);
	ehb();
	pr_debug("enable threading = %x\n",
		 (unsigned int)read_vpe_c0_vpecontrol());

	/* Turn off the vpe configuration flag and enable (other) vpe. */
	write_c0_mvpcontrol((read_c0_mvpcontrol() & ~MVPCONTROL_VPC) |
			     MVPCONTROL_EVP); /* VPC=0 EVP=1. */
	ehb();
	pr_debug("current mvp control = %x\n",
		 (unsigned int)read_c0_mvpcontrol());

	return 0;
}
EXPORT_SYMBOL(vmb_run_tc);
