/*
 * pp_qos_fw.c
 * Description: Packet Processor QoS Driver
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2017-2020 Intel Corporation
 */

#define pr_fmt(fmt) "[PP_QOS_FW]:%s:%d: " fmt, __func__, __LINE__

#include <linux/time.h>
#include <linux/bitops.h>
#include <linux/init.h>
#include "infra.h"
#include "pp_common.h"
#include "pp_qos_common.h"
#include "pp_qos_utils.h"
#include "pp_qos_fw.h"
#include "pp_regs.h"
#include "elf_parser.h"

#define FW_DCCM_START                   0xF0000000
#define FW_CMD_BUFFER_DCCM_START        0xF0004000
#define MAX_FW_CMD_SIZE                 (sizeof(union uc_qos_cmd_s))
#define ELF_MAX_SECTIONS                64
#define QOS_EWSP_DEFAULT_BYTE_THRESHOLD 8000
#define QOS_EWSP_DEFAULT_TIMEOUT_US     128

static bool qos_fw_log;

struct ppv4_qos_fw_hdr {
	u32 major;
	u32 minor;
	u32 build;
};

struct ppv4_qos_fw_sec {
	u32 dst;
	u32 size;
};

enum fw_soc_rev {
	FW_SOC_REV_A = 0,
	FW_SOC_REV_B,
	FW_SOC_REV_UNKNOWN
};

static enum fw_soc_rev __fw_soc_rev_get(void)
{
	enum pp_silicon_step rev = pp_silicon_step_get();

	switch (rev) {
	case PP_SSTEP_A:
		return FW_SOC_REV_A;
	case PP_SSTEP_B:
		return FW_SOC_REV_B;
	};

	return FW_SOC_REV_UNKNOWN;
}

static void copy_section_ddr(unsigned long dst, const void *_src, u32 size)
{
	u32 i;
	const u32 *src;

	src = (u32 *)_src;

	for (i = size; i > 3; i -= 4) {
		*(u32 *)dst = cpu_to_le32(*src++);
		dst += 4;
	}

	/* Section size must be aligned to 2 */
	if (i == 1 || i == 3)
		QOS_LOG_ERR("Size %u not aligned to 2 (i=%u)\n", size, i);

	/* Copy last u16 if exists */
	if (i == 2)
		*(u32 *)dst = cpu_to_le16(*(u16 *)src);
}

static void copy_section_qos(unsigned long dst, const void *_src, u32 size)
{
	u32 i;
	const u32 *src;

	src = (u32 *)_src;

	for (i = size; i > 3; i -= 4) {
		PP_REG_WR32(dst, cpu_to_le32(*src++));
		dst += 4;
	}

	/* Section size must be aligned to 2 */
	if (i == 1 || i == 3)
		QOS_LOG_ERR("Size %u not aligned to 2 (i=%u)\n", size, i);

	/* Copy last u16 if exists */
	if (i == 2)
		PP_REG_WR32(dst, cpu_to_le16(*(u16 *)src));
}

static void calc_elf_sections_sizes(struct elf_sec secs[], u16 num_secs,
				    u32 *total_sz, u32 *data_sz, u32 *stack_sz)
{
	u16 ind;

	for (ind = 0; ind < num_secs; ind++) {
		/* the addr field isn't in sequence after the previous one,
		 * and we must keep all sections in same address(offset)
		 * as defined by the elf file, so the total size is where
		 * the last section that should be copied ends.
		 */
		if (secs[ind].need_copy)
			*total_sz = secs[ind].addr + secs[ind].size;

		if (!strncmp(secs[ind].name, ".data", 5))
			*data_sz = secs[ind].size;
		else if (!strncmp(secs[ind].name, ".stack", 6))
			*stack_sz = secs[ind].size;
	}
}

static void copy_sections(struct elf_sec secs[],
			  u16 num_secs,
			  void *virt_txt,
			  void *virt_data,
			  dma_addr_t phys_txt,
			  dma_addr_t phys_data,
			  dma_addr_t phys_stack,
			  unsigned long ivt,
			  u32 is_in_dccm)
{
	u16 ind;
	void *src;
	unsigned long dst;
	bool copy_to_dccm = false;
	u32 ivt_tbl[3] = {0};

	for (ind = 0; ind < num_secs; ind++) {
		if (!secs[ind].need_copy)
			continue;

		src = secs[ind].data;

		if (!strncmp(secs[ind].name, ".data", 5)) {
			dst = (unsigned long)virt_data;
			copy_to_dccm = !!is_in_dccm;
		} else {
			dst = (unsigned long)(virt_txt) + secs[ind].addr;
			copy_to_dccm = false;
		}

		if (!strncmp(secs[ind].name, ".vectors", 8)) {
			ivt_tbl[0] = le32_to_cpu(*(u32 *)src) + phys_txt;
			ivt_tbl[1] = phys_stack;
			ivt_tbl[2] = phys_data;

			/* Copy to QoS */
			PP_REG_WR32_INC(ivt, ivt_tbl[0]);
			PP_REG_WR32_INC(ivt, ivt_tbl[1]);
			PP_REG_WR32_INC(ivt, ivt_tbl[2]);
		}

		pr_debug("Section %s: COPY %llu bytes from %#lx[%#x %#x..] to %#lx\n",
			 secs[ind].name, secs[ind].size,
			 (unsigned long)src, *(u32 *)src,
			 *(u32 *)((unsigned long)src + 4), dst);

		if (copy_to_dccm)
			copy_section_qos(dst, src, secs[ind].size);
		else
			copy_section_ddr(dst, src, secs[ind].size);
	}
}

/* This function loads the firmware.
 * The firmware is built from a header which holds the major, minor
 * and build numbers.
 * Following the header are sections. Each section is composed of
 * header which holds the memory destination where this section's
 * data should be copied and the size of this section in bytes.
 * After the header comes section's data which is a stream of uint32
 * words.
 * The memory destination on section header designates offset relative
 * to either ddr (a.k.a external) or qos (a.k.a) spaces. Offsets higher
 * than FW_DDR_LOWEST refer to ddr space.
 *
 * Firmware is little endian.
 *
 * When firmware runs it writes 0xCAFECAFE to offset FW_OK_OFFSET of ddr
 * space.
 */
s32 do_load_firmware(struct pp_qos_dev *qdev,
		     const struct ppv4_qos_fw *fw,
		     unsigned long qos_uc_base,
		     unsigned long wakeuc)
{
	u32 alloc_size, raw_version;
	u32 align = 4;
	struct elf_sec *secs;
	u16 num_secs;
	u32 total_sz = 0;
	u32 data_sz = 0;
	u32 stack_sz = 0;
	void *virt_txt;
	void *virt_data;
	dma_addr_t phys_txt;
	dma_addr_t phys_data;
	dma_addr_t phys_stack; /* End of stack! */
	s32 ret = 0;
	struct fw_sec_info *sec_info;
	struct fw_ver host_fwver;

	sec_info = &qdev->init_params.fw_sec_data_stack;
	secs = kmalloc_array(ELF_MAX_SECTIONS, sizeof(struct elf_sec),
			     GFP_KERNEL);
	if (!secs)
		return -ENOMEM;

	if (elf_parse(fw->data, fw->size, secs, ELF_MAX_SECTIONS, &num_secs)) {
		QOS_LOG_ERR("ELF parse error!\n");
		kfree(secs);
		return -ENOEXEC;
	}

	calc_elf_sections_sizes(secs, num_secs, &total_sz, &data_sz, &stack_sz);

	/* No room for data and stack as defined from DTS */
	if ((data_sz + stack_sz) > sec_info->max_size) {
		QOS_LOG_ERR("Need to alloc %u bytes while dts limits to %u\n",
			    (data_sz + stack_sz), sec_info->max_size);
		kfree(secs);
		return -ENOMEM;
	}

	QOS_LOG_DEBUG("=====> fw_data_stack_off %d, %d, %d\n",
		      sec_info->is_in_dccm,
		      sec_info->dccm_offset,
		      sec_info->max_size);

	/* Is data stack sections in DCCM */
	if (sec_info->is_in_dccm) {
		alloc_size = ALIGN(total_sz, align);
	} else {
		alloc_size = ALIGN(total_sz, align) +
			     ALIGN(data_sz, align) +
			     ALIGN(stack_sz, align);
	}

	virt_txt = pp_dma_alloc(alloc_size, GFP_KERNEL, &phys_txt, true);
	if (!virt_txt) {
		pr_err("Could not allocate %u bytes for fw\n", alloc_size);
		kfree(secs);
		return -ENOMEM;
	}

	if (sec_info->is_in_dccm) {
		phys_data = 0xF0000000 + sec_info->dccm_offset;
		virt_data = (void *)(qos_uc_base + sec_info->dccm_offset);
	} else {
		phys_data = phys_txt + ALIGN(total_sz, align);
		virt_data = (void *)((unsigned long)virt_txt +
				     ALIGN(total_sz, align));
	}

	phys_stack =
		phys_data + ALIGN(data_sz, align) + ALIGN(stack_sz, align) - 4;

	pr_debug("Text %#lx [%#lx] (%d), Data %#lx [%#lx] (%d), Stack %#lx (%d)\n",
		 (unsigned long)phys_txt, (unsigned long)(virt_txt), total_sz,
		 (unsigned long)phys_data, (unsigned long)virt_data, data_sz,
		 (unsigned long)phys_stack, stack_sz);

	copy_sections(secs, num_secs, virt_txt, virt_data, phys_txt,
		      phys_data, phys_stack, qos_uc_base + 0x8000,
		      sec_info->is_in_dccm);

	kfree(secs);

	/* Write back all elf sections */
	pp_cache_writeback(virt_txt, alloc_size);

	/* Set Guard Regions */
	PP_REG_WR32((ulong)qdev->init_params.qos_uc_base
		    + PPV4_QOS_CMD_BUF_GUARD_OFFSET,
		    PPV4_QOS_UC_GUARD_MAGIC);

	ret = wake_uc(wakeuc);
	if (ret) {
		pr_err("wake_uc failed\n");
		return ret;
	}
	QOS_LOG_DEBUG("waked fw\n");

	if (pp_reg_poll_x(qdev->init_params.fwcom.cmdbuf, U32_MAX,
			  FW_OK_CODE, PP_REG_MAX_RETRIES)) {
		QOS_LOG_ERR("FW OK value is 0x%x\n", FW_OK_CODE);
		return -ENODEV;
	}

	QOS_LOG_DEBUG("FW is running :)\n");

	PP_REG_WR32((ulong)qdev->init_params.fwcom.cmdbuf, 0);

	host_fwver.major = UC_VERSION_MAJOR;
	host_fwver.minor = UC_VERSION_MINOR;
	host_fwver.build = UC_VERSION_BUILD;

	raw_version = PP_REG_RD32((ulong)qdev->init_params.fwcom.cmdbuf + 4);
	qdev->fwver.major = raw_version & 0xF;
	qdev->fwver.minor = (raw_version & 0xFF00) >> 8;
	qdev->fwver.build = (raw_version & 0xFF0000) >> 16;

	QOS_LOG_INFO("FW [v %u.%u.%u] is running soc rev %d)\n",
		     qdev->fwver.major, qdev->fwver.minor,
		     qdev->fwver.build, __fw_soc_rev_get());

	if ((host_fwver.major != qdev->fwver.major) ||
	    (host_fwver.minor != qdev->fwver.minor)) {
		QOS_LOG_ERR("Ver mismatch: FW [%u.%u.%u]. Host [%u.%u.%u]\n",
			    qdev->fwver.major, qdev->fwver.minor,
			    qdev->fwver.build, host_fwver.major,
			    host_fwver.minor, host_fwver.build);
	}

	return ret;
}

s32 wake_uc(unsigned long wake_addr)
{
	u32 wake_status;

	wake_status = PP_REG_RD32(wake_addr);
	if (wake_status == 4) {
		pr_err("uC already waked\n");
		return -EINVAL;
	}

	PP_REG_WR32(wake_addr, 4);

	return 0;
}

bool is_uc_awake(unsigned long wake_addr)
{
	u32 wake_status;

	wake_status = PP_REG_RD32(wake_addr);
	if (wake_status == 4)
		return true;

	return false;
}

#ifndef PP_QOS_DISABLE_CMDQ
#define CMD_INT_RESPONSE_SZ        4

/* @brief driver internal command id - for debug */
enum cmd_type {
	CMD_TYPE_INIT_LOGGER,
	CMD_TYPE_INIT_QOS,
	CMD_TYPE_MOVE_QUEUE,
	CMD_TYPE_MOVE_SCHED,
	CMD_TYPE_ADD_PORT,
	CMD_TYPE_SET_PORT,
	CMD_TYPE_REM_PORT,
	CMD_TYPE_ADD_SCHED,
	CMD_TYPE_SET_SCHED,
	CMD_TYPE_REM_SCHED,
	CMD_TYPE_ADD_QUEUE,
	CMD_TYPE_SET_QUEUE,
	CMD_TYPE_REM_QUEUE,
	CMD_TYPE_GET_QUEUE_STATS,
	CMD_TYPE_GET_QM_STATS,
	CMD_TYPE_SET_AQM_SF,
	CMD_TYPE_CLK_UPDATE,
	CMD_TYPE_NODES_REFRESH,
	CMD_TYPE_SET_CODEL_CFG,
	CMD_TYPE_GET_CODEL_STATS,
	CMD_TYPE_GET_PORT_STATS,
	CMD_TYPE_PUSH_DESC,
	CMD_TYPE_POP_DESC,
	CMD_TYPE_MCDMA_COPY,
	CMD_TYPE_GET_NODE_INFO,
	CMD_TYPE_READ_TABLE_ENTRY,
	CMD_TYPE_REM_SHARED_GROUP,
	CMD_TYPE_ADD_SHARED_GROUP,
	CMD_TYPE_SET_SHARED_GROUP,
	CMD_TYPE_FLUSH_QUEUE,
	CMD_TYPE_GET_SYSTEM_INFO,
	CMD_TYPE_UPDATE_PORT_TREE,
	CMD_TYPE_SUSPEND_PORT_TREE,
	CMD_TYPE_GET_QUANTA,
	CMD_TYPE_SET_WSP_HELPER,
	CMD_TYPE_GET_WSP_HELPER_STATS,
	CMD_TYPE_SET_MOD_LOG_BMAP,
	CMD_TYPE_GET_MOD_LOG_BMAP,
	CMD_TYPE_INTERNAL,
	CMD_TYPE_LAST
};

static const char *const cmd_str[] = {
	[CMD_TYPE_INIT_LOGGER] = "CMD_TYPE_INIT_LOGGER",
	[CMD_TYPE_INIT_QOS] = "CMD_TYPE_INIT_QOS",
	[CMD_TYPE_MOVE_QUEUE] = "CMD_TYPE_MOVE_QUEUE",
	[CMD_TYPE_MOVE_SCHED] = "CMD_TYPE_MOVE_SCHED",
	[CMD_TYPE_ADD_PORT] = "CMD_TYPE_ADD_PORT",
	[CMD_TYPE_SET_PORT] = "CMD_TYPE_SET_PORT",
	[CMD_TYPE_REM_PORT] = "CMD_TYPE_REM_PORT",
	[CMD_TYPE_ADD_SCHED] = "CMD_TYPE_ADD_SCHED",
	[CMD_TYPE_SET_SCHED] = "CMD_TYPE_SET_SCHED",
	[CMD_TYPE_REM_SCHED] = "CMD_TYPE_REM_SCHED",
	[CMD_TYPE_ADD_QUEUE] = "CMD_TYPE_ADD_QUEUE",
	[CMD_TYPE_SET_QUEUE] = "CMD_TYPE_SET_QUEUE",
	[CMD_TYPE_REM_QUEUE] = "CMD_TYPE_REM_QUEUE",
	[CMD_TYPE_GET_QUEUE_STATS] = "CMD_TYPE_GET_QUEUE_STATS",
	[CMD_TYPE_GET_QM_STATS] = "CMD_TYPE_GET_QM_STATS",
	[CMD_TYPE_SET_AQM_SF] = "CMD_TYPE_SET_AQM_SF",
	[CMD_TYPE_CLK_UPDATE] = "CMD_TYPE_CLK_UPDATE",
	[CMD_TYPE_NODES_REFRESH] = "CMD_TYPE_NODES_REFRESH",
	[CMD_TYPE_SET_CODEL_CFG] = "CMD_TYPE_SET_CODEL_CFG",
	[CMD_TYPE_GET_CODEL_STATS] = "CMD_TYPE_GET_CODEL_STATS",
	[CMD_TYPE_GET_PORT_STATS] = "CMD_TYPE_GET_PORT_STATS",
	[CMD_TYPE_PUSH_DESC] = "CMD_TYPE_PUSH_DESC",
	[CMD_TYPE_POP_DESC] = "CMD_TYPE_POP_DESC",
	[CMD_TYPE_MCDMA_COPY] = "CMD_TYPE_MCDMA_COPY",
	[CMD_TYPE_GET_NODE_INFO] = "CMD_TYPE_GET_NODE_INFO",
	[CMD_TYPE_READ_TABLE_ENTRY] = "CMD_TYPE_READ_TABLE_ENTRY",
	[CMD_TYPE_REM_SHARED_GROUP] = "CMD_TYPE_REM_SHARED_GROUP",
	[CMD_TYPE_ADD_SHARED_GROUP] = "CMD_TYPE_ADD_SHARED_GROUP",
	[CMD_TYPE_SET_SHARED_GROUP] = "CMD_TYPE_SET_SHARED_GROUP",
	[CMD_TYPE_FLUSH_QUEUE] = "CMD_TYPE_FLUSH_QUEUE",
	[CMD_TYPE_GET_SYSTEM_INFO] = "CMD_TYPE_GET_SYSTEM_INFO",
	[CMD_TYPE_UPDATE_PORT_TREE] = "CMD_TYPE_UPDATE_PORT_TREE",
	[CMD_TYPE_SUSPEND_PORT_TREE] = "CMD_TYPE_SUSPEND_PORT_TREE",
	[CMD_TYPE_GET_QUANTA] = "CMD_TYPE_GET_QUANTA",
	[CMD_TYPE_SET_WSP_HELPER] = "CMD_TYPE_SET_WSP_HELPER",
	[CMD_TYPE_GET_WSP_HELPER_STATS] = "CMD_TYPE_GET_WSP_HELPER_STATS",
	[CMD_TYPE_SET_MOD_LOG_BMAP] = "CMD_TYPE_SET_MOD_LOG_BMAP",
	[CMD_TYPE_GET_MOD_LOG_BMAP] = "CMD_TYPE_GET_MOD_LOG_BMAP",
	[CMD_TYPE_INTERNAL] = "CMD_TYPE_INTERNAL"
};

