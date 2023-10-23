// SPDX-License-Identifier: GPL-2.0
/******************************************************************************
 * Copyright (c) 2020 - 2021, MaxLinear, Inc.
 * Copyright 2018 - 2020 Intel Corporation
 *
 ******************************************************************************/

#ifndef __DATAPATH_API_VPN_H_
#define __DATAPATH_API_VPN_H_

#include <net/datapath_api.h>

#if !IS_ENABLED(CONFIG_INTEL_DATAPATH_HAL_GSWIP30)

enum intel_vpn_mode {
	VPN_MODE_TUNNEL,
	VPN_MODE_TRANSPORT
};

enum intel_vpn_ip_mode {
	VPN_IP_MODE_IPV4,
	VPN_IP_MODE_IPV6
};

enum intel_vpn_direction {
	VPN_DIRECTION_OUT,
	VPN_DIRECTION_IN
};

/*! @brief ipsec parameter to be passed when creating tunnel */
struct intel_vpn_ipsec_params {
	/*!< cipher and auth algo name, based on xfrm.h */
	char *enc_algo;
	char *auth_algo;

	enum intel_vpn_mode mode;
	enum intel_vpn_ip_mode ip_mode;
	enum intel_vpn_direction direction;

	/*!< key, iv, inner/outer hash */
	u8 *key;
	u32 key_len;
	u8 *authkey;
	u32 authkeylen;
	u8 *iv;
	u8 *authkey3;
	u8 *nonce;

	u32 spi;
};

/*! @brief crypto engine info to be passed from eip197 driver to vpn firmware */
struct intel_vpn_ipsec_info {
	u32 mode;

	/*!< size info */
	u32 token_words;
	u32 token_header_word;
	u32 iv_size;
	u32 icv_len;
	u32 pad_blksize;

	/*!< offset of instructions within token */
	u32 ipcsum_offs;
	u32 total_pad_offs;
	u32 crypto_offs;
	u32 hash_pad_offs;
	u32 msg_len_offs;

	/*!< context, token, and cdr template buffer */
	u8 *sa_buffer;
	u8 *token_buffer;
	u8 *cdr_buffer;
};

/*! @brief crypto engine ops */
struct intel_vpn_ipsec_ops {
	/*!< create a tunnel */
	void *(*create_sa)(struct intel_vpn_ipsec_params *param);

	/*!< get info from an existing tunnel */
	int (*get_info)(void *ctx, struct intel_vpn_ipsec_info *info);

	/*!< delete a tunnel */
	void (*delete_sa)(void *ctx);
};

/*! @brief session/tunnel action */
struct intel_vpn_ipsec_act {
	u32 dw0_mask; /*!< mask set by PPA
		       * mask 0: reset the bit
		       * mask 1: keep the bit value
		       */
	u32 dw0_val;  /*!< new dw value set by PPA
		       * final dw0 = (receiving_dw0 & dw0_mask ) | dw0_val
		       */
	u32 dw1_mask; /*!< mask set by PPA
		       * mask 0: reset the bit
		       * mask 1: keep the bit value
		       */
	u32 dw1_val;  /*!< new dw value set by PPA
		       * final dw0 = (receiving_dw0 & dw0_mask ) | dw0_val
		       */
	u32 enq_qos; /*!< flag to indicate enqueue with or without qos
		      *   1: enqueue with qos
		      *   0: enqueue bypass qos
		      */
};

/*! @brief tunnel related info */
struct intel_vpn_tunnel_info {
	u32 tunnel_id;
	u32 gpid;
	u32 subif;
	u32 qid;
	enum intel_vpn_mode mode;
	enum intel_vpn_ip_mode ip_mode;
};

struct intel_vpn_ops {
	struct device *dev;

	/*!
	 *@brief Register crypto engine hardware (eip197) ops to vpn driver
	 *@param[in] dev: device
	 *@param[in] ops: pointer to ops
	 *@return Returns 0 on succeed
	 */
	int (*ipsec_register)(struct device *dev,
			      struct intel_vpn_ipsec_ops *ops);

	/*!
	 *@brief Look for an existing tunnel info from SPI
	 *@param[in] dev: device
	 *@param[in] spi: SPI index
	 *@param[out] info: tunnel info
	 *@return Returns 0 on succeed
	 */
	int (*get_tunnel_info)(struct device *dev, u32 spi,
			       struct intel_vpn_tunnel_info *info);

	/*!
	 *@brief Update inbound action of tunnel
	 *@param[in] dev: device
	 *@param[in] tunnel_id: tunnel id
	 *@param[in] act: action
	 *@return Returns 0 on succeed
	 */
	int (*update_tunnel_in_act)(struct device *dev, int tunnel_id,
				    struct intel_vpn_ipsec_act *act);

	/*!
	 *@brief Update outbound action of tunnel
	 *@param[in] dev: device
	 *@param[in] tunnel_id: tunnel id
	 *@param[in] act: action
	 *@return Returns 0 on succeed
	 */
	int (*update_tunnel_out_act)(struct device *dev, int tunnel_id,
				     struct intel_vpn_ipsec_act *act);

	/*!
	 *@brief Update ingress netdev
	 *@param[in] dev: device
	 *@param[in] tunnel_id: tunnel id
	 *@param[in] netdev: netdev
	 *@return Returns 0 on succeed
	 */
	int (*update_tunnel_in_netdev)(struct device *dev, int tunnel_id,
				       struct net_device *netdev);

	/*!
	 *@brief Add a session into tunnel
	 *@param[in] dev: device
	 *@param[in] tunnel_id: tunnel id
	 *@param[in] session_id: session id
	 *@param[in] act: action
	 *@return Returns 0 on succeed
	 */
	int (*add_session)(struct device *dev, int tunnel_id, int session_id,
			   struct intel_vpn_ipsec_act *act);

	/*!
	 *@brief Update a session action
	 *@param[in] dev: device
	 *@param[in] tunnel_id: tunnel id
	 *@param[in] session_id: session id
	 *@param[in] act: action
	 *@return Returns 0 on succeed
	 */
	int (*update_session)(struct device *dev, int tunnel_id, int session_id,
			      struct intel_vpn_ipsec_act *act);

	/*!
	 *@brief Delete a session from tunnel
	 *@param[in] dev: device
	 *@param[in] tunnel_id: tunnel id
	 *@param[in] session_id: session id
	 *@return Returns 0 on succeed
	 */
	int (*delete_session)(struct device *dev, int tunnel_id,
			      int session_id);

	/*!
	 *@brief Add xfrm state
	 *@param[in] x: xfrm state
	 *@return Returns 0 on succeed
	 */
	int (*add_xfrm_sa)(struct xfrm_state *x);

	/*!
	 *@brief Delete xfrm state
	 *@param[in] x: xfrm state
	 *@return void
	 */
	void (*delete_xfrm_sa)(struct xfrm_state *x);

	/*!
	 *@brief xfrm offload precheck
	 *@param[in] skb: skb
	 *@param[in] x: xfrm state
	 *@return true if offload is permitted, false otherwise
	 */
	bool (*xfrm_offload_ok)(struct sk_buff *skb, struct xfrm_state *x);
};

/*!
 *@brief get VPN ops registration
 *@param[in] inst: DP instance ID
 *@return VPN ops pointer if registered, or NULL if not registered
 */
static inline struct intel_vpn_ops *dp_get_vpn_ops(int inst)
{
	return (struct intel_vpn_ops *)dp_get_ops(inst, DP_OPS_VPN);
}
#endif /* CONFIG_INTEL_DATAPATH_HAL_GSWIP30 */
#endif /*__DATAPATH_API_VPN_H_ */
