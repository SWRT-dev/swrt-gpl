/******************************************************************************

                         Copyright (c) 2012, 2014, 2015
                        Lantiq Beteiligungs-GmbH & Co. KG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/

#ifndef __QOS_AL_CL_API_H /* [ */
#define __QOS_AL_CL_API_H

/*! \file qosal_cl_api.h
    \brief File contains the related data
           structures and API for 
           QoS Classification functionality
*/

/** \defgroup FAPI_QOS_GENERAL General
*   @ingroup FAPI_QOS
    \brief It provides a set of API for general configuration like, 
           enable/disable QoS, setting logging level etc.
*/

/** \defgroup FAPI_QOS_CLASS Classification
*   @ingroup FAPI_QOS
    \brief It provides a set of API to classify packets coming into the CPE device,
           and perform actions like assign tc, dscp mark, drop etc
*/

/** \addtogroup FAPI_QOS_CLASS */
/* @{ */
/*!
  \brief QOS_AL_CL_OK */
#define QOS_AL_CL_OK			0
/*!
  \brief QOS_AL_CL_ERROR */
#define QOS_AL_CL_ERROR			-1

/*!
  \brief CL remark outer VLAN header's priority
*/
#define CL_OUTER_PCP_MARK		0x1
/*!
  \brief CL remark outer VLAN header's tag
*/
#define CL_VLAN_OUTER_VLAN_TAG		0x2
/*!
  \brief CL have no outer VLAN header
*/
#define CL_VLAN_OUTER_VLAN_UNTAG	0x4
/*!
  \brief CL remark inner VLAN header's priority
*/
#define CL_VLAN_INNER_PCP_MARK		0x8
/*!
  \brief CL remark inner VLAN header's tag
*/
#define CL_VLAN_INNER_VLAN_TAG		0x10
/*!
  \brief CL have no inner VLAN header
*/
#define CL_VLAN_INNER_VLAN_UNTAG	0x20


/*!
  \brief CL Policer Actions
  \note Type of meter is mutually exclusive
*/
#define CL_POLICER_ACTION_NULL			0x1
#define CL_POLICER_ACTION_DROP			0x2
#define CL_POLICER_ACTION_MARK_DSCP		0x4
#define CL_POLICER_ACTION_MARK_PCP		0x8

/*!
  \brief Maximum name length 
*/
#define MAX_QOS_NAME_LEN 128
/*!
  \brief Maximum vendor id length
*/
#define MAX_VENDOR_ID_LEN 128
/*!
  \brief Maximum vendor id V6 length
*/
#define MAX_VENDOR_ID_V6_LEN 128
/*!
  \brief Maximum client id length
*/
#define MAX_CLIENTID_LEN 128
/*!
  \brief Maximum user class id length
*/
#define MAX_USER_CLASSID_LEN 128
/*!
  \brief Maximum vendor info length
*/
#define MAX_VENDOR_INFO_LEN 128 
/*!
  \brief Maximum MAC address length
*/
#define MAX_MACADDR_LEN 32
/*!
  \brief Maximum classifier name length
*/
#define MAX_CL_NAME_LEN 32
/*!
  \brief Maximum classifier AP name
*/
#define MAX_CL_AP_NAME 32
/*!
  \brief Maximum length of IP address in string form
*/
#define IP_ADDRSTRLEN 48

/*! 
 \brief Classifier add filter
*/
#define CL_FILTER_ADD 0x00000001
/*! 
 \brief Classifier modify filter
*/
#define CL_FILTER_MODIFY 0x00000002
/*! 
 \brief Classifier delete filter
*/
#define CL_FILTER_DELETE 0x00000004
/*! 
 \brief Classifier delete based on exact match filter
*/
#define CL_FILTER_DELETE_EXACT 0x00000008
/*! 
 \brief Classifier get filter
*/
#define CL_FILTER_GET 0x00000010
/*! 
 \brief Classifier filter for LAN to LAN traffic
*/
#define CL_FILTER_L2L 0x00000020

