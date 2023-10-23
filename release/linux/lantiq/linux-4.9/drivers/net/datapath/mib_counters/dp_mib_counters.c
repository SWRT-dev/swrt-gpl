// SPDX-License-Identifier: GPL-2.0
/* Copyright (C) 2020 Intel Corporation.
 */

#include <linux/module.h> /* Needed by all modules */
#include <linux/kernel.h> /* Needed for KERN_INFO */
#include <linux/init.h> /* Needed for the macros */
#include <linux/workqueue.h>
#include <linux/spinlock.h>
#include <linux/list.h>
#include <linux/string.h>

#include <net/datapath_api.h>
#include "dp_mib_counters.h"

/* Counters update interval = 3[min] -> 180'000[ms] */
#define CNT_UPDATE_TIME 180000U

/* Maximal number of dp ports and sub_ifs:
 * for port: FLM-12, LGM-16
 * for subif: SFU 256 and HGU 128
 */
#define MAX_PORT 12
#define MAX_SUBIF 256
#define DP_MAX_BP_NUM 128

#define INITIAL_UPDATE 1


/* Ethtool strings and counter names (based on GSW_RMON_Port_cnt_t) */
#define DP_GSW_ETHTOOL_STAT_REG_DECLARE	\
	_DP_ETHTOOL(RxExtendedVlanDiscardPkts, nRxExtendedVlanDiscardPkts) \
	_DP_ETHTOOL(MtuExceedDiscardPkts, nMtuExceedDiscardPkts) \
	_DP_ETHTOOL(TxUnderSizeGoodPkts, nTxUnderSizeGoodPkts) \
	_DP_ETHTOOL(TxOversizeGoodPkts, nTxOversizeGoodPkts) \
	_DP_ETHTOOL(RxGoodPkts, nRxGoodPkts) \
	_DP_ETHTOOL(RxUnicastPkts, nRxUnicastPkts) \
	_DP_ETHTOOL(RxBroadcastPkts, nRxBroadcastPkts) \
	_DP_ETHTOOL(RxMulticastPkts, nRxMulticastPkts) \
	_DP_ETHTOOL(RxFCSErrorPkts, nRxFCSErrorPkts) \
	_DP_ETHTOOL(RxUnderSizeGoodPkts, nRxUnderSizeGoodPkts) \
	_DP_ETHTOOL(RxOversizeGoodPkts, nRxOversizeGoodPkts) \
	_DP_ETHTOOL(RxUnderSizeErrorPkts, nRxUnderSizeErrorPkts) \
	_DP_ETHTOOL(RxGoodPausePkts, nRxGoodPausePkts) \
	_DP_ETHTOOL(RxOversizeErrorPkts, nRxOversizeErrorPkts) \
	_DP_ETHTOOL(RxAlignErrorPkts, nRxAlignErrorPkts) \
	_DP_ETHTOOL(RxFilteredPkts, nRxFilteredPkts) \
	_DP_ETHTOOL(Rx64BytePkts, nRx64BytePkts) \
	_DP_ETHTOOL(Rx127BytePkts, nRx127BytePkts) \
	_DP_ETHTOOL(Rx255BytePkts, nRx255BytePkts) \
	_DP_ETHTOOL(Rx511BytePkts, nRx511BytePkts) \
	_DP_ETHTOOL(Rx1023BytePkts, nRx1023BytePkts) \
	_DP_ETHTOOL(RxMaxBytePkts, nRxMaxBytePkts) \
	_DP_ETHTOOL(TxGoodPkts, nTxGoodPkts) \
	_DP_ETHTOOL(TxUnicastPkts, nTxUnicastPkts) \
	_DP_ETHTOOL(TxBroadcastPkts, nTxBroadcastPkts) \
	_DP_ETHTOOL(TxMulticastPkts, nTxMulticastPkts) \
	_DP_ETHTOOL(TxSingleCollCount, nTxSingleCollCount) \
	_DP_ETHTOOL(TxMultCollCount, nTxMultCollCount) \
	_DP_ETHTOOL(TxLateCollCount, nTxLateCollCount) \
	_DP_ETHTOOL(TxExcessCollCount, nTxExcessCollCount) \
	_DP_ETHTOOL(TxCollCount, nTxCollCount) \
	_DP_ETHTOOL(TxPauseCount, nTxPauseCount) \
	_DP_ETHTOOL(Tx64BytePkts, nTx64BytePkts) \
	_DP_ETHTOOL(Tx127BytePkts, nTx127BytePkts) \
	_DP_ETHTOOL(Tx255BytePkts, nTx255BytePkts) \
	_DP_ETHTOOL(Tx511BytePkts, nTx511BytePkts) \
	_DP_ETHTOOL(Tx1023BytePkts, nTx1023BytePkts) \
	_DP_ETHTOOL(TxMaxBytePkts, nTxMaxBytePkts) \
	_DP_ETHTOOL(TxDroppedPkts, nTxDroppedPkts) \
	_DP_ETHTOOL(TxAcmDroppedPkts, nTxAcmDroppedPkts) \
	_DP_ETHTOOL(RxDroppedPkts, nRxDroppedPkts) \
	_DP_ETHTOOL(RxGoodBytes, nRxGoodBytes) \
	_DP_ETHTOOL(RxBadBytes, nRxBadBytes) \
	_DP_ETHTOOL(TxGoodBytes, nTxGoodBytes)

