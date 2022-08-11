/* Copyright (c) 2014,2016 The Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/io.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/platform_device.h>
#include <linux/reboot.h>
#include <linux/watchdog.h>
#include <linux/interrupt.h>
#include <linux/sched.h>
#include <linux/qcom_scm.h>
#include <linux/smp.h>
#include <linux/utsname.h>
#include <linux/sizes.h>
#ifdef CONFIG_QCA_MINIDUMP
#include <linux/minidump_tlv.h>
#include <linux/spinlock.h>
#include <linux/pfn.h>
#include <linux/list.h>
#include <linux/string.h>
#endif
#include <linux/slab.h>
#include <linux/mhi.h>

#define TCSR_WONCE_REG 0x193d010

static int in_panic;
bool mhi_wdt_panic_enable;
enum wdt_reg {
	WDT_RST,
	WDT_EN,
	WDT_BARK_TIME,
	WDT_BITE_TIME,
};

static const u32 reg_offset_data_apcs_tmr[] = {
	[WDT_RST] = 0x38,
	[WDT_EN] = 0x40,
	[WDT_BARK_TIME] = 0x4C,
	[WDT_BITE_TIME] = 0x5C,
};

static const u32 reg_offset_data_kpss[] = {
	[WDT_RST] = 0x4,
	[WDT_EN] = 0x8,
	[WDT_BARK_TIME] = 0x10,
	[WDT_BITE_TIME] = 0x14,
};

struct qcom_wdt_props {
	const u32 *layout;
	unsigned int tlv_msg_offset;
	unsigned int crashdump_page_size;
	bool secure_wdog;
};

struct qcom_wdt {
	struct watchdog_device	wdd;
	struct clk		*clk;
	unsigned long		rate;
	unsigned int		bite;
	struct notifier_block	restart_nb;
	void __iomem		*base;
	const struct qcom_wdt_props *dev_props;
	struct resource *tlv_res;
};

#ifdef CONFIG_QCA_MINIDUMP
char mod_log[METADATA_FILE_SZ];
unsigned long mod_log_len;
unsigned long cur_modinfo_offset;
qcom_wdt_scm_tlv_msg_t tlv_msg;
struct minidump_metadata_list metadata_list;
#define REPLACE 1
#define APPEND 0
static int qcom_wdt_scm_replace_tlv(struct qcom_wdt_scm_tlv_msg *scm_tlv_msg,
		unsigned char type, unsigned int size, const char *data, unsigned char *offset);
static int qcom_wdt_scm_add_tlv(struct qcom_wdt_scm_tlv_msg *scm_tlv_msg,
		unsigned char type, unsigned int size, const char *data);
static int traverse_metadata_list(char *name, unsigned long virt_addr, unsigned long phy_addr,
		unsigned char **tlv_offset, unsigned long size);
extern void get_pgd_info(uint64_t *pt_start, uint64_t *pt_len);
extern void get_linux_buf_info(uint64_t *plinux_buf, uint64_t *plinux_buf_len);
extern int get_mmu_info(const void *vmalloc_addr, unsigned long *pt_address,
							unsigned long *pmd_address);
#endif
static void __iomem *wdt_addr(struct qcom_wdt *wdt, enum wdt_reg reg)
{
	return wdt->base + wdt->dev_props->layout[reg];
};

#ifdef CONFIG_QCA_MINIDUMP
/*
* Function: qcom_wdt_scm_replace_tlv
* Description: Adds dump segment as a TLV into the global crashdump
* buffer at specified offset.
*
* @param: [out] scm_tlv_msg - pointer to global crashdump buffer
*		[in] type - Type associated with Dump segment
*		[in] size - Size associted with Dump segment
*		[in] data - Physical address of the Dump segment
*		[in] offset - offset at which TLV entry is added to the crashdump
*		buffer
*
* Return: 0 on success, -ENOBUFS on failure
*/
static int qcom_wdt_scm_replace_tlv(struct qcom_wdt_scm_tlv_msg *scm_tlv_msg,
		unsigned char type, unsigned int size, const char *data, unsigned char *offset)
{
	unsigned char *x;
	unsigned char *y;
	unsigned long flags;

	if (!scm_tlv_msg) {
		return -ENOBUFS;
	}

	spin_lock_irqsave(&scm_tlv_msg->minidump_tlv_spinlock, flags);
	x = offset;
	y = scm_tlv_msg->msg_buffer + scm_tlv_msg->len;

	if ((x + QCOM_WDT_SCM_TLV_TYPE_LEN_SIZE + size) >= y) {
		spin_unlock_irqrestore(&scm_tlv_msg->minidump_tlv_spinlock, flags);
		return -ENOBUFS;
	}

	x[0] = type;
	x[1] = size;
	x[2] = size >> 8;

	memcpy(x + 3, data, size);
	spin_unlock_irqrestore(&scm_tlv_msg->minidump_tlv_spinlock, flags);

	return 0;
}
/*
* Function: qcom_wdt_scm_add_tlv
* Description: Appends dump segment as a TLV entry to the end of the
* global crashdump buffer.
*
* @param: [out] scm_tlv_msg - pointer to global crashdump buffer
*		[in] type - Type associated with Dump segment
*		[in] size - Size associated with Dump segment
*		[in] data - Physical address of the Dump segment
*
* Return: 0 on success, -ENOBUFS on failure
*/
static int qcom_wdt_scm_add_tlv(struct qcom_wdt_scm_tlv_msg *scm_tlv_msg,
			unsigned char type, unsigned int size, const char *data)
{
	unsigned char *x;
	unsigned char *y;
	unsigned long flags;

	if (!scm_tlv_msg) {
		return -ENOBUFS;
	}

	spin_lock_irqsave(&scm_tlv_msg->minidump_tlv_spinlock, flags);
	x = scm_tlv_msg->cur_msg_buffer_pos;
	y = scm_tlv_msg->msg_buffer + scm_tlv_msg->len;

	if ((x + QCOM_WDT_SCM_TLV_TYPE_LEN_SIZE + size) >= y) {
		spin_unlock_irqrestore(&scm_tlv_msg->minidump_tlv_spinlock, flags);
		return -ENOBUFS;
	}

	x[0] = type;
	x[1] = size;
	x[2] = size >> 8;

	memcpy(x + 3, data, size);

	scm_tlv_msg->cur_msg_buffer_pos +=
		(size + QCOM_WDT_SCM_TLV_TYPE_LEN_SIZE);

	spin_unlock_irqrestore(&scm_tlv_msg->minidump_tlv_spinlock, flags);
	return 0;
}
/*
* Function: fill_mmu_info
* Description: Adds PMD and PTE TLV entries into the global crashdump
* buffer at specified offset.
*
* @param: [in] start_address - Physical address of PMD / PTE entry
*		[in] type - Type associated with Dump segment
*		[in] replace - Flag used to determine if TLV entry needs to be
*		inserted at a specified offset or appended to the
*		end of the crashdump buffer
*		[in] mmu_offset - pmd or pte offset at which PMD or PTE TLV
*		entries are added to the crashdump buffer
*
* Return: 0 on success, -ENOBUFS on failure
*/
int fill_mmu_info(unsigned long start_address, unsigned char type,
			unsigned int replace, unsigned char *mmu_offset)
{
	struct minidump_tlv_info minidump_mmu_tlv_info;
	struct qcom_wdt_scm_tlv_msg *scm_tlv_msg = &tlv_msg;
	int ret;

	if (IS_ENABLED(CONFIG_ARM64)) {
		minidump_mmu_tlv_info.size = SZ_4K;
	} else {
		minidump_mmu_tlv_info.size = SZ_2K;
	}

	minidump_mmu_tlv_info.start = start_address;
	if (!minidump_mmu_tlv_info.start)
		return -ENOBUFS;

	if (replace && (*(mmu_offset) == QCA_WDT_LOG_DUMP_TYPE_EMPTY)) {
		ret = qcom_wdt_scm_replace_tlv(&tlv_msg, type,
				sizeof(minidump_mmu_tlv_info),
				(unsigned char *)&minidump_mmu_tlv_info, mmu_offset);
	} else {
		ret = qcom_wdt_scm_add_tlv(&tlv_msg, type,
				sizeof(minidump_mmu_tlv_info),
				(unsigned char *)&minidump_mmu_tlv_info);
	}

	if (ret)
		return ret;

	if (scm_tlv_msg->cur_msg_buffer_pos >=
			scm_tlv_msg->msg_buffer + scm_tlv_msg->len){
		return -ENOBUFS;
	}
	*scm_tlv_msg->cur_msg_buffer_pos =
	QCA_WDT_LOG_DUMP_TYPE_INVALID;
	return 0;
}

