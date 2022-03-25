// SPDX-License-Identifier: GPL-2.0
/*
 * Intel LED SSO driver
 *
 * Copyright (C) 2018 Intel Corporation.
 */
#include <linux/clk.h>
#include <linux/debugfs.h>
#include <linux/gpio/consumer.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/leds.h>
#include <linux/mfd/syscon.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/property.h>
#include <linux/platform_device.h>
#include <linux/regmap.h>
#include <linux/uaccess.h>

/* reg definition */
#define DUTY_CYCLE(x)	(0x8 + ((x) * 4))
#define LED_BLINK_H8_0	0x0
#define LED_BLINK_H8_1	0x4
#define SSO_CON0	0x2B0
#define SSO_CON1	0x2B4
#define SSO_CON2	0x2C4
#define SSO_CON3	0x2C8

/* CON0 */
#define RZFL		26
#define BLINK_R		30
/* BLINK H8 */
#define BLINK_FREQ_OFF(pin, src)	(((pin) * 6) + ((src) * 2))
#define BLINK_FREQ_MASK	0x3
#define BLINK_SRC_OFF(pin, src)		(((pin) * 6) + 4)
#define BLINK_SRC_MASK	0x3

/* Driver MACRO */
#define MAX_FREQ_RANK		10
#define GPTC_PRE_DIV		200
#define SSO_LED_MAX_NUM		32
#define BRIGHTNESS_MAX		255

enum {
	MAX_FPID_FREQ_RANK = 5, /* 1 - 4 */
	MAX_GPTC_FREQ_RANK = 9, /* 5 - 8 */
	MAX_GPTC_HS_FREQ_RANK = 10, /* 9 */
};

enum {
	LED_GRP0_PIN_MAX = 24,
	LED_GRP1_PIN_MAX = 29,
	LED_GRP2_PIN_MAX = 32,
};

enum {
	LED_GRP0_0_23,
	LED_GRP1_24_28,
	LED_GRP2_29_31,
	LED_GROUP_MAX,
};

enum {
	CLK_SRC_FPID = 0,
	CLK_SRC_GPTC = 1,
	CLK_SRC_GPTC_HS = 2,
};

enum {
	SSO_BRIGHTNESS_OFF = 0,
	SSO_DEF_BRIGHTNESS = 0x80,
	SSO_MAX_BRIGHTNESS = 0xFF
};

static const u32 freq_div_tbl[] = {4000, 2000, 1000, 800};

struct sso_led_priv;
/**
 * struct sso_led_desc
 *
 * @np: device tree node for this LED
 * @name: LED name
 * @default_trigger: LED trigger
 * @pin: gpio pin id/offset
 * @brighness: LED brightness/color
 * @blink_rate: LED HW blink frequency
 * @freq_idx: LED freqency idx of freq array of sso_led_priv
 * @retain_state_suspended: flag for LED class
 * @panic_indicator: flag for LED class
 * @hw_blink: HW blink
 * @hw_trig: HW control LED
 * @blinking: LED blink status
 */
struct sso_led_desc {
	struct device_node *np;
	const char *name;
	const char *default_trigger;
	u32 pin;
	unsigned int brightness;
	unsigned int blink_rate;
	u32 freq_idx;

	unsigned int retain_state_suspended:1;
	unsigned int retain_state_shutdown:1;
	unsigned int panic_indicator:1;
	unsigned int hw_blink:1;
	unsigned int hw_trig:1;
	unsigned int blinking:1;
};

/**
 * struct sso_led
 *
 * @list: LED struct list
 * @cdev: LED class device for this LED
 * @gpiod: gpio descriptor
 * @led: led settings for this LED
 * @priv: pointer to sso driver private data
 */
struct sso_led {
	struct list_head list;
	struct led_classdev cdev;
	struct gpio_desc *gpiod;
	struct sso_led_desc desc;
	struct sso_led_priv *priv;
};

/**
 *
 * @fpid_clk: sso slow clock
 * @fpid_clkrate: sso slow clock rate
 * @gptc_clkrate: sso fast clock rate
 * @brightness: default brightness for all leds
 * @blink_rate_idx: default brink rate idx of freq
 * @update_src: sso output source
 */
