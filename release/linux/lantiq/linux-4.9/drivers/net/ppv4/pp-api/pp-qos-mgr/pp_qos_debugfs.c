/*
 * Copyright (C) 2020-2021 MaxLinear, Inc.
 * Copyright (C) 2017-2020 Intel Corporation
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2, as published by the Free Software Foundation.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR APARTICULARPURPOSE.See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public
 * License along with this program; if not,see
 * <http://www.gnu.org/licenses/>.
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * Description: Packet Processor QoS Driver
 */

#define pr_fmt(fmt) "[PP_QOS_DBG]:%s:%d: " fmt, __func__, __LINE__

#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/debugfs.h>
#include <linux/jiffies.h>
#include "pp_debugfs_common.h"
#include "pp_qos_common.h"
#include "pp_qos_utils.h"
#include "pp_qos_fw.h"
#include "pp_regs.h"
#include "infra.h"
#include "pp_desc.h"

#define PP_QOS_DEBUGFS_DIR "ppv4_qos"

#define AQM_LATENCY_TARGET  (10)       /* 10 miliseconds */
#define AQM_PEAK_RATE       (13107200) /* 100 Mbps */
#define AQM_MSR             (13107200) /* 100 Mbps */
#define AQM_BUFFER_SIZE     (655360)   /* 50ms base on MSR */

static u16 g_node;
static u8  g_raw_config;

void remove_node(char *cmd_buf, void *data)
{
	struct pp_qos_dev *qdev;
	char node_type[16];
	u32 count, id = 0;

	qdev = pp_qos_dev_open((unsigned long)data);
	if (sscanf(cmd_buf, "%10s %u", node_type, &id) != 2) {
		QOS_LOG_ERR("sscanf err\n");
		return;
	}

	if (!strncmp(node_type, "port", strlen("port"))) {
		pp_qos_port_remove(qdev, id);
	} else if (!strncmp(node_type, "sched", strlen("sched"))) {
		pp_qos_sched_remove(qdev, id);
	} else if (!strncmp(node_type, "queue", strlen("queue"))) {
		pp_qos_queue_remove(qdev, id);
	} else if (!strncmp(node_type, "cont_queue", strlen("cont_queue"))) {
		if (sscanf(cmd_buf, "%16s %u %u", node_type, &id,
			   &count) != 3) {
			QOS_LOG_ERR("sscanf err\n");
			return;
		}
		pp_qos_contiguous_queue_remove(qdev, id, count);
	} else {
		QOS_LOG_ERR("Type %s not supported\n", node_type);
	}
}

void remove_node_help(struct seq_file *f)
{
	seq_puts(f, "<port/sched/queue> <node id>\n");
	seq_puts(f, "<cont_queue> <start id> <count>\n");
}

PP_DEFINE_DEBUGFS(remove, remove_node_help, remove_node);

void allocate_node(char *cmd_buf, void *data)
{
	struct pp_qos_dev *qdev;
	u32 phy = 0, id, count;
	char node_type[16];
	u32 *ids, *rlms;
	s32 rc;

	qdev = pp_qos_dev_open((unsigned long)data);
	if (sscanf(cmd_buf, "%16s", node_type) != 1) {
		QOS_LOG_ERR("sscanf err\n");
		return;
	}

	if (!strncmp(node_type, "port", 6)) {
		if (sscanf(cmd_buf, "%16s %u", node_type, &phy) != 2) {
			QOS_LOG_ERR("sscanf err\n");
			return;
		}
		pp_qos_port_allocate(qdev, phy, &id);
	} else if (!strncmp(node_type, "sched", strlen("sched"))) {
		pp_qos_sched_allocate(qdev, &id);
	} else if (!strncmp(node_type, "queue_id_phy",
			    strlen("queue_id_phy"))) {
		pp_qos_queue_allocate_id_phy(qdev, &id, &phy);
		QOS_LOG_INFO("Allocated id %u rlm %u\n", id, phy);
		return;
	} else if (!strncmp(node_type, "queue", strlen("queue"))) {
		pp_qos_queue_allocate(qdev, &id);
	} else if (!strncmp(node_type, "cont_queue", strlen("cont_queue"))) {
		if (sscanf(cmd_buf, "%16s %u", node_type, &count) != 2) {
			QOS_LOG_ERR("sscanf err\n");
			return;
		}
		ids = kzalloc(qdev->init_params.reserved_queues * sizeof(u32),
			      GFP_KERNEL);
		rlms = kzalloc(qdev->init_params.reserved_queues * sizeof(u32),
			       GFP_KERNEL);
		rc = pp_qos_contiguous_queue_allocate(qdev, ids, rlms, count);
		if (!rc) {
			QOS_LOG_INFO("Alloc %u contq start @ id %u rlm %u\n",
				     count, *ids, *rlms);
		}

		kfree(ids);
		kfree(rlms);
		return;
	} else {
		QOS_LOG_ERR("Type %s not supported\n", node_type);
		return;
	}

	QOS_LOG_INFO("Allocated id %u\n", id);
}

void allocate_node_help(struct seq_file *f)
{
	seq_printf(f, "<port/sched/queue> <port phy (%#x for automatic phy)>\n",
		   ALLOC_PORT_ID);
	seq_puts(f, "<cont_queue> <count>\n");
	seq_puts(f, "<queue_id_phy>\n");
}

PP_DEFINE_DEBUGFS(allocate, allocate_node_help, allocate_node);

void queue_flush(char *cmd_buf, void *data)
{
	struct pp_qos_dev *qdev;
	u32 id = 0;

	qdev = pp_qos_dev_open((unsigned long)data);

	if (kstrtou32(cmd_buf, 10, &id)) {
		QOS_LOG_ERR("kstrtou32 err\n");
		return;
	}

	qos_queue_flush(qdev, id);
}

void queue_flush_help(struct seq_file *f)
{
	seq_puts(f, "<queue id>\n");
}

PP_DEFINE_DEBUGFS(flush, queue_flush_help, queue_flush);

void add_shared_bwl_group(char *cmd_buf, void *data)
{
	struct pp_qos_dev *qdev;
	u32 max_burst;
	u32 id = 0;

	qdev = pp_qos_dev_open((unsigned long)data);

	if (kstrtou32(cmd_buf, 10, &max_burst)) {
		QOS_LOG_ERR("kstrtou32 err\n");
		return;
	}

	pp_qos_shared_limit_group_add(qdev, max_burst, &id);

	QOS_LOG_INFO("id %u, max_burst %u\n", id, max_burst);
}

void add_shared_bwl_group_help(struct seq_file *f)
{
	seq_puts(f, "<max_burst>\n");
}

PP_DEFINE_DEBUGFS(shared_grp_add, add_shared_bwl_group_help,
		  add_shared_bwl_group);

void remove_shared_bwl_group(char *cmd_buf, void *data)
{
	struct pp_qos_dev *qdev;
	u32 id = 0;

	qdev = pp_qos_dev_open((unsigned long)data);

	if (kstrtou32(cmd_buf, 10, &id)) {
		QOS_LOG_ERR("kstrtou32 err\n");
		return;
	}

	pp_qos_shared_limit_group_remove(qdev, id);
}

void remove_shared_bwl_group_help(struct seq_file *f)
{
	seq_puts(f, "<bwl group id to remove>\n");
}

PP_DEFINE_DEBUGFS(shared_grp_del, remove_shared_bwl_group_help,
		  remove_shared_bwl_group);

struct dbg_prop {
	char          field[32];
	char          desc[128];
	u8            data_type; /* 0 - normal, 1 - pointer */
	u32           *dest;
	unsigned long **pdest;
};

struct dbg_props_cbs {
	s32 (*first_prop_cb)(struct pp_qos_dev *qdev,
			     char *field,
			     u32 val,
			     void *user_data,
			     u8 raw_config);

	s32 (*done_props_cb)(struct pp_qos_dev *qdev,
			     u32 val, void *user_data);
};

void qos_dbg_props(char *cmd_buf,
		   struct pp_qos_dev *qdev,
		   struct dbg_props_cbs *cbs,
		   struct dbg_prop props[],
		   u16 num_props,
		   void *user_data)
{
	u8 *field;
	char *tok, *ptr, *pval;
	unsigned long res;
	u16 i, num_changed = 0;
	u32 id = PP_QOS_INVALID_ID;
	u32 len, first_prop = 1;
	s32 ret;

	field = kzalloc(PP_DBGFS_WR_STR_MAX, GFP_KERNEL);
	if (!field)
		return;

	ptr = (char *)cmd_buf;

	while ((tok = strsep(&ptr, " \t\n\r")) != NULL) {
		if (tok[0] == '\0')
			continue;

		ret = strscpy(field, tok, PP_DBGFS_WR_STR_MAX);
		if (ret < 0) {
			QOS_LOG_ERR("Strscpy failed (%d)\n", ret);
			goto out;
		}

		pval = strchr(field, '=');
		if (!pval) {
			QOS_LOG_ERR("Wrong format for prop %s\n", tok);
			goto out;
		}

		*pval = '\0';
		pval++;

		ret = kstrtoul(pval, 0, &res);
		if (ret) {
			QOS_LOG_ERR("kstrtoul failure (%d)\n", ret);
			goto out;
		}

		if (first_prop) {
			first_prop = 0;
			id = res;
			if (cbs && cbs->first_prop_cb &&
			    cbs->first_prop_cb(qdev, field, res,
					       user_data, g_raw_config)) {
				QOS_LOG_ERR("first_prop_cb failed\n");
				goto out;
			}
		}

		for (i = 0; i < num_props ; i++) {
			len = max(strlen(props[i].field), strlen(field));
			if (!strncmp(field, props[i].field, len)) {
				if (props[i].data_type == 0)
					*(props[i].dest) = res;
				else
					*(props[i].pdest) = (void *)res;
				num_changed++;
				break;
			}
		}

		if (i == num_props)
			QOS_LOG_ERR("Not supported field %s", field);
	}

	if (id != PP_QOS_INVALID_ID) {
		/* If only logical id was set, print current configuration */
		if (num_changed == 1) {
			QOS_LOG_INFO("Current configuration:\n");

			for (i = 0; i < num_props ; i++) {
				if (props[i].data_type == 0) {
					QOS_LOG_INFO("%-30s%u\n",
						     props[i].field,
						     *props[i].dest);
				} else {
					QOS_LOG_INFO("%-30s%#lx\n",
						     props[i].field,
						     (ulong)(*props[i].pdest));
				}
			}

			goto out;
		}

		if (cbs && cbs->done_props_cb) {
			if (cbs->done_props_cb(qdev, id, user_data)) {
				QOS_LOG_ERR("done_props_cb failed\n");
				goto out;
			}
		}
	}

out:
	kfree(field);
}