/* Ethtool strings and counter names for all counters */
#define DP_ETHTOOL_STAT_REG_DECLARE \
	DP_GSW_ETHTOOL_STAT_REG_DECLARE \
	_DP_ETHTOOL(RxOverflowError, nRxOverflowError)

/* Tx counters serviced by CTP bypass registers */
#define DP_ETHTOOL_STAT_REG_TX_DECLARE \
	_DP_ETHTOOL_TX(nTxUnderSizeGoodPkts) \
	_DP_ETHTOOL_TX(nTxOversizeGoodPkts) \
	_DP_ETHTOOL_TX(nTxGoodPkts) \
	_DP_ETHTOOL_TX(nTxUnicastPkts) \
	_DP_ETHTOOL_TX(nTxBroadcastPkts) \
	_DP_ETHTOOL_TX(nTxMulticastPkts) \
	_DP_ETHTOOL_TX(nTx64BytePkts) \
	_DP_ETHTOOL_TX(nTx127BytePkts) \
	_DP_ETHTOOL_TX(nTx255BytePkts) \
	_DP_ETHTOOL_TX(nTx511BytePkts) \
	_DP_ETHTOOL_TX(nTx1023BytePkts) \
	_DP_ETHTOOL_TX(nTxMaxBytePkts) \
	_DP_ETHTOOL_TX(nTxDroppedPkts) \
	_DP_ETHTOOL_TX(nTxAcmDroppedPkts) \
	_DP_ETHTOOL_TX(nTxGoodBytes)

struct mib_counters_u64 {
#define _DP_ETHTOOL(x, y)	u64 y;
DP_ETHTOOL_STAT_REG_DECLARE
#undef _DP_ETHTOOL
	u64 tx_good_pkts_ctp;
	u64 tx_good_pkts_ctp_bypass;
	u64 tx_cbm_pkts;
};

struct last_counters {
	GSW_RMON_Port_cnt_t rmon;
	u64 rx_ovfl_err;
	u32 tx_good_pkts_ctp;
	u32 tx_good_pkts_ctp_bypass;
	u32 tx_cbm_pkts;
};

/** Data set needed for maintain mib counters overflow for one net device */
struct mib_counters {
	/** devices list handle */
	struct list_head dev_list;
	/** lock for update the set */
	rwlock_t cnt_lock;
	/** net device handle */
	struct net_device *dev;
	/** inst */
	s32 inst;
	/** device id */
	s32 port_id;
	/** sub interface id group*/
	s32 sub_if_id_grp;
	/** flag indicates dev is UNI side bridge port */
	u8 dev_is_uni;
	/** last read counters */
	struct last_counters last;
	/** stored 64 bits wide counters */
	struct mib_counters_u64 wide_cntrs;
	/** bridge port id */
	u16 br_port_id;
	/** indicates if related device is a pmapper */
	bool is_pmapper;
};

struct work_list_el {
	/** work list element handle */
	struct list_head work_list_handle;
	/** device name */
	char name[IFNAMSIZ];
	/** device pointer */
	struct net_device *dev;
};

/* Ethtool statistics strings */
static const char dp_ethtool_gstrings[][ETH_GSTRING_LEN] = {
#define _DP_ETHTOOL(x, y)	# x,
	DP_ETHTOOL_STAT_REG_DECLARE
#undef _DP_ETHTOOL
};

static struct core_ops *gsw_ops;

/* Delayed work object for update counters for all registered net devices */
static struct delayed_work cnt_upd_work;

/* List of registered net devices */
static struct list_head registered_devices_list;

/* Devices list lock */
static rwlock_t dev_list_lock;

/* Notify callback registration ID (pointer) - used for deregister CB */
static void *g_notify_cb_id;

/* GSWIP "shortcut" mode flag - based on device tree */
static u8 shortcut_mode;

/* Wraparound counter handle */
static u64 wraparound(u64 curr, u64 last, u32 size)
{
	#define WRAPAROUND_MAX_32 0x100000000
	/* for 8 bytes(64bit mib),no need to do wraparound */
	if ((size > 4) || (curr >= last))
		return curr - last;
	return ((u64)WRAPAROUND_MAX_32) + curr - last;
}

static s16 get_bridge_port_id(struct net_device *dev)
{
	dp_subif_t subif = {0};
	int ret;

	ret = dp_get_netif_subifid(dev, NULL, NULL, NULL, &subif, 0);
	if (ret != DP_SUCCESS) {
		netdev_err(dev, "can not get subif\n");
		return -1;
	}

	if (subif.bport >= DP_MAX_BP_NUM) {
		pr_err("BP port(%d) out of range %d\n",
			subif.bport, DP_MAX_BP_NUM);
		return -1;
	}

	return subif.bport;
}


