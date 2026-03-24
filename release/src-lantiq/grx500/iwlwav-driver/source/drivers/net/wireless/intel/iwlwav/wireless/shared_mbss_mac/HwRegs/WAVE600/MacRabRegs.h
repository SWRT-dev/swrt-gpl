
/***********************************************************************************
File:				MacRabRegs.h
Module:				MacRab
SOC Revision:		
Purpose:
Description:		This File was auto generated using SOC Online

************************************************************************************/
#ifndef _MAC_RAB_REGS_H_
#define _MAC_RAB_REGS_H_

/*---------------------------------------------------------------------------------
/						Registers Addresses													 
/----------------------------------------------------------------------------------*/
#include "HwMemoryMap.h"

#define MAC_RAB_BASE_ADDRESS                             MEMORY_MAP_UNIT_18_BASE_ADDRESS
#define	REG_MAC_RAB_REVISION                     (MAC_RAB_BASE_ADDRESS + 0x40)
#define	REG_MAC_RAB_PAS_SW_RESET_REG_RELEASE     (MAC_RAB_BASE_ADDRESS + 0x48)
#define	REG_MAC_RAB_PAS_SW_RESET_REG2_RELEASE    (MAC_RAB_BASE_ADDRESS + 0x4C)
#define	REG_MAC_RAB_PAS_SW_RESET_REG_ASSERT      (MAC_RAB_BASE_ADDRESS + 0x50)
#define	REG_MAC_RAB_PAS_SW_RESET_REG2_ASSERT     (MAC_RAB_BASE_ADDRESS + 0x54)
#define	REG_MAC_RAB_PAS_SW_RESET_REG_RD          (MAC_RAB_BASE_ADDRESS + 0x58)
#define	REG_MAC_RAB_PAS_SW_RESET_REG2_RD         (MAC_RAB_BASE_ADDRESS + 0x5C)
#define	REG_MAC_RAB_TX_DMA_ALIGNER_FIFO_CLEAR    (MAC_RAB_BASE_ADDRESS + 0xD8)
#define	REG_MAC_RAB_TX_DMA_ALIGNER_FIFO_DEBUG    (MAC_RAB_BASE_ADDRESS + 0xDC)
#define	REG_MAC_RAB_TX_DMA_ALIGNER_DEBUG1        (MAC_RAB_BASE_ADDRESS + 0xE0)
#define	REG_MAC_RAB_TX_DMA_ALIGNER_DEBUG2        (MAC_RAB_BASE_ADDRESS + 0xE4)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_MAC_RAB_REVISION 0x40 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 revision : 12; //RAB  revision, reset value: 0x230, access type: RO
		uint32 reserved0 : 20;
	} bitFields;
} RegMacRabRevision_u;

