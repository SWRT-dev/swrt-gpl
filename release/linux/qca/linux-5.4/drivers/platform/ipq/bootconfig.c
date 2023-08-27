/*
 * Copyright (c) 2015-2016, 2020, The Linux Foundation. All rights reserved.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/seq_file.h>
#include <asm/setup.h>
#include <linux/mtd/partitions.h>
#include <linux/proc_fs.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/genhd.h>
#include <linux/major.h>
#include <linux/mtd/blktrans.h>
#include <linux/mtd/mtd.h>
#include <linux/types.h>
#include <linux/blkdev.h>
#include <linux/init.h>
#include <linux/qcom_scm.h>
#include <linux/soc/qcom/smem.h>
#include "bootconfig.h"


#define WRITE_ENABLE 	1
#define WRITE_DISABLE 	0
#define SMEM_TRYMODE_INFO 507
#define BOOTCONFIG_PARTITION	"0:BOOTCONFIG"
#define BOOTCONFIG_PARTITION1	"0:BOOTCONFIG1"
#define ROOTFS_PARTITION	"rootfs"
#define MAX_MMC_DEVICE		2

static struct proc_dir_entry *bc1_partname_dir[CONFIG_NUM_ALT_PARTITION];
static struct proc_dir_entry *bc2_partname_dir[CONFIG_NUM_ALT_PARTITION];

static unsigned int bc1_num_parts;
static unsigned int bc2_num_parts;

static unsigned int flash_type_emmc;

struct sbl_if_dualboot_info_type_v2 *bootconfig1;
struct sbl_if_dualboot_info_type_v2 *bootconfig2;
static struct proc_dir_entry *boot_info_dir;
static struct proc_dir_entry *upgrade_info_dir;;
static struct proc_dir_entry *bootconfig1_info_dir;
static struct proc_dir_entry *bootconfig2_info_dir;

static unsigned int age_write_permession = WRITE_DISABLE;
static u32 *trymode_inprogress;

static int try_feature;

static unsigned long int trybit;
#define CFG_MAX_DIG_COUNT	2
#define QCOM_SCM_SVC_BOOT	0x1

static int getbinary_show(struct seq_file *m, void *v)
{
	struct sbl_if_dualboot_info_type_v2 *sbl_info_v2;

	sbl_info_v2 = m->private;
	memcpy(m->buf + m->count, sbl_info_v2,
		sizeof(struct sbl_if_dualboot_info_type_v2));
	m->count += sizeof(struct sbl_if_dualboot_info_type_v2);

	return 0;
}

static int getbinary_open(struct inode *inode, struct file *file)
{
	return single_open(file, getbinary_show, PDE_DATA(inode));
}

static const struct file_operations getbinary_ops = {
	.open		= getbinary_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};

static int part_upgradepartition_show(struct seq_file *m, void *v)
{
	struct per_part_info *part_info_t = m->private;

	/*
	 * In case of NOR\NAND, SBLs change the names of paritions in
	 * such a way that the partition to upgrade is always suffixed
	 * by _1. This is not the case in eMMC as paritions are read
	 * from GPT and we have no control on it. So for eMMC we need
	 * to check and generate the name wheres for NOR\NAND it is
	 * always _1 SBLs should be modified not to change partition
	 * names so that it is consistent with GPT. Till that is done
	 * we will take care of it here.
	 */

	if (flash_type_emmc && (part_info_t->primaryboot))
		seq_printf(m, "%s\n", part_info_t->name);
	else
		seq_printf(m, "%s_1\n", part_info_t->name);

	return 0;

}

static int part_upgradepartition_open(struct inode *inode, struct file *file)
{
	return single_open(file, part_upgradepartition_show, PDE_DATA(inode));
}

static const struct file_operations upgradepartition_ops = {
	.open		= part_upgradepartition_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};

static ssize_t part_primaryboot_write(struct file *file,
				       const char __user *user,
				       size_t count, loff_t *data)
{
	int ret;
	char optstr[64];
	struct per_part_info *part_entry;
	unsigned long val;

	part_entry = PDE_DATA(file_inode(file));

	if (count == 0 || count > sizeof(optstr))
		return -EINVAL;

	ret = copy_from_user(optstr, user, count);
	if (ret)
		return ret;

	optstr[count - 1] = '\0';

	ret = kstrtoul(optstr, 0, &val);
	if (ret)
		return ret;

	part_entry->primaryboot = val;

	return count;

}

