/*
 * Description: PP parser HAL internal definitions
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2018-2019 Intel Corporation
 */

#ifndef __PARSER_INTERNAL_H__
#define __PARSER_INTERNAL_H__
#include <linux/types.h>
#include <linux/bitops.h>
#include <linux/bitmap.h>
#include <linux/spinlock.h>

#include "pp_regs.h"
#include "pp_fv.h"
#include "parser.h"
#include "parser_protocols.h"
#include "parser_regs.h"

#define prsr_err(fmt, ...)      pr_err("Error: caller '%ps':" fmt, \
	__builtin_return_address(0), ##__VA_ARGS__)

extern u64 prsr_base_addr;
#define PRSR_BASE_ADDR prsr_base_addr

/* ========================================================================== *
 *                               Tables Manager                               *
 * ========================================================================== *
 */
/**
 * @brief Table entry
 * @idx entry index within the table
 * @ref_cnt reference counter
 * @rsrv specify if the entry is from the reserved region of
 *       the table or not
 * @used specify if the entry is used or not
 * @name entry name
 * @data user private data
 */
struct table_entry {
	u32  idx;
	u32  ref_cnt;
	bool ent_rsrv;
	bool used;
	char name[PRSR_MAX_NAME];
	const void *data;
};

/**
 * @brief helper structure for struct table, define a region in a
 *        table, reserved or regular entries region
 * @n_used number of used entries
 * @n_free number of free entries
 * @map entries status bitmap, bit N specify if entry N is used(1) or
 *      not(0)
 */
struct tbl_region {
	u32 n_used;
	u32 n_free;
	unsigned long *map;
};

/**
 * @brief Table management structure
 *        A table have a list of reserved entries list to be used for
 *        specific types of entries, only 1 continues region of
 *        reserved entries is supported.
 * @valid table status
 * @name Table name
 * @n_ent total number of entries in the table
 * @n_rsrv total number of reserved entries out of @n_ent
 * @used_map reserved and regular used entires bitmap, bit n specify
 *       if entry n is used or not
 * @reg regular entries helper structure
 * @rsrv reserved entries helper structure
 * @entrs table's entries array, allocated upon table init
 */
struct table {
	bool valid;
	char name[PRSR_MAX_NAME];
	u32  n_ent;
	u32  n_rsrv;
	spinlock_t     lock;
	unsigned long *entrs_map;
	struct tbl_region   reg;
	struct tbl_region   rsrv;
	struct table_entry *entrs;
};

/**
 * @brief Create a new table
 *        A Table support 1 reserved region of entries which can
 *        start at any index
 * @param name table name
 * @param n_ent number of entries
 * @param n_rsrv number of reserved entries, use 0 for none
 * @param rsrv_base first reserve entry index
 * @return struct table * table context on success, error code encoded
 *         as a pointer otherwise, use IS_ERR to determine if error
 *         did occur
 */
struct table * __must_check
table_create(char *name, u32 n_ent, u32 n_rsrv, u32 rsrv_base);

/**
 * @brief Destroy a table which was allocated by table_create api
 * @param tbl table context
 * @return s32 0 n success, error code otherwise
 */
s32 table_destroy(struct table *tbl);

/**
 * @brief Allocate n sequenced entries from a table
 * @param tbl table context
 * @param name entry name
 * @param user private data
 * @param cnt number of entries to alloc
 * @param idx pointer to receive back the first allocated entry
 *        index set to TABLE_ENTRY_INVALID_IDX for dynamic
 *        index allocation
 * @param rsrv specify whether to allocate the 'n' entries from the
 *        reserved region or not
 * @return s32 0 on success, error code otherwise
 */
s32 table_alloc_entries(struct table *tbl, const char *name, const void *data,
			u16 cnt, u16 *idx, bool rsrv);

/**
 * @brief Free table's entry, entry must be first allocated by
 *        calling table_alloc_entries. <br>
 *        Entries doesn't need to be freed prior to destroying the
 *        table.
 * @param tbl table context
 * @param idx entry index
 * @return s32 0 on success, -EEXIST in case the entry wasn't freed
 *         due to ref counter > 0,  error code otherwise
 */
s32 table_free_entry(struct table *tbl, u16 idx);

/**
 * @brief Increment entry's reference counter, for cases where more
 *        than 1 entity is using the entry
 * @param tbl table context
 * @param ent_idx entry index
 * @return s32 update reference counter, negative error code otherwise
 */
s32 table_entry_ref_inc(struct table *tbl, u16 ent_idx);

/**
 * @brief Decrement entry's reference counter
 * @param tbl table context
 * @param ent_idx entry index
 * @return s32 update reference counter, negative error code otherwise
 */
s32 table_entry_ref_dec(struct table *tbl, u16 ent_idx);

/**
 * @brief Get table's first used entry index
 * @param tbl table context
 * @return u16 first used entry index in case the table isn't empty,
 *         PRSR_TBL_INVALID_IDX otherwise
 */
u16 table_get_first_entry(struct table *tbl);

/**
 * @brief Get table's first used entry index starting at offset @off
 * @param tbl table context
 * @return u16 entry index in case the table isn't empty,
 *         PRSR_TBL_INVALID_IDX otherwise
 */
u16 table_get_next_entry(struct table *tbl, u16 off);

/**
 * @brief Get table's first free entry index
 * @param tbl table context
 * @return u16 first used entry index in case the table isn't full,
 *         PRSR_TBL_INVALID_IDX otherwise
 */
u16 table_get_first_free_entry(struct table *tbl);

/**
 * @brief Get table's first used entry index starting at offset @off
 * @param tbl table context
 * @return u16 entry index in case the table isn't empty,
 *         PRSR_TBL_INVALID_IDX otherwise
 */
u16 table_get_next_free_entry(struct table *tbl, u16 off);

/**
 * @brief Table entry index validity test
 * @param table table context
 * @param e_idx entry index to test
 * @return bool true if the entry index is valid, false otherwise
 */
bool table_entry_idx_is_valid(struct table *tbl, u16 e_idx);

/**
 * @brief Get table entry
 * @param tbl table context
 * @param idx entry index
 * @return s32 entry pointer on success, NULL otherwise
 */
const struct table_entry *table_entry_get(struct table *tbl, u16 idx);

/**
 * @brief Get table entry description
 * @param tbl table context
 * @param e_idx entry index
 * @return s32 entry description on success, NULL otherwise
 */
const char *table_entry_name_get(struct table *tbl, u16 e_idx);

/**
 * @brief Shortcut for concatenating a string to an entry description
 */
#define table_entry_desc_cat(tbl, idx, name, sep) \
	table_entry_name_append(tbl, idx, name, sep, false)

/* ========================================================================== *
 *                            Parser Internals                                *
 * ========================================================================== *
 */

/**
 * @brief Per port classification fields, registers defining per port
 *        fields which will be copied from status word into the field
 *        vector for classification<br>
 *        Each port can specify up to 4 fields to be copied from
 *        status word to field vector to be part of session
 *        classification
 */
