// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2018 Macronix
 *
 * Author: Boris Brezillon <boris.brezillon@bootlin.com>
 */

#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/mtd/spinand.h>

#define SPINAND_MFR_MACRONIX		0xC2

#define SPINAND_PAGE_READ_FROM_CACHE_OP_NOR_EMU(fast, addr, ndummy, buf, len)	\
	SPI_MEM_OP(SPI_MEM_OP_CMD(fast ? 0x0b : 0x03, 1),		\
		   SPI_MEM_OP_ADDR(3, addr, 1),				\
		   SPI_MEM_OP_DUMMY(ndummy, 1),				\
		   SPI_MEM_OP_DATA_IN(len, buf, 1))

static SPINAND_OP_VARIANTS(read_cache_variants,
		SPINAND_PAGE_READ_FROM_CACHE_X4_OP(0, 1, NULL, 0),
		SPINAND_PAGE_READ_FROM_CACHE_X2_OP(0, 1, NULL, 0),
		SPINAND_PAGE_READ_FROM_CACHE_OP(true, 0, 1, NULL, 0),
		SPINAND_PAGE_READ_FROM_CACHE_OP(false, 0, 1, NULL, 0));

static SPINAND_OP_VARIANTS(read_cache_variants_nor_emu,
		SPINAND_PAGE_READ_FROM_CACHE_X4_OP(0, 1, NULL, 0),
		SPINAND_PAGE_READ_FROM_CACHE_X2_OP(0, 1, NULL, 0),
		SPINAND_PAGE_READ_FROM_CACHE_OP_NOR_EMU(true, 0, 1, NULL, 0),
		SPINAND_PAGE_READ_FROM_CACHE_OP_NOR_EMU(false, 0, 0, NULL, 0));

static SPINAND_OP_VARIANTS(write_cache_variants,
		SPINAND_PROG_LOAD_X4(true, 0, NULL, 0),
		SPINAND_PROG_LOAD(true, 0, NULL, 0));

static SPINAND_OP_VARIANTS(update_cache_variants,
		SPINAND_PROG_LOAD_X4(false, 0, NULL, 0),
		SPINAND_PROG_LOAD(false, 0, NULL, 0));

static int mx35lfxge4ab_ooblayout_ecc(struct mtd_info *mtd, int section,
				      struct mtd_oob_region *region)
{
	return -ERANGE;
}

static int mx35lfxge4ab_ooblayout_free(struct mtd_info *mtd, int section,
				       struct mtd_oob_region *region)
{
	if (section)
		return -ERANGE;

	region->offset = 2;
	region->length = mtd->oobsize - 2;

	return 0;
}

static const struct mtd_ooblayout_ops mx35lfxge4ab_ooblayout = {
	.ecc = mx35lfxge4ab_ooblayout_ecc,
	.free = mx35lfxge4ab_ooblayout_free,
};

static int mx35lf1ge4ab_get_eccsr(struct spinand_device *spinand, u8 *eccsr)
{
	struct spi_mem_op op = SPI_MEM_OP(SPI_MEM_OP_CMD(0x7c, 1),
					  SPI_MEM_OP_NO_ADDR,
					  SPI_MEM_OP_DUMMY(1, 1),
					  SPI_MEM_OP_DATA_IN(1, eccsr, 1));

	return spi_mem_exec_op(spinand->spimem, &op);
}

static int mx35lf1ge4ab_ecc_get_status(struct spinand_device *spinand,
				       u8 status)
{
	struct nand_device *nand = spinand_to_nand(spinand);
	u8 eccsr;

	switch (status & STATUS_ECC_MASK) {
	case STATUS_ECC_NO_BITFLIPS:
		return 0;

	case STATUS_ECC_UNCOR_ERROR:
		return -EBADMSG;

	case STATUS_ECC_HAS_BITFLIPS:
		/*
		 * Let's try to retrieve the real maximum number of bitflips
		 * in order to avoid forcing the wear-leveling layer to move
		 * data around if it's not necessary.
		 */
		if (mx35lf1ge4ab_get_eccsr(spinand, &eccsr))
			return nand->eccreq.strength;

		if (WARN_ON(eccsr > nand->eccreq.strength || !eccsr))
			return nand->eccreq.strength;

		return eccsr;

	default:
		break;
	}

	return -EINVAL;
}

