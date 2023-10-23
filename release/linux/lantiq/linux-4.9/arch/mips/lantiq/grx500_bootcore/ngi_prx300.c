// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2019 Intel Corporation.
 * William Widjaja <w.widjaja@intel.com>
 */

#include <asm/io.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/of_platform.h>

/* ssx register defintion */
#include "ngi_prx300_ssx1.h"
#include "ngi_prx300_ssx4.h"

#define NONE		0x0
#define M4KEC_ONLY	0x1
#define CPU0		0x2
#define CPU1		0x4
#define CPU2		0x8
#define CPU3		0x10
#define OTHERS		0x20
#define	EIP123		0x40
#define IAP_N_4KEC	M4KEC_ONLY | CPU0 | CPU1 | CPU2 | CPU3
#define	GRP0_TO_4	M4KEC_ONLY | CPU0 | CPU1 | CPU2 | CPU3 | OTHERS
#define	ALL_CPUS	GRP0_TO_4 | 0x40

#define SSX_REGSIZE 0x00100000

#if CONFIG_BOOTCORE_LOAD_ADDR == 0xffffffff88000000
/* enable only if bootcore linux aligned with FMT1 start */
#define AUTO_DDR_REGION_PROTECT
#endif /* CONFIG_BOOTCORE_LOAD_ADDR == 0xffffffff88000000 */

#define VAL_NGI_DDR_REGION_SIZE_16MB  0x79
#define VAL_NGI_DDR_REGION_SIZE_32MB  0x81
#define VAL_NGI_DDR_REGION_SIZE_64MB  0x89
#define VAL_NGI_DDR_REGION_SIZE_128MB 0x91

#ifdef AUTO_DDR_REGION_PROTECT

#if defined(CONFIG_TOS_SIZE_16M)
#define VAL_NGI_DDR_REGION_SIZE VAL_NGI_DDR_REGION_SIZE_16MB
#elif defined(CONFIG_TOS_SIZE_32M)
#define VAL_NGI_DDR_REGION_SIZE VAL_NGI_DDR_REGION_SIZE_32MB
#elif defined(CONFIG_TOS_SIZE_64M)
#define VAL_NGI_DDR_REGION_SIZE VAL_NGI_DDR_REGION_SIZE_64MB
#elif defined(CONFIG_TOS_SIZE_128M)
#define VAL_NGI_DDR_REGION_SIZE VAL_NGI_DDR_REGION_SIZE_128MB
#else
#define VAL_NGI_DDR_REGION_SIZE 0
#endif

#define SUB_4KEC_REG_BASE 0x14900000u
#define SUB_4KEC_REG_SIZE 0x200
#define AHB_CPU_PAGE_1_OFFSET 0x1D4

static u32 get_base_ddr_fmt1(void)
{
	u32 base_fmt1 = 0;
	void __iomem* sub_4kec_base = 0;

	sub_4kec_base = ioremap_nocache( SUB_4KEC_REG_BASE, SUB_4KEC_REG_SIZE);
	if (NULL == sub_4kec_base) {
		pr_err("prx300-ngi: Error ioremap sub 4kec!");
		return base_fmt1;
	}

	base_fmt1 = (__raw_readl(sub_4kec_base+AHB_CPU_PAGE_1_OFFSET) & 0x0000FFFF) << 16;

	return base_fmt1;
}

#endif /* AUTO_DDR_REGION_PROTECT */

static u32 permission_wr(void __iomem* base, u32 offset, u32 val)
{
	__raw_writel(val, (volatile void __iomem *)(base+offset));
	return 0;
}