#define PP_PRSR_PORT_CLS_READ_CMD		(0x1)
#define PP_PRSR_PORT_CLS_WRITE_CMD		(0x2)
#define PP_PRSR_PORT_CLS_FLD0_STW_OFF_MASK	GENMASK(6,   0)
#define PP_PRSR_PORT_CLS_FLD0_FV_OFF_MASK	GENMASK(11,  7)
#define PP_PRSR_PORT_CLS_FLD0_LEN_OFF_MASK	GENMASK(15, 12)
#define PP_PRSR_PORT_CLS_FLD1_STW_OFF_MASK	GENMASK(22, 16)
#define PP_PRSR_PORT_CLS_FLD1_FV_OFF_MASK	GENMASK(27, 23)
#define PP_PRSR_PORT_CLS_FLD1_LEN_OFF_MASK	GENMASK(31, 28)
#define PP_PRSR_PORT_CLS_FLD2_STW_OFF_MASK	GENMASK(6,   0)
#define PP_PRSR_PORT_CLS_FLD2_FV_OFF_MASK	GENMASK(11,  7)
#define PP_PRSR_PORT_CLS_FLD2_LEN_OFF_MASK	GENMASK(15, 12)
#define PP_PRSR_PORT_CLS_FLD3_STW_OFF_MASK	GENMASK(22, 16)
#define PP_PRSR_PORT_CLS_FLD3_FV_OFF_MASK	GENMASK(27, 23)
#define PP_PRSR_PORT_CLS_FLD3_LEN_OFF_MASK	GENMASK(31, 28)

/**
 * @brief Get status word offset field mask base on field index
 * @param fld field index
 */
#define PP_PRSR_PORT_CLS_FLD_STW_OFF_MASK(fld)           \
	(fld == 0 ? PP_PRSR_PORT_CLS_FLD0_STW_OFF_MASK : \
	(fld == 1 ? PP_PRSR_PORT_CLS_FLD1_STW_OFF_MASK : \
	(fld == 2 ? PP_PRSR_PORT_CLS_FLD2_STW_OFF_MASK : \
		    PP_PRSR_PORT_CLS_FLD3_STW_OFF_MASK)))
/**
 * @brief Get field vector offset field mask base on field index
 * @param fld field index
 */
#define PP_PRSR_PORT_CLS_FLD_FV_OFF_MASK(fld)           \
	(fld == 0 ? PP_PRSR_PORT_CLS_FLD0_FV_OFF_MASK : \
	(fld == 1 ? PP_PRSR_PORT_CLS_FLD1_FV_OFF_MASK : \
	(fld == 2 ? PP_PRSR_PORT_CLS_FLD2_FV_OFF_MASK : \
		    PP_PRSR_PORT_CLS_FLD3_FV_OFF_MASK)))
/**
 * @brief Get field length field mask base on field index
 * @param fld field index
 */
#define PP_PRSR_PORT_CLS_FLD_LEN_MASK(fld)               \
	(fld == 0 ? PP_PRSR_PORT_CLS_FLD0_LEN_OFF_MASK : \
	(fld == 1 ? PP_PRSR_PORT_CLS_FLD1_LEN_OFF_MASK : \
	(fld == 2 ? PP_PRSR_PORT_CLS_FLD2_LEN_OFF_MASK : \
		    PP_PRSR_PORT_CLS_FLD3_LEN_OFF_MASK)))
/**
 * @brief Get field register address based on field index
 * @param fld field index
 */
#define PP_PRSR_PORT_CLS_FLD_REG(fld)           \
	(fld == 0 ? PP_PRSR_OOB_FV_HACK_REC_VAL0_REG : \
	(fld == 1 ? PP_PRSR_OOB_FV_HACK_REC_VAL0_REG : \
	(fld == 2 ? PP_PRSR_OOB_FV_HACK_REC_VAL1_REG : \
		    PP_PRSR_OOB_FV_HACK_REC_VAL1_REG)))

/**
 * @brief Parser HW boundaries
 */
#define PRSR_NUM_L2_ETH_TYPE_ENTRIES    (8)
#define PRSR_NUM_IMEM_ENTRIES           (64)
#define PRSR_NUM_MERGE_ENTRIES          (16)
#define PRSR_NUM_PIT_ENTRIES            (64)
#define PRSR_MAX_L2_HDR_LEN             (64)
#define PRSR_NUM_ERR_FLAGS              (32)
#define PRSR_DBG_IMEM_SEQ_MAX           (15)

#define PRSR_LY3_PROTO_SEQ_REG(seq)                     \
	((seq) == 0  ? PP_PRSR_LY3_PROTO_SEQ_0_LAST_REG : \
	((seq) == 1  ? PP_PRSR_LY3_PROTO_SEQ_0_LAST_REG : \
	((seq) == 2  ? PP_PRSR_LY3_PROTO_SEQ_0_LAST_REG : \
	((seq) == 3  ? PP_PRSR_LY3_PROTO_SEQ_0_LAST_REG : \
	((seq) == 4  ? PP_PRSR_LY3_PROTO_SEQ_0_LAST_REG : \
	((seq) == 5  ? PP_PRSR_LY3_PROTO_SEQ_1_LAST_REG : \
	((seq) == 6  ? PP_PRSR_LY3_PROTO_SEQ_1_LAST_REG : \
	((seq) == 7  ? PP_PRSR_LY3_PROTO_SEQ_1_LAST_REG : \
	((seq) == 8  ? PP_PRSR_LY3_PROTO_SEQ_1_LAST_REG : \
	((seq) == 9  ? PP_PRSR_LY3_PROTO_SEQ_1_LAST_REG : \
	((seq) == 10 ? PP_PRSR_LY3_PROTO_SEQ_2_LAST_REG : \
	((seq) == 11 ? PP_PRSR_LY3_PROTO_SEQ_2_LAST_REG : \
	((seq) == 12 ? PP_PRSR_LY3_PROTO_SEQ_2_LAST_REG : \
	((seq) == 13 ? PP_PRSR_LY3_PROTO_SEQ_2_LAST_REG : \
		     PP_PRSR_LY3_PROTO_SEQ_2_LAST_REG))))))))))))))

#define PRSR_LY3_PROTO_SEQ_MASK(seq)                                     \
	((seq) == 0  ? PP_PRSR_LY3_PROTO_SEQ_0_LAST_MSK                  : \
	((seq) == 1  ? PP_PRSR_LY3_PROTO_SEQ_0_LAST_LY3_PROTO_SEQ_1_MSK  : \
	((seq) == 2  ? PP_PRSR_LY3_PROTO_SEQ_0_LAST_LY3_PROTO_SEQ_2_MSK  : \
	((seq) == 3  ? PP_PRSR_LY3_PROTO_SEQ_0_LAST_LY3_PROTO_SEQ_3_MSK  : \
	((seq) == 4  ? PP_PRSR_LY3_PROTO_SEQ_0_LAST_LY3_PROTO_SEQ_4_MSK  : \
	((seq) == 5  ? PP_PRSR_LY3_PROTO_SEQ_1_LAST_LY3_PROTO_SEQ_5_MSK  : \
	((seq) == 6  ? PP_PRSR_LY3_PROTO_SEQ_1_LAST_LY3_PROTO_SEQ_6_MSK  : \
	((seq) == 7  ? PP_PRSR_LY3_PROTO_SEQ_1_LAST_LY3_PROTO_SEQ_7_MSK  : \
	((seq) == 8  ? PP_PRSR_LY3_PROTO_SEQ_1_LAST_LY3_PROTO_SEQ_8_MSK  : \
	((seq) == 9  ? PP_PRSR_LY3_PROTO_SEQ_1_LAST_LY3_PROTO_SEQ_9_MSK  : \
	((seq) == 10 ? PP_PRSR_LY3_PROTO_SEQ_2_LAST_LY3_PROTO_SEQ_10_MSK : \
	((seq) == 11 ? PP_PRSR_LY3_PROTO_SEQ_2_LAST_LY3_PROTO_SEQ_11_MSK : \
	((seq) == 12 ? PP_PRSR_LY3_PROTO_SEQ_2_LAST_LY3_PROTO_SEQ_12_MSK : \
	((seq) == 13 ? PP_PRSR_LY3_PROTO_SEQ_2_LAST_LY3_PROTO_SEQ_13_MSK : \
	 PP_PRSR_LY3_PROTO_SEQ_2_LAST_LY3_PROTO_SEQ_14_MSK))))))))))))))

