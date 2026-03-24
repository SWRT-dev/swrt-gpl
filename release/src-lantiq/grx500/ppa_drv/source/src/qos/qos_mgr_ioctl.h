#ifndef __QOS_MGR_IOCTL_H__20200123_1911__
#define __QOS_MGR_IOCTL_H__20200123_1911__
/******************************************************************************
 **
 ** FILE NAME  : qos_mgr_ioctl.h
 ** PROJECT  : UGW
 ** MODULES  : QoS Manager Ioct
 **
 ** DATE  : 08 Jan 2020
 ** AUTHOR  : Mohammed Aarif
 ** DESCRIPTION  : QoS Manager Ioctl Implementation
 ** COPYRIGHT  : Copyright (c) 2019 - 2020 Intel Corporation
 **
 ** HISTORY
 ** $Date    $Author      $Comment
 ** 08 JAN 2020    Mohammed Aarif    Initiate Version
 *******************************************************************************/


/*! \file qos_mgr_ioctl.h
  \brief This file contains QoS Manager Ioctls
 */

/*
 * ####################################
 *    Definition
 * ####################################
 */

/*
 *  Compilation Switch
 */

#define ENABLE_DEBUG  1

#define ENABLE_ASSERT  1

#define ENABLE_ERROR  1

#define ENABLE_SESSION_DEBUG_FLAGS  1

#define DEBUG_DUMP_LIST_ITEM    1

#define ENABLE_DYNAMIC_NETIF_DETECTION    0

#if defined(ENABLE_DEBUG) && ENABLE_DEBUG
#define ENABLE_DEBUG_PRINT      1
#define DISABLE_INLINE        1
#endif

#if defined(DISABLE_INLINE) && DISABLE_INLINE
#define INLINE
#else
#define INLINE  inline
#endif


#if defined(ENABLE_ASSERT) && ENABLE_ASSERT
#undef  ASSERT
#define ASSERT(cond, format, arg...)  do { if ( (g_qos_mgr_dbg_enable & DBG_ENABLE_MASK_ASSERT) && !(cond) ) printk(KERN_ERR __FILE__ ":%d:%s: " format "\n", __LINE__, __FUNCTION__, ##arg); } while ( 0 )
#else
#define ASSERT(cond, format, arg...)
#endif

#if defined(ENABLE_ERROR) && ENABLE_ERROR
#undef  err
#define err(format, arg...)  do { if ( (g_qos_mgr_dbg_enable & DBG_ENABLE_MASK_ERR) ) printk(KERN_ERR __FILE__ ":%d:%s: " format "\n", __LINE__, __FUNCTION__, ##arg); } while ( 0 )
#else
#define err(format, arg...)
#endif


#if defined(ENABLE_ERROR) && ENABLE_ERROR
#undef  critial_err
#define critial_err(format, arg...)  do { printk(KERN_EMERG __FILE__ ":%d:%s: " format "\n", __LINE__, __FUNCTION__, ##arg); } while ( 0 )
#else
#define critial_err(format, arg...)
#endif

#define PRINT_RATELIMIT printk_ratelimited 
/*
 *  Debug Print Mask
 *  Note, once you add a new DBG_ macro, don't forget to add it in DBG_ENABLE_MASK_ALL also !!!!
 */
#define DBG_ENABLE_MASK_ERR           (1 << 0)
#define DBG_ENABLE_MASK_ASSERT        (1 << 1)
#define DBG_ENABLE_MASK_DEBUG_PRINT   (1 << 2)
#define DBG_ENABLE_MASK_QOS           (1 << 3)

#define DBG_ENABLE_MASK_ALL  (DBG_ENABLE_MASK_ERR | DBG_ENABLE_MASK_DEBUG_PRINT \
		| DBG_ENABLE_MASK_ASSERT | DBG_ENABLE_MASK_QOS)

/*
 *  helper macro
 */
#define NUM_ENTITY(x)  (sizeof(x) / sizeof(*(x)))

/*qos_mgr_debug currently supported flags:
  DBG_ENABLE_MASK_ERR,
  DBG_ENABLE_MASK_DEBUG_PRINT,
  DBG_ENABLE_MASK_ASSERT,
  DBG_ENABLE_MASK_QOS,
  DBG_ENABLE_MASK_ALL*/

#if defined(ENABLE_DEBUG_PRINT) && ENABLE_DEBUG_PRINT
#undef qos_mgr_debug
#define qos_mgr_debug(flag, fmt, arg...)  do { if ( (g_qos_mgr_dbg_enable & flag) && max_print_num ) { printk(KERN_ERR fmt, ##arg); if( max_print_num != ~0) max_print_num--; } } while ( 0 )
#else
#undef qos_mgr_debug
#define qos_mgr_debug(flag, fmt, arg...)
#endif

/*
 * ####################################
 *    Data Type
 * ####################################
 */

/*
 * ####################################
 *     Declaration
 * ####################################
 */

extern uint32_t g_qos_mgr_dbg_enable;
extern uint32_t max_print_num;

long qos_mgr_dev_ioctl(struct file *, unsigned int, unsigned long);

#endif  /*  __QOS_MGR_IOCTL_H__20200123_1911__*/
