#ifndef __PPA_API_HAL_SELECTOR_H_2014_02_28
#define __PPA_API_HAL_SELECTOR_H_2014_02_28
/*******************************************************************************
**
** FILE NAME    : ppa_api_hal_selector.h
** PROJECT      : PPA
** MODULES      : PPA API (PPA HAL SELECTOR  APIs)
**
** DATE         : 28 Feb 2014
** AUTHOR       : Kamal Eradath
** DESCRIPTION  : PPA HAL SELECTOR APIs
**                File
** COPYRIGHT    : Copyright (c) 2017 Intel Corporation
**
**   For licensing information, see the file 'LICENSE' in the root folder of
**   this software module.
**
** HISTORY
** $Date                $Author         $Comment
** 28-FEB-2014  	Kamal Eradath   Initiate Version
*******************************************************************************/

/*! \file ppa_api_hal_selector.h
\brief This file contains es.
provide PPA APIs for HAL selector.
*/

/*
* ####################################
*              Definition
* ####################################
*/

/*
* ####################################
*              Data Type
* ####################################
*/

/*
* ####################################
*             Declaration
* ####################################
*/

/* check if a session has hals registered for all its capabilities*/
int32_t ppa_is_hal_registered_for_all_caps(struct uc_session_node *p_item);

/* unicast routing entries*/
#define PPA_HSEL_F_SESSION_SW_ONLY 0x00000001
int32_t ppa_hsel_add_routing_session(struct uc_session_node *p_item, PPA_BUF *skb, uint32_t flags);
void ppa_hsel_del_routing_session(struct uc_session_node *p_item);
int32_t ppa_hsel_update_routing_session(struct uc_session_node *p_item, uint32_t flags);
void ppa_hsel_check_hit_stat_clear_mib(int32_t flag);

/* multicast routing entries*/
int32_t ppa_hsel_add_wan_mc_group(struct mc_session_node *p_item, PPA_C_OP *cop);
void ppa_hsel_del_wan_mc_group(struct mc_session_node *p_item);
int32_t ppa_hsel_update_wan_mc_group(struct mc_session_node *p_item, PPA_C_OP *cop, uint32_t flags);

#if defined(CONFIG_LTQ_TOE_DRIVER) && CONFIG_LTQ_TOE_DRIVER
int32_t ppa_add_lro_entry(struct uc_session_node *p_item, uint32_t f_test);
int32_t ppa_del_lro_entry(struct uc_session_node *p_item);
#endif
int32_t ppa_lro_entry_criteria(struct uc_session_node *p_item, PPA_BUF *ppa_buf, uint32_t flags);

#endif /*__PPA_API_HAL_SELECTOR_H_2014_02_28*/