/*
* Function: get_mmu_entry
*
* Description: Calculate PMD and PTE entries corresponding to a
* particular dump segment.
*
* @param: [in] vmalloc_addr - Virtual address of Dump segment
*		[in] type - Type associated with Dump segment
*		[in] replace - Flag used to determine if TLV entry needs to be
*		inserted at a specified offset or appended to the
*		end of the crashdump buffer
*		[in] pmd_offset - offset at which corresponding PMD TLV entry
*		will be added to the crashdump buffer
*		[in] pte_offset - offset at which corresponding PTE TLV entry
*		will be added to the crashdump buffer
*
* Return: 0 on success, -ENOBUFS on failure
*/

int get_mmu_entry(const void *vmalloc_addr, unsigned char type,
	unsigned int replace, unsigned char *pmd_offset, unsigned char *pte_offset)
{
	int ret;
	unsigned long pmd_address = 0;
	unsigned long pte_address = 0;

	ret = get_mmu_info((const void *)vmalloc_addr, &pte_address, &pmd_address);
	if (ret)
		return ret;

	ret = fill_mmu_info(pmd_address, type, replace, pmd_offset);
	if (ret)
		return ret;

	ret = fill_mmu_info(pte_address, type, replace, pte_offset);
	if (ret)
		return ret;

	return 0;
}

/*
* Function: remove_minidump_segments
* Description: Traverse metadata list and search for the TLV
* entry corresponding to the input virtual address. If found,
* set va of the Metadata list node to 0 and invalidate the TLV
* entry in the crashdump buffer by setting type to
* QCA_WDT_LOG_DUMP_TYPE_EMPTY
*
* @param: [in] virt_addr - virtual address of the TLV to be invalidated
*
* Return: 0
*/
int remove_minidump_segments(uint64_t virt_addr)
{
	struct minidump_metadata_list *cur_node;
	struct list_head *pos;
	unsigned long flags;
	struct qcom_wdt_scm_tlv_msg *scm_tlv_msg;
	unsigned long pmd_offset = sizeof(struct minidump_tlv_info) +
								QCOM_WDT_SCM_TLV_TYPE_LEN_SIZE;
	unsigned long pte_offset = pmd_offset + sizeof(struct minidump_tlv_info) +
								QCOM_WDT_SCM_TLV_TYPE_LEN_SIZE;

	if (!tlv_msg.msg_buffer) {
		return -ENOMEM;
	}

	scm_tlv_msg = &tlv_msg;

	if (!virt_addr) {
		pr_info("\nMINIDUMP: Attempt to remove an invalid VA.");
		return 0;
	}
	spin_lock_irqsave(&scm_tlv_msg->minidump_tlv_spinlock, flags);
	/* Traverse Metadata list*/
	list_for_each(pos, &metadata_list.list) {
	cur_node = list_entry(pos, struct minidump_metadata_list, list);
		if (cur_node->va == virt_addr) {
			/* If entry with a matching va is found, invalidate
			* this entry by setting va to 0
			*/
			cur_node->va = INVALID;
			/* Invalidate TLV entry in the crashdump buffer by setting type
			* ( value pointed to by cur_node->tlv_offset ) to
			* QCA_WDT_LOG_DUMP_TYPE_EMPTY
			*/
			*(cur_node->tlv_offset) = QCA_WDT_LOG_DUMP_TYPE_EMPTY;
			/* Also invalidate PMD and PTE TLV entries in the crashdump buffer*/
			*(cur_node->tlv_offset + pmd_offset) = QCA_WDT_LOG_DUMP_TYPE_EMPTY;
			*(cur_node->tlv_offset + pte_offset) = QCA_WDT_LOG_DUMP_TYPE_EMPTY;
			/* Retain offsets to the crashdump buffer and metadata file
			* which will be used by the next dump segment to be added
			*/
		}
	}
	spin_unlock_irqrestore(&scm_tlv_msg->minidump_tlv_spinlock, flags);
	return 0;
}
EXPORT_SYMBOL(remove_minidump_segments);

