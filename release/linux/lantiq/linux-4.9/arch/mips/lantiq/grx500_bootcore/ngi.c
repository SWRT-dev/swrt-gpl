/*
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 *
 *  Copyright (C) 2014 Nagaraj Suresh<suresh.nagaraj@lantiq.com>
 */

#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/clk.h>
#include <linux/of_platform.h>
#include <linux/proc_fs.h>
#include <linux/interrupt.h>
#include <linux/of_irq.h>
#include <linux/irq.h>

#include <asm/addrspace.h>

#include <lantiq.h>
#include <lantiq_soc.h>
//#include <lantiq_irq.h>
#include <asm/uaccess.h>

#include "ngi_reg.h"
#include "ngi.h"
#include "ngi_map.h"

/*
 * WW: to make the IRQ work as intended
 * until a decent conversion to irq domain
 * controller driver
 */
#define BOOTCORE_LEGACY_IRQ
#include <asm/setup.h>
#include <grx500_bootcore_interrupt.h>
#include <linux/version.h>

#define NGI_MAX_TA 25
#define NGI_MAX_IA 25
#define NGI_MAX_GROUP_VALUE 64
#define NGI_DEF_WAIT_CYCLES 100000

#define NGI_ERROR_HANDLING

#ifdef NGI_ERROR_HANDLING
#define LTQ_NGI_ROOT_INST1 6
#define LTQ_NGI_ROOT_INST2 7
static int current_ssx;
static int link_target;
static void ngi_err_handling_tasklet(unsigned long);
static struct tasklet_struct ngi_tasklet;
#ifdef BOOTCORE_LEGACY_IRQ
extern void grx500_bootcore_ngi_irq(void);
#endif /* BOOTCORE_LEGACY_IRQ */
#endif

#define NGI_F_IA 1
#define NGI_F_TA 2
#define NGI_F_CORE 4

#if 0
#ifdef __BIG_ENDIAN
#define ltq_ngi_r32(inst_id, reg)  __raw_readl(ltq_ngi_instance[inst_id].membase + (void *)((reg) ^ 0x4))
#else
#define ltq_ngi_r32(inst_id, reg)  __raw_readl(ltq_ngi_instance[inst_id].membase + (void *)(reg))
#endif
#endif

static struct proc_dir_entry *g_ngi_root_proc;
static int g_ngi_root_proc_once;

/* Error handling */
struct ngi_err_info {
	char *agent_name;
	u32 offset;
};

static const char *const ngi_name[LTQ_NUM_NGI_INSTANCES] = {
	"SSX0",
	"SSX1",
	"SSX2",
	"SSX3",
	"SSX4",
	"SSX5",
	"SSX6",
	"SSX7",
	"SSX8",
};

/* Initiator Agent */
struct ngi_ia {
	char *name;
	u32 offset;
	unsigned short rev_code;
	unsigned short core_code;
	unsigned short vendor_code;
	struct proc_dir_entry *proc;
};

struct ngi_inst;

/* Target Agent */
struct ngi_ta {
	struct ngi_inst *pinst; /* Back pointer to the instance */
	u32 offset;
	u32 pm_read_offset;
	u32 pm_write_offset;
	char *name;
	unsigned short rev_code;
	unsigned short core_code;
	unsigned short vendor_code;
	u32 num_pr;
	struct ngi_acl_pr acl_pr[LTQ_NGI_MAX_PR_REGIONS];
	struct proc_dir_entry *proc;
};

/* Register Target*/
struct ngi_rt {
	struct ngi_inst *pinst; /* Back pointer to the instance */
	u32 pm_read_offset;
	u32 pm_write_offset;
	char *name;
	u32 num_pr;
	struct ngi_acl_pr rt_pr[LTQ_NGI_MAX_RT_PR_REGIONS];
	struct proc_dir_entry *proc;
};

/* Generic stuff for all the Instances */
struct ngi_props {
	u32 network_rev;
	u32 network_id;
	u8 initid;
	u8 timeout_base;
	u8 clk_gate_disable;
	u8 si_ctrl_clk_gate_disable;
	u32 num_ta;
	u32 num_ia;
	struct ngi_ta target_agents[NGI_MAX_TA];
	struct ngi_ia init_agents[NGI_MAX_IA];
	struct ngi_rt rt_agent;
};

struct ngi_inst {
	int inst_id;
	const char *name;
	struct device *dev;
	void __iomem *membase;
	struct proc_dir_entry *proc;
	struct proc_dir_entry *ta_proc;
	struct proc_dir_entry *ia_proc;
	struct proc_dir_entry *rt_proc;
	struct ngi_props  props;
};

struct ngi_agent_core {
	u32 ngi_inst_id;
	u32 core_code;
};

static struct ngi_inst ltq_ngi_instance[LTQ_NUM_NGI_INSTANCES];

#define LTQ_NGI_MAX_ERROR_BITS 34

/* Error handling table */
static struct ngi_err_info ngi_error_info_flag_status_0_table[LTQ_NUM_NGI_INSTANCES][LTQ_NGI_MAX_ERROR_BITS] = {
	/* SSX0 */
{{"IA_DMA0", -1},
{"IA_DMA3", -1},
{"IA_DMA4", -1},
{"IA_E123W", -1},
{"IA_E123R", -1},
{"IA_E97W", -1},
{"IA_E97R", -1},
{"IA_EX50", -1},
{"IA_LN10", -1},
{"IA_LN20", -1},
{"IA_LN30", -1},
{"IA_LN60", -1},
{"TA_CBM1", -1},
{"TA_CBM2", -1},
{"TA_LN06", -1},
{"TA_SSB", -1},
{"TA_DMA3", -1},
{"TA_DMA4", -1},
{"TA_EIP123", -1},
{"TA_EIP97", -1},
{"TA_EX04", -1},
{"TA_EX05", -1},
{"TA_LN01", -1},
{"TA_LN02", -1},
{"TA_LN03", -1},
{"TA_OTP", -1},
{"TA_ROM", -1},
{"TA_DMA3", -1},
{"TA_DMA4", -1},
{"TA_EX04", -1},
{"TA_EX05", -1},
{"TA_LN01", -1},
{"TA_LN02", -1},
{"TA_LN03", -1} },

	/* SSX1 */
{{"IA_IXCM2", -1},
{"IA_IXCM3", -1},
{"IA_IXCM4", -1},
{"IA_IXCM5", -1},
{"IA_DMA2R", -1},
{"IA_DMA2T", -1},
{"IA_LN01", -1},
{"TA_DMA2R", -1},
{"NULL", -1},
{"TA_DMA2T", -1},
{"NULL", -1},
{"TA_GSWIP_L", -1},
{"NULL", -1},
{"TA_LN10", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1}, },

	/* SSX2 */
{{"TA_DMA1R", -1},
{"TA_DMA1T", -1},
{"TA_GSWIP_R", -1},
{"TA_USB0", -1},
{"TA_USB0C", -1},
{"TA_USB1", -1},
{"TA_USB1C", -1},
{"IA_DMA1_R", -1},
{"IA_DMA1_T", -1},
{"IA_LN02", -1},
{"IA_USB0_R", -1},
{"IA_USB0_W", -1},
{"IA_USB1_R", -1},
{"IA_USB1_W", -1},
{"IA_XCMF", -1},
{"TA_LN20", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1} },

	/* SSX3 */
{{"TA_PCIE3FPI", -1},
{"TA_PCIE3CR", -1},
{"TA_PCIE1FPI", -1},
{"TA_PCIE1CR", -1},
{"TA_PCIE2FPI", -1},
{"TA_PCIE2CR", -1},
{"IA_LN03", -1},
{"IA_PCIE3R", -1},
{"IA_PCIE3W", -1},
{"IA_PCIE1R", -1},
{"IA_PCIE1W", -1},
{"IA_PCIE2R", -1},
{"IA_PCIE2W", -1},
{"TA_LN30", -1},
{"TA_MSI3", -1},
{"TA_MSI1", -1},
{"TA_MSI2", -1},
{"TA_PCI3", -1},
{"TA_PCIE1", -1},
{"TA_PCIE2", -1},
{"TA_PCIE3", -1},
{"TA_PCIE3_A", -1},
{"TA_PCIE1", -1},
{"TA_PCIE1_A", -1},
{"TA_PCIE2", -1},
{"TA_PCIE2_A", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1} },

	/* SSX4 */
{{"IA_EX04", -1},
{"TA_CGU", -1},
{"TA_GPIO", -1},
{"TA_LEDC", -1},
{"TA_PCM", -1},
{"TA_PMU", -1},
{"TA_RCU", -1},
{"TA_ASC0", -1},
{"TA_ASC1", -1},
{"TA_DMA0", -1},
{"TA_EBU", -1},
{"TA_GPT0", -1},
{"TA_I2C", -1},
{"TA_SSC0", -1},
{"TA_SSC1", -1},
{"TA_ASC0", -1},
{"TA_ASC1", -1},
{"TA_DMA0", -1},
{"TA_EBU", -1},
{"TA_GPT0", -1},
{"TA_SSC0", -1},
{"TA_SSC1", -1},
{"TA_GPT1", -1},
{"TA_GPT2", -1},
{"TA_GPT1", -1},
{"TA_GPT2", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1}, },

	/* SSX5 */
{{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1} },

	/* SSX6 */
	{{"IA_L2", -1},
	{"IA_LN06", -1},
	{"TA_DDR", -1},
	{"TA_IC", -1},
	{"TA_LN60", -1},
	{"TA_PCTL", -1},
	{"TA_PUB", -1},
	{"TA_LN60", -1},
	{"IA_TEP", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1}, },

