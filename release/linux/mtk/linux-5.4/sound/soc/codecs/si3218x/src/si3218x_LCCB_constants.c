// SPDX-License-Identifier: GPL-2.0
/*
 * Si3218x ProSLIC API Configuration Tool Version 4.2.1
 * Last Updated in API Release: 9.2.0
 * source XML file: si3218x_LCCB_constants.xml
 *
 * Auto generated file from configuration tool.
 */


#include "../inc/proslic.h"
#include "../inc/si3218x.h"

Si3218x_General_Cfg Si3218x_General_Configuration  = {
0x72,                       /* DEVICE_KEY */
BO_DCDC_LCCB,               /* BOM_OPT */
VDC_4P5_16P0,               /* VDC_RANGE_OPT */
AUTO_ZCAL_ENABLED,          /* ZCAL_ENABLE */
BO_STD_BOM,                 /* PM_BOM */
0x00000000L,                /* DCDC_OITHRESH_LO */
0x01A00000L,                /* DCDC_OITHRESH_HI */
0x00A00000L,                /* DCDC_OVTHRESH */
5000L,                      /* DCDC_UVTHRESH */
1000L,                      /* DCDC_UVHYST */
0x00000000L,                /* DCDC_FSW_VTHLO */
0x00000000L,                /* DCDC_FSW_VHYST */
0x0048D15BL,                /* P_TH_HVIC */
0x07FEB800L,                /* COEF_P_HVIC */
0x00083120L,                /* BAT_HYST */
0x03D70A20L,                /* VBATH_EXPECT (60.00V) */
0x06666635L,                /* VBATR_EXPECT (100.00V) */
0x0FFF0000L,                /* PWRSAVE_TIMER */
0x01999A00L,                /* OFFHOOK_THRESH */
0x00F00000L,                /* VBAT_TRACK_MIN */
0x00F00000L,                /* VBAT_TRACK_MIN_RNG */
0x0ADD5500L,                /* DCDC_ANA_SCALE */
0x00800000L,                /* THERM_DBI */
0x00FFFFFFL,                /* VOV_DCDC_SLOPE */
0x00A18937L,                /* VOV_DCDC_OS */
0x00A18937L,                /* VOV_RING_BAT_DCDC */
0x00E49BA5L,                /* VOV_RING_BAT_MAX */
0x01018900L,                /* DCDC_VERR */
0x0080C480L,                /* DCDC_VERR_HYST */
0x00400000L,                /* PD_UVLO */
0x00400000L,                /* PD_OVLO */
0x00200000L,                /* PD_OCLO */
0x00400000L,                /* PD_SWDRV */
0x00000000L,                /* DCDC_UVPOL */
0x00200000L,                /* DCDC_RNGTYPE */
0x00180000L,                /* DCDC_ANA_TOFF */
0x00100000L,                /* DCDC_ANA_TONMIN */
0x012FC000L,                /* DCDC_ANA_TONMAX */
0x50,                       /* IRQEN1 */
0x13,                       /* IRQEN2 */
0x03,                       /* IRQEN3 */
0x00,                       /* IRQEN4 */
0x30,                       /* ENHANCE */
0x3F,                       /* AUTO */
};

Si3218x_audioGain_Cfg Si3218x_audioGain_Presets [] = {
{0x1377080L,0, 0x0L, 0x0L, 0x0L, 0x0L},
{0x80C3180L,0, 0x0L, 0x0L, 0x0L, 0x0L}
};

