/*
 * Copyright (c) 2018 Chion Tang <tech@chionlab.moe>
 * Copyright (c) 2019 lean <coolsnowwolf@gmail.com>
 * Copyright (c) 2021 paldier <paldier@hotmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 * 
 * qca/lantiq/mtk/bcm ported by lean and paldier
 */


#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include <xtables.h>
#include <limits.h> /* INT_MAX in ip_tables.h */
#include <linux/netfilter_ipv4/ip_tables.h>
#if !defined(CONFIG_LANTIQ)
#include <net/netfilter/nf_conntrack_tuple.h>
#endif
#if defined(BCMARM) && !defined(HND_ROUTER)
#include <net/netfilter/nf_nat.h>
#else
#include <linux/netfilter/nf_nat.h>
#endif

#ifndef NF_NAT_RANGE_PROTO_RANDOM_FULLY
#define NF_NAT_RANGE_PROTO_RANDOM_FULLY 16
#endif

enum {
	O_TO_PORTS = 0,
	O_RANDOM,
	O_RANDOM_FULLY,
};

static void FULLCONENAT_help(void)
{
	printf(
"FULLCONENAT target options:\n"
" --to-ports <port>[-<port>]\n"
"				Port (range) to map to.\n"
" --random\n"
"				Randomize source port.\n"
" --random-fully\n"
"				Fully randomize source port.\n");
}

static const struct xt_option_entry FULLCONENAT_opts[] = {
	{.name = "to-ports", .id = O_TO_PORTS, .type = XTTYPE_STRING},
	{.name = "random", .id = O_RANDOM, .type = XTTYPE_NONE},
	{.name = "random-fully", .id = O_RANDOM_FULLY, .type = XTTYPE_NONE},
	XTOPT_TABLEEND,
};

static void FULLCONENAT_init(struct xt_entry_target *t)
{
#if defined(BCMARM) && !defined(HND_ROUTER)
	struct nf_nat_multi_range_compat *mr = (struct nf_nat_multi_range_compat *)t->data;
#else
	struct nf_nat_ipv4_multi_range_compat *mr = (struct nf_nat_ipv4_multi_range_compat *)t->data;
#endif

	/* Actually, it's 0, but it's ignored at the moment. */
	mr->rangesize = 1;
}

/* Parses ports */
static void
#if defined(BCMARM) && !defined(HND_ROUTER)
parse_ports(const char *arg, struct nf_nat_multi_range_compat *mr)
#else
parse_ports(const char *arg, struct nf_nat_ipv4_multi_range_compat *mr)
#endif
{
	char *end;
	unsigned int port, maxport;
#if defined(BCMARM) && !defined(HND_ROUTER)
	mr->range[0].flags |= IP_NAT_RANGE_PROTO_SPECIFIED;
#else
	mr->range[0].flags |= NF_NAT_RANGE_PROTO_SPECIFIED;
#endif
	if (!xtables_strtoui(arg, &end, &port, 0, UINT16_MAX))
		xtables_param_act(XTF_BAD_VALUE, "FULLCONENAT", "--to-ports", arg);

	switch (*end) {
	case '\0':
#if defined(BCMARM) && !defined(HND_ROUTER)
		mr->range[0].min.tcp.port
			= mr->range[0].max.tcp.port
			= htons(port);
#else
		mr->range[0].min.tcp.port
			= mr->range[0].max.tcp.port
			= htons(port);
#endif
		return;
	case '-':
		if (!xtables_strtoui(end + 1, NULL, &maxport, 0, UINT16_MAX))
			break;

		if (maxport < port)
			break;

		mr->range[0].min.tcp.port = htons(port);
		mr->range[0].max.tcp.port = htons(maxport);
		return;
	default:
		break;
	}
	xtables_param_act(XTF_BAD_VALUE, "FULLCONENAT", "--to-ports", arg);
}

