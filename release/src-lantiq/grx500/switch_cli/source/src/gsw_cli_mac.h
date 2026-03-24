#ifndef GSW_CLI_MAC_H_
#define GSW_CLI_MAC_H_

#define NANOSEC_IN_ONESEC 		550000
#define NSEC_TO_SEC			1000000000


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
#define MAC_TSTAMP_STSR			0x0d20
#define MAC_TXTSTAMP_NSECR		0x0d30
#define MAC_TXTSTAMP_SECR		0x0d34
#define MAC_TXTSTAMP_STS		0x0d38
#define MAC_AUX_CTRL			0x0d40
#define MAC_AUX_NSEC			0x0d48
#define MAC_AUX_SEC			0x0d4c
#define MAC_RX_PCH_CRC_CNT		0x0d2c

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

#define MTL_OMR_ETSALG_POS			5
#define MTL_OMR_ETSALG_WIDTH			2
#define MTL_OMR_RAA_POS				2
#define MTL_OMR_RAA_WIDTH			1

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

#define MTL_DBG_DAT_FDBGDATA_POS	0
#define MTL_DBG_CTL_FDBGDATA_WIDTH	32

#define MTL_TCPR_MAP0_PSTC0_POS		0
#define MTL_TCPR_MAP0_PSTC0_WIDTH	8

#define MDIO_SCAR		0x200
#define MDIO_SCCDR		0x204
#define MDIO_CWAR		0x208
#define MDIO_CWDR		0x20C
#define MDIO_CSPE		0x210
#define MDIO_ISR		0x214
#define MDIO_IER		0x218
#define MDIO_PSR		0x21C
#define MDIO_C22P		0x220

#define MDIO_SCAR_DA_POS	21
#define MDIO_SCAR_DA_WIDTH	 5
#define MDIO_SCAR_PA_POS	16
#define MDIO_SCAR_PA_WIDTH	 5
#define MDIO_SCAR_RA_POS	 0

#define MDIO_SCAR_RA_WIDTH	16

#define MDIO_SCCDR_BUSY_POS	22
#define MDIO_SCCDR_BUSY_WIDTH	1
#define MDIO_SCCDR_CR_POS		19
#define MDIO_SCCDR_CR_WIDTH		3
#define MDIO_SCCDR_SADDR_POS	18
#define MDIO_SCCDR_SADDR_WIDTH	1
#define MDIO_SCCDR_CMD_POS	16
#define MDIO_SCCDR_CMD_WIDTH	2
#define MDIO_SCCDR_SDATA_POS	0
#define MDIO_SCCDR_SDATA_WIDTH	16

#define MDIO_CWAR_CWB_POS		22
#define MDIO_CWAR_CWB_WIDTH		1
#define MDIO_CWAR_CPRT_POS		21
#define MDIO_CWAR_CPRT_WIDTH		1
#define MDIO_CWAR_CADDR_POS		16
#define MDIO_CWAR_CADDR_WIDTH		5
#define MDIO_CWAR_CREGADDR_POS		0
#define MDIO_CWAR_CREGADDR_WIDTH	16

#define MDIO_CWDR_CDATA_POS		0
#define MDIO_CWDR_CDATA_WIDTH		16

#define MDIO_CSPE_PORT3_POS		3
#define MDIO_CSPE_PORT3_WIDTH		1
#define MDIO_CSPE_PORT2_POS		2
#define MDIO_CSPE_PORT2_WIDTH		1
#define MDIO_CSPE_PORT1_POS		1
#define MDIO_CSPE_PORT1_WIDTH		1
#define MDIO_CSPE_PORT0_POS		0
#define MDIO_CSPE_PORT0_WIDTH		1