/*
* Function: traverse_metadata_list
*
* Description: Maintain a Metadata list to keep track
* of TLVs in the crashdump buffer and entries in the Meta
* data file.
*
* Each node in the Metadata list stores the name and virtual
* address associated with the dump segments and two offsets,
* tlv_offset and mod_offset, that stores the offset corresponding
* to the TLV in the crashdump buffer and the entry in the Metadata
* file.
*
*                    Metadata file (12 K)
*                   |----------------|
*                   |     Entry 1    |<----------|
*                   |----------------|           |
*                   |    Entry 2     |           |
*                   |----------------|           |
*              |--->|    Entry 3     |           |
*              |    |----------------|           |
*              |    |    Entry 4     |           |
*              |    |----------------|           |
*              |    |    Entry n     |           |
*              |    |----------------|           |
*              |                                 |
*              |                                 |
*              |          Metadata List          |
*     --------------------------------------------------------
*    | Node | Node | Node | Node | Node | Node | Node | Node |
*    |  1   |  2   |  3   |  4   |  5   |  6   |  7   |  n   |
*    ---------------------------------------------------------
*              |                                  |
*              |                                  |
*              |-------------------|              |
*                         ------------------------|
*                         |        |
*                        \/       \/
*   --------------------------------------------------------------
*   |        |         |       |       |        |       |        |
*   | TLV    | TLV     | TLV   | TLV   | TLV    | TLV   | TLV    |
*   |        |         |       |       |        |       |        |
*   --------------------------------------------------------------
*                      Crashdump Buffer (12 K)
*
* When a dump segment needs to be added, the Metadata list is travered
* to check if any invalid entries (entries with va = 0) exist. If an invalid
* enrty exists, name and va of the node is updated with info from new dump segment
* and the dump segment is added as a TLV in the crashdump buffer at tlv_offset. If
* the dumpsegment has a valid name, entry is added to the Metadata file at mod_offset.
*
*
* @param: name - name associated with TLV
*	[in]  virt_addr - virtual address of the Dump segment to be added
*	[in]  phy_addr - physical address of the Dump segment to be added
*	[out] tlv_offset - offset at which corresponding TLV entry will be
*	      added to the crashdump buffer
*
* Return: 'REPLACE' if TLV needs to be inserted into the crashdump buffer at
*	offset position. 'APPEND' if TLV needs to be appended to the crashdump buffer.
*	Also tlv_offset is updated to offset at which corresponding TLV entry will be
*	added to the crashdump buffer. Return -ENOMEM if new list node was not created
*   due to an alloc failure or NULL address. Return -EINVAL if there is an attempt to
*   add a duplicate entry
*/
int traverse_metadata_list(char *name, unsigned long virt_addr, unsigned long phy_addr,
		unsigned char **tlv_offset, unsigned long size)
{

	unsigned long flags;
	struct minidump_metadata_list *cur_node;
	struct minidump_metadata_list *list_node;
	struct list_head *pos;
	struct qcom_wdt_scm_tlv_msg *scm_tlv_msg;
	int invalid_flag = 0;
	cur_node = NULL;

	/* If tlv_msg has not been initialized with non NULL value , return error*/
	if (!tlv_msg.msg_buffer) {
		return -ENOMEM;
	}

	scm_tlv_msg = &tlv_msg;
	spin_lock_irqsave(&scm_tlv_msg->minidump_tlv_spinlock, flags);
	list_for_each(pos, &metadata_list.list) {
		/* Traverse Metadata list to check if dump sgment to be added
		already has a duplicate entry in the crashdump buffer. Also store address
		of first invalid entry , if it exists. Return EINVAL*/
		list_node = list_entry(pos, struct minidump_metadata_list, list);
		if (list_node->va == virt_addr && list_node->size == size) {
			spin_unlock_irqrestore(&scm_tlv_msg->minidump_tlv_spinlock,
					flags);
			pr_debug("Minidump: TLV entry with this VA is already present\n");
			return -EINVAL;
		}

		if (!invalid_flag) {
			if (list_node->va == INVALID) {
				cur_node = list_node;
				invalid_flag = 1;
			}
		}
	}

	if (invalid_flag && cur_node) {
		/* If an invalid entry exits, update node entries and use
		* offset values to write TLVs to the crashdump buffer and
		* an entry in the Metadata file if applicable.
		*/
		*tlv_offset = cur_node->tlv_offset;
		cur_node->va = virt_addr;
		cur_node->size = size;

		if (cur_node->modinfo_offset != 0) {
		/* If the metadata list node has an entry in the Metadata file,
		* invalidate that entry and update metadata file pointer with the
		* value at mod_offset.
		*/
			cur_modinfo_offset = cur_node->modinfo_offset;
			memset((void *)(uintptr_t)cur_modinfo_offset, '\0', METADATA_FILE_ENTRY_LEN);
		} else {
			if (name != NULL) {
				/* If the metadta list node does not have an entry in the
				* Metdata file, update metadata file pointer to point
				* to the end of the metadata file.
				*/
				cur_node->modinfo_offset = cur_modinfo_offset;
				#ifdef CONFIG_QCA_MINIDUMP_DEBUG
				kfree(cur_node->name);
				cur_node->name = kstrndup(name, strlen(name), GFP_KERNEL);
				#endif
			} else {
				/* If dump segment does not have a valid name, set name
				* to null and mod_offset to 0.
				*/
				cur_node->modinfo_offset = 0;
				#ifdef CONFIG_QCA_MINIDUMP_DEBUG
				kfree(cur_node->name);
				cur_node->name = NULL;
				#endif
			}
		}
		spin_unlock_irqrestore(&scm_tlv_msg->minidump_tlv_spinlock,
							 flags);
		/* return REPLACE to indicate TLV needs to be inserted to the crashdump buffer*/
		return REPLACE;
	}

	spin_unlock_irqrestore(&scm_tlv_msg->minidump_tlv_spinlock, flags);
	/*
	* If no invalid entry was found, create new node provided the
	* crashdump buffer or metadata file are not full.
	*/
	if ((scm_tlv_msg->cur_msg_buffer_pos + QCOM_WDT_SCM_TLV_TYPE_LEN_SIZE +
			sizeof(struct minidump_tlv_info) >=
			scm_tlv_msg->msg_buffer + scm_tlv_msg->len) ||
			(mod_log_len + METADATA_FILE_ENTRY_LEN >= METADATA_FILE_SZ)) {
		return -ENOMEM;
	}
	cur_node = (struct minidump_metadata_list *)
					kmalloc(sizeof(struct minidump_metadata_list), GFP_KERNEL);

	if (!cur_node) {
		return -ENOMEM;
	}

	if (name != NULL) {
		/* If dump segment has a valid name, update name and offset with
		* pointer to the Metadata file
		*/
		cur_node->modinfo_offset = cur_modinfo_offset;
		#ifdef CONFIG_QCA_MINIDUMP_DEBUG
		cur_node->name = kstrndup(name, strlen(name), GFP_KERNEL);
		#endif
	} else {
		/* If dump segment does not have a valid name, set name to null and
		* mod_offset to 0
		*/
		cur_node->modinfo_offset = 0;
		#ifdef CONFIG_QCA_MINIDUMP_DEBUG
		cur_node->name = NULL;
		#endif
	}
	/* Update va and offset to crashdump buffer*/
	cur_node->va = virt_addr;
	cur_node->size = size;
	cur_node->tlv_offset = scm_tlv_msg->cur_msg_buffer_pos;

	spin_lock_irqsave(&scm_tlv_msg->minidump_tlv_spinlock, flags);
	list_add_tail(&(cur_node->list), &(metadata_list.list));
	spin_unlock_irqrestore(&scm_tlv_msg->minidump_tlv_spinlock, flags);
	/* return APPEND to indicate TLV needs to be appended to the crashdump buffer*/
	return APPEND;
}

