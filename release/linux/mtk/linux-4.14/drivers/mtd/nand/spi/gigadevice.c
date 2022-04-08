// SPDX-License-Identifier: GPL-2.0
/*
 * Author:
 *	Chuanhong Guo <gch981213@gmail.com>
 */

#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/mtd/spinand.h>

#define SPINAND_MFR_GIGADEVICE			0xC8

#define GD5FXGQ4XA_STATUS_ECC_1_7_BITFLIPS	(1 << 4)
#define GD5FXGQ4XA_STATUS_ECC_8_BITFLIPS	(3 << 4)

#define GD5FXGQ4UEXXG_REG_STATUS2		0xf0

#define GD5FXGQ4UXFXXG_STATUS_ECC_MASK		(7 << 4)
#define GD5FXGQ4UXFXXG_STATUS_ECC_NO_BITFLIPS	(0 << 4)
#define GD5FXGQ4UXFXXG_STATUS_ECC_1_3_BITFLIPS	(1 << 4)
#define GD5FXGQ4UXFXXG_STATUS_ECC_UNCOR_ERROR	(7 << 4)

#define SPINAND_PAGE_READ_FROM_CACHE_OP_NOR_EMU(fast, addr, ndummy, buf, len)	\
	SPI_MEM_OP(SPI_MEM_OP_CMD(fast ? 0x0b : 0x03, 1),		\
		   SPI_MEM_OP_ADDR(3, addr, 1),				\
		   SPI_MEM_OP_DUMMY(ndummy, 1),				\
		   SPI_MEM_OP_DATA_IN(len, buf, 1))

static SPINAND_OP_VARIANTS(read_cache_variants,
		SPINAND_PAGE_READ_FROM_CACHE_QUADIO_OP(0, 1, NULL, 0),
		SPINAND_PAGE_READ_FROM_CACHE_X4_OP(0, 1, NULL, 0),
		SPINAND_PAGE_READ_FROM_CACHE_DUALIO_OP(0, 1, NULL, 0),
		SPINAND_PAGE_READ_FROM_CACHE_X2_OP(0, 1, NULL, 0),
		SPINAND_PAGE_READ_FROM_CACHE_OP(true, 0, 1, NULL, 0),
		SPINAND_PAGE_READ_FROM_CACHE_OP(false, 0, 1, NULL, 0));

static SPINAND_OP_VARIANTS(read_cache_variants_f,
		SPINAND_PAGE_READ_FROM_CACHE_QUADIO_OP(0, 1, NULL, 0),
		SPINAND_PAGE_READ_FROM_CACHE_X4_OP_3A(0, 1, NULL, 0),
		SPINAND_PAGE_READ_FROM_CACHE_DUALIO_OP(0, 1, NULL, 0),
		SPINAND_PAGE_READ_FROM_CACHE_X2_OP_3A(0, 1, NULL, 0),
		SPINAND_PAGE_READ_FROM_CACHE_OP_3A(true, 0, 1, NULL, 0),
		SPINAND_PAGE_READ_FROM_CACHE_OP_3A(false, 0, 0, NULL, 0));

static SPINAND_OP_VARIANTS(read_cache_variants_nor_emu,
		SPINAND_PAGE_READ_FROM_CACHE_QUADIO_OP(0, 2, NULL, 0),
		SPINAND_PAGE_READ_FROM_CACHE_X4_OP(0, 1, NULL, 0),
		SPINAND_PAGE_READ_FROM_CACHE_DUALIO_OP(0, 1, NULL, 0),
		SPINAND_PAGE_READ_FROM_CACHE_X2_OP(0, 1, NULL, 0),
		SPINAND_PAGE_READ_FROM_CACHE_OP_NOR_EMU(true, 0, 1, NULL, 0),
		SPINAND_PAGE_READ_FROM_CACHE_OP_NOR_EMU(false, 0, 0, NULL, 0));

static SPINAND_OP_VARIANTS(write_cache_variants,
		SPINAND_PROG_LOAD_X4(true, 0, NULL, 0),
		SPINAND_PROG_LOAD(true, 0, NULL, 0));

