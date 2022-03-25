/*
 * Copyright 2010-2018 MediaTek, Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * Version 2 as published by the Free Software Foundation.
 */

#include <asm/arch/leopard.h>
#include <asm/arch/spm_mtcmos.h>
#include <common.h>

#include "pll2.h"
/*
 * for SPM register control
 */
#define SPM_POWERON_CONFIG_SET      (SPM_BASE + 0x0000)
#define SPM_MP0_CPU1_PWR_CON        (SPM_BASE + 0x0218)
#define SPM_MP0_CPU1_L1_PDN         (SPM_BASE + 0x0264)
#define SPM_PWR_STATUS              (SPM_BASE + 0x060c)
#define SPM_PWR_STATUS_S            (SPM_BASE + 0x0610)
#define SPM_SLEEP_TIMER_STA         (SPM_BASE + 0x0720)

#define SPM_PROJECT_CODE            0xb16

#define spm_read(addr)              DRV_Reg32(addr)
#define spm_write(addr, val)        DRV_WriteReg32(addr, val)

/*
 * for CPU MTCMOS
 */
/*
 * regiser bit definition
 */
/* SPM_MP0_CPU1_PWR_CON */
#define PD_SLPB_CLAMP   (1U << 7)
#define SRAM_ISOINT_B   (1U << 6)
#define SRAM_CKISO      (1U << 5)
#define PWR_CLK_DIS     (1U << 4)
#define PWR_ON_2ND      (1U << 3)
#define PWR_ON          (1U << 2)
#define PWR_ISO         (1U << 1)
#define PWR_RST_B       (1U << 0)

/* SPM_MP0_CPU1_L1_PDN */
#define MP0_CPU1_L1_PDN_ACK      (1U << 8)
#define MP0_CPU1_L1_PDN          (1U << 0)

/* SPM_PWR_STATUS */
/* SPM_PWR_STATUS_S */
#define SPM_PWR_STATUS_CPU1       (1U << 12)

/* SPM_SLEEP_TIMER_STA */
#define MP0_CPU1_STANDBYWFI    (1U << 17)

int spm_mtcmos_cpu1_off(void)
{
	printf("SPM_PWR_STATUS(0x%x) = 0x%x, turn off cpu1.\n",
	       SPM_PWR_STATUS, spm_read(SPM_PWR_STATUS));

	/* enable register control */
	spm_write(SPM_POWERON_CONFIG_SET, (SPM_PROJECT_CODE << 16) | (1U << 0));

	while ((spm_read(SPM_SLEEP_TIMER_STA) &
		MP0_CPU1_STANDBYWFI) == 0)
		;

	spm_write(SPM_MP0_CPU1_PWR_CON,
		  spm_read(SPM_MP0_CPU1_PWR_CON) |
		  PWR_ISO | PD_SLPB_CLAMP);
	spm_write(SPM_MP0_CPU1_PWR_CON,
		  spm_read(SPM_MP0_CPU1_PWR_CON) | SRAM_CKISO);

	spm_write(SPM_MP0_CPU1_L1_PDN,
		  spm_read(SPM_MP0_CPU1_L1_PDN) | MP0_CPU1_L1_PDN);
	spm_write(SPM_MP0_CPU1_PWR_CON,
		  (spm_read(SPM_MP0_CPU1_PWR_CON) & ~PWR_RST_B));
	spm_write(SPM_MP0_CPU1_PWR_CON,
		  (spm_read(SPM_MP0_CPU1_PWR_CON) | PWR_CLK_DIS));

	spm_write(SPM_MP0_CPU1_PWR_CON,
		  spm_read(SPM_MP0_CPU1_PWR_CON) &
		  ~PWR_ON & ~PWR_ON_2ND);
	while (((spm_read(SPM_PWR_STATUS) &
		 SPM_PWR_STATUS_CPU1) != 0) ||
	       ((spm_read(SPM_PWR_STATUS_S) &
		 SPM_PWR_STATUS_CPU1) != 0))
		;

	printf("SPM_PWR_STATUS(0x%x) = 0x%x, turn off cpu1 done.\n",
	       SPM_PWR_STATUS, spm_read(SPM_PWR_STATUS));

	return 0;
}

/**************************************
 * for non-CPU MTCMOS
 **************************************/