/*! 
 \brief Bit-field that specifies classifier name set/unset in classifier->filter structure
*/
#define CLSCFG_PARAM_FILTER_NAME                    (1 << 0)    
/*! 
 \brief Bit-field that specifies classifier name set/unset in classifier->filter structure
*/
#define CLSCFG_PARAM_ORDER_NUM                      (1 << 1)   
/*! 
 \brief Bit-field that specifies source DHCP options set/unset in classifier->filter structure
*/
#define CLSCFG_PARAM_SRC_DHCP                       (1 << 2)
/*! 
 \brief Bit-field that specifies vendor field set/unset in classifier->filter->source DHCP structure
*/
#define CLSCFG_PARAM_SRC_DHCP_VENDOR_CLASS_ID       (1 << 0)       
/*! 
 \brief Bit-field that specifies vendor class mode field set/unset in classifier->filter->source DHCP structure
*/
#define CLSCFG_PARAM_SRC_DHCP_VENDOR_CLASS_ID_MODE  (1 << 1)             
/*! 
 \brief Bit-field that specifies vendor class field for IPv6 set/unset in classifier->filter->source DHCP structure
*/
#define CLSCFG_PARAM_SRC_DHCP_VENDOR_CLASS_ID_V6    (1 << 2)           
/*! 
 \brief Bit-field that specifies client field set/unset in classifier->filter->source DHCP structure
*/
#define CLSCFG_PARAM_SRC_DHCP_CLIENT_ID             (1 << 3) 
/*! 
 \brief Bit-field that specifies user class field set/unset in classifier->filter->source DHCP structure
*/
#define CLSCFG_PARAM_SRC_DHCP_USER_CLASS_ID         (1 << 4) 
/*! 
 \brief Bit-field that specifies vendor specific field set/unset in classifier->filter->source DHCP structure
*/
#define CLSCFG_PARAM_SRC_DHCP_VENDOR_SPECIFIC_INFO  (1 << 5) 
/*! 
 \brief Bit-field that specifies destination DHCP options set/unset in classifier->filter structure
*/
#define CLSCFG_PARAM_DST_DHCP                       (1 << 3) 
/*! 
 \brief Bit-field that specifies vendor field set/unset in classifier->filter->destination DHCP structure
*/
#define CLSCFG_PARAM_DST_DHCP_VENDOR_CLASS_ID       (1 << 0)    
/*! 
 \brief Bit-field that specifies vendor class mode field set/unset in classifier->filter->destination DHCP structure
*/
#define CLSCFG_PARAM_DST_DHCP_VENDOR_CLASS_ID_MODE  (1 << 1)   
/*! 
 \brief Bit-field that specifies vendor class field for IPv6 set/unset in classifier->filter->destination DHCP structure
*/
#define CLSCFG_PARAM_DST_DHCP_VENDOR_CLASS_ID_V6    (1 << 2)  
/*! 
 \brief Bit-field that specifies client field set/unset in classifier->filter->destination DHCP structure
*/
#define CLSCFG_PARAM_DST_DHCP_CLIENT_ID             (1 << 3) 
/*! 
 \brief Bit-field that specifies user class field set/unset in classifier->filter->destination DHCP structure
*/
#define CLSCFG_PARAM_DST_DHCP_USER_CLASS_ID         (1 << 4) 
/*! 
 \brief Bit-field that specifies vendor specific field set/unset in classifier->filter->destination DHCP structure
*/
#define CLSCFG_PARAM_DST_DHCP_VENDOR_SPECIFIC_INFO  (1 << 5) 
/*! 
 \brief Bit-field that specifies source interface set/unset in classifier->filter structure
*/
#define CLSCFG_PARAM_RX_IF                          (1 << 4) 
/*! 
 \brief Bit-field that specifies destination interface set/unset in classifier->filter structure
*/
#define CLSCFG_PARAM_TX_IF                          (1 << 5) 
/*! 
 \brief Bit-field that specifies source MAC address set/unset in classifier->filter structure
*/
#define CLSCFG_PARAM_SRC_MAC                        (1 << 6) 
/*! 
 \brief Bit-field that specifies MAC address set/unset in classifier->filter->source MAC structure
*/
#define CLSCFG_PARAM_SRC_MAC_ADDRS                  (1 << 0) 
/*! 
 \brief Bit-field that specifies MAC address mask set/unset in classifier->filter->source MAC structure
*/
#define CLSCFG_PARAM_SRC_MAC_MASK                   (1 << 1) 
/*! 
 \brief Bit-field that specifies destination MAC address set/unset in classifier->filter structure
*/
#define CLSCFG_PARAM_DST_MAC                        (1 << 7) 
/*! 
 \brief Bit-field that specifies MAC address set/unset in classifier->filter->destination MAC structure
*/
#define CLSCFG_PARAM_DST_MAC_ADDRS                  (1 << 0) 
/*! 
 \brief Bit-field that specifies MAC address mask set/unset in classifier->filter->destination MAC structure
*/
#define CLSCFG_PARAM_DST_MAC_MASK                   (1 << 1) 
/*! 
 \brief Bit-field that specifies ethernet header options set/unset in classifier->filter structure
*/
#define CLSCFG_PARAM_ETH_HDR                        (1 << 8)  
/*! 
 \brief Bit-field that specifies type field set/unset in classifier->filter->ethernet header structure
*/
#define CLSCFG_PARAM_ETH_HDR_TYPE                   (1 << 0) 
/*! 
 \brief Bit-field that specifies type field set/unset in classifier->filter->ethernet header structure
*/
#define CLSCFG_PARAM_ETH_HDR_DSAP                   (1 << 1) 
/*! 
 \brief Bit-field that specifies type field set/unset in classifier->filter->ethernet header structure
*/
#define CLSCFG_PARAM_ETH_HDR_SSAP                   (1 << 2) 
/*! 
 \brief Bit-field that specifies type field set/unset in classifier->filter->ethernet header structure
*/
#define CLSCFG_PARAM_ETH_HDR_LLC_CONTROL            (1 << 3) 
/*! 
 \brief Bit-field that specifies type field set/unset in classifier->filter->ethernet header structure
*/
#define CLSCFG_PARAM_ETH_HDR_OUI                    (1 << 4) 
/*! 
 \brief Bit-field that specifies outer header header's VLAN priority field set/unset in classifier->filter->ethernet header structure
*/
#define CLSCFG_PARAM_ETH_HDR_PRIO                   (1 << 5) 
/*! 
 \brief Bit-field that specifies outer header's VLAN Id field set/unset in classifier->filter->ethernet header structure
*/
#define CLSCFG_PARAM_ETH_HDR_VLANID                 (1 << 6) 
/*! 
 \brief Bit-field that specifies outer header's DEI field set/unset in classifier->filter->ethernet header structure
*/
#define CLSCFG_PARAM_ETH_HDR_VLAN_DEI               (1 << 7) 
/*! 
 \brief Bit-field that specifies inner header's VLAN priority field set/unset in classifier->filter->ethernet header structure
*/
#define CLSCFG_PARAM_ETH_HDR_INNER_PRIO             (1 << 8) 
/*! 
 \brief Bit-field that specifies inner header's VLAN Id field set/unset in classifier->filter->ethernet header structure
*/
#define CLSCFG_PARAM_ETH_HDR_INNER_VLANID           (1 << 9) 
/*! 
 \brief Bit-field that specifies inner header's VLAN DEI field set/unset in classifier->filter->ethernet header structure
*/
#define CLSCFG_PARAM_ETH_HDR_INNER_VLAN_DEI         (1 << 10)
/*! 
 \brief Bit-field that specifies outer IP header options set/unset in classifier->filter structure
*/
#define CLSCFG_PARAM_OUTER_IP                (1 << 9)
/*! 
 \brief Bit-field that specifies source IP address field set/unset in classifier->filter->outer IP header structure
*/
#define CLSCFG_PARAM_OUTER_IP_SRC_IP         (1 << 0)
/*! 
 \brief Bit-field that specifies destination IP address field set/unset in classifier->filter->outer IP header structure
*/
#define CLSCFG_PARAM_OUTER_IP_DST_IP         (1 << 1)
/*! 
 \brief Bit-field that specifies source port field set/unset in classifier->filter->outer IP header structure
*/
#define CLSCFG_PARAM_OUTER_IP_SRC_PORT       (1 << 2)
/*! 
 \brief Bit-field that specifies destination port field set/unset in classifier->filter->outer IP header structure
*/
#define CLSCFG_PARAM_OUTER_IP_DST_PORT       (1 << 3)
/*! 
 \brief Bit-field that specifies protocol field set/unset in classifier->filter->outer IP header structure
*/
#define CLSCFG_PARAM_OUTER_IP_PROTO          (1 << 4)
/*! 
 \brief Bit-field that specifies inner IP header options set/unset in classifier->filter structure
*/
#define CLSCFG_PARAM_INNER_IP               (1 << 10) 
/*! 
 \brief Bit-field that specifies source IP address field set/unset in classifier->filter->innter IP header structure
*/
#define CLSCFG_PARAM_INNER_IP_SRC_IP        (1 << 0)
/*! 
 \brief Bit-field that specifies destination IP address field set/unset in classifier->filter->inner IP header structure
*/
#define CLSCFG_PARAM_INNER_IP_DST_IP        (1 << 1)
/*! 
 \brief Bit-field that specifies source port field set/unset in classifier->filter->inner IP header structure
*/
#define CLSCFG_PARAM_INNER_IP_SRC_PORT      (1 << 2)
/*! 
 \brief Bit-field that specifies destination port field set/unset in classifier->filter->inner IP header structure
*/
#define CLSCFG_PARAM_INNER_IP_DST_PORT      (1 << 3)
/*! 
 \brief Bit-field that specifies protocol field set/unset in classifier->filter->inner IP header structure
*/
#define CLSCFG_PARAM_INNER_IP_PROTO         (1 << 4)
/*! 
 \brief Bit-field that specifies generic IP header fields set/unset in classifier->filter structure
*/
#define CLSCFG_PARAM_IP_HDR                       (1 << 11) 
/*! 
 \brief Bit-field that specifies length field set/unset in classifier->filter->generic IP header structure
*/
#define CLSCFG_PARAM_IP_HDR_LEN_MIN               (1 << 0) 
/*! 
 \brief Bit-field that specifies maximum length field set/unset in classifier->filter->generic IP header structure
*/
#define CLSCFG_PARAM_IP_HDR_LEN_MAX               (1 << 1)
/*! 
 \brief Bit-field that specifies DSCP field set/unset in classifier->filter->generic IP header structure
*/
#define CLSCFG_PARAM_IP_HDR_DSCP                  (1 << 2)
/*! 
 \brief Bit-field that specifies TCP header fields set/unset in classifier->filter structure
*/
#define CLSCFG_PARAM_TCP_HDR                    (1 << 12) 
/*! 
 \brief Bit-field that specifies ACK flag field set/unset in classifier->filter->TCP header structure
*/
#define CLSCFG_PARAM_TCP_HDR_ACK                (1 << 0)
/*! 
 \brief Bit-field that specifies SYN flag field set/unset in classifier->filter->TCP header structure
*/
#define CLSCFG_PARAM_TCP_HDR_SYN                (1 << 1)
/*! 
 \brief Bit-field that specifies FIN flag field set/unset in classifier->filter->TCP header structure
*/
#define CLSCFG_PARAM_TCP_HDR_FIN                (1 << 2)
/*! 
 \brief Bit-field that specifies RST flag field set/unset in classifier->filter->TCP header structure
*/
#define CLSCFG_PARAM_TCP_HDR_RST                (1 << 3)
/*! 
 \brief Bit-field that specifies URG flag field set/unset in classifier->filter->TCP header structure
*/
#define CLSCFG_PARAM_TCP_HDR_URG                (1 << 4)
/*! 
 \brief Bit-field that specifies application protocol fields set/unset in classifier->filter structure
*/
#define CLSCFG_PARAM_APP                        (1 << 13)
/*! 
 \brief Bit-field that specifies Id set/unset in classifier->filter->application structure
*/
#define CLSCFG_PARAM_APP_ID                     (1 << 0)
/*! 
 \brief Bit-field that specifies sub-protocol Id set/unset in classifier->filter->application structure
*/
#define CLSCFG_PARAM_SUBPROTO_ID                (1 << 1)
/*! 
 \brief Bit-field that specifies action fields set/unset in classifier->filter structure
*/
#define CLSCFG_PARAM_ACT                        (1 << 14) 
/*! 
 \brief Bit-field that specifies traffic class field set/unset in classifier->action structure
*/
#define CLSCFG_PARAM_ACT_TC                     (1 << 0) 
/*! 
 \brief Bit-field that specifies drop field set/unset in classifier->filter->action structure
*/
#define CLSCFG_PARAM_ACT_DROP                   (1 << 1)
/*! 
 \brief Bit-field that specifies DSCP remarking field set/unset in classifier->action structure
*/
#define CLSCFG_PARAM_ACT_DSCP                   (1 << 2) 
/*! 
 \brief Bit-field that specifies VLAN action fields set/unset in classifier->action structure
*/
#define CLSCFG_PARAM_ACT_VLAN                   (1 << 15)
/*! 
 \brief Bit-field that specifies outer header's VLAN priority remarking field set/unset in classifier->action->vlan structure
*/
#define CLSCFG_PARAM_ACT_VLAN_PCP               (1 << 0)
/*! 
 \brief Bit-field that specifies outer header's VLAN Id remarking field set/unset in classifier->action->vlan structure
*/
#define CLSCFG_PARAM_ACT_VLAN_ID                (1 << 1)
/*! 
 \brief Bit-field that specifies inner header's VLAN priority remarking field set/unset in classifier->action->vlan structure
*/
#define CLSCFG_PARAM_ACT_VLAN_PCP_INNER         (1 << 2) 
/*! 
 \brief Bit-field that specifies inner header's VLAN Id remarking field set/unset in classifier->action->vlan structure
*/
#define CLSCFG_PARAM_ACT_VLAN_ID_INNER          (1 << 3)
/*! 
 \brief Bit-field that specifies policer action fields set/unset in classifier->action structure
*/
#define CLSCFG_PARAM_ACT_POLICER                (1 << 16) 
/*! 
 \brief Bit-field that specifies name field set/unset in classifier->action->policer structure
*/
#define CLSCFG_PARAM_ACT_POLICER_NAME           (1 << 0)
/*! 
 \brief Bit-field that specifies mtr type field set/unset in classifier->action->policer structure
*/
#define CLSCFG_PARAM_ACT_POLICER_MTR_TYPE       (1 << 1)
/*! 
 \brief Bit-field that specifies cr field set/unset in classifier->action->policer structure
*/
#define CLSCFG_PARAM_ACT_POLICER_CR             (1 << 2)
/*! 
 \brief Bit-field that specifies cbs field set/unset in classifier->action->policer structure
*/
#define CLSCFG_PARAM_ACT_POLICER_CBS            (1 << 3)
/*! 
 \brief Bit-field that specifies ebs field set/unset in classifier->action->policer structure
*/
#define CLSCFG_PARAM_ACT_POLICER_EBS            (1 << 4)
/*! 
 \brief Bit-field that specifies pr field set/unset in classifier->action->policer structure
*/
#define CLSCFG_PARAM_ACT_POLICER_PR             (1 << 5)
/*! 
 \brief Bit-field that specifies pbs field set/unset in classifier->action->policer structure
*/
#define CLSCFG_PARAM_ACT_POLICER_PBS            (1 << 6)
/*! 
 \brief Bit-field that specifies policy route fields set/unset in classifier->action structure
*/
#define CLSCFG_PARAM_ACT_POLICYRT               (1 << 17) 
/*! 
 \brief Bit-field that specifies name field set/unset in classifier->action->policy route structure
*/
#define CLSCFG_PARAM_ACT_POLICYRT_NAME          (1 << 0)
/*! 
 \brief Bit-field that specifies gateway IP address field set/unset in classifier->action->policy route structure
*/
#define CLSCFG_PARAM_ACT_POLICYRT_GW_IP         (1 << 1)
/*! 
 \brief Bit-field that specifies destination interface field set/unset in classifier->action->policy route structure
*/
#define CLSCFG_PARAM_ACT_POLICYRT_DST_IF        (1 << 2)
/*! 
 \brief Bit-field that specifies metric field set/unset in classifier->action->policy route structure
*/
#define CLSCFG_PARAM_ACT_POLICYRT_METRIC        (1 << 3)
/*! 
 \brief Bit-field that specifies layer7 protocol fields set/unset in classifier->filter structure
*/
#define CLSCFG_PARAM_L7_PROTO                        (1 << 18)
/*!
 \brief Bit-field that specifies ppp protocol fields set/unset in classifier->filter structure
*/
#define CLSCFG_PARAM_PPP_PROTO                        (1 << 19)

