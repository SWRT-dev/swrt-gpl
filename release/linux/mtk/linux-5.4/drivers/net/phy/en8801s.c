// SPDX-License-Identifier: GPL-2.0
/* FILE NAME:  airoha.c
 * PURPOSE:
 *      EN8801S phy driver for Linux
 * NOTES:
 *
 */

/* INCLUDE FILE DECLARATIONS
 */

#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/errno.h>
#include <linux/unistd.h>
#include <linux/interrupt.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/skbuff.h>
#include <linux/spinlock.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/mii.h>
#include <linux/ethtool.h>
#include <linux/phy.h>
#include <linux/delay.h>

//#include <linux/bitfield.h>
#include <linux/uaccess.h>
#include <linux/version.h>

#include "en8801s.h"

/* #define TEST_BOARD */

MODULE_DESCRIPTION("Airoha EN8801S PHY drivers");
MODULE_AUTHOR("Airoha");
MODULE_LICENSE("GPL");

static int preSpeed = 0;
/************************************************************************
*                  F U N C T I O N S
************************************************************************/
unsigned int mdiobus_write45(struct mii_bus *bus, u32 port, u32 devad, u32 reg, u16 val)
{
    mdiobus_write(bus, port, MII_MMD_ACC_CTL_REG, devad);
    mdiobus_write(bus, port, MII_MMD_ADDR_DATA_REG, reg);
    mdiobus_write(bus, port, MII_MMD_ACC_CTL_REG, MMD_OP_MODE_DATA | devad);
    mdiobus_write(bus, port, MII_MMD_ADDR_DATA_REG, val);
    return 0;
}

unsigned int mdiobus_read45(struct mii_bus *bus, u32 port, u32 devad, u32 reg, u32 *read_data)
{
    mdiobus_write(bus, port, MII_MMD_ACC_CTL_REG, devad);
    mdiobus_write(bus, port, MII_MMD_ADDR_DATA_REG, reg);
    mdiobus_write(bus, port, MII_MMD_ACC_CTL_REG, MMD_OP_MODE_DATA | devad);
    *read_data = mdiobus_read(bus, port, MII_MMD_ADDR_DATA_REG);
    return 0;
}

/* Airoha MII read function */
unsigned int ecnt_mii_cl22_read(struct mii_bus *ebus, unsigned int phy_addr,unsigned int phy_register,unsigned int *read_data)
{
    *read_data = mdiobus_read(ebus, phy_addr, phy_register);
    return 0;
}

/* Airoha MII write function */
unsigned int ecnt_mii_cl22_write(struct mii_bus *ebus, unsigned int phy_addr, unsigned int phy_register,unsigned int write_data)
{
    mdiobus_write(ebus, phy_addr, phy_register, write_data);
    return 0;
}

/* EN8801 PBUS write function */
void En8801_PbusRegWr(struct mii_bus *ebus, unsigned long pbus_address, unsigned long pbus_data)
{
    ecnt_mii_cl22_write(ebus, EN8801S_PBUS_PHY_ID, 0x1F, (unsigned int)(pbus_address >> 6));
    ecnt_mii_cl22_write(ebus, EN8801S_PBUS_PHY_ID, (unsigned int)((pbus_address >> 2) & 0xf), (unsigned int)(pbus_data & 0xFFFF));
    ecnt_mii_cl22_write(ebus, EN8801S_PBUS_PHY_ID, 0x10, (unsigned int)(pbus_data >> 16));
    return;
}

/* EN8801 PBUS read function */
unsigned long En8801_PbusRegRd(struct mii_bus *ebus, unsigned long pbus_address)
{
    unsigned long pbus_data;
    unsigned int pbus_data_low, pbus_data_high;

    ecnt_mii_cl22_write(ebus, EN8801S_PBUS_PHY_ID, 0x1F, (unsigned int)(pbus_address >> 6));
    ecnt_mii_cl22_read(ebus, EN8801S_PBUS_PHY_ID, (unsigned int)((pbus_address >> 2) & 0xf), &pbus_data_low);
    ecnt_mii_cl22_read(ebus, EN8801S_PBUS_PHY_ID, 0x10, &pbus_data_high);
    pbus_data = (pbus_data_high << 16) + pbus_data_low;
    return pbus_data;
}

