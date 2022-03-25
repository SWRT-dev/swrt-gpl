/******************************************************************************
 *                Copyright (c) 2016, 2017 Intel Corporation
 *
 *
 * For licensing information, see the file 'LICENSE' in the root folder of
 * this software module.
 *
 ******************************************************************************/

#ifndef _LMAC_
#define _LMAC_

#include <xgmac_common.h>
#if defined(PC_UTILITY) && PC_UTILITY
#define LEGACY_MAC_BASE			0x42000
#else
#define LEGACY_MAC_BASE			0xb8420000
#endif
#define MAC_TEST			0x300
#define MAC_PFADCFG			0x304
#define MAC_PFSA_0			0x308
#define MAC_PFSA_1			0x30C
#define MAC_PFSA_2			0x310
#define MAC_VLAN_ETYPE_0		0x318
#define MAC_VLAN_ETYPE_1		0x31C
#define LMAC_IER			0x320
#define LMAC_ISR			0x324
#define REG_LMAC_CNT_LSB		0x328
#define REG_LMAC_CNT_MSB		0x32C
#define REG_LMAC_CNT_ACC		0x330
#define MAC_PSTAT(idx)			(0x400 + ((idx - MAC_2) * (0x30)))
#define MAC_PISR(idx)			(0x404 + ((idx - MAC_2) * (0x30)))
#define MAC_PIER(idx)			(0x408 + ((idx - MAC_2) * (0x30)))
#define MAC_CTRL0(idx)			(0x40C + ((idx - MAC_2) * (0x30)))
#define MAC_CTRL1(idx)			(0x410 + ((idx - MAC_2) * (0x30)))
#define MAC_CTRL2(idx)			(0x414 + ((idx - MAC_2) * (0x30)))
#define MAC_CTRL3(idx)			(0x418 + ((idx - MAC_2) * (0x30)))
#define MAC_CTRL4(idx)			(0x41C + ((idx - MAC_2) * (0x30)))
#define MAC_CTRL5(idx)			(0x420 + ((idx - MAC_2) * (0x30)))
#define MAC_TESTEN(idx)			(0x42C + ((idx - MAC_2) * (0x30)))
#define MAC_LPITIMER0(idx)		(0x424 + ((idx - MAC_2) * (0x30)))
#define MAC_LPITIMER1(idx)		(0x428 + ((idx - MAC_2) * (0x30)))

#define MAC_CTRL0_GMII_POS      0
#define MAC_CTRL0_GMII_WIDTH      2
#define MAC_CTRL0_FDUP_POS      2
#define MAC_CTRL0_FDUP_WIDTH      2
#define MAC_CTRL0_FCON_POS      4
#define MAC_CTRL0_FCON_WIDTH      2
#define MAC_CTRL0_FCS_POS       7
#define MAC_CTRL0_FCS_WIDTH       1
#define MAC_CTRL0_PADEN_POS     8
#define MAC_CTRL0_PADEN_WIDTH     1
#define MAC_CTRL0_VPADEN_POS    9
#define MAC_CTRL0_VPADEN_WIDTH    1
#define MAC_CTRL0_VPAD2EN_POS   10
#define MAC_CTRL0_VPAD2EN_WIDTH   1
#define MAC_CTRL0_APADEN_POS    11
#define MAC_CTRL0_APADEN_WIDTH    1

#define MAC_CTRL1_IPG_POS		0
#define MAC_CTRL1_IPG_WIDTH		4
#define MAC_CTRL1_SHORTPRE_POS		8
#define MAC_CTRL1_SHORTPRE_WIDTH	1
#define MAC_CTRL1_DEFERMODE_POS		15
#define MAC_CTRL1_DEFERMODE_WIDTH	1

#define MAC_CTRL2_LCHKS_POS		0
#define MAC_CTRL2_LCHKS_WIDTH		2
#define MAC_CTRL2_LCHKL_POS		2
#define MAC_CTRL2_LCHKL_WIDTH		1
#define MAC_CTRL2_MLEN_POS		3
#define MAC_CTRL2_MLEN_WIDTH		1

