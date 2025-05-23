// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Core registration and callback routines for MTD
 * drivers and users.
 *
 * Copyright © 1999-2010 David Woodhouse <dwmw2@infradead.org>
 * Copyright © 2006      Red Hat UK Limited 
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/ptrace.h>
#include <linux/seq_file.h>
#include <linux/string.h>
#include <linux/timer.h>
#include <linux/major.h>
#include <linux/fs.h>
#include <linux/err.h>
#include <linux/ioctl.h>
#include <linux/init.h>
#include <linux/of.h>
#include <linux/proc_fs.h>
#include <linux/idr.h>
#include <linux/backing-dev.h>
#include <linux/gfp.h>
#include <linux/slab.h>
#include <linux/reboot.h>
#include <linux/leds.h>
#include <linux/debugfs.h>
#include <linux/root_dev.h>
#include <linux/nvmem-provider.h>

#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>

#include "mtdcore.h"

struct backing_dev_info *mtd_bdi;

#ifdef CONFIG_PM_SLEEP

static int mtd_cls_suspend(struct device *dev)
{
	struct mtd_info *mtd = dev_get_drvdata(dev);

	return mtd ? mtd_suspend(mtd) : 0;
}

static int mtd_cls_resume(struct device *dev)
{
	struct mtd_info *mtd = dev_get_drvdata(dev);

	if (mtd)
		mtd_resume(mtd);
	return 0;
}

static SIMPLE_DEV_PM_OPS(mtd_cls_pm_ops, mtd_cls_suspend, mtd_cls_resume);
#define MTD_CLS_PM_OPS (&mtd_cls_pm_ops)
#else
#define MTD_CLS_PM_OPS NULL
#endif

static struct class mtd_class = {
	.name = "mtd",
	.owner = THIS_MODULE,
	.pm = MTD_CLS_PM_OPS,
};

static DEFINE_IDR(mtd_idr);

/* These are exported solely for the purpose of mtd_blkdevs.c. You
   should not use them for _anything_ else */
DEFINE_MUTEX(mtd_table_mutex);
EXPORT_SYMBOL_GPL(mtd_table_mutex);

struct mtd_info *__mtd_next_device(int i)
{
	return idr_get_next(&mtd_idr, &i);
}
EXPORT_SYMBOL_GPL(__mtd_next_device);

static LIST_HEAD(mtd_notifiers);


#define MTD_DEVT(index) MKDEV(MTD_CHAR_MAJOR, (index)*2)

/* REVISIT once MTD uses the driver model better, whoever allocates
 * the mtd_info will probably want to use the release() hook...
 */
static void mtd_release(struct device *dev)
{
	struct mtd_info *mtd = dev_get_drvdata(dev);
	dev_t index = MTD_DEVT(mtd->index);

	/* remove /dev/mtdXro node */
	device_destroy(&mtd_class, index + 1);
}

static ssize_t mtd_type_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct mtd_info *mtd = dev_get_drvdata(dev);
	char *type;

	switch (mtd->type) {
	case MTD_ABSENT:
		type = "absent";
		break;
	case MTD_RAM:
		type = "ram";
		break;
	case MTD_ROM:
		type = "rom";
		break;
	case MTD_NORFLASH:
		type = "nor";
		break;
	case MTD_NANDFLASH:
		type = "nand";
		break;
	case MTD_DATAFLASH:
		type = "dataflash";
		break;
	case MTD_UBIVOLUME:
		type = "ubi";
		break;
	case MTD_MLCNANDFLASH:
		type = "mlc-nand";
		break;
	default:
		type = "unknown";
	}

	return snprintf(buf, PAGE_SIZE, "%s\n", type);
}
static DEVICE_ATTR(type, S_IRUGO, mtd_type_show, NULL);

static ssize_t mtd_flags_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct mtd_info *mtd = dev_get_drvdata(dev);

	return snprintf(buf, PAGE_SIZE, "0x%lx\n", (unsigned long)mtd->flags);
}
static DEVICE_ATTR(flags, S_IRUGO, mtd_flags_show, NULL);

static ssize_t mtd_size_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct mtd_info *mtd = dev_get_drvdata(dev);

	return snprintf(buf, PAGE_SIZE, "%llu\n",
		(unsigned long long)mtd->size);
}
static DEVICE_ATTR(size, S_IRUGO, mtd_size_show, NULL);

static ssize_t mtd_erasesize_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct mtd_info *mtd = dev_get_drvdata(dev);

	return snprintf(buf, PAGE_SIZE, "%lu\n", (unsigned long)mtd->erasesize);
}
static DEVICE_ATTR(erasesize, S_IRUGO, mtd_erasesize_show, NULL);

static ssize_t mtd_writesize_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct mtd_info *mtd = dev_get_drvdata(dev);

	return snprintf(buf, PAGE_SIZE, "%lu\n", (unsigned long)mtd->writesize);
}
static DEVICE_ATTR(writesize, S_IRUGO, mtd_writesize_show, NULL);

static ssize_t mtd_subpagesize_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct mtd_info *mtd = dev_get_drvdata(dev);
	unsigned int subpagesize = mtd->writesize >> mtd->subpage_sft;

	return snprintf(buf, PAGE_SIZE, "%u\n", subpagesize);
}
static DEVICE_ATTR(subpagesize, S_IRUGO, mtd_subpagesize_show, NULL);

static ssize_t mtd_oobsize_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct mtd_info *mtd = dev_get_drvdata(dev);

	return snprintf(buf, PAGE_SIZE, "%lu\n", (unsigned long)mtd->oobsize);
}
static DEVICE_ATTR(oobsize, S_IRUGO, mtd_oobsize_show, NULL);

static ssize_t mtd_oobavail_show(struct device *dev,
				 struct device_attribute *attr, char *buf)
{
	struct mtd_info *mtd = dev_get_drvdata(dev);

	return snprintf(buf, PAGE_SIZE, "%u\n", mtd->oobavail);
}
static DEVICE_ATTR(oobavail, S_IRUGO, mtd_oobavail_show, NULL);

static ssize_t mtd_numeraseregions_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct mtd_info *mtd = dev_get_drvdata(dev);

	return snprintf(buf, PAGE_SIZE, "%u\n", mtd->numeraseregions);
}
static DEVICE_ATTR(numeraseregions, S_IRUGO, mtd_numeraseregions_show,
	NULL);

static ssize_t mtd_name_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct mtd_info *mtd = dev_get_drvdata(dev);

	return snprintf(buf, PAGE_SIZE, "%s\n", mtd->name);
}
static DEVICE_ATTR(name, S_IRUGO, mtd_name_show, NULL);

static ssize_t mtd_ecc_strength_show(struct device *dev,
				     struct device_attribute *attr, char *buf)
{
	struct mtd_info *mtd = dev_get_drvdata(dev);

	return snprintf(buf, PAGE_SIZE, "%u\n", mtd->ecc_strength);
}
static DEVICE_ATTR(ecc_strength, S_IRUGO, mtd_ecc_strength_show, NULL);

static ssize_t mtd_bitflip_threshold_show(struct device *dev,
					  struct device_attribute *attr,
					  char *buf)
{
	struct mtd_info *mtd = dev_get_drvdata(dev);

	return snprintf(buf, PAGE_SIZE, "%u\n", mtd->bitflip_threshold);
}

static ssize_t mtd_bitflip_threshold_store(struct device *dev,
					   struct device_attribute *attr,
					   const char *buf, size_t count)
{
	struct mtd_info *mtd = dev_get_drvdata(dev);
	unsigned int bitflip_threshold;
	int retval;

	retval = kstrtouint(buf, 0, &bitflip_threshold);
	if (retval)
		return retval;

	mtd->bitflip_threshold = bitflip_threshold;
	return count;
}
static DEVICE_ATTR(bitflip_threshold, S_IRUGO | S_IWUSR,
		   mtd_bitflip_threshold_show,
		   mtd_bitflip_threshold_store);

static ssize_t mtd_ecc_step_size_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct mtd_info *mtd = dev_get_drvdata(dev);

	return snprintf(buf, PAGE_SIZE, "%u\n", mtd->ecc_step_size);

}
static DEVICE_ATTR(ecc_step_size, S_IRUGO, mtd_ecc_step_size_show, NULL);

static ssize_t mtd_ecc_stats_corrected_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct mtd_info *mtd = dev_get_drvdata(dev);
	struct mtd_ecc_stats *ecc_stats = &mtd->ecc_stats;

	return snprintf(buf, PAGE_SIZE, "%u\n", ecc_stats->corrected);
}
static DEVICE_ATTR(corrected_bits, S_IRUGO,
		   mtd_ecc_stats_corrected_show, NULL);

static ssize_t mtd_ecc_stats_errors_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct mtd_info *mtd = dev_get_drvdata(dev);
	struct mtd_ecc_stats *ecc_stats = &mtd->ecc_stats;

	return snprintf(buf, PAGE_SIZE, "%u\n", ecc_stats->failed);
}
static DEVICE_ATTR(ecc_failures, S_IRUGO, mtd_ecc_stats_errors_show, NULL);

static ssize_t mtd_badblocks_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct mtd_info *mtd = dev_get_drvdata(dev);
	struct mtd_ecc_stats *ecc_stats = &mtd->ecc_stats;

	return snprintf(buf, PAGE_SIZE, "%u\n", ecc_stats->badblocks);
}
static DEVICE_ATTR(bad_blocks, S_IRUGO, mtd_badblocks_show, NULL);