/*
* Function: fill_tlv_crashdump_buffer
*
* Description: Add TLV entries into the global crashdump
* buffer at specified offset.
*
* @param: [in] start_address - Physical address of Dump segment
*		[in] type - Type associated with the	Dump segment
*		[in] size - Size associated with the Dump segment
*		[in] replace - Flag used to determine if TLV entry needs to be
*		inserted at a specified offset or appended to the end of
*		the crashdump buffer
*		[in] tlv_offset - offset at which TLV entry is added to the
*		crashdump buffer
*
* Return: 0 on success, -ENOBUFS on failure
*/
int fill_tlv_crashdump_buffer(uint64_t start_addr, uint64_t size,
		unsigned char type, unsigned int replace, unsigned char *tlv_offset)
{
	struct minidump_tlv_info minidump_tlv_info;
	struct qcom_wdt_scm_tlv_msg *scm_tlv_msg = &tlv_msg;

	int ret;

	minidump_tlv_info.start = start_addr;
	minidump_tlv_info.size = size;

	if (replace && (*(tlv_offset) == QCA_WDT_LOG_DUMP_TYPE_EMPTY)) {
		ret = qcom_wdt_scm_replace_tlv(&tlv_msg, type,
				sizeof(minidump_tlv_info),
				(unsigned char *)&minidump_tlv_info, tlv_offset);
	} else {
		ret = qcom_wdt_scm_add_tlv(&tlv_msg, type,
			sizeof(minidump_tlv_info),
			(unsigned char *)&minidump_tlv_info);
	}

	if (ret) {
		pr_err("Minidump: Crashdump buffer is full %d\n", ret);
		return ret;
	}

	if (scm_tlv_msg->cur_msg_buffer_pos >=
		scm_tlv_msg->msg_buffer + scm_tlv_msg->len){
		pr_err("MINIDUMP buffer overflow %d\n", (int)type);
		return -ENOBUFS;
	}
	*scm_tlv_msg->cur_msg_buffer_pos =
		QCA_WDT_LOG_DUMP_TYPE_INVALID;

	return 0;
}
/*
* Function: fill_minidump_segment
*
* Description: Add a dump segment as a TLV entry in the Metadata list
* and global crashdump buffer. Call a function to retrieve MMU info
* corresponding to the dump segment and add TLVs to the crashdump buffer.
* Also writes module information to Metadata text file, which is
* useful for post processing of collected dumps.
*
* @param: [in] start_address - Virtual address of Dump segment
*		[in] type - Type associated with the Dump segment
*		[in] size - Size associated with the Dump segment
*		[in] name - name associated with the Dump segment. Can be set to NULL.
*
* Return: 0 on success, -ENOMEM on failure
*/
int fill_minidump_segments(uint64_t start_addr, uint64_t size, unsigned char type, char *name)
{

	int ret = 0;
	unsigned int replace = 0;
	struct page *minidump_tlv_page;
	uint64_t phys_addr;
	uint64_t temp_start_addr = start_addr;
	unsigned char *tlv_offset = NULL;
	unsigned long pmd_offset = sizeof(struct minidump_tlv_info) +
							QCOM_WDT_SCM_TLV_TYPE_LEN_SIZE;
	unsigned long pte_offset = pmd_offset + sizeof(struct minidump_tlv_info) +
							QCOM_WDT_SCM_TLV_TYPE_LEN_SIZE;

	if ((unsigned long)start_addr >= PAGE_OFFSET && (unsigned long) start_addr
						< (unsigned long)high_memory) {
		phys_addr = (uint64_t)__pa(start_addr);
		replace = traverse_metadata_list(name, start_addr, phys_addr, &tlv_offset, size);
		/* return value of -ENOMEM indicates  new list node was not created
		* due to an alloc failure. return value of -EINVAL indicates an attempt to
		* add a duplicate entry
		*/
		if (replace == -EINVAL)
			return 0;

		if (replace == -ENOMEM)
			return replace;

		ret = fill_tlv_crashdump_buffer(phys_addr, size, type, replace, tlv_offset);
		if (ret) {
			return ret;
		}

		if (type == QCA_WDT_LOG_DUMP_TYPE_WLAN_MOD || IS_ENABLED(CONFIG_ARM64)) {
			ret = get_mmu_entry((const void *)(uintptr_t)(temp_start_addr
				& (~(PAGE_SIZE - 1))), QCA_WDT_LOG_DUMP_TYPE_WLAN_MOD, replace,
				tlv_offset + pmd_offset, tlv_offset + pte_offset);
			if (ret) {
				pr_err("MINIDUMP error dumping MMU %d \n", ret);
				return ret;
			}
		}
	} else {
		minidump_tlv_page =	vmalloc_to_page((const void *)(uintptr_t)
							(start_addr & (~(PAGE_SIZE - 1))));
		phys_addr = page_to_phys(minidump_tlv_page) + offset_in_page(start_addr);
		replace = traverse_metadata_list(name, start_addr, phys_addr, &tlv_offset, size);

		if (replace == -EINVAL)
			return 0;

		if (replace == -ENOMEM)
			return replace;

		fill_tlv_crashdump_buffer(phys_addr, size, type, replace, tlv_offset);
		if (ret) {
			return ret;
		}
		ret = get_mmu_entry((const void *)(uintptr_t)(start_addr &
				(~(PAGE_SIZE - 1))), QCA_WDT_LOG_DUMP_TYPE_WLAN_MOD,
				replace, tlv_offset + pmd_offset, tlv_offset + pte_offset);
		if (ret) {
			pr_info("MINIDUMP error dumping MMU %d\n", ret);
			return ret;
		}
	}

	store_module_info(name, start_addr, type);
	return 0;
}
EXPORT_SYMBOL(fill_minidump_segments);
/*
* Function: store_module_info
* Description: Add module name and virtual address information
* to a metadata file 'MODULE_INFO.txt' at the specified offset.
* Useful for post processing with the collected dumps.
*
* @param: [in] address - Virtual address of Dump segment
*		[in] type - Type associated with the Dump segment
*		[in] name - name associated with the Dump segment.
*		If set to NULL,enrty is not written to the file
*
* Return: 0 on success, -ENOBUFS on failure
*/
int store_module_info(char *name ,unsigned long address, unsigned char type)
{

	char substring[METADATA_FILE_ENTRY_LEN];
	char *mod_name;
	int ret_val =0;
	struct qcom_wdt_scm_tlv_msg *scm_tlv_msg;
	unsigned long flags;

	if (!tlv_msg.msg_buffer) {
		return -ENOBUFS;
	}

	scm_tlv_msg = &tlv_msg;

	/* Check for Metadata file overflow */
	if ((cur_modinfo_offset == (uintptr_t)mod_log + mod_log_len) &&
		(mod_log_len + METADATA_FILE_ENTRY_LEN >= METADATA_FILE_SZ)) {
		pr_err("\nMINIDUMP Metadata file overflow error");
		return 0;
	}

	/*
	* Check for valid cur_modinfo_offset value. Ensure
	* that the offset is not NULL and is within bounds
	* of the Metadata file.
	*/
	if ((!(void *)(uintptr_t)cur_modinfo_offset) ||
		(cur_modinfo_offset < (uintptr_t)mod_log) ||
		(cur_modinfo_offset + METADATA_FILE_ENTRY_LEN >=
		((uintptr_t)mod_log + METADATA_FILE_SZ))) {
		pr_err("\nMINIDUMP Metadata file offset error");
		return  -ENOBUFS;
	}

	/* Check for valid name */
	if (!name)
		return 0;

	mod_name = kstrndup(name, strlen(name), GFP_KERNEL);
	if (!mod_name)
		return 0;

	/* Truncate name if name is greater than 28 char */
	if (strlen(mod_name) > NAME_LEN) {
		mod_name[NAME_LEN] = '\0';
	}

	if (type == QCA_WDT_LOG_DUMP_TYPE_LEVEL1_PT) {
		ret_val = snprintf(substring, METADATA_FILE_ENTRY_LEN,
		"\n%s pa=%lx", mod_name, (unsigned long)__pa(address));
	} else {
		ret_val = snprintf(substring, METADATA_FILE_ENTRY_LEN,
		"\n%s va=%lx", mod_name, address);
	}

	/* Check for Metadatafile overflow */
	if (mod_log_len + METADATA_FILE_ENTRY_LEN >=  METADATA_FILE_SZ) {
		kfree(mod_name);
		return -ENOBUFS;
	}

	spin_lock_irqsave(&scm_tlv_msg->minidump_tlv_spinlock, flags);
	memset((void *)(uintptr_t)cur_modinfo_offset, '\0', METADATA_FILE_ENTRY_LEN);
	snprintf((char *)(uintptr_t)cur_modinfo_offset, METADATA_FILE_ENTRY_LEN, "%s", substring);

	if (cur_modinfo_offset == (uintptr_t)mod_log + mod_log_len) {
		mod_log_len = mod_log_len + METADATA_FILE_ENTRY_LEN;
		cur_modinfo_offset = (uintptr_t)mod_log + mod_log_len;
	} else {
		cur_modinfo_offset = (uintptr_t)mod_log + mod_log_len;
	}
	spin_unlock_irqrestore(&scm_tlv_msg->minidump_tlv_spinlock, flags);
	kfree(mod_name);
	return 0;
}
/*
* Function: qcom_wdt_scm_fill_log_dump_tlv
* Description: Add 'static' dump segments - uname, demsg,
* page global directory, linux buffer and metadata text
* file to the global crashdump buffer
*
* @param: [in] scm_tlv_msg - pointer to crashdump buffer
*
* Return: 0 on success, -ENOBUFS on failure
*/
static int qcom_wdt_scm_fill_log_dump_tlv(
			struct qcom_wdt_scm_tlv_msg *scm_tlv_msg)
{
	struct new_utsname *uname;
	int ret_val;
	struct minidump_tlv_info pagetable_tlv_info;
	struct minidump_tlv_info log_buf_info;
	struct minidump_tlv_info linux_banner_info;
	mod_log_len = 0;
	cur_modinfo_offset = (uintptr_t)mod_log;
	uname = utsname();

	INIT_LIST_HEAD(&metadata_list.list);

