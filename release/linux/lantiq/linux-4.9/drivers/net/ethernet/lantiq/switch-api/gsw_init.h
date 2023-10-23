/******************************************************************************
                Copyright (c) 2016, 2017 Intel Corporation

******************************************************************************/
/*****************************************************************************
                Copyright (c) 2012, 2014, 2015
                    Lantiq Deutschland GmbH
    For licensing information, see the file 'LICENSE' in the root folder of
    this software module.
******************************************************************************/

#ifndef _ETHSW_INIT_H_
#define _ETHSW_INIT_H_

#define SWAPI_DRV_VERSION "3.0.1"

/* Switch Features  */
#define CONFIG_LTQ_STP		1
#define CONFIG_LTQ_8021X	1
#define CONFIG_LTQ_MULTICAST 1
#define CONFIG_LTQ_QOS		1
#define CONFIG_LTQ_VLAN		1
#define CONFIG_LTQ_WOL		1
#define CONFIG_LTQ_PMAC		1
#define CONFIG_LTQ_RMON		1

#define DEBUG_AX3000_F24S		0

#define CONFIG_MAC  1

/* User configuration options */

#define SMDIO_INTERFACE 0
#define GSW_IOCTL_SUPPORT 1
#if defined(WIN_PC_MODE) && WIN_PC_MODE
#define CONFIG_SOC_GRX500 0
#define KSEG1 0
#endif

#define ENABLE_MICROCODE 0
#define PTR_TO_INT_CAST void *

#ifdef __KERNEL__
#else
#define printk	printf
#define pr_err	printf
#define pr_info printf
#endif



#ifdef __KERNEL__
//#include <linux/module.h>
#include <linux/fs.h>
#include <linux/version.h>
#include <linux/string.h>
#include <linux/types.h>
#include <linux/interrupt.h>
//#include <asm/delay.h> */
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/of_mdio.h>

#ifndef CONFIG_X86_INTEL_CE2700
#include <net/switch_api/lantiq_gsw_api.h>
#else
#include <net/switch_api/lantiq_gsw_api.h>
#endif /* CONFIG_X86_INTEL_CE2700 */

#include <net/switch_api/gsw_dev.h>
#include <linux/netdevice.h>
#include <net/switch_api/gsw_tbl_rw.h>


#define LTQ_GSW_DEV_MAX 3
#endif /* KERNEL_MODE */

#if defined(WIN_PC_MODE) && WIN_PC_MODE
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <malloc.h>
#include <conio.h>
#include <signal.h>
#include <tchar.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <stdint.h>
//#include <sys/ioctl.h>
#endif

/* IOCTL handling incase the OS is not supporetd */
#if defined(WIN_PC_MODE) && WIN_PC_MODE
#define LTQ_GSW_DEV_MAX			1
#define NRBITS	8
#define TYPEBITS	8
#define SIZEBITS	13
#define DIRBITS	3
#define NRMASK	((1 << NRBITS)-1)
#define TYPEMASK	((1 << TYPEBITS)-1)
#define SIZEMASK	((1 << SIZEBITS)-1)
#define XSIZEMASK	((1 << (SIZEBITS+1))-1)
#define DIRMASK	((1 << DIRBITS)-1)
#define NRSHIFT     0
#define TYPESHIFT	(NRSHIFT + NRBITS)
#define SIZESHIFT	(TYPESHIFT + TYPEBITS)
#define DIRSHIFT	(SIZESHIFT + SIZEBITS)
#define NN	1U
#define RD	2U
#define WR	4U
#define IOC(dir, type, nr, size) \
	(((dir)  << DIRSHIFT) | \
	 ((type) << TYPESHIFT) | \
	 ((nr)   << NRSHIFT) | \
	 ((size) << SIZESHIFT))
#define IO(type, nr)	IOC(NN, (type), (nr), 0)
#define IOR(type, nr, size)	IOC(RD, (type), (nr), sizeof(size))
#define IOW(type, nr, size)	IOC(WR, (type), (nr), sizeof(size))
#define IOWR(type, nr, size)	IOC(RD | WR, (type), (nr), sizeof(size))
#define IO_TYPE(nr)	(((nr) >> TYPESHIFT) & TYPEMASK)
#define NR(nr)	(((nr) >> NRSHIFT) & NRMASK)
#define SIZE(nr)   \
	((((((nr) >> DIRSHIFT) & DIRMASK) & (WR|RD)) == 0) ? \
	 0 : (((nr) >> SIZESHIFT) & XSIZEMASK))
#define _IO	IO
#define _IOW IOWR
#define _IOR IOR
#define _IOWR IOWR
#define _IOC_TYPE IO_TYPE
#define _IOC_NR NR
#define _IOC_SIZE	SIZE
/* GSW Specific Include files */
#include <lantiq_gsw_api.h>
#include <gsw_tbl_rw.h>

