/*
 * DirectConnect provides a common interface for the network devices to achieve the full or partial 
   acceleration services from the underlying packet acceleration engine
 * Copyright (c) 2017, Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 */

/* Includes */
#include <linux/module.h>

static uint32_t g_dc_dp_dbg_flag = 0x1;

uint32_t
dc_dp_get_dbg_flag(void)
{
    return g_dc_dp_dbg_flag;
}
EXPORT_SYMBOL(dc_dp_get_dbg_flag);

uint32_t
dc_dp_set_dbg_flag(uint32_t dbg_flag)
{
    g_dc_dp_dbg_flag |= dbg_flag;
    return 0;
}
EXPORT_SYMBOL(dc_dp_set_dbg_flag);