void qos_dbg_props_help(struct seq_file *f, const char *name,
			const char *format, struct dbg_prop props[],
			u16 num_props)
{
	u32 idx;

	seq_printf(f, "<---- %s---->\n", name);
	seq_printf(f, "[FORMAT] %s\n", format);
	seq_puts(f, "[FORMAT] If only id is set, operation is get conf\n");
	seq_puts(f, "Supported fields\n");

	for (idx = 0; idx < num_props ; idx++)
		seq_printf(f, "%-30s%s\n", props[idx].field, props[idx].desc);
}

static void dbg_add_prop(struct dbg_prop *props, u16 *pos, u16 size,
			 const char *name, const char *desc, u32 *dest)
{
	s32 ret;

	if (*pos >= size) {
		QOS_LOG_ERR("pos %d >= size %d", *pos, size);
		return;
	}

	ret = strscpy(props[*pos].field, name, sizeof(props[*pos].field));
	if (ret < 0) {
		QOS_LOG_ERR("Strscpy failed (%d)\n", ret);
		return;
	}

	ret = strscpy(props[*pos].desc, desc, sizeof(props[*pos].desc));
	if (ret < 0) {
		QOS_LOG_ERR("Strscpy failed (%d)\n", ret);
		return;
	}

	props[*pos].data_type = 0;
	props[*pos].dest = dest;

	(*pos)++;
}

static void dbg_add_prop_ptr(struct dbg_prop *props, u16 *pos, u16 size,
			     const char *name, const char *desc,
			     unsigned long **dest)
{
	s32 ret;

	if (*pos >= size) {
		QOS_LOG_ERR("pos %d >= size %d", *pos, size);
		return;
	}

	ret = strscpy(props[*pos].field, name, sizeof(props[*pos].field));
	if (ret < 0) {
		QOS_LOG_ERR("Strscpy failed (%d)\n", ret);
		return;
	}

	ret = strscpy(props[*pos].desc, desc, sizeof(props[*pos].desc));
	if (ret < 0) {
		QOS_LOG_ERR("Strscpy failed (%d)\n", ret);
		return;
	}

	props[*pos].data_type = 1;
	props[*pos].pdest = dest;

	(*pos)++;
}

static u16 create_port_props(struct dbg_prop *props, u16 size,
			     u32 *id, struct pp_qos_port_conf *pconf)
{
	u16 num = 0;

	dbg_add_prop(props, &num, size, "port",
		     "Logical id. Must exist as the first property!", id);
	dbg_add_prop(props, &num, size, "bw", "Limit in kbps (80kbps steps)",
		     &pconf->common_prop.bandwidth_limit);
	dbg_add_prop(props, &num, size, "shared",
		     "Shared bw group: 1-511 (0 for remove group)",
		     &pconf->common_prop.shared_bw_group);
	dbg_add_prop(props, &num, size, "max_burst",
		     "Defines the max quantas that can be accumulated (1 << shift)",
		     &pconf->common_prop.max_burst);
	dbg_add_prop(props, &num, size, "arb",
		     "Arbitration: 0 - WSP, 1 - WRR",
		     &pconf->port_parent_prop.arbitration);
	dbg_add_prop(props, &num, size, "be",
		     "Best effort enable: best effort scheduling is enabled",
		     &pconf->port_parent_prop.best_effort_enable);
	dbg_add_prop_ptr(props, &num, size, "r_addr",
			 "Ring address", (ulong **)&pconf->ring_address);
	dbg_add_prop(props, &num, size, "r_size",
		     "Ring size", &pconf->ring_size);
	dbg_add_prop(props, &num, size, "pkt_cred",
		     "Packet credit: 0 - byte credit, 1 - packet credit",
		     &pconf->packet_credit_enable);
	dbg_add_prop(props, &num, size, "cred", "Port credit", &pconf->credit);
	dbg_add_prop(props, &num, size, "dis",
		     "Disable port tx", &pconf->disable);
	dbg_add_prop(props, &num, size, "green_threshold",
		     "Egress bytes green threshold", &pconf->green_threshold);
	dbg_add_prop(props, &num, size, "yellow_threshold",
		     "Egress bytes yellow threshold",
		     &pconf->yellow_threshold);
	dbg_add_prop(props, &num, size, "enhanced_wsp",
		     "Enhanced WSP",
		     &pconf->enhanced_wsp);

	return num;
}

static s32 port_first_prop_cb(struct pp_qos_dev *qdev,
			      char *field,
			      u32 val,
			      void *user_data,
			      u8 raw_config)
{
	/* Make sure first property is the port id */
	if (strncmp(field, "port", strlen("port"))) {
		QOS_LOG_ERR("First prop (%s) must be port\n", field);
		return -EINVAL;
	}

	if (raw_config) {
		pp_qos_port_conf_set_default(user_data);
	} else if (pp_qos_port_conf_get(qdev, val, user_data) != 0) {
		QOS_LOG_ERR("pp_qos_port_conf_get failed (id %u)", val);
		return -EINVAL;
	}

	return 0;
}

static s32 port_done_props_cb(struct pp_qos_dev *qdev,
			      u32 val, void *user_data)
{
	if (pp_qos_port_set(qdev, val, user_data) != 0) {
		QOS_LOG_ERR("pp_qos_port_set failed (id %u)", val);
		return -EINVAL;
	}

	return 0;
}

void port(char *cmd_buf, void *data)
{
	struct pp_qos_port_conf conf;
	struct dbg_props_cbs cbs = {port_first_prop_cb, port_done_props_cb};
	u32 id = PP_QOS_INVALID_ID;
	u16 num_props;
	struct dbg_prop *props;
	struct pp_qos_dev *qdev;

	qdev = pp_qos_dev_open((unsigned long)data);
	props = kmalloc_array(DBG_MAX_PROPS, sizeof(struct dbg_prop),
			      GFP_KERNEL | __GFP_ZERO);
	if (!props)
		return;

	num_props = create_port_props(props, DBG_MAX_PROPS, &id, &conf);
	qos_dbg_props(cmd_buf, qdev, &cbs, props, num_props, &conf);
	kfree(props);
}

void port_help(struct seq_file *f)
{
	struct pp_qos_port_conf conf;
	const char *name = "set port";
	const char *format =
		"echo port=[logical_id] [field]=[value]... > port";
	struct dbg_prop *props;
	u16 num_props;
	u32 id = PP_QOS_INVALID_ID;

	props = kmalloc_array(DBG_MAX_PROPS, sizeof(struct dbg_prop),
			      GFP_KERNEL);
	if (!props)
		return;

	num_props = create_port_props(props, DBG_MAX_PROPS, &id, &conf);
	qos_dbg_props_help(f, name, format, props, num_props);
	kfree(props);
}

PP_DEFINE_DEBUGFS(port_set, port_help, port);

static u16 create_sched_props(struct dbg_prop *props, u16 size,
			      u32 *id, struct pp_qos_sched_conf *pconf)
{
	u16 num = 0;

	dbg_add_prop(props, &num, size, "sched",
		     "Logical id. Must exist as the first property!", id);
	dbg_add_prop(props, &num, size, "bw", "Limit in kbps (80kbps steps)",
		     &pconf->common_prop.bandwidth_limit);
	dbg_add_prop(props, &num, size, "shared",
		     "Shared bw group: 1-511 (0 for remove group)",
		     &pconf->common_prop.shared_bw_group);
	dbg_add_prop(props, &num, size, "max_burst",
		     "Defines the max quantas that can be accumulated (1 << shift)",
		     &pconf->common_prop.max_burst);
	dbg_add_prop(props, &num, size, "arb",
		     "Arbitration: 0 - WSP, 1 - WRR",
		     &pconf->sched_parent_prop.arbitration);
	dbg_add_prop(props, &num, size, "be",
		     "Best effort enable: best effort scheduling is enabled",
		     &pconf->sched_parent_prop.best_effort_enable);
	dbg_add_prop(props, &num, size, "parent", "logical parent id",
		     &pconf->sched_child_prop.parent);
	dbg_add_prop(props, &num, size, "priority",
		     "priority (0-7) in WSP",
		     &pconf->sched_child_prop.priority);
	dbg_add_prop(props, &num, size, "wrr_weight", "percentage from parent",
		     &pconf->sched_child_prop.wrr_weight);

	return num;
}

static s32 sched_first_prop_cb(struct pp_qos_dev *qdev,
			       char *field,
			       u32 val,
			       void *user_data,
			       u8 raw_config)
{
	/* Make sure first property is the sched id */
	if (strncmp(field, "sched", strlen("sched"))) {
		QOS_LOG_ERR("First prop (%s) must be sched\n", field);
		return -EINVAL;
	}

	if (raw_config) {
		pp_qos_sched_conf_set_default(user_data);
	} else if (pp_qos_sched_conf_get(qdev, val, user_data) != 0) {
		QOS_LOG_ERR("pp_qos_sched_conf_get failed (id %u)", val);
		return -EINVAL;
	}

	return 0;
}

static s32 sched_done_props_cb(struct pp_qos_dev *qdev,
			       u32 val, void *user_data)
{
	if (pp_qos_sched_set(qdev, val, user_data) != 0) {
		QOS_LOG_ERR("pp_qos_sched_set failed (id %u)", val);
		return -EINVAL;
	}

	return 0;
}

void sched(char *cmd_buf, void *data)
{
	struct pp_qos_sched_conf conf;
	struct dbg_props_cbs cbs = {sched_first_prop_cb, sched_done_props_cb};
	u32 id = PP_QOS_INVALID_ID;
	u16 num_props;
	struct dbg_prop *props;
	struct pp_qos_dev *qdev;

	qdev = pp_qos_dev_open((unsigned long)data);
	props = kmalloc_array(DBG_MAX_PROPS, sizeof(struct dbg_prop),
			      GFP_KERNEL);
	if (!props)
		return;

	num_props = create_sched_props(props, DBG_MAX_PROPS, &id, &conf);

	qos_dbg_props(cmd_buf, qdev, &cbs, props, num_props, &conf);
	kfree(props);
}

void sched_help(struct seq_file *f)
{
	struct pp_qos_sched_conf conf;
	const char *name = "set sched";
	const char *format =
		"echo sched=[logical_id] [field]=[value]... > sched";
	struct dbg_prop *props;
	u16 num_props;
	u32 id = PP_QOS_INVALID_ID;

	props = kmalloc_array(DBG_MAX_PROPS, sizeof(struct dbg_prop),
			      GFP_KERNEL);
	if (!props)
		return;

	num_props = create_sched_props(props, DBG_MAX_PROPS, &id, &conf);
	qos_dbg_props_help(f, name, format, props, num_props);
	kfree(props);
}

PP_DEFINE_DEBUGFS(sched_set, sched_help, sched);

