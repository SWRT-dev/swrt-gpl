/******************************************************************************
                Copyright (c) 2016, 2017 Intel Corporation

******************************************************************************/
/*****************************************************************************
                Copyright (c) 2012, 2014, 2015
                    Lantiq Deutschland GmbH
    For licensing information, see the file 'LICENSE' in the root folder of
    this software module.
******************************************************************************/


#ifndef _LTQ_FLOW_CORE_H_
#define _LTQ_FLOW_CORE_H_


#ifdef __KERNEL__
#include <net/switch_api/gsw_flow_ops.h>
#include <linux/regmap.h>
#else
#include <gsw_flow_ops.h>
#endif

#define	PCE_ASSERT(t)	if ((t)) { \
		printk("%s:%s:%d (" # t ")\n", __FILE__, __func__, __LINE__); \
		return -1; }

#ifndef GSW_RETURN_PCE
#define GSW_RETURN_PCE	\
	{	\
		printk("ERROR:\n\tFile %s\n\tLine %d\n", __FILE__, __LINE__);	\
		return -1; \
	}
#endif

/* Below are two macros
For FILL_CTRL_REG make sure init 'outVal' to zeor before filling
Where as CLEAR_FILL_CTRL_REG takes care of clear then fill.
*/
//Set the value at given offset.
#define FILL_CTRL_REG(outVal, shift, inVal)	\
	outVal = (outVal | (inVal << shift))
//Clear first then set value at given offset.
#define CLEAR_FILL_CTRL_REG(outVal, shift, size, inVal)	\
	outVal = ((outVal & (~(((1 << size)-1) << shift))) |\
		  ((inVal & ((1 << size)-1)) << shift))
//Get the value at given offset.
#define GET_VAL_FROM_REG(outVal, shift, size, inVal)	\
	outVal = ((inVal >> shift) & ((1 << size)-1))

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(arr)	(sizeof(arr)/sizeof((arr)[0]))
#endif

#define MAX_BUSY_RETRY	1000
#define RETURN_ERROR_CODE		0
#define RETURN_FROM_FUNCTION	1

#define CHECK_BUSY(reg, shift, size, action) ({					\
		int retCode = GSW_statusOk;										\
		do{ 															\
			u32 busyRetry=MAX_BUSY_RETRY, value=0;						\
			do{															\
				gsw_r32(cdev, reg, shift, size, &value);				\
			}while (value && --busyRetry);								\
			if(value && !busyRetry)										\
			{															\
				pr_err("ERROR: Hardware busy for too long\n");			\
				pr_err("%s %s %d (Register=0x%x)\n",__FILE__, __func__, __LINE__,reg);\
				if(RETURN_FROM_FUNCTION == action)						\
					return GSW_statusErr;								\
				retCode = GSW_statusErr;    							\
			}															\
		}while(0);														\
		retCode;														\
	})																\

#ifdef __KERNEL__

#define CHECK_BUSY_MDIO(reg, shift, size, action) ({			\
		int retCode = GSW_statusOk; 									\
		do{ 															\
			u32 value=0;												\
			do{ 														\
				udelay(1);												\
				gsw_r32(cdev, reg, shift, size, &value);				\
			}while (value);												\
		}while(0);														\
		retCode;														\
	})																\

#endif

#if defined(WIN_PC_MODE) && WIN_PC_MODE

#define CHECK_BUSY_MDIO(reg, shift, size, action) ({			\
		int retCode = GSW_statusOk; 									\
		do{ 															\
			u32 value=0;												\
			do{ 														\
				gsw_r32(cdev, reg, shift, size, &value);				\
			}while (value);												\
		}while(0);														\
		retCode;														\
	})																\

#endif

#define PORT_STATE_LISTENING 0
#define PS_RENABLE_TDISABLE	1
#define PS_RDISABLE_TENABLE	2
#define PORT_STATE_DISABLE  3
#define PORT_STATE_LEARNING 4
#define PORT_STATE_FORWARDING 7
#define REX_TFLOW_CNT_1	0x28


/*GSWIP 3.1 --------------------------*/
#define MAX_PORT_NUMBER 128
#define DSCP2PCP_TABLE_SIZE         64
#define EXVLAN_TABLE_SIZE           1024
#define VLAN_FILTER_TABLE_SIZE      1024
#define BRDG_CONF_TABLE_SIZE        64
#define CTP_PORTCONF_TABLE_SIZE     288
#define BRDG_PORTCONF_TABLE_SIZE    128
#define METER_TABLE_SIZE            128
#define SHAPER_TABLE_SIZE           32
#define PMAPPER_TABLE_SIZE          32
#define GSW_TFLOW_TABLE_SIZE		512
#define CLEAR_U16(var) var &= ((u16)~(0xFFFF))
/*-------------------------------------*/

#define PCE_INNER_PCP_DFL_ENTRIES 8
#define PCE_INNER_PCP_MAX_ENTRIES 16

