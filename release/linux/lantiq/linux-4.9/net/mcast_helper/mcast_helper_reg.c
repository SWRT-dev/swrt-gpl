/******************************************************************************
 **
 ** FILE NAME    : mcast_helper_reg.c
 ** AUTHOR       :
 ** DESCRIPTION  : Multicast Helper Register function
 ** COPYRIGHT    :      Copyright (c) 2014 2015
 **                Lantiq Beteiligungs-GmbH & Co. KG
 **
 **    This program is free software; you can redistribute it and/or modify
 **    it under the terms of the GNU General Public License as published by
 **    the Free Software Foundation; either version 2 of the License, or
 **    (at your option) any later version.
 **
 ** HISTORY
 ** $Date         $Author                $Comment
 ** 26 AUG 2014                 	      Initial Version
 **
 *******************************************************************************/



#if IS_ENABLED(CONFIG_MCAST_HELPER_REG)

#include <linux/kernel.h>
#include <linux/netdevice.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/errno.h>
#include <linux/netdevice.h>
#include <linux/netlink.h>
#include <net/netlink.h>
#include <net/net_namespace.h>
#include <linux/string.h>
#include <linux/rtnetlink.h>

#define SUCCESS 0x1
#define FAILURE 0x0

/* reg/dereg flag definition */
#define LTQ_MC_F_REGISTER 0x01
#define LTQ_MC_F_DEREGISTER 0x02
#define LTQ_MC_F_DIRECTPATH 0x04
#define LTQ_MC_F_UPDATE_MAC_ADDR 0x08
#define LTQ_MC_F_FW_RESET 0x10
#define LTQ_MC_F_NEW_STA  0x20
#define LTQ_MC_F_DISCONN_MAC 0x40

/* callback flag definition */
#define LTQ_MC_F_ADD 0x01
#define LTQ_MC_F_DEL 0x02
#define LTQ_MC_F_UPD 0x03
#define LTQ_MC_F_DEL_UPD 0x08

#define MCAST_PROCESS_NAME "mcastd"

#define IF_INFO_SIZE	NLMSG_ALIGN(sizeof(struct ifinfomsg))
#define TOTAL_SIZE	NLMSG_ALIGN(sizeof(struct ifinfomsg)) + nla_total_size(MAX_ADDR_LEN)

typedef void (*Mcast_module_callback_t)(unsigned int grpidx, struct net_device *netdev, void *mc_stream, unsigned int flag);
typedef struct _mcast_callback_t{
	int numCbf;
	struct net_device *netDev; /* Member NetDevice */
	struct module *modName;
	Mcast_module_callback_t callbackfun;
	struct list_head list;	/* mcast_member interface map  List  */
	uint32_t uflag;
} MCAST_CALLBACK_t;

LIST_HEAD(mch_callback_list_g);

static int  mcast_helper_send_netlink_msg_user(struct net_device *netdev ,int type, int queryflag, void *data)
{
	struct sk_buff *skb;
	struct nlmsghdr *nlh;
	struct ifinfomsg *ifm;
	struct net *net = dev_net(netdev);
	unsigned int pid;
	int res, res_v6;
	struct task_struct *task;
	struct sk_buff *skb_v6;

	//printk(KERN_INFO "Creating skb.\n");
	for_each_process(task) {
		//printk(KERN_INFO "task info process name :%s pid:%d \n",task->comm,task->pid);
		/* compare the  process name with each of the task struct process name*/
		if (strstr(task->comm,MCAST_PROCESS_NAME) != NULL) {
			pid = task->pid;
			skb = nlmsg_new(TOTAL_SIZE, GFP_KERNEL);
			if (!skb) {
				//printk(KERN_INFO "Allocation failure.\n");
				return FAILURE;
			}
			nlh = nlmsg_put(skb, 0, 0, type, IF_INFO_SIZE, 0);
			if (nlh == NULL) {
				kfree_skb(skb);
				return -EMSGSIZE;
			}
			nlh->nlmsg_type = type;
			ifm = nlmsg_data(nlh);
			ifm->ifi_family = AF_UNSPEC;
			ifm->__ifi_pad = 0;
			ifm->ifi_type = netdev->type;
			ifm->ifi_index = netdev->ifindex;
			if (type == RTM_NEWLINK)
				ifm->ifi_flags = IFF_UP;
			if (queryflag)
				ifm->ifi_flags = ifm->ifi_flags | IFF_SLAVE;

			ifm->ifi_change = 0;
			if (data != NULL) {
				nla_put(skb, NLA_BINARY, MAX_ADDR_LEN, data);
				skb_v6 = skb_clone(skb, GFP_ATOMIC);
				if (!skb_v6) {
					pr_info("Allocation failure.\n");
					return FAILURE;
				}
				res = nlmsg_multicast(net->rtnl, skb, 0, RTNLGRP_LINK | RTNLGRP_IPV4_IFADDR, GFP_KERNEL);
				res_v6 = nlmsg_multicast(net->rtnl, skb_v6, 0, RTNLGRP_LINK | RTNLGRP_IPV6_IFADDR, GFP_KERNEL);
				if (res < 0 || res_v6 < 0)
					return FAILURE;
				else
					return SUCCESS;
			}
			res = netlink_unicast(net->rtnl, skb, pid, MSG_DONTWAIT);
			return res;
		}
	}
	return FAILURE;
}

/** mch_add_gimc_record - Add gitxmc entry */
static int mcast_helper_reg_callback(struct net_device *netdev,
		Mcast_module_callback_t *fun, struct module *modName,
		struct list_head *head, unsigned int flags)
{
	MCAST_CALLBACK_t *mc_callback_rec = NULL;

