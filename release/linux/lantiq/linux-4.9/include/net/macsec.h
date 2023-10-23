/*
 * include/net/macsec.h - MACsec device
 *
 * Copyright (c) 2015 Sabrina Dubroca <sd@queasysnail.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Includes Intel Corporation's changes dated: 2020.
 * Changed portions - Copyright 2020, Intel Corporation.
 */

#ifndef _NET_MACSEC_H
#define _NET_MACSEC_H

#include <linux/etherdevice.h>
#include <net/gro_cells.h>
#include <uapi/linux/if_macsec.h>

typedef u64 __bitwise sci_t;

#define MACSEC_SCI_LEN 8

/* SecTAG length = macsec_eth_header without the optional SCI */
#define MACSEC_TAG_LEN 6

struct macsec_eth_header {
	struct ethhdr eth;
	/* SecTAG */
	u8  tci_an;
#if defined(__LITTLE_ENDIAN_BITFIELD)
	u8  short_length:6,
		  unused:2;
#elif defined(__BIG_ENDIAN_BITFIELD)
	u8        unused:2,
	    short_length:6;
#else
#error	"Please fix <asm/byteorder.h>"
#endif
	__be32 packet_number;
	u8 secure_channel_id[8]; /* optional */
} __packed;

#define MACSEC_TCI_VERSION 0x80
#define MACSEC_TCI_ES      0x40 /* end station */
#define MACSEC_TCI_SC      0x20 /* SCI present */
#define MACSEC_TCI_SCB     0x10 /* epon */
#define MACSEC_TCI_E       0x08 /* encryption */
#define MACSEC_TCI_C       0x04 /* changed text */
#define MACSEC_AN_MASK     0x03 /* association number */
#define MACSEC_TCI_CONFID  (MACSEC_TCI_E | MACSEC_TCI_C)

/* minimum secure data length deemed "not short", see IEEE 802.1AE-2006 9.7 */
#define MIN_NON_SHORT_LEN 48

#define GCM_AES_IV_LEN 12
#define DEFAULT_ICV_LEN 16

#define MACSEC_NUM_AN 4 /* 2 bits for the association number */

#define for_each_rxsc(secy, sc)			\
	for (sc = rcu_dereference_bh(secy->rx_sc);	\
	     sc;				\
	     sc = rcu_dereference_bh(sc->next))
#define for_each_rxsc_rtnl(secy, sc)			\
	for (sc = rtnl_dereference(secy->rx_sc);	\
	     sc;					\
	     sc = rtnl_dereference(sc->next))

#ifdef CONFIG_MACSEC_HW_OFFLOAD
enum macsec_offload {
	MACSEC_OFFLOAD_OFF = 0,
	MACSEC_OFFLOAD_PHY = 1,
	MACSEC_OFFLOAD_MAC = 2,
	__MACSEC_OFFLOAD_END,
	MACSEC_OFFLOAD_MAX = __MACSEC_OFFLOAD_END - 1,
};
#endif

struct gcm_iv {
	union {
		u8 secure_channel_id[8];
		sci_t sci;
	};
	__be32 pn;
};

/**
 * struct macsec_key - SA key
 * @id: user-provided key identifier
 * @tfm: crypto struct, key storage
 */
struct macsec_key {
	u8 id[MACSEC_KEYID_LEN];
	struct crypto_aead *tfm;
};

struct macsec_rx_sc_stats {
	__u64 InOctetsValidated;
	__u64 InOctetsDecrypted;
	__u64 InPktsUnchecked;
	__u64 InPktsDelayed;
	__u64 InPktsOK;
	__u64 InPktsInvalid;
	__u64 InPktsLate;
	__u64 InPktsNotValid;
	__u64 InPktsNotUsingSA;
	__u64 InPktsUnusedSA;
};

struct macsec_rx_sa_stats {
	__u32 InPktsOK;
	__u32 InPktsInvalid;
	__u32 InPktsNotValid;
	__u32 InPktsNotUsingSA;
	__u32 InPktsUnusedSA;
};

struct macsec_tx_sa_stats {
	__u32 OutPktsProtected;
	__u32 OutPktsEncrypted;
};

struct macsec_tx_sc_stats {
	__u64 OutPktsProtected;
	__u64 OutPktsEncrypted;
	__u64 OutOctetsProtected;
	__u64 OutOctetsEncrypted;
};

struct macsec_dev_stats {
	__u64 OutPktsUntagged;
	__u64 InPktsUntagged;
	__u64 OutPktsTooLong;
	__u64 InPktsNoTag;
	__u64 InPktsBadTag;
	__u64 InPktsUnknownSCI;
	__u64 InPktsNoSCI;
	__u64 InPktsOverrun;
};

/**
 * struct macsec_rx_sa - receive secure association
 * @active:
 * @next_pn: packet number expected for the next packet
 * @lock: protects next_pn manipulations
 * @key: key structure
 * @stats: per-SA stats
 */
struct macsec_rx_sa {
	struct macsec_key key;
	spinlock_t lock;
	u32 next_pn;
	atomic_t refcnt;
	bool active;
	struct macsec_rx_sa_stats __percpu *stats;
	struct macsec_rx_sc *sc;
	struct rcu_head rcu;
};

struct pcpu_rx_sc_stats {
	struct macsec_rx_sc_stats stats;
	struct u64_stats_sync syncp;
};

/**
 * struct macsec_rx_sc - receive secure channel
 * @sci: secure channel identifier for this SC
 * @active: channel is active
 * @sa: array of secure associations
 * @stats: per-SC stats
 */
