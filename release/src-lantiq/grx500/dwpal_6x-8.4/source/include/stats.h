#ifndef __STATS_H__
#define __STATS_H__

#include "common.h"
#include "vendor_cmds_copy.h"

#define HAL_NUM_OF_ANTS             (4)
/* Length of IEEE address (bytes) */
#define IEEE_ADDR_LEN                   (6)

/* This type is used for Source and Destination MAC addresses and also as   */
/* unique identifiers for Stations and Networks.                            */

typedef struct _IEEE_ADDR
{
    unsigned char au8Addr[IEEE_ADDR_LEN]; /* WARNING: Special case! No padding here! This structure must be padded externally! */
} IEEE_ADDR;

#define MAC_PRINTF_FMT "%02X:%02X:%02X:%02X:%02X:%02X"

#define __BYTE_ARG(b,i)            (((unsigned char *)(b))[i])
#define __BYTE_ARG_TYPE(t,b,i)     ((t)__BYTE_ARG((b),(i)))
#define _BYTE_ARG_U(b,i)            __BYTE_ARG_TYPE(unsigned int,(b),(i))
#define MAC_PRINTF_ARG(x) \
  _BYTE_ARG_U((x), 0),_BYTE_ARG_U((x), 1),_BYTE_ARG_U((x), 2),_BYTE_ARG_U((x), 3),_BYTE_ARG_U((x), 4),_BYTE_ARG_U((x), 5)

#define MAX_NL_REPLY 8192
#define MAX_LEN_VALID_VALUE 1024
#define MAX_USAGE_LEN 128
#define MAX_COMMAND_LEN 64
#define num_stats(X) (sizeof(X)/sizeof(stats))
#define PRINT_DESCRIPTION(x)	printf("%s\n",x.description);
#define INDENTATION(x)		if(x>0)\
					{while(x) { printf("  "); x--;}};

typedef struct
{
	unsigned char  mcs;
	unsigned char  nss;
	unsigned char  gi;
	unsigned char  reserved1;
	unsigned short bw;
	unsigned short reserved2;
	unsigned int   per;
} packet_error_rate_t;

typedef struct peer_list{
	IEEE_ADDR addr;
	unsigned int is_sta_auth;
}peer_list_t;

typedef enum {
	PEER_FLOW_STATS=0,
	PEER_TRAFFIC_STAT,
	RETRANS_STAT,
	TR181_PEER_STATS,
	NETWORK_BITFIELD,
	PEER_CAPABILITY,
	VENDOR_ENUM,
	PEER_RATE_INFO,
	PEER_RATE_INFO1,
	PHY_ENUM,
	RECOVERY_STAT,
	HW_TXM_STAT,
	TRAFFIC_STATS,
	MGMT_STATS,
	HW_FLOW_STATUS,
	TR181_ERROR_STATS,
	TR181_WLAN_STATS,
	TR181_HW_STATS,
	PACKET_ERROR_RATE,
	GI_ENUM,
	PTA_STATS,
	PEER_LIST
}stat_id;

typedef struct 
{
	char cmd[MAX_COMMAND_LEN]; //command name
	enum ltq_nl80211_vendor_subcmds id; //NL command
	int num_arg; // number of arguments expected
	char usage[MAX_USAGE_LEN];
	stat_id c; //enum for each cmd
}stats_cmd;

typedef enum
{
	UCHAR=0,
	CHAR,
	BYTE,
	UINT,
	INT,
	LONG,
	SLONG,
	SLONGARRAY,
	SBYTEARRAY,
	FLAG,
	BITFIELD,
	ENUM,
	TIMESTAMP,
	LONGFRACT,
	SLONGFRACT,
	HUGE,
	NONE	
}type;

typedef struct
{
	stat_id c;
	char description[MAX_LEN_VALID_VALUE];
	type t;
	int element;
}stats;

struct print_struct {
	stat_id st;
	stats *sts;
	int size;
};

