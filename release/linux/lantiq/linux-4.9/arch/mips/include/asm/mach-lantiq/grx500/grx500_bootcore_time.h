#ifndef __GRX500_BOOTCORE_TIME_H__
#define __GRX500_BOOTCORE_TIME_H__

#ifdef  MT_GLOBAL
#define MT_EXTERN
#define MT_I(x) x
#else
#define MT_EXTERN extern
#define MT_I(x)
#endif

extern void __init setup_grx500_bootcore_timer(void);

enum {
  MT_MIPS_TIMER_0,
  MT_MIPS_TIMER_1,
  MT_MIPS_TIMER_2,
  MT_MIPS_TIMER_3
};

#define MT_MIPS_TIMER_DISABLE      0
#define MT_MIPS_TIMER_ENABLE       1

#define MT_MIPS_TIMER_ONE_SHOT     0
#define MT_MIPS_TIMER_MULT_SHOT    1

#ifdef DO_CPU_STAT
extern void grx500_bootcore_set_time_mips_timer(unsigned long timerId, unsigned long maxCount);	
extern void grx500_bootcore_set_mode_mips_timer(unsigned long timerId, unsigned long enableTimer, unsigned long shotMode);	
extern void grx500_bootcore_set_time_mips_prescaler(unsigned long  timerId, unsigned long  prescale);	
#endif

#undef MT_EXTERN
#undef MT_I

/*for real system undefine GRX500_BOOTCORE_SCALE_DOWN*/
extern unsigned int system_to_cpu_multiplier;
#ifdef CONFIG_VBG400_CHIPIT
#define GRX500_BOOTCORE_SCALE_DOWN
#endif

#ifdef GRX500_BOOTCORE_SCALE_DOWN
#define GRX500_BOOTCORE_SCALE_VAL 10
#else
#define GRX500_BOOTCORE_SCALE_VAL 1
#endif

#if 1
#ifdef CONFIG_USE_EMULATOR
#ifdef CONFIG_SOC_PRX300_BOOTCORE
#define GRX500_BOOTCORE_SYSTEM_CLK  15000000
#define GRX500_BOOTCORE_CPU_CLK     15000000;
#else /* CONFIG_SOC_PRX300_BOOTCORE */
#define GRX500_BOOTCORE_SYSTEM_CLK  6000000
#define GRX500_BOOTCORE_CPU_CLK     6000000; //480000000
#endif /* CONFIG_SOC_PRX300_BOOTCORE */
#else
#ifdef CONFIG_SOC_PRX300_BOOTCORE
#define CLOCK_500M  500000000
#define GRX500_BOOTCORE_SYSTEM_CLK CLOCK_500M
#define GRX500_BOOTCORE_CPU_CLK    CLOCK_500M
#else /* CONFIG_SOC_PRX300_BOOTCORE */
#define CLOCK_300M  300000000
#define GRX500_BOOTCORE_SYSTEM_CLK CLOCK_300M
#define GRX500_BOOTCORE_CPU_CLK    CLOCK_300M
#endif /* CONFIG_SOC_PRX300_BOOTCORE */
#endif
#else
#define GRX500_BOOTCORE_SYSTEM_CLK  (24000000*GRX500_BOOTCORE_SCALE_VAL)
#ifdef CONFIG_VBG400_CHIPIT
#define GRX500_BOOTCORE_CPU_CLK     (48000000*GRX500_BOOTCORE_SCALE_VAL)
#else
#define GRX500_BOOTCORE_CPU_CLK     (GRX500_BOOTCORE_SYSTEM_CLK *system_to_cpu_multiplier)
#endif
#endif

#define REG_NPU_SYS_INFO		0x60
#define CPU_FAST_MODE		0x00080000

/* Note - the timer used is GRX500_BOOTCORE, run 24000000M
* If we use the MIPS timer, change to  GRX500_BOOTCORE_CPU_CLK
*/
//#define TICK_DIV (GRX500_BOOTCORE_CPU_CLK/HZ)
#define TICK_DIV (GRX500_BOOTCORE_SYSTEM_CLK/HZ)


#endif /* __GRX500_BOOTCORE_TIME_H__ */ 
