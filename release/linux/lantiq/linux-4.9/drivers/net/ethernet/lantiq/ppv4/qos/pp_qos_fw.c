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
#include "pp_qos_uc_wrapper.h"
#include "pp_qos_fw.h"
#include "pp_qos_elf.h"
#include "lantiq.h"
#include <linux/dma-mapping.h>
#include <linux/platform_device.h>
#include <linux/slab.h>

#ifdef __LP64__
#define GET_ADDRESS_HIGH(addr) ((((uintptr_t)(addr)) >> 32) & 0xFFFFFFFF)
#else
#define GET_ADDRESS_HIGH(addr) (0)
#endif

#ifndef PP_QOS_DISABLE_CMDQ

#define CMD_FLAGS_WRAP_SUSPEND_RESUME 1
#define CMD_FLAGS_WRAP_PARENT_SUSPEND_RESUME 2
#define CMD_FLAGS_POST_PROCESS	4


#define FW_CMDS(OP) \
	OP(CMD_TYPE_INIT_LOGGER)		\
	OP(CMD_TYPE_INIT_QOS)			\
	OP(CMD_TYPE_MOVE)			\
	OP(CMD_TYPE_ADD_PORT)			\
	OP(CMD_TYPE_SET_PORT)			\
	OP(CMD_TYPE_REMOVE_PORT)		\
	OP(CMD_TYPE_ADD_SCHED)			\
	OP(CMD_TYPE_SET_SCHED)			\
	OP(CMD_TYPE_REMOVE_SCHED)		\
	OP(CMD_TYPE_ADD_QUEUE)			\
	OP(CMD_TYPE_SET_QUEUE)			\
	OP(CMD_TYPE_REMOVE_QUEUE)		\
	OP(CMD_TYPE_UPDATE_PREDECESSORS)	\
	OP(CMD_TYPE_PARENT_CHANGE)		\
	OP(CMD_TYPE_REMOVE_NODE)		\
	OP(CMD_TYPE_GET_QUEUE_STATS)		\
	OP(CMD_TYPE_GET_PORT_STATS)		\
	OP(CMD_TYPE_ADD_SHARED_GROUP)		\
	OP(CMD_TYPE_PUSH_DESC)			\
	OP(CMD_TYPE_GET_NODE_INFO)		\
	OP(CMD_TYPE_READ_TABLE_ENTRY)		\
	OP(CMD_TYPE_REMOVE_SHARED_GROUP)	\
	OP(CMD_TYPE_SET_SHARED_GROUP)		\
	OP(CMD_TYPE_FLUSH_QUEUE)		\
	OP(CMD_TYPE_GET_SYSTEM_INFO)		\
	OP(CMD_TYPE_INTERNAL)

enum cmd_type {
	FW_CMDS(GEN_ENUM)
};

static const char *const cmd_str[] = {
	FW_CMDS(GEN_STR)
};

struct ppv4_qos_fw_hdr {
	uint32_t major;
	uint32_t minor;
	uint32_t build;
};

#define FW_OK_SIGN			(0xCAFECAFEU)
#define QOS_ELF_MAX_SECS		(64)
#define FW_DCCM_START			(0xF0000000)
#define FW_CMD_BUFFER_DCCM_START	(FW_DCCM_START + 0x4000)

static void copy_section(void *_dst, const void *_src, unsigned int size)
{
	unsigned int i = size;
	const u8 *src;
	u8 *dst;

	src = (uint8_t *)_src;
	dst = (uint8_t *)_dst;

	for (i = size; i > 3; i -= 4) {
		*(uint32_t *)dst = cpu_to_le32(*(uint32_t *)src);
		src += 4;
		dst += 4;
	}

	/* Section size must be aligned to 2 */
	if (i == 1 || i == 3)
		QOS_LOG_ERR("Size %u not aligned to 2 (i=%u)\n", size, i);

	/* Copy last u16 if exists */
	if (i == 2)
		*(uint32_t *)dst = (uint32_t)cpu_to_le16(*(uint16_t *)src);
}

static void calc_elf_sections_sizes(struct elf_sec secs[], u16 num_secs,
				    u32 *total_sz, u32 *data_sz, u32 *stack_sz)
{
	u16 ind;

	for (ind = 0; ind < num_secs; ind++) {
		if (secs[ind].need_copy)
			*total_sz += secs[ind].size;

		if (!strncmp(secs[ind].name, ".data", 5))
			*data_sz = secs[ind].size;
		else if (!strncmp(secs[ind].name, ".stack", 6))
			*stack_sz = secs[ind].size;
	}
}

static void copy_sections(struct elf_sec secs[], u16 num_secs, void *virt_txt,
			  void *virt_data, dma_addr_t phys_txt,
			  dma_addr_t phys_data,
			  dma_addr_t phys_stack, void *ivt)
{
	u16 ind;
	void *src;
	void *dst;
	u32 ivt_tbl[3] = {0};

	for (ind = 0; ind < num_secs; ind++) {
		if (!secs[ind].need_copy)
			continue;

		src = secs[ind].data;

		if (!strncmp(secs[ind].name, ".data", 5)) {
			dst = virt_data;
		} else {
			dst = (void *)((unsigned long)(virt_txt) +
					(unsigned long)(secs[ind].addr));
		}

		if (!strncmp(secs[ind].name, ".vectors", 8)) {
			ivt_tbl[0] = le32_to_cpu(*(u32 *)src) + phys_txt;
			ivt_tbl[1] = phys_stack;
			ivt_tbl[2] = phys_data;
			/* Copy to QoS */
			memcpy(ivt, ivt_tbl, 12);
		}

		QOS_LOG_DEBUG("Section %s: COPY %llu bytes from %#lx[%#x %#x..] to %#lx\n",
			      secs[ind].name, secs[ind].size,
			      (unsigned long)src, *(u32 *)src,
			      *(u32 *)(src + 4), (unsigned long)dst);

		copy_section(dst, src, secs[ind].size);
	}
}

/*
 * This function loads the firmware.
 * The firmware binary is saved in ELF format
 * Text section is copyied to the DDR (Dynamically allocated).
 * The data and stack can be located in the DDR or in the DCCM. This is
 * configurable from the DTS.
 *
 * Firmware is little endian.
 *
 * When firmware runs it writes FW_OK_SIGN to FW_CMD_BUFFER_DCCM_START.
 */
int do_load_firmware(struct pp_qos_dev *qdev, const struct ppv4_qos_fw *fw,
		     struct pp_qos_drv_data *pdata)
{
	u32 val;
	u8 poll = 0;
	struct device *dev = &((struct platform_device *)qdev->pdev)->dev;
	u32 alloc_size;
	u32 align = 4;
	struct elf_sec *secs;
	u16 num_secs;
	u32 total_sz = 0;
	u32 data_sz = 0;
	u32 stack_sz = 0;
	void *virt_txt;
	void *virt_data;
	void *virt_stack;
	dma_addr_t phys_txt;
	dma_addr_t phys_data;
	dma_addr_t phys_stack; /* End of stack! */

	secs = kmalloc_array(QOS_ELF_MAX_SECS, sizeof(struct elf_sec),
			     GFP_KERNEL);
	if (!secs)
		return -ENOMEM;

	if (elf_parse(fw->data, fw->size, secs, QOS_ELF_MAX_SECS, &num_secs)) {
		QOS_LOG_ERR("ELF parse error!\n");
		kfree(secs);
		return -ENOEXEC;
	}

	calc_elf_sections_sizes(secs, num_secs, &total_sz, &data_sz, &stack_sz);

	/* No room for data and stack as defined from DTS */
	if ((data_sz + stack_sz) > pdata->fw_sec_data_stack.max_size) {
		QOS_LOG_ERR("Need to alloc %u bytes while dts limits to %u\n",
			    (data_sz + stack_sz),
			    pdata->fw_sec_data_stack.max_size);
		kfree(secs);
		return -ENOMEM;
	}

	QOS_LOG_DEBUG("do_load_firmware: fw_data_stack_off %d, %d, %d\n",
		      pdata->fw_sec_data_stack.is_in_dccm,
		      pdata->fw_sec_data_stack.dccm_offset,
		      pdata->fw_sec_data_stack.max_size);

	/* Is data stack sections in DCCM */
	if (pdata->fw_sec_data_stack.is_in_dccm) {
		alloc_size = ALIGN(total_sz, align);
	} else { /* DDR */
		alloc_size = ALIGN(total_sz, align) +
			     ALIGN(data_sz, align) +
			     ALIGN(stack_sz, align);
	}

	virt_txt = dmam_alloc_coherent(dev, alloc_size, &phys_txt, GFP_KERNEL);
	if (!virt_txt) {
		QOS_LOG_ERR("Could not allocate %u bytes for fw\n", alloc_size);
		kfree(secs);
		return -ENOMEM;
	}

	/* Is data stack sections in DCCM */
	if (pdata->fw_sec_data_stack.is_in_dccm) {
		phys_data = FW_DCCM_START +
			    pdata->fw_sec_data_stack.dccm_offset;
		virt_data = pdata->dccm + pdata->fw_sec_data_stack.dccm_offset;
	} else { /* DDR */
		phys_data = phys_txt + ALIGN(total_sz, align);
		virt_data = virt_txt + ALIGN(total_sz, align);
	}

	phys_stack = phys_data + ALIGN(data_sz, align) +
		     ALIGN(stack_sz, align) - 4;
	virt_stack = virt_data + ALIGN(data_sz, align) +
		     ALIGN(stack_sz, align) - 4;

	QOS_LOG_DEBUG("Text %#x [%#lx] (%d), Data %#x (%d), Stack %#x (%d)\n",
		      phys_txt,
		      (unsigned long)(virt_txt),
		      total_sz,
		      phys_data, data_sz,
		      phys_stack, stack_sz);

	copy_sections(secs, num_secs, virt_txt, virt_data, phys_txt,
		      phys_data, phys_stack, pdata->ivt);

	kfree(secs);

	wake_uc((void *)pdata);
	QOS_LOG_DEBUG("waked fw\n");

	do {
		poll++;
		qos_sleep(10);
		val = *((uint32_t *)(qdev->fwcom.cmdbuf));
		if (poll == 3) {
			QOS_LOG_ERR("FW OK value is 0x%08X, got 0x%08X\n",
				    FW_OK_SIGN, val);
			return -ENODEV;
		}
	} while (val != FW_OK_SIGN);

	val = *((uint32_t *)(qdev->fwcom.cmdbuf+4));

	qdev->fwver.major = val & 0xF;
	qdev->fwver.minor = (val & 0xFF00) >> 8;
	qdev->fwver.build = (val & 0xFF0000) >> 16;
	QOS_LOG_INFO("PPV4 QoS FW [ver %u.%u.%u] is running (%d polls) soc rev %d :)\n",
		     qdev->fwver.major, qdev->fwver.minor,
		     qdev->fwver.build, poll, ltq_get_soc_rev());

	*((uint32_t *)(qdev->fwcom.cmdbuf)) = 0;
	return 0;
}

static void swap_msg(char *msg, unsigned int len)
{
	unsigned int i;
	uint32_t *cur;

	cur = (uint32_t *)msg;

	for (i = 0; i < len; ++i)
		cur[i] = le32_to_cpu(cur[i]);
}

void print_fw_log(struct platform_device *pdev)
{
	char		msg[PPV4_QOS_LOGGER_MSG_SIZE];
	unsigned int	num;
	unsigned int	i;
	uint32_t	*addr;
	uint32_t	read;
	char		*cur;
	struct device	*dev;
	struct pp_qos_drv_data	*pdata;

	pdata = platform_get_drvdata(pdev);
	addr = (uint32_t *)(pdata->dbg.fw_logger_addr);
	num = qos_u32_from_uc(addr[0]);
	read = qos_u32_from_uc(addr[1]);
	dev = &pdev->dev;
	cur = (char *)(pdata->dbg.fw_logger_addr + 8);

	dev_info(dev, "addr is 0x%08X num of messages is %u, read index is %u",
		 (unsigned int)(uintptr_t)cur,
		 num,
		 read);

	for (i = read; i < num; ++i) {
		memcpy((char *)msg,
		       (char *)(cur + PPV4_QOS_LOGGER_MSG_SIZE * i),
		       PPV4_QOS_LOGGER_MSG_SIZE);
		swap_msg(msg, (PPV4_QOS_LOGGER_MSG_SIZE / sizeof(uint32_t)));
		msg[PPV4_QOS_LOGGER_MSG_SIZE - 1] = '\0';
		dev_info(dev, "[ARC]: %s\n", msg);
	}

	addr[1] = num;
}

/******************************************************************************/
/*                         Driver commands structures	                      */
/******************************************************************************/
struct cmd {
	unsigned int id;
	unsigned int fw_id;
	unsigned int flags;
	enum cmd_type  type;
	size_t len;
	uint32_t *pos;
};

struct cmd_internal {
	struct cmd base;
};

struct cmd_init_logger {
	struct cmd base;
	unsigned int	addr;
	int mode;
	int level;
	unsigned int num_of_msgs;
};

struct cmd_init_qos {
	struct cmd base;
	unsigned int qm_ddr_start;
	unsigned int qm_num_pages;
	unsigned int wred_total_avail_resources;
	unsigned int wred_prioritize_pop;
	unsigned int wred_avg_q_size_p;
	unsigned int wred_max_q_size;
	unsigned int num_of_ports;
	unsigned int qos_clock;
	unsigned int bwl_temp_buff;
	unsigned int sbwl_temp_buff;
};

struct cmd_move {
	struct cmd base;
	int      node_type;
	uint16_t src;
	uint16_t dst;
	unsigned int rlm;
	uint16_t dst_port;
	uint16_t preds[6];
};

struct cmd_remove_node {
	struct cmd base;
	unsigned int phy;
	unsigned int data; /* rlm in queue, otherwise irrlevant */
	unsigned int is_alias_slave; /* Relevant in Queue only */
};

struct cmd_update_preds {
	struct cmd base;
	int node_type;
	uint16_t preds[6];
	unsigned int phy;
	unsigned int rlm;
	unsigned int is_alias_slave;
	u32 queue_port;
	bool queue_port_changed;
};

struct port_properties {
	struct pp_qos_common_node_properties common;
	struct pp_qos_parent_node_properties parent;
	void *ring_addr;
	size_t ring_size;
	uint8_t  packet_credit_enable;
	unsigned int credit;
	int	     disable;
	unsigned int green_threshold;
	unsigned int yellow_threshold;
};

struct cmd_add_port {
	struct cmd base;
	unsigned int phy;
	struct port_properties prop;
};

