/*
 * pp_qos_main.c
 * Description: Packet Processor QoS Driver
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2017-2020 Intel Corporation
 */

#define pr_fmt(fmt) "[PP_QOS_MAIN]:%s:%d: " fmt, __func__, __LINE__

#include <linux/firmware.h>
#include <linux/bitops.h>
#include <linux/bitmap.h>
#include <linux/pp_qos_api.h>
#include "pp_qos_common.h"
#include "pp_qos_utils.h"
#include "pp_qos_fw.h"
#include "pp_regs.h"

#define PP_QOS_ENTER_FUNC()
#define PP_QOS_EXIT_FUNC()

static struct pp_qos_dev *qos_devs[PP_QOS_MAX_INSTANCES];
static spinlock_t drv_lock;
u64 qos_wred_base_addr;

/* CREDIT_MAX  = BW Limit Credit max value is (2^21) 2097152 */
/* NUM_UPDATES = There are ~91 updates per second per Mhz    */
/* KBPS        = CREDIT_MAX * NUM_UPDATES / 500 ~= 300000    */
#define QOS_MAX_BW_KBPS_PER_1MHZ_CLK (300000)

#define QOS_RC_SKIP_CMDS (0xFCFC)

/* #define QOS_ASSERT_UPON_FAILURE */

void stop_run(void)
{
#ifdef QOS_ASSERT_UPON_FAILURE
	u32 id;

	pr_crit("Qos driver in under assert\n");

	/* Assert all QoS Instances */
	for (id = 0 ; id < PP_QOS_MAX_INSTANCES ; id++)
		QOS_BITS_SET(qos_devs[id]->dbg_flags, PP_QOS_FLAGS_ASSERT);
#endif
}

void update_cmd_id(struct driver_cmds *drvcmds)
{
	drvcmds->cmd_id++;
}

#ifndef PP_QOS_DISABLE_CMDQ

s32 __must_check transmit_cmds(struct pp_qos_dev *qdev)
{
	s32 rc;

	/* Handle commands in case there are commands in queue or in case */
	/* ports were suspended and need to be resumed again */
	while (!cmd_queue_is_empty(qdev->drvcmds.cmdq)) {
		enqueue_cmds(qdev);
		rc = check_completion(qdev);
		if (unlikely(rc)) {
			pr_err("Failed sending command\n");
			return -EINVAL;
		}
	}

	return 0;
}
#else
s32 __must_check transmit_cmds(struct pp_qos_dev *qdev)
{
	return 0;
}
#endif
/******************************************************************************/
/*                                      PORTS                                 */
/******************************************************************************/
static s32 set_port_specific_prop(struct pp_qos_dev *qdev,
				  struct qos_node *node,
				  const struct pp_qos_port_conf *conf,
				  u32 *modified)
{
	if (node->data.port.ring_address != conf->ring_address) {
		node->data.port.ring_address = conf->ring_address;
		QOS_BITS_SET(*modified, QOS_MODIFIED_RING_ADDRESS);
	}

	if (node->data.port.ring_size != conf->ring_size) {
		node->data.port.ring_size = conf->ring_size;
		QOS_BITS_SET(*modified, QOS_MODIFIED_RING_SIZE);
	}

	if (!!(conf->packet_credit_enable) !=
			!!QOS_BITS_IS_SET(node->flags,
				NODE_FLAGS_PORT_PACKET_CREDIT_EN)) {
		QOS_BITS_TOGGLE(node->flags,
				NODE_FLAGS_PORT_PACKET_CREDIT_EN);
		QOS_BITS_SET(*modified, QOS_MODIFIED_BEST_EFFORT);
	}

	if (node->data.port.credit != conf->credit) {
		node->data.port.credit = conf->credit;
		QOS_BITS_SET(*modified, QOS_MODIFIED_CREDIT);
	}

	if (!!(node->data.port.disable) != !!(conf->disable)) {
		node->data.port.disable = !!(conf->disable);
		QOS_BITS_SET(*modified, QOS_MODIFIED_DISABLE);
	}

	if (node->data.port.green_threshold != conf->green_threshold) {
		node->data.port.green_threshold = conf->green_threshold;
		QOS_BITS_SET(*modified, QOS_MODIFIED_PORT_GREEN_THRESHOLD);
	}

	if (node->data.port.yellow_threshold != conf->yellow_threshold) {
		node->data.port.yellow_threshold = conf->yellow_threshold;
		QOS_BITS_SET(*modified, QOS_MODIFIED_PORT_YELLOW_THRESHOLD);
	}

	if (!!conf->enhanced_wsp !=
	    QOS_BITS_IS_SET(node->flags, NODE_FLAGS_ENHANCED_WSP)) {
		QOS_BITS_SET(*modified, QOS_MODIFIED_PORT_ENHANCED_WSP);
		if (conf->enhanced_wsp)
			QOS_BITS_SET(node->flags, NODE_FLAGS_ENHANCED_WSP);
		else
			QOS_BITS_CLEAR(node->flags, NODE_FLAGS_ENHANCED_WSP);
	}

	return 0;
}

static s32 is_node_configured(const struct pp_qos_dev *qdev,
			      u32 id, bool *configured)
{
	struct qos_node *node;
	u32 phy;

	if (unlikely(!QOS_ID_VALID(id))) {
		QOS_LOG_ERR("Illegal id %u\n", id);
		return -EINVAL;
	}

	phy = get_phy_from_id(qdev->mapping, id);
	if (QOS_PHY_UNMAPPED(phy)) {
		*configured = false;
	} else if (QOS_PHY_VALID(phy)) {
		*configured = true;
		node = get_node_from_phy(qdev->nodes, phy);
		if (node_rep(node) || node_internal(node)) {
			QOS_LOG_ERR("id %u phy %u not valid\n", id, phy);
			return -EINVAL;
		}
	} else {
		QOS_LOG_ERR("Illegal phy %u\n", phy);
		return -EINVAL;
	}

	return 0;
}

/**
 * is_tree_replica_exists() - is tree replica exists. Used in dual rate shaper.
 * @qdev: handle to qos device instance obtained previously from qos_dev_init
 * @id:   Node id. Can be any id in the tree
 *
 * Return: true - if supported (Dual rate supported), false - not supported
 */
static bool is_tree_replica_exists(struct pp_qos_dev *qdev, u32 id)
{
	return is_port_wsp(qdev->nodes, get_phy_from_id(qdev->mapping, id));
}

/**
 * attach_dr_sched() - Binds a dual rate scheduler to a port
 * @qdev: handle to qos device instance obtained previously from qos_dev_init
 * @port_id:   Port id
 * @dr_sched_id:   Dual rate scheduler ID
 *
 * Return: N/A
 */
static void attach_dr_sched(struct pp_qos_dev *qdev,
			    u32 port_id, u32 dr_sched_id)
{
	struct qos_node *node;

	node = get_node_from_phy(qdev->nodes,
				 get_phy_from_id(qdev->mapping, port_id));

	node->dr_sched_id = dr_sched_id;

	node = get_node_from_phy(qdev->nodes,
				 get_phy_from_id(qdev->mapping, dr_sched_id));
	QOS_BITS_SET(node->flags, NODE_FLAGS_REPLICATION_NODE);
}

/******************************************************************************/
/*                                 API                                        */
/******************************************************************************/
static s32 _pp_qos_port_set(struct pp_qos_dev *qdev,
			    u32 id,
			    const struct pp_qos_port_conf *conf);

static s32 _pp_qos_port_conf_get(struct pp_qos_dev *qdev,
				 u32 id,
				 struct pp_qos_port_conf *conf);

void pp_qos_port_conf_set_default(struct pp_qos_port_conf *conf)
{
	memset(conf, 0, sizeof(struct pp_qos_port_conf));
	conf->common_prop.bandwidth_limit = QOS_NO_BANDWIDTH_LIMIT;
	conf->common_prop.shared_bw_group =
		QOS_NO_SHARED_BANDWIDTH_GROUP;
	conf->common_prop.max_burst = QOS_DEAFULT_MAX_BURST;
	conf->port_parent_prop.arbitration = PP_QOS_ARBITRATION_WSP;
	conf->packet_credit_enable = 1;
	conf->green_threshold = 0xFFFFFFFF;
	conf->yellow_threshold = 0xFFFFFFFF;
	conf->enhanced_wsp = 1;
}
EXPORT_SYMBOL(pp_qos_port_conf_set_default);

u32 pp_qos_port_id_get(struct pp_qos_dev *qdev, u32 physical_id)
{
	u32 id;

	if (ptr_is_null(qdev))
		return -EINVAL;

	id = get_id_from_phy(qdev->mapping, physical_id);
	if (!QOS_ID_VALID(id))
		return PP_QOS_INVALID_ID;

	return id;
}
EXPORT_SYMBOL(pp_qos_port_id_get);

s32 pp_qos_port_allocate(struct pp_qos_dev *qdev, u32 physical_id, u32 *_id)
{
	struct qos_node *node;
	u32 phy;
	s32 rc = 0;
	u32 id;

	QOS_LOG_API_DEBUG("port %u > allocate\n", physical_id);

	QOS_LOCK(qdev);
	PP_QOS_ENTER_FUNC();
	if (!qos_device_ready(qdev)) {
		rc = -EINVAL;
		goto out;
	}

	if (physical_id == ALLOC_PORT_ID) {
		/* port phys pool is NULL in case all ports are pre-reserved */
		if (!qdev->portsphys) {
			rc = -EBUSY;
			goto out;
		}

		phy = pp_pool_get(qdev->portsphys);
		if (!QOS_PHY_VALID(phy)) {
			rc = -EBUSY;
			goto out;
		}
	} else if (physical_id >= qdev->init_params.max_ports ||
		   !qdev->init_params.reserved_ports[physical_id]) {
		QOS_LOG("requested phy id %u is not reserved / out of range\n",
			physical_id);
		rc = -EINVAL;
		goto out;
	} else if (node_used(get_const_node_from_phy(qdev->nodes,
						     physical_id))) {
		QOS_LOG("port %u already used\n", physical_id);
		rc = -EBUSY;
		goto out;
	} else {
		phy = physical_id;
	}

	node = get_node_from_phy(qdev->nodes, phy);
	if (unlikely(!node)) {
		QOS_LOG_ERR("get_node_from_phy(%u) returned NULL\n",
			    phy);
		rc = -EINVAL;
		goto out;
	}

	node_init(qdev, node, 1, 1, 0);
	nodes_modify_used_status(qdev, phy, 1, 1);

	id = pp_pool_get(qdev->ids);
	QOS_ASSERT(QOS_ID_VALID(id), "got illegal id %u\n", id);
	map_id_phy(qdev->mapping, id, phy);
	*_id = id;
out:
	QOS_UNLOCK(qdev);
	return rc;
}
EXPORT_SYMBOL(pp_qos_port_allocate);

s32 pp_qos_port_disable(struct pp_qos_dev *qdev, u32 id)
{
	s32 rc;
	struct pp_qos_port_conf conf;

	QOS_LOCK(qdev);
	PP_QOS_ENTER_FUNC();
	if (!qos_device_ready(qdev)) {
		rc = -EINVAL;
		goto out;
	}

	rc = _pp_qos_port_conf_get(qdev, id, &conf);
	if (rc)
		goto out;

	conf.disable = 1;
	rc =  _pp_qos_port_set(qdev, id, &conf);
	if (rc)
		goto out;

	update_cmd_id(&qdev->drvcmds);
	rc = transmit_cmds(qdev);
out:
	QOS_UNLOCK(qdev);
	return rc;
}
EXPORT_SYMBOL(pp_qos_port_disable);

s32 pp_qos_port_enable(struct pp_qos_dev *qdev, u32 id)
{
	s32 rc;
	struct pp_qos_port_conf conf;

	QOS_LOCK(qdev);
	PP_QOS_ENTER_FUNC();
	if (!qos_device_ready(qdev)) {
		rc = -EINVAL;
		goto out;
	}

	rc = _pp_qos_port_conf_get(qdev, id, &conf);
	if (rc)
		goto out;
	conf.disable = 0;
	rc =  _pp_qos_port_set(qdev, id, &conf);
	if (rc)
		goto out;

	update_cmd_id(&qdev->drvcmds);
	rc = transmit_cmds(qdev);
out:
	QOS_UNLOCK(qdev);
	return rc;
}
EXPORT_SYMBOL(pp_qos_port_enable);

s32 pp_qos_port_block(struct pp_qos_dev *qdev, u32 id)
{
	const struct qos_node *node;
	bool node_configured;
	s32 rc;

	QOS_LOCK(qdev);
	PP_QOS_ENTER_FUNC();
	if (!qos_device_ready(qdev)) {
		rc = -EINVAL;
		goto out;
	}

	rc = is_node_configured(qdev, id, &node_configured);
	if (rc || !node_configured) {
		rc = -EINVAL;
		goto out;
	}

	node = get_conform_node(qdev, id, node_port);
	if (!node) {
		rc = -EINVAL;
		goto out;
	}
	rc = tree_modify_blocked_status(qdev,
					get_phy_from_id(qdev->mapping, id),
					1);
	if (rc)
		goto out;

	update_cmd_id(&qdev->drvcmds);
	rc = transmit_cmds(qdev);
out:
	QOS_UNLOCK(qdev);
	return rc;
}
EXPORT_SYMBOL(pp_qos_port_block);

s32 pp_qos_port_unblock(struct pp_qos_dev *qdev, u32 id)
{
	const struct qos_node *node;
	bool node_configured;
	s32 rc;

	QOS_LOCK(qdev);
	PP_QOS_ENTER_FUNC();
	if (!qos_device_ready(qdev)) {
		rc = -EINVAL;
		goto out;
	}

	rc = is_node_configured(qdev, id, &node_configured);
	if (rc || !node_configured) {
		rc = -EINVAL;
		goto out;
	}

	node = get_conform_node(qdev, id, node_port);
	if (!node) {
		rc = -EINVAL;
		goto out;
	}
	rc = tree_modify_blocked_status(qdev,
					get_phy_from_id(qdev->mapping, id),
					0);
	if (rc)
		goto out;

	update_cmd_id(&qdev->drvcmds);
	rc = transmit_cmds(qdev);
out:
	QOS_UNLOCK(qdev);
	return rc;
}
EXPORT_SYMBOL(pp_qos_port_unblock);

s32 pp_qos_port_remove(struct pp_qos_dev *qdev, u32 id)
{
	const struct qos_node *node;
	bool node_configured;
	u32 phy;
	s32 rc;

	QOS_LOCK(qdev);
	PP_QOS_ENTER_FUNC();
	if (!qos_device_ready(qdev)) {
		rc = -EINVAL;
		goto out;
	}

	rc = is_node_configured(qdev, id, &node_configured);
	if (unlikely(rc))
		goto out;

	/* If node is not configured just release id */
	if (!node_configured) {
		release_id(qdev, id);
		goto out;
	}

	node = get_conform_node(qdev, id, node_port);
	if (!node) {
		rc = -EINVAL;
		goto out;
	}

	phy = get_phy_from_node(qdev->nodes, node);
	rc = tree_remove(qdev, phy);
	if (rc)
		goto out;

	update_cmd_id(&qdev->drvcmds);
	rc = transmit_cmds(qdev);

	QOS_LOG_API_DEBUG("port %u > remove\n", id);
out:
	QOS_UNLOCK(qdev);
	return rc;
}
EXPORT_SYMBOL(pp_qos_port_remove);

static s32 _pp_qos_port_conf_get(struct pp_qos_dev *qdev,
				 u32 id,
				 struct pp_qos_port_conf *conf)
{
	const struct qos_node *node;
	bool node_configured;
	s32 rc;

	if (!conf)
		return -EINVAL;

	rc = is_node_configured(qdev, id, &node_configured);
	if (rc || !node_configured)
		return -EINVAL;

	node = get_conform_node(qdev, id, node_port);
	if (!node)
		return -EINVAL;

	conf->ring_address = node->data.port.ring_address;
	conf->ring_size = node->data.port.ring_size;
	conf->packet_credit_enable =
		!!QOS_BITS_IS_SET(node->flags,
				  NODE_FLAGS_PORT_PACKET_CREDIT_EN);
	conf->credit = node->data.port.credit;
	conf->disable = node->data.port.disable;
	conf->green_threshold = node->data.port.green_threshold;
	conf->yellow_threshold = node->data.port.yellow_threshold;
	conf->enhanced_wsp =
		!!QOS_BITS_IS_SET(node->flags, NODE_FLAGS_ENHANCED_WSP);

	return  get_node_prop(qdev,
			      node,
			      &conf->common_prop,
			      &conf->port_parent_prop,
			      NULL);
}

s32 pp_qos_port_conf_get(struct pp_qos_dev *qdev,
			 u32 id,
			 struct pp_qos_port_conf *conf)
{
	s32 rc;

	QOS_LOCK(qdev);
	PP_QOS_ENTER_FUNC();
	if (!qos_device_ready(qdev)) {
		rc = -EINVAL;
		goto out;
	}

	rc = _pp_qos_port_conf_get(qdev, id, conf);
out:
	QOS_UNLOCK(qdev);
	return rc;
}
EXPORT_SYMBOL(pp_qos_port_conf_get);

s32 pp_qos_port_info_get(struct pp_qos_dev *qdev,
			 u32 id,
			 struct pp_qos_port_info *info)
{
	const struct qos_node *node;
	bool node_configured;
	u32 phy;
	s32 rc;

	QOS_LOCK(qdev);
	PP_QOS_ENTER_FUNC();
	if (!qos_device_ready(qdev)) {
		rc = -EINVAL;
		goto out;
	}

	if (!info) {
		rc = -EINVAL;
		goto out;
	}

	rc = is_node_configured(qdev, id, &node_configured);
	if (rc || !node_configured) {
		rc = -EINVAL;
		goto out;
	}

	node = get_conform_node(qdev, id, node_port);
	if (!node) {
		rc = -EINVAL;
		goto out;
	}

	phy = get_phy_from_node(qdev->nodes, node);
	info->physical_id = phy;
	get_user_queues(qdev, phy, NULL, 0, &info->num_of_queues);
	update_cmd_id(&qdev->drvcmds);
	rc = transmit_cmds(qdev);
out:
	QOS_UNLOCK(qdev);
	return rc;
}
EXPORT_SYMBOL(pp_qos_port_info_get);

s32 pp_qos_port_get_queues(struct pp_qos_dev *qdev,
			   u32 id,
			   u16 *queue_ids,
			   u32 size,
			   u32 *queues_num)
{
	const struct qos_node *node;
	bool node_configured;
	u32 phy;
	s32 rc;

	QOS_LOCK(qdev);
	PP_QOS_ENTER_FUNC();
	if (!qos_device_ready(qdev)) {
		rc = -EINVAL;
		goto out;
	}

	rc = is_node_configured(qdev, id, &node_configured);
	if (rc || !node_configured) {
		rc = -EINVAL;
		goto out;
	}

	node = get_conform_node(qdev, id, node_port);
	if (!node) {
		rc = -EINVAL;
		goto out;
	}

	phy = get_phy_from_node(qdev->nodes, node);
	get_user_queues(qdev, phy, queue_ids, size, queues_num);
	update_cmd_id(&qdev->drvcmds);
	rc = transmit_cmds(qdev);
out:
	QOS_UNLOCK(qdev);
	return rc;
}
EXPORT_SYMBOL(pp_qos_port_get_queues);

