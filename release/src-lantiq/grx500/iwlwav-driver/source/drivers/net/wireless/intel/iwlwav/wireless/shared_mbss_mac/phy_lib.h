
/***************************************************************************************************/
/***************************************************************************************************/
/*   PHY library functions definition                                                              */

/***************************************************************************************************/
/***************************************************************************************************/

#ifndef _PHY_LIB_H_
#define _PHY_LIB_H_

#include "phy_access_definitions.h"

/* PHY address domain enum */
typedef enum
{
	PHY_RXTD = 0,
	PHY_RXFD,
	PHY_RXBE,
	PHY_TX
} PHY_Domain_e;



/* reset PHY block */
void PhyResetSet(PHY_Domain_e domain_to_reset);
/* release PHy block from reset */
void PhyResetRelease(PHY_Domain_e domain_to_reset);
/* digital loopback configuration */
void DigitalLpbk(void);
/* PHY HW reset */
void PHY_Reset(uint32 band, uint32 value);
/* disbale all PHY blocks */
void phy_disable(void);
/* release all PHY blocks from reset */
void phy_enable(void);
/* reset strobes */
void PhyResetStrobes(void);
/* start modem */
bool PhyStartModem(void);
/* Request GenRisc to halt prior to reset (intrupt6)*/
void PhyRiscSyncResetReq(bool *success);
/* stop PHY GenRisc */
void PhyStopRiscOperation(void);
/* start PHY GenRisc */
void PhyStartRiscOperation(void);
/* configure 11B modem */
void Phy_Config_802_11B(void);
/* start 11AX clock */
bool Phy_WLAN_StartClock_11ax(void);
/* reset FCSI central master */
void FCSI_CENTRAL_RESET(uint32 reset);
/* enable FCSI central master */
void FCSI_CENTRAL_ENABLE(uint32 enable);
/* FCSI central RF write clock div */
void FCSI_CENTRAL_WR_EXT_RF_CLKDIV(uint32 div);
/* FCSI central RF read clock div */
void FCSI_CENTRAL_RD_EXT_RF_CLKDIV(uint32 div);
/* FCSI central AFE write clock div */
void FCSI_CENTRAL_WR_AFE_CLKDIV(uint32 div);
/* FCSI central AFE read clock div */
void FCSI_CENTRAL_RD_AFE_CLKDIV(uint32 div);
/* read from central FCSI */
void FCSI_READ_TRANSACTION_CENTRAL(uint16 ms_access, uint16 rf_access, uint16 addr, uint16* rd_data, bool* success);
/* write to central FCSI */
void FCSI_WRITE_TRANSACTION_CENTRAL(uint16 ms_access, uint16 rf_access, uint16 addr, uint16 data, bool* success);
/* write to antenna FCSI */
void FCSI_WRITE_TRANSACTION_ANT(uint16 ms_access, uint16 rf_access, uint16 ant_mask, uint16 addr, uint16* data, bool* success);
/* read from antenna FCSI */
void FCSI_READ_TRANSACTION_ANT(uint16 ms_access, uint16 rf_access, uint16 ant_mask, uint16 addr, uint16* data, bool* success);
/* reset FCSI module */
void FCSI_RESET_CENTRAL(void);
void FCSI_RESET_ANT(void);
/* wait for FCSI busy field */
void FCSI_WAIT_FOR_CENTRAL_BUSY_FIELD(uint32 exp_val,uint32 timeout,bool * success);
void FCSI_WAIT_FOR_ANT_BUSY_FIELD(uint32 exp_val,uint32 timeout,bool * success);

/******************************************************************
* 	PHY_BAND_INIT()
*
* 	configiration of mac_phy_band_config register
*
* 	reg field:
*   b0_phy_active	  	[0]  1b Band0 Phy active
*   b1_phy_active  		[1]  1b Band1 Phy active
*   fcsi_cen_reset_n   	[2]  1b Central FCSI reset_n
*   fcsi_cen_block_en 	[3]  1b Central FCSI block enable
*   phy_antenna0_sel  	[4]  1b Antenna0 assignment (0 = Band0 / 1 = Band1)
*   phy_antenna1_sel 	[5]  1b Antenna1 assignment (0 = Band0 / 1 = Band1)
*   phy_antenna2_sel  	[6]  1b Antenna2 assignment (0 = Band0 / 1 = Band1)
*   phy_antenna3_sel 	[7]  1b Antenna3 assignment (0 = Band0 / 1 = Band1)
*******************************************************************/
void PHY_MAC_BAND_CONFIG_REG_INIT(void);
void PHY_SET_MAC_BAND(uint8 band);
void PHY_RFIC_RESET(void);
void PHY_RFIC_RESET_RELEASE(void);
void PHY_ENABLE_RX(void);
void PHY_PRE_CAL(void);
void PHY_POST_CAL(void);
void PHY_SET_VHT_NONAGGREGATE(uint8 enable);
void PHY_SET_RX_HALFBAND(uint8 halfbandtype);
void PHY_SET_RX_PRIMARY(uint8 rxprimarytype, uint8 antMask);
void PHY_SET_TX_HALFBAND(uint8 halfbandtype);
void PHY_SET_TX_PRIMARY(uint8 txprimarytype, uint8 halfbandtype, uint8 antMask);
void PHY_SET_11B_RX_ANT_SELECTION(uint8 antselection);
void PHY_SET_11B_TX_ANT_SELECTION(uint8 antSelection);
#ifndef ENET_INC_ARCH_WAVE600D2	
void PHY_SET_11B_ROUNDROBIN(uint8 rate);
#endif
void PHY_CLEAR_PHY_INTERRUPTS(void);
void PHY_SET_SPACELESS(void);
void PHY_READ_RSSI(uint8 *rssiValues, uint8 *energyEstNumOfSamples);
void PHY_SET_MAC_ANT_CONFIG(uint8 band, uint8 antMask);
void PHY_SET_TX_ANT_OPERATION_SET(uint8 opSet);
void PHY_SET_RX_ANT_OPERATION_SET(uint8 opSet);
void PHY_SET_FREQUENCY(uint32 freq, uint8 antMask);
void PHY_SET_BAND(uint8 band);
void PHY_SET_BSS_COLOR(uint8 vapId, uint8 bssColor);
void PHY_SECURE_WRITE(void);


/************** Temp for v.27 ***********************/
void PHY_GCLK_BYPASS_RESET(uint8 reset);
#endif //_PHY_LIB_H_

