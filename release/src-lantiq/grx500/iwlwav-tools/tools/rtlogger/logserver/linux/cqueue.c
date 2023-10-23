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

#include "mtlkinc.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "logsrv_utils.h"
#include "cqueue.h"

#define LOG_LOCAL_GID   GID_CQUEUE
#define LOG_LOCAL_FID   1

/*****************************************************************************
**
** The group of functions with cqueue_ prefix implements a cyclic queue of
** fixed size for storing data before sending it to userspace.
**
** cqueue_init - initializes a newly created queue (constructor).
** cqueue_reset - allocates/reallocates a buffer for storing the data.
** cqueue_cleanup - frees queue resources (destructor).
** cqueue_space_left - returns size of free space left in a queue.
** cqueue_size - returns data size in a queue.
** cqueue_max_size - returns maximum data size a queue can hold.
** cqueue_empty - returns true if queue is empty.
** cqueue_full - returns true if no space left in queue.
** cqueue_get - gets data from queue.
** cqueue_pop_front - removes data at the front of queue.
** cqueue_push_back - copies data to the end of queue.
** cqueue_read - reads data from fd to a queue.
** cqueue_write - writes data from a queue to fd.
** cqueue_reserve - increase queue size preserving the data.
** cqueue_debug_dump - for debug-printing of queue contents.
**                     Note: use CQUEUE_DEBUG_DUMP macro instead.
******************************************************************************/

static int begin_reached_end(cqueue_t *pqueue);

void
cqueue_init (cqueue_t *pqueue)
{
  pqueue->overflow_state = 0;
  pqueue->data = NULL;
  pqueue->max_size = 0;
  pqueue->begin_idx = 0;
  pqueue->end_idx = 0;
  pqueue->is_empty = 1;
}

int
cqueue_reset (cqueue_t *pqueue, int max_size)
{
  int result = 0;

  pqueue->overflow_state = 0;
  if (pqueue->max_size != max_size) {
    // In case anything happens during initialization, treat the queue as empty
    pqueue->max_size = 0;
    if (pqueue->data) {
      free(pqueue->data);
      pqueue->data = NULL;
    }

    pqueue->data = (char *) malloc(max_size);
    if (!pqueue->data) {
      ELOG_V("Out of memory");
      result = -1;
    } else {
      pqueue->max_size = max_size;
    }
  }
  pqueue->begin_idx = 0;
  pqueue->end_idx = 0;
  pqueue->is_empty = 1;

  return result;
}

void
cqueue_cleanup (cqueue_t *pqueue)
{
  if (pqueue->data) {
    free(pqueue->data);
    pqueue->data = NULL;
  }
}

int
cqueue_space_left (cqueue_t *pqueue)
{
  if (pqueue->is_empty)
    return pqueue->max_size;
  // XXXX - free space, .... - data
  if (pqueue->begin_idx < pqueue->end_idx) {
    // [XXXX<begin>....<end>XXXX]
    return pqueue->begin_idx + (pqueue->max_size - pqueue->end_idx);
  }
  // begin >= end
  // [....<end>XXXX<begin>....]
  return pqueue->begin_idx - pqueue->end_idx;
}

int
cqueue_size (cqueue_t *pqueue)
{
  //return pqueue->max_size - cqueue_space_left(pqueue);

  if (pqueue->is_empty)
    return 0;

  // XXXX - free space, .... - data
  if (pqueue->begin_idx < pqueue->end_idx) {
    // [XXXX<begin>....<end>XXXX]
    return pqueue->end_idx - pqueue->begin_idx;
  }
  // begin >= end
  // [....<end>XXXX<begin>....]
  return pqueue->end_idx + (pqueue->max_size - pqueue->begin_idx);
}

int
cqueue_max_size (cqueue_t *pqueue)
{
  return pqueue->max_size;
}

int
cqueue_empty (cqueue_t *pqueue)
{
  return pqueue->is_empty;
}

int
cqueue_full (cqueue_t *pqueue)
{
  return cqueue_space_left(pqueue) == 0;
}

void
cqueue_get(cqueue_t *pqueue, int offset, int len, unsigned char *data)
{
#ifdef CPTCFG_IWLWAV_DEBUG
  int sz = cqueue_size(pqueue);
  ASSERT(offset >= 0 && offset <= sz);
  ASSERT(len >= 0 && len <= sz - offset);
  ILOG9_DDD("offset=%d, len=%d, sz=%d", offset, len, sz);
#endif
  // Take offset into account:
  int offset_begin_idx = pqueue->begin_idx + offset;
  if (offset_begin_idx > pqueue->max_size)
    offset_begin_idx -= pqueue->max_size;

  // XXXX - free space, .... - data
  if (offset_begin_idx < pqueue->end_idx) {
    // [XXXX<begin>....<end>XXXX]
    wave_memcpy(data, len, &pqueue->data[offset_begin_idx], len);
  } else {
    // begin >= end
    // [....<end>XXXX<begin>....]
    int datalen_at_the_end =
      pqueue->max_size - offset_begin_idx;
    unsigned int to_copy = MIN(len, datalen_at_the_end);
    if (to_copy > 0)
      wave_memcpy(data, len, &pqueue->data[offset_begin_idx], to_copy);
    if (len == to_copy) { // We're copying lesser data than available at the end
      // Everything was copied
    } else {
      // Wraparound and continue copying
      unsigned int bytes_remaining = len - to_copy;
      wave_memcpy(data + to_copy, bytes_remaining, pqueue->data, bytes_remaining);
    }
  }
}