stats network_bitfield[] = {
{NETWORK_BITFIELD,	"802.11a",		BITFIELD,	0}, 
{NETWORK_BITFIELD,	"802.11b",		BITFIELD,	1}, 
{NETWORK_BITFIELD,	"802.11g",		BITFIELD,	2}, 
{NETWORK_BITFIELD,	"802.11n",		BITFIELD,	3}, 
{NETWORK_BITFIELD,	"802.11ac",		BITFIELD,	4},
{NETWORK_BITFIELD,	"802.11ax",		BITFIELD,	5}
};

stats phy_enum[] = {
{PHY_ENUM,	"802.11a/g",	ENUM,	0}, 
{PHY_ENUM,	"802.11b",	ENUM,	1}, 
{PHY_ENUM,	"802.11n",	ENUM,	2}, 
{PHY_ENUM,	"802.11ac",	ENUM,	3},
{PHY_ENUM,	"802.11ax",	ENUM,	4}
};

stats vendor_enum[] = {
{VENDOR_ENUM,	"Unknown",	ENUM,	0},
{VENDOR_ENUM,	"Lantiq",	ENUM,	1},
{VENDOR_ENUM,	"W101",		ENUM,	2}
};

stats gi_enum[] = {
{GI_ENUM,       "Short",        ENUM,   0},
{GI_ENUM,       "Long",         ENUM,   1},
};

stats peer_traffic_stat[] = {
{PEER_TRAFFIC_STAT,	"Peer Traffic Statistics",			                        NONE,	0},
{PEER_TRAFFIC_STAT,	"BytesSent                - Number of bytes sent successfully",		LONG,	0},
{PEER_TRAFFIC_STAT,	"BytesReceived            - Number of bytes received",			LONG,	0},
{PEER_TRAFFIC_STAT,	"PacketsSent              - Number of packets transmitted",		LONG,	0},
{PEER_TRAFFIC_STAT,	"PacketsReceived          - Number of packets received",		LONG,	0}	
};

stats peer_retrans_stat[] = {
{RETRANS_STAT,		"Retransmition Statistics",		                                                    	NONE,	0},
{RETRANS_STAT,		"Retransmissions          - Number of re-transmitted, from the last 100 packets sent",      	LONG,	0},
{RETRANS_STAT,		"RetransCount             - Total number of transmitted packets which were retransmissions",	LONG,	0},
{RETRANS_STAT,		"RetryCount               - Number of Tx packets succeeded after one or more retransmissions",  LONG,	0},
{RETRANS_STAT,		"MultipleRetryCount       - Number of Tx packets succeeded after more than one retransmission",	LONG,	0},
{RETRANS_STAT,		"FailedRetransCount       - Number of Tx packets dropped because of retry limit exceeded",	LONG,	0}
};

stats tr181_peer_stat[] = {
{TR181_PEER_STATS,	"TR-181 Device.WiFi.AccessPoint.{i}.AssociatedDevice",					NONE,	0},
{TR181_PEER_STATS,	"StationID",					        				LONG,	0},
{NETWORK_BITFIELD,	"OperatingStandard    - Supported network modes",       				NONE,	0},
{PEER_TRAFFIC_STAT,	"Traffic statistics",				        				NONE,	0},
{RETRANS_STAT,		"Retransmission statistics",			        				NONE,	0},
{TR181_PEER_STATS,	"ErrorsSent           - Number of Tx packets not transmitted because of errors",	LONG,	0},
{TR181_PEER_STATS,	"LastDataDownlinkRate - Last data transmit rate (to peer) [kbps]",			LONG,	0},
{TR181_PEER_STATS,	"LastDataUplinkRate   - Last data receive rate (from peer) [kbps]",			LONG,	0},
{TR181_PEER_STATS,	"SignalStrength       - Radio signal strength of the uplink [dBm]",			SLONG,	0}
};