static ssize_t mtd_bbtblocks_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct mtd_info *mtd = dev_get_drvdata(dev);
	struct mtd_ecc_stats *ecc_stats = &mtd->ecc_stats;

	return snprintf(buf, PAGE_SIZE, "%u\n", ecc_stats->bbtblocks);
}
static DEVICE_ATTR(bbt_blocks, S_IRUGO, mtd_bbtblocks_show, NULL);

static struct attribute *mtd_attrs[] = {
	&dev_attr_type.attr,
	&dev_attr_flags.attr,
	&dev_attr_size.attr,
	&dev_attr_erasesize.attr,
	&dev_attr_writesize.attr,
	&dev_attr_subpagesize.attr,
	&dev_attr_oobsize.attr,
	&dev_attr_oobavail.attr,
	&dev_attr_numeraseregions.attr,
	&dev_attr_name.attr,
	&dev_attr_ecc_strength.attr,
	&dev_attr_ecc_step_size.attr,
	&dev_attr_corrected_bits.attr,
	&dev_attr_ecc_failures.attr,
	&dev_attr_bad_blocks.attr,
	&dev_attr_bbt_blocks.attr,
	&dev_attr_bitflip_threshold.attr,
	NULL,
};
ATTRIBUTE_GROUPS(mtd);

static const struct device_type mtd_devtype = {
	.name		= "mtd",
	.groups		= mtd_groups,
	.release	= mtd_release,
};

static int mtd_partid_show(struct seq_file *s, void *p)
{
	struct mtd_info *mtd = s->private;

	seq_printf(s, "%s\n", mtd->dbg.partid);

	return 0;
}

static int mtd_partid_debugfs_open(struct inode *inode, struct file *file)
{
	return single_open(file, mtd_partid_show, inode->i_private);
}

static const struct file_operations mtd_partid_debug_fops = {
	.open           = mtd_partid_debugfs_open,
	.read           = seq_read,
	.llseek         = seq_lseek,
	.release        = single_release,
};

static int mtd_partname_show(struct seq_file *s, void *p)
{
	struct mtd_info *mtd = s->private;

	seq_printf(s, "%s\n", mtd->dbg.partname);

	return 0;
}

static int mtd_partname_debugfs_open(struct inode *inode, struct file *file)
{
	return single_open(file, mtd_partname_show, inode->i_private);
}

static const struct file_operations mtd_partname_debug_fops = {
	.open           = mtd_partname_debugfs_open,
	.read           = seq_read,
	.llseek         = seq_lseek,
	.release        = single_release,
};

static struct dentry *dfs_dir_mtd;

static void mtd_debugfs_populate(struct mtd_info *mtd)
{
	struct device *dev = &mtd->dev;
	struct dentry *root, *dent;

	if (IS_ERR_OR_NULL(dfs_dir_mtd))
		return;

	root = debugfs_create_dir(dev_name(dev), dfs_dir_mtd);
	if (IS_ERR_OR_NULL(root)) {
		dev_dbg(dev, "won't show data in debugfs\n");
		return;
	}

	mtd->dbg.dfs_dir = root;

	if (mtd->dbg.partid) {
		dent = debugfs_create_file("partid", 0400, root, mtd,
					   &mtd_partid_debug_fops);
		if (IS_ERR_OR_NULL(dent))
			dev_err(dev, "can't create debugfs entry for partid\n");
	}

	if (mtd->dbg.partname) {
		dent = debugfs_create_file("partname", 0400, root, mtd,
					   &mtd_partname_debug_fops);
		if (IS_ERR_OR_NULL(dent))
			dev_err(dev,
				"can't create debugfs entry for partname\n");
	}
}

#ifndef CONFIG_MMU
unsigned mtd_mmap_capabilities(struct mtd_info *mtd)
{
	switch (mtd->type) {
	case MTD_RAM:
		return NOMMU_MAP_COPY | NOMMU_MAP_DIRECT | NOMMU_MAP_EXEC |
			NOMMU_MAP_READ | NOMMU_MAP_WRITE;
	case MTD_ROM:
		return NOMMU_MAP_COPY | NOMMU_MAP_DIRECT | NOMMU_MAP_EXEC |
			NOMMU_MAP_READ;
	default:
		return NOMMU_MAP_COPY;
	}
}
EXPORT_SYMBOL_GPL(mtd_mmap_capabilities);
#endif

static int mtd_reboot_notifier(struct notifier_block *n, unsigned long state,
			       void *cmd)
{
	struct mtd_info *mtd;

	mtd = container_of(n, struct mtd_info, reboot_notifier);
	mtd->_reboot(mtd);

	return NOTIFY_DONE;
}

/**
 * mtd_wunit_to_pairing_info - get pairing information of a wunit
 * @mtd: pointer to new MTD device info structure
 * @wunit: write unit we are interested in
 * @info: returned pairing information
 *
 * Retrieve pairing information associated to the wunit.
 * This is mainly useful when dealing with MLC/TLC NANDs where pages can be
 * paired together, and where programming a page may influence the page it is
 * paired with.
 * The notion of page is replaced by the term wunit (write-unit) to stay
 * consistent with the ->writesize field.
 *
 * The @wunit argument can be extracted from an absolute offset using
 * mtd_offset_to_wunit(). @info is filled with the pairing information attached
 * to @wunit.
 *
 * From the pairing info the MTD user can find all the wunits paired with
 * @wunit using the following loop:
 *
 * for (i = 0; i < mtd_pairing_groups(mtd); i++) {
 *	info.pair = i;
 *	mtd_pairing_info_to_wunit(mtd, &info);
 *	...
 * }
 */
int mtd_wunit_to_pairing_info(struct mtd_info *mtd, int wunit,
			      struct mtd_pairing_info *info)
{
	int npairs = mtd_wunit_per_eb(mtd) / mtd_pairing_groups(mtd);

	if (wunit < 0 || wunit >= npairs)
		return -EINVAL;

	if (mtd->pairing && mtd->pairing->get_info)
		return mtd->pairing->get_info(mtd, wunit, info);

	info->group = 0;
	info->pair = wunit;

	return 0;
}
EXPORT_SYMBOL_GPL(mtd_wunit_to_pairing_info);

/**
 * mtd_pairing_info_to_wunit - get wunit from pairing information
 * @mtd: pointer to new MTD device info structure
 * @info: pairing information struct
 *
 * Returns a positive number representing the wunit associated to the info
 * struct, or a negative error code.
 *
 * This is the reverse of mtd_wunit_to_pairing_info(), and can help one to
 * iterate over all wunits of a given pair (see mtd_wunit_to_pairing_info()
 * doc).
 *
 * It can also be used to only program the first page of each pair (i.e.
 * page attached to group 0), which allows one to use an MLC NAND in
 * software-emulated SLC mode:
 *
 * info.group = 0;
 * npairs = mtd_wunit_per_eb(mtd) / mtd_pairing_groups(mtd);
 * for (info.pair = 0; info.pair < npairs; info.pair++) {
 *	wunit = mtd_pairing_info_to_wunit(mtd, &info);
 *	mtd_write(mtd, mtd_wunit_to_offset(mtd, blkoffs, wunit),
 *		  mtd->writesize, &retlen, buf + (i * mtd->writesize));
 * }
 */
int mtd_pairing_info_to_wunit(struct mtd_info *mtd,
			      const struct mtd_pairing_info *info)
{
	int ngroups = mtd_pairing_groups(mtd);
	int npairs = mtd_wunit_per_eb(mtd) / ngroups;

	if (!info || info->pair < 0 || info->pair >= npairs ||
	    info->group < 0 || info->group >= ngroups)
		return -EINVAL;

	if (mtd->pairing && mtd->pairing->get_wunit)
		return mtd->pairing->get_wunit(mtd, info);

	return info->pair;
}
EXPORT_SYMBOL_GPL(mtd_pairing_info_to_wunit);

/**
 * mtd_pairing_groups - get the number of pairing groups
 * @mtd: pointer to new MTD device info structure
 *
 * Returns the number of pairing groups.
 *
 * This number is usually equal to the number of bits exposed by a single
 * cell, and can be used in conjunction with mtd_pairing_info_to_wunit()
 * to iterate over all pages of a given pair.
 */
int mtd_pairing_groups(struct mtd_info *mtd)
{
	if (!mtd->pairing || !mtd->pairing->ngroups)
		return 1;

	return mtd->pairing->ngroups;
}
EXPORT_SYMBOL_GPL(mtd_pairing_groups);

static int mtd_nvmem_reg_read(void *priv, unsigned int offset,
			      void *val, size_t bytes)
{
	struct mtd_info *mtd = priv;
	size_t retlen;
	int err;

	err = mtd_read(mtd, offset, bytes, &retlen, val);
	if (err && err != -EUCLEAN)
		return err;

	return retlen == bytes ? 0 : -EIO;
}

static int mtd_nvmem_add(struct mtd_info *mtd)
{
	struct device_node *node = mtd_get_of_node(mtd);
	struct nvmem_config config = {};

	config.id = -1;
	config.dev = &mtd->dev;
	config.name = dev_name(&mtd->dev);
	config.owner = THIS_MODULE;
	config.reg_read = mtd_nvmem_reg_read;
	config.size = mtd->size;
	config.word_size = 1;
	config.stride = 1;
	config.read_only = true;
	config.root_only = true;
	config.no_of_node = !of_device_is_compatible(node, "nvmem-cells");
	config.priv = mtd;

	mtd->nvmem = nvmem_register(&config);
	if (IS_ERR(mtd->nvmem)) {
		/* Just ignore if there is no NVMEM support in the kernel */
		if (PTR_ERR(mtd->nvmem) == -EOPNOTSUPP) {
			mtd->nvmem = NULL;
		} else {
			dev_err(&mtd->dev, "Failed to register NVMEM device\n");
			return PTR_ERR(mtd->nvmem);
		}
	}

	return 0;
}