/**
 * @brief shortcuts for iterating through parser tables entries
 */
#define PRSR_FOR_EACH_L2_ENTRY(i)	\
	for (i = 0; i < PRSR_NUM_L2_ETH_TYPE_ENTRIES; i++)
#define PRSR_FOR_EACH_LAST_ETHERTYPE_ENTRY(i)	\
	for (i = 0; i < PRSR_NUM_LAST_ETHTYPE_ENTRIES; i++)
#define PRSR_FOR_EACH_IMEM_ENTRY(i)	\
	for (i = 0; i < PRSR_NUM_IMEM_ENTRIES; i++)
#define PRSR_FOR_EACH_NP_SELECT_ENTRY(i)	\
	for (i = 0; i < PRSR_NUM_NP_SELECT_ENTRIES; i++)
#define PRSR_FOR_EACH_SKIP_ENTRY(i)	\
	for (i = 0; i < PRSR_NUM_SKIP_ENTRIES; i++)
#define PRSR_FOR_EACH_MERGE_ENTRY(i)	\
	for (i = 0; i < PRSR_NUM_MERGE_ENTRIES; i++)
#define PRSR_FOR_EACH_PIT_ENTRY(i)	\
	for (i = 0; i < PRSR_NUM_PIT_ENTRIES; i++)

/**
 * @brief Parser tables enumaration
 */
enum prsr_tables {
	PRSR_TBL_FIRST,
	PRSR_L2_TBL = PRSR_TBL_FIRST,	/* layer 2              */
	PRSR_LAST_ETHERTYPE_TBL,	/* last ethertype       */
	PRSR_IMEM_TBL,			/* instruction memory   */
	PRSR_NP_SEL_TBL,		/* next protocol select */
	PRSR_SKIP_TBL,			/* protocol skip        */
	PRSR_MERGE_TBL,			/* protocols merge      */
	PRSR_PIT_TBL,			/* protocol information */
	PRSR_NUM_TBLS
};

/**
 * @brief Shortcut for iterating through parser tables
 */
#define PRSR_FOR_EACH_TABLE(i) \
	for (i = PRSR_TBL_FIRST; i < PRSR_NUM_TBLS; i++)

/**
 * @brief Parser tables invalid index
 */
#define TABLE_ENTRY_INVALID_IDX (U16_MAX)
/**
 * @brief PP ports no layer 2 parsing value to configure into parser
 *        ports table to start parsing at layer 2 table
 */
#define PRSR_PORT_NO_L2_SKIP_VAL   (0x3F)

/**
 * @brief PIT table info
 * @base PIT table base index to start PIT commands from
 * @cnt number of PIT table command to execute
 */
struct pit_info {
	u16 base;
	u16 cnt;
};

/**
 * @brief Structure defining port classification field which will be
 *        copied from status word to the field vector.
 * @fv_off field vector bit offset, where the field should be written
 *         in the field vector
 * @stw_off status word bit offset, where the field is in the status
 *          word
 * @len the field length
 */
struct port_clsf_fld {
	u16 fv_off;
	u16 stw_off;
	u16 len;
};

/**
 * @brief Port configuration table. <br> The table defines per port
 *        whether the parser HW module should expect layer 2 on the
 *        incoming packets and if the incoming packets should be
 *        fallback to FW
 * @skip_l2_pc program counter (IMEM table entry index) to use as the
 *             first IMEM entry for parsing packets on the port <br>
 *             Value of @PRSR_PORT_NO_L2_SKIP_VAL for not skipping l2
 * @fallback specify whether to fallback all packets on the port the
 *           FW
 * @fld 4 classification fields, zero length field will be ignored
 */
struct port_cfg_entry {
	u32  skip_l2_pc;
	bool fallback;
	struct port_clsf_fld fld[4];
};

/**
 * @brief Last ethernet type table entry. <br>
 *        The table defines which protocols are expected to be the
 *        last ethernet type in the incoming packets. <br>
 *        If a protocol which isn't defined in this table is found in
 *        the the packet, a default is used, the default will probably
 *        be configured to the 'payload' imem index
 * @proto_id protocol id, the protocol id should be extracted using
 *           the field0 configuration in the IMEM table
 * @imem_idx the IMEM table entry to start higher layers parsing if
 *           @proto_id was found as the last ethernet type in a packet
 */
struct last_ethertype_entry {
	u16 proto_id;
	u16 imem_idx;
};

/**
 * @brief IMEM table entry, defining for each protocol how to extract
 *        the next protocol from the header, what is the header size
 *        for skipping to that next protocol.
 * @hdr_len header length
 * @np_off offset in bits to the first bit following the next protocol
 *         field within the header, e.g. if next protocol starts at
 *         offset x and has length of y, than np_off = x+y, AKA
 *         fld0_sel
 * @np_sz next protocol field size in bits, AKA fld0_width
 * @np_dflt default next protocol to use in case next protocol cannot
 *          be identified, AKA im0
 * @np_logic specify which logic should be used to determine the next
 *           protocol, AKA np_sel
 * @hdr_len_off offset in bits to the first bit following the header
 *              length field within the header, see @np_off example,
 *              AKA fld1_sel, use for extracting the header length
 *              from the header
 * @hdr_len_sz header length field size in bits, AKA fld1_width, use
 *             for extracting the header length from the header
 * @hdr_len_im header length for protocols with constant header
 *             length, e.g. IPv6 header length is always 40, AKA im1
 * @skip_logic which logic to use to determine what is the header size
 * @skip_op arithmetic operation opcode to do when arithmetic skip
 *          logic is used
 * @hdr_len_chck specify if header length should be verified versus
 *               minimum and maximum values
 * @hdr_len_max maximum header length allowed
 * @hdr_len_min minimum header length allowed
 * @rep_valid should always be set to true
 * @rep_idx should always be equal to the entry index
 * @dont_care specify if the protocol should be omitted from the
 *            parser output protocols, when true the parser will not
 *            save the protocol offset and pit info in the status word
 * @fallback specify if packets match to the entry should be fallback
 *           to the fw
 * @entry_type specify which protocol type the entry represent
 * @pit pit table info specifying which pit entries should be
 *        used the extract relevant data from the protocol
 *        header
 */
struct imem_entry {
	u16 hdr_len;
	/* next protocol (field 0) extraction info */
	u16 np_off;
	u16 np_sz;
	u16 np_dflt_im0;
	enum np_logic np_logic;
	/* protocol header length (field 1) calculation info */
	u16 hdr_len_off;
	u16 hdr_len_sz;
	u16 hdr_len_im1;
	enum skip_logic skip_logic;
	enum alu_op     skip_op;
	/* header length check */
	bool hdr_len_chck;
	u16  hdr_len_max;
	u16  hdr_len_min;
	/* replacement */
	bool rep_valid;
	u16  rep_idx;
	/* misc */
	bool dont_care;
	bool fallback;
	enum prsr_entry_type entry_type;
	/* pit */
	struct pit_info pit;
};

/**
 * @brief Convert IMEM entry index to
 *        PP_PRSR_REDUCER_DONT_CARE_PC_REG address
 */
#define PRSR_IMEM_IDX_TO_RDC_REG(idx) \
	(idx >= 32 ? PP_PRSR_REDUCER_DONT_CARE_PC_REG_IDX(1) : \
		     PP_PRSR_REDUCER_DONT_CARE_PC_REG_IDX(0))

/**
 * @brief Convert IMEM entry index to the bit the corresponding bit in
 *        PP_PRSR_REDUCER_DONT_CARE_PC_REG
 */
