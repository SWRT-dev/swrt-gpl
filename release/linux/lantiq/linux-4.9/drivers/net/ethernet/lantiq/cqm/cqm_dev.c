#include "cqm_dev.h"

#define CQM_NUM_DEV_SUPP 2
#define MAX_NUM_BASE_ADDR 16
#define MAX_NUM_INTR 8
static struct dt_node_inst dev_node_name[CQM_NUM_DEV_SUPP] = {
	{PRX300_DEV_NAME, "cqm", 0},
	{GRX500_DEV_NAME, "cbm", 1}
};

static struct device_node *parse_dts(int j, void **pdata, struct resource **res,
				     int *num_res);

static int add_cqm_dev(int i);
static int cqm_platdev_parse_dts(void);

void __attribute__((weak)) cqm_rst(struct cqm_data *lpp)
{
}

struct device_node *parse_dts(int j, void **pdata, struct resource **res,
			      int *num_res)
{
	struct device_node *node = NULL, *cpu_deq_port = NULL;
	int idx = 0;
	struct cqm_data *cqm_pdata = NULL;
	unsigned int intr[MAX_NUM_INTR];
	struct resource resource[MAX_NUM_BASE_ADDR];
	struct property *prop;
	const __be32 *p;
	unsigned int buf_num, split_num, split;
	unsigned int pool_size = 0;
	u8 count = 0;

	node = of_find_node_by_name(NULL, dev_node_name[j].node_name);
	if (!node)
		return NULL;
	*pdata = kzalloc(sizeof(*cqm_pdata), GFP_KERNEL);
	if (!*pdata) {
		pr_err("%s: Failed to allocate pdata.\n", __func__);
		goto err_free_pdata;
	}
	cqm_pdata = (struct cqm_data *)(*pdata);
	for (idx = 0; idx < MAX_NUM_BASE_ADDR; idx++) {
		if (of_address_to_resource(node, idx, &resource[idx]))
			break;
	}
	*res = kmalloc_array(idx, sizeof(struct resource),
					 GFP_KERNEL);
	if (!*res) {
		pr_info("error allocating memory\n");
		goto err_free_pdata;
	}
	memcpy(*res, resource, (sizeof(struct resource) * idx));
	cqm_pdata->num_resources = idx;
	*num_res = idx;
	pr_debug("num_res %d\n", *num_res);

	for (idx = 0; idx < MAX_NUM_INTR; idx++) {
		intr[idx] = irq_of_parse_and_map(node, idx);
		pr_debug("intr %d\n", intr[idx]);
		if (!intr[idx])
			break;
	}
	cqm_pdata->num_intrs = idx;
	cqm_pdata->intrs = kmalloc_array(idx, sizeof(unsigned int),
						   GFP_KERNEL);
	if (!cqm_pdata->intrs)
		goto err_free_pdata;
	memcpy(cqm_pdata->intrs, intr, (sizeof(unsigned int) * idx));
	cqm_pdata->rcu_reset = of_reset_control_get(node, "cqm");
	if (IS_ERR(cqm_pdata->rcu_reset)) {
		pr_debug("No rcu reset for %s\n", dev_node_name[j].node_name);
	} else {
		pr_debug("cqm reset deassert\n");
		cqm_rst(cqm_pdata);
	}

	cqm_pdata->syscfg = syscon_regmap_lookup_by_phandle(node,
							    "intel,syscon");
	if (IS_ERR(cqm_pdata->syscfg)) {
		pr_err("No syscon phandle specified for wan mux\n");
		cqm_pdata->syscfg = NULL;
	}

	of_property_read_u32(node, "intel,radio-32-vap",
			     &cqm_pdata->radio_dev_num);
	pr_debug("num of radio dev : %d\n", cqm_pdata->radio_dev_num);

	cqm_pdata->force_xpcs = of_property_read_bool(node, "intel,force-xpcs");
	/*checking split shared pools for US and DS*/
	split_num = 0;
	of_property_for_each_u32(node, "intel,bm-buff-split", prop, p,
				 split) {
		if (split_num < 2) {
			cqm_pdata->bm_buff_split[split_num++] = split;
		} else {
			pr_err("intel,bm-buff-split invalid entry: %d\n",
			       split_num);
			goto err_free_pdata;
		}
	}

	of_property_for_each_u32(node, "intel,bm-buff-num",
				 prop, p, buf_num) {
		if (cqm_pdata->num_pools < MAX_NUM_POOLS) {
			cqm_pdata->pool_ptrs[cqm_pdata->num_pools++] =
				buf_num;
		} else {
			pr_err("intel,bm-buff-num invalid entry: %d\n",
			       cqm_pdata->num_pools);
			goto err_free_pdata;
		}
	}

	of_property_for_each_u32(node, "intel,bm-buff-size",
				 prop, p, buf_num) {
		if (pool_size < MAX_NUM_POOLS) {
			cqm_pdata->pool_size[pool_size++] = buf_num;
		} else {
			pr_err("intel,bm-buff-size invalid entry: %d\n",
			       pool_size);
			goto err_free_pdata;
		}
	}

	if (cqm_pdata->num_pools != pool_size) {
		pr_err("buff num and buff size mismatch\n");
		goto err_free_pdata;
	}

	pool_size = 0;
	of_property_for_each_u32(node, "intel,bm-buff-num-a1", prop, p,
				 buf_num) {
		if (cqm_pdata->num_pools_a1 < MAX_NUM_POOLS) {
			cqm_pdata->pool_ptrs_a1[cqm_pdata->num_pools_a1++] =
				buf_num;
		} else {
			pr_err("intel,bm-buff-num-a1 invalid entry: %d\n",
			       cqm_pdata->num_pools_a1);
			goto err_free_pdata;
		}
	}

	of_property_for_each_u32(node, "intel,bm-buff-size-a1", prop, p,
				 buf_num) {
		if (pool_size < MAX_NUM_POOLS) {
			cqm_pdata->pool_size_a1[pool_size++] = buf_num;
		} else {
			pr_err("intel,bm-buff-size-a1 invalid entry: %d\n",
			       pool_size);
			goto err_free_pdata;
		}
	}

	if (cqm_pdata->num_pools_a1 != pool_size) {
		pr_err("buff num and buff size mismatch\n");
		goto err_free_pdata;
	}

	for_each_available_child_of_node(node, cpu_deq_port) {
		if (of_property_count_u32_elems(cpu_deq_port, "intel,deq-port")
						!= MAX_CPU_DQ_PORT_ARGS) {
			pr_err("Invalid args in %s\n", cpu_deq_port->name);
			continue;
		}

		of_property_for_each_u32(cpu_deq_port, "intel,deq-port", prop,
					 p, cqm_pdata->dq_port[count]) {
			count++;
		}

		if (count >= MAX_CPU_DQ_PORT_N_TYPE)
			break;
	}
	cqm_pdata->num_dq_port = count;

	if (of_property_read_s32(node, "intel,gint-mode",
				 &cqm_pdata->gint_mode))
		cqm_pdata->gint_mode = -1;

	if (of_property_read_s32(node, "intel,highprio-lan",
				 &cqm_pdata->highprio_lan))
		cqm_pdata->highprio_lan = -1;

	if (of_property_read_u32(node, "intel,re-insertion",
				 &cqm_pdata->re_insertion))
		cqm_pdata->re_insertion = -1;

	return node;

err_free_pdata:
	kfree(pdata);
	kfree(res);
	return NULL;
}