/* Update counters for one device */
static void update_dev_counters(struct mib_counters *cnt, u8 initial_update)
{
	GSW_RMON_Port_cnt_t *tmp_counters;
	struct mac_ops *mac_ops;
	struct mac_rmon *rmon = NULL;
	u32 tx_good_pkts_ctp = 0;
	u32 tx_good_pkts_ctp_bypass = 0;
	u32 tx_cbm_pkts = 0;
	s64 qos_drop_pkts = 0;
	int ret = 0;
	u8 i = 0;
	u16 port_id = 0;
	GSW_portType_t port_type = GSW_CTP_PORT;

	if (!cnt)
		return;

	tmp_counters = kzalloc(2 * sizeof(GSW_RMON_Port_cnt_t), GFP_ATOMIC);
	if (!tmp_counters)
		return;

	pr_debug("MIB CNT - update counters for dev: %s, port_id: %d, sub_if_id_grp: %d\n",
		 cnt->dev->name, cnt->port_id, cnt->sub_if_id_grp);

	if (cnt->is_pmapper) {
		port_type = GSW_BRIDGE_PORT;
		port_id = cnt->br_port_id;
		pr_debug("MIB CNT - use BP: %d\n", port_id);
	} else {
		port_id = cnt->port_id;
	}

	/* read current counters' values */
	for (i = 0; i < 2; i++) {
		tmp_counters[i].ePortType = port_type;
		tmp_counters[i].nPortId = port_id;
		tmp_counters[i].nSubIfIdGroup = cnt->sub_if_id_grp;
		tmp_counters[i].bPceBypass = i;

		/* read CTP counters */
		ret = gsw_ops->gsw_rmon_ops.RMON_Port_Get(gsw_ops, &tmp_counters[i]);
		if (ret != DP_SUCCESS) {
			pr_err("MIB CNT ERROR - RMON_Port_Get - %d\n", ret);
			kfree(tmp_counters);
			return;
		}
	}
	/* After this loop tmp_counters[0] contains all CTP counters, RX and TX
	 * where RX counters are valid but TX can be not valid.
	 * In tmp_counters[1] there are CTP Bypass TX counters, all RX counters
	 * here are invalid.
	 * CTP Tx counters are placed before QoS modules, and they
	 * record packets dropped by QoS modules too.
	 * CTP Tx PCE-Bypass counters are placed after QoS modules,
	 * and they record the packets actually transmitted.
	 */

	if (cnt->dev_is_uni && shortcut_mode) {
		/* In "shortcut" mode CTP counters don't contain information
		 * about packets sent from CPU. Required information is in
		 * CTP PCE-Bypass and have to be added to CTP counters
		 */
		#define _DP_ETHTOOL_TX(r) tmp_counters[0].r += tmp_counters[1].r;
		DP_ETHTOOL_STAT_REG_TX_DECLARE
		#undef _DP_ETHTOOL_TX
	} else {
		/* This check excludes overwriting CTP Counters by CTP Bypass
		 * Counters for LAN ports if Shortcut mode is enabled.
		 * For GEMs dev_is_uni is false so the drop counter will be
		 * calculated for all gems. For LAN the code checks "shortcut"
		 * mode is enabled - if not, drop counter will be calculated.
		 */
		/* Get counters required to calculate Tx drops on QoS */
		tx_good_pkts_ctp	= tmp_counters[0].nTxGoodPkts;
		tx_good_pkts_ctp_bypass = tmp_counters[1].nTxGoodPkts;
		tx_cbm_pkts = dp_get_tx_cbm_pkt(cnt->inst, cnt->port_id,
						cnt->sub_if_id_grp);
		/* Overwrite CTP Tx counters by CTP Tx Pce-Bypass counters,
		 * but do not overwrite CTP Tx counters for an UNI interface
		 * in "shortcut" mode.
		 */
		#define _DP_ETHTOOL_TX(r) tmp_counters[0].r = tmp_counters[1].r;
		DP_ETHTOOL_STAT_REG_TX_DECLARE
		#undef _DP_ETHTOOL_TX
	}

	/* mac_ops is optional, do not fail if unavailable */
	mac_ops = gsw_get_mac_ops(cnt->inst, cnt->port_id);
	if (mac_ops) {
		rmon = kzalloc(sizeof(*rmon), GFP_ATOMIC);
		if (rmon) {
			mac_ops->rmon_get(mac_ops, rmon);
			tmp_counters[0].nRxGoodPausePkts =
				rmon->cnt[RMON_RX_PAUSE];
			tmp_counters[0].nTxPauseCount =
				rmon->cnt[RMON_TX_PAUSE];
			tmp_counters[0].nTxExcessCollCount =
				rmon->cnt[RMON_TX_EXCS_COL];
			tmp_counters[0].nTxLateCollCount =
				rmon->cnt[RMON_TX_LATE_COL];
			tmp_counters[0].nTxSingleCollCount =
				rmon->cnt[RMON_TX_SINGLE_COL];
			tmp_counters[0].nTxMultCollCount =
				rmon->cnt[RMON_TX_MULTI_COL];
		}
	}

	/* On initial update, just after new device is added to update
	 * counter list, counters values read first time can be invalid.
	 * Therefore the value just read will not be added to the 64 bits
	 * cumulative counters
	 */
	if (initial_update == INITIAL_UPDATE) {
		write_lock(&cnt->cnt_lock);
		cnt->last.rmon = tmp_counters[0];
		if (rmon)
			cnt->last.rx_ovfl_err =	rmon->cnt[RMON_RX_OVERFLOW];
		write_unlock(&cnt->cnt_lock);
		goto free_exit;
	}

	/* update 64 bits wide counters based on last read values */
	write_lock(&cnt->cnt_lock);
#define _DP_ETHTOOL(x, y) cnt->wide_cntrs.y += wraparound(tmp_counters[0].y, cnt->last.rmon.y, sizeof(tmp_counters[0].y));
	DP_GSW_ETHTOOL_STAT_REG_DECLARE
#undef _DP_ETHTOOL

	cnt->wide_cntrs.tx_good_pkts_ctp +=
		wraparound(tx_good_pkts_ctp,
			   cnt->last.tx_good_pkts_ctp,
			   sizeof(tx_good_pkts_ctp));
	cnt->last.tx_good_pkts_ctp = tx_good_pkts_ctp;

	cnt->wide_cntrs.tx_good_pkts_ctp_bypass +=
		wraparound(tx_good_pkts_ctp_bypass,
			   cnt->last.tx_good_pkts_ctp_bypass,
			   sizeof(tx_good_pkts_ctp_bypass));
	cnt->last.tx_good_pkts_ctp_bypass = tx_good_pkts_ctp_bypass;

	cnt->wide_cntrs.tx_cbm_pkts +=
		wraparound(tx_cbm_pkts,
			   cnt->last.tx_cbm_pkts,
			   sizeof(tx_cbm_pkts));
	cnt->last.tx_cbm_pkts = tx_cbm_pkts;

	qos_drop_pkts = cnt->wide_cntrs.tx_good_pkts_ctp -
			(cnt->wide_cntrs.tx_good_pkts_ctp_bypass -
			 cnt->wide_cntrs.tx_cbm_pkts);
	if (qos_drop_pkts < 0)
		qos_drop_pkts = 0;
	cnt->wide_cntrs.nTxDroppedPkts = qos_drop_pkts;

	if (rmon)
		cnt->wide_cntrs.nRxOverflowError +=
			wraparound(rmon->cnt[RMON_RX_OVERFLOW],
				   cnt->last.rx_ovfl_err,
				   sizeof(rmon->cnt[RMON_RX_OVERFLOW]));

	/* Update last read counters set */
	cnt->last.rmon = tmp_counters[0];
	if (rmon)
		cnt->last.rx_ovfl_err =	rmon->cnt[RMON_RX_OVERFLOW];
	write_unlock(&cnt->cnt_lock);

free_exit:
	kfree(tmp_counters);
	kfree(rmon);
}

