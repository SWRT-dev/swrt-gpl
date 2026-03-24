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
* Cross - platfrom core module code
*/

#ifndef _MTLK_CORE_IFACE_H_
#define _MTLK_CORE_IFACE_H_

struct nic;
struct _mtlk_eeprom_data_t;
struct _mtlk_vap_handle_t;
typedef struct _mtlk_txmm_msg_t mtlk_txmm_msg_t;

/* Core "getters" */
struct _mtlk_eeprom_data_t* __MTLK_IFUNC mtlk_core_get_eeprom(struct nic* core);
struct scan_support*        __MTLK_IFUNC mtlk_core_get_scan_support(struct nic* core);

uint8   __MTLK_IFUNC mtlk_core_is_device_busy(mtlk_handle_t context);
BOOL    __MTLK_IFUNC mtlk_core_is_stopping(struct nic *core);
uint32  __MTLK_IFUNC mtlk_core_ta_on_timer(mtlk_osal_timer_t *timer, mtlk_handle_t ta_handle);

int     __MTLK_IFUNC core_on_rcvry_configure(mtlk_core_t *core, uint32 rcvry_net_state);
void core_recovery_free_sta_infos(mtlk_dlist_t *connected_sta_infos);
int  core_recovery_deactivate(mtlk_core_t *master_core, mtlk_core_t *nic);
int     __MTLK_IFUNC core_on_rcvry_isol(mtlk_core_t *core, uint32 rcvry_type);
void    __MTLK_IFUNC core_on_rcvry_isol_irbd_unregister(mtlk_core_t *core);
int     __MTLK_IFUNC core_on_rcvry_restore(mtlk_core_t *core, uint32 rcvry_type);
void    __MTLK_IFUNC core_on_rcvry_error(mtlk_core_t *core);
void    __MTLK_IFUNC mtlk_core_store_calibration_channel_bit_map(mtlk_core_t *core, uint32 *storedCalibrationChannelBitMap);
int     __MTLK_IFUNC core_remove_sid(mtlk_core_t *core, uint16 sid);
int     __MTLK_IFUNC core_ap_disconnect_all (mtlk_core_t *nic);
BOOL    __MTLK_IFUNC core_wds_frame_drop(mtlk_core_t *core, IEEE_ADDR *addr);

/**
*\defgroup CORE_SERIALIZED_TASKS Core serialization facility
*\brief Core interface for scheduling tasks on demand of outer world

*\{

*/

/*! Core serialized task callback prototype

    \param   object           Handle of receiver object
    \param   data             Pointer to the data buffer provided by caller
    \param   data_size        Size of data buffer provided by caller
    \return  MTLK_ERR_... code indicating whether function invocation succeeded

    \warning
    Do not garble function invocation result with execution result.
    Execution result indicates whether request was \b processed
    successfully. In case invocation result is MTK_ERR_OK, caller may examine
    Execution result and collect resulting data.
*/
typedef int __MTLK_IFUNC (*mtlk_core_task_func_t)(mtlk_handle_t object,
                                                  const void *data,
                                                  uint32 data_size);

void    __MTLK_IFUNC core_schedule_recovery_task (struct _mtlk_vap_handle_t *vap_handle,
            mtlk_core_task_func_t task, mtlk_handle_t rcvry_handle,
            const void* data, uint32 data_size);
/*! Function for scheduling serialized task on demand of internal core activities

    \param   nic              Pointer to the core object
    \param   object           Handle of receiver object
    \param   func             Task callback
    \param   data             Pointer to the data buffer provided by caller
    \param   data_size        Size of data buffer provided by caller

*/
int __MTLK_IFUNC mtlk_core_schedule_internal_task_ex(struct nic* core,
                                                     mtlk_handle_t object,
                                                     mtlk_core_task_func_t func,
                                                     const void *data, size_t size,
                                                     mtlk_slid_t issuer_slid);

#define mtlk_core_schedule_internal_task(core, object, func, data, size) \
  mtlk_core_schedule_internal_task_ex((core), (object), (func), (data), (size), MTLK_SLID)

int __MTLK_IFUNC mtlk_core_schedule_user_task_ex(struct nic* core,
                                                 mtlk_handle_t object,
                                                 mtlk_core_task_func_t func,
                                                 const void *data, size_t size,
                                                 mtlk_slid_t issuer_slid);

#define mtlk_core_schedule_user_task(core, object, func, data, size) \
  mtlk_core_schedule_user_task_ex((core), (object), (func), (data), (size), MTLK_SLID)


BOOL mtlk_core_is_20_40_active(struct nic* core);
void __MTLK_IFUNC mtlk_core_set_pm_enabled (mtlk_core_t *core, BOOL enabled);
BOOL __MTLK_IFUNC mtlk_core_has_connections(mtlk_core_t *core);

/*\}*/
#endif //_MTLK_CORE_IFACE_H_
