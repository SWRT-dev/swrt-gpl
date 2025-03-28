#include <net/if.h>
#include <errno.h>
#include <string.h>

#include <netlink/genl/genl.h>
#include <netlink/genl/family.h>
#include <netlink/genl/ctrl.h>
#include <netlink/msg.h>
#include <netlink/attr.h>
#include <time.h>

#include "nl80211.h"
#include "iw.h"

SECTION(station);

enum plink_state {
	LISTEN,
	OPN_SNT,
	OPN_RCVD,
	CNF_RCVD,
	ESTAB,
	HOLDING,
	BLOCKED
};

static void print_power_mode(struct nlattr *a)
{
	enum nl80211_mesh_power_mode pm = nla_get_u32(a);

	switch (pm) {
	case NL80211_MESH_POWER_ACTIVE:
		printf("ACTIVE");
		break;
	case NL80211_MESH_POWER_LIGHT_SLEEP:
		printf("LIGHT SLEEP");
		break;
	case NL80211_MESH_POWER_DEEP_SLEEP:
		printf("DEEP SLEEP");
		break;
	default:
		printf("UNKNOWN");
		break;
	}
}

int parse_txq_stats(char *buf, int buflen, struct nlattr *tid_stats_attr, int header,
		    int tid, const char *indent)
{
	struct nlattr *txqstats_info[NL80211_TXQ_STATS_MAX + 1], *txqinfo;
	static struct nla_policy txqstats_policy[NL80211_TXQ_STATS_MAX + 1] = {
		[NL80211_TXQ_STATS_BACKLOG_BYTES] = { .type = NLA_U32 },
		[NL80211_TXQ_STATS_BACKLOG_PACKETS] = { .type = NLA_U32 },
		[NL80211_TXQ_STATS_FLOWS] = { .type = NLA_U32 },
		[NL80211_TXQ_STATS_DROPS] = { .type = NLA_U32 },
		[NL80211_TXQ_STATS_ECN_MARKS] = { .type = NLA_U32 },
		[NL80211_TXQ_STATS_OVERLIMIT] = { .type = NLA_U32 },
		[NL80211_TXQ_STATS_COLLISIONS] = { .type = NLA_U32 },
		[NL80211_TXQ_STATS_TX_BYTES] = { .type = NLA_U32 },
		[NL80211_TXQ_STATS_TX_PACKETS] = { .type = NLA_U32 },
	};
	char *pos = buf;
	if (nla_parse_nested(txqstats_info, NL80211_TXQ_STATS_MAX, tid_stats_attr,
			     txqstats_policy)) {
		printf("failed to parse nested TXQ stats attributes!");
		return 0;
	}

	if (header)
		pos += snprintf(buf, buflen, "\n%s\t%s\tqsz-byt\t"
				"qsz-pkt\tflows\tdrops\tmarks\toverlmt\t"
				"hashcol\ttx-bytes\ttx-packets", indent,
				tid >= 0 ? "TID" : "");

	pos += snprintf(pos, buflen - (pos - buf), "\n%s\t", indent);
	if (tid >= 0)
		pos += snprintf(pos, buflen - (pos - buf), "%d", tid);

#define PRINT_STAT(key, spacer) do {					 \
		txqinfo = txqstats_info[NL80211_TXQ_STATS_ ## key];	 \
		pos += snprintf(pos, buflen - (pos - buf), spacer);	 \
		if (txqinfo)						 \
			pos += snprintf(pos, buflen - (pos - buf), "%u", \
					nla_get_u32(txqinfo));		 \
	} while (0)


	PRINT_STAT(BACKLOG_BYTES, "\t");
	PRINT_STAT(BACKLOG_PACKETS, "\t");
	PRINT_STAT(FLOWS, "\t");
	PRINT_STAT(DROPS, "\t");
	PRINT_STAT(ECN_MARKS, "\t");
	PRINT_STAT(OVERLIMIT, "\t");
	PRINT_STAT(COLLISIONS, "\t");
	PRINT_STAT(TX_BYTES, "\t");
	PRINT_STAT(TX_PACKETS, "\t\t");

#undef PRINT_STAT

	return pos - buf;

}

static void parse_tid_stats(struct nlattr *tid_stats_attr)
{
	struct nlattr *stats_info[NL80211_TID_STATS_MAX + 1], *tidattr, *info;
	static struct nla_policy stats_policy[NL80211_TID_STATS_MAX + 1] = {
		[NL80211_TID_STATS_RX_MSDU] = { .type = NLA_U64 },
		[NL80211_TID_STATS_TX_MSDU] = { .type = NLA_U64 },
		[NL80211_TID_STATS_TX_MSDU_RETRIES] = { .type = NLA_U64 },
		[NL80211_TID_STATS_TX_MSDU_FAILED] = { .type = NLA_U64 },
		[NL80211_TID_STATS_TXQ_STATS] = { .type = NLA_NESTED },
	};
	int rem, i = 0;
	char txqbuf[2000] = {}, *pos = txqbuf;
	int buflen = sizeof(txqbuf), foundtxq = 0;

	printf("\n\tMSDU:\n\t\tTID\trx\ttx\ttx retries\ttx failed");
	nla_for_each_nested(tidattr, tid_stats_attr, rem) {
		if (nla_parse_nested(stats_info, NL80211_TID_STATS_MAX,
				     tidattr, stats_policy)) {
			printf("failed to parse nested stats attributes!");
			return;
		}
		printf("\n\t\t%d", i);
		info = stats_info[NL80211_TID_STATS_RX_MSDU];
		if (info)
			printf("\t%llu", (unsigned long long)nla_get_u64(info));
		info = stats_info[NL80211_TID_STATS_TX_MSDU];
		if (info)
			printf("\t%llu", (unsigned long long)nla_get_u64(info));
		info = stats_info[NL80211_TID_STATS_TX_MSDU_RETRIES];
		if (info)
			printf("\t%llu", (unsigned long long)nla_get_u64(info));
		info = stats_info[NL80211_TID_STATS_TX_MSDU_FAILED];
		if (info)
			printf("\t\t%llu", (unsigned long long)nla_get_u64(info));
		info = stats_info[NL80211_TID_STATS_TXQ_STATS];
		if (info) {
			pos += parse_txq_stats(pos, buflen - (pos - txqbuf), info, !foundtxq, i, "\t");
			foundtxq = 1;
		}

		i++;
	}

	if (foundtxq)
		printf("\n\tTXQs:%s", txqbuf);
}

