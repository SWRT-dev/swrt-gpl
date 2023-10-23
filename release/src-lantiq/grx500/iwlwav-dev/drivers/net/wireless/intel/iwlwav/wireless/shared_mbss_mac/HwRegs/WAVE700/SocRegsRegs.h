
/***********************************************************************************
File:				SocRegsRegs.h
Module:				socRegs
SOC Revision:		
Purpose:
Description:		This File was auto generated using SOC Online

************************************************************************************/
#ifndef _SOC_REGS_REGS_H_
#define _SOC_REGS_REGS_H_

/*---------------------------------------------------------------------------------
/						Registers Addresses													 
/----------------------------------------------------------------------------------*/
#include "HwMemoryMap.h"

#define SOC_REGS_BASE_ADDRESS                             MEMORY_MAP_UNIT_59_BASE_ADDRESS
#define	REG_SOC_REGS_RESERVE_LOC0                    (SOC_REGS_BASE_ADDRESS + 0x0)
#define	REG_SOC_REGS_SOFT_RESET                      (SOC_REGS_BASE_ADDRESS + 0x4)
#define	REG_SOC_REGS_SOFT_RESET_STROBE_ENABLE        (SOC_REGS_BASE_ADDRESS + 0x8)
#define	REG_SOC_REGS_RCU_BOOT                        (SOC_REGS_BASE_ADDRESS + 0xC)
#define	REG_SOC_REGS_GPIO_PULLUP                     (SOC_REGS_BASE_ADDRESS + 0x10)
#define	REG_SOC_REGS_GPIO_PULLDN                     (SOC_REGS_BASE_ADDRESS + 0x14)
#define	REG_SOC_REGS_GPIO_SLEW_CTRL                  (SOC_REGS_BASE_ADDRESS + 0x18)
#define	REG_SOC_REGS_GPIO_DRIVE_CTRL0                (SOC_REGS_BASE_ADDRESS + 0x1C)
#define	REG_SOC_REGS_GPIO_DRIVE_CTRL1                (SOC_REGS_BASE_ADDRESS + 0x20)
#define	REG_SOC_REGS_GPIO_OPEN_DRAIN                 (SOC_REGS_BASE_ADDRESS + 0x24)
#define	REG_SOC_REGS_EEPROM_SW_ACCESS                (SOC_REGS_BASE_ADDRESS + 0x28)
#define	REG_SOC_REGS_PCIE_EP_CHIP_RST_ENA            (SOC_REGS_BASE_ADDRESS + 0x2C)
#define	REG_SOC_REGS_PCIE_PHY_DEBUG_CTL              (SOC_REGS_BASE_ADDRESS + 0x34)
#define	REG_SOC_REGS_PCIE_PHY_DEBUG_STATUS           (SOC_REGS_BASE_ADDRESS + 0x38)
#define	REG_SOC_REGS_PCIE_PHY_CONFIG_REG1            (SOC_REGS_BASE_ADDRESS + 0x3C)
#define	REG_SOC_REGS_PCIE_PHY_CONFIG_REG2            (SOC_REGS_BASE_ADDRESS + 0x40)
#define	REG_SOC_REGS_PCIE_RESP_CTL                   (SOC_REGS_BASE_ADDRESS + 0x44)
#define	REG_SOC_REGS_PCIE_INTR_VEC                   (SOC_REGS_BASE_ADDRESS + 0x48)
#define	REG_SOC_REGS_PCIE_MSI_ENABLE                 (SOC_REGS_BASE_ADDRESS + 0x4C)
#define	REG_SOC_REGS_PCIE_LEGACY_INT_ENA             (SOC_REGS_BASE_ADDRESS + 0x50)
#define	REG_SOC_REGS_PCIE_DIAG_CTRL_BUS              (SOC_REGS_BASE_ADDRESS + 0x54)
#define	REG_SOC_REGS_PCIE_PHY_CONFIG_REG3            (SOC_REGS_BASE_ADDRESS + 0x58)
#define	REG_SOC_REGS_PCIE_APPS_PM_CLK_SWITCH         (SOC_REGS_BASE_ADDRESS + 0x5C)
#define	REG_SOC_REGS_PCIE_APPS_PM_WAKEUP             (SOC_REGS_BASE_ADDRESS + 0x60)
#define	REG_SOC_REGS_PCIE0_RADM_STATUS               (SOC_REGS_BASE_ADDRESS + 0x64)
#define	REG_SOC_REGS_PCIE_LINK_STATUS                (SOC_REGS_BASE_ADDRESS + 0x68)
#define	REG_SOC_REGS_PCIE_AMBA_ORDR_MGR_WDT_INT      (SOC_REGS_BASE_ADDRESS + 0x6C)
#define	REG_SOC_REGS_EFUSE_DATA                      (SOC_REGS_BASE_ADDRESS + 0x70)
#define	REG_SOC_REGS_EFUSE_ADDRESS                   (SOC_REGS_BASE_ADDRESS + 0x74)
#define	REG_SOC_REGS_EFUSE_CONTROL                   (SOC_REGS_BASE_ADDRESS + 0x78)
#define	REG_SOC_REGS_EFUSE_STATUS                    (SOC_REGS_BASE_ADDRESS + 0x7C)
#define	REG_SOC_REGS_EFUSE_TIMER_CONFIG1             (SOC_REGS_BASE_ADDRESS + 0x80)
#define	REG_SOC_REGS_EFUSE_TIMER_CONFIG2             (SOC_REGS_BASE_ADDRESS + 0x84)
#define	REG_SOC_REGS_EFUSE_PCIE_FSM_OVR              (SOC_REGS_BASE_ADDRESS + 0x88)
#define	REG_SOC_REGS_EFUSE_PD                        (SOC_REGS_BASE_ADDRESS + 0x8C)
#define	REG_SOC_REGS_WATCHDOG_ENABLE                 (SOC_REGS_BASE_ADDRESS + 0x90)
#define	REG_SOC_REGS_WATCHDOG_RESET                  (SOC_REGS_BASE_ADDRESS + 0x94)
#define	REG_SOC_REGS_WATCHDOG_CMP_VALUE              (SOC_REGS_BASE_ADDRESS + 0x98)
#define	REG_SOC_REGS_WATCHDOG_COUNT                  (SOC_REGS_BASE_ADDRESS + 0x9C)
#define	REG_SOC_REGS_UART_ENABLE                     (SOC_REGS_BASE_ADDRESS + 0xA4)
#define	REG_SOC_REGS_EFUSE_INIT_FSM_OVR              (SOC_REGS_BASE_ADDRESS + 0xA8)
#define	REG_SOC_REGS_PCIE_MSI_INTR_MODE              (SOC_REGS_BASE_ADDRESS + 0xAC)
#define	REG_SOC_REGS_CHIP_ID                         (SOC_REGS_BASE_ADDRESS + 0xB0)
#define	REG_SOC_REGS_EFUSE_INIT_VECTOR_0             (SOC_REGS_BASE_ADDRESS + 0xB4)
#define	REG_SOC_REGS_EFUSE_INIT_VECTOR_24            (SOC_REGS_BASE_ADDRESS + 0xB8)
#define	REG_SOC_REGS_EFUSE_DWORD_ENABLE              (SOC_REGS_BASE_ADDRESS + 0xC0)
#define	REG_SOC_REGS_EFUSE_DWORD_WR_MAP              (SOC_REGS_BASE_ADDRESS + 0xC4)
#define	REG_SOC_REGS_EFUSE_DWORD_RD_DATA             (SOC_REGS_BASE_ADDRESS + 0xC8)
#define	REG_SOC_REGS_EFUSE_DWORD_CONTROL             (SOC_REGS_BASE_ADDRESS + 0xCC)
#define	REG_SOC_REGS_EFUSE_DWORD_DONE                (SOC_REGS_BASE_ADDRESS + 0xD0)
#define	REG_SOC_REGS_EFUSE_BRIDGE_SOFT_RESET         (SOC_REGS_BASE_ADDRESS + 0xD8)
#define	REG_SOC_REGS_PCIE1_RADM_STATUS               (SOC_REGS_BASE_ADDRESS + 0xE4)
#define	REG_SOC_REGS_PCIE_APPS_NS_CTRL               (SOC_REGS_BASE_ADDRESS + 0xE8)
#define	REG_SOC_REGS_PCIE0_PTM_MANUAL_UPDATE         (SOC_REGS_BASE_ADDRESS + 0xF0)
#define	REG_SOC_REGS_PCIE0_PTM_AUTO_UPDATE_SIGNAL    (SOC_REGS_BASE_ADDRESS + 0xF4)
#define	REG_SOC_REGS_PCIE0_PTM_CONTEXT_VALID         (SOC_REGS_BASE_ADDRESS + 0xF8)
#define	REG_SOC_REGS_PCIE0_PTM_CLOCK_UPDATED         (SOC_REGS_BASE_ADDRESS + 0xFC)
#define	REG_SOC_REGS_PCIE0_PTM_LOCAL_CLOCK_L         (SOC_REGS_BASE_ADDRESS + 0x110)
#define	REG_SOC_REGS_PCIE0_PTM_LOCAL_CLOCK_U         (SOC_REGS_BASE_ADDRESS + 0x114)
#define	REG_SOC_REGS_PCIE0_PTM_CLOCK_CORRECTION_L    (SOC_REGS_BASE_ADDRESS + 0x118)
#define	REG_SOC_REGS_PCIE0_PTM_CLOCK_CORRECTION_U    (SOC_REGS_BASE_ADDRESS + 0x11C)
#define	REG_SOC_REGS_PCIE1_PTM_MANUAL_UPDATE         (SOC_REGS_BASE_ADDRESS + 0x120)
#define	REG_SOC_REGS_PCIE1_PTM_AUTO_UPDATE_SIGNAL    (SOC_REGS_BASE_ADDRESS + 0x124)
#define	REG_SOC_REGS_PCIE1_PTM_CONTEXT_VALID         (SOC_REGS_BASE_ADDRESS + 0x128)
#define	REG_SOC_REGS_PCIE1_PTM_CLOCK_UPDATED         (SOC_REGS_BASE_ADDRESS + 0x12C)
#define	REG_SOC_REGS_PCIE1_PTM_LOCAL_CLOCK_L         (SOC_REGS_BASE_ADDRESS + 0x130)
#define	REG_SOC_REGS_PCIE1_PTM_LOCAL_CLOCK_U         (SOC_REGS_BASE_ADDRESS + 0x134)
#define	REG_SOC_REGS_PCIE1_PTM_CLOCK_CORRECTION_L    (SOC_REGS_BASE_ADDRESS + 0x138)
#define	REG_SOC_REGS_PCIE1_PTM_CLOCK_CORRECTION_U    (SOC_REGS_BASE_ADDRESS + 0x13C)
#define	REG_SOC_REGS_PCIE_GPIO_MUX_CONTROL           (SOC_REGS_BASE_ADDRESS + 0x140)
#define	REG_SOC_REGS_PCIE_PHY_CR_PARA_SEL            (SOC_REGS_BASE_ADDRESS + 0x144)
#define	REG_SOC_REGS_PCIE_MEM_CTRL_SP                (SOC_REGS_BASE_ADDRESS + 0x148)
#define	REG_SOC_REGS_PCIE_MEM_CTRL_DP                (SOC_REGS_BASE_ADDRESS + 0x14C)
#define	REG_SOC_REGS_PCIE_MEM_CTRL_TP                (SOC_REGS_BASE_ADDRESS + 0x150)
#define	REG_SOC_REGS_PCIE0_IATU_SEL                  (SOC_REGS_BASE_ADDRESS + 0x154)
#define	REG_SOC_REGS_PCIE1_IATU_SEL                  (SOC_REGS_BASE_ADDRESS + 0x158)
#define	REG_SOC_REGS_TEST_EFUSE_W0                   (SOC_REGS_BASE_ADDRESS + 0x15C)
#define	REG_SOC_REGS_TEST_EFUSE_W1                   (SOC_REGS_BASE_ADDRESS + 0x160)
#define	REG_SOC_REGS_TEST_EFUSE_W2                   (SOC_REGS_BASE_ADDRESS + 0x164)
#define	REG_SOC_REGS_TEST_EFUSE_W3                   (SOC_REGS_BASE_ADDRESS + 0x168)
#define	REG_SOC_REGS_TEST_EFUSE_W4                   (SOC_REGS_BASE_ADDRESS + 0x16C)
#define	REG_SOC_REGS_TEST_EFUSE_W5                   (SOC_REGS_BASE_ADDRESS + 0x170)
#define	REG_SOC_REGS_TEST_EFUSE_W6                   (SOC_REGS_BASE_ADDRESS + 0x174)
#define	REG_SOC_REGS_TEST_EFUSE_W7                   (SOC_REGS_BASE_ADDRESS + 0x178)
#define	REG_SOC_REGS_TEST_EFUSE_W8                   (SOC_REGS_BASE_ADDRESS + 0x17C)
#define	REG_SOC_REGS_TEST_EFUSE_W9                   (SOC_REGS_BASE_ADDRESS + 0x180)
#define	REG_SOC_REGS_TEST_EFUSE_W10                  (SOC_REGS_BASE_ADDRESS + 0x184)
#define	REG_SOC_REGS_TEST_EFUSE_W11                  (SOC_REGS_BASE_ADDRESS + 0x188)
#define	REG_SOC_REGS_TEST_EFUSE_W12                  (SOC_REGS_BASE_ADDRESS + 0x18C)
#define	REG_SOC_REGS_EFUSE_WLAN                      (SOC_REGS_BASE_ADDRESS + 0x190)
#define	REG_SOC_REGS_EFUSE_VID                       (SOC_REGS_BASE_ADDRESS + 0x194)
#define	REG_SOC_REGS_FCSI_PULLUP                     (SOC_REGS_BASE_ADDRESS + 0x200)
#define	REG_SOC_REGS_FCSI_PULLDN                     (SOC_REGS_BASE_ADDRESS + 0x204)
#define	REG_SOC_REGS_FCSI_SLEW_CTRL                  (SOC_REGS_BASE_ADDRESS + 0x208)
#define	REG_SOC_REGS_FCSI_DRIVE_CTRL0                (SOC_REGS_BASE_ADDRESS + 0x20C)
#define	REG_SOC_REGS_FCSI_DRIVE_CTRL1                (SOC_REGS_BASE_ADDRESS + 0x210)
#define	REG_SOC_REGS_FCSI_OPEN_DRAIN                 (SOC_REGS_BASE_ADDRESS + 0x214)
#define	REG_SOC_REGS_BME_RESET_MASK                  (SOC_REGS_BASE_ADDRESS + 0x220)
#define	REG_SOC_REGS_PCIE_PHY_SRAM                   (SOC_REGS_BASE_ADDRESS + 0x224)
#define	REG_SOC_REGS_SOC_SPARE_REG0                  (SOC_REGS_BASE_ADDRESS + 0x230)
#define	REG_SOC_REGS_SOC_SPARE_REG1                  (SOC_REGS_BASE_ADDRESS + 0x234)
#define	REG_SOC_REGS_SOC_SPARE_REG2                  (SOC_REGS_BASE_ADDRESS + 0x238)
#define	REG_SOC_REGS_SOC_SPARE_REG3                  (SOC_REGS_BASE_ADDRESS + 0x23C)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_SOC_REGS_RESERVE_LOC0 0x0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserveLoc0 : 16; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegSocRegsReserveLoc0_u;