#define MDIO_ISR_CWCOMPINT_POS		13
#define MDIO_ISR_CWCOMPINT_WIDTH	1
#define MDIO_ISR_SNGLCOMPINT_POS	12
#define MDIO_ISR_SNGLCOMPINT_WIDTH	1
#define MDIO_ISR_PRT3ALINT_POS		11
#define MDIO_ISR_PRT3ALINT_WIDTH	1
#define MDIO_ISR_PRT2ALINT_POS		10
#define MDIO_ISR_PRT2ALINT_WIDTH	1
#define MDIO_ISR_PRT1ALINT_POS		9
#define MDIO_ISR_PRT1ALINT_WIDTH	1
#define MDIO_ISR_PRT0ALINT_POS		8
#define MDIO_ISR_PRT0ALINT_WIDTH	1
#define MDIO_ISR_PRT3LSINT_POS		7
#define MDIO_ISR_PRT3LSINT_WIDTH	1
#define MDIO_ISR_PRT2LSINT_POS		6
#define MDIO_ISR_PRT2LSINT_WIDTH	1
#define MDIO_ISR_PRT1LSINT_POS		5
#define MDIO_ISR_PRT1LSINT_WIDTH	1
#define MDIO_ISR_PRT0LSINT_POS		4
#define MDIO_ISR_PRT0LSINT_WIDTH	1
#define MDIO_ISR_PRT3CONINT_POS		3
#define MDIO_ISR_PRT3CONINT_WIDTH	1
#define MDIO_ISR_PRT2CONINT_POS		2
#define MDIO_ISR_PRT2CONINT_WIDTH	1
#define MDIO_ISR_PRT1CONINT_POS		1
#define MDIO_ISR_PRT1CONINT_WIDTH	1
#define MDIO_ISR_PRT0CONINT_POS		0
#define MDIO_ISR_PRT0CONINT_WIDTH	1

#define MDIO_IER_CWCOMPIE_POS		13
#define MDIO_IER_CWCOMPIE_WIDTH		1
#define MDIO_IER_SNGLCOMPIE_POS		12
#define MDIO_IER_SNGLCOMPIE_WIDTH	1
#define MDIO_IER_PRT3ALIE_POS		11
#define MDIO_IER_PRT3ALIE_WIDTH		1
#define MDIO_IER_PRT2ALIE_POS		10
#define MDIO_IER_PRT2ALIE_WIDTH		1
#define MDIO_IER_PRT1ALIE_POS		9
#define MDIO_IER_PRT1ALIE_WIDTH		1
#define MDIO_IER_PRT0ALIE_POS		8
#define MDIO_IER_PRT0ALIE_WIDTH		1
#define MDIO_IER_PRT3LSIE_POS		7
#define MDIO_IER_PRT3LSIE_WIDTH		1
#define MDIO_IER_PRT2LSIE_POS		6
#define MDIO_IER_PRT2LSIE_WIDTH		1
#define MDIO_IER_PRT1LSIE_POS		5
#define MDIO_IER_PRT1LSIE_WIDTH		1
#define MDIO_IER_PRT0LSIE_POS		4
#define MDIO_IER_PRT0LSIE_WIDTH		1
#define MDIO_IER_PRT3CONIE_POS		3
#define MDIO_IER_PRT3CONIE_WIDTH	1
#define MDIO_IER_PRT2CONIE_POS		2
#define MDIO_IER_PRT2CONIE_WIDTH	1
#define MDIO_IER_PRT1CONIE_POS		1
#define MDIO_IER_PRT1CONIE_WIDTH	1
#define MDIO_IER_PRT0CONIE_POS		0
#define MDIO_IER_PRT0CONIE_WIDTH	1

#define MDIO_PSR_PORT3CON_POS		3
#define MDIO_PSR_PORT3CON_WIDTH		1
#define MDIO_PSR_PORT2CON_POS		2
#define MDIO_PSR_PORT2CON_WIDTH		1
#define MDIO_PSR_PORT1CON_POS		1
#define MDIO_PSR_PORT1CON_WIDTH		1
#define MDIO_PSR_PORT0CON_POS		0
#define MDIO_PSR_PORT0CON_WIDTH		1