	/* SSX7 */
	{{"IA_MO2", -1},
{"IA_MPE", -1},
{"TA_MI2", -1},
{"TA_MPE", -1},
{"TA_TOE", -1},
{"TA_MI2", -1},
{"TA_TOE", -1},
{"TA_MPE", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1}, },

	/* SSX8 */
{{"IA_SSX7", -1},
{"TA_MCPY", -1},
{"TA_MPEC", -1},
{"TA_MTR", -1},
{"TA_SE", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1}, }
};

static struct ngi_err_info ngi_error_info_flag_status_1_table[LTQ_NUM_NGI_INSTANCES][LTQ_NGI_MAX_ERROR_BITS] = {
/* SSX0 */
{ {"TA_REG0", -1},
{"TA_CBM1", -1},
{"TA_CBM2", -1},
{"TA_LN06", -1},
{"TA_SSB", -1},
{"TA_DMA3", -1},
{"TA_DMA4", -1},
{"TA_EIP123", -1},
{"TA_EIP97", -1},
{"TA_EX04", -1},
{"TA_EX05", -1},
{"TA_LN01", -1},
{"TA_LN02", -1},
{"TA_LN03", -1},
{"TA_OTP", -1},
{"TA_ROM", -1},
{"TA_EX04", -1},
{"TA_EX05", -1},
{"TA_LN01", -1},
{"TA_LN02", -1},
{"TA_LN03", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1} },

	/* SSX1 */
{ {"IA_DMA2R", -1},
{"IA_DMA2T", -1},
{"TA_GSWIP_L", -1},
{"TA_LN10", -1},
{"TA_REG1", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1} },

	/* SSX2 */
{ {"TA_DMA1R", -1},
{"TA_DMA1T", -1},
{"TA_GSWIP_R", -1},
{"TA_USB0", -1},
{"TA_USB0C", -1},
{"TA_USB1", -1},
{"TA_USB1C", -1},
{"TA_LN20", -1},
{"TA_REG2", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1} },

	/* SSX3 */
{ {"TA_REG3", -1},
{"TA_PCIE3_A", -1},
{"TA_PCIE3_C", -1},
{"TA_PCIE1_A", -1},
{"TA_PCIE1_C", -1},
{"TA_PCIE2_A", -1},
{"TA_PCIE2_C", -1},
{"TA_LN30", -1},
{"TA_MSI3", -1},
{"TA_MSI1", -1},
{"TA_MSI2", -1},
{"TA_PCIE3", -1},
{"TA_PCIE1", -1},
{"TA_PCIE2", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1} },

	/* SSX4 */
{{"TA_CGU", -1},
{"TA_GPIO", -1},
{"TA_LEDC", -1},
{"TA_PCM", -1},
{"TA_PMU", -1},
{"TA_RCU", -1},
{"TA_REG4", -1},
{"TA_ASC0", -1},
{"TA_ASC1", -1},
{"TA_DMA0", -1},
{"TA_EBU", -1},
{"TA_GPT0", -1},
{"TA_SSC0", -1},
{"TA_SSC1", -1},
{"TA_I2C", -1},
{"TA_GPT1", -1},
{"TA_GPT2", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1} },

	/* SSX5 */
{{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1} },

	/* SSX6 */
	{{"TA_DDR", -1},
{"TA_IC", -1},
{"TA_LN60", -1},
{"TA_PCTL", -1},
{"TA_PUB", -1},
{"TA_REG6", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1} },

	/* SSX7 */
	{{"TA_MI2", -1},
{"TA_MPE", -1},
{"TA_TOE", -1},
{"TA_MI2", -1},
{"TA_TOE", -1},
{"TA_MPE", -1},
{"TA_REG7", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1} },

	/* SSX8 */
{{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1},
{"NULL", -1}, }
};

/*
 * handy ngi register accessor
 */
static inline unsigned int ltq_ngi_r32(struct ngi_inst *pinst, u32 offset)
{
	return ltq_r32(pinst->membase + offset);
}

static inline void ltq_ngi_w32(struct ngi_inst *pinst, u32 value, u32 offset)
{
	return ltq_w32(value, pinst->membase + offset);
}

static inline void  ltq_ngi_w32_mask(struct ngi_inst *pinst, u32 clear, u32 set, u32 offset)
{
	return ltq_w32_mask(clear, set, pinst->membase + (offset));
}

#ifdef NGI_ERROR_HANDLING

static int get_current_ssx(void)
{
	return current_ssx;
}

static void set_current_ssx(int ssx)
{
	current_ssx = ssx;
}

static int get_is_bit_set(u64 flags)
{
	int i;

	for (i = 0; i <= 63; i++) {
		if ((flags & (1 << i)) != 0)
			return i;
	}
	return -1;
}

static int is_agent_link_target(int bitpos, int err_type, int *ssx)
{
	char agent_name[16];

	if (!err_type)
		strcpy(agent_name, ngi_error_info_flag_status_0_table[get_current_ssx()][bitpos].agent_name);
	else
		strcpy(agent_name, ngi_error_info_flag_status_1_table[get_current_ssx()][bitpos].agent_name);

	if (strncmp(agent_name, "TA_LN60", 7) == 0) {
		*ssx = 0;
		return 1;
	} else if (strncmp(agent_name, "TA_LN01", 7) == 0) {
		*ssx = 1;
		return 1;
	} else if (strncmp(agent_name, "TA_LN02", 7) == 0) {
		*ssx = 2;
		return 1;
	} else if (strncmp(agent_name, "TA_LN03", 7) == 0) {
		*ssx = 3;
		return 1;
	} else if (strncmp(agent_name, "TA_LN04", 7) == 0) {
		*ssx = 4;
		return 1;
	}
	return 0;
}
static void decode_ta_error(struct ngi_inst *p_inst, u64 agent_status, u64 error_log, u32 base_offset)
{
	u64 set_agent_status = 0;
	u32 reset_core_flag = 0;
	u32 code = 0, serror = 0, burst_close = 0, req_timeout = 0, func_rst_timeout = 0;
	code  = (u32)((uint64_t)(error_log & IE97R_IA_ERROR_LOG_CODE_MASK) >> IE97R_IA_ERROR_LOG_CODE_POS);

	switch (code) {
	case 7:
		pr_err("%s: req_timeout\r\n", __func__);
		break;
	case 8:
		pr_err("%s: req_timeout\r\n", __func__);
		break;
	case 9:
		pr_err("%s: inband error\r\n", __func__);
		break;
	};

	if (code) {
		code = error_log & ~(u64)IE97R_IA_ERROR_LOG_CODE_MASK;
		ltq_ngi_w32(p_inst, code, base_offset + 0x58);
	}

	serror = (u32)((uint64_t)(agent_status & TLN06_TA_AGENT_STATUS_SERROR_MASK) >> TLN06_TA_AGENT_STATUS_SERROR_POS);

	if (serror) {
		pr_err("%s: serror\r\n", __func__);
		set_agent_status |= TLN06_TA_AGENT_STATUS_SERROR_MASK;
	}

	burst_close = (u32)((uint64_t)(agent_status & TLN06_TA_AGENT_STATUS_BURST_CLOSE_MASK) >> TLN06_TA_AGENT_STATUS_BURST_CLOSE_POS);
	if (burst_close) {
		pr_err("%s: burst_close\r\n", __func__);
		reset_core_flag = 1;
	}

	req_timeout = (u32)((uint64_t)(agent_status & TLN06_TA_AGENT_STATUS_REQ_TIMEOUT_MASK) >> TLN06_TA_AGENT_STATUS_REQ_TIMEOUT_POS);
	if (req_timeout) {
		pr_err("%s: req_timeout\r\n", __func__);
		reset_core_flag = 1;
	}

	func_rst_timeout = (u32)((uint64_t)(agent_status & TLN06_TA_AGENT_STATUS_FUNCTIONAL_RESET_TIMEOUT_MASK) >> TLN06_TA_AGENT_STATUS_FUNCTIONAL_RESET_TIMEOUT_POS);
	if (func_rst_timeout) {
		pr_err("%s: func_rst_timeout\r\n", __func__);
		reset_core_flag = 1;
	}

	if (reset_core_flag) {
		/*#FIXME: */

	} else {
		set_agent_status |= TLN06_TA_AGENT_STATUS_RFU0_MASK | TLN06_TA_AGENT_STATUS_RFU1_MASK | TLN06_TA_AGENT_STATUS_RFU2_MASK;
		ltq_ngi_w32(p_inst, set_agent_status, base_offset + 0x28);
	}

}

static void decode_ia_error(struct ngi_inst *p_inst, u64 agent_status, u64 error_log, u32 base_offset)
{
	u64 set_agent_status = 0;
	u32 reset_core_flag = 0;
	u32 code = 0, merror = 0, burst_timeout = 0, resp_timeout = 0;
	code  = (u32)((uint64_t)(error_log & IE97R_IA_ERROR_LOG_CODE_MASK) >> IE97R_IA_ERROR_LOG_CODE_POS);

	switch (code) {
	case 1:
		pr_err("%s: unsupported command error\r\n", __func__);
		break;
	case 2:
		pr_err("%s: address hole error\r\n", __func__);
		break;
	case 4:
		pr_err("%s: inband error\r\n", __func__);
		break;
	};

	if (code) {
		code = error_log & ~(u64)IE97R_IA_ERROR_LOG_CODE_MASK;
		ltq_ngi_w32(p_inst, code, base_offset + 0x58);
	}

	merror = (u32)((uint64_t)(agent_status & IE97R_IA_AGENT_STATUS_MERROR_MASK) >> IE97R_IA_AGENT_STATUS_MERROR_POS);

	if (merror) {
		pr_err("%s: merror\r\n", __func__);
		set_agent_status |= IE97R_IA_AGENT_STATUS_MERROR_MASK;
	}

	burst_timeout = (u32)((uint64_t)(agent_status & IE97R_IA_AGENT_STATUS_BURST_TIMEOUT_MASK) >> IE97R_IA_AGENT_STATUS_BURST_TIMEOUT_POS);
	if (burst_timeout) {
		pr_err("%s: burst_timeout\r\n", __func__);
		reset_core_flag = 1;
	}

	resp_timeout = (u32)((uint64_t)(agent_status & IE97R_IA_AGENT_STATUS_RESP_TIMEOUT_MASK) >> IE97R_IA_AGENT_STATUS_RESP_TIMEOUT_POS);
	if (resp_timeout) {
		pr_err("%s: resp_timeout\r\n", __func__);
		reset_core_flag = 1;
	}

	if (reset_core_flag) {
		/*#FIXME: */

	} else {
		set_agent_status |= IE97R_IA_AGENT_STATUS_INBAND_ERROR_PRIMARY_MASK | IE97R_IA_AGENT_STATUS_INBAND_ERROR_SECONDARY_MASK;
		ltq_ngi_w32(p_inst, set_agent_status, base_offset + 0x28);
	}
}