#define SPM_ETH_PWR_CON			(SPM_BASE + 0x2E0)
#define SPM_HIF0_PWR_CON		(SPM_BASE + 0x2E4)
#define SPM_HIF1_PWR_CON		(SPM_BASE + 0x2E8)

#define ETH_PWR_STA_MASK    (0x1 << 24)
#define HIF0_PWR_STA_MASK   (0x1 << 25)
#define HIF1_PWR_STA_MASK   (0x1 << 26)

#define ETH_SRAM_PDN	(0x1 << 8)
#define ETH_SRAM_PDN1	(0x1 << 9)
#define ETH_SRAM_PDN2	(0x1 << 10)
#define ETH_SRAM_PDN3	(0x1 << 11)
#define HIF0_SRAM_PDN   (0xf << 8)
#define HIF1_SRAM_PDN   (0xf << 8)

#define ETH_SRAM_ACK	(0xf << 12)
#define ETH_SRAM_ACK1	(0x1 << 13)
#define ETH_SRAM_ACK2	(0x1 << 14)
#define ETH_SRAM_ACK3	(0x1 << 15)

#define HIF0_SRAM_ACK   (0xf << 12)
#define HIF1_SRAM_ACK   (0xf << 12)

#define ETH_PROT_MASK   ( (0x1<<3) | (0x1<<17) )
#define HIF0_PROT_MASK  ( (0x1<<24) | (0x1<<25) )
#define HIF1_PROT_MASK  ( (0x1<<26) | (0x1<<27) | (0x1<<28) )
#define SPM_PWR_STATUS_2ND		SPM_PWR_STATUS_S


int spm_mtcmos_ctrl_eth(int state)
{
    int err = 0;
    volatile unsigned int val;
	int count = 0;

    if (state == STA_POWER_DOWN) {
        spm_write(TOPAXI_PROT_EN, spm_read(TOPAXI_PROT_EN) | ETH_PROT_MASK);
        while ((spm_read(TOPAXI_PROT_STA1) & ETH_PROT_MASK) != ETH_PROT_MASK) {
#if 1
            count++;
            if(count>1000)
                break;
#endif
		}
		count = 0;

        spm_write(SPM_ETH_PWR_CON, spm_read(SPM_ETH_PWR_CON) | ETH_SRAM_PDN1);
		spm_write(SPM_ETH_PWR_CON, spm_read(SPM_ETH_PWR_CON) | ETH_SRAM_PDN2);
		spm_write(SPM_ETH_PWR_CON, spm_read(SPM_ETH_PWR_CON) | ETH_SRAM_PDN3);
        spm_write(SPM_ETH_PWR_CON, spm_read(SPM_ETH_PWR_CON) | ETH_SRAM_PDN);

        while ((spm_read(SPM_ETH_PWR_CON) & ETH_SRAM_ACK) != ETH_SRAM_ACK) {
#if 1
            count++;
            if(count>1000)
                break;
#endif
		}
		count = 0;

        spm_write(SPM_ETH_PWR_CON, spm_read(SPM_ETH_PWR_CON) | PWR_ISO);

        val = spm_read(SPM_ETH_PWR_CON);
        val = (val & ~PWR_RST_B) | PWR_CLK_DIS;
        spm_write(SPM_ETH_PWR_CON, val);

        spm_write(SPM_ETH_PWR_CON, spm_read(SPM_ETH_PWR_CON) & ~(PWR_ON | PWR_ON_2ND));

        while ((spm_read(SPM_PWR_STATUS) & ETH_PWR_STA_MASK)
                || (spm_read(SPM_PWR_STATUS_2ND) & ETH_PWR_STA_MASK)) {
#if 1
            count++;
            if(count>1000)
                break;
#endif
        }
    } else {
        spm_write(SPM_ETH_PWR_CON, spm_read(SPM_ETH_PWR_CON) | PWR_ON);
        spm_write(SPM_ETH_PWR_CON, spm_read(SPM_ETH_PWR_CON) | PWR_ON_2ND);

        while (!(spm_read(SPM_PWR_STATUS) & ETH_PWR_STA_MASK)
                || !(spm_read(SPM_PWR_STATUS_2ND) & ETH_PWR_STA_MASK)) {
#if 1
            count++;
            if(count>1000)
                break;
#endif
        }
		count = 0;

        spm_write(SPM_ETH_PWR_CON, spm_read(SPM_ETH_PWR_CON) & ~PWR_CLK_DIS);
        spm_write(SPM_ETH_PWR_CON, spm_read(SPM_ETH_PWR_CON) & ~PWR_ISO);

		spm_write(SPM_ETH_PWR_CON, spm_read(SPM_ETH_PWR_CON) & ~ETH_SRAM_PDN1);
		spm_write(SPM_ETH_PWR_CON, spm_read(SPM_ETH_PWR_CON) & ~ETH_SRAM_PDN2);
		spm_write(SPM_ETH_PWR_CON, spm_read(SPM_ETH_PWR_CON) & ~ETH_SRAM_PDN3);

        spm_write(SPM_ETH_PWR_CON, spm_read(SPM_ETH_PWR_CON) | PWR_RST_B);

        spm_write(SPM_ETH_PWR_CON, spm_read(SPM_ETH_PWR_CON) & ~ETH_SRAM_PDN);

        while ((spm_read(SPM_ETH_PWR_CON) & ETH_SRAM_ACK)) {
#if 1
            count++;
            if(count>1000)
                break;
#endif
		}
		count = 0;

        spm_write(TOPAXI_PROT_EN, spm_read(TOPAXI_PROT_EN) & ~ETH_PROT_MASK);
        while (spm_read(TOPAXI_PROT_STA1) & ETH_PROT_MASK) {
#if 1
            count++;
            if(count>1000)
                break;
#endif
        }
    }

    return err;
}

