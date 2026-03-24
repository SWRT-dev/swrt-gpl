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
 * Hotspot 2.0 PPF interface.
 * Modules performing ARP proxy, L2 firewall and Disable DGAF functions
 * will register with us using this API.
 *
 */

#include "mtlkinc.h"

#include "mtlk_hs20_api.h"
#include "mtlk_hs20.h"

#define LOG_LOCAL_GID   GID_HS20
#define LOG_LOCAL_FID   1

/* Private */

mtlk_osal_spinlock_t hs20_lock;

wave_wifi_parp_if_manage_t     g_wave_wifi_parp_if_manage;
wave_wifi_parp_client_manage_t g_wave_wifi_parp_client_manage;
wave_wifi_parp_lookup_t        g_wave_wifi_parp_lookup;
wave_wifi_parp_dump_t          g_wave_wifi_parp_dump;

wave_wifi_l2f_xmit_t           g_wave_wifi_l2f_xmit;
wave_wifi_ppa_l2f_xmit_t       g_wave_wifi_ppa_l2f_xmit;

mtlk_hs20_notify_t   hs20_notify_callback = NULL;
mtlk_osal_spinlock_t hs20_notify_lock;
mtlk_dlist_t         hs20_notify_list;

static void _mtlk_hs20_cleanup_callbacks(void)
{
  g_wave_wifi_parp_if_manage     = NULL;
  g_wave_wifi_parp_client_manage = NULL;
  g_wave_wifi_parp_lookup        = NULL;
  g_wave_wifi_parp_dump          = NULL;

  g_wave_wifi_l2f_xmit           = NULL;
  g_wave_wifi_ppa_l2f_xmit       = NULL;
}

int __MTLK_IFUNC
mtlk_hs20_init(void)
{
  _mtlk_hs20_cleanup_callbacks();
  mtlk_osal_lock_init(&hs20_lock);

  mtlk_osal_lock_init(&hs20_notify_lock);
  mtlk_dlist_init(&hs20_notify_list);

  return MTLK_ERR_OK;
}

void __MTLK_IFUNC
mtlk_hs20_cleanup(void)
{
  mtlk_osal_lock_cleanup(&hs20_notify_lock);
  mtlk_dlist_cleanup(&hs20_notify_list);

  mtlk_osal_lock_cleanup(&hs20_lock);
  _mtlk_hs20_cleanup_callbacks();
}

void __MTLK_IFUNC mtlk_parp_notify_disconnect(char *ifname, unsigned char *mac)
{
  mtlk_osal_lock_acquire(&hs20_lock);
  if(g_wave_wifi_parp_client_manage)
  {
    (void) g_wave_wifi_parp_client_manage(WAVE_WIFI_PARP_STA_DEL, ifname, mac, 0, NULL);
  }
  mtlk_osal_lock_release(&hs20_lock);
}

void* __MTLK_IFUNC mtlk_hs20_get_netdev_callback(void)
{
  void *rv = NULL;

  mtlk_osal_lock_acquire(&hs20_lock);
  rv = g_wave_wifi_l2f_xmit;
  mtlk_osal_lock_release(&hs20_lock);

  return rv;
}
#ifdef UNUSED_CODE
void* __MTLK_IFUNC mtlk_hs20_get_ppa_callback(void)
{
  void *rv = NULL;

  mtlk_osal_lock_acquire(&hs20_lock);
  rv = g_wave_wifi_ppa_l2f_xmit;
  mtlk_osal_lock_release(&hs20_lock);

  return rv;
}
#endif /* UNUSED_CODE */
static mtlk_hs20_notify_entry_t* find_notify_entry(mtlk_hs20_notify_entry_t *hs20_notify_entry)
{
  mtlk_dlist_entry_t *notify_list_entry, *notify_list_head;
  mtlk_hs20_notify_entry_t    *notify_entry = NULL;

  notify_list_head = mtlk_dlist_head(&hs20_notify_list);
  notify_list_entry = mtlk_dlist_next(notify_list_head);

  while(notify_list_entry != notify_list_head)
  {
    notify_entry = MTLK_LIST_GET_CONTAINING_RECORD(notify_list_entry, mtlk_hs20_notify_entry_t, list_entry);
    if(notify_entry == hs20_notify_entry)
    {
      return notify_entry;
    }
    notify_list_entry = mtlk_dlist_next(notify_list_entry);
  }

  return NULL;
}

