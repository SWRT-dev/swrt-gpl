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
 * Private Core's definitions
 *
 * Written by: Grygorii Strashko
 *
 */

#ifndef __CORE_PRIV_H_
#define __CORE_PRIV_H_

#include "mtlkdfdefs.h"
#include "mtlk_param_db.h"

#define MTLK_IDEFS_ON
#include "mtlkidefs.h"

#define LOG_LOCAL_GID   GID_CORE
#define LOG_LOCAL_FID   3

/*****************************************************************************************
 * Core definitions
 *****************************************************************************************/
/* TODO: Empiric values, must be replaced somehow */
#define MAC_WATCHDOG_DEFAULT_TIMEOUT_MS 30000
#define MAC_WATCHDOG_DEFAULT_PERIOD_MS 50000

#define DEFAULT_MAX_STAs_SUPPORTED (32)
#define DEFAULT_MAX_VAPs_SUPPORTED (5)

/*****************************************************************************************
 *****************************************************************************************/

/*****************************************************************************************
 * Core abilities private definitions
 *****************************************************************************************/
int
wave_core_abilities_register(mtlk_core_t *core);

void
wave_core_abilities_unregister(mtlk_core_t *core);
/*****************************************************************************************
 * Radio abilities private definitions
 *****************************************************************************************/
int
wave_radio_abilities_register(mtlk_core_t *core);

void
wave_radio_abilities_unregister(mtlk_core_t *core);
/*****************************************************************************************
 *****************************************************************************************/

/*****************************************************************************************
 * Core ParameterDB private definitions
 *****************************************************************************************/
typedef enum {
  CORE_DB_CORE_BRIDGE_MODE,
  CORE_DB_CORE_AP_FORWARDING,
  CORE_DB_CORE_RELIABLE_MCAST,
  CORE_DB_CORE_MAC_ADDR,
  CORE_DB_CORE_BSSID,
  CORE_DB_CORE_MESH_MODE,
  CORE_DB_CORE_FWRD_UNKWN_MCAST_FLAG,
  CORE_DB_LAST_VALUE_ID
} mtlk_core_pdb_handle_id_t;

typedef mtlk_pdb_handle_t mtlk_core_hot_path_param_handles_t[CORE_DB_LAST_VALUE_ID];

int
mtlk_core_pdb_fast_handles_open(mtlk_pdb_t* obj, mtlk_core_hot_path_param_handles_t handles);

void
mtlk_core_pdb_fast_handles_close(mtlk_core_hot_path_param_handles_t handles);


static mtlk_pdb_handle_t __INLINE
mtlk_core_pdb_fast_handle_get(mtlk_core_hot_path_param_handles_t handles, mtlk_core_pdb_handle_id_t core_pdb_id)
{
  MTLK_ASSERT(CORE_DB_LAST_VALUE_ID > core_pdb_id);
  return handles[core_pdb_id];
}

#define MTLK_CORE_HOT_PATH_PDB_GET_INT(core, id) \
    mtlk_pdb_fast_get_int(mtlk_core_pdb_fast_handle_get(core->pdb_hot_path_handles, id))

#define MTLK_CORE_HOT_PATH_PDB_GET_MAC(core, id, mac) \
    mtlk_pdb_fast_get_mac(mtlk_core_pdb_fast_handle_get(core->pdb_hot_path_handles, id), mac)

#define MTLK_CORE_HOT_PATH_PDB_CMP_MAC(core, id, mac) \
    mtlk_pdb_fast_cmp_mac(mtlk_core_pdb_fast_handle_get(core->pdb_hot_path_handles, id), mac)

#define MTLK_CORE_PDB_GET_INT(core, id) \
    wave_pdb_get_int(mtlk_vap_get_param_db(core->vap_handle), id)

#define MTLK_CORE_PDB_SET_INT(core, id, value) \
    wave_pdb_set_int(mtlk_vap_get_param_db(core->vap_handle), id, value)

#define MTLK_CORE_PDB_GET_BINARY(core, id, buf, size) \
    wave_pdb_get_binary(mtlk_vap_get_param_db(core->vap_handle), id, buf, size)

#define MTLK_CORE_PDB_SET_BINARY(core, id, value, size)                 \
    wave_pdb_set_binary(mtlk_vap_get_param_db(core->vap_handle), id, value, size)

#define MTLK_CORE_PDB_GET_MAC(core, id, mac) \
    wave_pdb_get_mac(mtlk_vap_get_param_db(core->vap_handle), id, mac)

#define MTLK_CORE_PDB_SET_MAC(core, id, mac) \
    wave_pdb_set_mac(mtlk_vap_get_param_db(core->vap_handle), id, mac)
/*****************************************************************************************
 *****************************************************************************************/

#undef LOG_LOCAL_GID
#undef LOG_LOCAL_FID

#define MTLK_IDEFS_OFF
#include "mtlkidefs.h"

#endif /* __CORE_PRIV_H_ */