s32 store_queue_list_max_allowed(struct pp_qos_dev *qdev,
				 u16 *rlms, u32 queues_num)
{
	s32 rc = 0;
	u32 queue_idx;
	u32 queue_id;
	u32 val;
	const struct qos_node *node;
	unsigned long addr = 0;

	if (unlikely(!qos_device_ready(qdev))) {
		rc = -EINVAL;
		goto out;
	}

	/* Store each queue max allowed value */
	for (queue_idx = 0; queue_idx < queues_num; queue_idx++) {
		queue_id = pp_qos_queue_id_get(qdev, *rlms);
		node = get_conform_node(qdev, queue_id, node_queue);
		if (unlikely(!node)) {
			rc = -EINVAL;
			goto out;
		}

		addr = (unsigned long)qdev->hwconf.max_allow_ddr_virt +
			(*rlms * sizeof(u32));

		*(u32 *)addr = node->data.queue.max_allowed;
		rlms++;
	}
	pp_cache_writeback(qdev->hwconf.max_allow_ddr_virt,
			   qdev->hwconf.max_allow_sz);

	/* Read last address to make sure data is written */
	if (addr)
		val = *(volatile u32 *)(addr);
out:
	return rc;
}

static void __port_api_dbg_print(struct pp_qos_dev *qdev, u32 id,
				 const struct pp_qos_port_conf *conf)
{
	char *buf = qdev->hwconf.api_str_buf;
	u32 len = 0;

	pr_buf_cat(buf, QOS_LOG_API_STR_MAX, len,
		   "port=%u bw=%u shared=%u max_burst %u arb=%u be=%u",
		   id, conf->common_prop.bandwidth_limit,
		   conf->common_prop.shared_bw_group,
		   conf->common_prop.max_burst,
		   conf->port_parent_prop.arbitration,
		   conf->port_parent_prop.best_effort_enable);

	if (conf->ring_address) {
		pr_buf_cat(buf, QOS_LOG_API_STR_MAX, len,
			   " r_addr=%#lx", (unsigned long)conf->ring_address);
	}

	if (conf->ring_size) {
		pr_buf_cat(buf, QOS_LOG_API_STR_MAX, len,
			   " r_size=%u", conf->ring_size);
	}

	pr_buf_cat(buf, QOS_LOG_API_STR_MAX, len,
		   " pkt_cred=%u cred=%u dis=%u green_threshold=%u"
		   " yellow_threshold=%u> port\n",
		   conf->packet_credit_enable,
		   conf->credit,
		   conf->disable,
		   conf->green_threshold,
		   conf->yellow_threshold);

	QOS_LOG_API_DEBUG("%s", buf);
}

static s32 _pp_qos_port_set(struct pp_qos_dev *qdev,
			    u32 id,
			    const struct pp_qos_port_conf *conf)
{
	s32 rc;
	u32 modified;
	struct qos_node node;
	struct qos_node *nodep;
	u16 phy;
	bool new_port = false;

	modified = 0;

	__port_api_dbg_print(qdev, id, conf);

	nodep = get_conform_node(qdev, id, NULL);
	if (!nodep)
		return -EINVAL;
	memcpy(&node, nodep, sizeof(node));

	if (node.type != TYPE_PORT) {
		node.type = TYPE_PORT;
		QOS_BITS_SET(modified, QOS_MODIFIED_NODE_TYPE);
		new_port = true;
	}

	rc = set_node_prop(qdev, &node, &conf->common_prop,
			   &conf->port_parent_prop, NULL, &modified);
	if (rc)
		return rc;

	rc = set_port_specific_prop(qdev, &node, conf, &modified);
	if (rc)
		return rc;

	if (!port_cfg_valid(qdev, &node, nodep)) {
		QOS_LOG_ERR("Port cfg is not valid (node %#x)\n",
			    get_phy_from_id(qdev->mapping, id));
		return -EINVAL;
	}

	memcpy(nodep, &node, sizeof(struct qos_node));

	if (modified) {
		phy = get_phy_from_node(qdev->nodes, nodep);
		create_set_port_cmd(qdev, conf, phy, modified);
	}

	if (!new_port &&
	    (modified & (QOS_MODIFIED_BANDWIDTH_LIMIT |
			 QOS_MODIFIED_SHARED_GROUP_ID |
			 QOS_MODIFIED_PORT_ENHANCED_WSP)))
		add_suspend_port(qdev, get_port(qdev->nodes, phy));

	return 0;
}

static s32 _pp_qos_sched_set(struct pp_qos_dev *qdev,
			     u32 id,
			     struct pp_qos_sched_conf *conf);

static s32 _rep_sched_set(struct pp_qos_dev *qdev,
			  u32 id,
			  const struct pp_qos_sched_conf *conf)
{
	s32 rc = 0;
	u32 rep_id;
	struct pp_qos_sched_conf rep_conf;
	struct qos_node *node;

	if (!is_tree_replica_exists(qdev, id))
		return 0;

	memcpy(&rep_conf, conf, sizeof(struct pp_qos_sched_conf));

	rep_id = get_rep_id_from_id(qdev->mapping, id);
	if (rep_id == PP_QOS_INVALID_ID) {
		/* Create a new rep scheduler */
		rep_id = pp_pool_get(qdev->ids);
		QOS_ASSERT(QOS_ID_VALID(rep_id), "illegal rep id %u\n", rep_id);
		map_id_reserved(qdev->mapping, rep_id);
		map_id_rep_id(qdev->mapping, id, rep_id);
	}

	/* Retrieve the parent rep id of the scheduler we are setting */
	rep_conf.sched_child_prop.parent = get_rep_id_from_id(qdev->mapping,
						conf->sched_child_prop.parent);

	rc = _pp_qos_sched_set(qdev, rep_id, &rep_conf);
	if (rc)
		return rc;

	map_id_rep_id(qdev->mapping, rep_id, id);

	node = get_node_from_phy(qdev->nodes, get_phy_from_id(qdev->mapping,
							      rep_id));
	QOS_BITS_SET(node->flags, NODE_FLAGS_REPLICATION_NODE);

	return rc;
}

/* When adding new WSP port, we internally add two schedulers:
 * dr_sched for priority 7 sched/queue, and replica tree (T') to hold
 * the replication of the tree (User can't access these schedulers directly)
 * We are not setting these schedulers (Only creating them)
 *
 * When user adds a scheduler, it will be added to the replica tree.
 * When user adds a queue, it will be added to the replica tree ONLY when
 * configuring non-zero eir (excess information rate) for this queue.
 * In this way, the user can define the committed rate (bw limit), and a
 * "nice to have" (eir) rate, in the same priority as the original node
 *
 *
 * Example: user adds a port, a scheduler and three queues: Queue1 and Queue2
 * with eir > 0, and Queue3 which is priority 7 Queue
 *
 *                    +------+
 *                    | Port |
 *                    | (WSP)|
 *                    +------+
 *       +--------+  +--------+  +-------------+
 *       | Sched1 |  | Queue2 |  |   dr_sched  |
 *       +--------+  +--------+  +-------------+
 *       +--------+             +--------+   +-------------------+
 *       | Queue1 |             + Queue3 +   | T' (tree replica) |
 *       +--------+             +--------+   +-------------------+
 *                                          +--------+  +--------+
 *                                          | Sched1'|  | Queue2'|
 *                                          +--------+  +--------+
 *                                          +--------+
 *                                          | Queue1'|
 *                                          +--------+
 */
static s32 _rep_port_set(struct pp_qos_dev *qdev, u32 port_id)
{
	s32 rc = 0;
	u32 dr_sched_id;
	u32 rep_id;
	struct pp_qos_sched_conf confs;
	struct qos_node *node;
	u32 phy;

	if (!is_tree_replica_exists(qdev, port_id))
		return 0;

	rep_id = get_rep_id_from_id(qdev->mapping, port_id);

	/* In Set port, we don't want to initiate a set scheduler */
	/* commands for the dr_sched scheduler & tree replica scheduler */
	if (rep_id != PP_QOS_INVALID_ID)
		return 0;

	/* Add dr_sched scheduler & tree replica scheduler */
	dr_sched_id = pp_pool_get(qdev->ids);
	QOS_ASSERT(QOS_ID_VALID(dr_sched_id), "illegal id %u\n", dr_sched_id);
	map_id_reserved(qdev->mapping, dr_sched_id);
	pp_qos_sched_conf_set_default(&confs);
	confs.sched_parent_prop.arbitration = PP_QOS_ARBITRATION_WSP;
	confs.sched_child_prop.parent = port_id;
	confs.sched_child_prop.priority = QOS_MAX_CHILD_PRIORITY;
	if (_pp_qos_sched_set(qdev, dr_sched_id, &confs)) {
		pp_pool_put(qdev->ids, dr_sched_id);
		QOS_LOG_ERR("Failed to create dr_sched sched\n");
		return -EINVAL;
	}
	attach_dr_sched(qdev, port_id, dr_sched_id);

	/* create T' */
	rep_id = pp_pool_get(qdev->ids);
	QOS_ASSERT(QOS_ID_VALID(rep_id), "illegal rep id %u\n", rep_id);
	map_id_reserved(qdev->mapping, rep_id);
	confs.sched_child_prop.parent = dr_sched_id;
	confs.sched_child_prop.priority = QOS_MAX_CHILD_PRIORITY;
	if (_pp_qos_sched_set(qdev, rep_id, &confs)) {
		pp_pool_put(qdev->ids, rep_id);
		QOS_LOG_ERR("Failed to create rep sched\n");
		return -EINVAL;
	}
	map_id_rep_id(qdev->mapping, port_id, rep_id);
	map_id_rep_id(qdev->mapping, rep_id, port_id);

	phy = get_phy_from_id(qdev->mapping, rep_id);
	node = get_node_from_phy(qdev->nodes, phy);
	QOS_BITS_SET(node->flags, NODE_FLAGS_REPLICATION_NODE);

	update_cmd_id(&qdev->drvcmds);
	rc = transmit_cmds(qdev);

	return rc;
}

s32 pp_qos_port_set(struct pp_qos_dev *qdev,
		    u32 id,
		    const struct pp_qos_port_conf *conf)
{
	s32 rc;

	QOS_LOCK(qdev);
	PP_QOS_ENTER_FUNC();
	if (!qos_device_ready(qdev)) {
		rc = -EINVAL;
		goto out;
	}

	rc = _pp_qos_port_set(qdev, id, conf);
	if (rc)
		goto out;

	update_cmd_id(&qdev->drvcmds);
	rc = transmit_cmds(qdev);

	if (!rc)
		rc = _rep_port_set(qdev, id);

out:
	QOS_UNLOCK(qdev);
	return rc;
}
EXPORT_SYMBOL(pp_qos_port_set);

/**
 * pp_qos_port_stat_get() - Get port's statistics
 * @qos_dev: handle to qos device instance obtained previously from qos_dev_init
 * @id: queue's id obtained from queue_allocate
 * @stat: pointer to struct to be filled with queue's statistics
 *
 * Return: 0 on success
 */
s32 pp_qos_port_stat_get(struct pp_qos_dev *qdev,
			 u32 id,
			 struct pp_qos_port_stat *stat)
{
	struct qos_node *node;
	bool node_configured;
	s32 rc;

	QOS_LOCK(qdev);
	PP_QOS_ENTER_FUNC();
	if (!qos_device_ready(qdev)) {
		rc = -EINVAL;
		goto out;
	}

	rc = is_node_configured(qdev, id, &node_configured);
	if (rc || !node_configured) {
		rc = -EINVAL;
		goto out;
	}

	node = get_conform_node(qdev, id, node_port);
	if (!node) {
		rc = -EINVAL;
		goto out;
	}

	create_get_port_stats_cmd(qdev,
				  get_phy_from_node(qdev->nodes, node),
				  qdev->hwconf.fw_stats_ddr_phys,
				  stat);

	update_cmd_id(&qdev->drvcmds);
	rc = transmit_cmds(qdev);
out:
	QOS_UNLOCK(qdev);
	return rc;
}
EXPORT_SYMBOL(pp_qos_port_stat_get);

/******************************************************************************/
/*                                     QUEUES                                 */
/******************************************************************************/
static s32 _pp_qos_queue_set(struct pp_qos_dev *qdev, u32 id,
			     struct pp_qos_queue_conf *conf, u16 rlm);

static s32 _pp_qos_queue_conf_get(struct pp_qos_dev *qdev, u32 id,
				  struct pp_qos_queue_conf *conf);

static s32 __qos_queue_stat_get(struct pp_qos_dev *qdev, u32 id,
				struct pp_qos_queue_stat *stat);

static void node_queue_init(struct pp_qos_dev *qdev, struct qos_node *node)
{
	node_init(qdev, node, 1, 0, 1);
	memset(&node->data.queue, 0x0, sizeof(struct _queue));
	node->data.queue.port_phy = QOS_INVALID_PHY;
	node->data.queue.last_active_port_phy = QOS_INVALID_PHY;
	node->type = TYPE_QUEUE;
}

static s32 set_queue_specific_prop(struct pp_qos_dev *qdev,
				   struct qos_node *node,
				   const struct pp_qos_queue_conf *conf,
				   u32 *modified)
{
	if (node->data.queue.green_min != conf->wred_min_avg_green) {
		node->data.queue.green_min = conf->wred_min_avg_green;
		QOS_BITS_SET(*modified, QOS_MODIFIED_WRED_MIN_GREEN);
	}

	if (node->data.queue.green_max != conf->wred_max_avg_green) {
		node->data.queue.green_max = conf->wred_max_avg_green;
		QOS_BITS_SET(*modified, QOS_MODIFIED_WRED_MAX_GREEN);
	}

	if (node->data.queue.green_slope != conf->wred_slope_green) {
		node->data.queue.green_slope = conf->wred_slope_green;
		QOS_BITS_SET(*modified, QOS_MODIFIED_WRED_SLOPE_GREEN);
	}

	if (node->data.queue.yellow_min != conf->wred_min_avg_yellow) {
		node->data.queue.yellow_min = conf->wred_min_avg_yellow;
		QOS_BITS_SET(*modified, QOS_MODIFIED_WRED_MIN_YELLOW);
	}

	if (node->data.queue.yellow_max != conf->wred_max_avg_yellow) {
		node->data.queue.yellow_max = conf->wred_max_avg_yellow;
		QOS_BITS_SET(*modified, QOS_MODIFIED_WRED_MAX_YELLOW);
	}

	if (node->data.queue.yellow_slope != conf->wred_slope_yellow) {
		node->data.queue.yellow_slope = conf->wred_slope_yellow;
		QOS_BITS_SET(*modified, QOS_MODIFIED_WRED_SLOPE_YELLOW);
	}

	if (node->data.queue.max_allowed != conf->wred_max_allowed) {
		node->data.queue.max_allowed = conf->wred_max_allowed;
		QOS_BITS_SET(*modified, QOS_MODIFIED_WRED_MAX_ALLOWED);
	}

	if (node->data.queue.min_guaranteed !=
			conf->wred_min_guaranteed) {
		node->data.queue.min_guaranteed =
			conf->wred_min_guaranteed;
		QOS_BITS_SET(*modified, QOS_MODIFIED_WRED_MIN_GUARANTEED);
	}

	if (!!(conf->wred_enable) !=
			!!QOS_BITS_IS_SET(node->flags,
				NODE_FLAGS_QUEUE_WRED_EN)) {
		QOS_BITS_TOGGLE(node->flags, NODE_FLAGS_QUEUE_WRED_EN);
		QOS_BITS_SET(*modified, QOS_MODIFIED_WRED_ENABLE);
	}

	if (!!(conf->wred_fixed_drop_prob_enable) !=
	    !!QOS_BITS_IS_SET(node->flags,
			      NODE_FLAGS_QUEUE_WRED_FIXED_DROP_PROB_EN)) {
		QOS_BITS_TOGGLE(node->flags,
				NODE_FLAGS_QUEUE_WRED_FIXED_DROP_PROB_EN);
		QOS_BITS_SET(*modified,
			     QOS_MODIFIED_WRED_FIXED_DROP_PROB_ENABLE);
	}

	if (node->data.queue.fixed_green_prob !=
			conf->wred_fixed_drop_prob_green) {
		node->data.queue.fixed_green_prob =
			conf->wred_fixed_drop_prob_green;
		QOS_BITS_SET(*modified, QOS_MODIFIED_WRED_FIXED_GREEN_PROB);
	}

	if (node->data.queue.fixed_yellow_prob !=
			conf->wred_fixed_drop_prob_yellow) {
		node->data.queue.fixed_yellow_prob =
			conf->wred_fixed_drop_prob_yellow;
		QOS_BITS_SET(*modified, QOS_MODIFIED_WRED_FIXED_YELLOW_PROB);
	}

	if (!!(conf->blocked) !=
			!!QOS_BITS_IS_SET(node->flags,
				NODE_FLAGS_QUEUE_BLOCKED)) {
		QOS_BITS_TOGGLE(node->flags, NODE_FLAGS_QUEUE_BLOCKED);
		QOS_BITS_SET(*modified, QOS_MODIFIED_BLOCKED);

		/* Store port phy once we block queue */
		node->data.queue.last_active_port_phy =
			node->data.queue.port_phy;
	}

	if (node->data.queue.codel_en != conf->codel_en) {
		node->data.queue.codel_en = !!conf->codel_en;
		QOS_BITS_SET(*modified, QOS_MODIFIED_CODEL);
	}

	return 0;
}

static s32 queue_cfg_valid(const struct pp_qos_dev *qdev,
			   const struct qos_node *node,
			   const struct qos_node *orig_node,
			   u32 prev_virt_parent_phy)
{
	if (!node_queue(node)) {
		QOS_LOG("Node is not a queue\n");
		return 0;
	}

	return node_cfg_valid(qdev, node, orig_node, prev_virt_parent_phy);
}