#define VLAN_ACTIVE_TABLE_SIZE 64
/* #define MAC_TABLE_SIZE 2048 */
#define MC_PC_SIZE 64
#define GSW_2X_SOC_CPU_PORT 6
#define GSW_3X_SOC_CPU_PORT 0
#define LTQ_SOC_CAP_SEGMENT 256
#define VRX_PLATFORM_CAP_FID 64
#define RMON_COUNTER_OFFSET 64
/*Applicable only for GSWIP 3.1*/
#define RMON_COUNTER_OFFSET_GSWIP3_1 22
#define RMON_TFLOW_DIR_MAX 2
#define MAX_PACKET_LENGTH 9600
#define V31_MAX_PACKET_LENGTH 10000
#define PCE_PKG_LNG_TBL_SIZE 16
#define PCE_DASA_MAC_TBL_SIZE 64
#define PCE_APPL_TBL_SIZE 64
#define PCE_FLAGS_TBL_SIZE 32
#define PCE_PAYLOAD_TBL_SIZE 64
#define IP_DASA_PC_MSIZE 16
#define IP_DASA_PC_LSIZE 64
#define PCE_PTCL_TBL_SIZE 32
#define PCE_PPPOE_TBL_SIZE 16
#define PCE_VLAN_ACT_TBL_SIZE 64
#define PCE_TABLE_SIZE 512
/*256 -- GSWIP3.0*/ /* 64 -- GSWIP2.2*/
#define PCE_MICRO_TABLE_SIZE 256
/* Pore redirect PCE rules set or port 0 (30), port 1 (31),*/
/* port 2 (32), port 3(33), port 4(34) and port 5(35) */
#define PRD_PRULE_INDEX 30
#define EAPOL_PCE_RULE_INDEX	8
#define BPDU_PCE_RULE_INDEX 9
#define MPCE_RULES_INDEX	0
#define LTQ_GSWIP_2_0 0x100
#define LTQ_GSWIP_2_1 0x021
#define LTQ_GSWIP_2_2 0x122
#define LTQ_GSWIP_2_2_ETC 0x023
#define LTQ_GSWIP_3_0 0x030
#define LTQ_GSWIP_3_1 0x031
#define LTQ_GSWIP_3_2 0x032

#define VRSN_CMP(x, y)	((int)((x) & 0xFF) - (int)((y) & 0xFF))

#define IS_VRSN_30_31(ver) \
	((ver == LTQ_GSWIP_3_0) || (ver == LTQ_GSWIP_3_1))

#define IS_VRSN_31(ver) \
	((ver == LTQ_GSWIP_3_1))

#define IS_VRSN_NOT_31(ver) \
	((ver != LTQ_GSWIP_3_1))

#define GSW_BRIDGE_PORT_SRC_IP_LOOKUP_DISABLE 1

/*PHY Reg 0x4 */
#define PHY_AN_ADV_10HDX 0x20
#define PHY_AN_ADV_10FDX 0x40
#define PHY_AN_ADV_100HDX 0x80
#define PHY_AN_ADV_100FDX 0x100
/*PHY Reg 0x9 */
#define PHY_AN_ADV_1000HDX 0x100
#define PHY_AN_ADV_1000FDX 0x200

#define DEFAULT_AGING_TIMEOUT	300
#define AGING_CLK		25000000
#define AGING_STEPS		16
/* Define Aging Counter Mantissa Value */
#define AGETIMER_1_DAY 0xFB75
#define AGETIMER_1_HOUR 0xA7BA
#define AGETIMER_300_SEC 0xDF84
#define AGETIMER_10_SEC 0x784
#define AGETIMER_1_SEC 0xBF

/* Define Duplex Mode */
#define DUPLEX_AUTO 0
#define DUPLEX_EN 1
#define DUPLEX_DIS 3

#define INSTR 0
#define IPV6 1
#define LENACCU 2

/*GSWIP 3.1 related Macro defines*/



/* parser's microcode output field type */
enum {
	OUT_MAC0 = 0,
	OUT_MAC1,
	OUT_MAC2,
	OUT_MAC3,
	OUT_MAC4,
	OUT_MAC5,
	OUT_ITAG0,
	OUT_ITAG1,
	OUT_ITAG2,
	OUT_ITAG3,
	OUT_1VTAG0, /* 10 */
	OUT_1VTAG1,
	OUT_2VTAG0,
	OUT_2VTAG1,
	OUT_3VTAG0,
	OUT_3VTAG1,
	OUT_4VTAG0,
	OUT_4VTAG1,
	OUT_ETYPE,
	OUT_PPPOE0,
	OUT_PPPOE1, /* 20 */
	OUT_PPPOE3,
	OUT_PPP,
	OUT_RES,
	OUT_1IP0,
	OUT_1IP1,
	OUT_1IP2,
	OUT_1IP3,
	OUT_1IP4,
	OUT_1IP5,
	OUT_1IP6, /* 30 */
	OUT_1IP7,
	OUT_1IP8,
	OUT_1IP9,
	OUT_1IP10,
	OUT_1IP11,
	OUT_1IP12,
	OUT_1IP13,
	OUT_1IP14,
	OUT_1IP15,
	OUT_1IP16, /* 40 */
	OUT_1IP17,
	OUT_1IP18,
	OUT_1IP19,
	OUT_2IP0,
	OUT_2IP1,
	OUT_2IP2,
	OUT_2IP3,
	OUT_2IP4,
	OUT_2IP5,
	OUT_2IP6, /* 50 */
	OUT_2IP7,
	OUT_2IP8,
	OUT_2IP9,
	OUT_2IP10,
	OUT_2IP11,
	OUT_2IP12,
	OUT_2IP13,
	OUT_2IP14,
	OUT_2IP15,
	OUT_2IP16, /* 60 */
	OUT_2IP17,
	OUT_2IP18,
	OUT_2IP19,
	OUT_APP0,
	OUT_APP1,
	OUT_APP2,
	OUT_APP3,
	OUT_APP4,
	OUT_APP5,
	OUT_APP6, /* 70 */
	OUT_APP7,
	OUT_APP8,
	OUT_APP9,
	OUT_1PL,
	OUT_2PL,
	OUT_1LNH,
	OUT_2LNH = 77,
	OUT_NONE = 127
};