struct macsec_rx_sc {
	struct macsec_rx_sc __rcu *next;
	sci_t sci;
	bool active;
	struct macsec_rx_sa __rcu *sa[MACSEC_NUM_AN];
	struct pcpu_rx_sc_stats __percpu *stats;
	atomic_t refcnt;
	struct rcu_head rcu_head;
};

/**
 * struct macsec_tx_sa - transmit secure association
 * @active:
 * @next_pn: packet number to use for the next packet
 * @lock: protects next_pn manipulations
 * @key: key structure
 * @stats: per-SA stats
 */
struct macsec_tx_sa {
	struct macsec_key key;
	spinlock_t lock;
	u32 next_pn;
	atomic_t refcnt;
	bool active;
	struct macsec_tx_sa_stats __percpu *stats;
	struct rcu_head rcu;
};

struct pcpu_tx_sc_stats {
	struct macsec_tx_sc_stats stats;
	struct u64_stats_sync syncp;
};

/**
 * struct macsec_tx_sc - transmit secure channel
 * @active:
 * @encoding_sa: association number of the SA currently in use
 * @encrypt: encrypt packets on transmit, or authenticate only
 * @send_sci: always include the SCI in the SecTAG
 * @end_station:
 * @scb: single copy broadcast flag
 * @sa: array of secure associations
 * @stats: stats for this TXSC
 */
struct macsec_tx_sc {
	bool active;
	u8 encoding_sa;
	bool encrypt;
	bool send_sci;
	bool end_station;
	bool scb;
	struct macsec_tx_sa __rcu *sa[MACSEC_NUM_AN];
	struct pcpu_tx_sc_stats __percpu *stats;
};

#define MACSEC_VALIDATE_DEFAULT MACSEC_VALIDATE_STRICT

/**
 * struct macsec_secy - MACsec Security Entity
 * @netdev: netdevice for this SecY
 * @n_rx_sc: number of receive secure channels configured on this SecY
 * @sci: secure channel identifier used for tx
 * @key_len: length of keys used by the cipher suite
 * @icv_len: length of ICV used by the cipher suite
 * @validate_frames: validation mode
 * @operational: MAC_Operational flag
 * @protect_frames: enable protection for this SecY
 * @replay_protect: enable packet number checks on receive
 * @replay_window: size of the replay window
 * @tx_sc: transmit secure channel
 * @rx_sc: linked list of receive secure channels
 */
struct macsec_secy {
	struct net_device *netdev;
	unsigned int n_rx_sc;
	sci_t sci;
	u16 key_len;
	u16 icv_len;
	enum macsec_validation_type validate_frames;
	bool operational;
	bool protect_frames;
	bool replay_protect;
	u32 replay_window;
	struct macsec_tx_sc tx_sc;
	struct macsec_rx_sc __rcu *rx_sc;
};

struct pcpu_secy_stats {
	struct macsec_dev_stats stats;
	struct u64_stats_sync syncp;
};

/**
 * struct macsec_dev - private data
 * @secy: SecY config
 * @real_dev: pointer to underlying netdevice
 * @stats: MACsec device stats
 * @secys: linked list of SecY's on the underlying device
 */
struct macsec_dev {
	struct macsec_secy secy;
	struct net_device *real_dev;
	struct pcpu_secy_stats __percpu *stats;
	struct list_head secys;
	struct gro_cells gro_cells;
	unsigned int nest_level;
#ifdef CONFIG_MACSEC_HW_OFFLOAD
	enum macsec_offload hw_offload;
#endif
};

/**
 * struct macsec_rxh_data - rx_handler private argument
 * @secys: linked list of SecY's on this underlying device
 */
struct macsec_rxh_data {
	struct list_head secys;
};

static inline struct macsec_dev *macsec_priv(const struct net_device *dev)
{
	return (struct macsec_dev *)netdev_priv(dev);
}

#ifdef CONFIG_MACSEC_HW_OFFLOAD
struct macsec_context{
	union {
		struct net_device *netdev;
		struct phy_device *phydev;
	};
	enum macsec_offload offload;
};

struct macsec_ops {
	int (*add_secy)(struct macsec_context *ctx, struct macsec_secy *secy);
	int (*del_secy)(struct macsec_context *ctx, struct macsec_secy *secy);
	int (*add_rxsc)(struct macsec_context *ctx, struct macsec_secy *secy,
			struct nlattr **tb_rxsc);
	int (*del_rxsc)(struct macsec_context *ctx, struct macsec_secy *secy,
			struct nlattr **tb_rxsc);
	int (*upd_rxsc)(struct macsec_context *ctx, struct macsec_secy *secy,
			struct nlattr **tb_rxsc);
	int (*add_txsa)(struct macsec_context *ctx, struct macsec_secy *secy,
			struct nlattr **tb_sa);
	int (*del_txsa)(struct macsec_context *ctx, struct macsec_secy *secy,
			struct nlattr **tb_sa);
	int (*upd_txsa)(struct macsec_context *ctx, struct macsec_secy *secy,
			struct nlattr **tb_sa);
	int (*add_rxsa)(struct macsec_context *ctx, struct macsec_secy *secy,
			struct nlattr **tb_rxsc, struct nlattr **tb_sa);
	int (*del_rxsa)(struct macsec_context *ctx, struct macsec_secy *secy,
			struct nlattr **tb_rxsc, struct nlattr **tb_sa);
	int (*upd_rxsa)(struct macsec_context *ctx, struct macsec_secy *secy,
			struct nlattr **tb_rxsc, struct nlattr **tb_sa);
	int (*dump)(struct macsec_context *ctx, struct macsec_secy *secy);
	int (*change_link)(struct macsec_context *ctx, struct macsec_secy *secy);
};
#endif

bool is_macsec_master(struct net_device *dev);

#endif
