/* Copyright (c) 2022, Qualcomm Innovation Center, Inc. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include "legacyirq_internal.h"

static inline void update_latency_stat(struct legacy2virtual_irqdata *lvirq,
				       int irq, unsigned long delay)
{
	if (delay >= INTERRUPT_TIME_0 && delay <= INTERRUPT_TIME_1)
		lvirq->schedlatency[irq][BUCKET_B0]++;
	else if (delay > INTERRUPT_TIME_1 && delay <= INTERRUPT_TIME_2)
		lvirq->schedlatency[irq][BUCKET_B1]++;
	else if (delay > INTERRUPT_TIME_2 && delay <= INTERRUPT_TIME_3)
		lvirq->schedlatency[irq][BUCKET_B2]++;
	else if (delay > INTERRUPT_TIME_3 && delay <= INTERRUPT_TIME_4)
		lvirq->schedlatency[irq][BUCKET_B3]++;
	else if (delay > INTERRUPT_TIME_4 && delay <= INTERRUPT_TIME_5)
		lvirq->schedlatency[irq][BUCKET_B4]++;
	else if (delay > INTERRUPT_TIME_5 && delay <= INTERRUPT_TIME_6)
		lvirq->schedlatency[irq][BUCKET_B5]++;
	else if (delay > INTERRUPT_TIME_6 && delay <= INTERRUPT_TIME_7)
		lvirq->schedlatency[irq][BUCKET_B6]++;
	else
		lvirq->schedlatency[irq][BUCKET_B7]++;
}

static inline void update_execution_time(struct legacy2virtual_irqdata *lvirq,
					 int irq, unsigned long time)
{
	if (time >= INTERRUPT_TIME_0 && time <= INTERRUPT_TIME_1)
		lvirq->executiontime[irq][BUCKET_B0]++;
	else if (time > INTERRUPT_TIME_1 && time <= INTERRUPT_TIME_2)
		lvirq->executiontime[irq][BUCKET_B1]++;
	else if (time > INTERRUPT_TIME_2 && time <= INTERRUPT_TIME_3)
		lvirq->executiontime[irq][BUCKET_B2]++;
	else if (time > INTERRUPT_TIME_3 && time <= INTERRUPT_TIME_4)
		lvirq->executiontime[irq][BUCKET_B3]++;
	else if (time > INTERRUPT_TIME_4 && time <= INTERRUPT_TIME_5)
		lvirq->executiontime[irq][BUCKET_B4]++;
	else if (time > INTERRUPT_TIME_5 && time <= INTERRUPT_TIME_6)
		lvirq->executiontime[irq][BUCKET_B5]++;
	else if (time > INTERRUPT_TIME_6 && time <= INTERRUPT_TIME_7)
		lvirq->executiontime[irq][BUCKET_B6]++;
	else
		lvirq->executiontime[irq][BUCKET_B7]++;
}

void cnss_pci_enable_legacy_intx(void __iomem *bar,
		struct pci_dev *pci_dev)
{
	writel(QCN9224_ENABLE_LEGACY_INTTERUPT_BIT,
	       bar + QCN9224_LEGACY_INTX_COMMON);
	writel(PCIE_LOCALREG_LEGACY_INTX_EN_BNK0_VAL,
	       bar + PCIE_LOCALREG_LEGACY_INTX_EN_BNK0);
	writel(PCIE_LOCALREG_LEGACY_INTX_EN_BNK1_VAL,
	       bar + PCIE_LOCALREG_LEGACY_INTX_EN_BNK1);
	writel(PCIE_LOCALREG_LEGACY_INTX_EN_BNK2_VAL,
	       bar + PCIE_LOCALREG_LEGACY_INTX_EN_BNK2);
	writel(PCIE_LOCALREG_LEGACY_INTX_EN_BNK3_VAL,
	       bar + PCIE_LOCALREG_LEGACY_INTX_EN_BNK3);
	writel(PCIE_LOCALREG_LEGACY_INTX_EN_BNK4_VAL,
	       bar + PCIE_LOCALREG_LEGACY_INTX_EN_BNK4);
	pci_intx(pci_dev, 1);

}
EXPORT_SYMBOL(cnss_pci_enable_legacy_intx);

void set_lvirq_bar(void *lvirqptr, void *bar)
{
	struct legacy2virtual_irqdata *lvirq =
		(struct legacy2virtual_irqdata *)lvirqptr;
	lvirq->regbase = bar;
}
EXPORT_SYMBOL(set_lvirq_bar);

void clear_lvirq_bar(void *lvirqptr)
{
	struct legacy2virtual_irqdata *lvirq =
		(struct legacy2virtual_irqdata *)lvirqptr;
	lvirq->regbase = NULL;
}
EXPORT_SYMBOL(clear_lvirq_bar);

static inline int is_irq_set_in(int *isr, int bit)
{
	return (isr[bit/32] & BIT(bit%32));
}

/* legacy intx handler */
static irqreturn_t qcn9224_legacy_hw_irq_handler(int irqnum, void *data)
{
	int isr[5] = {0};
	int p, b;
	int irq_pin;
	int irq;
	unsigned long irqtime = jiffies;
	unsigned long dt; //deliveredtime
	unsigned long et; //executiontime
	unsigned long ct; //completedtime
	unsigned long sclt; //schedlatencytime
	struct legacy2virtual_irqdata *lvirq = (struct legacy2virtual_irqdata *)data;
	struct irq_desc *desc;

	//disable global interrupt
	iowrite32(0x0, BAR + INTX_COMMON_REG_OFFSET);

	isr[0] = ioread32(BAR + INTX_INT_STS0_REG_OFFSET);
	if (isr[0] & 0x2)
		isr[1] = ioread32(BAR + INTX_INT_STS1_REG_OFFSET);
	if (isr[0] & 0x4)
		isr[2] = ioread32(BAR + INTX_INT_STS2_REG_OFFSET);
	if (isr[0] & 0x8)
		isr[3] = ioread32(BAR + INTX_INT_STS3_REG_OFFSET);
	if (isr[0] & 0x16)
		isr[4] = ioread32(BAR + INTX_INT_STS4_REG_OFFSET);

	for (p = 0; p < INTX_MAX_LEVEL; p++) {
		for (b = 0; (b < INTX_MAX_INTERRUPTS_PER_LEVEL) && intx_prioritymap[p][b]; b++) {
			irq = intx_prioritymap[p][b];
			if ((is_irq_set_in(isr, irq))
			    || (test_bit(irq, lvirq->irq_enabled)
			    && test_bit(irq, lvirq->irq_pending))) {
				if (test_bit(irq, lvirq->irq_enabled)) {
					if (test_bit(irq, lvirq->irq_pending)) {
						clear_bit(irq, lvirq->irq_pending);
						STAT_INC(pendingirqdelivered, irq);
					}
					irq_pin = irq_find_mapping(lvirq->domain, irq);
					if (irq_pin > 0) {
						STAT_INC(raisedirq, irq);
						dt = jiffies;
						desc = irq_to_desc(irq_pin);
						if (likely(desc)) {
							handle_simple_irq(desc);
						}
						ct = jiffies;
						et = ct - dt;
						sclt = dt - irqtime;
						et =  jiffies_to_msecs(et);
						sclt =  jiffies_to_msecs(sclt);
						update_latency_stat(lvirq, irq, sclt);
						update_execution_time(lvirq, irq, et);
					} else {
						STAT_INC(unregisteredirq, irq);
					}
				} else {
					if (test_bit(irq, lvirq->irq_requested)) {
						set_bit(irq, lvirq->irq_pending);
						STAT_INC(pendingirq, irq);
					} else {
						STAT_INC(unregisteredirq, irq);
					}
				}

			}
		}
	}
	/* Enable Global Interrupt */
	iowrite32(1, BAR + INTX_COMMON_REG_OFFSET);
	return IRQ_HANDLED;
}