int add_cqm_dev(int i)
{
	struct platform_device *pdev = NULL;
	void *pdata = NULL;
	struct device_node *node = NULL;
	struct resource *res;
	int ret = CBM_SUCCESS, num_res;

	node = parse_dts(i, &pdata, &res, &num_res);
	if (!node) {
		pr_debug("%s(#%d): parse_dts fail for %s\n",
		       __func__, __LINE__, dev_node_name[i].dev_name);
		return CBM_FAILURE;
	}
#if 1

	pdev = platform_device_alloc(dev_node_name[i].dev_name, 1);
	if (!pdev) {
		pr_err("%s(#%d): platform_device_alloc fail for %s\n",
		       __func__, __LINE__, dev_node_name[i].node_name);
		return -ENOMEM;
	}

	/* Attach node into platform device of_node */
	pdev->dev.of_node = node;
	/* Set the  private data */
	if (pdata)
		platform_set_drvdata(pdev, pdata);
	/* Add resources to platform device */
	if ((num_res > 0) && res) {
		ret = platform_device_add_resources(pdev, res, num_res);
		if (ret) {
			pr_info("%s: Failed to add resources for %s.\n",
				__func__, dev_node_name[i].node_name);
			goto err_free_pdata;
		}
	}

	/* Add platform device */
	ret = platform_device_add(pdev);
	if (ret) {
		pr_info("%s: Failed to add platform device for %s.\n",
			__func__, dev_node_name[i].node_name);
		goto err_free_pdata;
	}
#endif
	
	kfree(res);
	return ret;

err_free_pdata:
	kfree(pdata);
	kfree(res);
	return ret;
}

int nodefromdevice(const char *dev)
{
	int i;

	for (i = 0; i < CQM_NUM_DEV_SUPP; i++) {
		if (strcmp(dev_node_name[i].dev_name, dev) == 0)
			return i;
	}
	return -1;
}

static int cqm_platdev_parse_dts(void)
{
	int i, dev_add = 0;

	for (i = 0; i < CQM_NUM_DEV_SUPP; i++) {
		if (!add_cqm_dev(i))
			dev_add++;
	}
	if (!dev_add)
		pr_err("Not Even 1 CBM device registered\n");
	return 0;
}

static __init int cqm_platdev_init(void)
{
	cqm_platdev_parse_dts();
	return 0;
}

arch_initcall(cqm_platdev_init);

MODULE_LICENSE("GPL");
