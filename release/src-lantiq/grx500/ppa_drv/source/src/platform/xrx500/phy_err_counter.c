/******************************************************************************
 **
 ** FILE NAME    : phy_err_counter.c
 ** PROJECT      : PPA
 **
 ** DATE         : 25th February 2020
 ** AUTHOR       : Moinak Debnath
 ** DESCRIPTION  : Code violated Phy level error statistics
 ** COPYRIGHT    : Copyright (c) 2020 Intel Corporation
 **
 **   For licensing information, see the file 'LICENSE' in the root folder of
 **   this software module.
 **
 ** HISTORY
 ** $Date        		$Author         	$Comment
 ** 25th February 2020		Moinak Debnath		Initiate Version
 *******************************************************************************
 */

/* ####################################
 *              Head File
 * ####################################
 */

#include <linux/version.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/init.h>
#include <linux/of_net.h>
#include <linux/of_mdio.h>
#include <linux/of_device.h>
#include <net/switchdev.h>
#include <linux/debugfs.h>
#include <lantiq.h>
#include <net/datapath_api.h>
#include <net/datapath_proc_api.h>
#include <net/switch_api/lantiq_gsw_api.h>
#include <net/switch_api/gsw_flow_ops.h>
#include <net/switch_api/gsw_dev.h>
#include <linux/kthread.h>
#include "phy_err_counter.h"

struct task_struct *thread 	= NULL;
phy_err_node *phy_err		= NULL; 
uint32_t enable_phy_err_cnt	= 1;
uint8_t phy_err_port_num	= 0; 
static uint32_t poll_int	= (PHY_ERR_POLL_MIN + PHY_ERR_POLL_MAX) >> 2;

/*
 *  string process helper function
 */

static int stricmp(const char *p1, const char *p2)
{
	int c1, c2;

	while (*p1 && *p2) {
		c1 = *p1 >= 'A' && *p1 <= 'Z' ? *p1 + 'a' - 'A' : *p1;
		c2 = *p2 >= 'A' && *p2 <= 'Z' ? *p2 + 'a' - 'A' : *p2;
		if ((c1 -= c2))
			return c1;
		p1++;
		p2++;
	}

	return *p1 - *p2;
}

uint16_t mdio_err_count_read(uint8_t is_wan, uint8_t addr)
{
	GSW_MDIO_data_t mmd_data;
	struct core_ops *ops;

	ops = gsw_get_swcore_ops(is_wan);

	if (!ops) {
		pr_err("%s: Open SWAPI device FAILED!, is_wan: %u\n", __func__, is_wan);
		return 0;
	}

	memset((void *)&mmd_data, 0x00, sizeof(mmd_data));
	mmd_data.nAddressDev = addr;
	mmd_data.nAddressReg = MDIO_PHY_ERR_CNT_REG;
	ops->gsw_common_ops.MDIO_DataRead(ops, &mmd_data);

	return mmd_data.nData;
}

bool GSW_get_phy_err_count(void)
{
	uint8_t port;
	uint16_t count;
	bool detected = false;

	for (port = 1; port < PHY_ERR_PORTS; ++port) {
		count = mdio_err_count_read(phy_err[port].is_wan, port);
		phy_err[port].err_count += count;
		if (count != 0)
			detected = true;
	}

	return detected;
}

int phy_err_cnt_thread(void *data)
{
	bool detected = false;
	uint8_t retry = RETRY_TIME_DEFAULT;

	while (!is_xway_gphy_fw_loaded() && retry--) {
		msleep(100);
	}

	if (retry == 0) {
		pr_err("<%s> gphy fw not loaded, so not starting phy err count kthread!\n", __func__);
		return 0;
	}

	while (!kthread_should_stop()) {
		detected = GSW_get_phy_err_count();

		/* error detected, set poll time to POLL_MIN */
		if (detected) {
			poll_int = PHY_ERR_POLL_MIN;
		} else {
			poll_int++;
			if (poll_int > PHY_ERR_POLL_MAX)
				poll_int = PHY_ERR_POLL_MAX;
		}
		msleep(poll_int); /* msleep expects input in mili secs */
	}
	pr_info("<%s> phy err kthread stopped!\n", __func__);

	return 0;
}