struct cmd_set_port {
	struct cmd base;
	unsigned int phy;
	struct parent_node_properties parent_node_prop;
	struct port_properties prop;
	uint32_t modified;
};

struct sched_properties {
	struct pp_qos_common_node_properties common;
	struct pp_qos_parent_node_properties parent;
	struct pp_qos_child_node_properties  child;
};

struct cmd_add_sched {
	struct cmd base;
	unsigned int phy;
	unsigned int parent;
	uint16_t preds[6];
	struct sched_properties prop;
};

struct cmd_set_sched {
	struct cmd base;
	unsigned int phy;
	unsigned int parent;
	struct sched_properties prop;
	struct parent_node_properties parent_node_prop;
	uint32_t modified;
};

struct queue_properties {
	struct pp_qos_common_node_properties common;
	struct pp_qos_child_node_properties  child;
	uint8_t  blocked;
	uint8_t  wred_enable;
	uint8_t  fixed_drop_prob_enable;
	unsigned int max_burst;
	unsigned int queue_wred_min_avg_green;
	unsigned int queue_wred_max_avg_green;
	unsigned int queue_wred_slope_green;
	unsigned int queue_wred_min_avg_yellow;
	unsigned int queue_wred_max_avg_yellow;
	unsigned int queue_wred_slope_yellow;
	unsigned int queue_wred_min_guaranteed;
	unsigned int queue_wred_max_allowed;
	unsigned int queue_wred_fixed_drop_prob_green;
	unsigned int queue_wred_fixed_drop_prob_yellow;
	unsigned int      rlm;
};

struct cmd_add_queue {
	struct cmd base;
	unsigned int phy;
	unsigned int parent;
	unsigned int port;
	uint16_t preds[6];
	unsigned int is_alias_slave;
	struct queue_properties prop;
};

struct cmd_set_queue {
	struct cmd base;
	unsigned int phy;
	struct queue_properties prop;
	unsigned int is_alias_slave;
	uint32_t modified;
};

struct cmd_flush_queue {
	struct cmd base;
	unsigned int rlm;
};

struct cmd_parent_change {
	struct cmd base;
	unsigned int phy;
	int type;
	struct parent_node_properties parent_node_prop;
};

struct cmd_get_queue_stats {
	struct cmd base;
	unsigned int phy;
	unsigned int rlm;
	unsigned int addr;
	struct pp_qos_queue_stat *stat;
};

struct cmd_get_port_stats {
	struct cmd base;
	unsigned int phy;
	unsigned int addr;
	struct pp_qos_port_stat *stat;
};

struct cmd_push_desc {
	struct cmd base;
	unsigned int queue;
	unsigned int size;
	unsigned int color;
	unsigned int addr;
};

struct cmd_get_node_info {
	struct cmd base;
	unsigned int phy;
	unsigned int addr;
	struct pp_qos_node_info *info;
};

struct cmd_read_table_entry {
	struct cmd base;
	u32    phy;
	u32    table_type;
	u32    addr;
};

struct stub_cmd {
	struct cmd cmd;
	uint8_t data;
};

struct cmd_set_shared_group {
	struct cmd base;
	unsigned int id;
	unsigned int limit;
};

struct cmd_remove_shared_group {
	struct cmd base;
	unsigned int id;
};

struct cmd_get_system_info {
	struct cmd base;
	unsigned int addr;
	uint32_t *num_used;
	uint32_t *infinite_loop_error_occurred;
	uint32_t *bwl_update_error_occurred;
	uint32_t *quanta;
};

union driver_cmd {
	struct cmd	 cmd;
	struct stub_cmd  stub;
	struct cmd_init_logger init_logger;
	struct cmd_init_qos  init_qos;
	struct cmd_move  move;
	struct cmd_update_preds update_preds;
	struct cmd_remove_node remove_node;
	struct cmd_add_port add_port;
	struct cmd_set_port set_port;
	struct cmd_add_sched add_sched;
	struct cmd_set_sched set_sched;
	struct cmd_add_queue add_queue;
	struct cmd_set_queue set_queue;
	struct cmd_parent_change parent_change;
	struct cmd_get_queue_stats queue_stats;
	struct cmd_get_port_stats port_stats;
	struct cmd_set_shared_group set_shared_group;
	struct cmd_remove_shared_group remove_shared_group;
	struct cmd_push_desc	pushd;
	struct cmd_get_node_info node_info;
	struct cmd_read_table_entry    read_table_entry;
	struct cmd_flush_queue flush_queue;
	struct cmd_get_system_info sys_info;
	struct cmd_internal	internal;
};

/******************************************************************************/
/*                         Driver functions                                   */
/******************************************************************************/

/*
 * Following functions creates commands in driver fromat to be stored at
 * drivers queues before sending to firmware
 */

/*
 * Extract ancestors of node from driver's DB
 */
static void fill_preds(
		const struct pp_nodes *nodes,
		unsigned int phy,
		uint16_t *preds,
		size_t size)
{
	unsigned int i;
	const struct qos_node *node;

	i = 0;
	memset(preds, 0x0, size * sizeof(uint16_t));
	node = get_const_node_from_phy(nodes, phy);
	while (node_child(node) && (i < size)) {
		preds[i] = node->child_prop.parent_phy;
		node = get_const_node_from_phy(nodes,
				node->child_prop.parent_phy);
		i++;
	}
}

static void cmd_init(
		const struct pp_qos_dev *qdev,
		struct cmd *cmd,
		enum cmd_type type,
		size_t len,
		unsigned int flags)
{
	cmd->flags = flags;
	cmd->type = type;
	cmd->len = len;
	cmd->id = qdev->drvcmds.cmd_id;
	cmd->fw_id = qdev->drvcmds.cmd_fw_id;
	cmd->pos = NULL;
}

/* TODO - make less hard code */
void create_init_logger_cmd(struct pp_qos_dev *qdev, int level)
{
	struct cmd_init_logger cmd;

	cmd_init(qdev, &(cmd.base), CMD_TYPE_INIT_LOGGER, sizeof(cmd), 0);
	cmd.addr = qdev->hwconf.fw_logger_start;
	cmd.mode = UC_LOGGER_MODE_WRITE_HOST_MEM;
	cmd.level = level;
	cmd.num_of_msgs = PPV4_QOS_LOGGER_BUF_SIZE / PPV4_QOS_LOGGER_MSG_SIZE;
	QOS_LOG_DEBUG("cmd %u:%u CMD_TYPE_INIT_LOGGER\n",
			qdev->drvcmds.cmd_id,
			qdev->drvcmds.cmd_fw_id);
	cmd_queue_put(qdev->drvcmds.cmdq, (uint8_t *)&cmd, sizeof(cmd));
	qdev->drvcmds.cmd_fw_id++;
}

void create_init_qos_cmd(struct pp_qos_dev *qdev)
{
	struct cmd_init_qos cmd;

	if (PP_QOS_DEVICE_IS_ASSERT(qdev))
		return;

	cmd_init(qdev, &(cmd.base), CMD_TYPE_INIT_QOS, sizeof(cmd), 0);
	cmd.qm_ddr_start = qdev->hwconf.qm_ddr_start;
	cmd.qm_num_pages = qdev->hwconf.qm_num_pages;
	cmd.wred_total_avail_resources =
		qdev->hwconf.wred_total_avail_resources;
	cmd.wred_prioritize_pop = qdev->hwconf.wred_prioritize_pop;
	cmd.wred_avg_q_size_p = qdev->hwconf.wred_const_p;
	cmd.wred_max_q_size = qdev->hwconf.wred_max_q_size;
	cmd.num_of_ports = qdev->max_port + 1;
	cmd.qos_clock = qdev->hwconf.qos_clock;
	cmd.bwl_temp_buff = qdev->hwconf.bwl_ddr_phys;
	cmd.sbwl_temp_buff = qdev->hwconf.sbwl_ddr_phys;
	QOS_LOG_DEBUG("cmd %u:%u CMD_TYPE_INIT_QOS\n",
			qdev->drvcmds.cmd_id,
			qdev->drvcmds.cmd_fw_id);
	cmd_queue_put(qdev->drvcmds.cmdq, (uint8_t *)&cmd, sizeof(cmd));
	qdev->drvcmds.cmd_fw_id++;
}

void create_move_cmd(
		struct pp_qos_dev *qdev,
		uint16_t dst,
		uint16_t src,
		uint16_t dst_port)
{
	struct cmd_move cmd;
	const struct qos_node *node;

	if (PP_QOS_DEVICE_IS_ASSERT(qdev))
		return;

	/* If moving from tmp node, use the stored information */
	if (src == PP_QOS_TMP_NODE)
		node = get_const_node_from_phy(qdev->nodes, src);
	else
		node = get_const_node_from_phy(qdev->nodes, dst);

	cmd_init(qdev, &(cmd.base), CMD_TYPE_MOVE, sizeof(cmd), 0);
	cmd.src = src;
	cmd.dst = dst;
	cmd.dst_port = dst_port;
	cmd.node_type = node->type;
	if (node->type == TYPE_QUEUE)
		cmd.rlm = node->data.queue.rlm;
	else
		cmd.rlm = -1;

	fill_preds(qdev->nodes, dst, cmd.preds, 6);
	QOS_LOG_DEBUG("cmd %u:%u CMD_TYPE_MOVE %u-->%u type:%d, q:%d, port:%u",
			qdev->drvcmds.cmd_id,
			qdev->drvcmds.cmd_fw_id,
			src,
			dst,
			node->type,
			cmd.rlm,
			dst_port);

	cmd_queue_put(qdev->drvcmds.cmdq, (uint8_t *)&cmd, sizeof(cmd));
	add_suspend_port(qdev, dst_port);
	qdev->drvcmds.cmd_fw_id++;
}

void create_remove_node_cmd(
		struct pp_qos_dev *qdev,
		enum node_type ntype,
		unsigned int phy,
		unsigned int data)
{
	struct cmd_remove_node cmd = {0};
	enum cmd_type ctype;
	const struct qos_node *node;

	if (PP_QOS_DEVICE_IS_ASSERT(qdev))
		return;

	switch (ntype) {
	case TYPE_PORT:
		ctype = CMD_TYPE_REMOVE_PORT;
		break;
	case TYPE_SCHED:
		ctype = CMD_TYPE_REMOVE_SCHED;
		break;
	case TYPE_QUEUE:
	{
		ctype = CMD_TYPE_REMOVE_QUEUE;
		node = get_const_node_from_phy(qdev->nodes, phy);
		if (node->data.queue.alias_master_id != PP_QOS_INVALID_ID) {
			cmd.is_alias_slave = 1;
		}
	}
		break;
	case TYPE_UNKNOWN:
		QOS_ASSERT(0, "Unexpected unknown type\n");
		ctype = CMD_TYPE_REMOVE_NODE;
		break;
	default:
		QOS_LOG_ERR("illegal node type %d\n", ntype);
		return;
	}

	cmd_init(qdev, &(cmd.base), ctype, sizeof(cmd), 0);
	cmd.phy = phy;
	cmd.data = data;

	QOS_LOG_DEBUG("cmd %u:%u %s %u rlm %u alias %u\n",
			qdev->drvcmds.cmd_id,
			qdev->drvcmds.cmd_fw_id,
			cmd_str[ctype], phy, data, cmd.is_alias_slave);
	cmd_queue_put(qdev->drvcmds.cmdq, &cmd, sizeof(cmd));
	if (ctype != CMD_TYPE_REMOVE_PORT)
		add_suspend_port(qdev, get_port(qdev->nodes, phy));
	qdev->drvcmds.cmd_fw_id++;
}

void create_update_preds_cmd(struct pp_qos_dev *qdev, unsigned int phy,
		bool queue_port_changed)
{
	const struct qos_node *node;
	struct cmd_update_preds cmd;

	if (PP_QOS_DEVICE_IS_ASSERT(qdev))
		return;

	memset(&cmd, 0, sizeof(cmd));
	cmd_init(
			qdev,
			&(cmd.base),
			CMD_TYPE_UPDATE_PREDECESSORS,
			sizeof(cmd),
			0);
	cmd.phy = phy;
	fill_preds(qdev->nodes, phy, cmd.preds, 6);
	node = get_const_node_from_phy(qdev->nodes, phy);
	cmd.node_type = node->type;
	cmd.rlm = node->data.queue.rlm;
	cmd.is_alias_slave =
			(node->data.queue.alias_master_id == PP_QOS_INVALID_ID)
			? 0 : 1;
	cmd.queue_port = node->data.queue.port_phy;
	cmd.queue_port_changed = queue_port_changed;

	QOS_LOG_DEBUG("cmd %u:%u CMD_TYPE_UPDATE_PREDECESSORS %u:%u-->%u-->%u-->%u-->%u-->%u\n",
		      qdev->drvcmds.cmd_id,
		      qdev->drvcmds.cmd_fw_id,
		      phy,
		      cmd.preds[0], cmd.preds[1], cmd.preds[2],
		      cmd.preds[3], cmd.preds[4], cmd.preds[5]);

	cmd_queue_put(qdev->drvcmds.cmdq, &cmd, sizeof(cmd));
	qdev->drvcmds.cmd_fw_id++;
}

static void set_cmd_port_properties(
		struct port_properties *prop,
		const struct pp_qos_port_conf *conf)
{
	prop->common = conf->common_prop;
	prop->parent = conf->port_parent_prop;
	prop->packet_credit_enable = !!conf->packet_credit_enable;
	prop->ring_addr = conf->ring_address;
	prop->ring_size = conf->ring_size;
	prop->credit = conf->credit;
	prop->disable = !!conf->disable;
	prop->green_threshold = conf->green_threshold;
	prop->yellow_threshold = conf->yellow_threshold;
}

static void create_add_port_cmd(
		struct pp_qos_dev *qdev,
		const struct pp_qos_port_conf *conf,
		unsigned int phy)
{
	struct cmd_add_port cmd;

	if (PP_QOS_DEVICE_IS_ASSERT(qdev))
		return;

	memset(&cmd, 0, sizeof(cmd));
	cmd_init(qdev, &(cmd.base), CMD_TYPE_ADD_PORT, sizeof(cmd), 0);
	cmd.phy = phy;
	set_cmd_port_properties(&cmd.prop, conf);

	QOS_LOG_DEBUG("cmd %u:%u CMD_TYPE_ADD_PORT %u\n",
			qdev->drvcmds.cmd_id,
			qdev->drvcmds.cmd_fw_id,
			phy);
	cmd_queue_put(qdev->drvcmds.cmdq, &cmd, sizeof(cmd));
	qdev->drvcmds.cmd_fw_id++;
}