void
cqueue_pop_front (cqueue_t *pqueue, int len)
{
  ASSERT(cqueue_size(pqueue) >= len);

  // XXXX - free space, .... - data
  if (pqueue->begin_idx < pqueue->end_idx) {
    // [XXXX<begin>....<end>XXXX]
    pqueue->begin_idx += len;
  } else {
    // begin >= end
    // [....<end>XXXX<begin>....]
    unsigned int end_space_left = pqueue->max_size - pqueue->end_idx;
    unsigned int to_skip = MIN(len, end_space_left);
    if (len == to_skip) { // end_space_left > size we're going to skip
      // No wrap around
      pqueue->begin_idx += to_skip;
    } else { // (to_skip is end_space_left) < len
      // Wrap around and skip remaining bytes at the beginning
      pqueue->begin_idx = len - to_skip;
    }
  }
  if (begin_reached_end(pqueue)) {
    pqueue->begin_idx = 0;
    pqueue->end_idx = 0;
    pqueue->is_empty = 1;
  }
}

void
cqueue_push_back (cqueue_t *pqueue, unsigned char *data, int len)
{
  ASSERT(cqueue_space_left(pqueue) >= len);
  ASSERT(len > 0);
  // XXXX - free space, .... - data
  if (pqueue->begin_idx < pqueue->end_idx) {
    // [XXXX<begin>....<end>XXXX]
    unsigned int end_space_left = pqueue->max_size - pqueue->end_idx;
    unsigned int to_write = MIN(len, end_space_left);
    if (to_write > 0) {
      wave_memcpy(&pqueue->data[pqueue->end_idx], (pqueue->max_size - pqueue->end_idx), data, to_write);
    }
    if (len == to_write) {
      // Everything was copied
      pqueue->end_idx += to_write;
    } else {
      // Wrap around and write bytes for which there were no space at the tail
      unsigned int bytes_remaining = len - to_write;
      wave_memcpy(pqueue->data, pqueue->max_size, data + to_write, bytes_remaining);
      pqueue->end_idx = bytes_remaining;
    }
  } else {
    // begin >= end
    // [....<end>XXXX<begin>....]
    if (pqueue->is_empty)
      pqueue->is_empty = 0;
    wave_memcpy(&pqueue->data[pqueue->end_idx], (pqueue->max_size - pqueue->end_idx), data, len);
    pqueue->end_idx += len;
  }
}

int
cqueue_read (cqueue_t *pqueue, int fd)
{
  int ret;
  ILOG9_DD("cqueue_read: begin_idx=%d, end_idx=%d", pqueue->begin_idx,
    pqueue->end_idx);
  // XXXX - free space, .... - data
  if (pqueue->begin_idx < pqueue->end_idx) {
    // [XXXX<begin>....<end>XXXX]
    unsigned int end_space_left = pqueue->max_size - pqueue->end_idx;
    if (end_space_left > 0) {
      ret = read(fd, &pqueue->data[pqueue->end_idx], end_space_left);
      ILOG9_D("cqueue_read: read returned %d", ret);
      if (ret <= 0)
        return ret;
      pqueue->end_idx += ret;
      pqueue->is_empty = 0;
      if (ret < end_space_left) {
        // Everything was read
        return 1;
      }
    }
    // Wrap around and continue reading (if there's space in buffer)
    if (pqueue->begin_idx > 0) {
      ret = read(fd, pqueue->data, pqueue->begin_idx);
      if (ret <= 0)
        return ret;
      pqueue->end_idx = ret;
      pqueue->is_empty = 0;
    }
  } else {
    // begin >= end
    // [....<end>XXXX<begin>....]
    unsigned int to_read;
    if (pqueue->is_empty)
      to_read = pqueue->max_size;
    else
      to_read = pqueue->begin_idx - pqueue->end_idx;

    if (to_read > 0) {
      ret = read(fd, &pqueue->data[pqueue->end_idx], to_read);
      if (ret <= 0)
        return ret;
      pqueue->is_empty = 0;
      pqueue->end_idx += ret;
    }
  }
  return 1;
}