static void FULLCONENAT_parse(struct xt_option_call *cb)
{
	const struct ipt_entry *entry = cb->xt_entry;
	int portok;
#if defined(BCMARM) && !defined(HND_ROUTER)
	struct nf_nat_multi_range_compat *mr = cb->data;
#else
	struct nf_nat_ipv4_multi_range_compat *mr = cb->data;
#endif
	if (entry->ip.proto == IPPROTO_TCP
	    || entry->ip.proto == IPPROTO_UDP
	    || entry->ip.proto == IPPROTO_SCTP
	    || entry->ip.proto == IPPROTO_DCCP
	    || entry->ip.proto == IPPROTO_ICMP)
		portok = 1;
	else
		portok = 0;

	xtables_option_parse(cb);
	switch (cb->entry->id) {
	case O_TO_PORTS:
		if (!portok)
			xtables_error(PARAMETER_PROBLEM,
				   "Need TCP, UDP, SCTP or DCCP with port specification");
		parse_ports(cb->arg, mr);
		break;
	case O_RANDOM:
#if defined(BCMARM) && !defined(HND_ROUTER)
		mr->range[0].flags |=  IP_NAT_RANGE_PROTO_RANDOM;
#else
		mr->range[0].flags |=  NF_NAT_RANGE_PROTO_RANDOM;
#endif
		break;
	case O_RANDOM_FULLY:
		mr->range[0].flags |=  NF_NAT_RANGE_PROTO_RANDOM_FULLY;
		break;
	}
}

static void
FULLCONENAT_print(const void *ip, const struct xt_entry_target *target,
                 int numeric)
{
#if defined(BCMARM) && !defined(HND_ROUTER)
	const struct nf_nat_multi_range_compat *mr = (const void *)target->data;
	const struct nf_nat_range *r = &mr->range[0];
#else
	const struct nf_nat_ipv4_multi_range_compat *mr = (const void *)target->data;
	const struct nf_nat_ipv4_range *r = &mr->range[0];
#endif

#if defined(BCMARM) && !defined(HND_ROUTER)
	if (r->flags & IP_NAT_RANGE_PROTO_SPECIFIED) {
#else
	if (r->flags & NF_NAT_RANGE_PROTO_SPECIFIED) {
#endif
		printf(" masq ports: ");
		printf("%hu", ntohs(r->min.tcp.port));
		if (r->max.tcp.port != r->min.tcp.port)
			printf("-%hu", ntohs(r->max.tcp.port));
	}
#if defined(BCMARM) && !defined(HND_ROUTER)
	if (r->flags & IP_NAT_RANGE_PROTO_RANDOM)
#else
	if (r->flags & NF_NAT_RANGE_PROTO_RANDOM)
#endif
		printf(" random");

	if (r->flags & NF_NAT_RANGE_PROTO_RANDOM_FULLY)
		printf(" random-fully");
}

static void
FULLCONENAT_save(const void *ip, const struct xt_entry_target *target)
{
#if defined(BCMARM) && !defined(HND_ROUTER)
	const struct nf_nat_multi_range_compat *mr = (const void *)target->data;
	const struct nf_nat_range *r = &mr->range[0];
#else
	const struct nf_nat_ipv4_multi_range_compat *mr = (const void *)target->data;
	const struct nf_nat_ipv4_range *r = &mr->range[0];
#endif

#if defined(BCMARM) && !defined(HND_ROUTER)
	if (r->flags & IP_NAT_RANGE_PROTO_SPECIFIED) {
#else
	if (r->flags & NF_NAT_RANGE_PROTO_SPECIFIED) {
#endif
		printf(" --to-ports %hu", ntohs(r->min.tcp.port));
		if (r->max.tcp.port != r->min.tcp.port)
			printf("-%hu", ntohs(r->max.tcp.port));
	}
#if defined(BCMARM) && !defined(HND_ROUTER)
	if (r->flags & IP_NAT_RANGE_PROTO_RANDOM)
#else
	if (r->flags & NF_NAT_RANGE_PROTO_RANDOM)
#endif
		printf(" --random");

	if (r->flags & NF_NAT_RANGE_PROTO_RANDOM_FULLY)
		printf(" --random-fully");
}

static struct xtables_target fullconenat_tg_reg = {
	.name		= "FULLCONENAT",
	.version	= XTABLES_VERSION,
	.family		= NFPROTO_IPV4,
#if defined(BCMARM) && !defined(HND_ROUTER)
	.size		= XT_ALIGN(sizeof(struct nf_nat_multi_range_compat)),
	.userspacesize	= XT_ALIGN(sizeof(struct nf_nat_multi_range_compat)),
#else
	.size		= XT_ALIGN(sizeof(struct nf_nat_ipv4_multi_range_compat)),
	.userspacesize	= XT_ALIGN(sizeof(struct nf_nat_ipv4_multi_range_compat)),
#endif
	.help		= FULLCONENAT_help,
	.init		= FULLCONENAT_init,
	.x6_parse	= FULLCONENAT_parse,
	.print		= FULLCONENAT_print,
	.save		= FULLCONENAT_save,
	.x6_options	= FULLCONENAT_opts,
};

void _init(void)
{
	xtables_register_target(&fullconenat_tg_reg);
}

