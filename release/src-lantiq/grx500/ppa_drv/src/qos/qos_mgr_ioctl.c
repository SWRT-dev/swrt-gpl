/******************************************************************************
 **
 ** FILE NAME	: qos_mgr_ioctl.c
 ** PROJECT	: UGW
 ** MODULES	: QoS Manager ioctls
 **
 ** DATE	: 08 Jan 2020
 ** AUTHOR	: Mohammed Aarif
 ** DESCRIPTION	: QoS Manager Ioctl Implementation
 ** COPYRIGHT  : Copyright (c) 2019 - 2020 Intel Corporation
 **
 ** HISTORY
 ** $Date		$Author			$Comment
 ** 08 JAN 2020		Mohammed Aarif		Initiate Version
 *******************************************************************************/

/*
 * ####################################
 *			  Head File
 * ####################################
 */

/*
 *  Common Head File
 */
#include <linux/version.h>
#include <generated/autoconf.h>

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

#include "qos_mgr_stack_al.h"

#include "qos_mgr_ioctl.h"
#include "qos_mgr_api.h"


/*
 * ####################################
 *			  Definition
 * ####################################
 */


/*
 * ####################################
 *			  Data Type
 * ####################################
 */

/*
 * ####################################
 *			 Declaration
 * ####################################
 */

int32_t qos_mgr_ioctl_get_hook_list(unsigned int cmd, unsigned long arg, QOS_MGR_CMD_DATA * cmd_info);
int32_t qos_mgr_ioctl_set_hook(unsigned int cmd, unsigned long arg, QOS_MGR_CMD_DATA * cmd_info);

/*
 * ####################################
 *		   Global Variable
 * ####################################
 */

extern int g_qos_mgr_proc_dir_flag;
extern struct proc_dir_entry *g_qos_mgr_proc_dir;

/*
 * ####################################
 *		   Extern Variable
 * ####################################
 */
extern struct proc_dir_entry proc_root;

/*
 * ####################################
 *			Local Function
 * ####################################
 */

/*
 *  file operation functions
 */

#if defined(FIX_MEM_SIZE)
struct uc_session_node tmp_session_buf[FIX_MEM_SIZE];
#endif

