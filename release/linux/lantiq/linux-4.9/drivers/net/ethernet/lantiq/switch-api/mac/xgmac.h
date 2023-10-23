/******************************************************************************
 *                Copyright (c) 2016, 2017 Intel Corporation
 *
 *
 * For licensing information, see the file 'LICENSE' in the root folder of
 * this software module.
 *
 ******************************************************************************/
/* =========================================================================
 * This file incorporates work covered by the following copyright and
 * permission notice:
 * The Synopsys DWC ETHER XGMAC Software Driver and documentation (hereinafter
 * "Software") is an unsupported proprietary work of Synopsys, Inc. unless
 * otherwise expressly agreed to in writing between Synopsys and you.
 *
 * The Software IS NOT an item of Licensed Software or Licensed Product under
 * any End User Software License Agreement or Agreement for Licensed Product
 * with Synopsys or any supplement thereto.  Permission is hereby granted,
 * free of charge, to any person obtaining a copy of this software annotated
 * with this license and the Software, to deal in the Software without
 * restriction, including without limitation the rights to use, copy, modify,
 * merge, publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so, subject
 * to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THIS SOFTWARE IS BEING DISTRIBUTED BY SYNOPSYS SOLELY ON AN "AS IS" BASIS
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE HEREBY DISCLAIMED. IN NO EVENT SHALL SYNOPSYS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 * ========================================================================= */

#ifndef __XGMAC_H__
#define __XGMAC_H__

#include <xgmac_common.h>
#include <mac_cfg.h>

#define XGMAC_MIN_PACKET        60
#define XGMAC_STD_PACKET_MTU    1500
#define XGMAC_MAX_STD_PACKET    1518
#define XGMAC_JUMBO_PACKET_MTU  9000
#define XGMAC_MAX_JUMBO_PACKET  9018

/* Default maximum Gaint Packet Size Limit */
#define XGMAC_MAX_GPSL			9000
#define XGMAC_MAX_SUPPORTED_MTU		16380
#define FALCON_MAX_MTU			10000
#if defined(CHIPTEST) && CHIPTEST
#define NULL 0
#endif
/* Timestamp support - values based on 50MHz PTP clock
 *   50MHz => 20 nsec
 *   1 Mhz => 1000 nsec
 */
// PTP Clock in Mhz
#define PTP_CLK				500

#define XGMAC_TSTAMP_SNSINC		0

#define NSEC_TO_SEC			1000000000

#define MHZ_TO_HZ(val)			((val) * 1000000)

#define XGMAC_CTRL_REG(idx)		(0x1298 + ((idx - MAC_2) * 0x100))
#define XGMAC_DATA1_REG(idx)		(0x1294 + ((idx - MAC_2) * 0x100))
#define XGMAC_DATA0_REG(idx)		(0x1290 + ((idx - MAC_2) * 0x100))

/* MAC register offsets */
#define MAC_TX_CFG			0x0000
#define MAC_RX_CFG			0x0004
#define MAC_PKT_FR			0x0008
#define MAC_TX_FCR			0x0070
#define MAC_TX_FCR1			0x0074
#define MAC_TX_FCR2			0x0078
#define MAC_TX_FCR3			0x007C
#define MAC_TX_FCR4			0x0080
#define MAC_TX_FCR5			0x0084
#define MAC_TX_FCR6			0x0088
#define MAC_TX_FCR7			0x008C
#define MAC_RX_FCR			0x0090
#define MAC_ISR				0x00b0
#define MAC_IER				0x00b4
#define MAC_RXTX_STS			0x00b8
#define MAC_PMT_CSR			0x00c0
#define MAC_RWK_PFR			0x00c4
#define MAC_LPI_CSR			0x00d0
#define MAC_LPI_TCR			0x00d4
#define MAC_VR				0x0110
#define MAC_HW_F0			0x011c
#define MAC_HW_F1			0x0120
#define MAC_HW_F2			0x0124
#define MAC_EXTCFG			0x0140
#define MAC_MACA0HR			0x0300
#define MAC_MACA0LR			0x0304
#define MAC_MACA1HR			0x0308
#define MAC_MACA1LR			0x030c
#define MAC_TSTAMP_CR			0x0d00
#define MAC_SUBSEC_INCR			0x0d04
#define MAC_SYS_TIME_SEC		0x0d08
#define MAC_SYS_TIME_NSEC		0x0d0c
#define MAC_SYS_TIME_SEC_UPD		0x0d10
#define MAC_SYS_TIME_NSEC_UPD		0x0d14
#define MAC_TSTAMP_ADDNDR		0x0d18
#define MAC_SYS_TIME_HWS		0x0d1c
#define MAC_TSTAMP_STSR			0x0d20
#define MAC_RX_PCH_CRC_CNT		0x0d2c
#define MAC_TXTSTAMP_NSECR		0x0d30
#define MAC_TXTSTAMP_SECR		0x0d34
#define MAC_TXTSTAMP_STS		0x0d38
#define MAC_AUX_CTRL			0x0d40
#define MAC_AUX_NSEC			0x0d48
#define MAC_AUX_SEC			0x0d4c
#define MAC_TSTAMP_IG_CORR_NS		0x0d58
#define MAC_TSTAMP_EG_CORR_NS		0x0d60

/* MTL register offsets */
#define MTL_OMR				0x1000
#define MTL_DBG_CTL			0x1008
#define MTL_DBG_STS			0x100c
#define MTL_DBG_DAT			0x1010
#define MTL_ISR				0x1020
#define MTL_RQDCM0R			0x1030
#define MTL_TCPR_MAP0			0x1040

#define MTL_Q_TQOMR			0x1100
#define MTL_Q_TQUR			0x1104
#define MTL_Q_TQDG			0x1108
#define MTL_Q_RQOMR			0x1140
#define MTL_Q_RQMPOCR			0x1144
#define MTL_Q_RQDG			0x1148
#define MTL_Q_RQFCR			0x1150
#define MTL_Q_IER			0x1170
#define MTL_Q_ISR			0x1174


#define MAC_LPS_TLPIEN			0x00000001
#define MAC_LPS_TLPIEX			0x00000002
#define MAC_LPS_RLPIEN			0x00000004
#define MAC_LPS_RLPIEX			0x00000008

/* XGMAC_CTRL_REG */
#define XGMAC_CTRL_REG_ADDR_POS		0
#define XGMAC_CTRL_REG_ADDR_WIDTH	14
#define XGMAC_CTRL_REG_OPMODE_POS	14
#define XGMAC_CTRL_REG_OPMODE_WIDTH	1
#define XGMAC_CTRL_REG_BAS_POS		15
#define XGMAC_CTRL_REG_BAS_WIDTH	1

#define MAC_AUX_CTRL_ATSFC_POS		0
#define MAC_AUX_CTRL_ATSFC_WIDTH	1
#define MAC_AUX_CTRL_ATSEN0_POS		4
#define MAC_AUX_CTRL_ATSEN0_WIDTH	1
#define MAC_AUX_CTRL_ATSEN1_POS		5
#define MAC_AUX_CTRL_ATSEN1_WIDTH	1
#define MAC_AUX_CTRL_ATSEN2_POS		6
#define MAC_AUX_CTRL_ATSEN2_WIDTH	1
#define MAC_AUX_CTRL_ATSEN3_POS		7
#define MAC_AUX_CTRL_ATSEN3_WIDTH	1

