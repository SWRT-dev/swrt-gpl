/* SPDX-License-Identifier: GPL-2.0 */
/*
  *  Copyright (C) 2021 MAXLINEAR Corporation.
  */
#ifndef __DATAPATH_BR_DOMAIN_H
#define __DATAPATH_BR_DOMAIN_H

/*! @brief enum DP_SUBIF_BR_DM_TT
 *  Specific domains for traffic types.
 */
enum DP_SUBIF_BR_DM_TT {
	DP_BR_DM_UC = 0, /*!< Unicast domain */
	DP_BR_DM_MC = 1, /*!< Multicast domain */
	DP_BR_DM_BC1 = 2, /*!< Broadcast domain one */
	DP_BR_DM_BC2 = 3, /*!< Broadcast domain two */
	DP_BR_DM_US = 4, /*!< Upstream domain */
};

#endif
