#ifndef __PPA_API_PROC_H__20081103_2009__
#define __PPA_API_PROC_H__20081103_2009__

/*******************************************************************************
 **
 ** FILE NAME    : ppa_api_proc.h
 ** PROJECT      : PPA
 ** MODULES      : PPA API (Routing/Bridging Acceleration APIs)
 **
 ** DATE         : 3 NOV 2008
 ** AUTHOR       : Xu Liang
 ** DESCRIPTION  : PPA Protocol Stack Hook API Proc Filesystem Functions Header
 **                File
 ** COPYRIGHT    : Copyright (c) 2017 Intel Corporation
 **
 **   For licensing information, see the file 'LICENSE' in the root folder of
 **   this software module.
 **
 ** HISTORY
 ** $Date        $Author         $Comment
 ** 03 NOV 2008  Xu Liang        Initiate Version
 *******************************************************************************/
/*! \file ppa_api_proc.h
  \brief This file contains all proc api.
 */

/** \defgroup PPA_PROC_API PPA Proc API
  \brief  PPA Proc API provide a ppa configure command to let user comfigure PPA
  - ppa_api_proc.h: Header file for PPA PROC API
  - ppa_api_proc.c: C Implementation file for PPA API
 */
/* @{ */
#include "ppa_api_debug.h"

/*
 * ####################################
 *              Definition
 * ####################################
 */
#define PROC_STR_LEN 64
#define MAX_NAME_LEN 7
#define MAX_FLAG_SIZE 26
typedef struct {
  char name[MAX_NAME_LEN];
  int value;
}name_value;

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

extern PPA_FILTER_STATS ppa_filter;
extern PPA_PORT_QUEUE_NUM_STATUS ppa_port_qnum[PPA_MAX_PORT_NUM];

/*
 *  Init/Uninit Functions
 */
/*! \fn ppa_api_proc_file_create
  \brief This function to create ppa proc
 */
void ppa_api_proc_file_create(void);

/*! \fn ppa_api_proc_file_destroy
  \brief This function to destroy ppa proc
 */

void ppa_api_proc_file_destroy(void);

/* @} */
#endif  /*  __PPA_API_PROC_H__20081103_2009__*/