static SPINAND_OP_VARIANTS(update_cache_variants,
		SPINAND_PROG_LOAD_X4(false, 0, NULL, 0),
		SPINAND_PROG_LOAD(false, 0, NULL, 0));

static int gd5fxgq4xa_ooblayout_ecc(struct mtd_info *mtd, int section,
				  struct mtd_oob_region *region)
{
	if (section > 3)
		return -ERANGE;

	region->offset = (16 * section) + 8;
	region->length = 8;

	return 0;
}

static int gd5fxgq4xa_ooblayout_free(struct mtd_info *mtd, int section,
				   struct mtd_oob_region *region)
{
	if (section > 3)
		return -ERANGE;

	if (section) {
		region->offset = 16 * section;
		region->length = 8;
	} else {
		/* section 0 has one byte reserved for bad block mark */
		region->offset = 1;
		region->length = 7;
	}
	return 0;
}

static const struct mtd_ooblayout_ops gd5fxgq4xa_ooblayout = {
	.ecc = gd5fxgq4xa_ooblayout_ecc,
	.free = gd5fxgq4xa_ooblayout_free,
};

static int gd5fxgq4xa_ecc_get_status(struct spinand_device *spinand,
					 u8 status)
{
	switch (status & STATUS_ECC_MASK) {
	case STATUS_ECC_NO_BITFLIPS:
		return 0;

	case GD5FXGQ4XA_STATUS_ECC_1_7_BITFLIPS:
		/* 1-7 bits are flipped. return the maximum. */
		return 7;

	case GD5FXGQ4XA_STATUS_ECC_8_BITFLIPS:
		return 8;

	case STATUS_ECC_UNCOR_ERROR:
		return -EBADMSG;

	default:
		break;
	}

	return -EINVAL;
}

static int gd5fxgq4_variant2_ooblayout_ecc(struct mtd_info *mtd, int section,
				       struct mtd_oob_region *region)
{
	if (section)
		return -ERANGE;

	region->offset = 64;
	region->length = 64;

	return 0;
}

static int gd5fxgq4_variant2_ooblayout_free(struct mtd_info *mtd, int section,
					struct mtd_oob_region *region)
{
	if (section)
		return -ERANGE;

	/* Reserve 1 bytes for the BBM. */
	region->offset = 1;
	region->length = 63;

	return 0;
}

static const struct mtd_ooblayout_ops gd5fxgq4_variant2_ooblayout = {
	.ecc = gd5fxgq4_variant2_ooblayout_ecc,
	.free = gd5fxgq4_variant2_ooblayout_free,
};

static int gd5fxgq4xc_ooblayout_256_ecc(struct mtd_info *mtd, int section,
					struct mtd_oob_region *oobregion)
{
	if (section)
		return -ERANGE;

	oobregion->offset = 128;
	oobregion->length = 128;

	return 0;
}

static int gd5fxgq4xc_ooblayout_256_free(struct mtd_info *mtd, int section,
					 struct mtd_oob_region *oobregion)
{
	if (section)
		return -ERANGE;

	oobregion->offset = 1;
	oobregion->length = 127;

	return 0;
}

static const struct mtd_ooblayout_ops gd5fxgq4xc_oob_256_ops = {
	.ecc = gd5fxgq4xc_ooblayout_256_ecc,
	.free = gd5fxgq4xc_ooblayout_256_free,
};

static int gd5fxgq4uexxg_ecc_get_status(struct spinand_device *spinand,
					u8 status)
{
	u8 status2;
	struct spi_mem_op op = SPINAND_GET_FEATURE_OP(GD5FXGQ4UEXXG_REG_STATUS2,
						      &status2);
	int ret;

	switch (status & STATUS_ECC_MASK) {
	case STATUS_ECC_NO_BITFLIPS:
		return 0;

	case GD5FXGQ4XA_STATUS_ECC_1_7_BITFLIPS:
		/*
		 * Read status2 register to determine a more fine grained
		 * bit error status
		 */
		ret = spi_mem_exec_op(spinand->spimem, &op);
		if (ret)
			return ret;

		/*
		 * 4 ... 7 bits are flipped (1..4 can't be detected, so
		 * report the maximum of 4 in this case
		 */
		/* bits sorted this way (3...0): ECCS1,ECCS0,ECCSE1,ECCSE0 */
		return ((status & STATUS_ECC_MASK) >> 2) |
			((status2 & STATUS_ECC_MASK) >> 4);

	case GD5FXGQ4XA_STATUS_ECC_8_BITFLIPS:
		return 8;

	case STATUS_ECC_UNCOR_ERROR:
		return -EBADMSG;

	default:
		break;
	}

	return -EINVAL;
}

