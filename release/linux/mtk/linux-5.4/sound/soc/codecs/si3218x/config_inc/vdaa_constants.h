/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Si3217x ProSLIC API Configuration Tool Version 4.2.1
 * Last Updated in API Release: 9.2.0
 * source XML file: si3217x_FLBK_GDRV_constants.xml
 *
 * Auto generated file from configuration tool.
*/


#ifndef VDAA_CONSTANTS_H
#define VDAA_CONSTANTS_H

/** Vdaa_Country Presets */
enum {
	COU_USA,
	COU_GERMANY,
	COU_CHINA,
	COU_AUSTRALIA,
	VDAA_COUNTRY_LAST_ENUM
};

/** Vdaa_Audio_Gain Presets */
enum {
	AUDIO_GAIN_0DB,
	AUDIO_ATTEN_4DB,
	AUDIO_ATTEN_6DB,
	AUDIO_ATTEN_11DB,
	VDAA_AUDIO_GAIN_LAST_ENUM
};

/** Vdaa_Ring_Validation Presets */
enum {
	RING_DET_NOVAL_LOWV,
	RING_DET_VAL_HIGHV,
	VDAA_RING_VALIDATION_LAST_ENUM
};

/** Vdaa_PCM Presets */
enum {
	DAA_PCM_8ULAW,
	DAA_PCM_8ALAW,
	DAA_PCM_16LIN,
	VDAA_PCM_LAST_ENUM
};

/** Vdaa_Hybrid Presets */
enum {
	HYB_600_0_0_500FT_24AWG,
	HYB_270_750_150_500FT_24AWG,
	HYB_200_680_100_500FT_24AWG,
	HYB_220_820_120_500FT_24AWG,
	VDAA_HYBRID_LAST_ENUM
};

#endif