/* Use default PBUS_PHY_ID */
/* EN8801 PBUS write function */
void En8801_varPbusRegWr(struct mii_bus *ebus, unsigned long pbus_id,unsigned long pbus_address, unsigned long pbus_data)
{
    ecnt_mii_cl22_write(ebus, pbus_id, 0x1F, (unsigned int)(pbus_address >> 6));
    ecnt_mii_cl22_write(ebus, pbus_id, (unsigned int)((pbus_address >> 2) & 0xf), (unsigned int)(pbus_data & 0xFFFF));
    ecnt_mii_cl22_write(ebus, pbus_id, 0x10, (unsigned int)(pbus_data >> 16));
    return;
}

/* EN8801 PBUS read function */
unsigned long En8801_varPbusRegRd(struct mii_bus *ebus, unsigned long pbus_id, unsigned long pbus_address)
{
    unsigned long pbus_data;
    unsigned int pbus_data_low, pbus_data_high;

    ecnt_mii_cl22_write(ebus, pbus_id, 0x1F, (unsigned int)(pbus_address >> 6));
    ecnt_mii_cl22_read(ebus,  pbus_id, (unsigned int)((pbus_address >> 2) & 0xf), &pbus_data_low);
    ecnt_mii_cl22_read(ebus,  pbus_id, 0x10, &pbus_data_high);
    pbus_data = (pbus_data_high << 16) + pbus_data_low;
    return pbus_data;
}

/* EN8801 Token Ring Write function */
void En8801_TR_RegWr(struct mii_bus *ebus, unsigned long tr_address, unsigned long tr_data)
{
    ecnt_mii_cl22_write(ebus, EN8801S_MDIO_PHY_ID, 0x1F, 0x52b5);       /* page select */
    ecnt_mii_cl22_write(ebus, EN8801S_MDIO_PHY_ID, 0x11, (unsigned int)(tr_data & 0xffff));
    ecnt_mii_cl22_write(ebus, EN8801S_MDIO_PHY_ID, 0x12, (unsigned int)(tr_data >> 16));
    ecnt_mii_cl22_write(ebus, EN8801S_MDIO_PHY_ID, 0x10, (unsigned int)(tr_address | TrReg_WR));
    ecnt_mii_cl22_write(ebus, EN8801S_MDIO_PHY_ID, 0x1F, 0x0);          /* page resetore */
    return;
}

/* EN8801 Token Ring Read function */
unsigned long En8801_TR_RegRd(struct mii_bus *ebus, unsigned long tr_address)
{
    unsigned long tr_data;
    unsigned int tr_data_low, tr_data_high;

    ecnt_mii_cl22_write(ebus, EN8801S_MDIO_PHY_ID, 0x1F, 0x52b5);       /* page select */
    ecnt_mii_cl22_write(ebus, EN8801S_MDIO_PHY_ID, 0x10, (unsigned int)(tr_address | TrReg_RD));
    ecnt_mii_cl22_read(ebus, EN8801S_MDIO_PHY_ID, 0x11, &tr_data_low);
    ecnt_mii_cl22_read(ebus, EN8801S_MDIO_PHY_ID, 0x12, &tr_data_high);
    ecnt_mii_cl22_write(ebus, EN8801S_MDIO_PHY_ID, 0x1F, 0x0);          /* page resetore */
    tr_data = (tr_data_high << 16) + tr_data_low;
    return tr_data;
}