static void mtd_check_of_node(struct mtd_info *mtd)
{
	struct device_node *partitions, *parent_dn, *mtd_dn = NULL;
	const char *pname, *prefix = "partition-";
	int plen, mtd_name_len, offset, prefix_len;

	/* Check if MTD already has a device node */
	if (mtd_get_of_node(mtd))
		return;

	if (!mtd_is_partition(mtd))
		return;

	parent_dn = of_node_get(mtd_get_of_node(mtd_get_master(mtd)));
	if (!parent_dn)
		return;

	if (mtd_is_partition(mtd_get_master(mtd)))
		partitions = of_node_get(parent_dn);
	else
		partitions = of_get_child_by_name(parent_dn, "partitions");
	if (!partitions)
		goto exit_parent;

	prefix_len = strlen(prefix);
	mtd_name_len = strlen(mtd->name);

	/* Search if a partition is defined with the same name */
	for_each_child_of_node(partitions, mtd_dn) {
		/* Skip partition with no/wrong prefix */
		if (!of_node_name_prefix(mtd_dn, prefix))
			continue;

		/* Label have priority. Check that first */
		if (!of_property_read_string(mtd_dn, "label", &pname)) {
			offset = 0;
		} else {
			pname = mtd_dn->name;
			offset = prefix_len;
		}

		plen = strlen(pname) - offset;
		if (plen == mtd_name_len &&
		    !strncmp(mtd->name, pname + offset, plen)) {
			mtd_set_of_node(mtd, mtd_dn);
			break;
		}
	}

	of_node_put(partitions);
exit_parent:
	of_node_put(parent_dn);
}

/**
 *	add_mtd_device - register an MTD device
 *	@mtd: pointer to new MTD device info structure
 *
 *	Add a device to the list of MTD devices present in the system, and
 *	notify each currently active MTD 'user' of its arrival. Returns
 *	zero on success or non-zero on failure.
 */

int add_mtd_device(struct mtd_info *mtd)
{
	struct mtd_notifier *not;
	int i, error;

	/*
	 * May occur, for instance, on buggy drivers which call
	 * mtd_device_parse_register() multiple times on the same master MTD,
	 * especially with CONFIG_MTD_PARTITIONED_MASTER=y.
	 */
	if (WARN_ONCE(mtd->dev.type, "MTD already registered\n"))
		return -EEXIST;

	BUG_ON(mtd->writesize == 0);

	/*
	 * MTD drivers should implement ->_{write,read}() or
	 * ->_{write,read}_oob(), but not both.
	 */
	if (WARN_ON((mtd->_write && mtd->_write_oob) ||
		    (mtd->_read && mtd->_read_oob)))
		return -EINVAL;

	if (WARN_ON((!mtd->erasesize || !mtd->_erase) &&
		    !(mtd->flags & MTD_NO_ERASE)))
		return -EINVAL;

	mutex_lock(&mtd_table_mutex);

	i = idr_alloc(&mtd_idr, mtd, 0, 0, GFP_KERNEL);
	if (i < 0) {
		error = i;
		goto fail_locked;
	}

	mtd->index = i;
	mtd->usecount = 0;

	/* default value if not set by driver */
	if (mtd->bitflip_threshold == 0)
		mtd->bitflip_threshold = mtd->ecc_strength;

	if (is_power_of_2(mtd->erasesize))
		mtd->erasesize_shift = ffs(mtd->erasesize) - 1;
	else
		mtd->erasesize_shift = 0;

	if (is_power_of_2(mtd->writesize))
		mtd->writesize_shift = ffs(mtd->writesize) - 1;
	else
		mtd->writesize_shift = 0;

	mtd->erasesize_mask = (1 << mtd->erasesize_shift) - 1;
	mtd->writesize_mask = (1 << mtd->writesize_shift) - 1;

	/* Some chips always power up locked. Unlock them now */
	if ((mtd->flags & MTD_WRITEABLE) && (mtd->flags & MTD_POWERUP_LOCK)) {
		error = mtd_unlock(mtd, 0, mtd->size);
		if (error && error != -EOPNOTSUPP)
			printk(KERN_WARNING
			       "%s: unlock failed, writes may not work\n",
			       mtd->name);
		/* Ignore unlock failures? */
		error = 0;
	}

	/* Caller should have set dev.parent to match the
	 * physical device, if appropriate.
	 */
	mtd->dev.type = &mtd_devtype;
	mtd->dev.class = &mtd_class;
	mtd->dev.devt = MTD_DEVT(i);
	dev_set_name(&mtd->dev, "mtd%d", i);
	dev_set_drvdata(&mtd->dev, mtd);
	mtd_check_of_node(mtd);
	of_node_get(mtd_get_of_node(mtd));
	error = device_register(&mtd->dev);
	if (error) {
		put_device(&mtd->dev);
		goto fail_added;
	}

	/* Add the nvmem provider */
	error = mtd_nvmem_add(mtd);
	if (error)
		goto fail_nvmem_add;

	mtd_debugfs_populate(mtd);

	device_create(&mtd_class, mtd->dev.parent, MTD_DEVT(i) + 1, NULL,
		      "mtd%dro", i);

	pr_debug("mtd: Giving out device %d to %s\n", i, mtd->name);
	/* No need to get a refcount on the module containing
	   the notifier, since we hold the mtd_table_mutex */
	list_for_each_entry(not, &mtd_notifiers, list)
		not->add(mtd);

	mutex_unlock(&mtd_table_mutex);
	/* We _know_ we aren't being removed, because
	   our caller is still holding us here. So none
	   of this try_ nonsense, and no bitching about it
	   either. :) */
	__module_get(THIS_MODULE);

	if (!strcmp(mtd->name, "rootfs") &&
	    IS_ENABLED(CONFIG_MTD_ROOTFS_ROOT_DEV) &&
	    ROOT_DEV == 0) {
		pr_notice("mtd: device %d (%s) set to be root filesystem\n",
			  mtd->index, mtd->name);
		ROOT_DEV = MKDEV(MTD_BLOCK_MAJOR, mtd->index);
	}

	return 0;

fail_nvmem_add:
	device_unregister(&mtd->dev);
fail_added:
	of_node_put(mtd_get_of_node(mtd));
	idr_remove(&mtd_idr, i);
fail_locked:
	mutex_unlock(&mtd_table_mutex);
	return error;
}

/**
 *	del_mtd_device - unregister an MTD device
 *	@mtd: pointer to MTD device info structure
 *
 *	Remove a device from the list of MTD devices present in the system,
 *	and notify each currently active MTD 'user' of its departure.
 *	Returns zero on success or 1 on failure, which currently will happen
 *	if the requested device does not appear to be present in the list.
 */

int del_mtd_device(struct mtd_info *mtd)
{
	int ret;
	struct mtd_notifier *not;

	mutex_lock(&mtd_table_mutex);

	if (idr_find(&mtd_idr, mtd->index) != mtd) {
		ret = -ENODEV;
		goto out_error;
	}

	/* No need to get a refcount on the module containing
		the notifier, since we hold the mtd_table_mutex */
	list_for_each_entry(not, &mtd_notifiers, list)
		not->remove(mtd);

	if (mtd->usecount) {
		printk(KERN_NOTICE "Removing MTD device #%d (%s) with use count %d\n",
		       mtd->index, mtd->name, mtd->usecount);
		ret = -EBUSY;
	} else {
		debugfs_remove_recursive(mtd->dbg.dfs_dir);

		/* Try to remove the NVMEM provider */
		if (mtd->nvmem)
			nvmem_unregister(mtd->nvmem);

		device_unregister(&mtd->dev);

		idr_remove(&mtd_idr, mtd->index);
		of_node_put(mtd_get_of_node(mtd));

		module_put(THIS_MODULE);
		ret = 0;
	}

out_error:
	mutex_unlock(&mtd_table_mutex);
	return ret;
}

/*
 * Set a few defaults based on the parent devices, if not provided by the
 * driver
 */
static void mtd_set_dev_defaults(struct mtd_info *mtd)
{
	if (mtd->dev.parent) {
		if (!mtd->owner && mtd->dev.parent->driver)
			mtd->owner = mtd->dev.parent->driver->owner;
		if (!mtd->name)
			mtd->name = dev_name(mtd->dev.parent);
	} else {
		pr_debug("mtd device won't show a device symlink in sysfs\n");
	}

	mtd->orig_flags = mtd->flags;
}

/**
 * mtd_device_parse_register - parse partitions and register an MTD device.
 *
 * @mtd: the MTD device to register
 * @types: the list of MTD partition probes to try, see
 *         'parse_mtd_partitions()' for more information
 * @parser_data: MTD partition parser-specific data
 * @parts: fallback partition information to register, if parsing fails;
 *         only valid if %nr_parts > %0
 * @nr_parts: the number of partitions in parts, if zero then the full
 *            MTD device is registered if no partition info is found
 *
 * This function aggregates MTD partitions parsing (done by
 * 'parse_mtd_partitions()') and MTD device and partitions registering. It
 * basically follows the most common pattern found in many MTD drivers:
 *
 * * If the MTD_PARTITIONED_MASTER option is set, then the device as a whole is
 *   registered first.
 * * Then It tries to probe partitions on MTD device @mtd using parsers
 *   specified in @types (if @types is %NULL, then the default list of parsers
 *   is used, see 'parse_mtd_partitions()' for more information). If none are
 *   found this functions tries to fallback to information specified in
 *   @parts/@nr_parts.
 * * If no partitions were found this function just registers the MTD device
 *   @mtd and exits.
 *
 * Returns zero in case of success and a negative error code in case of failure.
 */