/*REG_SOC_REGS_SOFT_RESET 0x4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swWlanRstn : 1; //wlan software reset request : ,  0 - wlan is in reset state ,  1 - wlan is in active state , , reset value: 0x1, access type: RW
		uint32 swCphyRstn : 1; //pcie cphy software reset request  :   , 0 - pcie cphy is in reset state , 1 - pcie cphy  is in active state , , reset value: 0x1, access type: RW
		uint32 swPhyRstn : 1; //pcie phy software reset request  :   , 0 - pcie phy is in reset state , 1 - pcie phy  is in active state , , reset value: 0x1, access type: RW
		uint32 swPcsRstn : 1; //pcie pcs  software reset request :  , 0 - pcie module is in reset state , 1 - pcie module is in active state , , reset value: 0x1, access type: RW
		uint32 swApbPresetn : 1; //pcie apb software reset request  :   , 0 - pcie apb bridge is in reset state , 1 - pcie apb bridge is in active state ,  , , reset value: 0x1, access type: RW
		uint32 swDbiPresetn : 1; //pcie dbi software reset request  :   , 0 - pcie dbi is in reset state , 1 - pcie dbi is in active state ,  , , reset value: 0x1, access type: RW
		uint32 swPcie0Rstn : 1; //pcie 0 software reset request :  , 0 - pcie module is in reset state , 1 - pcie module is in active state , , reset value: 0x1, access type: RW
		uint32 swPcie1Rstn : 1; //pcie1 software reset request :  , 0 - pcie module is in reset state , 1 - pcie module is in active state , , reset value: 0x1, access type: RW
		uint32 swEfuseRstnRsvd : 1; //efuse software reset request  :  Reserved for future use, reset value: 0x1, access type: RW
		uint32 reserved0 : 7;
		uint32 swChipRstn : 1; //global software reset request  :   , 0 - chip is in reset state , 1 - chip is in active state , , reset value: 0x1, access type: RW
		uint32 reserved1 : 15;
	} bitFields;
} RegSocRegsSoftReset_u;

/*REG_SOC_REGS_SOFT_RESET_STROBE_ENABLE 0x8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swWlanRstnEn : 1; //wlan software reset strobe enable : , 0 - no wlan software reset generation , 1 - wlan software reset generation , , reset value: 0x1, access type: RW
		uint32 swCphyRstnEn : 1; //pcie cphy software reset strobe enable : , 0 - no pcie cphy software reset generation , 1 - pcie cphy software reset generation, reset value: 0x0, access type: RW
		uint32 swPhyRstnEn : 1; //pcie phy software reset strobe enable : , 0 - no pcie phy software reset generation , 1 - pcie phy software reset generation, reset value: 0x0, access type: RW
		uint32 swPcsRstnEn : 1; //pcie pcs software reset strobe enable : , 0 - no pcie pcs software reset generation , 1 - pcie pcs software reset generation, reset value: 0x0, access type: RW
		uint32 swApbPresetnEn : 1; //pcie apb software reset strobe enable : , 0 - no pcie apb software reset generation , 1 - pcie apb software reset generation, reset value: 0x0, access type: RW
		uint32 swDbiPresetnEn : 1; //pcie dbi software reset strobe enable : , 0 - no pcie dbi software reset generation , 1 - pcie dbi software reset generation, reset value: 0x0, access type: RW
		uint32 swPcie0RstnEn : 1; //pcie0 software reset strobe enable : , 0 - no pcie0 software reset generation , 1 - pcie0 software reset generation, reset value: 0x0, access type: RW
		uint32 swPcie1RstnEn : 1; //pce1 software reset strobe enable : , 0 - no pcie1 software reset generation , 1 - pcie1 software reset generation, reset value: 0x0, access type: RW
		uint32 swEfuseRstnEn : 1; //Efuse software reset strobe enable : , 0 - no efuse software reset generation , 1 - efuse software reset generation , , reset value: 0x0, access type: RW
		uint32 reserved0 : 7;
		uint32 swChipRstnEn : 1; //global software reset strobe enable : , 0 - no global software reset generation , 1 - global software reset generation , , reset value: 0x0, access type: RW
		uint32 reserved1 : 15;
	} bitFields;
} RegSocRegsSoftResetStrobeEnable_u;

/*REG_SOC_REGS_RCU_BOOT 0xC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 resetFsmState : 3; //RCU boot state machine status : ,    00 : HW_RESET ,    01 : INIT_EFUSE ,    10 : PCIE_EFUSE ,    11 : Reserved, reset value: 0x0, access type: RO
		uint32 reserved0 : 1;
		uint32 efuseInitOvr : 1; //init efuse override : , 0- no init efuse override , 1- init efuse done , , reset value: 0x0, access type: RW
		uint32 efusePcieOvr : 1; //pcie efuse override : , 0- no pcie efuse override , 1- pcie efuse done, reset value: 0x0, access type: RW
		uint32 reserved1 : 2;
		uint32 mspsStatus : 2; //PCIe Mode Select Pin-Strapping status : ,    00 : Mode 0 ,    01 : Mode 1 ,    10 : Mode 2 ,    11 : Mode 1, reset value: 0x2, access type: RO
		uint32 funStrapReg : 4; //no description, reset value: 0xF, access type: RO
		uint32 xtalFreqSelectStatus : 1; //XTAL frequency Select  Pin-Strapping status : ,    0 : 40 MHz ,    1 : 60 MHz, reset value: 0x1, access type: RO
		uint32 reserved2 : 17;
	} bitFields;
} RegSocRegsRcuBoot_u;

/*REG_SOC_REGS_GPIO_PULLUP 0x10 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 gpioPullup : 20; //gpio pull up enable : , 0 - GPIO[0--19] pads pull up disable , 1 - GPIO[0--19] pads pull up enable, reset value: 0xFFEFF, access type: RW
		uint32 reserved0 : 12;
	} bitFields;
} RegSocRegsGpioPullup_u;

/*REG_SOC_REGS_GPIO_PULLDN 0x14 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 gpioPulldn : 20; //gpio pull down enable : , 0 - GPIO[0--19] pads pull down disable , 1 - GPIO[0--19] pads pull down enable, reset value: 0x100, access type: RW
		uint32 reserved0 : 12;
	} bitFields;
} RegSocRegsGpioPulldn_u;

/*REG_SOC_REGS_GPIO_SLEW_CTRL 0x18 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 gpioSlewCtrl : 20; //gpio slew rate control enable : , 0 - GPIO[0--19] pads slew rate disable , 1 - GPIO[0--19] pads slew rate enable, reset value: 0x0, access type: RW
		uint32 reserved0 : 12;
	} bitFields;
} RegSocRegsGpioSlewCtrl_u;

/*REG_SOC_REGS_GPIO_DRIVE_CTRL0 0x1C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 gpioDriveCtrl0 : 20; //gpio drive strength control ds0. ds1 and ds0 are combined to define tge drive strength of pad.  , [ds1 ds0] 00 - 2mA , [ds1 ds0] 01 - 4mA , [ds1 ds0] 10 - 8mA , [ds1 ds0] 11 - 12mA , , reset value: 0x0, access type: RW
		uint32 reserved0 : 12;
	} bitFields;
} RegSocRegsGpioDriveCtrl0_u;

/*REG_SOC_REGS_GPIO_DRIVE_CTRL1 0x20 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 gpioDriveCtrl1 : 20; //gpio drive strength control ds1. ds1 and ds0 are combined to define tge drive strength of pad.  , [ds1 ds0] 00 - 2mA , [ds1 ds0] 01 - 4mA , [ds1 ds0] 10 - 8mA , [ds1 ds0] 11 - 12mA, reset value: 0xfffff, access type: RW
		uint32 reserved0 : 12;
	} bitFields;
} RegSocRegsGpioDriveCtrl1_u;

/*REG_SOC_REGS_GPIO_OPEN_DRAIN 0x24 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 gpioOpenDrain : 20; //gpio open drain enable : , 0 - GPIO[0--19] pads open drain disable/push pull enable , 1 - GPIO[0--19] pads open drain enable/push pull  disable, reset value: 0x0, access type: RW
		uint32 reserved0 : 12;
	} bitFields;
} RegSocRegsGpioOpenDrain_u;

/*REG_SOC_REGS_EEPROM_SW_ACCESS 0x28 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 eepromOverride : 1; //eeprom override : , 0 - EEPROM sw access is disable , 1 - EEPROM sw access is enable, reset value: 0x0, access type: RW
		uint32 eeResv1 : 3; //Reserved location, reset value: 0x0, access type: RO
		uint32 eeSclIn : 1; //eeprom scl input : , EEPROM serial clock is input, reset value: 0x0, access type: RO
		uint32 eeSclOut : 1; //eeprom scl output : , EEPROM serial clock is output, reset value: 0x0, access type: RW
		uint32 eeSclOe : 1; //eeprom scl output enable : , 0 - EEPROM scl is disable , 1 - EEPROM scl is enable, reset value: 0x0, access type: RW
		uint32 reserved0 : 1;
		uint32 eeSdaIn : 1; //eeprom sda input : , EEPROM serial data is input, reset value: 0x0, access type: RO
		uint32 eeSdaOut : 1; //eeprom sda output : , EEPROM serial data is output, reset value: 0x0, access type: RW
		uint32 eeSdaOe : 1; //eeprom sda output enable : , 0 - EEPROM sda is disable , 1 - EEPROM sda is enable, reset value: 0x0, access type: RW
		uint32 eeResv2 : 21; //Reserved location, reset value: 0x0, access type: RO
	} bitFields;
} RegSocRegsEepromSwAccess_u;

/*REG_SOC_REGS_PCIE_EP_CHIP_RST_ENA 0x2C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pcie0EpChipRstEna : 1; //pcie endpoint chip reset enable : , 0 - pcie endpoint chip reset is disable , 1 - pcie endpoint chip reset is enable, reset value: 0x0, access type: RW
		uint32 pcie0EpLinkRstEna : 1; //pcie endpoint link reset enable : , 0 - pcie endpoint link reset is disable , 1 - pcie endpoint link reset is enable, reset value: 0x1, access type: RW
		uint32 reserved0 : 2;
		uint32 pcie1EpChipRstEna : 1; //pcie endpoint 1 chip reset enable : , 0 - pcie endpoint 1 chip reset is disable , 1 - pcie endpoint 1 chip reset is enable, reset value: 0x0, access type: RW
		uint32 pcie1EpLinkRstEna : 1; //pcie endpoint 1 link reset enable : , 0 - pcie endpoint 1 link reset is disable , 1 - pcie endpoint 1  link reset is enable, reset value: 0x1, access type: RW
		uint32 reserved1 : 26;
	} bitFields;
} RegSocRegsPcieEpChipRstEna_u;

/*REG_SOC_REGS_PCIE_PHY_DEBUG_CTL 0x34 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 phyCrDataInReserved : 16; //CR Address and Write Data Input Bus : , Supplies and captures address and write data, reset value: 0x0, access type: RW
		uint32 phyCrCapAddrReserved : 1; //CR Capture Address : , Captures phy_cr_data_in[15:0] into the address register., reset value: 0x0, access type: RW
		uint32 phyCrCapDataReserved : 1; //CR Capture Data : , Captures phy_cr_data_in[15:0] into the write data register., reset value: 0x0, access type: RW
		uint32 phyCrReadReserved : 1; //CR Read : , Reads from the referenced address register., reset value: 0x0, access type: RW
		uint32 phyCrWriteReserved : 1; //CR Write : , Writes the write data register to the referenced address register., reset value: 0x0, access type: RW
		uint32 reserved0 : 12;
	} bitFields;
} RegSocRegsPciePhyDebugCtl_u;

/*REG_SOC_REGS_PCIE_PHY_DEBUG_STATUS 0x38 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 phyCrDataOutReserved : 16; //CR Data Output Bus : , Always presents last read data., reset value: 0x0, access type: RO
		uint32 phyCrAckReserved : 1; //CR Acknowledgement : , Acknowledgement for the phy_cr_cap_addr, phy_cr_cap_data, phy_cr_write, and phy_cr_read control signals., reset value: 0x0, access type: RO
		uint32 reserved0 : 15;
	} bitFields;
} RegSocRegsPciePhyDebugStatus_u;

/*REG_SOC_REGS_PCIE_PHY_CONFIG_REG1 0x3C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 crRegPcsTxDeemphGen1Reserved : 6; //Tx De-emphasis at 3.5 dB : , This static value sets the Tx driver de-emphasis value in the case where pipeP_tx_deemph is set to 1'b1 (default setting) and the PHY is running at the Gen1 rate. To enable tuning at the board level for Rx eye compliance, connect this signal to a register. , reset value: 0x18, access type: RW
		uint32 crRegPcsTxDeemphGen23P5DbReserved : 6; //Tx De-emphasis at 3.5 dB : , This static value sets the Tx driver de-emphasis value in the case where pipeP_tx_deemph is set to 1'b1 (default setting) and the PHY is running at the Gen2 rate. To enable tuning at the board level for Rx eye compliance, connect this signal to a register. , reset value: 0x18, access type: RW
		uint32 crRegPcsTxDeemphGen26DbReserved : 6; //Tx De-emphasis at 6 dB : , This static value sets the Tx driver de-emphasis value in the case where pipeP_tx_deemph is set to 1'b0 and the PHY is running at the Gen2 rate. This bus is provided for completeness and as a second potential launch amplitude. , reset value: 0x21, access type: RW
		uint32 crRegPcsTxSwingFullReserved : 7; //Tx Amplitude (Full Swing Mode) :  , This static value sets the launch amplitude of the transmitter when pipeP_tx_swing is set to 1'b0 (default state). To enable tuning at the board level for Rx eye compliance, connect this signal to a register. , reset value: 0x73, access type: RW
		uint32 crRegPcsTxSwingLowReserved : 7; //Tx Amplitude (Low Swing Mode) : , This static value sets the launch amplitude of the transmitter when pipeP_tx_swing is set to 1'b1 (low swing mode). This signal can be set to an alternate launch amplitude if required. This bus is provided for completeness and can be set to 7'b0., reset value: 0x73, access type: RW
	} bitFields;
} RegSocRegsPciePhyConfigReg1_u;

/*REG_SOC_REGS_PCIE_PHY_CONFIG_REG2 0x40 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 crRegPcsCommonClocksReserved : 1; //Select Common Clock Mode for Receive Data Path : , When both ends of a PCIe link share a common reference clock, the latency through the receiverâs elasticity buffer can be shorter, because no frequency offset exists between the two ends of the link., reset value: 0x0, access type: RW
		uint32 crRegPhyMpllMultiplierReserved : 7; //MPLL Frequency Multiplier Control : , Multiplies the reference clock to a frequency suitable for intended operating speed. , reset value: 0x19, access type: RW
		uint32 crRegPhyRefClkdiv2Reserved : 1; //Input Reference Clock Divider Control : , If the input reference clock frequency is greater than 100 MHz, this signal must be asserted. The reference clock frequency is then divided by 2 to keep it in the range required by the MPLL., reset value: 0x0, access type: RW
		uint32 crRegPhyRefUsePadReserved : 1; //Select Reference Clock Connected to ref_pad_clk_{p,m} : , When asserted, selects the external ref_pad_clk_{p,m} inputs as the reference clock source. When phy_ref_use_pad is deasserted, ref_alt_clk_{p,m} are selected for an on-chip source of the reference clock., reset value: 0x1, access type: RW
		uint32 crRegPhyRx0EqReserved : 3; //Rx Equalizer Setting : , Sets the fixed value of the Rx equalizer. To enable tuning at the board level for Rx eye compliance, connect this signal to a register. The recommended setting is 3âb010. The optimal EQ setting will vary depending on usersâ system environment., reset value: 0x2, access type: RW
		uint32 crRegPhyTx0TermOffsetReserved : 5; //Transmitter Termination Offset : , Enables adjustment of the transmitter termination value from the default value of 50  Î© ., reset value: 0x0, access type: RW
		uint32 crRegPipePortSelReserved : 1; //PIPE Enable Request : , PIPE enable request. Change only when phy_reset is asserted. , For x1 PHYs: , â       0: Disables PIPE. , â       1: Enables PIPE., reset value: 0x1, access type: RW
		uint32 crRegVregBypassReserved : 1; //Built-in 3.3-V Regulator Bypass Mode Control : , When a 2.5-V external supply is applied to the vph pin, vreg_bypass is asserted, and the regulator is placed in Bypass mode. Deasserted when a 3.3-V external supply is applied to the vph pin., reset value: 0x0, access type: RW
		uint32 reserved0 : 12;
	} bitFields;
} RegSocRegsPciePhyConfigReg2_u;

/*REG_SOC_REGS_PCIE_RESP_CTL 0x44 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pcie0SlvRespErrMap : 6; //Slave Response Error Map: ,  This map signal is designed to allow the application to selectively map the errors  , received from the PCIe completion (for non-posted requests) to the AXI slave response slv_rresp or slv_bresp. For each bit, a "1" maps the completion error to SLVERR and a '0' maps the completion error to DECERR. The recommended setting is SLVERR.  For more details, see the 'Advanced Error Handling For  , Received TLPs' appendix. The individual bits represent the following completion errors:   , â       [0]: UR (unsupported request)  , â       [1]: CRS (configuration request retry status)  , â       [2]: CA (Completer Abort)  , â       [3]: Poisoned  , â       [4]: Reserved , â       [5]: Completion Timeout, reset value: 0x0, access type: RW
		uint32 pcie0MstrRespErrMap : 2; //Master Response Error Map :  , Allows you to map the error response from your AXI application slave to a PCIe  , completion status error (for non-posted requests) as follows:   , â       0: CPL abort  , â       1: Unsupported request  , Recommended setting is '0'. For more details, see the 'Advanced  , Error Handling For Received TLPs' appendix.  , â       AXI: Bit [0] corresponds to SLVERR and bit [1] corresponds to  , DECERR., reset value: 0x0, access type: RW
		uint32 reserved0 : 8;
		uint32 pcie1SlvRespErrMap : 6; //Slave 1 Response Error Map: ,  This map signal is designed to allow the application to selectively map the errors  , received from the PCIe completion (for non-posted requests) to the AXI slave response slv_rresp or slv_bresp. For each bit, a "1" maps the completion error to SLVERR and a '0' maps the completion error to DECERR. The recommended setting is SLVERR.  For more details, see the 'Advanced Error Handling For  , Received TLPs' appendix. The individual bits represent the following completion errors:   , â       [0]: UR (unsupported request)  , â       [1]: CRS (configuration request retry status)  , â       [2]: CA (Completer Abort)  , â       [3]: Poisoned  , â       [4]: Reserved , â       [5]: Completion Timeout, reset value: 0x0, access type: RW
		uint32 pcie1MstrRespErrMap : 2; //Master 1 Response Error Map :  , Allows you to map the error response from your AXI application slave to a PCIe  , completion status error (for non-posted requests) as follows:   , â       0: CPL abort  , â       1: Unsupported request  , Recommended setting is '0'. For more details, see the 'Advanced  , Error Handling For Received TLPs' appendix.  , â       AXI: Bit [0] corresponds to SLVERR and bit [1] corresponds to  , DECERR., reset value: 0x0, access type: RW
		uint32 reserved1 : 8;
	} bitFields;
} RegSocRegsPcieRespCtl_u;

/*REG_SOC_REGS_PCIE_INTR_VEC 0x48 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pcieIntrVec : 8; //pcie interrupt vector status : , 0. Host I/F data Tx out interrupt , 1. Host I/F data Rx out interrupt , 2. MGMT Tx out interrupt , 3. MGMT Rx out interrupt , 4. FW-Driver mailbox interrupt , 5. RAB PHI interrupt , 6. WDT interrupt , 7. PVT interrupt, reset value: 0x0, access type: RO
		uint32 reserved0 : 24;
	} bitFields;
} RegSocRegsPcieIntrVec_u;

/*REG_SOC_REGS_PCIE_MSI_ENABLE 0x4C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 msiMsixIntrEn : 8; //PCIe MSI or MSI-X interrupts enable  : This bitfield is changed to reserved and not used. There is no functionality for this bitfield. The bitfield is read/write for future extension. Interrupt enable moved to WLAN IP. , 0. Host I/F data Tx out interrupt ,   0 : disable ,   1 : enable , 1. Host I/F data Rx out interrupt ,   0 : disable ,   1 : enable , 2. MGMT Tx out interrupt ,   0 : disable ,   1 : enable , 3. MGMT Rx out interrupt ,   0 : disable ,   1 : enable , 4. FW-Driver mailbox interrupt ,   0 : disable ,   1 : enable , 5. RAB PHI interrupt ,   0 : disable ,   1 : enable , 6. WDT interrupt ,   0 : disable ,   1 : enable , 7. PVT interrupt ,   0 : disable ,   1 : enable, reset value: 0x0, access type: RW
		uint32 msiMsixIntrMode : 1; //MSI or MSI-X Mode: ,   0 : MSI ,   1 : MSI-X , , reset value: 0x0, access type: RW
		uint32 reserved0 : 3;
		uint32 msixTc : 3; //MSI-X TC , Traffic Class for MSI-X. , reset value: 0x0, access type: RW
		uint32 reserved1 : 17;
	} bitFields;
} RegSocRegsPcieMsiEnable_u;

/*REG_SOC_REGS_PCIE_LEGACY_INT_ENA 0x50 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pcieLegacyIntEna : 1; //pcie legacy interrupt enable : , 0 - disable , 1 - enable, reset value: 0x0, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegSocRegsPcieLegacyIntEna_u;

/*REG_SOC_REGS_PCIE_DIAG_CTRL_BUS 0x54 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pcie0DiagCtrlBus : 3; // Description: Diagnostic Control Bus :   , â       x01: Insert LCRC error by inverting the LSB of LCRC  , â       x10: Insert ECRC error by inverting the LSB of ECRC  , â       The rising edge of these two signals ([1:0]) enable the core to  , assert an LCRC or ECRC to the packet that it currently being  , transferred. When LCRC or ECRC error packets are transmitted  , by the core, the core asserts diag_status_bus[lcrc_err_asserted]  , or diag_status_bus[ecrc_err_asserted] to report that the  , requested action has been completed. This handshake between  , control and status allows your application to control a specific  , packet being injected with an CRC or ECRC error. The LCRC and  , ECRC errors are generated by simply inverting the last bit of the  , LCRC or ECRC value.  , â       1xx: Select Fast Link Mode. Sets all internal timers to Fast Mode  , for speeding up simulation purposes. Forces the LTSSM training  , (link initialization) to use shorter timeouts and to link up faster.  , The scaling factor is 1024 for all internal timers. Fast Link Mode  , can also be activated by setting the "Fast Link Mode" bit of the  , "Port Link Control Register"., reset value: 0x0, access type: RW
		uint32 reserved0 : 13;
		uint32 pcie1DiagCtrlBus : 3; // Description: Diagnostic Control Bus :   , â       x01: Insert LCRC error by inverting the LSB of LCRC  , â       x10: Insert ECRC error by inverting the LSB of ECRC  , â       The rising edge of these two signals ([1:0]) enable the core to  , assert an LCRC or ECRC to the packet that it currently being  , transferred. When LCRC or ECRC error packets are transmitted  , by the core, the core asserts diag_status_bus[lcrc_err_asserted]  , or diag_status_bus[ecrc_err_asserted] to report that the  , requested action has been completed. This handshake between  , control and status allows your application to control a specific  , packet being injected with an CRC or ECRC error. The LCRC and  , ECRC errors are generated by simply inverting the last bit of the  , LCRC or ECRC value.  , â       1xx: Select Fast Link Mode. Sets all internal timers to Fast Mode  , for speeding up simulation purposes. Forces the LTSSM training  , (link initialization) to use shorter timeouts and to link up faster.  , The scaling factor is 1024 for all internal timers. Fast Link Mode  , can also be activated by setting the "Fast Link Mode" bit of the  , "Port Link Control Register"., reset value: 0x0, access type: RW
		uint32 reserved1 : 13;
	} bitFields;
} RegSocRegsPcieDiagCtrlBus_u;

/*REG_SOC_REGS_PCIE_PHY_CONFIG_REG3 0x58 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 phyTx2RxLoopbkReserved : 1; // When this signal is asserted, data from the transmit predriver is looped back to the receive slicers. LOS is bypassed and based on the txN_en input so that rxN_los = !txN_data_en., reset value: 0x0, access type: RW
		uint32 phyTxDeemphDisReserved : 1; //Phy tx deemphasis disable: , 0 :  phy tx deemphasis enable , 1 :  phy tx deemphasis disable, reset value: 0x0, access type: RW
		uint32 phyRtuneReqReserved : 1; //Resistor tune request : ,  Assertion triggers a resistor tune request (if one is not already in progress). When this signal is asserted, phy_rtune_ack will go high until calibration of the termination impedances is complete. Note that termination resistance tuning will disrupt the normal flow of data, so phy_rtune_req must be asserted only  , when the PHY is inactive. Because the PHY will perform a tune automatically after coming out of phy_reset, phy_rtune_req can be tied low unless users want to be able to manually force a tuning. , , reset value: 0x0, access type: RW
		uint32 phyRtuneAckReserved : 1; //Resistor Tune Acknowledge : , While asserted, indicates that a resistor tune is still in progress., reset value: 0x0, access type: RO
		uint32 reserved0 : 28;
	} bitFields;
} RegSocRegsPciePhyConfigReg3_u;

/*REG_SOC_REGS_PCIE_APPS_PM_CLK_SWITCH 0x5C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pcie0CfgL1ClkSwitchGateEn : 1; //cfg_l1_clk_switch_gate_en : ,   0 : core_clk to aux_clk L1 pm switch gate disable ,   1 : core_clk to aux_clk L1 pm switch gate enable, reset value: 0x0, access type: RW
		uint32 reserved0 : 15;
		uint32 pcie1CfgL1ClkSwitchGateEn : 1; //cfg_l1_clk_switch_gate_en : ,   0 : core_clk to aux_clk L1 pm switch gate disable ,   1 : core_clk to aux_clk L1 pm switch gate enable, reset value: 0x0, access type: RW
		uint32 reserved1 : 15;
	} bitFields;
} RegSocRegsPcieAppsPmClkSwitch_u;

/*REG_SOC_REGS_PCIE_APPS_PM_WAKEUP 0x60 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pcie0OutbandPwrupCmd : 1; //Wake Up: , Used by application logic to wake up the PMC state machine from a D1, D2 or D3 power state. Upon wake-up, the core sends a PM_PME Message. Needs to be asserted for one clock cycle. There is a separate apps_pm_xmt_pme input bit for each function in your core configuration. This port is functionally identical to outband_pwrup_cmd. This signal or outband_pwrup_cmd them must be used to request a return from D1/D2/D3 to D0/, reset value: 0x0, access type: RW
		uint32 pcie0AppsPmXmtPme : 1; // Wake Up. Used by application logic to wake up the  , PMC state machine from a D1, D2 or D3 power state. Upon wake-up,  , the core sends a PM_PME Message. Needs to be asserted for one  , clock cycle. There is a separate apps_pm_xmt_pme input bit for  , each function in your core configuration. This port is functionally  , identical to outband_pwrup_cmd. This signal or outband_pwrup_cmd  , them must be used to request a return from D1/D2/D3 to D0/, reset value: 0x0, access type: RW
		uint32 pcie0AppReqExitL1 : 1; //Application request to Exit L1. Request from your application to exit ASPM state L1. It is only effective when L1 is enabled., reset value: 0x0, access type: RW
		uint32 pcie0AppXferPending : 1; //ndicates that your application has , transfers pending and prevents the core from entering , L1, reset value: 0x1, access type: RW
		uint32 reserved0 : 12;
		uint32 pcie1OutbandPwrupCmd : 1; //Wake Up: , Used by application logic to wake up the PMC state machine from a D1, D2 or D3 power state. Upon wake-up, the core sends a PM_PME Message. Needs to be asserted for one clock cycle. There is a separate apps_pm_xmt_pme input bit for each function in your core configuration. This port is functionally identical to outband_pwrup_cmd. This signal or outband_pwrup_cmd them must be used to request a return from D1/D2/D3 to D0/, reset value: 0x0, access type: RW
		uint32 pcie1AppsPmXmtPme : 1; // Wake Up. Used by application logic to wake up the  , PMC state machine from a D1, D2 or D3 power state. Upon wake-up,  , the core sends a PM_PME Message. Needs to be asserted for one  , clock cycle. There is a separate apps_pm_xmt_pme input bit for  , each function in your core configuration. This port is functionally  , identical to outband_pwrup_cmd. This signal or outband_pwrup_cmd  , them must be used to request a return from D1/D2/D3 to D0/, reset value: 0x0, access type: RW
		uint32 pcie1AppReqExitL1 : 1; //Application request to Exit L1. Request from your application to exit ASPM state L1. It is only effective when L1 is enabled., reset value: 0x0, access type: RW
		uint32 pcie1AppXferPending : 1; //ndicates that your application has , transfers pending and prevents the core from entering , L1, reset value: 0x1, access type: RW
		uint32 reserved1 : 12;
	} bitFields;
} RegSocRegsPcieAppsPmWakeup_u;

/*REG_SOC_REGS_PCIE0_RADM_STATUS 0x64 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pcie0RadmxCmposerLookupErr : 1; //Slave Response Composer Lookup Error : , Indicates that an overflow occurred in the lookup table of the inbound  , responses and that there was a violation for the number of outstanding non-posted requests issued for the outbound direction. This should never happen, so this is provided as a debug and diagnostic option., reset value: 0x0, access type: RO
		uint32 pcie0RadmCplTimeout : 1; // Indicates that the completion TLP for a request has not been received within the expected time window., reset value: 0x0, access type: RO
		uint32 pcie0RadmTimeoutFuncNum : 3; //The function Number of the timed out completion. Function numbering starts at '0'., reset value: 0x0, access type: RO
		uint32 pcie0RadmTimeoutCplTc : 3; //The Traffic Class of the timed out completion., reset value: 0x0, access type: RO
		uint32 pcie0RadmTimeoutCplAttr : 2; //The Attributes field of the timed out completion., reset value: 0x0, access type: RO
		uint32 pcie0RadmTimeoutCplLen : 12; //Length (in bytes) of the timed out completion : , For a split completion, it indicates the number of bytes remaining to be delivered when the completion timed out., reset value: 0x0, access type: RO
		uint32 pcie0RadmTimeoutCplTag : 8; //The Tag field of the timed out completion., reset value: 0x0, access type: RO
		uint32 reserved0 : 2;
	} bitFields;
} RegSocRegsPcie0RadmStatus_u;

/*REG_SOC_REGS_PCIE_LINK_STATUS 0x68 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pcie0RdlhLinkUp : 1; //Data link layer up/down indicator :  , This status from the flow control initialization state machine indicates that flow control has been initiated and the Data link layer is ready to transmit and receive  , packets. For multi-VC designs, this signal indicates status for VC0 only.   , â       1: Link is up  , â       0: Link is down, reset value: 0x0, access type: RO
		uint32 pcie0PmCurntState : 3; //Indicates the current power state :  , The pm_curnt_state output is intended for debugging purposes, not for system operation., reset value: 0x0, access type: RO
		uint32 pcie0SmlhLtssmState : 6; // Current state of the LTSSM. , reset value: 0x0, access type: RO
		uint32 pcie0SmlhLinkUp : 1; //PHY Link up/down indicator:   , â       1: Link is up  , â       0: Link is down, reset value: 0x0, access type: RO
		uint32 pcie0LinkReqRstNot : 1; //Active low hot reset request: 0: REQ Reset requested 1: NOR Reset is not requested, reset value: 0x1, access type: RO
		uint32 pcie0LinkTimeoutFlushNot : 1; // The AHB/AXI bridge slave interface asserts this output when it is flushing outbound requests. It does this when the PCIe link is not yet up. This output is provided for informational and debugging purposes. , reset value: 0x1, access type: RO
		uint32 pipeLane0Phystatus : 1; //Communicates completion of PHY functions: 0:Idle 1:Busy, reset value: 0x1, access type: RO
		uint32 reserved0 : 2;
		uint32 pcie1RdlhLinkUp : 1; //Data link layer up/down indicator :  , This status from the flow control initialization state machine indicates that flow control has been initiated and the Data link layer is ready to transmit and receive  , packets. For multi-VC designs, this signal indicates status for VC0 only.   , â       1: Link is up  , â       0: Link is down, reset value: 0x0, access type: RO
		uint32 pcie1PmCurntState : 3; //Indicates the current power state :  , The pm_curnt_state output is intended for debugging purposes, not for system operation., reset value: 0x0, access type: RO
		uint32 pcie1SmlhLtssmState : 6; // Current state of the LTSSM. , reset value: 0x0, access type: RO
		uint32 pcie1SmlhLinkUp : 1; //PHY Link up/down indicator:   , â       1: Link is up  , â       0: Link is down, reset value: 0x0, access type: RO
		uint32 pcie1LinkReqRstNot : 1; //Active low hot reset request: 0: REQ Reset requested 1: NOR Reset is not requested, reset value: 0x1, access type: RO
		uint32 pcie1LinkTimeoutFlushNot : 1; // The AHB/AXI bridge slave interface asserts this output when it is flushing outbound requests. It does this when the PCIe link is not yet up. This output is provided for informational and debugging purposes. , reset value: 0x1, access type: RO
		uint32 pipeLane1Phystatus : 1; //Communicates completion of PHY functions: 0:Idle 1:Busy, reset value: 0x1, access type: RO
		uint32 reserved1 : 2;
	} bitFields;
} RegSocRegsPcieLinkStatus_u;

/*REG_SOC_REGS_PCIE_AMBA_ORDR_MGR_WDT_INT 0x6C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pcie0AmbaOrdrMgrWdtInt : 1; // AMBA order manager watchdog timer interrupt :  , This is a level interrupt that remains asserted while the rule "CPL must not  , pass P" is disabled by the order manager watchdog timer., reset value: 0x0, access type: RO
		uint32 reserved0 : 15;
		uint32 pcie1AmbaOrdrMgrWdtInt : 1; // AMBA order manager watchdog timer interrupt :  , This is a level interrupt that remains asserted while the rule "CPL must not  , pass P" is disabled by the order manager watchdog timer., reset value: 0x0, access type: RO
		uint32 reserved1 : 15;
	} bitFields;
} RegSocRegsPcieAmbaOrdrMgrWdtInt_u;

/*REG_SOC_REGS_EFUSE_DATA 0x70 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 efuseData : 32; //Data read from the fuse array. It holds the byte returned by last completed read operation., reset value: 0x0, access type: RO
	} bitFields;
} RegSocRegsEfuseData_u;

/*REG_SOC_REGS_EFUSE_ADDRESS 0x74 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 efuseAddr : 12; //BIT_ADDRESS[4:0] :	Bit address use only for programming operations. The value is ignored for reads. , WORD_ADDRESS[11:5] : WORD address of the fuse array use for read and program operation., reset value: 0x5F, access type: RW
		uint32 reserved0 : 20;
	} bitFields;
} RegSocRegsEfuseAddress_u;

/*REG_SOC_REGS_EFUSE_CONTROL 0x78 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 efuseWrRdn : 1; //1- program EFUSE array. , 0- read from EFUSE array. , Upon accessing this register a cs strobe will be generated towards the efuse_control_sm. , , reset value: 0x0, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegSocRegsEfuseControl_u;

/*REG_SOC_REGS_EFUSE_STATUS 0x7C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 efuseBusy : 1; //This RO bit is set when the module is busy writing or reading E-Fuse bits on either mode of EFUSE operation. It is cleared when EFUSE control sm has completed operation and is ready to receive a new read or write command., reset value: 0x0, access type: RO
		uint32 efuseInitDone : 1; //INIT machine will set this bit to indicate the EFUSE automatic read is done. If EFUSE_fail_n is active init_done will be set automatically., reset value: 0x0, access type: RO
		uint32 efuseFsmState : 3; //This is a RO register for debug purposes only to read the EFUSE control state machine., reset value: 0x0, access type: RO
		uint32 reserved0 : 27;
	} bitFields;
} RegSocRegsEfuseStatus_u;

/*REG_SOC_REGS_EFUSE_TIMER_CONFIG1 0x80 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 efuseTpgm : 16; //TIMER_TPGM , This register will define the length of STROBE pulse during read write. , During write the strobe should be 12us. , , reset value: 0x12C, access type: RW
		uint32 efuseTsuCsStrobe : 16; //TIMER_TSU_CS_STROBE , Setup time beween CSB activation to STROBE activation. , , reset value: 0x2, access type: RW
	} bitFields;
} RegSocRegsEfuseTimerConfig1_u;

/*REG_SOC_REGS_EFUSE_TIMER_CONFIG2 0x84 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 efuseTrd : 16; //TIMER_TRD  , Strobe width during read. Minimum size is 120ns. , , reset value: 0x8, access type: RW
		uint32 efuseTsuPsCsb : 16; //TSU_PS_CSB , Setup time between PS and CSB : 45ns, reset value: 0x3, access type: RW
	} bitFields;
} RegSocRegsEfuseTimerConfig2_u;

/*REG_SOC_REGS_EFUSE_PCIE_FSM_OVR 0x88 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 efusePcieFsmOvr : 1; //efuse control override : , 0 - no override , 1 - override by software, reset value: 0x0, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegSocRegsEfusePcieFsmOvr_u;

/*REG_SOC_REGS_EFUSE_PD 0x8C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 efusePd : 1; //Power down power switch, when accessing efuse for read/write the pd should be de-activated. Default value of bit is â0â., reset value: 0x0, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegSocRegsEfusePd_u;

/*REG_SOC_REGS_WATCHDOG_ENABLE 0x90 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 watchdogEnable : 1; //watchdog counter enable : , 0 - disable , 1 - enable, reset value: 0x0, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegSocRegsWatchdogEnable_u;

/*REG_SOC_REGS_WATCHDOG_RESET 0x94 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 watchdogReset : 1; //watchdog reset : , 0 - normal operation , 1 - generates a reset pulse, reset value: 0x0, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegSocRegsWatchdogReset_u;

/*REG_SOC_REGS_WATCHDOG_CMP_VALUE 0x98 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 watchdogCmpValue : 32; //32-bit expiration compare value for the watchdog counter., reset value: 0x0, access type: RW
	} bitFields;
} RegSocRegsWatchdogCmpValue_u;

/*REG_SOC_REGS_WATCHDOG_COUNT 0x9C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 watchdogCount : 32; //32-bit watchdog counter status., reset value: 0x0, access type: RO
	} bitFields;
} RegSocRegsWatchdogCount_u;

/*REG_SOC_REGS_UART_ENABLE 0xA4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 uartEnable : 1; //Uart mode enable : , 0 - disable uart mode , 1 - enable uart mode, reset value: 0x0, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegSocRegsUartEnable_u;

/*REG_SOC_REGS_EFUSE_INIT_FSM_OVR 0xA8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 efuseInitFsmOvrRsvd : 1; //efuse init sm override : Not used. Changed to reserved for future use. , 0 - no override , 1 - efuse init sm override, reset value: 0x0, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegSocRegsEfuseInitFsmOvr_u;

/*REG_SOC_REGS_PCIE_MSI_INTR_MODE 0xAC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pcieMsiIntrMode : 2; //pcie msi interrupt modes  :  ,  00 : 8 interrupts mode ,  01 : 4 interrupts mode ,  10 : 1 interrupts mode ,  11 : reserve, reset value: 0x0, access type: RW
		uint32 reserved0 : 30;
	} bitFields;
} RegSocRegsPcieMsiIntrMode_u;

/*REG_SOC_REGS_CHIP_ID 0xB0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 chipId : 32; //WAVE600-2 ChipID  : , Manufacturer ID = 0x089 , Part number = 0x00E1 , version ID = 0x0 , , reset value: 0xE1113, access type: RO
	} bitFields;
} RegSocRegsChipId_u;

/*REG_SOC_REGS_EFUSE_INIT_VECTOR_0 0xB4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 efuseInitVector0 : 32; //First WORD of EFUSE, reset value: 0x0, access type: RO
	} bitFields;
} RegSocRegsEfuseInitVector0_u;

/*REG_SOC_REGS_EFUSE_INIT_VECTOR_24 0xB8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 efuseInitVector24 : 32; //24th WORD of EFUSE, reset value: 0x0, access type: RO
	} bitFields;
} RegSocRegsEfuseInitVector24_u;

/*REG_SOC_REGS_EFUSE_DWORD_ENABLE 0xC0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 efuseDwordEnable : 1; //Efuse dword enable , 0 - Efuse dword operation is disabled , 1 - Efuse dword operation is enabled, reset value: 0x0, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegSocRegsEfuseDwordEnable_u;

/*REG_SOC_REGS_EFUSE_DWORD_WR_MAP 0xC4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 efuseDwordWrMap : 32; //efuse_dword_wr_map : , Indicates 32 bit efuse write operation map , 0 - Efuse bit need not burn , 1 - Efuse bit need to be burn, reset value: 0x0, access type: RW
	} bitFields;
} RegSocRegsEfuseDwordWrMap_u;

/*REG_SOC_REGS_EFUSE_DWORD_RD_DATA 0xC8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 efuseDwordRdData : 32; //efuse_dword_rd_data: , This contains 32 bits Efuse read data., reset value: 0x0, access type: RO
	} bitFields;
} RegSocRegsEfuseDwordRdData_u;

/*REG_SOC_REGS_EFUSE_DWORD_CONTROL 0xCC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 efuseDwordAddress : 7; //efuse_dword_address: , This contains dword aligned address for efuse read/write operations., reset value: 0x0, access type: RW
		uint32 reserved0 : 24;
		uint32 efuseDwordWrRd : 1; //efuse_dword_wr_rd : , 0: Efuse dword read operation , 1: Efuse dword write operation, reset value: 0x0, access type: RW
	} bitFields;
} RegSocRegsEfuseDwordControl_u;

/*REG_SOC_REGS_EFUSE_DWORD_DONE 0xD0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 efuseDwordDone : 1; //efuse_dword_done : , This indicates efuse dword operation done indication in toggle manner for each consecutive phase of operations., reset value: 0x0, access type: RO
		uint32 reserved0 : 31;
	} bitFields;
} RegSocRegsEfuseDwordDone_u;

/*REG_SOC_REGS_EFUSE_BRIDGE_SOFT_RESET 0xD8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0 : 8;
		uint32 swEfuseRstn : 1; //efuse software reset request for bridge :   , 0 - efuse bridge module is in reset state , 1 - efuse bridge module is in active state , , reset value: 0x1, access type: RW
		uint32 reserved1 : 23;
	} bitFields;
} RegSocRegsEfuseBridgeSoftReset_u;

/*REG_SOC_REGS_PCIE1_RADM_STATUS 0xE4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pcie1RadmxCmposerLookupErr : 1; //Slave Response Composer Lookup Error : , Indicates that an overflow occurred in the lookup table of the inbound  , responses and that there was a violation for the number of outstanding non-posted requests issued for the outbound direction. This should never happen, so this is provided as a debug and diagnostic option., reset value: 0x0, access type: RO
		uint32 pcie1RadmCplTimeout : 1; // Indicates that the completion TLP for a request has not been received within the expected time window., reset value: 0x0, access type: RO
		uint32 pcie1RadmTimeoutFuncNum : 3; //The function Number of the timed out completion. Function numbering starts at '0'., reset value: 0x0, access type: RO
		uint32 pcie1RadmTimeoutCplTc : 3; //The Traffic Class of the timed out completion., reset value: 0x0, access type: RO
		uint32 pcie1RadmTimeoutCplAttr : 2; //The Attributes field of the timed out completion., reset value: 0x0, access type: RO
		uint32 pcie1RadmTimeoutCplLen : 12; //Length (in bytes) of the timed out completion : , For a split completion, it indicates the number of bytes remaining to be delivered when the completion timed out., reset value: 0x0, access type: RO
		uint32 pcie1RadmTimeoutCplTag : 8; //The Tag field of the timed out completion., reset value: 0x0, access type: RO
		uint32 reserved0 : 2;
	} bitFields;
} RegSocRegsPcie1RadmStatus_u;

/*REG_SOC_REGS_PCIE_APPS_NS_CTRL 0xE8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pcie0RdTlpNoSnoop : 1; //No Snoop attribute bit in the PCIe0, read TLP header shall be set to the configured value in this register. The no_snoop side band signal on PCIe0 read AXI slave shall be driven with this value. , reset value: 0x0, access type: RW
		uint32 reserved0 : 7;
		uint32 pcie0WrTlpNoSnoop : 1; //No Snoop attribute bit in the PCIe0, write TLP header shall be set to the configured value in this register. The no_snoop side band signal on PCIe0 write AXI slave shall be driven with this value., reset value: 0x0, access type: RW
		uint32 reserved1 : 7;
		uint32 pcie1RdTlpNoSnoop : 1; //No Snoop attribute bit in the PCIe1, read TLP header shall be set to the configured value in this register. The no_snoop side band signal on PCIe1 read AXI slave shall be driven with this value., reset value: 0x0, access type: RW
		uint32 reserved2 : 7;
		uint32 pcie1WrTlpNoSnoop : 1; //No Snoop attribute bit in the PCIe1, write TLP header shall be set to the configured value in this register. The no_snoop side band signal on PCIe1 write AXI slave shall be driven with this value., reset value: 0x0, access type: RW
		uint32 reserved3 : 7;
	} bitFields;
} RegSocRegsPcieAppsNsCtrl_u;

/*REG_SOC_REGS_PCIE0_PTM_MANUAL_UPDATE 0xF0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pcie0PtmManUpdate : 1; //Indicates that the PCIe0 controller should update the PTM Requester Context and Clock now., reset value: 0x0, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegSocRegsPcie0PtmManualUpdate_u;

/*REG_SOC_REGS_PCIE0_PTM_AUTO_UPDATE_SIGNAL 0xF4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0 : 16;
		uint32 pcie0PtmAutoUpdateSignal : 1; //Indicates that the PCIe1 controller should update the PTM Requester Context and Clock automatically every 10ms., reset value: 0x0, access type: RW
		uint32 reserved1 : 15;
	} bitFields;
} RegSocRegsPcie0PtmAutoUpdateSignal_u;

/*REG_SOC_REGS_PCIE0_PTM_CONTEXT_VALID 0xF8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pcie0PtmContextValid : 1; //PTM Context Valid, reset value: 0x0, access type: RO
		uint32 reserved0 : 31;
	} bitFields;
} RegSocRegsPcie0PtmContextValid_u;

/*REG_SOC_REGS_PCIE0_PTM_CLOCK_UPDATED 0xFC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pcie0PtmClockUpdated : 1; //PTM Clock Updated, reset value: 0x0, access type: RO
		uint32 reserved0 : 31;
	} bitFields;
} RegSocRegsPcie0PtmClockUpdated_u;

/*REG_SOC_REGS_PCIE0_PTM_LOCAL_CLOCK_L 0x110 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pcie0PtmLocalClockL : 32; //Lower 32 bits of local clock value, reset value: 0x0, access type: RO
	} bitFields;
} RegSocRegsPcie0PtmLocalClockL_u;

/*REG_SOC_REGS_PCIE0_PTM_LOCAL_CLOCK_U 0x114 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pcie0PtmLocalClockU : 32; //Upper 32 bits of local clock value, reset value: 0x0, access type: RO
	} bitFields;
} RegSocRegsPcie0PtmLocalClockU_u;

/*REG_SOC_REGS_PCIE0_PTM_CLOCK_CORRECTION_L 0x118 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pcie0PtmClockCorrectionL : 32; //Lower 32 bits of local corrected clock value, reset value: 0x0, access type: RO
	} bitFields;
} RegSocRegsPcie0PtmClockCorrectionL_u;

/*REG_SOC_REGS_PCIE0_PTM_CLOCK_CORRECTION_U 0x11C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pcie0PtmClockCorrectionU : 32; //Upper 32 bits of local corrected clock value, reset value: 0x0, access type: RO
	} bitFields;
} RegSocRegsPcie0PtmClockCorrectionU_u;

/*REG_SOC_REGS_PCIE1_PTM_MANUAL_UPDATE 0x120 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pcie0PtmManualUpdate : 1; //Indicates that the PCIe1 controller should update the PTM Requester Context and Clock now., reset value: 0x0, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegSocRegsPcie1PtmManualUpdate_u;

/*REG_SOC_REGS_PCIE1_PTM_AUTO_UPDATE_SIGNAL 0x124 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pcie1PtmAutoUpdateSignal : 1; //Indicates that the PCIe0 controller should update the PTM Requester Context and Clock automatically every 10ms., reset value: 0x0, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegSocRegsPcie1PtmAutoUpdateSignal_u;

/*REG_SOC_REGS_PCIE1_PTM_CONTEXT_VALID 0x128 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pcie1PtmContextValid : 1; //PTM Context Valid, reset value: 0x0, access type: RO
		uint32 reserved0 : 31;
	} bitFields;
} RegSocRegsPcie1PtmContextValid_u;

/*REG_SOC_REGS_PCIE1_PTM_CLOCK_UPDATED 0x12C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pcie1PtmClockUpdated : 1; //PTM Clock Updated, reset value: 0x0, access type: RO
		uint32 reserved0 : 31;
	} bitFields;
} RegSocRegsPcie1PtmClockUpdated_u;

/*REG_SOC_REGS_PCIE1_PTM_LOCAL_CLOCK_L 0x130 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pcie1PtmLocalClockL : 32; //Lower 32 bits of local clock value, reset value: 0x0, access type: RO
	} bitFields;
} RegSocRegsPcie1PtmLocalClockL_u;

/*REG_SOC_REGS_PCIE1_PTM_LOCAL_CLOCK_U 0x134 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pcie1PtmLocalClockU : 32; //Upper 32 bits of local clock value, reset value: 0x0, access type: RO
	} bitFields;
} RegSocRegsPcie1PtmLocalClockU_u;

/*REG_SOC_REGS_PCIE1_PTM_CLOCK_CORRECTION_L 0x138 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pcie1PtmClockCorrectionL : 32; //Lower 32 bits of local corrected clock value, reset value: 0x0, access type: RO
	} bitFields;
} RegSocRegsPcie1PtmClockCorrectionL_u;

/*REG_SOC_REGS_PCIE1_PTM_CLOCK_CORRECTION_U 0x13C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pcie1PtmClockCorrectionU : 32; //Upper 32 bits of local corrected clock value, reset value: 0x0, access type: RO
	} bitFields;
} RegSocRegsPcie1PtmClockCorrectionU_u;

/*REG_SOC_REGS_PCIE_GPIO_MUX_CONTROL 0x140 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pcieClkreqEn : 1; //Enable GPIO6 for use of CLKREQ#, reset value: 0x1, access type: RW
		uint32 reserved0 : 3;
		uint32 pcieWakeEn : 1; //Enable GPIO14 for use of WAKE#, reset value: 0x1, access type: RW
		uint32 reserved1 : 3;
		uint32 phyDebugEn : 1; //Enable GPIO7 and GPIO13 for use as PHY debug, reset value: 0x0, access type: RW
		uint32 reserved2 : 3;
		uint32 vid : 1; //Enable GPIO12 and GPIO15 for use as vid0/1, reset value: 0x1, access type: RW
		uint32 reserved3 : 3;
		uint32 vid0Out : 1; //VID0_OUT: GPIO12 out data if VID=1. FW read VID0 efuse bit and write to this VID0_OUT as output to GPIO12. , reset value: 0x1, access type: RW
		uint32 vid1Out : 1; //VID1_OUT: GPIO15 out data if VID=1. FW read VID1 efuse bit and write to this VID1_OUT as output to GPIO12. , reset value: 0x1, access type: RW
		uint32 reserved4 : 14;
	} bitFields;
} RegSocRegsPcieGpioMuxControl_u;

/*REG_SOC_REGS_PCIE_PHY_CR_PARA_SEL 0x144 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pciePhyCrParaSel : 1; //PCIE PHY Control Register (CR) Parallel Interface Select (Controls selection between JTAG and CR interfaces) : ,  0 - JTAG ,  1 - CR, reset value: 0x1, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegSocRegsPciePhyCrParaSel_u;

/*REG_SOC_REGS_PCIE_MEM_CTRL_SP 0x148 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pcieSpHdRfRm : 4; //RM configuration for PCIE SP HD RFs, reset value: 0x2, access type: RW
		uint32 pcieSpHdRfRme : 1; //RME configuration for PCIE SP HD RFs, reset value: 0x0, access type: RW
		uint32 reserved0 : 3;
		uint32 pcieSpUdSramRm : 4; //RM configuration for PCIE SP UD memories, reset value: 0x2, access type: RW
		uint32 pcieSpUdSramRme : 1; //RME configuration for PCIE SP UD memories, reset value: 0x0, access type: RW
		uint32 reserved1 : 19;
	} bitFields;
} RegSocRegsPcieMemCtrlSp_u;

/*REG_SOC_REGS_PCIE_MEM_CTRL_DP 0x14C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pcieDpHdSramRma : 4; //RMA configuration for PCIE DP HD memories, reset value: 0x2, access type: RW
		uint32 pcieDpHdSramRmea : 1; //RMEA configuration for PCIE DP HD memories, reset value: 0x0, access type: RW
		uint32 reserved0 : 3;
		uint32 pcieDpHdSramRmb : 4; //RMB configuration for PCIE SP DP memories, reset value: 0x2, access type: RW
		uint32 pcieDpHdSramRmeb : 1; //RMEB configuration for PCIE DP HD memories, reset value: 0x0, access type: RW
		uint32 reserved1 : 19;
	} bitFields;
} RegSocRegsPcieMemCtrlDp_u;

/*REG_SOC_REGS_PCIE_MEM_CTRL_TP 0x150 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pcieTpHdRfRma : 4; //RMA configuration for PCIE TP HD RFs, reset value: 0x2, access type: RW
		uint32 pcieTpHdRfRmea : 1; //RMEA configuration for PCIE TP HD RFs, reset value: 0x0, access type: RW
		uint32 reserved0 : 3;
		uint32 pcieTpHdRfRmb : 4; //RMB configuration for PCIE TP HD RFs, reset value: 0x2, access type: RW
		uint32 pcieTpHdRfRmeb : 1; //RMEB configuration for PCIE TP HD RFs, reset value: 0x0, access type: RW
		uint32 reserved1 : 19;
	} bitFields;
} RegSocRegsPcieMemCtrlTp_u;

/*REG_SOC_REGS_PCIE0_IATU_SEL 0x154 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pcie0IatuSel : 16; //PCIe iATU registers access enable. To access iATU registers, this register shall be configured to 16'h0030. To access MSI-X table/PBA, 16'h4030. Otherwise, it shall be configured to 16'b0., reset value: 0x0, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegSocRegsPcie0IatuSel_u;

/*REG_SOC_REGS_PCIE1_IATU_SEL 0x158 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pcie1IatuSel : 16; //PCIe iATU registers access enable. To access iATU registers, this register shall be configured to 16'h0030. To access MSI-X table/PBA, 16'h4030. Otherwise, it shall be configured to 16'b0., reset value: 0x0, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegSocRegsPcie1IatuSel_u;

/*REG_SOC_REGS_TEST_EFUSE_W0 0x15C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 testEfuseW0 : 32; //Word 0 of test efuse, reset value: 0x0, access type: RO
	} bitFields;
} RegSocRegsTestEfuseW0_u;

/*REG_SOC_REGS_TEST_EFUSE_W1 0x160 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 testEfuseW1 : 32; //Word 1 of test efuse, reset value: 0x0, access type: RO
	} bitFields;
} RegSocRegsTestEfuseW1_u;

/*REG_SOC_REGS_TEST_EFUSE_W2 0x164 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 testEfuseW2 : 32; //Word 2 of test efuse, reset value: 0x0, access type: RO
	} bitFields;
} RegSocRegsTestEfuseW2_u;

/*REG_SOC_REGS_TEST_EFUSE_W3 0x168 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 testEfuseW3 : 32; //Word 3 of test efuse, reset value: 0x0, access type: RO
	} bitFields;
} RegSocRegsTestEfuseW3_u;

/*REG_SOC_REGS_TEST_EFUSE_W4 0x16C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 testEfuseW4 : 32; //Word 4 of test efuse, reset value: 0x0, access type: RO
	} bitFields;
} RegSocRegsTestEfuseW4_u;

/*REG_SOC_REGS_TEST_EFUSE_W5 0x170 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 testEfuseW5 : 32; //Word 5 of test efuse, reset value: 0x0, access type: RO
	} bitFields;
} RegSocRegsTestEfuseW5_u;

/*REG_SOC_REGS_TEST_EFUSE_W6 0x174 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 testEfuseW6 : 32; //Word 6 of test efuse, reset value: 0x0, access type: RO
	} bitFields;
} RegSocRegsTestEfuseW6_u;

/*REG_SOC_REGS_TEST_EFUSE_W7 0x178 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 testEfuseW7 : 32; //Word 7 of test efuse, reset value: 0x0, access type: RO
	} bitFields;
} RegSocRegsTestEfuseW7_u;

/*REG_SOC_REGS_TEST_EFUSE_W8 0x17C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 testEfuseW8 : 32; //Word 8 of test efuse, reset value: 0x0, access type: RO
	} bitFields;
} RegSocRegsTestEfuseW8_u;

/*REG_SOC_REGS_TEST_EFUSE_W9 0x180 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 testEfuseW9 : 32; //Word 9 of test efuse, reset value: 0x0, access type: RO
	} bitFields;
} RegSocRegsTestEfuseW9_u;

/*REG_SOC_REGS_TEST_EFUSE_W10 0x184 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 testEfuseW10 : 32; //Word 10 of test efuse, reset value: 0x0, access type: RO
	} bitFields;
} RegSocRegsTestEfuseW10_u;

/*REG_SOC_REGS_TEST_EFUSE_W11 0x188 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 testEfuseW11 : 32; //Word 11 of test efuse, reset value: 0x0, access type: RO
	} bitFields;
} RegSocRegsTestEfuseW11_u;

/*REG_SOC_REGS_TEST_EFUSE_W12 0x18C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 testEfuseW12 : 32; //Word 12 of test efuse, reset value: 0x0, access type: RO
	} bitFields;
} RegSocRegsTestEfuseW12_u;

/*REG_SOC_REGS_EFUSE_WLAN 0x190 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 firewallOn : 1; //Indicates that this is a productive device (non-development one). Firewall is applided based on this indication and FW in boot ROM needs to authenticate the loaded FW , In default (non-blown fuse), device should be in non-protected mode., reset value: 0x0, access type: RO
		uint32 spiDisable : 1; //Inidcates that SPI-based debug interface is disabled. In default (non-blown fuse), SPI should be enabled. This is an indication only! SPI disable is implemented in top level!, reset value: 0x0, access type: RO
		uint32 jtagDisable : 1; //Inidcates that EJTAG interface is disabled.In default (non-blown fuse), EJTAG should be enabled. This is an indication only! EJTAG disable is implemented in top level!, reset value: 0x0, access type: RO
		uint32 securityBootOn : 1; //Indication that FW needs to be checking authentication of loaded FW, reset value: 0x0, access type: RO
		uint32 skipPllInit : 1; //This bit is being read by FW (boot code) , reset value: 0x0, access type: RO
		uint32 reserved0 : 3;
		uint32 chipType : 8; //Chip Type: WAV654: 54H, WAV624: 24H, WAV614: 14H, WAV604: 4H, reset value: 0x0, access type: RO
		uint32 hwType : 8; //Hardware type - indication reflected to driver only, reset value: 0x0, access type: RO
		uint32 hwRevision : 8; //Hardware revision - indication reflected to driver only, reset value: 0x0, access type: RO
	} bitFields;
} RegSocRegsEfuseWlan_u;

/*REG_SOC_REGS_EFUSE_VID 0x194 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 vid0 : 1; //vid0, reset value: 0x0, access type: RO
		uint32 vid1 : 1; //vid1, reset value: 0x0, access type: RO
		uint32 reserved0 : 30;
	} bitFields;
} RegSocRegsEfuseVid_u;

/*REG_SOC_REGS_FCSI_PULLUP 0x200 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fcsiPullup : 19; //fcsi pull up enable : , 0 - fcsi[0--18] pads pull up disable , 1 - fcsi[0--18] pads pull up enable , 0: FCSI_RESET_Q_CENTRAL;  1: FCSI_CLK_CENTRAL;  2: FCSI_CLK_ANT0 ;  3: FCSI_CLK_ANT1 ;  4:FCSI_CLK_ANT2 ;  5: FCSI_CLK_ANT3 ;  6: FCSI_CLK_ANT4 ;  7: FCSI_EN_CENTRAL ;  8: FCSI_EN_ANT0 ; 9: FCSI_EN_ANT1 ;  10: FCSI_EN_ANT2 ;  11: FCSI_EN_ANT3 ;  12: FCSI_EN_ANT4 ; 13:FCSI_DATA_IO_CENTRAL;  14: FCSI_DATA_IO_ANT0;  15: FCSI_DATA_IO_ANT1;  16: FCSI_DATA_IO_ANT2;  17: FCSI_DATA_IO_ANT3;  18: FCSI_DATA_IO_ANT4 , , reset value: 0x1FFF, access type: RW
		uint32 reserved0 : 13;
	} bitFields;
} RegSocRegsFcsiPullup_u;

/*REG_SOC_REGS_FCSI_PULLDN 0x204 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fcsiPulldn : 19; //fcsi pull down enable : , 0 - fcsi[0--18] pads pull down disable , 1 - fcsi[0--18 pads pull down enable, reset value: 0x7E000, access type: RW
		uint32 reserved0 : 13;
	} bitFields;
} RegSocRegsFcsiPulldn_u;

/*REG_SOC_REGS_FCSI_SLEW_CTRL 0x208 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fcsiSlewCtrl : 19; //fcsi slew rate control enable : , 0 - fcsi[0--18] pads slew rate disable , 1 - fcsi[0--18] pads slew rate enable, reset value: 0x7FFFF, access type: RW
		uint32 reserved0 : 13;
	} bitFields;
} RegSocRegsFcsiSlewCtrl_u;

/*REG_SOC_REGS_FCSI_DRIVE_CTRL0 0x20C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fcsiDriveCtrl0 : 19; //fcsi drive strength control ds0. ds1 and ds0 are combined to define the drive strength of pad.  , [ds1 ds0] 00 - 2mA , [ds1 ds0] 01 - 4mA , [ds1 ds0] 10 - 8mA , [ds1 ds0] 11 - 12mA, reset value: 0x0, access type: RW
		uint32 reserved0 : 13;
	} bitFields;
} RegSocRegsFcsiDriveCtrl0_u;

/*REG_SOC_REGS_FCSI_DRIVE_CTRL1 0x210 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fcsiDriveCtrl1 : 19; //fcsi drive strength control ds1.ds1 and ds0 are combined to define the drive strength of pad.  , [ds1 ds0] 00 - 2mA , [ds1 ds0] 01 - 4mA , [ds1 ds0] 10 - 8mA , [ds1 ds0] 11 - 12mA , , reset value: 0x7FFFF, access type: RW
		uint32 reserved0 : 13;
	} bitFields;
} RegSocRegsFcsiDriveCtrl1_u;

/*REG_SOC_REGS_FCSI_OPEN_DRAIN 0x214 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fcsiOpenDrain : 19; //fcsi open drain enable : , 0 - fcsi[0--18] pads open drain disable/push pull enable , 1 - fcsi[0--18] pads open drain enable/push pull  disable, reset value: 0x0, access type: RW
		uint32 reserved0 : 13;
	} bitFields;
} RegSocRegsFcsiOpenDrain_u;

/*REG_SOC_REGS_BME_RESET_MASK 0x220 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bmeMask : 1; //bme reset mask : , 0 - BME toggle from 0 to 1 will reset the WLAN IP.  , 1 - BME toggle from 0 to 1 will not reset WLAN IP. , reset value: 0x0, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegSocRegsBmeResetMask_u;

/*REG_SOC_REGS_PCIE_PHY_SRAM 0x224 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 sramUpdate : 1; //PCIe PHY SRAM Update: , 0 - No SRAM update.  , 1 - SRAM Update. , reset value: 0x0, access type: RW
		uint32 reserved0 : 3;
		uint32 ltssm : 1; //LTSSM0: 0 - link training is disabled. 1 - Link training is enabled., reset value: 0x0, access type: RW
		uint32 reserved1 : 7;
		uint32 ltssmEnMuxSel : 1; //LTSSM Mux Select: 0 - FSM. 1 - SoC Register pcie_phy_sram bit field ltssm ., reset value: 0x0, access type: RW
		uint32 reserved2 : 19;
	} bitFields;
} RegSocRegsPciePhySram_u;

/*REG_SOC_REGS_SOC_SPARE_REG0 0x230 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 socSpare0 : 32; //SoC Spare register 0, reset value: 0x0, access type: RW
	} bitFields;
} RegSocRegsSocSpareReg0_u;

/*REG_SOC_REGS_SOC_SPARE_REG1 0x234 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 socSpare1 : 32; //SoC Spare register 1, reset value: 0x0, access type: RW
	} bitFields;
} RegSocRegsSocSpareReg1_u;

/*REG_SOC_REGS_SOC_SPARE_REG2 0x238 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 socSpare2 : 32; //SoC Spare register 2, reset value: 0x0, access type: RW
	} bitFields;
} RegSocRegsSocSpareReg2_u;

/*REG_SOC_REGS_SOC_SPARE_REG3 0x23C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 socSpare3 : 32; //SoC Spare register 3, reset value: 0x0, access type: RW
	} bitFields;
} RegSocRegsSocSpareReg3_u;



#endif // _SOC_REGS_REGS_H_
