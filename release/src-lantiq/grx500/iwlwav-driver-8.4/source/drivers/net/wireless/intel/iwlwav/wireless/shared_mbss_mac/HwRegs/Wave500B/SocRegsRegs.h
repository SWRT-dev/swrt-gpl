
/***********************************************************************************
File:				SocRegsRegs.h
Module:				socRegs
SOC Revision:		843
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
#define	REG_SOC_REGS_RESERVE_LOC0                  (SOC_REGS_BASE_ADDRESS + 0x0)
#define	REG_SOC_REGS_SOFT_RESET                    (SOC_REGS_BASE_ADDRESS + 0x4)
#define	REG_SOC_REGS_SOFT_RESET_STROBE_ENABLE      (SOC_REGS_BASE_ADDRESS + 0x8)
#define	REG_SOC_REGS_RCU_BOOT                      (SOC_REGS_BASE_ADDRESS + 0xC)
#define	REG_SOC_REGS_GPIO_PULLUP                   (SOC_REGS_BASE_ADDRESS + 0x10)
#define	REG_SOC_REGS_GPIO_PULLDN                   (SOC_REGS_BASE_ADDRESS + 0x14)
#define	REG_SOC_REGS_GPIO_SLEW_CTRL                (SOC_REGS_BASE_ADDRESS + 0x18)
#define	REG_SOC_REGS_GPIO_DRIVE_CTRL0              (SOC_REGS_BASE_ADDRESS + 0x1C)
#define	REG_SOC_REGS_GPIO_DRIVE_CTRL1              (SOC_REGS_BASE_ADDRESS + 0x20)
#define	REG_SOC_REGS_GPIO_OPEN_DRAIN               (SOC_REGS_BASE_ADDRESS + 0x24)
#define	REG_SOC_REGS_EEPROM_SW_ACCESS              (SOC_REGS_BASE_ADDRESS + 0x28)
#define	REG_SOC_REGS_PCIE_EP_CHIP_RST_ENA          (SOC_REGS_BASE_ADDRESS + 0x2C)
#define	REG_SOC_REGS_PCIE_PHY_DEBUG_CTL            (SOC_REGS_BASE_ADDRESS + 0x34)
#define	REG_SOC_REGS_PCIE_PHY_DEBUG_STATUS         (SOC_REGS_BASE_ADDRESS + 0x38)
#define	REG_SOC_REGS_PCIE_PHY_CONFIG_REG1          (SOC_REGS_BASE_ADDRESS + 0x3C)
#define	REG_SOC_REGS_PCIE_PHY_CONFIG_REG2          (SOC_REGS_BASE_ADDRESS + 0x40)
#define	REG_SOC_REGS_PCIE_RESP_CTL                 (SOC_REGS_BASE_ADDRESS + 0x44)
#define	REG_SOC_REGS_PCIE_INTR_VEC                 (SOC_REGS_BASE_ADDRESS + 0x48)
#define	REG_SOC_REGS_PCIE_MSI_ENABLE               (SOC_REGS_BASE_ADDRESS + 0x4C)
#define	REG_SOC_REGS_PCIE_LEGACY_INT_ENA           (SOC_REGS_BASE_ADDRESS + 0x50)
#define	REG_SOC_REGS_PCIE_DIAGS_CTL_BUS            (SOC_REGS_BASE_ADDRESS + 0x54)
#define	REG_SOC_REGS_PCIE_PHY_CONFIG_REG3          (SOC_REGS_BASE_ADDRESS + 0x58)
#define	REG_SOC_REGS_PCIE_APPS_PM_CLK_SWITCH       (SOC_REGS_BASE_ADDRESS + 0x5C)
#define	REG_SOC_REGS_PCIE_APPS_PM_WAKEUP           (SOC_REGS_BASE_ADDRESS + 0x60)
#define	REG_SOC_REGS_PCIE_RADM_STATUS              (SOC_REGS_BASE_ADDRESS + 0x64)
#define	REG_SOC_REGS_PCIE_LINK_STATUS              (SOC_REGS_BASE_ADDRESS + 0x68)
#define	REG_SOC_REGS_PCIE_AMBA_ORDR_MGR_WDT_INT    (SOC_REGS_BASE_ADDRESS + 0x6C)
#define	REG_SOC_REGS_EFUSE_DATA                    (SOC_REGS_BASE_ADDRESS + 0x70)
#define	REG_SOC_REGS_EFUSE_ADDRESS                 (SOC_REGS_BASE_ADDRESS + 0x74)
#define	REG_SOC_REGS_EFUSE_CONTROL                 (SOC_REGS_BASE_ADDRESS + 0x78)
#define	REG_SOC_REGS_EFUSE_STATUS                  (SOC_REGS_BASE_ADDRESS + 0x7C)
#define	REG_SOC_REGS_EFUSE_TIMER_CONFIG1           (SOC_REGS_BASE_ADDRESS + 0x80)
#define	REG_SOC_REGS_EFUSE_TIMER_CONFIG2           (SOC_REGS_BASE_ADDRESS + 0x84)
#define	REG_SOC_REGS_EFUSE_CTL_OVERRIDE            (SOC_REGS_BASE_ADDRESS + 0x88)
#define	REG_SOC_REGS_EFUSE_PD                      (SOC_REGS_BASE_ADDRESS + 0x8C)
#define	REG_SOC_REGS_WATCHDOG_ENABLE               (SOC_REGS_BASE_ADDRESS + 0x90)
#define	REG_SOC_REGS_WATCHDOG_RESET                (SOC_REGS_BASE_ADDRESS + 0x94)
#define	REG_SOC_REGS_WATCHDOG_CMP_VALUE            (SOC_REGS_BASE_ADDRESS + 0x98)
#define	REG_SOC_REGS_WATCHDOG_COUNT                (SOC_REGS_BASE_ADDRESS + 0x9C)
#define	REG_SOC_REGS_PCIE_MEM_CTRL                 (SOC_REGS_BASE_ADDRESS + 0xA0)
#define	REG_SOC_REGS_UART_ENABLE                   (SOC_REGS_BASE_ADDRESS + 0xA4)
#define	REG_SOC_REGS_EFUSE_INIT_SM_OVERRIDE        (SOC_REGS_BASE_ADDRESS + 0xA8)
#define	REG_SOC_REGS_PCIE_MSI_INTR_MODE            (SOC_REGS_BASE_ADDRESS + 0xAC)
#define	REG_SOC_REGS_WAVE500_CHIPID                (SOC_REGS_BASE_ADDRESS + 0xB0)
#define	REG_SOC_REGS_EFUSE_DWORD_ENABLE            (SOC_REGS_BASE_ADDRESS + 0xB4)
#define	REG_SOC_REGS_EFUSE_DWORD_WR_MAP            (SOC_REGS_BASE_ADDRESS + 0xB8)
#define	REG_SOC_REGS_EFUSE_DWORD_RD_DATA           (SOC_REGS_BASE_ADDRESS + 0xBC)
#define	REG_SOC_REGS_EFUSE_DWORD_CONTROL           (SOC_REGS_BASE_ADDRESS + 0xC0)
#define	REG_SOC_REGS_EFUSE_DWORD_DONE              (SOC_REGS_BASE_ADDRESS + 0xC4)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_SOC_REGS_RESERVE_LOC0 0x0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserveLoc0:16;
		uint32 reserved0:16;
	} bitFields;
} RegSocRegsReserveLoc0_u;

/*REG_SOC_REGS_SOFT_RESET 0x4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 wlanSwRstNReq:1;	// wlan software reset request : ,  0 - wlan is in reset state ,  1 - wlan is in active state , 
		uint32 pcieSwResetNReq:1;	// pcie software reset request :  , 0 - pcie module is in reset state , 1 - pcie module is in active state , 
		uint32 pciePhySwResetNReq:1;	// pcie phy software reset request  :   , 0 - pcie phy is in reset state , 1 - pcie phy  is in active state , 
		uint32 efuseSwResetNReq:1;	// efuse software reset request  :   , 0 - efuse module is in reset state , 1 - efuse module is in active state , 
		uint32 pcieAxiSwResetNReq:1;	// pcie axi software reset request  :   , 0 - pcie axi bridge is in reset state , 1 - pcie axi bridge is in active state ,  , 
		uint32 swHwResetNReq:1;	// global software reset request  :   , 0 - chip is in reset state , 1 - chip is in active state , 
		uint32 reserved0:26;
	} bitFields;
} RegSocRegsSoftReset_u;

/*REG_SOC_REGS_SOFT_RESET_STROBE_ENABLE 0x8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 wlanSwRstStbEna:1;	// wlan software reset strobe enable : , 0 - no wlan software reset generation , 1 - wlan software reset generation , 
		uint32 softResetStbEnaReserved1:1;	// Reserved location
		uint32 softResetStbEnaReserved2:1;	// Reserved location
		uint32 efuseSwResetReqStbEna:1;	// Efuse software reset strobe enable : , 0 - no efuse software reset generation , 1 - efuse software reset generation , 
		uint32 softResetStbEnaReserved3:1;	// Reserved location
		uint32 swHwResetReqStbEna:1;	// global software reset strobe enable : , 0 - no global software reset generation , 1 - global software reset generation , 
		uint32 reserved0:26;
	} bitFields;
} RegSocRegsSoftResetStrobeEnable_u;

/*REG_SOC_REGS_RCU_BOOT 0xC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rcuBootStatus:2;	// RCU boot state machine status : ,    00 : HW_RESET ,    01 : INIT_EFUSE ,    10 : PCIE_EFUSE ,    11 : Reserved
		uint32 initEfuseOverride:1;	// init efuse override : , 0- no init efuse override , 1- init efuse done , 
		uint32 pcieEfuseOverride:1;	// pcie efuse override : , 0- no pcie efuse override , 1- pcie efuse done
		uint32 reserved0:28;
	} bitFields;
} RegSocRegsRcuBoot_u;

/*REG_SOC_REGS_GPIO_PULLUP 0x10 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 gpioPullupEn:12;	// gpio pull up enable : , 0 - GPIO[0--11] pads pull up disable , 1 - GPIO[0--11] pads pull up enable
		uint32 reserved0:20;
	} bitFields;
} RegSocRegsGpioPullup_u;

/*REG_SOC_REGS_GPIO_PULLDN 0x14 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 gpioPulldnEn:12;	// gpio pull down enable : , 0 - GPIO[0--11] pads pull down disable , 1 - GPIO[0--11] pads pull down enable
		uint32 reserved0:20;
	} bitFields;
} RegSocRegsGpioPulldn_u;

/*REG_SOC_REGS_GPIO_SLEW_CTRL 0x18 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 gpioSlewCtrlEn:12;	// gpio slew rate control enable : , 0 - GPIO[0--11] pads slew rate disable , 1 - GPIO[0--11] pads slew rate enable
		uint32 reserved0:20;
	} bitFields;
} RegSocRegsGpioSlewCtrl_u;

/*REG_SOC_REGS_GPIO_DRIVE_CTRL0 0x1C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 gpioDriveCtrl0En:12;	// gpio drive strength control ds0 enable : , 0 - GPIO[0--11] pads drive strength control ds0 disable , 1 - GPIO[0--11] pads drive strength control ds0 enable
		uint32 reserved0:20;
	} bitFields;
} RegSocRegsGpioDriveCtrl0_u;

/*REG_SOC_REGS_GPIO_DRIVE_CTRL1 0x20 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 gpioDriveCtrl1En:12;	// gpio drive strength control ds1 enable : , 0 - GPIO[0--11] pads drive strength control ds1 disable , 1 - GPIO[0--11] pads drive strength control ds1 enable
		uint32 reserved0:20;
	} bitFields;
} RegSocRegsGpioDriveCtrl1_u;

/*REG_SOC_REGS_GPIO_OPEN_DRAIN 0x24 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 gpioOpenDrainEn:12;	// gpio open drain enable : , 0 - GPIO[0--11] pads open drain disable/push pull enable , 1 - GPIO[0--11] pads open drain enable/push pull disable
		uint32 reserved0:20;
	} bitFields;
} RegSocRegsGpioOpenDrain_u;

/*REG_SOC_REGS_EEPROM_SW_ACCESS 0x28 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 eepromOverride:1;	// eeprom override : , 0 - EEPROM sw access is disable , 1 - EEPROM sw access is enable
		uint32 eeResv1:3;	// Reserved location
		uint32 eeSclIn:1;	// eeprom scl input : , EEPROM serial clock is input
		uint32 eeSclOut:1;	// eeprom scl output : , EEPROM serial clock is output
		uint32 eeSclOe:1;	// eeprom scl output enable : , 0 - EEPROM scl is disable , 1 - EEPROM scl is enable
		uint32 reserved0:1;
		uint32 eeSdaIn:1;	// eeprom sda input : , EEPROM serial data is input
		uint32 eeSdaOut:1;	// eeprom sda output : , EEPROM serial data is output
		uint32 eeSdaOe:1;	// eeprom sda output enable : , 0 - EEPROM sda is disable , 1 - EEPROM sda is enable
		uint32 eeResv2:21;	// Reserved location
	} bitFields;
} RegSocRegsEepromSwAccess_u;

/*REG_SOC_REGS_PCIE_EP_CHIP_RST_ENA 0x2C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pcieEpChipRstEna:1;	// pcie endpoint chip reset enable : , 0 - pcie endpoint chip reset is disable , 1 - pcie endpoint chip reset is enable
		uint32 pcieEpLinkRstEna:1;	// pcie endpoint link reset enable : , 0 - pcie endpoint link reset is disable , 1 - pcie endpoint link reset is enable
		uint32 reserved0:30;
	} bitFields;
} RegSocRegsPcieEpChipRstEna_u;

/*REG_SOC_REGS_PCIE_PHY_DEBUG_CTL 0x34 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 phyCrDataIn:16;	// CR Address and Write Data Input Bus : , Supplies and captures address and write data
		uint32 phyCrCapAddr:1;	// CR Capture Address : , Captures phy_cr_data_in[15:0] into the address register.
		uint32 phyCrCapData:1;	// CR Capture Data : , Captures phy_cr_data_in[15:0] into the write data register.
		uint32 phyCrRead:1;	// CR Read : , Reads from the referenced address register.
		uint32 phyCrWrite:1;	// CR Write : , Writes the write data register to the referenced address register.
		uint32 reserved0:12;
	} bitFields;
} RegSocRegsPciePhyDebugCtl_u;

/*REG_SOC_REGS_PCIE_PHY_DEBUG_STATUS 0x38 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 phyCrDataOut:16;	// CR Data Output Bus : , Always presents last read data.
		uint32 phyCrAck:1;	// CR Acknowledgement : , Acknowledgement for the phy_cr_cap_addr, phy_cr_cap_data, phy_cr_write, and phy_cr_read control signals.
		uint32 reserved0:15;
	} bitFields;
} RegSocRegsPciePhyDebugStatus_u;

/*REG_SOC_REGS_PCIE_PHY_CONFIG_REG1 0x3C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 crRegPcsTxDeemphGen1:6;	// Tx De-emphasis at 3.5 dB : , This static value sets the Tx driver de-emphasis value in the case where pipeP_tx_deemph is set to 1'b1 (default setting) and the PHY is running at the Gen1 rate. To enable tuning at the board level for Rx eye compliance, connect this signal to a register. 
		uint32 crRegPcsTxDeemphGen23P5Db:6;	// Tx De-emphasis at 3.5 dB : , This static value sets the Tx driver de-emphasis value in the case where pipeP_tx_deemph is set to 1'b1 (default setting) and the PHY is running at the Gen2 rate. To enable tuning at the board level for Rx eye compliance, connect this signal to a register. 
		uint32 crRegPcsTxDeemphGen26Db:6;	// Tx De-emphasis at 6 dB : , This static value sets the Tx driver de-emphasis value in the case where pipeP_tx_deemph is set to 1'b0 and the PHY is running at the Gen2 rate. This bus is provided for completeness and as a second potential launch amplitude. 
		uint32 crRegPcsTxSwingFull:7;	// Tx Amplitude (Full Swing Mode) :  , This static value sets the launch amplitude of the transmitter when pipeP_tx_swing is set to 1'b0 (default state). To enable tuning at the board level for Rx eye compliance, connect this signal to a register. 
		uint32 crRegPcsTxSwingLow:7;	// Tx Amplitude (Low Swing Mode) : , This static value sets the launch amplitude of the transmitter when pipeP_tx_swing is set to 1'b1 (low swing mode). This signal can be set to an alternate launch amplitude if required. This bus is provided for completeness and can be set to 7'b0.
	} bitFields;
} RegSocRegsPciePhyConfigReg1_u;

/*REG_SOC_REGS_PCIE_PHY_CONFIG_REG2 0x40 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 crRegPcsCommonClocks:1;	// Select Common Clock Mode for Receive Data Path : , When both ends of a PCIe link share a common reference clock, the latency through the receiver’s elasticity buffer can be shorter, because no frequency offset exists between the two ends of the link.
		uint32 crRegPhyMpllMultiplier:7;	// MPLL Frequency Multiplier Control : , Multiplies the reference clock to a frequency suitable for intended operating speed. 
		uint32 crRegPhyRefClkdiv2:1;	// Input Reference Clock Divider Control : , If the input reference clock frequency is greater than 100 MHz, this signal must be asserted. The reference clock frequency is then divided by 2 to keep it in the range required by the MPLL.
		uint32 crRegPhyRefUsePad:1;	// Select Reference Clock Connected to ref_pad_clk_{p,m} : , When asserted, selects the external ref_pad_clk_{p,m} inputs as the reference clock source. When phy_ref_use_pad is deasserted, ref_alt_clk_{p,m} are selected for an on-chip source of the reference clock.
		uint32 crRegPhyRx0Eq:3;	// Rx Equalizer Setting : , Sets the fixed value of the Rx equalizer. To enable tuning at the board level for Rx eye compliance, connect this signal to a register. The recommended setting is 3’b010. The optimal EQ setting will vary depending on users’ system environment.
		uint32 crRegPhyTx0TermOffset:5;	// Transmitter Termination Offset : , Enables adjustment of the transmitter termination value from the default value of 50  Ω .
		uint32 crRegPipePortSel:1;	// PIPE Enable Request : , PIPE enable request. Change only when phy_reset is asserted. , For x1 PHYs: , ■      0: Disables PIPE. , ■      1: Enables PIPE.
		uint32 crRegVregBypass:1;	// Built-in 3.3-V Regulator Bypass Mode Control : , When a 2.5-V external supply is applied to the vph pin, vreg_bypass is asserted, and the regulator is placed in Bypass mode. Deasserted when a 3.3-V external supply is applied to the vph pin.
		uint32 reserved0:12;
	} bitFields;
} RegSocRegsPciePhyConfigReg2_u;

/*REG_SOC_REGS_PCIE_RESP_CTL 0x44 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pcieSlvRespErrMap:6;	// Slave Response Error Map: ,  This map signal is designed to allow the application to selectively map the errors  , received from the PCIe completion (for non-posted requests) to the AXI slave response slv_rresp or slv_bresp. For each bit, a "1" maps the completion error to SLVERR and a '0' maps the completion error to DECERR. The recommended setting is SLVERR.  For more details, see the 'Advanced Error Handling For  , Received TLPs' appendix. The individual bits represent the following completion errors:   , ■      [0]: UR (unsupported request)  , ■      [1]: CRS (configuration request retry status)  , ■      [2]: CA (Completer Abort)  , ■      [3]: Poisoned  , ■      [4]: Reserved , ■      [5]: Completion Timeout
		uint32 pcieMstrRespErrMap:2;	// Master Response Error Map :  , Allows you to map the error response from your AXI application slave to a PCIe  , completion status error (for non-posted requests) as follows:   , ■      0: CPL abort  , ■      1: Unsupported request  , Recommended setting is '0'. For more details, see the 'Advanced  , Error Handling For Received TLPs' appendix.  , ■      AXI: Bit [0] corresponds to SLVERR and bit [1] corresponds to  , DECERR.
		uint32 reserved0:24;
	} bitFields;
} RegSocRegsPcieRespCtl_u;

/*REG_SOC_REGS_PCIE_INTR_VEC 0x48 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pcieIntrVec:8;	// pcie interrupt vector status : , 0. Host I/F data Tx out interrupt , 1. Host I/F data Rx out interrupt , 2. MGMT Tx out interrupt , 3. MGMT Rx out interrupt , 4. FW-Driver mailbox interrupt , 5. RAB PHI interrupt , 6. WLAN IP's XBAR error interrupt , 7. WDT interrupt
		uint32 reserved0:24;
	} bitFields;
} RegSocRegsPcieIntrVec_u;

/*REG_SOC_REGS_PCIE_MSI_ENABLE 0x4C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 venMsiEna:8;	// pcie msi interrupts enable  : , 0. Host I/F data Tx out interrupt ,   0 : disable ,   1 : enable , 1. Host I/F data Rx out interrupt ,   0 : disable ,   1 : enable , 2. MGMT Tx out interrupt ,   0 : disable ,   1 : enable , 3. MGMT Rx out interrupt ,   0 : disable ,   1 : enable , 4. FW-Driver mailbox interrupt ,   0 : disable ,   1 : enable , 5. RAB PHI interrupt ,   0 : disable ,   1 : enable , 6. WLAN IP's XBAR error interrupt ,   0 : disable ,   1 : enable , 7. WDT interrupt ,   0 : disable ,   1 : enable
		uint32 reserved0:24;
	} bitFields;
} RegSocRegsPcieMsiEnable_u;

/*REG_SOC_REGS_PCIE_LEGACY_INT_ENA 0x50 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pcieLegacyIntEna:1;	// pcie legacy interrupt enable : , 0 - disable , 1 - enable
		uint32 reserved0:31;
	} bitFields;
} RegSocRegsPcieLegacyIntEna_u;

/*REG_SOC_REGS_PCIE_DIAGS_CTL_BUS 0x54 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pcieDiagsCtlBus:3;	//  Description: Diagnostic Control Bus :   , ■      x01: Insert LCRC error by inverting the LSB of LCRC  , ■      x10: Insert ECRC error by inverting the LSB of ECRC  , ■      The rising edge of these two signals ([1:0]) enable the core to  , assert an LCRC or ECRC to the packet that it currently being  , transferred. When LCRC or ECRC error packets are transmitted  , by the core, the core asserts diag_status_bus[lcrc_err_asserted]  , or diag_status_bus[ecrc_err_asserted] to report that the  , requested action has been completed. This handshake between  , control and status allows your application to control a specific  , packet being injected with an CRC or ECRC error. The LCRC and  , ECRC errors are generated by simply inverting the last bit of the  , LCRC or ECRC value.  , ■      1xx: Select Fast Link Mode. Sets all internal timers to Fast Mode  , for speeding up simulation purposes. Forces the LTSSM training  , (link initialization) to use shorter timeouts and to link up faster.  , The scaling factor is 1024 for all internal timers. Fast Link Mode  , can also be activated by setting the "Fast Link Mode" bit of the  , "Port Link Control Register".
		uint32 reserved0:29;
	} bitFields;
} RegSocRegsPcieDiagsCtlBus_u;

/*REG_SOC_REGS_PCIE_PHY_CONFIG_REG3 0x58 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 phyTx2RxLoopbk:1;	//  When this signal is asserted, data from the transmit predriver is looped back to the receive slicers. LOS is bypassed and based on the txN_en input so that rxN_los = !txN_data_en.
		uint32 phyTxDeemphDis:1;	// Phy tx deemphasis disable: , 0 :  phy tx deemphasis enable , 1 :  phy tx deemphasis disable
		uint32 phyRtuneReq:1;	// Resistor tune request : ,  Assertion triggers a resistor tune request (if one is not already in progress). When this signal is asserted, phy_rtune_ack will go high until calibration of the termination impedances is complete. Note that termination resistance tuning will disrupt the normal flow of data, so phy_rtune_req must be asserted only  , when the PHY is inactive. Because the PHY will perform a tune automatically after coming out of phy_reset, phy_rtune_req can be tied low unless users want to be able to manually force a tuning. , 
		uint32 phyRtuneAck:1;	// Resistor Tune Acknowledge : , While asserted, indicates that a resistor tune is still in progress.
		uint32 reserved0:28;
	} bitFields;
} RegSocRegsPciePhyConfigReg3_u;

/*REG_SOC_REGS_PCIE_APPS_PM_CLK_SWITCH 0x5C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 cfgL1ClkSwitchGateEn:1;	// cfg_l1_clk_switch_gate_en : ,   0 : core_clk to aux_clk L1 pm switch gate disable ,   1 : core_clk to aux_clk L1 pm switch gate enable
		uint32 reserved0:31;
	} bitFields;
} RegSocRegsPcieAppsPmClkSwitch_u;

/*REG_SOC_REGS_PCIE_APPS_PM_WAKEUP 0x60 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pcieOutbandPwrupCmd:1;	// Wake Up: , Used by application logic to wake up the PMC state machine from a D1, D2 or D3 power state. Upon wake-up, the core sends a PM_PME Message. Needs to be asserted for one clock cycle. There is a separate apps_pm_xmt_pme input bit for each function in your core configuration. This port is functionally identical to outband_pwrup_cmd. This signal or outband_pwrup_cmd them must be used to request a return from D1/D2/D3 to D0/
		uint32 pcieAppsPmXmtPme:1;	//  Wake Up. Used by application logic to wake up the  , PMC state machine from a D1, D2 or D3 power state. Upon wake-up,  , the core sends a PM_PME Message. Needs to be asserted for one  , clock cycle. There is a separate apps_pm_xmt_pme input bit for  , each function in your core configuration. This port is functionally  , identical to outband_pwrup_cmd. This signal or outband_pwrup_cmd  , them must be used to request a return from D1/D2/D3 to D0/
		uint32 appReqExitL1:1;	// Application request to Exit L1. Request from your application to exit ASPM state L1. It is only effective when L1 is enabled.
		uint32 reserved0:29;
	} bitFields;
} RegSocRegsPcieAppsPmWakeup_u;

/*REG_SOC_REGS_PCIE_RADM_STATUS 0x64 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 radmxCmposerLookupErr:1;	// Slave Response Composer Lookup Error : , Indicates that an overflow occurred in the lookup table of the inbound  , responses and that there was a violation for the number of outstanding non-posted requests issued for the outbound direction. This should never happen, so this is provided as a debug and diagnostic option.
		uint32 radmCplTimeout:1;	//  Indicates that the completion TLP for a request has not been received within the expected time window.
		uint32 radmTimeoutFuncNum:3;	// The function Number of the timed out completion. Function numbering starts at '0'.
		uint32 radmTimeoutCplTc:3;	// The Traffic Class of the timed out completion.
		uint32 radmTimeoutCplAttr:2;	// The Attributes field of the timed out completion.
		uint32 radmTimeoutCplLen:12;	// Length (in bytes) of the timed out completion : , For a split completion, it indicates the number of bytes remaining to be delivered when the completion timed out.
		uint32 radmTimeoutCplTag:8;	// The Tag field of the timed out completion.
		uint32 reserved0:2;
	} bitFields;
} RegSocRegsPcieRadmStatus_u;

/*REG_SOC_REGS_PCIE_LINK_STATUS 0x68 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rdlhLinkUp:1;	// Data link layer up/down indicator :  , This status from the flow control initialization state machine indicates that flow control has been initiated and the Data link layer is ready to transmit and receive  , packets. For multi-VC designs, this signal indicates status for VC0 only.   , ■      1: Link is up  , ■      0: Link is down
		uint32 pmCurntState:3;	// Indicates the current power state :  , The pm_curnt_state output is intended for debugging purposes, not for system operation.
		uint32 smlhLtssmState:6;	//  Current state of the LTSSM. 
		uint32 smlhLinkUp:1;	// PHY Link up/down indicator:   , ■      1: Link is up  , ■      0: Link is down
		uint32 reserve:1;	// Reserve location.
		uint32 linkTimeoutFlushNot:1;	//  The AHB/AXI bridge slave interface asserts this output when it is flushing outbound requests. It does this when the PCIe link is not yet up. This output is provided for informational and debugging purposes. 
		uint32 reserved0:19;
	} bitFields;
} RegSocRegsPcieLinkStatus_u;

/*REG_SOC_REGS_PCIE_AMBA_ORDR_MGR_WDT_INT 0x6C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ambaOrdrMgrWdtInt:1;	//  AMBA order manager watchdog timer interrupt :  , This is a level interrupt that remains asserted while the rule "CPL must not  , pass P" is disabled by the order manager watchdog timer.
		uint32 reserved0:31;
	} bitFields;
} RegSocRegsPcieAmbaOrdrMgrWdtInt_u;

/*REG_SOC_REGS_EFUSE_DATA 0x70 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 efuseDataQSamp:8;	// Data read from the fuse array. It holds the byte returned by last completed read operation.
		uint32 reserved0:24;
	} bitFields;
} RegSocRegsEfuseData_u;

/*REG_SOC_REGS_EFUSE_ADDRESS 0x74 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 npuEfuseAddr:12;	// BIT_ADDRESS[2:0] :	Bit address use only for programming operations. The value is ignored for reads. , BYTE ADDRESS[7:0] : Byte address of the fuse array use for read and program operation.
		uint32 reserved0:20;
	} bitFields;
} RegSocRegsEfuseAddress_u;

/*REG_SOC_REGS_EFUSE_CONTROL 0x78 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 npuEfuseWrRd:1;	// 1- program EFUSE array. , 0- read from EFUSE array. , Upon accessing this register a cs strobe will be generated towards the efuse_control_sm. , 
		uint32 reserved0:31;
	} bitFields;
} RegSocRegsEfuseControl_u;

/*REG_SOC_REGS_EFUSE_STATUS 0x7C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 efuseControlBusy:1;	// This RO bit is set when the module is busy writing or reading E-Fuse bits on either mode of EFUSE operation. It is cleared when EFUSE control sm has completed operation and is ready to receive a new read or write command.
		uint32 efuseInitDone:1;	// INIT machine will set this bit to indicate the EFUSE automatic read is done. If EFUSE_fail_n is active init_done will be set automatically.
		uint32 efuseControlSm:3;	// This is a RO register for debug purposes only to read the EFUSE control state machine.
		uint32 reserved0:3;
		uint32 efuseInitVector0:8;	// This vector holds 8 bits of data from efuse[7:0] that are read through the efuse_init_sm.
		uint32 efuseInitVector1:8;	// This vector holds 8 bits of data from efuse[15:8] that are read through the efuse_init_sm.
		uint32 reserved1:8;
	} bitFields;
} RegSocRegsEfuseStatus_u;

/*REG_SOC_REGS_EFUSE_TIMER_CONFIG1 0x80 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 npuEfuseTimerCfg1:32;	// npu_efuse_timer_cfg_1 [15:0] : TIMER_TPGM , This register will define the length of STROBE pulse during read write. , During write the strobe should be 5us. , During read the strobe should be 30ns.  , npu_efuse_timer_cfg_1[31:16] : TIMER_TSU_CS_STROBE , Setup time beween CSB activation to STROBE activation. , 
	} bitFields;
} RegSocRegsEfuseTimerConfig1_u;

/*REG_SOC_REGS_EFUSE_TIMER_CONFIG2 0x84 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 npuEfuseTimerCfg2:32;	// npu_efuse_timer_cfg_2 [15:0] : TIMER_TRD  , Strobe width during read. Minimmum size is 30ns. , npu_efuse_timer_cfg_2 [31:16] : TSU_PS_CSB , Setup time between PS and CSB
	} bitFields;
} RegSocRegsEfuseTimerConfig2_u;

/*REG_SOC_REGS_EFUSE_CTL_OVERRIDE 0x88 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 efuseCtlOverride:1;	// efuse control override : , 0 - no override , 1 - override by software
		uint32 reserved0:31;
	} bitFields;
} RegSocRegsEfuseCtlOverride_u;

/*REG_SOC_REGS_EFUSE_PD 0x8C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 efusePd:1;	// Power down power switch, when accessing efuse for read/write the pd should be de-activated. Default value of bit is “0”.
		uint32 reserved0:31;
	} bitFields;
} RegSocRegsEfusePd_u;

/*REG_SOC_REGS_WATCHDOG_ENABLE 0x90 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 watchdogEnable:1;	// watchdog counter enable : , 0 - disable , 1 - enable
		uint32 reserved0:31;
	} bitFields;
} RegSocRegsWatchdogEnable_u;

/*REG_SOC_REGS_WATCHDOG_RESET 0x94 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 watchdogReset:1;	// watchdog reset : , 0 - normal operation , 1 - generates a reset pulse
		uint32 reserved0:31;
	} bitFields;
} RegSocRegsWatchdogReset_u;

/*REG_SOC_REGS_WATCHDOG_CMP_VALUE 0x98 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 watchdogCmpValue:32;	// 32-bit expiration compare value for the watchdog counter.
	} bitFields;
} RegSocRegsWatchdogCmpValue_u;

/*REG_SOC_REGS_WATCHDOG_COUNT 0x9C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 watchdogCount:32;	// 32-bit watchdog counter status.
	} bitFields;
} RegSocRegsWatchdogCount_u;

/*REG_SOC_REGS_PCIE_MEM_CTRL 0xA0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pcieSpMemRm:4;	// Pcie single port memory rm parameter value.
		uint32 pcieDpMemRm:4;	// Pcie single port memory rme parameter value.
		uint32 pcieSpMemRme:1;	// Pcie single port memory test parameter value.
		uint32 pcieDpMemRme:1;	// Pcie dual port memory rm parameter value.
		uint32 pcieSpMemTest:1;	// Pcie dual port memory rme parameter value.
		uint32 pcieDpMemTest:1;	// Pcie dual port memory test parameter value.
		uint32 reserved0:20;
	} bitFields;
} RegSocRegsPcieMemCtrl_u;

/*REG_SOC_REGS_UART_ENABLE 0xA4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 uartEnable:1;	// Uart mode enable : , 0 - disable uart mode , 1 - enable uart mode
		uint32 reserved0:31;
	} bitFields;
} RegSocRegsUartEnable_u;

/*REG_SOC_REGS_EFUSE_INIT_SM_OVERRIDE 0xA8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 efuseInitSmOverride:1;	// efuse init sm override : , 0 - no override , 1 - efuse init sm override
		uint32 reserved0:31;
	} bitFields;
} RegSocRegsEfuseInitSmOverride_u;

/*REG_SOC_REGS_PCIE_MSI_INTR_MODE 0xAC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pcieMsiIntrMode:2;	// pcie msi interrupt modes  :  ,  00 : 8 interrupts mode ,  01 : 4 interrupts mode ,  10 : 1 interrupts mode ,  11 : reserve
		uint32 reserved0:30;
	} bitFields;
} RegSocRegsPcieMsiIntrMode_u;

/*REG_SOC_REGS_WAVE500_CHIPID 0xB0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 wave500Chipid:32;	// WAVE500 ChipID  : , Manufacturer ID = 0x389 , Part number = 0x0800 , version ID = 0x0 , 
	} bitFields;
} RegSocRegsWave500Chipid_u;

/*REG_SOC_REGS_EFUSE_DWORD_ENABLE 0xB4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 efuseDwordEnable:1;	// Efuse dword enable , 0 - Efuse dword operation is disabled , 1 - Efuse dword operation is enabled
		uint32 reserved0:31;
	} bitFields;
} RegSocRegsEfuseDwordEnable_u;

/*REG_SOC_REGS_EFUSE_DWORD_WR_MAP 0xB8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 efuseDwordWrMap:32;	// efuse_dword_wr_map : , Indicates 32 bit efuse write operation map , 0 - Efuse bit need not burn , 1 - Efuse bit need to be burn
	} bitFields;
} RegSocRegsEfuseDwordWrMap_u;

/*REG_SOC_REGS_EFUSE_DWORD_RD_DATA 0xBC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 efuseDwordRdData:32;	// efuse_dword_rd_data: , This contains 32 bits Efuse read data.
	} bitFields;
} RegSocRegsEfuseDwordRdData_u;

/*REG_SOC_REGS_EFUSE_DWORD_CONTROL 0xC0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 efuseDwordAddress:7;	// efuse_dword_address: , This contains dword aligned address for efuse read/write operations.
		uint32 reserved0:24;
		uint32 efuseDwordWrRd:1;	// efuse_dword_wr_rd : , 0: Efuse dword read operation , 1: Efuse dword write operation
	} bitFields;
} RegSocRegsEfuseDwordControl_u;

/*REG_SOC_REGS_EFUSE_DWORD_DONE 0xC4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 efuseDwordDone:1;	// efuse_dword_done : , This indicates efuse dword operation done indication in toggle manner for each consecutive phase of operations.
		uint32 reserved0:31;
	} bitFields;
} RegSocRegsEfuseDwordDone_u;



#endif // _SOC_REGS_REGS_H_
