
/***********************************************************************************
File:				EmeraldEnvRegs.h
Module:				emeraldEnv
SOC Revision:		843
Purpose:
Description:		This File was auto generated using SOC Online

************************************************************************************/
#ifndef _EMERALD_ENV_REGS_H_
#define _EMERALD_ENV_REGS_H_

/*---------------------------------------------------------------------------------
/						Registers Addresses													 
/----------------------------------------------------------------------------------*/
#include "HwMemoryMap.h"

#define EMERALD_ENV_BASE_ADDRESS                             MEMORY_MAP_UNIT_1_BASE_ADDRESS
#define	REG_EMERALD_ENV_SYS_IF_GCLK                    (EMERALD_ENV_BASE_ADDRESS + 0x0)
#define	REG_EMERALD_ENV_SYS_IF_SW_RESET                (EMERALD_ENV_BASE_ADDRESS + 0x4)
#define	REG_EMERALD_ENV_UART_WR                        (EMERALD_ENV_BASE_ADDRESS + 0xC)
#define	REG_EMERALD_ENV_UART_RD                        (EMERALD_ENV_BASE_ADDRESS + 0x10)
#define	REG_EMERALD_ENV_UART_DIVR                      (EMERALD_ENV_BASE_ADDRESS + 0x14)
#define	REG_EMERALD_ENV_UART_FWL_REG                   (EMERALD_ENV_BASE_ADDRESS + 0x18)
#define	REG_EMERALD_ENV_UART_INT_REG                   (EMERALD_ENV_BASE_ADDRESS + 0x1C)
#define	REG_EMERALD_ENV_UART_CONTROL_REG               (EMERALD_ENV_BASE_ADDRESS + 0x20)
#define	REG_EMERALD_ENV_UART_NOISE_COUNT               (EMERALD_ENV_BASE_ADDRESS + 0x24)
#define	REG_EMERALD_ENV_UART_IDLE_CNT_MAX              (EMERALD_ENV_BASE_ADDRESS + 0x28)
#define	REG_EMERALD_ENV_UART_COUNT_FIF                 (EMERALD_ENV_BASE_ADDRESS + 0x2C)
#define	REG_EMERALD_ENV_UART_INT_MODE                  (EMERALD_ENV_BASE_ADDRESS + 0x30)
#define	REG_EMERALD_ENV_PLL_DIV                        (EMERALD_ENV_BASE_ADDRESS + 0x40)
#define	REG_EMERALD_ENV_PLL_EN                         (EMERALD_ENV_BASE_ADDRESS + 0x44)
#define	REG_EMERALD_ENV_PLL_CTL                        (EMERALD_ENV_BASE_ADDRESS + 0x48)
#define	REG_EMERALD_ENV_SYS_CLR_DIVR                   (EMERALD_ENV_BASE_ADDRESS + 0x4C)
#define	REG_EMERALD_ENV_PLL_LOCK                       (EMERALD_ENV_BASE_ADDRESS + 0x50)
#define	REG_EMERALD_ENV_CPU_ENABLE                     (EMERALD_ENV_BASE_ADDRESS + 0x54)
#define	REG_EMERALD_ENV_CLOCK_SELECT                   (EMERALD_ENV_BASE_ADDRESS + 0x58)
#define	REG_EMERALD_ENV_SYSTEM_INFO                    (EMERALD_ENV_BASE_ADDRESS + 0x60)
#define	REG_EMERALD_ENV_SYSTEM_CFG                     (EMERALD_ENV_BASE_ADDRESS + 0x64)
#define	REG_EMERALD_ENV_SECURE_WRITE                   (EMERALD_ENV_BASE_ADDRESS + 0x68)
#define	REG_EMERALD_ENV_GPIO_SEL                       (EMERALD_ENV_BASE_ADDRESS + 0x70)
#define	REG_EMERALD_ENV_MIPS_CTL                       (EMERALD_ENV_BASE_ADDRESS + 0x74)
#define	REG_EMERALD_ENV_TOP_MUX                        (EMERALD_ENV_BASE_ADDRESS + 0x78)
#define	REG_EMERALD_ENV_BB_OPMODE_STATUS               (EMERALD_ENV_BASE_ADDRESS + 0x7C)
#define	REG_EMERALD_ENV_GPIO_OEN                       (EMERALD_ENV_BASE_ADDRESS + 0x80)
#define	REG_EMERALD_ENV_GPIO_DAT                       (EMERALD_ENV_BASE_ADDRESS + 0x84)
#define	REG_EMERALD_ENV_GPIO_STB_LEN                   (EMERALD_ENV_BASE_ADDRESS + 0x88)
#define	REG_EMERALD_ENV_GPO_REG                        (EMERALD_ENV_BASE_ADDRESS + 0x8C)
#define	REG_EMERALD_ENV_GPIO_ACTIVE                    (EMERALD_ENV_BASE_ADDRESS + 0x90)
#define	REG_EMERALD_ENV_UCPU_32K_BLOCKS                (EMERALD_ENV_BASE_ADDRESS + 0xAC)
#define	REG_EMERALD_ENV_LS_MODE                        (EMERALD_ENV_BASE_ADDRESS + 0xB0)
#define	REG_EMERALD_ENV_CPU_SPARE_GC_CONTROL           (EMERALD_ENV_BASE_ADDRESS + 0xB8)
#define	REG_EMERALD_ENV_CPU_GPR                        (EMERALD_ENV_BASE_ADDRESS + 0xC0)
#define	REG_EMERALD_ENV_CPU_GPO                        (EMERALD_ENV_BASE_ADDRESS + 0xC4)
#define	REG_EMERALD_ENV_CPU_GPI1                       (EMERALD_ENV_BASE_ADDRESS + 0xC8)
#define	REG_EMERALD_ENV_CPU_GPI2                       (EMERALD_ENV_BASE_ADDRESS + 0xCC)
#define	REG_EMERALD_ENV_RAM_CTL_ADDR                   (EMERALD_ENV_BASE_ADDRESS + 0xD0)
#define	REG_EMERALD_ENV_M4K_RAMS_RM                    (EMERALD_ENV_BASE_ADDRESS + 0xD4)
#define	REG_EMERALD_ENV_IRAM_RM                        (EMERALD_ENV_BASE_ADDRESS + 0xD8)
#define	REG_EMERALD_ENV_ERAM_RM                        (EMERALD_ENV_BASE_ADDRESS + 0xDC)
#define	REG_EMERALD_ENV_CPU_GPR1                       (EMERALD_ENV_BASE_ADDRESS + 0xE0)
#define	REG_EMERALD_ENV_CPU_GPR2                       (EMERALD_ENV_BASE_ADDRESS + 0xE4)
#define	REG_EMERALD_ENV_CPU_GPR3                       (EMERALD_ENV_BASE_ADDRESS + 0xE8)
#define	REG_EMERALD_ENV_SPARE_CONFIG_MAC_IN            (EMERALD_ENV_BASE_ADDRESS + 0xF0)
#define	REG_EMERALD_ENV_SPARE_CONFIG_MAC_OUT           (EMERALD_ENV_BASE_ADDRESS + 0xF4)
#define	REG_EMERALD_ENV_SWDOG_BOOT_CNT                 (EMERALD_ENV_BASE_ADDRESS + 0x100)
#define	REG_EMERALD_ENV_SWDOG_INT_CNT                  (EMERALD_ENV_BASE_ADDRESS + 0x104)
#define	REG_EMERALD_ENV_SWDOG_MASK                     (EMERALD_ENV_BASE_ADDRESS + 0x108)
#define	REG_EMERALD_ENV_SWDOG_BOOT_CTR                 (EMERALD_ENV_BASE_ADDRESS + 0x10C)
#define	REG_EMERALD_ENV_SWDOG_INT_CTR                  (EMERALD_ENV_BASE_ADDRESS + 0x110)
#define	REG_EMERALD_ENV_MAC_TO_PHY_RESET_N             (EMERALD_ENV_BASE_ADDRESS + 0x114)
#define	REG_EMERALD_ENV_IRQ_MASK                       (EMERALD_ENV_BASE_ADDRESS + 0x140)
#define	REG_EMERALD_ENV_IRQ_SEC_RF                     (EMERALD_ENV_BASE_ADDRESS + 0x144)
#define	REG_EMERALD_ENV_IRQ_MAP_00                     (EMERALD_ENV_BASE_ADDRESS + 0x148)
#define	REG_EMERALD_ENV_IRQ_MAP_01                     (EMERALD_ENV_BASE_ADDRESS + 0x14C)
#define	REG_EMERALD_ENV_IRQ_MAP_02                     (EMERALD_ENV_BASE_ADDRESS + 0x150)
#define	REG_EMERALD_ENV_IRQ_MAP_03                     (EMERALD_ENV_BASE_ADDRESS + 0x154)
#define	REG_EMERALD_ENV_IRQ_MAP_04                     (EMERALD_ENV_BASE_ADDRESS + 0x158)
#define	REG_EMERALD_ENV_IRQ_MAP_05                     (EMERALD_ENV_BASE_ADDRESS + 0x15C)
#define	REG_EMERALD_ENV_IRQ_MAPPED                     (EMERALD_ENV_BASE_ADDRESS + 0x160)
#define	REG_EMERALD_ENV_IRQ_UNMAPPED                   (EMERALD_ENV_BASE_ADDRESS + 0x164)
#define	REG_EMERALD_ENV_TIMERS_IRQ_EN                  (EMERALD_ENV_BASE_ADDRESS + 0x174)
#define	REG_EMERALD_ENV_TIMER0_PRESCALER               (EMERALD_ENV_BASE_ADDRESS + 0x178)
#define	REG_EMERALD_ENV_TIMER1_PRESCALER               (EMERALD_ENV_BASE_ADDRESS + 0x17C)
#define	REG_EMERALD_ENV_TIMER0_MAX_COUNT               (EMERALD_ENV_BASE_ADDRESS + 0x180)
#define	REG_EMERALD_ENV_TIMER1_MAX_COUNT               (EMERALD_ENV_BASE_ADDRESS + 0x184)
#define	REG_EMERALD_ENV_TIMER2_MAX_COUNT               (EMERALD_ENV_BASE_ADDRESS + 0x188)
#define	REG_EMERALD_ENV_TIMER3_MAX_COUNT               (EMERALD_ENV_BASE_ADDRESS + 0x18C)
#define	REG_EMERALD_ENV_TIMER0_CTR                     (EMERALD_ENV_BASE_ADDRESS + 0x190)
#define	REG_EMERALD_ENV_TIMER1_CTR                     (EMERALD_ENV_BASE_ADDRESS + 0x194)
#define	REG_EMERALD_ENV_TIMER2_CTR                     (EMERALD_ENV_BASE_ADDRESS + 0x198)
#define	REG_EMERALD_ENV_TIMER3_CTR                     (EMERALD_ENV_BASE_ADDRESS + 0x19C)
#define	REG_EMERALD_ENV_TIMERS_STATUS                  (EMERALD_ENV_BASE_ADDRESS + 0x1A0)
#define	REG_EMERALD_ENV_TIMERS_CONFIG                  (EMERALD_ENV_BASE_ADDRESS + 0x1A4)
#define	REG_EMERALD_ENV_TIMER2_PRESCALER               (EMERALD_ENV_BASE_ADDRESS + 0x1A8)
#define	REG_EMERALD_ENV_TIMER3_PRESCALER               (EMERALD_ENV_BASE_ADDRESS + 0x1AC)
#define	REG_EMERALD_ENV_IC_HIT_CNT                     (EMERALD_ENV_BASE_ADDRESS + 0x1C0)
#define	REG_EMERALD_ENV_IC_MISS_CNT                    (EMERALD_ENV_BASE_ADDRESS + 0x1C4)
#define	REG_EMERALD_ENV_DC_HIT_CNT                     (EMERALD_ENV_BASE_ADDRESS + 0x1C8)
#define	REG_EMERALD_ENV_DC_MISS_CNT                    (EMERALD_ENV_BASE_ADDRESS + 0x1CC)
#define	REG_EMERALD_ENV_MIPS_SI_INT_STATUS             (EMERALD_ENV_BASE_ADDRESS + 0x1D0)
#define	REG_EMERALD_ENV_MIPS_SI_INT_EN                 (EMERALD_ENV_BASE_ADDRESS + 0x1D4)
#define	REG_EMERALD_ENV_HW_EVENTS_INT_STATUS           (EMERALD_ENV_BASE_ADDRESS + 0x1D8)
#define	REG_EMERALD_ENV_HW_ERRORS_INT_STATUS           (EMERALD_ENV_BASE_ADDRESS + 0x1DC)
#define	REG_EMERALD_ENV_HW_EVENTS_INT_EN               (EMERALD_ENV_BASE_ADDRESS + 0x1E0)
#define	REG_EMERALD_ENV_HW_ERRORS_INT_EN               (EMERALD_ENV_BASE_ADDRESS + 0x1E4)
#define	REG_EMERALD_ENV_HW_EVENTS_INT_MASKED_STATUS    (EMERALD_ENV_BASE_ADDRESS + 0x1E8)
#define	REG_EMERALD_ENV_HW_ERRORS_INT_MASKED_STATUS    (EMERALD_ENV_BASE_ADDRESS + 0x1EC)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_EMERALD_ENV_SYS_IF_GCLK 0x0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 uartRxGclk:1;	// UART Rx gated clock
		uint32 uartRxFifoGclk:1;	// UART Rx FIFO gated clock
		uint32 uartTxGclk:1;	// UART Tx gated clock
		uint32 uartTxFifoGclk:1;	// UART Tx FIFO gated clock
		uint32 uartCdivGclk:1;	// UART Clock divider gated clock
		uint32 cacheCtrsGclk:1;	// Cache performance counters gated clock
		uint32 timersGclk:1;	// Timers gated clock
		uint32 testBusFifoGclk:1;	// Test bus FIFO gated clock
		uint32 reserved0:24;
	} bitFields;
} RegEmeraldEnvSysIfGclk_u;