#define PRSR_IMEM_IDX_TO_RDC_BIT(idx) \
	(idx >= 32 ? BIT(idx - 32) : BIT(idx))

/**
 * @brief Convert IPv6 next header value to corresponding register in
 *        the parser register file
 * @param nexthdr next header value
 */
#define PRSR_IPV6_NEXTHDR_TO_REG(nexthdr) \
	PP_PRSR_UNSUPPORTED_IPV6_NH_REG_IDX((nexthdr) / \
		(sizeof(u32) * BITS_PER_BYTE))

/**
 * @brief Convert IPv6 next header value to the corresponding bit in
 *        the parser registers
 * @param nexthdr next header value
 */
#define PRSR_IPV6_NEXTHDR_TO_BIT(nexthdr) \
	(nexthdr & (typeof(nexthdr))GENMASK(4, 0))

/**
 * @brief Convert value to PIT table hw granularity, hw support values
 *        in 2 bytes granularity.
 */
#define PRSR_PIT_VAL_TO_HW_GRAN(val)	((val) >> 1)

/**
 * @brief Next protocol select table entry, AKA NP_SELECT table <br>
 *        NP Select table defines the high levels protocols order in
 *        the packets <br>
 *        For example that UDP can appear after IPv6 and IPv4 headers
 * @key key constructed from np select logic and a protocol id
 * @imem_idx the next imem table entry to use to continue the parsing
 *           in case the combination of @logic and @proto_id is found
 */
struct np_sel_entry {
	u32 key;
	u16 imem_idx;
};

/**
 * @brief Next protocol select table key builder
 * @param logic next protocol select logic, see enum np_logic
 * @param proto_id protocol id
 */
#define PRSR_NP_SEL_KEY_BUILD(proto_id, logic)     \
	(u32)(PP_FIELD_PREP(GENMASK(15, 0),  proto_id) | \
	      PP_FIELD_PREP(GENMASK(19, 16), logic))

/**
 * @brief Skip table key builder
 * @param op ALU opcode, see enum alu_op
 * @param val value, expected value from the header
 */
#define PRSR_SKIP_KEY_BUILD(val, op)     \
	(u32)(PP_FIELD_PREP(GENMASK(15, 0),  val) | \
	      PP_FIELD_PREP(GENMASK(18, 16), op))

/**
 * @brief Skip table entry, a helper table to help the parser to find
 *        a protocol header length based on a @key extracted from the
 *        packet combined with a pre-defining @op, if such
 *             combination is found @hdr_len will be used to determine
 *             the protocol header length
 * @key the that should be extracted from the packet
 * @hdr_len the header length to use
 */
struct skip_entry {
	u16  key;
	u16  hdr_len;
};

/**
 * @brief Merge table entry, define which 2 consecutive protocols
 *        should be merged in the parser output.
 * @prev previous protocol imem index
 * @curr current protocol imem entry index
 * @merged imem index to use when both protocols appearconsecutively
 *         in a packet, MUST be either pp or cp
 */
struct merge_entry {
	u16 prev;
	u16 curr;
	u16 merged;
};

/**
 * @brief Pit table entry defining the data to extract from the
 *        protocol header and where it should be copied in the FV.
 * @fv_idx The place to copy the extracted data to in the field
 *      vector, byte index in the field vector, MUST be in
 *      2 bytes granularity.
 * @len extracted data length, number of bytes to extract, MUST be in
 *      2 bytes granularity
 * @off extracted data start offset from the start of the protocol
 *      header, MUST be in 2 bytes granularity
 * @mask extracted data bytes bitmap which specify which bytes from
 *       the extracted data should be masked, meaning, excluded from
 *       classification, bit N specify if byte N should be masked
 *       where '1' means to mask, '0' means not to mask
 * @example extract 10 bytes from offset 0 and copy to fv index 0<br>
 *          fv_idx = 0, len = 5, off = 0, mask 0<br>
 * @example extract 10 bytes from offset 1 and copy to fv index 0<br>
 *          fv_idx = 0, len = 6, off = 0, mask 0b100000000001<br>
 *          12 bytes will be copied to the fv but mask is used to mask
 *          bytes 0 and 11, note that 12 bytes MUST be used in
 *          the fv<br>
 * @example extract 9 bytes from offset 0 and copy to fv index 0<br>
 *          fv_idx = 0, len = 5, off = 0, mask = 0b1000000000<br> 10
 *          bytes will be copied to the fv but mask is used to mask
 *          byte 9, note that 10 bytes MUST be used in the fv<br>
 **/
struct pit_entry {
	u16 fv_idx;
	u16 len;
	u16 off;
	u32 mask;
};

/**
 * @brief Protocol database entry
 * @name protocol name
 * @valid secify if the entry is valid
 * @proto_id protocol identifier
 * @imem_idx imem entry index, may be invalid
 * @l2_idx layer 2 entry index, may be invalid
 * @pit pit table info
 * @id internal id
 * @np_map bitmap specifying which np select table entries are
 *        used by the protocol
 * @skip_map bitmap specifying which skip table entries are used
 *        by the protocol
 * @merge_map bitmap specifying which merge table entries are
 *        used by the protocol
 * @leth_map bitmap specifying which last ethertype table
 *        entries are used by the protocol
 */
struct proto_db_e {
	char name[PRSR_MAX_NAME];
	bool valid;
	u16  proto_id;
	u16  imem_idx;
	u16  l2_idx;
	struct pit_info    pit;
	enum prsr_proto_id id;
	ulong np_map[BITS_TO_LONGS(PRSR_NUM_NP_SELECT_ENTRIES) + 1];
	ulong skip_map[BITS_TO_LONGS(PRSR_NUM_SKIP_ENTRIES) + 1];
	ulong merge_map[BITS_TO_LONGS(PRSR_NUM_MERGE_ENTRIES) + 1];
	ulong leth_map[BITS_TO_LONGS(PRSR_NUM_LAST_ETHTYPE_ENTRIES) + 1];
};

/**
 * @brief Parser MAC protocol parameters definition
 * @hdr_len ethernet header length including only MAC addresses
 * @mac_name name to attach to the mac pit table entries
 * @mac_ext MAC addresses extract information
 * @ethertype_name last ethertype name to attach to the last ethertype
 *                 pit table entries
 * @ethtype_ext last ethertype extract info
 */
struct prsr_mac_proto_params {
	u16 hdr_len;
	char mac_name[PRSR_MAX_NAME];
	struct ext_info mac_ext;
	char ethtype_name[PRSR_MAX_NAME];
	struct ext_info ethtype_ext;
};

/**
 * @brief Profile protocol definition
 * @name protocol name
 * @id parser internal id
 * @prof_id porfile id specifying on which profile the protocol
 *        should be configured
 * @up upper layer protocol parameters
 * @l2 layer 2 protocol parameters
 * @mac mac protocol parameters
 */
struct protocol_info {
	char *name;
	enum prsr_proto_id id;
	enum prsr_prof_id  prof_id;
	union {
		struct prsr_up_layer_proto_params up;
		struct prsr_l2_proto_params       l2;
		struct prsr_mac_proto_params      mac;
	};
};

/**
 * @brief Protocols merge tuple, defines a pair of protocols which can
 *        appear in sequence in a packet and should be merge into 1
 *        protocol in the parser's output<br>
 *        The parser can include up to 6 protocols headers on his
 *        output, when more is needed, 2 protocols can be merged into
 *        1 using this struct.
 * @prof_id porfile id specifying on which profile the protocols
 *        should be merged
 * @prev previous protocol
 * @current current protocol
 * @merged protocol to use
 */