/* parser's microcode flag type */
enum {
	FLAG_NO = 0,
	FLAG_END,
	FLAG_CAPWAP,
	FLAG_GRE,
	FLAG_LEN,
	FLAG_GREK,
	FLAG_VXLAN,
	FLAG_TCPSYN,
	FLAG_ITAG,
	FLAG_1VLAN,
	FLAG_2VLAN,  /* 10 */
	FLAG_3VLAN,
	FLAG_4VLAN,
	FLAG_SNAP,
	FLAG_PPPOES,
	FLAG_1IPV4,
	FLAG_1IPV6,
	FLAG_2IPV4,
	FLAG_2IPV6,
	FLAG_ROUTEXP,
	FLAG_TCP,  /* 20 */
	FLAG_1UDP,
	FLAG_IGMP,
	FLAG_IPV4OPT,
	FLAG_1IPV6EXT,
	FLAG_TCPACK,
	FLAG_IPFRAG,
	FLAG_EAPOL,
	FLAG_2IPV6EXT,
	FLAG_2UDP,
	FLAG_L2TPNEXP,  /* 30 */
	FLAG_LROEXP,
	FLAG_L2TP,
	FLAG_GRE_VLAN1,
	FLAG_GRE_VLAN2,
	FLAG_GRE_PPPOE,
	FLAG_IPV4MC,
	FLAG_ETYPE_888E,
	FLAG_NN15,
	FLAG_NN16,
	FLAG_NN17, /* 40 */
	FLAG_NN18,
	FLAG_NN19,
	FLAG_NN20,
	FLAG_NN21,
	FLAG_NN22,
	FLAG_NN23,
	FLAG_NN24,
	FLAG_DHCPV4,
	FLAG_DHCPV6,
	FLAG_IPMC, /* 50 */
	FLAG_NN28,
	FLAG_NN29,
	FLAG_NN30,
	FLAG_NN31,
	FLAG_NN32,
	FLAG_NN33,
	FLAG_NN34,
	FLAG_NN35,
	FLAG_NN36,
	FLAG_NN37, /* 60 */
	FLAG_NN38,
	FLAG_NN39,
	FLAG_NN40,
};

/* GSWIP_2.X*/
enum {
	GOUT_MAC0 = 0,
	GOUT_MAC1,
	GOUT_MAC2,
	GOUT_MAC3,
	GOUT_MAC4,
	GOUT_MAC5,
	GOUT_ETHTYP,
	GOUT_VTAG0,
	GOUT_VTAG1,
	GOUT_ITAG0,
	GOUT_ITAG1,	/*10 */
	GOUT_ITAG2,
	GOUT_ITAG3,
	GOUT_IP0,
	GOUT_IP1,
	GOUT_IP2,
	GOUT_IP3,
	GOUT_SIP0,
	GOUT_SIP1,
	GOUT_SIP2,
	GOUT_SIP3,	/*20*/
	GOUT_SIP4,
	GOUT_SIP5,
	GOUT_SIP6,
	GOUT_SIP7,
	GOUT_DIP0,
	GOUT_DIP1,
	GOUT_DIP2,
	GOUT_DIP3,
	GOUT_DIP4,
	GOUT_DIP5,	/*30*/
	GOUT_DIP6,
	GOUT_DIP7,
	GOUT_SESID,
	GOUT_PROT,
	GOUT_APP0,
	GOUT_APP1,
	GOUT_IGMP0,
	GOUT_IGMP1,
//	GOUT_IPOFF,	/*39*/
	GOUT_STAG0 = 61,
	GOUT_STAG1 = 62,
	GOUT_NONE	=	63,
};

/* parser's microcode flag type */
enum {
	GFLAG_ITAG = 0,
	GFLAG_VLAN,
	GFLAG_SNAP,
	GFLAG_PPPOE,
	GFLAG_IPV6,
	GFLAG_IPV6FL,
	GFLAG_IPV4,
	GFLAG_IGMP,
	GFLAG_TU,
	GFLAG_HOP,
	GFLAG_NN1,	/*10 */
	GFLAG_NN2,
	GFLAG_END,
	GFLAG_NO,	/*13*/
	GFLAG_SVLAN,  /*14 */
};

typedef struct {
	u16 val_3;
	u16 val_2;
	u16 val_1;
	u16 val_0;
} pce_uc_row_t;


typedef enum {
	/** Parser microcode table */
	PMAC_BPMAP_INDEX = 0x00,
	PMAC_IGCFG_INDEX = 0x01,
	PMAC_EGCFG_INDEX = 0x02,
} pm_tbl_cmds_t;

/** Description */
typedef enum {
	PMAC_OPMOD_READ = 0,
	PMAC_OPMOD_WRITE = 1,
} pm_opcode_t;