/******************************************************************************/
/*                         Driver commands structures	                      */
/******************************************************************************/
struct cmd {
	u32           id; /* command sequence id - for debug */
#define CMD_FLAGS_POST_PROCESS_EXT BIT(0) /* Post process data from ext buff */
#define CMD_FLAGS_POST_PROCESS_INT BIT(1) /* Post process data from int buff */
	u32           flags;
	enum cmd_type type; /* command type - for debug */
	size_t        len; /* Command length */
	void          *response; /* response address */
	u32           int_response_offset; /* response offset in command */
	u32           response_sz; /* num bytes of response */
	unsigned long pos; /* Command offset in FW buffer */
} __attribute__((packed));

struct cmd_internal {
	struct cmd base;
} __attribute__((packed));

struct cmd_init_logger {
	struct cmd base;
	struct fw_cmd_init_logger fw;
} __attribute__((packed));

struct cmd_init_qos {
	struct cmd base;
	struct fw_cmd_init_qos fw;
} __attribute__((packed));

struct cmd_move_sched {
	struct cmd base;
	struct fw_cmd_move_sched fw;
} __attribute__((packed));

struct cmd_move_queue {
	struct cmd base;
	struct fw_cmd_move_queue fw;
} __attribute__((packed));

struct cmd_remove_queue {
	struct cmd base;
	struct fw_cmd_remove_queue fw;
} __attribute__((packed));

struct cmd_remove_port {
	struct cmd base;
	struct fw_cmd_remove_port fw;
} __attribute__((packed));

struct cmd_remove_sched {
	struct cmd base;
	struct fw_cmd_remove_sched fw;
} __attribute__((packed));

struct cmd_add_port {
	struct cmd base;
	struct fw_cmd_add_port fw;
} __attribute__((packed));

struct cmd_set_port {
	struct cmd base;
	struct fw_cmd_set_port fw;
} __attribute__((packed));

struct cmd_add_sched {
	struct cmd base;
	struct fw_cmd_add_sched fw;
} __attribute__((packed));

struct cmd_set_sched {
	struct cmd base;
	struct fw_cmd_set_sched fw;
} __attribute__((packed));

struct cmd_add_queue {
	struct cmd base;
	struct fw_cmd_add_queue fw;
} __attribute__((packed));

struct cmd_set_queue {
	struct cmd base;
	struct fw_cmd_set_queue fw;
} __attribute__((packed));

struct cmd_flush_queue {
	struct cmd base;
	struct fw_cmd_flush_queue fw;
} __attribute__((packed));

struct cmd_set_aqm_sf {
	struct cmd base;
	struct fw_cmd_set_aqm_sf fw;
} __attribute__((packed));

struct cmd_clk_update {
	struct cmd base;
	struct fw_cmd_clk_update fw;
} __attribute__((packed));

struct cmd_nodes_refresh {
	struct cmd base;
	struct fw_cmd_refresh_nodes fw;
} __attribute__((packed));

struct cmd_set_codel_cfg {
	struct cmd base;
	struct fw_cmd_set_codel fw;
} __attribute__((packed));

struct cmd_push_desc {
	struct cmd base;
	struct fw_cmd_push_desc fw;
} __attribute__((packed));

struct cmd_pop_desc {
	struct cmd base;
	struct fw_cmd_pop_desc fw;
} __attribute__((packed));

struct cmd_mcdma_copy {
	struct cmd base;
	struct fw_cmd_mcdma_copy fw;
} __attribute__((packed));

struct cmd_read_table_entry {
	struct cmd base;
	struct fw_cmd_read_table_entry fw;
} __attribute__((packed));

struct stub_cmd {
	struct cmd cmd;
	u32 data;
} __attribute__((packed));

struct cmd_add_shared_group {
	struct cmd base;
	struct fw_cmd_add_bwl_group fw;
} __attribute__((packed));

struct cmd_set_shared_group {
	struct cmd base;
	struct fw_cmd_set_bwl_group fw;
} __attribute__((packed));

struct cmd_remove_shared_group {
	struct cmd base;
	struct fw_cmd_rem_bwl_group fw;
} __attribute__((packed));

struct cmd_get_queue_stats {
	struct cmd base;
	struct fw_cmd_get_queue_stats fw;
} __attribute__((packed));

struct cmd_get_qm_stats {
	struct cmd base;
	struct fw_cmd_get_qm_stats fw;
} __attribute__((packed));

struct cmd_get_codel_stat {
	struct cmd base;
	struct fw_cmd_get_codel_stats fw;
} __attribute__((packed));

struct cmd_get_port_stats {
	struct cmd base;
	struct fw_cmd_get_port_stats fw;
} __attribute__((packed));

struct cmd_get_node_info {
	struct cmd base;
	struct fw_cmd_get_node_info fw;
} __attribute__((packed));

struct cmd_get_system_info {
	struct cmd base;
	struct fw_cmd_get_system_info fw;
} __attribute__((packed));

struct cmd_update_port_tree {
	struct cmd base;
	struct fw_cmd_update_port_tree fw;
} __attribute__((packed));

struct cmd_suspend_port_tree {
	struct cmd base;
	struct fw_cmd_suspend_port_tree fw;
} __attribute__((packed));

struct cmd_get_quanta {
	struct cmd base;
	struct fw_cmd_get_quanta fw;
} __attribute__((packed));

struct cmd_mod_log_bmap {
	struct cmd base;
	struct fw_cmd_mod_log_bmap fw;
} __attribute__((packed));

struct cmd_set_wsp_helper {
	struct cmd base;
	struct fw_cmd_wsp_helper fw;
} __attribute__((packed));

struct cmd_get_wsp_helper_stats {
	struct cmd base;
	struct fw_cmd_wsp_helper_stats fw;
} __attribute__((packed));

union driver_cmd {
	struct cmd                     cmd;
	struct stub_cmd                stub;
	struct cmd_init_logger         init_logger;
	struct cmd_init_qos            init_qos;
	struct cmd_move_queue          move_queue;
	struct cmd_move_sched          move_sched;
	struct cmd_remove_queue        rem_queue;
	struct cmd_remove_port         rem_port;
	struct cmd_remove_sched        rem_sched;
	struct cmd_add_port            add_port;
	struct cmd_set_port            set_port;
	struct cmd_add_sched           add_sched;
	struct cmd_set_sched           set_sched;
	struct cmd_add_queue           add_queue;
	struct cmd_set_queue           set_queue;
	struct cmd_get_queue_stats     queue_stats;
	struct cmd_get_qm_stats        qm_stats;
	struct cmd_set_aqm_sf          set_aqm_sf;
	struct cmd_set_codel_cfg       set_codel_cfg;
	struct cmd_clk_update          clk_update;
	struct cmd_nodes_refresh       nodes_clk_update;
	struct cmd_get_codel_stat      codel_stats;
	struct cmd_get_port_stats      port_stats;
	struct cmd_add_shared_group    add_shared_group;
	struct cmd_set_shared_group    set_shared_group;
	struct cmd_remove_shared_group remove_shared_group;
	struct cmd_push_desc           pushd;
	struct cmd_get_node_info       node_info;
	struct cmd_read_table_entry    read_table_entry;
	struct cmd_flush_queue         flush_queue;
	struct cmd_get_system_info     sys_info;
	struct cmd_update_port_tree    update_port_tree;
	struct cmd_suspend_port_tree   suspend_port_tree;
	struct cmd_get_quanta          get_quanta;
	struct cmd_mod_log_bmap        mod_log_bmap;
	struct cmd_set_wsp_helper      set_wsp_helper;
	struct cmd_get_wsp_helper_stats get_wsp_helper_stats;
	struct cmd_internal            internal;
} __attribute__((packed));

/******************************************************************************/
/*                                 FW CMDS                                    */
/******************************************************************************/

struct fw_set_common {
	u32 valid;
	s32 suspend;
	u32 bw_limit;
	u32 shared_bw_group;
	u32 max_burst;
	u32 port_phy; /* Relevant only for queue */
};

struct fw_set_parent {
	u32 valid;
	s32 best_effort_enable;
	u16 first;
	u16 last;
	u16 first_wrr;
};

struct fw_set_child {
	u32 valid;
	u32 bw_share;
	u32 preds[QOS_MAX_PREDECESSORS];
};

struct fw_set_port {
	u32 valid;
	u32 ring_addr_h;
	u32 ring_addr_l;
	size_t ring_size;
	s32 active;
	u32 green_threshold;
	u32 yellow_threshold;
};

struct fw_set_queue {
	u32 valid;
	u32 rlm;
	s32 active;
	u32 disable;
	u32 wred_min_avg_green;
	u32 wred_max_avg_green;
	u32 wred_slope_green;
	u32 wred_min_avg_yellow;
	u32 wred_max_avg_yellow;
	u32 wred_slope_yellow;
	u32 wred_min_guaranteed;
	u32 wred_max_allowed;
	u32 wred_fixed_drop_prob_green;
	u32 wred_fixed_drop_prob_yellow;
	s32 codel_en;
};

struct fw_internal {
	struct fw_set_common common;
	struct fw_set_parent parent;
	struct fw_set_child  child;
	union {
		struct fw_set_port port;
		struct fw_set_queue queue;
	} type_data;
	u32 suspend_ports_index;
	u32 num_suspend_ports;
	u32 suspend_ports[PP_QOS_MAX_PORTS];
	u32 pushed;
};

/******************************************************************************/
/*                                FW wrappers                                 */
/******************************************************************************/

static void set_common(struct pp_qos_dev *qdev,
		       const struct pp_qos_common_node_properties *conf,
		       struct fw_set_common *common, u32 modified, u32 phy)
{
	struct qos_node *node;
	u32 valid;

	valid = 0;

	node = get_node_from_phy(qdev->nodes, phy);
	if (unlikely(!node)) {
		QOS_LOG_ERR("get_node_from_phy(%u) returned NULL\n", phy);
		return;
	}

	if (QOS_BITS_IS_SET(modified, QOS_MODIFIED_BANDWIDTH_LIMIT)) {
		QOS_BITS_SET(valid, TSCD_NODE_CONF_BW_LIMIT);
		if (node_queue(node))
			common->bw_limit = node->bandwidth_limit;
		else
			common->bw_limit = conf->bandwidth_limit;
	}

	if (QOS_BITS_IS_SET(modified, QOS_MODIFIED_SHARED_GROUP_ID)) {
		QOS_BITS_SET(valid, TSCD_NODE_CONF_SHARED_BWL_GROUP);
		common->shared_bw_group = conf->shared_bw_group;
	}

	if (QOS_BITS_IS_SET(modified, QOS_MODIFIED_MAX_BURST)) {
		QOS_BITS_SET(valid, TSCD_NODE_CONF_MAX_BURST);
		common->max_burst = conf->max_burst;
	}

	common->valid = valid;
}

static void
update_arbitration(u32 *valid,
		   const struct parent_node_properties *parent_node_prop,
		   struct fw_set_parent *parent)
{
	/* Set parent direct children */
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

static void set_parent(const struct pp_qos_parent_node_properties *conf,
		       const struct parent_node_properties *parent_node_prop,
		       struct fw_set_parent *parent, u32 modified)
{
	u32 valid;

	valid = 0;
	if (QOS_BITS_IS_SET(modified, QOS_MODIFIED_ARBITRATION))
		update_arbitration(&valid, parent_node_prop, parent);

	if (QOS_BITS_IS_SET(modified, QOS_MODIFIED_BEST_EFFORT)) {
		QOS_BITS_SET(valid, TSCD_NODE_CONF_BEST_EFFORT_ENABLE);
		parent->best_effort_enable = conf->best_effort_enable;
	}
	parent->valid = valid;
}

static void set_child(const struct pp_qos_child_node_properties *conf,
		      const struct qos_node *node,
		      struct fw_set_child *child, u32 modified)
{
	u32 valid = 0;

	if (QOS_BITS_IS_SET(modified, QOS_MODIFIED_BW_WEIGHT)) {
		QOS_BITS_SET(valid, TSCD_NODE_CONF_NODE_WEIGHT);
		child->bw_share = node->child_prop.bw_weight;
	}

	/* Should be changed. Currently both are using bw_share variable */
	if (QOS_BITS_IS_SET(modified, QOS_MODIFIED_SHARED_GROUP_ID)) {
		QOS_BITS_SET(valid, TSCD_NODE_CONF_SHARED_BWL_GROUP);
		child->bw_share = conf->wrr_weight;
	}

	child->valid = valid;
}

static void set_port(const struct pp_qos_port_conf *conf,
		     struct fw_set_port *port, u32 modified)
{
	u32 valid = 0;

	if (QOS_BITS_IS_SET(modified, QOS_MODIFIED_RING_SIZE)) {
		QOS_BITS_SET(valid, PORT_CONF_RING_SIZE);
		port->ring_size = conf->ring_size;
	}

	if (QOS_BITS_IS_SET(modified, QOS_MODIFIED_RING_ADDRESS)) {
		QOS_BITS_SET(valid, PORT_CONF_RING_ADDRESS_LOW);
		QOS_BITS_SET(valid, PORT_CONF_RING_ADDRESS_HIGH);
		port->ring_addr_l = (u32)conf->ring_address;
		port->ring_addr_h = (u32)((u64)conf->ring_address >> 32);
	}

	if (QOS_BITS_IS_SET(modified, QOS_MODIFIED_DISABLE)) {
		QOS_BITS_SET(valid, PORT_CONF_ACTIVE);
		port->active = !conf->disable;
	}

	if (QOS_BITS_IS_SET(modified, QOS_MODIFIED_PORT_GREEN_THRESHOLD)) {
		QOS_BITS_SET(valid, PORT_CONF_GREEN_THRESHOLD);
		port->green_threshold = conf->green_threshold;
	}

	if (QOS_BITS_IS_SET(modified, QOS_MODIFIED_PORT_YELLOW_THRESHOLD)) {
		QOS_BITS_SET(valid, PORT_CONF_YELLOW_THRESHOLD);
		port->yellow_threshold = conf->yellow_threshold;
	}
	port->valid = valid;
}

static void set_queue(const struct pp_qos_queue_conf *conf,
		      struct fw_set_queue *queue, u32 modified)
{
	u32 valid = 0;
	u32 disable = 0;

	if (QOS_BITS_IS_SET(modified, QOS_MODIFIED_WRED_MAX_ALLOWED)) {
		QOS_BITS_SET(valid, WRED_QUEUE_CONF_MAX_ALLOWED);
		queue->wred_max_allowed = conf->wred_max_allowed;
	}

	if (QOS_BITS_IS_SET(modified, QOS_MODIFIED_BLOCKED)) {
		QOS_BITS_SET(valid, WRED_QUEUE_CONF_ACTIVE_Q);
		QOS_BITS_SET(valid, WRED_QUEUE_CONF_MAX_ALLOWED);
		queue->active = !conf->blocked;
		if (queue->active) {
			queue->wred_max_allowed =
				conf->wred_max_allowed;
		} else {
			queue->wred_max_allowed = 0;
		}
	}

	if (QOS_BITS_IS_SET(modified, QOS_MODIFIED_WRED_ENABLE)) {
		QOS_BITS_SET(valid, WRED_QUEUE_CONF_DISABLE);
		if (!conf->wred_enable)
			QOS_BITS_SET(disable, WRED_FLAGS_DISABLE_WRED);
	}
	if (QOS_BITS_IS_SET(modified,
			    QOS_MODIFIED_WRED_FIXED_DROP_PROB_ENABLE)) {
		QOS_BITS_SET(valid, WRED_QUEUE_CONF_DISABLE);
		if (conf->wred_fixed_drop_prob_enable)
			QOS_BITS_SET(disable, WRED_FLAGS_FIXED_PROBABILITY);
	}
	queue->disable = disable;

