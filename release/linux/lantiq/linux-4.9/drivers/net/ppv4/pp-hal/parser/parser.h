/*
 * Description: PP parser HAL APIs
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2018 Intel Corporation
 */

#ifndef __PARSER_H__
#define __PARSER_H__
#include <linux/types.h>
#include <linux/pp_api.h>
#include <linux/debugfs.h> /* struct dentry */

#include "parser_protocols.h"

#define PRSR_MAX_NAME                   64
#define PRSR_NUM_LAST_ETHTYPE_ENTRIES   16
#define PRSR_NUM_NP_SELECT_ENTRIES      64
#define PRSR_NUM_SKIP_ENTRIES           32
#define PRSR_MAX_EXTRACT_ENTRIES        3

/**
 * @brief Enum defining parser different profiles
 * @note common profile will be used in profiles
 */
enum prsr_prof_id {
	PRSR_PROFILE_FIRST,
	PRSR_PROFILE_COMMON = PRSR_PROFILE_FIRST,
	/* Specific profiles */
	/* Last Entry !!!  */
	PRSR_PROFILES_NUM,
	/* Invalid id      */
	PRSR_PROFILES_INVALID = PRSR_PROFILES_NUM
};

/**
 * @brief Parser Init Config
 * @valid params valid
 * @parser_base parser base address
 * @clk_ctrl_base clock ctrl base address
 * @param dbgfs debugfs parent directory
 */
struct prsr_init_param {
	bool valid;
	u64  parser_base;
	enum prsr_prof_id parser_profile;
	struct dentry *dbgfs;
};

#ifdef CONFIG_PPV4_LGM
/**
 * @struct prsr_stats
 * @brief parser statistics counters
 */
struct prsr_stats {
	/*! number of packets received from RPB */
	u32 rx_pkts;
	/*! number of valid packets received from RPB */
	u32 rx_valid_pkts;
	/*! number of packets sent by the parser */
	u32 tx_pkts;
	/*! number of packets sent to classifier */
	u32 tx_pkts_to_cls;
	/*! number of packets sent to micro controller */
	u32 tx_pkts_to_uc;
	/*! number of packetsdropped by the micro controller */
	u32 uc_discard;
	/*! number of packets received with discard flag */
	u32 pprs_discard;
	/*! number of packets received from RPB with discard flag */
	u32 rpb_discard;
	/*! number of packets received from RPB with error flag */
	u32 rpb_err;
	/*! number of packets received from RPB with zero length */
	u32 zero_len;
	/*! number of packets which a last ethernet type wasn't found */
	u32 last_eth_err;
	/*! number of packets that layer 3 protocol was bigger than the
	 *  maximum or smaller than the minimum
	 */
	u32 proto_len_err;
	/*! number of packets that layer 3 header was bigger or smaller than
	 *  the values configured in the imem table
	 */
	u32 hdr_len_err;
	/*! number of packets with protocol overflow error */
	u32 proto_overflow;
	/*! number of packets with a mismatch between the IP header length
	 *  and packet length reported by the RPB
	 */
	u32 ip_len_err;
	/*! number of packets with no payload */
	u32 payload_err;
	/*! number of aborted packets */
	u32 aborted_pkts;
};

/**
 * @brief Enumeration for L2 protocol id compere operation <br>
 *        2 operations available: equal and less than
 */
enum l2_cmp_op {
	L2_CMP_OP_EQ,
	L2_CMP_OP_LT,
	L2_CMP_OP_NUM,
};

/**
 * @brief Parser next protocol select logic, np select logic combined
 *        with the next protocol extract from the header gives the key
 *        for next protocol parsing.
 * @note AKA np sel logic
 */
enum np_logic {
	NP_LOGIC_TUNN,		/* tunnels          */
	NP_LOGIC_IPV4,		/* external IPv6    */
	NP_LOGIC_IPV6,		/* external IPv4    */
	NP_LOGIC_IP_SELECT,	/* IP select nibble */
	NP_LOGIC_IPV4_2,	/* internal IPv4    */
	NP_LOGIC_IPV6_2,	/* internal IPv6    */
	NP_LOGIC_UDP,		/* UDP tunnels      */
	NP_LOGIC_END,		/* for protocols prior to the payload */
	NP_LOGICS_NUM
};

