/******************************************************************************

			Copyright (c) 2012
			Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
#ifndef _DRV_TMU_API_H_
#define _DRV_TMU_API_H_

#define SEQ_PRINTF seq_printf


/*Macro tmu_proc_printf for sharing same dump API for cat/echo dump commands.
   if S == NULL, it will finally call printk API.
   otherwise it will call seq_printf via seq_file handler
*/
#define tmu_proc_printf(s, fmt, arg...) \
	do { \
		if (!s) \
			TMU_PRINT(fmt, ##arg); \
		else \
			seq_printf(s, fmt, ##arg); \
	} while (0)

/** \defgroup TMU_API Management API Reference - Internals
   @{
*/

/** \defgroup TMU_COMMON_INTERNAL Common Driver Interface

   This chapter describes the generic part of the internal driver interface.

   @{
*/
/** Driver version, major number */
#define TMU_VER_MAJOR            0
/** Driver version, minor number */
#define TMU_VER_MINOR            0
/** Driver version, build number */
#define TMU_VER_STEP             1
/** Driver version, package type */
#define TMU_VER_TYPE             0
/** Driver version as string */
/*Enable proc related debug tools */
#define TMU_LOCAL_STAT_DEBUG   1
/*Enable debugging support during calling TMU API */
#define INCLUDE_DEBUG_SUPPORT  1

#if defined(CONFIG_PROC_FS)
#define INCLUDE_PROCFS_SUPPORT 1
#endif

#include <linux/delay.h>
#include <asm/div64.h>		/* do_div */
#include <linux/uaccess.h>
#include <linux/io.h>
#include <linux/kernel.h>
#include <linux/ctype.h>
#include <linux/seq_file.h>
#include <linux/string.h>	/* memset */
#include <linux/sort.h>
#include <linux/version.h>
#include <linux/time.h>
#include <linux/jiffies.h>
#include <linux/types.h>
#include <linux/fs.h>
#include<linux/slab.h>
#ifdef CONFIG_LTQ_TMU_DEVICE_TREE
#include <linux/platform_device.h>
#endif
#include <linux/interrupt.h>
#ifdef CONFIG_MODULES
#include <linux/module.h>
#endif				/*MODULE */
#include <linux/klogging.h>

#if defined(CONFIG_PROC_FS) && defined(INCLUDE_PROCFS_SUPPORT)
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#endif				/*CONFIG_PROC_FS & INCLUDE_PROCFS_SUPPORT */

#define tmu_strcpy  strcpy
#define tmu_strlen  strlen
#define tmu_strspn  strspn
#define tmu_strpbrk strpbrk
#define tmu_strchr  strchr
#define tmu_memset  memset

#define PRINTK	LOGF_KLOG_CONT
#define PR_ERR	LOGF_KLOG_ERROR
#define PR_INFO	LOGF_KLOG_INFO
#define PR_CONT	LOGF_KLOG_CONT
#define PR_INFO_ONCE	LOGF_KLOG_INFO_ONCE

/* no simulation, FPGA, ... -> direct access possible */
/** Read value of register

   \param reg  register address
   \return register contents
*/
#define reg_r32(reg)		__raw_readl(reg)
/** Write value to register

   \param val  register value
   \param reg  register address
*/
#define reg_w32(val, reg)	__raw_writel(val, reg)


/** Clear / set bits within a register

   \param clear   clear mask
   \param set     set mask
   \param reg     register address
*/
#define reg_w32_mask(clear, set, reg)\
	reg_w32((reg_r32(reg) & ~(clear)) | (set), reg)
#define reg_r32_table(reg, idx) reg_r32(&((uint32_t *)&reg)[idx])
/** Write value to table entry

   \param val  register value
   \param reg  register address
   \param idx  number of the uint32 table element
*/
#define reg_w32_table(val, reg, idx) reg_w32(val, &((uint32_t *)&reg)[idx])
/** Write value to table entry

   \param clear   clear mask
   \param set     set mask
   \param reg     register address
   \param idx     number of the uint32 table element
*/
#define reg_w32_table_mask(clear, set, reg, idx) \
	reg_w32_table((reg_r32_table(reg, idx) & ~(clear)) | (set), reg, idx)

#define set_val(reg, val, mask, offset) do \
	{(reg) &= ~(mask);\
	(reg) |= (((val) << (offset)) & (mask));\
	} while (0)

#define get_val(val, mask, offset) (((val) & (mask)) >> (offset))

enum tmu_errorcode {
	/* Common error codes */
	/** Not supported by software yet */
	TMU_STATUS_NOT_IMPLEMENTED = 0,
	/** At least one value is out of range */
	TMU_STATUS_VALUE_RANGE_ERR = -5,
	/** GPE is not initialized */
	TMU_STATUS_NOT_INITIALIZED = -4,
	/** No IOCTL found error */
	TMU_STATUS_IOCTL_NOT_FOUND_ERR = -3,
	/** Operation is not supported for this chip */
	TMU_STATUS_CHIP_NOT_SUPPORTED = -2,
	/** Generic or unknown error occurred */
	TMU_STATUS_ERR = -1,
	/** No error */
	TMU_STATUS_OK = 0,

	/** Threshold Crossing Alert */
	TMU_STATUS_TCA = 1,

	/* GTC specific warnings */
	/** Warning: GEM Port in use. */
	GTC_STATUS_VALUE_PORT_IN_USE = 1009,
	/** Warning: Value is out of range.
	    Value was set to predefined value.
	*/
	GTC_STATUS_VALUE_RANGE_WARN = 1008,

	/** Resource exists */
	STATUS_EXISTS = 3011,

    /** Low Level modules enable failed */
	STATUS_VALUE_LL_MODULE_ENABLE_ERR = -3009,
	STATUS_NOT_AVAILABLE = -3011,

};

#define TMU_PROC_NAME    "tmu"
#define PROC_BASE        " /proc/"TMU_PROC_NAME"/"
#define TMU_PROC_PARENT ""
#define PROC_FILE_TMU    "tmu"
#define PROC_FILE_EQT    "eqt"
#define PROC_FILE_EPT    "ept"
#define PROC_FILE_SBIT   "sbit"
#define PROC_FILE_SBOT   "sbot"
#define PROC_FILE_TBST   "tbst"
#define PROC_FILE_TBST1  "tbst1"
#define PROC_FILE_PPT    "ppt"
#define PROC_FILE_QUEUE  "queue"
#define PROC_FILE_QUEUE1 "queue1"
#define PROC_FILE_DBG    "dbg"
#define PROC_FILE_TEST   "test"
#define PROC_FILE_VER    "ver"
#define PROC_FILE_CBMLOOKUP    "lookup"
#define PROC_FILE_CASCADE "cascade"

#define PROC_FILE_CLK "clk"

#define LOOKUP_BITS_INDEX(ep, classid, mpe1, mpe2, enc, dec, flowid, dic) {\
	(((uint32_t)flowid  << 12) & 0x3000) \
	| ((dec   << 11) & 0x800) \
	| ((enc   << 10) & 0x400) \
	| ((mpe2  << 9) & 0x200) \
	| ((mpe1  << 8) & 0x100) \
	| ((ep    << 4) & 0xF0)\
	| ((classid) & 0x0F) \
	}

#ifdef __GNUC__
#define TMU_ASSERT_SCOPE __func__
#else
#define TMU_ASSERT_SCOPE __FILE__
#endif

#define TMU_DEBUG_MSG             0x00000001
#define TMU_DEBUG_QUEUE1          0x40000000
#define TMU_DEBUG_ERR             0x80000000
#define TMU_DEBUG_ENABLE_MASK_ALL (-1)

#undef TMU_DEBUG
#undef TMU_DEBUG_ASSERT
#undef TMU_PRINT
#if defined(INCLUDE_DEBUG_SUPPORT) && INCLUDE_DEBUG_SUPPORT
extern unsigned int tmu_debug_lvl, tmu_max_print_num, tmu_print_num_en;
#define TMU_DEBUG(flag, fmt, arg...) \
	do { \
		if ((tmu_debug_lvl & flag) && (((tmu_print_num_en) \
			&& (tmu_max_print_num)) || (!tmu_print_num_en))) { \
			PRINTK(KERN_ERR fmt, ##arg); \
			if ((tmu_print_num_en) && (tmu_max_print_num)) \
				tmu_max_print_num--; \
			} \
	} while (0)

#define TMU_DEBUG_ASSERT(expr) \
	do { \
		if ((expr)) { \
			PR_INFO("%s\n", #expr); \
			return TMU_STATUS_VALUE_RANGE_ERR; \
	} \
	} while (0)
#define TMU_PRINT	LOGF_KLOG_CONT

#else
#define TMU_DEBUG(flag, fmt, arg...)
#define TMU_DEBUG_ASSERT(expr)
#define TMU_PRINT
#endif

struct tmu_tm {
	/*
	 * the number of seconds after the minute, normally in the range
	 * 0 to 59, but can be up to 60 to allow for leap seconds
	 */
	uint8_t tm_sec;
	/* the number of minutes after the hour, in the range 0 to 59 */
	uint8_t tm_min;
	/* the number of hours past midnight, in the range 0 to 23 */
	uint8_t tm_hour;
	/* the day of the month, in the range 1 to 31 */
	uint8_t tm_mday;
	/* the number of months since January, in the range 0 to 11 */
	uint8_t tm_mon;
	/* the number of years since 1900 */
	uint16_t tm_year;
	/* the number of days since Sunday, in the range 0 to 6 */
	uint8_t tm_wday;
	/* the number of days since January 1, in the range 0 to 365 */
	uint16_t tm_yday;
};

int32_t tmu_spin_lock_init(spinlock_t *id, const char *p_name);
int32_t tmu_spin_lock_delete(spinlock_t *id);
int32_t tmu_spin_lock_get(spinlock_t *id, unsigned long *flags);
int32_t tmu_spin_lock_release(spinlock_t *id, unsigned long flags);
int tmu_ll_chiptest(int layer_mode, int rate_kbps, int rate_port_flag,
		    int wfq[5], int tb_mode);

#if defined(TMU_LOCAL_STAT_DEBUG)

/* Dump the TMU information. */
void tmu_dump(struct seq_file *s);
void tmu_eqt_dump(struct seq_file *s);
void tmu_ept_dump(struct seq_file *s);
int tmu_sbit_dump(struct seq_file *s, int pos);
int tmu_sbot_dump(struct seq_file *s, int pos);
int tmu_ppt_dump_start(void);
int tmu_ppt_dump(struct seq_file *s, int pos);
int tmu_queue_dump_start(void);
int tmu_queue_dump(struct seq_file *s, int pos);
int tmu_queue_raw_dump(struct seq_file *s, int pos);
#endif

ssize_t tmu_proc_get_qid_via_index(struct file *file, const char *buf,
				   size_t count, loff_t *ppos);
ssize_t tmu_proc_eqt_write(struct file *file, const char *buf, size_t count,
			   loff_t *ppos);
ssize_t tmu_proc_port_write(struct file *file, const char *buf, size_t count,
			    loff_t *ppos);
void tmu_proc_port_write_help(void);
ssize_t tmu_proc_port_write(struct file *file, const char *buf, size_t count,
			    loff_t *ppos);
void tmu_proc_debug_dump(struct seq_file *s);
ssize_t tmu_proc_debug_write(struct file *file, const char *buf, size_t count,
			     loff_t *ppos);
ssize_t tmu_proc_queue_write(struct file *file, const char *buf, size_t count,
			     loff_t *ppos);
ssize_t tmu_proc_sbit_write(struct file *file, const char *buf, size_t count,
			    loff_t *ppos);
ssize_t tmu_proc_sbot_write(struct file *file, const char *buf, size_t count,
			    loff_t *ppos);
ssize_t tmu_proc_tbst_write(struct file *file, const char *buf, size_t count,
			    loff_t *ppos);
ssize_t tmu_proc_tmu_cascade_write(struct file *file, const char *buf,
				   size_t count, loff_t *ppos);
int tmu_sbit_dump_start(void);
int tmu_sbot_dump_start(void);
int tmu_sbit_dump(struct seq_file *s, int pos);
int tmu_sbot_dump(struct seq_file *s, int pos);
int tmu_tbst_dump(struct seq_file *s, int pos);
int tmu_tbst_dump_start(void);
/*void tmu_tbst_dump(struct seq_file *s);*/
int tmu_tbst_dump(struct seq_file *s, int pos);
int tmu_queue_dump_start(void);
int tmu_queue_raw_dump(struct seq_file *s, int pos);
int tmu_queue_dump_start(void);
int tmu_queue_dump(struct seq_file *s, int pos);
void tmu_dump(struct seq_file *s);
void tmu_eqt_dump(struct seq_file *s);
void tmu_ept_dump(struct seq_file *s);
void tmu_proc_dummy_read(struct seq_file *s);
ssize_t tmu_proc_clk_write(struct file *file, const char *buf, size_t count,
			   loff_t *ppos);
void tmu_proc_clk_dump(struct seq_file *s);
int tmu_reset_mib(u32 index);
int find_pattern(int port_id, struct seq_file *s, int qid);
char *get_dma_flags_str(uint32_t epn, char *buf, int buf_len);
/*! @} */

/*! @} */

#endif				/* _DRV_TMU_API_H_ */
