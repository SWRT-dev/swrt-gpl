#ifndef _XPCS_H_
#define _XPCS_H_

#include <linux/types.h>
#include <linux/ethtool.h>

#define XPCS_SUCCESS			0
#define XPCS_FAILURE			-1

#define XPCS_DEV_NAME			"intel_xpcs"

#define WAN_XPCS_BASE_ADD		0x18C42000
#define XPCS0_BASE_ADD			0x1A342000
#define XPCS1_BASE_ADD			0x1A842000

/* XPCS register offsets */
#define PMA_CTRL1			0x40000
#define PMA_STS1			0x40004
#define PMA_DEV1			0x40008
#define PMA_DEV2			0x4000C
#define PMA_SPD_ABL			0x40010
#define PMA_DEV_PKG1			0x40014
#define PMA_DEV_PKG2			0x40018
#define PMA_CTRL2			0x4001C
#define PMA_STS2			0x40020
#define PMA_TXDIS			0x40024
#define PMA_RXSIG_DET			0x40028
#define PMA_EXT_ABL			0x4002C
#define PMA_PKG1			0x40038
#define PMA_PKG2			0x4003C
#define PMA_KR_PMD_CTRL			0x40258
#define PMA_KR_PMD_STS			0x4025C
#define PMA_KR_LP_CEU			0x40260
#define PMA_KR_LP_CESTS			0x40264
#define PMA_KR_LD_CEU			0x40268
#define PMA_KR_LD_CESTS			0x4026C
#define PMA_KX_CTRL			0x40280
#define PMA_KX_STS			0x40284
#define PMA_KR_ABL			0x402A8
#define PMA_KR_CTRL			0x402AC
#define PMA_KR_CORR_BLK1		0x402B0
#define PMA_KR_CORR_BLK2		0x402B4
#define PMA_KR_UCORR_BLK1		0x402B8
#define PMA_KR_UCORR_BLK2		0x402BC
#define PMA_TS_PMA_ABL			0x41C20
#define PMA_TS_TMAX_L			0x41C24
#define PMA_TS_TMAX_U			0x41C28
#define PMA_TS_TMIN_L			0x41C2C
#define PMA_TS_TMIN_U			0x41C30
#define PMA_TS_RMAX_L			0x41C34
#define PMA_TS_RMAX_U			0x41C38
#define PMA_TS_RMIN_L			0x41C3C
#define PMA_TS_RMIN_U			0x41C40

/* Gen5 12G */
#define PMA_DIG_CTRL1			0x60000
#define PMA_KRTR_PRBS_C0		0x6000C
#define PMA_KRTR_PRBS_C1		0x60010
#define PMA_KRTR_PRBS_C2		0x60014
#define PMA_KRTR_TR_C0			0x60018
#define PMA_KRTR_TR_C1			0x6001C
#define PMA_KRTR_TR_C2			0x60020
#define PMA_KRTR_RXEQ			0x60024
#define PMA_KRTR_TXEQ_S			0x6002C
#define PMA_KRTR_TXEQ_CFF		0x60030
#define PMA_PHY_TXEQ_S			0x60034
#define PMA_DIG_STS			0x60040
#define PMA_RX_LSTS			0x60080
#define PMA_TX_GENCTRL0			0x600C0
#define PMA_TX_GENCTRL1			0x600C4
#define PMA_TX_GENCTRL2			0x600C8
#define PMA_TX_BOOST_CTRL		0x600CC
#define PMA_TX_RATE_CTRL		0x600D0
#define PMA_TX_PS_CTRL			0x600D4
#define PMA_TX_EQ_C0			0x600D8
#define PMA_TX_EQ_C1			0x600DC
#define PMA_TX_EQ_C2			0x600E0
#define PMA_TX_EQ_C3			0x600E4
#define PMA_EQ_INIT_CTRL0		0x600E8
#define PMA_EQ_INIT_CTRL1		0x600EC
#define PMA_TX_STS			0x60100
#define PMA_RX_GENCTRL0			0x60140
#define PMA_RX_GENCTRL1			0x60144
#define PMA_RX_GENCTRL2			0x60148
#define PMA_RX_GENCTRL3			0x6014C
#define PMA_RX_RATE_CTRL		0x60150
#define PMA_RX_PS_CTRL			0x60154
#define PMA_RX_CDR_CTRL			0x60158
#define PMA_RX_ATTN_CTRL		0x6015C
#define PMA_RX_EQ_CTRL0			0x60160
#define PMA_RX_EQ_CTRL1			0x60164
#define PMA_RX_EQ_CTRL2			0x60168
#define PMA_RX_EQ_CTRL3			0x6016C
#define PMA_RX_EQ_CTRL4			0x60170
#define PMA_AFE_DFE_EN_CTRL		0x60174
#define PMA_DFE_TAP_CTRL0		0x60178
#define PMA_DFE_TAP_CTRL1		0x6017C
#define PMA_RX_STS			0x60180
#define PMA_MPLL_CMN_CTRL		0x601C0
#define PMA_MPLLA_C0			0x601C4
#define PMA_MPLLA_C1			0x601C8
#define PMA_MPLLA_C2			0x601CC
#define PMA_MPLLB_C0			0x601D0
#define PMA_MPLLB_C1			0x601D4
#define PMA_MPLLB_C2			0x601D8
#define PMA_MPLLA_C3			0x601DC
#define PMA_MPLLB_C3			0x601E0
#define PMA_MISC_C0			0x60240
#define PMA_REF_CLK_CTRL		0x60244
#define PMA_VCO_CAL_LD0			0x60248
#define PMA_VCO_CAL_LD1			0x6024C
#define PMA_VCO_CAL_LD2			0x60250
#define PMA_VCO_CAL_LD3			0x60254
#define PMA_VCO_CAL_REF0		0x60258
#define PMA_VCO_CAL_REF1		0x6025C
#define PMA_MISC_STS			0x60260
#define PMA_MISC_CTRL1			0x60264
#define PMA_EEE_CTRL			0x60268
#define PMA_SRAM			0x6026C
#define PMA_SNPS_CR_CTRL		0x60280
#define PMA_SNPS_CR_ADDR		0x60284
#define PMA_SNPS_CR_DATA		0x60288

