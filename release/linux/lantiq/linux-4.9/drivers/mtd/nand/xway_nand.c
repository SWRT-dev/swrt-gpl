/*
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 *
 *  Copyright © 2012 John Crispin <blogic@openwrt.org>
 *  Copyright © 2016 Hauke Mehrtens <hauke@hauke-m.de>
 *  Copyright © 2017 Mohammad Firdaus B Alias Thanis <m.aliasthani@intel.com>
 */

#include <asm/io.h>
#include <linux/module.h>
#include <linux/mtd/nand.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/of_gpio.h>
#include <linux/of_platform.h>
#include <linux/of_address.h>

#include <lantiq_soc.h>

/* nand registers */
#define EBU_ADDSEL0		0x20
#define EBU_ADDSEL1		0x24
#define EBU_NAND_CON		0xB0
#define EBU_NAND_WAIT		0xB4
#define  NAND_WAIT_RD		BIT(0) /* NAND flash status output */
#define  NAND_WAIT_WR_C		BIT(3) /* NAND Write/Read complete */
#define EBU_NAND_ECC0		0xB8
#define EBU_NAND_ECC_AC		0xBC

/*
 * nand commands
 * The pins of the NAND chip are selected based on the address bits of the
 * "register" read and write. There are no special registers, but an
 * address range and the lower address bits are used to activate the
 * correct line. For example when the bit (1 << 2) is set in the address
 * the ALE pin will be activated.
 */
#define NAND_CMD_ALE		BIT(2) /* address latch enable */
#define NAND_CMD_CLE		BIT(3) /* command latch enable */
#define NAND_CMD_CS		BIT(4) /* chip select */
#define NAND_CMD_SE		BIT(5) /* spare area access latch */
#define NAND_CMD_WP		BIT(6) /* write protect */
#define NAND_WRITE_CMD		(NAND_CMD_CS | NAND_CMD_CLE)
#define NAND_WRITE_ADDR		(NAND_CMD_CS | NAND_CMD_ALE)
#define NAND_WRITE_DATA		(NAND_CMD_CS)
#define NAND_READ_DATA		(NAND_CMD_CS)

/* we need to tel the ebu which addr we mapped the nand to */
#define ADDSEL1_MASK(x)		(x << 4)
#define ADDSEL1_REGEN		1
#define ADDSEL0_REGEN		1

/* we need to tell the EBU that we have nand attached and set it up properly */
#define BUSCON1_SETUP		(1 << 22)
#define BUSCON1_BCGEN_RES	(0x3 << 12)
#define BUSCON1_WAITWRC2	(2 << 8)
#define BUSCON1_WAITRDC2	(2 << 6)
#define BUSCON1_HOLDC1		(1 << 4)
#define BUSCON1_RECOVC1		(1 << 2)
#define BUSCON1_CMULT4		1

#define BUSCON0_SETUP		(1 << 22)
#define BUSCON0_ALEC		(2 << 14)
#define BUSCON0_BCGEN_RES	(0x3 << 12)
#define BUSCON0_WAITWRC2	(7 << 8)
#define BUSCON0_WAITRDC2	(3 << 6)
#define BUSCON0_HOLDC1		(3 << 4)
#define BUSCON0_RECOVC1		(3 << 2)
#define BUSCON0_CMULT4		2

#define NAND_CON_CE		(1 << 20)
#define NAND_CON_OUT_CS1	(1 << 10)
#define NAND_CON_IN_CS1		(1 << 8)
#define NAND_CON_PRE_P		(1 << 7)
#define NAND_CON_WP_P		(1 << 6)
#define NAND_CON_SE_P		(1 << 5)
#define NAND_CON_CS_P		(1 << 4)
#define NAND_CON_CSMUX		(1 << 1)
#define NAND_CON_NANDM		1

#define NAND_ALE_SET		ltq_ebu_w32(ltq_ebu_r32(EBU_NAND_CON) | \
							(1 << 18), EBU_NAND_CON);
#define NAND_ALE_CLEAR		ltq_ebu_w32(ltq_ebu_r32(EBU_NAND_CON) & \
							~(1 << 18), EBU_NAND_CON);

#ifndef CONFIG_EVA
#define NANDPHYSADDR(x)    CPHYSADDR(x)
#else
#define NANDPHYSADDR(x) RPHYSADDR(x)
#endif /* CONFIG_EVA */

#define ltq_ebu_w32(x, y)	ltq_w32((x), ltq_ebu_membase + (y))
#define ltq_ebu_r32(x)		ltq_r32(ltq_ebu_membase + (x))
#define ltq_ebu_w32_mask(x, y, z) \
	ltq_w32_mask(x, y, ltq_ebu_membase + (z))

static u32 xway_latchcmd;
static void __iomem *ltq_ebu_membase;