static void parse_bss_param(struct nlattr *bss_param_attr)
{
	struct nlattr *bss_param_info[NL80211_STA_BSS_PARAM_MAX + 1], *info;
	static struct nla_policy bss_poilcy[NL80211_STA_BSS_PARAM_MAX + 1] = {
		[NL80211_STA_BSS_PARAM_CTS_PROT] = { .type = NLA_FLAG },
		[NL80211_STA_BSS_PARAM_SHORT_PREAMBLE] = { .type = NLA_FLAG },
		[NL80211_STA_BSS_PARAM_SHORT_SLOT_TIME] = { .type = NLA_FLAG },
		[NL80211_STA_BSS_PARAM_DTIM_PERIOD] = { .type = NLA_U8 },
		[NL80211_STA_BSS_PARAM_BEACON_INTERVAL] = { .type = NLA_U16 },
	};

	if (nla_parse_nested(bss_param_info, NL80211_STA_BSS_PARAM_MAX,
			     bss_param_attr, bss_poilcy)) {
		printf("failed to parse nested bss param attributes!");
	}

	info = bss_param_info[NL80211_STA_BSS_PARAM_DTIM_PERIOD];
	if (info)
		printf("\n\tDTIM period:\t%u", nla_get_u8(info));
	info = bss_param_info[NL80211_STA_BSS_PARAM_BEACON_INTERVAL];
	if (info)
		printf("\n\tbeacon interval:%u", nla_get_u16(info));
	info = bss_param_info[NL80211_STA_BSS_PARAM_CTS_PROT];
	if (info) {
		printf("\n\tCTS protection:");
		if (nla_get_u16(info))
			printf("\tyes");
		else
			printf("\tno");
	}
	info = bss_param_info[NL80211_STA_BSS_PARAM_SHORT_PREAMBLE];
	if (info) {
		printf("\n\tshort preamble:");
		if (nla_get_u16(info))
			printf("\tyes");
		else
			printf("\tno");
	}
	info = bss_param_info[NL80211_STA_BSS_PARAM_SHORT_SLOT_TIME];
	if (info) {
		printf("\n\tshort slot time:");
		if (nla_get_u16(info))
			printf("yes");
		else
			printf("no");
	}
}

void parse_bitrate(struct nlattr *bitrate_attr, char *buf, int buflen)
{
	int rate = 0;
	char *pos = buf;
	struct nlattr *rinfo[NL80211_RATE_INFO_MAX + 1];
	static struct nla_policy rate_policy[NL80211_RATE_INFO_MAX + 1] = {
		[NL80211_RATE_INFO_BITRATE] = { .type = NLA_U16 },
		[NL80211_RATE_INFO_BITRATE32] = { .type = NLA_U32 },
		[NL80211_RATE_INFO_MCS] = { .type = NLA_U8 },
		[NL80211_RATE_INFO_40_MHZ_WIDTH] = { .type = NLA_FLAG },
		[NL80211_RATE_INFO_SHORT_GI] = { .type = NLA_FLAG },
	};

	if (nla_parse_nested(rinfo, NL80211_RATE_INFO_MAX,
			     bitrate_attr, rate_policy)) {
		snprintf(buf, buflen, "failed to parse nested rate attributes!");
		return;
	}

	if (rinfo[NL80211_RATE_INFO_BITRATE32])
		rate = nla_get_u32(rinfo[NL80211_RATE_INFO_BITRATE32]);
	else if (rinfo[NL80211_RATE_INFO_BITRATE])
		rate = nla_get_u16(rinfo[NL80211_RATE_INFO_BITRATE]);
	if (rate > 0)
		pos += snprintf(pos, buflen - (pos - buf),
				"%d.%d MBit/s", rate / 10, rate % 10);
	else
		pos += snprintf(pos, buflen - (pos - buf), "(unknown)");

	if (rinfo[NL80211_RATE_INFO_MCS])
		pos += snprintf(pos, buflen - (pos - buf),
				" MCS %d", nla_get_u8(rinfo[NL80211_RATE_INFO_MCS]));
	if (rinfo[NL80211_RATE_INFO_VHT_MCS])
		pos += snprintf(pos, buflen - (pos - buf),
				" VHT-MCS %d", nla_get_u8(rinfo[NL80211_RATE_INFO_VHT_MCS]));
	if (rinfo[NL80211_RATE_INFO_40_MHZ_WIDTH])
		pos += snprintf(pos, buflen - (pos - buf), " 40MHz");
	if (rinfo[NL80211_RATE_INFO_80_MHZ_WIDTH])
		pos += snprintf(pos, buflen - (pos - buf), " 80MHz");
	if (rinfo[NL80211_RATE_INFO_80P80_MHZ_WIDTH])
		pos += snprintf(pos, buflen - (pos - buf), " 80P80MHz");
	if (rinfo[NL80211_RATE_INFO_160_MHZ_WIDTH])
		pos += snprintf(pos, buflen - (pos - buf), " 160MHz");
	if (rinfo[NL80211_RATE_INFO_SHORT_GI])
		pos += snprintf(pos, buflen - (pos - buf), " short GI");
	if (rinfo[NL80211_RATE_INFO_VHT_NSS])
		pos += snprintf(pos, buflen - (pos - buf),
				" VHT-NSS %d", nla_get_u8(rinfo[NL80211_RATE_INFO_VHT_NSS]));
	if (rinfo[NL80211_RATE_INFO_HE_MCS])
		pos += snprintf(pos, buflen - (pos - buf),
				" HE-MCS %d", nla_get_u8(rinfo[NL80211_RATE_INFO_HE_MCS]));
	if (rinfo[NL80211_RATE_INFO_HE_NSS])
		pos += snprintf(pos, buflen - (pos - buf),
				" HE-NSS %d", nla_get_u8(rinfo[NL80211_RATE_INFO_HE_NSS]));
	if (rinfo[NL80211_RATE_INFO_HE_GI])
		pos += snprintf(pos, buflen - (pos - buf),
				" HE-GI %d", nla_get_u8(rinfo[NL80211_RATE_INFO_HE_GI]));
	if (rinfo[NL80211_RATE_INFO_HE_DCM])
		pos += snprintf(pos, buflen - (pos - buf),
				" HE-DCM %d", nla_get_u8(rinfo[NL80211_RATE_INFO_HE_DCM]));
	if (rinfo[NL80211_RATE_INFO_HE_RU_ALLOC])
		pos += snprintf(pos, buflen - (pos - buf),
				" HE-RU-ALLOC %d", nla_get_u8(rinfo[NL80211_RATE_INFO_HE_RU_ALLOC]));
	if (rinfo[NL80211_RATE_INFO_EHT_MCS])
		pos += snprintf(pos, buflen - (pos - buf),
				" EHT-MCS %d", nla_get_u8(rinfo[NL80211_RATE_INFO_EHT_MCS]));
	if (rinfo[NL80211_RATE_INFO_EHT_NSS])
		pos += snprintf(pos, buflen - (pos - buf),
				" EHT-NSS %d", nla_get_u8(rinfo[NL80211_RATE_INFO_EHT_NSS]));
	if (rinfo[NL80211_RATE_INFO_EHT_GI])
		pos += snprintf(pos, buflen - (pos - buf),
				" EHT-GI %d", nla_get_u8(rinfo[NL80211_RATE_INFO_EHT_GI]));
	if (rinfo[NL80211_RATE_INFO_EHT_RU_ALLOC])
		pos += snprintf(pos, buflen - (pos - buf),
				" EHT-RU-ALLOC %d", nla_get_u8(rinfo[NL80211_RATE_INFO_EHT_RU_ALLOC]));

}