/* check if is it the one that is search for */
static bool check_match(struct work_list_el *wle, struct mib_counters *mib_cnt)
{
	if (!wle || !mib_cnt)
		return false;

	if ((wle->dev == mib_cnt->dev) &&
	    !strcmp(wle->name, mib_cnt->dev->name))
		return true;
	else
		return false;
}

/* update counters for all registered devices */
static void update_all_devs_counters(void)
{
	struct mib_counters *mib_cnt, *mib_cnt_match;
	struct list_head *p, *q, *r;
	struct work_list_el *wle;
	/* local list of registered net devices */
	struct list_head work_list;

	INIT_LIST_HEAD(&work_list);

	/* Fill work_list as a snapshot of the registered_devices_list
	 * with crucial identifiers only
	 */
	read_lock(&dev_list_lock);
	list_for_each(p, &registered_devices_list) {
		mib_cnt = list_entry(p, struct mib_counters, dev_list);
		wle = kzalloc(sizeof(struct work_list_el), GFP_ATOMIC);
		if (!wle) {
			read_unlock(&dev_list_lock);
			return;
		}
		wle->dev = mib_cnt->dev;
		memcpy(wle->name, mib_cnt->dev->name,
		       sizeof(mib_cnt->dev->name));

		list_add_tail(&wle->work_list_handle, &work_list);
	}
	read_unlock(&dev_list_lock);

	/* Iterate over the work_list, check if the device from work_list still
	 * exists in registered_devices_list and do counters updated for it.
	 */
	list_for_each_safe(p, q, &work_list) {
		wle = list_entry(p, struct work_list_el, work_list_handle);
		mib_cnt_match = NULL;
		read_lock(&dev_list_lock);
		/* The dev can not exist anymore in registered_devices_list
		 * because it could have been removed by dp in meantime.
		 */
		list_for_each(r, &registered_devices_list) {
			mib_cnt = list_entry(r, struct mib_counters, dev_list);
			if (check_match(wle, mib_cnt)) {
				mib_cnt_match = mib_cnt;
				/* Do update counters for the device */
				update_dev_counters(mib_cnt_match,
						    !INITIAL_UPDATE);
				break;
			}
		}
		read_unlock(&dev_list_lock);

		if (mib_cnt_match)
			pr_debug("MIB CNT - %s - counters updated\n",
				 wle->name);
		else
			pr_debug("MIB CNT - %s not found\n", wle->name);

		/* Remove processed work_list entry */
		list_del(p);
		kfree(wle);
	}
}