static int en8801s_config_init(struct phy_device *phydev)
{
    gephy_all_REG_LpiReg1Ch      GPHY_RG_LPI_1C;
    gephy_all_REG_dev1Eh_reg324h GPHY_RG_1E_324;
    gephy_all_REG_dev1Eh_reg012h GPHY_RG_1E_012;
    gephy_all_REG_dev1Eh_reg017h GPHY_RG_1E_017;
    unsigned long pbus_data;
    unsigned int pbusAddress;
    u32 reg_value;
    int retry;
    struct mii_bus *mbus;

    #if (LINUX_VERSION_CODE < KERNEL_VERSION(4, 5, 0))
    mbus = phydev->bus;
    #else
    mbus = phydev->mdio.bus;
    #endif

    pbusAddress = EN8801S_PBUS_DEFAULT_ID;
    retry = MAX_OUI_CHECK;
    while(1)
    {
        pbus_data = En8801_varPbusRegRd(mbus, pbusAddress, EN8801S_RG_ETHER_PHY_OUI);      /* PHY OUI */
        if(EN8801S_PBUS_OUI == pbus_data)
        {
            pbus_data = En8801_varPbusRegRd(mbus, pbusAddress, EN8801S_RG_SMI_ADDR);       /* SMI ADDR */
            pbus_data = (pbus_data & 0xffff0000) | (unsigned long)(EN8801S_PBUS_PHY_ID << 8) | (unsigned long)(EN8801S_MDIO_PHY_ID );
            printk("[Airoha] EN8801S SMI_ADDR=%lx (renew)\n", pbus_data);
            En8801_varPbusRegWr(mbus, pbusAddress, EN8801S_RG_SMI_ADDR, pbus_data);
            En8801_varPbusRegWr(mbus, EN8801S_PBUS_PHY_ID, EN8801S_RG_BUCK_CTL, 0x03);
            mdelay(10);
            break;
        }
        else
        {
            pbusAddress = EN8801S_PBUS_PHY_ID;
        }
        retry --;
        if (0 == retry)
        {
            printk("[Airoha] EN8801S probe fail !\n");
            return 0;
        }
    }

    reg_value = (En8801_PbusRegRd(mbus, EN8801S_RG_LTR_CTL) & 0xfffffffc) | 0x10 | (EN8801S_RX_POLARITY << 1) | EN8801S_TX_POLARITY;
    En8801_PbusRegWr(mbus, 0xcf8, reg_value);
    mdelay(10);
    reg_value &= 0xffffffef;
    En8801_PbusRegWr(mbus, 0xcf8, reg_value);

    retry = MAX_RETRY;
    while (1)
    {
        mdelay(10);
        reg_value = phy_read(phydev, MII_PHYSID2);
        if (reg_value == EN8801S_PHY_ID2)
        {
            break;    /* wait GPHY ready */
        }
        retry--;
        if (0 == retry)
        {
            printk("[Airoha] EN8801S initialize fail !\n");
            return 0;
        }
    }
    /* Software Reset PHY */
    reg_value = phy_read(phydev, MII_BMCR);
    reg_value |= BMCR_RESET;
    phy_write(phydev, MII_BMCR, reg_value);
    retry = MAX_RETRY;
    do
    {
        mdelay(10);
        reg_value = phy_read(phydev, MII_BMCR);
        retry--;
        if (0 == retry)
        {
            printk("[Airoha] EN8801S reset fail !\n");
            return 0;
        }
    } while (reg_value & BMCR_RESET);

    En8801_PbusRegWr(mbus, 0x0600, 0x0c000c00);
    En8801_PbusRegWr(mbus, 0x10, 0xD801);
    En8801_PbusRegWr(mbus, 0x0,  0x9140);

    En8801_PbusRegWr(mbus, 0x0A14, 0x0003);
    En8801_PbusRegWr(mbus, 0x0600, 0x0c000c00);
    /* Set FCM control */
    En8801_PbusRegWr(mbus, 0x1404, 0x004b);
    En8801_PbusRegWr(mbus, 0x140c, 0x0007);
    /* Set GPHY Perfomance*/
    /* Token Ring */
    En8801_TR_RegWr(mbus, RgAddr_PMA_01h,     0x6FB90A);
    En8801_TR_RegWr(mbus, RgAddr_PMA_18h,     0x0E2F00);
    En8801_TR_RegWr(mbus, RgAddr_DSPF_06h,    0x2EBAEF);
    En8801_TR_RegWr(mbus, RgAddr_DSPF_11h,    0x040001);
    En8801_TR_RegWr(mbus, RgAddr_DSPF_03h,    0x000004);
    En8801_TR_RegWr(mbus, RgAddr_DSPF_1Ch,    0x003210);
    En8801_TR_RegWr(mbus, RgAddr_DSPF_14h,    0x00024A);
    En8801_TR_RegWr(mbus, RgAddr_DSPF_0Ch,    0x00704D);
    En8801_TR_RegWr(mbus, RgAddr_DSPF_0Dh,    0x02314F);
    En8801_TR_RegWr(mbus, RgAddr_DSPF_10h,    0x005010);
    En8801_TR_RegWr(mbus, RgAddr_DSPF_0Fh,    0x003028);
    En8801_TR_RegWr(mbus, RgAddr_TR_26h,      0x444444);
    En8801_TR_RegWr(mbus, RgAddr_R1000DEC_15h,0x0055A0);
    /* CL22 & CL45 */
    phy_write(phydev, 0x1f, 0x03);
    GPHY_RG_LPI_1C.DATA = phy_read(phydev, RgAddr_LpiReg1Ch);
    GPHY_RG_LPI_1C.DataBitField.smi_deton_th = 0x0C;
    phy_write(phydev, RgAddr_LpiReg1Ch, GPHY_RG_LPI_1C.DATA);
    phy_write(phydev, 0x1f, 0x0);
    mdiobus_write45(mbus, EN8801S_MDIO_PHY_ID, 0x1E, 0x122, 0xffff);
    mdiobus_write45(mbus, EN8801S_MDIO_PHY_ID, 0x1E, 0x234, 0x0180);
    mdiobus_write45(mbus, EN8801S_MDIO_PHY_ID, 0x1E, 0x238, 0x0120);
    mdiobus_write45(mbus, EN8801S_MDIO_PHY_ID, 0x1E, 0x120, 0x9014);
    mdiobus_write45(mbus, EN8801S_MDIO_PHY_ID, 0x1E, 0x239, 0x0117);
    mdiobus_write45(mbus, EN8801S_MDIO_PHY_ID, 0x1E, 0x14A, 0xEE20);
    mdiobus_write45(mbus, EN8801S_MDIO_PHY_ID, 0x1E, 0x19B, 0x0111);
    mdiobus_write45(mbus, EN8801S_MDIO_PHY_ID, 0x1F, 0x268, 0x07F4);

    mdiobus_read45(mbus, EN8801S_MDIO_PHY_ID, 0x1E, 0x324, &reg_value);
    GPHY_RG_1E_324.DATA=(u16)reg_value;
    GPHY_RG_1E_324.DataBitField.smi_det_deglitch_off = 0;
    mdiobus_write45(mbus, EN8801S_MDIO_PHY_ID, 0x1E, 0x324, (u32)GPHY_RG_1E_324.DATA);
    mdiobus_write45(mbus, EN8801S_MDIO_PHY_ID, 0x1E, 0x19E, 0xC2);
    mdiobus_write45(mbus, EN8801S_MDIO_PHY_ID, 0x1E, 0x013, 0x0);

    /* EFUSE */
    En8801_PbusRegWr(mbus, 0x1C08, 0x40000040);
    retry = MAX_RETRY;
    while (0 != retry)
    {
        mdelay(1);
        reg_value = En8801_PbusRegRd(mbus, 0x1C08);
        if ((reg_value & (1 << 30)) == 0)
        {
            break;
        }
        retry--;
    }
    reg_value = En8801_PbusRegRd(mbus, 0x1C38);          /* RAW#2 */
    GPHY_RG_1E_012.DataBitField.da_tx_i2mpb_a_tbt = reg_value & 0x03f;
    mdiobus_write45(mbus, EN8801S_MDIO_PHY_ID, 0x1E, 0x12, (u32)GPHY_RG_1E_012.DATA);
    GPHY_RG_1E_017.DataBitField.da_tx_i2mpb_b_tbt=(reg_value >> 8) & 0x03f;
    mdiobus_write45(mbus, EN8801S_MDIO_PHY_ID, 0x1E, 0x12, (u32)GPHY_RG_1E_017.DATA);

    En8801_PbusRegWr(mbus, 0x1C08, 0x40400040);
    retry = MAX_RETRY;
    while (0 != retry)
    {
        mdelay(1);
        reg_value = En8801_PbusRegRd(mbus, 0x1C08);
        if ((reg_value & (1 << 30)) == 0)
        {
            break;
        }
        retry--;
    }
    reg_value = En8801_PbusRegRd(mbus, 0x1C30);          /* RAW#16 */
    GPHY_RG_1E_324.DataBitField.smi_det_deglitch_off = (reg_value >> 12) & 0x01;
    mdiobus_write45(mbus, EN8801S_MDIO_PHY_ID, 0x1E, 0x324, (u32)GPHY_RG_1E_324.DATA);

    printk("[Airoha] EN8801S initialize OK ! (1.0.5)\n");
    return 0;
}