struct sso_cfg {
	struct clk *fpid_clk;
	u32 fpid_clkrate;
	u32 gptc_clkrate;
	u32 brightness;
	int blink_rate_idx;
};

/**
 * struct sso_led_priv
 *
 * @membase: SSO mapped memory resource
 * @dev: sso basic device
 * @pdev: sso platform device
 * @freq: sso frequency array
 * @cfg: sso DT configuration
 * @led_list: link list of leds of SSO
 * @debugfs: debugfs proc
 */
struct sso_led_priv {
	struct regmap *mmap;
	struct device *dev;
	struct platform_device *pdev;
	u32 freq[MAX_FREQ_RANK];
	struct sso_cfg cfg;
	struct list_head led_list;
	struct dentry *debugfs;
};

static void sso_led_shutdown(struct sso_led *led);

static int sso_led_writel(struct regmap *map, u32 reg, u32 val)
{
	return regmap_write(map, reg, val);
}

static int
sso_led_update_bit(struct regmap *map, u32 reg, u32 off, u32 val)
{
	val = !!val;

	return regmap_update_bits(map, reg, BIT(off), (val << off));
}

static int
sso_led_write_mask(struct regmap *map, u32 reg, u32 off, u32 mask, u32 val)
{
	u32 reg_val;

	if (regmap_read(map, reg, &reg_val))
		return -EINVAL;

	reg_val = (reg_val & ~(mask << off)) | ((val & mask) << off);

	return sso_led_writel(map, reg, reg_val);
}

static u32 sso_rectify_brightness(u32 brightness)
{
	if (brightness > BRIGHTNESS_MAX)
		return BRIGHTNESS_MAX;
	else
		return brightness;
}

static int sso_rectify_blink_rate(struct sso_led_priv *priv, u32 rate)
{
	int i;

	for (i = 0; i < MAX_FREQ_RANK; i++) {
		if (rate <= priv->freq[i])
			return i;
	}

	return i - 1;
}

static unsigned int sso_led_pin_to_group(u32 pin)
{
	if (pin < LED_GRP0_PIN_MAX)
		return LED_GRP0_0_23;
	else if (pin < LED_GRP1_PIN_MAX)
		return LED_GRP1_24_28;
	else
		return LED_GRP2_29_31;
}

static u32 sso_led_get_freq_src(int freq_idx)
{
	if (freq_idx < MAX_FPID_FREQ_RANK)
		return CLK_SRC_FPID;
	else if (freq_idx < MAX_GPTC_FREQ_RANK)
		return CLK_SRC_GPTC;
	else
		return CLK_SRC_GPTC_HS;
}

static u32 sso_led_pin_blink_off(u32 pin, unsigned int group)
{
	if (group == LED_GRP2_29_31)
		return pin - LED_GRP1_PIN_MAX;
	else if (group == LED_GRP1_24_28)
		return pin - LED_GRP0_PIN_MAX;
	else	/* led 0 - 23 in led 32 location */
		return SSO_LED_MAX_NUM - LED_GRP1_PIN_MAX;
}

static struct sso_led
*cdev_to_sso_led_data(struct led_classdev *led_cdev)
{
	return container_of(led_cdev, struct sso_led, cdev);
}

static void
sso_led_set_blink(struct sso_led_priv *priv, u32 pin, bool set)
{
	sso_led_update_bit(priv->mmap, SSO_CON2, pin, set);
}

static void
sso_led_set_hw_trigger(struct sso_led_priv *priv, u32 pin, bool set)
{
	sso_led_update_bit(priv->mmap, SSO_CON3, pin, set);
}

static void sso_led_freq_set(struct sso_led_priv *priv, u32 pin, int freq_idx)
{
	unsigned int group;
	u32 reg, off, freq_src, val_freq;

	if (!freq_idx)
		return;

	group = sso_led_pin_to_group(pin);
	freq_src = sso_led_get_freq_src(freq_idx);
	off = sso_led_pin_blink_off(pin, group);

	if (group == LED_GRP0_0_23)
		return;
	else if (group == LED_GRP1_24_28)
		reg = LED_BLINK_H8_0;
	else
		reg = LED_BLINK_H8_1;

	if (freq_src == CLK_SRC_FPID)
		val_freq = freq_idx - 1;
	else if (freq_src == CLK_SRC_GPTC)
		val_freq = freq_idx - MAX_FPID_FREQ_RANK;

	/* set blink rate idx */
	if (freq_src != CLK_SRC_GPTC_HS)
		sso_led_write_mask(priv->mmap, reg,
				   BLINK_FREQ_OFF(off, freq_src),
				   BLINK_FREQ_MASK, val_freq);

	/* select clock source */
	sso_led_write_mask(priv->mmap, reg,
			   BLINK_SRC_OFF(off, freq_src),
			   BLINK_SRC_MASK, freq_src);
}