/* MAC Address String Length */
#define MAC_STR_LEN		18

typedef uint32_t ip_addr_t;

/* @} */

/** \addtogroup FAPI_QOS_CLASS */
/* @{ */
/*!
    \brief Bit-map indication of fields selected or excluded of structure
	within which this structure is placed
*/
typedef struct{
	uint32_t param_in; /*!< Bit-map to indicate various parameters included*/
	uint32_t param_ex; /*!< Bit-map to indicate various parameters excluded*/
} cl_param_in_ex_t;

/*!
    \brief VLAN action data structure for a classifier
*/
typedef struct {
	uint32_t	pcp; /*!< New VLAN PCP routing/forwarding action on the packet */
	uint32_t	vlan_id; /*!< Outer VLAN Id */
	uint32_t	inner_pcp; /*!< Inner VLAN PCP value */
	uint32_t	inner_vlan_id; /*!< Inner VLAN Id */
	uint32_t	flags; /*!< Which VLAN actions are specified */
}cl_action_vlan_t;
/*!
    \brief Policer Action data structure
*/
typedef struct {
	uint32_t	null; /*!< No Policer action */
	uint32_t	drop; /*!< Drop packet */
	uint32_t	dscp; /*!< Mark DSCP value */
	uint32_t	pcp; /*!< Mark Ethernet Priority/PCP  */
	uint32_t	flags; /*!< Which Policer Actions are configured */
}policer_action_t;