/* PCS */
#define PCS_CTRL1			0xC0000
#define PCS_STS1			0xC0004
#define PCS_DEV1			0xC0008
#define PCS_DEV2			0xC000C
#define PCS_SPD_ABL			0xC0010
#define PCS_DEV_PKG1			0xC0014
#define PCS_DEV_PKG2			0xC0018
#define PCS_CTRL2			0xC001C
#define PCS_STS2			0xC0020
#define PCS_PKG1			0xC0038
#define PCS_PKG2			0xC003C
#define PCS_EEE_ABL			0xC0050
#define PCS_EEE_WKERR			0xC0058
#define PCS_LSTS			0xC0060
#define PCS_TCTRL			0xC0064
#define PCS_KR_STS1			0xC0080
#define PCS_KR_STS2			0xC0084
#define PCS_TP_A0			0xC0088
#define PCS_TP_A1			0xC008C
#define PCS_TP_A2			0xC0090
#define PCS_TP_A3			0xC0094
#define PCS_TP_B0			0xC0098
#define PCS_TP_B1			0xC009C
#define PCS_TP_B2			0xC00A0
#define PCS_TP_B3			0xC00A4
#define PCS_TP_CTRL			0xC00A8
#define PCS_TP_ERRCR			0xC00AC
#define PCS_TS_PCS_ABL			0xC1C20
#define PCS_TS_TMAX_L			0xC1C24
#define PCS_TS_TMAX_U			0xC1C28
#define PCS_TS_TMIN_L			0xC1C2C
#define PCS_TS_TMIN_U			0xC1C30
#define PCS_TS_RMAX_L			0xC1C34
#define PCS_TS_RMAX_U			0xC1C38
#define PCS_TS_RMIN_L			0xC1C3C
#define PCS_TS_RMIN_U			0xC1C40
#define PCS_DIG_CTRL1			0xE0000
#define PCS_DIG_CTRL2			0xE0004
#define PCS_DIG_ERRCNT_SEL		0xE0008
#define PCS_XAUI_CTRL			0xE0010
#define PCS_DBG_CTRL			0xE0014
#define PCS_EEE_MCTRL0			0xE0018
#define PCS_KR_CTRL			0xE001C
#define PCS_EEE_TXTIMER			0xE0020
#define PCS_EEE_RXTIMER			0xE0024
#define PCS_EEE_MCTRL1			0xE002C
#define PCS_DIG_STS			0xE0040
#define PCS_ICG_ERRCNT1			0xE0044
#define PCS_ICG_ERRCNT2			0xE0048
#define PCS_DSKW_ERRCTR			0xE004C
#define PCS_TPM_ERRCTR			0xE0050
#define PCS_GPIO			0xE0054

/* Auto Negotiation */
#define AN_CTRL				0x1C0000
#define AN_STS				0x1C0004
#define AN_DEV_ID1			0x1C0008
#define AN_DEV_ID2			0x1C000C
#define AN_DEV_PKG1			0x1C0014
#define AN_DEV_PKG2			0x1C0018
#define AN_PKG1				0x1C0038
#define AN_PKG2				0x1C003C
#define AN_ADV1				0x1C0040
#define AN_ADV2				0x1C0044
#define AN_ADV3				0x1C0048
#define AN_LP_ABL1			0x1C004C
#define AN_LP_ABL2			0x1C0050
#define AN_LP_ABL3			0x1C0054
#define AN_XNP_TX1			0x1C0058
#define AN_XNP_TX2			0x1C005C
#define AN_XNP_TX3			0x1C0060
#define AN_LP_XNP_ABL1			0x1C0064
#define AN_LP_XNP_ABL2			0x1C0068
#define AN_LP_XNP_ABL3			0x1C006C
#define AN_COMP_STS			0x1C00C0
#define AN_EEE_ABL			0x1C00F0
#define AN_EEE_LP_ABL			0x1C00F4
#define AN_DIG_CTRL			0x1E0000
#define AN_INTR_MSK			0x1E0004
#define AN_INTR				0x1E0008
#define AN_KR_MODE_CTRL			0x1E000C
#define AN_TIMER_CTRL0			0x1E0010
#define AN_TIMER_CTRL1			0x1E0014

