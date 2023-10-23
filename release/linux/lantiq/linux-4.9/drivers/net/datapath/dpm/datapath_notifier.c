// SPDX-License-Identifier: GPL-2.0
/*****************************************************************************
 * Copyright (c) 2020 - 2021, MaxLinear, Inc.
 * Copyright 2018 - 2020 Intel Corporation
 *
 *****************************************************************************/

#include <linux/module.h>
#include <linux/types.h>	/* size_t */
#include <linux/inetdevice.h>
#include <net/datapath_api.h>
#include <net/datapath_proc_api.h>
#include "datapath.h"
#include "datapath_instance.h"
#include "datapath_swdev_api.h"
#include "datapath_swdev.h"

static int dp_event(struct notifier_block *this,
		    unsigned long event, void *ptr);
static struct notifier_block dp_dev_notifier = {
	dp_event, /*handler*/
	NULL,
	0
};

int register_notifier(u32 flag)
{
	return register_netdevice_notifier(&dp_dev_notifier);
}

int unregister_notifier(u32 flag)
{
	return unregister_netdevice_notifier(&dp_dev_notifier);
}

int dp_notifier_invoke(int inst, struct net_device *dev,
		       u32 port_id, u32 subif_id, enum DP_EVENT_TYPE type)
{
	struct pmac_port_info *port_info;
	struct dp_evt_notif_data notif_data = {0};

	port_info = get_dp_port_info(inst, port_id);

	notif_data.inst = inst;
	notif_data.dev = dev;
	notif_data.dev_port = port_info->dev_port;
	notif_data.mod = port_info->owner;
	notif_data.dpid = port_id;

	switch (type) {
	case DP_EVENT_ALLOC_PORT:
		break;
	case DP_EVENT_DE_ALLOC_PORT:
		break;
	case DP_EVENT_REGISTER_DEV:
		break;
	case DP_EVENT_DE_REGISTER_DEV:
		break;
	case DP_EVENT_REGISTER_SUBIF:
	case DP_EVENT_DE_REGISTER_SUBIF:
		notif_data.subif = subif_id;
		break;
	default:
		pr_err("%s Unsupported Event %d\n", __func__, type);
		return DP_FAILURE;
	}

#ifdef DP_SPIN_LOCK
#warning blocking_notifier will not work with DP_SPIN_LOCK, use atomic_notifier
#endif
	/* blocking_notifier_call_chain uses a semaphore to lock the execution,
	 * the atomic_notifier uses a rcu_read_lock. Using the semaphore is
	 * preferred, but only possible when it is not called inside a spinlock.
	 * When DP_SPIN_LOCK is defined this is called from a spinlock.
	 */
	blocking_notifier_call_chain(&dp_evt_notif_list, type, &notif_data);

	return DP_SUCCESS;
}