Si3218x_Ring_Cfg Si3218x_Ring_Presets[] ={
{
/*
	Loop = 500.0 ft @ 0.044 ohms/ft, REN = 3, Rcpe = 600 ohms
	Rprot = 54 ohms, Type = LPR, Waveform = SINE
*/ 
0x00050000L,	/* RTPER */
0x07EFE000L,	/* RINGFR (20.000 Hz) */
0x00209380L,	/* RINGAMP (55.000 vrms)  */
0x00000000L,	/* RINGPHAS */
0x00000000L,	/* RINGOF (0.000 vdc) */
0x15E5200EL,	/* SLOPE_RING (100.000 ohms) */
0x006C94D6L,	/* IRING_LIM (70.000 mA) */
0x00522220L,	/* RTACTH (45.357 mA) */
0x0FFFFFFFL,	/* RTDCTH (450.000 mA) */
0x00006000L,	/* RTACDB (75.000 ms) */
0x00006000L,	/* RTDCDB (75.000 ms) */
0x0051EB82L,	/* VOV_RING_BAT (5.000 v) */
0x00000000L,	/* VOV_RING_GND (0.000 v) */
0x05B835EFL,	/* VBATR_EXPECT (89.368 v) */
0x80,			/* RINGTALO (2.000 s) */
0x3E,			/* RINGTAHI */
0x00,			/* RINGTILO (4.000 s) */
0x7D,			/* RINGTIHI */
0x00000000L,	/* ADAP_RING_MIN_I */
0x00003000L,	/* COUNTER_IRING_VAL */
0x00051EB8L,	/* COUNTER_VTR_VAL */
0x00000000L,	/* CONST_028 */
0x00000000L,	/* CONST_032 */
0x00000000L,	/* CONST_038 */
0x00000000L,	/* CONST_046 */
0x00000000L,	/* RRD_DELAY */
0x00000000L,	/* RRD_DELAY2 */
0x01893740L,	/* VBAT_TRACK_MIN_RNG */
0x98,			/* RINGCON */
0x00,			/* USERSTAT */
0x02DC1AF7L,	/* VCM_RING (43.434 v) */
0x02DC1AF7L,	/* VCM_RING_FIXED */
0x003126E8L,	/* DELTA_VCM */
0x00200000L,	/* DCDC_RNGTYPE */
},  /* DEFAULT_RINGING */
{
/*
	Loop = 500.0 ft @ 0.044 ohms/ft, REN = 5, Rcpe = 600 ohms
	Rprot = 54 ohms, Type = LPR, Waveform = SINE
*/ 
0x00050000L,	/* RTPER */
0x07EFE000L,	/* RINGFR (20.000 Hz) */
0x001C0AFCL,	/* RINGAMP (45.000 vrms)  */
0x00000000L,	/* RINGPHAS */
0x00000000L,	/* RINGOF (0.000 vdc) */
0x15E5200EL,	/* SLOPE_RING (100.000 ohms) */
0x006C94D6L,	/* IRING_LIM (70.000 mA) */
0x0068A9B9L,	/* RTACTH (57.798 mA) */
0x0FFFFFFFL,	/* RTDCTH (450.000 mA) */
0x00006000L,	/* RTACDB (75.000 ms) */
0x00006000L,	/* RTDCDB (75.000 ms) */
0x0051EB82L,	/* VOV_RING_BAT (5.000 v) */
0x00000000L,	/* VOV_RING_GND (0.000 v) */
0x04F7DA57L,	/* VBATR_EXPECT (77.628 v) */
0x80,			/* RINGTALO (2.000 s) */
0x3E,			/* RINGTAHI */
0x00,			/* RINGTILO (4.000 s) */
0x7D,			/* RINGTIHI */
0x00000000L,	/* ADAP_RING_MIN_I */
0x00003000L,	/* COUNTER_IRING_VAL */
0x00051EB8L,	/* COUNTER_VTR_VAL */
0x00000000L,	/* CONST_028 */
0x00000000L,	/* CONST_032 */
0x00000000L,	/* CONST_038 */
0x00000000L,	/* CONST_046 */
0x00000000L,	/* RRD_DELAY */
0x00000000L,	/* RRD_DELAY2 */
0x01893740L,	/* VBAT_TRACK_MIN_RNG */
0x98,			/* RINGCON */
0x00,			/* USERSTAT */
0x027BED2BL,	/* VCM_RING (37.564 v) */
0x027BED2BL,	/* VCM_RING_FIXED */
0x003126E8L,	/* DELTA_VCM */
0x00200000L,	/* DCDC_RNGTYPE */
}   /* RING_F20_45VRMS_0VDC_LPR */
};

