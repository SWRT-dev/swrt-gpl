/******************************************************************************
 *                Copyright (c) 2016, 2017 Intel Corporation
 *
 *
 * For licensing information, see the file 'LICENSE' in the root folder of
 * this software module.
 *
 ******************************************************************************/
/* =========================================================================
 * This file incorporates work covered by the following copyright and
 * permission notice:
 * The Synopsys DWC ETHER XGMAC Software Driver and documentation (hereinafter
 * "Software") is an unsupported proprietary work of Synopsys, Inc. unless
 * otherwise expressly agreed to in writing between Synopsys and you.
 *
 * The Software IS NOT an item of Licensed Software or Licensed Product under
 * any End User Software License Agreement or Agreement for Licensed Product
 * with Synopsys or any supplement thereto.  Permission is hereby granted,
 * free of charge, to any person obtaining a copy of this software annotated
 * with this license and the Software, to deal in the Software without
 * restriction, including without limitation the rights to use, copy, modify,
 * merge, publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so, subject
 * to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THIS SOFTWARE IS BEING DISTRIBUTED BY SYNOPSYS SOLELY ON AN "AS IS" BASIS
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE HEREBY DISCLAIMED. IN NO EVENT SHALL SYNOPSYS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 * ========================================================================= */

#ifndef __XGMAC_MDIO_H__
#define __XGMAC_MDIO_H__

#include <xgmac_common.h>

#define MDIO_C_45	0
#define MDIO_C_22	1

#define MDIO_SCAR		0x200
#define MDIO_SCCDR		0x204
#define MDIO_CWAR		0x208
#define MDIO_CWDR		0x20C
#define MDIO_CSPE		0x210
#define MDIO_ISR		0x214
#define MDIO_IER		0x218
#define MDIO_PSR		0x21C
#define MDIO_C22P		0x220
#define MDIO_DIUR(port)	(0x230 + ((port) * 0x10))
#define MDIO_LSR(port)	(0x234 + ((port) * 0x10))
#define MDIO_ASR(port)	(0x238 + ((port) * 0x10))

#define MDIO_SCAR_DA_POS	21
#define MDIO_SCAR_DA_WIDTH	 5
#define MDIO_SCAR_PA_POS	16
#define MDIO_SCAR_PA_WIDTH	 5
#define MDIO_SCAR_RA_POS	 0
// Clause 22 (only lower 5 bits are used)
#define MDIO_SCAR_RA_WIDTH	16

#define MDIO_SCCDR_BUSY_POS	22
#define MDIO_SCCDR_BUSY_WIDTH	1
#define MDIO_SCCDR_CR_POS		19
#define MDIO_SCCDR_CR_WIDTH		3
#define MDIO_SCCDR_SADDR_POS	18
#define MDIO_SCCDR_SADDR_WIDTH	1
#define MDIO_SCCDR_CMD_POS	16
#define MDIO_SCCDR_CMD_WIDTH	2
#define MDIO_SCCDR_SDATA_POS	0
#define MDIO_SCCDR_SDATA_WIDTH	16

#define MDIO_CWAR_CWB_POS		22
#define MDIO_CWAR_CWB_WIDTH		1
#define MDIO_CWAR_CPRT_POS		21
#define MDIO_CWAR_CPRT_WIDTH		1
#define MDIO_CWAR_CADDR_POS		16
#define MDIO_CWAR_CADDR_WIDTH		5
#define MDIO_CWAR_CREGADDR_POS		0
#define MDIO_CWAR_CREGADDR_WIDTH	16

#define MDIO_CWDR_CDATA_POS		0
#define MDIO_CWDR_CDATA_WIDTH		16

#define MDIO_CSPE_PORT3_POS		3
#define MDIO_CSPE_PORT3_WIDTH		1
#define MDIO_CSPE_PORT2_POS		2
#define MDIO_CSPE_PORT2_WIDTH		1
#define MDIO_CSPE_PORT1_POS		1
#define MDIO_CSPE_PORT1_WIDTH		1
#define MDIO_CSPE_PORT0_POS		0
#define MDIO_CSPE_PORT0_WIDTH		1

