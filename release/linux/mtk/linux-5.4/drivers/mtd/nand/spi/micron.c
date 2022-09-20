// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2016-2017 Micron Technology, Inc.
 *
 * Authors:
 *	Peter Pan <peterpandong@micron.com>
 */

#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/mtd/spinand.h>

#define SPINAND_MFR_MICRON		0x2c

#define MICRON_STATUS_ECC_MASK		GENMASK(7, 4)
#define MICRON_STATUS_ECC_NO_BITFLIPS	(0 << 4)
#define MICRON_STATUS_ECC_1TO3_BITFLIPS	(1 << 4)
#define MICRON_STATUS_ECC_4TO6_BITFLIPS	(3 << 4)
#define MICRON_STATUS_ECC_7TO8_BITFLIPS	(5 << 4)

#define MICRON_CFG_CR			BIT(0)

#if defined(CONFIG_SOC_MT7621)
#define SPINAND_PAGE_READ_FROM_CACHE_OP_NOR_EMU(fast, addr, ndummy, buf, len)	\
	SPI_MEM_OP(SPI_MEM_OP_CMD(fast ? 0x0b : 0x03, 1),		\
		   SPI_MEM_OP_ADDR(3, addr, 1),				\
		   SPI_MEM_OP_DUMMY(ndummy, 1),				\
		   SPI_MEM_OP_DATA_IN(len, buf, 1))

static SPINAND_OP_VARIANTS(read_cache_variants_nor_emu,
		SPINAND_PAGE_READ_FROM_CACHE_QUADIO_OP(0, 2, NULL, 0),
		SPINAND_PAGE_READ_FROM_CACHE_X4_OP(0, 1, NULL, 0),
		SPINAND_PAGE_READ_FROM_CACHE_DUALIO_OP(0, 1, NULL, 0),
		SPINAND_PAGE_READ_FROM_CACHE_X2_OP(0, 1, NULL, 0),
		SPINAND_PAGE_READ_FROM_CACHE_OP_NOR_EMU(true, 0, 1, NULL, 0),
		SPINAND_PAGE_READ_FROM_CACHE_OP_NOR_EMU(false, 0, 0, NULL, 0));
#endif

static SPINAND_OP_VARIANTS(quadio_read_cache_variants,
		SPINAND_PAGE_READ_FROM_CACHE_QUADIO_OP(0, 2, NULL, 0),
		SPINAND_PAGE_READ_FROM_CACHE_X4_OP(0, 1, NULL, 0),
		SPINAND_PAGE_READ_FROM_CACHE_DUALIO_OP(0, 1, NULL, 0),
		SPINAND_PAGE_READ_FROM_CACHE_X2_OP(0, 1, NULL, 0),
		SPINAND_PAGE_READ_FROM_CACHE_OP(true, 0, 1, NULL, 0),
		SPINAND_PAGE_READ_FROM_CACHE_OP(false, 0, 1, NULL, 0));

static SPINAND_OP_VARIANTS(x4_write_cache_variants,
		SPINAND_PROG_LOAD_X4(true, 0, NULL, 0),
		SPINAND_PROG_LOAD(true, 0, NULL, 0));

static SPINAND_OP_VARIANTS(x4_update_cache_variants,
		SPINAND_PROG_LOAD_X4(false, 0, NULL, 0),
		SPINAND_PROG_LOAD(false, 0, NULL, 0));

static int micron_8_ooblayout_ecc(struct mtd_info *mtd, int section,
				  struct mtd_oob_region *region)
{
	if (section)
		return -ERANGE;

	region->offset = mtd->oobsize / 2;
	region->length = mtd->oobsize / 2;

	return 0;
}

static int micron_8_ooblayout_free(struct mtd_info *mtd, int section,
				   struct mtd_oob_region *region)
{
	if (section)
		return -ERANGE;

	/* Reserve 2 bytes for the BBM. */
	region->offset = 2;
	region->length = (mtd->oobsize / 2) - 2;

	return 0;
}