#define MAC_CTRL3_RCNT_POS		0
#define MAC_CTRL3_RCNT_WIDTH		4

#define MAC_CTRL4_WAIT_POS       0
#define MAC_CTRL4_WAIT_WIDTH       7
#define MAC_CTRL4_LPIEN_POS      7
#define MAC_CTRL4_LPIEN_WIDTH      1
#define MAC_CTRL4_GWAIT_POS      8
#define MAC_CTRL4_GWAIT_WIDTH      7

#define MAC_CTRL5_PJPS_BP_POS       0
#define MAC_CTRL5_PJPS_BP_WIDTH       1
#define MAC_CTRL5_PJPS_NOBP_POS     1
#define MAC_CTRL5_PJPS_NOBP_WIDTH     1

#define MAC_TESTEN_LOOP_POS       0
#define MAC_TESTEN_LOOP_WIDTH       1
#define MAC_TESTEN_TXER_POS       1
#define MAC_TESTEN_TXER_WIDTH       1
#define MAC_TESTEN_JTEN_POS       2
#define MAC_TESTEN_JTEN_WIDTH       1
#define MAC_TESTEN_LPITM_POS      8
#define MAC_TESTEN_LPITM_WIDTH      2

#define MAC_LPITMER0_TMLSB_POS     0
#define MAC_LPITMER0_TMLSB__WIDTH    16
#define MAC_LPITMER1_TMLSB_POS     0
#define MAC_LPITMER1_TMLSB__WIDTH    16

#define MAC_PIER_RXPAUSE_POS      0
#define MAC_PIER_RXPAUSE_WIDTH      1
#define MAC_PIER_TXPAUSE_POS      1
#define MAC_PIER_TXPAUSE_WIDTH      1
#define MAC_PIER_FCSERR_POS       2
#define MAC_PIER_FCSERR_WIDTH       1
#define MAC_PIER_LENERR_POS       3
#define MAC_PIER_LENERR_WIDTH       1
#define MAC_PIER_TOOLONG_POS      4
#define MAC_PIER_TOOLONG_WIDTH      1
#define MAC_PIER_TOOSHORT_POS     5
#define MAC_PIER_TOOSHORT_WIDTH     1
#define MAC_PIER_JAM_POS          6
#define MAC_PIER_JAM_WIDTH          1
#define MAC_PIER_LPION_POS        7
#define MAC_PIER_LPION_WIDTH        1
#define MAC_PIER_LPIOFF_POS       8
#define MAC_PIER_LPIOFF_WIDTH       1
#define MAC_PIER_TXPAUEN_POS      9
#define MAC_PIER_TXPAUEN_WIDTH      1
#define MAC_PIER_RXPAUEN_POS      10
#define MAC_PIER_RXPAUEN_WIDTH      1
#define MAC_PIER_FDUP_POS         11
#define MAC_PIER_FDUP_WIDTH         1
#define MAC_PIER_SPEED_POS        12
#define MAC_PIER_SPEED_WIDTH        1
#define MAC_PIER_PACT_POS         13
#define MAC_PIER_PACT_WIDTH         1
#define MAC_PIER_ALIGN_POS        14
#define MAC_PIER_ALIGN_WIDTH        1
#define MAC_PIER_PHYERR_POS       15
#define MAC_PIER_PHYERR_WIDTH       1

