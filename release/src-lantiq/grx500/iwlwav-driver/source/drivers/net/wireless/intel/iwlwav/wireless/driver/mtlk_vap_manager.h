/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#ifndef __MTLK_VAP_MANAGER_H__
#define __MTLK_VAP_MANAGER_H__

#include "mtlkdfdefs.h"
#include "mtlkirbd.h"
#include "mtlk_wss.h"
#include "mtlk_card_types.h"
#include "txmm.h"

#define   MTLK_IDEFS_ON
#include "mtlkidefs.h"

#define LOG_LOCAL_GID   GID_VAPM
#define LOG_LOCAL_FID   1



#define MTLK_INVALID_VAP_HANDLE ((mtlk_vap_handle_t)0)
#define MTLK_VAP_INVALID_IDX ((uint8)-1)

typedef enum {
  MTLK_MODE_NON_DUT,
  MTLK_MODE_DUT
} mtlk_work_mode_e;

typedef enum {
  MTLK_ROLE_AP,
  MTLK_ROLE_STA
} mtlk_work_role_e;


struct _mtlk_mmb_drv_t;

/* VAP Manager API */
mtlk_vap_manager_t * __MTLK_IFUNC mtlk_vap_manager_create(void *radio, unsigned radio_id, mtlk_hw_api_t *hw_api, mtlk_work_mode_e work_mode);
void                 __MTLK_IFUNC mtlk_vap_manager_delete(mtlk_vap_manager_t* obj);

int                  __MTLK_IFUNC mtlk_vap_manager_create_vap(mtlk_vap_manager_t *obj,
                                                              uint8 vap_index,
                                                              const char *name,
                                                              mtlk_work_role_e role,
                                                              BOOL is_master,
                                                              struct net_device* ndev);

mtlk_error_t         __MTLK_IFUNC mtlk_vap_manager_get_free_vap_index(mtlk_vap_manager_t *obj, uint32 *vap_index);
mtlk_error_t         __MTLK_IFUNC mtlk_vap_manager_check_free_vap_index(mtlk_vap_manager_t *obj, uint32 vap_index);

void                 __MTLK_IFUNC mtlk_vap_manager_set_master_ndev_taken (mtlk_vap_manager_t *obj, BOOL master_ndev_taken);
BOOL                 __MTLK_IFUNC mtlk_vap_manager_get_master_ndev_taken (mtlk_vap_manager_t *obj);
BOOL                 __MTLK_IFUNC mtlk_vap_manager_did_ap_started (mtlk_vap_manager_t *obj);

int                  __MTLK_IFUNC mtlk_vap_manager_prepare_vaps(mtlk_vap_manager_t *obj);
void                 __MTLK_IFUNC mtlk_vap_manager_deallocate_vaps(mtlk_vap_manager_t *obj);

void                 __MTLK_IFUNC mtlk_vap_manager_prepare_stop(mtlk_vap_manager_t *obj);
void                 __MTLK_IFUNC mtlk_vap_manager_stop_all_vaps(mtlk_vap_manager_t *obj);
int                  __MTLK_IFUNC mtlk_vap_manager_get_master_vap(mtlk_vap_manager_t *obj,
                                                                  mtlk_vap_handle_t  *vap_handle)   __MTLK_INT_HANDLER_SECTION;
int                  __MTLK_IFUNC mtlk_vap_manager_get_vap_handle(mtlk_vap_manager_t *obj,
                                                                  uint8               vap_id,
                                                                  mtlk_vap_handle_t  *vap_handle)   __MTLK_INT_HANDLER_SECTION;
int                  __MTLK_IFUNC mtlk_vap_manager_get_vap_handle_by_secondary_id (mtlk_vap_manager_t *obj,
                                                                                   uint8               vap_id,
                                                                                   mtlk_vap_handle_t  *vap_handle) __MTLK_INT_HANDLER_SECTION;

BOOL                  __MTLK_IFUNC mtlk_vap_manager_vap_is_not_ready(mtlk_vap_manager_t *obj,
                                                                  uint8               vap_id);
