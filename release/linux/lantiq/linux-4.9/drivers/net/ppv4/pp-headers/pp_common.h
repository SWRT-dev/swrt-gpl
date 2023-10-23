/*
 * Description: PP common definitions
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2018-2020 Intel Corporation
 */

#ifndef __PP_COMMON_H_
#define __PP_COMMON_H_

#include <linux/kernel.h>       /* for scnprintf */
#include <linux/debugfs.h>      /* struct dentry */
#include <linux/bitops.h>       /* BIT()         */
#include <linux/bitfield.h>     /* FIELD_GET/MOD */
#include <linux/ctype.h>        /* tolower       */
#include <linux/string.h>       /* strscpy       */
#include <linux/log2.h>         /* is_power_of_2 */
#include <linux/errno.h>
#include <linux/jiffies.h>
#include <linux/delay.h>
#include <linux/version.h>
#include <linux/dma-mapping.h>
#if KERNEL_VERSION(4, 12, 0) > LINUX_VERSION_CODE
#include <linux/vmalloc.h>
#endif
#include <linux/pp_api.h>

/**
 * @brief PP HW silicon stepping versions definitions
 * @note this is based on the version register in the infra HW module
 *       keep the values aligned to the register values!
 */
enum pp_silicon_step {
	PP_SSTEP_A = 1,
	PP_SSTEP_B = 2,
};

/**
 * @brief Defines the write policies for the PP session and counters
 *        caches
 */
enum cache_wr_policy {
	/* write back, every write to the memory will be written
	 * only to the cache
	 */
	PP_CACHE_WR_BACK,
	/* write through, every write to the memory will be written
	 * both to the cache and memory
	 */
	PP_CACHE_WR_THR
};

#define BITS_PER_U32                   (sizeof(u32) * BITS_PER_BYTE)
#define BITS_PER_U64                   (sizeof(u64) * BITS_PER_BYTE)

/**
 * Packet Processor version
 */
#define PP_VER_MAJOR                   (1)
#define PP_VER_MAJOR_MID               (0)
#define PP_VER_MID                     (0)
#define PP_VER_MINOR_MID               (0)
#define PP_VER_MINOR                   (0)

/**
 * @define PP max supported RPB ports
 */
#define PP_MAX_RPB_PORT                (4)

/**
 * @define PP max supported traffic classes
 */
#define PP_MAX_TC                      (4)

/**
 * @define Default number of bytes being prefetched for each
 *         packet entering the PP HW
 */
#define PP_DEFAULT_PREFETCH_SZ         (256)

/**
 * @define Shortcut macro to check if port number is a valid
 *         port number.
 * @param port port number
 */
#define PP_IS_PORT_VALID(port) \
	(0 <= (port) && (port) < PP_MAX_PORT)

/**
 * @define Check if a number is an odd number
 * @param num the number
 */
#define num_is_odd(num)                ((num) & (typeof(num))BIT(0))

/**
 * @define Check if a number is an even number
 * @param num the number
 */
#define num_is_even(num)               !num_is_odd(num)

/**
 * @brief Convert microseconds to numbers of cycles in hw based on hw
 *        clock frequency
 * @param freq clock frequency in MHZ
 * @param ms number of ms to convert
 */
#define US_TO_HW_CYCLES(freq, us)      (typeof(us))((us) * (freq))

/**
 * @brief Convert numbers of cycles in hw to microseconds based on hw
 *        clock frequency
 * @note User MUST verify freq isn't zero even due the macro is safe
 * @param freq clock frequency in MHZ
 * @param ms number of ms to convert
 */
#define HW_CYCLES_TO_US(freq, cycles)                                          \
	(typeof(cycles))((cycles) / max_t(typeof(freq), freq, 1))

/**
 * @brief Convert mega hertz to hertz
 */
#define MHZ_TO_HZ(mhz)                 ((mhz) * 1000000)

/**
 * @define Size of common status word in words
 */
#define STW_COMMON_WORD_CNT            (4)

/**
 * @enum STW common fields enumeration
 *
 * @STW_COMMON_FLD_PROTOCOL_SPECIFIC_0
 * @STW_COMMON_FLD_PROTOCOL_SPECIFIC_1
 *
 * @STW_COMMON_FLD_DATA_OFFSET
 *        In LGM: data offset to port distributer is:
 *        Data_offset + PreL2Size*16. Port distributer removes
 *        the PreL2 sizes
 *
 * @STW_COMMON_FLD_PMAC_HEADER_IND
 *        if set 16B of PMAC header Part of Pre L2
 *
 * @STW_COMMON_FLD_TC_COLOR
 *        TC @ ingress, color @ egress
 *
 * @STW_COMMON_FLD_SOURCE_POOL
 * @STW_COMMON_FLD_BUFF_BASE_PTR
 *        pointer = buffer base pointer*128
 *
 * @STW_COMMON_FLD_PRE_L2_SZ
 *        including PMAC header if exists, 16B granularity
 *
 * @STW_COMMON_FLD_TIMESTAMP_IND
 * @STW_COMMON_FLD_PORT
 *        Egress Port @ RXDMA output, Ingress port @ RPB input
 *
 * @STW_COMMON_FLD_POLICY
 * @STW_COMMON_FLD_PACKET_LEN
 *        In LGM: packet length to port distributer is: Packet
 *        Length = Packet Length - PreL2Size*16 - Timestamp
 *        indication*10. Port distributer removes the PreL2 & TS
 *        sizes
 *
 * @STW_COMMON_FLD_2ND_ROUND
 * @STW_COMMON_FLD_HDR_MODE
 */
enum pp_stw_common_fld {
	STW_COMMON_FLD_FIRST,
	STW_COMMON_FLD_PS_0 = STW_COMMON_FLD_FIRST,
	STW_COMMON_FLD_PS_1,
	STW_COMMON_FLD_DATA_OFFSET,
	STW_COMMON_FLD_PMAC_HDR_IND,
	STW_COMMON_FLD_TC_COLOR,
	STW_COMMON_FLD_SOURCE_POOL,
	STW_COMMON_FLD_BUFF_BASE_PTR,
	STW_COMMON_FLD_PRE_L2_SZ,
	STW_COMMON_FLD_TIMESTAMP_IND,
	STW_COMMON_FLD_PORT,
	STW_COMMON_FLD_POLICY,
	STW_COMMON_FLD_PACKET_LEN,
	STW_COMMON_FLD_2ND_ROUND,
	STW_COMMON_FLD_HDR_MODE,
	STW_COMMON_FLD_LAST = STW_COMMON_FLD_HDR_MODE,
	STW_COMMON_FLDS_NUM
};

