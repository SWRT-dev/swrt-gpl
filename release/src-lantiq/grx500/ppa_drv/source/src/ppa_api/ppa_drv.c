/*******************************************************************************
 **
 ** FILE NAME    : ppa_drv.c
 ** PROJECT      : PPA
 ** MODULES      : PPA API (Routing/Bridging Acceleration APIs)
 **
 ** DATE         : 3 NOV 2008
 ** AUTHOR       : Xu Liang
 ** DESCRIPTION  : PPA Protocol Stack Hook API Miscellaneous Functions
 ** COPYRIGHT    :              Copyright (c) 2009
 **                          Lantiq Deutschland GmbH
 **                   Am Campeon 3; 85579 Neubiberg, Germany
 **
 **   For licensing information, see the file 'LICENSE' in the root folder of
 **   this software module.
 **
 ** HISTORY
 ** $Date        $Author                $Comment
 ** 03 NOV 2008  Xu Liang               Initiate Version
 ** 10 DEC 2012  Manamohan Shetty       Added the support for RTP,MIB mode
 **                                     Features
 *******************************************************************************/



/*
 * ####################################
 *              Head File
 * ####################################
 */

/*
 *  Common Head File
 */
//#include <linux/autoconf.h>
#include <linux/version.h>
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 33)
#include <linux/autoconf.h>
#else
#include <generated/autoconf.h>
#endif

#include <linux/cdev.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/netdevice.h>
#include <linux/in.h>
#include <net/sock.h>
#include <asm/time.h>

/*
 *  PPA Specific Head File
 */
#include <net/ppa/ppa_api.h>
#include <net/ppa/ppa_hook.h>

#include "ppa_api_netif.h"
#include "ppa_api_misc.h"
#include "ppa_api_proc.h"
#if IS_ENABLED(CONFIG_PPA_QOS)
#include "ppa_api_qos.h"
#endif

/*
 * ####################################
 *              Definition
 * ####################################
 */

/*
 *  device constant
 */
#define PPA_CHR_DEV_MAJOR                       181
#define PPA_DEV_NAME                            "ppa"

/*
 * ####################################
 *              Data Type
 * ####################################
 */

/*
 * ####################################
 *             Declaration
 * ####################################
 */

static int ppa_dev_open(struct inode *, struct file *);
static int ppa_dev_release(struct inode *, struct file *);
static long ppa_drv_dev_ioctl(struct file *, unsigned int, unsigned long);

/*
 * ####################################
 *           Global Variable
 * ####################################
 */

extern int g_ppa_proc_dir_flag;
extern struct proc_dir_entry *g_ppa_proc_dir;

static struct file_operations g_ppa_dev_ops = {
	owner:      THIS_MODULE,
	unlocked_ioctl:      ppa_drv_dev_ioctl,
	open:       ppa_dev_open,
	release:    ppa_dev_release
};

static dev_t ppa_dev;
static struct cdev *ppa_cdev;
static struct class *ppa_class;
/*
 * ####################################
 *           Extern Variable
 * ####################################
 */
extern struct proc_dir_entry proc_root;

/*
 * ####################################
 *            Local Function
 * ####################################
 */

/*
 *  file operation functions
 */

static int ppa_dev_open(struct inode *inode, struct file *file)
{
	return 0;
}

static int ppa_dev_release(struct inode *inode, struct file *file)
{
	return 0;
}

static long ppa_drv_dev_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
#if IS_ENABLED(CONFIG_PPA_ACCEL)
	ppa_dev_ioctl(file, cmd, arg);
#endif
	return 0;
}

#ifdef PPA_API_PROC
static void ppa_drv_proc_file_create(void)
{
	if ( !g_ppa_proc_dir_flag )
	{
		g_ppa_proc_dir = proc_mkdir("ppa", NULL);
		g_ppa_proc_dir_flag = 1;
	}
}

static void ppa_drv_proc_file_delete(void)
{
	if ( g_ppa_proc_dir_flag )
	{
		remove_proc_entry("ppa", NULL);
		g_ppa_proc_dir = NULL;
		g_ppa_proc_dir_flag = 0;
	}
}
#endif

/*
 * ####################################
 *           Init/Cleanup API
 * ####################################
 */
/*NOTE: this part of the code is linux specific */
static int ppa_register_char_device(void)
{
	/* register the chardev region */
	ppa_dev = MKDEV(PPA_CHR_DEV_MAJOR, 0);
	if( register_chrdev_region(ppa_dev, 1, PPA_DEV_NAME)) {
		printk("register_chrdev_region failed %d\n", PPA_CHR_DEV_MAJOR);
		return -1;
	} 
	
	/* allocate the character device */
	ppa_cdev = cdev_alloc();
	if(!ppa_cdev) {
		printk("Allocate cdev failed\n");
		unregister_chrdev_region(ppa_dev, 1);
		return -1;
	}

	/* init the character device */
	cdev_init(ppa_cdev, &g_ppa_dev_ops);

	/* create a sysfs class for ppa */
	ppa_class = class_create(THIS_MODULE, "ppa");
	if(IS_ERR(ppa_class)) {
		printk("Failed to create a sysfs class for ppa\n");
		unregister_chrdev_region(ppa_dev, 1);
		cdev_del(ppa_cdev);
		return -1;
	}

	/* register with sysfs so that udev can get it */
	device_create(ppa_class, NULL, ppa_dev, NULL, "%s", "ifx_ppa");

	/* add  to the system */
	if (cdev_add(ppa_cdev, ppa_dev, 1) < 0) {
		printk("Failed to add intel_ppa as char device\n");
		device_destroy(ppa_class, ppa_dev);
		class_destroy(ppa_class);
		cdev_del(ppa_cdev);
		unregister_chrdev_region(ppa_dev, 1);
		return -1;
	}

	return 0;	
}

static void ppa_unregister_char_device(void)
{
	device_destroy(ppa_class, ppa_dev);
        class_destroy(ppa_class);

	if(ppa_cdev) {
		cdev_del(ppa_cdev);
		unregister_chrdev_region(ppa_dev, 1);
	}
}

int __init ppa_drv_init(void)
{
	int ret;
	if ( ppa_api_netif_manager_create() != PPA_SUCCESS )
		goto PPA_API_NETIF_MANAGER_CREATE_FAIL;

#if IS_ENABLED(CONFIG_PPA_ACCEL)
	ret = ppa_api_init();
	if ( ret != 0 ) {
		printk("PPA API --- Init failed\n");
		goto PPA_DRV_FAIL;
	}
#endif
#ifdef PPA_API_PROC
	ppa_drv_proc_file_create();
#endif
	ret = ppa_register_char_device();
	if ( ret != 0 ) {
		printk("PPA DRV --- failed in register_chrdev(%d, %s, g_ppa_dev_ops), errno = %d\n", PPA_CHR_DEV_MAJOR, PPA_DEV_NAME, ret);
		goto PPA_DRV_FAIL;
	}

	printk("PPA DRV --- init successfully\n");
	return 0;
PPA_DRV_FAIL:
PPA_API_NETIF_MANAGER_CREATE_FAIL:
	ppa_api_netif_manager_destroy();

	return -1;
}

void __exit ppa_drv_exit(void)
{
#ifdef PPA_API_PROC
	ppa_drv_proc_file_delete();
#endif
	ppa_api_netif_manager_destroy();
	ppa_unregister_char_device();
#if IS_ENABLED(CONFIG_PPA_ACCEL)
	ppa_api_exit();
#endif /* CONFIG_PPA_ACCEL */
}

module_init(ppa_drv_init);
module_exit(ppa_drv_exit);

MODULE_LICENSE("GPL");