static s32 check_queue_conf_validity(struct pp_qos_dev *qdev, u32 id,
				     const struct pp_qos_queue_conf *conf,
				     struct qos_node *node, u32 *modified,
				     bool syncq, u16 rlm)
{
	u32 phy, mapped_rlm;
	s32 rc;
	struct qos_node *nodep;

	/* The phy of the current first ancesstor which is not an
	 * internal scheduler QOS_INVALID_PHY if it is a new queue
	 */
	u32 prev_virt_parent_phy;

	if (!QOS_ID_VALID(id)) {
		QOS_LOG_ERR("Illegal id %u\n", id);
		return -EINVAL;
	}

	/* Check if node id is valid */
	phy = get_phy_from_id(qdev->mapping, id);
	if (!QOS_PHY_VALID(phy)) {
		if (!QOS_PHY_UNMAPPED(phy)) {
			QOS_LOG_ERR("Id %u is not a node\n", id);
			return -EINVAL;
		}
		nodep = NULL;

		/* New queue which has id, but no phy was allocated for it */
		node_queue_init(qdev, node);

		if (syncq) {
			QOS_BITS_SET(node->flags, NODE_FLAGS_QUEUE_NEW_SYNCQ);
			if (conf->queue_child_prop.priority != 0) {
				QOS_LOG_ERR("low prio sync queue %u, abort\n",
					    id);
				return -EINVAL;
			}

			/* Allocate rlm from syncq pool */
			node->data.queue.rlm =
				allocate_rlm(qdev, id, RLM_RANGES_SYNCQ);
		} else if (rlm == QOS_INVALID_RLM) {
			mapped_rlm = get_phy_from_id(qdev->rlm_mapping, id);
			if (!QOS_PHY_VALID(mapped_rlm)) {
				mapped_rlm = allocate_rlm(qdev, id,
							  RLM_RANGES_NORMAL);
			}

			node->data.queue.rlm = mapped_rlm;
		} else {
			node->data.queue.rlm = rlm;
			node->data.queue.is_alias = 1;
		}

		if (node->data.queue.rlm == QOS_INVALID_RLM ||
		    node->data.queue.rlm >= qdev->init_params.max_queues) {
			QOS_LOG_ERR("illegal rlm %u\n", node->data.queue.rlm);
			return -EBUSY;
		}

		QOS_BITS_SET(node->flags, NODE_FLAGS_USED);
		QOS_BITS_SET(*modified,
			     QOS_MODIFIED_NODE_TYPE |
			     QOS_MODIFIED_BANDWIDTH_LIMIT |
			     QOS_MODIFIED_BW_WEIGHT |
			     QOS_MODIFIED_PARENT |
			     QOS_MODIFIED_PRIORITY |
			     QOS_MODIFIED_MAX_BURST |
			     QOS_MODIFIED_BLOCKED |
			     QOS_MODIFIED_WRED_ENABLE |
			     QOS_MODIFIED_WRED_FIXED_DROP_PROB_ENABLE |
			     QOS_MODIFIED_WRED_FIXED_GREEN_PROB |
			     QOS_MODIFIED_WRED_FIXED_YELLOW_PROB |
			     QOS_MODIFIED_WRED_MIN_GREEN |
			     QOS_MODIFIED_WRED_MAX_GREEN |
			     QOS_MODIFIED_WRED_SLOPE_GREEN |
			     QOS_MODIFIED_WRED_MIN_YELLOW |
			     QOS_MODIFIED_WRED_MAX_YELLOW |
			     QOS_MODIFIED_WRED_SLOPE_YELLOW |
			     QOS_MODIFIED_WRED_MAX_ALLOWED |
			     QOS_MODIFIED_WRED_MIN_GUARANTEED |
			     QOS_MODIFIED_RLM |
			     QOS_MODIFIED_CODEL);
		prev_virt_parent_phy = QOS_INVALID_PHY;
	} else {
		nodep = get_node_from_phy(qdev->nodes, phy);
		if (unlikely(!nodep)) {
			QOS_LOG_ERR("get_node_from_phy(%u) returned NULL\n",
				    phy);
			return -EINVAL;
		}

		if (!node_queue(nodep)) {
			QOS_LOG("Id %u is not a queue\n", id);
			rc = -EINVAL;
		}
		memcpy(node, nodep, sizeof(struct qos_node));
		prev_virt_parent_phy =
			get_virtual_parent_phy(qdev->nodes, node);
	}

	/* Before calling set_node_prop:
	 * If new node
	 *	node.child.parent.phy = INVALID_PHY
	 *	prev_virt_parent_phy = INVALID_PHY
	 * Else
	 *	node.child.parent.phy = actual parent
	 *	prev_virt_parent_phy = virtual parent
	 *	(i.e. first non internal ancestor)
	 *
	 * When returning:
	 * If new node
	 *	node.child.parent.phy == virtual parent phy
	 * Else
	 *	If map_id_to_phy(conf.parent) != prev_virt_parent_phy
	 *		node.child.parent.phy = map_id_to_phy(conf.parent)
	 *	Else
	 *		node.child.parent.phy = actual parent (no change)
	 */
	rc = set_node_prop(qdev, node, &conf->common_prop, NULL,
			   &conf->queue_child_prop, modified);
	if (rc)
		return rc;

	rc = set_queue_specific_prop(qdev, node, conf, modified);
	if (rc)
		return rc;

	if (!queue_cfg_valid(qdev, node, nodep, prev_virt_parent_phy)) {
		QOS_LOG_ERR("Queue cfg is not valid (node %#x)\n",
			    get_phy_from_id(qdev->mapping, id));
		return -EINVAL;
	}

	return 0;
}

void pp_qos_queue_conf_set_default(struct pp_qos_queue_conf *conf)
{
	memset(conf, 0, sizeof(struct pp_qos_queue_conf));
	conf->common_prop.bandwidth_limit = QOS_NO_BANDWIDTH_LIMIT;
	conf->common_prop.shared_bw_group =
		QOS_NO_SHARED_BANDWIDTH_GROUP;
	conf->common_prop.max_burst = QOS_DEAFULT_MAX_BURST;
	conf->wred_max_allowed = WRED_MAX_ALLOWED_DEFAULT;
	conf->queue_child_prop.wrr_weight = 1;
	conf->ebs = QOS_DEAFULT_MAX_BURST;
}
EXPORT_SYMBOL(pp_qos_queue_conf_set_default);

static void qos_queue_allocate(struct pp_qos_dev *qdev, u32 *id, u32 *rlm,
			       enum rlm_range range)
{
	u16 _id;

	_id = pp_pool_get(qdev->ids);
	QOS_ASSERT(QOS_ID_VALID(_id), "got illegal id %u\n", _id);
	*id = _id;
	map_id_reserved(qdev->mapping, _id);
	map_id_reserved(qdev->rlm_mapping, _id);

	if (!rlm)
		return;

	switch (range) {
	case RLM_RANGES_NORMAL:
	case RLM_RANGES_RESERVED:
		*rlm = allocate_rlm(qdev, _id, range);
	break;
	default:
		*rlm = QOS_INVALID_RLM;
	}
}

s32 pp_qos_queue_allocate_id_phy(struct pp_qos_dev *qdev, u32 *id, u32 *phy)
{
	s32 rc = 0;

	QOS_LOG_API_DEBUG("queue_id_phy > allocate\n");

	QOS_LOCK(qdev);
	PP_QOS_ENTER_FUNC();
	if (!qos_device_ready(qdev)) {
		rc = -EINVAL;
		goto out;
	}
	qos_queue_allocate(qdev, id, phy, RLM_RANGES_NORMAL);
out:
	QOS_UNLOCK(qdev);
	return rc;
}
EXPORT_SYMBOL(pp_qos_queue_allocate_id_phy);

s32 pp_qos_queue_allocate(struct pp_qos_dev *qdev, u32 *id)
{
	s32 rc = 0;

	QOS_LOG_API_DEBUG("queue > allocate\n");

	QOS_LOCK(qdev);
	PP_QOS_ENTER_FUNC();
	if (!qos_device_ready(qdev)) {
		rc = -EINVAL;
		goto out;
	}
	qos_queue_allocate(qdev, id, NULL, RLM_RANGES_MAX);
out:
	QOS_UNLOCK(qdev);
	return rc;
}
EXPORT_SYMBOL(pp_qos_queue_allocate);

s32 pp_qos_contiguous_queue_allocate(struct pp_qos_dev *qdev, u32 *ids,
				     u32 *phy_ids, u32 count)
{
	u32 id_capacity, rlm_capacity, queue_idx;
	s32 rc = 0;

	QOS_LOG_API_DEBUG("cont_queue %u > allocate\n", count);

	PP_QOS_ENTER_FUNC();
	if (!qos_device_ready(qdev))
		return -EINVAL;

	if (!count || count != qdev->init_params.reserved_queues) {
		QOS_LOG_ERR("Cannot allocate %u contiguous queues (only %u)\n",
			    count, qdev->init_params.reserved_queues);
		return -EINVAL;
	}

	QOS_LOCK(qdev);
	id_capacity = pp_pool_capacity_get(qdev->ids);
	rlm_capacity = pp_pool_capacity_get(qdev->reserved_rlms);

	if (id_capacity < count) {
		QOS_LOG_ERR("Not enough nodes (requested %u. free %u)\n", count,
			    id_capacity);
		rc = -ENOMEM;
		goto out;
	}

	if (rlm_capacity < count) {
		QOS_LOG_ERR("Not enough reserved queues (reques %u. free %u)\n",
			    count, rlm_capacity);
		rc = -ENOMEM;
		goto out;
	}

	for (queue_idx = 0; queue_idx < count; queue_idx++) {
		qos_queue_allocate(qdev, &ids[queue_idx], &phy_ids[queue_idx],
				   RLM_RANGES_RESERVED);
	}

out:
	QOS_UNLOCK(qdev);
	return rc;
}
EXPORT_SYMBOL(pp_qos_contiguous_queue_allocate);

/**
 * @brief __qos_sync_queue_add() - Add sync queue
 * @param qdev handle to qos device instance obtained previously
 *             from qos_dev_init
 * @param id queue's id obtained from queue_allocate
 * @param rlm returned allocated rlm
 * @param conf new configuration for the queue
 * @return s32 0 on success
 */
static s32 __qos_sync_queue_add(struct pp_qos_dev *qdev, u32 id, u32 *rlm,
				const struct pp_qos_queue_conf *conf)
{
	s32 rc;
	u32 phy, modified = 0;
	struct qos_node *nodep, node;

	rc = check_queue_conf_validity(qdev, id, conf, &node, &modified, true,
				       QOS_INVALID_RLM);
	if (unlikely(rc)) {
		QOS_LOG_ERR("check_queue_conf_validity failed\n");
		goto out;
	}

	/* add the synch queue to the sync mechanism */
	phy = alloc_phy_from_syncq(qdev, id);
	if (unlikely(!QOS_PHY_VALID(phy))) {
		QOS_LOG_ERR("Phy %u is not valid\n", phy);
		rc = -EINVAL;
		goto out;
	}

	nodep = get_node_from_phy(qdev->nodes, phy);
	if (unlikely(!nodep)) {
		QOS_LOG_ERR("get_node_from_phy(%u) returned NULL\n", phy);
		rc = -EINVAL;
		goto out;
	}

	memcpy(nodep, &node, sizeof(node));
	create_add_sync_queue_cmd(qdev, conf, phy, nodep->data.queue.rlm,
				  modified);

	*rlm = node.data.queue.rlm;
out:
	return rc;
}

s32 qos_sync_queue_add(struct pp_qos_dev *qdev, u32 id, u32 *rlm,
		       const struct pp_qos_queue_conf *conf)
{
	s32 rc = 0;

	QOS_LOG_API_DEBUG("queue=%u bw=%u shared=%u max_burst=%u parent=%u"
			  " priority=%u wrr_weight=%u max_burst=%u blocked=%u"
			  " wred_enable=%u fixed_drop_prob=%u"
			  " min_avg_green=%u max_avg_green=%u"
			  " slope_green=%u fixed_drop_prob_green=%u"
			  " min_avg_yellow=%u max_avg_yellow=%u"
			  " slope_yellow=%u fixed_drop_prob_yellow=%u"
			  " min_guaranteed=%u max_allowed=%u"
			  " codel_en=%u > syncq\n",
			  id, conf->common_prop.bandwidth_limit,
			  conf->common_prop.shared_bw_group,
			  conf->common_prop.max_burst,
			  conf->queue_child_prop.parent,
			  conf->queue_child_prop.priority,
			  conf->queue_child_prop.wrr_weight,
			  conf->common_prop.max_burst,
			  conf->blocked,
			  conf->wred_enable,
			  conf->wred_fixed_drop_prob_enable,
			  conf->wred_min_avg_green,
			  conf->wred_max_avg_green,
			  conf->wred_slope_green,
			  conf->wred_fixed_drop_prob_green,
			  conf->wred_min_avg_yellow,
			  conf->wred_max_avg_yellow,
			  conf->wred_slope_yellow,
			  conf->wred_fixed_drop_prob_yellow,
			  conf->wred_min_guaranteed,
			  conf->wred_max_allowed,
			  conf->codel_en);

	if (unlikely(ptr_is_null(rlm) || ptr_is_null(conf)))
		return -EINVAL;

	QOS_LOCK(qdev);
	PP_QOS_ENTER_FUNC();
	if (unlikely(!qos_device_ready(qdev))) {
		rc = -EINVAL;
		goto out;
	}

	rc = __qos_sync_queue_add(qdev, id, rlm, conf);
	if (unlikely(rc)) {
		QOS_LOG_ERR("__qos_sync_queue_add failed\n");
		goto out;
	}

	update_cmd_id(&qdev->drvcmds);
	rc = transmit_cmds(qdev);

out:
	QOS_UNLOCK(qdev);
	return rc;
}

s32 qos_sync_queue_connect(struct pp_qos_dev *qdev, u32 id, u32 dstq_id)
{
	s32 rc;
	u32 phy, dst_phy;
	struct qos_node *node;
	bool is_configured;

	/* TBD */
	/* QOS_LOG_API_DEBUG("queue=%u dst_queue=%u > syncq\n", id, dstq_id); */
	QOS_LOG_DEBUG("queue=%u dst_queue=%u\n", id, dstq_id);

	QOS_LOCK(qdev);
	PP_QOS_ENTER_FUNC();

	if (unlikely(!qos_device_ready(qdev))) {
		rc = -EINVAL;
		goto out;
	}
	rc = is_node_configured(qdev, id, &is_configured);
	if (unlikely(rc))
		goto out;
	if (unlikely(!is_configured))
		goto out;
	rc = is_node_configured(qdev, dstq_id, &is_configured);
	if (unlikely(rc))
		goto out;
	if (unlikely(!is_configured))
		goto out;

	/* get the phy */
	phy = get_phy_from_syncq(qdev, id);
	node = get_node_from_phy(qdev->nodes, phy);
	if (unlikely(!node)) {
		rc = -EINVAL;
		goto out;
	}
	/* verify the syncq is new */
	if (unlikely(!node_new_syncq(node))) {
		QOS_LOG_ERR("sync queue %u already connected\n", id);
		rc = -EINVAL;
		goto out;
	}
	/* verify the dstq phy is valid */
	dst_phy = get_phy_from_id(qdev->mapping, dstq_id);
	if (unlikely(!QOS_PHY_VALID(dst_phy))) {
		QOS_LOG_ERR("invalid phy %u\n", dst_phy);
		rc = -EINVAL;
		goto out;
	}
	/* sync the queues */
	rc = connect_sync_queue(qdev, phy, dst_phy);
	if (unlikely(rc))
		goto out;

	QOS_BITS_SET(node->flags,   NODE_FLAGS_QUEUE_CONNECTED_SYNCQ);
	QOS_BITS_CLEAR(node->flags, NODE_FLAGS_QUEUE_NEW_SYNCQ);

	/* send the command */
	update_cmd_id(&qdev->drvcmds);
	rc = transmit_cmds(qdev);

out:
	QOS_UNLOCK(qdev);
	return rc;
}

s32 qos_sync_queue_disconnect(struct pp_qos_dev *qdev, u32 id)
{
	struct pp_qos_queue_stat stat;
	s32 rc;
	bool is_configured;
	u32 phy;
	struct qos_node *node;

	/* TBD */
	/* QOS_LOG_API_DEBUG("queue=%u > syncq\n", id);*/
	QOS_LOG_DEBUG("queue=%u\n", id);

	QOS_LOCK(qdev);
	PP_QOS_ENTER_FUNC();

	if (unlikely(!qos_device_ready(qdev))) {
		rc = -EINVAL;
		goto out;
	}

	rc = is_node_configured(qdev, id, &is_configured);
	if (unlikely(rc))
		goto out;
	if (unlikely(!is_configured)) {
		rc = -EINVAL;
		goto out;
	}

	/* get the phy */
	phy = get_phy_from_syncq(qdev, id);
	node = get_node_from_phy(qdev->nodes, phy);
	if (unlikely(!node)) {
		rc = -EINVAL;
		goto out;
	}
	if (unlikely(!node_connected_syncq(node))) {
		QOS_LOG_ERR("sync queue %u not connected\n", id);
		rc = -EINVAL;
		goto out;
	}

	rc = _pp_qos_queue_block(qdev, id);
	if (unlikely(rc)) {
		QOS_LOG_ERR("_pp_qos_queue_block(id %u) failed\n", id);
		goto out;
	}

	/* all valid, see if we able to remove the syncq */
	rc = __qos_queue_stat_get(qdev, id, &stat);
	if (unlikely(rc)) {
		QOS_LOG_ERR("__qos_queue_stat_get(%u) failed\n", id);
		goto out;
	}
	if (stat.queue_packets_occupancy) {
		/* queue not empty, leave the queue blocked and try again */
		QOS_LOG_DEBUG("queue %u is not empty yet, try next time\n", id);
		rc = -EBUSY;
		goto out;
	}

	rc = disconnect_sync_queue(qdev, phy);
	if (unlikely(rc))
		goto out;

	rc = _pp_qos_queue_unblock(qdev, id);
	if (unlikely(rc)) {
		QOS_LOG_ERR("_pp_qos_queue_unblock(id %u) failed\n", id);
		goto out;
	}

	/* Set syncq to 'new' only after all tree operations finish */
	QOS_BITS_SET(node->flags,   NODE_FLAGS_QUEUE_NEW_SYNCQ);
	QOS_BITS_CLEAR(node->flags, NODE_FLAGS_QUEUE_CONNECTED_SYNCQ);

	/* send the command */
	update_cmd_id(&qdev->drvcmds);
	rc = transmit_cmds(qdev);

out:
	QOS_UNLOCK(qdev);
	return rc;
}

static s32 _get_actual_parent(struct pp_qos_dev *qdev, u32 parent_id, u32 prio)
{
	u32 id;
	u32 parent_phy;
	u32 dr_sched_id;
	u32 port_id;
	u32 port_phy;

	id = parent_id;

	if (is_tree_replica_exists(qdev, id)) {
		parent_phy = get_phy_from_id(qdev->mapping, id);
		dr_sched_id = get_dr_sched_id(qdev, id);

		/* Parent can be changed only if parent is port or dr_sched */
		/* dr_sched_id can be zero if dr_sched_id has no node yet */
		if (dr_sched_id != 0 &&
		    (node_port(get_node_from_phy(qdev->nodes, parent_phy)) ||
		    id == dr_sched_id)) {
			/* if new prio is 7, new parent is dr_sched */
			if (prio == QOS_MAX_CHILD_PRIORITY) {
				id = dr_sched_id;
			} else if (id == dr_sched_id) { /* parent is port */
				port_phy = get_port(qdev->nodes, parent_phy);
				port_id = get_id_from_phy(qdev->mapping,
							  port_phy);
				id = port_id;
			}
		}
	}

	return id;
}

static s32 _pp_qos_queue_conf_get(struct pp_qos_dev *qdev, u32 id,
				  struct pp_qos_queue_conf *conf)
{
	bool node_configured;
	const struct qos_node *node;
	s32 rc;

	if (!conf)
		return -EINVAL;

	rc = is_node_configured(qdev, id, &node_configured);
	if (rc || !node_configured)
		return -EINVAL;

	node = get_conform_node(qdev, id, node_queue);
	if (!node)
		return -EINVAL;