/* MAC register entry bit positions and sizes */
#define MAC_HW_F0_ADDMACADRSEL_POS      18
#define MAC_HW_F0_ADDMACADRSEL_WIDTH    5
#define MAC_HW_F0_ARPOFFSEL_POS         9
#define MAC_HW_F0_ARPOFFSEL_WIDTH       1
#define MAC_HW_F0_EEESEL_POS            13
#define MAC_HW_F0_EEESEL_WIDTH          1
#define MAC_HW_F0_GMIISEL_POS           1
#define MAC_HW_F0_GMIISEL_WIDTH         1
#define MAC_HW_F0_MGKSEL_POS            7
#define MAC_HW_F0_MGKSEL_WIDTH          1
#define MAC_HW_F0_MMCSEL_POS            8
#define MAC_HW_F0_MMCSEL_WIDTH          1
#define MAC_HW_F0_RWKSEL_POS            6
#define MAC_HW_F0_RWKSEL_WIDTH          1
#define MAC_HW_F0_RXCOESEL_POS          16
#define MAC_HW_F0_RXCOESEL_WIDTH        1
#define MAC_HW_F0_SAVLANINS_POS         27
#define MAC_HW_F0_SAVLANINS_WIDTH       1
#define MAC_HW_F0_SMASEL_POS            5
#define MAC_HW_F0_SMASEL_WIDTH          1
#define MAC_HW_F0_TSSEL_POS             12
#define MAC_HW_F0_TSSEL_WIDTH           1
#define MAC_HW_F0_TSSTSSEL_POS          25
#define MAC_HW_F0_TSSTSSEL_WIDTH        2
#define MAC_HW_F0_TXCOESEL_POS          14
#define MAC_HW_F0_TXCOESEL_WIDTH        1
#define MAC_HW_F0_VLHASH_POS            4
#define MAC_HW_F0_VLHASH_WIDTH          1
#define MAC_HW_F0_VXN_POS		29
#define MAC_HW_F0_VXN_WIDTH		1
#define MAC_HW_F0_EDIFFC_POS		30
#define MAC_HW_F0_EDIFFC_WIDTH		1
#define MAC_HW_F0_EDMA_POS		31
#define MAC_HW_F0_EDMA_WIDTH		1

#define MAC_HW_F1_ADDR64_POS            14
#define MAC_HW_F1_ADDR64_WIDTH          2
#define MAC_HW_F1_ADVTHWORD_POS         13
#define MAC_HW_F1_ADVTHWORD_WIDTH       1
#define MAC_HW_F1_DBGMEMA_POS           19
#define MAC_HW_F1_DBGMEMA_WIDTH         1
#define MAC_HW_F1_DCBEN_POS             16
#define MAC_HW_F1_DCBEN_WIDTH           1
#define MAC_HW_F1_HASHTBLSZ_POS         24
#define MAC_HW_F1_HASHTBLSZ_WIDTH       3
#define MAC_HW_F1_L3L4FNUM_POS          27
#define MAC_HW_F1_L3L4FNUM_WIDTH        4
#define MAC_HW_F1_NUMTC_POS             21
#define MAC_HW_F1_NUMTC_WIDTH           3
#define MAC_HW_F1_RSSEN_POS             20
#define MAC_HW_F1_RSSEN_WIDTH           1
#define MAC_HW_F1_RXFIFOSIZE_POS        0
#define MAC_HW_F1_RXFIFOSIZE_WIDTH      5
#define MAC_HW_F1_SPHEN_POS             17
#define MAC_HW_F1_SPHEN_WIDTH           1
#define MAC_HW_F1_TSOEN_POS             18
#define MAC_HW_F1_TSOEN_WIDTH           1
#define MAC_HW_F1_TXFIFOSIZE_POS        6
#define MAC_HW_F1_TXFIFOSIZE_WIDTH      5
#define MAC_HW_F1_OSTEN_POS		11
#define MAC_HW_F1_OSTEN_WIDTH		1
#define MAC_HW_F1_PTOEN_POS             12
#define MAC_HW_F1_PTOEN_WIDTH           1

#define MAC_HW_F2_AUXSNAPNUM_POS        28
#define MAC_HW_F2_AUXSNAPNUM_WIDTH      3
#define MAC_HW_F2_PPSOUTNUM_POS         24
#define MAC_HW_F2_PPSOUTNUM_WIDTH       3
#define MAC_HW_F2_RXCHCNT_POS           12
#define MAC_HW_F2_RXCHCNT_WIDTH         4
#define MAC_HW_F2_RXQCNT_POS            0
#define MAC_HW_F2_RXQCNT_WIDTH          4
#define MAC_HW_F2_TXCHCNT_POS           18
#define MAC_HW_F2_TXCHCNT_WIDTH         4
#define MAC_HW_F2_TXQCNT_POS            6
#define MAC_HW_F2_TXQCNT_WIDTH          4

#define MAC_EXTCFG_SBDIOEN_POS		8
#define MAC_EXTCFG_SBDIOEN_WIDTH	1

#define MAC_IER_TSIE_POS                12
#define MAC_IER_TSIE_WIDTH              1
#define MAC_IER_TXESIE_POS              13
#define MAC_IER_TXESIE_WIDTH            1
#define MAC_IER_RXESIE_POS              14
#define MAC_IER_RXESIE_WIDTH            1
#define MAC_IER_LPIIE_POS               5
#define MAC_IER_LPIIE_WIDTH             1
#define MAC_IER_PMTIE_POS               4
#define MAC_IER_PMTIE_WIDTH             1

#define MAC_ISR_LSI_POS                 0
#define MAC_ISR_LSI_WIDTH               1
#define MAC_ISR_SMI_POS                 1
#define MAC_ISR_SMI_WIDTH               1
#define MAC_ISR_PMTIS_POS               4
#define MAC_ISR_PMTIS_WIDTH             1
#define MAC_ISR_LPIIS_POS               5
#define MAC_ISR_LPIIS_WIDTH             1
#define MAC_ISR_MMCRXIS_POS             9
#define MAC_ISR_MMCRXIS_WIDTH           1
#define MAC_ISR_MMCTXIS_POS             10
#define MAC_ISR_MMCTXIS_WIDTH           1
#define MAC_ISR_TSIS_POS                12
#define MAC_ISR_TSIS_WIDTH              1
#define MAC_ISR_TXESIS_POS              13
#define MAC_ISR_TXESIS_WIDTH            1
#define MAC_ISR_RXESIS_POS              14
#define MAC_ISR_RXESIS_WIDTH            1
#define MAC_ISR_GPIIS_POS               15
#define MAC_ISR_GPIIS_WIDTH             1
#define MAC_ISR_LS_POS                  24
#define MAC_ISR_LS_WIDTH                2