static char *get_chain_signal(struct nlattr *attr_list)
{
	struct nlattr *attr;
	static char buf[64];
	char *cur = buf;
	int i = 0, rem;
	const char *prefix;

	if (!attr_list)
		return "";

	nla_for_each_nested(attr, attr_list, rem) {
		if (i++ > 0)
			prefix = ", ";
		else
			prefix = "[";

		cur += snprintf(cur, sizeof(buf) - (cur - buf), "%s%d", prefix,
				(int8_t) nla_get_u8(attr));
	}

	if (i)
		snprintf(cur, sizeof(buf) - (cur - buf), "] ");

	return buf;
}

static int print_sta_handler(struct nl_msg *msg, void *arg)
{
	struct nlattr *tb[NL80211_ATTR_MAX + 1];
	struct genlmsghdr *gnlh = nlmsg_data(nlmsg_hdr(msg));
	struct nlattr *sinfo[NL80211_STA_INFO_MAX + 1];
	char mac_addr[20], state_name[10], dev[20];
	struct nl80211_sta_flag_update *sta_flags;
	static struct nla_policy stats_policy[NL80211_STA_INFO_MAX + 1] = {
		[NL80211_STA_INFO_INACTIVE_TIME] = { .type = NLA_U32 },
		[NL80211_STA_INFO_RX_BYTES] = { .type = NLA_U32 },
		[NL80211_STA_INFO_TX_BYTES] = { .type = NLA_U32 },
		[NL80211_STA_INFO_RX_BYTES64] = { .type = NLA_U64 },
		[NL80211_STA_INFO_TX_BYTES64] = { .type = NLA_U64 },
		[NL80211_STA_INFO_RX_PACKETS] = { .type = NLA_U32 },
		[NL80211_STA_INFO_TX_PACKETS] = { .type = NLA_U32 },
		[NL80211_STA_INFO_BEACON_RX] = { .type = NLA_U64},
		[NL80211_STA_INFO_SIGNAL] = { .type = NLA_U8 },
		[NL80211_STA_INFO_T_OFFSET] = { .type = NLA_U64 },
		[NL80211_STA_INFO_TX_BITRATE] = { .type = NLA_NESTED },
		[NL80211_STA_INFO_RX_BITRATE] = { .type = NLA_NESTED },
		[NL80211_STA_INFO_LLID] = { .type = NLA_U16 },
		[NL80211_STA_INFO_PLID] = { .type = NLA_U16 },
		[NL80211_STA_INFO_PLINK_STATE] = { .type = NLA_U8 },
		[NL80211_STA_INFO_TX_RETRIES] = { .type = NLA_U32 },
		[NL80211_STA_INFO_TX_FAILED] = { .type = NLA_U32 },
		[NL80211_STA_INFO_BEACON_LOSS] = { .type = NLA_U32},
		[NL80211_STA_INFO_RX_DROP_MISC] = { .type = NLA_U64},
		[NL80211_STA_INFO_STA_FLAGS] =
			{ .minlen = sizeof(struct nl80211_sta_flag_update) },
		[NL80211_STA_INFO_LOCAL_PM] = { .type = NLA_U32},
		[NL80211_STA_INFO_PEER_PM] = { .type = NLA_U32},
		[NL80211_STA_INFO_NONPEER_PM] = { .type = NLA_U32},
		[NL80211_STA_INFO_CHAIN_SIGNAL] = { .type = NLA_NESTED },
		[NL80211_STA_INFO_CHAIN_SIGNAL_AVG] = { .type = NLA_NESTED },
		[NL80211_STA_INFO_TID_STATS] = { .type = NLA_NESTED },
		[NL80211_STA_INFO_BSS_PARAM] = { .type = NLA_NESTED },
		[NL80211_STA_INFO_RX_DURATION] = { .type = NLA_U64 },
		[NL80211_STA_INFO_TX_DURATION] = { .type = NLA_U64 },
		[NL80211_STA_INFO_ACK_SIGNAL] = {.type = NLA_U8 },
		[NL80211_STA_INFO_ACK_SIGNAL_AVG] = { .type = NLA_U8 },
		[NL80211_STA_INFO_AIRTIME_LINK_METRIC] = { .type = NLA_U32 },
		[NL80211_STA_INFO_CONNECTED_TO_AS] = { .type = NLA_U8 },
		[NL80211_STA_INFO_CONNECTED_TO_GATE] = { .type = NLA_U8 },
	};
	char *chain;
	struct timeval now;
	unsigned long long now_ms;

	gettimeofday(&now, NULL);
	now_ms = now.tv_sec * 1000ULL;
	now_ms += (now.tv_usec / 1000);

	nla_parse(tb, NL80211_ATTR_MAX, genlmsg_attrdata(gnlh, 0),
		  genlmsg_attrlen(gnlh, 0), NULL);

	/*
	 * TODO: validate the interface and mac address!
	 * Otherwise, there's a race condition as soon as
	 * the kernel starts sending station notifications.
	 */

	if (!tb[NL80211_ATTR_STA_INFO]) {
		fprintf(stderr, "sta stats missing!\n");
		return NL_SKIP;
	}
	if (nla_parse_nested(sinfo, NL80211_STA_INFO_MAX,
			     tb[NL80211_ATTR_STA_INFO],
			     stats_policy)) {
		fprintf(stderr, "failed to parse nested attributes!\n");
		return NL_SKIP;
	}

	mac_addr_n2a(mac_addr, nla_data(tb[NL80211_ATTR_MAC]));
	if_indextoname(nla_get_u32(tb[NL80211_ATTR_IFINDEX]), dev);
	printf("Station %s (on %s)", mac_addr, dev);

	if (sinfo[NL80211_STA_INFO_INACTIVE_TIME])
		printf("\n\tinactive time:\t%u ms",
			nla_get_u32(sinfo[NL80211_STA_INFO_INACTIVE_TIME]));
	if (sinfo[NL80211_STA_INFO_RX_BYTES64])
		printf("\n\trx bytes:\t%llu",
		       (unsigned long long)nla_get_u64(sinfo[NL80211_STA_INFO_RX_BYTES64]));
	else if (sinfo[NL80211_STA_INFO_RX_BYTES])
		printf("\n\trx bytes:\t%u",
		       nla_get_u32(sinfo[NL80211_STA_INFO_RX_BYTES]));
	if (sinfo[NL80211_STA_INFO_RX_PACKETS])
		printf("\n\trx packets:\t%u",
			nla_get_u32(sinfo[NL80211_STA_INFO_RX_PACKETS]));
	if (sinfo[NL80211_STA_INFO_TX_BYTES64])
		printf("\n\ttx bytes:\t%llu",
		       (unsigned long long)nla_get_u64(sinfo[NL80211_STA_INFO_TX_BYTES64]));
	else if (sinfo[NL80211_STA_INFO_TX_BYTES])
		printf("\n\ttx bytes:\t%u",
		       nla_get_u32(sinfo[NL80211_STA_INFO_TX_BYTES]));
	if (sinfo[NL80211_STA_INFO_TX_PACKETS])
		printf("\n\ttx packets:\t%u",
			nla_get_u32(sinfo[NL80211_STA_INFO_TX_PACKETS]));
	if (sinfo[NL80211_STA_INFO_TX_RETRIES])
		printf("\n\ttx retries:\t%u",
			nla_get_u32(sinfo[NL80211_STA_INFO_TX_RETRIES]));
	if (sinfo[NL80211_STA_INFO_TX_FAILED])
		printf("\n\ttx failed:\t%u",
			nla_get_u32(sinfo[NL80211_STA_INFO_TX_FAILED]));
	if (sinfo[NL80211_STA_INFO_BEACON_LOSS])
		printf("\n\tbeacon loss:\t%u",
		       nla_get_u32(sinfo[NL80211_STA_INFO_BEACON_LOSS]));
	if (sinfo[NL80211_STA_INFO_BEACON_RX])
		printf("\n\tbeacon rx:\t%llu",
		       (unsigned long long)nla_get_u64(sinfo[NL80211_STA_INFO_BEACON_RX]));
	if (sinfo[NL80211_STA_INFO_RX_DROP_MISC])
		printf("\n\trx drop misc:\t%llu",
		       (unsigned long long)nla_get_u64(sinfo[NL80211_STA_INFO_RX_DROP_MISC]));

	chain = get_chain_signal(sinfo[NL80211_STA_INFO_CHAIN_SIGNAL]);
	if (sinfo[NL80211_STA_INFO_SIGNAL])
		printf("\n\tsignal:  \t%d %sdBm",
			(int8_t)nla_get_u8(sinfo[NL80211_STA_INFO_SIGNAL]),
			chain);

	chain = get_chain_signal(sinfo[NL80211_STA_INFO_CHAIN_SIGNAL_AVG]);
	if (sinfo[NL80211_STA_INFO_SIGNAL_AVG])
		printf("\n\tsignal avg:\t%d %sdBm",
			(int8_t)nla_get_u8(sinfo[NL80211_STA_INFO_SIGNAL_AVG]),
			chain);

	if (sinfo[NL80211_STA_INFO_BEACON_SIGNAL_AVG])
		printf("\n\tbeacon signal avg:\t%d dBm",
		       (int8_t)nla_get_u8(sinfo[NL80211_STA_INFO_BEACON_SIGNAL_AVG]));
	if (sinfo[NL80211_STA_INFO_T_OFFSET])
		printf("\n\tToffset:\t%llu us",
		       (unsigned long long)nla_get_u64(sinfo[NL80211_STA_INFO_T_OFFSET]));

	if (sinfo[NL80211_STA_INFO_TX_BITRATE]) {
		char buf[100];

		parse_bitrate(sinfo[NL80211_STA_INFO_TX_BITRATE], buf, sizeof(buf));
		printf("\n\ttx bitrate:\t%s", buf);
	}

	if (sinfo[NL80211_STA_INFO_TX_DURATION])
		printf("\n\ttx duration:\t%lld us",
		       (unsigned long long)nla_get_u64(sinfo[NL80211_STA_INFO_TX_DURATION]));

	if (sinfo[NL80211_STA_INFO_RX_BITRATE]) {
		char buf[100];

		parse_bitrate(sinfo[NL80211_STA_INFO_RX_BITRATE], buf, sizeof(buf));
		printf("\n\trx bitrate:\t%s", buf);
	}

	if (sinfo[NL80211_STA_INFO_RX_DURATION])
		printf("\n\trx duration:\t%lld us",
		       (unsigned long long)nla_get_u64(sinfo[NL80211_STA_INFO_RX_DURATION]));

	if (sinfo[NL80211_STA_INFO_ACK_SIGNAL])
		printf("\n\tlast ack signal:%d dBm",
			(int8_t)nla_get_u8(sinfo[NL80211_STA_INFO_ACK_SIGNAL]));

	if (sinfo[NL80211_STA_INFO_ACK_SIGNAL_AVG])
		printf("\n\tavg ack signal:\t%d dBm",
			(int8_t)nla_get_u8(sinfo[NL80211_STA_INFO_ACK_SIGNAL_AVG]));

	if (sinfo[NL80211_STA_INFO_AIRTIME_WEIGHT]) {
		printf("\n\tairtime weight: %d", nla_get_u16(sinfo[NL80211_STA_INFO_AIRTIME_WEIGHT]));
	}

	if (sinfo[NL80211_STA_INFO_EXPECTED_THROUGHPUT]) {
		uint32_t thr;

		thr = nla_get_u32(sinfo[NL80211_STA_INFO_EXPECTED_THROUGHPUT]);
		/* convert in Mbps but scale by 1000 to save kbps units */
		thr = thr * 1000 / 1024;

		printf("\n\texpected throughput:\t%u.%uMbps",
		       thr / 1000, thr % 1000);
	}

	if (sinfo[NL80211_STA_INFO_LLID])
		printf("\n\tmesh llid:\t%d",
			nla_get_u16(sinfo[NL80211_STA_INFO_LLID]));
	if (sinfo[NL80211_STA_INFO_PLID])
		printf("\n\tmesh plid:\t%d",
			nla_get_u16(sinfo[NL80211_STA_INFO_PLID]));
	if (sinfo[NL80211_STA_INFO_PLINK_STATE]) {
		switch (nla_get_u8(sinfo[NL80211_STA_INFO_PLINK_STATE])) {
		case LISTEN:
			strcpy(state_name, "LISTEN");
			break;
		case OPN_SNT:
			strcpy(state_name, "OPN_SNT");
			break;
		case OPN_RCVD:
			strcpy(state_name, "OPN_RCVD");
			break;
		case CNF_RCVD:
			strcpy(state_name, "CNF_RCVD");
			break;
		case ESTAB:
			strcpy(state_name, "ESTAB");
			break;
		case HOLDING:
			strcpy(state_name, "HOLDING");
			break;
		case BLOCKED:
			strcpy(state_name, "BLOCKED");
			break;
		default:
			strcpy(state_name, "UNKNOWN");
			break;
		}
		printf("\n\tmesh plink:\t%s", state_name);
	}
	if (sinfo[NL80211_STA_INFO_AIRTIME_LINK_METRIC])
		printf("\n\tmesh airtime link metric: %d",
			nla_get_u32(sinfo[NL80211_STA_INFO_AIRTIME_LINK_METRIC]));
	if (sinfo[NL80211_STA_INFO_CONNECTED_TO_GATE])
		printf("\n\tmesh connected to gate:\t%s",
			nla_get_u8(sinfo[NL80211_STA_INFO_CONNECTED_TO_GATE]) ?
			"yes" : "no");
	if (sinfo[NL80211_STA_INFO_CONNECTED_TO_AS])
		printf("\n\tmesh connected to auth server:\t%s",
			nla_get_u8(sinfo[NL80211_STA_INFO_CONNECTED_TO_AS]) ?
			"yes" : "no");

	if (sinfo[NL80211_STA_INFO_LOCAL_PM]) {
		printf("\n\tmesh local PS mode:\t");
		print_power_mode(sinfo[NL80211_STA_INFO_LOCAL_PM]);
	}
	if (sinfo[NL80211_STA_INFO_PEER_PM]) {
		printf("\n\tmesh peer PS mode:\t");
		print_power_mode(sinfo[NL80211_STA_INFO_PEER_PM]);
	}
	if (sinfo[NL80211_STA_INFO_NONPEER_PM]) {
		printf("\n\tmesh non-peer PS mode:\t");
		print_power_mode(sinfo[NL80211_STA_INFO_NONPEER_PM]);
	}

	if (sinfo[NL80211_STA_INFO_STA_FLAGS]) {
		sta_flags = (struct nl80211_sta_flag_update *)
			    nla_data(sinfo[NL80211_STA_INFO_STA_FLAGS]);

		if (sta_flags->mask & BIT(NL80211_STA_FLAG_AUTHORIZED)) {
			printf("\n\tauthorized:\t");
			if (sta_flags->set & BIT(NL80211_STA_FLAG_AUTHORIZED))
				printf("yes");
			else
				printf("no");
		}

		if (sta_flags->mask & BIT(NL80211_STA_FLAG_AUTHENTICATED)) {
			printf("\n\tauthenticated:\t");
			if (sta_flags->set & BIT(NL80211_STA_FLAG_AUTHENTICATED))
				printf("yes");
			else
				printf("no");
		}

		if (sta_flags->mask & BIT(NL80211_STA_FLAG_ASSOCIATED)) {
			printf("\n\tassociated:\t");
			if (sta_flags->set & BIT(NL80211_STA_FLAG_ASSOCIATED))
				printf("yes");
			else
				printf("no");
		}

		if (sta_flags->mask & BIT(NL80211_STA_FLAG_SHORT_PREAMBLE)) {
			printf("\n\tpreamble:\t");
			if (sta_flags->set & BIT(NL80211_STA_FLAG_SHORT_PREAMBLE))
				printf("short");
			else
				printf("long");
		}

		if (sta_flags->mask & BIT(NL80211_STA_FLAG_WME)) {
			printf("\n\tWMM/WME:\t");
			if (sta_flags->set & BIT(NL80211_STA_FLAG_WME))
				printf("yes");
			else
				printf("no");
		}

		if (sta_flags->mask & BIT(NL80211_STA_FLAG_MFP)) {
			printf("\n\tMFP:\t\t");
			if (sta_flags->set & BIT(NL80211_STA_FLAG_MFP))
				printf("yes");
			else
				printf("no");
		}

		if (sta_flags->mask & BIT(NL80211_STA_FLAG_TDLS_PEER)) {
			printf("\n\tTDLS peer:\t");
			if (sta_flags->set & BIT(NL80211_STA_FLAG_TDLS_PEER))
				printf("yes");
			else
				printf("no");
		}
	}

	if (sinfo[NL80211_STA_INFO_TID_STATS] && arg != NULL &&
	    !strcmp((char *)arg, "-v"))
		parse_tid_stats(sinfo[NL80211_STA_INFO_TID_STATS]);
	if (sinfo[NL80211_STA_INFO_BSS_PARAM])
		parse_bss_param(sinfo[NL80211_STA_INFO_BSS_PARAM]);
	if (sinfo[NL80211_STA_INFO_CONNECTED_TIME])
		printf("\n\tconnected time:\t%u seconds",
			nla_get_u32(sinfo[NL80211_STA_INFO_CONNECTED_TIME]));
	if (sinfo[NL80211_STA_INFO_ASSOC_AT_BOOTTIME]) {
		unsigned long long bt;
		struct timespec now_ts;
		unsigned long long boot_ns;
		unsigned long long assoc_at_ms;

		clock_gettime(CLOCK_BOOTTIME, &now_ts);
		boot_ns = now_ts.tv_sec * 1000000000ULL;
		boot_ns += now_ts.tv_nsec;

		bt = (unsigned long long)nla_get_u64(sinfo[NL80211_STA_INFO_ASSOC_AT_BOOTTIME]);
		printf("\n\tassociated at [boottime]:\t%llu.%.3llus",
		       bt/1000000000, (bt%1000000000)/1000000);
		assoc_at_ms = now_ms - ((boot_ns - bt) / 1000000);
		printf("\n\tassociated at:\t%llu ms", assoc_at_ms);
	}

	printf("\n\tcurrent time:\t%llu ms\n", now_ms);
	return NL_SKIP;
}