/*!
    \brief Policer Meter Type
*/
typedef enum cl_meter_type {
	CL_METER_TOKEN_BUCKET = 1,/*!< Simple Token Bucket */
	CL_METER_SR_TCM = 2, /*!< Single Rate Three Color Marker */
	CL_METER_TR_TCM = 3 /*!< Two Rate Three Color Marker */
}cl_meter_type_t;
/*!
    \brief Classifer action Policer data structure 
*/
typedef struct {
	uint32_t	enable; /*!< Whether Policer is enabled */
	char		name[MAX_QOS_NAME_LEN];  /*!< Name or Alias of Policer */
	cl_meter_type_t	type; /*!< Type of Meter - TokenBucket, srTCM, trTCM */
	uint32_t	cr; /*!< Committed Rate */
	uint32_t	cbs; /*!< Committed Burst Size */
	uint32_t	ebs; /*!< Excess Burst Size */
	uint32_t	pr; /*!< Peak Rate */
	uint32_t	pbs; /*!< Peak Burst Size */

	policer_action_t confg; /*!< Conforming Policer Action */
	policer_action_t non_confg; /*!< Non-Conforming Policer Action */
	policer_action_t partial_confg; /*!< Partial Conforming Policer Action */
}cl_action_policer_t;
/*!
    \brief Classifer action Policy Routing data structure
*/
typedef struct {
	uint32_t	enable; /*!< Whether Policer is enabled */
	char		name[MAX_QOS_NAME_LEN];  /*!< Name or Alias of Policer */
	char 		gw_ip[IP_ADDRSTRLEN]; /*!< Gateway IP address */
	char		dst_if[MAX_IF_NAME_LEN]; /*!< Destination Egress I/f */
	int32_t		metric; /*!< Routing Metric */
}cl_action_policy_rt_t;
/*!
    \brief Classifer action Meter data structure
*/
typedef struct {
	uint32_t	id; /*!< Meter ID */
	uint32_t	action; /*!< Meter Action */
}cl_action_meter_t;
/*!
    \brief Classifier Action Data structure
*/
typedef struct {
	uint32_t	flags; /*!< Following actions can be specified, and if the action is modified 
			 CL_ACTION_TC
			 CL_ACTION_DROP
			 CL_ACTION_DSCP
			 CL_ACTION_VLAN
			 CL_ACTION_POLICER
			 CL_ACTION_POLICYRT
			 CL_ACTION_METER
			 CL_ACTION_MODIFIED : Modified after being originally added
			 CL_ACTION_MODIFIED_OTHER : Modified by an entity other than the owner */
	char		q_name[MAX_IF_NAME_LEN]; /*!< Queue name the classifier is mapped to. */
	uint32_t	tc; /*!< Mark Traffic Class action */
	uint32_t	tc_ing; /*!< Mark INGRESS Traffic Class action */
	uint32_t        priority; /*priority of queue */
	uint32_t	drop; /*!< Drop packet action for firewall */
	uint32_t	dscp; /*!< New DSCP value to remark */
	uint32_t        accl; /*!< For Acceleration action support */
	cl_action_vlan_t vlan; /*!< VLAN PCP value and/or VLAN Id remark */
	cl_action_policer_t policer; /*!< Policer action */
	cl_action_policy_rt_t policy_rt; /*!< Policy routing/forwarding action on the packet */
	cl_action_meter_t meter;
	uint32_t 	iflags;
}cl_action_t;