/**
 * @brief Parser header length field arithmetic operations. Defines
 *        the arithmetic operations can be done on the header field
 *        length extracted from the packet
 * @ALU_OP_IM header size = immediate value
 * @ALU_OP_LEN_PLUS_IM header size = header field + immediate value
 * @ALU_OP_LEN_AND_IM header size = header field & immediate value
 * @ALU_OP_LEN_OR_IM header size = header field | immediate value
 * @ALU_OP_LEN_4_PLUS_IM header size = header field * 4 + immediate
 *                       value
 * @ALU_OP_LEN_8_PLUS_IM header size = header field * 8 + immediate
 *                       value
 * @ALU_OP_LEN_16_PLUS_IM header size = header field * 16 + immediate
 *                        value
 * @Note AKA skip opcode
 */
enum alu_op {
	ALU_OP_IM,
	ALU_OP_LEN_PLUS_IM,
	ALU_OP_LEN_AND_IM,
	ALU_OP_LEN_OR_IM,
	ALU_OP_4XLEN_PLUS_IM,
	ALU_OP_8XLEN_PLUS_IM,
	ALU_OP_16XLEN_PLUS_IM,
	ALU_OP_4XC_4XK_4XS_PLUS_4,
	ALU_OPS_NUM
};

/**
 * @brief Parser current protocol skip logic, for each protocol the
 *        parser need to find out how to skip to the next protocol,
 *        the skipping operation is different in each protocol. <br>
 *        3 types are supported: <br>
 *        Arithmetic operation: do an arithmetic operation on the
 *                              header len field extracted from the
 *                              packet <br>
 *        Lookup: the header len field extracted from the packet
 *                combined with 'enum skip_op', the value is than
 *                searched in the skip table.
 *        IM: header length is an immediate value, fixed size header
 *               length, e.g. IPv6 header size is always 40 bytes
 * @Note AKA skip select
 */
enum skip_logic {
	SKIP_LOGIC_ALU,
	SKIP_LOGIC_LOOKUP,
	SKIP_LOGIC_IM,
	SKIP_LOGICS_NUM
};

/**
 * @brief Upper protocol entry type,
 */
enum prsr_entry_type {
	ETYPE_NONE,
	ETYPE_IPV4,
	ETYPE_IPV6,
	ETYPE_OTHER_L3,
	ETYPE_UDP,
	ETYPE_TCP,
	ETYPE_RSRV_L4,
	ETYPE_OTHER_L4,
	ETYPE_L2,
	ETYPE_PAYLOAD,
	ETYPE_IPV6_FRAG,
	ETYPE_IPV6_EXT,
	ETYPE_NUM
};

/**
 * @brief Field extraction info, define how to extract a specific
 *        field from the protocol header and where it should be copied
 *        in the field vector.
 * @fv_idx The place to copy the extracted field to, byte index in the
 *      field vector, MUST be an even number.
 * @len extracted field length, number of bytes to extract
 * @off extracted data offset from the start of the protocol header
 * @mask extracted data bytes bitmap which specify which bytes from
 *       the extracted data should be masked, meaning, excluded from
 *       classification, bit N specify if byte N should be masked
 *       where '1' means to mask, '0' means not to mask
 * @example extract 0-9 bytes from and copy to fv index 0 <br>
 *          fv_idx = 0, len = 10, off = 0, mask 0<br>
 * @example extract bytes 1 and 3 and copy to fv index 4 <br>
 *          fv_idx = 4, len = 3, off = 1, mask 0b010<br>
 * @note if case of odd 'off' or 'len' more than 'len' bytes will be
 *       extracted and copied to the field vector where the extra
 *       bytes will be set to zero in the field vector
 **/