	if (QOS_BITS_IS_SET(modified, QOS_MODIFIED_WRED_FIXED_GREEN_PROB)) {
		QOS_BITS_SET(valid, WRED_QUEUE_CONF_FIXED_GREEN_DROP_P);
		queue->wred_fixed_drop_prob_green =
			conf->wred_fixed_drop_prob_green;
	}

	if (QOS_BITS_IS_SET(modified, QOS_MODIFIED_WRED_FIXED_YELLOW_PROB)) {
		QOS_BITS_SET(valid, WRED_QUEUE_CONF_FIXED_YELLOW_DROP_P);
		queue->wred_fixed_drop_prob_yellow =
			conf->wred_fixed_drop_prob_yellow;
	}

	if (QOS_BITS_IS_SET(modified, QOS_MODIFIED_WRED_MIN_YELLOW)) {
		QOS_BITS_SET(valid, WRED_QUEUE_CONF_MIN_AVG_YELLOW);
		queue->wred_min_avg_yellow = conf->wred_min_avg_yellow;
	}

	if (QOS_BITS_IS_SET(modified, QOS_MODIFIED_WRED_MAX_YELLOW)) {
		QOS_BITS_SET(valid, WRED_QUEUE_CONF_MAX_AVG_YELLOW);
		queue->wred_max_avg_yellow = conf->wred_max_avg_yellow;
	}

	if (QOS_BITS_IS_SET(modified, QOS_MODIFIED_WRED_SLOPE_YELLOW)) {
		QOS_BITS_SET(valid, WRED_QUEUE_CONF_SLOPE_YELLOW);
		queue->wred_slope_yellow = conf->wred_slope_yellow;
	}

	if (QOS_BITS_IS_SET(modified, QOS_MODIFIED_WRED_MIN_GREEN)) {
		QOS_BITS_SET(valid, WRED_QUEUE_CONF_MIN_AVG_GREEN);
		queue->wred_min_avg_green = conf->wred_min_avg_green;
	}

	if (QOS_BITS_IS_SET(modified, QOS_MODIFIED_WRED_MAX_GREEN)) {
		QOS_BITS_SET(valid, WRED_QUEUE_CONF_MAX_AVG_GREEN);
		queue->wred_max_avg_green = conf->wred_max_avg_green;
	}

	if (QOS_BITS_IS_SET(modified, QOS_MODIFIED_WRED_SLOPE_GREEN)) {
		QOS_BITS_SET(valid, WRED_QUEUE_CONF_SLOPE_GREEN);
		queue->wred_slope_green = conf->wred_slope_green;
	}

	if (QOS_BITS_IS_SET(modified, QOS_MODIFIED_WRED_MIN_GUARANTEED)) {
		QOS_BITS_SET(valid, WRED_QUEUE_CONF_MIN_GUARANTEED);
		queue->wred_min_guaranteed = conf->wred_min_guaranteed;
	}

	if (QOS_BITS_IS_SET(modified, QOS_MODIFIED_CODEL)) {
		QOS_BITS_SET(valid, TXMGR_QUEUE_CODEL_EN);
		queue->codel_en = conf->codel_en;
	}

	queue->valid = valid;
}

/******************************************************************************/
/*                         Driver functions                                   */
/******************************************************************************/

s32 load_firmware(struct pp_qos_dev *qdev, const char *name)
{
	s32 err;
	const struct firmware *firmware;
	struct ppv4_qos_fw fw;

	QOS_LOG_DEBUG("\n");

	if (is_uc_awake(qdev->init_params.wakeuc)) {
		pr_info("Firmware already loaded\n");
		return 0;
	}

	err = request_firmware(&firmware, name, pp_dev_get());
	if (err < 0) {
		pr_err("Failed loading firmware ret is %d\n", err);
		return err;
	}

	fw.size = firmware->size;
	fw.data = firmware->data;

	err = do_load_firmware(qdev, &fw,
			       qdev->init_params.qos_uc_base,
			       qdev->init_params.wakeuc);

	release_firmware(firmware);

	return err;
}

static int __init qos_fw_log_en(char *str)
{
	QOS_LOG_INFO("PPv4 QoS FW Logger is enabled\n");
	qos_fw_log = true;

	return 0;
}

early_param("qosFwLog", qos_fw_log_en);

s32 qos_fw_start(struct pp_qos_dev *qdev)
{
	struct wsp_helper_cfg *ewsp_cfg;
	s32 ret;

	if (qos_fw_log) {
		create_init_logger_cmd(qdev, UC_LOGGER_LEVEL_INFO,
				       UC_LOGGER_MODE_WRITE_HOST_MEM);
	} else {
		create_init_logger_cmd(qdev, UC_LOGGER_LEVEL_DEFAULT,
				       UC_LOGGER_MODE_WRITE_HOST_MEM);
	}

	create_init_qos_cmd(qdev);

	ewsp_cfg = &qdev->ewsp_cfg;
	ewsp_cfg->enable = qdev->init_params.enhanced_wsp;
	ewsp_cfg->timeout_microseconds = QOS_EWSP_DEFAULT_TIMEOUT_US;
	ewsp_cfg->byte_threshold = QOS_EWSP_DEFAULT_BYTE_THRESHOLD;
	create_set_wsp_helper_cmd(qdev, ewsp_cfg->enable,
				  ewsp_cfg->timeout_microseconds,
				  ewsp_cfg->byte_threshold);

	update_cmd_id(&qdev->drvcmds);
	ret = transmit_cmds(qdev);
	if (unlikely(ret))
		return ret;

	qdev->initialized = true;

	return 0;
}

s32 qos_fw_init(struct pp_qos_dev *qdev)
{
	s32 ret;

	ret = load_firmware(qdev, FIRMWARE_FILE);
	if (unlikely(ret))
		return ret;

	return qos_fw_start(qdev);
}

/* Following functions creates commands in driver fromat to be stored at
 * drivers queues before sending to firmware
 */

/* Extract ancestors of node from driver's DB */
static void fill_preds(const struct pp_nodes *nodes,
		       u32 phy, u32 *preds, size_t size)
{
	u32 i;
	const struct qos_node *node;

	i = 0;
	memset(preds, 0x0, size * sizeof(u32));
	node = get_const_node_from_phy(nodes, phy);
	if (unlikely(!node)) {
		QOS_LOG_ERR("get_const_node_from_phy(%u) returned NULL\n",
			    phy);
		return;
	}

	while (node_child(node) && (i < size)) {
		preds[i] = node->child_prop.parent_phy;
		node = get_const_node_from_phy(nodes,
					       node->child_prop.parent_phy);
		i++;
	}
}

static void cmd_init(const struct pp_qos_dev *qdev,
		     struct cmd *cmd, enum cmd_type type, size_t len,
		     u32 flags, void *response, u32 int_response_offset,
		     u32 response_sz)
{
	cmd->flags = flags;
	cmd->type = type;
	cmd->len = len;
	cmd->id = qdev->drvcmds.cmd_id;
	cmd->response = response;
	cmd->int_response_offset = int_response_offset;
	cmd->response_sz = response_sz;

	if ((flags & CMD_FLAGS_POST_PROCESS_EXT) && !response)
		QOS_LOG_ERR("POST PROCESS Ext without response ptr\n");

	if ((flags & CMD_FLAGS_POST_PROCESS_INT) && !int_response_offset)
		QOS_LOG_ERR("POST PROCESS Int flag without response offset\n");

	if ((flags & (CMD_FLAGS_POST_PROCESS_EXT |
	     CMD_FLAGS_POST_PROCESS_INT)) && !response_sz)
		QOS_LOG_ERR("POST PROCESS flag without response size\n");
}

void create_init_logger_cmd(struct pp_qos_dev *qdev,
			    enum uc_logger_level level,
			    enum uc_logger_mode mode)
{
	struct cmd_init_logger cmd;

	cmd_init(qdev, &cmd.base, CMD_TYPE_INIT_LOGGER,
		 sizeof(cmd), 0, NULL, 0, 0);
	cmd.fw.base.type = UC_QOS_CMD_INIT_UC_LOGGER;

	cmd.fw.write_idx_addr = qdev->hwconf.fw_logger_ddr_phys +
		PPV4_QOS_LOGGER_WRITE_IDX_OFFSET;

	cmd.fw.read_idx_addr = qdev->hwconf.fw_logger_ddr_phys +
		PPV4_QOS_LOGGER_READ_IDX_OFFSET;

	cmd.fw.ring_addr = qdev->hwconf.fw_logger_ddr_phys +
		PPV4_QOS_LOGGER_RING_OFFSET;

	cmd.fw.mode = mode;
	cmd.fw.level = level;
	cmd.fw.num_of_msgs =
		(qdev->hwconf.fw_logger_sz - PPV4_QOS_LOGGER_HEADROOM_SIZE)
		/ PPV4_QOS_LOGGER_MSG_SIZE;
	QOS_LOG_DEBUG("cmd %u: CMD_TYPE_INIT_LOGGER\n", qdev->drvcmds.cmd_id);
	cmd_queue_put(qdev->drvcmds.cmdq, (u8 *)&cmd, sizeof(cmd));
}

void create_init_qos_cmd(struct pp_qos_dev *qdev)
{
	struct cmd_init_qos cmd;

	if (PP_QOS_DEVICE_IS_ASSERT(qdev))
		return;

	cmd_init(qdev, &cmd.base, CMD_TYPE_INIT_QOS,
		 sizeof(cmd), 0, NULL, 0, 0);
	cmd.fw.base.type = UC_QOS_CMD_INIT_QOS;

	if (IS_ENABLED(CONFIG_PPV4_LGM))
		cmd.fw.qm_ddr_start = qdev->hwconf.qm_ddr_phys >> 4;
	else
		cmd.fw.qm_ddr_start = qdev->hwconf.qm_ddr_phys;
	cmd.fw.qm_num_pages = qdev->hwconf.qm_num_pages;
	cmd.fw.wred_total_avail_resources =
		qdev->init_params.wred_total_avail_resources;
	cmd.fw.wred_prioritize_pop = qdev->init_params.wred_prioritize_pop;
	cmd.fw.wred_avg_q_size_p = qdev->init_params.wred_p_const;
	cmd.fw.wred_max_q_size = qdev->init_params.wred_max_q_size;
	cmd.fw.num_of_ports = qdev->init_params.max_ports;
	cmd.fw.hw_clk = qdev->init_params.hw_clk;
	cmd.fw.bm_base = qdev->init_params.bm_base;
	cmd.fw.soc_rev = __fw_soc_rev_get();
	cmd.fw.bwl_ddr_base = qdev->hwconf.bwl_ddr_phys;
	cmd.fw.sbwl_ddr_base = qdev->hwconf.sbwl_ddr_phys;
	cmd.fw.wsp_queues_ddr_base = qdev->hwconf.wsp_queues_ddr_phys;
#ifdef CONFIG_PPV4_HW_MOD_REGS_LOGS
	cmd.fw.mod_reg_log_en = CONFIG_PPV4_HW_MOD_REGS_LOGS_EN & UC_MOD_ALL;
#else
	cmd.fw.mod_reg_log_en = 0;
#endif
	QOS_LOG_DEBUG("cmd %u: CMD_TYPE_INIT_QOS\n", qdev->drvcmds.cmd_id);
	cmd_queue_put(qdev->drvcmds.cmdq, (u8 *)&cmd, sizeof(cmd));
}

void create_move_cmd(struct pp_qos_dev *qdev, u16 dst, u16 src, u16 dst_port)
{
	struct cmd_move_queue queue_cmd = { 0 };
	struct cmd_move_sched sched_cmd = { 0 };
	const struct qos_node *node;
	enum cmd_type type;

	if (PP_QOS_DEVICE_IS_ASSERT(qdev))
		return;

	/* If moving from tmp node, use the stored information */
	if (src == PP_QOS_TMP_NODE)
		node = get_const_node_from_phy(qdev->nodes, src);
	else
		node = get_const_node_from_phy(qdev->nodes, dst);

	if (unlikely(!node)) {
		QOS_LOG_ERR("get_const_node_from_phy(%u) returned NULL\n",
			    dst);
		return;
	}

	add_suspend_port(qdev, dst_port);

	if (node->type == TYPE_QUEUE) {
		cmd_init(qdev, &queue_cmd.base, CMD_TYPE_MOVE_QUEUE,
			 sizeof(queue_cmd), 0, NULL, 0, 0);
		queue_cmd.fw.base.type = UC_QOS_CMD_MOVE_QUEUE;
		type = CMD_TYPE_MOVE_QUEUE;
		queue_cmd.fw.src = src;
		queue_cmd.fw.dst = dst;
		queue_cmd.fw.dst_port = dst_port;
		queue_cmd.fw.rlm = node->data.queue.rlm;
		queue_cmd.fw.is_alias = node->data.queue.is_alias;
		fill_preds(qdev->nodes, dst, queue_cmd.fw.preds,
			   QOS_MAX_PREDECESSORS);
		cmd_queue_put(qdev->drvcmds.cmdq,
			      (u8 *)&queue_cmd,
			      sizeof(queue_cmd));
	} else if (node->type == TYPE_SCHED) {
		cmd_init(qdev, &sched_cmd.base, CMD_TYPE_MOVE_SCHED,
			 sizeof(sched_cmd), 0, NULL, 0, 0);
		sched_cmd.fw.base.type = UC_QOS_CMD_MOVE_SCHEDULER;
		type = CMD_TYPE_MOVE_SCHED;
		sched_cmd.fw.src = src;
		sched_cmd.fw.dst = dst;
		fill_preds(qdev->nodes, dst, sched_cmd.fw.preds,
			   QOS_MAX_PREDECESSORS);
		cmd_queue_put(qdev->drvcmds.cmdq, (u8 *)&sched_cmd,
			      sizeof(sched_cmd));
	} else {
		QOS_LOG_ERR("Node type %u does not support move operation\n",
			    node->type);
		return;
	}

	QOS_LOG_DEBUG("cmd %u: %s %u-->%u type:%d, port:%u\n",
		      qdev->drvcmds.cmd_id, cmd_str[type],
		      src, dst, node->type, dst_port);
}

void create_remove_node_cmd(struct pp_qos_dev *qdev,
			    enum node_type ntype, u32 phy, u32 rlm)
{
	struct cmd_remove_queue queue_cmd;
	struct cmd_remove_port port_cmd;
	struct cmd_remove_sched sched_cmd;
	enum cmd_type ctype;
	const struct qos_node *node;
	void *cmd;
	u32 cmd_size;

	if (PP_QOS_DEVICE_IS_ASSERT(qdev))
		return;

	node = get_const_node_from_phy(qdev->nodes, phy);
	if (unlikely(!node)) {
		QOS_LOG_ERR("get_const_node_from_phy(%u) returned NULL\n", phy);
		return;
	}

	switch (ntype) {
	case TYPE_PORT:
		ctype = CMD_TYPE_REM_PORT;
		cmd_init(qdev, &port_cmd.base, ctype,
			 sizeof(port_cmd), 0, NULL, 0, 0);
		port_cmd.fw.base.type = UC_QOS_CMD_REM_PORT;
		port_cmd.fw.phy = phy;
		cmd = (void *)&port_cmd;
		cmd_size = sizeof(port_cmd);
		break;
	case TYPE_SCHED:
		ctype = CMD_TYPE_REM_SCHED;
		cmd_init(qdev, &sched_cmd.base, ctype,
			 sizeof(sched_cmd), 0, NULL, 0, 0);
		sched_cmd.fw.base.type = UC_QOS_CMD_REM_SCHEDULER;
		sched_cmd.fw.phy = phy;
		cmd = (void *)&sched_cmd;
		cmd_size = sizeof(sched_cmd);
		break;
	case TYPE_QUEUE:
		ctype = CMD_TYPE_REM_QUEUE;
		cmd_init(qdev, &queue_cmd.base, ctype,
			 sizeof(queue_cmd), 0, NULL, 0, 0);
		queue_cmd.fw.base.type = UC_QOS_CMD_REM_QUEUE;
		queue_cmd.fw.phy = phy;
		queue_cmd.fw.rlm = rlm;
		queue_cmd.fw.is_alias = node->data.queue.is_alias;
		cmd = (void *)&queue_cmd;
		cmd_size = sizeof(queue_cmd);
		break;
	case TYPE_UNKNOWN:
		QOS_ASSERT(0, "Unexpected unknown type\n");
		return;
	default:
		QOS_LOG_ERR("illegal node type %d\n", ntype);
		return;
	}

	QOS_LOG_DEBUG("cmd %u: %s %u rlm %u alias %u\n",
		      qdev->drvcmds.cmd_id,
		      cmd_str[ctype], phy, rlm,
		      node->data.queue.is_alias);

	if ((ctype != CMD_TYPE_REM_PORT) && (!node_syncq(node)))
		add_suspend_port(qdev, get_port(qdev->nodes, phy));

