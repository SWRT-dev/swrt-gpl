
/***********************************************************************************
File:				EvmFpgaRegs.h
Module:				evmFpga
SOC Revision:		
Purpose:
Description:		This File was auto generated using SOC Online

************************************************************************************/
#ifndef _EVM_FPGA_REGS_H_
#define _EVM_FPGA_REGS_H_

/*---------------------------------------------------------------------------------
/						Registers Addresses													 
/----------------------------------------------------------------------------------*/
#include "HwMemoryMap.h"

#define EVM_FPGA_BASE_ADDRESS                             MEMORY_MAP_UNIT_18_BASE_ADDRESS
#define	REG_EVM_FPGA_HYP_FCSI_MASTER_STUB_ACTIVE    (EVM_FPGA_BASE_ADDRESS + 0x80000000)
#define	REG_EVM_FPGA_FCSI_RSTRQ                     (EVM_FPGA_BASE_ADDRESS + 0x80000004)
#define	REG_EVM_FPGA_FCSI_CONTROL                   (EVM_FPGA_BASE_ADDRESS + 0x80000008)
#define	REG_EVM_FPGA_FCSI_ADDR_A                    (EVM_FPGA_BASE_ADDRESS + 0x8000000C)
#define	REG_EVM_FPGA_FCSI_WR_DATA_A                 (EVM_FPGA_BASE_ADDRESS + 0x80000018)
#define	REG_EVM_FPGA_FCSI_WR_DATA_B                 (EVM_FPGA_BASE_ADDRESS + 0x8000001C)
#define	REG_EVM_FPGA_FCSI_WR_DATA_C                 (EVM_FPGA_BASE_ADDRESS + 0x80000020)
#define	REG_EVM_FPGA_FCSI_RD_WR                     (EVM_FPGA_BASE_ADDRESS + 0x80000024)
#define	REG_EVM_FPGA_FCSI_BUSY                      (EVM_FPGA_BASE_ADDRESS + 0x80000080)
#define	REG_EVM_FPGA_FCSI_RD_DATA_A                 (EVM_FPGA_BASE_ADDRESS + 0x80000084)
#define	REG_EVM_FPGA_FCSI_RD_DATA_B                 (EVM_FPGA_BASE_ADDRESS + 0x80000088)
#define	REG_EVM_FPGA_FCSI_RD_DATA_C                 (EVM_FPGA_BASE_ADDRESS + 0x8000008C)
#define	REG_EVM_FPGA_MCS_RANK2_0                    (EVM_FPGA_BASE_ADDRESS + 0x0)
#define	REG_EVM_FPGA_MCS_RATE_0                     (EVM_FPGA_BASE_ADDRESS + 0x0)
#define	REG_EVM_FPGA_MCS_N_MODE_0                   (EVM_FPGA_BASE_ADDRESS + 0x0)
#define	REG_EVM_FPGA_MCS_RANK2_1                    (EVM_FPGA_BASE_ADDRESS + 0x4)
#define	REG_EVM_FPGA_MCS_RATE_1                     (EVM_FPGA_BASE_ADDRESS + 0x4)
#define	REG_EVM_FPGA_MCS_N_MODE_1                   (EVM_FPGA_BASE_ADDRESS + 0x4)
#define	REG_EVM_FPGA_USB_SUSPEND_POWER_SAVE_EN      (EVM_FPGA_BASE_ADDRESS + 0x48)
#define	REG_EVM_FPGA_USB_SUSPEND_TESTMODE_EN        (EVM_FPGA_BASE_ADDRESS + 0x48)
#define	REG_EVM_FPGA_MCS_RATE_63                    (EVM_FPGA_BASE_ADDRESS + 0xFC)
#define	REG_EVM_FPGA_MCS_RANK2_63                   (EVM_FPGA_BASE_ADDRESS + 0xFC)
#define	REG_EVM_FPGA_MCS_N_MODE_63                  (EVM_FPGA_BASE_ADDRESS + 0xFC)
#define	REG_EVM_FPGA_DPHY_RADIO_CHANNEL             (EVM_FPGA_BASE_ADDRESS + 0x200)
#define	REG_EVM_FPGA_DPHY_MEDIUM_BUSY_ASSERTION     (EVM_FPGA_BASE_ADDRESS + 0x204)
#define	REG_EVM_FPGA_DPHY_MEDIUM_BUSY_ARM           (EVM_FPGA_BASE_ADDRESS + 0x208)
#define	REG_EVM_FPGA_DPHY_MEDIUM_BUSY_STATUS        (EVM_FPGA_BASE_ADDRESS + 0x20C)
#define	REG_EVM_FPGA_DUMMY_PHY_MODE_ENABLE          (EVM_FPGA_BASE_ADDRESS + 0x210)
#define	REG_EVM_FPGA_DUMMY_PHY_SW_RESET             (EVM_FPGA_BASE_ADDRESS + 0x214)
#define	REG_EVM_FPGA_RESET_TIME_STAMP               (EVM_FPGA_BASE_ADDRESS + 0x20000000)
#define	REG_EVM_FPGA_MODE_REGISTER                  (EVM_FPGA_BASE_ADDRESS + 0x20000004)
#define	REG_EVM_FPGA_END_ADDRESS                    (EVM_FPGA_BASE_ADDRESS + 0x20000008)
#define	REG_EVM_FPGA_PACKET_SIZE_REGISTER           (EVM_FPGA_BASE_ADDRESS + 0x20000010)
#define	REG_EVM_FPGA_USB_AND_START_MODE             (EVM_FPGA_BASE_ADDRESS + 0x20000018)
#define	REG_EVM_FPGA_OFF_CNT_LIMIT                  (EVM_FPGA_BASE_ADDRESS + 0x2000001C)
#define	REG_EVM_FPGA_DSW                            (EVM_FPGA_BASE_ADDRESS + 0x20000020)
#define	REG_EVM_FPGA_DLY_REG                        (EVM_FPGA_BASE_ADDRESS + 0x20000024)
#define	REG_EVM_FPGA_DONE_REG                       (EVM_FPGA_BASE_ADDRESS + 0x2000002C)
#define	REG_EVM_FPGA_USB_START_READ_ADDRESS         (EVM_FPGA_BASE_ADDRESS + 0x20000030)
#define	REG_EVM_FPGA_STOP_FROM_REG                  (EVM_FPGA_BASE_ADDRESS + 0x20000034)
#define	REG_EVM_FPGA_LEDS_REG                       (EVM_FPGA_BASE_ADDRESS + 0x20000038)
#define	REG_EVM_FPGA_USB_FIFO_ADDRESS               (EVM_FPGA_BASE_ADDRESS + 0x2000003C)
#define	REG_EVM_FPGA_VERSION_REG                    (EVM_FPGA_BASE_ADDRESS + 0x2000007C)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_EVM_FPGA_HYP_FCSI_MASTER_STUB_ACTIVE 0x80000000 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hypFcsiMasterStubActive : 1; //Enable FCSI master , reset value: 0x0, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegEvmFpgaHypFcsiMasterStubActive_u;

