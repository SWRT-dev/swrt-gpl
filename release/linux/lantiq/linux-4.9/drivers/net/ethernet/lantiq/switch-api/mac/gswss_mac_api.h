/******************************************************************************
 *                Copyright (c) 2016, 2017 Intel Corporation
 *
 *
 * For licensing information, see the file 'LICENSE' in the root folder of
 * this software module.
 *
 ******************************************************************************/

#ifndef _GSWIPSS_MAC_API
#define _GSWIPSS_MAC_API

#include <xgmac_common.h>
#include <gswss_api.h>

#define MAC_IF_CFG(idx)			(0x1200 + ((idx - MAC_2) * (0x100)))
#define MAC_OP_CFG(idx)			(0x1204 + ((idx - MAC_2) * (0x100)))
#define MAC_MTU_CFG(idx)		(0x1208 + ((idx - MAC_2) * (0x100)))
#define MAC_GINT_CFG(idx)		(0x120C + ((idx - MAC_2) * (0x100)))
#define MAC_GINT_HD0_CFG(idx)		(0x1220 + ((idx - MAC_2) * (0x100)))
#define MAC_GINT_HD1_CFG(idx)		(0x1224 + ((idx - MAC_2) * (0x100)))
#define MAC_GINT_HD2_CFG(idx)		(0x1228 + ((idx - MAC_2) * (0x100)))
#define MAC_GINT_HD3_CFG(idx)		(0x122C + ((idx - MAC_2) * (0x100)))
#define MAC_GINT_HD4_CFG(idx)		(0x1230 + ((idx - MAC_2) * (0x100)))
#define MAC_GINT_HD5_CFG(idx)		(0x1234 + ((idx - MAC_2) * (0x100)))
#define MAC_GINT_HD6_CFG(idx)		(0x1238 + ((idx - MAC_2) * (0x100)))
#define LMAC_CNT_LSB(idx)		(0x1240 + ((idx - MAC_2) * (0x100)))
#define LMAC_CNT_MSB(idx)		(0x1244 + ((idx - MAC_2) * (0x100)))
#define LMAC_CNT_ACC(idx)		(0x1248 + ((idx - MAC_2) * (0x100)))
#define MAC_TXTS_0(idx)			(0x1250 + ((idx - MAC_2) * (0x100)))
#define MAC_TXTS_1(idx)			(0x1254 + ((idx - MAC_2) * (0x100)))
#define MAC_TXTS_2(idx)			(0x1258 + ((idx - MAC_2) * (0x100)))
#define MAC_TXTS_3(idx)			(0x125C + ((idx - MAC_2) * (0x100)))
#define MAC_TXTS_CIC(idx)		(0x1260 + ((idx - MAC_2) * (0x100)))
#define MAC_TXTS_ACC(idx)		(0x1264 + ((idx - MAC_2) * (0x100)))
#define PHY_MODE(idx)			(0x1270 + ((idx - MAC_2) * (0x100)))
#define PHY_STAT(idx)			(0x1274 + ((idx - MAC_2) * (0x100)))
#define ANEG_EEE(idx)			(0x1278 + ((idx - MAC_2) * (0x100)))
#define XGMAC_CTRL(idx)			(0x1280 + ((idx - MAC_2) * (0x100)))

#define MAC_IF_CFG_ADAP_RES_POS		15
#define MAC_IF_CFG_ADAP_RES_WIDTH	1
#define MAC_IF_CFG_LMAC_RES_POS		14
#define MAC_IF_CFG_LMAC_RES_WIDTH	1
#define MAC_IF_CFG_XGMAC_RES_POS	13
#define MAC_IF_CFG_XGMAC_RES_WIDTH	1
#define MAC_IF_CFG_MAC_EN_POS		12
#define MAC_IF_CFG_MAC_EN_WIDTH		1
#define MAC_IF_CFG_PTP_DIS_POS		11
#define MAC_IF_CFG_PTP_DIS_WIDTH	1
#define MAC_IF_CFG_CFG1G_POS		1
#define MAC_IF_CFG_CFG1G_WIDTH		1
#define MAC_IF_CFG_CFGFE_POS		2
#define MAC_IF_CFG_CFGFE_WIDTH		1
#define MAC_IF_CFG_CFG2G5_POS		0
#define MAC_IF_CFG_CFG2G5_WIDTH		1

