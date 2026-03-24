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

/* Includes */
#include <linux/module.h>
#include <linux/netdevice.h>
#include <linux/skbuff.h>
#include <linux/if_vlan.h>

#include "directconnect_dp_parser.h"

/* Defines */
struct protodb_entry {
	uint16_t proto;
	uint16_t pyld_start; /* From where to read the payload */
	uint16_t pyldlen_offset; /* From where to read the 'header' length */
	uint16_t pyld_len;
	uint32_t counter;
	struct list_head list;
	struct rcu_head rcu_head;
};

/*
 * ========================================================================
 * Global Variables
 * ========================================================================
 */

struct list_head g_protodb_list;

/*
 * ========================================================================
 * Local Interface API
 * ========================================================================
 */
static int
skip_l2_hdrs(struct ethhdr *eth, uint16_t *proto)
{
	int len = 0;
	uint8_t *ptr;
	uint16_t next_proto;

	next_proto  = (eth)->h_proto;
	ptr = (uint8_t *)(eth) + sizeof(struct ethhdr);
	len = sizeof(struct ethhdr);

	while (eth_type_vlan(next_proto)) { //qinq ? 0x9100?
		ptr += 2;
		next_proto = *(uint16_t *)ptr;
		ptr += 2;
		len += 4;
	}
	*proto = next_proto;
	return len;
}

static void
parser_trim_pkt(struct sk_buff *skb, struct protodb_entry *entry,
			uint32_t len)
{
	unsigned char *ptr;
	uint32_t proto_len = 0;

	/* Jump to start of L3 header */
	ptr = (unsigned char *)skb->data + len;

	/* Jump to payload length address using databse information */
	ptr += entry->pyldlen_offset;
	switch (entry->pyld_len) {
	case 1:
		proto_len = (uint8_t)(*ptr);
		break;
	case 2:
		proto_len = ntohs(*(uint16_t *)ptr);
		break;
	case 4:
		proto_len = ntohl(*(uint32_t *)ptr);
		break;
	default:
		break;
	}

	/* Update len */
	len += entry->pyld_start;
	len += proto_len;

	/* Remove padding */
	if (len < skb->len) {
		pr_debug("Protocol (0x%X) Original packet length: %d"
			" trim length: %d\n", entry->proto, len,
			skb->len - len);
		entry->counter++;
		__skb_trim(skb, len);
	}
}

void
dc_dp_parser_remove_pad(struct sk_buff *skb)
{
	struct protodb_entry *curr_entry;
	struct ethhdr *eth = NULL;
	uint16_t len = 0, proto;

	eth = (struct ethhdr *)skb->data;
	if (!eth)
		return;

	/* Parse 'Ethernet + Vlan (if any)' */
	len = skip_l2_hdrs(eth, &proto);

	/* Search for the protocol */
	rcu_read_lock();
	list_for_each_entry_rcu(curr_entry, &g_protodb_list, list) {
		if (curr_entry->proto == proto) {
			parser_trim_pkt(skb, curr_entry, len);
			break;
		}
	}
	rcu_read_unlock();
}

static struct protodb_entry*
protodb_find(uint16_t protocol)
{
	struct protodb_entry *curr_node;

	list_for_each_entry(curr_node, &g_protodb_list, list) {
		if (curr_node->proto == protocol)
			return curr_node;
	}
	return NULL;
}

void
dc_dp_parser_add_proto(uint16_t proto,
			uint32_t payload_start_offset,
			uint32_t payloadlen_start_offset, uint32_t pyld_len)
{
	struct protodb_entry *node = NULL;

	node = protodb_find(proto);
	if (node) {
		pr_err("Entry 0x%X already present\n", proto);
		return;
	}

	/* Not found. So add */
	node = kmalloc(sizeof(struct protodb_entry), GFP_ATOMIC);
	if (unlikely(!node)) {
		pr_err("%s(): Got OOM in kmalloc\n", __func__);
		return;
	}
	node->proto = proto;
	node->pyld_start = payload_start_offset;
	node->pyldlen_offset = payloadlen_start_offset;
	node->pyld_len = pyld_len;
	node->counter = 0;
	list_add_tail_rcu(&node->list, &g_protodb_list);
}

void
dc_dp_parser_del_proto(uint16_t protocol)
{
	struct protodb_entry *node = NULL;

	node = protodb_find(protocol);
	if (!node) {
		pr_err("Entry 0x%X not present\n", protocol);
		return;
	}

	/* Found. So remove */
	list_del_rcu(&node->list);
	kfree_rcu(node, rcu_head);
}

void
dc_dp_parser_dump_proto(struct seq_file *seq)
{
	struct protodb_entry *curr = NULL;

	seq_printf(seq, "+-------------+-------------+---------------+----------------+---------------+\n");
	seq_printf(seq, "|  Protocol   |   Pyld Off  | Pyld len Off  |  Pyld len size |      Stats    |\n");
	seq_printf(seq, "+-------------+-------------+---------------+----------------+---------------+\n");

	list_for_each_entry_rcu(curr, &g_protodb_list, list) {
		seq_printf(seq, "|    0x%X   | %10u  |  %10u   |   %10u   |  %10u   |\n", curr->proto,
				curr->pyld_start, curr->pyldlen_offset, curr->pyld_len, curr->counter);
	}
	seq_printf(seq, "+-------------+-------------+---------------+----------------+---------------+\n");
}

/* Add some pre-defined protocols in database list */
void
dc_dp_parser_init()
{
	INIT_LIST_HEAD(&g_protodb_list);
	dc_dp_parser_add_proto(0x8863, 6, 4, 2); /* PPPOED */
	dc_dp_parser_add_proto(0x8864, 6, 4, 2); /* PPP PAP */
	dc_dp_parser_add_proto(0x887b, 3, 2, 1); /* HOMEPLUG */
}

void
dc_dp_parser_exit()
{
	struct protodb_entry *entry, *tmp_entry;

	list_for_each_entry_safe(entry, tmp_entry, &g_protodb_list, list) {
		list_del(&entry->list);
		kfree(entry);
	}
}