/**
 * @brief STW common fields masks
 */
#define STW_COMMON_FLD_PS_0_LSB                 (0  + 0 * BITS_PER_U32)
#define STW_COMMON_FLD_PS_0_MSB                 (31 + 0 * BITS_PER_U32)
#define STW_COMMON_FLD_PS_1_LSB                 (0  + 1 * BITS_PER_U32)
#define STW_COMMON_FLD_PS_1_MSB                 (15 + 1 * BITS_PER_U32)
#define STW_COMMON_FLD_DATA_OFFSET_LSB          (16 + 1 * BITS_PER_U32)
#define STW_COMMON_FLD_DATA_OFFSET_MSB          (24 + 1 * BITS_PER_U32)
#define STW_COMMON_FLD_PMAC_HDR_IND_LSB         (25 + 1 * BITS_PER_U32)
#define STW_COMMON_FLD_PMAC_HDR_IND_MSB         (25 + 1 * BITS_PER_U32)
#define STW_COMMON_FLD_TC_COLOR_LSB             (26 + 1 * BITS_PER_U32)
#define STW_COMMON_FLD_TC_COLOR_MSB             (27 + 1 * BITS_PER_U32)
#define STW_COMMON_FLD_SOURCE_POOL_LSB          (28 + 1 * BITS_PER_U32)
#define STW_COMMON_FLD_SOURCE_POOL_MSB          (31 + 1 * BITS_PER_U32)
#define STW_COMMON_FLD_BUFF_BASE_PTR_LSB        (0  + 2 * BITS_PER_U32)
#define STW_COMMON_FLD_BUFF_BASE_PTR_MSB        (28 + 2 * BITS_PER_U32)
#define STW_COMMON_FLD_PRE_L2_SZ_LSB            (29 + 2 * BITS_PER_U32)
#define STW_COMMON_FLD_PRE_L2_SZ_MSB            (30 + 2 * BITS_PER_U32)
#define STW_COMMON_FLD_TIMESTAMP_IND_LSB        (31 + 2 * BITS_PER_U32)
#define STW_COMMON_FLD_TIMESTAMP_IND_MSB        (31 + 2 * BITS_PER_U32)
#define STW_COMMON_FLD_PORT_LSB                 (0  + 3 * BITS_PER_U32)
#define STW_COMMON_FLD_PORT_MSB                 (7  + 3 * BITS_PER_U32)
#define STW_COMMON_FLD_POLICY_LSB               (8  + 3 * BITS_PER_U32)
#define STW_COMMON_FLD_POLICY_MSB               (15 + 3 * BITS_PER_U32)
#define STW_COMMON_FLD_PACKET_LEN_LSB           (16 + 3 * BITS_PER_U32)
#define STW_COMMON_FLD_PACKET_LEN_MSB           (29 + 3 * BITS_PER_U32)
#define STW_COMMON_FLD_2ND_ROUND_LSB            (30 + 3 * BITS_PER_U32)
#define STW_COMMON_FLD_2ND_ROUND_MSB            (30 + 3 * BITS_PER_U32)
#define STW_COMMON_FLD_HDR_MODE_LSB             (31 + 3 * BITS_PER_U32)
#define STW_COMMON_FLD_HDR_MODE_MSB             (31 + 3 * BITS_PER_U32)

/**
 * @brief STW common fields description
 */
#define STW_COMMON_FLD_PS_0_DESC                "Protocol Specific 0"
#define STW_COMMON_FLD_PS_1_DESC                "Protocol specific 1"
#define STW_COMMON_FLD_DATA_OFFSET_DESC         "Data Offset"
#define STW_COMMON_FLD_PMAC_HDR_IND_DESC        "PMAC Header"
#define STW_COMMON_FLD_TC_COLOR_DESC            "TC Color"
#define STW_COMMON_FLD_SOURCE_POOL_DESC         "Source Pool"
#define STW_COMMON_FLD_BUFF_BASE_PTR_DESC       "Buffer Pointer"
#define STW_COMMON_FLD_PRE_L2_SZ_DESC           "Pre L2 Size"
#define STW_COMMON_FLD_TIMESTAMP_IND_DESC       "Timestamp Index"
#define STW_COMMON_FLD_PORT_DESC                "Ingress Port"
#define STW_COMMON_FLD_POLICY_DESC              "Policy"
#define STW_COMMON_FLD_PACKET_LEN_DESC          "Packet Length"
#define STW_COMMON_FLD_2ND_ROUND_DESC           "Second Round"
#define STW_COMMON_FLD_HDR_MODE_DESC            "Header Mode"

#define STW_DROP_PKT_STR(drop_pkt)                                \
	((drop_pkt) == 0 ? "No Drop" :                            \
	((drop_pkt) == 1 ? "Dropped in HW parser to uC" :         \
	((drop_pkt) == 2 ? "Dropped in Parser uC" :               \
	((drop_pkt) == 3 ? "Dropped in HW classifier to uC" :     \
	((drop_pkt) == 4 ? "Dropped in Classifier uC" :           \
	((drop_pkt) == 5 ? "Dropped in PPRS" :                    \
	((drop_pkt) == 6 ? "Dropped in Checker" :                 \
	((drop_pkt) == 7 ? "Dropped in Parser" :                  \
	("Unknown")))))))))

/**
 * @brief Convert pp hw 9 bits integer to 64 bits integer
 */
static inline s64 fld_data_offset_from_hw(s64 val)
{
	/* extend val from 9bit sign to 64 bits sign */
	return sign_extend64(val, 8);
}

/**
 * @brief Convert pp hw 10 bits integer to 64 bits integer
 */
static inline s64 fld_diff_from_hw(s64 val)
{
	/* extend val from 10bit sign to 64 bits sign */
	return sign_extend64(val, 9);
}

/**
 * @define STW_INIT_COMMON_FLDS
 * @brief Shortcut to initialize status word common fields
 * @param array of struct buf_fld_info
 */