int
cqueue_write (cqueue_t *pqueue, int fd)
{
  int ret;
  ILOG9_DD("cqueue_write: begin_idx=%d, end_idx=%d", pqueue->begin_idx,
    pqueue->end_idx);
  ASSERT(!cqueue_empty(pqueue));

  if (pqueue->begin_idx == pqueue->max_size)
    pqueue->begin_idx = 0;

  // XXXX - free space, .... - data
  if (pqueue->begin_idx < pqueue->end_idx) {
    // [XXXX<begin>....<end>XXXX]
    ret = write(fd, &pqueue->data[pqueue->begin_idx], 
        pqueue->end_idx - pqueue->begin_idx);
    ILOG9_D("cqueue_write: write returned %d", ret);
    if (ret <= 0)
      return ret;
    pqueue->begin_idx += ret;
    ILOG9_DDD("cqueue_write: begin_idx=%d, end_idx=%d, begin_reached_end=%d",
        pqueue->begin_idx, pqueue->end_idx, begin_reached_end(pqueue));
    if (begin_reached_end(pqueue)) {
      pqueue->begin_idx = 0;
      pqueue->end_idx = 0;
      pqueue->is_empty = 1;
    }
  } else {
    // begin >= end
    // [....<end>XXXX<begin>....]
    ret = write(fd, &pqueue->data[pqueue->begin_idx],
        pqueue->max_size - pqueue->begin_idx);
    ILOG9_D("cqueue_write: write returned %d", ret);
    if (ret <= 0)
      return ret;
    pqueue->begin_idx += ret;
    // Other data will be processed by a next call
    ILOG9_DDD("cqueue_write: begin_idx=%d, end_idx=%d, begin_reached_end=%d",
        pqueue->begin_idx, pqueue->end_idx, begin_reached_end(pqueue));
    if (begin_reached_end(pqueue)) {
      pqueue->begin_idx = 0;
      pqueue->end_idx = 0;
      pqueue->is_empty = 1;
    } else {
      // This call will perform a wraparound and write the rest of data
      return cqueue_write(pqueue, fd);
    }
  }
  return 1;
}

int cqueue_reserve (cqueue_t *pqueue, int max_size)
{
  if (max_size <= pqueue->max_size)
    return 0;

  void *newdata = realloc(pqueue->data, max_size);
  if (!newdata) {
    ELOG_V("Out of memory");
    return -1;
  }
  pqueue->data = (char *) newdata;

  // XXXX - free space, .... - data
  if (pqueue->begin_idx < pqueue->end_idx) {
    // [XXXX<begin>....<end>XXXX]
    // No need to move data
  } else {
    // begin > end
    // [....<end>XXXX<begin>....]
    // Move data after <begin> to new end of queue:
    int size_diff = max_size - pqueue->max_size;
    memmove(&pqueue->data[pqueue->begin_idx + size_diff],
      &pqueue->data[pqueue->begin_idx],
      pqueue->max_size - pqueue->begin_idx);

    pqueue->begin_idx = pqueue->begin_idx + size_diff;
  }
  pqueue->max_size = max_size;
  return 0;
}

#ifdef CPTCFG_IWLWAV_DEBUG
static void
errdump(char *pdata, unsigned int len)
{
  unsigned int i;
  for (i = 0; i < len; ++i) {
    fprintf(stderr, "(%x)", (unsigned char) pdata[i]);
  }
}

void
cqueue_debug_dump (const char *qname, cqueue_t *pqueue, int binary)
{
  int begin_idx;

  if (debug < 9)
    return;

  if (pqueue->begin_idx == pqueue->end_idx) {
    ILOG9_S("%s: queue is empty", qname);
    return;
  }

  ILOG9_S("%s:", qname);
  fprintf(stderr, "[[[");

  // XXXX - free space, .... - data
  begin_idx = pqueue->begin_idx;
  if (begin_idx > pqueue->end_idx) {
    // [....<end>XXXX<begin>....]
    int datalen = pqueue->max_size - begin_idx;
    if (datalen) {
      if (!binary) {
        fwrite(&pqueue->data[begin_idx],
          sizeof(char), datalen, stderr);
      } else {
        errdump(&pqueue->data[begin_idx], datalen);
      }
      begin_idx += datalen;
    }
    if (begin_idx == pqueue->max_size)
      begin_idx = 0;
  }

  if (begin_idx < pqueue->end_idx) {
    // [XXXX<begin>....<end>XXXX]
    int datalen =
      pqueue->end_idx - begin_idx;
    if (datalen) {
      if (!binary) {
        fwrite(&pqueue->data[begin_idx],
          sizeof(char), datalen, stderr);
      } else {
        errdump(&pqueue->data[begin_idx], datalen);
      }
      //begin_idx += datalen;
    }
  }

  fprintf(stderr, "]]]\n");
}
#endif // CPTCFG_IWLWAV_DEBUG

static int
begin_reached_end(cqueue_t *pqueue)
{
  if (pqueue->begin_idx == pqueue->end_idx)
    return 1;
  if (pqueue->begin_idx == pqueue->max_size && pqueue->end_idx == 0)
    return 1;
  return 0;
}

