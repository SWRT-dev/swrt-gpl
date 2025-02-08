/*
 * Driver for the Zynq-7000 PS I2C controller
 * IP from Cadence (ID T-CS-PE-0007-100, Version R1p10f2)
 *
 * Author: Joe Hershberger <joe.hershberger@ni.com>
 * Copyright (c) 2012 Joe Hershberger.
 *
 * Copyright (c) 2012-2013 Xilinx, Michal Simek
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#include <common.h>
#include <asm/io.h>
#include <i2c.h>
#include <asm/errno.h>
#include <asm/arch/hardware.h>

/* i2c register set */
struct zynq_i2c_registers {
	u32 control;
	u32 status;
	u32 address;
	u32 data;
	u32 interrupt_status;
	u32 transfer_size;
	u32 slave_mon_pause;
	u32 time_out;
	u32 interrupt_mask;
	u32 interrupt_enable;
	u32 interrupt_disable;
};

/* Control register fields */
#define ZYNQ_I2C_CONTROL_RW		0x00000001
#define ZYNQ_I2C_CONTROL_MS		0x00000002
#define ZYNQ_I2C_CONTROL_NEA		0x00000004
#define ZYNQ_I2C_CONTROL_ACKEN		0x00000008
#define ZYNQ_I2C_CONTROL_HOLD		0x00000010
#define ZYNQ_I2C_CONTROL_SLVMON		0x00000020
#define ZYNQ_I2C_CONTROL_CLR_FIFO	0x00000040
#define ZYNQ_I2C_CONTROL_DIV_B_SHIFT	8
#define ZYNQ_I2C_CONTROL_DIV_B_MASK	0x00003F00
#define ZYNQ_I2C_CONTROL_DIV_A_SHIFT	14
#define ZYNQ_I2C_CONTROL_DIV_A_MASK	0x0000C000

/* Status register values */
#define ZYNQ_I2C_STATUS_RXDV	0x00000020
#define ZYNQ_I2C_STATUS_TXDV	0x00000040
#define ZYNQ_I2C_STATUS_RXOVF	0x00000080
#define ZYNQ_I2C_STATUS_BA	0x00000100

/* Interrupt register fields */
#define ZYNQ_I2C_INTERRUPT_COMP		0x00000001
#define ZYNQ_I2C_INTERRUPT_DATA		0x00000002
#define ZYNQ_I2C_INTERRUPT_NACK		0x00000004
#define ZYNQ_I2C_INTERRUPT_TO		0x00000008
#define ZYNQ_I2C_INTERRUPT_SLVRDY	0x00000010
#define ZYNQ_I2C_INTERRUPT_RXOVF	0x00000020
#define ZYNQ_I2C_INTERRUPT_TXOVF	0x00000040
#define ZYNQ_I2C_INTERRUPT_RXUNF	0x00000080
#define ZYNQ_I2C_INTERRUPT_ARBLOST	0x00000200

#define ZYNQ_I2C_FIFO_DEPTH		16
#define ZYNQ_I2C_TRANSFERT_SIZE_MAX	255 /* Controller transfer limit */

#if defined(CONFIG_ZYNQ_I2C0)
# define ZYNQ_I2C_BASE	ZYNQ_I2C_BASEADDR0
#else
# define ZYNQ_I2C_BASE	ZYNQ_I2C_BASEADDR1
#endif

static struct zynq_i2c_registers *zynq_i2c =
	(struct zynq_i2c_registers *)ZYNQ_I2C_BASE;

/* I2C init called by cmd_i2c when doing 'i2c reset'. */
void i2c_init(int requested_speed, int slaveadd)
{
	/* 111MHz / ( (3 * 17) * 22 ) = ~100KHz */
	writel((16 << ZYNQ_I2C_CONTROL_DIV_B_SHIFT) |
		(2 << ZYNQ_I2C_CONTROL_DIV_A_SHIFT), &zynq_i2c->control);

	/* Enable master mode, ack, and 7-bit addressing */
	setbits_le32(&zynq_i2c->control, ZYNQ_I2C_CONTROL_MS |
		ZYNQ_I2C_CONTROL_ACKEN | ZYNQ_I2C_CONTROL_NEA);
}