static void qcn9224_legacy_unmask_irq(struct irq_data *irqd)
{
	return;
}
static void qcn9224_legacy_mask_irq(struct irq_data *irqd)
{
	return;
}

static void qcn9224_legacy_enable_irq(struct irq_data *irqd)
{
	struct legacy2virtual_irqdata *lvirq = irq_data_get_irq_chip_data(irqd);
	irq_hw_number_t irq = irqd_to_hwirq(irqd);
	STAT_INC(enableirq, irq);
	set_bit(irq, lvirq->irq_enabled);
	iowrite32(IRQ_BIT_IN_REG(irq),
		  BAR + LEGACY_INTX_EN_SET_BANK0 + IRQ_TO_REG_OFFSET(irq));
}

static void qcn9224_legacy_disable_irq(struct irq_data *irqd)
{
	struct legacy2virtual_irqdata *lvirq = irq_data_get_irq_chip_data(irqd);
	irq_hw_number_t irq = irqd_to_hwirq(irqd);
	STAT_INC(disableirq, irq);
	iowrite32(IRQ_BIT_IN_REG(irq),
		  BAR + LEGACY_INTX_EN_CLR_BANK0 + IRQ_TO_REG_OFFSET(irq));
	clear_bit(irq, lvirq->irq_enabled);
}