	cmd_queue_put(qdev->drvcmds.cmdq, cmd, cmd_size);
}

static void create_add_port_cmd(struct pp_qos_dev *qdev,
				const struct pp_qos_port_conf *conf, u32 phy)
{
	struct cmd_add_port cmd = { 0 };

	if (PP_QOS_DEVICE_IS_ASSERT(qdev))
		return;

	memset(&cmd, 0, sizeof(cmd));
	cmd_init(qdev, &cmd.base, CMD_TYPE_ADD_PORT, sizeof(cmd),
		 CMD_FLAGS_POST_PROCESS_INT, NULL,
		 offsetof(struct fw_cmd_add_port, params) +
		 offsetof(struct fw_cmd_port_params, actual_bw_limit),
		 CMD_INT_RESPONSE_SZ);

	cmd.fw.base.type = UC_QOS_CMD_ADD_PORT;
	cmd.fw.port_phy = phy;
	cmd.fw.active = !conf->disable;
	cmd.fw.params.first_child = 0;
	cmd.fw.params.last_child = 0;
	cmd.fw.params.bw_limit = conf->common_prop.bandwidth_limit;
	cmd.fw.params.best_effort = conf->port_parent_prop.best_effort_enable;
	cmd.fw.params.first_wrr_node = 0;
	cmd.fw.params.shared_bwl_entry = conf->common_prop.shared_bw_group;
	cmd.fw.params.max_burst = conf->common_prop.max_burst;
	cmd.fw.disable_byte_credit = conf->packet_credit_enable;
	cmd.fw.ring_params.txmgr_ring_sz = conf->ring_size;
	cmd.fw.ring_params.txmgr_ring_addr_h =
		(u32)((u64)conf->ring_address >> 32);
	cmd.fw.ring_params.txmgr_ring_addr_l =
		(u32)conf->ring_address;
	cmd.fw.txmgr_initial_port_credit = conf->credit;
	cmd.fw.green_thr = conf->green_threshold;
	cmd.fw.yellow_thr = conf->yellow_threshold;

	QOS_LOG_DEBUG("cmd %u: CMD_TYPE_ADD_PORT %u\n",
		      qdev->drvcmds.cmd_id, phy);

	cmd_queue_put(qdev->drvcmds.cmdq, &cmd, sizeof(cmd));
}

static void _create_set_port_cmd(struct pp_qos_dev *qdev,
				 u32 phy,
				 u32 modified,
				 struct fw_set_common *common,
				 struct fw_set_parent *parent,
				 struct fw_set_port *port)
{
	struct cmd_set_port cmd;
	u32 flags = 0;

	if (QOS_BITS_IS_SET(common->valid, TSCD_NODE_CONF_BW_LIMIT))
		flags |= CMD_FLAGS_POST_PROCESS_INT;

	memset(&cmd, 0, sizeof(cmd));
	cmd_init(qdev, &cmd.base, CMD_TYPE_SET_PORT, sizeof(cmd), flags, NULL,
		 offsetof(struct fw_cmd_set_port, params) +
		 offsetof(struct fw_cmd_port_params, actual_bw_limit),
		 flags ? CMD_INT_RESPONSE_SZ : 0);

	cmd.fw.base.type = UC_QOS_CMD_SET_PORT;
	cmd.fw.port_phy = phy;
	cmd.fw.valid = common->valid | parent->valid;

	if (common->valid) {
		cmd.fw.params.shared_bwl_entry = common->shared_bw_group;
		cmd.fw.params.bw_limit = common->bw_limit;
		cmd.fw.params.max_burst = common->max_burst;
	}

	if (parent->valid)
		cmd.fw.params.best_effort = parent->best_effort_enable;

	if (port->valid) {
		cmd.fw.port_valid = port->valid;
		cmd.fw.ring_params.txmgr_ring_sz = port->ring_size;
		cmd.fw.ring_params.txmgr_ring_addr_l = port->ring_addr_l;
		cmd.fw.ring_params.txmgr_ring_addr_h = port->ring_addr_h;
		cmd.fw.active = port->active;
		cmd.fw.green_thr = port->green_threshold;
		cmd.fw.yellow_thr = port->yellow_threshold;
	}

	if (parent->first > parent->last) {
		parent->first = 0;
		parent->last = 0;
		parent->first_wrr = 0;
	}

	if (QOS_BITS_IS_SET(common->valid, TSCD_NODE_CONF_SUSPEND_RESUME))
		cmd.fw.suspend = common->suspend;

	if (QOS_BITS_IS_SET(parent->valid, TSCD_NODE_CONF_FIRST_CHILD))
		cmd.fw.params.first_child = parent->first;

	if (QOS_BITS_IS_SET(parent->valid, TSCD_NODE_CONF_LAST_CHILD))
		cmd.fw.params.last_child = parent->last;

	if (QOS_BITS_IS_SET(parent->valid, TSCD_NODE_CONF_FIRST_WRR_NODE))
		cmd.fw.params.first_wrr_node = parent->first_wrr;

	if ((cmd.fw.valid | cmd.fw.port_valid) == 0) {
		QOS_LOG_DEBUG("IGNORING EMPTY CMD_TYPE_SET_PORT\n");
		return;
	}

	add_suspend_port(qdev, phy);
	cmd_queue_put(qdev->drvcmds.cmdq, &cmd, sizeof(cmd));
}

void create_set_port_cmd(struct pp_qos_dev *qdev,
			 const struct pp_qos_port_conf *conf,
			 u32 phy, u32 modified)
{
	struct fw_set_common common;
	struct fw_set_parent parent;
	struct fw_set_port port;
	struct qos_node *node;

	if (PP_QOS_DEVICE_IS_ASSERT(qdev))
		return;

	/* Only external parameters may change here */
	memset(&common, 0, sizeof(struct fw_set_common));
	memset(&parent, 0, sizeof(struct fw_set_parent));
	memset(&port, 0, sizeof(struct fw_set_port));

	if (QOS_BITS_IS_SET(modified, QOS_MODIFIED_NODE_TYPE)) {
		create_add_port_cmd(qdev, conf, phy);
	} else {
		node = get_node_from_phy(qdev->nodes, phy);
		set_common(qdev, &conf->common_prop, &common, modified, phy);
		set_parent(&conf->port_parent_prop, &node->parent_prop,
			   &parent, modified);
		set_port(conf, &port, modified);
		_create_set_port_cmd(qdev, phy, modified,
				     &common, &parent, &port);
	}

	if (!QOS_BITS_IS_SET(modified, QOS_MODIFIED_NODE_TYPE))
		QOS_LOG_DEBUG("cmd %u: CMD_TYPE_SET_PORT %u\n",
			      qdev->drvcmds.cmd_id, phy);
}

static void create_add_sched_cmd(struct pp_qos_dev *qdev,
				 const struct pp_qos_sched_conf *conf,
				 u32 phy)
{
	struct cmd_add_sched cmd;

	if (PP_QOS_DEVICE_IS_ASSERT(qdev))
		return;

	memset(&cmd, 0, sizeof(cmd));
	cmd_init(qdev, &cmd.base, CMD_TYPE_ADD_SCHED, sizeof(cmd),
		 CMD_FLAGS_POST_PROCESS_INT, NULL,
		 offsetof(struct fw_cmd_add_sched, actual_bw_limit),
		 CMD_INT_RESPONSE_SZ);

	cmd.fw.base.type = UC_QOS_CMD_ADD_SCHEDULER;
	cmd.fw.phy = phy;
	cmd.fw.first_child = 0;
	cmd.fw.last_child = 0;
	cmd.fw.bw_limit = conf->common_prop.bandwidth_limit;
	cmd.fw.best_effort = conf->sched_parent_prop.best_effort_enable;
	cmd.fw.first_wrr_node = 0;
	cmd.fw.shared_bwl_entry = conf->common_prop.shared_bw_group;
	cmd.fw.max_burst = conf->common_prop.max_burst;
	fill_preds(qdev->nodes, phy, cmd.fw.preds, QOS_MAX_PREDECESSORS);

	QOS_LOG_DEBUG("cmd %u: CMD_TYPE_ADD_SCHED %u\n",
		      qdev->drvcmds.cmd_id, phy);

	add_suspend_port(qdev, get_port(qdev->nodes, phy));
	cmd_queue_put(qdev->drvcmds.cmdq, &cmd, sizeof(cmd));
}

static void _create_set_sched_cmd(struct pp_qos_dev *qdev,
				  u32 phy,
				  u32 modified,
				  struct fw_set_common *common,
				  struct fw_set_parent *parent,
				  struct fw_set_child *child)
{
	struct cmd_set_sched cmd;
	u32 pred_id;
	u32 flags = 0;

	if (QOS_BITS_IS_SET(common->valid, TSCD_NODE_CONF_BW_LIMIT))
		flags |= CMD_FLAGS_POST_PROCESS_INT;

	memset(&cmd, 0, sizeof(cmd));
	cmd_init(qdev, &cmd.base, CMD_TYPE_SET_SCHED, sizeof(cmd), flags, NULL,
		 offsetof(struct fw_cmd_set_sched, actual_bw_limit),
		 flags ? CMD_INT_RESPONSE_SZ : 0);

	cmd.fw.base.type = UC_QOS_CMD_SET_SCHEDULER;
	cmd.fw.phy = phy;

	cmd.fw.best_effort = parent->best_effort_enable;
	cmd.fw.node_weight = child->bw_share;
	cmd.fw.valid = common->valid | parent->valid | child->valid;

	if (common->valid) {
		cmd.fw.bw_limit = common->bw_limit;
		cmd.fw.shared_bwl_entry = common->shared_bw_group;
		cmd.fw.max_burst = common->max_burst;
	}

	if (child->valid) {
		for (pred_id = 0; pred_id < QOS_MAX_PREDECESSORS; ++pred_id)
			cmd.fw.preds[pred_id] = child->preds[pred_id];
	}

	if (parent->first > parent->last) {
		parent->first = 0;
		parent->last = 0;
	}

	if (QOS_BITS_IS_SET(common->valid, TSCD_NODE_CONF_SUSPEND_RESUME))
		cmd.fw.suspend = common->suspend;

	if (QOS_BITS_IS_SET(parent->valid, TSCD_NODE_CONF_FIRST_CHILD))
		cmd.fw.first_child = parent->first;

	if (QOS_BITS_IS_SET(parent->valid, TSCD_NODE_CONF_LAST_CHILD))
		cmd.fw.last_child = parent->last;

	if (QOS_BITS_IS_SET(parent->valid, TSCD_NODE_CONF_FIRST_WRR_NODE))
		cmd.fw.first_wrr_node = parent->first_wrr;

	if (cmd.fw.valid == 0) {
		QOS_LOG_DEBUG("IGNORING EMPTY CMD_TYPE_SET_SCHED\n");
		return;
	}