#define MAC_LPI_CSR_TLPIEN_POS      	0
#define MAC_LPI_CSR_TLPIEN_WIDTH    	1
#define MAC_LPI_CSR_TLPIEX_POS         	1
#define MAC_LPI_CSR_TLPIEX_WIDTH       	1
#define MAC_LPI_CSR_RLPIEN_POS          2
#define MAC_LPI_CSR_RLPIEN_WIDTH       	1
#define MAC_LPI_CSR_RLPIEX_POS          3
#define MAC_LPI_CSR_RLPIEX_WIDTH       	1
#define MAC_LPI_CSR_LPITXEN_POS          16
#define MAC_LPI_CSR_LPITXEN_WIDTH        1
#define MAC_LPI_CSR_LPIATE_POS           20
#define MAC_LPI_CSR_LPIATE_WIDTH         1
#define MAC_LPI_CSR_LPITXA_POS           19
#define MAC_LPI_CSR_LPITXA_WIDTH         1
#define MAC_LPI_CSR_PLS_POS              17
#define MAC_LPI_CSR_PLS_WIDTH            1

#define MAC_LPI_TCR_LST_POS              16
#define MAC_LPI_TCR_LST_WIDTH            10
#define MAC_LPI_TCR_TWT_POS              0
#define MAC_LPI_TCR_TWT_WIDTH            16

#define MAC_MACA1HR_AE_POS              31
#define MAC_MACA1HR_AE_WIDTH            1

#define MAC_PKT_FR_PM_POS                  4
#define MAC_PKT_FR_PM_WIDTH                1
#define MAC_PKT_FR_PR_POS                  0
#define MAC_PKT_FR_PR_WIDTH                1
#define MAC_PKT_FR_VTFE_POS                16
#define MAC_PKT_FR_VTFE_WIDTH              1
#define MAC_PKT_FR_RA_POS                  31
#define MAC_PKT_FR_RA_WIDTH                1
#define MAC_PKT_FR_PCF_POS                 6
#define MAC_PKT_FR_PCF_WIDTH               2

#define MAC_PMT_CSR_PWRDWN_POS		0
#define MAC_PMT_CSR_PWRDWN_WIDTH	1
#define MAC_PMT_CSR_MGKPKTEN_POS	1
#define MAC_PMT_CSR_MGKPKTEN_WIDTH	1
#define MAC_PMT_CSR_RWKPKTEN_POS	2
#define MAC_PMT_CSR_RWKPKTEN_WIDTH	1
#define MAC_PMT_CSR_MGKPRCVD_POS	5
#define MAC_PMT_CSR_MGKPRCVD_WIDTH	1
#define MAC_PMT_CSR_RWKPRCVD_POS	6
#define MAC_PMT_CSR_RWKPRCVD_WIDTH	1
#define MAC_PMT_CSR_RWKPFE_POS		10
#define MAC_PMT_CSR_RWKPFE_WIDTH	1
#define MAC_PMT_CSR_RWKPTR_POS		24
#define MAC_PMT_CSR_RWKPTR_WIDTH	5
#define MAC_PMT_CSR_GLBLUCAST_POS	9
#define MAC_PMT_CSR_GLBLUCAST_WIDTH	1
#define MAC_PMT_CSR_RWKFILTRST_POS	31
#define MAC_PMT_CSR_RWKFILTRST_WIDTH	1

#define MAC_TX_FCR_PT_POS               16
#define MAC_TX_FCR_PT_WIDTH             16
#define MAC_TX_FCR_TFE_POS              1
#define MAC_TX_FCR_TFE_WIDTH            1
#define MAC_TX_FCR_FCB_POS              0
#define MAC_TX_FCR_FCB_WIDTH            1
#define MAC_TX_FCR1_PT_POS               16
#define MAC_TX_FCR1_PT_WIDTH             16
#define MAC_TX_FCR1_TFE_POS              1
#define MAC_TX_FCR1_TFE_WIDTH            1
#define MAC_TX_FCR2_PT_POS               16
#define MAC_TX_FCR2_PT_WIDTH             16
#define MAC_TX_FCR2_TFE_POS              1
#define MAC_TX_FCR2_TFE_WIDTH            1
#define MAC_TX_FCR3_PT_POS               16
#define MAC_TX_FCR3_PT_WIDTH             16
#define MAC_TX_FCR3_TFE_POS              1
#define MAC_TX_FCR3_TFE_WIDTH            1
#define MAC_TX_FCR4_PT_POS               16
#define MAC_TX_FCR4_PT_WIDTH             16
#define MAC_TX_FCR4_TFE_POS              1
#define MAC_TX_FCR4_TFE_WIDTH            1
#define MAC_TX_FCR5_PT_POS               16
#define MAC_TX_FCR5_PT_WIDTH             16
#define MAC_TX_FCR5_TFE_POS              1
#define MAC_TX_FCR5_TFE_WIDTH            1
#define MAC_TX_FCR6_PT_POS               16
#define MAC_TX_FCR6_PT_WIDTH             16
#define MAC_TX_FCR6_TFE_POS              1
#define MAC_TX_FCR6_TFE_WIDTH            1
#define MAC_TX_FCR7_PT_POS               16
#define MAC_TX_FCR7_PT_WIDTH             16
#define MAC_TX_FCR7_TFE_POS              1
#define MAC_TX_FCR7_TFE_WIDTH            1


#define MAC_RX_CFG_ACS_POS                 1
#define MAC_RX_CFG_ACS_WIDTH               1
#define MAC_RX_CFG_CST_POS                 2
#define MAC_RX_CFG_CST_WIDTH               1
#define MAC_RX_CFG_DCRCC_POS               3
#define MAC_RX_CFG_DCRCC_WIDTH             1
#define MAC_RX_CFG_HDSMS_POS               12
#define MAC_RX_CFG_HDSMS_WIDTH             3
#define MAC_RX_CFG_IPC_POS                 9
#define MAC_RX_CFG_IPC_WIDTH               1
#define MAC_RX_CFG_JE_POS                  8
#define MAC_RX_CFG_JE_WIDTH                1
#define MAC_RX_CFG_LM_POS                  10
#define MAC_RX_CFG_LM_WIDTH                1
#define MAC_RX_CFG_RE_POS                  0
#define MAC_RX_CFG_RE_WIDTH                1
#define MAC_RX_CFG_WD_POS                  7
#define MAC_RX_CFG_WD_WIDTH		   1
#define MAC_RX_CFG_GPSLCE_POS              6
#define MAC_RX_CFG_GPSLCE_WIDTH		   1
#define MAC_RX_CFG_GPSL_POS                16
#define MAC_RX_CFG_GPSL_WIDTH		   14
#define MAC_RX_CFG_PRXM_POS                15
#define MAC_RX_CFG_PRXM_WIDTH		   1

#define MAC_RX_FCR_PFCE_POS               8
#define MAC_RX_FCR_PFCE_WIDTH             1
#define MAC_RX_FCR_RFE_POS                0
#define MAC_RX_FCR_RFE_WIDTH              1
#define MAC_RX_FCR_UP_POS                 1
#define MAC_RX_FCR_UP_WIDTH               1

#define MAC_RXTX_STS_RWT_POS		8
#define MAC_RXTX_STS_RWT_WIDTH		1
#define MAC_RXTX_STS_TJT_POS		0
#define MAC_RXTX_STS_TJT_WIDTH		1