void set_permission(void)
{
	void __iomem* ssx1_base = 0;
	void __iomem* ssx4_base = 0;

	ssx1_base = ioremap_nocache( SSX1_MODULE_BASE,             SSX_REGSIZE);
	if (NULL == ssx1_base) {
		pr_err("prx300-ngi: Error ioremap sxx1!");
		return;
	}

	ssx4_base = ioremap_nocache( SSX4_SHARED_LINK_MODULE_BASE, SSX_REGSIZE);
	if (NULL == ssx4_base) {
		pr_err("prx300-ngi: Error ioremap sxx4!");
		return;
	}

	/* SSX1 Registers */
	permission_wr(ssx1_base,
			REG0_TREG0_PM_TREG0_PM_READ_PERMISSION_0, M4KEC_ONLY);
	permission_wr(ssx1_base,
			REG0_TREG0_PM_TREG0_PM_WRITE_PERMISSION_0, M4KEC_ONLY);
	permission_wr(ssx1_base,
			REG0_TOTP_PM_TOTP_PM_READ_PERMISSION_0, NONE);
	permission_wr(ssx1_base,
			REG0_TOTP_PM_TOTP_PM_WRITE_PERMISSION_0, GRP0_TO_4);
	permission_wr(ssx1_base,
			REG0_TCBM1_PM_TCBM1_PM_READ_PERMISSION_0, GRP0_TO_4);
	permission_wr(ssx1_base,
			REG0_TCBM1_PM_TCBM1_PM_WRITE_PERMISSION_0, GRP0_TO_4);
	permission_wr(ssx1_base,
			REG0_TCBM2_PM_TCBM2_PM_READ_PERMISSION_0, GRP0_TO_4);
	permission_wr(ssx1_base,
			REG0_TCBM2_PM_TCBM2_PM_WRITE_PERMISSION_0, GRP0_TO_4);
	permission_wr(ssx1_base,
			REG0_TE123_PM_TE123_PM_READ_PERMISSION_0, GRP0_TO_4);
	permission_wr(ssx1_base,
			REG0_TE123_PM_TE123_PM_WRITE_PERMISSION_0, GRP0_TO_4);
	permission_wr(ssx1_base,
			REG0_TDMA3_PM_TDM3_PM_READ_PERMISSION_0, GRP0_TO_4);
	permission_wr(ssx1_base,
			REG0_TDMA3_PM_TDM3_PM_WRITE_PERMISSION_0, GRP0_TO_4);
	permission_wr(ssx1_base,
			REG0_TDMAT1_PM_TDMAT1_PM_READ_PERMISSION_0, GRP0_TO_4);
	permission_wr(ssx1_base,
			REG0_TDMAT1_PM_TDMAT1_PM_WRITE_PERMISSION_0, GRP0_TO_4);
	permission_wr(ssx1_base,
			REG0_TEX04_PM_TEX04_PM_READ_PERMISSION_0, GRP0_TO_4);
	permission_wr(ssx1_base,
			REG0_TEX04_PM_TEX04_PM_WRITE_PERMISSION_0, GRP0_TO_4);
	permission_wr(ssx1_base,
			REG0_TDDR_PM_TDDR_PM_READ_PERMISSION_0, ALL_CPUS);
	permission_wr(ssx1_base,
			REG0_TDDR_PM_TDDR_PM_WRITE_PERMISSION_0, ALL_CPUS);
	#ifdef AUTO_DDR_REGION_PROTECT
	permission_wr(ssx1_base,
			REG0_TDDR_PM_TDDR_PM_ADDR_MATCH_1, (get_base_ddr_fmt1()|VAL_NGI_DDR_REGION_SIZE));
	#else /* AUTO_DDR_REGION_PROTECT */
	/* protect DDR region 0x3E000000~0x40000000 32MB */
	permission_wr(ssx1_base,
			REG0_TDDR_PM_TDDR_PM_ADDR_MATCH_1, 0x3E000081);
	#endif /* AUTO_DDR_REGION_PROTECT */
	permission_wr(ssx1_base,
			REG0_TDDR_PM_TDDR_PM_READ_PERMISSION_1, M4KEC_ONLY | EIP123);
	permission_wr(ssx1_base,
			REG0_TDDR_PM_TDDR_PM_WRITE_PERMISSION_1, M4KEC_ONLY | EIP123);
	permission_wr(ssx1_base,
			REG0_TDDR_PM_TDDR_PM_READ_PERMISSION_2, ALL_CPUS);
	permission_wr(ssx1_base,
			REG0_TDDR_PM_TDDR_PM_WRITE_PERMISSION_2, ALL_CPUS);
	permission_wr(ssx1_base,
			REG0_TDDR_PM_TDDR_PM_READ_PERMISSION_3, ALL_CPUS);
	permission_wr(ssx1_base,
			REG0_TDDR_PM_TDDR_PM_WRITE_PERMISSION_3, ALL_CPUS);
	permission_wr(ssx1_base,
			REG0_TDDR_PM_TDDR_PM_READ_PERMISSION_4, ALL_CPUS);
	permission_wr(ssx1_base,
			REG0_TDDR_PM_TDDR_PM_WRITE_PERMISSION_4, ALL_CPUS);
	permission_wr(ssx1_base,
			REG0_TDDR_PM_TDDR_PM_READ_PERMISSION_5, ALL_CPUS);
	permission_wr(ssx1_base,
			REG0_TDDR_PM_TDDR_PM_WRITE_PERMISSION_5, ALL_CPUS);
	permission_wr(ssx1_base,
			REG0_TDDR_PM_TDDR_PM_READ_PERMISSION_6, ALL_CPUS);
	permission_wr(ssx1_base,
			REG0_TDDR_PM_TDDR_PM_WRITE_PERMISSION_6, ALL_CPUS);
	permission_wr(ssx1_base,
			REG0_TDDR_PM_TDDR_PM_READ_PERMISSION_7, ALL_CPUS);
	permission_wr(ssx1_base,
			REG0_TDDR_PM_TDDR_PM_WRITE_PERMISSION_7, ALL_CPUS);
	permission_wr(ssx1_base,
			REG0_TPCTL_PM_TPCTL_PM_READ_PERMISSION_0, GRP0_TO_4);
	permission_wr(ssx1_base,
			REG0_TPCTL_PM_TPCTL_PM_WRITE_PERMISSION_0, GRP0_TO_4);
	permission_wr(ssx1_base,
			REG0_TPUB_PM_TPUB_PM_READ_PERMISSION_0, GRP0_TO_4);
	permission_wr(ssx1_base,
			REG0_TPUB_PM_TPUB_PM_WRITE_PERMISSION_0, GRP0_TO_4);
	permission_wr(ssx1_base,
			REG0_TPON_PM_TPON_PM_READ_PERMISSION_0, GRP0_TO_4);
	permission_wr(ssx1_base,
			REG0_TPON_PM_TPON_PM_WRITE_PERMISSION_0, GRP0_TO_4);
	permission_wr(ssx1_base,
			REG0_TGSWIP_PM_TGSWIP_PM_READ_PERMISSION_0, GRP0_TO_4);
	permission_wr(ssx1_base,
			REG0_TGSWIP_PM_TGSWIP_PM_WRITE_PERMISSION_0, GRP0_TO_4);
	permission_wr(ssx1_base,
			REG0_TQSPIC_PM_TQSPIC_PM_READ_PERMISSION_0, GRP0_TO_4);
	permission_wr(ssx1_base,
			REG0_TQSPIC_PM_TQSPIC_PM_WRITE_PERMISSION_0, GRP0_TO_4);
	permission_wr(ssx1_base,
			REG0_TMSI_PM_TMSI1_PM_READ_PERMISSION_0, GRP0_TO_4);
	permission_wr(ssx1_base,
			REG0_TMSI_PM_TMSI1_PM_WRITE_PERMISSION_0, GRP0_TO_4);
	permission_wr(ssx1_base,
			REG0_TDMAT2_PM_TDMAT2_PM_READ_PERMISSION_0, GRP0_TO_4);
	permission_wr(ssx1_base,
			REG0_TDMAT2_PM_TDMAT2_PM_WRITE_PERMISSION_0, GRP0_TO_4);
	permission_wr(ssx1_base,
			REG0_TQSPID_PM_TQSPID_PM_READ_PERMISSION_0, GRP0_TO_4);
	permission_wr(ssx1_base,
			REG0_TQSPID_PM_TQSPID_PM_WRITE_PERMISSION_0, GRP0_TO_4);
	permission_wr(ssx1_base,
			REG0_TDMAR1_PM_TDMAR1_PM_READ_PERMISSION_0, GRP0_TO_4);
	permission_wr(ssx1_base,
			REG0_TDMAR1_PM_TDMAR1_PM_WRITE_PERMISSION_0, GRP0_TO_4);
	permission_wr(ssx1_base,
			REG0_TDMAR2_PM_TDMAR2_PM_READ_PERMISSION_0, GRP0_TO_4);
	permission_wr(ssx1_base,
			REG0_TDMAR2_PM_TDMAR2_PM_WRITE_PERMISSION_0, GRP0_TO_4);
	permission_wr(ssx1_base,
			REG0_TACA_PM_TACA_PM_READ_PERMISSION_0, GRP0_TO_4);
	permission_wr(ssx1_base,
			REG0_TACA_PM_TACA_PM_WRITE_PERMISSION_0, GRP0_TO_4);
	permission_wr(ssx1_base,
			REG0_TPCIE_CR1_PM_TPCIE_CR1_PM_READ_PERMISSION_0, GRP0_TO_4);
	permission_wr(ssx1_base,
			REG0_TPCIE_CR1_PM_TPCIE_CR1_PM_WRITE_PERMISSION_0, GRP0_TO_4);
	permission_wr(ssx1_base,
			REG0_TPCI2_PM_TPCI2_PM_READ_PERMISSION_0, GRP0_TO_4);
	permission_wr(ssx1_base,
			REG0_TPCI2_PM_TPCI2_PM_WRITE_PERMISSION_0, GRP0_TO_4);
	permission_wr(ssx1_base,
			REG0_TPCI1_PM_TPCI1_PM_READ_PERMISSION_0, GRP0_TO_4);
	permission_wr(ssx1_base,
			REG0_TPCI1_PM_TPCI1_PM_WRITE_PERMISSION_0, GRP0_TO_4);
	permission_wr(ssx1_base,
			REG0_TMSI2_PM_TMSI2_PM_READ_PERMISSION_0, GRP0_TO_4);
	permission_wr(ssx1_base,
			REG0_TMSI2_PM_TMSI2_PM_WRITE_PERMISSION_0, GRP0_TO_4);
	permission_wr(ssx1_base,
			REG0_TPCIE_CR2_PM_TPCIE_CR2_PM_READ_PERMISSION_0,  GRP0_TO_4);
	permission_wr(ssx1_base,
			REG0_TPCIE_CR2_PM_TPCIE_CR2_PM_WRITE_PERMISSION_0, GRP0_TO_4);
	permission_wr(ssx1_base,
			REG0_TLNPPV4_PM_TLNPPV4_PM_READ_PERMISSION_0, GRP0_TO_4);
	permission_wr(ssx1_base,
			REG0_TLNPPV4_PM_TLNPPV4_PM_WRITE_PERMISSION_0, GRP0_TO_4);
	permission_wr(ssx1_base,
			REG0_TXPCS_PM_TXPCS_PM_READ_PERMISSION_0, GRP0_TO_4);
	permission_wr(ssx1_base,
			REG0_TXPCS_PM_TXPCS_PM_WRITE_PERMISSION_0, GRP0_TO_4);
	permission_wr(ssx1_base,
			REG0_TPC1DBI_PM_TPCI1DBI_PM_READ_PERMISSION_0, GRP0_TO_4);
	permission_wr(ssx1_base,
			REG0_TPC1DBI_PM_TPCI1DBI_PM_WRITE_PERMISSION_0, GRP0_TO_4);
	permission_wr(ssx1_base,
			REG0_TPC2DBI_PM_TPCI2DBI_PM_READ_PERMISSION_0,  GRP0_TO_4);
	permission_wr(ssx1_base,
			REG0_TPC2DBI_PM_TPCI2DBI_PM_WRITE_PERMISSION_0, GRP0_TO_4);
	permission_wr(ssx1_base,
			REG0_TSSB_PM_TSSB_PM_READ_PERMISSION_0, GRP0_TO_4);
	permission_wr(ssx1_base,
			REG0_TSSB_PM_TSSB_PM_WRITE_PERMISSION_0, GRP0_TO_4);
	permission_wr(ssx1_base,
			REG0_TROM_PM_TROM_PM_READ_PERMISSION_0, GRP0_TO_4);
	permission_wr(ssx1_base,
			REG0_TROM_PM_TROM_PM_WRITE_PERMISSION_0,  GRP0_TO_4);
	permission_wr(ssx1_base,
			REG0_TMACS_PM_TMAC_PM_READ_PERMISSION_0, GRP0_TO_4);
	permission_wr(ssx1_base,
			REG0_TMACS_PM_TMAC_PM_WRITE_PERMISSION_0, GRP0_TO_4);
	permission_wr(ssx1_base,
			REG0_TIOCU1_PM_TIOCU1_PM_READ_PERMISSION_0, GRP0_TO_4);
	permission_wr(ssx1_base,
			REG0_TIOCU1_PM_TIOCU1_PM_WRITE_PERMISSION_0, GRP0_TO_4);
	permission_wr(ssx1_base,
			REG0_TDM4_PM_TDM4_PM_READ_PERMISSION_0, GRP0_TO_4);
	permission_wr(ssx1_base,
			REG0_TDM4_PM_TDM4_PM_WRITE_PERMISSION_0, GRP0_TO_4);


	/* SSX 4 registers */
	permission_wr(ssx4_base,
			REG4_TREG4_PM_TREG4_PM_WRITE_PERMISSION_0, M4KEC_ONLY);
	permission_wr(ssx4_base,
			REG4_TREG4_PM_TREG4_PM_READ_PERMISSION_0, M4KEC_ONLY);
	permission_wr(ssx4_base,
			REG4_TCGU_PM_TCGU_PM_WRITE_PERMISSION_0, IAP_N_4KEC);
	permission_wr(ssx4_base,
			REG4_TCGU_PM_TCGU_PM_READ_PERMISSION_0, IAP_N_4KEC);
	permission_wr(ssx4_base,
			REG4_TRCU_PM_TRCU_PM_WRITE_PERMISSION_0, IAP_N_4KEC);
	permission_wr(ssx4_base,
			REG4_TRCU_PM_TRCU_PM_READ_PERMISSION_0, IAP_N_4KEC);
	permission_wr(ssx4_base,
			REG4_TPMU_PM_TPMU_PM_WRITE_PERMISSION_0, IAP_N_4KEC);
	permission_wr(ssx4_base,
			REG4_TPMU_PM_TPMU_PM_READ_PERMISSION_0, IAP_N_4KEC);
	permission_wr(ssx4_base,
			REG4_TPCM_PM_TPCM_PM_WRITE_PERMISSION_0, IAP_N_4KEC);
	permission_wr(ssx4_base,
			REG4_TPCM_PM_TPCM_PM_READ_PERMISSION_0, IAP_N_4KEC);
	permission_wr(ssx4_base,
			REG4_TGPIO_PM_TGPIO_PM_WRITE_PERMISSION_0, IAP_N_4KEC);
	permission_wr(ssx4_base,
			REG4_TGPIO_PM_TGPIO_PM_READ_PERMISSION_0, IAP_N_4KEC);
	permission_wr(ssx4_base,
			REG4_TLEDC_PM_TLEDC_PM_WRITE_PERMISSION_0, IAP_N_4KEC);
	permission_wr(ssx4_base,
			REG4_TLEDC_PM_TLEDC_PM_READ_PERMISSION_0, IAP_N_4KEC);
	permission_wr(ssx4_base,
			REG4_TI2C_PM_TI2C0_PM_WRITE_PERMISSION_0, IAP_N_4KEC);
	permission_wr(ssx4_base,
			REG4_TI2C_PM_TI2C0_PM_READ_PERMISSION_0, IAP_N_4KEC);
	permission_wr(ssx4_base,
			REG4_TASC0_PM_TASC0_PM_WRITE_PERMISSION_0, IAP_N_4KEC);
	permission_wr(ssx4_base,
			REG4_TASC0_PM_TASC0_PM_READ_PERMISSION_0,  IAP_N_4KEC);
	permission_wr(ssx4_base,
			REG4_TASC1_PM_TASC1_PM_WRITE_PERMISSION_0,  IAP_N_4KEC);
	permission_wr(ssx4_base,
			REG4_TASC1_PM_TASC1_PM_READ_PERMISSION_0, IAP_N_4KEC);
	permission_wr(ssx4_base,
			REG4_TGPT0_PM_TGPT0_PM_WRITE_PERMISSION_0, IAP_N_4KEC);
	permission_wr(ssx4_base,
			REG4_TGPT0_PM_TGPT0_PM_READ_PERMISSION_0, IAP_N_4KEC);
	permission_wr(ssx4_base,
			REG4_TSSC0_PM_TSSC0_PM_WRITE_PERMISSION_0,  IAP_N_4KEC);
	permission_wr(ssx4_base,
			REG4_TSSC0_PM_TSSC0_PM_READ_PERMISSION_0, IAP_N_4KEC);
	permission_wr(ssx4_base,
			REG4_TSSC1_PM_TSSC1_PM_WRITE_PERMISSION_0, IAP_N_4KEC);
	permission_wr(ssx4_base,
			REG4_TSSC1_PM_TSSC1_PM_READ_PERMISSION_0, IAP_N_4KEC);
	permission_wr(ssx4_base,
			REG4_TDMA0_PM_TDM0_PM_WRITE_PERMISSION_0, IAP_N_4KEC);
	permission_wr(ssx4_base,
			REG4_TDMA0_PM_TDM0_PM_READ_PERMISSION_0, IAP_N_4KEC);
	permission_wr(ssx4_base,
			REG4_TGPT1_PM_TGPT1_PM_WRITE_PERMISSION_0, IAP_N_4KEC);
	permission_wr(ssx4_base,
			REG4_TGPT1_PM_TGPT1_PM_READ_PERMISSION_0, IAP_N_4KEC);
	permission_wr(ssx4_base,
			REG4_TGPT2_PM_TGPT2_PM_WRITE_PERMISSION_0,  IAP_N_4KEC);
	permission_wr(ssx4_base,
			REG4_TGPT2_PM_TGPT2_PM_READ_PERMISSION_0, IAP_N_4KEC);
	permission_wr(ssx4_base,
			REG4_TI2C0_PM_TI2C1_PM_WRITE_PERMISSION_0, IAP_N_4KEC);
	permission_wr(ssx4_base,
			REG4_TI2C0_PM_TI2C1_PM_READ_PERMISSION_0, IAP_N_4KEC);
	permission_wr(ssx4_base,
			REG4_TI2C1_PM_TI2C2_PM_WRITE_PERMISSION_0, IAP_N_4KEC);
	permission_wr(ssx4_base,
			REG4_TI2C1_PM_TI2C2_PM_READ_PERMISSION_0, IAP_N_4KEC);

	return;
}

static int prx300_ngi_probe(struct platform_device *pdev)
{
	set_permission();

	return 0;
}

static const struct of_device_id prx300_ngi_match[] = {
	{ .compatible = "intel,prx300-ngi" },
	{},
};
MODULE_DEVICE_TABLE(of, prx300_ngi_match);

 
static struct platform_driver prx300_ngi_driver = {
	.probe = prx300_ngi_probe,
	.driver = {
		.name = "prx300-ngi",
		.owner = THIS_MODULE,
		.of_match_table = prx300_ngi_match,
	},
};
 
int __init prx300_ngi_init(void)
{
	int ret;
 
	ret = platform_driver_register(&prx300_ngi_driver);
	if (ret)
		pr_info("prx300-ngi: Error registering platform driver!");
	return ret;
}
 
core_initcall(prx300_ngi_init);