static void sso_led_brightness_set(struct led_classdev *led_cdev,
				   enum led_brightness brightness)
{
	struct sso_led *led;
	struct sso_led_priv *priv;
	struct sso_led_desc *desc;
	int val;

	led = cdev_to_sso_led_data(led_cdev);
	priv = led->priv;
	desc = &led->desc;

	desc->brightness = brightness;
	sso_led_writel(priv->mmap, DUTY_CYCLE(desc->pin), brightness);

	if (brightness == LED_OFF)
		val = 0;
	else
		val = 1;

	/* HW blink off */
	if (desc->hw_blink && !val && desc->blinking) {
		desc->blinking = 0;
		sso_led_set_blink(priv, desc->pin, 0);
	} else if (desc->hw_blink && val && !desc->blinking) {
		desc->blinking = 1;
		sso_led_set_blink(priv, desc->pin, 1);
	}

	if (!desc->hw_trig && led->gpiod)
		gpiod_set_value(led->gpiod, val);
}

static enum led_brightness sso_led_brightness_get(struct led_classdev *led_cdev)
{
	struct sso_led *led;

	led = cdev_to_sso_led_data(led_cdev);

	return (enum led_brightness)led->desc.brightness;
}

static u32
delay_to_freq_idx(struct sso_led *led,
		  unsigned long *delay_on, unsigned long *delay_off)
{
	u32 freq_idx, freq;
	struct sso_led_priv *priv = led->priv;
	unsigned long delay;

	if (!*delay_on && !*delay_off) {
		*delay_on = *delay_off = (1000 / priv->freq[0]) / 2;
		return 0;
	}

	delay = *delay_on + *delay_off;
	freq = 1000 / delay;
	freq_idx = sso_rectify_blink_rate(priv, freq);
	delay = 1000 / priv->freq[freq_idx];
	*delay_on = *delay_off = delay / 2;

	if (!*delay_on)
		*delay_on = *delay_off = 1;

	return freq_idx;
}

static int sso_led_blink_set(struct led_classdev *led_cdev,
			     unsigned long *delay_on,
			     unsigned long *delay_off)
{
	struct sso_led *led;
	struct sso_led_priv *priv;
	u32 freq_idx;

	led = cdev_to_sso_led_data(led_cdev);
	priv = led->priv;
	freq_idx = delay_to_freq_idx(led, delay_on, delay_off);

	sso_led_freq_set(priv, led->desc.pin, freq_idx);
	/* Enable HW blink */
	sso_led_set_blink(priv, led->desc.pin, 1);
	led->desc.freq_idx = freq_idx;
	led->desc.blink_rate = priv->freq[freq_idx];
	led->desc.blinking = 1;

	return 1;
}

static void sso_led_hw_cfg(struct sso_led_priv *priv, struct sso_led *led)
{
	struct sso_led_desc *desc = &led->desc;

	/* set freq */
	if (desc->hw_blink) {
		sso_led_freq_set(priv, desc->pin, desc->freq_idx);
		sso_led_set_blink(priv, desc->pin, 1);
	}

	if (desc->hw_trig)
		sso_led_set_hw_trigger(priv, desc->pin, 1);

	/* set brightness */
	sso_led_writel(priv->mmap, DUTY_CYCLE(desc->pin), desc->brightness);

	/* enable output */
	if (!desc->hw_trig && desc->brightness)
		gpiod_set_value(led->gpiod, 1);
}