#define MAC_SUBSEC_INCR_SNSINC_POS             8
#define MAC_SUBSEC_INCR_SNSINC_WIDTH           8
#define MAC_SUBSEC_INCR_SSINC_POS              16
#define MAC_SUBSEC_INCR_SSINC_WIDTH            8

#define MAC_SYS_TIME_NSEC_UPD_ADDSUB_POS	31
#define MAC_SYS_TIME_NSEC_UPD_ADDSUB_WIDTH	1
#define MAC_SYS_TIME_NSEC_UPD_TSSS_POS		0
#define MAC_SYS_TIME_NSEC_UPD_TSSS_WIDTH	31

#define MAC_TX_CFG_USS_POS                 31
#define MAC_TX_CFG_USS_WIDTH               1
#define MAC_TX_CFG_SS_POS                  29
#define MAC_TX_CFG_SS_WIDTH                2
#define MAC_TX_CFG_TE_POS                  0
#define MAC_TX_CFG_TE_WIDTH                1
#define MAC_TX_CFG_JD_POS                  16
#define MAC_TX_CFG_JD_WIDTH		   1
#define MAC_TX_CFG_IPG_POS                 8
#define MAC_TX_CFG_IPG_WIDTH		   3
#define MAC_TX_CFG_IFP_POS                 11
#define MAC_TX_CFG_IFP_WIDTH		   1
#define MAC_TX_CFG_PEN_POS                 19
#define MAC_TX_CFG_PEN_WIDTH		   1
#define MAC_TX_CFG_PCHM_POS                18
#define MAC_TX_CFG_PCHM_WIDTH		   1
#define MAC_TX_CFG_G9991EN_POS             28
#define MAC_TX_CFG_G9991EN_WIDTH	   1

#define MAC_TSTAMP_CR_AV8021ASMEN_POS        28
#define MAC_TSTAMP_CR_AV8021ASMEN_WIDTH      1
#define MAC_TSTAMP_CR_SNAPTYPSEL_POS         16
#define MAC_TSTAMP_CR_SNAPTYPSEL_WIDTH       2
#define MAC_TSTAMP_CR_TSADDREG_POS           5
#define MAC_TSTAMP_CR_TSADDREG_WIDTH         1
#define MAC_TSTAMP_CR_TSCFUPDT_POS           1
#define MAC_TSTAMP_CR_TSCFUPDT_WIDTH         1
#define MAC_TSTAMP_CR_TSCTRLSSR_POS          9
#define MAC_TSTAMP_CR_TSCTRLSSR_WIDTH        1
#define MAC_TSTAMP_CR_TSENA_POS              0
#define MAC_TSTAMP_CR_TSENA_WIDTH            1
#define MAC_TSTAMP_CR_TSENALL_POS            8
#define MAC_TSTAMP_CR_TSENALL_WIDTH          1
#define MAC_TSTAMP_CR_TSEVNTENA_POS          14
#define MAC_TSTAMP_CR_TSEVNTENA_WIDTH        1
#define MAC_TSTAMP_CR_TSINIT_POS             2
#define MAC_TSTAMP_CR_TSINIT_WIDTH           1
#define MAC_TSTAMP_CR_TSIPENA_POS            11
#define MAC_TSTAMP_CR_TSIPENA_WIDTH          1
#define MAC_TSTAMP_CR_TSIPV4ENA_POS          13
#define MAC_TSTAMP_CR_TSIPV4ENA_WIDTH        1
#define MAC_TSTAMP_CR_TSIPV6ENA_POS          12
#define MAC_TSTAMP_CR_TSIPV6ENA_WIDTH        1
#define MAC_TSTAMP_CR_TSMSTRENA_POS          15
#define MAC_TSTAMP_CR_TSMSTRENA_WIDTH        1
#define MAC_TSTAMP_CR_TSVER2ENA_POS          10
#define MAC_TSTAMP_CR_TSVER2ENA_WIDTH        1
#define MAC_TSTAMP_CR_TXTSSTSM_POS           24
#define MAC_TSTAMP_CR_TXTSSTSM_WIDTH         1
#define MAC_TSTAMP_CR_TSTRIG_POS             4
#define MAC_TSTAMP_CR_TSTRIG_WIDTH           1
#define MAC_TSTAMP_CR_TSUPDT_POS             3
#define MAC_TSTAMP_CR_TSUPDT_WIDTH           1
#define MAC_TSTAMP_CR_ESTI_POS               20
#define MAC_TSTAMP_CR_ESTI_WIDTH             1
#define MAC_TSTAMP_CR_CSC_POS               19
#define MAC_TSTAMP_CR_CSC_WIDTH             1

#define MAC_TSTAMP_STSR_ATSNS_POS              25
#define MAC_TSTAMP_STSR_ATSNS_WIDTH            5
#define MAC_TSTAMP_STSR_ATSSTM_POS             24
#define MAC_TSTAMP_STSR_ATSSTM_WIDTH           1
#define MAC_TSTAMP_STSR_ATSSTN_POS             16
#define MAC_TSTAMP_STSR_ATSSTN_WIDTH           4
#define MAC_TSTAMP_STSR_TXTSC_POS              15
#define MAC_TSTAMP_STSR_TXTSC_WIDTH            1
#define MAC_TSTAMP_STSR_TTSNS_POS              10
#define MAC_TSTAMP_STSR_TTSNS_WIDTH            5
#define MAC_TSTAMP_STSR_TSTRGTERR3_POS         9
#define MAC_TSTAMP_STSR_TSTRGTERR3_WIDTH       1
#define MAC_TSTAMP_STSR_TSTARGT3_POS           8
#define MAC_TSTAMP_STSR_TSTARGT3_WIDTH         1
#define MAC_TSTAMP_STSR_TSTRGTERR2_POS         7
#define MAC_TSTAMP_STSR_TSTRGTERR2_WIDTH       1
#define MAC_TSTAMP_STSR_TSTARGT2_POS           6
#define MAC_TSTAMP_STSR_TSTARGT2_WIDTH         1
#define MAC_TSTAMP_STSR_TSTRGTERR1_POS         5
#define MAC_TSTAMP_STSR_TSTRGTERR1_WIDTH       1
#define MAC_TSTAMP_STSR_TSTARGT1_POS           4
#define MAC_TSTAMP_STSR_TSTARGT1_WIDTH         1
#define MAC_TSTAMP_STSR_TSTRGTERR0_POS         3
#define MAC_TSTAMP_STSR_TSTRGTERR0_WIDTH       1
#define MAC_TSTAMP_STSR_TSTARGT0_POS           1
#define MAC_TSTAMP_STSR_TSTARGT0_WIDTH         1
#define MAC_TSTAMP_STSR_AUXTSTRIG_POS          2
#define MAC_TSTAMP_STSR_AUXTSTRIG_WIDTH        1
#define MAC_TSTAMP_STSR_TSSOVF_POS             0
#define MAC_TSTAMP_STSR_TSSOVF_WIDTH           1

#define MAC_TXTSTAMP_NSECR_TXTSSTSMIS_POS        31
#define MAC_TXTSTAMP_NSECR_TXTSSTSMIS_WIDTH      1

