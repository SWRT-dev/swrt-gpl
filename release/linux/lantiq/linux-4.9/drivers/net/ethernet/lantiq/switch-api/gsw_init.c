/******************************************************************************
                Copyright (c) 2016, 2017 Intel Corporation

******************************************************************************/
/*****************************************************************************
                Copyright (c) 2012, 2014, 2015
                    Lantiq Deutschland GmbH
    For licensing information, see the file 'LICENSE' in the root folder of
    this software module.
******************************************************************************/
#ifdef __KERNEL__
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h> /* printk() */
#include <linux/moduleparam.h>
#include <linux/platform_device.h>
#include <linux/export.h>
#include <linux/clk.h>
#include <linux/of_address.h>
#include <linux/of_platform.h>
#include <linux/version.h>

#ifndef CONFIG_X86_INTEL_CE2700

#include <lantiq.h>
#include <lantiq_soc.h>
#else
#include <linux/netip_subsystem.h>
#include <linux/avalanche/generic/modphy_mrpc_api.h>
extern int DWC_ETH_QOS_mdio_read_direct(int bus_number, bool c45, int phyaddr, int mmd, int phyreg, int *phydata);
extern int DWC_ETH_QOS_mdio_write_direct(int bus_number, bool c45, int phyaddr, int mmd, int phyreg, int phydata);

#define MDIO_ADDR_LANTIQ 31
#define C45_ENABLED 0
#define MMD_DISABLED 32
#define MDIO_BUS_NUMBER_0 0

/* netip-subsystem gpio value set register*/
#define DATA_OUT_SET_REG_OFFSET 0x14
/* netip-subsystem gpio value clear register*/
#define DATA_OUT_CLEAR_REG_OFFSET 0x18
/* netip-subsystem gpio directio set register*/
#define OE_SET_REG_OFFSET 0x20

#define NETSS_GPIO_17 (1 << 17)
#define NETSS_GPIO_19 (1 << 19)

#endif /*CONFIG_X86_INTEL_CE2700*/


#define sw_w32(x, y)	ltq_w32((x), (addr_gswl + (y)))
#define sw_r32(x)			ltq_r32(addr_gswl + (x))

#define gsw1_w32(x, y)	ltq_w32((x), ((y)))
#define gsw1_r32(x)			ltq_r32((x))


void __iomem		*addr_gswl;
void __iomem		*addr_gswr;
void __iomem		*addr_gsw;

#endif /* KERNEL_MODE */

#if defined(WIN_PC_MODE) && WIN_PC_MODE
void 		*addr_gswl;
void 		*addr_gswr;
void 		*addr_gsw;
#endif

#include <gsw_init.h>

#if defined(UART_INTERFACE) && UART_INTERFACE
static int uart_reg_rd(u32 regaddr, u32 *data);
static int uart_reg_wr(u32 regaddr, u32 data);
#endif /* UART_INTERFACE */

#define GSW_API_MAJOR_NUMBER	81
#define LTQ_INT_GSWITCH			0
#define LTQ_EXT_GSWITCH			1

static const u16 gsw_ops_type[] = {
	GSW_RMON_MAGIC,
	GSW_MAC_MAGIC,
	GSW_EXVLAN_MAGIC,
	GSW_VLANFILTER_MAGIC,
	GSW_CTP_MAGIC,
	GSW_BRDGPORT_MAGIC,
	GSW_BRDG_MAGIC,
	GSW_TFLOW_MAGIC,
	GSW_QOS_MAGIC,
	GSW_STP_MAGIC,
	GSW_EAPOL_MAGIC,
	GSW_MULTICAST_MAGIC,
	GSW_TRUNKING_MAGIC,
	GSW_WOL_MAGIC,
	GSW_VLAN_MAGIC,
	GSW_PMAC_MAGIC,
	GSW_COMMON_MAGIC,
	GSW_ROUTE_MAGIC,
	GSW_DEBUG_MAGIC,
	GSW_IRQ_MAGIC
};


extern ltq_lowlevel_fkts_t ltq_flow_fkt_tbl;
#ifdef __KERNEL__
extern ltq_lowlevel_fkts_t ltq_rt_fkt_tbl;
#endif

ioctl_wrapper_init_t ioctlinit;
ioctl_wrapper_ctx_t *pioctlctl = NULL;
ethsw_api_dev_t *pEDev0 = NULL, *pEDev1 = NULL, *pEDevExt = NULL;
struct ltq_lowlevel_fkts_t	 *ioct_cmd_start_node = NULL;

#if defined(WIN_PC_MODE) && WIN_PC_MODE
struct core_ops *gsw_get_swcore_ops(u32 devid)
{
	if (devid == 0)
		return (&pEDev0->ops);

	if (devid == 1)
		return (&pEDev1->ops);
}

#endif

static void insert_ioctl_type(ltq_lowlevel_fkts_t ops_content, struct fkts_linklist *node)
{
	ltq_lowlevel_fkts_t	*current_ptr;

#ifdef __KERNEL__
	current_ptr = (ltq_lowlevel_fkts_t *)kmalloc(sizeof(ltq_lowlevel_fkts_t), GFP_KERNEL);
#else
	current_ptr = (ltq_lowlevel_fkts_t *)malloc(sizeof(ltq_lowlevel_fkts_t));
#endif

	if (current_ptr == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return;
	}

	//printk("ltq_lowlevel_fkts_t ptr =%x\n",(unsigned int)current_ptr);
	current_ptr->pNext = NULL;
	current_ptr->nType = ops_content.nType;
	current_ptr->nNumFkts = (ops_content.nNumFkts + 1);
	current_ptr->pFkts = ops_content.pFkts;

	if (node->first_ptr != NULL) {
		node->last_ptr->pNext = current_ptr;
		node->last_ptr = current_ptr;
	} else {
		node->first_ptr = node->last_ptr = current_ptr;
	}
}

static void gsw_delete_ioctl_cmd_linklist(ltq_lowlevel_fkts_t *first_node)
{
	ltq_lowlevel_fkts_t *free_node, *last_node;
	last_node = first_node;

	while (last_node != NULL) {
		free_node = last_node;
		last_node = last_node->pNext;
#ifdef __KERNEL__
		kfree(free_node);
#else
		free(free_node);
#endif
	}
}