mtlk_hw_api_t *      __MTLK_IFUNC mtlk_vap_manager_get_hwapi(mtlk_vap_manager_t *obj)               __MTLK_INT_HANDLER_SECTION;
mtlk_work_mode_e     __MTLK_IFUNC mtlk_vap_manager_get_mode(mtlk_vap_manager_t *obj)                __MTLK_INT_HANDLER_SECTION;
void __MTLK_IFUNC
mtlk_vap_manager_notify_vap_activated(mtlk_vap_manager_t *obj);
void __MTLK_IFUNC
mtlk_vap_manager_notify_vap_deactivated(mtlk_vap_manager_t *obj);
uint32 __MTLK_IFUNC
mtlk_vap_manager_get_active_vaps_number(mtlk_vap_manager_t *obj);
void __MTLK_IFUNC
mtlk_vap_manager_set_active_vaps_number(mtlk_vap_manager_t *obj, uint32 num);
uint32 __MTLK_IFUNC
mtlk_vap_manager_get_master_vap_index(mtlk_vap_manager_t *obj);
uint32 __MTLK_IFUNC
mtlk_vap_manager_get_max_vaps_count(mtlk_vap_manager_t *obj);

mtlk_hw_t* __MTLK_IFUNC
mtlk_vap_manager_get_hw(mtlk_vap_manager_t *obj);

int __MTLK_IFUNC
mtlk_vap_manager_rcvry_start_all_subifs(mtlk_vap_manager_t  *obj);
void __MTLK_IFUNC
mtlk_vap_manager_rcvry_stop_all_subifs(mtlk_vap_manager_t  *obj);

struct _mtlk_abmgr_t;
struct _mtlk_core_api_t;
struct _mtlk_core_vft_t;
typedef struct _wave_radio_t wave_radio_t;

typedef uint8   wave_vap_id_t;

/* VAP API */
typedef struct _mtlk_vap_info_internal_t
{
  wave_vap_id_t           id;    /* Vap ID unique per Vap manager */
  wave_vap_id_t           id_fw; /* Vap ID in FW unique per HW card */
  uint8                   hw_idx;
  uint8                   radio_id;
  uint8                   radio_id_txmm;
  uint8                   ep_bit;
  mtlk_vap_manager_t      *manager;
  struct _mtlk_core_api_t *core_api;
  mtlk_df_t               *df;
  mtlk_df_t               **secondary_dfs;
  mtlk_pdb_t              *param_db;
  mtlk_txmm_t             txmm;
  mtlk_txmm_t             txdm;
  struct _mtlk_abmgr_t    *abmgr;
  mtlk_irbd_t             *irbd;
  wave_radio_t            *radio;
  mtlk_work_role_e        role;
  BOOL                    is_master;
  int32                   dcdp_port;

  MTLK_DECLARE_INIT_STATUS;
  MTLK_DECLARE_START_STATUS;
} __MTLK_IDATA mtlk_vap_info_internal_t;

static __INLINE int32
wave_vap_dcdp_port_get(mtlk_vap_handle_t vap_handle)
{
  mtlk_vap_info_internal_t *_info = (mtlk_vap_info_internal_t *)vap_handle;

  MTLK_ASSERT(NULL != _info);

  return _info->dcdp_port;
}

static __INLINE void
wave_vap_dcdp_port_set(mtlk_vap_handle_t vap_handle, int32 port_id)
{
  mtlk_vap_info_internal_t *_info = (mtlk_vap_info_internal_t *)vap_handle;

  MTLK_ASSERT(NULL != _info);

  _info->dcdp_port = port_id;
}

struct _mtlk_vap_handle_t __MTLK_IDATA
{
  uint8 internal[sizeof(mtlk_vap_info_internal_t)];
};

static __INLINE uint16
mtlk_vap_get_oid(mtlk_vap_handle_t vap_handle)
{
  mtlk_vap_info_internal_t *_info = (mtlk_vap_info_internal_t *)vap_handle;

  MTLK_ASSERT(NULL != _info);

  return ( ((uint16)_info->hw_idx) << 12) | (_info->radio_id << 8) | _info->id;
}

static __INLINE mtlk_vap_manager_t *
mtlk_vap_get_manager (mtlk_vap_handle_t vap_handle)
{
  mtlk_vap_info_internal_t *_info = (mtlk_vap_info_internal_t *)vap_handle;

  MTLK_ASSERT(NULL != _info);
  MTLK_ASSERT(NULL != _info->manager);

  return _info->manager;
}

static __INLINE mtlk_df_t *
mtlk_vap_get_df (mtlk_vap_handle_t vap_handle)
{
  mtlk_vap_info_internal_t *_info = (mtlk_vap_info_internal_t *)vap_handle;

  MTLK_ASSERT(NULL != _info);
  MTLK_ASSERT(NULL != _info->df);

  return _info->df;
}

