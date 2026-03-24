
/****************************************************************************** 
         Copyright 2020 - 2021 MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
 
  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.
 
******************************************************************************/

/*
 * Task of this module is to handle the ioctl request from user space
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <asm/uaccess.h>

#include "manager/ima_bonding_ioctl_if.h"
#include "common/ima_debug_manager.h"
#include "atm/ima_tx_module.h"

static dev_t ima_bond_dev = 0;
static struct class *ima_bond_class = NULL;
static struct mutex g_s_st_single_allow_mutex;

/*
 * Task of this function is to handle the device open call from user space
 */
static int ima_bond_dev_open(struct inode *inode, struct file *file)
{
	int ret = 0;

	/******************************************
	 * Will allow only one open of the device
	 ******************************************/
	if (mutex_is_locked(&g_s_st_single_allow_mutex))
		ret = -EBUSY;
	else
		mutex_lock(&g_s_st_single_allow_mutex);

	return ret;
}

/*
 * Task of this function is to handle the close operation from user space.
 */
static int ima_bond_dev_release(struct inode *inode, struct file *file)
{
	int ret = 0;

	if (mutex_is_locked(&g_s_st_single_allow_mutex))
		mutex_unlock(&g_s_st_single_allow_mutex);
	else
		ret = -EPERM;

	return ret;
}

/*
 * Task of this function is to handle the ioctl call from user space
 */
