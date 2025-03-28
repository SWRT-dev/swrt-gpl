// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2018, The Linux Foundation. All rights reserved.
 */

/*
 * In Certain QCOM SoCs like apq8096 and msm8996 that have KRYO processors,
 * the CPU frequency subset and voltage value of each OPP varies
 * based on the silicon variant in use. Qualcomm Process Voltage Scaling Tables
 * defines the voltage and frequency value based on the msm-id in SMEM
 * and speedbin blown in the efuse combination.
 * The qcom-cpufreq-nvmem driver reads the msm-id and efuse value from the SoC
 * to provide the OPP framework with required information.
 * This is used to determine the voltage and frequency value for each OPP of
 * operating-points-v2 table when it is parsed by the OPP framework.
 */

#include <linux/cpu.h>
#include <linux/err.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/nvmem-consumer.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/platform_device.h>
#include <linux/pm_domain.h>
#include <linux/pm_opp.h>
#include <linux/slab.h>
#include <linux/soc/qcom/smem.h>

#define MSM_ID_SMEM	137

enum _msm_id {
	MSM8996V3 = 0xF6ul,
	APQ8096V3 = 0x123ul,
	MSM8996SG = 0x131ul,
	APQ8096SG = 0x138ul,
	IPQ5332V1 = 0x250ul,
	IPQ5322V1 = 0x251ul,
	IPQ5312V1 = 0x252ul,
	IPQ5302V1 = 0x253ul,
	IPQ5300V1 = 0x270ul,
	IPQ6018V1 = 0x192ul,
	IPQ6028V1 = 0x193ul,
	IPQ6000V1 = 0x1a5ul,
	IPQ6010V1 = 0x1a6ul,
	IPQ6005V1 = 0x1c5ul,
	IPQ9574V1 = 0x202ul,
	IPQ9570V1 = 0x201ul,
	IPQ9554V1 = 0x200ul,
	IPQ9550V1 = 0x1fful,
	IPQ9514V1 = 0x1feul,
	IPQ9510V1 = 0x209ul,
};

enum _msm8996_version {
	MSM8996_V3,
	MSM8996_SG,
	IPQ53XX_V1,
	IPQ60XX_V1,
	IPQ6000_V1,
	IPQ95XX_V1,
	NUM_OF_MSM8996_VERSIONS,
};

struct qcom_cpufreq_drv;

struct qcom_cpufreq_match_data {
	int (*get_version)(struct device *cpu_dev,
			   struct nvmem_cell *speedbin_nvmem,
			   struct qcom_cpufreq_drv *drv);
	const char **genpd_names;
};

struct qcom_cpufreq_drv {
	struct opp_table **opp_tables;
	struct opp_table **genpd_opp_tables;
	u32 versions;
	const struct qcom_cpufreq_match_data *data;
};

static struct platform_device *cpufreq_dt_pdev, *cpufreq_pdev;

static enum _msm8996_version qcom_cpufreq_get_msm_id(void)
{
	size_t len;
	u32 *msm_id;
	enum _msm8996_version version;

	msm_id = qcom_smem_get(QCOM_SMEM_HOST_ANY, MSM_ID_SMEM, &len);
	if (IS_ERR(msm_id))
		return NUM_OF_MSM8996_VERSIONS;

	/* The first 4 bytes are format, next to them is the actual msm-id */
	msm_id++;

	switch ((enum _msm_id)*msm_id) {
	case MSM8996V3:
	case APQ8096V3:
		version = MSM8996_V3;
		break;
	case MSM8996SG:
	case APQ8096SG:
		version = MSM8996_SG;
		break;
	case IPQ5332V1:
	case IPQ5322V1:
	case IPQ5312V1:
	case IPQ5302V1:
	case IPQ5300V1:
		version = IPQ53XX_V1;
		break;
	case IPQ6018V1:
	case IPQ6028V1:
	case IPQ6010V1:
	case IPQ6005V1:
		version = IPQ60XX_V1;
		break;
	case IPQ6000V1:
		version = IPQ6000_V1;
		break;
	case IPQ9574V1:
	case IPQ9570V1:
	case IPQ9554V1:
	case IPQ9550V1:
	case IPQ9514V1:
	case IPQ9510V1:
		version = IPQ95XX_V1;
		break;
	default:
		version = NUM_OF_MSM8996_VERSIONS;
	}

	return version;
}

static int qcom_cpufreq_kryo_name_version(struct device *cpu_dev,
					  struct nvmem_cell *speedbin_nvmem,
					  struct qcom_cpufreq_drv *drv)
{
	size_t len;
	u8 *speedbin;
	enum _msm8996_version msm8996_version;

	msm8996_version = qcom_cpufreq_get_msm_id();
	if (NUM_OF_MSM8996_VERSIONS == msm8996_version) {
		dev_err(cpu_dev, "Not Snapdragon 820/821!");
		return -ENODEV;
	}

	speedbin = nvmem_cell_read(speedbin_nvmem, &len);
	if (IS_ERR(speedbin))
		return PTR_ERR(speedbin);