	conf->wred_min_avg_green = node->data.queue.green_min;
	conf->wred_max_avg_green = node->data.queue.green_max;
	conf->wred_slope_green = node->data.queue.green_slope;
	conf->wred_min_avg_yellow = node->data.queue.yellow_min;
	conf->wred_max_avg_yellow = node->data.queue.yellow_max;
	conf->wred_slope_yellow = node->data.queue.yellow_slope;
	conf->wred_max_allowed = node->data.queue.max_allowed;
	conf->wred_min_guaranteed = node->data.queue.min_guaranteed;
	conf->wred_fixed_drop_prob_green =
		node->data.queue.fixed_green_prob;
	conf->wred_fixed_drop_prob_yellow =
		node->data.queue.fixed_yellow_prob;
	conf->wred_enable =
		!!QOS_BITS_IS_SET(node->flags,
				NODE_FLAGS_QUEUE_WRED_EN);
	conf->wred_fixed_drop_prob_enable =
		!!QOS_BITS_IS_SET(node->flags,
			NODE_FLAGS_QUEUE_WRED_FIXED_DROP_PROB_EN);
	conf->blocked =
		!!QOS_BITS_IS_SET(node->flags,
				NODE_FLAGS_QUEUE_BLOCKED);
	conf->codel_en = !!node->data.queue.codel_en;
	conf->eir = node->data.queue.eir;
	conf->ebs = node->data.queue.ebs;

	return get_node_prop(qdev, node, &conf->common_prop,
			     NULL, &conf->queue_child_prop);
}

static void __queue_api_dbg_print(struct pp_qos_dev *qdev, u32 id, bool get_api,
				  const struct pp_qos_queue_conf *conf)
{
	char *buf = qdev->hwconf.api_str_buf;
	u32 len = 0;

	pr_buf_cat(buf, QOS_LOG_API_STR_MAX, len,
		   "queue=%u bw=%u shared=%u max_burst %u parent=%u"
		   " priority=%u wrr_weight=%u max_burst=%u blocked=%u"
		   " wred_enable=%u",
		   id, conf->common_prop.bandwidth_limit,
		   conf->common_prop.shared_bw_group,
		   conf->common_prop.max_burst,
		   conf->queue_child_prop.parent,
		   conf->queue_child_prop.priority,
		   conf->queue_child_prop.wrr_weight,
		   conf->common_prop.max_burst,
		   conf->blocked,
		   conf->wred_enable);

	if (conf->wred_enable &&
	    (conf->wred_fixed_drop_prob_enable ||
	     conf->wred_slope_green ||
	     conf->wred_slope_yellow)) {
		pr_buf_cat(buf, QOS_LOG_API_STR_MAX, len,
			   " fixed_drop_prob=%u min_avg_green=%u"
			   " max_avg_green=%u slope_green=%u"
			   " fixed_drop_prob_green=%u min_avg_yellow=%u"
			   " max_avg_yellow=%u slope_yellow=%u"
			   " fixed_drop_prob_yellow=%u",
			   conf->wred_fixed_drop_prob_enable,
			   conf->wred_min_avg_green,
			   conf->wred_max_avg_green,
			   conf->wred_slope_green,
			   conf->wred_fixed_drop_prob_green,
			   conf->wred_min_avg_yellow,
			   conf->wred_max_avg_yellow,
			   conf->wred_slope_yellow,
			   conf->wred_fixed_drop_prob_yellow);
	}

	pr_buf_cat(buf, QOS_LOG_API_STR_MAX, len,
		   " min_guaranteed=%u max_allowed=%u"
		   " codel_en=%u eir=%u ebs=%u%s\n",
		   conf->wred_min_guaranteed,
		   conf->wred_max_allowed,
		   conf->codel_en,
		   conf->eir,
		   conf->ebs,
		   get_api ? "" : " > queue");

	if (get_api)
		QOS_LOG_GET_API_DEBUG("%s", buf);
	else
		QOS_LOG_API_DEBUG("%s", buf);
}

static s32 qos_get_user_parent(struct pp_qos_dev *qdev,
			       u32 parent_id,
			       u32 *user_parent_id)
{
	u32 dr_sched_id;
	u32 parent_phy;
	struct qos_node *parent;

	parent_phy = get_phy_from_id(qdev->mapping, parent_id);
	if (!QOS_PHY_VALID(parent_phy))
		return -EINVAL;

	parent = get_node_from_phy(qdev->nodes, parent_phy);

	if (node_rep(parent)) {
		dr_sched_id = get_dr_sched_id(qdev, parent_id);

		if (!node_sched(parent)) {
			QOS_LOG_ERR("replica parent %u is not a sched\n",
				    parent_id);
			return -EINVAL;
		}

		/* Scenario in which a priority 7 queue's parent is dr_sched */
		if (parent_id != dr_sched_id) {
			QOS_LOG_ERR("Parent(%u) rep is not dr_sched (%u)\n",
				    parent_id, dr_sched_id);
			return -EINVAL;
		}

		/* Return the parent of dr_sched */
		*user_parent_id =
		    get_id_from_phy(qdev->mapping,
				    parent->child_prop.parent_phy);
	} else if (node_sched_syncq(parent)) {
		/* Return the parent of syncq sched */
		*user_parent_id =
		    get_id_from_phy(qdev->mapping,
				    parent->child_prop.parent_phy);
	} else {
		*user_parent_id = parent_id;
	}

	return 0;
}

s32 pp_qos_queue_conf_get(struct pp_qos_dev *qdev, u32 id,
			  struct pp_qos_queue_conf *conf)
{
	s32 rc;

	QOS_LOCK(qdev);
	PP_QOS_ENTER_FUNC();
	if (!qos_device_ready(qdev)) {
		rc = -EINVAL;
		goto out;
	}

	rc = _pp_qos_queue_conf_get(qdev, id, conf);
	if (rc)
		goto out;

	rc = qos_get_user_parent(qdev,
				 conf->queue_child_prop.parent,
				 &conf->queue_child_prop.parent);

	__queue_api_dbg_print(qdev, id, true, conf);
out:
	QOS_UNLOCK(qdev);
	return rc;
}
EXPORT_SYMBOL(pp_qos_queue_conf_get);

static s32 qos_queue_remove(struct pp_qos_dev *qdev, s32 id)
{
	struct qos_node *node;
	struct qos_node *parent;
	struct queue_stats_s qstat;
	s32 rc;

	node = get_conform_node(qdev, id, node_queue);
	if (!node)
		return -EINVAL;

	/* Check if the queue is empty (only if this is non-replica node)
	 * In case node is replica, we should remove it anyway
	 */
	if (!node_rep(node)) {
		memset(&qstat, 0, sizeof(qstat));
		create_get_queue_stats_cmd(qdev, get_phy_from_id(qdev->mapping,
								 id),
					   node->data.queue.rlm,
					   qdev->hwconf.fw_stats_ddr_phys,
					   &qstat, false);
		update_cmd_id(&qdev->drvcmds);
		rc = transmit_cmds(qdev);
		if (rc)
			return rc;

		if (qstat.qmgr_num_queue_entries) {
			QOS_LOG_ERR("Queue %u Occ %u, cannot remove queue\n",
				id, qstat.qmgr_num_queue_entries);
			return -EBUSY;
		}
	}

	parent = get_node_from_phy(qdev->nodes, node->child_prop.parent_phy);

	rc = node_remove(qdev, node);
	if (rc)
		return rc;

	/* If parent is empty internal scheduler, remove it */
	if (node_internal(parent) && !parent->parent_prop.num_of_children)
		return node_remove(qdev, parent);

	return rc;
}

static s32 _pp_qos_queue_remove(struct pp_qos_dev *qdev, u32 id)
{
	u32 rep_id, rlm;
	s32 rc = 0;
	bool node_configured;
	struct qos_node *node;

	rc = is_node_configured(qdev, id, &node_configured);
	if (unlikely(rc))
		return rc;

	/* If node is not configured just release id & rlm */
	if (!node_configured) {
		rlm = get_phy_from_id(qdev->rlm_mapping, id);
		if (QOS_PHY_VALID(rlm))
			release_rlm(qdev, rlm);

		release_id(qdev, id);
		return rc;
	}

	node = get_conform_node(qdev, id, node_queue);
	if (!node) {
		QOS_LOG_ERR("node NULL id %u\n", id);
		return -EINVAL;
	}

	rep_id = get_rep_id_from_id(qdev->mapping, id);

	if (QOS_ID_VALID(rep_id)) {
		rc = qos_queue_remove(qdev, rep_id);
		update_cmd_id(&qdev->drvcmds);
		rc = transmit_cmds(qdev);
		if (rc)
			return rc;
	}

	rc = qos_queue_remove(qdev, id);
	if (rc)
		return rc;

	update_cmd_id(&qdev->drvcmds);
	rc = transmit_cmds(qdev);

	return rc;
}

/**
 * pp_qos_queue_remove() - Remove a queue
 * @qos_dev: handle to qos device instance obtained previously from qos_dev_init
 * @id:   queue's id obtained from queue_allocate
 *
 * Note: client should make sure that queue is empty and
 * that new packets are not enqueued, by calling
 *       pp_qos_queue_disable
 *
 * Return: 0 on success
 */
s32 pp_qos_queue_remove(struct pp_qos_dev *qdev, u32 id)
{
	u32 rlm;
	s32 rc;

	QOS_LOCK(qdev);
	PP_QOS_ENTER_FUNC();
	if (!qos_device_ready(qdev)) {
		rc = -EINVAL;
		goto out;
	}

	rlm = get_phy_from_id(qdev->rlm_mapping, id);
	if (QOS_PHY_VALID(rlm)) {
		if (rlm_get_range_type(qdev, rlm) == RLM_RANGES_RESERVED) {
			QOS_LOG_ERR("Cannot remove cont queue id %u\n", id);
			rc = -EINVAL;
			goto out;
		}
	}

	rc = _pp_qos_queue_remove(qdev, id);

	QOS_LOG_API_DEBUG("queue %u > remove\n", id);
out:
	QOS_UNLOCK(qdev);
	if (rc == QOS_RC_SKIP_CMDS)
		return 0;

	return rc;
}
EXPORT_SYMBOL(pp_qos_queue_remove);

s32 pp_qos_contiguous_queue_remove(struct pp_qos_dev *qdev, u32 id, u32 count)
{
	u32 rlm_idx, _id;
	u32 first_rlm, last_rlm;
	s32 rc = 0;

	QOS_LOG_API_DEBUG("cont_queue %u %u > remove\n", id, count);

	PP_QOS_ENTER_FUNC();
	if (!qos_device_ready(qdev))
		return -EINVAL;

	if (!count || count != qdev->init_params.reserved_queues) {
		QOS_LOG_ERR("Cannot remove %u contiguous queues (only %u)\n",
			    count, qdev->init_params.reserved_queues);
		return -EINVAL;
	}

	first_rlm = get_phy_from_id(qdev->rlm_mapping, id);
	if (rlm_get_range_type(qdev, first_rlm) != RLM_RANGES_RESERVED) {
		QOS_LOG_ERR("Physical Queue ID %u is not contiguous\n",
			    first_rlm);
		return -EINVAL;
	}

	last_rlm = first_rlm + count - 1;
	if (rlm_get_range_type(qdev, last_rlm) != RLM_RANGES_RESERVED) {
		QOS_LOG_ERR("Physical Queue ID %u is not contiguous\n",
			    last_rlm);
		return -EINVAL;
	}

	QOS_LOCK(qdev);
	for (rlm_idx = last_rlm + 1; rlm_idx > first_rlm; rlm_idx--) {
		_id = get_id_from_phy(qdev->rlm_mapping, rlm_idx - 1);
		rc |= _pp_qos_queue_remove(qdev, _id);
	}
	QOS_UNLOCK(qdev);

	return rc;
}
EXPORT_SYMBOL(pp_qos_contiguous_queue_remove);

static u32 get_phy_by_priority(const struct pp_qos_dev *qdev,
			       u32 parent_id, u32 priority)
{
	u32 phy = QOS_INVALID_PHY;
	u32 num;
	struct qos_node *child = NULL;
	struct qos_node *parent;
	u32 dr_sched_id;

	parent = get_node_from_phy(qdev->nodes,
				   get_phy_from_id(qdev->mapping, parent_id));
	if (unlikely(!parent)) {
		QOS_LOG_ERR("parent has no node\n");
		return QOS_INVALID_PHY;
	}

	if (parent->parent_prop.arbitration != PP_QOS_ARBITRATION_WSP)
		return QOS_INVALID_PHY;

	num = parent->parent_prop.num_of_children;
	phy = parent->parent_prop.first_child_phy;
	if (!QOS_PHY_VALID(phy))
		return QOS_INVALID_PHY;

	child = get_node_from_phy(qdev->nodes, phy);

	if (unlikely(!child)) {
		QOS_LOG_ERR("phy %u is not a child\n", phy);
		return QOS_INVALID_PHY;
	}

	while (num) {
		/*
		 * Two options if same priority:
		 * Parent is dr_sched - We're ignoring the T'
		 * return the phy of queue/scheduler if exists.
		 * Parent is not dr_sched - return the phy
		 */
		if (priority == child->child_prop.priority) {
			dr_sched_id = get_dr_sched_id(qdev, parent_id);
			if (dr_sched_id == parent_id) {
				if (!node_rep(child))
					return phy;
			} else {
				return phy;
			}
		}

		++phy;
		++child;
		--num;
	}

	return QOS_INVALID_PHY;
}

/**
 * pp_qos_queue_set() - Set queue configuration
 * @qos_dev: handle to qos device instance obtained previously from qos_dev_init
 * @id:	  queue's id obtained from queue_allocate
 * @conf: new configuration for the queue
 *
 * Return: 0 on success
 */
static s32 _pp_qos_queue_set(struct pp_qos_dev *qdev, u32 id,
			     struct pp_qos_queue_conf *conf, u16 rlm)
{
	s32 rc;
	u32 phy, parent_phy;
	u32 prev_phy;
	struct qos_node *parent;
	struct qos_node node;
	struct qos_node *nodep = NULL;
	u32 modified = 0;
	s32 parent_changed = 0;
	u32 src_port, dst_port;
	struct queue_stats_s qstat;
	u32 ddr_addr;

	__queue_api_dbg_print(qdev, id, false, conf);

	phy = get_phy_from_id(qdev->mapping, id);
	if (!QOS_PHY_VALID(phy))
		goto no_port_move;

	nodep = get_node_from_phy(qdev->nodes, phy);
	parent_phy = get_phy_from_id(qdev->mapping,
				     conf->queue_child_prop.parent);
	src_port = get_port(qdev->nodes, phy);
	dst_port = get_port(qdev->nodes, parent_phy);

	if (src_port == dst_port)
		goto no_port_move;

	/* Queue must be blocked while moving between ports */
	if (!QOS_BITS_IS_SET(nodep->flags, NODE_FLAGS_QUEUE_BLOCKED) &&
	    !conf->blocked) {
		QOS_LOG_ERR("Q%u must be blocked during port move(%u => %u)\n",
			    id, src_port, dst_port);
		return -EINVAL;
	}

	/* Check if queue is about to be unblocked */
	if (QOS_BITS_IS_SET(nodep->flags, NODE_FLAGS_QUEUE_BLOCKED) &&
	    !conf->blocked) {
		/* Once unblocked after port move, Queue must be empty */
		if (QOS_PHY_VALID(nodep->data.queue.last_active_port_phy) &&
		    (nodep->data.queue.last_active_port_phy != dst_port)) {
			ddr_addr = qdev->hwconf.fw_stats_ddr_phys;
			memset(&qstat, 0, sizeof(qstat));
			create_get_queue_stats_cmd(qdev, phy,
						   nodep->data.queue.rlm,
						   ddr_addr, &qstat, false);
			update_cmd_id(&qdev->drvcmds);
			rc = transmit_cmds(qdev);
			if (rc)
				goto out;

			if (qstat.qmgr_num_queue_entries) {
				QOS_LOG_ERR("Cannot unblock Q%u (Occ %u)\n",
					    id, qstat.qmgr_num_queue_entries);
				return -EINVAL;
			}
		}
	}

no_port_move:
	conf->queue_child_prop.parent = _get_actual_parent(qdev,
		conf->queue_child_prop.parent, conf->queue_child_prop.priority);

	phy = get_phy_by_priority(qdev, conf->queue_child_prop.parent,
				  conf->queue_child_prop.priority);
	if (QOS_PHY_VALID(phy) &&
	    id != get_id_from_phy(qdev->mapping, phy)) {
		QOS_LOG_ERR("Prio %u is occupied by phy %u\n",
			    conf->queue_child_prop.priority, phy);
		rc = -EINVAL;
		goto out;
	}

	rc = check_queue_conf_validity(qdev, id, conf, &node, &modified,
				       false, rlm);

	parent_changed = QOS_BITS_IS_SET(modified, QOS_MODIFIED_PARENT);

	if (rc)
		goto out;

	if (parent_changed) {
		parent = get_node_from_phy(qdev->nodes,
					   node.child_prop.parent_phy);
		if (unlikely(!parent)) {
			QOS_LOG_ERR("get_node_from_phy(%u) returned NULL\n",
				    node.child_prop.parent_phy);
			rc = -EINVAL;
			goto out;
		}

		phy = phy_alloc_by_parent(qdev, parent,
					  conf->queue_child_prop.priority);
		if (unlikely(!QOS_PHY_VALID(phy))) {
			QOS_LOG_ERR("Phy %u is not valid\n", phy);
			rc = -EINVAL;
			goto out;
		}
		nodep = get_node_from_phy(qdev->nodes, phy);
		if (unlikely(!nodep)) {
			QOS_LOG_ERR("get_node_from_phy(%u) returned NULL\n",
				    phy);
			rc = -EINVAL;
			goto out;
		}

		node.child_prop.parent_phy = nodep->child_prop.parent_phy;
		memcpy(nodep, &node, sizeof(struct qos_node));

		/* If this is not a new queue - delete previous node */
		if (!QOS_BITS_IS_SET(modified, QOS_MODIFIED_NODE_TYPE)) {
			prev_phy = get_phy_from_id(qdev->mapping, id);
			add_suspend_port(qdev, get_port(qdev->nodes, prev_phy));
			dst_port = get_port(qdev->nodes, phy);
			create_move_cmd(qdev, phy, prev_phy, dst_port);
			map_invalidate_id_phy(qdev->mapping, id);
			map_id_phy(qdev->mapping, id, phy);
			node_phy_delete(qdev, prev_phy);
			phy = get_phy_from_id(qdev->mapping, id);
			nodep = get_node_from_phy(qdev->nodes, phy);
			if (unlikely(!nodep)) {
				QOS_LOG_ERR("get_node_from_phy(%u) NULL\n",
					    phy);
				rc = -EINVAL;
				goto out;
			}

			if (phy != prev_phy)
				update_predecessors(qdev, nodep, NULL);
		} else {
			map_id_phy(qdev->mapping, id, phy);
		}

		parent = get_node_from_phy(qdev->nodes,
					   nodep->child_prop.parent_phy);
		if (unlikely(!parent)) {
			QOS_LOG_ERR("get_node_from_phy(%u) returned NULL\n",
				    nodep->child_prop.parent_phy);
			rc = -EINVAL;
			goto out;
		}
	} else {
		phy = get_phy_from_id(qdev->mapping, id);
		nodep = get_node_from_phy(qdev->nodes, phy);
		if (unlikely(!nodep)) {
			QOS_LOG_ERR("get_node_from_phy(%u) returned NULL\n",
				    phy);
			rc = -EINVAL;
			goto out;
		}

		parent = get_node_from_phy(qdev->nodes,
					   nodep->child_prop.parent_phy);
		if (unlikely(!parent)) {
			QOS_LOG_ERR("get_node_from_phy(%u) returned NULL\n",
				    nodep->child_prop.parent_phy);
			rc = -EINVAL;
			goto out;
		}

		/* Child of WSP changes priority i.e. position */
		if ((parent->parent_prop.arbitration ==
		     PP_QOS_ARBITRATION_WSP) &&
		    QOS_BITS_IS_SET(modified, QOS_MODIFIED_PRIORITY)) {
			update_children_position(qdev, nodep, parent, &node);
		}  else {
			memcpy(nodep, &node, sizeof(struct qos_node));
		}
	}