/* Adds new net device to counter update list */
static int add_dev_to_list(struct net_device *dev,
			   s32 port_id, s32 sub_if, s32 inst)
{
	struct mib_counters *dev_cnt;
	struct dp_port_prop port_prop;
	s32 sub_if_id_grp;
	s16 bridge_port_id;

	if ((!dev) || (port_id < 0) || (port_id > MAX_PORT)) {
		pr_err("MIB CNT ERROR - WRONG data passed to %s: port_id %d, dev <%p>, dev->name %s\n",
		       __func__, port_id, dev, dev ? dev->name : "[NULL]");
		return -EINVAL;
	}
	if (dp_get_port_prop(inst, port_id, &port_prop)) {
		pr_err("dp_get_port_prop fail\n");
		return -EINVAL; 
	}
	sub_if_id_grp = GET_VAP(sub_if, port_prop.vap_offset,
				port_prop.vap_mask);

	if ((sub_if_id_grp < 0) || (sub_if_id_grp > MAX_SUBIF)) {
		pr_err("MIB CNT ERROR - WRONG data passed to %s: port_id %d, sub_if %d, dev %s - sub_if_id_grp %d !\n",
		       __func__, port_id, sub_if, dev->name, sub_if_id_grp);
		return -EINVAL;
	}

	bridge_port_id = get_bridge_port_id(dev);

	if (bridge_port_id < 0) {
		pr_err("MIB CNT ERROR - WRONG data passed to %s: port_id %d, sub_if %d, dev %s - bridge_port_id %d !\n",
		       __func__, port_id, sub_if, dev->name, bridge_port_id);
		return -EINVAL;
	}

	dev_cnt = kzalloc(sizeof(struct mib_counters), GFP_ATOMIC);
	if (!dev_cnt)
		return -ENOMEM;

	dev_cnt->inst = inst;
	dev_cnt->dev = dev;
	dev_cnt->port_id = port_id;
	dev_cnt->sub_if_id_grp = sub_if_id_grp;
	dev_cnt->br_port_id = bridge_port_id;
	dev_cnt->is_pmapper = dp_is_pmapper_check(dev);
	/* check if given net dev is on UNI side of bridge */
	dev_cnt->dev_is_uni = (port_prop.alloc_flags & DP_F_FAST_ETH_LAN) ||
		(port_prop.alloc_flags & DP_F_FAST_ETH_WAN);

	rwlock_init(&dev_cnt->cnt_lock);

	INIT_LIST_HEAD(&dev_cnt->dev_list);

	write_lock(&dev_list_lock);
	list_add_tail(&dev_cnt->dev_list, &registered_devices_list);
	/* read counters for just added dev to save initial value */
	update_dev_counters(dev_cnt, INITIAL_UPDATE);
	write_unlock(&dev_list_lock);

	pr_debug("MIB CNT - added device: %s port_id = %d, sub_if_id_grp = %d\n",
		 dev->name, port_id, dev_cnt->sub_if_id_grp);
	return 0;
}

/* Looks for matched list item and remove it */
static int rm_dev_from_list(struct net_device *dev,
			    s32 port_id, s32 sub_if, s32 inst)
{
	struct dp_port_prop port_prop;
	struct mib_counters *mib_cnt;
	struct list_head *p, *q;
	s32 sub_if_id_grp;
	u32 removed_num = 0;
	bool is_pmapper = false;

	if ((!dev) || (port_id < 0) || (port_id > MAX_PORT)) {
		pr_err("MIB CNT ERROR - WRONG data passed to %s: port_id %d, dev <%p>, dev->name %s\n",
		       __func__, port_id, dev, dev ? dev->name : "[NULL]");
		return -EINVAL;
	}

	if (dp_get_port_prop(inst, port_id, &port_prop)) {
		pr_err("dp_get_port_prop fail\n");
		return -EINVAL; 
	}
	sub_if_id_grp = GET_VAP(sub_if, port_prop.vap_offset,
				port_prop.vap_mask);

	if ((sub_if_id_grp < 0) || (sub_if_id_grp > MAX_SUBIF)) {
		pr_err("MIB CNT ERROR - WRONG data passed to %s: port_id %d, sub_if %d, dev %s - sub_if_id_grp %d !\n",
		       __func__, port_id, sub_if, dev->name, sub_if_id_grp);
		return -EINVAL;
	}

	pr_debug("MIB CNT - looking for item to delete: name = %s, port_id = %d, sub_if = %d, inst = %d\n",
		 dev->name, port_id, sub_if, inst);

	is_pmapper = dp_is_pmapper_check(dev);

	write_lock(&dev_list_lock);
	list_for_each_safe(p, q, &registered_devices_list) {
		mib_cnt = list_entry(p, struct mib_counters, dev_list);

		if ((mib_cnt->port_id == port_id) &&
		    (mib_cnt->sub_if_id_grp == sub_if_id_grp ||
		     (is_pmapper &&
		      (mib_cnt->br_port_id == get_bridge_port_id(dev)))) &&
		    (mib_cnt->inst == inst) &&
		    (mib_cnt->dev == dev)) {
			pr_debug("MIB CNT - list item: name = %s, port_id = %d, sub_if_id_grp = %d, inst = %d - removed\n",
				 mib_cnt->dev->name,
				 mib_cnt->port_id,
				 mib_cnt->sub_if_id_grp,
				 mib_cnt->inst);
			list_del(p);
			kfree(mib_cnt);
			removed_num++;
		} else {
			pr_debug("MIB CNT - list item: name = %s, port_id = %d, sub_if_id_grp = %d, inst = %d - doesn't match\n",
				 mib_cnt->dev->name,
				 mib_cnt->port_id,
				 mib_cnt->sub_if_id_grp,
				 mib_cnt->inst);
		}
	}
	write_unlock(&dev_list_lock);

	if (removed_num != 1) {
		pr_err("MIB CNT - item found %d times: name = %s, port_id = %d, sub_if = %d, inst = %d\n",
			removed_num, dev->name, port_id, sub_if, inst);
	}

	return 0;
}