static u16 create_queue_props(struct dbg_prop *props, u16 size,
			      u32 *id, struct pp_qos_queue_conf *pconf)
{
	u16 num = 0;

	dbg_add_prop(props, &num, size, "queue",
		     "Logical id. Must exist as the first property!", id);
	dbg_add_prop(props, &num, size, "bw", "Limit in kbps (80kbps steps)",
		     &pconf->common_prop.bandwidth_limit);
	dbg_add_prop(props, &num, size, "shared",
		     "Shared bw group: 1-511 (0 for remove group)",
		     &pconf->common_prop.shared_bw_group);
	dbg_add_prop(props, &num, size, "max_burst",
		     "Defines the max quantas that can be accumulated (1 << shift)",
		     &pconf->common_prop.max_burst);
	dbg_add_prop(props, &num, size, "parent", "logical parent id",
		     &pconf->queue_child_prop.parent);
	dbg_add_prop(props, &num, size, "priority",
		     "priority (0-7) in WSP",
		     &pconf->queue_child_prop.priority);
	dbg_add_prop(props, &num, size, "wrr_weight", "percentage from parent",
		     &pconf->queue_child_prop.wrr_weight);
	dbg_add_prop(props, &num, size, "blocked", "drop enqueued packets",
		     &pconf->blocked);
	dbg_add_prop(props, &num, size, "wred_enable", "enable wred drops",
		     &pconf->wred_enable);
	dbg_add_prop(props, &num, size, "wred_fixed_drop_prob",
		     "fixed prob instead of slope",
		     &pconf->wred_fixed_drop_prob_enable);
	dbg_add_prop(props, &num, size, "min_avg_green",
		     "Wred start of the slope area",
		     &pconf->wred_min_avg_green);
	dbg_add_prop(props, &num, size, "max_avg_green",
		     "Wred end of the slope area",
		     &pconf->wred_max_avg_green);
	dbg_add_prop(props, &num, size, "slope_green", "Wred 0-100 scale",
		     &pconf->wred_slope_green);
	dbg_add_prop(props, &num, size, "fixed_drop_prob_green",
		     "Wred fixed drop rate",
		     &pconf->wred_fixed_drop_prob_green);
	dbg_add_prop(props, &num, size, "min_avg_yellow",
		     "Wred start of the slope area",
		     &pconf->wred_min_avg_yellow);
	dbg_add_prop(props, &num, size, "max_avg_yellow",
		     "Wred end of the slope area",
		     &pconf->wred_max_avg_yellow);
	dbg_add_prop(props, &num, size, "slope_yellow", "Wred 0-100 scale",
		     &pconf->wred_slope_yellow);
	dbg_add_prop(props, &num, size, "fixed_drop_prob_yellow",
		     "Wred fixed drop rate",
		     &pconf->wred_fixed_drop_prob_yellow);
	dbg_add_prop(props, &num, size, "min_guaranteed",
		     "Wred min guaranteed for this queue",
		     &pconf->wred_min_guaranteed);
	dbg_add_prop(props, &num, size, "max_allowed",
		     "Wred max allowed for this queue",
		     &pconf->wred_max_allowed);
	dbg_add_prop(props, &num, size, "codel_en",
		     "Enable codel for this queue",
		     &pconf->codel_en);
	dbg_add_prop(props, &num, size, "eir",
		     "Excess Information Rate",
		     &pconf->eir);
	dbg_add_prop(props, &num, size, "ebs",
		     "Excess Burst Size",
		     &pconf->ebs);

	return num;
}

static s32 queue_first_prop_cb(struct pp_qos_dev *qdev,
			       char *field,
			       u32 val,
			       void *user_data,
			       u8 raw_config)
{
	/* Make sure first property is the queue id */
	if (strncmp(field, "queue", strlen("queue"))) {
		QOS_LOG_ERR("First prop (%s) must be queue\n", field);
		return -EINVAL;
	}

	if (raw_config) {
		pp_qos_queue_conf_set_default(user_data);
	} else if (pp_qos_queue_conf_get(qdev, val, user_data) != 0) {
		QOS_LOG_ERR("pp_qos_queue_conf_get failed (id %u)", val);
		return -EINVAL;
	}

	return 0;
}

static s32 queue_done_props_cb(struct pp_qos_dev *qdev,
			       u32 val, void *user_data)
{
	if (pp_qos_queue_set(qdev, val, user_data) != 0) {
		QOS_LOG_ERR("pp_qos_queue_set failed (id %u)", val);
		return -EINVAL;
	}

	return 0;
}

void queue(char *cmd_buf, void *data)
{
	struct pp_qos_queue_conf conf;
	struct dbg_props_cbs cbs = {queue_first_prop_cb, queue_done_props_cb};
	u32 id = PP_QOS_INVALID_ID;
	u16 num_props;
	struct dbg_prop *props;
	struct pp_qos_dev *qdev;

	qdev = pp_qos_dev_open((unsigned long)data);
	props = kmalloc_array(DBG_MAX_PROPS, sizeof(struct dbg_prop),
			      GFP_KERNEL);
	if (!props)
		return;

	num_props = create_queue_props(props, DBG_MAX_PROPS, &id, &conf);

	qos_dbg_props(cmd_buf, qdev, &cbs, props, num_props, &conf);
	kfree(props);
}

void queue_help(struct seq_file *f)
{
	struct pp_qos_queue_conf conf;
	const char *name = "set queue";
	const char *format =
		"echo queue=[logical_id] [field]=[value]... > queue";
	struct dbg_prop *props;
	u16 num_props;
	u32 id = PP_QOS_INVALID_ID;

	props = kmalloc_array(DBG_MAX_PROPS, sizeof(struct dbg_prop),
			      GFP_KERNEL);
	if (!props)
		return;

	num_props = create_queue_props(props, DBG_MAX_PROPS, &id, &conf);
	qos_dbg_props_help(f, name, format, props, num_props);
	kfree(props);
}

PP_DEFINE_DEBUGFS(queue_set, queue_help, queue);

static s32 syncq_first_prop_cb(struct pp_qos_dev *qdev, char *field, u32 val,
			       void *user_data, u8 raw_config)
{
	/* Make sure first property is the queue id */
	if (strncmp(field, "queue", strlen("queue"))) {
		QOS_LOG_ERR("First prop (%s) must be queue\n", field);
		return -EINVAL;
	}

	if (raw_config) {
		pp_qos_queue_conf_set_default(user_data);
	} else {
		QOS_LOG_ERR("Only raw config supported\n");
		return -EINVAL;
	}

	return 0;
}

static s32 syncq_done_props_cb(struct pp_qos_dev *qdev, u32 val,
			       void *user_data)
{
	u32 rlm;

	if (qos_sync_queue_add(qdev, val, &rlm, user_data) != 0) {
		QOS_LOG_ERR("qos_sync_queue_add failed (id %u)", val);
		return -EINVAL;
	}

	return 0;
}

void syncq(char *cmd_buf, void *data)
{
	struct pp_qos_queue_conf conf;
	struct dbg_props_cbs cbs = {syncq_first_prop_cb, syncq_done_props_cb};
	u32 id = PP_QOS_INVALID_ID;
	u16 num_props;
	struct dbg_prop *props;
	struct pp_qos_dev *qdev;

	qdev = pp_qos_dev_open((unsigned long)data);
	props = kmalloc_array(DBG_MAX_PROPS, sizeof(struct dbg_prop),
			      GFP_KERNEL);
	if (!props)
		return;

	num_props = create_queue_props(props, DBG_MAX_PROPS, &id, &conf);

	qos_dbg_props(cmd_buf, qdev, &cbs, props, num_props, &conf);
	kfree(props);
}

void syncq_help(struct seq_file *f)
{
	struct pp_qos_queue_conf conf;
	const char *name = "add sync queue";
	const char *format =
		"echo queue=[logical_id] [field]=[value]... > syncq";
	struct dbg_prop *props;
	u16 num_props;
	u32 id = PP_QOS_INVALID_ID;

	props = kmalloc_array(DBG_MAX_PROPS, sizeof(struct dbg_prop),
			      GFP_KERNEL);
	if (!props)
		return;

	num_props = create_queue_props(props, DBG_MAX_PROPS, &id, &conf);
	qos_dbg_props_help(f, name, format, props, num_props);
	kfree(props);
}

PP_DEFINE_DEBUGFS(syncq_add, syncq_help, syncq);

static int fw_logger_get(void *data, u64 *val)
{
	struct pp_qos_dev *qdev = pp_qos_dev_open((unsigned long)data);

	if (!capable(CAP_SYS_PACCT))
		return -EPERM;

	print_fw_log(qdev);

	return 0;
}

static int fw_logger_set(void *data, u64 val)
{
	struct pp_qos_dev *qdev = pp_qos_dev_open((unsigned long)data);
	s32    ret;

	if (!capable(CAP_SYS_PACCT))
		return -EPERM;

	QOS_LOG_INFO("fw_logger_set setting new fw logger level %u\n",
		     (u32)val);

	switch (val) {
	case 0:
		create_init_logger_cmd(qdev, UC_LOGGER_LEVEL_INFO,
				       UC_LOGGER_MODE_NONE);
		break;
	case UC_LOGGER_LEVEL_FATAL:
	case UC_LOGGER_LEVEL_ERROR:
	case UC_LOGGER_LEVEL_WARNING:
	case UC_LOGGER_LEVEL_INFO:
	case UC_LOGGER_LEVEL_DEBUG:
	case UC_LOGGER_LEVEL_DUMP_REGS:
		create_init_logger_cmd(qdev, (int)val,
				       UC_LOGGER_MODE_WRITE_HOST_MEM);
		break;
	default:
		QOS_LOG_INFO("Not supported fw logger level");
		QOS_LOG_INFO("Optional levels:\n");
		QOS_LOG_INFO("None: 0\n");
		QOS_LOG_INFO("Fatal: %d\n", UC_LOGGER_LEVEL_FATAL);
		QOS_LOG_INFO("Error: %d\n", UC_LOGGER_LEVEL_ERROR);
		QOS_LOG_INFO("Warning: %d\n", UC_LOGGER_LEVEL_WARNING);
		QOS_LOG_INFO("Info: %d\n", UC_LOGGER_LEVEL_INFO);
		QOS_LOG_INFO("Debug: %d\n", UC_LOGGER_LEVEL_DEBUG);
		QOS_LOG_INFO("Register Dump: %d\n", UC_LOGGER_LEVEL_DUMP_REGS);
		break;
	}

	update_cmd_id(&qdev->drvcmds);
	ret = transmit_cmds(qdev);
	if (ret)
		QOS_LOG_ERR("transmit_cmds failed (%d)\n", ret);

	return 0;
}