#define MAC_PISR_RXPAUSE_POS      0
#define MAC_PISR_RXPAUSE_WIDTH      1
#define MAC_PISR_TXPAUSE_POS      1
#define MAC_PISR_TXPAUSE_WIDTH      1
#define MAC_PISR_FCSERR_POS       2
#define MAC_PISR_FCSERR_WIDTH       1
#define MAC_PISR_LENERR_POS       3
#define MAC_PISR_LENERR_WIDTH       1
#define MAC_PISR_TOOLONG_POS      4
#define MAC_PISR_TOOLONG_WIDTH      1
#define MAC_PISR_TOOSHORT_POS     5
#define MAC_PISR_TOOSHORT_WIDTH     1
#define MAC_PISR_JAM_POS          6
#define MAC_PISR_JAM_WIDTH          1
#define MAC_PISR_LPION_POS        7
#define MAC_PISR_LPION_WIDTH        1
#define MAC_PISR_LPIOFF_POS       8
#define MAC_PISR_LPIOFF_WIDTH       1
#define MAC_PISR_TXPAUEN_POS      9
#define MAC_PISR_TXPAUEN_WIDTH      1
#define MAC_PISR_RXPAUEN_POS      10
#define MAC_PISR_RXPAUEN_WIDTH      1
#define MAC_PISR_FDUP_POS         11
#define MAC_PISR_FDUP_WIDTH         1
#define MAC_PISR_SPEED_POS        12
#define MAC_PISR_SPEED_WIDTH        1
#define MAC_PISR_PACT_POS         13
#define MAC_PISR_PACT_WIDTH         1
#define MAC_PISR_ALIGN_POS        14
#define MAC_PISR_ALIGN_WIDTH        1
#define MAC_PISR_PHYERR_POS       15
#define MAC_PISR_PHYERR_WIDTH       1

#define MAC_PSTAT_RXLPI_POS      0
#define MAC_PSTAT_RXLPI_WIDTH      1
#define MAC_PSTAT_TXLPI_POS      1
#define MAC_PSTAT_TXLPI_WIDTH      1
#define MAC_PSTAT_CRS_POS        2
#define MAC_PSTAT_CRS_WIDTH        1
#define MAC_PSTAT_LSTAT_POS      3
#define MAC_PSTAT_LSTAT_WIDTH      1
#define MAC_PSTAT_TXPAUEN_POS    4
#define MAC_PSTAT_TXPAUEN_WIDTH    1
#define MAC_PSTAT_RXPAUEN_POS    5
#define MAC_PSTAT_RXPAUEN_WIDTH    1
#define MAC_PSTAT_TXPAU_POS      6
#define MAC_PSTAT_TXPAU_WIDTH      1
#define MAC_PSTAT_RXPAU_POS      7
#define MAC_PSTAT_RXPAU_WIDTH      1
#define MAC_PSTAT_FDUP_POS       8
#define MAC_PSTAT_FDUP_WIDTH       1
#define MAC_PSTAT_MBIT_POS       9
#define MAC_PSTAT_MBIT_WIDTH       1
#define MAC_PSTAT_GBIT_POS       10
#define MAC_PSTAT_GBIT_WIDTH       1
#define MAC_PSTAT_PACT_POS       11
#define MAC_PSTAT_PACT_WIDTH       1

#define MAC_VLAN_ETPE1_STAG_POS  0
#define MAC_VLAN_ETPE1_STAG_WIDTH  16
#define MAC_VLAN_ETPE0_CTAG_POS  0
#define MAC_VLAN_ETPE0_CTAG_WIDTH  16

#define LMAC_IER_MAC2_POS	0
#define LMAC_IER_MAC2_WIDTH	1
#define LMAC_IER_MAC3_POS	1
#define LMAC_IER_MAC3_WIDTH	1
#define LMAC_IER_MAC4_POS	2
#define LMAC_IER_MAC4_WIDTH	1

#define LMAC_ISR_MAC2_POS	0
#define LMAC_ISR_MAC2_WIDTH	1
#define LMAC_ISR_MAC3_POS	1
#define LMAC_ISR_MAC3_WIDTH	1
#define LMAC_ISR_MAC4_POS	2
#define LMAC_ISR_MAC4_WIDTH	1

#define MAC_PFSA2_PFAD_POS  0
#define MAC_PFSA2_PFAD_WIDTH  16
#define MAC_PFSA1_PFAD_POS  0
#define MAC_PFSA1_PFAD_WIDTH  16
#define MAC_PFSA0_PFAD_POS  0
#define MAC_PFSA0_PFAD_WIDTH  16

#define MAC_PFADCFG_SAMOD_POS  0
#define MAC_PFADCFG_SAMOD_WIDTH  1

