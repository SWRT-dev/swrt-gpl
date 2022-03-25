#include <serial.h>
#include <common.h>
#include <config.h>

//#include <asm/arch/mt6735.h>
#include <asm/arch/mt_gpio.h>
#include <asm/arch/led.h>
#if defined(CONFIG_CMD_NMRP)
#include <net.h>
#endif

typedef struct {
    int led_id;
    int mask;
    int state;
    int delay_on; //ms
    int delay_off; //ms
    ulong cnt; //ms
} led_dev_t;

struct led_control_t;
struct led_control_inst_t;
static void ledctl_init_led_dev(struct led_control_inst_t *self);
static void ledctl_time_tick(struct led_control_inst_t *self, ulong timestamp);
static void ledctl_set_led_state(struct led_control_inst_t *self, int mask, int state, int delayon, int delayoff);
void led_test(int value);

typedef struct led_control_inst_t{
    struct led_control_t *class;
    int last_tick;
    led_dev_t led_dev[];
}led_control_inst;

typedef struct led_control_t{
    void (*init_led_dev)(led_control_inst *self);
    void (*time_tick)(led_control_inst *self, ulong timestamp);
    void (*set_led_state)(led_control_inst *self, int mask, int state, int delayon, int delayoff);
} led_control;

static led_control led_control_class = {
    .init_led_dev = ledctl_init_led_dev,
    .time_tick = ledctl_time_tick,
    .set_led_state = ledctl_set_led_state
};


static void ledctl_init_led_dev(led_control_inst *self)
{
    int idx;
printf("ledctl_init_led_dev......\n");
    for(idx = 0; self->led_dev[idx].led_id != -1; idx++)
    {
        led_dev_t *led = &self->led_dev[idx];
        mt_set_gpio_mode(led->led_id, GPIO_MODE_01);
        mt_set_gpio_dir(led->led_id, GPIO_DIR_OUT);
        if (led->state == STATUS_LED_ON)
        {
            mt_set_gpio_out(led->led_id, 0);
        }
        else if (led->state == STATUS_LED_OFF)
        {
            mt_set_gpio_out(led->led_id, 1);
        }
    }
}

static void ledctl_time_tick(led_control_inst *self, ulong timestamp)
{
    int idx;
    ulong interval = timestamp - self->last_tick;
    for(idx = 0; self->led_dev[idx].led_id != -1; idx++)
    {
        led_dev_t *led = &self->led_dev[idx];
        if (led->state != STATUS_LED_BLINKING)
        {
            continue;
        }
        led->cnt += interval;
        if (led->cnt < led->delay_on)
        {
            mt_set_gpio_out(led->led_id, 0);
        }
        else if(led->cnt < led->delay_on + led->delay_off)
        {
            mt_set_gpio_out(led->led_id, 1);
        }
        else
        {
            mt_set_gpio_out(led->led_id, 0);
            led->cnt = 0;
        }
    }
#if defined(CONFIG_CMD_NMRP)
    if (Nmrp_getState() == 5 /*NMRP_STATE_FILEUPLOADING*/)
    {
        nmrp_keepalive_send(timestamp);
    }
#endif
    self->last_tick = timestamp;
}

static void ledctl_set_led_state(led_control_inst *self, int mask, int state, int delayon, int delayoff)
{
    int idx;
    for(idx = 0; self->led_dev[idx].led_id != -1; idx++)
    {
        led_dev_t *led = &self->led_dev[idx];
        if(led->mask & mask)
        {
            if (led->state != state)
            {
                led->state = state;
                if (state == STATUS_LED_BLINKING)
                {
                    mt_set_gpio_out(led->led_id, 0);
                    led->delay_on = delayon;
                    led->delay_off = delayoff;
                }
                else if (state == STATUS_LED_ON)
                {
                    mt_set_gpio_out(led->led_id, 0);
                }
                else if (state == STATUS_LED_OFF)
                {
                    mt_set_gpio_out(led->led_id, 1);
                }
            }
            break;
        }
    }
};

led_control_inst led_dev_control = {
    .class = &led_control_class,
    .last_tick = 0,
    .led_dev = {
    {POWER_GREEN, POWER_GREEN_SELECTOR, STATUS_LED_OFF, 0, 0, 0},
    {POWER_RED, POWER_RED_SELECTOR, STATUS_LED_ON, 0, 0, 0},

    {NET_GREEN, NET_GREEN_SELECTOR, STATUS_LED_ON, 0, 0, 0},//pull up active
    {NET_RED, NET_RED_SELECTOR, STATUS_LED_OFF, 0, 0, 0},
    {WIFIN_GREEN, WIFIN_GREEN_SELECTOR, STATUS_LED_OFF, 0, 0, 0},
    {WIFIN_RED, WIFIN_RED_SELECTOR, STATUS_LED_OFF, 0, 0, 0},
    {WIFIA_GREEN, WIFIA_GREEN_SELECTOR, STATUS_LED_ON, 0, 0, 0},//pull up active
    {WIFIA_RED, WIFIA_RED_SELECTOR, STATUS_LED_OFF, 0, 0, 0},
    {-1, 0, 0, 0, 0}
    },
};

void red_led_on(void)
{
    led_dev_control.class->init_led_dev(&led_dev_control);
}

void led_set_state(int mask, int state, int delayon, int delayoff)
{
    led_dev_control.class->set_led_state(&led_dev_control, mask, state, delayon, delayoff);
}

void led_time_tick(ulong times)
{
    led_dev_control.class->time_tick(&led_dev_control, times);
}
