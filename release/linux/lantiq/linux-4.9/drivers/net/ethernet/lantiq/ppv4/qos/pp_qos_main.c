/*
 * GPL LICENSE SUMMARY
 *
 *  Copyright(c) 2017-2019 Intel Corporation.
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
#include "pp_qos_common.h"
#include "pp_qos_utils.h"
#include "pp_qos_fw.h"
#include "pp_qos_drv.h"

#define PP_QOS_ENTER_FUNC()
#define PP_QOS_EXIT_FUNC()

static bool qos_fw_log;

void update_cmd_id(struct driver_cmds *drvcmds)
{
	drvcmds->cmd_id++;
	drvcmds->cmd_fw_id = 0;
}

int is_ongoing(struct pp_qos_dev *qdev);

void transmit_cmds(struct pp_qos_dev *qdev)
{
	/* Handle commands in case there are commands in queue or in case
	   ports were suspended and need to be resumed again */
	while (!cmd_queue_is_empty(qdev->drvcmds.cmdq) || (is_ongoing(qdev))) {
		enqueue_cmds(qdev);
		check_completion(qdev);
	}
}

static struct pp_qos_dev *qos_devs[MAX_QOS_INSTANCES];

/******************************************************************************/
/*                                      PORTS                                 */
/******************************************************************************/
static int set_port_specific_prop(struct pp_qos_dev *qdev,
				  struct qos_node *node,
				  const struct pp_qos_port_conf *conf,
				  uint32_t *modified)
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
				QOS_NODE_FLAGS_PORT_PACKET_CREDIT_ENABLE)) {
		QOS_BITS_TOGGLE(node->flags,
				QOS_NODE_FLAGS_PORT_PACKET_CREDIT_ENABLE);
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

	return 0;
}

STATIC_UNLESS_TEST
int port_cfg_valid(
		const struct pp_qos_dev *qdev,
		const struct qos_node *node,
		const struct qos_node *orig_node)
{
	if (!node_port(node)) {
		QOS_LOG_ERR("Node is not a port\n");
		return 0;
	}

	if (node->data.port.ring_address == NULL ||
			node->data.port.ring_size == 0) {
		QOS_LOG_ERR("Invalid ring address %p or ring size %zu\n",
				node->data.port.ring_address,
				node->data.port.ring_size);
		return 0;
	}

	return node_cfg_valid(qdev, node, orig_node, QOS_INVALID_PHY);
}

static
struct qos_node *get_conform_node(const struct pp_qos_dev *qdev,
		unsigned int id,
		int (*conform)(const struct qos_node *node))
{
	struct qos_node *node;
	unsigned int phy;

	if (!QOS_ID_VALID(id)) {
		QOS_LOG_ERR("Illegal id %u\n", id);
		return NULL;
	}

	phy = get_phy_from_id(qdev->mapping, id);
	if (!QOS_PHY_VALID(phy)) {
		QOS_LOG_ERR("Illegal phy (id %u)\n", id);
		return NULL;
	}

	node = get_node_from_phy(qdev->nodes, phy);
	if (conform && !conform(node)) {
		QOS_LOG_ERR("Illegal node (id %u)\n", id);
		return NULL;
	}

	return node;
}

static int is_node_configured(const struct pp_qos_dev *qdev,
			      unsigned int id, bool *configured)
{
	unsigned int phy;

	if (!QOS_ID_VALID(id)) {
		QOS_LOG_ERR("Illegal id %u\n", id);
		return -EINVAL;
	}

	phy = get_phy_from_id(qdev->mapping, id);
	if (QOS_PHY_UNKNOWN(phy)) {
		*configured = false;
	} else if (QOS_PHY_VALID(phy)) {
		*configured = true;
	} else {
		QOS_LOG_ERR("Illegal phy %u\n", phy);
		return -EINVAL;
	}

	return 0;
}
/******************************************************************************/
/*                                 API                                        */
/******************************************************************************/
static int _pp_qos_port_set(
		struct pp_qos_dev *qdev,
		unsigned int id,
		const struct pp_qos_port_conf *conf);
static int
_pp_qos_port_conf_get(
		struct pp_qos_dev *qdev,
		unsigned int id,
		struct pp_qos_port_conf *conf);

void pp_qos_port_conf_set_default(struct pp_qos_port_conf *conf)
{
	memset(conf, 0, sizeof(struct pp_qos_port_conf));
	conf->common_prop.bandwidth_limit = QOS_NO_BANDWIDTH_LIMIT;
	conf->common_prop.shared_bandwidth_group =
		QOS_NO_SHARED_BANDWIDTH_GROUP;
	conf->port_parent_prop.arbitration = PP_QOS_ARBITRATION_WSP;
	conf->packet_credit_enable = 1;
	conf->green_threshold = 0xFFFFFFFF;
	conf->yellow_threshold = 0xFFFFFFFF;
}

