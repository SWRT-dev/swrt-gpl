#ifndef __LEOPARD_EVB_API_LED_H__
#define __LEOPARD_EVB_API_LED_H__

#define STATUS_LED_OFF      0
#define STATUS_LED_BLINKING 1
#define STATUS_LED_ON       2

#define POWER_GREEN (4)
#define POWER_RED (3)

#define NET_GREEN (20)
#define NET_RED (17)

#define WIFIN_GREEN (85)
#define WIFIN_RED (86)

#define WIFIA_GREEN (2)
#define WIFIA_RED (1)

#define POWER_GREEN_SELECTOR 0x1
#define POWER_RED_SELECTOR 0x2
#define POWER_AMBER_SELECTOR 0x4

#define NET_GREEN_SELECTOR 0x8
#define NET_RED_SELECTOR 0x10
#define NET_AMBER_SELECTOR 0x20

#define WIFIN_GREEN_SELECTOR 0x40
#define WIFIN_RED_SELECTOR 0x80
#define WIFIN_AMBER_SELECTOR 0x100

#define WIFIA_GREEN_SELECTOR 0x200
#define WIFIA_RED_SELECTOR 0x400
#define WIFIA_AMBER_SELECTOR 0x800


void led_set_state(int mask, int state, int delayon, int delayoff);
void led_time_tick(ulong times);
void led_test(int value);
/* demo
 led_set_state(POWER_AMBER_SELECTOR, STATUS_LED_BLINKING, 250, 750);

 //poll call
 led_time_tick(current_time); 
*/

#endif
