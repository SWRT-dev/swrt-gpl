/******************************************************************************
 * Copyright (c) 2021, MaxLinear, Inc.
 *
 ******************************************************************************/

#ifndef _DATAPATH_API_TX_GRX500_H
#define _DATAPATH_API_TX_GRX500_H
#include <linux/bits.h>
#include <linux/types.h>
#include <linux/netdevice.h>

struct sk_buff;

/*! @{ */
/*!
 * @file datapath_api_tx.h
 *
 * @brief Datapath TX path API
 */
/*! @} */

/**
 * enum DP_TX_FN_RET - define TX return value for dp_xmit() with DP_TX_NEWRET
 * flag
 */
/*! @brief define TX return value for dp_xmit() with DP_TX_NEWRET flag */
enum DP_TX_FN_RET {
	DP_TX_FN_CONSUMED = NETDEV_TX_OK,
	DP_TX_FN_DROPPED = NET_XMIT_DROP,
	DP_TX_FN_BUSY = NETDEV_TX_BUSY,
	/* DP_TX_FN_CONTINUE is an internal state, will not be returned to
	 * dp_xmit() */
	DP_TX_FN_CONTINUE = INT_MAX,
};

/**
 * enum DP_TX_PRIORITY - define TX chain priority from high to low
 */
/*! @brief define TX chain priority from high to low */
enum DP_TX_PRIORITY {
	DP_TX_PP, /*!< packet processor */
	DP_TX_VPNA, /*!< VPN adapter */
	DP_TX_TSO, /*!< traffic offload engine */
	DP_TX_CQM,  /*!< CQM CPU port */
	DP_TX_CNT,
};

/**
 * enum DP_TX_FLAGS - define TX path common flags
 */
/*! @brief define TX path common flags */
enum DP_TX_FLAGS {
	DP_TX_FLAG_NONE       = BIT(0), /*!< @brief enable HW checksum offload */
	DP_TX_CAL_CHKSUM      = BIT(1), /*!< @brief enable HW checksum offload */
	DP_TX_DSL_FCS         = BIT(2), /*!< @brief enable DSL FCS checksum offload */
	DP_TX_BYPASS_QOS      = BIT(3), /*!< @brief bypass QoS */
	DP_TX_BYPASS_FLOW     = BIT(4), /*!< @brief bypass DMA descriptor filling flow */
	DP_TX_OAM             = BIT(5), /*!< @brief OAM packet */
	DP_TX_TO_DL_MPEFW     = BIT(6), /*!< @brief Send pkt directly to DL FW */
	DP_TX_INSERT          = BIT(7), /*!< @brief GSWIP insertion Support */
	DP_TX_INSERT_POINT    = BIT(8), /*!< @brief For GSWIP insertion Point 0 or 1 */
	DP_TX_WITH_PMAC       = BIT(9), /*!< @brief caller already put pmac in
					*           the skb->data-16 to skb->data-1
					*/
	DP_TX_NEWRET          = BIT(10), /*!< @brief dp_xmit() returns enum DP_TX_FN_RET */
};

/**
 * struct dp_tx_common - datapath TX callback parameters
 * @flags: bitmap of DP_TX_FLAGS
 * @private: private value from dp_register_txpath()
 */
/*! @brief datapath TX callback parameters */
struct dp_tx_common {
	u32 subif;                    /*!< subif id */
	u32 flags;                    /*!< bitmap of enum DP_TX_FLAGS */
	int alloc_flags;              /*!< bitmap of enum DP_F_FLAG */
	u8 *pmac;                     /*!< PMAC */
	u8 pmac_len;                  /*!< length of PMAC */
	u8 tx_portid;                 /*!< tx portid */
	u8 inst;                      /*!< inst */
};

/**
 * tx_fn() - callback prototype for TX path
 * @param[in,out] skb: skb to process
 * @param[in,out] cmn: common data which is persist across the call chain
 * @param[in,out] p: private parameter passed in from dp_register_tx()
 *
 * @Return DP_TX_FN_CONSUMED - skb consumed, stop further process;
 *         DP_TX_FN_CONTINUE skb - not consumed, continue for further process;
 *         others - skb not consumed with error code, stop further process
 */
typedef int (*tx_fn)(struct sk_buff *skb, struct dp_tx_common *cmn, void *p);

/**
 * dp_register_tx() - register datapath tx path function
 * @priority: TX priority
 * @tx_fn: callback function
 * @p: parameters
 */
int dp_register_tx(enum DP_TX_PRIORITY priority, tx_fn fn, void *p);

#endif /* end of include guard: _DATAPATH_API_TX_GRX500_H */
