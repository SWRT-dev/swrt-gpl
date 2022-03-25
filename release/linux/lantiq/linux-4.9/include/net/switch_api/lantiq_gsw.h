/******************************************************************************
                Copyright (c) 2016, 2017 Intel Corporation

******************************************************************************/
/*****************************************************************************
                Copyright (c) 2012, 2014, 2015
                    Lantiq Deutschland GmbH
    For licensing information, see the file 'LICENSE' in the root folder of
    this software module.
******************************************************************************/



#ifndef _LANTIQ_GSW_H_
#define _LANTIQ_GSW_H_

/* =================================== */
/* Global typedef forward declarations */
/* =================================== */

/* ============================= */
/* Local Macros & Definitions    */
/* ============================= */
#include "gsw_types.h"
#include "gsw_ioctlcmd_type.h"
#include "mac_ops.h"

/** \defgroup GSW_GROUP GSWIP Functional APIs
    \brief This chapter describes the entire interface for accessing and configuring the different services of the Ethernet Switch module. The prefix GSW (Gigabit Switch) is used for all data structures and APIs pertaining to GSWIP.
*/

/*@{*/

/** \defgroup GSW_IOCTL_BRIDGE Ethernet Bridging Functions
    \brief Ethernet bridging (or switching) is the basic task of the device. It provides individual configurations per port and standard global switch features.
*/

/** \defgroup GSW_IOCTL_VLAN VLAN Functions
    \brief This section describes VLAN bridging functionality. This includes support for Customer VLAN Tags (C-VLAN) and also Service VLAN Tags (S-VLAN).
     The Egress VLAN Treatment is new addition to GSWIP-3.0.
*/

/** \defgroup GSW_IOCTL_MULTICAST Multicast Functions
    \brief Group of functional APIs for IGMP/MLD snooping configuration and support for IGMPv1/v2/v3 and MLDv1/v2.
*/

/** \defgroup GSW_IOCTL_OAM Operation, Administration, and Management Functions
    \brief Group of functions that are provided to perform OAM functions on Switch.
*/

/** \defgroup GSW_IOCTL_QOS Quality of Service Functions
    \brief Group of functional APIs for Quality of Service (QoS) components.
*/

/** \defgroup GSW_IOCTL_PMAC Pseudo-MAC Functions
    \brief Group of functional APIs for Pseudo MAC (PMAC). The PMAC config is applicable to GSWIP-3.0 only.
*/

/** \defgroup GSW_IOCTL_RMON RMON Counters Functions
    \brief Group of functional APIs for Remote-Monitoring (RMON) counters.
*/

/** \defgroup GSW_IOCTL_GSWIP31 GSWIP-3.1 Specific Functions
    \brief Group of functional APIs for GSWIP-3.1 to cover GPON/EPON/G.INT.
*/

/*@}*/

/* -------------------------------------------------------------------------- */
/*                 Structure and Enumeration Type Defintions                  */
/* -------------------------------------------------------------------------- */

/** \addtogroup GSW_IOCTL_BRIDGE */
/*@{*/
/** \brief MAC Table Entry to be read.
    Used by \ref GSW_MAC_TABLE_ENTRY_READ. */
typedef struct {
	/** Restart the get operation from the beginning of the table. Otherwise
	    return the next table entry (next to the entry that was returned
	    during the previous get operation). This boolean parameter is set by the
	    calling application. */
	ltq_bool_t bInitial;
	/** Indicates that the read operation got all last valid entries of the
	    table. This boolean parameter is set by the switch API
	    when the Switch API is called after the last valid one was returned already. */
	ltq_bool_t bLast;
	/** Get the MAC table entry belonging to the given Filtering Identifier
	    (not supported by all switches). */
	u32 nFId;
	/** Ethernet Port number (zero-based counting) in GSWIP-2.1/2.2/3.0. From
	    GSWIP-3.1, this field is Bridge Port ID. The valid range is hardware
	    dependent.

	    \remarks
	    In GSWIP-2.1/2.2/3.0, this field is used as portmap field, when the MSB
	    bit is set. In portmap mode, every value bit represents an Ethernet port.
	    LSB represents Port 0 with incrementing counting.
	    The (MSB - 1) bit represent the last port.
	    The macro \ref GSW_PORTMAP_FLAG_SET allows to set the MSB bit,
	    marking it as portmap variable.
	    Checking the portmap flag can be done by
	    using the \ref GSW_PORTMAP_FLAG_GET macro.
	    From GSWIP3.1, if MSB is set, other bits in this field are ignored.
	    array \ref GSW_MAC_tableRead_t::nPortMap is used for bit map. */
	u32 nPortId;
	/** Bridge Port Map - to support GSWIP-3.1, following field is added
	    for port map in static entry. It's valid only when MSB of
	    \ref GSW_MAC_tableRead_t::nPortId is set. Each bit stands for 1 bridge
	    port. */
	u16 nPortMap[16];
	/** Aging Time, given in multiples of 1 second in a range from 1 s to 1,000,000 s.
	    The value read back in a GET command might differ slightly from the value
	    given in the SET command due to limited hardware timing resolution.
	    Filled out by the switch API implementation. */
	int nAgeTimer;
	/** STAG VLAN Id. Only applicable in case SVLAN support is enabled on the device. */
	u16 nSVLAN_Id;
	/** Static Entry (value will be aged out after 'nAgeTimer' if the entry
	    is not set to static). */
	ltq_bool_t bStaticEntry;
	/** Sub-Interface Identifier Destination (supported in GSWIP-3.0/3.1 only). */
	u16 nSubIfId;
	/** MAC Address. Filled out by the switch API implementation. */
	u8 nMAC[GSW_MAC_ADDR_LEN];
	/** Source/Destination MAC address filtering flag (GSWIP-3.1 only)
	    Value 0 - not filter, 1 - source address filter,
	    2 - destination address filter, 3 - both source and destination filter.

	    \remarks
	    Please refer to "GSWIP Hardware Architecture Spec" chapter 3.4.4.6
	    "Source MAC Address Filtering and Destination MAC Address Filtering"
	    for more detail. */
	u8 nFilterFlag;
	/** Packet is marked as IGMP controlled if destination MAC address matches
	    MAC in this entry. (GSWIP-3.1 only) */
	ltq_bool_t bIgmpControlled;

	/** Changed
	0: the entry is not changed
	1: the entry is changed and not accessed yet */

	ltq_bool_t bEntryChanged;

	/** Associated Mac address -(GSWIP-3.2)*/
	u8 nAssociatedMAC[GSW_MAC_ADDR_LEN];
	/* MAC Table Hit Status Update (Supported in GSWip-3.1/3.2) */
	ltq_bool_t hitstatus;
} GSW_MAC_tableRead_t;

/** \brief Search for a MAC address entry in the address table.
    Used by \ref GSW_MAC_TABLE_ENTRY_QUERY. */
typedef struct {
	/** MAC Address. This parameter needs to be provided for the search operation.
	    This is an input parameter. */
	u8 nMAC[GSW_MAC_ADDR_LEN];
	/** Get the MAC table entry belonging to the given Filtering Identifier
	    (not supported by all switches).
	    This is an input parameter. */
	u32 nFId;
	/** MAC Address Found. Switch API sets this boolean variable in case
	    the requested MAC address 'nMAC' is found inside the address table,
	    otherwise it is set to FALSE.
	    This is an output parameter. */
	ltq_bool_t bFound;
	/** Ethernet Port number (zero-based counting) in GSWIP-2.1/2.2/3.0. From
	    GSWIP-3.1, this field is Bridge port ID. The valid range is hardware
	    dependent.

	    \remarks
	    In GSWIP-2.1/2.2/3.0, this field is used as portmap field, when the MSB
	    bit is set. In portmap mode, every value bit represents an Ethernet port.
	    LSB represents Port 0 with incrementing counting.
	    The (MSB - 1) bit represent the last port.
	    The macro \ref GSW_PORTMAP_FLAG_SET allows to set the MSB bit,
	    marking it as portmap variable.
	    Checking the portmap flag can be done by
	    using the \ref GSW_PORTMAP_FLAG_GET macro.
	    From GSWIP3.1, if MSB is set, other bits in this field are ignored.
	    array \ref GSW_MAC_tableRead_t::nPortMap is used for bit map. */
	u32 nPortId;
	/** Bridge Port Map - to support GSWIP-3.1, following field is added
	    for port map in static entry. It's valid only when MSB of
	    \ref GSW_MAC_tableRead_t::nPortId is set. Each bit stands for 1 bridge
	    port. */
	u16 nPortMap[16];
	/** Sub-Interface Identifier Destination (supported in GSWIP-3.0/3.1 only). */
	u16 nSubIfId;
	/** Aging Time, given in multiples of 1 second in a range from 1 s to 1,000,000 s.
	    The value read back in a GET command might differ slightly from the value
	    given in the SET command due to limited hardware timing resolution.
	    Filled out by the switch API implementation.
	    This is an output parameter. */
	int nAgeTimer;
	/** STAG VLAN Id. Only applicable in case SVLAN support is enabled on the device. */
	u16 nSVLAN_Id;
	/** Static Entry (value will be aged out after 'nAgeTimer' if the entry
	    is not set to static).
	    This is an output parameter. */
	ltq_bool_t bStaticEntry;
	/** Source/Destination MAC address filtering flag (GSWIP-3.1 only)
	    Value 0 - not filter, 1 - source address filter,
	    2 - destination address filter, 3 - both source and destination filter.

	    \remarks
	    Please refer to "GSWIP Hardware Architecture Spec" chapter 3.4.4.6
	    "Source MAC Address Filtering and Destination MAC Address Filtering"
	    for more detail. */
	u8 nFilterFlag;
	/** Packet is marked as IGMP controlled if destination MAC address matches
	    MAC in this entry. (GSWIP-3.1 only) */
	ltq_bool_t bIgmpControlled;
	/** Changed
	0: the entry is not changed
	1: the entry is changed and not accessed yet */
	ltq_bool_t bEntryChanged;
	/** Associated Mac address -(GSWIP-3.2)*/
	u8 nAssociatedMAC[GSW_MAC_ADDR_LEN];

	/* MAC Table Hit Status Update (Supported in GSWip-3.1/3.2) */
	ltq_bool_t hitstatus;
} GSW_MAC_tableQuery_t;

/** \brief MAC Table Entry to be added.
    Used by \ref GSW_MAC_TABLE_ENTRY_ADD. */
typedef struct {
	/** Filtering Identifier (FID) (not supported by all switches) */
	u32 nFId;
	/** Ethernet Port number (zero-based counting) in GSWIP-2.1/2.2/3.0. From
	    GSWIP-3.1, this field is Bridge Port ID. The valid range is hardware
	    dependent.

	    \remarks
	    In GSWIP-2.1/2.2/3.0, this field is used as portmap field, when the MSB
	    bit is set. In portmap mode, every value bit represents an Ethernet port.
	    LSB represents Port 0 with incrementing counting.
	    The (MSB - 1) bit represent the last port.
	    The macro \ref GSW_PORTMAP_FLAG_SET allows to set the MSB bit,
	    marking it as portmap variable.
	    Checking the portmap flag can be done by
	    using the \ref GSW_PORTMAP_FLAG_GET macro.
	    From GSWIP3.1, if MSB is set, other bits in this field are ignored.
	    array \ref GSW_MAC_tableRead_t::nPortMap is used for bit map. */
	u32 nPortId;
	/** Bridge Port Map - to support GSWIP-3.1, following field is added
	    for port map in static entry. It's valid only when MSB of
	    \ref GSW_MAC_tableRead_t::nPortId is set. Each bit stands for 1 bridge
	    port. */
	u16 nPortMap[16];
	/** Sub-Interface Identifier Destination (supported in GSWIP-3.0/3.1 only).

	    \remarks
	    In GSWIP-3.1, this field is sub interface ID for WLAN logical port. For
	    Other types, either outer VLAN ID if Nto1Vlan enabled or 0. */
	u16 nSubIfId;
	/** Aging Time, given in multiples of 1 second in a range
	    from 1 s to 1,000,000 s.
	    The configured value might be rounded that it fits to the given hardware platform. */
	int nAgeTimer;
	/** STAG VLAN Id. Only applicable in case SVLAN support is enabled on the device. */
	u16 nSVLAN_Id;
	/** Static Entry (value will be aged out if the entry is not set to static). The
	    switch API implementation uses the maximum age timer in case the entry
	    is not static. */
	ltq_bool_t bStaticEntry;
	/** Egress queue traffic class.
	    The queue index starts counting from zero.   */
	u8 nTrafficClass;
	/** MAC Address to add to the table. */
	u8 nMAC[GSW_MAC_ADDR_LEN];
	/** Source/Destination MAC address filtering flag (GSWIP-3.1 only)
	    Value 0 - not filter, 1 - source address filter,
	    2 - destination address filter, 3 - both source and destination filter.

	    \remarks
	    Please refer to "GSWIP Hardware Architecture Spec" chapter 3.4.4.6
	    "Source MAC Address Filtering and Destination MAC Address Filtering"
	    for more detail. */
	u8 nFilterFlag;
	/** Packet is marked as IGMP controlled if destination MAC address matches
	    MAC in this entry. (GSWIP-3.1 only) */
	ltq_bool_t bIgmpControlled;

	/** Associated Mac address -(GSWIP-3.2)*/
	u8 nAssociatedMAC[GSW_MAC_ADDR_LEN];
} GSW_MAC_tableAdd_t;

/** \brief MAC Table Entry to be removed.
    Used by \ref GSW_MAC_TABLE_ENTRY_REMOVE. */
typedef struct {
	/** Filtering Identifier (FID) (not supported by all switches) */
	u32 nFId;
	/** MAC Address to be removed from the table. */
	u8 nMAC[GSW_MAC_ADDR_LEN];
	/** Source/Destination MAC address filtering flag (GSWIP-3.1 only)
	    Value 0 - not filter, 1 - source address filter,
	    2 - destination address filter, 3 - both source and destination filter.

	    \remarks
	    Please refer to "GSWIP Hardware Architecture Spec" chapter 3.4.4.6
	    "Source MAC Address Filtering and Destination MAC Address Filtering"
	    for more detail. */
	u8 nFilterFlag;
} GSW_MAC_tableRemove_t;

/** \brief Packet forwarding.
    Used by \ref GSW_STP_BPDU_Rule_t and \ref GSW_multicastSnoopCfg_t
    and \ref GSW_8021X_EAPOL_Rule_t. */
typedef enum {
	/** Default; portmap is determined by the forwarding classification. */
	GSW_PORT_FORWARD_DEFAULT = 0,
	/** Discard; discard packets. */
	GSW_PORT_FORWARD_DISCARD = 1,
	/** Forward to the CPU port. This requires that the CPU port is previously
	    set by calling \ref GSW_CPU_PORT_CFG_SET. */
	GSW_PORT_FORWARD_CPU = 2,
	/** Forward to a port, selected by the parameter 'nForwardPortId'.
	    Please note that this feature is not supported by all
	    hardware platforms. */
	GSW_PORT_FORWARD_PORT = 3
} GSW_portForward_t;

/** \brief Spanning Tree Protocol port states.
    Used by \ref GSW_STP_portCfg_t. */
typedef enum {
	/** Forwarding state. The port is allowed to transmit and receive
	    all packets. Address Learning is allowed. */
	GSW_STP_PORT_STATE_FORWARD = 0,
	/** Disabled/Discarding state. The port entity will not transmit
	    and receive any packets. Learning is disabled in this state. */
	GSW_STP_PORT_STATE_DISABLE = 1,
	/** Learning state. The port entity will only transmit and receive
	    Spanning Tree Protocol packets (BPDU). All other packets are discarded.
	    MAC table address learning is enabled for all good frames. */
	GSW_STP_PORT_STATE_LEARNING = 2,
	/** Blocking/Listening. Only the Spanning Tree Protocol packets will
	    be received and transmitted. All other packets are discarded by
	    the port entity. MAC table address learning is disabled in this
	    state. */
	GSW_STP_PORT_STATE_BLOCKING = 3
} GSW_STP_PortState_t;

/** \brief Configures the Spanning Tree Protocol state of an Ethernet port.
    Used by \ref GSW_STP_PORT_CFG_SET
    and \ref GSW_STP_PORT_CFG_GET. */
typedef struct {
	/** Ethernet Port number (zero-based counting) in GSWIP-2.1/2.2/3.0. From
	    GSWIP-3.1, this field is Bridge Port ID. The valid range is hardware
	    dependent. An error code is delivered if the selected port is not
	    available. */
	u32 nPortId;
	/** Filtering Identifier (FID) (not supported by all switches).
	    The FID allows to keep multiple STP states per physical Ethernet port.
	    Multiple CTAG VLAN groups could be a assigned to one FID and therefore
	    share the same STP port state. Switch API ignores the FID value
	    in case the switch device does not support it or switch CTAG VLAN
	    awareness is disabled. */
	u32 nFId;
	/** Spanning Tree Protocol state of the port. */
	GSW_STP_PortState_t ePortState;
} GSW_STP_portCfg_t;

/** \brief Spanning tree packet detection and forwarding.
    Used by \ref GSW_STP_BPDU_RULE_SET
    and \ref GSW_STP_BPDU_RULE_GET. */
typedef struct {
	/** Filter spanning tree packets and forward them, discard them or
	    disable the filter. */
	GSW_portForward_t eForwardPort;
	/** Target (bridge) port for forwarded packets; only used if selected by
	    'eForwardPort'. Forwarding is done
	    if 'eForwardPort = GSW_PORT_FORWARD_PORT'. */
	u8 nForwardPortId;
} GSW_STP_BPDU_Rule_t;

/** \brief Describes the 802.1x port state.
    Used by \ref GSW_8021X_portCfg_t. */
typedef enum {
	/** Receive and transmit direction are authorized. The port is allowed to
	    transmit and receive all packets and the address learning process is
	    also allowed. */
	GSW_8021X_PORT_STATE_AUTHORIZED = 0,
	/** Receive and transmit direction are unauthorized. All the packets
	    except EAPOL are not allowed to transmit and receive. The address learning
	    process is disabled. */
	GSW_8021X_PORT_STATE_UNAUTHORIZED = 1,
	/** Receive direction is authorized, transmit direction is unauthorized.
	    The port is allowed to receive all packets. Packet transmission to this
	    port is not allowed. The address learning process is also allowed. */
	GSW_8021X_PORT_STATE_RX_AUTHORIZED = 2,
	/** Transmit direction is authorized, receive direction is unauthorized.
	    The port is allowed to transmit all packets. Packet reception on this
	    port is not allowed. The address learning process is disabled. */
	GSW_8021X_PORT_STATE_TX_AUTHORIZED = 3
} GSW_8021X_portState_t;

/** \brief EAPOL frames filtering rule parameter.
    Used by \ref GSW_8021X_EAPOL_RULE_GET
    and \ref GSW_8021X_EAPOL_RULE_SET. */
typedef struct {
	/** Filter authentication packets and forward them, discard them or
	    disable the filter. */
	GSW_portForward_t eForwardPort;
	/** Target (bridge) port for forwarded packets, only used if selected by
	    'eForwardPort'. Forwarding is done
	    if 'eForwardPort = GSW_PORT_FORWARD_PORT'. */
	u8 nForwardPortId;
} GSW_8021X_EAPOL_Rule_t;

/** \brief 802.1x port authentication status.
    Used by \ref GSW_8021X_PORT_CFG_GET
    and \ref GSW_8021X_PORT_CFG_SET. */
typedef struct {
	/** Ethernet Port number (zero-based counting) in GSWIP-2.1/2.2/3.0. From
	    GSWIP-3.1, this field is Bridge Port ID. The valid range is hardware
	    dependent. An error code is delivered if the selected port is not
	    available. */
	u32 nPortId;
	/** 802.1x state of the port. */
	GSW_8021X_portState_t eState;
} GSW_8021X_portCfg_t;

/** \brief Global Ethernet trunking configuration.
    Used by \ref GSW_TRUNKING_CFG_GET
    and \ref GSW_TRUNKING_CFG_SET. */
typedef struct {
	/** IP source address is used by the
	    hash algorithm to calculate the egress trunking port index. */
	ltq_bool_t bIP_Src;
	/** IP destination address is used by the
	    hash algorithm to calculate the egress trunking port index. */
	ltq_bool_t bIP_Dst;
	/** MAC source address is used by the
	    hash algorithm to calculate the egress trunking port index. */
	ltq_bool_t bMAC_Src;
	/** MAC destination address is used by the
	    hash algorithm to calculate the egress trunking port index. */
	ltq_bool_t bMAC_Dst;
} GSW_trunkingCfg_t;

/** \brief Ethernet port trunking configuration.
    Used by \ref GSW_TRUNKING_PORT_CFG_GET
    and \ref GSW_TRUNKING_PORT_CFG_SET. */
typedef struct {
	/** Ports are aggregated.
	    Enabling means that the 'nPortId' and
	    the 'nAggrPortId' ports form an aggregated link. */
	ltq_bool_t bAggregateEnable;
	/** Ethernet Port number (zero-based counting).
	    The valid range is hardware dependent.
	    An error code is delivered if the selected port is not
	    available. */
	u32 nPortId;
	/** Second Aggregated Ethernet Port number (zero-based counting).
	    The valid range is hardware dependent.
	    An error code is delivered if the selected port is not
	    available. */
	u32 nAggrPortId;
} GSW_trunkingPortCfg_t;

/*@}*/ /* GSW_IOCTL_BRIDGE */

/** \addtogroup GSW_IOCTL_VLAN */
/*@{*/

/** \brief VLAN port configuration for ingress packet filtering. Tagged packet and
    untagged packet can be configured to be accepted or dropped (filtered out).
    Used by \ref GSW_VLAN_portCfg_t. */
typedef enum {
	/** Admit all. Tagged and untagged packets are allowed. */
	GSW_VLAN_ADMIT_ALL = 0,
	/** Untagged packets only (not supported yet). Tagged packets are dropped. */
	GSW_VLAN_ADMIT_UNTAGGED = 1,
	/** Tagged packets only. Untagged packets are dropped. */
	GSW_VLAN_ADMIT_TAGGED = 2
} GSW_VLAN_Admit_t;

/** \brief Add a CTAG VLAN ID group to the CTAG VLAN hardware table of the switch.
    Used by \ref GSW_VLAN_ID_CREATE. Not applicable to GSWIP-3.1. */
typedef struct {
	/** CTAG VLAN ID. The valid range is from 0 to 4095.
	    An error code is delivered in case of range mismatch. */
	u16 nVId;
	/** Filtering Identifier (FID) (not supported by all switches). */
	u32 nFId;
} GSW_VLAN_IdCreate_t;

/** \brief Read out the CTAG VLAN ID to FID assignment. The user provides the CTAG VLAN ID
    parameter and the switch API returns the FID parameter.
    Used by \ref GSW_VLAN_ID_GET. Not applicable to GSWIP-3.1. */
typedef struct {
	/** CTAG VLAN ID. The valid range is from 0 to 4095.
	    An error code is delivered in case of range mismatch. */
	u16 nVId;
	/** Filtering Identifier (FID) (not supported by all switches). */
	u32 nFId;
} GSW_VLAN_IdGet_t;

/** \brief Default VLAN membership portmap and egress tagmap for unconfigured VLAN groups.
    Every bit in the portmap variables represents one port (port 0 = LSB bit).
    Used by \ref GSW_VLAN_MEMBER_INIT. Not applicable to GSWIP-3.1. */
typedef struct {
	/** Portmap field of the uninitialized VLAN groups. */
	u32 nPortMemberMap;
	/** Egress tagmap field of the uninitialized VLAN groups. */
	u32 nEgressTagMap;
} GSW_VLAN_memberInit_t;

/** \brief Remove a CTAG VLAN ID from the switch CTAG VLAN table.
    Used by \ref GSW_VLAN_ID_DELETE. Not applicable to GSWIP-3.1. */
typedef struct {
	/** CTAG VLAN ID. The valid range is from 0 to 4095.
	    An error code is delivered in case of range mismatch. */
	u16 nVId;
} GSW_VLAN_IdDelete_t;

/** \brief Adds a CTAG VLAN to a port and set its egress filter information.
    Used by \ref GSW_VLAN_PORT_MEMBER_ADD. Not applicable to GSWIP-3.1. */
typedef struct {
	/** CTAG VLAN ID. The valid range is from 0 to 4095.
	    An error code is delivered in case of range mismatch. */
	u16 nVId;
	/** Ethernet Port number (zero-based counting). The valid range is hardware
	    dependent. An error code is delivered if the selected port is not
	    available.

	    \remarks
	    This field is used as portmap field, when the MSB bit is set.
	    In portmap mode, every value bit represents an Ethernet port.
	    LSB represents Port 0 with incrementing counting.
	    The (MSB - 1) bit represent the last port.
	    The macro \ref GSW_PORTMAP_FLAG_SET allows to set the MSB bit,
	    marking it as portmap variable.
	    Checking the portmap flag can be done by
	    using the \ref GSW_PORTMAP_FLAG_GET macro. */
	u32 nPortId;
	/** Tag Member Egress. Enable egress tag-based support.
	    If enabled, all port egress traffic
	    from this CTAG VLAN group carries a CTAG VLAN group tag. */
	ltq_bool_t bVLAN_TagEgress;
} GSW_VLAN_portMemberAdd_t;

/** \brief Remove the CTAG VLAN configuration from an Ethernet port.
    Used by \ref GSW_VLAN_PORT_MEMBER_REMOVE. Not applicable to GSWIP-3.1. */
typedef struct {
	/** CTAG VLAN ID. The valid range is from 0 to 4095.
	    An error code is delivered in case of range mismatch.
	    If the selected VLAN ID is not found in the VLAN table,
	    an error code is delivered. */
	u16 nVId;
	/** Ethernet Port number (zero-based counting). The valid range is hardware
	    dependent. An error code is delivered if the selected port is not
	    available.

	    \remarks
	    This field is used as portmap field, when the MSB bit is set.
	    In portmap mode, every value bit represents an Ethernet port.
	    LSB represents Port 0 with incrementing counting.
	    The (MSB - 1) bit represent the last port.
	    The macro \ref GSW_PORTMAP_FLAG_SET allows to set the MSB bit,
	    marking it as portmap variable.
	    Checking the portmap flag can be done by
	    using the \ref GSW_PORTMAP_FLAG_GET macro. */
	u32 nPortId;
} GSW_VLAN_portMemberRemove_t;

/** \brief Read the CTAG VLAN port membership table.
    Used by \ref GSW_VLAN_PORT_MEMBER_READ. Not applicable to GSWIP-3.1. */
typedef struct {
	/** Restart the get operation from the start of the table. Otherwise
	    return the next table entry (next to the entry that was returned
	    during the previous get operation). This parameter is always reset
	    during the read operation. This boolean parameter is set by the
	    calling application. */
	ltq_bool_t bInitial;
	/** Indicates that the read operation got all last valid entries of the
	    table. This boolean parameter is set by the switch API
	    when the Switch API is called after the last valid one was returned already. */
	ltq_bool_t bLast;
	/** CTAG VLAN ID. The valid range is from 0 to 4095.
	    An error code is delivered in case of range mismatch. */
	u16 nVId;
	/** Ethernet Port number (zero-based counting). Every bit represents
	    an Ethernet port.

	    \remarks
	    This field is used as portmap field, when the MSB bit is set.
	    In portmap mode, every value bit represents an Ethernet port.
	    LSB represents Port 0 with incrementing counting.
	    The (MSB - 1) bit represent the last port.
	    The macro \ref GSW_PORTMAP_FLAG_SET allows to set the MSB bit,
	    marking it as portmap variable.
	    Checking the portmap flag can be done by
	    using the \ref GSW_PORTMAP_FLAG_GET macro. */
	u32 nPortId;
	/** Enable egress tag-Portmap. Every bit represents an Ethernet port.
	    This field is used as portmap field, and the MSB bit is
	    statically always set. LSB represents Port 0 with
	    incrementing counting.
	    The (MSB - 1) bit represent the last port.
	    All port egress traffic from this CTAG VLAN group carries a
	    tag, in case the port bit is set.

	    \remarks
	    Checking the portmap flag can be done by
	    using the \ref GSW_PORTMAP_FLAG_GET macro. */
	u32 nTagId;
} GSW_VLAN_portMemberRead_t;

/** \brief Port configuration for VLAN member violation.
    Used by \ref GSW_VLAN_portCfg_t. */
typedef enum {
	/** No VLAN member violation. Ingress and egress packets violating the
	    membership pass and are not filtered out. */
	GSW_VLAN_MEMBER_VIOLATION_NO = 0,
	/** VLAN member violation for ingress packets. Ingress packets violating
	    the membership are filtered out. Egress packets violating the
	    membership are not filtered out. */
	GSW_VLAN_MEMBER_VIOLATION_INGRESS = 1,
	/** VLAN member violation for egress packets. Egress packets violating
	    the membership are filtered out. Ingress packets violating the
	    membership are not filtered out.*/
	GSW_VLAN_MEMBER_VIOLATION_EGRESS = 2,
	/** VLAN member violation for ingress and egress packets.
	    Ingress and egress packets violating the membership are filtered out. */
	GSW_VLAN_MEMBER_VIOLATION_BOTH = 3
} GSW_VLAN_MemberViolation_t;

/** \brief CTAG VLAN Port Configuration.
    Used by \ref GSW_VLAN_PORT_CFG_GET
    and \ref GSW_VLAN_PORT_CFG_SET. Not applicable to GSWIP-3.1. */
typedef struct {
	/** Ethernet Port number (zero-based counting). The valid range is hardware
	    dependent. An error code is delivered if the selected port is not
	    available. */
	u16 nPortId;
	/** Port CTAG VLAN ID (PVID). The software shall ensure that the used VID has
	    been configured in advance on the hardware by
	    using \ref GSW_VLAN_ID_CREATE. */
	u16 nPortVId;
	/** Drop ingress CTAG VLAN-tagged packets if the VLAN ID
	    is not listed in the active VLAN set. If disabled, all incoming
	    VLAN-tagged packets are forwarded using the FID tag members and
	    the port members of the PVID.
	    This parameter is only supported for devices which do not
	    support 4k VLAN table entries (64 entries instead). */
	ltq_bool_t bVLAN_UnknownDrop;
	/** Reassign all ingress CTAG VLAN tagged packets to the port-based
	    VLAN ID (PVID). */
	ltq_bool_t bVLAN_ReAssign;
	/** VLAN ingress and egress membership violation mode. Allows admittance of
	    VLAN-tagged packets where the port is not a member of the VLAN ID
	    carried in the received and sent packet. */
	GSW_VLAN_MemberViolation_t eVLAN_MemberViolation;
	/** Ingress VLAN-tagged or untagged packet filter configuration. */
	GSW_VLAN_Admit_t eAdmitMode;
	/** Transparent CTAG VLAN Mode (TVM). All packets are handled as untagged
	    packets. Any existing tag is ignored and treated as packet payload. */
	ltq_bool_t bTVM;
} GSW_VLAN_portCfg_t;

/** \brief This CTAG VLAN configuration supports replacing of the VID of received packets
    with the PVID of the receiving port.
    Used by \ref GSW_VLAN_RESERVED_ADD
    and \ref GSW_VLAN_RESERVED_REMOVE. Not applicable to GSWIP-3.1. */
typedef struct {
	/** VID of the received packet to be replaced by the PVID.
	    The valid range is from 0 to 4095.
	    An error code is delivered in case of range mismatch. */
	u16 nVId;
} GSW_VLAN_reserved_t;

/** \brief STAG VLAN global configuration.
    Used by \ref GSW_SVLAN_CFG_GET
    and \ref GSW_SVLAN_CFG_SET. Not applicable to GSWIP-3.1. */
typedef struct {
	/** Protocol EtherType Field. This 16-bit of the STAG VLAN (default=0x88A8). */
	u16 nEthertype;
} GSW_SVLAN_cfg_t;

/** \brief STAG VLAN Port Configuration.
    Used by \ref GSW_SVLAN_PORT_CFG_GET
    and \ref GSW_SVLAN_PORT_CFG_SET. Not applicable to GSWIP-3.1. */
typedef struct {
	/** Ethernet Port number (zero-based counting). The valid range is hardware
	    dependent. An error code is delivered if the selected port is not
	    available. */
	u16 nPortId;
	/** Port based STAG VLAN Support. All STAG VLAN protocol parsing and
	    configuration features are only applied on this port in case the
	    STAG VLAN port support is enabled. */
	ltq_bool_t bSVLAN_TagSupport;
	/** Port Egress MAC based STAG VLAN. All egress packets contain a
	    STAG VLAN ID that is based on the VLAN ID which is retrieved
	    from the MAC bridging table. This MAC bridging table SVLAN ID
	    can be learned from the snooped traffic or statically added. */
	ltq_bool_t bSVLAN_MACbasedTag;
	/** Port STAG VLAN ID (PVID) */
	u16 nPortVId;
	/** Reassign all ingress STAG VLAN tagged packets to the port-based
	    STAG VLAN ID (PVID). */
	ltq_bool_t bVLAN_ReAssign;
	/** VLAN ingress and egress membership violation mode. Allows admittance of
	    STAG VLAN-tagged packets where the port is not a member of the STAG VLAN ID
	    carried in the received and sent packet. */
	GSW_VLAN_MemberViolation_t eVLAN_MemberViolation;
	/** Ingress STAG VLAN-tagged or untagged packet filter configuration. */
	GSW_VLAN_Admit_t eAdmitMode;
} GSW_SVLAN_portCfg_t;

/** \brief Egress VLAN Treatment Selector based upon FID or destination SubIf-Id MSB bits (Bit position 12 to 8) - for GSWIP-3.0 only.
    Used by \ref GSW_PCE_EgVLAN_Cfg_t. */
typedef enum {
	/** FID based Egress VLAN Treatment. */
	GSW_PCE_EG_VLAN_FID_BASED = 0,
	/** Sub-InterfaceId Group (Bits 12 to 8) based Egress VLAN Treatment. */
	GSW_PCE_EG_VLAN_SUBIFID_BASED = 1
} GSW_PCE_EgVLAN_mode_t;

/** \brief Egress VLAN Treatment Configuration - for GSWIP-3.0 only.
    Used by \ref GSW_PCE_EG_VLAN_CFG_GET and \ref GSW_PCE_EG_VLAN_CFG_SET. */
typedef struct {
	/** Egress Port Identifier */
	u16  nPortId;
	/** To enable or disable Egress VLAN treatment on given port */
	ltq_bool_t bEgVidEna;
	/** Egress VLAN Treatment Selector - FID or SubIf-Id-MSB (Bits 12 to 8) based */
	GSW_PCE_EgVLAN_mode_t eEgVLANmode;
	/** Egress VLAN Treatment start index for specified nPortId. Continuous block of VLAN is used until next port's Start block assignment */
	u8  nEgStartVLANIdx;
} GSW_PCE_EgVLAN_Cfg_t;


/** \brief Egress VLAN Treatment Entry Configuration - for GSWIP-3.0 only.
    Used by \ref GSW_PCE_EG_VLAN_ENTRY_READ and \ref GSW_PCE_EG_VLAN_ENTRY_WRITE. */
typedef struct {
	/** Egress Port Identifier */
	u16  nPortId;
	/** Index corresponding to FID or SubIf-ID Subfield (Bits 12 to 8) */
	u16  nIndex;
	/** Egress VLAN Treatment Action Enable */
	ltq_bool_t	bEgVLAN_Action;
	/** STAG VLAN Removal action */
	ltq_bool_t	bEgSVidRem_Action;
	/** STAG VLAN Insert action */
	ltq_bool_t	bEgSVidIns_Action;
	/** SVLAN Value to be inserted in egress */
	u16 nEgSVid;
	/** CTAG VLAN Removal action*/
	ltq_bool_t	bEgCVidRem_Action;
	/** CTAG VLAN Insert action */
	ltq_bool_t	bEgCVidIns_Action;
	/** CVLAN Value to be inserted in egress */
	u16 nEgCVid;
} GSW_PCE_EgVLAN_Entry_t;

/*@}*/ /* GSW_IOCTL_VLAN */

/** \addtogroup GSW_IOCTL_QOS */
/*@{*/

/** \brief Define setting the priority queue to an undefined value.
    This disables the priority feature. */
#define GSW_TRAFFIC_CLASS_DISABLE 0xFF

/** \brief DSCP mapping table.
    Used by \ref GSW_QOS_DSCP_CLASS_SET
    and \ref GSW_QOS_DSCP_CLASS_GET. */
typedef struct {
	/** Traffic class associated with a particular DSCP value.
	    DSCP is the index to an array of resulting traffic class values.
	    The index starts counting from zero. */
	u8	nTrafficClass[64];
} GSW_QoS_DSCP_ClassCfg_t;

/** \brief Traffic class associated with a particular 802.1P (PCP) priority mapping value.
    This table is global for the entire switch device. Priority map entry structure.
    Used by \ref GSW_QOS_PCP_CLASS_SET
    and \ref GSW_QOS_PCP_CLASS_GET. */
typedef struct {
	/** Configures the PCP to traffic class mapping.
	    The queue index starts counting from zero. */
	u8	nTrafficClass[8];
} GSW_QoS_PCP_ClassCfg_t;

/** \brief Ingress DSCP remarking attribute. This attribute defines on the
    ingress port packets how these will be remarked on the egress port.
    A packet is only remarked in case its ingress and its egress port
    have remarking enabled.
    Used by \ref GSW_QoS_portRemarkingCfg_t. */
typedef enum {
	/** No DSCP Remarking. No remarking is done on the egress port. */
	GSW_DSCP_REMARK_DISABLE = 0,
	/** TC DSCP 6-Bit Remarking. The complete DSCP remarking is done based
	    on the traffic class. The traffic class to DSCP value mapping is
	    given in a device global table. */
	GSW_DSCP_REMARK_TC6 = 1,
	/** TC DSCP 3-Bit Remarking. The upper 3-Bits of the DSCP field are
	    remarked based on the traffic class. The traffic class to DSCP value
	    mapping is given in a device global table. */
	GSW_DSCP_REMARK_TC3 = 2,
	/** Drop Precedence Remarking. The Drop Precedence is remarked on the
	    egress side. */
	GSW_DSCP_REMARK_DP3 = 3,
	/** TC Drop Precedence Remarking. The Drop Precedence is remarked on the
	    egress side and the upper 3-Bits of the DSCP field are
	    remarked based on the traffic class. The traffic class to DSCP value
	    mapping is given in a device global table. */
	GSW_DSCP_REMARK_DP3_TC3 = 4
} GSW_Qos_ingressRemarking_t;

/** \brief Port Remarking Configuration. Ingress and Egress remarking options for
    dedicated packet fields DSCP, CTAG VLAN PCP, STAG VLAN PCP
    and STAG VLAN DEI.
    Remarking is done either on the used traffic class or the
    drop precedence.
    Packet field specific remarking only applies on a packet if
    enabled on ingress and egress port.
    Used by \ref GSW_QOS_PORT_REMARKING_CFG_SET
    and \ref GSW_QOS_PORT_REMARKING_CFG_GET. */
typedef struct {
	/** Ethernet Port number (zero-based counting). The valid range is hardware
	    dependent. An error code is delivered if the selected port is not
	    available. */
	u16 nPortId;
	/** Ingress DSCP Remarking. Specifies on ingress side how a packet should
	    be remarked. This DSCP remarking only works in case remarking is
	    enabled on the egress port.
	    This configuration requires that remarking is also enabled on the
	    egress port. DSCP remarking enable on either ingress or egress port
	    side does not perform any remark operation. */
	GSW_Qos_ingressRemarking_t	eDSCP_IngressRemarkingEnable;
	/** Egress DSCP Remarking. Applies remarking on egress packets in a
	    fashion as specified on the ingress port. This ingress port remarking
	    is configured by the parameter 'eDSCP_IngressRemarking'.
	    This configuration requires that remarking is also enabled on the
	    ingress port. DSCP remarking enable on either ingress or egress port
	    side does not perform any remark operation. */
	ltq_bool_t bDSCP_EgressRemarkingEnable;
	/** Ingress PCP Remarking. Applies remarking to all port ingress packets.
	    This configuration requires that remarking is also enabled on the
	    egress port. PCP remarking enable on either ingress or egress port
	    side does not perform any remark operation. */
	ltq_bool_t bPCP_IngressRemarkingEnable;
	/** Egress PCP Remarking. Applies remarking for all port egress packets.
	    This configuration requires that remarking is also enabled on the
	    ingress port. PCP remarking enable on either ingress or egress port
	    side does not perform any remark operation. */
	ltq_bool_t bPCP_EgressRemarkingEnable;
	/** Ingress STAG VLAN PCP Remarking */
	ltq_bool_t bSTAG_PCP_IngressRemarkingEnable;
	/** Ingress STAG VLAN DEI Remarking */
	ltq_bool_t bSTAG_DEI_IngressRemarkingEnable;
	/** Egress STAG VLAN PCP & DEI Remarking */
	ltq_bool_t bSTAG_PCP_DEI_EgressRemarkingEnable;
} GSW_QoS_portRemarkingCfg_t;

/** \brief Traffic class to DSCP mapping table.
    Used by \ref GSW_QOS_CLASS_DSCP_SET
    and \ref GSW_QOS_CLASS_DSCP_GET. */
typedef struct {
	/** DSCP value (6-bit) associated with a particular Traffic class.
	    Traffic class is the index to an array of resulting DSCP values.
	    The index starts counting from zero. */
	u8 nDSCP[16];
} GSW_QoS_ClassDSCP_Cfg_t;

/** \brief Traffic class associated with a particular 802.1P (PCP) priority mapping value.
    This table is global for the entire switch device. Priority map entry structure.
    Used by \ref GSW_QOS_CLASS_PCP_SET
    and \ref GSW_QOS_CLASS_PCP_GET. */
typedef struct {
	/** Configures the traffic class to PCP (3-bit) mapping.
	    The queue index starts counting from zero. */
	u8 nPCP[16];
} GSW_QoS_ClassPCP_Cfg_t;

/** \brief DSCP Drop Precedence to color code assignment.
    Used by \ref GSW_QoS_DSCP_DropPrecedenceCfg_t. */
typedef enum {
	/** Critical Packet. Metering never changes the drop precedence of these packets. */
	GSW_DROP_PRECEDENCE_CRITICAL           = 0,
	/** Green Drop Precedence Packet. Packet is marked with a 'low' drop precedence. */
	GSW_DROP_PRECEDENCE_GREEN = 1,
	/** Yellow Drop Precedence Packet. Packet is marked with a 'middle' drop precedence. */
	GSW_DROP_PRECEDENCE_YELLOW	= 2,
	/** Red Drop Precedence Packet. Packet is marked with a 'high' drop precedence. */
	GSW_DROP_PRECEDENCE_RED = 3
} GSW_QoS_DropPrecedence_t;

/** \brief DSCP to Drop Precedence assignment table configuration.
    Used by \ref GSW_QOS_DSCP_DROP_PRECEDENCE_CFG_SET
    and \ref GSW_QOS_DSCP_DROP_PRECEDENCE_CFG_GET. */
typedef struct {
	/** DSCP to drop precedence assignment. Every array entry represents the
	    drop precedence for one of the 64 existing DSCP values.
	    DSCP is the index to an array of resulting drop precedence values.
	    The index starts counting from zero. */
	GSW_QoS_DropPrecedence_t nDSCP_DropPrecedence[64];
} GSW_QoS_DSCP_DropPrecedenceCfg_t;

/** \brief Selection of the traffic class field.
    Used by \ref GSW_QoS_portCfg_t.
    The port default traffic class is assigned in case non of the
    configured protocol code points given by the packet. */
typedef enum {
	/** No traffic class assignment based on DSCP or PCP */
	GSW_QOS_CLASS_SELECT_NO = 0,
	/** Traffic class assignment based on DSCP. PCP information is ignored.
	    The Port Class is used in case DSCP is not available in the packet. */
	GSW_QOS_CLASS_SELECT_DSCP = 1,
	/** Traffic class assignment based on PCP. DSCP information is ignored.
	    The Port Class is used in case PCP is not available in the packet. */
	GSW_QOS_CLASS_SELECT_PCP	= 2,
	/** Traffic class assignment based on DSCP. Make the assignment based on
	    PCP in case the DSCP information is not available in the packet header.
	    The Port Class is used in case both are not available in the packet. */
	GSW_QOS_CLASS_SELECT_DSCP_PCP          = 3,
	/** CTAG VLAN PCP, IP DSCP. Traffic class assignment based
	    on CTAG VLAN PCP, alternative use DSCP based assignment. */
	GSW_QOS_CLASS_SELECT_PCP_DSCP          = 4,
	/** STAG VLAN PCP. Traffic class assignment based
	    on STAG VLAN PCP. */
	GSW_QOS_CLASS_SELECT_SPCP	= 5,
	/** STAG VLAN PCP, IP DSCP. Traffic class assignment based
	    on STAG VLAN PCP, alternative use DSCP based assignment. */
	GSW_QOS_CLASS_SELECT_SPCP_DSCP         = 6,
	/** IP DSCP, STAG VLAN PCP. Traffic class assignment based
	    on DSCP, alternative use STAG VLAN PCP based assignment. */
	GSW_QOS_CLASS_SELECT_DSCP_SPCP         = 7,
	/** STAG VLAN PCP, CTAG VLAN PCP. Traffic class assignment based
	    on STAG VLAN PCP, alternative use CTAG VLAN PCP based assignment. */
	GSW_QOS_CLASS_SELECT_SPCP_PCP          = 8,
	/** STAG VLAN PCP, CTAG VLAN PCP, IP DSCP. Traffic class assignment
	    based on STAG VLAN PCP, alternative use CTAG VLAN PCP based
	    assignment, alternative use DSCP based assignment. */
	GSW_QOS_CLASS_SELECT_SPCP_PCP_DSCP     = 9,
	/** IP DSCP, STAG VLAN PCP, CTAG VLAN PCP. Traffic class assignment
	    based on DSCP, alternative use STAG VLAN PCP based
	    assignment, alternative use CTAG VLAN PCP based assignment. */
	GSW_QOS_CLASS_SELECT_DSCP_SPCP_PCP     = 10
} GSW_QoS_ClassSelect_t;

/** \brief Describes which priority information of ingress packets is used
    (taken into account) to identify the packet priority and the related egress
    priority queue. For DSCP, the priority to queue assignment is done
    using \ref GSW_QOS_DSCP_CLASS_SET. For VLAN, the priority to queue
    assignment is done using \ref GSW_QOS_PCP_CLASS_SET.
    Used by \ref GSW_QOS_PORT_CFG_SET and \ref GSW_QOS_PORT_CFG_GET. */
typedef struct {
	/** Ethernet Port number (zero-based counting). The valid range is hardware
	    dependent. An error code is delivered if the selected port is not
	    available. */
	u16	nPortId;
	/** Select the packet header field on which to base the traffic class assignment. */
	GSW_QoS_ClassSelect_t	eClassMode;
	/** Default port priority in case no other priority
	    (such as VLAN-based PCP or IP-based DSCP) is used. */
	u8	nTrafficClass;
} GSW_QoS_portCfg_t;

/** \brief Configures a rate shaper instance with the rate and the burst size.
    Used by \ref GSW_QOS_SHAPER_CFG_SET and \ref GSW_QOS_SHAPER_CFG_GET. */
typedef struct {
	/** Rate shaper index (zero-based counting). */
	u32	nRateShaperId;
	/** Enable/Disable the rate shaper. */
	ltq_bool_t	bEnable;
	/** 802.1Qav credit based shaper mode. This specific shaper
	    algorithm mode is used by the audio/video bridging (AVB)
	    network (according to 802.1Qav). By default, an token
	    based shaper algorithm is used. */
	ltq_bool_t	bAVB;
	/** Committed Burst Size (CBS [bytes]) */
	u32	nCbs;
	/** Rate [kbit/s] */
	u32	nRate;
} GSW_QoS_ShaperCfg_t;

/** \brief Assign one rate shaper instance to a QoS queue.
    Used by \ref GSW_QOS_SHAPER_QUEUE_ASSIGN and \ref GSW_QOS_SHAPER_QUEUE_DEASSIGN. */
typedef struct {
	/** Rate shaper index (zero-based counting). */
	u8	nRateShaperId;
	/** QoS queue index (zero-based counting). */
	u8	nQueueId;
} GSW_QoS_ShaperQueue_t;

/** \brief Retrieve if a rate shaper instance is assigned to a QoS egress queue.
    Used by \ref GSW_QOS_SHAPER_QUEUE_GET. */
typedef struct {
	/** QoS queue index (zero-based counting).
	    This parameter is the input parameter for the GET function. */
	u8	nQueueId;
	/** Rate shaper instance assigned.
	    If 1, a rate shaper instance is assigned to the queue. Otherwise no shaper instance is assigned. */
	ltq_bool_t	bAssigned;
	/** Rate shaper index (zero-based counting). Only a valid instance is returned in case 'bAssigned == 1'. */
	u8	nRateShaperId;
} GSW_QoS_ShaperQueueGet_t;

/** \brief WRED Cfg Type - Automatic (Adaptive) or Manual.
    Used by \ref GSW_QoS_WRED_Cfg_t. */
typedef enum {
	/** Automatic - Adaptive Watermark Type - GSWIP-3.0/3.1 only*/
	GSW_QOS_WRED_Adaptive	= 0,
	/** Manual Threshold Levels Type */
	GSW_QOS_WRED_Manual	= 1
} GSW_QoS_WRED_Mode_t;

/** \brief WRED Thresholds Mode Type. - GSWIP-3.0/3.1 only
    Used by \ref GSW_QoS_WRED_Cfg_t. */
typedef enum {
	/** Local Thresholds Mode */
	GSW_QOS_WRED_Local_Thresh	= 0,
	/** Global Thresholds Mode */
	GSW_QOS_WRED_Global_Thresh	= 1,
	/** Port queue and Port WRED Thresholds */
	GSW_QOS_WRED_Port_Thresh	= 2,

} GSW_QoS_WRED_ThreshMode_t;

/** \brief Drop Probability Profile. Defines the drop probability profile.
    Used by \ref GSW_QoS_WRED_Cfg_t. */
typedef enum {
	/** Pmin = 25%, Pmax = 75% (default) */
	GSW_QOS_WRED_PROFILE_P0	= 0,
	/** Pmin = 25%, Pmax = 50% */
	GSW_QOS_WRED_PROFILE_P1	= 1,
	/** Pmin = 50%, Pmax = 50% */
	GSW_QOS_WRED_PROFILE_P2	= 2,
	/** Pmin = 50%, Pmax = 75% */
	GSW_QOS_WRED_PROFILE_P3	= 3
} GSW_QoS_WRED_Profile_t;

/** \brief Egress Queue Congestion Notification Watermark.
    Used by \ref GSW_QoS_WRED_Cfg_t. */
typedef enum {
	/**
	>= 1/4 of green max water mark assert
	<= 1/4 of green max water mark de assert*/
	GSW_QOS_WRED_WATERMARK_1_4	= 0,
	/**
	>= 1/8 of green max water mark assert
	<= 1/8 of green max water mark de assert*/
	GSW_QOS_WRED_WATERMARK_1_8	= 1,
	/**
	>= 1/12 of green max water mark assert
	<= 1/12 of green max water mark de assert*/
	GSW_QOS_WRED_WATERMARK_1_12	= 2,
	/**
	>= 1/16 of green max water mark assert
	<= 1/16 of green max water mark de assert*/
	GSW_QOS_WRED_WATERMARK_1_16	= 3
} GSW_QoS_WRED_WATERMARK_t;

/** \brief Configures the global probability profile of the device.
    The min. and max. threshold values are given in number of packet
    buffer segments and required only in case of Manual Mode. The GSWIP-3.0/3.1 supports Auto mode and the threshold values are dynamically computed internally by GSWIP. The size of a segment can be retrieved using \ref GSW_CAP_GET.
    Used by \ref GSW_QOS_WRED_CFG_SET and \ref GSW_QOS_WRED_CFG_GET. */
typedef struct {
	/** Egress Queue Congestion Notification Watermark
	   only applicable for GSWIP 3.1*/
	GSW_QoS_WRED_WATERMARK_t eCongestionWatermark;
	/** Drop Probability Profile. */
	GSW_QoS_WRED_Profile_t	eProfile;
	/** Automatic or Manual Mode of Thresholds Config */
	GSW_QoS_WRED_Mode_t eMode;
	/** WRED Threshold Mode Config */
	GSW_QoS_WRED_ThreshMode_t eThreshMode;
	/** WRED Red Threshold Min [number of segments] - Valid for Manual Mode only. */
	u32	nRed_Min;
	/** WRED Red Threshold Max [number of segments] - Valid for Manual Mode only */
	u32	nRed_Max;
	/** WRED Yellow Threshold Min [number of segments] - Valid for Manual Mode only */
	u32	nYellow_Min;
	/** WRED Yellow Threshold Max [number of segments] - Valid for Manual Mode only */
	u32	nYellow_Max;
	/** WRED Green Threshold Min [number of segments] - Valid for Manual Mode only */
	u32	nGreen_Min;
	/** WRED Green Threshold Max [number of segments] - Valid for Manual Mode only */
	u32	nGreen_Max;
} GSW_QoS_WRED_Cfg_t;

/** \brief Configures the WRED threshold level values.
    The min. and max. values are given in number of packet
    buffer segments. The size of a segment can be
    retrieved using \ref GSW_CAP_GET.
    Used by \ref GSW_QOS_WRED_QUEUE_CFG_SET and \ref GSW_QOS_WRED_QUEUE_CFG_GET. */
typedef struct {
	/** QoS queue index (zero-based counting). */
	u32	nQueueId;
	/** WRED Red Threshold Min [number of segments]. */
	u32	nRed_Min;
	/** WRED Red Threshold Max [number of segments]. */
	u32	nRed_Max;
	/** WRED Yellow Threshold Min [number of segments]. */
	u32	nYellow_Min;
	/** WRED Yellow Threshold Max [number of segments]. */
	u32	nYellow_Max;
	/** WRED Green Threshold Min [number of segments]. */
	u32	nGreen_Min;
	/** WRED Green Threshold Max [number of segments]. */
	u32	nGreen_Max;
} GSW_QoS_WRED_QueueCfg_t;

/** \brief Configures the WRED threshold parameter per port.
    The configured thresholds apply to fill level sum
    of all egress queues which are assigned to the egress port.
    The min. and max. values are given in number of packet
    buffer segments. The size of a segment can be
    retrieved using \ref GSW_CAP_GET.
    Used by \ref GSW_QOS_WRED_PORT_CFG_SET and \ref GSW_QOS_WRED_PORT_CFG_GET. */
typedef struct {
	/** Ethernet Port number (zero-based counting).
	    The valid range is hardware dependent. */
	u32	nPortId;
	/** WRED Red Threshold Min [number of segments]. */
	u32	nRed_Min;
	/** WRED Red Threshold Max [number of segments]. */
	u32	nRed_Max;
	/** WRED Yellow Threshold Min [number of segments]. */
	u32	nYellow_Min;
	/** WRED Yellow Threshold Max [number of segments]. */
	u32	nYellow_Max;
	/** WRED Green Threshold Min [number of segments]. */
	u32	nGreen_Min;
	/** WRED Green Threshold Max [number of segments]. */
	u32	nGreen_Max;
} GSW_QoS_WRED_PortCfg_t;

/** \brief Configures the global buffer flow control threshold for
    conforming and non-conforming packets.
    The min. and max. values are given in number of packet
    buffer segments. The size of a segment can be
    retrieved using \ref GSW_CAP_GET.
    Used by \ref GSW_QOS_FLOWCTRL_CFG_SET and \ref GSW_QOS_FLOWCTRL_CFG_GET. */
typedef struct {
	/** Global Buffer Non Conforming Flow Control Threshold Minimum [number of segments]. */
	u32	nFlowCtrlNonConform_Min;
	/** Global Buffer Non Conforming Flow Control Threshold Maximum [number of segments]. */
	u32	nFlowCtrlNonConform_Max;
	/** Global Buffer Conforming Flow Control Threshold Minimum [number of segments]. */
	u32	nFlowCtrlConform_Min;
	/** Global Buffer Conforming Flow Control Threshold Maximum [number of segments]. */
	u32	nFlowCtrlConform_Max;
} GSW_QoS_FlowCtrlCfg_t;

/** \brief Configures the ingress port flow control threshold for
    used packet segments.
    The min. and max. values are given in number of packet
    buffer segments. The size of a segment can be
    retrieved using \ref GSW_CAP_GET.
    Used by \ref GSW_QOS_FLOWCTRL_PORT_CFG_SET and \ref GSW_QOS_FLOWCTRL_PORT_CFG_GET. */
typedef struct {
	/** Ethernet Port number (zero-based counting).
	    The valid range is hardware dependent. */
	u32	nPortId;
	/** Ingress Port occupied Buffer Flow Control Threshold Minimum [number of segments]. */
	u32	nFlowCtrl_Min;
	/** Ingress Port occupied Buffer Flow Control Threshold Maximum [number of segments]. */
	u32	nFlowCtrl_Max;
} GSW_QoS_FlowCtrlPortCfg_t;

/** \brief Meter Type - srTCM or trTCM. Defines the Metering algorithm Type.
    Used by \ref GSW_QoS_meterCfg_t. */
typedef enum {
	/** srTCM Meter Type */
	GSW_QOS_Meter_srTCM	= 0,
	/** trTCM Meter Type - GSWIP-3.0 only */
	GSW_QOS_Meter_trTCM	= 1,
} GSW_QoS_Meter_Type;

/** \brief Configures the parameters of a rate meter instance.
    Used by \ref GSW_QOS_METER_ALLOC, \ref GSW_QOS_METER_FREE,
    \ref GSW_QOS_METER_CFG_SET and \ref GSW_QOS_METER_CFG_GET. */
typedef struct {
	/** Enable/Disable the meter shaper. */
	ltq_bool_t	bEnable;
	/** Meter index (zero-based counting).

	    \remarks
	    For \ref GSW_QOS_METER_FREE, this is the only input and other fields are
	    ignored. For \ref GSW_QOS_METER_ALLOC, this is output when allocation
	    is successful. For \ref GSW_QOS_METER_CFG_SET and
	    \ref GSW_QOS_METER_CFG_GET, this is input to indicate meter to
	    configure/get-configuration. */
	u32	nMeterId;
	/** Meter Name string for easy reference (Id to Name Mapping) - TBD*/
	char	cMeterName[32];
	/** Meter Algorithm Type */
	GSW_QoS_Meter_Type eMtrType;
	/** Committed Burst Size (CBS [Bytes]). */
	u32	nCbs;
	/** Excess Burst Size (EBS [Bytes]). */
	u32	nEbs;
	/** Committed Information Rate (CIR [kbit/s])*/
	u32	nRate;
	/** Peak Information Rate (PIR [kbit/s]) - applicable for trTCM only */
	u32	nPiRate;
	/** Peak Burst Size (PBS [Bytes]) - applicable for trTCM only */
//   u32	nPbs;
	/** Meter colour mode **/
	u32 nColourBlindMode;
	/** Enable/Disable Packet Mode. 0- Byte, 1 - Pkt */
	ltq_bool_t bPktMode;
	/** Enable/Disable local overhead for metering rate calculation. */
	ltq_bool_t	bLocalOverhd;
	/** Local overhead for metering rate calculation when
	    \ref GSW_QoS_meterCfg_t::bLocalOverhd is TRUE. */
	u32 nLocaloverhd;
} GSW_QoS_meterCfg_t;

/** \brief Specifies the direction for ingress and egress.
    Used by \ref GSW_QoS_meterPort_t and \ref GSW_QoS_meterPortGet_t. */
typedef enum {
	/** No direction. */
	GSW_DIRECTION_NONE	= 0,
	/** Ingress direction. */
	GSW_DIRECTION_INGRESS	= 1,
	/** Egress direction. */
	GSW_DIRECTION_EGRESS	= 2,
	/** Ingress and egress direction. */
	GSW_DIRECTION_BOTH	= 3
} GSW_direction_t;

/** \brief Assign a rate meter instance to an ingress and/or egress port.
    Used by \ref GSW_QOS_METER_PORT_ASSIGN and \ref GSW_QOS_METER_PORT_DEASSIGN.
    Not applicable to GSWIP-3.1. */
typedef struct {
	/** Meter index (zero-based counting). */
	u32	nMeterId;
	/** Port assignment. Could be either ingress, egress or both. Setting it to
	    'GSW_DIRECTION_NONE' would remove the queue for any port
	    assignment. */
	GSW_direction_t eDir;
	/** Ingress Port Id. */
	u32	nPortIngressId;
	/** Egress Port Id. */
	u32	nPortEgressId;
} GSW_QoS_meterPort_t;

/** \brief Reads out all meter instance to port assignments.
    Used by \ref GSW_QOS_METER_PORT_GET. Not applicable to GSWIP-3.1. */
typedef struct {
	/** Restart the get operation from the start of the table. Otherwise
	    return the next table entry (next to the entry that was returned
	    during the previous get operation). This boolean parameter is set by the
	    calling application. */
	ltq_bool_t	bInitial;
	/** Indicates that the read operation got all last valid entries of the
	    table. This boolean parameter is set by the switch API
	    when the Switch API is called after the last valid one was returned already. */
	ltq_bool_t	bLast;
	/** Port assignment. Could be either ingress, egress or both. Setting it to
	    'GSW_DIRECTION_NONE' would remove the queue for any port
	    assignment. */
	GSW_direction_t	eDir;
	/** Meter index (zero-based counting). */
	u8	nMeterId;
	/** Ingress Port Id. */
	u8	nPortIngressId;
	/** Egress Port Id. */
	u8	nPortEgressId;
} GSW_QoS_meterPortGet_t;

/** \brief Assigns one meter instances for storm control.
    Used by \ref GSW_QOS_STORM_CFG_SET and \ref GSW_QOS_STORM_CFG_GET.
    Not applicable to GSWIP-3.1. */
typedef struct {
	/** Meter index 0 (zero-based counting). */
	int	nMeterId;
	/** Meter instances used for broadcast traffic. */
	ltq_bool_t	bBroadcast;
	/** Meter instances used for multicast traffic. */
	ltq_bool_t	bMulticast;
	/** Meter instances used for unknown unicast traffic. */
	ltq_bool_t	bUnknownUnicast;
} GSW_QoS_stormCfg_t;

/** \brief Triggers Metering Action for Traffic by CPU/MPE (available for GSWIP-3.0 only).
    Used by \ref GSW_QOS_METER_ACT */
typedef struct {
	/** CPU User Id  - GSWIP-3.0 supports up to 2 users - CPU-0 & CPU-1 */
	u8	nCpuUserId;
	/** Primary Meter Instance Id (zero-based counting). */
	u8	nMeterId;
	/** Primary Meter Enable or Disable Action */
	ltq_bool_t	bMeterEna;
	/** Secondary Meter Instance Id (zero-based counting). (-1) to indicate this is not used */
	u8	nSecMeterId;
	/** Secondary Meter Enable or Disable Action */
	ltq_bool_t	bSecMeterEna;
	/** Pre-Color to indicate the traffic type getting Metered. */
	GSW_QoS_DropPrecedence_t ePreColor;
	/** Packet Length in Bytes subject to Metering action from CPU */
	u16	pktLen;
	/** Metering Colour Output after it was subject to Metering Algorithm.
	    This is an output parameter carrying combined Meter action of two Meter instances */
	GSW_QoS_DropPrecedence_t eOutColor;
} GSW_QoS_mtrAction_t;

/** \brief Select the type of the Egress Queue Scheduler.
    Used by \ref GSW_QoS_schedulerCfg_t. */
typedef enum {
	/** Strict Priority Scheduler. */
	GSW_QOS_SCHEDULER_STRICT	= 0,
	/** Weighted Fair Queuing Shceduler. */
	GSW_QOS_SCHEDULER_WFQ	= 1
} GSW_QoS_Scheduler_t;

/** \brief Configures the egress queues attached to a single port, and that
    are scheduled to transmit the queued Ethernet packets.
    Used by \ref GSW_QOS_SCHEDULER_CFG_SET and \ref GSW_QOS_SCHEDULER_CFG_GET. */
typedef struct {
	/** QoS queue index (zero-based counting). */
	u8	nQueueId;
	/** Scheduler Type (Strict Priority/Weighted Fair Queuing). */
	GSW_QoS_Scheduler_t	eType;
	/** Weight in Token. Parameter used for WFQ configuration.
	    Sets the weight in token in relation to all remaining
	    queues on this egress port having WFQ configuration.
	    This parameter is only used when 'eType=GSW_QOS_SCHEDULER_WFQ'. */
	u32	nWeight;
} GSW_QoS_schedulerCfg_t;

/** \brief Describes the QoS Queue Mapping Mode. GSWIP-3.1 only.
    Used by \ref GSW_QoS_queuePort_t. */
typedef enum {
	/** This is default mode where the QID is fixed at
	    \ref GSW_QOS_QUEUE_PORT_SET. */
	GSW_QOS_QMAP_SINGLE_MODE = 0,
	/** This is new mode in GSWIP-3.1. The QID given in
	    \ref GSW_QOS_QUEUE_PORT_SET is base, and bit 0~3 of sub-interface ID
	    is offset. The final QID is base + SubIfId[0:3]. */
	GSW_QOS_QMAP_SUBIFID_MODE = 1
} GSW_QoS_qMapMode_t;

/** \brief Sets the Queue ID for one traffic class of one port.
    Used by \ref GSW_QOS_QUEUE_PORT_SET and \ref GSW_QOS_QUEUE_PORT_GET. */
typedef struct {
	/** Ethernet Port number (zero-based counting). The valid range is hardware
	    dependent. An error code is delivered if the selected port is not
	    available.
	    This is an input parameter for \ref GSW_QOS_QUEUE_PORT_GET. */
	u16	nPortId;
	/** Forward CPU (extraction) before external QoS queueing (DownMEP).
	    GSWIP-3.1 only. */
	ltq_bool_t bExtrationEnable;
	/** When \ref GSW_QoS_queuePort_t::bExtrationEnable is FALSE, this field
	    defines Queue Mapping Mode. GSWIP-3.1 only. */
	GSW_QoS_qMapMode_t eQMapMode;
	/** Traffic Class index (zero-based counting).
	    This is an input parameter for \ref GSW_QOS_QUEUE_PORT_GET. */
	u8	nTrafficClassId;
	/** QoS queue index (zero-based counting).
	    This is an output parameter for \ref GSW_QOS_QUEUE_PORT_GET. */
	u8	nQueueId;
	/** Queue Redirection bypass Option.
	    If enabled, all packets destined to 'nQueueId' are redirected from the
	    'nPortId' to 'nRedirectPortId'. This is used for 2nd stage of FULL QoS
	    Path, where the packet has completed QoS process at CBM/CQEM and been
	    injected into GSWIP again. */
	ltq_bool_t	bRedirectionBypass;
	/** Redirected traffic forward port.
	    All egress packets to 'nPortId' are redirected to "nRedirectPortId".
	    If there is no redirection required, it should be same as "nPortId".
	    GSWIP-3.0/3.1 only. */
	u8	nRedirectPortId;

	/**To enable Ingress PCE Bypass.Applicable for GSWIP 3.2*/
	ltq_bool_t	bEnableIngressPceBypass;
	/*Internal purpose only - user not allowed to use it
	  Applicable for GSWIP 3.2 only*/
	ltq_bool_t	bReservedPortMode;
} GSW_QoS_queuePort_t;

/** \brief Reserved egress queue buffer segments.
    Used by \ref GSW_QOS_QUEUE_BUFFER_RESERVE_CFG_SET and \ref GSW_QOS_QUEUE_BUFFER_RESERVE_CFG_GET. */
typedef struct {
	/** QoS queue index (zero-based counting).
	    This is an input parameter for \ref GSW_QOS_QUEUE_BUFFER_RESERVE_CFG_GET. */
	u8	nQueueId;
	/** Reserved Buffer Segment Threshold [number of segments].
	    This is an output parameter for \ref GSW_QOS_QUEUE_BUFFER_RESERVE_CFG_GET. */
	u32	nBufferReserved;
} GSW_QoS_QueueBufferReserveCfg_t;

/** \brief Traffic class associated with a particular 802.1P (PCP) priority mapping value.
    One table is given per egress port. Priority map entry structure.
    The lowest 3 LSB bits (0 ... 2) of 'nPCP_DEI' describe the PCP field.
    Bit 3 describes the 'DEI' field.
    Used by \ref GSW_QOS_SVLAN_CLASS_PCP_PORT_SET and \ref GSW_QOS_SVLAN_CLASS_PCP_PORT_GET.
    Not applicable to GSWIP-3.1. */
typedef struct {
	/** Ethernet Port number (zero-based counting). The valid range is hardware
	    dependent. An error code is delivered if the selected port is not
	    available. */
	u16	nPortId;
	/** Configures the traffic class to PCP (3-bit) mapping.
	    The queue index starts counting from zero. */
//   u8	nPCP_DEI[16];

	/** Configures the traffic class to CPCP (3-bit) mapping.
	    The queue index starts counting from zero. */
	u8	nCPCP[16];

	/** Configures the traffic class to SPCP (3-bit) mapping.
	    The queue index starts counting from zero. */
	u8	nSPCP[16];

	/** DSCP value (6-bit) associated with a particular Traffic class.
	    Traffic class is the index to an array of resulting DSCP values.
	    The index starts counting from zero. */
	u8	nDSCP[16];

} GSW_QoS_SVLAN_ClassPCP_PortCfg_t;

/** \brief Traffic class associated with a particular STAG VLAN 802.1P (PCP) priority and Drop Eligible Indicator (DEI) mapping value.
    This table is global for the entire switch device. Priority map entry structure.
    The table index value is calculated by 'index=PCP + 8*DEI'
    Used by \ref GSW_QOS_SVLAN_PCP_CLASS_SET and \ref GSW_QOS_SVLAN_PCP_CLASS_GET. */
typedef struct {
	/** Configures the PCP and DEI to traffic class mapping.
	    The queue index starts counting from zero. */
	u8	nTrafficClass[16];
	/**  Configures the PCP traffic color.
	     Not applicable to GSWIP-3.1. */
	u8	nTrafficColor[16];
	/** PCP Remark disable control.
	     Not applicable to GSWIP-3.1. */
	u8	nPCP_Remark_Enable[16];
	/** DEI Remark disable control.
	    Not applicable to GSWIP-3.1. */
	u8	nDEI_Remark_Enable[16];

} GSW_QoS_SVLAN_PCP_ClassCfg_t;

/*@}*/ /* GSW_IOCTL_QOS */

/** \addtogroup GSW_IOCTL_MULTICAST */
/*@{*/


/** \brief Configure the IGMP snooping mode.
    Used by \ref GSW_multicastSnoopCfg_t. */
typedef enum {
	/** IGMP management packet snooping and multicast level 3 table learning
	    is disabled. */
	GSW_MULTICAST_SNOOP_MODE_DISABLED = 0,
	/** IGMP management packet snooping is enabled and used for the hardware
	    auto-learning to fill the multicast level 3 table. */
	GSW_MULTICAST_SNOOP_MODE_AUTOLEARNING	= 1,
	/** IGMP management packet snooping is enabled and forwarded to the
	    configured port. No autolearning of the multicast level 3 table. This
	    table has to be maintained by the management software. */
	GSW_MULTICAST_SNOOP_MODE_FORWARD = 2
} GSW_multicastSnoopMode_t;

/** \brief Configure the IGMP report suppression mode.
    Used by \ref GSW_multicastSnoopCfg_t. */
typedef enum {
	/** Report Suppression and Join Aggregation. */
	GSW_MULTICAST_REPORT_JOIN	= 0,
	/** Report Suppression. No Join Aggregation. */
	GSW_MULTICAST_REPORT	= 1,
	/** Transparent Mode. No Report Suppression and no Join Aggregation. */
	GSW_MULTICAST_TRANSPARENT	= 2
} GSW_multicastReportSuppression_t;

/** \brief Configure the switch multicast configuration.
    Used by \ref GSW_MULTICAST_SNOOP_CFG_SET and \ref GSW_MULTICAST_SNOOP_CFG_GET. */
typedef struct {
	/** Enables and configures the IGMP/MLD snooping feature.
	Select autolearning or management packet forwarding mode.
	Packet forwarding is done to the port selected in 'eForwardPort'. */
	GSW_multicastSnoopMode_t	eIGMP_Mode;
	/** IGMPv3 hardware support.
	When enabled the IGMP table includes both the group table and
	the source list table. Otherwise the table only includes the
	group table. This feature is needed when supporting IGMPv3 and
	MLDv2 protocols. */
	ltq_bool_t	bIGMPv3;
	/** Enables snooped IGMP control packets treated as cross-CTAG VLAN packets. This
	parameter is used for hardware auto-learning and snooping packets
	forwarded to a dedicated port. This dedicated port can be selected
	over 'eForwardPort'. */
	ltq_bool_t	bCrossVLAN;
	/** Forward snooped packet, only used if forwarded mode
	is selected
	by 'eIGMP_Mode = GSW_MULTICAST_SNOOP_MODE_SNOOPFORWARD'. */
	GSW_portForward_t	eForwardPort;
	/** Target port for forwarded packets, only used if selected by
	'eForwardPort'. Forwarding is done
	if 'eForwardPort = GSW_PORT_FORWARD_PORT'.
	This is Bridge Port ID in GSWIP-3.1. */
	u8	nForwardPortId;
	/** Snooping control class of service.
	Snooping control packet can be forwarded to the 'nForwardPortId' when
	selected in 'eIGMP_Mode'. The class of service of this port can be
	selected for the snooped control packets, starting from zero.
	The maximum possible service class depends
	on the hardware platform used. The value
	GSW_TRAFFIC_CLASS_DISABLE disables overwriting the given
	class assignment. */
	u8	nClassOfService;
	/** Robustness variable.
	Used when the hardware-based IGMP/MLD snooping function is enabled. This
	robust variable is used in case IGMP hardware learning is
	enabled ('eIGMP_Mode = GSW_MULTICAST_SNOOP_MODE_AUTOLEARNING').
	Supported range: 1 ... 3 */
	u8	nRobust;
	/** Query interval.
	Used to define the query interval in units of 100 ms when the
	hardware-based IGMP/MLD snooping function is enabled.
	The automatically learned router port will be aged out if no IGMP/MLD
	query frame is received from the router port
	for (nQueryInterval * nRobust) seconds.
	The supported range is from 100 ms to 25.5 s, with a default value
	of 10 s. This query interval is used in case IGMP hardware learning is
	enabled ('eIGMP_Mode = GSW_MULTICAST_SNOOP_MODE_AUTOLEARNING'). */
	u8	nQueryInterval;
	/** IGMP/MLD report suppression and Join Aggregation control.
	Whenever the report message is already sent out for the same multicast
	group, the successive report message within the
	query-max-responsetime with the same group ID will be filtered
	by the switch. This is called report suppression.
	Whenever the join message is already sent out for the same multicast
	group, the successive join message with the same group ID will be filtered.
	This is called join aggregation. This suppression control is used in
	case IGMP hardware learning is
	enable ('eIGMP_Mode = GSW_MULTICAST_SNOOP_MODE_AUTOLEARNING'). */
	GSW_multicastReportSuppression_t       eSuppressionAggregation;
	/** Hardware IGMP snooping fast leave option.
	Allows the hardware to automatically clear the membership
	when receiving the IGMP leave packet. This
	fast leave option is used in case IGMP hardware learning is
	enabled ('eIGMP_Mode = GSW_MULTICAST_SNOOP_MODE_AUTOLEARNING').
	Note: The fast-leave option shall only be enabled where only
	one host is connected to each interface.
	If fast-leave is enabled where more than one host is connected
	to an interface, some hosts might be dropped inadvertently.
	Fast-leave processing is supported only with IGMP version 2 hosts. */
	ltq_bool_t	bFastLeave;
	/** Hardware router port auto-learning. Allows for the
	    ports on which a router is located to be learned automatically. This router port learning option is
	    used in case IGMP hardware learning is
	    enabled ('eIGMP_Mode = GSW_MULTICAST_SNOOP_MODE_AUTOLEARNING'). */
	ltq_bool_t	bLearningRouter;
	/** Discard Unknown IP Multicast Packets.
	    Multicast packets are defined as unknown in case the group address
	    cannot be found in the switch multicast group table. The table group
	    entries could be either automatically learned or they are statically
	    added. This Boolean parameter defines if such unknown multicast
	    packet are forwarded to the multicast forwarding
	    portmap (command \ref GSW_PORT_CFG_SET,
	    parameter 'bMulticastUnknownDrop') or if they are dropped instead.

	    - 1: Drop unknown multicast packets.
	    - 0: Forward unknown multicast packets for the
	      multicast forwarding portmap.
	*/
	ltq_bool_t	bMulticastUnknownDrop;
	/** Multicast Forwarding based upon FID or Not - valid for GSWIP-3.0 only.
	- 1 : Multicast Forwarding is based upon FID.
	- 0 : Multicast Forwarding is not based upon FID.  **/
	ltq_bool_t	bMulticastFIDmode;
} GSW_multicastSnoopCfg_t;

/** \brief Add an Ethernet port as router port to the switch hardware multicast table.
    Used by \ref GSW_MULTICAST_ROUTER_PORT_ADD and \ref GSW_MULTICAST_ROUTER_PORT_REMOVE. */
typedef struct {
	/** Ethernet Port number (zero-based counting) in GSWIP-2.1/2.2/3.0. From
	    GSWIP-3.1, this field is Bridge Port ID. The valid range is hardware
	    dependent. An error code is delivered if the selected port is not
	    available.

	    \remarks
	    This field is used as portmap field, when the MSB bit is set.
	    In portmap mode, every value bit represents an Ethernet port.
	    LSB represents Port 0 with incrementing counting.
	    The (MSB - 1) bit represent the last port.
	    The macro \ref GSW_PORTMAP_FLAG_SET allows to set the MSB bit,
	    marking it as portmap variable.
	    Checking the portmap flag can be done by
	    using the \ref GSW_PORTMAP_FLAG_GET macro. */
	u16	nPortId;
} GSW_multicastRouter_t;

/** \brief Check if a port has been selected as a router port.
    Used by \ref GSW_MULTICAST_ROUTER_PORT_READ. Not applicable to GSWIP-3.1. */
typedef struct {
	/** Restart the get operation from the start of the table. Otherwise
	    return the next table entry (next to the entry that was returned
	    during the previous get operation). This parameter is always reset
	    during the read operation. This boolean parameter is set by the
	    calling application. */
	ltq_bool_t	bInitial;
	/** Indicates that the read operation got all last valid entries of the
	    table. This boolean parameter is set by the switch API
	    when the Switch API is called after the last valid one was returned already. */
	ltq_bool_t	bLast;
	/** Ethernet Port number (zero-based counting). The valid range is hardware
	    dependent. An error code is delivered if the selected port is not
	    available. */
	u16	nPortId;
} GSW_multicastRouterRead_t;

/** \brief Defines the multicast group member mode.
    Used by \ref GSW_multicastTable_t and \ref GSW_multicastTableRead_t. */
typedef enum {
	/** Include source IP address membership mode.
	    Only supported for IGMPv3. */
	GSW_IGMP_MEMBER_INCLUDE	= 0,
	/** Exclude source IP address membership mode.
	    Only supported for IGMPv2. */
	GSW_IGMP_MEMBER_EXCLUDE	= 1,
	/** Group source IP address is 'don't care'. This means all source IP
	    addresses (*) are included for the multicast group membership.
	    This is the default mode for IGMPv1 and IGMPv2. */
	GSW_IGMP_MEMBER_DONT_CARE	= 2
} GSW_IGMP_MemberMode_t;

/** \brief Add a host as a member to a multicast group.
    Used by \ref GSW_MULTICAST_TABLE_ENTRY_ADD and \ref GSW_MULTICAST_TABLE_ENTRY_REMOVE. */
typedef struct {
	/** Ethernet Port number (zero-based counting) in GSWIP-2.1/2.2/3.0. From
	    GSWIP-3.1, this field is Bridge Port ID. The valid range is hardware
	    dependent. An error code is delivered if the selected port is not
	    available. */
	u32	nPortId;
	/** Sub-Interface Id - valid for GSWIP 3.0/3.1 only */
	u16	nSubIfId;
	/** Select the IP version of the 'uIP_Gda' and 'uIP_Gsa' fields.
	    Both fields support either IPv4 or IPv6. */
	GSW_IP_Select_t	eIPVersion;
	/** Group Destination IP address (GDA). */
	GSW_IP_t	uIP_Gda;
	/** Group Source IP address. Only used in case IGMPv3 support is enabled
	    and 'eModeMember != GSW_IGMP_MEMBER_DONT_CARE'. */
	GSW_IP_t	uIP_Gsa;
	/** FID - valid for GSWIP 3.0 only subject to Global FID for MC is enabled.
	          always valid in GSWIP-3.1. */
	u8 nFID;
	/** Exclude Mode - valid for GSWIP 3.0 only - Includes or Excludes Source IP - uIP_Gsa */
	ltq_bool_t bExclSrcIP;
	/** Group member filter mode.
	    This is valid for GSWIP-3.0/3.1 to replaces bExclSrcIP.
	    This parameter is ignored when deleting a multicast membership table entry.
	    The configurations 'GSW_IGMP_MEMBER_EXCLUDE'
	    and 'GSW_IGMP_MEMBER_INCLUDE' are only supported
	    if IGMPv3 is used. */
	GSW_IGMP_MemberMode_t	eModeMember;
} GSW_multicastTable_t;

/** \brief Read out the multicast membership table.
    Used by \ref GSW_MULTICAST_TABLE_ENTRY_READ. */
typedef struct {
	/** Restart the get operation from the beginning of the table. Otherwise
	    return the next table entry (next to the entry that was returned
	    during the previous get operation). This parameter is always reset
	    during the read operation. This boolean parameter is set by the
	    calling application. */
	ltq_bool_t	bInitial;
	/** Indicates that the read operation got all last valid entries of the
	    table. This boolean parameter is set by the switch API
	    when the Switch API is called after the last valid one was returned already. */
	ltq_bool_t	bLast;
	/** Ethernet Port number (zero-based counting) in GSWIP-2.1/2.2/3.0. From
	    GSWIP-3.1, this field is Bridge Port ID. The valid range is hardware
	    dependent. An error code is delivered if the selected port is not
	    available.

	    \remarks
	    This field is used as portmap field, when the MSB bit is set.
	    In portmap mode, every value bit represents an Ethernet port.
	    LSB represents Port 0 with incrementing counting.
	    The (MSB - 1) bit represent the last port.
	    The macro \ref GSW_PORTMAP_FLAG_SET allows to set the MSB bit,
	    marking it as portmap variable.
	    Checking the portmap flag can be done by
	    using the \ref GSW_PORTMAP_FLAG_GET macro. */
	u16	nPortId;
	/** Ethernet Port Map - to support GSWIP-3.1, following field is added
	    for port map in static entry. It's valid only when MSB of nPortId is set.
	    Each bit stands for 1 bridge port. */
	u16 nPortMap[16];
	/** Sub-Interface Id - valid for GSWIP 3.0 only */
	u8	nSubIfId;
	/** Select the IP version of the 'uIP_Gda' and 'uIP_Gsa' fields.
	    Both fields support either IPv4 or IPv6. */
	GSW_IP_Select_t	eIPVersion;
	/** Group Destination IP address (GDA). */
	GSW_IP_t	uIP_Gda;
	/** Group Source IP address. Only used in case IGMPv3 support is enabled. */
	GSW_IP_t	uIP_Gsa;
	/** FID - valid for GSWIP 3.0 only subject to Global FID for MC is enabled */
	u8 nFID;
	/** Exclude Mode - valid for GSWIP 3.0 only - Includes or Excludes Source IP - uIP_Gsa */
	ltq_bool_t bExclSrcIP;
	/** Group member filter mode.
	    This parameter is ignored when deleting a multicast membership table entry.
	    The configurations 'GSW_IGMP_MEMBER_EXCLUDE'
	    and 'GSW_IGMP_MEMBER_INCLUDE' are only supported
	    if IGMPv3 is used. */
	GSW_IGMP_MemberMode_t	eModeMember;
	/* MULTICAST Table Hit Status Update (Supported in GSWip-3.1/3.2) */
	ltq_bool_t hitstatus;
} GSW_multicastTableRead_t;

/*@}*/ /* GSW_IOCTL_MULTICAST */

/** \addtogroup GSW_IOCTL_OAM */
/*@{*/

/** \brief Maximum version information string length. */
#define GSW_VERSION_LEN 64

/** \brief Maximum String Length for the Capability String. */
#define GSW_CAP_STRING_LEN 128

/** \brief Sets the portmap flag of a PortID variable.
    Some Switch API commands allow to use a port index as portmap variable.
    This requires that the MSB bit is set to indicate that this variable
    contains a portmap, instead of a port index.
    In portmap mode, every value bit represents an Ethernet port.
    LSB represents Port 0 with incrementing counting.
    The (MSB - 1) bit represent the last port. */
#define GSW_PORTMAP_FLAG_SET(varType) (1 << ( sizeof(((varType *)0)->nPortId) * 8 - 1))

/** \brief Checks the portmap flag of a PortID variable.
    Some Switch API commands allow to use a port index as portmap variable.
    This requires that the MSB bit is set to indicate that this variable
    contains a portmap, instead of a port index.
    In portmap mode, every value bit represents an Ethernet port.
    LSB represents Port 0 with incrementing counting.
    The (MSB - 1) bit represent the last port. */
#define GSW_PORTMAP_FLAG_GET(varType) (1 << ( sizeof(((varType *)0)->nPortId) * 8 - 1))

/** \brief Data structure used to request the Switch API and device hardware
    version information. A zero-based index is provided to the Switch API that
    describes the request version information.
    Used by \ref GSW_VERSION_GET. */
typedef struct {
	/** Version ID starting with 0. */
	u16	nId;
	/** Name or ID of the version information. */
	char cName[GSW_VERSION_LEN];
	/** Version string information. */
	char	cVersion[GSW_VERSION_LEN];
} GSW_version_t;

/** \brief Switch API hardware initialization mode.
    Used by \ref GSW_HW_Init_t. */
typedef enum {
	/** Access the switch hardware to read out status and capability
	    information. Then define the basic hardware configuration to bring
	    the hardware into a pre-defined state. */
	GSW_HW_INIT_WR	= 0,
	/** Access the switch hardware to read out status and capability
	    information. Do not write any hardware configuration to the device.
	    This means that the current existing hardware configuration remains
	    unchanged. */
	GSW_HW_INIT_RO	= 1,
	/** Initialize the switch software module but do not touch the switch
	    hardware. This means that no read or write operations are done on
	    the switch hardware. Status and capability information cannot be
	    retrieved from the hardware. */
	GSW_HW_INIT_NO	= 2
} GSW_HW_InitMode_t;

/** \brief Switch hardware platform initialization structure.
    Used by \ref GSW_HW_INIT. */
typedef struct {
	/** Select the type of Switch API and hardware initialization. */
	GSW_HW_InitMode_t	eInitMode;
} GSW_HW_Init_t;

/** \brief Aging Timer Value.
    Used by \ref GSW_cfg_t. */
typedef enum {
	/** 1 second aging time */
	GSW_AGETIMER_1_SEC	= 1,
	/** 10 seconds aging time */
	GSW_AGETIMER_10_SEC	= 2,
	/** 300 seconds aging time */
	GSW_AGETIMER_300_SEC	= 3,
	/** 1 hour aging time */
	GSW_AGETIMER_1_HOUR	= 4,
	/** 24 hours aging time */
	GSW_AGETIMER_1_DAY	= 5,
	/** Custom aging time in seconds */
	GSW_AGETIMER_CUSTOM  = 6
} GSW_ageTimer_t;

/** \brief Ethernet port speed mode.
    For certain generations of GSWIP, a port might support only a subset of the possible settings.
    Used by \ref GSW_portLinkCfg_t. */
typedef enum {
	/** 10 Mbit/s */
	GSW_PORT_SPEED_10	= 10,
	/** 100 Mbit/s */
	GSW_PORT_SPEED_100	= 100,
	/** 200 Mbit/s */
	GSW_PORT_SPEED_200	= 200,
	/** 1000 Mbit/s */
	GSW_PORT_SPEED_1000	= 1000,
	/** 2.5 Gbit/s */
	GSW_PORT_SPEED_25000	= 25000,
	/** 10 Gbit/s */
	GSW_PORT_SPEED_100000 = 100000,
} GSW_portSpeed_t;

/** \brief Ethernet port duplex status.
    Used by \ref GSW_portLinkCfg_t. */
typedef enum {
	/** Port operates in full-duplex mode */
	GSW_DUPLEX_FULL	= 0,
	/** Port operates in half-duplex mode */
	GSW_DUPLEX_HALF	= 1,
	/** Port operates in Auto mode */
	GSW_DUPLEX_AUTO	= 2,
} GSW_portDuplex_t;

/** \brief Force the MAC and PHY link modus.
    Used by \ref GSW_portLinkCfg_t. */
typedef enum {
	/** Link up. Any connected LED
	    still behaves based on the real PHY status. */
	GSW_PORT_LINK_UP	= 0,
	/** Link down. */
	GSW_PORT_LINK_DOWN	= 1,
	/** Link Auto. */
	GSW_PORT_LINK_AUTO	= 2,
} GSW_portLink_t;

/** \brief Enumeration used for Switch capability types. GSWIP-3.0 only capabilities are explicitly indicated.
    Used by \ref GSW_cap_t. */
typedef enum {
	/** Number of physical Ethernet ports. */
	GSW_CAP_TYPE_PORT = 0,
	/** Number of virtual Ethernet ports. */
	GSW_CAP_TYPE_VIRTUAL_PORT = 1,
	/** Size of internal packet memory [in Bytes]. */
	GSW_CAP_TYPE_BUFFER_SIZE = 2,
	/** Buffer segment size.
	    Byte size of a segment, used to store received packet data. */
	GSW_CAP_TYPE_SEGMENT_SIZE = 3,
	/** Number of priority queues per device. */
	GSW_CAP_TYPE_PRIORITY_QUEUE = 4,
	/** Number of meter instances. */
	GSW_CAP_TYPE_METER	= 5,
	/** Number of rate shaper instances. */
	GSW_CAP_TYPE_RATE_SHAPER	= 6,
	/** Number of CTAG VLAN groups that can be configured on the switch hardware. */
	GSW_CAP_TYPE_VLAN_GROUP	= 7,
	/** Number of Filtering Identifiers (FIDs) */
	GSW_CAP_TYPE_FID	= 8,
	/** Number of MAC Bridging table entries */
	GSW_CAP_TYPE_MAC_TABLE_SIZE	= 9,
	/** Number of multicast level 3 hardware table entries */
	GSW_CAP_TYPE_MULTICAST_TABLE_SIZE      = 10,
	/** Number of supported PPPoE sessions. */
	GSW_CAP_TYPE_PPPOE_SESSION	= 11,
	/** Number of STAG VLAN groups that can be configured on the switch hardware. */
	GSW_CAP_TYPE_SVLAN_GROUP	= 12,
	/** Number of PMAC Supported in Switch Macro - for GSWIP-3.0 only. */
	GSW_CAP_TYPE_PMAC	= 13,
	/** Number of entries in Payload Table size - for GSWIP-3.0 only. */
	GSW_CAP_TYPE_PAYLOAD	= 14,
	/** Number of RMON Counters Supported - for GSWIP-3.0 only. */
	GSW_CAP_TYPE_IF_RMON	= 15,
	/** Number of Egress VLAN Treatment Entries - for GSWIP-3.0 only. */
	GSW_CAP_TYPE_EGRESS_VLAN = 16,
	/** Number of Routing Source-MAC Entries - for GSWIP-R-3.0 only. */
	GSW_CAP_TYPE_RT_SMAC = 17,
	/** Number of Routing Destination-MAC Entries - for GSWIP-R-3.0 only. */
	GSW_CAP_TYPE_RT_DMAC = 18,
	/** Number of Routing-PPPoE Entries - for GSWIP-R-3.0 only. */
	GSW_CAP_TYPE_RT_PPPoE = 19,
	/** Number of Routing-NAT Entries - for GSWIP-R-3.0 only. */
	GSW_CAP_TYPE_RT_NAT = 20,
	/** Number of MTU Entries - for GSWIP-R-3.0 only. */
	GSW_CAP_TYPE_RT_MTU = 21,
	/** Number of Tunnel Entries - for GSWIP-R-3.0 only. */
	GSW_CAP_TYPE_RT_TUNNEL = 22,
	/** Number of RTP Entries - for GSWIP-R-3.0 only. */
	GSW_CAP_TYPE_RT_RTP = 23,
	/** Number of CTP ports - for GSWIP-3.1 only. */
	GSW_CAP_TYPE_CTP = 24,
	/** Number of bridge ports - for GSWIP-3.1 only. */
	GSW_CAP_TYPE_BRIDGE_PORT = 25,
	/** Number of COMMON PCE Rules. */
	GSW_CAP_TYPE_COMMON_TFLOW_RULES = 26,
	/** Last Capability Index */
	GSW_CAP_TYPE_LAST	= 27
} GSW_capType_t;

/** \brief Capability structure.
    Used by \ref GSW_CAP_GET. */
typedef struct {
	/** Defines the capability type, see \ref GSW_capType_t.*/
	GSW_capType_t	nCapType;
	/** Description of the capability. */
	char cDesc[GSW_CAP_STRING_LEN];
	/** Defines if, what or how many are available. The definition of cap
	depends on the type, see captype. */
	u32 nCap;
} GSW_cap_t;

/** \brief Global Switch configuration Attributes.
    Used by \ref GSW_CFG_SET and \ref GSW_CFG_GET. */
typedef struct {
	/** MAC table aging timer. After this timer expires the MAC table
	    entry is aged out. */
	GSW_ageTimer_t	eMAC_TableAgeTimer;
	/** If eMAC_TableAgeTimer = GSW_AGETIMER_CUSTOM, this variable defines
	    MAC table aging timer in seconds. */
	u32  nAgeTimer;
	/** VLAN Awareness. The switch is VLAN unaware if this variable is disabled.
	    In this mode, no VLAN-related APIs are supported and return with an error.
	    The existing VLAN configuration is discarded when VLAN is disabled again. */
	ltq_bool_t	bVLAN_Aware;
	/** Maximum Ethernet packet length. */
	u16	nMaxPacketLen;
	/** Automatic MAC address table learning limitation consecutive action.
	    These frame addresses are not learned, but there exists control as to whether
	    the frame is still forwarded or dropped.

	    - False: Drop
	    - True: Forward
	*/
	ltq_bool_t	bLearningLimitAction;
	/** Accept or discard MAC spoofing and port MAC locking violation packets.
	    MAC spoofing detection features identifies ingress packets that carry
	    a MAC source address which was previously learned on a different
	    ingress port (learned by MAC bridging table). This also applies to
	    static added entries. MAC spoofing detection is enabled on port
	    level by 'bMAC_SpoofingDetection'.
	    MAC address port locking is configured on port level
	    by 'bLearningMAC_PortLock'.

	    - False: Drop
	    - True: Forward
	*/
	ltq_bool_t	bMAC_SpoofingAction;
	/** Pause frame MAC source address mode. If enabled, use the alternative
	    address specified with 'nMAC'. */
	ltq_bool_t	bPauseMAC_ModeSrc;
	/** Pause frame MAC source address. */
	u8	nPauseMAC_Src[GSW_MAC_ADDR_LEN];
} GSW_cfg_t;

/** \brief Port Enable Type Selection.
    Used by \ref GSW_portCfg_t. */
typedef enum {
	/** The port is disabled in both directions. */
	GSW_PORT_DISABLE	= 0,
	/** The port is enabled in both directions (ingress and egress). */
	GSW_PORT_ENABLE_RXTX	= 1,
	/** The port is enabled in the receive (ingress) direction only. */
	GSW_PORT_ENABLE_RX	= 2,
	/** The port is enabled in the transmit (egress) direction only. */
	GSW_PORT_ENABLE_TX	= 3
} GSW_portEnable_t;

/** \brief Port Mirror Options.
    Used by \ref GSW_portCfg_t. */
typedef enum {
	/** Mirror Feature is disabled. Normal port usage. */
	GSW_PORT_MONITOR_NONE	= 0,
	/** Port Ingress packets are mirrored to the monitor port. */
	GSW_PORT_MONITOR_RX	= 1,
	/** Port Egress packets are mirrored to the monitor port. */
	GSW_PORT_MONITOR_TX	= 2,
	/** Port Ingress and Egress packets are mirrored to the monitor port. */
	GSW_PORT_MONITOR_RXTX	= 3,
	/** Packet mirroring of 'unknown VLAN violation' frames. */
	GSW_PORT_MONITOR_VLAN_UNKNOWN          = 4,
	/** Packet mirroring of 'VLAN ingress or egress membership violation' frames. */
	GSW_PORT_MONITOR_VLAN_MEMBERSHIP       = 16,
	/** Packet mirroring of 'port state violation' frames. */
	GSW_PORT_MONITOR_PORT_STATE	= 32,
	/** Packet mirroring of 'MAC learning limit violation' frames. */
	GSW_PORT_MONITOR_LEARNING_LIMIT        = 64,
	/** Packet mirroring of 'port lock violation' frames. */
	GSW_PORT_MONITOR_PORT_LOCK	= 128
} GSW_portMonitor_t;

/** \brief Ethernet flow control status.
    Used by \ref GSW_portCfg_t. */
typedef enum {
	/** Automatic flow control mode selection through auto-negotiation. */
	GSW_FLOW_AUTO	= 0,
	/** Receive flow control only */
	GSW_FLOW_RX	= 1,
	/** Transmit flow control only */
	GSW_FLOW_TX	= 2,
	/** Receive and Transmit flow control */
	GSW_FLOW_RXTX	= 3,
	/** No flow control */
	GSW_FLOW_OFF	= 4
} GSW_portFlow_t;

/** \brief Interface RMON Counter Mode - (FID, SUBID or FLOWID) Config - GSWIP-3.0 only.
    Used by \ref GSW_portCfg_t. */
typedef enum {
	/** FID based Interface RMON counters Usage */
	GSW_IF_RMON_FID	= 0,
	/** Sub-Interface Id based Interface RMON counters Usage */
	GSW_IF_RMON_SUBID	= 1,
	/** Flow Id (LSB bits 3 to 0) based Interface RMON counters Usage */
	GSW_IF_RMON_FLOWID_LSB	= 2,
	/** Flow Id (MSB bits 7 to 4) based Interface RMON counters Usage */
	GSW_IF_RMON_FLOWID_MSB	= 3
} GSW_If_RMON_Mode_t;

/** \brief Port Type - GSWIP-3.1 only.
   Used by \ref GSW_portCfg_t. */
typedef enum {
	/** Logical Port */
	GSW_LOGICAL_PORT = 0,
	/** Physical Port
	    Applicable only for GSWIP-3.1/3.2 */
	GSW_PHYSICAL_PORT = 1,
	/** Connectivity Termination Port (CTP)
	    Applicable only for GSWIP-3.1/3.2 */
	GSW_CTP_PORT = 2,
	/** Bridge Port
	    Applicable only for GSWIP-3.1/3.2 */
	GSW_BRIDGE_PORT = 3
} GSW_portType_t;

/** \brief Port Configuration Parameters.
    Used by \ref GSW_PORT_CFG_GET and \ref GSW_PORT_CFG_SET. */
typedef struct {
	/** Port Type. This gives information which type of port is configured.
	    nPortId should be based on this field. */
	GSW_portType_t ePortType;

	/** Ethernet Port number (zero-based counting). The valid range is hardware
	    dependent. An error code is delivered if the selected port is not
	    available. */
	u16	nPortId;
	/** Enable Port (ingress only, egress only, both directions, or disabled).
	    This parameter is used for Spanning Tree Protocol and 802.1X applications. */
	GSW_portEnable_t	eEnable;
	/** Drop unknown unicast packets.
	    Do not send out unknown unicast packets on this port,
	    if the boolean parameter is enabled. By default packets of this type
	    are forwarded to this port. */
	ltq_bool_t	bUnicastUnknownDrop;
	/** Drop unknown multicast packets.
	    Do not send out unknown multicast packets on this port,
	    if boolean parameter is enabled. By default packets of this type
	    are forwarded to this port.
	    Some platforms also drop broadcast packets. */
	ltq_bool_t	bMulticastUnknownDrop;
	/** Drop reserved packet types
	    (destination address from '01 80 C2 00 00 00' to
	    '01 80 C2 00 00 2F') received on this port. */
	ltq_bool_t	bReservedPacketDrop;
	/** Drop Broadcast packets received on this port. By default packets of this
	  type are forwarded to this port. */
	ltq_bool_t	bBroadcastDrop;
	/** Enables MAC address table aging.
	    The MAC table entries learned on this port are removed after the
	    aging time has expired.
	    The aging time is a global parameter, common to all ports. */
	ltq_bool_t	bAging;
	/** MAC address table learning on the port specified by 'nPortId'.
	    By default this parameter is always enabled. */
	ltq_bool_t	bLearning;
	/** Automatic MAC address table learning locking on the port specified
	    by 'nPortId'.
	    This parameter is only taken into account when 'bLearning' is enabled. */
	ltq_bool_t	bLearningMAC_PortLock;
	/** Automatic MAC address table learning limitation on this port.
	    The learning functionality is disabled when the limit value is zero.
	    The value 0xFFFF to allow unlimited learned address.
	    This parameter is only taken into account when 'bLearning' is enabled. */
	u16 nLearningLimit;
	/** MAC spoofing detection. Identifies ingress packets that carry
	    a MAC source address which was previously learned on a different ingress
	    port (learned by MAC bridging table). This also applies to static added
	    entries. Those violated packets could be accepted or discarded,
	    depending on the global switch configuration 'bMAC_SpoofingAction'.
	    This parameter is only taken into account when 'bLearning' is enabled. */
	ltq_bool_t	bMAC_SpoofingDetection;
	/** Port Flow Control Status. Enables the flow control function. */
	GSW_portFlow_t	eFlowCtrl;
	/** Port monitor feature. Allows forwarding of egress and/or ingress
	    packets to the monitor port. If enabled, the monitor port gets
	    a copy of the selected packet type. */
	GSW_portMonitor_t	ePortMonitor;
	/** Assign Interface RMON Counters for this Port - GSWIP-3.0 */
	ltq_bool_t	bIfCounters;
	/** Interface RMON Counters Start Index - GSWIP-3.0.
	    Value of (-1) denotes unassigned Interface Counters.
	    Valid range : 0-255 available to be shared amongst ports in desired way*/
	int nIfCountStartIdx;
	/** Interface RMON Counters Mode - GSWIP-3.0 */
	GSW_If_RMON_Mode_t	eIfRMONmode;
} GSW_portCfg_t;

/** \brief Special tag Ethertype mode */
typedef enum {
	/** The EtherType field of the Special Tag of egress packets is always set
	    to a prefined value. This same defined value applies for all
	    switch ports. */
	GSW_CPU_ETHTYPE_PREDEFINED	= 0,
	/** The Ethertype field of the Special Tag of egress packets is set to
	    the FlowID parameter, which is a results of the switch flow
	    classification result. The switch flow table rule provides this
	    FlowID as action parameter. */
	GSW_CPU_ETHTYPE_FLOWID	= 1
} GSW_CPU_SpecialTagEthType_t;

/** \brief Parser Flags and Offsets Header settings on CPU Port for GSWIP-3.0.
    Used by \ref GSW_CPU_PortCfg_t. */
typedef enum {
	/** No Parsing Flags or Offsets accompanying to CPU Port for this combination */
	GSW_CPU_PARSER_NIL	= 0,
	/** 8-Bytes Parsing Flags (Bit 63:0) accompanying to CPU Port for this combination */
	GSW_CPU_PARSER_FLAGS	= 1,
	/** 40-Bytes Offsets (Offset-0 to -39) + 8-Bytes Parsing Flags (Bit 63:0) accompanying to CPU Port for this combination */
	GSW_CPU_PARSER_OFFSETS_FLAGS	= 2,
	/** Reserved - for future use */
	GSW_CPU_PARSER_RESERVED = 3
} GSW_CPU_ParserHeaderCfg_t;

/** \brief FCS and Pad Insertion operations for GSWIP 3.1
    Used by \ref GSW_CPU_PortCfgSet/Get. */
typedef enum {
	/** CRC Pad Insertion Enable */
	GSW_CRC_PAD_INS_EN	= 0,
	/** CRC Insertion Enable Pad Insertion Disable */
	GSW_CRC_EN_PAD_DIS	= 1,
	/** CRC Pad Insertion Disable */
	GSW_CRC_PAD_INS_DIS	= 2
} GSW_FCS_TxOps_t;

/** \brief Defines one port that is directly connected to the CPU and its applicable settings.
    Used by \ref GSW_CPU_PORT_CFG_SET and \ref GSW_CPU_PORT_CFG_GET. */
typedef struct {
	/** Ethernet Port number (zero-based counting) set to CPU Port. The valid number is hardware
	    dependent. (E.g. Port number 0 for GSWIP-3.0 or 6 for GSWIP-2.x). An error code is delivered if the selected port is not
	    available. */
	u16	nPortId;
	/** CPU port validity.
	    Set command: set true to define a CPU port, set false to undo the setting.
	    Get command: true if defined as CPU, false if not defined as CPU port. */
	ltq_bool_t	bCPU_PortValid;
	/** Special tag enable in ingress direction. */
	ltq_bool_t	bSpecialTagIngress;
	/** Special tag enable in egress direction. */
	ltq_bool_t	bSpecialTagEgress;
	/** Enable FCS check

	    - false: No check, forward all frames
	    - 1: Check FCS, drop frames with errors
	*/
	ltq_bool_t	bFcsCheck;
	/** Enable FCS generation

	    - false: Forward packets without FCS
	    - 1: Generate FCS for all frames
	*/
	ltq_bool_t	bFcsGenerate;
	/** Special tag Ethertype mode. Not applicable to GSWIP-3.1. */
	GSW_CPU_SpecialTagEthType_t	bSpecialTagEthType;
	/** GSWIP-3.0 specific Parser Header Config for no MPE flags (i.e. MPE1=0, MPE2=0). */
	GSW_CPU_ParserHeaderCfg_t  eNoMPEParserCfg;
	/** GSWIP-3.0 specific Parser Header Config for MPE-1 set flag (i.e. MPE1=1, MPE2=0). */
	GSW_CPU_ParserHeaderCfg_t  eMPE1ParserCfg;
	/** GSWIP-3.0 specific Parser Header Config for MPE-2 set flag (i.e. MPE1=0, MPE2=1). */
	GSW_CPU_ParserHeaderCfg_t  eMPE2ParserCfg;
	/** GSWIP-3.0 specific Parser Header Config for both MPE-1 and MPE-2 set flag (i.e. MPE1=1, MPE2=1). */
	GSW_CPU_ParserHeaderCfg_t  eMPE1MPE2ParserCfg;
	/** GSWIP-3.1 FCS tx Operations. */
	GSW_FCS_TxOps_t bFcsTxOps;
	/** GSWIP-3.2 Time Stamp Field Removal for PTP Packet
	    0 - DIS Removal is disabled
	    1 - EN Removal is enabled
	*/
	ltq_bool_t	bTsPtp;
	/** GSWIP-3.2 Time Stamp Field Removal for Non-PTP Packet
	    0 - DIS Removal is disabled
	    1 - EN Removal is enabled
	*/
	ltq_bool_t	bTsNonptp;
} GSW_CPU_PortCfg_t;

/** \brief Ethernet layer-2 header selector, when adding or removing on
    transmitted packets.
    Used by \ref GSW_CPU_PortExtendCfg_t. */
typedef enum {
	/** No additional Ethernet header. */
	GSW_CPU_HEADER_NO	= 0,
	/** Additional Ethernet header. */
	GSW_CPU_HEADER_MAC	= 1,
	/** Additional Ethernet- and CTAG VLAN- header. */
	GSW_CPU_HEADER_VLAN	= 2
} GSW_CPU_HeaderMode_t;

/** \brief CPU Port Layer-2 Header extension.
    Used by \ref GSW_CPU_PortExtendCfg_t. */
typedef struct {
	/** Packet MAC Source Address. */
	u8	nMAC_Src[GSW_MAC_ADDR_LEN];
	/** Packet MAC Destination Address. */
	u8	nMAC_Dst[GSW_MAC_ADDR_LEN];
	/** Packet EtherType Field. */
	u16	nEthertype;
	/** CTAG VLAN Tag Priority Field.
	    Only used when adding VLAN tag is
	    enabled (eHeaderAdd=GSW_CPU_HEADER_VLAN). */
	u8	nVLAN_Prio;
	/** CTAG VLAN Tag Canonical Format Identifier.
	    Only used when adding VLAN tag is
	    enabled (eHeaderAdd=GSW_CPU_HEADER_VLAN). */
	u8	nVLAN_CFI;
	/** CTAG VLAN Tag VID.
	    Only used when adding VLAN tag is
	    enabled (eHeaderAdd=GSW_CPU_HEADER_VLAN). */
	u16	nVLAN_ID;
} GSW_CPU_Header_t;

/** \brief CPU port PAUSE frame handling.
    Used by \ref GSW_CPU_PortExtendCfg_t. */
typedef enum {
	/** Forward all PAUSE frames coming from the switch macro towards
	    the DMA channel. These frames do not influence the packet transmission. */
	GSW_CPU_PAUSE_FORWARD	= 0,
	/** Dispatch all PAUSE frames coming from the switch macro towards
	    the DMA channel. These are filtered out and the packets transmission is
	    stopped and restarted accordingly. */
	GSW_CPU_PAUSE_DISPATCH	= 1
} GSW_CPU_Pause_t;

/** \brief Ethernet port interface mode.
    A port might support only a subset of the possible settings.
    Used by \ref GSW_portLinkCfg_t. */
typedef enum {
	/** Normal PHY interface (twisted pair), use the internal MII Interface. */
	GSW_PORT_HW_MII	= 0,
	/** Reduced MII interface in normal mode. */
	GSW_PORT_HW_RMII	= 1,
	/** GMII or MII, depending upon the speed. */
	GSW_PORT_HW_GMII	= 2,
	/** RGMII mode. */
	GSW_PORT_HW_RGMII = 3,
	/** XGMII mode. */
	GSW_PORT_HW_XGMII = 4,
} GSW_MII_Mode_t;

/** \brief Ethernet port configuration for PHY or MAC mode.
    Used by \ref GSW_portLinkCfg_t. */
typedef enum {
	/** MAC Mode. The Ethernet port is configured to work in MAC mode. */
	GSW_PORT_MAC	= 0,
	/** PHY Mode. The Ethernet port is configured to work in PHY mode. */
	GSW_PORT_PHY	= 1
} GSW_MII_Type_t;

/** \brief Ethernet port clock source configuration.
    Used by \ref GSW_portLinkCfg_t. */
typedef enum {
	/** Clock Mode not applicable. */
	GSW_PORT_CLK_NA	= 0,
	/** Clock Master Mode. The port is configured to provide the clock as output signal. */
	GSW_PORT_CLK_MASTER	= 1,
	/** Clock Slave Mode. The port is configured to use the input clock signal. */
	GSW_PORT_CLK_SLAVE	= 2
} GSW_clkMode_t;

/** \brief Additional CPU port configuration for platforms where the CPU port is
    fixed set on a dedicated port.

    Used by \ref GSW_CPU_PORT_EXTEND_CFG_SET
    and \ref GSW_CPU_PORT_EXTEND_CFG_GET. */
typedef struct {
	/** Add Ethernet layer-2 header (also CTAG VLAN) to the transmit packet.
	    The corresponding header fields are set in 'sHeader'. */
	GSW_CPU_HeaderMode_t	eHeaderAdd;
	/** Remove Ethernet layer-2 header (also CTAG VLAN) for packets going from
	    Ethernet switch to the DMA. Only the first VLAN tag found is removed
	    and additional available VLAN tags remain untouched. */
	ltq_bool_t	bHeaderRemove;
	/** Ethernet layer-2 header information. Used when adding a header to the
	    transmitted packet. The parameter 'eHeaderAdd' selects the mode if
	    a layer-2 header should be added (including VLAN).
	    This structure contains all fields of the Ethernet and VLAN header. */
	GSW_CPU_Header_t	sHeader;
	/** Describes how the port handles received PAUSE frames coming from the
	    switch. Either forward them to DMA or stop/start transmission.
	    Note that the parameter 'eFlowCtrl' of the
	    command 'GSW_PORT_CFG_SET' determines whether the switch
	    generates PAUSE frames. */
	GSW_CPU_Pause_t	ePauseCtrl;
	/** Remove the CRC (FCS) of all packets coming from the switch towards
	    the DMA channel.
	    Note that the FCS check and generation option can be configured
	    using 'GSW_CPU_PORT_CFG_SET'. */
	ltq_bool_t	bFcsRemove;
	/** Port map of Ethernet switch ports that are assigned to the WAN side
	    (dedicated for applications where ports are grouped into WAN- and
	    LAN-segments). All ports that are not selected belong to the LAN segment.
	    The LSB bit represents port 0, the higher bits represent the higher
	    port numbers. */
	u32	nWAN_Ports;
} GSW_CPU_PortExtendCfg_t;

/** \brief Ethernet port link, speed status and flow control status.
    Used by \ref GSW_PORT_LINK_CFG_GET and \ref GSW_PORT_LINK_CFG_SET. */
typedef struct {
	/** Ethernet Port number (zero-based counting). The valid range is hardware
	    dependent. An error code is delivered if the selected port is not
	    available. */
	u16	nPortId;
	/** Force Port Duplex Mode.

	    - 0: Negotiate Duplex Mode. Auto-negotiation mode. Negotiated
	      duplex mode given in 'eDuplex'
	      during GSW_PORT_LINK_CFG_GET calls.
	    - 1: Force Duplex Mode. Force duplex mode in 'eDuplex'.
	*/
	ltq_bool_t	bDuplexForce;
	/** Port Duplex Status. */
	GSW_portDuplex_t	eDuplex;
	/** Force Link Speed.

	    - 0: Negotiate Link Speed. Negotiated speed given in
	      'eSpeed' during GSW_PORT_LINK_CFG_GET calls.
	    - 1: Force Link Speed. Forced speed mode in 'eSpeed'.
	*/
	ltq_bool_t	bSpeedForce;
	/** Ethernet port link up/down and speed status. */
	GSW_portSpeed_t	eSpeed;
	/** Force Link.

	     - 0: Auto-negotiate Link. Current link status is given in
	       'eLink' during GSW_PORT_LINK_CFG_GET calls.
	     - 1: Force Duplex Mode. Force duplex mode in 'eLink'.
	 */
	ltq_bool_t	bLinkForce;
	/** Link Status. Read out the current link status.
	    Note that the link could be forced by setting 'bLinkForce'. */
	GSW_portLink_t	eLink;
	/** Selected interface mode (MII/RMII/RGMII/GMII/XGMII). */
	GSW_MII_Mode_t	eMII_Mode;
	/** Select MAC or PHY mode (PHY = Reverse xMII). */
	GSW_MII_Type_t	eMII_Type;
	/** Interface Clock mode (used for RMII mode). */
	GSW_clkMode_t	eClkMode;
	/** 'Low Power Idle' Support for 'Energy Efficient Ethernet'.
	    Only enable this feature in case the attached PHY also supports it. */
	ltq_bool_t	bLPI;
} GSW_portLinkCfg_t;

/** \brief Ethernet Interface RGMII Clock Configuration. Only needed in case the
    interface runs in RGMII mode.
    Used by \ref GSW_PORT_RGMII_CLK_CFG_SET and \ref GSW_PORT_RGMII_CLK_CFG_GET. */
typedef struct {
	/** Ethernet Port number (zero-based counting). The valid range is hardware
	    dependent. An error code is delivered if the selected port is not
	    available. */
	u16	nPortId;
	/** Clock Delay RX [multiple of 500 ps]. */
	u8	nDelayRx;
	/** Clock Delay TX [multiple of 500 ps]. */
	u8	nDelayTx;
} GSW_portRGMII_ClkCfg_t;

/** \brief Query whether the Ethernet switch hardware has detected a connected
    PHY on the port.
    Used by \ref GSW_PORT_PHY_QUERY. */
typedef struct {
	/** Ethernet Port number (zero-based counting). The valid range is hardware
	    dependent. An error code is delivered if the selected port is not
	    available. */
	u16	nPortId;
	/** Check if the Ethernet switch hardware has detected a connected PHY
	    on this port. */
	ltq_bool_t	bPHY_Present;
} GSW_portPHY_Query_t;

/** \brief Ethernet PHY address definition. Defines the relationship between a
    bridge port and the MDIO address of a PHY that is attached to this port.
    Used by \ref GSW_PORT_PHY_ADDR_GET. */
typedef struct {
	/** Ethernet Port number (zero-based counting). The valid range is hardware
	    dependent. An error code is delivered if the selected port is not
	    available. */
	u16	nPortId;
	/** Device address on the MDIO interface */
	u8	nAddressDev;
} GSW_portPHY_Addr_t;

/** \brief Port redirection control.
    Used by \ref GSW_PORT_REDIRECT_GET and \ref GSW_PORT_REDIRECT_SET. */
typedef struct {
	/** Ethernet Port number (zero-based counting). The valid range is hardware
	    dependent. An error code is delivered if the selected port is not
	    available. */
	u16	nPortId;
	/** Port Redirect Option.
	    If enabled, all packets destined to 'nPortId' are redirected to the
	    CPU port. The destination port map in the status header information is
	    not changed so that the original destination port can be identified by
	    software. */
	ltq_bool_t	bRedirectEgress;
	/** Port Ingress Direct Forwarding.
	    If enabled, all packets sourced from 'nPortId' are directly forwarded to queue 0
	    of the CPU port. These packets are not modified and are not affected by
	    normal learning, look up, VLAN processing and queue selection. */
	ltq_bool_t	bRedirectIngress;
} GSW_portRedirectCfg_t;

/** \brief Port monitor configuration.
    Used by \ref GSW_MONITOR_PORT_CFG_GET and \ref GSW_MONITOR_PORT_CFG_SET. */
typedef struct {
	/** Ethernet Port number (zero-based counting). The valid range is hardware
	    dependent. An error code is delivered if the selected port is not
	    available. */
	u16	nPortId;
	/** This port is used as a monitor port. To use this feature, the port
	  mirror function is enabled on one or more ports. */
	ltq_bool_t	bMonitorPort;
	/* Monitoring Sub-IF id */
	u16	nSubIfId;
	/**Enable Ingress Mirror Constants.Only valid when ingress PCE bypass
	is enabled.Applicable for GSWIP 3.2*/
	ltq_bool_t	bEnableIngressMirror;
	/**Enable Egress Mirror Constants.Applicable for GSWIP 3.2*/
	ltq_bool_t	bEnableEgressMirror;
	/**Monitor Lpid, if Ingress or Egress Mirroring enabled.
	   Applicable for GSWIP 3.2*/
	u16 nMonitorLpId;
	ltq_bool_t	bEnableMonitorQmap;
	u16 nMonitorQid;
	u16 nRedirectPortId;
} GSW_monitorPortCfg_t;

/** \brief MDIO Interface Configuration.
    Used by \ref GSW_MDIO_CFG_GET and \ref GSW_MDIO_CFG_SET. */
typedef struct {
	/** MDIO interface clock and data rate [in kHz]. */
	u32	nMDIO_Speed;
	/** MDIO interface enable. */
	ltq_bool_t	bMDIO_Enable;
} GSW_MDIO_cfg_t;

/** \brief MDIO Register Access.
    The 'nData' value is directly written to the device register
    or read from the device.
    Some PHY device registers have standard bit definitions as stated in
    IEEE 802.
    Used by \ref GSW_MDIO_DATA_READ and \ref GSW_MDIO_DATA_WRITE. */
typedef struct {
	/** Device address on the MDIO interface */
	u8	nAddressDev;
	/** Register address inside the device. */
	u8	nAddressReg;
	/** Exchange data word with the device (read / write). */
	u16	nData;
} GSW_MDIO_data_t;

/** \brief MAC Cli struct.
     MAC Cli struct for passing args and argument values. */
typedef struct {
	/** Number of args */
	u32 argc;
	/** Argument values */
	u8 argv[15][30];
} GSW_MAC_cfg_t;

/** \brief MACSec Cli struct.
     MACSec Cli struct for passing args and argument values. */
typedef struct {
	/** Command Type */
	u32 cmdType;
	/** Devid=0 Ingress, Devid=1 Egress */
	u32 devid;
	/** RegAddr */
	u32 nRegAddr;
	/** RegValue */
	u32 nData;
	u32 sa_st;
	u32 sa_end;
	u32 sptagen;
	u32 mtinfoen;
	u32 val;
} GSW_MACSec_cfg_t;

/** \brief MMD Register Access. The 'nData' value is directly written
    to the device register or read from the device. Some PHY
    device registers have standard bit definitions as stated in
    IEEE 802. Used by \ref GSW_MMD_DATA_READ and \ref
    GSW_MMD_DATA_WRITE. */
typedef struct {
	/** Device address on the MDIO interface */
	u8	nAddressDev;
	/** MMD Register address/offset inside the device. */
	u32	nAddressReg;
	/** Exchange data word with the device (read / write). */
	u16	nData;
} GSW_MMD_data_t;

/** \brief Enumeration for function status return. The upper four bits are reserved for error classification */
typedef enum {
	/** Correct or Expected Status */
	GSW_statusOk	= 0,
	/** Invalid function parameter */
	GSW_statusParam	= -2,
	/** No space left in VLAN table */
	GSW_statusVLAN_Space	= -3,
	/** Requested VLAN ID not found in table */
	GSW_statusVLAN_ID	= -4,
	/** Invalid ioctl */
	GSW_statusInvalIoctl	= -5,
	/** Operation not supported by hardware */
	GSW_statusNoSupport	= -6,
	/** Timeout */
	GSW_statusTimeout	= -7,
	/** At least one value is out of range */
	GSW_statusValueRange	= -8,
	/** The PortId/QueueId/MeterId/etc. is not available in this hardware or the
	    selected feature is not available on this port */
	GSW_statusPortInvalid	= -9,
	/** The interrupt is not available in this hardware */
	GSW_statusIRQ_Invalid	= -10,
	/** The MAC table is full, an entry could not be added */
	GSW_statusMAC_TableFull	= -11,
	/** Locking failed - SWAPI is busy */
	GSW_statusLock_Failed	=  -12,
	/** Generic or unknown error occurred */
	GSW_statusErr	= -1
} GSW_return_t;

/** \brief Configures the Wake-on-LAN function.
    Used by \ref GSW_WOL_CFG_SET and \ref GSW_WOL_CFG_GET. */
typedef struct {
	/** WoL MAC address. */
	u8	nWolMAC[GSW_MAC_ADDR_LEN];
	/** WoL password. */
	u8	nWolPassword[GSW_MAC_ADDR_LEN];
	/** WoL password enable. */
	ltq_bool_t	bWolPasswordEnable;
} GSW_WoL_Cfg_t;

/** \brief Enables Wake-on-LAN functionality on the port.
    Used by \ref GSW_WOL_PORT_CFG_SET and \ref GSW_WOL_PORT_CFG_GET. */
typedef struct {
	/** Ethernet Port number (zero-based counting). The valid range is hardware
	    dependent. An error code is delivered if the selected port is not
	    available. */
	u16	nPortId;
	/** Enable Wake-on-LAN. */
	ltq_bool_t	bWakeOnLAN_Enable;
	/** Ignore address check */
	ltq_bool_t	bIgnoreAdrCheck;
} GSW_WoL_PortCfg_t;

/*@}*/ /* GSW_IOCTL_OAM */

/** \addtogroup GSW_IOCTL_RMON */
/*@{*/


/** \brief RMON Counters Type enumeration.
    Used by \ref GSW_RMON_clear_t and \ref GSW_RMON_mode_t. */
typedef enum {
	/** All RMON Types Counters */
	GSW_RMON_ALL_TYPE = 0,
	/** All PMAC RMON Counters */
	GSW_RMON_PMAC_TYPE = 1,
	/** Port based RMON Counters */
	GSW_RMON_PORT_TYPE = 2,
	/** Meter based RMON Counters */
	GSW_RMON_METER_TYPE = 3,
	/** Interface based RMON Counters */
	GSW_RMON_IF_TYPE = 4,
	/** Route based RMON Counters */
	GSW_RMON_ROUTE_TYPE = 5,
	/** Redirected Traffic based RMON Counters */
	GSW_RMON_REDIRECT_TYPE	= 6,
	/** Bridge Port based RMON Counters */
	GSW_RMON_BRIDGE_TYPE	= 7,
	/** CTP Port based RMON Counters */
	GSW_RMON_CTP_TYPE	= 8,
} GSW_RMON_type_t;

/** \brief RMON Counters Data Structure for clearance of values.
    Used by \ref GSW_RMON_CLEAR. */
typedef struct {
	/** RMON Counters Type */
	GSW_RMON_type_t eRmonType;
	/** RMON Counters Identifier - Meter, Port, If, Route, etc. */
	u8 nRmonId;
} GSW_RMON_clear_t;

/**Defined as per RMON counter table structure
  Applicable only for GSWIP 3.1*/
typedef enum {
	GSW_RMON_CTP_PORT_RX = 0,
	GSW_RMON_CTP_PORT_TX = 1,
	GSW_RMON_BRIDGE_PORT_RX = 2,
	GSW_RMON_BRIDGE_PORT_TX = 3,
	GSW_RMON_CTP_PORT_PCE_BYPASS = 4,
	GSW_RMON_TFLOW_RX = 5,
	GSW_RMON_TFLOW_TX = 6,
	GSW_RMON_QMAP = 0x0E,
	GSW_RMON_METER = 0x19,
	GSW_RMON_PMAC = 0x1C,
} GSW_RMON_portType_t;

/* TFLOW counter mode type */
typedef enum {
	/* Global mode */
	GSW_TFLOW_CMODE_GLOBAL = 0,
	/* Logical mode */
	GSW_TFLOW_CMODE_LOGICAL = 1,
	/* CTP port mode */
	GSW_TFLOW_CMODE_CTP = 2,
	/* Bridge port mode */
	GSW_TFLOW_CMODE_BRIDGE = 3,
} GSW_TflowCmodeType_t;

/* TFLOW counter type */
typedef enum {
	/* Set all Rx/Tx/PCE-Bp-Tx registers to same value */
	GSW_TFLOW_COUNTER_ALL = 0, //Default for 'set' function.
	/* SEt PCE Rx register config only */
	GSW_TFLOW_COUNTER_PCE_Rx = 1, //Default for 'get' function.
	/* SEt PCE Tx register config only */
	GSW_TFLOW_COUNTER_PCE_Tx = 2,
	/* SEt PCE-Bypass Tx register config only */
	GSW_TFLOW_COUNTER_PCE_BP_Tx = 3,
} GSW_TflowCountConfType_t;

/* TFLOW CTP counter LSB bits */
typedef enum {
	/* Num of valid bits  */
	GSW_TCM_CTP_VAL_BITS_0 = 0,
	GSW_TCM_CTP_VAL_BITS_1 = 1,
	GSW_TCM_CTP_VAL_BITS_2 = 2,
	GSW_TCM_CTP_VAL_BITS_3 = 3,
	GSW_TCM_CTP_VAL_BITS_4 = 4,
	GSW_TCM_CTP_VAL_BITS_5 = 5,
	GSW_TCM_CTP_VAL_BITS_6 = 6,
} GSW_TflowCtpValBits_t;

/* TFLOW bridge port counter LSB bits */
typedef enum {
	/* Num of valid bits  */
	GSW_TCM_BRP_VAL_BITS_2 = 2,
	GSW_TCM_BRP_VAL_BITS_3 = 3,
	GSW_TCM_BRP_VAL_BITS_4 = 4,
	GSW_TCM_BRP_VAL_BITS_5 = 5,
	GSW_TCM_BRP_VAL_BITS_6 = 6,
} GSW_TflowBrpValBits_t;

/**
 \brief RMON Counters for individual Port.
 This structure contains the RMON counters of an Ethernet Switch Port.
    Used by \ref GSW_RMON_PORT_GET. */
typedef struct {
	/** Port Type. This gives information which type of port to get RMON.
	    nPortId should be based on this field.
	    This is new in GSWIP-3.1. For GSWIP-2.1/2.2/3.0, this field is always
	    ZERO (GSW_LOGICAL_PORT). */
	GSW_portType_t ePortType;
	/** Ethernet Port number (zero-based counting). The valid range is hardware
	    dependent. An error code is delivered if the selected port is not
	    available. This parameter specifies for which MAC port the RMON
	    counter is read. It has to be set by the application before
	    calling \ref GSW_RMON_PORT_GET. */
	u16	nPortId;
	/** Sub interface ID group. The valid range is hardware/protocol dependent.

	    \remarks
	    This field is valid when \ref GSW_RMON_Port_cnt_t::ePortType is
	    \ref GSW_portType_t::GSW_CTP_PORT.
	    Sub interface ID group is defined for each of \ref GSW_LogicalPortMode_t.
	    For both \ref GSW_LOGICAL_PORT_8BIT_WLAN and
	    \ref GSW_LOGICAL_PORT_9BIT_WLAN, this field is VAP.
	    For \ref GSW_LOGICAL_PORT_GPON, this field is GEM index.
	    For \ref GSW_LOGICAL_PORT_EPON, this field is stream index.
	    For \ref GSW_LOGICAL_PORT_GINT, this field is LLID.
	    For others, this field is 0. */
	u32 nSubIfIdGroup;
	/** Separate set of CTP Tx counters when PCE is bypassed. GSWIP-3.1 only.*/
	ltq_bool_t bPceBypass;
	/*Applicable only for GSWIP 3.1*/
	/** Discarded at Extended VLAN Operation Packet Count. GSWIP-3.1 only. */
	u32	nRxExtendedVlanDiscardPkts;
	/** Discarded MTU Exceeded Packet Count. GSWIP-3.1 only. */
	u32	nMtuExceedDiscardPkts;
	/** Tx Undersize (<64) Packet Count. GSWIP-3.1 only. */
	u32 nTxUnderSizeGoodPkts;
	/** Tx Oversize (>1518) Packet Count. GSWIP-3.1 only. */
	u32 nTxOversizeGoodPkts;
	/** Receive Packet Count (only packets that are accepted and not discarded). */
	u32	nRxGoodPkts;
	/** Receive Unicast Packet Count. */
	u32	nRxUnicastPkts;
	/** Receive Broadcast Packet Count. */
	u32	nRxBroadcastPkts;
	/** Receive Multicast Packet Count. */
	u32	nRxMulticastPkts;
	/** Receive FCS Error Packet Count. */
	u32	nRxFCSErrorPkts;
	/** Receive Undersize Good Packet Count. */
	u32	nRxUnderSizeGoodPkts;
	/** Receive Oversize Good Packet Count. */
	u32	nRxOversizeGoodPkts;
	/** Receive Undersize Error Packet Count. */
	u32	nRxUnderSizeErrorPkts;
	/** Receive Good Pause Packet Count. */
	u32	nRxGoodPausePkts;
	/** Receive Oversize Error Packet Count. */
	u32	nRxOversizeErrorPkts;
	/** Receive Align Error Packet Count. */
	u32	nRxAlignErrorPkts;
	/** Filtered Packet Count. */
	u32	nRxFilteredPkts;
	/** Receive Size 64 Bytes Packet Count. */
	u32	nRx64BytePkts;
	/** Receive Size 65-127 Bytes Packet Count. */
	u32	nRx127BytePkts;
	/** Receive Size 128-255 Bytes Packet Count. */
	u32	nRx255BytePkts;
	/** Receive Size 256-511 Bytes Packet Count. */
	u32	nRx511BytePkts;
	/** Receive Size 512-1023 Bytes Packet Count. */
	u32	nRx1023BytePkts;
	/** Receive Size 1024-1522 Bytes (or more, if configured) Packet Count. */
	u32	nRxMaxBytePkts;
	/** Overall Transmit Good Packets Count. */
	u32	nTxGoodPkts;
	/** Transmit Unicast Packet Count. */
	u32	nTxUnicastPkts;
	/** Transmit Broadcast Packet Count. */
	u32	nTxBroadcastPkts;
	/** Transmit Multicast Packet Count. */
	u32	nTxMulticastPkts;
	/** Transmit Single Collision Count. */
	u32	nTxSingleCollCount;
	/** Transmit Multiple Collision Count. */
	u32	nTxMultCollCount;
	/** Transmit Late Collision Count. */
	u32	nTxLateCollCount;
	/** Transmit Excessive Collision Count. */
	u32	nTxExcessCollCount;
	/** Transmit Collision Count. */
	u32	nTxCollCount;
	/** Transmit Pause Packet Count. */
	u32	nTxPauseCount;
	/** Transmit Size 64 Bytes Packet Count. */
	u32	nTx64BytePkts;
	/** Transmit Size 65-127 Bytes Packet Count. */
	u32	nTx127BytePkts;
	/** Transmit Size 128-255 Bytes Packet Count. */
	u32	nTx255BytePkts;
	/** Transmit Size 256-511 Bytes Packet Count. */
	u32	nTx511BytePkts;
	/** Transmit Size 512-1023 Bytes Packet Count. */
	u32	nTx1023BytePkts;
	/** Transmit Size 1024-1522 Bytes (or more, if configured) Packet Count. */
	u32	nTxMaxBytePkts;
	/** Transmit Drop Packet Count. */
	u32	nTxDroppedPkts;
	/** Transmit Dropped Packet Count, based on Congestion Management. */
	u32	nTxAcmDroppedPkts;
	/** Receive Dropped Packet Count. */
	u32	nRxDroppedPkts;
	/** Receive Good Byte Count (64 bit). */
	u64	nRxGoodBytes;
	/** Receive Bad Byte Count (64 bit). */
	u64	nRxBadBytes;
	/** Transmit Good Byte Count (64 bit). */
	u64	nTxGoodBytes;
} GSW_RMON_Port_cnt_t;

/** \brief RMON Counters Mode Enumeration.
    This enumeration defines Counters mode - Packets based or Bytes based counting.
    Metering and Routing Sessions RMON counting support either Byte based or packets based only. */
typedef enum {
	/** Packet based RMON Counters */
	GSW_RMON_COUNT_PKTS	= 0,
	/** Bytes based RMON Counters */
	GSW_RMON_COUNT_BYTES	= 1,
	/**  number of dropped frames, supported only for interface cunters */
	GSW_RMON_DROP_COUNT	= 2,
} GSW_RMON_CountMode_t;

/** \brief RMON Counters Mode for different Elements.
    This structure takes RMON Counter Element Name and mode config */
typedef struct {
	/** RMON Counters Type */
	GSW_RMON_type_t eRmonType;
	/** RMON Counters Mode */
	GSW_RMON_CountMode_t eCountMode;
} GSW_RMON_mode_t;

/**
 \brief RMON Counters for Meter - Type (GSWIP-3.0 only).
 This structure contains the RMON counters of one Meter Instance.
    Used by \ref GSW_RMON_METER_GET. */
typedef struct {
	/** Meter Instance number (zero-based counting). The valid range is hardware
	    dependent. An error code is delivered if the selected meter is not
	    available. This parameter specifies for which Meter Id the RMON-1
	    counter is read. It has to be set by the application before
	    calling \ref GSW_RMON_METER_GET. */
	u8	nMeterId;
	/** Metered Green colored packets or bytes (depending upon mode) count. */
	u32	nGreenCount;
	/** Metered Yellow colored packets or bytes (depending upon mode) count. */
	u32	nYellowCount;
	/** Metered Red colored packets or bytes (depending upon mode) count. */
	u32	nRedCount;
	/** Metered Reserved (Future Use) packets or bytes (depending upon mode) count. */
	u32	nResCount;
} GSW_RMON_Meter_cnt_t;

/**
 \brief RMON Redirected Counters for  (GSWIP-3.0 only).
 This structure contains the RMON counters of all Redirected packets.
    Used by \ref GSW_RMON_REDIRECT_GET. */
typedef struct {
	/** Received Total Packets count. */
	u32	nRxPktsCount;
	/** Received Discarded Packets count. */
	u32	nRxDiscPktsCount;
	/** Received Bytes count excluding discards. */
	u64	nRxBytesCount;
	/** Transmitted Total Packets count. */
	u32	nTxPktsCount;
	/** Egress Queue Discarded Packets count. */
	u32	nTxDiscPktsCount;
	/** Transmitted Bytes count excluding discards. */
	u64	nTxBytesCount;
} GSW_RMON_Redirect_cnt_t;

/**
 \brief RMON Interface Counters for  (GSWIP-3.0 only).
 This structure contains the RMON counters of Interface Instance.
    Used by \ref GSW_RMON_IF_GET. */
typedef struct {
	/** Interface Counter Index Number. (Range : 0-255)
	    It has to be reserved for ports by the application before
	    calling \ref GSW_RMON_IF_GET. */
	u8	nIfId;
	/** Counters Mode - Global level current settings. */
	GSW_RMON_CountMode_t  countMode;
	/** Received Total Packets count. */
	u32	nRxPktsCount;
	/** Received Discarded Packets count */
	u32	nRxDiscPktsCount;
	/** Received Bytes count - When this is used, Discarded Count is not available*/
	u32	nRxBytesCount;
	/** Transmitted Total Packets count. */
	u32	nTxPktsCount;
	/** Egress Discarded Packets count. */
	u32	nTxDiscPktsCount;
	/** Transmitted Bytes count - When this is used, Discarded Count is not available */
	u32	nTxBytesCount;
} GSW_RMON_If_cnt_t;

/**
 \brief RMON Routing Counters for  (GSWIP-3.0 only).
 This structure contains the RMON counters of Routing Port number.
    Used by \ref GSW_RMON_ROUTE_GET. */
typedef struct {
	/** Routed Port Number. */
	u8	nRoutedPortId;
	/** Fast Path unicast IPv4 Total UDP Packets Received Count. */
	u32	nRxUCv4UDPPktsCount;
	/** Fast Path unicast IPv4 Total TCP Packets Received Count. */
	u32	nRxUCv4TCPPktsCount;
	/** Fast Path unicast IPv6 Total UDP Packets Received Count. */
	u32	nRxUCv6UDPPktsCount;
	/** Fast Path unicast IPv6 Total TCP Packets Received Count. */
	u32	nRxUCv6TCPPktsCount;
	/** Fast Path multicast IPv4 packets Received Count. */
	u32	nRxMCv4PktsCount;
	/** Fast Path multicast IPv6 packets Received Count. */
	u32	nRxMCv6PktsCount;
	/** CPU Path Received Total Packets Count. */
	u32	nRxCpuPktsCount;
	/** Fast Path Dropped Total Packets Count. */
	u32	nRxPktsDropCount;
	/** Fast Path IPv4 Traffic Total Bytes Received Count */
	u32	nRxIPv4BytesCount;
	/** Fast Path IPv6 Traffic Total Bytes Received Count */
	u32	nRxIPv6BytesCount;
	/** CPU Path Traffic Total Bytes Received Count */
	u32	nRxCpuBytesCount;
	/** Fast Path Traffic Total Bytes Dropped Count */
	u32	nRxBytesDropCount;
	/** Fast Path Traffic Total Packets Transmitted Count */
	u32	nTxPktsCount;
	/** Fast Path Traffic Total Bytes Transmitted Count */
	u32	nTxBytesCount;
} GSW_RMON_Route_cnt_t;

/** Defined as per RMON counter table structure
 * Applicable only for GSWIP 3.0
 */
typedef enum {
	GSW_RMON_REDIRECTION = 0X18,
} GSW_RMON_Port_t;

/*@}*/ /* GSW_IOCTL_RMON */

/** \addtogroup GSW_IOCTL_PMAC */
/*@{*/
/** \brief Configure the Backpressure mapping for egress Queues Congestion or ingress (receiving)  ports to DMA channel.
    Used by \ref GSW_PMAC_BM_CFG_SET and \ref GSW_PMAC_BM_CFG_GET. */
typedef struct {
	/** PMAC Interface ID */
	u8 nPmacId;
	/**  Tx DMA Channel Identifier which receives sideband backpressure signal (0..15) */
	u8 nTxDmaChanId;
	/** Transmit Queues Selection Mask which will generate backpressure - (Configurable upto 32 Egress Queues) */
	u32 txQMask;
	/** Receive (Ingress) ports selection congestion Mask which will generate backpressure - (Configurable upto 16 ports) */
	u32 rxPortMask;
} GSW_PMAC_BM_Cfg_t;


/** \brief Short Length Received Frame Check Type for PMAC.
    Used by PMAC structure \ref GSW_PMAC_Glbl_Cfg_t. */
typedef enum {
	/** Short frame length check is disabled. */
	GSW_PMAC_SHORT_LEN_DIS = 0,
	/** Short frame length check is enabled without considering VLAN Tags. */
	GSW_PMAC_SHORT_LEN_ENA_UNTAG = 1,
	/** Short frame length check is enabled including VLAN Tags. */
	GSW_PMAC_SHORT_LEN_ENA_TAG = 2,
	/** Reserved - Currently unused */
	GSW_PMAC_SHORT_LEN_RESERVED = 3
} GSW_PMAC_Short_Frame_Chk_t;

/** \brief Egress PMAC Config Table Selector */
typedef enum {
	/** Use value of \ref GSW_PMAC_Glbl_Cfg_t::bProcFlagsEgCfgEna */
	GSW_PMAC_PROC_FLAGS_NONE = 0,
	/** Use traffic class for egress config table addressing */
	GSW_PMAC_PROC_FLAGS_TC = 1,
	/** Use flags (MPE1, MPE2, DEC, ENC) for egress config table addressing */
	GSW_PMAC_PROC_FLAGS_FLAG = 2,
	/** Use reduced traffic class (saturated to 3) and flags (MPE1, MPE2) for
	    egress config table addressing */
	GSW_PMAC_PROC_FLAGS_MIX = 3
} GSW_PMAC_Proc_Flags_Eg_Cfg_t;

/** \brief Configure the global settings of PMAC for GSWIP-3.x. This includes settings such as Jumbo frame, Checksum handling,
    Padding and Engress PMAC Selector Config.
    Used by \ref GSW_PMAC_GLBL_CFG_SET and \ref GSW_PMAC_GLBL_CFG_GET. */
typedef struct {
	/** PMAC Interface Id */
	u8 nPmacId;
	/**  Automatic Padding Settings - Disabled (Default), to enable set it true. */
	ltq_bool_t bAPadEna;
	/**  Global Padding Settings - Disabled (Default), to enable set it true. */
	ltq_bool_t bPadEna;
	/**  VLAN Padding Setting - Disabled (Default), to enable set it true - applicable when bPadEna is set. */
	ltq_bool_t bVPadEna;
	/**  Stacked VLAN Padding Setting - Disabled (Default), to enable set it true - applicable when bPadEna is set. */
	ltq_bool_t bSVPadEna;
	/**  Packet carry FCS after PMAC process - Enabled (Default), to disable set it true.
	     For GSWIP-3.1 only. */
	ltq_bool_t bRxFCSDis;
	/**  Transmit FCS Regeneration Setting - Enabled (Default), to disable set it true. */
	ltq_bool_t bTxFCSDis;
	/**   IP and Transport (TCP/UDP) Headers Checksum Generation Control - Enabled (Default), to disable set it true. */
	ltq_bool_t bIPTransChkRegDis;
	/**   IP and Transport (TCP/UDP) Headers Checksum Verification Control - Enabled (Default), to disable set it true. */
	ltq_bool_t bIPTransChkVerDis;
	/**   To enable receipt of Jumbo frames - Disabled (Default - 1518 bytes normal frames without VLAN tags), to enable Jumbo set it true. */
	ltq_bool_t bJumboEna;
	/**   Maximum length of Jumbo frames in terms of bytes (Bits 13:0). The maximum handled in Switch is 9990 bytes.  */
	u16 nMaxJumboLen;
	/**   Threshold length for Jumbo frames qualification in terms of bytes (Bits 13:0).  */
	u16 nJumboThreshLen;
	/**   Long frame length check - Enabled (Default), to disable set it true.  */
	ltq_bool_t bLongFrmChkDis;
	/**   Short frame length check Type - default (Enabled for 64 bytes without considering VLAN).  */
	GSW_PMAC_Short_Frame_Chk_t eShortFrmChkType;
	/** GSWIP3.0 specific - Egress PMAC Config Table Selector - TrafficClass or Processing Flags (MPE1, MPE22, DEC, ENC based).
	    The default setting is Traffic Class based selector for Egress PMAC. */
	ltq_bool_t	bProcFlagsEgCfgEna;
	/** GSWIP3.1 specific - Egress PMAC Config Table Selector
	    If this field is not \ref GSW_PMAC_PROC_FLAGS_NONE, it will override
	    bProcFlagsEgCfgEna. */
	GSW_PMAC_Proc_Flags_Eg_Cfg_t eProcFlagsEgCfg;
	/** GSWIP3.1 specific - frame size threshold for buffer selection.
	    Value in this array should be in ascending order. */
	u32 nBslThreshold[3];
} GSW_PMAC_Glbl_Cfg_t;

/** \brief PMAC Ingress Configuration Source
    Source of the corresponding field. */
typedef enum {
	/** Field is from DMA descriptor */
	GSW_PMAC_IG_CFG_SRC_DMA_DESC = 0,
	/** Field is from default PMAC header */
	GSW_PMAC_IG_CFG_SRC_DEF_PMAC = 1,
	/** Field is from PMAC header of packet */
	GSW_PMAC_IG_CFG_SRC_PMAC = 2,
} GSW_PMAC_Ig_Cfg_Src_t;

/** \brief Configure the PMAC Ingress Configuration on a given Tx DMA channel to PMAC. (Upto 16 entries).
    This Ingress PMAC table is addressed through Trasnmit DMA Channel Identifier.
    Used by \ref GSW_PMAC_IG_CFG_SET and \ref GSW_PMAC_IG_CFG_GET. */
typedef struct {
	/** PMAC Interface Id */
	u8 nPmacId;
	/**  Tx DMA Channel Identifier (0..16) - Index of Ingress PMAC Config Table */
	u8 nTxDmaChanId;
	/** Error set packets to be discarded (True) or not (False) */
	ltq_bool_t bErrPktsDisc;
	/** Port Map info from default PMAC header (True) or incoming PMAC header (False) */
	ltq_bool_t bPmapDefault;
	/** Port Map Enable info from default PMAC header (True) or incoming PMAC header (False) */
	ltq_bool_t bPmapEna;
	/** Class Info from default PMAC header (True) or incoming PMAC header (False) */
	ltq_bool_t bClassDefault;
	/** Class Enable info from default PMAC header (True) or incoming PMAC header (False) */
	ltq_bool_t bClassEna;
	/** Sub_Interface Id Info from ingress PMAC header (GSW_PMAC_IG_CFG_SRC_PMAC),
	    default PMAC header (GSW_PMAC_IG_CFG_SRC_DEF_PMAC), or source sub-If in
	    packet descriptor (GSW_PMAC_IG_CFG_SRC_DMA_DESC) */
	GSW_PMAC_Ig_Cfg_Src_t eSubId;
	/**  Source Port Id from default PMAC header (True) or incoming PMAC header (False) */
	ltq_bool_t bSpIdDefault;
	/** Packet PMAC header is present (True) or not (False) */
	ltq_bool_t bPmacPresent;
	/** Default PMAC header - 8 Bytes Configuration  - Ingress PMAC Header Format */
	u8 defPmacHdr[8];
} GSW_PMAC_Ig_Cfg_t;

/** \brief Configure the PMAC Egress Configuration. (Upto 1024 entries)
    This Egress PMAC table is addressed through combination of following fields (Bit0 - Bit 9).
     nDestPortId (Bits 0-3) + Combination of [bMpe1Flag (Bit 4) + bMpe2Flag (Bit 5) + bEncFlag (Bit 6) + bDecFlag (Bit 7) ] or TrafficClass Value (Bits 4-7) + nFlowIdMSB (Bits 8-9).
    The bits 4-7 of index option is either based upon TC (default) or combination of Processing flags is decided through bProcFlagsEgPMACEna.
    It is expected to pass the correct value in bProcFlagsSelect same as global bProcFlagsEgPMACEna;
    Used by \ref GSW_PMAC_EG_CFG_SET and \ref GSW_PMAC_EG_CFG_GET. */
typedef struct {
	/** PMAC Interface ID */
	u8 nPmacId;
	/**  Destination Port Identifier (0..15) - Part of Table Index (Bits 0-3)*/
	u8 nDestPortId;
	/** Traffic Class value [Lower 4 -bits (LSB-0, 1, 2, 3)]. - Part of Table Index Bits 4-7.
	    This value is considered, only when bProcFlagsSelect is not set */
	u8 nTrafficClass;
	/**  MPE-1 Flag value - Part of Table Index Bit 4. Valid only when bProcFlagsSelect is set. */
	ltq_bool_t bMpe1Flag;
	/**  MPE-2 Flag value - Part of Table Index Bit 5. Valid only when bProcFlagsSelect is set. */
	ltq_bool_t bMpe2Flag;
	/**  Cryptography Decryption Action Flag value - Part of Table Index Bit 6. Valid only, when bProcFlagsSelect is set. */
	ltq_bool_t bDecFlag;
	/**  Cryptography Encryption Action Flag value - Part of Table Index Bit 7. Valid only, when bProcFlagsSelect is set. */
	ltq_bool_t bEncFlag;
	/**  Flow-ID MSB (2-bits) value -  valid range (0..2). - Part of Table Index Bits 8-9. */
	u8 nFlowIDMsb;
	/** Selector for Processing Flags (MPE1, MPE2, DEC & ENC bits). If enabled, then the combination of flags bDecFlag, bEncFlag, bMpe1Flag and  bMpe2Flag are considered as index instead of nTrafficClass. For using these combination flags, turn ON this boolean selector.
	TC or combination processing flag is decided at global level through bProcFlagsEgPMACEna.
	It is expected that user always passes correct value based upon bProcFlagsEgMPACEna. If mismatch found with global PMAC mode, SWAPI will return error code.

	    \remarks
	    In GSWIP-3.1, this is ignored and driver will determine automatically by
	    reading register.
	*/
	ltq_bool_t bProcFlagsSelect;
	/**  Receive DMA Channel Identifier (0..15) */
	u8 nRxDmaChanId;
	/** To remove L2 header & additional bytes (True) or Not (False) */
	ltq_bool_t bRemL2Hdr;
	/** No. of bytes to be removed after Layer-2 Header, valid when bRemL2Hdr is set */
	u8 numBytesRem;
	/** Packet egressing will have FCS (True) or Not (False) */
	ltq_bool_t bFcsEna;
	/** Packet egressing will have PMAC (True) or Not (False) */
	ltq_bool_t bPmacEna;
	/** Enable redirection flag. GSWIP-3.1 only.
	    Overwritten by bRes1DW0Enable and nRes1DW0. */
	ltq_bool_t bRedirEnable;
	/** Allow (False) or not allow (True) segmentation during buffer selection.
	    GSWIP-3.1 only. Overwritten by bResDW1Enable and nResDW1. */
	ltq_bool_t bBslSegmentDisable;
	/** Traffic class used for buffer selection. GSWIP-3.1 only.
	    Overwritten by bResDW1Enable and nResDW1. */
	u8 nBslTrafficClass;
	/** If false, nResDW1 is ignored. */
	ltq_bool_t bResDW1Enable;
	/** 4-bits Reserved Field in DMA Descriptor - DW1 (bit 7 to 4) - for any future/custom usage. (Valid range : 0-15) */
	u8 nResDW1;
	/** If false, nRes1DW0 is ignored. */
	ltq_bool_t bRes1DW0Enable;
	/** 3-bits Reserved Field in DMA Descriptor - DW0 (bit 31 to 29) - for any future/custom usage. (Valid range : 0-7) */
	u8 nRes1DW0;
	/** If false, nRes2DW0 is ignored. */
	ltq_bool_t bRes2DW0Enable;
	/** 2-bits Reserved Field in DMA Descriptor - DW0 (bit 14 to 13) - for any future/custom usage. (Valid range : 0-2) */
	u8 nRes2DW0;
	/** Selector for TrafficClass bits. If enabled, then the flags
	bDecFlag, bEncFlag, bMpe1Flag and  bMpe2Flag are not used instead nTrafficClass parameter is used. For using these flags turn off this boolean */
	ltq_bool_t bTCEnable;
} GSW_PMAC_Eg_Cfg_t;

/** \brief PMAC Counters available for specified DMA Channel.
    Used by \ref GSW_PMAC_COUNT_GET. */
typedef struct {
	/** PMAC Interface ID Applicable only for GSWIP 3.1 */
	u8 nPmacId;
	/*Applicable only for GSWIP 3.1*/
	ltq_bool_t  b64BitMode;
	/**  Transmit DMA Channel Identifier (0..15) for GSWIP3.0  (0..16) for GSWIP3.1 Source PortId for Egress Counters (0..15) for GSWIP3.1 - Index */
	u8 nTxDmaChanId;
	/** Ingress Total discarded packets counter (32-bits) */
	u32 nDiscPktsCount;
	/** Ingress Total discarded bytes counter (32-bits) */
	u32 nDiscBytesCount;
	/** Egress Total TCP/UDP/IP checksum error-ed packets counter (32-bits) */
	u32 nChkSumErrPktsCount;
	/** Egress Total TCP/UDP/IP checksum error-ed bytes counter (32-bits) */
	u32 nChkSumErrBytesCount;
	/** Total Ingress Packet Count in Applicable only for GSWIP 3.1 (32-bits) */
	u32 nIngressPktsCount;
	/** Total Ingress Bytes Count in Applicable only for GSWIP 3.1 (32-bits) */
	u32 nIngressBytesCount;
	/** Total Engress Packet Count in Applicable only for GSWIP 3.1 (32-bits) */
	u32 nEgressPktsCount;
	/** Total Engress Bytes Count in Applicable only for GSWIP 3.1 (32-bits) */
	u32 nEgressBytesCount;
	/** Ingress header Packet Count Applicable only for GSWIP 3.2 (32-bits) */
	u32 nIngressHdrPktsCount;
	/** Ingress header Byte Count Applicable only for GSWIP 3.2 (32-bits) */
	u32 nIngressHdrBytesCount;
	/** Egress header Packet Count Applicable only for GSWIP 3.2 (32-bits) */
	u32 nEgressHdrPktsCount;
	/** Egress header Byte Count Applicable only for GSWIP 3.2 (32-bits) */
	u32 nEgressHdrBytesCount;
	/** Egress header Discard Packet Count Applicable only for GSWIP 3.2 (32-bits) */
	u32 nEgressHdrDiscPktsCount;
	/** Egress header Discard Byte Count Applicable only for GSWIP 3.2 (32-bits) */
	u32 nEgressHdrDiscBytesCount;
} GSW_PMAC_Cnt_t;



/** \brief For debugging Purpose only.
    Used for GSWIP 3.1. */
typedef struct {
	/** Table Index to get status of the Table Index only for GSWIP 3.1 */
	u32 nTableIndex;
	/** Force table Index In USEonly for GSWIP 3.1 */
	u8 nForceSet;
	/** To check dispaly index which are In USE for GSWIP 3.1 */
	u8 nCheckIndexInUse;
	/** Vlan Filter or Exvlan BlockID*/
	u32 nblockid;
	/** Vlan Filter debugging usage*/
	u8 nDiscardUntagged;
	/** Vlan Filter debugging usage*/
	u8 nDiscardUnMatchedTagged;
	/** Pmac debugging purpose*/
	u8 nPmacId;
	/** Pmac debugging purpose*/
	u8 nDestPort;
} GSW_debug_t;

/** \brief For debugging Purpose only.
    Used for GSWIP 3.1. */

typedef struct {
	/** Ethernet Port number (zero-based counting). The valid range is hardware
	    dependent. An error code is delivered if the selected port is not
	    available. This parameter specifies for which MAC port the RMON
	    counter is read. It has to be set by the application before
	    calling \ref GSW_RMON_PORT_GET. */
	u16	nPortId;
	/**Table address selection based on port type
	  Applicable only for GSWIP 3.1
	 \ref GSW_RMON_portType_t**/
	GSW_RMON_portType_t ePortType;
	/*Applicable only for GSWIP 3.1*/
	ltq_bool_t  b64BitMode;
	/*Applicable only for GSWIP 3.1*/
	u32	nRxExtendedVlanDiscardPkts;
	/*Applicable only for GSWIP 3.1*/
	u32	nMtuExceedDiscardPkts;
	/*Applicable only for GSWIP 3.1*/
	u32 nTxUnderSizeGoodPkts;
	/*Applicable only for GSWIP 3.1*/
	u32 nTxOversizeGoodPkts;
	/** Receive Packet Count (only packets that are accepted and not discarded). */
	u32	nRxGoodPkts;
	/** Receive Unicast Packet Count. */
	u32	nRxUnicastPkts;
	/** Receive Broadcast Packet Count. */
	u32	nRxBroadcastPkts;
	/** Receive Multicast Packet Count. */
	u32	nRxMulticastPkts;
	/** Receive FCS Error Packet Count. */
	u32	nRxFCSErrorPkts;
	/** Receive Undersize Good Packet Count. */
	u32	nRxUnderSizeGoodPkts;
	/** Receive Oversize Good Packet Count. */
	u32	nRxOversizeGoodPkts;
	/** Receive Undersize Error Packet Count. */
	u32	nRxUnderSizeErrorPkts;
	/** Receive Good Pause Packet Count. */
	u32	nRxGoodPausePkts;
	/** Receive Oversize Error Packet Count. */
	u32	nRxOversizeErrorPkts;
	/** Receive Align Error Packet Count. */
	u32	nRxAlignErrorPkts;
	/** Filtered Packet Count. */
	u32	nRxFilteredPkts;
	/** Receive Size 64 Bytes Packet Count. */
	u32	nRx64BytePkts;
	/** Receive Size 65-127 Bytes Packet Count. */
	u32	nRx127BytePkts;
	/** Receive Size 128-255 Bytes Packet Count. */
	u32	nRx255BytePkts;
	/** Receive Size 256-511 Bytes Packet Count. */
	u32	nRx511BytePkts;
	/** Receive Size 512-1023 Bytes Packet Count. */
	u32	nRx1023BytePkts;
	/** Receive Size 1024-1522 Bytes (or more, if configured) Packet Count. */
	u32	nRxMaxBytePkts;
	/** Overall Transmit Good Packets Count. */
	u32	nTxGoodPkts;
	/** Transmit Unicast Packet Count. */
	u32	nTxUnicastPkts;
	/** Transmit Broadcast Packet Count. */
	u32	nTxBroadcastPkts;
	/** Transmit Multicast Packet Count. */
	u32	nTxMulticastPkts;
	/** Transmit Single Collision Count. */
	u32	nTxSingleCollCount;
	/** Transmit Multiple Collision Count. */
	u32	nTxMultCollCount;
	/** Transmit Late Collision Count. */
	u32	nTxLateCollCount;
	/** Transmit Excessive Collision Count. */
	u32	nTxExcessCollCount;
	/** Transmit Collision Count. */
	u32	nTxCollCount;
	/** Transmit Pause Packet Count. */
	u32	nTxPauseCount;
	/** Transmit Size 64 Bytes Packet Count. */
	u32	nTx64BytePkts;
	/** Transmit Size 65-127 Bytes Packet Count. */
	u32	nTx127BytePkts;
	/** Transmit Size 128-255 Bytes Packet Count. */
	u32	nTx255BytePkts;
	/** Transmit Size 256-511 Bytes Packet Count. */
	u32	nTx511BytePkts;
	/** Transmit Size 512-1023 Bytes Packet Count. */
	u32	nTx1023BytePkts;
	/** Transmit Size 1024-1522 Bytes (or more, if configured) Packet Count. */
	u32	nTxMaxBytePkts;
	/** Transmit Drop Packet Count. */
	u32	nTxDroppedPkts;
	/** Transmit Dropped Packet Count, based on Congestion Management. */
	u32	nTxAcmDroppedPkts;
	/** Receive Dropped Packet Count. */
	u32	nRxDroppedPkts;
	/** Receive Good Byte Count (64 bit). */
	u64	nRxGoodBytes;
	/** Receive Bad Byte Count (64 bit). */
	u64	nRxBadBytes;
	/** Transmit Good Byte Count (64 bit). */
	u64	nTxGoodBytes;
	/**For GSWIP V32 only **/
	/** Receive Unicast Packet Count for Yellow & Red packet. */
	u32	nRxUnicastPktsYellowRed;
	/** Receive Broadcast Packet Count for Yellow & Red packet. */
	u32	nRxBroadcastPktsYellowRed;
	/** Receive Multicast Packet Count for Yellow & Red packet. */
	u32	nRxMulticastPktsYellowRed;
	/** Receive Good Byte Count (64 bit) for Yellow & Red packet. */
	u64 nRxGoodBytesYellowRed;
	/** Receive Packet Count for Yellow & Red packet.  */
	u32 nRxGoodPktsYellowRed;
	/** Transmit Unicast Packet Count for Yellow & Red packet. */
	u32	nTxUnicastPktsYellowRed;
	/** Transmit Broadcast Packet Count for Yellow & Red packet. */
	u32	nTxBroadcastPktsYellowRed;
	/** Transmit Multicast Packet Count for Yellow & Red packet. */
	u32	nTxMulticastPktsYellowRed;
	/** Transmit Good Byte Count (64 bit) for Yellow & Red packet. */
	u64 nTxGoodBytesYellowRed;
	/** Transmit Packet Count for Yellow & Red packet.  */
	u32 nTxGoodPktsYellowRed;
} GSW_Debug_RMON_Port_cnt_t;




#if 0
/** \brief PMAC Egress Counters available for every egress PMAC port.
    Used by \ref GSW_PMAC_EG_COUNT_GET. */
typedef struct {
	/**  Transmitting Port Identifier (0..15) */
	u8 nTxPortId;
	/** Total TCP/UDP/IP checksum error-ed packets counter (32-bits) */
	u32 nChkSumErrPktsCount;
	/** Total TCP/UDP/IP checksum error-ed bytes counter (32-bits) */
	u32 nChkSumErrBytesCount;
} GSW_PMAC_Eg_Cnt_t;
#endif
/*@}*/ /* GSW_IOCTL_PMAC */

/** \addtogroup GSW_IOCTL_GSWIP31 */
/*@{*/
/** \brief Invalid Value.
    Used mainly during resource allocation to indicate that resource is not
    allocated. */
#define INVALID_HANDLE  (~0)

/** \brief Extended VLAN Filter Type.
    Used by \ref GSW_EXTENDEDVLAN_filterVLAN_t. */
typedef enum {
	/** There is tag and criteria applies. */
	GSW_EXTENDEDVLAN_FILTER_TYPE_NORMAL = 0,
	/** There is tag but no criteria. */
	GSW_EXTENDEDVLAN_FILTER_TYPE_NO_FILTER = 1,
	/** Default entry if no other rule applies. */
	GSW_EXTENDEDVLAN_FILTER_TYPE_DEFAULT = 2,
	/** There is no tag. */
	GSW_EXTENDEDVLAN_FILTER_TYPE_NO_TAG = 3,
	/** Block invalid*/
	GSW_EXTENDEDVLAN_BLOCK_INVALID = 4
} GSW_ExtendedVlanFilterType_t;

/** \brief Extended VLAN Filter TPID Field.
    Used by \ref GSW_EXTENDEDVLAN_filterVLAN_t. */
typedef enum {
	/** Do not filter. */
	GSW_EXTENDEDVLAN_FILTER_TPID_NO_FILTER = 0,
	/** TPID is 0x8100. */
	GSW_EXTENDEDVLAN_FILTER_TPID_8021Q = 1,
	/** TPID is global configured value. */
	GSW_EXTENDEDVLAN_FILTER_TPID_VTETYPE = 2
} GSW_ExtendedVlanFilterTpid_t;

typedef enum {
	GSW_EXTENDEDVLAN_TPID_VTETYPE_1 = 0,
	GSW_EXTENDEDVLAN_TPID_VTETYPE_2 = 1,
	GSW_EXTENDEDVLAN_TPID_VTETYPE_3 = 2,
	GSW_EXTENDEDVLAN_TPID_VTETYPE_4 = 3
} GSW_ExtendedVlan_4_Tpid_Mode_t;

/** \brief Extended VLAN Filter DEI Field.
    Used by \ref GSW_EXTENDEDVLAN_filterVLAN_t. */
typedef enum {
	/** Do not filter. */
	GSW_EXTENDEDVLAN_FILTER_DEI_NO_FILTER = 0,
	/** DEI is 0. */
	GSW_EXTENDEDVLAN_FILTER_DEI_0 = 1,
	/** DEI is 1. */
	GSW_EXTENDEDVLAN_FILTER_DEI_1 = 2
} GSW_ExtendedVlanFilterDei_t;

/** \brief Extended VLAN Filter EtherType.
    Used by \ref GSW_EXTENDEDVLAN_filterVLAN_t. */
typedef enum {
	/** Do not filter. */
	GSW_EXTENDEDVLAN_FILTER_ETHERTYPE_NO_FILTER = 0,
	/** IPoE frame (Ethertyp is 0x0800). */
	GSW_EXTENDEDVLAN_FILTER_ETHERTYPE_IPOE = 1,
	/** PPPoE frame (Ethertyp is 0x8863 or 0x8864). */
	GSW_EXTENDEDVLAN_FILTER_ETHERTYPE_PPPOE = 2,
	/** ARP frame (Ethertyp is 0x0806). */
	GSW_EXTENDEDVLAN_FILTER_ETHERTYPE_ARP = 3,
	/** IPv6 IPoE frame (Ethertyp is 0x86DD). */
	GSW_EXTENDEDVLAN_FILTER_ETHERTYPE_IPV6IPOE = 4,
	/** EAPOL (Ethertyp is 0x888E). */
	GSW_EXTENDEDVLAN_FILTER_ETHERTYPE_EAPOL = 5
} GSW_ExtendedVlanFilterEthertype_t;

/** \brief Extended VLAN Filter VLAN Tag.
    Used by \ref GSW_EXTENDEDVLAN_filter_t. */
typedef struct {
	/** Filter Type: normal filter, default rule, or no tag */
	GSW_ExtendedVlanFilterType_t eType;
	/** Enable priority field filtering. */
	ltq_bool_t bPriorityEnable;
	/** Filter priority value if bPriorityEnable is TRUE. */
	u32 nPriorityVal;
	/** Enable VID filtering. */
	ltq_bool_t bVidEnable;
	/** Filter VID if bVidEnable is TRUE. */
	u32 nVidVal;
	/** Mode to filter TPID of VLAN tag. */
	GSW_ExtendedVlanFilterTpid_t eTpid;
	/** Mode to filter DEI of VLAN tag. */
	GSW_ExtendedVlanFilterDei_t eDei;
} GSW_EXTENDEDVLAN_filterVLAN_t;

/** \brief Extended VLAN Filter.
    Used by \ref GSW_EXTENDEDVLAN_config_t. */
typedef struct {
	/** Filter on Original Packet. */
	ltq_bool_t bOriginalPacketFilterMode;
	GSW_ExtendedVlan_4_Tpid_Mode_t eFilter_4_Tpid_Mode;
	/** Filter for outer VLAN tag. */
	GSW_EXTENDEDVLAN_filterVLAN_t sOuterVlan;
	/** Filter for inner VLAN tag. */
	GSW_EXTENDEDVLAN_filterVLAN_t sInnerVlan;
	/** Filter EtherType. */
	GSW_ExtendedVlanFilterEthertype_t eEtherType;
} GSW_EXTENDEDVLAN_filter_t;

/** \brief Extended VLAN Treatment Remove Tag.
    Used by \ref GSW_EXTENDEDVLAN_treatmentVlan_t. */
typedef enum {
	/** Do not remove VLAN tag. */
	GSW_EXTENDEDVLAN_TREATMENT_NOT_REMOVE_TAG = 0,
	/** Remove 1 VLAN tag following DA/SA. */
	GSW_EXTENDEDVLAN_TREATMENT_REMOVE_1_TAG = 1,
	/** Remove 2 VLAN tag following DA/SA. */
	GSW_EXTENDEDVLAN_TREATMENT_REMOVE_2_TAG = 2,
	/** Discard upstream traffic. */
	GSW_EXTENDEDVLAN_TREATMENT_DISCARD_UPSTREAM = 3,
} GSW_ExtendedVlanTreatmentRemoveTag_t;

/** \brief Extended VLAN Treatment Set Priority.
   Used by \ref GSW_EXTENDEDVLAN_treatmentVlan_t. */
typedef enum {
	/** Set priority with given value. */
	GSW_EXTENDEDVLAN_TREATMENT_PRIORITY_VAL = 0,
	/** Prority value is copied from inner VLAN tag of received packet. */
	GSW_EXTENDEDVLAN_TREATMENT_INNER_PRORITY = 1,
	/** Prority value is copied from outer VLAN tag of received packet. */
	GSW_EXTENDEDVLAN_TREATMENT_OUTER_PRORITY = 2,
	/** Prority value is derived from DSCP field of received packet. */
	GSW_EXTENDEDVLAN_TREATMENT_DSCP = 3
} GSW_ExtendedVlanTreatmentPriority_t;

/** \brief Extended VLAN Treatment Set VID.
   Used by \ref GSW_EXTENDEDVLAN_treatmentVlan_t. */
typedef enum {
	/** Set VID with given value. */
	GSW_EXTENDEDVLAN_TREATMENT_VID_VAL = 0,
	/** VID is copied from inner VLAN tag of received packet. */
	GSW_EXTENDEDVLAN_TREATMENT_INNER_VID = 1,
	/** VID is copied from outer VLAN tag of received packet. */
	GSW_EXTENDEDVLAN_TREATMENT_OUTER_VID = 2,
} GSW_ExtendedVlanTreatmentVid_t;

/** \brief Extended VLAN Treatment Set TPID.
   Used by \ref GSW_EXTENDEDVLAN_treatmentVlan_t. */
typedef enum {
	/** TPID is copied from inner VLAN tag of received packet. */
	GSW_EXTENDEDVLAN_TREATMENT_INNER_TPID = 0,
	/** TPID is copied from outer VLAN tag of received packet. */
	GSW_EXTENDEDVLAN_TREATMENT_OUTER_TPID = 1,
	/** TPID is global configured value. */
	GSW_EXTENDEDVLAN_TREATMENT_VTETYPE = 2,
	/** TPID is 0x8100. */
	GSW_EXTENDEDVLAN_TREATMENT_8021Q = 3
} GSW_ExtendedVlanTreatmentTpid_t;

/** \brief Extended VLAN Treatment Set DEI.
   Used by \ref GSW_EXTENDEDVLAN_treatmentVlan_t. */
typedef enum {
	/** DEI (if applicable) is copied from inner VLAN tag of received packet. */
	GSW_EXTENDEDVLAN_TREATMENT_INNER_DEI = 0,
	/** DEI (if applicable) is copied from outer VLAN tag of received packet. */
	GSW_EXTENDEDVLAN_TREATMENT_OUTER_DEI = 1,
	/** DEI is 0. */
	GSW_EXTENDEDVLAN_TREATMENT_DEI_0 = 2,
	/** DEI is 1. */
	GSW_EXTENDEDVLAN_TREATMENT_DEI_1 = 3
} GSW_ExtendedVlanTreatmentDei_t;

/** \brief Extended VLAN Treatment VLAN Tag.
    Used by \ref GSW_EXTENDEDVLAN_treatment_t. */
typedef struct {
	/** Select source of priority field of VLAN tag. */
	GSW_ExtendedVlanTreatmentPriority_t ePriorityMode;
	/** If \ref GSW_EXTENDEDVLAN_treatmentVlan_t::ePriorityMode is
	    \ref GSW_EXTENDEDVLAN_TREATMENT_PRIORITY_VAL, use this value for
	    priority field of VLAN tag. */
	u32 ePriorityVal;
	/** Select source of VID field of VLAN tag. */
	GSW_ExtendedVlanTreatmentVid_t eVidMode;
	/** If \ref GSW_EXTENDEDVLAN_treatmentVlan_t::eVidMode is
	    \ref GSW_EXTENDEDVLAN_TREATMENT_VID_VAL, use this value for VID field
	    of VLAN tag. */
	u32 eVidVal;
	/** Select source of TPID field of VLAN tag. */
	GSW_ExtendedVlanTreatmentTpid_t eTpid;
	/** Select source of DEI field of VLAN tag. */
	GSW_ExtendedVlanTreatmentDei_t eDei;
} GSW_EXTENDEDVLAN_treatmentVlan_t;

/** \brief Extended VLAN Treatment.
    Used by \ref GSW_EXTENDEDVLAN_config_t. */
typedef struct {
	/** Number of VLAN tag(s) to remove. */
	GSW_ExtendedVlanTreatmentRemoveTag_t eRemoveTag;

	GSW_ExtendedVlan_4_Tpid_Mode_t eTreatment_4_Tpid_Mode;

	/** Enable outer VLAN tag add/modification. */
	ltq_bool_t bAddOuterVlan;
	/** If bAddOuterVlan is TRUE, add or modify outer VLAN tag. */
	GSW_EXTENDEDVLAN_treatmentVlan_t sOuterVlan;

	/** Enable inner VLAN tag add/modification. */
	ltq_bool_t bAddInnerVlan;
	/** If bAddInnerVlan is TRUE, add or modify inner VLAN tag. */
	GSW_EXTENDEDVLAN_treatmentVlan_t sInnerVlan;

	/** Enable re-assignment of bridge port. */
	ltq_bool_t bReassignBridgePort;
	/** If bReassignBridgePort is TRUE, use this value for bridge port. */
	u32 nNewBridgePortId;

	/** Enable new DSCP. */
	ltq_bool_t bNewDscpEnable;
	/** If bNewDscpEnable is TRUE, use this value for DSCP. */
	u32 nNewDscp;

	/** Enable new traffic class. */
	ltq_bool_t bNewTrafficClassEnable;
	/** If bNewTrafficClassEnable is TRUE, use this value for traffic class. */
	u32 nNewTrafficClass;

	/** Enable new meter. */
	ltq_bool_t bNewMeterEnable;
	/** New meter ID.

	    \remarks
	    Meter should be allocated with \ref GSW_QOS_METER_ALLOC before extended
	    VLAN treatment is added. If this extended VLAN treatment is deleted,
	    this meter should be released with \ref GSW_QOS_METER_FREE. */
	u32 sNewTrafficMeterId;

	/** DSCP to PCP mapping, if
	    \ref GSW_EXTENDEDVLAN_treatmentVlan_t::ePriorityMode in
	    \ref GSW_EXTENDEDVLAN_treatment_t::sOuterVlan.ePriorityMode or
	    \ref GSW_EXTENDEDVLAN_treatment_t::sInnerVlan.ePriorityMode is
	    \ref GSW_EXTENDEDVLAN_TREATMENT_DSCP.

	    \remarks
	    The index of array stands for DSCP value. Each byte of the array is 3-bit
	    PCP value. For implementation, if DSCP2PCP is separate hardware table,
	    a resource management mechanism should be implemented. Allocation happens
	    when extended VLAN treatment added, and release happens when the
	    treatment is deleted. For debug, the DSCP2PCP table can be dumped with
	    \ref GSW_DSCP2PCP_MAP_GET. */
	u8 nDscp2PcpMap[64];

	/** Enable loopback. */
	ltq_bool_t bLoopbackEnable;
	/** Enable destination/source MAC address swap. */
	ltq_bool_t bDaSaSwapEnable;
	/** Enable traffic mirrored to the monitoring port. */
	ltq_bool_t bMirrorEnable;
} GSW_EXTENDEDVLAN_treatment_t;

/** \brief Extended VLAN Allocation.
    Used by \ref GSW_EXTENDEDVLAN_ALLOC and \ref GSW_EXTENDEDVLAN_FREE. */
typedef struct {
	/** Total number of extended VLAN entries are requested. Proper value should
	    be given for \ref GSW_EXTENDEDVLAN_ALLOC. This field is ignored for
	    \ref GSW_EXTENDEDVLAN_FREE. */
	u32 nNumberOfEntries;

	/** If \ref GSW_EXTENDEDVLAN_ALLOC is successful, a valid ID will be returned
	    in this field. Otherwise, \ref INVALID_HANDLE is returned in this field.
	    For \ref GSW_EXTENDEDVLAN_FREE, this field should be valid ID returned by
	    \ref GSW_EXTENDEDVLAN_ALLOC. */
	u32 nExtendedVlanBlockId;
} GSW_EXTENDEDVLAN_alloc_t;

/** \brief Extended VLAN Configuration.
    Used by \ref GSW_EXTENDEDVLAN_SET and \ref GSW_EXTENDEDVLAN_GET. */
typedef struct {
	/** This should be valid ID returned by \ref GSW_EXTENDEDVLAN_ALLOC.
	    If it is \ref INVALID_HANDLE, \ref GSW_EXTENDEDVLAN_config_t::nEntryIndex
	    is absolute index of Extended VLAN entry in hardware for debug purpose,
	    bypassing any check. */
	u32 nExtendedVlanBlockId;

	/** Index of entry, ranges between 0 and
	    \ref GSW_EXTENDEDVLAN_alloc_t::nNumberOfEntries - 1, to
	    set (\ref GSW_EXTENDEDVLAN_SET) or get (\ref GSW_EXTENDEDVLAN_GET)
	    Extended VLAN Configuration entry. For debug purpose, this field could be
	    absolute index of Entended VLAN entry in hardware, when
	    \ref GSW_EXTENDEDVLAN_config_t::nExtendedVlanBlockId is
	    \ref INVALID_HANDLE. */
	u32 nEntryIndex;

	/** Extended VLAN Filter */
	GSW_EXTENDEDVLAN_filter_t sFilter;
	/** Extended VLAN Treatment */
	GSW_EXTENDEDVLAN_treatment_t sTreatment;
} GSW_EXTENDEDVLAN_config_t;

/** \brief VLAN Filter TCI Mask.
    Used by \ref GSW_VLANFILTER_config_t */
typedef enum {
	GSW_VLAN_FILTER_TCI_MASK_VID = 0,
	GSW_VLAN_FILTER_TCI_MASK_PCP = 1,
	GSW_VLAN_FILTER_TCI_MASK_TCI = 2
} GSW_VlanFilterTciMask_t;

/** \brief VLAN Filter Allocation.
    Used by \ref GSW_VLANFILTER_ALLOC and \ref GSW_VLANFILTER_FREE. */
typedef struct {
	/** Total number of VLAN Filter entries are requested. Proper value should
	    be given for \ref GSW_VLANFILTER_ALLOC. This field is ignored for
	    \ref GSW_VLANFILTER_FREE. */
	u32 nNumberOfEntries;

	/** If \ref GSW_VLANFILTER_ALLOC is successful, a valid ID will be returned
	    in this field. Otherwise, \ref INVALID_HANDLE is returned in this field.
	    For \ref GSW_EXTENDEDVLAN_FREE, this field should be valid ID returned by
	    \ref GSW_VLANFILTER_ALLOC. */
	u32 nVlanFilterBlockId;

	/** Discard packet without VLAN tag. */
	ltq_bool_t bDiscardUntagged;
	/** Discard VLAN tagged packet not matching any filter entry. */
	ltq_bool_t bDiscardUnmatchedTagged;
	/** Use default port VLAN ID for VLAN filtering

	    \remarks
	    This field is not available in PRX300. */
	ltq_bool_t bUseDefaultPortVID;
} GSW_VLANFILTER_alloc_t;

/** \brief VLAN Filter.
    Used by \ref GSW_VLANFILTER_SET and \ref GSW_VLANFILTER_GET */
typedef struct {
	/** This should be valid ID return by \ref GSW_VLANFILTER_ALLOC.
	    If it is \ref INVALID_HANDLE, \ref GSW_VLANFILTER_config_t::nEntryIndex
	    is absolute index of VLAN Filter entry in hardware for debug purpose,
	    bypassing any check. */
	u32 nVlanFilterBlockId;

	/** Index of entry. ranges between 0 and
	    \ref GSW_VLANFILTER_alloc_t::nNumberOfEntries - 1, to
	    set (\ref GSW_VLANFILTER_SET) or get (\ref GSW_VLANFILTER_GET)
	    VLAN FIlter entry. For debug purpose, this field could be absolute index
	    of VLAN Filter entry in hardware, when
	    \ref GSW_VLANFILTER_config_t::nVlanFilterBlockId is
	    \ref INVALID_HANDLE. */
	u32 nEntryIndex;

	/** VLAN TCI filter mask mode.

	    \remarks
	    In GSWIP-3.1, this field of first entry in the block will applies to rest
	    of entries in the same block. */
	GSW_VlanFilterTciMask_t eVlanFilterMask;

	/** This is value for VLAN filtering. It depends on
	    \ref GSW_VLANFILTER_config_t::eVlanFilterMask.
	    For GSW_VLAN_FILTER_TCI_MASK_VID, this is 12-bit VLAN ID.
	    For GSW_VLAN_FILTER_TCI_MASK_PCP, this is 3-bit PCP field of VLAN tag.
	    For GSW_VLAN_FILTER_TCI_MASK_TCI, this is 16-bit TCI of VLAN tag. */
	u32 nVal;
	/** Discard packet if match. */
	ltq_bool_t bDiscardMatched;
} GSW_VLANFILTER_config_t;

/** \brief CTP Port configuration mask.
    Used by \ref GSW_CTP_portConfig_t. */
typedef enum {
	/** Mask for \ref GSW_CTP_portConfig_t::nBridgePortId */
	GSW_CTP_PORT_CONFIG_MASK_BRIDGE_PORT_ID = 0x00000001,
	/** Mask for \ref GSW_CTP_portConfig_t::bForcedTrafficClass and
	    \ref GSW_CTP_portConfig_t::nDefaultTrafficClass */
	GSW_CTP_PORT_CONFIG_MASK_FORCE_TRAFFIC_CLASS = 0x00000002,
	/** Mask for \ref GSW_CTP_portConfig_t::bIngressExtendedVlanEnable and
	    \ref GSW_CTP_portConfig_t::nIngressExtendedVlanBlockId */
	GSW_CTP_PORT_CONFIG_MASK_INGRESS_VLAN = 0x00000004,
	/** Mask for \ref GSW_CTP_portConfig_t::bIngressExtendedVlanIgmpEnable and
	    \ref GSW_CTP_portConfig_t::nIngressExtendedVlanBlockIdIgmp */
	GSW_CTP_PORT_CONFIG_MASK_INGRESS_VLAN_IGMP = 0x00000008,
	/** Mask for \ref GSW_CTP_portConfig_t::bEgressExtendedVlanEnable and
	    \ref GSW_CTP_portConfig_t::nEgressExtendedVlanBlockId */
	GSW_CTP_PORT_CONFIG_MASK_EGRESS_VLAN = 0x00000010,
	/** Mask for \ref GSW_CTP_portConfig_t::bEgressExtendedVlanIgmpEnable and
	    \ref GSW_CTP_portConfig_t::nEgressExtendedVlanBlockIdIgmp */
	GSW_CTP_PORT_CONFIG_MASK_EGRESS_VLAN_IGMP = 0x00000020,
	/** Mask for \ref GSW_CTP_portConfig_t::bIngressNto1VlanEnable */
	GSW_CTP_PORT_CONFIG_MASK_INRESS_NTO1_VLAN = 0x00000040,
	/** Mask for \ref GSW_CTP_portConfig_t::bEgressNto1VlanEnable */
	GSW_CTP_PORT_CONFIG_MASK_EGRESS_NTO1_VLAN = 0x00000080,
	/** Mask for \ref GSW_CTP_portConfig_t::eIngressMarkingMode */
	GSW_CTP_PORT_CONFIG_INGRESS_MARKING = 0x00000100,
	/** Mask for \ref GSW_CTP_portConfig_t::eEgressMarkingMode */
	GSW_CTP_PORT_CONFIG_EGRESS_MARKING = 0x00000200,
	/** Mask for \ref GSW_CTP_portConfig_t::bEgressMarkingOverrideEnable and
	    \ref GSW_CTP_portConfig_t::eEgressMarkingModeOverride */
	GSW_CTP_PORT_CONFIG_EGRESS_MARKING_OVERRIDE = 0x00000400,
	/** Mask for \ref GSW_CTP_portConfig_t::eEgressRemarkingMode */
	GSW_CTP_PORT_CONFIG_EGRESS_REMARKING = 0x00000800,
	/** Mask for \ref GSW_CTP_portConfig_t::bIngressMeteringEnable and
	    \ref GSW_CTP_portConfig_t::nIngressTrafficMeterId */
	GSW_CTP_PORT_CONFIG_INGRESS_METER = 0x00001000,
	/** Mask for \ref GSW_CTP_portConfig_t::bEgressMeteringEnable and
	    \ref GSW_CTP_portConfig_t::nEgressTrafficMeterId */
	GSW_CTP_PORT_CONFIG_EGRESS_METER = 0x00002000,
	/** Mask for \ref GSW_CTP_portConfig_t::bBridgingBypass,
	    \ref GSW_CTP_portConfig_t::nDestLogicalPortId,
	    \ref GSW_CTP_portConfig_t::bPmapperEnable,
	    \ref GSW_CTP_portConfig_t::nDestSubIfIdGroup,
	    \ref GSW_CTP_portConfig_t::ePmapperMappingMode and
	    \ref GSW_CTP_portConfig_t::sPmapper */
	GSW_CTP_PORT_CONFIG_BRIDGING_BYPASS = 0x00004000,
	/** Mask for \ref GSW_CTP_portConfig_t::nFirstFlowEntryIndex and
	    \ref GSW_CTP_portConfig_t::nNumberOfFlowEntries */
	GSW_CTP_PORT_CONFIG_FLOW_ENTRY = 0x00008000,
	/** Mask for \ref GSW_CTP_portConfig_t::bIngressLoopbackEnable,
	    \ref GSW_CTP_portConfig_t::bIngressDaSaSwapEnable,
	    \ref GSW_CTP_portConfig_t::bEgressLoopbackEnable,
	    \ref GSW_CTP_portConfig_t::bEgressDaSaSwapEnable,
	    \ref GSW_CTP_portConfig_t::bIngressMirrorEnable and
	    \ref GSW_CTP_portConfig_t::bEgressMirrorEnable */
	GSW_CTP_PORT_CONFIG_LOOPBACK_AND_MIRROR = 0x00010000,

	/** Enable all fields */
	GSW_CTP_PORT_CONFIG_MASK_ALL = 0x7FFFFFFF,
	/** Bypass any check for debug purpose */
	GSW_CTP_PORT_CONFIG_MASK_FORCE = 0x80000000
} GSW_CtpPortConfigMask_t;

/** \brief Color Marking Mode
    Used by \ref GSW_CTP_portConfig_t. */
typedef enum {
	/** mark packets (except critical) to green */
	GSW_MARKING_ALL_GREEN = 0,
	/** do not change color and priority */
	GSW_MARKING_INTERNAL_MARKING = 1,
	/** DEI mark mode */
	GSW_MARKING_DEI = 2,
	/** PCP 8P0D mark mode */
	GSW_MARKING_PCP_8P0D = 3,
	/** PCP 7P1D mark mode */
	GSW_MARKING_PCP_7P1D = 4,
	/** PCP 6P2D mark mode */
	GSW_MARKING_PCP_6P2D = 5,
	/** PCP 5P3D mark mode */
	GSW_MARKING_PCP_5P3D = 6,
	/** DSCP AF class */
	GSW_MARKING_DSCP_AF = 7
} GSW_ColorMarkingMode_t;

/** \brief Color Remarking Mode
    Used by \ref GSW_CTP_portConfig_t. */
typedef enum {
	/** values from last process stage */
	GSW_REMARKING_NONE = 0,
	/** DEI mark mode */
	GSW_REMARKING_DEI = 2,
	/** PCP 8P0D mark mode */
	GSW_REMARKING_PCP_8P0D = 3,
	/** PCP 7P1D mark mode */
	GSW_REMARKING_PCP_7P1D = 4,
	/** PCP 6P2D mark mode */
	GSW_REMARKING_PCP_6P2D = 5,
	/** PCP 5P3D mark mode */
	GSW_REMARKING_PCP_5P3D = 6,
	/** DSCP AF class */
	GSW_REMARKING_DSCP_AF = 7
} GSW_ColorRemarkingMode_t;

/** \brief P-mapper Mapping Mode
    Used by \ref GSW_CTP_portConfig_t. */
typedef enum {
	/** Use PCP for VLAN tagged packets to derive sub interface ID group.

	    \remarks
	    P-mapper table entry 1-8. */
	GSW_PMAPPER_MAPPING_PCP = 0,
	/** Use DSCP for VLAN tagged IP packets to derive sub interface ID group.

	    \remarks
	    P-mapper table entry 9-72. */
	GSW_PMAPPER_MAPPING_DSCP = 1
} GSW_PmapperMappingMode_t;

/** \brief P-mapper Configuration
    Used by \ref GSW_CTP_portConfig_t. */
typedef struct {
	/** Index of P-mapper. */
	u32 nPmapperId;

	/** Sub interface ID group.

	    \remarks
	    Entry 0 is for non-IP and non-VLAN tagged packets. Entries 1-8 are PCP
	    mapping entries for VLAN tagged packets with \ref GSW_PMAPPER_MAPPING_PCP
	    selected. Entries 9-72 are DSCP mapping entries for IP packets without
	    VLAN tag or VLAN tagged packets with \ref GSW_PMAPPER_MAPPING_DSCP
	    selected */
	u8 nDestSubIfIdGroup[73];
} GSW_PMAPPER_t;

/** \brief CTP Port Configuration.
    Used by \ref GSW_CTP_PORT_CONFIG_SET and \ref GSW_CTP_PORT_CONFIG_GET. */
typedef struct {
	/** Logical Port Id. The valid range is hardware dependent.
	    If \ref GSW_CTP_portConfig_t::eMask has
	    \ref GSW_CtpPortConfigMask_t::GSW_CTP_PORT_CONFIG_MASK_FORCE, this field
	    is ignored. */
	u32 nLogicalPortId;

	/** Sub interface ID group. The valid range is hardware/protocol dependent.

	    \remarks
	    Sub interface ID group is defined for each of \ref GSW_LogicalPortMode_t.
	    For both \ref GSW_LOGICAL_PORT_8BIT_WLAN and
	    \ref GSW_LOGICAL_PORT_9BIT_WLAN, this field is VAP.
	    For \ref GSW_LOGICAL_PORT_GPON, this field is GEM index.
	    For \ref GSW_LOGICAL_PORT_EPON, this field is stream index.
	    For \ref GSW_LOGICAL_PORT_GINT, this field is LLID.
	    For others, this field is 0.
	    If \ref GSW_CTP_portConfig_t::eMask has
	    \ref GSW_CtpPortConfigMask_t::GSW_CTP_PORT_CONFIG_MASK_FORCE, this field
	    is absolute index of CTP in hardware for debug purpose, bypassing
	    any check. */
	u32 nSubIfIdGroup;

	/** Mask for updating/retrieving fields. */
	GSW_CtpPortConfigMask_t eMask;

	/** Ingress Bridge Port ID to which this CTP port is associated for ingress
	    traffic. */
	u32 nBridgePortId;

	/** Default traffic class can not be overridden by other rules (except
	    traffic flow table and special tag) in processing stages. */
	ltq_bool_t bForcedTrafficClass;
	/** Default traffic class associated with all ingress traffic from this CTP
	    Port. */
	u32 nDefaultTrafficClass;

	/** Enable Extended VLAN processing for ingress non-IGMP traffic. */
	ltq_bool_t bIngressExtendedVlanEnable;
	/** Extended VLAN block allocated for ingress non-IGMP traffic. It defines
	    extended VLAN process for ingress non-IGMP traffic. Valid when
	    bIngressExtendedVlanEnable is TRUE. */
	u32 nIngressExtendedVlanBlockId;
	/** Extended VLAN block size for ingress non-IGMP traffic. This is optional.
	    If it is 0, the block size of nIngressExtendedVlanBlockId will be used.
	    Otherwise, this field will be used. */
	u32 nIngressExtendedVlanBlockSize;
	/** Enable extended VLAN processing for ingress IGMP traffic. */
	ltq_bool_t bIngressExtendedVlanIgmpEnable;
	/** Extended VLAN block allocated for ingress IGMP traffic. It defines
	    extended VLAN process for ingress IGMP traffic. Valid when
	    bIngressExtendedVlanIgmpEnable is TRUE. */
	u32 nIngressExtendedVlanBlockIdIgmp;
	/** Extended VLAN block size for ingress IGMP traffic. This is optional.
	    If it is 0, the block size of nIngressExtendedVlanBlockIdIgmp will be
	    used. Otherwise, this field will be used. */
	u32 nIngressExtendedVlanBlockSizeIgmp;

	/** Enable extended VLAN processing for egress non-IGMP traffic. */
	ltq_bool_t bEgressExtendedVlanEnable;
	/** Extended VLAN block allocated for egress non-IGMP traffic. It defines
	    extended VLAN process for egress non-IGMP traffic. Valid when
	    bEgressExtendedVlanEnable is TRUE. */
	u32 nEgressExtendedVlanBlockId;
	/** Extended VLAN block size for egress non-IGMP traffic. This is optional.
	    If it is 0, the block size of nEgressExtendedVlanBlockId will be used.
	    Otherwise, this field will be used. */
	u32 nEgressExtendedVlanBlockSize;
	/** Enable extended VLAN processing for egress IGMP traffic. */
	ltq_bool_t bEgressExtendedVlanIgmpEnable;
	/** Extended VLAN block allocated for egress IGMP traffic. It defines
	    extended VLAN process for egress IGMP traffic. Valid when
	    bEgressExtendedVlanIgmpEnable is TRUE. */
	u32 nEgressExtendedVlanBlockIdIgmp;
	/** Extended VLAN block size for egress IGMP traffic. This is optional.
	    If it is 0, the block size of nEgressExtendedVlanBlockIdIgmp will be
	    used. Otherwise, this field will be used. */
	u32 nEgressExtendedVlanBlockSizeIgmp;

	/** For WLAN type logical port, this should be FALSE. For other types, if
	     enabled and ingress packet is VLAN tagged, outer VLAN ID is used for
	    "nSubIfId" field in MAC table, otherwise, 0 is used for "nSubIfId". */
	ltq_bool_t bIngressNto1VlanEnable;
	/** For WLAN type logical port, this should be FALSE. For other types, if
	     enabled and egress packet is known unicast, outer VLAN ID is from
	     "nSubIfId" field in MAC table. */
	ltq_bool_t bEgressNto1VlanEnable;

	/** Ingress color marking mode for ingress traffic. */
	GSW_ColorMarkingMode_t eIngressMarkingMode;
	/** Egress color marking mode for ingress traffic at egress priority queue
	    color marking stage */
	GSW_ColorMarkingMode_t eEgressMarkingMode;
	/** Egress color marking mode override color marking mode from last stage. */
	ltq_bool_t bEgressMarkingOverrideEnable;
	/** Egress color marking mode for egress traffic. Valid only when
	    bEgressMarkingOverride is TRUE. */
	GSW_ColorMarkingMode_t eEgressMarkingModeOverride;

	/** Color remarking for egress traffic. */
	GSW_ColorRemarkingMode_t eEgressRemarkingMode;

	/** Traffic metering on ingress traffic applies. */
	ltq_bool_t bIngressMeteringEnable;
	/** Meter for ingress CTP process.

	    \remarks
	    Meter should be allocated with \ref GSW_QOS_METER_ALLOC before CTP
	    port configuration. If this CTP port is re-set, the last used meter
	    should be released. */
	u32 nIngressTrafficMeterId;
	/** Traffic metering on egress traffic applies. */
	ltq_bool_t bEgressMeteringEnable;
	/** Meter for egress CTP process.

	    \remarks
	    Meter should be allocated with \ref GSW_QOS_METER_ALLOC before CTP
	    port configuration. If this CTP port is re-set, the last used meter
	    should be released. */
	u32 nEgressTrafficMeterId;

	/** Ingress traffic bypass bridging/multicast processing. Following
	    parameters are used to determine destination. Traffic flow table is not
	    bypassed. */
	ltq_bool_t bBridgingBypass;
	/** When bBridgingBypass is TRUE, this field defines destination logical
	    port. */
	u32 nDestLogicalPortId;
	/** When bBridgingBypass is TRUE, this field indicates whether to use
	    \ref GSW_CTP_portConfig_t::nDestSubIfIdGroup or
	    \ref GSW_CTP_portConfig_t::ePmapperMappingMode/
	    \ref GSW_CTP_portConfig_t::sPmapper. */
	ltq_bool_t bPmapperEnable;
	/** When bBridgingBypass is TRUE and bPmapperEnable is FALSE, this field
	    defines destination sub interface ID group. */
	u32 nDestSubIfIdGroup;
	/** When bBridgingBypass is TRUE and bPmapperEnable is TRUE, this field
	    selects either DSCP or PCP to derive sub interface ID. */
	GSW_PmapperMappingMode_t ePmapperMappingMode;
	/** When bBridgingBypass is TRUE and bPmapperEnable is TRUE, P-mapper is
	    used. API implementation need take care of P-mapper allocation, and
	    maintain the reference counter of P-mapper used multiple times. */
	GSW_PMAPPER_t sPmapper;

	/** First traffic flow table entry is associated to this CTP port. Ingress
	    traffic from this CTP port will go through traffic flow table search
	    starting from nFirstFlowEntryIndex. Should be times of 4. */
	u32 nFirstFlowEntryIndex;
	/** Number of traffic flow table entries are associated to this CTP port.
	    Ingress traffic from this CTP port will go through PCE rules search
	    ending at (nFirstFlowEntryIndex+nNumberOfFlowEntries)-1. Should
	    be times of 4. */
	u32 nNumberOfFlowEntries;

	/** Ingress traffic from this CTP port will be redirected to ingress
	    logical port of this CTP port with source sub interface ID used as
	    destination sub interface ID. Following processing except traffic
	    flow table search is bypassed if loopback enabled. */
	ltq_bool_t bIngressLoopbackEnable;
	/** Destination/Source MAC address of ingress traffic is swapped before
	    transmitted (not swapped during PCE processing stages). If destination
	    is multicast, there is no swap, but source MAC address is replaced
	    with global configurable value. */
	ltq_bool_t bIngressDaSaSwapEnable;
	/** Egress traffic to this CTP port will be redirected to ingress logical
	    port with same sub interface ID as ingress. */
	ltq_bool_t bEgressLoopbackEnable;
	/** Destination/Source MAC address of egress traffic is swapped before
	    transmitted. */
	ltq_bool_t bEgressDaSaSwapEnable;

	/** If enabled, ingress traffic is mirrored to the monitoring port.

	    \remarks
	    This should be used exclusive with bIngressLoopbackEnable. */
	ltq_bool_t bIngressMirrorEnable;
	/** If enabled, egress traffic is mirrored to the monitoring port.

	    \remarks
	    This should be used exclusive with bEgressLoopbackEnable. */
	ltq_bool_t bEgressMirrorEnable;
} GSW_CTP_portConfig_t;

/** \brief Bridge Port configuration mask.
    Used by \ref GSW_BRIDGE_portConfig_t. */
typedef enum {
	/** Mask for \ref GSW_BRIDGE_portConfig_t::nBridgeId */
	GSW_BRIDGE_PORT_CONFIG_MASK_BRIDGE_ID = 0x00000001,
	/** Mask for \ref GSW_BRIDGE_portConfig_t::bIngressExtendedVlanEnable and
	    \ref GSW_BRIDGE_portConfig_t::nIngressExtendedVlanBlockId */
	GSW_BRIDGE_PORT_CONFIG_MASK_INGRESS_VLAN = 0x00000002,
	/** Mask for \ref GSW_BRIDGE_portConfig_t::bEgressExtendedVlanEnable and
	    \ref GSW_BRIDGE_portConfig_t::nEgressExtendedVlanBlockId */
	GSW_BRIDGE_PORT_CONFIG_MASK_EGRESS_VLAN = 0x00000004,
	/** Mask for \ref GSW_BRIDGE_portConfig_t::eIngressMarkingMode */
	GSW_BRIDGE_PORT_CONFIG_MASK_INGRESS_MARKING = 0x00000008,
	/** Mask for \ref GSW_BRIDGE_portConfig_t::eEgressRemarkingMode */
	GSW_BRIDGE_PORT_CONFIG_MASK_EGRESS_REMARKING = 0x00000010,
	/** Mask for \ref GSW_BRIDGE_portConfig_t::bIngressMeteringEnable and
	    \ref GSW_BRIDGE_portConfig_t::nIngressTrafficMeterId */
	GSW_BRIDGE_PORT_CONFIG_MASK_INGRESS_METER = 0x00000020,
	/** Mask for \ref GSW_BRIDGE_portConfig_t::bEgressSubMeteringEnable and
	    \ref GSW_BRIDGE_portConfig_t::nEgressTrafficSubMeterId */
	GSW_BRIDGE_PORT_CONFIG_MASK_EGRESS_SUB_METER = 0x00000040,
	/** Mask for \ref GSW_BRIDGE_portConfig_t::nDestLogicalPortId,
	    \ref GSW_BRIDGE_portConfig_t::bPmapperEnable,
	    \ref GSW_BRIDGE_portConfig_t::nDestSubIfIdGroup,
	    \ref GSW_BRIDGE_portConfig_t::ePmapperMappingMode and
	    \ref GSW_BRIDGE_portConfig_t::sPmapper. */
	GSW_BRIDGE_PORT_CONFIG_MASK_EGRESS_CTP_MAPPING = 0x00000080,
	/** Mask for \ref GSW_BRIDGE_portConfig_t::nBridgePortMap */
	GSW_BRIDGE_PORT_CONFIG_MASK_BRIDGE_PORT_MAP = 0x00000100,

	/** Mask for \ref GSW_BRIDGE_portConfig_t::bMcDestIpLookupEnable. */
	GSW_BRIDGE_PORT_CONFIG_MASK_MC_DEST_IP_LOOKUP = 0x00000200,
	/** Mask for \ref GSW_BRIDGE_portConfig_t::bMcSrcIpLookupEnable. */
	GSW_BRIDGE_PORT_CONFIG_MASK_MC_SRC_IP_LOOKUP = 0x00000400,
	/** Mask for \ref GSW_BRIDGE_portConfig_t::bDestMacLookupEnable. */
	GSW_BRIDGE_PORT_CONFIG_MASK_MC_DEST_MAC_LOOKUP = 0x00000800,
	/** Mask for \ref GSW_BRIDGE_portConfig_t::bSrcMacLearningEnable. */
	GSW_BRIDGE_PORT_CONFIG_MASK_MC_SRC_MAC_LEARNING = 0x00001000,
	/** Mask for \ref GSW_BRIDGE_portConfig_t::bMacSpoofingDetectEnable. */
	GSW_BRIDGE_PORT_CONFIG_MASK_MAC_SPOOFING = 0x00002000,
	/** Mask for \ref GSW_BRIDGE_portConfig_t::bPortLockEnable. */
	GSW_BRIDGE_PORT_CONFIG_MASK_PORT_LOCK = 0x00004000,

	/** Mask for \ref GSW_BRIDGE_portConfig_t::bMacLearningLimitEnable and
	    \ref GSW_BRIDGE_portConfig_t::nMacLearningLimit. */
	GSW_BRIDGE_PORT_CONFIG_MASK_MAC_LEARNING_LIMIT = 0x00008000,
	/** Mask for \ref GSW_BRIDGE_portConfig_t::nMacLearningCount */
	GSW_BRIDGE_PORT_CONFIG_MASK_MAC_LEARNED_COUNT = 0x00010000,

	/** Mask for \ref GSW_BRIDGE_portConfig_t::bIngressVlanFilterEnable and
	    \ref GSW_BRIDGE_portConfig_t::nIngressVlanFilterBlockId. */
	GSW_BRIDGE_PORT_CONFIG_MASK_INGRESS_VLAN_FILTER = 0x00020000,
	/** Mask for \ref GSW_BRIDGE_portConfig_t::bBypassEgressVlanFilter1,
	    \ref GSW_BRIDGE_portConfig_t::bEgressVlanFilter1Enable
	    and \ref GSW_BRIDGE_portConfig_t::nEgressVlanFilter1BlockId. */
	GSW_BRIDGE_PORT_CONFIG_MASK_EGRESS_VLAN_FILTER1 = 0x00040000,
	/** Mask for \ref GSW_BRIDGE_portConfig_t::bEgressVlanFilter2Enable and
	    \ref GSW_BRIDGE_portConfig_t::nEgressVlanFilter2BlockId. */
	GSW_BRIDGE_PORT_CONFIG_MASK_EGRESS_VLAN_FILTER2 = 0x00080000,

	/** Enable all */
	GSW_BRIDGE_PORT_CONFIG_MASK_ALL = 0x7FFFFFFF,
	/** Bypass any check for debug purpose */
	GSW_BRIDGE_PORT_CONFIG_MASK_FORCE = 0x80000000
} GSW_BridgePortConfigMask_t;

/** \brief Meters for various egress traffic type.
    Used by \ref GSW_BRIDGE_portConfig_t. */
typedef enum {
	/** Index of broadcast traffic meter */
	GSW_BRIDGE_PORT_EGRESS_METER_BROADCAST = 0,
	/** Index of known multicast traffic meter */
	GSW_BRIDGE_PORT_EGRESS_METER_MULTICAST = 1,
	/** Index of unknown multicast IP traffic meter */
	GSW_BRIDGE_PORT_EGRESS_METER_UNKNOWN_MC_IP = 2,
	/** Index of unknown multicast non-IP traffic meter */
	GSW_BRIDGE_PORT_EGRESS_METER_UNKNOWN_MC_NON_IP = 3,
	/** Index of unknown unicast traffic meter */
	GSW_BRIDGE_PORT_EGRESS_METER_UNKNOWN_UC = 4,
	/** Index of traffic meter for other types */
	GSW_BRIDGE_PORT_EGRESS_METER_OTHERS = 5,
	/** Number of index */
	GSW_BRIDGE_PORT_EGRESS_METER_MAX = 6
} GSW_BridgePortEgressMeter_t;

/** \brief Bridge forwarding type of packet.
    Used by \ref GSW_BRIDGE_portConfig_t. */
typedef enum {
	/** Packet is flooded to port members of ingress bridge port */
	GSW_BRIDGE_FORWARD_FLOOD = 0,
	/** Packet is dscarded */
	GSW_BRIDGE_FORWARD_DISCARD = 1,
	/** Packet is forwarded to logical port 0 CTP port 0 bridge port 0 */
	GSW_BRIDGE_FORWARD_CPU = 2
} GSW_BridgeForwardMode_t;

/** \brief Bridge Port Allocation.
    Used by \ref GSW_BRIDGE_PORT_ALLOC and \ref GSW_BRIDGE_PORT_FREE. */
typedef struct {
	/** If \ref GSW_BRIDGE_PORT_ALLOC is successful, a valid ID will be returned
	    in this field. Otherwise, \ref INVALID_HANDLE is returned in this field.
	    For \ref GSW_BRIDGE_PORT_FREE, this field should be valid ID returned by
	    \ref GSW_BRIDGE_PORT_ALLOC. ID 0 is special for CPU port in PRX300
	    by mapping to CTP 0 (Logical Port 0 with Sub-interface ID 0), and
	    pre-alloced during initialization. */
	u32 nBridgePortId;
} GSW_BRIDGE_portAlloc_t;

/** \brief Bridge Port Configuration.
    Used by \ref GSW_BRIDGE_PORT_CONFIG_SET and \ref GSW_BRIDGE_PORT_CONFIG_GET. */
typedef struct {
	/** Bridge Port ID allocated by \ref GSW_BRIDGE_PORT_ALLOC.

	    \remarks
	    If \ref GSW_BRIDGE_portConfig_t::eMask has
	    \ref GSW_BridgePortConfigMask_t::GSW_BRIDGE_PORT_CONFIG_MASK_FORCE, this
	    field is absolute index of Bridge Port in hardware for debug purpose,
	    bypassing any check. */
	u32 nBridgePortId;

	/** Mask for updating/retrieving fields. */
	GSW_BridgePortConfigMask_t eMask;

	/** Bridge ID (FID) to which this bridge port is associated. A default
	    bridge (ID 0) should be always available. */
	u32 nBridgeId;

	/** Enable extended VLAN processing for ingress non-IGMP traffic. */
	ltq_bool_t bIngressExtendedVlanEnable;
	/** Extended VLAN block allocated for ingress non-IGMP traffic. It defines
	    extended VLAN process for ingress non-IGMP traffic. Valid when
	    bIngressExtendedVlanEnable is TRUE. */
	u32 nIngressExtendedVlanBlockId;
	/** Extended VLAN block size for ingress non-IGMP traffic. This is optional.
	    If it is 0, the block size of nIngressExtendedVlanBlockId will be used.
	    Otherwise, this field will be used. */
	u32 nIngressExtendedVlanBlockSize;

	/** Enable extended VLAN processing enabled for egress non-IGMP traffic. */
	ltq_bool_t bEgressExtendedVlanEnable;
	/** Extended VLAN block allocated for egress non-IGMP traffic. It defines
	    extended VLAN process for egress non-IGMP traffic. Valid when
	    bEgressExtendedVlanEnable is TRUE. */
	u32 nEgressExtendedVlanBlockId;
	/** Extended VLAN block size for egress non-IGMP traffic. This is optional.
	    If it is 0, the block size of nEgressExtendedVlanBlockId will be used.
	    Otherwise, this field will be used. */
	u32 nEgressExtendedVlanBlockSize;

	/** Ingress color marking mode for ingress traffic. */
	GSW_ColorMarkingMode_t eIngressMarkingMode;

	/** Color remarking for egress traffic. */
	GSW_ColorRemarkingMode_t eEgressRemarkingMode;

	/** Traffic metering on ingress traffic applies. */
	ltq_bool_t bIngressMeteringEnable;
	/** Meter for ingress Bridge Port process.

	    \remarks
	    Meter should be allocated with \ref GSW_QOS_METER_ALLOC before Bridge
	    port configuration. If this Bridge port is re-set, the last used meter
	    should be released. */
	u32 nIngressTrafficMeterId;

	/** Traffic metering on various types of egress traffic (such as broadcast,
	    multicast, unknown unicast, etc) applies. */
	ltq_bool_t bEgressSubMeteringEnable[GSW_BRIDGE_PORT_EGRESS_METER_MAX];
	/** Meter for egress Bridge Port process with specific type (such as
	    broadcast, multicast, unknown unicast, etc). Need pre-allocated for each
	    type. */
	u32 nEgressTrafficSubMeterId[GSW_BRIDGE_PORT_EGRESS_METER_MAX];

	/** This field defines destination logical port. */
	u32 nDestLogicalPortId;
	/** This field indicates whether to enable P-mapper. */
	ltq_bool_t bPmapperEnable;
	/** When bPmapperEnable is FALSE, this field defines destination sub
	    interface ID group. */
	u32 nDestSubIfIdGroup;
	/** When bPmapperEnable is TRUE, this field selects either DSCP or PCP to
	    derive sub interface ID. */
	GSW_PmapperMappingMode_t ePmapperMappingMode;
	/** When bPmapperEnable is TRUE, P-mapper is used. API implementation need
	    take care of P-mapper allocation, and maintain the reference counter of
	    P-mapper used multiple times. */
	GSW_PMAPPER_t sPmapper;

	/** Port map define broadcast domain.

	    \remarks
	    Each bit is one bridge port. Bridge port ID is index * 16 + bit offset.
	    For example, bit 1 of nBridgePortMap[1] is bridge port ID 17. */
	u16 nBridgePortMap[16];

	/** Multicast IP table is searched if this field is FALSE and traffic is IP
	    multicast. */
	ltq_bool_t bMcDestIpLookupDisable;
	/** Multicast IP table is searched if this field is TRUE and traffic is IP
	    multicast. */
	ltq_bool_t bMcSrcIpLookupEnable;

	/** Default is FALSE. Packet is treated as "unknown" if it's not
	    broadcast/multicast packet. */
	ltq_bool_t bDestMacLookupDisable;

	/** Default is FALSE. Source MAC address is learned. */
	ltq_bool_t bSrcMacLearningDisable;

	/** If this field is TRUE and MAC address which is already learned in another
	    bridge port appears on this bridge port, port locking violation is
	    detected. */
	ltq_bool_t bMacSpoofingDetectEnable;

	/** If this field is TRUE and MAC address which is already learned in this
	    bridge port appears on another bridge port, port locking violation is
	    detected. */
	ltq_bool_t bPortLockEnable;

	/** Enable MAC learning limitation. */
	ltq_bool_t bMacLearningLimitEnable;
	/** Max number of MAC can be learned from this bridge port. */
	u32 nMacLearningLimit;

	/** Get number of MAC address learned from this bridge port. */
	u32 nMacLearningCount;

	/** Enable ingress VLAN filter */
	ltq_bool_t bIngressVlanFilterEnable;
	/** VLAN filter block of ingress traffic if
	    \ref GSW_BRIDGE_portConfig_t::bIngressVlanFilterEnable is TRUE. */
	u32 nIngressVlanFilterBlockId;
	/** VLAN filter block size. This is optional.
	    If it is 0, the block size of nIngressVlanFilterBlockId will be used.
	    Otherwise, this field will be used. */
	u32 nIngressVlanFilterBlockSize;
	/** For ingress traffic, bypass VLAN filter 1 at egress bridge port
	    processing. */
	ltq_bool_t bBypassEgressVlanFilter1;
	/** Enable egress VLAN filter 1 */
	ltq_bool_t bEgressVlanFilter1Enable;
	/** VLAN filter block 1 of egress traffic if
	    \ref GSW_BRIDGE_portConfig_t::bEgressVlanFilter1Enable is TRUE. */
	u32 nEgressVlanFilter1BlockId;
	/** VLAN filter block 1 size. This is optional.
	    If it is 0, the block size of nEgressVlanFilter1BlockId will be used.
	    Otherwise, this field will be used. */
	u32 nEgressVlanFilter1BlockSize;
	/** Enable egress VLAN filter 2 */
	ltq_bool_t bEgressVlanFilter2Enable;
	/** VLAN filter block 2 of egress traffic if
	    \ref GSW_BRIDGE_portConfig_t::bEgressVlanFilter2Enable is TRUE. */
	u32 nEgressVlanFilter2BlockId;
	/** VLAN filter block 2 size. This is optional.
	    If it is 0, the block size of nEgressVlanFilter2BlockId will be used.
	    Otherwise, this field will be used. */
	u32 nEgressVlanFilter2BlockSize;
} GSW_BRIDGE_portConfig_t;

/** \brief Bridge configuration mask.
    Used by \ref GSW_BRIDGE_config_t. */
typedef enum {
	/** Mask for \ref GSW_BRIDGE_config_t::bMacLearningLimitEnable
	    and \ref GSW_BRIDGE_config_t::nMacLearningLimit. */
	GSW_BRIDGE_CONFIG_MASK_MAC_LEARNING_LIMIT = 0x00000001,
	/** Mask for \ref GSW_BRIDGE_config_t::nMacLearningCount */
	GSW_BRIDGE_CONFIG_MASK_MAC_LEARNED_COUNT = 0x00000002,
	/** Mask for \ref GSW_BRIDGE_config_t::nLearningDiscardEvent */
	GSW_BRIDGE_CONFIG_MASK_MAC_DISCARD_COUNT = 0x00000004,
	/** Mask for \ref GSW_BRIDGE_config_t::bSubMeteringEnable and
	    \ref GSW_BRIDGE_config_t::nTrafficSubMeterId */
	GSW_BRIDGE_CONFIG_MASK_SUB_METER = 0x00000008,
	/** Mask for \ref GSW_BRIDGE_config_t::eForwardBroadcast,
	    \ref GSW_BRIDGE_config_t::eForwardUnknownMulticastIp,
	    \ref GSW_BRIDGE_config_t::eForwardUnknownMulticastNonIp,
	    and \ref GSW_BRIDGE_config_t::eForwardUnknownUnicast. */
	GSW_BRIDGE_CONFIG_MASK_FORWARDING_MODE = 0x00000010,

	/** Enable all */
	GSW_BRIDGE_CONFIG_MASK_ALL = 0x7FFFFFFF,
	/** Bypass any check for debug purpose */
	GSW_BRIDGE_CONFIG_MASK_FORCE = 0x80000000
} GSW_BridgeConfigMask_t;

/** \brief Bridge Allocation.
    Used by \ref GSW_BRIDGE_ALLOC and \ref GSW_BRIDGE_FREE. */
typedef struct {
	/** If \ref GSW_BRIDGE_ALLOC is successful, a valid ID will be returned
	    in this field. Otherwise, \ref INVALID_HANDLE is returned in this field.
	    For \ref GSW_BRIDGE_FREE, this field should be valid ID returned by
	    \ref GSW_BRIDGE_ALLOC. ID 0 is special Bridge created during
	    initialization. */
	u32 nBridgeId;
} GSW_BRIDGE_alloc_t;

/** \brief Bridge Configuration.
    Used by \ref GSW_BRIDGE_CONFIG_SET and \ref GSW_BRIDGE_CONFIG_GET. */
typedef struct {
	/** Bridge ID (FID) allocated by \ref GSW_BRIDGE_ALLOC.

	    \remarks
	    If \ref GSW_BRIDGE_config_t::eMask has
	    \ref GSW_BridgeConfigMask_t::GSW_BRIDGE_CONFIG_MASK_FORCE, this field is
	    absolute index of Bridge (FID) in hardware for debug purpose, bypassing
	    any check. */
	u32 nBridgeId;

	/** Mask for updating/retrieving fields. */
	GSW_BridgeConfigMask_t eMask;

	/** Enable MAC learning limitation. */
	ltq_bool_t bMacLearningLimitEnable;
	/** Max number of MAC can be learned in this bridge (all bridge ports). */
	u32 nMacLearningLimit;

	/** Get number of MAC address learned from this bridge port. */
	u32 nMacLearningCount;

	/** Number of learning discard event due to hardware resource not available.

	    \remarks
	    This is discard event due to either MAC table full or Hash collision.
	    Discard due to nMacLearningCount reached is not counted in this field. */
	u32 nLearningDiscardEvent;

	/** Traffic metering on type of traffic (such as broadcast, multicast,
	    unknown unicast, etc) applies. */
	ltq_bool_t bSubMeteringEnable[GSW_BRIDGE_PORT_EGRESS_METER_MAX];
	/** Meter for bridge process with specific type (such as broadcast,
	    multicast, unknown unicast, etc). Need pre-allocated for each type. */
	u32 nTrafficSubMeterId[GSW_BRIDGE_PORT_EGRESS_METER_MAX];

	/** Forwarding mode of broadcast traffic. */
	GSW_BridgeForwardMode_t eForwardBroadcast;
	/** Forwarding mode of unknown multicast IP traffic. */
	GSW_BridgeForwardMode_t eForwardUnknownMulticastIp;
	/** Forwarding mode of unknown multicast non-IP traffic. */
	GSW_BridgeForwardMode_t eForwardUnknownMulticastNonIp;
	/** Forwarding mode of unknown unicast traffic. */
	GSW_BridgeForwardMode_t eForwardUnknownUnicast;
} GSW_BRIDGE_config_t;

/** \brief Logical port mode.
    Used by \ref GSW_CTP_portAssignment_t. */
typedef enum {
	/** WLAN with 8-bit station ID */
	GSW_LOGICAL_PORT_8BIT_WLAN = 0,
	/** WLAN with 9-bit station ID */
	GSW_LOGICAL_PORT_9BIT_WLAN = 1,
	/** GPON OMCI context */
	GSW_LOGICAL_PORT_GPON = 2,
	/** EPON context */
	GSW_LOGICAL_PORT_EPON = 3,
	/** G.INT context */
	GSW_LOGICAL_PORT_GINT = 4,
	/** Others (sub interface ID is 0 by default) */
	GSW_LOGICAL_PORT_OTHER = 0xFF,
} GSW_LogicalPortMode_t;

/** \brief CTP Port Assignment/association with logical port.
    Used by \ref GSW_CTP_PORT_ASSIGNMENT_ALLOC, \ref GSW_CTP_PORT_ASSIGNMENT_SET
    and \ref GSW_CTP_PORT_ASSIGNMENT_GET. */
typedef struct {
	/** Logical Port Id. The valid range is hardware dependent. */
	u32 nLogicalPortId;

	/** First CTP Port ID mapped to above logical port ID.

	    \remarks
	    For \ref GSW_CTP_PORT_ASSIGNMENT_ALLOC, this is output when CTP
	    allocation is successful. For other APIs, this is input. */
	u32 nFirstCtpPortId;
	/** Total number of CTP Ports mapped above logical port ID. */
	u32 nNumberOfCtpPort;

	/** Logical port mode to define sub interface ID format. */
	GSW_LogicalPortMode_t eMode;

	/** Bridge ID (FID)

	    \remarks
	    For \ref GSW_CTP_PORT_ASSIGNMENT_ALLOC, this is input. Each CTP allocated
	    is mapped to Bridge Port given by this field. The Bridge Port will be
	    configured to use first CTP
	    (\ref GSW_CTP_portAssignment_t::nFirstCtpPortId) as egress CTP.
	    For other APIs, this is ignored. */
	u32 nBridgePortId;
} GSW_CTP_portAssignment_t;

/** \brief GPID Assignment/association with logical port.
    Used by \ref GSW_LPID_TO_GPID_ASSIGNMENT_SET
    and \ref GSW_LPID_TO_GPID_ASSIGNMENT_GET. */
typedef struct {
	/** Logical Port Id. The valid range is hardware dependent. */
	u32 nLogicalPortId;

	/** First Global Port ID mapped to above logical port ID.
	    */
	u32 nFirstGlobalPortId;
	/** Total number of Global Ports mapped above logical port ID. */
	u32 nNumberOfGlobalPort;
	/** To configure Valid bits for SunInterface id LP mode: others. */
	u32 nValidBits;
}  GSW_LPID_to_GPID_Assignment_t;

/** \brief Logical port and subif Assignment/association with global port.
    Used by \ref GSW_GPID_TO_LPID_ASSIGNMENT_SET
    and \ref GSW_GPID_TO_LPID_ASSIGNMENT_GET. */
typedef struct {
	u32 nGlobalPortId;
	/** Logical Port Id. The valid range is hardware dependent. */
	u32 nLogicalPortId;

	/** Sub Interface Group field.
	    */
	u8 nSubIf_GroupField;
	/** Sub-interface ID override bit and sub-interface ID group field:
	If it is set, sub-interface group ID is from the table.
		Otherwise, it is from the descriptor. */
	ltq_bool_t nSubIf_GroupField_OverRide;
} GSW_GPID_to_LPID_Assignment_t;

/** \brief Color Marking Table.
    There are standards to define the marking table. User should use
    \ref GSW_QOS_COLOR_MARKING_TABLE_SET to initialize the table before color
    marking happens. \ref GSW_QOS_COLOR_MARKING_TABLE_GET is used to get
    the marking table, mainly for debug purpose. */
typedef struct {
	/** Mode of color marking. */
	GSW_ColorMarkingMode_t eMode;

	/** If eMode is GSW_REMARKING_DSCP_AF, index stands for 6-bit DSCP value.
	    If eMode is one of GSW_REMARKING_PCP_8P0D, GSW_REMARKING_PCP_7P1D,
	    GSW_REMARKING_PCP_6P2D and GSW_REMARKING_PCP_5P3D, index 0-7 is
	    3-bit PCP value with DEI is 0, and index 8-15 is 3-bit PCP value with
	    DEI is 1. Ignored in other modes. */
	u8 nPriority[64];
	/** If eMode is GSW_REMARKING_DSCP_AF, index stands for 6-bit DSCP value.
	    If eMode is one of GSW_REMARKING_PCP_8P0D, GSW_REMARKING_PCP_7P1D,
	    GSW_REMARKING_PCP_6P2D and GSW_REMARKING_PCP_5P3D, index 0-7 is 3-bit
	    PCP value with DEI is 0, and index 8-15 is 3-bit PCP value with DEI is 1.
	    Ignored in other modes. */
	GSW_QoS_DropPrecedence_t nColor[64];
} GSW_QoS_colorMarkingEntry_t;

/** \brief Color Remarking Table.
    There are standards to define the remarking table. User should use
    \ref GSW_QOS_COLOR_REMARKING_TABLE_SET to initialize the table before color
    remarking happens. \ref GSW_QOS_COLOR_REMARKING_TABLE_GET is used to get
    the remarking table, mainly for debug purpose. */
typedef struct {
	/** Mode of color remarking. */
	GSW_ColorRemarkingMode_t eMode;

	/** Index stands for color and priority. Index 0-7 is green color with
	    priority (traffic class) 0-7. Index 8-15 is yellow color with priority
	    (traffic class) 0-7. Value is DSCP if eMode is GSW_REMARKING_DSCP_AF.
	    Value bit 0 is DEI and bit 1-3 is PCP if eMode is one of
	    GSW_REMARKING_PCP_8P0D, GSW_REMARKING_PCP_7P1D, GSW_REMARKING_PCP_6P2D
	    and GSW_REMARKING_PCP_5P3D. Value is ignored for other mode. */
	u8 nVal[16];
} GSW_QoS_colorRemarkingEntry_t;

/** \brief DSCP to PCP Mapping.
    Used by \ref GSW_DSCP2PCP_MAP_GET. */
typedef struct {
	/** Index of entry in mapping table. */
	u32 nIndex;

	/** The index of array stands for DSCP value. Each byte of the array is 3-bit
	    PCP value. */
	u8 nMap[64];
} GSW_DSCP2PCP_map_t;

/** \brief MAC Address Filter Type.
    Used by \ref GSW_MACFILTER_default_t */
typedef enum {
	/** Source MAC Address Filter */
	GSW_MACFILTERTYPE_SRC = 0,
	/** Destination MAC Address Filter */
	GSW_MACFILTERTYPE_DEST = 1
} GSW_MacFilterType_t;

/** \brief Default MAC Address Filter.
    Used by \ref GSW_DEFAUL_MAC_FILTER_SET and \ref GSW_DEFAUL_MAC_FILTER_GET */
typedef struct {
	/** MAC Filter Type */
	GSW_MacFilterType_t eType;

	/** Destination bridge port map. For GSWIP-3.1 only.

	    \remarks
	    Each bit stands for 1 bridge port. For PRX300 (GSWIP-3.1 integrated),
	    only index 0-7 is valid. */
	u16 nPortmap[16];
} GSW_MACFILTER_default_t;

/** \brief I-TAG header defintion .GSWIP-3.2 only
	Used by \ref GSW_PBB_Tunnel_Template_Config_t*/
typedef struct {
	/**I-TAG TPID -2 bytes field*/
	ltq_bool_t bTpidEnable;
	u16 nTpid;

	/**I-TAG PCP -3 Bit field*/
	ltq_bool_t bPcpEnable;
	u8 nPcp;

	/**I-TAG DEI -1 Bit field*/
	ltq_bool_t bDeiEnable;
	u8 nDei;

	/**I-TAG UAC -1 Bit field*/
	ltq_bool_t bUacEnable;
	u8 nUac;

	/**I-TAG RES -3 Bit field*/
	ltq_bool_t bResEnable;
	u8 nRes;

	/**I-TAG SID -24 Bit field*/
	ltq_bool_t bSidEnable;
	u32 nSid;
} GSW_I_TAG_Config_t;

/** \brief B-TAG header defintion .GSWIP-3.2 only
	Used by \ref GSW_PBB_Tunnel_Template_Config_t*/
typedef struct {
	/**B-TAG TPID -2 bytes field*/
	ltq_bool_t bTpidEnable;
	u16 nTpid;

	/**B-TAG PCP -3 Bit field*/
	ltq_bool_t bPcpEnable;
	u8 nPcp;

	/**B-TAG DEI -1 Bit field*/
	ltq_bool_t bDeiEnable;
	u8 nDei;

	/**B-TAG VID -12 Bit field*/
	ltq_bool_t bVidEnable;
	u16 nVid;
} GSW_B_TAG_Config_t;

/** \brief Tunnel Template Configuration.GSWIP-3.2 only
    Used by \ref GSW_PBB_TunnelTempate_Config_Set and \ref GSW_PBB_TunnelTempate_Config_Get
    For \ref GSW_PBB_TunnelTempate_Free, this field should be valid ID returned by
	    \ref GSW_PBB_TunnelTempate_Alloc.*/
typedef struct {
	u32 nTunnelTemplateId;

	/** I-Header Destination Address*/
	ltq_bool_t bIheaderDstMACEnable;
	u8 nIheaderDstMAC[GSW_MAC_ADDR_LEN];

	/** I-Header source Address*/
	ltq_bool_t bIheaderSrcMACEnable;
	u8 nIheaderSrcMAC[GSW_MAC_ADDR_LEN];

	/** I-Tag*/
	ltq_bool_t bItagEnable;
	GSW_I_TAG_Config_t sItag;

	/** B-Tag*/
	ltq_bool_t bBtagEnable;
	GSW_B_TAG_Config_t sBtag;
} GSW_PBB_Tunnel_Template_Config_t;

/** \brief TRAFFIC FLOW TABLE  Allocation.
 *	Used by \ref GSW_TFLOW_ALLOC and \ref GSW_TFLOW_FREE.
 */
typedef struct {
	/** Number of traffic flow table entries are
	 * associated to CTP port.Ingress traffic from this CTP
	 *	port will go through PCE rules search ending at
	 *	(nFirstFlowEntryIndex+nNumberOfFlowEntries)-1. Should
	 *	be times of 4. Proper value should be given
	 *	for \ref GSW_TFLOW_ALLOC.
	 *	This field is ignored for \ref GSW_TFLOW_FREE.
	 */
	u32 num_of_pcerules;
	/** If \ref GSW_TFLOW_ALLOC is successful, a valid ID will be returned
	 *  in this field. Otherwise, \ref INVALID_HANDLE is
	 *	returned in this field.
	 *  For \ref GSW_TFLOW_FREE, this field should be valid ID returned by
	 *  \ref GSW_TFLOW_ALLOC.
	 */
	u32 tflowblockid;
} gsw_tflow_alloc_t;

/*@}*/ /* GSW_IOCTL_GSWIP31 */

/* -------------------------------------------------------------------------- */
/*                        IOCTL Command Definitions                           */
/* -------------------------------------------------------------------------- */

/** \addtogroup GSW_IOCTL_BRIDGE */
/*@{*/

/**
   \brief Read an entry of the MAC table.
   If the parameter 'bInitial=TRUE', the GET operation starts at the beginning
   of the table. Otherwise it continues the GET operation at the entry that
   follows the previous access.
   The function sets all fields to zero in case the end of the table is reached.
   In order to read out the complete table, this function can be called in a loop.
   The Switch API sets 'bLast=1' when the last entry is read out.
   This 'bLast' parameter could be the loop exit criteria.

   \param GSW_MAC_tableRead_t Pointer to a MAC table entry
   \ref GSW_MAC_tableRead_t structure that is filled out by the switch driver.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_MAC_TABLE_ENTRY_READ	_IOWR(GSW_MAC_MAGIC, 0x03, GSW_MAC_tableRead_t)

/**
   \brief Search the MAC Address table for a specific address entry.
   A MAC address is provided by the application and Switch API
   performs a search operation on the hardware's MAC address table.
   Many hardware platforms provide an optimized and fast address search algorithm.

   \param GSW_MAC_tableQuery_t Pointer to a MAC table entry
   \ref GSW_MAC_tableQuery_t structure that is filled out by the switch
   implementation.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_MAC_TABLE_ENTRY_QUERY	_IOWR(GSW_MAC_MAGIC, 0x04, GSW_MAC_tableQuery_t)

/**
   \brief Add a MAC table entry. If an entry already exists for the given MAC Address
   in Filtering Database, this entry is overwritten. If not,
   a new entry is added.

   \param GSW_MAC_tableAdd_t Pointer to a MAC table entry
   \ref GSW_MAC_tableAdd_t structure.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_MAC_TABLE_ENTRY_ADD	_IOW(GSW_MAC_MAGIC, 0x02, GSW_MAC_tableAdd_t)

/**
   \brief Remove a single MAC entry from the MAC table.

   \param GSW_MAC_tableRemove_t Pointer to a MAC table entry
   \ref GSW_MAC_tableRemove_t structure.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_MAC_TABLE_ENTRY_REMOVE	_IOW(GSW_MAC_MAGIC, 0x05, GSW_MAC_tableRemove_t)

/**
   \brief Remove all MAC entries from the MAC table.

   \param void This command does not require any parameter structure

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_MAC_TABLE_CLEAR	_IO(GSW_MAC_MAGIC, 0x01)

/**
   \brief Configure the Spanning Tree Protocol state of an Ethernet port.
   The switch supports four Spanning Tree Port states (Disable/Discarding,
   Blocking/Listening, Learning and Forwarding state) for every port, to enable
   the Spanning Tree Protocol function when co-operating with software on
   the CPU port.
   Identified Spanning Tree Protocol packets can be redirected to the CPU port.
   Depending on the hardware implementation, the CPU port assignment is fixed
   or can be configured using \ref GSW_CPU_PORT_CFG_SET.
   The current port state can be read back
   using \ref GSW_STP_PORT_CFG_GET.

   \param GSW_STP_portCfg_t Pointer to \ref GSW_STP_portCfg_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_STP_PORT_CFG_SET	_IOW(GSW_STP_MAGIC, 0x04, GSW_STP_portCfg_t)

/**
   \brief Read out the current Spanning Tree Protocol state of an Ethernet port.
   This configuration can be set using \ref GSW_STP_PORT_CFG_SET.

   \param GSW_STP_portCfg_t Pointer to \ref GSW_STP_portCfg_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_STP_PORT_CFG_GET	_IOWR(GSW_STP_MAGIC, 0x03, GSW_STP_portCfg_t)

/**
   \brief Set the Spanning Tree configuration. This configuration includes the
   filtering of detected spanning tree packets. These packets could be
   redirected to one dedicated port (e.g. CPU port) or they could be discarded.
   The current configuration can be read using \ref GSW_STP_BPDU_RULE_GET.

   \param GSW_STP_BPDU_Rule_t Pointer to \ref GSW_STP_BPDU_Rule_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_STP_BPDU_RULE_SET	_IOW(GSW_STP_MAGIC, 0x02, GSW_STP_BPDU_Rule_t)

/**
   \brief Read the Spanning Tree configuration.
   The configuration can be modified using \ref GSW_STP_BPDU_RULE_SET.

   \param GSW_STP_BPDU_Rule_t Pointer to \ref GSW_STP_BPDU_Rule_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_STP_BPDU_RULE_GET	_IOWR(GSW_STP_MAGIC, 0x01, GSW_STP_BPDU_Rule_t)

/**
   \brief Read the IEEE 802.1x filter configuration.
   The parameters can be modified using \ref GSW_8021X_EAPOL_RULE_SET.

   \param GSW_8021X_EAPOL_Rule_t Pointer to \ref GSW_8021X_EAPOL_Rule_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_8021X_EAPOL_RULE_GET	_IOWR(GSW_EAPOL_MAGIC, 0x01, GSW_8021X_EAPOL_Rule_t)

/**
   \brief Set the IEEE 802.1x filter rule for a dedicated port. Filtered packets can be
   redirected to one dedicated port (e.g. CPU port).
   The switch supports the addition of a specific packet header to the filtered packets
   that contains information like source port, priority and so on.
   The parameters can be read using \ref GSW_8021X_EAPOL_RULE_GET.

   \param GSW_8021X_EAPOL_Rule_t Pointer to \ref GSW_8021X_EAPOL_Rule_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_8021X_EAPOL_RULE_SET	_IOW(GSW_EAPOL_MAGIC, 0x02, GSW_8021X_EAPOL_Rule_t)

/**
   \brief Get the 802.1x port status for a switch port.
   A configuration can be set using \ref GSW_8021X_PORT_CFG_SET

   \param GSW_8021X_portCfg_t Pointer to a
      802.1x port authorized state port
      configuration \ref GSW_8021X_portCfg_t

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_8021X_PORT_CFG_GET	_IOWR(GSW_EAPOL_MAGIC, 0x03, GSW_8021X_portCfg_t)

/**
   \brief Set the 802.1x port status for a switch port.
   The port configuration can be read using \ref GSW_8021X_PORT_CFG_GET.

   \param GSW_8021X_portCfg_t Pointer to a
      802.1x port authorized state port
      configuration \ref GSW_8021X_portCfg_t

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_8021X_PORT_CFG_SET	_IOW(GSW_EAPOL_MAGIC, 0x04, GSW_8021X_portCfg_t)

/**
   \brief Read out the current port trunking algorithm that is used to retrieved if
   a packet is sent on the lower or higher trunking port index number. The algorithm
   performs an hash calculation over the MAC- and IP- addresses using the
   source- and destination- fields. This command retrieve which of the
   mentioned fields is used by the hash algorithm.
   The usage of any field could be configured over
   the \ref GSW_TRUNKING_CFG_SET command.

   \param GSW_trunkingCfg_t Pointer to a
      configuration \ref GSW_trunkingCfg_t

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_TRUNKING_CFG_GET	_IOWR(GSW_TRUNKING_MAGIC, 0x01, GSW_trunkingCfg_t)
/**
   \brief Configure the current port trunking algorithm that is used to retrieved if
   a packet is sent on the lower or higher trunking port index number. The algorithm
   performs an hash calculation over the MAC- and IP- addresses using the
   source- and destination- fields. This command retrieve which of the
   mentioned fields is used by the hash algorithm.
   The usage of any field could be configured over
   the \ref GSW_TRUNKING_CFG_SET command.

   \param GSW_trunkingCfg_t Pointer to a
      configuration \ref GSW_trunkingCfg_t

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_TRUNKING_CFG_SET	_IOW(GSW_TRUNKING_MAGIC, 0x02, GSW_trunkingCfg_t)
/**
   \brief Read out the port trunking state of a given physical Ethernet switch port 'nPortId'.
   Switch API sets the boolean flag 'bAggregateEnable' and the aggregated trunking
   port 'nAggrPortId' in case trunking is enabled on the port.

   Port trunking can be configures by using the command \ref GSW_TRUNKING_PORT_CFG_GET.

   \param GSW_trunkingPortCfg_t Pointer to a
      configuration \ref GSW_trunkingPortCfg_t

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_TRUNKING_PORT_CFG_GET	_IOWR(GSW_TRUNKING_MAGIC, 0x03, GSW_trunkingPortCfg_t)
/**
   \brief Configure the port trunking on two physical Ethernet switch ports.
   A new port trunking group of two groups could be placed or removed.
   The two port index number are given with the parameter 'nPortId' and 'nAggrPortId'.

   The current trunking port state can be read out by using the command \ref GSW_TRUNKING_PORT_CFG_GET.

   \param GSW_trunkingPortCfg_t Pointer to a
      configuration \ref GSW_trunkingPortCfg_t

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_TRUNKING_PORT_CFG_SET	_IOW(GSW_TRUNKING_MAGIC, 0x04, GSW_trunkingPortCfg_t)
/*@}*/ /* GSW_IOCTL_BRIDGE */

/** \addtogroup GSW_IOCTL_VLAN */
/*@{*/

/**
   \brief Add VLAN ID to a reserved VLAN list.
   The switch supports replacing the VID of received packets with the PVID of
   the receiving port. This function adds a VID to the list of VIDs to replace.
   All switch devices support adding VID=0, VID=1 and VID=FFF to be replaced.
   Some devices also allow adding other VIDs to be replaced.
   An added VID could be removed again by
   calling \ref GSW_VLAN_RESERVED_REMOVE.
   This configuration applies to the whole switch device.

   \param GSW_VLAN_reserved_t Pointer to
      an \ref GSW_VLAN_reserved_t structure element.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_VLAN_RESERVED_ADD	_IOW(GSW_VLAN_MAGIC, 0x0A, GSW_VLAN_reserved_t)
/**
   \brief Remove VLAN ID from a reserved VLAN group list.
   This function removes a VID replacement configuration from the switch
   hardware. This replacement configuration replaces the VID of received
   packets with the PVID of the receiving port. This configuration can be
   added using \ref GSW_VLAN_RESERVED_ADD.
   This configuration applies to the whole switch device.

   \param GSW_VLAN_reserved_t Pointer to
      an \ref GSW_VLAN_reserved_t structure element.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_VLAN_RESERVED_REMOVE	_IOW(GSW_VLAN_MAGIC, 0x0B, GSW_VLAN_reserved_t)

/**
   \brief Get VLAN Port Configuration.
   This function returns the VLAN configuration of the given Port 'nPortId'.

   \param GSW_VLAN_portCfg_t Pointer to an
      \ref GSW_VLAN_portCfg_t structure element. Based on the parameter
      'nPortId', the switch API implementation fills out the remaining structure
      elements.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_VLAN_PORT_CFG_GET	_IOWR(GSW_VLAN_MAGIC, 0x05, GSW_VLAN_portCfg_t)

/**
   \brief Set VLAN Port Configuration.
   This function sets the VLAN configuration of the given Port 'nPortId'.

   \param GSW_VLAN_portCfg_t Pointer to an \ref GSW_VLAN_portCfg_t
      structure element.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_VLAN_PORT_CFG_SET	_IOW(GSW_VLAN_MAGIC, 0x06, GSW_VLAN_portCfg_t)
/**
   \brief Add a VLAN ID group to the active VLAN set of the
   Ethernet switch hardware.
   Based on this configuration, VLAN group port members can
   be added using \ref GSW_VLAN_PORT_MEMBER_ADD.
   The VLAN ID configuration can be removed again by
   calling \ref GSW_VLAN_ID_DELETE.

   \param GSW_VLAN_IdCreate_t Pointer to
      an \ref GSW_VLAN_IdCreate_t structure element.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_VLAN_ID_CREATE	_IOW(GSW_VLAN_MAGIC, 0x02, GSW_VLAN_IdCreate_t)
/**
   \brief Remove a VLAN ID group from the active VLAN set of the switch
   hardware. The VLAN ID group was set
   using \ref GSW_VLAN_ID_CREATE. A VLAN ID group can only be
   removed when no port group members are currently configured on the hardware.
   This VLAN ID group membership configuration is done
   using \ref GSW_VLAN_PORT_MEMBER_ADD and can be
   removed again using \ref GSW_VLAN_PORT_MEMBER_REMOVE.

   \param GSW_VLAN_IdDelete_t Pointer to an
          \ref GSW_VLAN_IdDelete_t structure element.

   \remarks A VLAN ID can only be removed in case it was created by
         \ref GSW_VLAN_ID_CREATE and is currently not assigned
         to any Ethernet port (done using \ref GSW_VLAN_PORT_MEMBER_ADD).

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_VLAN_ID_DELETE	_IOW(GSW_VLAN_MAGIC, 0x03, GSW_VLAN_IdDelete_t)
/**
   \brief Add Ethernet port to port members of a given CTAG VLAN group.
   The assignment can be removed using \ref GSW_VLAN_PORT_MEMBER_REMOVE.

   \param GSW_VLAN_portMemberAdd_t Pointer to
      an \ref GSW_VLAN_portMemberAdd_t structure element.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_VLAN_PORT_MEMBER_ADD	_IOW(GSW_VLAN_MAGIC, 0x07, GSW_VLAN_portMemberAdd_t)
/**
   \brief Remove Ethernet port from port members of a given CTAG VLAN group.
   This assignment was done using \ref GSW_VLAN_PORT_MEMBER_ADD.

   \param GSW_VLAN_portMemberRemove_t Pointer to
      an \ref GSW_VLAN_portMemberRemove_t structure element.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_VLAN_PORT_MEMBER_REMOVE          _IOW(GSW_VLAN_MAGIC, 0x09, GSW_VLAN_portMemberRemove_t)
/**
   \brief Read out all given CTAG VLAN group port memberships. Every command call
   returns one VLAN and port membership pair with the corresponding
   egress traffic tag behavior. Call the command in a loop till
   Switch API sets the 'bLast' variable to read all VLAN port memberships.
   Please set the 'bInitial' parameter for the first call starting the
   read operation at the beginning of the VLAN table.

   \param GSW_VLAN_portMemberRead_t Pointer to
      an \ref GSW_VLAN_portMemberRead_t structure element.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_VLAN_PORT_MEMBER_READ	_IOWR(GSW_VLAN_MAGIC, 0x08, GSW_VLAN_portMemberRead_t)
/**
   \brief Read out the FID of a given CTAG VLAN ID.
   This VLAN ID can be added using \ref GSW_VLAN_ID_CREATE.
   This function returns an error in case no valid configuration is
   available for the given VLAN ID.

   \param GSW_VLAN_IdGet_t Pointer to \ref GSW_VLAN_IdGet_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_VLAN_ID_GET	_IOWR(GSW_VLAN_MAGIC, 0x04, GSW_VLAN_IdGet_t)
/**
   \brief This function initializes the VLAN membership and the egress tagged
   portmap of all unconfigured VLAN groups. A VLAN group is defined as
   unconfigured in case it is not configured
   through \ref GSW_VLAN_ID_CREATE.

   \param GSW_VLAN_memberInit_t Pointer to \ref GSW_VLAN_memberInit_t.

   \remarks
   This API is only supported in case the device support 4k VLAN entries
   (capability retrieved by \ref GSW_CAP_GET).
   Devices with a smaller VLAN group table do not support this API,
   because that table does not contain unconfigured entries.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_VLAN_MEMBER_INIT	_IOW(GSW_VLAN_MAGIC, 0x01, GSW_VLAN_memberInit_t)
/**
   \brief Get The current STAG VLAN global device configuration.
   The configuration can be modified by \ref GSW_SVLAN_CFG_SET.

   \param GSW_SVLAN_cfg_t Pointer to an \ref GSW_SVLAN_cfg_t
      structure element.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_SVLAN_CFG_GET	 _IOWR(GSW_VLAN_MAGIC, 0x10, GSW_SVLAN_cfg_t)
/**
   \brief Set STAG VLAN global device configuration.
   The current configuration can be retrieved by \ref GSW_SVLAN_CFG_GET.

   \param GSW_SVLAN_cfg_t Pointer to an \ref GSW_SVLAN_cfg_t
      structure element.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_SVLAN_CFG_SET	_IOW(GSW_VLAN_MAGIC, 0x11, GSW_SVLAN_cfg_t)
/**
   \brief Get STAG VLAN Port Configuration.
   This function returns the STAG VLAN configuration of the given Port 'nPortId'.

   \param GSW_SVLAN_portCfg_t Pointer to an
      \ref GSW_SVLAN_portCfg_t structure element. Based on the parameter
      'nPortId', the switch API implementation fills out the remaining structure
      elements.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_SVLAN_PORT_CFG_GET	_IOWR(GSW_VLAN_MAGIC, 0x12, GSW_SVLAN_portCfg_t)
/**
   \brief Set STAG VLAN Port Configuration.
   This function sets the STAG VLAN configuration of the given Port 'nPortId'.

   \param GSW_SVLAN_portCfg_t Pointer to an \ref GSW_SVLAN_portCfg_t
      structure element.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_SVLAN_PORT_CFG_SET	_IOW(GSW_VLAN_MAGIC, 0x13, GSW_SVLAN_portCfg_t)
/**
   \brief This command writes Egress VLAN Treatment settings of PCE for given egress Port.
   The egress VLAN Config can be read using the command \ref GSW_PCE_EG_VLAN_CFG_GET.

   \param GSW_PCE_EgVLAN_Cfg_t Pointer to \ref GSW_PCE_EgVLAN_Cfg_t.

   \remarks The function returns an error code in case of overlapping VLAN Block configuration.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_SUCCESS: if successful
   - An error code in case an error occurs
*/
#define GSW_PCE_EG_VLAN_CFG_SET	_IOW(GSW_VLAN_MAGIC, 0x0C, GSW_PCE_EgVLAN_Cfg_t)

/**
   \brief This command reads Egress VLAN Treatment settings of PCE for given egress Port.
   An entry can be written using the command \ref GSW_PCE_EG_VLAN_CFG_SET.

   \param GSW_PCE_EgVLAN_Cfg_t Pointer to \ref GSW_PCE_EgVLAN_Cfg_t.

   \remarks The function returns an error code in case of overlapping VLAN Block configuration.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_SUCCESS: if successful
   - An error code in case an error occurs
*/
#define GSW_PCE_EG_VLAN_CFG_GET	_IOWR(GSW_VLAN_MAGIC, 0x0D, GSW_PCE_EgVLAN_Cfg_t)

/**
   \brief This command writes Egress VLAN Treatment entry of PCE for given index (corresponding to FID or SubInterfaceIdGroup (Bits 12-to-8)).
   The egress VLAN Entry can be read using the command \ref GSW_PCE_EG_VLAN_ENTRY_READ.

   \param GSW_PCE_EgVLAN_Entry_t Pointer to \ref GSW_PCE_EgVLAN_Entry_t.

   \remarks The function returns an error code in case of out of range VLAN Entries configuration.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_SUCCESS: if successful
   - An error code in case an error occurs
*/
#define GSW_PCE_EG_VLAN_ENTRY_WRITE	_IOW(GSW_VLAN_MAGIC, 0x0E, GSW_PCE_EgVLAN_Entry_t)

/**
   \brief This command reads Egress VLAN Treatment entry of PCE for given index (corresponding to FID or SubInterfaceIdGroup (Bits 12-to-8)).
   The egress VLAN Entry can be written using the command \ref GSW_PCE_EG_VLAN_ENTRY_WRITE.

   \param GSW_PCE_EgVLAN_Entry_t Pointer to \ref GSW_PCE_EgVLAN_Entry_t.

   \remarks The function returns an error code in case of out of range VLAN Entries configuration.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_SUCCESS: if successful
   - An error code in case an error occurs
*/
#define GSW_PCE_EG_VLAN_ENTRY_READ	_IOWR(GSW_VLAN_MAGIC, 0x0F, GSW_PCE_EgVLAN_Entry_t)

/*@}*/ /* GSW_IOCTL_VLAN */

/** \addtogroup GSW_IOCTL_QOS */
/*@{*/

/**
   \brief Configures the Ethernet port based traffic class assignment of ingress packets.
   It is used to identify the packet priority and the related egress
   priority queue. For DSCP, the priority to queue assignment is done
   using \ref GSW_QOS_DSCP_CLASS_SET.
   For VLAN, the priority to queue assignment is done
   using \ref GSW_QOS_PCP_CLASS_SET. The current port configuration can be
   read using \ref GSW_QOS_PORT_CFG_GET.

   \param GSW_QoS_portCfg_t Pointer to a
      QOS port priority control configuration \ref GSW_QoS_portCfg_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_QOS_PORT_CFG_SET	_IOW(GSW_QOS_MAGIC, 0x13, GSW_QoS_portCfg_t)

/**
   \brief Read out the current Ethernet port traffic class of ingress packets.
   It is used to identify the packet priority and the related egress
   priority queue. The port configuration can be set
   using \ref GSW_QOS_PORT_CFG_SET.

   \param GSW_QoS_portCfg_t Pointer to a
      QOS port priority control configuration \ref GSW_QoS_portCfg_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_QOS_PORT_CFG_GET	_IOWR(GSW_QOS_MAGIC, 0x12, GSW_QoS_portCfg_t)

/**
   \brief Initialize the QoS 64 DSCP mapping to the switch priority queues.
   This configuration applies for the whole switch device. The table
   configuration can be read using \ref GSW_QOS_DSCP_CLASS_GET.

   \param GSW_QoS_DSCP_ClassCfg_t Pointer to the QoS filter parameters
   \ref GSW_QoS_DSCP_ClassCfg_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_QOS_DSCP_CLASS_SET	_IOW(GSW_QOS_MAGIC, 0x07, GSW_QoS_DSCP_ClassCfg_t)

/**
   \brief Read out the QoS 64 DSCP mapping to the switch priority queues.
   The table configuration can be set using \ref GSW_QOS_DSCP_CLASS_SET.

   \param GSW_QoS_DSCP_ClassCfg_t Pointer to the QoS filter parameters
   \ref GSW_QoS_DSCP_ClassCfg_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_QOS_DSCP_CLASS_GET	_IOWR(GSW_QOS_MAGIC, 0x06, GSW_QoS_DSCP_ClassCfg_t)

/**
   \brief Configure the incoming PCP to traffic class mapping table.
   This configuration applies to the entire switch device.
   The table configuration can be read using \ref GSW_QOS_PCP_CLASS_GET.

   \param GSW_QoS_PCP_ClassCfg_t Pointer to the QoS filter parameters
   \ref GSW_QoS_PCP_ClassCfg_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_QOS_PCP_CLASS_SET	_IOW(GSW_QOS_MAGIC, 0x11, GSW_QoS_PCP_ClassCfg_t)

/**
   \brief Read out the incoming PCP to traffic class mapping table.
   The table configuration can be set using \ref GSW_QOS_PCP_CLASS_SET.

   \param GSW_QoS_PCP_ClassCfg_t Pointer to the QoS filter parameters
   \ref GSW_QoS_PCP_ClassCfg_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_QOS_PCP_CLASS_GET	_IOWR(GSW_QOS_MAGIC, 0x10, GSW_QoS_PCP_ClassCfg_t)

/**
   \brief Configures the DSCP to Drop Precedence assignment mapping table.
   This mapping table is used to identify the switch internally used drop
   precedence based on the DSCP value of the incoming packet.
   The current mapping table configuration can be read
   using \ref GSW_QOS_DSCP_DROP_PRECEDENCE_CFG_GET.

   \param GSW_QoS_DSCP_DropPrecedenceCfg_t Pointer to the QoS
   DSCP drop precedence parameters
   \ref GSW_QoS_DSCP_DropPrecedenceCfg_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_QOS_DSCP_DROP_PRECEDENCE_CFG_SET _IOW(GSW_QOS_MAGIC, 0x0B, GSW_QoS_DSCP_DropPrecedenceCfg_t)

/**
   \brief Read out the current DSCP to Drop Precedence assignment mapping table.
   The table can be configured
   using \ref GSW_QOS_DSCP_DROP_PRECEDENCE_CFG_SET.

   \param GSW_QoS_DSCP_DropPrecedenceCfg_t Pointer to the QoS
   DSCP drop precedence parameters
   \ref GSW_QoS_DSCP_DropPrecedenceCfg_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_QOS_DSCP_DROP_PRECEDENCE_CFG_GET _IOWR(GSW_QOS_MAGIC, 0x0A, GSW_QoS_DSCP_DropPrecedenceCfg_t)

/**
   \brief Port Remarking Configuration. Ingress and Egress remarking options for
   DSCP and PCP. Remarking is done either on the used traffic class or
   the drop precedence.
   The current configuration can be read
   using \ref GSW_QOS_PORT_REMARKING_CFG_GET.

   \param GSW_QoS_portRemarkingCfg_t Pointer to the remarking configuration
   \ref GSW_QoS_portRemarkingCfg_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_QOS_PORT_REMARKING_CFG_SET	_IOW(GSW_QOS_MAGIC, 0x0D, GSW_QoS_portRemarkingCfg_t)

/**
   \brief Read out the Port Remarking Configuration. Ingress and Egress remarking options for
   DSCP and PCP. Remarking is done either on the used traffic class or
   the drop precedence.
   The current configuration can be set
   using \ref GSW_QOS_PORT_REMARKING_CFG_SET.

   \param GSW_QoS_portRemarkingCfg_t Pointer to the QoS filter parameters
   \ref GSW_QoS_portRemarkingCfg_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_QOS_PORT_REMARKING_CFG_GET	_IOWR(GSW_QOS_MAGIC, 0x0C, GSW_QoS_portRemarkingCfg_t)

/**
   \brief Configure the traffic class to DSCP mapping table.
   This table is global and valid for the entire switch device.
   The table can be read using \ref GSW_QOS_CLASS_DSCP_GET.

   \param GSW_QoS_ClassDSCP_Cfg_t Pointer to the DSCP mapping parameter
   \ref GSW_QoS_ClassDSCP_Cfg_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_QOS_CLASS_DSCP_SET	_IOW(GSW_QOS_MAGIC, 0x09, GSW_QoS_ClassDSCP_Cfg_t)

/**
   \brief Read out the current traffic class to DSCP mapping table.
   The table can be written using \ref GSW_QOS_CLASS_DSCP_SET.

   \param GSW_QoS_ClassDSCP_Cfg_t Pointer to the DSCP mapping parameter
   \ref GSW_QoS_ClassDSCP_Cfg_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_QOS_CLASS_DSCP_GET	_IOWR(GSW_QOS_MAGIC, 0x08, GSW_QoS_ClassDSCP_Cfg_t)

/**
   \brief Configure the traffic class to outgoing 802.1P (PCP) priority mapping table.
   This table is global and valid for the entire switch device.
   The table can be read using \ref GSW_QOS_CLASS_PCP_GET.

   \param GSW_QoS_ClassPCP_Cfg_t Pointer to the PCP priority mapping parameter
   \ref GSW_QoS_ClassPCP_Cfg_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_QOS_CLASS_PCP_SET	_IOWR(GSW_QOS_MAGIC, 0x0F, GSW_QoS_ClassPCP_Cfg_t)

/**
   \brief Read out the current traffic class to outgoing 802.1P (PCP) priority mapping table.
   This table is global and valid for the entire switch device.
   The table can be written using \ref GSW_QOS_CLASS_PCP_SET.

   \param GSW_QoS_ClassPCP_Cfg_t Pointer to the PCP priority mapping parameter
   \ref GSW_QoS_ClassPCP_Cfg_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_QOS_CLASS_PCP_GET	_IOWR(GSW_QOS_MAGIC, 0x0E, GSW_QoS_ClassPCP_Cfg_t)

/** \brief This command configures a rate shaper instance with the rate and the
    burst size. This instance can be assigned to QoS queues by
    using \ref GSW_QOS_SHAPER_QUEUE_ASSIGN.
    The total number of available rate shapers can be retrieved by the
    capability list using \ref GSW_CAP_GET.

   \param GSW_QoS_ShaperCfg_t Pointer to the parameters
   structure \ref GSW_QoS_ShaperCfg_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_QOS_SHAPER_CFG_SET	_IOW(GSW_QOS_MAGIC, 0x19, GSW_QoS_ShaperCfg_t)

/** \brief This command retrieves the rate and the burst size configuration of a
    rate shaper instance. A configuration can be modified
    using \ref GSW_QOS_SHAPER_CFG_SET.
    The total number of available rate shapers can be retrieved by the
    capability list using \ref GSW_CAP_GET.

   \param GSW_QoS_ShaperCfg_t Pointer to the parameters
   structure \ref GSW_QoS_ShaperCfg_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_QOS_SHAPER_CFG_GET	_IOWR(GSW_QOS_MAGIC, 0x18, GSW_QoS_ShaperCfg_t)

/** \brief Assign one rate shaper instance to a QoS queue. The function returns with an
    error in case there already are too many shaper instances assigned to a queue.
    The queue instance can be enabled and configured
    using \ref GSW_QOS_SHAPER_CFG_SET.
    To remove a rate shaper instance from a QoS queue,
    please use \ref GSW_QOS_SHAPER_QUEUE_DEASSIGN.
    The total number of available rate shaper instances can be retrieved by the
    capability list using \ref GSW_CAP_GET.

   \param GSW_QoS_ShaperQueue_t Pointer to the parameters
   structure \ref GSW_QoS_ShaperQueue_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_QOS_SHAPER_QUEUE_ASSIGN          _IOW(GSW_QOS_MAGIC, 0x1A, GSW_QoS_ShaperQueue_t)

/** \brief De-assign or Unassign one rate shaper instance from a QoS queue. The function returns
    with an error in case the requested instance is not currently assigned
    to the queue.
    The queue instance can be enabled and configured by
    using \ref GSW_QOS_SHAPER_CFG_SET.
    To assign a rate shaper instance to a QoS queue,
    please use \ref GSW_QOS_SHAPER_QUEUE_ASSIGN.
    The total number of available rate shapers can be retrieved by the
    capability list using \ref GSW_CAP_GET.

   \param GSW_QoS_ShaperQueue_t Pointer to the parameters
   structure \ref GSW_QoS_ShaperQueue_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_QOS_SHAPER_QUEUE_DEASSIGN        _IOW(GSW_QOS_MAGIC, 0x1B, GSW_QoS_ShaperQueue_t)

/** \brief Check whether a rate shaper instance is assigned to the egress queue.
    The egress queue index is the function input parameter.
    The switch API sets the boolean parameter 'bAssigned == 1' in case a
    rate shaper is assigned and then sets 'nRateShaperId' to describe the rater
    shaper instance.
    The parameter 'bAssigned == 0' in case no rate shaper instance
    is currently assigned to the queue instance.
    The commands \ref GSW_QOS_SHAPER_QUEUE_ASSIGN allow a
    rate shaper instance to be assigned, and \ref GSW_QOS_SHAPER_CFG_SET allows
    for configuration of a shaper instance.
    The total number of available rate shapers can be retrieved by the
    capability list using \ref GSW_CAP_GET.

   \param GSW_QoS_ShaperQueueGet_t Pointer to the parameters
   structure \ref GSW_QoS_ShaperQueueGet_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_QOS_SHAPER_QUEUE_GET	_IOWR(GSW_QOS_MAGIC, 0x1C, GSW_QoS_ShaperQueueGet_t)

/** \brief Configures the global WRED drop probability profile and thresholds of the device.
    Given parameters are rounded to the segment size of the HW platform.
    The supported segment size is given by the capability list by
    using \ref GSW_CAP_GET.

   \param GSW_QoS_WRED_Cfg_t Pointer to the parameters
   structure \ref GSW_QoS_WRED_Cfg_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_QOS_WRED_CFG_SET	_IOW(GSW_QOS_MAGIC, 0x20, GSW_QoS_WRED_Cfg_t)

/** \brief Read out the global WRED drop probability profile and thresholds of the device.
    Given parameters are rounded to the segment size of the HW platform.
    The supported segment size is given by the capability list by
    using \ref GSW_CAP_GET.

   \param GSW_QoS_WRED_Cfg_t Pointer to the parameters
   structure \ref GSW_QoS_WRED_Cfg_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_QOS_WRED_CFG_GET	_IOWR(GSW_QOS_MAGIC, 0x1F, GSW_QoS_WRED_Cfg_t)

/** \brief Configures the WRED drop thresholds for a dedicated egress queue.
    Given parameters are rounded to the segment size of the HW platform.
    The supported segment size is given by the capability list by
    using \ref GSW_CAP_GET.
    The command \ref GSW_QOS_WRED_QUEUE_CFG_GET retrieves the current
    configuration.

   \param GSW_QoS_WRED_QueueCfg_t Pointer to the parameters
   structure \ref GSW_QoS_WRED_QueueCfg_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_QOS_WRED_QUEUE_CFG_SET           _IOW(GSW_QOS_MAGIC, 0x22, GSW_QoS_WRED_QueueCfg_t)

/** \brief Read out the WRED drop thresholds for a dedicated egress queue.
    Given parameters are rounded to the segment size of the HW platform.
    The supported segment size is given by the capability list by
    using \ref GSW_CAP_GET.
    The configuration can be changed by
    using \ref GSW_QOS_WRED_QUEUE_CFG_SET.

   \param GSW_QoS_WRED_QueueCfg_t Pointer to the parameters
   structure \ref GSW_QoS_WRED_QueueCfg_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_QOS_WRED_QUEUE_CFG_GET           _IOWR(GSW_QOS_MAGIC, 0x21, GSW_QoS_WRED_QueueCfg_t)

/** \brief Configures the WRED drop thresholds for a dedicated egress port.
    Given parameters are rounded to the segment size of the HW platform.
    The supported segment size is given by the capability list by
    using \ref GSW_CAP_GET.
    The command \ref GSW_QOS_WRED_PORT_CFG_GET retrieves the current
    configuration.

   \param GSW_QoS_WRED_PortCfg_t Pointer to the parameters
   structure \ref GSW_QoS_WRED_PortCfg_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_QOS_WRED_PORT_CFG_SET	_IOW(GSW_QOS_MAGIC, 0x24, GSW_QoS_WRED_PortCfg_t)

/** \brief Read out the WRED drop thresholds for a dedicated egress port.
    Given parameters are rounded to the segment size of the HW platform.
    The supported segment size is given by the capability list by
    using \ref GSW_CAP_GET.
    The configuration can be changed by
    using \ref GSW_QOS_WRED_PORT_CFG_SET.

   \param GSW_QoS_WRED_PortCfg_t Pointer to the parameters
   structure \ref GSW_QoS_WRED_PortCfg_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_QOS_WRED_PORT_CFG_GET	_IOWR(GSW_QOS_MAGIC, 0x23, GSW_QoS_WRED_PortCfg_t)

/** \brief Configures the global flow control thresholds for conforming and non-conforming packets.
    The configured thresholds apply to the global switch segment buffer.
    The current configuration can be retrieved by \ref GSW_QOS_FLOWCTRL_CFG_GET.
    Given parameters are rounded to the segment size of the HW platform.
    The supported segment size is given by the capability list by
    using \ref GSW_CAP_GET.

   \param GSW_QoS_FlowCtrlCfg_t Pointer to the parameters
   structure \ref GSW_QoS_FlowCtrlCfg_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_QOS_FLOWCTRL_CFG_SET	_IOW(GSW_QOS_MAGIC, 0x26, GSW_QoS_FlowCtrlCfg_t)

/** \brief Read out the global flow control thresholds for conforming and non-conforming packets.
    The configured thresholds apply to the global switch segment buffer.
    The configuration can be changed by \ref GSW_QOS_FLOWCTRL_CFG_SET.
    Given parameters are rounded to the segment size of the HW platform.
    The supported segment size is given by the capability list by
    using \ref GSW_CAP_GET.

   \param GSW_QoS_FlowCtrlCfg_t Pointer to the parameters
   structure \ref GSW_QoS_FlowCtrlCfg_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_QOS_FLOWCTRL_CFG_GET	_IOWR(GSW_QOS_MAGIC, 0x25, GSW_QoS_FlowCtrlCfg_t)

/** \brief Configures the ingress port flow control thresholds for occupied buffer segments.
    The current configuration can be retrieved by \ref GSW_QOS_FLOWCTRL_PORT_CFG_GET.
    Given parameters are rounded to the segment size of the HW platform.
    The supported segment size is given by the capability list by
    using \ref GSW_CAP_GET.

   \param GSW_QoS_FlowCtrlPortCfg_t Pointer to the parameters
   structure \ref GSW_QoS_FlowCtrlPortCfg_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_QOS_FLOWCTRL_PORT_CFG_SET        _IOW(GSW_QOS_MAGIC, 0x28, GSW_QoS_FlowCtrlPortCfg_t)

/** \brief Read out the ingress port flow control thresholds for occupied buffer segments.
    The configuration can be changed by \ref GSW_QOS_FLOWCTRL_PORT_CFG_SET.
    Given parameters are rounded to the segment size of the HW platform.
    The supported segment size is given by the capability list by
    using \ref GSW_CAP_GET.

   \param GSW_QoS_FlowCtrlPortCfg_t Pointer to the parameters
   structure \ref GSW_QoS_FlowCtrlPortCfg_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_QOS_FLOWCTRL_PORT_CFG_GET        _IOWR(GSW_QOS_MAGIC, 0x27, GSW_QoS_FlowCtrlPortCfg_t)

/** \brief This command configures the parameters of a rate meter instance.
    This instance can be assigned to an ingress/egress port by
    using \ref GSW_QOS_METER_PORT_ASSIGN. It can also be used by the
    flow classification engine.
    The total number of available rate meters can be retrieved by the
    capability list using \ref GSW_CAP_GET.
    The current configuration of a meter instance can be retrieved
    using \ref GSW_QOS_METER_CFG_GET.

   \param GSW_QoS_meterCfg_t Pointer to the parameters
   structure \ref GSW_QoS_meterCfg_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_QOS_METER_CFG_SET	_IOW(GSW_QOS_MAGIC, 0x02, GSW_QoS_meterCfg_t)

/** \brief Configure the parameters of a rate meter instance.
    This instance can be assigned to an ingress/egress port
    using \ref GSW_QOS_METER_PORT_ASSIGN. It can also be used by the
    flow classification engine.
    The total number of available rate meters can be retrieved by the
    capability list using \ref GSW_CAP_GET.
    The current configuration of a meter instance can be retrieved
    using \ref GSW_QOS_METER_CFG_GET.

   \param GSW_QoS_meterCfg_t Pointer to the parameters
   structure \ref GSW_QoS_meterCfg_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_QOS_METER_CFG_GET	_IOWR(GSW_QOS_MAGIC, 0x01, GSW_QoS_meterCfg_t)

/** \brief Assign a rate meter instance to an ingress and/or egress port.
    A maximum of two meter IDs can be assigned to one single ingress port.
    This meter instance to port assignment can be removed
    using \ref GSW_QOS_METER_PORT_DEASSIGN. A list of all available
    assignments can be read using \ref GSW_QOS_METER_PORT_GET.

   \param GSW_QoS_meterPort_t Pointer to the parameters
   structure \ref GSW_QoS_meterPort_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_QOS_METER_PORT_ASSIGN	_IOW(GSW_QOS_MAGIC, 0x03, GSW_QoS_meterPort_t)

/** \brief Deassign a rate meter instance from an ingress and/or egress port.
    A maximum of two meter IDs can be assigned to one single ingress port.
    The meter instance is given to the command and the port configuration is
    returned. An instance to port assignment can be done
    using \ref GSW_QOS_METER_PORT_ASSIGN. A list of all available
    assignments can be read using \ref GSW_QOS_METER_PORT_GET.

   \param GSW_QoS_meterPort_t Pointer to the parameters
   structure \ref GSW_QoS_meterPort_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_QOS_METER_PORT_DEASSIGN          _IOW(GSW_QOS_MAGIC, 0x04, GSW_QoS_meterPort_t)

/** \brief Reads out all meter instance to port assignments that are done
    using \ref GSW_QOS_METER_PORT_ASSIGN. All assignments are read from an
    internal table where every read call retrieves the next entry of the table.
    Setting the parameter 'bInitial' starts the read operation at the beginning
    of the table. The returned parameter 'bLast' indicates that the last
    element of the table was returned.

   \param GSW_QoS_meterPortGet_t Pointer to the parameters
   structure \ref GSW_QoS_meterPortGet_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_QOS_METER_PORT_GET	_IOWR(GSW_QOS_MAGIC, 0x05, GSW_QoS_meterPortGet_t)

/** \brief This command configures one meter instances for storm control.
    These instances can be used for ingress broadcast-, multicast- and
    unknown unicast- packets. Some platforms support addition of additional meter
    instances for this type of packet.
    Repeated calls of \ref GSW_QOS_STORM_CFG_SET allow addition of
    additional meter instances.
    An assignment can be retrieved using \ref GSW_QOS_STORM_CFG_GET.
    Setting the broadcast, multicast and unknown unicast packets boolean switch to zero
    deletes all metering instance assignments.

   \param GSW_QoS_stormCfg_t Pointer to the parameters
   structure \ref GSW_QoS_stormCfg_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_QOS_STORM_CFG_SET	_IOW(GSW_QOS_MAGIC, 0x1D, GSW_QoS_stormCfg_t)

/** \brief Reads out the current meter instance assignment for storm control. This
    configuration can be modified using \ref GSW_QOS_STORM_CFG_SET.

   \param GSW_QoS_stormCfg_t Pointer to the parameters
   structure \ref GSW_QoS_stormCfg_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_QOS_STORM_CFG_GET	_IOWR(GSW_QOS_MAGIC, 0x1E, GSW_QoS_stormCfg_t)

/** \brief GSWIP-3.0 provides two sets of metering actions support for CPU/MPE traffic.
    This command provides Control Action for triggering metering action from
 CPU users [(CPU or MPE) or (CPU-1 & CPU-2) ]. This control trigger action performs metering and returns output color.

   \param GSW_QoS_mtrAction_t Pointer to the Metering Action parameters
   structure \ref GSW_QoS_mtrAction_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_QOS_METER_ACT	_IOWR(GSW_QOS_MAGIC, 0x2B, GSW_QoS_mtrAction_t)

/** \brief This configuration decides how the egress queues, attached to a single port,
    are scheduled to transmit the queued Ethernet packets.
    The configuration differentiates between 'Strict Priority' and
    'weighted fair queuing'. This applies when multiple egress queues are
    assigned to an Ethernet port.
    Using the WFQ feature on a port requires the configuration of weights on all
    given queues that are assigned to that port.
    Strict Priority means that no dedicated weight is configured and the
    queue can transmit following its priority status.
    The given configuration can be read out
    using \ref GSW_QOS_SCHEDULER_CFG_GET.

   \param GSW_QoS_schedulerCfg_t Pointer to the parameters
   structure \ref GSW_QoS_schedulerCfg_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_QOS_SCHEDULER_CFG_SET	_IOW(GSW_QOS_MAGIC, 0x17, GSW_QoS_schedulerCfg_t)

/** \brief Read out the current scheduler configuration of a given egress port. This
    configuration can be modified
    using \ref GSW_QOS_SCHEDULER_CFG_SET.

   \param GSW_QoS_schedulerCfg_t Pointer to the parameters
   structure \ref GSW_QoS_schedulerCfg_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_QOS_SCHEDULER_CFG_GET	_IOWR(GSW_QOS_MAGIC, 0x16, GSW_QoS_schedulerCfg_t)

/** \brief Sets the Queue ID for one traffic class of one port.
    The total amount of supported ports, queues and traffic classes can be
    retrieved from the capability list using \ref GSW_CAP_GET.
    Please note that the device comes along with a
    default configuration and assignment.

   \param GSW_QoS_queuePort_t Pointer to the parameters
   structure \ref GSW_QoS_queuePort_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_QOS_QUEUE_PORT_SET	_IOW(GSW_QOS_MAGIC, 0x15, GSW_QoS_queuePort_t)

/** \brief Read out the traffic class and port assignment done
    using \ref GSW_QOS_QUEUE_PORT_SET.
    Please note that the device comes along with a
    default configuration and assignment.

   \param GSW_QoS_queuePort_t Pointer to the parameters
   structure \ref GSW_QoS_queuePort_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_QOS_QUEUE_PORT_GET	_IOWR(GSW_QOS_MAGIC, 0x14, GSW_QoS_queuePort_t)
/** \brief Configure the egress queue buffer reservation.
    WRED GREEN packets are never dropped by any WRED algorithm (queue,
    port or global buffer level) in case they are below this reservation threshold.
    The amount of reserved segments cannot be occupied by other queues of the switch.
    The egress queue related configuration can be retrieved by
    calling \ref GSW_QOS_QUEUE_BUFFER_RESERVE_CFG_GET.

    \remarks
    The command \ref GSW_QOS_QUEUE_PORT_SET allows to assign egress queue to ports with related traffic classes.

   \param GSW_QoS_QueueBufferReserveCfg_t Pointer to the parameters
   structure \ref GSW_QoS_QueueBufferReserveCfg_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_QOS_QUEUE_BUFFER_RESERVE_CFG_SET _IOW(GSW_QOS_MAGIC, 0x2A, GSW_QoS_QueueBufferReserveCfg_t)

/** \brief Read out the egress queue specific buffer reservation.
    Configuration can be read by \ref GSW_QOS_QUEUE_BUFFER_RESERVE_CFG_SET.

   \param GSW_QoS_QueueBufferReserveCfg_t Pointer to the parameters
   structure \ref GSW_QoS_QueueBufferReserveCfg_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_QOS_QUEUE_BUFFER_RESERVE_CFG_GET _IOWR(GSW_QOS_MAGIC, 0x29, GSW_QoS_QueueBufferReserveCfg_t)
/**
   \brief Configure the egress port related traffic class to STAG VLAN 802.1P (PCP) priority mapping table.
   One table is given for each egress port.
   The table can be read using \ref GSW_QOS_SVLAN_CLASS_PCP_PORT_GET.

   \param GSW_QoS_SVLAN_ClassPCP_PortCfg_t Pointer to the PCP priority mapping parameter
   \ref GSW_QoS_SVLAN_ClassPCP_PortCfg_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_QOS_SVLAN_CLASS_PCP_PORT_SET	_IOWR(GSW_QOS_MAGIC, 0x37, GSW_QoS_SVLAN_ClassPCP_PortCfg_t)

/**
   \brief Read out the current egress port related traffic class to 802.1P (PCP) priority mapping table.
   One table is given for each egress port.
   The table can be written using \ref GSW_QOS_SVLAN_CLASS_PCP_PORT_SET.

   \param GSW_QoS_SVLAN_ClassPCP_PortCfg_t Pointer to the PCP priority mapping parameter
   \ref GSW_QoS_SVLAN_ClassPCP_PortCfg_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_QOS_SVLAN_CLASS_PCP_PORT_GET	_IOWR(GSW_QOS_MAGIC, 0x36, GSW_QoS_SVLAN_ClassPCP_PortCfg_t)

/**
   \brief Configure the PCP to traffic class mapping table.
   This configuration applies to the entire switch device.
   The table configuration can be read using \ref GSW_QOS_SVLAN_PCP_CLASS_GET.

   \param GSW_QoS_SVLAN_PCP_ClassCfg_t Pointer to the QoS filter parameters
   \ref GSW_QoS_SVLAN_PCP_ClassCfg_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_QOS_SVLAN_PCP_CLASS_SET	_IOW(GSW_QOS_MAGIC, 0x39, GSW_QoS_SVLAN_PCP_ClassCfg_t)

/**
   \brief Read out the PCP to traffic class mapping table.
   The table configuration can be set using \ref GSW_QOS_SVLAN_PCP_CLASS_SET.

   \param GSW_QoS_SVLAN_PCP_ClassCfg_t Pointer to the QoS filter parameters
   \ref GSW_QoS_SVLAN_PCP_ClassCfg_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_QOS_SVLAN_PCP_CLASS_GET	_IOWR(GSW_QOS_MAGIC, 0x38, GSW_QoS_SVLAN_PCP_ClassCfg_t)

/*@}*/ /* GSW_IOCTL_QOS */

/** \addtogroup GSW_IOCTL_MULTICAST */
/*@{*/

/**
   \brief Configure the switch multicast configuration. The currently used
   configuration can be read using \ref GSW_MULTICAST_SNOOP_CFG_GET.

   \param GSW_multicastSnoopCfg_t Pointer to the
   multicast configuration \ref GSW_multicastSnoopCfg_t.

   \remarks IGMP/MLD snooping is disabled when
   'eIGMP_Mode = GSW_MULTICAST_SNOOP_MODE_SNOOPFORWARD'.
   Then all other structure parameters are unused.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_MULTICAST_SNOOP_CFG_SET	_IOW(GSW_MULTICAST_MAGIC, 0x05, GSW_multicastSnoopCfg_t)

/**
   \brief Read out the current switch multicast configuration.
   The configuration can be set using \ref GSW_MULTICAST_SNOOP_CFG_SET.

   \param GSW_multicastSnoopCfg_t Pointer to the
   multicast configuration \ref GSW_multicastSnoopCfg_t.

   \remarks IGMP/MLD snooping is disabled when
   'eIGMP_Mode = GSW_MULTICAST_SNOOP_MODE_SNOOPFORWARD'.
   Then all other structure parameters are unused.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_MULTICAST_SNOOP_CFG_GET	_IOWR(GSW_MULTICAST_MAGIC, 0x04, GSW_multicastSnoopCfg_t)

/**
   \brief Add static router port to the switch hardware multicast table.
   These added router ports will not be removed by the router port learning aging process.
   The router port learning is enabled over the parameter 'bLearningRouter'
   over the \ref GSW_MULTICAST_SNOOP_CFG_GET command.
   Router port learning and static added entries can both be used together.
   In case of a software IGMP stack/daemon environment, the router port learning does
   not have to be configured on the switch hardware. Instead the router port
   management is handled by the IGMP stack/daemon.
   A port can be removed using \ref GSW_MULTICAST_ROUTER_PORT_REMOVE.

   \param GSW_multicastRouter_t Pointer to \ref GSW_multicastRouter_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_MULTICAST_ROUTER_PORT_ADD	_IOW(GSW_MULTICAST_MAGIC, 0x01, GSW_multicastRouter_t)

/**
   \brief Remove an Ethernet router port from the switch hardware multicast table.
   A port can be added using \ref GSW_MULTICAST_ROUTER_PORT_ADD.

   \param GSW_multicastRouter_t Pointer to \ref GSW_multicastRouter_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs (e.g. Ethernet port parameter out of range)
*/
#define GSW_MULTICAST_ROUTER_PORT_REMOVE	_IOW(GSW_MULTICAST_MAGIC, 0x03, GSW_multicastRouter_t)

/**
   \brief Check if a port has been selected as a router port, either by automatic learning or by manual setting.
   A port can be added using \ref GSW_MULTICAST_ROUTER_PORT_ADD.
   A port can be removed again using \ref GSW_MULTICAST_ROUTER_PORT_REMOVE.

   \param GSW_multicastRouterRead_t Pointer to \ref GSW_multicastRouterRead_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs (e.g. Ethernet port parameter out of range)
*/
#define GSW_MULTICAST_ROUTER_PORT_READ	_IOWR(GSW_MULTICAST_MAGIC, 0x02, GSW_multicastRouterRead_t)

/**
   \brief Adds a multicast group configuration to the multicast table.
   No new entry is added in case this multicast group already
   exists in the table. This commands adds a host member to
   the multicast group.
   A member can be removed again using \ref GSW_MULTICAST_TABLE_ENTRY_REMOVE.

   \param GSW_multicastTable_t Pointer
      to \ref GSW_multicastTable_t.

   \remarks The Source IP parameter is ignored in case IGMPv3 support is
      not enabled in the hardware.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_MULTICAST_TABLE_ENTRY_ADD	_IOW(GSW_MULTICAST_MAGIC, 0x06, GSW_multicastTable_t)

/**
   \brief Remove an host member from a multicast group. The multicast group entry
   is completely removed from the multicast table in case it has no
   host member port left.
   Group members can be added using \ref GSW_MULTICAST_TABLE_ENTRY_ADD.

   \param GSW_multicastTable_t Pointer
      to \ref GSW_multicastTable_t.

   \remarks The Source IP parameter is ignored in case IGMPv3 support is
      not enabled in the hardware.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_MULTICAST_TABLE_ENTRY_REMOVE	_IOWR(GSW_MULTICAST_MAGIC, 0x08, GSW_multicastTable_t)

/**
   \brief Read out the multicast membership table that is located inside the switch
   hardware. The 'bInitial' parameter restarts the read operation at the beginning of
   the table. Every following \ref GSW_MULTICAST_TABLE_ENTRY_READ call reads out
   the next found entry. The 'bLast' parameter is set by the switch API in case
   the last entry of the table is reached.

   \param GSW_multicastTableRead_t Pointer
      to \ref GSW_multicastTableRead_t.

   \remarks The 'bInitial' parameter is reset during the read operation.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_MULTICAST_TABLE_ENTRY_READ	_IOWR(GSW_MULTICAST_MAGIC, 0x07, GSW_multicastTableRead_t)

/*@}*/ /* GSW_IOCTL_MULTICAST */

/** \addtogroup GSW_IOCTL_OAM */
/*@{*/

/** \brief Hardware Initialization. This command should be called right after the
    Switch API software module is initialized and loaded.
    It accesses the hardware platform, retrieving platform capabilities and
    performing the first basic configuration.

   \param GSW_HW_Init_t Pointer to pre-allocated initialization structure
   \ref GSW_HW_Init_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_HW_INIT	_IOW(GSW_COMMON_MAGIC, 0x0E, GSW_HW_Init_t)

/**
   \brief Retrieve the version string of the currently version index. The returned
   string format might vary between the device platforms used. This
   means that the version information cannot be compared between different
   device platforms.
   All returned version information is in the form of zero-terminated character strings.
   The returned strings are empty ('') in case the given version
   index is out of range.

   \param GSW_version_t* The parameter points to a
   \ref GSW_version_t structure.

   \return Returns value as follows:
   - GSW_statusOk: if successful
   - LTQ_ERROR: in case of an error

   \code
   GSW_version_t param;
   int fd;

   memset (&param, 0, sizeof(GSW_version_t));

   while (1)
   {
      if (ioctl(fd, GSW_VERSION_GET, (int) &param)
         != GSW_statusOk)
      {
         printf("ERROR: SW-API version request failed!\n);
         return LTQ_ERROR;
      }

      if ((strlen(param.cName) == 0) || (strlen(param.cVersion) == 0))
         // No more version entries found
         break;

      printf("%s version: %s", param.cName, param.cVersion);
      param.nId++;
   }

   return GSW_statusOk;
   \endcode
*/
#define GSW_VERSION_GET	_IOWR(GSW_COMMON_MAGIC, 0x0A, GSW_version_t)

/** \brief This service returns the capability referenced by the provided index
    (zero-based counting index value). The Switch API uses the index to return
    the capability parameter from an internal list. For instance,
    the capability list contains information about the amount of supported
    features like number of supported VLAN groups or MAC table entries.
    The command returns zero-length strings ('') in case the
    requested index number is out of range.

   \param GSW_cap_t Pointer to pre-allocated capability
      list structure \ref GSW_cap_t.
      The switch API implementation fills out the structure with the supported
      features, based on the provided 'nCapType' parameter.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs

   \code
   GSW_cap_t param;
   int fd;
   int i;

   // Open SWAPI file descriptor *
   fd = open("/dev/switchapi/1", O_RDWR, 0x644);

   for (i = 0; i < GSW_CAP_TYPE_LAST, i++)
   {
      memset(&param, 0, sizeof(param));
      param.nCapType = i;
      //Get the cap list
      if (ioctl(fd, GSW_CAP_GET, (int) &param) == LTQ_ERROR)
         return LTQ_ERROR;

      printf("%s: %d\n", param.cDesc, param.nCap);
   }

   // Close open fd
   close(fd);
   return GSW_statusOk;
   \endcode
*/
#define GSW_CAP_GET	_IOWR(GSW_COMMON_MAGIC, 0x0B, GSW_cap_t)

/**
   \brief Modify the switch configuration.
   The configuration can be read using \ref GSW_CFG_GET.
   The switch can be enabled using \ref GSW_ENABLE.

   \param GSW_cfg_t Pointer to an \ref GSW_cfg_t structure.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_CFG_SET	_IOW(GSW_COMMON_MAGIC, 0x0D, GSW_cfg_t)

/**
   \brief Read the global switch configuration.
   This configuration can be set using \ref GSW_CFG_SET.

   \param GSW_cfg_t Pointer to an \ref GSW_cfg_t structure.
      The structure is filled out by the switch implementation.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_CFG_GET	_IOWR(GSW_COMMON_MAGIC, 0x0C, GSW_cfg_t)

/**
   \brief Enables the whole switch. The switch device is enabled with the default
   configuration in case no other configuration is applied.
   The switch can be disabled using the \ref GSW_DISABLE command

   \param void This command does not require any parameter structure

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_ENABLE	_IO(GSW_COMMON_MAGIC, 0x07)

/**
   \brief Disables the whole switch.
   The switch can be enabled using the \ref GSW_ENABLE command

   \param void This command does not require any parameter structure

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_DISABLE	_IO(GSW_COMMON_MAGIC, 0x09)

/**
   \brief Read out the current Ethernet port configuration.

   \param GSW_portCfg_t Pointer to a port configuration
   \ref GSW_portCfg_t structure to fill out by the driver.
   The parameter 'nPortId' tells the driver which port parameter is requested.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_PORT_CFG_GET	_IOWR(GSW_COMMON_MAGIC, 0x20, GSW_portCfg_t)

/**
   \brief Set the Ethernet port configuration.

   \param GSW_portCfg_t Pointer to an \ref GSW_portCfg_t structure
   to configure the switch port hardware.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_PORT_CFG_SET	_IOW(GSW_COMMON_MAGIC, 0x21, GSW_portCfg_t)

/**
   \brief Defines one port that is directly connected to the software running on a CPU.
   This allows for the redirecting of protocol-specific packets to the CPU port and
   special packet treatment when sent by the CPU.
   If the CPU port cannot be set, the function returns an error code.

   \param GSW_CPU_PortCfg_t Pointer to
      an \ref GSW_CPU_PortCfg_t structure.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_CPU_PORT_CFG_SET	_IOW(GSW_COMMON_MAGIC, 0x18, GSW_CPU_PortCfg_t)

/**
   \brief Get the port that is directly connected to the software running on a CPU and defined as
   CPU port. This port assignment can be set using \ref GSW_CPU_PORT_CFG_SET
   if it is not fixed and defined by the switch device architecture.

   \param GSW_CPU_PortCfg_t Pointer to
      an \ref GSW_CPU_PortCfg_t structure.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_CPU_PORT_CFG_GET	_IOWR(GSW_COMMON_MAGIC, 0x17, GSW_CPU_PortCfg_t)

/**
   \brief Configure an additional CPU port configuration. This configuration applies to
   devices where the CPU port is fixed to one dedicated port.

   \param GSW_CPU_PortExtendCfg_t Pointer to
      an \ref GSW_CPU_PortExtendCfg_t structure.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_CPU_PORT_EXTEND_CFG_SET	_IOW(GSW_COMMON_MAGIC, 0x1A, GSW_CPU_PortExtendCfg_t)

/**
   \brief Reads out additional CPU port configuration. This configuration applies to
   devices where the CPU port is fixed to one dedicated port.

   \param GSW_CPU_PortExtendCfg_t Pointer to
      an \ref GSW_CPU_PortExtendCfg_t structure.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs

*/
#define GSW_CPU_PORT_EXTEND_CFG_GET	_IOWR(GSW_COMMON_MAGIC, 0x19, GSW_CPU_PortExtendCfg_t)

/**
   \brief Read out the Ethernet port's speed, link status, and flow control status.
   The information for one single port 'nPortId' is returned.
   An error code is returned if the selected port does not exist.

   \param GSW_portLinkCfg_t Pointer to
      an \ref GSW_portLinkCfg_t structure to read out the port status.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_PORT_LINK_CFG_GET	_IOWR(GSW_COMMON_MAGIC, 0x0F, GSW_portLinkCfg_t)

/**
   \brief Set the Ethernet port link, speed status and flow control status.
   The configuration applies to a single port 'nPortId'.

   \param GSW_portLinkCfg_t Pointer to
      an \ref GSW_portLinkCfg_t structure to set the port configuration.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_PORT_LINK_CFG_SET	_IOW(GSW_COMMON_MAGIC, 0x10, GSW_portLinkCfg_t)

/**
   \brief Configure the RGMII clocking parameter in case the Ethernet port is configured in RGMII mode.
   The configuration can be read by calling \ref GSW_PORT_RGMII_CLK_CFG_GET.
   It applies to a single port 'nPortId'.

   \param GSW_portRGMII_ClkCfg_t Pointer to
      an \ref GSW_portRGMII_ClkCfg_t structure to set the port configuration.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_PORT_RGMII_CLK_CFG_SET	_IOW(GSW_COMMON_MAGIC, 0x14, GSW_portRGMII_ClkCfg_t)

/**
   \brief Read the RGMII clocking parameter in case the Ethernet port is configured in RGMII mode.
   The configuration can be set by calling \ref GSW_PORT_RGMII_CLK_CFG_SET.
   It applies to a single port 'nPortId'.

   \param GSW_portRGMII_ClkCfg_t Pointer to
      an \ref GSW_portRGMII_ClkCfg_t structure to set the port configuration.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_PORT_RGMII_CLK_CFG_GET	_IOWR(GSW_COMMON_MAGIC, 0x13, GSW_portRGMII_ClkCfg_t)

/**
   \brief Check whether the Ethernet switch hardware has detected an Ethernet PHY connected
   to the given Ethernet port 'nPortId'.

   \param GSW_portPHY_Query_t Pointer to
      an \ref GSW_portPHY_Query_t structure to set the port configuration.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_PORT_PHY_QUERY	_IOWR(GSW_COMMON_MAGIC, 0x12, GSW_portPHY_Query_t)

/**
   \brief Read out the MDIO device address of an Ethernet PHY that is connected to
   an Ethernet port. This device address is useful when accessing PHY
   registers using the commands \ref GSW_MDIO_DATA_WRITE,
   \ref GSW_MDIO_DATA_READ, \ref GSW_MMD_DATA_WRITE
        and \ref GSW_MMD_DATA_READ.

   \param GSW_portPHY_Addr_t Pointer to \ref GSW_portPHY_Addr_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_PORT_PHY_ADDR_GET	_IOWR(GSW_COMMON_MAGIC, 0x11, GSW_portPHY_Addr_t)

/**
   \brief Ingress and egress packets of one specific Ethernet port can be redirected to
   the CPU port. The ingress and egress packet redirection can be configured
   individually. This command reads out the current configuration of a
   dedicated port. A new configuration can be applied
   by calling \ref GSW_PORT_REDIRECT_SET.

   \param GSW_portRedirectCfg_t Pointer
          to \ref GSW_portRedirectCfg_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.
   \remarks Not all hardware platforms support this feature. The function
            returns an error if this feature is not supported.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_PORT_REDIRECT_GET	_IOWR(GSW_COMMON_MAGIC, 0x15, GSW_portRedirectCfg_t)

/**
   \brief Select ingress and egress packets of one specific Ethernet port that can be
   redirected to a port that is configured as the 'CPU port'. The ingress and
   egress packet direction can be configured individually.
   The packet filter of the original port still
   applies to the packet (for example, MAC address learning is done for the
   selected port and not for the CPU port).
   On CPU port side, no additional learning, forwarding look up,
   VLAN processing and queue selection is performed for redirected packets.
   Depending on the hardware platform used, the CPU port has to be set in
   advance using \ref GSW_CPU_PORT_CFG_SET.
   The currently used configuration can be read
   using \ref GSW_PORT_REDIRECT_GET.

   \param GSW_portRedirectCfg_t Pointer
          to \ref GSW_portRedirectCfg_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.
   \remarks Not all hardware platforms support this feature. The function
            returns an error if this feature is not supported.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_PORT_REDIRECT_SET	_IOW(GSW_COMMON_MAGIC, 0x16, GSW_portRedirectCfg_t)

/**
   \brief Reads out the current monitor options for a
   dedicated Ethernet port. This configuration can be set
   using \ref GSW_MONITOR_PORT_CFG_SET.

   \param GSW_monitorPortCfg_t Pointer
          to \ref GSW_monitorPortCfg_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_MONITOR_PORT_CFG_GET	_IOWR(GSW_COMMON_MAGIC, 0x1B, GSW_monitorPortCfg_t)

/**
   \brief Configures the monitor options for a
   dedicated Ethernet port. This current configuration can be read back
   using \ref GSW_MONITOR_PORT_CFG_GET.

   \param GSW_monitorPortCfg_t Pointer
          to \ref GSW_monitorPortCfg_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_MONITOR_PORT_CFG_SET	_IOW(GSW_COMMON_MAGIC, 0x1C, GSW_monitorPortCfg_t)



/**
   \brief Read the MDIO interface configuration.
   The parameters can be modified using \ref GSW_MDIO_CFG_SET.

   \param GSW_MDIO_cfg_t Pointer to \ref GSW_MDIO_cfg_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/


#define GSW_MDIO_CFG_GET	_IOWR(GSW_COMMON_MAGIC, 0x22, GSW_MDIO_cfg_t)

/**
   \brief Set the MDIO interface configuration.
   The parameters can be read using \ref GSW_MDIO_CFG_GET.
   The given frequency is rounded off to fitting to the hardware support.
   \ref GSW_MDIO_CFG_GET will return the exact programmed (rounded) frequency value.

   \param GSW_MDIO_cfg_t Pointer to \ref GSW_MDIO_cfg_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_MDIO_CFG_SET	_IOW(GSW_COMMON_MAGIC, 0x23, GSW_MDIO_cfg_t)

/**
   \brief Read data from the MDIO Interface of the switch device. This function allows
   various kinds of information to be read out for any attached device by register and
   device addressing.
   The 'nData' value (\ref GSW_MDIO_data_t) contains the read
   device register.
   A write operation can be done using \ref GSW_MDIO_DATA_WRITE.

   \param GSW_MDIO_data_t Pointer to \ref GSW_MDIO_data_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_MDIO_DATA_READ	_IOWR(GSW_COMMON_MAGIC, 0x24, GSW_MDIO_data_t)

/**
   \brief Write data to the MDIO Interface of the switch device. This function allows
   for configuration of any attached device by register and device addressing.
   This applies to external and internal Ethernet PHYs as well.
   The 'nData' value (\ref GSW_MDIO_data_t) is directly written to the
   device register.
   A read operation can be performed using \ref GSW_MDIO_DATA_READ.

   \param GSW_MDIO_data_t Pointer to \ref GSW_MDIO_data_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs

   \code
   GSW_MDIO_data_t         phy;

   // access the device 2
   phy.nAddressDev = 2;
   // PHY register 0
   phy.nAddressReg = 0;
   // copy the helper PHY register union to the data field to configure
   phy.nData = 0x1235;

   if (ioctl(fd, GSW_MDIO_DATA_WRITE, (int)&phy))
      return LTQ_ERROR;

   // access the device 5
   phy.nAddressDev = 5;
   // Device specific register 20
   phy.nAddressReg = 20;
   // set the data field to configure
   phy.nData = 0x1234;

   if (ioctl(fd, GSW_MDIO_DATA_WRITE, (int)&phy))
      return LTQ_ERROR;

   return GSW_statusOk;
   \endcode
*/
#define GSW_MDIO_DATA_WRITE	_IOW(GSW_COMMON_MAGIC, 0x25, GSW_MDIO_data_t)

/**
   \brief Read MMD Ethernet PHY register over the MDIO Interface
   attached to the switch device. This function allows various
   kinds of information to be read out for any attached device
   by register and device addressing. The 'nData' value (\ref
   GSW_MMD_data_t) contains the read MMD device register.
   A write operation can be done using \ref
   GSW_MMD_DATA_WRITE.

   \param GSW_MMD_data_t Pointer to
          \ref GSW_MMD_data_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_MMD_DATA_READ	_IOWR(GSW_COMMON_MAGIC, 0x26, GSW_MMD_data_t)

/**
   \brief Write MMD Ethernet PHY register over the MDIO Interface
   attached to the switch device. This function allows
   configuration of any attached device by MMD register and
   device addressing. This applies to external and internal
   Ethernet PHYs as well. The 'nData' value (\ref
   GSW_MMD_data_t) is directly written to the device
   register. A read operation can be performed using \ref
   GSW_MMD_DATA_READ.

   \param GSW_MMD_data_t Pointer to \ref
          GSW_MMD_data_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_MMD_DATA_WRITE	_IOW(GSW_COMMON_MAGIC, 0x27, GSW_MMD_data_t)


/**
   \brief Set the Wake-on-LAN configuration.
   The parameters can be read using \ref GSW_WOL_CFG_GET.

   \param GSW_WoL_Cfg_t Pointer to \ref GSW_WoL_Cfg_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_WOL_CFG_SET	_IOW(GSW_WOL_MAGIC, 0x02, GSW_WoL_Cfg_t)

/**
   \brief Read the Wake-on-LAN configuration.
   The parameters can be modified using \ref GSW_WOL_CFG_SET.

   \param GSW_WoL_Cfg_t Pointer to \ref GSW_WoL_Cfg_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_WOL_CFG_GET	_IOWR(GSW_WOL_MAGIC, 0x01, GSW_WoL_Cfg_t)

/**
   \brief Set the current Wake-On-LAN status for a dedicated port. The
   Wake-On-LAN specific parameter can be configured
   using \ref GSW_WOL_CFG_SET.

   \param GSW_WoL_PortCfg_t Pointer to \ref GSW_WoL_PortCfg_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_WOL_PORT_CFG_SET	_IOW(GSW_WOL_MAGIC, 0x04, GSW_WoL_PortCfg_t)

/**
   \brief Read out the current status of the Wake-On-LAN feature
   on a dedicated port. This status can be changed
   using \ref GSW_WOL_PORT_CFG_SET.
   The Wake-On-LAN specific parameter can be configured
   using \ref GSW_WOL_CFG_SET.

   \param GSW_WoL_PortCfg_t Pointer to \ref GSW_WoL_PortCfg_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_WOL_PORT_CFG_GET	_IOWR(GSW_WOL_MAGIC, 0x03, GSW_WoL_PortCfg_t)

/*@}*/ /* GSW_IOCTL_OAM */

/** \addtogroup GSW_IOCTL_RMON */
/*@{*/

/**
   \brief Read out the Ethernet port statistic counter (RMON counter).
   The zero-based 'nPortId' structure element describes the physical switch
   port for the requested statistic information.

   \param GSW_RMON_Port_cnt_t  Pointer to pre-allocated
   \ref GSW_RMON_Port_cnt_t structure. The structure element 'nPortId' is
   an input parameter that describes from which port to read the RMON counter.
   All remaining structure elements are filled with the counter values.

   \remarks The function returns an error in case the given 'nPortId' is
   out of range.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_RMON_PORT_GET	_IOWR(GSW_RMON_MAGIC, 0x1, GSW_RMON_Port_cnt_t)

/**
   \brief Clears all or specific identifier (e.g. Port Id or Meter Id) Statistic counter (RMON counter).

   \param GSW_RMON_clear_t  Pointer to a pre-allocated
   \ref GSW_RMON_clear_t structure. The structure element 'nRmonId' is
   an input parameter stating on which identifier to clear RMON counters.

   \remarks The function returns an error in case the given 'nRmonId' is
   out of range for given 'nRmonType'

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_RMON_CLEAR	_IOW(GSW_RMON_MAGIC, 0x07, GSW_RMON_clear_t)

/**
   \brief Clears all or specific identifier (e.g. Port Id or Meter Id) Statistic counter (RMON counter).

   \param GSW_RMON_extendGet_t  Pointer to a pre-allocated
   \ref GSW_RMON_clear_t structure. The structure element 'nRmonId' is
   an input parameter stating on which identifier to clear RMON counters.

   \remarks The function returns an error in case the given 'nRmonId' is
   out of range for given 'nRmonType'

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_RMON_TFLOW_CLEAR _IOW(GSW_RMON_MAGIC, 0x0A, GSW_RMON_extendGet_t)

/**
   \brief Sets TFLOW counter mode type.

   \param GSW_TflowCmodeConf_t  Pointer to a pre-allocated
   \ref GSW_RMON_clear_t structure.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_TFLOW_COUNT_MODE_SET _IOW(GSW_RMON_MAGIC, 0x0B, GSW_TflowCmodeConf_t)

/**
   \brief Sets TFLOW counter mode type.

   \param GSW_TflowCmodeConf_t  Pointer to a pre-allocated
   \ref GSW_RMON_clear_t structure.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_TFLOW_COUNT_MODE_GET _IOW(GSW_RMON_MAGIC, 0x0C, GSW_TflowCmodeConf_t)

/**
   \brief Configures a Traffic Statistic Counter (RMON counter).

   \param GSW_RMON_mode_t  Pointer to a pre-allocated
   \ref GSW_RMON_mode_t structure. The structure elements 'eRmonType' and 'eRmonMode' are input parameters to set RMON counting mode to bytes or Packet based.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/

#define GSW_RMON_MODE_SET	_IOW(GSW_RMON_MAGIC, 0x02, GSW_RMON_mode_t)

/**
   \brief Read out an Interface statistic counter (RMON counter).

   \param GSW_RMON_If_cnt_t  Pointer to pre-allocated Statistics placeholder
   \ref GSW_RMON_If_cnt_t structure. The Interface Counter Index element 'nIfId' is an input parameter that describes which Interface RMON counters to be read.
   All remaining structure elements will be returned filled with the counter values.

   \remarks The function returns an error in case the given 'nIfId' is
   out of range.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_RMON_IF_GET	_IOWR(GSW_RMON_MAGIC, 0x05, GSW_RMON_If_cnt_t)

/**
   \brief Read out the Redirected statistic counter (RMON counter).

   \param GSW_RMON_Redirect_cnt_t  Pointer to pre-allocated stats placeholder
   \ref GSW_RMON_Redirect_cnt_t structure. There is no input and all combined redirected counters are returned back.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_RMON_REDIRECT_GET	_IOWR(GSW_RMON_MAGIC, 0x04, GSW_RMON_Redirect_cnt_t)

/**
   \brief Read out the Routed statistic counter (RMON counter) on Routed port. These counters are applicable to PAE only.

   \param GSW_RMON_Route_cnt_t  Pointer to pre-allocated stats placeholder
   \ref GSW_RMON_Route_cnt_t structure. The Redirected Port Id element 'nRoutedPortId' is an input parameter for which  RMON counters to be read.
   All remaining structure elements will be returned filled with the counter values.

   \remarks The function returns an error in case the given 'nRoutedPortId' is
   out of range.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_RMON_ROUTE_GET	_IOWR(GSW_RMON_MAGIC, 0x06, GSW_RMON_Route_cnt_t)

/**
   \brief Read out the Meter Instance statistic counter (RMON counter).
   The zero-based 'nMeterId' structure element describes the Meter Identifier
   instance for the requested statistic information.

   \param GSW_RMON_Meter_cnt_t  Pointer to pre-allocated stats placeholder
   \ref GSW_RMON_Meter_cnt_t structure. The structure element 'nMeterId' is
   an input parameter that describes from which Meter to read the RMON counter.
   All remaining structure elements are filled with the counter values.

   \remarks The function returns an error in case the given 'nMeterId' is
   out of range.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/

#define GSW_RMON_METER_GET	_IOWR(GSW_RMON_MAGIC, 0x03, GSW_RMON_Meter_cnt_t)

/*@}*/ /* GSW_IOCTL_RMON */
/** \addtogroup GSW_IOCTL_PMAC */
/*@{*/

/**
   \brief Configures the Backpressure Mapping Table for PMAC.
   It is used to configure backpressure mapping table between Tx Queues for Egress and Rx Ports for Ingress congestion on given DMA channel.
   The Backpressure mapping can also be read using \ref GSW_PMAC_BM_CFG_GET.

   \param GSW_PMAC_BM_Cfg_t Pointer to a
      Backpressure mapping configuration \ref GSW_PMAC_BM_Cfg_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_PMAC_BM_CFG_SET	_IOW(GSW_PMAC_MAGIC, 0x04, GSW_PMAC_BM_Cfg_t)

/**
   \brief Queries the Backpressure Mapping Table for PMAC.
   It is used to read backpressure mapping table between Tx Queues for Egress and Rx Ports for Ingress congestion on given DMA channel.
   The mapping config can be written using \ref GSW_PMAC_BM_CFG_SET.

   \param GSW_PMAC_BM_Cfg_t Pointer to a
      Backpressure mapping configuration \ref GSW_PMAC_BM_Cfg_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_PMAC_BM_CFG_GET	_IOWR(GSW_PMAC_MAGIC, 0x05, GSW_PMAC_BM_Cfg_t)

/**
   \brief Configures the Ingress PMAC Attributes Config for Receive DMA Channel.
   It is used to configure ingress attributes on given Receive DMA channel.
   The PMAC Ingress config can also be read using \ref GSW_PMAC_IG_CFG_GET.

   \param GSW_PMAC_Ig_Cfg_t Pointer to a
      Ingress PMAC Attributes configuration \ref GSW_PMAC_Ig_Cfg_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_PMAC_IG_CFG_SET	_IOWR(GSW_PMAC_MAGIC, 0x06, GSW_PMAC_Ig_Cfg_t)

/**
   \brief Queries the Ingress PMAC Attributes Config for given receive DMA channel.
   It is used to read ingress PMAC attributes config on given DMA Rx channel.
   The PMAC Ingress config can also be set using \ref GSW_PMAC_IG_CFG_SET.

   \param GSW_PMAC_Ig_Cfg_t Pointer to a
      Ingress PMAC Attributes configuration \ref GSW_PMAC_Ig_Cfg_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_PMAC_IG_CFG_GET	_IOWR(GSW_PMAC_MAGIC, 0x07, GSW_PMAC_Ig_Cfg_t)

/**
   \brief Configures the Egress Attributes Config for a given PMAC port.
   It is used to configure egress attributes on given PMAC port.
   The PMAC Egress config can also be read using \ref GSW_PMAC_EG_CFG_GET.

   \param GSW_PMAC_Eg_Cfg_t Pointer to a
      Egress PMAC Attributes configuration \ref GSW_PMAC_Eg_Cfg_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_PMAC_EG_CFG_SET	_IOWR(GSW_PMAC_MAGIC, 0x08, GSW_PMAC_Eg_Cfg_t)

/**
   \brief Queries the Egress Attributes Config for given PMAC port.
   It is used to read egress attributes config on given PMAC port.
   The PMAC Egress config can also be set using \ref GSW_PMAC_EG_CFG_SET.

   \param GSW_PMAC_Eg_Cfg_t Pointer to a
      Egress PMAC Attributes configuration \ref GSW_PMAC_Eg_Cfg_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_PMAC_EG_CFG_GET	_IOWR(GSW_PMAC_MAGIC, 0x09, GSW_PMAC_Eg_Cfg_t)

/**
   \brief Reads the Counters for given DMA Channel Identifier associated to PMAC.
   It is used to read ingress statistics counters providing discarded packets and bytes on given PMAC port.

   \param GSW_PMAC_Cnt_t Pointer to a
      Statistics Counters of DMA channel associated to PMAC \ref GSW_PMAC_Cnt_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_PMAC_COUNT_GET	_IOWR(GSW_PMAC_MAGIC, 0x01, GSW_PMAC_Cnt_t)

/**
   \brief Writes the global PMAC settings applicable to GSWIP-3.0 PMAC ports.
   It is used to configure the global settings such as Padding, Checksum, Length and Egress PMAC Selector fields.

   \param GSW_PMAC_Glbl_Cfg_t Pointer to a
      global config of PMAC \ref GSW_PMAC_Glbl_Cfg_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_PMAC_GLBL_CFG_SET	_IOWR(GSW_PMAC_MAGIC, 0x02, GSW_PMAC_Glbl_Cfg_t)

/**
   \brief Reads the global PMAC settings currently configured to GSWIP-3.0 PMAC ports.
   It is used to configure the global stetinsg such as PAdding, Checksum, Length and Egress PMAC Selector fields.

   \param GSW_PMAC_Glbl_Cfg_t Pointer to a
      current global config of PMAC \ref GSW_PMAC_Glbl_Cfg_t (returned).

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_PMAC_GLBL_CFG_GET	_IOR(GSW_PMAC_MAGIC, 0x03, GSW_PMAC_Glbl_Cfg_t)


/*@}*/ /* GSW_IOCTL_PMAC */

/** \addtogroup GSW_IOCTL_GSWIP31 */
/*@{*/
/**
   \brief Allocate Extended VLAN Configuration block. Valid for GSWIP-3.1.
   It allocates consecutive VLAN configuration entries and return the block ID
   for further operations: \ref GSW_EXTENDEDVLAN_FREE, \ref GSW_EXTENDEDVLAN_SET
   and \ref GSW_EXTENDEDVLAN_GET.

   \param GSW_EXTENDEDVLAN_alloc_t Pointer to \ref GSW_EXTENDEDVLAN_alloc_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_EXTENDEDVLAN_ALLOC	_IOWR(GSW_EXVLAN_MAGIC, 0x01, GSW_EXTENDEDVLAN_alloc_t)
/**
   \brief Release Extended VLAN Configuration block. Valid for GSWIP-3.1.
   It is used to release Extended VLAN Configuration block allocated by
   \ref GSW_EXTENDEDVLAN_ALLOC.

   \param GSW_EXTENDEDVLAN_alloc_t Pointer to \ref GSW_EXTENDEDVLAN_alloc_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_EXTENDEDVLAN_FREE	_IOWR(GSW_EXVLAN_MAGIC, 0x04, GSW_EXTENDEDVLAN_alloc_t)

/**
   \brief Set Extended VLAN Configuration entry. Valid for GSWIP-3.1.
   It is used to set Extended VLAN Configuration entry with index
   \ref GSW_EXTENDEDVLAN_config_t::nEntryIndex, ranging between 0 and
   \ref GSW_EXTENDEDVLAN_alloc_t::nNumberOfEntries - 1, with valid
   \ref GSW_EXTENDEDVLAN_config_t::nExtendedVlanBlockId returned by
   \ref GSW_EXTENDEDVLAN_ALLOC.
   If \ref GSW_EXTENDEDVLAN_config_t::nExtendedVlanBlockId is
   \ref INVALID_HANDLE, this is absolute index of Extended VLAN Configuration
   entry in hardware, used for debugging purpose.

   \param GSW_EXTENDEDVLAN_config_t Pointer to \ref GSW_EXTENDEDVLAN_config_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_EXTENDEDVLAN_SET	_IOWR(GSW_EXVLAN_MAGIC, 0x02, GSW_EXTENDEDVLAN_config_t)

/**
   \brief Get Extended VLAN Configuration entry. Valid for GSWIP-3.1.
   It is used to get Extended VLAN Configuration entry with index
   \ref GSW_EXTENDEDVLAN_config_t::nEntryIndex, ranging between 0 and
   \ref GSW_EXTENDEDVLAN_alloc_t::nNumberOfEntries - 1, with valid
   \ref GSW_EXTENDEDVLAN_config_t::nExtendedVlanBlockId returned by
   \ref GSW_EXTENDEDVLAN_ALLOC.
   If \ref GSW_EXTENDEDVLAN_config_t::nExtendedVlanBlockId is
   \ref INVALID_HANDLE, this is absolute index of Extended VLAN Configuration
   entry in hardware, used for debugging purpose.

   \param GSW_EXTENDEDVLAN_config_t Pointer to \ref GSW_EXTENDEDVLAN_config_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_EXTENDEDVLAN_GET	_IOWR(GSW_EXVLAN_MAGIC, 0x03, GSW_EXTENDEDVLAN_config_t)

/**
   \brief Allocate VLAN Filter block. Valid for GSWIP-3.1.
   It allocates consecutive VLAN Filter entries and return the block ID
   for further operations: \ref GSW_VLANFILTER_FREE, \ref GSW_VLANFILTER_SET
   and \ref GSW_VLANFILTER_GET.

   \param GSW_VLANFILTER_alloc_t Pointer to \ref GSW_VLANFILTER_alloc_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_VLANFILTER_ALLOC	_IOWR(GSW_VLANFILTER_MAGIC, 0x01, GSW_VLANFILTER_alloc_t)

/**
   \brief Delete VLAN Filter Block. Valid for GSWIP-3.1.
   It is used to release VLAN Filter block allocated by
   \ref GSW_VLANFILTER_ALLOC.

   \param GSW_VLANFILTER_alloc_t Pointer to \ref GSW_VLANFILTER_alloc_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/

#define GSW_VLANFILTER_FREE	_IOWR(GSW_VLANFILTER_MAGIC, 0x04, GSW_VLANFILTER_alloc_t)

/**
   \brief Set VLAN Filter entry. Valid for GSWIP-3.1.
   It is used to set VLAN Filter entry with index
   \ref GSW_VLANFILTER_config_t::nEntryIndex, ranging between 0 and
   \ref GSW_VLANFILTER_alloc_t::nNumberOfEntries - 1, with valid
   \ref GSW_VLANFILTER_config_t::nVlanFilterBlockId returned by
   \ref GSW_VLANFILTER_ALLOC.
   If \ref GSW_VLANFILTER_config_t::nVlanFilterBlockId is \ref INVALID_HANDLE,
   this is absolute index of VLAN Filter entry in hardware, used for debugging
   purpose.

   \param GSW_VLANFILTER_config_t Pointer to \ref GSW_VLANFILTER_config_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/

#define GSW_VLANFILTER_SET	_IOWR(GSW_VLANFILTER_MAGIC, 0x02, GSW_VLANFILTER_config_t)

/**
   \brief Get VLAN Filter Entry. Valid for GSWIP-3.1.
   It is used to get VLAN filter entry with index
   \ref GSW_VLANFILTER_config_t::nEntryIndex, ranging between 0 and
   \ref GSW_VLANFILTER_alloc_t::nNumberOfEntries - 1, with valid
   \ref GSW_VLANFILTER_config_t::nVlanFilterBlockId returned by
   \ref GSW_VLANFILTER_ALLOC.
   If \ref GSW_VLANFILTER_config_t::nVlanFilterBlockId is \ref INVALID_HANDLE,
   this is absolute index of VLAN Filter entry in hardware, used for debugging
   purpose.

   \param GSW_VLANFILTER_config_t Pointer to \ref GSW_VLANFILTER_config_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/

#define GSW_VLANFILTER_GET	_IOWR(GSW_VLANFILTER_MAGIC, 0x03, GSW_VLANFILTER_config_t)

/**
   \brief Config CTP Port. Valid for GSWIP-3.1.
   It is used to setup CTP port.

   \param GSW_CTP_portConfig_t Pointer to \ref GSW_CTP_portConfig_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/

//#define GSW_CTP_PORT_CONFIG_SET	_IOW(GSW_CTP_MAGIC, 0x05, GSW_CTP_portConfig_t)
#define GSW_CTP_PORT_CONFIG_SET	_IOWR(GSW_CTP_MAGIC, 0x05, GSW_CTP_portConfig_t)
/**
   \brief get configuration of CTP Port. Valid for GSWIP-3.1.
   It is used to retrieve CTP port configuration.

   \param GSW_CTP_portConfig_t Pointer to \ref GSW_CTP_portConfig_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_CTP_PORT_CONFIG_GET	_IOWR(GSW_CTP_MAGIC, 0x06, GSW_CTP_portConfig_t)

/**
   \brief Reset configuration of CTP (Connectivity Termination Point). Valid for GSWIP-3.1.
   It is used to reset CTP configuration and, if necessary, release resources
   such as Extended VLAN, Meter, etc. Bridge Port ID will not be changed.
   If \ref GSW_CTP_portConfig_t::eMask has
   \ref GSW_CtpPortConfigMask_t::GSW_CTP_PORT_CONFIG_MASK_FORCE,
   \ref GSW_CTP_portConfig_t::nLogicalPortId is ignored and
   \ref GSW_CTP_portConfig_t::nSubIfIdGroup is absolute index of CTP in hardware
   for debug purpose, bypassing any check.

   \param GSW_CTP_portConfig_t Pointer to \ref GSW_CTP_portConfig_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_CTP_PORT_CONFIG_RESET _IOWR(GSW_CTP_MAGIC, 0x07, GSW_CTP_portConfig_t)

/**
   \brief Allocate Bridge Port. Valid for GSWIP-3.1.
   It is used to allocate a bridge port.

   \param GSW_BRIDGE_portAlloc_t Pointer to \ref GSW_BRIDGE_portAlloc_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_BRIDGE_PORT_ALLOC	_IOWR(GSW_BRDGPORT_MAGIC, 0x01, GSW_BRIDGE_portAlloc_t)

/**
   \brief Delete Bridge Port. Valid for GSWIP-3.1.
   It is used to release bridge port allocated with
   \ref GSW_BRIDGE_PORT_CONFIG_SET.

   \param GSW_BRIDGE_portAlloc_t Pointer to \ref GSW_BRIDGE_portAlloc_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/

#define GSW_BRIDGE_PORT_FREE	_IOWR(GSW_BRDGPORT_MAGIC, 0x04, GSW_BRIDGE_portAlloc_t)

/**
   \brief Config Bridge Port. Valid for GSWIP-3.1.
   It is used to configure bridge port. If \ref GSW_BRIDGE_portConfig_t::eMask
   has \ref GSW_BridgePortConfigMask_t::GSW_BRIDGE_PORT_CONFIG_MASK_FORCE,
   \ref GSW_BRIDGE_portConfig_t::nBridgePortId is absolute index of Bridge Port
   in hardware for debug purpose, bypassing any check.

   \param GSW_BRIDGE_portConfig_t Pointer to \ref GSW_BRIDGE_portConfig_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/

//#define GSW_BRIDGE_PORT_CONFIG_SET	_IOW(GSW_BRDGPORT_MAGIC, 0x02, GSW_BRIDGE_portConfig_t)
#define GSW_BRIDGE_PORT_CONFIG_SET	_IOWR(GSW_BRDGPORT_MAGIC, 0x02, GSW_BRIDGE_portConfig_t)

/**
   \brief get configuration of Bridge Port. Valid for GSWIP-3.1.
   It is used to retrieve bridge port configuration.
   If \ref GSW_BRIDGE_portConfig_t::eMask has
   \ref GSW_BridgePortConfigMask_t::GSW_BRIDGE_PORT_CONFIG_MASK_FORCE,
   \ref GSW_BRIDGE_portConfig_t::nBridgePortId is absolute index of Bridge Port
   in hardware for debug purpose, bypassing any check

   \param GSW_BRIDGE_portConfig_t Pointer to \ref GSW_BRIDGE_portConfig_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_BRIDGE_PORT_CONFIG_GET	_IOWR(GSW_BRDGPORT_MAGIC, 0x03, GSW_BRIDGE_portConfig_t)

/**
   \brief Allocate Bridge. Valid for GSWIP-3.1.
   It is used to allocate a bridge. Bridge 0 is always available as default
   bridge.

   \param GSW_BRIDGE_alloc_t Pointer to \ref GSW_BRIDGE_alloc_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_BRIDGE_ALLOC _IOWR(GSW_BRDG_MAGIC, 0x01, GSW_BRIDGE_alloc_t)

/**
   \brief Delete Bridge. Valid for GSWIP-3.1.
   It is used to release bridge allocated with \ref GSW_BRIDGE_ALLOC.

   \param GSW_BRIDGE_alloc_t Pointer to \ref GSW_BRIDGE_alloc_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/

#define GSW_BRIDGE_FREE	_IOWR(GSW_BRDG_MAGIC, 0x04, GSW_BRIDGE_alloc_t)

/**
   \brief Config Bridge. Valid for GSWIP-3.1.
   It is used to configure bridge. If \ref GSW_BRIDGE_config_t::eMask has
   \ref GSW_BridgeConfigMask_t::GSW_BRIDGE_CONFIG_MASK_FORCE,
   \ref GSW_BRIDGE_config_t::nBridgeId is absolute index of Bridge (FID) in
   hardware for debug purpose, bypassing any check.

   \param GSW_BRIDGE_config_t Pointer to \ref GSW_BRIDGE_config_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_BRIDGE_CONFIG_SET	_IOW(GSW_BRDG_MAGIC, 0x02, GSW_BRIDGE_config_t)

/**
   \brief get configuration of Bridge. Valid for GSWIP-3.1.
   It is used to retrieve bridge configuration.
   If \ref GSW_BRIDGE_config_t::eMask has
   \ref GSW_BridgeConfigMask_t::GSW_BRIDGE_CONFIG_MASK_FORCE,
   \ref GSW_BRIDGE_config_t::nBridgeId is absolute index of Bridge (FID) in
   hardware for debug purpose, bypassing any check.

   \param GSW_BRIDGE_config_t Pointer to \ref GSW_BRIDGE_config_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/

#define GSW_BRIDGE_CONFIG_GET	_IOWR(GSW_BRDG_MAGIC, 0x03, GSW_BRIDGE_config_t)
/**
   \brief Assign CTP to Logical Port. Valid for GSWIP-3.1.
   It is used to allocate a range of CTP and associate them to Logical Port.
   Apart from setting proper mode, it will do basic mapping between CTP and
   Bridge Port, then enable SDMA to allow ingress traffic from this port.

   \param GSW_CTP_portAssignment_t Pointer to \ref GSW_CTP_portAssignment_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_CTP_PORT_ASSIGNMENT_ALLOC	_IOWR(GSW_CTP_MAGIC, 0x01, GSW_CTP_portAssignment_t)

/**
   \brief Free CTP from Logical Port. Valid for GSWIP-3.1.
   It is used to stop association between CTP and Logical port. And it will stop
   SDMA so that the ingress traffic from this port is stopped.
*/
#define GSW_CTP_PORT_ASSIGNMENT_FREE	_IOW(GSW_CTP_MAGIC, 0x02, GSW_CTP_portAssignment_t)

/**
   \brief Assign CTP Ports to logical port. Valid for GSWIP-3.1.
   It is used to associate a range of CTP ports to logical port and set proper
   mode.

   \param GSW_CTP_portAssignment_t Pointer to \ref GSW_CTP_portAssignment_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_CTP_PORT_ASSIGNMENT_SET	_IOW(GSW_CTP_MAGIC, 0x03, GSW_CTP_portAssignment_t)

/**
   \brief Get CTP Ports assignment from logical port. Valid for GSWIP-3.1.
   It is used to retrieve CTP ports range of logical port and the mode of port.

   \param GSW_CTP_portAssignment_t Pointer to \ref GSW_CTP_portAssignment_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_CTP_PORT_ASSIGNMENT_GET	_IOWR(GSW_CTP_MAGIC, 0x04, GSW_CTP_portAssignment_t)

/**
   \brief Allocate Meter.
   This is a part of APIs to manage meters. This API works in 2 modes. If
   \ref GSW_QoS_meterCfg_t::nMeterId is \ref INVALID_HANDLE, this API will
   allocate a free meter, config it with all parameters in
   \ref GSW_QoS_meterCfg_t, and return the meter ID in
   \ref GSW_QoS_meterCfg_t::nMeterId. Otherwise,
   \ref GSW_QoS_meterCfg_t::nMeterId should be a valid meter ID, and this API
   increase the reference counter of this meter. Other fields are ignored.
   This API is wrapper of \ref GSW_QOS_METER_CFG_SET with meter resource
   management.

   \param GSW_QoS_meterCfg_t Pointer to \ref GSW_QoS_meterCfg_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_QOS_METER_ALLOC   _IOWR(GSW_QOS_MAGIC, 0x30, GSW_QoS_meterCfg_t)

/**
   \brief Free Meter.
   Decrease reference counter of the meter. If reference counter is 0, disable
   the meter then free it.

   \param u32 Meter ID.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_QOS_METER_FREE   _IOW(GSW_QOS_MAGIC, 0x31, u32)

/**
   \brief Update Color Marking Table.
   Should be used to setup color marking table at early stage.

   \param GSW_QoS_colorMarkingEntry_t Pointer to \ref GSW_QoS_colorMarkingEntry_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
//#define GSW_QOS_COLOR_MARKING_TABLE_SET   _IOW(GSW_QOS_MAGIC, 0x2C, GSW_QoS_colorMarkingEntry_t)
#define GSW_QOS_COLOR_MARKING_TABLE_SET   _IOWR(GSW_QOS_MAGIC, 0x2C, GSW_QoS_colorMarkingEntry_t)

/**
   \brief Get Color Marking Table.

   \param GSW_QoS_colorMarkingEntry_t Pointer to \ref GSW_QoS_colorMarkingEntry_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_QOS_COLOR_MARKING_TABLE_GET   _IOWR(GSW_QOS_MAGIC, 0x2D, GSW_QoS_colorMarkingEntry_t)

/**
   \brief Update Color Remarking Table.
   Should be used to setup color remarking table at early stage.

   \param GSW_QoS_colorRemarkingEntry_t Pointer to \ref GSW_QoS_colorRemarkingEntry_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_QOS_COLOR_REMARKING_TABLE_SET   _IOW(GSW_QOS_MAGIC, 0x2E, GSW_QoS_colorRemarkingEntry_t)

/**
   \brief Get Color Remarking Table.

   \param GSW_QoS_colorRemarkingEntry_t Pointer to \ref GSW_QoS_colorRemarkingEntry_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_QOS_COLOR_REMARKING_TABLE_GET   _IOWR(GSW_QOS_MAGIC, 0x2F, GSW_QoS_colorRemarkingEntry_t)

/**
   \brief Get DSCP to PCP Mapping Table.

   \param GSW_DSCP2PCP_map_t Pointer to \ref GSW_DSCP2PCP_map_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_DSCP2PCP_MAP_SET _IOWR(GSW_QOS_MAGIC, 0x32, GSW_DSCP2PCP_map_t)

#define GSW_DSCP2PCP_MAP_GET _IOWR(GSW_QOS_MAGIC, 0x33, GSW_DSCP2PCP_map_t)

/**
   \brief Get P-mapper Configuration.

   \param GSW_PMAPPER_t Pointer to \ref GSW_PMAPPER_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_PMAPPER_SET _IOWR(GSW_QOS_MAGIC, 0x34, GSW_PMAPPER_t)

#define GSW_PMAPPER_GET _IOWR(GSW_QOS_MAGIC, 0x35, GSW_PMAPPER_t)

/**
   \brief Set Default MAC Filter.

   \param GSW_MACFILTER_default_t Pointer to \ref GSW_MACFILTER_default_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_DEFAUL_MAC_FILTER_SET _IOW(GSW_MAC_MAGIC, 0x06, GSW_MACFILTER_default_t)

/**
   \brief Get Default MAC Filter.

   \param GSW_MACFILTER_default_t Pointer to \ref GSW_MACFILTER_default_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_DEFAUL_MAC_FILTER_GET _IOWR(GSW_MAC_MAGIC, 0x07, GSW_MACFILTER_default_t)
/*@}*/ /* GSW_IOCTL_GSWIP31 */



/**
	\brief Following are for Debug purpose only

	\param GSW_debug_t Pointer to \ref GSW_debug_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/

#define GSW_DEBUG_CTPTABLE_STATUS 			_IOWR(GSW_DEBUG_MAGIC, 0x01, GSW_debug_t)
#define GSW_DEBUG_BRDGPORTTABLE_STATUS 		_IOWR(GSW_DEBUG_MAGIC, 0x02, GSW_debug_t)
#define GSW_DEBUG_BRDGTABLE_STATUS 			_IOWR(GSW_DEBUG_MAGIC, 0x03, GSW_debug_t)
#define GSW_DEBUG_EXVLANTABLE_STATUS		_IOWR(GSW_DEBUG_MAGIC, 0x04, GSW_debug_t)
#define GSW_DEBUG_VLANFILTERTABLE_STATUS	_IOWR(GSW_DEBUG_MAGIC, 0x05, GSW_debug_t)
#define GSW_DEBUG_METERTABLE_STATUS 		_IOWR(GSW_DEBUG_MAGIC, 0x06, GSW_debug_t)
#define GSW_DEBUG_DSCP2PCPTABLE_STATUS		_IOWR(GSW_DEBUG_MAGIC, 0x07, GSW_debug_t)
#define GSW_DEBUG_PMAPPER_STATUS 			_IOWR(GSW_DEBUG_MAGIC, 0x08, GSW_debug_t)
#define GSW_DEBUG_PMAC_EG					_IOWR(GSW_DEBUG_MAGIC, 0x09, GSW_debug_t)
#define GSW_DEBUG_PMAC_IG 					_IOWR(GSW_DEBUG_MAGIC, 0x0A, GSW_debug_t)
#define GSW_DEBUG_DEF_QMAP					_IOWR(GSW_DEBUG_MAGIC, 0x0B, GSW_debug_t)
#define GSW_DEBUG_DEF_BYP_QMAP 				_IOWR(GSW_DEBUG_MAGIC, 0x0C, GSW_debug_t)
#define GSW_DEBUG_PMAC_BP 					_IOWR(GSW_DEBUG_MAGIC, 0x0D, GSW_debug_t)
#define GSW_DEBUG_LP_STATISTICS 			_IOWR(GSW_DEBUG_MAGIC, 0x0E, GSW_debug_t)
#define GSW_DEBUG_CTP_STATISTICS 			_IOWR(GSW_DEBUG_MAGIC, 0x0F, GSW_debug_t)
/**
   \brief XGMAC Cfg Commands to Read and write operation
   GSW_XGMAC_CFG.
   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.
   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_XGMAC_CFG	_IOWR(GSW_DEBUG_MAGIC, 0x10, GSW_MAC_Cli_t)

/**
   \brief GSWSS Cfg Commands to Read and write operation
   GSW_GSWSS_CFG.
   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.
   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_GSWSS_CFG	_IOWR(GSW_DEBUG_MAGIC, 0x11, GSW_MAC_Cli_t)

/**
   \brief LMAC Cfg Commands to Read and write operation
   GSW_LMAC_CFG.
   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.
   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_LMAC_CFG	_IOWR(GSW_DEBUG_MAGIC, 0x12, GSW_MAC_Cli_t)

/**
   \brief MACSEC Cfg Commands to Read and write operation
   GSW_MACSEC_CFG.
   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.
   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_MACSEC_CFG	_IOWR(GSW_DEBUG_MAGIC, 0x13, GSW_MACSec_cfg_t)
/**
   \brief PMACBR Cfg Commands to Read and write operation
   GSW_PMACBR_CFG.
   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.
   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_PMACBR_CFG	_IOWR(GSW_DEBUG_MAGIC, 0x14, GSW_MAC_cfg_t)

/**
   \brief DUMP MEM operation
   GSW_DUMP_MEM.
   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.
   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_DUMP_MEM	_IOWR(GSW_DEBUG_MAGIC, 0x15, GSW_table_t)

#define GSW_DEBUG_PRINT_PCEIRQ_LIST 		_IO(GSW_DEBUG_MAGIC, 0x16)
#define GSW_DEBUG_RMON_PORT_GET				_IOWR(GSW_DEBUG_MAGIC, 0x17, GSW_Debug_RMON_Port_cnt_t)
#define GSW_DEBUG_TUNNELTEMP_STATUS 		_IOWR(GSW_DEBUG_MAGIC, 0x18, GSW_debug_t)
#define GSW_DEBUG_TFLOWTABLE_STATUS _IOWR(GSW_DEBUG_MAGIC, 0x19, GSW_debug_t)

/**
   \brief Following are for GSWIP IRQ operation

   \param GSW_Irq_Op_t Pointer to \ref GSW_Irq_Op_t.
*/
#define GSW_IRQ_REGISTER 	_IOWR(GSW_IRQ_MAGIC, 0x01, GSW_Irq_Op_t)
#define GSW_IRQ_UNREGISTER 	_IOWR(GSW_IRQ_MAGIC, 0x02, GSW_Irq_Op_t)
#define GSW_IRQ_ENABLE 		_IOWR(GSW_IRQ_MAGIC, 0x03, GSW_Irq_Op_t)
#define GSW_IRQ_DISBALE 	_IOWR(GSW_IRQ_MAGIC, 0x04, GSW_Irq_Op_t)

/**
   \brief Following are for GSWIP 3.2 PBB Tunnel template operation

   \param GSW_PBB_Tunnel_Template_Config_t Pointer to \ref GSW_PBB_Tunnel_Template_Config_t.
*/
#define GSW_PBB_TUNNEL_TEMPLATE_ALLOC 	_IOWR(GSW_PBB_MAGIC, 0x01, GSW_PBB_Tunnel_Template_Config_t)
#define GSW_PBB_TUNNEL_TEMPLATE_FREE 	_IOWR(GSW_PBB_MAGIC, 0x02, GSW_PBB_Tunnel_Template_Config_t)
#define GSW_PBB_TUNNEL_TEMPLATE_SET 	_IOWR(GSW_PBB_MAGIC, 0x03, GSW_PBB_Tunnel_Template_Config_t)
#define GSW_PBB_TUNNEL_TEMPLATE_GET 	_IOWR(GSW_PBB_MAGIC, 0x04, GSW_PBB_Tunnel_Template_Config_t)

/**
   \brief Following are for GSWIP 3.2 LPID->GPID/GPID->LPID operation

   \param GSW_LPID_to_GPID_Assignment_t Pointer to \ref GSW_LPID_to_GPID_Assignment_t.
   \param GSW_GPID_to_LPID_Assignment_t Pointer to \ref GSW_GPID_to_LPID_Assignment_t.

*/
#define GSW_LPID_TO_GPID_ASSIGNMENT_SET 	_IOWR(GSW_GPID_MAGIC, 0x01, GSW_LPID_to_GPID_Assignment_t)
#define GSW_LPID_TO_GPID_ASSIGNMENT_GET 	_IOWR(GSW_GPID_MAGIC, 0x02, GSW_LPID_to_GPID_Assignment_t)
#define GSW_GPID_TO_LPID_ASSIGNMENT_SET 	_IOWR(GSW_GPID_MAGIC, 0x03, GSW_GPID_to_LPID_Assignment_t)
#define GSW_GPID_TO_LPID_ASSIGNMENT_GET 	_IOWR(GSW_GPID_MAGIC, 0x04, GSW_GPID_to_LPID_Assignment_t)

#endif    /* _LANTIQ_GSW_H_ */