DEFINE_SIMPLE_ATTRIBUTE(dbg_fw_logger_fops, fw_logger_get,
			fw_logger_set, "%llu\n");

/**
 * @brief dump node information
 */
void pp_qos_dbg_node_show(struct seq_file *s)
{
	struct pp_qos_dev *qdev;
	const char *typename;
	u32 phy;
	u32 id;
	u32 i;
	static const char *const types[] = {"Port", "Sched", "Queue",
		"Unknown"};
	static const char *const yesno[] = {"No", "Yes"};
	s32 rc;
	struct pp_qos_node_info info = {0, };

	qdev = pp_qos_dev_open((unsigned long)s->private);
	QOS_LOG_INFO("node_show called\n");
	if (qdev) {
		if (unlikely(!qos_device_ready(qdev))) {
			seq_puts(s, "Device is not ready !!!!\n");
			return;
		}

		id = g_node;
		phy = get_phy_from_id(qdev->mapping, id);
		if (unlikely(!QOS_PHY_VALID(phy))) {
			seq_printf(s, "Invalid id %u\n", id);
			return;
		}
		rc = qos_get_node_info(qdev, id, &info);
		if (rc) {
			seq_printf(s, "Could not get info for node %u!!!!\n",
				   id);
			return;
		}

		if (info.type >=  PPV4_QOS_NODE_TYPE_PORT &&
		    info.type <= PPV4_QOS_NODE_TYPE_QUEUE)
			typename = types[info.type];
		else
			typename = types[3];

		seq_printf(s, "%u(%u) - %s: internal node(%s)\n",
			   id, phy,
			   typename,
			   yesno[!!info.is_internal]);

		if (info.preds[0].phy != PPV4_QOS_INVALID) {
			seq_printf(s, "%u(%u)", id, phy);
			for (i = 0; i < 6; ++i) {
				if (info.preds[i].phy == PPV4_QOS_INVALID)
					break;
				seq_printf(s, " ==> %u(%u)",
					   info.preds[i].id,
					   info.preds[i].phy);
			}
			seq_puts(s, "\n");
		}

		if (info.children[0].phy != PPV4_QOS_INVALID) {
			for (i = 0; i < 8; ++i) {
				if (info.children[i].phy == PPV4_QOS_INVALID)
					break;
				seq_printf(s, "%u(%u) ",
					   info.children[i].id,
					   info.children[i].phy);
			}
			seq_puts(s, "\n");
		}

		if (info.type == PPV4_QOS_NODE_TYPE_QUEUE)  {
			seq_printf(s, "Physical queue: %u\n",
				   info.queue_physical_id);
			seq_printf(s, "Port: %u\n", info.port);
		}

		seq_printf(s, "Bandwidth: %u Kbps\n", info.bw_limit);
	}
}

PP_DEFINE_DEBUGFS(node_show, pp_qos_dbg_node_show, NULL);

/**
 * @brief dump node statistics
 */
void pp_qos_dbg_stat_show(struct seq_file *s)
{
	struct pp_qos_dev *qdev;
	struct pp_qos_queue_stat qstat;
	struct pp_qos_port_stat pstat;
	struct qos_node *node;
	u32 phy;
	u32 id;

	qdev = pp_qos_dev_open((unsigned long)s->private);
	QOS_LOG_INFO("node_show called\n");
	if (unlikely(!qdev)) {
		seq_puts(s, "qdev Null\n");
		return;
	}

	if (unlikely(!qos_device_ready(qdev))) {
		seq_puts(s, "Device is not ready\n");
		return;
	}

	id = g_node;
	phy = get_phy_from_id(qdev->mapping, id);
	if (unlikely(!QOS_PHY_VALID(phy))) {
		seq_printf(s, "Invalid id %u\n", id);
		return;
	}

	node = get_node_from_phy(qdev->nodes, phy);

	if (unlikely(!node_used(node))) {
		seq_printf(s, "Node %u is unused\n", id);
		return;
	}

	seq_printf(s, "%u(%u) - ", id, phy);
	if (node_queue(node)) {
		seq_puts(s, "Queue\n");
		memset(&qstat, 0, sizeof(qstat));
		if (pp_qos_queue_stat_get(qdev, id, &qstat) == 0) {
			seq_printf(s, "queue_packets_occupancy:%u\n",
				   qstat.queue_packets_occupancy);

			seq_printf(s, "queue_bytes_occupancy:%u\n",
				   qstat.queue_bytes_occupancy);

			seq_printf(s, "total_packets_accepted:%u\n",
				   qstat.total_packets_accepted);

			seq_printf(s, "total_packets_dropped:%u\n",
				   qstat.total_packets_dropped);

			seq_printf(s, "total_packets_red_dropped:%u\n",
				   qstat.total_packets_red_dropped);

			seq_printf(s, "total_bytes_accepted:%llu\n",
				   qstat.total_bytes_accepted);

			seq_printf(s, "total_bytes_dropped:%llu\n",
				   qstat.total_bytes_dropped);
		} else {
			seq_puts(s, "Could not obtained statistics\n");
		}
	} else if (node_port(node)) {
		seq_puts(s, "Port\n");
		memset(&pstat, 0, sizeof(pstat));
		if (pp_qos_port_stat_get(qdev, id, &pstat) == 0) {
			seq_printf(s, "total_green_bytes in port's queues:%u\n",
				   pstat.total_green_bytes);

			seq_printf(s, "total_yellow_bytes in port's queues:%u\n",
				   pstat.total_yellow_bytes);

			seq_printf(s, "back pressure status:%u\n",
				   pstat.debug_back_pressure_status);

			seq_printf(s, "Actual packet credit:%u\n",
				   pstat.debug_actual_packet_credit);

			seq_printf(s, "Actual byte credit:%u\n",
				   pstat.debug_actual_byte_credit);
		} else {
			seq_puts(s, "Could not obtained statistics\n");
		}
	} else {
		seq_puts(s, "Node is not a queue or port, no statistics\n");
	}
}

PP_DEFINE_DEBUGFS(stats, pp_qos_dbg_stat_show, NULL);

/**
 * @brief dump queue statistics
 */
void pp_qos_dbg_queues_stats_show(struct seq_file *s)
{
	struct pp_qos_dev *qdev;

	qdev = pp_qos_dev_open((unsigned long)s->private);
	if (unlikely(!qdev)) {
		seq_puts(s, "qdev Null\n");
		return;
	}

	if (unlikely(!qos_device_ready(qdev))) {
		seq_puts(s, "Device is not ready\n");
		return;
	}

	pp_stats_show_seq(s, sizeof(struct queue_stats), NUM_OF_NODES,
			  qos_queues_stats_get, qos_queues_stats_show, qdev);
}

/**
 * @brief reset queues statistics
 */
void pp_qos_dbg_queues_stats_reset(char *cmd_buf, void *data)
{
	struct pp_qos_dev *qdev;
	u32 is_reset;

	if ((sscanf(cmd_buf, "%u ", &is_reset) == 1) && (is_reset == 0)) {
		qdev = pp_qos_dev_open(PP_QOS_INSTANCE_ID);
		if (unlikely(ptr_is_null(qdev)))
			return;
		pp_qos_stats_reset(qdev);
		return;
	}

	QOS_LOG_INFO("\nqos_queues_stats help:\n");
	QOS_LOG_INFO("cat pp/qos_queues_stats      : Display statistics\n");
	QOS_LOG_INFO("echo 0 > pp/qos_queues_stats : Reset statistics\n");
}

PP_DEFINE_DEBUGFS(queues_stats, pp_qos_dbg_queues_stats_show,
		  pp_qos_dbg_queues_stats_reset);

/**
 * @brief dump queues pps statistics
 */
void pp_qos_dbg_queues_pps_show(struct seq_file *s)
{
	struct pp_qos_dev *qdev;

	qdev = pp_qos_dev_open((unsigned long)s->private);
	if (unlikely(!qdev)) {
		seq_puts(s, "qdev Null\n");
		return;
	}

	if (unlikely(!qos_device_ready(qdev))) {
		seq_puts(s, "Device is not ready\n");
		return;
	}

	pp_pps_show_seq(s, sizeof(struct queue_stats), NUM_OF_NODES,
			qos_queues_stats_get, qos_queues_stats_diff,
			qos_queues_stats_show, qdev);
}

PP_DEFINE_DEBUGFS(queues_pps, pp_qos_dbg_queues_pps_show, NULL);

/**
 * @brief dump complete qos tree
 */
void pp_qos_dbg_tree_show(struct seq_file *s)
{
	struct pp_qos_dev *qdev;

	qdev = pp_qos_dev_open((unsigned long)s->private);
	QOS_LOG_INFO("tree_show called\n");
	if (unlikely(!qdev)) {
		seq_puts(s, "qdev Null\n");
		return;
	}

	if (unlikely(!qos_device_ready(qdev))) {
		seq_puts(s, "Device is not ready\n");
		return;
	}

	qos_dbg_tree_show_locked(qdev, true, s);
}

PP_DEFINE_DEBUGFS(tree, pp_qos_dbg_tree_show, NULL);

/**
 * @brief dump complete qos tree
 */
void pp_qos_dbg_full_tree_show(struct seq_file *s)
{
	struct pp_qos_dev *qdev;

	qdev = pp_qos_dev_open((unsigned long)s->private);
	QOS_LOG_INFO("tree_show called\n");
	if (unlikely(!qdev)) {
		seq_puts(s, "qdev Null\n");
		return;
	}

	if (unlikely(!qos_device_ready(qdev))) {
		seq_puts(s, "Device is not ready\n");
		return;
	}

	qos_dbg_tree_show_locked(qdev, false, s);
}

PP_DEFINE_DEBUGFS(full_tree, pp_qos_dbg_full_tree_show, NULL);

void pp_qos_dbg_tree_remove_help(struct seq_file *s)
{
	seq_puts(s, "echo 0 > destroy_tree\n");
}

