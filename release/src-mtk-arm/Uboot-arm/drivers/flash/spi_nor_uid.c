// SPDX-License-Identifier:	GPL-2.0+
/*
 * GPIO-simulated SPI driver for reading Unique ID from Serial NOR flashes
 *
 * Copyright (C) 2018  MediaTek, Inc.
 * Author: Weijie Gao <weijie.gao@mediatek.com>
 *
 */

#include <common.h>
#include <asm/io.h>
#include <linux/bitops.h>

#define GPIO_BASE		0x10217000

#define GPIO_DIR_REG(x)		(0x10 * (x))
#define GPIO_DOUT_REG(x)	(0x100 + 0x10 * (x))
#define GPIO_DIN_REG(x)		(0x200 + 0x10 * (x))
#define GPIO_MODE_REG(x)	(0x300 + 0x10 * (x))

/* Leopard SPI pins to GPIO */
#define SPI_CLK			62
#define SPI_CS			63
#define SPI_MOSI		64
#define SPI_MISO		65
#define SPI_WP			66
#define SPI_HOLD		67

struct spi_gpio_priv
{
	void __iomem *reg_base;
	u32 gpio_dir_val1;
	u32 gpio_dir_val2;
	u32 gpio_mode_val7;
	u32 gpio_mode_val8;
};

static inline void gpio_out(struct spi_gpio_priv *sgp, u32 n, int high)
{
	if (high)
		setbits_le32(sgp->reg_base + GPIO_DOUT_REG(n / 32), 1 << (n % 32));
	else
		clrbits_le32(sgp->reg_base + GPIO_DOUT_REG(n / 32), 1 << (n % 32));
}

static inline u32 gpio_in(struct spi_gpio_priv *sgp, u32 n)
{
	return readl(sgp->reg_base + GPIO_DIN_REG(n / 32)) & (1 << (n % 32));
}

static void leopard_spi_gpio_init(struct spi_gpio_priv *sgp)
{
	/* Backup original GPIO direction and pin mux */
	sgp->gpio_dir_val1 = readl(sgp->reg_base + GPIO_DIR_REG(1));
	sgp->gpio_dir_val2 = readl(sgp->reg_base + GPIO_DIR_REG(2));
	sgp->gpio_mode_val7 = readl(sgp->reg_base + GPIO_MODE_REG(7));
	sgp->gpio_mode_val8 = readl(sgp->reg_base + GPIO_MODE_REG(8));

	/* Setup GPIO pin mux */
	clrbits_le32(sgp->reg_base + GPIO_MODE_REG(7), 0xff000000);
	clrbits_le32(sgp->reg_base + GPIO_MODE_REG(8), 0xffff);

	/* Setup GPIO pin direction */
	setbits_le32(sgp->reg_base + GPIO_DIR_REG(1), 0xc0000000);
	clrsetbits_le32(sgp->reg_base + GPIO_DIR_REG(2), 0x2, 0xd);
}

static void leopard_spi_gpio_restore(struct spi_gpio_priv *sgp)
{
	/* Restore original GPIO pin mux */
	clrsetbits_le32(sgp->reg_base + GPIO_MODE_REG(7),
		0xff000000, sgp->gpio_mode_val7 & 0xff000000);

	clrsetbits_le32(sgp->reg_base + GPIO_MODE_REG(8),
		0xffff, sgp->gpio_mode_val8 & 0xffff);

	/* Restore original GPIO pin direction */
	clrsetbits_le32(sgp->reg_base + GPIO_DIR_REG(1),
		0xc0000000, sgp->gpio_dir_val1 & 0xc0000000);

	clrsetbits_le32(sgp->reg_base + GPIO_DIR_REG(2),
		0xf, sgp->gpio_dir_val2 & 0xf);
}

