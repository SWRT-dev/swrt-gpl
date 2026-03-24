/******************************************************************************

                               Copyright (c) 2012
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
/****************************************************************************
 ****************************************************************************
 **
 ** COMPONENT:      ENET SW - RSN 802.11i
 **
 ** MODULE:         $File: //bwp/enet/demo153_sw/develop/src/rsn/rsn.h $
 **
 ** DESCRIPTION:    Public RSN related definitions
 **
 ** AUTHOR:         Julian Hall
 **
 ** THIS VERSION:   $Revision: #2 $
 **   CHANGED ON:   $Date: 2004/01/22 $
 **           BY:   $Author: prh $
 **
 ****************************************************************************
 **
 ** 
 **
 ** 
 **
 ****************************************************************************
 ****************************************************************************/

#ifndef __RSN_H
#define __RSN_H

/***************************************************************************/
/***                   Include File Dependencies                         ***/
/***************************************************************************/
//#include "System_GlobalDefinitions.h"
//#include "ErrorHandler_Api.h"
#include "umi_rsn.h"

#define  MTLK_PACK_ON
#include "mtlkpack.h"

/***************************************************************************/
/***                        Public Defines                               ***/
/***************************************************************************/

/* RSN Info Element types */
#define RSN_IE_ID_IEEE                              (48)
#define RSN_IE_ID_WECA                              (221)

/* RSN IE Header */
#define RSN_IE_ELEMENT_ID_LEN						(1)
#define RSN_IE_LENGTH_LEN							(1)
#define RSN_IE_HEADER_LEN                           (RSN_IE_ELEMENT_ID_LEN + RSN_IE_LENGTH_LEN)// 1 + 1

#define RSN_IE_ID_OFFSET                            (0)
#define RSN_IE_LENGTH_OFFSET                        (RSN_IE_ID_OFFSET + RSN_IE_ELEMENT_ID_LEN)// 0 + 1

/* Maximum RSN IE lengths */
#define RSN_IE_IEEE_MAX_LEN                         UMI_RSN_IE_IEEE_MAX_LEN
#define RSN_IE_WECA_MAX_LEN                         UMI_RSN_IE_WECA_MAX_LEN
#define RSN_IE_MAX_LEN                              UMI_RSN_IE_MAX_LEN

/* Supported RSN IE versions */
#define RSN_IE_SUPPORTED_IEEE_VERSION               (1)
#define RSN_IE_SUPPORTED_WECA_VERSION               (1)

/* RSN IE field sizes in octets */
#define RSN_IE_VERSION_LEN                          (2)
#define RSN_IE_SUITE_SELECTOR_LEN                   (4)
#define RSN_IE_SUITE_COUNT_LEN                      (2)
#define RSN_IE_CAPABILITIES_LEN                     (2)
#define RSN_IE_WECA_VERSION_LEN                     (4)

/* IEEE RSN IE offsets */
#define RSN_IE_IEEE_GROUP_OFFSET                    (RSN_IE_HEADER_LEN + RSN_IE_VERSION_LEN)
#define RSN_IE_IEEE_PAIRWISE_COUNT_OFFSET           (RSN_IE_IEEE_GROUP_OFFSET + RSN_IE_SUITE_SELECTOR_LEN)
#define RSN_IE_IEEE_PAIRWISE_LIST_OFFSET            (RSN_IE_IEEE_PAIRWISE_COUNT_OFFSET + RSN_IE_SUITE_COUNT_LEN)

/* WECA IE offsets */
#define RSN_IE_WECA_GROUP_OFFSET                    (RSN_IE_IEEE_GROUP_OFFSET + RSN_IE_WECA_VERSION_LEN)
#define RSN_IE_WECA_PAIRWISE_COUNT_OFFSET           (RSN_IE_WECA_GROUP_OFFSET + RSN_IE_SUITE_SELECTOR_LEN)
#define RSN_IE_WECA_PAIRWISE_LIST_OFFSET            (RSN_IE_WECA_PAIRWISE_COUNT_OFFSET + RSN_IE_SUITE_COUNT_LEN)

#define RSN_IE_IEEE_PAIRWISE_CIPHER_SUITE_SIZE		(4)
#define RSN_IE_IEEE_AKM_SUITE_SIZE					(4)