Si3218x_DCfeed_Cfg Si3218x_DCfeed_Presets[] = {
{
0x1C8A024CL,	/* SLOPE_VLIM */
0x1F909679L,	/* SLOPE_RFEED */
0x0040A0E0L,	/* SLOPE_ILIM */
0x1D5B21A9L,	/* SLOPE_DELTA1 */
0x1DD87A3EL,	/* SLOPE_DELTA2 */
0x05A38633L,	/* V_VLIM (48.000 v) */
0x050D2839L,	/* V_RFEED (43.000 v) */
0x03FE7F0FL,	/* V_ILIM  (34.000 v) */
0x00B4F3C3L,	/* CONST_RFEED (15.000 mA) */
0x005D0FA6L,	/* CONST_ILIM (20.000 mA) */
0x002D8D96L,	/* I_VLIM (0.000 mA) */
0x005B0AFBL,	/* LCRONHK (10.000 mA) */
0x006D4060L,	/* LCROFFHK (12.000 mA) */
0x00008000L,	/* LCRDBI (5.000 ms) */
0x0048D595L,	/* LONGHITH (8.000 mA) */
0x003FBAE2L,	/* LONGLOTH (7.000 mA) */
0x00008000L,	/* LONGDBI (5.000 ms) */
0x000F0000L,	/* LCRMASK (150.000 ms) */
0x00080000L,	/* LCRMASK_POLREV (80.000 ms) */
0x00140000L,	/* LCRMASK_STATE (200.000 ms) */
0x00140000L,	/* LCRMASK_LINECAP (200.000 ms) */
0x01BA5E35L,	/* VCM_OH (27.000 v) */
0x0051EB85L,	/* VOV_BAT (5.000 v) */
0x00418937L,	/* VOV_GND (4.000 v) */
},  /* DCFEED_48V_20MA */
{
0x1C8A024CL,	/* SLOPE_VLIM */
0x1EE08C11L,	/* SLOPE_RFEED */
0x0040A0E0L,	/* SLOPE_ILIM */
0x1C940D71L,	/* SLOPE_DELTA1 */
0x1DD87A3EL,	/* SLOPE_DELTA2 */
0x05A38633L,	/* V_VLIM (48.000 v) */
0x050D2839L,	/* V_RFEED (43.000 v) */
0x03FE7F0FL,	/* V_ILIM  (34.000 v) */
0x01241BC9L,	/* CONST_RFEED (15.000 mA) */
0x0074538FL,	/* CONST_ILIM (25.000 mA) */
0x002D8D96L,	/* I_VLIM (0.000 mA) */
0x005B0AFBL,	/* LCRONHK (10.000 mA) */
0x006D4060L,	/* LCROFFHK (12.000 mA) */
0x00008000L,	/* LCRDBI (5.000 ms) */
0x0048D595L,	/* LONGHITH (8.000 mA) */
0x003FBAE2L,	/* LONGLOTH (7.000 mA) */
0x00008000L,	/* LONGDBI (5.000 ms) */
0x000F0000L,	/* LCRMASK (150.000 ms) */
0x00080000L,	/* LCRMASK_POLREV (80.000 ms) */
0x00140000L,	/* LCRMASK_STATE (200.000 ms) */
0x00140000L,	/* LCRMASK_LINECAP (200.000 ms) */
0x01BA5E35L,	/* VCM_OH (27.000 v) */
0x0051EB85L,	/* VOV_BAT (5.000 v) */
0x00418937L,	/* VOV_GND (4.000 v) */
},  /* DCFEED_48V_25MA */
{
0x1E655196L,	/* SLOPE_VLIM */
0x001904EFL,	/* SLOPE_RFEED */
0x0040A0E0L,	/* SLOPE_ILIM */
0x1B4CAD9EL,	/* SLOPE_DELTA1 */
0x1BB0F47CL,	/* SLOPE_DELTA2 */
0x05A38633L,	/* V_VLIM (48.000 v) */
0x043AA4A6L,	/* V_RFEED (36.000 v) */
0x025977EAL,	/* V_ILIM  (20.000 v) */
0x0068B19AL,	/* CONST_RFEED (18.000 mA) */
0x005D0FA6L,	/* CONST_ILIM (20.000 mA) */
0x002D8D96L,	/* I_VLIM (0.000 mA) */
0x005B0AFBL,	/* LCRONHK (10.000 mA) */
0x006D4060L,	/* LCROFFHK (12.000 mA) */
0x00008000L,	/* LCRDBI (5.000 ms) */
0x0048D595L,	/* LONGHITH (8.000 mA) */
0x003FBAE2L,	/* LONGLOTH (7.000 mA) */
0x00008000L,	/* LONGDBI (5.000 ms) */
0x000F0000L,	/* LCRMASK (150.000 ms) */
0x00080000L,	/* LCRMASK_POLREV (80.000 ms) */
0x00140000L,	/* LCRMASK_STATE (200.000 ms) */
0x00140000L,	/* LCRMASK_LINECAP (200.000 ms) */
0x01BA5E35L,	/* VCM_OH (27.000 v) */
0x0051EB85L,	/* VOV_BAT (5.000 v) */
0x00418937L,	/* VOV_GND (4.000 v) */
},  /* DCFEED_PSTN_DET_1 */
{
0x1A10433FL,	/* SLOPE_VLIM */
0x1C206275L,	/* SLOPE_RFEED */
0x0040A0E0L,	/* SLOPE_ILIM */
0x1C1F426FL,	/* SLOPE_DELTA1 */
0x1EB51625L,	/* SLOPE_DELTA2 */
0x041C91DBL,	/* V_VLIM (35.000 v) */
0x03E06C43L,	/* V_RFEED (33.000 v) */
0x038633E0L,	/* V_ILIM  (30.000 v) */
0x022E5DE5L,	/* CONST_RFEED (10.000 mA) */
0x005D0FA6L,	/* CONST_ILIM (20.000 mA) */
0x0021373DL,	/* I_VLIM (0.000 mA) */
0x005B0AFBL,	/* LCRONHK (10.000 mA) */
0x006D4060L,	/* LCROFFHK (12.000 mA) */
0x00008000L,	/* LCRDBI (5.000 ms) */
0x0048D595L,	/* LONGHITH (8.000 mA) */
0x003FBAE2L,	/* LONGLOTH (7.000 mA) */
0x00008000L,	/* LONGDBI (5.000 ms) */
0x000F0000L,	/* LCRMASK (150.000 ms) */
0x00080000L,	/* LCRMASK_POLREV (80.000 ms) */
0x00140000L,	/* LCRMASK_STATE (200.000 ms) */
0x00140000L,	/* LCRMASK_LINECAP (200.000 ms) */
0x01BA5E35L,	/* VCM_OH (27.000 v) */
0x0051EB85L,	/* VOV_BAT (5.000 v) */
0x00418937L,	/* VOV_GND (4.000 v) */
}   /* DCFEED_PSTN_DET_2 */
};