	add_suspend_port(qdev, get_port(qdev->nodes, phy));
	cmd_queue_put(qdev->drvcmds.cmdq, &cmd, sizeof(cmd));
}

void create_set_sched_cmd(struct pp_qos_dev *qdev,
			  const struct pp_qos_sched_conf *conf,
			  const struct qos_node *node,
			  u32 modified)
{
	struct fw_set_common common;
	struct fw_set_parent parent;
	struct fw_set_child child;
	u32 phy = get_phy_from_node(qdev->nodes, node);

	if (PP_QOS_DEVICE_IS_ASSERT(qdev))
		return;

	/* Only external parameters may change here */
	memset(&common, 0, sizeof(struct fw_set_common));
	memset(&parent, 0, sizeof(struct fw_set_parent));
	memset(&child, 0, sizeof(struct fw_set_child));

	if (QOS_BITS_IS_SET(modified, QOS_MODIFIED_NODE_TYPE)) {
		create_add_sched_cmd(qdev, conf, phy);
	} else {
		set_common(qdev, &conf->common_prop, &common, modified, phy);
		set_parent(&conf->sched_parent_prop, &node->parent_prop,
			   &parent, modified);
		set_child(&conf->sched_child_prop, node, &child, modified);
		_create_set_sched_cmd(qdev, phy, modified,
				      &common, &parent, &child);
	}

	if (!QOS_BITS_IS_SET(modified, QOS_MODIFIED_NODE_TYPE)) {
		QOS_LOG_DEBUG("cmd %u: CMD_TYPE_SET_SCHED %u\n",
			      qdev->drvcmds.cmd_id, phy);
	}
}

static void create_add_queue_cmd(struct pp_qos_dev *qdev,
				 const struct pp_qos_queue_conf *conf,
				 u32 phy, u32 rlm,
				 bool is_aliased, bool is_orphaned)
{
	struct cmd_add_queue   cmd;

	if (PP_QOS_DEVICE_IS_ASSERT(qdev))
		return;

	memset(&cmd, 0, sizeof(cmd));
	cmd_init(qdev, &cmd.base, CMD_TYPE_ADD_QUEUE,
		 sizeof(cmd), CMD_FLAGS_POST_PROCESS_INT, NULL,
		 offsetof(struct fw_cmd_add_queue, actual_bw_limit),
		 CMD_INT_RESPONSE_SZ);

	cmd.fw.base.type = UC_QOS_CMD_ADD_QUEUE;
	cmd.fw.phy = phy;
	if (is_orphaned)
		cmd.fw.port = 0;
	else
		cmd.fw.port = get_port(qdev->nodes, phy);

	cmd.fw.rlm = rlm;
	cmd.fw.bw_limit = conf->common_prop.bandwidth_limit;
	cmd.fw.shared_bwl_entry = conf->common_prop.shared_bw_group;
	cmd.fw.max_burst = conf->common_prop.max_burst;

	if (!is_orphaned) {
		fill_preds(qdev->nodes, phy, cmd.fw.preds,
			   QOS_MAX_PREDECESSORS);
	}

	cmd.fw.params.active = !conf->blocked;

	if (!conf->wred_enable) {
		QOS_BITS_SET(cmd.fw.params.disable_flags,
			     WRED_FLAGS_DISABLE_WRED);
	}

	if (conf->wred_fixed_drop_prob_enable) {
		QOS_BITS_SET(cmd.fw.params.disable_flags,
			     WRED_FLAGS_FIXED_PROBABILITY);
	}

	cmd.fw.params.fixed_green_drop_p = conf->wred_fixed_drop_prob_green;
	cmd.fw.params.fixed_yellow_drop_p = conf->wred_fixed_drop_prob_yellow;
	cmd.fw.params.min_avg_yellow = conf->wred_min_avg_yellow;
	cmd.fw.params.max_avg_yellow = conf->wred_max_avg_yellow;
	cmd.fw.params.slope_yellow = conf->wred_slope_yellow;
	cmd.fw.params.min_avg_green = conf->wred_min_avg_green;
	cmd.fw.params.max_avg_green = conf->wred_max_avg_green;
	cmd.fw.params.slope_green = conf->wred_slope_green;
	cmd.fw.params.min_guaranteed = conf->wred_min_guaranteed;
	cmd.fw.params.max_allowed = conf->blocked ? 0 : conf->wred_max_allowed;
	cmd.fw.params.is_alias = is_aliased;

	if ((pp_silicon_step_get() == PP_SSTEP_A) && conf->codel_en) {
		pr_warn("CoDel is not available for this silicon step\n");
		cmd.fw.params.codel_en = 0;
	} else {
		cmd.fw.params.codel_en = conf->codel_en;
	}

	QOS_LOG_DEBUG("cmd %u: CMD_TYPE_ADD_QUEUE %u\n",
		      qdev->drvcmds.cmd_id, phy);

	if (!is_orphaned)
		add_suspend_port(qdev, cmd.fw.port);

	cmd_queue_put(qdev->drvcmds.cmdq, &cmd, sizeof(cmd));
}

void create_add_sync_queue_cmd(struct pp_qos_dev *qdev,
			       const struct pp_qos_queue_conf *conf,
			       u32 phy, u32 rlm, u32 modified)
{
	if (PP_QOS_DEVICE_IS_ASSERT(qdev))
		return;

	create_add_queue_cmd(qdev, conf, phy, rlm, false, true);
	suspend_node(qdev, phy, 1);
}

static void _create_set_queue_cmd(struct pp_qos_dev *qdev, u32 phy,
				  u32 rlm, bool is_aliased, u32 modified,
				  struct fw_set_common *common,
				  struct fw_set_child *child,
				  struct fw_set_queue *queue,
				  bool is_orphaned)
{
	struct cmd_set_queue cmd;
	u32 pred_id;
	u32 flags = 0;

	if (QOS_BITS_IS_SET(common->valid, TSCD_NODE_CONF_BW_LIMIT))
		flags |= CMD_FLAGS_POST_PROCESS_INT;

	memset(&cmd, 0, sizeof(cmd));
	cmd_init(qdev, &cmd.base, CMD_TYPE_SET_QUEUE,
		 sizeof(cmd), flags, NULL,
		 offsetof(struct fw_cmd_set_queue, actual_bw_limit),
		 flags ? CMD_INT_RESPONSE_SZ : 0);

	cmd.fw.base.type = UC_QOS_CMD_SET_QUEUE;
	cmd.fw.phy = phy;
	cmd.fw.rlm = rlm;
	cmd.fw.params.is_alias = is_aliased;
	cmd.fw.valid = common->valid | child->valid;

	if (cmd.fw.valid == 0 && queue->valid == 0) {
		QOS_LOG_DEBUG("IGNORING EMPTY CMD_TYPE_SET_QUEUE\n");
		return;
	}

	if (QOS_BITS_IS_SET(common->valid, TSCD_NODE_CONF_SUSPEND_RESUME))
		cmd.fw.suspend = common->suspend;

	if (common->valid) {
		cmd.fw.bw_limit = common->bw_limit;
		cmd.fw.shared_bwl_entry = common->shared_bw_group;
		cmd.fw.max_burst = common->max_burst;
	}

	if (child->valid) {
		cmd.fw.node_weight = child->bw_share;
		for (pred_id = 0; pred_id < QOS_MAX_PREDECESSORS; ++pred_id)
			cmd.fw.preds[pred_id] = child->preds[pred_id];
	}

	if (!is_orphaned)
		cmd.fw.port = get_port(qdev->nodes, phy);

	if (queue->valid) {
		cmd.fw.queue_valid = queue->valid;
		cmd.fw.params.active = queue->active;
		cmd.fw.params.disable_flags = queue->disable;
		cmd.fw.params.fixed_green_drop_p =
			queue->wred_fixed_drop_prob_green;
		cmd.fw.params.fixed_yellow_drop_p =
			queue->wred_fixed_drop_prob_yellow;
		cmd.fw.params.min_avg_yellow = queue->wred_min_avg_yellow;
		cmd.fw.params.max_avg_yellow = queue->wred_max_avg_yellow;
		cmd.fw.params.slope_yellow = queue->wred_slope_yellow;
		cmd.fw.params.min_avg_green = queue->wred_min_avg_green;
		cmd.fw.params.max_avg_green = queue->wred_max_avg_green;
		cmd.fw.params.slope_green = queue->wred_slope_green;
		cmd.fw.params.min_guaranteed = queue->wred_min_guaranteed;
		cmd.fw.params.max_allowed = queue->wred_max_allowed;
		cmd.fw.params.codel_en = queue->codel_en;
	}

	if (!is_orphaned)
		add_suspend_port(qdev, get_port(qdev->nodes, phy));

	cmd_queue_put(qdev->drvcmds.cmdq, &cmd, sizeof(cmd));
}

void create_set_queue_cmd(struct pp_qos_dev *qdev,
			  const struct pp_qos_queue_conf *conf,
			  u32 phy, const struct qos_node *node,
			  u32 modified, bool is_orphaned)
{
	struct fw_set_common common;
	struct fw_set_child child;
	struct fw_set_queue queue;

	if (PP_QOS_DEVICE_IS_ASSERT(qdev))
		return;

	/* Only external parameters may change here */
	memset(&common, 0, sizeof(struct fw_set_common));
	memset(&child, 0, sizeof(struct fw_set_child));
	memset(&queue, 0, sizeof(struct fw_set_queue));

	if (QOS_BITS_IS_SET(modified, QOS_MODIFIED_NODE_TYPE)) {
		create_add_queue_cmd(qdev, conf, phy, node->data.queue.rlm,
				     node->data.queue.is_alias, is_orphaned);
	} else {
		set_common(qdev, &conf->common_prop, &common, modified, phy);
		set_child(&conf->queue_child_prop, node, &child, modified);
		set_queue(conf, &queue, modified);
		_create_set_queue_cmd(qdev, phy, node->data.queue.rlm,
				      node->data.queue.is_alias, modified,
				      &common, &child, &queue, is_orphaned);
	}

	if (!QOS_BITS_IS_SET(modified, QOS_MODIFIED_NODE_TYPE)) {
		QOS_LOG_DEBUG("cmd %u: CMD_TYPE_SET_QUEUE %u\n",
			      qdev->drvcmds.cmd_id, phy);
	}
}

void suspend_node(struct pp_qos_dev *qdev, u32 phy, u32 suspend)
{
	struct qos_node *node;
	struct fw_set_common common;
	struct fw_set_parent parent;
	struct fw_set_child child;
	struct fw_set_port port;
	struct fw_set_queue queue;

	if (PP_QOS_DEVICE_IS_ASSERT(qdev))
		return;

	memset(&common, 0, sizeof(struct fw_set_common));
	memset(&parent, 0, sizeof(struct fw_set_parent));
	memset(&child, 0, sizeof(struct fw_set_child));
	memset(&port, 0, sizeof(struct fw_set_port));
	memset(&queue, 0, sizeof(struct fw_set_queue));

	common.valid = TSCD_NODE_CONF_SUSPEND_RESUME;
	common.suspend = suspend;

	node = get_node_from_phy(qdev->nodes, phy);
	if (!node)
		return;

	switch (node->type) {
	case TYPE_PORT:
		_create_set_port_cmd(qdev, phy, 0,
				     &common, &parent, &port);
	break;

	case TYPE_SCHED:
		_create_set_sched_cmd(qdev, phy, 0,
				      &common, &parent, &child);
	break;

	case TYPE_QUEUE:
		_create_set_queue_cmd(qdev, phy, node->data.queue.rlm,
				      node->data.queue.is_alias, 0,
				      &common, &child, &queue,
				      node_new_syncq(node));
	break;

	default:
		QOS_LOG_ERR("Unnode type for phy %u\n", phy);
	}
}

void create_flush_queue_cmd(struct pp_qos_dev *qdev, u32 rlm)
{
	struct cmd_flush_queue cmd;

	if (PP_QOS_DEVICE_IS_ASSERT(qdev))
		return;

	memset(&cmd, 0, sizeof(cmd));
	cmd_init(qdev, &cmd.base, CMD_TYPE_FLUSH_QUEUE,
		 sizeof(cmd), 0, NULL, 0, 0);
	cmd.fw.base.type = UC_QOS_CMD_FLUSH_QUEUE;
	cmd.fw.rlm = rlm;

	QOS_LOG_DEBUG("cmd %u: CMD_TYPE_FLUSH_QUEUE %u\n",
		      qdev->drvcmds.cmd_id, rlm);

	cmd_queue_put(qdev->drvcmds.cmdq, &cmd, sizeof(cmd));
}

static void create_update_port_tree_cmd(struct pp_qos_dev *qdev, u32 phy)
{
	struct cmd_update_port_tree cmd;
	u32 bitmap_word[QOS_MAX_NUM_QUEUES >> 5];
	u32 num_active_queues;
	u32 queue_idx;
	u32 word_idx;
	u32 bit_in_word;
	u16 *queues;
	s32 ret;

	if (PP_QOS_DEVICE_IS_ASSERT(qdev))
		return;

	memset(&cmd, 0, sizeof(cmd));
	cmd_init(qdev, &cmd.base, CMD_TYPE_UPDATE_PORT_TREE,
		 sizeof(cmd), 0, NULL, 0, 0);
	cmd.fw.base.type = UC_QOS_CMD_UPDATE_PORT_TREE;
	cmd.fw.phy = phy;
	cmd.fw.max_allowed_addr = (u32)qdev->hwconf.max_allow_ddr_phys;

	queues = qdev->hwconf.rlms_container;
	memset(queues, 0, sizeof(u16) * QOS_MAX_NUM_QUEUES);
	memset(bitmap_word, 0, sizeof(bitmap_word));

	get_active_port_rlms(qdev, phy, queues, QOS_MAX_NUM_QUEUES,
			     &num_active_queues);

	ret = store_queue_list_max_allowed(qdev, queues, num_active_queues);
	if (unlikely(ret)) {
		QOS_LOG_ERR("Failed setting queues max allowed buffer\n");
		return;
	}

	for (queue_idx = 0; queue_idx < num_active_queues; queue_idx++) {
		word_idx = *queues >> 5;
		bit_in_word = *queues - (32 * word_idx);
		bitmap_word[word_idx] |= BIT(bit_in_word);
		queues++;
	}

	for (word_idx = 0; word_idx < QOS_MAX_NUM_QUEUES >> 5; word_idx++)
		cmd.fw.bitmap[word_idx] = bitmap_word[word_idx];

	pp_cache_writeback(qdev->hwconf.rlms_container,
			   QOS_MAX_NUM_QUEUES * sizeof(u16));

	update_wsp_info(qdev);

	/* Update BWL buffer before tree update */
	update_bwl_buffer(qdev, phy);

	QOS_LOG_DEBUG("cmd %u: CMD_TYPE_UPDATE_PORT_TREE %u\n",
		      qdev->drvcmds.cmd_id, phy);

	cmd_queue_put(qdev->drvcmds.cmdq, &cmd, sizeof(cmd));
}

static void create_suspend_port_tree_cmd(struct pp_qos_dev *qdev, u32 phy)
{
	struct cmd_suspend_port_tree cmd;

	if (PP_QOS_DEVICE_IS_ASSERT(qdev))
		return;

	memset(&cmd, 0, sizeof(cmd));
	cmd_init(qdev, &cmd.base, CMD_TYPE_SUSPEND_PORT_TREE,
		 sizeof(cmd), 0, NULL, 0, 0);
	cmd.fw.base.type = UC_QOS_CMD_SUSPEND_PORT_TREE;
	cmd.fw.phy = phy;

	QOS_LOG_DEBUG("cmd %u: UC_QOS_CMD_SUSPEND_PORT_TREE %u\n",
		      qdev->drvcmds.cmd_id, phy);

	cmd_queue_put(qdev->drvcmds.cmdq, &cmd, sizeof(cmd));
}

void create_set_wsp_helper_cmd(struct pp_qos_dev *qdev, bool enable,
			       u32 us_timeout, u32 byte_threshold)
{
	struct cmd_set_wsp_helper cmd;

	if (PP_QOS_DEVICE_IS_ASSERT(qdev))
		return;

	memset(&cmd, 0, sizeof(cmd));
	cmd_init(qdev, &cmd.base, CMD_TYPE_SET_WSP_HELPER,
		 sizeof(cmd), 0, NULL, 0, 0);
	cmd.fw.base.type = UC_QOS_CMD_WSP_HELPER_SET;
	cmd.fw.db_address = FW_DCCM_START + PPV4_QOS_UC_WSP_DB_OFFSET;
	cmd.fw.enable = enable;
	cmd.fw.timeout_microseconds = us_timeout;
	cmd.fw.byte_threshold = byte_threshold;

	QOS_LOG_DEBUG("cmd %u: UC_QOS_CMD_WSP_HELPER_SET\n",
		      qdev->drvcmds.cmd_id);

	cmd_queue_put(qdev->drvcmds.cmdq, &cmd, sizeof(cmd));
	update_wsp_info(qdev);
}

void create_get_wsp_helper_stats_cmd(struct pp_qos_dev *qdev, u32 addr,
				     bool reset, struct wsp_stats_t *stats)
{
	struct cmd_get_wsp_helper_stats cmd;

	if (PP_QOS_DEVICE_IS_ASSERT(qdev))
		return;

	memset(&cmd, 0, sizeof(cmd));
	cmd_init(qdev, &cmd.base, CMD_TYPE_GET_WSP_HELPER_STATS, sizeof(cmd),
		 CMD_FLAGS_POST_PROCESS_EXT, (void *)stats, 0,
		 sizeof(struct wsp_stats_t));
	cmd.fw.base.type = UC_QOS_CMD_WSP_HELPER_STATS_GET;
	cmd.fw.reset = reset;
	cmd.fw.addr = addr;

	QOS_LOG_DEBUG("cmd %u: UC_QOS_CMD_WSP_HELPER_STATS_GET\n",
		      qdev->drvcmds.cmd_id);

	cmd_queue_put(qdev->drvcmds.cmdq, &cmd, sizeof(cmd));
}

void update_parent(struct pp_qos_dev *qdev, u32 phy)
{
	struct fw_set_common common;
	struct fw_set_parent parent;
	struct fw_set_child child;
	struct fw_set_port port;
	const struct qos_node *node;
	const struct parent_node_properties *parent_prop;

	if (PP_QOS_DEVICE_IS_ASSERT(qdev))
		return;

	node = get_const_node_from_phy(qdev->nodes, phy);
	if (unlikely(!node)) {
		QOS_LOG_ERR("get_const_node_from_phy(%u) returned NULL\n",
			    phy);
		return;
	}
	parent_prop = &node->parent_prop;

	memset(&common, 0, sizeof(struct fw_set_common));
	memset(&parent, 0, sizeof(struct fw_set_parent));
	memset(&child, 0, sizeof(struct fw_set_child));
	memset(&port, 0, sizeof(struct fw_set_port));

	update_arbitration(&parent.valid, parent_prop, &parent);

	common.valid = 0;
	if (node->type == TYPE_PORT)
		_create_set_port_cmd(qdev, phy, 0, &common, &parent, &port);
	else if (node->type == TYPE_SCHED)
		_create_set_sched_cmd(qdev, phy, 0, &common, &parent, &child);
}

void update_preds(struct pp_qos_dev *qdev, u32 phy, bool queue_port_changed)
{
	struct fw_set_common common;
	struct fw_set_child child;
	struct fw_set_parent parent;
	struct fw_set_queue queue;
	const struct qos_node *node;

	/* Only external parameters may change here */
	memset(&common, 0, sizeof(struct fw_set_common));
	memset(&child, 0, sizeof(struct fw_set_child));
	memset(&parent, 0, sizeof(struct fw_set_parent));
	memset(&queue, 0, sizeof(struct fw_set_queue));

	if (PP_QOS_DEVICE_IS_ASSERT(qdev))
		return;

	node = get_const_node_from_phy(qdev->nodes, phy);
	if (unlikely(!node)) {
		QOS_LOG_ERR("get_const_node_from_phy(%u) returned NULL\n",
			    phy);
		return;
	}

	child.valid = TSCD_NODE_CONF_PREDECESSOR_0 |
		TSCD_NODE_CONF_PREDECESSOR_1 |
		TSCD_NODE_CONF_PREDECESSOR_2 |
		TSCD_NODE_CONF_PREDECESSOR_3 |
		TSCD_NODE_CONF_PREDECESSOR_4 |
		TSCD_NODE_CONF_PREDECESSOR_5;

	fill_preds(qdev->nodes, phy, child.preds, QOS_MAX_PREDECESSORS);

	switch (node->type) {
	case TYPE_SCHED:
		_create_set_sched_cmd(qdev, phy, 0,
				      &common, &parent, &child);
		break;
	case TYPE_QUEUE:
		if (queue_port_changed) {
			common.valid |= TSCD_NODE_CONF_SET_PORT_TO_QUEUE;
			common.port_phy = node->data.queue.port_phy;
		}

		_create_set_queue_cmd(qdev, phy, node->data.queue.rlm,
				      node->data.queue.is_alias, 0,
				      &common, &child, &queue, false);
		break;
	default:
		QOS_LOG_ERR("illegal node type %d\n", node->type);
		return;
	}
}

void stats_identifier_put(struct pp_qos_dev *qdev, u32 cmd_size)
{
	u32 *addr;

	addr = (u32 *)(qdev->hwconf.fw_stats_ddr_virt + cmd_size);
	*addr = FW_OK_CODE;
	pp_cache_writeback(addr, 4);
}

void create_get_port_stats_cmd(struct pp_qos_dev *qdev, u32 phy, u32 addr,
			       struct pp_qos_port_stat *pstat)
{
	struct cmd_get_port_stats cmd;

	if (PP_QOS_DEVICE_IS_ASSERT(qdev))
		return;

	stats_identifier_put(qdev, sizeof(struct port_stats_s));
	memset(&cmd, 0, sizeof(cmd));
	cmd_init(qdev, &cmd.base, CMD_TYPE_GET_PORT_STATS,
		 sizeof(cmd), CMD_FLAGS_POST_PROCESS_EXT, (void *)pstat,
		 0, sizeof(struct port_stats_s));
	cmd.fw.base.type = UC_QOS_CMD_GET_PORT_STATS;
	cmd.fw.phy = phy;
	cmd.fw.addr = addr;
	if (pstat->reset)
		cmd.fw.reset = PORT_STATS_CLEAR_ALL;
	else
		cmd.fw.reset = PORT_STATS_CLEAR_NONE;

	QOS_LOG_DEBUG("cmd %u: CMD_TYPE_GET_PORT_STATS %u\n",
		      qdev->drvcmds.cmd_id, phy);

	cmd_queue_put(qdev->drvcmds.cmdq, &cmd, sizeof(cmd));
}

void create_get_queue_stats_cmd(struct pp_qos_dev *qdev,
				u32 phy, u32 rlm, u32 addr,
				struct queue_stats_s *qstat,
				bool reset)
{
	struct cmd_get_queue_stats cmd;

	if (PP_QOS_DEVICE_IS_ASSERT(qdev))
		return;

	stats_identifier_put(qdev, sizeof(struct queue_stats_s));
	memset(&cmd, 0, sizeof(cmd));
	cmd_init(qdev, &cmd.base, CMD_TYPE_GET_QUEUE_STATS,
		 sizeof(cmd), CMD_FLAGS_POST_PROCESS_EXT, (void *)qstat,
		 0, sizeof(struct queue_stats_s));
	cmd.fw.base.type = UC_QOS_CMD_GET_QUEUE_STATS;
	cmd.fw.phy = phy;
	cmd.fw.rlm = rlm;
	cmd.fw.addr =  addr;

	if (reset) {
		cmd.fw.reset = QUEUE_STATS_CLEAR_Q_AVG_SIZE_BYTES |
			QUEUE_STATS_CLEAR_DROP_P_YELLOW |
			QUEUE_STATS_CLEAR_DROP_P_GREEN |
			QUEUE_STATS_CLEAR_TOTAL_BYTES_ADDED |
			QUEUE_STATS_CLEAR_TOTAL_ACCEPTS |
			QUEUE_STATS_CLEAR_TOTAL_DROPS |
			QUEUE_STATS_CLEAR_TOTAL_DROPPED_BYTES |
			QUEUE_STATS_CLEAR_TOTAL_RED_DROPS;
	} else {
		cmd.fw.reset = QUEUE_STATS_CLEAR_NONE;
	}

	QOS_LOG_DEBUG("cmd %u: CMD_TYPE_GET_QUEUE_STATS %u\n",
		      qdev->drvcmds.cmd_id, phy);

	cmd_queue_put(qdev->drvcmds.cmdq, &cmd, sizeof(cmd));
}

void create_get_qm_stats_cmd(struct pp_qos_dev *qdev, u32 rlm, u32 addr,
			     struct qm_info *qstat)
{
	struct cmd_get_qm_stats cmd;