static int handle_station_get(struct nl80211_state *state,
			      struct nl_msg *msg,
			      int argc, char **argv,
			      enum id_input id)
{
	unsigned char mac_addr[ETH_ALEN];

	if (argc < 1)
		return 1;

	if (mac_addr_a2n(mac_addr, argv[0])) {
		fprintf(stderr, "invalid mac address\n");
		return 2;
	}

	argc--;
	argv++;

	if (argc)
		return 1;

	NLA_PUT(msg, NL80211_ATTR_MAC, ETH_ALEN, mac_addr);

	register_handler(print_sta_handler, NULL);

	return 0;
 nla_put_failure:
	return -ENOBUFS;
}
COMMAND(station, get, "<MAC address>",
	NL80211_CMD_GET_STATION, 0, CIB_NETDEV, handle_station_get,
	"Get information for a specific station.");

static int handle_station_del(struct nl80211_state *state,
			      struct nl_msg *msg,
			      int argc, char **argv,
			      enum id_input id)
{
	char *end;
	unsigned char mac_addr[ETH_ALEN];
	int subtype;
	int reason_code;

	if (argc < 1)
		return 1;

	if (mac_addr_a2n(mac_addr, argv[0])) {
		fprintf(stderr, "invalid mac address\n");
		return 2;
	}