Si3218x_Impedance_Cfg Si3218x_Impedance_Presets[] ={
/* Source: Database file: cwdb.db */
/* Database information: */
/* parameters: zref=600_0_0 rprot=30 rfuse=24 emi_cap=10*/
{
{0x07F3A400L, 0x000FF180L, 0x00009380L, 0x1FFDA800L,    /* TXACEQ */
 0x07EF1600L, 0x0014B500L, 0x1FFD6580L, 0x1FFCA400L},   /* RXACEQ */
{0x0008EF00L, 0x00099780L, 0x017DF600L, 0x0096B900L,    /* ECFIR/ECIIR */
 0x02549000L, 0x1E4B7D00L, 0x018EEE00L, 0x1EEE0600L,
 0x008A8080L, 0x1F713080L, 0x0489BA00L, 0x03592500L},
{0x0086CE00L, 0x1EF46980L, 0x0084CB00L, 0x0FE34F00L,    /* ZSYNTH */
 0x181CA780L, 0x5D}, 
 0x08EB8E00L,   /* TXACGAIN */
 0x01532100L,   /* RXACGAIN */
 0x07AA7180L, 0x18558F00L, 0x0754E300L,    /* RXACHPF */
#ifdef ENABLE_HIRES_GAIN
 0, 0  /* TXGAIN*10, RXGAIN*10 (hi_res) */
#else
 0, 0  /* TXGAIN, RXGAIN */
#endif
 },  /* ZSYN_600_0_0_30_0 */
/* Source: Database file: cwdb.db */
/* Database information: */
/* parameters: zref=270_750_150 rprot=30 rfuse=24 emi_cap=10*/
{
{0x0750E500L, 0x1FC70280L, 0x000BA980L, 0x1FFD2880L,    /* TXACEQ */
 0x0A8E2380L, 0x1B905280L, 0x00847700L, 0x1FDAFA00L},   /* RXACEQ */
{0x002C8880L, 0x1F630D80L, 0x027F7980L, 0x1F3AD200L,    /* ECFIR/ECIIR */
 0x040B8680L, 0x1F414D00L, 0x01427B00L, 0x00208200L,
 0x0026AE00L, 0x1FD71680L, 0x0C8EDB00L, 0x1B688A00L},
{0x1F657980L, 0x0096FE00L, 0x00035500L, 0x0D7FE800L,    /* ZSYNTH */
 0x1A7F1A80L, 0xB4}, 
 0x08000000L,   /* TXACGAIN */
 0x01106B80L,   /* RXACGAIN */
 0x07BC8400L, 0x18437C80L, 0x07790880L,    /* RXACHPF */
#ifdef ENABLE_HIRES_GAIN
 0, 0  /* TXGAIN*10, RXGAIN*10 (hi_res) */
#else
 0, 0  /* TXGAIN, RXGAIN */
#endif
 },  /* ZSYN_270_750_150_30_0 */
/* Source: Database file: cwdb.db */
/* Database information: */
/* parameters: zref=370_620_310 rprot=30 rfuse=24 emi_cap=10*/
{
{0x08363C80L, 0x1FB03200L, 0x1FFBD200L, 0x1FFC7A00L,    /* TXACEQ */
 0x0A0D0800L, 0x1BEB0880L, 0x1F9DF080L, 0x1FE07F00L},   /* RXACEQ */
{0x00236380L, 0x1F947D00L, 0x020DE380L, 0x1FBEED00L,    /* ECFIR/ECIIR */
 0x03050300L, 0x1F7D1D00L, 0x010A9F80L, 0x00329D80L,
 0x003E4100L, 0x1FC0DF00L, 0x0DAADE80L, 0x1A4F2600L},
{0x00226100L, 0x1F8EEE80L, 0x004E9D00L, 0x0F0B9B00L,    /* ZSYNTH */
 0x18F3E580L, 0x99}, 
 0x0808D100L,   /* TXACGAIN */
 0x0131BE80L,   /* RXACGAIN */
 0x07B5C100L, 0x184A3F80L, 0x076B8200L,    /* RXACHPF */
#ifdef ENABLE_HIRES_GAIN
 0, 0  /* TXGAIN*10, RXGAIN*10 (hi_res) */
#else
 0, 0  /* TXGAIN, RXGAIN */
#endif
 },  /* ZSYN_370_620_310_30_0 */
/* Source: Database file: cwdb.db */
/* Database information: */
/* parameters: zref=220_820_120 rprot=30 rfuse=24 emi_cap=10*/
{
{0x07194B80L, 0x1FC63800L, 0x0008D280L, 0x1FFC0600L,    /* TXACEQ */
 0x0A849680L, 0x1BB04480L, 0x00A4AA00L, 0x1FD3E680L},   /* RXACEQ */
{0x001B8C00L, 0x1FC65400L, 0x016A5F00L, 0x01323C80L,    /* ECFIR/ECIIR */
 0x01DB4980L, 0x01484700L, 0x00258000L, 0x007E9C80L,
 0x0016FF00L, 0x1FE69100L, 0x0CE9A400L, 0x1B0EA980L},
{0x00B3D800L, 0x1D2F8280L, 0x021C8B00L, 0x0A157F00L,    /* ZSYNTH */
 0x1DE99E80L, 0xAD}, 
 0x08000000L,   /* TXACGAIN */
 0x01084680L,   /* RXACGAIN */
 0x07BBFA80L, 0x18440600L, 0x0777F580L,    /* RXACHPF */
#ifdef ENABLE_HIRES_GAIN
 0, 0  /* TXGAIN*10, RXGAIN*10 (hi_res) */
#else
 0, 0  /* TXGAIN, RXGAIN */
#endif
 },  /* ZSYN_220_820_120_30_0 */
/* Source: Database file: cwdb.db */
/* Database information: */
/* parameters: zref=600_0_0 rprot=30 rfuse=24 emi_cap=10*/
{
{0x07F3A400L, 0x000FF180L, 0x00009380L, 0x1FFDA800L,    /* TXACEQ */
 0x07EF1600L, 0x0014B500L, 0x1FFD6580L, 0x1FFCA400L},   /* RXACEQ */
{0x0008EF00L, 0x00099780L, 0x017DF600L, 0x0096B900L,    /* ECFIR/ECIIR */
 0x02549000L, 0x1E4B7D00L, 0x018EEE00L, 0x1EEE0600L,
 0x008A8080L, 0x1F713080L, 0x0489BA00L, 0x03592500L},
{0x0086CE00L, 0x1EF46980L, 0x0084CB00L, 0x0FE34F00L,    /* ZSYNTH */
 0x181CA780L, 0x5D}, 
 0x08EB8E00L,   /* TXACGAIN */
 0x01532100L,   /* RXACGAIN */
 0x07AA7180L, 0x18558F00L, 0x0754E300L,    /* RXACHPF */
#ifdef ENABLE_HIRES_GAIN
 0, 0  /* TXGAIN*10, RXGAIN*10 (hi_res) */
#else
 0, 0  /* TXGAIN, RXGAIN */
#endif
 },  /* ZSYN_600_0_1000_30_0 */
/* Source: Database file: cwdb.db */
/* Database information: */
/* parameters: zref=200_680_100 rprot=30 rfuse=24 emi_cap=10*/
{
{0x0778B980L, 0x1FB97E00L, 0x00030780L, 0x1FFC2580L,    /* TXACEQ */
 0x09CC0780L, 0x1D104400L, 0x0076CB80L, 0x1FDE3D80L},   /* RXACEQ */
{0x1FF64C00L, 0x00456280L, 0x00BEC500L, 0x014D3E80L,    /* ECFIR/ECIIR */
 0x02EB2B00L, 0x1E983B80L, 0x029EE280L, 0x1E7B7400L,
 0x00D19A80L, 0x1F293D80L, 0x06116D00L, 0x01D55C00L},
{0x01241700L, 0x1CB53A80L, 0x02269400L, 0x0A14BA00L,    /* ZSYNTH */
 0x1DE9D080L, 0x99}, 
 0x08000000L,   /* TXACGAIN */
 0x01152480L,   /* RXACGAIN */
 0x07B96C00L, 0x18469480L, 0x0772D800L,    /* RXACHPF */
#ifdef ENABLE_HIRES_GAIN
 0, 0  /* TXGAIN*10, RXGAIN*10 (hi_res) */
#else
 0, 0  /* TXGAIN, RXGAIN */
#endif
 },  /* ZSYN_200_680_100_30_0 */
/* Source: Database file: cwdb.db */
/* Database information: */
/* parameters: zref=220_820_115 rprot=30 rfuse=24 emi_cap=10*/
{
{0x070AC700L, 0x1FCC7280L, 0x00098700L, 0x1FFCE080L,    /* TXACEQ */
 0x0A6A6400L, 0x1BE48B80L, 0x009F3B80L, 0x1FD56000L},   /* RXACEQ */
{0x00314700L, 0x1F6C1D80L, 0x02347480L, 0x00158B80L,    /* ECFIR/ECIIR */
 0x03173D00L, 0x0058E580L, 0x00A6DA80L, 0x004B0780L,
 0x001B1300L, 0x1FE2DE80L, 0x0C313180L, 0x1BB7FE00L},
{0x1FD95980L, 0x1ECDE680L, 0x0156F600L, 0x0A0C9600L,    /* ZSYNTH */
 0x1DEBF080L, 0xB4}, 
 0x08000000L,   /* TXACGAIN */
 0x01069C80L,   /* RXACGAIN */
 0x07BECB80L, 0x18413500L, 0x077D9700L,    /* RXACHPF */
#ifdef ENABLE_HIRES_GAIN
 0, 0  /* TXGAIN*10, RXGAIN*10 (hi_res) */
#else
 0, 0  /* TXGAIN, RXGAIN */
#endif
 },  /* ZSYN_220_820_115_30_0 */
/* Source: Database file: cwdb.db */
/* Database information: */
/* parameters: zref=600_0_0 rprot=30 rfuse=24 emi_cap=0*/
{
{0x081A5300L, 0x1FE00A00L, 0x00071580L, 0x1FFE2600L,    /* TXACEQ */
 0x07F9A800L, 0x1FFA7D80L, 0x1FF59E80L, 0x1FFF1400L},   /* RXACEQ */
{0x0052DC80L, 0x1F455780L, 0x0297A080L, 0x0006D980L,    /* ECFIR/ECIIR */
 0x0195DC00L, 0x000E1E80L, 0x1FC53680L, 0x00050800L,
 0x00233400L, 0x1FE6DC00L, 0x1FCD1E00L, 0x1FD2FE00L},
{0x007B9C00L, 0x1F296C80L, 0x005BCD00L, 0x09F07F00L,    /* ZSYNTH */
 0x1DF35080L, 0x6F}, 
 0x08C2DA80L,   /* TXACGAIN */
 0x01495A80L,   /* RXACGAIN */
 0x07BECC80L, 0x18413400L, 0x077D9900L,    /* RXACHPF */
#ifdef ENABLE_HIRES_GAIN
 0, 0  /* TXGAIN*10, RXGAIN*10 (hi_res) */
#else
 0, 0  /* TXGAIN, RXGAIN */
#endif
 }   /* WB_ZSYN_600_0_0_20_0 */
};