long qos_mgr_dev_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	int res = 0;

	QOS_MGR_CMD_DATA *cmd_info = (QOS_MGR_CMD_DATA *)qos_mgr_malloc(sizeof(QOS_MGR_CMD_DATA));

	if ( cmd_info == NULL )
		return -EFAULT;

	if ( qos_mgr_ioc_type(cmd) != QOS_MGR_IOC_MAGIC ) {
		printk("qos_mgr_ioc_type(%08X - %d) != QOS_MGR_IOC_MAGIC(%d)\n", cmd, _IOC_TYPE(cmd), QOS_MGR_IOC_MAGIC);
		goto EXIT_EIO;
	} else if( qos_mgr_ioc_nr(cmd) >= QOS_MGR_IOC_MAXNR ) {
		printk("Current cmd is %02x wrong, it should less than %02x\n", _IOC_NR(cmd), QOS_MGR_IOC_MAXNR );
		goto EXIT_EIO;
	}

	if ( ((qos_mgr_ioc_dir(cmd) & qos_mgr_ioc_read()) && !access_ok(VERIFY_WRITE, arg, qos_mgr_ioc_size(cmd)))
			|| ((qos_mgr_ioc_dir(cmd) & qos_mgr_ioc_write()) && !access_ok(VERIFY_READ, arg, qos_mgr_ioc_size(cmd))) ) {
		printk("access check: (%08X && %d) || (%08X && %d)\n",
				(qos_mgr_ioc_dir(cmd) & qos_mgr_ioc_read()),
				(int)!qos_mgr_ioc_access_ok(qos_mgr_ioc_verify_write(), arg, qos_mgr_ioc_size(cmd)),
				(qos_mgr_ioc_dir(cmd) & qos_mgr_ioc_write()),
				(int)!qos_mgr_ioc_access_ok(qos_mgr_ioc_verify_read(), arg, qos_mgr_ioc_size(cmd)));
		goto EXIT_EFAULT;
	}


	switch ( cmd ) {
		case QOS_MGR_CMD_SET_CTRL_QOS_RATE: {
			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;
			res = qos_mgr_ioctl_set_ctrl_qos_rate(cmd, arg, cmd_info );
			break;
		}
		case QOS_MGR_CMD_GET_CTRL_QOS_RATE: {
			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;
			res = qos_mgr_ioctl_get_ctrl_qos_rate(cmd, arg, cmd_info );
			break;
		}
		case QOS_MGR_CMD_SET_QOS_RATE: {
			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;
			res = qos_mgr_ioctl_set_qos_rate(cmd, arg, cmd_info );
			break;
		}
		case QOS_MGR_CMD_RESET_QOS_RATE: {
			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;
			res = qos_mgr_ioctl_reset_qos_rate(cmd, arg, cmd_info );
			break;
		}
		case QOS_MGR_CMD_GET_QOS_RATE: {
			res = qos_mgr_ioctl_get_qos_rate(cmd, arg, cmd_info );
			break;
		}
		case QOS_MGR_CMD_ADD_QOS_QUEUE: {
			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;
			res = qos_mgr_ioctl_add_qos_queue(cmd, arg, cmd_info );
			break;
		}
		case QOS_MGR_CMD_MOD_QOS_QUEUE: {
			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;
			res = qos_mgr_ioctl_modify_qos_queue(cmd, arg, cmd_info );
			break;
		}
		case QOS_MGR_CMD_DEL_QOS_QUEUE: {
			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;
			res = qos_mgr_ioctl_delete_qos_queue(cmd, arg, cmd_info );
			break;
		}
		case QOS_MGR_CMD_ENG_QUEUE_INIT: {
			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;
			res = qos_mgr_ioctl_qos_init_cfg(cmd);
			break;
		}
		case QOS_MGR_CMD_ENG_QUEUE_UNINIT: {
			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;
			res = qos_mgr_ioctl_qos_init_cfg(cmd);
			break;
		}
		case QOS_MGR_CMD_MOD_SUBIF_PORT: {
			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;
			res = qos_mgr_ioctl_mod_subif_port_config(cmd, arg, cmd_info );
			break;
		}
		case QOS_MGR_CMD_SET_QOS_INGGRP: {
			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;
			res = qos_mgr_ioctl_set_qos_ingress_group(cmd, arg, cmd_info );
			break;
		}
		case QOS_MGR_CMD_GET_QOS_INGGRP: {
			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;
			res = qos_mgr_ioctl_get_qos_ingress_group(cmd, arg, cmd_info );
			break;
		}
		case QOS_MGR_CMD_SET_QOS_SHAPER: {
			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;
			res = qos_mgr_ioctl_set_qos_shaper(cmd, arg, cmd_info );
			break;
		}
		case QOS_MGR_CMD_GET_QOS_SHAPER: {
			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;
			res = qos_mgr_ioctl_get_qos_shaper(cmd, arg, cmd_info );
			break;
		}
		default:
			qos_mgr_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"cmd not supported:0x%0x(its nr value=0x%0x)\n", cmd, qos_mgr_ioc_nr(cmd) );
			goto EXIT_ENOIOCTLCMD;
	}
	if ( res != QOS_MGR_SUCCESS ) {
		goto EXIT_EIO;
	} else
		goto EXIT_ZERO;

EXIT_EIO:
	if (res != -EINVAL)
		res = -EIO;
	goto EXIT_LAST;

EXIT_EFAULT:
	res = -EFAULT;
	goto EXIT_LAST;
EXIT_ZERO:
	res = 0;
	goto EXIT_LAST;
EXIT_ENOIOCTLCMD:
	res = ENOIOCTLCMD;
	goto EXIT_LAST;
EXIT_LAST:
	if( cmd_info )
		qos_mgr_free(cmd_info);
	return res;
}

/* since from no where calling following functions, commented*/
int32_t qos_mgr_ioctl_bridge_enable(unsigned int cmd, unsigned long arg, QOS_MGR_CMD_DATA *cmd_info)
{
	return QOS_MGR_SUCCESS;
}

int32_t qos_mgr_ioctl_get_bridge_enable_status(unsigned int cmd, unsigned long arg, QOS_MGR_CMD_DATA *cmd_info)
{
	return QOS_MGR_SUCCESS;
}
