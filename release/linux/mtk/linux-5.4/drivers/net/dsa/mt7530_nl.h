/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (c) 2018 MediaTek Inc.
 * Author: Sirui Zhao <Sirui.Zhao@mediatek.com>
 */

#ifndef _MT753x_NL_H_
#define _MT753x_NL_H_

#define MT7530_DSA_GENL_NAME "mt753x_dsa"
#define MT7530_GENL_VERSION		0x1

enum mt7530_cmd {
	MT7530_CMD_UNSPEC = 0,
	MT7530_CMD_REQUEST,
	MT7530_CMD_REPLY,
	MT7530_CMD_READ,
	MT7530_CMD_WRITE,

	__MT7530_CMD_MAX,
};

enum mt7530_attr {
	MT7530_ATTR_TYPE_UNSPEC = 0,
	MT7530_ATTR_TYPE_MESG,
	MT7530_ATTR_TYPE_PHY,
	MT7530_ATTR_TYPE_DEVAD,
	MT7530_ATTR_TYPE_REG,
	MT7530_ATTR_TYPE_VAL,
	MT7530_ATTR_TYPE_DEV_NAME,
	MT7530_ATTR_TYPE_DEV_ID,

	__MT7530_ATTR_TYPE_MAX,
};

#define MT7530_NR_ATTR_TYPE		(__MT7530_ATTR_TYPE_MAX - 1)

struct mt7530_info {
	struct mii_bus	*bus;
	void __iomem *base;
	int direct_access;
};

#ifdef __KERNEL__
int  mt7530_nl_init(struct mt7530_priv **priv);
void mt7530_nl_exit(void);
#endif /* __KERNEL__ */

#endif /* _MT7530_NL_H_ */