#define MDIO_CL22P_PORT3_POS		3
#define MDIO_CL22P_PORT3_WIDTH		1
#define MDIO_CL22P_PORT2_POS		2
#define MDIO_CL22P_PORT2_WIDTH		1
#define MDIO_CL22P_PORT1_POS		1
#define MDIO_CL22P_PORT1_WIDTH		1
#define MDIO_CL22P_PORT0_POS		0
#define MDIO_CL22P_PORT0_WIDTH		1

#define MDIO_DIUR_PVSD2_POS		31
#define MDIO_DIUR_PVSD2_WIDTH		1
#define MDIO_DIUR_PVSD1_POS		30
#define MDIO_DIUR_PVSD1_WIDTH		1
#define MDIO_DIUR_PTC_POS		6
#define MDIO_DIUR_PTC_WIDTH		1
#define MDIO_DIUR_PDTEXS_POS		5
#define MDIO_DIUR_PDTEXS_WIDTH		1
#define MDIO_DIUR_PPHYXS_POS		4
#define MDIO_DIUR_PPHYXS_WIDTH		1
#define MDIO_DIUR_PPCS_POS		3
#define MDIO_DIUR_PPCS_WIDTH		1
#define MDIO_DIUR_PWIS_POS		2
#define MDIO_DIUR_PWIS_WIDTH		1
#define MDIO_DIUR_PPMDPMA_POS		1
#define MDIO_DIUR_PPMDPMA_WIDTH		1

#define MDIO_LSR_PVSD2_POS		31
#define MDIO_LSR_PVSD2_WIDTH		1
#define MDIO_LSR_PVSD1_POS		30
#define MDIO_LSR_PVSD1_WIDTH		1
#define MDIO_LSR_PTC_POS		6
#define MDIO_LSR_PTC_WIDTH		1
#define MDIO_LSR_PDTEXS_POS		5
#define MDIO_LSR_PDTEXS_WIDTH		1
#define MDIO_LSR_PPHYXS_POS		4
#define MDIO_LSR_PPHYXS_WIDTH		1
#define MDIO_LSR_PPCS_POS		3
#define MDIO_LSR_PPCS_WIDTH		1
#define MDIO_LSR_PWIS_POS		2
#define MDIO_LSR_PWIS_WIDTH		1
#define MDIO_LSR_PPMDPMA_POS		1
#define MDIO_LSR_PPMDPMA_WIDTH		1

#define MDIO_ASR_PVSD2_POS		31
#define MDIO_ASR_PVSD2_WIDTH		1
#define MDIO_ASR_PVSD1_POS		30
#define MDIO_ASR_PVSD1_WIDTH		1
#define MDIO_ASR_PTC_POS		6
#define MDIO_ASR_PTC_WIDTH		1
#define MDIO_ASR_PDTEXS_POS		5
#define MDIO_ASR_PDTEXS_WIDTH		1
#define MDIO_ASR_PPHYXS_POS		4
#define MDIO_ASR_PPHYXS_WIDTH		1
#define MDIO_ASR_PPCS_POS		3
#define MDIO_ASR_PPCS_WIDTH		1
#define MDIO_ASR_PWIS_POS		2
#define MDIO_ASR_PWIS_WIDTH		1
#define MDIO_ASR_PPMDPMA_POS		1
#define MDIO_ASR_PPMDPMA_WIDTH		1

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
// New for GSWIP3.2
#define PHY_STAT_TXACT_POS		9
#define PHY_STAT_TXACT_WIDTH		1
#define PHY_STAT_RXACT_POS		10
#define PHY_STAT_RXACT_WIDTH		1
// =======
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


#define GSWIP_CFG			0x0000
#define MACSEC_EN			0x0008
#define GSWIPSS_IER0			0x0010
#define GSWIPSS_ISR0			0x0014
#define GSWIPSS_IER1			0x0018
#define GSWIPSS_ISR1			0x001C
#define NCO_LSB				0x0040
#define NCO_MSB				0x0044
#define CFG0_1588			0x0050
#define CFG1_1588			0x0054
#define NCO1_LSB			0x0060
#define NCO1_MSB			0x0064
#define NCO2_LSB			0x0068
#define NCO2_MSB			0x006C
#define NCO3_LSB			0x0070
#define NCO3_MSB			0x0074
#define NCO4_LSB			0x0078
#define NCO4_MSB			0x007C

