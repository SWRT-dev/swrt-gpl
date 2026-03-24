#ifndef __PHY_ERR_CTR_H__20200225_2320__
#define __PHY_ERR_CTR_H__20200225_2320__
/******************************************************************************
 **
 ** FILE NAME    : phy_err_counter.h
 ** PROJECT      : PPA
 **
 ** DATE         : 25th February 2020
 ** AUTHOR       : Moinak Debnath
 ** DESCRIPTION  : Header for Code violated Phy level error statistics
 ** COPYRIGHT    : Copyright (c) 2020 Intel Corporation
 **
 **   For licensing information, see the file 'LICENSE' in the root folder of
 **   this software module.
 **
 ** HISTORY
 ** $Date        		$Author         	$Comment
 ** 25th February 2020		Moinak Debnath		Initiate Version
 *******************************************************************************
 */

/*
 * ####################################
 *              Definition
 * ####################################
 */

typedef struct {
	bool is_wan;
	uint64_t err_count;
} phy_err_node;
 
#define MDIO_PHY_ERR_CNT_REG	21
#define WAN_PORT_PHY		1
#define PHY_ERR_PORTS		6
#define RETRY_TIME_DEFAULT	100
#define PHY_ERR_PROC_PARENT	""
#define PHY_ERR_PROC_NAME	"eth"
#define PROC_FILE_PHY_ERR	"phy_err_cnt"
#define PHY_ERR_POLL_MIN	1		/* 1 mili-secs */
#define PHY_ERR_POLL_MAX	10000		/* 10 sec */

extern bool is_xway_gphy_fw_loaded(void);
#endif /*	__PHY_ERR_CTR_H__20200225_2320__	*/
