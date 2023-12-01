// SPDX-License-Identifier: GPL-2.0
/* FILE NAME:  en8801s.h
 * PURPOSE:
 *      Define EN8801S driver function
 *
 * NOTES:
 *
 */

#ifndef __EN8801S_H
#define __EN8801S_H

/* NAMING DECLARATIONS
 */
#define EN8801S_DRIVER_VERSION  "1.1.0"

#define PHY_ADDRESS_RANGE       0x18
#define EN8801S_PBUS_DEFAULT_ID 0x1e
#define EN8801S_MDIO_PHY_ID     0x18       /* Range PHY_ADDRESS_RANGE .. 0x1e */
#define EN8801S_PBUS_PHY_ID     (EN8801S_MDIO_PHY_ID + 1)

#define EN8801S_RG_ETHER_PHY_OUI 0x19a4
#define EN8801S_RG_SMI_ADDR      0x19a8
#define EN8801S_RG_BUCK_CTL      0x1a20
#define EN8801S_RG_LTR_CTL      0x0cf8

#define EN8801S_PBUS_OUI        0x17a5
#define EN8801S_PHY_ID1         0x03a2
#define EN8801S_PHY_ID2         0x9461
#define EN8801S_PHY_ID          (unsigned long)((EN8801S_PHY_ID1 << 16) | EN8801S_PHY_ID2)

#define DEV1E_REG013_VALUE      0
#define DEV1E_REG19E_VALUE      0xC2
#define DEV1E_REG324_VALUE      0x200

#define TRUE                    1
#define FALSE                   0
#define LINK_UP                 1
#define LINK_DOWN               0

//#define TEST_BOARD
#if defined(TEST_BOARD)
/* SFP sample for verification */
#define EN8801S_TX_POLARITY     1
#define EN8801S_RX_POLARITY     0
#else
/* chip on board */
#define EN8801S_TX_POLARITY     0
#define EN8801S_RX_POLARITY     1       /* The pin default assignment is set to 1 */
#endif

#define MAX_RETRY               5
#define MAX_OUI_CHECK           2
/* CL45 MDIO control */
#define MII_MMD_ACC_CTL_REG     0x0d
#define MII_MMD_ADDR_DATA_REG   0x0e
#define MMD_OP_MODE_DATA        BIT(14)

#define MAX_TRG_COUNTER         5

/* CL22 Reg Support Page Select */
#define RgAddr_Reg1Fh        0x1f
#define CL22_Page_Reg        0x0000
#define CL22_Page_ExtReg     0x0001
#define CL22_Page_MiscReg    0x0002
#define CL22_Page_LpiReg     0x0003
#define CL22_Page_tReg       0x02A3
#define CL22_Page_TrReg      0x52B5

/* CL45 Reg Support DEVID */
#define DEVID_03             0x03
#define DEVID_07             0x07
#define DEVID_1E             0x1E
#define DEVID_1F             0x1F

/* TokenRing Reg Access */
#define TrReg_PKT_XMT_STA    0x8000
#define TrReg_WR             0x8000
#define TrReg_RD             0xA000

#define RgAddr_LpiReg1Ch     0x1c
#define RgAddr_PMA_01h       0x0f82
#define RgAddr_PMA_18h       0x0fb0
#define RgAddr_DSPF_03h      0x1686
#define RgAddr_DSPF_06h      0x168c
#define RgAddr_DSPF_0Ch      0x1698
#define RgAddr_DSPF_0Dh      0x169a
#define RgAddr_DSPF_0Fh      0x169e
#define RgAddr_DSPF_10h      0x16a0
#define RgAddr_DSPF_11h      0x16a2
#define RgAddr_DSPF_14h      0x16a8
#define RgAddr_DSPF_1Ch      0x16b8
#define RgAddr_TR_26h        0x0ecc
#define RgAddr_R1000DEC_15h  0x03aa

/* DATA TYPE DECLARATIONS
 */
typedef struct
{
    u16 DATA_Lo;
    u16 DATA_Hi;
}TR_DATA_T;

typedef union
{
    struct
    {
        /* b[15:00] */
        u16 smi_deton_wt                             : 3;
        u16 smi_det_mdi_inv                          : 1;
        u16 smi_detoff_wt                            : 3;
        u16 smi_sigdet_debouncing_en                 : 1;
        u16 smi_deton_th                             : 6;
        u16 rsv_14                                   : 2;
    } DataBitField;
    u16 DATA;
} gephy_all_REG_LpiReg1Ch, *Pgephy_all_REG_LpiReg1Ch;

typedef union
{
    struct
    {
        /* b[15:00] */
        u16 rg_smi_detcnt_max                        : 6;
        u16 rsv_6                                    : 2;
        u16 rg_smi_det_max_en                        : 1;
        u16 smi_det_deglitch_off                     : 1;
        u16 rsv_10                                   : 6;
    } DataBitField;
    u16 DATA;
} gephy_all_REG_dev1Eh_reg324h, *Pgephy_all_REG_dev1Eh_reg324h;

typedef union
{
    struct
    {
        /* b[15:00] */
        u16 da_tx_i2mpb_a_tbt                        : 6;
        u16 rsv_6                                    : 4;
        u16 da_tx_i2mpb_a_gbe                        : 6;
    } DataBitField;
    u16 DATA;
} gephy_all_REG_dev1Eh_reg012h, *Pgephy_all_REG_dev1Eh_reg012h;

typedef union
{
    struct
    {
        /* b[15:00] */
        u16 da_tx_i2mpb_b_tbt                        : 6;
        u16 rsv_6                                    : 2;
        u16 da_tx_i2mpb_b_gbe                        : 6;
        u16 rsv_14                                   : 2;
    } DataBitField;
    u16 DATA;
} gephy_all_REG_dev1Eh_reg017h, *Pgephy_all_REG_dev1Eh_reg017h;

#endif /* End of __EN8801S_H */