/*REG_EVM_FPGA_FCSI_RSTRQ 0x80000004 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fcsiRstrq : 1; //FCSI master to slaves reset , reset value: 0x0, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegEvmFpgaFcsiRstrq_u;

/*REG_EVM_FPGA_FCSI_CONTROL 0x80000008 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fcsiClkWrDivr : 5; //FCSI clock write divider, reset value: 0x0, access type: RW
		uint32 reserved0 : 3;
		uint32 fcsiClkRdDivr : 5; //FCSI clock read divider, reset value: 0x0, access type: RW
		uint32 fcsiRfMsSelect : 1; //1'b1 - MS, 1'b0 - RF, reset value: 0x0, access type: RW
		uint32 reserved1 : 18;
	} bitFields;
} RegEvmFpgaFcsiControl_u;

/*REG_EVM_FPGA_FCSI_ADDR_A 0x8000000C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 _fcsiAddrB : 7; //FCSI address channel 1, reset value: 0x0, access type: RW
		uint32 reserved0 : 25;
	} bitFields;
} RegEvmFpgaFcsiAddrA_u;

/*REG_EVM_FPGA_FCSI_WR_DATA_A 0x80000018 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fcsiWrDataA : 16; //FCSI write data channel 0, reset value: 0x0, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegEvmFpgaFcsiWrDataA_u;

/*REG_EVM_FPGA_FCSI_WR_DATA_B 0x8000001C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fcsiWrDataB : 16; //FCSI write data channel 1, reset value: 0x0, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegEvmFpgaFcsiWrDataB_u;

/*REG_EVM_FPGA_FCSI_WR_DATA_C 0x80000020 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fcsiWrDataC : 16; //FCSI write data channel 2, reset value: 0x0, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegEvmFpgaFcsiWrDataC_u;

/*REG_EVM_FPGA_FCSI_RD_WR 0x80000024 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fcsiRdWr : 1; //1'b1 - read, 1'b0 - write, Generates start pulse , reset value: 0x0, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegEvmFpgaFcsiRdWr_u;

/*REG_EVM_FPGA_FCSI_BUSY 0x80000080 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fcsiBusy : 1; //FCSI busy, reset value: 0x0, access type: RO
		uint32 reserved0 : 31;
	} bitFields;
} RegEvmFpgaFcsiBusy_u;

/*REG_EVM_FPGA_FCSI_RD_DATA_A 0x80000084 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fcsiRdDataA : 16; //FCSI read data channel 0, reset value: 0x0, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegEvmFpgaFcsiRdDataA_u;

/*REG_EVM_FPGA_FCSI_RD_DATA_B 0x80000088 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fcsiRdDataB : 16; //FCSI read data channel 1, reset value: 0x0, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegEvmFpgaFcsiRdDataB_u;

/*REG_EVM_FPGA_FCSI_RD_DATA_C 0x8000008C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fcsiRdDataC : 16; //FCSI read data channel 2, reset value: 0x0, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegEvmFpgaFcsiRdDataC_u;

/*REG_EVM_FPGA_MCS_RANK2_0 0x0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0 : 9;
		uint32 mcsRank20 : 1; //MCS Rank 2 for index 0, reset value: 0x0, access type: RW
		uint32 reserved1 : 22;
	} bitFields;
} RegEvmFpgaMcsRank20_u;

/*REG_EVM_FPGA_MCS_RATE_0 0x0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mcsRate0 : 8; //MCS rate for index 0, reset value: 0x0, access type: RW
		uint32 reserved0 : 24;
	} bitFields;
} RegEvmFpgaMcsRate0_u;

/*REG_EVM_FPGA_MCS_N_MODE_0 0x0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0 : 8;
		uint32 mcsNMode0 : 1; //MCS N mode for index 0, reset value: 0x0, access type: RW
		uint32 reserved1 : 23;
	} bitFields;
} RegEvmFpgaMcsNMode0_u;

/*REG_EVM_FPGA_MCS_RANK2_1 0x4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0 : 9;
		uint32 mcsRank21 : 1; //MCS Rank 2 for index 1, reset value: 0x0, access type: RW
		uint32 reserved1 : 22;
	} bitFields;
} RegEvmFpgaMcsRank21_u;

/*REG_EVM_FPGA_MCS_RATE_1 0x4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mcsRate1 : 8; //MCS rate for index 1, reset value: 0x0, access type: RW
		uint32 reserved0 : 24;
	} bitFields;
} RegEvmFpgaMcsRate1_u;

/*REG_EVM_FPGA_MCS_N_MODE_1 0x4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0 : 8;
		uint32 mcsNMode1 : 1; //MCS N mode for index 1, reset value: 0x0, access type: RW
		uint32 reserved1 : 23;
	} bitFields;
} RegEvmFpgaMcsNMode1_u;

/*REG_EVM_FPGA_USB_SUSPEND_POWER_SAVE_EN 0x48 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0 : 1;
		uint32 usbSuspendPowerSaveEn : 1; //USB suspend power save enable, reset value: 0x0, access type: RW
		uint32 reserved1 : 30;
	} bitFields;
} RegEvmFpgaUsbSuspendPowerSaveEn_u;

/*REG_EVM_FPGA_USB_SUSPEND_TESTMODE_EN 0x48 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 usbSuspendTestmodeEn : 1; //USB suspend testmode enable, reset value: 0x0, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegEvmFpgaUsbSuspendTestmodeEn_u;

/*REG_EVM_FPGA_MCS_RATE_63 0xFC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mcsRate63 : 8; //MCS rate for index 63, reset value: 0x0, access type: RW
		uint32 reserved0 : 24;
	} bitFields;
} RegEvmFpgaMcsRate63_u;

/*REG_EVM_FPGA_MCS_RANK2_63 0xFC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0 : 9;
		uint32 mcsRank263 : 1; //MCS Rank 2 for index 63, reset value: 0x0, access type: RW
		uint32 reserved1 : 22;
	} bitFields;
} RegEvmFpgaMcsRank263_u;

/*REG_EVM_FPGA_MCS_N_MODE_63 0xFC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0 : 8;
		uint32 mcsNMode63 : 1; //MCS N mode for index 63, reset value: 0x0, access type: RW
		uint32 reserved1 : 23;
	} bitFields;
} RegEvmFpgaMcsNMode63_u;

/*REG_EVM_FPGA_DPHY_RADIO_CHANNEL 0x200 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dphyRadioChannel : 1; //Dummy phy radio channel, reset value: 0x0, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegEvmFpgaDphyRadioChannel_u;

/*REG_EVM_FPGA_DPHY_MEDIUM_BUSY_ASSERTION 0x204 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dphyMediumBusyDelay : 8; //Dummy phy busy delay, reset value: 0x0, access type: RW
		uint32 dphyMediumBusyTime : 8; //Dummy phy busy time, reset value: 0x0, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegEvmFpgaDphyMediumBusyAssertion_u;

/*REG_EVM_FPGA_DPHY_MEDIUM_BUSY_ARM 0x208 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dphyMediumBusyArm : 1; //Dummy phy medium busy arm, reset value: 0x0, access type: WO
		uint32 reserved0 : 31;
	} bitFields;
} RegEvmFpgaDphyMediumBusyArm_u;

/*REG_EVM_FPGA_DPHY_MEDIUM_BUSY_STATUS 0x20C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dphyMediumBusyDelayTimer : 8; //Busy delay - current timer count, reset value: 0x0, access type: RO
		uint32 dphyMediumBusyTimeTimer : 8; //Busy time - current timer count, reset value: 0x0, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegEvmFpgaDphyMediumBusyStatus_u;

/*REG_EVM_FPGA_DUMMY_PHY_MODE_ENABLE 0x210 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dummyPhyModeEnable : 1; //Dummy phy enable, reset value: 0x0, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegEvmFpgaDummyPhyModeEnable_u;

/*REG_EVM_FPGA_DUMMY_PHY_SW_RESET 0x214 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dummyPhyTxSwReset : 1; //Dummy phy Tx SW reset, reset value: 0x0, access type: RW
		uint32 dummyPhyRxSwReset : 1; //Dummy phy Rx SW reset, reset value: 0x0, access type: RW
		uint32 reserved0 : 30;
	} bitFields;
} RegEvmFpgaDummyPhySwReset_u;

/*REG_EVM_FPGA_RESET_TIME_STAMP 0x20000000 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 resetTimeStamp : 1; //Reset time stamp counter, reset value: 0x0, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegEvmFpgaResetTimeStamp_u;

/*REG_EVM_FPGA_MODE_REGISTER 0x20000004 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 loggerMode : 1; //Logger mode - add time stamp value to the data written in the Sdram, reset value: 0x0, access type: RW
		uint32 continuousMode : 1; //Continuous mode - data from the Sdram will be sent continuously to the Gpio, reset value: 0x0, access type: RW
		uint32 dataFromGpio : 1; //Data from Gpio mode, reset value: 0x0, access type: RW
		uint32 dataFromUsb : 1; //Data from Usb mode, reset value: 0x0, access type: RW
		uint32 dataToHyperion : 1; //Data to Hyperion mode, reset value: 0x0, access type: RW
		uint32 dataToUsb : 1; //Data to Usb mode, reset value: 0x0, access type: RW
		uint32 cyclicMode : 1; //Cyclic mode - data from Gpio will be written continuously to the Sdram till "ffff" will be recetved, reset value: 0x0, access type: RW
		uint32 stopFromRegisterMode : 1; //Stop from register mode -  data from Gpio will be written continuously to the Sdram till write 1 to the stop register, reset value: 0x0, access type: RW
		uint32 reserved0 : 24;
	} bitFields;
} RegEvmFpgaModeRegister_u;

/*REG_EVM_FPGA_END_ADDRESS 0x20000008 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 endAddress : 32; //Last address of the transaction in the Sdram, reset value: 0x0, access type: RW
	} bitFields;
} RegEvmFpgaEndAddress_u;

/*REG_EVM_FPGA_PACKET_SIZE_REGISTER 0x20000010 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 packetSizeRegister : 16; //Needed for Usb read transmition, reset value: 0x0, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegEvmFpgaPacketSizeRegister_u;

/*REG_EVM_FPGA_USB_AND_START_MODE 0x20000018 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 packetEndDisable : 1; //For Usb, reset value: 0x0, access type: RW
		uint32 usbAddr : 2; //For Usb, reset value: 0x0, access type: RW
		uint32 crcMode : 1; //For Usb, reset value: 0x0, access type: RW
		uint32 startOp20 : 1; //Start write to Usb, reset value: 0x0, access type: RW
		uint32 startOp21 : 1; //Start read from Usb, reset value: 0x0, access type: RW
		uint32 startOp0 : 1; //Start Sdram write fifo, reset value: 0x0, access type: RW
		uint32 startOp1 : 1; //Start Sdram read fifo, reset value: 0x0, access type: RW
		uint32 reserved0 : 24;
	} bitFields;
} RegEvmFpgaUsbAndStartMode_u;

/*REG_EVM_FPGA_OFF_CNT_LIMIT 0x2000001C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 offCntLimit : 16; //The register includes the delay between each start of the new data in continuous mode , reset value: 0x0, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegEvmFpgaOffCntLimit_u;

/*REG_EVM_FPGA_DSW 0x20000020 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 deepSwitchesVal : 32; //Read the deep switches values by host, reset value: 0x0, access type: RO
	} bitFields;
} RegEvmFpgaDsw_u;

/*REG_EVM_FPGA_DLY_REG 0x20000024 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 delayStop : 32; //no description, reset value: 0x0, access type: RW
	} bitFields;
} RegEvmFpgaDlyReg_u;

/*REG_EVM_FPGA_DONE_REG 0x2000002C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 done0 : 1; //Finish read from Sdram, reset value: 0x0, access type: RO
		uint32 done1 : 1; //Finish write to Sdram, reset value: 0x0, access type: RO
		uint32 doneOp20 : 1; //Usb read finished, reset value: 0x0, access type: RO
		uint32 doneOp21 : 1; //Usb write finished - valid with done_1 only, reset value: 0x0, access type: RO
		uint32 reserved0 : 28;
	} bitFields;
} RegEvmFpgaDoneReg_u;

/*REG_EVM_FPGA_USB_START_READ_ADDRESS 0x20000030 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 usbStartReadAddress : 32; //Will contain the stop address +dly_reg in cyclic mode, reset value: 0x0, access type: RO
	} bitFields;
} RegEvmFpgaUsbStartReadAddress_u;

/*REG_EVM_FPGA_STOP_FROM_REG 0x20000034 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 stopFromReg : 1; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegEvmFpgaStopFromReg_u;

/*REG_EVM_FPGA_LEDS_REG 0x20000038 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ledsReg : 32; //no description, reset value: 0x0, access type: RW
	} bitFields;
} RegEvmFpgaLedsReg_u;

/*REG_EVM_FPGA_USB_FIFO_ADDRESS 0x2000003C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 usbFifoAddress : 2; //00 - using Fifo2 - usb2sdram; 10 - using Fifo6 - sdram2usb, reset value: 0x0, access type: RW
		uint32 reserved0 : 30;
	} bitFields;
} RegEvmFpgaUsbFifoAddress_u;

/*REG_EVM_FPGA_VERSION_REG 0x2000007C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 versionReg : 32; //Version of the Fpga., reset value: 0x0, access type: RO
	} bitFields;
} RegEvmFpgaVersionReg_u;



#endif // _EVM_FPGA_REGS_H_
