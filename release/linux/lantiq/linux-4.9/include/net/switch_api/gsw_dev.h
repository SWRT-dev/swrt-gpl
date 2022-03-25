/******************************************************************************
 *                Copyright (c) 2016, 2017 Intel Corporation
 *
 *
 * For licensing information, see the file 'LICENSE' in the root folder of
 * this software module.
 *
 ******************************************************************************/

#ifndef _GSW_DEV_H
#define _GSW_DEV_H

#include <linux/types.h>
#include <linux/platform_device.h>
#include <linux/export.h>
#include <net/switch_api/adap_ops.h>
#include <net/switch_api/mac_ops.h>
#include <net/switch_api/gsw_flow_ops.h>
#include <net/switch_api/lantiq_gsw_api.h>
#include <linux/cdev.h>

#define MAX_GSWDEV		16
#define MAC_DEV_NAME		"gsw_mac"
#define CORE_DEV_NAME		"gsw_core"

#define GSW_SHORTCUT_MODE	1
#define GSW_FULLQOS_MODE	0

/* This represents the GSWIP SubSystem block, one instance per SubSystem
 * For xrx500 GSWIP-L device ID is 0
 * For xrx500 GSWIP-R device ID is 1
 * For PRX300 device ID is 0
 */
struct gswss {
	/* Product ID, GRX500, PRX300, ..... */
	u32 prod_id;
	/* Adaption layer Operations Structure */
	struct adap_ops *adap_ops;
	/* Number of devices in this GSWIP SubSystem */
	u32 num_devs;
	/* Core Platform device */
	struct platform_device *core_dev;
	/* Number of Mac Subdevice in this GSWIP SubSystem */
	u32 mac_subdevs_cnt;
	/* cdev interface for GSWIP SubSystem */
	struct cdev gswss_cdev;
	/* Mac Platform device for each Mac Subdevice */
	struct platform_device *mac_dev[];
};

/* Enumeration for Product ID
 * This is to identify the Product ID in functional drivers in GSWIP Subsystem
 */
enum {
	GRX500     = 0x01,
	PRX300     = 0x02,
};

/* This function returns the Adaptions Operations registered.
 * param[in/out]	IN:  devid   	Device ID used.
 *			OUT: adap_ops	Adaption ops Func Pointer Structure
 * return		NULL:		No Adaption operations registered
 * return		!NULL:		Returns Adaption operations registered
 */
struct adap_ops *gsw_get_adap_ops(u32 devid);

/* This function returns the GSWIP Core Operations registered.
 * param[in/out]	IN:  devid	Device ID used.
 *			OUT: core_ops	Core operations Func Pointer Structure
 * return         NULL:			No GSWIP Core operations registered
 * return         !NULL:		Returns GSWIP Core operations registered
 */
struct core_ops *gsw_get_swcore_ops(u32 devid);

/* This function returns the MAC Operations registered.
 * param[in/out]	IN:  devid	Device ID used.
 *			IN:  mac_idx   	Mac Idx used(0/1/2..).
 *					Depends on mac_subdev_cnt
 *			OUT: mac_ops	MAC operations Func Pointer Structure
 * return		NULL:		No MAC operations registered
 * return		!NULL:		Returns MAC operations registered
 */
struct mac_ops *gsw_get_mac_ops(u32 devid, u32 mac_idx);

/* This function returns the MAC Platform Device registered.
 * param[in/out]	IN:  devid	Device ID used.
 *			OUT: platform_device	Core Platform Device registered Structure
 * return		NULL:		No Core Platform Device registered
 * return		!NULL:		Returns Core Platform Device registered
 */
struct platform_device *gsw_get_coredev(u32 devid);

/* This function returns the MAC Platform Device registered.
 * param[in/out]	IN:  devid	Device ID used.
 *			IN:  mac_idx	Mac Idx used(0/1/2..).
 *			Depends on mac_subdev_cnt
 *			OUT: platform_device	MAC Platform Device registered Structure
 * return		NULL:		No MAC Platform Device registered
 * return		!NULL:		Returns MAC Platform Device registered
 */
struct platform_device *gsw_get_macdev(u32 devid, u32 mac_idx);

/* This function returns the MAC Sub Interface Count.
 * param[in/out]	IN:  devid   	Device ID used.
 * return		OUT:		Returns MAC Sub Interface Count
 */
u32 gsw_get_mac_subifcnt(u32 devid);

/* This function returns the Total GSWIP device Count.
 * param[in/out]	IN:  devid	Device ID used.
 * return		OUT:		Returns Total GSWIP device Count
 */
u32 gsw_get_total_devs(u32 devid);

#endif
