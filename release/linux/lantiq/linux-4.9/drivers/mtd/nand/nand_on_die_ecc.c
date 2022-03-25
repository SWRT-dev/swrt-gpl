/*
 * This file provides functions for On-die ECC NAND flash support.
 * This is tested on both Toshiba BENAND and MXIC ECC-Free NAND flashes
 *
 * (C) Copyright TOSHIBA CORPORATION 2013
 * All Rights Reserved.
 *
 * (C) Copyright Intel Corporation  2017
 * All Rights Reserved.
 * Author: Brandon.yang@intel.com
 * Authir: m.aliasthani@intel.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/bitops.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/nand.h>
#include <linux/mtd/nand_ecc_on_die.h>

/* The ECC Bits Status for MXIC_MX30LF1GE8AB */
#define NAND_ECC_STATUS_0_1_BIT_ERR	0x00
#define NAND_ECC_STATUS_2_BIT_ERR	0x10
#define NAND_ECC_STATUS_3_BIT_ERR	0x08
#define NAND_ECC_STATUS_4_BIT_ERR	0x18

/* Recommended to rewrite for BENAND */
#define NAND_STATUS_RECOM_REWRT	0x08
/* ECC Status Read Command */
#define NAND_CMD_ECC_STATUS	0x7A

static int nand_benand_status_chk(struct mtd_info *mtd, u8 *data,
				u8 *rd_ecc, u8 *cal_ecc)
{
	unsigned int max_bitflips = 0;
	struct nand_chip *chip = mtd_to_nand(mtd);
	u8 status;

	/* Check Read Status */
	chip->cmdfunc(mtd, NAND_CMD_STATUS, -1, -1);
	status = chip->read_byte(mtd);

	/* timeout */
	if (!(status & NAND_STATUS_READY)) {
		pr_debug("BENAND : Time Out!\n");
		return -EIO;
	}

	/* uncorrectable */
	else if (status & NAND_STATUS_FAIL)
		mtd->ecc_stats.failed++;

	/* correctable */
	else if (status & NAND_STATUS_RECOM_REWRT) {
		if (chip->cmd_ctrl &&
			IS_ENABLED(CONFIG_MTD_NAND_BENAND_ECC_STATUS)) {

			int i;
			u8 ecc_status;
			unsigned int bitflips;

			/* Check Read ECC Status */
			chip->cmd_ctrl(mtd, NAND_CMD_ECC_STATUS,
				NAND_NCE | NAND_CLE | NAND_CTRL_CHANGE);
			/* Get bitflips info per 512Byte */
			for (i = 0; i < mtd->writesize >> 9; i++) {
				ecc_status = chip->read_byte(mtd);
				bitflips = ecc_status & 0x0f;
				max_bitflips = max_t(unsigned int,
						max_bitflips, bitflips);
			}
			mtd->ecc_stats.corrected += max_bitflips;
		} else {
			/*
			 * If can't use chip->cmd_ctrl,
			 * we can't get real number of bitflips.
			 * So, we set max_bitflips mtd->bitflip_threshold.
			 */
			max_bitflips = mtd->bitflip_threshold;
			mtd->ecc_stats.corrected += max_bitflips;
		}
	}

	return max_bitflips;

}

static int nand_mxic_status_chk(struct mtd_info *mtd, u8 *data,
				u8 *rd_ecc, u8 *cal_ecc)
{
	unsigned int max_bitflips = 0;
	struct nand_chip *chip = mtd_to_nand(mtd);
	u8 status;

	/* Check Read Status */
	chip->cmdfunc(mtd, NAND_CMD_STATUS, -1, -1);
	status = chip->read_byte(mtd);

	/* timeout */
	if (!(status & NAND_STATUS_READY)) {
		pr_debug("BENAND : Time Out!\n");
		return -EIO;
	}

	/* uncorrectable */
	else if (status & NAND_STATUS_FAIL)
		mtd->ecc_stats.failed++;

	/* correctable: MXIC recommends to rewrite when
	 * 3~4 bits error been corrected
	 */
	else if (status & NAND_ECC_STATUS_3_BIT_ERR) {
		max_bitflips = 3;
		mtd->ecc_stats.corrected += max_bitflips;
	} else if ((status & NAND_ECC_STATUS_4_BIT_ERR) ==
			NAND_ECC_STATUS_4_BIT_ERR) {
		max_bitflips = 4;
		mtd->ecc_stats.corrected += max_bitflips;
	}

	return max_bitflips;
}

/**
 * nand_read_page_raw - [Intern] read raw page data with mxic.
 * @mtd: mtd info structure
 * @chip: nand chip info structure
 * @buf: buffer to store read data
 * @oob_required: caller requires OOB data read to chip->oob_poi
 * @page: page number to read
 *
 * Not for syndrome calculating ECC controllers,
 * which use a special oob layout.
 */

static int nand_read_page_ecc_on_die(struct mtd_info *mtd,
					struct nand_chip *chip, u8 *buf,
					int oob_required, int page)
{
	unsigned int max_bitflips = 0;

	chip->ecc.read_page_raw(mtd, chip, buf, oob_required, page);

	/* Check Read Status */
	max_bitflips = chip->ecc.correct(mtd, NULL, NULL, NULL);

	return max_bitflips;
}

static int nand_read_subpage_ecc_on_die(struct mtd_info *mtd,
				struct nand_chip *chip, u32 data_offs,
				u32 readlen, u8 *bufpoi, int page)
{
	u8 *p;
	unsigned int max_bitflips = 0;

	if (data_offs != 0)
		chip->cmdfunc(mtd, NAND_CMD_RNDOUT, data_offs, -1);

	p = bufpoi + data_offs;
	chip->read_buf(mtd, p, readlen);

	max_bitflips = chip->ecc.correct(mtd, NULL, NULL, NULL);

	return max_bitflips;
}

void nand_on_die_ecc_init(struct mtd_info *mtd, u8 id)
{
	struct nand_chip *chip = mtd_to_nand(mtd);

	if (id == NAND_MFR_TOSHIBA)
		chip->ecc.correct = nand_benand_status_chk;
	else if (id == NAND_MFR_MACRONIX)
		chip->ecc.correct = nand_mxic_status_chk;
	else
		WARN(1, "This flash is not supported in this mode!\n");

	chip->options |= NAND_SUBPAGE_READ;
	chip->ecc.read_page = nand_read_page_ecc_on_die;
	chip->ecc.read_subpage = nand_read_subpage_ecc_on_die;

	return;

}
EXPORT_SYMBOL(nand_on_die_ecc_init);

MODULE_AUTHOR("INTEL Corporation");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("On-die ECC NAND Flash support");