static ltq_lowlevel_fkts_t *gsw_create_ioctl_cmd_linklist(struct core_ops *ops)
{
	struct fkts_linklist *node;
	ltq_lowlevel_fkts_t *first_node;
	ltq_lowlevel_fkts_t ops_content;
	u16 i, num_of_types = 0;
#ifdef __KERNEL__
	node = (struct fkts_linklist *)kmalloc(sizeof(struct fkts_linklist), GFP_KERNEL);
#else
	node = (struct fkts_linklist *)malloc(sizeof(struct fkts_linklist));
#endif

	if (node == NULL)
		return (ltq_lowlevel_fkts_t *)node;

	node->first_ptr = NULL;
	node->last_ptr = NULL;

	num_of_types = (sizeof(gsw_ops_type) / sizeof(gsw_ops_type[0]));

	for (i = 0; i < num_of_types; i++) {
		switch (gsw_ops_type[i]) {
		case GSW_RMON_MAGIC:
			ops_content.pNext = NULL;
			//printk("address of rmon =%x\n",(unsigned int)&ops->gsw_rmon_ops);
			ops_content.pFkts = (LTQ_ll_fkt *)&ops->gsw_rmon_ops;
			ops_content.nType = GSW_RMON_MAGIC;
			//printk("size of pdata_t.ops.gsw_rmon_ops = %x\n",(sizeof(ops->gsw_rmon_ops)/sizeof(ops_content.pFkts[0])));
			ops_content.nNumFkts = (sizeof(ops->gsw_rmon_ops) / sizeof(ops_content.pFkts[0]));
			insert_ioctl_type(ops_content, node);
			break;

		case GSW_MAC_MAGIC:
			ops_content.pNext = NULL;
			//printk("address of mac =%x\n",(unsigned int)&ops->gsw_swmac_ops);
			ops_content.pFkts = (LTQ_ll_fkt *)&ops->gsw_swmac_ops;
			ops_content.nType = GSW_MAC_MAGIC;
			//printk("size of pdata_t.ops.gsw_mac_ops = %x\n",(sizeof(ops->gsw_swmac_ops)/sizeof(ops_content.pFkts[0])));
			ops_content.nNumFkts = (sizeof(ops->gsw_swmac_ops) / sizeof(ops_content.pFkts[0]));
			insert_ioctl_type(ops_content, node);
			break;

		case GSW_EXVLAN_MAGIC:
			ops_content.pNext = NULL;
			//printk("address of gsw_extvlan_ops =%x\n",(unsigned int)&ops->gsw_extvlan_ops);
			ops_content.pFkts = (LTQ_ll_fkt *)&ops->gsw_extvlan_ops;
			ops_content.nType = GSW_EXVLAN_MAGIC;
			//printk("size of pdata_t.ops.gsw_extvlan_ops = %x\n",(sizeof(ops->gsw_extvlan_ops)/sizeof(ops_content.pFkts[0])));
			ops_content.nNumFkts = (sizeof(ops->gsw_extvlan_ops) / sizeof(ops_content.pFkts[0]));
			insert_ioctl_type(ops_content, node);
			break;

		case GSW_VLANFILTER_MAGIC:
			ops_content.pNext = NULL;
			//printk("address of gsw_vlanfilter_ops =%x\n",(unsigned int)&ops->gsw_vlanfilter_ops);
			ops_content.pFkts = (LTQ_ll_fkt *)&ops->gsw_vlanfilter_ops;
			ops_content.nType = GSW_VLANFILTER_MAGIC;
			//printk("size of pdata_t.ops.gsw_vlanfilter_ops = %x\n",(sizeof(ops->gsw_vlanfilter_ops)/sizeof(ops_content.pFkts[0])));
			ops_content.nNumFkts = (sizeof(ops->gsw_vlanfilter_ops) / sizeof(ops_content.pFkts[0]));
			insert_ioctl_type(ops_content, node);
			break;

		case GSW_CTP_MAGIC:
			ops_content.pNext = NULL;
			//printk("address of gsw_ctp_ops =%x\n",(unsigned int)&ops->gsw_ctp_ops);
			ops_content.pFkts = (LTQ_ll_fkt *)&ops->gsw_ctp_ops;
			ops_content.nType = GSW_CTP_MAGIC;
			//printk("size of pdata_t.ops.gsw_ctp_ops = %x\n",(sizeof(ops->gsw_ctp_ops)/sizeof(ops_content.pFkts[0])));
			ops_content.nNumFkts = (sizeof(ops->gsw_ctp_ops) / sizeof(ops_content.pFkts[0]));
			insert_ioctl_type(ops_content, node);
			break;

		case GSW_BRDGPORT_MAGIC:
			ops_content.pNext = NULL;
			//printk("address of gsw_brdgport_ops =%x\n",(unsigned int)&ops->gsw_brdgport_ops);
			ops_content.pFkts = (LTQ_ll_fkt *)&ops->gsw_brdgport_ops;
			ops_content.nType = GSW_BRDGPORT_MAGIC;
			//printk("size of pdata_t.ops.gsw_brdgport_ops = %x\n",(sizeof(ops->gsw_brdgport_ops)/sizeof(ops_content.pFkts[0])));
			ops_content.nNumFkts = (sizeof(ops->gsw_brdgport_ops) / sizeof(ops_content.pFkts[0]));
			insert_ioctl_type(ops_content, node);
			break;

		case GSW_BRDG_MAGIC:
			ops_content.pNext = NULL;
			//printk("address of gsw_brdg_ops =%x\n",(unsigned int)&ops->gsw_brdg_ops);
			ops_content.pFkts = (LTQ_ll_fkt *)&ops->gsw_brdg_ops;
			ops_content.nType = GSW_BRDG_MAGIC;
			//printk("size of pdata_t.ops.gsw_brdg_ops = %x\n",(sizeof(ops->gsw_brdg_ops)/sizeof(ops_content.pFkts[0])));
			ops_content.nNumFkts = (sizeof(ops->gsw_brdg_ops) / sizeof(ops_content.pFkts[0]));
			insert_ioctl_type(ops_content, node);
			break;

		case GSW_TFLOW_MAGIC:
			ops_content.pNext = NULL;
			//printk("address of gsw_tflow_ops =%x\n",(unsigned int)&ops->gsw_tflow_ops);
			ops_content.pFkts = (LTQ_ll_fkt *)&ops->gsw_tflow_ops;
			ops_content.nType = GSW_TFLOW_MAGIC;
			//printk("size of pdata_t.ops.gsw_tflow_ops = %x\n",(sizeof(ops->gsw_tflow_ops)/sizeof(ops_content.pFkts[0])));
			ops_content.nNumFkts = (sizeof(ops->gsw_tflow_ops) / sizeof(ops_content.pFkts[0]));
			insert_ioctl_type(ops_content, node);
			break;

		case GSW_QOS_MAGIC:
			ops_content.pNext = NULL;
			//printk("address of gsw_qos_ops =%x\n",(unsigned int)&ops->gsw_qos_ops);
			ops_content.pFkts = (LTQ_ll_fkt *)&ops->gsw_qos_ops;
			ops_content.nType = GSW_QOS_MAGIC;
			//printk("size of pdata_t.ops.gsw_qos_ops = %x\n",(sizeof(ops->gsw_qos_ops)/sizeof(ops_content.pFkts[0])));
			ops_content.nNumFkts = (sizeof(ops->gsw_qos_ops) / sizeof(ops_content.pFkts[0]));
			insert_ioctl_type(ops_content, node);
			break;

		case GSW_STP_MAGIC:
			ops_content.pNext = NULL;
			//printk("address of gsw_stp_ops =%x\n",(unsigned int)&ops->gsw_stp_ops);
			ops_content.pFkts = (LTQ_ll_fkt *)&ops->gsw_stp_ops;
			ops_content.nType = GSW_STP_MAGIC;
			//printk("size of pdata_t.ops.gsw_stp_ops = %x\n",(sizeof(ops->gsw_stp_ops)/sizeof(ops_content.pFkts[0])));
			ops_content.nNumFkts = (sizeof(ops->gsw_stp_ops) / sizeof(ops_content.pFkts[0]));
			insert_ioctl_type(ops_content, node);
			break;

		case GSW_EAPOL_MAGIC:
			ops_content.pNext = NULL;
			//printk("address of gsw_8021x_ops =%x\n",(unsigned int)&ops->gsw_8021x_ops);
			ops_content.pFkts = (LTQ_ll_fkt *)&ops->gsw_8021x_ops;
			ops_content.nType = GSW_EAPOL_MAGIC;
			//printk("size of pdata_t.ops.gsw_8021x_ops = %x\n",(sizeof(ops->gsw_8021x_ops)/sizeof(ops_content.pFkts[0])));
			ops_content.nNumFkts = (sizeof(ops->gsw_8021x_ops) / sizeof(ops_content.pFkts[0]));
			insert_ioctl_type(ops_content, node);
			break;

		case GSW_MULTICAST_MAGIC:
			ops_content.pNext = NULL;
			//printk("address of gsw_multicast_ops =%x\n",(unsigned int)&ops->gsw_multicast_ops);
			ops_content.pFkts = (LTQ_ll_fkt *)&ops->gsw_multicast_ops;
			ops_content.nType = GSW_MULTICAST_MAGIC;
			//printk("size of pdata_t.ops.gsw_multicast_ops = %x\n",(sizeof(ops->gsw_multicast_ops)/sizeof(ops_content.pFkts[0])));
			ops_content.nNumFkts = (sizeof(ops->gsw_multicast_ops) / sizeof(ops_content.pFkts[0]));
			insert_ioctl_type(ops_content, node);
			break;

		case GSW_TRUNKING_MAGIC:
			ops_content.pNext = NULL;
			//printk("address of gsw_trunking_ops =%x\n",(unsigned int)&ops->gsw_trunking_ops);
			ops_content.pFkts = (LTQ_ll_fkt *)&ops->gsw_trunking_ops;
			ops_content.nType = GSW_TRUNKING_MAGIC;
			//printk("size of pdata_t.ops.gsw_trunking_ops = %x\n",(sizeof(ops->gsw_trunking_ops)/sizeof(ops_content.pFkts[0])));
			ops_content.nNumFkts = (sizeof(ops->gsw_trunking_ops) / sizeof(ops_content.pFkts[0]));
			insert_ioctl_type(ops_content, node);
			break;

		case GSW_WOL_MAGIC:
			ops_content.pNext = NULL;
			//printk("address of gsw_wol_ops =%x\n",(unsigned int)&ops->gsw_wol_ops);
			ops_content.pFkts = (LTQ_ll_fkt *)&ops->gsw_wol_ops;
			ops_content.nType = GSW_WOL_MAGIC;
			//printk("size of pdata_t.ops.gsw_wol_ops = %x\n",(sizeof(ops->gsw_wol_ops)/sizeof(ops_content.pFkts[0])));
			ops_content.nNumFkts = (sizeof(ops->gsw_wol_ops) / sizeof(ops_content.pFkts[0]));
			insert_ioctl_type(ops_content, node);
			break;

		case GSW_VLAN_MAGIC:
			ops_content.pNext = NULL;
			//printk("address of gsw_vlan_ops =%x\n",(unsigned int)&ops->gsw_vlan_ops);
			ops_content.pFkts = (LTQ_ll_fkt *)&ops->gsw_vlan_ops;
			ops_content.nType = GSW_VLAN_MAGIC;
			//printk("size of pdata_t.ops.gsw_vlan_ops = %x\n",(sizeof(ops->gsw_vlan_ops)/sizeof(ops_content.pFkts[0])));
			ops_content.nNumFkts = (sizeof(ops->gsw_vlan_ops) / sizeof(ops_content.pFkts[0]));
			insert_ioctl_type(ops_content, node);
			break;

		case GSW_PMAC_MAGIC:
			ops_content.pNext = NULL;
			//printk("address of gsw_pmac_ops =%x\n",(unsigned int)&ops->gsw_pmac_ops);
			ops_content.pFkts = (LTQ_ll_fkt *)&ops->gsw_pmac_ops;
			ops_content.nType = GSW_PMAC_MAGIC;
			//printk("size of pdata_t.ops.gsw_pmac_ops = %x\n",(sizeof(ops->gsw_pmac_ops)/sizeof(ops_content.pFkts[0])));
			ops_content.nNumFkts = (sizeof(ops->gsw_pmac_ops) / sizeof(ops_content.pFkts[0]));
			insert_ioctl_type(ops_content, node);
			break;

		case GSW_ROUTE_MAGIC:
			ops_content.pNext = NULL;
			//printk("address of gsw_pae_ops =%x\n",(unsigned int)&ops->gsw_pae_ops);
			ops_content.pFkts = (LTQ_ll_fkt *)&ops->gsw_pae_ops;
			ops_content.nType = GSW_ROUTE_MAGIC;
			//printk("size of pdata_t.ops.gsw_pae_ops = %x\n",(sizeof(ops->gsw_pae_ops)/sizeof(ops_content.pFkts[0])));
			ops_content.nNumFkts = (sizeof(ops->gsw_pae_ops) / sizeof(ops_content.pFkts[0]));
			insert_ioctl_type(ops_content, node);
			break;

		case GSW_COMMON_MAGIC:
			ops_content.pNext = NULL;
			//printk("address of gsw_common_ops =%x\n",(unsigned int)&ops->gsw_common_ops);
			ops_content.pFkts = (LTQ_ll_fkt *)&ops->gsw_common_ops;
			ops_content.nType = GSW_COMMON_MAGIC;
			//printk("size of pdata_t.ops.gsw_common_ops = %x\n",(sizeof(ops->gsw_common_ops)/sizeof(ops_content.pFkts[0])));
			ops_content.nNumFkts = (sizeof(ops->gsw_common_ops) / sizeof(ops_content.pFkts[0]));
			insert_ioctl_type(ops_content, node);
			break;

		case GSW_DEBUG_MAGIC:
			ops_content.pNext = NULL;
			//printk("address of gsw_debug_ops =%x....\n",(unsigned int)&ops->gsw_debug_ops);
			ops_content.pFkts = (LTQ_ll_fkt *)&ops->gsw_debug_ops;
			ops_content.nType = GSW_DEBUG_MAGIC;
			//printk("size of pdata_t.ops.gsw_debug_ops = %x\n",(sizeof(ops->gsw_debug_ops)/sizeof(ops_content.pFkts[0])));
			ops_content.nNumFkts = (sizeof(ops->gsw_debug_ops) / sizeof(ops_content.pFkts[0]));
			insert_ioctl_type(ops_content, node);
			break;

		case GSW_IRQ_MAGIC:
			ops_content.pNext = NULL;
			//printk("address of gsw_debug_ops =%x....\n",(unsigned int)&ops->gsw_debug_ops);
			ops_content.pFkts = (LTQ_ll_fkt *)&ops->gsw_irq_ops;
			ops_content.nType = GSW_IRQ_MAGIC;
			//printk("size of pdata_t.ops.gsw_irq_ops = %x\n", (sizeof(ops->gsw_irq_ops) / sizeof(ops_content.pFkts[0])));
			ops_content.nNumFkts = (sizeof(ops->gsw_irq_ops) / sizeof(ops_content.pFkts[0]));
			insert_ioctl_type(ops_content, node);
			break;


		default:
			break;
		}
	}

	first_node = node->first_ptr;
#ifdef __KERNEL__
	kfree(node);
#else
	free(node);
#endif
	return first_node;
}

