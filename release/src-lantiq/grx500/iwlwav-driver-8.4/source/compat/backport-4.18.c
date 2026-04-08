/*
 * Copyright (C) 2018 Intel Corporation
 */

#include <linux/hrtimer.h>

#if LINUX_VERSION_IS_GEQ(4,0,0)
time64_t ktime_get_boottime_seconds(void)
{
	return ktime_divns(ktime_get_boottime(), NSEC_PER_SEC);
}
EXPORT_SYMBOL_GPL(ktime_get_boottime_seconds);
#endif