static void handle_agent_error(int bitpos, int err_type)
{
	u64 value;
	u64 agent_status, error_log;
	u32 base_offset;
	struct ngi_inst *p_inst = &ltq_ngi_instance[get_current_ssx()];
	u32 offset;

	if (err_type) {
		base_offset = ngi_error_info_flag_status_1_table[get_current_ssx()][bitpos].offset;
		offset = base_offset;
		pr_err("%s: pm error log offset 0x%x\r\n", __func__, base_offset);
		value = (((uint64_t)ltq_ngi_r32(p_inst, offset+4) << 32) | (uint64_t)ltq_ngi_r32(p_inst, offset));
		pr_err("command0x%x\r\n", (unsigned int)((uint64_t)(value & TREG0_PM_ERROR_LOG_CMD_MASK) >> TREG0_PM_ERROR_LOG_CMD_POS));
		pr_err("Protection region number 0x%x\r\n", (unsigned int)((uint64_t)(value & TREG0_PM_ERROR_LOG_REGION_MASK) >> TREG0_PM_ERROR_LOG_REGION_POS));
		pr_err("Initiator Id 0x%x\r\n", (unsigned int)((uint64_t)(value & TREG0_PM_ERROR_LOG_INITID_MASK) >> TREG0_PM_ERROR_LOG_INITID_POS));
		pr_err("Req_Info Role bits 0x%x\r\n", (unsigned int)((uint64_t)(value & TREG0_PM_ERROR_LOG_REQ_INFO_MASK) >> TREG0_PM_ERROR_LOG_REQ_INFO_POS));
		pr_err("Code 0x%x\r\n", (unsigned int)((uint64_t)(value & TREG0_PM_ERROR_LOG_CODE_MASK) >> TREG0_PM_ERROR_LOG_CODE_POS));
		pr_err("Secondary Error 0x%x\r\n", (unsigned int)((uint64_t)(value & TREG0_PM_ERROR_LOG_SECONDARY_MASK) >> TREG0_PM_ERROR_LOG_SECONDARY_POS));
		pr_err("multi Error 0x%x\r\n", (unsigned int)((uint64_t)(value & TREG0_PM_ERROR_LOG_MULTI_MASK) >> TREG0_PM_ERROR_LOG_MULTI_POS));
		pr_err("Group 0x%x\r\n", (unsigned int)((uint64_t)(value & TREG0_PM_ERROR_LOG_GROUP_MASK) >> TREG0_PM_ERROR_LOG_GROUP_POS));

		if ((uint64_t)(value & TREG0_PM_ERROR_LOG_MULTI_MASK) >> TREG0_PM_ERROR_LOG_MULTI_POS)
			offset = base_offset + 0x18;
		else 
			offset =  base_offset + 0x10;

		/*read the register to clear the error*/
		value = (((uint64_t)ltq_ngi_r32(p_inst, offset+4) << 32) | (uint64_t)ltq_ngi_r32(p_inst, offset));
	} else {
		char agent_name[16];
		strcpy(agent_name, ngi_error_info_flag_status_0_table[get_current_ssx()][bitpos].agent_name);
		base_offset =  ngi_error_info_flag_status_0_table[get_current_ssx()][bitpos].offset;
		pr_err("%s: pm error log offset 0x%x %s\r\n", __func__, base_offset, agent_name);
		offset = base_offset + 0x28;
		agent_status = (((uint64_t)ltq_ngi_r32(p_inst, offset+4) << 32) | (uint64_t)ltq_ngi_r32(p_inst, offset));
		offset = base_offset + 0x58;
		error_log = (((uint64_t)ltq_ngi_r32(p_inst, offset+4) << 32) | (uint64_t)ltq_ngi_r32(p_inst, offset));
		if (strncmp(agent_name, "TA", 2) == 0)
			decode_ta_error(p_inst, agent_status, error_log, base_offset);
		else 
			decode_ia_error(p_inst, agent_status, error_log, base_offset);
	}
}

static void decode_bits(u64 flags, int err_type)
{
	int bitpos = 0;
	int i = 0;

	while (flags) {
		bitpos = get_is_bit_set(flags);
		if (bitpos ==  -1) {
			pr_err("%s bitpos cannot be -1\r\n", __func__);
			return;
		}

		if (is_agent_link_target(bitpos, err_type, &i)) {
			link_target |= 1 << i;
		} else {
			printk("%s: bitpos %d, errType %d\r\n", __func__,
							bitpos, err_type);
			if (bitpos < LTQ_NGI_MAX_ERROR_BITS) {
				handle_agent_error(bitpos, err_type);
			} else {
				pr_err("bitpos cannot be > %d \n", LTQ_NGI_MAX_ERROR_BITS);
				return;
			}
		}

		/*clear the flag for the current bitpos*/
		flags &= ~(1 << bitpos);

		if ((!flags) && (get_current_ssx() == 6) && (link_target && 0x1)) {
			set_current_ssx(0);
			return;
		}
		if ((!flags) && (!(get_current_ssx() == 6))) {
			if (link_target & 0x2) {
				set_current_ssx(1);
				return;
			} else if (link_target & 0x4) {
				set_current_ssx(2);
				return;
			} else if (link_target & 0x8) {
				set_current_ssx(3);
				return;
			} else if (link_target & 0x10) {
				set_current_ssx(4);
				return;
			}
		}
	}
}

static u64 read_flag_status(struct ngi_inst *p_inst, int err_type)
{
	u64 value;
	u32 offset;

	if (!err_type)
		offset = TREG0_SI_FLAG_STATUS_0;
	else
		offset = TREG0_SI_FLAG_STATUS_1;
	pr_err(" %s: offset 0x%x\r\n", __func__, offset);

	value = (((uint64_t)ltq_ngi_r32(p_inst, offset+4) << 32) | (uint64_t)ltq_ngi_r32(p_inst, offset));
	/* For testing */
#if 0
	if (get_current_ssx() == 6)
		value = 0x10;
	else if (get_current_ssx() == 0)
		value = 0x800000;
	else
		value = 0x10;
#endif
	return value;
}

static void ssx_handle(int err_type)
{
	u64 flag_status;
	struct ngi_inst *ngi_instance = &ltq_ngi_instance[get_current_ssx()];

	flag_status = read_flag_status(ngi_instance, err_type);
	if (flag_status) {
		decode_bits(flag_status, err_type);
	}
}

static void ngi_err_handling_tasklet(unsigned long tmp)
{
	set_current_ssx(6);

	do {
		link_target &= ~(1 << get_current_ssx());
		ssx_handle(0);
	} while (link_target);

	set_current_ssx(6);
	do {
		link_target &= ~(1 << get_current_ssx());
		ssx_handle(1);
	} while (link_target);
}

static irqreturn_t ngi_isr(int irq, void *dev_id)
{
	/* Disable all interrupts */

	/*clear interrupt*/

	/* Schedule the tasklet */
	/*FIX ME*/
	/*if the interrupt is for ssx6*/
	tasklet_schedule(&ngi_tasklet);

	return IRQ_HANDLED;
}

#if defined(NGI_ERROR_HANDLING) && defined(BOOTCORE_LEGACY_IRQ)
static struct irqaction ngi_irqaction = {
      .handler        = ngi_isr,
      .flags          = 0,
      .name           = "grx500_bootcore_ngi",
};
#endif /* defined(NGI_ERROR_HANDLING) && defined(BOOTCORE_LEGACY_IRQ) */

#endif

static struct ngi_ta *ltq_ngi_lookup_target_agent(struct ngi_inst *pinst, u32 core_code, u32 flags)
{
	int i;

	if (flags) {
		/* Compare only the first byte and return */
		for (i = 0; i < pinst->props.num_ta; i++) {
			if ((pinst->props.target_agents[i].core_code & 0xff) == (core_code & 0xff))
				break;
		}
	} else {
		for (i = 0; i < pinst->props.num_ta; i++) {
			if (pinst->props.target_agents[i].core_code == core_code)
				break;
		}
	}

	if (i < pinst->props.num_ta)
		return &(pinst->props.target_agents[i]);
	else
		return NULL;
}

static struct ngi_ia *ltq_ngi_lookup_init_agent (struct ngi_inst *pinst, u32 core_code, u32 flags)
{
	int i;

	if (flags) {
		/* Compare only the first byte */
		for (i = 0; i < pinst->props.num_ia; i++) {
			if ((pinst->props.init_agents[i].core_code & 0xff) == (core_code & 0xff))
				break;
		}
	} else {
		for (i = 0; i < pinst->props.num_ia; i++) {
			if (pinst->props.init_agents[i].core_code == core_code)
				break;
		}
	}

	if (i < pinst->props.num_ia)
		return &(pinst->props.init_agents[i]);
	else
		return NULL;
}