/* Function needed for ethtool to find on list appropriate wide counters set */
static struct mib_counters *find_registered_dev(s32 port_id, s32 sub_if_id_grp,
						s32 inst,
						struct net_device *dev)
{
	struct mib_counters *mib_cnt;
	struct list_head *p;
	bool is_pmapper;

	is_pmapper = dp_is_pmapper_check(dev);

	read_lock(&dev_list_lock);
	list_for_each(p, &registered_devices_list)
	{
		mib_cnt = list_entry(p, struct mib_counters, dev_list);
		if ((mib_cnt->port_id == port_id) &&
		    (mib_cnt->sub_if_id_grp == sub_if_id_grp ||
		     (is_pmapper &&
		      (mib_cnt->br_port_id == get_bridge_port_id(dev)))) &&
		    (mib_cnt->inst == inst) &&
		    (mib_cnt->dev == dev)) {
			read_unlock(&dev_list_lock);
			return mib_cnt;
		}
	}
	read_unlock(&dev_list_lock);

	return NULL;
}

/* Callback function to register new net device (subif) reported by DP API */
static s32 subif_register_cb(struct dp_event_info *info)
{
	pr_debug("MIB CNT - EVENT - SUBIF REGISTER CB called\n - event info type = %d\n",
		 info->type);

	switch (info->type) {
	case DP_EVENT_REGISTER_SUBIF:
		pr_debug("MIB CNT - event type: DP_EVENT_REGISTER_SUBIF\n");
		pr_debug("MIB CNT - info->reg_subif_info.dpid = %d\n",
			 info->reg_subif_info.dpid);
		pr_debug("MIB CNT - info->reg_subif_info.subif = %d\n",
			 info->reg_subif_info.subif);
		pr_debug("MIB CNT - info->reg_subif_info.dev->name = %s\n",
			 info->reg_subif_info.dev->name);

		/* add new net device to registered devices list */
		add_dev_to_list(info->reg_subif_info.dev,
				info->reg_subif_info.dpid,
				info->reg_subif_info.subif,
				info->inst);
		break;
	case DP_EVENT_DE_REGISTER_SUBIF:

		pr_debug("MIB CNT - event type: DP_EVENT_DE_REGISTER_SUBIF\n");
		pr_debug("MIB CNT - info->de_reg_subif_info.dpid = %d\n",
			 info->de_reg_subif_info.dpid);
		pr_debug("MIB CNT - info->de_reg_subif_info.subif = %d\n",
			 info->de_reg_subif_info.subif);
		pr_debug("MIB CNT - info->de_reg_subif_info.dev->name = %s\n",
			 info->de_reg_subif_info.dev->name);

		/* remove net device from registered devices list */
		rm_dev_from_list(info->de_reg_subif_info.dev,
				 info->de_reg_subif_info.dpid,
				 info->de_reg_subif_info.subif,
				 info->inst);
		break;
	default:
		return 0;
	}

	return 0;
}