/* MII 0,1,2,3 */
#define MII_CTRL(idx)			(0x680000 + ((idx) * 0x40000))
#define MII_STS(idx)			(0x680000 + ((idx) * 0x40000))
#define MII_DEV_ID1(idx)		(0x680000 + ((idx) * 0x40000))
#define MII_DEV_ID2(idx)		(0x680000 + ((idx) * 0x40000))
#define MII_AN_ADV(idx)			(0x680000 + ((idx) * 0x40000))
#define MII_LP_BABL(idx)		(0x680000 + ((idx) * 0x40000))
#define MII_EXPN(idx)			(0x680000 + ((idx) * 0x40000))
#define MII_EXT_STS(idx)		(0x680000 + ((idx) * 0x40000))
#define MII_DIG_CTRL1(idx)		(0x680000 + ((idx) * 0x40000))
#define MII_AN_CTRL(idx)		(0x680000 + ((idx) * 0x40000))
#define MII_AN_INTR_STS(idx)		(0x680000 + ((idx) * 0x40000))
#define MII_DBG_CTRL(idx)		(0x680000 + ((idx) * 0x40000))
#define MII_LINK_TIMER_CTRL(idx)	(0x680000 + ((idx) * 0x40000))
#define MII_DIG_STS(idx)		(0x680000 + ((idx) * 0x40000))
#define MII_ICG_ERRCNT1(idx)		(0x680000 + ((idx) * 0x40000))
#define MII_DIG_ERRCNT_SEL(idx)		(0x680000 + ((idx) * 0x40000))

/* MMD */
#define MMD_PMA_ID1			0x780000
#define MMD_PMA_ID2			0x780004
#define MMD_DEV_ID1			0x780008
#define MMD_DEV_ID2			0x78000C
#define MMD_PCS_ID1			0x780010
#define MMD_PCS_ID2			0x780014
#define MMD_AN_ID1			0x780018
#define MMD_AN_ID2			0x78001C
#define MMD_STS				0x780020
#define MMD_CTRL			0x780024
#define MMD_PKGID1			0x780038
#define MMD_PKGID2			0x78003C

#define SR_MII_CTRL			0x7C0000
#define SR_MII_STS			0x7C0004
#define SR_MII_DEV_ID1			0x7C0008
#define SR_MII_DEV_ID2			0x7C000C
#define SR_MII_AN_ADV			0x7C0010
#define SR_MII_LP_BABL			0x7C0014
#define SR_MII_EXPN			0x7C0018
#define SR_MII_EXT_STS			0x7C003C
#define VR_MII_DIG_CTRL1		0x7E0000
#define VR_MII_AN_CTRL			0x7E0004
#define VR_MII_AN_INTR_STS		0x7E0008
#define VR_MII_LINK_TIMER_CTRL		0x7E0028

#define AN_CTRL_AN_EN_POS			12
#define AN_CTRL_AN_EN_WIDTH			1
#define AN_CTRL_RSTRT_AN_POS			9
#define AN_CTRL_RSTRT_AN_WIDTH			1

#define AN_INTR_AN_INT_CMPLT_POS		0
#define AN_INTR_AN_INT_CMPLT_WIDTH		1
#define AN_INTR_AN_INC_LINK_POS			1
#define AN_INTR_AN_INC_LINK_WIDTH		1
#define AN_INTR_AN_PG_RCV_POS			2
#define AN_INTR_AN_PG_RCV_WIDTH			1

#define AN_STS_LP_AN_ABL_POS			0
#define AN_STS_LP_AN_ABL_WIDTH			1
#define AN_STS_AN_LS_POS			2
#define AN_STS_AN_LS_WIDTH			1
#define AN_STS_AN_ABL_POS			3
#define AN_STS_AN_ABL_WIDTH			1
#define AN_STS_AN_RF_POS			4
#define AN_STS_AN_RF_WIDTH			1
#define AN_STS_ANC_POS				5
#define AN_STS_ANC_WIDTH			1
#define AN_STS_PR_POS				6
#define AN_STS_PR_WIDTH				1
#define AN_STS_PR_POS				6
#define AN_STS_PR_WIDTH				1
#define AN_STS_EXT_NP_STS_POS			7
#define AN_STS_EXT_NP_STS_WIDTH			1
#define AN_STS_PDF_POS				9
#define AN_STS_PDF_WIDTH			1

#define AN_TIMER_CTRL1_INHBT_OR_WAIT_TIME_POS	0
#define AN_TIMER_CTRL1_INHBT_OR_WAIT_TIME_WIDTH	16

#define AN_DIG_CTRL_CL73_TMR_OVR_RIDE_POS	3
#define AN_DIG_CTRL_CL73_TMR_OVR_RIDE_WIDTH	1

