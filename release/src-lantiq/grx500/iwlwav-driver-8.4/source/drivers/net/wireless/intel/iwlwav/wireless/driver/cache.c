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
#include "mtlkinc.h"

#include "cache.h"

#define LOG_LOCAL_GID   GID_CACHE
#define LOG_LOCAL_FID   1

static void
_cache_free_entry(cache_entry_t *slot)
{
  mtlk_osal_mem_free(slot);
}

/*************************************************************************************
**                                                                                  **
** NAME         mtlk_cache_init                                                     **
**                                                                                  **
** PARAMETERS                                                                       **
**                                                                                  **
**                                                                                  **
*************************************************************************************/

MTLK_INIT_STEPS_LIST_BEGIN(cache)
  MTLK_INIT_STEPS_LIST_ENTRY(cache, SLIST_INIT)
  MTLK_INIT_STEPS_LIST_ENTRY(cache, LOCK_INIT)
  MTLK_INIT_STEPS_LIST_ENTRY(cache, BSS_LOCK_INIT)
  MTLK_INIT_STEPS_LIST_ENTRY(cache, BSS_CACHE_INIT)
MTLK_INIT_INNER_STEPS_BEGIN(cache)
MTLK_INIT_STEPS_LIST_END(cache);

int __MTLK_IFUNC
mtlk_cache_init (scan_cache_t *cache, unsigned long expire)
{
  MTLK_INIT_TRY(cache, MTLK_OBJ_PTR(cache))
    MTLK_INIT_STEP_VOID(cache, SLIST_INIT, MTLK_OBJ_PTR(cache), 
                        mtlk_slist_init, (&cache->bss_list));
    MTLK_INIT_STEP(cache, LOCK_INIT, MTLK_OBJ_PTR(cache), 
                   mtlk_osal_lock_init, (&cache->lock));
    MTLK_INIT_STEP(cache, BSS_LOCK_INIT, MTLK_OBJ_PTR(cache), 
                   mtlk_osal_lock_init, (&cache->temp_bss_lock));
    MTLK_INIT_STEP_VOID(cache, BSS_CACHE_INIT, MTLK_OBJ_PTR(cache),
                        MTLK_NOACTION, ());

    cache->cache_expire = expire;
    
  MTLK_INIT_FINALLY(cache, MTLK_OBJ_PTR(cache))
  MTLK_INIT_RETURN(cache, MTLK_OBJ_PTR(cache), mtlk_cache_cleanup, (cache));
}

void __MTLK_IFUNC
mtlk_cache_cleanup (scan_cache_t *cache)
{
  MTLK_CLEANUP_BEGIN(cache, MTLK_OBJ_PTR(cache))
  
    MTLK_CLEANUP_STEP(cache, BSS_CACHE_INIT, MTLK_OBJ_PTR(cache),
                      mtlk_cache_clear, (cache));
    MTLK_CLEANUP_STEP(cache, BSS_LOCK_INIT, MTLK_OBJ_PTR(cache),
                      mtlk_osal_lock_cleanup, (&cache->temp_bss_lock));
    MTLK_CLEANUP_STEP(cache, LOCK_INIT, MTLK_OBJ_PTR(cache),
                      mtlk_osal_lock_cleanup, (&cache->lock));
    MTLK_CLEANUP_STEP(cache, SLIST_INIT, MTLK_OBJ_PTR(cache),
                      mtlk_slist_cleanup, (&cache->bss_list));
  MTLK_CLEANUP_END(cache, MTLK_OBJ_PTR(cache));
}

void __MTLK_IFUNC
mtlk_cache_clear (scan_cache_t *cache)
{
    mtlk_slist_entry_t *list_entry_to_delete = NULL;
    cache_entry_t *cache_entry_to_delete = NULL;

    mtlk_osal_lock_acquire(&cache->lock);

    while ((list_entry_to_delete = mtlk_slist_pop(&cache->bss_list)) != NULL)
    {
        cache_entry_to_delete = MTLK_LIST_GET_CONTAINING_RECORD(list_entry_to_delete, cache_entry_t, link_entry);
        _cache_free_entry(cache_entry_to_delete);
    }
    
    cache->modified = 1;
    cache->cur_entry = NULL;

    mtlk_osal_lock_release(&cache->lock);
}