#ifdef __KERNEL__
void gsw_r32_raw(void *cdev, short offset, u32 *value)
{
	ethsw_api_dev_t *pethdev = GSW_PDATA_GET(cdev);

	if (pethdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return;
	}

	if (pethdev->gsw_base != 0) {
		*value = gsw1_r32((volatile void *)pethdev->gsw_base + (offset * 4));
	} else {
		/*external switch*/
		gsw_ext_r32(cdev, offset, 0, 16, value);
	}

}

void gsw_w32_raw(void *cdev, short offset, u32 value)
{
	ethsw_api_dev_t *pethdev = GSW_PDATA_GET(cdev);

	if (pethdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return;
	}

	if (pethdev->gsw_base != 0) {
		gsw1_w32(value, (volatile void *)(pethdev->gsw_base + (offset * 4)));
	} else {
		/*external switch*/
		gsw_ext_w32(cdev, offset, 0, 16, value);
	}
}


int GSW_SMDIO_DataRead(void *cdev, GSW_MDIO_data_t *pPar)
{
	int ret = 0;
#ifdef CONFIG_X86_INTEL_CE2700
	u32 data;
	ret = DWC_ETH_QOS_mdio_read_direct(MDIO_BUS_NUMBER_0, C45_ENABLED,
					   MDIO_ADDR_LANTIQ, MMD_DISABLED, pPar->nAddressReg & 0x1F, &data);
	pPar->nData = data & 0xFFFF;
#else
	struct core_ops *gsw_ops;
	ethsw_api_dev_t *pethdev = GSW_PDATA_GET(cdev);

	if (pethdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}
	gsw_ops = gsw_get_swcore_ops(pethdev->parent_devid);
	if(!gsw_ops) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}
	gsw_ops->gsw_common_ops.MDIO_DataRead(gsw_ops, pPar);

