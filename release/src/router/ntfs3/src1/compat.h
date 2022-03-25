/* SPDX-License-Identifier: GPL-2.0 */

#include <linux/device-mapper.h>
#include <uapi/linux/msdos_fs.h>
#ifndef __bitmap_set
#define __bitmap_set(a, b, c)	bitmap_set(a, b, c)
#endif

#ifndef __bitmap_clear
#define __bitmap_clear(a, b, c)	bitmap_clear(a, b, c)
#endif

/*
 * Copy from include/linux/compiler_attributes.h
 */
#ifndef __has_attribute
#define __has_attribute(x) 0
#endif

#ifndef fallthrough
#if __has_attribute(__fallthrough__)
#define fallthrough __attribute__((__fallthrough__))
#else
#define fallthrough do {} while (0)  /* fallthrough */
#endif
#endif

/*
 * Copy from include/linux/build_bug.h
 */
#ifndef static_assert
#define static_assert(expr, ...) __static_assert(expr, ##__VA_ARGS__, #expr)
#define __static_assert(expr, msg, ...) _Static_assert(expr, msg)
#endif

/*
 * Copy from include/linux/overflow.h
 */
#ifndef struct_size
#define struct_size(p, member, n) (sizeof(*(p)) + n * sizeof(*(p)->member))
#endif

#ifndef REQ_OP_READ
#define REQ_OP_READ  READ

#endif

#ifndef REQ_OP_WRITE
#define REQ_OP_WRITE  WRITE
#endif

#ifndef PAGE_KERNEL_RO
#define PAGE_KERNEL_RO PAGE_KERNEL
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(4,9,0)
#define time64_to_tm(x,y,z)	time_to_tm(x,y,z)   
#define current_time(x)	CURRENT_TIME_SEC
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 18, 0)
typedef struct timespec64 ntfs3_timespec_t;
#else
typedef struct timespec ntfs3_timespec_t;
#endif