static char *ltq_lookup_ia_code(u32 core_code)
{
	int i;

	for (i = 0; i < LTQ_NUM_INIT_AGENTS; i++) {
		if (ngi_ia_core_table[i].core_code == core_code)
			break;
	}

	if (i < LTQ_NUM_INIT_AGENTS)
		return ngi_ia_core_table[i].core_name;
	else
		return NULL;
}

static int ltq_lookup_ta_index(u32 core_code)
{
	int i;

	for (i = 0; i < LTQ_NUM_TARGET_AGENTS; i++) {
		if (ngi_ta_core_table[i].core_code == core_code)
			break;
	}

	if (i < LTQ_NUM_TARGET_AGENTS)
		return i;
	else
		return -1;
}

static int ltq_lookup_rt_index(u32 inst_id)
{
	int i;

	for (i = 0; i < LTQ_NUM_NGI_INSTANCES; i++) {
		if (ngi_rt_core_table[i].inst_id == inst_id)
			break;
	}

	if (i < LTQ_NUM_NGI_INSTANCES)
		return i;
	else
		return -1;
}

/* PROC related functions */
static void ngi_remove_space(char **p, int *len)
{
	while (*len && **p == ' ') {
		(*p)++;
		(*len)--;
	}
}

static int get_number(char **p, int *len, int is_hex)
{
	unsigned int ret = 0;
	int n = 0;
	int nega_sign = 0;

	ngi_remove_space(p, len);
	if (*len == 0) {
		pr_err("get_number's buffer len is 0\n");
		return 0;
	}
	if ((*p)[0] == '0' && (*p)[1] == 'x') {
		is_hex = 1;
		(*p) += 2;
		(*len) -= 2;
	} else if ((*p)[0] == '-') {
		nega_sign = 1;
		(*p) += 1;
		(*len) -= 1;
	}

	if (is_hex) {
		while (*len && ((**p >= '0' && **p <= '9') || (**p >= 'a' && **p <= 'f') || (**p >= 'A' && **p <= 'F'))) {
			if (**p >= '0' && **p <= '9')
				n = **p - '0';
			else if (**p >= 'a' && **p <= 'f')
				n = **p - 'a' + 10;
			else if (**p >= 'A' && **p <= 'F')
				n = **p - 'A' + 10;
			ret = (ret << 4) | n;
			(*p)++;
			(*len)--;
		}
	} else {
		while (*len && **p >= '0' && **p <= '9') {
			n = **p - '0';
			ret = ret * 10 + n;
			(*p)++;
			(*len)--;
		}
	}
	if (nega_sign) 
		ret = -ret;

	return ret;
}

static int ngi_inst_enum_read_proc(struct seq_file *s, void *v)
{
	struct ngi_inst *pinst = s->private;
	struct ngi_props *pprops = &(pinst->props);

	seq_puts(s, "\nEnumeration info\n");
	seq_puts(s, "-----------------------------------------\n");
	seq_printf(s, "Instance ID =        %d\n", pinst->inst_id);
	seq_printf(s, "Name =         %s\n", pinst->name);
	seq_printf(s, "Memory base =       %08x\n", (unsigned int)(pinst->membase));
	seq_printf(s, "Number or TAs =  %d\n", pprops->num_ta);
	seq_printf(s, "Number of IAs =   %d\n", pprops->num_ia);
	seq_printf(s, "Network Rev =     %08x\n", pprops->network_rev);
	seq_printf(s, "Network Id =     %08x\n", pprops->network_id);
	seq_printf(s, "InitId =  %08x\n", pprops->initid);
	seq_printf(s, "Timeout Base =  %08x\n", pprops->timeout_base);
	seq_printf(s, "Clk Gate Disable = %08x\n", pprops->clk_gate_disable);
	seq_printf(s, "SI Control clk gate disable = %08x\n", pprops->si_ctrl_clk_gate_disable);

	return 0;
}

static int ngi_inst_enum_read_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, ngi_inst_enum_read_proc, PDE_DATA(inode));
}

static const struct file_operations ngi_inst_enum_proc_fops = {
	.open           = ngi_inst_enum_read_proc_open,
	.read           = seq_read,
	.llseek         = seq_lseek,
	.release        = single_release,
};

/* Target Agent related info proc fs */
static int ngi_ta_info_read_proc(struct seq_file *s, void *v)
{
	struct ngi_ta *pta = s->private;

	seq_puts(s, "\n TA info\n");
	seq_puts(s, "-----------------------------------------\n");
	seq_printf(s, "Name =         %s\n", pta->name);
	seq_printf(s, "Rev code =       %08x\n", (unsigned int)(pta->rev_code));
	seq_printf(s, "Core code =  %08x\n", (unsigned int) (pta->core_code));
	seq_printf(s, "Vendor code =  %08x\n", (unsigned int) (pta->vendor_code));
	seq_printf(s, "Reg. Offset =  %08x\n", (unsigned int) (pta->offset));
	seq_printf(s, "Number of Protection Regions =   %d\n", pta->num_pr);

	return 0;
}

static int ngi_ta_info_read_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, ngi_ta_info_read_proc, PDE_DATA(inode));
}

static const struct file_operations ngi_ta_info_proc_fops = {
	.open           = ngi_ta_info_read_proc_open,
	.read           = seq_read,
	.llseek         = seq_lseek,
	.release        = single_release,
};

/* Init Agent related info proc fs */
static int ngi_ia_info_read_proc(struct seq_file *s, void *v)
{
	struct ngi_ia *pia = s->private;

	seq_puts(s, "\n IA info\n");
	seq_puts(s, "-----------------------------------------\n");
	seq_printf(s, "Name =         %s\n", pia->name);
	seq_printf(s, "Rev code =       %08x\n", (unsigned int)(pia->rev_code));
	seq_printf(s, "Core code =  %08x\n", (unsigned int) (pia->core_code));
	seq_printf(s, "Vendor code =  %08x\n", (unsigned int) (pia->vendor_code));
	seq_printf(s, "Reg. Offset =  %08x\n", (unsigned int) (pia->offset));

	return 0;
}

static int ngi_ia_info_read_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, ngi_ia_info_read_proc, PDE_DATA(inode));
}

static const struct file_operations ngi_ia_info_proc_fops = {
	.open           = ngi_ia_info_read_proc_open,
	.read           = seq_read,
	.llseek         = seq_lseek,
	.release        = single_release,
};


/* Permission related proc fs for Target Agents */
ssize_t ngi_ta_pm_info_proc_write(struct file *file, const char *buf, size_t count, loff_t *ppos)
{
	int len;
	char str[64];
	char *p = (char *)buf;
	struct seq_file *m = file->private_data;
	struct ngi_ta *pta  = (struct ngi_ta *) (m->private);
	int f_read = 0, f_write = 0;
	u32 value, region;

	len = (sizeof(str) > count) ? count : sizeof(str)-1 ;
	len -= copy_from_user(str, buf, len);
	str[len] = 0;

	if ( !*p || !len )
		return count;

    if (strncmp(p, "read_pm", 7) == 0) {
		p += 7;
		f_read = 1;
	} else if (strncmp(p, "write_pm", 8) == 0) {
		p += 8;
		f_write = 1;
	} else if (strncmp(p, "help", 4) == 0 || *p == '?') {
		printk("echo <write_pm/read_pm> [pr_region register_value] > /proc/driver/ngi/SSXx/TA_xxx/protection_info \n");
	}

	if (f_read || f_write) {
		region = get_number(&p, &len, 0);

		if (region > (pta->num_pr - 1)) {
			pr_err("Wrong PR region %u, it should less than %u\n", region, pta->num_pr);
			return count;
		}

		value = get_number(&p, &len, 0);

		/* Write the group value in the PM register and update the table as well */
		if (f_read) {
			pr_info ("we have to write : %x to read pm register of region : %u for TA: %s \n", value, region, pta->name);
		pta->acl_pr[region].group_read_pr = value; 
			pr_debug("address: %x \n", (pta->pm_read_offset + (region*0x20) + 4));
			ltq_ngi_w32(pta->pinst, value, (pta->pm_read_offset + (region*0x20) + 4));
		} else {
			pr_debug("we have to write : %x to write pm register of region: %u for TA: %s \n", value, region, pta->name);
			pta->acl_pr[region].group_write_pr = value; 
			pr_debug("address: %x \n", (pta->pm_write_offset + (region*0x20) + 4));
			ltq_ngi_w32(pta->pinst, value, (pta->pm_write_offset + (region*0x20) + 4));
		}
	}

	return count;
}

static int ngi_ta_pm_info_proc_read (struct seq_file *s, void *v)
{
	struct ngi_ta *pta = s->private;
	struct ngi_acl_pr *pacl;
	int i;

	for (i = 0; i < pta->num_pr; i++) {
		pacl = &(pta->acl_pr[i]);
		seq_printf(s, "Protection Region: %d = \n", i);
		seq_printf(s, "---------------------\n");
		seq_printf(s, "Name =         %s\n", pacl->pr_name);
		seq_printf(s, "ID =         %d\n", pacl->pr_id);
		seq_printf(s, "Base =         %08x\n", (unsigned int) (pacl->base));
		seq_printf(s, "Size =         %08x\n", pacl->size);
		seq_printf(s, "Level =         %d\n", pacl->level);
		seq_printf(s, "Read permission =        %08x\n", pacl->group_read_pr);
		seq_printf(s, "Write permission =        %08x\n", pacl->group_write_pr);
	}

	return 0;
}

static int ngi_ta_pm_info_proc_open (struct inode *inode, struct file *file)
{
	return single_open(file, ngi_ta_pm_info_proc_read, PDE_DATA(inode));
}