	ret_val = qcom_wdt_scm_add_tlv(scm_tlv_msg,
			    QCA_WDT_LOG_DUMP_TYPE_UNAME,
			    sizeof(*uname),
			    (unsigned char *)uname);
	if (ret_val)
		return ret_val;

	get_log_buf_info(&log_buf_info.start, &log_buf_info.size);
	ret_val = fill_minidump_segments(log_buf_info.start, log_buf_info.size,
						QCA_WDT_LOG_DUMP_TYPE_DMESG, NULL);
	if (ret_val) {
		pr_err("Minidump: Crashdump buffer is full %d \n", ret_val);
		return ret_val;
	}

	get_pgd_info(&pagetable_tlv_info.start, &pagetable_tlv_info.size);
	ret_val = fill_minidump_segments(pagetable_tlv_info.start,
				pagetable_tlv_info.size, QCA_WDT_LOG_DUMP_TYPE_LEVEL1_PT, "PGD");
	if (ret_val) {
		pr_err("Minidump: Crashdump buffer is full %d \n", ret_val);
		return ret_val;
	}

	get_linux_buf_info(&linux_banner_info.start, &linux_banner_info.size);
	ret_val = fill_minidump_segments(linux_banner_info.start, linux_banner_info.size,
				QCA_WDT_LOG_DUMP_TYPE_WLAN_MOD, NULL);
	if (ret_val) {
		pr_err("Minidump: Crashdump buffer is full %d \n", ret_val);
		return ret_val;
	}

	ret_val = fill_minidump_segments((uint64_t)(uintptr_t)mod_log, (uint64_t)__pa(&mod_log_len),
					QCA_WDT_LOG_DUMP_TYPE_WLAN_MOD_INFO, NULL);
	if (ret_val) {
		pr_err("Minidump: Crashdump buffer is full %d \n", ret_val);
		return ret_val;
	}

	if (scm_tlv_msg->cur_msg_buffer_pos >=
		scm_tlv_msg->msg_buffer + scm_tlv_msg->len)
	return -ENOBUFS;

	return 0;
}
/*
* Function: wlan_module_notifier
* Description: Notfier Call back for WLAN MODULE LIST when modules are loaded.
* Add modules structure , section attributes and bss sections to the
* Metadata list for specified modules.
*
* @param: [in] nb - notifier block object
*		[in] event - current state of module
*		[in] data - pointer to module structure
*
* Return: 0 on success, -ENOBUFS on failure
*/
static int wlan_module_notifier(struct notifier_block *nb, unsigned long event, void *data)
{
	struct module *mod = data;
	struct minidump_tlv_info module_tlv_info;
	int ret_val;
	unsigned int i;

	if ((event == MODULE_STATE_LIVE) && ((!strcmp("ecm", mod->name)) ||
					(!strcmp("smart_antenna", mod->name)) ||
					(!strcmp("ath_pktlog", mod->name))    ||
					(!strcmp("wifi_2_0", mod->name))      ||
					(!strcmp("wifi_3_0", mod->name))      ||
					(!strcmp("qca_ol", mod->name))        ||
					(!strcmp("qca_spectral", mod->name))  ||
					(!strcmp("umac", mod->name))          ||
					(!strcmp("asf", mod->name))           ||
					(!strcmp("qdf", mod->name)))) {

		/* For all modules dump module meta data*/
		module_tlv_info.start = (uintptr_t)mod;
		module_tlv_info.size = sizeof(struct module);
		ret_val = fill_minidump_segments(module_tlv_info.start,
				module_tlv_info.size, QCA_WDT_LOG_DUMP_TYPE_WLAN_MOD, NULL);
		if (ret_val) {
			pr_err("Minidump: Crashdump buffer is full %d\n", ret_val);
			return ret_val;
		}

		/* For ecm, also dump previous*/

		if ((!strcmp("ecm", mod->name))) {
			module_tlv_info.start = (unsigned long)mod->list.prev;
			module_tlv_info.size = sizeof(struct module);
			ret_val = fill_minidump_segments(module_tlv_info.start,
				module_tlv_info.size, QCA_WDT_LOG_DUMP_TYPE_WLAN_MOD, NULL);
			if (ret_val) {
				pr_err("Minidump: Crashdump buffer is full %d\n", ret_val);
				return ret_val;
			}
		}

	/* For umac , qca_ol, wifi_3_0 modules, additionally dump module sections and bss */

		if ((!strcmp("qca_ol", mod->name)) || (!strcmp("umac", mod->name)) ||
					(!strcmp("wifi_3_0", mod->name)) ||
					(!strcmp("qdf", mod->name))) {
			module_tlv_info.start = (unsigned long)mod->sect_attrs;
			module_tlv_info.size = SZ_2K;
			ret_val = fill_minidump_segments(module_tlv_info.start,
					module_tlv_info.size, QCA_WDT_LOG_DUMP_TYPE_WLAN_MOD, NULL);
			if (ret_val) {
				pr_err("Minidump: Crashdump buffer is full %d\n", ret_val);
				return ret_val;
			}

			for (i = 0; i <= mod->sect_attrs->nsections; i++) {
				if ((!strcmp(".bss", mod->sect_attrs->attrs[i].name))) {
					module_tlv_info.start = (unsigned long)
					mod->sect_attrs->attrs[i].address;
					module_tlv_info.size = (unsigned long)mod->module_core
						+ (unsigned long) mod->core_size -
						(unsigned long)mod->sect_attrs->attrs[i].address;
					pr_err("MINIDUMP VA .bss start=%lx module=%s",
						(unsigned long)mod->sect_attrs->attrs[i].address,
						mod->name);

					/* Log .bss VA of module in buffer */
					ret_val = fill_minidump_segments(module_tlv_info.start,
					module_tlv_info.size, QCA_WDT_LOG_DUMP_TYPE_WLAN_MOD,
					mod->name);
					if (ret_val) {
						pr_err("Minidump: Crashdump buffer is full %d", ret_val);
						return ret_val;
					}
				}
			}

		}
	}

	return NOTIFY_OK;
};
/*
* Function: wlan_module_notify_ecit
*
* Description: Notfier Call back for WLAN MODULE LIST when modules are unloaded.
* Remove / invalidate modules structure , section attributes and bss
* sections from the Metadata list for specified modules.
*
* @param: [in] self - notifier block object
*		[in] event - current state of module
*		[in] data - pointer to module structure
*
* Return: NOTIFY_OK on success, -ENOBUFS on failure
*/
static int wlan_module_notify_exit(struct notifier_block *self,
									unsigned long event, void *data)
{
	int ret;
	int i = 0;
	struct module *mod = data;

	if ((event == MODULE_STATE_GOING) && ((!strcmp("ecm", mod->name)) ||
					(!strcmp("smart_antenna", mod->name)) ||
					(!strcmp("ath_pktlog", mod->name))    ||
					(!strcmp("wifi_2_0", mod->name))      ||
					(!strcmp("wifi_3_0", mod->name))      ||
					(!strcmp("qca_ol", mod->name))        ||
					(!strcmp("qca_spectral", mod->name))  ||
					(!strcmp("umac", mod->name))          ||
					(!strcmp("asf", mod->name))           ||
					(!strcmp("qdf", mod->name)))) {

		ret = remove_minidump_segments((uint64_t)(uintptr_t)mod);
		if ((!strcmp("qca_ol", mod->name)) || (!strcmp("umac", mod->name)) ||
					(!strcmp("wifi_3_0", mod->name)) ||
					(!strcmp("qdf", mod->name))) {
			ret = remove_minidump_segments((uint64_t)
						(uintptr_t)mod->sect_attrs);
			ret = remove_minidump_segments((uint64_t)
						(uintptr_t)mod);
			for (i = 0; i <= mod->sect_attrs->nsections; i++) {
				if ((!strcmp(".bss", mod->sect_attrs->attrs[i].name))) {
					ret = remove_minidump_segments((uint64_t)
					(uintptr_t)mod->sect_attrs->attrs[i].address);
				}
			}
		}
	}

	return NOTIFY_OK;
};