static const struct spinand_info macronix_spinand_table[] = {
	SPINAND_INFO("MX35LF1GE4AB", 0x12,
		     NAND_MEMORG(1, 2048, 64, 64, 1024, 1, 1, 1),
		     NAND_ECCREQ(4, 512),
		     SPINAND_INFO_OP_VARIANTS(&read_cache_variants,
					      &write_cache_variants,
					      &update_cache_variants),
		     SPINAND_HAS_QE_BIT,
		     SPINAND_ECCINFO(&mx35lfxge4ab_ooblayout,
				     mx35lf1ge4ab_ecc_get_status)),
	SPINAND_INFO("MX35LF2GE4AB", 0x22,
		     NAND_MEMORG(1, 2048, 64, 64, 2048, 2, 1, 1),
		     NAND_ECCREQ(4, 512),
		     SPINAND_INFO_OP_VARIANTS(&read_cache_variants,
					      &write_cache_variants,
					      &update_cache_variants),
		     SPINAND_HAS_QE_BIT,
		     SPINAND_ECCINFO(&mx35lfxge4ab_ooblayout, NULL)),
	SPINAND_INFO("MX35LF2GE4AD", 0x26,
		     NAND_MEMORG(1, 2048, 64, 64, 2048, 1, 1, 1),
		     NAND_ECCREQ(8, 512),
		     SPINAND_INFO_OP_VARIANTS(&read_cache_variants,
					      &write_cache_variants,
					      &update_cache_variants),
		     0,
		     SPINAND_ECCINFO(&mx35lfxge4ab_ooblayout,
				     mx35lf1ge4ab_ecc_get_status)),
	SPINAND_INFO("MX35LF4GE4AD", 0x37,
		     NAND_MEMORG(1, 4096, 128, 64, 2048, 1, 1, 1),
		     NAND_ECCREQ(8, 512),
		     SPINAND_INFO_OP_VARIANTS(&read_cache_variants,
					      &write_cache_variants,
					      &update_cache_variants),
		     0,
		     SPINAND_ECCINFO(&mx35lfxge4ab_ooblayout,
				     mx35lf1ge4ab_ecc_get_status)),
	SPINAND_INFO("MX35LF1G24AD", 0x14,
		     NAND_MEMORG(1, 2048, 128, 64, 1024, 1, 1, 1),
		     NAND_ECCREQ(8, 512),
		     SPINAND_INFO_OP_VARIANTS(&read_cache_variants,
					      &write_cache_variants,
					      &update_cache_variants),
		     0,
		     SPINAND_ECCINFO(&mx35lfxge4ab_ooblayout, NULL)),
	SPINAND_INFO("MX35LF2G24AD", 0x24,
		     NAND_MEMORG(1, 2048, 128, 64, 2048, 1, 1, 1),
		     NAND_ECCREQ(8, 512),
		     SPINAND_INFO_OP_VARIANTS(&read_cache_variants,
					      &write_cache_variants,
					      &update_cache_variants),
		     0,
		     SPINAND_ECCINFO(&mx35lfxge4ab_ooblayout, NULL)),
	SPINAND_INFO("MX35LF4G24AD", 0x35,
		     NAND_MEMORG(1, 4096, 256, 64, 2048, 2, 1, 1),
		     NAND_ECCREQ(8, 512),
		     SPINAND_INFO_OP_VARIANTS(&read_cache_variants,
					      &write_cache_variants,
					      &update_cache_variants),
		     0,
		     SPINAND_ECCINFO(&mx35lfxge4ab_ooblayout, NULL)),
	SPINAND_INFO("MX31LF1GE4BC", 0x1e,
		     NAND_MEMORG(1, 2048, 64, 64, 1024, 1, 1, 1),
		     NAND_ECCREQ(8, 512),
		     SPINAND_INFO_OP_VARIANTS(&read_cache_variants,
					      &write_cache_variants,
					      &update_cache_variants),
		     0 /*SPINAND_HAS_QE_BIT*/,
		     SPINAND_ECCINFO(&mx35lfxge4ab_ooblayout,
				     mx35lf1ge4ab_ecc_get_status)),
	SPINAND_INFO("MX35UF4GE4AD", 0xb7,
		     NAND_MEMORG(1, 4096, 256, 64, 2048, 1, 1, 1),
		     NAND_ECCREQ(8, 512),
		     SPINAND_INFO_OP_VARIANTS(&read_cache_variants,
					      &write_cache_variants,
					      &update_cache_variants),
		     0,
		     SPINAND_ECCINFO(&mx35lfxge4ab_ooblayout,
				     mx35lf1ge4ab_ecc_get_status)),
	SPINAND_INFO("MX35UF2GE4AD", 0xa6,
		     NAND_MEMORG(1, 2048, 128, 64, 2048, 1, 1, 1),
		     NAND_ECCREQ(8, 512),
		     SPINAND_INFO_OP_VARIANTS(&read_cache_variants,
					      &write_cache_variants,
					      &update_cache_variants),
		     0,
		     SPINAND_ECCINFO(&mx35lfxge4ab_ooblayout,
				     mx35lf1ge4ab_ecc_get_status)),
	SPINAND_INFO("MX35UF2GE4AC", 0xa2,
		     NAND_MEMORG(1, 2048, 64, 64, 2048, 1, 1, 1),
		     NAND_ECCREQ(4, 512),
		     SPINAND_INFO_OP_VARIANTS(&read_cache_variants,
					      &write_cache_variants,
					      &update_cache_variants),
		     0,
		     SPINAND_ECCINFO(&mx35lfxge4ab_ooblayout,
				     mx35lf1ge4ab_ecc_get_status)),
	SPINAND_INFO("MX35UF1GE4AD", 0x96,
		     NAND_MEMORG(1, 2048, 128, 64, 1024, 1, 1, 1),
		     NAND_ECCREQ(8, 512),
		     SPINAND_INFO_OP_VARIANTS(&read_cache_variants,
					      &write_cache_variants,
					      &update_cache_variants),
		     0,
		     SPINAND_ECCINFO(&mx35lfxge4ab_ooblayout,
				     mx35lf1ge4ab_ecc_get_status)),
	SPINAND_INFO("MX35UF1GE4AC", 0x92,
		     NAND_MEMORG(1, 2048, 64, 64, 1024, 1, 1, 1),
		     NAND_ECCREQ(4, 512),
		     SPINAND_INFO_OP_VARIANTS(&read_cache_variants,
					      &write_cache_variants,
					      &update_cache_variants),
		     0,
		     SPINAND_ECCINFO(&mx35lfxge4ab_ooblayout,
				     mx35lf1ge4ab_ecc_get_status)),
	SPINAND_INFO("MX31UF1GE4BC", 0x9e,
		     NAND_MEMORG(1, 2048, 64, 64, 1024, 1, 1, 1),
		     NAND_ECCREQ(8, 512),
		     SPINAND_INFO_OP_VARIANTS(&read_cache_variants,
					      &write_cache_variants,
					      &update_cache_variants),
		     0 /*SPINAND_HAS_QE_BIT*/,
		     SPINAND_ECCINFO(&mx35lfxge4ab_ooblayout,
				     mx35lf1ge4ab_ecc_get_status)),
};