int spm_mtcmos_ctrl_hif0(int state)
{
    int err = 0;
    volatile unsigned int val;
	int count = 0;

    if (state == STA_POWER_DOWN) {
        spm_write(TOPAXI_PROT_EN, spm_read(TOPAXI_PROT_EN) | HIF0_PROT_MASK);
        while ((spm_read(TOPAXI_PROT_STA1) & HIF0_PROT_MASK) != HIF0_PROT_MASK) {
#if 1
            count++;
            if(count>1000)
                break;
#endif
        }
		count = 0;

        spm_write(SPM_HIF0_PWR_CON, spm_read(SPM_HIF0_PWR_CON) | HIF0_SRAM_PDN);

        while ((spm_read(SPM_HIF0_PWR_CON) & HIF0_SRAM_ACK) != HIF0_SRAM_ACK) {
#if 1
            count++;
            if(count>1000)
                break;
#endif
		}
		count = 0;

        spm_write(SPM_HIF0_PWR_CON, spm_read(SPM_HIF0_PWR_CON) | PWR_ISO);

        val = spm_read(SPM_HIF0_PWR_CON);
        val = (val & ~PWR_RST_B) | PWR_CLK_DIS;
        spm_write(SPM_HIF0_PWR_CON, val);

        spm_write(SPM_HIF0_PWR_CON, spm_read(SPM_HIF0_PWR_CON) & ~(PWR_ON | PWR_ON_2ND));

        while ((spm_read(SPM_PWR_STATUS) & HIF0_PWR_STA_MASK)
                || (spm_read(SPM_PWR_STATUS_2ND) & HIF0_PWR_STA_MASK)) {
#if 1
            count++;
            if(count>1000)
                break;
#endif
        }
    } else {
        spm_write(SPM_HIF0_PWR_CON, spm_read(SPM_HIF0_PWR_CON) | PWR_ON);
        spm_write(SPM_HIF0_PWR_CON, spm_read(SPM_HIF0_PWR_CON) | PWR_ON_2ND);

        while (!(spm_read(SPM_PWR_STATUS) & HIF0_PWR_STA_MASK)
                || !(spm_read(SPM_PWR_STATUS_2ND) & HIF0_PWR_STA_MASK)) {
#if 1
            count++;
            if(count>1000)
                break;
#endif
        }
		count = 0;

        spm_write(SPM_HIF0_PWR_CON, spm_read(SPM_HIF0_PWR_CON) & ~PWR_CLK_DIS);
        spm_write(SPM_HIF0_PWR_CON, spm_read(SPM_HIF0_PWR_CON) & ~PWR_ISO);
        spm_write(SPM_HIF0_PWR_CON, spm_read(SPM_HIF0_PWR_CON) | PWR_RST_B);

        spm_write(SPM_HIF0_PWR_CON, spm_read(SPM_HIF0_PWR_CON) & ~HIF0_SRAM_PDN);

        while ((spm_read(SPM_HIF0_PWR_CON) & HIF0_SRAM_ACK)) {
#if 1
            count++;
            if(count>1000)
                break;
#endif
        }
		count = 0;

        spm_write(TOPAXI_PROT_EN, spm_read(TOPAXI_PROT_EN) & ~HIF0_PROT_MASK);
        while (spm_read(TOPAXI_PROT_STA1) & HIF0_PROT_MASK) {
#if 1
            count++;
            if(count>1000)
                break;
#endif
        }
    }

    return err;
}

