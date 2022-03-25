/*
 * Copyright 2010-2018 MediaTek, Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * Version 2 as published by the Free Software Foundation.
 */

#include <common.h>
#include <config.h>

#include <asm/arch/wdt.h>

#define RESET_TIMEOUT 10

void reset_cpu(ulong addr)
{
    mtk_arch_reset(0);
}