#endif
	return ret;
}

int GSW_SMDIO_DataWrite(void *cdev, GSW_MDIO_data_t *pPar)
{
	int ret = 0;
#ifdef CONFIG_X86_INTEL_CE2700
	ret = DWC_ETH_QOS_mdio_write_direct(MDIO_BUS_NUMBER_0, C45_ENABLED,
					    MDIO_ADDR_LANTIQ, MMD_DISABLED, pPar->nAddressReg & 0x1F, pPar->nData & 0xFFFF);
#else
	struct core_ops *gsw_ops;
	ethsw_api_dev_t *pethdev = GSW_PDATA_GET(cdev);

	if (pethdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

	gsw_ops = gsw_get_swcore_ops(pethdev->parent_devid);
	if(!gsw_ops) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}
	gsw_ops->gsw_common_ops.MDIO_DataWrite(gsw_ops, pPar);
#endif

	return ret;
}

/** read the gswitch register */
void gsw_ext_r32(void *cdev, short offset, short shift, short size, u32 *value)
{
	u32 rvalue, mask;
	GSW_MDIO_data_t mdio_data;
	ethsw_api_dev_t *pethdev = GSW_PDATA_GET(cdev);

	if (pethdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return;
	}

	mdio_data.nAddressDev = pethdev->ext_phyid;
	mdio_data.nAddressReg = 0x1F;

	if ((offset & pethdev->gswex_sgmiibase) == 0xD000)
		mdio_data.nData = (offset);
	else
		mdio_data.nData = (offset | pethdev->gswex_base);

	GSW_SMDIO_DataWrite(cdev, &mdio_data);
	mdio_data.nAddressDev = pethdev->ext_phyid;
	mdio_data.nAddressReg = 0x00;
	mdio_data.nData = 0;
	GSW_SMDIO_DataRead(cdev, &mdio_data);
	rvalue = mdio_data.nData;

	mask = (1 << size) - 1;
	rvalue = (rvalue >> shift);
	*value = (rvalue & mask);

}

/** read and update the GSWIP register */
void gsw_ext_w32(void *cdev, short offset, short shift, short size, u32 value)
{
	u32 rvalue, mask;
	GSW_MDIO_data_t mdio_data;
	ethsw_api_dev_t *pethdev = GSW_PDATA_GET(cdev);

	if (pethdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return;
	}

	mdio_data.nAddressDev = pethdev->ext_phyid;
	mdio_data.nAddressReg = 0x1F;

	if ((offset & pethdev->gswex_sgmiibase) == 0xD000)
		mdio_data.nData = (offset);
	else
		mdio_data.nData = (offset | pethdev->gswex_base);

	GSW_SMDIO_DataWrite(cdev, &mdio_data);

	if (size != 16) {
		mdio_data.nAddressDev = pethdev->ext_phyid;;
		mdio_data.nAddressReg = 0x00;
		mdio_data.nData = 0;
		GSW_SMDIO_DataRead(cdev, &mdio_data);
		rvalue = mdio_data.nData;

		/* Prepare the mask	*/
		mask = (1 << size) - 1 ;
		mask = (mask << shift);
		/* Shift the value to the right place and mask the rest of the bit*/
		value = (value << shift) & mask;
		/*  Mask out the bit field from the read register and place in the new value */
		value = (rvalue & ~mask) | value ;

		mdio_data.nAddressDev = pethdev->ext_phyid;
		mdio_data.nAddressReg = 0x1F;

		if ((offset & pethdev->gswex_sgmiibase) == 0xD000)
			mdio_data.nData = (offset);
		else
			mdio_data.nData = (offset | pethdev->gswex_base);

		GSW_SMDIO_DataWrite(cdev, &mdio_data);
	}

	mdio_data.nAddressDev = pethdev->ext_phyid;
	mdio_data.nAddressReg = 0x0;
	mdio_data.nData = value;
	GSW_SMDIO_DataWrite(cdev, &mdio_data);
}

