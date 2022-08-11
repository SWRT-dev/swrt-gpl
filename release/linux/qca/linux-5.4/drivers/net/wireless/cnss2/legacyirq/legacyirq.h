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

#ifndef __LEGACY_IRQ_H
#define __LEGACY_IRQ_H

void *cnss_get_lvirq_by_qrtr_id(int qrtr_node_id);
void set_lvirq_bar(void *lvirq, void *bar);
void clear_lvirq_bar(void *lvirq);
int qcn9224_unregister_legacy_irq(void *lvirq, int irq);
int qcn9224_register_legacy_irq(void *lvirq, int irq);
void cnss_pci_enable_legacy_intx(void __iomem *bar,
				 struct pci_dev *pci_dev);

#endif /* __LEGACY_IRQ_H */