struct merge_profile_info {
	enum prsr_prof_id prof_id;
	struct merge_info info;
};

/**
 * @brief Parser protocols information, define all the protocols that
 *        are statically supported by the parser
 * @proto array of protocols
 * @n_proto number of valid protocols in @proto array, starting at 0
 * @merge protocols merges info, specify which pairs of protocols
 *        should merged
 * @n_merge number of valid entries in @merge array, starting at 0
 */
struct protocols {
	struct protocol_info      proto[PRSR_PROTOS_NUM];
	u16                       n_proto;
	struct merge_profile_info merge[PRSR_NUM_MERGE_ENTRIES];
	u16                       n_merge;
};

/**
 * @struct pkt_imem_flow
 * @brief Define a packet flow in the imem table
 * @ent_idx array specifying which entries were used to parse the
 *          packet, each entry in the array specify an index in the
 *          imem entry that was used to parse the packet
 * @note this structure is used for reading the last packet sequence
 *       from the HW
 */
struct pkt_imem_flow {
	u8 ent_idx[PRSR_DBG_IMEM_SEQ_MAX];
};

/**
 * @enum pp_stw_prsr_fld
 * @brief STW parser fields enumaration
 */
enum pp_stw_prsr_fld {
	STW_PRSR_FLD_FIRST = STW_COMMON_FLDS_NUM,
	STW_PRSR_FLD_TTL_EXPIRED = STW_PRSR_FLD_FIRST,
	STW_PRSR_FLD_IP_UNSUPP,
	STW_PRSR_FLD_EXT_DF,
	STW_PRSR_FLD_INT_DF,
	STW_PRSR_FLD_EXT_FRAG_TYPE,
	STW_PRSR_FLD_INT_FRAG_TYPE,
	STW_PRSR_FLD_TCP_FIN,
	STW_PRSR_FLD_TCP_SYN,
	STW_PRSR_FLD_TCP_RST,
	STW_PRSR_FLD_TCP_ACK,
	STW_PRSR_FLD_TCP_DATA_OFFSET,
	STW_PRSR_FLD_EXT_L3_OFFSET,
	STW_PRSR_FLD_INT_L3_OFFSET,
	STW_PRSR_FLD_EXT_L4_OFFSET,
	STW_PRSR_FLD_INT_L4_OFFSET,
	STW_PRSR_FLD_EXT_FRAG_INFO_OFFSET,
	STW_PRSR_FLD_INT_FRAG_INFO_OFFSET,
	STW_PRSR_FLD_L3_HDR_OFFSET_5,
	STW_PRSR_FLD_L3_HDR_OFFSET_4,
	STW_PRSR_FLD_L3_HDR_OFFSET_3,
	STW_PRSR_FLD_L3_HDR_OFFSET_2,
	STW_PRSR_FLD_L3_HDR_OFFSET_1,
	STW_PRSR_FLD_L3_HDR_OFFSET_0,
	STW_PRSR_FLD_ANA_PKT_TYPE,
	STW_PRSR_FLD_TUNN_INNER_OFFSET,
	STW_PRSR_FLD_PAYLOAD_OFFSET,
	STW_PRSR_FLD_ERROR_IND,
	STW_PRSR_FLD_LRU_EXP,
	STW_PRSR_FLD_RPB_CLID,
	STW_PRSR_FLD_DROP_PKT,
	STW_PRSR_FLD_LAST = STW_PRSR_FLD_DROP_PKT,
	STW_PRSR_FLDS_NUM
};

#define STW_PRSR_WORD_CNT                        (5 + STW_COMMON_WORD_CNT)

/**
 * @brief STW parser fields bits
 */
#define STW_PRSR_FLD_TTL_EXPIRED_LSB             (0  + 4 * BITS_PER_U32)
#define STW_PRSR_FLD_TTL_EXPIRED_MSB             (0  + 4 * BITS_PER_U32)
#define STW_PRSR_FLD_IP_UNSUPP_LSB               (1  + 4 * BITS_PER_U32)
#define STW_PRSR_FLD_IP_UNSUPP_MSB               (1  + 4 * BITS_PER_U32)
#define STW_PRSR_FLD_EXT_DF_LSB                  (2  + 4 * BITS_PER_U32)
#define STW_PRSR_FLD_EXT_DF_MSB                  (2  + 4 * BITS_PER_U32)
#define STW_PRSR_FLD_INT_DF_LSB                  (3  + 4 * BITS_PER_U32)
#define STW_PRSR_FLD_INT_DF_MSB                  (3  + 4 * BITS_PER_U32)
#define STW_PRSR_FLD_EXT_FRAG_TYPE_LSB           (4  + 4 * BITS_PER_U32)
#define STW_PRSR_FLD_EXT_FRAG_TYPE_MSB           (5  + 4 * BITS_PER_U32)
#define STW_PRSR_FLD_INT_FRAG_TYPE_LSB           (6  + 4 * BITS_PER_U32)
#define STW_PRSR_FLD_INT_FRAG_TYPE_MSB           (7  + 4 * BITS_PER_U32)
#define STW_PRSR_FLD_TCP_FIN_LSB                 (8  + 4 * BITS_PER_U32)
#define STW_PRSR_FLD_TCP_FIN_MSB                 (8  + 4 * BITS_PER_U32)
#define STW_PRSR_FLD_TCP_SYN_LSB                 (9  + 4 * BITS_PER_U32)
#define STW_PRSR_FLD_TCP_SYN_MSB                 (9  + 4 * BITS_PER_U32)
#define STW_PRSR_FLD_TCP_RST_LSB                 (10 + 4 * BITS_PER_U32)
#define STW_PRSR_FLD_TCP_RST_MSB                 (10 + 4 * BITS_PER_U32)
#define STW_PRSR_FLD_TCP_ACK_LSB                 (11 + 4 * BITS_PER_U32)
#define STW_PRSR_FLD_TCP_ACK_MSB                 (11 + 4 * BITS_PER_U32)
#define STW_PRSR_FLD_TCP_DATA_OFFSET_LSB         (12 + 4 * BITS_PER_U32)
#define STW_PRSR_FLD_TCP_DATA_OFFSET_MSB         (15 + 4 * BITS_PER_U32)
#define STW_PRSR_FLD_EXT_L3_OFFSET_LSB           (16 + 4 * BITS_PER_U32)
#define STW_PRSR_FLD_EXT_L3_OFFSET_MSB           (23 + 4 * BITS_PER_U32)
#define STW_PRSR_FLD_INT_L3_OFFSET_LSB           (24 + 4 * BITS_PER_U32)
#define STW_PRSR_FLD_INT_L3_OFFSET_MSB           (31 + 4 * BITS_PER_U32)
#define STW_PRSR_FLD_EXT_L4_OFFSET_LSB           (0  + 5 * BITS_PER_U32)
#define STW_PRSR_FLD_EXT_L4_OFFSET_MSB           (7  + 5 * BITS_PER_U32)
#define STW_PRSR_FLD_INT_L4_OFFSET_LSB           (8  + 5 * BITS_PER_U32)
#define STW_PRSR_FLD_INT_L4_OFFSET_MSB           (15 + 5 * BITS_PER_U32)
#define STW_PRSR_FLD_EXT_FRAG_INFO_OFFSET_LSB    (16 + 5 * BITS_PER_U32)
#define STW_PRSR_FLD_EXT_FRAG_INFO_OFFSET_MSB    (23 + 5 * BITS_PER_U32)
#define STW_PRSR_FLD_INT_FRAG_INFO_OFFSET_LSB    (24 + 5 * BITS_PER_U32)
#define STW_PRSR_FLD_INT_FRAG_INFO_OFFSET_MSB    (31 + 5 * BITS_PER_U32)
#define STW_PRSR_FLD_L3_HDR_OFFSET_5_LSB         (0  + 6 * BITS_PER_U32)
#define STW_PRSR_FLD_L3_HDR_OFFSET_5_MSB         (7  + 6 * BITS_PER_U32)
#define STW_PRSR_FLD_L3_HDR_OFFSET_4_LSB         (8  + 6 * BITS_PER_U32)
#define STW_PRSR_FLD_L3_HDR_OFFSET_4_MSB         (15 + 6 * BITS_PER_U32)
#define STW_PRSR_FLD_L3_HDR_OFFSET_3_LSB         (16 + 6 * BITS_PER_U32)
#define STW_PRSR_FLD_L3_HDR_OFFSET_3_MSB         (23 + 6 * BITS_PER_U32)
#define STW_PRSR_FLD_L3_HDR_OFFSET_2_LSB         (24 + 6 * BITS_PER_U32)
#define STW_PRSR_FLD_L3_HDR_OFFSET_2_MSB         (31 + 6 * BITS_PER_U32)
#define STW_PRSR_FLD_L3_HDR_OFFSET_1_LSB         (0  + 7 * BITS_PER_U32)
#define STW_PRSR_FLD_L3_HDR_OFFSET_1_MSB         (7  + 7 * BITS_PER_U32)
#define STW_PRSR_FLD_L3_HDR_OFFSET_0_LSB         (8  + 7 * BITS_PER_U32)
#define STW_PRSR_FLD_L3_HDR_OFFSET_0_MSB         (15 + 7 * BITS_PER_U32)
#define STW_PRSR_FLD_ANA_PKT_TYPE_LSB            (16 + 7 * BITS_PER_U32)
#define STW_PRSR_FLD_ANA_PKT_TYPE_MSB            (23 + 7 * BITS_PER_U32)
#define STW_PRSR_FLD_TUNN_INNER_OFFSET_LSB       (24 + 7 * BITS_PER_U32)
#define STW_PRSR_FLD_TUNN_INNER_OFFSET_MSB       (26 + 7 * BITS_PER_U32)
#define STW_PRSR_FLD_PAYLOAD_OFFSET_LSB          (27 + 7 * BITS_PER_U32)
#define STW_PRSR_FLD_PAYLOAD_OFFSET_MSB          (29 + 7 * BITS_PER_U32)
#define STW_PRSR_FLD_ERROR_IND_LSB               (30 + 7 * BITS_PER_U32)
#define STW_PRSR_FLD_ERROR_IND_MSB               (30 + 7 * BITS_PER_U32)
#define STW_PRSR_FLD_LRU_EXP_LSB                 (31 + 7 * BITS_PER_U32)
#define STW_PRSR_FLD_LRU_EXP_MSB                 (31 + 7 * BITS_PER_U32)
#define STW_PRSR_FLD_RPB_CLID_LSB                (0  + 8 * BITS_PER_U32)
#define STW_PRSR_FLD_RPB_CLID_MSB                (9  + 8 * BITS_PER_U32)
#define STW_PRSR_FLD_DROP_PKT_LSB                (10 + 8 * BITS_PER_U32)
#define STW_PRSR_FLD_DROP_PKT_MSB                (12 + 8 * BITS_PER_U32)

