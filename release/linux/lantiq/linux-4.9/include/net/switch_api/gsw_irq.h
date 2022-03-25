/******************************************************************************
                Copyright (c) 2016, 2017 Intel Corporation

For licensing information, see the file 'LICENSE' in the root folder of
this software module.
******************************************************************************/

#ifndef _GSW_IRQ_H_
#define _GSW_IRQ_H_

typedef enum {
	BM 		= 0,
	SDMA	= 1,
	FDMA	= 2,
	PCE		= 3,
	PMAC	= 4
} GSWIP_IRQ_BLK;

typedef enum {
	PCE_MAC_TABLE_FULL			= 0,
	PCE_IGMP_TABLE_FULL			= 1,
	PCE_PARSER_READY 			= 2,
	PCE_CLASSIFICATION_PHASE_0 	= 3,
	PCE_CLASSIFICATION_PHASE_1 	= 4,
	PCE_CLASSIFICATION_PHASE_2 	= 5,
	PCE_FLOW_TABLE_RULE_MATCHED = 6,
	PCE_MAC_TABLE_CHANGE 		= 7,
	PCE_METER_EVENT = 8,
} GSWIP_PCE_EVENT;

typedef struct {
	unsigned int blk;
	unsigned int event;
	unsigned int portid;
	void *call_back;
	void *param;
} GSW_Irq_Op_t;

typedef enum {
	XGMAC_BLK	= 0,
	LMAC_BLK	= 1,
} MAC_IRQ_BLK;

typedef enum {
	XGMAC_TSTAMP_EVNT = 0,
	XGMAC_LPI_EVNT,
	XGMAC_TXERR_STS_EVNT,
	XGMAC_RXERR_STS_EVNT,
	XGMAC_PMT_EVNT,
	XGMAC_TXQ_OVFW_EVNT,
	XGMAC_RXQ_OVFW_EVNT,
	XGMAC_AVG_BPS_EVNT,
	LMAC_PHYERR_EVNT,
	LMAC_ALIGN_EVNT,
	LMAC_SPEED_EVNT,
	LMAC_FDUP_EVNT,
	LMAC_RXPAUEN_EVNT,
	LMAC_TXPAUEN_EVNT,
	LMAC_LPIOFF_EVNT,
	LMAC_LPION_EVNT,
	LMAC_JAM_EVNT,
	LMAC_FCSERR_EVNT,
	LMAC_TXPAU_EVNT,
	LMAC_RXPAU_EVNT,
	MAX_IRQ_EVNT,
	XGMAC_ALL_EVNT,
	LMAC_ALL_EVNT
} MAC_IRQ_ENT;


// TODO: MACSec IRq Events need to add
#define MACSEC_MAX_IRQ_EVENT	10

#endif