#define MAC_OP_CFG_RXSPTAG_POS		0
#define MAC_OP_CFG_RXSPTAG_WIDTH	2
#define MAC_OP_CFG_RXTIME_POS		2
#define MAC_OP_CFG_RXTIME_WIDTH		2
#define MAC_OP_CFG_RXFCS_POS		4
#define MAC_OP_CFG_RXFCS_WIDTH		2
#define MAC_OP_CFG_TXSPTAG_POS		6
#define MAC_OP_CFG_TXSPTAG_WIDTH	2
#define MAC_OP_CFG_TXFCS_RM_POS		8
#define MAC_OP_CFG_TXFCS_RM_WIDTH	1
#define MAC_OP_CFG_TXFCS_INS_POS	9
#define MAC_OP_CFG_TXFCS_INS_WIDTH	1
#define MAC_OP_CFG_NUM_STREAM_POS	10
#define MAC_OP_CFG_NUM_STREAM_WIDTH	1

#define MAC_MTU_CFG_POS			0
#define MAC_MTU_CFG_WIDTH		14

#define MAC_TXTS_CIC_CIC_POS		0
#define MAC_TXTS_CIC_CIC_WIDTH		2
#define MAC_TXTS_CIC_OSTAVAIL_POS	2
#define MAC_TXTS_CIC_OSTAVAIL_WIDTH	1
#define MAC_TXTS_CIC_OSTC_POS		3
#define MAC_TXTS_CIC_OSTC_WIDTH		1
#define MAC_TXTS_CIC_TTSE_POS		4
#define MAC_TXTS_CIC_TTSE_WIDTH		1

#define MAC_TXTS_ACC_ADDR_POS		0
#define MAC_TXTS_ACC_ADDR_WIDTH		6
#define MAC_TXTS_ACC_OPMOD_POS		14
#define MAC_TXTS_ACC_OPMOD_WIDTH	1
#define MAC_TXTS_ACC_BAS_POS		15
#define MAC_TXTS_ACC_BAS_WIDTH		1

#define PHY_MODE_FCONRX_POS		5
#define PHY_MODE_FCONRX_WIDTH		2
#define PHY_MODE_FCONTX_POS		7
#define PHY_MODE_FCONTX_WIDTH		2
#define PHY_MODE_FDUP_POS		9
#define PHY_MODE_FDUP_WIDTH		2
#define PHY_MODE_SPEEDLSB_POS		11
#define PHY_MODE_SPEEDLSB_WIDTH		2
#define PHY_MODE_LINKST_POS		13
#define PHY_MODE_LINKST_WIDTH		2
#define PHY_MODE_SPEEDMSB_POS		15
#define PHY_MODE_SPEEDMSB_WIDTH		1

#define PHY_STAT_TXPAUEN_POS		0
#define PHY_STAT_TXPAUEN_WIDTH		1
#define PHY_STAT_RXPAUEN_POS		1
#define PHY_STAT_RXPAUEN_WIDTH		1
#define PHY_STAT_FDUP_POS		2
#define PHY_STAT_FDUP_WIDTH		1
#define PHY_STAT_SPEEDLSB_POS		3
#define PHY_STAT_SPEEDLSB_WIDTH		2
#define PHY_STAT_LSTAT_POS		5
#define PHY_STAT_LSTAT_WIDTH		1
#define PHY_STAT_EEECAP_POS		7
#define PHY_STAT_EEECAP_WIDTH		1
#define PHY_STAT_CLKSTOPCAP_POS		8
#define PHY_STAT_CLKSTOPCAP_WIDTH	1
#define PHY_STAT_SPEEDMSB_POS		11
#define PHY_STAT_SPEEDMSB_WIDTH		1

#define XGMAC_CTRL_CPC_POS		0
#define XGMAC_CTRL_CPC_WIDTH		2
#define XGMAC_CTRL_DISRX_POS		2
#define XGMAC_CTRL_DISRX_WIDTH		1
#define XGMAC_CTRL_DISTX_POS		3
#define XGMAC_CTRL_DISTX_WIDTH		1

#define ANEG_EEE_EEE_CAPABLE_POS	0
#define ANEG_EEE_EEE_CAPABLE_WIDTH	2

static inline u32 GSWSS_MAC_RGRD(struct mac_prv_data *pdata, u32 reg)
{
	u32 reg_val;
#if defined(PC_UTILITY) || defined(CHIPTEST)
	u32 reg_addr = pdata->ss_addr_base + reg;
#else
	void __iomem *reg_addr = (void __iomem *)pdata->ss_addr_base + reg;
#endif

#if defined(CHIPTEST) && CHIPTEST
	reg_val = REG32(reg_addr);
#endif
#if defined(PC_UTILITY) && PC_UTILITY
	pcuart_reg_rd(reg_addr, &reg_val);
#endif
#ifdef __KERNEL__
	reg_val = mac_r32(reg_addr);
#endif
	return reg_val;
}