static void _create_set_port_cmd(
		struct pp_qos_dev *qdev,
		const struct pp_qos_port_conf *conf,
		unsigned int phy,
		uint32_t modified,
		struct cmd_queue *q,
		uint32_t *pos)
{
	const struct qos_node *node;
	struct cmd_set_port cmd;

	if (PP_QOS_DEVICE_IS_ASSERT(qdev))
		return;

	if (QOS_BITS_IS_SET(modified, QOS_MODIFIED_NODE_TYPE)) {
		create_add_port_cmd(qdev, conf, phy);
	} else {
		memset(&cmd, 0, sizeof(cmd));
		cmd_init(qdev, &(cmd.base), CMD_TYPE_SET_PORT, sizeof(cmd), 0);
		node = get_const_node_from_phy(qdev->nodes, phy);
		cmd.parent_node_prop = node->parent_prop;
		cmd.phy = phy;
		set_cmd_port_properties(&cmd.prop, conf);
		cmd.modified = modified;
		cmd.base.pos = pos;
		cmd_queue_put(q, &cmd, sizeof(cmd));
		qdev->drvcmds.cmd_fw_id++;
	}
}

void create_set_port_cmd(
		struct pp_qos_dev *qdev,
		const struct pp_qos_port_conf *conf,
		unsigned int phy,
		uint32_t modified)
{
	if (PP_QOS_DEVICE_IS_ASSERT(qdev))
		return;

	_create_set_port_cmd(qdev, conf, phy, modified,
			qdev->drvcmds.cmdq, NULL);
	if (!QOS_BITS_IS_SET(modified, QOS_MODIFIED_NODE_TYPE))
		QOS_LOG_DEBUG("cmd %u:%u CMD_TYPE_SET_PORT %u\n",
				qdev->drvcmds.cmd_id,
				qdev->drvcmds.cmd_fw_id,
				phy);

}

static void set_cmd_sched_properties(
		struct sched_properties *prop,
		const struct pp_qos_sched_conf *conf)
{
	prop->common = conf->common_prop;
	prop->parent = conf->sched_parent_prop;
	prop->child = conf->sched_child_prop;
}

static void create_add_sched_cmd(
		struct pp_qos_dev *qdev,
		const struct pp_qos_sched_conf *conf,
		unsigned int phy,
		unsigned int parent)
{
	struct cmd_add_sched cmd;

	if (PP_QOS_DEVICE_IS_ASSERT(qdev))
		return;

	memset(&cmd, 0, sizeof(cmd));
	cmd_init(qdev, &(cmd.base), CMD_TYPE_ADD_SCHED, sizeof(cmd), 0);
	cmd.phy = phy;
	cmd.parent = parent;
	fill_preds(qdev->nodes, phy, cmd.preds, 6);
	set_cmd_sched_properties(&cmd.prop, conf);

	QOS_LOG_DEBUG("cmd %u:%u CMD_TYPE_ADD_SCHED %u\n",
			qdev->drvcmds.cmd_id,
			qdev->drvcmds.cmd_fw_id, phy);
	cmd_queue_put(qdev->drvcmds.cmdq, &cmd, sizeof(cmd));
	add_suspend_port(qdev, get_port(qdev->nodes, phy));
	qdev->drvcmds.cmd_fw_id++;
}

static void _create_set_sched_cmd(
		struct pp_qos_dev *qdev,
		const struct pp_qos_sched_conf *conf,
		unsigned int phy,
		unsigned int parent,
		uint32_t modified)
{
	const struct qos_node *node;
	struct cmd_set_sched cmd;

	if (PP_QOS_DEVICE_IS_ASSERT(qdev))
		return;

	if (QOS_BITS_IS_SET(modified, QOS_MODIFIED_NODE_TYPE)) {
		create_add_sched_cmd(qdev, conf, phy, parent);
	} else {
		memset(&cmd, 0, sizeof(cmd));
		cmd_init(qdev, &(cmd.base), CMD_TYPE_SET_SCHED, sizeof(cmd), 0);
		node = get_const_node_from_phy(qdev->nodes, phy);
		cmd.parent_node_prop = node->parent_prop;
		cmd.phy = phy;
		set_cmd_sched_properties(&cmd.prop, conf);
		cmd.modified = modified;
		cmd_queue_put(qdev->drvcmds.cmdq, &cmd, sizeof(cmd));
		qdev->drvcmds.cmd_fw_id++;
	}
}

void create_set_sched_cmd(
		struct pp_qos_dev *qdev,
		const struct pp_qos_sched_conf *conf,
		unsigned int phy,
		unsigned int parent,
		uint32_t modified)
{
	if (PP_QOS_DEVICE_IS_ASSERT(qdev))
		return;

	_create_set_sched_cmd(qdev, conf, phy, parent, modified);

	if (!QOS_BITS_IS_SET(modified, QOS_MODIFIED_NODE_TYPE))
		QOS_LOG_DEBUG("cmd %u:%u CMD_TYPE_SET_SCHED %u\n",
				qdev->drvcmds.cmd_id,
				qdev->drvcmds.cmd_fw_id, phy);
}

static void set_cmd_queue_properties(
		struct queue_properties *prop,
		const struct pp_qos_queue_conf *conf,
		unsigned int rlm)
{
	prop->common = conf->common_prop;
	prop->child = conf->queue_child_prop;
	prop->blocked = !!conf->blocked;
	prop->wred_enable = !!conf->wred_enable;
	prop->fixed_drop_prob_enable = !!conf->wred_fixed_drop_prob_enable;
	prop->max_burst =  conf->max_burst;
	prop->queue_wred_min_avg_green = conf->queue_wred_min_avg_green;
	prop->queue_wred_max_avg_green = conf->queue_wred_max_avg_green;
	prop->queue_wred_slope_green = conf->queue_wred_slope_green;
	prop->queue_wred_min_avg_yellow = conf->queue_wred_min_avg_yellow;
	prop->queue_wred_max_avg_yellow = conf->queue_wred_max_avg_yellow;
	prop->queue_wred_slope_yellow = conf->queue_wred_slope_yellow;
	prop->queue_wred_min_guaranteed = conf->queue_wred_min_guaranteed;
	prop->queue_wred_max_allowed = conf->queue_wred_max_allowed;
	prop->queue_wred_fixed_drop_prob_green =
		conf->queue_wred_fixed_drop_prob_green;
	prop->queue_wred_fixed_drop_prob_yellow =
		conf->queue_wred_fixed_drop_prob_yellow;
	prop->rlm = rlm;
}

static void create_add_queue_cmd(
		struct pp_qos_dev *qdev,
		const struct pp_qos_queue_conf *conf,
		unsigned int phy,
		unsigned int parent,
		unsigned int rlm)
{
	struct cmd_add_queue cmd;
	const struct qos_node *node;

	if (PP_QOS_DEVICE_IS_ASSERT(qdev))
		return;

	memset(&cmd, 0, sizeof(cmd));
	cmd_init(qdev,
		 &(cmd.base),
		 CMD_TYPE_ADD_QUEUE,
		 sizeof(cmd),
		 CMD_FLAGS_WRAP_PARENT_SUSPEND_RESUME);
	cmd.phy = phy;
	cmd.parent = parent;

	cmd.port = get_port(qdev->nodes, phy);
	node = get_const_node_from_phy(qdev->nodes, phy);
	cmd.is_alias_slave =
			(node->data.queue.alias_master_id == PP_QOS_INVALID_ID)
			? 0 : 1;

	fill_preds(qdev->nodes, phy, cmd.preds, 6);
	set_cmd_queue_properties(&cmd.prop, conf, rlm);

	QOS_LOG_DEBUG("cmd %u:%u CMD_TYPE_ADD_QUEUE %u\n",
			qdev->drvcmds.cmd_id,
			qdev->drvcmds.cmd_fw_id,
			phy);
	cmd_queue_put(qdev->drvcmds.cmdq, &cmd, sizeof(cmd));
	add_suspend_port(qdev, cmd.port);
	qdev->drvcmds.cmd_fw_id++;
}

static void _create_set_queue_cmd(
		struct pp_qos_dev *qdev,
		const struct pp_qos_queue_conf *conf,
		unsigned int phy,
		unsigned int parent,
		unsigned int rlm,
		uint32_t modified)
{
	struct cmd_set_queue cmd;
	const struct qos_node *node;

	if (PP_QOS_DEVICE_IS_ASSERT(qdev))
		return;

	if (QOS_BITS_IS_SET(modified, QOS_MODIFIED_NODE_TYPE)) {
		create_add_queue_cmd(qdev, conf, phy, parent, rlm);
	} else {
		memset(&cmd, 0, sizeof(cmd));
		cmd_init(qdev,
			 &(cmd.base),
			 CMD_TYPE_SET_QUEUE,
			 sizeof(cmd),
			 CMD_FLAGS_WRAP_SUSPEND_RESUME |
			 CMD_FLAGS_WRAP_PARENT_SUSPEND_RESUME);
		cmd.phy = phy;
		set_cmd_queue_properties(&cmd.prop, conf, rlm);
		cmd.modified = modified;
		node = get_const_node_from_phy(qdev->nodes, phy);
		cmd.is_alias_slave =
				(node->data.queue.alias_master_id == PP_QOS_INVALID_ID)
				? 0 : 1;

		cmd_queue_put(qdev->drvcmds.cmdq, &cmd, sizeof(cmd));
		qdev->drvcmds.cmd_fw_id++;
	}
}

void create_set_queue_cmd(
		struct pp_qos_dev *qdev,
		const struct pp_qos_queue_conf *conf,
		unsigned int phy,
		unsigned int parent,
		unsigned int rlm,
		uint32_t modified)
{
	if (PP_QOS_DEVICE_IS_ASSERT(qdev))
		return;

	_create_set_queue_cmd(qdev, conf, phy, parent, rlm, modified);
	if (!QOS_BITS_IS_SET(modified, QOS_MODIFIED_NODE_TYPE))
		QOS_LOG_DEBUG("cmd %u:%u CMD_TYPE_SET_QUEUE %u\n",
				qdev->drvcmds.cmd_id,
				qdev->drvcmds.cmd_fw_id, phy);
}

void create_flush_queue_cmd(struct pp_qos_dev *qdev, unsigned int rlm)
{
	struct cmd_flush_queue cmd;

	if (PP_QOS_DEVICE_IS_ASSERT(qdev))
		return;

	cmd_init(qdev, &(cmd.base), CMD_TYPE_FLUSH_QUEUE, sizeof(cmd), 0);
	cmd.rlm = rlm;
	QOS_LOG_DEBUG("cmd %u:%u CMD_TYPE_FLUSH_QUEUE %u\n",
			qdev->drvcmds.cmd_id,
			qdev->drvcmds.cmd_fw_id,
			rlm);
	cmd_queue_put(qdev->drvcmds.cmdq, &cmd, sizeof(cmd));
	qdev->drvcmds.cmd_fw_id++;
}

void create_parent_change_cmd(struct pp_qos_dev *qdev, unsigned int phy)
{
	struct cmd_parent_change cmd;
	const struct qos_node *node;

	if (PP_QOS_DEVICE_IS_ASSERT(qdev))
		return;

	node = get_const_node_from_phy(qdev->nodes, phy);
	memset(&cmd, 0, sizeof(cmd));
	cmd_init(qdev, &(cmd.base), CMD_TYPE_PARENT_CHANGE, sizeof(cmd), 0);
	cmd.phy = phy;
	cmd.type = node->type;
	cmd.parent_node_prop = node->parent_prop;
	QOS_LOG_DEBUG("cmd %u:%u CMD_TYPE_PARENT_CHANGE %u first:%u num:%d\n",
		      qdev->drvcmds.cmd_id,
		      qdev->drvcmds.cmd_fw_id,
		      phy,
		      cmd.parent_node_prop.first_child_phy,
		      cmd.parent_node_prop.num_of_children);
	cmd_queue_put(qdev->drvcmds.cmdq, &cmd, sizeof(cmd));
	qdev->drvcmds.cmd_fw_id++;
}

void create_get_port_stats_cmd(
		struct pp_qos_dev *qdev,
		unsigned int phy,
		unsigned int addr,
		struct pp_qos_port_stat *pstat)
{
	struct cmd_get_port_stats cmd;

	if (PP_QOS_DEVICE_IS_ASSERT(qdev))
		return;

	memset(&cmd, 0, sizeof(cmd));
	cmd_init(
			qdev,
			&(cmd.base),
			CMD_TYPE_GET_PORT_STATS,
			sizeof(cmd),
			CMD_FLAGS_POST_PROCESS);
	cmd.phy = phy;
	cmd.addr =  addr;
	cmd.stat = pstat;
	QOS_LOG_DEBUG("cmd %u:%u CMD_TYPE_GET_PORT_STATS %u\n",
			qdev->drvcmds.cmd_id,
			qdev->drvcmds.cmd_fw_id,
			phy);
	cmd_queue_put(qdev->drvcmds.cmdq, &cmd, sizeof(cmd));
	qdev->drvcmds.cmd_fw_id++;
}

void create_get_queue_stats_cmd(
		struct pp_qos_dev *qdev,
		unsigned int phy,
		unsigned int rlm,
		unsigned int addr,
		struct pp_qos_queue_stat *qstat)
{
	struct cmd_get_queue_stats cmd;

	if (PP_QOS_DEVICE_IS_ASSERT(qdev))
		return;

	memset(&cmd, 0, sizeof(cmd));
	cmd_init(
			qdev,
			&(cmd.base),
			CMD_TYPE_GET_QUEUE_STATS,
			sizeof(cmd),
			CMD_FLAGS_POST_PROCESS);
	cmd.phy = phy;
	cmd.rlm = rlm;
	cmd.addr =  addr;
	cmd.stat = qstat;
	QOS_LOG_DEBUG("cmd %u:%u CMD_TYPE_GET_QUEUE_STATS %u\n",
			qdev->drvcmds.cmd_id,
			qdev->drvcmds.cmd_fw_id,
			phy);
	cmd_queue_put(qdev->drvcmds.cmdq, &cmd, sizeof(cmd));
	qdev->drvcmds.cmd_fw_id++;
}

void create_get_node_info_cmd(
		struct pp_qos_dev *qdev,
		unsigned int phy,
		unsigned int addr,
		struct pp_qos_node_info *info)
{
	struct cmd_get_node_info cmd;

	if (PP_QOS_DEVICE_IS_ASSERT(qdev))
		return;

