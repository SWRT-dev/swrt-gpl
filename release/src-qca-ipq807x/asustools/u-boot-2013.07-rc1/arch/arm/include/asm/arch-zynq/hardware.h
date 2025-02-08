/*
 * Copyright (c) 2013 Xilinx Inc.
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

#ifndef _ASM_ARCH_HARDWARE_H
#define _ASM_ARCH_HARDWARE_H

#define ZYNQ_SYS_CTRL_BASEADDR		0xF8000000
#define ZYNQ_DEV_CFG_APB_BASEADDR	0xF8007000
#define ZYNQ_SCU_BASEADDR		0xF8F00000
#define ZYNQ_SCUTIMER_BASEADDR		0xF8F00600
#define ZYNQ_GEM_BASEADDR0		0xE000B000
#define ZYNQ_GEM_BASEADDR1		0xE000C000
#define ZYNQ_SDHCI_BASEADDR0		0xE0100000
#define ZYNQ_SDHCI_BASEADDR1		0xE0101000
#define ZYNQ_I2C_BASEADDR0		0xE0004000
#define ZYNQ_I2C_BASEADDR1		0xE0005000

/* Reflect slcr offsets */
struct slcr_regs {
	u32 scl; /* 0x0 */
	u32 slcr_lock; /* 0x4 */
	u32 slcr_unlock; /* 0x8 */
	u32 reserved0[75];
	u32 gem0_rclk_ctrl; /* 0x138 */
	u32 gem1_rclk_ctrl; /* 0x13c */
	u32 gem0_clk_ctrl; /* 0x140 */
	u32 gem1_clk_ctrl; /* 0x144 */
	u32 reserved1[46];
	u32 pss_rst_ctrl; /* 0x200 */
	u32 reserved2[15];
	u32 fpga_rst_ctrl; /* 0x240 */
	u32 reserved3[5];
	u32 reboot_status; /* 0x258 */
	u32 boot_mode; /* 0x25c */
	u32 reserved4[116];
	u32 trust_zone; /* 0x430 */ /* FIXME */
	u32 reserved5_1[63];
	u32 pss_idcode; /* 0x530 */
	u32 reserved5_2[51];
	u32 ddr_urgent; /* 0x600 */
	u32 reserved6[6];
	u32 ddr_urgent_sel; /* 0x61c */
	u32 reserved7[56];
	u32 mio_pin[54]; /* 0x700 - 0x7D4 */
	u32 reserved8[74];
	u32 lvl_shftr_en; /* 0x900 */
	u32 reserved9[3];
	u32 ocm_cfg; /* 0x910 */
};

#define slcr_base ((struct slcr_regs *)ZYNQ_SYS_CTRL_BASEADDR)

struct devcfg_regs {
	u32 ctrl; /* 0x0 */
	u32 lock; /* 0x4 */
	u32 cfg; /* 0x8 */
	u32 int_sts; /* 0xc */
	u32 int_mask; /* 0x10 */
	u32 status; /* 0x14 */
	u32 dma_src_addr; /* 0x18 */
	u32 dma_dst_addr; /* 0x1c */
	u32 dma_src_len; /* 0x20 */
	u32 dma_dst_len; /* 0x24 */
	u32 rom_shadow; /* 0x28 */
	u32 reserved1[2];
	u32 unlock; /* 0x34 */
	u32 reserved2[18];
	u32 mctrl; /* 0x80 */
	u32 reserved3;
	u32 write_count; /* 0x88 */
	u32 read_count; /* 0x8c */
};

#define devcfg_base ((struct devcfg_regs *)ZYNQ_DEV_CFG_APB_BASEADDR)

struct scu_regs {
	u32 reserved1[16];
	u32 filter_start; /* 0x40 */
	u32 filter_end; /* 0x44 */
};

#define scu_base ((struct scu_regs *)ZYNQ_SCU_BASEADDR)

#endif /* _ASM_ARCH_HARDWARE_H */