static int gd5fxgq4ufxxg_ecc_get_status(struct spinand_device *spinand,
					u8 status)
{
	switch (status & GD5FXGQ4UXFXXG_STATUS_ECC_MASK) {
	case GD5FXGQ4UXFXXG_STATUS_ECC_NO_BITFLIPS:
		return 0;

	case GD5FXGQ4UXFXXG_STATUS_ECC_1_3_BITFLIPS:
		return 3;

	case GD5FXGQ4UXFXXG_STATUS_ECC_UNCOR_ERROR:
		return -EBADMSG;

	default: /* (2 << 4) through (6 << 4) are 4-8 corrected errors */
		return ((status & GD5FXGQ4UXFXXG_STATUS_ECC_MASK) >> 4) + 2;
	}

	return -EINVAL;
}

static const struct spinand_info gigadevice_spinand_table[] = {
	SPINAND_INFO("GD5F1GQ4xA", 0xf1,
		     NAND_MEMORG(1, 2048, 64, 64, 1024, 1, 1, 1),
		     NAND_ECCREQ(8, 512),
		     SPINAND_INFO_OP_VARIANTS(&read_cache_variants,
					      &write_cache_variants,
					      &update_cache_variants),
		     SPINAND_HAS_QE_BIT,
		     SPINAND_ECCINFO(&gd5fxgq4xa_ooblayout,
				     gd5fxgq4xa_ecc_get_status)),
	SPINAND_INFO("GD5F2GQ4xA", 0xf2,
		     NAND_MEMORG(1, 2048, 64, 64, 2048, 1, 1, 1),
		     NAND_ECCREQ(8, 512),
		     SPINAND_INFO_OP_VARIANTS(&read_cache_variants,
					      &write_cache_variants,
					      &update_cache_variants),
		     SPINAND_HAS_QE_BIT,
		     SPINAND_ECCINFO(&gd5fxgq4xa_ooblayout,
				     gd5fxgq4xa_ecc_get_status)),
	SPINAND_INFO("GD5F4GQ4xA", 0xf4,
		     NAND_MEMORG(1, 2048, 64, 64, 4096, 1, 1, 1),
		     NAND_ECCREQ(8, 512),
		     SPINAND_INFO_OP_VARIANTS(&read_cache_variants,
					      &write_cache_variants,
					      &update_cache_variants),
		     SPINAND_HAS_QE_BIT,
		     SPINAND_ECCINFO(&gd5fxgq4xa_ooblayout,
				     gd5fxgq4xa_ecc_get_status)),
	SPINAND_INFO("GD5F4GQ4RC",0xa468,
		     NAND_MEMORG(1, 4096, 256, 64, 2048, 1, 1, 1),
		     NAND_ECCREQ(8, 512),
		     SPINAND_INFO_OP_VARIANTS(&read_cache_variants_f,
					      &write_cache_variants,
					      &update_cache_variants),
		     SPINAND_HAS_QE_BIT,
		     SPINAND_ECCINFO(&gd5fxgq4xc_oob_256_ops,
				     gd5fxgq4ufxxg_ecc_get_status)),
	SPINAND_INFO("GD5F4GQ4UC",0xb468,
		     NAND_MEMORG(1, 4096, 256, 64, 2048, 1, 1, 1),
		     NAND_ECCREQ(8, 512),
		     SPINAND_INFO_OP_VARIANTS(&read_cache_variants_f,
					      &write_cache_variants,
					      &update_cache_variants),
		     SPINAND_HAS_QE_BIT,
		     SPINAND_ECCINFO(&gd5fxgq4xc_oob_256_ops,
				     gd5fxgq4ufxxg_ecc_get_status)),
	SPINAND_INFO("GD5F1GQ4UExxG", 0xd1,
		     NAND_MEMORG(1, 2048, 128, 64, 1024, 1, 1, 1),
		     NAND_ECCREQ(8, 512),
		     SPINAND_INFO_OP_VARIANTS(&read_cache_variants,
					      &write_cache_variants,
					      &update_cache_variants),
		     SPINAND_HAS_QE_BIT,
		     SPINAND_ECCINFO(&gd5fxgq4_variant2_ooblayout,
				     gd5fxgq4uexxg_ecc_get_status)),
	SPINAND_INFO("GD5F1GQ4UFxxG",0xb148,
		     NAND_MEMORG(1, 2048, 128, 64, 1024, 1, 1, 1),
		     NAND_ECCREQ(8, 512),
		     SPINAND_INFO_OP_VARIANTS(&read_cache_variants_f,
					      &write_cache_variants,
					      &update_cache_variants),
		     SPINAND_HAS_QE_BIT,
		     SPINAND_ECCINFO(&gd5fxgq4_variant2_ooblayout,
				     gd5fxgq4ufxxg_ecc_get_status)),
};