typedef pce_uc_row_t PCE_MICROCODE[PCE_MICRO_TABLE_SIZE];
/** Provides the address of the configured/fetched lookup table. */
typedef enum {
	/** Parser microcode table */
	PCE_PARS_INDEX = 0x00,
	PCE_ACTVLAN_INDEX = 0x01,
	PCE_VLANMAP_INDEX = 0x02,
	PCE_PPPOE_INDEX = 0x03,
	PCE_PROTOCOL_INDEX = 0x04,
	PCE_APPLICATION_INDEX	= 0x05,
	PCE_IP_DASA_MSB_INDEX	= 0x06,
	PCE_IP_DASA_LSB_INDEX	= 0x07,
	PCE_PACKET_INDEX = 0x08,
	PCE_PCP_INDEX = 0x09,
	PCE_DSCP_INDEX = 0x0A,
	PCE_MAC_BRIDGE_INDEX	= 0x0B,
	PCE_MAC_DASA_INDEX = 0x0C,
	PCE_MULTICAST_SW_INDEX = 0x0D,
	PCE_MULTICAST_HW_INDEX = 0x0E,
	PCE_TFLOW_INDEX = 0x0F,
	PCE_REMARKING_INDEX = 0x10,
	PCE_QUEUE_MAP_INDEX = 0x11,
	PCE_METER_INS_0_INDEX	= 0x12,
	PCE_METER_INS_1_INDEX	= 0x13,
	PCE_IPDALSB_INDEX = 0x14,
	PCE_IPSALSB_INDEX = 0x15,
	PCE_MACDA_INDEX = 0x16,
	PCE_MACSA_INDEX = 0x17,
	PCE_PARSER_FLAGS_INDEX = 0x18,
	PCE_PARS_INDIVIDUAL_INDEX	= 0x19,
	PCE_SPCP_INDEX = 0x1A,
	PCE_MSTP_INDEX = 0x1B,
	PCE_EGREMARK_INDEX = 0x1C,
	PCE_PAYLOAD_INDEX = 0x1D,
	PCE_EG_VLAN_INDEX = 0x1E,
} ptbl_cmds_t;

/*GSWIP 3.1 related defines*/
#define PMAPPER_ENTRY_INVALID 0xFFFF
#define EXVLAN_ENTRY_INVALID 0xFFFF
#define VLANFILTER_ENTRY_INVALID 0xFFFF
#define BRDG_CONF_ENTRY_INVALID 0xFFFF
#define BRDG_PORTCONF_ENTRY_INVALID 0xFFFF
#define METER_ENTRY_INVALID 0xFFFF
#define TFLOW_ENTRY_INVALID 0xFFFF

typedef enum {
	PCE_VLANFILTER_INDEX = 0x2,
	PCE_DSCP2PCP_INDEX = 0xC,
	PCE_IGCTP_INDEX = 0x12,
	PCE_EGCTP_INDEX = 0x13,
	PCE_IGBGP_INDEX = 0x14,
	PCE_EGBGP_INDEX = 0x15,
	PCE_BRGCFG_INDEX = 0x19,
	PCE_MAMRK_INDEX = 0x1B,
	PCE_REMARK_INDEX = 0x1C,
	PCE_EXTVLAN_INDEX = 0x1E,
	PCE_PMAP_INDEX = 0x1F
} ptbl_3_1_cmds_t;

/** Description */
typedef enum {
	PCE_OP_MODE_ADRD = 0,
	PCE_OP_MODE_ADWR = 1,
	PCE_OP_MODE_KSRD = 2,
	PCE_OP_MODE_KSWR = 3
} ptbl_opcode_t;

typedef enum {
	LTQ_FLOW_DEV_INT	= 0,
	LTQ_FLOW_DEV_INT_R	= 1,
	LTQ_FLOW_DEV_EXT_AX3000_F24S = 2,
	LTQ_FLOW_DEV_MAX
} gsw_devtype_t;

/*Applicable only for GSWIP 3.0*/
enum {
	PCE_VXLAN_INDEX_0 = 126,
	PCE_VXLAN_INDEX_1 = 127
};

typedef struct {
	u16 pkg_lng;
	u16 pkg_lng_rng;
}	pce_pkt_length_t;

typedef struct {
	u8 mac[6];
	u16 mac_mask;
}	pce_sa_prog_t;

typedef struct {
	u8 mac[6];
	u16 mac_mask;
}	pce_da_prog_t;

typedef struct {
	u16 appl_data;
	u16 mask_range;
	u8 mask_range_type;	/* 0 - nibble mask
				 * 1 - range
				 * 2 - bit mask (GSWIP3.x) */
} app_tbl_t;

typedef struct {
	u16 parser_flag_data;
	u16 mask_value;
	u8 valid;
} flag_tbl_t;

typedef struct {
	u16 payload_data;
	u16 mask_range;
	u8 mask_range_type: 1;
	u8 valid: 1;
} payload_tbl_t;

/* IP DA/SA MSB Table */
typedef struct {
	u8 imsb[8];
	u16 mask[4];
	u16 nmask; //Not in use.
	ltq_bool_t valid;
} pce_dasa_msb_t;

/* IP DA/SA LSB Table */
typedef struct {
	u8 ilsb[8];
	u16	mask[4];
	u16 nmask; //Not in use.
	ltq_bool_t valid;
} pce_dasa_lsb_t;