void pp_qos_dbg_tree_remove(char *cmd_buf, void *data)
{
	u32 node_id, total_occupancy;
	struct qos_node *node;
	struct pp_qos_dev *qdev;
	struct pp_qos_queue_stat stats;
	u32 idx;
	s32 node_phy;

	qdev = pp_qos_dev_open((unsigned long)data);
	if (unlikely(!qos_device_ready(qdev))) {
		QOS_LOG_ERR("Device is not ready\n");
		return;
	}

	/* Iterate through all queue nodes */
	for (node_phy = NUM_OF_NODES - 1; node_phy >= 0; --node_phy) {
		node = get_node_from_phy(qdev->nodes, node_phy);
		if (node_queue(node) && !node_syncq(node)) {
			node_id = get_id_from_phy(qdev->mapping, node_phy);
			pp_qos_queue_block(qdev, node_id);
		}
	}

	memset(&stats, 0, sizeof(stats));

	/* Read stats */
	for (idx = 0; idx < 100; ++idx) {
		total_occupancy = 0;
		for (node_phy = 0; node_phy < NUM_OF_NODES; ++node_phy) {
			node = get_node_from_phy(qdev->nodes, node_phy);

			if (!node_queue(node))
				continue;

			node_id = get_id_from_phy(qdev->mapping, node_phy);
			pp_qos_queue_stat_get(qdev, node_id, &stats);
			total_occupancy += stats.queue_packets_occupancy;
			if (stats.queue_packets_occupancy) {
				QOS_LOG_INFO("Queue %u has %u packet occ\n",
					     node_id,
					     stats.queue_packets_occupancy);
			}
		}

		if (total_occupancy != 0)
			qos_dbg_qm_stat_show(qdev, NULL);
		else
			break;

		mdelay(10);
	}

	if (total_occupancy != 0) {
		QOS_LOG_ERR("Cannot remove tree while occupancy=%u\n",
			    total_occupancy);
		qos_dbg_qm_stat_show(qdev, NULL);
		return;
	}

	delete_all_syncq(qdev);

	/* Iterate through all queue nodes */
	for (node_phy = NUM_OF_NODES - 1; node_phy >= 0; --node_phy) {
		node = get_node_from_phy(qdev->nodes, node_phy);
		if (node_queue(node)) {
			node_id = get_id_from_phy(qdev->mapping, node_phy);
			pp_qos_queue_remove(qdev, node_id);
		}
	}

	/* Iterate through all port nodes */
	for (node_phy = 0; node_phy < NUM_OF_NODES; ++node_phy) {
		node = get_node_from_phy(qdev->nodes, node_phy);
		if (node_port(node)) {
			node_id = get_id_from_phy(qdev->mapping, node_phy);
			pp_qos_port_remove(qdev, node_id);
		}
	}

	qos_pools_clean(qdev);
	qos_pools_init(qdev, qdev->init_params.max_ports,
		       qdev->init_params.max_queues,
		       qdev->init_params.reserved_queues,
		       qdev->init_params.num_syncqs);
}

PP_DEFINE_DEBUGFS(destroy_tree, pp_qos_dbg_tree_remove_help,
		  pp_qos_dbg_tree_remove);

/**
 * @brief dump complete qos tree
 */
void pp_qos_dbg_qm_stat_show(struct seq_file *s)
{
	struct pp_qos_dev *qdev;

	qdev = pp_qos_dev_open((unsigned long)s->private);
	if (unlikely(!qdev)) {
		seq_puts(s, "qdev Null\n");
		return;
	}

	if (unlikely(!qos_device_ready(qdev))) {
		seq_puts(s, "Device is not ready\n");
		return;
	}

	qos_dbg_qm_stat_show(qdev, s);
}

/**
 * @brief reset queues statistics
 */
void pp_qos_dbg_qm_stat_reset(char *cmd_buf, void *data)
{
	struct pp_qos_dev *qdev;
	u32 is_reset;

	if ((sscanf(cmd_buf, "%u ", &is_reset) == 1) && (is_reset == 0)) {
		qdev = pp_qos_dev_open(PP_QOS_INSTANCE_ID);
		if (unlikely(ptr_is_null(qdev)))
			return;
		pp_qos_stats_reset(qdev);
		return;
	}

	QOS_LOG_INFO("\npp/qos0/qstat help:\n");
	QOS_LOG_INFO("cat pp/qos0/qstat      : Display statistics\n");
	QOS_LOG_INFO("echo 0 > pp/qos0/qstat : Reset statistics\n");
}

PP_DEFINE_DEBUGFS(qm_stats, pp_qos_dbg_qm_stat_show, pp_qos_dbg_qm_stat_reset);

/**
 * @brief dump general qos info
 */
void pp_qos_dbg_gen_show(struct seq_file *s)
{
	struct qos_drv_stats stats;
	struct pp_qos_dev *qdev;
	u32 major, minor, build;

	qdev = pp_qos_dev_open((unsigned long)s->private);
	if (unlikely(ptr_is_null(qdev)))
		return;

	if (unlikely(!qos_device_ready(qdev))) {
		seq_puts(s, "Device is not ready\n");
		return;
	}

	if (unlikely(qos_drv_stats_get(qdev, &stats))) {
		seq_puts(s, "failed to get driver stats\n");
		return;
	}

	seq_printf(s, "Driver version: %s\n", PPV4_QOS_DRV_VER);

	if (pp_qos_get_fw_version(qdev, &major, &minor, &build) == 0) {
		seq_printf(s, "FW version:\tmajor(%u) minor(%u) build(%u)\n",
			   major, minor, build);
	} else {
		seq_puts(s, "Could not obtain FW version\n");
	}

	seq_printf(s, "Used nodes:\t%u\n", stats.active_nodes);
	seq_printf(s, "Ports:\t\t%u\n", stats.active_ports);
	seq_printf(s, "Scheds:\t\t%u\n", stats.active_sched);
	seq_printf(s, "Queues:\t\t%u\n", stats.active_queues);
	seq_printf(s, "Reserved Queues:\t%u\n", stats.active_reserved_queues);
	seq_printf(s, "Internals:\t%u\n", stats.active_internals);

	seq_printf(s, "Command queue watermark:\n\tcmd_q: %u\n\tpend_q: %u\n",
		   (u32)cmd_queue_watermark_get(qdev->drvcmds.cmdq),
		   (u32)cmd_queue_watermark_get(qdev->drvcmds.pendq));
	seq_printf(s, "WRED resources:\t%u\n",
		   qdev->init_params.wred_total_avail_resources);
}

PP_DEFINE_DEBUGFS(gen_show, pp_qos_dbg_gen_show, NULL);

#ifndef PP_QOS_DISABLE_CMDQ
/**
 * @brief send custom command to FW
 */
void dbg_cmd_write(char *cmd_buf, void *data)
{
	struct pp_qos_dev *qdev;
	unsigned long dst;
	u32 cmd_idx;
	u32 cmd_type;
	u32 cmd_flags;
	u32 cmd_len;
	u32 cmd_params[5];

	qdev = pp_qos_dev_open((unsigned long)data);
	dst = qdev->init_params.fwcom.cmdbuf;

	if (sscanf(cmd_buf, "%u %u %u",
		   &cmd_type, &cmd_flags, &cmd_len) != 3) {
		QOS_LOG_ERR("sscanf err - minimum 3 params\n");
		return;
	}

	PP_REG_WR32_INC(dst, cmd_type);
	QOS_LOG_INFO("Wrote 0x%x\n", cmd_type);
	PP_REG_WR32_INC(dst, cmd_flags);
	QOS_LOG_INFO("Wrote 0x%x\n", cmd_flags);
	PP_REG_WR32_INC(dst, cmd_len);
	QOS_LOG_INFO("Wrote 0x%x\n", cmd_len);

	switch (cmd_len - 3) {
	case 0:
	break;
	case 1:
		if (sscanf(cmd_buf, "%u %u %u %u",
			   &cmd_type,
			   &cmd_flags,
			   &cmd_len,
			   &cmd_params[0]) != 4) {
			QOS_LOG_ERR("sscanf err\n");
			return;
		}
	break;
	case 2:
		if (sscanf(cmd_buf, "%u %u %u %u %u",
			   &cmd_type,
			   &cmd_flags,
			   &cmd_len,
			   &cmd_params[0],
			   &cmd_params[1]) != 5) {
			QOS_LOG_ERR("sscanf err\n");
			return;
		}
	break;
	case 3:
		if (sscanf(cmd_buf, "%u %u %u %u %u %u",
			   &cmd_type,
			   &cmd_flags,
			   &cmd_len,
			   &cmd_params[0],
			   &cmd_params[1],
			   &cmd_params[2]) != 6) {
			QOS_LOG_ERR("sscanf err\n");
			return;
		}
	break;
	case 4:
		if (sscanf(cmd_buf, "%u %u %u %u %u %u %u",
			   &cmd_type,
			   &cmd_flags,
			   &cmd_len,
			   &cmd_params[0],
			   &cmd_params[1],
			   &cmd_params[2],
			   &cmd_params[3]) != 7) {
			QOS_LOG_ERR("sscanf err\n");
			return;
		}
	break;
	case 5:
		if (sscanf(cmd_buf, "%u %u %u %u %u %u %u %u",
			   &cmd_type,
			   &cmd_flags,
			   &cmd_len,
			   &cmd_params[0],
			   &cmd_params[1],
			   &cmd_params[2],
			   &cmd_params[3],
			   &cmd_params[4]) != 8) {
			QOS_LOG_ERR("sscanf err\n");
			return;
		}
	break;
	default:
		QOS_LOG_INFO("len %d is not supported (range is 3 - 8)\n",
			     cmd_len);
		return;
	}

	if ((cmd_len - 3) > 5) {
		QOS_LOG_INFO("len %d is not supported (range is 3 - 8)\n",
			     cmd_len);
		return;
	}

	for (cmd_idx = 0; cmd_idx < cmd_len - 3; cmd_idx++) {
		PP_REG_WR32_INC(dst, cmd_params[cmd_idx]);
		QOS_LOG_INFO("Wrote 0x%x\n", cmd_params[cmd_idx]);
	}

	signal_uc(qdev);
}

PP_DEFINE_DEBUGFS(cmd, NULL, dbg_cmd_write);
#endif

/**
 * @brief advanced debug features
 */
