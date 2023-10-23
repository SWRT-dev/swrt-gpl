/*******************************************************************************
**
** FILE NAME    : ppa_api_sess_helper.h
** PROJECT      : PPA
** MODULES      : PPA API - Routing/Bridging(flow based) helper routines
**
** DATE         : 24 Feb 2015
** AUTHOR       : Mahipati Deshpande
** DESCRIPTION  : PPA Protocol Stack Hook API Session Operation Functions
** COPYRIGHT    : Copyright (c) 2017 Intel Corporation
**
**   For licensing information, see the file 'LICENSE' in the root folder of
**   this software module.
**
** HISTORY
** $Date        $Author                $Comment
** 24 Feb 2015  Mahipati               The helper functions are moved from
**                                     ppa_api_session.c
*******************************************************************************/
#ifndef __PPA_SAE_HAL_H__

#define __PPA_SAE_HAL_H__

extern int32_t (*ppa_sw_add_session_hook)(PPA_BUF *skb,
void *pitem);

extern int32_t (*ppa_sw_update_session_hook)(PPA_BUF *skb,
void *pitem,
void *txifinfo);

extern void (*ppa_sw_del_session_hook)(void *pitem);

extern int32_t (*ppa_sw_fastpath_enable_hook)(uint32_t f_enable,
uint32_t flags);

extern int32_t (*ppa_get_sw_fastpath_status_hook)(uint32_t *f_enable,
uint32_t flags);

extern int32_t (*ppa_sw_session_enable_hook)(void *pitem,
uint32_t f_enable,
uint32_t flags);

extern int32_t (*ppa_get_sw_session_status_hook)(void *pitem,
uint32_t *f_enable,
uint32_t flags);

extern int32_t (*ppa_sw_fastpath_send_hook)(struct sk_buff *skb);

int32_t ppa_sw_add_session(struct sk_buff *skb,
void *pitem);

int32_t ppa_sw_update_session(struct sk_buff *skb,
void *pitem,
void *txifinfo);

int32_t ppa_sw_del_session(void *pitem);
int32_t ppa_sw_fastpath_send(void *skb);

int32_t ppa_sw_fastpath_enable(uint32_t f_enable);

int32_t ppa_get_sw_fastpath_status(uint32_t *f_enable);

int32_t ppa_sw_session_enable(void *pitem,
uint32_t f_enable,
uint32_t flags);

int32_t ppa_get_sw_session_status(void *pitem,
uint32_t *f_enable,
uint32_t flags);

#endif