	memset(&cmd, 0, sizeof(cmd));
	cmd_init(
			qdev,
			&(cmd.base),
			CMD_TYPE_GET_NODE_INFO,
			sizeof(cmd),
			CMD_FLAGS_POST_PROCESS);
	cmd.phy = phy;
	cmd.addr =  addr;
	cmd.info = info;
	QOS_LOG_DEBUG("cmd %u:%u CMD_TYPE_GET_NODE_INFO %u\n",
			qdev->drvcmds.cmd_id,
			qdev->drvcmds.cmd_fw_id,
			phy);
	cmd_queue_put(qdev->drvcmds.cmdq, &cmd, sizeof(cmd));
	qdev->drvcmds.cmd_fw_id++;
}

void create_get_table_entry_cmd(struct pp_qos_dev *qdev, u32 phy,
				u32 addr, u32 table_type)
{
	struct cmd_read_table_entry cmd;

	if (PP_QOS_DEVICE_IS_ASSERT(qdev))
		return;

	memset(&cmd, 0, sizeof(cmd));
	cmd_init(qdev, &cmd.base, CMD_TYPE_READ_TABLE_ENTRY, sizeof(cmd), 0);

	cmd.phy = phy;
	cmd.table_type = table_type;
	cmd.addr =  addr;

	QOS_LOG_DEBUG("cmd %u:%u CMD_TYPE_READ_TABLE_ENTRY %u from phy %u\n",
		      qdev->drvcmds.cmd_id,
		      qdev->drvcmds.cmd_fw_id,
		      table_type, phy);

	cmd_queue_put(qdev->drvcmds.cmdq, &cmd, sizeof(cmd));
	qdev->drvcmds.cmd_fw_id++;
}

void create_get_sys_info_cmd(struct pp_qos_dev *qdev,
			     unsigned int addr,
			     struct qos_hw_info *info)
{
	struct cmd_get_system_info cmd;

	if (PP_QOS_DEVICE_IS_ASSERT(qdev))
		return;

	memset(&cmd, 0, sizeof(cmd));
	cmd_init(qdev,
		 &(cmd.base),
		 CMD_TYPE_GET_SYSTEM_INFO,
		 sizeof(cmd),
		 CMD_FLAGS_POST_PROCESS);
	cmd.addr =  addr;
	cmd.num_used = &(info->num_used);
	cmd.infinite_loop_error_occurred =
			&(info->infinite_loop_error_occurred);
	cmd.bwl_update_error_occurred = &(info->bwl_update_error_occurred);
	cmd.quanta = &(info->quanta);

	QOS_LOG_DEBUG("cmd %u:%u CMD_TYPE_GET_SYSTEM_INFO\n",
			qdev->drvcmds.cmd_id,
			qdev->drvcmds.cmd_fw_id);
	cmd_queue_put(qdev->drvcmds.cmdq, &cmd, sizeof(cmd));
	qdev->drvcmds.cmd_fw_id++;
}

static void _create_set_shared_group_cmd(struct pp_qos_dev *qdev,
		enum cmd_type ctype,
		unsigned int id,
		unsigned int limit)
{
	struct cmd_set_shared_group cmd;

	if (PP_QOS_DEVICE_IS_ASSERT(qdev))
		return;

	cmd_init(qdev, &(cmd.base), ctype, sizeof(cmd), 0);
	cmd.id = id;
	cmd.limit = limit;
	QOS_LOG("cmd %u:%u %s id %u limit %u\n",
		qdev->drvcmds.cmd_id,
		qdev->drvcmds.cmd_fw_id,
		cmd_str[ctype],
		id, limit);
	cmd_queue_put(qdev->drvcmds.cmdq, &cmd, sizeof(cmd));
	qdev->drvcmds.cmd_fw_id++;
}

void create_add_shared_group_cmd(struct pp_qos_dev *qdev,
		unsigned int id,
		unsigned int limit)
{
	_create_set_shared_group_cmd(qdev, CMD_TYPE_ADD_SHARED_GROUP,
			id, limit);
}

void create_set_shared_group_cmd(struct pp_qos_dev *qdev,
		unsigned int id,
		unsigned int limit)
{
	_create_set_shared_group_cmd(qdev, CMD_TYPE_SET_SHARED_GROUP,
			id, limit);
}

void create_remove_shared_group_cmd(struct pp_qos_dev *qdev,
		unsigned int id)
{
	struct cmd_remove_shared_group cmd;

	if (PP_QOS_DEVICE_IS_ASSERT(qdev))
		return;

	cmd_init(
			qdev,
			&(cmd.base),
			CMD_TYPE_REMOVE_SHARED_GROUP,
			sizeof(cmd), 0);
	cmd.id = id;
	QOS_LOG_DEBUG("cmd %u:%u CMD_TYPE_REMOVE_SHARED_GROUP id %u\n",
			qdev->drvcmds.cmd_id,
			qdev->drvcmds.cmd_fw_id,
			id);
	cmd_queue_put(qdev->drvcmds.cmdq, &cmd, sizeof(cmd));
	qdev->drvcmds.cmd_fw_id++;
}

void create_push_desc_cmd(struct pp_qos_dev *qdev, unsigned int queue,
		unsigned int size, unsigned int color, unsigned int addr)
{
	struct cmd_push_desc cmd;

	if (PP_QOS_DEVICE_IS_ASSERT(qdev))
		return;

	cmd_init(qdev, &(cmd.base), CMD_TYPE_PUSH_DESC, sizeof(cmd), 0);
	cmd.queue = queue;
	cmd.size = size;
	cmd.color = color;
	cmd.addr = addr;
	cmd_queue_put(qdev->drvcmds.cmdq, &cmd, sizeof(cmd));
	qdev->drvcmds.cmd_fw_id++;
}

/******************************************************************************/
/*                                 FW CMDS                                    */
/******************************************************************************/

struct fw_set_common {
	uint32_t valid;
	int suspend;
	unsigned int bw_limit;
	unsigned int shared_bw_group;
	u32 port_phy; /* Relevant only for queue */
};

struct fw_set_parent {
	uint32_t valid;
	int best_effort_enable;
	uint16_t first;
	uint16_t last;
	uint16_t first_wrr;
};

struct fw_set_child {
	uint32_t valid;
	uint32_t bw_share;
	uint16_t preds[6];
};

struct fw_set_port {
	uint32_t valid;
	void	 *ring_addr;
	size_t	 ring_size;
	int active;
	unsigned int green_threshold;
	unsigned int yellow_threshold;
};

struct fw_set_sched {
	uint32_t valid;
};

struct fw_set_queue {
	uint32_t valid;
	unsigned int rlm;
	int active;
	uint32_t disable;
	unsigned int queue_wred_min_avg_green;
	unsigned int queue_wred_max_avg_green;
	unsigned int queue_wred_slope_green;
	unsigned int queue_wred_min_avg_yellow;
	unsigned int queue_wred_max_avg_yellow;
	unsigned int queue_wred_slope_yellow;
	unsigned int queue_wred_min_guaranteed;
	unsigned int queue_wred_max_allowed;
	unsigned int queue_wred_fixed_drop_prob_green;
	unsigned int queue_wred_fixed_drop_prob_yellow;
	unsigned int is_alias_slave;
};

struct fw_internal {
	struct fw_set_common common;
	struct fw_set_parent parent;
	struct fw_set_child  child;
	union {
		struct fw_set_port port;
		struct fw_set_sched sched;
		struct fw_set_queue queue;
	} type_data;
	unsigned int suspend_ports_index;
	unsigned int num_suspend_ports;
	unsigned int suspend_ports[QOS_MAX_PORTS];
	unsigned int	pushed;
	int		ongoing;	/* Suspend ports indication */
};

/* Is ongoing means, we suspended the ports without resuming yet */
int is_ongoing(struct pp_qos_dev *qdev)
{
	return ((struct fw_internal *)(qdev->fwbuf))->ongoing;
}

/******************************************************************************/
/*                         FW write functions                                 */
/******************************************************************************/

void add_suspend_port(struct pp_qos_dev *qdev, unsigned int port)
{
	struct fw_internal *internals;
	unsigned int i;
	struct qos_node *node;

	node = get_node_from_phy(qdev->nodes, port);
	QOS_ASSERT(node_port(node), "Node %u is not a port\n", port);
	internals = qdev->fwbuf;
	for (i = 0; i <  internals->num_suspend_ports; ++i)
		if (internals->suspend_ports[i] == port)
			return;
	QOS_ASSERT(internals->num_suspend_ports <= qdev->max_port,
		   "Suspend ports buffer is full\n");
	internals->suspend_ports[internals->num_suspend_ports] = port;
	++(internals->num_suspend_ports);
}

int init_fwdata_internals(struct pp_qos_dev *qdev)
{
	qdev->fwbuf = QOS_MALLOC(sizeof(struct fw_internal));
	if (qdev->fwbuf) {
		memset(qdev->fwbuf, 0, sizeof(struct fw_internal));
		return 0;
	}
	return -EBUSY;
}

void clean_fwdata_internals(struct pp_qos_dev *qdev)
{
	if (qdev->fwbuf)
		QOS_FREE(qdev->fwbuf);
}

/*
 * Following functions translate driver commands to firmware
 * commands
 */
static uint32_t *fw_write_init_logger_cmd(
		uint32_t *buf,
		const struct cmd_init_logger *cmd,
		uint32_t flags)
{
	*buf++ = qos_u32_to_uc(UC_QOS_CMD_INIT_UC_LOGGER);
	*buf++ = qos_u32_to_uc(flags);
	*buf++ = qos_u32_to_uc(4);
	*buf++ = qos_u32_to_uc((uintptr_t)cmd->addr & 0xFFFFFFFF);
	*buf++ = qos_u32_to_uc(cmd->mode);
	*buf++ = qos_u32_to_uc(cmd->level);
	*buf++ = qos_u32_to_uc(cmd->num_of_msgs);
	return buf;
}

static uint32_t *fw_write_init_qos_cmd(
		uint32_t *buf,
		const struct cmd_init_qos *cmd,
		uint32_t flags)
{
	*buf++ = qos_u32_to_uc(UC_QOS_CMD_INIT_QOS);
	*buf++ = qos_u32_to_uc(flags);
	*buf++ = qos_u32_to_uc(12);
	*buf++ = qos_u32_to_uc(cmd->qm_ddr_start & 0xFFFFFFFF);
	*buf++ = qos_u32_to_uc(cmd->qm_num_pages);
	*buf++ = qos_u32_to_uc(cmd->wred_total_avail_resources);
	*buf++ = qos_u32_to_uc(cmd->wred_prioritize_pop);
	*buf++ = qos_u32_to_uc(cmd->wred_avg_q_size_p);
	*buf++ = qos_u32_to_uc(cmd->wred_max_q_size);
	*buf++ = qos_u32_to_uc(cmd->num_of_ports);
	*buf++ = qos_u32_to_uc(cmd->qos_clock);
	*buf++ = qos_u32_to_uc(0); /* BM push address for CoDel. LGM ONLY */
	*buf++ = qos_u32_to_uc(ltq_get_soc_rev());
	*buf++ = qos_u32_to_uc(cmd->bwl_temp_buff);
	*buf++ = qos_u32_to_uc(cmd->sbwl_temp_buff);
	return buf;
}

static uint32_t *fw_write_add_port_cmd(
		uint32_t *buf,
		const struct cmd_add_port *cmd,
		uint32_t flags)
{
	*buf++ = qos_u32_to_uc(UC_QOS_CMD_ADD_PORT);
	*buf++ = qos_u32_to_uc(flags);
	*buf++ = qos_u32_to_uc(15);
	*buf++ = qos_u32_to_uc(cmd->phy);
	*buf++ = qos_u32_to_uc(!cmd->prop.disable);
	*buf++ = qos_u32_to_uc(0);
	*buf++ = qos_u32_to_uc(0);
	*buf++ = qos_u32_to_uc(cmd->prop.common.bandwidth_limit);
	*buf++ = qos_u32_to_uc(!!cmd->prop.parent.best_effort_enable);
	*buf++ = qos_u32_to_uc(0);
	*buf++ = qos_u32_to_uc(cmd->prop.common.shared_bandwidth_group);
	*buf++ = qos_u32_to_uc(cmd->prop.packet_credit_enable);
	*buf++ = qos_u32_to_uc(cmd->prop.ring_size);
	*buf++ = qos_u32_to_uc(GET_ADDRESS_HIGH(cmd->prop.ring_addr));
	*buf++ = qos_u32_to_uc(((uintptr_t)cmd->prop.ring_addr) & 0xFFFFFFFF);
	*buf++ = qos_u32_to_uc(cmd->prop.credit);
	*buf++ = qos_u32_to_uc(cmd->prop.green_threshold);
	*buf++ = qos_u32_to_uc(cmd->prop.yellow_threshold);
	return buf;
}

static uint32_t *fw_write_set_port_cmd(
		uint32_t *buf,
		unsigned int phy,
		uint32_t flags,
		const struct fw_set_common *common,
		struct fw_set_parent *parent,
		const struct fw_set_port *port)
{
	*buf++ = qos_u32_to_uc(UC_QOS_CMD_SET_PORT);
	*buf++ = qos_u32_to_uc(flags);
	*buf++ = qos_u32_to_uc(16);
	*buf++ = qos_u32_to_uc(phy);
	*buf++ = qos_u32_to_uc(common->valid | parent->valid);
	*buf++ = qos_u32_to_uc(common->suspend);
	if (parent->first > parent->last) {
		parent->first = 0;
		parent->last = 0;
	}
	*buf++ = qos_u32_to_uc(parent->first);
	*buf++ = qos_u32_to_uc(parent->last);
	*buf++ = qos_u32_to_uc(common->bw_limit);
	*buf++ = qos_u32_to_uc(parent->best_effort_enable);
	*buf++ = qos_u32_to_uc(parent->first_wrr);
	*buf++ = qos_u32_to_uc(common->shared_bw_group);
	*buf++ = qos_u32_to_uc(port->valid);
	*buf++ = qos_u32_to_uc(port->ring_size);
	*buf++ = qos_u32_to_uc(GET_ADDRESS_HIGH(port->ring_addr));
	*buf++ = qos_u32_to_uc(((uintptr_t)port->ring_addr) & 0xFFFFFFFF);
	*buf++ = qos_u32_to_uc(port->active);
	*buf++ = qos_u32_to_uc(port->green_threshold);
	*buf++ = qos_u32_to_uc(port->yellow_threshold);
	return buf;
}

