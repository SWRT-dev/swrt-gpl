// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2023 MediaTek Inc.
 *
 * Author: Weijie Gao <weijie.gao@mediatek.com>
 */

#include <linux/arm-smccc.h>
#include <linux/printk.h>

/*
 * MTK_SIP_EMERG_MEM_DUMP - Do emergency memory dump thru. ethernet
 *
 * parameters
 * @x1:		reboot after memory dump
 *
 * no return
 */
#define MTK_SIP_EMERG_MEM_DUMP			0xC2000540

void arm64_atf_memory_dump(int reboot_timeout)
{
	struct arm_smccc_res res;

	pr_emerg("Starting Memory dump SMCC\n");

	arm_smccc_smc(MTK_SIP_EMERG_MEM_DUMP, reboot_timeout,
		      0, 0, 0, 0, 0, 0, &res);

	pr_err("Memory dump SMCC failed\n");
}
