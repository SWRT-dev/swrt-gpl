/*
 * Copyright 2010-2018 MediaTek, Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * Version 2 as published by the Free Software Foundation.
 */

#include <common.h>
#include <config.h>
#include <asm/arch/timer.h>
#include <asm/arch/leopard.h>


#define GPT4_CON            ((volatile u32 *)(GPT_BASE + 0x0040))
#define GPT4_CLK            ((volatile u32 *)(GPT_BASE + 0x0044))
#define GPT4_DAT            ((volatile u32 *)(GPT_BASE + 0x0048))

#define GPT4_EN             0x0001
#define GPT4_FREERUN        0x0030
#define GPT4_SYS_CLK        0x0000
#define GPT4_CLK_DIV1       0X0000
#define GPT4_CLK_DIV2       0X0001

#define GPT4_CLK_SETTING    (GPT4_SYS_CLK | GPT4_CLK_DIV1)

#if CFG_FPGA_PLATFORM
/* FPGA : 12MHz setting */
#define GPT4_1US_TICK       ((u32)12)
#define GPT4_1MS_TICK       ((u32)12000)
#else
/* ASIC : 40M/4 = 10MHz setting */
#define GPT4_1US_TICK       ((u32)10)
#define GPT4_1MS_TICK       ((u32)10000)
#endif

#define GPT4_MAX_TICK_CNT   ((u32)0xFFFFFFFF)
#define MAX_TIMESTAMP_MS    ((u32)0xFFFFFFFF)

#define TIME_TO_TICK_US(us) ((us)*GPT4_1US_TICK)
#define TIME_TO_TICK_MS(ms) ((ms)*GPT4_1MS_TICK)

#define GPT4_MAX_US_TIMEOUT (GPT4_MAX_TICK_CNT / GPT4_1US_TICK)
#define GPT4_MAX_MS_TIMEOUT (GPT4_MAX_TICK_CNT / GPT4_1MS_TICK)

#define CFG_HZ              (100)
#define MAX_REG_MS          (GPT4_MAX_MS_TIMEOUT)


static ulong timestamp;
static ulong lastinc;


/*
 * Leopard GPT4 fixed 10MHz counter.
 */
static void gpt4_start(void)
{
	*GPT4_CLK = (GPT4_CLK_SETTING);
	*GPT4_CON = (GPT4_EN | GPT4_FREERUN);
}

static void gpt4_stop(void)
{
	*GPT4_CON = 0x0; /* disable */
	*GPT4_CON = 0x2; /* clear counter */
}

static void gpt4_init(bool bStart)
{
	/* clear GPT4 first */
	gpt4_stop();

	/* enable GPT4 without lock */
	if (bStart)
		gpt4_start();
}

u32 gpt4_get_current_tick(void)
{
	return (u32)(*GPT4_DAT);
}

bool gpt4_timeout_tick(u32 start_tick, u32 timeout_tick)
{
	register u32 cur_tick;
	register u32 elapse_tick;

	/* get current tick */
	cur_tick = gpt4_get_current_tick();

	/* check elapse time */
	if (start_tick <= cur_tick)
		elapse_tick = cur_tick - start_tick;
	else
		elapse_tick = (GPT4_MAX_TICK_CNT - start_tick) + cur_tick;

	/* check if timeout */
	if (timeout_tick <= elapse_tick) {
		/*timeout*/
		return true;
	}

	return false;
}

/*
 * us interface
 */
u32 gpt4_tick2time_us(u32 tick)
{
	return (u32)((tick + (GPT4_1US_TICK - 1)) / GPT4_1US_TICK);
}

u32 gpt4_time2tick_us(u32 time_us)
{
	if (GPT4_MAX_US_TIMEOUT <= time_us)
		return GPT4_MAX_TICK_CNT;
	else
		return TIME_TO_TICK_US(time_us);
}

/*
 * ms interface
 */
static u32 gpt4_tick2time_ms(u32 tick)
{
	return (u32)((tick + (GPT4_1MS_TICK - 1)) / GPT4_1MS_TICK);
}

static u32 gpt4_time2tick_ms(u32 time_ms)
{
	if (GPT4_MAX_MS_TIMEOUT <= time_ms)
		return GPT4_MAX_TICK_CNT;
	else
		return TIME_TO_TICK_MS(time_ms);
}

/*
 * busy wait
 */
void gpt_busy_wait_us(u32 timeout_us)
{
	u32 start_tick, timeout_tick;

	/* get timeout tick */
	timeout_tick = gpt4_time2tick_us(timeout_us);
	start_tick = gpt4_get_current_tick();

	/* wait for timeout */
	while (!gpt4_timeout_tick(start_tick, timeout_tick))
		;
}

void gpt_busy_wait_ms(u32 timeout_ms)
{
	u32 start_tick, timeout_tick;

	/* get timeout tick */
	timeout_tick = gpt4_time2tick_ms(timeout_ms);
	start_tick = gpt4_get_current_tick();

	/* wait for timeout */
	while (!gpt4_timeout_tick(start_tick, timeout_tick))
		;
}


void reset_timer_masked(void)
{
	lastinc = gpt4_tick2time_ms(*GPT4_DAT);
	timestamp = 0;
}

/*
 * The unit of the return value is ms.
 */
ulong get_timer_masked(void)
{
	ulong now;

	now = gpt4_tick2time_ms(*GPT4_DAT);

	if (now >= lastinc) {
		/* normal */
		timestamp = timestamp + now - lastinc;
	} else {
		/* overflow */
		timestamp = timestamp + MAX_REG_MS - lastinc + now;
	}

	lastinc = now;

	return timestamp;
}

void reset_timer(void)
{
	reset_timer_masked();
}

ulong get_timer(ulong base)
{
	ulong current_timestamp = 0;
	ulong temp = 0;

	current_timestamp = get_timer_masked();

	if (current_timestamp >= base) {
		/* timestamp normal */
		return (ulong)(current_timestamp - base);
	}

	/* timestamp overflow */
	temp = base - current_timestamp;

	return (ulong)(MAX_TIMESTAMP_MS - temp);
}


/*
 * Iverson 20150522 :
 *      The function is ported from LK.
 *      but multiple definition with <src>\lib\Time.c
 */
#if 0

/* delay msec mseconds */
void mdelay(unsigned long msec)
{
	gpt_busy_wait_ms(msec);
}

/* delay usec useconds */
void udelay(unsigned long usec)
{
	gpt_busy_wait_us(usec);
}
#endif

/*
 * Iverson 20150522 :
 *      This is a very importmant function,
 *      uboot use the function for bootmenu countdown counter.
 *      The function is not defined in preloader. We should port it by ourself.
 */

/* delay usec useconds */
void __udelay(unsigned long usec)
{
	gpt_busy_wait_us(usec);
}

/*
 * This function is derived from PowerPC code (read timebase as long long).
 * On ARM it just returns the timer value.
 */
unsigned long long get_ticks(void)
{
	return (unsigned long long) get_timer(0);
}

/*
 * This function is derived from PowerPC code (timebase clock frequency).
 * On ARM it returns the number of timer ticks per second.
 */
ulong get_tbclk(void)
{
	ulong tbclk;
	tbclk = CFG_HZ;
	return tbclk;
}

/*
 * timer_init()
 */
void mtk_timer_init(void)
{
	gpt4_init(true);
	/* init timer system */
	reset_timer();
}