static uint32_t *fw_write_add_sched_cmd(
		uint32_t *buf,
		const struct cmd_add_sched *cmd,
		uint32_t flags)
{
	unsigned int i;

	*buf++ = qos_u32_to_uc(UC_QOS_CMD_ADD_SCHEDULER);
	*buf++ = qos_u32_to_uc(flags);
	*buf++ = qos_u32_to_uc(13);
	*buf++ = qos_u32_to_uc(cmd->phy);
	*buf++ = qos_u32_to_uc(0);
	*buf++ = qos_u32_to_uc(0);
	*buf++ = qos_u32_to_uc(cmd->prop.common.bandwidth_limit);
	*buf++ = qos_u32_to_uc(!!cmd->prop.parent.best_effort_enable);
	*buf++ = qos_u32_to_uc(0);
	*buf++ = qos_u32_to_uc(cmd->prop.common.shared_bandwidth_group);
	for (i = 0; i < 6; ++i)
		*buf++ = qos_u32_to_uc(cmd->preds[i]);

	return buf;
}

static uint32_t *fw_write_set_sched_cmd(
		uint32_t *buf,
		unsigned int phy,
		uint32_t flags,
		const struct fw_set_common *common,
		struct fw_set_parent *parent,
		const struct fw_set_child *child,
		const struct fw_set_sched *sched)
{
	unsigned int i;

	*buf++ = qos_u32_to_uc(UC_QOS_CMD_SET_SCHEDULER);
	*buf++ = qos_u32_to_uc(flags);
	*buf++ = qos_u32_to_uc(16);
	*buf++ = qos_u32_to_uc(phy);
	*buf++ = qos_u32_to_uc(common->valid | parent->valid |
			child->valid | sched->valid);
	*buf++ = qos_u32_to_uc(common->suspend);
	if (parent->first > parent->last) {
		parent->first = 0;
		parent->last = 0;
	}
	*buf++ = qos_u32_to_uc(parent->first);
	*buf++ = qos_u32_to_uc(parent->last);
	*buf++ = qos_u32_to_uc(common->bw_limit);
	*buf++ = qos_u32_to_uc(parent->best_effort_enable);
	*buf++ = qos_u32_to_uc(parent->first_wrr);
	*buf++ = qos_u32_to_uc(child->bw_share);
	*buf++ = qos_u32_to_uc(common->shared_bw_group);

	for (i = 0; i < 6; ++i)
		*buf++ = qos_u32_to_uc(child->preds[i]);

	return buf;
}

static uint32_t *fw_write_add_queue_cmd(
		uint32_t *buf,
		const struct cmd_add_queue *cmd,
		uint32_t flags)
{
	unsigned int i;
	uint32_t disable;

	*buf++ = qos_u32_to_uc(UC_QOS_CMD_ADD_QUEUE);
	*buf++ = qos_u32_to_uc(flags);
	*buf++ = qos_u32_to_uc(25);
	*buf++ = qos_u32_to_uc(cmd->phy);
	*buf++ = qos_u32_to_uc(cmd->port);
	*buf++ = qos_u32_to_uc(cmd->prop.rlm);
	*buf++ = qos_u32_to_uc(cmd->prop.common.bandwidth_limit);
	*buf++ = qos_u32_to_uc(cmd->prop.common.shared_bandwidth_group);
	for (i = 0; i < 6; ++i)
		*buf++ = qos_u32_to_uc(cmd->preds[i]);
	*buf++ = qos_u32_to_uc(!cmd->prop.blocked);

	disable = 0;
	if (!cmd->prop.wred_enable)
		QOS_BITS_SET(disable, 1);
	if (cmd->prop.fixed_drop_prob_enable)
		QOS_BITS_SET(disable, 8);
	*buf++ = qos_u32_to_uc(disable);

	*buf++ = qos_u32_to_uc(cmd->prop.queue_wred_fixed_drop_prob_green);
	*buf++ = qos_u32_to_uc(cmd->prop.queue_wred_fixed_drop_prob_yellow);
	*buf++ = qos_u32_to_uc(cmd->prop.queue_wred_min_avg_yellow);
	*buf++ = qos_u32_to_uc(cmd->prop.queue_wred_max_avg_yellow);
	*buf++ = qos_u32_to_uc(cmd->prop.queue_wred_slope_yellow);
	*buf++ = qos_u32_to_uc(cmd->prop.queue_wred_min_avg_green);
	*buf++ = qos_u32_to_uc(cmd->prop.queue_wred_max_avg_green);
	*buf++ = qos_u32_to_uc(cmd->prop.queue_wred_slope_green);
	*buf++ = qos_u32_to_uc(cmd->prop.queue_wred_min_guaranteed);
	if (cmd->prop.blocked)
		*buf++ = qos_u32_to_uc(0);
	else
		*buf++ = qos_u32_to_uc(cmd->prop.queue_wred_max_allowed);
	*buf++ = qos_u32_to_uc(cmd->is_alias_slave);
	*buf++ = qos_u32_to_uc(0); /* Disable Codel. LGM ONLY */
	return buf;
}

static uint32_t *fw_write_set_queue_cmd(
		uint32_t *buf,
		unsigned int phy,
		uint32_t flags,
		const struct fw_set_common *common,
		const struct fw_set_child *child,
		const struct fw_set_queue *queue)
{
	unsigned int i;

	*buf++ = qos_u32_to_uc(UC_QOS_CMD_SET_QUEUE);
	*buf++ = qos_u32_to_uc(flags);
	*buf++ = qos_u32_to_uc(29);
	*buf++ = qos_u32_to_uc(phy);
	*buf++ = qos_u32_to_uc(queue->rlm);
	*buf++ = qos_u32_to_uc(common->valid | child->valid);
	*buf++ = qos_u32_to_uc(common->suspend);
	*buf++ = qos_u32_to_uc(common->bw_limit);
	*buf++ = qos_u32_to_uc(child->bw_share);
	*buf++ = qos_u32_to_uc(common->shared_bw_group);
	for (i = 0; i < 6; ++i)
		*buf++ = qos_u32_to_uc(child->preds[i]);
	*buf++ = qos_u32_to_uc(common->port_phy);
	*buf++ = qos_u32_to_uc(queue->valid);
	*buf++ = qos_u32_to_uc(queue->active);
	*buf++ = qos_u32_to_uc(queue->disable);

	*buf++ = qos_u32_to_uc(queue->queue_wred_fixed_drop_prob_green);
	*buf++ = qos_u32_to_uc(queue->queue_wred_fixed_drop_prob_yellow);
	*buf++ = qos_u32_to_uc(queue->queue_wred_min_avg_yellow);
	*buf++ = qos_u32_to_uc(queue->queue_wred_max_avg_yellow);
	*buf++ = qos_u32_to_uc(queue->queue_wred_slope_yellow);
	*buf++ = qos_u32_to_uc(queue->queue_wred_min_avg_green);
	*buf++ = qos_u32_to_uc(queue->queue_wred_max_avg_green);
	*buf++ = qos_u32_to_uc(queue->queue_wred_slope_green);
	*buf++ = qos_u32_to_uc(queue->queue_wred_min_guaranteed);
	*buf++ = qos_u32_to_uc(queue->queue_wred_max_allowed);
	*buf++ = qos_u32_to_uc(queue->is_alias_slave);
	*buf++ = qos_u32_to_uc(0); /* Disable Codel. LGM ONLY */

	return buf;
}

static uint32_t *fw_write_flush_queue_cmd(
		uint32_t *buf,
		const struct cmd_flush_queue *cmd,
		uint32_t flags)
{
	*buf++ = qos_u32_to_uc(UC_QOS_CMD_FLUSH_QUEUE);
	*buf++ = qos_u32_to_uc(flags);
	*buf++ = qos_u32_to_uc(1);
	*buf++ = qos_u32_to_uc(cmd->rlm);
	return buf;
}

static uint32_t *fw_write_move_sched_cmd(
		uint32_t *buf,
		const struct cmd_move *cmd,
		uint32_t flags)
{
	unsigned int i;

	*buf++ = qos_u32_to_uc(UC_QOS_CMD_MOVE_SCHEDULER);
	*buf++ = qos_u32_to_uc(flags);
	*buf++ = qos_u32_to_uc(8);
	*buf++ = qos_u32_to_uc(cmd->src);
	*buf++ = qos_u32_to_uc(cmd->dst);

	for (i = 0; i < 6; ++i)
		*buf++ = qos_u32_to_uc(cmd->preds[i]);

	return buf;
}

static uint32_t *fw_write_move_queue_cmd(
		uint32_t *buf,
		const struct cmd_move *cmd,
		uint32_t flags)
{
	unsigned int i;

	*buf++ = qos_u32_to_uc(UC_QOS_CMD_MOVE_QUEUE);
	*buf++ = qos_u32_to_uc(flags);
	*buf++ = qos_u32_to_uc(10);
	*buf++ = qos_u32_to_uc(cmd->src);
	*buf++ = qos_u32_to_uc(cmd->dst);
	*buf++ = qos_u32_to_uc(cmd->dst_port);
	*buf++ = qos_u32_to_uc(cmd->rlm);

	for (i = 0; i < 6; ++i)
		*buf++ = qos_u32_to_uc(cmd->preds[i]);

	return buf;
}

static uint32_t *fw_write_remove_queue_cmd(
		uint32_t *buf,
		const struct cmd_remove_node *cmd,
		uint32_t flags)
{
	*buf++ = qos_u32_to_uc(UC_QOS_CMD_REM_QUEUE);
	*buf++ = qos_u32_to_uc(flags);
	*buf++ = qos_u32_to_uc(3);
	*buf++ = qos_u32_to_uc(cmd->phy);
	*buf++ = qos_u32_to_uc(cmd->data);
	*buf++ = qos_u32_to_uc(cmd->is_alias_slave);
	return buf;
}

static uint32_t *fw_write_remove_sched_cmd(
		uint32_t *buf,
		const struct cmd_remove_node *cmd,
		uint32_t flags)
{
	*buf++ = qos_u32_to_uc(UC_QOS_CMD_REM_SCHEDULER);
	*buf++ = qos_u32_to_uc(flags);
	*buf++ = qos_u32_to_uc(1);
	*buf++ = qos_u32_to_uc(cmd->phy);
	return buf;
}

static uint32_t *fw_write_remove_port_cmd(
		uint32_t *buf,
		const struct cmd_remove_node *cmd,
		uint32_t flags)
{
	*buf++ = qos_u32_to_uc(UC_QOS_CMD_REM_PORT);
	*buf++ = qos_u32_to_uc(flags);
	*buf++ = qos_u32_to_uc(1);
	*buf++ = qos_u32_to_uc(cmd->phy);
	return buf;
}

static uint32_t *fw_write_get_queue_stats(
		uint32_t *buf,
		const struct cmd_get_queue_stats *cmd,
		uint32_t flags)
{
	uint32_t reset;

	*buf++ = qos_u32_to_uc(UC_QOS_CMD_GET_QUEUE_STATS);
	*buf++ = qos_u32_to_uc(flags);
	*buf++ = qos_u32_to_uc(4);
	*buf++ = qos_u32_to_uc(cmd->phy);
	*buf++ = qos_u32_to_uc(cmd->rlm);
	*buf++ = qos_u32_to_uc((uintptr_t)cmd->addr & 0xFFFFFFFF);
	if (cmd->stat->reset)
		reset = QUEUE_STATS_CLEAR_Q_AVG_SIZE_BYTES |
			QUEUE_STATS_CLEAR_DROP_P_YELLOW |
			QUEUE_STATS_CLEAR_DROP_P_GREEN |
			QUEUE_STATS_CLEAR_TOTAL_BYTES_ADDED |
			QUEUE_STATS_CLEAR_TOTAL_ACCEPTS |
			QUEUE_STATS_CLEAR_TOTAL_DROPS |
			QUEUE_STATS_CLEAR_TOTAL_DROPPED_BYTES |
			QUEUE_STATS_CLEAR_TOTAL_RED_DROPS;
	else
		reset = QUEUE_STATS_CLEAR_NONE;
	*buf++ = qos_u32_to_uc(reset);

	return buf;
}

static uint32_t *fw_write_get_port_stats(
		uint32_t *buf,
		const struct cmd_get_port_stats *cmd,
		uint32_t flags)
{
	uint32_t reset;

	*buf++ = qos_u32_to_uc(UC_QOS_CMD_GET_PORT_STATS);
	*buf++ = qos_u32_to_uc(flags);
	*buf++ = qos_u32_to_uc(3);
	*buf++ = qos_u32_to_uc(cmd->phy);
	*buf++ = qos_u32_to_uc((uintptr_t)cmd->addr & 0xFFFFFFFF);
	if (cmd->stat->reset)
		reset = PORT_STATS_CLEAR_ALL;
	else
		reset = PORT_STATS_CLEAR_NONE;
	*buf++ = qos_u32_to_uc(reset);
	return buf;
}

static uint32_t *fw_write_get_system_info(
		uint32_t *buf,
		const struct cmd_get_system_info *cmd,
		uint32_t flags)
{
	*buf++ = qos_u32_to_uc(UC_QOS_CMD_GET_SYSTEM_STATS);
	*buf++ = qos_u32_to_uc(flags);
	*buf++ = qos_u32_to_uc(2);
	*buf++ = qos_u32_to_uc((uintptr_t)cmd->addr & 0xFFFFFFFF);
	*buf++ = qos_u32_to_uc(0);
	return buf;
}

static uint32_t *fw_write_set_shared_group(
		uint32_t *buf,
		enum cmd_type ctype,
		const struct cmd_set_shared_group *cmd,
		uint32_t flags)
{
	uint32_t uc_cmd;

	if (ctype == CMD_TYPE_ADD_SHARED_GROUP)
		uc_cmd = UC_QOS_CMD_ADD_SHARED_BW_LIMIT_GROUP;
	else
		uc_cmd = UC_QOS_CMD_SET_SHARED_BW_LIMIT_GROUP;

	*buf++ = qos_u32_to_uc(uc_cmd);
	*buf++ = qos_u32_to_uc(flags);
	*buf++ = qos_u32_to_uc(2);
	*buf++ = qos_u32_to_uc(cmd->id);
	*buf++ = qos_u32_to_uc(cmd->limit);
	return buf;
}

static uint32_t *fw_write_remove_shared_group(uint32_t *buf,
		const struct cmd_remove_shared_group *cmd,
		uint32_t flags)
{
	*buf++ = qos_u32_to_uc(UC_QOS_CMD_REM_SHARED_BW_LIMIT_GROUP);
	*buf++ = qos_u32_to_uc(flags);
	*buf++ = qos_u32_to_uc(1);
	*buf++ = qos_u32_to_uc(cmd->id);
	return buf;
}