	argc--;
	argv++;
	NLA_PUT(msg, NL80211_ATTR_MAC, ETH_ALEN, mac_addr);

	if (argc > 1 && strcmp(argv[0], "subtype") == 0) {
		argv++;
		argc--;

		subtype = strtod(argv[0], &end);
		if (*end != '\0')
			return 1;

		NLA_PUT_U8(msg, NL80211_ATTR_MGMT_SUBTYPE, subtype);
		argv++;
		argc--;
	}

	if (argc > 1 && strcmp(argv[0], "reason-code") == 0) {
		argv++;
		argc--;

		reason_code = strtod(argv[0], &end);
		if (*end != '\0')
			return 1;

		NLA_PUT_U16(msg, NL80211_ATTR_REASON_CODE, reason_code);
		argv++;
		argc--;
	}

	if (argc)
		return 1;

	register_handler(print_sta_handler, NULL);

	return 0;
 nla_put_failure:
	return -ENOBUFS;
}
COMMAND(station, del, "<MAC address> [subtype <subtype>] [reason-code <code>]",
	NL80211_CMD_DEL_STATION, 0, CIB_NETDEV, handle_station_del,
	"Remove the given station entry (use with caution!)\n"
	"Example subtype values: 0xA (disassociation), 0xC (deauthentication)");