/* Clear all cache entries except the entry with specified bssid */
void __MTLK_IFUNC
mtlk_cache_clear_except_entry (scan_cache_t *cache, const IEEE_ADDR *bssid)
{
    BOOL found = FALSE;
    mtlk_slist_entry_t *list_entry_to_delete = NULL;
    mtlk_slist_entry_t *list_entry_to_save   = NULL;
    cache_entry_t *cache_entry_to_delete = NULL;
    cache_entry_t *cache_entry_to_save   = NULL;

    MTLK_ASSERT(NULL != bssid);

    mtlk_osal_lock_acquire(&cache->lock);

    while ((list_entry_to_delete = mtlk_slist_pop(&cache->bss_list)) != NULL)
    {
        cache_entry_to_delete = MTLK_LIST_GET_CONTAINING_RECORD(list_entry_to_delete, cache_entry_t, link_entry);

        if (!ieee_addr_compare(&cache_entry_to_delete->bss.bssid, bssid))
        {
            found = TRUE;
            list_entry_to_save  = list_entry_to_delete;
            cache_entry_to_save = cache_entry_to_delete;
        }
        else
        {
            _cache_free_entry(cache_entry_to_delete);
        }
    }

    if (found)
    {
        mtlk_slist_push(&cache->bss_list, list_entry_to_save);
        cache->cur_entry = cache_entry_to_save;
    }
    else
    {
        cache->cur_entry = NULL;
    }

    cache->modified = 1;

    mtlk_osal_lock_release(&cache->lock);
}

void __MTLK_IFUNC
mtlk_cache_rewind (scan_cache_t *cache)
{
    mtlk_slist_entry_t *current_list_entry;

    mtlk_osal_lock_acquire(&cache->lock);

    current_list_entry = mtlk_slist_begin(&cache->bss_list);
    if (current_list_entry)
    {
        cache->cur_entry = MTLK_LIST_GET_CONTAINING_RECORD(current_list_entry, cache_entry_t, link_entry);
    }
    else
    {
        cache->cur_entry = NULL;
    }

    mtlk_osal_lock_release(&cache->lock);
}

uint8 __MTLK_IFUNC
mtlk_cache_get_next_bss (scan_cache_t *cache, bss_data_t *bss, int *freq, unsigned long *timestamp)
{
    uint8 res = 0;
    mtlk_slist_entry_t *prev_list_entry = NULL;
    mtlk_slist_entry_t *next_list_entry = NULL;
    mtlk_osal_msec_t cur_time = mtlk_osal_timestamp_to_ms(mtlk_osal_timestamp());

    mtlk_osal_lock_acquire(&cache->lock);

    if (cache->cur_entry)
    {
        prev_list_entry = mtlk_slist_head(&cache->bss_list);
        while (prev_list_entry)
        {
            next_list_entry = mtlk_slist_next(prev_list_entry);
            if (next_list_entry == &cache->cur_entry->link_entry)
            {
                break;
            }
            prev_list_entry = next_list_entry;
        }
    }

    if (!prev_list_entry)
    {
        cache->cur_entry = NULL;    
        /* cache->cur_entry was not found in the list 
           it shouldn't normally happen, but we check
           for it just in case
        */
    }

    while (cache->cur_entry && !res)
    {
        mtlk_slist_entry_t *last_valid_entry = prev_list_entry;
        if (cache->cache_expire && 
            ((cur_time - cache->cur_entry->local_timestamp) > cache->cache_expire * 1000) && /* expire is in sec, timestamp is in ms */
            !cache->cur_entry->is_persistent)
        {
            ILOG3_YD("Expired cache entry removed: %Y (ch = %d)",
              cache->cur_entry->bss.bssid.au8Addr, cache->cur_entry->bss.channel);
            mtlk_slist_remove_next(&cache->bss_list, prev_list_entry);
            cache->modified = 1;
            /* prev_list_entry remains the same, but it now points to the next list element */

            _cache_free_entry(cache->cur_entry);
        }
        else
        {
            ILOG3_YD("Cache entry copied to user buffer: %Y (ch = %d)",
              cache->cur_entry->bss.bssid.au8Addr, cache->cur_entry->bss.channel);
            if (bss)
            {
              // It's possible that we are not interested in the actual data but have rather called
              // this function to remove the outdated entries from the ap database and to count the
              // remaining valid entries
                *bss = cache->cur_entry->bss;
            }
            if (freq)
            {
                *freq = cache->cur_entry->freq;
            }
            if (timestamp)
            {
                *timestamp = cache->cur_entry->local_timestamp;
            }

            last_valid_entry = &cache->cur_entry->link_entry;
            res = 1;
        }
        next_list_entry = mtlk_slist_next(last_valid_entry);
        if (next_list_entry)
        {
            cache->cur_entry = MTLK_LIST_GET_CONTAINING_RECORD(next_list_entry, cache_entry_t, link_entry);
        }
        else
        {
            cache->cur_entry = NULL;
        }
    }

    mtlk_osal_lock_release(&cache->lock);

    return res;
}