static int en8801s_read_status(struct phy_device *phydev)
{
    int ret;
    struct mii_bus *mbus;

    #if (LINUX_VERSION_CODE < KERNEL_VERSION(4, 5, 0))
    mbus = phydev->bus;
    #else
    mbus = phydev->mdio.bus;
    #endif

    ret = genphy_read_status(phydev);
    if (LINK_DOWN == phydev->link) preSpeed =0;

    if ((preSpeed != phydev->speed) && (LINK_UP == phydev->link))
    {
        preSpeed = phydev->speed;
        En8801_PbusRegWr(mbus, 0x0600, 0x0c000c00);
        if (SPEED_1000 == preSpeed)
        {
            En8801_PbusRegWr(mbus, 0x10, 0xD801);
            En8801_PbusRegWr(mbus, 0x0,  0x9140);

            En8801_PbusRegWr(mbus, 0x0A14, 0x0003);
            En8801_PbusRegWr(mbus, 0x0600, 0x0c000c00);
            mdelay(2);      /* delay 2 ms */
            En8801_PbusRegWr(mbus, 0x1404, 0x004b);
            En8801_PbusRegWr(mbus, 0x140c, 0x0007);
        }
        else if (SPEED_100 == preSpeed)
        {
            En8801_PbusRegWr(mbus, 0x10, 0xD401);
            En8801_PbusRegWr(mbus, 0x0,  0x9140);

            En8801_PbusRegWr(mbus, 0x0A14, 0x0007);
            En8801_PbusRegWr(mbus, 0x0600, 0x0c11);
            mdelay(2);      /* delay 2 ms */
            En8801_PbusRegWr(mbus, 0x1404, 0x0027);
            En8801_PbusRegWr(mbus, 0x140c, 0x0007);
        }
        else if (SPEED_10 == preSpeed)
        {
            En8801_PbusRegWr(mbus, 0x10, 0xD001);
            En8801_PbusRegWr(mbus, 0x0,  0x9140);

            En8801_PbusRegWr(mbus, 0x0A14, 0x000b);
            En8801_PbusRegWr(mbus, 0x0600, 0x0c11);
            mdelay(2);      /* delay 2 ms */
            En8801_PbusRegWr(mbus, 0x1404, 0x0027);
            En8801_PbusRegWr(mbus, 0x140c, 0x0007);
        }
    }
    return ret;
}

static struct phy_driver Airoha_driver[] = {
{
    .phy_id         = EN8801S_PHY_ID,
    .name           = "Airoha EN8801S",
    .phy_id_mask    = 0x0ffffff0,
    .features       = PHY_GBIT_FEATURES,
    .config_init    = en8801s_config_init,
    .config_aneg    = genphy_config_aneg,
    .read_status    = en8801s_read_status,
    .suspend        = genphy_suspend,
    .resume         = genphy_resume,
} };

module_phy_driver(Airoha_driver);

static struct mdio_device_id __maybe_unused Airoha_tbl[] = {
    { EN8801S_PHY_ID, 0x0ffffff0 },
    { }
};

MODULE_DEVICE_TABLE(mdio, Airoha_tbl);
MODULE_LICENSE("GPL");