#define SR_MII_STS_LINK_STS_POS			2
#define SR_MII_STS_LINK_STS_WIDTH		1

#define SR_MII_CTRL_SS13_POS			13
#define SR_MII_CTRL_SS13_WIDTH			1
#define SR_MII_CTRL_SS6_POS			6
#define SR_MII_CTRL_SS6_WIDTH			1
#define SR_MII_CTRL_SS5_POS			5
#define SR_MII_CTRL_SS5_WIDTH			1
#define SR_MII_CTRL_DUPLEX_MODE_POS		8
#define SR_MII_CTRL_DUPLEX_MODE_WIDTH		1
#define SR_MII_CTRL_AN_ENABLE_POS		12
#define SR_MII_CTRL_AN_ENABLE_WIDTH		1

#define VR_MII_AN_CTRL_PCS_MODE_POS		1
#define VR_MII_AN_CTRL_PCS_MODE_WIDTH		2
#define VR_MII_AN_CTRL_TX_CONFIG_POS		3
#define VR_MII_AN_CTRL_TX_CONFIG_WIDTH		1
#define VR_MII_AN_CTRL_MII_CONTROL_POS		8
#define VR_MII_AN_CTRL_MII_CONTROL_WIDTH	1
#define VR_MII_AN_CTRL_MII_AN_INTR_EN_POS	0
#define VR_MII_AN_CTRL_MII_AN_INTR_EN_WIDTH	1
#define VR_MII_AN_CTRL_SGMII_LINK_STS_POS	4
#define VR_MII_AN_CTRL_SGMII_LINK_STS_WIDTH	1

#define VR_MII_DIG_CTRL1_PHY_MODE_CTRL_POS	0
#define VR_MII_DIG_CTRL1_PHY_MODE_CTRL_WIDTH	1
#define VR_MII_DIG_CTRL1_MAC_AUTO_SW_POS	9
#define VR_MII_DIG_CTRL1_MAC_AUTO_SW_WIDTH	1
#define VR_MII_DIG_CTRL1_EN_2_5G_MODE_POS	2
#define VR_MII_DIG_CTRL1_EN_2_5G_MODE_WIDTH	1
#define VR_MII_DIG_CTRL1_CL37_TMR_OVR_RIDE_POS		3
#define VR_MII_DIG_CTRL1_CL37_TMR_OVR_RIDE_WIDTH	1

#define PMA_KR_PMD_CTRL_TR_EN_POS		1
#define PMA_KR_PMD_CTRL_TR_EN_WIDTH		1
#define PMA_KR_PMD_CTRL_RS_TR_POS		0
#define PMA_KR_PMD_CTRL_RS_TR_WIDTH		1

#define PMA_MPLL_CMN_CTRL_MPLLB_SEL_3_1_POS	5
#define PMA_MPLL_CMN_CTRL_MPLLB_SEL_3_1_WIDTH	3
#define PMA_MPLL_CMN_CTRL_MPLLB_SEL_0_POS	4
#define PMA_MPLL_CMN_CTRL_MPLLB_SEL_0_WIDTH	1

#define PMA_CTRL1_SS13_POS			13
#define PMA_CTRL1_SS13_WIDTH			1

#define PMA_MPLLA_C0_MPLLA_MULTIPLIER_POS	0
#define PMA_MPLLA_C0_MPLLA_MULTIPLIER_WIDTH	8

#define PMA_MPLLB_C0_MPLLB_MULTIPLIER_POS	0
#define PMA_MPLLB_C0_MPLLB_MULTIPLIER_WIDTH	8

#define PMA_MPLLA_C3_MPPLA_BANDWIDTH_POS	0
#define PMA_MPLLA_C3_MPPLA_BANDWIDTH_WIDTH	11

#define PMA_MPLLB_C3_MPPLB_BANDWIDTH_POS	0
#define PMA_MPLLB_C3_MPPLB_BANDWIDTH_WIDTH	11

#define PMA_VCO_CAL_LD0_VCO_LD_VAL_0_POS	0
#define PMA_VCO_CAL_LD0_VCO_LD_VAL_0_WIDTH	13
#define PMA_VCO_CAL_LD1_VCO_LD_VAL_1_POS	0
#define PMA_VCO_CAL_LD1_VCO_LD_VAL_1_WIDTH	13
#define PMA_VCO_CAL_LD2_VCO_LD_VAL_2_POS	0
#define PMA_VCO_CAL_LD2_VCO_LD_VAL_2_WIDTH	13
#define PMA_VCO_CAL_LD3_VCO_LD_VAL_3_POS	0
#define PMA_VCO_CAL_LD3_VCO_LD_VAL_3_WIDTH	13

#define PMA_VCO_CAL_REF0_VCO_REF_LD_0_POS	0
#define PMA_VCO_CAL_REF0_VCO_REF_LD_0_WIDTH	6
#define PMA_VCO_CAL_REF0_VCO_REF_LD_1_POS	8
#define PMA_VCO_CAL_REF0_VCO_REF_LD_1_WIDTH	6
#define PMA_VCO_CAL_REF1_VCO_REF_LD_2_POS	0
#define PMA_VCO_CAL_REF1_VCO_REF_LD_2_WIDTH	6
#define PMA_VCO_CAL_REF1_VCO_REF_LD_3_POS	8
#define PMA_VCO_CAL_REF1_VCO_REF_LD_3_WIDTH	6

