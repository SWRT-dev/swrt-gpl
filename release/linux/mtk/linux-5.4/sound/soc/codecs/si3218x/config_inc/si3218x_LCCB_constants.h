/* SPDX-License-Identifier: GPL-2.0 */
/*
 *
 * Si3218x ProSLIC API Configuration Tool Version 4.2.1
 * Last Updated in API Release: 9.2.0
 * source XML file: si3218x_LCCB_constants.xml
 *
 * Auto generated file from configuration tool.
 */


#ifndef SI3218X_CONSTANTS_H
#define SI3218X_CONSTANTS_H

/** Ringing Presets */
enum {
	DEFAULT_RINGING,
	RING_F20_45VRMS_0VDC_LPR,
	RINGING_LAST_ENUM
};

/** DC_Feed Presets */
enum {
	DCFEED_48V_20MA,
	DCFEED_48V_25MA,
	DCFEED_PSTN_DET_1,
	DCFEED_PSTN_DET_2,
	DC_FEED_LAST_ENUM
};

/** Impedance Presets */
enum {
	ZSYN_600_0_0_30_0,
	ZSYN_270_750_150_30_0,
	ZSYN_370_620_310_30_0,
	ZSYN_220_820_120_30_0,
	ZSYN_600_0_1000_30_0,
	ZSYN_200_680_100_30_0,
	ZSYN_220_820_115_30_0,
	WB_ZSYN_600_0_0_20_0,
	IMPEDANCE_LAST_ENUM
};

/** FSK Presets */
enum {
	DEFAULT_FSK,
	ETSI_FSK,
	FSK_LAST_ENUM
};

/** Pulse_Metering Presets */
enum {
	DEFAULT_PULSE_METERING,
	PULSE_METERING_LAST_ENUM
};

/** Tone Presets */
enum {
	TONEGEN_FCC_DIAL,
	TONEGEN_FCC_BUSY,
	TONEGEN_FCC_RINGBACK,
	TONEGEN_FCC_REORDER,
	TONEGEN_FCC_CONGESTION,
	TONEGEN_FCC_CAS,
	TONEGEN_FCC_SAS,
	TONEGEN_ETSI_DTAS,
	TONEGEN_1004,
	TONE_LAST_ENUM
};

/** PCM Presets */
enum {
	PCM_8ULAW,
	PCM_8ALAW,
	PCM_16LIN,
	PCM_16LIN_WB,
	PCM_LAST_ENUM
};



#endif

