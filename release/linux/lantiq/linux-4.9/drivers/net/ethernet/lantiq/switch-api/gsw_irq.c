/******************************************************************************
                Copyright (c) 2016, 2017 Intel Corporation

For licensing information, see the file 'LICENSE' in the root folder of
this software module.
******************************************************************************/

#include <gsw_init.h>

typedef void (*gsw_call_back)(void *param);

#define IS_PCE_IER_MATCH(p1, p2) (p1->pce_ier_mask==p2->pce_ier_mask)
#define IS_PCE_GIER_MATCH(p1, p2) (p1->pce_gier_mask==p2->pce_gier_mask)
#define IS_PCE_PIER_MATCH(p1, p2) (p1->pce_pier_mask==p2->pce_pier_mask)
#define INVALID -1

GSW_return_t  GSW_Debug_PrintPceIrqList(void *cdev)
{
	gsw_pce_irq *irq;
	u32 i = 0;
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

	irq = gswdev->PceIrqList->first_ptr;

	while (irq != NULL) {

		if (irq->pce_gier_mask == INVALID &&
		    irq->pce_ier_mask == INVALID &&
		    irq->pce_pier_mask == INVALID) {
			goto cont;
		}

		printk("PCE Node                 %d:\n", i);
		printk("PceIrqList address                 = 0x%p\n", irq);
		printk("PceIrqList Next Irq Address                 = 0x%p\n\n", irq->pNext);

		printk("pce_ier_mask                 = %u (port no)\n", irq->portId);

		switch (irq->pce_gier_mask) {
		case PCE_MAC_TABLE_CHANGE:
			printk("pce_gier_mask                 = PCE_MAC_TABLE_CHANGE\n");
			break;

		case PCE_FLOW_TABLE_RULE_MATCHED:
			printk("pce_gier_mask                 = PCE_FLOW_TABLE_RULE_MATCHED\n");
			break;

		case PCE_CLASSIFICATION_PHASE_2:
			printk("pce_gier_mask                 = PCE_CLASSIFICATION_PHASE_2\n");
			break;

		case PCE_CLASSIFICATION_PHASE_1:
			printk("pce_gier_mask                 = PCE_CLASSIFICATION_PHASE_1\n");
			break;

		case PCE_CLASSIFICATION_PHASE_0:
			printk("pce_gier_mask                 = PCE_CLASSIFICATION_PHASE_0\n");
			break;

		case PCE_PARSER_READY:
			printk("pce_gier_mask                 = PCE_PARSER_READY\n");
			break;

		case PCE_IGMP_TABLE_FULL:
			printk("pce_gier_mask                 = PCE_IGMP_TABLE_FULL\n");
			break;

		case PCE_MAC_TABLE_FULL:
			printk("pce_gier_mask                 = PCE_MAC_TABLE_FULL\n");
			break;
		}

		printk("pce_isr_mask                 = %u\n", irq->pce_isr_mask);
		printk("pce_gisr_mask                 = %u\n", irq->pce_gisr_mask);
		printk("pce_pisr_mask                 = %u\n", irq->pce_pisr_mask);
		printk("call_back                 = 0x%p\n", irq->call_back);
		printk("\n");
cont:
		irq = irq->pNext;
		i++;
	}

	return GSW_statusOk;
}

static GSW_return_t pce_irq_add(void *cdev, gsw_pce_irq *pce_irq,
				struct pce_irq_linklist *node)
{
	gsw_pce_irq *reg_irq = NULL;
	gsw_pce_irq *irq = NULL;

	irq = node->first_ptr;

