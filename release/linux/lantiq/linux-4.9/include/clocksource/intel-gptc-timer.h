/*
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 *
 * Copyright (C) 2016 Intel Corporation.
 */

struct gptc_ht_yield {
	u32 yield_pin;
	u32 interval;
	u32 mips_tc_shared_ctxt_mem;
};

int gptc_ht_yield_init(struct gptc_ht_yield *param, void *call_back,
		       void *call_back_param);
int gptc_ht_yield_interval(u32 interval);