static int sso_create_led(struct sso_led_priv *priv, struct sso_led *led)
{
	struct sso_led_desc *desc = &led->desc;

	led->cdev.name = desc->name;
	led->cdev.default_trigger = desc->default_trigger;
	led->cdev.brightness_set = sso_led_brightness_set;
	led->cdev.brightness_get = sso_led_brightness_get;
	led->cdev.brightness = desc->brightness;
	led->cdev.max_brightness = BRIGHTNESS_MAX;

	if (desc->retain_state_suspended)
		led->cdev.flags |= LED_CORE_SUSPENDRESUME;
	if (desc->panic_indicator)
		led->cdev.flags |= LED_PANIC_INDICATOR;

	if (desc->hw_blink)
		led->cdev.blink_set = sso_led_blink_set;

	sso_led_hw_cfg(priv, led);

	if (devm_led_classdev_register(priv->dev, &led->cdev)) {
		dev_err(priv->dev, "register led class %s failed!\n",
			desc->name);
		return -EINVAL;
	}

	list_add(&led->list, &priv->led_list);

	return 0;
}

static int sso_led_dt_parse(struct sso_led_priv *priv)
{
	struct sso_led_desc *desc;
	struct sso_led *led;
	struct fwnode_handle *child;
	struct device *dev = priv->dev;
	struct list_head *p;
	int count;
	u32 prop;

	count = device_get_child_node_count(dev);
	if (!count)
		return 0;

	device_for_each_child_node(dev, child) {
		led = devm_kzalloc(dev, sizeof(*led), GFP_KERNEL);
		if (!led) {
			dev_err(dev, "sso led!\n");
			return -ENOMEM;
		}

		INIT_LIST_HEAD(&led->list);
		led->priv = priv;
		desc = &led->desc;

		desc->np = to_of_node(child);
		if (fwnode_property_read_string(child, "label", &desc->name)) {
			dev_err(dev, "LED no label name!\n");
			goto __dt_err;
		}

		/**
		 * v4.11
		 * update to devm_fwnode_get_gpiod_from_child
		 */
		led->gpiod = devm_get_gpiod_from_child(dev, "led", child);
		if (IS_ERR(led->gpiod)) {
			dev_err(dev, "led: %s get gpio fail!\n", desc->name);
			goto __dt_err;
		}

		fwnode_property_read_string(child, "linux,default-trigger",
					    &desc->default_trigger);

		if (fwnode_property_present(child, "retain_state_suspended"))
			desc->retain_state_suspended = 1;

		if (fwnode_property_present(child, "retain-state-shutdown"))
			desc->retain_state_shutdown = 1;

		if (fwnode_property_present(child, "panic-indicator"))
			desc->panic_indicator = 1;

		if (fwnode_property_read_u32(child, "intel,led-pin", &prop)) {
			dev_err(dev, "Failed to find led pin id!\n");
			goto __dt_err;
		} else {
			desc->pin = prop;
			if (desc->pin >= SSO_LED_MAX_NUM) {
				dev_err(dev, "invalid LED pin: %u\n",
					desc->pin);
				goto __dt_err;
			}
		}

		if (fwnode_property_present(child, "intel,sso-hw-blink"))
			desc->hw_blink = 1;

		if (fwnode_property_present(child, "intel,sso-hw-trigger"))
			desc->hw_trig = 1;

		if (desc->hw_trig) {
			desc->default_trigger = NULL;
			desc->retain_state_shutdown = 0;
			desc->retain_state_suspended = 0;
			desc->panic_indicator = 0;
			desc->hw_blink = 0;
		}

		if (fwnode_property_read_u32(child,
					     "intel,sso-blink-rate", &prop)) {
			/* default first freq rate */
			desc->freq_idx = priv->cfg.blink_rate_idx;
			desc->blink_rate = priv->freq[desc->freq_idx];
		} else {
			desc->freq_idx = sso_rectify_blink_rate(priv, prop);
			desc->blink_rate = priv->freq[desc->freq_idx];
		}

		if (fwnode_property_read_u32(child,
					     "intel,sso-brightness", &prop))
			desc->brightness = priv->cfg.brightness;
		else
			desc->brightness = sso_rectify_brightness(prop);

		if (sso_create_led(priv, led))
			goto __dt_err;
	}

	return 0;

__dt_err:
	fwnode_handle_put(child);
	/* unregister leds */
	list_for_each(p, &priv->led_list) {
		led = list_entry(p, struct sso_led, list);
		sso_led_shutdown(led);
	}
	return -EINVAL;
}