stats peer_flow_stats[] = {
{PEER_FLOW_STATS,	"Peer packets flow statistics",						NONE,		0},
{TR181_PEER_STATS,	"TR-181 statistics",				                        NONE,		0},
{PEER_FLOW_STATS,	"ShortTermRSSI        - Short-term RSSI average per antenna [dBm]",	SLONGARRAY,	4},
{PEER_FLOW_STATS,	"SNR                  - Signal to Noise ratio per antenna [dB]",	SBYTEARRAY,	4},
{PEER_FLOW_STATS,	"AirtimeEfficiency    - Efficiency of used air time [bytes/sec]",	LONG,		0},
{PEER_FLOW_STATS,	"AirtimeUsage         - Air Time Used by RX/TX to/from STA [%]",	BYTE,		0}
};

stats peer_rate_info1[] = {
{PEER_RATE_INFO1,	"Rate info is valid",	  FLAG,	0},
{PHY_ENUM,		"Network (Phy) Mode",	  NONE,	0},
{PEER_RATE_INFO1,	"BW index",		        SLONG,	0},
{PEER_RATE_INFO1,	"BW [MHz]",		        SLONG,	0},
{PEER_RATE_INFO1,	"SGI",			          SLONG,	0},
{PEER_RATE_INFO1,	"MCS index",		      SLONG,	0},
{PEER_RATE_INFO1,	"NSS",			          SLONG,	0}
};

stats peer_rate_info[] = {
{PEER_RATE_INFO,	"Peer TX/RX info",			NONE,		0},
{PEER_RATE_INFO1,	"Mgmt uplink rate info",		NONE,		0},
{PEER_RATE_INFO,	"Last mgmt uplink rate [Mbps]",		LONGFRACT,	1},
{PEER_RATE_INFO1,	"Data uplink rate info",		NONE,		0},
{PEER_RATE_INFO,	"Last data uplink rate [Mbps]",		LONGFRACT,	1},
{PEER_RATE_INFO1,	"Data downlink rate info",		NONE,		0},
{PEER_RATE_INFO,	"Last data downlink rate [Mbps]",	LONGFRACT,	1},
{PEER_RATE_INFO,	"Beamforming mode",			LONG,		0},
{PEER_RATE_INFO,	"STBC mode",				LONG,		0},
{PEER_RATE_INFO,	"TX power for current rate [dBm]",	LONGFRACT,	2},
{PEER_RATE_INFO,	"TX management power       [dBm]",	LONGFRACT,	2}
};

stats peer_capability[] = {
{PEER_CAPABILITY,	"Peer capabilities",			NONE,		0},
{NETWORK_BITFIELD,	"Supported network modes",		NONE,		0},
{PEER_CAPABILITY,	"WMM is supported",			FLAG,		0},
{PEER_CAPABILITY,	"Channel bonding supported",		FLAG,		0},
{PEER_CAPABILITY,	"SGI20 supported",			FLAG,		0},
{PEER_CAPABILITY,	"SGI40 supported",			FLAG,		0},
{PEER_CAPABILITY,	"STBC supported",	                FLAG,		0}, 
{PEER_CAPABILITY,	"LDPC supported",	                FLAG,		0},
{PEER_CAPABILITY,	"Explicit beam forming supported",	FLAG,		0},
{PEER_CAPABILITY,	"40MHz intolerant",			FLAG,		0},
{VENDOR_ENUM,		"Vendor",				NONE,		0},
{PEER_CAPABILITY,	"Tx STBC support",	        	LONG,		0},
{PEER_CAPABILITY,	"Rx STBC support",      		LONG,		0},
{PEER_CAPABILITY,	"Maximum A-MPDU Length Exponent",	LONG,		0},
{PEER_CAPABILITY,	"Minimum MPDU Start Spacing",		LONG,		0},
{PEER_CAPABILITY,	"Timestamp of station association",	TIMESTAMP,	0}
};