/* @} */

/** \addtogroup FAPI_QOS_CLASS */
/* @{ */
/*!
    \brief Types of String Match operation
*/
typedef enum {
	STR_EXACT_MATCH=1, /*!< Exact match required */
	STR_PREFIX_MATCH=2, /*!< Prefix match required */
	STR_SUFFIX_MATCH=3, /*!< Suffix match required */
	STR_SUBSTRING_MATCH=4 /*!< Substring match required */
} str_match_mode_t;
/*!
    \brief DHCP match fields 
	\note The DHCP match fields are matched against DHCP Request and response messages from/to a Host, and then applied
    to all traffic from/to that IP address (Src or Dst)
*/
typedef struct {
 	char		vendor_classid[MAX_VENDOR_ID_LEN]; /*!< Vendor Id string to match
					against */
	str_match_mode_t vendor_classid_mode; /*!<  Vendor Id match mode - Exact,
					     Prefix, Suffix, Substring */
	char		vendor_classid_v6[MAX_VENDOR_ID_V6_LEN];/*!<  Vendor Id for DHCPv6 string to match
					against */
	uint32_t	vendor_exclude; /*!< Exclude Flag or Invert match
					      for Vendor Class Id */
 	char		clientid[MAX_CLIENTID_LEN]; /*!< Client Id string to match
					against */
	uint32_t	clientid_exclude; /*!< Exclude Flag or Invert match
					      for Client Id */
 	char		user_classid[MAX_USER_CLASSID_LEN]; /*!< User Class Id string to match
					against */
	uint32_t	user_classid_exclude; /*!< Exclude Flag or Invert match
					      for User Class Id */
 	char		vendor_specific_info[MAX_VENDOR_INFO_LEN]; /*!< Vendor Specific Info string to match
					against */
	uint32_t	vendor_specific_info_exclude; /*!< Exclude Flag or Invert match
					      for Vendor Specific Info */
	cl_param_in_ex_t  param_in_ex; /*!< Bit-map to indicate included and excluded parameters of DHCP */
}cl_dhcp_match_t;
/*!
    \brief IP Address Match fields structure
*/
typedef struct {
	//struct in_addr ipaddr; /*!< IPv4 or IPv6 address */
	//struct in_addr mask; /*!< Mask of IPv4 or IPv6 addr */
	char ipaddr[IP_ADDRSTRLEN]; /*!< IPv4 or IPv6 address */
	char mask[IP_ADDRSTRLEN]; /*!< Mask of IPv4 or IPv6 addr */
	uint32_t	exclude; /*!< Exclude Flag or Invert match */
}cl_ipaddr_match_t;
/*!
    \brief TCP/UDP Port Match fields structure
*/
typedef struct {
	int32_t		start_port; /*!< Start Port of Range. -1 means NULL */
	int32_t		end_port; /*!< End Port of Range. -1 means no range
				    specified */
	uint32_t	exclude; /*!< Exclude Flag or Invert match */
	cl_param_in_ex_t  param_in_ex; /*!< Bit-map to indicate included and excluded parameters of ports */
}cl_port_match_t;
/*!
    \brief 5-tuple IP Match fields structure
*/
typedef struct {
	cl_ipaddr_match_t 	src_ip;/*!< Source IP address (IPv4 or IPv6) match of packet */
	cl_ipaddr_match_t 	dst_ip;/*!< Destination IP address (IPv4 or IPv6) match of packet */
	int32_t			ip_p; /*!<IP Protocol of packet. -1 means not specified */
	int32_t			ip_p_exclude; /*!<Exclude flag for IP Protocol */
	cl_port_match_t 	src_port; /*!< Destination TCP/UDP port match */
	cl_port_match_t 	dst_port;/*!< Source TCP/UDP port match */
	cl_param_in_ex_t  param_in_ex; /*!< Bit-map to indicate included and excluded parameters of 5-tuple IP */
}cl_ip_5tuple_match_t;
/*!
    \brief MAC Address Match fields structure
*/
typedef struct {
	char		mac_addr[MAX_MACADDR_LEN]; /*!< MAC address to match. Null
						     string means ignore */
	char		mac_addr_mask[MAX_MACADDR_LEN]; /*!< MAC address mask. A mask of FF:FF:FF:FF:FF means mask is ignored. */
	uint32_t	exclude; /*!< Exclude Flag or Invert match */
	cl_param_in_ex_t  param_in_ex;  /*!< Bit-map to indicate included and excluded parameters of MAC address */
}cl_macaddr_match_t;