	while (irq != NULL) {
		if (IS_PCE_IER_MATCH(pce_irq, irq) &&
		    IS_PCE_GIER_MATCH(pce_irq, irq)) {
			pr_debug("Event & Port already registered\n");
			return GSW_statusErr;
		}

		if (IS_PCE_IER_MATCH(pce_irq, irq) &&
		    IS_PCE_PIER_MATCH(pce_irq, irq)) {
			pr_debug("Event & PIER already registered\n");
			return GSW_statusErr;
		}

		irq = irq->pNext;
	}

#ifdef __KERNEL__
	reg_irq = (gsw_pce_irq *)kmalloc(sizeof(gsw_pce_irq), GFP_KERNEL);
	memset(reg_irq, 0, sizeof(gsw_pce_irq));
#else
	reg_irq = (gsw_pce_irq *)malloc(sizeof(gsw_pce_irq));
	memset(reg_irq, 0, sizeof(gsw_pce_irq));
#endif

	reg_irq->pNext			=	NULL;
	reg_irq->portId			= 	pce_irq->portId;
	reg_irq->pce_ier_mask		=	pce_irq->pce_ier_mask;
	reg_irq->pce_isr_mask		=	pce_irq->pce_isr_mask;
	reg_irq->pce_gier_mask		=	pce_irq->pce_gier_mask;
	reg_irq->pce_gisr_mask		=	pce_irq->pce_gisr_mask;
	reg_irq->pce_pier_mask		=	pce_irq->pce_pier_mask;
	reg_irq->pce_pisr_mask		=	pce_irq->pce_pisr_mask;
	reg_irq->call_back		=	pce_irq->call_back;
	reg_irq->param			=	pce_irq->param;

	if (node->first_ptr != NULL) {
		node->last_ptr->pNext = reg_irq;
		node->last_ptr = reg_irq;
	} else {
		node->first_ptr = reg_irq;
		node->last_ptr = reg_irq;
	}

	return GSW_statusOk;
}

static GSW_return_t pce_irq_del(void *cdev, gsw_pce_irq *pce_irq,
				struct pce_irq_linklist *node)
{
	gsw_pce_irq *prv_irq = NULL, *delete_irq = NULL;
	u32 p_ierq = 0, found = 0;

	delete_irq = node->first_ptr;

	while (delete_irq != NULL) {
		if (IS_PCE_IER_MATCH(pce_irq, delete_irq) &&
		    IS_PCE_GIER_MATCH(pce_irq, delete_irq)) {

			gsw_r32(cdev, PCE_IER_0_OFFSET,
				delete_irq->pce_ier_mask,
				1, &p_ierq);

			/*Check only Port IER*/
			if (p_ierq) {
				pr_err("ERROR : Can not Un-Register IRQ, disable IRQ first %s:%s:%d\n",
				       __FILE__, __func__, __LINE__);
				return -1;
			}

			found = 1;
		}

		if (IS_PCE_IER_MATCH(pce_irq, delete_irq) &&
		    IS_PCE_PIER_MATCH(pce_irq, delete_irq)) {

			gsw_r32(cdev, PCE_IER_0_OFFSET,
				delete_irq->pce_ier_mask,
				1, &p_ierq);

			/*Check only Port IER*/
			if (p_ierq) {
				pr_err("ERROR : Can not Un-Register IRQ, disable IRQ first %s:%s:%d\n",
				       __FILE__, __func__, __LINE__);
				return -1;
			}

			found = 1;
		}


		if (found) {
			if (node->first_ptr == delete_irq &&
			    node->last_ptr == delete_irq) {
				node->first_ptr = delete_irq->pNext;
				node->last_ptr = delete_irq->pNext;
			} else if (node->first_ptr == delete_irq) {
				node->first_ptr = delete_irq->pNext;
			} else if (node->last_ptr == delete_irq) {
				node->last_ptr = prv_irq;
				prv_irq->pNext = delete_irq->pNext;
			} else {
				prv_irq->pNext = delete_irq->pNext;
			}

#ifdef __KERNEL__
			kfree(delete_irq);
#else
			free(delete_irq);
#endif
		}

		prv_irq = delete_irq;
		delete_irq = delete_irq->pNext;
	}

	return GSW_statusOk;
}

static GSW_return_t pce_irq_enable(void *cdev, gsw_pce_irq *pce_irq,
				   struct pce_irq_linklist *node)
{
	gsw_pce_irq *irq = NULL;
	short pce_pier_offset = 0;

