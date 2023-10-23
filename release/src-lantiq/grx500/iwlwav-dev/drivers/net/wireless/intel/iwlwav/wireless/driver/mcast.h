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
 * $Id$
 *
 * 
 *
 *  Originaly written by Artem Migaev
 *
 */

#ifndef _MCAST_H_
#define _MCAST_H_

#include "stadb.h"

/***************************************************/

#define MCAST_HASH_SIZE       (64)  /* must be power of 2 */

#define MC_MIN_GROUP          (1)   /* group id = 0 - is used in HD descriptors for broadcast packets */
#define MC_GROUPS             (MAX_NUM_OF_GROUPS)

#define MCAST_GROUP_UNDEF     (0xff)
#define MCAST_GROUP_BROADCAST (0x0)

typedef enum _mcast_sta_status_t {
  MCAST_STA_STATUS_NONE,
  MCAST_STA_STATUS_JOIN,
  MCAST_STA_STATUS_DELETE
} mcast_sta_status_t;

typedef enum _mcast_mode_t {
  MCAST_NO_MODE,
  MCAST_INCLUDE_MODE,
  MCAST_EXCLUDE_MODE
} mcast_mode_t;

typedef struct _mcast_sta_t {
  struct _mcast_sta_t   *next;
  IEEE_ADDR             sta_addr;
  IEEE_ADDR             host_addr;
  mcast_sta_status_t    status;
  uint32                vap_id;
} mcast_sta_t;

typedef struct _mcast_group_info_t {
  mcast_sta_t           *mcsta;           /*!< List of STA's subscribed to this group/source */
  uint8                 grp_id;           /*!< group id */
  mcast_mode_t          mode;             /*!< mode: INCLUDE/EXCLUDE */
} mcast_group_info_t;

typedef struct _mcast_ip4_src_t {
  struct _mcast_ip4_src_t *next;          /*!< Reference to a next entry */
  struct _mcast_ip4_grp_t *ip4_grp;       /*!< Reference to a mcast group structure */
  struct in_addr           ip4_src_addr;  /*!< Source address */
  mcast_group_info_t       info;
} mcast_ip4_src_t;

typedef struct _mcast_ip4_grp_t {
  struct _mcast_ip4_grp_t *next;          /*!< Reference to a next entry */
  struct _mcast_ip4_src_t *ip4_src_list;  /*!< List of multicast sources */
  struct in_addr           ip4_grp_addr;  /*!< Multicast group address   */
  mcast_group_info_t       info;
} mcast_ip4_grp_t;

typedef struct _mcast_ip6_src_t {
  struct _mcast_ip6_src_t *next;          /*!< Reference to a next entry */
  struct _mcast_ip6_grp_t *ip6_grp;       /*!< Reference to a mcast group structure */
  struct in6_addr          ip6_src_addr;  /*!< Source address */
  mcast_group_info_t       info;
} mcast_ip6_src_t;

typedef struct _mcast_ip6_grp_t {
  struct _mcast_ip6_grp_t *next;          /*!< Reference to a next entry */
  struct _mcast_ip6_src_t *ip6_src_list;  /*!< List of multicast sources */
  struct in6_addr          ip6_grp_addr;  /*!< Multicast group address   */
  mcast_group_info_t       info;
} mcast_ip6_grp_t;

typedef struct _mcast_ip4_range {
  mtlk_dlist_entry_t    list_entry;
  struct in_addr        ip4_addr;
  struct in_addr        ip4_mask;
} mcast_ip4_range;

typedef struct _mcast_ip6_range {
  mtlk_dlist_entry_t    list_entry;
  struct in6_addr       ip6_addr;
  struct in6_addr       ip6_mask;
} mcast_ip6_range;

typedef struct _mcast_ctx_t {
  mcast_ip4_grp_t *mcast_ip4_hash[MCAST_HASH_SIZE];
  mcast_ip6_grp_t *mcast_ip6_hash[MCAST_HASH_SIZE];
  mtlk_dlist_t  ip4_ranges_list;
  mtlk_dlist_t  ip6_ranges_list;
  mtlk_osal_spinlock_t lock;
  mtlk_osal_spinlock_t lock_ranges;
  MTLK_DECLARE_INIT_STATUS;
} mcast_ctx_t;

/* IGMP table definition for DF UI */