#define PMA_AFE_DFE_EN_CTRL_AFE_EN_0_POS	0
#define PMA_AFE_DFE_EN_CTRL_AFE_EN_0_WIDTH	1
#define PMA_AFE_DFE_EN_CTRL_AFE_EN_3_1_POS	1
#define PMA_AFE_DFE_EN_CTRL_AFE_EN_3_1_WIDTH	3

#define PMA_AFE_DFE_EN_CTRL_DFE_EN_0_POS	4
#define PMA_AFE_DFE_EN_CTRL_DFE_EN_0_WIDTH	1
#define PMA_AFE_DFE_EN_CTRL_DFE_EN_3_1_POS	5
#define PMA_AFE_DFE_EN_CTRL_DFE_EN_3_1_WIDTH	3

#define PMA_RX_EQ_CTRL4_CONT_ADAPT_0_POS	0
#define PMA_RX_EQ_CTRL4_CONT_ADAPT_0_WIDTH	1
#define PMA_RX_EQ_CTRL4_CONT_ADAPT_3_1_POS	1
#define PMA_RX_EQ_CTRL4_CONT_ADAPT_3_1_WIDTH	3

#define PMA_TX_RATE_CTRL_TX_RATE_0_POS		0
#define PMA_TX_RATE_CTRL_TX_RATE_0_WIDTH	3
#define PMA_TX_RATE_CTRL_TX_RATE_1_POS		4
#define PMA_TX_RATE_CTRL_TX_RATE_1_WIDTH	3
#define PMA_TX_RATE_CTRL_TX_RATE_2_POS		8
#define PMA_TX_RATE_CTRL_TX_RATE_2_WIDTH	3
#define PMA_TX_RATE_CTRL_TX_RATE_3_POS		12
#define PMA_TX_RATE_CTRL_TX_RATE_3_WIDTH	3

#define PMA_RX_RATE_CTRL_RX_RATE_0_POS		0
#define PMA_RX_RATE_CTRL_RX_RATE_0_WIDTH	2
#define PMA_RX_RATE_CTRL_RX_RATE_1_POS		4
#define PMA_RX_RATE_CTRL_RX_RATE_1_WIDTH	2
#define PMA_RX_RATE_CTRL_RX_RATE_2_POS		8
#define PMA_RX_RATE_CTRL_RX_RATE_2_WIDTH	2
#define PMA_RX_RATE_CTRL_RX_RATE_3_POS		12
#define PMA_RX_RATE_CTRL_RX_RATE_3_WIDTH	2

#define PMA_RX_GENCTRL1_RX_DIV16P5_CLK_EN_POS   13
#define PMA_RX_GENCTRL1_RX_DIV16P5_CLK_EN_WIDTH 3

#define PMA_TX_GENCTRL1_VBOOST_EN_0_POS		4
#define PMA_TX_GENCTRL1_VBOOST_EN_0_WIDTH	1
#define PMA_TX_GENCTRL1_VBOOST_LVL_POS		8
#define PMA_TX_GENCTRL1_VBOOST_LVL_WIDTH	3

#define PMA_TX_GENCTRL2_TX_WIDTH_0_POS		8
#define PMA_TX_GENCTRL2_TX_WIDTH_0_WIDTH	2
#define PMA_TX_GENCTRL2_TX_WIDTH_1_POS		10
#define PMA_TX_GENCTRL2_TX_WIDTH_1_WIDTH	2
#define PMA_TX_GENCTRL2_TX_WIDTH_2_POS		12
#define PMA_TX_GENCTRL2_TX_WIDTH_2_WIDTH	2
#define PMA_TX_GENCTRL2_TX_WIDTH_3_POS		14
#define PMA_TX_GENCTRL2_TX_WIDTH_3_WIDTH	2

#define PMA_RX_GENCTRL2_RX_WIDTH_0_POS		8
#define PMA_RX_GENCTRL2_RX_WIDTH_0_WIDTH	2
#define PMA_RX_GENCTRL2_RX_WIDTH_1_POS		10
#define PMA_RX_GENCTRL2_RX_WIDTH_1_WIDTH	2
#define PMA_RX_GENCTRL2_RX_WIDTH_2_POS		12
#define PMA_RX_GENCTRL2_RX_WIDTH_2_WIDTH	2
#define PMA_RX_GENCTRL2_RX_WIDTH_3_POS		14
#define PMA_RX_GENCTRL2_RX_WIDTH_3_WIDTH	2