static long ima_bond_dev_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	int ret = 0;

	if (!capable(CAP_NET_ADMIN)) {
		ret = -EACCES;
		goto err_out;
	}

	switch (cmd) {

#ifdef ENABLE_IOCTL_DEBUG
	/* Transmit side ioctls */
	case IMA_IMAP_DEBUG_TX_SID_OFFSET:
	{
		st_tx_sid_offset_t tx_offset;

		ret = copy_from_user((void*)(&tx_offset), (void*)arg, sizeof(tx_offset));
		if (0 != ret) {
			pr_err("[%s, %d] IMA_IMAP_DEBUG_TX_SID_OFFSET copy_from_user() failed, ret = %d\n", __func__, __LINE__, ret);
			goto err_out;
		}

		ret = ima_debug_tx_sid_offset(tx_offset.offset, tx_offset.no_of_cells);

		break;
	}

	case IMA_IMAP_DEBUG_TX_SID_SET:
	{
		st_tx_sid_set_t tx_set;

		ret = copy_from_user((void*)(&tx_set), (void*)arg, sizeof(tx_set));
		if (0 != ret) {
			pr_err("[%s, %d] IMA_IMAP_DEBUG_TX_SID_SET copy_from_user() failed, ret = %d\n", __func__, __LINE__, ret);
			goto err_out;
		}

		ret = ima_debug_tx_sid_set(tx_set.value, tx_set.no_of_cells);

		break;
	}

	case IMA_IMAP_DEBUG_TX_QDC:
	{
		st_tx_qdc_t tx_qdc;

		ret = copy_from_user((void*)(&tx_qdc), (void*)arg, sizeof(tx_qdc));
		if (0 != ret) {
			pr_err("[%s, %d] IMA_IMAP_DEBUG_TX_QDC copy_from_user() failed, ret = %d\n", __func__, __LINE__, ret);
			goto err_out;
		}

		ret = ima_debug_tx_qdc(tx_qdc.delay);

		break;
	}

	case IMA_IMAP_DEBUG_TX_ABORT:
	{
		ret = ima_debug_tx_abort();
		break;
	}

	case IMA_IMAP_DEBUG_TX_FLUSH:
	{
		ret = ima_debug_tx_flush();
		break;
	}

	case IMA_IMAP_DEBUG_TX_DELAY:
	{
		st_tx_delay_t tx_delay;

		ret = copy_from_user((void*)(&tx_delay), (void*)arg, sizeof(tx_delay));
		if (0 != ret) {
			pr_err("[%s, %d] IMA_IMAP_DEBUG_TX_DELAY copy_from_user() failed, ret = %d\n", __func__, __LINE__, ret);
			goto err_out;
		}

		if (tx_delay.link > 1) {
			ret = 1;
			goto err_out;
		}

		ret = ima_debug_tx_delay(tx_delay.link, tx_delay.delay);

		break;
	}

	/* Receive side ioctls */
	case IMA_IMAP_DEBUG_RX_SID_OFFSET:
	{
		st_rx_sid_offset_t rx_offset;

		ret = copy_from_user((void*)(&rx_offset), (void*)arg, sizeof(rx_offset));
		if (0 != ret) {
			pr_err("[%s, %d] IMA_IMAP_DEBUG_RX_SID_OFFSET copy_from_user() failed, ret = %d\n", __func__, __LINE__, ret);
			goto err_out;
		}

		if (rx_offset.link > 1) {
			ret = 1;
			goto err_out;
		}

		ret = ima_debug_rx_sid_offset(rx_offset.offset, rx_offset.no_of_cells, rx_offset.link);

		break;
	}

	case IMA_IMAP_DEBUG_RX_SID_SET:
	{
		st_rx_sid_set_t rx_set;

		ret = copy_from_user((void*)(&rx_set), (void*)arg, sizeof(rx_set));
		if (0 != ret) {
			pr_err("[%s, %d] IMA_IMAP_DEBUG_RX_SID_SET copy_from_user() failed, ret = %d\n", __func__, __LINE__, ret);
			goto err_out;
		}

		if (rx_set.link > 1) {
			ret = 1;
			goto err_out;
		}

		ret = ima_debug_rx_sid_set(rx_set.value, rx_set.no_of_cells, rx_set.link);

		break;
	}

	case IMA_IMAP_DEBUG_RX_SID_SKIP:
	{
		st_rx_sid_skip_t rx_skip;

		ret = copy_from_user((void*)(&rx_skip), (void*)arg, sizeof(rx_skip));
		if (0 != ret) {
			pr_err("[%s, %d] IMA_IMAP_DEBUG_RX_SID_SKIP copy_from_user() failed, ret = %d\n", __func__, __LINE__, ret);
			goto err_out;
		}

		if (rx_skip.link > 1) {
			ret = 1;
			goto err_out;
		}

		ret = ima_debug_rx_sid_skip(rx_skip.no_of_cells, rx_skip.link);

		break;
	}

	case IMA_IMAP_DEBUG_RX_TIMEOUT:
	{
		st_rx_timeout_t rx_timeout;

		ret = copy_from_user((void*)(&rx_timeout), (void*)arg, sizeof(rx_timeout));
		if (0 != ret) {
			pr_err("[%s, %d] IMA_IMAP_DEBUG_RX_TIMEOUT copy_from_user() failed, ret = %d\n", __func__, __LINE__, ret);
			goto err_out;
		}

		ret = ima_debug_rx_timeout(rx_timeout.delay);

		break;
	}

	case IMA_IMAP_DEBUG_RX_FULL:
	{
		st_rx_full_t rx_full;

		ret = copy_from_user((void*)(&rx_full), (void*)arg, sizeof(rx_full));
		if (0 != ret) {
			pr_err("[%s, %d] IMA_IMAP_DEBUG_RX_FULL copy_from_user() failed, ret = %d\n", __func__, __LINE__, ret);
			goto err_out;
		}

		if (rx_full.link > 1) {
			ret = 1;
			goto err_out;
		}

		ret = ima_debug_rx_full(rx_full.link, rx_full.status);

		break;
	}

	/* General ioctls */
	case IMA_IMAP_DEBUG_STATS:
	{
		st_stats_t user_stats;

		ret = ima_debug_stats(&user_stats);
		if (0 != ret)
			goto err_out;

		ret = copy_to_user((void *)(arg), (void *)(&user_stats), sizeof(user_stats));
		break;
	}
	case IMA_IMAP_DEBUG_RX_ASM_GET:
	{
		st_print_asm_t print_asm;

		ret = copy_from_user((void*)(&print_asm), (void*)arg, sizeof(st_print_asm_t));
		if (0 != ret) {
			pr_err("[%s, %d] IMA_IMAP_DEBUG_RX_ASM_GET copy_from_user() failed, ret = %d\n", __func__, __LINE__, ret);
			goto err_out;
		}

		ret = ima_debug_rx_asm_get(print_asm.status);

		break;
	}
	case IMA_IMAP_DEBUG_TX_ASM_GET:
	{
		st_print_asm_t print_asm;

		ret = copy_from_user((void*)(&print_asm), (void*)arg, sizeof(st_print_asm_t));
		if (0 != ret) {
			pr_err("[%s, %d] IMA_IMAP_DEBUG_TX_ASM_GET copy_from_user() failed, ret = %d\n", __func__, __LINE__, ret);
			goto err_out;
		}

		ret = ima_debug_tx_asm_get(print_asm.status);

		break;
	}

#endif /* End: ENABLE_IOCTL_DEBUG */

	/**************************************************************
	 * This ioctl is out of ENABLE_IOCTL_DEBUG macro because
	 * release version shall support DSL line rate change
	 **************************************************************/
	case IMA_IMAP_QOS_SET_LINK_RATE:
	{
		struct st_link_rate rate;

		ret = copy_from_user(&rate, (void *)arg, sizeof(rate));
		if (ret != 0)
			goto err_out;

		if (!set_tx_dsl_info(rate.link0Rate, rate.link1Rate, 0, 0, 2)) {
			pr_crit( "Bonding Driver: Failed to apply line rate change (mbps): "
			         "[0:%d, 1:%d] Interleave delay (us): [0:%d 1:%d]\n",
					 rate.link0Rate, rate.link1Rate, 0, 0);

			goto err_out;
		}

		pr_crit( "Bonding Driver: Succeeded in applying Line rate change (mbps): "
				 "[0:%d, 1:%d] Interleave delay (us): [0:%d 1:%d]\n",
				 rate.link0Rate, rate.link1Rate, 0, 0);

		break;
	}

	default:
	{
		pr_err("[%s:%d] unknown command %d!\n", __func__, __LINE__, cmd);
		ret = -EINVAL;
	}

	} /* End: switch */

