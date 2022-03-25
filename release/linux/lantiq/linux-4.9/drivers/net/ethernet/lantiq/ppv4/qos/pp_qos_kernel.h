/*
 * GPL LICENSE SUMMARY
 *
 *  Copyright(c) 2017 Intel Corporation.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of version 2 of the GNU General Public License as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 *  The full GNU General Public License is included in this distribution
 *  in the file called LICENSE.GPL.
 *
 *  Contact Information:
 *  Intel Corporation
 *  2200 Mission College Blvd.
 *  Santa Clara, CA  97052
 */
#ifndef _PP_QOS_KERNEL_H
#define _PP_QOS_KERNEL_H

#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/platform_device.h>

int qos_dbg_dev_init(struct platform_device *pdev);
void qos_dbg_dev_clean(struct platform_device *pdev);
int qos_dbg_module_init(void);
void qos_dbg_module_clean(void);

struct pp_qos_dbg_data {
	struct dentry *dir;
	uint16_t	node;
	uint8_t		raw_config;
	void		*fw_logger_addr;
};

struct fw_sec_info {
	u32	is_in_dccm;
	u32	dccm_offset;
	u32	max_size;
};

struct pp_qos_drv_data {
	int	id;
	struct pp_qos_dev *qdev;
	struct pp_qos_dbg_data dbg;
	void *ddr;
	void *dccm;
	void *ivt;
	void __iomem *wakeuc;
	resource_size_t ddr_phy_start;
	struct fw_sec_info fw_sec_data_stack;
};

#ifndef __BIG_ENDIAN
#define ENDIANESS_MATCH(endianess) (endianess == 0)
#else
#define ENDIANESS_MATCH(endianess) (endianess != 0)
#endif

#ifdef CONFIG_OF
#define PPV4_QOS_MANUAL_ADD(rc)
#define PPV4_QOS_MANUAL_REMOVE()
#else
#define PPV4_QOS_MANUAL_ADD(rc)		\
do {					\
	if (rc == 0)			\
		rc = add_qos_dev();	\
} while (0)
#define PPV4_QOS_MANUAL_REMOVE() remove_qos_dev()
#endif

#endif