static const struct file_operations ngi_ta_pm_info_proc_fops = {
	.open           = ngi_ta_pm_info_proc_open,
	.read           = seq_read,
	.llseek         = seq_lseek,
	.release        = single_release,
	.write		= ngi_ta_pm_info_proc_write
};

/* Permission related proc fs for Register Target */
ssize_t ngi_rt_pm_info_proc_write(struct file *file, const char *buf, size_t count, loff_t *ppos)
{
	int len;
	char str[64];
	char *p = (char *)buf;
	struct seq_file *m = file->private_data;
	struct ngi_rt *prt  = (struct ngi_rt *) (m->private);
	int f_read = 0, f_write = 0;
	u32 value, region;

	len = (sizeof(str) > count) ? count : sizeof(str)-1 ;
	len -= copy_from_user(str, buf, len);
	str[len] = 0;

	if (!*p || !len)
		return count;

        if (strncmp(p, "read_pm", 7) == 0 ) {
		p += 7;
		f_read = 1;
	} else if (strncmp(p, "write_pm", 8) == 0) {
		p += 8;
		f_write = 1;
	} else if (strncmp(p, "help", 4) == 0 || *p == '?') { 
		printk("echo <enable/disable> [err/dbg/assert/rx/tx/header/init/all] > /proc/eth/dbg\n");
	}
	
	if (f_read || f_write) {
		region = get_number(&p, &len, 0);

		if (region > (prt->num_pr - 1)) {
			pr_err("Wrong PR region %u, it should less than %u\n", region, prt->num_pr);
			return count;
		}

		value = get_number(&p, &len, 0);

		/* Write the group value in the PM register and update the table as well */
		if (f_read) {
			pr_info ("we have to write : %x to read pm register of region : %u for TA: %s \n", value, region, prt->name);
			prt->rt_pr[region].group_read_pr = value; 
			pr_debug("address: %x \n", (prt->pm_read_offset + (region*0x20) + 4));
			ltq_ngi_w32(prt->pinst, value, (prt->pm_read_offset + (region*0x20) + 4));
		} else {
			pr_debug("we have to write : %x to write pm register of region: %u for TA: %s \n", value, region, prt->name);
			prt->rt_pr[region].group_write_pr = value; 
			pr_debug("address: %x \n", (prt->pm_write_offset + (region*0x20) + 4));
			ltq_ngi_w32(prt->pinst, value, (prt->pm_write_offset + (region*0x20) + 4));
		}
	}

	return count;
}

static int ngi_rt_pm_info_proc_read(struct seq_file *s, void *v)
{
	struct ngi_rt *prt = s->private;
	struct ngi_acl_pr *pacl;
	int i;

	for (i = 0; i < prt->num_pr; i++) {
		pacl = &(prt->rt_pr[i]);
		seq_printf(s, "Protection Region: %d = \n", i);
		seq_printf(s, "---------------------\n");
		seq_printf(s, "Name =         %s\n", pacl->pr_name);
		seq_printf(s, "ID =         %d\n", pacl->pr_id);
		seq_printf(s, "Base =         %08x\n", (unsigned int) (pacl->base));
		seq_printf(s, "Size =         %08x\n", pacl->size);
		seq_printf(s, "Level =         %d\n", pacl->level);
		seq_printf(s, "Read permission =        %08x\n", pacl->group_read_pr);
		seq_printf(s, "Write permission =        %08x\n", pacl->group_write_pr);
	}

	return 0;
}

static int ngi_rt_pm_info_proc_open (struct inode *inode, struct file *file)
{
	return single_open(file, ngi_rt_pm_info_proc_read, PDE_DATA(inode));
}

static const struct file_operations ngi_rt_pm_info_proc_fops = {
	.open           = ngi_rt_pm_info_proc_open,
	.read           = seq_read,
	.llseek         = seq_lseek,
	.release        = single_release,
	.write		= ngi_rt_pm_info_proc_write
};

static int ngi_proc_init(struct ngi_inst *pinst)
{
	char proc_name[64] = {0};
	struct proc_dir_entry *entry;
	struct ngi_props *pprops = &pinst->props;
	struct ngi_ta *pta;
	struct ngi_ia *pia;
	struct ngi_rt *prt;
	int i;

	if (!g_ngi_root_proc_once) {
		strcpy(proc_name, "driver/ngi");
		g_ngi_root_proc = proc_mkdir(proc_name, NULL);
		if (!g_ngi_root_proc)
			return -ENOMEM;
		g_ngi_root_proc_once++;
	}

	strcpy(proc_name, pinst->name);
	pinst->proc = proc_mkdir(proc_name, g_ngi_root_proc);
	if (!pinst->proc)
		return -ENOMEM;

	entry = proc_create_data("enumeration_info", 0, pinst->proc,
		&ngi_inst_enum_proc_fops, pinst);
	if (!entry)
		goto err1;

	strcpy(proc_name, "target_agents");
	pinst->ta_proc = proc_mkdir(proc_name, pinst->proc);
	if (!pinst->ta_proc)
		return -ENOMEM;


	/* Create the Target Agents proc */
	for(i = 0; i < pprops->num_ta; i++) {

		pta = &(pprops->target_agents[i]);

		strcpy(proc_name, pta->name);
		pta->proc = proc_mkdir(proc_name, pinst->ta_proc);
		if (!pta->proc)
			return -ENOMEM;

		entry = proc_create_data("info", 0, pta->proc,
				&ngi_ta_info_proc_fops, pta);
		if (!entry)
			goto err1;

		entry = proc_create_data("protection_info", 0, pta->proc,
				&ngi_ta_pm_info_proc_fops, pta);
		if (!entry)
			goto err1;

#if 0
		for (j = 0; j < pta->num_pr; j++)
		{
			priv_data = kmalloc (sizeof(struct ngi_private_data), GFP_KERNEL);
			if (!priv_data)
				goto err1;

			strcpy(proc_name, pta->acl_pr[j].pr_name);
			strcat(proc_name, "_write_pm");
			priv_data->pinstance = pinst;
			priv_data->ta_index = i;
			priv_data->acl_index = j;
			entry = proc_create_data(proc_name, 0, pta->proc,
				&ngi_pr_write_pm_proc_fops, priv_data);
			if (!entry)
				goto err1;

			strcpy(proc_name, pta->acl_pr[j].pr_name);
			strcat(proc_name, "_read_pm");
			entry = proc_create_data(proc_name, 0, pta->proc,
				&ngi_pr_read_pm_proc_fops, priv_data);
			if (!entry)
				goto err1;
		}
#endif
	}

	pr_info("created ngi ta proc success .. \n");
	
	/* Create the Init Agents related proc */
	strcpy(proc_name, "init_agents");
	pinst->ia_proc = proc_mkdir(proc_name, pinst->proc);
	if (!pinst->ia_proc)
		return -ENOMEM;

	for(i = 0; i < pinst->props.num_ia; i++) {
		pia = &(pprops->init_agents[i]);

		strcpy(proc_name, pia->name);
		pia->proc = proc_mkdir(proc_name, pinst->ia_proc);
		if (!pia->proc)
			return -ENOMEM;

		entry = proc_create_data("info", 0, pia->proc,
				&ngi_ia_info_proc_fops, pia);
		if (!entry)
			goto err1;
	}
	pr_info("created ngi ia proc success .. \n");

	/* Create the Register Target related proc */
	strcpy(proc_name, "reg_target");
	pinst->rt_proc = proc_mkdir(proc_name, pinst->proc);
	if (!pinst->rt_proc)
		return -ENOMEM;

	prt = &(pprops->rt_agent);

	strcpy(proc_name, prt->name);
	prt->proc = proc_mkdir(proc_name, pinst->rt_proc);
	if (!prt->proc)
		return -ENOMEM;

	entry = proc_create_data("protection_info", 0, prt->proc,
				&ngi_rt_pm_info_proc_fops, prt);
	if (!entry)
		goto err1;

	return 0;
err1:
	remove_proc_entry(proc_name, NULL);
	return -ENOMEM;
}