Si3218x_FSK_Cfg Si3218x_FSK_Presets[] ={
{
{
0x02232000L,	 /* FSK01 */
0x077C2000L 	 /* FSK10 */
},
{
0x0015C000L,	 /* FSKAMP0 (0.080 vrms )*/
0x000BA000L 	 /* FSKAMP1 (0.080 vrms) */
},
{
0x06B60000L,	 /* FSKFREQ0 (2200.0 Hz space) */
0x079C0000L 	 /* FSKFREQ1 (1200.0 Hz mark) */
},
0x00,			 /* FSK8 */
0x00,			 /* FSKDEPTH (1 deep fifo) */
},  /* DEFAULT_FSK */
{
{
0x026E4000L,	 /* FSK01 */
0x0694C000L 	 /* FSK10 */
},
{
0x0014C000L,	 /* FSKAMP0 (0.080 vrms )*/
0x000CA000L 	 /* FSKAMP1 (0.080 vrms) */
},
{
0x06D20000L,	 /* FSKFREQ0 (2100.0 Hz space) */
0x078B0000L 	 /* FSKFREQ1 (1300.0 Hz mark) */
},
0x00,			 /* FSK8 */
0x00,			 /* FSKDEPTH (1 deep fifo) */
}   /* ETSI_FSK */
};