/*!
    \brief Ethernet Header Match fields structure - does not include MAC
    address fields
*/
typedef struct {
	int32_t		eth_type; /*!< Ethernet Protocol Type. A value of -1
				    means no match required */
	uint32_t	eth_type_exclude; /*!< Whether to exclude the specified
					    Ethernet Type */
	int16_t		dsap; /*!< 802.2 DSAP if 802.2 hdr exists. A value of -1
			       means not needed to match */
	uint16_t	dsap_exclude; /*!< Whether to exclude/invert DSAP match
					*/
	int16_t		ssap; /*!< 802.2 SSAP if 802.2 hdr exists */
	uint16_t	ssap_exclude; /*!< Whether to exclude/invert DSAP match
					*/
	int32_t		llc_control; /*!< LLC Control field */
	uint32_t	llc_control_exclude; /*!< Exclude Flag or Invert match */
	int32_t		oui; /*!< SNAP OUI  field */
	uint32_t	oui_exclude; /*!< Exclude Flag or Invert match */
	int16_t		prio; /*!< 802.1D Annex H or 802.1Q Priority. -1 means
				no match required */
	uint16_t	prio_exclude; /*!< Exclude Flag or Invert match for
					prio */
	int16_t		vlanid; /*!< VLAN Id value, -1 means no match required
				    */
	uint16_t	vlanid_exclude; /*!< Exclude Flag or Invert match for
					    VLAN tag field */
	int8_t		vlan_dei; /*!< Match against DEI field of VLAN tag. -1
				    means ignore for match purpose */
	int8_t		vlan_dei_exclude; /*!< Exclude Flag or Invert match for
					    vlan_dei */
	int16_t		inner_prio; /*!< 802.1Q Priority of inner VLAN header. -1 means
				no match required */
	uint16_t	inner_prio_exclude; /*!< Exclude Flag or Invert match for
					prio */
	int16_t		inner_vlanid; /*!< VLAN Id value in inner VLAN header, -1 means no match required
				    */
	uint16_t	inner_vlanid_exclude; /*!< Exclude Flag or Invert match for
					    inner VLAN tag field */
	int8_t		inner_vlan_dei; /*!< Match against DEI field of VLAN tag. -1
				    means ignore for match purpose */
	int8_t		inner_vlan_dei_exclude; /*!< Exclude Flag or Invert match for
					    vlan_dei */
	cl_param_in_ex_t  param_in_ex; /*!< Bit-map to indicate included and excluded parameters of Ethernet header */
}cl_eth_match_t;

/*!
    \brief Other IP header Classifer Match fields structure
*/
typedef struct {
	int32_t		ip_len_min; /*!< Min Length of IP packet. -1 means ignore for
				  match purpose */
	int32_t		ip_len_max; /*!< Max Length of IP packet. -1 means
				      ignore for match purpose */
	int32_t		ip_len_exclude; /*!< Exclude flag/ Invert Length match
					 */
	int16_t		dscp;  /*!< DSCP value of packet to match against. -1
				 means ignore for match purpose */
	int16_t		dscp_exclude; /*!< Exclude flag/Invert DSCP match */

	cl_param_in_ex_t  param_in_ex;  /*!< Bit-map to indicate included and excluded parameters of IP header */
} cl_ip_match_t;

/*!
    \brief Other TCP header Classifer Match fields structure
*/
typedef struct {
	int8_t			tcp_ack; /*!<Match TCP ACK : 1- match TCP ACK */
	int8_t			tcp_ack_exclude; /*!<Packet does not contain TCP
						    ACK flag*/
	int8_t			tcp_syn; /*!<Match TCP SYN : 1- match TCP SYN */
	int8_t			tcp_syn_exclude; /*!<Packet does not contain TCP
						    SYN flag*/

	int8_t			tcp_fin; /*!<Match TCP FIN : 1- match TCP FIN */
	int8_t			tcp_fin_exclude; /*!<Packet does not contain TCP
						    FIN flag*/
	int8_t			tcp_rst; /*!<Match TCP FIN : 1- match TCP FIN */
	int8_t			tcp_rst_exclude; /*!<Packet does not contain TCP
						    RST flag */

	int8_t			tcp_urg; /*!<Match TCP URG : 1- match TCP URG
					    i.e. OOB data */
	int8_t			tcp_urg_exclude; /*!<Packet does not contain TCP
						    URG flag */

	cl_param_in_ex_t  param_in_ex;  /*!< Bit-map to indicate included and excluded parameters of TCP flags */
} cl_tcp_match_t;