	if (PP_QOS_DEVICE_IS_ASSERT(qdev))
		return;

	stats_identifier_put(qdev, sizeof(struct qm_info));
	memset(&cmd, 0, sizeof(cmd));
	cmd_init(qdev, &cmd.base, CMD_TYPE_GET_QM_STATS,
		 sizeof(cmd), CMD_FLAGS_POST_PROCESS_EXT, (void *)qstat,
		 0, sizeof(struct qm_info));
	cmd.fw.base.type = UC_QOS_CMD_GET_ACTIVE_QUEUES_STATS;
	cmd.fw.first_rlm = rlm;
	cmd.fw.last_rlm = rlm + NUM_Q_ON_QUERY - 1;
	cmd.fw.addr =  addr;

	QOS_LOG_DEBUG("cmd %u: CMD_TYPE_GET_QM_STATS\n",
		      qdev->drvcmds.cmd_id);

	cmd_queue_put(qdev->drvcmds.cmdq, &cmd, sizeof(cmd));
}

void create_aqm_sf_set_cmd(struct pp_qos_dev *qdev, u8 sf_id, u8 enable,
			   struct pp_qos_aqm_sf_config *sf_cfg, u32 *queue_id)
{
	struct cmd_set_aqm_sf cmd;
	u32 queue_idx;

	if (PP_QOS_DEVICE_IS_ASSERT(qdev))
		return;

	memset(&cmd, 0, sizeof(cmd));
	cmd_init(qdev, &cmd.base, CMD_TYPE_SET_AQM_SF,
		 sizeof(cmd), 0, NULL, 0, 0);
	cmd.fw.base.type = UC_QOS_CMD_SET_AQM_SF;
	cmd.fw.aqm_context = sf_id;
	cmd.fw.enable = enable;
	cmd.fw.latency_target_ms = sf_cfg->latency_target_ms;
	cmd.fw.peak_rate = sf_cfg->peak_rate;
	cmd.fw.msr = sf_cfg->msr;
	cmd.fw.buffer_size = sf_cfg->buffer_size;
	cmd.fw.msrtokens_addr_offset =
		FW_DCCM_START + PPV4_QOS_AQM_BUF_OFFSET + (sizeof(u32) * sf_id);
	cmd.fw.num_rlms = sf_cfg->num_queues;

	for (queue_idx = 0; queue_idx < sf_cfg->num_queues; queue_idx++)
		cmd.fw.rlms[queue_idx] = queue_id[queue_idx];

	QOS_LOG_DEBUG("cmd %u: CMD_TYPE_SET_AQM_SF %u\n",
		      qdev->drvcmds.cmd_id, sf_id);

	cmd_queue_put(qdev->drvcmds.cmdq, &cmd, sizeof(cmd));
}

void create_codel_cfg_set_cmd(struct pp_qos_dev *qdev,
			      struct pp_qos_codel_cfg *cfg)
{
	struct cmd_set_codel_cfg cmd;

	if (PP_QOS_DEVICE_IS_ASSERT(qdev))
		return;

	memset(&cmd, 0, sizeof(cmd));
	cmd_init(qdev, &cmd.base, CMD_TYPE_SET_CODEL_CFG,
		 sizeof(cmd), 0, NULL, 0, 0);
	cmd.fw.base.type = UC_QOS_CMD_SET_CODEL_CFG;
	cmd.fw.consecutive_drop_limit = cfg->consecutive_drop_limit;
	cmd.fw.target_delay_msec = cfg->target_delay_msec;
	cmd.fw.interval_time_msec = cfg->interval_time_msec;

	QOS_LOG_DEBUG("cmd %u: CMD_TYPE_SET_CODEL_CFG\n",
		      qdev->drvcmds.cmd_id);

	cmd_queue_put(qdev->drvcmds.cmdq, &cmd, sizeof(cmd));
}

static void __create_nodes_refresh_cmd(struct pp_qos_dev *qdev, ulong *nodes,
				       u32 n)
{
	struct cmd_nodes_refresh cmd = { 0 };
	u32 nodes_per_cmd, bit, word_idx, bit_idx, bits_per_word;
	bool wr_cmd;

	if (PP_QOS_DEVICE_IS_ASSERT(qdev))
		return;

	cmd_init(qdev, &cmd.base, CMD_TYPE_NODES_REFRESH,
		 sizeof(cmd), 0, NULL, 0, 0);
	QOS_LOG_DEBUG("cmd %u: %s\n", qdev->drvcmds.cmd_id,
		      cmd_str[cmd.base.id]);
	nodes_per_cmd = sizeof(cmd.fw.nodes_bmap) * BITS_PER_BYTE;
	bits_per_word = sizeof(cmd.fw.nodes_bmap[0]) * BITS_PER_BYTE;
	cmd.fw.base.type = UC_QOS_CMD_NODES_REFRESH;
	/* due to command length limitations we split the command
	 * into groups of nodes
	 */
	while (cmd.fw.base_phy_id < n) {
		memset(cmd.fw.nodes_bmap, 0, sizeof(cmd.fw.nodes_bmap));
		wr_cmd = false;
		/* convert 'nodes_per_cmd' bits to a 32 bits words bitmap */
		for_each_set_bit(bit, nodes, nodes_per_cmd) {
			word_idx = bit / bits_per_word;
			bit_idx  = bit % bits_per_word;
			cmd.fw.nodes_bmap[word_idx] |= BIT(bit_idx);
			wr_cmd = true;
		}
		/* only if at least one of the nodes in the group
		 * needs to be updated we send the command
		 */
		if (wr_cmd)
			cmd_queue_put(qdev->drvcmds.cmdq, &cmd, sizeof(cmd));

		/* prepare next command */
		cmd.fw.base_phy_id += nodes_per_cmd;
		nodes += BITS_TO_LONGS(nodes_per_cmd);
	}
}

void create_clk_update_cmd(struct pp_qos_dev *qdev, u32 clk_MHz, ulong *nodes,
			   u32 n)
{
	struct cmd_clk_update cmd = { 0 };

	if (PP_QOS_DEVICE_IS_ASSERT(qdev))
		return;

	cmd_init(qdev, &cmd.base, CMD_TYPE_CLK_UPDATE,
		 sizeof(cmd), 0, NULL, 0, 0);

	QOS_LOG_DEBUG("cmd %u: %s\n", qdev->drvcmds.cmd_id,
		      cmd_str[cmd.base.id]);

	cmd.fw.base.type = UC_QOS_CMD_CLK_UPDATE;
	cmd.fw.clk_MHz   = clk_MHz;

	cmd_queue_put(qdev->drvcmds.cmdq, &cmd, sizeof(cmd));
	__create_nodes_refresh_cmd(qdev, nodes, n);
}

void create_get_codel_stats_cmd(struct pp_qos_dev *qdev, u32 rlm, u32 addr,
				struct pp_qos_codel_stat *qstat)
{
	struct cmd_get_codel_stat cmd;

	if (PP_QOS_DEVICE_IS_ASSERT(qdev))
		return;

	stats_identifier_put(qdev, sizeof(struct codel_qstats_s));
	memset(&cmd, 0, sizeof(cmd));
	cmd_init(qdev, &cmd.base, CMD_TYPE_GET_CODEL_STATS,
		 sizeof(cmd), CMD_FLAGS_POST_PROCESS_EXT, (void *)qstat,
		 0, sizeof(struct codel_qstats_s));

	cmd.fw.base.type = UC_QOS_CMD_GET_CODEL_QUEUE_STATS;
	cmd.fw.rlm = rlm;
	cmd.fw.addr = addr;
	cmd.fw.reset = qstat->reset;

	QOS_LOG_DEBUG("cmd %u: CMD_TYPE_GET_CODEL_STATS rlm %u\n",
		      qdev->drvcmds.cmd_id, rlm);

	cmd_queue_put(qdev->drvcmds.cmdq, &cmd, sizeof(cmd));
}

void create_get_node_info_cmd(struct pp_qos_dev *qdev, u32 phy, u32 addr,
			      struct pp_qos_node_info *info)
{
	struct cmd_get_node_info cmd;

	if (PP_QOS_DEVICE_IS_ASSERT(qdev))
		return;

	stats_identifier_put(qdev, sizeof(struct hw_node_info_s));
	memset(&cmd, 0, sizeof(cmd));
	cmd_init(qdev, &cmd.base, CMD_TYPE_GET_NODE_INFO,
		 sizeof(cmd), CMD_FLAGS_POST_PROCESS_EXT, (void *)info,
		 0, sizeof(struct hw_node_info_s));
	cmd.fw.base.type = UC_QOS_CMD_GET_NODE_INFO;
	cmd.fw.phy = phy;
	cmd.fw.addr = addr;

	QOS_LOG_DEBUG("cmd %u: CMD_TYPE_GET_NODE_INFO %u\n",
		      qdev->drvcmds.cmd_id, phy);