static int ltq_ngi_enumerate(struct ngi_inst *pinst)
{
	int ia_count = 0, ta_count = 0, ngi_ta_count = 0, ngi_ia_count = 0;
	u32 initial_comp_addr;
	u32 initial_read_pm_addr;
	u32 initial_write_pm_addr;
	u32 initial_addr_match_addr;
	u32 initial_rt_read_pm_addr;
	u32 initial_rt_write_pm_addr;
	u32 comp_code;
	int i, table_index, rt_table_index;
	int temp_count;

	pr_info("%s called..\n", __func__);

	/* Read the RT.component register */
	if (((ltq_ngi_r32(pinst, TREG0_RT_COMPONENT) & TREG0_RT_COMPONENT_CODE_MASK) >> TREG0_RT_COMPONENT_CODE_POS) != 0x6200)
		panic ("not an NGI segment !!\n");

	/* Read the Network, Initid, Network Control */
	pinst->props.network_rev = ltq_ngi_r32(pinst, TREG0_RT_NETWORK + 4) & 0xFFFF; /* upper 32-bits (bit 32~63) */
	pinst->props.network_id = (ltq_ngi_r32(pinst, TREG0_RT_NETWORK + 4) >> 16) & 0xFFFF; /* upper 32-bits (bit 32~63) */

	pinst->props.initid = ltq_ngi_r32(pinst, TREG0_RT_INITID_READBACK) & TREG0_RT_INITID_READBACK_INITID_MASK;
	pinst->props.timeout_base = (ltq_ngi_r32(pinst, TREG0_RT_NETWORK_CONTROL) & TREG0_RT_NETWORK_CONTROL_TIMEOUT_BASE_MASK) >> TREG0_RT_NETWORK_CONTROL_TIMEOUT_BASE_POS;
	pinst->props.clk_gate_disable = (ltq_ngi_r32(pinst, TREG0_RT_NETWORK_CONTROL + 4) & (TREG0_RT_NETWORK_CONTROL_CLOCK_GATE_DISABLE_MASK >> 32) >> (TREG0_RT_NETWORK_CONTROL_CLOCK_GATE_DISABLE_POS - 32)); /* upper 32-bits (bit 32~63) */
	pinst->props.si_ctrl_clk_gate_disable = (ltq_ngi_r32(pinst, TREG0_SI_CONTROL + 4) & (TREG0_SI_CONTROL_CLOCK_GATE_DISABLE_MASK >> 32) >> (TREG0_SI_CONTROL_CLOCK_GATE_DISABLE_POS - 32)); /* upper 32-bits (bit 32~63) */

	/*--------------------------------------------------------------------*/
	/* Enumerate the Register Target protection region */
	/*--------------------------------------------------------------------*/
	initial_rt_read_pm_addr  = TREG0_PM_READ_PERMISSION_0;
	initial_rt_write_pm_addr = TREG0_PM_WRITE_PERMISSION_0;

	/* Register target related stuff */
	rt_table_index = ltq_lookup_rt_index(pinst->inst_id);
	pr_info ("table_index of RT = %d\n", rt_table_index);

	if (rt_table_index < 0) {
		pr_info("%s: couldn't find the RT info from the table \n", __func__);
		return -ENODEV;
	} else {
		/* Change the RT NETWORK Control timeout base to 4 */
		pr_debug("timeout base for TREG: %s BEFORE is : %x \n", pinst->props.rt_agent.name, ltq_ngi_r32(pinst, TREG0_RT_NETWORK_CONTROL));
		ltq_ngi_w32_mask(pinst, 0, 4 << TREG0_RT_NETWORK_CONTROL_TIMEOUT_BASE_POS, TREG0_RT_NETWORK_CONTROL);
		pr_debug("timeout base for TREG: %s AFTER is : %x \n", pinst->props.rt_agent.name, ltq_ngi_r32(pinst, TREG0_RT_NETWORK_CONTROL));
	}

	/* Store the name and other info */
	pinst->props.rt_agent.name		= ngi_rt_core_table[rt_table_index].name;
	pinst->props.rt_agent.pm_read_offset	= initial_rt_read_pm_addr;
	pinst->props.rt_agent.pm_write_offset	= initial_rt_write_pm_addr;
	pinst->props.rt_agent.pinst		= pinst;
	pinst->props.rt_agent.num_pr		= ngi_rt_core_table[rt_table_index].num_pr;

	/* Save the offset in the error info table */
	for (i = 0; i < LTQ_NGI_MAX_ERROR_BITS; i++) {
		if (strcmp(ngi_error_info_flag_status_1_table[pinst->inst_id][i].agent_name, pinst->props.rt_agent.name) == 0)
		ngi_error_info_flag_status_1_table[pinst->inst_id][i].offset = (initial_rt_read_pm_addr - 0x30);
	}

	for (i = 0; i < pinst->props.rt_agent.num_pr; i++)
	{
		pinst->props.rt_agent.rt_pr[i].pr_name		= ngi_rt_core_table[rt_table_index].rt_pr[i].pr_name;
		pinst->props.rt_agent.rt_pr[i].pr_id		= ngi_rt_core_table[rt_table_index].rt_pr[i].pr_id;
		pinst->props.rt_agent.rt_pr[i].base		= ngi_rt_core_table[rt_table_index].rt_pr[i].base;
		pinst->props.rt_agent.rt_pr[i].size		= ngi_rt_core_table[rt_table_index].rt_pr[i].size;
		pinst->props.rt_agent.rt_pr[i].level		= ngi_rt_core_table[rt_table_index].rt_pr[i].level;
		pinst->props.rt_agent.rt_pr[i].group_read_pr	= ngi_rt_core_table[rt_table_index].rt_pr[i].group_read_pr;
		pinst->props.rt_agent.rt_pr[i].group_write_pr	= ngi_rt_core_table[rt_table_index].rt_pr[i].group_write_pr;

#if 0
		if (i > 0) {
			ltq_ngi_w32_mask(pinst, TSSB_PM_ADDR_MATCH_1_LEVEL_MASK, (pinst->props.target_agents[ta_count].acl_pr[i].level) << TSSB_PM_ADDR_MATCH_1_LEVEL_POS, initial_addr_match_addr);
			ltq_ngi_w32_mask(pinst, TSSB_PM_ADDR_MATCH_1_SIZE_MASK, (pinst->props.target_agents[ta_count].acl_pr[i].size) << TSSB_PM_ADDR_MATCH_1_SIZE_POS, initial_addr_match_addr);
		}
#endif

		/* Program the registers with the above values */
		ltq_ngi_w32(pinst, pinst->props.rt_agent.rt_pr[i].group_read_pr, (initial_rt_read_pm_addr + (i*0x20)));
		ltq_ngi_w32(pinst, pinst->props.rt_agent.rt_pr[i].group_write_pr, (initial_rt_write_pm_addr + (i*0x20)));
	}

	/*--------------------------------------------------------------------*/
	/* Enumerate the Target Agents */
	/*--------------------------------------------------------------------*/

	initial_comp_addr = TSSB_TA_COMPONENT;
	/*initial_core_addr = TSSB_TA_CORE; */
	initial_read_pm_addr = TSSB_PM_READ_PERMISSION_0;
	initial_write_pm_addr = TSSB_PM_WRITE_PERMISSION_0;
	initial_addr_match_addr = TSSB_PM_ADDR_MATCH_1;

	if (pinst->inst_id == 0)
		ngi_ta_count = 15;
	else if (pinst->inst_id == 1)
		ngi_ta_count = 4;
	else if (pinst->inst_id == 2)
		ngi_ta_count = 8;
	else if (pinst->inst_id == 3)
		ngi_ta_count = 13;
	else if (pinst->inst_id == 4)
		ngi_ta_count = 16;
	else if (pinst->inst_id == 6)
		ngi_ta_count = 5;
	
	//do {
	for (temp_count = 0; temp_count < ngi_ta_count; temp_count++)
	{
		pr_debug ("reading from offset: %x \n", (unsigned int) initial_comp_addr);
		/* Read the component register */
		comp_code = (ltq_ngi_r32(pinst, initial_comp_addr) & TSSB_TA_COMPONENT_CODE_MASK) >> TSSB_TA_COMPONENT_CODE_POS;
		if (comp_code == 0x6020) {
			/* Read the TA.Core */
			pinst->props.target_agents[ta_count].rev_code = ltq_ngi_r32(pinst, initial_comp_addr + 0x18) & TSSB_TA_CORE_REV_CODE_MASK;
			pinst->props.target_agents[ta_count].core_code =
							(ltq_ngi_r32(pinst, initial_comp_addr + 0x18) & TSSB_TA_CORE_CORE_CODE_MASK) >> TSSB_TA_CORE_CORE_CODE_POS;
			pinst->props.target_agents[ta_count].vendor_code = ltq_ngi_r32(pinst, initial_comp_addr + 0x18 + 4) & (TSSB_TA_CORE_VENDOR_CODE_MASK >> 32); /* upper 32-bits (bit 32~63) */
			
			/* Get the default table index based on the vendor code */
			table_index = ltq_lookup_ta_index(pinst->props.target_agents[ta_count].core_code);
			pr_info ("table_index = %d and core_code = %x \n", table_index, (unsigned int) (pinst->props.target_agents[ta_count].core_code));

			if (table_index < 0) {
				pr_info("%s: couldn't find the TA info from the table \n", __func__);
				continue;
			}

			/* Store the name and other info */
			pinst->props.target_agents[ta_count].name = ngi_ta_core_table[table_index].core_name;
			pinst->props.target_agents[ta_count].offset = initial_comp_addr;
			pinst->props.target_agents[ta_count].pm_read_offset = initial_read_pm_addr;
			pinst->props.target_agents[ta_count].pm_write_offset = initial_write_pm_addr;
			pinst->props.target_agents[ta_count].pinst = pinst;

			/* Initialise the Error table based on the name */
			for (i = 0; i < LTQ_NGI_MAX_ERROR_BITS; i++) {
				if (strcmp(ngi_error_info_flag_status_0_table[pinst->inst_id][i].agent_name, pinst->props.target_agents[ta_count].name) == 0)
				ngi_error_info_flag_status_0_table[pinst->inst_id][i].offset = initial_comp_addr;
			}

			for (i = 0; i < LTQ_NGI_MAX_ERROR_BITS; i++) {
				if (strcmp(ngi_error_info_flag_status_1_table[pinst->inst_id][i].agent_name, pinst->props.target_agents[ta_count].name) == 0)
				ngi_error_info_flag_status_1_table[pinst->inst_id][i].offset = (initial_read_pm_addr - 0x30);
			}

			/* Store all the properties of Protection Region from the table */
			pinst->props.target_agents[ta_count].num_pr = ngi_ta_core_table[table_index].num_pr;
			for (i = 0; i < pinst->props.target_agents[ta_count].num_pr; i++) {

				pinst->props.target_agents[ta_count].acl_pr[i].pr_name = ngi_ta_core_table[table_index].acl_pr[i].pr_name;
				pinst->props.target_agents[ta_count].acl_pr[i].pr_id = ngi_ta_core_table[table_index].acl_pr[i].pr_id;
				pinst->props.target_agents[ta_count].acl_pr[i].base = ngi_ta_core_table[table_index].acl_pr[i].base;
				pinst->props.target_agents[ta_count].acl_pr[i].size = ngi_ta_core_table[table_index].acl_pr[i].size;
				pinst->props.target_agents[ta_count].acl_pr[i].level = ngi_ta_core_table[table_index].acl_pr[i].level;
				pinst->props.target_agents[ta_count].acl_pr[i].group_read_pr = ngi_ta_core_table[table_index].acl_pr[i].group_read_pr;
				pinst->props.target_agents[ta_count].acl_pr[i].group_write_pr = ngi_ta_core_table[table_index].acl_pr[i].group_write_pr;

#if 0
				if (i > 0) {
					ltq_ngi_w32_mask(pinst, TSSB_PM_ADDR_MATCH_1_LEVEL_MASK, (pinst->props.target_agents[ta_count].acl_pr[i].level) << TSSB_PM_ADDR_MATCH_1_LEVEL_POS, initial_addr_match_addr);
					ltq_ngi_w32_mask(pinst, TSSB_PM_ADDR_MATCH_1_SIZE_MASK, (pinst->props.target_agents[ta_count].acl_pr[i].size) << TSSB_PM_ADDR_MATCH_1_SIZE_POS, initial_addr_match_addr);
				}
#endif

				/* Program the registers with the above values */
				ltq_ngi_w32(pinst, pinst->props.target_agents[ta_count].acl_pr[i].group_read_pr, (initial_read_pm_addr + (i*0x20)));
				ltq_ngi_w32(pinst, pinst->props.target_agents[ta_count].acl_pr[i].group_write_pr, (initial_write_pm_addr + (i*0x20)));
			}

			/* Set the default REQ_TIMEOUT to be 4 */
			pr_debug("request timeout for TA: %s BEFORE is : %x \n", 
						pinst->props.target_agents[ta_count].name, ltq_ngi_r32(pinst, initial_comp_addr + 0x20));
#ifndef CONFIG_USE_WAVE600_2_EMULATOR
			ltq_ngi_w32_mask(pinst, 0, 4 << TSSB_TA_AGENT_CONTROL_REQ_TIMEOUT_POS, (initial_comp_addr + 0x20));
#endif
			pr_debug("request timeout for TA: %s AFTER is : %x \n", 
						pinst->props.target_agents[ta_count].name, ltq_ngi_r32(pinst, initial_comp_addr + 0x20));
			ta_count++;
		}
		initial_comp_addr += 0x400;
		initial_read_pm_addr += 0x400;
		initial_write_pm_addr += 0x400;
		initial_addr_match_addr += 0x400;
	}
	/*} while ((pinst->props.target_agents[ta_count - 1].core_code != 0xf06) || (pinst->props.target_agents[ta_count - 1].core_code != 0xf10)); */

	pinst->props.num_ta = ta_count;

	/*--------------------------------------------------------------------*/
	/* Enumerate the Initiator Agents */
	/*--------------------------------------------------------------------*/

	initial_comp_addr = IE97W_IA_COMPONENT;
	if (pinst->inst_id == 0)
		ngi_ia_count = 13;
	else if (pinst->inst_id == 1)
		ngi_ia_count = 7;
	else if (pinst->inst_id == 2)
		ngi_ia_count = 8;
	else if (pinst->inst_id == 3)
		ngi_ia_count = 7;
	else if (pinst->inst_id == 4)
		ngi_ia_count = 1;
	else if (pinst->inst_id == 6)
		ngi_ia_count = 3;

	/*do { */
	for (temp_count = 0; temp_count < ngi_ia_count; temp_count++) {

		/* Read the component register */
		comp_code = (ltq_ngi_r32(pinst, initial_comp_addr) & IE97W_IA_COMPONENT_CODE_MASK) >> IE97W_IA_COMPONENT_CODE_POS;
		if (comp_code == 0x6010) {
			/* Read the IA.Core */
			pinst->props.init_agents[ia_count].rev_code = ltq_ngi_r32(pinst, initial_comp_addr + 0x18) & IE97W_IA_CORE_REV_CODE_MASK;
			pinst->props.init_agents[ia_count].core_code =
							(ltq_ngi_r32(pinst, initial_comp_addr + 0x18) & IE97W_IA_CORE_CORE_CODE_MASK) >>
							IE97W_IA_CORE_CORE_CODE_POS;
			pinst->props.init_agents[ia_count].vendor_code = ltq_ngi_r32(pinst, initial_comp_addr + 0x18 + 4) & (IE97W_IA_CORE_VENDOR_CODE_MASK >> 32); /* upper 32-bits (bit 32~63) */

			/* Lookup the core code and get the name and store */
			pinst->props.init_agents[ia_count].name = ltq_lookup_ia_code(pinst->props.init_agents[ia_count].core_code);
			pinst->props.init_agents[ia_count].offset = initial_comp_addr;
			pr_info ("core name = %s and core_code = %x \n", pinst->props.init_agents[ia_count].name, (unsigned int) (pinst->props.init_agents[ia_count].core_code));

			/* Initialise the Error table based on the name */
			for (i = 0; i < LTQ_NGI_MAX_ERROR_BITS; i++) {
				if (strcmp(ngi_error_info_flag_status_0_table[pinst->inst_id][i].agent_name, pinst->props.init_agents[ia_count].name) == 0)
				ngi_error_info_flag_status_0_table[pinst->inst_id][i].offset = initial_comp_addr;
			}

			for (i = 0; i < LTQ_NGI_MAX_ERROR_BITS; i++) {
				if (strcmp(ngi_error_info_flag_status_1_table[pinst->inst_id][i].agent_name, pinst->props.init_agents[ia_count].name) == 0)
				ngi_error_info_flag_status_1_table[pinst->inst_id][i].offset = initial_comp_addr;
			}

			ia_count++;
			initial_comp_addr += 0x400;
		}
	}
	/*} while ((pinst->props.init_agents[ia_count - 1].core_code != 7) || (pinst->props.init_agents[ia_count - 1].core_code != 0xf10)); */

	pinst->props.num_ia = ia_count; 

	return 0;
}