/*!
    \brief Application Match fields structure
*/
typedef struct {
	int32_t		app_id; /*!< Application Protocol Id */
	int32_t		app_id_exclude; /*!< Exclude flag/ Invert App match
					 */
	int16_t		subproto_id;  /*!< Application Sub-Protocol Id */
	int16_t		subproto_id_exclude; /*!< Exclude flag/Invert Sub
					       Protocol match */
	cl_param_in_ex_t  param_in_ex;  /*!< Bit-map to indicate included and excluded parameters of Application */
} cl_app_match_t;

/*!
    \brief Classifer Match fields structure
*/
typedef struct {
	uint32_t		enable; /*!< Whether Filter is enabled */
	uint32_t		status; /*!< Operational status of the filter. Only
					  valid on GET */
	char			name[MAX_CL_NAME_LEN]; /*!< Name or Alias of the
							  classifier */
   	uint32_t		order; /*!< Ordered location of the classifier */
   	uint32_t		owner; /*!<Enumerated value of who is doing the operation
						Possible owners can be: TR-069, Web GUI, CLI, System, BaseRules */
	uint32_t		flags; /*!<Following flags can be kept as per
					 filter structure:
			 CL_FILTER_MODIFIED : Modified after being originally added
			 CL_FILTER_MODIFIED_OTHER : Modified by an entity other than the owner */
	uint32_t		mptcp_flag; /*!< 1-when classifier is for MPTCP, 0-otherwise */
	cl_dhcp_match_t		src_dhcp; /*!< DHCP Classifier based filter fields for Pkt Src */
	cl_dhcp_match_t		dst_dhcp; /*!< DHCP Classifier based filter fields for Pkt Dst*/
	char			rx_if[MAX_IF_NAME_LEN]; /*!< Rx Interface name. Null
							   string means any interface */
	int32_t			rx_if_exclude; /*!< Exclude /invert match of specificed
						 rx_if field */
	char			tx_if[MAX_IF_NAME_LEN]; /*!< Tx Interface name. Null
							   string means any interface */
	int32_t			tx_if_exclude; /*!< Exclude /invert match of specificed
						 Tx_if field */
	cl_macaddr_match_t	dst_mac; /*!< Match against Dst MAC address  of
					  pkt */
	cl_macaddr_match_t	src_mac; /*!< Match against Src MAC address  of
					   pkt */
	cl_eth_match_t		ethhdr; /*!< Match against Ethernet header
					  fields */
	cl_ip_5tuple_match_t	outer_ip; /*!< IP 5-tuple match for Outer IP
					    header */
	cl_ip_5tuple_match_t	inner_ip; /*!< IP 5-tuple match for Inner IP
					    header */
	cl_ip_match_t		iphdr; /*!< Match against other IP header fields
					*/
	uint16_t 			ppp_proto; /*!< Match against PPP Protocol */
	cl_tcp_match_t		tcphdr; /*!< Match against other TCP header fields */
	cl_app_match_t		app; /*!< Application based classifier */
	char			        l7_proto[MAX_L7_PROTO_NAME_LEN]; /*!< Layer7 protocol name, like ftp,sip etc */
	cl_param_in_ex_t  param_in_ex;  /*!< Bit-map to indicate included and excluded parameters of main filter */
}cl_match_filter_t;

/*!
    \brief Classifier structure with action and filter combined
*/
typedef struct {
	cl_match_filter_t	filter; /*!< Classifier filter fields structure */
	cl_action_t 		action; /*!< Classifier action fields structure */
	uint32_t intermediate_filter; /*!<Used as an aditional criteria in conjunction with class cfg filter crieria in case of partial classification */
	uint32_t intermediate_result; /*!<Used to pass intermediate information to next engine as an output of partial classification */
}qos_class_cfg_t;

/* @} */



/** \addtogroup FAPI_QOS_CLASS */
/* @{ */
/*! \brief  Set QoS AL Classifier - involves adding a new classifier, 
 * modifying an existing classifier, or deleting an existing classifier
   \param[in] order  Order number of the classifier in the list - this allows append and insert operations
   \param[in] filter Match classifier filter definition for the filter
   \param[in] action Match classifier action for the filter
   \param[in] flags  Specifies the action to be performed by the Filter as follows:
   	 CL_FILTER_ADD
	 CL_FILTER_MODIFY
	 CL_FILTER_DELETE
	 CL_FILTER_DELETE_EXACT
   \return 0 if OK / -1 on ERROR
   \note  While deleting a classifier, if order=-1, then exact match of filter
   will be done. If order >= 0, then exact match will only be done if
   CL_FILTER_DELETE_EXACT is passed
*/
int32_t
fapi_qos_classifier_set(
	int32_t			order,
	cl_match_filter_t	filter,
	cl_action_t 		action,
	uint32_t 		flags
	  );

/*! \brief  Get QoS AL Classifier - Get one or more Classifiers at the position
 * given by order
   \param[in] order  Order number of the classifier in the list - this allows append and insert operations
   \param[out] num_entries Number of classifiers to return at Order number. The value is
   updated with the number of classifiers returned by the API
   \param[out] filter Array of filters based on num_entries. Caller has to free
   the memory
   \param[out] action Array of actions based on num_entries. Caller has to free
   the memory
   \param[in] flags Reserved
   \return 0 if OK / -1 on ERROR
   \note The #filters and #actions are same, as each filter has a corresponing
   action
*/
int32_t
fapi_qos_classifier_get(
	uint32_t		order,
	int32_t			*num_entries,
	cl_match_filter_t	**filter,
	cl_action_t 		**action,
	uint32_t		flags
	  );