void ctrl_set(char *cmd_buf, void *data)
{
	s32    ret;
	u32    choise;
	struct pp_qos_dev *qdev;

	qdev = pp_qos_dev_open((unsigned long)data);
	if (unlikely(!qdev)) {
		QOS_LOG_ERR("Error: qdev NULL\n");
		return;
	}

	if (kstrtou32(cmd_buf, 10, &choise)) {
		QOS_LOG_ERR("kstrtou32 err\n");
		goto ctrl_set_done;
	}

	switch (choise) {
	case 1:
		QOS_LOG_INFO("loading FW\n");
		ret = load_firmware(qdev, FIRMWARE_FILE);
		if (ret) {
			QOS_LOG_ERR("load_firmware failed (%d)\n", ret);
			goto ctrl_set_done;
		}
	break;

	case 2:
#ifndef PP_QOS_DISABLE_CMDQ
		QOS_LOG_INFO("Sending init logger cmd (Info)\n");
		create_init_logger_cmd(qdev,
				       UC_LOGGER_LEVEL_INFO,
				       UC_LOGGER_MODE_WRITE_HOST_MEM);
		update_cmd_id(&qdev->drvcmds);
		ret = transmit_cmds(qdev);
		if (ret)
			QOS_LOG_ERR("transmit_cmds failed (%d)\n", ret);
#endif
		break;
	case 3:
#ifndef PP_QOS_DISABLE_CMDQ
		QOS_LOG_INFO("Sending init logger cmd (Debug)\n");
		create_init_logger_cmd(qdev,
				       UC_LOGGER_LEVEL_DEBUG,
				       UC_LOGGER_MODE_WRITE_HOST_MEM);
		update_cmd_id(&qdev->drvcmds);
		ret = transmit_cmds(qdev);
		if (ret)
			QOS_LOG_ERR("transmit_cmds failed (%d)\n", ret);
#endif
		break;
	case 4:
#ifndef PP_QOS_DISABLE_CMDQ
		QOS_LOG_INFO("Sending init logger cmd (Off)\n");
		create_init_logger_cmd(qdev,
				       UC_LOGGER_LEVEL_INFO,
				       UC_LOGGER_MODE_NONE);
		update_cmd_id(&qdev->drvcmds);
		ret = transmit_cmds(qdev);
		if (ret)
			QOS_LOG_ERR("transmit_cmds failed (%d)\n", ret);
#endif
		break;
	case 5:
		QOS_LOG_INFO("printing logger\n");
		print_fw_log(qdev);
	break;

	case 6:
		QOS_LOG_INFO("Sending init qos cmd\n");
		create_init_qos_cmd(qdev);
		update_cmd_id(&qdev->drvcmds);
		ret = transmit_cmds(qdev);
		if (ret)
			QOS_LOG_ERR("transmit_cmds failed (%d)\n", ret);
	break;

	case 7:
		check_sync_with_fw(qdev, true);
	break;

	default:
		QOS_LOG_INFO("unknown option\n");
		break;
	}

ctrl_set_done:
	return;
}

void ctrl_set_help(struct seq_file *f)
{
	seq_puts(f, "1: Load Firmware\n");
	seq_puts(f, "2: Set FW Logger (Info)\n");
	seq_puts(f, "3: Set FW Logger (Debug)\n");
	seq_puts(f, "4: Set FW Logger (Off)\n");
	seq_puts(f, "5: Print FW Logger\n");
	seq_puts(f, "6: Init QoS\n");
	seq_puts(f, "7: Check Sync With FW\n");
}

PP_DEFINE_DEBUGFS(ctrl, ctrl_set_help, ctrl_set);

/**
 * @brief read hw table entry (will be dumped to logger)
 */
void read_table_entry(char *cmd_buf, void *data)
{
	struct pp_qos_dev *qdev;
	u32 physical_id;
	u32 table_id;
	s32 ret;

	qdev = pp_qos_dev_open((unsigned long)data);
	if (unlikely(!qdev)) {
		QOS_LOG_ERR("Error: qdev NULL\n");
		return;
	}

	if (sscanf(cmd_buf, "%u %u",
		   &physical_id, &table_id) != 2) {
		QOS_LOG_ERR("sscanf err\n");
		goto read_table_entry_done;
	}

#ifndef PP_QOS_DISABLE_CMDQ
		QOS_LOG_INFO("Sending read table entry (%u) from phy (%u)\n",
			     table_id, physical_id);
		create_get_table_entry_cmd(qdev, physical_id,
					   qdev->hwconf.fw_stats_ddr_phys,
					   table_id);
		update_cmd_id(&qdev->drvcmds);
		ret = transmit_cmds(qdev);
		if (ret)
			QOS_LOG_ERR("transmit_cmds failed (%d)\n", ret);
#endif

read_table_entry_done:
	return;
}

void read_table_entry_help(struct seq_file *f)
{
	seq_puts(f, "<physical node/rlm> <table id>\n");
}

PP_DEFINE_DEBUGFS(table_entry, read_table_entry_help, read_table_entry);

/**
 * @brief push descriptor to qos queue
 */
void queue_push(char *cmd_buf, void *data)
{
	struct pp_qos_dev *qdev;
	unsigned long addr;
	u32 logical_id, size, color, policy, pool, port, data_off;

	qdev = pp_qos_dev_open((unsigned long)data);
	if (unlikely(!qdev)) {
		QOS_LOG_ERR("Error: qdev NULL\n");
		return;
	}

	if (sscanf(cmd_buf, "%u %u %u %u %u %u %u %lx",
		   &logical_id,
		   &size,
		   &pool,
		   &policy,
		   &color,
		   &port,
		   &data_off,
		   &addr) != 8) {
		QOS_LOG_ERR("sscanf err\n");
		goto queue_push_done;
	}

	qos_descriptor_push(qdev, logical_id, size, pool, policy, color, port,
			    data_off, addr);

queue_push_done:
	return;
}

void queue_push_help(struct seq_file *f)
{
	seq_puts(f, "<q logic id> <size> <pool> <policy>"
		    " <color> <tx port> <data off> <addr>\n");
}

PP_DEFINE_DEBUGFS(queue_push, queue_push_help, queue_push);

/**
 * @brief pop descriptor from qos queue
 */
void queue_pop(char *cmd_buf, void *data)
{
	struct pp_qos_dev *qdev;
#ifdef CONFIG_PPV4_LGM
	struct pp_qos_desc desc;
	struct pp_qos_hw_desc hw_desc;
#endif
	u32 descriptor[4];
	u32 logical_id;

	qdev = pp_qos_dev_open((unsigned long)data);
	if (unlikely(!qdev)) {
		QOS_LOG_ERR("Error: qdev NULL\n");
		return;
	}

	if (sscanf(cmd_buf, "%u",
		   &logical_id) != 1) {
		QOS_LOG_ERR("sscanf err\n");
		return;
	}

	qos_descriptor_pop(qdev, logical_id, descriptor);

	QOS_LOG_INFO("descriptor[0]: %#x\n", descriptor[0]);
	QOS_LOG_INFO("descriptor[1]: %#x\n", descriptor[1]);
	QOS_LOG_INFO("descriptor[2]: %#x\n", descriptor[2]);
	QOS_LOG_INFO("descriptor[3]: %#x\n", descriptor[3]);

#ifdef CONFIG_PPV4_LGM
	hw_desc.word[0] = descriptor[0];
	hw_desc.word[1] = descriptor[1];
	hw_desc.word[2] = descriptor[2];
	hw_desc.word[3] = descriptor[3];
	pp_qos_desc_decode(&desc, &hw_desc);
	pp_qos_desc_dump(&desc);
#endif
}

void queue_pop_help(struct seq_file *f)
{
	seq_puts(f, "<queue logical id>\n");
}

PP_DEFINE_DEBUGFS(queue_pop, queue_pop_help, queue_pop);

/**
 * @brief mcmda copy
 */
void mcdma_copy(char *cmd_buf, void *data)
{
	struct pp_qos_dev *qdev;
	u32 src, dst, size;
	s32 ret;

	qdev = pp_qos_dev_open((unsigned long)data);
	if (unlikely(!qdev)) {
		QOS_LOG_ERR("Error: qdev NULL\n");
		return;
	}

	if (sscanf(cmd_buf, "%x %x %u",
		   &src, &dst, &size) != 3) {
		QOS_LOG_ERR("sscanf err\n");
		goto mcdma_copy_done;
	}

	create_mcdma_copy_cmd(qdev, src, dst, size);
	update_cmd_id(&qdev->drvcmds);
	ret = transmit_cmds(qdev);
	if (unlikely(ret))
		QOS_LOG_ERR("transmit_cmds failed (%d)\n", ret);

mcdma_copy_done:
	return;
}

void mcdma_copy_help(struct seq_file *f)
{
	seq_puts(f, "<src address> <dst address> <size>"
		    "(addresses should be ddr contiguous)\n");
}

PP_DEFINE_DEBUGFS(mcdma_copy, mcdma_copy_help, mcdma_copy);

/**
 * @brief wsp helper, set enhanced WSP parameters
 */
void ewsp_helper(char *cmd_buf, void *data)
{
	struct pp_qos_dev *qdev;
	u32 enable, timeout, threshold;
	s32 ret;

	qdev = pp_qos_dev_open((unsigned long)data);
	if (unlikely(!qdev)) {
		QOS_LOG_ERR("Error: qdev NULL\n");
		return;
	}

	if (sscanf(cmd_buf, "%u %u %u",
		   &enable, &timeout, &threshold) != 3) {
		QOS_LOG_ERR("sscanf err\n");
		goto ewsp_helper_done;
	}

	if (timeout > 255) {
		QOS_LOG_ERR("Timeout value is up to 255 uS\n");
		goto ewsp_helper_done;
	}

	qdev->ewsp_cfg.enable = enable;
	qdev->ewsp_cfg.timeout_microseconds = timeout;
	qdev->ewsp_cfg.byte_threshold = threshold;

	create_set_wsp_helper_cmd(qdev, enable, timeout, threshold);
	update_cmd_id(&qdev->drvcmds);
	ret = transmit_cmds(qdev);
	if (unlikely(ret))
		QOS_LOG_ERR("transmit_cmds failed (%d)\n", ret);

ewsp_helper_done:
	return;
}

void ewsp_helper_help(struct seq_file *f)
{
	seq_puts(f, "<enable> <microseconds timeout> <byte threshold>\n");
}

PP_DEFINE_DEBUGFS(ewsp_helper, ewsp_helper_help, ewsp_helper);

/**
 * @brief wsp helper reset, reset Enhanced WSP statistics
 */
void ewsp_helper_reset(char *cmd_buf, void *data)
{
	struct pp_qos_dev *qdev;
	struct wsp_stats_t stats;
	s32 ret;

	qdev = pp_qos_dev_open((unsigned long)data);
	if (unlikely(!qdev)) {
		QOS_LOG_ERR("Error: qdev NULL\n");
		return;
	}

	create_get_wsp_helper_stats_cmd(qdev, qdev->hwconf.fw_stats_ddr_phys,
					true, &stats);
	update_cmd_id(&qdev->drvcmds);
	ret = transmit_cmds(qdev);
	if (unlikely(ret))
		QOS_LOG_ERR("transmit_cmds failed (%d)\n", ret);

	return;
}

/**
 * @brief wsp helper stats, dump Enhanced WSP statistics
 */