static const struct cmd *station_set_plink;
static const struct cmd *station_set_vlan;
static const struct cmd *station_set_mesh_power_mode;
static const struct cmd *station_set_airtime_weight;
static const struct cmd *station_set_txpwr;
static const struct cmd *station_set_mgmt_rts_cts;

static const struct cmd *select_station_cmd(int argc, char **argv)
{
	if (argc < 2)
		return NULL;
	if (strcmp(argv[1], "plink_action") == 0)
		return station_set_plink;
	if (strcmp(argv[1], "vlan") == 0)
		return station_set_vlan;
	if (strcmp(argv[1], "mesh_power_mode") == 0)
		return station_set_mesh_power_mode;
	if (strcmp(argv[1], "airtime_weight") == 0)
		return station_set_airtime_weight;
	if (strcmp(argv[1], "txpwr") == 0)
		return station_set_txpwr;
	if (strcmp(argv[1], "mgmt_rts_cts") == 0)
		return station_set_mgmt_rts_cts;
	return NULL;
}

static int handle_station_set_plink(struct nl80211_state *state,
			      struct nl_msg *msg,
			      int argc, char **argv,
			      enum id_input id)
{
	unsigned char plink_action;
	unsigned char mac_addr[ETH_ALEN];

	if (argc < 3)
		return 1;

