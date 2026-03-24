#ifndef __PPA_API_DEBUG_H__20171109__
#define __PPA_API_DEBUG_H__20171109__

/* This is for ppa_filter stats. This will eliminate unnecessary 
prints when we do echo enable all > /proc/ppa/api/dbg and 
user can easily detect type and count of filtered packets 
for debugging purposes by doing a cat of proc entry*/

typedef struct {
	uint32_t ppa_is_pkt_host_output;
	uint32_t ppa_is_pkt_broadcast;
	uint32_t ppa_is_pkt_multicast;
	uint32_t ppa_is_pkt_loopback;
	uint32_t ppa_is_pkt_protocol_invalid;
	uint32_t ppa_is_pkt_fragment;
	uint32_t ppa_is_pkt_ignore_special_session;
	uint32_t ppa_is_pkt_not_routing;
	uint32_t ppa_is_filtered;
} PPA_FILTER_STATS;


/* new PPA_API Data Structure for Port-->QueueNum Mapping and Status Check */

typedef struct {
	uint8_t portid;
	int8_t num;
	char *status;
} PPA_PORT_QUEUE_NUM_STATUS;

extern PPA_PORT_QUEUE_NUM_STATUS ppa_port_qnum[PPA_MAX_PORT_NUM];
extern PPA_FILTER_STATS ppa_filter;
extern PPA_PORT_QUEUE_NUM_STATUS ppa_port_qnum[PPA_MAX_PORT_NUM];


#endif  //  __PPA_API_DEBUG_H__20171109__