	switch (msm8996_version) {
	case MSM8996_V3:
		drv->versions = 1 << (unsigned int)(*speedbin);
		break;
	case MSM8996_SG:
		drv->versions = 1 << ((unsigned int)(*speedbin) + 4);
		break;
	case IPQ53XX_V1:
		/* Fuse Value    Freq    BIT to set
		 * ---------------------------------
		 *   2’b00     No Limit     BIT(0)
		 *   2’b01     1.5 GHz      BIT(1)
		 *   2’b10     1.2 Ghz      BIT(2)
		 *   2’b11     1.0 GHz      BIT(3)
		 */
		drv->versions = 1 << (unsigned int)(*speedbin);
		break;
	case IPQ95XX_V1:
		/* Fuse Value    Freq    BIT to set
		 * ---------------------------------
		 *   2’b00     No Limit     BIT(0)
		 *   2’b10     1.8 GHz      BIT(1)
		 *   2’b01     1.5 Ghz      BIT(2)
		 *   2’b11     1.2 GHz      BIT(3)  */
		if ((unsigned int)(*speedbin) == 2)
			drv->versions = BIT(1);
		else if ((unsigned int)(*speedbin) == 1)
			drv->versions = BIT(2);
		else
			drv->versions = 1 << (unsigned int)(*speedbin);
		break;
	case IPQ60XX_V1:
		/* Fuse Value    Freq    BIT to set
		 * ---------------------------------
		 *   2’b0     No Limit     BIT(0)
		 *   2’b1     1.5 GHz      BIT(1)
		 */
		drv->versions = 1 << (unsigned int)(*speedbin);
		break;
	case IPQ6000_V1:
		/**
		 * IPQ60xx family of SoCs has only one bit in QFPROM to
		 * limit the CPU frequency to 1.5GHz. IPQ6000 variant
		 * is limited to 1.2GHz. So lets hard code the value.
		 */
		drv->versions = BIT(2);
		break;
	default:
		BUG();
		break;
	}

	kfree(speedbin);
	return 0;
}

static const struct qcom_cpufreq_match_data match_data_kryo = {
	.get_version = qcom_cpufreq_kryo_name_version,
};

static const char *qcs404_genpd_names[] = { "cpr", NULL };

static const struct qcom_cpufreq_match_data match_data_qcs404 = {
	.genpd_names = qcs404_genpd_names,
};

static int qcom_cpufreq_probe(struct platform_device *pdev)
{
	struct qcom_cpufreq_drv *drv;
	struct nvmem_cell *speedbin_nvmem;
	struct device_node *np;
	struct device *cpu_dev;
	unsigned cpu;
	const struct of_device_id *match;
	int ret;

	cpu_dev = get_cpu_device(0);
	if (!cpu_dev)
		return -ENODEV;

	np = dev_pm_opp_of_get_opp_desc_node(cpu_dev);
	if (!np)
		return -ENOENT;

	ret = of_device_is_compatible(np, "operating-points-v2-kryo-cpu") ||
		of_device_is_compatible(np, "operating-points-v2-ipq5332") ||
		of_device_is_compatible(np, "operating-points-v2-ipq6018");
	if (!ret) {
		of_node_put(np);
		return -ENOENT;
	}

	drv = kzalloc(sizeof(*drv), GFP_KERNEL);
	if (!drv)
		return -ENOMEM;

	match = pdev->dev.platform_data;
	drv->data = match->data;
	if (!drv->data) {
		ret = -ENODEV;
		goto free_drv;
	}

	if (drv->data->get_version) {
		speedbin_nvmem = of_nvmem_cell_get(np, NULL);
		if (IS_ERR(speedbin_nvmem)) {
			if (PTR_ERR(speedbin_nvmem) != -EPROBE_DEFER)
				dev_err(cpu_dev,
					"Could not get nvmem cell: %ld\n",
					PTR_ERR(speedbin_nvmem));
			ret = PTR_ERR(speedbin_nvmem);
			goto free_drv;
		}

		ret = drv->data->get_version(cpu_dev, speedbin_nvmem, drv);
		if (ret) {
			nvmem_cell_put(speedbin_nvmem);
			goto free_drv;
		}
		nvmem_cell_put(speedbin_nvmem);
	}
	of_node_put(np);

	drv->opp_tables = kcalloc(num_possible_cpus(), sizeof(*drv->opp_tables),
				  GFP_KERNEL);
	if (!drv->opp_tables) {
		ret = -ENOMEM;
		goto free_drv;
	}

	drv->genpd_opp_tables = kcalloc(num_possible_cpus(),
					sizeof(*drv->genpd_opp_tables),
					GFP_KERNEL);
	if (!drv->genpd_opp_tables) {
		ret = -ENOMEM;
		goto free_opp;
	}

	for_each_possible_cpu(cpu) {
		cpu_dev = get_cpu_device(cpu);
		if (NULL == cpu_dev) {
			ret = -ENODEV;
			goto free_genpd_opp;
		}

		if (drv->data->get_version) {
			drv->opp_tables[cpu] =
				dev_pm_opp_set_supported_hw(cpu_dev,
							    &drv->versions, 1);
			if (IS_ERR(drv->opp_tables[cpu])) {
				ret = PTR_ERR(drv->opp_tables[cpu]);
				dev_err(cpu_dev,
					"Failed to set supported hardware\n");
				goto free_genpd_opp;
			}
		}

		if (drv->data->genpd_names) {
			drv->genpd_opp_tables[cpu] =
				dev_pm_opp_attach_genpd(cpu_dev,
							drv->data->genpd_names,
							NULL);
			if (IS_ERR(drv->genpd_opp_tables[cpu])) {
				ret = PTR_ERR(drv->genpd_opp_tables[cpu]);
				if (ret != -EPROBE_DEFER)
					dev_err(cpu_dev,
						"Could not attach to pm_domain: %d\n",
						ret);
				goto free_genpd_opp;
			}
		}
	}

	cpufreq_dt_pdev = platform_device_register_simple("cpufreq-dt", -1,
							  NULL, 0);
	if (!IS_ERR(cpufreq_dt_pdev)) {
		platform_set_drvdata(pdev, drv);
		return 0;
	}

	ret = PTR_ERR(cpufreq_dt_pdev);
	dev_err(cpu_dev, "Failed to register platform device\n");

free_genpd_opp:
	for_each_possible_cpu(cpu) {
		if (IS_ERR_OR_NULL(drv->genpd_opp_tables[cpu]))
			break;
		dev_pm_opp_detach_genpd(drv->genpd_opp_tables[cpu]);
	}
	kfree(drv->genpd_opp_tables);
free_opp:
	for_each_possible_cpu(cpu) {
		if (IS_ERR_OR_NULL(drv->opp_tables[cpu]))
			break;
		dev_pm_opp_put_supported_hw(drv->opp_tables[cpu]);
	}
	kfree(drv->opp_tables);
free_drv:
	kfree(drv);

	return ret;
}