#define STW_INIT_COMMON_FLDS(flds)                                          \
do {                                                                        \
	BUF_FLD_INIT_DESC(flds, STW_COMMON_FLD_PS_0);                       \
	BUF_FLD_INIT_DESC(flds, STW_COMMON_FLD_PS_1);                       \
	BUF_FLD_INIT_DESC(flds, STW_COMMON_FLD_DATA_OFFSET);                \
	flds[STW_COMMON_FLD_DATA_OFFSET].from_hw = fld_data_offset_from_hw; \
	BUF_FLD_INIT_DESC(flds, STW_COMMON_FLD_PMAC_HDR_IND);               \
	BUF_FLD_INIT_DESC(flds, STW_COMMON_FLD_TC_COLOR);                   \
	BUF_FLD_INIT_DESC(flds, STW_COMMON_FLD_SOURCE_POOL);                \
	BUF_FLD_INIT_DESC(flds, STW_COMMON_FLD_BUFF_BASE_PTR);              \
	BUF_FLD_INIT_DESC(flds, STW_COMMON_FLD_PRE_L2_SZ);                  \
	BUF_FLD_INIT_DESC(flds, STW_COMMON_FLD_TIMESTAMP_IND);              \
	BUF_FLD_INIT_DESC(flds, STW_COMMON_FLD_PORT);                       \
	BUF_FLD_INIT_DESC(flds, STW_COMMON_FLD_POLICY);                     \
	BUF_FLD_INIT_DESC(flds, STW_COMMON_FLD_PACKET_LEN);                 \
	BUF_FLD_INIT_DESC(flds, STW_COMMON_FLD_2ND_ROUND);                  \
	BUF_FLD_INIT_DESC(flds, STW_COMMON_FLD_HDR_MODE);                   \
} while (0)

/**
 * @define Boolean to str conversion for printing
 */
#define BOOL2STR(_bool)         ((_bool) ? "True" : "False")

/**
 * @define Boolean to enable/disable str conversion for printing
 */
#define BOOL2EN(_bool)          ((_bool) ? "Enable" : "Disable")

struct pp_stw_common_values {
	u32 val[STW_COMMON_FLDS_NUM];
};

/**
 * @brief Convert mask to the maximum value allowed by the mask
 * @mask the mask
 * @note we must cast to unsigned type to avoid int to be shifted
 *       down with ones, we case to u64 to support all types
 */
#define MASK2MAX(mask) \
	((typeof(mask))(((u64)mask) >> __ffs((ulong)mask)))

/**
 * @brief Check if a value fits into a field defined by mask
 * @param mask mask to test
 * @param val value to check
 * @return bool return true if value can fit info the field defined by
 *         mask, false otherwise
 */
static inline
bool __pp_mask_value_valid(unsigned long mask, unsigned long val)
{
	if (likely(val <= MASK2MAX(mask)))
		return true;

	pr_err("value %#lx doesn't fit mask %#lx\n", val, mask);
	return false;
}

/**
 * @brief Same as '__pp_mask_value_valid' just with extra debug prints
 * @param mask mask to test
 * @param mask_str mask variable name in the caller function
 * @param val value to check
 * @param val_str val variable name in the caller function
 * @param caller caller name
 * @return bool return true if value can fit info the field defined by
 *         mask, false otherwise
 */
static inline
bool _pp_mask_value_valid(unsigned long mask, const char *mask_str,
			  unsigned long val, const char *val_str,
			  const char *caller)
{
	if (likely(val <= MASK2MAX(mask)))
		return true;

	pr_err("%s: value '%s' %#lx doesn't fit mask '%s' %#lx\n",
	       caller, val_str, val, mask_str, mask);
	return false;
}

/**
 * @brief Check if a value fits into a field defined by mask
 * @param mask mask to test
 * @param val value to check
 * @return bool return true if value can fit info the field defined by
 *         mask, false otherwise
 */