static void spi_gpio_chip_select(struct spi_gpio_priv *sgp, int active)
{
	gpio_out(sgp, SPI_CS, 1);

	if (active)
	{
		/* /WP and /HOLD to high level */
		gpio_out(sgp, SPI_WP, 1);
		gpio_out(sgp, SPI_HOLD, 1);

		/* Mode 0 */
		gpio_out(sgp, SPI_CLK, 0);
		gpio_out(sgp, SPI_CS, 0);
	}
}

static void spi_gpio_tx_byte(struct spi_gpio_priv *sgp, u8 data)
{
	u32 i;

	for (i = 0; i < 8; i++)
	{
		if (data & 0x80)
			gpio_out(sgp, SPI_MOSI, 1);
		else
			gpio_out(sgp, SPI_MOSI, 0);
		gpio_out(sgp, SPI_CLK, 1);
		gpio_out(sgp, SPI_CLK, 0);

		data <<= 1;
	}
}

static void __attribute__ ((unused)) spi_gpio_tx(struct spi_gpio_priv *sgp, const u8 *data, u32 size)
{
	u32 i;

	for (i = 0; i < size; i++)
		spi_gpio_tx_byte(sgp, data[i]);
}

static u8 spi_gpio_rx_byte(struct spi_gpio_priv *sgp)
{
	u32 i;
	u8 data = 0;

	for (i = 0; i < 8; i++)
	{
		data <<= 1;

		gpio_out(sgp, SPI_CLK, 1);
		if (gpio_in(sgp, SPI_MISO))
			data |= 1;
		gpio_out(sgp, SPI_CLK, 0);
	}

	return data;
}

static void spi_gpio_rx(struct spi_gpio_priv *sgp, u8 *data, u32 size)
{
	u32 i;

	for (i = 0; i < size; i++)
		data[i] = spi_gpio_rx_byte(sgp);
}

void spi_gpio_read_unique_id(u8 *data)
{
	struct spi_gpio_priv sgp = { .reg_base = (void __iomem *) GPIO_BASE };

	leopard_spi_gpio_init(&sgp);

	spi_gpio_chip_select(&sgp, 1);

	/* OPCODE */
	spi_gpio_tx_byte(&sgp, 0x4b);

	/* Dummy bytes */
	spi_gpio_tx_byte(&sgp, 0);
	spi_gpio_tx_byte(&sgp, 0);
	spi_gpio_tx_byte(&sgp, 0);
	spi_gpio_tx_byte(&sgp, 0);

	/* Read Unique ID */
	spi_gpio_rx(&sgp, data, 8);

	spi_gpio_chip_select(&sgp, 0);

	leopard_spi_gpio_restore(&sgp);
}

void spi_gpio_read_jedec_id(u8 *data)
{
	struct spi_gpio_priv sgp = { .reg_base = (void __iomem *) GPIO_BASE };

	leopard_spi_gpio_init(&sgp);

	spi_gpio_chip_select(&sgp, 1);

	/* OPCODE */
	spi_gpio_tx_byte(&sgp, 0x9f);

	/* Read JEDEC ID */
	spi_gpio_rx(&sgp, data, 3);

	spi_gpio_chip_select(&sgp, 0);

	leopard_spi_gpio_restore(&sgp);
}

static int cmd_uid(cmd_tbl_t *cmd, int flag, int argc, char * const argv[])
{
	u8 id[3], uid[8];

	spi_gpio_read_jedec_id(id);

	printf("JEDEC ID:  %02x%02x%02x\n", id[0], id[1], id[2]);

	if (id[0] != 0xef)
	{
		printf("Not a Winbond Serial NOR flash\n");
		return 0;
	}

	spi_gpio_read_unique_id(uid);

	printf("Unique ID: %02x%02x%02x%02x%02x%02x%02x%02x\n",
		uid[0], uid[1], uid[2], uid[3], uid[4], uid[5], uid[6], uid[7]);

	return 0;
}

U_BOOT_CMD(
	uid, 1, 1, cmd_uid,
	"uid    - Read Unique ID from spi-nor flash\n",
	NULL
);