	if (modified & (QOS_MODIFIED_BANDWIDTH_LIMIT |
			QOS_MODIFIED_SHARED_GROUP_ID))
		add_suspend_port(qdev, get_port(qdev->nodes, phy));

	if (modified != QOS_MODIFIED_PARENT) {
		create_set_queue_cmd(qdev, conf, phy,
				     nodep, modified,
				     node_new_syncq(nodep));
	}

	if (QOS_BITS_IS_SET(modified, QOS_MODIFIED_PARENT) ||
	    QOS_BITS_IS_SET(modified, QOS_MODIFIED_BW_WEIGHT))
		update_internal_bandwidth(qdev, parent);

out:
	if (!rc || !parent_changed)
		return rc;

	if (QOS_BITS_IS_SET(modified, QOS_MODIFIED_RLM)) {
		phy = get_phy_from_id(qdev->rlm_mapping, id);

		/* Make sure this id is mapped to an rlm */
		if (!QOS_PHY_UNMAPPED(phy)) {
			QOS_ASSERT(phy == node.data.queue.rlm,
				   "mapped_rlm %u != node's rlm %u\n",
				   phy, node.data.queue.rlm);
			release_rlm(qdev, node.data.queue.rlm);
		}
	}

	return rc;
}

static s32 _set_queue_eir(struct pp_qos_dev *qdev, u32 id, u32 eir)
{
	struct qos_node *node;

	node = get_conform_node(qdev, id, node_queue);
	if (!node) {
		QOS_LOG_ERR("%s: Node is NULL, id %u\n", __func__, id);
		return -EINVAL;
	}

	node->data.queue.eir = eir;

	return 0;
}

static s32 _rep_queue_set(struct pp_qos_dev *qdev,
			  u32 id,
			  const struct pp_qos_queue_conf *conf)
{
	s32 rc;
	u32 rep_id;
	u32 dr_sched_id;
	u32 port_id = 0;
	struct pp_qos_queue_conf rep_conf;
	struct qos_node *node;
	struct qos_node *rep_node;
	u32 parent_id;
	u32 parent_phy;
	u32 port_phy;
	u32 port_rep_id = 0;

	if (!is_tree_replica_exists(qdev, id))
		return 0;

	if (conf->ebs > QOS_MAX_MAX_BURST) {
		QOS_LOG_ERR("ebs %u > max %u\n", conf->ebs, QOS_MAX_MAX_BURST);
		return -EINVAL;
	}

	node = get_conform_node(qdev, id, node_queue);
	if (!node) {
		QOS_LOG_ERR("%s: Node is NULL, id %u\n", __func__, id);
		return -EINVAL;
	}

	memcpy(&rep_conf, conf, sizeof(struct pp_qos_queue_conf));

	rep_id = get_rep_id_from_id(qdev->mapping, id);
	if (rep_id == PP_QOS_INVALID_ID) {
		rep_id = pp_pool_get(qdev->ids);
		QOS_ASSERT(QOS_ID_VALID(rep_id), "illegal rep id %u\n", rep_id);
		map_id_reserved(qdev->mapping, rep_id);
		map_id_rep_id(qdev->mapping, id, rep_id);
		map_id_reserved(qdev->rlm_mapping, rep_id);
	}

	/* The original node holds the BW limit and the eir of the rep node */
	/* The aliased node holds its eir in the BW limit field (eir is 0) */
	node->data.queue.eir = conf->eir;
	rep_conf.common_prop.bandwidth_limit = conf->eir;
	rep_conf.eir = 0;

	node->data.queue.ebs = conf->ebs;
	rep_conf.common_prop.max_burst = conf->ebs;
	rep_conf.ebs = 0;

	dr_sched_id = get_dr_sched_id(qdev, conf->queue_child_prop.parent);

	if (conf->queue_child_prop.parent == dr_sched_id) {
		parent_phy = get_phy_from_id(qdev->mapping,
					     conf->queue_child_prop.parent);
		port_phy = get_port(qdev->nodes, parent_phy);
		port_id = get_id_from_phy(qdev->mapping, port_phy);
		port_rep_id = get_rep_id_from_id(qdev->mapping, port_id);
		rep_conf.queue_child_prop.parent = port_rep_id;
	} else {
		parent_id = get_rep_id_from_id(qdev->mapping,
					       conf->queue_child_prop.parent);
		rep_conf.queue_child_prop.parent = parent_id;
	}

	/* Call queue set for the rep_id */
	rc = _pp_qos_queue_set(qdev, rep_id, &rep_conf, node->data.queue.rlm);
	if (rc) {
		QOS_LOG_ERR("%s: _pp_qos_queue_set failed\n", __func__);
		return rc;
	}
	map_id_rep_id(qdev->mapping, rep_id, id);

	rep_node = get_node_from_phy(qdev->nodes,
				     get_phy_from_id(qdev->mapping, rep_id));
	QOS_BITS_SET(rep_node->flags, NODE_FLAGS_REPLICATION_NODE);

	return 0;
}

s32 pp_qos_queue_set(struct pp_qos_dev *qdev, u32 id,
		     const struct pp_qos_queue_conf *conf)
{
	s32 rc;
	u32 phy;
	u32 rep_id;
	u32 prev_eir = 0;
	struct qos_node *node;
	struct pp_qos_queue_conf temp_conf;

	QOS_LOCK(qdev);
	PP_QOS_ENTER_FUNC();
	if (!qos_device_ready(qdev)) {
		rc = -EINVAL;
		goto out;
	}

	/* Verify user is not setting a replica parent */
	phy = get_phy_from_id(qdev->mapping, conf->queue_child_prop.parent);
	if (!QOS_PHY_VALID(phy))
		goto queue_set_cont;

	node = get_node_from_phy(qdev->nodes, phy);
	if (node_rep(node) || node_internal(node)) {
		QOS_LOG_ERR("Parent %u is not valid\n",
			    conf->queue_child_prop.parent);
		rc = -EINVAL;
		goto out;
	}

queue_set_cont:
	memcpy(&temp_conf, conf, sizeof(struct pp_qos_queue_conf));

	/* Save Current EIR from node, if exists */
	phy = get_phy_from_id(qdev->mapping, id);
	if (QOS_PHY_VALID(phy) && is_tree_replica_exists(qdev, id)) {
		node = get_node_from_phy(qdev->nodes, phy);
		prev_eir = node->data.queue.eir;
	}

	rc = _pp_qos_queue_set(qdev, id, &temp_conf, QOS_INVALID_RLM);
	if (rc)
		goto out;

	if (is_tree_replica_exists(qdev, id)) {
		if (conf->eir == 0) {
			/* Reset the EIR */
			_set_queue_eir(qdev, id, 0);

			/* Remove the rep_id */
			if (prev_eir > 0) {
				rep_id = get_rep_id_from_id(qdev->mapping, id);
				if (QOS_ID_VALID(rep_id)) {
					rc = qos_queue_remove(qdev, rep_id);
					if (rc) {
						QOS_LOG_ERR("rm rep error\n");
						_set_queue_eir(qdev, id,
							       prev_eir);
					}
				} else {
					QOS_LOG_ERR("%u has no rep_id\n", id);
				}
			}
		} else {
			/* Add or update rep_id */
			_rep_queue_set(qdev, id, conf);
		}
	}

	update_cmd_id(&qdev->drvcmds);
	rc = transmit_cmds(qdev);

out:
	QOS_UNLOCK(qdev);
	return rc;
}
EXPORT_SYMBOL(pp_qos_queue_set);

s32 _pp_qos_queue_block(struct pp_qos_dev *qdev, u32 id)
{
	s32 rc;
	struct pp_qos_queue_conf conf;

	rc = _pp_qos_queue_conf_get(qdev, id, &conf);
	if (rc)
		return rc;
	conf.blocked = 1;
	return _pp_qos_queue_set(qdev, id, &conf, QOS_INVALID_RLM);
}

/**
 * pp_qos_queue_block() - All new packets enqueue to this queue will be dropped
 * @qos_dev: handle to qos device instance obtained previously from qos_dev_init
 * @id:   queue's id obtained from queue_allocate
 *
 * Note	- already enqueued descriptors will be transmitted
 *
 * Return: 0 on success
 */
s32 pp_qos_queue_block(struct pp_qos_dev *qdev, u32 id)
{
	s32 rc;

	QOS_LOCK(qdev);
	PP_QOS_ENTER_FUNC();
	if (!qos_device_ready(qdev)) {
		rc = -EINVAL;
		goto out;
	}
	rc = _pp_qos_queue_block(qdev, id);
	if (rc)
		goto out;

	update_cmd_id(&qdev->drvcmds);
	rc = transmit_cmds(qdev);
out:
	QOS_UNLOCK(qdev);
	return rc;
}
EXPORT_SYMBOL(pp_qos_queue_block);

s32 _pp_qos_queue_unblock(struct pp_qos_dev *qdev, u32 id)
{
	s32 rc;
	struct pp_qos_queue_conf conf;

	rc = _pp_qos_queue_conf_get(qdev, id, &conf);
	if (rc)
		return rc;

	conf.blocked = 0;
	return _pp_qos_queue_set(qdev, id, &conf, QOS_INVALID_RLM);
}

/**
 * pp_qos_queue_unblock() - Unblock enqueuing of new packets
 * @qos_dev: handle to qos device instance obtained previously from qos_dev_init
 * @id:	  queue's id obtained from queue_allocate
 *
 * Return: 0 on success
 */
s32 pp_qos_queue_unblock(struct pp_qos_dev *qdev, u32 id)
{
	s32 rc;

	QOS_LOCK(qdev);
	PP_QOS_ENTER_FUNC();
	if (!qos_device_ready(qdev)) {
		rc = -EINVAL;
		goto out;
	}
	rc = _pp_qos_queue_unblock(qdev, id);
	if (rc)
		goto out;

	update_cmd_id(&qdev->drvcmds);
	rc = transmit_cmds(qdev);
out:
	QOS_UNLOCK(qdev);
	return rc;
}
EXPORT_SYMBOL(pp_qos_queue_unblock);

/**
 * pp_qos_queue_info_get() - Get information about queue
 * @qos_dev: handle to qos device instance obtained previously from qos_dev_init
 * @id:	  queue's id obtained from queue_allocate
 * @info: pointer to struct to be filled with queue's info
 *
 * Return: 0 on success
 */
s32 pp_qos_queue_info_get(struct pp_qos_dev *qdev,
			  u32 id,
			  struct pp_qos_queue_info *info)
{
	struct qos_node *node;
	bool node_configured;
	u32 rc;

	QOS_LOCK(qdev);
	PP_QOS_ENTER_FUNC();
	if (unlikely(!qos_device_ready(qdev))) {
		rc = -EINVAL;
		goto out;
	}

	if (ptr_is_null(info)) {
		rc = -EINVAL;
		goto out;
	}

	rc = is_node_configured(qdev, id, &node_configured);
	if (rc || !node_configured) {
		rc = -EINVAL;
		goto out;
	}

	node = get_conform_node(qdev, id, node_queue);
	if (ptr_is_null(node)) {
		rc = -EINVAL;
		goto out;
	}

	info->physical_id = node->data.queue.rlm;
	info->port_id =
		get_port(qdev->nodes, get_phy_from_node(qdev->nodes, node));
	update_cmd_id(&qdev->drvcmds);
	rc = transmit_cmds(qdev);
out:
	QOS_UNLOCK(qdev);
	return rc;
}
EXPORT_SYMBOL(pp_qos_queue_info_get);

u32 pp_qos_queue_id_get(struct pp_qos_dev *qdev, u32 physical_id)
{
	u32 id;

	id = get_id_from_phy(qdev->rlm_mapping, physical_id);
	if (!QOS_ID_VALID(id))
		return PP_QOS_INVALID_ID;

	return id;
}
EXPORT_SYMBOL(pp_qos_queue_id_get);

static s32 __qos_queue_stat_get(struct pp_qos_dev *qdev, u32 id,
				struct pp_qos_queue_stat *stat)
{
	s32 rc;
	u32 bytes_accepted_h;
	u32 bytes_accepted_l;
	u32 bytes_drop_h;
	u32 bytes_drop_l;
	struct qos_node *node;
	struct queue_stats_s qstat;

	node = get_conform_node(qdev, id, node_queue);
	if (!node) {
		rc = -EINVAL;
		goto out;
	}

	memset(&qstat, 0, sizeof(qstat));
	create_get_queue_stats_cmd(qdev,
				   get_phy_from_node(qdev->nodes, node),
				   node->data.queue.rlm,
				   qdev->hwconf.fw_stats_ddr_phys,
				   &qstat, stat->reset);

	update_cmd_id(&qdev->drvcmds);
	rc = transmit_cmds(qdev);

	stat->queue_packets_occupancy = qstat.queue_size_entries;
	stat->queue_bytes_occupancy = qstat.queue_size_bytes;
	stat->total_packets_accepted = qstat.total_accepts;
	stat->total_packets_dropped = qstat.total_drops;
	stat->total_packets_red_dropped = qstat.total_red_dropped;
	bytes_accepted_h = qstat.total_bytes_added_high;
	bytes_accepted_l = qstat.total_bytes_added_low;
	stat->total_bytes_accepted =
		((u64)bytes_accepted_h << 32) | bytes_accepted_l;
	bytes_drop_h = qstat.total_dropped_bytes_high;
	bytes_drop_l = qstat.total_dropped_bytes_low;
	stat->total_bytes_dropped =
		((u64)bytes_drop_h << 32) | bytes_drop_l;

out:
	return rc;
}

/**
 * pp_qos_queue_stat_get() - Get queue's statistics
 * @qos_dev: handle to qos device instance obtained previously from qos_dev_init
 * @id:	  queue's id obtained from queue_allocate
 * @stat: pointer to struct to be filled with queue's statistics
 *
 * Return: 0 on success
 */
s32 pp_qos_queue_stat_get(struct pp_qos_dev *qdev, u32 id,
			  struct pp_qos_queue_stat *stat)
{
	bool node_configured;
	s32 rc;

	QOS_LOCK(qdev);
	PP_QOS_ENTER_FUNC();
	if (!qos_device_ready(qdev)) {
		rc = -EINVAL;
		goto out;
	}

	rc = is_node_configured(qdev, id, &node_configured);
	if (rc || !node_configured) {
		rc = -EINVAL;
		goto out;
	}

	rc = __qos_queue_stat_get(qdev, id, stat);
out:
	QOS_UNLOCK(qdev);
	return rc;
}
EXPORT_SYMBOL(pp_qos_queue_stat_get);

s32 qos_clock_set(struct pp_qos_dev *qdev, u32 clk_MHz)
{
	ulong nodes[BITS_TO_LONGS(NUM_OF_NODES)] = { 0 };
	s32 rc;

	if (unlikely(ptr_is_null(qdev)))
		return -EINVAL;

	QOS_LOCK(qdev);
	PP_QOS_ENTER_FUNC();
	if (!qos_device_ready(qdev)) {
		rc = -ENODEV;
		goto out;
	}

	/* Calculate maximum bw limit */
	qdev->hwconf.max_bw_limit_kbps = QOS_MAX_BW_KBPS_PER_1MHZ_CLK * clk_MHz;
	rc = qos_clk_depend_nodes_list_get(qdev, nodes, NUM_OF_NODES);
	if (unlikely(rc))
		goto out;

	create_clk_update_cmd(qdev, clk_MHz, nodes, NUM_OF_NODES);
	update_cmd_id(&qdev->drvcmds);
	rc = transmit_cmds(qdev);
out:
	QOS_UNLOCK(qdev);
	return rc;
}

/******************************************************************************/
/*                                     CODEL                                  */
/******************************************************************************/
s32 pp_qos_codel_cfg_set(struct pp_qos_dev *qdev, struct pp_qos_codel_cfg *cfg)
{
	s32 rc;

	QOS_LOCK(qdev);
	PP_QOS_ENTER_FUNC()
	if (unlikely(!qos_device_ready(qdev))) {
		rc = -EINVAL;
		goto out;
	}

	if (unlikely(ptr_is_null(cfg))) {
		rc = -EINVAL;
		goto out;
	}

	if (unlikely(cfg->consecutive_drop_limit >
		     PP_QOSCODEL_MAX_CONSECUTIVE_DROP_LIMIT)) {
		QOS_LOG_ERR("CONSECUTIVE_DROP_LIMIT %u is invalid (max %u)\n",
			    cfg->consecutive_drop_limit,
			    PP_QOSCODEL_MAX_CONSECUTIVE_DROP_LIMIT);
		rc = -EINVAL;
		goto out;
	}

	create_codel_cfg_set_cmd(qdev, cfg);
	update_cmd_id(&qdev->drvcmds);
	rc = transmit_cmds(qdev);

out:
	QOS_UNLOCK(qdev);
	return rc;
}
EXPORT_SYMBOL(pp_qos_codel_cfg_set);

static s32 __qos_codel_queue_stat_get(struct pp_qos_dev *qdev, u32 id,
				      struct pp_qos_codel_stat *stat)
{
	struct qos_node *node;

	node = get_conform_node(qdev, id, node_queue);
	if (unlikely(!node))
		return -EINVAL;

	if (!node->data.queue.codel_en)
		return 0;

	get_phy_from_node(qdev->nodes, node);
	create_get_codel_stats_cmd(qdev, node->data.queue.rlm,
				   qdev->hwconf.fw_stats_ddr_phys, stat);
	update_cmd_id(&qdev->drvcmds);

	return transmit_cmds(qdev);
}

/**
 * pp_qos_queue_stat_get() - Get queue's statistics
 * @qos_dev: handle to qos device instance obtained previously from qos_dev_init
 * @id:	  queue's id obtained from queue_allocate
 * @stat: pointer to struct to be filled with queue's statistics
 *
 * Return: 0 on success
 */
s32 pp_qos_codel_queue_stat_get(struct pp_qos_dev *qdev, u32 id,
				struct pp_qos_codel_stat *stat)
{
	s32 rc;

	QOS_LOCK(qdev);
	PP_QOS_ENTER_FUNC();
	if (!qos_device_ready(qdev)) {
		rc = -EINVAL;
		goto out;
	}

	rc = __qos_codel_queue_stat_get(qdev, id, stat);
out:
	QOS_UNLOCK(qdev);
	return rc;
}
EXPORT_SYMBOL(pp_qos_codel_queue_stat_get);