int mtd_device_parse_register(struct mtd_info *mtd, const char * const *types,
			      struct mtd_part_parser_data *parser_data,
			      const struct mtd_partition *parts,
			      int nr_parts)
{
	int ret;

	mtd_set_dev_defaults(mtd);

	if (IS_ENABLED(CONFIG_MTD_PARTITIONED_MASTER)) {
		ret = add_mtd_device(mtd);
		if (ret)
			return ret;
	}

	/* Prefer parsed partitions over driver-provided fallback */
	ret = parse_mtd_partitions(mtd, types, parser_data);
	if (ret == -EPROBE_DEFER)
		goto out;

	if (ret > 0)
		ret = 0;
	else if (nr_parts)
		ret = add_mtd_partitions(mtd, parts, nr_parts);
	else if (!device_is_registered(&mtd->dev))
		ret = add_mtd_device(mtd);
	else
		ret = 0;

	if (ret)
		goto out;

	/*
	 * FIXME: some drivers unfortunately call this function more than once.
	 * So we have to check if we've already assigned the reboot notifier.
	 *
	 * Generally, we can make multiple calls work for most cases, but it
	 * does cause problems with parse_mtd_partitions() above (e.g.,
	 * cmdlineparts will register partitions more than once).
	 */
	WARN_ONCE(mtd->_reboot && mtd->reboot_notifier.notifier_call,
		  "MTD already registered\n");
	if (mtd->_reboot && !mtd->reboot_notifier.notifier_call) {
		mtd->reboot_notifier.notifier_call = mtd_reboot_notifier;
		register_reboot_notifier(&mtd->reboot_notifier);
	}

out:
	if (ret && device_is_registered(&mtd->dev))
		del_mtd_device(mtd);

	return ret;
}
EXPORT_SYMBOL_GPL(mtd_device_parse_register);

/**
 * mtd_device_unregister - unregister an existing MTD device.
 *
 * @master: the MTD device to unregister.  This will unregister both the master
 *          and any partitions if registered.
 */
int mtd_device_unregister(struct mtd_info *master)
{
	int err;

	if (master->_reboot)
		unregister_reboot_notifier(&master->reboot_notifier);

	err = del_mtd_partitions(master);
	if (err)
		return err;

	if (!device_is_registered(&master->dev))
		return 0;

	return del_mtd_device(master);
}
EXPORT_SYMBOL_GPL(mtd_device_unregister);

/**
 *	register_mtd_user - register a 'user' of MTD devices.
 *	@new: pointer to notifier info structure
 *
 *	Registers a pair of callbacks function to be called upon addition
 *	or removal of MTD devices. Causes the 'add' callback to be immediately
 *	invoked for each MTD device currently present in the system.
 */
void register_mtd_user (struct mtd_notifier *new)
{
	struct mtd_info *mtd;

	mutex_lock(&mtd_table_mutex);

	list_add(&new->list, &mtd_notifiers);

	__module_get(THIS_MODULE);

	mtd_for_each_device(mtd)
		new->add(mtd);

	mutex_unlock(&mtd_table_mutex);
}
EXPORT_SYMBOL_GPL(register_mtd_user);

/**
 *	unregister_mtd_user - unregister a 'user' of MTD devices.
 *	@old: pointer to notifier info structure
 *
 *	Removes a callback function pair from the list of 'users' to be
 *	notified upon addition or removal of MTD devices. Causes the
 *	'remove' callback to be immediately invoked for each MTD device
 *	currently present in the system.
 */
int unregister_mtd_user (struct mtd_notifier *old)
{
	struct mtd_info *mtd;

	mutex_lock(&mtd_table_mutex);

	module_put(THIS_MODULE);

	mtd_for_each_device(mtd)
		old->remove(mtd);

	list_del(&old->list);
	mutex_unlock(&mtd_table_mutex);
	return 0;
}
EXPORT_SYMBOL_GPL(unregister_mtd_user);

/**
 *	get_mtd_device - obtain a validated handle for an MTD device
 *	@mtd: last known address of the required MTD device
 *	@num: internal device number of the required MTD device
 *
 *	Given a number and NULL address, return the num'th entry in the device
 *	table, if any.	Given an address and num == -1, search the device table
 *	for a device with that address and return if it's still present. Given
 *	both, return the num'th driver only if its address matches. Return
 *	error code if not.
 */
struct mtd_info *get_mtd_device(struct mtd_info *mtd, int num)
{
	struct mtd_info *ret = NULL, *other;
	int err = -ENODEV;

	mutex_lock(&mtd_table_mutex);

	if (num == -1) {
		mtd_for_each_device(other) {
			if (other == mtd) {
				ret = mtd;
				break;
			}
		}
	} else if (num >= 0) {
		ret = idr_find(&mtd_idr, num);
		if (mtd && mtd != ret)
			ret = NULL;
	}

	if (!ret) {
		ret = ERR_PTR(err);
		goto out;
	}

	err = __get_mtd_device(ret);
	if (err)
		ret = ERR_PTR(err);
out:
	mutex_unlock(&mtd_table_mutex);
	return ret;
}
EXPORT_SYMBOL_GPL(get_mtd_device);


int __get_mtd_device(struct mtd_info *mtd)
{
	int err;

	if (!try_module_get(mtd->owner))
		return -ENODEV;

	if (mtd->_get_device) {
		err = mtd->_get_device(mtd);

		if (err) {
			module_put(mtd->owner);
			return err;
		}
	}
	mtd->usecount++;
	return 0;
}
EXPORT_SYMBOL_GPL(__get_mtd_device);

/**
 * of_get_mtd_device_by_node - obtain an MTD device associated with a given node
 *
 * @np: device tree node
 */
struct mtd_info *of_get_mtd_device_by_node(struct device_node *np)
{
	struct mtd_info *mtd = NULL;
	struct mtd_info *tmp;
	int err;

	mutex_lock(&mtd_table_mutex);

	err = -EPROBE_DEFER;
	mtd_for_each_device(tmp) {
		if (mtd_get_of_node(tmp) == np) {
			mtd = tmp;
			err = __get_mtd_device(mtd);
			break;
		}
	}

	mutex_unlock(&mtd_table_mutex);

	return err ? ERR_PTR(err) : mtd;
}
EXPORT_SYMBOL_GPL(of_get_mtd_device_by_node);

/**
 *	get_mtd_device_nm - obtain a validated handle for an MTD device by
 *	device name
 *	@name: MTD device name to open
 *
 * 	This function returns MTD device description structure in case of
 * 	success and an error code in case of failure.
 */
struct mtd_info *get_mtd_device_nm(const char *name)
{
	int err = -ENODEV;
	struct mtd_info *mtd = NULL, *other;

	mutex_lock(&mtd_table_mutex);

	mtd_for_each_device(other) {
		if (!strcmp(name, other->name)) {
			mtd = other;
			break;
		}
	}

	if (!mtd)
		goto out_unlock;

	err = __get_mtd_device(mtd);
	if (err)
		goto out_unlock;

	mutex_unlock(&mtd_table_mutex);
	return mtd;

out_unlock:
	mutex_unlock(&mtd_table_mutex);
	return ERR_PTR(err);
}
EXPORT_SYMBOL_GPL(get_mtd_device_nm);

/**
 *	get_mtd_device_by_node - obtain a validated handle for an MTD device
 *	by of_node
 *	@of_node: OF node of MTD device to open
 *
 *	This function returns MTD device description structure in case of
 *	success and an error code in case of failure.
 */
struct mtd_info *get_mtd_device_by_node(const struct device_node *of_node)
{
	int err = -ENODEV;
	struct mtd_info *mtd = NULL, *other;

	mutex_lock(&mtd_table_mutex);

	mtd_for_each_device(other) {
		if (of_node == other->dev.of_node) {
			mtd = other;
			break;
		}
	}

	if (!mtd)
		goto out_unlock;

	err = __get_mtd_device(mtd);
	if (err)
		goto out_unlock;

	mutex_unlock(&mtd_table_mutex);
	return mtd;

out_unlock:
	mutex_unlock(&mtd_table_mutex);
	return ERR_PTR(err);
}
EXPORT_SYMBOL_GPL(get_mtd_device_by_node);

void put_mtd_device(struct mtd_info *mtd)
{
	mutex_lock(&mtd_table_mutex);
	__put_mtd_device(mtd);
	mutex_unlock(&mtd_table_mutex);

}
EXPORT_SYMBOL_GPL(put_mtd_device);

void __put_mtd_device(struct mtd_info *mtd)
{
	--mtd->usecount;
	BUG_ON(mtd->usecount < 0);

	if (mtd->_put_device)
		mtd->_put_device(mtd);

	module_put(mtd->owner);
}
EXPORT_SYMBOL_GPL(__put_mtd_device);

/*
 * Erase is an synchronous operation. Device drivers are epected to return a
 * negative error code if the operation failed and update instr->fail_addr
 * to point the portion that was not properly erased.
 */
int mtd_erase(struct mtd_info *mtd, struct erase_info *instr)
{
	instr->fail_addr = MTD_FAIL_ADDR_UNKNOWN;

	if (!mtd->erasesize || !mtd->_erase)
		return -ENOTSUPP;

	if (instr->addr >= mtd->size || instr->len > mtd->size - instr->addr)
		return -EINVAL;
	if (!(mtd->flags & MTD_WRITEABLE))
		return -EROFS;

	if (!instr->len)
		return 0;

	ledtrig_mtd_activity();
	return mtd->_erase(mtd, instr);
}
EXPORT_SYMBOL_GPL(mtd_erase);

