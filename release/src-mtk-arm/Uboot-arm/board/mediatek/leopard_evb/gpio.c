/*
 * (C) Copyright 2018 MediaTek.Inc
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <asm/arch/leopard.h>
#include <asm/arch/gpio.h>
#include <common.h>

static const struct mtk_pin_info *
mtk_pinctrl_get_gpio_array(int pin, int size,
			   const struct mtk_pin_info p_array[])
{
	unsigned long i = 0;

	for (i = 0; i < size; i++) {
		if (pin == p_array[i].pin)
			return &p_array[i];
	}
	return NULL;
}

int mtk_pinctrl_set_gpio_value(int pin, int value, int size,
			       const struct mtk_pin_info pin_info[])
{
	unsigned int reg_set_addr, reg_rst_addr;
	const struct mtk_pin_info *spec_pin_info;

	spec_pin_info = mtk_pinctrl_get_gpio_array(pin, size, pin_info);

	if (spec_pin_info != NULL) {
		reg_set_addr = spec_pin_info->offset + 4;
		reg_rst_addr = spec_pin_info->offset + 8;
		if (value)
			GPIO_SET_BITS((1L << spec_pin_info->bit), (GPIO_BASE + reg_set_addr));
		else
			GPIO_SET_BITS((1L << spec_pin_info->bit), (GPIO_BASE + reg_rst_addr));
	} else {
		return -ERINVAL;
	}

	return 0;
}

int mtk_pinctrl_get_gpio_value(int pin, int size,
			       const struct mtk_pin_info pin_info[])
{
	unsigned int reg_value, reg_get_addr;
	const struct mtk_pin_info *spec_pin_info;
	unsigned char bit_width, reg_bit;

	spec_pin_info = mtk_pinctrl_get_gpio_array(pin, size, pin_info);

	if (spec_pin_info != NULL) {
		reg_get_addr = spec_pin_info->offset;
		bit_width = spec_pin_info->width;
		reg_bit = spec_pin_info->bit;
		reg_value = GPIO_RD32(GPIO_BASE+  reg_get_addr);
		return ((reg_value >> reg_bit) & ((1 << bit_width) - 1));
	} else {
		return -ERINVAL;
	}
}

int mtk_pinctrl_update_gpio_value(int pin, unsigned char value, int size,
				  const struct mtk_pin_info pin_info[])
{
	unsigned int reg_update_addr;
	unsigned int mask, reg_value;
	const struct mtk_pin_info *spec_update_pin;
	unsigned char bit_width;

	spec_update_pin = mtk_pinctrl_get_gpio_array(pin, size, pin_info);

	if (spec_update_pin != NULL) {
		reg_update_addr = spec_update_pin->offset;
		bit_width = spec_update_pin->width;
		mask = ((1 << bit_width) - 1) << spec_update_pin->bit;
		reg_value = GPIO_RD32(GPIO_BASE + reg_update_addr);
		reg_value &= ~(mask);
		reg_value |= (value << spec_update_pin->bit);
		GPIO_WR32((GPIO_BASE + reg_update_addr),reg_value);
	} else {
		return -ERINVAL;
	}
	return 0;
}

int mt_set_gpio_dir_chip(unsigned long pin, unsigned long dir)
{
	if (pin >= MAX_GPIO_PIN)
		return -ERINVAL;

	if (dir >= GPIO_DIR_MAX)
		return -ERINVAL;

	mtk_pinctrl_set_gpio_value(pin, dir, MAX_GPIO_PIN, mtk_pin_info_dir);
	return RSUCCESS;

}

int mt_get_gpio_dir_chip(unsigned long pin)
{
	if (pin >= MAX_GPIO_PIN)
		return -ERINVAL;

	return mtk_pinctrl_get_gpio_value(pin, MAX_GPIO_PIN, mtk_pin_info_dir);
}

int mt_set_gpio_smt_chip(unsigned long pin, unsigned long enable)
{
	if (pin >= MAX_GPIO_PIN)
		return -ERINVAL;

	mtk_pinctrl_set_gpio_value(pin, enable, MAX_GPIO_PIN, mtk_pin_info_smt);
	return RSUCCESS;
}

int mt_get_gpio_smt_chip(unsigned long pin)
{
	if (pin >= MAX_GPIO_PIN)
		return -ERINVAL;

	return mtk_pinctrl_get_gpio_value(pin, MAX_GPIO_PIN, mtk_pin_info_smt);
}

int mt_set_gpio_ies_chip(unsigned long pin, unsigned long enable)
{
	if (pin >= MAX_GPIO_PIN)
		return -ERINVAL;

	mtk_pinctrl_set_gpio_value(pin, enable, MAX_GPIO_PIN, mtk_pin_info_ies);
	return RSUCCESS;
}

int mt_get_gpio_ies_chip(unsigned long pin)
{
	if (pin >= MAX_GPIO_PIN)
		return -ERINVAL;

	return mtk_pinctrl_get_gpio_value(pin, MAX_GPIO_PIN, mtk_pin_info_ies);
}

int mt_set_gpio_pull_select_chip(unsigned long pin, unsigned long select)
{
	if (pin >= MAX_GPIO_PIN)
		return -ERINVAL;

	if (select == 1)
		mtk_pinctrl_set_gpio_value(pin, 1, MAX_GPIO_PIN,
					   mtk_pin_info_pullsel);
	else
		mtk_pinctrl_set_gpio_value(pin, 0, MAX_GPIO_PIN,
					   mtk_pin_info_pullsel);

	return RSUCCESS;
}

int mt_get_gpio_pull_select_chip(unsigned long pin)
{
	 unsigned long pull_sel;

	 pull_sel = mtk_pinctrl_get_gpio_value(pin, MAX_GPIO_PIN,
					       mtk_pin_info_pullsel);
	 return pull_sel;
}

int mt_set_gpio_pull_enable_chip(unsigned long pin, unsigned long enable)
{
	if (pin >= MAX_GPIO_PIN)
		return -ERINVAL;

	if (enable == 1)
		mtk_pinctrl_set_gpio_value(pin, 1, MAX_GPIO_PIN,
					   mtk_pin_info_pullen);
	else
		mtk_pinctrl_set_gpio_value(pin, 0, MAX_GPIO_PIN,
					   mtk_pin_info_pullen);

	return RSUCCESS;
}

int mt_get_gpio_pull_enable_chip(unsigned long pin)
{
	unsigned long pullen_val;

	pullen_val = mtk_pinctrl_get_gpio_value(pin, MAX_GPIO_PIN,
						mtk_pin_info_pullsel);
	return pullen_val;
}

int mt_set_gpio_out_chip(unsigned long pin, unsigned long output)
{
	if (pin >= MAX_GPIO_PIN)
		return -ERINVAL;

	if (output >= GPIO_OUT_MAX)
		return -ERINVAL;

	mtk_pinctrl_set_gpio_value(pin, output, MAX_GPIO_PIN,
				   mtk_pin_info_dataout);
	return RSUCCESS;
}

int mt_get_gpio_out_chip(unsigned long pin)
{
	if (pin >= MAX_GPIO_PIN)
		return -ERINVAL;

	return mtk_pinctrl_get_gpio_value(pin, MAX_GPIO_PIN,
					  mtk_pin_info_dataout);
}

int mt_get_gpio_in_chip(unsigned long pin)
{
	if (pin >= MAX_GPIO_PIN)
		return -ERINVAL;

	return mtk_pinctrl_get_gpio_value(pin, MAX_GPIO_PIN, mtk_pin_info_datain);
}

int mt_set_gpio_mode_chip(unsigned long pin, unsigned long mode)
{
	if (pin >= MAX_GPIO_PIN)
		return -ERINVAL;

	if (mode >= GPIO_MODE_MAX)
		return -ERINVAL;

	mtk_pinctrl_update_gpio_value(pin, mode, MAX_GPIO_PIN,
				      mtk_pin_info_mode);
	return RSUCCESS;
}

int mt_get_gpio_mode_chip(unsigned long pin)
{
	if (pin >= MAX_GPIO_PIN)
		return -ERINVAL;

	return mtk_pinctrl_get_gpio_value(pin, MAX_GPIO_PIN,
					  mtk_pin_info_mode);
}

int mt_set_gpio_drving_chip(unsigned long pin,unsigned long drv)
{
	if (pin >= MAX_GPIO_PIN)
		return -ERINVAL;

	if (drv >= GPIO_DRIVE_MAX)
		return -ERINVAL;

	mtk_pinctrl_update_gpio_value(pin, drv, MAX_GPIO_PIN,
				      mtk_pin_info_drv);
	return RSUCCESS;
}

int mt_get_gpio_drving_chip(unsigned long pin)
{
	if (pin >= MAX_GPIO_PIN)
		return -ERINVAL;

	return mtk_pinctrl_get_gpio_value(pin, MAX_GPIO_PIN,
					  mtk_pin_info_drv);
}

#ifndef CONFIG_SPL_BUILD
void mt_gpio_status_dump(void)
{
	int i=0;

	printf("gpio dump status begin ++++++++++++++++++++++++++++\n");
	for(i = 0; i < MAX_GPIO_PIN; i++) {
		printf("g[%d]\n",i);
		printf("g[%d], mode(%x)\n",i,mt_get_gpio_mode(i));
		printf("g[%d], dir(%x)\n",i,mt_get_gpio_dir(i));
		printf("g[%d], pull_en(%x)\n",i,mt_get_gpio_pull_enable(i));
		printf("g[%d], pull_sel(%x)\n",i,mt_get_gpio_pull_select(i));
		printf("g[%d], out(%x)\n",i,mt_get_gpio_out(i));
		printf("g[%d], in(%x)\n",i,mt_get_gpio_in(i));
		printf("g[%d], smt(%x)\n",i,mt_get_gpio_smt(i));
		printf("g[%d], ies(%x)\n",i,mt_get_gpio_ies(i));
		printf("g[%d], drv(%x)\n",i,mt_get_gpio_drving(i));

	}
	printf("gpio dump status end  ---------------------------------------\n");
}
#endif

int mt_set_gpio_dir(unsigned long pin, unsigned long dir)
{
	return mt_set_gpio_dir_chip(pin, dir);
}

int mt_get_gpio_dir(unsigned long pin)
{
	return mt_get_gpio_dir_chip(pin);
}

int mt_set_gpio_pull_select(unsigned long pin, unsigned long select)
{
	return mt_set_gpio_pull_select_chip(pin, select);
}

int mt_get_gpio_pull_select(unsigned long pin)
{
	return mt_get_gpio_pull_select_chip(pin);
}

int mt_set_gpio_pull_enable(unsigned long pin, unsigned long enable)
{
	return mt_set_gpio_pull_enable_chip(pin, enable);
}

int mt_get_gpio_pull_enable(unsigned long pin)
{
	return mt_get_gpio_pull_enable_chip(pin);
}

int mt_set_gpio_pullen(unsigned long pin, unsigned long enable)
{
	return mt_set_gpio_pull_enable_chip(pin, enable);
}

int mt_get_gpio_pullen(unsigned long pin)
{
	return mt_get_gpio_pull_enable_chip(pin);
}

int mt_set_gpio_smt(unsigned long pin, unsigned long enable)
{
	return mt_set_gpio_smt_chip(pin, enable);
}

int mt_get_gpio_smt(unsigned long pin)
{
	return mt_get_gpio_smt_chip(pin);
}

int mt_set_gpio_ies(unsigned long pin, unsigned long enable)
{
	return mt_set_gpio_ies_chip(pin, enable);
}

int mt_get_gpio_ies(unsigned long pin)
{
	return mt_get_gpio_ies_chip(pin);
}

int mt_set_gpio_out(unsigned long pin, unsigned long output)
{
	return mt_set_gpio_out_chip(pin, output);
}

int mt_get_gpio_out(unsigned long pin)
{
	return mt_get_gpio_out_chip(pin);
}

int mt_get_gpio_in(unsigned long pin)
{
	return mt_get_gpio_in_chip(pin);
}

int mt_set_gpio_mode(unsigned long pin, unsigned long mode)
{
	return mt_set_gpio_mode_chip(pin, mode);
}

int mt_get_gpio_mode(unsigned long pin)
{
	return mt_get_gpio_mode_chip(pin);
}

int mt_set_gpio_drving(unsigned long pin, unsigned long drv)
{
	return mt_set_gpio_drving_chip(pin, drv);
}

int mt_get_gpio_drving(unsigned long pin)
{
	return mt_get_gpio_drving_chip(pin);
}

#ifndef CONFIG_SPL_BUILD
void mt_get_gpio_status_dump(void)
{
      mt_gpio_status_dump();
}
#endif
