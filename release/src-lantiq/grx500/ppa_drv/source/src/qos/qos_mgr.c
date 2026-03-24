/******************************************************************************
 **
 ** FILE NAME	: qos_mgr_drv.c
 ** PROJECT	: UGW
 ** MODULES	: QoS Manager Driver Module
 **
 ** DESCRIPTION	: QoS Manager Driver Module
 ** COPYRIGHT  : Copyright (c) 2019 - 2020 Intel Corporation
 **
 *******************************************************************************/

#include <linux/cdev.h>
#include <linux/version.h>
#include <generated/autoconf.h>
#include <net/qos_mgr/qos_hal_api.h>

#include "qos_mgr_ioctl.h"
#include "qos_hal.h"
#include "qos_mgr_api.h"


/*
 *  device constant
 */
#define QOS_MGR_CHR_DEV_MAJOR                       182
#define QOS_MGR_DEV_NAME                            "qos_mgr"
static int qos_mgr_dev_open(struct inode *, struct file *);
static int qos_mgr_dev_release(struct inode *, struct file *);
static long qos_mgr_drv_dev_ioctl(struct file *, unsigned int, unsigned long);

/*
 * ####################################
 *           Global Variable
 * ####################################
 */

extern int g_qos_mgr_proc_dir_flag;
extern struct proc_dir_entry *g_qos_mgr_proc_dir;

static struct file_operations g_qos_mgr_dev_ops = {
	owner:      THIS_MODULE,
	unlocked_ioctl:      qos_mgr_drv_dev_ioctl,
	open:       qos_mgr_dev_open,
	release:    qos_mgr_dev_release
};

static dev_t qos_mgr_dev;
static struct cdev *qos_mgr_cdev;
static struct class *qos_mgr_class;
/*
 *  file operation functions
 */

static int qos_mgr_dev_open(struct inode *inode, struct file *file)
{
	return 0;
}

static int qos_mgr_dev_release(struct inode *inode, struct file *file)
{
	return 0;
}

static long qos_mgr_drv_dev_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	qos_mgr_dev_ioctl(file, cmd, arg);
	return 0;
}

static int qos_mgr_register_char_device(void)
{
	/* register the chardev region */
	qos_mgr_dev = MKDEV(QOS_MGR_CHR_DEV_MAJOR, 0);
	if( register_chrdev_region(qos_mgr_dev, 1, QOS_MGR_DEV_NAME)) {
		printk("register_chrdev_region failed %d\n", QOS_MGR_CHR_DEV_MAJOR);
		return -1;
	} 
	
	/* allocate the character device */
	qos_mgr_cdev = cdev_alloc();
	if(!qos_mgr_cdev) {
		printk("Allocate cdev failed\n");
		unregister_chrdev_region(qos_mgr_dev, 1);
		return -1;
	}

	/* init the character device */
	cdev_init(qos_mgr_cdev, &g_qos_mgr_dev_ops);

	/* create a sysfs class for qos_mgr */
	qos_mgr_class = class_create(THIS_MODULE, "qos_mgr");
	if(IS_ERR(qos_mgr_class)) {
		printk("Failed to create a sysfs class for qos_mgr\n");
		unregister_chrdev_region(qos_mgr_dev, 1);
		cdev_del(qos_mgr_cdev);
		return -1;
	}

	/* register with sysfs so that udev can get it */
	device_create(qos_mgr_class, NULL, qos_mgr_dev, NULL, "%s", "qos_mgr");

	/* add  to the system */
	if (cdev_add(qos_mgr_cdev, qos_mgr_dev, 1) < 0) {
		printk("Failed to add intel_qos_mgr as char device\n");
		device_destroy(qos_mgr_class, qos_mgr_dev);
		class_destroy(qos_mgr_class);
		cdev_del(qos_mgr_cdev);
		unregister_chrdev_region(qos_mgr_dev, 1);
		return -1;
	}

	return 0;	
}

static void qos_mgr_unregister_char_device(void)
{
	device_destroy(qos_mgr_class, qos_mgr_dev);
        class_destroy(qos_mgr_class);

	if(qos_mgr_cdev) {
		cdev_del(qos_mgr_cdev);
		unregister_chrdev_region(qos_mgr_dev, 1);
	}
}

static int __init qos_mgr_init(void)
{
	if (qos_hal_init() != QOS_MGR_SUCCESS) {
		pr_err("QoS HAL init failed.\n");
		return QOS_MGR_FAILURE;
	}
	
	if (qos_mgr_api_create() != QOS_MGR_SUCCESS) {
		pr_err("QoS Manager API init failed.\n");
		qos_hal_exit();
		return QOS_MGR_FAILURE;
	}

	if (qos_mgr_register_char_device() != QOS_MGR_SUCCESS) {
		pr_err("Failed to create QoS MGR device.\n");
		qos_hal_exit();
		qos_mgr_api_destroy();
		return QOS_MGR_FAILURE;
	}

	pr_info("QoS Manager Init Successful.\n");
	return QOS_MGR_SUCCESS;
}

static void __exit qos_mgr_exit(void)
{
	qos_mgr_unregister_char_device();
	qos_hal_exit();
}

module_init(qos_mgr_init);
module_exit(qos_mgr_exit);

MODULE_LICENSE("GPL");