/*
 * This stuff for eXecute-In-Place. phys is optional and may be set to NULL.
 */
int mtd_point(struct mtd_info *mtd, loff_t from, size_t len, size_t *retlen,
	      void **virt, resource_size_t *phys)
{
	*retlen = 0;
	*virt = NULL;
	if (phys)
		*phys = 0;
	if (!mtd->_point)
		return -EOPNOTSUPP;
	if (from < 0 || from >= mtd->size || len > mtd->size - from)
		return -EINVAL;
	if (!len)
		return 0;
	return mtd->_point(mtd, from, len, retlen, virt, phys);
}
EXPORT_SYMBOL_GPL(mtd_point);

/* We probably shouldn't allow XIP if the unpoint isn't a NULL */
int mtd_unpoint(struct mtd_info *mtd, loff_t from, size_t len)
{
	if (!mtd->_unpoint)
		return -EOPNOTSUPP;
	if (from < 0 || from >= mtd->size || len > mtd->size - from)
		return -EINVAL;
	if (!len)
		return 0;
	return mtd->_unpoint(mtd, from, len);
}
EXPORT_SYMBOL_GPL(mtd_unpoint);

/*
 * Allow NOMMU mmap() to directly map the device (if not NULL)
 * - return the address to which the offset maps
 * - return -ENOSYS to indicate refusal to do the mapping
 */
unsigned long mtd_get_unmapped_area(struct mtd_info *mtd, unsigned long len,
				    unsigned long offset, unsigned long flags)
{
	size_t retlen;
	void *virt;
	int ret;

	ret = mtd_point(mtd, offset, len, &retlen, &virt, NULL);
	if (ret)
		return ret;
	if (retlen != len) {
		mtd_unpoint(mtd, offset, retlen);
		return -ENOSYS;
	}
	return (unsigned long)virt;
}
EXPORT_SYMBOL_GPL(mtd_get_unmapped_area);

int mtd_read(struct mtd_info *mtd, loff_t from, size_t len, size_t *retlen,
	     u_char *buf)
{
	struct mtd_oob_ops ops = {
		.len = len,
		.datbuf = buf,
	};
	int ret;

	ret = mtd_read_oob(mtd, from, &ops);
	*retlen = ops.retlen;

	return ret;
}
EXPORT_SYMBOL_GPL(mtd_read);

#if !defined(CONFIG_FACTORY_CHECKSUM)	//ASUS_EXT
#include "crc32.c"
#if defined(CONFIG_MTK_NAND_BLOCK2)
#define FLASH_BLOCK_SIZE			(128 * 2 * 1024U)
#else
#define FLASH_BLOCK_SIZE			(128 * 1024U)
#endif
#define EEPROM_SET_HEADER_OFFSET	(FLASH_BLOCK_SIZE - 2 * 1024U)
#define MAX_EEPROM_SET_LENGTH		EEPROM_SET_HEADER_OFFSET
#define FACTORY_MTD_NAME	"Factory"
#define FACTORY2_MTD_NAME	"Factory2"

#define FACTORY_IMAGE_MAGIC	0x46545259	/* 'F', 'T', 'R', 'Y' */
typedef struct eeprom_set_hdr_s {
	uint32_t ih_magic;	/* Image Header Magic Number = 'F', 'T', 'R', 'Y' */
	uint32_t ih_hcrc;	/* Image Header CRC Checksum    */
	uint32_t ih_hdr_ver;	/* Image Header Version Number  */
	uint32_t ih_write_ver;	/* Number of writes             */
	uint32_t ih_dcrc;	/* Image Data CRC Checksum      */
} eeprom_set_hdr_t;

/* Update header checksum, data checksum, write times, etc.
 * Caller must hold act_set->mutex
 * @buf:	pointer to a EEPROM set
 * @return:
 * 	0:	Success
 * 	-1:	Invalid parameter.
 */
static int __update_eeprom_checksum(unsigned char *buf)
{
	eeprom_set_hdr_t *hdr;
	unsigned long checksum, dcrc;

	if (!buf)
		return -1;

	/* calculate image checksum */
	dcrc = crc32(0, buf, MAX_EEPROM_SET_LENGTH);

	hdr = (eeprom_set_hdr_t *)(buf + EEPROM_SET_HEADER_OFFSET);
	/* reset write version to 0 if header magic number is incorrect or wrap */
	if (hdr->ih_magic != htonl(FACTORY_IMAGE_MAGIC) ||
	    ntohl(hdr->ih_write_ver) >= 0x7FFFFFFFU)
		hdr->ih_write_ver = htonl(0);

	/* Build new header */
	hdr->ih_magic = htonl(FACTORY_IMAGE_MAGIC);
	hdr->ih_hcrc = 0;
	hdr->ih_hdr_ver = htonl(1);
	hdr->ih_write_ver = htonl(ntohl(hdr->ih_write_ver) + 1);
	hdr->ih_dcrc = htonl(dcrc);

	checksum = crc32(0, (const char *)hdr, sizeof(*hdr));
	hdr->ih_hcrc = htonl(checksum);

	printk(KERN_DEBUG "header/data checksum: 0x%08x/0x%08x (ver: %d)\n",
		ntohl(hdr->ih_hcrc), ntohl(hdr->ih_dcrc), ntohl(hdr->ih_write_ver));

	return 0;
}

/* Write EEPROM set in RAM to all factory volume except active EEPROM set.
 * Caller must hod act_set->mutex
 * @buf:	pointer to active EEPROM set in RAM
 * @return:
 *     >0:	number erase/write to EEPROM set is failed.
 * 	0:	Success.
 *     -1:	Invalid parameter.
 *     -2:	Read header from active EEPROM set fail.
 */
static int __sync_factory2_eeprom(unsigned char *buf)
{
	int err = 0, err1 = 0, ret = 0;
	size_t wrlen = 0;
	struct mtd_info *mtd;
	struct erase_info instr;
	unsigned int erase_offset = 0;

	if (!buf)
		return -1;
#if defined(CONFIG_MTK_NAND_BLOCK2)
	mtd = get_mtd_device_nm(FACTORY_MTD_NAME);
	if (IS_ERR(mtd)) {
		printk("%s: Can't get mtd_info of %s\n", __func__, "Factory");
	}

	err1 = mtd_read(mtd, 0, FLASH_BLOCK_SIZE, &wrlen, buf);
	if (err1 || wrlen != FLASH_BLOCK_SIZE) {
		printk("%s: read to [%s] fail!. (err %d)\n",
			__func__, mtd->name, err1);
	}
#endif
	mtd = get_mtd_device_nm(FACTORY2_MTD_NAME);
	if (IS_ERR(mtd)) {
		printk("%s: Can't get mtd_info of %s\n", __func__, "Factory2");
	}

	for (erase_offset = 0; erase_offset < mtd->size; erase_offset += mtd->erasesize) {
		memset(&instr, 0, sizeof(instr));
		instr.addr = erase_offset;
		instr.len = mtd->erasesize;
		//printk("erase_offset(%x), erase_size(%x)\n", erase_offset, mtd->erasesize);
		err = mtd_erase(mtd, &instr);
		if (err) {
			printk("%s: erase [%s] fail! (err %d)\n",
				__func__, mtd->name, err);
		}
	}

	err1 = mtd_write(mtd, 0, FLASH_BLOCK_SIZE, &wrlen, buf);
	if (err1 || wrlen != FLASH_BLOCK_SIZE) {
		printk("%s: write to [%s] fail!. (err %d)\n",
			__func__, mtd->name, err1);
	}

	if (!err && !err1)
		printk("Sync %s's EEPROM to [%s] OK.\n", FACTORY_MTD_NAME, mtd->name);

	put_mtd_device(mtd);

	return ret;
}

int mtd_write(struct mtd_info *mtd, loff_t to, size_t len, size_t *retlen,
	      const u_char *buf)
{
	size_t ret_len = 0;

	*retlen = 0;
	if (to < 0 || to > mtd->size || len > mtd->size - to)
		return -EINVAL;
	if (!mtd->_write || !(mtd->flags & MTD_WRITEABLE))
		return -EROFS;
	if (!len)
		return 0;

	if (!strcmp(mtd->name, FACTORY_MTD_NAME) && to < EEPROM_SET_HEADER_OFFSET) {
		int alloc_type = 0;
		unsigned char *p = NULL;

		//printk("%s: name-%s, to-0x%llx, len-0x%lx, erasesize %lx\n", __func__, mtd->name, to, len, FLASH_BLOCK_SIZE);

		p = kmalloc(FLASH_BLOCK_SIZE, GFP_KERNEL);
		if (!p) {
			if ((p = vmalloc(FLASH_BLOCK_SIZE)) != NULL)
				alloc_type = 1;
			else {
				printk(KERN_ERR "%s: allocate 0x%x bytes fail!\n",
					__func__, mtd->erasesize);
				return -ENOMEM;
			}
		}

		memcpy(p, buf, len);

		/* update checksum */
		__update_eeprom_checksum(p);

		ret_len = mtd->_write(mtd, to, len, retlen, p);

		/* sync to factory2's EEPROM */
		__sync_factory2_eeprom(p);

		if (!alloc_type)
			kfree(p);
		else
			vfree(p);
	}
	else
		ret_len = mtd->_write(mtd, to, len, retlen, buf);

	return ret_len;
}
#else
int mtd_write(struct mtd_info *mtd, loff_t to, size_t len, size_t *retlen,
	      const u_char *buf)
{
	struct mtd_oob_ops ops = {
		.len = len,
		.datbuf = (u8 *)buf,
	};
	int ret;

	ret = mtd_write_oob(mtd, to, &ops);
	*retlen = ops.retlen;

	return ret;
}
#endif
EXPORT_SYMBOL_GPL(mtd_write);

