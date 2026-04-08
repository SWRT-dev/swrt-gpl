/******************************************************************************

                               Copyright (c) 2012
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
/*
 * $Id$
 *
 * 
 *
 * Written by: Roman Sikorskyy
 *
 */

#ifndef __cache_h__
#define __cache_h__

#include "mtlkerr.h"
#include "mtlk_osal.h"
#include "mtlklist.h"
#include "frame.h"

#define MTLK_IDEFS_ON
#include "mtlkidefs.h"

typedef struct _cache_entry_t
{
    mtlk_slist_entry_t      link_entry;
    mtlk_osal_msec_t        local_timestamp;
    BOOL                    is_persistent;  //entry is not removed even if expired
    int                     freq;
    bss_data_t              bss;
} __MTLK_IDATA cache_entry_t;

typedef struct _scan_cache_t
{
    unsigned long           cache_expire; // in seconds
    mtlk_osal_spinlock_t    lock;
    cache_entry_t           *cur_entry;
    mtlk_slist_t            bss_list;
    mtlk_osal_spinlock_t    temp_bss_lock;
    bss_data_t              temp_bss_data;
    uint8                   modified;
    MTLK_DECLARE_INIT_STATUS;
} __MTLK_IDATA scan_cache_t;

int __MTLK_IFUNC mtlk_cache_init (scan_cache_t *cache, unsigned long expire);
void __MTLK_IFUNC mtlk_cache_cleanup (scan_cache_t *cache);

void __MTLK_IFUNC mtlk_cache_clear (scan_cache_t *cache);
uint32 __MTLK_IFUNC mtlk_cache_get_bss_count (scan_cache_t *cache, uint32 *storage_needed_for_ies);
void __MTLK_IFUNC mtlk_cache_rewind (scan_cache_t *cache);
uint8 __MTLK_IFUNC mtlk_cache_get_next_bss (scan_cache_t *cache, bss_data_t *bss, int *freq, unsigned long *timestamp);
uint8 __MTLK_IFUNC mtlk_cache_find_bss_by_bssid (scan_cache_t *cache, const IEEE_ADDR *bssid, bss_data_t *bss_data, int *freq);
void __MTLK_IFUNC mtlk_cache_delete_current (scan_cache_t *cache);

/* Clear all cache entries except the entry with specified bssid */
void __MTLK_IFUNC mtlk_cache_clear_except_entry (scan_cache_t *cache, const IEEE_ADDR *bssid);

uint8 __MTLK_IFUNC mtlk_cache_was_modified (scan_cache_t *cache);
void __MTLK_IFUNC mtlk_cache_clear_modified_flag(scan_cache_t *cache);

bss_data_t *__MTLK_IFUNC mtlk_cache_temp_bss_acquire(scan_cache_t *cache);
void __MTLK_IFUNC mtlk_cache_temp_bss_release(scan_cache_t *cache);

#define MTLK_IDEFS_OFF
#include "mtlkidefs.h"

#endif /* __cache_h__ */