uint32 __MTLK_IFUNC
mtlk_cache_get_bss_count(scan_cache_t *cache, uint32 *storage_needed_for_ies)
{
    uint32 number_of_aps = 0;
    bss_data_t *bss_data;

    if (storage_needed_for_ies)
    {
        *storage_needed_for_ies = 0;
    }

    bss_data = mtlk_cache_temp_bss_acquire(cache);

    mtlk_cache_rewind(cache);
    while (mtlk_cache_get_next_bss(cache, bss_data, NULL, NULL))
    {
        number_of_aps ++;
        if (storage_needed_for_ies)
        {
            *storage_needed_for_ies += bss_data->ie_used_length;
        }
    }

    mtlk_cache_temp_bss_release(cache);

    return number_of_aps;
}

uint8 __MTLK_IFUNC
mtlk_cache_find_bss_by_bssid(scan_cache_t *cache, const IEEE_ADDR *bssid, bss_data_t *bss_data, int *freq)
{
    uint8 found = 0;

    mtlk_cache_rewind(cache);
    while (mtlk_cache_get_next_bss(cache, bss_data, freq, NULL))
    {
        if (!ieee_addr_compare(bssid, &bss_data->bssid))
        {
            found = 1;
            break;
        }
    }

    return found;
}

void __MTLK_IFUNC
mtlk_cache_delete_current (scan_cache_t *cache)
{
    if (cache->cur_entry)
    {
        mtlk_slist_entry_t *prev_list_entry = mtlk_slist_head(&cache->bss_list);
        while (prev_list_entry)
        {
            mtlk_slist_entry_t *next_list_entry = mtlk_slist_next(prev_list_entry);
            if (next_list_entry == &cache->cur_entry->link_entry)
            {
                break;
            }
            prev_list_entry = next_list_entry;
        }

        if (prev_list_entry)
        {
            ILOG4_Y("BSS removed from cache: %Y", cache->cur_entry->bss.bssid.au8Addr);
            mtlk_slist_remove_next(&cache->bss_list, prev_list_entry);
            cache->modified = 1;
            _cache_free_entry(cache->cur_entry);
        }
        cache->cur_entry = NULL;
    }
}

/*****************************************************************************
**
** NAME         mtlk_cache_temp_bss_acquire
**
** PARAMETERS   cache           cache pointer
**              lock_value      pointer to location that receives the value
**                              to be passed later to mtlk_cache_temp_bss_release
**
** RETURNS      pointer to the temporary structure of bss_data_t type
**
** DESCRIPTION  Locks the spinlock that protects the temporary bss_data structure
**              and returns pointer to that structure
**
******************************************************************************/
bss_data_t *__MTLK_IFUNC
mtlk_cache_temp_bss_acquire(scan_cache_t *cache)
{
    mtlk_osal_lock_acquire(&cache->temp_bss_lock);
    return &cache->temp_bss_data;
}

/*****************************************************************************
**
** NAME         mtlk_cache_temp_bss_release
**
** PARAMETERS   cache           cache pointer
**              lock_value      The value returned in lock_value by the call to
**                              mtlk_scan_temp_bss_acquire
**
** RETURNS      none
**
** DESCRIPTION  Unlocks the spinlock that protects the temporary bss_data structure
**
******************************************************************************/
void __MTLK_IFUNC
mtlk_cache_temp_bss_release(scan_cache_t *cache)
{
    mtlk_osal_lock_release(&cache->temp_bss_lock);
}

/*****************************************************************************
**
** NAME         mtlk_cache_was_modified
**
** PARAMETERS   cache           cache pointer
**             
** RETURNS      uint32          if a new entry was entered to removed from cache
**
** DESCRIPTION  this function is an API to check if the content of cache was modified.
**
******************************************************************************/
uint8 __MTLK_IFUNC 
mtlk_cache_was_modified (scan_cache_t *cache)
{
    uint8 ret_val = 0;
    if(cache)
    {
        mtlk_osal_lock_acquire(&cache->lock);

        ret_val = cache->modified;

        mtlk_osal_lock_release(&cache->lock);
    }
    return ret_val;
}

/*****************************************************************************
**
** NAME         mtlk_cache_clear_modified_flag
**
** PARAMETERS   cache           cache pointer
**
** RETURNS      none
**
** DESCRIPTION  clears the modified flag; should be used once it was read.
**
******************************************************************************/
void __MTLK_IFUNC 
mtlk_cache_clear_modified_flag(scan_cache_t *cache)
{
    if(cache)
    {
        mtlk_osal_lock_acquire(&cache->lock);

        cache->modified = 0;

        mtlk_osal_lock_release(&cache->lock);
    }    
}