static uint32_t *fw_write_push_desc(
		uint32_t *buf,
		const struct cmd_push_desc *cmd,
		uint32_t flags)
{
	*buf++ = qos_u32_to_uc(UC_QOS_CMD_DEBUG_PUSH_DESC);
	*buf++ = qos_u32_to_uc(flags);
	*buf++ = qos_u32_to_uc(6);
	*buf++ = qos_u32_to_uc(cmd->queue);
	*buf++ = qos_u32_to_uc(cmd->size);
	*buf++ = qos_u32_to_uc(cmd->color);
	*buf++ = qos_u32_to_uc(cmd->addr);
	*buf++ = qos_u32_to_uc(0);
	*buf++ = qos_u32_to_uc(0);
	return buf;
}

static uint32_t *fw_write_get_node_info(
		uint32_t *buf,
		const struct cmd_get_node_info *cmd,
		uint32_t flags)
{
	*buf++ = qos_u32_to_uc(UC_QOS_CMD_GET_NODE_INFO);
	*buf++ = qos_u32_to_uc(flags);
	*buf++ = qos_u32_to_uc(2);
	*buf++ = qos_u32_to_uc(cmd->phy);
	*buf++ = qos_u32_to_uc(cmd->addr);
	return buf;
}

static uint32_t *fw_read_table_entry(u32 *buf,
				     const struct cmd_read_table_entry *cmd,
				     u32 flags)
{
	*buf++ = qos_u32_to_uc(UC_QOS_CMD_DEBUG_READ_NODE);
	*buf++ = qos_u32_to_uc(flags);
	*buf++ = qos_u32_to_uc(3);
	*buf++ = qos_u32_to_uc(cmd->phy);
	*buf++ = qos_u32_to_uc(cmd->table_type);
	*buf++ = qos_u32_to_uc(cmd->addr);

	return buf;
}

static uint32_t *fw_update_tree_cmd(uint32_t* buf, u32 phy, u32 flags,
				    const struct fw_set_common *common,
				    struct fw_set_parent *parent,
				    const struct fw_set_port *port,
				    u32 max_allowed_ddr_phy_addr,
				    u16 *queues, u32 num_queues,
				    u16 max_queues)
{
	u32 *bitmap_word;
	u16 queue_idx;
	u32 word_idx;
	u32 bit_in_word;
	u32 bitmap_words = max_queues >> 5;

	pr_debug("port %u. words %u\n", phy, bitmap_words);

	bitmap_word = (u32 *)kzalloc(bitmap_words * sizeof(u32), GFP_ATOMIC);
	if (unlikely(!bitmap_word)) {
		pr_err("queue bitmap allocation failed\n");
		return 0;
	}

	for (queue_idx = 0; queue_idx < num_queues; queue_idx++) {
		word_idx = *queues / 32;
		bit_in_word = *queues - (32 * word_idx);
		bitmap_word[word_idx] |= BIT(bit_in_word);
		queues++;
	}

	*buf++ = qos_u32_to_uc(UC_QOS_CMD_UPDATE_PORT_TREE);
	*buf++ = qos_u32_to_uc(flags);
	*buf++ = qos_u32_to_uc(2 + bitmap_words);
	*buf++ = qos_u32_to_uc(phy);
	*buf++ = qos_u32_to_uc(max_allowed_ddr_phy_addr);

	for (word_idx = 0; word_idx < bitmap_words; word_idx++)
		*buf++ = qos_u32_to_uc(bitmap_word[word_idx]);

	kfree(bitmap_word);

	return buf;
}

static uint32_t *fw_suspend_tree_cmd(u32 *buf, u32 phy, u32 flags)
{
	*buf++ = qos_u32_to_uc(UC_QOS_CMD_SUSPEND_PORT_TREE);
	*buf++ = qos_u32_to_uc(flags);
	*buf++ = qos_u32_to_uc(1);
	*buf++ = qos_u32_to_uc(phy);

	return buf;
}

/******************************************************************************/
/*                                FW wrappers                                 */
/******************************************************************************/

static void set_common(
		const struct pp_qos_common_node_properties *conf,
		struct fw_set_common *common, uint32_t modified)
{
	uint32_t valid;

	valid = 0;

	if (QOS_BITS_IS_SET(modified, QOS_MODIFIED_BANDWIDTH_LIMIT)) {
		QOS_BITS_SET(valid, TSCD_NODE_CONF_BW_LIMIT);
		common->bw_limit = conf->bandwidth_limit;
	}

	if (QOS_BITS_IS_SET(modified, QOS_MODIFIED_SHARED_GROUP_ID)) {
		QOS_BITS_SET(valid, TSCD_NODE_CONF_SHARED_BWL_GROUP);
		common->shared_bw_group = conf->shared_bandwidth_group;
	}
	common->valid = valid;
}

static void
update_arbitration(uint32_t *valid,
		   const struct parent_node_properties *parent_node_prop,
		   struct fw_set_parent *parent)
{
	QOS_BITS_SET(*valid,
		     TSCD_NODE_CONF_FIRST_CHILD |
		     TSCD_NODE_CONF_LAST_CHILD  |
		     TSCD_NODE_CONF_FIRST_WRR_NODE);

	if (parent_node_prop->arbitration == PP_QOS_ARBITRATION_WSP)
		parent->first_wrr = 0;
	else
		parent->first_wrr = parent_node_prop->first_child_phy;

	parent->first = parent_node_prop->first_child_phy;
	parent->last = parent_node_prop->first_child_phy +
		parent_node_prop->num_of_children - 1;
}

static void set_parent(
		const struct pp_qos_parent_node_properties *conf,
		const struct parent_node_properties *parent_node_prop,
		struct fw_set_parent *parent,
		uint32_t modified)
{
	uint32_t valid;

	valid = 0;
	if (QOS_BITS_IS_SET(modified, QOS_MODIFIED_ARBITRATION))
		update_arbitration(&valid, parent_node_prop, parent);

	if (QOS_BITS_IS_SET(modified, QOS_MODIFIED_BEST_EFFORT)) {
		QOS_BITS_SET(valid, TSCD_NODE_CONF_BEST_EFFORT_ENABLE);
		parent->best_effort_enable = conf->best_effort_enable;
	}
	parent->valid = valid;
}

static void set_child(
		const struct pp_qos_child_node_properties *conf,
		struct fw_set_child *child,
		uint32_t modified)
{
	uint32_t valid = 0;

	if (QOS_BITS_IS_SET(modified, QOS_MODIFIED_BW_WEIGHT)) {
		QOS_BITS_SET(valid, TSCD_NODE_CONF_NODE_WEIGHT);
		child->bw_share = conf->wrr_weight;
	}

	/* Should be changed. Currently both are using bw_share variable */
	if (QOS_BITS_IS_SET(modified, QOS_MODIFIED_SHARED_GROUP_ID)) {
		QOS_BITS_SET(valid, TSCD_NODE_CONF_SHARED_BWL_GROUP);
		child->bw_share = conf->wrr_weight;
	}

	child->valid = valid;
}

static uint32_t *suspend_node(
		struct pp_qos_dev *qdev,
		unsigned int phy,
		unsigned int suspend,
		uint32_t *_cur,
		uint32_t **_prev,
		struct cmd_internal *cmd)
{
	struct qos_node *node;
	struct fw_set_common common;
	struct fw_set_parent parent;
	struct fw_set_child child;
	struct fw_set_sched sched;
	struct fw_set_queue queue;
	uint32_t *cur;

	cur = _cur;
	common.valid = TSCD_NODE_CONF_SUSPEND_RESUME;
	parent.valid = 0;
	child.valid = 0;
	sched.valid = 0;
	queue.valid = 0;
	common.suspend = suspend;

	node = get_node_from_phy(qdev->nodes, phy);
	QOS_ASSERT(!node_port(node), "Can't restart port %u\n", phy);

	if (node_sched(node)) {
		QOS_LOG_DEBUG("CMD_INTERNAL_SUSPEND_SCHED: %u\n", phy);
		cmd->base.pos = cur;
		cur = fw_write_set_sched_cmd(
				cur,
				phy,
				0,
				&common,
				&parent,
				&child,
				&sched);

		cmd_queue_put(
				qdev->drvcmds.pendq,
				cmd,
				cmd->base.len);
	} else if (node_queue(node)) {
		cmd->base.pos = cur;
		queue.rlm = node->data.queue.rlm;
		queue.is_alias_slave =
			(node->data.queue.alias_master_id == PP_QOS_INVALID_ID)
			? 0 : 1;

		QOS_LOG_DEBUG("CMD_INTERNAL_SUSPEND_QUEUE: %u\n", phy);
		cur = fw_write_set_queue_cmd(
				cur,
				phy,
				0,
				&common,
				&child,
				&queue);
		cmd_queue_put(
				qdev->drvcmds.pendq,
				cmd,
				cmd->base.len);
	}
	return cur;
}

#if 0
static void set_port_specific(
		const struct port_properties *conf,
		struct fw_set_port *port,
		uint32_t modified)
{
	uint32_t valid;

	valid = 0;
	if (QOS_BITS_IS_SET(modified, QOS_MODIFIED_RING_ADDRESS)) {
		QOS_BITS_SET(valid,
				PORT_CONF_RING_ADDRESS_HIGH |
				PORT_CONF_RING_ADDRESS_LOW);
		port->ring_addr = conf->ring_addr;
	}
	if (QOS_BITS_IS_SET(modified, QOS_MODIFIED_RING_SIZE)) {
		QOS_BITS_SET(valid, PORT_CONF_RING_SIZE);
		port->ring_size = conf->ring_size;
	}
	port->valid = valid;
}
#endif

static uint32_t *set_port_cmd_wrapper(
		struct fw_internal *fwdata,
		uint32_t *buf,
		const struct cmd_set_port *cmd,
		uint32_t flags)
{
	uint32_t modified;

	modified = cmd->modified;
	set_common(&cmd->prop.common, &fwdata->common, modified);
	set_parent(&cmd->prop.parent, &cmd->parent_node_prop,
		   &fwdata->parent, modified);
	fwdata->type_data.port.valid = 0;

	if (QOS_BITS_IS_SET(modified, QOS_MODIFIED_DISABLE)) {
		QOS_BITS_SET(fwdata->type_data.port.valid, PORT_CONF_ACTIVE);
		fwdata->type_data.port.active = !!cmd->prop.disable;
	}

	if (QOS_BITS_IS_SET(modified, QOS_MODIFIED_PORT_GREEN_THRESHOLD)) {
		QOS_BITS_SET(fwdata->type_data.port.valid,
			     PORT_CONF_GREEN_THRESHOLD);
		fwdata->type_data.port.green_threshold =
			cmd->prop.green_threshold;
	}

	if (QOS_BITS_IS_SET(modified, QOS_MODIFIED_PORT_YELLOW_THRESHOLD)) {
		QOS_BITS_SET(fwdata->type_data.port.valid,
			     PORT_CONF_YELLOW_THRESHOLD);
		fwdata->type_data.port.yellow_threshold =
			cmd->prop.yellow_threshold;
	}

	if ((fwdata->common.valid | fwdata->parent.valid |
				fwdata->type_data.port.valid) == 0) {
		QOS_LOG_DEBUG("IGNORING EMPTY CMD_TYPE_SET_PORT\n");
		return buf;
	}

	return fw_write_set_port_cmd(
			buf,
			cmd->phy,
			flags,
			&fwdata->common,
			&fwdata->parent,
			&fwdata->type_data.port);
}

static uint32_t *set_sched_cmd_wrapper(
		struct fw_internal *fwdata,
		uint32_t *buf,
		const struct cmd_set_sched *cmd,
		uint32_t flags)
{
	uint32_t modified;

	modified = cmd->modified;
	set_common(&cmd->prop.common, &fwdata->common, modified);
	set_parent(&cmd->prop.parent, &cmd->parent_node_prop,
		   &fwdata->parent, modified);
	set_child(&cmd->prop.child, &fwdata->child, modified);
	fwdata->type_data.sched.valid = 0;

	if ((fwdata->common.valid |
				fwdata->parent.valid |
				fwdata->child.valid) == 0) {
		QOS_LOG_DEBUG("IGNORING EMPTY CMD_TYPE_SET_SCHED\n");
		return buf;
	}

	return fw_write_set_sched_cmd(
			buf,
			cmd->phy,
			flags,
			&fwdata->common,
			&fwdata->parent,
			&fwdata->child,
			&fwdata->type_data.sched);
}

/*
 * Topologic changes like first/last child change or change of predecessors
 * will not be manifested through this path. They will be manifested through
 * CMD_TYPE_PARENT_CHANGE and CMD_TYPE_UPDATE_PREDECESSORS driver commands
 * which will call fw_write_set_node_cmd.
 * So the only think that needs to use fw_write_set_node_cmd in this path is
 * modify of suspend/resume
 *
 */