int dp_event(struct notifier_block *this, unsigned long event, void *ptr)
{
#if IS_ENABLED(CONFIG_INTEL_DATAPATH_SWITCHDEV)
	struct net_device *dev;
	u8 *addr;
	int i;
	struct net_device *br_dev;
	struct dp_dev *dp_dev;
	struct br_info *br_info;
	int fid, inst, vap = 0;
	struct pmac_port_info *port;
	dp_subif_t subif = {0};
	struct inst_property *prop;
	struct dp_subif_info *sif;
	struct inst_info *dp_info;

	dev = netdev_notifier_info_to_dev(ptr);
	if (!dev)
		return 0;
	addr = (u8 *)dev->dev_addr;
	if (!addr || dev->addr_len != ETH_ALEN) /*only support ethernet */
		return 0;
	if (is_zero_ether_addr(addr)) {
		DP_DEBUG(DP_DBG_FLAG_NOTIFY,
			 "Skip zero mac address for %s\n", dev->name);
		return 0;
	}
	DP_LIB_LOCK(&dp_lock);
	dp_dev = dp_dev_lookup(dev);
	if (!dp_dev) {
		DP_DEBUG(DP_DBG_FLAG_NOTIFY,
			 "datapath dev(%s) not exists!!,No mac config\n",
			 dev ? dev->name : "NULL");
		if ((netif_is_bridge_master(dev)) &&
				(event == NETDEV_UNREGISTER)) {
			dp_notif_br_free(dev);
		}

		DP_LIB_UNLOCK(&dp_lock);
		return 0;
	}

	if (!(dp_dev->owner & DP_OPS_OWNER_DPM)) {
		DP_LIB_UNLOCK(&dp_lock);
		return 0;
	}
	inst = dp_dev->inst;
	port = get_dp_port_info(inst, dp_dev->ep);
	if (dp_get_netif_subifid(dev, NULL, NULL, NULL, &subif, 0)) {
		DP_DEBUG(DP_DBG_FLAG_DBG,
			 "get subifid fail(%s), No Mac config\n",
			 dev ? dev->name : "NULL");
		DP_LIB_UNLOCK(&dp_lock);
		return DP_FAILURE;
	}
	vap = GET_VAP(subif.subif, port->vap_offset,
		      port->vap_mask);
	prop = &dp_port_prop[inst];
	sif = get_dp_port_subif(port, vap);
	dp_info = get_dp_prop_info(inst);

	switch (event) {
	case NETDEV_GOING_DOWN:
		DP_DEBUG(DP_DBG_FLAG_NOTIFY,
			 "%s%d %s%d %s%s %s%02x%02x%02x%02x%02x%02x\n",
			 "Rem MAC with BP:",
			 sif->bp, "FID:", dp_dev->fid,
			 "dev:", dev ? dev->name : "NULL",
			 "MAC:", addr[0], addr[1], addr[2],
			 addr[3], addr[4], addr[5]);
		for (i = 0; i < prop->info.cap.max_num_subif_per_port; i++) {
			if (get_dp_port_subif(port, i)->netif == dev) {
				vap = i;
				sif = get_dp_port_subif(port, vap);
				DP_DEBUG(DP_DBG_FLAG_NOTIFY, "vap:%d\n", vap);
				sif->fid = 0;
			}
		}
		dp_info->dp_mac_reset(0, dp_dev->fid, dp_dev->inst, addr);
		/* de-register vlan device from DP if vlan interface removed
		 * when in linux bridge
		 */
		br_dev = netdev_master_upper_dev_get(dev);
		if (br_dev)
			dp_del_br_if(dev, br_dev, dp_dev->inst, dp_dev->bp);
		break;
	case NETDEV_CHANGEUPPER:
		dp_info->dp_mac_reset(0, dp_dev->fid, dp_dev->inst, addr);
		DP_DEBUG(DP_DBG_FLAG_NOTIFY,
			 "%s%d %s%d %s%s %s%02x%02x%02x%02x%02x%02x\n",
			 "Rem MAC with BP:",
			 sif->bp, "FID:", dp_dev->fid,
			 "dev:", dev ? dev->name : "NULL",
			 "MAC:", addr[0], addr[1], addr[2],
			 addr[3], addr[4], addr[5]);
		if (!netif_is_bridge_port(dev)) {
			DP_DEBUG(DP_DBG_FLAG_NOTIFY,
				 "chg upper:dev not a Bport\n");
		}
		br_dev = netdev_master_upper_dev_get(dev);
		if (!br_dev) {
			DP_DEBUG(DP_DBG_FLAG_NOTIFY,
				 "chg upper:without br name\n");
			/* Set FID to Zero when br not attached to
			 * bport
			 */
			dp_dev->fid = 0;
			sif->fid = dp_dev->fid;
			goto dev_status;
		}
		/* Get respective FID when bport attached to bridge
		 */
		DP_DEBUG(DP_DBG_FLAG_NOTIFY, "Bridge name:%s\n",
			 br_dev ? br_dev->name : "NULL");
		br_info = dp_swdev_bridge_entry_lookup_rcu(br_dev);
		if (sif->swdev_en == 1) {
			if (br_info) {
				dp_dev->fid = br_info->fid;
			} else {
				fid = dp_notif_br_alloc(br_dev);
				if (fid > 0) {
					dp_dev->fid = fid;
				} else {
					pr_err("FID alloc failed in %s\r\n",
					       __func__);
					DP_LIB_UNLOCK(&dp_lock);
					return 0;
				}
			}
		} else {
			dp_dev->fid = 0;
		}
		for (i = 0; i < prop->info.cap.max_num_subif_per_port; i++) {
			if (get_dp_port_subif(port, i)->netif == dev) {
				vap = i;
				sif = get_dp_port_subif(port, vap);
				DP_DEBUG(DP_DBG_FLAG_NOTIFY, "vap:%d\n", vap);
				sif->fid = dp_dev->fid;
			}
		}
dev_status:
		if (dev->flags & IFF_UP) {
			DP_DEBUG(DP_DBG_FLAG_NOTIFY,
				 "%s%s%d%s%d %s%s %s%02x%02x%02x%02x%02x%02x\n",
				 "link UP,",
				 "ADD MAC with BP:",
				 sif->bp, " FID:", dp_dev->fid,
				 "dev:", dev ? dev->name : "NULL",
				 "MAC:", addr[0], addr[1], addr[2],
				 addr[3], addr[4], addr[5]);
			/* Note: For Linux network device's mac address,
			 *       its bridge port should be CPU port, not its
			 *       mapped bridge port in GSWIP
			 */
			dp_info->dp_mac_set(0, dp_dev->fid, dp_dev->inst, addr);
		} else {
			DP_DEBUG(DP_DBG_FLAG_NOTIFY, "DEV:%s %s %s\n",
				 dev ? dev->name : "NULL",
				 "link down", "No MAC configuration");
		}
		break;
	case NETDEV_UP:
		DP_DEBUG(DP_DBG_FLAG_NOTIFY,
			 "%s%d %s%d %s%s %s%02x%02x%02x%02x%02x%02x\n",
			 "ADD MAC with BP:",
			 sif->bp, "FID:", dp_dev->fid,
			 "dev:", dev ? dev->name : "NULL",
			 "MAC:", addr[0], addr[1], addr[2],
			 addr[3], addr[4], addr[5]);
		dp_info->dp_mac_set(0, dp_dev->fid, dp_dev->inst, addr);
		break;
	case NETDEV_UNREGISTER:
		DP_DEBUG(DP_DBG_FLAG_NOTIFY,
			 "DevUnreg %s:%02x:%02x:%02x:%02x:%02x:%02x\n",
			 dev->name,
			 addr[0], addr[1], addr[2],
			 addr[3], addr[4], addr[5]);
		break;
	case NETDEV_CHANGEADDR:
		DP_DEBUG(DP_DBG_FLAG_NOTIFY,
			 "DevChg %s:%02x:%02x:%02x:%02x:%02x:%02x\n",
			 dev->name,
			 addr[0], addr[1], addr[2],
			 addr[3], addr[4], addr[5]);
		break;
	default:
		break;
	}
	DP_LIB_UNLOCK(&dp_lock);
#endif
	return 0;
}