/* Maximum allowable suite selectors */
#define RSN_IE_MAX_PAIRWISE_CIPHER_SUITES           (4) 
#define RSN_IE_MAX_AUTH_SUITES                      (3) 

/* Organisation identifiers */
#define RSN_OUI_IEEE_OCTET_0                        (0x00)
#define RSN_OUI_IEEE_OCTET_1                        (0x0F)
#define RSN_OUI_IEEE_OCTET_2                        (0xAC)

#define RSN_OUI_WECA_OCTET_0                        (0x00)
#define RSN_OUI_WECA_OCTET_1                        (0x50)
#define RSN_OUI_WECA_OCTET_2                        (0xF2)





/* Legacy WEP default key index values */
#define RSN_WEP_USE_DEFAULT_KEY_INDEX               UMI_RSN_USE_DEFAULT_KEY_INDEX
#define RSN_USE_PER_ASSOCIATION_KEY                 UMI_RSN_USE_PER_ASSOCIATION_KEY

/* Key management suite selectors */
#define RSN_KEY_MGMT_SUITE_802_1X                   (0x01)
#define RSN_KEY_MGMT_SUITE_PRESHARED                (0x02)

/* General capability bit masks */
#define RSN_GENERAL_CAPS_PREAUTH_SUPPORTED          (0x80)
#define RSN_GENERAL_CAPS_DEFAULT_PAIWISE_KEY        (0x40)

/*************************/
/* Security status codes */
/*************************/
typedef uint16 RSN_STATUS;
#define RSN_STATUS_NO_ALERT                         (0x0000)

/* Loggable security events */
#define RSN_STATUS_ENCRYPT_UNSUCCESSFUL             0
#define RSN_STATUS_TKIP_ICV_ERROR                   (1 << UMI_RSN_EVENT_TKIP_ICV_ERROR)
#define RSN_STATUS_TKIP_MIC_FAILURE                 (1 << UMI_RSN_EVENT_TKIP_MIC_FAILURE)
#define RSN_STATUS_TKIP_REPLAY                      (1 << UMI_RSN_EVENT_TKIP_REPLAY)
#define RSN_STATUS_CCMP_MIC_FAILURE                 (1 << UMI_RSN_EVENT_CCMP_MIC_FAILURE)
#define RSN_STATUS_CCMP_REPLAY                      (1 << UMI_RSN_EVENT_CCMP_REPLAY)
#define RSN_STATUS_ENCRYPT_SUCCESSFUL               (1 << UMI_RSN_EVENT_REKEY_PACKETS)
#define RSN_STATUS_NUM_LOGGABLE                     (6)
#define RSN_STATUS_LOGGABLE_MASK                    UMI_RSN_EVENT_MASK

/* Non loggable security events */
#define RSN_STATUS_DECRYPT_UNSUCCESSFUL             0
#define RSN_STATUS_WEP_ICV_ERROR                    (0x8000)
#define RSN_STATUS_CIPHER_TRANSACTION_ABORTED       (0x4000)
#define RSN_STATUS_PRIVACY_DISABLED                 (0x2000)
#define RSN_STATUS_DECRYPT_SUCCESSFUL               (0x1000)
#define RSN_STATUS_KEY_NOT_AVAILABLE                (0x0800)
#define RSN_STATUS_ILLEGAL_RX_PAYLOAD_LENGTH        (0x0400)
#define RSN_STATUS_ILLEGAL_PACKET                   (0x0200)


/* Key related field lengths */
#define RSN_SEQ_NUM_LEN                             UMI_RSN_SEQ_NUM_LEN
#define RSN_TK1_LEN                                 UMI_RSN_TK1_LEN
#define RSN_TK2_LEN                                 UMI_RSN_TK2_LEN

/* Default number of traffic class priorities supported */
#ifndef RSN_NUM_TRAFFIC_PRIORITIES
#define RSN_NUM_TRAFFIC_PRIORITIES                  (4)
#endif

#define RSN_GET_TRAFFIC_PRIORITY(p)                 (((p) < RSN_NUM_TRAFFIC_PRIORITIES) ? (p) : 0)


#define  MTLK_PACK_OFF
#include "mtlkpack.h"

#endif