#define LMAC_CNT_ACC_BAS_POS     15
#define LMAC_CNT_ACC_BAS_WIDTH     1
#define LMAC_CNT_ACC_OPMOD_POS  13
#define LMAC_CNT_ACC_OPMOD_WIDTH  2
#define LMAC_CNT_ACC_MAC_POS     8
#define LMAC_CNT_ACC_MAC_WIDTH     4
#define LMAC_CNT_ACC_ADDR_POS    0
#define LMAC_CNT_ACC_ADDR_WIDTH    5

#define SGLE_COLN_CNT			0
#define MPLE_COLN_CNT			1
#define LATE_COLN_CNT			2
#define EXCS_COLN_CNT			3
#define RXPA_FRAM_CNT			4
#define TXPA_FRAM_CNT			5

enum {
	LMAC_RMON_RD = 0,
	LMAC_RMON_CLR,
	LMAC_RMON_WR,
	LMAC_RMON_CLRALL
};

static inline u32 LMAC_RGRD(struct mac_prv_data *pdata, u32 reg)
{
	u32 reg_val;
#if defined(PC_UTILITY) || defined(CHIPTEST)
	u32 reg_addr = pdata->lmac_addr_base + reg;
#else
	void __iomem *reg_addr = (void __iomem *)pdata->lmac_addr_base + reg;
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

static inline void LMAC_RGWR(struct mac_prv_data *pdata, u32 reg, u32 val)
{
#if defined(PC_UTILITY) || defined(CHIPTEST)
	u32 reg_addr = pdata->lmac_addr_base + reg;
#else
	void __iomem *reg_addr = (void __iomem *)pdata->lmac_addr_base + reg;
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

#define LMAC_RGRD_BITS(reg, field)			\
	GET_N_BITS(LMAC_RGRD(reg),			\
		   reg##_##field##_POS,			\
		   reg##_##field##_WIDTH)

#define LMAC_RGWR_BITS(reg, field, val)			\
	do {						\
		u32 reg_val = LMAC_RGRD(reg);		\
		SET_N_BITS(reg_val,			\
			   reg##_##field##_POS,		\
			   reg##_##field##_WIDTH, (val));\
		LMAC_RGWR(reg, reg_val);		\
	} while (0)

int lmac_set_intf_mode(void *pdev, u32 val) ;
int lmac_set_duplex_mode(void *pdev, u32 val);
int lmac_set_flowcon_mode(void *pdev, u32 val);
int lmac_set_txfcs(void *pdev, u32 val);
int lmac_set_ipg(void *pdev, u32 val);
int lmac_set_preamble(void *pdev, u32 val);
int lmac_set_defermode(void *pdev, u32 val);
int lmac_set_lpi(void *pdev, u32 mode_en, u32 lpi_waitg, u32 lpi_waitm);
int lmac_set_jps(void *pdev, u32 pjps_bp, u32 pjps_nobp);
int lmac_set_loopback(void *pdev, u32 val);
int lmac_set_txer(void *pdev, u32 val);
int lmac_set_lpimonitor_mode(void *pdev, u32 val);
int lmac_set_pauseframe_samode(void *pdev, u32 val);
int lmac_set_pauseframe_addr(void *pdev, u8 *mac_addr);
int lmac_set_int(void *pdev, u32 val);
int lmac_set_event_int(void *pdev, u32 evnt, u32 val);

int lmac_get_event_int(void *pdev, u32 evnt);
int lmac_get_pauseframe_samode(void *pdev);
int lmac_get_pauseframe_addr(void *pdev, u8 *mac_addr);
int lmac_get_int_stat(void *pdev);
int lmac_get_int(void *pdev);
int lmac_clear_int(void *pdev, u32 event);

void lmac_test_all_reg(void *pdev);
void lmac_check_reg(void *pdev, u32 reg, char *name, int idx,
		    u16 set_val, u16 clr_val);

void lmac_rmon_rd(void *pdev, struct lmac_rmon_cnt *lmac_cnt);
void lmac_rmon_wr(void *pdev, struct lmac_rmon_cnt *lmac_cnt);
int lmac_rmon_clr(void *pdev);
void lmac_rmon_clr_allmac(void *pdev);

#endif