#define MAC_TXTSTAMP_STS_PKTID_POS        0
#define MAC_TXTSTAMP_STS_PKTID_WIDTH      10

#define MAC_VR_SNPSVER_POS              0
#define MAC_VR_SNPSVER_WIDTH            8

/* MMC register offsets */
#define MMC_CR                          0x0800

#define MMC_TXOCTETCOUNT_GB_LO          0x0814
#define MMC_TXFRAMECOUNT_GB_LO          0x081c
#define MMC_TXBROADCASTFRAMES_G_LO      0x0824
#define MMC_TXMULTICASTFRAMES_G_LO      0x082c
#define MMC_TXUNICASTFRAMES_GB_LO       0x0864
#define MMC_TXMULTICASTFRAMES_GB_LO     0x086c
#define MMC_TXBROADCASTFRAMES_GB_LO     0x0874
#define MMC_TXUNDERFLOWERROR_LO         0x087c
#define MMC_TXOCTETCOUNT_G_LO           0x0884
#define MMC_TXFRAMECOUNT_G_LO           0x088c
#define MMC_TXPAUSEFRAMES_LO            0x0894
#define MMC_TXVLANFRAMES_G_LO           0x089c

#define MMC_RXFRAMECOUNT_GB_LO          0x0900
#define MMC_RXOCTETCOUNT_GB_LO          0x0908
#define MMC_RXOCTETCOUNT_G_LO           0x0910
#define MMC_RXBROADCASTFRAMES_G_LO      0x0918
#define MMC_RXMULTICASTFRAMES_G_LO      0x0920
#define MMC_RXCRCERROR_LO               0x0928
#define MMC_RXRUNTERROR                 0x0930
#define MMC_RXJABBERERROR               0x0934
#define MMC_RXUNDERSIZE_G               0x0938
#define MMC_RXOVERSIZE_G                0x093c
#define MMC_RXUNICASTFRAMES_G_LO        0x0970
#define MMC_RXLENGTHERROR_LO            0x0978
#define MMC_RXOUTOFRANGETYPE_LO         0x0980
#define MMC_RXPAUSEFRAMES_LO            0x0988
#define MMC_RXFIFOOVERFLOW_LO           0x0990
#define MMC_RXVLANFRAMES_GB_LO          0x0998
#define MMC_RXWATCHDOGERROR             0x09a0

/* MMC register entry bit positions and sizes */
#define MMC_CR_CR_POS                         0
#define MMC_CR_CR_WIDTH                       1
#define MMC_CR_CSR_POS                        1
#define MMC_CR_CSR_WIDTH                      1
#define MMC_CR_ROR_POS                        2
#define MMC_CR_ROR_WIDTH                      1
#define MMC_CR_MCF_POS                        3
#define MMC_CR_MCF_WIDTH                      1
#define MMC_CR_MCT_POS                        4
#define MMC_CR_MCT_WIDTH                      2

/* MTL register entry bit positions and sizes */
#define MTL_OMR_ETSALG_POS			5
#define MTL_OMR_ETSALG_WIDTH			2
#define MTL_OMR_RAA_POS				2
#define MTL_OMR_RAA_WIDTH			1

/* MTL queue register entry bit positions and sizes */
#define MTL_Q_RQMPOCR_MISCNTOVF_POS	     31
#define MTL_Q_RQMPOCR_MISCNTOVF_WIDTH	      1
#define MTL_Q_RQMPOCR_MISPKTCNT_POS	     16
#define MTL_Q_RQMPOCR_MISPKTCNT_WIDTH	     11
#define MTL_Q_RQMPOCR_OVFCNTOVF_POS	     15
#define MTL_Q_RQMPOCR_OVFCNTOVF_WIDTH	      1
#define MTL_Q_RQMPOCR_OVFPKTCNT_POS	     0
#define MTL_Q_RQMPOCR_OVFPKTCNT_WIDTH	     11

#define MTL_Q_TQUR_UFCNTOVF_POS		     15
#define MTL_Q_TQUR_UFCNTOVF_WIDTH	      1
#define MTL_Q_TQUR_UFPKTCNT_POS		      0
#define MTL_Q_TQUR_UFPKTCNT_WIDTH	     11

#define MTL_Q_RQDR_PRXQ_POS                  16
#define MTL_Q_RQDR_PRXQ_WIDTH                14
#define MTL_Q_RQDR_RXQSTS_POS         4
#define MTL_Q_RQDR_RXQSTS_WIDTH       2
#define MTL_Q_RQFCR_RFA_POS           1
#define MTL_Q_RQFCR_RFA_WIDTH         6
#define MTL_Q_RQFCR_RFD_POS           17
#define MTL_Q_RQFCR_RFD_WIDTH         6
#define MTL_Q_RQOMR_EHFC_POS          7
#define MTL_Q_RQOMR_EHFC_WIDTH        1
#define MTL_Q_RQOMR_RQS_POS           16
#define MTL_Q_RQOMR_RQS_WIDTH         9
#define MTL_Q_RQOMR_RSF_POS           5
#define MTL_Q_RQOMR_RSF_WIDTH         1
#define MTL_Q_RQOMR_RTC_POS           0
#define MTL_Q_RQOMR_RTC_WIDTH         2
#define MTL_Q_RQOMR_FUP_POS           3
#define MTL_Q_RQOMR_FUP_WIDTH         1
#define MTL_Q_RQOMR_FEF_POS           4
#define MTL_Q_RQOMR_FEF_WIDTH         1

#define MTL_Q_RQDG_PRXQ_POS           16
#define MTL_Q_RQDG_PRXQ_WIDTH         14
#define MTL_Q_RQDG_RXQSTS_POS         4
#define MTL_Q_RQDG_RXQSTS_WIDTH       2
#define MTL_Q_RQDG_RRCSTS_POS         1
#define MTL_Q_RQDG_RRCSTS_WIDTH       2

#define MTL_Q_TQOMR_FTQ_POS           0
#define MTL_Q_TQOMR_FTQ_WIDTH         1
#define MTL_Q_TQOMR_Q2TCMAP_POS       8
#define MTL_Q_TQOMR_Q2TCMAP_WIDTH     3
#define MTL_Q_TQOMR_TQS_POS           16
#define MTL_Q_TQOMR_TQS_WIDTH         10
#define MTL_Q_TQOMR_TSF_POS           1
#define MTL_Q_TQOMR_TSF_WIDTH         1
#define MTL_Q_TQOMR_TTC_POS           4
#define MTL_Q_TQOMR_TTC_WIDTH         3
#define MTL_Q_TQOMR_TXQEN_POS         2
#define MTL_Q_TQOMR_TXQEN_WIDTH       2

#define MTL_Q_TQDG_TXQSTS_POS		4
#define MTL_Q_TQDG_TXQSTS_WIDTH		1
#define MTL_Q_TQDG_TRCSTS_POS		1
#define MTL_Q_TQDG_TRCSTS_WIDTH		2

