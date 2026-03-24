/*
 * DirectConnect provides a common interface for the network devices to
 * achieve the full or partial acceleration services from the underlying
 * packet acceleration engine Copyright (c) 2017, Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 */

#ifndef _DIRECTCONNECT_DP_PARSER_H_
#define _DIRECTCONNECT_DP_PARSER_H_

/*
 * ========================================================================
 * Includes
 * ========================================================================
 */
#include <dc_mode_sw_common.h>

/*
 * ========================================================================
 * Static Declarations
 * ========================================================================
 */
static inline bool
dc_dp_pad_is_available(struct sk_buff *skb)
{
    return (skb->len == DCDP_SHORT_PKT_LEN);
}

/*
 * ========================================================================
 * Extern Declarations
 * ========================================================================
 */
extern void
dc_dp_parser_remove_pad(struct sk_buff *skb);

extern void
dc_dp_parser_dump_proto(struct seq_file *seq);

extern void
dc_dp_parser_add_proto(uint16_t proto,
			uint32_t payload_start_offset,
			uint32_t payloadlen_start_offset, uint32_t pyld_len);

extern void
dc_dp_parser_del_proto(uint16_t protocol);

extern void
dc_dp_parser_init(void);

extern void
dc_dp_parser_exit(void);

#endif  /* _DIRECTCONNECT_DP_PARSER_H_ */
