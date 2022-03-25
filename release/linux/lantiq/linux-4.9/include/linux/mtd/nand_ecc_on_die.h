/*
 * (C) Copyright TOSHIBA CORPORATION 2013
 * All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This file is the header for the NAND BENAND implementation.
 */

#ifndef __MTD_NAND_ON_DIE_ECC_H__
#define __MTD_NAND_ON_DIE_ECC_H__

#if defined(CONFIG_MTD_NAND_ON_DIE_ECC)

/* init on-die-ecc nand_chip specific functions */
void nand_on_die_ecc_init(struct mtd_info *mtd, u8 id);

#else

static inline void nand_on_die_ecc_init(struct mtd_info *mtd,
					u8 id) {}

#endif /* CONFIG_MTD_NAND_ON_DIE_ECC */
#endif /* __MTD_NAND_ON_DIE_ECC_H__ */