/* Agent and Core reset related code */
int ltq_ngi_reset_core_agent (struct ngi_agent_core *agent, u32 flags)
{
	struct ngi_inst *pinst = NULL;
	struct ngi_ta *pta = NULL;
	struct ngi_ia *pia = NULL;
	u32 status, ta_reset_count = 0, ia_reset_count = 0;
	struct ngi_ta *pta_temp[10];
	struct ngi_ia *pia_temp[10];
	int i = 0, j = 0;

	if (!agent || (agent->ngi_inst_id > LTQ_NUM_NGI_INSTANCES) || 
			(flags > NGI_F_CORE))
		return -ENODEV;

	pinst = &(ltq_ngi_instance[agent->ngi_inst_id]);
	
	if (flags & NGI_F_IA) {
		pia = ltq_ngi_lookup_init_agent (pinst, agent->core_code, 0);
		if (!pia)
	  		return -ENODEV;

		/* Set the IA.AGENT_CONTROL.REJECT */
		ltq_ngi_w32_mask(pinst, 0, 1 << IE97W_IA_AGENT_CONTROL_REJECT_POS, (pia->offset + 0x20));

		for (i = 0; i < NGI_DEF_WAIT_CYCLES; i++) {
			/* Wait for BURST, READEX, REQ_WAITING and RESP_ACTIVE to be 0 */
			status = ltq_ngi_r32(pinst, (pia->offset + 0x28));

			if ((status & IE97W_IA_AGENT_STATUS_BURST_POS) ||
				   (status & IE97W_IA_AGENT_STATUS_READEX_POS) || 
				   (status & IE97W_IA_AGENT_STATUS_REQ_ACTIVE_POS) || 
				   (status & IE97W_IA_AGENT_STATUS_RESP_WAITING_POS))
				continue;
			else
				break;
		}
		if (i >= NGI_DEF_WAIT_CYCLES) {
			pr_err("waiting for IA to be free failed..\n");
			return -EBUSY;
		}
		/* Now reset the Init Agent */
		ltq_ngi_w32_mask(pinst, 0, 1 << IE97W_IA_AGENT_CONTROL_CORE_RESET_POS, (pia->offset + 0x20));

	} else if (flags & NGI_F_TA) {
		pta = ltq_ngi_lookup_target_agent (pinst, agent->core_code, 0);
		if (!pta)
	  		return -ENODEV;

		/* Set the TA.AGENT_CONTROL.REJECT */
		ltq_ngi_w32_mask(pinst, 0, 1 << TSSB_TA_AGENT_CONTROL_REJECT_POS, (pta->offset + 0x20));

		for (i = 0; i < NGI_DEF_WAIT_CYCLES; i++) {

			/* Wait for BURST, READEX, REQ_WAITING and RESP_ACTIVE to be 0 */
			status = ltq_ngi_r32(pinst, (pta->offset + 0x28));
		
			if ((status & TSSB_TA_AGENT_STATUS_BURST_POS) && 
				(status & TSSB_TA_AGENT_STATUS_READEX_POS) &&
				(status & TSSB_TA_AGENT_STATUS_RESP_ACTIVE_POS) &&
				(status & TSSB_TA_AGENT_STATUS_REQ_WAITING_POS))
				continue;
			else
				break;
		}
		if (i >= NGI_DEF_WAIT_CYCLES) {
			pr_err("waiting for TA to be free failed..\n");
			return -EBUSY;
		}

		/* Now reset the Target Agent */
		ltq_ngi_w32_mask(pinst, 0, 1 << TSSB_TA_AGENT_CONTROL_CORE_RESET_POS, (pta->offset + 0x20));

	} else if (flags & NGI_F_CORE) {
		pr_info ("trying to reset the core with multiple IA and TA\n");

		/* Do the core reset based on sequence of IAs and TAs */

		/* First lookup all the IAs matching the core code */
		/* Compare only the first byte */
		for (i = 0; (i < pinst->props.num_ia); i++) {
			if ((pinst->props.init_agents[i].core_code & 0xff) == (agent->core_code & 0xff)) {
				pia = &(pinst->props.init_agents[i]);

				pr_info("setting the reject and waiting for IA: %s\n", pia->name);

				/* Set the IA.AGENT_CONTROL.REJECT */
				ltq_ngi_w32_mask(pinst, 0, 1 << IE97W_IA_AGENT_CONTROL_REJECT_POS, (pia->offset + 0x20));

				for (j = 0; j < NGI_DEF_WAIT_CYCLES; j++) {
					/* Wait for BURST, READEX, REQ_WAITING and RESP_ACTIVE to be 0 */
					status = ltq_ngi_r32(pinst, (pia->offset + 0x28));

					if ((status & IE97W_IA_AGENT_STATUS_BURST_POS) ||
						(status & IE97W_IA_AGENT_STATUS_READEX_POS) ||
						(status & IE97W_IA_AGENT_STATUS_REQ_ACTIVE_POS) ||
						(status & IE97W_IA_AGENT_STATUS_RESP_WAITING_POS))
						continue;
					else
						break;
				}
				if (j >= NGI_DEF_WAIT_CYCLES) {
					pr_err("waiting for IA to be free failed..\n");
					return -EBUSY;
				}

				pia_temp[ia_reset_count] = pia;
				ia_reset_count++;
			}
		}

		/* Start the TAs now */
		/* Compare only the first byte */
		for (i = 0; (i < pinst->props.num_ta); i++) {
			if ((pinst->props.target_agents[i].core_code & 0xff) == (agent->core_code & 0xff)) {
	
				pta = &(pinst->props.target_agents[i]);

				pr_info("setting the reject and waiting for TA: %s\n", pta->name);
				/* Set the TA.AGENT_CONTROL.REJECT */
				ltq_ngi_w32_mask(pinst, 0, 1 << TSSB_TA_AGENT_CONTROL_REJECT_POS, (pta->offset + 0x20));

				for (j = 0; j < NGI_DEF_WAIT_CYCLES; j++) {
					/* Wait for BURST, READEX, REQ_WAITING and RESP_ACTIVE to be 0 */
					status = ltq_ngi_r32(pinst, (pia->offset + 0x28));

					if ((status & TSSB_TA_AGENT_STATUS_BURST_POS) ||
						(status & TSSB_TA_AGENT_STATUS_READEX_POS) ||
						(status & TSSB_TA_AGENT_STATUS_RESP_ACTIVE_POS) || 
						(status & TSSB_TA_AGENT_STATUS_REQ_WAITING_POS))
						continue;
					else
						break;
				}

				if (j >= NGI_DEF_WAIT_CYCLES) {
					pr_err("waiting for TA to be free failed..\n");
					return -EBUSY;
				}
				pta_temp[ta_reset_count] = pta;
				ta_reset_count++;
			}
		}

		/* Now reset the TAs first */	
		for (i = 0; i < ta_reset_count; i++) {
			pr_info("resetting the TA: %s\n", pta_temp[i]->name);
			/* Now reset the Target Agent */
			ltq_ngi_w32_mask(pinst, 0, 1 << TSSB_TA_AGENT_CONTROL_CORE_RESET_POS, 
						(pta_temp[i]->offset + 0x20));
		}

		/* Now reset the IAs first */	
		for (i = 0; i < ia_reset_count; i++) {
			pr_info("resetting the IA: %s\n", pia_temp[i]->name);
			/* Now reset the Init Agent */
			ltq_ngi_w32_mask(pinst, 0, 1 << IE97W_IA_AGENT_CONTROL_CORE_RESET_POS,
						(pia_temp[i]->offset + 0x20));
		}

		/* clear reject then core_reset on IAs first */
		for (i = 0; i < ia_reset_count; i++) {
			pr_info("clear reject, core_reset IA: %s\n", pia_temp[i]->name);
			/* clear reject of Init Agent */
			ltq_ngi_w32_mask(pinst, 1 << IE97W_IA_AGENT_CONTROL_REJECT_POS, 0,
					(pia_temp[i]->offset + 0x20));
			/* clear core_rest of Init Agent */
			ltq_ngi_w32_mask(pinst, 1 << IE97W_IA_AGENT_CONTROL_CORE_RESET_POS, 0,
					(pia_temp[i]->offset + 0x20));
		}

		/* clear reject then core_reset on TAs */
		for (i = 0; i < ta_reset_count; i++) {
			pr_info("clear reject, core_reset TA: %s\n", pta_temp[i]->name);
			/* clear reject of Target Agent */
			ltq_ngi_w32_mask(pinst, 1 << TSSB_TA_AGENT_CONTROL_REJECT_POS, 0,
						(pta_temp[i]->offset + 0x20));
			/* clear core_reset of Target Agent */
			ltq_ngi_w32_mask(pinst, 1 << TSSB_TA_AGENT_CONTROL_CORE_RESET_POS, 0,
						(pta_temp[i]->offset + 0x20));
		}
	} else {
		pr_err("%s: incorrect flags\n", __func__);
		return -ENODEV;
	}
	return 0;
}
EXPORT_SYMBOL(ltq_ngi_reset_core_agent);