static int part_primaryboot_show(struct seq_file *m, void *v)
{
	struct per_part_info *part_entry;

	part_entry = m->private;
	seq_printf(m, "%x\n", part_entry->primaryboot);
	return 0;
}

static int part_primaryboot_open(struct inode *inode, struct file *file)
{
	return single_open(file, part_primaryboot_show, PDE_DATA(inode));
}

static const struct file_operations primaryboot_ops = {
	.open		= part_primaryboot_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
	.write		= part_primaryboot_write,
};

static ssize_t trybit_write(struct file *file,
				       const char __user *user,
				       size_t count, loff_t *data)
{
	int ret;
	char optstr[64];
	unsigned long int *value;
	unsigned long val;

	value = PDE_DATA(file_inode(file));

	if (count == 0 || count > sizeof(optstr))
		return -EINVAL;

	ret = copy_from_user(optstr, user, count);
	if (ret)
		return ret;

	optstr[count - 1] = '\0';

	ret = kstrtoul(optstr, 0, &val);
	if (ret)
		return ret;

	if(1 == val)
	{
		qti_scm_set_trybit(QCOM_SCM_SVC_BOOT);
	} else{
		return -EINVAL;
	}
	return count;
}

static int trybit_show(struct seq_file *m, void *v)
{
	uint32_t val;
	val = qti_read_dload_reg();
	val = (val & QTI_TRYBIT) ? 1 : 0;

	seq_printf(m, "%x\n", val);
	return 0;
}

static int trybit_open(struct inode *inode, struct file *file)
{
	return single_open(file, trybit_show, PDE_DATA(inode));
}

static const struct file_operations trybit_ops = {
	.open           = trybit_open,
	.read           = seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
	.write		= trybit_write,
};

static int trymode_inprogress_show(struct seq_file *m, void *v)
{
	static uint8_t *update_age;
	update_age = m->private;
	seq_printf(m, "%x\n", *update_age);
	*update_age = WRITE_DISABLE;
	return 0;
}

static int trymode_inprogress_open(struct inode *inode, struct file *file)
{
	return single_open(file, trymode_inprogress_show, PDE_DATA(inode));
}

static const struct file_operations trymode_inprogress_ops = {
	.open		= trymode_inprogress_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};

struct sbl_if_dualboot_info_type_v2 *read_bootconfig_mtd(
						struct mtd_info *master,
						uint64_t offset)
{

	size_t retlen = 0;
	struct sbl_if_dualboot_info_type_v2 *bootconfig_mtd;
	int ret;

	while (mtd_block_isbad(master, offset)) {
		offset += master->erasesize;
		if (offset >= master->size) {
			pr_alert("Bad blocks occurred while reading from \"%s\"\n",
					master->name);
			return NULL;
		}
	}
	bootconfig_mtd = kmalloc(sizeof(struct sbl_if_dualboot_info_type_v2),
				   GFP_ATOMIC);

	if (!bootconfig_mtd)
		return NULL;

	ret = mtd_read(master, offset,
			sizeof(struct sbl_if_dualboot_info_type_v2),
			&retlen, (void *)bootconfig_mtd);
	if (ret < 0) {
		pr_alert("error occured while reading from \"%s\"\n",
				master->name);
		bootconfig_mtd = NULL;
		kfree(bootconfig_mtd);
		return NULL;
	}

	if ((bootconfig_mtd->magic_start != SMEM_DUAL_BOOTINFO_MAGIC_START) &&
		(bootconfig_mtd->magic_start != SMEM_DUAL_BOOTINFO_MAGIC_START_TRYMODE)) {
		pr_alert("Magic not found in \"%s\"\n", master->name);
		kfree(bootconfig_mtd);
		return NULL;
	}

	return bootconfig_mtd;
}

#ifdef CONFIG_MMC
struct sbl_if_dualboot_info_type_v2 *read_bootconfig_emmc(struct gendisk *disk,
						struct hd_struct *part)
{
	sector_t n;
	Sector sect;
	int ret;
	unsigned char *data;
	struct sbl_if_dualboot_info_type_v2 *bootconfig_emmc;
	unsigned ssz;
	struct block_device *bdev = NULL;

	bdev = bdget_disk(disk, 0);
	if (!bdev)
		return NULL;

	bdev->bd_invalidated = 1;
	ret = blkdev_get(bdev, FMODE_READ , NULL);
	if (ret)
		return NULL;

