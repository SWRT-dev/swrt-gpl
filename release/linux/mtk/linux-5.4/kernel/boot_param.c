/* SPDX-License-Identifier: BSD-3-Clause */
/*
* Copyright (C) 2022 MediaTek Inc. All rights reserved.
 *
 * Author: Weijie Gao <weijie.gao@mediatek.com>
 */

#include <linux/kernel.h>
#include <linux/moduleparam.h>

#define BOOT_PARAM_STR_MAX_LEN			256

bool dual_boot;
module_param(dual_boot, bool, 0444);

static bool reset_boot_count;
module_param(reset_boot_count, bool, 0444);

static bool no_split_rootfs_data;
module_param(no_split_rootfs_data, bool, 0444);

static bool reserve_rootfs_data;
module_param(reserve_rootfs_data, bool, 0444);

static uint boot_image_slot;
module_param(boot_image_slot, uint, 0444);

static uint upgrade_image_slot;
module_param(upgrade_image_slot, uint, 0444);

static char rootfs_data_part[BOOT_PARAM_STR_MAX_LEN];
module_param_string(rootfs_data_part, rootfs_data_part, BOOT_PARAM_STR_MAX_LEN, 0644);

static char boot_kernel_part[BOOT_PARAM_STR_MAX_LEN];
module_param_string(boot_kernel_part, boot_kernel_part, BOOT_PARAM_STR_MAX_LEN, 0444);

static char boot_rootfs_part[BOOT_PARAM_STR_MAX_LEN];
module_param_string(boot_rootfs_part, boot_rootfs_part, BOOT_PARAM_STR_MAX_LEN, 0444);

static char upgrade_kernel_part[BOOT_PARAM_STR_MAX_LEN];
module_param_string(upgrade_kernel_part, upgrade_kernel_part, BOOT_PARAM_STR_MAX_LEN, 0444);

static char upgrade_rootfs_part[BOOT_PARAM_STR_MAX_LEN];
module_param_string(upgrade_rootfs_part, upgrade_rootfs_part, BOOT_PARAM_STR_MAX_LEN, 0444);

static char env_part[BOOT_PARAM_STR_MAX_LEN];
module_param_string(env_part, env_part, BOOT_PARAM_STR_MAX_LEN, 0444);