static uint32_t *set_queue_cmd_wrapper(
		struct fw_internal *fwdata,
		uint32_t *buf,
		const struct cmd_set_queue *cmd,
		uint32_t flags)
{
	uint32_t modified;
	uint32_t valid;
	uint32_t disable;
	struct fw_set_queue *queue;

	queue = &fwdata->type_data.queue;
	modified = cmd->modified;
	set_common(&cmd->prop.common, &fwdata->common, modified);
	set_child(&cmd->prop.child, &fwdata->child, modified);

	valid = 0;
	queue->rlm = cmd->prop.rlm;

	if (QOS_BITS_IS_SET(modified, QOS_MODIFIED_WRED_MAX_ALLOWED)) {
		QOS_BITS_SET(valid, WRED_QUEUE_CONF_MAX_ALLOWED);
		queue->queue_wred_max_allowed =
			cmd->prop.queue_wred_max_allowed;
	}

	if (QOS_BITS_IS_SET(modified, QOS_MODIFIED_BLOCKED)) {
		QOS_BITS_SET(valid, WRED_QUEUE_CONF_ACTIVE_Q);
		QOS_BITS_SET(valid, WRED_QUEUE_CONF_MAX_ALLOWED);
		queue->active = !cmd->prop.blocked;
		if (queue->active)
			queue->queue_wred_max_allowed =
				cmd->prop.queue_wred_max_allowed;
		else
			queue->queue_wred_max_allowed = 0;

	}

	disable = 0;
	if (QOS_BITS_IS_SET(modified, QOS_MODIFIED_WRED_ENABLE)) {
		QOS_BITS_SET(valid, WRED_QUEUE_CONF_DISABLE);
		if (!cmd->prop.wred_enable)
			QOS_BITS_SET(disable, BIT(0));
	}
	if (QOS_BITS_IS_SET(
				modified,
				QOS_MODIFIED_WRED_FIXED_DROP_PROB_ENABLE)) {
		QOS_BITS_SET(valid, WRED_QUEUE_CONF_DISABLE);
		if (cmd->prop.fixed_drop_prob_enable)
			QOS_BITS_SET(disable, BIT(3));
	}
	queue->disable = disable;

	if (QOS_BITS_IS_SET(modified, QOS_MODIFIED_WRED_FIXED_GREEN_PROB)) {
		QOS_BITS_SET(valid, WRED_QUEUE_CONF_FIXED_GREEN_DROP_P);
		queue->queue_wred_fixed_drop_prob_green =
			cmd->prop.queue_wred_fixed_drop_prob_green;
	}

	if (QOS_BITS_IS_SET(modified, QOS_MODIFIED_WRED_FIXED_YELLOW_PROB)) {
		QOS_BITS_SET(valid, WRED_QUEUE_CONF_FIXED_YELLOW_DROP_P);
		queue->queue_wred_fixed_drop_prob_yellow =
			cmd->prop.queue_wred_fixed_drop_prob_yellow;
	}

	if (QOS_BITS_IS_SET(modified, QOS_MODIFIED_WRED_MIN_YELLOW)) {
		QOS_BITS_SET(valid, WRED_QUEUE_CONF_MIN_AVG_YELLOW);
		queue->queue_wred_min_avg_yellow =
			cmd->prop.queue_wred_min_avg_yellow;
	}

	if (QOS_BITS_IS_SET(modified, QOS_MODIFIED_WRED_MAX_YELLOW)) {
		QOS_BITS_SET(valid, WRED_QUEUE_CONF_MAX_AVG_YELLOW);
		queue->queue_wred_max_avg_yellow =
			cmd->prop.queue_wred_max_avg_yellow;
	}

	if (QOS_BITS_IS_SET(modified, QOS_MODIFIED_WRED_SLOPE_YELLOW)) {
		QOS_BITS_SET(valid, WRED_QUEUE_CONF_SLOPE_YELLOW);
		queue->queue_wred_slope_yellow =
			cmd->prop.queue_wred_slope_yellow;
	}

	if (QOS_BITS_IS_SET(modified, QOS_MODIFIED_WRED_MIN_GREEN)) {
		QOS_BITS_SET(valid, WRED_QUEUE_CONF_MIN_AVG_GREEN);
		queue->queue_wred_min_avg_green =
			cmd->prop.queue_wred_min_avg_green;
	}

	if (QOS_BITS_IS_SET(modified, QOS_MODIFIED_WRED_MAX_GREEN)) {
		QOS_BITS_SET(valid, WRED_QUEUE_CONF_MAX_AVG_GREEN);
		queue->queue_wred_max_avg_green =
			cmd->prop.queue_wred_max_avg_green;
	}

	if (QOS_BITS_IS_SET(modified, QOS_MODIFIED_WRED_SLOPE_GREEN)) {
		QOS_BITS_SET(valid, WRED_QUEUE_CONF_SLOPE_GREEN);
		queue->queue_wred_slope_green =
			cmd->prop.queue_wred_slope_green;
	}

	if (QOS_BITS_IS_SET(modified, QOS_MODIFIED_WRED_MIN_GUARANTEED)) {
		QOS_BITS_SET(valid, WRED_QUEUE_CONF_MIN_GUARANTEED);
		queue->queue_wred_min_guaranteed =
			cmd->prop.queue_wred_min_guaranteed;
	}

	if ((valid | fwdata->common.valid | fwdata->child.valid) == 0) {
		QOS_LOG_DEBUG("IGNORING EMPTY CMD_TYPE_SET_QUEUE\n");
		return buf;
	}

	queue->is_alias_slave = cmd->is_alias_slave;
	queue->valid = valid;

	return fw_write_set_queue_cmd(
			buf,
			cmd->phy,
			flags,
			&fwdata->common,
			&fwdata->child,
			queue);
}

static uint32_t *parent_change_cmd_wrapper(
		struct fw_internal *fwdata,
		uint32_t *buf,
		const struct cmd_parent_change *cmd,
		uint32_t flags)
{
	update_arbitration(&fwdata->parent.valid,
			   &cmd->parent_node_prop,
			   &fwdata->parent);
	fwdata->common.valid = 0;
	if (cmd->type == TYPE_PORT) {
		fwdata->type_data.port.valid  = 0;
		return fw_write_set_port_cmd(
				buf,
				cmd->phy,
				flags,
				&fwdata->common,
				&fwdata->parent,
				&fwdata->type_data.port);
	} else {
		fwdata->child.valid = 0;
		fwdata->type_data.sched.valid = 0;
		return fw_write_set_sched_cmd(
				buf,
				cmd->phy,
				flags,
				&fwdata->common,
				&fwdata->parent,
				&fwdata->child,
				&fwdata->type_data.sched);
	}
}

static uint32_t *update_preds_cmd_wrapper(
		struct fw_internal *fwdata,
		uint32_t *buf,
		const struct cmd_update_preds *cmd,
		uint32_t flags)
{
	unsigned int i;

	fwdata->common.valid = 0;
	fwdata->child.valid = TSCD_NODE_CONF_PREDECESSOR_0 |
		TSCD_NODE_CONF_PREDECESSOR_1 |
		TSCD_NODE_CONF_PREDECESSOR_2 |
		TSCD_NODE_CONF_PREDECESSOR_3 |
		TSCD_NODE_CONF_PREDECESSOR_4 |
		TSCD_NODE_CONF_PREDECESSOR_5;
	for (i = 0; i < 6; ++i)
		fwdata->child.preds[i] = cmd->preds[i];

	if (cmd->node_type == TYPE_SCHED) {
		fwdata->type_data.sched.valid  = 0;
		fwdata->parent.valid = 0;
		return fw_write_set_sched_cmd(
				buf,
				cmd->phy,
				flags,
				&fwdata->common,
				&fwdata->parent,
				&fwdata->child,
				&fwdata->type_data.sched);
	} else {
		fwdata->type_data.queue.valid = 0;
		fwdata->type_data.queue.rlm = cmd->rlm;
		fwdata->type_data.queue.is_alias_slave = cmd->is_alias_slave;
		if (cmd->queue_port_changed) {
			fwdata->common.port_phy = cmd->queue_port;
			fwdata->common.valid |=
				TSCD_NODE_CONF_SET_PORT_TO_QUEUE;
		}

		return fw_write_set_queue_cmd(
				buf,
				cmd->phy,
				flags,
				&fwdata->common,
				&fwdata->child,
				&fwdata->type_data.queue);
	}
}

/*
 * Signal firmware to read and executes commands from cmd
 * buffer.
 * This is done by using the mailbox bundle.
 * Refer to driver's design document for further information
 * Since this is the only signal that is sent from driver to firmware
 * the value of DRV_SIGNAL is insignificant
 */
#define DRV_SIGNAL (2U)
void signal_uc(struct pp_qos_dev *qdev)
{
	if (PP_QOS_DEVICE_IS_ASSERT(qdev))
		return;
#ifndef NO_FW
	iowrite32(DRV_SIGNAL, qdev->fwcom.mbx_to_uc + 4);
	QOS_LOG_DEBUG("signal uc was called\n");
#endif
}

/******************************************************************************/
/*                                Engine                                      */
/******************************************************************************/

/*
 * Translation to user format and sanity checks on node info
 * obtained from firmware
 */
static void post_process_get_node_info(
		struct pp_qos_dev *qdev,
		struct cmd_get_node_info *cmd)
{
	struct pp_qos_node_info *info;
	struct hw_node_info_s *fw_info;
	struct qos_node *node;
	struct qos_node *child;
	uint16_t preds[6];
	uint32_t *fw_preds;
	unsigned int first;
	unsigned int phy;
	unsigned int id;
	unsigned int i;
	unsigned int num;
	unsigned int port;
	int reached_port;

	port = PPV4_QOS_INVALID;
	node = get_node_from_phy(qdev->nodes, cmd->phy);
	QOS_ASSERT(node_used(node), "Node %u is not used\n", cmd->phy);
	fw_info = (struct hw_node_info_s *)(qdev->stat);
	info = cmd->info;
	memset(info, 0xFF, sizeof(*info));

	QOS_ASSERT(node->type != TYPE_UNKNOWN,
			"Node %u has unknown type\n",
			cmd->phy);
	info->type = node->type - 1;
	info->is_internal = node_internal(node);

	if (node_parent(node)) {
		first = fw_info->first_child;
		if (first == 0) {
			QOS_ASSERT(fw_info->last_child == 0,
					"HW reports first child 0 but last child is %u\n",
					fw_info->last_child);
			first = QOS_INVALID_PHY;
			num = 0;
		} else {
			num = fw_info->last_child - first + 1;
			child = get_node_from_phy(qdev->nodes, first);
		}
		QOS_ASSERT(node->parent_prop.num_of_children == num,
				"Driver has %u as the number of children of node %u, while HW has %u\n",
				node->parent_prop.num_of_children,
				cmd->phy,
				num);

		QOS_ASSERT(num == 0 ||
				node->parent_prop.first_child_phy == first,
				"Driver has %u as the phy of node's %u first child, while HW has %u\n",
				node->parent_prop.first_child_phy,
				cmd->phy,
				first);

		for (i = 0; i < num; ++i) {
			phy = i + first;
			id = get_id_from_phy(qdev->mapping, phy);
			QOS_ASSERT(QOS_ID_VALID(id),
					"Child of %u with phy %u has no valid id\n",
					cmd->phy, phy);
			QOS_ASSERT(node_used(child),
					"Child node with phy %u and id %u is not used\n",
					phy,
					id);
			info->children[i].phy = phy;
			info->children[i].id = id;
			++child;
		}
	}

	if (!node_port(node)) {
		fill_preds(qdev->nodes, cmd->phy, preds, 6);
		fw_preds = &(fw_info->predecessor0);
		reached_port = 0;
		for (i = 0; i < 6; ++i) {
			QOS_ASSERT(preds[i] == *fw_preds,
					"Driver has %u as the %u predecessor of node %u, while HW has %u\n",
					preds[i],
					i,
					cmd->phy,
					*fw_preds);

			if (!reached_port) {
				info->preds[i].phy = preds[i];
				id = get_id_from_phy(qdev->mapping, preds[i]);
				QOS_ASSERT(QOS_ID_VALID(id),
						"Pred with phy %u has no valid id\n",
						preds[i]);
				info->preds[i].id = id;
				if (preds[i] <= qdev->max_port) {
					reached_port = 1;
					port = preds[i];
				}
			} else {
				info->preds[i].phy = PPV4_QOS_INVALID;
			}
			++fw_preds;
		}
	}

	if (node_queue(node)) {
		QOS_ASSERT(node->data.queue.rlm == fw_info->queue_physical_id,
				"Node %u physical queue is %u according to driver and %u according to HW\n",
				cmd->phy, node->data.queue.rlm,
				fw_info->queue_physical_id);
		QOS_ASSERT(port == fw_info->queue_port,
				"Driver has %u as %u port, while HW has %u\n",
				port,
				cmd->phy,
				fw_info->queue_physical_id);

		info->queue_physical_id = fw_info->queue_physical_id;
		info->port = fw_info->queue_port;
	}

	QOS_ASSERT(fw_info->bw_limit == node->bandwidth_limit,
			"Driver has %u as node's %u bandwidth limit, while HW has %u\n",
			node->bandwidth_limit,
			cmd->phy,
			fw_info->bw_limit);

	info->bw_limit = fw_info->bw_limit;
}

/*
 * Commands that are marked with POST_PROCESS reach
 * here for further processing before return to client
 */
static void post_process(struct pp_qos_dev *qdev, union driver_cmd *dcmd)
{
	enum cmd_type  type;
	struct pp_qos_queue_stat *qstat;
	struct pp_qos_port_stat *pstat;
	struct queue_stats_s *fw_qstat;
	struct port_stats_s *fw_pstat;
	struct system_stats_s *fw_sys_stat;

	if (PP_QOS_DEVICE_IS_ASSERT(qdev))
		return;

	type = dcmd->cmd.type;
	switch (type) {
	case CMD_TYPE_GET_QUEUE_STATS:
		fw_qstat = (struct queue_stats_s *)(qdev->stat);

		if (fw_qstat->queue_size_entries !=
		    fw_qstat->qmgr_num_queue_entries) {
			QOS_LOG_ERR("Queue %u occ mismatch (qmgr %u / wred %u)\n",
				    get_id_from_phy(qdev->mapping,
						    dcmd->queue_stats.phy),
				    fw_qstat->qmgr_num_queue_entries,
				    fw_qstat->queue_size_entries);
		}

		qstat = dcmd->queue_stats.stat;
		qstat->queue_packets_occupancy = fw_qstat->
			qmgr_num_queue_entries;
		qstat->queue_bytes_occupancy = fw_qstat->queue_size_bytes;
		qstat->total_packets_accepted = fw_qstat->total_accepts;
		qstat->total_packets_dropped = fw_qstat->total_drops;
		qstat->total_packets_red_dropped = fw_qstat->total_red_dropped;
		qstat->total_bytes_accepted =
			(((uint64_t)fw_qstat->total_bytes_added_high) << 32)
			| fw_qstat->total_bytes_added_low;
		qstat->total_bytes_dropped =
			(((uint64_t)fw_qstat->total_dropped_bytes_high) << 32)
			| fw_qstat->total_dropped_bytes_low;
		break;

	case CMD_TYPE_GET_PORT_STATS:
		fw_pstat = (struct port_stats_s *)(qdev->stat);
		pstat = dcmd->port_stats.stat;
		pstat->total_green_bytes = fw_pstat->total_green_bytes;
		pstat->total_yellow_bytes = fw_pstat->total_yellow_bytes;
		pstat->debug_back_pressure_status =
				fw_pstat->debug_back_pressure_status;
		pstat->debug_actual_packet_credit =
				fw_pstat->debug_actual_packet_credit;
		pstat->debug_actual_byte_credit =
				fw_pstat->debug_actual_byte_credit;
		break;

	case CMD_TYPE_GET_SYSTEM_INFO:
		fw_sys_stat = (struct system_stats_s *)qdev->stat;
		*(dcmd->sys_info.num_used) =
				fw_sys_stat->tscd_num_of_used_nodes;
		*(dcmd->sys_info.infinite_loop_error_occurred) =
				fw_sys_stat->tscd_infinite_loop_error_occurred;
		*(dcmd->sys_info.bwl_update_error_occurred) =
				fw_sys_stat->tscd_bwl_update_error_occurred;
		*(dcmd->sys_info.quanta) = fw_sys_stat->tscd_quanta;
		break;

	case CMD_TYPE_GET_NODE_INFO:
		post_process_get_node_info(qdev, &dcmd->node_info);
		break;

	default:
		QOS_ASSERT(0, "Unexpected cmd %d for post process\n", type);
		return;

	}
}

#define MAX_FW_CMD_SIZE sizeof(struct uc_qos_cmd_s)
#define NUM_OF_POLLS	1000000U