static int sso_dt_parse(struct sso_led_priv *priv)
{
	struct sso_cfg *cfg;
	struct device *dev = priv->dev;
	u32 prop;

	cfg = &priv->cfg;

	if (device_property_read_u32(dev, "intel,sso-def-brightness", &prop)) {
		cfg->brightness = SSO_DEF_BRIGHTNESS;
	} else {
		cfg->brightness = prop;
		if (cfg->brightness > SSO_MAX_BRIGHTNESS)
			cfg->brightness = SSO_MAX_BRIGHTNESS;
	}

	if (device_property_read_u32(dev, "intel,sso-def-blinkrate", &prop))
		cfg->blink_rate_idx = 0;
	else
		cfg->blink_rate_idx = sso_rectify_blink_rate(priv, prop);

	if (sso_led_dt_parse(priv))
		return -EINVAL;

	dev_dbg(priv->dev, "sso LED init success!\n");
	return 0;
}

static void sso_init_freq(struct sso_led_priv *priv)
{
	int i;
	struct sso_cfg *cfg = &priv->cfg;

	priv->freq[0] = 0;
	for (i = 1; i < MAX_FREQ_RANK; i++) {
		if (i < MAX_FPID_FREQ_RANK) {
			priv->freq[i] = cfg->fpid_clkrate / freq_div_tbl[i - 1];
		} else if (i < MAX_GPTC_FREQ_RANK) {
			priv->freq[i] = cfg->gptc_clkrate /
				freq_div_tbl[i - MAX_FPID_FREQ_RANK];
		} else if (i < MAX_GPTC_HS_FREQ_RANK) {
			priv->freq[i] = cfg->gptc_clkrate;
		}
	}

	/* debug dump */
	for (i = 0; i < MAX_FREQ_RANK; i++)
		dev_dbg(priv->dev, "SSO freq[%d]: %u\n", i, priv->freq[i]);
}

static void sso_led_shutdown(struct sso_led *led)
{
	struct sso_led_priv *priv;

	priv = led->priv;

	/* unregister led */
	devm_led_classdev_unregister(priv->dev, &led->cdev);

	/* clear HW control bit */
	if (led->desc.hw_trig)
		sso_led_set_hw_trigger(priv, led->desc.pin, 0);

	if (led->gpiod)
		devm_gpiod_put(priv->dev, led->gpiod);

	led->priv = NULL;
}

#if defined(CONFIG_DEBUG_FS)

static ssize_t
sso_led_create_write(struct file *s, const char __user *buffer,
		     size_t count, loff_t *pos)
{
	char buf[128] = {0};
	char *start, *led_name;
	u32 pin, blink_rate;
	struct sso_led_priv *priv;
	struct sso_led *led;
	struct sso_led_desc *desc;
	struct device *dev;
	int i;

	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;

	priv = file_inode(s)->i_private;
	dev = priv->dev;
	copy_from_user(buf, buffer, sizeof(buf) - 1);

	if (strcmp(buf, "help") == 0)
		goto __create_help;

	for (i = 0; buf[i] != ' ' && buf[i] != 0; i++)
		;
	if (buf[i] == 0)
		goto __create_help;
	buf[i++] = 0;
	start = &buf[i];
	for (; buf[i] >= '0' && buf[i] <= '9'; i++)
		;
	buf[i] = 0;

	if (kstrtou32(buf, 0, &pin)) {
		dev_err(dev, "Failed to parse led pin!\n");
		goto __create_help;
	}

	if (kstrtou32(start, 0, &blink_rate)) {
		dev_err(dev, "Failed to parse led blink rate!\n");
		goto __create_help;
	}

	if (pin >= SSO_LED_MAX_NUM)
		return -EINVAL;

	led = devm_kzalloc(dev, sizeof(*led), GFP_KERNEL);
	if (!led)
		return -ENOMEM;

	INIT_LIST_HEAD(&led->list);
	led->priv = priv;
	desc = &led->desc;

	led_name = devm_kmalloc(dev, 32, GFP_KERNEL);
	if (!led_name)
		return -ENOMEM;

	sprintf(led_name, "sso-led-%u", pin);
	desc->name = (const char *)led_name;

	if (blink_rate)
		desc->hw_blink = 1;
	else
		desc->hw_blink = 0;
	desc->freq_idx = sso_rectify_blink_rate(priv, blink_rate);
	desc->blink_rate = priv->freq[desc->freq_idx];
	desc->brightness = 0;
	desc->pin = pin;
	desc->hw_trig = 0;

	if (sso_create_led(priv, led))
		return -EINVAL;

	return count;

__create_help:
	dev_info(priv->dev, "echo <pin> <blink rate> > create\n");
	return count;
}

