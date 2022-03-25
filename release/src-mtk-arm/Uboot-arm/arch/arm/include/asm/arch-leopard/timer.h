/*
 * Copyright 2010-2018 MediaTek, Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * Version 2 as published by the Free Software Foundation.
 */

#ifndef __MTK_TIMER_H__
#define __MTK_TIMER_H__

#include <linux/compiler.h>


extern ulong get_timer(ulong base);
extern void mdelay(unsigned long msec);
extern void udelay(unsigned long usec);
extern void mtk_timer_init(void);

extern void gpt_busy_wait_us(u32 timeout_us);
extern void gpt_busy_wait_ms(u32 timeout_ms);

extern u32 gpt4_get_current_tick(void);
extern bool gpt4_timeout_tick(u32 start_tick, u32 timeout_tick);
extern u32 gpt4_tick2time_us(u32 tick);
extern u32 gpt4_time2tick_us(u32 time_us);

#endif
