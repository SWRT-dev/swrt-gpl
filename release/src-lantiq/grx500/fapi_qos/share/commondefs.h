/******************************************************************************

                         Copyright (c) 2012, 2014, 2015
                        Lantiq Beteiligungs-GmbH & Co. KG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/

#ifndef __COMMONDEFS_H__
#define __COMMONDEFS_H__
#include <stdint.h>
/*! \file commondefs.h
 \brief File contains the common custom definitions used across QoS FAPI
*/

/*!
  \brief Custom type definition of char data type.
*/
typedef char               char8;
/*!
  \brief Custom type definition of unsigned char data type.
*/
typedef unsigned char      uchar8;
/*!
  \brief Custom type definition of char data type.
*/
typedef char               int8;
/*!
  \brief Custom type definition of unsigned char data type.
*/
typedef unsigned char      uint8;
/*!
  \brief Custom type definition of short int data type.
*/
typedef short int          int16;
/*!
  \brief Custom type definition of unsigned short int data type.
*/
typedef unsigned short int  uint16;
/*!
  \brief Custom type definition of int data type.
*/
typedef int                int32;
/*!
  \brief Custom type definition of unsigned int data type.
*/
typedef unsigned int        uint32;
/*!
  \brief int32_t 
*/
typedef int                int32_t;
/*!
  \brief uint32 
*/
//typedef unsigned int        uint32_t;
/*!
  \brief Custom type definition of short int data type. 
*/
typedef short int          int16_t;
/*!
  \brief int8_t  
*/
#ifndef _SYS_TYPES_H
//typedef char               int8_t;
#endif
/*!
  \brief Custom type definition of unsigned short int data type.
*/
typedef unsigned short int  uint16_t;
/*!
  \brief Custom type definition of unsigned char data type. 
*/
typedef unsigned char  bool_t;

/*!
  \brief Custom type definition of unsigned long data type.
*/
//typedef unsigned long uint64_t;
typedef unsigned long uint64;


/** \addtogroup FAPI_QOS_GENERAL */
/* @{ */
/*!
  \brief IN 
*/
#define IN
/*!
  \brief OUT 
*/
#define OUT
/*!
  \brief IN_OUT 
*/
#define IN_OUT

/*! \def LTQ_SUCCESS
    \brief Macro that defines success state.
 */
#define LTQ_SUCCESS 0
/*! \def LTQ_FAIL
    \brief Macro that defines failure state.
 */
#define LTQ_FAIL -1
/*! \def MAX_DATA_LEN
    \brief Macro that defines maximum data length for stack buffers.
 */
#ifndef MAX_DATA_LEN
#define MAX_DATA_LEN 2024
#endif
#define MAX_IF_LEN 100
#define MAX_CMD_LEN 224
#define MAX_STR_LEN 1024
/*! \def LTQ_ENABLE
    \brief Macro that defines enable state.
 */
#define QOSAL_ENABLE 1
/*! \def LTQ_DISABLE
    \brief Macro that defines enable state.
 */
#define QOSAL_DISABLE 0

/*! \def LTQ_TRUE
    \brief Macro that defines enable state.
 */
#define QOSAL_TRUE 1
/*! \def LTQ_FALSE
    \brief Macro that defines enable state.
 */
#define QOSAL_FALSE 0
/*! \def MAX_NAME_LEN
    \brief Macro that defines Name length.
 */
#define MAX_NAME_LEN 64

/*! \def MAX_INTERFACES
    \brief Macro that defines max interfaces.
 */
#define MAX_INTERFACES 9

/*! \def MAX_QUEUE_COUNT
    \brief Macro that defines max queue count
 */
#define MAX_QUEUE_COUNT 9

/*!
  \brief MAX Interface name length
*/
#define MAX_IF_NAME_LEN 32

/*!
  \brief MAX Interface name length
*/
#define MAX_L7_PROTO_NAME_LEN 32

/*!
  \brief QoS PID file
*/
#define QOSD_PID_FILE "/var/run/qosd.pid"

/*!
  \brief QoS log file
*/
#define LOG_FILE_LOC "/tmp/qosal_dbg"

/*!
  \brief Macro to suppress unused variable warning during compilation
*/
#define UNUSED(x)	(void)(x)

/*!
    \brief Enum defining various interface categories supported.
*/
typedef enum iftype {
	QOS_IF_CATEGORY_ETHLAN, /*!< Ethernet LAN interface category. */
	QOS_IF_CATEGORY_ETHWAN, /*!< Ethernet WAN interface category. */
	QOS_IF_CATEGORY_PTMWAN, /*!< xDSL PTM WAN interface category. */
	QOS_IF_CATEGORY_ATMWAN, /*!< xDSL ATM WAN interface category. */
	QOS_IF_CATEGORY_LTEWAN, /*!< LTE WAN interface category. */
	QOS_IF_CATEGORY_WLANDP, /*!< WLAN in Directpath interface category. */
	QOS_IF_CATEGORY_WLANNDP, /*!< WLAN in Non-directpath interface category. */
	QOS_IF_CATEGORY_LOCAL, /*!< Local interface category. */
	QOS_IF_CATEGORY_CPU, /*!< CPU interface category. */
	QOS_IF_CATEGORY_MAX /*!< Max interface category count. */
} iftype_t;

#define NUM_IF_CATEGORIES	QOS_IF_CAT_INGGRP_MAX
#define MAX_IF_PER_GROUP	20
#define IFGROUP_INVALID(g) 	((g < QOS_IF_CATEGORY_ETHLAN) || (g >= QOS_IF_CATEGORY_MAX))
#define IFGROUP_VALID(g)	(!(IFGROUP_INVALID(g)))
#define IFGROUP_IS_WAN(g) 	((g == QOS_IF_CATEGORY_ETHWAN) || (g == QOS_IF_CATEGORY_PTMWAN) || (g == QOS_IF_CATEGORY_ATMWAN))

/*!
    \brief Enum defining interface groups supported.
*/

typedef enum ifinggrp {
    QOS_IF_CAT_INGGRP0,
    QOS_IF_CAT_INGGRP1,
    QOS_IF_CAT_INGGRP2,
    QOS_IF_CAT_INGGRP3,
    QOS_IF_CAT_INGGRP4,
    QOS_IF_CAT_INGGRP5,
    QOS_IF_CAT_INGGRP_MAX /*!< Max ingres group category count. */
} ifinggrp_t;

#define NUM_INGRESS_GROUPS 		QOS_IF_CAT_INGGRP_MAX
#define MAX_IF_PER_INGRESS_GROUP	20
#define IFINGGRP_INVALID(g) 		((g < QOS_IF_CAT_INGGRP0) ||(g >= QOS_IF_CAT_INGGRP_MAX))
#define IFINGGRP_VALID(g) 		(!IFINGGRP_INVALID(g))
/*!
    \brief QoS log mechanism
*/
typedef enum {
	QOS_LOG_CONSOLE=2, /*!< Log messages to be shown on console */
	QOS_LOG_FILE=1 /*!< Log messages to be put into file */
}log_type_t;
/* @} */

/* These should be same as queues and classifiers operation flags */
/*!
  \brief Queue add operation flag
*/
#define QOS_OP_ADD 0x00000001
/*!
  \brief Queue modify operation flag
*/
#define QOS_OP_MODIFY 0x00000002
/*!
  \brief Queue delete operation flag
*/
#define QOS_OP_DELETE 0x00000004
/*!
  \brief Queue NO operation flag
*/
#define QOS_OP_NOOPER 0x80000000

int32_t qosd_set_qoscfg(void);
#endif /*__COMMONDEFS_H__*/
