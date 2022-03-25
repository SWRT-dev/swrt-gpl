/*
 * Copyright 2010-2018 MediaTek, Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * Version 2 as published by the Free Software Foundation.
 */

#ifndef _LEOPARD_H
#define _LEOPARD_H

#include <linux/compiler.h>


/*
 * Constant Definitions
 */
#define IO_PHYS             (0x10000000)
#define IO_SIZE             (0x02000000)

/*
 * Module register bases
 */
#define INFRACFG_AO_BASE    (IO_PHYS + 0x0000)
#define CONFIG_BASE         (IO_PHYS + 0x1000)
#define IOCFG_L_BASE        (IO_PHYS + 0x2000)
#define IOCFG_B_BASE        (IO_PHYS + 0x2400)
#define IOCFG_R_BASE        (IO_PHYS + 0x2800)
#define IOCFG_T_BASE        (IO_PHYS + 0x2C00)
#define PERICFG_BASE        (IO_PHYS + 0x2000)
#define GPT_BASE            (IO_PHYS + 0x4000)
#define SPM_BASE            (IO_PHYS + 0x6000)
#define DEVICE_APC_BASE     (IO_PHYS + 0x7000)

#define GPIO_BASE           (IO_PHYS + 0x00210000)
#define RGU_BASE            (IO_PHYS + 0x00212000)
#define DDRPHY_BASE         (IO_PHYS + 0x00213000)
#define DRAMC0_BASE         (IO_PHYS + 0x00214000)

#define MCUSYS_CFGREG_BASE  (IO_PHYS + 0x00200000)
#define CA7MCUCFG_BASE      (IO_PHYS + 0x00200100)
#define SRAMROM_BASE        (IO_PHYS + 0x00202000)
#define EMI_BASE            (IO_PHYS + 0x00203000)
#define EFUSE_BASE          (IO_PHYS + 0x00206000)
#define APMIXED_BASE        (IO_PHYS + 0x00209000)
#define CRYPTO_ENG_BASE     (IO_PHYS + 0x0020A000)
#define DRAMC_NAO_BASE      (IO_PHYS + 0x0020E000)
#define CKSYS_BASE          (IO_PHYS + 0x00210000)

#define UART0_BASE          (IO_PHYS + 0x01002000)
#define UART1_BASE          (IO_PHYS + 0x01003000)
#define UART2_BASE          (IO_PHYS + 0x01004000)
#define UART3_BASE          (IO_PHYS + 0x01005000)
#define I2C0_BASE           (IO_PHYS + 0x01007000)
#define I2C1_BASE           (IO_PHYS + 0x01008000)
#define I2C2_BASE           (IO_PHYS + 0x01009000)
#define SPI_BASE            (IO_PHYS + 0x0100A000)
#define NFI_BASE            (IO_PHYS + 0x0100D000)
#define NFIECC_BASE         (IO_PHYS + 0x0100E000)

/*
 * AP HW code offset
 */
#define VER_BASE            (0x08000000)
#define APHW_CODE           (VER_BASE + 0x00)
#define APHW_SUBCODE        (VER_BASE + 0x04)
#define APHW_VER            (VER_BASE + 0x08)
#define APSW_VER            (VER_BASE + 0x0C)

/*
 * arm GIC register maps
 */
#define GIC_CPU_BASE        (0x10320000)
#define GIC_CPU_CTRL                0x00
#define GIC_CPU_PRIMASK             0x04

#define GIC_DIST_BASE       (0x10310000)
#define GIC_DIST_CTRL               0x000
#define GIC_DIST_CTR                0x004
#define GIC_DIST_SECURITY_SET       0x080
#define GIC_DIST_ENABLE_SET         0x100
#define GIC_DIST_ENABLE_CLEAR       0x180
#define GIC_DIST_PENDING_SET        0x200
#define GIC_DIST_PENDING_CLEAR      0x280
#define GIC_DIST_ACTIVE_BIT         0x300

/*
 * ARM and MCUSYS register offset
 */
#define MP0_AXI_CONFIG              (MCUSYS_CFGREG_BASE + 0x002C)
#define MP0_ACINACTM                (1U << 4)
#define ACLKEN_DIV                  (MCUSYS_CFGREG_BASE + 0x0640)
#define MCU_BUS_MUX                 (MCUSYS_CFGREG_BASE + 0x07C0)
#define L2C_CFG_MP0                 (MCUSYS_CFGREG_BASE + 0x07F0)
#define SEC_POL_CTL_EN0             (MCUSYS_CFGREG_BASE + 0x0A00)