	if (mac_addr_a2n(mac_addr, argv[0])) {
		fprintf(stderr, "invalid mac address\n");
		return 2;
	}
	argc--;
	argv++;

	if (strcmp("plink_action", argv[0]) != 0)
		return 1;
	argc--;
	argv++;

	if (strcmp("open", argv[0]) == 0)
		plink_action = NL80211_PLINK_ACTION_OPEN;
	else if (strcmp("block", argv[0]) == 0)
		plink_action = NL80211_PLINK_ACTION_BLOCK;
	else {
		fprintf(stderr, "plink action not supported\n");
		return 2;
	}
	argc--;
	argv++;

	if (argc)
		return 1;

	NLA_PUT(msg, NL80211_ATTR_MAC, ETH_ALEN, mac_addr);
	NLA_PUT_U8(msg, NL80211_ATTR_STA_PLINK_ACTION, plink_action);

	return 0;
 nla_put_failure:
	return -ENOBUFS;
}
COMMAND_ALIAS(station, set, "<MAC address> plink_action <open|block>",
	NL80211_CMD_SET_STATION, 0, CIB_NETDEV, handle_station_set_plink,
	"Set mesh peer link action for this station (peer).",
	select_station_cmd, station_set_plink);

static int handle_station_set_vlan(struct nl80211_state *state,
				   struct nl_msg *msg,
				   int argc, char **argv,
				   enum id_input id)
{
	unsigned char mac_addr[ETH_ALEN];
	unsigned long sta_vlan = 0;
	char *err = NULL;

	if (argc < 3)
		return 1;

	if (mac_addr_a2n(mac_addr, argv[0])) {
		fprintf(stderr, "invalid mac address\n");
		return 2;
	}
	argc--;
	argv++;

	if (strcmp("vlan", argv[0]) != 0)
		return 1;
	argc--;
	argv++;

	sta_vlan = strtoul(argv[0], &err, 0);
	if (err && *err) {
		fprintf(stderr, "invalid vlan id\n");
		return 2;
	}
	argc--;
	argv++;

	if (argc)
		return 1;

	NLA_PUT(msg, NL80211_ATTR_MAC, ETH_ALEN, mac_addr);
	NLA_PUT_U32(msg, NL80211_ATTR_STA_VLAN, sta_vlan);

	return 0;
 nla_put_failure:
	return -ENOBUFS;
}
COMMAND_ALIAS(station, set, "<MAC address> vlan <ifindex>",
	NL80211_CMD_SET_STATION, 0, CIB_NETDEV, handle_station_set_vlan,
	"Set an AP VLAN for this station.",
	select_station_cmd, station_set_vlan);

static int handle_station_set_mesh_power_mode(struct nl80211_state *state,
					      struct nl_msg *msg,
					      int argc, char **argv,
					      enum id_input id)
{
	unsigned char mesh_power_mode;
	unsigned char mac_addr[ETH_ALEN];

	if (argc < 3)
		return 1;

	if (mac_addr_a2n(mac_addr, argv[0])) {
		fprintf(stderr, "invalid mac address\n");
		return 2;
	}
	argc--;
	argv++;

	if (strcmp("mesh_power_mode", argv[0]) != 0)
		return 1;
	argc--;
	argv++;

	if (strcmp("active", argv[0]) == 0)
		mesh_power_mode = NL80211_MESH_POWER_ACTIVE;
	else if (strcmp("light", argv[0]) == 0)
		mesh_power_mode = NL80211_MESH_POWER_LIGHT_SLEEP;
	else if (strcmp("deep", argv[0]) == 0)
		mesh_power_mode = NL80211_MESH_POWER_DEEP_SLEEP;
	else {
		fprintf(stderr, "unknown mesh power mode\n");
		return 2;
	}
	argc--;
	argv++;

	if (argc)
		return 1;

	NLA_PUT(msg, NL80211_ATTR_MAC, ETH_ALEN, mac_addr);
	NLA_PUT_U32(msg, NL80211_ATTR_LOCAL_MESH_POWER_MODE, mesh_power_mode);