static int ltq_ngi_probe(struct platform_device *pdev)
{
	struct device_node *node = pdev->dev.of_node;
	struct resource *memres;
	static int inst_id;
	struct ngi_inst *pinstance;
#ifdef NGI_ERROR_HANDLING
	struct resource irqres;
	int ret = 0;
	struct ngi_agent_core temp_agent;
#endif

	pr_info("%s called..\n", __func__);

	/* DMA controller idx */
	if (!of_property_read_u32(node, "lantiq,ngi-inst-id", &inst_id))
		pdev->id = inst_id;
	else
		panic("Failed to get NGI instance id\n");

	if (pdev->id < 0 || pdev->id >= LTQ_NUM_NGI_INSTANCES)
		return -EINVAL;

	pinstance = &ltq_ngi_instance[pdev->id];

	memset(pinstance, 0, sizeof(*pinstance));

	pinstance->inst_id = pdev->id;
	pinstance->name = ngi_name[pinstance->inst_id];

	/* Link controller to platform device */
	pinstance->dev = &pdev->dev;
	memres = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!memres)
		panic("Failed to get NGI resource");

	/* remap dma register range */
	pinstance->membase = devm_ioremap_resource(&pdev->dev, memres);

	if (IS_ERR(pinstance->membase))
		panic("Failed to remap NGI resource");

	pr_info("NGI membase: %x \n", (unsigned int) (pinstance->membase));

#ifdef NGI_ERROR_HANDLING
	if ((inst_id == LTQ_NGI_ROOT_INST1)) {
		ret = of_irq_to_resource_table(node, &irqres, 1);
		if (ret != 1) {
			#if (LINUX_VERSION_CODE > KERNEL_VERSION(3,6,0))
			/*
			 * Due to Workaround to avoid breaking interrupt controller drivers
			 * that don't yet register an irq_domain.  This is temporary
			 * code. ~~~gcl, Feb 24, 2012
			 *
			 * Scheduled for removal in Linux v3.6.  That should be enough
			 * time.
			 */
			/* well since above workaround in irq_create_of_mapping is removed
			 * in kernel 4.9, there is no way to read irq resource from device tree
			 * unless we port the irq controller driver to irq domain subsystem,
			 * i will just hardcoded it for now
			 */
			 irqres.start = GRX500_BOOTCORE_NGIERROR_IRQ_OUT_INDEX;
			#else
			dev_err(&pdev->dev,
					"failed to get irq for NGI");
			return -ENODEV;
			#endif
		}
		pr_info("irq resource: %d \n", irqres.start);
		#ifdef BOOTCORE_LEGACY_IRQ
		grx500_bootcore_register_static_irq(GRX500_BOOTCORE_NGIERROR_IRQ_IN_INDEX,
							irqres.start,
							&ngi_irqaction,
							grx500_bootcore_ngi_irq);
		#else /* BOOTCORE_LEGACY_IRQ */
		ret = request_irq(irqres.start, ngi_isr, 0, "ngi", pinstance);
		if (ret) {
			pr_err("failed to request irq - %d still continuing though !!", ret);
			/* return ret; */
		}
		#endif /* BOOTCORE_LEGACY_IRQ */
		tasklet_init(&ngi_tasklet,
			ngi_err_handling_tasklet, 0);
	}
#endif

	/* Enumerate the bus */
	ltq_ngi_enumerate(pinstance);

	/* Init the procfs for this instance */
	ngi_proc_init(pinstance);

#if 0
#ifdef NGI_ERROR_HANDLING
	/* Dummy schedule */
	if ((inst_id == LTQ_NGI_ROOT_INST1)) {
		tasklet_schedule(&ngi_tasklet);
	}
#endif
#endif
	/* Dummy reset */
	if ((inst_id == LTQ_NGI_ROOT_INST1)) {
		/* PCIE3 reset */
		temp_agent.ngi_inst_id = 3;
		temp_agent.core_code = 0x3a;
		ltq_ngi_reset_core_agent (&temp_agent, NGI_F_CORE);
	}
	return 0;
}

static const struct of_device_id ltq_ngi_match[] = {
	{ .compatible = "lantiq,ngi-xrx500" },
	{},
};
MODULE_DEVICE_TABLE(of, ltq_xrx500_match);

static struct platform_driver ltq_ngi_driver = {
	.probe = ltq_ngi_probe,
	.driver = {
		.name = "ngi-xrx500",
		.owner = THIS_MODULE,
		.of_match_table = ltq_ngi_match,
	},
};

int __init ltq_ngi_init(void)
{
	int ret;

	printk("%s: called..\n", __func__);
	ret = platform_driver_register(&ltq_ngi_driver);
	if (ret)
		pr_info("ngi-xrx500: Error registering platform driver!");
	return ret;
}

core_initcall(ltq_ngi_init);