/*
 * Go over all commands on pending queue until cmd id
 * is changed or queue is empty
 * (refer to driver design document to learn more about cmd id).
 * On current implementation it is expected that pending queue contain
 * firmware commands for a single client command, therfore queue should
 * become empty before cmd id is changed.
 *
 * For each command wait until firmware signals
 * completion before continue to next command.
 * Completion status for each command is polled NUM_OF_POLLS
 * times. If command have not completed after all that polls
 * function asserts.
 *
 */
void check_completion(struct pp_qos_dev *qdev)
{
	union driver_cmd dcmd;

	volatile uint32_t *pos;
	uint32_t	val;
	size_t len;
	unsigned int idcur;
	int rc;
	unsigned int i;
	unsigned int popped;
	struct fw_internal *internals;

	if (PP_QOS_DEVICE_IS_ASSERT(qdev))
		return;

	popped = 0;
	idcur = 0;
	while (cmd_queue_peek(
			      qdev->drvcmds.pendq,
			      &dcmd.cmd,
			      sizeof(struct cmd)) == 0) {
		pos = dcmd.stub.cmd.pos;
		len = dcmd.cmd.len;
		++pos;
#ifndef NO_FW
		i = 0;
		val = qos_u32_from_uc(*pos);
		while ((val &
			(UC_CMD_FLAG_UC_DONE | UC_CMD_FLAG_UC_ERROR)) == 0) {
			cpu_relax();
			val = qos_u32_from_uc(*pos);
			++i;
			if (i == NUM_OF_POLLS) {
				print_fw_log(qdev->pdev);
				QOS_ASSERT(
					0,
					"FW is not responding, polling offset 0x%04tX for cmd type %s\n",
					(void *)pos -
					(void *)(qdev->fwcom.cmdbuf),
					cmd_str[dcmd.cmd.type]);
				return;
			}
		}
		if (val & UC_CMD_FLAG_UC_ERROR) {
			print_fw_log(qdev->pdev);
			QOS_ASSERT(0,
				   "FW signaled error, polling offset 0x%04tX, cmd type %s\n",
				   (void *)pos - (void *)(qdev->fwcom.cmdbuf),
				   cmd_str[dcmd.cmd.type]);
			return;
		}
#endif
		rc = cmd_queue_get(qdev->drvcmds.pendq, &dcmd.stub, len);
		QOS_ASSERT(rc == 0,
			   "Command queue does not contain a full command\n");
		if (dcmd.cmd.flags & CMD_FLAGS_POST_PROCESS)
			post_process(qdev, &dcmd);
		++popped;
	}

	internals = qdev->fwbuf;
	QOS_ASSERT(popped == internals->pushed,
		   "Expected to pop %u msgs from pending queue but popped %u\n",
		   internals->pushed, popped);
	QOS_ASSERT(cmd_queue_is_empty(qdev->drvcmds.pendq),
		   "Driver's pending queue is not empty\n");
	internals->pushed = 0;
	qdev->drvcmds.cmd_fw_id = 0;
}

/*
 * Take all commands from driver cmd queue, translate them to
 * firmware format and put them on firmware queue.
 * When finish signal firmware.
 */
void enqueue_cmds(struct pp_qos_dev *qdev)
{
	size_t len;
	int rc;
	uint32_t *cur;
	uint32_t *prev;
	uint32_t *start;
	int remain;
	uint32_t flags;
	union driver_cmd dcmd;
	struct cmd_internal	cmd_internal;
	struct fw_internal *internals;
	unsigned int pushed;
	unsigned int i;
	struct fw_set_common common = {0};
	struct fw_set_parent parent = {0};
	struct fw_set_port port = {0};
	unsigned int id;
	u16 *rlms;
	u16 *rlm_ids;
	u32 num_queues = 0;
	u32 max_allowed_addr_phy;
	u32 cmdbuf_sz;

	if (PP_QOS_DEVICE_IS_ASSERT(qdev))
		return;

	start = qdev->fwcom.cmdbuf;
	cmdbuf_sz = qdev->fwcom.cmdbuf_sz;

	pushed = 0;
	cur = start;
	prev = start;
	*cur++ = qos_u32_to_uc(UC_QOS_CMD_MULTIPLE_COMMANDS);
	*cur++ = qos_u32_to_uc(UC_CMD_FLAG_IMMEDIATE);
	*cur++ = qos_u32_to_uc(1);
	*cur = qos_u32_to_uc(((uintptr_t)(cur) - (uintptr_t)start +
				4 + FW_CMD_BUFFER_DCCM_START) & 0xFFFFFFFF);
	++cur;

	remain = cmdbuf_sz - ((uintptr_t)cur - (uintptr_t)start);
	internals = qdev->fwbuf;
	flags = UC_CMD_FLAG_IMMEDIATE;

	cmd_init(
			qdev,
			&(cmd_internal.base),
			CMD_TYPE_INTERNAL,
			sizeof(cmd_internal), 0);
	common.valid = TSCD_NODE_CONF_SUSPEND_RESUME;
	parent.valid = 0;
	port.valid = 0;

	if (!internals->ongoing) {
		for (i = 0; i < internals->num_suspend_ports; ++i) {
			prev = cur;
			QOS_LOG_DEBUG("CMD_INTERNAL_SUSPEND_TREE port: %u\n",
				      internals->suspend_ports[i]);

			cur = fw_suspend_tree_cmd(prev,
						  internals->suspend_ports[i],
						  flags);
			if (cur != prev) {
				cmd_internal.base.pos = prev;
				cmd_queue_put(
						qdev->drvcmds.pendq,
						&cmd_internal,
						cmd_internal.base.len);
				++pushed;
				remain = cmdbuf_sz - ((uintptr_t)cur - (uintptr_t)start);
			}
		}
		if (pushed)
			internals->ongoing = 1;
	}

	while ((remain >= MAX_FW_CMD_SIZE) &&
			cmd_queue_peek(
				qdev->drvcmds.cmdq,
				&dcmd.cmd,
				sizeof(struct cmd)) == 0) {
		len = dcmd.cmd.len;
		rc = cmd_queue_get(qdev->drvcmds.cmdq, &dcmd.stub, len);
		QOS_ASSERT(rc == 0,
				"Command queue does not contain a full command\n");
		prev = cur;
		dcmd.stub.cmd.pos = cur;
		switch (dcmd.cmd.type) {
		case CMD_TYPE_INIT_LOGGER:
			cur = fw_write_init_logger_cmd(
					prev,
					&dcmd.init_logger,
					flags);
			break;

		case CMD_TYPE_INIT_QOS:
			cur = fw_write_init_qos_cmd(
					prev,
					&dcmd.init_qos,
					flags);
			break;

		case CMD_TYPE_MOVE:
			if (dcmd.move.node_type == TYPE_QUEUE)
				cur = fw_write_move_queue_cmd(
						prev,
						&dcmd.move,
						flags);
			else
				cur = fw_write_move_sched_cmd(
						prev,
						&dcmd.move,
						flags);
			break;

		case CMD_TYPE_PARENT_CHANGE:
			cur = parent_change_cmd_wrapper(
					internals,
					prev,
					&dcmd.parent_change,
					flags);
			break;

		case CMD_TYPE_UPDATE_PREDECESSORS:
			cur = update_preds_cmd_wrapper(
					internals,
					prev,
					&dcmd.update_preds,
					flags);
			break;

		case CMD_TYPE_ADD_PORT:
			cur = fw_write_add_port_cmd(
					prev,
					&dcmd.add_port,
					flags);
			break;

		case CMD_TYPE_SET_PORT:
			cur = set_port_cmd_wrapper(
					internals,
					prev,
					&dcmd.set_port,
					flags);
			break;

		case CMD_TYPE_REMOVE_PORT:
			cur = fw_write_remove_port_cmd(
					prev,
					&dcmd.remove_node,
					flags);
			break;

		case CMD_TYPE_ADD_SCHED:
			cur = fw_write_add_sched_cmd(
					prev,
					&dcmd.add_sched,
					flags);
			break;

		case CMD_TYPE_ADD_QUEUE:
			cur = fw_write_add_queue_cmd(
					prev,
					&dcmd.add_queue,
					flags);
			break;

		case CMD_TYPE_REMOVE_QUEUE:
			cur = fw_write_remove_queue_cmd(
					prev,
					&dcmd.remove_node,
					flags);
			break;

		case CMD_TYPE_REMOVE_SCHED:
			cur = fw_write_remove_sched_cmd(
					prev,
					&dcmd.remove_node,
					flags);
			break;

		case CMD_TYPE_SET_SCHED:
			cur = set_sched_cmd_wrapper(
					internals,
					prev,
					&dcmd.set_sched,
					flags);
			break;

		case CMD_TYPE_SET_QUEUE:
			cur = set_queue_cmd_wrapper(
					internals,
					prev,
					&dcmd.set_queue,
					flags);
			break;

		case CMD_TYPE_REMOVE_NODE:
			QOS_ASSERT(0,
					"Did not expect CMD_TYPE_REMOVE_NODE\n");
			break;

		case CMD_TYPE_GET_QUEUE_STATS:
			cur = fw_write_get_queue_stats(
					prev, &dcmd.queue_stats, flags);
			break;

		case CMD_TYPE_GET_PORT_STATS:
			cur = fw_write_get_port_stats(
					prev, &dcmd.port_stats, flags);
			break;

		case CMD_TYPE_GET_SYSTEM_INFO:
			cur = fw_write_get_system_info(
					prev, &dcmd.sys_info, flags);
			break;

		case CMD_TYPE_ADD_SHARED_GROUP:
		case CMD_TYPE_SET_SHARED_GROUP:
			cur = fw_write_set_shared_group(
					prev,
					dcmd.cmd.type,
					&dcmd.set_shared_group,
					flags);
			break;

		case CMD_TYPE_REMOVE_SHARED_GROUP:
			cur = fw_write_remove_shared_group(
					prev,
					&dcmd.remove_shared_group,
					flags);
			break;
		case CMD_TYPE_PUSH_DESC:
			cur = fw_write_push_desc(
					prev,
					&dcmd.pushd,
					flags);
			break;

		case CMD_TYPE_GET_NODE_INFO:
			cur = fw_write_get_node_info(
					prev,
					&dcmd.node_info,
					flags);
			break;

		case CMD_TYPE_READ_TABLE_ENTRY:
			cur = fw_read_table_entry(prev, &dcmd.read_table_entry,
						  flags);
			break;

		case CMD_TYPE_FLUSH_QUEUE:
			cur = fw_write_flush_queue_cmd(
					prev,
					&dcmd.flush_queue,
					flags);
			break;

		default:
			QOS_ASSERT(0, "Unexpected msg type %d\n",
					dcmd.cmd.type);
			return;
		}
		if (cur != prev) {
			cmd_queue_put(
					qdev->drvcmds.pendq,
					&dcmd.stub,
					dcmd.stub.cmd.len);
			++pushed;
			remain = cmdbuf_sz - ((uintptr_t)cur - (uintptr_t)start);
		}
	}

	if (cmd_queue_is_empty(qdev->drvcmds.cmdq)) {
		common.suspend = 0;
		for (i = internals->suspend_ports_index;
		     i < internals->num_suspend_ports; ++i) {
			/* In case port was suspended, and it was removed,
			 * don't Resume it (Otherwise driver and fw will be
			 * out of sync) */
			id = get_id_from_phy(qdev->mapping,
					     internals->suspend_ports[i]);
			if (!QOS_ID_VALID(id))
				continue;

			// Verify space for resume command
			if (remain < MAX_FW_CMD_SIZE) {
				internals->suspend_ports_index = i;
				QOS_LOG_DEBUG("reentry required: remain %d, suspend ind %u, num suspend %u\n",
					      remain,
					      internals->suspend_ports_index,
					      internals->num_suspend_ports);
				goto signal_uc;
			}

			/* Update BWL buffer before tree update */
			update_bwl_buffer(qdev, internals->suspend_ports[i]);

			prev = cur;
			QOS_LOG_DEBUG("CMD_INTERNAL_UPDATE_PORT_TREE port:%u\n",
					internals->suspend_ports[i]);

			rlms = (u16 *)kzalloc(NUM_OF_QUEUES * sizeof(u16),
					      GFP_ATOMIC);
			if (!rlms) {
				pr_err("queue array memory alloc failed\n");
				continue;
			}

			rlm_ids = (u16 *)kzalloc(NUM_OF_QUEUES * sizeof(u16),
						 GFP_ATOMIC);
			if (!rlm_ids) {
				kfree(rlms);
				pr_err("rlm_ids memory alloc failed\n");
				continue;
			}

			rc = get_active_port_phy_queues(qdev, id, rlms,
							rlm_ids, NUM_OF_QUEUES,
							&num_queues);
			if (rc) {
				pr_err("Failed fetching port queues\n");
				kfree(rlms);
				kfree(rlm_ids);
				continue;
			}

			rc = store_port_queue_max_allowed(qdev, id, rlms,
							  rlm_ids, num_queues);
			if (rc) {
				pr_err("Failed to set q's max allowed buf\n");
				kfree(rlms);
				kfree(rlm_ids);
				continue;
			}

			max_allowed_addr_phy =
				(u32)qdev->hwconf.max_allowed_ddr_phys;

			cur = fw_update_tree_cmd(prev,
						 internals->suspend_ports[i],
						 flags, &common,
						 &parent, &port,
						 max_allowed_addr_phy,
						 rlms, num_queues,
						 NUM_OF_QUEUES);
			kfree(rlms);
			kfree(rlm_ids);

			if (cur != prev) {
				cmd_internal.base.pos = prev;
				cmd_queue_put(
						qdev->drvcmds.pendq,
						&cmd_internal,
						cmd_internal.base.len);
				++pushed;
				remain = cmdbuf_sz - ((uintptr_t)cur - (uintptr_t)start);
			}
		}
		/* No pending suspended ports/moving nodes - Reset counters */
		internals->suspend_ports_index = 0;
		internals->num_suspend_ports = 0;
		internals->ongoing = 0;
	}

signal_uc:
	if (pushed) {
		internals->pushed = pushed;
		prev += 1;
		flags = qos_u32_from_uc(*prev);
		QOS_BITS_SET(flags, UC_CMD_FLAG_MULTIPLE_COMMAND_LAST);
		*prev = qos_u32_to_uc(flags);

		/* Read start & end of command buffer to avoid race with FW */
		qos_u32_from_uc(*(volatile uint32_t *)start);

		if (cur)
			qos_u32_from_uc(*(volatile uint32_t *)cur);

		signal_uc(qdev);
	}
}
#endif