void ewsp_helper_stats(struct seq_file *f)
{
	struct pp_qos_dev *qdev;
	struct wsp_stats_t stats;
	s32 ret;

	qdev = pp_qos_dev_open(PP_QOS_INSTANCE_ID);
	if (unlikely(!qdev)) {
		QOS_LOG_ERR("Error: qdev NULL\n");
		return;
	}

	create_get_wsp_helper_stats_cmd(qdev, qdev->hwconf.fw_stats_ddr_phys,
					false, &stats);
	update_cmd_id(&qdev->drvcmds);
	ret = transmit_cmds(qdev);
	if (unlikely(ret))
		QOS_LOG_ERR("transmit_cmds failed (%d)\n", ret);

	seq_puts(f, "Wsp helper cfg:\n");
	seq_puts(f, "===========================\n");
	seq_printf(f, "enable: %u\n", qdev->ewsp_cfg.enable);
	seq_printf(f, "timeout_microseconds: %u\n",
		   qdev->ewsp_cfg.timeout_microseconds);
	seq_printf(f, "byte_threshold: %u\n", qdev->ewsp_cfg.byte_threshold);
	seq_printf(f, "num_queues: %u\n", qdev->ewsp_cfg.num_queues);

	seq_puts(f, "\n--------------------------\n");

	seq_puts(f, "Wsp helper stats:\n");
	seq_puts(f, "===========================\n");
	seq_printf(f, "num_timeouts: %u\n", stats.num_timeouts);
	seq_printf(f, "num_toggles: %u\n", stats.num_toggles_wm);
	seq_printf(f, "num_iterations: %u\n", stats.num_iterations_wm);
	seq_printf(f, "num_queues: %u\n", stats.num_queues);
}

PP_DEFINE_DEBUGFS(ewsp_helper_stats, ewsp_helper_stats, ewsp_helper_reset);


/**
 * @brief Set Qos modules registers write logs on/off
 */
static s32 mod_reg_log_en_set(void *data, u64 val)
{
	struct pp_qos_dev *qdev;
	s32 ret;

	qdev = pp_qos_dev_open((unsigned long)data);
	if (ptr_is_null(qdev))
		return -EINVAL;

	create_mod_log_bmap_set_cmd(qdev, (u32)val);
	update_cmd_id(&qdev->drvcmds);
	ret = transmit_cmds(qdev);
	if (unlikely(ret))
		QOS_LOG_ERR("transmit_cmds failed (%d)\n", ret);

	return ret;
}

/**
 * @brief Get Qos modules registers write logs status
 */
static s32 mod_reg_log_en_get(void *data, u64 *val)
{
	u32 bmap;
	struct pp_qos_dev *qdev;
	s32 ret;

	qdev = pp_qos_dev_open((unsigned long)data);
	if (ptr_is_null(qdev))
		return -EINVAL;

	create_mod_log_bmap_get_cmd(qdev, &bmap);
	update_cmd_id(&qdev->drvcmds);
	ret = transmit_cmds(qdev);
	if (unlikely(ret))
		QOS_LOG_ERR("transmit_cmds failed (%d)\n", ret);
	*val = (u64)bmap;

	return ret;
}

PP_DEFINE_DBGFS_ATT_FMT(mod_reg_log_en, mod_reg_log_en_get, mod_reg_log_en_set,
			"%llx\n");

/**
 * @brief add sf
 */
void sf_add(char *cmd_buf, void *data)
{
	struct pp_qos_dev *qdev;
	struct pp_qos_aqm_sf_config sf_cfg;
	u32 sf_id;
	s32 ret;

	qdev = pp_qos_dev_open((unsigned long)data);
	if (unlikely(!qdev)) {
		QOS_LOG_ERR("Error: qdev NULL\n");
		return;
	}

	if (sscanf(cmd_buf, "%u %u",
		   &sf_id, &sf_cfg.num_queues) != 2) {
		QOS_LOG_ERR("sscanf err - 2 params\n");
		goto sf_add_done;
	}

	if (unlikely(sf_id >= PP_QOS_AQM_MAX_SERVICE_FLOWS)) {
		QOS_LOG_ERR("SF %u is invalid (max %u)\n",
			    sf_id, PP_QOS_AQM_MAX_SERVICE_FLOWS);
		goto sf_add_done;
	}

	if (!sf_cfg.num_queues || sf_cfg.num_queues > 3) {
		QOS_LOG_ERR("Num queues 1 - 3\n");
		goto sf_add_done;
	}

	switch (sf_cfg.num_queues) {
	case 1:
		if (sscanf(cmd_buf, "%u %u %u",
			   &sf_id,
			   &sf_cfg.num_queues,
			   &sf_cfg.queue_id[0]) != 3) {
			QOS_LOG_ERR("sscanf err - 3 params\n");
			goto sf_add_done;
		}
	break;

	case 2:
		if (sscanf(cmd_buf, "%u %u %u %u",
			   &sf_id,
			   &sf_cfg.num_queues,
			   &sf_cfg.queue_id[0],
			   &sf_cfg.queue_id[1]) != 4) {
			QOS_LOG_ERR("sscanf err - 4 params\n");
			goto sf_add_done;
		}
	break;

	case 3:
		if (sscanf(cmd_buf, "%u %u %u %u %u",
			   &sf_id,
			   &sf_cfg.num_queues,
			   &sf_cfg.queue_id[0],
			   &sf_cfg.queue_id[1],
			   &sf_cfg.queue_id[2]) != 5) {
			QOS_LOG_ERR("sscanf err - 5 params\n");
			goto sf_add_done;
		}
	break;

	default:
		QOS_LOG_ERR("%u Queues is not supported for deubg\n",
			    sf_cfg.num_queues);
		goto sf_add_done;
	break;
	}

	QOS_LOG_INFO("setting sf %u with %u queues\n",
		     sf_id, sf_cfg.num_queues);

	sf_cfg.latency_target_ms = AQM_LATENCY_TARGET;
	sf_cfg.peak_rate = AQM_PEAK_RATE;
	sf_cfg.msr = AQM_MSR;
	sf_cfg.buffer_size = AQM_BUFFER_SIZE;

	ret = pp_qos_aqm_sf_set(qdev, sf_id, &sf_cfg);
	if (unlikely(ret)) {
		QOS_LOG_ERR("pp_qos_aqm_sf_set %d failed\n", sf_id);
		goto sf_add_done;
	}

sf_add_done:
	return;
}

void sf_add_help(struct seq_file *f)
{
	seq_puts(f, "<sf id> <num queues> <queue_id>"
		    " <queue_id (optional)> <queue_id (optional)>\n");
}

PP_DEFINE_DEBUGFS(sf_add, sf_add_help, sf_add);

/**
 * @brief del sf
 */
void sf_del(char *cmd_buf, void *data)
{
	struct pp_qos_dev *qdev;
	u32 sf_id;
	s32 ret;

	qdev = pp_qos_dev_open((unsigned long)data);
	if (unlikely(!qdev)) {
		QOS_LOG_ERR("Error: qdev NULL\n");
		return;
	}

	if (kstrtou32(cmd_buf, 10, &sf_id)) {
		QOS_LOG_ERR("kstrtou32 err\n");
		goto sf_del_done;
	}

	if (unlikely(sf_id >= PP_QOS_AQM_MAX_SERVICE_FLOWS)) {
		QOS_LOG_ERR("SF %u is invalid (max %u)\n",
			    sf_id, PP_QOS_AQM_MAX_SERVICE_FLOWS);
		goto sf_del_done;
	}

	QOS_LOG_INFO("Removing sf %u\n", sf_id);

	ret = pp_qos_aqm_sf_remove(qdev, sf_id);
	if (unlikely(ret)) {
		QOS_LOG_ERR("pp_qos_aqm_sf_remove %d failed\n", sf_id);
		goto sf_del_done;
	}

sf_del_done:
	return;
}

void sf_del_help(struct seq_file *f)
{
	seq_puts(f, "<sf id>\n");
}

PP_DEFINE_DEBUGFS(sf_del, sf_del_help, sf_del);

/**
 * @brief configure CoDel
 */
void codel_cfg(char *cmd_buf, void *data)
{
	struct pp_qos_dev *qdev;
	struct pp_qos_codel_cfg cfg;
	s32 ret;

	qdev = pp_qos_dev_open((unsigned long)data);
	if (unlikely(!qdev)) {
		QOS_LOG_ERR("Error: qdev NULL\n");
		return;
	}

	if (unlikely(!qos_device_ready(qdev))) {
		QOS_LOG_ERR("Device is not ready\n");
		return;
	}

	if (sscanf(cmd_buf, "%u %u %u",
		   &cfg.consecutive_drop_limit,
		   &cfg.target_delay_msec,
		   &cfg.interval_time_msec) != 3) {
		QOS_LOG_ERR("sscanf err - 3 params\n");
		goto done;
	}

	ret = pp_qos_codel_cfg_set(qdev, &cfg);
	if (unlikely(ret)) {
		QOS_LOG_ERR("pp_qos_codel_cfg_set failed\n");
		goto done;
	}

done:
	return;
}

void codel_cfg_help(struct seq_file *f)
{
	seq_puts(f, "<consecutive_drop_limit> <target_delay_msec>"
		    " <interval_time_msec>\n");
}

PP_DEFINE_DEBUGFS(codel_cfg, codel_cfg_help, codel_cfg);

/**
 * @brief configure CoDel
 */
void codel_stats(char *cmd_buf, void *data)
{
	const struct qos_node *node;
	struct pp_qos_dev *qdev;
	struct pp_qos_codel_stat cstats;
	u32 node_id, node_phy;

	qdev = pp_qos_dev_open((unsigned long)data);
	if (unlikely(!qdev)) {
		QOS_LOG_ERR("Error: qdev NULL\n");
		return;
	}

	if (unlikely(!qos_device_ready(qdev))) {
		QOS_LOG_ERR("Device is not ready\n");
		return;
	}

	memset(&cstats, 0, sizeof(cstats));
	if (sscanf(cmd_buf, "%u %d", &node_id, &cstats.reset) != 2) {
		QOS_LOG_ERR("sscanf err - 2 params\n");
		goto done;
	}

	node_phy = get_phy_from_id(qdev->mapping, node_id);
	if (unlikely(!QOS_PHY_VALID(node_phy))) {
		QOS_LOG_ERR("Invalid id %u\n", node_id);
		return;
	}

	node = get_node_from_phy(qdev->nodes, node_phy);
	if (!node_queue(node)) {
		QOS_LOG_ERR("not a queue node, node %u\n", node_id);
		return;
	}

	if (unlikely(pp_qos_codel_queue_stat_get(qdev, node_id, &cstats))) {
		QOS_LOG_ERR("codel_stat_get(%u) failed\n", node_id);
		return;
	}

	QOS_LOG_INFO(" Dropped packets            : %u\n",
		     cstats.packets_dropped);
	QOS_LOG_INFO(" Dropped bytes              : %u\n",
		     cstats.bytes_dropped);
	QOS_LOG_INFO(" Max sojourn time   [mSec]  : %u\n",
		     cstats.max_sojourn_time);
	QOS_LOG_INFO(" Min sojourn time   [mSec]  : %u\n",
		     cstats.min_sojourn_time);
	QOS_LOG_INFO(" Total sojourn time [mSec]  : %u\n",
		     cstats.total_packets);
	QOS_LOG_INFO(" Max packet size            : %u\n",
		     cstats.max_packet_size);
	QOS_LOG_INFO(" Total packets              : %u\n",
		     cstats.total_packets);

done:
	return;
}