struct xway_nand_data {
	struct nand_chip	chip;
	unsigned long		csflags;
	void __iomem		*nandaddr;
};

static u8 xway_readb(struct mtd_info *mtd, int op)
{
	struct nand_chip *chip = mtd_to_nand(mtd);
	struct xway_nand_data *data = nand_get_controller_data(chip);
	u8 ret;

	ret = readb(data->nandaddr + op);
	while ((ltq_ebu_r32(EBU_NAND_WAIT) & NAND_WAIT_WR_C) == 0)
		;

	return ret;
}

static void xway_writeb(struct mtd_info *mtd, int op, u8 value)
{
	struct nand_chip *chip = mtd_to_nand(mtd);
	struct xway_nand_data *data = nand_get_controller_data(chip);

	writeb(value, data->nandaddr + op);
	while ((ltq_ebu_r32(EBU_NAND_WAIT) & NAND_WAIT_WR_C) == 0)
		;
}

static void xway_select_chip(struct mtd_info *mtd, int select)
{
	switch (select) {
	case -1:
		ltq_ebu_w32_mask(NAND_CON_CE, 0, EBU_NAND_CON);
		ltq_ebu_w32_mask(NAND_CON_NANDM, 0, EBU_NAND_CON);
		break;
	case 0:
		ltq_ebu_w32_mask(0, NAND_CON_NANDM, EBU_NAND_CON);
		ltq_ebu_w32_mask(0, NAND_CON_CE, EBU_NAND_CON);
		break;
	default:
		BUG();
	}
}

static void xway_cmd_ctrl(struct mtd_info *mtd, int cmd, unsigned int ctrl)
{
	if (ctrl & NAND_CTRL_CHANGE) {
		if (ctrl & NAND_CLE) {
			NAND_ALE_CLEAR;
			xway_latchcmd = NAND_WRITE_CMD;
		} else if (ctrl & NAND_ALE) {
			NAND_ALE_SET;
			xway_latchcmd = NAND_WRITE_ADDR;
		} else {
			if (xway_latchcmd == NAND_WRITE_ADDR) {
				NAND_ALE_CLEAR;
				xway_latchcmd = NAND_WRITE_DATA;
			}
		}
	}

	if (cmd != NAND_CMD_NONE)
		xway_writeb(mtd, xway_latchcmd, cmd);
}

static int xway_dev_ready(struct mtd_info *mtd)
{
	return ltq_ebu_r32(EBU_NAND_WAIT) & NAND_WAIT_RD;
}

static unsigned char xway_read_byte(struct mtd_info *mtd)
{
	return xway_readb(mtd, NAND_READ_DATA);
}

static void xway_read_buf(struct mtd_info *mtd, u_char *buf, int len)
{
	int i;

	for (i = 0; i < len; i++)
		buf[i] = xway_readb(mtd, NAND_WRITE_DATA);
}

static void xway_write_buf(struct mtd_info *mtd, const u_char *buf, int len)
{
	int i;

	for (i = 0; i < len; i++)
		xway_writeb(mtd, NAND_WRITE_DATA, buf[i]);
}

static int xway_ebu_probe(void)
{
	struct resource res_ebu;
	struct device_node *np_ebu = NULL;

	if (of_machine_is_compatible("lantiq,grx500"))
		np_ebu = of_find_compatible_node(NULL, NULL, "lantiq,ebu-grx500");
	else
		np_ebu = of_find_compatible_node(NULL, NULL, "lantiq,ebu-xway");

	/* check if all the core register ranges are available */
	if (!np_ebu) {
		pr_err("Failed to load core nodes from devicetree");
		return -EINVAL;
	}

	BUG_ON(of_address_to_resource(np_ebu, 0, &res_ebu));

	BUG_ON(!request_mem_region(res_ebu.start,
		resource_size(&res_ebu), res_ebu.name));
		
	ltq_ebu_membase = ioremap_nocache(res_ebu.start,
		resource_size(&res_ebu));
	BUG_ON(!ltq_ebu_membase);
	
	ltq_ebu_w32(ltq_ebu_r32(LTQ_EBU_BUSCON0) & ~EBU_WRDIS,
			LTQ_EBU_BUSCON0);

	return 0;
}

/*
 * Probe for the NAND device.
 */