static const struct file_operations sso_led_create_fops = {
	.write = sso_led_create_write,
	.llseek = noop_llseek,
};

static ssize_t
sso_led_delete_write(struct file *s, const char __user *buffer,
		     size_t count, loff_t *pos)
{
	char buf[32] = {0};
	u32 pin;
	struct sso_led_priv *priv;
	struct list_head *p, *n;
	struct sso_led *led;
	int i;

	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;

	priv = file_inode(s)->i_private;
	copy_from_user(buf, buffer, sizeof(buf) - 1);

	if (strcmp(buf, "help") == 0)
		goto __delete_err;

	for (i = 0; buf[i] >= '0' && buf[i] <= '9'; i++)
		;
	buf[i] = 0;

	if (kstrtou32(buf, 0, &pin))
		goto __delete_err;

	list_for_each_safe(p, n, &priv->led_list) {
		led = list_entry(p, struct sso_led, list);
		if (led->desc.pin == pin && !led->gpiod) {
			list_del(p);
			sso_led_shutdown(led);
			devm_kfree(priv->dev, (void *)led->desc.name);
			devm_kfree(priv->dev, led);
			break;
		}
	}

	return count;

__delete_err:
	dev_info(priv->dev, "echo <pin> > delete\n");
	return count;
}

static const struct file_operations sso_led_delete_fops = {
	.write = sso_led_delete_write,
	.llseek = noop_llseek,
};

static void *sso_led_show_seq_start(struct seq_file *s, loff_t *pos)
{
	struct sso_led_priv *priv = s->private;

	return seq_list_start(&priv->led_list, *pos);
}

static void *sso_led_show_seq_next(struct seq_file *s, void *v, loff_t *pos)
{
	struct sso_led_priv *priv = s->private;

	return seq_list_next(v, &priv->led_list, pos);
}

static void sso_led_show_seq_stop(struct seq_file *s, void *v)
{
}

static int sso_led_show_seq_show(struct seq_file *s, void *v)
{
	struct list_head *p = v;
	struct sso_led *led;
	struct sso_led_desc *desc;

	led = list_entry(p, struct sso_led, list);
	desc = &led->desc;

	seq_printf(s, "-------------%s-----------------------\n", desc->name);
	seq_printf(s, "pin: %u\n", desc->pin);
	seq_printf(s, "brightness : %u\n", desc->brightness);
	seq_printf(s, "blink rate: %u\n", desc->blink_rate);
	if (desc->default_trigger)
		seq_printf(s, "default trigger: %s\n", desc->default_trigger);
	seq_printf(s, "%s driven LED\n", (!desc->hw_trig) ? "SW" : "HW");
	seq_printf(s, "%s blinking\n", (!desc->hw_blink) ? "SW" : "HW");
	seq_printf(s, "blinking status: %s\n", (desc->blinking) ? "Yes" : "No");

	return 0;
}

static const struct seq_operations sso_led_show_seq_ops = {
	.start = sso_led_show_seq_start,
	.next = sso_led_show_seq_next,
	.stop = sso_led_show_seq_stop,
	.show = sso_led_show_seq_show,
};

static int sso_led_show_seq_open(struct inode *inode, struct file *file)
{
	struct seq_file *s;
	int ret;

	ret = seq_open(file, &sso_led_show_seq_ops);
	if (!ret) {
		s = file->private_data;
		s->private = inode->i_private;
	}

	return ret;
}

static const struct file_operations sso_led_show_fops = {
	.open = sso_led_show_seq_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = seq_release,
};