struct field_ext_info {
	u16 fv_idx;
	u16 len;
	u16 off;
	u32 mask;
};

/**
 * @brief Protocol header fields extract info.
 *        Defined offset and length of fields within the protocol
 *        header which should be extracted and copied to the field
 *        vector
 * @fld fields info array
 * @n_fld number of valid fields in the array
 */
struct ext_info {
	struct field_ext_info fld[PRSR_MAX_EXTRACT_ENTRIES];
	u16 n_fld;
};

/**
 * @brief Layer 2 table entry. <br>
 *        Layer 2 table defined the protocols the parser should expect
 *        in the packets layer 2 header, e.g. VLAN, STAG, PPPoE, SNAP.
 * @ethtype ethertype, the protocol id in the layer2 header
 * @fallback specify if the protocol should be skipped by the parser
 *           HW and will be forwarded to the parser FW
 * @hdr_len protocol header length in bytes excluding the 2 bytes of
 *          the EtherType
 * @cmp_op choose how should the protocol should be match, equal to or
 *         less than @proto_id
 */
struct l2_entry {
	u16  ethtype;
	u16  hdr_len;
	bool fallback;
	enum l2_cmp_op cmp_op;
};

/**
 * @brief Parser low level protocol definition. <br>
 *        Low level protocol is L2 protocol like: VLAN, STAG, PPPoE,
 *        SNAP.
 * @name protocol name
 * @hw hardware information
 * @hw_idx index where the protocol should be configured to the layer
 *         2 table, for dynamic index allocation use
 *         PRSR_TBL_ENTRY_INVALID_IDX
 * @ext protocol header fields extract info
 */
struct prsr_l2_proto_params {
	char name[PRSR_MAX_NAME];
	struct l2_entry hw;
	struct ext_info ext;
	u16 hw_idx;
};

/**
 * @brief Next protocol value extraction info, defines how the next
 *        protocol value should be extracted from the protocol header
 * @off next protocol field offset in BITS
 * @len next protocol field length in BITS
 * @logic specify which logic should be used to determine the next
 *        protocol
 * @dflt internal id of the default protocol to use in case
 *       the next protocol cannot be identified, usually should be
 *       'PRSR_PROTO_PAYLOAD'
 */
struct np_info {
	u16 off;
	u16 len;
	enum np_logic      logic;
	enum prsr_proto_id dflt;
};

/**
 * @brief Header length lookup info based on the field which is
 *        extracted from the header. This should be used for protocols
 *        which the header contain some value that the header size is
 *        based on. For example UDP protocol support tunneling by
 *        setting specfic values in the dest port field, so in case
 *        the UDP header size will be different based on the tunnel
 *        type, one should use this struct to configure these tunnel
 *        type values
 * @pairs array of pairs of keys and values where the @val is the
 *        header length in case @kay was extracted from the header
 * @n number of pairs
 *
 */
struct lut_table {
	struct {
		u16 key;
		u16 val;
	} pairs[PRSR_NUM_SKIP_ENTRIES];
	u16 n;
};

/**
 * @brief Protocol header length calculation info.
 *        user should fill the information on how the protocol header
 *        size should be calculated in order for the parser to be able
 *        to skip to the next protocol.<br>
 *        Few available options to choose, @see 'enum np_logic' <br>
 *        When the logic is ALU, few arithmetic operations are
 *        available, @see 'enum alu_op'
 * @hdr_len_off offset in bits to the first bit following the header
 *           length field within the header, see @np_off example,
 *           AKA fld1_sel, use for extracting the header length
 *           from the header
 * @hdr_len_sz header length field size in bits, AKA fld1_width, use
 *          for extracting the header length from the header
 * @hdr_len_im immediate header length value use for protocols with
 *          fixed header length, e.g. IPv6 header length is
 *          always 40
 * @logic which skip logic to use to determine what is the header
 *        size, when logic is set to: <br>
 *        ALU - @hdr_len_off and @hdr_len_sz are used to extract the
 *              header length field from the header and an arithmetic
 *              operation will be done on the extracted value, the
 *              result is the header length <br>
 *        LOOKUP - @hdr_len_off and @hdr_len_sz are used to extract
 *                 a field from the header, a lookup key is created as
 *                 the combination of the extracted field and @op,
 *                 then, a lookup is done using the lookup key in a
 *                 table which contains the header length based on the
 *                 lookup key
 *        IM - @hdr_len_im is used as a fixed size for the header
 *             length
 * @op arithmetic operation opcode
 * @hdr_len_min minimum allowed header length
 * @hdr_len_max maximum allowed header length, used only if greater
 *              than @hdr_len_min
 * @lut_table lookup table to use if @logic is set to LOOKUP
 * @note to enable header length min and max check, @hdr_len_max must
 *       be greater than @hdr_len_min
 */
