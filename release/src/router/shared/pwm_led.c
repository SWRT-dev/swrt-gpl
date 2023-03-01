/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 *
 * Copyright 2014, ASUSTeK Inc.
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND ASUS GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 */
#include <shared.h>

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(ary) (sizeof(ary) / sizeof((ary)[0]))
#endif

#define SYS_PWMCHIP	"/sys/class/pwm/pwmchip0"

#if defined(RTAX59U)
#define PWMDEV_NO	(1)
#else
#error Define PWM device number!
#endif

static void set_pwm(char *type, char *config)
{
	char pwmdev[sizeof(SYS_PWMCHIP"/pwmXXX")];

	snprintf(pwmdev, sizeof(pwmdev), "%s/pwm%d", SYS_PWMCHIP, PWMDEV_NO);
	if (!d_exists(pwmdev))
		doSystem("echo %d > %s/export", PWMDEV_NO, SYS_PWMCHIP);
	
	doSystem("echo 0 > %s/mm_enable", pwmdev);
	if (!strcmp(type, "0"))
		return;

	doSystem("echo %s > %s/mm_type", type, pwmdev);
	doSystem("echo %s > %s/mm_config", config, pwmdev);
	doSystem("echo 1 > %s/mm_enable", pwmdev);
}

static void set_color(unsigned int color)
{
	struct led_mapping {
		unsigned int bitmask;
		unsigned int led_id;
	} l[] = {
		{RGBLED_BLED, LED_BLUE},
		{RGBLED_GLED, LED_GREEN},
		{RGBLED_RLED, LED_RED},
		{RGBLED_WLED, LED_WHITE}
	};
	int i;
	
	for (i = 0; i < ARRAY_SIZE(l); i++) {
		if (l[i].bitmask & color)
			led_control(l[i].led_id, LED_ON);
		else
			led_control(l[i].led_id, LED_OFF);
	}
}

void set_rgbled(unsigned int mode)
{
	unsigned int cmask = RGBLED_COLOR_MESK, bmask = RGBLED_BLINK_MESK;
	unsigned int c = mode & cmask;
	unsigned int b = mode & bmask;
	struct pwm_conf {
		char type[8];
		char config[16];
	} p[] = {
		{      "0",         "0-0-0"},	/* on */
		{      "3",   "0xf-250-250"},	/* off:0.5s, on:0.5s */
		{      "3",   "0xf-125-375"},	/* off:0.25s, on:0.75s */
		{      "3", "0xf-1500-1500"},	/* off:3s, on:3s */
		{"1-128-1",   "0x3-625-625"}	/* breathing */
	};
	int pidx = 0;

	if ((c == RGBLED_CONNECTED || c == RGBLED_ETH_BACKHAUL)
	  && b == 0
	  && nvram_match("AllLED", "0")
	)
		c = 0;

	switch (b) {
	case RGBLED_ATE_MODE:
		pidx = 0;
		break;
	case RGBLED_SBLINK:
		pidx = 1;
		break;
	case RGBLED_3ON1OFF:
		pidx = 2;
		break;
	case RGBLED_3ON3OFF:
		pidx = 3;
		break;
	case RGBLED_BREATHING:
		pidx = 4;
		break;
	default:
		;
	}

	if (b != RGBLED_ATE_MODE)
		set_color(c);
	set_pwm(p[pidx].type, p[pidx].config);
}
