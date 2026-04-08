
/***********************************************************************************
File:				TwtRegs.h
Module:				Twt
SOC Revision:		
Purpose:
Description:		This File was auto generated using SOC Online

************************************************************************************/
#ifndef _TWT_REGS_H_
#define _TWT_REGS_H_

/*---------------------------------------------------------------------------------
/						Registers Addresses													 
/----------------------------------------------------------------------------------*/
#include "HwMemoryMap.h"

#define TWT_BASE_ADDRESS                             MEMORY_MAP_UNIT_56_BASE_ADDRESS
#define	REG_TWT_TX_TWT_SW_HALT                (TWT_BASE_ADDRESS + 0x0)
#define	REG_TWT_TX_TWT_CFG                    (TWT_BASE_ADDRESS + 0x4)
#define	REG_TWT_TX_TWT_SP_GROUP_UPDATE        (TWT_BASE_ADDRESS + 0x8)
#define	REG_TWT_TX_TWT_SP_GROUP_START_TSF     (TWT_BASE_ADDRESS + 0xC)
#define	REG_TWT_TX_TWT_SP_GROUP_LOW_PHASE     (TWT_BASE_ADDRESS + 0x10)
#define	REG_TWT_TX_TWT_SP_STA_UPDATE          (TWT_BASE_ADDRESS + 0x14)
#define	REG_TWT_TX_TWT_SP_GROUP_VALID         (TWT_BASE_ADDRESS + 0x18)
#define	REG_TWT_TX_TWT_SP_GROUP_DEACTIVATE    (TWT_BASE_ADDRESS + 0x1C)
#define	REG_TWT_TX_TWT_FSM_STATUS             (TWT_BASE_ADDRESS + 0x20)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_TWT_TX_TWT_SW_HALT 0x0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 twtSwHalt : 1; //Enables update of twt_avail field, reset value: 0x1, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegTwtTxTwtSwHalt_u;

/*REG_TWT_TX_TWT_CFG 0x4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 twtAvailUpdateEn : 1; //Enables update of twt_avail field, reset value: 0x1, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegTwtTxTwtCfg_u;

/*REG_TWT_TX_TWT_SP_GROUP_UPDATE 0x8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 twtSpHighLimit : 19; //High phase of the SP. Values are in ~32uS resolution., reset value: 0x0, access type: RW
		uint32 reserved0 : 5;
		uint32 twtSpGroupIdx : 5; //SP Group index, reset value: 0x0, access type: RW
		uint32 reserved1 : 2;
		uint32 twtSpUpdateDone : 1; //Instruction done indication, reset value: 0x1, access type: RO
	} bitFields;
} RegTwtTxTwtSpGroupUpdate_u;

/*REG_TWT_TX_TWT_SP_GROUP_START_TSF 0xC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 twtSpStartTsf : 32; //Start TSF value. 32 LSB bit only (Out of the full 64 bits), reset value: 0x0, access type: RW
	} bitFields;
} RegTwtTxTwtSpGroupStartTsf_u;

/*REG_TWT_TX_TWT_SP_GROUP_LOW_PHASE 0x10 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 twtSpLowLimit : 19; //Low phase of the SP. Values are in ~32uS resolution., reset value: 0x0, access type: RW
		uint32 reserved0 : 1;
		uint32 twtAvailUpdateValue : 1; //twt_avail field write value during workflow., reset value: 0x0, access type: RW
		uint32 reserved1 : 11;
	} bitFields;
} RegTwtTxTwtSpGroupLowPhase_u;

/*REG_TWT_TX_TWT_SP_STA_UPDATE 0x14 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 twtSpStaGroupIdx : 5; //SP Group index, reset value: 0x0, access type: RW
		uint32 reserved0 : 3;
		uint32 twtSpStaIdx : 8; //STA index, reset value: 0x0, access type: RW
		uint32 twtSpStaValue : 1; //Connectd/Disconnect STA from group, reset value: 0x0, access type: RW
		uint32 reserved1 : 14;
		uint32 twtSpStaUpdateDone : 1; //Instruction done indication, reset value: 0x1, access type: RO
	} bitFields;
} RegTwtTxTwtSpStaUpdate_u;

/*REG_TWT_TX_TWT_SP_GROUP_VALID 0x18 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 twtSpGroupValid : 32; //Enabled SP groups, reset value: 0x0, access type: RO
	} bitFields;
} RegTwtTxTwtSpGroupValid_u;

/*REG_TWT_TX_TWT_SP_GROUP_DEACTIVATE 0x1C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 twtSpDeactivateGroupIdx : 5; //SP group index, reset value: 0x0, access type: RW
		uint32 reserved0 : 26;
		uint32 twtSpDeactivateDone : 1; //Instruction done indication, reset value: 0x1, access type: RO
	} bitFields;
} RegTwtTxTwtSpGroupDeactivate_u;

/*REG_TWT_TX_TWT_FSM_STATUS 0x20 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 twtSpUpdateSm : 3; //twt_sp_update SM state, reset value: 0x0, access type: RO
		uint32 twtSpGroupInitSm : 2; //twt_sp_group_init SM state, reset value: 0x0, access type: RO
		uint32 twtSpGroupClearSm : 3; //twt_sp_group_clear SM state, reset value: 0x0, access type: RO
		uint32 reserved0 : 24;
	} bitFields;
} RegTwtTxTwtFsmStatus_u;



#endif // _TWT_REGS_H_
