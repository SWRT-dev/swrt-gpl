// SPDX-License-Identifier: GPL-2.0
/*
 * Author:
 *	Chuanhong Guo <gch981213@gmail.com>
 *	Zhaowei Xu <paldier@hotmail.com>
 */

#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/mtd/spinand.h>

/* ESMT uses GigaDevice 0xc8 JECDEC ID on some SPI NANDs */
#define SPINAND_MFR_ESMT_C8			0xc8

static SPINAND_OP_VARIANTS(read_cache_variants,
			   SPINAND_PAGE_READ_FROM_CACHE_QUADIO_OP(0, 2, NULL, 0),
			   SPINAND_PAGE_READ_FROM_CACHE_X4_OP(0, 1, NULL, 0),
			   SPINAND_PAGE_READ_FROM_CACHE_DUALIO_OP(0, 1, NULL, 0),
			   SPINAND_PAGE_READ_FROM_CACHE_X2_OP(0, 1, NULL, 0),
			   SPINAND_PAGE_READ_FROM_CACHE_OP(true, 0, 1, NULL, 0),
			   SPINAND_PAGE_READ_FROM_CACHE_OP(false, 0, 1, NULL, 0));

static SPINAND_OP_VARIANTS(write_cache_variants,
			   SPINAND_PROG_LOAD_X4(true, 0, NULL, 0),
			   SPINAND_PROG_LOAD(true, 0, NULL, 0));

static SPINAND_OP_VARIANTS(update_cache_variants,
			   SPINAND_PROG_LOAD_X4(false, 0, NULL, 0),
			   SPINAND_PROG_LOAD(false, 0, NULL, 0));

static int f50l1g41lb_ooblayout_ecc(struct mtd_info *mtd, int section,
				    struct mtd_oob_region *region)
{
	if (section > 3)
		return -ERANGE;

	region->offset = 16 * section + 8;
	region->length = 8;

	return 0;
}

static int f50l1g41lb_ooblayout_free(struct mtd_info *mtd, int section,
				     struct mtd_oob_region *region)
{
	if (section > 3)
		return -ERANGE;

	region->offset = 16 * section + 2;
	region->length = 6;

	return 0;
}

static const struct mtd_ooblayout_ops f50l1g41lb_ooblayout = {
	.ecc = f50l1g41lb_ooblayout_ecc,
	.free = f50l1g41lb_ooblayout_free,
};

static const struct spinand_info esmt_c8_spinand_table[] = {
	SPINAND_INFO("F50L1G41LB", 0x01,
		     NAND_MEMORG(1, 2048, 64, 64, 1024, 20, 1, 1, 1),
		     NAND_ECCREQ(1, 512),
		     SPINAND_INFO_OP_VARIANTS(&read_cache_variants,
					      &write_cache_variants,
					      &update_cache_variants),
		     0,
		     SPINAND_ECCINFO(&f50l1g41lb_ooblayout, NULL)),
	SPINAND_INFO("F50D1G41LB", 0x11,
		     NAND_MEMORG(1, 2048, 64, 64, 1024, 20, 1, 1, 1),
		     NAND_ECCREQ(1, 512),
		     SPINAND_INFO_OP_VARIANTS(&read_cache_variants,
					      &write_cache_variants,
					      &update_cache_variants),
		     0,
		     SPINAND_ECCINFO(&f50l1g41lb_ooblayout, NULL)),
};

static int esmt_spinand_detect(struct spinand_device *spinand)
{
	u8 *id = spinand->id.data;
	u16 did;
	int ret;

	if (id[0] == SPINAND_MFR_ESMT_C8)
		did = (id[1] << 8) + id[2];
	else if (id[0] == 0 && id[1] == SPINAND_MFR_ESMT_C8)
		did = id[2];
	else
		return 0;
	ret = spinand_match_and_init(spinand, esmt_c8_spinand_table,
				     ARRAY_SIZE(esmt_c8_spinand_table),
				     did);
	if (ret)
		return ret;

	return 1;
}

static const struct spinand_manufacturer_ops esmt_spinand_manuf_ops = {
	.detect = esmt_spinand_detect,
};

const struct spinand_manufacturer esmt_c8_spinand_manufacturer = {
	.id = SPINAND_MFR_ESMT_C8,
	.name = "ESMT",
	.ops = &esmt_spinand_manuf_ops,
};
