/*
 * Copyright 2011-2012 Freescale Semiconductor, Inc.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <asm/fsl_law.h>
#include <asm/mmu.h>

struct law_entry law_table[] = {
	SET_LAW(CONFIG_SYS_FLASH_BASE_PHYS, LAW_SIZE_256M, LAW_TRGT_IF_IFC),
#ifdef CONFIG_SYS_BMAN_MEM_PHYS
	SET_LAW(CONFIG_SYS_BMAN_MEM_PHYS, LAW_SIZE_32M, LAW_TRGT_IF_BMAN),
#endif
#ifdef CONFIG_SYS_QMAN_MEM_PHYS
	SET_LAW(CONFIG_SYS_QMAN_MEM_PHYS, LAW_SIZE_32M, LAW_TRGT_IF_QMAN),
#endif
	SET_LAW(QIXIS_BASE_PHYS, LAW_SIZE_4K, LAW_TRGT_IF_IFC),
#ifdef CONFIG_SYS_MAPLE_MEM_PHYS
	SET_LAW(CONFIG_SYS_MAPLE_MEM_PHYS, LAW_SIZE_16M, LAW_TRGT_IF_MAPLE),
#endif
#ifdef CONFIG_SYS_DCSRBAR_PHYS
	/* Limit DCSR to 32M to access NPC Trace Buffer */
	SET_LAW(CONFIG_SYS_DCSRBAR_PHYS, LAW_SIZE_32M, LAW_TRGT_IF_DCSR),
#endif
#ifdef CONFIG_SYS_NAND_BASE_PHYS
	SET_LAW(CONFIG_SYS_NAND_BASE_PHYS, LAW_SIZE_64K, LAW_TRGT_IF_IFC),
#endif
};

int num_law_entries = ARRAY_SIZE(law_table);