static int qcn9224_legacy_set_irq_type(struct irq_data *irqd, unsigned int type)
{
	return 0;
}

static struct irq_chip qcn9224_legacy_irq_chip = {
	.name           = "QCNVIC",
	.irq_mask       = qcn9224_legacy_mask_irq,
	.irq_unmask     = qcn9224_legacy_unmask_irq,
	.irq_set_type	= qcn9224_legacy_set_irq_type,
	.irq_enable     = qcn9224_legacy_enable_irq,
	.irq_disable    = qcn9224_legacy_disable_irq,
};

static int qcn9224_irq_map(struct irq_domain *d,
			 unsigned int irq,
			 irq_hw_number_t hw)
{
	struct legacy2virtual_irqdata *lvirq = d->host_data;
	irq_set_chip_and_handler(irq, &qcn9224_legacy_irq_chip,
				 handle_level_irq);
	irq_set_chip_data(irq, lvirq);
	irq_set_noprobe(irq);
	STAT_INC(registeredirq, hw);
	set_bit(hw, lvirq->irq_requested);
	return 0;
}

static const struct irq_domain_ops qcn9224_irq_ops = {
	.map = qcn9224_irq_map,
	.xlate = irq_domain_xlate_twocell,
};

static const struct of_device_id qcom_qcn9224_of_match[] = {
	{ .compatible = "qcom,qcn9224_legacy_irq" },
	{}
};
MODULE_DEVICE_TABLE(of, qcom_qcn9224_of_match);