	if (!node)
		return GSW_statusErr;

	irq = node->first_ptr;

	while (irq != NULL) {

		if (IS_PCE_IER_MATCH(pce_irq, irq) &&
		    IS_PCE_GIER_MATCH(pce_irq, irq)) {

			gsw_w32(cdev, PCE_IER_0_OFFSET,
				irq->pce_ier_mask, 1, 1);

			gsw_w32(cdev, PCE_IER_1_OFFSET,
				irq->pce_gier_mask, 1, 1);
		}

		if (IS_PCE_IER_MATCH(pce_irq, irq) &&
		    IS_PCE_PIER_MATCH(pce_irq, irq)) {

			pce_pier_offset = PCE_PIER_OFFSET(pce_irq->portId);

			gsw_w32(cdev, PCE_IER_0_OFFSET,
				irq->pce_ier_mask, 1, 1);

			gsw_w32(cdev, pce_pier_offset,
				irq->pce_pier_mask, 1, 1);
		}

		irq = irq->pNext;
	}

	return GSW_statusOk;
}

static GSW_return_t pce_irq_disable(void *cdev, gsw_pce_irq *pce_irq,
				    struct pce_irq_linklist *node)
{
	gsw_pce_irq *irq = NULL;
	short pce_pier_offset = 0;

	irq = node->first_ptr;

	while (irq != NULL) {
		if (IS_PCE_IER_MATCH(pce_irq, irq) &&
		    IS_PCE_GIER_MATCH(pce_irq, irq)) {
			gsw_w32(cdev, PCE_IER_0_OFFSET, irq->pce_ier_mask, 1, 0);
			gsw_w32(cdev, PCE_IER_1_OFFSET, irq->pce_gier_mask, 1, 0);
		}

		irq = irq->pNext;
	}

	irq = node->first_ptr;

	while (irq != NULL) {
		if ((IS_PCE_IER_MATCH(pce_irq, irq)) &&
		    (IS_PCE_PIER_MATCH(pce_irq, irq))) {
			pce_pier_offset = PCE_PIER_OFFSET(pce_irq->portId);
			gsw_w32(cdev, pce_pier_offset, irq->pce_gier_mask, 1, 0);
		}

		irq = irq->pNext;
	}

	return GSW_statusOk;
}

