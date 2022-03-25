/*
 * Copyright (c) 2013, Google Inc.
 *
 * Copyright (C) 2011
 * Corscience GmbH & Co. KG - Simon Schwarz <schwarz@corscience.de>
 *  - Added prep subcommand support
 *  - Reorganized source - modeled after powerpc version
 *
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 *
 * Copyright (C) 2001  Erik Mouw (J.A.K.Mouw@its.tudelft.nl)
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <fdt_support.h>

DECLARE_GLOBAL_DATA_PTR;

int arch_fixup_memory_node(void *blob)
{
	bd_t *bd = gd->bd;
	int bank;
	u64 start[CONFIG_NR_DRAM_BANKS];
	u64 size[CONFIG_NR_DRAM_BANKS];

	for (bank = 0; bank < CONFIG_NR_DRAM_BANKS; bank++) {
		start[bank] = bd->bi_dram[bank].start;
	#if (defined (MT7622_ASIC_BOARD)) || (defined (MT7622_FPGA_BOARD)) //mtk40224  return 16M dram to kernel , dram_init() furnction borrow 16M in MT7622_evb.c
		size[bank] = bd->bi_dram[bank].size+SZ_16M;
	#else
		size[bank] = bd->bi_dram[bank].size;
	#endif
	}

	return fdt_fixup_memory_banks(blob, start, size, CONFIG_NR_DRAM_BANKS);
}