#endif


#if defined(UART_INTERFACE) && UART_INTERFACE
/* UART inetrface suppot function */
static int uart_reg_rd(u32 regaddr, u32 *data)
{
	/* Add customer UART routines*/
	/* pseudo function */
	pc_uart_dataread(regaddr, data);
	return 1;
}
static int uart_reg_wr(u32 regaddr, u32 data)
{
	/* Add customer UART routines*/
	/* pseudo function */
	pc_uart_datawrite(regaddr, data);
	return 1;
}
#endif /* UART_INTERFACE */

/** read the gswitch register */
void gsw_r32(void *cdev, short offset, short shift, short size, u32 *value)
{
	u32 rvalue, mask;
	ethsw_api_dev_t *pethdev = GSW_PDATA_GET(cdev);

	if (pethdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return;
	}

	if (pethdev->gsw_base != 0) {

#if defined(UART_INTERFACE) && UART_INTERFACE
		u32 ro;
		ro = (uintptr_t)(pethdev->gsw_base + (offset * 4));
		uart_reg_rd(ro, &rvalue);
#else
		rvalue = gsw1_r32((volatile void *)(pethdev->gsw_base + (offset * 4)));
#endif /* UART_INTERFACE */
		mask = (1 << size) - 1;
		rvalue = (rvalue >> shift);
		*value = (rvalue & mask);
	} else {

#ifdef __KERNEL__

		/*external switch*/
		if (pethdev->ext_devid) {
			gsw_ext_r32(cdev, offset, shift, size, value);
		} else
#endif
			pr_err("%s:%s:%d,(ERROR)\n", __FILE__, __func__, __LINE__);
	}

}

/** read and update the GSWIP register */
void gsw_w32(void *cdev, short offset, short shift, short size, u32 value)
{
	u32 rvalue, mask;
	ethsw_api_dev_t *pethdev = GSW_PDATA_GET(cdev);

	if (pethdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return;
	}


	if (pethdev->gsw_base != 0) {

#if defined(UART_INTERFACE) && UART_INTERFACE
		u32 ro;
		ro = (uintptr_t)(pethdev->gsw_base + (offset * 4));
		uart_reg_rd(ro, &rvalue);
#else
		rvalue = gsw1_r32((volatile void *)(pethdev->gsw_base + (offset * 4)));
#endif /* UART_INTERFACE */
		mask = (1 << size) - 1;
		mask = (mask << shift);
		value = ((value << shift) & mask);
		value = ((rvalue & ~mask) | value);
#if defined(UART_INTERFACE) && UART_INTERFACE
		uart_reg_wr(ro, value);
#else
		gsw1_w32(value, ((volatile void *)pethdev->gsw_base + (offset * 4)));
#endif /* UART_INTERFACE */
	} else {

#ifdef __KERNEL__

		/*external switch*/
		if (pethdev->ext_devid)
			gsw_ext_w32(cdev, offset, shift, size, value);
		else
#endif
			pr_err("%s:%s:%d,(ERROR)\n", __FILE__, __func__, __LINE__);
	}
}


#ifdef CONFIG_X86_INTEL_CE2700
void inline gsw_p7_netss_write(void *base, unsigned int off, unsigned int val)
{
	*((volatile unsigned long *)(base + off)) = cpu_to_be32(val);
}

void gsw_p7_netss_unmap(void *base)
{
	iounmap(base);
}

void *gsw_p7_netss_map(netss_dev_t netss_subdevice)
{
	int ret;
	netss_dev_info_t pbase;
	volatile void *vbase = NULL;

	if (!netss_driver_ready()) {
		pr_err("%s:%s:%d (Netss Driver Not Ready)\n",
		       __FILE__, __func__, __LINE__);
		return NULL;
	}

	if (netss_device_get_info(netss_subdevice, &pbase)) {
		pr_err("%s:%s:%d (Netss Get Info Failed)\n",
		       __FILE__, __func__, __LINE__);
		return NULL;
	}

	vbase = ioremap_nocache(pbase.base, pbase.size);

	if (!vbase) {
		pr_err("%s:%s:%d (Virt_base Is Null)\n",
		       __FILE__, __func__, __LINE__);
		return NULL;
	}

	return vbase;
}

int gsw_p7_reset_modphy_lane(void)
{

#ifndef EXT_SWITCH_SGMII0
	ModphyController_e client = MODPHY_SGMII1_2_5G_CLIENT_ID;
#else
	ModphyController_e client = MODPHY_SGMII0_2_5G_CLIENT_ID;
#endif
	modphy_reset_client(client);

	return 0;
}

int gsw_p7_power_on(void)
{
	void *netss_gpio_base = NULL;

	netss_gpio_base = gsw_p7_netss_map(NETSS_DEV_GPIO);

	if (!netss_gpio_base) {
		pr_err("%s:%s:%d (Gpio Base Map Failed)\n",
		       __FILE__, __func__, __LINE__);
		return -1;
	}

	/* set power gpio to output and value 1*/
	gsw_p7_netss_write(netss_gpio_base, OE_SET_REG_OFFSET, NETSS_GPIO_17);
	gsw_p7_netss_write(netss_gpio_base, DATA_OUT_SET_REG_OFFSET,
			   NETSS_GPIO_17);
	mdelay(200);
	/* set reset gpio to output*/
	gsw_p7_netss_write(netss_gpio_base, OE_SET_REG_OFFSET, NETSS_GPIO_19);

	/* set reset gpio value to 0*/
	gsw_p7_netss_write(netss_gpio_base, DATA_OUT_CLEAR_REG_OFFSET,
			   NETSS_GPIO_19);
	mdelay(200);
	/* set reset gpio value to 1*/
	gsw_p7_netss_write(netss_gpio_base, DATA_OUT_SET_REG_OFFSET,
			   NETSS_GPIO_19);
	mdelay(1000);

	gsw_p7_netss_unmap(netss_gpio_base);

	return 0;
}