stats recovery_stat[] =
{
{RECOVERY_STAT,		"Recovery statistics",					NONE,	0},
{RECOVERY_STAT,		"Number of FAST recovery processed successfully",	LONG,	0},
{RECOVERY_STAT,		"Number of FULL recovery processed successfully",	LONG,	0},
{RECOVERY_STAT,		"Number of FAST recovery failed",			LONG,	0},
{RECOVERY_STAT,		"Number of FULL recovery failed",			LONG,	0}
};

stats hw_txm_stat[] = {
{HW_TXM_STAT,		"HW TXM Statistics",					NONE,	0},
{HW_TXM_STAT,		"Number of FW MAN messages sent",			LONG,	0},
{HW_TXM_STAT,		"Number of FW MAN messages confirmed",			LONG,	0},
{HW_TXM_STAT,		"Peak number of FW MAN messages sent simultaneously",	LONG,	0},
{HW_TXM_STAT,		"Number of FW DBG messages sent",			LONG,	0},
{HW_TXM_STAT,		"Number of FW DBG messages confirmed",			LONG,	0},
{HW_TXM_STAT,		"Peak number of FW DBG messages sent simultaneously",	LONG,	0}
};

stats traffic_stats[] = {
{TRAFFIC_STATS,		"Traffic Statistics",				                                NONE,	0},
{TRAFFIC_STATS,		"BytesSent                - Number of bytes sent successfully (64-bit)",	HUGE,	0},
{TRAFFIC_STATS,		"BytesReceived            - Number of bytes received (64-bit)",			HUGE,	0},
{TRAFFIC_STATS,		"PacketsSent              - Number of packets transmitted (64-bit)",		HUGE,	0},
{TRAFFIC_STATS,		"PacketsReceived          - Number of packets received (64-bit)",		HUGE,	0},
{TRAFFIC_STATS,		"UnicastPacketsSent       - Number of unicast packets transmitted",		LONG,	0},
{TRAFFIC_STATS,		"UnicastPacketsReceived   - Number of unicast packets received",		LONG,	0},
{TRAFFIC_STATS,		"MulticastPacketsSent     - Number of multicast packets transmitted",		LONG,	0},
{TRAFFIC_STATS,		"MulticastPacketsReceived - Number of multicast packets received",		LONG,	0},
{TRAFFIC_STATS,		"BroadcastPacketsSent     - Number of broadcast packets transmitted",		LONG,	0},
{TRAFFIC_STATS,		"BroadcastPacketsReceived - Number of broadcast packets received",		LONG,	0}
};

stats mgmt_stats[] = {
{MGMT_STATS,		"Management frames statistics",					NONE,	0},
{MGMT_STATS,		"Number of management frames in reserved queue",		LONG,	0},
{MGMT_STATS,		"Number of management frames sent",		                LONG,	0},
{MGMT_STATS,		"Number of management frames confirmed",			LONG,	0},
{MGMT_STATS,		"Number of management frames received",				LONG,	0},
{MGMT_STATS,		"Number of management frames dropped due to retries",		LONG,	0},
{MGMT_STATS,		"Number of management frames dropped due to TX que full",	LONG,	0},
{MGMT_STATS,		"Number of probe responses sent",                         	LONG,0},
{MGMT_STATS,		"Number of probe responses dropped",                      	LONG,0}
};

stats hw_flow_status[] = {
{RECOVERY_STAT,		"HW Recovery Statistics",		NONE,	0},
{HW_TXM_STAT,		"HW TXM statistics",			NONE,	0},
{TRAFFIC_STATS,		"Radio Traffic statistics",		NONE,	0},
{MGMT_STATS,		"Radio MGMT statistics",		NONE,	0},
{HW_FLOW_STATUS,	"Radars detected",	                LONG,	0},
{HW_FLOW_STATUS,	"Channel Load [%]",	                BYTE,	0},
{HW_FLOW_STATUS,	"Channel Utilization [%]",		BYTE,	0},
{HW_FLOW_STATUS,	"Total Airtime [%]",			BYTE,	0},
{HW_FLOW_STATUS,	"Total Airtime Efficiency [bytes/sec]",	LONG,	0}
};