static int xway_nand_probe(struct platform_device *pdev)
{
	struct xway_nand_data *data;
	struct mtd_info *mtd;
	struct resource *res;
	static const char *pm;
	int err;
	u32 cs;
	u32 cs_flag = 0;

	err = xway_ebu_probe();
	if (err)
		return -EINVAL;

	/* Allocate memory for the device structure (and zero it) */
	data = devm_kzalloc(&pdev->dev, sizeof(struct xway_nand_data),
			    GFP_KERNEL);
	if (!data)
		return -ENOMEM;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	data->nandaddr = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(data->nandaddr))
		return PTR_ERR(data->nandaddr);

	nand_set_flash_node(&data->chip, pdev->dev.of_node);
	mtd = nand_to_mtd(&data->chip);
	mtd->dev.parent = &pdev->dev;

	data->chip.cmd_ctrl = xway_cmd_ctrl;
	data->chip.dev_ready = xway_dev_ready;
	data->chip.select_chip = xway_select_chip;
	data->chip.write_buf = xway_write_buf;
	data->chip.read_buf = xway_read_buf;
	data->chip.read_byte = xway_read_byte;
	data->chip.chip_delay = 30;

	err = of_property_read_string(pdev->dev.of_node, "nand-ecc-algo", &pm);
	if (err) {
		pr_err("nand-ecc-algo not set in device tree\n");
	}

	/* BCH is used mainly on MLC NAND flashes which does
	 * not like SUBPAGE writes 
	 */
	if (!strcasecmp(pm, "bch"))
		data->chip.options |= NAND_NO_SUBPAGE_WRITE;	

	platform_set_drvdata(pdev, data);
	nand_set_controller_data(&data->chip, data);

	/* load our CS from the DT. Either we find a valid 1 or default to 0 */
	err = of_property_read_u32(pdev->dev.of_node, "lantiq,cs", &cs);
	if (!err && cs == 1) {
		cs_flag = NAND_CON_IN_CS1 | NAND_CON_OUT_CS1;

		/* setup the EBU to run in NAND mode on our base addr */
		if (of_machine_is_compatible("lantiq,vr9")) {
			ltq_ebu_w32(NANDPHYSADDR(data->nandaddr)
				| ADDSEL1_MASK(3) | ADDSEL1_REGEN, EBU_ADDSEL1);
		} else if (of_machine_is_compatible("lantiq,grx500")) {
			ltq_ebu_w32(NANDPHYSADDR(data->nandaddr)
			| ADDSEL1_MASK(5) | ADDSEL1_REGEN, EBU_ADDSEL1);
		} else {
			ltq_ebu_w32(NANDPHYSADDR(data->nandaddr)
				| ADDSEL1_MASK(2) | ADDSEL1_REGEN, EBU_ADDSEL1);
		}

		ltq_ebu_w32(BUSCON1_SETUP | BUSCON1_BCGEN_RES | BUSCON1_WAITWRC2
			    | BUSCON1_WAITRDC2 | BUSCON1_HOLDC1 | BUSCON1_RECOVC1
			    | BUSCON1_CMULT4, LTQ_EBU_BUSCON1);

		ltq_ebu_w32(NAND_CON_NANDM | NAND_CON_CSMUX | NAND_CON_CS_P
			    | NAND_CON_SE_P | NAND_CON_WP_P | NAND_CON_PRE_P
			    | cs_flag, EBU_NAND_CON);
	} else if (!err && cs == 0) {
		ltq_ebu_w32(NANDPHYSADDR(data->nandaddr)
				|  ADDSEL1_MASK(1) | ADDSEL0_REGEN, EBU_ADDSEL0);

		ltq_ebu_w32(BUSCON0_SETUP | BUSCON0_ALEC | BUSCON0_BCGEN_RES
				| BUSCON0_WAITWRC2 | BUSCON0_WAITRDC2
				| BUSCON0_HOLDC1 | BUSCON0_RECOVC1
				| BUSCON0_CMULT4, LTQ_EBU_BUSCON0);

		ltq_ebu_w32(NAND_CON_CSMUX | NAND_CON_CS_P
				| NAND_CON_SE_P | NAND_CON_WP_P
				| NAND_CON_PRE_P, EBU_NAND_CON);
	} else {
		pr_err("Platform does not support chip select %d\n", cs_flag);
	}

	/* Scan to find existence of the device */
	err = nand_scan(mtd, 1);
	if (err)
		return err;

	err = mtd_device_register(mtd, NULL, 0);
	if (err)
		nand_release(mtd);

	return err;
}

/*
 * Remove a NAND device.
 */
static int xway_nand_remove(struct platform_device *pdev)
{
	struct xway_nand_data *data = platform_get_drvdata(pdev);

	nand_release(nand_to_mtd(&data->chip));

	return 0;
}

static const struct of_device_id xway_nand_match[] = {
	{ .compatible = "lantiq,nand-xway" },
	{},
};

static struct platform_driver xway_nand_driver = {
	.probe	= xway_nand_probe,
	.remove	= xway_nand_remove,
	.driver	= {
		.name		= "lantiq,nand-xway",
		.of_match_table = xway_nand_match,
	},
};

builtin_platform_driver(xway_nand_driver);