/* programme the Protocol Table */
typedef struct {
	u16 ethertype;
	u16 emask;
} prtcol_tbl_t;

/* PPPoE Table  */
typedef struct {
	u16	sess_id;
} pce_ppoe_tbl_t;

typedef struct {
	u16 pkt_lng_idx: 8;
	u16 dst_mac_addr_idx: 8;
	u16 src_mac_addr_idx: 8;
	u16 dst_appl_fld_idx: 8;
	u16 src_appl_fld_idx: 8;
	u16 dip_msb_idx: 8;
	u16 dip_lsb_idx: 8;
	u16 sip_msb_idx: 8;
	u16 sip_lsb_idx: 8;
	u16 inr_dip_msb_idx: 8;
	u16 inr_dip_lsb_idx: 8;
	u16 inr_sip_msb_idx: 8;
	u16 inr_sip_lsb_idx: 8;
	u16 ip_prot_idx: 8;
	u16 ethertype_idx: 8;
	u16 pppoe_idx: 8;
	u16 vlan_idx: 8;
	u16 svlan_idx: 8;
	u16 payload1_idx: 8;
	u16 payload2_idx: 8;
	u16 ppp_prot_idx: 8;
	u16 parse_lsb_idx: 8;
	u16 parse_msb_idx: 8;
	u16 parse1_lsb_idx: 8;
	u16 parse1_msb_idx: 8;
} pce_table_t;

typedef struct {
	/* table reference counter */
	u16 pkg_lng_tbl_cnt[PCE_PKG_LNG_TBL_SIZE];
	u16 sa_mac_tbl_cnt[PCE_DASA_MAC_TBL_SIZE];
	u16 da_mac_tbl_cnt[PCE_DASA_MAC_TBL_SIZE];
	u16 appl_tbl_cnt[PCE_APPL_TBL_SIZE];
	u16 flags_tbl_cnt[PCE_FLAGS_TBL_SIZE];
	u16 payload_tbl_cnt[PCE_PAYLOAD_TBL_SIZE];
	u16 ipmsbtcnt[IP_DASA_PC_MSIZE];
	u16 iplsbtcnt[IP_DASA_PC_LSIZE];
	u16 ptcl_tbl_cnt[PCE_PTCL_TBL_SIZE];
	u16 pppoe_tbl_cnt[PCE_PPPOE_TBL_SIZE];
	u16 vlan_act_tbl_cnt[PCE_VLAN_ACT_TBL_SIZE];
	/* cached tables */
	pce_pkt_length_t pkg_lng_tbl[PCE_PKG_LNG_TBL_SIZE];
	pce_sa_prog_t sa_mac_tbl[PCE_DASA_MAC_TBL_SIZE];
	pce_da_prog_t da_mac_tbl[PCE_DASA_MAC_TBL_SIZE];
	app_tbl_t appl_tbl[PCE_APPL_TBL_SIZE];
	flag_tbl_t flags_tbl[PCE_FLAGS_TBL_SIZE];
	payload_tbl_t payload_tbl[PCE_PAYLOAD_TBL_SIZE];
	pce_dasa_msb_t ip_dasa_msb_tbl[IP_DASA_PC_MSIZE];
	pce_dasa_lsb_t ip_dasa_lsb_tbl[IP_DASA_PC_LSIZE];
	prtcol_tbl_t ptcl_tbl[PCE_PTCL_TBL_SIZE];
	pce_ppoe_tbl_t pppoe_tbl[PCE_PPPOE_TBL_SIZE];
} pcetbl_prog_t /*pce_table_handle_t*/;

typedef struct {
	/* Parameter for the sub-tables */
	pcetbl_prog_t pce_sub_tbl;
	pce_table_t pce_tbl[PCE_TABLE_SIZE];
	GSW_PCE_action_t	pce_act[PCE_TABLE_SIZE];
	u8 ptblused[PCE_TABLE_SIZE];
} ltq_pce_table_t;

typedef struct {
	GSW_capType_t captype;
	/* Description String */
	char desci[GSW_CAP_STRING_LEN];
} gsw_capdesc_t;

typedef struct {
	GSW_STP_PortState_t psstate /*ifx_stp_state*/;
	GSW_8021X_portState_t	ps8021x /*ifx_8021_state*/;
	u8	pen_reg;
	u8	pstate_reg;
	u8	lrnlim;
} pstpstate_t;

typedef struct {
	/* Port Enable */
	ltq_bool_t penable;
	/* Learning Limit Action */
	/*	ltq_bool_t laction; */
	/* Automatic MAC address table learning locking */
	/*	ltq_bool_t lplock; */
	/* Automatic MAC address table learning limitation */
	u16 llimit;
	/* Port State */
	u16 ptstate;
	/* Port State for STP */
	GSW_STP_PortState_t	pcstate;
	/* Port State for 8021.x */
	GSW_8021X_portState_t	p8021xs;
} port_config_t;

typedef struct {
	/* 8021x Port Forwarding State */
	GSW_portForward_t sfport;
	/* STP port State */
	GSW_portForward_t spstate;
	/* 8021X Forwarding Port ID*/
	u8	fpid8021x;
	/* STP Port ID */
	u16	stppid;
} stp8021x_t;

typedef struct {
	ltq_bool_t	valid;
	u16 vid;
	u32 fid;
	u16 pm;
	u16 tm;
	ltq_bool_t	reserved;
} avlan_tbl_t;

