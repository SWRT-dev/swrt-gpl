/******************************************************************************

                               Copyright (c) 2012
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
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