int pp_qos_port_allocate(
		struct pp_qos_dev *qdev,
		unsigned int physical_id,
		unsigned int *_id)
{
	unsigned int phy;
	int rc;
	unsigned int id;

	QOS_LOCK(qdev);
	PP_QOS_ENTER_FUNC();
	if (!qos_device_ready(qdev)) {
		rc = -EINVAL;
		goto out;
	}

	QOS_LOG_API_DEBUG("port %u > allocate\n", physical_id);

	if (physical_id == ALLOC_PORT_ID) {
		phy = pp_pool_get(qdev->portsphys);
		if (!QOS_PHY_VALID(phy)) {
			rc = -EBUSY;
			goto out;
		}
	} else if (physical_id > qdev->max_port ||
			!qdev->reserved_ports[physical_id]) {
		QOS_LOG(
				"requested physical id %u is not reserved or out of range\n",
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

	node_init(qdev, get_node_from_phy(qdev->nodes, phy), 1, 1, 0);
	nodes_modify_used_status(qdev, phy, 1, 1);

	id = pp_pool_get(qdev->ids);
	QOS_ASSERT(QOS_ID_VALID(id), "got illegal id %u\n", id);
	map_id_phy(qdev->mapping, id, phy);
	*_id = id;
	rc = 0;
out:
	QOS_UNLOCK(qdev);
	return rc;
}

int pp_qos_port_disable(struct pp_qos_dev *qdev, unsigned int id)
{
	int rc;
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
	update_cmd_id(&qdev->drvcmds);
	transmit_cmds(qdev);
out:
	QOS_UNLOCK(qdev);
	return rc;

}

int pp_qos_port_enable(struct pp_qos_dev *qdev, unsigned int id)
{
	int rc;
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
	update_cmd_id(&qdev->drvcmds);
	transmit_cmds(qdev);
out:
	QOS_UNLOCK(qdev);
	return rc;

}

int pp_qos_port_block(struct pp_qos_dev *qdev, unsigned int id)
{
	const struct qos_node *node;
	bool node_configured;
	int rc;

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
	rc = tree_modify_blocked_status(
			qdev,
			get_phy_from_id(qdev->mapping, id), 1);
	update_cmd_id(&qdev->drvcmds);
	transmit_cmds(qdev);
out:
	QOS_UNLOCK(qdev);
	return rc;

}

int pp_qos_port_unblock(struct pp_qos_dev *qdev, unsigned int id)
{
	const struct qos_node *node;
	bool node_configured;
	int rc;

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
	rc = tree_modify_blocked_status(
			qdev,
			get_phy_from_id(qdev->mapping, id), 0);
	update_cmd_id(&qdev->drvcmds);
	transmit_cmds(qdev);
out:
	QOS_UNLOCK(qdev);
	return rc;

}

int pp_qos_port_flush(struct pp_qos_dev *qdev, unsigned int id)
{
	const struct qos_node *node;
	bool node_configured;
	int rc;

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
	rc = tree_flush(qdev, get_phy_from_id(qdev->mapping, id));
	update_cmd_id(&qdev->drvcmds);
	transmit_cmds(qdev);
out:
	QOS_UNLOCK(qdev);
	return rc;
}

int pp_qos_port_remove(struct pp_qos_dev *qdev, unsigned int id)
{
	const struct qos_node *node;
	bool node_configured;
	unsigned int phy;
	int rc;

	QOS_LOCK(qdev);
	PP_QOS_ENTER_FUNC();
	if (!qos_device_ready(qdev)) {
		rc = -EINVAL;
		goto out;
	}

	rc = is_node_configured(qdev, id, &node_configured);
	if (rc)
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
	update_cmd_id(&qdev->drvcmds);
	transmit_cmds(qdev);

	QOS_LOG_API_DEBUG("port %u > remove\n", id);
out:
	QOS_UNLOCK(qdev);
	return rc;
}

static int _pp_qos_port_conf_get(
		struct pp_qos_dev *qdev,
		unsigned int id,
		struct pp_qos_port_conf *conf)
{
	const struct qos_node *node;
	bool node_configured;
	int rc;

	if (conf == NULL)
		return -EINVAL;

	rc = is_node_configured(qdev, id, &node_configured);
	if (rc || !node_configured)
		return -EINVAL;

	node = get_conform_node(qdev, id, node_port);
	if (!node)
		return -EINVAL;

	conf->ring_address = node->data.port.ring_address;
	conf->ring_size = node->data.port.ring_size;
	conf->packet_credit_enable = !!QOS_BITS_IS_SET(node->flags,
			QOS_NODE_FLAGS_PORT_PACKET_CREDIT_ENABLE);
	conf->credit = node->data.port.credit;
	conf->disable = node->data.port.disable;
	conf->green_threshold = node->data.port.green_threshold;
	conf->yellow_threshold = node->data.port.yellow_threshold;

	return  get_node_prop(
			qdev,
			node,
			&conf->common_prop,
			&conf->port_parent_prop,
			NULL);
}

int pp_qos_port_conf_get(
		struct pp_qos_dev *qdev,
		unsigned int id,
		struct pp_qos_port_conf *conf)
{
	int rc;

	QOS_LOCK(qdev);
	PP_QOS_ENTER_FUNC();
	if (!qos_device_ready(qdev)) {
		rc = -EINVAL;
		goto out;
	}

	rc = _pp_qos_port_conf_get(qdev, id, conf);
	update_cmd_id(&qdev->drvcmds);
	transmit_cmds(qdev);
out:
	QOS_UNLOCK(qdev);
	return rc;
}

int pp_qos_port_info_get(
		struct pp_qos_dev *qdev,
		unsigned int id,
		struct pp_qos_port_info *info)
{
	const struct qos_node *node;
	bool node_configured;
	unsigned int phy;
	int rc;

	QOS_LOCK(qdev);
	PP_QOS_ENTER_FUNC();
	if (!qos_device_ready(qdev)) {
		rc = -EINVAL;
		goto out;
	}

	if (info == NULL) {
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
	get_node_queues(qdev, phy, NULL, 0, &info->num_of_queues);
	update_cmd_id(&qdev->drvcmds);
	transmit_cmds(qdev);
	rc = 0;
out:
	QOS_UNLOCK(qdev);
	return rc;
}

int pp_qos_port_get_queues(
		struct pp_qos_dev *qdev,
		unsigned int id,
		uint16_t *queue_ids,
		unsigned int size,
		unsigned int *queues_num)
{
	const struct qos_node *node;
	bool node_configured;
	unsigned int phy;
	int rc;

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
	get_node_queues(qdev, phy, queue_ids, size, queues_num);
	update_cmd_id(&qdev->drvcmds);
	transmit_cmds(qdev);
	rc = 0;
out:
	QOS_UNLOCK(qdev);
	return rc;
}

int get_active_port_phy_queues(struct pp_qos_dev *qdev, u32 port_id,
			       u16 *rlms, u16 *ids, u32 size, u32 *queues_num)
{
	s32 rc = 0;
	u32 phy;
	const struct qos_node *node;

	if (unlikely(!qos_device_ready(qdev))) {
		rc = -EINVAL;
		goto out;
	}

	node = get_conform_node(qdev, port_id, node_port);
	if (!node) {
		rc = -EINVAL;
		goto out;
	}

	phy = get_phy_from_node(qdev->nodes, node);
	get_active_port_rlms(qdev, phy, rlms, size, queues_num);
	get_active_node_queues(qdev, phy, ids, size, queues_num);
out:
	return rc;
}

int store_port_queue_max_allowed(struct pp_qos_dev *qdev,
				 u32 port_id, u16 *rlms, u16 *rlms_ids,
				 u32 queues_num)
{
	s32 rc = 0;
	u32 queue_idx;
	const struct qos_node *node;
	unsigned long addr;

	if (!qos_device_ready(qdev)) {
		rc = -EINVAL;
		goto out;
	}

	node = get_conform_node(qdev, port_id, node_port);
	if (!node) {
		rc = -EINVAL;
		goto out;
	}

	/* Store each queue max allowed value */
	for (queue_idx = 0; queue_idx < queues_num; queue_idx++) {
		node = get_conform_node(qdev, *rlms_ids, node_queue);
		if (!node) {
			rc = -EINVAL;
			goto out;
		}

		addr = (unsigned long)qdev->hwconf.max_allowed_ddr_virt +
			(*rlms * sizeof(u32));

		*(volatile unsigned long*)(addr) = node->data.queue.max_allowed;

		rlms++;
		rlms_ids++;
	}

out:
	return rc;
}

static int _pp_qos_port_set(
		struct pp_qos_dev *qdev,
		unsigned int id,
		const struct pp_qos_port_conf *conf)
{
	int rc;
	uint32_t modified;
	struct qos_node node;
	struct qos_node *nodep;
	uint16_t phy;
	bool new_port = false;

	modified = 0;

	QOS_LOG_API_DEBUG("port=%u bw=%u shared=%u arb=%u be=%u"
			  " r_addr=%#lx r_size=%u pkt_cred=%u cred=%u"
			  " dis=%u > port\n",
			  id, conf->common_prop.bandwidth_limit,
			  conf->common_prop.shared_bandwidth_group,
			  conf->port_parent_prop.arbitration,
			  conf->port_parent_prop.best_effort_enable,
			  (unsigned long)conf->ring_address, conf->ring_size,
			  conf->packet_credit_enable, conf->credit,
			  conf->disable);

	nodep = get_conform_node(qdev, id, NULL);
	if (!nodep)
		return -EINVAL;
	memcpy(&node, nodep, sizeof(node));

	if (node.type != TYPE_PORT) {
		node.type = TYPE_PORT;
		QOS_BITS_SET(modified, QOS_MODIFIED_NODE_TYPE);
		new_port = true;
	}

	rc = set_node_prop(
			qdev,
			&node,
			&conf->common_prop,
			&conf->port_parent_prop,
			NULL,
			&modified);
	if (rc)
		return rc;

	rc = set_port_specific_prop(qdev, &node, conf, &modified);
	if (rc)
		return rc;

	if (!port_cfg_valid(qdev, &node, nodep))
		return -EINVAL;

	memcpy(nodep, &node, sizeof(struct qos_node));

	if (modified) {
		phy = get_phy_from_node(qdev->nodes, nodep);
		create_set_port_cmd(qdev, conf, phy, modified);
	}

	if (!new_port &&
	    (modified & (QOS_MODIFIED_BANDWIDTH_LIMIT |
			 QOS_MODIFIED_SHARED_GROUP_ID)))
		add_suspend_port(qdev, get_port(qdev->nodes, phy));

	return 0;
}

int pp_qos_port_set(
		struct pp_qos_dev *qdev,
		unsigned int id,
		const struct pp_qos_port_conf *conf)
{
	int rc;

	QOS_LOCK(qdev);
	PP_QOS_ENTER_FUNC();
	if (!qos_device_ready(qdev)) {
		rc = -EINVAL;
		goto out;
	}

	rc = _pp_qos_port_set(qdev, id, conf);
	update_cmd_id(&qdev->drvcmds);
	transmit_cmds(qdev);
out:
	QOS_UNLOCK(qdev);
	return rc;
}

/**
 * pp_qos_port_stat_get() - Get port's statistics
 * @qos_dev: handle to qos device instance obtained previously from qos_dev_init
 * @id:	  queue's id obtained from queue_allocate
 * @stat: pointer to struct to be filled with queue's statistics
 *
 * Return: 0 on success
 */
int pp_qos_port_stat_get(
		struct pp_qos_dev *qdev,
		unsigned int id,
		struct pp_qos_port_stat *stat)
{
	struct qos_node *node;
	bool node_configured;
	int rc;

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

	create_get_port_stats_cmd(
			qdev,
			get_phy_from_node(qdev->nodes, node),
			qdev->hwconf.fw_stat,
			stat);

	update_cmd_id(&qdev->drvcmds);
	transmit_cmds(qdev);

	rc = 0;
out:
	QOS_UNLOCK(qdev);
	return rc;
}


/******************************************************************************/
/*                                     QUEUES                                 */
/******************************************************************************/
static int _pp_qos_queue_set(
		struct pp_qos_dev *qdev,
		unsigned int id,
		const struct pp_qos_queue_conf *conf,
		struct qos_node *alias_node);
static int _pp_qos_queue_conf_get(
		struct pp_qos_dev *qdev,
		unsigned int id,
		struct pp_qos_queue_conf *conf);

static void node_queue_init(struct pp_qos_dev *qdev, struct qos_node *node)
{
	node_init(qdev, node, 1, 0, 1);
	memset(&(node->data.queue), 0x0, sizeof(struct _queue));
	node->type = TYPE_QUEUE;
}

static int set_queue_specific_prop(struct pp_qos_dev *qdev,
				  struct qos_node *node,
				  const struct pp_qos_queue_conf *conf,
				  uint32_t *modified)
{
	if (node->data.queue.max_burst != conf->max_burst) {
		node->data.queue.max_burst = conf->max_burst;
		QOS_BITS_SET(*modified, QOS_MODIFIED_MAX_BURST);
	}

	if (node->data.queue.green_min != conf->queue_wred_min_avg_green) {
		node->data.queue.green_min = conf->queue_wred_min_avg_green;
		QOS_BITS_SET(*modified, QOS_MODIFIED_WRED_MIN_GREEN);
	}

	if (node->data.queue.green_max != conf->queue_wred_max_avg_green) {
		node->data.queue.green_max = conf->queue_wred_max_avg_green;
		QOS_BITS_SET(*modified, QOS_MODIFIED_WRED_MAX_GREEN);
	}

	if (node->data.queue.green_slope != conf->queue_wred_slope_green) {
		node->data.queue.green_slope = conf->queue_wred_slope_green;
		QOS_BITS_SET(*modified, QOS_MODIFIED_WRED_SLOPE_GREEN);
	}

	if (node->data.queue.yellow_min != conf->queue_wred_min_avg_yellow) {
		node->data.queue.yellow_min = conf->queue_wred_min_avg_yellow;
		QOS_BITS_SET(*modified, QOS_MODIFIED_WRED_MIN_YELLOW);
	}

	if (node->data.queue.yellow_max != conf->queue_wred_max_avg_yellow) {
		node->data.queue.yellow_max = conf->queue_wred_max_avg_yellow;
		QOS_BITS_SET(*modified, QOS_MODIFIED_WRED_MAX_YELLOW);
	}

	if (node->data.queue.yellow_slope != conf->queue_wred_slope_yellow) {
		node->data.queue.yellow_slope = conf->queue_wred_slope_yellow;
		QOS_BITS_SET(*modified, QOS_MODIFIED_WRED_SLOPE_YELLOW);
	}

	if (node->data.queue.max_burst != conf->max_burst) {
		node->data.queue.max_burst = conf->max_burst;
		QOS_BITS_SET(*modified, QOS_MODIFIED_MAX_BURST);
	}

	if (node->data.queue.max_allowed != conf->queue_wred_max_allowed) {
		node->data.queue.max_allowed = conf->queue_wred_max_allowed;
		QOS_BITS_SET(*modified, QOS_MODIFIED_WRED_MAX_ALLOWED);
	}

	if (node->data.queue.min_guaranteed !=
			conf->queue_wred_min_guaranteed) {
		node->data.queue.min_guaranteed =
			conf->queue_wred_min_guaranteed;
		QOS_BITS_SET(*modified, QOS_MODIFIED_WRED_MIN_GUARANTEED);
	}

	if (!!(conf->wred_enable) !=
			!!QOS_BITS_IS_SET(node->flags,
				QOS_NODE_FLAGS_QUEUE_WRED_ENABLE)) {
		QOS_BITS_TOGGLE(node->flags, QOS_NODE_FLAGS_QUEUE_WRED_ENABLE);
		QOS_BITS_SET(*modified, QOS_MODIFIED_WRED_ENABLE);
	}

	if (!!(conf->wred_fixed_drop_prob_enable) !=
			!!QOS_BITS_IS_SET(node->flags,
			QOS_NODE_FLAGS_QUEUE_WRED_FIXED_DROP_PROB_ENABLE)) {
		QOS_BITS_TOGGLE(node->flags,
			QOS_NODE_FLAGS_QUEUE_WRED_FIXED_DROP_PROB_ENABLE);
		QOS_BITS_SET(*modified,
			QOS_MODIFIED_WRED_FIXED_DROP_PROB_ENABLE);
	}

	if (node->data.queue.fixed_green_prob !=
			conf->queue_wred_fixed_drop_prob_green) {
		node->data.queue.fixed_green_prob =
			conf->queue_wred_fixed_drop_prob_green;
		QOS_BITS_SET(*modified, QOS_MODIFIED_WRED_FIXED_GREEN_PROB);
	}

	if (node->data.queue.fixed_yellow_prob !=
			conf->queue_wred_fixed_drop_prob_yellow) {
		node->data.queue.fixed_yellow_prob =
			conf->queue_wred_fixed_drop_prob_yellow;
		QOS_BITS_SET(*modified, QOS_MODIFIED_WRED_FIXED_YELLOW_PROB);
	}

	if (!!(conf->blocked) !=
			!!QOS_BITS_IS_SET(node->flags,
				QOS_NODE_FLAGS_QUEUE_BLOCKED)) {
		QOS_BITS_TOGGLE(node->flags, QOS_NODE_FLAGS_QUEUE_BLOCKED);
		QOS_BITS_SET(*modified, QOS_MODIFIED_BLOCKED);
	}
	return 0;
}

STATIC_UNLESS_TEST int queue_cfg_valid(
		const struct pp_qos_dev *qdev,
		const struct qos_node *node,
		const struct qos_node *orig_node,
		unsigned int prev_virt_parent_phy)
{
	if (!node_queue(node)) {
		QOS_LOG("Node is not a queue\n");
		return 0;
	}

	return node_cfg_valid(qdev, node, orig_node, prev_virt_parent_phy);
}

static int check_queue_conf_validity(
		struct pp_qos_dev *qdev,
		unsigned int id,
		const struct pp_qos_queue_conf *conf,
		struct qos_node *node,
		uint32_t *modified,
		struct qos_node *alias_node)
{
	unsigned int phy;
	int rc;
	struct qos_node *nodep;

	/*
	 * The phy of the current first ancesstor which is not an
	 * internal scheduler QOS_INVALID_PHY if it is a new queue
	 */
	unsigned int prev_virt_parent_phy;

	if (!QOS_ID_VALID(id)) {
		QOS_LOG_ERR("Illegal id %u\n", id);
		return -EINVAL;
	}

	/* Check if node id is valid */
	phy = get_phy_from_id(qdev->mapping, id);
	if (!QOS_PHY_VALID(phy)) {
		if (!QOS_PHY_UNKNOWN(phy)) {
			QOS_LOG_ERR("Id %u is not a node\n", id);
			return -EINVAL;
		}
		nodep = NULL;

		/* New queue which has id, but no phy was allocated for it */
		node_queue_init(qdev, node);

		if (alias_node == NULL) {
			node->data.queue.alias_master_id = PP_QOS_INVALID_ID;
			node->data.queue.alias_slave_id = PP_QOS_INVALID_ID;
			node->data.queue.rlm = pp_pool_get(qdev->rlms);
		} else {
			node->data.queue.alias_master_id = get_id_from_phy(
					qdev->mapping,
					get_phy_from_node(qdev->nodes, alias_node));
			node->data.queue.alias_slave_id = PP_QOS_INVALID_ID;
			node->data.queue.rlm = alias_node->data.queue.rlm;
		}

		if (node->data.queue.rlm == QOS_INVALID_RLM) {
			QOS_LOG_ERR("Can't get free rlm\n");
			return  -EBUSY;
		}
		QOS_BITS_SET(node->flags, QOS_NODE_FLAGS_USED);
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
			     QOS_MODIFIED_RLM);
		prev_virt_parent_phy = QOS_INVALID_PHY;
	} else {
		nodep = get_node_from_phy(qdev->nodes, phy);
		if (!node_queue(nodep)) {
			QOS_LOG("Id %u is not a queue\n", id);
			rc = -EINVAL;
		}
		memcpy(node, nodep, sizeof(struct qos_node));
		prev_virt_parent_phy = get_virtual_parent_phy(
				qdev->nodes,
				node);
	}

	/*
	 * Before calling set_node_prop:
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
	 *     node.child.parent.phy == virtual parent phy
	 * Else
	 *     If map_id_to_phy(conf.parent) != prev_virt_parent_phy
	 *		node.child.parent.phy = map_id_to_phy(conf.parent)
	 *     Else
	 *		node.child.parent.phy = actual parent (no change)
	 *
	 */
	rc = set_node_prop(
			qdev,
			node,
			&conf->common_prop,
			NULL,
			&conf->queue_child_prop,
			modified);
	if (rc)
		return rc;

	rc = set_queue_specific_prop(qdev, node, conf, modified);
	if (rc)
		return rc;

	if (!queue_cfg_valid(qdev, node, nodep, prev_virt_parent_phy))
		return -EINVAL;

	return 0;
}

void pp_qos_queue_conf_set_default(struct pp_qos_queue_conf *conf)
{
	memset(conf, 0, sizeof(struct pp_qos_queue_conf));
	conf->common_prop.bandwidth_limit = QOS_NO_BANDWIDTH_LIMIT;
	conf->common_prop.shared_bandwidth_group =
		QOS_NO_SHARED_BANDWIDTH_GROUP;
}

int pp_qos_queue_allocate(struct pp_qos_dev *qdev, unsigned int *id)
{
	int rc;
	uint16_t _id;

	QOS_LOG_API_DEBUG("queue > allocate\n");

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
	rc = 0;
out:
	QOS_UNLOCK(qdev);
	return rc;
}

static int _pp_qos_queue_conf_get(
		struct pp_qos_dev *qdev,
		unsigned int id,
		struct pp_qos_queue_conf *conf)
{
	bool node_configured;
	const struct qos_node *node;
	int rc;

	if (conf == NULL)
		return -EINVAL;

	rc = is_node_configured(qdev, id, &node_configured);
	if (rc || !node_configured)
		return -EINVAL;

	node = get_conform_node(qdev, id, node_queue);
	if (!node)
		return -EINVAL;

	conf->max_burst = node->data.queue.max_burst;
	conf->queue_wred_min_avg_green = node->data.queue.green_min;
	conf->queue_wred_max_avg_green = node->data.queue.green_max;
	conf->queue_wred_slope_green = node->data.queue.green_slope;
	conf->queue_wred_min_avg_yellow = node->data.queue.yellow_min;
	conf->queue_wred_max_avg_yellow = node->data.queue.yellow_max;
	conf->queue_wred_slope_yellow = node->data.queue.yellow_slope;
	conf->queue_wred_max_allowed = node->data.queue.max_allowed;
	conf->queue_wred_min_guaranteed = node->data.queue.min_guaranteed;
	conf->queue_wred_fixed_drop_prob_green =
		node->data.queue.fixed_green_prob;
	conf->queue_wred_fixed_drop_prob_yellow =
		node->data.queue.fixed_yellow_prob;
	conf->wred_enable =
		!!QOS_BITS_IS_SET(node->flags,
				QOS_NODE_FLAGS_QUEUE_WRED_ENABLE);
	conf->wred_fixed_drop_prob_enable =
		!!QOS_BITS_IS_SET(node->flags,
			QOS_NODE_FLAGS_QUEUE_WRED_FIXED_DROP_PROB_ENABLE);
	conf->blocked =
		!!QOS_BITS_IS_SET(node->flags,
				QOS_NODE_FLAGS_QUEUE_BLOCKED);
	return get_node_prop(
			qdev,
			node,
			&conf->common_prop,
			NULL,
			&conf->queue_child_prop);
}

int pp_qos_queue_conf_get(
		struct pp_qos_dev *qdev,
		unsigned int id,
		struct pp_qos_queue_conf *conf)
{
	int rc;

	QOS_LOCK(qdev);
	PP_QOS_ENTER_FUNC();
	if (!qos_device_ready(qdev)) {
		rc = -EINVAL;
		goto out;
	}

	rc = _pp_qos_queue_conf_get(qdev, id, conf);
	update_cmd_id(&qdev->drvcmds);
	transmit_cmds(qdev);
out:
	QOS_UNLOCK(qdev);
	return rc;
}

static int _pp_qos_queue_remove(struct pp_qos_dev *qdev, int id)
{
	struct qos_node *node;
	int rc;

	node = get_conform_node(qdev, id, node_queue);
	if (!node)
		return -EINVAL;

	// Can't delete a master aliased queue, which has active slave
	if (node->data.queue.alias_slave_id != PP_QOS_INVALID_ID) {
		QOS_LOG_ERR("Can't remove master aliased queue (slave is %d)\n",
			node->data.queue.alias_slave_id);
		return -EINVAL;
	}

	// In case this is a slave alised queue, update its master
	if (node->data.queue.alias_master_id != PP_QOS_INVALID_ID) {
		struct qos_node *master_node;

		master_node = get_conform_node(qdev,
				node->data.queue.alias_master_id, node_queue);
		if (!master_node)
			return -EINVAL;

		master_node->data.queue.alias_slave_id = PP_QOS_INVALID_ID;
	}

	rc = node_remove(qdev, node);
	return rc;
}

/**
 * pp_qos_queue_remove() -	Remove a queue
 * @qos_dev: handle to qos device instance obtained previously from qos_dev_init
 * @id:   queue's id obtained from queue_allocate
 *
 * Note: client should make sure that queue is empty and
 * that new packets are not enqueued, by calling
 *       pp_qos_queue_disable and pp_qos_queue_flush
 *
 * Return: 0 on success
 */
int pp_qos_queue_remove(struct pp_qos_dev *qdev, unsigned int id)
{
	bool node_configured;
	int rc;

	QOS_LOCK(qdev);
	PP_QOS_ENTER_FUNC();
	if (!qos_device_ready(qdev)) {
		rc = -EINVAL;
		goto out;
	}

	rc = is_node_configured(qdev, id, &node_configured);
	if (rc)
		goto out;

	/* If node is not configured just release id */
	if (!node_configured) {
		release_id(qdev, id);
		goto out;
	}

	rc = _pp_qos_queue_remove(qdev, id);
	update_cmd_id(&qdev->drvcmds);
	transmit_cmds(qdev);

	QOS_LOG_API_DEBUG("queue %u > remove\n", id);
out:
	QOS_UNLOCK(qdev);
	return rc;
}


/**
 * pp_qos_queue_set() - Set queue configuration
 * @qos_dev: handle to qos device instance obtained previously from qos_dev_init
 * @id:	  queue's id obtained from queue_allocate
 * @conf: new configuration for the queue
 *
 * Return: 0 on success
 */
static int _pp_qos_queue_set(
		struct pp_qos_dev *qdev,
		unsigned int id,
		const struct pp_qos_queue_conf *conf,
		struct qos_node *alias_node)
{
	int rc;
	unsigned int phy;
	unsigned int prev_phy;
	struct qos_node *parent;
	struct qos_node node;
	struct qos_node *nodep;
	uint32_t modified;
	int parent_changed;
	unsigned int dst_port;

	modified = 0;
	nodep = NULL;

	QOS_LOG_API_DEBUG(
		"queue=%u bw=%u shared=%u parent=%u"
		" priority=%u wrr_weight=%u max_burst=%u blocked=%u"
		" wred_enable=%u wred_fixed_drop_prob=%u"
		" wred_min_avg_green=%u wred_max_avg_green=%u"
		" wred_slope_green=%u wred_fixed_drop_prob_green=%u"
		" wred_min_avg_yellow=%u wred_max_avg_yellow=%u"
		" wred_slope_yellow=%u wred_fixed_drop_prob_yellow=%u"
		" wred_min_guaranteed=%u wred_max_allowed=%u > queue\n",
		id, conf->common_prop.bandwidth_limit,
		conf->common_prop.shared_bandwidth_group,
		conf->queue_child_prop.parent, conf->queue_child_prop.priority,
		conf->queue_child_prop.wrr_weight, conf->max_burst,
		conf->blocked, conf->wred_enable,
		conf->wred_fixed_drop_prob_enable,
		conf->queue_wred_min_avg_green, conf->queue_wred_max_avg_green,
		conf->queue_wred_slope_green,
		conf->queue_wred_fixed_drop_prob_green,
		conf->queue_wred_min_avg_yellow,
		conf->queue_wred_max_avg_yellow, conf->queue_wred_slope_yellow,
		conf->queue_wred_fixed_drop_prob_yellow,
		conf->queue_wred_min_guaranteed, conf->queue_wred_max_allowed);

	rc = check_queue_conf_validity(qdev, id, conf,
			&node, &modified, alias_node);
	parent_changed = QOS_BITS_IS_SET(modified, QOS_MODIFIED_PARENT);
	if (rc)
		goto out;

	if (parent_changed) {
		parent = get_node_from_phy(qdev->nodes,
					   node.child_prop.parent_phy);
		phy = phy_alloc_by_parent(qdev,
					  parent,
					  conf->queue_child_prop.priority);
		if (!QOS_PHY_VALID(phy)) {
			rc = -EINVAL;
			goto out;
		}
		nodep = get_node_from_phy(qdev->nodes, phy);
		node.child_prop.parent_phy = nodep->child_prop.parent_phy;
		memcpy(nodep, &node, sizeof(struct qos_node));

		/* If this is not a new queue - delete previous node */
		if (!QOS_BITS_IS_SET(modified, QOS_MODIFIED_NODE_TYPE)) {
			prev_phy = get_phy_from_id(qdev->mapping, id);
			add_suspend_port(qdev, get_port(qdev->nodes, prev_phy));
			dst_port = get_port(qdev->nodes, phy);
			create_move_cmd(qdev, phy, prev_phy, dst_port);
			map_invalidate_id(qdev->mapping, id);
			map_id_phy(qdev->mapping, id, phy);
			node_phy_delete(qdev, prev_phy);
			phy = get_phy_from_id(qdev->mapping, id);
			nodep = get_node_from_phy(qdev->nodes, phy);
			if (phy != prev_phy)
				update_predecessors(qdev, nodep, NULL);
		} else {
			map_id_phy(qdev->mapping, id, phy);
		}

		parent = get_node_from_phy(qdev->nodes,
					   nodep->child_prop.parent_phy);
	} else {
		phy = get_phy_from_id(qdev->mapping, id);
		nodep = get_node_from_phy(qdev->nodes, phy);
		parent = get_node_from_phy(qdev->nodes,
					   nodep->child_prop.parent_phy);

		/* Child of WSP changes priority i.e. position */
		if ((parent->parent_prop.arbitration ==
			 PP_QOS_ARBITRATION_WSP)
			&&
			QOS_BITS_IS_SET(modified, QOS_MODIFIED_PRIORITY)) {
			update_children_position(
					qdev,
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
		create_set_queue_cmd(
				qdev,
				conf,
				phy,
				nodep->child_prop.parent_phy,
				nodep->data.queue.rlm,
				modified);

	if (parent_changed && nodep->child_prop.virt_bw_share)
		update_internal_bandwidth(qdev, parent);

out:
	if (rc) {
		if (parent_changed) {
			if (QOS_BITS_IS_SET(modified, QOS_MODIFIED_RLM))
				release_rlm(qdev->rlms, node.data.queue.rlm);
		}
	}

	return rc;
}

int pp_qos_queue_set(struct pp_qos_dev *qdev,
		     unsigned int id,
		     const struct pp_qos_queue_conf *conf)
{
	int rc;

	QOS_LOCK(qdev);
	PP_QOS_ENTER_FUNC();
	if (!qos_device_ready(qdev)) {
		rc = -EINVAL;
		goto out;
	}

	rc = _pp_qos_queue_set(qdev, id, conf, NULL);
	update_cmd_id(&qdev->drvcmds);
	transmit_cmds(qdev);
out:
	QOS_UNLOCK(qdev);
	return rc;
}

int pp_qos_queue_set_aliased_queue(struct pp_qos_dev *qdev,
				   unsigned int id,
				   const struct pp_qos_queue_conf *conf,
				   unsigned int alias_master_id)
{
	int rc;
	unsigned int phy;
	struct qos_node *node;

	QOS_LOCK(qdev);
	PP_QOS_ENTER_FUNC();
	if (!qos_device_ready(qdev)) {
		rc = -EINVAL;
		goto out;
	}

	/* Check if node id is valid */
	phy = get_phy_from_id(qdev->mapping, alias_master_id);
	if (!QOS_PHY_VALID(phy)) {
		rc = -EINVAL;
		goto out;
	}

	node = get_conform_node(qdev, alias_master_id, node_queue);
	if (!node) {
		rc = -EINVAL;
		goto out;
	}

	rc = _pp_qos_queue_set(qdev, id, conf, node);
	if (!rc) {
		// Retrieve the master alias node again since it might have
		// been relocated in the DB
		node = get_conform_node(qdev, alias_master_id, node_queue);
		if (!node) {
			rc = -EINVAL;
			goto out;
		}

		node->data.queue.alias_master_id = PP_QOS_INVALID_ID;
		node->data.queue.alias_slave_id = id;
	}

	update_cmd_id(&qdev->drvcmds);
	transmit_cmds(qdev);

out:
	QOS_UNLOCK(qdev);
	return rc;
}

int _pp_qos_queue_block(struct pp_qos_dev *qdev, unsigned int id)
{
	int rc;
	struct pp_qos_queue_conf conf;

	rc = _pp_qos_queue_conf_get(qdev, id, &conf);
	if (rc)
		return rc;
	conf.blocked = 1;
	return _pp_qos_queue_set(qdev, id, &conf, NULL);
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
int pp_qos_queue_block(struct pp_qos_dev *qdev, unsigned int id)
{
	int rc;

	QOS_LOCK(qdev);
	PP_QOS_ENTER_FUNC();
	if (!qos_device_ready(qdev)) {
		rc = -EINVAL;
		goto out;
	}
	rc = _pp_qos_queue_block(qdev, id);
	update_cmd_id(&qdev->drvcmds);
	transmit_cmds(qdev);
out:
	QOS_UNLOCK(qdev);
	return rc;
}

int _pp_qos_queue_unblock(struct pp_qos_dev *qdev, unsigned int id)
{
	int rc;
	struct pp_qos_queue_conf conf;

	rc = _pp_qos_queue_conf_get(qdev, id, &conf);
	if (rc)
		return rc;

	conf.blocked = 0;
	return _pp_qos_queue_set(qdev, id, &conf, NULL);
}

/**
 * pp_qos_queue_unblock() - Unblock enqueuing of new packets
 * @qos_dev: handle to qos device instance obtained previously from qos_dev_init
 * @id:	  queue's id obtained from queue_allocate
 *
 * Return: 0 on success
 */
int pp_qos_queue_unblock(struct pp_qos_dev *qdev, unsigned int id)
{
	int rc;

	QOS_LOCK(qdev);
	PP_QOS_ENTER_FUNC();
	if (!qos_device_ready(qdev)) {
		rc = -EINVAL;
		goto out;
	}
	rc = _pp_qos_queue_unblock(qdev, id);
	update_cmd_id(&qdev->drvcmds);
	transmit_cmds(qdev);
out:
	QOS_UNLOCK(qdev);
	return rc;
}


int _pp_qos_queue_flush(struct pp_qos_dev *qdev, unsigned int id)
{
	struct qos_node *node;

	node = get_conform_node(qdev, id, node_queue);
	if (!node)
		return -EINVAL;
	return node_flush(qdev, node);
}

/**
 * pp_qos_queue_flush() - Drop all enqueued packets
 * @qos_dev: handle to qos device instance obtained previously from qos_dev_init
 * @id:	  queue's id obtained from queue_allocate
 *
 * Return: 0 on success
 */
int pp_qos_queue_flush(struct pp_qos_dev *qdev, unsigned int id)
{
	bool node_configured;
	int rc;

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

	rc = _pp_qos_queue_flush(qdev, id);
	update_cmd_id(&qdev->drvcmds);
	transmit_cmds(qdev);
out:
	QOS_UNLOCK(qdev);
	return rc;
}


/**
 * pp_qos_queue_info_get() - Get information about queue
 * @qos_dev: handle to qos device instance obtained previously from qos_dev_init
 * @id:	  queue's id obtained from queue_allocate
 * @info: pointer to struct to be filled with queue's info
 *
 * Return: 0 on success
 */
int pp_qos_queue_info_get(struct pp_qos_dev *qdev, unsigned int id,
			  struct pp_qos_queue_info *info)
{
	struct qos_node *node;
	bool node_configured;
	int rc;

	QOS_LOCK(qdev);
	PP_QOS_ENTER_FUNC();
	if (!qos_device_ready(qdev)) {
		rc = -EINVAL;
		goto out;
	}

	if (info == NULL) {
		rc = -EINVAL;
		goto out;
	}

	rc = is_node_configured(qdev, id, &node_configured);
	if (rc || !node_configured) {
		rc = -EINVAL;
		goto out;
	}

	node = get_conform_node(qdev, id, node_queue);
	if (!node) {
		rc = -EINVAL;
		goto out;
	}

	info->physical_id = node->data.queue.rlm;
	info->port_id = get_port(
			qdev->nodes,
			get_phy_from_node(qdev->nodes, node));
	rc = 0;
	update_cmd_id(&qdev->drvcmds);
	transmit_cmds(qdev);
out:
	QOS_UNLOCK(qdev);
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
int pp_qos_queue_stat_get(struct pp_qos_dev *qdev, unsigned int id,
			  struct pp_qos_queue_stat *stat)
{
	struct qos_node *node;
	bool node_configured;
	int rc;

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

	node = get_conform_node(qdev, id, node_queue);
	if (!node) {
		rc = -EINVAL;
		goto out;
	}
	create_get_queue_stats_cmd(
			qdev,
			get_phy_from_node(qdev->nodes, node),
			node->data.queue.rlm,
			qdev->hwconf.fw_stat,
			stat);

	update_cmd_id(&qdev->drvcmds);
	transmit_cmds(qdev);

	rc = 0;
out:
	QOS_UNLOCK(qdev);
	return rc;
}

/******************************************************************************/
/*                                 Schedulers                                 */
/******************************************************************************/
static int _pp_qos_sched_set(
		struct pp_qos_dev *qdev,
		unsigned int id,
		const struct pp_qos_sched_conf *conf);
static int _pp_qos_sched_conf_get(
		struct pp_qos_dev *qdev,
		unsigned int id,
		struct pp_qos_sched_conf *conf);

static void node_sched_init(struct pp_qos_dev *qdev, struct qos_node *node)
{
	node_init(qdev, node, 1, 1, 1);
	node->type = TYPE_SCHED;
	node->data.sched.new_child_phy = QOS_UNKNOWN_PHY;
	node->data.sched.new_child_skipped = 0;
}

STATIC_UNLESS_TEST int sched_cfg_valid(
		const struct pp_qos_dev *qdev,
		const struct qos_node *node,
		const struct qos_node *orig_node,
		unsigned int prev_virt_parent_phy)
{
	if (!node_sched(node)) {
		QOS_LOG_ERR("Node is not a sched\n");
		return 0;
	}

	return node_cfg_valid(qdev, node, orig_node, prev_virt_parent_phy);
}

static int check_sched_conf_validity(
		struct pp_qos_dev *qdev,
		unsigned int id,
		const struct pp_qos_sched_conf *conf,
		struct qos_node *node,
		uint32_t *modified)
{
	unsigned int phy;
	int rc;
	struct qos_node *nodep;
	unsigned int prev_virt_parent_phy_phy;

	if (!QOS_ID_VALID(id)) {
		QOS_LOG_ERR("Illegal id %u\n", id);
		return -EINVAL;
	}

	/* Check if node id is valid */
	phy = get_phy_from_id(qdev->mapping, id);
	if (!QOS_PHY_VALID(phy)) {
		if (!QOS_PHY_UNKNOWN(phy)) {
			QOS_LOG_ERR("Id %u is not a node\n", id);
			return -EINVAL;
		}

		/* New sched which has id, but no phy was allocated for it */
		node_sched_init(qdev, node);
		QOS_BITS_SET(node->flags, QOS_NODE_FLAGS_USED);
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
		prev_virt_parent_phy_phy = get_virtual_parent_phy(
				qdev->nodes,
				node);
	}

	rc = set_node_prop(qdev,
			node,
			&conf->common_prop,
			&conf->sched_parent_prop,
			&conf->sched_child_prop,
			modified);
	if (rc)
		return rc;

	if (!sched_cfg_valid(qdev, node, nodep, prev_virt_parent_phy_phy))
		return -EINVAL;

	return 0;
}

void pp_qos_sched_conf_set_default(struct pp_qos_sched_conf *conf)
{
	memset(conf, 0, sizeof(struct pp_qos_sched_conf));
	conf->common_prop.bandwidth_limit = QOS_NO_BANDWIDTH_LIMIT;
	conf->common_prop.shared_bandwidth_group =
		QOS_NO_SHARED_BANDWIDTH_GROUP;
	conf->sched_parent_prop.arbitration = PP_QOS_ARBITRATION_WRR;
}

int pp_qos_sched_allocate(struct pp_qos_dev *qdev, unsigned int *id)
{
	uint16_t _id;
	int rc;

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
	rc = 0;
out:
	QOS_UNLOCK(qdev);
	return rc;
}

int pp_qos_sched_remove(struct pp_qos_dev *qdev, unsigned int id)
{
	struct qos_node *node;
	bool node_configured;
	int rc;

	QOS_LOCK(qdev);
	PP_QOS_ENTER_FUNC();
	if (!qos_device_ready(qdev)) {
		rc = -EINVAL;
		goto out;
	}

	rc = is_node_configured(qdev, id, &node_configured);
	if (rc)
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

	rc = tree_remove(qdev, get_phy_from_node(qdev->nodes, node));
	update_cmd_id(&qdev->drvcmds);
	transmit_cmds(qdev);

	QOS_LOG_API_DEBUG("sched %u > remove\n", id);
out:
	QOS_UNLOCK(qdev);
	return rc;
}

static int _pp_qos_sched_set(
		struct pp_qos_dev *qdev,
		unsigned int id,
		const struct pp_qos_sched_conf *conf)
{
	int rc;
	unsigned int phy;
	unsigned int prev_phy;
	struct qos_node *parent;
	struct qos_node node;
	struct qos_node *nodep;
	struct qos_node *oldnode;
	uint32_t modified;
	unsigned int dst_port;

	modified = 0;

	rc = check_sched_conf_validity(qdev, id, conf, &node, &modified);
	if (rc)
		return rc;

	if (QOS_BITS_IS_SET(modified, QOS_MODIFIED_PARENT)) {
		parent = get_node_from_phy(
				qdev->nodes,
				node.child_prop.parent_phy);
		phy = phy_alloc_by_parent(
				qdev,
				parent,
				conf->sched_child_prop.priority);
		if (!QOS_PHY_VALID(phy))
			return -EINVAL;

		/*
		 * nodep's parent_phy holds value of actual parent, node's
		 * parent_phy holds phy of virtual parent
		 */
		nodep = get_node_from_phy(qdev->nodes, phy);
		node.child_prop.parent_phy = nodep->child_prop.parent_phy;
		memcpy(nodep, &node, sizeof(struct qos_node));

		/* If this is not a new sched - delete previous node */
		if (!QOS_BITS_IS_SET(modified, QOS_MODIFIED_NODE_TYPE)) {
			prev_phy = get_phy_from_id(qdev->mapping, id);
			add_suspend_port(qdev, get_port(qdev->nodes, prev_phy));
			dst_port = get_port(qdev->nodes, phy);
			create_move_cmd(qdev, phy, prev_phy, dst_port);
			node_update_children(qdev,
				get_node_from_phy(qdev->nodes, prev_phy),
				phy);
			map_invalidate_id(qdev->mapping, id);
			map_id_phy(qdev->mapping, id, phy);
			oldnode = get_node_from_phy(qdev->nodes, prev_phy);
			nodep->parent_prop.first_child_phy =
				oldnode->parent_prop.first_child_phy;
			nodep->parent_prop.num_of_children =
				oldnode->parent_prop.num_of_children;
			oldnode->parent_prop.first_child_phy = QOS_INVALID_PHY;
			oldnode->parent_prop.num_of_children = 0;
			node_phy_delete(qdev, prev_phy);
			phy = get_phy_from_id(qdev->mapping, id);
			nodep = get_node_from_phy(qdev->nodes, phy);
			tree_update_predecessors(qdev, phy);
		} else {
			map_id_phy(qdev->mapping, id, phy);
		}

		parent = get_node_from_phy(
				qdev->nodes,
				nodep->child_prop.parent_phy);
	} else {
		phy = get_phy_from_id(qdev->mapping, id);
		nodep = get_node_from_phy(qdev->nodes, phy);
		parent = get_node_from_phy(
				qdev->nodes,
				nodep->child_prop.parent_phy);

		/* Child of WSP changes priority i.e. position */
		if (parent->parent_prop.arbitration == PP_QOS_ARBITRATION_WSP &&
			QOS_BITS_IS_SET(modified, QOS_MODIFIED_PRIORITY)) {
			update_children_position(
					qdev,
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
		create_set_sched_cmd(
				qdev,
				conf,
				phy,
				nodep->child_prop.parent_phy,
				modified);

	if (QOS_BITS_IS_SET(modified, QOS_MODIFIED_PARENT) &&
	    nodep->child_prop.virt_bw_share)
		update_internal_bandwidth(qdev, parent);

	return 0;
}

int pp_qos_sched_set(
		struct pp_qos_dev *qdev,
		unsigned int id,
		const struct pp_qos_sched_conf *conf)
{
	int rc;

	QOS_LOG_API_DEBUG("sched=%u bw=%u shared=%u arb=%u be=%u"
			  " parent=%u priority=%u wrr_weight=%u > sched\n",
			  id, conf->common_prop.bandwidth_limit,
			  conf->common_prop.shared_bandwidth_group,
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
	rc = _pp_qos_sched_set(qdev, id, conf);
	update_cmd_id(&qdev->drvcmds);
	transmit_cmds(qdev);
out:
	QOS_UNLOCK(qdev);
	return rc;
}

static int _pp_qos_sched_conf_get(
		struct pp_qos_dev *qdev,
		unsigned int id,
		struct pp_qos_sched_conf *conf)
{
	const struct qos_node *node;

	if (conf == NULL)
		return -EINVAL;

	node = get_conform_node(qdev, id, node_sched);
	if (!node)
		return -EINVAL;

	return get_node_prop(
			qdev,
			node,
			&conf->common_prop,
			&conf->sched_parent_prop,
			&conf->sched_child_prop);
}

int pp_qos_sched_conf_get(
		struct pp_qos_dev *qdev,
		unsigned int id,
		struct pp_qos_sched_conf *conf)
{
	bool node_configured;
	int rc;

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
	update_cmd_id(&qdev->drvcmds);
	transmit_cmds(qdev);
out:
	QOS_UNLOCK(qdev);
	return rc;
}

int pp_qos_sched_info_get(
		struct pp_qos_dev *qdev,
		unsigned int id,
		struct pp_qos_sched_info *info)
{
	struct qos_node *node;
	bool node_configured;
	unsigned int phy;
	int rc;

	QOS_LOCK(qdev);
	PP_QOS_ENTER_FUNC();
	if (!qos_device_ready(qdev)) {
		rc = -EINVAL;
		goto out;
	}

	if (info == NULL) {
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
	info->port_id = get_port(
			qdev->nodes,
			get_phy_from_node(qdev->nodes, node));
	get_node_queues(qdev, phy, NULL, 0, &info->num_of_queues);
	update_cmd_id(&qdev->drvcmds);
	transmit_cmds(qdev);
	rc = 0;
out:
	QOS_UNLOCK(qdev);
	return rc;
}

int pp_qos_sched_get_queues(struct pp_qos_dev *qdev, unsigned int id,
			    uint16_t *queue_ids, unsigned int size,
			    unsigned int *queues_num)
{
	const struct qos_node *node;
	bool node_configured;
	unsigned int phy;
	int rc;

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
	get_node_queues(qdev, phy, queue_ids, size, queues_num);
	update_cmd_id(&qdev->drvcmds);
	transmit_cmds(qdev);
	rc = 0;
out:
	QOS_UNLOCK(qdev);
	return rc;
}

int pp_qos_get_fw_version(
		struct pp_qos_dev *qdev,
		unsigned int *major,
		unsigned int *minor,
		unsigned int *build)
{
	int rc;

	QOS_LOCK(qdev);
	PP_QOS_ENTER_FUNC();
	if (!qos_device_ready(qdev)) {
		rc = -EINVAL;
		goto out;
	}

	*major = qdev->fwver.major;
	*minor = qdev->fwver.minor;
	*build = qdev->fwver.build;
	rc = 0;
out:
	QOS_UNLOCK(qdev);
	return rc;
}

/******************************************************************************/
/*                                 Node                                       */
/******************************************************************************/
int pp_qos_get_node_info(
		struct pp_qos_dev *qdev,
		unsigned int id,
		struct pp_qos_node_info *info)
{
	int rc;
	struct qos_node *node;

	QOS_LOCK(qdev);
	PP_QOS_ENTER_FUNC();
	if (!qos_device_ready(qdev)) {
		rc = -EINVAL;
		goto out;
	}

	node = get_conform_node(qdev, id, node_used);
	if (!node) {
		rc = -EINVAL;
		goto out;
	}

	create_get_node_info_cmd(
			qdev,
			get_phy_from_node(qdev->nodes, node),
			qdev->hwconf.fw_stat,
			info);

	update_cmd_id(&qdev->drvcmds);
	transmit_cmds(qdev);

	rc = 0;
out:
	QOS_UNLOCK(qdev);
	return rc;

}

/******************************************************************************/
/*                                 SHARED_LIMIT                               */
/******************************************************************************/
static int _pp_qos_shared_limit_group_get_members(
		struct pp_qos_dev *qdev,
		unsigned int id,
		uint16_t *members,
		unsigned int size,
		unsigned int *members_num);

int pp_qos_shared_limit_group_add(
		struct pp_qos_dev *qdev,
		unsigned int limit,
		unsigned int *id)
{
	unsigned int i;
	int rc;
	struct shared_bandwidth_group *grp;

	QOS_LOCK(qdev);
	PP_QOS_ENTER_FUNC();
	if (!qos_device_ready(qdev)) {
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
	grp->limit = limit;
	*id = i;

	create_add_shared_group_cmd(qdev, i, limit);
	update_cmd_id(&qdev->drvcmds);
	transmit_cmds(qdev);
	rc = 0;
out:
	QOS_UNLOCK(qdev);
	return rc;
}

int pp_qos_shared_limit_group_remove(struct pp_qos_dev *qdev, unsigned int id)
{
	int rc;
	unsigned int num;
	uint16_t tmp;

	QOS_LOCK(qdev);
	PP_QOS_ENTER_FUNC();
	if (!qos_device_ready(qdev)) {
		rc = -EINVAL;
		goto out;
	}
	if (QOS_BW_GRP_VALID(id) && (qdev->groups[id].used)) {
		_pp_qos_shared_limit_group_get_members(
				qdev, id, &tmp, 1, &num);
		if (num) {
			QOS_LOG_ERR(
					"Shared group %u has members can't remove\n",
					id);
			rc = -EBUSY;
			goto out;
		}
		qdev->groups[id].used = 0;
		create_remove_shared_group_cmd(qdev, id);
		update_cmd_id(&qdev->drvcmds);
		transmit_cmds(qdev);
		rc = 0;
	} else {
		QOS_LOG_ERR("Shared group %u is not used\n", id);
		rc = -EINVAL;
	}
out:
	QOS_UNLOCK(qdev);
	return rc;
}

int pp_qos_shared_limit_group_modify(
		struct pp_qos_dev *qdev,
		unsigned int id,
		unsigned int limit)
{
	int rc;

	QOS_LOCK(qdev);
	PP_QOS_ENTER_FUNC();
	if (QOS_BW_GRP_VALID(id) && (qdev->groups[id].used)) {
		if (qdev->groups[id].limit != limit) {
			create_set_shared_group_cmd(qdev, id, limit);
			update_cmd_id(&qdev->drvcmds);
			transmit_cmds(qdev);
		}
		rc = 0;
	} else {
		QOS_LOG_ERR("Shared group %u is not used\n", id);
		rc = -EINVAL;
	}
	QOS_UNLOCK(qdev);
	return rc;
}

static int _pp_qos_shared_limit_group_get_members(
		struct pp_qos_dev *qdev, unsigned int id,
		uint16_t *members,
		unsigned int size,
		unsigned int *members_num)
{
	unsigned int phy;
	unsigned int total;
	unsigned int tmp;
	const struct qos_node *node;

	total = 0;
	for (phy = 0; (phy <= qdev->max_port) && (size > 0); ++phy) {
		node = get_const_node_from_phy(qdev->nodes, phy);
		if (node_used(node)) {
			get_bw_grp_members_under_node(
					qdev,
					id,
					phy,
					members,
					size,
					&tmp);
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

int pp_qos_shared_limit_group_get_members(
		struct pp_qos_dev *qdev,
		unsigned int id,
		uint16_t *members,
		unsigned int size,
		unsigned int *members_num)
{
	int rc;

	QOS_LOCK(qdev);
	PP_QOS_ENTER_FUNC();
	if (!qos_device_ready(qdev)) {
		rc = -EINVAL;
		goto out;
	}
	rc = _pp_qos_shared_limit_group_get_members(
			qdev, id, members, size, members_num);
	update_cmd_id(&qdev->drvcmds);
	transmit_cmds(qdev);
out:
	QOS_UNLOCK(qdev);
	return rc;
}

struct pp_qos_dev *pp_qos_dev_open(unsigned int id)
{
	if (id < MAX_QOS_INSTANCES)
		return qos_devs[id];

	QOS_LOG_ERR("Illegal qos instance %u\n", id);
	return NULL;
}

static int __init qos_fw_log_en(char *str)
{
	QOS_LOG_INFO("PPv4 QoS FW Logger is enabled\n");
	qos_fw_log = true;

	return 0;
}

early_param("qosFwLog", qos_fw_log_en);

int pp_qos_dev_init(struct pp_qos_dev *qdev, struct pp_qos_init_param *conf)
{
	int rc;

	QOS_LOCK(qdev);
	PP_QOS_ENTER_FUNC();

	if (qdev->initialized) {
		QOS_LOG_ERR("Device already initialized, can't initialize again\n");
		rc = -EINVAL;
		goto out;
	}

	if ((qdev->max_port + 1) & 7) {
		QOS_LOG_ERR("Given max port %u is not last node of an octet\n",
				qdev->max_port);
		rc = -EINVAL;
		goto out;
	}

	if (conf->wred_p_const > 1023) {
		QOS_LOG_ERR("wred_p_const should be not greater than 1023\n");
		rc = -EINVAL;
		goto out;
	}

	qdev->hwconf.wred_total_avail_resources =
		conf->wred_total_avail_resources;

	if (!IS_ALIGNED(qdev->hwconf.wred_total_avail_resources,
			PPV4_QOS_DESC_IN_PAGE)) {
		QOS_LOG_ERR("Num resources %u must be aligned to %u\n",
			    qdev->hwconf.wred_total_avail_resources,
			    PPV4_QOS_DESC_IN_PAGE);
		rc = -EINVAL;
		goto out;
	}

	qdev->portsphys = free_ports_phys_init(
			qdev->reserved_ports,
			qdev->max_port,
			conf->reserved_ports,
			QOS_MAX_PORTS);
	if (!qdev->portsphys) {
		QOS_LOG_ERR("portsphys alloc failed\n");
		rc = -EINVAL;
		goto out;
	}

	rc = allocate_ddr_for_qm(qdev);
	if (rc) {
		QOS_FREE(qdev->portsphys);
		QOS_LOG_ERR("Could not allocate %u bytes for queue manager\n",
				qdev->hwconf.wred_total_avail_resources *
				PPV4_QOS_DESC_SIZE);
		goto out;
	}

	QOS_LOG_DEBUG("wred total resources\t%u\n",
		      qdev->hwconf.wred_total_avail_resources);
	QOS_LOG_DEBUG("qm_ddr_start\t\t0x%08X\n",
		      qdev->hwconf.qm_ddr_start);
	QOS_LOG_DEBUG("qm_num_of_pages\t\t%u\n",
		      qdev->hwconf.qm_num_pages);
	QOS_LOG_DEBUG("clock\t\t\t%u\n", qdev->hwconf.qos_clock);

	qdev->hwconf.wred_const_p = conf->wred_p_const;
	qdev->hwconf.wred_max_q_size = conf->wred_max_q_size;

	QOS_LOG_DEBUG("wred p const\t\t%u\n", qdev->hwconf.wred_const_p);
	QOS_LOG_DEBUG("wred max q size\t\t%u\n", qdev->hwconf.wred_max_q_size);

	rc = load_firmware(qdev, FIRMWARE_FILE);
	if (rc) {
		free_ddr_for_qm(qdev);
		QOS_FREE(qdev->portsphys);
		goto out;
	}

	if (qos_fw_log)
		create_init_logger_cmd(qdev, UC_LOGGER_LEVEL_INFO);
	else
		create_init_logger_cmd(qdev, UC_LOGGER_LEVEL_DEFAULT);

	create_init_qos_cmd(qdev);
	update_cmd_id(&qdev->drvcmds);
	transmit_cmds(qdev);

	qdev->initialized = 1;

	rc = 0;
out:
	QOS_UNLOCK(qdev);
	QOS_LOG_DEBUG("pp_qos_dev_init completed with %d\n", rc);
	return rc;
}

struct pp_qos_dev *create_qos_dev_desc(struct qos_dev_init_info *initinfo)
{
	struct pp_qos_dev *qdev;
	int rc;
	unsigned int id;

	id = initinfo->pl_data.id;
	if (id >= MAX_QOS_INSTANCES) {
		QOS_LOG_ERR("Illegal qos instance %u\n", id);
		return NULL;
	}

	if (qos_devs[id]) {
		QOS_LOG_ERR("qos instance %u already exists\n", id);
		return NULL;
	}

	_qos_init(initinfo->pl_data.max_port, &qdev);
	if (qdev) {
		qdev->hwconf.wred_prioritize_pop =
			initinfo->pl_data.wred_prioritize_pop;
		qdev->hwconf.fw_logger_start =
			initinfo->pl_data.fw_logger_start;
		qdev->hwconf.fw_stat = initinfo->pl_data.fw_stat;
		qdev->hwconf.qos_clock = initinfo->pl_data.qos_clock;

		qdev->hwconf.max_allowed_ddr_virt =
			initinfo->pl_data.max_allowed_ddr_virt;
		qdev->hwconf.max_allowed_ddr_phys =
			initinfo->pl_data.max_allowed_ddr_phys;

		qdev->hwconf.bwl_ddr_virt =
			(unsigned int *)initinfo->pl_data.bwl_ddr_virt;
		qdev->hwconf.bwl_ddr_phys =
			initinfo->pl_data.bwl_ddr_phys;

		qdev->hwconf.sbwl_ddr_virt =
			(unsigned int *)initinfo->pl_data.sbwl_ddr_virt;
		qdev->hwconf.sbwl_ddr_phys =
			initinfo->pl_data.sbwl_ddr_phys;

		memcpy(&qdev->fwcom, &initinfo->fwcom, sizeof(struct fw_com));
		rc = init_fwdata_internals(qdev);
		if (rc)
			goto err;
		qos_devs[id] = qdev;

		QOS_LOG_DEBUG("Initialized qos instance\nmax_port:\t\t%u\n",
			      qdev->max_port);
		QOS_LOG_DEBUG("fw_logger_start:\t0x%08X\n",
			      qdev->hwconf.fw_logger_start);
		QOS_LOG_DEBUG("fw_stat:\t\t0x%08X\n",
			      qdev->hwconf.fw_stat);
		QOS_LOG_DEBUG("cmdbuf:\t\t0x%08X\ncmdbuf size:\t\t%zu\n",
			      (unsigned int)(uintptr_t)qdev->fwcom.cmdbuf,
			      qdev->fwcom.cmdbuf_sz);
	} else {
		QOS_LOG_CRIT("Failed creating qos instance %u\n", id);
	}

	return qdev;
err:
	_qos_clean(qdev);
	QOS_LOG_CRIT("Failed creating qos instance %u\n", id);
	return NULL;
}

void remove_qos_instance(unsigned int id)
{
	if (id >= MAX_QOS_INSTANCES) {
		QOS_LOG_ERR("Illegal qos instance %u\n", id);
		return;
	}

	if (qos_devs[id] == NULL) {
		QOS_LOG_ERR("qos instance %u not exists\n", id);
		return;
	}

	_qos_clean(qos_devs[id]);
	qos_devs[id] = NULL;
	QOS_LOG_INFO("removed qos instance %u\n", id);
}

void qos_module_init(void)
{
	unsigned int i;

	for (i = 0 ; i < MAX_QOS_INSTANCES; ++i)
		qos_devs[i] = NULL;
}

int pp_qos_get_quanta(struct pp_qos_dev *qdev, unsigned int *quanta)
{
	struct qos_hw_info info = {0};

	QOS_LOCK(qdev);
	PP_QOS_ENTER_FUNC();

	create_get_sys_info_cmd(qdev, qdev->hwconf.fw_stat, &info);
	update_cmd_id(&qdev->drvcmds);
	transmit_cmds(qdev);

	*quanta = info.quanta;

	QOS_UNLOCK(qdev);
	return 0;
}