#define TABLE_SZ (ARRAY_SIZE(gigadevice_spinand_table))
static struct spinand_info gigadevice_spinand_nor_emu_table[TABLE_SZ];

static void gigadevice_spinand_generate_nor_emu_table(void)
{
	static struct spinand_info *info = gigadevice_spinand_nor_emu_table;
	unsigned int i;

	memcpy(gigadevice_spinand_nor_emu_table, gigadevice_spinand_table,
	       sizeof(gigadevice_spinand_table));

	for (i = 0; i < TABLE_SZ; i++) {
		info[i].op_variants.read_cache = &read_cache_variants_nor_emu;
		info[i].flags |= SPINAND_RELOAD_PAGE_0;
	}
}

static int gigadevice_spinand_read_cfg_reg(struct spinand_device *spinand, u8 *buf,
				       u8 *val)
{
	struct spi_mem_op op = SPINAND_GET_FEATURE_OP(REG_CFG, buf);
	int ret;

	ret = spi_mem_exec_op(spinand->spimem, &op);
	if (ret)
		return ret;

	*val = buf[0];
	return 0;
}

static int gigadevice_spinand_write_cfg_reg(struct spinand_device *spinand, u8 *buf,
					u8 val)
{
	struct spi_mem_op op = SPINAND_SET_FEATURE_OP(REG_CFG, buf);

	buf[0] = val;
	return spi_mem_exec_op(spinand->spimem, &op);
}

static int gigadevice_spinand_load_page(struct spinand_device *spinand,
				    unsigned int row)
{
	struct spi_mem_op op = SPINAND_PAGE_READ_OP(row);

	return spi_mem_exec_op(spinand->spimem, &op);
}

static int gigadevice_spinand_read_from_cache(struct spinand_device *spinand,
					  void *buf, unsigned int len)
{
	struct spi_mem_op op = SPINAND_PAGE_READ_FROM_CACHE_OP_NOR_EMU(false, 0,
								0, NULL, 0);
	unsigned int nbytes = len;
	u8 *bufptr = buf;
	int ret;

	/*
	 * Some controllers are limited in term of max RX data size. In this
	 * case, just repeat the READ_CACHE operation after updating the
	 * column.
	 */
	while (nbytes) {
		op.data.buf.in = bufptr;
		op.data.nbytes = nbytes;
		ret = spi_mem_adjust_op_size(spinand->spimem, &op);
		if (ret)
			return ret;

		ret = spi_mem_exec_op(spinand->spimem, &op);
		if (ret)
			return ret;

		bufptr += op.data.nbytes;
		nbytes -= op.data.nbytes;
		op.addr.val += op.data.nbytes;
	}

	return 0;
}

