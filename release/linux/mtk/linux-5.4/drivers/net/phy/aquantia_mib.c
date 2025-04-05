// SPDX-License-Identifier: GPL-2.0
/* Packet counter driver for Aquantia PHY
 */

#include <linux/phy.h>
#include <linux/kernel.h>
#include <linux/debugfs.h>
#include <linux/kthread.h>

#include "aquantia.h"

#define MDIO_PCS_LS_TX_GOOD_COUNTER		0xc820
#define MDIO_PCS_LS_TX_BAD_COUNTER		0xc822
#define MDIO_PCS_SS_TX_GOOD_COUNTER		0xc860
#define MDIO_PCS_SS_TX_BAD_COUNTER		0xc862
#define MDIO_PCS_CRC8_ERROR_COUNTER		0xe810
#define MDIO_PCS_LS_RX_GOOD_COUNTER		0xe812
#define MDIO_PCS_LS_RX_BAD_COUNTER		0xe814
#define MDIO_PCS_LDPC_ERROR_COUNTER		0xe820
#define MDIO_PCS_SS_RX_GOOD_COUNTER		0xe860
#define MDIO_PCS_SS_RX_BAD_COUNTER		0xe862

static int aqr107_mib_read_word(struct phy_device *phydev, u32 reg, u16 *lsw, u16 *msw)
{
	int val;

	val = phy_read_mmd(phydev, MDIO_MMD_PCS, reg + 1);
	if (val < 0)
		return val;

	*msw = val;

	val = phy_read_mmd(phydev, MDIO_MMD_PCS, reg);
	if (val < 0)
		return val;

	*lsw = val;

	return 0;
}

static void aqr107_mib_read(struct phy_device *phydev)
{
	struct aqr107_priv *priv = phydev->priv;
	u16 lsw, msw;

	if (!aqr107_mib_read_word(phydev, MDIO_PCS_CRC8_ERROR_COUNTER, &lsw, &msw))
		priv->mib.crc8_error_packets += ((msw << 16) | lsw);

	if (!aqr107_mib_read_word(phydev, MDIO_PCS_LDPC_ERROR_COUNTER, &lsw, &msw))
		priv->mib.ldpc_error_packets += ((msw << 16) | lsw);

	if (!aqr107_mib_read_word(phydev, MDIO_PCS_LS_TX_GOOD_COUNTER, &lsw, &msw))
		priv->mib.ls_tx_good_packets += ((msw << 16) | lsw);

	if (!aqr107_mib_read_word(phydev, MDIO_PCS_LS_TX_BAD_COUNTER, &lsw, &msw))
		priv->mib.ls_tx_bad_packets += ((msw << 16) | lsw);

	if (!aqr107_mib_read_word(phydev, MDIO_PCS_LS_RX_GOOD_COUNTER, &lsw, &msw))
		priv->mib.ls_rx_good_packets += ((msw << 16) | lsw);

	if (!aqr107_mib_read_word(phydev, MDIO_PCS_LS_RX_BAD_COUNTER, &lsw, &msw))
		priv->mib.ls_rx_bad_packets += ((msw << 16) | lsw);

	if (!aqr107_mib_read_word(phydev, MDIO_PCS_SS_TX_GOOD_COUNTER, &lsw, &msw))
		priv->mib.ss_tx_good_packets += ((msw << 16) | lsw);

	if (!aqr107_mib_read_word(phydev, MDIO_PCS_SS_TX_BAD_COUNTER, &lsw, &msw))
		priv->mib.ss_tx_bad_packets += ((msw << 16) | lsw);

	if (!aqr107_mib_read_word(phydev, MDIO_PCS_SS_RX_GOOD_COUNTER, &lsw, &msw))
		priv->mib.ss_rx_good_packets += ((msw << 16) | lsw);

	if (!aqr107_mib_read_word(phydev, MDIO_PCS_SS_RX_BAD_COUNTER, &lsw, &msw))
		priv->mib.ss_rx_bad_packets += ((msw << 16) | lsw);
}

static int aqr107_mib_thread(void *data)
{
	struct phy_device *phydev = data;

	for (;;) {
		if (kthread_should_stop())
			break;

		aqr107_mib_read(phydev);

		set_current_state(TASK_INTERRUPTIBLE);
		schedule_timeout(HZ);
	}

	return 0;
}

static int aqr107_mib_show(struct seq_file *m, void *private)
{
	struct phy_device *phydev = m->private;
	struct aqr107_priv *priv = phydev->priv;

	aqr107_mib_read(phydev);

	seq_printf(m, "+---------------------------------+\n");
	seq_printf(m, "|         <<AQUANTIA MIB>>        |\n");
	seq_printf(m, "| CRC8 Error Packets=%012lld |\n", priv->mib.crc8_error_packets);
	seq_printf(m, "| LDPC Error Packets=%012lld |\n", priv->mib.ldpc_error_packets);
	seq_printf(m, "|           [Line Side]\n");
	seq_printf(m, "| TX   Good  Packets=%012lld |\n", priv->mib.ls_tx_good_packets);
	seq_printf(m, "| TX   Bad   Packets=%012lld |\n", priv->mib.ls_tx_bad_packets);
	seq_printf(m, "| RX   Good  Packets=%012lld |\n", priv->mib.ls_rx_good_packets);
	seq_printf(m, "| RX   Bad   Packets=%012lld |\n", priv->mib.ls_rx_bad_packets);
	seq_printf(m, "|          [System Side]\n");
	seq_printf(m, "| TX   Good  Packets=%012lld |\n", priv->mib.ss_tx_good_packets);
	seq_printf(m, "| TX   Bad   Packets=%012lld |\n", priv->mib.ss_tx_bad_packets);
	seq_printf(m, "| RX   Good  Packets=%012lld |\n", priv->mib.ss_rx_good_packets);
	seq_printf(m, "| RX   Bad   Packets=%012lld |\n", priv->mib.ss_rx_bad_packets);
	seq_printf(m, "+---------------------------------+\n");

	memset(&priv->mib, 0, sizeof(priv->mib));

	return 0;
}

static int aqr107_mib_open(struct inode *inode, struct file *file)
{
	return single_open(file, aqr107_mib_show, inode->i_private);
}

int aqr107_config_mib(struct phy_device *phydev)
{
	static const struct file_operations fops_mib = {
		.open = aqr107_mib_open,
		.read = seq_read,
		.llseek = seq_lseek,
		.release = single_release
	};

	struct aqr107_priv *priv = phydev->priv;
	struct dentry *root;
	char dirname[5];

	snprintf(dirname, sizeof(dirname), "phy%d", phydev->mdio.addr);

	root = debugfs_lookup("aquantia", NULL);
	if (!root) {
		root = debugfs_create_dir("aquantia", NULL);
		if (!root)
			return -ENOMEM;
	}

	debugfs_create_file(dirname, S_IRUGO, root, phydev, &fops_mib);

	if (!priv->mib_thread) {
		/* create a thread for recording packet counts */
		priv->mib_thread = kthread_create(aqr107_mib_thread,
						  phydev,
						  "aqr107_mib_thread");
		if (IS_ERR(priv->mib_thread)) {
			phydev_err(phydev,
				   "failed to create aqr107_mib_thread(%ld)\n",
				   PTR_ERR(priv->mib_thread));
			return PTR_ERR(priv->mib_thread);
		}
		wake_up_process(priv->mib_thread);
	}

	return 0;
}
