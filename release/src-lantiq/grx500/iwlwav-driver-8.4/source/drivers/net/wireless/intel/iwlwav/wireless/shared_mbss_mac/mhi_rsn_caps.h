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
 ** MODULE:         $File: //bwp/enet/demo153_sw/develop/src/shared/mhi_rsn_caps.h $
 **
 ** DESCRIPTION:    RsnCaps definition.  The RSN Capabilities class
 **                 definition.
 **
 ** AUTHOR:         Julian Hall
 **
 ** THIS VERSION:   $Revision: #1 $
 **   CHANGED ON:   $Date: 2003/10/27 $
 **           BY:   $Author: njk $
 **
 ****************************************************************************
 **
 ** 
 **
 ** 
 **
 ****************************************************************************
 ****************************************************************************/

#ifndef __MHI_RSN_CAPS_H
#define __MHI_RSN_CAPS_H

#define   MTLK_PACK_ON
#include "mtlkpack.h"

/***************************************************************************/
/***                           Public Defines                            ***/
/***************************************************************************/

/* RSN capabilities bit map definitions */

/* No RSN capabilities */
#define RSN_CAPS_NONE                               (0x00000000)

/* Group Cipher Suites */
#define RSN_CAPS_GROUP_CIPHER_SUITE_MASK            (0x0000003F)
#define RSN_CAPS_GROUP_CIPHER_SUITE_NONE            (0x00000001)
#define RSN_CAPS_GROUP_CIPHER_SUITE_WEP40           (0x00000002)
#define RSN_CAPS_GROUP_CIPHER_SUITE_TKIP            (0x00000004)
#define RSN_CAPS_GROUP_CIPHER_SUITE_WRAP            (0x00000008)
#define RSN_CAPS_GROUP_CIPHER_SUITE_CCMP            (0x00000010)
#define RSN_CAPS_GROUP_CIPHER_SUITE_WEP104          (0x00000020)

/* Pairwise Cipher Suites */
#define RSN_CAPS_PAIRWISE_CIPHER_SUITE_MASK         (0x00000FC0)
#define RSN_CAPS_PAIRWISE_CIPHER_SUITE_NONE         (0x00000040)
#define RSN_CAPS_PAIRWISE_CIPHER_SUITE_WEP40        (0x00000080)
#define RSN_CAPS_PAIRWISE_CIPHER_SUITE_TKIP         (0x00000100)
#define RSN_CAPS_PAIRWISE_CIPHER_SUITE_WRAP         (0x00000200)
#define RSN_CAPS_PAIRWISE_CIPHER_SUITE_CCMP         (0x00000400)
#define RSN_CAPS_PAIRWISE_CIPHER_SUITE_WEP104       (0x00000800)

/* Combined group and pairwise cipher suite mask */
#define RSN_CAPS_CIPHER_SUITE_MASK \
    (RSN_CAPS_GROUP_CIPHER_SUITE_MASK | RSN_CAPS_PAIRWISE_CIPHER_SUITE_MASK)

/* Authenticated Key Management Suites */
#define RSN_CAPS_AUTH_SUITE_MASK                    (0x00003000)
#define RSN_CAPS_AUTH_SUITE_IEEE802_1X              (0x00001000)
#define RSN_CAPS_AUTH_SUITE_PRESHARED_KEY           (0x00002000)

/* General capabilities */
#define RSN_CAPS_GENERAL_CAPS_MASK                  (0x0000C000)
#define RSN_CAPS_DEFAULT_GENERAL_CAPS               (0x00000000)
#define RSN_CAPS_PRE_AUTH_SUPPORTED                 (0x00004000)
#define RSN_CAPS_USE_DEFAULT_PAIRWISE_KEY           (0x00008000)

/* Info Element type */
#define RSN_CAPS_IE_TYPE_MASK                       (0x00030000)
#define RSN_CAPS_IE_TYPE_IEEE                       (0x00010000)
#define RSN_CAPS_IE_TYPE_WECA                       (0x00020000)


/***************************************************************************/
/***                        Public Typedefs                              ***/
/***************************************************************************/

typedef struct _RSN_CAPS
{
    uint32  u32CapsMap;
} __MTLK_PACKED RSN_CAPS;

#define   MTLK_PACK_OFF
#include "mtlkpack.h"

#endif // __MHI_RSN_CAPS__H