	cmd_queue_put(qdev->drvcmds.cmdq, &cmd, sizeof(cmd));
}

void create_get_table_entry_cmd(struct pp_qos_dev *qdev, u32 phy,
				u32 addr, u32 table_type)
{
	struct cmd_read_table_entry cmd;

	if (PP_QOS_DEVICE_IS_ASSERT(qdev))
		return;

	memset(&cmd, 0, sizeof(cmd));
	cmd_init(qdev, &cmd.base, CMD_TYPE_READ_TABLE_ENTRY,
		 sizeof(cmd), 0, NULL, 0, 0);
	cmd.fw.base.type = UC_QOS_CMD_DEBUG_READ_NODE;
	cmd.fw.phy = phy;
	cmd.fw.table_type = table_type;
	cmd.fw.addr =  addr;

	QOS_LOG_DEBUG("cmd %u: CMD_TYPE_READ_TABLE_ENTRY %u from phy %u\n",
		      qdev->drvcmds.cmd_id, table_type, phy);

	cmd_queue_put(qdev->drvcmds.cmdq, &cmd, sizeof(cmd));
}

void create_get_sys_info_cmd(struct pp_qos_dev *qdev,
			     u32 addr, struct system_stats_s *info,
			     bool dump_entries)
{
	struct cmd_get_system_info cmd;

	if (PP_QOS_DEVICE_IS_ASSERT(qdev))
		return;

	stats_identifier_put(qdev, sizeof(struct system_stats_s));
	memset(&cmd, 0, sizeof(cmd));
	cmd_init(qdev, &cmd.base, CMD_TYPE_GET_SYSTEM_INFO,
		 sizeof(cmd), CMD_FLAGS_POST_PROCESS_EXT, (void *)info,
		 0, sizeof(struct system_stats_s));
	cmd.fw.base.type = UC_QOS_CMD_GET_SYSTEM_STATS;
	cmd.fw.addr = addr;
	cmd.fw.dump = dump_entries;

	QOS_LOG_DEBUG("cmd %u: CMD_TYPE_GET_SYSTEM_INFO\n",
		      qdev->drvcmds.cmd_id);
	cmd_queue_put(qdev->drvcmds.cmdq, &cmd, sizeof(cmd));
}

void create_get_quanta_cmd(struct pp_qos_dev *qdev, u32 *quanta)
{
	struct cmd_get_quanta cmd;

	if (PP_QOS_DEVICE_IS_ASSERT(qdev))
		return;

	memset(&cmd, 0, sizeof(cmd));
	cmd_init(qdev, &cmd.base, CMD_TYPE_GET_QUANTA, sizeof(cmd),
		 CMD_FLAGS_POST_PROCESS_INT, quanta,
		 offsetof(struct fw_cmd_get_quanta, quanta),
		 CMD_INT_RESPONSE_SZ);
	cmd.fw.base.type = UC_QOS_CMD_GET_QUANTA;

	QOS_LOG_DEBUG("cmd %u: CMD_TYPE_GET_QUANTA\n",
		      qdev->drvcmds.cmd_id);
	cmd_queue_put(qdev->drvcmds.cmdq, &cmd, sizeof(cmd));
}

void create_mod_log_bmap_set_cmd(struct pp_qos_dev *qdev, u32 bmap)
{
	struct cmd_mod_log_bmap cmd;

	if (PP_QOS_DEVICE_IS_ASSERT(qdev))
		return;

	memset(&cmd, 0, sizeof(cmd));
	cmd_init(qdev, &cmd.base, CMD_TYPE_SET_MOD_LOG_BMAP,
		 sizeof(cmd), 0, NULL, 0, 0);
	cmd.fw.base.type = UC_QOS_CMD_MOD_REG_BMAP_SET;
	cmd.fw.bmap = bmap;
	QOS_LOG_DEBUG("cmd %u: UC_QOS_CMD_MOD_REG_BMAP_SET\n",
		      qdev->drvcmds.cmd_id);

	cmd_queue_put(qdev->drvcmds.cmdq, &cmd, sizeof(cmd));
}

void create_mod_log_bmap_get_cmd(struct pp_qos_dev *qdev, u32 *bmap)
{
	struct cmd_mod_log_bmap cmd;

	if (PP_QOS_DEVICE_IS_ASSERT(qdev))
		return;

	memset(&cmd, 0, sizeof(cmd));
	cmd_init(qdev, &cmd.base, CMD_TYPE_GET_MOD_LOG_BMAP, sizeof(cmd),
		 CMD_FLAGS_POST_PROCESS_INT, bmap,
		 offsetof(struct fw_cmd_mod_log_bmap, bmap),
		 sizeof(*bmap));
	cmd.fw.base.type = UC_QOS_CMD_MOD_REG_BMAP_GET;

	QOS_LOG_DEBUG("cmd %u: CMD_TYPE_GET_MOD_LOG_BMAP\n",
		      qdev->drvcmds.cmd_id);
	cmd_queue_put(qdev->drvcmds.cmdq, &cmd, sizeof(cmd));
}

void create_add_shared_group_cmd(struct pp_qos_dev *qdev, u32 id,
				 u32 max_burst)
{
	struct cmd_add_shared_group cmd;

	if (PP_QOS_DEVICE_IS_ASSERT(qdev))
		return;

	memset(&cmd, 0, sizeof(cmd));
	cmd_init(qdev, &cmd.base, CMD_TYPE_ADD_SHARED_GROUP,
		 sizeof(cmd), 0, NULL, 0, 0);
	cmd.fw.base.type = UC_QOS_CMD_ADD_SHARED_BW_LIMIT_GROUP;
	cmd.fw.id = id;
	cmd.fw.max_burst = max_burst;

	QOS_LOG_DEBUG("cmd %u: CMD_TYPE_ADD_SHARED_GROUP %u\n",
		      qdev->drvcmds.cmd_id, id);

	cmd_queue_put(qdev->drvcmds.cmdq, &cmd, sizeof(cmd));
}

void create_set_shared_group_cmd(struct pp_qos_dev *qdev, u32 id,
				 u32 max_burst)
{
	struct cmd_add_shared_group cmd;

	if (PP_QOS_DEVICE_IS_ASSERT(qdev))
		return;

	memset(&cmd, 0, sizeof(cmd));
	cmd_init(qdev, &cmd.base, CMD_TYPE_SET_SHARED_GROUP,
		 sizeof(cmd), 0, NULL, 0, 0);
	cmd.fw.base.type = UC_QOS_CMD_SET_SHARED_BW_LIMIT_GROUP;
	cmd.fw.id = id;
	cmd.fw.max_burst = max_burst;

	QOS_LOG_DEBUG("cmd %u: CMD_TYPE_SET_SHARED_GROUP %u\n",
		      qdev->drvcmds.cmd_id, id);

	cmd_queue_put(qdev->drvcmds.cmdq, &cmd, sizeof(cmd));
}

void create_remove_shared_group_cmd(struct pp_qos_dev *qdev, u32 id)
{
	struct cmd_remove_shared_group cmd;

	if (PP_QOS_DEVICE_IS_ASSERT(qdev))
		return;

	memset(&cmd, 0, sizeof(cmd));
	cmd_init(qdev, &cmd.base, CMD_TYPE_REM_SHARED_GROUP,
		 sizeof(cmd), 0, NULL, 0, 0);
	cmd.fw.base.type = UC_QOS_CMD_REM_SHARED_BW_LIMIT_GROUP;
	cmd.fw.id = id;

	QOS_LOG_DEBUG("cmd %u: CMD_TYPE_REM_SHARED_GROUP %u\n",
		      qdev->drvcmds.cmd_id, id);

	cmd_queue_put(qdev->drvcmds.cmdq, &cmd, sizeof(cmd));
}

void create_push_desc_cmd(struct pp_qos_dev *qdev, u32 queue, u32 size,
			  u32 pool, u32 policy, u32 color, u32 gpid,
			  u32 data_off, u32 addr)
{
	struct cmd_push_desc cmd;

	if (PP_QOS_DEVICE_IS_ASSERT(qdev))
		return;

	memset(&cmd, 0, sizeof(cmd));
	cmd_init(qdev, &cmd.base, CMD_TYPE_PUSH_DESC,
		 sizeof(cmd), 0, NULL, 0, 0);
	cmd.fw.base.type = UC_QOS_CMD_DEBUG_PUSH_DESC;
	cmd.fw.rlm = queue;
	cmd.fw.size = size;
	cmd.fw.pool = pool;
	cmd.fw.policy = policy;
	cmd.fw.color = color;
	cmd.fw.gpid = gpid;
	cmd.fw.data_offset = data_off;
	cmd.fw.addr = addr;

	cmd_queue_put(qdev->drvcmds.cmdq, &cmd, sizeof(cmd));
}

void create_pop_desc_cmd(struct pp_qos_dev *qdev, u32 queue, u32 addr)
{
	struct cmd_pop_desc cmd;

	if (PP_QOS_DEVICE_IS_ASSERT(qdev))
		return;

	memset(&cmd, 0, sizeof(cmd));
	cmd_init(qdev, &cmd.base, CMD_TYPE_POP_DESC,
		 sizeof(cmd), 0, NULL, 0, 0);
	cmd.fw.base.type = UC_QOS_CMD_DEBUG_POP_DESC;
	cmd.fw.rlm = queue;
	cmd.fw.addr = addr;

	cmd_queue_put(qdev->drvcmds.cmdq, &cmd, sizeof(cmd));
}

void create_mcdma_copy_cmd(struct pp_qos_dev *qdev, u32 src, u32 dst, u32 sz)
{
	struct cmd_mcdma_copy cmd;

	if (PP_QOS_DEVICE_IS_ASSERT(qdev))
		return;

	memset(&cmd, 0, sizeof(cmd));
	cmd_init(qdev, &cmd.base, CMD_TYPE_MCDMA_COPY,
		 sizeof(cmd), 0, NULL, 0, 0);
	cmd.fw.base.type = UC_QOS_CMD_DEBUG_MCDMA_COPY;
	cmd.fw.src_addr = src;
	cmd.fw.dst_addr = dst;
	cmd.fw.size = sz;

	cmd_queue_put(qdev->drvcmds.cmdq, &cmd, sizeof(cmd));
}

/******************************************************************************/
/*                         FW write functions                                 */
/******************************************************************************/
void add_suspend_port(struct pp_qos_dev *qdev, u32 port_id)
{
	struct fw_internal *internals;
	struct qos_node *node;
	u32 i;

	node = get_node_from_phy(qdev->nodes, port_id);
	if (unlikely(!node)) {
		QOS_LOG_ERR("get_node_from_phy(%u) returned NULL\n", port_id);
		return;
	}

	QOS_ASSERT(node_port(node), "Node %u is not a port\n", port_id);
	internals = qdev->fwbuf;

	for (i = 0; i < internals->num_suspend_ports; ++i) {
		if (internals->suspend_ports[i] == port_id)
			return;
	}

	QOS_ASSERT(internals->num_suspend_ports < qdev->init_params.max_ports,
		   "Suspend ports buffer is full\n");
	internals->suspend_ports[internals->num_suspend_ports] = port_id;
	++(internals->num_suspend_ports);

	create_suspend_port_tree_cmd(qdev, port_id);
}

s32 init_fwdata_internals(struct pp_qos_dev *qdev)
{
	QOS_LOG_DEBUG("\n");

	qdev->fwbuf = kzalloc(sizeof(struct fw_internal), GFP_KERNEL);
	if (qdev->fwbuf) {
		memset(qdev->fwbuf, 0, sizeof(struct fw_internal));
		return 0;
	}
	return -EBUSY;
}

void clean_fwdata_internals(struct pp_qos_dev *qdev)
{
	if (qdev) {
		kfree(qdev->fwbuf);
		qdev->fwbuf = NULL;
	}
}

static ulong __fw_cmd_wr(ulong buf, void *cmd, u32 cmd_sz, u32 flags)
{
	struct uc_qos_cmd_base *fw_base = (struct uc_qos_cmd_base *)cmd;
	u32 *_cmd = (u32 *)cmd;
	u32 *_buf = (u32 *)buf;

	cmd_sz /= sizeof(u32);
	fw_base->flags      = flags;
	fw_base->num_params = cmd_sz - sizeof(*fw_base) / sizeof(u32);

	pr_debug("buf %#lx, writing %u command, flags %#x, cmd_sz %u\n", buf,
		 fw_base->type, flags, cmd_sz);

	while (cmd_sz--)
		PP_REG_WR32((ulong)_buf++, *_cmd++);

	return (ulong)_buf;
}

/* Signal firmware to read and executes commands from cmd
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

	PP_REG_WR32((ulong)qdev->init_params.fwcom.mbx_to_uc + 4, DRV_SIGNAL);
}

/******************************************************************************/
/*                                Engine                                      */
/******************************************************************************/

/* Translation to user format and sanity checks on node info
 * obtained from firmware
 */
static s32 post_process_get_node_info(struct pp_qos_dev *qdev,
				      struct cmd_get_node_info *cmd,
				      void *response)
{
	struct pp_qos_node_info *info;
	struct qos_node *node, *parent;
	struct qos_node *child;
	struct hw_node_info_s *hw_node_info;
	u32 preds[QOS_MAX_PREDECESSORS];
	u32 fw_pred[QOS_MAX_PREDECESSORS];
	u32 first, last;
	u32 phy;
	u32 id;
	u32 i;
	u32 num;
	u32 port;
	s32 reached_port;
	u32 queue_port;
	u32 queue_physical_id;
	u32 bw_limit, weight;

	port = PPV4_QOS_INVALID;
	node = get_node_from_phy(qdev->nodes, cmd->fw.phy);
	if (unlikely(!node)) {
		QOS_LOG_ERR("get_node_from_phy(%u) returned NULL\n",
			    cmd->fw.phy);
		return -EINVAL;
	}

	if (node_new_syncq(node)) {
		QOS_LOG_DEBUG("new (disconnected) syncq node(%u)\n",
			      cmd->fw.phy);
		return 0;
	}

	QOS_ASSERT(node_used(node), "Node %u is not used\n", cmd->fw.phy);
	if (!node_used(node))
		return -EINVAL;

	info = (struct pp_qos_node_info *)response;
	memset(info, 0xFF, sizeof(*info));

	QOS_ASSERT(node->type != TYPE_UNKNOWN,
		   "Node %u has unknown type\n",
		   cmd->fw.phy);
	info->type = node->type - 1;
	info->is_internal = node_internal(node);

	hw_node_info = (struct hw_node_info_s *)qdev->hwconf.fw_stats_ddr_virt;

	if (node_parent(node)) {
		first = hw_node_info->first_child;
		last  = hw_node_info->last_child;

		if (first == 0) {
			QOS_ASSERT(last == 0,
				   "HW reports first child 0 but last child is %u\n",
				   last);
			first = QOS_INVALID_PHY;
			num = 0;
		} else {
			num = last - first + 1;
			child = get_node_from_phy(qdev->nodes, first);
			if (unlikely(!child)) {
				QOS_LOG_ERR("get_node_from_phy NULL\n");
				return -EINVAL;
			}
		}
		QOS_ASSERT(node->parent_prop.num_of_children == num,
			   "Driver has %u as the number of children of node %u, while HW has %u\n",
			   node->parent_prop.num_of_children, cmd->fw.phy, num);

		if (node->parent_prop.num_of_children != num)
			return -EINVAL;

		QOS_ASSERT(num == 0 ||
			   node->parent_prop.first_child_phy == first,
			   "Driver has %u as the phy of node's %u first child, while HW has %u\n",
			   node->parent_prop.first_child_phy,
			   cmd->fw.phy,
			   first);

		if ((num != 0) && (node->parent_prop.first_child_phy != first))
			return -EINVAL;

		for (i = 0; i < num; ++i) {
			phy = i + first;
			id = get_id_from_phy(qdev->mapping, phy);
			QOS_ASSERT(QOS_ID_VALID(id),
				   "Child of %u with phy %u has no valid id\n",
				   cmd->fw.phy, phy);
			QOS_ASSERT(node_used(child),
				   "Child node with phy %u and id %u is not used\n",
				   phy, id);
			info->children[i].phy = phy;
			info->children[i].id = id;
			++child;
		}
	}

	if (!node_port(node)) {
		weight = hw_node_info->weight;
		parent = get_node_from_phy(qdev->nodes,
					   node->child_prop.parent_phy);

		QOS_ASSERT(node->child_prop.bw_weight == weight,
			   "Node %u weight diff (driver %u, HW %u\n",
			   cmd->fw.phy, node->child_prop.bw_weight, weight);

		if (node->child_prop.bw_weight != weight)
			return -EINVAL;

		if (parent->parent_prop.arbitration ==
		    PP_QOS_ARBITRATION_WSP) {
			QOS_ASSERT(node->child_prop.bw_weight ==
				   QOS_CHILD_UNLIMITED_WEIGHT,
				   "Node %u weight should be %u under WSP\n",
				   cmd->fw.phy, QOS_CHILD_UNLIMITED_WEIGHT);

			if (node->child_prop.bw_weight !=
			    QOS_CHILD_UNLIMITED_WEIGHT)
				return -EINVAL;
		}

		if (parent->parent_prop.arbitration ==
		    PP_QOS_ARBITRATION_WRR) {
			QOS_ASSERT(node->child_prop.bw_weight !=
				   QOS_CHILD_UNLIMITED_WEIGHT,
				   "Node %u weight shouldnt be %u under WRR\n",
				   cmd->fw.phy, QOS_CHILD_UNLIMITED_WEIGHT);

			if (node->child_prop.bw_weight ==
			    QOS_CHILD_UNLIMITED_WEIGHT)
				return -EINVAL;
		}

		fill_preds(qdev->nodes, cmd->fw.phy, preds,
			   QOS_MAX_PREDECESSORS);

		memcpy(fw_pred, &hw_node_info->predecessor0, sizeof(fw_pred));

		reached_port = 0;
		for (i = 0; i < QOS_MAX_PREDECESSORS; ++i) {
			QOS_ASSERT(preds[i] == fw_pred[i],
				   "Driver has %u as the %u predecessor of node %u, while HW has %u\n",
				   preds[i], i, cmd->fw.phy, fw_pred[i]);

			if (preds[i] != fw_pred[i])
				return -EINVAL;

			if (reached_port) {
				info->preds[i].phy = PPV4_QOS_INVALID;
				continue;
			}

			info->preds[i].phy = preds[i];
			id = get_id_from_phy(qdev->mapping, preds[i]);
			QOS_ASSERT(QOS_ID_VALID(id),
				   "Pred with phy %u has no valid id\n",
				   preds[i]);
			info->preds[i].id = id;
			if (preds[i] < qdev->init_params.max_ports) {
				reached_port = 1;
				port = preds[i];
			}
		}
	}

	if (node_queue(node)) {
		queue_physical_id = hw_node_info->queue_physical_id;
		queue_port = hw_node_info->queue_port;

		if (QOS_BITS_IS_SET(node->flags, NODE_FLAGS_REPLICATION_NODE))
			queue_physical_id -= qdev->init_params.max_queues;

		QOS_ASSERT(node->data.queue.rlm == queue_physical_id,
			   "Node %u physical queue is %u according to driver and %u according to HW\n",
			   cmd->fw.phy, node->data.queue.rlm,
			   queue_physical_id);

		if (node->data.queue.rlm != queue_physical_id)
			return -EINVAL;

		QOS_ASSERT(port == queue_port,
			   "Driver has %u as %u port, while HW has %u\n",
			   port, cmd->fw.phy, queue_port);

		if (port != queue_port)
			return -EINVAL;

		info->queue_physical_id = queue_physical_id;
		info->port = queue_port;
	}


	bw_limit = hw_node_info->bw_limit;
	if (bw_limit != node->actual_bandwidth_limit) {
		QOS_ASSERT(bw_limit == node->actual_bandwidth_limit,
			   "Driver has %u as node's %u bandwidth limit, while HW has %u\n",
			   node->actual_bandwidth_limit,
			   cmd->fw.phy,
			   bw_limit);

		if (bw_limit != node->bandwidth_limit)
			return -EINVAL;
	}

	info->bw_limit = bw_limit;

	return 0;
}

static void post_process_internal(struct pp_qos_dev *qdev,
				  union driver_cmd *dcmd,
				  u32 response)
{
	struct qos_node *node = NULL;
	enum cmd_type type;

	type = dcmd->cmd.type;
	switch (type) {
	case CMD_TYPE_ADD_PORT:
		node = get_node_from_phy(qdev->nodes,
					 dcmd->add_port.fw.port_phy);
		node->actual_bandwidth_limit = response;
		break;

	case CMD_TYPE_SET_PORT:
		node = get_node_from_phy(qdev->nodes,
					 dcmd->set_port.fw.port_phy);
		node->actual_bandwidth_limit = response;
		break;

	case CMD_TYPE_ADD_SCHED:
		node = get_node_from_phy(qdev->nodes,
					 dcmd->add_sched.fw.phy);
		node->actual_bandwidth_limit = response;
		break;

	case CMD_TYPE_SET_SCHED:
		node = get_node_from_phy(qdev->nodes,
					 dcmd->set_sched.fw.phy);
		node->actual_bandwidth_limit = response;
		break;

	case CMD_TYPE_ADD_QUEUE:
		node = get_node_from_phy(qdev->nodes,
					 dcmd->add_queue.fw.phy);
		node->actual_bandwidth_limit = response;
		break;

	case CMD_TYPE_SET_QUEUE:
		node = get_node_from_phy(qdev->nodes,
					 dcmd->set_queue.fw.phy);
		node->actual_bandwidth_limit = response;
		break;

	case CMD_TYPE_GET_QUANTA:
		*(u32 *)dcmd->cmd.response = response;
		break;

	case CMD_TYPE_GET_MOD_LOG_BMAP:
		*(u32 *)dcmd->cmd.response = response;
		break;

	default:
		QOS_ASSERT(0, "Unexpected cmd %d for post process\n", type);
		return;
	}
}

/* Commands that are marked with POST_PROCESS reach
 * here for further processing before return to client
 */
static s32 post_process_external(struct pp_qos_dev *qdev,
				 union driver_cmd *dcmd)
{
	enum cmd_type type;
	struct queue_stats_s *qstat;
	struct pp_qos_codel_stat *cstat;
	struct codel_qstats_s *cstat_ptr;
	struct port_stats_s *pstat_ptr;
	struct pp_qos_port_stat *pstat;
	struct system_stats_s *sys_stats, *sys_stats_ptr;
	struct qm_info *qminfo, *qminfo_ptr;
	struct wsp_stats_t *wsp_stats_resp, *wsp_stats_ptr;
	u32 stats_poll = 0;
	u32 fw_signal = FW_OK_CODE;
	volatile u32 *fw_signal_addr;
	void *response = NULL;
	s32 ret = 0;

	if (PP_QOS_DEVICE_IS_ASSERT(qdev))
		return -EINVAL;

	/* Validate statistics buffer is ready before read */
	while (fw_signal == FW_OK_CODE) {
		fw_signal_addr =
			(volatile u32 *)(qdev->hwconf.fw_stats_ddr_virt +
					 dcmd->cmd.response_sz);
		pp_cache_invalidate(fw_signal_addr, 4);
		fw_signal = *fw_signal_addr;
		if (stats_poll++ > PP_REG_DFLT_RETRIES) {
			QOS_LOG_ERR("DDR stats poll exceeded %u thr\n",
				    PP_REG_DFLT_RETRIES);
			return -EINVAL;
		}
	}

	response = dcmd->cmd.response;
	if (!response) {
		QOS_LOG_ERR("Response ptr is NULL for %s\n",
			    cmd_str[dcmd->cmd.type]);
		return -EINVAL;
	}