Si3218x_PulseMeter_Cfg Si3218x_PulseMeter_Presets[] ={
{
0x007A2B6AL,  /* PM_AMP_THRESH (1.000) */
0,            /* Freq (12kHz) */ 
0,            /* PM_AUTO (off)*/
0x07D00000L,  /* PM_active (2000 ms) */
0x07D00000L   /* PM_inactive (2000 ms) */
 }   /* DEFAULT_PULSE_METERING */
};

Si3218x_Tone_Cfg Si3218x_Tone_Presets[] = {
{
	{
	0x07B30000L,	 /* OSC1FREQ (350.000 Hz) */
	0x000C6000L,	 /* OSC1AMP (-18.000 dBm) */
	0x00000000L,	 /* OSC1PHAS (0.000 rad) */
	0x00,			 /* O1TALO (0 ms) */
	0x00,			 /* O1TAHI */
	0x00,			 /* O1TILO (0 ms) */
	0x00			 /* O1TIHI */
	},
	{
	0x07870000L,	 /* OSC2FREQ (440.000 Hz) */
	0x000FA000L,	 /* OSC2AMP (-18.000 dBm) */
	0x00000000L,	 /* OSC2PHAS (0.000 rad) */
	0x00,			 /* O2TALO (0 ms) */
	0x00,			 /* O2TAHI */
	0x00,			 /* O2TILO (0 ms) */
	0x00 			 /* O2TIHI */
	},
	0x66 			 /* OMODE */
},  /* TONEGEN_FCC_DIAL */
{
	{
	0x07700000L,	 /* OSC1FREQ (480.000 Hz) */
	0x00112000L,	 /* OSC1AMP (-18.000 dBm) */
	0x00000000L,	 /* OSC1PHAS (0.000 rad) */
	0xA0,			 /* O1TALO (500 ms) */
	0x0F,			 /* O1TAHI */
	0xA0,			 /* O1TILO (500 ms) */
	0x0F			 /* O1TIHI */
	},
	{
	0x07120000L,	 /* OSC2FREQ (620.000 Hz) */
	0x00164000L,	 /* OSC2AMP (-18.000 dBm) */
	0x00000000L,	 /* OSC2PHAS (0.000 rad) */
	0xA0,			 /* O2TALO (500 ms) */
	0x0F,			 /* O2TAHI */
	0xA0,			 /* O2TILO (500 ms) */
	0x0F 			 /* O2TIHI */
	},
	0x66 			 /* OMODE */
},  /* TONEGEN_FCC_BUSY */
{
	{
	0x07700000L,	 /* OSC1FREQ (480.000 Hz) */
	0x00112000L,	 /* OSC1AMP (-18.000 dBm) */
	0x00000000L,	 /* OSC1PHAS (0.000 rad) */
	0x80,			 /* O1TALO (2000 ms) */
	0x3E,			 /* O1TAHI */
	0x00,			 /* O1TILO (4000 ms) */
	0x7D			 /* O1TIHI */
	},
	{
	0x07870000L,	 /* OSC2FREQ (440.000 Hz) */
	0x000FA000L,	 /* OSC2AMP (-18.000 dBm) */
	0x00000000L,	 /* OSC2PHAS (0.000 rad) */
	0x80,			 /* O2TALO (2000 ms) */
	0x3E,			 /* O2TAHI */
	0x00,			 /* O2TILO (4000 ms) */
	0x7D 			 /* O2TIHI */
	},
	0x66 			 /* OMODE */
},  /* TONEGEN_FCC_RINGBACK */
{
	{
	0x07700000L,	 /* OSC1FREQ (480.000 Hz) */
	0x00112000L,	 /* OSC1AMP (-18.000 dBm) */
	0x00000000L,	 /* OSC1PHAS (0.000 rad) */
	0x60,			 /* O1TALO (300 ms) */
	0x09,			 /* O1TAHI */
	0x60,			 /* O1TILO (300 ms) */
	0x09			 /* O1TIHI */
	},
	{
	0x07120000L,	 /* OSC2FREQ (620.000 Hz) */
	0x00164000L,	 /* OSC2AMP (-18.000 dBm) */
	0x00000000L,	 /* OSC2PHAS (0.000 rad) */
	0x60,			 /* O2TALO (300 ms) */
	0x09,			 /* O2TAHI */
	0x40,			 /* O2TILO (200 ms) */
	0x06 			 /* O2TIHI */
	},
	0x66 			 /* OMODE */
},  /* TONEGEN_FCC_REORDER */
{
	{
	0x07700000L,	 /* OSC1FREQ (480.000 Hz) */
	0x00112000L,	 /* OSC1AMP (-18.000 dBm) */
	0x00000000L,	 /* OSC1PHAS (0.000 rad) */
	0x40,			 /* O1TALO (200 ms) */
	0x06,			 /* O1TAHI */
	0x40,			 /* O1TILO (200 ms) */
	0x06			 /* O1TIHI */
	},
	{
	0x07120000L,	 /* OSC2FREQ (620.000 Hz) */
	0x00164000L,	 /* OSC2AMP (-18.000 dBm) */
	0x00000000L,	 /* OSC2PHAS (0.000 rad) */
	0x40,			 /* O2TALO (200 ms) */
	0x06,			 /* O2TAHI */
	0x40,			 /* O2TILO (200 ms) */
	0x06 			 /* O2TIHI */
	},
	0x66 			 /* OMODE */
},  /* TONEGEN_FCC_CONGESTION */
{
	{
	0x1F2F0000L,	 /* OSC1FREQ (2130.000 Hz) */
	0x0063A000L,	 /* OSC1AMP (-18.000 dBm) */
	0x00000000L,	 /* OSC1PHAS (0.000 rad) */
	0x80,			 /* O1TALO (80 ms) */
	0x02,			 /* O1TAHI */
	0x80,			 /* O1TILO (80 ms) */
	0x02			 /* O1TIHI */
	},
	{
	0x1B8E0000L,	 /* OSC2FREQ (2750.000 Hz) */
	0x00A84000L,	 /* OSC2AMP (-18.000 dBm) */
	0x00000000L,	 /* OSC2PHAS (0.000 rad) */
	0x80,			 /* O2TALO (80 ms) */
	0x02,			 /* O2TAHI */
	0x40,			 /* O2TILO (1000 ms) */
	0x1F 			 /* O2TIHI */
	},
	0x66 			 /* OMODE */
},  /* TONEGEN_FCC_CAS */
{
	{
	0x07870000L,	 /* OSC1FREQ (440.000 Hz) */
	0x000FA000L,	 /* OSC1AMP (-18.000 dBm) */
	0x00000000L,	 /* OSC1PHAS (0.000 rad) */
	0x60,			 /* O1TALO (300 ms) */
	0x09,			 /* O1TAHI */
	0x00,			 /* O1TILO (8000 ms) */
	0xFA			 /* O1TIHI */
	},
	{
	0x1B8E0000L,	 /* OSC2FREQ (2750.000 Hz) */
	0x00A84000L,	 /* OSC2AMP (-18.000 dBm) */
	0x00000000L,	 /* OSC2PHAS (0.000 rad) */
	0x80,			 /* O2TALO (2000 ms) */
	0x3E,			 /* O2TAHI */
	0x00,			 /* O2TILO (4000 ms) */
	0x7D 			 /* O2TIHI */
	},
	0x06 			 /* OMODE */
},  /* TONEGEN_FCC_SAS */
{
	{
	0x1F2F0000L,	 /* OSC1FREQ (2130.000 Hz) */
	0x01BD0000L,	 /* OSC1AMP (-5.000 dBm) */
	0x00000000L,	 /* OSC1PHAS (0.000 rad) */
	0x20,			 /* O1TALO (100 ms) */
	0x03,			 /* O1TAHI */
	0x20,			 /* O1TILO (100 ms) */
	0x03			 /* O1TIHI */
	},
	{
	0x1B8E0000L,	 /* OSC2FREQ (2750.000 Hz) */
	0x02EFC000L,	 /* OSC2AMP (-5.000 dBm) */
	0x00000000L,	 /* OSC2PHAS (0.000 rad) */
	0x20,			 /* O2TALO (100 ms) */
	0x03,			 /* O2TAHI */
	0x20,			 /* O2TILO (100 ms) */
	0x03 			 /* O2TIHI */
	},
	0x66 			 /* OMODE */
},  /* TONEGEN_ETSI_DTAS */
{
	{
	0x05A40000L,	 /* OSC1FREQ (1004.000 Hz) */
	0x005DE000L,	 /* OSC1AMP (-10.000 dBm) */
	0x00000000L,	 /* OSC1PHAS (0.000 rad) */
	0x00,			 /* O1TALO (8000 ms) */
	0xFA,			 /* O1TAHI */
	0x00,			 /* O1TILO (8000 ms) */
	0xFA			 /* O1TIHI */
	},
	{
	0x07870000L,	 /* OSC2FREQ (440.000 Hz) */
	0x000FA000L,	 /* OSC2AMP (-18.000 dBm) */
	0x00000000L,	 /* OSC2PHAS (0.000 rad) */
	0x80,			 /* O2TALO (2000 ms) */
	0x3E,			 /* O2TAHI */
	0x00,			 /* O2TILO (4000 ms) */
	0x7D 			 /* O2TIHI */
	},
	0x47 			 /* OMODE */
}   /* TONEGEN_1004 */
};

