/******************************************************************************

                               Copyright (c) 2012
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
/***************************************************************************
****************************************************************************
**
** COMPONENT:        ENET Upper MAC    SW
**
** MODULE:           $File: //bwp/enet/demo153_sw/develop/src/mac_upper/umi_rsn.h $
**
** VERSION:          $Revision: #1 $
**
** DATED:            $Date: 2007/03/04 $
**
** AUTHOR:           S Genia
**
** DESCRIPTION:      Upper MAC Public Header
**
*****************************************************************************
**
** 
**
****************************************************************************/

#ifndef UMI_RSN_INCLUDED_H
#define UMI_RSN_INCLUDED_H

#define   MTLK_PACK_ON
#include "mtlkpack.h"

/* RSN key types */
#define UMI_RSN_PAIRWISE_KEY			0
#define UMI_RSN_GROUP_KEY				1
#define UMI_RSN_MGMT_GROUP_KEY			2
#define UMI_RSN_MGMT_BEACON_GROUP_KEY	3


/* RSN key index options */
#define UMI_RSN_USE_DEFAULT_KEY_INDEX   4
#define UMI_RSN_USE_PER_ASSOCIATION_KEY 5

#define UMI_RSN_KEY_INDEX_MAX_VALUE		3 	//   (0/1/2/3)
#define UMI_RSN_KEY_INDEX_INVALID		0xFF

#define UMI_RSN_IGTK_GN_KEY_INDEX 5
#define UMI_RSN_IGTK_GM_KEY_INDEX 6


#ifndef CPU_ARC

/* The next enumeration is defined in DescriptionsDefinitions.h aswell 
   therefore to inhibit fuplicate definition errors while
   compiling at FW_SRC, the preprocessor CPU_ARC definition is used */

/* notice, WAVE600 does not support WAPI cipher anymore (number 3) */
/* --> The following enumaration should be identical to the same enumration
   in DescriptionsDefinitions.h (in shared_header repository) <--  */
typedef enum _SecurityTypeSecd_e
{
	SECURITY_TYPE_SECD_TKIP        = 0,
	SECURITY_TYPE_SECD_WEP40       = 1,
	SECURITY_TYPE_SECD_CCMP        = 2,
	SECURITY_TYPE_SECD_BIP_CMAC128 = 4,
	SECURITY_TYPE_SECD_WEP104      = 5,
	SECURITY_TYPE_SECD_GCMP128     = 6,
	SECURITY_TYPE_SECD_GCMP256     = 7,
	SECURITY_TYPE_SECD_BIP_GMAC128 = 8,
	SECURITY_TYPE_SECD_BIP_GMAC256 = 9,
	SECURITY_TYPE_SECD_SKIP        = 15,
	SECURITY_TYPE_SECD_LAST,
} SecurityTypeSecd_e;

#define UNUSED_SECURITY_TYPE_WAPI	3  // the WAPI cipher mode was canceled in WAVE600-2

#endif


/* RSN field sizes */
#define UMI_RSN_IE_IEEE_MAX_LEN         40
#define UMI_RSN_IE_WECA_MAX_LEN         40
#define UMI_RSN_IE_MAX_LEN              (UMI_RSN_IE_IEEE_MAX_LEN + UMI_RSN_IE_WECA_MAX_LEN) /* In Mix Mode, driver sends both IE as one */
#define UMI_RSN_SEQ_NUM_LEN             6
#define UMI_RSN_TK1_LEN                 16
#define UMI_RSN_TK2_LEN                 16

/* RSN security event codes */
#define UMI_RSN_EVENT_TKIP_ICV_ERROR    0
#define UMI_RSN_EVENT_TKIP_MIC_FAILURE  1
#define UMI_RSN_EVENT_TKIP_REPLAY       2
#define UMI_RSN_EVENT_CCMP_MIC_FAILURE  3
#define UMI_RSN_EVENT_CCMP_REPLAY       4
#define UMI_RSN_EVENT_REKEY_PACKETS     5
#define UMI_RSN_EVENT_TEST_BIT(s,b)     ((s) & (1 << (b))
#define UMI_RSN_EVENT_MASK              (0x3F)

#define   MTLK_PACK_OFF
#include "mtlkpack.h"

#endif /* !UMI_RSN_INCLUDED_H */