	ssz = bdev_logical_block_size(bdev);
	bootconfig_emmc = kmalloc(ssz, GFP_ATOMIC);
	if (!bootconfig_emmc)
		return NULL;

	n =  part->start_sect * (bdev_logical_block_size(bdev) / 512);
	data = read_dev_sector(bdev, n, &sect);
	put_dev_sector(sect);
	blkdev_put(bdev, FMODE_READ);
	if (!data) {
		kfree(bootconfig_emmc);
		return NULL;
	}

	memcpy(bootconfig_emmc, data, 512);

	if ((bootconfig_emmc->magic_start != SMEM_DUAL_BOOTINFO_MAGIC_START) &&
		(bootconfig_emmc->magic_start != SMEM_DUAL_BOOTINFO_MAGIC_START_TRYMODE)) {
		pr_alert("Magic not found\n");
		kfree(bootconfig_emmc);
		return NULL;
	}

	return bootconfig_emmc;
}
#endif

static ssize_t age_write(struct file *file,
				       const char __user *user,
				       size_t count, loff_t *data)
{
	int ret;
	char optstr[64];
	struct sbl_if_dualboot_info_type_v2 *bootconfig;
	unsigned long val;

	bootconfig = PDE_DATA(file_inode(file));

	if (count == 0 || count > sizeof(optstr))
		return -EINVAL;

	ret = copy_from_user(optstr, user, count);
	if (ret)
		return ret;

	optstr[count - 1] = '\0';

	ret = kstrtoul(optstr, 0, &val);
	if (ret)
		return ret;

	bootconfig->age = val;

	return count;
}

static int age_show(struct seq_file *m, void *v)
{
	struct sbl_if_dualboot_info_type_v2 *bootconfig;
	bootconfig = m->private;
	seq_printf(m, "%d\n", bootconfig->age);

	return 0;
}

static int age_open(struct inode *inode, struct file *file)
{
	return single_open(file, age_show, PDE_DATA(inode));
}

static const struct file_operations age_ops = {
	.open		= age_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
	.write          = age_write,
};