/**
 * @brief STW parser fields description
 */
#define STW_PRSR_FLD_TTL_EXPIRED_DESC            "TTL Expired"
#define STW_PRSR_FLD_IP_UNSUPP_DESC              "IP Unsupported"
#define STW_PRSR_FLD_EXT_DF_DESC                 "External Don't Frag"
#define STW_PRSR_FLD_INT_DF_DESC                 "Internal Don't Frag"
#define STW_PRSR_FLD_EXT_FRAG_TYPE_DESC          "External Frag Type"
#define STW_PRSR_FLD_INT_FRAG_TYPE_DESC          "Internal Frag Type"
#define STW_PRSR_FLD_TCP_FIN_DESC                "TCP Fin"
#define STW_PRSR_FLD_TCP_SYN_DESC                "TCP Syn"
#define STW_PRSR_FLD_TCP_RST_DESC                "TCP Reset"
#define STW_PRSR_FLD_TCP_ACK_DESC                "TCP Ack"
#define STW_PRSR_FLD_TCP_DATA_OFFSET_DESC        "TCP Data Offset"
#define STW_PRSR_FLD_EXT_L3_OFFSET_DESC          "External L3 Offset"
#define STW_PRSR_FLD_INT_L3_OFFSET_DESC          "Internal L3 Offset"
#define STW_PRSR_FLD_EXT_L4_OFFSET_DESC          "External L4 Offset"
#define STW_PRSR_FLD_INT_L4_OFFSET_DESC          "Internal L4 Offset"
#define STW_PRSR_FLD_EXT_FRAG_INFO_OFFSET_DESC   "External Frag Info Offset"
#define STW_PRSR_FLD_INT_FRAG_INFO_OFFSET_DESC   "Internal Frag Info Offset"
#define STW_PRSR_FLD_L3_HDR_OFFSET_5_DESC        "L3 Header Offset 5"
#define STW_PRSR_FLD_L3_HDR_OFFSET_4_DESC        "L3 Header Offset 4"
#define STW_PRSR_FLD_L3_HDR_OFFSET_3_DESC        "L3 Header Offset 3"
#define STW_PRSR_FLD_L3_HDR_OFFSET_2_DESC        "L3 Header Offset 2"
#define STW_PRSR_FLD_L3_HDR_OFFSET_1_DESC        "L3 Header Offset 1"
#define STW_PRSR_FLD_L3_HDR_OFFSET_0_DESC        "L3 Header Offset 0"
#define STW_PRSR_FLD_ANA_PKT_TYPE_DESC           "Analyzer Packet Type"
#define STW_PRSR_FLD_TUNN_INNER_OFFSET_DESC      "Tunnel Inner Offset"
#define STW_PRSR_FLD_PAYLOAD_OFFSET_DESC         "Payload Offset"
#define STW_PRSR_FLD_ERROR_IND_DESC              "Error"
#define STW_PRSR_FLD_LRU_EXP_DESC                "LRU Exception"
#define STW_PRSR_FLD_RPB_CLID_DESC               "RPB CLID"
#define STW_PRSR_FLD_DROP_PKT_DESC               "Drop"

/**
 * @struct prsr_hw_stw
 * @brief parser debug status word registers definition
 */
struct prsr_hw_stw {
	u32 word[STW_PRSR_WORD_CNT];
};

/**
 * @brief Layer 2 compare operation description
 */
extern const char * const l2_cmp_op_str[L2_CMP_OP_NUM];
/**
 * @brief NP logic description
 */
extern const char * const np_logic_str[NP_LOGICS_NUM];
/**
 * @brief IMEM entries type descriptions
 */
extern const char * const etype_str[ETYPE_NUM];
/**
 * @brief Skip logic description
 */
extern const char * const skip_logic_str[SKIP_LOGICS_NUM];
/**
 * @brief Parser HW error flags description
 */
extern const char * const err_flag_str[PRSR_NUM_ERR_FLAGS];
/**
 * @brief Field vector protocol information description
 */
extern const char * const proto_info_str[PP_FV_PROTO_INFO_MAX];

/**
 * @brief Get parser driver table context
 * @param idx table index
 * @return struct table* table context on success, NULL otherwise
 */
struct table *prsr_get_tbl(u32 idx);

/**
 * @brief Set MAC configuration to HW registers
 * @param len MAC addresses total length
 * @param pit PRSR_PROTO_MAC protocol pit table info
 * @return s32 0 on success, error code otherwise
 */