#define PMA_MPLLA_C2_MPLLA_DIV16P5_CLK_EN_POS	10
#define PMA_MPLLA_C2_MPLLA_DIV16P5_CLK_EN_WIDTH	1
#define PMA_MPLLA_C2_MPLLA_DIV10_CLK_EN_POS	9
#define PMA_MPLLA_C2_MPLLA_DIV10_CLK_EN_WIDTH	1
#define PMA_MPLLA_C2_MPLLA_DIV8_CLK_EN_POS	8
#define PMA_MPLLA_C2_MPLLA_DIV8_CLK_EN_WIDTH	1
#define PMA_MPLLA_C2_MPLLA_DIV_CLK_EN_POS	7
#define PMA_MPLLA_C2_MPLLA_DIV_CLK_EN_WIDTH	1

#define PMA_MPLLB_C2_MPLLB_DIV_CLK_EN_POS	7
#define PMA_MPLLB_C2_MPLLB_DIV_CLK_EN_WIDTH	1
#define PMA_MPLLB_C2_MPLLB_DIV10_CLK_EN_POS	9
#define PMA_MPLLB_C2_MPLLB_DIV10_CLK_EN_WIDTH	1
#define PMA_MPLLB_C2_MPLLB_DIV8_CLK_EN_POS	8
#define PMA_MPLLB_C2_MPLLB_DIV8_CLK_EN_WIDTH	1

#define PMA_TX_EQ_C0_TX_EQ_MAIN_POS		8
#define PMA_TX_EQ_C0_TX_EQ_MAIN_WIDTH		6
#define PMA_TX_EQ_C0_TX_EQ_PRE_POS		0
#define PMA_TX_EQ_C0_TX_EQ_PRE_WIDTH		6

#define PMA_TX_EQ_C1_TX_EQ_POST_POS		0
#define PMA_TX_EQ_C1_TX_EQ_POST_WIDTH		6
#define PMA_TX_EQ_C1_TX_EQ_OVR_RIDE_POS		6
#define PMA_TX_EQ_C1_TX_EQ_OVR_RIDE_WIDTH	1

#define PMA_MISC_C0_RX_VREF_CTRL_POS		8
#define PMA_MISC_C0_RX_VREF_CTRL_WIDTH		5
#define PMA_MISC_C0_CR_PARA_SEL_POS		14
#define PMA_MISC_C0_CR_PARA_SEL_WIDTH		1

#define PMA_REF_CLK_CTRL_REF_USE_PAD_POS	1
#define PMA_REF_CLK_CTRL_REF_USE_PAD_WIDTH	1

#define PMA_CTRL2_PMA_TYPE_POS			0
#define PMA_CTRL2_PMA_TYPE_WIDTH		4

#define PMA_TX_BOOST_CTRL_TX0_IBOOST_POS	0
#define PMA_TX_BOOST_CTRL_TX0_IBOOST_WIDTH	3
#define PMA_TX_BOOST_CTRL_TX1_IBOOST_POS	4
#define PMA_TX_BOOST_CTRL_TX1_IBOOST_WIDTH	3
#define PMA_TX_BOOST_CTRL_TX2_IBOOST_POS	8
#define PMA_TX_BOOST_CTRL_TX2_IBOOST_WIDTH	3
#define PMA_TX_BOOST_CTRL_TX3_IBOOST_POS	12
#define PMA_TX_BOOST_CTRL_TX3_IBOOST_WIDTH	3

#define PMA_TX_STS_TX_ACK_0_POS			0
#define PMA_TX_STS_TX_ACK_0_WIDTH		1
#define PMA_TX_STS_DETRX_RSLT_0_POS		4
#define PMA_TX_STS_DETRX_RSLT_0_WIDTH		1

#define PMA_TX_PS_CTRL_TX0_PSTATE_POS		0
#define PMA_TX_PS_CTRL_TX0_PSTATE_WIDTH		3

#define PMA_STS1_RLU_POS			2
#define PMA_STS1_RLU_WIDTH			1

#define PMA_STS2_RF_POS				10
#define PMA_STS2_RF_WIDTH			1
#define PMA_STS2_TF_POS				11
#define PMA_STS2_TF_WIDTH			1

#define PMA_KR_PMD_STS_RCV_STS_POS		0
#define PMA_KR_PMD_STS_RCV_STS_WIDTH		1
#define PMA_KR_PMD_STS_FRM_LCK_POS		1
#define PMA_KR_PMD_STS_FRM_LCK_WIDTH		1
#define PMA_KR_PMD_STS_SU_PR_DTD_POS		2
#define PMA_KR_PMD_STS_SU_PR_DTD_WIDTH		1
#define PMA_KR_PMD_STS_TR_FAIL_POS		3
#define PMA_KR_PMD_STS_TR_FAIL_WIDTH		1

#define PMA_KR_LP_CEU_LP_INIT_POS		12
#define PMA_KR_LP_CEU_LP_INIT_WIDTH		1
#define PMA_KR_LP_CEU_LP_PRST_POS		13
#define PMA_KR_LP_CEU_LP_PRST_WIDTH		1

#define PMA_KR_LP_CESTS_LP_RR_POS		15
#define PMA_KR_LP_CESTS_LP_RR_WIDTH		1

