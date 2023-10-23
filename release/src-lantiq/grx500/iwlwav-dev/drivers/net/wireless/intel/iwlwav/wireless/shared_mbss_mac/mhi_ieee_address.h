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
** MODULE:         $File: //bwp/enet/demo153_sw/develop/src/shared/mhi_ieee_address.h $
**
** DESCRIPTION:    Public interface to IEEE_ADDR class
**
** AUTHOR:         Ross Hetherington
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

#ifndef __MHI_IEEE_ADDRESS_H
#define __MHI_IEEE_ADDRESS_H


#define   MTLK_PACK_ON
#include "mtlkpack.h"

/* Length of IEEE address (bytes) */
#define IEEE_ADDR_LEN                   (6)

/* This type is used for Source and Destination MAC addresses and also as   */
/* unique identifiers for Stations and Networks.                            */

typedef struct _IEEE_ADDR
{
    uint8 au8Addr[IEEE_ADDR_LEN]; /* WARNING: Special case! No padding here! This structure must be padded externally! */
} __MTLK_PACKED IEEE_ADDR;

#define IEEE_ADDR_CONTROL_BITS_MASK     (0x03) /* masks out bottom two bits */
#define IEEE_ADDR_UNIVERSAL             (0x02) /* the universal/local bit */
#define IEEE_ADDR_GROUP                 (0x01) /* the group/individual bit */

#define   MTLK_PACK_OFF
#include "mtlkpack.h"

#endif /* !__MHI_IEEE_ADDRESS_H */