static int qcom_cpufreq_remove(struct platform_device *pdev)
{
	struct qcom_cpufreq_drv *drv = platform_get_drvdata(pdev);
	unsigned int cpu;

	platform_device_unregister(cpufreq_dt_pdev);

	for_each_possible_cpu(cpu) {
		if (drv->opp_tables[cpu])
			dev_pm_opp_put_supported_hw(drv->opp_tables[cpu]);
		if (drv->genpd_opp_tables[cpu])
			dev_pm_opp_detach_genpd(drv->genpd_opp_tables[cpu]);
	}

	kfree(drv->opp_tables);
	kfree(drv->genpd_opp_tables);
	kfree(drv);

	return 0;
}

static struct platform_driver qcom_cpufreq_driver = {
	.probe = qcom_cpufreq_probe,
	.remove = qcom_cpufreq_remove,
	.driver = {
		.name = "qcom-cpufreq-nvmem",
	},
};

static const struct of_device_id qcom_cpufreq_match_list[] __initconst = {
	{ .compatible = "qcom,apq8096", .data = &match_data_kryo },
	{ .compatible = "qcom,msm8996", .data = &match_data_kryo },
	{ .compatible = "qcom,qcs404", .data = &match_data_qcs404 },
	{ .compatible = "qcom,ipq5332", .data = &match_data_kryo },
	{ .compatible = "qcom,ipq6018", .data = &match_data_kryo },
	{ .compatible = "qcom,ipq9574", .data = &match_data_kryo },
	{},
};
MODULE_DEVICE_TABLE(of, qcom_cpufreq_match_list);

/*
 * Since the driver depends on smem and nvmem drivers, which may
 * return EPROBE_DEFER, all the real activity is done in the probe,
 * which may be defered as well. The init here is only registering
 * the driver and the platform device.
 */
static int __init qcom_cpufreq_init(void)
{
	struct device_node *np = of_find_node_by_path("/");
	const struct of_device_id *match;
	int ret;

	if (!np)
		return -ENODEV;

	match = of_match_node(qcom_cpufreq_match_list, np);
	of_node_put(np);
	if (!match)
		return -ENODEV;

	ret = platform_driver_register(&qcom_cpufreq_driver);
	if (unlikely(ret < 0))
		return ret;

	cpufreq_pdev = platform_device_register_data(NULL, "qcom-cpufreq-nvmem",
						     -1, match, sizeof(*match));
	ret = PTR_ERR_OR_ZERO(cpufreq_pdev);
	if (0 == ret)
		return 0;

	platform_driver_unregister(&qcom_cpufreq_driver);
	return ret;
}
module_init(qcom_cpufreq_init);

static void __exit qcom_cpufreq_exit(void)
{
	platform_device_unregister(cpufreq_pdev);
	platform_driver_unregister(&qcom_cpufreq_driver);
}
module_exit(qcom_cpufreq_exit);

MODULE_DESCRIPTION("Qualcomm Technologies, Inc. CPUfreq driver");
MODULE_LICENSE("GPL v2");