s32 __qos_aqm_rlms_attach_get(struct pp_qos_dev *qdev, u8 sf_id, u32 *rlms,
			      struct pp_qos_aqm_sf_config *sf_cfg)
{
	s32 rc = 0;
	u32 queue_idx;
	struct qos_node *node;
	struct pp_qos_queue_conf conf;

	/* Iterate through queue list */
	for (queue_idx = 0; queue_idx < sf_cfg->num_queues; queue_idx++) {
		node = get_conform_node(qdev,
					sf_cfg->queue_id[queue_idx],
					node_queue);
		if (unlikely(!node)) {
			QOS_LOG_ERR("SF %u. queue %u invalid\n",
				    sf_id, sf_cfg->queue_id[queue_idx]);
			rc = -EINVAL;
			goto out;
		}

		/* WRED is not allowed in parallel to AQM */
		if (QOS_BITS_IS_SET(node->flags, NODE_FLAGS_QUEUE_WRED_EN)) {
			QOS_LOG_ERR("SF %u. queue %u wred is enabled\n",
				    sf_id, sf_cfg->queue_id[queue_idx]);
			rc = -EINVAL;
			goto out;
		}

		/* Compose rlm list */
		rlms[queue_idx] = node->data.queue.rlm;

		/* Mark AQM node */
		QOS_BITS_SET(node->flags, NODE_FLAGS_QUEUE_AQM);
		node->data.queue.aqm_context = sf_id;

		/* Set max allowed to support AQM buffer */
		rc = _pp_qos_queue_conf_get(qdev,
					    sf_cfg->queue_id[queue_idx],
					    &conf);
		if (unlikely(rc)) {
			QOS_LOG_ERR("SF %u. queue %u conf get failed\n",
				    sf_id, sf_cfg->queue_id[queue_idx]);
			rc = -EINVAL;
			goto out;
		}

		/* Handle worst case in which all buffers are 64 bytes long */
		conf.wred_max_allowed = sf_cfg->buffer_size >> 6;
		rc = _pp_qos_queue_set(qdev,
				       sf_cfg->queue_id[queue_idx],
				       &conf, QOS_INVALID_RLM);
		if (unlikely(rc)) {
			QOS_LOG_ERR("SF %u. queue %u conf get failed\n",
				    sf_id, sf_cfg->queue_id[queue_idx]);
			rc = -EINVAL;
			goto out;
		}

		update_cmd_id(&qdev->drvcmds);
	}

out:
	return rc;
}

/**
 * @brief Get the packet statistics
 * @param qos_dev: handle to qos device instance obtained from
 *        pp_qos_dev_open
 * @param stats
 * @param reset_stats specify whether to reset the stats or not
 * @return s32 0 on success, error code otherwise
 */
static inline
s32 __qos_stats_get(struct pp_qos_dev *qos_dev, struct pp_qos_stats *stats,
		    bool reset_stats)
{
	const struct qos_node *node;
	struct pp_qos_queue_stat qstats;
	struct pp_qos_codel_stat cstats;
	u32 node_id;
	u32 id;
	s32 rc;

	for_each_qos_queue(qos_dev, node) {
		node_id = get_phy_from_node(qos_dev->nodes, node);
		id      = get_id_from_phy(qos_dev->mapping, node_id);
		QOS_ASSERT(QOS_ID_VALID(id),
			   "Invalid id for phy %u\n", node_id);

		memset(&qstats, 0, sizeof(qstats));
		qstats.reset = (s32)reset_stats;
		rc = __qos_queue_stat_get(qos_dev, id, &qstats);
		if (rc)
			return rc;

		memset(&cstats, 0, sizeof(cstats));
		cstats.reset = (s32)reset_stats;
		rc = __qos_codel_queue_stat_get(qos_dev, id, &cstats);
		if (unlikely(rc))
			return rc;

		if (!stats)
			continue;

		/* packets */
		stats->pkts_rcvd += qstats.total_packets_accepted +
				    qstats.total_packets_dropped;
		stats->pkts_dropped += qstats.total_packets_dropped +
				       cstats.packets_dropped;
		stats->pkts_transmit += qstats.total_packets_accepted -
					qstats.queue_packets_occupancy -
					cstats.packets_dropped;
		/* bytes */
		stats->bytes_rcvd += qstats.total_bytes_accepted +
				    qstats.total_bytes_dropped;
		stats->bytes_dropped += qstats.total_bytes_dropped +
				       cstats.bytes_dropped;
		stats->bytes_transmit += qstats.total_bytes_accepted -
					 qstats.queue_bytes_occupancy -
					 cstats.bytes_dropped;
		/* drop reasons */
		stats->wred_pkts_dropped = qstats.total_packets_dropped;
		stats->codel_pkts_dropped = cstats.packets_dropped;
	}

	return 0;
}

s32 pp_qos_aqm_sf_set(struct pp_qos_dev *qdev, u8 sf_id,
		      struct pp_qos_aqm_sf_config *sf_cfg)
{
	s32 rc;
	u32 rlms[PP_QOS_AQM_CONTEXT_MAX_QUEUES];

	QOS_LOCK(qdev);
	PP_QOS_ENTER_FUNC()
	if (unlikely(!qos_device_ready(qdev))) {
		rc = -EINVAL;
		goto out;
	}

	if (unlikely(ptr_is_null(sf_cfg))) {
		rc = -EINVAL;
		goto out;
	}

	if (unlikely(sf_id >= PP_QOS_AQM_MAX_SERVICE_FLOWS)) {
		QOS_LOG_ERR("SF %u is invalid (max %u)\n",
			    sf_id, PP_QOS_AQM_MAX_SERVICE_FLOWS);
		rc = -EINVAL;
		goto out;
	}

	if (qdev->sf_entry[sf_id].enabled) {
		QOS_LOG_ERR("SF %u. already enabled\n", sf_id);
		rc = -EINVAL;
		goto out;
	}

	if (unlikely(sf_cfg->num_queues > PP_QOS_AQM_CONTEXT_MAX_QUEUES)) {
		QOS_LOG_ERR("SF %u. Num queues %u invalid (max %u)\n",
			    sf_id, sf_cfg->num_queues,
			    PP_QOS_AQM_CONTEXT_MAX_QUEUES);
		rc = -EINVAL;
		goto out;
	}

	rc = __qos_aqm_rlms_attach_get(qdev, sf_id, rlms, sf_cfg);
	if (unlikely(rc))
		goto out;

	/* Store SF config in qos db */
	memcpy(&qdev->sf_entry[sf_id], sf_cfg, sizeof(*sf_cfg));
	qdev->sf_entry[sf_id].enabled = true;

	create_aqm_sf_set_cmd(qdev, sf_id, true, sf_cfg, rlms);
	update_cmd_id(&qdev->drvcmds);
	rc = transmit_cmds(qdev);

out:
	QOS_UNLOCK(qdev);
	return rc;
}
EXPORT_SYMBOL(pp_qos_aqm_sf_set);

s32 __qos_aqm_rlms_dettach_get(struct pp_qos_dev *qdev, u8 sf_id, u32 *rlms,
			       struct pp_qos_aqm_sf_config *sf_cfg)
{
	s32 rc = 0;
	u32 queue_idx;
	struct qos_node *node;
	struct pp_qos_queue_conf conf;

	for (queue_idx = 0; queue_idx < sf_cfg->num_queues; queue_idx++) {
		node = get_conform_node(qdev,
					sf_cfg->queue_id[queue_idx],
					node_queue);
		if (unlikely(!node)) {
			QOS_LOG_ERR("SF %u. queue %u invalid\n",
				    sf_id, sf_cfg->queue_id[queue_idx]);
			rc = -EINVAL;
			goto out;
		}

		rlms[queue_idx] = node->data.queue.rlm;
		QOS_BITS_CLEAR(node->flags, NODE_FLAGS_QUEUE_AQM);
		node->data.queue.aqm_context = PP_QOS_AQM_MAX_SERVICE_FLOWS;

		/* Set max allowed to support AQM buffer */
		rc = _pp_qos_queue_conf_get(qdev,
					    sf_cfg->queue_id[queue_idx],
					    &conf);
		if (unlikely(rc)) {
			QOS_LOG_ERR("SF %u. queue %u conf get failed\n",
				    sf_id, sf_cfg->queue_id[queue_idx]);
			rc = -EINVAL;
			goto out;
		}

		conf.wred_max_allowed = WRED_MAX_ALLOWED_DEFAULT;
		rc = _pp_qos_queue_set(qdev,
				       sf_cfg->queue_id[queue_idx],
				       &conf, QOS_INVALID_RLM);
		if (unlikely(rc)) {
			QOS_LOG_ERR("SF %u. queue %u conf get failed\n",
				    sf_id, sf_cfg->queue_id[queue_idx]);
			rc = -EINVAL;
			goto out;
		}
	}

out:
	return rc;
}

s32 pp_qos_aqm_sf_remove(struct pp_qos_dev *qdev, u8 sf_id)
{
	s32 rc;
	u32 rlms[PP_QOS_AQM_CONTEXT_MAX_QUEUES];
	struct pp_qos_aqm_sf_config *sf_cfg;

	QOS_LOCK(qdev);
	PP_QOS_ENTER_FUNC()
	if (unlikely(!qos_device_ready(qdev))) {
		rc = -EINVAL;
		goto out;
	}

	if (unlikely(sf_id >= PP_QOS_AQM_MAX_SERVICE_FLOWS)) {
		QOS_LOG_ERR("SF %u is invalid (max %u)\n",
			    sf_id, PP_QOS_AQM_MAX_SERVICE_FLOWS);
		rc = -EINVAL;
		goto out;
	}

	if (!qdev->sf_entry[sf_id].enabled) {
		QOS_LOG_ERR("SF %u is not enabled\n", sf_id);
		rc = -EINVAL;
		goto out;
	}

	sf_cfg = &qdev->sf_entry[sf_id].sf_cfg;

	rc = __qos_aqm_rlms_dettach_get(qdev, sf_id, rlms, sf_cfg);
	if (unlikely(rc))
		goto out;

	create_aqm_sf_set_cmd(qdev, sf_id, false, sf_cfg, rlms);
	update_cmd_id(&qdev->drvcmds);

	rc = transmit_cmds(qdev);
	if (unlikely(rc)) {
		QOS_LOG_ERR("SF %u remove cmd failed\n", sf_id);
		rc = -EIO;
		goto out;
	}

	/* reset SF config in qos db */
	memset(&qdev->sf_entry[sf_id], 0, sizeof(*sf_cfg));
	qdev->sf_entry[sf_id].enabled = false;

out:
	QOS_UNLOCK(qdev);
	return rc;
}
EXPORT_SYMBOL(pp_qos_aqm_sf_remove);

/******************************************************************************/
/*                                 Schedulers                                 */
/******************************************************************************/
static s32 _pp_qos_sched_set(struct pp_qos_dev *qdev,
			     u32 id,
			     struct pp_qos_sched_conf *conf);

static s32 _pp_qos_sched_conf_get(struct pp_qos_dev *qdev,
				  u32 id,
				  struct pp_qos_sched_conf *conf);

static void node_sched_init(struct pp_qos_dev *qdev, struct qos_node *node)
{
	node_init(qdev, node, 1, 1, 1);
	node->type = TYPE_SCHED;
	node->data.sched.new_child_phy = QOS_UNMAPPED_PHY;
	node->data.sched.new_child_skipped = 0;
}

static s32 sched_cfg_valid(const struct pp_qos_dev *qdev,
			   const struct qos_node *node,
			   const struct qos_node *orig_node,
			   u32 prev_virt_parent_phy)
{
	if (!node_sched(node)) {
		QOS_LOG_ERR("Node is not a sched\n");
		return 0;
	}

	return node_cfg_valid(qdev, node, orig_node, prev_virt_parent_phy);
}

static u32 get_tree_depth(struct pp_qos_dev *qdev, u32 phy)
{
	u32 depth = 0;
	struct qos_node *node;

	while (QOS_PHY_VALID(phy)) {
		depth++;
		node = get_node_from_phy(qdev->nodes, phy);
		if (node_port(node))
			break;

		phy = node->child_prop.parent_phy;
	}

	return depth;
}

#define QOS_TREE_MAX_SCHEDS_DEPTH	(5)
static bool is_max_tree_depth(struct pp_qos_dev *qdev, u32 parent_phy)
{
	struct qos_node *node = get_node_from_phy(qdev->nodes, parent_phy);
	u32 parent_id;
	u32 rep_id;
	u32 depth;

	/* The check was already done on the original node */
	if (node_rep(node))
		return false;

	parent_id = get_id_from_phy(qdev->mapping, parent_phy);

	if (is_tree_replica_exists(qdev, parent_id)) {
		rep_id = get_rep_id_from_id(qdev->mapping, parent_id);

		if (QOS_ID_VALID(rep_id) &&
		    get_tree_depth(qdev,
				   get_phy_from_id(qdev->mapping, rep_id))
				   > QOS_TREE_MAX_SCHEDS_DEPTH)
			return true;
	} else {
		depth = get_tree_depth(qdev, parent_phy);
		if (depth > QOS_TREE_MAX_SCHEDS_DEPTH)
			return true;
	}

	return false;
}

static s32 check_sched_conf_validity(struct pp_qos_dev *qdev, u32 id,
				     const struct pp_qos_sched_conf *conf,
				     struct qos_node *node, u32 *modified)
{
	u32 phy, prev_virt_parent_phy_phy;
	struct qos_node *nodep;
	s32 rc;

	if (!QOS_ID_VALID(id)) {
		QOS_LOG_ERR("Illegal id %u\n", id);
		return -EINVAL;
	}

	/* Check if node id is valid */
	phy = get_phy_from_id(qdev->mapping, id);
	if (!QOS_PHY_VALID(phy)) {
		if (!QOS_PHY_UNMAPPED(phy)) {
			QOS_LOG_ERR("Id %u is not a node\n", id);
			return -EINVAL;
		}

		/* New sched which has id, but no phy was allocated for it */
		node_sched_init(qdev, node);
		QOS_BITS_SET(node->flags, NODE_FLAGS_USED);
		QOS_BITS_SET(*modified,
			     QOS_MODIFIED_NODE_TYPE |
			     QOS_MODIFIED_SHARED_GROUP_ID |
			     QOS_MODIFIED_BANDWIDTH_LIMIT |
			     QOS_MODIFIED_BW_WEIGHT |
			     QOS_MODIFIED_PARENT |
			     QOS_MODIFIED_ARBITRATION |
			     QOS_MODIFIED_BEST_EFFORT);

		nodep = NULL;
		prev_virt_parent_phy_phy = QOS_INVALID_PHY;
	} else {
		nodep = get_node_from_phy(qdev->nodes, phy);
		if (!node_sched(nodep)) {
			QOS_LOG_ERR("Id %u is not a sched\n", id);
			rc = -EINVAL;
		}
		memcpy(node, nodep, sizeof(struct qos_node));
		prev_virt_parent_phy_phy = get_virtual_parent_phy(qdev->nodes,
								  node);
	}

	rc = set_node_prop(qdev, node, &conf->common_prop,
			   &conf->sched_parent_prop, &conf->sched_child_prop,
			   modified);
	if (rc)
		return rc;

	if (unlikely(!sched_cfg_valid(qdev, node, nodep,
				      prev_virt_parent_phy_phy))) {
		QOS_LOG_ERR("Sched cfg is not valid (node %#x)\n",
			    get_phy_from_id(qdev->mapping, id));
		return -EINVAL;
	}

	return 0;
}

void pp_qos_sched_conf_set_default(struct pp_qos_sched_conf *conf)
{
	memset(conf, 0, sizeof(struct pp_qos_sched_conf));
	conf->common_prop.bandwidth_limit = QOS_NO_BANDWIDTH_LIMIT;
	conf->common_prop.shared_bw_group =
		QOS_NO_SHARED_BANDWIDTH_GROUP;
	conf->common_prop.max_burst = QOS_DEAFULT_MAX_BURST;
	conf->sched_parent_prop.arbitration = PP_QOS_ARBITRATION_WSP;
	conf->sched_child_prop.wrr_weight = 1;
}
EXPORT_SYMBOL(pp_qos_sched_conf_set_default);

s32 pp_qos_sched_allocate(struct pp_qos_dev *qdev, u32 *id)
{
	u16 _id;
	s32 rc = 0;

	QOS_LOG_API_DEBUG("sched > allocate\n");

	QOS_LOCK(qdev);
	PP_QOS_ENTER_FUNC();
	if (!qos_device_ready(qdev)) {
		rc = -EINVAL;
		goto out;
	}

	_id = pp_pool_get(qdev->ids);
	QOS_ASSERT(QOS_ID_VALID(_id), "got illegal id %u\n", _id);
	*id = _id;
	map_id_reserved(qdev->mapping, _id);
out:
	QOS_UNLOCK(qdev);
	return rc;
}
EXPORT_SYMBOL(pp_qos_sched_allocate);

s32 pp_qos_sched_remove(struct pp_qos_dev *qdev, u32 id)
{
	struct qos_node *node;
	bool node_configured;
	s32 rc;
	u32 rep_id;

	QOS_LOCK(qdev);
	PP_QOS_ENTER_FUNC();
	if (!qos_device_ready(qdev)) {
		rc = -EINVAL;
		goto out;
	}

	rc = is_node_configured(qdev, id, &node_configured);
	if (unlikely(rc))
		goto out;

	/* If node is not configured just release id */
	if (!node_configured) {
		release_id(qdev, id);
		goto out;
	}

	node = get_conform_node(qdev, id, node_sched);
	if (!node)  {
		rc = -EINVAL;
		goto out;
	}

	QOS_LOG_DEBUG("remove tree starting at phy %d\n",
		      get_phy_from_node(qdev->nodes, node));

	rep_id = get_rep_id_from_id(qdev->mapping, id);

	rc = tree_remove(qdev, get_phy_from_node(qdev->nodes, node));
	update_cmd_id(&qdev->drvcmds);
	rc = transmit_cmds(qdev);
	if (rc)
		goto out;

	if (!rc && QOS_ID_VALID(rep_id)) {
		rc = tree_remove(qdev, get_phy_from_id(qdev->mapping, rep_id));
		update_cmd_id(&qdev->drvcmds);
		rc = transmit_cmds(qdev);
		if (rc)
			goto out;
	}

	QOS_LOG_API_DEBUG("sched %u > remove\n", id);
out:
	QOS_UNLOCK(qdev);
	return rc;
}
EXPORT_SYMBOL(pp_qos_sched_remove);

static s32 _pp_qos_sched_set(struct pp_qos_dev *qdev,
			     u32 id,
			     struct pp_qos_sched_conf *conf)
{
	s32 rc;
	u32 phy, parent_phy;
	u32 prev_phy;
	struct qos_node *parent;
	struct qos_node node;
	struct qos_node *nodep;
	struct qos_node *oldnode;
	u32 modified;
	u32 src_port, dst_port;
	u32 num_nonblocked_queues;

	modified = 0;