stats tr181_error_stats[] = {
{TR181_ERROR_STATS,	"TR-181 Errors",			                                                NONE,	0},
{TR181_ERROR_STATS,	"ErrorsSent               - Number of Tx packets not transmitted because of errors",	LONG,	0},
{TR181_ERROR_STATS,	"ErrorsReceived           - Number of Rx packets that contained errors",		LONG,	0},
{TR181_ERROR_STATS,	"DiscardPacketsSent       - Number of Tx packets discarded",				LONG,	0},
{TR181_ERROR_STATS,	"DiscardPacketsReceived   - Number of Rx packets discarded",				LONG,	0}
};

stats tr181_wlan_stats[] = {
{TR181_WLAN_STATS,  	"TR-181 Device.WiFi.SSID.{i}.Stats",							NONE,	0},
{TRAFFIC_STATS,		"Traffic Statistics",			                                                NONE,	0},
{TR181_ERROR_STATS,	"Erros Statistics",									NONE,	0},
{RETRANS_STAT,		"Retransmission statistics",								NONE,	0},
{TR181_WLAN_STATS,	"ACKFailureCount             - Number of expected ACKs never received",			LONG,	0},
{TR181_WLAN_STATS,	"AggregatedPacketCount       - Number of aggregated packets transmitted",		LONG,	0},
{TR181_WLAN_STATS,	"UnknownProtoPacketsReceived - Number of Rx packets unknown or unsupported protocol",	LONG,	0}
};

stats tr181_hw_stats[] = {
{TR181_HW_STATS,	"TR-181 Device.WiFi.Radio.{i}.Stats",						NONE,	0},
{TRAFFIC_STATS,		"Traffic Statistics",			                                        NONE,	0},
{TR181_ERROR_STATS,	"Erros Statistics",			                                        NONE,	0},
{TR181_HW_STATS,	"FCSErrorCount            - Number of Rx packets with detected FCS error",	LONG,	0},
{TR181_HW_STATS,	"Noise                    - Average noise strength received [dBm]",		SLONG,	0}
};

stats packet_error_rate[] = {
{PACKET_ERROR_RATE,     "PER per rate",                                                                 NONE,   0},
{PACKET_ERROR_RATE,     "MCS",                                                                          BYTE,   0},
{PACKET_ERROR_RATE,     "NSS",                                                                          BYTE,   0},
{GI_ENUM,               "GI",                                                                           ENUM,   0},
{PACKET_ERROR_RATE,     "BW",                                                                           LONG,   0},
{PACKET_ERROR_RATE,     "PER",                                                                          LONG,   0}
};

stats pta_stats[] = {
{PTA_STATS,		"PTA Stats: Priority GPIO",							NONE,   0},
{PTA_STATS,		"PriorityIsHigh",								LONG,   0},
{PTA_STATS,		"PriorityIsLow",								LONG,   0}
};