	type = dcmd->cmd.type;
	switch (type) {
	case CMD_TYPE_GET_QUEUE_STATS:
		qstat = (struct queue_stats_s *)response;
		memcpy((void *)qstat,
		       (void *)qdev->hwconf.fw_stats_ddr_virt,
		       sizeof(*qstat));
		break;

	case CMD_TYPE_GET_QM_STATS:
		qminfo = (struct qm_info *)response;
		qminfo_ptr = ((struct qm_info *)qdev->hwconf.fw_stats_ddr_virt);
		qminfo->num_queues = qminfo_ptr->num_queues;

		if (qminfo->num_queues > NUM_Q_ON_QUERY) {
			QOS_LOG_ERR("QM stats: Num queues %d exceeds max %u\n",
				    qminfo->num_queues,
				    NUM_Q_ON_QUERY);
			break;
		}

		memcpy((void *)qminfo->qstat, (void *)qminfo_ptr->qstat,
		       sizeof(qminfo->qstat[0]) * qminfo->num_queues);
		break;

	case CMD_TYPE_GET_CODEL_STATS:
		cstat = (struct pp_qos_codel_stat *)response;
		cstat_ptr =
		(struct codel_qstats_s *)qdev->hwconf.fw_stats_ddr_virt;
		cstat->packets_dropped = cstat_ptr->drop_pkts;
		cstat->bytes_dropped = cstat_ptr->drop_bytes;
		cstat->min_sojourn_time = cstat_ptr->min_packet_sojourn_time;
		cstat->max_sojourn_time = cstat_ptr->max_packet_sojourn_time;
		cstat->total_sojourn_time = cstat_ptr->total_sojourn_time;
		cstat->max_packet_size = cstat_ptr->max_pkt_sz;
		cstat->total_packets = cstat_ptr->num_pkts;
		break;

	case CMD_TYPE_GET_PORT_STATS:
		pstat = (struct pp_qos_port_stat *)response;
		pstat_ptr =
		(struct port_stats_s *)qdev->hwconf.fw_stats_ddr_virt;
		pstat->total_green_bytes = pstat_ptr->total_green_bytes;
		pstat->total_yellow_bytes = pstat_ptr->total_yellow_bytes;

		pstat->debug_back_pressure_status =
		pstat_ptr->debug_back_pressure_status;

		pstat->debug_actual_packet_credit =
		pstat_ptr->debug_actual_packet_credit;

		pstat->debug_actual_byte_credit =
		pstat_ptr->debug_actual_byte_credit;
		break;

	case CMD_TYPE_GET_SYSTEM_INFO:
		sys_stats = (struct system_stats_s *)response;
		sys_stats_ptr =
		(struct system_stats_s *)qdev->hwconf.fw_stats_ddr_virt;
		sys_stats->tscd_num_of_used_nodes =
		sys_stats_ptr->tscd_num_of_used_nodes;

		sys_stats->tscd_infinite_loop_error_occurred =
		sys_stats_ptr->tscd_infinite_loop_error_occurred;

		sys_stats->tscd_bwl_update_error_occurred =
		sys_stats_ptr->tscd_bwl_update_error_occurred;

		sys_stats->tscd_quanta =
		sys_stats_ptr->tscd_quanta;

#if IS_ENABLED(CONFIG_PPV4_LGM)
		sys_stats->wred_fake_pops =
		sys_stats_ptr->wred_fake_pops;

		sys_stats->wred_pop_underflow_count =
		sys_stats_ptr->wred_pop_underflow_count;

		sys_stats->wred_pop_underflow_sum =
		sys_stats_ptr->wred_pop_underflow_sum;

		sys_stats->wred_last_push_address_high =
		sys_stats_ptr->wred_last_push_address_high;

		sys_stats->wred_last_push_address_low =
		sys_stats_ptr->wred_last_push_address_low;

		sys_stats->wred_last_push_drop =
		sys_stats_ptr->wred_last_push_drop;

		sys_stats->wred_last_push_info_q_id =
		sys_stats_ptr->wred_last_push_info_q_id;

		sys_stats->wred_last_push_info_color =
		sys_stats_ptr->wred_last_push_info_color;

		sys_stats->wred_last_push_info_pkt_size =
		sys_stats_ptr->wred_last_push_info_pkt_size;

		sys_stats->wred_last_pop_info_q_id =
		sys_stats_ptr->wred_last_pop_info_q_id;

		sys_stats->wred_last_pop_info_fake =
		sys_stats_ptr->wred_last_pop_info_fake;

		sys_stats->wred_last_pop_info_color =
		sys_stats_ptr->wred_last_pop_info_color;

		sys_stats->wred_last_pop_info_pkt_size =
		sys_stats_ptr->wred_last_pop_info_pkt_size;
#endif
		sys_stats->txmgr_bp_status_ports_0_31 =
		sys_stats_ptr->txmgr_bp_status_ports_0_31;

		sys_stats->txmgr_bp_status_ports_32_63 =
		sys_stats_ptr->txmgr_bp_status_ports_32_63;

		sys_stats->txmgr_bp_status_ports_64_95 =
		sys_stats_ptr->txmgr_bp_status_ports_64_95;

		sys_stats->txmgr_bp_status_ports_96_127 =
		sys_stats_ptr->txmgr_bp_status_ports_96_127;

#if IS_ENABLED(CONFIG_PPV4_LGM)
		sys_stats->txmgr_bp_status_ports_128_159 =
		sys_stats_ptr->txmgr_bp_status_ports_128_159;

		sys_stats->txmgr_bp_status_ports_160_191 =
		sys_stats_ptr->txmgr_bp_status_ports_160_191;

		sys_stats->txmgr_bp_status_ports_192_223 =
		sys_stats_ptr->txmgr_bp_status_ports_192_223;

		sys_stats->txmgr_bp_status_ports_224_255 =
		sys_stats_ptr->txmgr_bp_status_ports_224_255;
#endif
		break;

	case CMD_TYPE_GET_NODE_INFO:
		ret = post_process_get_node_info(qdev, &dcmd->node_info,
						 response);
		break;

	case CMD_TYPE_GET_WSP_HELPER_STATS:
		wsp_stats_resp = (struct wsp_stats_t *)response;
		wsp_stats_ptr =
			(struct wsp_stats_t *)qdev->hwconf.fw_stats_ddr_virt;
		memcpy(wsp_stats_resp, wsp_stats_ptr,
		       sizeof(struct wsp_stats_t));
		break;

	default:
		QOS_ASSERT(0, "Unexpected cmd %d for post process\n", type);
		return -EINVAL;
	}

	return ret;
}

/* Go over all commands on pending queue until cmd id
 * is changed or queue is empty
 * (refer to driver design document to learn more about cmd id).
 * On current implementation it is expected that pending queue contain
 * firmware commands for a single client command, therefore queue should
 * become empty before cmd id is changed.
 *
 * For each command wait until firmware signals
 * completion before continue to next command.
 * Completion status for each command is polled PP_REG_DEFAULT_RETRIES
 * times.
 * If command have not completed after all that polls -
 * function asserts.
 *
 */
s32 check_completion(struct pp_qos_dev *qdev)
{
	u32 expected_flags = UC_CMD_FLAG_UC_DONE | UC_CMD_FLAG_UC_ERROR;
	u32 val, idcur, attempts, popped, int_response, read_len = 0;
	struct fw_internal *internals;
	union driver_cmd dcmd;
	unsigned long pos;
	size_t len;
	s32 ret;

	if (PP_QOS_DEVICE_IS_ASSERT(qdev))
		return -ENODEV;

	popped = 0;
	idcur = 0;
	read_len = cmd_queue_peek(qdev->drvcmds.pendq, &dcmd.cmd,
				  sizeof(struct cmd));
	while (read_len != 0) {
		pos = dcmd.stub.cmd.pos;
		len = dcmd.cmd.len;
		pos += 4;

		ret = pp_raw_reg_poll(pos, expected_flags, 0,
				      PP_REG_MAX_RETRIES, &attempts,
				      &val, PP_POLL_NE);
		if (ret) {
			pr_err("FW is not responding, polling offset %#lx for cmd type %s, (received 0x%x)\n",
			       pos, cmd_str[dcmd.cmd.type], val);
#ifdef DUMP_LOG_ON_FAILURE
			qos_dbg_tree_show(qdev, false, NULL);
			print_fw_log(qdev);
#endif
			return -ENODEV;
		}

		if (val & UC_CMD_FLAG_UC_DONE)
			QOS_LOG_DEBUG("cmd %s (attempts %u)\n",
				      cmd_str[dcmd.cmd.type], attempts);

		if (val & UC_CMD_FLAG_UC_ERROR) {
			QOS_ASSERT(0,
				   "FW signaled error, polling offset %#lx, cmd type %s\n",
				   pos, cmd_str[dcmd.cmd.type]);
#ifdef DUMP_LOG_ON_FAILURE
			qos_dbg_tree_show(qdev, false, NULL);
			print_fw_log(qdev);
#endif
			return -ENODEV;
		}

		ret = cmd_queue_get(qdev->drvcmds.pendq, &dcmd.stub, len);
		QOS_ASSERT(ret == len,
			   "Command queue does not contain a full command\n");
		if (dcmd.cmd.flags & CMD_FLAGS_POST_PROCESS_EXT) {
			pp_cache_invalidate(qdev->hwconf.fw_stats_ddr_virt,
					    qdev->hwconf.fw_stats_sz);
			ret = post_process_external(qdev, &dcmd);
			if (ret)
				return ret;
		}

		if (dcmd.cmd.flags & CMD_FLAGS_POST_PROCESS_INT) {
			int_response =
				PP_REG_RD32(dcmd.stub.cmd.pos +
					    dcmd.cmd.int_response_offset);
			post_process_internal(qdev, &dcmd, int_response);
		}

		++popped;

		read_len = cmd_queue_peek(qdev->drvcmds.pendq,
					  &dcmd.cmd,
					  sizeof(struct cmd));
	}

	internals = qdev->fwbuf;
	QOS_ASSERT(popped == internals->pushed,
		   "Expected to pop %u msgs from pending queue but popped %u\n",
		   internals->pushed, popped);
	QOS_ASSERT(cmd_queue_is_empty(qdev->drvcmds.pendq),
		   "Driver's pending queue is not empty\n");
	internals->pushed = 0;

	memset((void *)qdev->hwconf.bwl_ddr_virt, 0, qdev->hwconf.bwl_sz);
	pp_cache_writeback(qdev->hwconf.bwl_ddr_virt, qdev->hwconf.bwl_sz);

	memset(qdev->hwconf.sbwl_ddr_virt, 0, qdev->hwconf.sbwl_sz);
	pp_cache_writeback(qdev->hwconf.sbwl_ddr_virt, qdev->hwconf.sbwl_sz);
	return 0;
}

static void swap_msg(char *msg)
{
	u32 i;
	u32 *cur;

	cur = (u32 *)msg;

	for (i = 0; i < 32; ++i)
		cur[i] = le32_to_cpu(cur[i]);
}

void print_fw_log(struct pp_qos_dev *qdev)
{
	char msg[PPV4_QOS_LOGGER_MSG_SIZE];
	u32  num_messages;
	u32  max_messages;
	u32  msg_idx;
	u32  msg_read_count;
	u32  *msg_word;
	u32  msg_word_idx;
	unsigned long msg_addr;
	unsigned long addr, cur;

	pp_cache_invalidate(qdev->hwconf.fw_logger_ddr_virt,
			    qdev->hwconf.fw_logger_sz);
	max_messages =
		(qdev->hwconf.fw_logger_sz - PPV4_QOS_LOGGER_HEADROOM_SIZE) /
		PPV4_QOS_LOGGER_MSG_SIZE;
	addr = (unsigned long)(qdev->hwconf.fw_logger_ddr_virt);
	num_messages = *(u32 *)(addr + PPV4_QOS_LOGGER_WRITE_IDX_OFFSET);
	msg_read_count = *(u32 *)(addr + PPV4_QOS_LOGGER_READ_IDX_OFFSET);

	/* check if wraparound occur */
	if ((num_messages - msg_read_count) > max_messages)
		msg_read_count = num_messages - max_messages;

	cur = (unsigned long)(qdev->hwconf.fw_logger_ddr_virt) +
		PPV4_QOS_LOGGER_RING_OFFSET;

	pr_info("FW Total messages Printed : %u\n", num_messages);
	pr_info("Host total messages read  : %u\n", msg_read_count);
	pr_info("Messages to print         : %u\n",
		num_messages - msg_read_count);
	pr_info("Log buffer size           : %u\n", max_messages);
	pr_info("Buffer base               : %#lx\n", cur);
	pr_info("==============================================\n");

	for (; msg_read_count < num_messages; ++msg_read_count) {
		msg_idx = msg_read_count % max_messages;
		for (msg_word_idx = 0; msg_word_idx < 32; ++msg_word_idx) {
			msg_word = (u32 *)(msg) + msg_word_idx;
			msg_addr = cur +
				(PPV4_QOS_LOGGER_MSG_SIZE * msg_idx) +
				(msg_word_idx * 4);
			*msg_word = *(volatile u32 *)(msg_addr);
		}

		swap_msg(msg);
		msg[PPV4_QOS_LOGGER_MSG_SIZE - 1] = '\0';
		pr_info("[ARC]: %s\n", msg);
	}

	/* When done write the correct read count */
	*(u32 *)(addr + PPV4_QOS_LOGGER_READ_IDX_OFFSET) = msg_read_count;
	pp_cache_writeback((void *)addr + PPV4_QOS_LOGGER_READ_IDX_OFFSET, 4);
}

/* Post commands to be pushed at the end of cmdq */
static void pre_enqueue_cmds(struct pp_qos_dev *qdev)
{
	struct fw_internal *intern = qdev->fwbuf;
	u32 i, id;

	if (!intern->num_suspend_ports)
		return;

	/* Enqueue update tree command for all relevant ports */
	for (i = intern->suspend_ports_index; i <
	      intern->num_suspend_ports; ++i) {
		/* In case port was suspended, and it was removed,  */
		/* don't Resume it (Otherwise driver and fw will be */
		/* out of sync) */
		id = get_id_from_phy(qdev->mapping,
				     intern->suspend_ports[i]);
		if (!QOS_ID_VALID(id))
			continue;
		create_update_port_tree_cmd(qdev, intern->suspend_ports[i]);
	}

	/* No pending suspended ports - Reset counters */
	intern->suspend_ports_index = 0;
	intern->num_suspend_ports = 0;
}

/* Take all commands from driver cmd queue, translate them to
 * firmware format and put them on firmware queue.
 * When finish signal firmware.
 */
void enqueue_cmds(struct pp_qos_dev *qdev)
{
	struct fw_internal *intern = qdev->fwbuf;
	struct fw_cmd_multiple_cmds multiple_cmd;
	unsigned long cur, prev, start;
	union driver_cmd dcmd;
	u32 flags, pushed, read_len = 0;
	s32 remain, rc;
	u32 cmdbuf_sz;
	size_t len;

	if (PP_QOS_DEVICE_IS_ASSERT(qdev))
		return;

	cmdbuf_sz = qdev->init_params.fwcom.cmdbuf_sz;
	start = qdev->init_params.fwcom.cmdbuf;

	pushed = 0;
	cur = start;
	prev = start;

	multiple_cmd.base.type = UC_QOS_CMD_MULTIPLE_COMMANDS;
	multiple_cmd.next_cmd_addr =
		FW_CMD_BUFFER_DCCM_START + sizeof(multiple_cmd);
	cur = __fw_cmd_wr(prev, &multiple_cmd, sizeof(multiple_cmd), 0);

	remain = cmdbuf_sz - (cur - start);

	/* Add to cmdq tail: queue restart and port update */
	pre_enqueue_cmds(qdev);

	read_len = cmd_queue_peek(qdev->drvcmds.cmdq,
				  &dcmd.cmd,
				  sizeof(struct cmd));

	while ((remain >= MAX_FW_CMD_SIZE) && (read_len != 0)) {
		len = dcmd.cmd.len;
		rc = cmd_queue_get(qdev->drvcmds.cmdq, &dcmd.stub, len);
		QOS_ASSERT(rc == len,
			   "Command queue does not contain a full command\n");
		prev = cur;
		dcmd.stub.cmd.pos = cur;

		/* Verify command type is valid */
		if (dcmd.cmd.type >= CMD_TYPE_LAST) {
			QOS_LOG_ERR("Unexpected msg type %d\n", dcmd.cmd.type);
			goto cmd_peek;
		}

		/* Write command to the uC command buffer */
		cur = __fw_cmd_wr(prev, &dcmd.stub.data,
				  dcmd.cmd.len - sizeof(dcmd.cmd),
				  0);

		/* Push command to pend queue */
		if (cur != prev) {
			cmd_queue_put(qdev->drvcmds.pendq, &dcmd.stub,
				      dcmd.stub.cmd.len);
			++pushed;
			remain = cmdbuf_sz - (cur - start);
		}

cmd_peek:
		read_len = cmd_queue_peek(qdev->drvcmds.cmdq, &dcmd.cmd,
					  sizeof(struct cmd));
	} /* while (read_len != 0) */

	if (pushed) {
		intern->pushed = pushed;
		prev += 4;
		flags = PP_REG_RD32(prev);
		QOS_BITS_SET(flags, UC_CMD_FLAG_MULTIPLE_COMMAND_LAST);
		PP_REG_WR32(prev, flags);

		/* Read start & end of command buffer to avoid race with FW */
		PP_REG_RD32(start);
		PP_REG_RD32(cur);

		if ((cmdbuf_sz - remain) > qdev->drvcmds.cmdbuf_watermark) {
			pr_debug("New cmdbuf watermark %u (old %u) (remain %d)\n",
				 cmdbuf_sz - remain,
				 qdev->drvcmds.cmdbuf_watermark, remain);
			qdev->drvcmds.cmdbuf_watermark = cmdbuf_sz - remain;
		}

		signal_uc(qdev);
	}

	if ((cur - start) > cmdbuf_sz) {
		pr_err("Batch Command Overflow: %lu (cur %#lx - start %#lx)\n",
		       cur - start,
		       (unsigned long)pp_virt_to_phys((void *)cur),
		       (unsigned long)pp_virt_to_phys((void *)start));
	}

	/* Verify Guard Regions */
	if (pp_reg_poll_x(qdev->init_params.qos_uc_base +
			  PPV4_QOS_CMD_BUF_GUARD_OFFSET,
			  GENMASK(31, 0), PPV4_QOS_UC_GUARD_MAGIC, 1)) {
		QOS_LOG_ERR("Command Buffer Overflow\n");
	}
}
#else
s32 qos_fw_init(struct pp_qos_dev *qdev)
{
	qdev->initialized = true;

	return 0;
}
#endif