static int sso_led_proc_init(struct sso_led_priv *priv)
{
	char sso_led_dir[64] = {0};
	struct dentry *file;

	strlcpy(sso_led_dir, priv->dev->of_node->name, sizeof(sso_led_dir));
	priv->debugfs = debugfs_create_dir(sso_led_dir, NULL);

	if (!priv->debugfs)
		return -ENOMEM;

	file = debugfs_create_file("create", 0200, priv->debugfs,
				   priv, &sso_led_create_fops);
	if (!file)
		goto __proc_err;

	file = debugfs_create_file("delete", 0200, priv->debugfs,
				   priv, &sso_led_delete_fops);
	if (!file)
		goto __proc_err;

	file = debugfs_create_file("show", 0400, priv->debugfs,
				   priv, &sso_led_show_fops);
	if (!file)
		goto __proc_err;

	return 0;

__proc_err:
	debugfs_remove_recursive(priv->debugfs);
	return -ENOMEM;
}

static void sso_led_proc_exit(struct sso_led_priv *priv)
{
	debugfs_remove_recursive(priv->debugfs);
	priv->debugfs = NULL;
}
#else
static int sso_led_proc_init(struct sso_led_priv *priv)
{
	return 0;
}

static void sso_led_proc_exit(struct sso_led_priv *priv)
{
}

#endif /* CONFIG_DEBUG_FS */

static int intel_sso_led_probe(struct platform_device *pdev)
{
	struct sso_led_priv *priv;
	struct device *dev = &pdev->dev;
	struct sso_cfg *cfg;
	u32 prop;

	priv = devm_kzalloc(dev, sizeof(*priv), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	INIT_LIST_HEAD(&priv->led_list);

	/* led mem */
	priv->mmap = syscon_node_to_regmap(dev->parent->of_node);
	if (IS_ERR_OR_NULL(priv->mmap)) {
		dev_err(dev, "Failed to get sso led iomem");
		return PTR_ERR(priv->mmap);
	}

	cfg = &priv->cfg;
	cfg->fpid_clk = devm_clk_get(dev, "fpid");
	if (IS_ERR(cfg->fpid_clk)) {
		dev_err(dev, "Failed to get fpid clock!\n");
		goto __hw_err;
	}
	if (clk_prepare_enable(cfg->fpid_clk)) {
		dev_err(dev, "Failed to enable fpid clock!\n");
		goto __hw_err;
	}
	cfg->fpid_clkrate = clk_get_rate(cfg->fpid_clk);

	if (device_property_read_u32(dev, "intel,gptc-clkrate", &prop)) {
		dev_err(dev, "Failed to get gptc clock rate");
		goto __hw_err;
	} else {
		cfg->gptc_clkrate = prop;
	}

	priv->dev = dev;
	priv->pdev = pdev;
	platform_set_drvdata(pdev, priv);

	sso_init_freq(priv);
	if (sso_dt_parse(priv))
		goto __hw_err;

	if (sso_led_proc_init(priv))
		goto __hw_err;

	return 0;

__hw_err:
	regmap_exit(priv->mmap);
	return -EINVAL;
}

static int intel_sso_led_remove(struct platform_device *pdev)
{
	struct sso_led_priv *priv;
	struct sso_led *led;
	struct list_head *pos, *n;

	priv = platform_get_drvdata(pdev);

	list_for_each_safe(pos, n, &priv->led_list) {
		list_del(pos);
		led = list_entry(pos, struct sso_led, list);
		sso_led_shutdown(led);
	}

	clk_disable_unprepare(priv->cfg.fpid_clk);
	regmap_exit(priv->mmap);

	sso_led_proc_exit(priv);

	return 0;
}

static const struct of_device_id of_sso_led_match[] = {
	{ .compatible = "intel,sso-led" },
	{},
};

MODULE_DEVICE_TABLE(of, of_sso_led_match);

static struct platform_driver intel_sso_led_driver = {
	.probe		= intel_sso_led_probe,
	.remove		= intel_sso_led_remove,
	.driver		= {
			.name = "intel-sso-led",
			.of_match_table = of_match_ptr(of_sso_led_match),
	},
};

module_platform_driver(intel_sso_led_driver);

MODULE_AUTHOR("Zhu Yixin <Yixin.zhu@intel.com>");
MODULE_DESCRIPTION("Intel SSO LED driver");
MODULE_LICENSE("GPL v2");