static struct notifier_block wlan_nb = {
	.notifier_call  = wlan_module_notifier,
};

static struct notifier_block wlan_module_exit_nb = {
	.notifier_call  = wlan_module_notify_exit,
};

#ifdef CONFIG_QCA_MINIDUMP_DEBUG
static int wlan_modinfo_panic_handler(struct notifier_block *this,
				unsigned long event, void *ptr)
{
	struct minidump_metadata_list *cur_node;
	struct list_head *pos;
	int count = 0;

	if (!tlv_msg.msg_buffer) {
		pr_err("\n Minidump: Crashdump buffer is empty");
		return NOTIFY_OK;
	}

	pr_err("\n Minidump: Size of Metadata file = %ld\n",mod_log_len);
	pr_err("\n Minidump: Printing out contents of Metadata list\n");

	list_for_each(pos, &metadata_list.list) {
		count ++;
		cur_node = list_entry(pos, struct minidump_metadata_list, list);
		if (cur_node->name != NULL)
			pr_info(" %s [%lx] ---> ",cur_node->name,cur_node->va);
		else
			pr_info(" un-named [%lx] ---> ",cur_node->va);
	}
	pr_err("\n Minidump: # nodes in the Metadata list = %d\n",count);
	pr_err("\n Minidump: Size of node in Metadata list = %ld\n",
		(unsigned long)sizeof(struct minidump_metadata_list));
	return NOTIFY_DONE;
}

static struct notifier_block wlan_panic_nb = {
	.notifier_call  = wlan_modinfo_panic_handler,
};
#endif /* CONFIG_QCA_MINIDUMP_DEBUG */
#endif /*CONFIG_QCA_MINIDUMP  */

static inline
struct qcom_wdt *to_qcom_wdt(struct watchdog_device *wdd)
{
	return container_of(wdd, struct qcom_wdt, wdd);
}

static int panic_prep_restart(struct notifier_block *this,
				unsigned long event, void *ptr)
{
	in_panic = 1;
	return NOTIFY_DONE;
}

static struct notifier_block panic_blk = {
	.notifier_call  = panic_prep_restart,
};

static long qcom_wdt_configure_bark_dump(void *arg)
{
	void *scm_regsave;
	void *tlv_ptr;
	resource_size_t tlv_base;
	resource_size_t tlv_size;
	struct qcom_wdt *wdt = (struct qcom_wdt *) arg;
	const struct qcom_wdt_props *device_props = wdt->dev_props;
	long ret = -ENOMEM;
	struct resource *res = wdt->tlv_res;

	scm_regsave = (void *) __get_free_pages(GFP_KERNEL,
				get_order(device_props->crashdump_page_size));
	if (!scm_regsave)
		return -ENOMEM;

	ret = qcom_scm_regsave(SCM_SVC_UTIL, SCM_CMD_SET_REGSAVE,
			scm_regsave, device_props->crashdump_page_size);

	if (ret) {
		pr_err("Setting register save address failed.\n"
			"Registers won't be dumped on a dog bite\n");
		return ret;
	}

	/* Initialize the tlv and fill all the details */
#ifdef CONFIG_QCA_MINIDUMP
	spin_lock_init(&tlv_msg.minidump_tlv_spinlock);
	tlv_msg.msg_buffer = scm_regsave + device_props->tlv_msg_offset;
	tlv_msg.cur_msg_buffer_pos = tlv_msg.msg_buffer;
	tlv_msg.len = device_props->crashdump_page_size -
				 device_props->tlv_msg_offset;
	ret = qcom_wdt_scm_fill_log_dump_tlv(&tlv_msg);

	/* if failed, we still return 0 because it should not
	 * affect the boot flow. The return value 0 does not
	 * necessarily indicate success in this function.
	 */
	if (ret) {
		pr_err("log dump initialization failed\n");
		return 0;
	}
#endif

	if (res) {
		tlv_base = res->start;
		tlv_size = resource_size(res);
		res = request_mem_region(tlv_base, tlv_size, "tlv_dump");

		if (!res) {
			pr_err("requesting memory region failed\n");
			return 0;
		}

		tlv_ptr = ioremap(tlv_base, tlv_size);

		if (!tlv_ptr) {
			pr_err("mapping physical mem failed\n");
			release_mem_region(tlv_base, tlv_size);
			return 0;
		}

#ifdef CONFIG_QCA_MINIDUMP
		memcpy_toio(tlv_ptr, tlv_msg.msg_buffer, tlv_msg.len);
#endif
		iounmap(tlv_ptr);
		release_mem_region(tlv_base, tlv_size);
	}

	return 0;
}

static int qcom_fiq_extwdt(unsigned int regaddr, unsigned int value)
{
	int ret;

	ret = qcom_scm_extwdt(SCM_SVC_EXTWDT, SCM_CMD_EXTWDT, regaddr, value);
	if (ret)
		pr_err("Setting value to TCSR_WONCE register failed\n");

	return ret;
}

static int qcom_wdt_start_secure(struct watchdog_device *wdd)
{
	struct qcom_wdt *wdt = to_qcom_wdt(wdd);

	writel(0, wdt_addr(wdt, WDT_EN));
	writel(1, wdt_addr(wdt, WDT_RST));

	if (wdt->bite) {
		writel((wdd->timeout - 1) * wdt->rate,
			wdt_addr(wdt, WDT_BARK_TIME));
		writel(wdd->timeout * wdt->rate, wdt_addr(wdt, WDT_BITE_TIME));
	} else {
		writel(wdd->timeout * wdt->rate, wdt_addr(wdt, WDT_BARK_TIME));
		writel((wdd->timeout * wdt->rate) * 2, wdt_addr(wdt, WDT_BITE_TIME));
	}

	writel(1, wdt_addr(wdt, WDT_EN));
	return 0;
}

static int qcom_wdt_start_nonsecure(struct watchdog_device *wdd)
{
	struct qcom_wdt *wdt = to_qcom_wdt(wdd);

	writel(0, wdt_addr(wdt, WDT_EN));
	writel(1, wdt_addr(wdt, WDT_RST));
	writel(wdd->timeout * wdt->rate, wdt_addr(wdt, WDT_BARK_TIME));
	writel(0x0FFFFFFF, wdt_addr(wdt, WDT_BITE_TIME));
	writel(1, wdt_addr(wdt, WDT_EN));
	return 0;
}

static int qcom_wdt_stop(struct watchdog_device *wdd)
{
	struct qcom_wdt *wdt = to_qcom_wdt(wdd);

	writel(0, wdt_addr(wdt, WDT_EN));
	return 0;
}

static int qcom_wdt_ping(struct watchdog_device *wdd)
{
	struct qcom_wdt *wdt = to_qcom_wdt(wdd);

	writel(1, wdt_addr(wdt, WDT_RST));
	return 0;
}

static int qcom_wdt_set_timeout(struct watchdog_device *wdd,
				unsigned int timeout)
{
	wdd->timeout = timeout;
	return wdd->ops->start(wdd);
}

static const struct watchdog_ops qcom_wdt_ops_secure = {
	.start		= qcom_wdt_start_secure,
	.stop		= qcom_wdt_stop,
	.ping		= qcom_wdt_ping,
	.set_timeout	= qcom_wdt_set_timeout,
	.owner		= THIS_MODULE,
};