Si3218x_PCM_Cfg Si3218x_PCM_Presets[] ={
	{
	0x01, 	 /* PCM_FMT - u-Law */
	0x00, 	 /* WIDEBAND - DISABLED (3.4kHz BW) */
	0x00, 	 /* PCM_TRI - PCLK RISING EDGE */
	0x00, 	 /* TX_EDGE - PCLK RISING EDGE */
	0x00 	 /* A-LAW -  INVERT NONE */
	},  /* PCM_8ULAW */
	{
	0x00, 	 /* PCM_FMT - A-Law */
	0x00, 	 /* WIDEBAND - DISABLED (3.4kHz BW) */
	0x00, 	 /* PCM_TRI - PCLK RISING EDGE */
	0x00, 	 /* TX_EDGE - PCLK RISING EDGE */
	0x00 	 /* A-LAW -  INVERT NONE */
	},  /* PCM_8ALAW */
	{
	0x03, 	 /* PCM_FMT - 16-bit Linear */
	0x00, 	 /* WIDEBAND - DISABLED (3.4kHz BW) */
	0x00, 	 /* PCM_TRI - PCLK RISING EDGE */
	0x00, 	 /* TX_EDGE - PCLK RISING EDGE */
	0x00 	 /* A-LAW -  INVERT NONE */
	},  /* PCM_16LIN */
	{
	0x03, 	 /* PCM_FMT - 16-bit Linear */
	0x01, 	 /* WIDEBAND - ENABLED (7kHz BW) */
	0x00, 	 /* PCM_TRI - PCLK RISING EDGE */
	0x00, 	 /* TX_EDGE - PCLK RISING EDGE */
	0x00 	 /* A-LAW -  INVERT NONE */
	}   /* PCM_16LIN_WB */
};