static inline void GSWSS_MAC_RGWR(struct mac_prv_data *pdata, u32 reg,
				  u32 val)
{
#if defined(PC_UTILITY) || defined(CHIPTEST)
	u32 reg_addr = pdata->ss_addr_base + reg;
#else
	void __iomem *reg_addr = (void __iomem *)pdata->ss_addr_base + reg;
#endif

#if defined(CHIPTEST) && CHIPTEST
	REG32(reg_addr) = val;
#endif
#if defined(PC_UTILITY) && PC_UTILITY
	pcuart_reg_wr(reg_addr, val);
#endif
#ifdef __KERNEL__
	mac_w32(val, reg_addr);
#endif
}

enum {
	LMAC_MII = 0,
	LMAC_GMII,
	XGMAC_GMII,
	XGMAC_XGMII,
};

enum {
	CFG_1G = 0,
	CFG_FE,
	CFG_2G5
};

enum {
	FCS = 0,
	SPTAG,
	TIME
};

enum {
	RX = 0,
	TX,
};

enum {
	EEE_CAP_AUTO = 0,
	EEE_CAP_ON,
	EEE_CAP_RES,
	EEE_CAP_OFF
};

int gswss_mac_enable(void *pdev, u32 enable);
int gswss_set_1g_intf(void *pdev, u32 macif);
int gswss_set_fe_intf(void *pdev, u32 macif);
int gswss_set_2G5_intf(void *pdev, u32 macif);
int gswss_set_mac_txfcs_ins_op(void *pdev, u32 val);
int gswss_set_mac_txfcs_rm_op(void *pdev, u32 val);
int gswss_set_mac_rxfcs_op(void *pdev, u32 val);
int gswss_set_mac_txsptag_op(void *pdev, u32 val);
int gswss_set_mac_rxsptag_op(void *pdev, u32 val);
int gswss_set_mac_rxtime_op(void *pdev, u32 val);
int gswss_set_mtu(void *pdev, u32 mtu);
int gswss_set_txtstamp_access(void *pdev, u32 op_mode, u32 addr);
int gswss_set_duplex_mode(void *pdev, u32 val);
int gswss_set_speed(void *pdev, u8 speed);
int gswss_set_linkstatus(void *pdev, u8 linkst);
int gswss_set_flowctrl_tx(void *pdev, u8 flow_ctrl_tx);
int gswss_set_flowctrl_rx(void *pdev, u8 flow_ctrl_rx);
int gswss_xgmac_reset(void *pdev, u32 reset);
int gswss_lmac_reset(void *pdev, u32 reset);
int gswss_adap_reset(void *pdev, u32 reset);
int gswss_set_xgmac_tx_disable(void *pdev, u32 val);
int gswss_set_xgmac_rx_disable(void *pdev, u32 val);
int gswss_set_xgmac_crc_ctrl(void *pdev, u32 val);
int gswss_get_xgmac_crc_ctrl(void *pdev);

int gswss_get_mtu(void *pdev);
u32 gswss_get_flowctrl_tx(void *pdev);
u32 gswss_get_flowctrl_rx(void *pdev);
u8 gswss_get_speed(void *pdev);
u32 gswss_get_2G5_intf(void *pdev);
u32 gswss_get_1g_intf(void *pdev);
u32 gswss_get_fe_intf(void *pdev);
int gswss_get_duplex_mode(void *pdev);
int gswss_get_linkstatus(void *pdev);
void gswss_get_xgmac_ctrl(void *pdev);
int gswss_get_phy2mode(void *pdev);
int gswss_get_macop(void *pdev);
int gswss_get_macif(void *pdev);
int gswss_get_mac_en(void *pdev);
int gswss_get_mac_reset(void *pdev);
int gswss_set_txtstamp_fifo(void *pdev,
			    struct mac_fifo_entry *f_entry);
int gswss_get_txtstamp_fifo(void *pdev, u32 record_id, struct mac_fifo_entry *f_entry);
int gswss_set_eee_cap(void *pdev, u32 val);
int gswss_cfg_main(GSW_MAC_Cli_t *params);
int gswss_get_mac_txfcs_ins_op(void *pdev);
int gswss_get_mac_txfcs_rm_op(void *pdev);
int gswss_get_mac_rxfcs_op(void *pdev);
int gswss_get_mac_rxsptag_op(void *pdev);
int gswss_get_mac_txsptag_op(void *pdev);
int gswss_get_mac_rxtime_op(void *pdev);

#endif