static int update_lct_cnt(struct mib_counters_u64 *counters,
			  const struct dp_lct_rx_cnt *lct_rx_cnt,
			  bool add)
{
	int modifier = add ? 1 : -1;

	counters->nRx64BytePkts += (modifier * lct_rx_cnt->dp_64BytePkts);
	counters->nRx127BytePkts += (modifier * lct_rx_cnt->dp_127BytePkts);
	counters->nRx255BytePkts += (modifier * lct_rx_cnt->dp_255BytePkts);
	counters->nRx511BytePkts += (modifier * lct_rx_cnt->dp_511BytePkts);
	counters->nRx1023BytePkts += (modifier * lct_rx_cnt->dp_1023BytePkts);
	counters->nRxMaxBytePkts += (modifier * lct_rx_cnt->dp_MaxBytePkts);
	counters->nRxUnicastPkts += (modifier * lct_rx_cnt->dp_UnicastPkts);
	counters->nRxGoodPkts += (modifier * lct_rx_cnt->dp_GoodPkts);
	counters->nRxGoodBytes += (modifier * lct_rx_cnt->dp_GoodBytes);
	counters->nRxBroadcastPkts += (modifier * lct_rx_cnt->dp_BroadcastPkts);
	counters->nRxMulticastPkts += (modifier * lct_rx_cnt->dp_MulticastPkts);

	return 0;
}

/************************* ETHTOOL SUPPORT ************************/

static void mib_cnt_get_ethtool_stats(struct net_device *dev,
				      struct ethtool_stats *stats, u64 *data)
{
	struct dp_port_prop port_prop;
	struct mib_counters *mib_cnt;
	struct mib_counters_u64 wide_cnt_mod;
	struct dp_reinsert_count dp_reins_count;
	struct dp_lct_rx_cnt lct_rx_cnt[2];
	dp_subif_t *ps_subif;
	u32 sub_if_id_grp;
	u64 *data_dst;
	u32 flags = 0;
	u32 lct_idx = 0;
	int ret;

	ps_subif = kzalloc(sizeof(dp_subif_t), GFP_ATOMIC);
	if (!ps_subif)
		return;

	/* Get port_id and sub_if information from net device */
	ret = dp_get_netif_subifid(dev, NULL, NULL, NULL, ps_subif, flags);
	if (ret != DP_SUCCESS) {
		netdev_err(dev, "dp_get_netif_subifid returned %d\n", ret);
		kfree(ps_subif);
		return;
	}

	if (dp_get_port_prop(ps_subif->inst, ps_subif->port_id, &port_prop)) {
		kfree(ps_subif);
		netdev_err(dev, "dp_get_port_prop fail\n");
		return;
	}

	sub_if_id_grp = GET_VAP(ps_subif->subif,
				port_prop.vap_offset,
				port_prop.vap_mask);

	/* Find required registered device on list */
	read_lock(&dev_list_lock);
	mib_cnt = find_registered_dev(ps_subif->port_id, sub_if_id_grp,
				      ps_subif->inst, dev);
	if (!mib_cnt) {
		kfree(ps_subif);
		read_unlock(&dev_list_lock);
		return;
	}

	kfree(ps_subif);

	/* Update counters for the requested net device */
	update_dev_counters(mib_cnt, !INITIAL_UPDATE);

	/* Copy wide counters to update by reinserted packets */
	wide_cnt_mod = mib_cnt->wide_cntrs;

	read_unlock(&dev_list_lock);

	/* Get reinserted packets counters */
	(void)dp_get_reinsert_cnt(mib_cnt->inst,
				     mib_cnt->port_id,
				     mib_cnt->sub_if_id_grp,
				     0,
				     &dp_reins_count);

	/* Take away reinserted packets */
	wide_cnt_mod.nRx64BytePkts -= dp_reins_count.dp_64BytePkts;
	wide_cnt_mod.nRx127BytePkts -= dp_reins_count.dp_127BytePkts;
	wide_cnt_mod.nRx255BytePkts -= dp_reins_count.dp_255BytePkts;
	wide_cnt_mod.nRx511BytePkts -= dp_reins_count.dp_511BytePkts;
	wide_cnt_mod.nRx1023BytePkts -= dp_reins_count.dp_1023BytePkts;
	wide_cnt_mod.nRxMaxBytePkts -= dp_reins_count.dp_MaxBytePkts;
	wide_cnt_mod.nRxOversizeGoodPkts -= dp_reins_count.dp_OversizeGoodPkts;
	wide_cnt_mod.nRxUnicastPkts -= dp_reins_count.dp_UnicastPkts;
	wide_cnt_mod.nRxBroadcastPkts -= dp_reins_count.dp_BroadcastPkts;
	wide_cnt_mod.nRxMulticastPkts -= dp_reins_count.dp_MulticastPkts;
	wide_cnt_mod.nRxGoodPkts -= dp_reins_count.dp_GoodPkts;
	wide_cnt_mod.nRxGoodBytes -= dp_reins_count.dp_GoodBytes;

	/* For LCT the rx packets are redirected in DP from a base interface
	 * to the "lct" sub-interface, so correct the hardware counters with
	 * the things happened in software.
	 * For interfaces without registered lct we will not get the counters
	 * and just ignore that.
	 */
	ret = dp_get_lct_cnt(mib_cnt->inst, mib_cnt->port_id, 0,
			     &lct_idx, lct_rx_cnt);
	if (ret == DP_SUCCESS) {
		/* base interfaces, remove unicast counters */
		if (mib_cnt->sub_if_id_grp == 0)
			update_lct_cnt(&wide_cnt_mod, &lct_rx_cnt[0], false);
		/* this is the lct, add unicast and multicast counters */
		if (lct_idx == mib_cnt->sub_if_id_grp) {
			update_lct_cnt(&wide_cnt_mod, &lct_rx_cnt[0], true);
			update_lct_cnt(&wide_cnt_mod, &lct_rx_cnt[1], true);
		}
	}

