#ifndef _UAPI_LINUX_KERNEL_H
#define _UAPI_LINUX_KERNEL_H

#if defined(__KERNEL__) || defined( __GLIBC__)
#include <linux/sysinfo.h>
#include <linux/const.h>
#endif

/*
 * 'kernel.h' contains some often-used function prototypes etc
 */
#define __ALIGN_KERNEL(x, a)		__ALIGN_KERNEL_MASK(x, (typeof(x))(a) - 1)
#define __ALIGN_KERNEL_MASK(x, mask)	(((x) + (mask)) & ~(mask))

#define __KERNEL_DIV_ROUND_UP(n, d) (((n) + (d) - 1) / (d))

#endif /* _UAPI_LINUX_KERNEL_H */