static __INLINE mtlk_df_t *
mtlk_vap_get_secondary_df (mtlk_vap_handle_t vap_handle, uint8 secondary_vap_id)
{
  mtlk_vap_info_internal_t *_info = (mtlk_vap_info_internal_t *)vap_handle;

  MTLK_ASSERT(NULL != _info);
  MTLK_ASSERT(NULL != _info->secondary_dfs);

  return _info->secondary_dfs[secondary_vap_id];
}

static __INLINE BOOL
mtlk_vap_is_master (mtlk_vap_handle_t vap_handle)
{
  mtlk_vap_info_internal_t *_info = (mtlk_vap_info_internal_t *)vap_handle;

  MTLK_ASSERT(_info != NULL);

  /* NOTE: don't use the mtlk_vap_get_id API here since this function
   *       can be called in context of DF/Core creation, i.e.
   *       prior to manager data member assignment.
   */
  return (_info->is_master);
}

static __INLINE BOOL
mtlk_vap_is_ap (mtlk_vap_handle_t vap_handle)
{
  mtlk_vap_info_internal_t *_info = (mtlk_vap_info_internal_t *)vap_handle;

  MTLK_ASSERT(_info != NULL);
  return (_info->role != MTLK_ROLE_STA);
}

static __INLINE BOOL
mtlk_vap_is_sta(mtlk_vap_handle_t vap_handle)
{
  mtlk_vap_info_internal_t *_info = (mtlk_vap_info_internal_t *)vap_handle;

  MTLK_ASSERT(_info != NULL);
  return (_info->role != MTLK_ROLE_AP);
}

static __INLINE BOOL
mtlk_vap_is_master_ap (mtlk_vap_handle_t vap_handle)
{
  return (BOOL)(mtlk_vap_is_ap(vap_handle) && mtlk_vap_is_master(vap_handle));
}

static __INLINE BOOL
mtlk_vap_is_slave_ap (mtlk_vap_handle_t vap_handle)
{
  return (BOOL)(mtlk_vap_is_ap(vap_handle) && !mtlk_vap_is_master(vap_handle));
}

static __INLINE mtlk_work_role_e
mtlk_vap_get_role (mtlk_vap_handle_t vap_handle)
{
  mtlk_vap_info_internal_t *_info = (mtlk_vap_info_internal_t *)vap_handle;

  MTLK_ASSERT(_info != NULL);
  return (_info->role);
}

static __INLINE mtlk_hw_api_t *
mtlk_vap_get_hwapi (mtlk_vap_handle_t vap_handle)
{
  mtlk_vap_info_internal_t *_info = (mtlk_vap_info_internal_t *)vap_handle;

  MTLK_ASSERT(_info != NULL);
  MTLK_ASSERT(_info->manager != NULL);

  return mtlk_vap_manager_get_hwapi(_info->manager);
}
#ifdef UNUSED_CODE
mtlk_core_t ** __MTLK_IFUNC
mtlk_vap_get_core_array (mtlk_vap_manager_t *obj, uint32 *core_cnt);
#endif /* UNUSED_CODE */
mtlk_core_t * __MTLK_IFUNC
mtlk_vap_get_core (mtlk_vap_handle_t vap_handle);

struct _mtlk_core_vft_t const * __MTLK_IFUNC
mtlk_vap_get_core_vft (mtlk_vap_handle_t vap_handle) __MTLK_INT_HANDLER_SECTION;

static __INLINE wave_radio_t*
wave_vap_radio_get(mtlk_vap_handle_t vap_handle)
{
  mtlk_vap_info_internal_t *_info = (mtlk_vap_info_internal_t *)vap_handle;

  MTLK_ASSERT(NULL != _info);
  MTLK_ASSERT(NULL != _info->manager);
  MTLK_ASSERT(NULL != _info->radio);

  return _info->radio;
}

mtlk_pdb_t *wave_radio_param_db_get(wave_radio_t *radio); /* FIXME: prototype */

static __INLINE mtlk_pdb_t*
mtlk_vap_get_radio_pdb(mtlk_vap_handle_t vap_handle)
{
  wave_radio_t * radio = wave_vap_radio_get(vap_handle);
  return wave_radio_param_db_get(radio);
}

static __INLINE wave_int
wave_vap_radio_id_get(mtlk_vap_handle_t vap_handle)
{
  mtlk_vap_info_internal_t *_info = (mtlk_vap_info_internal_t *)vap_handle;

  MTLK_ASSERT(NULL != _info);
  MTLK_ASSERT(NULL != _info->manager);
  MTLK_ASSERT(NULL != _info->radio);

  return _info->radio_id;
}