/*
 * In blackbox flight recorder like scenarios we want to make successful writes
 * in interrupt context. panic_write() is only intended to be called when its
 * known the kernel is about to panic and we need the write to succeed. Since
 * the kernel is not going to be running for much longer, this function can
 * break locks and delay to ensure the write succeeds (but not sleep).
 */
int mtd_panic_write(struct mtd_info *mtd, loff_t to, size_t len, size_t *retlen,
		    const u_char *buf)
{
	*retlen = 0;
	if (!mtd->_panic_write)
		return -EOPNOTSUPP;
	if (to < 0 || to >= mtd->size || len > mtd->size - to)
		return -EINVAL;
	if (!(mtd->flags & MTD_WRITEABLE))
		return -EROFS;
	if (!len)
		return 0;
	if (!mtd->oops_panic_write)
		mtd->oops_panic_write = true;

	return mtd->_panic_write(mtd, to, len, retlen, buf);
}
EXPORT_SYMBOL_GPL(mtd_panic_write);

static int mtd_check_oob_ops(struct mtd_info *mtd, loff_t offs,
			     struct mtd_oob_ops *ops)
{
	/*
	 * Some users are setting ->datbuf or ->oobbuf to NULL, but are leaving
	 * ->len or ->ooblen uninitialized. Force ->len and ->ooblen to 0 in
	 *  this case.
	 */
	if (!ops->datbuf)
		ops->len = 0;

	if (!ops->oobbuf)
		ops->ooblen = 0;

	if (offs < 0 || offs + ops->len > mtd->size)
		return -EINVAL;

	if (ops->ooblen) {
		size_t maxooblen;

		if (ops->ooboffs >= mtd_oobavail(mtd, ops))
			return -EINVAL;

		maxooblen = ((size_t)(mtd_div_by_ws(mtd->size, mtd) -
				      mtd_div_by_ws(offs, mtd)) *
			     mtd_oobavail(mtd, ops)) - ops->ooboffs;
		if (ops->ooblen > maxooblen)
			return -EINVAL;
	}

	return 0;
}

int mtd_read_oob(struct mtd_info *mtd, loff_t from, struct mtd_oob_ops *ops)
{
	int ret_code;
	ops->retlen = ops->oobretlen = 0;

	ret_code = mtd_check_oob_ops(mtd, from, ops);
	if (ret_code)
		return ret_code;

	ledtrig_mtd_activity();

	/* Check the validity of a potential fallback on mtd->_read */
	if (!mtd->_read_oob && (!mtd->_read || ops->oobbuf))
		return -EOPNOTSUPP;

	if (mtd->_read_oob)
		ret_code = mtd->_read_oob(mtd, from, ops);
	else
		ret_code = mtd->_read(mtd, from, ops->len, &ops->retlen,
				      ops->datbuf);

	/*
	 * In cases where ops->datbuf != NULL, mtd->_read_oob() has semantics
	 * similar to mtd->_read(), returning a non-negative integer
	 * representing max bitflips. In other cases, mtd->_read_oob() may
	 * return -EUCLEAN. In all cases, perform similar logic to mtd_read().
	 */
	if (unlikely(ret_code < 0))
		return ret_code;
	if (mtd->ecc_strength == 0)
		return 0;	/* device lacks ecc */
	return ret_code >= mtd->bitflip_threshold ? -EUCLEAN : 0;
}
EXPORT_SYMBOL_GPL(mtd_read_oob);

int mtd_write_oob(struct mtd_info *mtd, loff_t to,
				struct mtd_oob_ops *ops)
{
	int ret;

	ops->retlen = ops->oobretlen = 0;

	if (!(mtd->flags & MTD_WRITEABLE))
		return -EROFS;

	ret = mtd_check_oob_ops(mtd, to, ops);
	if (ret)
		return ret;

	ledtrig_mtd_activity();

	/* Check the validity of a potential fallback on mtd->_write */
	if (!mtd->_write_oob && (!mtd->_write || ops->oobbuf))
		return -EOPNOTSUPP;

	if (mtd->_write_oob)
		return mtd->_write_oob(mtd, to, ops);
	else
		return mtd->_write(mtd, to, ops->len, &ops->retlen,
				   ops->datbuf);
}
EXPORT_SYMBOL_GPL(mtd_write_oob);

/**
 * mtd_ooblayout_ecc - Get the OOB region definition of a specific ECC section
 * @mtd: MTD device structure
 * @section: ECC section. Depending on the layout you may have all the ECC
 *	     bytes stored in a single contiguous section, or one section
 *	     per ECC chunk (and sometime several sections for a single ECC
 *	     ECC chunk)
 * @oobecc: OOB region struct filled with the appropriate ECC position
 *	    information
 *
 * This function returns ECC section information in the OOB area. If you want
 * to get all the ECC bytes information, then you should call
 * mtd_ooblayout_ecc(mtd, section++, oobecc) until it returns -ERANGE.
 *
 * Returns zero on success, a negative error code otherwise.
 */
int mtd_ooblayout_ecc(struct mtd_info *mtd, int section,
		      struct mtd_oob_region *oobecc)
{
	memset(oobecc, 0, sizeof(*oobecc));

	if (!mtd || section < 0)
		return -EINVAL;

	if (!mtd->ooblayout || !mtd->ooblayout->ecc)
		return -ENOTSUPP;

	return mtd->ooblayout->ecc(mtd, section, oobecc);
}
EXPORT_SYMBOL_GPL(mtd_ooblayout_ecc);

/**
 * mtd_ooblayout_free - Get the OOB region definition of a specific free
 *			section
 * @mtd: MTD device structure
 * @section: Free section you are interested in. Depending on the layout
 *	     you may have all the free bytes stored in a single contiguous
 *	     section, or one section per ECC chunk plus an extra section
 *	     for the remaining bytes (or other funky layout).
 * @oobfree: OOB region struct filled with the appropriate free position
 *	     information
 *
 * This function returns free bytes position in the OOB area. If you want
 * to get all the free bytes information, then you should call
 * mtd_ooblayout_free(mtd, section++, oobfree) until it returns -ERANGE.
 *
 * Returns zero on success, a negative error code otherwise.
 */
int mtd_ooblayout_free(struct mtd_info *mtd, int section,
		       struct mtd_oob_region *oobfree)
{
	memset(oobfree, 0, sizeof(*oobfree));

	if (!mtd || section < 0)
		return -EINVAL;

	if (!mtd->ooblayout || !mtd->ooblayout->free)
		return -ENOTSUPP;

	return mtd->ooblayout->free(mtd, section, oobfree);
}
EXPORT_SYMBOL_GPL(mtd_ooblayout_free);

/**
 * mtd_ooblayout_find_region - Find the region attached to a specific byte
 * @mtd: mtd info structure
 * @byte: the byte we are searching for
 * @sectionp: pointer where the section id will be stored
 * @oobregion: used to retrieve the ECC position
 * @iter: iterator function. Should be either mtd_ooblayout_free or
 *	  mtd_ooblayout_ecc depending on the region type you're searching for
 *
 * This function returns the section id and oobregion information of a
 * specific byte. For example, say you want to know where the 4th ECC byte is
 * stored, you'll use:
 *
 * mtd_ooblayout_find_region(mtd, 3, &section, &oobregion, mtd_ooblayout_ecc);
 *
 * Returns zero on success, a negative error code otherwise.
 */
static int mtd_ooblayout_find_region(struct mtd_info *mtd, int byte,
				int *sectionp, struct mtd_oob_region *oobregion,
				int (*iter)(struct mtd_info *,
					    int section,
					    struct mtd_oob_region *oobregion))
{
	int pos = 0, ret, section = 0;

	memset(oobregion, 0, sizeof(*oobregion));

	while (1) {
		ret = iter(mtd, section, oobregion);
		if (ret)
			return ret;

		if (pos + oobregion->length > byte)
			break;

		pos += oobregion->length;
		section++;
	}

	/*
	 * Adjust region info to make it start at the beginning at the
	 * 'start' ECC byte.
	 */
	oobregion->offset += byte - pos;
	oobregion->length -= byte - pos;
	*sectionp = section;

	return 0;
}

/**
 * mtd_ooblayout_find_eccregion - Find the ECC region attached to a specific
 *				  ECC byte
 * @mtd: mtd info structure
 * @eccbyte: the byte we are searching for
 * @sectionp: pointer where the section id will be stored
 * @oobregion: OOB region information
 *
 * Works like mtd_ooblayout_find_region() except it searches for a specific ECC
 * byte.
 *
 * Returns zero on success, a negative error code otherwise.
 */
int mtd_ooblayout_find_eccregion(struct mtd_info *mtd, int eccbyte,
				 int *section,
				 struct mtd_oob_region *oobregion)
{
	return mtd_ooblayout_find_region(mtd, eccbyte, section, oobregion,
					 mtd_ooblayout_ecc);
}
EXPORT_SYMBOL_GPL(mtd_ooblayout_find_eccregion);

/**
 * mtd_ooblayout_get_bytes - Extract OOB bytes from the oob buffer
 * @mtd: mtd info structure
 * @buf: destination buffer to store OOB bytes
 * @oobbuf: OOB buffer
 * @start: first byte to retrieve
 * @nbytes: number of bytes to retrieve
 * @iter: section iterator
 *
 * Extract bytes attached to a specific category (ECC or free)
 * from the OOB buffer and copy them into buf.
 *
 * Returns zero on success, a negative error code otherwise.
 */