static ssize_t cnss_statdebug_debug_write(struct file *fp,
					 const char __user *user_buf,
					 size_t count, loff_t *off)
{
	struct legacy2virtual_irqdata *lvirq =
		((struct seq_file *)fp->private_data)->private;
	char buf[64];
	char *cmd;
	unsigned int len = 0;
	int option = 0;
	int i;

	if (!lvirq) {
		pr_emerg("lvirq is not intialized\n");
		return count;
	}

	len = min(count, sizeof(buf) - 1);
	if (copy_from_user(buf, user_buf, len))
		return -EFAULT;

	buf[len] = '\0';
	cmd = (char *)buf;
	if (strstr(cmd, "enable")) {
		option = enableirq;
	} else if (strstr(cmd, "disable")) {
		option = disableirq;
	} else if (strstr(cmd, "unregistered")) {
		option = unregisteredirq;
	} else if (strstr(cmd, "registered")) {
		option = registeredirq;
	} else if (strstr(cmd, "raised")) {
		option = raisedirq;
	} else if (strstr(cmd, "pendingirqdelivered")) {
		option = pendingirqdelivered;
	} else if (strstr(cmd, "pendingirq")) {
		option = pendingirq;
	} else if (strstr(cmd, "executiontime")) {
		option = executiontime;
	} else if (strstr(cmd, "schedlatency")) {
		option = schedlatency;
	} else {
		option = maxirq;
	}
	for (i = 0; i < INTX_MAX_INTERRUPTS; i++) {
		switch (option) {
		case enableirq:
			if(lvirq->stats[enableirq][i]) {
				printk("E %4d D %4d RAI %4d, REG %4d UNREG %4d P %4d PD %4d %30s[%d]\n",
					lvirq->stats[enableirq][i],
					lvirq->stats[disableirq][i],
					lvirq->stats[raisedirq][i],
					lvirq->stats[registeredirq][i],
					lvirq->stats[unregisteredirq][i],
					lvirq->stats[pendingirq][i],
					lvirq->stats[pendingirqdelivered][i],
					intx_irqname[i], i);
			}
		break;
		case disableirq:
			if(lvirq->stats[disableirq][i]) {
				printk("E %4d D %4d RAI %4d, REG %4d UNREG %4d P %4d PD %4d %30s[%d]\n",
					lvirq->stats[enableirq][i],
					lvirq->stats[disableirq][i],
					lvirq->stats[raisedirq][i],
					lvirq->stats[registeredirq][i],
					lvirq->stats[unregisteredirq][i],
					lvirq->stats[pendingirq][i],
					lvirq->stats[pendingirqdelivered][i],
					intx_irqname[i], i);
			}
		break;
		case registeredirq:
			if(lvirq->stats[registeredirq][i]) {
				printk("E %4d D %4d RAI %4d, REG %4d UNREG %4d P %4d PD %4d %30s[%d]\n",
					lvirq->stats[enableirq][i],
					lvirq->stats[disableirq][i],
					lvirq->stats[raisedirq][i],
					lvirq->stats[registeredirq][i],
					lvirq->stats[unregisteredirq][i],
					lvirq->stats[pendingirq][i],
					lvirq->stats[pendingirqdelivered][i],
					intx_irqname[i], i);
			}
		break;
		case unregisteredirq:
			if(lvirq->stats[unregisteredirq][i]) {
				printk("E %4d D %4d RAI %4d, REG %4d UNREG %4d P %4d PD %4d %30s[%d]\n",
					lvirq->stats[enableirq][i],
					lvirq->stats[disableirq][i],
					lvirq->stats[raisedirq][i],
					lvirq->stats[registeredirq][i],
					lvirq->stats[unregisteredirq][i],
					lvirq->stats[pendingirq][i],
					lvirq->stats[pendingirqdelivered][i],
					intx_irqname[i], i);
			}
		break;
		case raisedirq:
			if(lvirq->stats[raisedirq][i]) {
				printk("E %4d D %4d RAI %4d, REG %4d UNREG %4d P %4d PD %4d %30s[%d]\n",
					lvirq->stats[enableirq][i],
					lvirq->stats[disableirq][i],
					lvirq->stats[raisedirq][i],
					lvirq->stats[registeredirq][i],
					lvirq->stats[unregisteredirq][i],
					lvirq->stats[pendingirq][i],
					lvirq->stats[pendingirqdelivered][i],
					intx_irqname[i], i);
			}
		break;
		case pendingirq:
			if(lvirq->stats[pendingirq][i]) {
				printk("E %4d D %4d RAI %4d, REG %4d UNREG %4d P %4d PD %4d %30s[%d]\n",
					lvirq->stats[enableirq][i],
					lvirq->stats[disableirq][i],
					lvirq->stats[raisedirq][i],
					lvirq->stats[registeredirq][i],
					lvirq->stats[unregisteredirq][i],
					lvirq->stats[pendingirq][i],
					lvirq->stats[pendingirqdelivered][i],
					intx_irqname[i], i);
			}
		break;
		case pendingirqdelivered:
			if(lvirq->stats[pendingirqdelivered][i]) {
				printk("E %4d D %4d RAI %4d, REG %4d UNREG %4d P %4d PD %4d %30s[%d]\n",
					lvirq->stats[enableirq][i],
					lvirq->stats[disableirq][i],
					lvirq->stats[raisedirq][i],
					lvirq->stats[registeredirq][i],
					lvirq->stats[unregisteredirq][i],
					lvirq->stats[pendingirq][i],
					lvirq->stats[pendingirqdelivered][i],
					intx_irqname[i], i);
			}
		break;
		case maxirq:
			printk("E %4d D %4d RAI %4d, REG %4d UNREG %4d P %4d PD %4d %30s[%d]\n",
				lvirq->stats[enableirq][i],
				lvirq->stats[disableirq][i],
				lvirq->stats[raisedirq][i],
				lvirq->stats[registeredirq][i],
				lvirq->stats[unregisteredirq][i],
				lvirq->stats[pendingirq][i],
				lvirq->stats[pendingirqdelivered][i],
				intx_irqname[i], i);
		break;
		case executiontime:
			printk("%5d %5d %5d %5d %5d %5d %5d %5d %s [%d]",
				lvirq->executiontime[i][BUCKET_B0],
				lvirq->executiontime[i][BUCKET_B1],
				lvirq->executiontime[i][BUCKET_B2],
				lvirq->executiontime[i][BUCKET_B3],
				lvirq->executiontime[i][BUCKET_B4],
				lvirq->executiontime[i][BUCKET_B5],
				lvirq->executiontime[i][BUCKET_B6],
				lvirq->executiontime[i][BUCKET_B7],
				intx_irqname[i], i);
		break;
		case schedlatency:
			printk("%5d %5d %5d %5d %5d %5d %5d %5d %s [%d]",
				lvirq->schedlatency[i][BUCKET_B0],
				lvirq->schedlatency[i][BUCKET_B1],
				lvirq->schedlatency[i][BUCKET_B2],
				lvirq->schedlatency[i][BUCKET_B3],
				lvirq->schedlatency[i][BUCKET_B4],
				lvirq->schedlatency[i][BUCKET_B5],
				lvirq->schedlatency[i][BUCKET_B6],
				lvirq->schedlatency[i][BUCKET_B7],
				intx_irqname[i], i);
		break;
		}

	}

	return count;
}