#define MTL_Q_IER_TXUIE_POS		0
#define MTL_Q_IER_TXUIE_WIDTH		1
#define MTL_Q_IER_ABPSIE_POS		1
#define MTL_Q_IER_ABPSIE_WIDTH		1
#define MTL_Q_IER_RXOIE_POS		16
#define MTL_Q_IER_RXOIE_WIDTH		1

#define MTL_Q_ISR_TXUNFIS_POS		0
#define MTL_Q_ISR_TXUNFIS_WIDTH		1
#define MTL_Q_ISR_ABPSIS_POS		1
#define MTL_Q_ISR_ABPSIS_WIDTH		1
#define MTL_Q_ISR_RXOVFIS_POS		16
#define MTL_Q_ISR_RXOVFIS_WIDTH		1

/* MTL DEBUG CONTROL */
#define MTL_DBG_CTL_FDBGEN_POS		0
#define MTL_DBG_CTL_FDBGEN_WIDTH	1
#define MTL_DBG_CTL_DBGMOD_POS		1
#define MTL_DBG_CTL_DBGMOD_WIDTH	1
#define MTL_DBG_CTL_PKTSTATE_POS	5
#define MTL_DBG_CTL_PKTSTATE_WIDTH	2
#define MTL_DBG_CTL_RSTALL_POS		8
#define MTL_DBG_CTL_RSTALL_WIDTH	1
#define MTL_DBG_CTL_RSTSEL_POS		9
#define MTL_DBG_CTL_RSTSEL_WIDTH	1
#define MTL_DBG_CTL_FIFORDEN_POS	10
#define MTL_DBG_CTL_FIFORDEN_WIDTH	1
#define MTL_DBG_CTL_FIFOWREN_POS	11
#define MTL_DBG_CTL_FIFOWREN_WIDTH	1
#define MTL_DBG_CTL_FIFOSEL_POS		12
#define MTL_DBG_CTL_FIFOSEL_WIDTH	2
#define MTL_DBG_CTL_PKTIE_POS		14
#define MTL_DBG_CTL_PKTIE_WIDTH		2
#define MTL_DBG_CTL_BYTEEN_POS		2
#define MTL_DBG_CTL_BYTEEN_WIDTH	2

/* MTL DEBUG STATUS */
#define MTL_DBG_STS_FIFOBUSY_POS	0
#define MTL_DBG_STS_FIFOBUSY_WIDTH	1
#define MTL_DBG_STS_PKTSTATE_POS	1
#define MTL_DBG_STS_PKTSTATE_WIDTH	2
#define MTL_DBG_STS_PKTI_POS		8
#define MTL_DBG_STS_PKTI_WIDTH		1
#define MTL_DBG_STS_LOCR_POS		16
#define MTL_DBG_STS_LOCR_WIDTH		16
#define MTL_DBG_STS_BYTEEN_POS		3
#define MTL_DBG_STS_BYTEEN_WIDTH	2

/* MTL DEBUG DATA */
#define MTL_DBG_DAT_FDBGDATA_POS	0
#define MTL_DBG_CTL_FDBGDATA_WIDTH	32

#define MTL_TCPR_MAP0_PSTC0_POS		0
#define MTL_TCPR_MAP0_PSTC0_WIDTH	8

enum {
	XGMAC_256 = 0x0,
	XGMAC_512 = 0x1,
	XGMAC_1k = 0x3,
	XGMAC_2k = 0x7,
	XGMAC_4k = 0xf,
	XGMAC_8k = 0x1f,
	XGMAC_16k = 0x3f,
	XGMAC_32k = 0x7f
};

enum {
	TX_PKT_DATA,
	TX_CTRL_WORD,
	TX_SOP_DATA,
	TX_EOP_DATA,
};

enum {
	RX_PKT_DATA,
	RX_NORMAL_STS,
	RX_LAST_STS,
	RX_EOP_DATA
};

struct _vlan_tag {
	u32 vlan_tag2: 16;
	u32 vlan_tag1: 16;
};

struct _pkt_len_ctrl {
	u32 vxn: 1;
	u32 svtv: 1;
	u32 vtv: 1;
	u32 vtir: 2;
	u32 saic: 3;
	u32 ivtir: 2;
	u32 cic_ost_avail: 2;
	u32 ostc: 1;
	u32 ttse: 1;
	u32 cpc: 2;
	u32 ost_avail: 1;
	u32 res: 11;
	u32 pl: 5;
};

struct ctrl_word0 {
	struct _vlan_tag vlan_tag;
	struct _pkt_len_ctrl pkt_len_ctrl;
};

struct ctrl_word1 {
	u32 ttsh;
	u32 ttsl;
};

struct ctrl_word {
	struct ctrl_word0 w0;
	struct ctrl_word1 w1;
};

enum  {
	XGMAC_SPEED_1000 = 0,
	XGMAC_SPEED_2500,
	XGMAC_SPEED_10000,
	XGMAC_SPEEDS,
};

#define MASK(reg, field)		(1 << reg##_##field##_POS)
#define MASK_N_BITS(reg, field)		((1 << (reg##_##field##_WIDTH)) - 1)


#ifdef __KERNEL__
static inline u32 XGMAC_IO_R32(struct mac_prv_data *pdata, u16 reg)
{
	u32 reg_val, idx = 0;
	void __iomem *xgmac_ctrl_reg  =
		(void __iomem *)(pdata->ss_addr_base + pdata->xgmac_ctrl_reg);
	void __iomem *xgmac_data0_reg =
		(void __iomem *)pdata->ss_addr_base + pdata->xgmac_data0_reg;
	void __iomem *xgmac_data1_reg =
		(void __iomem *)pdata->ss_addr_base + pdata->xgmac_data1_reg;

	mac_w32((0x8000 | reg), xgmac_ctrl_reg);

	do {
		if ((mac_r32(xgmac_ctrl_reg) & 0x8000) == 0)
			break;

	} while (++idx <= MAX_RETRY);

	if (idx >= MAX_RETRY)
		pr_err(":::: Xgmac register read failed for Offset %x ::::\n",reg);

	reg_val = ((mac_r32(xgmac_data1_reg) << 16) |
		   (mac_r32(xgmac_data0_reg)));

	return reg_val;
}

static inline void XGMAC_IO_W32(struct mac_prv_data *pdata, u16 reg,
				u32 val)
{
	u32 idx = 0;
	void __iomem *xgmac_ctrl_reg  =
		(void __iomem *)(pdata->ss_addr_base + pdata->xgmac_ctrl_reg);
	void __iomem *xgmac_data0_reg =
		(void __iomem *)(pdata->ss_addr_base + pdata->xgmac_data0_reg);
	void __iomem *xgmac_data1_reg =
		(void __iomem *)(pdata->ss_addr_base + pdata->xgmac_data1_reg);

	mac_w32(((val & 0xFFFF0000) >> 16), xgmac_data1_reg);
	mac_w32((val & 0x0000FFFF), xgmac_data0_reg);
	mac_w32((0xC000 | reg), xgmac_ctrl_reg);

	do {
		if ((mac_r32(xgmac_ctrl_reg) & 0x8000) == 0)
			break;

	} while (++idx <= MAX_RETRY);

	if (idx >= MAX_RETRY)
		pr_err(":::: Xgmac register write failed for Offset %x ::::\n",reg);

}
#else
static inline u32 XGMAC_R32(struct mac_prv_data *pdata, u16 reg)
{
	u32 reg_val;
	u32 xgmac_ctrl_reg  = pdata->ss_addr_base + pdata->xgmac_ctrl_reg;
	u32 xgmac_data0_reg = pdata->ss_addr_base + pdata->xgmac_data0_reg;
	u32 xgmac_data1_reg = pdata->ss_addr_base + pdata->xgmac_data1_reg;

	REG32(xgmac_ctrl_reg) = (0x8000 | reg);

	while (1) {
		if ((REG32(xgmac_ctrl_reg) & 0x8000) == 0)
			break;
	}

	reg_val = ((REG32(xgmac_data1_reg) << 16) | (REG32(xgmac_data0_reg)));

	return reg_val;
}