struct skip_info {
	/* header length extraction info */
	u16 hdr_len_off;
	u16 hdr_len_sz;
	u16 hdr_len_im;
	enum skip_logic logic;
	enum alu_op op;
	u16 hdr_len_min;
	u16 hdr_len_max;
	struct lut_table lut;
};

/**
 * @brief Preceding protocol info, define a preceding protocol for
 *        protocol. e.g. IPv4 is a preceding protocol for UDP/TCP.
 * @np_logic next protocol logic
 * @val the value that identify the protocol in the preceding protocol
 *      header. e.g. UDP val in IPv4/IPv6 header is IPPROTO_UDP
 */
struct pre_proto {
	enum np_logic np_logic;
	u16           val;
};

/**
 * @brief Protocol last ethernet type info, defines the information
 *        needed to configure a protocol as a last ethernet type.
 * @ethtype the ethernet type as it should appear in the ethertype
 *           field of l2
 * @name name describing the last ethertype
 * @example IPv4 can appear in 2 forms, 0x800 and 0x0021 for PPPoE
 **/
struct last_eth {
	u16  ethtype;
	char name[PRSR_MAX_NAME];
};

/**
 * @brief Parser upper layer protocol definition. <br> Upper layer
 *        protocol is L3 or higher protocol or L2 protocol of an
 *        internal packet e.g. [ETH][IPv4][GRE][ETH][PPPoE] the PPPoE
 *        should be configured as upper layer protocol
 * @name protocol name
 * @proto_id protocol identifier, the id that should appear on the
 *           packet
 * @hw_idx index where the protocol should be configured to the layer
 *         2 table, for dynamic index allocating use
 *         PRSR_TBL_ENTRY_INVALID_IDX
 * @hdr_len always set to zero
 * @n_pre number of valid entries in 'pre' array
 * @fallback specify if the protocol should be skipped by the parser
 *           HW and will be forwarded to the parser FW
 * @ignore specify if the protocol should be ignored during the
 *         parsing and only be skipped
 * @np information needed to extract the next protocol from the header
 * @skip information needed to skip to the next protocol header
 * @ext protocol header fields extract info, specify which fields
 *      should be extracted, from where, and to where they should be
 *      copied in the field vector
 * @pre array of the protocol's preceding protocols, specify on which
 *      protocols the protocol can appear as the next protocol
 * @leth last ethertype values array, specify which last ethertype
 *       values means that the protocol is the first protocol after
 *       layer 2
 * @entry_type specify the protocol type
 */
struct prsr_up_layer_proto_params {
	char name[PRSR_MAX_NAME];
	u16  proto_id;
	u16  hw_idx;
	u16  hdr_len;
	u16  n_pre;
	u16  n_leth;
	bool fallback;
	bool ignore;
	struct np_info   np;
	struct skip_info skip;
	struct ext_info  ext;
	struct pre_proto pre[PRSR_NUM_NP_SELECT_ENTRIES];
	struct last_eth  leth[PRSR_NUM_LAST_ETHTYPE_ENTRIES];
	enum prsr_entry_type entry_type;
};