BLOCKING_NOTIFIER_HEAD(dp_evt_notif_list);
static int dp_notifier_cb(struct notifier_block *self, unsigned long action,
		       void *data)
{
	struct dp_evt_notif_data *notif_data = data;
	struct dp_evt_notif_info *evt_notif = NULL;
	struct dp_event_info info;

	if (!data) {
		pr_err("invalid notifier data\n");
		return -EINVAL;
	}

	evt_notif = container_of(self, struct dp_evt_notif_info, nb);
	if (!evt_notif->evt_info.dp_event_cb) {
		pr_err("skipping event 0x%lx, callback not registerd: owner[%d] type[0x%x]\n",
		       action, evt_notif->evt_info.owner,
		       evt_notif->evt_info.type);
		goto end;
	}

	if (evt_notif->evt_info.inst != notif_data->inst)
		goto end;

	switch (evt_notif->evt_info.type & action) {
	case DP_EVENT_INIT:
		info.init_info.dev = notif_data->dev;
		info.init_info.owner = notif_data->mod;
		info.init_info.dev_port = notif_data->dev_port;
		break;
	case DP_EVENT_ALLOC_PORT:
		info.alloc_info.dev = notif_data->dev;
		info.alloc_info.owner = notif_data->mod;
		info.alloc_info.dev_port = notif_data->dev_port;
		break;
	case DP_EVENT_DE_ALLOC_PORT:
		info.de_alloc_info.dev = notif_data->dev;
		info.de_alloc_info.owner = notif_data->mod;
		info.de_alloc_info.dev_port = notif_data->dev_port;
		break;
	case DP_EVENT_REGISTER_DEV:
		info.reg_dev_info.dev = notif_data->dev;
		info.reg_dev_info.dpid = notif_data->dpid;
		info.reg_dev_info.owner = notif_data->mod;
		break;
	case DP_EVENT_DE_REGISTER_DEV:
		info.dereg_dev_info.dev = notif_data->dev;
		info.dereg_dev_info.dpid = notif_data->dpid;
		info.dereg_dev_info.owner = notif_data->mod;
		break;
	case DP_EVENT_REGISTER_SUBIF:
		info.reg_subif_info.dev = notif_data->dev;
		info.reg_subif_info.dpid = notif_data->dpid;
		info.reg_subif_info.subif = notif_data->subif;
		break;
	case DP_EVENT_DE_REGISTER_SUBIF:
		info.de_reg_subif_info.dev = notif_data->dev;
		info.de_reg_subif_info.dpid = notif_data->dpid;
		info.de_reg_subif_info.subif = notif_data->subif;
		break;
	case DP_EVENT_OWNER_SWITCH:
		info.owner_info.dpid = notif_data->dpid;
		info.owner_info.new_owner = notif_data->type;
		break;
	default:
		goto end;
	}

	info.inst = notif_data->inst;
	info.type = action;
	info.data = evt_notif->evt_info.data;
	evt_notif->evt_info.dp_event_cb(&info);
end:
	return NOTIFY_DONE;
}