#define GSWIP_CFG_CORE_SE_POS		15
#define GSWIP_CFG_CORE_SE_WIDTH		1
#define GSWIP_CFG_CLK_MD_POS		2
#define GSWIP_CFG_CLK_MD_WIDTH		2
#define GSWIP_CFG_SS_HWRES_POS		1
#define GSWIP_CFG_SS_HWRES_WIDTH	1

#define MACSEC_EN_SEL_POS		0
#define MACSEC_EN_SEL_WIDTH		4
#define MACSEC_EN_RES_POS		15
#define MACSEC_EN_RES_WIDTH		1

#define GSWIPSS_IER0_XGMAC2_POS		2
#define GSWIPSS_IER0_XGMAC2_WIDTH	1
#define GSWIPSS_IER0_XGMAC3_POS		3
#define GSWIPSS_IER0_XGMAC3_WIDTH	1
#define GSWIPSS_IER0_XGMAC4_POS		4
#define GSWIPSS_IER0_XGMAC4_WIDTH	1
#define GSWIPSS_IER0_XGMAC5_POS		5
#define GSWIPSS_IER0_XGMAC5_WIDTH	1
#define GSWIPSS_IER0_XGMAC6_POS		6
#define GSWIPSS_IER0_XGMAC6_WIDTH	1
#define GSWIPSS_IER0_XGMAC7_POS		7
#define GSWIPSS_IER0_XGMAC7_WIDTH	1
#define GSWIPSS_IER0_XGMAC8_POS		8
#define GSWIPSS_IER0_XGMAC8_WIDTH	1
#define GSWIPSS_IER0_XGMAC9_POS		9
#define GSWIPSS_IER0_XGMAC9_WIDTH	1
#define GSWIPSS_IER0_XGMAC10_POS	10
#define GSWIPSS_IER0_XGMAC10_WIDTH	1
#define GSWIPSS_IER0_XGMAC11_POS	11
#define GSWIPSS_IER0_XGMAC11_WIDTH	1


#define GSWIPSS_ISR0_XGMAC2_POS		2
#define GSWIPSS_ISR0_XGMAC2_WIDTH	1
#define GSWIPSS_ISR0_XGMAC3_POS		3
#define GSWIPSS_ISR0_XGMAC3_WIDTH	1
#define GSWIPSS_ISR0_XGMAC4_POS		4
#define GSWIPSS_ISR0_XGMAC4_WIDTH	1
#define GSWIPSS_ISR0_XGMAC5_POS		5
#define GSWIPSS_ISR0_XGMAC5_WIDTH	1
#define GSWIPSS_ISR0_XGMAC6_POS		6
#define GSWIPSS_ISR0_XGMAC6_WIDTH	1
#define GSWIPSS_ISR0_XGMAC7_POS		7
#define GSWIPSS_ISR0_XGMAC7_WIDTH	1
#define GSWIPSS_ISR0_XGMAC8_POS		8
#define GSWIPSS_ISR0_XGMAC8_WIDTH	1
#define GSWIPSS_ISR0_XGMAC9_POS		9
#define GSWIPSS_ISR0_XGMAC9_WIDTH	1
#define GSWIPSS_ISR0_XGMAC10_POS	10
#define GSWIPSS_ISR0_XGMAC10_WIDTH	1
#define GSWIPSS_ISR0_XGMAC11_POS	11
#define GSWIPSS_ISR0_XGMAC11_WIDTH	1