static int cnss_statdebug_debug_show(struct seq_file *s, void *data)
{
	seq_puts(s, "echo enable > statdebug\n");
	seq_puts(s, "echo disable > statdebug\n");
	seq_puts(s, "echo registered > statdebug\n");
	seq_puts(s, "echo unregistered > statdebug\n");
	seq_puts(s, "echo raised > statdebug\n");
	seq_puts(s, "echo pendingirq > statdebug\n");
	seq_puts(s, "echo pendingirqdelivered > statdebug\n");
	seq_puts(s, "echo executiontime > statdebug\n");
	seq_puts(s, "echo schedlatency > statdebug\n");
	seq_puts(s, "echo all > statdebug\n");

	return 0;
}

static int cnss_statdebug_debug_open(struct inode *inode, struct file *file)
{
	return single_open(file, cnss_statdebug_debug_show, inode->i_private);
}

static const struct file_operations cnss_statdebug_debug_fops = {
	.read		= seq_read,
	.write		= cnss_statdebug_debug_write,
	.release	= single_release,
	.open		= cnss_statdebug_debug_open,
	.owner		= THIS_MODULE,
	.llseek		= seq_lseek,
};

static int qcom_qcn9224_probe(struct platform_device *pdev)
{
	int ret;
	struct device_node *n, *irqnode = NULL;
	u32 node_id = 0;
	struct legacy2virtual_irqdata *lvirq;
	char name[10];

	lvirq = kzalloc(sizeof(struct legacy2virtual_irqdata), GFP_KERNEL);
	if (!lvirq) {
		pr_err("lvirq allocation failed\n");
		return -ENOMEM;
	}
	lvirq->pdev = pdev;
	lvirq_list[lvirq_index++] = lvirq;

	if(of_property_read_u32(pdev->dev.of_node,
                          "qrtr_node_id", &node_id)) {
	}
	lvirq->qrtr_node_id = node_id;

	snprintf(name, sizeof(name), "qcnvic%d", node_id - QCN9224_0);
	irq_root_dentry = debugfs_create_dir(name, 0);
	if (IS_ERR(irq_root_dentry)) {
		ret = PTR_ERR(irq_root_dentry);
	}

	debugfs_create_file("statdebug", 0600, irq_root_dentry, lvirq,
			    &cnss_statdebug_debug_fops);

	for_each_available_child_of_node(pdev->dev.of_node, n) {
		if (of_property_read_bool(n, "interrupt-controller")) {
			irqnode = n;
			break;
		}
	}

	lvirq->domain = irq_domain_add_linear(irqnode, INTX_MAX_INTERRUPTS,
					      &qcn9224_irq_ops, lvirq);
	if (!lvirq->domain) {
		pr_err("failed to add irq_domain\n");
		return -ENOMEM;
	}

	platform_set_drvdata(pdev, lvirq);
	return 0;
}

