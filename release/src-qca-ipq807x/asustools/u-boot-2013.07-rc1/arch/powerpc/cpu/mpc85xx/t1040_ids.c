/*
 * Copyright 2012 Freescale Semiconductor, Inc.
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <asm/fsl_portals.h>
#include <asm/fsl_liodn.h>

#ifdef CONFIG_SYS_DPAA_QBMAN
struct qportal_info qp_info[CONFIG_SYS_QMAN_NUM_PORTALS] = {
	/* dqrr liodn, frame data liodn, liodn off, sdest */
	SET_QP_INFO(1, 27, 1, 0),
	SET_QP_INFO(2, 28, 1, 0),
	SET_QP_INFO(3, 29, 1, 1),
	SET_QP_INFO(4, 30, 1, 1),
	SET_QP_INFO(5, 31, 1, 2),
	SET_QP_INFO(6, 32, 1, 2),
	SET_QP_INFO(7, 33, 1, 3),
	SET_QP_INFO(8, 34, 1, 3),
	SET_QP_INFO(9, 35, 1, 0),
	SET_QP_INFO(10, 36, 1, 0),
	SET_QP_INFO(11, 37, 1, 1),
	SET_QP_INFO(12, 38, 1, 1),
	SET_QP_INFO(13, 39, 1, 2),
	SET_QP_INFO(14, 40, 1, 2),
	SET_QP_INFO(15, 41, 1, 3),
	SET_QP_INFO(16, 42, 1, 3),
	SET_QP_INFO(17, 43, 1, 0),
	SET_QP_INFO(18, 44, 1, 0),
	SET_QP_INFO(19, 45, 1, 1),
	SET_QP_INFO(20, 46, 1, 1),
	SET_QP_INFO(21, 47, 1, 2),
	SET_QP_INFO(22, 48, 1, 2),
	SET_QP_INFO(23, 49, 1, 3),
	SET_QP_INFO(24, 50, 1, 3),
	SET_QP_INFO(25, 51, 1, 0),
};
#endif

struct srio_liodn_id_table srio_liodn_tbl[] = {
	SET_SRIO_LIODN_1(1, 307),
	SET_SRIO_LIODN_1(2, 387),
};
int srio_liodn_tbl_sz = ARRAY_SIZE(srio_liodn_tbl);

struct liodn_id_table liodn_tbl[] = {
#ifdef CONFIG_SYS_DPAA_QBMAN
	SET_QMAN_LIODN(62),
	SET_BMAN_LIODN(63),
#endif

	SET_SDHC_LIODN(1, 552),

	SET_USB_LIODN(1, "fsl-usb2-mph", 553),

	SET_PCI_LIODN("fsl,qoriq-pcie-v2.2", 1, 148),

	SET_DMA_LIODN(1, 147),
	SET_DMA_LIODN(2, 227),

	SET_GUTS_LIODN("fsl,rapidio-delta", 199, rio1liodnr, 0),
	SET_GUTS_LIODN(NULL, 200, rio2liodnr, 0),
	SET_GUTS_LIODN(NULL, 201, rio1maintliodnr, 0),
	SET_GUTS_LIODN(NULL, 202, rio2maintliodnr, 0),

	/* SET_NEXUS_LIODN(557), -- not yet implemented */
};
int liodn_tbl_sz = ARRAY_SIZE(liodn_tbl);

#ifdef CONFIG_SYS_DPAA_FMAN
struct liodn_id_table fman1_liodn_tbl[] = {
	SET_FMAN_RX_1G_LIODN(1, 0, 88),
	SET_FMAN_RX_1G_LIODN(1, 1, 89),
	SET_FMAN_RX_1G_LIODN(1, 2, 90),
	SET_FMAN_RX_1G_LIODN(1, 3, 91),
	SET_FMAN_RX_1G_LIODN(1, 4, 92),
	SET_FMAN_RX_1G_LIODN(1, 5, 93),
	SET_FMAN_RX_10G_LIODN(1, 0, 94),
	SET_FMAN_RX_10G_LIODN(1, 1, 95),
};
int fman1_liodn_tbl_sz = ARRAY_SIZE(fman1_liodn_tbl);
#endif

struct liodn_id_table sec_liodn_tbl[] = {
	SET_SEC_JR_LIODN_ENTRY(0, 454, 458),
	SET_SEC_JR_LIODN_ENTRY(1, 455, 459),
	SET_SEC_JR_LIODN_ENTRY(2, 456, 460),
	SET_SEC_JR_LIODN_ENTRY(3, 457, 461),
	SET_SEC_RTIC_LIODN_ENTRY(a, 453),
	SET_SEC_RTIC_LIODN_ENTRY(b, 549),
	SET_SEC_RTIC_LIODN_ENTRY(c, 550),
	SET_SEC_RTIC_LIODN_ENTRY(d, 551),
	SET_SEC_DECO_LIODN_ENTRY(0, 541, 610),
	SET_SEC_DECO_LIODN_ENTRY(1, 542, 611),
};
int sec_liodn_tbl_sz = ARRAY_SIZE(sec_liodn_tbl);

#ifdef CONFIG_SYS_DPAA_RMAN
struct liodn_id_table rman_liodn_tbl[] = {
	/* Set RMan block 0-3 liodn offset */
	SET_RMAN_LIODN(0, 678),
	SET_RMAN_LIODN(1, 679),
	SET_RMAN_LIODN(2, 680),
	SET_RMAN_LIODN(3, 681),
};
int rman_liodn_tbl_sz = ARRAY_SIZE(rman_liodn_tbl);
#endif

struct liodn_id_table liodn_bases[] = {
	[FSL_HW_PORTAL_SEC]  = SET_LIODN_BASE_2(462, 558),
#ifdef CONFIG_SYS_DPAA_FMAN
	[FSL_HW_PORTAL_FMAN1] = SET_LIODN_BASE_1(973),
#endif
#ifdef CONFIG_SYS_DPAA_RMAN
	[FSL_HW_PORTAL_RMAN] = SET_LIODN_BASE_1(922),
#endif
};