/* phy err enable/disable debugfs */
void  phy_err_read(struct seq_file *s)
{
	uint8_t port;

	if (!capable(CAP_NET_ADMIN)) {
		pr_info ("Read Permission denied\n");
		return;
	}

	seq_printf(s, "PHY_ERR is: %s\n", enable_phy_err_cnt ? "ENABLED" : "DISABLED");
	if (enable_phy_err_cnt)
		seq_printf(s, "kthread poll interval is at : %d mili-secs\n", poll_int);

	seq_printf(s, "\n\t\t --ETHERNET CODE VIOLATION STATISTICS--\n\n");
	seq_printf(s, "\t\tport\t\tLAN/WAN\t\tcount\n");
	for (port = 1; port < PHY_ERR_PORTS; ++port)
		seq_printf(s, "\t\t  %u \t\t  %s \t\t  %llu\n", port, phy_err[port].is_wan ? "WAN" : "LAN", phy_err[port].err_count);

	return;
}

ssize_t write_phy_err(struct file *file,
				       const char __user *buf, size_t count,
				       loff_t *data)
{
	int len, ret = 0;
	char str[64];
	char *p;

	if (!capable(CAP_NET_ADMIN)) {
		pr_info ("Write Permission denied\n");
		return 0;
	}

	len = min(count, (size_t)(sizeof(str) - 1));
	len -= copy_from_user(str, buf, len);
	while (len && (str[len - 1] <= ' '))
		len--;

	str[len] = 0;
	for (p = str; *p && (*p <= ' '); p++, len--)
		;
	if (!*p)
		return count;

	if (stricmp(p, "disable") == 0) {
		pr_info("Diabling phy err cnt kthread!\n");
		enable_phy_err_cnt = 0;
		if (thread) {
			ret = kthread_stop(thread);
			if (ret != -EINTR)
				pr_info("kthread stopped\n");

			thread = NULL;
		}
	} else if (stricmp(p, "enable") == 0) {
		enable_phy_err_cnt = 1;
		if (!thread) {
			thread = kthread_run(phy_err_cnt_thread, NULL, "phyerr_kthread");
			if (IS_ERR(thread))
				pr_err("ERR in starting kthread!\n");
			else
				pr_info("Starting kthread from proc success!\n");
		}
		pr_info("Enabling  phy err cnt kthread!\n");
	} else {
		pr_info("Usage: echo (enable/disable) > /sys/kernel/debug/eth/phy_err_counter\n");
	}

	return len;
}
static struct dp_proc_entry proc_entries[] = {
	{PROC_FILE_PHY_ERR, phy_err_read, NULL, NULL, write_phy_err}
};

int phy_err_proc_install(struct dentry *parent)
{
	int i;
	struct dentry *driver_proc_node;
	driver_proc_node = debugfs_create_dir(PHY_ERR_PROC_PARENT PHY_ERR_PROC_NAME,
					      parent);
	if (driver_proc_node != NULL) {
		for (i = 0; i < ARRAY_SIZE(proc_entries); ++i)
			dp_proc_entry_create(driver_proc_node,
					     &proc_entries[i]);
	} else {
		pr_err("cannot create sysfs entry for phy err counter\n");
		return -1;
	}
	return 0;
}

static int __init phy_err_counter_init(void)
{
	phy_err = (phy_err_node *) kmalloc (sizeof(phy_err_node) * PHY_ERR_PORTS, GFP_KERNEL);
	memset(phy_err, 0, sizeof(phy_err_node) *  PHY_ERR_PORTS);
	phy_err_proc_install(NULL);
	thread = kthread_run(phy_err_cnt_thread, NULL, "phy_err_cnt_kthread");

	if (IS_ERR(thread))
		pr_err("ERR in starting kthread from init!\n");
	else
		pr_err("Starting kthread from init success!\n");

	phy_err[WAN_PORT_PHY].is_wan = true;
		
	pr_info("MODULE Phy Err Counter Stats Init Success!\n");

	return 0;
}

static void __exit phy_err_counter_exit(void)
{
	int ret = 0;
	remove_proc_entry(PHY_ERR_PROC_PARENT PHY_ERR_PROC_NAME, 0);
	if (thread) {
		ret = kthread_stop(thread);
		if (ret != -EINTR)
			pr_err("kthread stopped\n");

		thread = NULL;
	}
	kfree(phy_err);
	phy_err = NULL;

	pr_info("MODULE Phy Err Counter Stats Exit Success!\n");
}

module_init(phy_err_counter_init);
module_exit(phy_err_counter_exit);

MODULE_AUTHOR("Moinak Debnath");
MODULE_DESCRIPTION("Code violated Phy level error statistics");
MODULE_LICENSE("GPL");