int spm_mtcmos_ctrl_hif1(int state)
{
    int err = 0;
    volatile unsigned int val;
	int count = 0;

    if (state == STA_POWER_DOWN) {
        spm_write(TOPAXI_PROT_EN, spm_read(TOPAXI_PROT_EN) | HIF1_PROT_MASK);
        while ((spm_read(TOPAXI_PROT_STA1) & HIF1_PROT_MASK) != HIF1_PROT_MASK) {
#if 1
            count++;
            if(count>1000)
                break;
#endif
		}
		count = 0;

        spm_write(SPM_HIF1_PWR_CON, spm_read(SPM_HIF1_PWR_CON) | HIF1_SRAM_PDN);

        while ((spm_read(SPM_HIF1_PWR_CON) & HIF1_SRAM_ACK) != HIF1_SRAM_ACK) {
#if 1
            count++;
            if(count>1000)
                break;
#endif
		}
		count = 0;

        spm_write(SPM_HIF1_PWR_CON, spm_read(SPM_HIF1_PWR_CON) | PWR_ISO);

        val = spm_read(SPM_HIF1_PWR_CON);
        val = (val & ~PWR_RST_B) | PWR_CLK_DIS;
        spm_write(SPM_HIF1_PWR_CON, val);

        spm_write(SPM_HIF1_PWR_CON, spm_read(SPM_HIF1_PWR_CON) & ~(PWR_ON | PWR_ON_2ND));

        while ((spm_read(SPM_PWR_STATUS) & HIF1_PWR_STA_MASK)
                || (spm_read(SPM_PWR_STATUS_2ND) & HIF1_PWR_STA_MASK)) {
#if 1
            count++;
            if(count>1000)
                break;
#endif
		}
    } else {
        spm_write(SPM_HIF1_PWR_CON, spm_read(SPM_HIF1_PWR_CON) | PWR_ON);
        spm_write(SPM_HIF1_PWR_CON, spm_read(SPM_HIF1_PWR_CON) | PWR_ON_2ND);

        while (!(spm_read(SPM_PWR_STATUS) & HIF1_PWR_STA_MASK)
                || !(spm_read(SPM_PWR_STATUS_2ND) & HIF1_PWR_STA_MASK)) {
#if 1
            count++;
            if(count>1000)
                break;
#endif
		}
		count = 0;

        spm_write(SPM_HIF1_PWR_CON, spm_read(SPM_HIF1_PWR_CON) & ~PWR_CLK_DIS);
        spm_write(SPM_HIF1_PWR_CON, spm_read(SPM_HIF1_PWR_CON) & ~PWR_ISO);
        spm_write(SPM_HIF1_PWR_CON, spm_read(SPM_HIF1_PWR_CON) | PWR_RST_B);

        spm_write(SPM_HIF1_PWR_CON, spm_read(SPM_HIF1_PWR_CON) & ~HIF1_SRAM_PDN);

        while ((spm_read(SPM_HIF1_PWR_CON) & HIF1_SRAM_ACK)) {
#if 1
            count++;
            if(count>1000)
                break;
#endif
        }
		count = 0;

        spm_write(TOPAXI_PROT_EN, spm_read(TOPAXI_PROT_EN) & ~HIF1_PROT_MASK);
        while (spm_read(TOPAXI_PROT_STA1) & HIF1_PROT_MASK) {
#if 1
            count++;
            if(count>1000)
                break;
#endif
        }
    }

    return err;
}

