/*
 * Flash partitions described by the OF (or flattened) device tree
 *
 * Copyright © 2006 MontaVista Software Inc.
 * Author: Vitaly Wool <vwool@ru.mvista.com>
 *
 * Revised to handle newer style flash binding by:
 *   Copyright © 2007 David Gibson, IBM Corporation.
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/of.h>
#include <linux/mtd/mtd.h>
#include <linux/slab.h>
#include <linux/mtd/partitions.h>

static bool node_has_compatible(struct device_node *pp)
{
	return of_get_property(pp, "compatible", NULL);
}

#if defined(RAX120) || defined(SWRT360V6)
struct dni_nand_partition_entry {
	u32 offset; /* bytes */
	u32 size;   /* bytes */
	char    *name;
};

struct dni_nand_partition_entry dni_nand_partition[] = {
#if defined(RAX120)
	{ 0x00000000, 0x00F80000, "Bootloader" },
	{ 0x00d00000, 0x00080000, "appsblenv" },
	{ 0x00f80000, 0x00080000, "art" },
	{ 0x01180000, 0x00100000, "boarddata" },
	{ 0x01980000, 0x06400000, "firmware" },
	{ 0x01980000, 0x00620000, "kernel" },
	{ 0x01fa0000, 0x05de0000, "rootfs" },
	{ 0x0e880000, 0x11760000, "UBI_DEV" }
#elif defined(SWRT360V6)
	{ 0x00000000, 0x00180000, "0:SBL1" },
	{ 0x00180000, 0x00100000, "0:MIBIB" },
	{ 0x00280000, 0x00080000, "0:BOOTCONFIG" },
	{ 0x00300000, 0x00080000, "0:BOOTCONFIG1" },
	{ 0x00380000, 0x00380000, "0:QSEE" },
	{ 0x00700000, 0x00380000, "0:QSEE_1" },
	{ 0x00A80000, 0x00080000, "0:DEVCFG" },
	{ 0x00B00000, 0x00080000, "0:DEVCFG_1" },
	{ 0x00B80000, 0x00080000, "0:RPM" },
	{ 0x00C00000, 0x00080000, "0:RPM_1" },
	{ 0x00C80000, 0x00080000, "0:CDT" },
	{ 0x00D00000, 0x00080000, "0:CDT_1" },
	{ 0x00D80000, 0x00080000, "0:APPSBLENV" },
	{ 0x00E00000, 0x00180000, "0:APPSBL" },
	{ 0x00F80000, 0x00180000, "0:APPSBL_1" },
	{ 0x01100000, 0x00080000, "0:ART" },
//	{ 0x01180000, 0x02CA0000, "firmware" },
//	{ 0x03E20000, 0x03520000, "UBI_DEV" },
	{ 0x01180000, 0x05940000, "UBI_DEV" },
	{ 0x06AC0000, 0x00880000, "jffsconcat1" },
	{ 0x07340000, 0x00CC0000, "BACKUP" }
#endif
};
#endif

static int parse_ofpart_partitions(struct mtd_info *master,
				   struct mtd_partition **pparts,
				   struct mtd_part_parser_data *data)
{
	struct device_node *mtd_node;
	struct device_node *ofpart_node;
	const char *partname;
	struct device_node *pp;
	int nr_parts, i, ret = 0;
	bool dedicated = true;
#if defined(RAX120) || defined(SWRT360V6)
	int count;
#endif

	if (!data)
		return 0;

	mtd_node = data->of_node;
	if (!mtd_node)
		return 0;

	ofpart_node = of_get_child_by_name(mtd_node, "partitions");
	if (!ofpart_node) {
		/*
		 * We might get here even when ofpart isn't used at all (e.g.,
		 * when using another parser), so don't be louder than
		 * KERN_DEBUG
		 */
		pr_debug("%s: 'partitions' subnode not found on %s. Trying to parse direct subnodes as partitions.\n",
			 master->name, mtd_node->full_name);
		ofpart_node = mtd_node;
		dedicated = false;
	} else if (!of_device_is_compatible(ofpart_node, "fixed-partitions")) {
		/* The 'partitions' subnode might be used by another parser */
		return 0;
	}

	/* First count the subnodes */
	nr_parts = 0;
	for_each_child_of_node(ofpart_node,  pp) {
		if (!dedicated && node_has_compatible(pp))
			continue;

		nr_parts++;
	}

	if (nr_parts == 0)
		return 0;
#if defined(RAX120)
	*pparts = kzalloc(35 * sizeof(**pparts), GFP_KERNEL);
#elif defined(SWRT360V6)
	*pparts = kzalloc(24 * sizeof(**pparts), GFP_KERNEL);
#else
	*pparts = kzalloc(nr_parts * sizeof(**pparts), GFP_KERNEL);
#endif
	if (!*pparts)
		return -ENOMEM;

