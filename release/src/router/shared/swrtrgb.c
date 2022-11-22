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
 * Copyright 2022, SWRTdev.
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND SWRTdev GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 */
#include <rtconfig.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <bcmnvram.h>
#include <shared.h>
#include <stdio.h>
#include "shutils.h"

static unsigned int rgbled_udef_mode = 0;

void set_rgbled(unsigned int mode)
{
	unsigned int cmask = RGBLED_COLOR_MESK, bmask = RGBLED_BLINK_MESK;
	unsigned int c = mode & cmask;
	unsigned int b = mode & bmask;

	int uidx = 0;
	char *led_color[] = {
		"0 0 0 0",		/* off: B G R W */
		"1 0 0 0",		/* RGBLED_BLUE */
		"0 1 0 0",		/* RGBLED_GREEN */
		"0 0 1 0",		/* RGBLED_RED */
		"1 1 0 0",		/* RGBLED_NIAGARA_BLUE */
		"0 1 1 0",		/* RGBLED_YELLOW */
		"1 0 1 0",		/* RGBLED_PURPLE */
		"1 1 1 1"		/* RGBLED_WHITE */
	};
	char *udef_trigger = led_color[0];
#ifdef RTCONFIG_SW_CTRL_ALLLED
	int ctrl_led_off = nvram_match("AllLED", "0");
#endif

	if (rgbled_udef_mode == 0) {
		led_control(LED_BLUE, LED_ON);
		led_control(LED_GREEN, LED_ON);
		led_control(LED_RED, LED_ON);
#if defined(RTAC59_CD6R) || defined(RTAC59_CD6N) || defined(PLAX56_XP4) || defined(MR60) || defined(MS60)
		if (RGBLED_WHITE & RGBLED_WLED)
			led_control(LED_WHITE, LED_ON);
#endif		
		rgbled_udef_mode = 1;
	}

	switch (c) {
	case RGBLED_BLUE:
		udef_trigger = led_color[1];
#if defined(RTCONFIG_SWRT_LED_RGB)
#if defined(MR60) || defined(MS60)
		led_control(LED_BLUE, LED_ON);
		led_control(LED_GREEN, LED_OFF);
		led_control(LED_RED, LED_OFF);
		led_control(LED_WHITE, LED_OFF);
#elif defined(RMAX6000)
		i2cled_control(LED_BLUE, LED_ON);
#endif
#endif
		break;
	case RGBLED_GREEN:
		udef_trigger = led_color[2];
#if defined(RTCONFIG_SWRT_LED_RGB)
#if defined(MR60) || defined(MS60)
		led_control(LED_BLUE, LED_OFF);
		led_control(LED_GREEN, LED_ON);
		led_control(LED_RED, LED_OFF);
		led_control(LED_WHITE, LED_OFF);
#elif defined(RMAX6000)
		i2cled_control(LED_GREEN, LED_ON);
#endif
#endif
		break;
	case RGBLED_RED:
		udef_trigger = led_color[3];
#if defined(RTCONFIG_SWRT_LED_RGB)
#if defined(MR60) || defined(MS60)
		led_control(LED_BLUE, LED_OFF);
		led_control(LED_GREEN, LED_OFF);
		led_control(LED_RED, LED_ON);
		led_control(LED_WHITE, LED_OFF);
#elif defined(RMAX6000)
		i2cled_control(LED_RED, LED_ON);
#endif
#endif
		break;
	case RGBLED_NIAGARA_BLUE:
		udef_trigger = led_color[4];
#if defined(RTCONFIG_SWRT_LED_RGB)
#if defined(MR60) || defined(MS60)
		led_control(LED_BLUE, LED_ON);
		led_control(LED_GREEN, LED_ON);
		led_control(LED_RED, LED_OFF);
		led_control(LED_WHITE, LED_OFF);
#endif
#endif
		break;
	case RGBLED_YELLOW:
		udef_trigger = led_color[5];
#if defined(RTCONFIG_SWRT_LED_RGB)
#if defined(MR60) || defined(MS60)
		led_control(LED_BLUE, LED_OFF);
		led_control(LED_GREEN, LED_ON);
		led_control(LED_RED, LED_OFF);
		led_control(LED_WHITE, LED_OFF);
#elif defined(RMAX6000)
		i2cled_control(LED_GREEN|LED_RED, LED_ON);
#endif
#endif
		break;
	case RGBLED_PURPLE:
		udef_trigger = led_color[6];
#if defined(RTCONFIG_SWRT_LED_RGB)
#if defined(MR60) || defined(MS60)
		led_control(LED_BLUE, LED_ON);
		led_control(LED_GREEN, LED_OFF);
		led_control(LED_RED, LED_ON);
		i2cled_control(LED_WHITE, LED_OFF);
#endif
#endif
		break;
	case RGBLED_WHITE:
		udef_trigger = led_color[7];
#if defined(RTCONFIG_SWRT_LED_RGB)
#if defined(MR60) || defined(MS60)
		led_control(LED_BLUE, LED_OFF);
		led_control(LED_GREEN, LED_OFF);
		led_control(LED_RED, LED_OFF);
		led_control(LED_WHITE, LED_ON);
#elif defined(RMAX6000)
		i2cled_control(LED_WHITE, LED_ON);
#endif
#endif
		break;
	default:
		;
	}
#if defined(RTCONFIG_SWRT_LED_RGB)
	return;
#endif
	if ((c == RGBLED_CONNECTED || c == RGBLED_ETH_BACKHAUL)
	  && b == 0
#ifdef RTCONFIG_SW_CTRL_ALLLED
	  && ctrl_led_off
#endif
	)
		udef_trigger = led_color[0];

	switch (b) {
	case RGBLED_SBLINK:
		uidx = 1;
		break;
	case RGBLED_3ON1OFF:
		uidx = 2;
		break;
	case RGBLED_3ON3OFF:
		uidx = 3;
		break;
	default:
		;
	}

}