#define MDIO_ISR_CWCOMPINT_POS		13
#define MDIO_ISR_CWCOMPINT_WIDTH	1
#define MDIO_ISR_SNGLCOMPINT_POS	12
#define MDIO_ISR_SNGLCOMPINT_WIDTH	1
#define MDIO_ISR_PRT3ALINT_POS		11
#define MDIO_ISR_PRT3ALINT_WIDTH	1
#define MDIO_ISR_PRT2ALINT_POS		10
#define MDIO_ISR_PRT2ALINT_WIDTH	1
#define MDIO_ISR_PRT1ALINT_POS		9
#define MDIO_ISR_PRT1ALINT_WIDTH	1
#define MDIO_ISR_PRT0ALINT_POS		8
#define MDIO_ISR_PRT0ALINT_WIDTH	1
#define MDIO_ISR_PRT3LSINT_POS		7
#define MDIO_ISR_PRT3LSINT_WIDTH	1
#define MDIO_ISR_PRT2LSINT_POS		6
#define MDIO_ISR_PRT2LSINT_WIDTH	1
#define MDIO_ISR_PRT1LSINT_POS		5
#define MDIO_ISR_PRT1LSINT_WIDTH	1
#define MDIO_ISR_PRT0LSINT_POS		4
#define MDIO_ISR_PRT0LSINT_WIDTH	1
#define MDIO_ISR_PRT3CONINT_POS		3
#define MDIO_ISR_PRT3CONINT_WIDTH	1
#define MDIO_ISR_PRT2CONINT_POS		2
#define MDIO_ISR_PRT2CONINT_WIDTH	1
#define MDIO_ISR_PRT1CONINT_POS		1
#define MDIO_ISR_PRT1CONINT_WIDTH	1
#define MDIO_ISR_PRT0CONINT_POS		0
#define MDIO_ISR_PRT0CONINT_WIDTH	1

#define MDIO_IER_CWCOMPIE_POS		13
#define MDIO_IER_CWCOMPIE_WIDTH		1
#define MDIO_IER_SNGLCOMPIE_POS		12
#define MDIO_IER_SNGLCOMPIE_WIDTH	1
#define MDIO_IER_PRT3ALIE_POS		11
#define MDIO_IER_PRT3ALIE_WIDTH		1
#define MDIO_IER_PRT2ALIE_POS		10
#define MDIO_IER_PRT2ALIE_WIDTH		1
#define MDIO_IER_PRT1ALIE_POS		9
#define MDIO_IER_PRT1ALIE_WIDTH		1
#define MDIO_IER_PRT0ALIE_POS		8
#define MDIO_IER_PRT0ALIE_WIDTH		1
#define MDIO_IER_PRT3LSIE_POS		7
#define MDIO_IER_PRT3LSIE_WIDTH		1
#define MDIO_IER_PRT2LSIE_POS		6
#define MDIO_IER_PRT2LSIE_WIDTH		1
#define MDIO_IER_PRT1LSIE_POS		5
#define MDIO_IER_PRT1LSIE_WIDTH		1
#define MDIO_IER_PRT0LSIE_POS		4
#define MDIO_IER_PRT0LSIE_WIDTH		1
#define MDIO_IER_PRT3CONIE_POS		3
#define MDIO_IER_PRT3CONIE_WIDTH	1
#define MDIO_IER_PRT2CONIE_POS		2
#define MDIO_IER_PRT2CONIE_WIDTH	1
#define MDIO_IER_PRT1CONIE_POS		1
#define MDIO_IER_PRT1CONIE_WIDTH	1
#define MDIO_IER_PRT0CONIE_POS		0
#define MDIO_IER_PRT0CONIE_WIDTH	1

#define MDIO_PSR_PORT3CON_POS		3
#define MDIO_PSR_PORT3CON_WIDTH		1
#define MDIO_PSR_PORT2CON_POS		2
#define MDIO_PSR_PORT2CON_WIDTH		1
#define MDIO_PSR_PORT1CON_POS		1
#define MDIO_PSR_PORT1CON_WIDTH		1
#define MDIO_PSR_PORT0CON_POS		0
#define MDIO_PSR_PORT0CON_WIDTH		1