static int __init bootconfig_partition_init(void)
{
	struct per_part_info *bc1_part_info;
	struct per_part_info *bc2_part_info;

	int i;
#ifdef CONFIG_MMC
	struct gendisk *disk = NULL;
	struct disk_part_iter piter;
	struct hd_struct *part;
	int partno;
#endif
	struct mtd_info *mtd;
	size_t len;

	/*
	 * In case of NOR\NAND boot, there is a chance that emmc
	 * might have bootconfig paritions. This will try to read
	 * the bootconfig partitions and create a proc entry which
	 * is not correct since it is not booting from emmc.
	 */

	mtd = get_mtd_device_nm(ROOTFS_PARTITION);
	if (IS_ERR(mtd))
		flash_type_emmc = 1;
	else
		put_mtd_device(mtd);

	mtd = get_mtd_device_nm(BOOTCONFIG_PARTITION);
	if (!IS_ERR(mtd)) {

		bootconfig1 = read_bootconfig_mtd(mtd, 0);
		put_mtd_device(mtd);

		mtd = get_mtd_device_nm(BOOTCONFIG_PARTITION1);
		if (IS_ERR(mtd)) {
			pr_alert("%s: " BOOTCONFIG_PARTITION1 " not found\n",
				__func__);
			goto free_memory;
		}

		bootconfig2 = read_bootconfig_mtd(mtd, 0);
		put_mtd_device(mtd);
	}
#ifdef CONFIG_MMC
	else if (flash_type_emmc == 1) {
		flash_type_emmc = 0;
		for (i = 0; i < MAX_MMC_DEVICE; i++) {

			disk = get_gendisk(MKDEV(MMC_BLOCK_MAJOR, i*CONFIG_MMC_BLOCK_MINORS), &partno);
			if (!disk)
				goto free_memory;

			disk_part_iter_init(&piter, disk, DISK_PITER_INCL_PART0);
			while ((part = disk_part_iter_next(&piter))) {

				if (part->info) {
					if (!strcmp((char *)part->info->volname,
							BOOTCONFIG_PARTITION)) {
						bootconfig1 = read_bootconfig_emmc(disk,
									part);
					}

					if (!strcmp((char *)part->info->volname,
							BOOTCONFIG_PARTITION1)) {
						bootconfig2 = read_bootconfig_emmc(disk,
									 part);
						flash_type_emmc = 1;
					}
				}
			}
			disk_part_iter_exit(&piter);

			if (bootconfig1 || bootconfig2)
			       break;
		}
	}
#endif

/*
 * The following check is to handle the case when an image without
 * apps upgrade support is upgraded to the image that supports APPS
 * upgrade. Earlier, the bootconfig file will be chosen based on age,
 * but now bootconfig1 only is considered and bootconfig2 is a backup.
 * When bootconfig2 is active in the older image and sysupgrade
 * is done to it, we copy the bootconfig2 to bootconfig1 so that the
 * failsafe parameters can be retained.
 */

	if (!bootconfig1 || !bootconfig2)
		goto free_memory;

	boot_info_dir = proc_mkdir("boot_info",NULL);
	upgrade_info_dir = proc_mkdir("upgrade_info",NULL);

	bc1_num_parts = bootconfig1->numaltpart;
	bc1_part_info = (struct per_part_info *)bootconfig1->per_part_entry;
	bootconfig1_info_dir = proc_mkdir("bootconfig0", boot_info_dir);

	bc2_num_parts = bootconfig2->numaltpart;
	bc2_part_info = (struct per_part_info *)bootconfig2->per_part_entry;
	bootconfig2_info_dir = proc_mkdir("bootconfig1", boot_info_dir);

	if(!bootconfig1_info_dir || !bootconfig2_info_dir)
		goto remove_empty_dir;

	for (i = 0; i < bc1_num_parts; i++) {
		if (!flash_type_emmc &&
				(strncmp(bc1_part_info[i].name, "kernel",
					ALT_PART_NAME_LENGTH) == 0))
			continue;
		bc1_partname_dir[i] = proc_mkdir(bc1_part_info[i].name, bootconfig1_info_dir);
		if (bc1_partname_dir != NULL) {
			proc_create_data("primaryboot", S_IRUGO,
					bc1_partname_dir[i],
					&primaryboot_ops,
					bc1_part_info + i);
			proc_create_data("upgradepartition", S_IRUGO,
					bc1_partname_dir[i],
					&upgradepartition_ops,
					bc1_part_info + i);
		}
	}

	for (i = 0; i < bc2_num_parts; i++) {
		if (!flash_type_emmc &&
				(strncmp(bc2_part_info[i].name, "kernel",
					ALT_PART_NAME_LENGTH) == 0))
			continue;
		bc2_partname_dir[i] = proc_mkdir(bc2_part_info[i].name, bootconfig2_info_dir);
		if (bc2_partname_dir != NULL) {
			proc_create_data("primaryboot", S_IRUGO,
					bc2_partname_dir[i],
					&primaryboot_ops,
					bc2_part_info + i);
			proc_create_data("upgradepartition", S_IRUGO,
					bc2_partname_dir[i],
					&upgradepartition_ops,
					bc2_part_info + i);
		}
	}

	if ((SMEM_DUAL_BOOTINFO_MAGIC_START_TRYMODE == bootconfig1->magic_start) ||
		(SMEM_DUAL_BOOTINFO_MAGIC_START_TRYMODE == bootconfig2->magic_start)) {
		try_feature = 1;
		printk("\nBootconfig: Try feature is enabled\n");

		proc_create_data("trybit", S_IRUGO, upgrade_info_dir,
				&trybit_ops,&trybit);

		trymode_inprogress = qcom_smem_get(QCOM_SMEM_HOST_ANY, SMEM_TRYMODE_INFO, &len);
		if(IS_ERR(trymode_inprogress))
		{
			pr_err("\nBootconfig: SMEM read failed\n");
			goto remove_part_dir;
		}

		if(1 == *trymode_inprogress){
			age_write_permession = WRITE_ENABLE;
			printk("\nBootconfig: Try mode in progress\n");
			proc_create_data("trymode_inprogress", S_IRUGO, upgrade_info_dir,
					&trymode_inprogress_ops, &age_write_permession);
		}

	} else {
		if(bootconfig1->age == bootconfig2->age) {
			bootconfig1->age++;
			bootconfig2->age++;
		} else if(bootconfig1->age > bootconfig2->age) {
			bootconfig2->age = ++bootconfig1->age;
		} else {
			bootconfig1->age = ++bootconfig2->age;
		}
	}

	proc_create_data("getbinary_bootconfig", S_IRUGO, bootconfig1_info_dir,
			&getbinary_ops, bootconfig1);

	proc_create_data("age", S_IRUGO, bootconfig1_info_dir,
			&age_ops, bootconfig1);

	proc_create_data("getbinary_bootconfig", S_IRUGO, bootconfig2_info_dir,
			&getbinary_ops, bootconfig2);

	proc_create_data("age", S_IRUGO, bootconfig2_info_dir,
			&age_ops, bootconfig2);

	return 0;

remove_part_dir:
	for (i = 0; i < bc1_num_parts; i++) {
		if (!flash_type_emmc &&
				(strncmp(bc1_part_info[i].name, "kernel",
					ALT_PART_NAME_LENGTH) == 0))
			continue;

		remove_proc_entry("primaryboot", bc1_partname_dir[i]);
		remove_proc_entry("upgradepartition", bc1_partname_dir[i]);
		remove_proc_entry(bc1_part_info[i].name, bootconfig1_info_dir);
	}

	for (i = 0; i < bc2_num_parts; i++) {
		if (!flash_type_emmc &&
				(strncmp(bc2_part_info[i].name, "kernel",
					 ALT_PART_NAME_LENGTH) == 0))
			continue;

		remove_proc_entry("primaryboot", bc2_partname_dir[i]);
		remove_proc_entry("upgradepartition", bc2_partname_dir[i]);
		remove_proc_entry(bc2_part_info[i].name, bootconfig2_info_dir);
	}

	if(1 == try_feature){
		remove_proc_entry("trybit",upgrade_info_dir);
	}
remove_empty_dir:
	bootconfig1_info_dir ? remove_proc_entry("bootconfig0",boot_info_dir):NULL;
	bootconfig2_info_dir ? remove_proc_entry("bootconfig1",boot_info_dir):NULL;
	remove_proc_entry("boot_info", NULL);
	remove_proc_entry("upgrade_info",NULL);
free_memory:
	bootconfig1 ? kfree(bootconfig1) : NULL;
	bootconfig2 ? kfree(bootconfig2) : NULL;

	return IS_ERR(trymode_inprogress) ? -ENOMEM:0;

}
module_init(bootconfig_partition_init);

