
/***********************************************************************************
File:				DlmRegs.h
Module:				dlm
SOC Revision:		843
Purpose:
Description:		This File was auto generated using SOC Online

************************************************************************************/
#ifndef _DLM_REGS_H_
#define _DLM_REGS_H_

/*---------------------------------------------------------------------------------
/						Registers Addresses													 
/----------------------------------------------------------------------------------*/
#include "HwMemoryMap.h"

#define DLM_BASE_ADDRESS                             MEMORY_MAP_UNIT_40_BASE_ADDRESS
#define	REG_DLM_DLM0_DESC_BASE_ADDR       (DLM_BASE_ADDRESS + 0x2800)
#define	REG_DLM_DLM0_DIS_CHECK_EN         (DLM_BASE_ADDRESS + 0x2804)
#define	REG_DLM_DLM0_GCLK_BYPASS          (DLM_BASE_ADDRESS + 0x2808)
#define	REG_DLM_DLM0_ERR_STATUS           (DLM_BASE_ADDRESS + 0x280C)
#define	REG_DLM_DLM0_ERR_STATUS_CLR       (DLM_BASE_ADDRESS + 0x2810)
#define	REG_DLM_DLM0_FLUSH_COUNTER        (DLM_BASE_ADDRESS + 0x2814)
#define	REG_DLM_DLM0_FLUSH_COUNTER_CLR    (DLM_BASE_ADDRESS + 0x2818)
#define	REG_DLM_DLM1_GCLK_BYPASS          (DLM_BASE_ADDRESS + 0x2828)
#define	REG_DLM_DLM1_FLUSH_COUNTER        (DLM_BASE_ADDRESS + 0x2834)
#define	REG_DLM_DLM1_FLUSH_COUNTER_CLR    (DLM_BASE_ADDRESS + 0x2838)
#define	REG_DLM_DLM2_DESC_BASE_ADDR       (DLM_BASE_ADDRESS + 0x2840)
#define	REG_DLM_DLM2_GCLK_BYPASS          (DLM_BASE_ADDRESS + 0x2848)
#define	REG_DLM_DLM2_FLUSH_COUNTER        (DLM_BASE_ADDRESS + 0x2854)
#define	REG_DLM_DLM2_FLUSH_COUNTER_CLR    (DLM_BASE_ADDRESS + 0x2858)
#define	REG_DLM_DLM3_GCLK_BYPASS          (DLM_BASE_ADDRESS + 0x2868)
#define	REG_DLM_DLM3_FLUSH_COUNTER        (DLM_BASE_ADDRESS + 0x2874)
#define	REG_DLM_DLM3_FLUSH_COUNTER_CLR    (DLM_BASE_ADDRESS + 0x2878)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_DLM_DLM0_DESC_BASE_ADDR 0x2800 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dlm0DescBaseAddr:22;	// Descriptors base address in the shared memory Pointers List TX base address 
		uint32 reserved0:10;
	} bitFields;
} RegDlmDlm0DescBaseAddr_u;

/*REG_DLM_DLM0_DIS_CHECK_EN 0x2804 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dlm0DisCheckEn:8;	// DLM Disable check enable
		uint32 reserved0:24;
	} bitFields;
} RegDlmDlm0DisCheckEn_u;

/*REG_DLM_DLM0_GCLK_BYPASS 0x2808 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dlm0GclkBypassEn:1;	// Gated clock bypass
		uint32 reserved0:31;
	} bitFields;
} RegDlmDlm0GclkBypass_u;

/*REG_DLM_DLM0_ERR_STATUS 0x280C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dlm0ErrPopDisabled:1;	// Pop of empty list - No PDs
		uint32 reserved0:31;
	} bitFields;
} RegDlmDlm0ErrStatus_u;

/*REG_DLM_DLM0_ERR_STATUS_CLR 0x2810 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dlm0ErrPopDisabledClr:1;	// Clears empty list pop error
		uint32 reserved0:31;
	} bitFields;
} RegDlmDlm0ErrStatusClr_u;

/*REG_DLM_DLM0_FLUSH_COUNTER 0x2814 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dlm0FlushCounter:16;	// Counts number of Flush requests done in the DLM
		uint32 reserved0:16;
	} bitFields;
} RegDlmDlm0FlushCounter_u;

/*REG_DLM_DLM0_FLUSH_COUNTER_CLR 0x2818 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dlm0FlushCounterClr:1;	// Clears flush counter
		uint32 reserved0:31;
	} bitFields;
} RegDlmDlm0FlushCounterClr_u;

/*REG_DLM_DLM1_GCLK_BYPASS 0x2828 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dlm1GclkBypassEn:1;	// Gated clock bypass
		uint32 reserved0:31;
	} bitFields;
} RegDlmDlm1GclkBypass_u;

/*REG_DLM_DLM1_FLUSH_COUNTER 0x2834 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dlm1FlushCounter:16;	// Counts number of Flush requests done in the DLM
		uint32 reserved0:16;
	} bitFields;
} RegDlmDlm1FlushCounter_u;

/*REG_DLM_DLM1_FLUSH_COUNTER_CLR 0x2838 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dlm1FlushCounterClr:1;	// Clears flush counter
		uint32 reserved0:31;
	} bitFields;
} RegDlmDlm1FlushCounterClr_u;

/*REG_DLM_DLM2_DESC_BASE_ADDR 0x2840 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dlm2DescBaseAddr:22;	// Descriptors base address in the shared memory Pointers List TX base address 
		uint32 reserved0:10;
	} bitFields;
} RegDlmDlm2DescBaseAddr_u;

/*REG_DLM_DLM2_GCLK_BYPASS 0x2848 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dlm2GclkBypassEn:1;	// Gated clock bypass
		uint32 reserved0:31;
	} bitFields;
} RegDlmDlm2GclkBypass_u;

/*REG_DLM_DLM2_FLUSH_COUNTER 0x2854 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dlm2FlushCounter:16;	// Counts number of Flush requests done in the DLM
		uint32 reserved0:16;
	} bitFields;
} RegDlmDlm2FlushCounter_u;

/*REG_DLM_DLM2_FLUSH_COUNTER_CLR 0x2858 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dlm2FlushCounterClr:1;	// Clears flush counter
		uint32 reserved0:31;
	} bitFields;
} RegDlmDlm2FlushCounterClr_u;

/*REG_DLM_DLM3_GCLK_BYPASS 0x2868 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dlm3GclkBypassEn:1;	// Gated clock bypass
		uint32 reserved0:31;
	} bitFields;
} RegDlmDlm3GclkBypass_u;

/*REG_DLM_DLM3_FLUSH_COUNTER 0x2874 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dlm3FlushCounter:16;	// Counts number of Flush requests done in the DLM
		uint32 reserved0:16;
	} bitFields;
} RegDlmDlm3FlushCounter_u;

/*REG_DLM_DLM3_FLUSH_COUNTER_CLR 0x2878 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dlm3FlushCounterClr:1;	// Clears flush counter
		uint32 reserved0:31;
	} bitFields;
} RegDlmDlm3FlushCounterClr_u;



#endif // _DLM_REGS_H_