#define GSWIPSS_IER1_LINK2_POS		2
#define GSWIPSS_IER1_LINK2_WIDTH	1
#define GSWIPSS_IER1_LINK3_POS		3
#define GSWIPSS_IER1_LINK3_WIDTH	1
#define GSWIPSS_IER1_LINK4_POS		4
#define GSWIPSS_IER1_LINK4_WIDTH	1
#define GSWIPSS_IER1_LINK5_POS		5
#define GSWIPSS_IER1_LINK5_WIDTH	1
#define GSWIPSS_IER1_LINK6_POS		6
#define GSWIPSS_IER1_LINK6_WIDTH	1
#define GSWIPSS_IER1_LINK7_POS		7
#define GSWIPSS_IER1_LINK7_WIDTH	1
#define GSWIPSS_IER1_LINK8_POS		8
#define GSWIPSS_IER1_LINK8_WIDTH	1
#define GSWIPSS_IER1_LINK9_POS		9
#define GSWIPSS_IER1_LINK9_WIDTH	1
#define GSWIPSS_IER1_LINK10_POS		10
#define GSWIPSS_IER1_LINK10_WIDTH	1
#define GSWIPSS_IER1_LINK11_POS		11
#define GSWIPSS_IER1_LINK11_WIDTH	1


#define GSWIPSS_ISR1_LINK2_POS		2
#define GSWIPSS_ISR1_LINK2_WIDTH	1
#define GSWIPSS_ISR1_LINK3_POS		3
#define GSWIPSS_ISR1_LINK3_WIDTH	1
#define GSWIPSS_ISR1_LINK4_POS		4
#define GSWIPSS_ISR1_LINK4_WIDTH	1
#define GSWIPSS_ISR1_LINK5_POS		5
#define GSWIPSS_ISR1_LINK5_WIDTH	1
#define GSWIPSS_ISR1_LINK6_POS		6
#define GSWIPSS_ISR1_LINK6_WIDTH	1
#define GSWIPSS_ISR1_LINK7_POS		7
#define GSWIPSS_ISR1_LINK7_WIDTH	1
#define GSWIPSS_ISR1_LINK8_POS		8
#define GSWIPSS_ISR1_LINK8_WIDTH	1
#define GSWIPSS_ISR1_LINK9_POS		9
#define GSWIPSS_ISR1_LINK9_WIDTH	1
#define GSWIPSS_ISR1_LINK10_POS		10
#define GSWIPSS_ISR1_LINK10_WIDTH	1
#define GSWIPSS_ISR1_LINK11_POS		11
#define GSWIPSS_ISR1_LINK11_WIDTH	1


#define CFG0_1588_REFTIME_POS		0
#define CFG0_1588_REFTIME_WIDTH		3
#define CFG0_1588_DIGTIME_POS		4
#define CFG0_1588_DIGTIME_WIDTH		3
#define CFG0_1588_BINTIME_POS		8
#define CFG0_1588_BINTIME_WIDTH		3
#define CFG0_1588_PPSSEL_POS		12
#define CFG0_1588_PPSSEL_WIDTH		3

#define CFG1_1588_SWTRIG_POS		7
#define CFG1_1588_SWTRIG_WIDTH		1
#define CFG1_1588_TRIG0SEL_POS		8
#define CFG1_1588_TRIG0SEL_WIDTH	4
#define CFG1_1588_TRIG1SEL_POS		12
#define CFG1_1588_TRIG1SEL_WIDTH	4



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


#define MASK(reg, field)		(1 << reg##_##field##_POS)
#define MASK_N_BITS(reg, field)		((1 << (reg##_##field##_WIDTH)) - 1)

#define GET_N_BITS(reg, pos, n) \
	(((reg) >> (pos)) & ((0x1 << (n)) - 1))

#define SET_N_BITS(reg, pos, n, val)                            \
	do {                                                    \
		(reg) &= ~(((0x1 << (n)) - 1) << (pos));               \
		(reg) |= (((val) & ((0x1 << (n)) - 1)) << (pos));     \
	} while (0)

#define mac_get_val(var, reg, field)				\
	GET_N_BITS((var),				\
		   reg##_##field##_POS, 		\
		   reg##_##field##_WIDTH)

#define mac_set_val(var, reg, field, val)			\
	SET_N_BITS((var),				\
		   reg##_##field##_POS, 		\
		   reg##_##field##_WIDTH, (val))

#endif