s32 prsr_mac_cfg_set(u8 len, struct pit_info *pit);

/**
 * @brief Get MAC configuration from HW registers
 * @param len pointer to save MAC addresses length
 * @param pit pointer to save MAC's PIT info
 * @return s32 0 on success, error code otherwise
 */
s32 prsr_mac_cfg_get(u16 *len, struct pit_info *pit);

/**
 * @brief Set MAC ethertype configuration to HW registers
 * @note
 * @param pit ethertype pit table info
 * @return s32 0 on success, error code otherwise
 */
s32 prsr_ethertype_cfg_set(struct pit_info *pit);

/**
 * @brief Get MAC ethertype configuration from HW registers
 * @note
 * @param pit ethertype pit table info
 * @return s32 0 on success, error code otherwise
 */
s32 prsr_ethertype_cfg_get(u16 *dflt_pc, struct pit_info *pit);

/**
 * @brief Get a shallow parser database protocol info
 * @note for debug only
 * @param id protocol parser private id
 * @return reference to the protocol info, NULL in case of error
 */
const struct proto_db_e *prsr_proto_info_get(enum prsr_proto_id id);

/**
 * @brief Create protocols array, the array is being allocated
 *        dynamically, it is the caller responsibility to free the
 *        memory even in case or error.
 * @param protocols pointer to store the array pointer
 * @return s32 0 on success, error code otherwise
 */
s32 __prsr_protocols_init(struct protocols *protos);

/**
 * @brief Set PIT table entry to HW registers
 * @param idx PIT table entry index where to use
 * @param ent entry info, use NULL to invalidate the entry in hw
 * @return s32 0 in success, error code otherwise
 */
s32 prsr_pit_entry_set(u16 idx, const struct pit_entry *ent);

/**
 * @brief Get PIT table entry from HW registers
 * @param idx PIT table entry index to read
 * @param e pointer to write the entry to
 * @return s32 0 on success, error code otherwise
 */
s32 prsr_pit_entry_get(u16 idx, struct pit_entry *ent);

/**
 * @brief Set port configuration entry to HW registers
 * @param cfg port configuration
 * @return s32 0 on success, error code otherwise
 */
s32 prsr_port_cfg_entry_set(u16 port, struct port_cfg_entry *cfg);

/**
 * @brief Get port configuration entry from HW registers
 * @param cfg port configuration buffer
 * @return s32 0 on success, error code otherwise
 */
s32 prsr_port_cfg_entry_get(u16 port, struct port_cfg_entry *cfg);

/**
 * @brief Layer 2 table entry set to HW registers
 * @param idx table index
 * @param l2 entry info
 * @param pit entry pit table info
 * @note use NULL pointers to invalidate the entry in hw
 * @return s32 0 on success, error code otherwise
 */
s32 prsr_l2_entry_set(u32 idx, const struct l2_entry *l2,
		      const struct pit_info *pit);

/**
 * @brief Get Layer 2 table entry info from HW registers
 * @param idx table index
 * @param valid valid status buffer
 * @param l2 entry info buffer
 * @param pit entry pit table info buffer
 * @return s32 0 on success, error code otherwise
 */
s32 prsr_l2_entry_get(u16 idx, bool *valid, struct l2_entry *l2,
		      struct pit_info *pit);

/**
 * @brief Set last ethertype entry to HW registers
 * @param idx entry index
 * @param valid specify whether to validate or invalidate the entry
 * @param ent entry info, use NULL to invalidate the entry in hw
 * @return s32 0 on success, error code otherwise
 */
s32 prsr_last_ethertype_entry_set(u16 idx,
				  const struct last_ethertype_entry *ent);

/**
 * @brief Get last ethertype entry from HW registers
 * @param idx entry index
 * @param validity status buffer
 * @param e entry buffer
 * @return s32 0 on success, error code otherwise
 */
s32 prsr_last_ethertype_entry_get(u16 idx, bool *valid,
				  struct last_ethertype_entry *e);

/**
 * @brief Set IMEM entry to HW registers
 * @param idx entry index
 * @param ent entry info, use NULL to invalidate the entry in hw
 * @return s32 0 on success, error code otherwise
 */
s32 prsr_imem_entry_set(u16 idx, const struct imem_entry *ent);

/**
 * @brief Get IMEM entry from HW registers
 * @param idx entry index
 * @param e entry buffer
 * @return s32 0 on success, error code otherwise
 */
s32 prsr_imem_entry_get(u16 idx, struct imem_entry *e);

/**
 * @brief Set next protocol select table entry to HW registers
 * @param idx entry index
 * @param ent entry info, use NULL to invalidate the entry in hw
 */
s32 prsr_np_sel_entry_set(u16 idx, const struct np_sel_entry *ent);

/**
 * @brief Get next protocol select entry from HW registers
 * @param idx entry index
 * @param valid specify if the entry is valid
 * @param e entry buffer
 * @return s32 0 on success, error code otherwise
 */
s32 prsr_np_sel_entry_get(u16 idx, bool *valid, struct np_sel_entry *e);

/**
 * @brief Set next protocol select table entry to HW registers
 * @param idx entry index
 * @param ent entry info, use NULL to invalidate the entry in hw
 * @return s32 0 on success, error code otherwise
 */
s32 prsr_skip_entry_set(u16 idx, const struct skip_entry *ent);

/**
 * @brief Get skip cam entry from HW registers
 * @param idx entry index
 * @param valid specify if the entry is valid
 * @param e entry buffer
 * @return s32 0 on success, error code otherwise
 */
s32 prsr_skip_entry_get(u16 idx, bool *valid, struct skip_entry *e);

/**
 * @brief Set merge table entry to HW registers
 * @param idx entry index
 * @param ent entry info, use NULL to invalidate the entry in hw
 * @return s32 0 on success, error code otherwise
 */
s32 prsr_merge_entry_set(u16 idx, const struct merge_entry *ent);

/**
 * @brief Get merge table entry from HW registers
 * @param idx entry index
 * @param valid specify if the entry is valid
 * @param e entry buffer
 * @return s32 0 on success, error code otherwise
 */
s32 prsr_merge_entry_get(u16 idx, bool *valid, struct merge_entry *e);
/**
 * @brief Set parser HW default IMEM entry to start parsing process in
 *        case where last ethertype is not found in the last ethertype
 *        table
 * @param id internal id to set as the default
 * @return s32 0 on success, error code otherwise
 */
s32 prsr_dflt_ethertype_set(enum prsr_proto_id id);

/**
 * @brief Get default protocol used in case last ethertype isn't found
 *        in the last ethertype table
 * @param id
 * @return s32 0 on success, error code otherwise
 */
s32 prsr_dflt_ethertype_get(enum prsr_proto_id *id);

/**
 * @brief Set the parser field vector output swapping.
 * @param swap specify whether to swap the field vector before on the
 *             output
 */
void prsr_fv_swap_set(bool swap);
/**
 * @brief Set parser's fv mask swapping
 * @param swap specify whether to swap the field vector mask before
 *             using it to mask the field vector
 */
void prsr_fv_mask_swap_set(bool swap);
/**
 * @brief The parser output the field vector in 2 blocks of 512, it
 *        can be configured to swap the order of the blocks on the
 *        output.
 * @param swap specify whether to swap the blocks or not
 */
void prsr_fv_blocks_swap_set(bool swap);
/**
 * @brief Get parser field vector output swap status
 * @return bool swap status
 */
bool prsr_fv_swap_get(void);
/**
 * @brief Get parser field vector mask swap status
 * @return bool swap status
 */