static const struct mtd_ooblayout_ops micron_8_ooblayout = {
	.ecc = micron_8_ooblayout_ecc,
	.free = micron_8_ooblayout_free,
};

static int micron_8_ecc_get_status(struct spinand_device *spinand,
				   u8 status)
{
	switch (status & MICRON_STATUS_ECC_MASK) {
	case STATUS_ECC_NO_BITFLIPS:
		return 0;

	case STATUS_ECC_UNCOR_ERROR:
		return -EBADMSG;

	case MICRON_STATUS_ECC_1TO3_BITFLIPS:
		return 3;

	case MICRON_STATUS_ECC_4TO6_BITFLIPS:
		return 6;

	case MICRON_STATUS_ECC_7TO8_BITFLIPS:
		return 8;

	default:
		break;
	}

	return -EINVAL;
}

static const struct spinand_info micron_spinand_table[] = {
	/* M79A 2Gb 3.3V */
	SPINAND_INFO("MT29F2G01ABAGD", 0x24,
		     NAND_MEMORG(1, 2048, 128, 64, 2048, 40, 2, 1, 1),
		     NAND_ECCREQ(8, 512),
		     SPINAND_INFO_OP_VARIANTS(&quadio_read_cache_variants,
					      &x4_write_cache_variants,
					      &x4_update_cache_variants),
		     0,
		     SPINAND_ECCINFO(&micron_8_ooblayout,
				     micron_8_ecc_get_status)),
	/* M79A 2Gb 1.8V */
	SPINAND_INFO("MT29F2G01ABBGD", 0x25,
		     NAND_MEMORG(1, 2048, 128, 64, 2048, 40, 2, 1, 1),
		     NAND_ECCREQ(8, 512),
		     SPINAND_INFO_OP_VARIANTS(&quadio_read_cache_variants,
					      &x4_write_cache_variants,
					      &x4_update_cache_variants),
		     0,
		     SPINAND_ECCINFO(&micron_8_ooblayout,
				     micron_8_ecc_get_status)),
	/* M78A 1Gb 3.3V */
	SPINAND_INFO("MT29F1G01ABAFD", 0x14,
		     NAND_MEMORG(1, 2048, 128, 64, 1024, 20, 1, 1, 1),
		     NAND_ECCREQ(8, 512),
		     SPINAND_INFO_OP_VARIANTS(&quadio_read_cache_variants,
					      &x4_write_cache_variants,
					      &x4_update_cache_variants),
		     0,
		     SPINAND_ECCINFO(&micron_8_ooblayout,
				     micron_8_ecc_get_status)),
	/* M78A 1Gb 1.8V */
	SPINAND_INFO("MT29F1G01ABAFD", 0x15,
		     NAND_MEMORG(1, 2048, 128, 64, 1024, 20, 1, 1, 1),
		     NAND_ECCREQ(8, 512),
		     SPINAND_INFO_OP_VARIANTS(&quadio_read_cache_variants,
					      &x4_write_cache_variants,
					      &x4_update_cache_variants),
		     0,
		     SPINAND_ECCINFO(&micron_8_ooblayout,
				     micron_8_ecc_get_status)),
	SPINAND_INFO("MT29F4G01ABAFD", 0x34,
		     NAND_MEMORG(1, 4096, 256, 64, 2048, 40, 1, 1, 1),
		     NAND_ECCREQ(8, 512),
		     SPINAND_INFO_OP_VARIANTS(&quadio_read_cache_variants,
					      &x4_write_cache_variants,
					      &x4_update_cache_variants),
		     SPINAND_HAS_CR_FEAT_BIT,
		     SPINAND_ECCINFO(&micron_8_ooblayout,
				     micron_8_ecc_get_status)),
};

#if defined(CONFIG_SOC_MT7621)
#define TABLE_SZ (ARRAY_SIZE(micron_spinand_table))
static struct spinand_info micron_spinand_nor_emu_table[TABLE_SZ];