#define MDIO_CL22P_PORT3_POS		3
#define MDIO_CL22P_PORT3_WIDTH		1
#define MDIO_CL22P_PORT2_POS		2
#define MDIO_CL22P_PORT2_WIDTH		1
#define MDIO_CL22P_PORT1_POS		1
#define MDIO_CL22P_PORT1_WIDTH		1
#define MDIO_CL22P_PORT0_POS		0
#define MDIO_CL22P_PORT0_WIDTH		1

#define MDIO_DIUR_PVSD2_POS		31
#define MDIO_DIUR_PVSD2_WIDTH		1
#define MDIO_DIUR_PVSD1_POS		30
#define MDIO_DIUR_PVSD1_WIDTH		1
#define MDIO_DIUR_PTC_POS		6
#define MDIO_DIUR_PTC_WIDTH		1
#define MDIO_DIUR_PDTEXS_POS		5
#define MDIO_DIUR_PDTEXS_WIDTH		1
#define MDIO_DIUR_PPHYXS_POS		4
#define MDIO_DIUR_PPHYXS_WIDTH		1
#define MDIO_DIUR_PPCS_POS		3
#define MDIO_DIUR_PPCS_WIDTH		1
#define MDIO_DIUR_PWIS_POS		2
#define MDIO_DIUR_PWIS_WIDTH		1
#define MDIO_DIUR_PPMDPMA_POS		1
#define MDIO_DIUR_PPMDPMA_WIDTH		1

#define MDIO_LSR_PVSD2_POS		31
#define MDIO_LSR_PVSD2_WIDTH		1
#define MDIO_LSR_PVSD1_POS		30
#define MDIO_LSR_PVSD1_WIDTH		1
#define MDIO_LSR_PTC_POS		6
#define MDIO_LSR_PTC_WIDTH		1
#define MDIO_LSR_PDTEXS_POS		5
#define MDIO_LSR_PDTEXS_WIDTH		1
#define MDIO_LSR_PPHYXS_POS		4
#define MDIO_LSR_PPHYXS_WIDTH		1
#define MDIO_LSR_PPCS_POS		3
#define MDIO_LSR_PPCS_WIDTH		1
#define MDIO_LSR_PWIS_POS		2
#define MDIO_LSR_PWIS_WIDTH		1
#define MDIO_LSR_PPMDPMA_POS		1
#define MDIO_LSR_PPMDPMA_WIDTH		1

#define MDIO_ASR_PVSD2_POS		31
#define MDIO_ASR_PVSD2_WIDTH		1
#define MDIO_ASR_PVSD1_POS		30
#define MDIO_ASR_PVSD1_WIDTH		1
#define MDIO_ASR_PTC_POS		6
#define MDIO_ASR_PTC_WIDTH		1
#define MDIO_ASR_PDTEXS_POS		5
#define MDIO_ASR_PDTEXS_WIDTH		1
#define MDIO_ASR_PPHYXS_POS		4
#define MDIO_ASR_PPHYXS_WIDTH		1
#define MDIO_ASR_PPCS_POS		3
#define MDIO_ASR_PPCS_WIDTH		1
#define MDIO_ASR_PWIS_POS		2
#define MDIO_ASR_PWIS_WIDTH		1
#define MDIO_ASR_PPMDPMA_POS		1
#define MDIO_ASR_PPMDPMA_WIDTH		1

int xgmac_mdio_single_wr(void *pdev,
			 u32 dev_adr,
			 u32 phy_id,
			 u32 phy_reg,
			 u32 phy_reg_data);
int xgmac_mdio_single_rd(void *pdev, u32 dev_adr, u32 phy_id, u32 phy_reg);
int mdio_set_clause(void *pdev, u32 clause, u32 phy_id);
int mdio_get_clause(void *pdev,  u32 phy_id);
int mdio_set_interrupt(void *pdev, u32 val);
int xgmac_mdio_get_int_sts(void *pdev);
void print_mdio_rd_cnt(void *pdev,
		       u32 dev_adr,
		       u32 phy_id,
		       u32 phy_reg_st,
		       u32 phy_reg_end);
void xgmac_print_mdio(void *pdev,
		      u32 op,
		      u32 dev_adr,
		      u32 phy_id,
		      u32 phy_reg,
		      u32 phy_reg_data);

#ifdef __KERNEL__
int xgmac_mdio_register(void *pdev);
void xgmac_mdio_unregister(void *pdev);
#endif

#endif