static __INLINE wave_int
wave_vap_radio_id_txmm_get(mtlk_vap_handle_t vap_handle)
{
  mtlk_vap_info_internal_t *_info = (mtlk_vap_info_internal_t *)vap_handle;

  MTLK_ASSERT(NULL != _info);
  MTLK_ASSERT(NULL != _info->manager);
  MTLK_ASSERT(NULL != _info->radio);

  return _info->radio_id_txmm;
}

static __INLINE uint8
wave_vap_ep_bit_get(mtlk_vap_handle_t vap_handle)
{
  mtlk_vap_info_internal_t *_info = (mtlk_vap_info_internal_t *)vap_handle;

  MTLK_ASSERT(NULL != _info);
  MTLK_ASSERT(NULL != _info->manager);

  return _info->ep_bit;
}

static __INLINE struct _mtlk_abmgr_t*
mtlk_vap_get_abmgr(mtlk_vap_handle_t vap_handle)
{
  mtlk_vap_info_internal_t *_info = (mtlk_vap_info_internal_t *)vap_handle;

  MTLK_ASSERT(NULL != _info);
  MTLK_ASSERT(NULL != _info->abmgr);

  return _info->abmgr;
}

static __INLINE mtlk_pdb_t *
mtlk_vap_get_param_db (mtlk_vap_handle_t vap_handle)
{
  mtlk_vap_info_internal_t *_info = (mtlk_vap_info_internal_t *)vap_handle;

  MTLK_ASSERT(NULL != _info);
  MTLK_ASSERT(NULL != _info->param_db);

  return _info->param_db;
}

static __INLINE mtlk_txmm_t *
mtlk_vap_get_txmm (mtlk_vap_handle_t vap_handle)
{
  mtlk_vap_info_internal_t *_info = (mtlk_vap_info_internal_t *)vap_handle;

  MTLK_ASSERT(NULL != _info);
  MTLK_ASSERT(NULL != _info->txmm.base);

  return &_info->txmm;
}

static __INLINE mtlk_txmm_t *
mtlk_vap_get_txdm (mtlk_vap_handle_t vap_handle)
{
  mtlk_vap_info_internal_t *_info = (mtlk_vap_info_internal_t *)vap_handle;

  MTLK_ASSERT(NULL != _info);
  MTLK_ASSERT(NULL != _info->txdm.base);

  return &_info->txdm;
}

static __INLINE wave_vap_id_t
mtlk_vap_get_id (mtlk_vap_handle_t vap_handle)
{
  mtlk_vap_info_internal_t *_info = (mtlk_vap_info_internal_t *)vap_handle;

  MTLK_ASSERT(NULL != _info);

  return _info->id;
}

static __INLINE wave_vap_id_t
mtlk_vap_get_id_fw (mtlk_vap_handle_t vap_handle)
{
  mtlk_vap_info_internal_t *_info = (mtlk_vap_info_internal_t *)vap_handle;

  MTLK_ASSERT(NULL != _info);

  return _info->id_fw;
}

static __INLINE uint8
mtlk_vap_get_hw_idx(mtlk_vap_handle_t vap_handle)
{
  mtlk_vap_info_internal_t *_info = (mtlk_vap_info_internal_t *)vap_handle;

  MTLK_ASSERT(NULL != _info);

  return _info->hw_idx;
}

static __INLINE mtlk_irbd_t *
mtlk_vap_get_irbd (mtlk_vap_handle_t vap_handle)
{
  mtlk_vap_info_internal_t *_info = (mtlk_vap_info_internal_t *)vap_handle;

  MTLK_ASSERT(NULL != _info);
  MTLK_ASSERT(NULL != _info->irbd);

  return _info->irbd;
}

mtlk_hw_t* __MTLK_IFUNC
mtlk_vap_get_hw(mtlk_vap_handle_t vap_handle) __MTLK_INT_HANDLER_SECTION;

mtlk_handle_t __MTLK_IFUNC
mtlk_vap_get_ta(mtlk_vap_handle_t vap_handle);

BOOL __MTLK_IFUNC
mtlk_core_rcvry_is_running(mtlk_core_t *core);