void codel_stats_help(struct seq_file *f)
{
	seq_puts(f, "<queue node id> <1=reset stats>\n");
}

PP_DEFINE_DEBUGFS(codel_stats, codel_stats_help, codel_stats);

static int __gnode_set(void *data, u64 val)
{
	g_node = (u16)val;

	return 0;
}

static int __gnode_get(void *data, u64 *val)
{
	*val = (u64)g_node;

	return 0;
}

PP_DEFINE_DBGFS_ATT(gnode, __gnode_get, __gnode_set);

static int __g_raw_config_set(void *data, u64 val)
{
	g_raw_config = (u8)val;

	return 0;
}

static int __g_raw_config_get(void *data, u64 *val)
{
	*val = (u64)g_raw_config;

	return 0;
}

PP_DEFINE_DBGFS_ATT(raw_config, __g_raw_config_get, __g_raw_config_set);

void pp_qos_dbg_quanta_get(struct seq_file *s)
{
	struct pp_qos_dev *qdev;
	u32 quanta;

	qdev = pp_qos_dev_open((unsigned long)s->private);
	if (unlikely(ptr_is_null(qdev)))
		return;

	if (unlikely(!qos_device_ready(qdev))) {
		seq_puts(s, "Device is not ready\n");
		return;
	}

	if (pp_qos_get_quanta(qdev, &quanta) == 0)
		seq_printf(s, "quanta: %u\n", quanta);
	else
		seq_puts(s, "pp_qos_get_quanta failed\n");
}

PP_DEFINE_DEBUGFS(quanta, pp_qos_dbg_quanta_get, NULL);

void pp_qos_dbg_system_stats_show(struct seq_file *s)
{
	struct pp_qos_dev *qdev;
	struct system_stats_s sys_stats;
	s32 ret = 0;

	qdev = pp_qos_dev_open((unsigned long)s->private);
	if (unlikely(ptr_is_null(qdev)))
		return;

	if (unlikely(!qos_device_ready(qdev))) {
		seq_puts(s, "Device is not ready\n");
		return;
	}

	create_get_sys_info_cmd(qdev, qdev->hwconf.fw_stats_ddr_phys,
				&sys_stats, false);
	update_cmd_id(&qdev->drvcmds);
	ret = transmit_cmds(qdev);
	if (ret) {
		QOS_LOG_ERR("transmit_cmds failed (%d)\n", ret);
		return;
	}

	seq_puts(s, "System stats:\n");
	seq_puts(s, "=============\n");
	seq_printf(s, "tscd_infinite_loop_error_occurred: %u\n",
		   sys_stats.tscd_infinite_loop_error_occurred);
	seq_printf(s, "tscd_bwl_update_error_occurred: %u\n",
		   sys_stats.tscd_bwl_update_error_occurred);
	seq_printf(s, "tscd_quanta: %u\n", sys_stats.tscd_quanta);

#if IS_ENABLED(CONFIG_PPV4_LGM)
	if (pp_silicon_step_get() == PP_SSTEP_B) {
		seq_printf(s, "wred_fake_pops: %u\n", sys_stats.wred_fake_pops);
		seq_printf(s, "wred_pop_underflow_count: %u\n",
			   sys_stats.wred_pop_underflow_count);
		seq_printf(s, "wred_pop_underflow_sum: %u\n",
			   sys_stats.wred_pop_underflow_sum);
		seq_printf(s, "wred_last_push_address_high: %#x\n",
			   sys_stats.wred_last_push_address_high);
		seq_printf(s, "wred_last_push_address_low: %#x\n",
			   sys_stats.wred_last_push_address_low);
		seq_printf(s, "wred_last_push_drop: %u\n",
			   sys_stats.wred_last_push_drop);
		seq_printf(s, "wred_last_push_info_q_id: %u\n",
			   sys_stats.wred_last_push_info_q_id);
		seq_printf(s, "wred_last_push_info_color: %u\n",
			   sys_stats.wred_last_push_info_color);
		seq_printf(s, "wred_last_push_info_pkt_size: %u\n",
			   sys_stats.wred_last_push_info_pkt_size);
		seq_printf(s, "wred_last_pop_info_q_id: %u\n",
			   sys_stats.wred_last_pop_info_q_id);
		seq_printf(s, "wred_last_pop_info_fake: %u\n",
			   sys_stats.wred_last_pop_info_fake);
		seq_printf(s, "wred_last_pop_info_color: %u\n",
			   sys_stats.wred_last_pop_info_color);
		seq_printf(s, "wred_last_pop_info_pkt_size: %u\n",
			   sys_stats.wred_last_pop_info_pkt_size);
	}
#endif

	seq_printf(s, "txmgr_bp_status_ports_0_31: %#x\n",
		   sys_stats.txmgr_bp_status_ports_0_31);
	seq_printf(s, "txmgr_bp_status_ports_32_63: %#x\n",
		   sys_stats.txmgr_bp_status_ports_32_63);
	seq_printf(s, "txmgr_bp_status_ports_64_95: %#x\n",
		   sys_stats.txmgr_bp_status_ports_64_95);
	seq_printf(s, "txmgr_bp_status_ports_96_127: %#x\n",
		   sys_stats.txmgr_bp_status_ports_96_127);
#if IS_ENABLED(CONFIG_PPV4_LGM)
	seq_printf(s, "txmgr_bp_status_ports_128_159: %#x\n",
		   sys_stats.txmgr_bp_status_ports_128_159);
	seq_printf(s, "txmgr_bp_status_ports_160_191: %#x\n",
		   sys_stats.txmgr_bp_status_ports_160_191);
	seq_printf(s, "txmgr_bp_status_ports_192_223: %#x\n",
		   sys_stats.txmgr_bp_status_ports_192_223);
	seq_printf(s, "txmgr_bp_status_ports_224_255: %#x\n",
		   sys_stats.txmgr_bp_status_ports_224_255);
#endif
}

PP_DEFINE_DEBUGFS(system_stats, pp_qos_dbg_system_stats_show, NULL);

static struct debugfs_file qos_debugfs_files[] = {
	{"node", &PP_DEBUGFS_FOPS(gnode)},
	{"raw_config", &PP_DEBUGFS_FOPS(raw_config)},
	{"nodeinfo", &PP_DEBUGFS_FOPS(node_show)},
	{"read_table_entry", &PP_DEBUGFS_FOPS(table_entry)},
	{"queues_stats", &PP_DEBUGFS_FOPS(queues_stats)},
	{"qstat", &PP_DEBUGFS_FOPS(qm_stats)},
	{"queues_pps", &PP_DEBUGFS_FOPS(queues_pps)},
	{"stat", &PP_DEBUGFS_FOPS(stats)},
	{"tree", &PP_DEBUGFS_FOPS(tree)},
	{"full_tree", &PP_DEBUGFS_FOPS(full_tree)},
	{"destroy_tree", &PP_DEBUGFS_FOPS(destroy_tree)},
	{"fw_logger", &dbg_fw_logger_fops},
	{"ctrl", &PP_DEBUGFS_FOPS(ctrl)},
	{"geninfo", &PP_DEBUGFS_FOPS(gen_show)},
	{"sf_add", &PP_DEBUGFS_FOPS(sf_add)},
	{"sf_del", &PP_DEBUGFS_FOPS(sf_del)},
	{"codel_cfg", &PP_DEBUGFS_FOPS(codel_cfg)},
	{"codel_stats", &PP_DEBUGFS_FOPS(codel_stats)},
	{"mcdma_copy", &PP_DEBUGFS_FOPS(mcdma_copy)},
	{"queue", &PP_DEBUGFS_FOPS(queue_set)},
	{"syncq", &PP_DEBUGFS_FOPS(syncq_add)},
	{"sched", &PP_DEBUGFS_FOPS(sched_set)},
	{"shared_grp_add", &PP_DEBUGFS_FOPS(shared_grp_add)},
	{"shared_grp_rem", &PP_DEBUGFS_FOPS(shared_grp_del)},
	{"port", &PP_DEBUGFS_FOPS(port_set)},
	{"push_desc", &PP_DEBUGFS_FOPS(queue_push)},
	{"pop_desc", &PP_DEBUGFS_FOPS(queue_pop)},
	{"allocate", &PP_DEBUGFS_FOPS(allocate)},
	{"remove", &PP_DEBUGFS_FOPS(remove)},
	{"flush", &PP_DEBUGFS_FOPS(flush)},
	{"quanta", &PP_DEBUGFS_FOPS(quanta)},
	{"mod_reg_log_en", &PP_DEBUGFS_FOPS(mod_reg_log_en)},
	{"ewsp_helper", &PP_DEBUGFS_FOPS(ewsp_helper)},
	{"ewsp_helper_stats", &PP_DEBUGFS_FOPS(ewsp_helper_stats)},
	{"system_stats", &PP_DEBUGFS_FOPS(system_stats)},
#ifndef PP_QOS_DISABLE_CMDQ
	{"cmd", &PP_DEBUGFS_FOPS(cmd)},
#endif
};

static struct debugfs_file qos_root_debugfs_files[] = {
	{"qos_queues_stats", &PP_DEBUGFS_FOPS(queues_stats)},
	{"qos_queues_pps", &PP_DEBUGFS_FOPS(queues_pps)},
	{"qos_tree", &PP_DEBUGFS_FOPS(tree)},
};

#define MAX_DIR_NAME 11
s32 qos_dbg_module_init(struct pp_qos_dev *qdev)
{
	char dirname[MAX_DIR_NAME];
	s32  ret = 0;

	QOS_LOG_DEBUG("\n");

	snprintf(dirname, MAX_DIR_NAME, "qos%d", qdev->id);
	ret = pp_debugfs_create(qdev->init_params.dbgfs, dirname, &qdev->dbgfs,
				qos_debugfs_files,
				ARRAY_SIZE(qos_debugfs_files),
				(void *)(unsigned long)qdev->id);
	if (unlikely(ret))
		return ret;

	/* create subset in the root directory */
	ret = pp_debugfs_create(qdev->init_params.dbgfs, NULL, NULL,
				qos_root_debugfs_files,
				ARRAY_SIZE(qos_root_debugfs_files),
				(void *)(unsigned long)qdev->id);

	return ret;
}

void qos_dbg_module_clean(struct pp_qos_dev *qdev)
{
	QOS_LOG_DEBUG("qdev->dbgfs %p\n", qdev->dbgfs);
	debugfs_remove_recursive(qdev->dbgfs);
}