#define pp_mask_value_valid(mask, val) \
	_pp_mask_value_valid(mask, #mask, val, #val, __func__)

/**
 * @brief Test whether the specified mask is a valid mask
 *        Valid mask is a sequence on set bits: 0xf, 0xff, 0xfc, 0x38
 *        0xffffffff, 0xffffffffffffffff
 * @param mask mask to test
 * @return bool true if mask is valid, false otherwise
 */
#define pp_is_mask_valid(mask) \
	__pp_is_mask_valid(mask, #mask, __func__)
/**
 * @brief Test whether the specified mask is a valid mask
 * @param mask mask to test
 * @param mask_str mask variable name in the caller function
 * @param caller caller name
 * @return bool true if mask is valid, false otherwise
 */
static inline bool __pp_is_mask_valid(unsigned long mask, const char *mask_str,
				      const char *caller)
{
	if (likely(mask == ULONG_MAX))
		return true;
	if (likely(mask && is_power_of_2((mask >> __ffs(mask)) + 1)))
		return true;

	pr_err("%s: mask '%s' %#lx is invalid\n", caller, mask_str, mask);
	return false;
}

/**
 * @brief Extract a bitfield element
 * @note Similar implementation to FIELD_GET with support for
 *       non-const mask.
 * @param mask shifted mask defining the field's length and position,
 *             use BIT(n) or GENMASK(h, l) <br>
 *             Valid mask 0x7f, 0x3, 0x4, 0xfc, 0x6 <br>
 *             Non-valid mask: 0x5, 0xA, 0x34
 * @param reg 32bit value to extract the bitfield from
 * @return unsigned long extracted bitfield shifted down
 */
#define pp_field_get(mask, reg) \
	__pp_field_get(mask, #mask, reg, __func__)

/**
 * @brief Extract a bitfield element
 * @note Similar implementation to FIELD_GET with support for
 *       non-const mask.
 * @note internal -- DO NOT USE
 * @param mask shifted mask defining the field's length and position,
 * @param mask_str mask variable name in the caller function
 * @param reg 32bit value to extract the bitfield from
 * @param caller caller name
 * @return unsigned long extracted bitfield shifted down
 */
static inline
unsigned long __pp_field_get(unsigned long mask, const char *mask_str,
			     unsigned long reg, const char *caller)
{
	if (unlikely(!pp_is_mask_valid(mask))) {
		pr_err("%s: mask '%s' %#lx isn't valid\n",
		       caller, mask_str, mask);
		return 0;
	}

	return (reg & mask) >> (__ffs(mask));
}

/**
 * @brief Prepare a bitfield element
 * @param mask shifted mask defining the field's length and position,
 *             use ONLY BIT(n) or GENMASK(h, l) <br>
 *             Valid mask: 0x7f, 0x3, 0x4, 0xfc, 0x6 <br>
 *             Non-valid mask: 0x5, 0xA, 0x34
 * @param val value to prepare
 * @return unsigned long val masks and shifted up
 */
#define pp_field_prep(mask, val) \
	__pp_field_prep(mask, #mask, val, #val, __func__)

/**
 * @brief Prepare a bitfield element
 * @note Similar implementation to FIELD_PREP with support for
 *       non-const mask.
 * @note internal -- DO NOT USE
 * @param mask shifted mask defining the field's length and position
 * @param mask_str mask variable name in the caller function
 * @param val value to prepare
 * @param val_str val variable name in the caller function
 * @param caller caller name
 * @return unsigned long val masks and shifted up
 */
static inline
unsigned long __pp_field_prep(unsigned long mask, const char *mask_str,
			      unsigned long val, const char *val_str,
			      const char *caller)
{
	if (unlikely(!pp_is_mask_valid(mask))) {
		pr_err("%s: mask '%s' %#lx isn't valid\n",
		       caller, mask_str, mask);
		return 0;
	}
	if (unlikely(!pp_mask_value_valid(mask, val))) {
		pr_err("%s: value '%s' %#lx doesn't fit mask '%s' %#lx\n",
		       caller, val_str, val, mask_str, mask);
		return 0;
	}

	return (val << __ffs(mask)) & mask;
}

/**
 * @define Extract a bitfield element
 * @param mask
 * @param val MUST be 32bit
 */
#define PP_FIELD_GET(mask, val)				\
	(__builtin_constant_p(mask) ?			\
	FIELD_GET(mask, val) :				\
	(typeof(mask))pp_field_get((unsigned long)mask,	\
		(unsigned long)val))

/**
 * @define Extract a bitfield element
 * @param mask
 * @param val
 */
#define PP_FIELD_PREP(mask, val)					\
	(__builtin_constant_p(mask) && __builtin_constant_p(val) ?	\
	FIELD_PREP(mask, val) :						\
	(typeof(mask))pp_field_prep((unsigned long)mask,		\
		(unsigned long)val))

/**
 * @define Prepare a bitfield element in and or it another value
 * @param mask shifted mask defining the field's length and position,
 *             use BIT(n) or GENMASK(h, l) <br>
 *             Valid mask 0x7f, 0x3, 0x4, 0xfc, 0x6 <br>
 *             Non-valid mask: 0x5, 0xA, 0x34
 * @param val bitfield element value
 * @param dest the other value
 */
#define PP_FIELD_MOD(mask, val, dest)                 \
	(PP_FIELD_PREP(mask, val) | (dest & (~mask)))

/**
 * @brief Check if port is valid and print error message in
 *        case not
 * @param port PP port id
 * @return bool true in case the port is valid, false otherwise
 */
static inline bool __pp_is_port_valid(u16 port)
{
	if (likely(PP_IS_PORT_VALID(port)))
		return true;

	pr_err("Invalid port %hu, valid values: 0 - %u\n",
	       port, PP_MAX_PORT - 1);
	return false;
}

/**
 * @define Shortcut macro to check if color is valid
 * @param color the color
 */
#define PP_IS_COLOR_VALID(color) \
	(PP_COLOR_INVALID < (color) && (color) < PP_COLOR_MAX)

/**
 * @define Shortcut to convert color to string
 */
#define PP_COLOR_TO_STR(color)   \
	((color == PP_COLOR_GREEN)  ? "Green"  : \
	((color == PP_COLOR_YELLOW) ? "Yellow" : \
	((color == PP_COLOR_RED)    ? "Red"    : "Invalid")))

/**
 * @brief Check if color is valid and print error message in
 *        case not
 * @param color color id
 * @return bool true in case the color is valid, false otherwise
 */
static inline bool __pp_is_color_valid(u8 color)
{
	if (likely(PP_IS_COLOR_VALID(color)))
		return true;

	pr_err("Invalid color %hhu, valid values: %u - %u\n",
	       color, PP_COLOR_GREEN, PP_COLOR_MAX - 1);
	return false;
}

/**
 * @define Shortcut macro to check if port number is a valid RPB
 *         port number.
 * @param port RPB port number
 */
#define PP_IS_RPB_PORT_VALID(port) \
	(0 <= (port) && (port) < PP_MAX_RPB_PORT)

/**
 * @define Shortcut macro to check if traffic class (tc) number
 *         is a valid PP tc number
 * @param tc TC number
 */
#define PP_IS_TC_VALID(tc) \
	(0 <= (tc) && (tc) < PP_MAX_TC)

/**
 * @define Build 64 bit word from 32bit-low and 32bit-high
 */
#define PP_BUILD_64BIT_WORD(_l, _h) ((u64)(((u64)(_h) << BITS_PER_U32) | (_l)))

/**
 * @define Shortcut macro to check if pp nf type is valid
 */
#define PP_IS_NF_VALID(nf) \
	(0 <= (nf) && (nf) < PP_NF_NUM)

/**
 * @brief Shortcut for checking if pointer is null
 */
#define ptr_is_null(ptr)        __ptr_is_null(ptr, #ptr, __func__)

/**
 * @brief Shortcut for checking if pointer is null and print error
 *        message in case he is which includes the parameter name and
 *        the caller function name
 * @param ptr pointer to check
 * @param ptr_name pointer variable name for error print
 * @return bool true if the pointer is null, false otherwise
 */
static inline bool __ptr_is_null(const void *ptr, const char *name,
				 const char *caller)
{
	if (likely(ptr))
		return false;

	pr_err("%s: '%s' is null\n", caller, name);
	return true;
}

/**
 * @define Packet Processor Descriptor cookie name to register
 * to skb extension module
 */
#define PP_DESC_COOKIE_NAME     "pp-desc-cookie"

/**
 * @define Shortcut macro for writing formatted string into
 *         buffer
 * @param buf buffer pointer
 * @param len buffer length including trailing '\0'
 * @param n number of characters written to the 'buf' excluding
 *          trailing '\0'
 * @param fmt format string
 */
#define pr_buf(buf, len, n, fmt, ...) \
	do {                                                                   \
		n = snprintf(buf, len, fmt, ##__VA_ARGS__);                    \
		n = min_t(int, n, len);                                        \
	} while (0)

/**
 * @define Shortcut macro for appending formatted string into a
 *         buffer in subsequent calls,
 * @note first write to the buffer should be with pr_buff
 * @param buf buffer pointer
 * @param len buffer length including trailing '\0'
 * @param n characters written to 'buf' counter excluding trailing
 *          '\0'
 */
#define pr_buf_cat(buf, len, n, fmt, ...)                                      \
	do {                                                                   \
		n += snprintf((buf) + (n), (len) - (n), fmt, ##__VA_ARGS__);   \
		n = min_t(int, n, len);                                        \
	} while (0)

/**
 * @brief Find array's next entry which meet a specified condition
 * @param last array's last entry
 * @param curr current entry
 * @param sz entry size in bytes
 * @param cond condition to meet, use NULL to iterate all entries
 * @return void* the next entry
 */
static inline const void *find_next_arr_entry(const void *arr, u32 n,
					      const void *curr, size_t sz,
					      bool (*cond)(const void *))
{
	const void *last = arr + (sz * (n - 1));

	/* while loop to iterate the entries till we found one that meet
	 * the condition or we pass the last entry
	 */
	while (curr <= last && cond && !cond(curr))
		curr += sz; /* advance to next one entry */

	return curr;
}

/**
 * @brief Find array's first entry that meet the specified condition
 * @param arr the array
 * @param n array's number of entries
 * @param sz entry size in bytes
 * @param cond condition to meet, NULL to iterate all entries
 * @return void* first entry
 */
#define find_first_arr_entry(arr, n, sz, cond)                                 \
	find_next_arr_entry(arr, n, arr, sz, cond)

/**
 * @brief Shortcut for iterating through array entries which meet
 * @param it iterator, a pointer of same type as the array elements
 *           which is used to iterate over the elements
 * @param arr the array
 * @param n number of entries in the array
 * @param cond condition to meet, use NULL to iterate all entries
 */
#define for_each_arr_entry_cond(it, arr, n, cond)                              \
	for (it = (typeof(it))find_first_arr_entry(arr, n, sizeof(*it), cond); \
	     it < (typeof(it))(arr) + n;                                       \
	     it = (typeof(it))find_next_arr_entry(arr, n, it + 1, sizeof(*it), \
						  cond))

/**
 * @brief Shortcut for iterating through array entries
 * @param it iterator, a pointer of same type as the array entries
 *        which is used to iterate over the entries
 * @param arr the array
 * @param n number of entries in the array
 */
#define for_each_arr_entry(it, arr, n, ...)                                    \
	for (it = (typeof(it))(arr); (it) < (typeof(it))(arr) + (n);           \
	     it++, ##__VA_ARGS__)

/**
 * @brief Calculate entry index inside an array
 * @arr the array
 * @entry the entry pointer
 */
#define arr_entry_idx(arr, entry) ((u32)((entry) - (typeof(it))arr))

/**
 * @brief Shortcut for iterating over structure members
 * @note all members MUST be from same type, otherwise it won't work
 * @param _struct the structure to iterate
 * @param _it iterator, pointer from same type of the members
 */
#define for_each_struct_mem(_struct, _it, ...)                                 \
	for (_it = (typeof(_it))(_struct);                                     \
	     (u8 *)_it < (u8 *)(_struct) + sizeof(*_struct);                   \
	     _it++, ##__VA_ARGS__)

/**
 * @brief Test whether PP stats are non zero
 * @param stats the stats
 * @return bool true if stats aren't zero, false otherwise
 */
static inline bool pp_stats_is_non_zero(const void *stats)
{
	const struct pp_stats *st = stats;

	return st->packets || st->bytes;
}

static inline void __u32_diff(u32 *a, u32 *b, u32 *res)
{
	*res = *b - *a;
}

static inline void __u64_diff(u64 *a, u64 *b, u64 *res)
{
	*res = *b - *a;
}

static inline void __atomic_diff(atomic_t *a, atomic_t *b, atomic_t *res)
{
	atomic_set(res, atomic_read(b) - atomic_read(a));
}

#define __STRUCT_DIFF(subtrahend, subtracted, res, type, func)                 \
	do {                                                                   \
		type *__a, *__b, *__res;                                       \
									       \
		__b   = (type *)subtracted;                                    \
		__res = (type *)res;                                           \
		for_each_struct_mem(subtrahend, __a, __b++, __res++)           \
			func(__a, __b, __res);                                 \
	} while (0)

#define U32_STRUCT_DIFF(subtrahend, subtracted, res)                           \
	__STRUCT_DIFF(subtrahend, subtracted, res, u32, __u32_diff)

#define U64_STRUCT_DIFF(subtrahend, subtracted, res)                           \
	__STRUCT_DIFF(subtrahend, subtracted, res, u64, __u64_diff)

#define ATOMIC_STRUCT_DIFF(subtrahend, subtracted, res)                        \
	__STRUCT_DIFF(subtrahend, subtracted, res, atomic_t, __atomic_diff)

/**
 * @brief Get a reference to PP device
 * @return struct device* PP device, MUST check for NULL pointer
 */
struct device *pp_dev_get(void);

/**
 * @brief Check if PP driver is ready for other to start calling
 *        the driver's exported API
 * @note this MUST be used by every API which is exported outside the PP
 *       to verify the PP initialize done successfully
 * @return bool true if PP is ready, false otherwise
 */
bool __pp_is_ready(const char *caller);
#define pp_is_ready()           __pp_is_ready(__func__)

typedef bool (*buf_fld_is_valid)(s64);
typedef s64  (*buf_fld_to_hw_gran)(s64);
typedef s64  (*buf_fld_from_hw_gran)(s64);
typedef u32  (*buf_fld_bit_get)(const u32 *);

/**
 * @struct Buffer field definition defining how to encode and
 *         decode a field to and from a 32 words buffer
 * @note this mechanism support fields up to 64 bits
 * @id field id
 * @name field name
 * @desc field description
 * @word_idx buffer word index where the field should be encoded to
 *           and decoded from
 * @mask mask to use to encode and decode the field
 * @is_valid callback to test whether a given value is valid for the
 *           field, if not specified an alternative 'is_valid' will be
 *           used to make sure at least that the value fits the field
 *           mask
 * @to_hw callback to convert a value to the field granularity, this
 *        is OPTIONAL
 * @from_hw callback to do the opposite from 'to_hw', this is OPTIONAL
 * @base_bit_get callback to calculate the base bit offset, this base
 *               together with the msb and lsb will help to find
 *               the real msb and lsb
 */
struct buf_fld_info {
	char name[64];
	char desc[64];
	u32  id;
	u8   word_idx;
	u8   last_word_idx;
	u32  msb;
	u32  lsb;
	buf_fld_is_valid     is_valid;
	buf_fld_to_hw_gran   to_hw;
	buf_fld_from_hw_gran from_hw;
	buf_fld_bit_get      base_bit_get;
};

/**
 * @define Shortcut macro to initialize field info
 * @param flds fields array of type 'struct fld_info'
 * @param id field id
 * @note the field MUST have word index and mask defined as follow:
 *       if the field enum is X, word definition should be X_WORD
 *       and the mask definition should be X_MSK
 */
#define BUF_FLD_INIT(flds, id, desc, is_valid, to, from, base_bit_get)         \
	__buf_fld_init(&(flds)[id], id, #id, desc, id##_MSB, id##_LSB,         \
		       is_valid, to, from, base_bit_get)

#define BUF_FLD_PREFIX(flds, id, desc, prefix, is_valid, to, from,             \
		       base_bit_get)                                           \
	__buf_fld_init(&(flds)[id], id, #id, desc, prefix##id##_MSB,           \
		       prefix##id##_LSB, is_valid, to, from, base_bit_get)

/**
 * @define Even Shorter macro to initialize simple SI field info
 *         that doesn't need any granularity conversions or
 *         special validity checks
 */
#define BUF_FLD_INIT_SIMPLE(flds, id)                                          \
	BUF_FLD_INIT(flds, id, NULL, NULL, NULL, NULL, NULL)

/**
 * @define Same as BUF_FLD_INIT_SIMPLE but also needs a prefix
 *         before enum
 */
#define BUF_FLD_INIT_PREFIX(flds, id, prefix)                                  \
	BUF_FLD_PREFIX(flds, id, NULL, prefix, NULL, NULL, NULL, NULL)

/**
 * @define Same as BUF_FLD_INIT_SIMPLE but also needs X_DESC to be
 *         defined to be used as the description of the field instead
 *         of the enum name
 */
#define BUF_FLD_INIT_DESC(flds, id)                                            \
	BUF_FLD_INIT(flds, id, id##_DESC, NULL, NULL, NULL, NULL)

/**
 * @define Shortcut macro to initialize field info that only
 *         needs special validity check
 */
#define BUF_FLD_INIT_VALID(flds, id, is_valid)                                 \
	BUF_FLD_INIT(flds, id, NULL, is_valid, NULL, NULL, NULL)

/**
 * @define Shortcut macro to initialize dynamic field info
 *         Dynamic field is a field that we his word index can
 *         be changed and is based on other field
 * @param flds fields array of type 'struct fld_info'
 * @param id field id
 * @note the field and mask defined as follow: if the field enum is X,
 *       the mask definition should be X_MSK
 */
#define BUF_DFLD_INIT(flds, id, desc, is_valid, to, from, base_bit_get)        \
	__buf_fld_init(&(flds)[id], id, #id, desc, id##_MSB, id##_LSB,         \
		       is_valid, to, from, base_bit_get)

/**
 * @define Shortcut macro to initialize simple dynamic field
 *         info which doesn't require anything other than
 *         word_get callback
 */
#define BUF_DFLD_INIT_SIMPLE(flds, id, base_bit_get) \
	BUF_DFLD_INIT(flds, id, NULL, NULL, NULL, NULL, base_bit_get)

/**
 * @brief Helper function to initialize field info
 * @note see 'struct buf_fld_info' for params description
 */
static inline s32 __buf_fld_init(struct buf_fld_info *fld, u32 id,
				 const char *name, const char *desc, u32 msb,
				 u32 lsb, buf_fld_is_valid is_valid,
				 buf_fld_to_hw_gran to_hw,
				 buf_fld_from_hw_gran from_hw,
				 buf_fld_bit_get base_bit_get)
{
	s32 ret;

	if (!fld) {
		pr_err("null pointer\n");
		return -EINVAL;
	}

	if ((msb - lsb) > BITS_PER_LONG_LONG) {
		pr_err("%s: Cannot support fields larger than 64 bits\n", name);
		return -EINVAL;
	}

	fld->id            = id;
	fld->is_valid      = is_valid;
	fld->to_hw         = to_hw;
	fld->from_hw       = from_hw;
	fld->base_bit_get  = base_bit_get;
	fld->word_idx      = lsb / BITS_PER_U32;
	fld->last_word_idx = msb / BITS_PER_U32;
	fld->lsb           = lsb;
	fld->msb           = msb;

	if (name)
		ret = strscpy(fld->name, name, sizeof(fld->name));
	if (desc)
		ret = strscpy(fld->desc, desc, sizeof(fld->desc));
	else if (name)
		ret = strscpy(fld->desc, name, sizeof(fld->desc));

	pr_debug("%s: desc '%s', msb %u, lsb %u, word_idx %u, last_word_idx %u\n",
		 name, desc, fld->msb, fld->lsb, fld->word_idx,
		 fld->last_word_idx);

	return 0;
}

/**
 * @brief Get a field from a buf, helper function to extract a field
 *        from a specific word in the provided buffer
 * @note assume buffer has sufficent amount of words to access using
 *       the provided field info
 * @param fld the field info required to extract the field from the
 *            buffer
 * @param buf the buffer
 * @return s64 the field value
 */
static inline s64 buf_fld_get(struct buf_fld_info *fld, const u32 *buf)
{
	s64 val, hw_val = 0;
	u32 msb, lsb, mask, shift = 0;
	u8  word_idx;

	if (unlikely(ptr_is_null(fld) || ptr_is_null(buf)))
		return S64_MAX;

	if (fld->base_bit_get) {
		lsb                = fld->base_bit_get(buf) + fld->lsb;
		msb                = fld->base_bit_get(buf) + fld->msb;
		word_idx           = lsb / BITS_PER_U32;
		fld->last_word_idx = msb / BITS_PER_U32;
	} else {
		lsb      = fld->lsb;
		msb      = fld->msb;
		word_idx = fld->word_idx;
	}

	while (word_idx <= fld->last_word_idx) {
		if (word_idx < fld->last_word_idx)
			mask = GENMASK(BITS_PER_U32 - 1, lsb % BITS_PER_U32);
		else
			mask = GENMASK((msb % BITS_PER_U32),
				       lsb % BITS_PER_U32);
		val     = PP_FIELD_GET(mask, buf[word_idx]);
		hw_val |= val << shift;
		word_idx++;
		val = hweight32(mask);
		lsb   += val;
		shift += val;
	}

	/* back to normal */
	return fld->from_hw ? fld->from_hw(hw_val) : hw_val;
}

/**
 * @brief Set a field into a buf, helper function to encode a field
 *        into a specific word in the provided buffer
 * @note assume buffer has sufficent amount of words to access using
 *       the provided field info
 * @param fld the field info required to encode the field into the
 *            buffer
 * @param buf the buffer
 * @return s64 0 on success, error code otherwise
 */
static inline s32 buf_fld_set(struct buf_fld_info *fld, u32 *buf, s64 val)
{
	s64 tmp, hw_val;
	u32 msb, lsb, mask, hweight, shift = 0;
	u8  word_idx;

	if (unlikely(ptr_is_null(fld) || ptr_is_null(buf)))
		return -EINVAL;

	if (fld->base_bit_get) {
		lsb                = fld->base_bit_get(buf) + fld->lsb;
		msb                = fld->base_bit_get(buf) + fld->msb;
		word_idx           = lsb / BITS_PER_U32;
		fld->last_word_idx = msb / BITS_PER_U32;
	} else {
		lsb      = fld->lsb;
		msb      = fld->msb;
		word_idx = fld->word_idx;
	}

	/* value validity check */
	if (fld->is_valid && !fld->is_valid(val))
		goto invalid;

	/* convert val if needed */
	hw_val = fld->to_hw ? fld->to_hw(val) : val;

	while (word_idx <= fld->last_word_idx) {
		if (word_idx < fld->last_word_idx)
			mask = GENMASK(BITS_PER_U32 - 1, lsb % BITS_PER_U32);
		else
			mask = GENMASK((msb % BITS_PER_U32),
				       lsb % BITS_PER_U32);
		/* extract hweight32(mask) first bits from the value */
		hweight       = hweight32(mask);
		tmp           = PP_FIELD_GET(GENMASK(hweight - 1, 0), hw_val);
		/* write the value to the buffer */
		buf[word_idx] = PP_FIELD_MOD(mask, tmp, buf[word_idx]);

		pr_debug("%s(%u): lsb %u, msb %u, word%u %#x, mask %#x, tmp %#llx, hw_val %#llx\n",
			 fld->name, fld->id, lsb, msb, word_idx,
			 buf[word_idx], mask, tmp, hw_val);

		word_idx++;
		lsb   += hweight;
		shift += hweight;
		hw_val = hw_val >> hweight;
	}

	return 0;

invalid:
	pr_err("Invalid %s(%u) field value %llu, %#llx, is_valid %ps\n",
	       fld->name, fld->id, val, val, fld->is_valid);
	return -EINVAL;
}

static inline u64 buf_fld_max_val_get(struct buf_fld_info *fld)
{
	ulong mask = GENMASK(fld->msb, fld->lsb);

	return MASK2MAX(mask);
}

static inline int pp_devm_kvmalloc_match(struct device *dev, void *res,
				     void *match_data)
{
	return *((ulong *)res) == (ulong)match_data;
}

static inline void pp_devm_kvmalloc_release(struct device *dev, void *res)
{
	void *ptr = (void *)*((ulong *)res);

#if KERNEL_VERSION(4, 12, 0) > LINUX_VERSION_CODE
	vfree(ptr);
#else
	kvfree(ptr);
#endif
}

/**
 * @brief Resource-managed kvfree
 * @param dev Device this memory belongs to
 * @param ptr Memory to free
 *
 * Free memory allocated with pp_devm_kvmalloc().
 */
static inline void pp_devm_kvfree(struct device *dev, void *ptr)
{
	s32 rc;

	rc = devres_destroy(dev, pp_devm_kvmalloc_release,
			    pp_devm_kvmalloc_match, ptr);
	WARN_ON(rc);
}

/**
 * @brief Resource-managed kvmalloc
 * @param dev Device to allocate memory for
 * @param sz Allocation size
 * @param flags Allocation gfp flags
 *
 * Managed kvmalloc. Memory allocated with this function is
 * automatically freed on driver detach.  Like all other devres
 * resources, guaranteed alignment is unsigned long long.
 * Allocated memory may not be physically contiguos, do not use
 * for HW.
 *
 * @return Pointer to allocated memory on success, NULL on failure.
 */
static inline void *pp_devm_kvmalloc(struct device *dev, size_t sz, gfp_t flags)
{
	ulong *res;
	void  *ptr;

	res = devres_alloc(pp_devm_kvmalloc_release, sizeof(res), flags);
	if (!res)
		return NULL;

#if KERNEL_VERSION(4, 12, 0) > LINUX_VERSION_CODE
	if (flags & __GFP_ZERO)
		ptr = vzalloc(sz);
	else
		ptr = vmalloc(sz);
#else
	ptr = kvmalloc(sz, flags);
#endif
	if (!ptr) {
		devres_free(res);
		return NULL;
	}

	*res = (ulong)ptr;
	devres_add(dev, res);
	return ptr;
}

static inline void *pp_devm_kvzalloc(struct device *dev, size_t sz, gfp_t flags)
{
	return pp_devm_kvmalloc(dev, sz, flags | __GFP_ZERO);
}

static inline void *pp_devm_kvcalloc(struct device *dev, size_t n, size_t sz,
				     gfp_t flags)
{
	size_t size;

#if KERNEL_VERSION(4, 18, 0) > LINUX_VERSION_CODE
	size = sz * n;
#else
	if (unlikely(check_mul_overflow(n, sz, &size)))
		return NULL;
#endif

	return pp_devm_kvmalloc(dev, size, flags | __GFP_ZERO);
}


/**
 * @brief Swap source buffer and write to dest buffer<br>
 *        dst[0] = src[src_sz - 1]<br>
 *        dst[1] = src[src_sz - 2]<br>
 *        ...
 * @return number of bytes swapped on success, negative error code
 *         otherwise
 */
static inline s32 __buf_swap(void *dst, size_t dst_sz, const void *src,
			     size_t src_sz)
{
	const char *__src;
	char       *__dst;
	size_t      count;

	if (unlikely(src == dst)) {
		pr_err("cannot swap on same buffer\n");
		return -EINVAL;
	}

	pr_debug("dst = %p, dst_sz = %u, src %p, src_sz = %u, caller '%ps'\n",
		 dst, (u32)dst_sz, src, (u32)src_sz,
		 __builtin_return_address(0));

	count = min(dst_sz, src_sz);
	__src = (const char *) src + src_sz - 1; /* set src to the last byte */
	__dst = (char *) dst;

	while (count--)
		*__dst++ = *__src--;

	return (s32)min(dst_sz, src_sz);
}

static inline s32 str_tolower(char *str)
{
	if (unlikely(ptr_is_null(str)))
		return -EINVAL;

	for (; *str; str++)
		*str = tolower(*str);

	return 0;
}

/**
 * @define Test whether PP event is valid
 * @param ev the event to check
 */
#define PP_IS_EVENT_VALID(ev) \
	(ev >= 0 && ev < PP_EVENTS_NUM)

/**
 * @brief Test whether PP event is valid, print an error message in
 *        case it is not valid
 * @param ev the event
 * @return bool true in case the event is valid, false otherwise
 */
static inline bool __pp_is_event_valid(enum pp_event ev)
{
	if (likely(PP_IS_EVENT_VALID(ev)))
		return true;

	pr_err("Invalid PP event id %u\n", ev);
	return false;
}

/**
 * @brief Generic definition for get stats function to use with
 *        pp_stats_show and pp_pps_show
 * @param stats array of stats, this is used to return the stats
 *              to the user
 * @param num_stats number of stats entries in the array
 * @param data user defined data, this is the data which is
 *             provided to pp_stats_show and pp_pps_show
 */
typedef s32 (*__stats_get)(void *stats, u32 num_stats, void *data);

/**
 * @brief Generic definition for a function to substract 2 arrays
 *        of stats and save the result in another one, to use
 *        with pp_stats_show and pp_pps_show
 *        'post' - 'pre' = 'delta'
 * @param pre the subtrahend
 * @param num_pre subtrahend size
 * @param post the subtracted
 * @param num_pre subtracted size
 * @param post the result
 * @param num_pre result size
 * @param data user defined data, this is the data which is
 *             provided to pp_stats_show and pp_pps_show
 */
typedef s32 (*__stats_diff)(void *pre, u32 num_pre, void *post, u32 num_post,
			    void *delta, u32 num_delta, void *data);

/**
 * @brief Generic definition for printing stats into a buffer function
 *                to use with pp_stats_show and pp_pps_show
 * @param buf the buffer to print to
 * @param sz the buffer size
 * @param n pointer to return number of bytes written
 * @param stats array of stats, this is used to return the stats
 *              to the user
 * @param num_stats number of stats entries in the array
 * @param data user defined data, this is the data which is
 *             provided to pp_stats_show and pp_pps_show
 */
typedef s32 (*__stats_show)(char *buf, size_t sz, size_t *n, void *stats,
			    u32 num_stats, void *data);

/**
 * @brief Helper for printing stats fetched by 'get'
 *        into a function using 'show'
 * @param stats_sz stats array entry size
 * @param num_stats stats array size
 * @param get get stats function pointer
 * @param show show stats function pointer
 * @param data user specific data which will be provided to all functions
 * @param buf buffer to print the stats to
 * @param sz buffer sz
 * @param n pointer to update the number of bytes written
 * @return s32 0 on success, error code otherwise
 */
static inline s32 pp_stats_show(size_t stats_sz, u32 num_stats, __stats_get get,
				__stats_show show, void *data, char *buf,
				size_t sz, size_t *n)
{
	void *stats = NULL;
	s32 ret = 0;

	stats = kcalloc(num_stats, stats_sz, GFP_KERNEL);
	if (unlikely(ptr_is_null(stats))) {
		ret = -ENOMEM;
		goto done;
	}

	ret = get(stats, num_stats, data);
	if (unlikely(ret))
		goto done;

	ret = show(buf, sz, n, stats, num_stats, data);

done:
	kfree(stats);
	return ret;
}

/**
 * @brief Helper for printing stats pps into buffer
 *        Measures the stats for 1 second use the provided
 *        'get' function and print it to 'buf'
 *        'get', 'diff', and 'show' should get an array of
 *        stats to work on
 * @param stats_sz stats array entry size
 * @param num_stats stats array size
 * @param get get stats function pointer
 * @param diff diff stats function pointer
 * @param show show stats function pointer
 * @param data user specific data which will be provided to all functions
 * @param buf buffer to print the stats to
 * @param sz buffer sz
 * @param n pointer to update the number of bytes written
 * @return s32 0 on success, error code otherwise
 */
static inline s32 pp_pps_show(size_t stats_sz, u32 num_stats, __stats_get get,
			      __stats_diff diff, __stats_show show, void *data,
			      char *buf, size_t sz, size_t *n)
{
	void *post = NULL;
	void *pre  = NULL;
	u64 start, end;
	u32 time;
	s32 ret = 0;

	pre = kcalloc(num_stats, stats_sz, GFP_KERNEL);
	if (unlikely(ptr_is_null(pre))) {
		ret = -ENOMEM;
		goto done;
	}

	post = kcalloc(num_stats, stats_sz, GFP_KERNEL);
	if (unlikely(ptr_is_null(post))) {
		ret = -ENOMEM;
		goto done;
	}

	start = get_jiffies_64();
	ret = get(pre, num_stats, data);
	if (unlikely(ret))
		goto done;
	end = get_jiffies_64();

	time = jiffies_to_msecs(end - start);
	if (time < 1000)
		mdelay(1000 - time);
	else
		pr_warn("Reading counters take too long\n");

	ret = get(post, num_stats, data);
	if (unlikely(ret))
		goto done;

	ret = diff(pre, num_stats, post, num_stats, post, num_stats, data);
	if (unlikely(ret))
		goto done;

	ret = show(buf, sz, n, post, num_stats, data);

done:
	kfree(pre);
	kfree(post);
	return ret;
}

/**
 * @brief pp_stats_show version for seq_file
 */
#define pp_stats_show_seq(f, ssz, num, get, show, data)                        \
	pp_stats_show(ssz, num, get, show, data, (f)->buf, (f)->size,          \
		      &(f)->count)

/**
 * @brief pp_pps_show version for seq_file
 */
#define pp_pps_show_seq(f, ssz, num, get, diff, show, data)                    \
	pp_pps_show(ssz, num, get, diff, show, data, (f)->buf, (f)->size,      \
		    &(f)->count)

/**
 * @brief Allocate contiguos physical memory
 * @param size memory size
 * @param flag the gfp_t flags for allocation __GFP_ZERO is added by default
 * @param dma_handle da handle (physical address)
 * @param ioc if the memory is ioc or nioc
 * @note ioc is relevant only for B-step as in A-step its nioc by default)
 */
void *pp_dma_alloc(size_t size, gfp_t flag, dma_addr_t *dma_handle, bool ioc);

/**
 * @brief Free contiguos physical memory
 */
void pp_dma_free(size_t size, void *address, dma_addr_t *dma_handle, bool ioc);

/**
 * @brief Cache writeback
 */
static inline void pp_cache_writeback(void *addr, size_t size)
{
	if (!IS_ENABLED(CONFIG_PPV4_LGM))
		dma_map_single(pp_dev_get(), addr, size, DMA_TO_DEVICE);
}

/**
 * @brief Cache invalidate
 */
static inline void pp_cache_invalidate(volatile void *addr, size_t size)
{
	if (!IS_ENABLED(CONFIG_PPV4_LGM))
		dma_map_single(pp_dev_get(), (void *)addr, size,
			       DMA_FROM_DEVICE);
}

#if IS_ENABLED(CONFIG_DEBUG_FS)
/**
 * @brief PPv4 FPGA init sequence debug init
 * @param void
 * @return s32 0 on success, non-zero value otherwise
 */
s32 fpga_ppv4_dbg_init(struct dentry *parent);

/**
 * @brief PPv4 FPGA init sequence debug cleanup
 * @param void
 * @return s32 0 on success, non-zero value otherwise
 */
s32 fpga_ppv4_dbg_clean(void);
#else /* !CONFIG_DEBUG_FS */
static inline s32 fpga_ppv4_dbg_init(struct dentry *parent)
{
	return 0;
}

static inline s32 fpga_ppv4_dbg_clean(void)
{
	return 0;
}
#endif /* CONFIG_DEBUG_FS */
#endif /* __PP_COMMON_H_ */
