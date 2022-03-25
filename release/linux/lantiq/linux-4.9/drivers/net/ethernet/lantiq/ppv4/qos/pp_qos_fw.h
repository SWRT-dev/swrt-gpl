/*
 * GPL LICENSE SUMMARY
 *
 *  Copyright(c) 2017 Intel Corporation.
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
#ifndef PP_QOS_FW_H
#define PP_QOS_FW_H

#include "pp_qos_utils.h"
#include "pp_qos_common.h"
#include "pp_qos_uc_wrapper.h"
#include "pp_qos_kernel.h"

struct ppv4_qos_fw {
	size_t size;
	const uint8_t *data;
};

int load_firmware(struct pp_qos_dev *qdev, const char *name);
void print_fw_log(struct platform_device *pdev);

#ifndef PP_QOS_DISABLE_CMDQ
void create_move_cmd(struct pp_qos_dev *qdev, uint16_t dst, uint16_t src,
		     uint16_t dst_port);
void create_set_port_cmd(struct pp_qos_dev *qdev,
			 const struct pp_qos_port_conf *conf, unsigned int phy,
			 uint32_t modified);
void create_set_queue_cmd(struct pp_qos_dev *qdev,
			  const struct pp_qos_queue_conf *conf,
			  unsigned int phy,
			  unsigned int parent,
			  unsigned int rlm, uint32_t modified);
void create_set_sched_cmd(struct pp_qos_dev *qdev,
			  const struct pp_qos_sched_conf *conf,
			  unsigned int phy,
			  unsigned int parent, uint32_t modified);
void create_remove_node_cmd(struct pp_qos_dev *qdev, enum node_type type,
			    unsigned int phy, unsigned int data);
void create_parent_change_cmd(struct pp_qos_dev *qdev, unsigned int phy);
void create_update_preds_cmd(struct pp_qos_dev *qdev, unsigned int phy,
		bool queue_port_changed);
void create_init_qos_cmd(struct pp_qos_dev *qdev);
void enqueue_cmds(struct pp_qos_dev *qdev);
void check_completion(struct pp_qos_dev *qdev);
void create_get_queue_stats_cmd(
		struct pp_qos_dev *qdev,
		unsigned int phy,
		unsigned int rlm,
		unsigned int addr,
		struct pp_qos_queue_stat *qstat);
void create_get_port_stats_cmd(
		struct pp_qos_dev *qdev,
		unsigned int phy,
		unsigned int addr,
		struct pp_qos_port_stat *pstat);
int init_fwdata_internals(struct pp_qos_dev *qdev);
void clean_fwdata_internals(struct pp_qos_dev *qdev);
void create_init_logger_cmd(struct pp_qos_dev *qdev, int level);
void create_add_shared_group_cmd(struct pp_qos_dev *qdev,
		unsigned int id,
		unsigned int limit);
void create_remove_shared_group_cmd(struct pp_qos_dev *qdev,
		unsigned int id);

void create_set_shared_group_cmd(struct pp_qos_dev *qdev,
		unsigned int id,
		unsigned int limit);

void create_push_desc_cmd(struct pp_qos_dev *qdev, unsigned int queue,
		unsigned int size, unsigned int color, unsigned int addr);

void create_get_node_info_cmd(
		struct pp_qos_dev *qdev,
		unsigned int phy,
		unsigned int addr,
		struct pp_qos_node_info *info);

void create_get_table_entry_cmd(struct pp_qos_dev *qdev, u32 phy,
				u32 addr, u32 table_type);

void create_flush_queue_cmd(struct pp_qos_dev *qdev, unsigned int rlm);
int do_load_firmware(
		struct pp_qos_dev *qdev,
		const struct ppv4_qos_fw *fw,
		struct pp_qos_drv_data *pdata);

void add_suspend_port(struct pp_qos_dev *qdev, unsigned int port);
void signal_uc(struct pp_qos_dev *qdev);
void create_get_sys_info_cmd(struct pp_qos_dev *qdev,
			     unsigned int addr,
			     struct qos_hw_info *sys_info);
#elif defined(PRINT_CREATE_CMD)
#define create_move_cmd(qdev, dst, src, dst_port)\
	QOS_LOG_DEBUG("MOVE: %u ==> %u\n", src, dst)
#define create_set_port_cmd(qdev, conf, phy, modified)\
	QOS_LOG_DEBUG("SET PORT: %u\n", phy)
#define create_set_sched_cmd(qdev, conf, phy, parent, modified)\
	QOS_LOG_DEBUG("SET SCH: %u\n", phy)
#define create_set_queue_cmd(qdev, conf, phy, parent, rlm, modified)\
	QOS_LOG_DEBUG("SET QUEUE: %u\n", phy)
#define create_remove_node_cmd(qdev, type, phy, data)\
	QOS_LOG_DEBUG("REMOVE: %u(%u)\n", phy, type)
#define create_parent_change_cmd(qdev, phy)\
	QOS_LOG_DEBUG("PARENT_CHANGE: %u\n", phy)
#define create_update_preds_cmd(qdev, phy, queue_port_changed)\
	QOS_LOG_DEBUG("UPDATE_PREDS: %u\n", phy)
#define create_get_queue_stats_cmd(qdev, phy, rlm, addr, qstat)
#define create_init_qos_cmd(qdev)
#define enqueue_cmds(qdev)
#define check_completion(qdev)
#define init_fwdata_internals(qdev) 0
#define clean_fwdata_internals(qdev)
#define create_init_logger_cmd(qdev)
#define create_add_shared_group_cmd(qdev, id, limit)
#define create_set_shared_group_cmd(qdev, id, limit)
#define create_remove_shared_group_cmd(qdev, id)
#define create_get_queue_stats_cmd(qdev, phy, rlm, addr, qstat)
#define create_get_port_stats_cmd(qdev, phy, addr, pstat)
#define create_get_node_info_cmd(qdev, phy, addr, info)
#define create_get_table_entry_cmd(qdev, phy, addr, table_type)
#define create_push_desc_cmd(qdev, queue, size, color, addr)
#define create_get_sys_info_cmd(qdev, addr, sys_info)
#define add_suspend_port(qdev, port)
#define create_flush_queue_cmd(qdev, rlm)
#else
#define create_move_cmd(qdev, dst, src, dst_port)
#define create_set_port_cmd(qdev, conf, phy, modified)
#define create_set_sched_cmd(qdev, conf, phy, parent, modified)
#define create_set_queue_cmd(qdev, conf, phy, parent, rlm, modified)
#define create_remove_node_cmd(qdev, type, phy, data)
#define create_parent_change_cmd(qdev, phy)
#define create_update_preds_cmd(qdev, phy, queue_port_changed)
#define create_get_queue_stats_cmd(qdev, phy, rlm, addr, qstat)
#define create_init_qos_cmd(qdev)
#define enqueue_cmds(qdev)
#define check_completion(qdev)
#define init_fwdata_internals(qdev) 0
#define clean_fwdata_internals(qdev)
#define create_init_logger_cmd(qdev)
#define create_add_shared_group_cmd(qdev, id, limit)
#define create_set_shared_group_cmd(qdev, id, limit)
#define create_remove_shared_group_cmd(qdev, id)
#define create_get_queue_stats_cmd(qdev, phy, rlm, addr, qstat)
#define create_get_port_stats_cmd(qdev, phy, addr, pstat)
#define create_get_node_info_cmd(qdev, phy, addr, info)
#define create_get_table_entry_cmd(qdev, phy, addr, table_type)
#define create_push_desc_cmd(qdev, queue, size, color, addr)
#define create_flush_queue_cmd(qdev, rlm)
#define create_get_sys_info_cmd(qdev, addr, sys_info)
#define add_suspend_port(qdev, port)
#endif
#endif
