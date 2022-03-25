/*
 * Copyright 2010-2018 MediaTek, Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * Version 2 as published by the Free Software Foundation.
 */

#ifndef	_ASM_ARCH_SPL_H_
#define	_ASM_SPL_H_

/*
 * boot device type
 */
#define BOOT_DEVICE_NONE	0
#define BOOT_DEVICE_XIP		1
#define BOOT_DEVICE_XIPWAIT	2
#define BOOT_DEVICE_NAND	3
#define BOOT_DEVICE_ONENAND	4
#define BOOT_DEVICE_MMC1	5
#define BOOT_DEVICE_MMC2	6
#define BOOT_DEVICE_MMC2_2	7
#define BOOT_DEVICE_NOR		8
#define BOOT_DEVICE_I2C		9
#define BOOT_DEVICE_SPI		10
#define BOOT_DEVICE_RAM		11

/*
 * SPL jump to 64KB OnChip-SRAM for transfer station
 */
#define JUMP_CODE_REGION    (CONFIG_SPL_STACK + 0x1000)
#define JUMP_CODE_SIZE      (0x100)
#define JUMP_DADA_REGION    (JUMP_CODE_REGION + JUMP_CODE_SIZE)

#endif