static GSW_return_t pce_irq_config(void *cdev, GSW_Irq_Op_t *irq, IRQ_TYPE IrqType)
{
	gsw_pce_irq pce_irq = {0};
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

	if (irq->portid > gswdev->tpnum) {
		pr_err("ERROR : PortId %d is not with in GSWIP capabilty %s:%s:%d\n",
		       irq->portid, __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

	pce_irq.pce_gier_mask = INVALID;
	pce_irq.pce_ier_mask  = INVALID;
	pce_irq.pce_pier_mask = INVALID;
	pce_irq.pce_gisr_mask = INVALID;
	pce_irq.pce_isr_mask  = INVALID;
	pce_irq.pce_pisr_mask = INVALID;

	pce_irq.portId 		= irq->portid;
	pce_irq.pce_ier_mask 	= PCE_IER_0_PORT_MASK_GET(irq->portid);
	pce_irq.pce_isr_mask 	= PCE_ISR_0_PORT_MASK_GET(irq->portid);

	pce_irq.pce_ier_mask = pce_irq.pce_ier_mask >> 1;
	pce_irq.pce_isr_mask = pce_irq.pce_isr_mask >> 1;

	switch (irq->event) {
	case PCE_MAC_TABLE_CHANGE:
		pce_irq.pce_gier_mask = PCE_IER_1_CHG_SHIFT;
		pce_irq.pce_gisr_mask = PCE_ISR_1_CHG_SHIFT;
		break;

	case PCE_FLOW_TABLE_RULE_MATCHED:
		pce_irq.pce_gier_mask = PCE_IER_1_FLOWINT_SHIFT;
		pce_irq.pce_gisr_mask = PCE_ISR_1_FLOWINT_SHIFT;
		break;

	case PCE_CLASSIFICATION_PHASE_2:
		pce_irq.pce_gier_mask = PCE_IER_1_CPH2_SHIFT;
		pce_irq.pce_gisr_mask = PCE_ISR_1_CPH2_SHIFT;
		break;

	case PCE_CLASSIFICATION_PHASE_1:
		pce_irq.pce_gier_mask = PCE_IER_1_CPH1_SHIFT;
		pce_irq.pce_gisr_mask = PCE_ISR_1_CPH1_SHIFT;
		break;

	case PCE_CLASSIFICATION_PHASE_0:
		pce_irq.pce_gier_mask = PCE_IER_1_CPH0_SHIFT;
		pce_irq.pce_gisr_mask = PCE_ISR_1_CPH0_SHIFT;
		break;

	case PCE_PARSER_READY:
		pce_irq.pce_gier_mask = PCE_IER_1_PRDY_SHIFT;
		pce_irq.pce_gisr_mask = PCE_ISR_1_PRDY_SHIFT;
		break;

	case PCE_IGMP_TABLE_FULL:
		pce_irq.pce_gier_mask = PCE_IER_1_IGTF_SHIFT;
		pce_irq.pce_gisr_mask = PCE_ISR_1_IGTF_SHIFT;
		break;

	case PCE_MAC_TABLE_FULL:
		pce_irq.pce_gier_mask = PCE_IER_1_MTF_SHIFT;
		pce_irq.pce_gisr_mask = PCE_ISR_1_MTF_SHIFT;
		break;

	case PCE_METER_EVENT:
		pce_irq.pce_pier_mask = PCE_PIER_METER_SHIFT;
		pce_irq.pce_pisr_mask = PCE_PIER_METER_SHIFT;
		break;

	default:
		pr_err("ERROR : Invalid pce blk event ENUM = %d %s:%s:%d\n",
		       irq->event, __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

	switch (IrqType) {
	case IRQ_REGISTER:
		if (irq->call_back != NULL) {
			pce_irq.call_back	= irq->call_back;
			pce_irq.param		= irq->param;
		} else {
			pr_err("ERROR : callback handle is NULL %s:%s:%d\n",
			       __FILE__, __func__, __LINE__);
			return GSW_statusErr;
		}

		pce_irq_add(cdev, &pce_irq, gswdev->PceIrqList);
		break;

	case IRQ_UNREGISTER:
		pce_irq_del(cdev, &pce_irq, gswdev->PceIrqList);
		break;

	case IRQ_ENABLE:
		pce_irq_enable(cdev, &pce_irq, gswdev->PceIrqList);
		break;

	case IRQ_DISABLE:
		pce_irq_disable(cdev, &pce_irq, gswdev->PceIrqList);
		break;

	default:
		pr_err("invalid irq type %s:%s:%d\n",
		       __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

	return GSW_statusOk;
}

static GSW_return_t swcore_irq_config(void *cdev, GSW_Irq_Op_t *irq, IRQ_TYPE IrqType)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_irq);
#endif

	switch (irq->blk) {
	case BM:
		pr_err("BM BLK IRQ not supported %s:%s:%d\n",
		       __FILE__, __func__, __LINE__);
		break;

	case SDMA:
		pr_err("SDMA IRQ not supported %s:%s:%d\n",
		       __FILE__, __func__, __LINE__);
		break;

	case FDMA:
		pr_err("FDMA IRQ not supported %s:%s:%d\n",
		       __FILE__, __func__, __LINE__);
		break;

	case PMAC:
		pr_err("PMAC IRQ not supported %s:%s:%d\n",
		       __FILE__, __func__, __LINE__);
		break;

	case PCE:
		pce_irq_config(cdev, irq, IrqType);
		break;

	default:
		pr_err("invalid Switch IRQ Blk %s:%s:%d\n",
		       __FILE__, __func__, __LINE__);
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_irq);
#endif
	return ret;
}

void GSW_Irq_tasklet(unsigned long ops)
{
	gsw_pce_irq *pceirq = NULL;
	gsw_call_back callback = NULL;
	u32 p_isr = 0, e_isr = 0, e_pisr = 0;
	u32 pce_event = 0;
	short pce_pisr_offset = 0;
	void *cdev = (void *)ops;
	ethsw_api_dev_t *gswdev;

	if (!cdev)
		return;

	gswdev = GSW_PDATA_GET(cdev);

	gsw_r32(cdev, ETHSW_ISR_PCEINT_OFFSET,
		ETHSW_ISR_PCEINT_SHIFT, ETHSW_ISR_PCEINT_SIZE,
		&pce_event);

	if (pce_event) {

		/*PCE : Service registered IRQ list*/
		if (gswdev->PceIrqList)
			pceirq = gswdev->PceIrqList->first_ptr;

		while (pceirq != NULL) {
			callback = NULL;

			/* Check for Pending Interrupt */
			if (pceirq->pce_isr_mask == INVALID) {
				pceirq = pceirq->pNext;
				continue;
			}

			if (pceirq->pce_gisr_mask != INVALID) {
				gsw_r32(cdev, PCE_ISR_0_OFFSET,
					pceirq->pce_isr_mask,
					1, &p_isr);
				gsw_r32(cdev, PCE_ISR_1_OFFSET,
					pceirq->pce_gisr_mask,
					1, &e_isr);
			}

			if (pceirq->pce_pisr_mask != INVALID) {

				pce_pisr_offset = PCE_PISR_OFFSET(pceirq->portId);

				gsw_r32(cdev, PCE_ISR_0_OFFSET,
					pceirq->pce_isr_mask,
					1, &p_isr);
				gsw_r32(cdev, pce_pisr_offset,
					pceirq->pce_pisr_mask,
					1, &e_pisr);
			}

			if (e_isr) {
				gsw_w32(cdev, PCE_ISR_1_OFFSET,
					pceirq->pce_isr_mask, 1, 1);
				callback = pceirq->call_back;
			}

			if (e_pisr) {
				gsw_w32(cdev, pce_pisr_offset,
					pceirq->pce_pisr_mask, 1, 1);

				callback = pceirq->call_back;
			}

			/*Call back Service*/
			if (callback)
				callback(pceirq->param);

			pceirq = pceirq->pNext;
		}

		/*Enable PCE Global IER*/
		gsw_w32(cdev, ETHSW_IER_PCEIE_OFFSET,
			ETHSW_IER_PCEIE_SHIFT, ETHSW_IER_PCEIE_SIZE, 1);
	}
}

#ifdef __KERNEL__
static irqreturn_t GSW_ISR(int irq, void *cdev)
{
	struct core_ops *sw_ops;
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 dev, maxdev = 0, isr_event;

	if (gswdev->gipver == LTQ_GSWIP_3_0)
		maxdev = 2;
	else if (gswdev->gipver == LTQ_GSWIP_3_1)
		maxdev = 1;

	for (dev = 0; dev < maxdev; dev++) {
		sw_ops = NULL;
		gswdev = NULL;
		sw_ops = gsw_get_swcore_ops(dev);
		gsw_r32_raw(sw_ops, ETHSW_ISR_PCEINT_OFFSET, &isr_event);

		if (isr_event == 0)
			continue;

		if (sw_ops) {
			gswdev = GSW_PDATA_GET(sw_ops);

			if (isr_event & (1 << ETHSW_IER_PCEIE_SHIFT)) {
				gsw_w32(sw_ops, ETHSW_IER_PCEIE_OFFSET,
					ETHSW_IER_PCEIE_SHIFT, ETHSW_IER_PCEIE_SIZE, 0);
			} else if (isr_event & (1 << ETHSW_ISR_BMINT_SHIFT)) {
				gsw_w32(sw_ops, ETHSW_IER_PCEIE_OFFSET,
					ETHSW_ISR_BMINT_SHIFT, ETHSW_ISR_BMINT_SIZE, 0);
			} else if (isr_event & (1 << ETHSW_ISR_MACINT_SHIFT)) {
				gsw_w32(sw_ops, ETHSW_IER_PCEIE_OFFSET,
					ETHSW_ISR_MACINT_SHIFT, ETHSW_ISR_MACINT_SIZE, 0);
			} else if (isr_event & (1 << ETHSW_ISR_SDMAINT_SHIFT)) {
				gsw_w32(sw_ops, ETHSW_IER_PCEIE_OFFSET,
					ETHSW_ISR_SDMAINT_SHIFT, ETHSW_ISR_SDMAINT_SIZE, 0);
			} else if (isr_event & (1 << ETHSW_ISR_FDMAINT_SHIFT)) {
				gsw_w32(sw_ops, ETHSW_IER_PCEIE_OFFSET,
					ETHSW_ISR_FDMAINT_SHIFT, ETHSW_ISR_FDMAINT_SIZE, 0);
			}

			tasklet_schedule(&gswdev->gswip_tasklet);
		}
	}

	return IRQ_HANDLED;
}
#endif

GSW_return_t GSW_Irq_init(void *cdev)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

	/*Enable PCE Global IER*/
	gsw_w32(cdev, ETHSW_IER_PCEIE_OFFSET,
		ETHSW_IER_PCEIE_SHIFT, ETHSW_IER_PCEIE_SIZE, 1);

	/*GSWIP PCE BLK IRQ Pointer*/
#ifdef __KERNEL__
	gswdev->PceIrqList =
		(struct pce_irq_linklist *)kmalloc(sizeof(struct pce_irq_linklist), GFP_KERNEL);
	ret = request_irq(gswdev->irq_num, GSW_ISR, 0, "gswip", cdev);

	if (ret) {
		pr_err("Switch irq request error %s:%s:%d", __FILE__, __func__, __LINE__);
		return ret;
	}

	gswdev->PceIrqList->first_ptr = NULL;
	gswdev->PceIrqList->last_ptr = NULL;

	tasklet_init(&gswdev->gswip_tasklet,
		     GSW_Irq_tasklet,
		     (unsigned long)cdev);
#else
	gswdev->PceIrqList =
		(struct pce_irq_linklist *)malloc(sizeof(struct pce_irq_linklist));

	gswdev->PceIrqList->first_ptr = NULL;
	gswdev->PceIrqList->last_ptr = NULL;

#endif

	return GSW_statusOk;
}

GSW_return_t GSW_Irq_deinit(void *cdev)
{
	gsw_pce_irq *irq = NULL;
	gsw_pce_irq *free_irq = NULL;
	ethsw_api_dev_t *gswdev = (ethsw_api_dev_t *)cdev;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

	/*Free PCE Irq list*/
	if (gswdev->PceIrqList) {
		irq = gswdev->PceIrqList->first_ptr;

		while (irq != NULL) {
			free_irq = irq;
			irq = irq->pNext;
#ifdef __KERNEL__
			kfree(free_irq);
#else
			free(free_irq);
#endif
		}

		/*Free PCE Irq blk*/
#ifdef __KERNEL__
		kfree(gswdev->PceIrqList);
#else
		free(gswdev->PceIrqList);
#endif
	}

	return GSW_statusOk;
}

GSW_return_t GSW_Irq_register(void *cdev, GSW_Irq_Op_t *irq)
{
	return swcore_irq_config(cdev, irq, IRQ_REGISTER);
}

GSW_return_t GSW_Irq_unregister(void *cdev, GSW_Irq_Op_t *irq)
{
	return swcore_irq_config(cdev, irq, IRQ_UNREGISTER);
}

GSW_return_t GSW_Irq_enable(void *cdev, GSW_Irq_Op_t *irq)
{
	return swcore_irq_config(cdev, irq, IRQ_ENABLE);
}

GSW_return_t GSW_Irq_disable(void *cdev, GSW_Irq_Op_t *irq)
{
	return swcore_irq_config(cdev, irq, IRQ_DISABLE);
}