static const struct watchdog_ops qcom_wdt_ops_nonsecure = {
	.start		= qcom_wdt_start_nonsecure,
	.stop		= qcom_wdt_stop,
	.ping		= qcom_wdt_ping,
	.set_timeout	= qcom_wdt_set_timeout,
	.owner		= THIS_MODULE,
};

static const struct watchdog_info qcom_wdt_info = {
	.options	= WDIOF_KEEPALIVEPING
			| WDIOF_MAGICCLOSE
			| WDIOF_SETTIMEOUT,
	.identity	= KBUILD_MODNAME,
};

const struct qcom_wdt_props qcom_wdt_props_ipq8064 = {
	.layout = reg_offset_data_apcs_tmr,
	.tlv_msg_offset = SZ_2K,
	.crashdump_page_size = SZ_4K,
	.secure_wdog = false,
};

const struct qcom_wdt_props qcom_wdt_props_ipq807x = {
	.layout = reg_offset_data_kpss,
	.tlv_msg_offset = (500 * SZ_1K),
	/* As SBL overwrites the NSS IMEM, TZ has to copy it to some memory
	 * on crash before it restarts the system. Hence, reserving of 384K
	 * is required to copy the NSS IMEM before restart is done.
	 * So that TZ can dump NSS dump data after the first 8K.
	 * Additionally 8K memory is allocated which can be used by TZ
	 * to dump PMIC memory.
	 * get_order function returns the next higher order as output,
	 * so when we pass 400K as argument 512K will be allocated.
	 * 3K is required for DCC regsave memory.
	 * 15K is required for CPR.
	* 82K is unused currently and can be used based on future needs.
	* 12K is used for crashdump TLV buffer for Minidump feature.
	 */
	/*
	 * The memory is allocated using alloc_pages, hence it will be in
	 * power of 2. The unused memory is the result of using alloc_pages.
	 * As we need contigous memory for > 256K we have to use alloc_pages.
	 *
	 *		 ---------------
	 *		|      8K	|
	 *		|    regsave	|
	 *		 ---------------
	 *		|		|
	 *		|     384K	|
	 *		|    NSS IMEM	|
	 *		|		|
	 *		|		|
	 *		 ---------------
	 *		|      8K	|
	 *		|    PMIC mem	|
	 *		 ---------------
	 *		|    3K - DCC	|
	 *		 ---------------
	 *		 --------------
	 *		|      15K     |
	 *		|    CPR Reg   |
	 *		 --------------
	 *		|		|
	 *		|     82K	|
	 *		|    Unused	|
	 *		|		|
	 *		 ---------------
	*		|     12 K     |
	*		|   TLV Buffer |
	*		 ---------------
	*
	*/
	.crashdump_page_size = (SZ_8K + (384 * SZ_1K) + (SZ_8K) + (3 * SZ_1K) +
				(15 * SZ_1K) + (82 * SZ_1K) + (12 * SZ_1K)),
	.secure_wdog = true,
};

const struct qcom_wdt_props qcom_wdt_props_ipq40xx = {
	.layout = reg_offset_data_kpss,
	.tlv_msg_offset = SZ_2K,
	.crashdump_page_size = SZ_4K,
	.secure_wdog = true,
};

const struct qcom_wdt_props qcom_wdt_props_ipq6018 = {
	.layout = reg_offset_data_kpss,
	.tlv_msg_offset = (244 * SZ_1K),
	/* As XBL overwrites the NSS UTCM, TZ has to copy it to some memory
	 * on crash before it restarts the system. Hence, reserving of 192K
	 * is required to copy the NSS UTCM before restart is done.
	 * So that TZ can dump NSS dump data after the first 8K.
	 *
	 * 3K for DCC Memory
	 *
	 * get_order function returns the next higher order as output,
	 * so when we pass 203K as argument 256K will be allocated.
	 * 41K is unused currently and can be used based on future needs.
	 * 12K is used for crashdump TLV buffer for Minidump feature.
	 * For minidump feature, last 16K of crashdump page size is used for
	 * TLV buffer in the case of ipq807x. Same offset (last 16 K from end
	 * of crashdump page) is used for ipq60xx as well, to keep design
	 * consistent.
	 */
	/*
	 * The memory is allocated using alloc_pages, hence it will be in
	 * power of 2. The unused memory is the result of using alloc_pages.
	 * As we need contigous memory for > 256K we have to use alloc_pages.
	 *
	 *		 ---------------
	 *		|      8K	|
	 *		|    regsave	|
	 *		 ---------------
	 *		|		|
	 *		|     192K	|
	 *		|    NSS UTCM	|
	 *		|		|
	 *		|		|
	 *		 ---------------
	 *		|    3K - DCC	|
	 *		 ---------------
	 *		|		|
	 *		|     41K	|
	 *		|    Unused	|
	 *		|		|
	 *		 ---------------
	 *		|     12 K     |
	 *		|   TLV Buffer |
	 *		---------------
	 *
*/
	.crashdump_page_size = (SZ_8K + (192 * SZ_1K) + (3 * SZ_1K) +
				(41 * SZ_1K) + (12 * SZ_1K)),
	.secure_wdog = true,
};

static const struct of_device_id qcom_wdt_of_table[] = {
	{	.compatible = "qcom,kpss-wdt-ipq8064",
		.data = (void *) &qcom_wdt_props_ipq8064,
	},
	{	.compatible = "qcom,kpss-wdt-ipq807x",
		.data = (void *) &qcom_wdt_props_ipq807x,
	},
	{	.compatible = "qcom,kpss-wdt-ipq40xx",
		.data = (void *) &qcom_wdt_props_ipq40xx,
	},
	{	.compatible = "qcom,kpss-wdt-ipq6018",
		.data = (void *) &qcom_wdt_props_ipq6018,
	},
	{}
};
MODULE_DEVICE_TABLE(of, qcom_wdt_of_table);
static int qcom_wdt_restart(struct notifier_block *nb, unsigned long action,
			    void *data)
{
	struct qcom_wdt *wdt = container_of(nb, struct qcom_wdt, restart_nb);
	u32 timeout;

	/*
	 * Trigger watchdog bite:
	 *    Setup BITE_TIME to be 128ms, and enable WDT.
	 */
	timeout = 128 * wdt->rate / 1000;

	writel(0, wdt_addr(wdt, WDT_EN));
	writel(1, wdt_addr(wdt, WDT_RST));
	if (in_panic) {
		writel(timeout, wdt_addr(wdt, WDT_BARK_TIME));
		writel(2 * timeout, wdt_addr(wdt, WDT_BITE_TIME));
	} else {
		writel(5 * timeout, wdt_addr(wdt, WDT_BARK_TIME));
		writel(timeout, wdt_addr(wdt, WDT_BITE_TIME));
	}

	writel(1, wdt_addr(wdt, WDT_EN));
	/*
	 * Actually make sure the above sequence hits hardware before sleeping.
	 */
	wmb();

	mdelay(150);
	return NOTIFY_DONE;
}

static irqreturn_t wdt_bark_isr(int irq, void *wdd)
{
	struct qcom_wdt *wdt = to_qcom_wdt(wdd);
	unsigned long nanosec_rem;
	unsigned long long t = sched_clock();

	nanosec_rem = do_div(t, 1000000000);
	pr_info("Watchdog bark! Now = %lu.%06lu\n", (unsigned long) t,
							nanosec_rem / 1000);
#ifdef CONFIG_MHI_BUS
	if (mhi_wdt_panic_enable)
		mhi_wdt_panic_handler();
#endif

	pr_info("Causing a watchdog bite!");
	writel(0, wdt_addr(wdt, WDT_EN));
	writel(1, wdt_addr(wdt, WDT_BITE_TIME));
	mb(); /* Avoid unpredictable behaviour in concurrent executions */
	pr_info("Configuring Watchdog Timer\n");
	writel(1, wdt_addr(wdt, WDT_RST));
	writel(1, wdt_addr(wdt, WDT_EN));
	mb(); /* Make sure the above sequence hits hardware before Reboot. */
	pr_info("Waiting for Reboot\n");

	mdelay(1);
	pr_err("Wdog - CTL: 0x%x, BARK TIME: 0x%x, BITE TIME: 0x%x",
		readl(wdt_addr(wdt, WDT_EN)),
		readl(wdt_addr(wdt, WDT_BARK_TIME)),
		readl(wdt_addr(wdt, WDT_BITE_TIME)));
	return IRQ_HANDLED;
}

