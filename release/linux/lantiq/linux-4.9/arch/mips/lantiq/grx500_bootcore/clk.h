/*
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 *
 * Copyright (C) 2010 John Crispin <blogic@openwrt.org>
 * Copyright (C) 2013 Lei Chuanhua <Chuanhua.lei@lantiq.com>
 */

#ifndef _LTQ_CLK_H__
#define _LTQ_CLK_H__

#include <linux/clkdev.h>

/* clock speeds */
#define CLOCK_33M	33333333
#define CLOCK_60M	60000000
#define CLOCK_62_5M	62500000
#define CLOCK_83M	83333333
#define CLOCK_83_5M	83500000
#define CLOCK_98_304M	98304000
#define CLOCK_100M	100000000
#define CLOCK_111M	111111111
#define CLOCK_125M	125000000
#define CLOCK_133M	133333333
#define CLOCK_150M	150000000
#define CLOCK_166M	166666666
#define CLOCK_167M	166666667
#define CLOCK_180M	180000000
#define CLOCK_196_608M	196608000
#define CLOCK_200M	200000000
#define CLOCK_222M	222000000
#define CLOCK_240M	240000000
#define CLOCK_250M	250000000
#define CLOCK_266M	266666666
#define CLOCK_288M	288888888
#define CLOCK_300M	300000000
#define CLOCK_333M	333333333
#define CLOCK_360M	360000000
#define CLOCK_393M	393215332
#define CLOCK_400M	400000000
#define CLOCK_432M	432000000
#define CLOCK_450M	450000000
#define CLOCK_500M	500000000
#define CLOCK_600M	600000000
#define CLOCK_666M	666666666
#define CLOCK_720M	720000000

/* clock out speeds */
#define CLOCK_32_768K	32768
#define CLOCK_1_536M	1536000
#define CLOCK_2_5M	2500000
#define CLOCK_12M	12000000
#define CLOCK_24M	24000000
#define CLOCK_25M	25000000
#define CLOCK_30M	30000000
#define CLOCK_40M	40000000
#define CLOCK_48M	48000000
#define CLOCK_50M	50000000
#define CLOCK_60M	60000000

enum {
	STATIC_CPU_CLK = 1,
	STATIC_FPI_CLK,
	STATIC_IO_CLK,
	STATIC_PPE_CLK,
	STATIC_NO_PARENT = 0xff,
};

struct clk_rates {
	unsigned int cpu_freq;
	unsigned int ddr_freq;
	unsigned int cgu_sys;
};

struct clk {
	struct clk_lookup cl;
	unsigned long rate;
	unsigned long *rates;
	unsigned int module;
	unsigned int bits;
	unsigned int parent;
	unsigned int scale_data;
	atomic_t refcount;
	bool always_on; /* 0 -- always on or not used, 1 -- configurable */
	unsigned long (*get_rate) (void);
	int (*set_rate) (unsigned long cpu_freq);
	int (*enable) (struct clk *clk);
	void (*disable) (struct clk *clk);
	int (*activate) (struct clk *clk);
	void (*deactivate) (struct clk *clk);
	void (*reboot) (struct clk *clk);
};

/*
extern void clkdev_add_static(unsigned long cpu, unsigned long fpi,
				unsigned long io, unsigned long ppe);
*/
extern unsigned long ltq_grx500_cpu_hz(void);
extern int ltq_grx500_set_cpu_hz(unsigned long cpu_freq);

extern unsigned long ltq_grx500_fpi_hz(void);
extern int ltq_grx500_set_fpi_hz(unsigned long cpu_freq);

unsigned long *ltq_get_avail_scaling_rates(int sel);

#endif
