/*
 * pp_qos_tests.c
 * Description: Packet Processor unit tests
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2018-2020 Intel Corporation
 */

#define pr_fmt(fmt) "[PP_QOS_TEST] %s:%d: " fmt, __func__, __LINE__

#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/pp_qos_api.h>
#include "../pput.h"
#include "pp_regs.h"
#include "pp_qos_utils.h"
#include "pp_qos_fw.h"
#include "pp_qos_utils.h"
#include "pp_qos_uc_defs.h"

#define QOS_TEST_NUM_RING_ADDR   (5)
#define QOS_NUM_RING_DESCRIPTORS (5)

static void       *ring_addr_virt[QOS_TEST_NUM_RING_ADDR];
static dma_addr_t ring_addr_phys[QOS_TEST_NUM_RING_ADDR];
static bool       syncq_enabled;

#ifdef CONFIG_PPV4_LGM
#define QOS_TEST_MAX_PORTS 256
#define QOS_TEST_MAX_QUEUES 512
#else
#define QOS_TEST_MAX_PORTS 128
#define QOS_TEST_MAX_QUEUES 256
#endif

#undef QOS_ASSERT
#define QOS_ASSERT(condition, format, arg...)                 \
do {                                                          \
	if (!(condition)) {                                   \
		QOS_LOG_CRIT("Assertion. " format, ##arg);    \
		return -1;                                    \
	}                                                     \
} while (0)

#define PPUT_FAIL_TEST(condition, format, arg...)             \
do {                                                          \
	if (!(condition)) {                                   \
		QOS_LOG_CRIT("Assertion. " format, ##arg);    \
		goto done;                                    \
	}                                                     \
} while (0)

#define PPUT_FAIL_RETURN(condition, format, arg...)           \
do {                                                          \
	if (!(condition)) {                                   \
		QOS_LOG_CRIT("Assertion. " format, ##arg);    \
		return PP_UT_FAIL;                            \
	}                                                     \
} while (0)

#define NUM_QUEUES_ON_QUERY (32U)
#define NUM_OF_TRIES (50U)
#define FW_CHECK_SYNC
/* #define QOS_TEST_ADVANCED_DBG */

#ifdef QOS_TEST_ADVANCED_DBG
#define DUMP_QOS_TREE(qdev) (qos_dbg_tree_show(qdev, true, NULL))
#else
#define DUMP_QOS_TREE(qdev)
#endif

static uint8_t ids[NUM_OF_NODES];
static uint8_t rlms[QOS_TEST_MAX_QUEUES];

static int qos_pre_test(void *data);
static int qos_post_test(void *data);
static int qos_internal_api_tests(void *data);
static int qos_advance_tests(void *data);
static int bw_reconfiguration_test(void *data);
static int qos_falcon_test(void *data);
static int qos_syncq_test(void *data);
static int bw_share_test(void *data);
static int four_ports_four_queues_test(void *data);
static int block_unblock_test(void *data);

#ifndef PP_QOS_DISABLE_CMDQ
static int stat_test(void *data);
static int info_test(void *data);
#endif
static int reposition_test(void *data);
static int reposition_internal_test(void *data);
static int wa_test(void *data);
static int wsp_test(void *data);
static int wa_min_test(void *data);
static int test_cmd_queue(void *data);
static int dual_rate_shaper_test(void *data);
static int same_priority_test(void *data);
static int arbitration_test(void *data);
static int enhanced_wsp_test(void *data);

static struct pp_qos_init_param conf;
static bool   restore_config;

static struct pp_test_t pp_qos_tests[] = {
	{
		.component    = PP_QOS_TESTS,
		.test_name    = "qos_pre_test",
		.level_bitmap = PP_UT_LEVEL_FULL |
				PP_UT_LEVEL_MUST_PASS |
				PP_UT_LEVEL_MUST_RUN,
		.test_data    = NULL,
		.fn           = qos_pre_test,
	},
	{
		.component    = PP_QOS_TESTS,
		.test_name    = "qos_syncq_test",
		.level_bitmap = PP_UT_LEVEL_FULL |
				PP_UT_LEVEL_MUST_PASS,
		.test_data    = NULL,
		.fn           = qos_syncq_test,
	},
	{
		.component    = PP_QOS_TESTS,
		.test_name    = "qos_internal_api_tests",
		.level_bitmap = PP_UT_LEVEL_FULL |
				PP_UT_LEVEL_MUST_PASS,
		.test_data    = NULL,
		.fn           = qos_internal_api_tests,
	},
	{
		.component    = PP_QOS_TESTS,
		.test_name    = "reposition_internal_test",
		.level_bitmap = PP_UT_LEVEL_REGRESSION |
				PP_UT_LEVEL_MUST_PASS,
		.test_data    = NULL,
		.fn           = reposition_internal_test,
	},
	{
		.component    = PP_QOS_TESTS,
		.test_name    = "qos_advance_tests",
		.level_bitmap = PP_UT_LEVEL_REGRESSION |
				PP_UT_LEVEL_MUST_PASS,
		.test_data    = NULL,
		.fn           = qos_advance_tests,
	},
	{
		.component    = PP_QOS_TESTS,
		.test_name    = "qos_falcon_test",
		.level_bitmap = PP_UT_LEVEL_REGRESSION |
				PP_UT_LEVEL_MUST_PASS,
		.test_data    = NULL,
		.fn           = qos_falcon_test,
	},
	{
		.component    = PP_QOS_TESTS,
		.test_name    = "bw_share_test",
		.level_bitmap = PP_UT_LEVEL_FULL |
				PP_UT_LEVEL_MUST_PASS,
		.test_data    = NULL,
		.fn           = bw_share_test,
	},
	{
		.component    = PP_QOS_TESTS,
		.test_name    = "four_ports_four_queues_test",
		.level_bitmap = PP_UT_LEVEL_FULL |
				PP_UT_LEVEL_MUST_PASS,
		.test_data    = NULL,
		.fn           = four_ports_four_queues_test,
	},
	{
		.component    = PP_QOS_TESTS,
		.test_name    = "block_unblock_test",
		.level_bitmap = PP_UT_LEVEL_FULL |
				PP_UT_LEVEL_MUST_PASS,
		.test_data    = NULL,
		.fn           = block_unblock_test,
	},
#ifndef PP_QOS_DISABLE_CMDQ
	{
		.component    = PP_QOS_TESTS,
		.test_name    = "info_test",
		.level_bitmap = PP_UT_LEVEL_FULL |
				PP_UT_LEVEL_MUST_PASS,
		.test_data    = NULL,
		.fn           = info_test,
	},
	{
		.component    = PP_QOS_TESTS,
		.test_name    = "stat_test",
		.level_bitmap = PP_UT_LEVEL_FULL |
				PP_UT_LEVEL_MUST_PASS,
		.test_data    = NULL,
		.fn           = stat_test,
	},
#endif
	{
		.component    = PP_QOS_TESTS,
		.test_name    = "reposition_test",
		.level_bitmap = PP_UT_LEVEL_REGRESSION |
				PP_UT_LEVEL_MUST_PASS,
		.test_data    = NULL,
		.fn           = reposition_test,
	},
	{
		.component    = PP_QOS_TESTS,
		.test_name    = "wa_test",
		.level_bitmap = PP_UT_LEVEL_REGRESSION |
				PP_UT_LEVEL_MUST_PASS,
		.test_data    = NULL,
		.fn           = wa_test,
	},
	{
		.component    = PP_QOS_TESTS,
		.test_name    = "wsp_test",
		.level_bitmap = PP_UT_LEVEL_REGRESSION |
				PP_UT_LEVEL_MUST_PASS,
		.test_data    = NULL,
		.fn           = wsp_test,
	},
	{
		.component    = PP_QOS_TESTS,
		.test_name    = "wa_min_test",
		.level_bitmap = PP_UT_LEVEL_REGRESSION |
				PP_UT_LEVEL_MUST_PASS,
		.test_data    = NULL,
		.fn           = wa_min_test,
	},
	{
		.component    = PP_QOS_TESTS,
		.test_name    = "test_cmd_queue",
		.level_bitmap = PP_UT_LEVEL_REGRESSION |
				PP_UT_LEVEL_MUST_PASS,
		.test_data    = NULL,
		.fn           = test_cmd_queue,
	},
	{
		.component    = PP_QOS_TESTS,
		.test_name    = "dual_rate_shaper_test",
		.level_bitmap = PP_UT_LEVEL_REGRESSION |
				PP_UT_LEVEL_MUST_PASS,
		.test_data    = NULL,
		.fn           = dual_rate_shaper_test,
	},
	{
		.component    = PP_QOS_TESTS,
		.test_name    = "same_priority_test",
		.level_bitmap = PP_UT_LEVEL_REGRESSION |
				PP_UT_LEVEL_MUST_PASS,
		.test_data    = NULL,
		.fn           = same_priority_test,
	},
	{
		.component    = PP_QOS_TESTS,
		.test_name    = "arbitration_test",
		.level_bitmap = PP_UT_LEVEL_REGRESSION |
				PP_UT_LEVEL_MUST_PASS,
		.test_data    = NULL,
		.fn           = arbitration_test,
	},
	{
		.component    = PP_QOS_TESTS,
		.test_name    = "bw_reconfiguration_test",
		.level_bitmap = PP_UT_LEVEL_REGRESSION |
				PP_UT_LEVEL_MUST_PASS,
		.test_data    = NULL,
		.fn           = bw_reconfiguration_test,
	},
	{
		.component    = PP_QOS_TESTS,
		.test_name    = "enhanced_wsp_test",
		.level_bitmap = PP_UT_LEVEL_REGRESSION |
				PP_UT_LEVEL_MUST_PASS,
		.test_data    = NULL,
		.fn           = enhanced_wsp_test,
	},
	{
		.component    = PP_QOS_TESTS,
		.test_name    = "qos_post_test",
		.level_bitmap = PP_UT_LEVEL_FULL |
				PP_UT_LEVEL_MUST_PASS |
				PP_UT_LEVEL_MUST_RUN,
		.test_data    = NULL,
		.fn           = qos_post_test,
	},
};

static s32 __must_check debug_verify_octet_usage(struct pp_octets *octets,
						 u32 octet,
						 u32 usage)
{
	struct list_head *octet_link;
	struct list_head *pos;
	u32 tmp;
	int found;

	tmp = octet_get_use_count(octets, octet);
	QOS_ASSERT(tmp == usage, "Octet %u count is %u and not %u\n", octet,
		   tmp, usage);

	octet_link = &(octets->info[octet].list);
	if (octet <= octets->last_port_octet) {
		QOS_ASSERT(list_empty(octet_link),
			   "Octet %u is on a list though its a port octet\n",
			   octet);
		return 0;
	}

	found = 0;
	for (pos = octet_link->next; pos != octet_link; pos = pos->next) {
		if (pos >= octets->lists && pos <= octets->lists + 8) {
			tmp = (pos - octets->lists);
			QOS_ASSERT(tmp == usage,
				   "Octet is on list %u but should be on %u\n",
				   tmp, usage);
			found = 1;
			break;
		} else if (pos > &octets->lists[8]) {
			break;
		}
	}

	QOS_ASSERT(found, "Did not find list of octet %u\n", octet);

	return 0;
}

static s32 __must_check check_resources_usage(struct pp_qos_dev *qdev)
{
	int i;
	int id;
	int rlm;
	struct qos_node *node;

	memset(ids, 0, NUM_OF_NODES);
	memset(rlms, 0, QOS_TEST_MAX_QUEUES);

	node = get_node_from_phy(qdev->nodes, 0);
	QOS_ASSERT(node, "get_node_from_phy(%u) returned NULL\n", 0);
	for (i = 0; i < NUM_OF_NODES; ++i) {
		if (node_used(node)) {
			id = get_id_from_phy(qdev->mapping, i);
			QOS_ASSERT(ids[id] == 0,
				   "Id %d of phy %d already found\n",
				   id, i);
			ids[id] = 1;
			if (node->type == TYPE_QUEUE) {
				if (!node->data.queue.is_alias) {
					rlm = node->data.queue.rlm;
					QOS_ASSERT(rlms[rlm] == 0,
						   "Rlm %d phy %d already found\n",
						   rlm, i);
					rlms[rlm] = 1;
				}
			}
		}

		if (qdev->ids->top > i) {
			id = qdev->ids->data[i];
			QOS_ASSERT(ids[id] == 0,
				   "Id %d on free ids pool already found\n",
				   id);
			ids[id] = 1;
		}

		if (qdev->rlms->top > i) {
			rlm = qdev->rlms->data[i];
			QOS_ASSERT(rlms[rlm] == 0,
				   "Rlm %d on free rlms pool already found\n",
				   rlm);
			rlms[rlm] = 1;
		}

		++node;
	}

	for (i = 0; i < NUM_OF_NODES; ++i) {
		QOS_ASSERT(ids[i], "Id %d was not found\n", i);
		QOS_ASSERT(i >= QOS_TEST_MAX_QUEUES || rlms[i],
			   "Rlm %d was not found\n", i);
	}

	return 0;
}

/*
 * Check the following for each id:
 *   If id is mapped to a valid phy
 *	- Node designated by this phy is marked as used
 *	- The inverse mapping from phy maps to the same id
 *
 * Check the following for each phy:
 *   If phy designates a used node
 *     - It is mapped to valid id
 *     - The inverse mapping from id maps to the same phy
 *
 */
static s32 __must_check check_mapping(struct pp_qos_dev *qdev)
{
	uint16_t phy;
	uint16_t id;
	struct qos_node *node;
	unsigned int i;

	for (i = 0; i < NUM_OF_NODES; ++i) {
		phy = get_phy_from_id(qdev->mapping, i);
		if (QOS_PHY_VALID(phy)) {
			node = get_node_from_phy(qdev->nodes, phy);
			QOS_ASSERT(node,
				   "get_node_from_phy(%u) returned NULL\n",
				   phy);
			QOS_ASSERT(node_used(node),
				   "Id %u is mapped to unused node %u\n",
				   i, phy);
			id = get_id_from_phy(qdev->mapping, phy);
			QOS_ASSERT(id == i,
				   "Id %u is mapped to phy %u, but the latter is mapped to %u\n",
				   i, phy, id);
		}

		node = get_node_from_phy(qdev->nodes, i);
		QOS_ASSERT(node, "get_node_from_phy(%u) returned NULL\n", i);
		id = get_id_from_phy(qdev->mapping, i);
		QOS_ASSERT(!node_used(node) || QOS_ID_VALID(id),
			   "Node %u is used but has invalid id\n", i);
		if (QOS_ID_VALID(id)) {
			phy = get_phy_from_id(qdev->mapping, id);
			QOS_ASSERT(phy == i,
				   "Phy %u is mapped to id %u, but the latter is mapped to %u\n",
				   i, id, phy);
		}
	}

	return 0;
}

 /*
  * Check following tests for each node:
  * If node is a parent
  * All the nodes it claims are its children,
  * are indeed children and have it as their parent
  * If its a WSP parent then its children are ordered correctly
  * The sum of share of its children does not exceed 100
  *
  * If node is a child
  *     - It is indeed recognized by its claimed parent as a child
  *
  */
static s32 __must_check check_parent_children(struct pp_qos_dev *qdev)
{
	int phy;
	int i;
	struct qos_node *node;
	struct qos_node *cur;
	int first_child_phy;
	int last_child_phy;
	int share;

	for (phy = 0; phy < NUM_OF_NODES; ++phy) {
		node = get_node_from_phy(qdev->nodes, phy);
		QOS_ASSERT(node, "get_node_from_phy(%u) returned NULL\n", phy);
		if (node_parent(node)) {
			i = node->parent_prop.num_of_children;
			QOS_ASSERT(i >= 0,
				   "Illegal number of children %d for node %d\n",
				   i, phy);
			if (i) {
				cur = get_node_from_phy(
					qdev->nodes,
					node->parent_prop.first_child_phy);
			}

			for (; i > 0; --i) {
				if (!node_child(cur)) {
					QOS_LOG_ERR(
						"Node %u claims that %u is its child but the latter is not a child (or marked unused)\n",
						phy,
						get_phy_from_node(
							qdev->nodes, cur));
					return -1;
				}

				if (cur->child_prop.parent_phy != phy) {
					QOS_LOG_ERR(
						"Node %d has node %u as a child, but the latter has node %u as its parent\n",
						phy,
						get_phy_from_node(
							qdev->nodes, cur),
						cur->child_prop.parent_phy);
					return -1;
				}

				++cur;
			}

			if (node_internal(node)) {
				share = get_children_bandwidth_share(qdev,
								     node);
				if (share != node->child_prop.virt_bw_share) {
					QOS_LOG_ERR(
					"Internal scheduler: sum bandwidth children is %d while parent is %d\n",
					share,
					node->child_prop.virt_bw_share);
					return -1;
				}
			}
		}

		if (node_child(node)) {
			cur = get_node_from_phy(qdev->nodes,
						node->child_prop.parent_phy);
			if (!node_parent(cur)) {
				QOS_LOG_ERR(
					"Node %u claims that %u is its parent, but the latter is not a parent (or marked unused)\n",
					phy,
					get_phy_from_node(qdev->nodes, cur));
				return -1;
			}

			first_child_phy = cur->parent_prop.first_child_phy;
			last_child_phy = first_child_phy +
				cur->parent_prop.num_of_children - 1;

			if ((phy < first_child_phy) || (phy > last_child_phy)) {
				QOS_LOG_ERR(
					"Node %d has node %u as a parent, but the latter children are %u..%u\n",
					phy,
					get_phy_from_node(qdev->nodes, cur),
					first_child_phy, last_child_phy);
				return -1;
			}
		}
		++node;
	}

	return 0;
}

/*
 * Check following for each octet:
 * Number of nodes marked as used on each octet matches octet usage
 * Ports' octets are not enqueued
 * Non ports' octets are enqueued on a list which match their usage count
 * No unused node interleaves sequence of used nodes
 *
 */
static s32 __must_check check_octets(struct pp_qos_dev *qdev)
{
	int i;
	int j;
	int unused_seen;
	uint8_t usage;
	int last;
	struct qos_node *node;
	s32 ret;

	last = octet_of_phy(qdev->init_params.max_ports - 1);

	for (i = 0; i < NUM_OF_OCTETS; ++i) {
		node = get_node_from_phy(qdev->nodes, 8 * i);
		usage = 0;
		unused_seen = 0;
		for (j = 0; j < 8; ++j) {
			if (node_used(node)) {
				if (i > last) {
					QOS_ASSERT(!unused_seen,
						   "Node %u is used after previous node in same octet was unused\n",
						   i * 8 + j);
				}
				usage++;
			} else {
				unused_seen = 1;
			}
			node++;
		}
		ret = debug_verify_octet_usage(qdev->octets, i, usage);
		if (ret)
			return ret;
	}

	return 0;
}

static s32 __must_check check_consistency(struct pp_qos_dev *qdev)
{
	s32 ret;

	QOS_ASSERT(!QOS_SPIN_IS_LOCKED(qdev), "Lock is taken\n");
	ret = check_octets(qdev);
	if (ret)
		return ret;

	ret = check_parent_children(qdev);
	if (ret)
		return ret;

	ret = check_mapping(qdev);
	if (ret)
		return ret;

	return 0;
}

static s32 __must_check
check_consistency_with_resources(struct pp_qos_dev *qdev)
{
	s32 ret;

	ret = check_consistency(qdev);
	if (ret)
		return ret;

	ret = check_resources_usage(qdev);
	if (ret)
		return ret;

	return 0;
}

static struct pp_qos_dev * __must_check qos_restart(u32 instance_id)
{
	static struct pp_qos_init_param new_conf;
	struct pp_qos_dev *qdev;
	u32 port_idx;
	s32 ret;

	pr_debug("\n");

	qdev = pp_qos_dev_open(instance_id);
	if (unlikely(ptr_is_null(qdev)))
		return NULL;

	QOS_LOG_DEBUG("Command queue watermark:\n\tcmd_q: %u\n\tpend_q: %u\n",
		      (u32)cmd_queue_watermark_get(qdev->drvcmds.cmdq),
		      (u32)cmd_queue_watermark_get(qdev->drvcmds.pendq));

	qos_dev_clean(qdev, false);
	qdev = pp_qos_dev_open(instance_id);
	if (unlikely(ptr_is_null(qdev)))
		return NULL;

	memcpy(&new_conf, &conf, sizeof(new_conf));
	memset(new_conf.reserved_ports, 0, sizeof(new_conf.reserved_ports));
	new_conf.reserved_ports[0] = 1;
	new_conf.reserved_ports[1] = 1;
	new_conf.reserved_ports[2] = 1;
	new_conf.reserved_ports[3] = 1;
	new_conf.reserved_ports[7] = 1;
	new_conf.reserved_ports[12] = 1;

	for (port_idx = 25; port_idx < 89; port_idx++)
		new_conf.reserved_ports[port_idx] = 1;

	new_conf.reserved_ports[89] = 1;
	new_conf.reserved_ports[97] = 1;
	new_conf.reserved_ports[105] = 1;

	new_conf.reserved_queues = 0;

	if (!syncq_enabled)
		new_conf.num_syncqs = 0;

	ret = qos_dev_init(qdev, &new_conf, false);
	if (ret) {
		pr_err("qos_dev_init failed\n");
		return NULL;
	}

#ifdef DUMP_LOG_ON_FAILURE
	create_init_logger_cmd(qdev,
			       UC_LOGGER_LEVEL_FATAL,
			       UC_LOGGER_MODE_NONE);
	update_cmd_id(&qdev->drvcmds);
	ret = transmit_cmds(qdev);
	if (ret)
		QOS_LOG_ERR("transmit_cmds failed (%d)\n", ret);
#endif

	return qdev;
}

static s32 __must_check expected_queues(struct pp_qos_dev *qdev,
					int phy, unsigned int exp)
{
	unsigned int num;

	get_node_queues(qdev, phy, NULL, 0, &num);
	QOS_ASSERT(exp == num,
		   "Expected %u queues on phy %d but got %u\n",
		   exp, phy, num);
	return 0;
}

static s32 __must_check expected_id_phy(const struct pp_qos_dev *qdev,
					int id, int eid, int ephy)
{
	int phy;

	phy = get_phy_from_id(qdev->mapping, id);
	QOS_ASSERT(id == eid, "Got id %d expected %d\n", id, eid);
	QOS_ASSERT(phy == ephy, "Got phy %d expected %d\n", phy, ephy);
	return 0;
}

static void create_nodes(struct pp_qos_dev *qdev,
			 uint16_t start_phy, int count)
{
	int i;
	uint16_t id;
	struct qos_node *node;

	nodes_modify_used_status(qdev, start_phy, count, 1);
	node = get_node_from_phy(qdev->nodes, start_phy);
	if (!node) {
		QOS_LOG_ERR("get_node_from_phy(%u) returned NULL\n",
			    start_phy);
		return;
	}

	QOS_LOG_DEBUG("Creating nodes %d - %d\n",
		      start_phy, start_phy + count - 1);

	for (i = start_phy; i < start_phy + count; ++i) {
		id = pp_pool_get(qdev->ids);
		map_id_phy(qdev->mapping, id, i);
		node->type = TYPE_UNKNOWN;
		++node;
	}
}

static void destroy_nodes(struct pp_qos_dev *qdev,
			  uint16_t start_phy, int count)
{
	int i;
	uint16_t id;

	nodes_modify_used_status(qdev, start_phy, count, 0);

	QOS_LOG_DEBUG("Destroying nodes %d - %d\n",
		      start_phy, start_phy + count - 1);

	for (i = start_phy; i < start_phy + count; ++i) {
		id = get_id_from_phy(qdev->mapping, i);
		map_invalidate_id_phy(qdev->mapping, id);
		pp_pool_put(qdev->ids, id);
	}
}

static void config_parent_node(struct pp_qos_dev *qdev,
			       uint16_t phy,
			       enum node_type type,
			       enum pp_qos_arbitration arb,
			       uint16_t first_child,
			       int num)
{
	struct qos_node *node;

	QOS_LOG_DEBUG("Config Parent %d with children %d - %d\n",
		      phy, first_child, first_child + num - 1);

	node = get_node_from_phy(qdev->nodes, phy);
	if (!node) {
		QOS_LOG_ERR("get_node_from_phy(%u) returned NULL\n",
			    phy);
		return;
	}

	node->type = type;
	node->parent_prop.arbitration = arb;
	node->parent_prop.first_child_phy = first_child;
	node->parent_prop.num_of_children = num;
}

static void config_child_node(struct pp_qos_dev *qdev,
			      uint16_t phy,
			      enum node_type type,
			      uint16_t parent_phy)
{
	struct qos_node *node;

	node = get_node_from_phy(qdev->nodes, phy);
	if (!node) {
		QOS_LOG_ERR("get_node_from_phy(%u) returned NULL\n",
			    phy);
		return;
	}

	node->type = type;

	if (QOS_PHY_VALID(parent_phy))
		node->child_prop.parent_phy = parent_phy;
}

static s32 test_modify_used_status(struct pp_qos_dev *qdev)
{
	s32 ret;

	ret = debug_verify_octet_usage(qdev->octets, 2, 0);
	if (ret)
		return ret;

	ret = debug_verify_octet_usage(qdev->octets, 3, 0);
	if (ret)
		return ret;

	ret = debug_verify_octet_usage(qdev->octets, 4, 0);
	if (ret)
		return ret;

	create_nodes(qdev, 18, 15);
	ret = debug_verify_octet_usage(qdev->octets, 2, 6);
	if (ret)
		return ret;

	ret = debug_verify_octet_usage(qdev->octets, 3, 8);
	if (ret)
		return ret;

	ret = debug_verify_octet_usage(qdev->octets, 4, 1);
	if (ret)
		return ret;

	destroy_nodes(qdev, 25, 2);
	ret = debug_verify_octet_usage(qdev->octets, 2, 6);
	if (ret)
		return ret;

	ret = debug_verify_octet_usage(qdev->octets, 3, 6);
	if (ret)
		return ret;

	ret = debug_verify_octet_usage(qdev->octets, 4, 1);
	if (ret)
		return ret;

	destroy_nodes(qdev, 18, 7);
	ret = debug_verify_octet_usage(qdev->octets, 2, 0);
	if (ret)
		return ret;

	ret = debug_verify_octet_usage(qdev->octets, 3, 5);
	if (ret)
		return ret;

	ret = debug_verify_octet_usage(qdev->octets, 4, 1);
	if (ret)
		return ret;

	destroy_nodes(qdev, 27, 6);
	ret = debug_verify_octet_usage(qdev->octets, 2, 0);
	if (ret)
		return ret;

	ret = debug_verify_octet_usage(qdev->octets, 3, 0);
	if (ret)
		return ret;

	ret = debug_verify_octet_usage(qdev->octets, 4, 0);
	if (ret)
		return ret;

	ret = check_consistency(qdev);
	if (ret)
		return ret;

	return 0;
}

static s32 test_phy_allocation_not_full_octet(struct pp_qos_dev *qdev)
{
	struct qos_node *base;
	uint16_t phy;
	uint16_t expected_phy;
	uint16_t id;
	s32 ret;

	create_nodes(qdev, 0, 2);
	create_nodes(qdev, 16, 6);

	base = get_node_from_phy(qdev->nodes, 0);

	// Port (Node 1)
	config_parent_node(qdev, 1, TYPE_PORT, PP_QOS_ARBITRATION_WRR, 16, 2);
	config_parent_node(qdev, 16, TYPE_SCHED, PP_QOS_ARBITRATION_WSP, 18, 2);
	config_child_node(qdev, 16, TYPE_SCHED, 1);
	config_parent_node(qdev, 17, TYPE_SCHED, PP_QOS_ARBITRATION_WRR, 20, 2);
	config_child_node(qdev, 17, TYPE_SCHED, 1);

	// 2 Queues of first sched (Nodes, 18, 19)
	//
	config_child_node(qdev, 18, TYPE_QUEUE, 16);
	config_child_node(qdev, 19, TYPE_QUEUE, 16);

	// 2 Queues of second sched (Nodes, 20, 21)
	config_child_node(qdev, 20, TYPE_QUEUE, 17);
	config_child_node(qdev, 21, TYPE_QUEUE, 17);
	ret = check_consistency(qdev);
	if (ret)
		return ret;

	expected_phy = 20;
	phy = phy_alloc_by_parent(qdev, base + 16, 1);
	QOS_ASSERT(phy == expected_phy,
		   "Expected phy for new node %d but got %d\n",
		   expected_phy, phy);
	config_child_node(qdev, phy, TYPE_QUEUE, QOS_INVALID_PHY);
	id = pp_pool_get(qdev->ids);
	map_id_phy(qdev->mapping, id, phy);
	ret = check_consistency(qdev);
	if (ret)
		return ret;

	destroy_nodes(qdev, 18, 1);
	octet_nodes_shift(qdev, 19, 4, -1);
	ret = check_consistency(qdev);
	if (ret)
		return ret;

	expected_phy = 18;
	phy = phy_alloc_by_parent(qdev, base + 16, 0);
	QOS_ASSERT(phy == expected_phy,
		   "Expected phy for new node %d but got %d\n",
		   expected_phy, phy);
	config_child_node(qdev, phy, TYPE_QUEUE, QOS_INVALID_PHY);
	id = pp_pool_get(qdev->ids);
	map_id_phy(qdev->mapping, id, phy);
	ret = check_consistency(qdev);
	if (ret)
		return ret;

	destroy_nodes(qdev, phy, 1);
	octet_nodes_shift(qdev, expected_phy + 1, 4, -1);
	ret = check_consistency(qdev);
	if (ret)
		return ret;

	expected_phy = 20;
	phy = phy_alloc_by_parent(qdev, base + 16, 6);
	QOS_ASSERT(phy == expected_phy,
		   "Expected phy for new node %d but got %d\n",
		   expected_phy, phy);
	config_child_node(qdev, phy, TYPE_QUEUE, QOS_INVALID_PHY);
	id = pp_pool_get(qdev->ids);
	map_id_phy(qdev->mapping, id, phy);
	ret = check_consistency(qdev);
	if (ret)
		return ret;

	destroy_nodes(qdev, phy, 1);
	config_parent_node(qdev, 16, TYPE_SCHED, PP_QOS_ARBITRATION_WSP, 18, 2);
	octet_nodes_shift(qdev, expected_phy + 1, 2, -1);
	ret = check_consistency(qdev);
	if (ret)
		return ret;

	expected_phy = 22;
	phy = phy_alloc_by_parent(qdev, base + 17, 1);
	QOS_ASSERT(phy == expected_phy,
		   "Expected phy for new node %d but got %d\n",
		   expected_phy, phy);
	id = pp_pool_get(qdev->ids);
	map_id_phy(qdev->mapping, id, phy);
	ret = check_consistency(qdev);
	if (ret)
		return ret;

	destroy_nodes(qdev, 0, 2);
	destroy_nodes(qdev, 16, 7);
	ret = check_consistency(qdev);
	if (ret)
		return ret;

	return 0;
}

static s32 test_phy_allocation_full_octet(struct pp_qos_dev *qdev)
{
	struct qos_node *base;
	uint16_t phy;
	uint16_t sched;
	uint16_t sched2;
	uint16_t expected_phy;
	uint16_t id;
	struct qos_node *node;
	s32 ret;

	create_nodes(qdev, 0, 2);
	create_nodes(qdev, 16, 8);
	base = get_node_from_phy(qdev->nodes, 0);

	/* 1 => 16,17 */
	/* 16 => 18,19,20 */
	/* 17 => 21,22,23 */
	// Port (Node 1) and 2 scheds - 16(WSP), 17(WRR)
	config_parent_node(qdev, 1, TYPE_PORT, PP_QOS_ARBITRATION_WRR, 16, 2);
	config_parent_node(qdev, 16, TYPE_SCHED, PP_QOS_ARBITRATION_WSP, 18, 3);
	config_child_node(qdev, 16, TYPE_SCHED, 1);
	config_parent_node(qdev, 17, TYPE_SCHED, PP_QOS_ARBITRATION_WRR, 21, 3);
	config_child_node(qdev, 17, TYPE_SCHED, 1);

	// 3 Queues of first sched (Nodes, 18, 19, 20)
	config_child_node(qdev, 18, TYPE_QUEUE, 16);
	config_child_node(qdev, 19, TYPE_QUEUE, 16);
	config_child_node(qdev, 20, TYPE_QUEUE, 16);

	// 3 Queues of second sched (Nodes, 21, 22, 23)
	config_child_node(qdev, 23, TYPE_QUEUE, 17);
	config_child_node(qdev, 21, TYPE_QUEUE, 17);
	config_child_node(qdev, 22, TYPE_QUEUE, 17);
	ret = check_consistency(qdev);
	if (ret)
		return ret;

	sched = get_id_from_phy(qdev->mapping, 16);
	sched2 = get_id_from_phy(qdev->mapping, 17);

	expected_phy = 16;
	/* 16,17 --> 256,257 */
	/* 18-23 --> 17-22 */
	/* Now we have phys 16,17 free */
	/* 16 - highest priority new child of 256 */
	phy = phy_alloc_by_parent(qdev, base + 16, 0);
	QOS_ASSERT(phy == expected_phy,
		   "Expected phy for new node %d but got %d\n",
		   expected_phy, phy);
	config_child_node(qdev, phy, TYPE_QUEUE, QOS_INVALID_PHY);
	id = pp_pool_get(qdev->ids);
	map_id_phy(qdev->mapping, id, phy);
	ret = check_consistency(qdev);
	if (ret)
		return ret;

	/* Deleting phy 16 */
	destroy_nodes(qdev, phy, 1);
	/* Shifting left 6 nodes from phy 17 */
	/* 17-22 --> 16-21 */
	octet_nodes_shift(qdev, expected_phy + 1, 6, -1);
	ret = check_consistency(qdev);
	if (ret)
		return ret;

	/* Shifting 2right 6 nodes from phy 16 */
	/* 16-21 --> 18-23 */
	/* Now we have phys 16,17 free */
	octet_nodes_shift(qdev, 16, 6, 2);
	phy = get_phy_from_id(qdev->mapping, sched);
	/* Move schedulers 256,257 --> 16,17 */
	/* We are back to initial config */
	nodes_move(qdev, 16, phy, 2);
	ret = check_consistency(qdev);
	if (ret)
		return ret;

	expected_phy = 19;
	/* 16,17 --> 264,265 */
	/* 18-20 --> 16-18 */
	/* 21-23 --> 20-22 */
	/* Now we have phys 19 free */
	/* 19 - lowest priority new child of 264 */
	phy = phy_alloc_by_parent(qdev, base + 16, 1);
	QOS_ASSERT(phy == expected_phy,
		   "Expected phy for new node %d but got %d\n",
		   expected_phy, phy);
	//config_child_node(qdev, phy, TYPE_QUEUE, QOS_INVALID_PHY);
	id = pp_pool_get(qdev->ids);
	map_id_phy(qdev->mapping, id, phy);
	ret = check_consistency(qdev);
	if (ret)
		return ret;

	/* Deleting phy 19 */
	//destroy_nodes(qdev, phy, 1);
	QOS_LOG_DEBUG("Removing node of phy %d\n", phy);
	map_id_reserved(qdev->rlm_mapping, id);
	node_remove(qdev,  get_node_from_phy(qdev->nodes, phy));

	/* Shifting left 3 nodes from phy 20 */
	/* 20-22 --> 19-21 */
	octet_nodes_shift(qdev, expected_phy + 1, 3, -1);
	ret = check_consistency(qdev);
	if (ret)
		return ret;

	/* Shifting 2right 6 nodes from phy 16 */
	/* 16-21 --> 18-23 */
	/* Now we have phys 16,17 free */
	octet_nodes_shift(qdev, 16, 6, 2);
	phy = get_phy_from_id(qdev->mapping, sched);
	/* Move schedulers 264,265 --> 16,17 */
	/* We are back to initial config */
	nodes_move(qdev, 16, phy, 2);
	ret = check_consistency(qdev);
	if (ret)
		return ret;

	expected_phy = 19;
	phy = phy_alloc_by_parent(qdev, base + 16, 6);
	QOS_ASSERT(phy == expected_phy,
		   "Expected phy for new node %d but got %d\n",
		   expected_phy, phy);
	config_child_node(qdev, phy, TYPE_QUEUE, QOS_INVALID_PHY);
	id = pp_pool_get(qdev->ids);
	map_id_phy(qdev->mapping, id, phy);
	ret = check_consistency(qdev);
	if (ret)
		return ret;

	node = get_node_from_phy(qdev->nodes,
				 get_phy_from_id(qdev->mapping, sched));
	QOS_ASSERT(node, "get_node_from_phy(%u) returned NULL\n",
		   get_phy_from_id(qdev->mapping, sched));
	node->parent_prop.num_of_children--;
	destroy_nodes(qdev, phy, 1);
	octet_nodes_shift(qdev, expected_phy + 1, 3, -1);
	ret = check_consistency(qdev);
	if (ret)
		return ret;

	octet_nodes_shift(qdev, 16, 6, 2);
	phy = get_phy_from_id(qdev->mapping, sched);
	nodes_move(qdev, 16, phy, 2);
	ret = check_consistency(qdev);
	if (ret)
		return ret;

	expected_phy = 22;
	phy = phy_alloc_by_parent(qdev, base + 17, 0);
	QOS_ASSERT(phy == expected_phy,
		   "Expected phy for new node %d but got %d\n",
		   expected_phy, phy);
	id = pp_pool_get(qdev->ids);
	map_id_phy(qdev->mapping, id, phy);
	ret = check_consistency(qdev);
	if (ret)
		return ret;

	node = get_node_from_phy(qdev->nodes,
				 get_phy_from_id(qdev->mapping, sched2));
	QOS_ASSERT(node, "get_node_from_phy(%u) returned NULL\n",
		   get_phy_from_id(qdev->mapping, sched2));
	node->parent_prop.num_of_children--;
	destroy_nodes(qdev, phy, 1);
	octet_nodes_shift(qdev, 16, 6, 2);
	phy = get_phy_from_id(qdev->mapping, sched);
	nodes_move(qdev, 16, phy, 2);
	ret = check_consistency(qdev);
	if (ret)
		return ret;

	destroy_nodes(qdev, 16, 8);
	destroy_nodes(qdev, 0, 2);
	ret = check_consistency(qdev);
	if (ret)
		return ret;

	return 0;
}

static s32 test_phy_allocation_full_octet_2(struct pp_qos_dev *qdev)
{
	struct qos_node *base;
	uint16_t phy;
	uint16_t queue;
	uint16_t expected_phy;
	uint16_t id;
	s32 ret;

	create_nodes(qdev, 0, 2);
	create_nodes(qdev, 16, 8);
	base = get_node_from_phy(qdev->nodes, 0);

	// Port (Node 1) and 2 scheds - 16(WSP), 17(WRR)
	config_parent_node(qdev, 1, TYPE_PORT, PP_QOS_ARBITRATION_WRR, 16, 2);
	config_parent_node(qdev, 16, TYPE_SCHED, PP_QOS_ARBITRATION_WSP, 18, 5);
	config_child_node(qdev, 16, TYPE_SCHED, 1);
	config_parent_node(qdev, 17, TYPE_SCHED, PP_QOS_ARBITRATION_WRR, 23, 1);
	config_child_node(qdev, 17, TYPE_SCHED, 1);

	// 4 Queues of first sched (Nodes, 18, 19, 20, 21, 22)
	config_child_node(qdev, 18, TYPE_QUEUE, 16);
	config_child_node(qdev, 19, TYPE_QUEUE, 16);
	config_child_node(qdev, 20, TYPE_QUEUE, 16);
	config_child_node(qdev, 21, TYPE_QUEUE, 16);
	config_child_node(qdev, 22, TYPE_QUEUE, 16);

	// 1 Queues of second sched (Node 23)
	config_child_node(qdev, 23, TYPE_QUEUE, 17);
	queue = get_id_from_phy(qdev->mapping, 23);
	ret = check_consistency(qdev);
	if (ret)
		return ret;

	expected_phy = 18;
	phy = phy_alloc_by_parent(qdev, base + 16, 0);
	QOS_ASSERT(phy == expected_phy,
		   "Expected phy for new node %d but got %d\n",
		   expected_phy, phy);
	config_child_node(qdev, phy, TYPE_QUEUE, QOS_INVALID_PHY);
	id = pp_pool_get(qdev->ids);
	map_id_phy(qdev->mapping, id, phy);
	destroy_nodes(qdev, phy, 1);
	octet_nodes_shift(qdev, expected_phy + 1, 5, -1);
	ret = check_consistency(qdev);
	if (ret)
		return ret;

	phy = get_phy_from_id(qdev->mapping, queue);
	nodes_move(qdev, 23, phy, 1);
	ret = check_consistency(qdev);
	if (ret)
		return ret;

	//
	// change config to 3 schedulers, first has 1 child, second 4 and third
	// none
	//
	config_parent_node(qdev, 1, TYPE_PORT, PP_QOS_ARBITRATION_WRR, 16, 3);
	config_parent_node(qdev, 16, TYPE_SCHED, PP_QOS_ARBITRATION_WSP, 19, 1);
	config_parent_node(qdev, 17, TYPE_SCHED, PP_QOS_ARBITRATION_WRR, 20, 4);
	config_child_node(qdev, 18, TYPE_SCHED, 1);
	config_child_node(qdev, 19, TYPE_QUEUE, 16);
	config_child_node(qdev, 23, TYPE_QUEUE, 17);
	config_child_node(qdev, 20, TYPE_QUEUE, 17);
	config_child_node(qdev, 21, TYPE_QUEUE, 17);
	config_child_node(qdev, 22, TYPE_QUEUE, 17);
	ret = check_consistency(qdev);
	if (ret)
		return ret;

	queue = get_id_from_phy(qdev->mapping, 19);
	phy = phy_alloc_by_parent(qdev, base + 16, 0);
	expected_phy = get_phy_from_id(qdev->mapping, queue) - 1;
	QOS_ASSERT(phy ==  expected_phy,
		   "Expected phy for new node %d but got %d\n",
		   expected_phy, phy);
	config_child_node(qdev, phy, TYPE_QUEUE, QOS_INVALID_PHY);
	id = pp_pool_get(qdev->ids);
	map_id_phy(qdev->mapping, id, phy);
	destroy_nodes(qdev, phy, 1);
	octet_nodes_shift(qdev, phy + 1, 1, -1);
	ret = check_consistency(qdev);
	if (ret)
		return ret;

	destroy_nodes(qdev, 16, 7);
	destroy_nodes(qdev, 0, 2);
	destroy_nodes(qdev, phy, 1);
	ret = check_consistency(qdev);
	if (ret)
		return ret;

	return 0;
}

static int test_ports(struct pp_qos_dev *qdev)
{
	struct pp_qos_port_conf conf;
	struct pp_qos_queue_conf confq;
	unsigned int id1;
	unsigned int id2;
	int tmp;
	struct qos_node *node;
	struct pp_qos_port_info info;
	uint16_t ids[5];
	unsigned int tmp2;
	u32 qid[3];
	s32 ret;

	/* Allocate port with arbitrary phy */
	tmp = pp_qos_port_allocate(qdev, ALLOC_PORT_ID, &id1);
	QOS_ASSERT(tmp == 0, "could not allocate qos port\n");
	ret = expected_id_phy(qdev, id1, 0, 4);
	if (ret)
		return ret;

	pp_qos_port_conf_set_default(&conf);
	conf.port_parent_prop.arbitration = PP_QOS_ARBITRATION_WRR;
	conf.ring_address = ring_addr_phys[0];
	conf.ring_size = 1;
	QOS_ASSERT(pp_qos_port_set(qdev, id1, &conf) == 0,
		   "Could not set port\n");

	node = get_node_from_phy(qdev->nodes,
				 get_phy_from_id(qdev->mapping, id1));
	conf.ring_size = 3;
	QOS_ASSERT(pp_qos_port_set(qdev, id1, &conf) == 0,
		   "Could not set port\n");
	QOS_ASSERT(port_cfg_valid(qdev, node, node),
		   "Configuration not valid\n");

	conf.ring_size = 0;
	QOS_ASSERT(pp_qos_port_set(qdev, id1, &conf) == -EINVAL,
		   "Port setting should have failed\n");
	QOS_ASSERT(port_cfg_valid(qdev, node, node),
		   "Configuration not valid\n");

	/* Allocate port with certain phy */
	tmp = pp_qos_port_allocate(qdev, 10, &id2);
	QOS_ASSERT(tmp == -EINVAL, "Port allocation should have failed\n");

	tmp = pp_qos_port_allocate(qdev, 12, &id2);
	QOS_ASSERT(tmp == 0, "Port allocation failed\n");

	conf.ring_size = 1;
	tmp = pp_qos_port_set(qdev, id2, &conf);
	QOS_ASSERT(tmp == 0, "Port set failed\n");

	ret = expected_id_phy(qdev, id2, 1, 12);
	if (ret)
		return ret;

	tmp = pp_qos_port_allocate(qdev, 12, &id2);
	QOS_ASSERT(tmp == -EBUSY, "Port allocation should have failed\n");

	/* Configure 3 queues */
	pp_qos_queue_conf_set_default(&confq);
	confq.queue_child_prop.parent = id2;

	ret = pp_qos_queue_allocate(qdev, &qid[0]);
	QOS_ASSERT(ret == 0, "queue allocate fail %d\n", ret);
	ret = pp_qos_queue_allocate(qdev, &qid[1]);
	QOS_ASSERT(ret == 0, "queue allocate fail %d\n", ret);
	ret = pp_qos_queue_allocate(qdev, &qid[2]);
	QOS_ASSERT(ret == 0, "queue allocate fail %d\n", ret);

	ret = pp_qos_queue_set(qdev, qid[0], &confq);
	QOS_ASSERT(ret == 0, "queue %u set fail (%d)\n", qid[0], ret);
	ret = pp_qos_queue_set(qdev, qid[1], &confq);
	QOS_ASSERT(ret == 0, "queue %u set fail (%d)\n", qid[1], ret);
	ret = pp_qos_queue_set(qdev, qid[2], &confq);
	QOS_ASSERT(ret == 0, "queue %u set fail (%d)\n", qid[2], ret);

	pp_qos_port_info_get(qdev, id2, &info);
	QOS_ASSERT(info.num_of_queues == 3 && info.physical_id == 12,
		   "Number of queues %d or phy %d is not what expected\n",
		   info.num_of_queues, info.physical_id);

	pp_qos_port_get_queues(qdev, id2, ids, 2, &tmp2);
	QOS_ASSERT(tmp2 == 3, "Number of queues %d is not expected\n", tmp2);

	DUMP_QOS_TREE(qdev);

	QOS_ASSERT(pp_qos_port_remove(qdev, id1) == 0,
		   "Could not remove port\n");
	QOS_ASSERT(pp_qos_port_remove(qdev, id2) == 0,
		   "Could not remove port\n");

	ret = check_consistency(qdev);
	if (ret)
		return ret;

	return 0;
}

static int __compare_queue_nodes(struct qos_node *node1, struct qos_node *node2)
{
	if ((node1->type != node2->type) ||
	    (node1->bandwidth_limit != node2->bandwidth_limit) ||
	    (node1->shared_bw_group != node2->shared_bw_group) ||
	    (memcmp(&node1->child_prop,
		    &node2->child_prop,
		    sizeof(struct child_node_properties))) ||
	    (memcmp(&node1->parent_prop,
		    &node2->parent_prop,
		    sizeof(struct parent_node_properties))) ||
	    (node1->data.queue.green_min != node2->data.queue.green_min) ||
	    (node1->data.queue.green_max != node2->data.queue.green_max) ||
	    (node1->data.queue.yellow_min != node2->data.queue.yellow_min) ||
	    (node1->data.queue.yellow_max != node2->data.queue.yellow_max)) {
		pr_err("Queue nodes differs\n");
		pr_err("Node1: type: %d. bw: %hu. bw_grp: %hu. green_min: %u. green_max: %u. yellow_min: %u. yellow_max: %u\n",
		       node1->type,
		       node1->bandwidth_limit,
		       node1->shared_bw_group,
		       node1->data.queue.green_min,
		       node1->data.queue.green_max,
		       node1->data.queue.yellow_min,
		       node1->data.queue.yellow_max);
		pr_err("Node1: parent_phy: %hu. priority: %hhu. virt_bw_share: %hhu. bw_weight: %hhu\n",
		       node1->child_prop.parent_phy,
		       node1->child_prop.priority,
		       node1->child_prop.virt_bw_share,
		       node1->child_prop.bw_weight);
		pr_err("Node1: arbitration: %d. first_child_phy:%hu. num_of_children:%hhu\n",
		       node1->parent_prop.arbitration,
		       node1->parent_prop.first_child_phy,
		       node1->parent_prop.num_of_children);
		pr_err("Node2: type: %d. bw: %hu. bw_grp: %hu. green_min: %u. green_max: %u. yellow_min: %u. yellow_max: %u\n",
		       node2->type,
		       node2->bandwidth_limit,
		       node1->shared_bw_group,
		       node2->data.queue.green_min,
		       node2->data.queue.green_max,
		       node2->data.queue.yellow_min,
		       node2->data.queue.yellow_max);
		pr_err("Node2: parent_phy: %hu. priority: %hhu. virt_bw_share: %hhu. bw_weight: %hhu\n",
		       node2->child_prop.parent_phy,
		       node2->child_prop.priority,
		       node2->child_prop.virt_bw_share,
		       node2->child_prop.bw_weight);
		pr_err("Node2: arbitration: %d. first_child_phy:%hu. num_of_children:%hhu\n",
		       node2->parent_prop.arbitration,
		       node2->parent_prop.first_child_phy,
		       node2->parent_prop.num_of_children);
		return -EINVAL;
	}

	return 0;
}

static int test_queues1(struct pp_qos_dev *qdev)
{
	int tmp;
	struct pp_qos_queue_conf confq;
	struct pp_qos_port_conf confp;
	struct qos_node node;
	struct qos_node *nodep;
	unsigned int p1 = PP_QOS_INVALID_ID;
	unsigned int p7 = PP_QOS_INVALID_ID;
	unsigned int q[72] = {PP_QOS_INVALID_ID};
	unsigned int i;
	unsigned int firstnonport;
	s32 ret;

	firstnonport = qdev->init_params.max_ports;

	/* Allocate port 1  */
	pp_qos_port_conf_set_default(&confp);
	confp.port_parent_prop.arbitration = PP_QOS_ARBITRATION_WRR;
	confp.ring_address = ring_addr_phys[0];
	confp.ring_size = 1;
	tmp = pp_qos_port_allocate(qdev, 1, &p1);
	ret = expected_id_phy(qdev, p1, 0, 1);
	if (ret)
		return ret;

	QOS_ASSERT(pp_qos_port_set(qdev, p1, &confp) == 0,
		   "Could not set port\n");
	ret = check_consistency_with_resources(qdev);
	if (ret)
		return ret;

	/* Allocate first queue */
	pp_qos_queue_conf_set_default(&confq);
	confq.queue_child_prop.parent = p1;
	tmp = pp_qos_queue_allocate(qdev, &q[0]);
	pp_qos_queue_set(qdev, q[0], &confq);
	ret = expected_id_phy(qdev, q[0], 1, firstnonport);
	if (ret)
		return ret;

	ret = check_consistency_with_resources(qdev);
	if (ret)
		return ret;

	/* Allocate 7 more queues */
	for (i = 1; i < 8; ++i) {
		QOS_LOG_DEBUG("Allocate queue %d\n", i);
		tmp = pp_qos_queue_allocate(qdev, &q[i]);
		confq.wred_max_allowed = i;
		pp_qos_queue_set(qdev, q[i], &confq);
		ret = expected_id_phy(qdev, q[i], i + 1, firstnonport + i);
		if (ret)
			return ret;

		ret = check_consistency_with_resources(qdev);
		if (ret)
			return ret;
	}

	/* Allocate 64 more queues */
	for (i = 8; i < 72; ++i) {
		QOS_LOG_DEBUG("Allocate queue %d\n", i);
		tmp = pp_qos_queue_allocate(qdev, &q[i]);
		confq.wred_max_allowed = i;
		pp_qos_queue_set(qdev, q[i], &confq);
		ret = check_consistency_with_resources(qdev);
		if (ret)
			return ret;
	}

	/* change setting without parent */
	tmp = get_phy_from_id(qdev->mapping, q[7]);
	nodep = get_node_from_phy(qdev->nodes, tmp);
	QOS_ASSERT(nodep, "get_node_from_phy(%u) returned NULL\n", tmp);
	memcpy(&node, nodep, sizeof(struct qos_node));
	QOS_ASSERT(pp_qos_queue_conf_get(qdev, q[7], &confq) == 0,
		   "pp_qos_queue_conf_get failed\n");
	confq.wred_max_allowed = 77;
	pp_qos_queue_set(qdev, q[7], &confq);
	QOS_ASSERT(__compare_queue_nodes(&node, nodep) == 0,
		   "Unexpected change\n");
	QOS_ASSERT(nodep->data.queue.max_allowed == 77,
		   "Set max allowed did not work\n");
	QOS_ASSERT(memcmp(&(node.data.queue.min_guaranteed),
			  &(nodep->data.queue.min_guaranteed),
			  sizeof(struct qos_node) -
			  offsetof(struct qos_node,
				   data.queue.min_guaranteed)) == 0,
		   "Unexpected change\n");
	ret = check_consistency_with_resources(qdev);
	if (ret)
		return ret;

	/* change setting with parent */
	tmp = pp_qos_port_allocate(qdev, 7, &p7);
	QOS_ASSERT(pp_qos_port_set(qdev, p7, &confp) == 0,
		   "Could not set port\n");

	tmp = get_phy_from_id(qdev->mapping, q[14]);
	nodep = get_node_from_phy(qdev->nodes, tmp);
	QOS_ASSERT(nodep, "get_node_from_phy(%u) returned NULL\n", tmp);
	memcpy(&node, nodep, sizeof(struct qos_node));
	QOS_ASSERT(pp_qos_queue_conf_get(qdev, q[14], &confq) == 0,
		   "pp_qos_queue_conf_get failed\n");
	confq.queue_child_prop.parent = p7;
	confq.wred_max_allowed = 777;
	pp_qos_queue_block(qdev, q[14]);
	pp_qos_queue_set(qdev, q[14], &confq);
	pp_qos_queue_unblock(qdev, q[14]);

	tmp = get_phy_from_id(qdev->mapping, q[14]);
	nodep = get_node_from_phy(qdev->nodes, tmp);
	QOS_ASSERT(nodep, "get_node_from_phy(%u) returned NULL\n", tmp);
	QOS_ASSERT(nodep->data.queue.max_allowed == 777,
		   "Set max allowed not work\n");
	QOS_ASSERT(nodep->child_prop.parent_phy ==
		   get_phy_from_id(qdev->mapping, p7),
		   "Set of parent did not work has %d instead of %d\n",
		   nodep->child_prop.parent_phy,
		   get_phy_from_id(qdev->mapping, p7));
	QOS_ASSERT(memcmp(&node, nodep,
			  offsetof(struct qos_node,
				   child_prop.parent_phy)) == 0,
		   "Unexpected change\n");
	QOS_ASSERT(memcmp(&(node.child_prop.virt_bw_share),
			  &(nodep->child_prop.virt_bw_share),
			  offsetof(struct qos_node,
				   data.queue.max_allowed) -
			  offsetof(struct qos_node,
				   child_prop.virt_bw_share)) == 0,
		   "Unexpected change\n");
	QOS_ASSERT(memcmp(&(node.data.queue.min_guaranteed),
			  &(nodep->data.queue.min_guaranteed),
			  sizeof(struct qos_node) -
			  offsetof(struct qos_node,
				   data.queue.min_guaranteed)) == 0,
		   "Unexpected change\n");

	ret = check_consistency_with_resources(qdev);
	if (ret)
		return ret;

	for (i = 20; i < 50; ++i) {
		QOS_ASSERT(pp_qos_queue_conf_get(qdev, q[i], &confq) == 0,
			   "pp_qos_queue_conf_get failed on %d iteration\n", i);
		confq.wred_max_allowed = 300 + i;
		confq.queue_child_prop.parent = p7;
		confq.blocked = 1;
		QOS_ASSERT(pp_qos_queue_set(qdev, q[i], &confq) == 0,
			   "Setting failed on %d iteration\n", i);
		pp_qos_queue_unblock(qdev, q[i]);
	}

	for (i = 20; i < 50; ++i) {
		QOS_ASSERT(pp_qos_queue_conf_get(qdev, q[i], &confq) == 0,
			   "pp_qos_queue_conf_get failed on %d iteration\n", i);
		confq.wred_max_allowed = 400 + i;
		confq.queue_child_prop.parent = p1;
		confq.blocked = 1;
		QOS_ASSERT(pp_qos_queue_set(qdev, q[i], &confq) == 0,
			   "Setting failed on %d iteration\n", i);
		pp_qos_queue_unblock(qdev, q[i]);
	}

	DUMP_QOS_TREE(qdev);

	/* Delete queues */
	for (i = 0; i < 72; ++i) {
		QOS_LOG_DEBUG("Remvove queue %d\n", i);
		pp_qos_queue_block(qdev, q[i]);
		qos_queue_flush(qdev, q[i]);
		QOS_ASSERT(pp_qos_queue_remove(qdev, q[i]) == 0,
			   "Could not remove queue %d\n", q[i]);
	}

	QOS_ASSERT(pp_qos_port_remove(qdev, p1) == 0,
		   "Could not remove port\n");
	QOS_ASSERT(pp_qos_port_remove(qdev, p7) == 0,
		   "Could not remove port\n");

	ret = check_consistency_with_resources(qdev);
	if (ret)
		return ret;

	return 0;
}

static int test_queues2(struct pp_qos_dev *qdev)
{
	int tmp;
	struct pp_qos_queue_conf confq;
	struct pp_qos_port_conf confp;
	struct qos_node node;
	struct qos_node *nodep;
	unsigned int p1 = PP_QOS_INVALID_ID;
	unsigned int p7 = PP_QOS_INVALID_ID;
	unsigned int p12 = PP_QOS_INVALID_ID;
	unsigned int q[100] = {PP_QOS_INVALID_ID};
	unsigned int i;
	unsigned int tmp1;
	unsigned int tmp2;
	unsigned int tmp3;
	unsigned int tmp4;
	unsigned int firstnonport;
	s32 ret;

	firstnonport = qdev->init_params.max_ports;

	/* Allocate port 1  */
	pp_qos_port_conf_set_default(&confp);
	confp.port_parent_prop.arbitration = PP_QOS_ARBITRATION_WRR;
	confp.ring_address = ring_addr_phys[0];
	confp.ring_size = 1;
	tmp = pp_qos_port_allocate(qdev, 1, &p1);
	ret = expected_id_phy(qdev, p1, 0, 1);
	if (ret)
		return ret;

	QOS_ASSERT(pp_qos_port_set(qdev, p1, &confp) == 0,
		   "Could not set port\n");
	ret = check_consistency_with_resources(qdev);
	if (ret)
		return ret;

	/* Allocate first queue */
	pp_qos_queue_conf_set_default(&confq);
	confq.queue_child_prop.parent = p1;
	tmp = pp_qos_queue_allocate(qdev, &q[0]);
	pp_qos_queue_set(qdev, q[0], &confq);
	ret = expected_id_phy(qdev, q[0], 1, firstnonport);
	if (ret)
		return ret;

	ret = check_consistency_with_resources(qdev);
	if (ret)
		return ret;

	/* Allocate 7 more queues */
	for (i = 1; i < 8; ++i) {
		tmp = pp_qos_queue_allocate(qdev, &q[i]);
		confq.wred_max_allowed = i;
		pp_qos_queue_set(qdev, q[i], &confq);
		ret = expected_id_phy(qdev, q[i], i + 1, firstnonport + i);
		if (ret)
			return ret;

		ret = check_consistency_with_resources(qdev);
		if (ret)
			return ret;
	}

	/* Allocate 64 more queues */
	for (i = 8; i < 100; ++i) {
		tmp = pp_qos_queue_allocate(qdev, &q[i]);
		confq.wred_max_allowed = i;
		confq.queue_child_prop.wrr_weight = 1;
		pp_qos_queue_set(qdev, q[i], &confq);
		ret = check_consistency_with_resources(qdev);
		if (ret)
			return ret;
	}

	/* change setting without parent */
	tmp = get_phy_from_id(qdev->mapping, q[7]);
	nodep = get_node_from_phy(qdev->nodes, tmp);
	QOS_ASSERT(nodep, "get_node_from_phy(%u) returned NULL\n", tmp);
	memcpy(&node, nodep, sizeof(struct qos_node));
	QOS_ASSERT(pp_qos_queue_conf_get(qdev, q[7], &confq) == 0,
		   "pp_qos_queue_conf_get failed\n");
	confq.wred_max_allowed = 77;
	pp_qos_queue_set(qdev, q[7], &confq);
	QOS_ASSERT(__compare_queue_nodes(&node, nodep) == 0,
		   "Unexpected change\n");
	QOS_ASSERT(nodep->data.queue.max_allowed == 77,
		   "Set max allowed did not work\n");
	QOS_ASSERT(memcmp(&(node.data.queue.min_guaranteed),
			  &(nodep->data.queue.min_guaranteed),
			  sizeof(struct qos_node) -
			  offsetof(struct qos_node,
				   data.queue.min_guaranteed)) == 0,
		   "Unexpected change\n");
	ret = check_consistency_with_resources(qdev);
	if (ret)
		return ret;

	/* allocate port 7 and port 12 */
	tmp = pp_qos_port_allocate(qdev, 7, &p7);
	QOS_ASSERT(pp_qos_port_set(qdev, p7, &confp) == 0,
		   "Could not set port\n");
	tmp = pp_qos_port_allocate(qdev, 12, &p12);
	QOS_ASSERT(pp_qos_port_set(qdev, p12, &confp) == 0,
		   "Could not set port\n");

	/* change setting with parent */
	tmp = get_phy_from_id(qdev->mapping, q[14]);
	nodep = get_node_from_phy(qdev->nodes, tmp);
	QOS_ASSERT(nodep, "get_node_from_phy(%u) returned NULL\n", tmp);
	memcpy(&node, nodep, sizeof(struct qos_node));

	QOS_ASSERT(pp_qos_queue_conf_get(qdev, q[14], &confq) == 0,
		   "pp_qos_queue_conf_get failed\n");
	confq.queue_child_prop.parent = p7;
	confq.wred_max_allowed = 777;

	pp_qos_queue_block(qdev, q[14]);
	pp_qos_queue_set(qdev, q[14], &confq);
	pp_qos_queue_unblock(qdev, q[14]);
	tmp = get_phy_from_id(qdev->mapping, q[14]);
	nodep = get_node_from_phy(qdev->nodes, tmp);
	QOS_ASSERT(nodep, "get_node_from_phy(%u) returned NULL\n", tmp);
	QOS_ASSERT(nodep->data.queue.max_allowed == 777,
		   "Set max allowed not work\n");
	QOS_ASSERT(nodep->child_prop.parent_phy ==
		   get_phy_from_id(qdev->mapping, p7),
		   "Set of parent did not work has %d instead of %d\n",
		   nodep->child_prop.parent_phy,
		   get_phy_from_id(qdev->mapping, p7));
	QOS_ASSERT(memcmp(&node, nodep,
			  offsetof(struct qos_node,
				   child_prop.parent_phy)) == 0,
		   "Unexpected change\n");
	QOS_ASSERT(memcmp(&node, nodep,
			  offsetof(struct qos_node,
				   child_prop.parent_phy)) == 0,
		   "Unexpected change\n");
	QOS_ASSERT(memcmp(&(node.child_prop.virt_bw_share),
			  &(nodep->child_prop.virt_bw_share),
			  offsetof(struct qos_node, data.queue.max_allowed) -
			  offsetof(struct qos_node,
				   child_prop.virt_bw_share)) == 0,
		   "Unexpected change\n");
	QOS_ASSERT(memcmp(&(node.data.queue.min_guaranteed),
			  &(nodep->data.queue.min_guaranteed),
			  sizeof(struct qos_node) -
			  offsetof(struct qos_node,
				   data.queue.min_guaranteed)) == 0,
		   "Unexpected change\n");

	ret = check_consistency_with_resources(qdev);
	if (ret)
		return ret;

	tmp = get_phy_from_id(qdev->mapping, q[21]);
	nodep = get_node_from_phy(qdev->nodes, tmp);
	QOS_ASSERT(nodep, "get_node_from_phy(%u) returned NULL\n", tmp);
	memcpy(&node, nodep, sizeof(struct qos_node));
	QOS_ASSERT(pp_qos_queue_conf_get(qdev, q[21], &confq) == 0,
		   "pp_qos_queue_conf_get failed\n");
	confq.queue_child_prop.parent = p12;
	confq.wred_max_allowed = 2121;
	pp_qos_queue_block(qdev, q[21]);
	pp_qos_queue_set(qdev, q[21], &confq);
	pp_qos_queue_unblock(qdev, q[21]);
	tmp = get_phy_from_id(qdev->mapping, q[21]);
	nodep = get_node_from_phy(qdev->nodes, tmp);
	QOS_ASSERT(nodep, "get_node_from_phy(%u) returned NULL\n", tmp);
	QOS_ASSERT(nodep->data.queue.max_allowed == 2121,
		   "Set max allowed not work\n");
	QOS_ASSERT(nodep->child_prop.parent_phy ==
		   get_phy_from_id(qdev->mapping, p12),
		   "Set of parent did not work has %d instead of %d\n",
		   nodep->child_prop.parent_phy,
		   get_phy_from_id(qdev->mapping, p12));
	QOS_ASSERT(memcmp(&node, nodep,
			  offsetof(struct qos_node,
				   child_prop.parent_phy)) == 0,
		   "Unexpected change\n");
	QOS_ASSERT(memcmp(&node, nodep,
			  offsetof(struct qos_node,
				   child_prop.parent_phy)) == 0,
		   "Unexpected change\n");
	QOS_ASSERT(memcmp(&(node.child_prop.virt_bw_share),
			  &(nodep->child_prop.virt_bw_share),
			  offsetof(struct qos_node, data.queue.max_allowed) -
			  offsetof(struct qos_node,
				   child_prop.virt_bw_share)) == 0,
		   "Unexpected change\n");
	QOS_ASSERT(memcmp(&(node.data.queue.min_guaranteed),
			  &(nodep->data.queue.min_guaranteed),
			  sizeof(struct qos_node) -
			  offsetof(struct qos_node,
				   data.queue.min_guaranteed)) == 0,
		   "Unexpected change\n");


	ret = check_consistency_with_resources(qdev);
	if (ret)
		return ret;

	ret = expected_queues(qdev, 1, 98);
	if (ret)
		return ret;

	ret = expected_queues(qdev, 7, 1);
	if (ret)
		return ret;

	ret = expected_queues(qdev, 12, 1);
	if (ret)
		return ret;

	for (i = 25; i < 50; ++i) {
		QOS_ASSERT(pp_qos_queue_conf_get(qdev, q[i], &confq) == 0,
			   "pp_qos_queue_conf_get failed on %d iteration\n",
			   i);
		confq.wred_max_allowed = 300 + i;
		confq.queue_child_prop.parent = p12;
		confq.blocked = 1;
		QOS_ASSERT(pp_qos_queue_set(qdev, q[i], &confq) == 0,
			   "Setting failed on %d iteration\n", i);
		pp_qos_queue_unblock(qdev, q[i]);
		ret = check_consistency_with_resources(qdev);
		if (ret)
			return ret;
	}

	ret = expected_queues(qdev, 1, 73);
	if (ret)
		return ret;

	ret = expected_queues(qdev, 7, 1);
	if (ret)
		return ret;

	ret = expected_queues(qdev, 12, 26);
	if (ret)
		return ret;

	for (i = 50; i < 70; ++i) {
		QOS_ASSERT(pp_qos_queue_conf_get(qdev, q[i], &confq) == 0,
			   "pp_qos_queue_conf_get failed on %d iteration\n",
			   i);
		confq.wred_max_allowed = 400 + i;
		confq.queue_child_prop.parent = p7;
		confq.blocked = 1;
		QOS_ASSERT(pp_qos_queue_set(qdev, q[i], &confq) == 0,
			   "Setting failed on %d iteration\n", i);
		pp_qos_queue_unblock(qdev, q[i]);
		ret = check_consistency_with_resources(qdev);
		if (ret)
			return ret;
	}

	ret = expected_queues(qdev, 1, 53);
	if (ret)
		return ret;

	ret = expected_queues(qdev, 7, 21);
	if (ret)
		return ret;

	ret = expected_queues(qdev, 12, 26);
	if (ret)
		return ret;

	for (i = 25; i < 45; ++i) {
		QOS_ASSERT(pp_qos_queue_conf_get(qdev, q[i], &confq) == 0,
			   "pp_qos_queue_conf_get failed on %d iteration\n",
			   i);
		confq.wred_max_allowed = 500 + i;
		confq.queue_child_prop.parent = p7;
		confq.blocked = 1;
		QOS_ASSERT(pp_qos_queue_set(qdev, q[i], &confq) == 0,
			   "Setting failed on %d iteration\n", i);
		pp_qos_queue_unblock(qdev, q[i]);
		ret = check_consistency_with_resources(qdev);
		if (ret)
			return ret;
	}

	ret = expected_queues(qdev, 1, 53);
	if (ret)
		return ret;

	ret = expected_queues(qdev, 7, 41);
	if (ret)
		return ret;

	ret = expected_queues(qdev, 12, 6);
	if (ret)
		return ret;

	for (i = 45; i < 60; ++i) {
		QOS_ASSERT(pp_qos_queue_conf_get(qdev, q[i], &confq) == 0,
			   "pp_qos_queue_conf_get failed on %d iteration\n",
			   i);
		confq.wred_max_allowed = 600 + i;
		confq.queue_child_prop.parent = p12;
		confq.blocked = 1;
		QOS_ASSERT(pp_qos_queue_set(qdev, q[i], &confq) == 0,
			   "Setting failed on %d iteration\n", i);
		pp_qos_queue_unblock(qdev, q[i]);
		ret = check_consistency_with_resources(qdev);
		if (ret)
			return ret;
	}

	ret = expected_queues(qdev, 1, 53);
	if (ret)
		return ret;

	ret = expected_queues(qdev, 7, 31);
	if (ret)
		return ret;

	ret = expected_queues(qdev, 12, 16);
	if (ret)
		return ret;

	for (i = 60; i < 70; ++i) {
		QOS_ASSERT(pp_qos_queue_conf_get(qdev, q[i], &confq) == 0,
			   "pp_qos_queue_conf_get failed on %d iteration\n",
			   i);
		confq.wred_max_allowed = 700 + i;
		confq.queue_child_prop.parent = p1;
		confq.blocked = 1;
		QOS_ASSERT(pp_qos_queue_set(qdev, q[i], &confq) == 0,
			   "Setting failed on %d iteration\n", i);
		pp_qos_queue_unblock(qdev, q[i]);
		ret = check_consistency_with_resources(qdev);
		if (ret)
			return ret;
	}

	ret = expected_queues(qdev, 1, 63);
	if (ret)
		return ret;

	ret = expected_queues(qdev, 7, 21);
	if (ret)
		return ret;

	ret = expected_queues(qdev, 12, 16);
	if (ret)
		return ret;

	for (i = 50; i < 70; ++i) {
		QOS_ASSERT(pp_qos_queue_conf_get(qdev, q[i], &confq) == 0,
			   "pp_qos_queue_conf_get failed on %d iteration\n",
			   i);
		confq.wred_max_allowed = 400 + i;
		confq.queue_child_prop.parent = p7;
		confq.blocked = 1;
		QOS_ASSERT(pp_qos_queue_set(qdev, q[i], &confq) == 0,
			   "Setting failed on %d iteration\n", i);
		pp_qos_queue_unblock(qdev, q[i]);
		ret = check_consistency_with_resources(qdev);
		if (ret)
			return ret;
	}

	for (i = 25; i < 45; ++i) {
		QOS_ASSERT(pp_qos_queue_conf_get(qdev, q[i], &confq) == 0,
			   "pp_qos_queue_conf_get failed on %d iteration\n",
			   i);
		confq.wred_max_allowed = 500 + i;
		confq.queue_child_prop.parent = p7;
		confq.blocked = 1;
		QOS_ASSERT(pp_qos_queue_set(qdev, q[i], &confq) == 0,
			   "Setting failed on %d iteration\n", i);
		pp_qos_queue_unblock(qdev, q[i]);
		ret = check_consistency_with_resources(qdev);
		if (ret)
			return ret;
	}

	for (i = 45; i < 60; ++i) {
		QOS_ASSERT(pp_qos_queue_conf_get(qdev, q[i], &confq) == 0,
			   "pp_qos_queue_conf_get failed on %d iteration\n",
			   i);
		confq.wred_max_allowed = 600 + i;
		confq.queue_child_prop.parent = p12;
		confq.blocked = 1;
		QOS_ASSERT(pp_qos_queue_set(qdev, q[i], &confq) == 0,
			   "Setting failed on %d iteration\n", i);
		pp_qos_queue_unblock(qdev, q[i]);
		ret = check_consistency_with_resources(qdev);
		if (ret)
			return ret;
	}

	for (i = 60; i < 70; ++i) {
		QOS_ASSERT(pp_qos_queue_conf_get(qdev, q[i], &confq) == 0,
			   "pp_qos_queue_conf_get failed on %d iteration\n",
			   i);
		confq.wred_max_allowed = 700 + i;
		confq.queue_child_prop.parent = p1;
		confq.blocked = 1;
		QOS_ASSERT(pp_qos_queue_set(qdev, q[i], &confq) == 0,
			   "Setting failed on %d iteration\n", i);
		pp_qos_queue_unblock(qdev, q[i]);
		ret = check_consistency_with_resources(qdev);
		if (ret)
			return ret;
	}

	get_node_queues(qdev, 1, NULL, 0, &tmp1);
	get_node_queues(qdev, 7, NULL, 0, &tmp2);
	get_node_queues(qdev, 12, NULL, 0, &tmp3);
	tmp4 = tmp1 + tmp2 + tmp3;
	QOS_ASSERT(tmp4 == 100,
		   "Number of queues expected to be 100 but it is %u\n", tmp4);

	DUMP_QOS_TREE(qdev);

	/* Delete queues */
	for (i = 0; i < 100; ++i) {
		QOS_LOG_DEBUG("Remvove queue %d\n", i);
		pp_qos_queue_block(qdev, q[i]);
		qos_queue_flush(qdev, q[i]);
		QOS_ASSERT(pp_qos_queue_remove(qdev, q[i]) == 0,
			   "Could not remove queue %d\n", q[i]);
	}

	QOS_ASSERT(pp_qos_port_remove(qdev, p1) == 0,
		   "Could not remove port\n");
	QOS_ASSERT(pp_qos_port_remove(qdev, p7) == 0,
		   "Could not remove port\n");
	QOS_ASSERT(pp_qos_port_remove(qdev, p12) == 0,
		   "Could not remove port\n");

	return 0;
}

static int test_queues3(struct pp_qos_dev *qdev)
{
	struct pp_qos_queue_conf confq;
	struct pp_qos_port_conf confp;
	unsigned int p1 = PP_QOS_INVALID_ID;
	unsigned int q[65] = {PP_QOS_INVALID_ID};
	unsigned int i;
	int tmp;
	unsigned int bw[] = { 13, 0, 20, 17, 0, 0, 0, 8 };
	unsigned int firstnonport;
	s32 ret;

	firstnonport = qdev->init_params.max_ports;

	/* Allocate port 1  */
	pp_qos_port_conf_set_default(&confp);
	confp.port_parent_prop.arbitration = PP_QOS_ARBITRATION_WRR;
	confp.ring_address = ring_addr_phys[0];
	confp.ring_size = 1;
	tmp = pp_qos_port_allocate(qdev, 1, &p1);
	ret = expected_id_phy(qdev, p1, 0, 1);
	if (ret)
		return ret;

	QOS_ASSERT(pp_qos_port_set(qdev, p1, &confp) == 0,
		   "Could not set port\n");
	ret = check_consistency_with_resources(qdev);
	if (ret)
		return ret;

	pp_qos_queue_conf_set_default(&confq);
	confq.queue_child_prop.parent = p1;

	/* Allocate 7 queues */
	for (i = 0; i < 7; ++i) {
		tmp = pp_qos_queue_allocate(qdev, &q[i]);
		QOS_ASSERT(tmp == 0, "Could not allocate queue\n");
		confq.wred_max_allowed = i;
		confq.queue_child_prop.wrr_weight = 13;
		QOS_ASSERT(pp_qos_queue_set(qdev, q[i], &confq) == 0,
			   "Could not set queue\n");
	}

	ret = check_consistency_with_resources(qdev);
	if (ret)
		return ret;

	/* Try to add a new queue with a share that cross 128 */
	tmp = pp_qos_queue_allocate(qdev, &q[7]);
	QOS_ASSERT(tmp == 0, "Could not allocate queue\n");
	confq.wred_max_allowed = 8;
	confq.queue_child_prop.wrr_weight = 128;
	QOS_ASSERT(pp_qos_queue_set(qdev, q[7], &confq) != 0,
		   "Set should have failed due to over bandwidth\n");
	pp_pool_put(qdev->ids, q[7]);
	ret = check_consistency_with_resources(qdev);
	if (ret)
		return ret;

	/* Change setting on existing node */
	QOS_ASSERT(pp_qos_queue_conf_get(qdev, q[6], &confq) == 0,
		   "pp_qos_queue_conf_get failed\n");
	confq.queue_child_prop.wrr_weight = 15;
	QOS_ASSERT(pp_qos_queue_set(qdev, q[6], &confq) == 0,
		   "Set failed\n");

	/* Add a new queue that does not cross 100??*/
	tmp = pp_qos_queue_allocate(qdev, &q[7]);
	QOS_ASSERT(tmp == 0, "Could not allocate queue\n");
	confq.wred_max_allowed = 8;
	confq.queue_child_prop.wrr_weight = 6;
	QOS_ASSERT(pp_qos_queue_set(qdev, q[7], &confq) == 0,
		   "Set have failed\n");
	ret = check_consistency_with_resources(qdev);
	if (ret)
		return ret;

	for (i = 0; i < 8; ++i) {
		QOS_ASSERT(pp_qos_queue_conf_get(qdev, q[i], &confq) == 0,
			   "pp_qos_queue_conf_get failed\n");
		QOS_ASSERT(tmp == 0, "Could not allocate queue\n");
		confq.queue_child_prop.wrr_weight = bw[i];
		QOS_ASSERT(pp_qos_queue_set(qdev, q[i], &confq) == 0,
			   "Could not set queue\n");
	}

	tmp = pp_qos_queue_allocate(qdev, &q[8]);
	QOS_ASSERT(tmp == 0, "Could not allocate queue\n");
	confq.queue_child_prop.wrr_weight = 10;
	QOS_ASSERT(pp_qos_queue_set(qdev, q[8], &confq) == 0,
		   "Could not set queue\n");
	ret = check_consistency_with_resources(qdev);
	if (ret)
		return ret;

	for (i = 9; i < 64; ++i) {
		tmp = pp_qos_queue_allocate(qdev, &q[i]);
		QOS_ASSERT(tmp == 0, "Could not allocate queue\n");
		confq.queue_child_prop.wrr_weight = 0;
		QOS_ASSERT(pp_qos_queue_set(qdev, q[i], &confq) == 0,
			   "Could not set queue\n");
		ret = check_consistency_with_resources(qdev);
		if (ret)
			return ret;
	}

	tmp = pp_qos_queue_allocate(qdev, &q[64]);
	QOS_ASSERT(tmp == 0, "Could not allocate queue\n");
	confq.queue_child_prop.wrr_weight = 15;
	QOS_ASSERT(pp_qos_queue_set(qdev, q[64], &confq) == 0,
		   "Could not set queue\n");

	pp_qos_queue_block(qdev, q[64]);
	qos_queue_flush(qdev, q[64]);
	QOS_ASSERT(pp_qos_queue_remove(qdev, q[64]) == 0,
		   "Could not remove queue %d\n", q[64]);

	ret = check_consistency_with_resources(qdev);
	if (ret)
		return ret;

	DUMP_QOS_TREE(qdev);

	tree_remove(qdev, firstnonport);
	ret = check_consistency_with_resources(qdev);
	if (ret)
		return ret;

	QOS_ASSERT(pp_qos_port_remove(qdev, p1) == 0,
		   "Could not remove port\n");

	return 0;
}

static int reposition_internal_test(void *data)
{
	struct pp_qos_queue_conf confq;
	struct pp_qos_port_conf confp;
	struct pp_qos_sched_conf confs;
	struct pp_qos_dev *qdev;

	unsigned int p89;
	unsigned int p97;
	unsigned int p105;
	unsigned int p7;
	unsigned int q[20];
	unsigned int s[5] = {PP_QOS_INVALID_ID};
	int i;
	int tmp;
	s32 ret;
	u32 test_ret = PP_UT_FAIL;

	qdev = qos_restart(PP_QOS_INSTANCE_ID);
	if (!qdev)
		return PP_UT_FAIL;

	tmp = pp_qos_port_allocate(qdev, 7, &p7);
	QOS_ASSERT(tmp == 0, "could not allocate qos port\n");
	QOS_LOG_DEBUG("Allocated port with id %d\n", p7);

	tmp = pp_qos_port_allocate(qdev, 89, &p89);
	QOS_ASSERT(tmp == 0, "could not allocate qos port\n");
	QOS_LOG_DEBUG("Allocated port with id %d\n", p89);

	tmp = pp_qos_port_allocate(qdev, 97, &p97);
	QOS_ASSERT(tmp == 0, "could not allocate qos port\n");
	QOS_LOG_DEBUG("Allocated port with id %d\n", p97);

	tmp = pp_qos_port_allocate(qdev, 105, &p105);
	QOS_ASSERT(tmp == 0, "could not allocate qos port\n");
	QOS_LOG_DEBUG("Allocated port with id %d\n", p105);

	pp_qos_port_conf_set_default(&confp);
	confp.port_parent_prop.arbitration = PP_QOS_ARBITRATION_WRR;
	confp.ring_size = 1;
	confp.ring_address = ring_addr_phys[0];
	QOS_ASSERT(pp_qos_port_set(qdev, p7, &confp) == 0,
		   "Could not set port\n");

	confp.ring_address = ring_addr_phys[1];
	QOS_ASSERT(pp_qos_port_set(qdev, p89, &confp) == 0,
		   "Could not set port\n");

	confp.ring_address = ring_addr_phys[2];
	QOS_ASSERT(pp_qos_port_set(qdev, p97, &confp) == 0,
		   "Could not set port\n");

	confp.ring_address = ring_addr_phys[3];
	QOS_ASSERT(pp_qos_port_set(qdev, p105, &confp) == 0,
		   "Could not set port\n");

	pp_qos_queue_conf_set_default(&confq);
	confq.queue_child_prop.parent = p89;
	for (i = 0; i < 20; ++i) {
		tmp = pp_qos_queue_allocate(qdev, &q[i]);
		QOS_LOG_DEBUG("Allocated queue with id %d\n", q[i]);
		QOS_ASSERT(pp_qos_queue_set(qdev, q[i], &confq) == 0,
			   "q set failed\n");
	}

	/* moving queues from p89 to p7 */
	confq.queue_child_prop.parent = p7;
	for (i = 0; i < 20; ++i) {
		confq.blocked = 1;
		QOS_ASSERT(pp_qos_queue_set(qdev, q[i], &confq) == 0,
			   "q set failed\n");
		pp_qos_queue_unblock(qdev, q[i]);
	}

	for (i = 0; i < 15; ++i) {
		QOS_LOG_DEBUG("Removing queue with id %d\n", q[i]);
		pp_qos_queue_block(qdev, q[i]);
		qos_queue_flush(qdev, q[i]);
		QOS_ASSERT(pp_qos_queue_remove(qdev, q[i]) == 0,
			   "q remove failed\n");
	}

	ret = check_consistency_with_resources(qdev);
	if (ret) {
		QOS_LOG_INFO("check_consistency_with_resources failed\n");
		goto done;
	}

	pp_qos_sched_conf_set_default(&confs);
	for (i = 0; i < 5; ++i) {
		QOS_ASSERT(pp_qos_sched_allocate(qdev, &s[i]) == 0,
			   "Could not allocate sched\n");
		QOS_LOG_DEBUG("Allocated sched with id %d\n", s[i]);
		if (i == 0)
			confs.sched_child_prop.parent = p97;
		else
			confs.sched_child_prop.parent = s[i - 1];
		QOS_ASSERT(pp_qos_sched_set(qdev, s[i], &confs) == 0,
			   "Could not set sched\n");
	}

	ret = check_consistency_with_resources(qdev);
	if (ret) {
		QOS_LOG_INFO("check_consistency_with_resources failed\n");
		goto done;
	}

	QOS_ASSERT(pp_qos_queue_allocate(qdev, &q[0]) == 0,
		   "Could not allocate queue\n");
	QOS_LOG_DEBUG("Allocated queue with id %d\n", q[0]);

	confq.queue_child_prop.parent = s[4];
	pp_qos_queue_block(qdev, q[0]);
	QOS_ASSERT(pp_qos_queue_set(qdev, q[0], &confq) == 0,
		   "Could not set queue\n");
	pp_qos_queue_unblock(qdev, q[0]);
	ret = check_consistency_with_resources(qdev);
	if (ret) {
		QOS_LOG_INFO("check_consistency_with_resources failed\n");
		goto done;
	}

	confs.sched_child_prop.parent = p97;
	QOS_ASSERT(pp_qos_sched_set(qdev, s[3], &confs) == 0,
		   "Could not set sched\n");
	ret = check_consistency_with_resources(qdev);
	if (ret) {
		QOS_LOG_INFO("check_consistency_with_resources failed\n");
		return ret;
	}

	confs.sched_child_prop.parent = p105;
	pp_qos_queue_block(qdev, q[0]);
	QOS_ASSERT(pp_qos_sched_set(qdev, s[4], &confs) == 0,
		   "Could not set sched\n");
	pp_qos_queue_unblock(qdev, q[0]);
	ret = check_consistency_with_resources(qdev);
	if (ret) {
		QOS_LOG_INFO("check_consistency_with_resources failed\n");
		goto done;
	}

#ifdef FW_CHECK_SYNC
	PPUT_FAIL_TEST(check_sync_with_fw(qdev, true) == 0,
		       "DB not consistent with fw\n");
#endif

	test_ret = PP_UT_PASS;
	DUMP_QOS_TREE(qdev);

done:
	QOS_LOG_DEBUG("Removing queue with id %d\n", q[0]);
	pp_qos_queue_block(qdev, q[0]);
	qos_queue_flush(qdev, q[0]);
	QOS_ASSERT(pp_qos_queue_remove(qdev, q[0]) == 0,
		   "Could not remove queue %d\n", q[0]);

	for (i = 15; i < 20; ++i) {
		QOS_LOG_DEBUG("Removing queue with id %d\n", q[i]);
		pp_qos_queue_block(qdev, q[i]);
		qos_queue_flush(qdev, q[i]);
		QOS_ASSERT(pp_qos_queue_remove(qdev, q[i]) == 0,
			   "q remove failed\n");
	}

	for (i = 4; i >= 0; --i) {
		QOS_LOG_DEBUG("Remove sched with id %d\n", s[i]);
		QOS_ASSERT(pp_qos_sched_remove(qdev, s[i]) == 0,
			   "Could not remove sched %d\n", s[i]);
	}

	QOS_ASSERT(pp_qos_port_remove(qdev, p7) == 0,
		   "Could not remove port\n");
	QOS_ASSERT(pp_qos_port_remove(qdev, p89) == 0,
		   "Could not remove port\n");
	QOS_ASSERT(pp_qos_port_remove(qdev, p97) == 0,
		   "Could not remove port\n");
	QOS_ASSERT(pp_qos_port_remove(qdev, p105) == 0,
		   "Could not remove port\n");

	return test_ret;
}

static int test_shared_group(struct pp_qos_dev *qdev)
{
	int rc;
	unsigned int id1;
	unsigned int id2;
	unsigned int p = PP_QOS_INVALID_ID;
	unsigned int q[9] = {PP_QOS_INVALID_ID};
	unsigned int phy;
	unsigned int gid;
	uint16_t members[5];
	unsigned int members_num;
	int i;
	struct qos_node *node;
	struct pp_qos_queue_conf confq;
	struct pp_qos_port_conf confp;
	s32 ret;

	rc = pp_qos_shared_limit_group_add(qdev, 100, &id1);
	QOS_ASSERT(rc == 0 && id1 == 1,
		   "Return code is %d id is %u expected 1\n", rc, id1);

	rc = pp_qos_shared_limit_group_add(qdev, 200, &id2);
	QOS_ASSERT(rc == 0 && id2 == 2,
		   "Return code is %d id is %u expected 2\n", rc, id2);

	rc = pp_qos_shared_limit_group_remove(qdev, 3);
	QOS_ASSERT(rc == -EINVAL,
		   "Removing not existent group should have failed\n");

	rc = pp_qos_shared_limit_group_remove(qdev, id2);
	QOS_ASSERT(rc == 0, "Removing failed with %d\n", rc);

	rc = pp_qos_shared_limit_group_modify(qdev, id2, 500);
	QOS_ASSERT(rc == -EINVAL,
		   "Modifying not existent group should have failed\n");

	rc = pp_qos_shared_limit_group_modify(qdev, id1, 100);
	QOS_ASSERT(rc == 0, "Modifying group failed with %d\n", rc);

	pp_qos_port_conf_set_default(&confp);
	confp.port_parent_prop.arbitration = PP_QOS_ARBITRATION_WRR;
	confp.ring_address = ring_addr_phys[0];
	confp.ring_size = 1;
	rc = pp_qos_port_allocate(qdev, 1, &p);
	QOS_ASSERT(pp_qos_port_set(qdev, p, &confp) == 0,
		   "Could not set port\n");
	node = get_node_from_phy(qdev->nodes, 1);
	QOS_ASSERT(node->shared_bw_group ==  QOS_NO_SHARED_BANDWIDTH_GROUP,
		   "shared bandwidth group shoud be %u but it is %u\n",
		   QOS_NO_SHARED_BANDWIDTH_GROUP,
		   node->shared_bw_group);

	pp_qos_queue_conf_set_default(&confq);
	confq.queue_child_prop.parent = p;

	/* Allocate 7 queues */
	for (i = 0; i < 7; ++i) {
		rc = pp_qos_queue_allocate(qdev, &q[i]);
		QOS_ASSERT(rc == 0, "Could not allocate queue\n");

		if (i > 3)
			confq.common_prop.shared_bw_group = id1;
		else
			confq.common_prop.shared_bw_group = 0;

		QOS_ASSERT(pp_qos_queue_set(qdev, q[i], &confq) == 0,
			   "Could not set queue\n");
	}

	for (i = 0; i < 7; ++i) {
		phy = get_phy_from_id(qdev->mapping, q[i]);
		node = get_node_from_phy(qdev->nodes, phy);
		QOS_ASSERT(node, "get_node_from_phy(%u) returned NULL\n", phy);

		gid = node->shared_bw_group;
		if (i > 3)
			QOS_ASSERT(gid == id1,
				   "shared group id is %u expected %u\n",
				   gid, id1);
		else

			QOS_ASSERT(gid == QOS_NO_SHARED_BANDWIDTH_GROUP,
				   "shared group id is %u expected %u\n",
				   gid, QOS_NO_SHARED_BANDWIDTH_GROUP);
	}

	rc = pp_qos_queue_allocate(qdev, &q[8]);
	QOS_ASSERT(rc == 0, "Could not allocate queue\n");
	confq.common_prop.shared_bw_group = 10;
	QOS_ASSERT(pp_qos_queue_set(qdev, q[8], &confq) == -EINVAL,
		   "Illegal shared group, should have failed\n");

	pp_qos_queue_block(qdev, q[8]);
	qos_queue_flush(qdev, q[8]);
	QOS_ASSERT(pp_qos_queue_remove(qdev, q[8]) == 0,
		   "Could not remove queue %d\n", q[8]);

	rc = pp_qos_shared_limit_group_get_members(qdev, id1, members,
						   5, &members_num);
	QOS_ASSERT(rc == 0 && members_num == 3,
		   "get_members returned %d, expected number of members to be %u but it is %u\n",
		   rc, 3, members_num);

	QOS_ASSERT(members[0] == q[4] &&
		   members[1] == q[5] &&
		   members[2] == q[6],
		   "Unexpected members %u %u %u\n",
		   members[0], members[1], members[2]);

	/* Block removing of queue with members */
	rc = pp_qos_shared_limit_group_remove(qdev, id1);
	QOS_ASSERT(rc == -EBUSY, "Removing group with members should fail\n");

	confq.common_prop.shared_bw_group = 0;

	QOS_ASSERT(pp_qos_queue_set(qdev, q[4], &confq) == 0,
		   "Could not set queue\n");
	rc = pp_qos_shared_limit_group_remove(qdev, id1);
	QOS_ASSERT(rc == -EBUSY, "Removing group with members should fail\n");

	QOS_ASSERT(pp_qos_queue_set(qdev, q[5], &confq) == 0,
		   "Could not set queue\n");
	rc = pp_qos_shared_limit_group_remove(qdev, id1);
	QOS_ASSERT(rc == -EBUSY, "Removing group with members should fail\n");

	ret = check_consistency_with_resources(qdev);
	if (ret)
		return ret;

	QOS_ASSERT(pp_qos_queue_set(qdev, q[6], &confq) == 0,
		   "Could not set queue\n");
	rc = pp_qos_shared_limit_group_remove(qdev, id1);
	QOS_ASSERT(rc == 0, "Removing group failed\n");

	ret = check_consistency_with_resources(qdev);
	if (ret)
		return ret;

	DUMP_QOS_TREE(qdev);

	/* Delete queues */
	for (i = 0; i < 7; ++i) {
		QOS_LOG_DEBUG("Remvove queue %d\n", i);
		pp_qos_queue_block(qdev, q[i]);
		qos_queue_flush(qdev, q[i]);
		QOS_ASSERT(pp_qos_queue_remove(qdev, q[i]) == 0,
			   "Could not remove queue %d\n", q[i]);
	}

	QOS_ASSERT(pp_qos_port_remove(qdev, p) == 0,
		   "Could not remove port\n");

	return 0;
}

static int test_internal_scheduler(struct pp_qos_dev *qdev)
{
	uint16_t phy;
	struct qos_node *base;
	int i;
	uint16_t id;
	s32 ret;

	base = get_node_from_phy(qdev->nodes, 0);
	create_nodes(qdev, 0, 2);
	create_nodes(qdev, 16, 8);
	config_parent_node(qdev, 1, TYPE_PORT, PP_QOS_ARBITRATION_WRR, 16, 8);
	config_child_node(qdev, 16, TYPE_QUEUE, 1);
	config_child_node(qdev, 17, TYPE_QUEUE, 1);
	config_child_node(qdev, 18, TYPE_QUEUE, 1);
	config_child_node(qdev, 19, TYPE_QUEUE, 1);
	config_child_node(qdev, 20, TYPE_QUEUE, 1);
	config_child_node(qdev, 21, TYPE_QUEUE, 1);
	config_child_node(qdev, 22, TYPE_QUEUE, 1);
	config_child_node(qdev, 23, TYPE_QUEUE, 1);
	ret = check_consistency(qdev);
	if (ret)
		return ret;

	for (i = 0 ; i < 100; ++i) {
		phy = phy_alloc_by_parent(qdev, base + 1, 0);
		id = pp_pool_get(qdev->ids);
		map_id_phy(qdev->mapping, id, phy);
		config_child_node(qdev, phy, TYPE_QUEUE, QOS_INVALID_PHY);
		ret = check_consistency(qdev);
		if (ret)
			return ret;
	}

	return 0;
}

static int qos_pre_test(void *data)
{
	struct pp_qos_dev *qdev;
	struct qos_node *node;
	u32 node_id, node_phy;
	s32 ret;
	u32 ring_idx;

	/* Get QoS Config */
	qdev = pp_qos_dev_open(PP_QOS_INSTANCE_ID);
	if (unlikely(ptr_is_null(qdev)))
		return PP_UT_FAIL;

	if (qdev->initialized) {
		restore_config = true;
		ret = pp_qos_config_get(qdev, &conf);
		if (unlikely(ret)) {
			pr_err("pp_qos_config_get failed\n");
			return PP_UT_FAIL;
		}
	}

	/* delete all syncqs */
	delete_all_syncq(qdev);

	/* Iterate through all port nodes */
	for (node_phy = 0; node_phy < NUM_OF_NODES; ++node_phy) {
		node = get_node_from_phy(qdev->nodes, node_phy);
		node_id = get_id_from_phy(qdev->mapping, node_phy);
		if (node_port(node))
			pp_qos_port_remove(qdev, node_id);
	}

	for (ring_idx = 0; ring_idx < QOS_TEST_NUM_RING_ADDR; ring_idx++) {
		ring_addr_virt[ring_idx] =
			pp_dma_alloc(PPV4_QOS_DESC_SIZE *
				     QOS_NUM_RING_DESCRIPTORS,
				     GFP_KERNEL, &ring_addr_phys[ring_idx],
				     true);
		if (!ring_addr_virt[ring_idx]) {
			pr_err("Could not allocate %u bytes for ring\n",
			       PPV4_QOS_DESC_SIZE * QOS_NUM_RING_DESCRIPTORS);
			return PP_UT_FAIL;
		}
	}

#ifdef FW_CHECK_SYNC
	PPUT_FAIL_TEST(check_sync_with_fw(qdev, true) == 0,
		       "DB not consistent with fw\n");
#endif
	return PP_UT_PASS;
#ifdef FW_CHECK_SYNC
done:
#endif
	return PP_UT_FAIL;
}

static int qos_post_test(void *data)
{
	static struct pp_qos_dev *qdev;
	s32 ret;
	u32 ring_idx;

	qdev = pp_qos_dev_open(PP_QOS_INSTANCE_ID);
	if (unlikely(!qdev)) {
		pr_err("qdev NULL\n");
		return PP_UT_FAIL;
	}

	/* Restore qos config */
	qos_dev_clean(qdev, false);
	if (restore_config) {
		qdev = pp_qos_dev_open(PP_QOS_INSTANCE_ID);
		if (unlikely(ptr_is_null(qdev)))
			return PP_UT_FAIL;

		ret = qos_dev_init(qdev, &conf, false);
		if (unlikely(ret)) {
			pr_err("qos_dev_init failed\n");
			return PP_UT_FAIL;
		}
	}

	for (ring_idx = 0; ring_idx < QOS_TEST_NUM_RING_ADDR; ring_idx++) {
		pp_dma_free(PPV4_QOS_DESC_SIZE *
			    QOS_NUM_RING_DESCRIPTORS,
			    ring_addr_virt[ring_idx],
			    &ring_addr_phys[ring_idx], true);
	}

#ifdef FW_CHECK_SYNC
	PPUT_FAIL_TEST(check_sync_with_fw(qdev, true) == 0,
		       "DB not consistent with fw\n");
#endif
	return PP_UT_PASS;
#ifdef FW_CHECK_SYNC
done:
#endif
	return PP_UT_FAIL;
}

static int qos_internal_api_tests(void *data)
{
	static struct pp_qos_dev *qdev;
	s32 ret;

	qdev = qos_restart(PP_QOS_INSTANCE_ID);
	if (!qdev)
		return PP_UT_FAIL;

	pr_info("test_modify_used_status\n");
	qdev = qos_restart(PP_QOS_INSTANCE_ID);
	if (!qdev)
		return PP_UT_FAIL;

	ret = test_modify_used_status(qdev);
	if (ret) {
		pr_debug("test_modify_used_status failed\n");
		return PP_UT_FAIL;
	}

	pr_info("test_phy_allocation_not_full_octet\n");
	qdev = qos_restart(PP_QOS_INSTANCE_ID);
	if (!qdev)
		return PP_UT_FAIL;

	ret = test_phy_allocation_not_full_octet(qdev);
	if (ret) {
		pr_debug("test_phy_allocation_not_full_octet failed\n");
		return PP_UT_FAIL;
	}

	pr_info("test_phy_allocation_full_octet\n");
	qdev = qos_restart(PP_QOS_INSTANCE_ID);
	if (!qdev)
		return PP_UT_FAIL;

	ret = test_phy_allocation_full_octet(qdev);
	if (ret) {
		pr_debug("test_phy_allocation_full_octet failed\n");
		return PP_UT_FAIL;
	}

	pr_info("test_phy_allocation_full_octet_2\n");
	qdev = qos_restart(PP_QOS_INSTANCE_ID);
	if (!qdev)
		return PP_UT_FAIL;

	ret = test_phy_allocation_full_octet_2(qdev);
	if (ret) {
		pr_debug("test_phy_allocation_full_octet_2 failed\n");
		return PP_UT_FAIL;
	}

	pr_info("test_internal_scheduler\n");
	qdev = qos_restart(PP_QOS_INSTANCE_ID);
	if (!qdev)
		return PP_UT_FAIL;

	ret = test_internal_scheduler(qdev);
	if (ret) {
		pr_debug("test_internal_scheduler failed\n");
		return PP_UT_FAIL;
	}

	return PP_UT_PASS;
}

static int qos_syncq_test(void *data)
{
	struct pp_qos_port_conf conf;
	struct pp_qos_queue_conf confq;
	unsigned int port_id, q_id, q_phy, prev_q_phy, sq_id, sq_phy;
	static struct pp_qos_dev *qdev;
	struct qos_node *node;

	syncq_enabled = true;

	qdev = qos_restart(PP_QOS_INSTANCE_ID);
	if (!qdev)
		return PP_UT_FAIL;

	syncq_enabled = false;

	if (!qdev->init_params.num_syncqs)
		return PP_UT_PASS;

	/* Allocate port with arbitrary phy */
	PPUT_FAIL_RETURN(!pp_qos_port_allocate(qdev, ALLOC_PORT_ID, &port_id),
			 "Could not allocate port\n");

	/* Set default port conf */
	pp_qos_port_conf_set_default(&conf);
	conf.port_parent_prop.arbitration = PP_QOS_ARBITRATION_WRR;
	conf.ring_address = ring_addr_phys[0];
	conf.ring_size = 1;

	/* Set the port conf */
	PPUT_FAIL_RETURN(!pp_qos_port_set(qdev, port_id, &conf),
			 "Could not set port %u\n", port_id);

	/* Set default queue conf */
	pp_qos_queue_conf_set_default(&confq);
	confq.queue_child_prop.parent = port_id;

	/* Allocate new queue */
	PPUT_FAIL_RETURN(!pp_qos_queue_allocate(qdev, &q_id),
			 "Queue allocate failed\n");

	/* Set queue conf */
	PPUT_FAIL_RETURN(!pp_qos_queue_set(qdev, q_id, &confq),
			 "Could not set queue %u\n", q_id);

	/* Save the original phy id */
	prev_q_phy = get_phy_from_id(qdev->mapping, q_id);
	PPUT_FAIL_RETURN(QOS_PHY_VALID(prev_q_phy),
			 "Failed to get queue %u phy id\n", q_id);

	/* Dump tree */
	DUMP_QOS_TREE(qdev);
#ifdef FW_CHECK_SYNC
	PPUT_FAIL_RETURN(!check_sync_with_fw(qdev, true),
			 "DB not consistent with fw\n");
#endif

	/* Allocate new syncq */
	PPUT_FAIL_RETURN(!pp_qos_queue_allocate(qdev, &sq_id),
			 "Sync queue allocate failed\n");

	/* Set default queue conf */
	pp_qos_queue_conf_set_default(&confq);

	/* Add the syncq */
	PPUT_FAIL_RETURN(!qos_sync_queue_add(qdev, sq_id, &sq_phy, &confq),
			 "Sync queue %u add failed\n", sq_id);

	/* Connect the syncq */
	PPUT_FAIL_RETURN(!qos_sync_queue_connect(qdev, sq_id, q_id),
			 "Sync queue %u connect failed\n", sq_id);

	/* Dump tree */
	DUMP_QOS_TREE(qdev);

	/* Get original queue conf */
	node = get_node_from_phy(qdev->nodes,
				 get_phy_from_id(qdev->mapping, q_id));

	PPUT_FAIL_RETURN(node, "Failed to get queue node\n");

	/* Get the parent phy */
	PPUT_FAIL_RETURN(QOS_PHY_VALID(node->child_prop.parent_phy),
			 "Failed to get the parent sched phy\n");

	/* Compare the parent phy with the original queue phy */
	PPUT_FAIL_RETURN(node->child_prop.parent_phy == prev_q_phy,
			 "Orig queue %u is not connected to scheduler\n", q_id);

	/* disconnect the syncq */
	PPUT_FAIL_RETURN(!qos_sync_queue_disconnect(qdev, sq_id),
			 "Sync queue %u disconnect failed\n", sq_id);

	/* Dump tree */
	DUMP_QOS_TREE(qdev);

	/* Get the queue phy */
	q_phy = get_phy_from_id(qdev->mapping, q_id);
	PPUT_FAIL_RETURN(QOS_PHY_VALID(q_phy),
			 "Failed to get the queue phy\n");

	/* Compare the phy with the original queue phy */
	PPUT_FAIL_RETURN(q_phy == prev_q_phy,
		  "Orig queue %u wasn't return to the same phy\n", q_id);

	delete_all_syncq(qdev);

	PPUT_FAIL_RETURN(!pp_qos_queue_remove(qdev, q_id),
			 "Queue remove failed\n");

	PPUT_FAIL_RETURN(!pp_qos_port_remove(qdev, port_id),
			 "Port remove failed\n");

#ifdef FW_CHECK_SYNC
	PPUT_FAIL_RETURN(!check_sync_with_fw(qdev, true),
			 "DB not consistent with fw\n");
#endif

	return PP_UT_PASS;
}

static int qos_advance_tests(void *data)
{
	static struct pp_qos_dev *qdev;
	s32 ret;

	qdev = qos_restart(PP_QOS_INSTANCE_ID);
	if (!qdev)
		return PP_UT_FAIL;

	if (qdev->init_params.max_ports <= 15) {
		QOS_LOG_ERR("max port will be at least 15 !!!!\n");
		return PP_UT_FAIL;
	}

	pr_info("test_ports\n");
	qdev = qos_restart(PP_QOS_INSTANCE_ID);
	if (!qdev)
		return PP_UT_FAIL;

	ret = test_ports(qdev);
	if (ret)
		return PP_UT_FAIL;

#ifdef FW_CHECK_SYNC
	PPUT_FAIL_TEST(check_sync_with_fw(qdev, true) == 0,
		       "DB not consistent with fw\n");
#endif

	pr_info("test_queues1\n");
	qdev = qos_restart(PP_QOS_INSTANCE_ID);
	if (!qdev)
		return PP_UT_FAIL;

	ret = test_queues1(qdev);
	if (ret)
		return PP_UT_FAIL;

#ifdef FW_CHECK_SYNC
	PPUT_FAIL_TEST(check_sync_with_fw(qdev, true) == 0,
		       "DB not consistent with fw\n");
#endif

	pr_info("test_queues2\n");
	qdev = qos_restart(PP_QOS_INSTANCE_ID);
	if (!qdev)
		return PP_UT_FAIL;

	ret = test_queues2(qdev);
	if (ret)
		return PP_UT_FAIL;

#ifdef FW_CHECK_SYNC
	PPUT_FAIL_TEST(check_sync_with_fw(qdev, true) == 0,
		       "DB not consistent with fw\n");
#endif

	pr_info("test_queues3\n");
	qdev = qos_restart(PP_QOS_INSTANCE_ID);
	if (!qdev)
		return PP_UT_FAIL;

	ret = test_queues3(qdev);
	if (ret)
		return PP_UT_FAIL;

#ifdef FW_CHECK_SYNC
	PPUT_FAIL_TEST(check_sync_with_fw(qdev, true) == 0,
		       "DB not consistent with fw\n");
#endif

	pr_info("test_shared_group\n");
	qdev = qos_restart(PP_QOS_INSTANCE_ID);
	if (!qdev)
		return PP_UT_FAIL;

#ifdef FW_CHECK_SYNC
	PPUT_FAIL_TEST(check_sync_with_fw(qdev, true) == 0,
		       "DB not consistent with fw\n");
#endif

	ret = test_shared_group(qdev);
	if (ret)
		return PP_UT_FAIL;

#ifdef FW_CHECK_SYNC
	PPUT_FAIL_TEST(check_sync_with_fw(qdev, true) == 0,
		       "DB not consistent with fw\n");
#endif

	return PP_UT_PASS;
#ifdef FW_CHECK_SYNC
done:
#endif
	return PP_UT_FAIL;
}

static int qos_falcon_test(void *data)
{
	unsigned int i;
	unsigned int p[3] = {PP_QOS_INVALID_ID};
	unsigned int p89 = PP_QOS_INVALID_ID;
	unsigned int q[10] = {PP_QOS_INVALID_ID};
	static struct pp_qos_dev *qdev;
	struct pp_qos_port_conf confp;
	struct pp_qos_queue_conf confq;
	struct pp_qos_node_info info;
	unsigned int rlms[] = {14, 74, 30, 87, 235, 42, 242, 190, 119, 103};
	s32 ret;
	u32 test_ret = PP_UT_FAIL;

	qdev = qos_restart(PP_QOS_INSTANCE_ID);
	if (!qdev)
		return PP_UT_FAIL;

	/* Hacking rlms array to give rlm that the slim driver currently uses */
	for (i = 0; i < ARRAY_SIZE(rlms); ++i) {
		qdev->rlms->data[QOS_TEST_MAX_QUEUES - 1 - i] = rlms[i];
		qdev->rlms->data[QOS_TEST_MAX_QUEUES - 1 - rlms[i]] = i;
	}

	/* Configure ports 0, 7, 26 */
	PPUT_FAIL_TEST(pp_qos_port_allocate(qdev, 0, &p[0]) == 0,
		       "Could not allocate port\n");
	PPUT_FAIL_TEST(pp_qos_port_allocate(qdev, 7, &p[1]) == 0,
		       "Could not allocate port\n");
	PPUT_FAIL_TEST(pp_qos_port_allocate(qdev, 26, &p[2]) == 0,
		       "Could not allocate port\n");
	PPUT_FAIL_TEST(pp_qos_port_allocate(qdev, 89, &p89) == 0,
		       "Could not allocate port\n");
	pp_qos_port_conf_set_default(&confp);

	confp.port_parent_prop.arbitration = PP_QOS_ARBITRATION_WRR;
	confp.ring_size = 1;
	for (i = 0 ; i < 3; ++i) {
		confp.ring_address = ring_addr_phys[i];
		if (i == 0)
			confp.credit = 2;
		else
			confp.credit = 8;
		PPUT_FAIL_TEST(pp_qos_port_set(qdev, p[i], &confp) == 0,
			       "Could not set port\n");
	}

	PPUT_FAIL_TEST(pp_qos_port_set(qdev, p89, &confp) == 0,
		       "Could not set port\n");
	pp_qos_queue_conf_set_default(&confq);
	confq.wred_max_allowed = 0x400;
	confq.queue_child_prop.parent = p89;
	for (i = 0; i < 10; ++i) {
		PPUT_FAIL_TEST(pp_qos_queue_allocate(qdev, q + i) == 0,
			       "Could not allocate queue\n");
		PPUT_FAIL_TEST(pp_qos_queue_set(qdev, q[i], &confq) == 0,
			       "Setting failed on %d iteration\n", i);
	}

	for (i = 0; i < 10; ++i) {
		if (i < 4)
			confq.queue_child_prop.parent = p[0];
		else if (i < 7)
			confq.queue_child_prop.parent = p[1];
		else
			confq.queue_child_prop.parent = p[2];
		confq.blocked = 1;
		PPUT_FAIL_TEST(pp_qos_queue_set(qdev, q[i], &confq) == 0,
			       "Setting failed on %d iteration\n", i);
		pp_qos_queue_unblock(qdev, q[i]);
	}

	ret = check_consistency_with_resources(qdev);
	if (ret)
		goto done;

	PPUT_FAIL_TEST(qos_get_node_info(qdev, p89, &info) == 0,
		       "Could not get node info\n");

	for (i = 0; i < 3; ++i) {
		PPUT_FAIL_TEST(qos_get_node_info(qdev, p[i], &info) == 0,
			       "Could not get node info\n");
	}

	for (i = 0; i < 10; ++i) {
		PPUT_FAIL_TEST(qos_get_node_info(qdev, q[i], &info) == 0,
			       "Could not get node info\n");
	}

	ret = check_consistency_with_resources(qdev);
	if (ret)
		goto done;

#ifndef PP_QOS_DISABLE_CMDQ
#ifdef FW_CHECK_SYNC
	PPUT_FAIL_TEST(check_sync_with_fw(qdev, false) == 0,
		       "DB not consistent with fw\n");
#endif
#endif
	test_ret = PP_UT_PASS;
	DUMP_QOS_TREE(qdev);
done:
	for (i = 0; i < 10; ++i) {
		pp_qos_queue_block(qdev, q[i]);
		qos_queue_flush(qdev, q[i]);
		PPUT_FAIL_RETURN(pp_qos_queue_remove(qdev, q[i]) == 0,
				 "Could not remove queue %d\n", q[i]);
	}

	for (i = 0; i < 3; ++i) {
		PPUT_FAIL_RETURN(pp_qos_port_remove(qdev, p[i]) == 0,
				 "Could not remove port %d\n", p[i]);
	}

	PPUT_FAIL_RETURN(pp_qos_port_remove(qdev, p89) == 0,
			 "Could not remove port %d\n", p89);

	return test_ret;
}

static int reposition_test(void *data)
{
	unsigned int s[5] = {PP_QOS_INVALID_ID};
	unsigned int q = PP_QOS_INVALID_ID;
	unsigned int p1 = PP_QOS_INVALID_ID;
	unsigned int p7 = PP_QOS_INVALID_ID;
	int tmp;
	int i;
	struct pp_qos_port_conf confp;
	struct pp_qos_sched_conf confs;
	struct pp_qos_queue_conf confq;
	struct pp_qos_dev *qdev;
	struct pp_qos_node_info info;
	s32 ret;
	u32 test_ret = PP_UT_FAIL;

	qdev = qos_restart(PP_QOS_INSTANCE_ID);
	if (!qdev)
		return PP_UT_FAIL;

	tmp = pp_qos_port_allocate(qdev, 1, &p1);
	PPUT_FAIL_TEST(tmp == 0, "could not allocate qos port\n");

	tmp = pp_qos_port_allocate(qdev, 7, &p7);
	PPUT_FAIL_TEST(tmp == 0, "could not allocate qos port\n");

	pp_qos_port_conf_set_default(&confp);
	confp.port_parent_prop.arbitration = PP_QOS_ARBITRATION_WRR;
	confp.ring_size = 1;
	confp.ring_address = ring_addr_phys[0];
	PPUT_FAIL_TEST(pp_qos_port_set(qdev, p1, &confp) == 0,
		       "Could not set port\n");

	confp.ring_address = ring_addr_phys[1];
	PPUT_FAIL_TEST(pp_qos_port_set(qdev, p7, &confp) == 0,
	"Could not set port\n");

	pp_qos_sched_conf_set_default(&confs);
	for (i = 0; i < 5; ++i) {
		PPUT_FAIL_TEST(pp_qos_sched_allocate(qdev, &s[i]) == 0,
			       "Could not allocate sched\n");
		if (i == 0)
			confs.sched_child_prop.parent = p1;
		else
			confs.sched_child_prop.parent = s[i - 1];
		PPUT_FAIL_TEST(pp_qos_sched_set(qdev, s[i], &confs) == 0,
			       "Could not set sched\n");
	}
	PPUT_FAIL_TEST(pp_qos_queue_allocate(qdev, &q) == 0,
		       "Could not allocate queue\n");
	pp_qos_queue_conf_set_default(&confq);
	confq.queue_child_prop.parent = s[4];
	PPUT_FAIL_TEST(pp_qos_queue_set(qdev, q, &confq) == 0,
		       "Could not set queue\n");

	confs.sched_child_prop.parent = p1;
	PPUT_FAIL_TEST(pp_qos_sched_set(qdev, s[3], &confs) == 0,
		       "Could not set sched\n");

	confs.sched_child_prop.parent = p7;
	PPUT_FAIL_TEST(pp_qos_sched_set(qdev, s[4], &confs) != 0,
		       "Moving sched should have failed\n");
	pp_qos_queue_block(qdev, q);
	PPUT_FAIL_TEST(pp_qos_sched_set(qdev, s[4], &confs) == 0,
		       "Moving sched should have passed\n");
	pp_qos_queue_unblock(qdev, q);
	ret = check_consistency_with_resources(qdev);
	if (ret)
		goto done;

	PPUT_FAIL_TEST(qos_get_node_info(qdev, p1, &info) == 0,
		       "Could not get node info\n");
	PPUT_FAIL_TEST(qos_get_node_info(qdev, p7, &info) == 0,
		       "Could not get node info\n");
	PPUT_FAIL_TEST(qos_get_node_info(qdev, q, &info) == 0,
		       "Could not get node info\n");

	for (i = 0; i < 5; ++i) {
		PPUT_FAIL_TEST(qos_get_node_info(qdev, s[i], &info) == 0,
			       "Could not get node info\n");
	}

	ret = check_consistency_with_resources(qdev);
	if (ret)
		goto done;

#ifndef PP_QOS_DISABLE_CMDQ
#ifdef FW_CHECK_SYNC
	PPUT_FAIL_TEST(check_sync_with_fw(qdev, false) == 0,
		       "DB not consistent with fw\n");
#endif
#endif
	test_ret = PP_UT_PASS;
	DUMP_QOS_TREE(qdev);
done:
	pp_qos_queue_block(qdev, q);
	qos_queue_flush(qdev, q);
	PPUT_FAIL_RETURN(pp_qos_queue_remove(qdev, q) == 0,
			 "Could not remove queue %d\n", q);

	for (i = 4; i >= 0; --i) {
		PPUT_FAIL_RETURN(pp_qos_sched_remove(qdev, s[i]) == 0,
				 "Could not remove sched %d\n", s[i]);
	}

	PPUT_FAIL_RETURN(pp_qos_port_remove(qdev, p1) == 0,
			 "Could not remove port %d\n", p1);

	PPUT_FAIL_RETURN(pp_qos_port_remove(qdev, p7) == 0,
			 "Could not remove port %d\n", p7);

	return test_ret;
}

static int wa_test(void *data)
{
	struct pp_qos_dev *qdev;
	unsigned int p0 = PP_QOS_INVALID_ID;
	unsigned int p7 = PP_QOS_INVALID_ID;
	unsigned int p25 = PP_QOS_INVALID_ID;
	unsigned int q0 = PP_QOS_INVALID_ID;
	unsigned int q1 = PP_QOS_INVALID_ID;
	unsigned int q2 = PP_QOS_INVALID_ID;
	struct pp_qos_port_conf confp;
	struct pp_qos_queue_conf confq;
	s32 ret;
	u32 test_ret = PP_UT_FAIL;

	qdev = qos_restart(PP_QOS_INSTANCE_ID);
	if (!qdev)
		return PP_UT_FAIL;

	pp_qos_port_conf_set_default(&confp);
	confp.port_parent_prop.arbitration = PP_QOS_ARBITRATION_WSP;
	confp.disable = 1;
	confp.ring_size = 1;

	/* add port 25 */
	PPUT_FAIL_TEST(pp_qos_port_allocate(qdev, 25, &p25) == 0,
	"Could not allocate port 25\n");
	confp.disable = 1;
	confp.packet_credit_enable = 1;
	confp.credit = 2;
	confp.ring_address = ring_addr_phys[0];
	PPUT_FAIL_TEST(pp_qos_port_set(qdev, p25, &confp) == 0,
	"Could not set port 0\n");

	pp_qos_queue_conf_set_default(&confq);
	confq.wred_max_allowed = 0x400;
	confq.blocked = 0;

	PPUT_FAIL_TEST(pp_qos_queue_allocate(qdev, &q0) == 0,
	"Could not allocate queue\n");
	confq.queue_child_prop.parent = p25;
	PPUT_FAIL_TEST(pp_qos_queue_set(qdev, q0, &confq) == 0,
	"Setting queue fail\n");


	/* add port 0 */
	confp.packet_credit_enable = 1;
	PPUT_FAIL_TEST(pp_qos_port_allocate(qdev, 0, &p0) == 0,
	"Could not allocate port 0\n");
	confp.disable = 0;
	confp.credit = 2;
	confp.ring_address = ring_addr_phys[1];
	PPUT_FAIL_TEST(pp_qos_port_set(qdev, p0, &confp) == 0,
	"Could not set port 7\n");

	PPUT_FAIL_TEST(pp_qos_queue_allocate(qdev, &q1) == 0,
	"Could not allocate queue\n");

	confq.queue_child_prop.parent = p25;
	confq.blocked = 0;
	confq.queue_child_prop.priority = 2;
	PPUT_FAIL_TEST(pp_qos_queue_set(qdev, q1, &confq) == 0,
	"Setting queue fail\n");

	confq.queue_child_prop.parent = p0;
	pp_qos_queue_block(qdev, q1);
	PPUT_FAIL_TEST(pp_qos_queue_set(qdev, q1, &confq) == 0,
	"Setting queue fail\n");
	pp_qos_queue_unblock(qdev, q1);

	PPUT_FAIL_TEST(pp_qos_port_allocate(qdev, 7, &p7) == 0,
	"Could not allocate port 0\n");
	confp.credit = 8;
	confp.ring_address = ring_addr_phys[2];
	PPUT_FAIL_TEST(pp_qos_port_set(qdev, p7, &confp) == 0,
	"Could not set port 7\n");

	PPUT_FAIL_TEST(pp_qos_queue_allocate(qdev, &q2) == 0,
	"Could not allocate queue\n");
	confq.queue_child_prop.parent = p25;
	confq.queue_child_prop.priority = 3;
	PPUT_FAIL_TEST(pp_qos_queue_set(qdev, q2, &confq) == 0,
	"Setting queue fail\n");

	confq.queue_child_prop.parent = p7;
	confq.queue_child_prop.priority = 0;
	pp_qos_queue_block(qdev, q2);
	PPUT_FAIL_TEST(pp_qos_queue_set(qdev, q2, &confq) == 0,
	"Setting queue fail\n");
	pp_qos_queue_unblock(qdev, q2);

	confq.queue_child_prop.priority = 2;
	pp_qos_queue_block(qdev, q1);
	PPUT_FAIL_TEST(pp_qos_queue_set(qdev, q1, &confq) == 0,
	"Setting queue fail\n");
	pp_qos_queue_unblock(qdev, q1);

	ret = check_consistency_with_resources(qdev);
	if (ret)
		goto done;

#ifndef PP_QOS_DISABLE_CMDQ
#ifdef FW_CHECK_SYNC
	PPUT_FAIL_TEST(check_sync_with_fw(qdev, false) == 0,
		       "DB not consistent with fw\n");
#endif
#endif
	test_ret = PP_UT_PASS;
	DUMP_QOS_TREE(qdev);
done:
	pp_qos_queue_block(qdev, q0);
	qos_queue_flush(qdev, q0);
	PPUT_FAIL_RETURN(pp_qos_queue_remove(qdev, q0) == 0,
			 "Could not remove queue %d\n", q0);

	pp_qos_queue_block(qdev, q1);
	qos_queue_flush(qdev, q1);
	PPUT_FAIL_RETURN(pp_qos_queue_remove(qdev, q1) == 0,
			 "Could not remove queue %d\n", q1);

	pp_qos_queue_block(qdev, q2);
	qos_queue_flush(qdev, q2);
	PPUT_FAIL_RETURN(pp_qos_queue_remove(qdev, q2) == 0,
			 "Could not remove queue %d\n", q2);

	PPUT_FAIL_RETURN(pp_qos_port_remove(qdev, p0) == 0,
			 "Could not remove queue %d\n", p0);

	PPUT_FAIL_RETURN(pp_qos_port_remove(qdev, p7) == 0,
			 "Could not remove queue %d\n", p7);

	PPUT_FAIL_RETURN(pp_qos_port_remove(qdev, p25) == 0,
			 "Could not remove queue %d\n", p25);

	return test_ret;
}

/*
 *       p1                       p7
 *   s0      s1                   q3
 * q0  s2    q1
 *     q2
 */
static int test_cmd_queue(void *data)
{
	struct pp_qos_dev *qdev;
	struct pp_qos_port_conf confp;
	struct pp_qos_sched_conf confs;
	struct pp_qos_queue_conf confq;
	unsigned int p1 = PP_QOS_INVALID_ID;
	unsigned int p7 = PP_QOS_INVALID_ID;
	unsigned int s0 = PP_QOS_INVALID_ID;
	unsigned int s1 = PP_QOS_INVALID_ID;
	unsigned int s2 = PP_QOS_INVALID_ID;
	unsigned int q0 = PP_QOS_INVALID_ID;
	unsigned int q1 = PP_QOS_INVALID_ID;
	unsigned int q2 = PP_QOS_INVALID_ID;
	unsigned int q3 = PP_QOS_INVALID_ID;
	unsigned int q[7] = {PP_QOS_INVALID_ID};
	unsigned int i;
	unsigned int firstnonport;
	s32 ret;
	u32 test_ret = PP_UT_FAIL;

	pr_debug("\n");

	qdev = qos_restart(PP_QOS_INSTANCE_ID);
	if (!qdev)
		return PP_UT_FAIL;

	firstnonport = qdev->init_params.max_ports;
	// Add ports 1 and 7
	PPUT_FAIL_TEST(pp_qos_port_allocate(qdev, 1, &p1) == 0,
		       "Could not allocate port\n");
	PPUT_FAIL_TEST(pp_qos_port_allocate(qdev, 7, &p7) == 0,
		       "Could not allocate port\n");
	pp_qos_port_conf_set_default(&confp);
	confp.port_parent_prop.arbitration = PP_QOS_ARBITRATION_WRR;
	confp.ring_address = ring_addr_phys[0];
	confp.ring_size = 1;
	PPUT_FAIL_TEST(pp_qos_port_set(qdev, p1, &confp) == 0,
		       "Could not set port\n");
	PPUT_FAIL_TEST(pp_qos_port_set(qdev, p7, &confp) == 0,
		       "Could not set port\n");

	// Add schedulers
	pr_debug("\n");
	PPUT_FAIL_TEST(pp_qos_sched_allocate(qdev, &s0) == 0,
		       "Could not allocate sched\n");
	PPUT_FAIL_TEST(pp_qos_sched_allocate(qdev, &s1) == 0,
		       "Could not allocate sched\n");
	PPUT_FAIL_TEST(pp_qos_sched_allocate(qdev, &s2) == 0,
		       "Could not allocate sched\n");
	pp_qos_sched_conf_set_default(&confs);
	confs.sched_parent_prop.arbitration = PP_QOS_ARBITRATION_WRR;
	confs.sched_child_prop.parent = p1;
	PPUT_FAIL_TEST(pp_qos_sched_set(qdev, s0, &confs) == 0,
		       "Could not set sched\n");
	PPUT_FAIL_TEST(pp_qos_sched_set(qdev, s1, &confs) == 0,
		       "Could not set sched\n");
	confs.sched_child_prop.parent = s0;
	confs.sched_child_prop.wrr_weight = 7;
	PPUT_FAIL_TEST(pp_qos_sched_set(qdev, s2, &confs) == 0,
		       "Could not set sched\n");

	// Add queues
	pr_debug("\n");
	PPUT_FAIL_TEST(pp_qos_queue_allocate(qdev, &q0) == 0,
		       "Could not allocate queue\n");
	PPUT_FAIL_TEST(pp_qos_queue_allocate(qdev, &q1) == 0,
		       "Could not allocate queue\n");
	PPUT_FAIL_TEST(pp_qos_queue_allocate(qdev, &q2) == 0,
		       "Could not allocate queue\n");
	PPUT_FAIL_TEST(pp_qos_queue_allocate(qdev, &q3) == 0,
		       "Could not allocate queue\n");
	pp_qos_queue_conf_set_default(&confq);
	confq.queue_child_prop.parent = s0;
	PPUT_FAIL_TEST(pp_qos_queue_set(qdev, q0, &confq) == 0,
		       "Could not set queue\n");
	confq.queue_child_prop.parent = s1;
	PPUT_FAIL_TEST(pp_qos_queue_set(qdev, q1, &confq) == 0,
		       "Could not set queue\n");
	confq.queue_child_prop.parent = s2;
	PPUT_FAIL_TEST(pp_qos_queue_set(qdev, q2, &confq) == 0,
		       "Could not set queue\n");
	confq.queue_child_prop.parent = p7;
	PPUT_FAIL_TEST(pp_qos_queue_set(qdev, q3, &confq) == 0,
		       "Could not set queue\n");

	pr_debug("\n");
	ret = check_consistency_with_resources(qdev);
	if (ret)
		goto done;

	/* s0 has 2 children, adding 5 more */
	pr_debug("\n");
	confq.queue_child_prop.parent = s0;
	confq.queue_child_prop.wrr_weight = 7;
	for (i = 0; i < 5; ++i) {
		PPUT_FAIL_TEST(pp_qos_queue_allocate(qdev, &q[i]) == 0,
			       "Could not allocate queue\n");
		PPUT_FAIL_TEST(pp_qos_queue_set(qdev, q[i], &confq) == 0,
			       "Could not set queue\n");
	}

	PPUT_FAIL_TEST(pp_qos_queue_allocate(qdev, &q[5]) == 0,
		       "Could not allocate queue\n");
	PPUT_FAIL_TEST(pp_qos_queue_set(qdev, q[5], &confq) == 0,
		       "Could not set queue\n");

	/* This should call create internal scheduler */
	PPUT_FAIL_TEST(pp_qos_queue_allocate(qdev, &q[6]) == 0,
		       "Could not allocate queue\n");
	PPUT_FAIL_TEST(pp_qos_queue_set(qdev, q[6], &confq) == 0,
		       "Could not set queue\n");

	test_ret = PP_UT_PASS;
	DUMP_QOS_TREE(qdev);
done:
	pp_qos_queue_block(qdev, q0);
	qos_queue_flush(qdev, q0);
	PPUT_FAIL_RETURN(pp_qos_queue_remove(qdev, q0) == 0,
			 "Could not remove queue %d\n", q0);

	pp_qos_queue_block(qdev, q1);
	qos_queue_flush(qdev, q1);
	PPUT_FAIL_RETURN(pp_qos_queue_remove(qdev, q1) == 0,
			 "Could not remove queue %d\n", q1);

	pp_qos_queue_block(qdev, q2);
	qos_queue_flush(qdev, q2);
	PPUT_FAIL_RETURN(pp_qos_queue_remove(qdev, q2) == 0,
			 "Could not remove queue %d\n", q2);

	pp_qos_queue_block(qdev, q3);
	qos_queue_flush(qdev, q3);
	PPUT_FAIL_RETURN(pp_qos_queue_remove(qdev, q3) == 0,
			 "Could not remove queue %d\n", q3);

	for (i = 0; i < 7; ++i) {
		pp_qos_queue_block(qdev, q[i]);
		qos_queue_flush(qdev, q[i]);
		PPUT_FAIL_RETURN(pp_qos_queue_remove(qdev, q[i]) == 0,
				 "Could not remove queue %d\n", q[i]);
	}

	PPUT_FAIL_RETURN(pp_qos_sched_remove(qdev, s2) == 0,
			 "Could not remove sched %d\n", s2);

	PPUT_FAIL_RETURN(pp_qos_sched_remove(qdev, s0) == 0,
			 "Could not remove sched %d\n", s0);

	PPUT_FAIL_RETURN(pp_qos_sched_remove(qdev, s1) == 0,
			 "Could not remove sched %d\n", s1);

	PPUT_FAIL_RETURN(pp_qos_port_remove(qdev, p1) == 0,
			 "Could not remove queue %d\n", p1);

	PPUT_FAIL_RETURN(pp_qos_port_remove(qdev, p7) == 0,
			 "Could not remove queue %d\n", p7);

	pr_debug("\n");
	ret = check_consistency_with_resources(qdev);
	if (ret) {
		pr_err("check_consistency_with_resources failed\n");
		return PP_UT_FAIL;
	}

#ifdef FW_CHECK_SYNC
	if (check_sync_with_fw(qdev, true))
		pr_err("DB not consistent with fw\n");
#endif

	return test_ret;
}

static int dual_rate_shaper_test(void *data)
{
	struct pp_qos_dev *qdev;
	struct pp_qos_port_conf confp;
	struct pp_qos_sched_conf confs;
	struct pp_qos_queue_conf confq;
	unsigned int p1 = PP_QOS_INVALID_ID;
	unsigned int p7 = PP_QOS_INVALID_ID;
	unsigned int s[11];
	unsigned int q[11];
	unsigned int i;
	unsigned int firstnonport;
	u16 ids[10];
	u32 num_queues;
	s32 ret;
	u32 test_ret = PP_UT_FAIL;

	pr_debug("\n");

	qdev = qos_restart(PP_QOS_INSTANCE_ID);
	if (!qdev)
		return PP_UT_FAIL;

	firstnonport = qdev->init_params.max_ports;
	// Add ports 1
	PPUT_FAIL_TEST(pp_qos_port_allocate(qdev, 1, &p1) == 0,
		       "Could not allocate port\n");
	// Add ports 7
	PPUT_FAIL_TEST(pp_qos_port_allocate(qdev, 7, &p7) == 0,
		       "Could not allocate port\n");

	pp_qos_port_conf_set_default(&confp);
	confp.ring_address = ring_addr_phys[0];
	confp.ring_size = 1;

	confp.port_parent_prop.arbitration = PP_QOS_ARBITRATION_WRR;
	PPUT_FAIL_TEST(pp_qos_port_set(qdev, p1, &confp) == 0,
		       "Could not set port\n");

	confp.port_parent_prop.arbitration = PP_QOS_ARBITRATION_WSP;
	PPUT_FAIL_TEST(pp_qos_port_set(qdev, p7, &confp) == 0,
		       "Could not set port\n");

	pr_debug("\n");

	pp_qos_sched_conf_set_default(&confs);
	pp_qos_queue_conf_set_default(&confq);

	confs.sched_parent_prop.arbitration = PP_QOS_ARBITRATION_WRR;
	confs.sched_child_prop.parent = p1;
	confs.sched_child_prop.priority = 0;

	/* Add schedulers */
	for (i = 0; i < 5; i++) {
		PPUT_FAIL_TEST(pp_qos_sched_allocate(qdev, &s[i]) == 0,
			       "Could not allocate sched\n");
		PPUT_FAIL_TEST(pp_qos_queue_allocate(qdev, &q[i]) == 0,
			       "Could not allocate queue\n");
		PPUT_FAIL_TEST(pp_qos_sched_set(qdev, s[i], &confs) == 0,
			       "Could not set sched\n");
		confs.sched_child_prop.parent = s[i];
		confq.queue_child_prop.parent = s[i];
		PPUT_FAIL_TEST(pp_qos_queue_set(qdev, q[i], &confq) == 0,
			       "Could not set queue\n");
	}

	PPUT_FAIL_TEST(pp_qos_sched_allocate(qdev, &s[5]) == 0,
		       "Could not allocate sched\n");
	confs.sched_child_prop.parent = s[4];
	PPUT_FAIL_TEST(pp_qos_sched_set(qdev, s[5], &confs) != 0,
		       "Succeeded set sched\n");
	pp_pool_put(qdev->ids, s[5]);

	/* Now test for WSP port */
	confs.sched_child_prop.parent = p7;
	confs.sched_child_prop.priority = 0;

	/* Add schedulers */
	for (i = 5; i < 8; i++) {
		PPUT_FAIL_TEST(pp_qos_sched_allocate(qdev, &s[i]) == 0,
			       "Could not allocate sched\n");
		PPUT_FAIL_TEST(pp_qos_queue_allocate(qdev, &q[i]) == 0,
			       "Could not allocate queue\n");
		PPUT_FAIL_TEST(pp_qos_sched_set(qdev, s[i], &confs) == 0,
			       "Could not set sched\n");
		confs.sched_child_prop.parent = s[i];
		confq.queue_child_prop.parent = s[i];
		if (i == 7)
			confq.queue_child_prop.priority = 7;
		PPUT_FAIL_TEST(pp_qos_queue_set(qdev, q[i], &confq) == 0,
			       "Could not set queue\n");
	}

	/* Make sure only 3 queues under the WSP port */
	pp_qos_port_get_queues(qdev, p7, ids, 10, &num_queues);
	PPUT_FAIL_TEST(num_queues == 3,
		       "Num queues %u is not expected (expected 3 queues)\n",
		       num_queues);

	confq.queue_child_prop.priority = 0;

	PPUT_FAIL_TEST(pp_qos_sched_allocate(qdev, &s[8]) == 0,
		       "Could not allocate sched\n");
	confs.sched_child_prop.parent = s[7];
	PPUT_FAIL_TEST(pp_qos_sched_set(qdev, s[8], &confs) != 0,
		       "Succeeded set sched\n");
	pp_pool_put(qdev->ids, s[8]);

	pr_debug("\n");

	confq.common_prop.bandwidth_limit = 2048;
	confq.eir = 1024;
	for (i = 5; i < 8; i++) {
		confq.queue_child_prop.parent = s[i];
		if (i == 7)
			confq.queue_child_prop.priority = 1;
		pp_qos_queue_block(qdev, q[i]);
		PPUT_FAIL_TEST(pp_qos_queue_set(qdev, q[i], &confq) == 0,
			       "Could not set queue\n");
		pp_qos_queue_unblock(qdev, q[i]);
	}

	/* Make sure only 3 queues under the WSP port after apllying eir */
	pp_qos_port_get_queues(qdev, p7, ids, 10, &num_queues);
	PPUT_FAIL_TEST(num_queues == 3,
		       "Num queues %u is not expected (expected 3 queues)\n",
		       num_queues);

#ifdef FW_CHECK_SYNC
	if (check_sync_with_fw(qdev, true))
		pr_err("DB not consistent with fw\n");
#endif

	confq.eir = 0;
	PPUT_FAIL_TEST(pp_qos_queue_set(qdev, q[7], &confq) == 0,
		       "Could not set queue\n");

	confq.queue_child_prop.priority = 0;

	confp.port_parent_prop.arbitration = PP_QOS_ARBITRATION_WSP;
	PPUT_FAIL_TEST(pp_qos_port_set(qdev, p1, &confp) != 0,
		       "port arb change succeeds\n");

	confp.port_parent_prop.arbitration = PP_QOS_ARBITRATION_WRR;
	PPUT_FAIL_TEST(pp_qos_port_set(qdev, p7, &confp) != 0,
		       "port arb change succeeds\n");

	ret = check_consistency_with_resources(qdev);
	if (ret)
		goto done;

	test_ret = PP_UT_PASS;
	DUMP_QOS_TREE(qdev);
done:
	PPUT_FAIL_TEST(pp_qos_port_remove(qdev, p1) == 0,
		       "Could not remove port\n");
	PPUT_FAIL_TEST(pp_qos_port_remove(qdev, p7) == 0,
		       "Could not remove port\n");

	pr_debug("\n");
	ret = check_consistency_with_resources(qdev);
	if (ret) {
		pr_err("check_consistency_with_resources failed\n");
		return PP_UT_FAIL;
	}

#ifdef FW_CHECK_SYNC
	if (check_sync_with_fw(qdev, true))
		pr_err("DB not consistent with fw\n");
#endif

	return test_ret;
}

static int same_priority_test(void *data)
{
	struct pp_qos_dev *qdev;
	struct pp_qos_port_conf confp;
	struct pp_qos_queue_conf confq;
	unsigned int p1 = PP_QOS_INVALID_ID;
	unsigned int q1;
	unsigned int q2;
	s32 ret;
	u32 test_ret = PP_UT_FAIL;

	pr_debug("\n");

	qdev = qos_restart(PP_QOS_INSTANCE_ID);
	if (!qdev)
		return PP_UT_FAIL;

	// Add ports 1
	PPUT_FAIL_TEST(pp_qos_port_allocate(qdev, 1, &p1) == 0,
		       "Could not allocate port\n");

	pp_qos_port_conf_set_default(&confp);
	confp.ring_address = ring_addr_phys[0];
	confp.ring_size = 1;

	PPUT_FAIL_TEST(pp_qos_port_set(qdev, p1, &confp) == 0,
		       "Could not set port\n");

	pr_debug("\n");

	pp_qos_queue_conf_set_default(&confq);
	confq.queue_child_prop.priority = 4;

	PPUT_FAIL_TEST(pp_qos_queue_allocate(qdev, &q1) == 0,
		       "Could not allocate queue\n");
	PPUT_FAIL_TEST(pp_qos_queue_set(qdev, q1, &confq) == 0,
		       "Could not set queue\n");

	PPUT_FAIL_TEST(pp_qos_queue_allocate(qdev, &q2) == 0,
		       "Could not allocate queue\n");
	PPUT_FAIL_TEST(pp_qos_queue_set(qdev, q2, &confq) != 0,
		       "Set q with same prio succeeds\n");
	pp_pool_put(qdev->ids, q2);

	ret = check_consistency_with_resources(qdev);
	if (ret)
		goto done;

	test_ret = PP_UT_PASS;
	DUMP_QOS_TREE(qdev);
done:

	PPUT_FAIL_TEST(pp_qos_port_remove(qdev, p1) == 0,
		       "Could not remove port\n");

	pr_debug("\n");
	ret = check_consistency_with_resources(qdev);
	if (ret) {
		pr_err("check_consistency_with_resources failed\n");
		return PP_UT_FAIL;
	}

#ifdef FW_CHECK_SYNC
	if (check_sync_with_fw(qdev, true))
		pr_err("DB not consistent with fw\n");
#endif

	return test_ret;
}

static int arbitration_test(void *data)
{
	struct pp_qos_dev *qdev;
	struct pp_qos_port_conf confp;
	struct pp_qos_sched_conf confs;
	struct pp_qos_queue_conf confq;
	struct qos_node *q_node, *s_node;
	u32 wsp_port_id = PP_QOS_INVALID_ID;
	u32 wrr_port_id = PP_QOS_INVALID_ID;
	u32 q[4];
	u32 s[4];
	u32 phy;
	u32 test_ret = PP_UT_FAIL;
	s32 ret;

	qdev = qos_restart(PP_QOS_INSTANCE_ID);
	if (!qdev)
		return PP_UT_FAIL;

	/* Add WSP port */
	PPUT_FAIL_TEST(pp_qos_port_allocate(qdev, 0, &wsp_port_id) == 0,
		       "Could not allocate port\n");
	pp_qos_port_conf_set_default(&confp);
	confp.port_parent_prop.arbitration = PP_QOS_ARBITRATION_WSP;
	confp.ring_address = ring_addr_phys[0];
	confp.ring_size = 1;
	PPUT_FAIL_TEST(pp_qos_port_set(qdev, wsp_port_id, &confp) == 0,
		       "Could not set port\n");
	/* Add WRR port */
	PPUT_FAIL_TEST(pp_qos_port_allocate(qdev, 1, &wrr_port_id) == 0,
		       "Could not allocate port\n");
	pp_qos_port_conf_set_default(&confp);
	confp.port_parent_prop.arbitration = PP_QOS_ARBITRATION_WRR;
	confp.ring_address = ring_addr_phys[1];
	confp.ring_size = 1;
	PPUT_FAIL_TEST(pp_qos_port_set(qdev, wrr_port_id, &confp) == 0,
		       "Could not set port\n");

	/* Test adding queues/scheds to WSP */
	pp_qos_queue_conf_set_default(&confq);
	pp_qos_sched_conf_set_default(&confs);
	confq.queue_child_prop.priority = 0;
	confs.sched_child_prop.priority = 1;
	confq.queue_child_prop.wrr_weight = 10;
	confs.sched_child_prop.wrr_weight = 10;
	confq.queue_child_prop.parent = wsp_port_id;
	confs.sched_child_prop.parent = wsp_port_id;
	PPUT_FAIL_TEST(pp_qos_queue_allocate(qdev, &q[0]) == 0,
		       "Could not allocate queue\n");
	PPUT_FAIL_TEST(pp_qos_sched_allocate(qdev, &s[0]) == 0,
		       "Could not allocate sched\n");
	/* Add queue/sched to WSP parent with weight */
	PPUT_FAIL_TEST(pp_qos_queue_set(qdev, q[0], &confq) == 0,
		       "Could not set queue\n");
	PPUT_FAIL_TEST(pp_qos_sched_set(qdev, s[0], &confs) == 0,
		       "Could not set sched\n");
#ifdef FW_CHECK_SYNC
	if (check_sync_with_fw(qdev, true)) {
		pr_err("DB not consistent with fw\n");
		test_ret = PP_UT_FAIL;
		goto done;
	}
#endif
	confq.queue_child_prop.priority = 2;
	confs.sched_child_prop.priority = 3;
	confq.queue_child_prop.wrr_weight = 1;
	confs.sched_child_prop.wrr_weight = 1;
	PPUT_FAIL_TEST(pp_qos_queue_allocate(qdev, &q[1]) == 0,
		       "Could not allocate queue\n");
	PPUT_FAIL_TEST(pp_qos_sched_allocate(qdev, &s[1]) == 0,
		       "Could not allocate sched\n");
	/* Add queue/sched to WSP parent without weight */
	PPUT_FAIL_TEST(pp_qos_queue_set(qdev, q[1], &confq) == 0,
		       "Could not set queue\n");
	PPUT_FAIL_TEST(pp_qos_sched_set(qdev, s[1], &confs) == 0,
		       "Could not set sched\n");
#ifdef FW_CHECK_SYNC
	if (check_sync_with_fw(qdev, true)) {
		pr_err("DB not consistent with fw\n");
		test_ret = PP_UT_FAIL;
		goto done;
	}
#endif
	/* Set queue/sched weight */
	confq.queue_child_prop.wrr_weight = 5;
	confs.sched_child_prop.wrr_weight = 10;
	PPUT_FAIL_TEST(pp_qos_queue_set(qdev, q[1], &confq) == 0,
		       "Could not set queue\n");
	PPUT_FAIL_TEST(pp_qos_sched_set(qdev, s[1], &confs) == 0,
		       "Could not set sched\n");
#ifdef FW_CHECK_SYNC
	if (check_sync_with_fw(qdev, true)) {
		pr_err("DB not consistent with fw\n");
		test_ret = PP_UT_FAIL;
		goto done;
	}
#endif

	/* Test adding queues/scheds to WRR */
	pp_qos_queue_conf_set_default(&confq);
	pp_qos_sched_conf_set_default(&confs);
	confq.queue_child_prop.priority = 0;
	confs.sched_child_prop.priority = 1;
	confq.queue_child_prop.wrr_weight = 10;
	confs.sched_child_prop.wrr_weight = 10;
	confq.queue_child_prop.parent = wrr_port_id;
	confs.sched_child_prop.parent = wrr_port_id;
	PPUT_FAIL_TEST(pp_qos_queue_allocate(qdev, &q[2]) == 0,
		       "Could not allocate queue\n");
	PPUT_FAIL_TEST(pp_qos_sched_allocate(qdev, &s[2]) == 0,
		       "Could not allocate sched\n");
	/* Add queue to WRR parent with weight */
	PPUT_FAIL_TEST(pp_qos_queue_set(qdev, q[2], &confq) == 0,
		       "Could not set queue\n");
	PPUT_FAIL_TEST(pp_qos_sched_set(qdev, s[2], &confs) == 0,
		       "Could not set sched\n");
#ifdef FW_CHECK_SYNC
	if (check_sync_with_fw(qdev, true)) {
		pr_err("DB not consistent with fw\n");
		test_ret = PP_UT_FAIL;
		goto done;
	}
#endif
	confq.queue_child_prop.priority = 2;
	confs.sched_child_prop.priority = 3;
	confq.queue_child_prop.wrr_weight = 1;
	confs.sched_child_prop.wrr_weight = 1;
	PPUT_FAIL_TEST(pp_qos_queue_allocate(qdev, &q[3]) == 0,
		       "Could not allocate queue\n");
	PPUT_FAIL_TEST(pp_qos_sched_allocate(qdev, &s[3]) == 0,
		       "Could not allocate sched\n");
	/* Add queue to WRR parent without weight */
	PPUT_FAIL_TEST(pp_qos_queue_set(qdev, q[3], &confq) == 0,
		       "Could not set queue\n");
	PPUT_FAIL_TEST(pp_qos_sched_set(qdev, s[3], &confs) == 0,
		       "Could not set sched\n");
#ifdef FW_CHECK_SYNC
	if (check_sync_with_fw(qdev, true)) {
		pr_err("DB not consistent with fw\n");
		test_ret = PP_UT_FAIL;
		goto done;
	}
#endif
	/* Set queue/sched weight */
	confq.queue_child_prop.wrr_weight = 5;
	confs.sched_child_prop.wrr_weight = 10;
	PPUT_FAIL_TEST(pp_qos_queue_set(qdev, q[3], &confq) == 0,
		       "Could not set queue\n");
	PPUT_FAIL_TEST(pp_qos_sched_set(qdev, s[3], &confs) == 0,
		       "Could not set sched\n");

	phy = get_phy_from_id(qdev->mapping, q[3]);
	q_node = get_node_from_phy(qdev->nodes, phy);
	PPUT_FAIL_TEST(q_node->child_prop.bw_weight == 1,
		       "Queue weight should be 1\n");

	phy = get_phy_from_id(qdev->mapping, s[3]);
	s_node = get_node_from_phy(qdev->nodes, phy);
	PPUT_FAIL_TEST(s_node->child_prop.bw_weight == 2,
		       "Sched weight should be 2\n");

	/* Test changing Arbitration to a an empty Scheduler */
	confs.sched_parent_prop.arbitration = PP_QOS_ARBITRATION_WRR;
	PPUT_FAIL_TEST(pp_qos_sched_set(qdev, s[3], &confs) == 0,
		       "Could not set sched\n");

	confs.sched_parent_prop.arbitration = PP_QOS_ARBITRATION_WSP;
	PPUT_FAIL_TEST(pp_qos_sched_set(qdev, s[3], &confs) == 0,
		       "Could not set sched\n");

#ifdef FW_CHECK_SYNC
	if (check_sync_with_fw(qdev, true)) {
		pr_err("DB not consistent with fw\n");
		test_ret = PP_UT_FAIL;
		goto done;
	}
#endif

	/* Move queue/sched WSP => WRR */
	confq.queue_child_prop.parent = wrr_port_id;
	confs.sched_child_prop.parent = wrr_port_id;
	confq.queue_child_prop.priority = 5;
	confs.sched_child_prop.priority = 6;

	pp_qos_queue_block(qdev, q[0]);
	PPUT_FAIL_TEST(pp_qos_queue_set(qdev, q[0], &confq) == 0,
		       "Could not set queue\n");
	pp_qos_queue_unblock(qdev, q[0]);

	PPUT_FAIL_TEST(pp_qos_sched_set(qdev, s[0], &confs) == 0,
		       "Could not set sched\n");
#ifdef FW_CHECK_SYNC
	if (check_sync_with_fw(qdev, true)) {
		pr_err("DB not consistent with fw\n");
		test_ret = PP_UT_FAIL;
		goto done;
	}
#endif

	/* Move queue/sched WRR => WSP */
	confq.queue_child_prop.parent = wsp_port_id;
	confs.sched_child_prop.parent = wsp_port_id;
	confq.queue_child_prop.priority = 5;
	confs.sched_child_prop.priority = 6;

	pp_qos_queue_block(qdev, q[0]);
	PPUT_FAIL_TEST(pp_qos_queue_set(qdev, q[0], &confq) == 0,
		       "Could not set queue\n");
	pp_qos_queue_unblock(qdev, q[0]);

	PPUT_FAIL_TEST(pp_qos_sched_set(qdev, s[0], &confs) == 0,
		       "Could not set sched\n");
#ifdef FW_CHECK_SYNC
	if (check_sync_with_fw(qdev, true)) {
		pr_err("DB not consistent with fw\n");
		test_ret = PP_UT_FAIL;
		goto done;
	}
#endif

	test_ret = PP_UT_PASS;
	DUMP_QOS_TREE(qdev);

done:
	PPUT_FAIL_TEST(pp_qos_port_remove(qdev, wsp_port_id) == 0,
		       "Could not remove port\n");

	PPUT_FAIL_TEST(pp_qos_port_remove(qdev, wrr_port_id) == 0,
		       "Could not remove port\n");

	pr_debug("\n");
	ret = check_consistency_with_resources(qdev);
	if (ret) {
		pr_err("check_consistency_with_resources failed\n");
		return PP_UT_FAIL;
	}

#ifdef FW_CHECK_SYNC
	if (check_sync_with_fw(qdev, true)) {
		pr_err("DB not consistent with fw\n");
		test_ret = PP_UT_FAIL;
	}
#endif

	return test_ret;
}

#define EWSP_PORT_NUM   3
#define EWSP_QUEUES_NUM 8
static int enhanced_wsp_test(void *data)
{
	struct pp_qos_dev *qdev;
	struct pp_qos_port_conf confp;
	struct pp_qos_queue_conf confq;
	u32 port, queue, ports[EWSP_PORT_NUM],
		queues[EWSP_PORT_NUM][EWSP_QUEUES_NUM];
	u32 test_ret = PP_UT_FAIL;

	qdev = qos_restart(PP_QOS_INSTANCE_ID);
	if (!qdev)
		return PP_UT_FAIL;

	if (!qdev->init_params.enhanced_wsp)
		return PP_UT_PASS;

	pp_qos_port_conf_set_default(&confp);
	confp.port_parent_prop.arbitration = PP_QOS_ARBITRATION_WSP;
	confp.ring_size = 1;
	confp.enhanced_wsp = 1;

	pp_qos_queue_conf_set_default(&confq);
	confq.wred_max_allowed = 0x400;

	/* add 3 ports, each has 8 queues */
	for (port = 0; port < ARRAY_SIZE(ports); port++) {
		pp_qos_port_allocate(qdev, ALLOC_PORT_ID, &ports[port]);
		confp.ring_address = ring_addr_phys[port];
		pp_qos_port_set(qdev, ports[port], &confp);

		confq.queue_child_prop.parent = ports[port];
		for (queue = 0; queue < EWSP_QUEUES_NUM; queue++) {
			pp_qos_queue_allocate(qdev, &queues[port][queue]);
			confq.queue_child_prop.priority = queue;
			pp_qos_queue_set(qdev, queues[port][queue], &confq);
		}
	}

	/* Make sure only maximum ewsp queues are allocated */
	PPUT_FAIL_RETURN(
		qdev->ewsp_cfg.num_queues == QOS_ENHANCED_WSP_MAX_QUEUES,
		"expected %u ewsp queues, found %u\n",
		QOS_ENHANCED_WSP_MAX_QUEUES, qdev->ewsp_cfg.num_queues);

	/* Make sure only first 2 ports are assigned with 6 ewsp queues each */
	for (port = 0; port < ARRAY_SIZE(ports); port++) {
		for (queue = 0; queue < EWSP_QUEUES_NUM; queue++) {
			if ((port == 0 && (queue >= 0 && queue <= 5)) ||
			    (port == 1 && (queue >= 0 && queue <= 5))) {
				PPUT_FAIL_RETURN(is_q_ewsp(qdev,
							   queues[port][queue]),
						 "P%u.Q%u should be ewsp\n",
						 port, queue);
			} else {
				PPUT_FAIL_RETURN(
					!is_q_ewsp(qdev, queues[port][queue]),
					"P%u.Q%u should not be ewsp\n", port,
					queue);
			}
		}
	}

	/* Remove port0 queue0, verify port2 is assigned with 1 ewsp queue */
	pp_qos_queue_remove(qdev, queues[0][0]);
	for (port = 0; port < ARRAY_SIZE(ports); port++) {
		for (queue = 0; queue < EWSP_QUEUES_NUM; queue++) {
			if (port == 0 && queue == 0)
				continue;
			if ((port == 0 && (queue >= 1 && queue <= 5)) ||
			    (port == 1 && (queue >= 0 && queue <= 5)) ||
			    (port == 2 && queue == 0)) {
				PPUT_FAIL_RETURN(is_q_ewsp(qdev,
							   queues[port][queue]),
						 "P%u.Q%u should be ewsp\n",
						 port, queue);
			} else {
				PPUT_FAIL_RETURN(
					!is_q_ewsp(qdev, queues[port][queue]),
					"P%u.Q%u should not be ewsp\n", port,
					queue);
			}
		}
	}

	/* Remove port0 queue7, verify port2 is assigned with 2 ewsp queue */
	pp_qos_queue_remove(qdev, queues[0][7]);
	port = 2;
	for (queue = 0; queue < EWSP_QUEUES_NUM; queue++) {
		if (queue >= 0 && queue <= 1) {
			PPUT_FAIL_RETURN(is_q_ewsp(qdev, queues[port][queue]),
					 "P%u.Q%u should be ewsp\n", port,
					 queue);
		} else {
			PPUT_FAIL_RETURN(!is_q_ewsp(qdev, queues[port][queue]),
					 "P%u.Q%u should not be ewsp\n", port,
					 queue);
		}
	}

	/* Move port2 queue7 to port1 */
	pp_qos_queue_conf_get(qdev, queues[2][7], &confq);
	confq.blocked = 1;
	confq.queue_child_prop.parent = ports[0];
	queues[0][7] = queues[2][7];
	pp_qos_queue_set(qdev, queues[0][7], &confq);
	for (port = 0; port < ARRAY_SIZE(ports); port++) {
		for (queue = 0; queue < EWSP_QUEUES_NUM; queue++) {
			if (port == 0 && queue == 0)
				continue;
			if ((port == 0 && (queue >= 1 && queue <= 5)) ||
			    (port == 1 && (queue >= 0 && queue <= 5)) ||
			    (port == 2 && queue == 0)) {
				PPUT_FAIL_RETURN(is_q_ewsp(qdev,
							   queues[port][queue]),
						 "P%u.Q%u should be ewsp\n",
						 port, queue);
			} else {
				PPUT_FAIL_RETURN(
					!is_q_ewsp(qdev, queues[port][queue]),
					"P%u.Q%u should not be ewsp\n", port,
					queue);
			}
		}
	}
	test_ret = PP_UT_PASS;

	DUMP_QOS_TREE(qdev);
	for (port = 0; port < 3; port++)
		pp_qos_port_remove(qdev, ports[port]);

#ifdef FW_CHECK_SYNC
	if (check_sync_with_fw(qdev, true)) {
		pr_err("DB not consistent with fw\n");
		test_ret = PP_UT_FAIL;
	}
#endif

	return test_ret;
}

static int bw_reconfiguration_test(void *data)
{
	struct pp_qos_dev *qdev;
	struct pp_qos_port_conf confp;
	struct pp_qos_sched_conf confs;
	struct pp_qos_queue_conf confq;
	u32 p_idx, *port, *sched, *q;
	u32 qidx, qprio;
	u32 reconfiguration_idx = 1;
	u32 bw_limit = 0;
	u32 test_ret = PP_UT_FAIL;

	qdev = qos_restart(PP_QOS_INSTANCE_ID);
	if (!qdev)
		return PP_UT_FAIL;

	q = kzalloc(sizeof(u32) * 256, GFP_KERNEL);
	if (!q)
		return PP_UT_FAIL;

	port = kzalloc(sizeof(u32) * 64, GFP_KERNEL);
	if (!q)
		return PP_UT_FAIL;

	sched = kzalloc(sizeof(u32) * 64, GFP_KERNEL);
	if (!q)
		return PP_UT_FAIL;

	/* Add WSP ports */
	pp_qos_port_conf_set_default(&confp);
	pp_qos_sched_conf_set_default(&confs);
	pp_qos_queue_conf_set_default(&confq);
	confp.port_parent_prop.arbitration = PP_QOS_ARBITRATION_WSP;
	confp.ring_address = ring_addr_phys[0];
	confp.ring_size = 1;
	for (p_idx = 0; p_idx < 64; p_idx++) {
		/* Allocate ports 25 - 88 */
		PPUT_FAIL_TEST(pp_qos_port_allocate(qdev,
						    25 + p_idx,
						    &port[p_idx])
			       == 0, "Could not allocate port\n");
		confp.common_prop.bandwidth_limit = bw_limit;
		bw_limit += 200;
		PPUT_FAIL_TEST(pp_qos_port_set(qdev, port[p_idx], &confp) == 0,
			       "Could not set port\n");
	}

sched_queue_config:
	QOS_LOG_INFO("Iteration %u out of 3\n", reconfiguration_idx);
	for (p_idx = 0; p_idx < 64; p_idx++) {
		PPUT_FAIL_TEST(pp_qos_sched_allocate(qdev, &sched[p_idx]) == 0,
			       "Could not allocate sched\n");
		confs.sched_child_prop.parent = port[p_idx];
		confs.common_prop.bandwidth_limit = bw_limit;
		bw_limit += 200;
		PPUT_FAIL_TEST(pp_qos_sched_set(qdev, sched[p_idx], &confs)
			       == 0, "Could not set sched\n");

		/* For each sched create 4 queues */
		qprio = 0;
		confq.queue_child_prop.parent = sched[p_idx];
		for (qidx = (p_idx * 4); qidx < ((p_idx + 1) * 4); qidx++) {
			PPUT_FAIL_TEST(pp_qos_queue_allocate(qdev, &q[qidx])
				       == 0, "Could not allocate queue\n");
			confq.queue_child_prop.priority = qprio++;
			confq.common_prop.bandwidth_limit = bw_limit;
			bw_limit += 200;
			PPUT_FAIL_TEST(pp_qos_queue_set(qdev, q[qidx], &confq)
				       == 0, "Could not set queue\n");
		}
	}

#ifdef FW_CHECK_SYNC
	if (check_sync_with_fw(qdev, false)) {
		QOS_LOG_ERR("Sync failed after config idx %u\n",
			    reconfiguration_idx);
		test_ret = PP_UT_FAIL;
		goto done;
	}
#endif

	/* Remove all scheds and queues */
	for (p_idx = 0; p_idx < 64; p_idx++) {
		PPUT_FAIL_TEST(pp_qos_sched_remove(qdev, sched[p_idx]) == 0,
			       "Could not remove sched\n");
	}

#ifdef FW_CHECK_SYNC
	if (check_sync_with_fw(qdev, false)) {
		QOS_LOG_ERR("Sync failed after config idx %u removal\n",
			    reconfiguration_idx);
		test_ret = PP_UT_FAIL;
	}
#endif

	if (reconfiguration_idx < 3) {
		reconfiguration_idx++;
		goto sched_queue_config;
	}

	for (p_idx = 0; p_idx < 64; p_idx++) {
		PPUT_FAIL_TEST(pp_qos_port_remove(qdev, port[p_idx])
			       == 0, "Could not allocate port\n");
	}

	test_ret = PP_UT_PASS;

done:
	DUMP_QOS_TREE(qdev);
	kfree(q);
	kfree(port);
	kfree(sched);

	return test_ret;
}

static int wa_min_test(void *data)
{
	struct pp_qos_dev *qdev;
	unsigned int p7 = PP_QOS_INVALID_ID;
	unsigned int p25 = PP_QOS_INVALID_ID;
	unsigned int q0 = PP_QOS_INVALID_ID;
	unsigned int q2 = PP_QOS_INVALID_ID;
	struct pp_qos_port_conf confp;
	struct pp_qos_queue_conf confq;
	u32 test_ret = PP_UT_FAIL;

	qdev = qos_restart(PP_QOS_INSTANCE_ID);
	if (!qdev)
		return PP_UT_FAIL;

	pp_qos_port_conf_set_default(&confp);
	confp.port_parent_prop.arbitration = PP_QOS_ARBITRATION_WSP;
	confp.disable = 1;
	confp.ring_size = 1;

	/* add port 25 */
	PPUT_FAIL_TEST(pp_qos_port_allocate(qdev, 25, &p25) == 0,
		       "Could not allocate port 25\n");
	confp.disable = 1;
	confp.packet_credit_enable = 1;
	confp.credit = 2;
	confp.ring_address = ring_addr_phys[0];
	PPUT_FAIL_TEST(pp_qos_port_set(qdev, p25, &confp) == 0,
		       "Could not set port 0\n");

	pp_qos_queue_conf_set_default(&confq);
	confq.wred_max_allowed = 0x400;
	confq.blocked = 0;
	PPUT_FAIL_TEST(pp_qos_queue_allocate(qdev, &q0) == 0,
		       "Could not allocate queue\n");
	confq.queue_child_prop.parent = p25;
	PPUT_FAIL_TEST(pp_qos_queue_set(qdev, q0, &confq) == 0,
		       "Setting queue fail\n");

	confp.disable = 0;
	PPUT_FAIL_TEST(pp_qos_port_allocate(qdev, 7, &p7) == 0,
		       "Could not allocate port 0\n");
	confp.credit = 8;
	confp.ring_address = ring_addr_phys[1];
	PPUT_FAIL_TEST(pp_qos_port_set(qdev, p7, &confp) == 0,
		       "Could not set port 7\n");

	PPUT_FAIL_TEST(pp_qos_queue_allocate(qdev, &q2) == 0,
		       "Could not allocate queue\n");
	confq.queue_child_prop.parent = p25;
	confq.queue_child_prop.priority = 3;
	PPUT_FAIL_TEST(pp_qos_queue_set(qdev, q2, &confq) == 0,
		       "Setting queue fail\n");

	confq.queue_child_prop.parent = p7;
	confq.queue_child_prop.priority = 0;
	pp_qos_queue_block(qdev, q2);
	PPUT_FAIL_TEST(pp_qos_queue_set(qdev, q2, &confq) == 0,
		       "Setting queue fail\n");
	pp_qos_queue_unblock(qdev, q2);

	test_ret = PP_UT_PASS;
	DUMP_QOS_TREE(qdev);
done:
	PPUT_FAIL_RETURN(pp_qos_port_remove(qdev, p7) == 0,
			 "Could not remove port %d\n", p7);

	PPUT_FAIL_RETURN(pp_qos_port_remove(qdev, p25) == 0,
			 "Could not remove port %d\n", p25);

#ifdef FW_CHECK_SYNC
	if (check_sync_with_fw(qdev, true)) {
		pr_err("DB not consistent with fw\n");
		test_ret = PP_UT_FAIL;
	}
#endif

	return test_ret;
}

void dump_queue_stats(struct pp_qos_queue_stat *qstat)
{
	pr_info("reset %d\npackets_occupancy %d\nbytes_occupancy %d\n",
		qstat->reset,
		qstat->queue_packets_occupancy,
		qstat->queue_bytes_occupancy);

	pr_info("packet_accepted %d\npacket_dropped %d\nred_dropped %d\n",
		qstat->total_packets_accepted,
		qstat->total_packets_dropped,
		qstat->total_packets_red_dropped);

	pr_info("bytes_accepted %llu\nbytes_dropped %llu\n",
		qstat->total_bytes_accepted,
		qstat->total_bytes_dropped);
}

void dump_port_stats(struct pp_qos_port_stat *pstat)
{
	pr_info("reset %d\ntotal_green_bytes %d\ntotal_yellow_bytes %d\n",
		pstat->reset,
		pstat->total_green_bytes,
		pstat->total_yellow_bytes);
}

void dump_node_info(struct pp_qos_node_info *info)
{
	pr_info("type %d\n", info->type);
	pr_info("children phys[0 - 7]: %d %d %d %d %d %d %d %d\n",
		info->children[0].phy, info->children[1].phy,
		info->children[2].phy, info->children[3].phy,
		info->children[4].phy, info->children[5].phy,
		info->children[6].phy, info->children[7].phy);

	pr_info("children id's[0 - 7]: %d %d %d %d %d %d %d %d\n",
		info->children[0].id, info->children[1].id,
		info->children[2].id, info->children[3].id,
		info->children[4].id, info->children[5].id,
		info->children[6].id, info->children[7].id);

	pr_info("internal %d. bw_limit %d. port %d. queue_phys %d\n",
		info->is_internal, info->bw_limit,
		info->port, info->queue_physical_id);

	pr_info("preds phy's[0 - 5]: %d %d %d %d %d %d\n",
		info->preds[0].phy, info->preds[1].phy,
		info->preds[2].phy, info->preds[3].phy,
		info->preds[4].phy, info->preds[5].phy);

	pr_info("preds id's[0 - 5]: %d %d %d %d %d %d\n",
		info->preds[0].id, info->preds[1].id,
		info->preds[2].id, info->preds[3].id,
		info->preds[4].id, info->preds[5].id);
}

static int wsp_test(void *data)
{
	struct pp_qos_queue_conf confq;
	struct pp_qos_port_conf confp;
	struct pp_qos_sched_conf confs;
	struct qos_node *pnode;
	struct qos_node *cnode;
	struct pp_qos_dev *qdev;
	unsigned int p1 = PP_QOS_INVALID_ID;
	unsigned int q[100] = {PP_QOS_INVALID_ID};
	unsigned int s[8] = {PP_QOS_INVALID_ID};
	int i;
	int j;
	struct pp_qos_node_info info;
	s32 ret;
	u32 qid;
	u32 test_ret = PP_UT_FAIL;
	u32 phy;

	qdev = qos_restart(PP_QOS_INSTANCE_ID);
	if (!qdev)
		return PP_UT_FAIL;

	/* Allocate port 1 */
	pp_qos_port_conf_set_default(&confp);
	confp.port_parent_prop.arbitration = PP_QOS_ARBITRATION_WSP;
	confp.ring_address = ring_addr_phys[0];
	confp.ring_size = 1;
	pp_qos_port_allocate(qdev, 1, &p1);
	PPUT_FAIL_TEST(pp_qos_port_set(qdev, p1, &confp) == 0,
		       "Could not set port\n");
	ret = check_consistency_with_resources(qdev);
	if (ret)
		goto done;

	PPUT_FAIL_TEST(qos_get_node_info(qdev, p1, &info) == 0,
		       "Could not get node info\n");

	/* Allocate 8 queues */
	pp_qos_queue_conf_set_default(&confq);
	confq.wred_max_allowed = 400;
	confq.queue_child_prop.parent = p1;
	for (i = 0; i < 8; ++i) {
		pp_qos_queue_allocate(qdev, &q[i]);
		confq.queue_child_prop.priority = i;
		QOS_LOG_DEBUG("Allocated queue with id %d\n", q[i]);
		PPUT_FAIL_TEST(pp_qos_queue_set(qdev, q[i], &confq) == 0,
			       "queue set failed\n");
	}

	confq.queue_child_prop.priority = 0;

	/* Trying to add 9'th queue should fail */
	pp_qos_queue_allocate(qdev, &q[8]);
	QOS_LOG_DEBUG("Allocated queue with id %d\n", q[8]);
	ret = pp_qos_queue_set(qdev, q[8], &confq);
	QOS_LOG_ERR("queue set ret %d\n", ret);
	PPUT_FAIL_TEST(ret != 0,
		       "queue set should have failed\n");

	pp_qos_queue_block(qdev, q[8]);
	qos_queue_flush(qdev, q[8]);
	PPUT_FAIL_TEST(pp_qos_queue_remove(qdev, q[8]) == 0,
		       "Could not remove queue %d\n", q[8]);

	pnode = get_node_from_phy(qdev->nodes,
				  get_phy_from_id(qdev->mapping, p1));
	PPUT_FAIL_TEST(pnode, "get_node_from_phy(%u) returned NULL\n",
		       get_phy_from_id(qdev->mapping, p1));
	PPUT_FAIL_TEST(pnode->parent_prop.num_of_children == 8,
		       "Port %d has %d children instead of 8\n",
		       p1, pnode->parent_prop.num_of_children);

	pp_qos_queue_block(qdev, q[7]);
	qos_queue_flush(qdev, q[7]);
	PPUT_FAIL_TEST(pp_qos_queue_remove(qdev, q[7]) == 0,
		       "Could not remove queue %d\n", q[7]);

	/* Switch queues priorities */
	for (i = 6; i >= 0; --i) {
		confq.queue_child_prop.priority = i + 1;
		PPUT_FAIL_TEST(pp_qos_queue_set(qdev, q[i], &confq) == 0,
			       "queue set failed\n");
		ret = check_consistency_with_resources(qdev);
		if (ret)
			goto done;
	}

	for (i = 0; i < 7; i++) {
		cnode = get_node_from_phy(qdev->nodes,
					  pnode->parent_prop.first_child_phy +
					  i);
		PPUT_FAIL_TEST(cnode, "get_node_from_phy(%u) returned NULL\n",
			       pnode->parent_prop.first_child_phy + i);
		QOS_LOG_ERR("child %d: prio %u\n", i,
			    cnode->child_prop.priority);
	}

	for (i = 0; i < 7; i++) {
		cnode = get_node_from_phy(qdev->nodes,
					  pnode->parent_prop.first_child_phy
					  + i);
		PPUT_FAIL_TEST(cnode, "get_node_from_phy(%u) returned NULL\n",
			       pnode->parent_prop.first_child_phy + i);
		PPUT_FAIL_TEST((cnode->child_prop.priority == i + 1),
			       "Child %d priority is %d and not %d\n",
			       i, cnode->child_prop.priority, i + 1);

		phy = pnode->parent_prop.first_child_phy + i;
		if (i == 6)
			phy += 2; /* Last node is reserved for the prio7 */
		qid = get_id_from_phy(qdev->mapping, phy);

		PPUT_FAIL_TEST(qid == q[i], "queue id is %d instead of %d\n",
			       qid, q[i]);
	}

	for (i = 0; i < 7; ++i) {
		QOS_LOG_DEBUG("Remove queue with id %d\n", q[i]);
		pp_qos_queue_block(qdev, q[i]);
		qos_queue_flush(qdev, q[i]);
		PPUT_FAIL_TEST(pp_qos_queue_remove(qdev, q[i]) == 0,
			       "queue remove failed");
		ret = check_consistency_with_resources(qdev);
		if (ret)
			goto done;
	}

	/* Doing the same with schedulers instead of queues */
	pp_qos_sched_conf_set_default(&confs);
	confs.sched_parent_prop.arbitration = PP_QOS_ARBITRATION_WRR;
	confs.sched_child_prop.parent = p1;
	for (i = 0; i < 8; ++i) {
		confs.sched_child_prop.priority = i;
		PPUT_FAIL_TEST(pp_qos_sched_allocate(qdev, &s[i]) == 0,
			       "Could not allocate sched\n");
		QOS_LOG_DEBUG("Allocated sched with id %d\n", s[i]);
		PPUT_FAIL_TEST(pp_qos_sched_set(qdev, s[i], &confs) == 0,
			       "Could not set sched\n");
		confq.queue_child_prop.parent = s[i];

		for (j = 0; j < 8; ++j) {
			pp_qos_queue_allocate(qdev, &q[i * 8 + j]);
			QOS_LOG_DEBUG("Allocated queue with id %d\n",
				      q[i * 8 + j]);
			PPUT_FAIL_TEST(pp_qos_queue_set(qdev, q[i * 8 + j],
							&confq) == 0,
				       "queue set failed\n");
		}
	}

	PPUT_FAIL_RETURN(pp_qos_sched_remove(qdev, s[7]) == 0,
			 "Could not remove sched %d\n", s[7]);

	for (i = 6; i >= 0; --i) {
		confs.sched_child_prop.priority = i + 1;
		PPUT_FAIL_TEST(pp_qos_sched_set(qdev, s[i], &confs) == 0,
			       "sched set failed\n");
		ret = check_consistency_with_resources(qdev);
		if (ret)
			goto done;
	}

	for (i = 0; i < 7; ++i) {
		pp_qos_sched_conf_get(qdev, s[i], &confs);
		PPUT_FAIL_TEST((confs.sched_child_prop.priority == i + 1),
			       "Priority of sched %u is %u\n",
			       i, confs.sched_child_prop.priority);
	}

	ret = check_consistency_with_resources(qdev);
	if (ret)
		goto done;

#ifndef PP_QOS_DISABLE_CMDQ
#ifdef FW_CHECK_SYNC
	PPUT_FAIL_TEST(check_sync_with_fw(qdev, false) == 0,
		       "DB not consistent with fw\n");
#endif
#endif
	test_ret = PP_UT_PASS;
	DUMP_QOS_TREE(qdev);
done:
	for (i = 6 ; i >= 0 ; --i) {
		for (j = 7; j >= 0 ; --j) {
			pp_qos_queue_block(qdev, q[i * 8 + j]);
			qos_queue_flush(qdev, q[i * 8 + j]);
			PPUT_FAIL_RETURN(
				pp_qos_queue_remove(qdev, q[i * 8 + j]) == 0,
				"Could not remove queue %d\n", q[i * 8 + j]);
		}

		PPUT_FAIL_RETURN(pp_qos_sched_remove(qdev, s[i]) == 0,
				 "Could not remove sched %d\n", s[i]);
	}

	PPUT_FAIL_RETURN(pp_qos_port_remove(qdev, p1) == 0,
			 "Could not remove queue %d\n", p1);

	ret = check_consistency_with_resources(qdev);
	if (ret)
		test_ret = PP_UT_FAIL;

#ifndef PP_QOS_DISABLE_CMDQ
#ifdef FW_CHECK_SYNC
	if (check_sync_with_fw(qdev, true)) {
		pr_err("DB not consistent with fw\n");
		test_ret = PP_UT_FAIL;
	}
#endif
#endif

	return test_ret;
}

#ifndef PP_QOS_DISABLE_CMDQ

static int stat_test(void *data)
{
	struct pp_qos_dev *qdev;
	unsigned int p = PP_QOS_INVALID_ID;
	unsigned int q1 = PP_QOS_INVALID_ID;
	unsigned int q2 = PP_QOS_INVALID_ID;
	struct pp_qos_port_conf confp;
	struct pp_qos_queue_conf confq;
	struct pp_qos_queue_stat qstat;
	struct pp_qos_port_stat pstat;
	s32 ret;
	u32 test_ret = PP_UT_FAIL;

	qdev = qos_restart(PP_QOS_INSTANCE_ID);
	if (!qdev)
		return PP_UT_FAIL;

	PPUT_FAIL_TEST(pp_qos_port_allocate(qdev, 1, &p) == 0,
		       "Could not allocate port\n");
	pp_qos_port_conf_set_default(&confp);

	confp.port_parent_prop.arbitration = PP_QOS_ARBITRATION_WRR;
	confp.ring_size = 1;
	confp.disable = 1;
	confp.ring_address = ring_addr_phys[0];

	PPUT_FAIL_TEST(pp_qos_port_set(qdev, p, &confp) == 0,
		       "Could not set port\n");

	pp_qos_queue_conf_set_default(&confq);
	confq.wred_max_allowed = 1000;
	PPUT_FAIL_TEST(pp_qos_queue_allocate(qdev, &q1) == 0,
		       "Could not allocate queue\n");

	PPUT_FAIL_TEST(pp_qos_queue_allocate(qdev, &q2) == 0,
		       "Could not allocate queue\n");

	confq.queue_child_prop.parent = p;
	PPUT_FAIL_TEST(pp_qos_queue_set(qdev, q1, &confq) == 0,
		       "Could not set queue\n");

	PPUT_FAIL_TEST(pp_qos_queue_set(qdev, q2, &confq) == 0,
		       "Could not set queue\n");

	memset(&pstat, 0, sizeof(pstat));
	PPUT_FAIL_TEST(pp_qos_port_stat_get(qdev, p, &pstat) == 0,
		       "Could not get port statistics\n");

	memset(&qstat, 0, sizeof(qstat));
	PPUT_FAIL_TEST(pp_qos_queue_stat_get(qdev, q1, &qstat) == 0,
		       "Could not get queue statistics\n");

	if (!(qstat.reset == 0 &&
	      qstat.queue_packets_occupancy == 0 &&
	      qstat.queue_bytes_occupancy == 0 &&
	      qstat.total_packets_accepted == 0 &&
	      qstat.total_packets_dropped == 0 &&
	      qstat.total_packets_red_dropped == 0 &&
	      qstat.total_bytes_accepted == 0 &&
	      qstat.total_bytes_dropped == 0)) {
		pr_err("Some field of q1 stats is not 0\n");
		goto done;
	}

	memset(&qstat, 0, sizeof(qstat));
	PPUT_FAIL_TEST(pp_qos_queue_stat_get(qdev, q2, &qstat) == 0,
		       "Could not get queue statistics\n");

	if (!(qstat.reset == 0 &&
	      qstat.queue_packets_occupancy == 0 &&
	      qstat.queue_bytes_occupancy == 0 &&
	      qstat.total_packets_accepted == 0 &&
	      qstat.total_packets_dropped == 0 &&
	      qstat.total_packets_red_dropped == 0 &&
	      qstat.total_bytes_accepted == 0 &&
	      qstat.total_bytes_dropped == 0)) {
		pr_err("Some field of q2 stats is not 0\n");
		goto done;
	}

	memset(&pstat, 0, sizeof(pstat));
	PPUT_FAIL_TEST(pp_qos_port_stat_get(qdev, p, &pstat) == 0,
		       "Could not get port statistics\n");

	if (!(pstat.reset == 0 &&
	      pstat.total_green_bytes == 0 &&
	      pstat.total_yellow_bytes == 0)) {
		pr_err("Some field of p stats is not 0\n");
		dump_port_stats(&pstat);
		goto done;
	}

	qos_descriptor_push(qdev, q1, 128, 0, 0, PP_COLOR_GREEN, 0, 0,
			    qdev->hwconf.fw_stats_ddr_phys);

	qos_descriptor_push(qdev, q1, 256, 0, 0, PP_COLOR_YELLOW, 0, 0,
			    qdev->hwconf.fw_stats_ddr_phys);

	qos_descriptor_push(qdev, q2, 100, 0, 0, PP_COLOR_GREEN, 0, 0,
			    qdev->hwconf.fw_stats_ddr_phys);

	qos_descriptor_push(qdev, q2, 200, 0, 0, PP_COLOR_YELLOW, 0, 0,
			    qdev->hwconf.fw_stats_ddr_phys);

	memset(&qstat, 0, sizeof(qstat));
	PPUT_FAIL_TEST(pp_qos_queue_stat_get(qdev, q1, &qstat) == 0,
		       "Could not get queue statistics\n");

	if (!(qstat.reset == 0 &&
	      qstat.queue_bytes_occupancy == 384 &&
	      qstat.total_bytes_accepted == 384 &&
	      qstat.total_bytes_dropped == 0)) {
		pr_err("Some fields of q1 stats are not expected\n");
		dump_queue_stats(&qstat);
		goto done;
	}

	if (!(qstat.queue_packets_occupancy == 2 &&
	      qstat.total_packets_accepted == 2 &&
	      qstat.total_packets_dropped == 0 &&
	      qstat.total_packets_red_dropped == 0)) {
		pr_err("Some fields of q1 stats are not expected\n");
		dump_queue_stats(&qstat);
		goto done;
	}

	memset(&qstat, 0, sizeof(qstat));
	qstat.reset = 1;
	PPUT_FAIL_TEST(pp_qos_queue_stat_get(qdev, q2, &qstat) == 0,
		       "Could not get queue statistics\n");

	if (!(qstat.reset == 1 &&
	      qstat.queue_bytes_occupancy == 300 &&
	      qstat.total_bytes_accepted == 300 &&
	      qstat.total_bytes_dropped == 0)) {
		pr_err("Some fields of q2 stats are not expected\n");
		dump_queue_stats(&qstat);
		goto done;
	}

	if (!(qstat.queue_packets_occupancy == 2 &&
	      qstat.total_packets_accepted == 2 &&
	      qstat.total_packets_dropped == 0 &&
	      qstat.total_packets_red_dropped == 0)) {
		pr_err("Some fields of q2 stats are not expected\n");
		dump_queue_stats(&qstat);
		goto done;
	}

	memset(&pstat, 0, sizeof(pstat));
	PPUT_FAIL_TEST(pp_qos_port_stat_get(qdev, p, &pstat) == 0,
		       "Could not get port statistics\n");

	if (!(pstat.reset == 0 &&
	      pstat.total_green_bytes == 228 &&
	      pstat.total_yellow_bytes == 456)) {
		pr_err("Some fields of p stats are not expected\n");
		dump_port_stats(&pstat);
		goto done;
	}

	qos_descriptor_push(qdev, q1, 100, 0, 0, PP_COLOR_GREEN, 0, 0,
			    qdev->hwconf.fw_stats_ddr_phys);

	qos_descriptor_push(qdev, q2, 300, 0, 0, PP_COLOR_GREEN, 0, 0,
			    qdev->hwconf.fw_stats_ddr_phys);

	memset(&qstat, 0, sizeof(qstat));
	PPUT_FAIL_TEST(pp_qos_queue_stat_get(qdev, q1, &qstat) == 0,
		       "Could not get queue statistics\n");

	if (!(qstat.reset == 0 &&
	      qstat.queue_bytes_occupancy == 484 &&
	      qstat.total_bytes_accepted == 484 &&
	      qstat.total_bytes_dropped == 0)) {
		pr_err("Some fields of q1 stats are not expected\n");
		dump_queue_stats(&qstat);
		goto done;
	}

	if (!(qstat.queue_packets_occupancy == 3 &&
	      qstat.total_packets_accepted == 3 &&
	      qstat.total_packets_dropped == 0 &&
	      qstat.total_packets_red_dropped == 0)) {
		pr_err("Some fields of q1 stats are not expected\n");
		dump_queue_stats(&qstat);
		goto done;
	}

	PPUT_FAIL_TEST(pp_qos_queue_stat_get(qdev, q2, &qstat) == 0,
		       "Could not get queue statistics\n");

	if (!(qstat.reset == 0 &&
	      qstat.queue_bytes_occupancy == 600 &&
	      qstat.total_bytes_accepted == 300 &&
	      qstat.total_bytes_dropped == 0)) {
		pr_err("Some fields of q2 stats are not expected\n");
		dump_queue_stats(&qstat);
		goto done;
	}

	if (!(qstat.queue_packets_occupancy == 3 &&
	      qstat.total_packets_accepted == 1 &&
	      qstat.total_packets_dropped == 0 &&
	      qstat.total_packets_red_dropped == 0)) {
		pr_err("Some fields of q2 stats are not expected\n");
		dump_queue_stats(&qstat);
		goto done;
	}

	memset(&pstat, 0, sizeof(pstat));
	pstat.reset = 1;
	PPUT_FAIL_TEST(pp_qos_port_stat_get(qdev, p, &pstat) == 0,
		       "Could not get port statistics\n");

	if (!(pstat.reset == 1 &&
	      pstat.total_green_bytes == 628 &&
	      pstat.total_yellow_bytes == 456)) {
		pr_err("Some fields of p stats are not expected\n");
		dump_port_stats(&pstat);
		goto done;
	}

	qos_descriptor_push(qdev, q1, 100, 0, 0, PP_COLOR_YELLOW, 0, 0,
			    qdev->hwconf.fw_stats_ddr_phys);
	memset(&pstat, 0, sizeof(pstat));

	PPUT_FAIL_TEST(pp_qos_port_stat_get(qdev, p, &pstat) == 0,
		       "Could not get port statistics\n");

	if (!(pstat.reset == 0 &&
	      pstat.total_green_bytes == 0 &&
	      pstat.total_yellow_bytes == 100)) {
		pr_err("Some fields of p stats are not expected\n");
		dump_port_stats(&pstat);
		goto done;
	}

	ret = check_consistency_with_resources(qdev);
	if (ret)
		goto done;

	test_ret = PP_UT_PASS;
	DUMP_QOS_TREE(qdev);
done:

	pp_qos_queue_block(qdev, q1);
	qos_queue_flush(qdev, q1);
	PPUT_FAIL_RETURN(pp_qos_queue_remove(qdev, q1) == 0,
			 "Could not remove queue %d\n", q1);

	pp_qos_queue_block(qdev, q2);
	qos_queue_flush(qdev, q2);
	PPUT_FAIL_RETURN(pp_qos_queue_remove(qdev, q2) == 0,
			 "Could not remove queue %d\n", q2);

	PPUT_FAIL_RETURN(pp_qos_port_remove(qdev, p) == 0,
			 "Could not remove queue %d\n", p);

#ifdef FW_CHECK_SYNC
	if (check_sync_with_fw(qdev, true)) {
		pr_err("DB not consistent with fw\n");
		test_ret = PP_UT_FAIL;
	}
#endif

	return test_ret;
}

static int info_test(void *data)
{
	struct pp_qos_queue_conf confq;
	struct pp_qos_port_conf confp;
	struct pp_qos_dev *qdev;
	unsigned int p1 = PP_QOS_INVALID_ID;
	unsigned int q[100] = {PP_QOS_INVALID_ID};
	unsigned int i;
	struct pp_qos_node_info info;
	s32 ret;
	u32 test_ret = PP_UT_FAIL;

	qdev = qos_restart(PP_QOS_INSTANCE_ID);
	if (!qdev)
		return PP_UT_FAIL;

	memset(&info, 0, sizeof(info));

	/* Allocate port 1  */
	pp_qos_port_conf_set_default(&confp);
	confp.port_parent_prop.arbitration = PP_QOS_ARBITRATION_WRR;
	confp.ring_address = ring_addr_phys[0];
	confp.ring_size = 1;
	pp_qos_port_allocate(qdev, 1, &p1);
	PPUT_FAIL_TEST(pp_qos_port_set(qdev, p1, &confp) == 0,
		       "Could not set port\n");
	ret = check_consistency_with_resources(qdev);
	if (ret)
		goto done;

	PPUT_FAIL_TEST(qos_get_node_info(qdev, p1, &info) == 0,
		       "Could not get node info\n");

	pp_qos_queue_conf_set_default(&confq);
	confq.queue_child_prop.parent = p1;

	/* Allocate 8 queues */
	for (i = 0; i < 8; ++i) {
		pp_qos_queue_allocate(qdev, &q[i]);
		confq.wred_max_allowed = i;
		pp_qos_queue_set(qdev, q[i], &confq);
	}

	PPUT_FAIL_TEST(qos_get_node_info(qdev, p1, &info) == 0,
		       "Could not get node info\n");
	if (!(info.type == PPV4_QOS_NODE_TYPE_PORT &&
	      info.children[0].phy == QOS_TEST_MAX_PORTS &&
	      info.children[1].phy == (QOS_TEST_MAX_PORTS + 1) &&
	      info.children[2].phy == (QOS_TEST_MAX_PORTS + 2) &&
	      info.children[3].phy == (QOS_TEST_MAX_PORTS + 3) &&
	      info.children[4].phy == (QOS_TEST_MAX_PORTS + 4) &&
	      info.children[5].phy == (QOS_TEST_MAX_PORTS + 5) &&
	      info.children[6].phy == (QOS_TEST_MAX_PORTS + 6) &&
	      info.children[7].phy == (QOS_TEST_MAX_PORTS + 7) &&
	      info.children[0].id == 1 &&
	      info.children[1].id == 2 &&
	      info.children[2].id == 3 &&
	      info.children[3].id == 4 &&
	      info.children[4].id == 5 &&
	      info.children[5].id == 6 &&
	      info.children[6].id == 7 &&
	      info.children[7].id == 8 &&
	      info.is_internal == 0 &&
	      info.bw_limit == 0 &&
	      (info.preds[0].phy == info.preds[0].id &&
	       info.preds[1].phy == info.preds[1].id &&
	       info.preds[2].phy == info.preds[2].id &&
	       info.preds[3].phy == info.preds[3].id &&
	       info.preds[4].phy == info.preds[4].id &&
	       info.preds[5].phy == info.preds[5].id &&
	       info.port == info.queue_physical_id))) {
		pr_err("Unexpected value from node get info\n");
		dump_node_info(&info);
	}

	PPUT_FAIL_TEST(qos_get_node_info(qdev, q[7], &info) == 0,
		       "Could not get node info\n");
	if (!(info.type == PPV4_QOS_NODE_TYPE_QUEUE &&
	      info.children[0].phy == PPV4_QOS_INVALID &&
	      info.children[1].phy == PPV4_QOS_INVALID &&
	      info.children[2].phy == PPV4_QOS_INVALID &&
	      info.children[3].phy == PPV4_QOS_INVALID &&
	      info.children[4].phy == PPV4_QOS_INVALID &&
	      info.children[5].phy == PPV4_QOS_INVALID &&
	      info.children[6].phy == PPV4_QOS_INVALID &&
	      info.children[7].phy == PPV4_QOS_INVALID &&
	      info.children[0].id == PPV4_QOS_INVALID &&
	      info.children[1].id == PPV4_QOS_INVALID &&
	      info.children[2].id == PPV4_QOS_INVALID &&
	      info.children[3].id == PPV4_QOS_INVALID &&
	      info.children[4].id == PPV4_QOS_INVALID &&
	      info.children[5].id == PPV4_QOS_INVALID &&
	      info.children[6].id == PPV4_QOS_INVALID &&
	      info.children[7].id == PPV4_QOS_INVALID &&
	      info.is_internal == 0 &&
	      info.bw_limit == 0 &&
	      info.preds[0].phy == 1 &&
	      info.preds[0].id == 0 &&
	      info.preds[1].phy == info.preds[1].id &&
	      info.preds[2].phy == info.preds[2].id &&
	      info.preds[3].phy == info.preds[3].id &&
	      info.preds[4].phy == info.preds[4].id &&
	      info.preds[5].phy == info.preds[5].id &&
	      info.port == 1 &&
	      info.queue_physical_id == 7)) {
		pr_err("Unexpected value from node get info\n");
		dump_node_info(&info);
	}

	/* Allocate 64 more queues */
	for (i = 8; i < 100; ++i) {
		pp_qos_queue_allocate(qdev, &q[i]);
		pp_qos_queue_set(qdev, q[i], &confq);
	}
	PPUT_FAIL_TEST(qos_get_node_info(qdev, q[7], &info) == 0,
		       "Could not get node info\n");
	if (!(info.type == PPV4_QOS_NODE_TYPE_QUEUE &&
	      info.children[0].phy == PPV4_QOS_INVALID &&
	      info.children[1].phy == PPV4_QOS_INVALID &&
	      info.children[2].phy == PPV4_QOS_INVALID &&
	      info.children[3].phy == PPV4_QOS_INVALID &&
	      info.children[4].phy == PPV4_QOS_INVALID &&
	      info.children[5].phy == PPV4_QOS_INVALID &&
	      info.children[6].phy == PPV4_QOS_INVALID &&
	      info.children[7].phy == PPV4_QOS_INVALID &&
	      info.children[0].id == PPV4_QOS_INVALID &&
	      info.children[1].id == PPV4_QOS_INVALID &&
	      info.children[2].id == PPV4_QOS_INVALID &&
	      info.children[3].id == PPV4_QOS_INVALID &&
	      info.children[4].id == PPV4_QOS_INVALID &&
	      info.children[5].id == PPV4_QOS_INVALID &&
	      info.children[6].id == PPV4_QOS_INVALID &&
	      info.children[7].id == PPV4_QOS_INVALID &&
	      info.is_internal == 0 &&
	      info.bw_limit == 0 &&
	      info.preds[0].phy == (QOS_TEST_MAX_PORTS + 7) &&
	      info.preds[0].id == 66 &&
	      info.preds[1].phy == 1 &&
	      info.preds[1].id == 0 &&
	      info.preds[2].phy == info.preds[2].id &&
	      info.preds[3].phy == info.preds[3].id &&
	      info.preds[4].phy == info.preds[4].id &&
	      info.preds[5].phy == info.preds[5].id &&
	      info.port == 1 &&
	      info.queue_physical_id == 7)) {
		pr_err("Unexpected value from node get info\n");
		dump_node_info(&info);
	}

	PPUT_FAIL_TEST(qos_get_node_info(qdev, 66, &info) == 0,
		       "Could not get node info\n");
	if (!(info.type == PPV4_QOS_NODE_TYPE_SCHED &&
	      info.children[0].phy == (QOS_TEST_MAX_PORTS + 64) &&
	      info.children[1].phy == (QOS_TEST_MAX_PORTS + 65) &&
	      info.children[2].phy == (QOS_TEST_MAX_PORTS + 66) &&
	      info.children[3].phy == (QOS_TEST_MAX_PORTS + 67) &&
	      info.children[4].phy == (QOS_TEST_MAX_PORTS + 68) &&
	      info.children[5].phy == (QOS_TEST_MAX_PORTS + 69) &&
	      info.children[6].phy == (QOS_TEST_MAX_PORTS + 70) &&
	      info.children[7].phy == (QOS_TEST_MAX_PORTS + 71) &&
	      info.children[0].id == 8 &&
	      info.children[1].id == 65 &&
	      info.children[2].id == 67 &&
	      info.children[3].id == 68 &&
	      info.children[4].id == 69 &&
	      info.children[5].id == 70 &&
	      info.children[6].id == 71 &&
	      info.children[7].id == 72 &&
	      info.is_internal == 1 &&
	      info.bw_limit == 0 &&
	      info.preds[0].phy == 1 &&
	      info.preds[0].id == 0 &&
	      info.preds[1].phy == info.preds[1].id &&
	      info.preds[2].phy == info.preds[2].id &&
	      info.preds[3].phy == info.preds[3].id &&
	      info.preds[4].phy == info.preds[4].id &&
	      info.preds[5].phy == info.preds[5].id &&
	      info.port == PPV4_QOS_INVALID &&
	      info.queue_physical_id == PPV4_QOS_INVALID)) {
		pr_err("Unexpected value from node get info\n");
		dump_node_info(&info);
	}

	ret = check_consistency_with_resources(qdev);
	if (ret)
		goto done;

	test_ret = PP_UT_PASS;
	DUMP_QOS_TREE(qdev);
done:
	for (i = 0; i < 100; ++i) {
		pp_qos_queue_block(qdev, q[i]);
		qos_queue_flush(qdev, q[i]);
		PPUT_FAIL_RETURN(pp_qos_queue_remove(qdev, q[i]) == 0,
				 "Could not remove queue %d\n", q[i]);
	}

	PPUT_FAIL_RETURN(pp_qos_port_remove(qdev, p1) == 0,
			 "Could not remove queue %d\n", p1);

#ifdef FW_CHECK_SYNC
	if (check_sync_with_fw(qdev, true)) {
		pr_err("DB not consistent with fw\n");
		test_ret = PP_UT_FAIL;
	}
#endif
	return test_ret;
}
#endif

static int block_unblock_test(void *data)
{
	struct pp_qos_dev *qdev;
	unsigned int p25 = PP_QOS_INVALID_ID;
	unsigned int q0 = PP_QOS_INVALID_ID;
	unsigned int q1 = PP_QOS_INVALID_ID;
	struct pp_qos_port_conf confp;
	struct pp_qos_queue_conf confq;
	u32 test_ret = PP_UT_FAIL;

	qdev = qos_restart(PP_QOS_INSTANCE_ID);
	if (!qdev)
		return PP_UT_FAIL;

	pp_qos_port_conf_set_default(&confp);
	confp.port_parent_prop.arbitration = PP_QOS_ARBITRATION_WRR;
	confp.ring_size = 1;

	/* add port 25 */
	PPUT_FAIL_TEST(pp_qos_port_allocate(qdev, 25, &p25) == 0,
		       "Could not allocate port 25\n");
	confp.packet_credit_enable = 1;
	confp.credit = 2;
	confp.ring_address = ring_addr_phys[0];
	PPUT_FAIL_TEST(pp_qos_port_set(qdev, p25, &confp) == 0,
		       "Could not set port 0\n");

	pp_qos_queue_conf_set_default(&confq);
	confq.queue_child_prop.parent = p25;

	PPUT_FAIL_TEST(pp_qos_queue_allocate(qdev, &q0) == 0,
		       "Could not allocate queue\n");
	confq.blocked = 1;
	confq.wred_max_allowed = 0x400;
	PPUT_FAIL_TEST(pp_qos_queue_set(qdev, q0, &confq) == 0,
		       "Setting queue fail\n");

	PPUT_FAIL_TEST(pp_qos_queue_allocate(qdev, &q1) == 0,
		       "Could not allocate queue\n");
	confq.blocked = 0;
	confq.wred_max_allowed = 0x200;
	PPUT_FAIL_TEST(pp_qos_queue_set(qdev, q1, &confq) == 0,
		       "Setting queue fail\n");

	pp_qos_queue_unblock(qdev, q0);
	pp_qos_queue_block(qdev, q1);

	confq.blocked = 1;
	confq.wred_max_allowed = 0x200;
	PPUT_FAIL_TEST(pp_qos_queue_set(qdev, q0, &confq) == 0,
		       "Setting queue fail\n");

	confq.wred_max_allowed = 0x400;
	confq.blocked = 0;
	PPUT_FAIL_TEST(pp_qos_queue_set(qdev, q1, &confq) == 0,
		       "Setting queue fail\n");

	test_ret = PP_UT_PASS;
	DUMP_QOS_TREE(qdev);
done:
	pp_qos_queue_block(qdev, q0);
	qos_queue_flush(qdev, q0);
	PPUT_FAIL_RETURN(pp_qos_queue_remove(qdev, q0) == 0,
			 "Could not remove queue %d\n", q0);

	pp_qos_queue_block(qdev, q1);
	qos_queue_flush(qdev, q1);
	PPUT_FAIL_RETURN(pp_qos_queue_remove(qdev, q1) == 0,
			 "Could not remove queue %d\n", q1);

	PPUT_FAIL_RETURN(pp_qos_port_remove(qdev, p25) == 0,
			 "Could not remove queue %d\n", p25);

#ifdef FW_CHECK_SYNC
	if (check_sync_with_fw(qdev, true)) {
		pr_err("DB not consistent with fw\n");
		test_ret = PP_UT_FAIL;
	}
#endif

	return test_ret;
}

static int four_ports_four_queues_test(void *data)
{
	struct pp_qos_dev *qdev;
	unsigned int p[4] = {PP_QOS_INVALID_ID};
	unsigned int q[4] = {PP_QOS_INVALID_ID};
	struct pp_qos_port_conf confp;
	struct pp_qos_queue_conf confq;
	u32 test_ret = PP_UT_FAIL;

	qdev = qos_restart(PP_QOS_INSTANCE_ID);
	if (!qdev)
		return PP_UT_FAIL;

	pp_qos_port_conf_set_default(&confp);

	confp.port_parent_prop.arbitration = PP_QOS_ARBITRATION_WRR;
	confp.ring_size = 1;
	confp.packet_credit_enable = 1;
	confp.credit = 2;

	/* add port 0 */
	PPUT_FAIL_TEST(pp_qos_port_allocate(qdev, 0, &p[0]) == 0,
		       "Could not allocate port 0\n");
	confp.ring_address = ring_addr_phys[0];
	PPUT_FAIL_TEST(pp_qos_port_set(qdev, p[0], &confp) == 0,
		       "Could not set port 0\n");

	/* add port 1 */
	PPUT_FAIL_TEST(pp_qos_port_allocate(qdev, 1, &p[1]) == 0,
		       "Could not allocate port 1\n");
	confp.ring_address = ring_addr_phys[1];
	PPUT_FAIL_TEST(pp_qos_port_set(qdev, p[1], &confp) == 0,
		       "Could not set port 1\n");

	/* add port 2 */
	PPUT_FAIL_TEST(pp_qos_port_allocate(qdev, 2, &p[2]) == 0,
		       "Could not allocate port 2\n");
	confp.ring_address = ring_addr_phys[2];
	PPUT_FAIL_TEST(pp_qos_port_set(qdev, p[2], &confp) == 0,
		       "Could not set port 2\n");

	/* add port 3 */
	PPUT_FAIL_TEST(pp_qos_port_allocate(qdev, 3, &p[3]) == 0,
		       "Could not allocate port 3\n");
	confp.ring_address = ring_addr_phys[3];
	PPUT_FAIL_TEST(pp_qos_port_set(qdev, p[3], &confp) == 0,
		       "Could not set port 3\n");

	pp_qos_queue_conf_set_default(&confq);
	confq.wred_max_allowed = 0x400;

	confq.queue_child_prop.parent = p[0];
	PPUT_FAIL_TEST(pp_qos_queue_allocate(qdev, &q[0]) == 0,
		       "Could not allocate queue\n");
	PPUT_FAIL_TEST(pp_qos_queue_set(qdev, q[0], &confq) == 0,
		       "Setting queue fail\n");

	confq.queue_child_prop.parent = p[1];
	PPUT_FAIL_TEST(pp_qos_queue_allocate(qdev, &q[1]) == 0,
		       "Could not allocate queue\n");
	PPUT_FAIL_TEST(pp_qos_queue_set(qdev, q[1], &confq) == 0,
		       "Setting queue fail\n");

	confq.queue_child_prop.parent = p[2];
	PPUT_FAIL_TEST(pp_qos_queue_allocate(qdev, &q[2]) == 0,
		       "Could not allocate queue\n");
	PPUT_FAIL_TEST(pp_qos_queue_set(qdev, q[2], &confq) == 0,
		       "Setting queue fail\n");

	confq.queue_child_prop.parent = p[3];
	PPUT_FAIL_TEST(pp_qos_queue_allocate(qdev, &q[3]) == 0,
		       "Could not allocate queue\n");
	PPUT_FAIL_TEST(pp_qos_queue_set(qdev, q[3], &confq) == 0,
		       "Setting queue fail\n");

	confp.port_parent_prop.arbitration = PP_QOS_ARBITRATION_WSP;
	PPUT_FAIL_TEST(pp_qos_port_set(qdev, p[0], &confp) != 0,
		       "Succeeds setting port arbitration\n");

	test_ret = PP_UT_PASS;
	DUMP_QOS_TREE(qdev);
done:
	pp_qos_queue_block(qdev, q[0]);
	qos_queue_flush(qdev, q[0]);
	PPUT_FAIL_RETURN(pp_qos_queue_remove(qdev, q[0]) == 0,
			 "Could not remove queue %d\n", q[0]);

	pp_qos_queue_block(qdev, q[1]);
	qos_queue_flush(qdev, q[1]);
	PPUT_FAIL_RETURN(pp_qos_queue_remove(qdev, q[1]) == 0,
			 "Could not remove queue %d\n", q[1]);

	pp_qos_queue_block(qdev, q[2]);
	qos_queue_flush(qdev, q[2]);
	PPUT_FAIL_RETURN(pp_qos_queue_remove(qdev, q[2]) == 0,
			 "Could not remove queue %d\n", q[2]);

	pp_qos_queue_block(qdev, q[3]);
	qos_queue_flush(qdev, q[3]);
	PPUT_FAIL_RETURN(pp_qos_queue_remove(qdev, q[3]) == 0,
			 "Could not remove queue %d\n", q[3]);

	PPUT_FAIL_RETURN(pp_qos_port_remove(qdev, p[0]) == 0,
		 "Could not remove queue %d\n", p[0]);

	PPUT_FAIL_RETURN(pp_qos_port_remove(qdev, p[1]) == 0,
		 "Could not remove queue %d\n", p[1]);

	PPUT_FAIL_RETURN(pp_qos_port_remove(qdev, p[2]) == 0,
		 "Could not remove queue %d\n", p[2]);

	PPUT_FAIL_RETURN(pp_qos_port_remove(qdev, p[3]) == 0,
		 "Could not remove queue %d\n", p[3]);

#ifdef FW_CHECK_SYNC
	if (check_sync_with_fw(qdev, true)) {
		pr_err("DB not consistent with fw\n");
		test_ret = PP_UT_FAIL;
	}
#endif

	return test_ret;
}

static int bw_share_test(void *data)
{
	struct pp_qos_dev *qdev;
	unsigned int p0 = PP_QOS_INVALID_ID;
	unsigned int q[4] = {PP_QOS_INVALID_ID};
	struct pp_qos_port_conf confp;
	struct pp_qos_queue_conf confq;
	u32 test_ret = PP_UT_FAIL;

	qdev = qos_restart(PP_QOS_INSTANCE_ID);
	if (!qdev)
		return PP_UT_FAIL;

	pp_qos_port_conf_set_default(&confp);

	confp.port_parent_prop.arbitration = PP_QOS_ARBITRATION_WRR;
	confp.ring_size = 1;
	confp.packet_credit_enable = 1;
	confp.credit = 2;

	/* add port 0 */
	PPUT_FAIL_TEST(pp_qos_port_allocate(qdev, 0, &p0) == 0,
		       "Could not allocate port 0\n");
	confp.ring_address = ring_addr_phys[0];
	confp.common_prop.bandwidth_limit = 480;
	PPUT_FAIL_TEST(pp_qos_port_set(qdev, p0, &confp) == 0,
		       "Could not set port 0\n");

	pp_qos_queue_conf_set_default(&confq);
	confq.wred_max_allowed = 0x400;
	confq.queue_child_prop.parent = p0;

	confq.queue_child_prop.priority = 0;
	confq.queue_child_prop.wrr_weight = 10;
	PPUT_FAIL_TEST(pp_qos_queue_allocate(qdev, &q[0]) == 0,
		       "Could not allocate queue\n");
	PPUT_FAIL_TEST(pp_qos_queue_set(qdev, q[0], &confq) == 0,
		       "Setting queue fail\n");

	confq.queue_child_prop.priority = 1;
	confq.queue_child_prop.wrr_weight = 40;
	PPUT_FAIL_TEST(pp_qos_queue_allocate(qdev, &q[1]) == 0,
		       "Could not allocate queue\n");
	PPUT_FAIL_TEST(pp_qos_queue_set(qdev, q[1], &confq) == 0,
		       "Setting queue fail\n");

	confq.queue_child_prop.priority = 2;
	confq.queue_child_prop.wrr_weight = 30;
	PPUT_FAIL_TEST(pp_qos_queue_allocate(qdev, &q[2]) == 0,
		       "Could not allocate queue\n");
	PPUT_FAIL_TEST(pp_qos_queue_set(qdev, q[2], &confq) == 0,
		       "Setting queue fail\n");

	confq.queue_child_prop.priority = 3;
	confq.queue_child_prop.wrr_weight = 5;
	PPUT_FAIL_TEST(pp_qos_queue_allocate(qdev, &q[3]) == 0,
		       "Could not allocate queue\n");
	PPUT_FAIL_TEST(pp_qos_queue_set(qdev, q[3], &confq) == 0,
		       "Setting queue fail\n");

#ifdef FW_CHECK_SYNC
	if (check_sync_with_fw(qdev, false)) {
		pr_err("DB not consistent with fw\n");
		test_ret = PP_UT_FAIL;
	}
#endif

	test_ret = PP_UT_PASS;
	DUMP_QOS_TREE(qdev);
done:
	pp_qos_queue_block(qdev, q[0]);
	qos_queue_flush(qdev, q[0]);
	PPUT_FAIL_RETURN(pp_qos_queue_remove(qdev, q[0]) == 0,
			 "Could not remove queue %d\n", q[0]);

	pp_qos_queue_block(qdev, q[1]);
	qos_queue_flush(qdev, q[1]);
	PPUT_FAIL_RETURN(pp_qos_queue_remove(qdev, q[1]) == 0,
			 "Could not remove queue %d\n", q[1]);

	pp_qos_queue_block(qdev, q[2]);
	qos_queue_flush(qdev, q[2]);
	PPUT_FAIL_RETURN(pp_qos_queue_remove(qdev, q[2]) == 0,
			 "Could not remove queue %d\n", q[2]);

	pp_qos_queue_block(qdev, q[3]);
	qos_queue_flush(qdev, q[3]);
	PPUT_FAIL_RETURN(pp_qos_queue_remove(qdev, q[3]) == 0,
			 "Could not remove queue %d\n", q[3]);

	PPUT_FAIL_RETURN(pp_qos_port_remove(qdev, p0) == 0,
		 "Could not remove queue %d\n", p0);

#ifdef FW_CHECK_SYNC
	if (check_sync_with_fw(qdev, true)) {
		pr_err("DB not consistent with fw\n");
		test_ret = PP_UT_FAIL;
	}
#endif

	return test_ret;
}

/**
 * Register all pp_qos tests to pput
 */
void pp_qos_tests_init(void)
{
	int test_idx;

	/* Add Tests */
	for (test_idx = 0 ; test_idx < ARRAY_SIZE(pp_qos_tests); test_idx++)
		pp_register_test(&pp_qos_tests[test_idx]);
}
