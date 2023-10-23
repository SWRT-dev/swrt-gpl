// SPDX-License-Identifier: (BSD-3-Clause OR GPL-2.0-only)
/*
 * Driver for MACsec Engine inside Intel CHD GSWIP or GPHY
 *
 * Copyright 2019 - 2020 Intel Corporation
 */

#ifndef _E160_H_
#define _E160_H_

#include <linux/netdevice.h>
#include <net/macsec.h>

typedef int (*macsec_irq_handler_t)(struct macsec_context *);

struct e160_metadata {
	/* section to be provided for e160_register/e160_unregister */
	struct macsec_context ctx;
	int (*prepare)(struct macsec_context *ctx, macsec_irq_handler_t ig,
		       macsec_irq_handler_t eg);
	void (*unprepare)(struct macsec_context *ctx);
	int (*ig_irq_ena)(struct macsec_context *ctx);
	int (*ig_irq_dis)(struct macsec_context *ctx);
	int (*ig_reg_rd)(struct macsec_context *ctx, u32 off, u32 *pdata);
	int (*ig_reg_wr)(struct macsec_context *ctx, u32 off, u32 data);
	int (*eg_irq_ena)(struct macsec_context *ctx);
	int (*eg_irq_dis)(struct macsec_context *ctx);
	int (*eg_reg_rd)(struct macsec_context *ctx, u32 off, u32 *pdata);
	int (*eg_reg_wr)(struct macsec_context *ctx, u32 off, u32 data);

	/*
	 * The number of SA match sets, flow control registers and SAs
	 * (transform record) supported. The value "0" corresponds to 256 SAs.
	 */
	u32 ig_nr_of_sas;
	u32 eg_nr_of_sas;
};

extern int e160_register(const struct e160_metadata *pdev);
extern void e160_unregister(const struct e160_metadata *pdev);

#endif
