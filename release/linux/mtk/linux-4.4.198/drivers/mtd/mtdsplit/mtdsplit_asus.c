/*
 *  Copyright (C) 2022 Zhaowei Xu <paldier@hotmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 *
 */

#define pr_fmt(fmt)	KBUILD_MODNAME ": " fmt

#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/byteorder/generic.h>

#include "mtdsplit.h"

#define IH_MAGIC			0x27051956	/* Image Magic Number		*/
#define MAX_HEADER_LEN		64
#define MAX_STRING			12
#define MAX_VER				4
#define IH_NMLEN		32	/* Image Name Length		*/

/* If hw[i].kernel == ROOTFS_OFFSET_MAGIC,
 * rootfilesystem offset (uImage header size + kernel size)
 * can be calculated by following equation:
 * (hw[i].minor << 16) | (hw[i+1].major << 8) | (hw[i+1].minor)
 */
#define ROOTFS_OFFSET_MAGIC	0xA9	/* Occupy two version_t		*/

typedef struct {
	uint8_t major;
	uint8_t minor; 
} version_t;

typedef struct {
	version_t kernel;
	version_t fs;
	char	  productid[MAX_STRING];
	uint16_t  sn;
	uint16_t  en;
	uint8_t   pkey;
	uint8_t   key;
	version_t hw[MAX_VER];
} TAIL;

typedef struct image_header {
	uint32_t	ih_magic;	/* Image Header Magic Number	*/
	uint32_t	ih_hcrc;	/* Image Header CRC Checksum	*/
	uint32_t	ih_time;	/* Image Creation Timestamp	*/
	uint32_t	ih_size;	/* Image Data Size		*/
	uint32_t	ih_load;	/* Data	 Load  Address		*/
	uint32_t	ih_ep;		/* Entry Point Address		*/
	uint32_t	ih_dcrc;	/* Image Data CRC Checksum	*/
	uint8_t		ih_os;		/* Operating System		*/
	uint8_t		ih_arch;	/* CPU architecture		*/
	uint8_t		ih_type;	/* Image Type			*/
	uint8_t		ih_comp;	/* Compression Type		*/
	union {
		uint8_t		ih_name[IH_NMLEN];	/* Image Name		*/
		TAIL		tail;		/* ASUS firmware infomation	*/
	} u;
} image_header_t;

static int
read_asus_header(struct mtd_info *mtd, size_t offset,u_char *buf,
		   size_t header_len)
{
	size_t retlen;
	int ret;

	ret = mtd_read(mtd, offset, header_len, &retlen, buf);
	if (ret) {
		printk("read error in \"%s\"\n", mtd->name);
		return ret;
	}

	if (retlen != header_len) {
		printk("short read in \"%s\"\n", mtd->name);
		return -EIO;
	}

	return 0;
}

static int
mtdsplit_parse_asus(struct mtd_info *master,
				   struct mtd_partition **pparts,
				   struct mtd_part_parser_data *data)
{
	struct mtd_partition *parts;
	u_char *buf;
	int nr_parts;
	size_t offset;
	size_t kernel_offset;
	size_t kernel_size = 0;
	size_t rootfs_offset = 0;
	size_t rootfs_size = 0;
	int ret;

	nr_parts = 2;
	parts = kzalloc(nr_parts * sizeof(*parts), GFP_KERNEL);
	if (!parts)
		return -ENOMEM;

	buf = vmalloc(MAX_HEADER_LEN);
	if (!buf) {
		ret = -ENOMEM;
		goto err;
	}

	/* find asus image on erase block boundaries */
	for (offset = 0; offset < master->size; offset += master->erasesize) {
		image_header_t *hdr;
		kernel_size = 0;

		ret = read_asus_header(master, offset, buf, MAX_HEADER_LEN);
		if (ret)
			continue;

		hdr = (image_header_t *)buf;
		if (be32_to_cpu(hdr->ih_magic) != IH_MAGIC) {
			printk("no valid asus header found in \"%s\" at offset %llx\n",
				 master->name, (unsigned long long) offset);
			continue;
		}
		if(hdr->u.tail.hw[3].major == ROOTFS_OFFSET_MAGIC){
			rootfs_offset = (hdr->u.tail.hw[3].minor << 16) | (hdr->u.tail.hw[4].major << 8) | (hdr->u.tail.hw[4].minor);
		}
		if(be32_to_cpu(hdr->ih_size) < 0x400000){//uImage header:only kernel
			rootfs_offset = 0x400000;
			rootfs_size = master->size - rootfs_offset;
			kernel_size = rootfs_offset;
		} else {// asus header:kernel+rootfs
			rootfs_size = master->size - rootfs_offset;
			kernel_size = rootfs_offset;
		}

		if ((offset + kernel_size) > master->size) {
			printk("asus image exceeds MTD device \"%s\"\n",
				 master->name);
			continue;
		}
		break;
	}

	if (kernel_size == 0) {
		printk("no asus header found in \"%s\"\n", master->name);
		ret = -ENODEV;
		goto err_free_buf;
	}

	kernel_offset = offset;

	if (rootfs_size == 0) {
		printk("no rootfs found in \"%s\"\n", master->name);
		ret = -ENODEV;
		goto err_free_buf;
	}

	parts[0].name = KERNEL_PART_NAME;
	parts[0].offset = kernel_offset;
	parts[0].size = kernel_size;

	parts[1].name = ROOTFS_PART_NAME;
	parts[1].offset = rootfs_offset;
	parts[1].size = rootfs_size;

	vfree(buf);

	*pparts = parts;
	return nr_parts;

err_free_buf:
	vfree(buf);

err:
	kfree(parts);
	return ret;
}

static struct mtd_part_parser asus_parser = {
	.owner = THIS_MODULE,
	.name = "asus-fw",
	.parse_fn = mtdsplit_parse_asus,
	.type = MTD_PARSER_TYPE_FIRMWARE,
};

static int __init mtdsplit_asus_init(void)
{
	register_mtd_parser(&asus_parser);

	return 0;
}

module_init(mtdsplit_asus_init);