static int mtd_ooblayout_get_bytes(struct mtd_info *mtd, u8 *buf,
				const u8 *oobbuf, int start, int nbytes,
				int (*iter)(struct mtd_info *,
					    int section,
					    struct mtd_oob_region *oobregion))
{
	struct mtd_oob_region oobregion;
	int section, ret;

	ret = mtd_ooblayout_find_region(mtd, start, &section,
					&oobregion, iter);

	while (!ret) {
		int cnt;

		cnt = min_t(int, nbytes, oobregion.length);
		memcpy(buf, oobbuf + oobregion.offset, cnt);
		buf += cnt;
		nbytes -= cnt;

		if (!nbytes)
			break;

		ret = iter(mtd, ++section, &oobregion);
	}

	return ret;
}

/**
 * mtd_ooblayout_set_bytes - put OOB bytes into the oob buffer
 * @mtd: mtd info structure
 * @buf: source buffer to get OOB bytes from
 * @oobbuf: OOB buffer
 * @start: first OOB byte to set
 * @nbytes: number of OOB bytes to set
 * @iter: section iterator
 *
 * Fill the OOB buffer with data provided in buf. The category (ECC or free)
 * is selected by passing the appropriate iterator.
 *
 * Returns zero on success, a negative error code otherwise.
 */
static int mtd_ooblayout_set_bytes(struct mtd_info *mtd, const u8 *buf,
				u8 *oobbuf, int start, int nbytes,
				int (*iter)(struct mtd_info *,
					    int section,
					    struct mtd_oob_region *oobregion))
{
	struct mtd_oob_region oobregion;
	int section, ret;

	ret = mtd_ooblayout_find_region(mtd, start, &section,
					&oobregion, iter);

	while (!ret) {
		int cnt;

		cnt = min_t(int, nbytes, oobregion.length);
		memcpy(oobbuf + oobregion.offset, buf, cnt);
		buf += cnt;
		nbytes -= cnt;

		if (!nbytes)
			break;

		ret = iter(mtd, ++section, &oobregion);
	}

	return ret;
}

/**
 * mtd_ooblayout_count_bytes - count the number of bytes in a OOB category
 * @mtd: mtd info structure
 * @iter: category iterator
 *
 * Count the number of bytes in a given category.
 *
 * Returns a positive value on success, a negative error code otherwise.
 */
static int mtd_ooblayout_count_bytes(struct mtd_info *mtd,
				int (*iter)(struct mtd_info *,
					    int section,
					    struct mtd_oob_region *oobregion))
{
	struct mtd_oob_region oobregion;
	int section = 0, ret, nbytes = 0;

	while (1) {
		ret = iter(mtd, section++, &oobregion);
		if (ret) {
			if (ret == -ERANGE)
				ret = nbytes;
			break;
		}

		nbytes += oobregion.length;
	}

	return ret;
}

/**
 * mtd_ooblayout_get_eccbytes - extract ECC bytes from the oob buffer
 * @mtd: mtd info structure
 * @eccbuf: destination buffer to store ECC bytes
 * @oobbuf: OOB buffer
 * @start: first ECC byte to retrieve
 * @nbytes: number of ECC bytes to retrieve
 *
 * Works like mtd_ooblayout_get_bytes(), except it acts on ECC bytes.
 *
 * Returns zero on success, a negative error code otherwise.
 */
int mtd_ooblayout_get_eccbytes(struct mtd_info *mtd, u8 *eccbuf,
			       const u8 *oobbuf, int start, int nbytes)
{
	return mtd_ooblayout_get_bytes(mtd, eccbuf, oobbuf, start, nbytes,
				       mtd_ooblayout_ecc);
}
EXPORT_SYMBOL_GPL(mtd_ooblayout_get_eccbytes);

/**
 * mtd_ooblayout_set_eccbytes - set ECC bytes into the oob buffer
 * @mtd: mtd info structure
 * @eccbuf: source buffer to get ECC bytes from
 * @oobbuf: OOB buffer
 * @start: first ECC byte to set
 * @nbytes: number of ECC bytes to set
 *
 * Works like mtd_ooblayout_set_bytes(), except it acts on ECC bytes.
 *
 * Returns zero on success, a negative error code otherwise.
 */
int mtd_ooblayout_set_eccbytes(struct mtd_info *mtd, const u8 *eccbuf,
			       u8 *oobbuf, int start, int nbytes)
{
	return mtd_ooblayout_set_bytes(mtd, eccbuf, oobbuf, start, nbytes,
				       mtd_ooblayout_ecc);
}
EXPORT_SYMBOL_GPL(mtd_ooblayout_set_eccbytes);

/**
 * mtd_ooblayout_get_databytes - extract data bytes from the oob buffer
 * @mtd: mtd info structure
 * @databuf: destination buffer to store ECC bytes
 * @oobbuf: OOB buffer
 * @start: first ECC byte to retrieve
 * @nbytes: number of ECC bytes to retrieve
 *
 * Works like mtd_ooblayout_get_bytes(), except it acts on free bytes.
 *
 * Returns zero on success, a negative error code otherwise.
 */
int mtd_ooblayout_get_databytes(struct mtd_info *mtd, u8 *databuf,
				const u8 *oobbuf, int start, int nbytes)
{
	return mtd_ooblayout_get_bytes(mtd, databuf, oobbuf, start, nbytes,
				       mtd_ooblayout_free);
}
EXPORT_SYMBOL_GPL(mtd_ooblayout_get_databytes);

/**
 * mtd_ooblayout_set_databytes - set data bytes into the oob buffer
 * @mtd: mtd info structure
 * @databuf: source buffer to get data bytes from
 * @oobbuf: OOB buffer
 * @start: first ECC byte to set
 * @nbytes: number of ECC bytes to set
 *
 * Works like mtd_ooblayout_get_bytes(), except it acts on free bytes.
 *
 * Returns zero on success, a negative error code otherwise.
 */
int mtd_ooblayout_set_databytes(struct mtd_info *mtd, const u8 *databuf,
				u8 *oobbuf, int start, int nbytes)
{
	return mtd_ooblayout_set_bytes(mtd, databuf, oobbuf, start, nbytes,
				       mtd_ooblayout_free);
}
EXPORT_SYMBOL_GPL(mtd_ooblayout_set_databytes);

/**
 * mtd_ooblayout_count_freebytes - count the number of free bytes in OOB
 * @mtd: mtd info structure
 *
 * Works like mtd_ooblayout_count_bytes(), except it count free bytes.
 *
 * Returns zero on success, a negative error code otherwise.
 */
int mtd_ooblayout_count_freebytes(struct mtd_info *mtd)
{
	return mtd_ooblayout_count_bytes(mtd, mtd_ooblayout_free);
}
EXPORT_SYMBOL_GPL(mtd_ooblayout_count_freebytes);

/**
 * mtd_ooblayout_count_eccbytes - count the number of ECC bytes in OOB
 * @mtd: mtd info structure
 *
 * Works like mtd_ooblayout_count_bytes(), except it count ECC bytes.
 *
 * Returns zero on success, a negative error code otherwise.
 */
int mtd_ooblayout_count_eccbytes(struct mtd_info *mtd)
{
	return mtd_ooblayout_count_bytes(mtd, mtd_ooblayout_ecc);
}
EXPORT_SYMBOL_GPL(mtd_ooblayout_count_eccbytes);

/*
 * Method to access the protection register area, present in some flash
 * devices. The user data is one time programmable but the factory data is read
 * only.
 */
int mtd_get_fact_prot_info(struct mtd_info *mtd, size_t len, size_t *retlen,
			   struct otp_info *buf)
{
	if (!mtd->_get_fact_prot_info)
		return -EOPNOTSUPP;
	if (!len)
		return 0;
	return mtd->_get_fact_prot_info(mtd, len, retlen, buf);
}
EXPORT_SYMBOL_GPL(mtd_get_fact_prot_info);

int mtd_read_fact_prot_reg(struct mtd_info *mtd, loff_t from, size_t len,
			   size_t *retlen, u_char *buf)
{
	*retlen = 0;
	if (!mtd->_read_fact_prot_reg)
		return -EOPNOTSUPP;
	if (!len)
		return 0;
	return mtd->_read_fact_prot_reg(mtd, from, len, retlen, buf);
}
EXPORT_SYMBOL_GPL(mtd_read_fact_prot_reg);

int mtd_get_user_prot_info(struct mtd_info *mtd, size_t len, size_t *retlen,
			   struct otp_info *buf)
{
	if (!mtd->_get_user_prot_info)
		return -EOPNOTSUPP;
	if (!len)
		return 0;
	return mtd->_get_user_prot_info(mtd, len, retlen, buf);
}
EXPORT_SYMBOL_GPL(mtd_get_user_prot_info);

int mtd_read_user_prot_reg(struct mtd_info *mtd, loff_t from, size_t len,
			   size_t *retlen, u_char *buf)
{
	*retlen = 0;
	if (!mtd->_read_user_prot_reg)
		return -EOPNOTSUPP;
	if (!len)
		return 0;
	return mtd->_read_user_prot_reg(mtd, from, len, retlen, buf);
}
EXPORT_SYMBOL_GPL(mtd_read_user_prot_reg);

int mtd_write_user_prot_reg(struct mtd_info *mtd, loff_t to, size_t len,
			    size_t *retlen, u_char *buf)
{
	int ret;

	*retlen = 0;
	if (!mtd->_write_user_prot_reg)
		return -EOPNOTSUPP;
	if (!len)
		return 0;
	ret = mtd->_write_user_prot_reg(mtd, to, len, retlen, buf);
	if (ret)
		return ret;

	/*
	 * If no data could be written at all, we are out of memory and
	 * must return -ENOSPC.
	 */
	return (*retlen) ? 0 : -ENOSPC;
}
EXPORT_SYMBOL_GPL(mtd_write_user_prot_reg);