/*REG_EMERALD_ENV_SYS_IF_SW_RESET 0x4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 uartRxSwReset:1;	// UART Rx SW reset
		uint32 uartRxFifoSwReset:1;	// UART Rx FIFO  SW reset
		uint32 uartTxSwReset:1;	// UART Tx SW reset
		uint32 uartTxFifoSwReset:1;	// UART Tx FIFO SW reset
		uint32 uartCdivSwReset:1;	// UART Clock divider SW reset
		uint32 cacheCtrsSwReset:1;	// Cache perfomance counters SW reset
		uint32 timersGclkSwReset:1;	// Timers SW reset
		uint32 testBusFifoSwReset:1;	// Test bus FIFO SW reset
		uint32 reserved0:24;
	} bitFields;
} RegEmeraldEnvSysIfSwReset_u;

/*REG_EMERALD_ENV_UART_WR 0xC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 uartWriteData:8;	// UART Write data
		uint32 reserved0:24;
	} bitFields;
} RegEmeraldEnvUartWr_u;

/*REG_EMERALD_ENV_UART_RD 0x10 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 uartReadData:8;	// UART read data
		uint32 uartReadParityErrorBit:1;	// UART read parity error bit
		uint32 reserved0:23;
	} bitFields;
} RegEmeraldEnvUartRd_u;

/*REG_EMERALD_ENV_UART_DIVR 0x14 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 uartClockDivisionRatio:14;	// UART clock division ratio
		uint32 reserved0:18;
	} bitFields;
} RegEmeraldEnvUartDivr_u;

/*REG_EMERALD_ENV_UART_FWL_REG 0x18 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 uartRxFifoWaterLevel:5;	// UART Rx. FIFO water level
		uint32 uartTxFifoWaterLevel:5;	// UART Tx. FIFO water level
		uint32 reserved0:22;
	} bitFields;
} RegEmeraldEnvUartFwlReg_u;

/*REG_EMERALD_ENV_UART_INT_REG 0x1C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 uartOverrunIrqEnable:1;	// UART overrun IRQ enable
		uint32 uartFramingErrorIrqEnable:1;	// UART framing error IRQ enable
		uint32 uartParityErrorIrqEnable:1;	// UART parity error IRQ enable
		uint32 uartReadCollapseBufferIrqEnable:1;	// UART read collapse buffer IRQ enable
		uint32 uartWriteCollapseBufferIrqEnable:1;	// UART write collapse buffer IRQ enable
		uint32 uartReadFifoWaterLevelIrqEnable:1;	// UART read FIFO water level IRQ enable
		uint32 uartWriteFifoWaterLevelIrqEnable:1;	// UART write FIFO water level IRQ enable
		uint32 uartIdleIrqEnable:1;	// UART idle IRQ enable
		uint32 uartOverrunIrq:1;	// UART overrun IRQ
		uint32 uartFramingErrorIrq:1;	// UART framing error IRQ
		uint32 uartParityErrorIrq:1;	// UART parity error IRQ
		uint32 uartReadCollapseBufferIrq:1;	// UART read collapse buffer IRQ
		uint32 uartWriteCollapseBufferIrq:1;	// UART write collapse buffer IRQ
		uint32 uartReadFifoWaterLevelIrq:1;	// UART read FIFO water level IRQ
		uint32 uartWriteFifoWaterLevelIrq:1;	// UART write FIFO water level IRQ
		uint32 uartIdleIrq:1;	// UART idle IRQ
		uint32 reserved0:16;
	} bitFields;
} RegEmeraldEnvUartIntReg_u;

/*REG_EMERALD_ENV_UART_CONTROL_REG 0x20 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 uartTxOutputEnable:1;	// UART Tx. Output enable
		uint32 uartRxParityMode:2;	// UART Rx. Parity mode
		uint32 uartTxParityMode:2;	// UART Tx. Parity mode
		uint32 uartRxStop:1;	// UART Rx. stop
		uint32 uartTxStop:1;	// UART Tx. stop
		uint32 uartLoopbackMode:1;	// UART loopback mode
		uint32 reserved0:24;
	} bitFields;
} RegEmeraldEnvUartControlReg_u;

/*REG_EMERALD_ENV_UART_NOISE_COUNT 0x24 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 uartNoiseCounterClear:1;	// UART Noise counter
		uint32 uartNoiseCounter:8;	// UART Noise counter
		uint32 reserved0:23;
	} bitFields;
} RegEmeraldEnvUartNoiseCount_u;

/*REG_EMERALD_ENV_UART_IDLE_CNT_MAX 0x28 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 uartRxIdleCounter:8;	// UART Rx. idle counter
		uint32 reserved0:24;
	} bitFields;
} RegEmeraldEnvUartIdleCntMax_u;

/*REG_EMERALD_ENV_UART_COUNT_FIF 0x2C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 uartBytesInRxFifo:5;	// UART bytes in Rx. FIFO
		uint32 uartBytesInTxFifo:5;	// UART bytes in Tx. FIFO
		uint32 reserved0:22;
	} bitFields;
} RegEmeraldEnvUartCountFif_u;

/*REG_EMERALD_ENV_UART_INT_MODE 0x30 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 uartTxInterruptMode:1;	// UART Tx. interrupt mode
		uint32 reserved0:31;
	} bitFields;
} RegEmeraldEnvUartIntMode_u;

/*REG_EMERALD_ENV_PLL_DIV 0x40 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 testClkDivD2A:2;	// Test clock divider
		uint32 wlanClkDivD2A:2;	// WLAN clock divider
		uint32 adcClkDivD2A:2;	// ADC clock divider
		uint32 dacClkDivD2A:2;	// DAC clock divider
		uint32 ssiClkDivD2A:2;	// SSI clock divider
		uint32 reserved0:6;
		uint32 lcpllDividerIntD2A:8;	// PLL integer divider
		uint32 reserved1:8;
	} bitFields;
} RegEmeraldEnvPllDiv_u;

/*REG_EMERALD_ENV_PLL_EN 0x44 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 testClkPdnD2A:1;	// Test clock power-down
		uint32 wlanClkPdnD2A:1;	// WLAN clock power-down
		uint32 adcClkPdnD2A:1;	// ADC clock power-down
		uint32 dacClkPdnD2A:1;	// DAC clock power-down
		uint32 ssiClkPdnD2A:1;	// SSI clock power-down
		uint32 reserved0:27;
	} bitFields;
} RegEmeraldEnvPllEn_u;

/*REG_EMERALD_ENV_PLL_CTL 0x48 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:7;
		uint32 lcpllPupD2A:1;
		uint32 lcpllResetnD2A:1;	// PLL reset_n
		uint32 lcpllBypassD2A:1;	// PLL LDO disable
		uint32 reserved1:3;
		uint32 fcsiLdoPdD2A:1;	// FCSI LDO powerdown
		uint32 usbSpndWupD2A:1;	// USB suspend wakeup
		uint32 reserved2:17;
	} bitFields;
} RegEmeraldEnvPllCtl_u;

/*REG_EMERALD_ENV_SYS_CLR_DIVR 0x4C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 slowClockDivr:18;	// Slow clock division ratio
		uint32 fastClockDivr:4;	// Fast clock division ratio
		uint32 reserved0:10;
	} bitFields;
} RegEmeraldEnvSysClrDivr_u;

/*REG_EMERALD_ENV_PLL_LOCK 0x50 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pllLock:1;	// PLL lock
		uint32 reserved0:31;
	} bitFields;
} RegEmeraldEnvPllLock_u;

/*REG_EMERALD_ENV_CPU_ENABLE 0x54 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 cpuEnable:1;	// CPU Enable
		uint32 reserved0:31;
	} bitFields;
} RegEmeraldEnvCpuEnable_u;

/*REG_EMERALD_ENV_CLOCK_SELECT 0x58 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 cpuClockSelect:2;	// CPU Clock select
		uint32 reserved0:30;
	} bitFields;
} RegEmeraldEnvClockSelect_u;

/*REG_EMERALD_ENV_SYSTEM_INFO 0x60 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 chipId:16;	// Chip ID
		uint32 bootMode:2;	// Boot mode
		uint32 upperLowerCpu:1;	// Upper/lower CPU
		uint32 reserved0:13;
	} bitFields;
} RegEmeraldEnvSystemInfo_u;

/*REG_EMERALD_ENV_SYSTEM_CFG 0x64 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 abortCounterEnable:1;	// Abort counter enable
		uint32 chainedDebugMode:1;	// Chained debug mode
		uint32 reserved0:30;
	} bitFields;
} RegEmeraldEnvSystemCfg_u;

/*REG_EMERALD_ENV_SECURE_WRITE 0x68 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 secureWrite:16;	// Secure write
		uint32 reserved0:16;
	} bitFields;
} RegEmeraldEnvSecureWrite_u;

/*REG_EMERALD_ENV_GPIO_SEL 0x70 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 gpioSel:3;	// Clock Select on GPIO[0]
		uint32 clkgenClkOnGpio:1;	// Enable ClkGen output to GPIO[0]
		uint32 genriscGpoSelected:1;	// GenRISC GPO selected
		uint32 selectPhySignals:1;	// Select Phy signals
		uint32 gpioDivBypass:1;	// divider bypass
		uint32 reserved0:25;
	} bitFields;
} RegEmeraldEnvGpioSel_u;

/*REG_EMERALD_ENV_MIPS_CTL 0x74 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bootModeReq:2;	// Boot mode request , 00 = Boot from IRAM , 01 = Boot from SHRAM , 10 = reserved (formerly boot from ROM) , 11 = Debug mode
		uint32 reserved0:2;
		uint32 mipsSwResetNReg:1;	// "1" = MIPS running , "0" = MIPS in reset
		uint32 reserved1:27;
	} bitFields;
} RegEmeraldEnvMipsCtl_u;

/*REG_EMERALD_ENV_TOP_MUX 0x78 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 lvdsXoSel:1;	// LVDS XO select
		uint32 modemXoSel:1;	// MODEM XO select
		uint32 modemPreDiv:1;	// MODEM pre-divider
		uint32 cpuPhyDiv:2;	// Phy divider
		uint32 cpuPhyDivEnable:1;	// Phy divider enable
		uint32 cpuDividersXoSel:1;	// CPU dividers XO select
		uint32 reserved0:25;
	} bitFields;
} RegEmeraldEnvTopMux_u;

/*REG_EMERALD_ENV_BB_OPMODE_STATUS 0x7C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bbOpmodeStatus:16;	// bb_opmode_status
		uint32 reserved0:16;
	} bitFields;
} RegEmeraldEnvBbOpmodeStatus_u;

/*REG_EMERALD_ENV_GPIO_OEN 0x80 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 gpioOutputEnable:17;	// GPIO Output enable
		uint32 reserved0:15;
	} bitFields;
} RegEmeraldEnvGpioOen_u;

/*REG_EMERALD_ENV_GPIO_DAT 0x84 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 gpioData:16;	// GPIO Data
		uint32 reserved0:16;
	} bitFields;
} RegEmeraldEnvGpioDat_u;

/*REG_EMERALD_ENV_GPIO_STB_LEN 0x88 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 gpioStrobeLength:5;	// GPIO sttobe length
		uint32 reserved0:27;
	} bitFields;
} RegEmeraldEnvGpioStbLen_u;

/*REG_EMERALD_ENV_GPO_REG 0x8C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 gpoRegister:16;	// GPO register
		uint32 reserved0:16;
	} bitFields;
} RegEmeraldEnvGpoReg_u;

/*REG_EMERALD_ENV_GPIO_ACTIVE 0x90 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 gpioActive:1;	// GPIO active
		uint32 reserved0:31;
	} bitFields;
} RegEmeraldEnvGpioActive_u;

/*REG_EMERALD_ENV_UCPU_32K_BLOCKS 0xAC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ucpu32KBlocks:3;	// Upper CPU 32K blocks count in ERAM
		uint32 reserved0:21;
		uint32 total32KBlocksAvailable:3;
		uint32 reserved1:5;
	} bitFields;
} RegEmeraldEnvUcpu32KBlocks_u;

/*REG_EMERALD_ENV_LS_MODE 0xB0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 lsForceDisable:1;	// Force light sleep disabled
		uint32 lsForceEnable:1;	// Force light sleep enabled
		uint32 lsDynamic:1;	// Light sleep - dynamic mode
		uint32 reserved0:29;
	} bitFields;
} RegEmeraldEnvLsMode_u;

/*REG_EMERALD_ENV_CPU_SPARE_GC_CONTROL 0xB8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 cpuSpareGcBypass:4;	// CPU spare gates gated clock bypass
		uint32 cpuSpareGcEnable:4;	// CPU spare gates gated clock enable
		uint32 reserved0:24;
	} bitFields;
} RegEmeraldEnvCpuSpareGcControl_u;

/*REG_EMERALD_ENV_CPU_GPR 0xC0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 cpuGpr:32;	// CPU GP register
	} bitFields;
} RegEmeraldEnvCpuGpr_u;

/*REG_EMERALD_ENV_CPU_GPO 0xC4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 cpuGpo:16;	// CPU GP output register
		uint32 reserved0:16;
	} bitFields;
} RegEmeraldEnvCpuGpo_u;

/*REG_EMERALD_ENV_CPU_GPI1 0xC8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 cpuGpi1:16;	// CPU GP input register 1
		uint32 reserved0:16;
	} bitFields;
} RegEmeraldEnvCpuGpi1_u;

/*REG_EMERALD_ENV_CPU_GPI2 0xCC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 cpuGpi2:16;	// CPU GP input register 2
		uint32 reserved0:16;
	} bitFields;
} RegEmeraldEnvCpuGpi2_u;

/*REG_EMERALD_ENV_RAM_CTL_ADDR 0xD0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:1;
		uint32 ramTestMode:1;	// ram_test_mode
		uint32 memGlobalRm:2;	// mem_global_rm
		uint32 reserved1:28;
	} bitFields;
} RegEmeraldEnvRamCtlAddr_u;

/*REG_EMERALD_ENV_M4K_RAMS_RM 0xD4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 m4KRamsRm:26;	// MIPS 4Kc RAMs RM value
		uint32 reserved0:6;
	} bitFields;
} RegEmeraldEnvM4KRamsRm_u;

/*REG_EMERALD_ENV_IRAM_RM 0xD8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iramRm:30;	// IRAM RM value
		uint32 reserved0:2;
	} bitFields;
} RegEmeraldEnvIramRm_u;

/*REG_EMERALD_ENV_ERAM_RM 0xDC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 eramRm:22;	// External RAM RM value
		uint32 reserved0:10;
	} bitFields;
} RegEmeraldEnvEramRm_u;

/*REG_EMERALD_ENV_CPU_GPR1 0xE0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 cpuGpr1:32;	// CPU GP register1
	} bitFields;
} RegEmeraldEnvCpuGpr1_u;

/*REG_EMERALD_ENV_CPU_GPR2 0xE4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 cpuGpr2:32;	// CPU GP register2
	} bitFields;
} RegEmeraldEnvCpuGpr2_u;

/*REG_EMERALD_ENV_CPU_GPR3 0xE8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 cpuGpr3:32;	// CPU GP register3
	} bitFields;
} RegEmeraldEnvCpuGpr3_u;

/*REG_EMERALD_ENV_SPARE_CONFIG_MAC_IN 0xF0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 spareConfigMacIn:16;	// BIST active override
		uint32 reserved0:16;
	} bitFields;
} RegEmeraldEnvSpareConfigMacIn_u;

/*REG_EMERALD_ENV_SPARE_CONFIG_MAC_OUT 0xF4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 spareConfigMacOut:16;	// BIST active override
		uint32 reserved0:16;
	} bitFields;
} RegEmeraldEnvSpareConfigMacOut_u;

/*REG_EMERALD_ENV_SWDOG_BOOT_CNT 0x100 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swWatchdogCountValueForNmi:32;	// SW Watchdog count value for NMI
	} bitFields;
} RegEmeraldEnvSwdogBootCnt_u;

/*REG_EMERALD_ENV_SWDOG_INT_CNT 0x104 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swWatchdogCountValueForIrq:32;	// SW Watchdog count value for IRQ
	} bitFields;
} RegEmeraldEnvSwdogIntCnt_u;

/*REG_EMERALD_ENV_SWDOG_MASK 0x108 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swWatchdogMask:1;	// SW Watchdog mask
		uint32 reserved0:31;
	} bitFields;
} RegEmeraldEnvSwdogMask_u;

/*REG_EMERALD_ENV_SWDOG_BOOT_CTR 0x10C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swWatchdogCounterValueForNmi:32;	// SW Watchdog counter value for NMI
	} bitFields;
} RegEmeraldEnvSwdogBootCtr_u;

/*REG_EMERALD_ENV_SWDOG_INT_CTR 0x110 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swWatchdogCounterValueForIrq:32;	// SW Watchdog counter value for IRQ
	} bitFields;
} RegEmeraldEnvSwdogIntCtr_u;

/*REG_EMERALD_ENV_MAC_TO_PHY_RESET_N 0x114 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 macToPhyResetN:1;	// WLAN Phy SW reset
		uint32 reserved0:31;
	} bitFields;
} RegEmeraldEnvMacToPhyResetN_u;

/*REG_EMERALD_ENV_IRQ_MASK 0x140 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:1;
		uint32 irqMask:31;	// IRQ mask
	} bitFields;
} RegEmeraldEnvIrqMask_u;

/*REG_EMERALD_ENV_IRQ_SEC_RF 0x144 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 useSecondRegisterFileForIrqs:31;	// Use Second register file for IRQ's
		uint32 reserved0:1;
	} bitFields;
} RegEmeraldEnvIrqSecRf_u;

/*REG_EMERALD_ENV_IRQ_MAP_00 0x148 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 irqSource01Mapping:5;	// IRQ Source 01 mapping
		uint32 irqSource02Mapping:5;	// IRQ Source 02 mapping
		uint32 irqSource03Mapping:5;	// IRQ Source 03 mapping
		uint32 irqSource04Mapping:5;	// IRQ Source 04 mapping
		uint32 irqSource05Mapping:5;	// IRQ Source 05 mapping
		uint32 irqSource06Mapping:5;	// IRQ Source 06 mapping
		uint32 reserved0:2;
	} bitFields;
} RegEmeraldEnvIrqMap00_u;

/*REG_EMERALD_ENV_IRQ_MAP_01 0x14C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 irqSource07Mapping:5;	// IRQ Source 07 mapping
		uint32 irqSource08Mapping:5;	// IRQ Source 08 mapping
		uint32 irqSource09Mapping:5;	// IRQ Source 09 mapping
		uint32 irqSource10Mapping:5;	// IRQ Source 10 mapping
		uint32 irqSource11Mapping:5;	// IRQ Source 11 mapping
		uint32 irqSource12Mapping:5;	// IRQ Source 12 mapping
		uint32 reserved0:2;
	} bitFields;
} RegEmeraldEnvIrqMap01_u;

/*REG_EMERALD_ENV_IRQ_MAP_02 0x150 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 irqSource13Mapping:5;	// IRQ Source 13 mapping
		uint32 irqSource14Mapping:5;	// IRQ Source 14 mapping
		uint32 irqSource15Mapping:5;	// IRQ Source 15 mapping
		uint32 irqSource16Mapping:5;	// IRQ Source 16 mapping
		uint32 irqSource17Mapping:5;	// IRQ Source 17 mapping
		uint32 irqSource18Mapping:5;	// IRQ Source 18 mapping
		uint32 reserved0:2;
	} bitFields;
} RegEmeraldEnvIrqMap02_u;

/*REG_EMERALD_ENV_IRQ_MAP_03 0x154 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 irqSource19Mapping:5;	// IRQ Source 19 mapping
		uint32 irqSource20Mapping:5;	// IRQ Source 20 mapping
		uint32 irqSource21Mapping:5;	// IRQ Source 21 mapping
		uint32 irqSource22Mapping:5;	// IRQ Source 22 mapping
		uint32 irqSource23Mapping:5;	// IRQ Source 23 mapping
		uint32 irqSource24Mapping:5;	// IRQ Source 24 mapping
		uint32 reserved0:2;
	} bitFields;
} RegEmeraldEnvIrqMap03_u;

/*REG_EMERALD_ENV_IRQ_MAP_04 0x158 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 irqSource25Mapping:5;	// IRQ Source 25 mapping
		uint32 irqSource26Mapping:5;	// IRQ Source 26 mapping
		uint32 irqSource27Mapping:5;	// IRQ Source 27 mapping
		uint32 irqSource28Mapping:5;	// IRQ Source 28 mapping
		uint32 irqSource29Mapping:5;	// IRQ Source 29 mapping
		uint32 irqSource30Mapping:5;	// IRQ Source 30 mapping
		uint32 reserved0:2;
	} bitFields;
} RegEmeraldEnvIrqMap04_u;

/*REG_EMERALD_ENV_IRQ_MAP_05 0x15C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 irqSource31Mapping:5;	// IRQ Source 31 mapping
		uint32 reserved0:27;
	} bitFields;
} RegEmeraldEnvIrqMap05_u;

/*REG_EMERALD_ENV_IRQ_MAPPED 0x160 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mappedIrqsFromMapper:32;	// Mapped IRQs from mapper
	} bitFields;
} RegEmeraldEnvIrqMapped_u;

/*REG_EMERALD_ENV_IRQ_UNMAPPED 0x164 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 unmappedIrqsToMapper:31;	// Unmapped IRQs to mapper
		uint32 reserved0:1;
	} bitFields;
} RegEmeraldEnvIrqUnmapped_u;

/*REG_EMERALD_ENV_TIMERS_IRQ_EN 0x174 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 timer0IrqEn:1;	// Timer 0 IRQ enable
		uint32 timer1IrqEn:1;	// Timer 1 IRQ enable
		uint32 timer2IrqEn:1;	// Timer 2 IRQ enable
		uint32 timer3IrqEn:1;	// Timer 3 IRQ enable
		uint32 reserved0:28;
	} bitFields;
} RegEmeraldEnvTimersIrqEn_u;

/*REG_EMERALD_ENV_TIMER0_PRESCALER 0x178 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 timer0Prescaler:12;	// Timer 0 prescaler
		uint32 reserved0:4;
		uint32 rfT0PrescaleValue:12;	// Timer 0 prescaler reduced freq
		uint32 reserved1:4;
	} bitFields;
} RegEmeraldEnvTimer0Prescaler_u;

/*REG_EMERALD_ENV_TIMER1_PRESCALER 0x17C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 timer1Prescaler:12;	// Timer 1 prescaler
		uint32 reserved0:4;
		uint32 rfT1PrescaleValue:12;	// Timer 1 prescaler reduced freq
		uint32 reserved1:4;
	} bitFields;
} RegEmeraldEnvTimer1Prescaler_u;

/*REG_EMERALD_ENV_TIMER0_MAX_COUNT 0x180 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 timer0MaxCount:32;	// Timer 0 max. count
	} bitFields;
} RegEmeraldEnvTimer0MaxCount_u;

/*REG_EMERALD_ENV_TIMER1_MAX_COUNT 0x184 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 timer1MaxCount:32;	// Timer 1 max. count
	} bitFields;
} RegEmeraldEnvTimer1MaxCount_u;

/*REG_EMERALD_ENV_TIMER2_MAX_COUNT 0x188 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 timer2MaxCount:32;	// Timer 2 max. count
	} bitFields;
} RegEmeraldEnvTimer2MaxCount_u;

/*REG_EMERALD_ENV_TIMER3_MAX_COUNT 0x18C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 timer3MaxCount:32;	// Timer 3 max. count
	} bitFields;
} RegEmeraldEnvTimer3MaxCount_u;

/*REG_EMERALD_ENV_TIMER0_CTR 0x190 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 timer0CountValue:32;	// Timer 0 count value
	} bitFields;
} RegEmeraldEnvTimer0Ctr_u;

/*REG_EMERALD_ENV_TIMER1_CTR 0x194 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 timer1CountValue:32;	// Timer 1 count value
	} bitFields;
} RegEmeraldEnvTimer1Ctr_u;

/*REG_EMERALD_ENV_TIMER2_CTR 0x198 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 timer2CountValue:32;	// Timer 2 count value
	} bitFields;
} RegEmeraldEnvTimer2Ctr_u;

/*REG_EMERALD_ENV_TIMER3_CTR 0x19C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 timer3CountValue:32;	// Timer 3 count value
	} bitFields;
} RegEmeraldEnvTimer3Ctr_u;

/*REG_EMERALD_ENV_TIMERS_STATUS 0x1A0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 timer0Expired:1;	// Timer0 Expired
		uint32 timer1Expired:1;	// Timer1 Expired
		uint32 timer2Expired:1;	// Timer2 Expired
		uint32 timer3Expired:1;	// Timer3 Expired
		uint32 reserved0:28;
	} bitFields;
} RegEmeraldEnvTimersStatus_u;

/*REG_EMERALD_ENV_TIMERS_CONFIG 0x1A4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 timer0OneshotMode:1;	// Timer 0 OneShot mode
		uint32 timer1OneshotMode:1;	// Timer 1 OneShot mode
		uint32 timer2OneshotMode:1;	// Timer 2 OneShot mode
		uint32 timer3OneshotMode:1;	// Timer 3 OneShot mode
		uint32 timer0Enable:1;	// Timer 0 enable
		uint32 timer1Enable:1;	// Timer 1 enable
		uint32 timer2Enable:1;	// Timer 2 enable
		uint32 timer3Enable:1;	// Timer 3 enable
		uint32 reserved0:24;
	} bitFields;
} RegEmeraldEnvTimersConfig_u;

/*REG_EMERALD_ENV_TIMER2_PRESCALER 0x1A8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 timer2Prescaler:12;	// Timer 2 prescaler
		uint32 reserved0:4;
		uint32 rfT2PrescaleValue:12;	// Timer 2 prescaler reduced freq
		uint32 reserved1:4;
	} bitFields;
} RegEmeraldEnvTimer2Prescaler_u;

/*REG_EMERALD_ENV_TIMER3_PRESCALER 0x1AC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 timer3Prescaler:12;	// Timer 3 prescaler
		uint32 reserved0:4;
		uint32 rfT3PrescaleValue:12;	// Timer 3 prescaler reduced freq
		uint32 reserved1:4;
	} bitFields;
} RegEmeraldEnvTimer3Prescaler_u;

/*REG_EMERALD_ENV_IC_HIT_CNT 0x1C0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 instructionCacheHitCount:32;	// Instruction Cache hit count
	} bitFields;
} RegEmeraldEnvIcHitCnt_u;

/*REG_EMERALD_ENV_IC_MISS_CNT 0x1C4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 instructionCacheMissCount:32;	// Instruction Cache miss count
	} bitFields;
} RegEmeraldEnvIcMissCnt_u;

/*REG_EMERALD_ENV_DC_HIT_CNT 0x1C8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dataCacheHitCount:32;	// Data Cache hit count
	} bitFields;
} RegEmeraldEnvDcHitCnt_u;

/*REG_EMERALD_ENV_DC_MISS_CNT 0x1CC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dataCacheMissCount:32;	// Data Cache miss count
	} bitFields;
} RegEmeraldEnvDcMissCnt_u;

/*REG_EMERALD_ENV_MIPS_SI_INT_STATUS 0x1D0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 siSwIrq:2;	// SI SW IRQ
		uint32 siTimerIrq:1;	// SI Timer IRQ
		uint32 reserved0:29;
	} bitFields;
} RegEmeraldEnvMipsSiIntStatus_u;

/*REG_EMERALD_ENV_MIPS_SI_INT_EN 0x1D4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 siSwIrqEn:2;	// SI SW IRQ enable
		uint32 siTimerIrqEn:1;	// SI Timer IRQ enable
		uint32 reserved0:29;
	} bitFields;
} RegEmeraldEnvMipsSiIntEn_u;

/*REG_EMERALD_ENV_HW_EVENTS_INT_STATUS 0x1D8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hwEventsInterrupts:32;	// HW events interrupts
	} bitFields;
} RegEmeraldEnvHwEventsIntStatus_u;

/*REG_EMERALD_ENV_HW_ERRORS_INT_STATUS 0x1DC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hwErrorsInterrupts:32;	// HW errors interrupts
	} bitFields;
} RegEmeraldEnvHwErrorsIntStatus_u;

/*REG_EMERALD_ENV_HW_EVENTS_INT_EN 0x1E0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hwEventsIntEn:32;	// HW events int enable
	} bitFields;
} RegEmeraldEnvHwEventsIntEn_u;

/*REG_EMERALD_ENV_HW_ERRORS_INT_EN 0x1E4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hwErrorsIntEn:32;	// HW errors int enable
	} bitFields;
} RegEmeraldEnvHwErrorsIntEn_u;

/*REG_EMERALD_ENV_HW_EVENTS_INT_MASKED_STATUS 0x1E8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hwEventsIntMasked:32;	// HW events int masked
	} bitFields;
} RegEmeraldEnvHwEventsIntMaskedStatus_u;

/*REG_EMERALD_ENV_HW_ERRORS_INT_MASKED_STATUS 0x1EC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hwErrorsIntMasked:32;	// HW errors int masked
	} bitFields;
} RegEmeraldEnvHwErrorsIntMaskedStatus_u;



#endif // _EMERALD_ENV_REGS_H_