/*! \brief  Set QoS AL Interface Application Abstraction like SIP, RTSP etc
   \param[in] app_name Name of application
   \param[in] app_filter Application filter fields structure
   \param[in] flags Flags are defined as follows:
   	 CL_APP_ABS_ADD : Add a match filter abstraction to Application
	Abstraction
	 CL_APP_ABS_DEL : Delete a match filter abstraction from Application
	Abstraction
   \return app_id if OK / -1 on ERROR
   \note  If no app_name does not exist, a new app_id will be allocated. This
   can be done even with null app_filter
*/
int32_t
fapi_qos_app_abs_set(
	char			app_name[MAX_CL_AP_NAME],
	cl_match_filter_t	*app_filter,
	uint32_t 		flags
	);

/*! \brief  Get QoS AL Interface Application Abstraction filters for applications like SIP, RTSP, etc
   \param[in] app_id  Application Id for which to retrieve filter mappings
   \param[in] app_name  Application Name for which to retrieve filter mappings
   \param[out] num_entries  Number of entries returned in allocated array
   \param[out] app_filters  Array of filter entries defining the Application
   currently. Caller must free the buffer returned
   \param[in] flags Flags are reserved currently
   \return 0 if OK / -1 on ERROR
   \note One of app_id or app_name must be specified
*/
int32_t
fapi_qos_app_abs_get(
	int32_t			app_id,
	char			*app_name,
	int32_t			*num_entries,
	cl_match_filter_t	*app_filters,
	uint32_t 		flags
	);

/*! \brief  Set QoS AL base interface for an interface, such as, whether pppoe 
  interface is configured above ptm0 or eth1 etc
   \param[in] ifname  Name of interface name to add/delete
   \param[in] base_ifname  Name of base interface name for interface
   \param[in] logical_base Name of logical interface name upon which interface is created (might be same as base_ifname)
   \param[in] flags Flags are defined as follows: 
   	 QOS_OP_F_ADD : Add base interface for an interface
	 QOS_OP_F_DELETE : Delete base interface for an interface
   \return 0 if OK / -1 on ERROR
*/

int32_t
fapi_qos_if_base_set(
	char		*ifname,
	char		*base_ifname,
	char		*logical_base,
	uint32_t 	flags
	);

/*! \brief  Set QoS AL Interface Group Abstraction like LAN, WAN, WLAN - involves adding a new
  interface to an interface group or deleting 
   \param[in] ifgroup  Name of interface Group name
   \param[in] ifname  Name of interface name to add
   each pointer, and then the array of pointers
   \param[in] flags Flags are defined as follows: 
   	 QOS_OP_F_ADD : Add an interface to AL Interface Group Abstraction
	 QOS_OP_F_DELETE : Delete an interface from an AL Interface Group
   \return 0 if OK / -1 on ERROR
*/

int32_t
fapi_qos_if_abs_set(
	iftype_t	ifgroup,
	char		*ifname,
	uint32_t 	flags
	);

/*! \brief  Get QoS AL Interface Group Abstraction member interfaces, such as for LAN, WAN, WLAN 
   \param[in] ifgroup  Name of interface Group name
   \param[out] num_entries  Number of entries returned in allocated array
   \param[out] ifname  Array of pointers to interface names. Caller must free
   each pointer, and then the array of pointers
   \param[in] flags Flags are reserved currently
   \return 0 if OK / -1 on ERROR
*/
int32_t
fapi_qos_if_abs_get(
	iftype_t	ifgroup,
	int32_t		*num_entries,
	char		ifnames[MAX_IF_PER_GROUP][32],
	uint32_t 	flags
	);

/*! \brief  Set QoS AL Classifier DHCP Abstraction map - add or delete
   \param[in] ipaddr  IPv4 or IPv6 address of system
   \param[out] dhcp  DHCP parameter match to associate or dis-associate with ipaddr
   \param[in] flags Flags are defined as follows: 
   	 CL_F_DHCP_ADD : Add a DHCP Association  to ipaddr
	 CL_F_DHCP_DEL : Delete a DHCP Association from ipaddr
   \return 0 if OK / -1 on ERROR 
*/
int32_t
fapi_qos_dhcp_abs_set(
	ip_addr_t		ipaddr,
	cl_dhcp_match_t		*dhcp,
	uint32_t 		flags
	);

/*! \brief  Get all DHCP Associations for a given IP address
   \param[in] ifgroup  Name of interface Group name
   \param[out] num_entries  Number of entries returned in allocated array of DHCP maps for ipaddr
   \param[out] dhcp_arr  Allocated array of DHCP match entries for ifaddr. Caller needs to free buffer
   \param[in] flags Flags are defined below
   \return 0 if OK / -1 on ERROR
*/
int32_t
fapi_qos_dhcp_abs_get(
	iftype_t	ifgroup,
	int32_t		*num_entries,
	char		*dhcp_arr,
	uint32_t 	flags
	);


/*! \brief  Set ingress qos group for a given interface
   \param[in] ifinggrp  Name of ingress qos group
   \param[in] ifname  Name of the interface
   \param[in] flags Flags are defined below
   \return 0 if OK / -1 on ERROR
*/
int32_t
fapi_qos_if_inggrp_set(
        IN ifinggrp_t ifinggrp,
        IN char *ifname,
        IN uint32_t flags);


/*! \brief  Get ingress qos group interfaces
   \param[in] ifinggrp  Name of interface Group
   \param[out] num_entries  Number of entries returned in allocated array of interface names
   \param[out] ifnames  Allocated array of interface names which belong to this ingress qos group
   \param[in] flags Flags are defined below
   \return 0 if OK / -1 on ERROR
*/
int32_t
fapi_qos_if_inggrp_get(
        IN ifinggrp_t ifinggrp,
        OUT int32_t *num_entries,
        OUT char ifnames[MAX_IF_PER_INGRESS_GROUP][32],
        IN uint32_t flags);
/* @} */
#endif

/* Query Capabilities @top-level - for eg. from Web/TR-181 */
/* owner_id - BASE_QOS, WEB, ACS */