#ifdef DEBUG
static void zynq_i2c_debug_status(void)
{
	int int_status;
	int status;
	int_status = readl(&zynq_i2c->interrupt_status);

	status = readl(&zynq_i2c->status);
	if (int_status || status) {
		debug("Status: ");
		if (int_status & ZYNQ_I2C_INTERRUPT_COMP)
			debug("COMP ");
		if (int_status & ZYNQ_I2C_INTERRUPT_DATA)
			debug("DATA ");
		if (int_status & ZYNQ_I2C_INTERRUPT_NACK)
			debug("NACK ");
		if (int_status & ZYNQ_I2C_INTERRUPT_TO)
			debug("TO ");
		if (int_status & ZYNQ_I2C_INTERRUPT_SLVRDY)
			debug("SLVRDY ");
		if (int_status & ZYNQ_I2C_INTERRUPT_RXOVF)
			debug("RXOVF ");
		if (int_status & ZYNQ_I2C_INTERRUPT_TXOVF)
			debug("TXOVF ");
		if (int_status & ZYNQ_I2C_INTERRUPT_RXUNF)
			debug("RXUNF ");
		if (int_status & ZYNQ_I2C_INTERRUPT_ARBLOST)
			debug("ARBLOST ");
		if (status & ZYNQ_I2C_STATUS_RXDV)
			debug("RXDV ");
		if (status & ZYNQ_I2C_STATUS_TXDV)
			debug("TXDV ");
		if (status & ZYNQ_I2C_STATUS_RXOVF)
			debug("RXOVF ");
		if (status & ZYNQ_I2C_STATUS_BA)
			debug("BA ");
		debug("TS%d ", readl(&zynq_i2c->transfer_size));
		debug("\n");
	}
}
#endif

/* Wait for an interrupt */
static u32 zynq_i2c_wait(u32 mask)
{
	int timeout, int_status;

	for (timeout = 0; timeout < 100; timeout++) {
		udelay(100);
		int_status = readl(&zynq_i2c->interrupt_status);
		if (int_status & mask)
			break;
	}
#ifdef DEBUG
	zynq_i2c_debug_status();
#endif
	/* Clear interrupt status flags */
	writel(int_status & mask, &zynq_i2c->interrupt_status);

	return int_status & mask;
}

/*
 * I2C probe called by cmd_i2c when doing 'i2c probe'.
 * Begin read, nak data byte, end.
 */
int i2c_probe(u8 dev)
{
	/* Attempt to read a byte */
	setbits_le32(&zynq_i2c->control, ZYNQ_I2C_CONTROL_CLR_FIFO |
		ZYNQ_I2C_CONTROL_RW);
	clrbits_le32(&zynq_i2c->control, ZYNQ_I2C_CONTROL_HOLD);
	writel(0xFF, &zynq_i2c->interrupt_status);
	writel(dev, &zynq_i2c->address);
	writel(1, &zynq_i2c->transfer_size);

	return (zynq_i2c_wait(ZYNQ_I2C_INTERRUPT_COMP |
		ZYNQ_I2C_INTERRUPT_NACK) &
		ZYNQ_I2C_INTERRUPT_COMP) ? 0 : -ETIMEDOUT;
}

/*
 * I2C read called by cmd_i2c when doing 'i2c read' and by cmd_eeprom.c
 * Begin write, send address byte(s), begin read, receive data bytes, end.
 */
