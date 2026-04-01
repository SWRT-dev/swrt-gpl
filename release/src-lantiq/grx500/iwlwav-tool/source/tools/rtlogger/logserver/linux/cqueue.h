/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

/*
 * 
 *
 * Cyclic queue
 *
 * Written by: Andrey Fidrya
 *
 */

#ifndef __CQUEUE_H__
#define __CQUEUE_H__

#ifdef CPTCFG_IWLWAV_DEBUG
#define CQUEUE_DEBUG_DUMP(pqueue, binary) \
  cqueue_debug_dump(#pqueue, pqueue, binary)
#else
#define CQUEUE_DEBUG_DUMP(pqueue, binary)
#endif

typedef struct _cqueue_t
{
  int overflow_state;
  char *data;
  int max_size;
  int begin_idx;
  int end_idx;
  int is_empty;
} cqueue_t;

void cqueue_init (cqueue_t *pqueue);
int cqueue_reset (cqueue_t *pqueue, int max_size);
void cqueue_cleanup (cqueue_t *pqueue);
int cqueue_space_left (cqueue_t *pqueue);
int cqueue_size (cqueue_t *pqueue);
int cqueue_max_size (cqueue_t *pqueue);
int cqueue_empty (cqueue_t *pqueue);
int cqueue_full (cqueue_t *pqueue);
void cqueue_get (cqueue_t *pqueue, int offset, int len, unsigned char *data);
void cqueue_pop_front (cqueue_t *pqueue, int len);
void cqueue_push_back (cqueue_t *pqueue, unsigned char *data, int len);
int cqueue_read (cqueue_t *pqueue, int fd);
int cqueue_write (cqueue_t *pqueue, int fd);
int cqueue_reserve (cqueue_t *pqueue, int max_size);
#ifdef CPTCFG_IWLWAV_DEBUG
void cqueue_debug_dump (const char *qname, cqueue_t *pqueue, int binary);
#endif // CPTCFG_IWLWAV_DEBUG
    
#endif // !__CQUEUE_H__