int ltq_ethsw_api_register_p7(struct platform_device *pdev)
{
	int result;
	ethsw_core_init_t core_init;
	ethsw_api_dev_t *pethdev;
	ioctl_wrapper_ctx_t *iowrap;
	ioctl_wrapper_init_t iowrapinit;

	/* Switch memory not mapped*/
	addr_gsw = 0;

	/* Enable Switch Power  */
	if (gsw_p7_power_on()) {
		pr_err("%s:%s:%d (Switch Power On Failed)\n",
		       __FILE__, __func__, __LINE__);
		return -1;
	}

	/* Init FLOW Switch Core Layer */
	core_init.sdev = LTQ_FLOW_DEV_INT;
	core_init.gsw_base_addr = addr_gsw;
	/*	core_init.pDev = pRALDev; */
	pethdev = ethsw_api_core_init(&core_init);

	if (pethdev == NULL) {
		pr_err("%s:%s:%d (Init Failed)\n",
		       __FILE__, __func__, __LINE__);
		return -1;
	}

	pethdev->cport = GSW_2X_SOC_CPU_PORT;
	pethdev->gsw_base = addr_gsw;
	iowrapinit.pLlTable = &ltq_flow_fkt_tbl;
	iowrapinit.default_handler = NULL;
	iowrap = ioctl_wrapper_init(&iowrapinit);

	if (iowrap == NULL) {
		pr_err("%s:%s:%d (WrapperInit Failed)\n",
		       __FILE__, __func__, __LINE__);
		return -1;
	}

	/* add Internal switch */
	ioctl_wrapper_dev_add(iowrap, pethdev, LTQ_INT_GSWITCH);

	if (gsw_p7_reset_modphy_lane()) {
		pr_err("%s:%s:%d (Reset Modphy Lane Failed)\n",
		       __FILE__, __func__, __LINE__);
		return -1;
	}

	/* Register Char Device */
	result = gsw_api_drv_register(GSW_API_MAJOR_NUMBER);

	if (result != 0) {
		pr_err("%s:%s:%d (Register Char Device Failed)\n",
		       __FILE__, __func__, __LINE__);
		return result;
	}

	return 0;
}

#endif /*CONFIG_X86_INTEL_CE2700*/


#if defined(WIN_PC_MODE) && WIN_PC_MODE

int ethsw_swapi_register(void)
{
	int ret;
	ethsw_core_init_t core_init;

	/* Find and map our resources */
	/* Switch device index */
	addr_gswl = (void *)0x50000;

	/* Init FLOW Switch Core Layer */
	memset(&core_init, 0, sizeof(ethsw_core_init_t));
	core_init.sdev = LTQ_FLOW_DEV_INT;
	core_init.gsw_base_addr = addr_gswl;
	pEDev0 = ethsw_api_core_init(&core_init);

	if (pEDev0 == NULL) {
		printf("%s:%s:%d (Init Failed)\n",
		       __FILE__, __func__, __LINE__);
		return -1;
	}

	pEDev0->cport = GSW_3X_SOC_CPU_PORT;
	pEDev0->gsw_dev = LTQ_FLOW_DEV_INT;
	pEDev0->gswl_base = addr_gswl;
	pEDev0->gsw_base = addr_gswl;

#if defined(GSW_IOCTL_SUPPORT) && GSW_IOCTL_SUPPORT
	ioct_cmd_start_node = gsw_create_ioctl_cmd_linklist(&pEDev0->ops);
	ioctlinit.pLlTable = ioct_cmd_start_node;
	ioctlinit.default_handler = NULL;
	pioctlctl = ioctl_wrapper_init(&ioctlinit);

	if (pioctlctl == NULL) {
		printf("%s:%s:%d (WrapperInit Failed)\n",
		       __FILE__, __func__, __LINE__);
		return -1;
	}

	/* add Internal switch */
	if (pioctlctl && pEDev0)
		ioctl_wrapper_dev_add(pioctlctl, pEDev0, LTQ_INT_GSWITCH);

#endif /* GSW_IOCTL_SUPPORT */
	return 0;
}

int ethsw_swapi_unregister(void)
{
	//ethsw_api_core_exit(pEDev0);
#if defined(GSW_IOCTL_SUPPORT) && GSW_IOCTL_SUPPORT
	gsw_api_ioctl_wrapper_cleanup();
#endif /* GSW_IOCTL_SUPPORT */
	return 0;
}

/*
static int __init gsw_swapi_init(void *)
*/
int gsw_swapi_init()
{
	ethsw_swapi_register();
	return 0;
}

/*
static void __exit ltq_etshw_api_exit(void)
*/
static void gsw_swapi_exit(void)
{
	ethsw_swapi_unregister();
}

#endif /*WIN_PC_MODE*/

#ifdef __KERNEL__

