/*
 * Broadcom 802.11abg Networking Device Driver Configuration file
 *
 * Broadcom Proprietary and Confidential. Copyright (C) 2016,
 * All Rights Reserved.
 * 
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom;
 * the contents of this file may not be disclosed to third parties, copied
 * or duplicated in any form, in whole or in part, without the prior
 * written permission of Broadcom.
 *
 *
 * <<Broadcom-WL-IPTag/Proprietary:>>
 *
 * $Id: wltunable_rte_43236b_usbap.h 523134 2014-12-27 05:54:56Z $
 *
 * wl driver tunables for 43236b1 BMAC rte dev
 */

#define D11CONF		0x1000000	/* D11 Core Rev 24 */
#define D11CONF2	0		/* D11 Core Rev > 31 */
#define NCONF		0x200		/* N-Phy Rev 9 */

#define CTFPOOLSZ       384	/* max buffers in ctfpool */
#define NTXD		64	/* THIS HAS TO MATCH with HIGH driver tunable, AMPDU/rpc_agg */
#define NRXD		32
#define NRXBUFPOST	16
#define WLC_DATAHIWAT	10
#define RXBND		16
#define NRPCTXBUFPOST	64	/* used in HIGH driver */
#define DNGL_MEM_RESTRICT_RXDMA    (6*2048)