void *cnss_get_lvirq_by_qrtr_id(int qrtr_node_id)
{
	int i;
	for (i = 0; i < lvirq_index; i++) {
		if (qrtr_node_id == lvirq_list[i]->qrtr_node_id)
			return lvirq_list[i];
	}
	return NULL;
}
EXPORT_SYMBOL(cnss_get_lvirq_by_qrtr_id);

int qcn9224_register_legacy_irq(void *lvirqptr, int irq)
{
	int ret = 0;
	struct legacy2virtual_irqdata *lvirq = (struct legacy2virtual_irqdata *)lvirqptr;
	lvirq->pci_legacy_irq = irq;
	ret = request_irq(irq, qcn9224_legacy_hw_irq_handler,
				   IRQF_ONESHOT, "qcn9224_hwirq", lvirq);
	if (ret < 0) {
		pr_err("PCI wifi IRQ request failed \n");
		return -ENOENT;
	}
	return ret;

}
EXPORT_SYMBOL(qcn9224_register_legacy_irq);

int qcn9224_unregister_legacy_irq(void *lvirqptr, int irq)
{
	struct legacy2virtual_irqdata *lvirq = (struct legacy2virtual_irqdata *)lvirqptr;
	if (lvirq->pci_legacy_irq) {
		disable_irq(lvirq->pci_legacy_irq);
		free_irq(lvirq->pci_legacy_irq, lvirq);
		lvirq->pci_legacy_irq = 0;
	}
	return 0;
}
EXPORT_SYMBOL(qcn9224_unregister_legacy_irq);

static int qcom_qcn9224_remove(struct platform_device *pdev)
{
	struct legacy2virtual_irqdata *lvirq = platform_get_drvdata(pdev);
	if (!lvirq)
		return -EINVAL;
	disable_irq(lvirq->pci_legacy_irq);
	free_irq(lvirq->pci_legacy_irq, lvirq);
	irq_domain_remove(lvirq->domain);
	debugfs_remove_recursive(irq_root_dentry);
	kfree(lvirq);
	lvirq = NULL;
	return 0;
}

static struct platform_driver qcom_qcn9224_driver = {
	.probe = qcom_qcn9224_probe,
	.remove = qcom_qcn9224_remove,
	.driver  = {
		.name  = "qcom,qcn9224_legacy_irq",
		.of_match_table = qcom_qcn9224_of_match,
	},
};
module_platform_driver(qcom_qcn9224_driver);

MODULE_DESCRIPTION("QCN9224 IRQ driver");
MODULE_LICENSE("GPL v2");