typedef struct {
	u16 smsbindex;
	u16 dmsbindex;
	u16 slsbindex;
	u16 dlsbindex;
	u16 pmap;
	u16 subifid;
	u8 fid;
	u16 mcmode;
	ltq_bool_t valid;
	ltq_bool_t exclude;
} mcsw_table_t;


typedef struct {
	u16	igmode;
	ltq_bool_t igv3;
	u16	igfport;
	u8	igfpid;
	ltq_bool_t igxvlan;
	u8	igcos;
	mcsw_table_t mctable[MC_PC_SIZE];
	u16	igrport;
	u8	itblsize;
} gsw_igmp_t;

typedef struct {
	u16 VlanBlockId;
	u8  IndexInUse;
	u16 IndexInUsageCnt;
	u8  Dscp2PcpPointerAssigned;
	u16 Dscp2PcpPointer;
	u8  MeterAssigned;
	u16 MeterId;
} vlan_entry_idx;

typedef struct {
	vlan_entry_idx vlan_idx[EXVLAN_TABLE_SIZE];
	u16 nUsedEntry;
} gsw_extendvlan_t;

typedef struct {
	u8 IndexInUse;
	u16 IndexInUsageCnt;
} gsw_dscp2pcp_t;

typedef struct {
	u8 IndexInUse;
	u16 IndexInUsageCnt;
	u8 BroadcastMeteringAssigned;
	u16 BroadcastMeteringId;
	u8 MulticastMeteringAssigned;
	u16 MulticastMeteringId;
	u8 UnknownUniCastMeteringAssigned;
	u16 UnknownUniCastMeteringId;
	u8 UnknownMultiIpMeteringAssigned;
	u16 UnknownMultiIpMeteringId;
	u8 UnknownMultiNonIpMeteringAssigned;
	u16 UnknownMultiNonIpMeteringId;
} gsw_brdgconfig_t;

typedef struct {
	u8 IndexInUse;
	u16 IndexInUsageCnt;
	u8 BrdgIdAssigned;
	u16 BrdgId;
	u8 IngressExVlanBlkAssigned;
	u16 IngressExVlanBlkId;
	u8 EgressExVlanBlkAssigned;
	u16 EgressExVlanBlkId;
	u8 IngressMeteringAssigned;
	u16 IngressTrafficMeterId;
	u8 EgressMeteringAssigned;
	u16 EgressTrafficMeterId;
	u8 BroadcastMeteringAssigned;
	u16 BroadcastMeteringId;
	u8 MulticastMeteringAssigned;
	u16 MulticastMeteringId;
	u8 UnknownUniCastMeteringAssigned;
	u16 UnknownUniCastMeteringId;
	u8 UnknownMultiIpMeteringAssigned;
	u16 UnknownMultiIpMeteringId;
	u8 UnknownMultiNonIpMeteringAssigned;
	u16 UnknownMultiNonIpMeteringId;
	u8 PmapperAssigned;
	u16 PmappperIdx;
	u8 IngressVlanFilterAssigned;
	u16 IngressVlanFilterBlkId;
	u8 EgressVlanFilter1Assigned;
	u16 EgressVlanFilter1BlkId;
	u8 EgressVlanFilter2Assigned;
	u16 EgressVlanFilter2BlkId;
	GSW_STP_PortState_t	StpState;
	GSW_8021X_portState_t P8021xState;
	u16 LearningLimit;
} gsw_brdgportconfig_t;

typedef struct {
	u8 IndexInUse;
	u16 IndexInUsageCnt;
	u16 AssociatedLogicalPort;
	u8 BrdgIdPortAssigned;
	u16 BrdgPortId;
	u8 IngressExVlanNonIgmpBlkAssigned;
	u16 IngressExVlanNonIgmpBlkId;
	u8 IngressExVlanIgmpBlkAssigned;
	u16 IngressExVlanIgmpBlkId;
	u8 IngressMeteringAssigned;
	u16 IngressTrafficMeterId;
	u8 IngressBridgeBypassPmapperAssigned;
	u16 IngressBridgeBypassPmappperIdx;
	u8 EgressExVlanNonIgmpBlkAssigned;
	u16 EgressExVlanNonIgmpBlkId;
	u8 EgressExVlanIgmpBlkAssigned;
	u16 EgressExVlanIgmpBlkId;
	u8 EgressMeteringAssigned;
	u16 EgressTrafficMeterId;
	u8 IngressTflowAssigned;
	u16 IngressTflowFirstIdx;
	u16 IngressTflowNumberOfEntrys;
} gsw_ctpportconfig_t;

typedef struct {
	u8 IndexInUse;
	u16 IndexInUsageCnt;
} gsw_pmapper_t;

typedef struct {
	u8 IndexInUse;
	u16 IndexInUsageCnt;
} gsw_meter_t;

typedef struct {
	u8 usageCount;
} gsw_shaper_t;


typedef struct {
	u16 FilterBlockId;
	u8  IndexInUse;
	u16 IndexInUsageCnt;
	u8 	DiscardUntagged;
	u8	DiscardUnMatchedTagged;
	GSW_VlanFilterTciMask_t FilterMask;
	/*Not available for PRX300-10G*/
	u8	UseDefaultPortVid;
} vlanfilter_entry_idx;