/**
 * @brief Protocols merge info, defines a pair of protocols which can
 *        appear in sequence in a packet and should be merge into 1
 *        protocol in the parser's output<br>
 *        The parser can include up to 6 protocols headers on his
 *        output, when more is needed, 2 protocols can be merged into
 *        1 using this struct.
 * @prev previous protocol
 * @current current protocol
 * @merged protocol to use, must be either 'prev' or 'curr'
 */
struct merge_info {
	enum prsr_proto_id prev;
	enum prsr_proto_id curr;
	enum prsr_proto_id merged;
};

/**
 * @brief Configure per port parsing configuration and classification
 * @param port pp port number
 * @param cls classification info, status word fields to copy to fv
 * @param parse_type parsing type
 * @return s32 0 on success, error code otherwise
 */
s32 prsr_port_cfg_set(u16 port, struct pp_port_cls_info *cls,
		      enum pp_port_parsing parse_type);

/**
 * @brief Set port fallback to FW, enable/disable all packets received
 *        on specified port to be fallback to FW
 * @param port port id
 * @param fallback
 * @return s32 0 on success, error code otherwise
 */
s32 prsr_port_fallback_set(u16 port, bool fallback);

/**
 * @brief Configure preceding protocols for a protocol.<br>
 *        Configure for which protocols the protocol can be the next
 *        protocol.
 * @param id the protocol internal id
 * @param pre_protos the protocols internal id array with the values
 *                   to identify the protocol
 * @param n number of entries in pre_protos
 * @return s32 0 on success, error code otherwise
 */
s32 prsr_up_layer_proto_pre_proto_add(enum prsr_proto_id id,
				      const struct pre_proto *pre, u16 pre_n);

/**
 * @brief Adds new high level protocol to PP parser
 * @note Caller must be familiar with 'struct pp_field_vector'
 * @param id protocol internal parser driver id
 * @param params protocol parameters
 * @return s32 0 on success, error code on failure
 */
s32 prsr_up_layer_proto_add(enum prsr_proto_id id,
			    const struct prsr_up_layer_proto_params *params);

/**
 * @brief Merge 2 protocols into 1 <br>
 *        The parser can include up to 6 protocols headers on his
 *        output, when more is needed, 2 protocols can be merged
 *        together.
 * @param info merge info including the 2 protocols to merge
 * @return s32 0 on success, error code otherwise
 */
s32 prsr_protocols_merge(const struct merge_info *info);

/**
 * @brief Delete a protocol from parser driver and hw
 * @param id protocol internal id
 * @return s32 0 on success, error code otherwise
 */
s32 prsr_proto_del(enum prsr_proto_id id);

/**
 * @brief Enable/Disable support for IPv6 next headers
 * @param nexthdr next header field value
 * @param en specify whether to support or not the specified next
 *           header
 * @return u32 0 on success, false otherwise
 */
s32 prsr_ipv6_nexthdr_support_set(u32 nexthdr, bool en);

/**
 * @brief Get Enable/Disable support status for IPv6 next headers
 * @param nexthdr next header field value
 * @param en buffer to store the status
 * @return u32 0 on success, false otherwise
 */
s32 prsr_ipv6_nexthdr_support_get(u32 nexthdr, bool *en);

/**
 * @brief Get the parser statistics
 * @param stats
 * @return s32 0 on success, error code otherwise
 */
s32 prsr_stats_get(struct prsr_stats *stats);

/**
 * @brief Reset Parser statistics
 */
void prsr_stats_reset(void);

/**
 * @brief Parser driver init API
 * @param cfg parser init parameters
 * @return s32 0 on success, error code otherwise
 */
s32 prsr_init(struct prsr_init_param *init_param);

/**
 * @brief Parser driver exit API, clean up all allocated memories
 */
void prsr_exit(void);
#else
static inline s32 prsr_init(struct prsr_init_param *cfg)
{
	if (cfg->valid)
		return -EINVAL;
	else
		return 0;
}

static inline void prsr_exit(void)
{
}
#endif /* CONFIG_PPV4_LGM */
#endif