err_out:
	return ret;
}

static struct file_operations g_ima_bond_dev_ops = {
	.owner = THIS_MODULE,
	.unlocked_ioctl = ima_bond_dev_ioctl,
	.open = ima_bond_dev_open,
	.release = ima_bond_dev_release
};

/*
 * Task of this function is to initialize the ioctl module.
 */
s32 init_ioctl_interface(void)
{
	int ret;

	mutex_init(&g_s_st_single_allow_mutex);

	ret = __register_chrdev(0, 0, 1, IMA_BOND_DEV_NAME, &g_ima_bond_dev_ops);
	if (ret < 0) {
		pr_err("__register_chrdev() failed ret=%d!\n", ret);
		goto err_out;
	}
	ima_bond_dev = MKDEV(ret, 0);
	ret = 0;

	/* create a sysfs class for ima_bond */
	ima_bond_class = class_create(THIS_MODULE, IMA_BOND_DEV_NAME);
	if (IS_ERR(ima_bond_class)) {
		pr_err("Failed to create a sysfs class for ima_bond\n");
		ret = -1;
		goto err_out_unreg_chrdev;
	}

	/* creates a device and registers it with sysfs */
	device_create(ima_bond_class, NULL, ima_bond_dev, NULL, "%s", IMA_BOND_DEV_NAME);

	return ret;

err_out_unreg_chrdev:
	__unregister_chrdev(MAJOR(ima_bond_dev), 0, 1, IMA_BOND_DEV_NAME);
	ima_bond_dev = 0;

err_out:
	return ret;
}

/*
 * Task of this function is to cleanup the ioctl module
 */
s32 cleanup_ioctl_interface(void)
{
	s32 ret = RS_SUCCESS;

	if (mutex_is_locked(&g_s_st_single_allow_mutex) == 1)
		mutex_unlock(&g_s_st_single_allow_mutex);

	device_destroy(ima_bond_class, ima_bond_dev);
	class_destroy(ima_bond_class);

	__unregister_chrdev(MAJOR(ima_bond_dev), 0, 1, IMA_BOND_DEV_NAME);
	ima_bond_dev = 0;

	return ret;
}