static int gigadevice_spinand_detect_nor_emu(struct spinand_device *spinand,
					 u16 devid, int *enabled)
{
	struct device *dev = &spinand->spimem->spi->dev;
	unsigned int i, num_ffs = 0, num_zeros = 0, len = 0;
	u8 cfg, *buf;
	int ret;

	for (i = 0; i < TABLE_SZ; i++) {
		const struct spinand_info *info = &gigadevice_spinand_table[i];

		if (devid != info->devid)
			continue;

		len = info->memorg.pagesize + info->memorg.oobsize;
		break;
	}

	if (!len) {
		*enabled = 0;
		return 0;
	}

	buf = kmalloc(len, GFP_KERNEL);
	if (!buf)
		return -ENOMEM;

	ret = gigadevice_spinand_read_cfg_reg(spinand, buf, &cfg);
	if (ret) {
		dev_err(dev, "failed to read configuration\n");
		goto cleanup;
	}

	cfg &= ~0xc2;
	cfg |= 0x82;

	ret = gigadevice_spinand_write_cfg_reg(spinand, buf, cfg);
	if (ret) {
		dev_err(dev, "failed to write configuration\n");
		goto cleanup;
	}

	ret = gigadevice_spinand_read_cfg_reg(spinand, buf, &cfg);
	if (ret) {
		dev_err(dev, "failed to read back configuration\n");
		goto cleanup;
	}

	if ((cfg & 0xc2) != 0x82) {
		dev_info(dev,
			 "flash chip does not support NOR read emulation\n");
		ret = -ENOTSUPP;
		goto cleanup;
	}

	ret = gigadevice_spinand_load_page(spinand, 0);
	if (ret) {
		dev_err(dev, "failed to load NOR read configuration\n");
		goto cleanup;
	}

	ret = gigadevice_spinand_read_from_cache(spinand, buf, len);
	if (ret) {
		dev_err(dev, "failed to read NOR read configuration\n");
		goto cleanup;
	}

	if (!ret) {
		for (i = 0; i < len; i++) {
			if (buf[i] == 0xff)
				num_ffs++;
			else if (!buf[i])
				num_zeros++;
		}

		if (num_ffs == len) {
			*enabled = 0;
		} else if (num_zeros == len) {
			*enabled = 1;
		} else {
			dev_warn(dev,
				 "failed to check NOR read configuration\n");
			ret = -EINVAL;
		}
	}

cleanup:
	if (gigadevice_spinand_write_cfg_reg(spinand, buf, cfg & (~0xc2)))
		dev_err(dev, "failed to leave NOR read configuration\n");

	kfree(buf);

	return ret;
}


static int gigadevice_spinand_detect(struct spinand_device *spinand)
{
	struct device *dev = &spinand->spimem->spi->dev;
	const struct spinand_info *table;
	u8 *id = spinand->id.data;
	u16 did;
	int ret, nor_read = 0;

	/*
	 * GigaDevice SPI NAND read ID need a dummy byte,
	 * so the first byte in raw_id is dummy.
	 */
	if (id[1] == SPINAND_MFR_GIGADEVICE)
		did = (id[1] << 8) + id[2];
	else if (id[0] == 0 && id[1] == SPINAND_MFR_GIGADEVICE)
		did = id[2];
	else
		return 0;

	ret = gigadevice_spinand_detect_nor_emu(spinand, did, &nor_read);
	if (ret)
		nor_read = 0;

	if (nor_read) {
		dev_info(dev, "flash chip is using NOR read emulation\n");
		gigadevice_spinand_generate_nor_emu_table();
		table = gigadevice_spinand_nor_emu_table;
	} else {
		table = gigadevice_spinand_table;
	}

	ret = spinand_match_and_init(spinand, table, TABLE_SZ, did);
	if (ret)
		return ret;

	return 1;
}

static const struct spinand_manufacturer_ops gigadevice_spinand_manuf_ops = {
	.detect = gigadevice_spinand_detect,
};

const struct spinand_manufacturer gigadevice_spinand_manufacturer = {
	.id = SPINAND_MFR_GIGADEVICE,
	.name = "GigaDevice",
	.ops = &gigadevice_spinand_manuf_ops,
};