bool prsr_fv_mask_swap_get(void);
/**
 * @brief Get parser field vector blocks output swap status
 * @return bool swap status
 */
bool prsr_fv_blocks_swap_get(void);
/**
 * @brief Enable/Disable field vector masking on the output
 * @param mask_en specify whether to enable or disable field vector
 */
void prsr_fv_mask_en_set(bool mask_en);
/**
 * @brief Get fv mask enable/disable status
 * @return bool fv mask status
 */
bool prsr_fv_mask_en_get(void);

/**
 * @brief Returns a bitmap specifying which imem table entries was hit
 *        since startup
 * @return u64 imem table entries hit map
 */
u64 prsr_imem_hit_map_get(void);

/**
 * @brief Returns the last packet flow in the imem entry, which imem
 *        entries were used to parse the last packet
 * @param flow buffer to return the flow
 * @return s32 0 on success, error code otherwise
 */
s32 prsr_imem_last_pkt_flow_get(struct pkt_imem_flow *flow);

/**
 * @brief Returns a bitmap specifying which l2 table entries was hit
 *        since startup
 * @return u64 l2 table entries hit map
 */
u32 prsr_l2_table_hit_map_get(void);

/**
 * @brief Returns a bitmap specifying which l2 table entries was hit
 *        by the last packet
 * @return u64 l2 table entries hit map
 */
u32 prsr_l2_table_last_hit_map_get(void);

/**
 * @brief Returns the last field vector of the parser
 * @param fv unmasked field vector
 * @param masked_fv masked field vector
 */
void prsr_last_fv_get(struct pp_fv *fv, struct pp_fv *masked_fv);

/**
 * @brief Get debug status word, the status word returned here is from
 *        the egress of the parser HW
 * @param stw status word buffer
 * @return s32 0 on success, error code otherwise
 */
s32 prsr_last_stw_get(struct prsr_hw_stw *hw_stw);

/**
 * @brief Reset status word stickiness to allow debug register
 *        to reflect most recent status word info to be stored
 */
void prsr_stw_sticky_reset(void);

/**
 * @brief Adds new upper level protocol to be PP parser
 *        Caller must be familiar with 'struct pp_field_vector'
 * @param id protocol internal parser driver id
 * @param dbgfs_add specify if debugfs files should be created
 * @param params protocol parameters
 * @return s32 0 on success, error code on failure
 */
s32 __prsr_up_layer_proto_add(enum prsr_proto_id id, bool dbgfs_add,
			      const struct prsr_up_layer_proto_params *params);

/**
 * @brief Allocate parser internal protocol id
 * @param id
 * @note the id will be free once the protocol will be removed
 * @return s32 0 on success, error code otherwise
 */
s32 prsr_proto_id_alloc(enum prsr_proto_id *id);

/**
 * @brief Delete a protocol from parser driver and hw
 * @param id protocol internal id
 * @param dbgfs_del specify whether or not to delete the protocol
 *                  debugfs files
 * @note for internal use only
 * @return s32 0 on success, error code otherwise
 */
s32 __prsr_proto_del(enum prsr_proto_id id, bool dbgfs_del);

/**
 * @brief Add/Overwrite the specified hw error/abort/drop bitmap flags
 *        to the existing one.
 * @param err_flags
 * @param overwrite specify whether to overwrite the existing bitmap
 *                  with the new one or just add it
 * @return 0 on success, error code otherwise
 */
void prsr_hw_err_flags_set(u32 err_flags, bool overwrite);

/**
 * @brief Get hw error/drop/abort flags
 * @return u32 the error flags
 */
u32 prsr_hw_err_flags_get(void);

/**
 * @brief hw error init
 */
void prsr_hw_err_flags_init(void);

/**
 * @brief Set packets maximum header length allowed for parsing,
 *        packets with header size exceeding the maximum will be
 *        aborted/dropped or marked with an error flag
 * @param hdr_len header length
 * @return s32 0 on success, error code otherwise
 */
s32 prsr_max_hdr_len_set(u32 hdr_len);

/**
 * @brief Get packets maximum header length allowed
 * @return u32 the maximum header length
 */
u32 prsr_max_hdr_len_get(void);

/**
 * @brief Set imem protocol maximum header length allowed for parsing,
 *        packets with header size exceeding the maximum will be
 *        aborted/dropped or marked with an error flag
 * @note this is a maximum which is apply to all imem entries
 * @param hdr_len
 * @return s32 0 on success, error code otherwise
 */
s32 prsr_max_imem_proto_hdr_len_set(u32 hdr_len);

/**
 * @brief Get imem protocol maximum header length allowed.
 * @return u32 the maximum header length
 */
u32 prsr_max_imem_proto_hdr_len_get(void);

/**
 * @brief Set prefetched header length, the parser will not be able to
 *        access packet data at offset greater than this value, in
 *        such case the packet will aborted
 * @param hdr_len the maximum to set
 * @return s32 0 on success, error code otherwise
 */
s32 prsr_prefetch_hdr_len_set(u32 hdr_len);

/**
 * @brief Get maximum prefetched header length
 * @return u32 the maximum header length
 */
u32 prsr_prefetch_hdr_len_get(void);

/**
 * @brief Get Enable/Disable support status for all IPv6 next headers
 *        values in a bitmap form
 * @param nexthdr_map the bitmap
 * @param map_sz bitmap size, must be equal to NEXTHDR_MAX
 * @return u32 0 on success, false otherwise
 */
s32 prsr_ipv6_nexthdr_support_get_all(unsigned long *nexthdr_map, u32 map_sz);

#ifdef CONFIG_DEBUG_FS
/**
 * @brief Parser debug init
 * @return s32
 */
s32 prsr_dbg_init(struct dentry *parent);

/**
 * @brief Parser debug cleanup
 * @return s32
 */
s32 prsr_dbg_clean(void);

/**
 * @brief Create protocol debugfs files
 * @param id protocol id
 * @return s32 0 on success, error code otherwise
 */
s32 prsr_dbg_proto_add(enum prsr_proto_id id);

/**
 * @brief Delete protocol debugfs files
 * @param id protocol id
 * @return s32 0 on success, error code otherwise
 */
s32 prsr_dbg_proto_del(enum prsr_proto_id id);

/**
 * @brief Append a length-limited, C-string 'name' to the current
 *        entry name.
 * @param tbl table context
 * @param idx the entry index within the table
 * @param name the new description to append
 * @param sep separator
 * @param before specify if 'name' should be added at the to start or
 *               to the end
 * @return s32 0 in case the new description was successfully
 *         appended, error code otherwise
 */
s32 table_entry_name_append(struct table *tbl, u16 idx, const char *name,
			    char *sep, bool before);
/**
 * @brief Remove a C-string 'name' from an entry name
 * @param tbl table context
 * @param idx entry index
 * @param name name to remove
 * @return s32 0 on success, error code otherwise
 */
s32 table_entry_name_remove(struct table *tbl, u16 idx, const char *name);

#else
static inline s32 prsr_dbg_init(struct dentry *parent)
{
	return 0;
}

static inline s32 prsr_dbg_clean(void)
{
	return 0;
}

static inline s32 prsr_dbg_proto_add(enum prsr_proto_id id)
{
	return 0;
}

static inline s32 prsr_dbg_proto_del(enum prsr_proto_id id)
{
	return 0;
}

static inline s32 table_entry_name_append(struct table *tbl, u16 idx,
					  const char *name, char *sep,
					  bool before)
{
	return 0;
}

static inline s32 table_entry_name_remove(struct table *tbl, u16 idx,
					  const char *name)
{
	return 0;
}

#endif
#endif /* __PARSER_INTERNAL_H__ */