void __MTLK_IFUNC
mtlk_vap_manager_inc_wds_bridgemode (mtlk_vap_manager_t *vap_manager);
void __MTLK_IFUNC
mtlk_vap_manager_dec_wds_bridgemode (mtlk_vap_manager_t *vap_manager);
uint32 __MTLK_IFUNC
mtlk_vap_manager_get_wds_bridgemode (mtlk_vap_manager_t *vap_manager);
void __MTLK_IFUNC
wave_vap_radio_id_txmm_set(mtlk_vap_handle_t vap_handle, unsigned new_radio_id_txmm);
#ifdef UNUSED_CODE
BOOL                 __MTLK_IFUNC mtlk_vap_manager_has_connections(mtlk_vap_manager_t  *vap_mng);
#endif /* UNUSED_CODE */
BOOL                 __MTLK_IFUNC mtlk_vap_manager_has_peer_connections(mtlk_vap_manager_t  *vap_mng);

void                 __MTLK_IFUNC mtlk_vap_delete(mtlk_vap_handle_t vap_handle);

int                  __MTLK_IFUNC mtlk_vap_start(mtlk_vap_handle_t vap_handle);
void                 __MTLK_IFUNC mtlk_vap_stop(mtlk_vap_handle_t vap_handle);

int                  __MTLK_IFUNC mtlk_vap_create_secondary_vap(mtlk_vap_handle_t vap_handle,
                                                                const unsigned char* addr,
                                                                const char* name,
                                                                uint8 vap_idx);
void                 __MTLK_IFUNC mtlk_vap_del_all_secondary_vaps(mtlk_vap_handle_t vap_handle);
int                  __MTLK_IFUNC mtlk_vap_connect_backhaul_sta(mtlk_vap_handle_t vap_handle,
                                                                sta_entry *sta);
void                 __MTLK_IFUNC mtlk_vap_disconnect_backhaul_sta(mtlk_vap_handle_t vap_handle,
                                                                  sta_entry *sta);

BOOL                 __MTLK_IFUNC mtlk_vap_is_dut(mtlk_vap_handle_t vap_handle);

void                 __MTLK_IFUNC mtlk_vap_set_stopped (mtlk_vap_handle_t vap_handle);

void                 __MTLK_IFUNC mtlk_vap_set_ready (mtlk_vap_handle_t vap_handle);

bool __MTLK_IFUNC
mtlk_vap_manager_is_sta_connected (mtlk_core_t *core, mtlk_core_t **sta_core, const unsigned char *mac);

uint32 __MTLK_IFUNC
mtlk_vap_manager_get_wlan_index (mtlk_vap_manager_t *vap_manager);

/* Auxiliary VAP Manager API wrappers */
static __INLINE mtlk_df_t*
mtlk_vap_manager_get_master_df(mtlk_vap_manager_t *obj)
{
  mtlk_vap_handle_t master_vap_handle;
  int               ires;

  ires = mtlk_vap_manager_get_master_vap(obj, &master_vap_handle);

  if (MTLK_ERR_OK != ires)
    return NULL;

  return mtlk_vap_get_df(master_vap_handle);
}

static __INLINE mtlk_core_t*
mtlk_vap_manager_get_master_core(mtlk_vap_manager_t *obj)
{
  mtlk_vap_handle_t master_vap_handle;
  mtlk_core_t*      core;
  int               ires;

  ires = mtlk_vap_manager_get_master_vap(obj, &master_vap_handle);
  MTLK_ASSERT(MTLK_ERR_OK == ires);
  if (MTLK_ERR_OK != ires) {
    return NULL;
  }

  core = mtlk_vap_get_core(master_vap_handle);
  MTLK_ASSERT(NULL != core);
  return core;
}

static __INLINE mtlk_txmm_t *
wave_vap_txmm_master_get(mtlk_vap_manager_t *obj)
{
  mtlk_vap_handle_t master_vap_handle;
  int               res;

  res = mtlk_vap_manager_get_master_vap(obj, &master_vap_handle);
  MTLK_ASSERT(MTLK_ERR_OK == res);

  if (res != MTLK_ERR_OK)
    return NULL;

  return mtlk_vap_get_txmm(master_vap_handle);
}

static __INLINE mtlk_pdb_t *
wave_vap_param_db_master_get(mtlk_vap_manager_t *obj)
{
  mtlk_vap_handle_t master_vap_handle;
  int               res;

  res = mtlk_vap_manager_get_master_vap(obj, &master_vap_handle);
  MTLK_ASSERT(MTLK_ERR_OK == res);

  if (res != MTLK_ERR_OK)
    return NULL;

  return mtlk_vap_get_param_db(master_vap_handle);
}


#undef LOG_LOCAL_GID
#undef LOG_LOCAL_FID

#define   MTLK_IDEFS_OFF
#include "mtlkidefs.h"

#endif /* __MTLK_VAP_MANAGER_H__ */