	i = 0;
	for_each_child_of_node(ofpart_node,  pp) {
		const __be32 *reg;
		int len;
		int a_cells, s_cells;

		if (!dedicated && node_has_compatible(pp))
			continue;

		reg = of_get_property(pp, "reg", &len);
		if (!reg) {
			if (dedicated) {
				pr_debug("%s: ofpart partition %s (%s) missing reg property.\n",
					 master->name, pp->full_name,
					 mtd_node->full_name);
				goto ofpart_fail;
			} else {
				nr_parts--;
				continue;
			}
		}

		a_cells = of_n_addr_cells(pp);
		s_cells = of_n_size_cells(pp);
		if (len / 4 != a_cells + s_cells) {
			pr_debug("%s: ofpart partition %s (%s) error parsing reg property.\n",
				 master->name, pp->full_name,
				 mtd_node->full_name);
			goto ofpart_fail;
		}

		(*pparts)[i].offset = of_read_number(reg, a_cells);
		(*pparts)[i].size = of_read_number(reg + a_cells, s_cells);

		partname = of_get_property(pp, "label", &len);
		if (!partname)
			partname = of_get_property(pp, "name", &len);
		(*pparts)[i].name = partname;

		if (of_get_property(pp, "read-only", &len))
			(*pparts)[i].mask_flags |= MTD_WRITEABLE;

		if (of_get_property(pp, "lock", &len))
			(*pparts)[i].mask_flags |= MTD_POWERUP_LOCK;

		i++;
	}

	if (!nr_parts)
		goto ofpart_none;

#if defined(RAX120)
    	/* default get 8 mtd partitions.
     	* 0	Bootloader
     	* 1	appsblenv
     	* 2	art
     	* 3	boarddata
     	* 4	firmware
     	* 5	kernel
     	* 6	rootfs
     	* 7	UBI_DEV
     	*/

	nr_parts = 0;
	count = sizeof(dni_nand_partition)/sizeof(struct dni_nand_partition_entry);
	for (i = 0; i < count; i++, nr_parts++) {
		(*pparts)[nr_parts].offset = dni_nand_partition[i].offset;
		(*pparts)[nr_parts].size = dni_nand_partition[i].size;
		(*pparts)[nr_parts].name = dni_nand_partition[i].name;
		if(i == 2 || i == 3)
			(*pparts)[nr_parts].mask_flags |= MTD_WRITEABLE;
	}
#elif defined(SWRT360V6)
	nr_parts = 0;
	count = sizeof(dni_nand_partition)/sizeof(struct dni_nand_partition_entry);
	for (i = 0; i < count; i++, nr_parts++) {
		(*pparts)[nr_parts].offset = dni_nand_partition[i].offset;
		(*pparts)[nr_parts].size = dni_nand_partition[i].size;
		(*pparts)[nr_parts].name = dni_nand_partition[i].name;
	}
	(*pparts)[nr_parts].mask_flags |= MTD_WRITEABLE;//lock factory
#endif
	return nr_parts;

ofpart_fail:
	pr_err("%s: error parsing ofpart partition %s (%s)\n",
	       master->name, pp->full_name, mtd_node->full_name);
	ret = -EINVAL;
ofpart_none:
	of_node_put(pp);
	kfree(*pparts);
	*pparts = NULL;
	return ret;
}

static struct mtd_part_parser ofpart_parser = {
	.owner = THIS_MODULE,
	.parse_fn = parse_ofpart_partitions,
	.name = "ofpart",
};

static int parse_ofoldpart_partitions(struct mtd_info *master,
				      struct mtd_partition **pparts,
				      struct mtd_part_parser_data *data)
{
	struct device_node *dp;
	int i, plen, nr_parts;
	const struct {
		__be32 offset, len;
	} *part;
	const char *names;

	if (!data)
		return 0;

	dp = data->of_node;
	if (!dp)
		return 0;

	part = of_get_property(dp, "partitions", &plen);
	if (!part)
		return 0; /* No partitions found */

	pr_warning("Device tree uses obsolete partition map binding: %s\n",
			dp->full_name);

	nr_parts = plen / sizeof(part[0]);

	*pparts = kzalloc(nr_parts * sizeof(*(*pparts)), GFP_KERNEL);
	if (!*pparts)
		return -ENOMEM;

	names = of_get_property(dp, "partition-names", &plen);

	for (i = 0; i < nr_parts; i++) {
		(*pparts)[i].offset = be32_to_cpu(part->offset);
		(*pparts)[i].size   = be32_to_cpu(part->len) & ~1;
		/* bit 0 set signifies read only partition */
		if (be32_to_cpu(part->len) & 1)
			(*pparts)[i].mask_flags = MTD_WRITEABLE;

		if (names && (plen > 0)) {
			int len = strlen(names) + 1;

			(*pparts)[i].name = names;
			plen -= len;
			names += len;
		} else {
			(*pparts)[i].name = "unnamed";
		}

		part++;
	}

	return nr_parts;
}

static struct mtd_part_parser ofoldpart_parser = {
	.owner = THIS_MODULE,
	.parse_fn = parse_ofoldpart_partitions,
	.name = "ofoldpart",
};

static int __init ofpart_parser_init(void)
{
	register_mtd_parser(&ofpart_parser);
	register_mtd_parser(&ofoldpart_parser);
	return 0;
}

static void __exit ofpart_parser_exit(void)
{
	deregister_mtd_parser(&ofpart_parser);
	deregister_mtd_parser(&ofoldpart_parser);
}

module_init(ofpart_parser_init);
module_exit(ofpart_parser_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Parser for MTD partitioning information in device tree");
MODULE_AUTHOR("Vitaly Wool, David Gibson");
/*
 * When MTD core cannot find the requested parser, it tries to load the module
 * with the same name. Since we provide the ofoldpart parser, we should have
 * the corresponding alias.
 */
MODULE_ALIAS("ofoldpart");