struct print_struct gStat[] = {
{PEER_FLOW_STATS,	peer_flow_stats,	num_stats(peer_flow_stats)},
{PEER_TRAFFIC_STAT,	peer_traffic_stat,	num_stats(peer_traffic_stat)},
{RETRANS_STAT,		peer_retrans_stat,	num_stats(peer_retrans_stat)},
{TR181_PEER_STATS,	tr181_peer_stat,	num_stats(tr181_peer_stat)},
{NETWORK_BITFIELD,	network_bitfield,	num_stats(network_bitfield)},
{PEER_CAPABILITY,	peer_capability,	num_stats(peer_capability)},
{VENDOR_ENUM,		vendor_enum,		num_stats(vendor_enum)},
{PEER_RATE_INFO,	peer_rate_info,		num_stats(peer_rate_info)},
{PEER_RATE_INFO1,	peer_rate_info1,	num_stats(peer_rate_info1)},
{PHY_ENUM,		phy_enum,		num_stats(phy_enum)},
{RECOVERY_STAT,		recovery_stat,		num_stats(recovery_stat)},
{HW_TXM_STAT,		hw_txm_stat,		num_stats(hw_txm_stat)},
{TRAFFIC_STATS,		traffic_stats,		num_stats(traffic_stats)},
{MGMT_STATS,		mgmt_stats,		num_stats(mgmt_stats)},
{HW_FLOW_STATUS,	hw_flow_status,		num_stats(hw_flow_status)},
{TR181_ERROR_STATS,	tr181_error_stats,	num_stats(tr181_error_stats)},
{TR181_WLAN_STATS,	tr181_wlan_stats,	num_stats(tr181_wlan_stats)},
{TR181_HW_STATS,	tr181_hw_stats,		num_stats(tr181_hw_stats)},
{PACKET_ERROR_RATE,	packet_error_rate,	num_stats(packet_error_rate)},
{GI_ENUM,			gi_enum,			num_stats(gi_enum)},
{PTA_STATS,			pta_stats,			num_stats(pta_stats)}
};

stats_cmd gCmd[] =
{
{"peerlist", LTQ_NL80211_VENDOR_SUBCMD_GET_PEER_LIST, 1, 
		"usage: dwpal_cli [INTERFACENAME] PeerList",PEER_LIST},
{"peerflowstatus", LTQ_NL80211_VENDOR_SUBCMD_GET_PEER_FLOW_STATUS, 2, 
		"usage: dwpal_cli [INTERFACENAME] PeerFlowStatus [MACADDR]", PEER_FLOW_STATS},
{"peercapabilities", LTQ_NL80211_VENDOR_SUBCMD_GET_PEER_CAPABILITIES, 2, 
		"usage: dwpal_cli [INTERFACENAME] PeerCapabilities [MACADDR]", PEER_CAPABILITY},
{"peerratesinfo", LTQ_NL80211_VENDOR_SUBCMD_GET_PEER_RATE_INFO, 2, 
		"usage: dwpal_cli [INTERFACENAME] PeerRatesInfo [MACADDR]", PEER_RATE_INFO},
{"recoverystats", LTQ_NL80211_VENDOR_SUBCMD_GET_RECOVERY_STATS, 1,
		"usage: dwpal_cli [INTERFACENAME] RecoveryStats", RECOVERY_STAT},
{"hwflowstatus", LTQ_NL80211_VENDOR_SUBCMD_GET_HW_FLOW_STATUS, 1,
		"usage: dwpal_cli [INTERFACENAME] HWFlowStatus", HW_FLOW_STATUS},
{"tr181wlanstat", LTQ_NL80211_VENDOR_SUBCMD_GET_TR181_WLAN_STATS, 1,
		"usage: dwpal_cli [INTERFACENAME] TR181WLANStat", TR181_WLAN_STATS},
{"tr181hwstat", LTQ_NL80211_VENDOR_SUBCMD_GET_TR181_HW_STATS, 1,
		"usage: dwpal_cli [INTERFACENAME] TR181HWStat", TR181_HW_STATS},
{"tr181peerstat", LTQ_NL80211_VENDOR_SUBCMD_GET_TR181_PEER_STATS, 2,
		"usage: dwpal_cli [INTERFACENAME] TR181PeerStat [MACADDR]", TR181_PEER_STATS},
{"packeterrorrate", -1, 2,
		"usage: dwpal_cli [INTERFACENAME] PacketErrorRate [MACADDR]", PACKET_ERROR_RATE},
{"ptastats", LTQ_NL80211_VENDOR_SUBCMD_GET_PRIORITY_GPIO, 1,
		"usage: dwpal_cli [INTERFACENAME] PTAStats", PTA_STATS},
};


int check_and_dump_packet_error_rate(char *cmd[]);
int check_stats_cmd(int argc,char *argv[]);
#endif