#define TABLE_SZ (ARRAY_SIZE(macronix_spinand_table))
static struct spinand_info macronix_spinand_nor_emu_table[TABLE_SZ];

static void macronix_spinand_generate_nor_emu_table(void)
{
	static struct spinand_info *info = macronix_spinand_nor_emu_table;
	unsigned int i;

	memcpy(macronix_spinand_nor_emu_table, macronix_spinand_table,
	       sizeof(macronix_spinand_table));

	for (i = 0; i < TABLE_SZ; i++) {
		info[i].op_variants.read_cache = &read_cache_variants_nor_emu;
		info[i].flags |= SPINAND_RELOAD_PAGE_0;
	}
}

static int macronix_spinand_read_cfg_reg(struct spinand_device *spinand, u8 *buf,
				       u8 *val)
{
	struct spi_mem_op op = SPINAND_GET_FEATURE_OP(0x60, buf);
	int ret;

	ret = spi_mem_exec_op(spinand->spimem, &op);
	if (ret)
		return ret;

	*val = buf[0];
	return 0;
}

static int macronix_spinand_write_cfg_reg(struct spinand_device *spinand, u8 *buf,
					u8 val)
{
	struct spi_mem_op op = SPINAND_SET_FEATURE_OP(0x60, buf);

	buf[0] = val;
	return spi_mem_exec_op(spinand->spimem, &op);
}