typedef struct {
	vlanfilter_entry_idx filter_idx[VLAN_FILTER_TABLE_SIZE];
	u16 nUsedEntry;
} gsw_vlanfilter_t;

typedef struct {
	u16 num_key;
	u16 num_mask;
	u16 num_val;
} gsw_pce_tbl_info_t;

typedef struct {
	const u32 *key;
	const u32 *mask;
	const u32 *value;
} gsw_pce_tbl_reg_t;

/*Switch IRQ related structures*/
typedef struct gsw_pce_irq gsw_pce_irq;

struct gsw_pce_irq {
	gsw_pce_irq *pNext;
	u32 portId;
	int pce_ier_mask;
	int pce_gier_mask;
	int pce_pier_mask;
	int pce_isr_mask;
	int pce_gisr_mask;
	int pce_pisr_mask;
	void *call_back;
	void *param;
};

struct pce_irq_linklist {
	gsw_pce_irq	*first_ptr;
	gsw_pce_irq	*last_ptr;
};

typedef struct {
	u16 tflowblockid;
	u8 indexinuse;
	u16 indexinusagecnt;
} gsw_tflow_entry_t;

typedef struct {
	gsw_tflow_entry_t flow_idx[GSW_TFLOW_TABLE_SIZE];
	u16 usedentry;
} gsw_tflow_t;

/** \brief Parameter to manage a rule from the packet classification engine.
 */
typedef struct {
	/** Logical Port Id. The valid range is hardware dependent. */
	u32 logicalportid;
	/** Sub interface ID group,
	 *The valid range is hardware/protocol dependent.
	 */
	u32 subifidgroup;
	/** Rule Index in the PCE Table. */
	u32	nIndex;
} GSW_PCE_RuleManage_t;

/* \brief Bitmap used Global PCE Rules */
typedef struct {
	/** Number of Global PCE rules map size. */
	u32 global_rule_map_size;
	/** Bitmap for used Global PCE Rules. */
	unsigned long *global_rule_idx_map;
} GSW_Global_PCE_RuleMap;

