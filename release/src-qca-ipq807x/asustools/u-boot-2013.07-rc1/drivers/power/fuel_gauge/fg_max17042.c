/*
 *  Copyright (C) 2012 Samsung Electronics
 *  Lukasz Majewski <l.majewski@samsung.com>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <power/pmic.h>
#include <power/max17042_fg.h>
#include <i2c.h>
#include <power/max8997_pmic.h>
#include <power/power_chrg.h>
#include <power/battery.h>
#include <power/fg_battery_cell_params.h>
#include <errno.h>

static int fg_write_regs(struct pmic *p, u8 addr, u16 *data, int num)
{
	int ret = 0;
	int i;

	for (i = 0; i < num; i++, addr++)
		ret |= pmic_reg_write(p, addr, *(data + i));

	return ret;
}

static int fg_read_regs(struct pmic *p, u8 addr, u16 *data, int num)
{
	int ret = 0;
	int i;

	for (i = 0; i < num; i++, addr++)
		ret |= pmic_reg_read(p, addr, (u32 *) (data + i));

	return ret;
}

static int fg_write_and_verify(struct pmic *p, u8 addr, u16 data)
{
	unsigned int val = data;
	int ret = 0;

	ret |= pmic_reg_write(p, addr, val);
	ret |= pmic_reg_read(p, addr, &val);

	if (ret)
		return ret;

	if (((u16) val) == data)
		return 0;

	return -1;
}

static void por_fuelgauge_init(struct pmic *p)
{
	u16 r_data0[16], r_data1[16], r_data2[16];
	u32 rewrite_count = 5, i = 0;
	unsigned int val;
	int ret = 0;

	/* Delay 500 ms */
	mdelay(500);
	/* Initilize Configuration */
	pmic_reg_write(p, MAX17042_CONFIG, 0x2310);

rewrite_model:
	/* Unlock Model Access */
	pmic_reg_write(p, MAX17042_MLOCKReg1, MODEL_UNLOCK1);
	pmic_reg_write(p, MAX17042_MLOCKReg2, MODEL_UNLOCK2);

	/* Write/Read/Verify the Custom Model */
	ret |= fg_write_regs(p, MAX17042_MODEL1, cell_character0,
			     ARRAY_SIZE(cell_character0));
	ret |= fg_write_regs(p, MAX17042_MODEL2, cell_character1,
			     ARRAY_SIZE(cell_character1));
	ret |= fg_write_regs(p, MAX17042_MODEL3, cell_character2,
			     ARRAY_SIZE(cell_character2));

	if (ret) {
		printf("%s: Cell parameters write failed!\n", __func__);
		return;
	}

	ret |= fg_read_regs(p, MAX17042_MODEL1, r_data0, ARRAY_SIZE(r_data0));
	ret |= fg_read_regs(p, MAX17042_MODEL2, r_data1, ARRAY_SIZE(r_data1));
	ret |= fg_read_regs(p, MAX17042_MODEL3, r_data2, ARRAY_SIZE(r_data2));

	if (ret)
		printf("%s: Cell parameters read failed!\n", __func__);

	for (i = 0; i < 16; i++) {
		if ((cell_character0[i] != r_data0[i])
		    || (cell_character1[i] != r_data1[i])
		    || (cell_character2[i] != r_data2[i]))
			goto rewrite_model;
		}

	/* Lock model access */
	pmic_reg_write(p, MAX17042_MLOCKReg1, MODEL_LOCK1);
	pmic_reg_write(p, MAX17042_MLOCKReg2, MODEL_LOCK2);

	/* Verify the model access is locked */
	ret |= fg_read_regs(p, MAX17042_MODEL1, r_data0, ARRAY_SIZE(r_data0));
	ret |= fg_read_regs(p, MAX17042_MODEL2, r_data1, ARRAY_SIZE(r_data1));
	ret |= fg_read_regs(p, MAX17042_MODEL3, r_data2, ARRAY_SIZE(r_data2));

	if (ret) {
		printf("%s: Cell parameters read failed!\n", __func__);
		return;
	}

	for (i = 0; i < ARRAY_SIZE(r_data0); i++) {
		/* Check if model locked */
		if (r_data0[i] || r_data1[i] || r_data2[i]) {
			/* Rewrite model data - prevent from endless loop */
			if (rewrite_count--) {
				puts("FG - Lock model access failed!\n");
				goto rewrite_model;
			}
		}
	}

	/* Write Custom Parameters */
	fg_write_and_verify(p, MAX17042_RCOMP0, RCOMP0);
	fg_write_and_verify(p, MAX17042_TEMPCO, TempCo);

	/* Delay at least 350mS */
	mdelay(350);

	/* Initialization Complete */
	pmic_reg_read(p, MAX17042_STATUS, &val);
	/* Write and Verify Status with POR bit Cleared */
	fg_write_and_verify(p, MAX17042_STATUS, val & ~MAX17042_POR);

	/* Delay at least 350 ms */
	mdelay(350);
}

