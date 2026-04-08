/******************************************************************************

                               Copyright (c) 2012
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
#ifndef __LOGSERVER_PROTOCOL_H__
#define __LOGSERVER_PROTOCOL_H__

#define MTLK_IDEFS_ON
#define MTLK_IDEFS_PACKING 1
#include "mtlkidefs.h"

/* This message is sent by log server on each connection prior to
 * any log data.
 * NOTE: all fields are in network order
 */
#define LOGSRV_INFO_MAGIC 0xABCDDCBA

struct logsrv_info
{
  uint32 magic; /* LOGSRV_INFO_MAGIC */
  uint32 size; /* sizeof(struct logsrv_info) */
  uint16 log_ver_major;
  uint16 log_ver_minor;
} __MTLK_IDATA;

#define   MTLK_IDEFS_OFF
#include "mtlkidefs.h"

#endif /* __LOGSERVER_PROTOCOL_H__ */
