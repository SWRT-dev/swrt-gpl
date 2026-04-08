/******************************************************************************

                               Copyright (c) 2012
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
#include "mtlkinc.h"
#include "mtlk_dbg.h"

#if defined (CONFIG_ARCH_STR9100) /* Star boards */

#define LOG_LOCAL_GID   GID_DEBUG
#define LOG_LOCAL_FID   1

/*********************************************************************************
 * This code is taken from arch/arm/mach-str9100/str9100_setup.c since the 
 * APB_clock variable is not exported by 9100.
 *********************************************************************************/

u32 CPU_clock;
u32 AHB_clock;
u32 APB_clock;

static void str9100_determine_clock(void)
{
        switch ((PWRMGT_RESET_LATCH_CONFIGURATION_REG >> 6) & 0x03) {
        case 0x00:
                CPU_clock = 175000000;
                break;

        case 0x01:
                CPU_clock = 200000000;
                break;

        case 0x02:
                CPU_clock = 225000000;
                break;

        case 0x03:
                CPU_clock = 250000000;
                break;
        }

        AHB_clock = CPU_clock >> 1;
        APB_clock = AHB_clock >> 1;

        ILOG1_D("CPU clock at %dMHz\n", CPU_clock / 1000000);
        ILOG1_D("AHB clock at %dMHz\n", AHB_clock / 1000000);
        ILOG1_D("APB clock at %dMHz\n", APB_clock / 1000000);
}
/*********************************************************************************/

static void str9100_counter_init( void ) {
  u32 control_value,mask_value;
  control_value = TIMER_CONTROL_REG;
  control_value &= ~(1 << TIMER2_CLOCK_SOURCE_BIT_INDEX);
  control_value &= ~(1 << TIMER2_UP_DOWN_COUNT_BIT_INDEX);
  TIMER_CONTROL_REG = control_value;

  mask_value = TIMER_INTERRUPT_MASK_REG;
  mask_value |= (0x7 << 3);
  TIMER_INTERRUPT_MASK_REG = mask_value;
}

static void str9100_counter_enable ( void ) {
  u32 volatile control_value;
  TIMER2_COUNTER_REG=0;
  control_value = TIMER_CONTROL_REG;
  control_value |= (1 << TIMER2_ENABLE_BIT_INDEX);
  TIMER_CONTROL_REG = control_value;
}

static void str9100_counter_disable ( void ) { 
  u32 volatile control_value;
  control_value = TIMER_CONTROL_REG;
  control_value &= ~(1 << TIMER2_ENABLE_BIT_INDEX);
  TIMER_CONTROL_REG = control_value;
}

void str9100_counter_on_init( void ) {
  str9100_determine_clock();
  str9100_counter_init();
  str9100_counter_enable();
}

void str9100_counter_on_cleanup( void ) {
  str9100_counter_disable();
}

#endif