	/* Copy counters values to ethtool buffer */
	data_dst = data;

#define _DP_ETHTOOL(x, y)	*data_dst++ = wide_cnt_mod.y;
	DP_ETHTOOL_STAT_REG_DECLARE
#undef _DP_ETHTOOL
}

static int dp_dev_get_stat_strings_count(struct net_device *dev)
{
	return ARRAY_SIZE(dp_ethtool_gstrings);
}

static void dp_dev_get_stat_strings(struct net_device *dev, u8 *data)
{
	memcpy(data, dp_ethtool_gstrings, sizeof(dp_ethtool_gstrings));
}

/********************* workqueue handler ***************************/

static void cnt_upd_timer_handler(struct work_struct *work)
{
	/* Add delayed work object again for next run, the old object is not
	 * changed any more.
	 */
	queue_delayed_work(system_long_wq, &cnt_upd_work,
			   msecs_to_jiffies(CNT_UPDATE_TIME));

	update_all_devs_counters();
}

/*******************************************************************/
static int __init mib_counters_mod_init(void)
{
	struct dp_event event_info = {0};
	struct device_node *np;
	u32 gsw_mode = 0;
	int ret;

	/* Initialize empty list of registered net devices */
	INIT_LIST_HEAD(&registered_devices_list);

	rwlock_init(&dev_list_lock);

	/* Initialize and schedule  delayed work */
	INIT_DELAYED_WORK(&cnt_upd_work, cnt_upd_timer_handler);
	queue_delayed_work(system_long_wq, &cnt_upd_work,
			   msecs_to_jiffies(CNT_UPDATE_TIME));

	/* Registering of net dev register callback */
	event_info.inst = 0;
	event_info.owner = DP_EVENT_OWNER_MIB;
	event_info.type = DP_EVENT_REGISTER_SUBIF | DP_EVENT_DE_REGISTER_SUBIF;
	event_info.id = 0;
	event_info.dp_event_cb = subif_register_cb;

	ret = dp_register_event_cb(&event_info, 0);
	if (ret != DP_SUCCESS) {
		pr_err("Can't register DP_EVENT_REGISTERT_SUBIF callback\n");
		return ret;
	}

	/* Save callback ID for deregistration purpose */
	g_notify_cb_id = event_info.id;

	/* Get info from device tree about "shortcut" mode */
	np = of_find_node_by_name(NULL, "gsw_core");
	of_property_read_u32(np, "gsw_mode", &gsw_mode);
	shortcut_mode = ((gsw_mode & BIT(0)) == GSW_SHORTCUT_MODE);

	/* Register ethtool stats cb function */
	dp_set_ethtool_stats_fn(0, mib_cnt_get_ethtool_stats);
	dp_set_ethtool_stats_strings_cnt_fn(0, dp_dev_get_stat_strings_count);
	dp_set_ethtool_stats_strings_fn(0, dp_dev_get_stat_strings);

	gsw_ops = gsw_get_swcore_ops(0);
	if (!gsw_ops) {
		pr_err("%s swcore ops is NULL\n", __func__);
		return DP_FAILURE;
	}

	return 0;
}

static void __exit mib_counters_mod_exit(void)
{
	struct dp_event event_info = {0};
	int ret;
	struct mib_counters *mib_cnt;
	struct list_head *p, *q;

	/* Deregistering the callback function */
	event_info.inst = 0;
	event_info.owner = DP_EVENT_OWNER_MIB;
	event_info.type = DP_EVENT_REGISTER_SUBIF | DP_EVENT_DE_REGISTER_SUBIF;
	event_info.id = g_notify_cb_id;
	event_info.dp_event_cb = subif_register_cb;

	ret = dp_register_event_cb(&event_info, DP_F_DEREGISTER);
	if (ret != DP_SUCCESS) {
		pr_err("Can't deregister SUBIF notify callback\n");
		return;
	}

	/* Stop counter update and wait till current task finished */
	cancel_delayed_work_sync(&cnt_upd_work);

	/* Free all list entries for registered net devices */
	write_lock(&dev_list_lock);
	list_for_each_safe(p, q, &registered_devices_list)
	{
		mib_cnt = list_entry(p, struct mib_counters, dev_list);
		list_del(p);
		kfree(mib_cnt);
	}
	write_unlock(&dev_list_lock);

	/* Deregister ethtool stats cb function */
	dp_set_ethtool_stats_fn(0, NULL);
	dp_set_ethtool_stats_strings_cnt_fn(0, NULL);
	dp_set_ethtool_stats_strings_fn(0, NULL);
}

module_init(mib_counters_mod_init);
module_exit(mib_counters_mod_exit);

MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("MIB counters driver for overflow handling");