int mtd_lock_user_prot_reg(struct mtd_info *mtd, loff_t from, size_t len)
{
	if (!mtd->_lock_user_prot_reg)
		return -EOPNOTSUPP;
	if (!len)
		return 0;
	return mtd->_lock_user_prot_reg(mtd, from, len);
}
EXPORT_SYMBOL_GPL(mtd_lock_user_prot_reg);

/* Chip-supported device locking */
int mtd_lock(struct mtd_info *mtd, loff_t ofs, uint64_t len)
{
	if (!mtd->_lock)
		return -EOPNOTSUPP;
	if (ofs < 0 || ofs >= mtd->size || len > mtd->size - ofs)
		return -EINVAL;
	if (!len)
		return 0;
	return mtd->_lock(mtd, ofs, len);
}
EXPORT_SYMBOL_GPL(mtd_lock);

int mtd_unlock(struct mtd_info *mtd, loff_t ofs, uint64_t len)
{
	if (!mtd->_unlock)
		return -EOPNOTSUPP;
	if (ofs < 0 || ofs >= mtd->size || len > mtd->size - ofs)
		return -EINVAL;
	if (!len)
		return 0;
	return mtd->_unlock(mtd, ofs, len);
}
EXPORT_SYMBOL_GPL(mtd_unlock);

int mtd_is_locked(struct mtd_info *mtd, loff_t ofs, uint64_t len)
{
	if (!mtd->_is_locked)
		return -EOPNOTSUPP;
	if (ofs < 0 || ofs >= mtd->size || len > mtd->size - ofs)
		return -EINVAL;
	if (!len)
		return 0;
	return mtd->_is_locked(mtd, ofs, len);
}
EXPORT_SYMBOL_GPL(mtd_is_locked);

int mtd_block_isreserved(struct mtd_info *mtd, loff_t ofs)
{
	if (ofs < 0 || ofs >= mtd->size)
		return -EINVAL;
	if (!mtd->_block_isreserved)
		return 0;
	return mtd->_block_isreserved(mtd, ofs);
}
EXPORT_SYMBOL_GPL(mtd_block_isreserved);

int mtd_block_isbad(struct mtd_info *mtd, loff_t ofs)
{
	if (ofs < 0 || ofs >= mtd->size)
		return -EINVAL;
	if (!mtd->_block_isbad)
		return 0;
	return mtd->_block_isbad(mtd, ofs);
}
EXPORT_SYMBOL_GPL(mtd_block_isbad);

int mtd_block_markbad(struct mtd_info *mtd, loff_t ofs)
{
	if (!mtd->_block_markbad)
		return -EOPNOTSUPP;
	if (ofs < 0 || ofs >= mtd->size)
		return -EINVAL;
	if (!(mtd->flags & MTD_WRITEABLE))
		return -EROFS;
	return mtd->_block_markbad(mtd, ofs);
}
EXPORT_SYMBOL_GPL(mtd_block_markbad);

/*
 * default_mtd_writev - the default writev method
 * @mtd: mtd device description object pointer
 * @vecs: the vectors to write
 * @count: count of vectors in @vecs
 * @to: the MTD device offset to write to
 * @retlen: on exit contains the count of bytes written to the MTD device.
 *
 * This function returns zero in case of success and a negative error code in
 * case of failure.
 */
static int default_mtd_writev(struct mtd_info *mtd, const struct kvec *vecs,
			      unsigned long count, loff_t to, size_t *retlen)
{
	unsigned long i;
	size_t totlen = 0, thislen;
	int ret = 0;

	for (i = 0; i < count; i++) {
		if (!vecs[i].iov_len)
			continue;
		ret = mtd_write(mtd, to, vecs[i].iov_len, &thislen,
				vecs[i].iov_base);
		totlen += thislen;
		if (ret || thislen != vecs[i].iov_len)
			break;
		to += vecs[i].iov_len;
	}
	*retlen = totlen;
	return ret;
}

/*
 * mtd_writev - the vector-based MTD write method
 * @mtd: mtd device description object pointer
 * @vecs: the vectors to write
 * @count: count of vectors in @vecs
 * @to: the MTD device offset to write to
 * @retlen: on exit contains the count of bytes written to the MTD device.
 *
 * This function returns zero in case of success and a negative error code in
 * case of failure.
 */
int mtd_writev(struct mtd_info *mtd, const struct kvec *vecs,
	       unsigned long count, loff_t to, size_t *retlen)
{
	*retlen = 0;
	if (!(mtd->flags & MTD_WRITEABLE))
		return -EROFS;
	if (!mtd->_writev)
		return default_mtd_writev(mtd, vecs, count, to, retlen);
	return mtd->_writev(mtd, vecs, count, to, retlen);
}
EXPORT_SYMBOL_GPL(mtd_writev);

/**
 * mtd_kmalloc_up_to - allocate a contiguous buffer up to the specified size
 * @mtd: mtd device description object pointer
 * @size: a pointer to the ideal or maximum size of the allocation, points
 *        to the actual allocation size on success.
 *
 * This routine attempts to allocate a contiguous kernel buffer up to
 * the specified size, backing off the size of the request exponentially
 * until the request succeeds or until the allocation size falls below
 * the system page size. This attempts to make sure it does not adversely
 * impact system performance, so when allocating more than one page, we
 * ask the memory allocator to avoid re-trying, swapping, writing back
 * or performing I/O.
 *
 * Note, this function also makes sure that the allocated buffer is aligned to
 * the MTD device's min. I/O unit, i.e. the "mtd->writesize" value.
 *
 * This is called, for example by mtd_{read,write} and jffs2_scan_medium,
 * to handle smaller (i.e. degraded) buffer allocations under low- or
 * fragmented-memory situations where such reduced allocations, from a
 * requested ideal, are allowed.
 *
 * Returns a pointer to the allocated buffer on success; otherwise, NULL.
 */
void *mtd_kmalloc_up_to(const struct mtd_info *mtd, size_t *size)
{
	gfp_t flags = __GFP_NOWARN | __GFP_DIRECT_RECLAIM | __GFP_NORETRY;
	size_t min_alloc = max_t(size_t, mtd->writesize, PAGE_SIZE);
	void *kbuf;

	*size = min_t(size_t, *size, KMALLOC_MAX_SIZE);

	while (*size > min_alloc) {
		kbuf = kmalloc(*size, flags);
		if (kbuf)
			return kbuf;

		*size >>= 1;
		*size = ALIGN(*size, mtd->writesize);
	}

	/*
	 * For the last resort allocation allow 'kmalloc()' to do all sorts of
	 * things (write-back, dropping caches, etc) by using GFP_KERNEL.
	 */
	return kmalloc(*size, GFP_KERNEL);
}
EXPORT_SYMBOL_GPL(mtd_kmalloc_up_to);

#ifdef CONFIG_PROC_FS

/*====================================================================*/
/* Support for /proc/mtd */

static int mtd_proc_show(struct seq_file *m, void *v)
{
	struct mtd_info *mtd;

	seq_puts(m, "dev:    size   erasesize  name\n");
	mutex_lock(&mtd_table_mutex);
	mtd_for_each_device(mtd) {
		seq_printf(m, "mtd%d: %8.8llx %8.8x \"%s\"\n",
			   mtd->index, (unsigned long long)mtd->size,
			   mtd->erasesize, mtd->name);
	}
	mutex_unlock(&mtd_table_mutex);
	return 0;
}
#endif /* CONFIG_PROC_FS */

/*====================================================================*/
/* Init code */

static struct backing_dev_info * __init mtd_bdi_init(char *name)
{
	struct backing_dev_info *bdi;
	int ret;

	bdi = bdi_alloc(GFP_KERNEL);
	if (!bdi)
		return ERR_PTR(-ENOMEM);

	bdi->name = name;
	/*
	 * We put '-0' suffix to the name to get the same name format as we
	 * used to get. Since this is called only once, we get a unique name. 
	 */
	ret = bdi_register(bdi, "%.28s-0", name);
	if (ret)
		bdi_put(bdi);

	return ret ? ERR_PTR(ret) : bdi;
}

static struct proc_dir_entry *proc_mtd;

static int __init init_mtd(void)
{
	int ret;

	ret = class_register(&mtd_class);
	if (ret)
		goto err_reg;

	mtd_bdi = mtd_bdi_init("mtd");
	if (IS_ERR(mtd_bdi)) {
		ret = PTR_ERR(mtd_bdi);
		goto err_bdi;
	}

	proc_mtd = proc_create_single("mtd", 0, NULL, mtd_proc_show);

	ret = init_mtdchar();
	if (ret)
		goto out_procfs;

	dfs_dir_mtd = debugfs_create_dir("mtd", NULL);

	return 0;

out_procfs:
	if (proc_mtd)
		remove_proc_entry("mtd", NULL);
	bdi_put(mtd_bdi);
err_bdi:
	class_unregister(&mtd_class);
err_reg:
	pr_err("Error registering mtd class or bdi: %d\n", ret);
	return ret;
}

static void __exit cleanup_mtd(void)
{
	debugfs_remove_recursive(dfs_dir_mtd);
	cleanup_mtdchar();
	if (proc_mtd)
		remove_proc_entry("mtd", NULL);
	class_unregister(&mtd_class);
	bdi_put(mtd_bdi);
	idr_destroy(&mtd_idr);
}

module_init(init_mtd);
module_exit(cleanup_mtd);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("David Woodhouse <dwmw2@infradead.org>");
MODULE_DESCRIPTION("Core MTD registration and access routines");