	return 0;
nla_put_failure:
	return -ENOBUFS;
}
COMMAND_ALIAS(station, set, "<MAC address> mesh_power_mode "
	"<active|light|deep>", NL80211_CMD_SET_STATION, 0, CIB_NETDEV,
	handle_station_set_mesh_power_mode,
	"Set link-specific mesh power mode for this station",
	select_station_cmd, station_set_mesh_power_mode);

static int handle_station_set_airtime_weight(struct nl80211_state *state,
					     struct nl_msg *msg,
					     int argc, char **argv,
					     enum id_input id)
{
	unsigned char mac_addr[ETH_ALEN];
	unsigned long airtime_weight = 0;
	char *err = NULL;

	if (argc < 3)
		return 1;

	if (mac_addr_a2n(mac_addr, argv[0])) {
		fprintf(stderr, "invalid mac address\n");
		return 2;
	}
	argc--;
	argv++;

	if (strcmp("airtime_weight", argv[0]) != 0)
		return 1;
	argc--;
	argv++;

	airtime_weight = strtoul(argv[0], &err, 0);
	if (err && *err) {
		fprintf(stderr, "invalid airtime weight\n");
		return 2;
	}
	argc--;
	argv++;

	if (argc)
		return 1;

	NLA_PUT(msg, NL80211_ATTR_MAC, ETH_ALEN, mac_addr);
	NLA_PUT_U16(msg, NL80211_ATTR_AIRTIME_WEIGHT, airtime_weight);

	return 0;
 nla_put_failure:
	return -ENOBUFS;

}
COMMAND_ALIAS(station, set, "<MAC address> airtime_weight <weight>",
	NL80211_CMD_SET_STATION, 0, CIB_NETDEV, handle_station_set_airtime_weight,
	"Set airtime weight for this station.",
	select_station_cmd, station_set_airtime_weight);

static int handle_station_set_txpwr(struct nl80211_state *state,
				    struct nl_msg *msg,
				    int argc, char **argv,
				    enum id_input id)
{
	enum nl80211_tx_power_setting type;
	unsigned char mac_addr[ETH_ALEN];
	int sta_txpwr = 0;
	char *err = NULL;

	if (argc != 3 && argc != 4)
		return 1;

	if (mac_addr_a2n(mac_addr, argv[0])) {
		fprintf(stderr, "invalid mac address\n");
		return 2;
	}

	NLA_PUT(msg, NL80211_ATTR_MAC, ETH_ALEN, mac_addr);
	argc--;
	argv++;

	if (strcmp("txpwr", argv[0]) != 0)
		return 1;
	argc--;
	argv++;

	if (!strcmp(argv[0], "auto"))
		type = NL80211_TX_POWER_AUTOMATIC;
	else if (!strcmp(argv[0], "limit"))
		type = NL80211_TX_POWER_LIMITED;
	else {
		printf("Invalid parameter: %s\n", argv[0]);
		return 2;
	}

	NLA_PUT_U8(msg, NL80211_ATTR_STA_TX_POWER_SETTING, type);

	if (type != NL80211_TX_POWER_AUTOMATIC) {
		if (argc != 2) {
			printf("Missing TX power level argument.\n");
			return 2;
		}

		argc--;
		argv++;

		sta_txpwr = strtoul(argv[0], &err, 0);
		NLA_PUT_U16(msg, NL80211_ATTR_STA_TX_POWER, sta_txpwr);
	}

	argc--;
	argv++;

	if (argc)
		return 1;

	return 0;
 nla_put_failure:
	return -ENOBUFS;
}
COMMAND_ALIAS(station, set, "<MAC address> txpwr <auto|limit> [<tx power dBm>]",
	NL80211_CMD_SET_STATION, 0, CIB_NETDEV, handle_station_set_txpwr,
	"Set Tx power for this station.",
	select_station_cmd, station_set_txpwr);

static int handle_station_set_mgmt_rts_cts(struct nl80211_state *state,
					      struct nl_msg *msg,
					      int argc, char **argv,
					      enum id_input id)
{
	unsigned char rts_cts_conf;
	unsigned char mac_addr[ETH_ALEN];

	if (argc != 3)
		return 1;

	if (mac_addr_a2n(mac_addr, argv[0])) {
		fprintf(stderr, "invalid mac address\n");
		return 2;
	}
	argc--;
	argv++;

	if (strcmp("mgmt_rts_cts", argv[0]) != 0)
		return 1;
	argc--;
	argv++;

	if (strcmp("on", argv[0]) == 0)
		rts_cts_conf = NL80211_MGMT_RTS_CTS_ENABLE;
	else if (strcmp("off", argv[0]) == 0)
		rts_cts_conf = NL80211_MGMT_RTS_CTS_DISABLE;
	else {
		fprintf(stderr, "unknown mgmt rts cts conf\n");
		return 2;
	}
	argc--;
	argv++;

	if (argc)
		return 1;

	NLA_PUT(msg, NL80211_ATTR_MAC, ETH_ALEN, mac_addr);
	NLA_PUT_U8(msg, NL80211_ATTR_STA_MGMT_RTS_CTS_CONFIG, rts_cts_conf);

	return 0;
nla_put_failure:
	return -ENOBUFS;
}

COMMAND_ALIAS(station, set, "<MAC address> mgmt_rts_cts <on|off>",
	NL80211_CMD_SET_STATION, 0, CIB_NETDEV, handle_station_set_mgmt_rts_cts,
	"Enable/Disable rts/cts for management frames",
	select_station_cmd, station_set_mgmt_rts_cts);

static int handle_station_dump(struct nl80211_state *state,
			       struct nl_msg *msg,
			       int argc, char **argv,
			       enum id_input id)
{
	register_handler(print_sta_handler, *argv);
	return 0;
}
COMMAND(station, dump, "[-v]",
	NL80211_CMD_GET_STATION, NLM_F_DUMP, CIB_NETDEV, handle_station_dump,
	"List all stations known, e.g. the AP on managed interfaces");