static inline void XGMAC_W32(struct mac_prv_data *pdata, u16 reg, u32 val)
{
	u32 xgmac_ctrl_reg  = pdata->ss_addr_base + pdata->xgmac_ctrl_reg;
	u32 xgmac_data0_reg = pdata->ss_addr_base + pdata->xgmac_data0_reg;
	u32 xgmac_data1_reg = pdata->ss_addr_base + pdata->xgmac_data1_reg;

	REG32(xgmac_data1_reg) = ((val & 0xFFFF0000) >> 16);
	REG32(xgmac_data0_reg) = (val & 0x0000FFFF);
	REG32(xgmac_ctrl_reg) = (0xC000 | reg);

	while (1) {
		if ((REG32(xgmac_ctrl_reg) & 0x8000) == 0)
			break;
	}
}

#if defined(PC_UTILITY) && PC_UTILITY
static inline u32 XGMAC_URT_R32(struct mac_prv_data *pdata, u16 reg)
{
	u32 reg_val;
	u32 xgmac_ctrl_reg  = pdata->ss_addr_base + pdata->xgmac_ctrl_reg;
	u32 xgmac_data0_reg = pdata->ss_addr_base + pdata->xgmac_data0_reg;
	u32 xgmac_data1_reg = pdata->ss_addr_base + pdata->xgmac_data1_reg;
	u32 data0_val, data1_val;

	pcuart_reg_wr(xgmac_ctrl_reg, (0x8000 | reg));

	while (1) {
		pcuart_reg_rd(xgmac_ctrl_reg, &reg_val);

		if ((reg_val & 0x8000) == 0)
			break;
	}

	pcuart_reg_rd(xgmac_data1_reg, &data1_val);
	pcuart_reg_rd(xgmac_data0_reg, &data0_val);
	reg_val = ((data1_val << 16) | data0_val);

	return reg_val;
}

static inline void XGMAC_URT_W32(struct mac_prv_data *pdata, u16 reg,
				 u32 val)
{
	u32 xgmac_ctrl_reg  = pdata->ss_addr_base + pdata->xgmac_ctrl_reg;
	u32 xgmac_data0_reg = pdata->ss_addr_base + pdata->xgmac_data0_reg;
	u32 xgmac_data1_reg = pdata->ss_addr_base + pdata->xgmac_data1_reg;
	u32 reg_val;

	pcuart_reg_wr(xgmac_data1_reg, ((val & 0xFFFF0000) >> 16));
	pcuart_reg_wr(xgmac_data0_reg, (val & 0x0000FFFF));
	pcuart_reg_wr(xgmac_ctrl_reg, (0xC000 | reg));

	while (1) {
		pcuart_reg_rd(xgmac_ctrl_reg, &reg_val);

		if ((reg_val & 0x8000) == 0)
			break;
	}
}

#endif
#endif

static inline u32 XGMAC_RGRD(struct mac_prv_data *pdata, u16 reg)
{
	u32 reg_val;
#if defined(PC_UTILITY) && PC_UTILITY
	reg_val = XGMAC_URT_R32(pdata, reg);
#endif
#if defined(CHIPTEST) && CHIPTEST
	reg_val = XGMAC_R32(pdata, reg);
#endif
#ifdef __KERNEL__
	spin_lock_bh(&pdata->rw_lock);
	reg_val = XGMAC_IO_R32(pdata, reg);
	spin_unlock_bh(&pdata->rw_lock);
#endif

	return reg_val;
}

static inline void XGMAC_RGWR(struct mac_prv_data *pdata, u16 reg, u32 val)
{
#if defined(PC_UTILITY) && PC_UTILITY
	XGMAC_URT_W32(pdata, reg, val);
#endif
#if defined(CHIPTEST) && CHIPTEST
	XGMAC_W32(pdata, reg, val);
#endif
#ifdef __KERNEL__
	spin_lock_bh(&pdata->rw_lock);
	XGMAC_IO_W32(pdata, reg, val);
	spin_unlock_bh(&pdata->rw_lock);
#endif
}