#if defined(CONFIG_SOC_GRX500) && CONFIG_SOC_GRX500
int ltq_gsw_api_register(struct platform_device *pdev)
{
	int result;
	struct resource *memres;
	ethsw_core_init_t core_init;
	struct clk *clk;
	struct gswss *gswdev = dev_get_drvdata(pdev->dev.parent);
	u32 device_id = pdev->dev.parent->id;
	struct core_ops *ops = platform_get_drvdata(pdev);
	ethsw_api_dev_t *prvdata = container_of(ops, ethsw_api_dev_t, ops);
	/** Clear core_init */
	memset(&core_init, 0, sizeof(ethsw_core_init_t));


	/*Initialize global array*/
	gswdev->core_dev = pdev;

	memset(&core_init, 0, sizeof(ethsw_core_init_t));

	if (device_id < LTQ_FLOW_DEV_INT || device_id >= LTQ_FLOW_DEV_MAX)
		return -EINVAL;

	if (!prvdata->ext_devid) {

		/* Find and map our resources */
		memres = platform_get_resource(pdev, IORESOURCE_MEM, 0);

	if (memres == NULL) {
		pr_err("%s:%s:%d (Failed)\n", __FILE__, __func__, __LINE__);
		/*		dev_err(&pdev->dev, "Cannot get IORESOURCE_MEM\n");*/
		return -ENOENT;
	}

	/*Enable Switch Power  */
	clk = devm_clk_get(&pdev->dev, "gate");

	if (IS_ERR(clk))
		panic("Failed to get switch clock");

	clk_prepare_enable(clk);

	if (device_id == 0) {
		addr_gswl = devm_ioremap_resource(&pdev->dev, memres);

		if (IS_ERR(addr_gswl))
			return PTR_ERR(addr_gswl);
	}

	if (device_id == 1) {
		addr_gswr = devm_ioremap_resource(&pdev->dev, memres);

			if (IS_ERR(addr_gswr))
				return PTR_ERR(addr_gswr);
		}
	}

	/* Register Char Device */
	if (device_id == 0) {
		result = gsw_api_drv_register(GSW_API_MAJOR_NUMBER);

		if (result != 0) {
			pr_err("%s:%s:%d (Reg Char Device Failed)\n",
			       __FILE__, __func__, __LINE__);
			return result;
		}
	}

	result = gsw_cdev_interface(GSW_API_MAJOR_NUMBER, device_id, gswdev);
	if (result != 0) {
		pr_err("Failed to create cdev interface\n");
		return result;
	}

	if (device_id == LTQ_FLOW_DEV_INT) {
		/* Init FLOW Switch Core Layer */
		core_init.sdev = LTQ_FLOW_DEV_INT;
		core_init.gsw_base_addr = addr_gswl;
		core_init.pdev = (void *)pdev;

		pEDev0 = ethsw_api_core_init(&core_init);

		if (pEDev0 == NULL) {
			pr_err("%s:%s:%d (Init Failed)\n",
			       __FILE__, __func__, __LINE__);
			return -1;
		}

		pEDev0->cport = GSW_3X_SOC_CPU_PORT;
		pEDev0->gsw_dev = LTQ_FLOW_DEV_INT;
		pEDev0->gswl_base = addr_gswl;
		pEDev0->gsw_base = addr_gswl;
	}

	if (device_id == LTQ_FLOW_DEV_INT_R) {
		/* Init FLOW Switch Core Layer */
		core_init.sdev = LTQ_FLOW_DEV_INT_R;
		core_init.gsw_base_addr = addr_gswr;
		core_init.pdev = (void *)pdev;
		pEDev1 = ethsw_api_core_init(&core_init);

		if (pEDev1 == NULL) {
			pr_err("%s:%s:%d (Init Failed)\n",
			       __FILE__, __func__, __LINE__);
			return -1;
		}

		pEDev1->cport = GSW_3X_SOC_CPU_PORT;
		pEDev1->gsw_dev = LTQ_FLOW_DEV_INT_R;
		pEDev1->gswr_base = addr_gswr;
		pEDev1->gsw_base = addr_gswr;
	}

	if (device_id == LTQ_FLOW_DEV_EXT_AX3000_F24S) {

		prvdata->cport = GSW_2X_SOC_CPU_PORT;
		prvdata->gsw_dev = LTQ_FLOW_DEV_EXT_AX3000_F24S;
		prvdata->parent_devid = LTQ_FLOW_DEV_INT_R;

		/* Init External Switch Core Layer */
		core_init.sdev = LTQ_FLOW_DEV_EXT_AX3000_F24S;
		core_init.gsw_base_addr = NULL;
		core_init.pdev = (void *)prvdata;
		pEDevExt = ethsw_api_core_init(&core_init);

		if (pEDevExt == NULL) {
			pr_err("%s:%s:%d (Init Failed)\n",
			       __FILE__, __func__, __LINE__);
			return -1;
		}

	}


	if (device_id == 0) {
		ioct_cmd_start_node = gsw_create_ioctl_cmd_linklist(&pEDev0->ops);
		ioctlinit.pLlTable = ioct_cmd_start_node;
		ioctlinit.default_handler = NULL;
		pioctlctl = ioctl_wrapper_init(&ioctlinit);

		if (pioctlctl == NULL) {
			pr_err("%s:%s:%d (WrapperInit Failed)\n",
			       __FILE__, __func__, __LINE__);
			return -1;
		}
	}

	/* add Internal switch */
	if (device_id == LTQ_FLOW_DEV_INT && pioctlctl && pEDev0)
		ioctl_wrapper_dev_add(pioctlctl, &pEDev0->ops, LTQ_FLOW_DEV_INT);

	/* add Internal switch */
	if ((device_id == LTQ_FLOW_DEV_INT_R) && pioctlctl && pEDev1)
		ioctl_wrapper_dev_add(pioctlctl, &pEDev1->ops, LTQ_FLOW_DEV_INT_R);

	/* Init wrapper , if external switch attached to GSWIP-R*/
	if ((device_id == LTQ_FLOW_DEV_EXT_AX3000_F24S) && pioctlctl
						&& pEDevExt)
		ioctl_wrapper_dev_add(pioctlctl, &pEDevExt->ops,
				LTQ_FLOW_DEV_EXT_AX3000_F24S);

	return 0;
}
#endif /* CONFIG_SOC_GRX500 */

int ltq_ethsw_api_register(struct platform_device *pdev)
{
	int result;
	struct resource *res;
	ethsw_core_init_t core_init;
	ethsw_api_dev_t *pethdev;
	ioctl_wrapper_ctx_t *iowrap;
	ioctl_wrapper_init_t iowrapinit;
	struct clk *clk;

	/* Find and map our resources */
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);

	if (res == NULL) {
		pr_err("%s:%s:%d (Get IORESOURCE_MEM Failed)\n",
		       __FILE__, __func__, __LINE__);
		/*		dev_err(&pdev->dev, "Cannot get IORESOURCE_MEM\n");*/
		return -ENOENT;
	}

	addr_gsw = devm_ioremap_resource(&pdev->dev, res);

	if (IS_ERR(addr_gsw))
		return PTR_ERR(addr_gsw);

	/* Register Char Device */
	result = gsw_api_drv_register(GSW_API_MAJOR_NUMBER);

	if (result != 0) {
		pr_err("%s:%s:%d (Register Char Device Failed)\n",
		       __FILE__, __func__, __LINE__);
		return result;
	}

	/*Enable Switch Power  */
	clk = clk_get_sys("1e108000.eth", NULL);
	clk_enable(clk);

	/* Init FLOW Switch Core Layer */
	core_init.pdev = NULL;
	core_init.sdev = LTQ_FLOW_DEV_INT;
	core_init.gsw_base_addr = addr_gsw;
	/*	core_init.pDev = pRALDev; */
	pethdev = ethsw_api_core_init(&core_init);

	if (pethdev == NULL) {
		pr_err("%s:%s:%d (Init Failed)\n",
		       __FILE__, __func__, __LINE__);
		return -1;
	}

	pethdev->cport = GSW_2X_SOC_CPU_PORT;
	pethdev->gsw_base = addr_gsw;
	iowrapinit.pLlTable = &ltq_flow_fkt_tbl;
	iowrapinit.default_handler = NULL;
	iowrap = ioctl_wrapper_init(&iowrapinit);

	if (iowrap == NULL) {
		pr_err("%s:%s:%d (WrapperInit Failed)\n",
		       __FILE__, __func__, __LINE__);
		return -1;
	}

	/* add Internal switch */
	ioctl_wrapper_dev_add(iowrap, pethdev, LTQ_INT_GSWITCH);
	return 0;
}

int ltq_ethsw_api_unregister(void)
{
	/* Free the device data block */
	gsw_api_drv_unregister(GSW_API_MAJOR_NUMBER);
	gsw_corecleanup();
	gsw_delete_ioctl_cmd_linklist(ioct_cmd_start_node);
	gsw_api_ioctl_wrapper_cleanup();
	return 0;
}