#ifdef CONFIG_WAVE_DEBUG
typedef enum {
  MTLK_MC_IPV4_GRP_ADDR,
  MTLK_MC_IPV4_SRC_ADDR,
  MTLK_MC_IPV6_GRP_ADDR,
  MTLK_MC_IPV6_SRC_ADDR,
  MTLK_MC_MAC_ADDR
} mtlk_mc_igmp_tbl_row_type_e;

typedef struct _mtlk_mc_igmp_tbl_item {
  mtlk_mc_igmp_tbl_row_type_e type;
} mtlk_mc_igmp_tbl_item_t;

typedef struct _mtlk_mc_igmp_tbl_ipv4_item {
  mtlk_mc_igmp_tbl_item_t header;
  struct in_addr          addr;
  unsigned                grp_id;
} mtlk_mc_igmp_tbl_ipv4_item_t;

typedef struct _mtlk_mc_igmp_tbl_ipv6_item {
  mtlk_mc_igmp_tbl_item_t header;
  struct in6_addr         addr;
  unsigned                grp_id;
} mtlk_mc_igmp_tbl_ipv6_item_t;

typedef struct _mtlk_mc_igmp_tbl_mac_item {
  mtlk_mc_igmp_tbl_item_t header;
  IEEE_ADDR               addr;
  IEEE_ADDR               host_addr;
  uint32                  vap_id;
} mtlk_mc_igmp_tbl_mac_item_t;
#endif

#include "core.h"
#include "mtlk_coreui.h"
#include "mtlk_clipboard.h"

#define LOG_LOCAL_GID   GID_MCAST
#define LOG_LOCAL_FID   0

int  mtlk_mc_parse (mtlk_core_t* nic, mtlk_nbuf_t *nbuf, const IEEE_ADDR *sta_addr, uint32 vap_id)
  __MTLK_INT_HANDLER_SECTION;
int  mtlk_mc_transmit (mtlk_core_t* nic, mtlk_core_handle_tx_data_t *tx_data, uint32 nbuf_flags, int bridge_mode, sta_entry *src_sta)
  __MTLK_INT_HANDLER_SECTION;
void mtlk_mc_drop_sta (mtlk_core_t *nic, const IEEE_ADDR *sta_addr, uint32 vap_id);
#ifdef CONFIG_WAVE_DEBUG
int  mtlk_mc_dump_groups (mtlk_core_t *nic, mtlk_clpb_t *clpb);
#endif
int  mtlk_mc_init (mtlk_core_t *nic);
void mtlk_mc_uninit (mtlk_core_t *nic);
void mtlk_mc_update_group_id_sta (mtlk_core_t *nic, uint32 grid, mc_action_t action, mtlk_mc_addr_t *mc_addr, sta_entry *sta);
void mtlk_mc_update_stadb (mtlk_core_t *nic, mtlk_core_ui_mc_update_sta_db_t *req);

int mtlk_mc_get_grid(mtlk_core_t *nic, mtlk_nbuf_t *nbuf)
    __MTLK_INT_HANDLER_SECTION;

BOOL __MTLK_IFUNC wv_is_dhcpv6(mtlk_nbuf_t *nbuf);

BOOL __MTLK_IFUNC
wv_check_dgaf_disabled_packet(mtlk_nbuf_t *nbuf);
BOOL __MTLK_IFUNC
wv_is_na_ns(mtlk_nbuf_t *nbuf);
const struct in6_addr *__MTLK_IFUNC
wv_is_unsolic_na(mtlk_nbuf_t *nbuf);

int  __MTLK_IFUNC mtlk_mc_ranges_add (mtlk_core_t *nic, mtlk_ip_netmask_t *netmask);
int  __MTLK_IFUNC mtlk_mc_ranges_del (mtlk_core_t *nic, mtlk_ip_netmask_t *netmask);
int  __MTLK_IFUNC mtlk_mc_ranges_cleanup (mtlk_core_t *nic, ip_type_t type);
int  __MTLK_IFUNC mtlk_mc_ranges_get_vect (mtlk_core_t *nic, ip_type_t ip_type, mtlk_mcast_range_vect_cfg_t *mcast_range_vect_cfg);

#undef LOG_LOCAL_GID
#undef LOG_LOCAL_FID

#endif // _MCAST_H_
