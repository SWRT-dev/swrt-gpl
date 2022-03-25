/*
 * (C) Copyright 2010
 * Jason Kridner <jkridner@beagleboard.org>
 *
 * Based on cmd_led.c patch from:
 * http://www.mail-archive.com/u-boot@lists.denx.de/msg06873.html
 * (C) Copyright 2008
 * Ulf Samuelsson <ulf.samuelsson@atmel.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <config.h>
#include <command.h>
#ifdef MT7622_ASIC_BOARD
#include <asm/arch/led.h>
#else
#include <status_led.h>

struct led_tbl_s {
	char		*string;	/* String for use in the command */
	led_id_t	mask;		/* Mask used for calling __led_set() */
	void		(*off)(void);	/* Optional function for turning LED off */
	void		(*on)(void);	/* Optional function for turning LED on */
	void		(*toggle)(void);/* Optional function for toggling LED */
};

typedef struct led_tbl_s led_tbl_t;

static const led_tbl_t led_commands[] = {
#ifdef CONFIG_BOARD_SPECIFIC_LED
#ifdef STATUS_LED_BIT
	{ "0", STATUS_LED_BIT, NULL, NULL, NULL },
#endif
#ifdef STATUS_LED_BIT1
	{ "1", STATUS_LED_BIT1, NULL, NULL, NULL },
#endif
#ifdef STATUS_LED_BIT2
	{ "2", STATUS_LED_BIT2, NULL, NULL, NULL },
#endif
#ifdef STATUS_LED_BIT3
	{ "3", STATUS_LED_BIT3, NULL, NULL, NULL },
#endif
#endif
#ifdef STATUS_LED_GREEN
	{ "green", STATUS_LED_GREEN, green_led_off, green_led_on, NULL },
#endif
#ifdef STATUS_LED_YELLOW
	{ "yellow", STATUS_LED_YELLOW, yellow_led_off, yellow_led_on, NULL },
#endif
#ifdef STATUS_LED_RED
	{ "red", STATUS_LED_RED, red_led_off, red_led_on, NULL },
#endif
#ifdef STATUS_LED_BLUE
	{ "blue", STATUS_LED_BLUE, blue_led_off, blue_led_on, NULL },
#endif
	{ NULL, 0, NULL, NULL, NULL }
};

enum led_cmd { LED_ON, LED_OFF, LED_TOGGLE };
#endif

#ifdef MT7622_ASIC_BOARD
int  get_led_cmd(char *var)
{
	if (strcmp(var, "clear") == 0) {
		return STATUS_LED_OFF;
	}
	if (strcmp(var, "set") == 0) {
		return STATUS_LED_ON;
	}
	if (strcmp(var, "blink") == 0)
		return STATUS_LED_BLINKING;
#else
enum led_cmd get_led_cmd(char *var)
{
	if (strcmp(var, "off") == 0) {
		return LED_OFF;
	}
	if (strcmp(var, "on") == 0) {
		return LED_ON;
	}
	if (strcmp(var, "toggle") == 0)
		return LED_TOGGLE;
#endif
	return -1;
}

int do_led (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
#ifdef MT7622_ASIC_BOARD
	int cmd;

	/* Validate arguments */
	if ((argc != 4)) {
		return CMD_RET_USAGE;
	}

	cmd = get_led_cmd(argv[1]);
	if (cmd < 0) {
		return CMD_RET_USAGE;
	}

	if (strcmp("power", argv[2]) == 0)
	{
		if (strcmp("green", argv[3]) == 0)
		{
			led_set_state(POWER_GREEN_SELECTOR, cmd, 500, 500);
		}

		if (strcmp("red", argv[3]) == 0)
		{
			led_set_state(POWER_RED_SELECTOR, cmd, 500, 500);
		}
	}
	if (strcmp("net", argv[2]) == 0)
	{
		if (strcmp("green", argv[3]) == 0)
		{
			led_set_state(NET_GREEN_SELECTOR, cmd, 500, 500);
		}

		if (strcmp("red", argv[3]) == 0)
		{
			led_set_state(NET_RED_SELECTOR, cmd, 500, 500);
		}
	}
	if (strcmp("wifin", argv[2]) == 0)
	{
		if (strcmp("green", argv[3]) == 0)
		{
			led_set_state(WIFIN_GREEN_SELECTOR, cmd, 500, 500);
		}

		if (strcmp("red", argv[3]) == 0)
		{
			led_set_state(WIFIN_RED_SELECTOR, cmd, 500, 500);
		}
	}
	if (strcmp("wifia", argv[2]) == 0)
	{
		if (strcmp("green", argv[3]) == 0)
		{
			led_set_state(WIFIA_GREEN_SELECTOR, cmd, 500, 500);
		}

		if (strcmp("red", argv[3]) == 0)
		{
			led_set_state(WIFIA_RED_SELECTOR, cmd, 500, 500);
		}
	}

#else
	int i, match = 0;
	enum led_cmd cmd;

	/* Validate arguments */
	if ((argc != 3)) {
		return CMD_RET_USAGE;
	}

	cmd = get_led_cmd(argv[2]);
	if (cmd < 0) {
		return CMD_RET_USAGE;
	}

	for (i = 0; led_commands[i].string; i++) {
		if ((strcmp("all", argv[1]) == 0) ||
		    (strcmp(led_commands[i].string, argv[1]) == 0)) {
			match = 1;
			switch (cmd) {
			case LED_ON:
				if (led_commands[i].on)
					led_commands[i].on();
				else
					__led_set(led_commands[i].mask,
							  STATUS_LED_ON);
				break;
			case LED_OFF:
				if (led_commands[i].off)
					led_commands[i].off();
				else
					__led_set(led_commands[i].mask,
							  STATUS_LED_OFF);
				break;
			case LED_TOGGLE:
				if (led_commands[i].toggle)
					led_commands[i].toggle();
				else
					__led_toggle(led_commands[i].mask);
			}
			/* Need to set only 1 led if led_name wasn't 'all' */
			if (strcmp("all", argv[1]) != 0)
				break;
		}
	}

	/* If we ran out of matches, print Usage */
	if (!match) {
		return CMD_RET_USAGE;
	}

#endif
	return 0;
}

U_BOOT_CMD(
#ifdef MT7622_ASIC_BOARD
	led, 4, 1, do_led,
	"<set|clear|blink>  <power|net|wifin|wifia>  <green|red>",
	"<set|clear|blink>  <power|net|wifin|wifia>  <green|red>"
#else
	led, 3, 1, do_led,
	"["
#ifdef CONFIG_BOARD_SPECIFIC_LED
#ifdef STATUS_LED_BIT
	"0|"
#endif
#ifdef STATUS_LED_BIT1
	"1|"
#endif
#ifdef STATUS_LED_BIT2
	"2|"
#endif
#ifdef STATUS_LED_BIT3
	"3|"
#endif
#endif
#ifdef STATUS_LED_GREEN
	"green|"
#endif
#ifdef STATUS_LED_YELLOW
	"yellow|"
#endif
#ifdef STATUS_LED_RED
	"red|"
#endif
#ifdef STATUS_LED_BLUE
	"blue|"
#endif
	"all] [on|off|toggle]",
	"[led_name] [on|off|toggle] sets or clears led(s)"
#endif
);
