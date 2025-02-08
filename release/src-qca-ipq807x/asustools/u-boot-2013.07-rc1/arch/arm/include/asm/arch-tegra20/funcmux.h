/*
 * Copyright (c) 2011 The Chromium OS Authors.
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

/* Tegra20 high-level function multiplexing */

#ifndef _TEGRA20_FUNCMUX_H_
#define _TEGRA20_FUNCMUX_H_

#include <asm/arch-tegra/funcmux.h>

/* Configs supported by the func mux */
enum {
	FUNCMUX_DEFAULT = 0,	/* default config */

	/* UART configs */
	FUNCMUX_UART1_IRRX_IRTX = 0,
	FUNCMUX_UART1_UAA_UAB,
	FUNCMUX_UART1_GPU,
	FUNCMUX_UART1_SDIO1,
	FUNCMUX_UART2_UAD = 0,
	FUNCMUX_UART4_GMC = 0,

	/* I2C configs */
	FUNCMUX_DVC_I2CP = 0,
	FUNCMUX_I2C1_RM = 0,
	FUNCMUX_I2C2_DDC = 0,
	FUNCMUX_I2C2_PTA,
	FUNCMUX_I2C3_DTF = 0,

	/* SDMMC configs */
	FUNCMUX_SDMMC1_SDIO1_4BIT = 0,
	FUNCMUX_SDMMC2_DTA_DTD_8BIT = 0,
	FUNCMUX_SDMMC3_SDB_4BIT = 0,
	FUNCMUX_SDMMC3_SDB_SLXA_8BIT,
	FUNCMUX_SDMMC4_ATC_ATD_8BIT = 0,
	FUNCMUX_SDMMC4_ATB_GMA_4_BIT,
	FUNCMUX_SDMMC4_ATB_GMA_GME_8_BIT,

	/* USB configs */
	FUNCMUX_USB2_ULPI = 0,

	/* Serial Flash configs */
	FUNCMUX_SPI1_GMC_GMD = 0,

	/* NAND flags */
	FUNCMUX_NDFLASH_ATC = 0,
	FUNCMUX_NDFLASH_KBC_8_BIT,
};
#endif	/* _TEGRA20_FUNCMUX_H_ */