#endif /* WIN_PC_MODE */


/*include*/
#include <gsw_ioctl_wrapper.h>
#include <gsw_flow_core.h>
#include <gsw_ll_func.h>
#include <gsw_reg.h>
#include <gsw_reg_top.h>
#include <gsw30_reg_top.h>



#if defined(CONFIG_SOC_GRX500) && CONFIG_SOC_GRX500
#include <gsw_pae.h>
#endif

#define GSWIP_GET_BITS(x, msb, lsb)	\
	(((x) & ((1 << ((msb) + 1)) - 1)) >> (lsb))
#define GSWIP_SET_BITS(x, msb, lsb, value)	\
	(((x) & ~(((1 << ((msb) + 1)) - 1) ^ ((1 << (lsb)) - 1)))	\
	 | (((value) & ((1 << (1 + (msb) - (lsb))) - 1)) << (lsb)))

#define GSW_API_MODULE_NAME "GSW SWITCH API"
#define GSW_API_DRV_VERSION "3.0.2"
#define MICRO_CODE_VERSION "212"


typedef struct {
	void *ecdev;
	void *pdev;
	gsw_devtype_t sdev;
	void *gsw_base_addr;
} ethsw_core_init_t;

void gsw_r32(void *cdev, short offset, short shift, short size, u32 *value);
void gsw_w32(void *cdev, short offset, short shift, short size, u32 value);
void *ethsw_api_core_init(ethsw_core_init_t *pinit);
void gsw_corecleanup(void);
int	gsw_pmicro_code_init(void *cdev);
int	gsw_pmicro_code_init_f24s(void *cdev);
int gsw_swapi_init(void);
int gsw_global_pcerule_bitmap_alloc(void *cdev);

#if defined(WIN_PC_MODE) && WIN_PC_MODE
struct core_ops *gsw_get_swcore_ops(u32 devid);
#endif


#if defined(UART_INTERFACE) && UART_INTERFACE
int pc_uart_dataread(u32 Offset, u32 *value);
int pc_uart_datawrite(u32 Offset, u32 value);
int pc_uart_dataread_32(u32 Offset, u32 *value);
int pc_uart_datawrite_32(u32 Offset, u32 value);
#endif /* UART_INTERFACE */

#ifdef __KERNEL__
void gsw_r32_raw(void *cdev, short offset, u32 *value);
void gsw_w32_raw(void *cdev, short offset, u32 value);
void gsw_ext_r32(void *cdev, short offset, short shift, short size, u32 *value);
void gsw_ext_w32(void *cdev, short offset, short shift, short size, u32 value);
int gsw_mdio_alloc(struct platform_device *pdev);
void grx550_misc_config(ethsw_api_dev_t *gswdev);
#endif

static inline u32 gsw_field_r32(u32 rval, short shift, short size)
{
	return (rval >> shift) & ((1 << size) - 1);
}

static inline u32 gsw_field_w32(u32 rval, short shift, short size, u32 val)
{
	u32 mask;

	mask = ((1 << size) - 1) << shift;
	val = (val << shift)&mask;
	return (rval & ~mask) | val;
}

#if defined(WIN_PC_MODE) && WIN_PC_MODE
static inline void gsw_r32_raw(void *cdev, short offset, u32 *value)
{
	ethsw_api_dev_t *pethdev = (ethsw_api_dev_t *)cdev;
#if defined(UART_INTERFACE) && UART_INTERFACE
	pc_uart_dataread((uintptr_t)pethdev->gsw_base + (offset * 4), value);
#endif /* UART_INTERFACE */
}

static inline void gsw_w32_raw(void *cdev, short offset, u32 value)
{
	ethsw_api_dev_t *pethdev = (ethsw_api_dev_t *)cdev;
#if defined(UART_INTERFACE) && UART_INTERFACE
	pc_uart_datawrite((uintptr_t)pethdev->gsw_base + (offset * 4), value);
#endif
}
#endif

static inline ethsw_api_dev_t *GSW_PDATA_GET(void *pdev)
{
	ethsw_api_dev_t *pdata = NULL;

	if (pdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return NULL;
	}

#ifdef __KERNEL__
	pdata = container_of((struct core_ops *)pdev, ethsw_api_dev_t, ops);
#else
	pdata = (ethsw_api_dev_t *)pdev;
#endif
	return pdata;
}

#if defined(WIN_PC_MODE) && WIN_PC_MODE
struct core_ops *gsw_get_swcore_ops(u32 devid);
#endif

typedef enum {
	IRQ_REGISTER 		= 0,
	IRQ_UNREGISTER 		= 1,
	IRQ_ENABLE 			= 2,
	IRQ_DISABLE 		= 3,
} IRQ_TYPE;



#endif    /* _ETHSW_INIT_H_ */
