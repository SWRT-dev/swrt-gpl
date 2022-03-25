/*
 * Copyright 2010-2018 MediaTek, Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * Version 2 as published by the Free Software Foundation.
 */

extern int spm_mtcmos_cpu1_off(void);

/*
 * 1. for non-CPU MTCMOS: ETH, HIF0(PCIE), HIF1(ssusb)
 * 2. call spm_mtcmos_noncpu_lock/unlock() before/after any operations
 */
extern int spm_mtcmos_ctrl_eth(int state);
extern int spm_mtcmos_ctrl_hif0(int state);
extern int spm_mtcmos_ctrl_hif1(int state);