int i2c_read(u8 dev, uint addr, int alen, u8 *data, int length)
{
	u32 status;
	u32 i = 0;
	u8 *cur_data = data;

	/* Check the hardware can handle the requested bytes */
	if ((length < 0) || (length > ZYNQ_I2C_TRANSFERT_SIZE_MAX))
		return -EINVAL;

	/* Write the register address */
	setbits_le32(&zynq_i2c->control, ZYNQ_I2C_CONTROL_CLR_FIFO |
		ZYNQ_I2C_CONTROL_HOLD);
	/*
	 * Temporarily disable restart (by clearing hold)
	 * It doesn't seem to work.
	 */
	clrbits_le32(&zynq_i2c->control, ZYNQ_I2C_CONTROL_RW |
		ZYNQ_I2C_CONTROL_HOLD);
	writel(0xFF, &zynq_i2c->interrupt_status);
	while (alen--)
		writel(addr >> (8*alen), &zynq_i2c->data);
	writel(dev, &zynq_i2c->address);

	/* Wait for the address to be sent */
	if (!zynq_i2c_wait(ZYNQ_I2C_INTERRUPT_COMP)) {
		/* Release the bus */
		clrbits_le32(&zynq_i2c->control, ZYNQ_I2C_CONTROL_HOLD);
		return -ETIMEDOUT;
	}
	debug("Device acked address\n");

	setbits_le32(&zynq_i2c->control, ZYNQ_I2C_CONTROL_CLR_FIFO |
		ZYNQ_I2C_CONTROL_RW);
	/* Start reading data */
	writel(dev, &zynq_i2c->address);
	writel(length, &zynq_i2c->transfer_size);

	/* Wait for data */
	do {
		status = zynq_i2c_wait(ZYNQ_I2C_INTERRUPT_COMP |
			ZYNQ_I2C_INTERRUPT_DATA);
		if (!status) {
			/* Release the bus */
			clrbits_le32(&zynq_i2c->control, ZYNQ_I2C_CONTROL_HOLD);
			return -ETIMEDOUT;
		}
		debug("Read %d bytes\n",
		      length - readl(&zynq_i2c->transfer_size));
		for (; i < length - readl(&zynq_i2c->transfer_size); i++)
			*(cur_data++) = readl(&zynq_i2c->data);
	} while (readl(&zynq_i2c->transfer_size) != 0);
	/* All done... release the bus */
	clrbits_le32(&zynq_i2c->control, ZYNQ_I2C_CONTROL_HOLD);

#ifdef DEBUG
	zynq_i2c_debug_status();
#endif
	return 0;
}

/*
 * I2C write called by cmd_i2c when doing 'i2c write' and by cmd_eeprom.c
 * Begin write, send address byte(s), send data bytes, end.
 */
int i2c_write(u8 dev, uint addr, int alen, u8 *data, int length)
{
	u8 *cur_data = data;

	/* Write the register address */
	setbits_le32(&zynq_i2c->control, ZYNQ_I2C_CONTROL_CLR_FIFO |
		ZYNQ_I2C_CONTROL_HOLD);
	clrbits_le32(&zynq_i2c->control, ZYNQ_I2C_CONTROL_RW);
	writel(0xFF, &zynq_i2c->interrupt_status);
	while (alen--)
		writel(addr >> (8*alen), &zynq_i2c->data);
	/* Start the tranfer */
	writel(dev, &zynq_i2c->address);
	if (!zynq_i2c_wait(ZYNQ_I2C_INTERRUPT_COMP)) {
		/* Release the bus */
		clrbits_le32(&zynq_i2c->control, ZYNQ_I2C_CONTROL_HOLD);
		return -ETIMEDOUT;
	}

	debug("Device acked address\n");
	while (length--) {
		writel(*(cur_data++), &zynq_i2c->data);
		if (readl(&zynq_i2c->transfer_size) == ZYNQ_I2C_FIFO_DEPTH) {
			if (!zynq_i2c_wait(ZYNQ_I2C_INTERRUPT_COMP)) {
				/* Release the bus */
				clrbits_le32(&zynq_i2c->control,
					     ZYNQ_I2C_CONTROL_HOLD);
				return -ETIMEDOUT;
			}
		}
	}

	/* All done... release the bus */
	clrbits_le32(&zynq_i2c->control, ZYNQ_I2C_CONTROL_HOLD);
	/* Wait for the address and data to be sent */
	if (!zynq_i2c_wait(ZYNQ_I2C_INTERRUPT_COMP))
		return -ETIMEDOUT;
	return 0;
}

int i2c_set_bus_num(unsigned int bus)
{
	/* Only support bus 0 */
	if (bus > 0)
		return -1;
	return 0;
}

unsigned int i2c_get_bus_num(void)
{
	/* Only support bus 0 */
	return 0;
}