static int power_update_battery(struct pmic *p, struct pmic *bat)
{
	struct power_battery *pb = bat->pbat;
	unsigned int val;
	int ret = 0;

	if (pmic_probe(p)) {
		puts("Can't find max17042 fuel gauge\n");
		return -1;
	}

	ret |= pmic_reg_read(p, MAX17042_VFSOC, &val);
	pb->bat->state_of_chrg = (val >> 8);

	pmic_reg_read(p, MAX17042_VCELL, &val);
	debug("vfsoc: 0x%x\n", val);
	pb->bat->voltage_uV = ((val & 0xFFUL) >> 3) + ((val & 0xFF00) >> 3);
	pb->bat->voltage_uV = (pb->bat->voltage_uV * 625);

	pmic_reg_read(p, 0x05, &val);
	pb->bat->capacity = val >> 2;

	return ret;
}

static int power_check_battery(struct pmic *p, struct pmic *bat)
{
	struct power_battery *pb = bat->pbat;
	unsigned int val;
	int ret = 0;

	if (pmic_probe(p)) {
		puts("Can't find max17042 fuel gauge\n");
		return -1;
	}

	ret |= pmic_reg_read(p, MAX17042_STATUS, &val);
	debug("fg status: 0x%x\n", val);

	if (val == MAX17042_POR)
		por_fuelgauge_init(p);

	ret |= pmic_reg_read(p, MAX17042_VERSION, &val);
	pb->bat->version = val;

	power_update_battery(p, bat);
	debug("fg ver: 0x%x\n", pb->bat->version);
	printf("BAT: state_of_charge(SOC):%d%%\n",
	       pb->bat->state_of_chrg);

	printf("     voltage: %d.%6.6d [V] (expected to be %d [mAh])\n",
	       pb->bat->voltage_uV / 1000000,
	       pb->bat->voltage_uV % 1000000,
	       pb->bat->capacity);

	if (pb->bat->voltage_uV > 3850000)
		pb->bat->state = EXT_SOURCE;
	else if (pb->bat->voltage_uV < 3600000 || pb->bat->state_of_chrg < 5)
		pb->bat->state = CHARGE;
	else
		pb->bat->state = NORMAL;

	return ret;
}

static struct power_fg power_fg_ops = {
	.fg_battery_check = power_check_battery,
	.fg_battery_update = power_update_battery,
};

int power_fg_init(unsigned char bus)
{
	static const char name[] = "MAX17042_FG";
	struct pmic *p = pmic_alloc();

	if (!p) {
		printf("%s: POWER allocation error!\n", __func__);
		return -ENOMEM;
	}

	debug("Board Fuel Gauge init\n");

	p->name = name;
	p->interface = PMIC_I2C;
	p->number_of_regs = FG_NUM_OF_REGS;
	p->hw.i2c.addr = MAX17042_I2C_ADDR;
	p->hw.i2c.tx_num = 2;
	p->sensor_byte_order = PMIC_SENSOR_BYTE_ORDER_BIG;
	p->bus = bus;

	p->fg = &power_fg_ops;
	return 0;
}
