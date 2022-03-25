/*
 * Copyright (c) 2012  MediaTek, Inc.
 * Author : Guochun.Mao@mediatek.com
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __MTK_NOR_H__
#define __MTK_NOR_H__

#include "core/snfc.h"

/**
 * 7623/7622/7626 have same info about reg & mem base.
 * 7622 & 7626 use default pinmux setting.
 */
#define SNFC_REG_BASE	0x11014000
#define SNFC_MEM_BASE	0x30000000
#define SNFC_BUF_SIZE	256

#if defined(MT7623)
#define NOR_NEED_DO_PINMUX_SETTING
#define NOR_CS_REG	0x10005A60
#define NOR_CS_MODE	0x1
#define NOR_CS_SHIFT	0x3
#define NOR_CS_MASK	0x7

#define NOR_CLK_REG	0x10005A60
#define NOR_CLK_MODE	0x1
#define NOR_CLK_SHIFT	0x0
#define NOR_CLK_MASK	0x7

#define NOR_SIO0_REG	0x10005A50
#define NOR_SIO0_MODE	0x1
#define NOR_SIO0_SHIFT	0xC
#define NOR_SIO0_MASK	0x7

#define NOR_SIO1_REG	0x10005A50
#define NOR_SIO1_MODE	0x1
#define NOR_SIO1_SHIFT	0x9
#define NOR_SIO1_MASK	0x7

#define NOR_SIO2_REG	0x10005A50
#define NOR_SIO2_MODE	0x1
#define NOR_SIO2_SHIFT	0x6
#define NOR_SIO2_MASK	0x7

#define NOR_SIO3_REG	0x10005A50
#define NOR_SIO3_MODE	0x1
#define NOR_SIO3_SHIFT	0x3
#define NOR_SIO3_MASK	0x7
#endif

int mtk_nor_init(void);
int mtk_nor_read(u32 from, u32 len, size_t *retlen, u_char *buf);
int mtk_nor_write(u32 to, u32 len, size_t *retlen, const u_char *buf);
int mtk_nor_erase(u32 addr, u32 len);

#endif