static void micron_spinand_generate_nor_emu_table(void)
{
	static struct spinand_info *info = micron_spinand_nor_emu_table;
	unsigned int i;

	memcpy(micron_spinand_nor_emu_table, micron_spinand_table,
	       sizeof(micron_spinand_table));

	for (i = 0; i < TABLE_SZ; i++) {
		info[i].op_variants.read_cache = &read_cache_variants_nor_emu;
		info[i].flags |= SPINAND_RELOAD_PAGE_0;
	}
}

static int micron_spinand_read_cfg_reg(struct spinand_device *spinand, u8 *buf,
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

static int micron_spinand_write_cfg_reg(struct spinand_device *spinand, u8 *buf,
					u8 val)
{
	struct spi_mem_op op = SPINAND_SET_FEATURE_OP(REG_CFG, buf);

	buf[0] = val;
	return spi_mem_exec_op(spinand->spimem, &op);
}

static int micron_spinand_load_page(struct spinand_device *spinand,
				    unsigned int row)
{
	struct spi_mem_op op = SPINAND_PAGE_READ_OP(row);

	return spi_mem_exec_op(spinand->spimem, &op);
}

static int micron_spinand_read_from_cache(struct spinand_device *spinand,
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

static int micron_spinand_detect_nor_emu(struct spinand_device *spinand,
					 u8 devid, int *enabled)
{
	struct device *dev = &spinand->spimem->spi->dev;
	unsigned int i, num_ffs = 0, num_zeros = 0, len = 0;
	u8 cfg, *buf;
	int ret;

	for (i = 0; i < TABLE_SZ; i++) {
		const struct spinand_info *info = &micron_spinand_table[i];

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

	ret = micron_spinand_read_cfg_reg(spinand, buf, &cfg);
	if (ret) {
		dev_err(dev, "failed to read configuration\n");
		goto cleanup;
	}

	cfg &= ~0xc2;
	cfg |= 0x82;

	ret = micron_spinand_write_cfg_reg(spinand, buf, cfg);
	if (ret) {
		dev_err(dev, "failed to write configuration\n");
		goto cleanup;
	}

	ret = micron_spinand_read_cfg_reg(spinand, buf, &cfg);
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

	ret = micron_spinand_load_page(spinand, 0);
	if (ret) {
		dev_err(dev, "failed to load NOR read configuration\n");
		goto cleanup;
	}

	ret = micron_spinand_read_from_cache(spinand, buf, len);
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
	if (micron_spinand_write_cfg_reg(spinand, buf, cfg & (~0xc2)))
		dev_err(dev, "failed to leave NOR read configuration\n");

	kfree(buf);

	return ret;
}
#endif

static int micron_spinand_detect(struct spinand_device *spinand)
{
	u8 *id = spinand->id.data;
	int ret;
#if defined(CONFIG_SOC_MT7621)
	struct device *dev = &spinand->spimem->spi->dev;
	const struct spinand_info *table;
	int nor_read = 0;
#endif

	/*
	 * Micron SPI NAND read ID need a dummy byte,
	 * so the first byte in raw_id is dummy.
	 */
	if (id[1] != SPINAND_MFR_MICRON)
		return 0;
#if defined(CONFIG_SOC_MT7621)
	ret = micron_spinand_detect_nor_emu(spinand, id[2], &nor_read);
	if (ret)
		nor_read = 0;

	if (nor_read) {
		dev_info(dev, "flash chip is using NOR read emulation\n");
		micron_spinand_generate_nor_emu_table();
		table = micron_spinand_nor_emu_table;
	} else {
		table = micron_spinand_table;
	}

	ret = spinand_match_and_init(spinand, table, TABLE_SZ, id[2]);
#else
	ret = spinand_match_and_init(spinand, micron_spinand_table,
				     ARRAY_SIZE(micron_spinand_table), id[2]);
#endif
	if (ret)
		return ret;

	return 1;
}

static const struct spinand_manufacturer_ops micron_spinand_manuf_ops = {
	.detect = micron_spinand_detect,
};

const struct spinand_manufacturer micron_spinand_manufacturer = {
	.id = SPINAND_MFR_MICRON,
	.name = "Micron",
	.ops = &micron_spinand_manuf_ops,
};