typedef struct {
	/**Switch Opertations**/
	struct core_ops ops;
	gsw_devtype_t	sdev;
	port_config_t pconfig[MAX_PORT_NUMBER];
	avlan_tbl_t avtable[VLAN_ACTIVE_TABLE_SIZE];
	stp8021x_t stpconfig;
	gsw_igmp_t iflag;
	ltq_pce_table_t phandler;
	gsw_dscp2pcp_t dscp2pcp_idx[DSCP2PCP_TABLE_SIZE];
	gsw_extendvlan_t extendvlan_idx;
	gsw_vlanfilter_t vlanfilter_idx;
	gsw_brdgconfig_t brdgeconfig_idx[BRDG_CONF_TABLE_SIZE];
	gsw_brdgportconfig_t brdgeportconfig_idx[BRDG_PORTCONF_TABLE_SIZE];
	gsw_ctpportconfig_t ctpportconfig_idx[CTP_PORTCONF_TABLE_SIZE];
	gsw_meter_t meter_idx[METER_TABLE_SIZE];
	gsw_shaper_t shaper_idx[SHAPER_TABLE_SIZE];
	gsw_pmapper_t pmapper_idx[PMAPPER_TABLE_SIZE];
	gsw_tflow_t tflow_idx;
	void *raldev;
	/*platform device struct*/
	void *pdev;
	u16 pnum;
	u16 tpnum;			/* Total number of ports including vitual ports*/
	u16 mpnum; 			/* ManagementPortNumber */
	u32 aging_timeout;
	ltq_bool_t rst;
	ltq_bool_t hwinit;
	u16 vlan_rd_index; 		/* read VLAN table index */
	u16 mac_rd_index; 		/* read mac table index */
	u16 mhw_rinx;
	u16 msw_rinx;
	u8 cport;
	u8 gsw_dev;

	u32 parent_devid;
	u32 ext_devid;
	u32 ext_phyid;

	u16 mrtpcnt;			/* multicast router port count */
	u16 meter_cnt;
	u16 num_of_queues; 		/* Number of priority queues . */
	u16 num_of_meters;  		/* Number of traffic meters */
	u16 num_of_shapers; 		/* Number of traffic shapers */
	u16 num_of_pppoe;		/* PPPoE table size  */
	u16 avlantsz; 			/* Active VLAN table size */
	u16 ip_pkt_lnt_size;	 	/* IP packet length table size */
	u16 prot_table_size;	 	/* Protocol table size */
	u16 mac_dasa_table_size; 	/* MAC DA/SA table size */
	u16 app_table_size;		/* Application table size */
	u16 idsmtblsize;		/* IP DA/SA MSB table size */
	u16 idsltblsize;		/* IP DA/SA LSB table size*/
	u16 mctblsize;			/* Multicast table size */
	u16 tftblsize;			/* Flow Aggregation table size */
	u16 mactblsize;			/* MAC bridging table size */
	u16 num_of_pmac;		/* Number of PMAC */
	u16 pdtblsize;			/* Payload Table Size  */
	u16 num_of_ifrmon;		/* Interface RMON Counter Table Size */
	u16 num_of_egvlan;		/* Egress VLAN Treatment Table Size */
	u16 num_of_rt_smac;		/* Routing MAC Table Size for Source MAC */
	u16 num_of_rt_dmac;		/* Routing MAC Table Size for Destination MAC */
	u16 num_of_rt_ppoe;		/* Routing PPPoE Table Size  */
	u16 num_of_rt_nat;		/* Routing Session Table Size */
	u16 num_of_rt_mtu;		/* Routing MTU Table Size */
	u16 num_of_rt_tunnel;		/* Routing Tunnel Table Size  */
	u16 num_of_rt_rtp;		/* Routing RTP Table Size */

	u16 num_of_bridge;		/*Number of Bridge - Applicable for 3.1*/
	u16 num_of_bridge_port;		/*Number of Bridge Port - Applicable for 3.1*/
	u16 num_of_ctp;			/*Number of CTP port - Applicable for 3.1 */
	u16 num_of_extendvlan;		/*Number of extended VLAN tagging operation index in table  - Applicable for 3.1 */
	u16 num_of_vlanfilter;		/*Number of VLAN Filter index in table  - Applicable for 3.1 */
	u16 num_of_pmapper;		/*Number of pmapper index in table  - Applicable for 3.1
					  p-mapper total entry 2336/73=32 number of index (i.e) each pmapper idx
					  has 73 entries*/
	u16 mcsthw_snoop;		/*Multicast HW snooping feature */


	u16 gipver;
	void *gswl_base;		/*Base address GSWIP-L */
	void *gswr_base;		/*Base address GSWIP-R */
	u32 gswex_base;			/*Base address GSWIP External switch */
	u32 gswex_sgmiibase;		/*Base address GSWIP External switch-SGMII */

	void *gsw_base;			/*Base address GSWITCH */
	u32 gsw_mode;			/* GSWIP Mode 0 "short_cut", 1 "full_qos" */
	u32 dpu;			/* DPU = 1, pmac G.INT config, DPU = 0, Non-G.INT config */
	u32 num_of_global_rules;	/* Number of Global(Common) TFLOW Rules */
	u16 hitstatus_idx;
	u16 hitstatus_mask;
	u32 ext_switch;			/* External Switch or Internal Switch */
	u32 devid;		/* Device id, device instance number */
	u32 eg_sptag;	/*Special Tag Insertion Enable/Disable */
	u32 prod_id;	/* Product id, get each SOC ID */

#ifdef __KERNEL__
	spinlock_t lock_pce;
	spinlock_t lock_bm;
	spinlock_t lock_pmac;
	spinlock_t lock_misc;
	spinlock_t lock_pae;
	spinlock_t lock_ctp_map;
	spinlock_t lock_irq;
	spinlock_t lock_mdio;
	spinlock_t lock_mmd;
	spinlock_t lock_pce_tbl;	/* lock of PCE Table register */
	struct tasklet_struct gswip_tasklet;
	struct device_node *master_mii_node; /* MDIO node for Master MDIO */
	struct mii_bus *master_mii_bus;	/* MII bus for Mater MDIO */
	struct device *dev;
	struct regmap *chiptop_base;
#endif

	u16 num_of_pce_tbl;
	const gsw_pce_tbl_info_t *pce_tbl_info;
	gsw_pce_tbl_reg_t pce_tbl_reg;
	/*IRQ*/
	struct pce_irq_linklist *PceIrqList;
	u32  irq_num;
	GSW_Global_PCE_RuleMap Global_Rule_BitMap;
} ethsw_api_dev_t;



u8 find_active_vlan_index(void *cdev, u16 vid);
int find_msb_tbl_entry(pcetbl_prog_t *ptbl,
		       pce_dasa_msb_t *parm);
int pce_dasa_msb_tbl_write(void *cdev, pcetbl_prog_t *ptbl,
			   pce_dasa_msb_t *parm);
int find_dasa_tbl_entry(pcetbl_prog_t *ptbl,
			pce_dasa_lsb_t *parm);
int pce_dasa_lsb_tbl_write(void *cdev, pcetbl_prog_t *ptbl,
			   pce_dasa_lsb_t *parm);
int pce_table_init(ltq_pce_table_t *pchndl);
int ip_dasa_msb_tbl_del(void *cdev, pcetbl_prog_t *ptbl, u32 index);
int ip_dasa_lsb_tbl_del(void *cdev, pcetbl_prog_t *ptbl, u32 index);
int ipdsmsb_tblidx_del(pcetbl_prog_t *ptbl, u32 index);
int gavlan_tbl_index(pcetbl_prog_t *ptbl, u8 index);
int pce_pattern_delete(void *cdev, ltq_pce_table_t *pthandle, u32 index);
int ipdslsb_tblidx_del(pcetbl_prog_t *ptbl, u32 index);
int pce_action_delete(void *cdev, ltq_pce_table_t *pthandle, u32 index);
int pce_rule_read(void *cdev, ltq_pce_table_t *pthandle,
		  GSW_PCE_rule_t *parm);
int pce_rule_write(void *cdev, ltq_pce_table_t *pthandle,
		   GSW_PCE_rule_t *parm);
int gsw_pce_table_key_write(void *cdev, pctbl_prog_t *ptdata);
int gsw_pce_table_key_read(void *cdev, pctbl_prog_t *ptdata);
GSW_return_t GSW_Freeze(void);
GSW_return_t GSW_UnFreeze(void);

#endif    /* _LTQ_FLOW_CORE_H_ */
