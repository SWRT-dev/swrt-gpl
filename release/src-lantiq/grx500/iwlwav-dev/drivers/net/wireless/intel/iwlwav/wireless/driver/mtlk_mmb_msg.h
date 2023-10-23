/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#ifndef _MTLK_MMB_MSG_H_
#define _MTLK_MMB_MSG_H_

#include "mhi_umi.h"

typedef UMI_MSG_HEADER *PMSG_OBJ;

static __INLINE uint16
MSG_OBJ_GET_ID (const UMI_MSG_HEADER *msg_hdr)
{
  return msg_hdr->u16MsgId;
}

static __INLINE void
MSG_OBJ_SET_ID (UMI_MSG_HEADER *msg_hdr, uint16 id)
{
  msg_hdr->u16MsgId = id;
}

static __INLINE void *
MSG_OBJ_PAYLOAD (UMI_MSG_HEADER *msg_hdr)
{
  return &msg_hdr[1];
}

#endif /* !_MTLK_MMB_MSG_H_ */ 