	phy = get_phy_from_id(qdev->mapping, id);
	if (QOS_PHY_VALID(phy)) {
		nodep = get_node_from_phy(qdev->nodes, phy);
		parent_phy = get_phy_from_id(qdev->mapping,
					     conf->sched_child_prop.parent);
		src_port = get_port(qdev->nodes, phy);
		dst_port = get_port(qdev->nodes, parent_phy);

		if (src_port != dst_port) {
			get_num_nonblocked_queues(qdev, phy,
						  &num_nonblocked_queues);
			if (num_nonblocked_queues) {
				QOS_LOG_ERR("All Sched's %u queues must be "
					    "blocked during sched move"
					    "(%u => %u)\n",
					    id, src_port, dst_port);
				return -EINVAL;
			}
		}
	}

	conf->sched_child_prop.parent = _get_actual_parent(qdev,
		conf->sched_child_prop.parent, conf->sched_child_prop.priority);

	phy = get_phy_by_priority(qdev, conf->sched_child_prop.parent,
				  conf->sched_child_prop.priority);
	if (QOS_PHY_VALID(phy) &&
	    id != get_id_from_phy(qdev->mapping, phy)) {
		QOS_LOG_ERR("Prio %u is occupied by phy %u\n",
			    conf->sched_child_prop.priority, phy);
		rc = -EINVAL;
		return rc;
	}

	rc = check_sched_conf_validity(qdev, id, conf, &node, &modified);
	if (rc)
		return rc;

	/* make sure we have enough room for this sched + queue */
	if (QOS_BITS_IS_SET(modified, QOS_MODIFIED_PARENT)) {
		if (is_max_tree_depth(qdev,
				      get_phy_from_id(qdev->mapping,
					conf->sched_child_prop.parent))) {
			QOS_LOG_ERR("Can't add a sched. max levels reached\n");
			return -EINVAL;
		}

		parent = get_node_from_phy(qdev->nodes,
					   node.child_prop.parent_phy);
		if (unlikely(!parent)) {
			QOS_LOG_ERR("get_node_from_phy(%u) returned NULL\n",
				    node.child_prop.parent_phy);
			return 0;
		}

		phy = phy_alloc_by_parent(qdev,
					  parent,
					  conf->sched_child_prop.priority);
		if (unlikely(!QOS_PHY_VALID(phy)))
			return -EINVAL;

		/* nodep's parent_phy holds value of actual parent, node's */
		/* parent_phy holds phy of virtual parent                  */
		nodep = get_node_from_phy(qdev->nodes, phy);
		if (unlikely(!nodep)) {
			QOS_LOG_ERR("get_node_from_phy(%u) returned NULL\n",
				    phy);
			return 0;
		}

		node.child_prop.parent_phy = nodep->child_prop.parent_phy;
		memcpy(nodep, &node, sizeof(struct qos_node));

		/* If this is not a new sched - delete previous node */
		if (!QOS_BITS_IS_SET(modified, QOS_MODIFIED_NODE_TYPE)) {
			prev_phy = get_phy_from_id(qdev->mapping, id);
			add_suspend_port(qdev, get_port(qdev->nodes, prev_phy));
			dst_port = get_port(qdev->nodes, phy);
			create_move_cmd(qdev, phy, prev_phy, dst_port);
			oldnode = get_node_from_phy(qdev->nodes, prev_phy);
			if (unlikely(!oldnode)) {
				QOS_LOG_ERR("get_node_from_phy(%u) NULL\n",
					    prev_phy);
				return 0;
			}

			node_update_children(qdev, oldnode, phy);
			map_invalidate_id_phy(qdev->mapping, id);
			map_id_phy(qdev->mapping, id, phy);
			nodep->parent_prop.first_child_phy =
				oldnode->parent_prop.first_child_phy;
			nodep->parent_prop.num_of_children =
				oldnode->parent_prop.num_of_children;
			oldnode->parent_prop.first_child_phy = QOS_INVALID_PHY;
			oldnode->parent_prop.num_of_children = 0;
			node_phy_delete(qdev, prev_phy);
			phy = get_phy_from_id(qdev->mapping, id);
			nodep = get_node_from_phy(qdev->nodes, phy);
			if (unlikely(!nodep)) {
				QOS_LOG_ERR("node->phy(%u) returned NULL\n",
					    phy);
				return 0;
			}

			tree_update_predecessors(qdev, phy);
		} else {
			map_id_phy(qdev->mapping, id, phy);
		}

		parent = get_node_from_phy(qdev->nodes,
					   nodep->child_prop.parent_phy);
		if (unlikely(!parent)) {
			QOS_LOG_ERR("get_node_from_phy(%u) returned NULL\n",
				    nodep->child_prop.parent_phy);
			return 0;
		}
	} else {
		phy = get_phy_from_id(qdev->mapping, id);
		nodep = get_node_from_phy(qdev->nodes, phy);
		if (unlikely(!nodep)) {
			QOS_LOG_ERR("get_node_from_phy(%u) returned NULL\n",
				    phy);
			return 0;
		}

		parent = get_node_from_phy(qdev->nodes,
					   nodep->child_prop.parent_phy);
		if (unlikely(!parent)) {
			QOS_LOG_ERR("get_node_from_phy(%u) returned NULL\n",
				    nodep->child_prop.parent_phy);
			return 0;
		}

		/* Child of WSP changes priority i.e. position */
		if (parent->parent_prop.arbitration == PP_QOS_ARBITRATION_WSP &&
		    QOS_BITS_IS_SET(modified, QOS_MODIFIED_PRIORITY)) {
			update_children_position(qdev,
						 nodep,
						 parent,
						 &node);
		}  else {
			memcpy(nodep, &node, sizeof(struct qos_node));
		}
	}

	if (modified & (QOS_MODIFIED_BANDWIDTH_LIMIT |
			QOS_MODIFIED_SHARED_GROUP_ID))
		add_suspend_port(qdev, get_port(qdev->nodes, phy));

	if (modified != QOS_MODIFIED_PARENT)
		create_set_sched_cmd(qdev, conf, nodep, modified);

	if (QOS_BITS_IS_SET(modified, QOS_MODIFIED_PARENT) ||
	    QOS_BITS_IS_SET(modified, QOS_MODIFIED_BW_WEIGHT))
		update_internal_bandwidth(qdev, parent);

	/* Update sub tree only if scheduler is not new */
	if (QOS_BITS_IS_SET(modified, QOS_MODIFIED_ARBITRATION) &&
	    !QOS_BITS_IS_SET(modified, QOS_MODIFIED_NODE_TYPE)) {
		if (nodep->parent_prop.arbitration == PP_QOS_ARBITRATION_WSP) {
			/* Update children as WSP */
			set_children_wsp_weight(qdev, nodep);
		} else {
			/* Re-calculate weights upon arbitration change */
			update_internal_bandwidth(qdev, nodep);
		}
	}

	return 0;
}

s32 pp_qos_sched_set(struct pp_qos_dev *qdev, u32 id,
		     const struct pp_qos_sched_conf *conf)
{
	s32 rc;
	u32 phy;
	struct qos_node *node;
	struct pp_qos_sched_conf temp_conf;

	QOS_LOG_API_DEBUG("sched=%u bw=%u shared=%u max_burst=%u arb=%u be=%u"
			  " parent=%u priority=%u wrr_weight=%u > sched\n",
			  id, conf->common_prop.bandwidth_limit,
			  conf->common_prop.shared_bw_group,
			  conf->common_prop.max_burst,
			  conf->sched_parent_prop.arbitration,
			  conf->sched_parent_prop.best_effort_enable,
			  conf->sched_child_prop.parent,
			  conf->sched_child_prop.priority,
			  conf->sched_child_prop.wrr_weight);

	QOS_LOCK(qdev);
	PP_QOS_ENTER_FUNC();
	if (!qos_device_ready(qdev)) {
		rc = -EINVAL;
		goto out;
	}

	/* Verify user is not setting a replica parent */
	phy = get_phy_from_id(qdev->mapping, conf->sched_child_prop.parent);
	if (!QOS_PHY_VALID(phy))
		goto sched_set_cont;

	node = get_node_from_phy(qdev->nodes, phy);
	if (node_rep(node) || node_internal(node)) {
		QOS_LOG_ERR("Parent %u is not valid\n",
			    conf->sched_child_prop.parent);
		rc = -EINVAL;
		goto out;
	}

sched_set_cont:
	memcpy(&temp_conf, conf, sizeof(struct pp_qos_sched_conf));

	rc = _pp_qos_sched_set(qdev, id, &temp_conf);
	if (rc)
		goto out;

	_rep_sched_set(qdev, id, conf);

	update_cmd_id(&qdev->drvcmds);
	rc = transmit_cmds(qdev);

out:
	QOS_UNLOCK(qdev);
	return rc;
}
EXPORT_SYMBOL(pp_qos_sched_set);

static s32 _pp_qos_sched_conf_get(struct pp_qos_dev *qdev,
				  u32 id,
				  struct pp_qos_sched_conf *conf)
{
	const struct qos_node *node;

	if (!conf)
		return -EINVAL;

	node = get_conform_node(qdev, id, node_sched);
	if (!node)
		return -EINVAL;

	return get_node_prop(qdev,
			     node,
			     &conf->common_prop,
			     &conf->sched_parent_prop,
			     &conf->sched_child_prop);
}

s32 pp_qos_sched_conf_get(struct pp_qos_dev *qdev,
			  u32 id,
			  struct pp_qos_sched_conf *conf)
{
	bool node_configured;
	s32 rc;

	QOS_LOCK(qdev);
	PP_QOS_ENTER_FUNC();
	if (!qos_device_ready(qdev)) {
		rc = -EINVAL;
		goto out;
	}

	rc = is_node_configured(qdev, id, &node_configured);
	if (rc || !node_configured) {
		rc = -EINVAL;
		goto out;
	}

	rc = _pp_qos_sched_conf_get(qdev, id, conf);
	if (rc)
		goto out;

	rc = qos_get_user_parent(qdev,
				 conf->sched_child_prop.parent,
				 &conf->sched_child_prop.parent);

	QOS_LOG_GET_API_DEBUG("sched=%u bw=%u shared=%u max_burst%u arb=%u"
			      " be=%u parent=%u priority=%u wrr_weight=%u\n",
			      id, conf->common_prop.bandwidth_limit,
			      conf->common_prop.shared_bw_group,
			      conf->common_prop.max_burst,
			      conf->sched_parent_prop.arbitration,
			      conf->sched_parent_prop.best_effort_enable,
			      conf->sched_child_prop.parent,
			      conf->sched_child_prop.priority,
			      conf->sched_child_prop.wrr_weight);
out:
	QOS_UNLOCK(qdev);
	return rc;
}
EXPORT_SYMBOL(pp_qos_sched_conf_get);

s32 pp_qos_sched_info_get(struct pp_qos_dev *qdev,
			  u32 id,
			  struct pp_qos_sched_info *info)
{
	struct qos_node *node;
	bool node_configured;
	u32 phy;
	u32 rc;

	QOS_LOCK(qdev);
	PP_QOS_ENTER_FUNC();
	if (!qos_device_ready(qdev)) {
		rc = -EINVAL;
		goto out;
	}

	if (!info) {
		rc = -EINVAL;
		goto out;
	}

	rc = is_node_configured(qdev, id, &node_configured);
	if (rc || !node_configured) {
		rc = -EINVAL;
		goto out;
	}

	node = get_conform_node(qdev, id, node_sched);
	if (!node) {
		rc = -EINVAL;
		goto out;
	}
	phy = get_phy_from_node(qdev->nodes, node);
	info->port_id = get_port(qdev->nodes,
				 get_phy_from_node(qdev->nodes, node));
	get_user_queues(qdev, phy, NULL, 0, &info->num_of_queues);
	update_cmd_id(&qdev->drvcmds);
	rc = transmit_cmds(qdev);
out:
	QOS_UNLOCK(qdev);
	return rc;
}
EXPORT_SYMBOL(pp_qos_sched_info_get);

s32 pp_qos_sched_get_queues(struct pp_qos_dev *qdev,
			    u32 id,
			    u16 *queue_ids,
			    u32 size,
			    u32 *queues_num)
{
	const struct qos_node *node;
	bool node_configured;
	u32 phy;
	u32 rc;

	QOS_LOCK(qdev);
	PP_QOS_ENTER_FUNC();
	if (!qos_device_ready(qdev)) {
		rc = -EINVAL;
		goto out;
	}

	rc = is_node_configured(qdev, id, &node_configured);
	if (rc || !node_configured) {
		rc = -EINVAL;
		goto out;
	}

	node = get_conform_node(qdev, id, node_sched);
	if (!node) {
		rc = -EINVAL;
		goto out;
	}

	phy = get_phy_from_node(qdev->nodes, node);
	get_user_queues(qdev, phy, queue_ids, size, queues_num);
	update_cmd_id(&qdev->drvcmds);
	rc = transmit_cmds(qdev);
out:
	QOS_UNLOCK(qdev);
	return rc;
}
EXPORT_SYMBOL(pp_qos_sched_get_queues);

s32 pp_qos_get_fw_version(struct pp_qos_dev *qdev,
			  u32 *major,
			  u32 *minor,
			  u32 *build)
{
	s32 rc = 0;

	QOS_LOCK(qdev);
	PP_QOS_ENTER_FUNC();
	if (!qos_device_ready(qdev)) {
		rc = -EINVAL;
		goto out;
	}

	*major = qdev->fwver.major;
	*minor = qdev->fwver.minor;
	*build = qdev->fwver.build;
out:
	QOS_UNLOCK(qdev);
	return rc;
}
EXPORT_SYMBOL(pp_qos_get_fw_version);

/******************************************************************************/
/*                                 SHARED_LIMIT                               */
/******************************************************************************/
static s32 _pp_qos_shared_limit_group_get_members(struct pp_qos_dev *qdev,
						  u32 id,
						  u16 *members,
						  u32 size,
						  u32 *members_num);

s32 pp_qos_shared_limit_group_add(struct pp_qos_dev *qdev,
				  u32 max_burst,
				  u32 *id)
{
	u32 i;
	s32 rc;
	struct shared_bw_group *grp;

	QOS_LOCK(qdev);
	PP_QOS_ENTER_FUNC();
	if (!qos_device_ready(qdev)) {
		rc = -EINVAL;
		goto out;
	}

	if (max_burst > QOS_MAX_MAX_BURST) {
		QOS_LOG_ERR("max max_burst is %u\n", QOS_MAX_MAX_BURST);
		rc = -EINVAL;
		goto out;
	}

	grp = qdev->groups + 1;
	for (i = 1; i <= QOS_MAX_SHARED_BANDWIDTH_GROUP; ++grp, ++i) {
		if (!grp->used)
			break;
	}

	if (grp->used) {
		QOS_LOG_ERR("No free shared groups\n");
		rc = -EBUSY;
		goto out;
	}

	grp->used = 1;
	grp->max_burst = max_burst;
	*id = i;

	create_add_shared_group_cmd(qdev, i, max_burst);
	update_cmd_id(&qdev->drvcmds);
	rc = transmit_cmds(qdev);
out:
	QOS_UNLOCK(qdev);
	return rc;
}
EXPORT_SYMBOL(pp_qos_shared_limit_group_add);

s32 pp_qos_shared_limit_group_remove(struct pp_qos_dev *qdev, u32 id)
{
	s32 rc;
	u32 num;
	u16 tmp;

	QOS_LOCK(qdev);
	PP_QOS_ENTER_FUNC();
	if (!qos_device_ready(qdev)) {
		rc = -EINVAL;
		goto out;
	}

	if (QOS_BW_GRP_VALID(id) && qdev->groups[id].used) {
		_pp_qos_shared_limit_group_get_members(qdev, id,
						       &tmp, 1, &num);
		if (num) {
			QOS_LOG_ERR("Share group %u has members cant remove\n",
				    id);
			rc = -EBUSY;
			goto out;
		}

		qdev->groups[id].used = 0;
		create_remove_shared_group_cmd(qdev, id);
		update_cmd_id(&qdev->drvcmds);
		rc = transmit_cmds(qdev);
	} else {
		QOS_LOG_ERR("Shared group %u is not used\n", id);
		rc = -EINVAL;
	}
out:
	QOS_UNLOCK(qdev);
	return rc;
}
EXPORT_SYMBOL(pp_qos_shared_limit_group_remove);

s32 pp_qos_shared_limit_group_modify(struct pp_qos_dev *qdev,
				     u32 id, u32 max_burst)
{
	s32 rc = 0;

	if (max_burst > QOS_MAX_MAX_BURST) {
		QOS_LOG_ERR("max max_burst is %u\n", QOS_MAX_MAX_BURST);
		return -EINVAL;
	}

	QOS_LOCK(qdev);
	PP_QOS_ENTER_FUNC();

	if (QOS_BW_GRP_VALID(id) && qdev->groups[id].used) {
		if (qdev->groups[id].max_burst != max_burst) {
			create_set_shared_group_cmd(qdev, id, max_burst);
			update_cmd_id(&qdev->drvcmds);
			rc = transmit_cmds(qdev);
		}
	} else {
		QOS_LOG_ERR("Shared group %u is not used\n", id);
		rc = -EINVAL;
	}

	QOS_UNLOCK(qdev);
	return rc;
}
EXPORT_SYMBOL(pp_qos_shared_limit_group_modify);

static s32 _pp_qos_shared_limit_group_get_members(struct pp_qos_dev *qdev,
						  u32 id, u16 *members,
						  u32 size, u32 *members_num)
{
	u32 phy;
	u32 total;
	u32 tmp;
	u32 max_ports;
	const struct qos_node *node;

	total = 0;
	max_ports = qdev->init_params.max_ports;
	for (phy = 0; (phy < max_ports) && (size > 0); ++phy) {
		node = get_const_node_from_phy(qdev->nodes, phy);
		if (node_used(node)) {
			get_bw_grp_members_under_node(qdev, id, phy,
						      members, size, &tmp);
			total += tmp;
			members += tmp;
			if (tmp < size)
				size -= tmp;
			else
				size = 0;
		}
	}

	*members_num = total;
	return 0;
}

s32 pp_qos_shared_limit_group_get_members(struct pp_qos_dev *qdev,
					  u32 id, u16 *members,
					  u32 size, u32 *members_num)
{
	s32 rc;

	QOS_LOCK(qdev);
	PP_QOS_ENTER_FUNC();
	if (!qos_device_ready(qdev)) {
		rc = -EINVAL;
		goto out;
	}
	rc = _pp_qos_shared_limit_group_get_members(qdev, id, members,
						    size, members_num);
	if (rc)
		goto out;

	update_cmd_id(&qdev->drvcmds);
	rc = transmit_cmds(qdev);
out:
	QOS_UNLOCK(qdev);
	return rc;
}
EXPORT_SYMBOL(pp_qos_shared_limit_group_get_members);

