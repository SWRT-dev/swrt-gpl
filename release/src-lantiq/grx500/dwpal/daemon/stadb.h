/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#ifndef __WAVE_STA_DB__H__
#define __WAVE_STA_DB__H__

#include "wave_ipc_server.h"
#include "linked_list.h"

#include <pthread.h>

typedef struct _station_db
{
	l_list *sta_list;
	pthread_mutex_t lock;
} station_db;

#define LOCK_STA_DB(__stadb) pthread_mutex_lock(&((__stadb)->lock));
#define UNLOCK_STA_DB(__stadb) pthread_mutex_unlock(&((__stadb)->lock));

#endif /* __WAVE_STA_DB__H__ */
