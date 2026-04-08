/******************************************************************************

                               Copyright (c) 2014
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
/*
 *
 * Hotspot 2.0 PPF interface.
 * Modules performing ARP proxy, L2 firewall and Disable DGAF functions
 * will register with us using this API.
 *
 * Private header file.
 *
 */


#ifndef __MTLK_HS20_H__
#define __MTLK_HS20_H__

#define   MTLK_IDEFS_ON
#include "mtlkidefs.h"

int __MTLK_IFUNC mtlk_hs20_init(void);
void __MTLK_IFUNC mtlk_hs20_cleanup(void);

void __MTLK_IFUNC mtlk_parp_notify_disconnect(char *ifname, unsigned char *mac);
void* __MTLK_IFUNC mtlk_hs20_get_netdev_callback(void);
void* __MTLK_IFUNC mtlk_hs20_get_ppa_callback(void);

typedef struct
{
  void               *data;
  mtlk_dlist_entry_t list_entry;
} mtlk_hs20_notify_entry_t;

typedef void (*mtlk_hs20_notify_t)(
  void *data
);

void __MTLK_IFUNC mtlk_hs20_notify_register(mtlk_hs20_notify_t callback, mtlk_hs20_notify_entry_t *hs20_notify_entry);
void __MTLK_IFUNC mtlk_hs20_notify_unregister(mtlk_hs20_notify_entry_t *hs20_notify_entry);

#define   MTLK_IDEFS_OFF
#include "mtlkidefs.h"

#endif /* __MTLK_HS20_H__ */