static int macronix_spinand_load_page(struct spinand_device *spinand,
				    unsigned int row)
{
	struct spi_mem_op op = SPINAND_PAGE_READ_OP(row);

	return spi_mem_exec_op(spinand->spimem, &op);
}

static int macronix_spinand_read_from_cache(struct spinand_device *spinand,
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

static int macronix_spinand_detect_nor_emu(struct spinand_device *spinand,
					 u8 devid, int *enabled)
{
	struct device *dev = &spinand->spimem->spi->dev;
	unsigned int i, num_ffs = 0, num_zeros = 0, len = 0;
	u8 cfg, *buf;
	int ret;

	for (i = 0; i < TABLE_SZ; i++) {
		const struct spinand_info *info = &macronix_spinand_table[i];

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

	ret = macronix_spinand_read_cfg_reg(spinand, buf, &cfg);
	if (ret) {
		dev_err(dev, "failed to read configuration\n");
		goto cleanup;
	}
	cfg |= 0x2;
	ret = macronix_spinand_write_cfg_reg(spinand, buf, cfg);
	if (ret) {
		dev_err(dev, "failed to write configuration\n");
		goto cleanup;
	}

	ret = macronix_spinand_read_cfg_reg(spinand, buf, &cfg);
	if (ret) {
		dev_err(dev, "failed to read back configuration\n");
		goto cleanup;
	}

	if ((cfg & 0x2) != 0x2) {
		dev_info(dev,
			 "flash chip does not support NOR read emulation\n");
		ret = -ENOTSUPP;
		goto cleanup;
	}

	ret = macronix_spinand_load_page(spinand, 0);
	if (ret) {
		dev_err(dev, "failed to load NOR read configuration\n");
		goto cleanup;
	}

	ret = macronix_spinand_read_from_cache(spinand, buf, len);
	if (ret) {
		dev_err(dev, "failed to read NOR read configuration\n");
		goto cleanup;
	}
#if 0
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
#endif
cleanup:
	if (macronix_spinand_write_cfg_reg(spinand, buf, cfg & (~0x2)))
		dev_err(dev, "failed to leave NOR read configuration\n");

	kfree(buf);

	return ret;
}

static int macronix_spinand_detect(struct spinand_device *spinand)
{
	struct device *dev = &spinand->spimem->spi->dev;
	const struct spinand_info *table;
	u8 *id = spinand->id.data;
	int ret, nor_read = 0;

	/*
	 * Macronix SPI NAND read ID needs a dummy byte, so the first byte in
	 * raw_id is garbage.
	 */
	if (id[1] != SPINAND_MFR_MACRONIX)
		return 0;

	ret = macronix_spinand_detect_nor_emu(spinand, id[2], &nor_read);
	if (ret)
		nor_read = 0;

	if (nor_read) {
		dev_info(dev, "flash chip is using NOR read emulation\n");
		macronix_spinand_generate_nor_emu_table();
		table = macronix_spinand_nor_emu_table;
	} else {
		table = macronix_spinand_table;
	}

	ret = spinand_match_and_init(spinand, table, TABLE_SZ, id[2]);
	if (ret)
		return ret;

	return 1;
}

static const struct spinand_manufacturer_ops macronix_spinand_manuf_ops = {
	.detect = macronix_spinand_detect,
};

const struct spinand_manufacturer macronix_spinand_manufacturer = {
	.id = SPINAND_MFR_MACRONIX,
	.name = "Macronix",
	.ops = &macronix_spinand_manuf_ops,
};