#define PMA_KR_LD_CEU_LD_INIT_POS		12
#define PMA_KR_LD_CEU_LD_INIT_WIDTH		1
#define PMA_KR_LD_CEU_LD_PRST_POS		13
#define PMA_KR_LD_CEU_LD_PRST_WIDTH		1
#define PMA_KR_LD_CESTS_LD_RR_POS		15
#define PMA_KR_LD_CESTS_LD_RR_WIDTH		1

#define PCS_STS1_RLU_POS			2
#define PCS_STS1_RLU_WIDTH			1
#define PCS_STS2_RF_POS				10
#define PCS_STS2_RF_WIDTH			1
#define PCS_STS2_TF_POS				11
#define PCS_STS2_TF_WIDTH			1
#define PCS_STS2_CAP_EN_POS			0
#define PCS_STS2_CAP_EN_WIDTH			1
#define PCS_STS2_CAP_10_1GC_POS			1
#define PCS_STS2_CAP_10_1GC_WIDTH		1
#define PCS_STS2_CAP_10GBW_POS			2
#define PCS_STS2_CAP_10GBW_WIDTH		1
#define PCS_STS2_CAP_10GBT_POS			3
#define PCS_STS2_CAP_10GBT_WIDTH		1
#define PCS_STS2_DS_POS				14
#define PCS_STS2_DS_WIDTH			2

#define PCS_DIG_CTRL1_VR_RST_POS		15
#define PCS_DIG_CTRL1_VR_RST_WIDTH		1
#define PCS_DIG_CTRL1_CL37_BP_POS		12
#define PCS_DIG_CTRL1_CL37_BP_WIDTH		1
#define PCS_DIG_CTRL1_EN_2_5G_MODE_POS		2
#define PCS_DIG_CTRL1_EN_2_5G_MODE_WIDTH	1
#define PCS_DIG_CTRL1_BYP_PWRUP_POS		1
#define PCS_DIG_CTRL1_BYP_PWRUP_WIDTH		1

#define PCS_CTRL1_RST_POS			15
#define PCS_CTRL1_RST_WIDTH			1

#define PCS_XAUI_CTRL_XAUI_MODE_POS		0
#define PCS_XAUI_CTRL_XAUI_MODE_WIDTH		1

#define PCS_CTRL1_SS13_POS			13
#define PCS_CTRL1_SS13_WIDTH			1

#define PCS_CTRL2_PCS_TYPE_SEL_POS		0
#define PCS_CTRL2_PCS_TYPE_SEL_WIDTH		2

#define PCS_DIG_STS_PSEQ_STATE_POS		2
#define PCS_DIG_STS_PSEQ_STATE_WIDTH		3

#define MAX_XPCS 				6

enum {
	BACKPL_ETH_PCS = 0,
	OTHER_PCS_CONFIG = 1
};

enum {
	TENG_KR_MODE = 0,
	TENG_XAUI_MODE,
	ONEG_XAUI_MODE,
	TWOP5G_SGMII_MODE,
	MAX_XPCS_MODE,
};

enum {
	LANE_1 = 1,
	LANE_2,
	LANE_3,
	LANE_4,
	LANE_MAX,
};

enum {
	PHY_SIDE_SGMII = 0,
	MAC_SIDE_SGMII = 1
};

enum {
	CONN_TYPE_SFP = 0,
	CONN_TYPE_PHY = 1,
	CONN_TYPE_MAX,
};

enum {
	PMA_TYPE_10G_BASE_CX4 = 0,
	PMA_TYPE_10G_BASE_EW,
	PMA_TYPE_10G_BASE_LW,
	PMA_TYPE_10G_BASE_SW,
	PMA_TYPE_10G_BASE_LX4,
	PMA_TYPE_10G_BASE_ER,
	PMA_TYPE_10G_BASE_LR,
	PMA_TYPE_10G_BASE_SR,
	PMA_TYPE_10G_BASE_LRM,
	PMA_TYPE_10G_BASE_T,
	PMA_TYPE_10G_BASE_KX4,
	PMA_TYPE_10G_BASE_KR,
	PMA_TYPE_1G_BASE_T,
	PMA_TYPE_1G_BASE_KX,
	PMA_TYPE_100_BASE_TX,
	PMA_TYPE_10_BASE_T
};

enum {
	PCS_TYPE_10G_BASE_R = 0,
	PCS_TYPE_10G_BASE_X,
	PCS_TYPE_10G_BASE_W,
	PCS_TYPE_RESERVED,
};

struct xpcs_prv_data;