/* register address - CCI400 */
#define CCI400_BASE                 (0x10390000)
#define CCI400_STATUS               (CCI400_BASE + 0x000C)
#define CHANGE_PENDING              (1U << 0)
/* cluster 0 - Slave interface 4 registers */
#define CCI400_SI4_BASE             (CCI400_BASE + 0x5000)
#define CCI400_SI4_SNOOP_CONTROL    (CCI400_SI4_BASE)
#define DVM_MSG_REQ                 (1U << 1)
#define SNOOP_REQ                   (1U << 0)

/*
 * HIF TOP address
 */
#define HIF_TOP_CSR_BASE	    0x1af01000
#define PHY_RST_CTL		    HIF_TOP_CSR_BASE
#define AXI_SYS_SLICE_CTL	    (HIF_TOP_CSR_BASE + 0x04)

/*=======================================================================*/
/* MTK Register Manipulations                                                 */
/*=======================================================================*/

#define READ_REGISTER_UINT32(reg) \
    (*(volatile u32 * const)(reg))

#define WRITE_REGISTER_UINT32(reg, val) \
    (*(volatile u32 * const)(reg)) = (val)

#define READ_REGISTER_UINT16(reg) \
    (*(volatile u16 * const)(reg))

#define WRITE_REGISTER_UINT16(reg, val) \
    (*(volatile u16 * const)(reg)) = (val)

#define READ_REGISTER_UINT8(reg) \
    (*(volatile u8 * const)(reg))

#define WRITE_REGISTER_UINT8(reg, val) \
    (*(volatile u8 * const)(reg)) = (val)

#define INREG8(x)           READ_REGISTER_UINT8((u8*)((void*)(x)))
#define OUTREG8(x, y)       WRITE_REGISTER_UINT8((u8*)((void*)(x)), (u8)(y))
#define SETREG8(x, y)       OUTREG8(x, INREG8(x)|(y))
#define CLRREG8(x, y)       OUTREG8(x, INREG8(x)&~(y))
#define MASKREG8(x, y, z)   OUTREG8(x, (INREG8(x)&~(y))|(z))

#define INREG16(x)          READ_REGISTER_UINT16((u16*)((void*)(x)))
#define OUTREG16(x, y)      WRITE_REGISTER_UINT16((u16*)((void*)(x)),(u16)(y))
#define SETREG16(x, y)      OUTREG16(x, INREG16(x)|(y))
#define CLRREG16(x, y)      OUTREG16(x, INREG16(x)&~(y))
#define MASKREG16(x, y, z)  OUTREG16(x, (INREG16(x)&~(y))|(z))

#define INREG32(x)          READ_REGISTER_UINT32((u32*)((void*)(x)))
#define OUTREG32(x, y)      WRITE_REGISTER_UINT32((u32*)((void*)(x)), (u32)(y))
#define SETREG32(x, y)      OUTREG32(x, INREG32(x)|(y))
#define CLRREG32(x, y)      OUTREG32(x, INREG32(x)&~(y))
#define MASKREG32(x, y, z)  OUTREG32(x, (INREG32(x)&~(y))|(z))

#define DRV_Reg8(addr)              INREG8(addr)
#define DRV_WriteReg8(addr, data)   OUTREG8(addr, data)
#define DRV_SetReg8(addr, data)     SETREG8(addr, data)
#define DRV_ClrReg8(addr, data)     CLRREG8(addr, data)

#define DRV_Reg16(addr)             INREG16(addr)
#define DRV_WriteReg16(addr, data)  OUTREG16(addr, data)
#define DRV_SetReg16(addr, data)    SETREG16(addr, data)
#define DRV_ClrReg16(addr, data)    CLRREG16(addr, data)

#define DRV_Reg32(addr)             INREG32(addr)
#define DRV_WriteReg32(addr, data)  OUTREG32(addr, data)
#define DRV_SetReg32(addr, data)    SETREG32(addr, data)
#define DRV_ClrReg32(addr, data)    CLRREG32(addr, data)

// !!! DEPRECATED, WILL BE REMOVED LATER !!!
#define DRV_Reg(addr)               DRV_Reg16(addr)
#define DRV_WriteReg(addr, data)    DRV_WriteReg16(addr, data)
#define DRV_SetReg(addr, data)      DRV_SetReg16(addr, data)
#define DRV_ClrReg(addr, data)      DRV_ClrReg16(addr, data)

#define READ_REG(REG)               DRV_Reg32(REG)
#define WRITE_REG(VAL, REG)         DRV_WriteReg32(REG, VAL)

#endif
