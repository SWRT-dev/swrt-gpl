#ifndef _ASUS_GPIO_H
#define _ASUS_GPIO_H

enum gpio_idx_e {
	RST_BTN = 0,
	WPS_BTN,
	USB_BTN,
	USB3_BTN,
	WIFI_SW_BTN,
	LED_ON_OFF_BTN,
	TURBO_BTN,
	FAN_SPEED,	/* RPM status, input */

	PWR_LED,
	PWR_RED_LED,
	WPS_LED,
	WIFI_2G_LED,
	WIFI_5G_LED,
	WIFI_5GH_LED,
	USB_LED,
	USB3_LED,
	SATA_LED,
	WAN_LED,	/* WHITE or BLUE, depends on platform. */
	WAN2_LED,	/* WHITE or BLUE, depends on platform. */
	WAN_RED_LED,
	WAN2_RED_LED,
	LAN_LED,
	FAIL_OVER_LED,
	SFPP_10G_LED,
	RJ45_10G_LED,
	TURBO_LED,
	WIFI_B_LED,
	WIFI_G_LED,
	WIFI_R_LED,
	WIFI_W_LED,
	ALL_LED,

	USB3_POWER,
	PHY_RESET,
	SWITCH_RESET,
	MALIBU_RESET,

	FAN_CTRL_H,	/* high-bit of FAN speed control pin. */
	FAN_CTRL_L,	/*  low-bit of FAN speed control pin. */

	GPIO_IDX_MAX,	/* Last item */
};

extern void led_init(void);
extern void gpio_init(void);
extern void led_onoff(enum gpio_idx_e gpio_idx, int onoff);
extern void power_led_on(void);
extern void power_led_off(void);
extern void leds_on(void);
extern void leds_off(void);
extern void all_leds_on(void);
extern void all_leds_off(void);
extern void asus_blue_led_on(void);
extern void asus_green_led_on(void);
extern void asus_green_led_off(void);
extern void asus_red_led_on(void);
extern void asus_red_led_off(void);
extern void asus_purple_led_on(void);
extern void asus_yellow_led_on(void);
extern unsigned long DETECT(void);
extern unsigned long DETECT_WPS(void);

#if defined(ALL_LED_OFF)
extern void enable_all_leds(void);
extern void disable_all_leds(void);
#else
static inline void enable_all_leds(void) { }
static inline void disable_all_leds(void) { }
#endif

#if defined(CONFIG_HAVE_WAN_RED_LED)
static inline void wan_red_led_on(void)
{
	led_onoff(WAN_RED_LED, 1);
	led_onoff(WAN2_RED_LED, 1);
}

static inline void wan_red_led_off(void)
{
	led_onoff(WAN_RED_LED, 0);
	led_onoff(WAN2_RED_LED, 0);
}
#else
static inline void wan_red_led_on(void) { }
static inline void wan_red_led_off(void) { }
#endif

#endif /* _ASUS_GPIO_H */
