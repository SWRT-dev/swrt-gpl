// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 Etron Technology, Inc.
 *
 */

#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/mtd/spinand.h>

#define SPINAND_MFR_ETRON		0xD5

#define STATUS_ECC_LIMIT_BITFLIPS (3 << 4)

static SPINAND_OP_VARIANTS(read_cache_variants,
		SPINAND_PAGE_READ_FROM_CACHE_QUADIO_OP(0, 1, NULL, 0),
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

static int etron_ooblayout_ecc(struct mtd_info *mtd, int section,
				  struct mtd_oob_region *region)
{
	if (section > 3)
		return -ERANGE;

	region->offset = (14 * section) + 72;
	region->length = 14;

	return 0;
}

static int etron_ooblayout_free(struct mtd_info *mtd, int section,
				   struct mtd_oob_region *region)
{
	if (section > 3)
		return -ERANGE;

	if (section) {
		region->offset = 18 * section;
		region->length = 18;
	} else {
		/* section 0 has one byte reserved for bad block mark */
		region->offset = 2;
		region->length = 16;
	}

	return 0;
}

static const struct mtd_ooblayout_ops etron_ooblayout = {
	.ecc = etron_ooblayout_ecc,
	.free = etron_ooblayout_free,
};

static int etron_ecc_get_status(struct spinand_device *spinand,
				   u8 status)
{
	struct nand_device *nand = spinand_to_nand(spinand);

	switch (status & STATUS_ECC_MASK) {
	case STATUS_ECC_NO_BITFLIPS:
		return 0;

	case STATUS_ECC_UNCOR_ERROR:
		return -EBADMSG;

	case STATUS_ECC_HAS_BITFLIPS:
		return nand->eccreq.strength >> 1;

	case STATUS_ECC_LIMIT_BITFLIPS:
		return nand->eccreq.strength;

	default:
		break;
	}

	return -EINVAL;
}

static const struct spinand_info etron_spinand_table[] = {
	/* EM73C 1Gb 3.3V */
	SPINAND_INFO("EM73C044VCF",
		     SPINAND_ID(SPINAND_READID_METHOD_OPCODE_ADDR, 0x25),
		     NAND_MEMORG(1, 2048, 64, 64, 1024, 20, 1, 1, 1),
		     NAND_ECCREQ(4, 512),
		     SPINAND_INFO_OP_VARIANTS(&read_cache_variants,
					      &write_cache_variants,
					      &update_cache_variants),
		     SPINAND_HAS_QE_BIT,
		     SPINAND_ECCINFO(&etron_ooblayout, etron_ecc_get_status)),
	/* EM7xD 2Gb */
	SPINAND_INFO("EM73D044VCR",
		     SPINAND_ID(SPINAND_READID_METHOD_OPCODE_ADDR, 0x41),
		     NAND_MEMORG(1, 2048, 64, 64, 2048, 40, 1, 1, 1),
		     NAND_ECCREQ(4, 512),
		     SPINAND_INFO_OP_VARIANTS(&read_cache_variants,
					      &write_cache_variants,
					      &update_cache_variants),
		     SPINAND_HAS_QE_BIT,
		     SPINAND_ECCINFO(&etron_ooblayout, etron_ecc_get_status)),
	SPINAND_INFO("EM73D044VCO",
		     SPINAND_ID(SPINAND_READID_METHOD_OPCODE_ADDR, 0x3A),
		     NAND_MEMORG(1, 2048, 128, 64, 2048, 40, 1, 1, 1),
		     NAND_ECCREQ(8, 512),
		     SPINAND_INFO_OP_VARIANTS(&read_cache_variants,
					      &write_cache_variants,
					      &update_cache_variants),
		     SPINAND_HAS_QE_BIT,
		     SPINAND_ECCINFO(&etron_ooblayout, etron_ecc_get_status)),
	SPINAND_INFO("EM78D044VCM",
		     SPINAND_ID(SPINAND_READID_METHOD_OPCODE_ADDR, 0x8E),
		     NAND_MEMORG(1, 2048, 128, 64, 2048, 40, 1, 1, 1),
		     NAND_ECCREQ(8, 512),
		     SPINAND_INFO_OP_VARIANTS(&read_cache_variants,
					      &write_cache_variants,
					      &update_cache_variants),
		     SPINAND_HAS_QE_BIT,
		     SPINAND_ECCINFO(&etron_ooblayout, etron_ecc_get_status)),
	/* EM7xE 4Gb */
	SPINAND_INFO("EM73E044VCE",
		     SPINAND_ID(SPINAND_READID_METHOD_OPCODE_ADDR, 0x3B),
		     NAND_MEMORG(1, 2048, 128, 64, 4096, 80, 1, 1, 1),
		     NAND_ECCREQ(8, 512),
		     SPINAND_INFO_OP_VARIANTS(&read_cache_variants,
					      &write_cache_variants,
					      &update_cache_variants),
		     SPINAND_HAS_QE_BIT,
		     SPINAND_ECCINFO(&etron_ooblayout, etron_ecc_get_status)),
	SPINAND_INFO("EM78E044VCD",
		     SPINAND_ID(SPINAND_READID_METHOD_OPCODE_ADDR, 0x8F),
		     NAND_MEMORG(1, 2048, 128, 64, 4096, 80, 1, 1, 1),
		     NAND_ECCREQ(8, 512),
		     SPINAND_INFO_OP_VARIANTS(&read_cache_variants,
					      &write_cache_variants,
					      &update_cache_variants),
		     SPINAND_HAS_QE_BIT,
		     SPINAND_ECCINFO(&etron_ooblayout, etron_ecc_get_status)),
	/* EM7xF044VCA 8Gb */
	SPINAND_INFO("EM73F044VCA",
		     SPINAND_ID(SPINAND_READID_METHOD_OPCODE_DUMMY, 0x15),
		     NAND_MEMORG(1, 4096, 256, 64, 4096, 80, 1, 1, 1),
		     NAND_ECCREQ(8, 512),
		     SPINAND_INFO_OP_VARIANTS(&read_cache_variants,
					      &write_cache_variants,
					      &update_cache_variants),
		     SPINAND_HAS_QE_BIT,
		     SPINAND_ECCINFO(&etron_ooblayout, etron_ecc_get_status)),
	SPINAND_INFO("EM78F044VCA",
		     SPINAND_ID(SPINAND_READID_METHOD_OPCODE_DUMMY, 0x8D),
		     NAND_MEMORG(1, 4096, 256, 64, 4096, 80, 1, 1, 1),
		     NAND_ECCREQ(8, 512),
		     SPINAND_INFO_OP_VARIANTS(&read_cache_variants,
					      &write_cache_variants,
					      &update_cache_variants),
		     SPINAND_HAS_QE_BIT,
		     SPINAND_ECCINFO(&etron_ooblayout, etron_ecc_get_status)),
};

static const struct spinand_manufacturer_ops etron_spinand_manuf_ops = {
};

const struct spinand_manufacturer etron_spinand_manufacturer = {
	.id = SPINAND_MFR_ETRON,
	.name = "Etron",
	.chips = etron_spinand_table,
	.nchips = ARRAY_SIZE(etron_spinand_table),
	.ops = &etron_spinand_manuf_ops,
};