static void __exit bootconfig_partition_exit(void)
{
	struct per_part_info *bc1_part_info;
	struct per_part_info *bc2_part_info;
	int i;

	if (!bootconfig1)
		return;

	if (!bootconfig2)
		return;

	bc1_part_info = (struct per_part_info *)bootconfig1->per_part_entry;
	for (i = 0; i < bc1_num_parts; i++) {
		if (!flash_type_emmc &&
				(strncmp(bc1_part_info[i].name, "kernel",
					ALT_PART_NAME_LENGTH) == 0))
			continue;

		remove_proc_entry("primaryboot", bc1_partname_dir[i]);
		remove_proc_entry("upgradepartition", bc1_partname_dir[i]);
		remove_proc_entry(bc1_part_info[i].name, bootconfig1_info_dir);
	}
	remove_proc_entry("getbinary_bootconfig", bootconfig1_info_dir);
	remove_proc_entry("age", bootconfig1_info_dir);

	bc2_part_info = (struct per_part_info *)bootconfig2->per_part_entry;
	for (i = 0; i < bc2_num_parts; i++) {
		if (!flash_type_emmc &&
				(strncmp(bc2_part_info[i].name, "kernel",
					 ALT_PART_NAME_LENGTH) == 0))
			continue;

		remove_proc_entry("primaryboot", bc2_partname_dir[i]);
		remove_proc_entry("upgradepartition", bc2_partname_dir[i]);
		remove_proc_entry(bc2_part_info[i].name, bootconfig2_info_dir);
	}
	remove_proc_entry("getbinary_bootconfig", bootconfig2_info_dir);
	remove_proc_entry("age", bootconfig1_info_dir);

	if(1 == *trymode_inprogress){
		remove_proc_entry("trymode_inprogress",upgrade_info_dir);
	}
	if(1 == try_feature){
		remove_proc_entry("trybit",upgrade_info_dir);
	}

	remove_proc_entry("bootconfig0",boot_info_dir);
	remove_proc_entry("bootconfig1",boot_info_dir);
	remove_proc_entry("boot_info", NULL);
	remove_proc_entry("upgrade_info",NULL);
	kfree(bootconfig1);
	kfree(bootconfig2);
}

module_exit(bootconfig_partition_exit);

MODULE_LICENSE("Dual BSD/GPL");
