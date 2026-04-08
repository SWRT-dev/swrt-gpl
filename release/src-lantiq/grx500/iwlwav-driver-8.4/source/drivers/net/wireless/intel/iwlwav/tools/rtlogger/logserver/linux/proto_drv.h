/******************************************************************************

                               Copyright (c) 2012
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
/*
 * 
 *
 * Driver protocol
 *
 * Written by: Andrey Fidrya
 *
 */

#ifndef __PROTO_DRV_H__
#define __PROTO_DRV_H__

#include "cqueue.h"

typedef struct _cdev_t
{
  char *name;
  int fd;
  uint16 log_ver_major;
  uint16 log_ver_minor;
} cdev_t;

cdev_t *cdev_open(const char *dev_name);
int cdev_close(cdev_t *dev);
int cdev_read_to_q(cqueue_t *pqueue, cdev_t *dev);

static inline uint16
cdev_get_ver_major (cdev_t *dev)
{
  return dev->log_ver_major;
}

static inline uint16
cdev_get_ver_minor (cdev_t *dev)
{
  return dev->log_ver_minor;
}

int drv_process_next_pkt(cqueue_t *pqueue);

#endif // !__PROTO_DRV_H__