	if (netdev == NULL)
		return FAILURE;

	mc_callback_rec = kmalloc(sizeof(MCAST_CALLBACK_t), GFP_KERNEL);
	if (mc_callback_rec == NULL)
		return FAILURE;

	mc_callback_rec->netDev = netdev;
	mc_callback_rec->callbackfun = (void *)fun;
	mc_callback_rec->modName = modName;
	mc_callback_rec->uflag = flags & LTQ_MC_F_DIRECTPATH;
	INIT_LIST_HEAD(&mc_callback_rec->list);
	list_add_tail(&mc_callback_rec->list, head);

	if ((flags & LTQ_MC_F_FW_RESET) == LTQ_MC_F_FW_RESET)
		mcast_helper_send_netlink_msg_user(netdev, RTM_NEWLINK, 0, NULL);

	return SUCCESS;
}

/** delete_gimc_record - delete gimc entry */
static int mcast_helper_dereg_callback(struct net_device *netdev,
		Mcast_module_callback_t *fun, struct module *modName,
		struct list_head *head, unsigned int flags)

{
	struct list_head *liter = NULL;
	struct list_head *gliter = NULL;
	MCAST_CALLBACK_t *mc_callback_rec = NULL;

	if (netdev == NULL)
		return FAILURE;

	list_for_each_safe(liter, gliter, head) {
		mc_callback_rec = list_entry(liter, MCAST_CALLBACK_t, list);
		if (mc_callback_rec != NULL) {
			if (mc_callback_rec->netDev->name != NULL) {
				if (!strncmp (netdev->name, mc_callback_rec->netDev->name, IFNAMSIZ)) {
					if (!strncmp (modName->name, mc_callback_rec->modName->name, MODULE_NAME_LEN)) {
						list_del(&mc_callback_rec->list);
						kfree(mc_callback_rec);
						if ((flags & LTQ_MC_F_FW_RESET) == LTQ_MC_F_FW_RESET)
							mcast_helper_send_netlink_msg_user(netdev, RTM_DELLINK, 0, NULL);
						return SUCCESS;
					}
				}
			}
		}
	}

	return FAILURE;
}

/** Register callback function **/
void  mcast_helper_register_module (
		struct net_device *dev, /*Registered netdev e.g. wlan0 */
		struct module *modName, /* Kernel Module Name */
		char *addlName, /* Optional Additional Name*/
		Mcast_module_callback_t *fnCB, /* Callback Fn */
		void *data, /* Variable input data */
		unsigned int flags) /* Flag - LTQ_F_REGISTER or LTQ_F_DEREGISTER*/
{

	if (dev->name != NULL) {

		if ((flags & LTQ_MC_F_REGISTER) == LTQ_MC_F_REGISTER) {
			mcast_helper_reg_callback(dev, fnCB, modName, &mch_callback_list_g, flags);
		} else if ((flags & LTQ_MC_F_DEREGISTER) == LTQ_MC_F_DEREGISTER) {
			mcast_helper_dereg_callback(dev, fnCB, modName, &mch_callback_list_g, flags);
		} else if((flags & LTQ_MC_F_NEW_STA) == LTQ_MC_F_NEW_STA) {
			mcast_helper_send_netlink_msg_user(dev, RTM_NEWLINK, 1, NULL);
		} else if ((flags & LTQ_MC_F_DISCONN_MAC) == LTQ_MC_F_DISCONN_MAC) {
			if (data == NULL)
				pr_info("MAC is NULL - flag : LTQ_MC_F_DISCONN_MAC\n");
			else
				mcast_helper_send_netlink_msg_user(dev, RTM_DELLINK, 0, data);
		}
	}

}

EXPORT_SYMBOL(mcast_helper_register_module);

int mcast_helper_invoke_callback(unsigned int grpidx,struct net_device *netdev,void *mc_stream,unsigned int flag, unsigned int iface_count)
{
	struct list_head *liter = NULL;
	MCAST_CALLBACK_t *mc_callback_rec = NULL;
	unsigned int passflag = flag;
	list_for_each(liter,&mch_callback_list_g) {
		mc_callback_rec = list_entry(liter, MCAST_CALLBACK_t,list);

		if((mc_callback_rec->netDev->name != NULL) && (netdev->name != NULL)){
			if (strcmp (netdev->name, mc_callback_rec->netDev->name)== 0)
			{
				if(mc_callback_rec->callbackfun != NULL)
				{
					if(((mc_callback_rec->uflag & LTQ_MC_F_DIRECTPATH)!= 0 ) )
					{
						if((mc_callback_rec->uflag & LTQ_MC_F_UPDATE_MAC_ADDR) == 0) {
							if(flag == LTQ_MC_F_UPD)
								passflag = LTQ_MC_F_ADD;
							if((flag !=  LTQ_MC_F_DEL_UPD) )
								mc_callback_rec->callbackfun(grpidx,netdev,mc_stream,passflag);

						}
						else {
							if((flag & LTQ_MC_F_DEL_UPD) == LTQ_MC_F_DEL_UPD )
								passflag = LTQ_MC_F_UPD;
							mc_callback_rec->callbackfun(grpidx,netdev,mc_stream,passflag);

						}
					}
					else if (mc_callback_rec->uflag != LTQ_MC_F_DIRECTPATH) {
						if((flag & LTQ_MC_F_DEL_UPD) == LTQ_MC_F_DEL_UPD )
							passflag = LTQ_MC_F_UPD;
						mc_callback_rec->callbackfun(grpidx,netdev,mc_stream,passflag);
					}
				}
			}
		}
	}
	return SUCCESS;
}

EXPORT_SYMBOL(mcast_helper_invoke_callback);

#endif

