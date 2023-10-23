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
 * Driver framework debug API.
 *
 * Originally written by Andrii Tseglytskyi
 *
 */

#ifndef __DF_UI__
#define __DF_UI__

#include "mtlkdfdefs.h"
#include "mtlk_vap_manager.h"
#include "mtlkirbd.h"
#include "mtlk_wss.h"

#define   MTLK_IDEFS_ON
#include "mtlkidefs.h"

/**********************************************************************
 * Functions declaration
***********************************************************************/
/*! \fn      mtlk_df_t* mtlk_df_create(mtlk_vap_handle_t vap_handle, void *ctx)
    
    \brief   Allocates and initiates DF object.

    \return  pointer to created DF
*/
mtlk_df_t* mtlk_df_create(mtlk_vap_handle_t vap_handle, const char *name, struct net_device *ndev);

/*! \fn      void mtlk_df_delete(mtlk_df_t *df)
    
    \brief   Cleanups and deletes DF object.

    \param   df          DF object 
*/
void mtlk_df_delete(mtlk_df_t *df);

/*! \fn      void mtlk_df_start(mtlk_df_t *df)
    
    \brief   Starts the DF object.

    \param   df          DF object
*/
int mtlk_df_start(mtlk_df_t *df);

/*! \fn      void mtlk_df_stop(mtlk_df_t *df)
    
    \brief   Stops the DF object.

    \param   df          DF object
*/
void mtlk_df_stop(mtlk_df_t *df);

mtlk_vap_manager_t*
mtlk_df_get_vap_manager(const mtlk_df_t* df);

mtlk_vap_handle_t
mtlk_df_get_vap_handle(const mtlk_df_t* df);

const char* mtlk_df_get_name(mtlk_df_t *df);
int         mtlk_df_down(mtlk_df_t *df);
int         mtlk_df_up(mtlk_df_t *df);

#ifdef CPTCFG_IWLWAV_TSF_TIMER_ACCESS_ENABLED
/*! \fn      mtlk_df_t* mtlk_df_get_df_by_dfg_entry(mtlk_dlist_entry_t *df_entry)

    \brief   Returns pointer to DF object, expanded from node in DFG list

    \param   df_entry list entry for DF object
*/
mtlk_df_t* mtlk_df_get_df_by_dfg_entry(mtlk_dlist_entry_t *df_entry);
#endif /* CPTCFG_IWLWAV_TSF_TIMER_TIMESTAMPS_IN_DEBUG_PRINTOUTS */

/**********************************************************************
 * DF USER utilities (CORE->DF->DF_USER->User space)
 **********************************************************************/
/**

*\brief Outer world interface subsidiary of driver framework

*\defgroup DFUSER DF user
*\{
*/

enum BCL_UNIT_TYPE {
  BCL_UNIT_INT_RAM = 0,
  BCL_UNIT_AFE_RAM,
  BCL_UNIT_RFIC_RAM,
  BCL_UNIT_EEPROM,
  BCL_UNIT_MAX = 10
};


/*! Notifies DF UI about user request processing completion

    \param   req     Request object.
    \param   result  Processing result.

    \warning
    Do not garble request processing result with request execution result.
    Request processing result indicates whether request was \b processed
    successfully. In case processing result is MTK_ERR_OK, caller may examine
    request execution result and collect request execution resulting data.

*/
void __MTLK_IFUNC
mtlk_df_ui_req_complete(mtlk_user_request_t *req, int result);

BOOL __MTLK_IFUNC
mtlk_df_ui_is_promiscuous(mtlk_df_t *df);

/***
 * Requests from Core
 ***/
/* Data transfer functions */
int __MTLK_IFUNC
mtlk_df_ui_indicate_rx_data(mtlk_df_t *df, mtlk_nbuf_t *nbuf) __MTLK_INT_HANDLER_SECTION;

BOOL __MTLK_IFUNC
mtlk_df_ui_check_is_mc_group_member(mtlk_df_t *df, const uint8* group_addr);

#if MTLK_USE_DIRECTCONNECT_DP_API
BOOL __MTLK_IFUNC
mtlk_df_ui_mcast_is_registered (mtlk_df_t* df);
#else
#define mtlk_df_ui_mcast_is_registered(df)  (FALSE)
#endif

/***
 * Notifications from Core
 ***/
void __MTLK_IFUNC
mtlk_df_ui_notify_tx_start(mtlk_df_t *df);


typedef enum
{
  MIC_FAIL_PAIRWISE = 0,
  MIC_FAIL_GROUP
} mtlk_df_ui_mic_fail_type_t;

void __MTLK_IFUNC
mtlk_df_ui_notify_mic_failure(mtlk_df_t *df,
                                const uint8 *src_mac,
                                mtlk_df_ui_mic_fail_type_t fail_type);

void __MTLK_IFUNC
mtlk_df_ui_notify_notify_rmmod(const char *wlanitrf);

void __MTLK_IFUNC
mtlk_df_ui_notify_notify_fw_hang(mtlk_df_t *df, uint32 fw_cpu, uint32 sw_watchdog_data);

void __MTLK_IFUNC
mtlk_df_on_rcvry_isol(mtlk_df_t *df);

void __MTLK_IFUNC
mtlk_df_on_rcvry_restore(mtlk_df_t *df);

/* Remove MAC addr from switch DC DP table */
void __MTLK_IFUNC
mtlk_df_user_dcdp_remove_mac_addr(mtlk_df_t *df, const uint8 *mac_addr);

/* Set HS20 enabled flag */
void __MTLK_IFUNC
mtlk_df_user_set_hs20_status(mtlk_df_t *df, BOOL hs20_enabled);

/* Get HS20 status */
BOOL __MTLK_IFUNC
mtlk_df_user_get_hs20_status(mtlk_df_t *df);

/* Recovery: subinterface start */
int __MTLK_IFUNC
mtlk_df_on_rcvry_subxface_start (mtlk_df_t* df);

/* Recovery: subinterface stop */
int __MTLK_IFUNC
mtlk_df_on_rcvry_subxface_stop (mtlk_df_t* df);

#define   MTLK_IDEFS_OFF
#include "mtlkidefs.h"

#endif /* __DF_UI__ */