/*REG_MAC_RAB_PAS_SW_RESET_REG_RELEASE 0x48 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swResetReleaseTxPdAcc : 1; //TX_PD Acc SW enable Release, by write '1', reset value: 0x0, access type: WO
		uint32 swResetReleaseTxPdAccReg : 1; //TX_PD Acc registers SW enable Release, by write '1', reset value: 0x0, access type: WO
		uint32 swResetReleaseTxHc : 1; //TX_HC SW enable Release, by write '1', reset value: 0x0, access type: WO
		uint32 swResetReleaseTxHcReg : 1; //TX_HC registers SW enable Release, by write '1', reset value: 0x0, access type: WO
		uint32 swResetReleaseTim : 1; //TIM SW enable Release, by write '1', reset value: 0x0, access type: WO
		uint32 swResetReleaseTimReg : 1; //TIM Reg block SW enable Release, by write '1', reset value: 0x0, access type: WO
		uint32 swResetReleaseDel : 1; //Delia SW enable Release, by write '1', reset value: 0x0, access type: WO
		uint32 swResetReleaseDelReg : 1; //Deliea Reg block SW enable Release, by write '1', reset value: 0x0, access type: WO
		uint32 swResetReleaseTcc : 1; //TCC SW enable Release, by write '1', reset value: 0x0, access type: WO
		uint32 swResetReleaseReserved4 : 1; //SW enable reserved 4 Release, by write '1', reset value: 0x0, access type: WO
		uint32 swResetReleaseRxc : 1; //RxC SW enable Release, by write '1', reset value: 0x0, access type: WO
		uint32 swResetReleaseRxcReg : 1; //RxC Reg block SW enable Release, by write '1', reset value: 0x0, access type: WO
		uint32 swResetReleaseRxf : 1; //RxFC SW enable Release, by write '1', reset value: 0x0, access type: WO
		uint32 swResetReleaseRxfReg : 1; //RxF Reg block SW enable Release, by write '1', reset value: 0x0, access type: WO
		uint32 swResetReleaseRcc : 1; //RCC SW enable Release, by write '1', reset value: 0x0, access type: WO
		uint32 swResetReleaseBeaconTimers : 1; //Beacon Timers SW registers enable Release, by write '1', reset value: 0x0, access type: WO
		uint32 swResetReleaseRxd : 1; //RxC SW enable Release, by write '1', reset value: 0x0, access type: WO
		uint32 swResetReleaseRxdReg : 1; //RxC Reg block SW enable Release, by write '1', reset value: 0x0, access type: WO
		uint32 swResetReleaseDur : 1; //DUR SW enable Release, by write '1', reset value: 0x0, access type: WO
		uint32 swResetReleaseDurReg : 1; //DUR Reg block SW enable Release, by write '1', reset value: 0x0, access type: WO
		uint32 swResetReleaseRta : 1; //RTA SW enable Release, by write '1', reset value: 0x0, access type: WO
		uint32 swResetReleaseBf : 1; //BF SW enable Release, by write '1', reset value: 0x0, access type: WO
		uint32 swResetReleaseWep : 1; //WEP SW enable Release, by write '1', reset value: 0x0, access type: WO
		uint32 swResetReleaseAggBuilder : 1; //AGG builder SW enable Release, by write '1', reset value: 0x0, access type: WO
		uint32 swResetReleaseAggBuilderReg : 1; //AGG builder registers SW enable Release, by write '1', reset value: 0x0, access type: WO
		uint32 swResetReleaseAddr2Index : 1; //Addr2index SW enable Release, by write '1', reset value: 0x0, access type: WO
		uint32 swResetReleaseAddr2IndexReg : 1; //Addr2index registers SW enable Release, by write '1', reset value: 0x0, access type: WO
		uint32 swResetReleaseTxHandler : 1; //Tx handler SW enable Release, by write '1', reset value: 0x0, access type: WO
		uint32 swResetReleaseTxHandlerReg : 1; //Tx handler registers SW enable Release, by write '1', reset value: 0x0, access type: WO
		uint32 swResetReleaseTxSelector : 1; //Tx Selector SW enable Release, by write '1', reset value: 0x0, access type: WO
		uint32 swResetReleaseTxSelectorReg : 1; //Tx Selector registers SW enable Release, by write '1', reset value: 0x0, access type: WO
		uint32 swResetReleaseBeaconTimersReg : 1; //Beacon Timers SW enable Release, by write '1', reset value: 0x0, access type: WO
	} bitFields;
} RegMacRabPasSwResetRegRelease_u;

/*REG_MAC_RAB_PAS_SW_RESET_REG2_RELEASE 0x4C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swResetReleaseTxhTxc : 1; //TXH TXC SW enable Release, by write '1', reset value: 0x0, access type: WO
		uint32 swResetReleaseTxhTxcReg : 1; //TXH TXC registers SW enable Release, by write '1', reset value: 0x0, access type: WO
		uint32 swResetReleaseTxhNtd : 1; //TXH NTD SW enable Release, by write '1', reset value: 0x0, access type: WO
		uint32 swResetReleaseTxhNtdReg : 1; //TXH NTD registers SW enable Release, by write '1', reset value: 0x0, access type: WO
		uint32 swResetReleaseTxDmaAligner : 1; //Tx DMA aligner SW enable  Release, by write '1', reset value: 0x0, access type: WO
		uint32 swResetReleaseBaAnalizer : 1; //ba_analizer  SW enable  Release, by write '1', reset value: 0x0, access type: WO
		uint32 swResetReleaseOtfa : 1; //OTFA SW enable Release, by write '1', reset value: 0x0, access type: WO
		uint32 swResetReleaseOtfaReg : 1; //OTFA register file SW enable Release, by write '1', reset value: 0x0, access type: WO
		uint32 swResetReleaseLiberatorReg : 1; //sw_reRelease_liberator_reg Release, by write '1', reset value: 0x0, access type: WO
		uint32 swResetReleaseRxClas : 1; //Rx Classifier SW enable  Release, by write '1', reset value: 0x0, access type: WO
		uint32 swResetReleaseRxClasReg : 1; //Rx Classifier registers SW enable  Release, by write '1', reset value: 0x0, access type: WO
		uint32 swResetReleasePsSetting : 1; //PS Setting SW enable  Release, by write '1', reset value: 0x0, access type: WO
		uint32 swResetReleasePsSettingReg : 1; //PS Setting registers SW enable  Release, by write '1', reset value: 0x0, access type: WO
		uint32 swResetReleaseTfGenerator : 1; //TF Generator enable Release, by write '1', reset value: 0x0, access type: WO
		uint32 swResetReleaseTxSequencer : 1; //Tx Sequencer SW enable Release, by write '1', reset value: 0x0, access type: WO
		uint32 swResetReleasePacExtrap : 1; //PAC extrapolator SW enable  Release, by write '1', reset value: 0x0, access type: WO
		uint32 swResetReleasePacExtrapReg : 1; //PAC extrapolator registers SW enable  Release, by write '1', reset value: 0x0, access type: WO
		uint32 swResetReleaseTfGeneratorReg : 1; //TF Generator registers enable Release, by write '1', reset value: 0x0, access type: WO
		uint32 swResetReleaseGenriscTx : 1; //Genrisc TX SW enable  Release, by write '1', reset value: 0x0, access type: WO
		uint32 swResetReleaseGenriscRx : 1; //Genrisc Rx SW enable  Release, by write '1', reset value: 0x0, access type: WO
		uint32 swResetReleaseRxMsduParser : 1; //Rx MSDU Parser SW enable  Release, by write '1', reset value: 0x0, access type: WO
		uint32 swResetReleaseRxMsduParserReg : 1; //Rx MSDU Parser Registers SW enable  Release, by write '1', reset value: 0x0, access type: WO
		uint32 swResetReleaseRxMsduParserLogger : 1; //Rx MSDU Parser Logger SW enable  Release, by write '1', reset value: 0x0, access type: WO
		uint32 swResetReleaseRxCoordinator : 1; //Rx Coordinator SW enable  Release, by write '1', reset value: 0x0, access type: WO
		uint32 swResetReleaseMbfm : 1; //MAC BFM SW enable  Release, by write '1', reset value: 0x0, access type: WO
		uint32 swResetReleaseUlPostRx : 1; //UL Post Rx enable Release, by write '1', reset value: 0x0, access type: WO
		uint32 swResetReleaseUlPostRxReg : 1; //UL Post RX registers enable Release, by write '1', reset value: 0x0, access type: WO
		uint32 swResetReleaseBsrc : 1; //BSRC enable Release, by write '1', reset value: 0x0, access type: WO
		uint32 swResetReleaseReserved : 4; //no description, reset value: 0x0, access type: WO
	} bitFields;
} RegMacRabPasSwResetReg2Release_u;

/*REG_MAC_RAB_PAS_SW_RESET_REG_ASSERT 0x50 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swResetAssertTxPdAcc : 1; //TX_PD Acc SW enable Assert, by write '1', reset value: 0x0, access type: WO
		uint32 swResetAssertTxPdAccReg : 1; //TX_PD Acc registers SW enable Assert, by write '1', reset value: 0x0, access type: WO
		uint32 swResetAssertTxHc : 1; //TX_HC SW enable Assert, by write '1', reset value: 0x0, access type: WO
		uint32 swResetAssertTxHcReg : 1; //TX_HC registers SW enable Assert, by write '1', reset value: 0x0, access type: WO
		uint32 swResetAssertTim : 1; //TIM SW enable Assert, by write '1', reset value: 0x0, access type: WO
		uint32 swResetAssertTimReg : 1; //TIM Reg block SW enable Assert, by write '1', reset value: 0x0, access type: WO
		uint32 swResetAssertDel : 1; //Delia SW enable Assert, by write '1', reset value: 0x0, access type: WO
		uint32 swResetAssertDelReg : 1; //Delia Reg block SW enable Assert, by write '1', reset value: 0x0, access type: WO
		uint32 swResetAssertTcc : 1; //TCC SW enable Assert, by write '1', reset value: 0x0, access type: WO
		uint32 swResetAssertReserved4 : 1; //SW enable reserved 4 Assert, by write '1', reset value: 0x0, access type: WO
		uint32 swResetAssertRxc : 1; //RxC SW enable Assert, by write '1', reset value: 0x0, access type: WO
		uint32 swResetAssertRxcReg : 1; //RxC Reg block SW enable Assert, by write '1', reset value: 0x0, access type: WO
		uint32 swResetAssertRxf : 1; //RxFC SW enable Assert, by write '1', reset value: 0x0, access type: WO
		uint32 swResetAssertRxfReg : 1; //RxF Reg block SW enable Assert, by write '1', reset value: 0x0, access type: WO
		uint32 swResetAssertRcc : 1; //RCC SW enable Assert, by write '1', reset value: 0x0, access type: WO
		uint32 swResetAssertBeaconTimers : 1; //Beacon Timers SW registers enable Assert, by write '1', reset value: 0x0, access type: WO
		uint32 swResetAssertRxd : 1; //RxC SW enable Assert, by write '1', reset value: 0x0, access type: WO
		uint32 swResetAssertRxdReg : 1; //RxC Reg block SW enable Assert, by write '1', reset value: 0x0, access type: WO
		uint32 swResetAssertDur : 1; //DUR SW enable Assert, by write '1', reset value: 0x0, access type: WO
		uint32 swResetAssertDurReg : 1; //DUR Reg block SW enable Assert, by write '1', reset value: 0x0, access type: WO
		uint32 swResetAssertRta : 1; //RTA SW enable Assert, by write '1', reset value: 0x0, access type: WO
		uint32 swResetAssertBf : 1; //BF SW enable Assert, by write '1', reset value: 0x0, access type: WO
		uint32 swResetAssertWep : 1; //WEP SW enable Assert, by write '1', reset value: 0x0, access type: WO
		uint32 swResetAssertAggBuilder : 1; //AGG builder SW enable Assert, by write '1', reset value: 0x0, access type: WO
		uint32 swResetAssertAggBuilderReg : 1; //AGG builder registers SW enable Assert, by write '1', reset value: 0x0, access type: WO
		uint32 swResetAssertAddr2Index : 1; //Addr2index SW enable Assert, by write '1', reset value: 0x0, access type: WO
		uint32 swResetAssertAddr2IndexReg : 1; //Addr2index registers SW enable Assert, by write '1', reset value: 0x0, access type: WO
		uint32 swResetAssertTxHandler : 1; //Tx handler SW enable Assert, by write '1', reset value: 0x0, access type: WO
		uint32 swResetAssertTxHandlerReg : 1; //Tx handler registers SW enable Assert, by write '1', reset value: 0x0, access type: WO
		uint32 swResetAssertTxSelector : 1; //Tx Selector SW enable Assert, by write '1', reset value: 0x0, access type: WO
		uint32 swResetAssertTxSelectorReg : 1; //Tx Selector registers SW enable Assert, by write '1', reset value: 0x0, access type: WO
		uint32 swResetAssertBeaconTimersReg : 1; //Beacon Timers SW enable Assert, by write '1', reset value: 0x0, access type: WO
	} bitFields;
} RegMacRabPasSwResetRegAssert_u;

/*REG_MAC_RAB_PAS_SW_RESET_REG2_ASSERT 0x54 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swResetAssertTxhTxc : 1; //TXH TXC SW enable Assert, by write '1', reset value: 0x0, access type: WO
		uint32 swResetAssertTxhTxcReg : 1; //TXH TXC registers SW enable Assert, by write '1', reset value: 0x0, access type: WO
		uint32 swResetAssertTxhNtd : 1; //TXH NTD SW enable Assert, by write '1', reset value: 0x0, access type: WO
		uint32 swResetAssertTxhNtdReg : 1; //TXH NTD registers SW enable Assert, by write '1', reset value: 0x0, access type: WO
		uint32 swResetAssertTxDmaAligner : 1; //Tx DMA aligner SW enable  Assert, by write '1', reset value: 0x0, access type: WO
		uint32 swResetAssertBaAnalizer : 1; //ba_analizer  SW enable  Assert, by write '1', reset value: 0x0, access type: WO
		uint32 swResetAssertOtfa : 1; //OTFA SW enable Assert, by write '1', reset value: 0x0, access type: WO
		uint32 swResetAssertOtfaReg : 1; //OTFA register file SW enable Assert, by write '1', reset value: 0x0, access type: WO
		uint32 swResetAssertLiberatorReg : 1; //sw_reAssert_liberator_reg Assert, by write '1', reset value: 0x0, access type: WO
		uint32 swResetAssertRxClas : 1; //Rx Classifier SW enable  Assert, by write '1', reset value: 0x0, access type: WO
		uint32 swResetAssertRxClasReg : 1; //Rx Classifier registers SW enable  Assert, by write '1', reset value: 0x0, access type: WO
		uint32 swResetAssertPsSetting : 1; //PS Setting SW enable  Assert, by write '1', reset value: 0x0, access type: WO
		uint32 swResetAssertPsSettingReg : 1; //PS Setting registers SW enable  Assert, by write '1', reset value: 0x0, access type: WO
		uint32 swResetAssertTfGenerator : 1; //TF Generator SW enable Assert, by write '1', reset value: 0x0, access type: WO
		uint32 swResetAssertTxSequencer : 1; //Tx Sequencer SW enable Assert, by write '1', reset value: 0x0, access type: WO
		uint32 swResetAssertPacExtrap : 1; //PAC extrapolator SW enable  Assert, by write '1', reset value: 0x0, access type: WO
		uint32 swResetAssertPacExtrapReg : 1; //PAC extrapolator registers SW enable  Assert, by write '1', reset value: 0x0, access type: WO
		uint32 swResetAssertTfGeneratorReg : 1; //TF Generator registers SW enable Assert, by write '1', reset value: 0x0, access type: WO
		uint32 swResetAssertGenriscTx : 1; //Genrisc Tx  SW enable  Assert, by write '1', reset value: 0x0, access type: WO
		uint32 swResetAssertGenriscRx : 1; //Genrisc Rx SW enable  Assert, by write '1', reset value: 0x0, access type: WO
		uint32 swResetAssertRxMsduParser : 1; //Rx MSDU Parser SW enable  Assert, by write '1', reset value: 0x0, access type: WO
		uint32 swResetAssertRxMsduParserReg : 1; //Rx MSDU Parser Registers SW enable  Assert, by write '1', reset value: 0x0, access type: WO
		uint32 swResetAssertRxMsduParserLogger : 1; //Rx MSDU Parser Logger SW enable  Assert, by write '1', reset value: 0x0, access type: WO
		uint32 swResetAssertRxCoordinator : 1; //Rx Coordinator SW enable  Assert, by write '1', reset value: 0x0, access type: WO
		uint32 swResetAssertMbfm : 1; //MAC BFM SW enable  Assert, by write '1', reset value: 0x0, access type: WO
		uint32 swResetAssertUlPostRx : 1; //UL Post Rx SW enable Assert, by write '1', reset value: 0x0, access type: WO
		uint32 swResetAssertUlPostRxReg : 1; //UL Post Rx registers SW enable Assert, by write '1', reset value: 0x0, access type: WO
		uint32 swResetAssertBsrc : 1; //BSRC SW enable Assert, by write '1', reset value: 0x0, access type: WO
		uint32 swResetAssertReserved : 4; //no description, reset value: 0x0, access type: WO
	} bitFields;
} RegMacRabPasSwResetReg2Assert_u;

/*REG_MAC_RAB_PAS_SW_RESET_REG_RD 0x58 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swResetRdTxPdAcc : 1; //TX_PD Acc SW enable , reset value: 0x1, access type: RO
		uint32 swResetRdTxPdAccReg : 1; //TX_PD Acc registers SW enable , reset value: 0x1, access type: RO
		uint32 swResetRdTxHc : 1; //TX_HC SW enable , reset value: 0x1, access type: RO
		uint32 swResetRdTxHcReg : 1; //TX_HC registers SW enable , reset value: 0x1, access type: RO
		uint32 swResetRdTim : 1; //TIM SW enable , reset value: 0x1, access type: RO
		uint32 swResetRdTimReg : 1; //TIM Reg block SW enable , reset value: 0x1, access type: RO
		uint32 swResetRdDel : 1; //Delia SW enable , reset value: 0x1, access type: RO
		uint32 swResetRdDelReg : 1; //Delia Reg block SW enable , reset value: 0x1, access type: RO
		uint32 swResetRdTcc : 1; //TCC SW enable , reset value: 0x1, access type: RO
		uint32 swResetRdReserved4 : 1; //SW enable reserved 4, reset value: 0x1, access type: RO
		uint32 swResetRdRxc : 1; //RxC SW enable , reset value: 0x1, access type: RO
		uint32 swResetRdRxcReg : 1; //RxC Reg block SW enable , reset value: 0x1, access type: RO
		uint32 swResetRdRxf : 1; //RxFC SW enable , reset value: 0x1, access type: RO
		uint32 swResetRdRxfReg : 1; //RxF Reg block SW enable , reset value: 0x1, access type: RO
		uint32 swResetRdRcc : 1; //RCC SW enable , reset value: 0x1, access type: RO
		uint32 swResetRdBeaconTimers : 1; //Beacon Timers SW registers enable , reset value: 0x1, access type: RO
		uint32 swResetRdRxd : 1; //RxC SW enable , reset value: 0x1, access type: RO
		uint32 swResetRdRxdReg : 1; //RxC Reg block SW enable , reset value: 0x1, access type: RO
		uint32 swResetRdDur : 1; //DUR SW enable , reset value: 0x1, access type: RO
		uint32 swResetRdDurReg : 1; //DUR Reg block SW enable , reset value: 0x1, access type: RO
		uint32 swResetRdRta : 1; //RTA SW enable , reset value: 0x1, access type: RO
		uint32 swResetRdBf : 1; //BF SW enable , reset value: 0x1, access type: RO
		uint32 swResetRdWep : 1; //WEP SW enable , reset value: 0x1, access type: RO
		uint32 swResetRdAggBuilder : 1; //AGG builder SW enable , reset value: 0x1, access type: RO
		uint32 swResetRdAggBuilderReg : 1; //AGG builder registers SW enable , reset value: 0x1, access type: RO
		uint32 swResetRdAddr2Index : 1; //Addr2index SW enable , reset value: 0x1, access type: RO
		uint32 swResetRdAddr2IndexReg : 1; //Addr2index registers SW enable , reset value: 0x1, access type: RO
		uint32 swResetRdTxHandler : 1; //Tx handler SW enable , reset value: 0x1, access type: RO
		uint32 swResetRdTxHandlerReg : 1; //Tx handler registers SW enable , reset value: 0x1, access type: RO
		uint32 swResetRdTxSelector : 1; //Tx Selector SW enable , reset value: 0x1, access type: RO
		uint32 swResetRdTxSelectorReg : 1; //Tx Selector registers SW enable , reset value: 0x1, access type: RO
		uint32 swResetRdBeaconTimersReg : 1; //Beacon Timers SW enable , reset value: 0x1, access type: RO
	} bitFields;
} RegMacRabPasSwResetRegRd_u;

/*REG_MAC_RAB_PAS_SW_RESET_REG2_RD 0x5C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swResetRdTxhTxc : 1; //TXH TXC SW enable , reset value: 0x1, access type: RO
		uint32 swResetRdTxhTxcReg : 1; //TXH TXC registers SW enable , reset value: 0x1, access type: RO
		uint32 swResetRdTxhNtd : 1; //TXH NTD SW enable , reset value: 0x1, access type: RO
		uint32 swResetRdTxhNtdReg : 1; //TXH NTD registers SW enable , reset value: 0x1, access type: RO
		uint32 swResetRdTxDmaAligner : 1; //Tx DMA aligner SW enable  , reset value: 0x1, access type: RO
		uint32 swResetRdBaAnalizer : 1; //ba_analizer  SW enable  , reset value: 0x1, access type: RO
		uint32 swResetRdOtfa : 1; //OTFA SW enable reset, reset value: 0x1, access type: RO
		uint32 swResetRdOtfaReg : 1; //OTFA register file SW enable, reset value: 0x1, access type: RO
		uint32 swResetRdLiberatorReg : 1; //sw_reset_liberator_reg , reset value: 0x1, access type: RO
		uint32 swResetRdRxClas : 1; //Rx Classifier SW enable  , reset value: 0x1, access type: RO
		uint32 swResetRdRxClasReg : 1; //Rx Classifier registers SW enable  , reset value: 0x1, access type: RO
		uint32 swResetRdPsSetting : 1; //PS Setting SW enable  , reset value: 0x1, access type: RO
		uint32 swResetRdPsSettingReg : 1; //PS Setting registers SW enable  , reset value: 0x1, access type: RO
		uint32 swResetRdTfGenerator : 1; //TF Generator SW Enable, reset value: 0x1, access type: RO
		uint32 swResetRdTxSequencer : 1; //Tx Sequencer SW enable , reset value: 0x1, access type: RO
		uint32 swResetRdPacExtrap : 1; //PAC extrapolator SW enable  , reset value: 0x1, access type: RO
		uint32 swResetRdPacExtrapReg : 1; //PAC extrapolator registers SW enable  , reset value: 0x1, access type: RO
		uint32 swResetRdTfGeneratorReg : 1; //TF Generator registers SW Enable, reset value: 0x1, access type: RO
		uint32 swResetRdGenriscTx : 1; //Genrisc Tx SW enable  , reset value: 0x1, access type: RO
		uint32 swResetRdGenriscRx : 1; //Genrisc Rx SW enable  , reset value: 0x1, access type: RO
		uint32 swResetRdRxMsduParser : 1; //Rx MSDU Parser SW enable, reset value: 0x1, access type: RO
		uint32 swResetRdRxMsduParserReg : 1; //Rx MSDU Parser Registers SW enable, reset value: 0x1, access type: RO
		uint32 swResetRdRxMsduParserLogger : 1; //Rx MSDU Parser Logger SW enable, reset value: 0x1, access type: RO
		uint32 swResetRdRxCoordinator : 1; //Rx Coordinator SW enable, reset value: 0x1, access type: RO
		uint32 swResetRdMbfm : 1; //MAC BFM SW enable, reset value: 0x1, access type: RO
		uint32 swResetRdUlPostRx : 1; //UL Post Rx SW Enable, reset value: 0x1, access type: RO
		uint32 swResetRdUlPostRxReg : 1; //UL Post Rx registers SW Enable, reset value: 0x1, access type: RO
		uint32 swResetRdBsrc : 1; //BSRC SW Enable, reset value: 0x1, access type: RO
		uint32 swResetRdReserved : 4; //no description, reset value: 0x1f, access type: RO
	} bitFields;
} RegMacRabPasSwResetReg2Rd_u;

/*REG_MAC_RAB_TX_DMA_ALIGNER_FIFO_CLEAR 0xD8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txDmaAlignerFifoClear : 1; //Tx DMA aligner FIFO clear, reset value: 0x0, access type: WO
		uint32 txDmaAlignerFcClear : 1; //Write 1 for clearing the Flow Control vector. , The Flow Control value of all User IDs shall be set to 0., reset value: 0x0, access type: WO
		uint32 reserved0 : 30;
	} bitFields;
} RegMacRabTxDmaAlignerFifoClear_u;

/*REG_MAC_RAB_TX_DMA_ALIGNER_FIFO_DEBUG 0xDC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txDmaAlignerFifoNumWords : 7; //Live value of Tx DMA aligner FIFO occupancy, reset value: 0x0, access type: RO
		uint32 reserved0 : 1;
		uint32 txDmaAlignerFifoEmpty : 1; //Tx DMA aligner FIFO empty, reset value: 0x1, access type: RO
		uint32 txDmaAlignerFifoFull : 1; //Tx DMA aligner FIFO full, reset value: 0x0, access type: RO
		uint32 reserved1 : 2;
		uint32 txDmaAlignerFifoOccupancyMax : 7; //Max value of Tx DMA aligner FIFO occupancy. , Cleared by the Read Pulse of this register, reset value: 0x0, access type: RO
		uint32 reserved2 : 13;
	} bitFields;
} RegMacRabTxDmaAlignerFifoDebug_u;

/*REG_MAC_RAB_TX_DMA_ALIGNER_DEBUG1 0xE0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 xbarToAlignerAccessCounter : 12; //xbar_to_aligner access counter, reset value: 0x0, access type: RO
		uint32 reserved0 : 4;
		uint32 alignerToSmcAccessCounter : 12; //aligner_to_smc access counter, reset value: 0x0, access type: RO
		uint32 reserved1 : 4;
	} bitFields;
} RegMacRabTxDmaAlignerDebug1_u;

/*REG_MAC_RAB_TX_DMA_ALIGNER_DEBUG2 0xE4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 xbarToAlignerTotalTransactions : 12; //xbar_to_aligner total transactions, reset value: 0x0, access type: RO
		uint32 reserved0 : 4;
		uint32 alignerToSmcTotalTransactions : 12; //aligner_to_smc total transactions, reset value: 0x0, access type: RO
		uint32 txDmaAlignerIdle : 1; //no description, reset value: 0x1, access type: RO
		uint32 reserved1 : 3;
	} bitFields;
} RegMacRabTxDmaAlignerDebug2_u;



#endif // _MAC_RAB_REGS_H_