s32 __qos_ddr_allocate(struct pp_qos_dev *qdev)
{
	struct hw_conf *cfg = &qdev->hwconf;

	/* Allocate queue manager descriptor buffer */
	cfg->qm_sz = PAGE_ALIGN(qdev->init_params.wred_total_avail_resources *
				PPV4_QOS_DESC_SIZE);

	cfg->qm_ddr_virt =
		pp_dma_alloc(cfg->qm_sz, GFP_KERNEL, &cfg->qm_ddr_phys, false);
	if (unlikely(!cfg->qm_ddr_virt)) {
		pr_err("Could not allocate %u bytes for queue manager\n",
		       (u32)cfg->qm_sz);
		goto qm_alloc_err;
	}

	cfg->qm_num_pages =
		qdev->init_params.wred_total_avail_resources /
		PPV4_QOS_DESC_IN_PAGE;

	/* Allocate FW Logger buffer */
	cfg->fw_logger_sz = PAGE_ALIGN(PPV4_QOS_LOGGER_BUF_SIZE);
	cfg->fw_logger_ddr_virt = pp_dma_alloc(cfg->fw_logger_sz, GFP_KERNEL,
					       &cfg->fw_logger_ddr_phys, true);
	if (unlikely(!cfg->fw_logger_ddr_virt)) {
		pr_err("Could not allocate %u bytes for logger\n",
		       (u32)cfg->fw_logger_sz);
		goto logger_alloc_err;
	}

	/* Allocate FW Statistics buffer */
	cfg->fw_stats_sz = PAGE_ALIGN(PPV4_QOS_STAT_SIZE);
	cfg->fw_stats_ddr_virt = pp_dma_alloc(cfg->fw_stats_sz, GFP_KERNEL,
					      &cfg->fw_stats_ddr_phys, true);
	if (unlikely(!cfg->fw_stats_ddr_virt)) {
		pr_err("Could not allocate %u bytes for stats buffer\n",
		       (u32)cfg->fw_stats_sz);
		goto stats_alloc_err;
	}

	/* Allocate max allowed buffer */
	cfg->max_allow_sz =
		PAGE_ALIGN(qdev->init_params.max_queues * sizeof(u32));
	cfg->max_allow_ddr_virt = pp_dma_alloc(cfg->max_allow_sz, GFP_KERNEL,
					       &cfg->max_allow_ddr_phys, true);
	if (unlikely(!cfg->max_allow_ddr_virt)) {
		pr_err("Could not allocate %u bytes for max allowed buffer\n",
		       (u32)cfg->max_allow_sz);
		goto max_allowed_alloc_err;
	}

	/* Allocate wsp queue list buffer + num queues placeholder */
	cfg->wsp_queues_sz = PAGE_ALIGN((QOS_ENHANCED_WSP_MAX_QUEUES *
					sizeof(struct wsp_queue_t)) + 4);
	cfg->wsp_queues_ddr_virt = pp_dma_alloc(cfg->wsp_queues_sz, GFP_KERNEL,
						&cfg->wsp_queues_ddr_phys,
						true);
	if (unlikely(!cfg->wsp_queues_ddr_virt)) {
		pr_err("Could not allocate %u bytes for wsp list buffer\n",
		       (u32)cfg->wsp_queues_sz);
		goto wsp_alloc_err;
	}

	cfg->api_str_buf = kzalloc(QOS_LOG_API_STR_MAX, GFP_KERNEL);
	if (unlikely(!cfg->api_str_buf)) {
		pr_err("Could not allocate %u bytes for api string buffer\n",
		       QOS_LOG_API_STR_MAX);
		goto api_str_alloc_err;
	}

	qdev->hwconf.rlms_container = kcalloc(QOS_MAX_NUM_QUEUES, sizeof(u16),
					      GFP_KERNEL);
	if (unlikely(!qdev->hwconf.rlms_container)) {
		pr_err("Could not allocate %u bytes for rlms_container buffer\n",
		       (u32)(QOS_MAX_NUM_QUEUES * sizeof(u16)));
		goto rlm_container_alloc_err;
	}

	/* Allocate BWL allowed buffer.
	 * Buffer contains bw(u32) + max burst(u32) per node
	 */
	cfg->bwl_sz = PAGE_ALIGN(2 * NUM_OF_NODES * sizeof(u32));
	cfg->bwl_ddr_virt = pp_dma_alloc(cfg->bwl_sz, GFP_KERNEL,
					 &cfg->bwl_ddr_phys, true);
	if (unlikely(!cfg->bwl_ddr_virt)) {
		pr_err("Could not allocate %u bytes for bwl buffer\n",
		       (u32)cfg->bwl_sz);
		goto bwl_alloc_err;
	}

	/* Allocate SBWL allowed buffer */
	cfg->sbwl_sz = PAGE_ALIGN(NUM_OF_NODES * sizeof(u32));
	cfg->sbwl_ddr_virt = pp_dma_alloc(cfg->sbwl_sz, GFP_KERNEL,
					  &cfg->sbwl_ddr_phys, true);
	if (unlikely(!cfg->sbwl_ddr_virt)) {
		pr_err("Could not allocate %u bytes for sbwl buffer\n",
		       (u32)cfg->sbwl_sz);
		goto sbwl_alloc_err;
	}

	/* Allocate buffer for dma usage */
	cfg->dma_buff_sz = PAGE_ALIGN(PPV4_QOS_DMA_BUFF_SIZE);
	cfg->dma_buff_virt = pp_dma_alloc(cfg->dma_buff_sz, GFP_KERNEL,
					  &cfg->dma_buff_phys, true);
	if (unlikely(!cfg->dma_buff_virt)) {
		pr_err("Could not allocate %u bytes for dma buffer\n",
		       (u32)cfg->dma_buff_sz);
		goto dma_buff_err;
	}

	return 0;

dma_buff_err:
	pp_dma_free(cfg->sbwl_sz, cfg->sbwl_ddr_virt, &cfg->sbwl_ddr_phys,
		    true);

sbwl_alloc_err:
	pp_dma_free(cfg->bwl_sz, cfg->bwl_ddr_virt, &cfg->bwl_ddr_phys, true);

bwl_alloc_err:
	kfree(qdev->hwconf.rlms_container);

rlm_container_alloc_err:
	kfree(cfg->api_str_buf);

api_str_alloc_err:
	/* Free wsp queue list buffer */
	pp_dma_free(cfg->wsp_queues_sz,
		    cfg->wsp_queues_ddr_virt,
		    &cfg->wsp_queues_ddr_phys, true);

wsp_alloc_err:
	/* Free max allowed buffer */
	pp_dma_free(cfg->max_allow_sz,
		    cfg->max_allow_ddr_virt,
		    &cfg->max_allow_ddr_phys, true);

max_allowed_alloc_err:
	/* Free stats buffer */
	pp_dma_free(cfg->fw_stats_sz,
		    cfg->fw_stats_ddr_virt,
		    &cfg->fw_stats_ddr_phys, true);

stats_alloc_err:
	/* Free Logger */
	pp_dma_free(cfg->fw_logger_sz,
		    cfg->fw_logger_ddr_virt,
		    &cfg->fw_logger_ddr_phys, true);
logger_alloc_err:
	/* Free QM */
	pp_dma_free(cfg->qm_sz,
		    cfg->qm_ddr_virt,
		    &cfg->qm_ddr_phys, false);
qm_alloc_err:
	return -ENOMEM;
}

void __qos_ddr_free(struct pp_qos_dev *qdev)
{
	/* Free queue manager descriptor buffer */
	if (qdev->hwconf.qm_ddr_virt) {
		pp_dma_free(qdev->hwconf.qm_sz,
			    qdev->hwconf.qm_ddr_virt,
			    &qdev->hwconf.qm_ddr_phys, false);
	}

	/* Free FW Logger buffer */
	if (qdev->hwconf.fw_logger_ddr_virt) {
		pp_dma_free(qdev->hwconf.fw_logger_sz,
			    qdev->hwconf.fw_logger_ddr_virt,
			    &qdev->hwconf.fw_logger_ddr_phys, true);
	}

	/* Free FW Statistics buffer */
	if (qdev->hwconf.fw_stats_ddr_virt) {
		pp_dma_free(qdev->hwconf.fw_stats_sz,
			    qdev->hwconf.fw_stats_ddr_virt,
			    &qdev->hwconf.fw_stats_ddr_phys, true);
	}

	/* Free wsp queue list buffer */
	if (qdev->hwconf.wsp_queues_ddr_virt) {
		pp_dma_free(qdev->hwconf.wsp_queues_sz,
			    qdev->hwconf.wsp_queues_ddr_virt,
			    &qdev->hwconf.wsp_queues_ddr_phys, true);
	}

	/* Free max allow buffer */
	if (qdev->hwconf.max_allow_ddr_virt) {
		pp_dma_free(qdev->hwconf.max_allow_sz,
			    qdev->hwconf.max_allow_ddr_virt,
			    &qdev->hwconf.max_allow_ddr_phys, true);
	}

	/* Free BWL buffer */
	if (qdev->hwconf.bwl_ddr_virt) {
		pp_dma_free(qdev->hwconf.bwl_sz,
			    qdev->hwconf.bwl_ddr_virt,
			    &qdev->hwconf.bwl_ddr_phys, true);
	}

	/* Free SBWL buffer */
	if (qdev->hwconf.sbwl_ddr_virt) {
		pp_dma_free(qdev->hwconf.sbwl_sz,
			    qdev->hwconf.sbwl_ddr_virt,
			    &qdev->hwconf.sbwl_ddr_phys, true);
	}

	if (qdev->hwconf.dma_buff_virt) {
		pp_dma_free(qdev->hwconf.dma_buff_sz,
			    qdev->hwconf.dma_buff_virt,
			    &qdev->hwconf.dma_buff_phys, true);
	}

	/* Free API string buffer */
	kfree(qdev->hwconf.api_str_buf);

	/* Free rlm container buffer */
	kfree(qdev->hwconf.rlms_container);
}

s32 pp_qos_stats_get(struct pp_qos_dev *qdev, struct pp_qos_stats *stats)
{
	bool reset_stats = false;
	s32 rc;

	QOS_LOCK(qdev);
	PP_QOS_ENTER_FUNC();
	if (!qos_device_ready(qdev)) {
		rc = -EINVAL;
		goto out;
	}

	if (unlikely(!stats)) {
		pr_err("Invalid stats parameter\n");
		rc = -EINVAL;
		goto out;
	}

	memset(stats, 0, sizeof(struct pp_qos_stats));
	rc = __qos_stats_get(qdev, stats, reset_stats);

out:
	QOS_UNLOCK(qdev);
	return rc;
}
EXPORT_SYMBOL(pp_qos_stats_get);

s32 pp_qos_stats_reset(struct pp_qos_dev *qdev)
{
	s32 rc;
	bool reset_stats = true;

	QOS_LOCK(qdev);
	PP_QOS_ENTER_FUNC();
	if (!qos_device_ready(qdev)) {
		rc = -EINVAL;
		goto out;
	}

	rc = __qos_stats_get(qdev, NULL, reset_stats);

out:
	QOS_UNLOCK(qdev);
	return rc;
}
EXPORT_SYMBOL(pp_qos_stats_reset);

struct pp_qos_dev *pp_qos_dev_open(u32 id)
{
	if (id >= PP_QOS_MAX_INSTANCES) {
		QOS_LOG_ERR("Illegal qos instance %u\n", id);
		return NULL;
	}

	spin_lock(&drv_lock);

	/* Allocate QoS Instance */
	if (!qos_devs[id]) {
		QOS_LOG_DEBUG("Allocate new QoS instance id %d\n", id);
		qos_devs[id] = kzalloc(sizeof(*qos_devs[id]), GFP_ATOMIC);
		qos_devs[id]->id = id;
	}

	spin_unlock(&drv_lock);

	return qos_devs[id];
}
EXPORT_SYMBOL(pp_qos_dev_open);

s32 pp_qos_get_quanta(struct pp_qos_dev *qdev, u32 *quanta)
{
	s32 rc = 0;

	QOS_LOCK(qdev);
	PP_QOS_ENTER_FUNC();

	if (!qos_device_ready(qdev)) {
		rc = -EINVAL;
		goto out;
	}

	create_get_quanta_cmd(qdev, quanta);
	update_cmd_id(&qdev->drvcmds);
	rc = transmit_cmds(qdev);

out:
	QOS_UNLOCK(qdev);
	return rc;
}
EXPORT_SYMBOL(pp_qos_get_quanta);

s32 qos_dev_init(struct pp_qos_dev *qdev,
		 struct pp_qos_init_param *conf,
		 bool create_debugfs)
{
	s32 rc;
	u32 num_reserved_ports = 0;

	qos_wred_base_addr = conf->qos_wred_base;

	rc = qos_dev_init_db(qdev, conf->max_ports, conf->max_queues,
			     conf->reserved_queues, conf->num_syncqs);
	if (unlikely(rc))
		goto err;

	/* Store init parameters */
	memcpy(&qdev->init_params,
	       conf,
	       sizeof(struct pp_qos_init_param));

	rc = qos_init_wred_hw(qdev);
	if (unlikely(rc))
		goto err;

	/* Calculate maximum bw limit */
	qdev->hwconf.max_bw_limit_kbps =
		QOS_MAX_BW_KBPS_PER_1MHZ_CLK * qdev->init_params.hw_clk;

	rc = init_fwdata_internals(qdev);
	if (unlikely(rc))
		goto err;

	QOS_LOG_DEBUG("Initialized qos instance\nmax_ports:\t\t%u\n",
		      qdev->init_params.max_ports);

	QOS_LOG_DEBUG("cmdbuf:\t\t0x%08X\ncmdbuf size:\t\t%zu\n",
		      (u32)qdev->init_params.fwcom.cmdbuf,
		      qdev->init_params.fwcom.cmdbuf_sz);

#ifdef CONFIG_PPV4_LGM
	rc = qos_sysfs_init(qdev);
	if (unlikely(rc)) {
		QOS_LOG_ERR("Failed to init sysfs stuff\n");
		goto err;
	}
#endif

	if (create_debugfs) {
		rc = qos_dbg_module_init(qdev);
		if (unlikely(rc)) {
			QOS_LOG_ERR("qos_dbg_module_init failed (%d)\n", rc);
			goto err;
		}
	}

	/* Reserve Ports */
	qdev->portsphys =
		free_ports_phys_init(qdev->init_params.reserved_ports,
				     qdev->init_params.max_ports,
				     conf->reserved_ports,
				     PP_QOS_MAX_PORTS,
				     &num_reserved_ports);

	if ((num_reserved_ports != qdev->init_params.max_ports) &&
	    unlikely(!qdev->portsphys)) {
		QOS_LOG_ERR("portsphys alloc failed\n");
		rc = -ENOMEM;
		goto err;
	}

	qdev->init_params.wred_total_avail_resources =
		conf->wred_total_avail_resources;

	rc = __qos_ddr_allocate(qdev);
	if (unlikely(rc)) {
		QOS_LOG_ERR("Could not allocate %u bytes for queue manager\n",
			    qdev->init_params.wred_total_avail_resources *
			    PPV4_QOS_DESC_SIZE);
		goto err;
	}

	QOS_LOG_DEBUG("wred total resources\t%u\n",
		      qdev->init_params.wred_total_avail_resources);
	QOS_LOG_DEBUG("qm_ddr_phys\t\t%#lx\n", (ulong)qdev->hwconf.qm_ddr_phys);
	QOS_LOG_DEBUG("qm_num_of_pages\t%u\n", qdev->hwconf.qm_num_pages);
	QOS_LOG_DEBUG("wred p const\t\t%u\n", qdev->init_params.wred_p_const);
	QOS_LOG_DEBUG("wred max q size\t%u\n",
		      qdev->init_params.wred_max_q_size);

#ifndef PP_QOS_DISABLE_CMDQ
	if (is_uc_awake(qdev->init_params.wakeuc)) {
		rc = qos_fw_start(qdev);
		if (rc)
			goto err;
	}
#endif

	return 0;
err:
	qos_dev_clear_db(qdev);
	return rc;
}

s32 pp_qos_dev_init(struct pp_qos_dev *qdev, struct pp_qos_init_param *conf)
{
	s32 rc;

	if (!qdev) {
		QOS_LOG_ERR("Null Device\n");
		return -EINVAL;
	}

	if (!conf->valid)
		return -EINVAL;

	QOS_LOG_DEBUG("Init QoS Device %d\n", qdev->id);

	if (conf->max_ports & 7) {
		QOS_LOG_ERR("Given max port %u is not last node of an octet\n",
			    qdev->init_params.max_ports);
		return -EINVAL;
	}

	if (conf->wred_p_const > 1023) {
		QOS_LOG_ERR("wred_p_const should be not greater than 1023\n");
		return -EINVAL;
	}

	if (!IS_ALIGNED(qdev->init_params.wred_total_avail_resources,
			PPV4_QOS_DESC_IN_PAGE)) {
		QOS_LOG_ERR("Num resources %u must be aligned to %u\n",
			    qdev->init_params.wred_total_avail_resources,
			    PPV4_QOS_DESC_IN_PAGE);
		return -EINVAL;
	}

	PP_QOS_ENTER_FUNC();

	if (qdev->initialized) {
		QOS_LOG_ERR("Dev already initialized, can't init again\n");
		rc = -EINVAL;
		goto out;
	}

	rc = qos_dev_init(qdev, conf, true);
	if (rc) {
		QOS_LOG_ERR("__qos_dev_init failed\n");
		goto out;
	}

out:
	QOS_LOG_DEBUG("Init QoS Device %d Done with %d\n", qdev->id, rc);
	return rc;
}

void qos_dev_clean(struct pp_qos_dev *qos_dev, bool delete_debugfs)
{
	if (unlikely(!qos_dev))
		return;

	delete_all_syncq(qos_dev);
	qos_init_wred_hw(qos_dev);

	__qos_ddr_free(qos_dev);

	if (delete_debugfs)
		qos_dbg_module_clean(qos_dev);

#ifdef CONFIG_PPV4_LGM
	qos_sysfs_clean(qos_dev);
#endif

	qos_dev_clear_db(qos_dev);
	qos_devs[qos_dev->id] = NULL;

	kfree(qos_dev);
}

void pp_qos_dev_clean(struct pp_qos_dev *qos_dev)
{
	if (!qos_dev) {
		QOS_LOG_ERR("qos device NULL\n");
		return;
	}

	QOS_LOG_DEBUG("Removing qos instance %u\n", qos_dev->id);

	spin_lock(&drv_lock);
	qos_dev_clean(qos_dev, true);
	spin_unlock(&drv_lock);
}

s32 pp_qos_config_get(struct pp_qos_dev *qos_dev,
		      struct pp_qos_init_param *conf)
{
	if (!qos_dev) {
		QOS_LOG_ERR("qos device NULL\n");
		return -EINVAL;
	}

	if (!qos_dev->initialized) {
		QOS_LOG_ERR("Device not initialized\n");
		return -EINVAL;
	}

	memcpy(conf, &qos_dev->init_params, sizeof(struct pp_qos_init_param));
	return 0;
}

void pp_qos_mgr_init(void)
{
	u32 i;

	spin_lock_init(&drv_lock);

	for (i = 0 ; i < PP_QOS_MAX_INSTANCES; ++i)
		qos_devs[i] = NULL;

	QOS_LOG_DEBUG("qos_module_init completed\n");
}

void pp_qos_mgr_exit(void)
{
	u32 id;

	QOS_LOG_DEBUG("start\n");

	spin_lock(&drv_lock);
	for (id = 0 ; id < PP_QOS_MAX_INSTANCES; ++id) {
		qos_dev_clean(qos_devs[id], true);
		qos_devs[id] = NULL;
	}
	spin_unlock(&drv_lock);

	QOS_LOG_DEBUG("done\n");
}