#define XGMAC_RGRD_BITS(pdata, reg, field)			\
	GET_N_BITS(XGMAC_RGRD(pdata, reg),			\
		   reg##_##field##_POS,				\
		   reg##_##field##_WIDTH)

#define XGMAC_RGWR_BITS(pdata, reg, field, _val)		\
	do {							\
		u32 reg_val = XGMAC_RGRD(pdata, reg);		\
		SET_N_BITS(reg_val,				\
			   reg##_##field##_POS,			\
			   reg##_##field##_WIDTH, (_val));	\
		XGMAC_RGWR(pdata, reg, reg_val);		\
	} while (0)

/* Function prototypes*/

/* MTL related features*/
int xgmac_set_mtl_rx_mode(void *pdev, u32 rx_mode);
int xgmac_set_mtl_rx_thresh(void *pdev, u32 rx_thresh);
int xgmac_set_mtl_tx_mode(void *pdev, u32 tx_mode);
int xgmac_set_mtl_tx_thresh(void *pdev, u32 tx_thresh);
int xgmac_clear_mtl_int(void *pdev, u32 event);
int xgmac_set_mtl_int(void *pdev, u32 val);
int xgmac_flush_tx_queues(void *pdev);
int xgmac_set_flow_control_threshold(void *pdev,
				     u32 rfa, u32 rfd);
int xgmac_set_mmc(void *pdev);

int xgmac_clear_rmon(void *pdev);
int xgmac_read_mmc_stats(void *pdev, struct xgmac_mmc_stats *stats);
int xgmac_set_debug_ctl(void *pdev, u32 dbg_en, u32 dbg_mode);
int xgmac_set_debug_data(void *pdev, u32 dbg_data);
int xgmac_tx_debug_data(void *pdev, u32 dbg_pktstate);
int xgmac_rx_debug_data(void *pdev, u32 *dbg_pktstate, u32 *dbg_byteen);
int xgmac_set_rx_debugctrl_int(void *pdev, u32 dbg_pktie);
int xgmac_set_fifo_reset(void *pdev, u32 dbg_rst_sel, u32 dbg_rst_all);
int xgmac_poll_fifo_sts(void *pdev);
int xgmac_poll_pkt_rx(void *pdev);
int xgmac_clr_debug_sts(void *pdev);
int xgmac_forward_fup_fep_pkt(void *pdev, u32 fup, u32 fef);
int xgmac_disable_tstamp(void *pdev);

/* MAC related features */

int xgmac_set_loopback(void *pdev, u32 val);
int xgmac_disable_tx_flow_ctl(void *pdev);
int xgmac_disable_rx_flow_ctl(void *pdev);
int xgmac_enable_tx_flow_ctl(void *pdev, u32 pause_time);
int xgmac_enable_rx_flow_ctl(void *pdev);
int xgmac_initiate_pause_tx(void *pdev);
int xgmac_clear_mac_int(void *pdev);
int xgmac_set_mac_int(void *pdev, u32 event, u32 val);
int xgmac_set_gmii_speed(void *pdev);
int xgmac_set_gmii_2500_speed(void *pdev);
int xgmac_set_xgmii_2500_speed(void *pdev);
int xgmac_set_xgmii_speed(void *pdev);
int xgmac_set_promiscuous_mode(void *pdev, u32 val);
int xgmac_set_all_multicast_mode(void *pdev, u32 val);
int xgmac_set_mac_address(void *pdev, u8 *mac_addr);
int xgmac_set_checksum_offload(void *pdev, u32 val);
int xgmac_set_tstamp_addend(void *pdev, u32 tstamp_addend);
int xgmac_init_systime(void *pdev, u64 sec, u32 nsec);
int xgmac_adjust_systime(void *pdev, u32 sec, u32 nsec, u32 add_sub,
			 u32 one_nsec_accuracy);
u64 xgmac_get_systime(void *pdev);
u64 xgmac_get_tx_tstamp(void *pdev);
int xgmac_config_tstamp(void *pdev, u32 mac_tscr);
int xgmac_get_hwtstamp_settings(void *pdev,
				struct hwtstamp_config *config);
int xgmac_set_hwtstamp_settings(void *pdev, u32 tx_type,
				u32 rx_filter);
int xgmac_config_jumbo_pkt(void *pdev, u32 mtu);
int xgmac_config_std_pkt(void *pdev);
void xgmac_soft_restart(void *pdev);
int xgmac_powerup(void *pdev);
int xgmac_powerdown(void *pdev);
int xgmac_config_subsec_inc(void *pdev, u32 ptp_clk);
int xgmac_ptp_txtstamp_mode(void *pdev,
			    u32 snaptypesel,
			    u32 tsmstrena,
			    u32 tsevntena);

int xgmac_set_eee_mode(void *pdev, u32 val);
int xgmac_set_eee_pls(void *pdev, u32 val);
int xgmac_set_eee_timer(void *pdev, u32 twt, u32 lst);
int xgmac_set_crc_strip_type(void *pdev, u32 val);
int xgmac_set_acs(void *pdev, u32 val);
int xgmac_set_ipg(void *pdev, u32 ipg);
int xgmac_set_magic_pmt(void *pdev, u32 val);
int xgmac_set_rwk_pmt(void *pdev, u32 val);
int xgmac_set_extcfg(void *pdev, u32 val);
int xgmac_set_mac_rxtx(void *pdev, u32 wd, u32 jd);
int xgmac_set_rwk_filter_registers(void *pdev, u32 count,
				   u32 *val);
int xgmac_set_pmt_gucast(void *pdev, u32 val);
int xgmac_set_ptp_offload(void *pdev, u32 type, u32 val);
int xgmac_set_ptp_offload_msg_gen(void *pdev, u32 mode);
int xgmac_set_mac_lpitx(void *pdev, u32 val);
int xgmac_pause_frame_filtering(void *pdev, u32 val);
int xgmac_set_pch(void *pdev, u32 pch_en, u32 pch_rx, u32 pch_tx);
int xgmac_set_pfc(void *pdev, u32 enable);
int xgmac_set_gint(void *pdev, u32 val);
int xgmac_set_rxcrc(void *pdev, u32 val);
int xgmac_set_exttime_source(void *pdev, u32 val);


/* GET API's */
int xgmac_get_hw_capability(void *pdev);
int xgmac_print_hw_cap(void *pdev);
int xgmac_dbg_eee_status(void *pdev);
int xgmac_get_priv_data(void *pdev);
int xgmac_get_stats(void *pdev);
int xgmac_get_tx_cfg(void *pdev);
int xgmac_get_counters_cfg(void *pdev);
int xgmac_get_fifo_size(void *pdev);
int xgmac_get_flow_control_threshold(void *pdev);
int xgmac_get_mtl_rx_flow_ctl(void *pdev);
int xgmac_get_mtl_tx(void *pdev);
int xgmac_get_tx_threshold(void *pdev);
int xgmac_get_rx_threshold(void *pdev);
int xgmac_get_mtl_rx(void *pdev);
int xgmac_get_mtl_q_alg(void *pdev);
int xgmac_get_crc_settings(void *pdev);
int xgmac_get_eee_settings(void *pdev);
int xgmac_get_mac_settings(void *pdev);
int xgmac_get_mtu_settings(void *pdev);
int xgmac_get_checksum_offload(void *pdev);
int xgmac_get_mac_addr(void *pdev);
int xgmac_get_mac_rx_mode(void *pdev);
int xgmac_get_rx_vlan_filtering_mode(void *pdev);
int xgmac_get_mac_speed(void *pdev);
int xgmac_get_pause_frame_ctl(void *pdev);
int xgmac_get_mac_loopback_mode(void *pdev);
int xgmac_get_tstamp_settings(void *pdev);
u64 xgmac_get_tx_tstamp(void *pdev);
int xgmac_get_stats_all(void *pdev);
int xgmac_get_fup_fep_setting(void *pdev);
int xgmac_get_int_sts(void *pdev);
int xgmac_dbg_int_sts(void *pdev);
int xgmac_get_ipg(void *pdev);
int xgmac_get_txtstamp_cnt(void *pdev);
int xgmac_get_txtstamp_pktid(void *pdev);
int xgmac_get_pch_crc_cnt(void *pdev);


#if defined(CHIPTEST) && CHIPTEST
u32 xgmac_mmc_read(void *pdev, u32 reg_lo);
#else
u64 xgmac_mmc_read(void *pdev, u32 reg_lo);
#endif
int xgmac_get_mtl_int_sts(void *pdev);
int xgmac_dbg_pmt(void *pdev);
int xgmac_get_debug_sts(void *pdev);
int xgmac_get_debug_data(void *pdev, u32 *dbg_data);
int xgmac_get_extcfg(void *pdev);
int xgmac_get_mac_rxtx_sts(void *pdev);
int xgmac_get_mtl_missed_pkt_cnt(void *pdev);
int xgmac_get_mtl_underflow_pkt_cnt(void *pdev);
int xgmac_get_tstamp_status(void *pdev);
int xgmac_get_ptp_offload_msg_gen(void *pdev);
int xgmac_get_ptp_offload(void *pdev);
u32 xgmac_get_eee_mode(void *pdev);
int xgmac_get_txtstamp_mode(void *pdev);
int xgmac_print_system_time(void *pdev);

int xgmac_pmt_int_clr(void *pdev);
int xgmac_lpi_int_clr(void *pdev);
int xgmac_ptp_isr_hdlr(void *pdev);

#endif