void __MTLK_IFUNC mtlk_hs20_notify_register(mtlk_hs20_notify_t callback, mtlk_hs20_notify_entry_t *hs20_notify_entry)
{
  hs20_notify_callback = callback;

  mtlk_osal_lock_acquire(&hs20_notify_lock);

  if(find_notify_entry(hs20_notify_entry))
  {
    ELOG_V("The interface is already registered");
  }
  else
  {
    mtlk_dlist_push_back(&hs20_notify_list, &hs20_notify_entry->list_entry);
  }

  mtlk_osal_lock_release(&hs20_notify_lock);
}

void __MTLK_IFUNC mtlk_hs20_notify_unregister(mtlk_hs20_notify_entry_t *hs20_notify_entry)
{
  mtlk_hs20_notify_entry_t *notify_entry;

  mtlk_osal_lock_acquire(&hs20_notify_lock);
  notify_entry = find_notify_entry(hs20_notify_entry);
  if(notify_entry)
  {
    mtlk_dlist_remove(&hs20_notify_list, &hs20_notify_entry->list_entry);
  }
  else
  {
    ILOG1_V("The interface is not registered");
  }
  mtlk_osal_lock_release(&hs20_notify_lock);
}

/* Public: to be called from wave_wifi_l2f.ko */

static void iterate_notify_list(void)
{
  mtlk_dlist_entry_t *notify_list_entry, *notify_list_head;
  mtlk_hs20_notify_entry_t    *notify_entry = NULL;

  notify_list_head = mtlk_dlist_head(&hs20_notify_list);
  notify_list_entry = mtlk_dlist_next(notify_list_head);

  while(notify_list_entry != notify_list_head)
  {
    notify_entry = MTLK_LIST_GET_CONTAINING_RECORD(notify_list_entry, mtlk_hs20_notify_entry_t, list_entry);
    notify_list_entry = mtlk_dlist_next(notify_list_entry);

    hs20_notify_callback(notify_entry->data);
  }
}

int mtlk_parp_register(
  wave_wifi_parp_if_manage_t     p_wave_wifi_parp_if_manage,
  wave_wifi_parp_client_manage_t p_wave_wifi_parp_client_manage,
  wave_wifi_parp_lookup_t        p_wave_wifi_parp_lookup,
  wave_wifi_parp_dump_t          p_wave_wifi_parp_dump,
  int                            flag
)
{
  int res = -EFAULT;

  mtlk_osal_lock_acquire(&hs20_lock);
  if(flag == WAVE_WIFI_REG)
  {
    g_wave_wifi_parp_if_manage     = p_wave_wifi_parp_if_manage;
    g_wave_wifi_parp_client_manage = p_wave_wifi_parp_client_manage;
    g_wave_wifi_parp_lookup        = p_wave_wifi_parp_lookup;
    g_wave_wifi_parp_dump          = p_wave_wifi_parp_dump;
    res = 0;
  }
  else if(flag == WAVE_WIFI_UNREG)
  {
    g_wave_wifi_parp_if_manage     = NULL;
    g_wave_wifi_parp_client_manage = NULL;
    g_wave_wifi_parp_lookup        = NULL;
    g_wave_wifi_parp_dump          = NULL;
    res = 0;
  }
  else
  {
    ELOG_D("Invalid flag value: %d", flag);
  }
  mtlk_osal_lock_release(&hs20_lock);

  return res;
}
EXPORT_SYMBOL(mtlk_parp_register);

int mtlk_hs2_register(
  wave_wifi_l2f_xmit_t     p_wave_wifi_l2f_xmit,
  wave_wifi_ppa_l2f_xmit_t p_wave_wifi_ppa_l2f_xmit,
  int                      flag
)
{
  int res = -EFAULT;

  if(flag == WAVE_WIFI_REG)
  {
    mtlk_osal_lock_acquire(&hs20_lock);
    g_wave_wifi_l2f_xmit     = p_wave_wifi_l2f_xmit;
    g_wave_wifi_ppa_l2f_xmit = p_wave_wifi_ppa_l2f_xmit;
    mtlk_osal_lock_release(&hs20_lock);
    res = 0;

    iterate_notify_list();
  }
  else if(flag == WAVE_WIFI_UNREG)
  {
    mtlk_osal_lock_acquire(&hs20_lock);
    g_wave_wifi_l2f_xmit     = NULL;
    g_wave_wifi_ppa_l2f_xmit = NULL;
    mtlk_osal_lock_release(&hs20_lock);
    res = 0;

    iterate_notify_list();
  }
  else
  {
    ELOG_D("Invalid flag value: %d", flag);
  }

  return res;
}
EXPORT_SYMBOL(mtlk_hs2_register);