/*ltq_ethsw_api_init   the init function, called when the module is loaded.*/
/*	Returns zero if successfully loaded, nonzero otherwise.*/
static int __init ltq_ethsw_api_init(struct platform_device *pdev)
{
	/* Print Version Number */
#ifdef CONFIG_SOC_GRX500
	/*	if (of_machine_is_compatible("lantiq,grx500")) { */
	ltq_gsw_api_register(pdev);
#endif
	/*	} else { */
#ifdef CONFIG_SOC_XWAY
	ltq_ethsw_api_register(pdev);
#endif

#ifdef CONFIG_X86_INTEL_CE2700
	ltq_ethsw_api_register_p7(pdev);
#endif
	/*	} */
	return 0;
}

/*ltq_etshw_api_exit  the exit function, called when the module is removed.*/
static void __exit ltq_etshw_api_exit(void)
{
	ltq_ethsw_api_unregister();
}

static int ltq_switch_api_probe(struct platform_device *pdev)
{
	ltq_ethsw_api_init(pdev);
	return 0;
}

static int ltq_switch_api_remove(struct platform_device *pdev)
{
	ltq_etshw_api_exit();
	return 0;
}

#if defined(CONFIG_SOC_GRX500) && CONFIG_SOC_GRX500

static void __iomem		*gswl_addr;
static void __iomem		*gswr_addr;
/** read and update the GSWIP register */
static void ltq_gsw_w32(short offset, short shift, short size, u32 value)
{
	u32 rvalue, mask;

	if (gswl_addr != 0) {
		rvalue = gsw1_r32(gswl_addr + (offset * 4));
		mask = (1 << size) - 1;
		mask = (mask << shift);
		value = ((value << shift) & mask);
		value = ((rvalue & ~mask) | value);
		/*		pr_info("writing %x to the address = %x \n", value, (u32) (gswl_addr + (offset * 4)));*/
		gsw1_w32(value, (gswl_addr + (offset * 4)));
	} else {
		pr_err("%s:%s:%d,(ERROR)\n", __FILE__, __func__, __LINE__);
	}
}

static void ltq_gsw_r_w32(short offset, short shift, short size, u32 value)
{
	u32 rvalue, mask;

	if (gswr_addr != 0) {
		rvalue = gsw1_r32(gswr_addr + (offset * 4));
		mask = (1 << size) - 1;
		mask = (mask << shift);
		value = ((value << shift) & mask);
		value = ((rvalue & ~mask) | value);
		/*		pr_info("writing %x to the address = %x \n", value, (u32) (gswr_addr + (offset * 4)));*/
		gsw1_w32(value, (gswr_addr + (offset * 4)));
	} else {
		pr_err("%s:%s:%d,(ERROR)\n", __FILE__, __func__, __LINE__);
	}
}
void gsw_api_disable_switch_ports(void)
{
	int pidx;
	gswl_addr = (void __iomem *)(KSEG1 | 0x1c000000);
	gswr_addr = (void __iomem *)(KSEG1 | 0x1a000000);

	for (pidx = 2; pidx < 6; pidx++) {
		/* Set SDMA_PCTRL_PEN PORT disable */
		ltq_gsw_w32((SDMA_PCTRL_PEN_OFFSET + (6 * pidx)),
			    SDMA_PCTRL_PEN_SHIFT, SDMA_PCTRL_PEN_SIZE, 0);
		/* Set FDMA_PCTRL_EN PORT disable */
		ltq_gsw_w32((FDMA_PCTRL_EN_OFFSET + (0x6 * pidx)),
			    FDMA_PCTRL_EN_SHIFT, FDMA_PCTRL_EN_SIZE, 0);
	}

	for (pidx = 0; pidx < 16; pidx++) {
		/* Set SDMA_PCTRL_PEN PORT disable */
		ltq_gsw_r_w32((SDMA_PCTRL_PEN_OFFSET + (6 * pidx)),
			      SDMA_PCTRL_PEN_SHIFT, SDMA_PCTRL_PEN_SIZE, 0);
		/* Set FDMA_PCTRL_EN PORT disable */
		ltq_gsw_r_w32((FDMA_PCTRL_EN_OFFSET + (0x6 * pidx)),
			      FDMA_PCTRL_EN_SHIFT, FDMA_PCTRL_EN_SIZE, 0);
	}
}
EXPORT_SYMBOL(gsw_api_disable_switch_ports);

static const struct of_device_id ltq_switch_api_match[] = {
	{ .compatible = "intel,prx300-gswapi" },
	{},
};

#endif /* CONFIG_SOC_GRX500  */

#ifdef CONFIG_SOC_XWAY

static const struct of_device_id ltq_switch_api_match[] = {
	{ .compatible = "lantiq,xway-gsw2x" },
	{},
};

#endif /* CONFIG_SOC_XWAY */

#ifdef CONFIG_X86_INTEL_CE2700

static const struct of_device_id ltq_switch_api_match[] = {
	{ .compatible = "lantiq,xway-gsw2x" },
	{},
};

#endif /* CONFIG_X86_INTEL_CE2700 */

MODULE_DEVICE_TABLE(of, ltq_switch_api_match);

static struct platform_driver ltq_switch_api = {
	.probe = ltq_switch_api_probe,
	.remove = ltq_switch_api_remove,
	.driver = {
		.name = "gsw_core",
		.of_match_table = ltq_switch_api_match,
		.owner = THIS_MODULE,
	},
};


#ifdef CONFIG_X86_INTEL_CE2700

static struct platform_device *ltq_switch_api_dev;

static int __init ltq_ethsw_api_init_p7(void)
{
	int ret;

	ret = platform_driver_register(&ltq_switch_api);

	if (ret < 0) {
		pr_err("%s:%s:%d switch_api driver register failed\n",
		       __FILE__, __func__, __LINE__);
		return ret;
	}

	ltq_switch_api_dev =
		platform_device_register_simple("xway-gsw2xapi", -1, NULL, 0);

	if (IS_ERR(ltq_switch_api_dev)) {
		pr_err("%s:%s:%d switch_api device register failed\n",
		       __FILE__, __func__, __LINE__);
		platform_driver_unregister(&ltq_switch_api);
		return PTR_ERR(ltq_switch_api_dev);
	}

	return 0;
}

static void __exit ltq_ethsw_api_exit_p7(void)
{
	platform_device_unregister(ltq_switch_api_dev);
	platform_driver_unregister(&ltq_switch_api);
}

module_init(ltq_ethsw_api_init_p7);
module_exit(ltq_ethsw_api_exit_p7);

#else

module_platform_driver(ltq_switch_api);

#endif /*CONFIG_X86_INTEL_CE2700*/

MODULE_AUTHOR("LANTIQ");
MODULE_DESCRIPTION("LTQ ETHSW API");
MODULE_LICENSE("GPL");
MODULE_VERSION(GSW_API_DRV_VERSION);

#endif