/* 10G XAUI Mode Switching */
struct xpcs_mode_cfg {
	u32 id;
	u32 pma_type;
	u32 pcs_type;
	u32 xaui_mode;
	u32 lane;
	u32 mplla_mult;
	u32 mplla_bw;
	u32 mpllb_mult;
	u32 mpllb_bw;
	u32 vco_ld_val[4];
	u32 vco_ref_ld[4];
	u32 afe_en;
	u32 afe_en_31;
	u32 dfe_en;
	u32 dfe_en_31;
	u32 cont_adapt0;
	u32 cont_adapt31;
	u32 tx_rate[4];
	u32 rx_rate[4];
	u32 tx_width[4];
	u32 rx_width[4];
	u32 mplla_div165_clk_en;
	u32 mplla_div10_clk_en;
	u32 mplla_div8_clk_en;
	u32 mplla_div_clk_en;
	u32 mpllb_div_clk_en;
	u32 mpllb_div10_clk_en;
	u32 mpllb_div8_clk_en;
	u32 rx_div165_clk_en;
	u32 tx_eq_main;
	u32 tx_eq_pre;
	u32 tx_eq_post;
	u32 tx_eq_ovrride;
	u32 los_thr;
	u32 phy_boost_gain_val;
	u32 rx_vref_ctrl;
	u32 ref_clk_ctrl;
	u32 vboost_lvl;
	u32 tx_iboost[4];
	int (*set_mode)(struct xpcs_prv_data *);
};

struct xpcs_prv_data {
	u32 id;
	u32 num_resources;

	/* XPCS registers for indirect accessing */
	void __iomem *addr_base;

	struct device *dev;
	struct phy *phy;

	/* XPCS Interface Name */
	const char *name;

	/* XPCS Mode 10G, 1G, 2.5G */
	u32 mode;

	/* IRQ number */
	u32 irq_num;

	/* PCS Mode */
	u32 pcs_mode;

	u32 sgmii_type;
	u32 duplex;

	u32 mpllb;

	/* Connection Type SFP or PHY */
	u32 conntype;

	/* Power save mode 0 - Normal Mode, 1 - power save mode */
	u8 power_save;

	struct reset_control *xpcs_rst;

	/* mac_idx where xpcs is connected */
	u32 mac_idx;

	/* XPCS Mode Switching */
	struct xpcs_mode_cfg *mode_cfg;
};

#define xpcs_r32(reg)		readl(reg)
#define xpcs_w32(val, reg)	writel(val, reg)

static inline u32 XPCS_RGRD(struct xpcs_prv_data *pdata, u32 reg)
{
	u32 reg_val;
	u32 shift_addr;

	void __iomem *addr_reg  =
		(void __iomem *)((u32)pdata->addr_base + (0xFF << 2));
	void __iomem *data_reg  =
		(void __iomem *)((u32)pdata->addr_base | (reg & 0x000003FF));

	shift_addr = (u32)(((reg >> 2) & 0x001FFF00) >> 8);

	xpcs_w32(shift_addr, addr_reg);
	reg_val = xpcs_r32(data_reg);

	/* Dummy register read */
	xpcs_w32(shift_addr, addr_reg);
	reg_val = xpcs_r32(data_reg);

	return reg_val;
}

static inline void XPCS_RGWR(struct xpcs_prv_data *pdata, u32 reg, u32 val)
{
	u32 shift_addr;
	u32 reg_val;
	void __iomem *addr_reg  =
		(void __iomem *)((u32)pdata->addr_base + (0xFF << 2));
	void __iomem *data_reg  =
		(void __iomem *)((u32)pdata->addr_base | (reg & 0x000003FF));

	shift_addr = (u32)(((reg >> 2) & 0x001FFF00) >> 8);

	xpcs_w32(shift_addr, addr_reg);
	xpcs_w32(val, data_reg);

	/* Dummy register read */
	xpcs_w32(shift_addr, addr_reg);
	reg_val = xpcs_r32(data_reg);
}

#define GET_N_BITS(reg, pos, n) \
	(((reg) >> (pos)) & ((0x1 << (n)) - 1))

#define SET_N_BITS(reg, pos, n, val)					\
	do {								\
		(reg) &= ~(((0x1 << (n)) - 1) << (pos));              \
		(reg) |= (((val) & ((0x1 << (n)) - 1)) << (pos));    \
	} while (0)

#define XPCS_GET_VAL(var, reg, field)				\
	GET_N_BITS((var),					\
		   reg##_##field##_POS,				\
		   reg##_##field##_WIDTH)

#define XPCS_SET_VAL(var, reg, field, val)			\
	SET_N_BITS((var),					\
		   reg##_##field##_POS,				\
		   reg##_##field##_WIDTH, (val))

#define XPCS_RGRD_VAL(pdata, reg, field)			\
	GET_N_BITS(XPCS_RGRD(pdata, reg),			\
		   reg##_##field##_POS,				\
		   reg##_##field##_WIDTH)

#define XPCS_RGWR_VAL(pdata, reg, field, _val)			\
	do {							\
		u32 reg_val = XPCS_RGRD(pdata, reg);		\
		SET_N_BITS(reg_val,				\
			   reg##_##field##_POS,			\
			   reg##_##field##_WIDTH, (_val));	\
		XPCS_RGWR(pdata, reg, reg_val);			\
	} while (0)

int xpcs_sysfs_init(struct xpcs_prv_data *priv);
int xpcs_ethtool_ksettings_get(struct device *dev,
				struct ethtool_link_ksettings *cmd);
int xpcs_ethtool_ksettings_set(struct device *dev,
			       const struct ethtool_link_ksettings *cmd);
int xpcs_reinit(struct device *dev, u32 mode);

#endif

