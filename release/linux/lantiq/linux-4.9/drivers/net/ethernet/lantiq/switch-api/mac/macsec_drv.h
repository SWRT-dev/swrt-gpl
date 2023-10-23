// SPDX-License-Identifier: GPL-2.0
/*
 * Driver for MACsec Engine inside Intel CHD GSWIP
 *
 * Copyright 2019 - 2020 Intel Corporation
 */

#ifndef _MACSEC_DRV_H_
#define _MACSEC_DRV_H_

struct macsec_prv_data {
	u32 devid;
	void __iomem *eip160_base;
	int irq_num;
};

#endif