int register_dp_event_notifier(struct dp_event *info)
{
	struct dp_evt_notif_info *evt_notif;
	struct notifier_block *nb = NULL;
	int ret = 0;

	if (!info) {
		pr_err("%s: invalid info\n", __func__);
		return DP_FAILURE;
	}

	if (!info->id) {
		evt_notif = devm_kzalloc(&g_dp_dev->dev, sizeof(*evt_notif),
					 GFP_ATOMIC);
		if (!evt_notif)
			return DP_FAILURE;

		evt_notif->nb.notifier_call = dp_notifier_cb;
		info->id = &evt_notif->nb;
		nb = &evt_notif->nb;
	} else {
		/* to avoid race between dp_notifier_cb and update_dp_evt_notif_info,
		 * remove nb from list, modify evt_info and re-insert the nb
		 */
		nb = info->id;
		ret = blocking_notifier_chain_unregister(&dp_evt_notif_list,
							 nb);
		if (ret) {
			pr_err("%s: invalid handle\n", __func__);
			return DP_FAILURE;
		}

		evt_notif = container_of(nb, struct dp_evt_notif_info, nb);
	}

	memcpy(&evt_notif->evt_info, info, sizeof(*info));
	blocking_notifier_chain_register(&dp_evt_notif_list, nb);
	return 0;
}

int unregister_dp_event_notifier(struct dp_event *info)
{
	struct dp_evt_notif_info *evt_notif;
	int ret = 0;

	if (!info || !info->id) {
		pr_err("%s:%d: invalid handle\n", __func__, __LINE__);
		return DP_FAILURE;
	}

	ret = blocking_notifier_chain_unregister(&dp_evt_notif_list, info->id);
	if (ret) {
		pr_err("%s:%d: invalid handle\n", __func__, __LINE__);
		return DP_FAILURE;
	}

	evt_notif = container_of(info->id, struct dp_evt_notif_info, nb);
	devm_kfree(&g_dp_dev->dev, evt_notif);
	info->id = NULL;
	return 0;
}

/* NOTE: keep in sync with enum DP_EVENT_OWNER */
const char *dp_event_owner_str[] = {
	[DP_EVENT_OWNER_OTHERS] = "OTHERS",
	[DP_EVENT_OWNER_DPDK] = "DPDK",
	[DP_EVENT_OWNER_PPA] = "PPA",
	[DP_EVENT_OWNER_MIB] = "MIB",
};

/* NOTE: keep in sync with enum DP_EVENT_TYPE_BITS */
const char *dp_event_type_str[] = {
	[DP_EVENT_INIT_BIT] = "INIT",
	[DP_EVENT_ALLOC_PORT_BIT] = "ALLOC_PORT",
	[DP_EVENT_DE_ALLOC_PORT_BIT] = "DE_ALLOC_PORT",
	[DP_EVENT_REGISTER_DEV_BIT] = "REGISTER_DEV",
	[DP_EVENT_DE_REGISTER_DEV_BIT] = "DE_REGISTER_DEV",
	[DP_EVENT_REGISTER_SUBIF_BIT] = "REGISTER_SUBIF",
	[DP_EVENT_DE_REGISTER_SUBIF_BIT] = "DEREGISTER_SUBIF",
	[DP_EVENT_OWNER_SWITCH_BIT] = "OWNER_SWITCH"
};

int proc_dp_event_list_dump(struct seq_file *s, int pos)
{
	struct dp_evt_notif_info *evt_notif;
	struct notifier_block *nb, *next_nb;
	int i;

	if (!capable(CAP_SYS_PACCT))
		return -1;
	if (!dp_port_prop[pos].valid)
		goto NEXT;

	seq_printf(s, "Inst[%d]\n", pos);
	rcu_read_lock();
	nb = rcu_dereference_raw(dp_evt_notif_list.head);
	while (nb) {
		next_nb = rcu_dereference_raw(nb->next);
		evt_notif = container_of(nb, struct dp_evt_notif_info, nb);
		if (evt_notif->evt_info.inst != pos) {
			nb = next_nb;
			continue;
		}
		if (evt_notif->evt_info.owner < DP_EVENT_OWNER_MAX)
			seq_printf(s, " owner: %s\n",
					dp_event_owner_str[evt_notif->evt_info.owner]);

		seq_puts(s, " type:\n");
		for (i = 0; i < DP_EVENT_MAX_BIT; i++)
			if (evt_notif->evt_info.type & BIT(i))
				seq_printf(s, "  %s\n", dp_event_type_str[i]);

		seq_printf(s, " id: 0x%px\n", evt_notif->evt_info.id);
		nb = next_nb;
	}
	rcu_read_unlock();
NEXT:
	pos++;
	if (pos == DP_MAX_INST)
		return -1;
	return pos;
}