void register_wdt_bark_irq(int irq, struct qcom_wdt *wdt)
{
	int ret;

	ret = request_irq(irq, wdt_bark_isr, IRQF_TRIGGER_HIGH,
						"watchdog bark", wdt);
	if (ret)
		pr_err("error request_irq(irq_num:%d ) ret:%d\n", irq, ret);
}

static int qcom_wdt_probe(struct platform_device *pdev)
{
	const struct of_device_id *id;
	struct qcom_wdt *wdt;
	struct resource *res;
	struct device_node *np = pdev->dev.of_node;
	u32 percpu_offset;
	int ret, irq;
	uint32_t val;
	unsigned int retn, extwdt_val = 0, regaddr;

	wdt = devm_kzalloc(&pdev->dev, sizeof(*wdt), GFP_KERNEL);
	if (!wdt)
		return -ENOMEM;
	irq = platform_get_irq_byname(pdev, "bark_irq");
	res = platform_get_resource_byname(pdev, IORESOURCE_MEM, "kpss_wdt");
	if (!res) {
		dev_err(&pdev->dev, "%s: no mem resource\n", __func__);
		return -EINVAL;
	}

	/* We use CPU0's DGT for the watchdog */
	if (of_property_read_u32(np, "cpu-offset", &percpu_offset))
		percpu_offset = 0;

	res->start += percpu_offset;
	res->end += percpu_offset;

	wdt->base = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(wdt->base))
		return PTR_ERR(wdt->base);

	wdt->tlv_res = platform_get_resource_byname
			(pdev, IORESOURCE_MEM, "tlv");

	id = of_match_device(qcom_wdt_of_table, &pdev->dev);
	if (!id)
		return -ENODEV;

	wdt->dev_props = (struct qcom_wdt_props *)id->data;

	mhi_wdt_panic_enable = of_property_read_bool(np, "mhi-wdt-panic-enable");

	/*
	 * mhi-wdt-panic-enable if set, BARK and BITE time should have
	 * enough difference for the MDM to collect crash dump and
	 * reboot i.e BITE time should be set twice as BARK time.
	 */
	if (wdt->dev_props->secure_wdog && !mhi_wdt_panic_enable)
		wdt->bite = 1;

	if (irq > 0)
		register_wdt_bark_irq(irq, wdt);

	wdt->clk = devm_clk_get(&pdev->dev, NULL);
	if (IS_ERR(wdt->clk)) {
		dev_err(&pdev->dev, "failed to get input clock\n");
		return PTR_ERR(wdt->clk);
	}

	ret = clk_prepare_enable(wdt->clk);
	if (ret) {
		dev_err(&pdev->dev, "failed to setup clock\n");
		return ret;
	}

	/*
	 * We use the clock rate to calculate the max timeout, so ensure it's
	 * not zero to avoid a divide-by-zero exception.
	 *
	 * WATCHDOG_CORE assumes units of seconds, if the WDT is clocked such
	 * that it would bite before a second elapses it's usefulness is
	 * limited.  Bail if this is the case.
	 */
	wdt->rate = clk_get_rate(wdt->clk);
	if (wdt->rate == 0 ||
	    wdt->rate > 0x10000000U) {
		dev_err(&pdev->dev, "invalid clock rate\n");
		ret = -EINVAL;
		goto err_clk_unprepare;
	}

	ret = work_on_cpu(0, qcom_wdt_configure_bark_dump, wdt);
	if (ret)
		wdt->wdd.ops = &qcom_wdt_ops_nonsecure;
	else
		wdt->wdd.ops = &qcom_wdt_ops_secure;

	wdt->wdd.dev = &pdev->dev;
	wdt->wdd.info = &qcom_wdt_info;
	wdt->wdd.min_timeout = 1;
	if (!of_property_read_u32(np, "wdt-max-timeout", &val))
		wdt->wdd.max_timeout = val;
	else
		wdt->wdd.max_timeout = 0x10000000U / wdt->rate;
	wdt->wdd.parent = &pdev->dev;

	/*
	 * If 'timeout-sec' unspecified in devicetree, assume a 30 second
	 * default, unless the max timeout is less than 30 seconds, then use
	 * the max instead.
	 */
	wdt->wdd.timeout = min(wdt->wdd.max_timeout, 30U);
	watchdog_init_timeout(&wdt->wdd, 0, &pdev->dev);

	ret = watchdog_register_device(&wdt->wdd);
	if (ret) {
		dev_err(&pdev->dev, "failed to register watchdog\n");
		goto err_clk_unprepare;
	}

	/*
	 * WDT restart notifier has priority 0 (use as a last resort)
	 */
	wdt->restart_nb.notifier_call = qcom_wdt_restart;
	atomic_notifier_chain_register(&panic_notifier_list, &panic_blk);
	ret = register_restart_handler(&wdt->restart_nb);
	if (ret)
		dev_err(&pdev->dev, "failed to setup restart handler\n");

#ifdef CONFIG_QCA_MINIDUMP
	ret = register_module_notifier(&wlan_nb);
	if (ret)
		dev_err(&pdev->dev, "failed to register WLAN modules callback\n");
	ret = register_module_notifier(&wlan_module_exit_nb);
	if (ret)
		dev_err(&pdev->dev, "Failed to register WLAN module exit notifier\n");
#ifdef CONFIG_QCA_MINIDUMP_DEBUG
	ret = atomic_notifier_chain_register(&panic_notifier_list,
				&wlan_panic_nb);
	if (ret)
		dev_err(&pdev->dev,
			"Failed to register panic notifier for WLAN module info\n");
#endif /*CONFIG_QCA_MINIDUMP_DEBUG*/
#endif /*CONFIG_QCA_MINIDUMP*/
	platform_set_drvdata(pdev, wdt);

	if (!of_property_read_u32(np, "extwdt-val", &val)) {
		extwdt_val = val;

		if (of_property_read_u32(np, "tcsr-reg", &regaddr))
			regaddr = TCSR_WONCE_REG;

		retn = qcom_fiq_extwdt(regaddr, extwdt_val);
		if (retn)
			dev_err(&pdev->dev, "FIQ scm_call failed\n");
	}

	return 0;

err_clk_unprepare:
	clk_disable_unprepare(wdt->clk);
	return ret;
}

static int qcom_wdt_remove(struct platform_device *pdev)
{
	struct qcom_wdt *wdt = platform_get_drvdata(pdev);

	unregister_restart_handler(&wdt->restart_nb);
	watchdog_unregister_device(&wdt->wdd);
	clk_disable_unprepare(wdt->clk);
	return 0;
}

static struct platform_driver qcom_watchdog_driver = {
	.probe	= qcom_wdt_probe,
	.remove	= qcom_wdt_remove,
	.driver	= {
		.name		= KBUILD_MODNAME,
		.of_match_table	= qcom_wdt_of_table,
	},
};
module_platform_driver(qcom_watchdog_driver);

MODULE_DESCRIPTION("QCOM KPSS Watchdog Driver");
MODULE_LICENSE("GPL v2");
