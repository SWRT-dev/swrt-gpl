/******************************************************************************
				Copyright (c) 2016, 2019 Intel Corporation

******************************************************************************/
/*****************************************************************************
				Copyright (c) 2012, 2014, 2015
					Lantiq Deutschland GmbH
	For licensing information, see the file 'LICENSE' in the root folder of
	this software module.
******************************************************************************/
#ifdef __KERNEL__
#include <linux/jiffies.h>
#include <linux/timer.h>

#ifndef CONFIG_X86_INTEL_CE2700
#include <lantiq_soc.h>
#endif
#endif

#include <gsw_init.h>
#include <gsw_swmcast.h>
#include <gsw_defconf.h>

#if defined(CONFIG_MAC) && CONFIG_MAC
#include <gswss_api.h>
#endif
#if defined(WIN_PC_MODE) && WIN_PC_MODE
#include <eip160_common.h>
#endif

//#define CONFIG_USE_EMULATOR 1
/* Global Variables */
ethsw_api_dev_t *ecoredev[LTQ_FLOW_DEV_MAX];
u8 VERSION_NAME[] = GSW_API_MODULE_NAME " for GSWITCH Platform";
u8 VERSION_NUMBER[] = GSW_API_DRV_VERSION;
u8 MICRO_CODE_VERSION_NAME[] = " GSWIP macro revision ID";
u8 MICRO_CODE_VERSION_NUMBER[] = "0x"MICRO_CODE_VERSION;
/* Port redirect flag */
static u8 prdflag;
static void get_gsw_hw_cap(void *cdev);
static GSW_return_t gsw_init_fn_ptrs(struct core_ops *ops);

static const u32 gsw_pce_tbl_reg_key[] = {
	PCE_TBL_KEY_0_KEY0_OFFSET, PCE_TBL_KEY_1_KEY1_OFFSET,
	PCE_TBL_KEY_2_KEY2_OFFSET, PCE_TBL_KEY_3_KEY3_OFFSET,
	PCE_TBL_KEY_4_KEY4_OFFSET, PCE_TBL_KEY_5_KEY5_OFFSET,
	PCE_TBL_KEY_6_KEY6_OFFSET, PCE_TBL_KEY_7_KEY7_OFFSET,
	PCE_TBL_KEY_8_KEY8_OFFSET, PCE_TBL_KEY_9_KEY9_OFFSET,
	PCE_TBL_KEY_10_KEY10_OFFSET, PCE_TBL_KEY_11_KEY11_OFFSET,
	PCE_TBL_KEY_12_KEY12_OFFSET, PCE_TBL_KEY_13_KEY13_OFFSET,
	PCE_TBL_KEY_14_KEY14_OFFSET, PCE_TBL_KEY_15_KEY15_OFFSET,
	PCE_TBL_KEY_16_KEY16_OFFSET, PCE_TBL_KEY_17_KEY17_OFFSET,
	PCE_TBL_KEY_18_KEY18_OFFSET, PCE_TBL_KEY_19_KEY19_OFFSET,
	PCE_TBL_KEY_20_KEY20_OFFSET, PCE_TBL_KEY_21_KEY21_OFFSET
};
static const u32 gsw_pce_tbl_reg_mask[] = {
	PCE_TBL_MASK_0_MASK0_OFFSET, PCE_TBL_MASK_1_MASK1_OFFSET,
	PCE_TBL_MASK_2_MASK2_OFFSET, PCE_TBL_MASK_3_MASK3_OFFSET
};
static const u32 gsw_pce_tbl_reg_value[] = {
	PCE_TBL_VAL_0_VAL0_OFFSET, PCE_TBL_VAL_1_VAL1_OFFSET,
	PCE_TBL_VAL_2_VAL2_OFFSET, PCE_TBL_VAL_3_VAL3_OFFSET,
	PCE_TBL_VAL_4_VAL4_OFFSET, PCE_TBL_VAL_5_VAL5_OFFSET,
	PCE_TBL_VAL_6_VAL6_OFFSET, PCE_TBL_VAL_7_VAL7_OFFSET,
	PCE_TBL_VAL_8_VAL8_OFFSET, PCE_TBL_VAL_9_VAL9_OFFSET,
	PCE_TBL_VAL_10_VAL10_OFFSET, PCE_TBL_VAL_11_VAL11_OFFSET,
	PCE_TBL_VAL_12_VAL12_OFFSET, PCE_TBL_VAL_13_VAL13_OFFSET,
	PCE_TBL_VAL_14_VAL14_OFFSET, PCE_TBL_VAL_15_VAL15_OFFSET,
	PCE_TBL_VAL_16_VAL16_OFFSET, PCE_TBL_VAL_17_VAL17_OFFSET,
	PCE_TBL_VAL_18_VAL18_OFFSET, PCE_TBL_VAL_19_VAL19_OFFSET,
	PCE_TBL_VAL_20_VAL20_OFFSET, PCE_TBL_VAL_21_VAL21_OFFSET,
	PCE_TBL_VAL_22_VAL22_OFFSET, PCE_TBL_VAL_23_VAL23_OFFSET,
	PCE_TBL_VAL_24_VAL24_OFFSET, PCE_TBL_VAL_25_VAL25_OFFSET
};
static const gsw_pce_tbl_info_t gsw_pce_tbl_info_22[] = {
	{0, 0, 4}, {1, 1, 1}, {0, 0, 3}, {1, 0, 0}, {1, 1, 0}, {1, 1, 0}, {4, 1, 0}, {4, 1, 0},
	{1, 1, 0}, {0, 0, 1}, {0, 0, 1}, {4, 0, 2}, {3, 1, 0}, {2, 0, 1}, {2, 0, 5}, {9, 0, 7},
	{0, 0, 1}, {0, 0, 1}, {1, 1, 1}, {1, 1, 1}, {4, 1, 0}, {4, 1, 0}, {3, 1, 0}, {3, 1, 0},
	{0, 0, 0}, {0, 0, 4}, {0, 0, 1}, {0, 0, 1}, {0, 0, 2}
};
static const gsw_pce_tbl_info_t gsw_pce_tbl_info_30[] = {
	{0, 0, 4}, {1, 1, 0}, {0, 0, 3}, {1, 0, 0}, {1, 1, 0}, {1, 1, 0}, {4, 4, 0}, {4, 4, 0},
	{1, 1, 0}, {0, 0, 1}, {0, 0, 1}, {4, 0, 2}, {0, 0, 0}, {3, 0, 2}, {2, 0, 5}, {16, 0, 10},
	{0, 0, 0}, {0, 0, 1}, {1, 1, 1}, {1, 1, 1}, {0, 0, 0}, {0, 0, 0}, {3, 1, 0}, {3, 1, 0},
	{1, 1, 0}, {0, 0, 0}, {0, 0, 1}, {0, 0, 1}, {0, 0, 2}, {1, 1, 0}, {0, 0, 2}
};
static const gsw_pce_tbl_info_t gsw_pce_tbl_info_31[] = {
	{0, 0, 4}, {0, 0, 0}, {1, 0, 1}, {1, 0, 0}, {1, 1, 0}, {1, 1, 0}, {4, 4, 0}, {4, 4, 0},
	{1, 1, 0}, {0, 0, 1}, {0, 0, 1}, {4, 0, 10}, {0, 0, 2}, {19, 0, 10}, {2, 0, 5}, {22, 0, 18},
	{0, 0, 0}, {0, 0, 1}, {0, 0, 9}, {0, 0, 7}, {0, 0, 18}, {0, 0, 14}, {3, 1, 0}, {3, 1, 0},
	{1, 1, 0}, {0, 0, 10}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {1, 1, 0}, {4, 0, 6}, {0, 0, 1}
};

/* Local Macros & Definitions */
static pstpstate_t pstpstate[] = {
	{
		GSW_STP_PORT_STATE_FORWARD, GSW_8021X_PORT_STATE_AUTHORIZED,
		1, PORT_STATE_FORWARDING, 1
	},
	{
		GSW_STP_PORT_STATE_FORWARD, GSW_8021X_PORT_STATE_UNAUTHORIZED,
		1, PORT_STATE_LISTENING, 0
	}, /* Govind - set 'lrnlim' to 0. Double check for legacy code. */
	{
		GSW_STP_PORT_STATE_FORWARD, GSW_8021X_PORT_STATE_RX_AUTHORIZED,
		1, PS_RENABLE_TDISABLE, 1
	},
	{
		GSW_STP_PORT_STATE_FORWARD, GSW_8021X_PORT_STATE_TX_AUTHORIZED,
		1, PS_RDISABLE_TENABLE, 0
	}, /* Govind - set 'lrnlim' to 0. Double check for legacy code. */
	{
		GSW_STP_PORT_STATE_DISABLE, GSW_8021X_PORT_STATE_AUTHORIZED,
		0, PORT_STATE_LISTENING, 0
	},
	{
		GSW_STP_PORT_STATE_DISABLE, GSW_8021X_PORT_STATE_UNAUTHORIZED,
		0, PORT_STATE_LISTENING, 0
	},
	{
		GSW_STP_PORT_STATE_DISABLE, GSW_8021X_PORT_STATE_RX_AUTHORIZED,
		0, PORT_STATE_LISTENING, 0
	},
	{
		GSW_STP_PORT_STATE_DISABLE, GSW_8021X_PORT_STATE_TX_AUTHORIZED,
		0, PORT_STATE_LISTENING, 0
	},
	{
		GSW_STP_PORT_STATE_LEARNING, GSW_8021X_PORT_STATE_AUTHORIZED,
		1, PORT_STATE_LEARNING, 1
	},
	{
		GSW_STP_PORT_STATE_LEARNING, GSW_8021X_PORT_STATE_UNAUTHORIZED,
		1, PORT_STATE_LEARNING, 1
	},
	{
		GSW_STP_PORT_STATE_LEARNING, GSW_8021X_PORT_STATE_RX_AUTHORIZED,
		1, PORT_STATE_LEARNING, 1
	},
	{
		GSW_STP_PORT_STATE_LEARNING, GSW_8021X_PORT_STATE_TX_AUTHORIZED,
		1, PORT_STATE_LEARNING, 1
	},
	{
		GSW_STP_PORT_STATE_BLOCKING, GSW_8021X_PORT_STATE_AUTHORIZED,
		1, PORT_STATE_LISTENING, 0
	},
	{
		GSW_STP_PORT_STATE_BLOCKING, GSW_8021X_PORT_STATE_UNAUTHORIZED,
		1, PORT_STATE_LISTENING, 0
	},
	{
		GSW_STP_PORT_STATE_BLOCKING, GSW_8021X_PORT_STATE_RX_AUTHORIZED,
		1, PORT_STATE_LISTENING, 0
	},
	{
		GSW_STP_PORT_STATE_BLOCKING, GSW_8021X_PORT_STATE_TX_AUTHORIZED,
		1, PORT_STATE_LISTENING, 0
	}
};

static gsw_capdesc_t capdes[] = {
	{ GSW_CAP_TYPE_PORT, "Number of Ethernet ports"},
	{ GSW_CAP_TYPE_VIRTUAL_PORT, "Number of virtual ports"},
	{ GSW_CAP_TYPE_BUFFER_SIZE, "Pcket buffer size[in Bytes]:"},
	{ GSW_CAP_TYPE_SEGMENT_SIZE, "Buffer Segment size:"},
	{ GSW_CAP_TYPE_PRIORITY_QUEUE, "Number of queues:"},
	{ GSW_CAP_TYPE_METER, "Number of traffic meters:"},
	{ GSW_CAP_TYPE_RATE_SHAPER, "Number of rate shapers:"},
	{ GSW_CAP_TYPE_VLAN_GROUP, "Number of VLAN groups:"},
	{ GSW_CAP_TYPE_FID, "Number of FIDs:"},
	{ GSW_CAP_TYPE_MAC_TABLE_SIZE, "Number of MAC entries:"},
	{ GSW_CAP_TYPE_MULTICAST_TABLE_SIZE, "Number of multicast entries"},
	{ GSW_CAP_TYPE_PPPOE_SESSION, "Number of PPPoE sessions:"},
	{ GSW_CAP_TYPE_SVLAN_GROUP, "Number of STAG VLAN groups:"},
	{ GSW_CAP_TYPE_PMAC, "Number of PMACs:"},
	{ GSW_CAP_TYPE_PAYLOAD, "Number of entries in Payload Table size:"},
	{ GSW_CAP_TYPE_IF_RMON, "Number of IF RMON Counters:"},
	{ GSW_CAP_TYPE_EGRESS_VLAN, "Number of Egress VLAN Entries:"},
	{ GSW_CAP_TYPE_RT_SMAC, "Number of Routing Source-MAC Entries:"},
	{ GSW_CAP_TYPE_RT_DMAC, "Number of Routing Dest-MAC Entries:"},
	{ GSW_CAP_TYPE_RT_PPPoE, "Number of Routing-PPPoE Entries:"},
	{ GSW_CAP_TYPE_RT_NAT, "Number of Routing-NAT Entries:"},
	{ GSW_CAP_TYPE_RT_MTU, "Number of MTU Entries:"},
	{ GSW_CAP_TYPE_RT_TUNNEL, "Number of Tunnel Entries:"},
	{ GSW_CAP_TYPE_RT_RTP, "Number of RTP Entries:"},
	{ GSW_CAP_TYPE_CTP, "Number of CTP ports"},
	{ GSW_CAP_TYPE_BRIDGE_PORT, "Number of bridge ports"},
	{ GSW_CAP_TYPE_LAST, "Last Capability Index"}
};

/*****************/
/* Function Body */
/*****************/
#define GAP_MAX	50
#define GAP_MAX1	1000
#define MAX_READ 20
#define REPEAT_L 10
#define REPEAT_M 10
#define NOT_ASSIGNED	   0xFF


struct mac_ops *get_mac_ops(ethsw_api_dev_t *gswdev, int idx)
{
#ifdef __KERNEL__
	u32 dev_id = (((struct platform_device *)(gswdev->pdev))->dev.parent->id);
#else
	u32 dev_id = 0;
#endif
	u32 max_mac = gsw_get_mac_subifcnt(dev_id);

	if (idx < MAC_2 || idx > ((max_mac + MAC_2) - 1)) {
		pr_err("Invalid MAC Idx %d, Only MAC idx > 2 is Allowed\n", idx);
		return NULL;
	}

	return gsw_get_mac_ops(dev_id, idx);
}

struct adap_ops *get_adap_ops(ethsw_api_dev_t *gswdev)
{
#ifdef __KERNEL__
	u32 dev_id = (((struct platform_device *)(gswdev->pdev))->dev.parent->id);
#else
	u32 dev_id = 0;
#endif
	return gsw_get_adap_ops(dev_id);
}

static int calc_credit_value(u32 *rxcnt, u8 *mv, u8 *locv)
{
	u8 s, c, i, m, loc = 0, crd[MAX_READ] = {0};

	for (s = 0; s < MAX_READ - 1; s++) {
		for (c = s + 1; c < MAX_READ; c++) {
			if ((rxcnt[s] <= rxcnt[c]) && (rxcnt[c] <= (rxcnt[s] + (c - s) * GAP_MAX))) {
				crd[s]++;
				crd[c]++;
			}
		}
	}

	m = crd[0];

	for (i = 1; i < MAX_READ; i++) {
		if (crd[i] > m) {
			m = crd[i];
			loc = i;
		}
	}

	*mv = m;
	*locv = loc;
	return 0;
}

static int calc_credit_byte_value(u32 *rxcnt, u8 *mv, u8 *locv)
{
	u8 s, c, i, m, loc = 0, crd[MAX_READ] = {0};

	for (s = 0; s < MAX_READ - 1; s++) {
		for (c = s + 1; c < MAX_READ; c++) {
			if ((rxcnt[s] <= rxcnt[c]) && (rxcnt[c] <= (rxcnt[s] + (c - s) * GAP_MAX1))) {
				crd[s]++;
				crd[c]++;
			}
		}
	}

	m = crd[0];

	for (i = 1; i < MAX_READ; i++) {
		if (crd[i] > m) {
			m = crd[i];
			loc = i;
		}
	}

	*mv = m;
	*locv = loc;
	return 0;
}

#ifdef CONFIG_X86_INTEL_CE2700
static void cport_sgmii_config(void *cdev)
{
	gsw_w32(cdev, 0xfa01, 0, 16, 0x0030);
	gsw_w32(cdev, 0xfa01, 0, 14, 0x0010);
	gsw_w32(cdev, 0xD009, 0, 16, 0x0009);
	gsw_w32(cdev, 0xD004, 0, 16, 0x053A);
	/*Reset of TBI FSM's*/
	gsw_w32(cdev, 0xD305, 0, 16, 0x0033);
	/*Release  TBI FSM's*/
	gsw_w32(cdev, 0xD305, 0, 16, 0x0032);
	/*  TX Buffer and pointers are initialized */
	gsw_w32(cdev, 0xD404, 0, 16, 0x0003);
	/* TX Buffer and pointers are normally operating */
	gsw_w32(cdev, 0xD404, 0, 16, 0x0001);
	/*  RX Buffer and pointers are initialized */
	gsw_w32(cdev, 0xD401, 0, 16, 0x0003);
	/*  RX Buffer and pointers are normally operating */
	gsw_w32(cdev, 0xD401, 0, 16, 0x0001);
	gsw_w32(cdev, 0xD300, 0, 16, 0x0018);
	gsw_w32(cdev, 0xD301, 0, 16, 0x0001);
	/*  Restart Autonegotiation */
	gsw_w32(cdev, 0xD304, 0, 16, 0x803c);
	/*SGMII_PHY SGMII PHY mode */
	gsw_w32(cdev, 0xD304, 0, 16, 0x80b4);
}
#endif

static void ltq_ethsw_port_cfg_init(void *cdev)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 i, value;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return;
	}


	gsw_r32(cdev, ETHSW_CAP_1_PPORTS_OFFSET,
		ETHSW_CAP_1_PPORTS_SHIFT,
		ETHSW_CAP_1_PPORTS_SIZE, &value);
	gswdev->pnum = value;

	for (i = 0; i < gswdev->pnum; i++) {
		memset(&gswdev->pconfig[i], 0, sizeof(port_config_t));
		gswdev->pconfig[i].llimit = 0xFF;
		gswdev->pconfig[i].penable = 1;
	}

	gswdev->stpconfig.sfport = GSW_PORT_FORWARD_DEFAULT;
	gswdev->stpconfig.fpid8021x = gswdev->cport;
	gsw_r32(cdev, ETHSW_CAP_1_VPORTS_OFFSET, ETHSW_CAP_1_VPORTS_SHIFT,
		ETHSW_CAP_1_VPORTS_SIZE, &value);
	gswdev->tpnum = value + gswdev->pnum;
}
#if defined(CONFIG_LTQ_MULTICAST) && CONFIG_LTQ_MULTICAST
static void reset_multicast_sw_table(void *cdev)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 i;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return;
	}

	if (IS_VRSN_31(gswdev->gipver)) {
		gsw_init_hash_table(cdev);
	}

	memset(&gswdev->iflag, 0, sizeof(gsw_igmp_t));
	gswdev->iflag.itblsize = gswdev->mctblsize;

	for (i = 0; i < gswdev->iflag.itblsize; i++) {
		gswdev->iflag.mctable[i].slsbindex = 0x7F;
		gswdev->iflag.mctable[i].dlsbindex = 0x7F;
		gswdev->iflag.mctable[i].smsbindex = 0x1F;
		gswdev->iflag.mctable[i].dmsbindex = 0x1F;
	}
}

/* Multicast Software Table Include/Exclude Add function */
static int gsw2x_msw_table_wr(void *cdev, GSW_multicastTable_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	int	i, j, exclude_rule = 0;
	int	dlix = 0, dmix = 0;
	int slix = 0, smix = 0;
	int dlflag = 0, dmflag = 0, slflag = 0, smflag = 0;
	ltq_bool_t new_entry = 0;
	pctbl_prog_t ptdata;
	pce_dasa_lsb_t	ltbl;
	pce_dasa_msb_t	mtbl;
	ltq_pce_table_t *hpctbl = NULL;
	gsw_igmp_t *hitbl = NULL;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}


	hpctbl = &gswdev->phandler;
	hitbl = &gswdev->iflag;


	memset(&ptdata, 0, sizeof(pctbl_prog_t));
	memset(&ltbl, 0, sizeof(pce_dasa_lsb_t));
	memset(&mtbl, 0, sizeof(pce_dasa_msb_t));

	if ((parm->eModeMember == GSW_IGMP_MEMBER_INCLUDE)
	    || (parm->eModeMember == GSW_IGMP_MEMBER_EXCLUDE)) {
		if (gswdev->iflag.igv3 != 1) {
			pr_err("%s:%s:%d(bIGMPv3 need to be enable)\n",
			       __FILE__, __func__, __LINE__);
			return GSW_statusErr;
		}
	}

	if ((parm->eIPVersion != GSW_IP_SELECT_IPV4)
	    && (parm->eIPVersion != GSW_IP_SELECT_IPV6)) {
		pr_err("%s:%s:%d (IPv4/IPV6 need to enable)\n",
		       __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

	if (parm->eIPVersion == GSW_IP_SELECT_IPV4) {
		for (i = 0; i < 4; i++)
			ltbl.ilsb[i] =
				((parm->uIP_Gda.nIPv4 >> (i * 8)) & 0xFF);

		if (gswdev->gipver == LTQ_GSWIP_3_0) {
			ltbl.mask[0] = 0;
			ltbl.mask[1] = 0;
			ltbl.mask[2] = 0xFFFF;
			ltbl.mask[3] = 0xFFFF;
		} else {
			ltbl.mask[0] = 0xFF00;
		}
	}

	if (parm->eIPVersion == GSW_IP_SELECT_IPV6) {
		for (i = 0, j = 7; i < 4; i++, j -= 2) {
			mtbl.imsb[j - 1] = (parm->uIP_Gda.nIPv6[i] & 0xFF);
			mtbl.imsb[j] = ((parm->uIP_Gda.nIPv6[i] >> 8) & 0xFF);
		}

		if (gswdev->gipver == LTQ_GSWIP_3_0) {
			mtbl.mask[0] = 0;
			mtbl.mask[1] = 0;
			mtbl.mask[2] = 0;
			mtbl.mask[3] = 0;
		} else {
			mtbl.mask[0] = 0;
		}

		dmix = find_msb_tbl_entry(&hpctbl->pce_sub_tbl, &mtbl);

		if (dmix == 0xFF) {
			dmix = pce_dasa_msb_tbl_write(cdev,
						      &hpctbl->pce_sub_tbl, &mtbl);
			dmflag = 1;
		}

		if (dmix < 0) {
			pr_err("%s:%s:%d (IGMP Table full)\n",
			       __FILE__, __func__, __LINE__);
			return GSW_statusErr;
		}

		/* First, search for DIP in the DA/SA table (DIP LSB) */
		for (i = 0, j = 7; i < 4; i++, j -= 2) {
			ltbl.ilsb[j - 1] = (parm->uIP_Gda.nIPv6[i + 4] & 0xFF);
			ltbl.ilsb[j] = ((parm->uIP_Gda.nIPv6[i + 4] >> 8) & 0xFF);
		}

		if (gswdev->gipver == LTQ_GSWIP_3_0) {
			ltbl.mask[0] = 0;
			ltbl.mask[1] = 0;
			ltbl.mask[2] = 0;
			ltbl.mask[3] = 0;
		} else {
			ltbl.mask[0] = 0;/* DIP LSB Nibble Mask */
		}
	}

	dlix = find_dasa_tbl_entry(&hpctbl->pce_sub_tbl, &ltbl);

	if (dlix == 0xFF) {
		dlix = pce_dasa_lsb_tbl_write(cdev,
					      &hpctbl->pce_sub_tbl, &ltbl);
		dlflag = 1;
	}

	if (dlix < 0) {
		pr_err("%s:%s:%d (IGMP Table full)\n",
		       __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

	if ((parm->eModeMember == GSW_IGMP_MEMBER_INCLUDE)
	    || (parm->eModeMember == GSW_IGMP_MEMBER_EXCLUDE)) {
		if (parm->eIPVersion == GSW_IP_SELECT_IPV4) {
			for (i = 0; i < 4; i++)
				ltbl.ilsb[i] =
					((parm->uIP_Gsa.nIPv4 >> (i * 8)) & 0xFF);

			if (gswdev->gipver == LTQ_GSWIP_3_0) {
				ltbl.mask[0] = 0x0;
				ltbl.mask[1] = 0x0;
				ltbl.mask[2] = 0xFFFF;
				ltbl.mask[3] = 0xFFFF;
			} else {
				/* DIP LSB Nibble Mask */
				ltbl.mask[0] = 0xFF00;
			}

			if (parm->eModeMember == GSW_IGMP_MEMBER_EXCLUDE) {
				if ((ltbl.ilsb[3] == 0) &&
				    (ltbl.ilsb[2] == 0) &&
				    (ltbl.ilsb[1] == 0) &&
				    (ltbl.ilsb[0] == 0)) {
					pr_err("%s:%s:%d (Exclude Rule Source IP is Wildcard)\n",
					       __FILE__, __func__, __LINE__);
					return GSW_statusErr;
				}
			}
		}

		if (parm->eIPVersion == GSW_IP_SELECT_IPV6) {
			int src_zero = 0;

			/*First, search for DIP in the DA/SA table (DIP MSB)*/
			for (i = 0, j = 7; i < 4; i++, j -= 2) {
				mtbl.imsb[j - 1] =
					(parm->uIP_Gsa.nIPv6[i] & 0xFF);
				mtbl.imsb[j] =
					((parm->uIP_Gsa.nIPv6[i] >> 8) & 0xFF);
			}

			if (gswdev->gipver == LTQ_GSWIP_3_0) {
				mtbl.mask[0] = 0;
				mtbl.mask[1] = 0;
				mtbl.mask[2] = 0;
				mtbl.mask[3] = 0;
			} else {
				mtbl.mask[0] = 0;/* DIP MSB Nibble Mask */
			}

			if (parm->eModeMember == GSW_IGMP_MEMBER_EXCLUDE) {
				if ((mtbl.imsb[0] == 0) &&
				    (mtbl.imsb[1] == 0) &&
				    (mtbl.imsb[2] == 0) &&
				    (mtbl.imsb[3] == 0) &&
				    (mtbl.imsb[4] == 0) &&
				    (mtbl.imsb[5] == 0) &&
				    (mtbl.imsb[6] == 0) &&
				    (mtbl.imsb[7] == 0)) {
					src_zero = 1;
				}
			}

			/* First, search for DIP in the DA/SA table (DIP LSB) */
			for (i = 0, j = 7; i < 4; i++, j -= 2) {
				ltbl.ilsb[j - 1] =
					(parm->uIP_Gsa.nIPv6[i + 4] & 0xFF);
				ltbl.ilsb[j] =
					((parm->uIP_Gsa.nIPv6[i + 4] >> 8) & 0xFF);
			}

			if (gswdev->gipver == LTQ_GSWIP_3_0) {
				ltbl.mask[0] = 0;
				ltbl.mask[1] = 0;
				ltbl.mask[2] = 0;
				ltbl.mask[3] = 0;
			} else {
				ltbl.mask[0] = 0;/* DIP LSB Nibble Mask */
			}

			if (parm->eModeMember == GSW_IGMP_MEMBER_EXCLUDE) {
				if ((ltbl.ilsb[0] == 0) &&
				    (ltbl.ilsb[1] == 0) &&
				    (ltbl.ilsb[2] == 0) &&
				    (ltbl.ilsb[3] == 0) &&
				    (ltbl.ilsb[4] == 0) &&
				    (ltbl.ilsb[5] == 0) &&
				    (ltbl.ilsb[6] == 0) &&
				    (ltbl.ilsb[7] == 0)) {
					if (src_zero) {
						pr_err("%s:%s:%d (Exclude Rule Source IP is Wildcard)\n",
						       __FILE__, __func__, __LINE__);
						return GSW_statusErr;
					}
				}
			}

			smix = find_msb_tbl_entry(&hpctbl->pce_sub_tbl, &mtbl);

			if (smix == 0xFF) {
				smix = pce_dasa_msb_tbl_write(cdev,
							      &hpctbl->pce_sub_tbl, &mtbl);
				smflag = 1;
			}

			if (smix < 0) {
				pr_err("%s:%s:%d (IGMP Table full)\n",
				       __FILE__, __func__, __LINE__);
				return GSW_statusErr;
			}
		}

		slix = find_dasa_tbl_entry(&hpctbl->pce_sub_tbl, &ltbl);

		if (slix == 0xFF) {
			slix = pce_dasa_lsb_tbl_write(cdev,
						      &hpctbl->pce_sub_tbl, &ltbl);
			slflag = 1;
		}

		if (slix < 0) {
			pr_err("%s:%s:%d (IGMP Table full)\n",
			       __FILE__, __func__, __LINE__);
			return GSW_statusErr;
		}
	}

	/* update the entry for another port number if already exists*/
	for (i = 0; i < gswdev->iflag.itblsize; i++) {
		/* Check if port was already exist */
		if ((hitbl->mctable[i].dlsbindex == dlix) &&
		    (hitbl->mctable[i].dmsbindex == dmix) &&
		    (hitbl->mctable[i].slsbindex == slix) &&
		    (hitbl->mctable[i].smsbindex == smix) &&
		    (hitbl->mctable[i].valid == 1)) {
			if (((hitbl->mctable[i].pmap >> parm->nPortId)
			     & 0x1) == 1)
				return GSW_statusOk;

			switch (hitbl->mctable[i].mcmode) {
			case GSW_IGMP_MEMBER_DONT_CARE:
				hpctbl->pce_sub_tbl.iplsbtcnt[dlix]++;

				if (parm->eIPVersion == GSW_IP_SELECT_IPV6)
					hpctbl->pce_sub_tbl.ipmsbtcnt[dmix]++;

				/* Add the port */
				hitbl->mctable[i].pmap |= (1 << parm->nPortId);
				break;

			case GSW_IGMP_MEMBER_EXCLUDE:
				if (gswdev->gipver == LTQ_GSWIP_3_0)
					exclude_rule = 0;
				else
					exclude_rule = 1;

			case GSW_IGMP_MEMBER_INCLUDE:
				/* Add the port */
				hitbl->mctable[i].pmap |= (1 << parm->nPortId);
				hpctbl->pce_sub_tbl.iplsbtcnt[dlix]++;
				hpctbl->pce_sub_tbl.iplsbtcnt[slix]++;

				if (parm->eIPVersion == GSW_IP_SELECT_IPV6) {
					hpctbl->pce_sub_tbl.ipmsbtcnt[dmix]++;
					hpctbl->pce_sub_tbl.ipmsbtcnt[smix]++;
				}

				break;
			} /* end switch */

			/* Now, we write into Multicast SW Table */
			memset(&ptdata, 0, sizeof(pctbl_prog_t));
			ptdata.table = PCE_MULTICAST_SW_INDEX;
			ptdata.pcindex = i;
			ptdata.key[1] = (hitbl->mctable[i].smsbindex << 8)
					| hitbl->mctable[i].slsbindex;
			ptdata.key[0] = (hitbl->mctable[i].dmsbindex << 8)
					| hitbl->mctable[i].dlsbindex;

			if (gswdev->gipver == LTQ_GSWIP_3_0) {
				ptdata.val[0] = hitbl->mctable[i].pmap;
				ptdata.val[1] =
					(parm->nSubIfId & 0xFFFF)	<< 3;
				ptdata.key[2] = parm->nFID & 0x3F;

				if (parm->eModeMember ==
				    GSW_IGMP_MEMBER_EXCLUDE) {
					ptdata.key[2] |= (1 << 15);
					/*ptdata.key[2] |= (parm->bExclSrcIP & 1) << 15; */
				} else {
					ptdata.key[2] &= ~(1 << 15);
				}
			} else {
				if (parm->eModeMember ==
				    GSW_IGMP_MEMBER_EXCLUDE)
					ptdata.val[0] = (0 << parm->nPortId);
				else
					ptdata.val[0] = hitbl->mctable[i].pmap;
			}

			ptdata.valid = hitbl->mctable[i].valid;
			gsw_pce_table_write(cdev, &ptdata);
			new_entry = 1;

			if (exclude_rule == 0)
				return GSW_statusOk;
		}
	}

	/* wildcard entry for EXCLUDE rule for  port number if already exists*/
	/*if (gswdev->gipver != LTQ_GSWIP_3_0) {*/
	if ((exclude_rule == 1) && (new_entry == 1)) {
		for (i = 0; i < gswdev->iflag.itblsize; i++) {
			/* Check if port was already exist */
			if ((hitbl->mctable[i].dlsbindex == dlix) &&
			    (hitbl->mctable[i].dmsbindex == dmix) &&
			    (hitbl->mctable[i].slsbindex == 0x7F) &&
			    (hitbl->mctable[i].smsbindex == 0x7F) &&
			    (hitbl->mctable[i].valid == 1)) {
				if (((hitbl->mctable[i].pmap >>
				      parm->nPortId) & 0x1) == 1) {
					return GSW_statusOk;
				} else {
					hpctbl->pce_sub_tbl.iplsbtcnt[dlix]++;

					if (parm->eIPVersion ==
					    GSW_IP_SELECT_IPV6)
						hpctbl->pce_sub_tbl.ipmsbtcnt[dmix]++;

					/* Add the port */
					hitbl->mctable[i].pmap |=
						(1 << parm->nPortId);
				}

				hitbl->mctable[i].mcmode =
					GSW_IGMP_MEMBER_DONT_CARE;
				memset(&ptdata, 0, sizeof(pctbl_prog_t));
				ptdata.table = PCE_MULTICAST_SW_INDEX;
				ptdata.pcindex = i;
				ptdata.key[1] =
					((hitbl->mctable[i].smsbindex << 8)
					 | (hitbl->mctable[i].slsbindex));
				ptdata.key[0] =
					((hitbl->mctable[i].dmsbindex << 8)
					 | (hitbl->mctable[i].dlsbindex));
				ptdata.val[0] = hitbl->mctable[i].pmap;
				ptdata.valid = hitbl->mctable[i].valid;

				if (gswdev->gipver == LTQ_GSWIP_3_0) {
					ptdata.val[1] =
						(parm->nSubIfId & 0xFFFF)	<< 3;
					ptdata.key[2] = parm->nFID & 0x3F;
					ptdata.key[2] |=
						(parm->bExclSrcIP & 1) << 15;
				}

				gsw_pce_table_write(cdev, &ptdata);
				return GSW_statusOk;
			}
		}
	}

	/*	}*/
	/* Create the new DstIP & SrcIP entry */
	if (new_entry == 0) {
		if ((parm->eModeMember == GSW_IGMP_MEMBER_INCLUDE)
		    || (parm->eModeMember == GSW_IGMP_MEMBER_EXCLUDE)) {
			i = 0;

			while (i < gswdev->iflag.itblsize) {
				/* Find a new empty entry to add */
				if (hitbl->mctable[i].valid == 0)
					break;

				i++;
			}
		} else if (parm->eModeMember == GSW_IGMP_MEMBER_DONT_CARE) {
			i = 63;

			while (i > 0) {
				/* Find a new empty entry to add */
				if (hitbl->mctable[i].valid == 0)
					break;

				i--;
			}
		}

		if (i >= 0 && i < gswdev->iflag.itblsize) {
			hitbl->mctable[i].dlsbindex = dlix;
			hitbl->mctable[i].dmsbindex = dmix;
			hitbl->mctable[i].pmap |= (1 << parm->nPortId);

			if (dlflag)
				hpctbl->pce_sub_tbl.iplsbtcnt[dlix] = 1;
			else
				hpctbl->pce_sub_tbl.iplsbtcnt[dlix]++;

			if (parm->eIPVersion == GSW_IP_SELECT_IPV6) {
				if (dmflag)
					hpctbl->pce_sub_tbl.ipmsbtcnt[dmix] = 1;
				else
					hpctbl->pce_sub_tbl.ipmsbtcnt[dmix]++;
			}

			hitbl->mctable[i].valid = 1;
			hitbl->mctable[i].mcmode = parm->eModeMember;

			if ((parm->eModeMember == GSW_IGMP_MEMBER_INCLUDE)
			    || (parm->eModeMember == GSW_IGMP_MEMBER_EXCLUDE)) {
				hitbl->mctable[i].slsbindex = slix;
				hitbl->mctable[i].smsbindex = smix;

				if (slflag)
					hpctbl->pce_sub_tbl.iplsbtcnt[slix] = 1;
				else
					hpctbl->pce_sub_tbl.iplsbtcnt[slix]++;

				if (parm->eIPVersion == GSW_IP_SELECT_IPV6) {
					if (smflag)
						hpctbl->pce_sub_tbl.ipmsbtcnt[smix] = 1;
					else
						hpctbl->pce_sub_tbl.ipmsbtcnt[smix]++;
				}
			} else if (parm->eModeMember ==
				   GSW_IGMP_MEMBER_DONT_CARE) {
				hitbl->mctable[i].slsbindex = 0x7F;
				hitbl->mctable[i].smsbindex = 0x7F;
			}
		}

		memset(&ptdata, 0, sizeof(pctbl_prog_t));
		/* Now, we write into Multicast SW Table */
		ptdata.table = PCE_MULTICAST_SW_INDEX;
		ptdata.pcindex = i;
		ptdata.key[1] = ((hitbl->mctable[i].smsbindex << 8)
				 | hitbl->mctable[i].slsbindex);
		ptdata.key[0] = ((hitbl->mctable[i].dmsbindex << 8)
				 | hitbl->mctable[i].dlsbindex);

		if (gswdev->gipver == LTQ_GSWIP_3_0) {
			ptdata.val[0] = hitbl->mctable[i].pmap;
		} else {
			if ((parm->eModeMember == GSW_IGMP_MEMBER_EXCLUDE))
				ptdata.val[0] = (0 << parm->nPortId);
			else
				ptdata.val[0] = hitbl->mctable[i].pmap;
		}

		ptdata.valid = hitbl->mctable[i].valid;

		if (gswdev->gipver == LTQ_GSWIP_3_0) {
			ptdata.val[1] = (parm->nSubIfId & 0xFFFF)	<< 3;
			ptdata.key[2] = parm->nFID & 0x3F;
			ptdata.key[2] |= (parm->bExclSrcIP & 1) << 15;
		}

		gsw_pce_table_write(cdev, &ptdata);

		if ((parm->eModeMember == GSW_IGMP_MEMBER_EXCLUDE)) {
			for (i = 0; i < gswdev->iflag.itblsize; i++) {
				/* Check if port was already exist */
				if ((hitbl->mctable[i].dlsbindex == dlix) &&
				    (hitbl->mctable[i].dmsbindex == dmix) &&
				    (hitbl->mctable[i].slsbindex == 0x7F) &&
				    (hitbl->mctable[i].smsbindex == 0x7F) &&
				    (hitbl->mctable[i].valid == 1)) {
					if (((hitbl->mctable[i].pmap >>
					      parm->nPortId) & 0x1) == 1)
						return GSW_statusOk;

					hpctbl->pce_sub_tbl.iplsbtcnt[dlix]++;

					if (parm->eIPVersion ==
					    GSW_IP_SELECT_IPV6)
						hpctbl->pce_sub_tbl.ipmsbtcnt[dmix]++;

					hitbl->mctable[i].mcmode =
						GSW_IGMP_MEMBER_DONT_CARE;
					/* Add the port */
					hitbl->mctable[i].pmap |=
						(1 << parm->nPortId);
					memset(&ptdata, 0, sizeof(pctbl_prog_t));
					ptdata.table = PCE_MULTICAST_SW_INDEX;
					ptdata.pcindex = i;
					ptdata.key[1] =
						((hitbl->mctable[i].smsbindex << 8)
						 | (hitbl->mctable[i].slsbindex));
					ptdata.key[0] =
						((hitbl->mctable[i].dmsbindex << 8)
						 | (hitbl->mctable[i].dlsbindex));
					ptdata.val[0] = hitbl->mctable[i].pmap;
					ptdata.valid = hitbl->mctable[i].valid;
					gsw_pce_table_write(cdev, &ptdata);
					return GSW_statusOk;
				}
			}

			i = 63;

			while (i > 0) {
				/* Find a new empty entry to add */
				if (hitbl->mctable[i].valid == 0)
					break;

				i--;
			}

			if (i >= 0 && i < gswdev->iflag.itblsize) {
				/* Now, we write into Multicast SW Table */
				hitbl->mctable[i].dlsbindex = dlix;
				hitbl->mctable[i].dmsbindex = dmix;
				hitbl->mctable[i].slsbindex = 0x7F;
				hitbl->mctable[i].smsbindex = 0x7F;
				hitbl->mctable[i].pmap |= (1 << parm->nPortId);
				hitbl->mctable[i].mcmode = GSW_IGMP_MEMBER_DONT_CARE;
				hitbl->mctable[i].valid = 1;
				hpctbl->pce_sub_tbl.iplsbtcnt[dlix]++;

				if (parm->eIPVersion == GSW_IP_SELECT_IPV6)
					hpctbl->pce_sub_tbl.ipmsbtcnt[dmix]++;

				memset(&ptdata, 0, sizeof(pctbl_prog_t));
				ptdata.table = PCE_MULTICAST_SW_INDEX;
				ptdata.pcindex = i;
				ptdata.key[1] = ((hitbl->mctable[i].smsbindex << 8)
						 | hitbl->mctable[i].slsbindex);
				ptdata.key[0] = ((hitbl->mctable[i].dmsbindex << 8)
						 | hitbl->mctable[i].dlsbindex);
				ptdata.val[0] = hitbl->mctable[i].pmap;
				ptdata.valid = hitbl->mctable[i].valid;

				if (gswdev->gipver == LTQ_GSWIP_3_0) {
					ptdata.val[1] =
						(parm->nSubIfId & 0xFFFF)	<< 3;
					ptdata.key[2] = parm->nFID & 0x3F;
					ptdata.key[2] |= (parm->bExclSrcIP & 1) << 15;
				}

				gsw_pce_table_write(cdev, &ptdata);
			} else {
				pr_err("%s:%s:%d (IGMP Table full)\n",
				       __FILE__, __func__, __LINE__);
			}
		}
	}

	/* Debug */
	return GSW_statusOk;
}

/* Multicast Software Table Include/Exclude Add function */
static int gsw3x_msw_table_wr(void *cdev, GSW_multicastTable_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	int	i, j, exclude_rule = 0;
	int	dlix = 0, dmix = 0;
	int slix = 0, smix = 0;
	int dlflag = 0, dmflag = 0, slflag = 0, smflag = 0;
	ltq_bool_t new_entry = 0;
	pctbl_prog_t ptdata;
	pce_dasa_lsb_t	ltbl;
	pce_dasa_msb_t	mtbl;
	ltq_pce_table_t *hpctbl = NULL;
	gsw_igmp_t *hitbl = NULL;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

	hpctbl = &gswdev->phandler;
	hitbl = &gswdev->iflag;


	memset(&ptdata, 0, sizeof(pctbl_prog_t));
	memset(&ltbl, 0, sizeof(pce_dasa_lsb_t));
	memset(&mtbl, 0, sizeof(pce_dasa_msb_t));

	if ((parm->eModeMember == GSW_IGMP_MEMBER_INCLUDE)
	    || (parm->eModeMember == GSW_IGMP_MEMBER_EXCLUDE)) {
		if (gswdev->iflag.igv3 != 1) {
			pr_err("%s:%s:%d(bIGMPv3 need to be enable)\n",
			       __FILE__, __func__, __LINE__);
			return GSW_statusErr;
		}
	}

	if ((parm->eIPVersion != GSW_IP_SELECT_IPV4)
	    && (parm->eIPVersion != GSW_IP_SELECT_IPV6)) {
		pr_err("%s:%s:%d (IPv4/IPV6 need to enable)\n",
		       __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

	if (parm->eIPVersion == GSW_IP_SELECT_IPV4) {
		for (i = 0; i < 4; i++)
			ltbl.ilsb[i] = ((parm->uIP_Gda.nIPv4 >> (i * 8))
					& 0xFF);

		ltbl.mask[0] = 0;
		ltbl.mask[1] = 0;
		ltbl.mask[2] = 0xFFFF;
		ltbl.mask[3] = 0xFFFF;
	}

	if (parm->eIPVersion == GSW_IP_SELECT_IPV6) {
		for (i = 0, j = 7; i < 4; i++, j -= 2) {
			mtbl.imsb[j - 1] = (parm->uIP_Gda.nIPv6[i] & 0xFF);
			mtbl.imsb[j] = ((parm->uIP_Gda.nIPv6[i] >> 8) & 0xFF);
		}

		mtbl.mask[0] = 0;
		mtbl.mask[1] = 0;
		mtbl.mask[2] = 0;
		mtbl.mask[3] = 0;
		dmix = find_msb_tbl_entry(&hpctbl->pce_sub_tbl,
					  &mtbl);

		if (dmix == 0xFF) {
			dmix = pce_dasa_msb_tbl_write(cdev,
						      &hpctbl->pce_sub_tbl, &mtbl);
			dmflag = 1;
		}

		if ((dmix < 0) || (dmix > (IP_DASA_PC_MSIZE - 1))) {
			pr_err("%s:%s:%d (IGMP Table full)\n",
			       __FILE__, __func__, __LINE__);
			return GSW_statusErr;
		}

		/* First, search for DIP in the DA/SA table (DIP LSB) */
		for (i = 0, j = 7; i < 4; i++, j -= 2) {
			ltbl.ilsb[j - 1] = (parm->uIP_Gda.nIPv6[i + 4] & 0xFF);
			ltbl.ilsb[j] = ((parm->uIP_Gda.nIPv6[i + 4] >> 8) & 0xFF);
		}

		ltbl.mask[0] = 0;
		ltbl.mask[1] = 0;
		ltbl.mask[2] = 0;
		ltbl.mask[3] = 0;
	}

	dlix = find_dasa_tbl_entry(&hpctbl->pce_sub_tbl,
				   &ltbl);

	if (dlix == 0xFF) {
		dlix = pce_dasa_lsb_tbl_write(cdev,
					      &hpctbl->pce_sub_tbl, &ltbl);
		dlflag = 1;
	}



	if ((dlix < 0) || (dlix > (IP_DASA_PC_LSIZE - 1))) {
		pr_err("%s:%s:%d (IGMP Table full)\n",
		       __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

	if ((parm->eModeMember == GSW_IGMP_MEMBER_INCLUDE) ||
	    (parm->eModeMember == GSW_IGMP_MEMBER_EXCLUDE)) {
		if (parm->eIPVersion == GSW_IP_SELECT_IPV4) {
			for (i = 0; i < 4; i++)
				ltbl.ilsb[i] = ((parm->uIP_Gsa.nIPv4 >> (i * 8))
						& 0xFF);

			ltbl.mask[0] = 0x0;
			ltbl.mask[1] = 0x0;
			ltbl.mask[2] = 0xFFFF;
			ltbl.mask[3] = 0xFFFF;

			if (parm->eModeMember == GSW_IGMP_MEMBER_EXCLUDE) {
				if ((ltbl.ilsb[3] == 0) &&
				    (ltbl.ilsb[2] == 0) &&
				    (ltbl.ilsb[1] == 0) &&
				    (ltbl.ilsb[0] == 0)) {
					pr_err("%s:%s:%d (Exclude Rule Source IP is Wildcard)\n",
					       __FILE__, __func__, __LINE__);
					return GSW_statusErr;
				}
			}
		}

		if (parm->eIPVersion == GSW_IP_SELECT_IPV6) {
			int src_zero = 0;

			/* First, search for DIP in the DA/SA table (DIP MSB) */
			for (i = 0, j = 7; i < 4; i++, j -= 2) {
				mtbl.imsb[j - 1] = (parm->uIP_Gsa.nIPv6[i]
						    & 0xFF);
				mtbl.imsb[j] = ((parm->uIP_Gsa.nIPv6[i] >> 8)
						& 0xFF);
			}

			mtbl.mask[0] = 0;
			mtbl.mask[1] = 0;
			mtbl.mask[2] = 0;
			mtbl.mask[3] = 0;

			if (parm->eModeMember ==
			    GSW_IGMP_MEMBER_EXCLUDE) {
				if (mtbl.imsb[0] == 0 &&
				    mtbl.imsb[1] == 0 &&
				    mtbl.imsb[2] == 0 &&
				    mtbl.imsb[3] == 0 &&
				    mtbl.imsb[4] == 0 &&
				    mtbl.imsb[5] == 0 &&
				    mtbl.imsb[6] == 0 &&
				    mtbl.imsb[7] == 0) {
					src_zero = 1;
				}
			}

			/* First, search for DIP in the DA/SA table (DIP LSB) */
			for (i = 0, j = 7; i < 4; i++, j -= 2) {
				ltbl.ilsb[j - 1] = (parm->uIP_Gsa.nIPv6[i + 4]
						    & 0xFF);
				ltbl.ilsb[j] = ((parm->uIP_Gsa.nIPv6[i + 4] >> 8)
						& 0xFF);
			}

			ltbl.mask[0] = 0;
			ltbl.mask[1] = 0;
			ltbl.mask[2] = 0;
			ltbl.mask[3] = 0;

			if (parm->eModeMember ==
			    GSW_IGMP_MEMBER_EXCLUDE) {
				if ((ltbl.ilsb[0] == 0) &&
				    (ltbl.ilsb[1] == 0) &&
				    (ltbl.ilsb[2] == 0) &&
				    (ltbl.ilsb[3] == 0) &&
				    (ltbl.ilsb[4] == 0) &&
				    (ltbl.ilsb[5] == 0) &&
				    (ltbl.ilsb[6] == 0) &&
				    (ltbl.ilsb[7] == 0)) {
					if (src_zero) {
						pr_err("%s:%s:%d (Exclude rule SIP is Wildcard)\n",
						       __FILE__, __func__, __LINE__);
						return GSW_statusErr;
					}
				}
			}

			smix = find_msb_tbl_entry(&hpctbl->pce_sub_tbl,
						  &mtbl);

			if (smix == 0xFF) {
				smix = pce_dasa_msb_tbl_write(cdev,
							      &hpctbl->pce_sub_tbl, &mtbl);
				smflag = 1;
			}

			if ((smix < 0) || (smix > (IP_DASA_PC_MSIZE - 1))) {
				pr_err("%s:%s:%d (IGMP Table full)\n",
				       __FILE__, __func__, __LINE__);
				return GSW_statusErr;
			}
		}

		slix = find_dasa_tbl_entry(&hpctbl->pce_sub_tbl,
					   &ltbl);

		if (slix == 0xFF) {
			slix = pce_dasa_lsb_tbl_write(cdev,
						      &hpctbl->pce_sub_tbl, &ltbl);
			slflag = 1;
		}

		if ((slix < 0) || (slix > (IP_DASA_PC_LSIZE - 1))) {
			pr_err("%s:%s:%d (IGMP Table full)\n",
			       __FILE__, __func__, __LINE__);
			return GSW_statusErr;
		}
	}

	/* update the entry for another port number if already exists*/
	for (i = 0; i < gswdev->iflag.itblsize; i++) {
		/* Check if port was already exist */
		if ((hitbl->mctable[i].dlsbindex == dlix) &&
		    (hitbl->mctable[i].dmsbindex == dmix) &&
		    (hitbl->mctable[i].slsbindex == slix) &&
		    (hitbl->mctable[i].smsbindex == smix) &&
		    (hitbl->mctable[i].valid == 1) &&
		    (hitbl->mctable[i].fid == parm->nFID)) {
			if (((hitbl->mctable[i].pmap >> parm->nPortId)
			     & 0x1) == 1)
				return GSW_statusOk;

			exclude_rule = 0;

			switch (hitbl->mctable[i].mcmode) {
			case GSW_IGMP_MEMBER_DONT_CARE:
				hpctbl->pce_sub_tbl.iplsbtcnt[dlix]++;

				if (parm->eIPVersion == GSW_IP_SELECT_IPV6)
					hpctbl->pce_sub_tbl.ipmsbtcnt[dmix]++;

				/* Add the port */
				hitbl->mctable[i].pmap |= (1 << parm->nPortId);
				break;

			case GSW_IGMP_MEMBER_EXCLUDE:
				exclude_rule = 1;

			/*		hitbl->mctable[i].exclude = 1; */
			/*		ptdata.key[2] |= (parm->bExclSrcIP & 1) << 15; */
			case GSW_IGMP_MEMBER_INCLUDE:
				/* Add the port */
				hitbl->mctable[i].pmap |= (1 << parm->nPortId);
				hpctbl->pce_sub_tbl.iplsbtcnt[dlix]++;
				hpctbl->pce_sub_tbl.iplsbtcnt[slix]++;

				if (parm->eIPVersion == GSW_IP_SELECT_IPV6) {
					hpctbl->pce_sub_tbl.ipmsbtcnt[dmix]++;
					hpctbl->pce_sub_tbl.ipmsbtcnt[smix]++;
				}

				break;
			} /* end switch */

			/* Now, we write into Multicast SW Table */
			memset(&ptdata, 0, sizeof(pctbl_prog_t));
			ptdata.table = PCE_MULTICAST_SW_INDEX;
			ptdata.pcindex = i;
			ptdata.key[1] = ((hitbl->mctable[i].smsbindex << 8)
					 | (hitbl->mctable[i].slsbindex));
			ptdata.key[0] = ((hitbl->mctable[i].dmsbindex << 8)
					 | (hitbl->mctable[i].dlsbindex));
			ptdata.key[2] = ((hitbl->mctable[i].fid) & 0x3F);
			ptdata.key[2] |= ((hitbl->mctable[i].exclude) << 15);
			ptdata.val[0] = hitbl->mctable[i].pmap;
			ptdata.val[1] = ((hitbl->mctable[i].subifid)	<< 3);
			ptdata.valid = hitbl->mctable[i].valid;
			gsw_pce_table_write(cdev, &ptdata);
			new_entry = 1;
		}
	}

	/* Create the new DstIP & SrcIP entry */
	if (new_entry == 0) {
		if ((parm->eModeMember == GSW_IGMP_MEMBER_INCLUDE) ||
		    (parm->eModeMember == GSW_IGMP_MEMBER_EXCLUDE)) {
			i = 0;

			while (i < gswdev->iflag.itblsize) {
				/* Find a new empty entry to add */
				if (hitbl->mctable[i].valid == 0)
					break;

				i++;
			}
		} else if (parm->eModeMember == GSW_IGMP_MEMBER_DONT_CARE) {
			i = 63;

			while (i > 0) {
				/* Find a new empty entry to add */
				if (hitbl->mctable[i].valid == 0)
					break;

				i--;
			}
		}

		if (i >= 0 && i < gswdev->iflag.itblsize) {
			hitbl->mctable[i].dlsbindex = dlix;
			hitbl->mctable[i].dmsbindex = dmix;
			hitbl->mctable[i].pmap |= (1 << parm->nPortId);

			if (dlflag)
				hpctbl->pce_sub_tbl.iplsbtcnt[dlix] = 1;
			else
				hpctbl->pce_sub_tbl.iplsbtcnt[dlix]++;

			if (parm->eIPVersion == GSW_IP_SELECT_IPV6) {
				if (dmflag)
					hpctbl->pce_sub_tbl.ipmsbtcnt[dmix] = 1;
				else
					hpctbl->pce_sub_tbl.ipmsbtcnt[dmix]++;
			}

			hitbl->mctable[i].valid = 1;
			hitbl->mctable[i].mcmode = parm->eModeMember;

			if ((parm->eModeMember == GSW_IGMP_MEMBER_INCLUDE)
			    || (parm->eModeMember ==
				GSW_IGMP_MEMBER_EXCLUDE)) {
				hitbl->mctable[i].slsbindex = slix;
				hitbl->mctable[i].smsbindex = smix;

				if (slflag)
					hpctbl->pce_sub_tbl.iplsbtcnt[slix] = 1;
				else
					hpctbl->pce_sub_tbl.iplsbtcnt[slix]++;

				if (parm->eIPVersion == GSW_IP_SELECT_IPV6) {
					if (smflag)
						hpctbl->pce_sub_tbl.ipmsbtcnt[smix] = 1;
					else
						hpctbl->pce_sub_tbl.ipmsbtcnt[smix]++;
				}
			} else if (parm->eModeMember ==
				   GSW_IGMP_MEMBER_DONT_CARE) {
				hitbl->mctable[i].slsbindex = 0x7F;
				hitbl->mctable[i].smsbindex = 0x7F;
			}
		}

		hitbl->mctable[i].fid = (parm->nFID & 0x3F);
		hitbl->mctable[i].subifid = (parm->nSubIfId & 0xFFFF);
		memset(&ptdata, 0, sizeof(pctbl_prog_t));
		/* Now, we write into Multicast SW Table */
		ptdata.table = PCE_MULTICAST_SW_INDEX;
		ptdata.pcindex = i;
		ptdata.key[1] = ((hitbl->mctable[i].smsbindex << 8)
				 | (hitbl->mctable[i].slsbindex));
		ptdata.key[0] = ((hitbl->mctable[i].dmsbindex << 8)
				 | (hitbl->mctable[i].dlsbindex));

		ptdata.val[0] = hitbl->mctable[i].pmap;
		ptdata.valid = hitbl->mctable[i].valid;
		ptdata.val[1] = (parm->nSubIfId & 0xFFFF)	<< 3;
		ptdata.key[2] |= parm->nFID & 0x3F;

		if (parm->eModeMember == GSW_IGMP_MEMBER_EXCLUDE) {
			/* ptdata.key[2] |= (parm->bExclSrcIP & 1) << 15; */
			ptdata.key[2] |= (1 << 15);
			hitbl->mctable[i].exclude = 1;
		}

		gsw_pce_table_write(cdev, &ptdata);
	}

	return GSW_statusOk;
}

/* Multicast Software Table Include/Exclude Remove function */
static int gsw3x_msw_table_rm(void *cdev, GSW_multicastTable_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u8	i, j;
	ltq_bool_t MATCH = 0;
	ltq_pce_table_t *hpctbl = NULL;
	gsw_igmp_t *hitbl = NULL;
	pctbl_prog_t ptdata;
	pce_dasa_lsb_t ltbl;
	pce_dasa_msb_t mtbl;
	int dlix = 0, dmix = 0, slix = 0, smix = 0;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

	hpctbl = &gswdev->phandler;
	hitbl = &gswdev->iflag;

	memset(&ptdata, 0, sizeof(pctbl_prog_t));
	memset(&ltbl, 0, sizeof(pce_dasa_lsb_t));
	memset(&mtbl, 0, sizeof(pce_dasa_msb_t));

	if ((parm->eIPVersion != GSW_IP_SELECT_IPV4)
	    && (parm->eIPVersion != GSW_IP_SELECT_IPV6)) {
		pr_err("%s:%s:%d (IPv4/IPV6 need to enable!!!)\n",
		       __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

	if ((parm->eModeMember == GSW_IGMP_MEMBER_INCLUDE) &&
	    (parm->eModeMember == GSW_IGMP_MEMBER_EXCLUDE) &&
	    (parm->eModeMember == GSW_IGMP_MEMBER_DONT_CARE)) {
		pr_err("%s:%s:%d (!!!)\n", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

	if (parm->eIPVersion == GSW_IP_SELECT_IPV4) {
		/* First, search for DIP in the DA/SA table (DIP LSB) */
		for (i = 0; i < 4; i++)
			ltbl.ilsb[i] = ((parm->uIP_Gda.nIPv4 >> (i * 8))
					& 0xFF);

		ltbl.mask[0] = 0x0;
		ltbl.mask[1] = 0x0;
		ltbl.mask[2] = 0xFFFF;
		ltbl.mask[3] = 0xFFFF;
	}

	if (parm->eIPVersion == GSW_IP_SELECT_IPV6 /* IPv6 */) {
		/* First, search for DIP in the DA/SA table (DIP MSB) */
		for (i = 0, j = 7; i < 4; i++, j -= 2) {
			mtbl.imsb[j - 1] = (parm->uIP_Gda.nIPv6[i] & 0xFF);
			mtbl.imsb[j] = ((parm->uIP_Gda.nIPv6[i] >> 8) & 0xFF);
		}

		mtbl.mask[0] = 0;
		mtbl.mask[1] = 0;
		mtbl.mask[2] = 0;
		mtbl.mask[3] = 0;
		dmix = find_msb_tbl_entry(&hpctbl->pce_sub_tbl,
					  &mtbl);

		if (dmix == 0xFF) {
			pr_err("%s:%s:%d (IGMP Entry not found)\n",
			       __FILE__, __func__, __LINE__);
			return GSW_statusErr;
		}

		if ((dmix < 0) || (dmix > (IP_DASA_PC_MSIZE - 1))) {
			pr_err("%s:%s:%d (IGMP Table full)\n",
			       __FILE__, __func__, __LINE__);
			return GSW_statusErr;
		}

		/* First, search for DIP in the DA/SA table (DIP LSB) */
		for (i = 0, j = 7; i < 4; i++, j -= 2) {
			ltbl.ilsb[j - 1] = (parm->uIP_Gda.nIPv6[i + 4] & 0xFF);
			ltbl.ilsb[j] = ((parm->uIP_Gda.nIPv6[i + 4] >> 8) & 0xFF);
		}

		ltbl.mask[0] = 0;
		ltbl.mask[1] = 0;
		ltbl.mask[2] = 0;
		ltbl.mask[3] = 0;
	}

	dlix = find_dasa_tbl_entry(&hpctbl->pce_sub_tbl,
				   &ltbl);

	if (dlix == 0xFF) {
		pr_err("%s:%s:%d (IGMP Entry not found)\n",
		       __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

	if ((dlix < 0) || (dlix > (IP_DASA_PC_LSIZE - 1))) {
		pr_err("%s:%s:%d (IGMP Table full)\n",
		       __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

	if ((parm->eModeMember == GSW_IGMP_MEMBER_INCLUDE)
	    || (parm->eModeMember == GSW_IGMP_MEMBER_EXCLUDE)) {
		if (parm->eIPVersion == GSW_IP_SELECT_IPV4) {
			/* First, search for DIP in the DA/SA table (DIP LSB) */
			for (i = 0; i < 4; i++)
				ltbl.ilsb[i] = ((parm->uIP_Gsa.nIPv4 >> (i * 8))
						& 0xFF);

			ltbl.mask[0] = 0x0;
			ltbl.mask[1] = 0x0;
			ltbl.mask[2] = 0xFFFF;
			ltbl.mask[3] = 0xFFFF;

			if (parm->eModeMember == GSW_IGMP_MEMBER_EXCLUDE) {
				if (ltbl.ilsb[3] == 0 &&
				    (ltbl.ilsb[2] == 0) &&
				    (ltbl.ilsb[1] == 0) &&
				    (ltbl.ilsb[0] == 0)) {
					pr_err("%s:%s:%d (Exclude SIP is Wildcard)\n",
					       __FILE__, __func__, __LINE__);
					return GSW_statusErr;
				}
			}
		}

		if (parm->eIPVersion == GSW_IP_SELECT_IPV6) {
			int src_zero = 0;

			/* First, search for DIP in the DA/SA table (DIP MSB) */
			for (i = 0, j = 7; i < 4; i++, j -= 2) {
				mtbl.imsb[j - 1] = (parm->uIP_Gsa.nIPv6[i]
						    & 0xFF);
				mtbl.imsb[j] = ((parm->uIP_Gsa.nIPv6[i] >> 8)
						& 0xFF);
			}

			mtbl.mask[0] = 0;
			mtbl.mask[1] = 0;
			mtbl.mask[2] = 0;
			mtbl.mask[3] = 0;

			if (parm->eModeMember == GSW_IGMP_MEMBER_EXCLUDE) {
				if ((mtbl.imsb[0] == 0) &&
				    (mtbl.imsb[1] == 0) &&
				    (mtbl.imsb[2] == 0) &&
				    (mtbl.imsb[3] == 0) &&
				    (mtbl.imsb[4] == 0) &&
				    (mtbl.imsb[5] == 0) &&
				    (mtbl.imsb[6] == 0) &&
				    (mtbl.imsb[7] == 0))
					src_zero = 1;
			}

			smix = find_msb_tbl_entry(&hpctbl->pce_sub_tbl,
						  &mtbl);

			if (smix == 0xFF) {
				pr_err("%s:%s:%d (IGMP Entry not found)\n",
				       __FILE__, __func__, __LINE__);
				return GSW_statusErr;
			}

			if ((smix < 0) || (smix > (IP_DASA_PC_MSIZE - 1))) {
				pr_err("%s:%s:%d (IGMP Table full)\n",
				       __FILE__, __func__, __LINE__);
				return GSW_statusErr;
			}

			/* First, search for DIP in the DA/SA table (DIP LSB) */
			for (i = 0, j = 7; i < 4; i++, j -= 2) {
				ltbl.ilsb[j - 1] = (parm->uIP_Gsa.nIPv6[i + 4]
						    & 0xFF);
				ltbl.ilsb[j] = ((parm->uIP_Gsa.nIPv6[i + 4] >> 8)
						& 0xFF);
			}

			ltbl.mask[0] = 0;
			ltbl.mask[1] = 0;
			ltbl.mask[2] = 0;
			ltbl.mask[3] = 0;

			if (parm->eModeMember == GSW_IGMP_MEMBER_EXCLUDE) {
				if ((ltbl.ilsb[0] == 0) &&
				    (ltbl.ilsb[1] == 0) &&
				    (ltbl.ilsb[2] == 0) &&
				    (ltbl.ilsb[3] == 0) &&
				    (ltbl.ilsb[4] == 0) &&
				    (ltbl.ilsb[5] == 0) &&
				    (ltbl.ilsb[6] == 0) &&
				    (ltbl.ilsb[7] == 0)) {
					if (src_zero) {
						pr_err("%s:%s:%d (Exclude SIP is Wildcard)\n",
						       __FILE__, __func__, __LINE__);
						return GSW_statusErr;
					}
				}
			}
		}

		slix = find_dasa_tbl_entry(&hpctbl->pce_sub_tbl,
					   &ltbl);

		if (slix == 0xFF) {
			pr_err("%s:%s:%d (IGMP Entry not found)\n",
			       __FILE__, __func__, __LINE__);
			return GSW_statusErr;
		}

		if ((slix < 0) || (slix > (IP_DASA_PC_LSIZE - 1))) {
			pr_err("%s:%s:%d (IGMP Table full)\n",
			       __FILE__, __func__, __LINE__);
			return GSW_statusErr;
		}

	}

	for (i = 0; i < gswdev->iflag.itblsize; i++) {
		if ((hitbl->mctable[i].dlsbindex == dlix) &&
		    (hitbl->mctable[i].slsbindex == slix) &&
		    (hitbl->mctable[i].dmsbindex == dmix) &&
		    (hitbl->mctable[i].smsbindex == smix) &&
		    (hitbl->mctable[i].valid == 1) &&
		    (hitbl->mctable[i].fid == parm->nFID)) {
			switch (hitbl->mctable[i].mcmode) {
			case GSW_IGMP_MEMBER_DONT_CARE:
				if (((hitbl->mctable[i].pmap >> parm->nPortId) & 0x1) == 1) {
					hitbl->mctable[i].pmap &= ~(1 << parm->nPortId);

					if (hpctbl->pce_sub_tbl.iplsbtcnt[dlix] > 0) {
						ipdslsb_tblidx_del(&hpctbl->pce_sub_tbl, dlix);

						if (hpctbl->pce_sub_tbl.iplsbtcnt[dlix] == 0)
							/* Delet the sub table */
							ip_dasa_lsb_tbl_del(cdev,
									    &hpctbl->pce_sub_tbl, dlix);
					}

					/* Delet the sub table */
					if (hpctbl->pce_sub_tbl.ipmsbtcnt[dmix] > 0) {
						ipdsmsb_tblidx_del(&hpctbl->pce_sub_tbl, dmix);

						if (hpctbl->pce_sub_tbl.ipmsbtcnt[dmix] == 0) {
							if (parm->eIPVersion == GSW_IP_SELECT_IPV6)
								ip_dasa_msb_tbl_del(cdev,
										    &hpctbl->pce_sub_tbl, dmix);
						}
					}

					/* Check the port map status */
					/* Delet the entry from Multicast sw Table */
					if (hitbl->mctable[i].pmap == 0)
						hitbl->mctable[i].valid = 0;

					MATCH = 1;
				}

				break;

			case GSW_IGMP_MEMBER_INCLUDE:
			case GSW_IGMP_MEMBER_EXCLUDE:
				if (((hitbl->mctable[i].pmap >> parm->nPortId) & 0x1) == 1) {
					hitbl->mctable[i].pmap &= ~(1 << parm->nPortId);

					if (hpctbl->pce_sub_tbl.iplsbtcnt[dlix] > 0) {
						ipdslsb_tblidx_del(&hpctbl->pce_sub_tbl, dlix);

						/* Delet the sub table */
						if (hpctbl->pce_sub_tbl.iplsbtcnt[dlix] == 0) {
							ip_dasa_lsb_tbl_del(cdev,
									    &hpctbl->pce_sub_tbl, dlix);
						}
					}

					if (hpctbl->pce_sub_tbl.ipmsbtcnt[dmix] > 0) {
						ipdsmsb_tblidx_del(&hpctbl->pce_sub_tbl, dmix);

						/* Delet the sub table */
						if (hpctbl->pce_sub_tbl.ipmsbtcnt[dmix] == 0) {
							ip_dasa_msb_tbl_del(cdev,
									    &hpctbl->pce_sub_tbl, dmix);
						}
					}

					if (hpctbl->pce_sub_tbl.iplsbtcnt[slix] > 0) {
						ipdslsb_tblidx_del(&hpctbl->pce_sub_tbl, slix);

						/* Delet the sub table */
						if (hpctbl->pce_sub_tbl.iplsbtcnt[slix] == 0) {
							ip_dasa_lsb_tbl_del(cdev,
									    &hpctbl->pce_sub_tbl, slix);
						}
					}

					if (hpctbl->pce_sub_tbl.ipmsbtcnt[smix] > 0) {
						ipdsmsb_tblidx_del(&hpctbl->pce_sub_tbl, smix);

						/* Delet the sub table */
						if (hpctbl->pce_sub_tbl.ipmsbtcnt[smix] == 0) {
							ip_dasa_msb_tbl_del(cdev,
									    &hpctbl->pce_sub_tbl, smix);
						}
					}

					/* Check the port map status */
					if (hitbl->mctable[i].pmap == 0) {
						/* Delet the entry from Multicast sw Table */
						hitbl->mctable[i].valid = 0;
					}

					MATCH = 1;
				}

				break;
			}

			if (MATCH == 1) {
				memset(&ptdata, 0, sizeof(pctbl_prog_t));
				ptdata.table = PCE_MULTICAST_SW_INDEX;
				ptdata.pcindex = i;
				ptdata.key[1] = ((hitbl->mctable[i].smsbindex << 8)
						 | (hitbl->mctable[i].slsbindex));
				ptdata.key[0] = ((hitbl->mctable[i].dmsbindex << 8)
						 | (hitbl->mctable[i].dlsbindex));
				ptdata.key[2] = (hitbl->mctable[i].fid) & 0x3F;
				ptdata.key[2] |= (hitbl->mctable[i].exclude) << 15;
				ptdata.val[0] = hitbl->mctable[i].pmap;
				ptdata.val[1] = (hitbl->mctable[i].subifid)	<< 3;
				ptdata.valid = hitbl->mctable[i].valid;
				gsw_pce_table_write(cdev, &ptdata);
			}
		}
	}

	if (MATCH == 0)
		pr_info("The GIP/SIP not found\n");

	return GSW_statusOk;
}

/* Multicast Software Table Include/Exclude Remove function */
static int gsw2x_msw_table_rm(void *cdev, GSW_multicastTable_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u8	i, j;
	ltq_bool_t MATCH = 0;
	ltq_pce_table_t *hpctbl = NULL;
	gsw_igmp_t *hitbl = NULL;
	pctbl_prog_t ptdata;
	pce_dasa_lsb_t ltbl;
	pce_dasa_msb_t mtbl;
	int dlix = 0, dmix = 0, slix = 0, smix = 0;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

	hitbl = &gswdev->iflag;
	hpctbl = &gswdev->phandler;


	memset(&ptdata, 0, sizeof(pctbl_prog_t));
	memset(&ltbl, 0, sizeof(pce_dasa_lsb_t));
	memset(&mtbl, 0, sizeof(pce_dasa_msb_t));

	if ((parm->eIPVersion != GSW_IP_SELECT_IPV4)
	    && (parm->eIPVersion != GSW_IP_SELECT_IPV6)) {
		pr_err("%s:%s:%d (IPv4/IPV6 need to enable!!!)\n",
		       __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

	if ((parm->eModeMember == GSW_IGMP_MEMBER_INCLUDE)
	    && (parm->eModeMember == GSW_IGMP_MEMBER_EXCLUDE)
	    && (parm->eModeMember == GSW_IGMP_MEMBER_DONT_CARE)) {
		pr_err("%s:%s:%d (!!!)\n", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

	if (parm->eIPVersion == GSW_IP_SELECT_IPV4) {
		/* First, search for DIP in the DA/SA table (DIP LSB) */
		for (i = 0; i < 4; i++)
			ltbl.ilsb[i] = ((parm->uIP_Gda.nIPv4 >> (i * 8)) & 0xFF);

		if (gswdev->gipver == LTQ_GSWIP_3_0) {
			ltbl.mask[0] = 0x0;
			ltbl.mask[1] = 0x0;
			ltbl.mask[2] = 0xFFFF;
			ltbl.mask[3] = 0xFFFF;
		} else {
			/* DIP LSB Nibble Mask */
			ltbl.mask[0] = 0xFF00;
		}
	}

	if (parm->eIPVersion == GSW_IP_SELECT_IPV6 /* IPv6 */) {
		/* First, search for DIP in the DA/SA table (DIP MSB) */
		for (i = 0, j = 7; i < 4; i++, j -= 2) {
			mtbl.imsb[j - 1] = (parm->uIP_Gda.nIPv6[i] & 0xFF);
			mtbl.imsb[j] = ((parm->uIP_Gda.nIPv6[i] >> 8) & 0xFF);
		}

		if (gswdev->gipver == LTQ_GSWIP_3_0) {
			mtbl.mask[0] = 0;
			mtbl.mask[1] = 0;
			mtbl.mask[2] = 0;
			mtbl.mask[3] = 0;
		} else {
			mtbl.mask[0] = 0;/* DIP MSB Nibble Mask */
		}

		dmix = find_msb_tbl_entry(&hpctbl->pce_sub_tbl, &mtbl);

		if (dmix == 0xFF) {
			pr_err("%s:%s:%d (IGMP Entry not found)\n",
			       __FILE__, __func__, __LINE__);
			return GSW_statusErr;
		}

		if ((dmix < 0) || (dmix > (IP_DASA_PC_MSIZE - 1))) {
			pr_err("%s:%s:%d (IGMP Table full)\n",
			       __FILE__, __func__, __LINE__);
			return GSW_statusErr;
		}

		/* First, search for DIP in the DA/SA table (DIP LSB) */
		for (i = 0, j = 7; i < 4; i++, j -= 2) {
			ltbl.ilsb[j - 1] = (parm->uIP_Gda.nIPv6[i + 4] & 0xFF);
			ltbl.ilsb[j] = ((parm->uIP_Gda.nIPv6[i + 4] >> 8) & 0xFF);
		}

		if (gswdev->gipver == LTQ_GSWIP_3_0) {
			ltbl.mask[0] = 0;
			ltbl.mask[1] = 0;
			ltbl.mask[2] = 0;
			ltbl.mask[3] = 0;
		} else {
			ltbl.mask[0] = 0;/* DIP LSB Nibble Mask */
		}
	}

	dlix = find_dasa_tbl_entry(&hpctbl->pce_sub_tbl, &ltbl);

	if (dlix == 0xFF) {
		pr_err("%s:%s:%d (IGMP Entry not found)\n",
		       __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

	if ((dlix < 0) || (dlix > (IP_DASA_PC_LSIZE - 1))) {
		pr_err("%s:%s:%d (IGMP Table full)\n",
		       __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

	if ((parm->eModeMember == GSW_IGMP_MEMBER_INCLUDE)
	    || (parm->eModeMember == GSW_IGMP_MEMBER_EXCLUDE)) {
		if (parm->eIPVersion == GSW_IP_SELECT_IPV4) {
			/* First, search for DIP in the DA/SA table (DIP LSB) */
			for (i = 0; i < 4; i++)
				ltbl.ilsb[i] = ((parm->uIP_Gsa.nIPv4 >> (i * 8)) & 0xFF);

			if (gswdev->gipver == LTQ_GSWIP_3_0) {
				ltbl.mask[0] = 0x0;
				ltbl.mask[1] = 0x0;
				ltbl.mask[2] = 0xFFFF;
				ltbl.mask[3] = 0xFFFF;
			} else {
				/* DIP LSB Nibble Mask */
				ltbl.mask[0] = 0xFF00;
			}

			if (parm->eModeMember == GSW_IGMP_MEMBER_EXCLUDE) {
				if (ltbl.ilsb[3] == 0 && ltbl.ilsb[2] == 0
				    && ltbl.ilsb[1] == 0 && ltbl.ilsb[0] == 0) {
					pr_err("%s:%s:%d (Exclude Rule Source IP is Wildcard)\n",
					       __FILE__, __func__, __LINE__);
					return GSW_statusErr;
				}
			}
		}

		if (parm->eIPVersion == GSW_IP_SELECT_IPV6) {
			int src_zero = 0;

			/* First, search for DIP in the DA/SA table (DIP MSB) */
			for (i = 0, j = 7; i < 4; i++, j -= 2) {
				mtbl.imsb[j - 1] = (parm->uIP_Gsa.nIPv6[i] & 0xFF);
				mtbl.imsb[j] = ((parm->uIP_Gsa.nIPv6[i] >> 8) & 0xFF);
			}

			if (gswdev->gipver == LTQ_GSWIP_3_0) {
				mtbl.mask[0] = 0;
				mtbl.mask[1] = 0;
				mtbl.mask[2] = 0;
				mtbl.mask[3] = 0;
			} else {
				mtbl.mask[0] = 0;/* DIP MSB Nibble Mask */
			}

			if (parm->eModeMember == GSW_IGMP_MEMBER_EXCLUDE) {
				if ((mtbl.imsb[0] == 0) &&
				    (mtbl.imsb[1] == 0) &&
				    (mtbl.imsb[2] == 0) &&
				    (mtbl.imsb[3] == 0) &&
				    (mtbl.imsb[4] == 0) &&
				    (mtbl.imsb[5] == 0) &&
				    (mtbl.imsb[6] == 0) &&
				    (mtbl.imsb[7] == 0))
					src_zero = 1;
			}

			smix = find_msb_tbl_entry(&hpctbl->pce_sub_tbl,
						  &mtbl);

			if (smix == 0xFF) {
				pr_err("%s:%s:%d (IGMP Entry not found)\n",
				       __FILE__, __func__, __LINE__);
				return GSW_statusErr;
			}

			if ((smix < 0) || (smix > (IP_DASA_PC_MSIZE - 1))) {
				pr_err("%s:%s:%d (IGMP Table full)\n",
				       __FILE__, __func__, __LINE__);
				return GSW_statusErr;
			}

			/* First, search for DIP in the DA/SA table (DIP LSB) */
			for (i = 0, j = 7; i < 4; i++, j -= 2) {
				ltbl.ilsb[j - 1] = (parm->uIP_Gsa.nIPv6[i + 4] & 0xFF);
				ltbl.ilsb[j] = ((parm->uIP_Gsa.nIPv6[i + 4] >> 8) & 0xFF);
			}

			if (gswdev->gipver == LTQ_GSWIP_3_0) {
				ltbl.mask[0] = 0;
				ltbl.mask[1] = 0;
				ltbl.mask[2] = 0;
				ltbl.mask[3] = 0;
			} else {
				ltbl.mask[0] = 0;/* DIP LSB Nibble Mask */
			}

			if (parm->eModeMember == GSW_IGMP_MEMBER_EXCLUDE) {
				if ((ltbl.ilsb[0] == 0) &&
				    (ltbl.ilsb[1] == 0) &&
				    (ltbl.ilsb[2] == 0) &&
				    (ltbl.ilsb[3] == 0) &&
				    (ltbl.ilsb[4] == 0) &&
				    (ltbl.ilsb[5] == 0) &&
				    (ltbl.ilsb[6] == 0) &&
				    (ltbl.ilsb[7] == 0)) {
					if (src_zero) {
						pr_err("%s:%s:%d (Exclude Rule Source IP is Wildcard)\n",
						       __FILE__, __func__, __LINE__);
						return GSW_statusErr;
					}
				}
			}
		}

		slix = find_dasa_tbl_entry(&hpctbl->pce_sub_tbl,
					   &ltbl);

		if (slix == 0xFF) {
			pr_err("%s:%s:%d (IGMP Entry not found)\n",
			       __FILE__, __func__, __LINE__);
			return GSW_statusErr;
		}

		if ((slix < 0) || (slix > (IP_DASA_PC_LSIZE - 1))) {
			pr_err("%s:%s:%d (IGMP Table full)\n",
			       __FILE__, __func__, __LINE__);
			return GSW_statusErr;
		}

	}

	for (i = 0; i < gswdev->iflag.itblsize; i++) {
		if ((hitbl->mctable[i].dlsbindex == dlix) &&
		    (hitbl->mctable[i].slsbindex == slix) &&
		    (hitbl->mctable[i].dmsbindex == dmix) &&
		    (hitbl->mctable[i].smsbindex == smix) &&
		    (hitbl->mctable[i].valid == 1)) {

			switch (hitbl->mctable[i].mcmode) {
			case GSW_IGMP_MEMBER_DONT_CARE:
				if (((hitbl->mctable[i].pmap >> parm->nPortId) & 0x1) == 1) {
					hitbl->mctable[i].pmap &= ~(1 << parm->nPortId);

					if (hpctbl->pce_sub_tbl.iplsbtcnt[dlix] > 0) {
						ipdslsb_tblidx_del(&hpctbl->pce_sub_tbl, dlix);

						if (hpctbl->pce_sub_tbl.iplsbtcnt[dlix] == 0) {
							/* Delet the sub table */
							ip_dasa_lsb_tbl_del(cdev,
									    &hpctbl->pce_sub_tbl, dlix);
						}
					}

					/* Delet the sub table */
					if (hpctbl->pce_sub_tbl.ipmsbtcnt[dmix] > 0) {
						ipdsmsb_tblidx_del(&hpctbl->pce_sub_tbl, dmix);

						if (hpctbl->pce_sub_tbl.ipmsbtcnt[dmix] == 0) {
							if (parm->eIPVersion == GSW_IP_SELECT_IPV6)
								ip_dasa_msb_tbl_del(cdev,
										    &hpctbl->pce_sub_tbl, dmix);
						}
					}

					/* Check the port map status */
					/* Delet the entry from Multicast sw Table */
					if (hitbl->mctable[i].pmap == 0)
						hitbl->mctable[i].valid = 0;

					MATCH = 1;
				}

				break;

			case GSW_IGMP_MEMBER_INCLUDE:
			case GSW_IGMP_MEMBER_EXCLUDE:
				if (((hitbl->mctable[i].pmap >> parm->nPortId) & 0x1) == 1) {
					hitbl->mctable[i].pmap &= ~(1 << parm->nPortId);

					if (hpctbl->pce_sub_tbl.iplsbtcnt[dlix] > 0) {
						ipdslsb_tblidx_del(&hpctbl->pce_sub_tbl, dlix);

						/* Delet the sub table */
						if (hpctbl->pce_sub_tbl.iplsbtcnt[dlix] == 0) {
							ip_dasa_lsb_tbl_del(cdev,
									    &hpctbl->pce_sub_tbl, dlix);
						}
					}

					if (hpctbl->pce_sub_tbl.ipmsbtcnt[dmix] > 0) {
						ipdsmsb_tblidx_del(&hpctbl->pce_sub_tbl, dmix);

						/* Delet the sub table */
						if (hpctbl->pce_sub_tbl.ipmsbtcnt[dmix] == 0) {
							ip_dasa_msb_tbl_del(cdev,
									    &hpctbl->pce_sub_tbl, dmix);
						}
					}

					if (hpctbl->pce_sub_tbl.iplsbtcnt[slix] > 0) {
						ipdslsb_tblidx_del(&hpctbl->pce_sub_tbl, slix);

						/* Delet the sub table */
						if (hpctbl->pce_sub_tbl.iplsbtcnt[slix] == 0) {
							ip_dasa_lsb_tbl_del(cdev,
									    &hpctbl->pce_sub_tbl, slix);
						}
					}

					if (hpctbl->pce_sub_tbl.ipmsbtcnt[smix] > 0) {
						ipdsmsb_tblidx_del(&hpctbl->pce_sub_tbl, smix);

						/* Delet the sub table */
						if (hpctbl->pce_sub_tbl.ipmsbtcnt[smix] == 0) {
							ip_dasa_msb_tbl_del(cdev,
									    &hpctbl->pce_sub_tbl, smix);
						}
					}

					/* Check the port map status */
					/* Delet the entry from Multicast sw Table */
					if (hitbl->mctable[i].pmap == 0)
						hitbl->mctable[i].valid = 0;

					MATCH = 1;

					if (parm->eModeMember == GSW_IGMP_MEMBER_EXCLUDE) {
						for (j = 0; j < gswdev->iflag.itblsize; j++) {
							if ((hitbl->mctable[j].dlsbindex == dlix) &&
							    (hitbl->mctable[j].slsbindex == 0x7F) &&
							    (hitbl->mctable[j].dmsbindex == dmix) &&
							    (hitbl->mctable[j].smsbindex == 0x7F) &&
							    (hitbl->mctable[j].valid == 1)) {
								if (((hitbl->mctable[j].pmap >> parm->nPortId) & 0x1) == 1) {
									hitbl->mctable[j].pmap &= ~(1 << parm->nPortId);

									if (hpctbl->pce_sub_tbl.iplsbtcnt[dlix] > 0) {
										ipdslsb_tblidx_del(&hpctbl->pce_sub_tbl, dlix);

										if (hpctbl->pce_sub_tbl.iplsbtcnt[dlix] == 0) {
											/* Delet the sub table */
											ip_dasa_lsb_tbl_del(cdev,
													    &hpctbl->pce_sub_tbl, dlix);
										}
									}

									if (hpctbl->pce_sub_tbl.ipmsbtcnt[dmix] > 0) {
										ipdsmsb_tblidx_del(&hpctbl->pce_sub_tbl, dmix);

										if (hpctbl->pce_sub_tbl.ipmsbtcnt[dmix] == 0) {
											/* Delet the sub table */
											ip_dasa_msb_tbl_del(cdev,
													    &hpctbl->pce_sub_tbl, dmix);
										}
									}

									/* Check the port map status */
									if (hitbl->mctable[j].pmap == 0) {
										/* Delet the entry from Multicast sw Table */
										hitbl->mctable[j].valid = 0;
										hitbl->mctable[i].valid = 0;
									}

									memset(&ptdata, 0, sizeof(pctbl_prog_t));
									ptdata.table = PCE_MULTICAST_SW_INDEX;
									ptdata.pcindex = j;
									ptdata.key[1] = ((0x7F << 8) | 0x7F);
									ptdata.key[0] = ((hitbl->mctable[j].dmsbindex << 8)
											 | (hitbl->mctable[i].dlsbindex));
									ptdata.val[0] = hitbl->mctable[j].pmap;
									ptdata.valid = hitbl->mctable[j].valid;
									gsw_pce_table_write(cdev, &ptdata);
								}
							}
						}
					}
				}

				break;
			}

			if (MATCH == 1) {
				memset(&ptdata, 0, sizeof(pctbl_prog_t));
				ptdata.table = PCE_MULTICAST_SW_INDEX;
				ptdata.pcindex = i;
				ptdata.key[1] = ((hitbl->mctable[i].smsbindex << 8)
						 | (hitbl->mctable[i].slsbindex));
				ptdata.key[0] = ((hitbl->mctable[i].dmsbindex << 8)
						 | (hitbl->mctable[i].dlsbindex));
				ptdata.val[0] = hitbl->mctable[i].pmap;
				ptdata.valid = hitbl->mctable[i].valid;
				gsw_pce_table_write(cdev, &ptdata);
			}
		}
	}

	if (MATCH == 0)
		pr_err("The GIP/SIP not found\n");

	return GSW_statusOk;
}
#endif /*CONFIG_LTQ_MULTICAST */
#if	((defined(CONFIG_LTQ_STP) && CONFIG_LTQ_STP) ||  (defined(CONFIG_LTQ_8021X) && CONFIG_LTQ_8021X))

/* Function: Internal function to program the registers */
/* when 802.1x and STP API are called. */
/* Description: Referene the matrix table to program the  */
/* LRNLIM, PSTATE and PEN bit*/
/* according to the Software architecture spec design.*/
static void set_port_state(void *cdev, u32 pid, u32 stpstate, u32 st8021)
{
	pctbl_prog_t tbl_prog_brdgeport_ingress;
	pctbl_prog_t tbl_prog_brdgeport_egress;
	u16 State;
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 i;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return;
	}

	for (i = 0; i < sizeof(pstpstate) / sizeof(pstpstate_t); i++) {
		pstpstate_t *pststate = &pstpstate[i];

		if ((pststate->psstate == stpstate) &&
		    (pststate->ps8021x == st8021)) {
			if (IS_VRSN_NOT_31(gswdev->gipver)) {
				gswdev->pconfig[pid].penable = pststate->pen_reg;
				gswdev->pconfig[pid].ptstate = pststate->pstate_reg;

				/* Learning Limit */
				if (pststate->lrnlim == 0) {
					gsw_w32(cdev, (PCE_PCTRL_1_LRNLIM_OFFSET + (0xA * pid)),
						PCE_PCTRL_1_LRNLIM_SHIFT, PCE_PCTRL_1_LRNLIM_SIZE, 0);
				} else {
					gsw_w32(cdev, (PCE_PCTRL_1_LRNLIM_OFFSET + (0xA * pid)),
						PCE_PCTRL_1_LRNLIM_SHIFT, PCE_PCTRL_1_LRNLIM_SIZE,
						gswdev->pconfig[pid].llimit);
				}

				/* Port State */
				gsw_w32(cdev, (PCE_PCTRL_0_PSTATE_OFFSET + (0xA * pid)),
					PCE_PCTRL_0_PSTATE_SHIFT, PCE_PCTRL_0_PSTATE_SIZE,
					gswdev->pconfig[pid].ptstate);
				/* Port Enable */
				gsw_w32(cdev, (SDMA_PCTRL_PEN_OFFSET + (0xA * pid)),
					SDMA_PCTRL_PEN_SHIFT, SDMA_PCTRL_PEN_SIZE,
					gswdev->pconfig[pid].penable);
			}

			if (IS_VRSN_31(gswdev->gipver)) {
				/*Same bridge port idx for ingress and egress bridge port configuration*/
				memset(&tbl_prog_brdgeport_ingress, 0, sizeof(pctbl_prog_t));
				tbl_prog_brdgeport_ingress.table = PCE_IGBGP_INDEX;

				CLEAR_U16(tbl_prog_brdgeport_ingress.pcindex);
				/*Table Entry address (Bridge port ingress Table index) Bit 7:0 in PCE_TBL_ADDR*/
				tbl_prog_brdgeport_ingress.pcindex |= (pid & 0x7F);

				memset(&tbl_prog_brdgeport_egress, 0, sizeof(pctbl_prog_t));
				tbl_prog_brdgeport_egress.table = PCE_EGBGP_INDEX;
				CLEAR_U16(tbl_prog_brdgeport_egress.pcindex);
				/*Table Entry address (Bridge port egress Table index) Bit 7:0 in PCE_TBL_ADDR*/
				tbl_prog_brdgeport_egress.pcindex |= (pid & 0x7F);
				/*Address-based read for ingress bridge port configuration*/
				gsw_pce_table_read(cdev, &tbl_prog_brdgeport_ingress);

				/*Address-based read for egress bridge port configuration*/
				gsw_pce_table_read(cdev, &tbl_prog_brdgeport_egress);

				/* Learning Limit */
				if (pststate->lrnlim == 0) {
					/*Disable learning and set learning limit to zero*/
					tbl_prog_brdgeport_ingress.val[0] |= (1 << 15);
				} else {
					/*Enable learning and set learning limit to given value*/
					tbl_prog_brdgeport_ingress.val[0] &= ~(1 << 15);
				}

				if (pststate->psstate == GSW_STP_PORT_STATE_DISABLE)
					State = PORT_STATE_DISABLE;
				else
					State = pststate->pstate_reg;

				/*VAL 0 Reg 2:0 STP state ingress/egress*/
				tbl_prog_brdgeport_ingress.val[0] &= ~0x7;
				tbl_prog_brdgeport_egress.val[0]  &= ~0x7;
				tbl_prog_brdgeport_ingress.val[0] |= (State & 0x7);
				tbl_prog_brdgeport_egress.val[0]  |= (State & 0x7);

				tbl_prog_brdgeport_ingress.table = PCE_IGBGP_INDEX;
				CLEAR_U16(tbl_prog_brdgeport_ingress.pcindex);
				/*Table Entry address (Bridge port ingress Table index) Bit 7:0 in PCE_TBL_ADDR*/
				tbl_prog_brdgeport_ingress.pcindex |= (pid & 0x7F);

				tbl_prog_brdgeport_egress.table = PCE_EGBGP_INDEX;
				CLEAR_U16(tbl_prog_brdgeport_egress.pcindex);
				/*Table Entry address (Bridge port egress Table index) Bit 7:0 in PCE_TBL_ADDR*/
				tbl_prog_brdgeport_egress.pcindex |= (pid & 0x7F);

				/*Address-based write for ingress bridge port configuration*/
				gsw_pce_table_write(cdev, &tbl_prog_brdgeport_ingress);
				/*Address-based write for egress bridge port configuration*/
				gsw_pce_table_write(cdev, &tbl_prog_brdgeport_egress);
			}
		}
	}
}
#endif /* CONFIG_LTQ_STP / CONFIG_LTQ_8021X */
#if defined(CONFIG_LTQ_QOS) && CONFIG_LTQ_QOS

/* Function: Internal function to calculate the mrate when */
/* Shaper and Meter API is called.*/
/*  Description: Calculate the mrate by input Ibs, Exp and Mant.*/
/* The algorithm designed based on software architecture spec.*/
static u32 mratecalc(u32 ibsid, u32 expont, u32 mants)
{
	static const u16 ibs_table[] = {8 * 8, 32 * 8, 64 * 8, 96 * 8};
	u16 ibs;
	u32 mrate = 0;

	if ((ibsid == 0) && (expont == 0) && (mants == 0))
		return 0;

	ibs = ibs_table[ibsid];

	if (mants)
		mrate = ((ibs * 25000) >> expont) / mants;

	return mrate;
}

/* Function: Internal function to calculate the Token when */
/* Shaper and Meter API is called.*/
/*  Description: Calculate the Token by input mrate, Ibs, Exp and Mant.*/
/* The algorithm designed based on software architecture spec. */
static int calc_mtoken(u32 mrate, u32 *ibsid, u32 *expont, u32 *mants)
{
	static const u16 ibs_table[] = {8 * 8, 32 * 8, 64 * 8, 96 * 8};
	u8 i;

	for (i = 3; i >= 0; i--) {
		u32 exp;
		u16 ibs = ibs_table[i];

		/* target is to get the biggest mantissa value */
		/* that can be used for the 10-Bit register */
		for (exp = 0; exp < 16; exp++) {
			u32 mant = ((ibs * 25000) >> exp) / mrate;

			if (mant < (1 << 10))  {
				*ibsid = i;
				*expont = exp;
				*mants = mant;
				return 0;
			}
		}
	}
}

static u32 mratecalc_3_1(u32 ibsid, u32 expont, u32 mants)
{
	u32 mrate = 0;

	if ((ibsid == 0) && (expont == 0) && (mants == 0))
		return 0;

	if (mants)
		mrate = (ibsid * 200000) / ((mants + 1) << expont);

	return mrate;
}


#if defined(WIN_PC_MODE) && WIN_PC_MODE
static __inline int fls(int input)
{
	u32 pos, ifzero = ~0;
	__asm__("bsr %1, %0\n\t"
		"cmovz %2, %0"
		: "=r"(pos)
		: "rm"(input)
		, "rm"(ifzero));
	return (int)(pos + 1);
}
#endif

static int calc_mtoken_3_1(u32 mrate, u32 *ibsid, u32 *expont, u32 *mants)
{
	u32 temp, exp_val, exp, mant, ibs;

	temp = 100000000 / mrate;
	exp_val = temp >> 9;
	exp = (u32)fls((int)exp_val);
	mant = exp ? 499 : (temp & 0x01FF);
	ibs = ((mant + 1) << exp) * mrate / 200000;

	*ibsid = ibs;
	*expont = exp;
	*mants = mant;

	return 0;
}

#endif /*CONFIG_LTQ_QOS */
#if defined(CONFIG_LTQ_VLAN) && CONFIG_LTQ_VLAN
static void reset_vlan_sw_table(void *cdev)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u16 i;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return;
	}

	for (i = 0; i < gswdev->avlantsz /* VLAN_ACTIVE_TABLE_SIZE */; i++)
		memset(&gswdev->avtable[i], 0, sizeof(avlan_tbl_t));
}
#endif /*CONFIG_LTQ_VLAN*/

#if defined(CONFIG_LTQ_VLAN) && CONFIG_LTQ_VLAN
u8 find_active_vlan_index(void *cdev, u16 vid)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u8 i, index = 0xFF;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

	for (i = 0; i < gswdev->avlantsz; i++) {
		if ((vid == gswdev->avtable[i].vid)
		    && (gswdev->avtable[i].valid == 1)) {
			index = i;
			break;
		}
	}

	return index;
}

u8 fempty_avlan_index_table(void *cdev)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u8 i, index = 0xFF;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

	for (i = 1; i < gswdev->avlantsz; i += 1) {
		if (gswdev->avtable[i].valid == 0) {
			index = i;
			break;
		}
	}

	if ((index == 0xFF) &&
	    (gswdev->avtable[0].valid == 0))
		return 0;

	return index;
}

static void vlan_entry_set(void *cdev, u8 index,
			   avlan_tbl_t *avlan_entry)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return;
	}

	gswdev->avtable[index].valid = avlan_entry->valid;
	gswdev->avtable[index].reserved	= avlan_entry->reserved;
	gswdev->avtable[index].vid = avlan_entry->vid;
	gswdev->avtable[index].fid = avlan_entry->fid;
	gswdev->avtable[index].pm = avlan_entry->pm;
	gswdev->avtable[index].tm = avlan_entry->tm;
}

static void get_vlan_sw_table(void *cdev, u8 pcindex,
			      avlan_tbl_t *avlan_entry)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return;
	}

	avlan_entry->valid = gswdev->avtable[pcindex].valid;
	avlan_entry->reserved = gswdev->avtable[pcindex].reserved;
	avlan_entry->vid = gswdev->avtable[pcindex].vid;
	avlan_entry->fid = gswdev->avtable[pcindex].fid;
	avlan_entry->pm = gswdev->avtable[pcindex].pm;
	avlan_entry->tm = gswdev->avtable[pcindex].tm;

}
#endif /* CONFIG_LTQ_VLAN */


static void get_gsw_hw_cap(void *cdev)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 reg_val;
	u16 hitstatus, u;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return;
	}

	gsw_r32(cdev, ETHSW_VERSION_REV_ID_OFFSET,
		ETHSW_VERSION_REV_ID_SHIFT, 16, &reg_val);
	/*GET GSWIP version*/
	gswdev->gipver = reg_val;

	/* Total number of ports*/
	gsw_r32(cdev, ETHSW_CAP_1_PPORTS_OFFSET,
		ETHSW_CAP_1_PPORTS_SHIFT,
		ETHSW_CAP_1_PPORTS_SIZE, &reg_val);
	gswdev->pnum = reg_val;

	/* Total number of ports including vitual ports*/
	gsw_r32(cdev, ETHSW_CAP_1_VPORTS_OFFSET,
		ETHSW_CAP_1_VPORTS_SHIFT,
		ETHSW_CAP_1_VPORTS_SIZE, &reg_val);
	gswdev->tpnum = reg_val + gswdev->pnum;

	gsw_r32(cdev, ETHSW_CAP_1_QUEUE_OFFSET,
		ETHSW_CAP_1_QUEUE_SHIFT,
		ETHSW_CAP_1_QUEUE_SIZE, &reg_val);
	gswdev->num_of_queues = reg_val;

	gsw_r32(cdev, ETHSW_CAP_3_METERS_OFFSET,
		ETHSW_CAP_3_METERS_SHIFT,
		ETHSW_CAP_3_METERS_SIZE, &reg_val);
	gswdev->num_of_meters = reg_val;

	gsw_r32(cdev, ETHSW_CAP_3_SHAPERS_OFFSET,
		ETHSW_CAP_3_SHAPERS_SHIFT,
		ETHSW_CAP_3_SHAPERS_SIZE, &reg_val);
	gswdev->num_of_shapers = reg_val;

	gsw_r32(cdev, ETHSW_CAP_4_PPPOE_OFFSET,
		ETHSW_CAP_4_PPPOE_SHIFT,
		ETHSW_CAP_4_PPPOE_SIZE, &reg_val);
	gswdev->num_of_pppoe = reg_val;


	gsw_r32(cdev, ETHSW_CAP_5_IPPLEN_OFFSET,
		ETHSW_CAP_5_IPPLEN_SHIFT,
		ETHSW_CAP_5_IPPLEN_SIZE, &reg_val);
	gswdev->ip_pkt_lnt_size = reg_val;


	gsw_r32(cdev, ETHSW_CAP_5_PROT_OFFSET,
		ETHSW_CAP_5_PROT_SHIFT,
		ETHSW_CAP_5_PROT_SIZE, &reg_val);
	gswdev->prot_table_size = reg_val;

	gsw_r32(cdev, ETHSW_CAP_6_MACDASA_OFFSET,
		ETHSW_CAP_6_MACDASA_SHIFT,
		ETHSW_CAP_6_MACDASA_SIZE, &reg_val);
	gswdev->mac_dasa_table_size = reg_val;

	gsw_r32(cdev, ETHSW_CAP_6_APPL_OFFSET,
		ETHSW_CAP_6_APPL_SHIFT,
		ETHSW_CAP_6_APPL_SIZE, &reg_val);
	gswdev->app_table_size = reg_val;


	gsw_r32(cdev, ETHSW_CAP_7_IPDASAM_OFFSET,
		ETHSW_CAP_7_IPDASAM_SHIFT,
		ETHSW_CAP_7_IPDASAM_SIZE, &reg_val);
	gswdev->idsmtblsize = reg_val;


	gsw_r32(cdev, ETHSW_CAP_7_IPDASAL_OFFSET,
		ETHSW_CAP_7_IPDASAL_SHIFT,
		ETHSW_CAP_7_IPDASAL_SIZE, &reg_val);
	gswdev->idsltblsize = reg_val;


	gsw_r32(cdev, ETHSW_CAP_8_MCAST_OFFSET,
		ETHSW_CAP_8_MCAST_SHIFT,
		ETHSW_CAP_8_MCAST_SIZE, &reg_val);
	gswdev->mctblsize = reg_val;


	gsw_r32(cdev, ETHSW_CAP_9_FLAGG_OFFSET,
		ETHSW_CAP_9_FLAGG_SHIFT,
		ETHSW_CAP_9_FLAGG_SIZE, &reg_val);
	gswdev->tftblsize = reg_val;


	gsw_r32(cdev, ETHSW_CAP_10_MACBT_OFFSET,
		ETHSW_CAP_10_MACBT_SHIFT,
		ETHSW_CAP_10_MACBT_SIZE, &reg_val);
	gswdev->mactblsize = reg_val;



	if (gswdev->gipver == LTQ_GSWIP_3_0) {
		gsw_r32(cdev, ETHSW_CAP_4_VLAN_OFFSET,
			ETHSW_CAP_4_VLAN_SHIFT,
			ETHSW_CAP_4_VLAN_SIZE, &reg_val);
		gswdev->avlantsz = reg_val;

		gsw_r32(cdev, ETHSW_CAP_14_SMAC_OFFSET,
			ETHSW_CAP_14_SMAC_SHIFT,
			ETHSW_CAP_14_SMAC_SIZE, &reg_val);
		gswdev->num_of_rt_smac = (1 << reg_val);
		gsw_r32(cdev, ETHSW_CAP_14_DMAC_OFFSET,
			ETHSW_CAP_14_DMAC_SHIFT,
			ETHSW_CAP_14_DMAC_SIZE, &reg_val);
		gswdev->num_of_rt_dmac = (1 << reg_val);
		gsw_r32(cdev, ETHSW_CAP_14_PPPoE_OFFSET,
			ETHSW_CAP_14_PPPoE_SHIFT,
			ETHSW_CAP_14_PPPoE_SIZE, &reg_val);
		gswdev->num_of_rt_ppoe = (1 << reg_val);
		gsw_r32(cdev, ETHSW_CAP_14_NAT_OFFSET,
			ETHSW_CAP_14_NAT_SHIFT,
			ETHSW_CAP_14_NAT_SIZE, &reg_val);
		gswdev->num_of_rt_nat = (1 << reg_val);
		gsw_r32(cdev, ETHSW_CAP_15_MTU_OFFSET,
			ETHSW_CAP_15_MTU_SHIFT,
			ETHSW_CAP_15_MTU_SIZE, &reg_val);
		gswdev->num_of_rt_mtu = (1 << reg_val);
		gsw_r32(cdev, ETHSW_CAP_15_TUNNEL_OFFSET,
			ETHSW_CAP_15_TUNNEL_SHIFT,
			ETHSW_CAP_15_TUNNEL_SIZE, &reg_val);
		gswdev->num_of_rt_tunnel = (1 << reg_val);
		gsw_r32(cdev, ETHSW_CAP_15_RTP_OFFSET,
			ETHSW_CAP_15_RTP_SHIFT,
			ETHSW_CAP_15_RTP_SIZE, &reg_val);
		gswdev->num_of_rt_rtp = (1 << reg_val);
		gswdev->cport = GSW_3X_SOC_CPU_PORT;
	}

	if (gswdev->gipver == LTQ_GSWIP_3_0 || IS_VRSN_31(gswdev->gipver)) {
		gsw_r32(cdev, ETHSW_CAP_13_PMAC_OFFSET,
			ETHSW_CAP_13_PMAC_SHIFT,
			ETHSW_CAP_13_PMAC_SIZE, &reg_val);
		gswdev->num_of_pmac = reg_val;

		gsw_r32(cdev, ETHSW_CAP_13_PAYLOAD_OFFSET,
			ETHSW_CAP_13_PAYLOAD_SHIFT,
			ETHSW_CAP_13_PAYLOAD_SIZE, &reg_val);
		gswdev->pdtblsize = (1 << reg_val);

		gsw_r32(cdev, ETHSW_CAP_13_INTRMON_OFFSET,
			ETHSW_CAP_13_INTRMON_SHIFT,
			ETHSW_CAP_13_INTRMON_SIZE, &reg_val);
		gswdev->num_of_ifrmon = (1 << reg_val);

		gsw_r32(cdev, ETHSW_CAP_13_EVLAN_OFFSET,
			ETHSW_CAP_13_EVLAN_SHIFT,
			ETHSW_CAP_13_EVLAN_SIZE, &reg_val);

		if (gswdev->gipver == LTQ_GSWIP_3_0)
			gswdev->num_of_egvlan = (1 << reg_val);

		if (IS_VRSN_31(gswdev->gipver))
			gswdev->num_of_extendvlan = (1 << reg_val);

		gswdev->cport = GSW_3X_SOC_CPU_PORT;

	} else {
		gswdev->cport = GSW_2X_SOC_CPU_PORT;
	}

	/*Applicable only for 3.1*/
	if (gswdev->gipver >= LTQ_GSWIP_3_1) {
		gsw_r32(cdev, ETHSW_CAP_16_VLANMAP_OFFSET,
			ETHSW_CAP_16_VLANMAP_SHIFT,
			ETHSW_CAP_16_VLANMAP_SIZE, &reg_val);
		gswdev->num_of_vlanfilter = 1 << reg_val;

		gsw_r32(cdev, ETHSW_CAP_16_MCASTHW_OFFSET,
			ETHSW_CAP_16_MCASTHW_SHIFT,
			ETHSW_CAP_16_MCASTHW_SIZE, &reg_val);
		gswdev->mcsthw_snoop = reg_val ? MCAST_HWSNOOP_DIS : MCAST_HWSNOOP_EN;

		gsw_r32(cdev, ETHSW_CAP_17_BRG_OFFSET,
			ETHSW_CAP_17_BRG_SHIFT,
			ETHSW_CAP_17_BRG_SIZE, &reg_val);
		gswdev->num_of_bridge = 1 << reg_val;


		gsw_r32(cdev, ETHSW_CAP_17_BRGPT_OFFSET,
			ETHSW_CAP_17_BRGPT_SHIFT,
			ETHSW_CAP_17_BRGPT_SIZE, &reg_val);
		gswdev->num_of_bridge_port = 1 << reg_val;


		gsw_r32(cdev, ETHSW_CAP_17_PMAP_OFFSET,
			ETHSW_CAP_17_PMAP_SHIFT,
			ETHSW_CAP_17_PMAP_SIZE, &reg_val);
		gswdev->num_of_pmapper = 1 << reg_val;

		gsw_r32(cdev, ETHSW_CAP_18_CTP_OFFSET,
			ETHSW_CAP_18_CTP_SHIFT,
			ETHSW_CAP_18_CTP_SIZE, &reg_val);
		gswdev->num_of_ctp = (reg_val & 0xFFFF);
	}

	/*load PCE parameters*/
	gswdev->pce_tbl_reg.key = gsw_pce_tbl_reg_key;
	gswdev->pce_tbl_reg.mask = gsw_pce_tbl_reg_mask;
	gswdev->pce_tbl_reg.value = gsw_pce_tbl_reg_value;

	if (gswdev->gipver == LTQ_GSWIP_3_1) {
		gswdev->num_of_pce_tbl = ARRAY_SIZE(gsw_pce_tbl_info_31);
		gswdev->pce_tbl_info = gsw_pce_tbl_info_31;
	} else if (gswdev->gipver == LTQ_GSWIP_3_0) {
		gswdev->num_of_pce_tbl = ARRAY_SIZE(gsw_pce_tbl_info_30);
		gswdev->pce_tbl_info = gsw_pce_tbl_info_30;
	} else {
		gswdev->num_of_pce_tbl = ARRAY_SIZE(gsw_pce_tbl_info_22);
		gswdev->pce_tbl_info = gsw_pce_tbl_info_22;
	}

	if (IS_VRSN_31_OR_32(gswdev->gipver) && gswdev->num_of_bridge_port) {
		hitstatus = gswdev->num_of_bridge_port - 1;
		u = hitstatus / (sizeof(gswdev->hitstatus_mask) * 8);
		gswdev->hitstatus_idx = u;
		u = hitstatus % (sizeof(gswdev->hitstatus_mask) * 8);
		gswdev->hitstatus_mask = BIT(u);
	}
	if (1) {
		if (gswdev->gipver == LTQ_GSWIP_3_0)
			pr_debug("\nGSWIP 3.0 HardWare Capability\n");
		else if (gswdev->gipver == LTQ_GSWIP_3_1)
			pr_debug("\nGSWIP 3.1 HardWare Capability\n");
		else
			pr_debug("\nGSWIP 2.2 HardWare Capability\n");

		pr_debug("-----------------------------\n\n");
		pr_debug("Switch Version ID                =  0x%x\n", gswdev->gipver);
		pr_debug("\n");
		pr_debug("Number of logical port           =  %d\n", gswdev->pnum);
		pr_debug("Number of ports including V port =  %d\n", gswdev->tpnum);

		if (gswdev->gipver == LTQ_GSWIP_3_1) {
			pr_debug("Number of CTP Port               =  %d\n", gswdev->num_of_ctp);
			pr_debug("Number of Bridge                 =  %d\n", gswdev->num_of_bridge);
			pr_debug("Number of Bridge Port            =  %d\n", gswdev->num_of_bridge_port);
			pr_debug("Number of P-Mapper               =  %d\n", gswdev->num_of_pmapper);

		}

		pr_debug("Number of queues                 =  %d\n", gswdev->num_of_queues);
		pr_debug("Number of meter instance         =  %d\n", gswdev->num_of_meters);
		pr_debug("Number of shapers                =  %d\n", gswdev->num_of_shapers);
		pr_debug("Number of PMAC                   =  %d\n", gswdev->num_of_pmac);
		pr_debug("Number of CPU PORT               =  %d\n", gswdev->cport);
		pr_debug("\n");
		pr_debug("PPPOE table size                 =  %d\n", gswdev->num_of_pppoe);
		pr_debug("IP packet length table size      =  %d\n", gswdev->ip_pkt_lnt_size);
		pr_debug("Protocol table size              =  %d\n", gswdev->prot_table_size);
		pr_debug("MAC DA/SA table size             =  %d\n", gswdev->mac_dasa_table_size);
		pr_debug("Application table size           =  %d\n", gswdev->app_table_size);
		pr_debug("IP DA/SA MSB table size          =  %d\n", gswdev->idsmtblsize);
		pr_debug("IP DA/SA LSB table size          =  %d\n", gswdev->idsltblsize);
		pr_debug("Multicast table size             =  %d\n", gswdev->mctblsize);
		pr_debug("Multicast Hw Snoop               =  %d\n", gswdev->mcsthw_snoop);
		pr_debug("TFLOW table size                 =  %d\n", gswdev->tftblsize);
		pr_debug("MAC bridge table size            =  %d\n", gswdev->mactblsize);

		if (gswdev->gipver == LTQ_GSWIP_3_0 || IS_VRSN_31(gswdev->gipver)) {
			pr_debug("TFLOW RMON counter table Size    =  %d\n", gswdev->num_of_ifrmon);
			pr_debug("Payload Table Size               =  %d\n", gswdev->pdtblsize);
			pr_debug("Extend VLAN Table Size table     =  %d\n", gswdev->num_of_extendvlan);
			pr_debug("VlanFilter table Size            =  %d\n\n", gswdev->num_of_vlanfilter);
		}
	}

}

#if defined(WIN_PC_MODE) && WIN_PC_MODE
int gsw_r_init()
{
	return GSW_statusOk;
}
#endif

#if defined(CONFIG_USE_EMULATOR) && CONFIG_USE_EMULATOR
static GSW_return_t emulator_config_coreinit_gswip3_0(void *cdev)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 j = 0;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

	if (gswdev->gipver == LTQ_GSWIP_3_0) {
		if (gswdev->sdev == LTQ_FLOW_DEV_INT_R)
#ifdef __KERNEL__
			gsw_r_init();

#endif
		/* Set Auto-Polling of connected PHYs - For all ports */
		gsw_w32(cdev, (GSWT_MDCCFG_0_PEN_1_OFFSET + GSW30_TOP_OFFSET),
			GSWT_MDCCFG_0_PEN_1_SHIFT, 6, 0x0);
	} else {
		/* Set Auto-Polling of connected PHYs - For all ports */
		gsw_w32(cdev, (MDC_CFG_0_PEN_0_OFFSET
			       + GSW_TREG_OFFSET),
			MDC_CFG_0_PEN_0_SHIFT, 6, 0x0);
	}

	for (j = 0; j < gswdev->pnum - 1; j++) {
		u32 reg_value;

		if (gswdev->gipver == LTQ_GSWIP_3_0) {
			gsw_w32(cdev,
				(FDMA_PCTRL_EN_OFFSET + (j * 0x6)),
				FDMA_PCTRL_EN_SHIFT,
				FDMA_PCTRL_EN_SIZE, 1);
			gsw_w32(cdev,
				(SDMA_PCTRL_PEN_OFFSET + (j * 0x6)),
				SDMA_PCTRL_PEN_SHIFT,
				SDMA_PCTRL_PEN_SIZE, 1);
			gsw_w32(cdev,
				(BM_PCFG_CNTEN_OFFSET + (j * 2)),
				BM_PCFG_CNTEN_SHIFT,
				BM_PCFG_CNTEN_SIZE, 1);
			gsw_w32(cdev,
				(MAC_CTRL_0_FDUP_OFFSET + (0xC * j)),
				MAC_CTRL_0_FDUP_SHIFT,
				MAC_CTRL_0_FDUP_SIZE, 1);
			gsw_w32(cdev,
				(MAC_CTRL_0_GMII_OFFSET + (0xC * j)),
				MAC_CTRL_0_GMII_SHIFT,
				MAC_CTRL_0_GMII_SIZE, 2);

			gsw_w32(cdev,
				((GSWT_PHY_ADDR_1_SPEED_OFFSET + (j * 4))
				 + GSW30_TOP_OFFSET),
				GSWT_PHY_ADDR_1_SPEED_SHIFT,
				GSWT_PHY_ADDR_1_SPEED_SIZE, 2);
			gsw_w32(cdev,
				((GSWT_PHY_ADDR_1_FDUP_OFFSET
				  + (j * 4))
				 + GSW30_TOP_OFFSET),
				GSWT_PHY_ADDR_1_FDUP_SHIFT,
				GSWT_PHY_ADDR_1_FDUP_SIZE, 1);
			gsw_w32(cdev,
				((GSWT_PHY_ADDR_1_LNKST_OFFSET
				  + (j * 4))
				 + GSW30_TOP_OFFSET),
				GSWT_PHY_ADDR_1_LNKST_SHIFT,
				GSWT_PHY_ADDR_1_LNKST_SIZE, 1);
			gsw_r32(cdev,
				((GSWT_PHY_ADDR_1_LNKST_OFFSET
				  + (j * 4))
				 + GSW30_TOP_OFFSET),
				0, 16, &reg_value);
		}
	}

	return GSW_statusOk;
}
#endif

/*This API is used for GSWIP 3.0 and Lower*/
static GSW_return_t set_srcmac_pauseframe(void *cdev)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

	/* Set the the source MAC register of pause frame gswip 3.0 */
	gsw_w32(cdev, MAC_PFSA_0_PFAD_OFFSET,
		MAC_PFSA_0_PFAD_SHIFT,
		MAC_PFSA_0_PFAD_SIZE, 0x0000);
	gsw_w32(cdev, MAC_PFSA_1_PFAD_OFFSET,
		MAC_PFSA_1_PFAD_SHIFT,
		MAC_PFSA_1_PFAD_SIZE, 0x9600);
	gsw_w32(cdev, MAC_PFSA_2_PFAD_OFFSET,
		MAC_PFSA_2_PFAD_SHIFT,
		MAC_PFSA_2_PFAD_SIZE, 0xAC9A);
	return GSW_statusOk;
}

static GSW_return_t mdio_init_gswip3_0(void *cdev)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

	/* Configure the MDIO Clock 97.6 Khz 3.0*/
	gsw_w32(cdev, (GSWT_MDCCFG_1_FREQ_OFFSET + GSW30_TOP_OFFSET),
		GSWT_MDCCFG_1_FREQ_SHIFT, GSWT_MDCCFG_1_FREQ_SIZE, 0xFF);
	gsw_w32(cdev, (GSWT_MDCCFG_1_MCEN_OFFSET + GSW30_TOP_OFFSET),
		GSWT_MDCCFG_1_MCEN_SHIFT, GSWT_MDCCFG_1_MCEN_SIZE, 1);
	/* MDIO Hardware Reset */
	gsw_w32(cdev, (GSWT_MDCCFG_1_RES_OFFSET + GSW30_TOP_OFFSET),
		GSWT_MDCCFG_1_RES_SHIFT, GSWT_MDCCFG_1_RES_SIZE, 1);

	return GSW_statusOk;
}

static GSW_return_t mdio_init_gswip2_2(void *cdev)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

	/* Configure the MDIO Clock 97.6 Khz 2.2*/
	gsw_w32(cdev, (MDC_CFG_1_FREQ_OFFSET
		       + GSW_TREG_OFFSET),
		MDC_CFG_1_FREQ_SHIFT,
		MDC_CFG_1_FREQ_SIZE, 0xFF);
	return GSW_statusOk;
}

static GSW_return_t legacy_switch_core_init(void *cdev)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 j = 0;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#if defined(CONFIG_LTQ_VLAN) && CONFIG_LTQ_VLAN
	/** Reset vlan software tableApplicable only for GSWIP 3.0 */
	reset_vlan_sw_table(cdev);
#endif /* CONFIG_LTQ_VLAN */
	ltq_ethsw_port_cfg_init(cdev);
	/* Set the the source MAC register of pause frame  */
	set_srcmac_pauseframe(cdev);

	if (gswdev->gipver == LTQ_GSWIP_3_0) {
		/**Apllicable only for GSWIP 3.0**/
#if defined(CONFIG_USE_EMULATOR) && CONFIG_USE_EMULATOR
		emulator_config_coreinit_gswip3_0(cdev);
#else
		mdio_init_gswip3_0(cdev);

		if (gswdev->sdev == LTQ_FLOW_DEV_INT_R) {

#ifdef __KERNEL__
			gsw_r_init();
#endif
			gsw_w32(cdev, (GSWT_MDCCFG_0_PEN_1_OFFSET + GSW30_TOP_OFFSET),
				GSWT_MDCCFG_0_PEN_1_SHIFT, 6, 0x1);
			/*			gsw_w32(cdev, (GSWT_PHY_ADDR_1_LNKST_OFFSET + GSW30_TOP_OFFSET),
							GSWT_PHY_ADDR_1_LNKST_SHIFT, GSWT_PHY_ADDR_1_LNKST_SIZE, 1);*/
		} else {
			gsw_w32(cdev, (GSWT_MDCCFG_0_PEN_1_OFFSET
				       + GSW30_TOP_OFFSET),
				GSWT_MDCCFG_0_PEN_1_SHIFT, 6, 0x1e);
		}

		/* SDMA/FDMA and RMON counter Enable*/
		for (j = 0; j < gswdev->pnum - 1; j++) {
			if (gswdev->gipver == LTQ_GSWIP_3_0) {
				gsw_w32(cdev,
					(FDMA_PCTRL_EN_OFFSET + (j * 0x6)),
					FDMA_PCTRL_EN_SHIFT,
					FDMA_PCTRL_EN_SIZE, 1);
				gsw_w32(cdev,
					(SDMA_PCTRL_PEN_OFFSET + (j * 0x6)),
					SDMA_PCTRL_PEN_SHIFT,
					SDMA_PCTRL_PEN_SIZE, 1);
				gsw_w32(cdev,
					(BM_PCFG_CNTEN_OFFSET + (j * 2)),
					BM_PCFG_CNTEN_SHIFT,
					BM_PCFG_CNTEN_SIZE, 1);
			}
		}

		for (j = 0; j < gswdev->pnum - 1; j++) {
			gsw_w32(cdev,
				((GSWT_ANEG_EEE_1_CLK_STOP_CAPABLE_OFFSET
				  + (4 * j)) + GSW30_TOP_OFFSET),
				GSWT_ANEG_EEE_1_CLK_STOP_CAPABLE_SHIFT,
				GSWT_ANEG_EEE_1_CLK_STOP_CAPABLE_SIZE, 0x3);
		}

#endif
	} else {
		/* Switch Core 2.2 and lower*/
		mdio_init_gswip2_2(cdev);

		/* EEE auto negotiation overides:  */
		/*clock disable (ANEG_EEE_0.CLK_STOP_CAPABLE)  */
		for (j = 0; j < gswdev->pnum - 1; j++) {
			gsw_w32(cdev,
				((ANEG_EEE_0_CLK_STOP_CAPABLE_OFFSET + j)
				 + GSW_TREG_OFFSET),
				ANEG_EEE_0_CLK_STOP_CAPABLE_SHIFT,
				ANEG_EEE_0_CLK_STOP_CAPABLE_SIZE, 0x3);
		}
	}

	if (gswdev->gipver == LTQ_GSWIP_3_0) {
		for (j = 0; j < gswdev->tpnum; j++) {
			//pr_err("Enable CTP RMON for logical port %d\n",j);
			gsw_w32(cdev, (BM_PCFG_CNTEN_OFFSET + (j * 2)), BM_PCFG_CNTEN_SHIFT,
				BM_PCFG_CNTEN_SIZE, 1);
			gsw_w32(cdev, (BM_RMON_CTRL_BCAST_CNT_OFFSET + (j * 0x2)),
				BM_RMON_CTRL_BCAST_CNT_SHIFT, BM_RMON_CTRL_BCAST_CNT_SIZE, 1);
		}

		if (gswdev->gipver == LTQ_GSWIP_3_0 && gswdev->sdev == LTQ_FLOW_DEV_INT_R) {
			gsw_w32(cdev, PCE_TFCR_NUM_NUM_OFFSET, PCE_TFCR_NUM_NUM_SHIFT,
				PCE_TFCR_NUM_NUM_SIZE, 0x80);
		}
	}

	/*Initalize GSWIP Irq*/
	GSW_Irq_init(cdev);

	return GSW_statusOk;
}

static GSW_return_t switch_core_init(void *cdev)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	static GSW_BRIDGE_portConfig_t brdgport_config;
	static GSW_CTP_portConfig_t ctp_config;
	static GSW_CTP_portAssignment_t ctp_assign;
	static pctbl_prog_t tbl_prog_ctpport_ingress;
	static pctbl_prog_t tbl_prog_ctpport_egress;
	u32 j = 0, ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

	if (IS_VRSN_31(gswdev->gipver)) {
#ifdef __KERNEL__

		/*Default Exvlan and Vlan filter Local table Init*/
		gswdev->extendvlan_idx.nUsedEntry = 0;

		for (j = 0; j < gswdev->num_of_extendvlan; j++) {
			gswdev->extendvlan_idx.vlan_idx[j].VlanBlockId = EXVLAN_ENTRY_INVALID;
			gswdev->extendvlan_idx.vlan_idx[j].IndexInUsageCnt = 0;
		}

		gswdev->vlanfilter_idx.nUsedEntry = 0;

		for (j = 0; j < gswdev->num_of_vlanfilter; j++) {
			gswdev->vlanfilter_idx.filter_idx[j].FilterBlockId = VLANFILTER_ENTRY_INVALID;
			gswdev->vlanfilter_idx.filter_idx[j].IndexInUsageCnt = 0;
		}

		/**Bridge Settings*/
		/*Default configuration :Allocate Bridge/FID 0*/
		gswdev->brdgeconfig_idx[0].IndexInUse = 1;
		/*Default configuration :Allocate Bridge port 0 and 1*/
		gswdev->brdgeportconfig_idx[0].IndexInUse = 1;
		gswdev->brdgeportconfig_idx[1].IndexInUse = 1;

		/*Default configuration :Force Set 0-127 Ingress bridge port mapping
		  to no Member and with Bridge/Fid 0,
		  Egress bridge port destination logical port to 0 */
		memset(&brdgport_config, 0, sizeof(GSW_BRIDGE_portConfig_t));
		brdgport_config.eMask = GSW_BRIDGE_PORT_CONFIG_MASK_FORCE |
					GSW_BRIDGE_PORT_CONFIG_MASK_BRIDGE_ID |
					GSW_BRIDGE_PORT_CONFIG_MASK_EGRESS_CTP_MAPPING |
					GSW_BRIDGE_PORT_CONFIG_MASK_BRIDGE_PORT_MAP;

		for (j = 0; j < gswdev->num_of_bridge_port; j++) {
			brdgport_config.nBridgePortId = j;
			/*initialize the local BP table*/
			gswdev->brdgeportconfig_idx[j].IngressExVlanBlkId = EXVLAN_ENTRY_INVALID;
			gswdev->brdgeportconfig_idx[j].EgressExVlanBlkId = EXVLAN_ENTRY_INVALID;
			gswdev->brdgeportconfig_idx[j].IngressVlanFilterBlkId = VLANFILTER_ENTRY_INVALID;
			gswdev->brdgeportconfig_idx[j].EgressVlanFilter1BlkId = VLANFILTER_ENTRY_INVALID;
			gswdev->brdgeportconfig_idx[j].EgressVlanFilter2BlkId = VLANFILTER_ENTRY_INVALID;
			gswdev->brdgeportconfig_idx[j].BrdgId = 0;
			gswdev->brdgeportconfig_idx[j].LearningLimit = 0xFF;
			/*Default STP State is GSW_STP_PORT_STATE_FORWARD
			  it can be changed using GSW_Stp_PortCfgSet*/
			gswdev->brdgeportconfig_idx[j].StpState =
				GSW_STP_PORT_STATE_FORWARD;
			/*By Default 8021X State is GSW_8021X_PORT_STATE_AUTHORIZED
			  it can be changed using GSW_8021X_PortCfgSet*/
			gswdev->brdgeportconfig_idx[j].P8021xState =
				GSW_8021X_PORT_STATE_AUTHORIZED;

			ret = GSW_BridgePortConfigSet(cdev, &brdgport_config);

			if (ret == GSW_statusErr) {
				pr_err("%s:%s:%d (GSW_BridgePortConfigSet returns ERROR)\n",
				       __FILE__, __func__, __LINE__);
				return GSW_statusErr;
			}
		}

		/**Logical Port Assignment Settings*/
		/*Default Configuration :Allocate and Assign Logical port 0 with CTP 0-PMAC 0*/
		/*and*/
		/*Default Configuration :Allocate and Assign Logical port 1 with CTP 1-PMAC 1*/
		ctp_assign.nNumberOfCtpPort = 1;
		ctp_assign.eMode = GSW_LOGICAL_PORT_OTHER;

		for (j = 0; j < 2; j++) {
			gswdev->ctpportconfig_idx[j].IndexInUse = 1;
			ctp_assign.nFirstCtpPortId = j;
			ctp_assign.nLogicalPortId = j;
			ret = GSW_CTP_PortAssignmentSet(cdev, &ctp_assign);

			if (ret == GSW_statusErr) {
				pr_err("%s:%s:%d (GSW_CTP_PortAssignmentSet returns ERROR)\n",
				       __FILE__, __func__, __LINE__);
				return GSW_statusErr;
			}
		}

		/*Allocate CTP 287 - user is not allowed to use this CTP port*/
		/*Default Configuration :Allocate and Assign Logical port 2 to 11 with CTP 287*/
		gswdev->ctpportconfig_idx[287].IndexInUse = 1;
		ctp_assign.nFirstCtpPortId = 287;
		ctp_assign.nNumberOfCtpPort = 1;
		ctp_assign.eMode = GSW_LOGICAL_PORT_OTHER;

		for (j = 2; j < gswdev->tpnum; j++) {
			ctp_assign.nLogicalPortId = j;
			ret = GSW_CTP_PortAssignmentSet(cdev, &ctp_assign);

			if (ret == GSW_statusErr) {
				pr_err("%s:%s:%d (GSW_CTP_PortAssignmentSet returns ERROR)\n",
				       __FILE__, __func__, __LINE__);
				return GSW_statusErr;
			}
		}

		/**CTP port Settings*/
		/*Default configuration :CTP 0 and 1
		  Assign Bridge port 0  and 1 respectively
		  and disable bridge bypass*/
		for (j = 0; j < 2; j++) {
			/*initialize the local CTP table*/
			gswdev->ctpportconfig_idx[j].BrdgPortId = j;
			gswdev->ctpportconfig_idx[j].AssociatedLogicalPort = j;
			gswdev->ctpportconfig_idx[j].IngressExVlanIgmpBlkId = EXVLAN_ENTRY_INVALID;
			gswdev->ctpportconfig_idx[j].IngressExVlanNonIgmpBlkId = EXVLAN_ENTRY_INVALID;
			gswdev->ctpportconfig_idx[j].EgressExVlanIgmpBlkId = EXVLAN_ENTRY_INVALID;
			gswdev->ctpportconfig_idx[j].EgressExVlanNonIgmpBlkId = EXVLAN_ENTRY_INVALID;
			memset(&ctp_config, 0, sizeof(GSW_CTP_portConfig_t));
			ctp_config.nLogicalPortId = j;
			ctp_config.nSubIfIdGroup = 0;
			ctp_config.nBridgePortId = j;
			ctp_config.eMask = GSW_CTP_PORT_CONFIG_MASK_BRIDGE_PORT_ID |
					   GSW_CTP_PORT_CONFIG_BRIDGING_BYPASS;
			ret = GSW_CtpPortConfigSet(cdev, &ctp_config);

			if (ret == GSW_statusErr) {
				pr_err("%s:%s:%d (GSW_CtpPortConfigSet returns ERROR)\n",
				       __FILE__, __func__, __LINE__);
				return GSW_statusErr;
			}
		}

		/*Allocate Bridge 127- user is not allowed to use this BP
		 CTP 2 to 286 not assigned to any logical port and configured with bridge port 127
		 bridge port 0,1 ,127 and CTP 287 is Special port/Special usage*/
		gswdev->brdgeportconfig_idx[127].IndexInUse = 1;

		for (j = 2; j < gswdev->num_of_ctp; j++) {
			gswdev->ctpportconfig_idx[j].AssociatedLogicalPort = NOT_ASSIGNED;
			gswdev->ctpportconfig_idx[j].BrdgPortId = 127;
			gswdev->ctpportconfig_idx[j].IngressExVlanIgmpBlkId = EXVLAN_ENTRY_INVALID;
			gswdev->ctpportconfig_idx[j].IngressExVlanNonIgmpBlkId = EXVLAN_ENTRY_INVALID;
			gswdev->ctpportconfig_idx[j].EgressExVlanIgmpBlkId = EXVLAN_ENTRY_INVALID;
			gswdev->ctpportconfig_idx[j].EgressExVlanNonIgmpBlkId = EXVLAN_ENTRY_INVALID;

			/*Same ctp port idx for ingress and egress ctp port configuration*/
			memset(&tbl_prog_ctpport_ingress, 0, sizeof(pctbl_prog_t));
			tbl_prog_ctpport_ingress.table = PCE_IGCTP_INDEX;
			CLEAR_U16(tbl_prog_ctpport_ingress.pcindex);
			/*Table Entry address (ctp port ingress Table index) Bit 8:0 in PCE_TBL_ADDR*/
			tbl_prog_ctpport_ingress.pcindex |= (j & 0x1FF);

			memset(&tbl_prog_ctpport_egress, 0, sizeof(pctbl_prog_t));
			tbl_prog_ctpport_egress.table = PCE_EGCTP_INDEX;
			CLEAR_U16(tbl_prog_ctpport_egress.pcindex);
			/*Table Entry address (ctp port egress Table index) Bit 8:0 in PCE_TBL_ADDR*/
			tbl_prog_ctpport_egress.pcindex |= (j & 0x1FF);

			tbl_prog_ctpport_ingress.val[0] &= ~(0xFF);
			/*bridge port 127*/
			tbl_prog_ctpport_ingress.val[0] |= 127;

			/*Address-based write for ingress ctp port configuration*/
			gsw_pce_table_write(cdev, &tbl_prog_ctpport_ingress);
			/*Address-based write for egress ctp port configuration*/
			gsw_pce_table_write(cdev, &tbl_prog_ctpport_egress);

		}

		/*Enable SDMA for logical port 0 (PMAC 0) and 1 (PMAC 1) only
		  Note: Other logical port's SDMA will be enabled only during CTP Alloc*/
		for (j = 0; j < 2 ; j++) {
			gsw_w32(cdev,
				(SDMA_PCTRL_PEN_OFFSET + (j * 0x6)),
				SDMA_PCTRL_PEN_SHIFT,
				SDMA_PCTRL_PEN_SIZE, 1);
		}

		/*Enable FDMA for all logical ports*/
		for (j = 0; j < gswdev->tpnum; j++) {
			gsw_w32(cdev,
				(FDMA_PCTRL_EN_OFFSET + (j * 0x6)),
				FDMA_PCTRL_EN_SHIFT,
				FDMA_PCTRL_EN_SIZE, 1);
		}

		/* Enable Mlticast Table Hit Status Update */
		gsw_w32(cdev, PCE_GCTRL_0_MCSTHITEN_OFFSET, PCE_GCTRL_0_MCSTHITEN_SHIFT,
			PCE_GCTRL_0_MCSTHITEN_SIZE, 1);
		/* Enable MAC Table Hit Status Update */
		gsw_w32(cdev, PCE_GCTRL_0_MACHITEN_OFFSET, PCE_GCTRL_0_MACHITEN_SHIFT,
			PCE_GCTRL_0_MACHITEN_SIZE, 1);

		/*PMAC default queue assignment and configuration*/
		gsw_set_def_bypass_qmap(cdev, GSW_QOS_QMAP_SINGLE_MODE);
		gsw_set_def_pce_qmap(cdev);
		gsw_qos_def_config(cdev);
		gsw_misc_config(cdev);
		if(gswdev->dpu)
			gsw_pmac_init_dpu();
		else
			gsw_pmac_init_nondpu();
#else

		/*Emulation/PC tool*/
		/*Enable FDMA/SDMA for all logical ports*/
		for (j = 0; j < gswdev->tpnum; j++) {
			gsw_w32(cdev,
				(FDMA_PCTRL_EN_OFFSET + (j * 0x6)),
				FDMA_PCTRL_EN_SHIFT,
				FDMA_PCTRL_EN_SIZE, 1);
			gsw_w32(cdev,
				(SDMA_PCTRL_PEN_OFFSET + (j * 0x6)),
				SDMA_PCTRL_PEN_SHIFT,
				SDMA_PCTRL_PEN_SIZE, 1);
		}

#if defined(CONFIG_MAC) && CONFIG_MAC
		struct mac_ops *mac_ops;
		u32 max_mac = gsw_get_mac_subifcnt(0);

		for (j = 0; j < max_mac; j++) {
			mac_ops = get_mac_ops(gswdev, j + 2);
			mac_init_fn_ptrs(mac_ops);
			mac_ops->init(mac_ops);
		}

		struct adap_ops *ops = gsw_get_adap_ops(0);

		if (ops == NULL) {
			pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
			return GSW_statusErr;
		}

		gswss_init_fn_ptrs(ops);
#endif

#endif

		/* Enable CTP RMON for all logical port*/
		for (j = 0; j < gswdev->tpnum; j++) {
			gsw_w32(cdev, (BM_PCFG_CNTEN_OFFSET + (j * 2)), BM_PCFG_CNTEN_SHIFT,
				BM_PCFG_CNTEN_SIZE, 1);
		}

		/* Switch Core Enable for Windows mode will be done in GSW_ENABLE */
#if defined(WIN_PC_MODE) && WIN_PC_MODE
#else
		/*Note: Default Config*/
		/*LP0 (fdma/sdma enabled) -> CTP 0 allocated ->Bridge port 0 allocated -> bridge/fid 0 allocated) */
		/*Enable GSWIP 3.1 Switch Core*/
		GSW_UnFreeze();
#endif
		/*Initalize GSWIP Irq*/
		GSW_Irq_init(cdev);
	}

	return GSW_statusOk;
}


/*	This is the switch core layer init function.*/
/*	\param ethcinit This parameter is a pointer */
/*  to the switch core context. */
/*	\return Return value as follows: */
/*	cdev: if successful */
void *ethsw_api_core_init(ethsw_core_init_t *ethcinit)
{
	ethsw_api_dev_t *PrvData;
	struct core_ops *ops;
	void *cdev;
	u32 ret = 0, i = 0, reg_val;

#ifdef __KERNEL__

	/* KERNEL_MODE */
	/** Get Platform Driver Data */
	if ((ethcinit->sdev == LTQ_FLOW_DEV_INT) || (ethcinit->sdev == LTQ_FLOW_DEV_INT_R)) {
		ops = platform_get_drvdata(ethcinit->pdev);

		/** Get Switch Core Private Data */
		PrvData = container_of(ops, ethsw_api_dev_t, ops);

		if (PrvData == NULL) {
			pr_err("%s:%s:%d (Plateform driver data not allocated)\n",
			       __FILE__, __func__, __LINE__);
			return PrvData;
		}

		/** Clear Switch Core Private Data */
	} else {
		/*External switch*/
		PrvData = ethcinit->pdev;
	}

	/** Set Core OPS struct Adress to cdev*/
	cdev = &PrvData->ops;

	/** Store Platform Device struct in Switch Core Private Data */
	PrvData->pdev = ethcinit->pdev;

	spin_lock_init(&PrvData->lock_pce);
	spin_lock_init(&PrvData->lock_bm);
	spin_lock_init(&PrvData->lock_misc);
	spin_lock_init(&PrvData->lock_pmac);
	spin_lock_init(&PrvData->lock_pae);
	spin_lock_init(&PrvData->lock_ctp_map);
	spin_lock_init(&PrvData->lock_irq);
	spin_lock_init(&PrvData->lock_mdio);
	spin_lock_init(&PrvData->lock_mmd);
	spin_lock_init(&PrvData->lock_pce_tbl);

	/* KERNEL_MODE*/
#else
	/* USER SPACE APPLICATION */
	PrvData = (ethsw_api_dev_t *)malloc(sizeof(ethsw_api_dev_t));
	cdev = (void *)PrvData;

	if (!PrvData) {
		pr_err("%s:%s:%d (memory allocation failed)\n",
		       __FILE__, __func__, __LINE__);
		return PrvData;
	}

	memset(PrvData, 0, sizeof(ethsw_api_dev_t));
	/* USER SPACE APPLICATION */
#endif

	PrvData->raldev = ethcinit->ecdev;
	ecoredev[ethcinit->sdev] = PrvData;
	PrvData->sdev = ethcinit->sdev;
	PrvData->rst = 0;
	PrvData->hwinit = 0;
	PrvData->matimer = DEFAULT_AGING_TIMEOUT;
	/** Switch Core Base Address */
	PrvData->gsw_base = ethcinit->gsw_base_addr;

	/** Get Switch Hardware capablity */
	get_gsw_hw_cap(cdev);

	/** Initialize Switch Core Function Pointer */
	ret = gsw_init_fn_ptrs(cdev);

	if (ret ==
	    GSW_statusErr) {
		pr_err("%s:%s:%d (Initialize Switch Core Function Pointer failed)\n",
		       __FILE__, __func__, __LINE__);
		return PrvData;
	}

#if defined(CONFIG_LTQ_MULTICAST) && CONFIG_LTQ_MULTICAST

	if ((ethcinit->sdev == LTQ_FLOW_DEV_INT) || (ethcinit->sdev == LTQ_FLOW_DEV_INT_R)) {
		/*Reset Multicast software table*/
		reset_multicast_sw_table(cdev);
	}

#endif /*CONFIG_LTQ_MULTICAST*/

	/** TFlow Table Init */
	pce_table_init(&PrvData->phandler);

	if (!PrvData->num_of_global_rules) {
		if (IS_VRSN_30_31_32(PrvData->gipver)) {
			gsw_r32(cdev, PCE_TFCR_NUM_NUM_OFFSET,
				PCE_TFCR_NUM_NUM_SHIFT,
				PCE_TFCR_NUM_NUM_SIZE, &PrvData->num_of_global_rules);
			PrvData->num_of_global_rules = PrvData->num_of_global_rules << 2;
		} else if (IS_VRSN_BELOW_30(PrvData->gipver))
			PrvData->num_of_global_rules = PrvData->tftblsize;
	}

	/* Mark the tflow global rule indexes are in  inuse */
	for (i = 0; i < PrvData->num_of_global_rules; i++) {
		PrvData->tflow_idx.flow_idx[i].indexinuse = 1;
		PrvData->tflow_idx.usedentry++;
		PrvData->tflow_idx.flow_idx[i].indexinusagecnt++;
	}

	if (IS_VRSN_31_OR_32(PrvData->gipver)) {
		/* First Entry of Common Region in TFLOW Table */
		gsw_w32(cdev, PCE_TFCR_ID_INDEX_OFFSET, PCE_TFCR_ID_INDEX_SHIFT,
			PCE_TFCR_ID_INDEX_SIZE, 0x0);
		reg_val = PrvData->num_of_global_rules;
		/* Number of Entries of Common Region in TFLOW Table */
		gsw_w32(cdev, PCE_TFCR_NUM_NUM_OFFSET, PCE_TFCR_NUM_NUM_SHIFT,
			PCE_TFCR_NUM_NUM_SIZE, (reg_val >> 2));
	}

	/** Parser Micro Code Init */
#if defined(CONFIG_USE_EMULATOR)
	gsw_w32(cdev, PCE_GCTRL_0_MC_VALID_OFFSET,
		PCE_GCTRL_0_MC_VALID_SHIFT, PCE_GCTRL_0_MC_VALID_SIZE, 0x1);
#else

	if ((ethcinit->sdev == LTQ_FLOW_DEV_INT) ||
	    (ethcinit->sdev == LTQ_FLOW_DEV_INT_R))
		gsw_pmicro_code_init(cdev);
#endif

	if ((ethcinit->sdev == LTQ_FLOW_DEV_INT) || (ethcinit->sdev == LTQ_FLOW_DEV_INT_R)) {
		if (IS_VRSN_31(PrvData->gipver))
			switch_core_init(cdev);
		else
			legacy_switch_core_init(cdev);
	}

#ifdef CONFIG_X86_INTEL_CE2700
	cport_sgmii_config(cdev);
	GSW_Enable(cdev);
#endif /*CONFIG_X86_INTEL_CE2700*/
	return PrvData;
}

/**	This is the switch core layer cleanup function.*/
/*	\return Return value as follows: */
/*	GSW_statusOk: if successful */
void gsw_corecleanup(void)
{
	u8 i;
	ethsw_api_dev_t *cdev;

	for (i = 0; i < LTQ_FLOW_DEV_MAX; i++) {
		cdev = (ethsw_api_dev_t *)ecoredev[i];

		if (cdev) {
			GSW_Irq_deinit(cdev);
#ifdef __KERNEL__
			kfree(cdev);
#else
			free(cdev);
#endif
			cdev = NULL;
		}
	}
}

GSW_return_t GSW_MAC_TableClear(void *cdev)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_pce);
#endif

	/*  Flush all entries from the MAC table */
	gsw_w32(cdev, PCE_GCTRL_0_MTFL_OFFSET,
		PCE_GCTRL_0_MTFL_SHIFT,
		PCE_GCTRL_0_MTFL_SIZE, 1);
	ret = GSW_statusOk;

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_pce);
#endif
	return ret;
}


static int conv_id(unsigned short val)
{
	static int templ[16] = {
		-1,  0,  1, -2,
		2, -2, -2, -2,
		3, -2, -2, -2,
		-2, -2, -2, -2
	};

	int result, r;
	int i;

	result = -1;

	for (i = 0; val != 0 && i < 4; i++, val >>= 4) {
		if ((val & 0x0F) == 0)
			continue;

		r = templ[val & 0x0F];

		if (r < 0 || result >= 0)
			return -1;

		result = r + i * 4;
	}

	return result;
}

static int conv_id_array(unsigned short *pval, unsigned int size)
{
	int result, r;
	unsigned int i;

	result = -1;

	for (i = 0; i < size; i++) {
		if (pval[i] == 0)
			continue;

		r = conv_id(pval[i]);

		if (r < 0 || result >= 0)
			return -1;

		result = r + i * 16;
	}

	return result;
}

GSW_return_t GSW_MAC_TableEntryAdd(void *cdev,
				   GSW_MAC_tableAdd_t *parm)
{
	pctbl_prog_t tbl_prog;
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 ret = 0;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_pce);
#endif

	if (gswdev->gipver == LTQ_GSWIP_3_0) {
		if ((parm->nPortId >= gswdev->tpnum) &&
		    (!(parm->nPortId & 0x80000000))) {
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}
	}

	memset(&tbl_prog, 0, sizeof(pctbl_prog_t));
	tbl_prog.table = PCE_MAC_BRIDGE_INDEX;
	tbl_prog.key[0] = parm->nMAC[4] << 8 | parm->nMAC[5];
	tbl_prog.key[1] = parm->nMAC[2] << 8 | parm->nMAC[3];
	tbl_prog.key[2] = parm->nMAC[0] << 8 | parm->nMAC[1];
	tbl_prog.key[3] = parm->nFId;

	if (gswdev->gipver == LTQ_GSWIP_2_2_ETC)
		tbl_prog.val[1] = ((parm->nSubIfId & 0xFFF) << 4);

	/*	tbl_prog.val[1] = ((parm->nSVLAN_Id & 0xFFF) << 4); */
	if (IS_VRSN_30_31(gswdev->gipver))
		tbl_prog.val[1] = ((parm->nSubIfId & 0x1FFF) << 3);

	/*To configure PCE_TBL_CTRL VLD 12th BIT*/
	tbl_prog.valid = 1;
	tbl_prog.val[1] |= (1 << 1); /* Valid Entry */

	if (IS_VRSN_31(gswdev->gipver)) {
		/*Enable source address filter*/
		if (parm->nFilterFlag & 0x1)
			tbl_prog.key[3] |= (1 << 8);

		/*Enable destination address filter*/
		if (parm->nFilterFlag & 0x2)
			tbl_prog.key[3] |= (1 << 9);
	}

	if (parm->bStaticEntry) { /*static entry*/
		/*Enable static entry flag */
		tbl_prog.val[1] |= 1;

		if (IS_VRSN_31(gswdev->gipver)) {
			/*set igmp control flag*/
			if (parm->bIgmpControlled) {
				tbl_prog.val[0] |= (1 << 2);
				//Govind - Is the DMAC matched will go to below Port-Map or?
			}

			/*Set Bridge Port Map*/
			if (parm->nPortId & 0x80000000) {
				tbl_prog.val[2] = parm->nPortMap[0];
				tbl_prog.val[3] = parm->nPortMap[1];
				tbl_prog.val[4] = parm->nPortMap[2];
				tbl_prog.val[5] = parm->nPortMap[3];
				tbl_prog.val[6] = parm->nPortMap[4];
				tbl_prog.val[7] = parm->nPortMap[5];
				tbl_prog.val[8] = parm->nPortMap[6];
				tbl_prog.val[9] = parm->nPortMap[7];
				tbl_prog.val[10] = parm->nPortMap[8];
				tbl_prog.val[11] = parm->nPortMap[9];
				tbl_prog.val[12] = parm->nPortMap[10];
				tbl_prog.val[13] = parm->nPortMap[11];
				tbl_prog.val[14] = parm->nPortMap[12];
				tbl_prog.val[15] = parm->nPortMap[13];
				tbl_prog.val[16] = parm->nPortMap[14];
				tbl_prog.val[17] = parm->nPortMap[15];
			} else {
				u32 i, j;

				i = parm->nPortId / (sizeof(tbl_prog.val[0]) * 8);
				j = parm->nPortId & (sizeof(tbl_prog.val[0]) * 8 - 1);
				tbl_prog.val[i + 2] = 1 << j;
			}
		} else {
			if (parm->nPortId & 0x80000000) { /*Port Map 3.0 */
				if (parm->nPortMap[0])
					tbl_prog.val[0] = parm->nPortMap[0];
				else
					tbl_prog.val[0] = (parm->nPortId & 0x7FFF);
			} else {
				tbl_prog.val[0] = (1 << parm->nPortId);
			}
		}

	} else { /*Dynamic Entry*/
		if (IS_VRSN_31(gswdev->gipver)) {
			tbl_prog.val[0] = (parm->nAgeTimer & 0xF);
			/*Set Bridge Port Member val 0 to 7 bits*/
			tbl_prog.val[2] = (parm->nPortId & 0xFF);
		} else {
			tbl_prog.val[0] = (((parm->nPortId & 0xF) << 4)
					   | (parm->nAgeTimer & 0xF));
		}
	}

	gsw_pce_table_key_write(cdev, &tbl_prog);

	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_pce);
#endif
	return ret;

}

GSW_return_t GSW_MAC_TableEntryRead(void *cdev,
				    GSW_MAC_tableRead_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	pctbl_prog_t tbl_prog;
	u32 ret = 0;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_pce);
#endif

	memset(&tbl_prog, 0, sizeof(pctbl_prog_t));

	if (parm->bInitial == 1) {
		gswdev->mac_rd_index = 0; /*Start from the index 0 */
		parm->bInitial = 0;
	}

	if (gswdev->mac_rd_index >= gswdev->mactblsize) {
		memset(parm, 0, sizeof(GSW_MAC_tableRead_t));
		parm->bLast = 1;
		gswdev->mac_rd_index = 0;
		ret = GSW_statusOk;
		goto UNLOCK_AND_RETURN;
	}

	tbl_prog.table = PCE_MAC_BRIDGE_INDEX;

	do {
		tbl_prog.pcindex = gswdev->mac_rd_index;
		gsw_pce_table_read(cdev, &tbl_prog);
		gswdev->mac_rd_index++;

		if (tbl_prog.valid != 0)
			break;
	} while (gswdev->mac_rd_index < gswdev->mactblsize);

	if (tbl_prog.valid == 1) {
		/*get FID*/
		parm->nFId = tbl_prog.key[3] & 0x3F;

		if (IS_VRSN_31(gswdev->gipver)) {
			/*source address filter flag*/
			if (tbl_prog.key[3] & 0x100)
				parm->nFilterFlag = parm->nFilterFlag | 0x1;

			/*destination address filter flag*/
			if (tbl_prog.key[3] & 0x200)
				parm->nFilterFlag = parm->nFilterFlag | 0x2;
		}

		/*static flag*/
		parm->bStaticEntry = (tbl_prog.val[1] & 0x1);

		if (parm->bStaticEntry == 1) { /*Static Entry*/
			parm->nAgeTimer = 0;

			if (IS_VRSN_31(gswdev->gipver)) {
				int id;

				/*get igmp control flag*/
				if (tbl_prog.val[0] & 0x4) {
					parm->bIgmpControlled = 1;
				}

				/*Get Bridge Port Map*/
				parm->nPortMap[0] = tbl_prog.val[2];
				parm->nPortMap[1] = tbl_prog.val[3];
				parm->nPortMap[2] = tbl_prog.val[4];
				parm->nPortMap[3] = tbl_prog.val[5];
				parm->nPortMap[4] = tbl_prog.val[6];
				parm->nPortMap[5] = tbl_prog.val[7];
				parm->nPortMap[6] = tbl_prog.val[8];
				parm->nPortMap[7] = tbl_prog.val[9];
				parm->nPortMap[8] = tbl_prog.val[10];
				parm->nPortMap[9] = tbl_prog.val[11];
				parm->nPortMap[10] = tbl_prog.val[12];
				parm->nPortMap[11] = tbl_prog.val[13];
				parm->nPortMap[12] = tbl_prog.val[14];
				parm->nPortMap[13] = tbl_prog.val[15];
				parm->nPortMap[14] = tbl_prog.val[16];
				parm->nPortMap[15] = tbl_prog.val[17];


				id = conv_id_array(&tbl_prog.val[2], 16);

				if (id < 0)
					parm->nPortId = 0x80000000;
				else
					parm->nPortId = (u32)id;

				if ((parm->nPortMap[gswdev->hitstatus_idx]
				     & gswdev->hitstatus_mask)) {
					parm->hitstatus = LTQ_TRUE;

					tbl_prog.val[gswdev->hitstatus_idx + 2] &=
						~gswdev->hitstatus_mask;
					gsw_pce_table_key_write(cdev, &tbl_prog);
				} else
					parm->hitstatus = LTQ_FALSE;
			} else {

				int id = conv_id(tbl_prog.val[0]);

				if (id < 0)
					parm->nPortId = (u32)tbl_prog.val[0] | 0x80000000;
				else
					parm->nPortId = (u32)id;
			}

		} else { /*Dynamic Entry*/
			u32 mant, timer = 300;
			/* Aging Counter Mantissa Value */
			gsw_r32(cdev, PCE_AGE_1_MANT_OFFSET,
				PCE_AGE_1_MANT_SHIFT,
				PCE_AGE_1_MANT_SIZE, &mant);

			switch (mant) {
			case AGETIMER_1_DAY:
				timer = 86400;
				break;

			case AGETIMER_1_HOUR:
				timer = 3600;
				break;

			case AGETIMER_300_SEC:
				timer = 300;
				break;

			case AGETIMER_10_SEC:
				timer = 10;
				break;

			case AGETIMER_1_SEC:
				timer = 1;
				break;
			}

			parm->nAgeTimer =	tbl_prog.val[0] & 0xF;
			parm->nAgeTimer =	(timer * parm->nAgeTimer) / 0xF;

			if (IS_VRSN_31(gswdev->gipver)) {
				/*Set Bridge Port Member val 0 to 7 bits*/
				parm->nPortId = (tbl_prog.val[2] & 0xFF);
				/*Changed
					0: the entry is not changed
					1: the entry is changed and not accessed yet */
				parm->bEntryChanged = ((tbl_prog.val[0] >> 8) & 0x1);
			} else {
				parm->nPortId =	(tbl_prog.val[0] >> 4) & 0xF;
			}
		}

		parm->nMAC[0] = tbl_prog.key[2] >> 8;
		parm->nMAC[1] = tbl_prog.key[2] & 0xFF;
		parm->nMAC[2] = tbl_prog.key[1] >> 8;
		parm->nMAC[3] = tbl_prog.key[1] & 0xFF;
		parm->nMAC[4] = tbl_prog.key[0] >> 8;
		parm->nMAC[5] = tbl_prog.key[0] & 0xFF;

		if (gswdev->gipver == LTQ_GSWIP_2_2_ETC)
			parm->nSubIfId = (tbl_prog.val[1] >> 4 & 0xFFF);

		/*	parm->nSVLAN_Id = (tbl_prog.val[1] >> 4 & 0xFFF); */
		if (IS_VRSN_30_31(gswdev->gipver))
			parm->nSubIfId = (tbl_prog.val[1] >> 3 & 0x1FFF);

		parm->bInitial = 0;
		parm->bLast = 0;
	} else {
		memset(parm, 0, sizeof(GSW_MAC_tableRead_t));
		parm->bLast = 1;
	}

	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_pce);
#endif
	return ret;
}

GSW_return_t GSW_MAC_TableEntryQuery(void *cdev,
				     GSW_MAC_tableQuery_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	pctbl_prog_t tbl_prog;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_pce);
#endif

	parm->bFound = 0;
	memset(&tbl_prog, 0, sizeof(pctbl_prog_t));
	tbl_prog.table = PCE_MAC_BRIDGE_INDEX;
	tbl_prog.key[0] = parm->nMAC[4] << 8 | parm->nMAC[5];
	tbl_prog.key[1] = parm->nMAC[2] << 8 | parm->nMAC[3];
	tbl_prog.key[2] = parm->nMAC[0] << 8 | parm->nMAC[1];
	tbl_prog.key[3] = parm->nFId;

	if (IS_VRSN_31(gswdev->gipver)) {
		/*source address filter key*/
		if (parm->nFilterFlag & 0x1) {
			tbl_prog.key[3] |= (1 << 8);
		}

		/*destination address filter key*/
		if (parm->nFilterFlag & 0x2) {
			tbl_prog.key[3] |= (1 << 9);
		}
	}

	gsw_pce_table_key_read(cdev, &tbl_prog);

	if (tbl_prog.valid == 1) {
		parm->bFound = 1;
		parm->bStaticEntry = (tbl_prog.val[1] & 0x1);
		parm->nSubIfId = ((tbl_prog.val[1] >> 3) & 0x1FFF);

		if (parm->bStaticEntry == 1) {
			parm->nAgeTimer = 0;

			if (IS_VRSN_31(gswdev->gipver)) {
				int id;

				/*get igmp control flag*/
				if (tbl_prog.val[0] & 0x4) {
					parm->bIgmpControlled = 1;
				}

				/*Get Bridge Port Map*/
				parm->nPortMap[0] = tbl_prog.val[2];
				parm->nPortMap[1] = tbl_prog.val[3];
				parm->nPortMap[2] = tbl_prog.val[4];
				parm->nPortMap[3] = tbl_prog.val[5];
				parm->nPortMap[4] = tbl_prog.val[6];
				parm->nPortMap[5] = tbl_prog.val[7];
				parm->nPortMap[6] = tbl_prog.val[8];
				parm->nPortMap[7] = tbl_prog.val[9];
				parm->nPortMap[8] = tbl_prog.val[10];
				parm->nPortMap[9] = tbl_prog.val[11];
				parm->nPortMap[10] = tbl_prog.val[12];
				parm->nPortMap[11] = tbl_prog.val[13];
				parm->nPortMap[12] = tbl_prog.val[14];
				parm->nPortMap[13] = tbl_prog.val[15];
				parm->nPortMap[14] = tbl_prog.val[16];
				parm->nPortMap[15] = tbl_prog.val[17];

				id = conv_id_array(&tbl_prog.val[2], 16);

				if (id < 0)
					parm->nPortId = 0x80000000;
				else
					parm->nPortId = (u32)id;

				if ((parm->nPortMap[gswdev->hitstatus_idx]
				     & gswdev->hitstatus_mask)) {
					parm->hitstatus = LTQ_TRUE;

					tbl_prog.val[gswdev->hitstatus_idx + 2] &=
						~gswdev->hitstatus_mask;
					gsw_pce_table_key_write(cdev, &tbl_prog);
				} else
					parm->hitstatus = LTQ_FALSE;
			} else {
				int id = conv_id(tbl_prog.val[0]);

				if (id < 0)
					parm->nPortId = (u32)tbl_prog.val[0] | 0x80000000;
				else
					parm->nPortId = (u32)id;
			}
		} else {
			u32 mant, timer = 300;
			/* Aging Counter Mantissa Value */
			gsw_r32(cdev, PCE_AGE_1_MANT_OFFSET,
				PCE_AGE_1_MANT_SHIFT,
				PCE_AGE_1_MANT_SIZE, &mant);

			switch (mant) {
			case AGETIMER_1_DAY:
				timer = 86400;
				break;

			case AGETIMER_1_HOUR:
				timer = 3600;
				break;

			case AGETIMER_300_SEC:
				timer = 300;
				break;

			case AGETIMER_10_SEC:
				timer = 10;
				break;

			case AGETIMER_1_SEC:
				timer = 1;
				break;
			}

			parm->nAgeTimer = tbl_prog.val[0] & 0xF;
			parm->nAgeTimer = (timer * parm->nAgeTimer) / 0xF;

			if (IS_VRSN_31(gswdev->gipver)) {
				/*Set Bridge Port Member val 0 to 7 bits*/
				parm->nPortId = (tbl_prog.val[2] & 0xFF);

				/*Changed
					0: the entry is not changed
					1: the entry is changed and not accessed yet */
				parm->bEntryChanged = ((tbl_prog.val[0] >> 8) & 0x1);
			} else {
				parm->nPortId =	(tbl_prog.val[0] >> 4) & 0xF;
			}
		}
	}

	ret = GSW_statusOk;

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_pce);
#endif
	return ret;
}

GSW_return_t GSW_MAC_TableEntryRemove(void *cdev,
				      GSW_MAC_tableRemove_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	pctbl_prog_t tbl_prog;
	u32 value;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_pce);
#endif

	/* MAC table flushing control bit */
	gsw_r32(cdev, PCE_GCTRL_0_MTFL_OFFSET,
		PCE_GCTRL_0_MTFL_SHIFT,
		PCE_GCTRL_0_MTFL_SIZE, &value);

	/* If value is 1, flush all entries from the MAC table in process */
	if (!value) {
		memset(&tbl_prog, 0, sizeof(pctbl_prog_t));
		tbl_prog.table = PCE_MAC_BRIDGE_INDEX;
		tbl_prog.key[0] = parm->nMAC[4] << 8 | parm->nMAC[5];
		tbl_prog.key[1] = parm->nMAC[2] << 8 | parm->nMAC[3];
		tbl_prog.key[2] = parm->nMAC[0] << 8 | parm->nMAC[1];
		tbl_prog.key[3] = parm->nFId;

		if (IS_VRSN_31(gswdev->gipver)) {
			/*source address filter key*/
			if (parm->nFilterFlag & 0x1) {
				tbl_prog.key[3] |= (1 << 8);
			}

			/*destination address filter key*/
			if (parm->nFilterFlag & 0x2) {
				tbl_prog.key[3] |= (1 << 9);
			}
		}

		gsw_pce_table_key_read(cdev, &tbl_prog);

		if (tbl_prog.valid == 1) {
			pctbl_prog_t tbl_cl_prog;
			memset(&tbl_cl_prog, 0, sizeof(pctbl_prog_t));
			tbl_cl_prog.table = PCE_MAC_BRIDGE_INDEX;
			tbl_cl_prog.pcindex = tbl_prog.pcindex;
			gsw_pce_table_write(cdev, &tbl_cl_prog);
		}
	}

	ret = GSW_statusOk;

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_pce);
#endif
	return ret;
}

GSW_return_t GSW_PortCfgGet(void *cdev, GSW_portCfg_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u8 pidx = parm->nPortId;
	u32 value, monrx = 0, montx = 0, PEN, EN;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_misc);
#endif

	if (IS_VRSN_NOT_31(gswdev->gipver)) {
		if (parm->nPortId >= gswdev->tpnum) {
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}
	}

	/* See if PORT enable or not */
	gsw_r32(cdev, (SDMA_PCTRL_PEN_OFFSET + (0x6 * pidx)),
		SDMA_PCTRL_PEN_SHIFT, SDMA_PCTRL_PEN_SIZE, &PEN);
	gsw_r32(cdev, (FDMA_PCTRL_EN_OFFSET + (0x6 * pidx)),
		FDMA_PCTRL_EN_SHIFT, FDMA_PCTRL_EN_SIZE, &EN);

	/* Port Enable feature only support 6 port */
	if (pidx >= gswdev->pnum) {
		parm->eEnable = 1;
	} else {
		if ((PEN == 1) && (EN == 1))
			parm->eEnable = GSW_PORT_ENABLE_RXTX;
		else if ((PEN == 1) && (EN == 0))
			parm->eEnable = GSW_PORT_ENABLE_RX;
		else if ((PEN == 0) && (EN == 1))
			parm->eEnable = GSW_PORT_ENABLE_TX;
		else
			parm->eEnable = GSW_PORT_DISABLE;
	}

	/* Learning Limit */
	gsw_r32(cdev, (PCE_PCTRL_1_LRNLIM_OFFSET + (0xA * pidx)),
		PCE_PCTRL_1_LRNLIM_SHIFT,
		PCE_PCTRL_1_LRNLIM_SIZE, &value);
	parm->nLearningLimit = value;

	/* Learning Limit Port Lock */
	gsw_r32(cdev, (PCE_PCTRL_0_PLOCK_OFFSET + (0xA * pidx)),
		PCE_PCTRL_0_PLOCK_SHIFT,
		PCE_PCTRL_0_PLOCK_SIZE, &value);
	parm->bLearningMAC_PortLock = value;
	/* Aging */
	gsw_r32(cdev, PCE_PCTRL_0_AGEDIS_OFFSET + (0xA * pidx),
		PCE_PCTRL_0_AGEDIS_SHIFT,
		PCE_PCTRL_0_AGEDIS_SIZE, &value);
	parm->bAging = value;

	if ((gswdev->gipver == LTQ_GSWIP_2_2) ||
	    (gswdev->gipver == LTQ_GSWIP_2_2_ETC) ||
	    (gswdev->gipver == LTQ_GSWIP_3_0)) {
		/** MAC address table learning on the port specified. */
		gsw_r32(cdev, (PCE_PCTRL_3_LNDIS_OFFSET + (0xA * pidx)),
			PCE_PCTRL_3_LNDIS_SHIFT,
			PCE_PCTRL_3_LNDIS_SIZE, &parm->bLearning);
		/** MAC spoofing detection. */
		gsw_r32(cdev, (PCE_PCTRL_0_SPFDIS_OFFSET + (0xA * pidx)),
			PCE_PCTRL_0_SPFDIS_SHIFT,
			PCE_PCTRL_0_SPFDIS_SIZE, &parm->bMAC_SpoofingDetection);
	}

	/* UnicastUnknownDrop */
	gsw_r32(cdev, PCE_PMAP_3_UUCMAP_OFFSET,
		PCE_PMAP_3_UUCMAP_SHIFT,
		PCE_PMAP_3_UUCMAP_SIZE, &value);

	/* UnicastUnknownDrop feature  support  */
	if ((value & (1 << pidx)) == 0)
		parm->bUnicastUnknownDrop = 1;
	else
		parm->bUnicastUnknownDrop = 0;

	/* MulticastUnknownDrop */
	gsw_r32(cdev, PCE_PMAP_2_DMCPMAP_OFFSET,
		PCE_PMAP_2_DMCPMAP_SHIFT,
		PCE_PMAP_2_DMCPMAP_SIZE, &value);

	/* MulticastUnknownDrop feature  support  */
	if ((value & (1 << pidx)) == 0) {
		parm->bMulticastUnknownDrop = 1;
		parm->bBroadcastDrop = 1;
	} else {
		parm->bMulticastUnknownDrop = 0;
		parm->bBroadcastDrop = 0;
	}

	/* Require to check later - 3M */
	parm->bReservedPacketDrop = 0;
	/* Port Monitor */
	gsw_r32(cdev, (PCE_PCTRL_3_RXVMIR_OFFSET + (0xA * pidx)),
		PCE_PCTRL_3_RXVMIR_SHIFT,
		PCE_PCTRL_3_RXVMIR_SIZE, &monrx);
	gsw_r32(cdev, (PCE_PCTRL_3_TXMIR_OFFSET + (0xA * pidx)),
		PCE_PCTRL_3_TXMIR_SHIFT,
		PCE_PCTRL_3_TXMIR_SIZE, &montx);

	if ((monrx == 1) && (montx == 1))
		parm->ePortMonitor = GSW_PORT_MONITOR_RXTX;
	else if ((monrx == 1) && (montx == 0))
		parm->ePortMonitor = GSW_PORT_MONITOR_RX;
	else if ((monrx == 0) && (montx == 1))
		parm->ePortMonitor = GSW_PORT_MONITOR_TX;
	else
		parm->ePortMonitor = GSW_PORT_MONITOR_NONE;

	gsw_r32(cdev, (PCE_PCTRL_3_VIO_2_OFFSET + (0xA * pidx)),
		PCE_PCTRL_3_VIO_2_SHIFT,
		PCE_PCTRL_3_VIO_2_SIZE, &monrx);

	if (monrx == 1)
		parm->ePortMonitor |= GSW_PORT_MONITOR_VLAN_UNKNOWN;

	gsw_r32(cdev, (PCE_PCTRL_3_VIO_4_OFFSET + (0xA * pidx)),
		PCE_PCTRL_3_VIO_4_SHIFT,
		PCE_PCTRL_3_VIO_4_SIZE, &monrx);

	if (monrx == 1)
		parm->ePortMonitor |= GSW_PORT_MONITOR_VLAN_MEMBERSHIP;

	gsw_r32(cdev, (PCE_PCTRL_3_VIO_5_OFFSET + (0xA * pidx)),
		PCE_PCTRL_3_VIO_5_SHIFT,
		PCE_PCTRL_3_VIO_5_SIZE, &monrx);

	if (monrx == 1)
		parm->ePortMonitor |= GSW_PORT_MONITOR_PORT_STATE;

	gsw_r32(cdev, (PCE_PCTRL_3_VIO_6_OFFSET + (0xA * pidx)),
		PCE_PCTRL_3_VIO_6_SHIFT,
		PCE_PCTRL_3_VIO_6_SIZE, &monrx);

	if (monrx == 1)
		parm->ePortMonitor |= GSW_PORT_MONITOR_LEARNING_LIMIT;

	gsw_r32(cdev, (PCE_PCTRL_3_VIO_7_OFFSET + (0xA * pidx)),
		PCE_PCTRL_3_VIO_7_SHIFT,
		PCE_PCTRL_3_VIO_7_SIZE, &monrx);

	if (monrx == 1)
		parm->ePortMonitor |= GSW_PORT_MONITOR_PORT_LOCK;

	if (gswdev->gipver == LTQ_GSWIP_3_0) {
		gsw_r32(cdev, (BM_RMON_CTRL_IFRMONFST_OFFSET + (0x2 * pidx)),
			BM_RMON_CTRL_IFRMONFST_SHIFT,
			BM_RMON_CTRL_IFRMONFST_SIZE, &parm->nIfCountStartIdx);

		if (parm->nIfCountStartIdx)
			parm->bIfCounters = 1;

		gsw_r32(cdev, (BM_RMON_CTRL_IFRMONMD_OFFSET + (0x2 * pidx)),
			BM_RMON_CTRL_IFRMONMD_SHIFT,
			BM_RMON_CTRL_IFRMONMD_SIZE, &parm->eIfRMONmode);

		if (gswdev->sdev == LTQ_FLOW_DEV_INT_R) {
			if (parm->nPortId == 15) {
				/*				pr_err("%s:%s:%d",__FILE__, __func__, __LINE__);*/
				gsw_r32(cdev, MAC_PSTAT_TXPAUEN_OFFSET,
					MAC_PSTAT_TXPAUEN_SHIFT, 2, &value);
			}
		} else {
			if ((parm->nPortId |= 0) && (parm->nPortId < (gswdev->pnum - 1))) {
				gsw_r32(cdev, (MAC_PSTAT_TXPAUEN_OFFSET + (0xC * (parm->nPortId - 1))),
					MAC_PSTAT_TXPAUEN_SHIFT, 2, &value);
			}
		}
	} else {
		gsw_r32(cdev, (MAC_PSTAT_TXPAUEN_OFFSET + (0xC * pidx)),
			MAC_PSTAT_TXPAUEN_SHIFT, 2, &value);
	}

	if (IS_VRSN_31(gswdev->gipver)) {
#if defined(CONFIG_MAC) && CONFIG_MAC

		/* MAC API's to get flow control */
		if (parm->ePortType == GSW_PHYSICAL_PORT ||
		    parm->ePortType == GSW_LOGICAL_PORT) {
			struct mac_ops *ops = get_mac_ops(gswdev, parm->nPortId);

			if (!ops) {
				pr_err("MAC %d is not initialized\n", parm->nPortId);
				ret = GSW_statusErr;
				goto UNLOCK_AND_RETURN;
			}

			parm->eFlowCtrl = ops->get_flow_ctl(ops);
		} else
			pr_err("MAC configuration is not valid for CTP or Bridge Port %d \n", parm->nPortId);

#endif
	} else {
		switch (value) {
		case 0:
			parm->eFlowCtrl = GSW_FLOW_OFF;
			break;

		case 1:
			parm->eFlowCtrl = GSW_FLOW_TX;
			break;

		case 3:
			parm->eFlowCtrl = GSW_FLOW_RXTX;
			break;

		case 2:
			parm->eFlowCtrl = GSW_FLOW_RX;
			break;

		default:
			parm->eFlowCtrl = GSW_FLOW_AUTO;
		}
	}

	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_misc);
#endif
	return ret;

}

GSW_return_t GSW_PortCfgSet(void *cdev, GSW_portCfg_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u8 pidx = parm->nPortId;
	u32 paddr;
	u32 value, EN, PEN, PACT;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_misc);
#endif

	if (IS_VRSN_NOT_31(gswdev->gipver)) {
		if (parm->nPortId >= gswdev->tpnum) {
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}

		/* Port locking flag  */
		gsw_w32(cdev, (PCE_PCTRL_0_PLOCK_OFFSET + (0xA * pidx)),
			PCE_PCTRL_0_PLOCK_SHIFT,
			PCE_PCTRL_0_PLOCK_SIZE, parm->bLearningMAC_PortLock);
	}

	/* Learning Limit Action */
	if (parm->nLearningLimit == 0xFFFF)
		value = 0xFF;
	else
		value = parm->nLearningLimit;

	gswdev->pconfig[parm->nPortId].llimit = value;
	/* Learning Limit */
	gsw_w32(cdev, (PCE_PCTRL_1_LRNLIM_OFFSET + (0xA * pidx)),
		PCE_PCTRL_1_LRNLIM_SHIFT,
		PCE_PCTRL_1_LRNLIM_SIZE, value);

	if ((gswdev->gipver == LTQ_GSWIP_2_2) ||
	    (gswdev->gipver == LTQ_GSWIP_2_2_ETC) ||
	    (gswdev->gipver == LTQ_GSWIP_3_0)) {
		/** MAC address table learning on the port specified */
		gsw_w32(cdev, (PCE_PCTRL_3_LNDIS_OFFSET + (0xA * pidx)),
			PCE_PCTRL_3_LNDIS_SHIFT,
			PCE_PCTRL_3_LNDIS_SIZE, parm->bLearning);
		/** MAC spoofing detection. */
		gsw_w32(cdev, (PCE_PCTRL_0_SPFDIS_OFFSET + (0xA * pidx)),
			PCE_PCTRL_0_SPFDIS_SHIFT,
			PCE_PCTRL_0_SPFDIS_SIZE, parm->bMAC_SpoofingDetection);
	}

	/* Aging */ //Govind- aging dis/en is not in 3.1?
	gsw_w32(cdev, PCE_PCTRL_0_AGEDIS_OFFSET + (0xA * pidx),
		PCE_PCTRL_0_AGEDIS_SHIFT,
		PCE_PCTRL_0_AGEDIS_SIZE, parm->bAging);

	if (IS_VRSN_NOT_31(gswdev->gipver)) {
		/* UnicastUnknownDrop Read first */
		gsw_r32(cdev, PCE_PMAP_3_UUCMAP_OFFSET,
			PCE_PMAP_3_UUCMAP_SHIFT,
			PCE_PMAP_3_UUCMAP_SIZE, &value);

		if (parm->bUnicastUnknownDrop == 1)
			value &= ~(1 << pidx);
		else
			value |= 1 << pidx;

		/* UnicastUnknownDrop write back */
		gsw_w32(cdev, PCE_PMAP_3_UUCMAP_OFFSET,
			PCE_PMAP_3_UUCMAP_SHIFT,
			PCE_PMAP_3_UUCMAP_SIZE, value);

		/* MulticastUnknownDrop */
		gsw_r32(cdev, PCE_PMAP_2_DMCPMAP_OFFSET,
			PCE_PMAP_2_DMCPMAP_SHIFT,
			PCE_PMAP_2_DMCPMAP_SIZE, &value);

		if (parm->bMulticastUnknownDrop == 1)
			value &= ~(1 << pidx);
		else
			value |= 1 << pidx;

		/* MulticastUnknownDrop */
		gsw_w32(cdev, PCE_PMAP_2_DMCPMAP_OFFSET,
			PCE_PMAP_2_DMCPMAP_SHIFT,
			PCE_PMAP_2_DMCPMAP_SIZE, value);
	}

	if (IS_VRSN_NOT_31(gswdev->gipver)) {
		/* Flow Control */
		if (gswdev->gipver == LTQ_GSWIP_3_0) {
			if ((pidx == 0 /*GSW_3X_SOC_CPU_PORT*/)) {
				PEN = 0;
				PACT = 0;
			} else {
				if (gswdev->sdev == LTQ_FLOW_DEV_INT_R) {
					if (parm->nPortId == 15) {
						gsw_r32(cdev, (GSWT_MDCCFG_0_PEN_1_OFFSET + GSW30_TOP_OFFSET),
							(GSWT_MDCCFG_0_PEN_1_SHIFT), GSWT_MDCCFG_0_PEN_1_SIZE, &PEN);
						gsw_r32(cdev, (GSWT_MDIO_STAT_1_PACT_OFFSET + GSW30_TOP_OFFSET),
							GSWT_MDIO_STAT_1_PACT_SHIFT, GSWT_MDIO_STAT_1_PACT_SIZE, &PACT);
						gsw_r32(cdev, (GSWT_PHY_ADDR_1_ADDR_OFFSET  + GSW30_TOP_OFFSET),
							GSWT_PHY_ADDR_1_ADDR_SHIFT, GSWT_PHY_ADDR_1_ADDR_SIZE, &paddr);
					} else {
						PEN = 0;
						PACT = 0;
					}
				} else {
					if (pidx < gswdev->pnum) {
						gsw_r32(cdev, (GSWT_MDCCFG_0_PEN_0_OFFSET + GSW30_TOP_OFFSET),
							(GSWT_MDCCFG_0_PEN_0_SHIFT + pidx),	GSWT_MDCCFG_0_PEN_0_SIZE, &PEN);
						gsw_r32(cdev, ((GSWT_MDIO_STAT_1_PACT_OFFSET + ((pidx - 1) * 4)) + GSW30_TOP_OFFSET),
							GSWT_MDIO_STAT_1_PACT_SHIFT, GSWT_MDIO_STAT_1_PACT_SIZE, &PACT);
						gsw_r32(cdev, ((GSWT_PHY_ADDR_1_ADDR_OFFSET + ((parm->nPortId - 1) * 4)) + GSW30_TOP_OFFSET),
							GSWT_PHY_ADDR_1_ADDR_SHIFT, GSWT_PHY_ADDR_1_ADDR_SIZE, &paddr);
					} else {
						PEN = 0;
						PACT = 0;
					}
				}
			}
		} else {
			if (pidx < gswdev->pnum) {
				gsw_r32(cdev, (MDC_CFG_0_PEN_0_OFFSET + GSW_TREG_OFFSET),
					(MDC_CFG_0_PEN_0_SHIFT + pidx), MDC_CFG_0_PEN_0_SIZE, &PEN);
				gsw_r32(cdev, (MDIO_STAT_0_PACT_OFFSET + GSW_TREG_OFFSET + pidx),
					MDIO_STAT_0_PACT_SHIFT, MDIO_STAT_0_PACT_SIZE, &PACT);
				gsw_r32(cdev, ((PHY_ADDR_0_ADDR_OFFSET - pidx) + GSW_TREG_OFFSET),
					PHY_ADDR_0_ADDR_SHIFT, PHY_ADDR_0_ADDR_SIZE, &paddr);
			} else {
				PEN = 0;
				PACT = 0;
			}
		}
	} else {
		PEN = 0;
		PACT = 0;
	}

	/* PHY polling statemachine (of the MAC) is activated and */
	/* an external PHY reacts on the MDIO accesses. */
	/* Therefore update the MDIO register of the attached PHY.*/
	if (IS_VRSN_NOT_31(gswdev->gipver)) {
		if ((PEN == 1) && (PACT == 1)) {
			GSW_MDIO_data_t mddata;
			/* Write directly to MDIO register */
			mddata.nAddressDev = paddr;
			mddata.nAddressReg = 0x4;
			GSW_MDIO_DataRead(cdev, &mddata);
			mddata.nData &= ~(0xC00);

			switch (parm->eFlowCtrl) {
			case GSW_FLOW_OFF:
				break;

			case GSW_FLOW_TX:
				mddata.nData |= 0x800;
				break;

			case GSW_FLOW_RXTX:
				mddata.nData |= 0x400;
				break;

			case GSW_FLOW_RX:
			case GSW_FLOW_AUTO:
				mddata.nData |= 0xC00;
				break;
			}

			GSW_MDIO_DataWrite(cdev, &mddata);
			/* Restart Auto negotiation */
			mddata.nAddressReg = 0x0;
			GSW_MDIO_DataRead(cdev, &mddata);
			mddata.nData |= 0x1200;
			GSW_MDIO_DataWrite(cdev, &mddata);
		} else {
			u32 RX = 0, TX = 0;

			switch (parm->eFlowCtrl) {
			case GSW_FLOW_OFF:
				RX = 3;
				TX = 3;
				break;

			case GSW_FLOW_TX:
				RX = 3;
				TX = 2;
				break;

			case GSW_FLOW_RXTX:
				RX = 2;
				TX = 2;
				break;

			case GSW_FLOW_RX:
				RX = 2;
				TX = 3;
				break;

			case GSW_FLOW_AUTO:
				RX = 0;
				TX = 0;
				break;
			}

			if (gswdev->gipver == LTQ_GSWIP_3_0) {
				if (pidx != 0) {
					if (gswdev->sdev == LTQ_FLOW_DEV_INT_R) {
						if (pidx == 15) {
							gsw_w32(cdev, MAC_CTRL_0_FCON_OFFSET, MAC_CTRL_0_FCON_SHIFT,
								MAC_CTRL_0_FCON_SIZE, parm->eFlowCtrl);
							gsw_w32(cdev, (GSWT_PHY_ADDR_1_FCONTX_OFFSET + GSW30_TOP_OFFSET),
								GSWT_PHY_ADDR_1_FCONTX_SHIFT, GSWT_PHY_ADDR_1_FCONTX_SIZE, TX);
							gsw_w32(cdev, (GSWT_PHY_ADDR_1_FCONRX_OFFSET + GSW30_TOP_OFFSET),
								GSWT_PHY_ADDR_1_FCONRX_SHIFT,	GSWT_PHY_ADDR_1_FCONRX_SIZE, RX);
						}
					} else {
						if (pidx < gswdev->pnum) {
							gsw_w32(cdev, (MAC_CTRL_0_FCON_OFFSET + (0xC * (pidx - 1))),
								MAC_CTRL_0_FCON_SHIFT, MAC_CTRL_0_FCON_SIZE, parm->eFlowCtrl);
							gsw_w32(cdev, ((GSWT_PHY_ADDR_1_FCONTX_OFFSET + ((pidx - 1) * 4)) + GSW30_TOP_OFFSET),
								GSWT_PHY_ADDR_1_FCONTX_SHIFT, GSWT_PHY_ADDR_1_FCONTX_SIZE, TX);
							gsw_w32(cdev, ((GSWT_PHY_ADDR_1_FCONRX_OFFSET + ((pidx - 1) * 4)) + GSW30_TOP_OFFSET),
								GSWT_PHY_ADDR_1_FCONRX_SHIFT, GSWT_PHY_ADDR_1_FCONRX_SIZE, RX);
						}
					}
				}
			} else {
				if (pidx < gswdev->pnum) {
					gsw_w32(cdev, (MAC_CTRL_0_FCON_OFFSET + (0xC * pidx)),
						MAC_CTRL_0_FCON_SHIFT, MAC_CTRL_0_FCON_SIZE, parm->eFlowCtrl);
					gsw_w32(cdev, (PHY_ADDR_0_FCONTX_OFFSET - (0x1 * pidx)),
						PHY_ADDR_0_FCONTX_SHIFT, PHY_ADDR_0_FCONTX_SIZE, TX);
					gsw_w32(cdev, (PHY_ADDR_0_FCONRX_OFFSET - (0x1 * pidx)),
						PHY_ADDR_0_FCONRX_SHIFT, PHY_ADDR_0_FCONRX_SIZE, RX);
				}
			}
		}
	}

	if (IS_VRSN_31(gswdev->gipver)) {
#if defined(CONFIG_MAC) && CONFIG_MAC

		/* MAC API's to set flow control */
		if (parm->ePortType == GSW_PHYSICAL_PORT ||
		    parm->ePortType == GSW_LOGICAL_PORT) {
			struct mac_ops *ops = get_mac_ops(gswdev, parm->nPortId);

			if (!ops) {
				pr_err("MAC %d is not initialized\n", parm->nPortId);
				ret = GSW_statusErr;
				goto UNLOCK_AND_RETURN;
			}

			ops->set_flow_ctl(ops, parm->eFlowCtrl);
		} else
			pr_err("MAC configuration is not valid for CTP or Bridge Port %d\n", parm->nPortId);

#endif
	}

	if (IS_VRSN_NOT_31(gswdev->gipver)) {
		switch (parm->ePortMonitor) {
		case GSW_PORT_MONITOR_NONE:
			gsw_w32(cdev,
				(PCE_PCTRL_3_RXVMIR_OFFSET + (0xA * pidx)),
				PCE_PCTRL_3_RXVMIR_SHIFT,
				PCE_PCTRL_3_RXVMIR_SIZE, 0);
			gsw_w32(cdev,
				(PCE_PCTRL_3_TXMIR_OFFSET + (0xA * pidx)),
				PCE_PCTRL_3_TXMIR_SHIFT,
				PCE_PCTRL_3_TXMIR_SIZE, 0);
			gsw_w32(cdev,
				(PCE_PCTRL_3_VIO_2_OFFSET + (0xA * pidx)),
				PCE_PCTRL_3_VIO_2_SHIFT,
				PCE_PCTRL_3_VIO_2_SIZE, 0);
			gsw_w32(cdev,
				(PCE_PCTRL_3_VIO_4_OFFSET + (0xA * pidx)),
				PCE_PCTRL_3_VIO_4_SHIFT,
				PCE_PCTRL_3_VIO_4_SIZE, 0);
			gsw_w32(cdev,
				(PCE_PCTRL_3_VIO_5_OFFSET + (0xA * pidx)),
				PCE_PCTRL_3_VIO_5_SHIFT,
				PCE_PCTRL_3_VIO_5_SIZE, 0);
			gsw_w32(cdev,
				(PCE_PCTRL_3_VIO_6_OFFSET + (0xA * pidx)),
				PCE_PCTRL_3_VIO_6_SHIFT,
				PCE_PCTRL_3_VIO_6_SIZE, 0);
			gsw_w32(cdev,
				(PCE_PCTRL_3_VIO_7_OFFSET + (0xA * pidx)),
				PCE_PCTRL_3_VIO_7_SHIFT,
				PCE_PCTRL_3_VIO_7_SIZE, 0);
			break;

		case GSW_PORT_MONITOR_RX:
			gsw_w32(cdev, (PCE_PCTRL_3_RXVMIR_OFFSET + (0xA * pidx)),
				PCE_PCTRL_3_RXVMIR_SHIFT,
				PCE_PCTRL_3_RXVMIR_SIZE, 1);
			gsw_w32(cdev, (PCE_PCTRL_3_TXMIR_OFFSET + (0xA * pidx)),
				PCE_PCTRL_3_TXMIR_SHIFT,
				PCE_PCTRL_3_TXMIR_SIZE, 0);
			break;

		case GSW_PORT_MONITOR_TX:
			gsw_w32(cdev, (PCE_PCTRL_3_RXVMIR_OFFSET + (0xA * pidx)),
				PCE_PCTRL_3_RXVMIR_SHIFT,
				PCE_PCTRL_3_RXVMIR_SIZE, 0);
			gsw_w32(cdev, (PCE_PCTRL_3_TXMIR_OFFSET + (0xA * pidx)),
				PCE_PCTRL_3_TXMIR_SHIFT,
				PCE_PCTRL_3_TXMIR_SIZE, 1);
			break;

		case GSW_PORT_MONITOR_RXTX:
			gsw_w32(cdev, (PCE_PCTRL_3_RXVMIR_OFFSET + (0xA * pidx)),
				PCE_PCTRL_3_RXVMIR_SHIFT,
				PCE_PCTRL_3_RXVMIR_SIZE, 1);
			gsw_w32(cdev, (PCE_PCTRL_3_TXMIR_OFFSET + (0xA * pidx)),
				PCE_PCTRL_3_TXMIR_SHIFT,
				PCE_PCTRL_3_TXMIR_SIZE, 1);
			break;

		case GSW_PORT_MONITOR_VLAN_UNKNOWN:
			gsw_w32(cdev, (PCE_PCTRL_3_VIO_2_OFFSET + (0xA * pidx)),
				PCE_PCTRL_3_VIO_2_SHIFT,
				PCE_PCTRL_3_VIO_2_SIZE, 1);
			break;

		case GSW_PORT_MONITOR_VLAN_MEMBERSHIP:
			gsw_w32(cdev, (PCE_PCTRL_3_VIO_4_OFFSET + (0xA * pidx)),
				PCE_PCTRL_3_VIO_4_SHIFT,
				PCE_PCTRL_3_VIO_4_SIZE, 1);
			break;

		case GSW_PORT_MONITOR_PORT_STATE:
			gsw_w32(cdev, (PCE_PCTRL_3_VIO_5_OFFSET + (0xA * pidx)),
				PCE_PCTRL_3_VIO_5_SHIFT,
				PCE_PCTRL_3_VIO_5_SIZE, 1);
			break;

		case GSW_PORT_MONITOR_LEARNING_LIMIT:
			gsw_w32(cdev, (PCE_PCTRL_3_VIO_6_OFFSET + (0xA * pidx)),
				PCE_PCTRL_3_VIO_6_SHIFT,
				PCE_PCTRL_3_VIO_6_SIZE, 1);
			break;

		case GSW_PORT_MONITOR_PORT_LOCK:
			gsw_w32(cdev, (PCE_PCTRL_3_VIO_7_OFFSET + (0xA * pidx)),
				PCE_PCTRL_3_VIO_7_SHIFT,
				PCE_PCTRL_3_VIO_7_SIZE, 1);
			break;
		}
	} else {
		// TODO: Port Mirroring uses CTP port configuration
	}

	if (parm->eEnable == GSW_PORT_ENABLE_RXTX) {
		PEN = 1;
		EN = 1;
	} else if (parm->eEnable == GSW_PORT_ENABLE_RX) {
		PEN = 1;
		EN = 0;
	} else if (parm->eEnable == GSW_PORT_ENABLE_TX) {
		PEN = 0;
		EN = 1;
	} else {
		PEN = 0;
		EN = 0;
	}

	/* Set SDMA_PCTRL_PEN PORT enable */
	gsw_w32(cdev, (SDMA_PCTRL_PEN_OFFSET + (6 * pidx)),
		SDMA_PCTRL_PEN_SHIFT, SDMA_PCTRL_PEN_SIZE, PEN);
	/* Set FDMA_PCTRL_EN PORT enable  */
	gsw_w32(cdev, (FDMA_PCTRL_EN_OFFSET + (0x6 * pidx)),
		FDMA_PCTRL_EN_SHIFT, FDMA_PCTRL_EN_SIZE, EN);

	if (gswdev->gipver == LTQ_GSWIP_3_0) {
		if (parm->bIfCounters == 1) {
			gsw_w32(cdev, (BM_RMON_CTRL_IFRMONFST_OFFSET + (0x2 * pidx)),
				BM_RMON_CTRL_IFRMONFST_SHIFT, BM_RMON_CTRL_IFRMONFST_SIZE, parm->nIfCountStartIdx);
		}

		gsw_w32(cdev, (BM_RMON_CTRL_IFRMONMD_OFFSET + (0x2 * pidx)),
			BM_RMON_CTRL_IFRMONMD_SHIFT, BM_RMON_CTRL_IFRMONMD_SIZE, parm->eIfRMONmode);
	}

	ret = GSW_statusOk;


UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_misc);
#endif
	return ret;
}


#if defined(CONFIG_LTQ_STP) && CONFIG_LTQ_STP
GSW_return_t GSW_STP_BPDU_RuleGet(void *cdev,
				  GSW_STP_BPDU_Rule_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	stp8021x_t *scfg;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_pce);
#endif

	scfg = &gswdev->stpconfig;
	parm->eForwardPort = scfg->spstate;
	parm->nForwardPortId = scfg->stppid;
	ret = GSW_statusOk;

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_pce);
#endif
	return ret;
}

GSW_return_t GSW_STP_BPDU_RuleSet(void *cdev,
				  GSW_STP_BPDU_Rule_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	stp8021x_t *scfg = &gswdev->stpconfig;
	static GSW_PCE_rule_t pcrule;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_pce);
#endif

	scfg->spstate = parm->eForwardPort;
	scfg->stppid = parm->nForwardPortId;
	memset(&pcrule, 0, sizeof(GSW_PCE_rule_t));
	/* Attached the PCE rule for BPDU packet */
	pcrule.pattern.nIndex = BPDU_PCE_RULE_INDEX;
	pcrule.pattern.bEnable = 1;
	pcrule.pattern.bMAC_DstEnable = 1;
	pcrule.pattern.nMAC_Dst[0] = 0x01;
	pcrule.pattern.nMAC_Dst[1] = 0x80;
	pcrule.pattern.nMAC_Dst[2] = 0xC2;
	pcrule.pattern.nMAC_Dst[3] = 0x00;
	pcrule.pattern.nMAC_Dst[4] = 0x00;
	pcrule.pattern.nMAC_Dst[5] = 0x00;
	pcrule.action.eCrossStateAction	= GSW_PCE_ACTION_CROSS_STATE_CROSS;

	if ((scfg->spstate < 4) && (scfg->spstate > 0))
		pcrule.action.ePortMapAction = scfg->spstate + 1;
	else {
		pr_err("(Incorrect forward port action) %s:%s:%d\n",
		       __FILE__, __func__, __LINE__);
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	pcrule.action.nForwardPortMap[0] = (1 << scfg->stppid); //Govind - Can occur out of array bounds problem.

	/* We prepare everything and write into PCE Table */
	if (0 != pce_rule_write(cdev, &gswdev->phandler, &pcrule)) {
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_pce);
#endif
	return ret;

}

GSW_return_t GSW_STP_PortCfgGet(void *cdev, GSW_STP_portCfg_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_pce);
#endif

	if (IS_VRSN_NOT_31(gswdev->gipver)) {
		if (parm->nPortId >= gswdev->tpnum) {
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}

		parm->ePortState = gswdev->pconfig[parm->nPortId].pcstate;
	}

	if (IS_VRSN_31(gswdev->gipver)) {
		if (parm->nPortId >= gswdev->num_of_bridge_port) {
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}

		/*If Bridge Port ID is valid,Check whether it is InUSE
		  if not InUse,return ERROR*/
		if (!(gswdev->brdgeportconfig_idx[parm->nPortId].IndexInUse)) {
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}

		parm->ePortState = gswdev->brdgeportconfig_idx[parm->nPortId].StpState;
	}

	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_pce);
#endif
	return ret;
}

GSW_return_t GSW_STP_PortCfgSet(void *cdev, GSW_STP_portCfg_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u8 StpState, P8021xState;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_pce);
#endif

	if (IS_VRSN_NOT_31(gswdev->gipver)) {
		if (parm->nPortId >= gswdev->tpnum) {
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}

		gswdev->pconfig[parm->nPortId].pcstate = parm->ePortState;
		/* Config the Table */
		set_port_state(cdev, parm->nPortId,
			       gswdev->pconfig[parm->nPortId].pcstate,
			       gswdev->pconfig[parm->nPortId].p8021xs);
	}

	if (IS_VRSN_31(gswdev->gipver)) {
		if (parm->nPortId >= gswdev->num_of_bridge_port) {
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}

		/*If Bridge Port ID is valid,Check whether it is InUSE
		  if not InUse,return ERROR*/
		if (!(gswdev->brdgeportconfig_idx[parm->nPortId].IndexInUse)) {
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}

		StpState = parm->ePortState;
		P8021xState = gswdev->brdgeportconfig_idx[parm->nPortId].P8021xState;
		gswdev->brdgeportconfig_idx[parm->nPortId].StpState = StpState;

		/* Config the Table */
		set_port_state(cdev, parm->nPortId, StpState, P8021xState);
	}

	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_pce);
#endif
	return ret;
}

GSW_return_t GSW_TrunkingCfgGet(void *cdev, GSW_trunkingCfg_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_misc);
#endif

	/* Supported for GSWIP 2.2 and newer and returns */
	/* with an error for older hardware revisions. */
	if (gswdev->gipver != LTQ_GSWIP_2_0) {
		/* Destination IP Mask */
		gsw_r32(cdev, PCE_TRUNK_CONF_DIP_OFFSET,
			PCE_TRUNK_CONF_DIP_SHIFT,
			PCE_TRUNK_CONF_DIP_SIZE, &parm->bIP_Dst);
		/* 'Source IP Mask */
		gsw_r32(cdev, PCE_TRUNK_CONF_SIP_OFFSET,
			PCE_TRUNK_CONF_SIP_SHIFT,
			PCE_TRUNK_CONF_SIP_SIZE, &parm->bIP_Src);
		/* Destination MAC Mask */
		gsw_r32(cdev, PCE_TRUNK_CONF_DA_OFFSET,
			PCE_TRUNK_CONF_DA_SHIFT,
			PCE_TRUNK_CONF_DA_SIZE, &parm->bMAC_Dst);
		/* 'Source MAC Mask */
		gsw_r32(cdev, PCE_TRUNK_CONF_SA_OFFSET,
			PCE_TRUNK_CONF_SA_SHIFT,
			PCE_TRUNK_CONF_SA_SIZE, &parm->bMAC_Src);
		ret = GSW_statusOk;
		goto UNLOCK_AND_RETURN;
	} else {
		ret = GSW_statusNoSupport;
	}

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_misc);
#endif
	return ret;

}

GSW_return_t GSW_TrunkingCfgSet(void *cdev,
				GSW_trunkingCfg_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 ret;

	/* Supported for GSWIP 2.2 and newer and returns */
	/* with an error for older hardware revisions. */
	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_misc);
#endif

	if (gswdev->gipver != LTQ_GSWIP_2_0) {
		/* Destination IP Mask */
		if (parm->bIP_Dst == 1) {
			gsw_w32(cdev, PCE_TRUNK_CONF_DIP_OFFSET,
				PCE_TRUNK_CONF_DIP_SHIFT,
				PCE_TRUNK_CONF_DIP_SIZE, 1);
		} else {
			gsw_w32(cdev, PCE_TRUNK_CONF_DIP_OFFSET,
				PCE_TRUNK_CONF_DIP_SHIFT,
				PCE_TRUNK_CONF_DIP_SIZE, 0);
		}

		/* 'Source IP Mask */
		if (parm->bIP_Src == 1) {
			gsw_w32(cdev, PCE_TRUNK_CONF_SIP_OFFSET,
				PCE_TRUNK_CONF_SIP_SHIFT,
				PCE_TRUNK_CONF_SIP_SIZE, 1);
		} else {
			gsw_w32(cdev, PCE_TRUNK_CONF_SIP_OFFSET,
				PCE_TRUNK_CONF_SIP_SHIFT,
				PCE_TRUNK_CONF_SIP_SIZE, 0);
		}

		/* Destination MAC Mask */
		if (parm->bMAC_Dst == 1) {
			gsw_w32(cdev, PCE_TRUNK_CONF_DA_OFFSET,
				PCE_TRUNK_CONF_DA_SHIFT,
				PCE_TRUNK_CONF_DA_SIZE, 1);
		} else {
			gsw_w32(cdev, PCE_TRUNK_CONF_DA_OFFSET,
				PCE_TRUNK_CONF_DA_SHIFT,
				PCE_TRUNK_CONF_DA_SIZE, 0);
		}

		/* 'Source MAC Mask */
		if (parm->bMAC_Src == 1) {
			gsw_w32(cdev, PCE_TRUNK_CONF_SA_OFFSET,
				PCE_TRUNK_CONF_SA_SHIFT,
				PCE_TRUNK_CONF_SA_SIZE, 1);
		} else {
			gsw_w32(cdev, PCE_TRUNK_CONF_SA_OFFSET,
				PCE_TRUNK_CONF_SA_SHIFT,
				PCE_TRUNK_CONF_SA_SIZE, 0);
		}

		ret = GSW_statusOk;
		goto UNLOCK_AND_RETURN;
	} else {
		ret = GSW_statusNoSupport;
	}

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_misc);
#endif
	return ret;
}

GSW_return_t GSW_TrunkingPortCfgGet(void *cdev,
				    GSW_trunkingPortCfg_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 ret;
	u32 value;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_misc);
#endif

	if (parm->nPortId >= (gswdev->tpnum)) {
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	/* Supported for GSWIP 2.2 and newer and returns with */
	/* an error for older hardware revisions. */
	if (gswdev->gipver != LTQ_GSWIP_2_0) {
		/** Ports are aggregated. the 'nPortId' and the */
		/* 'nAggrPortId' ports form an aggregated link. */
		gsw_r32(cdev, (PCE_PTRUNK_EN_OFFSET + (parm->nPortId * 0x2)),
			PCE_PTRUNK_EN_SHIFT, PCE_PTRUNK_EN_SIZE, &value);
		parm->bAggregateEnable = value;
		gsw_r32(cdev, (PCE_PTRUNK_PARTER_OFFSET + (parm->nPortId * 0x2)),
			PCE_PTRUNK_PARTER_SHIFT, PCE_PTRUNK_PARTER_SIZE, &value);
		parm->nAggrPortId = value;
		ret = GSW_statusOk;
		goto UNLOCK_AND_RETURN;
	} else {
		ret = GSW_statusNoSupport;
	}

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_misc);
#endif
	return ret;
}

GSW_return_t GSW_TrunkingPortCfgSet(void *cdev, GSW_trunkingPortCfg_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_misc);
#endif

	if (parm->nPortId >= (gswdev->tpnum)) {
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	/* Supported for GSWIP 2.2 and newer and returns */
	/* with an error for older hardware revisions. */
	if (gswdev->gipver != LTQ_GSWIP_2_0) {
		/** Ports are aggregated. the 'nPortId' and the */
		/* 'nAggrPortId' ports form an aggregated link.*/
		if (parm->bAggregateEnable == 1) {
			gsw_w32(cdev, (PCE_PTRUNK_EN_OFFSET + (parm->nPortId * 0x2)),
				PCE_PTRUNK_EN_SHIFT, PCE_PTRUNK_EN_SIZE, 1);
			gsw_w32(cdev, (PCE_PTRUNK_PARTER_OFFSET + (parm->nPortId * 0x2)),
				PCE_PTRUNK_PARTER_SHIFT, PCE_PTRUNK_PARTER_SIZE, (parm->nAggrPortId & 0xF));
		} else {
			gsw_w32(cdev, (PCE_PTRUNK_EN_OFFSET + (parm->nPortId * 0x2)),
				PCE_PTRUNK_EN_SHIFT, PCE_PTRUNK_EN_SIZE, 0);
		}

		ret = GSW_statusOk;
		goto UNLOCK_AND_RETURN;
	} else {
		ret = GSW_statusNoSupport;
	}

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_misc);
#endif
	return ret;
}

GSW_return_t GSW_TimestampTimerSet(void *cdev, GSW_TIMESTAMP_Timer_t *parm)
{
	u32 value;
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_misc);
#endif

	/* Supported for GSWIP 2.2 and newer and returns with */
	/* an error for older hardware revisions. */
	if (gswdev->gipver != LTQ_GSWIP_2_0) {
		/** Second. Absolute second timer count. */
		gsw_w32(cdev, (TIMER_SEC_LSB_SECLSB_OFFSET),
			TIMER_SEC_LSB_SECLSB_SHIFT,
			TIMER_SEC_LSB_SECLSB_SIZE, (parm->nSec & 0xFFFF));
		gsw_w32(cdev, (TIMER_SEC_MSB_SECMSB_OFFSET),
			TIMER_SEC_MSB_SECMSB_SHIFT,
			TIMER_SEC_MSB_SECMSB_SIZE,
			((parm->nSec >> 16) & 0xFFFF));
		/** Nano Second. Absolute nano second timer count.*/
		gsw_w32(cdev, (TIMER_NS_LSB_NSLSB_OFFSET),
			TIMER_NS_LSB_NSLSB_SHIFT,
			TIMER_NS_LSB_NSLSB_SIZE,
			(parm->nNanoSec & 0xFFFF));
		gsw_w32(cdev, (TIMER_NS_MSB_NSMSB_OFFSET),
			TIMER_NS_MSB_NSMSB_SHIFT,
			TIMER_NS_MSB_NSMSB_SIZE,
			((parm->nNanoSec >> 16) & 0xFFFF));
		/** Fractional Nano Second. Absolute fractional nano */
		/* second timer count. This counter specifis a */
		/* 2^32 fractional 'nNanoSec'. */
		gsw_w32(cdev, (TIMER_FS_LSB_FSLSB_OFFSET),
			TIMER_FS_LSB_FSLSB_SHIFT,
			TIMER_FS_LSB_FSLSB_SIZE,
			(parm->nFractionalNanoSec & 0xFFFF));
		gsw_w32(cdev, (TIMER_FS_MSB_FSMSB_OFFSET),
			TIMER_FS_MSB_FSMSB_SHIFT,
			TIMER_FS_MSB_FSMSB_SIZE,
			((parm->nFractionalNanoSec >> 16) & 0xFFFF));
		value = 1;
		gsw_w32(cdev, (TIMER_CTRL_WR_OFFSET),
			TIMER_CTRL_WR_SHIFT,
			TIMER_CTRL_WR_SIZE, value);

		CHECK_BUSY(TIMER_CTRL_WR_OFFSET,
			   TIMER_CTRL_WR_SHIFT, TIMER_CTRL_WR_SIZE, RETURN_FROM_FUNCTION);


		ret = GSW_statusOk;
		goto UNLOCK_AND_RETURN;
	} else {
		ret = GSW_statusNoSupport;
	}

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_misc);
#endif
	return ret;
}

GSW_return_t GSW_TimestampTimerGet(void *cdev,
				   GSW_TIMESTAMP_Timer_t *parm)
{
	u32 value;
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 ret;
	value = 1;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_misc);
#endif

	/* Supported for GSWIP 2.2 and newer and returns */
	/* with an error for older hardware revisions. */
	if (gswdev->gipver != LTQ_GSWIP_2_0) {
		gsw_w32(cdev, (TIMER_CTRL_RD_OFFSET),
			TIMER_CTRL_RD_SHIFT,
			TIMER_CTRL_RD_SIZE, value);

		CHECK_BUSY(TIMER_CTRL_RD_OFFSET,
			   TIMER_CTRL_RD_SHIFT, TIMER_CTRL_RD_SIZE, RETURN_FROM_FUNCTION);

		/** Second. Absolute second timer count. */
		gsw_r32(cdev, (TIMER_SEC_LSB_SECLSB_OFFSET),
			TIMER_SEC_LSB_SECLSB_SHIFT,
			TIMER_SEC_LSB_SECLSB_SIZE, &value);
		parm->nSec = value & 0xFFFF;
		gsw_r32(cdev, (TIMER_SEC_MSB_SECMSB_OFFSET),
			TIMER_SEC_MSB_SECMSB_SHIFT,
			TIMER_SEC_MSB_SECMSB_SIZE, &value);
		parm->nSec |= (value & 0xFFFF << 16);
		/** Nano Second. Absolute nano second timer count. */
		gsw_r32(cdev, (TIMER_NS_LSB_NSLSB_OFFSET),
			TIMER_NS_LSB_NSLSB_SHIFT,
			TIMER_NS_LSB_NSLSB_SIZE, &value);
		parm->nNanoSec = value & 0xFFFF;
		gsw_r32(cdev, (TIMER_NS_MSB_NSMSB_OFFSET),
			TIMER_NS_MSB_NSMSB_SHIFT,
			TIMER_NS_MSB_NSMSB_SIZE, &value);
		parm->nNanoSec |= (value & 0xFFFF << 16);
		/** Fractional Nano Second. Absolute fractional */
		/* nano second timer count. */
		/*	This counter specifis a 2^32 fractional 'nNanoSec'. */
		gsw_r32(cdev, (TIMER_FS_LSB_FSLSB_OFFSET),
			TIMER_FS_LSB_FSLSB_SHIFT,
			TIMER_FS_LSB_FSLSB_SIZE, &value);
		parm->nFractionalNanoSec = value & 0xFFFF;
		gsw_r32(cdev, (TIMER_FS_MSB_FSMSB_OFFSET),
			TIMER_FS_MSB_FSMSB_SHIFT,
			TIMER_FS_MSB_FSMSB_SIZE, &value);
		parm->nFractionalNanoSec |= (value & 0xFFFF << 16);
		ret = GSW_statusOk;
		goto UNLOCK_AND_RETURN;
	} else {
		ret = GSW_statusNoSupport;
	}

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_misc);
#endif
	return ret;

}
GSW_return_t GSW_TimestampPortRead(void *cdev,
				   GSW_TIMESTAMP_PortRead_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 tstamp0, tstamp1;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_misc);
#endif

	if (parm->nPortId >= ((gswdev->tpnum - 1))) {
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	/* Supported for GSWIP 2.2 and newer and returns */
	/* with an error for older hardware revisions. */
	if (gswdev->gipver != LTQ_GSWIP_2_0) {
		/** Second. Absolute second timer count. */
		gsw_r32(cdev, (FDMA_TSTAMP0_TSTL_OFFSET +
			       (parm->nPortId * 0x6)),
			FDMA_TSTAMP0_TSTL_SHIFT,
			FDMA_TSTAMP0_TSTL_SIZE, &tstamp0);
		gsw_r32(cdev, (FDMA_TSTAMP1_TSTH_OFFSET +
			       (parm->nPortId * 0x6)),
			FDMA_TSTAMP1_TSTH_SHIFT,
			FDMA_TSTAMP1_TSTH_SIZE, &tstamp1);
		parm->nEgressSec = ((tstamp0 | (tstamp1 << 16))) >> 30;
		parm->nEgressNanoSec = (((tstamp0 | (tstamp1 << 16)))
					& 0x7FFFFFFF);
		/** Nano Second. Absolute nano second timer count. */
		gsw_r32(cdev, (SDMA_TSTAMP0_TSTL_OFFSET +
			       (parm->nPortId * 0x6)),
			SDMA_TSTAMP0_TSTL_SHIFT,
			SDMA_TSTAMP0_TSTL_SIZE, &tstamp0);
		gsw_r32(cdev, (SDMA_TSTAMP1_TSTH_OFFSET +
			       (parm->nPortId * 0x6)),
			SDMA_TSTAMP1_TSTH_SHIFT,
			SDMA_TSTAMP1_TSTH_SIZE, &tstamp1);
		parm->nIngressSec = ((tstamp0 | (tstamp1 << 16))) >> 30;
		parm->nIngressNanoSec = (((tstamp0 | (tstamp1 << 16)))
					 & 0x7FFFFFFF);
		ret = GSW_statusOk;
		goto UNLOCK_AND_RETURN;
	} else {
		ret = GSW_statusNoSupport;
	}

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_misc);
#endif
	return ret;
}

#endif /* CONFIG_LTQ_STP */
#if defined(CONFIG_LTQ_VLAN) && CONFIG_LTQ_VLAN
GSW_return_t GSW_VLAN_Member_Init(void *cdev,
				  GSW_VLAN_memberInit_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_pce);
#endif

	if ((gswdev->gipver == LTQ_GSWIP_2_2_ETC) ||
	    (gswdev->gipver == LTQ_GSWIP_3_0)) {
		pctbl_prog_t pcetable;
		u16 pcindex;

		for (pcindex = 0; pcindex < 4096; pcindex++) {
			memset(&pcetable, 0, sizeof(pctbl_prog_t));
			pcetable.pcindex = pcindex;
			pcetable.table = PCE_VLANMAP_INDEX;
			gsw_pce_table_read(cdev, &pcetable);
			pcetable.pcindex = pcindex;
			pcetable.table = PCE_VLANMAP_INDEX;
			pcetable.val[1] = (parm->nPortMemberMap & 0xFFFF);
			pcetable.val[2] = (parm->nEgressTagMap & 0xFFFF);
			gsw_pce_table_write(cdev, &pcetable);
		}

		ret = GSW_statusOk;
		goto UNLOCK_AND_RETURN;
	} else {
		ret = GSW_statusNoSupport;
	}

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_pce);
#endif
	return ret;

}

GSW_return_t GSW_VLAN_IdCreate(void *cdev,
			       GSW_VLAN_IdCreate_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	pctbl_prog_t pcetable;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_pce);
#endif

	if ((gswdev->gipver == LTQ_GSWIP_2_2_ETC) ||
	    (gswdev->gipver == LTQ_GSWIP_3_0)) {
		memset(&pcetable, 0, sizeof(pctbl_prog_t));
		pcetable.pcindex = parm->nVId;
		pcetable.table = PCE_VLANMAP_INDEX;
		pcetable.val[0] = (parm->nFId & 0x3F);
		gsw_pce_table_write(cdev, &pcetable);
	} else {
		u8 pcindex;
		avlan_tbl_t avlantbl;

		if (find_active_vlan_index(cdev, parm->nVId) != 0xFF) {
			pr_err("This vid exists\n");
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}

		pcindex = fempty_avlan_index_table(cdev);

		if (pcindex == 0xFF) {
			pr_err("There is no table entry avariable\n");
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}

		memset(&avlantbl, 0, sizeof(avlan_tbl_t));
		memset(&pcetable, 0, sizeof(pctbl_prog_t));
		avlantbl.valid = 1;
		avlantbl.vid = parm->nVId;
		avlantbl.fid = parm->nFId;

		if (pcindex >= 64) {
			ret = GSW_statusValueRange;
			goto UNLOCK_AND_RETURN;
		}

		vlan_entry_set(cdev, pcindex, &avlantbl);
		memset(&pcetable, 0, sizeof(pctbl_prog_t));
		pcetable.pcindex = pcindex;
		pcetable.table = PCE_ACTVLAN_INDEX;
		pcetable.key[0] = parm->nVId;
		pcetable.val[0] = parm->nFId;
		pcetable.valid = 1;
		gsw_pce_table_write(cdev, &pcetable);
		pcetable.table = PCE_VLANMAP_INDEX;
		pcetable.val[0] = parm->nVId;
		pcetable.val[1] = 0;
		pcetable.val[2] = 0;
		gsw_pce_table_write(cdev, &pcetable);
	}

	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_pce);
#endif
	return ret;

}

GSW_return_t GSW_VLAN_IdDelete(void *cdev,
			       GSW_VLAN_IdDelete_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	pctbl_prog_t pcetable;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_pce);
#endif

	if ((gswdev->gipver == LTQ_GSWIP_2_2_ETC) ||
	    (gswdev->gipver == LTQ_GSWIP_3_0)) {
		memset(&pcetable, 0, sizeof(pctbl_prog_t));
		pcetable.pcindex = parm->nVId;
		pcetable.table = PCE_VLANMAP_INDEX;
		gsw_pce_table_write(cdev, &pcetable);
	} else {
		u8 pcindex;
		avlan_tbl_t avlantbl;
		ltq_pce_table_t *pcvtbl = &gswdev->phandler;
		memset(&avlantbl, 0, sizeof(avlan_tbl_t));
		pcindex = find_active_vlan_index(cdev, parm->nVId);

		if (pcindex == 0xFF) {
			pr_err("(VID not exists) %s:%s:%d\n",
			       __FILE__, __func__, __LINE__);
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}

		if (gavlan_tbl_index(&pcvtbl->pce_sub_tbl,
				     pcindex) != GSW_statusOk) {
			pr_err("(VID: 0x%0x used by flow table) %s:%s:%d\n",
			       parm->nVId, __FILE__, __func__, __LINE__);
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}

		if (pcindex >= 64) {
			ret = GSW_statusValueRange;
			goto UNLOCK_AND_RETURN;
		}

		vlan_entry_set(cdev, pcindex, &avlantbl);
		memset(&pcetable, 0, sizeof(pctbl_prog_t));
		pcetable.pcindex = pcindex;
		pcetable.table = PCE_ACTVLAN_INDEX;
		pcetable.valid = 0;
		gsw_pce_table_write(cdev, &pcetable);
		pcetable.table = PCE_VLANMAP_INDEX;
		gsw_pce_table_write(cdev, &pcetable);
	}

	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_pce);
#endif
	return ret;
}

GSW_return_t GSW_VLAN_IdGet(void *cdev, GSW_VLAN_IdGet_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	pctbl_prog_t pcetable;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_pce);
#endif

	if ((gswdev->gipver == LTQ_GSWIP_2_2_ETC) ||
	    (gswdev->gipver == LTQ_GSWIP_3_0)) {
		memset(&pcetable, 0, sizeof(pctbl_prog_t));
		pcetable.pcindex = parm->nVId;
		pcetable.table = PCE_VLANMAP_INDEX;
		gsw_pce_table_read(cdev, &pcetable);
		parm->nFId = pcetable.val[0] & 0x3F;
	} else {
		u8 pcindex;
		avlan_tbl_t avlantbl;
		pcindex = find_active_vlan_index(cdev, parm->nVId);

		if (pcindex == 0xFF) {
			pr_err("(VID not exists) %s:%s:%d\n",
			       __FILE__, __func__, __LINE__);
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}

		if (pcindex >= 64) {
			ret = GSW_statusValueRange;
			goto UNLOCK_AND_RETURN;
		}

		get_vlan_sw_table(cdev, pcindex, &avlantbl);
		parm->nFId = avlantbl.fid;
	}

	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_pce);
#endif
	return ret;

}

GSW_return_t GSW_VLAN_PortCfgGet(void *cdev,
				 GSW_VLAN_portCfg_t *parm)
{
	u32 value;
	int pcindex;
	ltq_bool_t uvr, vimr, vemr;
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	avlan_tbl_t avlantbl;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_pce);
#endif

	if (parm->nPortId >= gswdev->tpnum) {
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	gsw_r32(cdev, (PCE_DEFPVID_PVID_OFFSET + (10 * parm->nPortId)),
		PCE_DEFPVID_PVID_SHIFT, PCE_DEFPVID_PVID_SIZE, &value);

	if ((gswdev->gipver == LTQ_GSWIP_2_2_ETC) ||
	    (gswdev->gipver == LTQ_GSWIP_3_0)) {
		parm->nPortVId = value;
	} else {
		pcindex = value;

		if (pcindex >= 64) {
			ret = GSW_statusValueRange;
			goto UNLOCK_AND_RETURN;
		}

		get_vlan_sw_table(cdev, pcindex, &avlantbl);
		parm->nPortVId = avlantbl.vid;
	}

	gsw_r32(cdev, (PCE_VCTRL_UVR_OFFSET + (10 * parm->nPortId)),
		PCE_VCTRL_UVR_SHIFT, PCE_VCTRL_UVR_SIZE, &value);
	uvr = value;

	if (uvr == 1)
		parm->bVLAN_UnknownDrop = 1;
	else
		parm->bVLAN_UnknownDrop = 0;

	gsw_r32(cdev, (PCE_VCTRL_VSR_OFFSET +
		       (10 * parm->nPortId)),
		PCE_VCTRL_VSR_SHIFT,
		PCE_VCTRL_VSR_SIZE, &value);
	parm->bVLAN_ReAssign = value;
	gsw_r32(cdev, (PCE_VCTRL_VIMR_OFFSET +
		       (10 * parm->nPortId)),
		PCE_VCTRL_VIMR_SHIFT,
		PCE_VCTRL_VIMR_SIZE, &value);
	vimr = value;

	gsw_r32(cdev, (PCE_VCTRL_VEMR_OFFSET +
		       (10 * parm->nPortId)),
		PCE_VCTRL_VEMR_SHIFT,
		PCE_VCTRL_VEMR_SIZE, &value);
	vemr = value;

	if (vimr == 0 && vemr == 0)
		parm->eVLAN_MemberViolation = GSW_VLAN_MEMBER_VIOLATION_NO;
	else if (vimr == 1 && vemr == 0)
		parm->eVLAN_MemberViolation = GSW_VLAN_MEMBER_VIOLATION_INGRESS;
	else if (vimr == 0 && vemr == 1)
		parm->eVLAN_MemberViolation = GSW_VLAN_MEMBER_VIOLATION_EGRESS;
	else if (vimr == 1 && vemr == 1)
		parm->eVLAN_MemberViolation = GSW_VLAN_MEMBER_VIOLATION_BOTH;

	gsw_r32(cdev, (PCE_VCTRL_VINR_OFFSET +
		       (10 * parm->nPortId)),
		PCE_VCTRL_VINR_SHIFT,
		PCE_VCTRL_VINR_SIZE, &value);

	switch (value) {
	case 0:
		parm->eAdmitMode = GSW_VLAN_ADMIT_ALL;
		break;

	case 1:
		parm->eAdmitMode = GSW_VLAN_ADMIT_TAGGED;
		break;

	case 2:
		parm->eAdmitMode = GSW_VLAN_ADMIT_UNTAGGED;
		break;

	default:
		break;
	}

	gsw_r32(cdev, (PCE_PCTRL_0_TVM_OFFSET +
		       (10 * parm->nPortId)),
		PCE_PCTRL_0_TVM_SHIFT,
		PCE_PCTRL_0_TVM_SIZE, &value);
	parm->bTVM = value;
	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_pce);
#endif
	return ret;
}

GSW_return_t GSW_VLAN_PortCfgSet(void *cdev,
				 GSW_VLAN_portCfg_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 value;
	u8 pcindex;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_pce);
#endif

	if (parm->nPortId >= gswdev->tpnum) {
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	if ((gswdev->gipver == LTQ_GSWIP_2_2_ETC) ||
	    (gswdev->gipver == LTQ_GSWIP_3_0)) {
		value = parm->nPortVId;
	} else {
		pcindex = find_active_vlan_index(cdev, parm->nPortVId);

		if (pcindex == 0xFF) {
			pr_err("(VID not exists) %s:%s:%d\n",
			       __FILE__, __func__, __LINE__);
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}

		value = pcindex;
	}

	gsw_w32(cdev, (PCE_DEFPVID_PVID_OFFSET +
		       (10 * parm->nPortId)),
		PCE_DEFPVID_PVID_SHIFT,
		PCE_DEFPVID_PVID_SIZE, value);
	value = 0;

	if (parm->bVLAN_UnknownDrop == 1)
		value = 1;

	gsw_w32(cdev, (PCE_VCTRL_UVR_OFFSET +
		       (10 * parm->nPortId)),
		PCE_VCTRL_UVR_SHIFT,
		PCE_VCTRL_UVR_SIZE, value);
	value = parm->bVLAN_ReAssign;
	gsw_w32(cdev, (PCE_VCTRL_VSR_OFFSET +
		       (10 * parm->nPortId)),
		PCE_VCTRL_VSR_SHIFT,
		PCE_VCTRL_VSR_SIZE, value);

	switch (parm->eVLAN_MemberViolation) {
	case GSW_VLAN_MEMBER_VIOLATION_NO:
		gsw_w32(cdev, (PCE_VCTRL_VIMR_OFFSET +
			       (10 * parm->nPortId)),
			PCE_VCTRL_VIMR_SHIFT,
			PCE_VCTRL_VIMR_SIZE, 0);
		gsw_w32(cdev, (PCE_VCTRL_VEMR_OFFSET +
			       (10 * parm->nPortId)),
			PCE_VCTRL_VEMR_SHIFT,
			PCE_VCTRL_VEMR_SIZE, 0);
		break;

	case GSW_VLAN_MEMBER_VIOLATION_INGRESS:
		gsw_w32(cdev, (PCE_VCTRL_VIMR_OFFSET +
			       (10 * parm->nPortId)),
			PCE_VCTRL_VIMR_SHIFT,
			PCE_VCTRL_VIMR_SIZE, 1);
		gsw_w32(cdev, (PCE_VCTRL_VEMR_OFFSET +
			       (10 * parm->nPortId)),
			PCE_VCTRL_VEMR_SHIFT,
			PCE_VCTRL_VEMR_SIZE, 0);
		break;

	case GSW_VLAN_MEMBER_VIOLATION_EGRESS:
		gsw_w32(cdev, (PCE_VCTRL_VIMR_OFFSET +
			       (10 * parm->nPortId)),
			PCE_VCTRL_VIMR_SHIFT,
			PCE_VCTRL_VIMR_SIZE, 0);
		gsw_w32(cdev, (PCE_VCTRL_VEMR_OFFSET +
			       (10 * parm->nPortId)),
			PCE_VCTRL_VEMR_SHIFT,
			PCE_VCTRL_VEMR_SIZE, 1);
		break;

	case GSW_VLAN_MEMBER_VIOLATION_BOTH:
		gsw_w32(cdev, (PCE_VCTRL_VIMR_OFFSET +
			       (10 * parm->nPortId)),
			PCE_VCTRL_VIMR_SHIFT,
			PCE_VCTRL_VIMR_SIZE, 1);
		gsw_w32(cdev, (PCE_VCTRL_VEMR_OFFSET +
			       (10 * parm->nPortId)),
			PCE_VCTRL_VEMR_SHIFT,
			PCE_VCTRL_VEMR_SIZE, 1);
		break;

	default:
		pr_err("WARNING:(eVLAN_MemberViolation) %s:%s:%d\n",
		       __FILE__, __func__, __LINE__);
	}

	switch (parm->eAdmitMode) {
	case GSW_VLAN_ADMIT_ALL:
		value = 0;
		break;

	case GSW_VLAN_ADMIT_TAGGED:
		value = 1;
		break;

	case GSW_VLAN_ADMIT_UNTAGGED:
		value = 2;
		break;

	default:
		value = 0;
		pr_err("%s:%s:%d (eAdmitMode)\n",
		       __FILE__, __func__, __LINE__);
	}

	gsw_w32(cdev, (PCE_VCTRL_VINR_OFFSET +
		       (10 * parm->nPortId)),
		PCE_VCTRL_VINR_SHIFT,
		PCE_VCTRL_VINR_SIZE, value);
	value = 0;

	if (parm->bTVM == 1)
		value = 1;

	gsw_w32(cdev, (PCE_PCTRL_0_TVM_OFFSET +
		       (10 * parm->nPortId)),
		PCE_PCTRL_0_TVM_SHIFT,
		PCE_PCTRL_0_TVM_SIZE, value);
	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_pce);
#endif
	return ret;
}

GSW_return_t GSW_VLAN_PortMemberAdd(void *cdev,
				    GSW_VLAN_portMemberAdd_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	pctbl_prog_t pcetable;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_pce);
#endif

	if ((parm->nPortId >= gswdev->tpnum) &&
	    (!(parm->nPortId & 0x80000000))) {
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;

	}

	if ((gswdev->gipver == LTQ_GSWIP_2_2_ETC) ||
	    (gswdev->gipver == LTQ_GSWIP_3_0)) {
		u16  portmap, tagmap, val0;

		if (parm->nVId > 4096) {
			pr_err("ERROR: %s:%s:%d, (VID:%d)\n",
			       __FILE__, __func__, __LINE__, parm->nVId);
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}

		memset(&pcetable, 0, sizeof(pctbl_prog_t));
		pcetable.table = PCE_VLANMAP_INDEX;
		pcetable.pcindex = parm->nVId;
		gsw_pce_table_read(cdev, &pcetable);
		portmap	= (pcetable.val[1]);
		tagmap = (pcetable.val[2]);
		val0 = (pcetable.val[0]);

		/*  Support  portmap information. */
		/*  To differentiate between port index and portmap, */
		/* the MSB (highest data bit) should be 1.*/
		if (parm->nPortId & 0x80000000) { /*Port Map */
			portmap |= ((parm->nPortId) & 0xFFFF);

			if (parm->bVLAN_TagEgress)
				tagmap |= ((parm->nPortId) & 0xFFFF);
			else
				tagmap &= ~((parm->nPortId) & 0xFFFF);
		} else {
			portmap |= 1 << parm->nPortId;

			if (parm->bVLAN_TagEgress)
				tagmap |= 1 << parm->nPortId;
			else
				tagmap &= ~(1 << parm->nPortId);
		}

		pcetable.table = PCE_VLANMAP_INDEX;
		pcetable.pcindex = parm->nVId;
		pcetable.val[0] = val0;
		pcetable.val[1] = portmap;
		pcetable.val[2] = tagmap;
		gsw_pce_table_write(cdev, &pcetable);
	} else {
		u8 pcindex;
		avlan_tbl_t avlantbl;
		pcindex = find_active_vlan_index(cdev,
						 parm->nVId);

		if (pcindex == 0xFF) {
			pr_err("(VID not exists) %s:%s:%d\n",
			       __FILE__, __func__, __LINE__);
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}

		if (pcindex >= 64) {
			ret = GSW_statusValueRange;
			goto UNLOCK_AND_RETURN;
		}

		get_vlan_sw_table(cdev, pcindex, &avlantbl);

		if (avlantbl.reserved == 1) {
			pr_err("(VID was already reserved) %s:%s:%d\n",
			       __FILE__, __func__, __LINE__);
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}

		if (parm->nPortId & 0x80000000) { /*Port Map */
			avlantbl.pm |= ((parm->nPortId) & 0x7FFF);

			if (parm->bVLAN_TagEgress)
				avlantbl.tm |= ((parm->nPortId) & 0x7FFF);
			else
				avlantbl.tm &= ~((parm->nPortId) & 0x7FFF);
		} else {
			avlantbl.pm |= 1 << parm->nPortId;

			if (parm->bVLAN_TagEgress)
				avlantbl.tm |= 1 << parm->nPortId;
			else
				avlantbl.tm &= ~(1 << parm->nPortId);
		}

		if (pcindex >= 64) {
			ret = GSW_statusValueRange;
			goto UNLOCK_AND_RETURN;
		}

		vlan_entry_set(cdev, pcindex, &avlantbl);
		pcetable.table = PCE_VLANMAP_INDEX;
		pcetable.pcindex = pcindex;
		gsw_pce_table_read(cdev, &pcetable);
		pcetable.table = PCE_VLANMAP_INDEX;
		pcetable.pcindex = pcindex;
		pcetable.val[1] = avlantbl.pm;
		pcetable.val[2] = avlantbl.tm;
		gsw_pce_table_write(cdev, &pcetable);
	}

	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_pce);
#endif
	return ret;
}

GSW_return_t GSW_VLAN_PortMemberRead(void *cdev,
				     GSW_VLAN_portMemberRead_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	pctbl_prog_t pcetable;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_pce);
#endif

	if (parm->bInitial == 1) {
		/*Start from the index 0 */
		gswdev->vlan_rd_index = 0;
		pcetable.table = PCE_VLANMAP_INDEX;
		pcetable.pcindex = gswdev->vlan_rd_index;
		gsw_pce_table_read(cdev, &pcetable);
		parm->nVId = gswdev->vlan_rd_index;
		/* Port Map */
		parm->nPortId = (pcetable.val[1] | 0x80000000);
		parm->nTagId = (pcetable.val[2] | 0x80000000);
		parm->bInitial = 0;
		parm->bLast = 0;
	}

	if (parm->bLast != 1) {
		if ((gswdev->gipver == LTQ_GSWIP_2_2_ETC) ||
		    (gswdev->gipver == LTQ_GSWIP_3_0)) {
			if (gswdev->vlan_rd_index < 4096) {
				gswdev->vlan_rd_index++;
				pcetable.table = PCE_VLANMAP_INDEX;
				pcetable.pcindex = gswdev->vlan_rd_index;
				gsw_pce_table_read(cdev, &pcetable);
				parm->nVId = gswdev->vlan_rd_index;
				/* Port Map */
				parm->nPortId = (pcetable.val[1] | 0x80000000);
				parm->nTagId = (pcetable.val[2] | 0x80000000);
			} else {
				parm->bLast = 1;
				gswdev->vlan_rd_index = 0;
			}
		} else {
			if (gswdev->vlan_rd_index < gswdev->avlantsz) {
				gswdev->vlan_rd_index++;
				pcetable.table = PCE_VLANMAP_INDEX;
				pcetable.pcindex = gswdev->vlan_rd_index;
				gsw_pce_table_read(cdev, &pcetable);
				parm->nVId = (pcetable.val[0] & 0xFFF);
				/* Port Map */
				parm->nPortId = (pcetable.val[1] | 0x80000000);
				parm->nTagId = (pcetable.val[2] | 0x80000000);
			} else {
				parm->bLast = 1;
				gswdev->vlan_rd_index = 0;
			}
		}
	}

	ret = GSW_statusOk;


#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_pce);
#endif
	return ret;
}

GSW_return_t GSW_VLAN_PortMemberRemove(void *cdev,
				       GSW_VLAN_portMemberRemove_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	pctbl_prog_t pcetable;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_pce);
#endif

	if ((parm->nPortId >= gswdev->tpnum) &&
	    (!(parm->nPortId & 0x80000000))) {
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	if ((gswdev->gipver == LTQ_GSWIP_2_2_ETC) ||
	    (gswdev->gipver == LTQ_GSWIP_3_0)) {
		u16  portmap, tagmap, val0;

		if (parm->nVId > 4096) {
			pr_err("ERROR: %s:%s:%d, (VID:%d)\n",
			       __FILE__, __func__, __LINE__, parm->nVId);
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}

		memset(&pcetable, 0, sizeof(pctbl_prog_t));
		pcetable.table = PCE_VLANMAP_INDEX;
		pcetable.pcindex = parm->nVId;
		gsw_pce_table_read(cdev, &pcetable);
		portmap	= (pcetable.val[1]);
		tagmap = (pcetable.val[2]);
		val0 = (pcetable.val[0]);

		if (parm->nPortId & 0x80000000)
			portmap  &= ~((parm->nPortId) & 0x7FFF);
		else
			portmap &= ~(1 << parm->nPortId);

		tagmap &= ~(1 << parm->nPortId);
		pcetable.table = PCE_VLANMAP_INDEX;
		pcetable.pcindex = parm->nVId;
		pcetable.val[0] = val0;
		pcetable.val[1] = portmap;
		pcetable.val[2] = tagmap;
		gsw_pce_table_write(cdev, &pcetable);
	} else {
		avlan_tbl_t avlantbl;
		u8 pcindex;
		pcindex = find_active_vlan_index(cdev,
						 parm->nVId);

		if (pcindex == 0xFF) {
			pr_err("This vid doesn't exists\n");
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}

		if (pcindex >= 64) {
			ret = GSW_statusValueRange;
			goto UNLOCK_AND_RETURN;
		}

		get_vlan_sw_table(cdev, pcindex, &avlantbl);

		if (parm->nPortId & 0x80000000)
			avlantbl.pm  &= ~((parm->nPortId) & 0x7FFF);
		else
			avlantbl.pm &= ~(1 << parm->nPortId);

		avlantbl.tm &= ~(1 << parm->nPortId);

		if (pcindex >= 64) {
			ret = GSW_statusValueRange;
			goto UNLOCK_AND_RETURN;
		}

		vlan_entry_set(cdev, pcindex, &avlantbl);
		pcetable.table = PCE_VLANMAP_INDEX;
		pcetable.pcindex = pcindex;
		gsw_pce_table_read(cdev, &pcetable);

		if (parm->nPortId & 0x80000000) {
			pcetable.val[1] &= ~((parm->nPortId) & 0x7FFF);
			pcetable.val[2] &= ~((parm->nPortId) & 0x7FFF);
		} else {
			pcetable.val[1] &= ~(1 << parm->nPortId);
			pcetable.val[2] &= ~(1 << parm->nPortId);
		}

		gsw_pce_table_write(cdev, &pcetable);
	}

	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_pce);
#endif
	return ret;
}

GSW_return_t GSW_VLAN_ReservedAdd(void *cdev,
				  GSW_VLAN_reserved_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	pctbl_prog_t pcetable;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_pce);
#endif

	if ((gswdev->gipver == LTQ_GSWIP_2_2_ETC) ||
	    (gswdev->gipver == LTQ_GSWIP_3_0)) {
		if (parm->nVId > 4096) {
			pr_err("ERROR: %s:%s:%d,(VID:%d)\n",
			       __FILE__, __func__, __LINE__, parm->nVId);
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}

		memset(&pcetable, 0, sizeof(pctbl_prog_t));
		pcetable.table = PCE_VLANMAP_INDEX;
		pcetable.pcindex = parm->nVId;
		gsw_pce_table_read(cdev, &pcetable);
		pcetable.val[0] |= (1 << 8);
		gsw_pce_table_write(cdev, &pcetable);
	} else {
		u8 pcindex;
		avlan_tbl_t avlantbl;
		pcindex = find_active_vlan_index(cdev,
						 parm->nVId);

		if (pcindex == 0xFF) {
			pr_err("(VID not exist, create VID) %s:%s:%d\n",
			       __FILE__, __func__, __LINE__);
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}

		if (pcindex >= 64) {
			ret = GSW_statusValueRange;
			goto UNLOCK_AND_RETURN;
		}

		get_vlan_sw_table(cdev, pcindex, &avlantbl);

		if (avlantbl.pm != 0) {
			pr_err("(Added to member & can't be reserve %s:%s:%d\n",
			       __FILE__, __func__, __LINE__);
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}

		if (avlantbl.tm != 0) {
			pr_err("(Added to member & can't be reserve %s:%s:%d\n",
			       __FILE__, __func__, __LINE__);
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}

		avlantbl.reserved = 1;

		if (pcindex >= 64) {
			ret = GSW_statusValueRange;
			goto UNLOCK_AND_RETURN;
		}

		vlan_entry_set(cdev, pcindex, &avlantbl);
		memset(&pcetable, 0, sizeof(pctbl_prog_t));
		pcetable.pcindex = pcindex;
		pcetable.table = PCE_ACTVLAN_INDEX;
		pcetable.val[0] |= (1 << 8);
		gsw_pce_table_write(cdev, &pcetable);
	}

	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_pce);
#endif
	return ret;

}

GSW_return_t GSW_VLAN_ReservedRemove(void *cdev,
				     GSW_VLAN_reserved_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	pctbl_prog_t pcetable;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_pce);
#endif

	if ((gswdev->gipver == LTQ_GSWIP_2_2_ETC) ||
	    (gswdev->gipver == LTQ_GSWIP_3_0)) {
		if (parm->nVId > 4096) {
			pr_err("ERROR: %s:%s:%d,(VID:%d)\n",
			       __FILE__, __func__, __LINE__, parm->nVId);
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}

		memset(&pcetable, 0, sizeof(pctbl_prog_t));
		pcetable.table = PCE_VLANMAP_INDEX;
		pcetable.pcindex = parm->nVId;
		gsw_pce_table_read(cdev, &pcetable);
		pcetable.val[0] &= ~(1 << 8);
		gsw_pce_table_write(cdev, &pcetable);
	} else {
		u8 pcindex;
		avlan_tbl_t avlantbl;
		pcindex = find_active_vlan_index(cdev, parm->nVId);

		if (pcindex == 0xFF) {
			pr_err("This vid doesn't exists, create VID first\n");
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}

		if (pcindex >= 64) {
			ret = GSW_statusValueRange;
			goto UNLOCK_AND_RETURN;
		}

		get_vlan_sw_table(cdev, pcindex, &avlantbl);

		if (avlantbl.pm != 0) {
			pr_err("(Added to member & can't be remove the reserve %s:%s:%d\n",
			       __FILE__, __func__, __LINE__);
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}

		if (avlantbl.tm != 0) {
			pr_err("(Added to member & can't be remove the reserve %s:%s:%d\n",
			       __FILE__, __func__, __LINE__);
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}

		if (pcindex >= 64) {
			ret = GSW_statusValueRange;
			goto UNLOCK_AND_RETURN;
		}

		if (avlantbl.reserved == 0) {
			pr_err("This VID was not reserve, reserve it first\n");
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		} else {
			avlantbl.reserved = 0;
			vlan_entry_set(cdev, pcindex, &avlantbl);
		}

		vlan_entry_set(cdev, pcindex, &avlantbl);
		memset(&pcetable, 0, sizeof(pctbl_prog_t));
		pcetable.pcindex = pcindex;
		pcetable.table = PCE_ACTVLAN_INDEX;
		pcetable.val[0] &= ~(1 << 8);
		gsw_pce_table_write(cdev, &pcetable);
	}

	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_pce);
#endif
	return ret;
}

GSW_return_t GSW_PCE_EG_VLAN_CfgSet(void *cdev,
				    GSW_PCE_EgVLAN_Cfg_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_pce);
#endif

	if (parm->nPortId >= gswdev->tpnum) {
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	if (gswdev->gipver == LTQ_GSWIP_3_0) {
		if (parm->eEgVLANmode ==
		    GSW_PCE_EG_VLAN_SUBIFID_BASED) {
			gsw_w32(cdev, (PCE_EVLANCFG_EGVMD_OFFSET
				       + (0x10 * parm->nPortId)),
				PCE_EVLANCFG_EGVMD_SHIFT,
				PCE_EVLANCFG_EGVMD_SIZE, 1);
		} else {
			gsw_w32(cdev, (PCE_EVLANCFG_EGVMD_OFFSET
				       + (0x10 * parm->nPortId)),
				PCE_EVLANCFG_EGVMD_SHIFT,
				PCE_EVLANCFG_EGVMD_SIZE, 0);
		}

		gsw_w32(cdev, (PCE_EVLANCFG_EGVFST_OFFSET
			       + (0x10 * parm->nPortId)),
			PCE_EVLANCFG_EGVFST_SHIFT,
			PCE_EVLANCFG_EGVFST_SIZE,
			parm->nEgStartVLANIdx);
	}

	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_pce);
#endif
	return ret;

}

GSW_return_t GSW_PCE_EG_VLAN_CfgGet(void *cdev,
				    GSW_PCE_EgVLAN_Cfg_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 value;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return  GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_pce);
#endif

	/*	if (parm->nPortId >= gswdev->tpnum) */
	/*		ret = GSW_statusErr; */
	if (gswdev->gipver == LTQ_GSWIP_3_0) {
		gsw_r32(cdev, (PCE_EVLANCFG_EGVMD_OFFSET
			       + (0x10 * parm->nPortId)),
			PCE_EVLANCFG_EGVMD_SHIFT,
			PCE_EVLANCFG_EGVMD_SIZE, &parm->eEgVLANmode);
		gsw_r32(cdev, (PCE_EVLANCFG_EGVFST_OFFSET
			       + (0x10 * parm->nPortId)),
			PCE_EVLANCFG_EGVFST_SHIFT,
			PCE_EVLANCFG_EGVFST_SIZE, &value);
		parm->nEgStartVLANIdx = value;
	}

	ret = GSW_statusOk;

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_pce);
#endif
	return ret;

}

GSW_return_t GSW_PCE_EG_VLAN_EntryWrite(void *cdev,
					GSW_PCE_EgVLAN_Entry_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	pctbl_prog_t pcetable;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_pce);
#endif

	/*	if (parm->nPortId >= gswdev->tpnum) */
	/*		ret = GSW_statusErr;*/
	if (gswdev->gipver == LTQ_GSWIP_3_0) {
		memset(&pcetable, 0, sizeof(pctbl_prog_t));
		pcetable.pcindex = (parm->nIndex & 0xFF);

		if (parm->bEgVLAN_Action == 1)
			pcetable.val[0] |= (1 << 0);

		if (parm->bEgSVidRem_Action == 1)
			pcetable.val[0] |= (1 << 2);

		if (parm->bEgSVidIns_Action == 1)
			pcetable.val[0] |= (1 << 3);

		pcetable.val[0] |= ((parm->nEgSVid & 0xFFF) << 4);

		if (parm->bEgCVidRem_Action == 1)
			pcetable.val[1] |= (1 << 2);

		if (parm->bEgCVidIns_Action == 1)
			pcetable.val[1] |= (1 << 3);

		pcetable.val[1] |= ((parm->nEgCVid & 0xFFF) << 4);
		pcetable.table = PCE_EG_VLAN_INDEX;
		gsw_pce_table_write(cdev, &pcetable);
	}

	ret = GSW_statusOk;

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_pce);
#endif
	return ret;

}

GSW_return_t GSW_PCE_EG_VLAN_EntryRead(void *cdev,
				       GSW_PCE_EgVLAN_Entry_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	pctbl_prog_t pcetable;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_pce);
#endif

	/*if (parm->nPortId >= gswdev->tpnum) */
	/*	ret = GSW_statusErr; */
	if (gswdev->gipver == LTQ_GSWIP_3_0) {
		memset(&pcetable, 0, sizeof(pctbl_prog_t));
		pcetable.pcindex = (parm->nIndex & 0xFF);
		pcetable.table = PCE_EG_VLAN_INDEX;
		gsw_pce_table_read(cdev, &pcetable);
		parm->bEgVLAN_Action = pcetable.val[0] & 0x1;
		parm->bEgSVidRem_Action = (pcetable.val[0] >> 2) & 0x1;
		parm->bEgSVidIns_Action = (pcetable.val[0] >> 3) & 0x1;
		parm->nEgSVid = (pcetable.val[0] >> 4) & 0xFFF;
		parm->bEgCVidRem_Action = (pcetable.val[1] >> 2) & 0x1;
		parm->bEgCVidIns_Action = (pcetable.val[1] >> 3) & 0x1;
		parm->nEgCVid = (pcetable.val[1] >> 4) & 0xFFF;
	}

	gsw_w32(cdev, PCE_TBL_CTRL_ADDR_OFFSET, 0, 16, 0);

	ret = GSW_statusOk;

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_pce);
#endif
	return ret;

}

GSW_return_t GSW_SVLAN_CfgGet(void *cdev,
			      GSW_SVLAN_cfg_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 reg_val;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_misc);
#endif

	if ((gswdev->gipver == LTQ_GSWIP_2_2_ETC)
	    || (gswdev->gipver == LTQ_GSWIP_3_0)) {
		gsw_r32(cdev, (FDMA_SVTETYPE_OFFSET),
			FDMA_SVTETYPE_ETYPE_SHIFT,
			FDMA_SVTETYPE_ETYPE_SIZE, &reg_val);
		parm->nEthertype = reg_val;
	}

	ret = GSW_statusOk;

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_misc);
#endif
	return ret;

}

GSW_return_t GSW_SVLAN_CfgSet(void *cdev,
			      GSW_SVLAN_cfg_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 reg_val;
	u32 ret;
	reg_val = parm->nEthertype & 0xFFFF;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_misc);
#endif

	if ((gswdev->gipver == LTQ_GSWIP_2_2_ETC) ||
	    (gswdev->gipver == LTQ_GSWIP_3_0)) {
		gsw_w32(cdev, (FDMA_SVTETYPE_OFFSET),
			FDMA_SVTETYPE_ETYPE_SHIFT,
			FDMA_SVTETYPE_ETYPE_SIZE, reg_val);
		gsw_w32(cdev, (MAC_VLAN_ETYPE_1_INNER_OFFSET),
			MAC_VLAN_ETYPE_1_INNER_SHIFT,
			MAC_VLAN_ETYPE_1_INNER_SIZE, reg_val);
		/* ToDo: Update the Micro code based on SVAN*/
	}

	ret = GSW_statusOk;

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_misc);
#endif
	return ret;

}

GSW_return_t GSW_SVLAN_PortCfgGet(void *cdev,
				  GSW_SVLAN_portCfg_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 value;
	ltq_bool_t svimr, svemr;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_misc);
#endif

	if (parm->nPortId >= gswdev->tpnum) {
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	/** nPortVId: retrieve the corresponding VLAN ID */
	/* from the Active VLAN Table*/
	if ((gswdev->gipver == LTQ_GSWIP_2_2_ETC) ||
	    (gswdev->gipver == LTQ_GSWIP_3_0)) {
		gsw_r32(cdev, (PCE_DEFPSVID_PVID_OFFSET +
			       (2 * parm->nPortId)),
			PCE_DEFPSVID_PVID_SHIFT,
			PCE_DEFPSVID_PVID_SIZE, &value);
		parm->nPortVId = value;
		/* bSVLAN_TagSupport */
		gsw_r32(cdev, (PCE_VCTRL_STEN_OFFSET +
			       (10 * parm->nPortId)),
			PCE_VCTRL_STEN_SHIFT,
			PCE_VCTRL_STEN_SIZE, &value);
		parm->bSVLAN_TagSupport = value;

		/** bVLAN_ReAssign */
		gsw_r32(cdev, (PCE_VCTRL_SVSR_OFFSET +
			       (10 * parm->nPortId)),
			PCE_VCTRL_SVSR_SHIFT,
			PCE_VCTRL_SVSR_SIZE, &value);
		parm->bVLAN_ReAssign = value;

		/** bVlanMemberViolationIngress */
		gsw_r32(cdev, (PCE_VCTRL_SVIMR_OFFSET
			       + (10 * parm->nPortId)),
			PCE_VCTRL_SVIMR_SHIFT,
			PCE_VCTRL_SVIMR_SIZE, &value);
		svimr = value;
		/** bVlanMemberViolationEgress */
		gsw_r32(cdev, (PCE_VCTRL_SVEMR_OFFSET
			       + (10 * parm->nPortId)),
			PCE_VCTRL_SVEMR_SHIFT,
			PCE_VCTRL_SVEMR_SIZE, &value);
		svemr = value;

		if (svimr == 0 && svemr == 0)
			parm->eVLAN_MemberViolation =
				GSW_VLAN_MEMBER_VIOLATION_NO;

		if (svimr == 1 && svemr == 0)
			parm->eVLAN_MemberViolation =
				GSW_VLAN_MEMBER_VIOLATION_INGRESS;

		if (svimr == 0 && svemr == 1)
			parm->eVLAN_MemberViolation =
				GSW_VLAN_MEMBER_VIOLATION_EGRESS;

		if (svimr == 1 && svemr == 1)
			parm->eVLAN_MemberViolation =
				GSW_VLAN_MEMBER_VIOLATION_BOTH;

		/* eAdmitMode:  */
		gsw_r32(cdev, (PCE_VCTRL_SVINR_OFFSET
			       + (10 * parm->nPortId)),
			PCE_VCTRL_SVINR_SHIFT,
			PCE_VCTRL_SVINR_SIZE, &value);

		switch (value) {
		case 0:
			parm->eAdmitMode = GSW_VLAN_ADMIT_ALL;
			break;

		case 1:
			parm->eAdmitMode = GSW_VLAN_ADMIT_TAGGED;
			break;

		case 2:
			parm->eAdmitMode = GSW_VLAN_ADMIT_UNTAGGED;
			break;

		default:
			break;
		} /* -----  end switch  ----- */

		/** bSVLAN_MACbasedTag */
		gsw_r32(cdev, (PCE_VCTRL_MACEN_OFFSET +
			       (10 * parm->nPortId)),
			PCE_VCTRL_MACEN_SHIFT,
			PCE_VCTRL_MACEN_SIZE, &value);
		parm->bSVLAN_MACbasedTag = value;
	}

	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_misc);
#endif
	return ret;

}

GSW_return_t GSW_SVLAN_PortCfgSet(void *cdev,
				  GSW_SVLAN_portCfg_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 value;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_misc);
#endif

	if (parm->nPortId >= gswdev->tpnum) {
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	if ((gswdev->gipver == LTQ_GSWIP_2_2_ETC) ||
	    (gswdev->gipver == LTQ_GSWIP_3_0)) {
		value = parm->nPortVId;
		gsw_w32(cdev, (PCE_DEFPSVID_PVID_OFFSET +
			       (2 * parm->nPortId)),
			PCE_DEFPSVID_PVID_SHIFT,
			PCE_DEFPSVID_PVID_SIZE, value);
		/* bSVLAN_TagSupport */
		value = parm->bSVLAN_TagSupport;
		gsw_w32(cdev, (PCE_VCTRL_STEN_OFFSET +
			       (10 * parm->nPortId)),
			PCE_VCTRL_STEN_SHIFT,
			PCE_VCTRL_STEN_SIZE, value);

		if (parm->bSVLAN_TagSupport == 1) {
			gsw_w32(cdev, (FDMA_PCTRL_SVLANMOD_OFFSET +
				       (6 * parm->nPortId)),
				FDMA_PCTRL_SVLANMOD_SHIFT,
				FDMA_PCTRL_SVLANMOD_SIZE, 3);
		} else {
			gsw_w32(cdev, (FDMA_PCTRL_SVLANMOD_OFFSET +
				       (6 * parm->nPortId)),
				FDMA_PCTRL_SVLANMOD_SHIFT,
				FDMA_PCTRL_SVLANMOD_SIZE, 0);
		}

		/** bVLAN_ReAssign */
		value = parm->bVLAN_ReAssign;
		gsw_w32(cdev, (PCE_VCTRL_SVSR_OFFSET +
			       (10 * parm->nPortId)),
			PCE_VCTRL_SVSR_SHIFT,
			PCE_VCTRL_SVSR_SIZE, value);

		/** eVLAN_MemberViolation  */
		switch (parm->eVLAN_MemberViolation) {
		case GSW_VLAN_MEMBER_VIOLATION_NO:
			gsw_w32(cdev, (PCE_VCTRL_SVIMR_OFFSET +
				       (10 * parm->nPortId)),
				PCE_VCTRL_SVIMR_SHIFT,
				PCE_VCTRL_SVIMR_SIZE, 0);
			gsw_w32(cdev, (PCE_VCTRL_SVEMR_OFFSET +
				       (10 * parm->nPortId)),
				PCE_VCTRL_SVEMR_SHIFT,
				PCE_VCTRL_SVEMR_SIZE, 0);
			break;

		case GSW_VLAN_MEMBER_VIOLATION_INGRESS:
			gsw_w32(cdev, (PCE_VCTRL_SVIMR_OFFSET +
				       (10 * parm->nPortId)),
				PCE_VCTRL_SVIMR_SHIFT,
				PCE_VCTRL_SVIMR_SIZE, 1);
			gsw_w32(cdev, (PCE_VCTRL_SVEMR_OFFSET +
				       (10 * parm->nPortId)),
				PCE_VCTRL_SVEMR_SHIFT,
				PCE_VCTRL_SVEMR_SIZE, 0);
			break;

		case GSW_VLAN_MEMBER_VIOLATION_EGRESS:
			gsw_w32(cdev, (PCE_VCTRL_SVIMR_OFFSET +
				       (10 * parm->nPortId)),
				PCE_VCTRL_SVIMR_SHIFT,
				PCE_VCTRL_SVIMR_SIZE, 0);
			gsw_w32(cdev, (PCE_VCTRL_SVEMR_OFFSET +
				       (10 * parm->nPortId)),
				PCE_VCTRL_SVEMR_SHIFT,
				PCE_VCTRL_SVEMR_SIZE, 1);
			break;

		case GSW_VLAN_MEMBER_VIOLATION_BOTH:
			gsw_w32(cdev, (PCE_VCTRL_SVIMR_OFFSET +
				       (10 * parm->nPortId)),
				PCE_VCTRL_SVIMR_SHIFT,
				PCE_VCTRL_SVIMR_SIZE, 1);
			gsw_w32(cdev, (PCE_VCTRL_SVEMR_OFFSET +
				       (10 * parm->nPortId)),
				PCE_VCTRL_SVEMR_SHIFT,
				PCE_VCTRL_SVEMR_SIZE, 1);
			break;
		} /* -----  end switch  ----- */

		/** eAdmitMode */
		switch (parm->eAdmitMode) {
		case GSW_VLAN_ADMIT_ALL:
			value = 0;
			break;

		case GSW_VLAN_ADMIT_TAGGED:
			value = 1;
			break;

		case GSW_VLAN_ADMIT_UNTAGGED:
			value = 2;
			break;

		default:
			value = 0;
		} /* -----  end switch  ----- */

		gsw_w32(cdev, (PCE_VCTRL_SVINR_OFFSET +
			       (10 * parm->nPortId)),
			PCE_VCTRL_SVINR_SHIFT,
			PCE_VCTRL_SVINR_SIZE, value);
		/** bSVLAN_MACbasedTag */
		value = parm->bSVLAN_MACbasedTag;
		gsw_w32(cdev, (PCE_VCTRL_MACEN_OFFSET +
			       (10 * parm->nPortId)),
			PCE_VCTRL_MACEN_SHIFT,
			PCE_VCTRL_MACEN_SIZE, value);
	}

	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_misc);
#endif
	return ret;
}

GSW_return_t GSW_QoS_SVLAN_ClassPCP_PortGet(void *cdev,
		GSW_QoS_SVLAN_ClassPCP_PortCfg_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	pctbl_prog_t pcetable;
	u32  value, dei;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_pce);
#endif

	if (parm->nPortId >= gswdev->tpnum) {
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	if ((gswdev->gipver == LTQ_GSWIP_2_2_ETC) ||
	    (gswdev->gipver == LTQ_GSWIP_3_0)) {
		for (value = 0; value < 16; value++) {
			memset(&pcetable, 0, sizeof(pctbl_prog_t));
			pcetable.pcindex = (((parm->nPortId & 0xF) << 4)
					    | (value));
			pcetable.table = PCE_EGREMARK_INDEX;
			gsw_pce_table_read(cdev, &pcetable);
			parm->nDSCP[value] = (pcetable.val[0] & 0x3F);
			parm->nCPCP[value] = ((pcetable.val[0] >> 8) & 0x7);
			parm->nSPCP[value] = ((pcetable.val[1] >> 8) & 0x7);
			dei = ((pcetable.val[1]) & 0x1);
			parm->nSPCP[value]	|= (dei << 7);
		}
	}

	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_pce);
#endif
	return ret;

}

GSW_return_t GSW_QoS_SVLAN_ClassPCP_PortSet(void *cdev,
		GSW_QoS_SVLAN_ClassPCP_PortCfg_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	pctbl_prog_t pcetable;
	u32  value;
	u32 ret;
	u8 cpcp, dscp, spcp, dei;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_pce);
#endif

	if (parm->nPortId >= gswdev->tpnum) {
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	if ((gswdev->gipver == LTQ_GSWIP_2_2_ETC) ||
	    (gswdev->gipver == LTQ_GSWIP_3_0)) {
		for (value = 0; value < 16; value++) {
			memset(&pcetable, 0, sizeof(pctbl_prog_t));
			pcetable.pcindex = (((parm->nPortId & 0xF) << 4)
					    | (value));
			dscp = parm->nDSCP[value] & 0x3F;
			spcp = parm->nSPCP[value] & 0x7;
			cpcp = parm->nCPCP[value] & 0x7;
			dei = ((parm->nSPCP[value] >> 7) & 1);
			pcetable.val[1] = ((spcp << 8) | dei);
			pcetable.val[0] = (dscp | (cpcp << 8));
			pcetable.table = PCE_EGREMARK_INDEX;
			gsw_pce_table_write(cdev, &pcetable);
		}
	}

	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_pce);
#endif
	return ret;

}

GSW_return_t GSW_QoS_SVLAN_PCP_ClassGet(void *cdev,
					GSW_QoS_SVLAN_PCP_ClassCfg_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	pctbl_prog_t ptbl;
	u32  value;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_pce);
#endif

	if ((gswdev->gipver == LTQ_GSWIP_2_2_ETC) ||
	    (gswdev->gipver == LTQ_GSWIP_3_0) ||
	    (IS_VRSN_31(gswdev->gipver))) {
		memset(&ptbl, 0, sizeof(pctbl_prog_t));

		for (value = 0; value < 16; value++) {
			ptbl.table = PCE_SPCP_INDEX;
			ptbl.pcindex = value;
			gsw_pce_table_read(cdev, &ptbl);
			parm->nTrafficClass[value] = ptbl.val[0] & 0xF;

			/*Following are not applicable for GSWIP 3.1*/
			if (IS_VRSN_NOT_31(gswdev->gipver)) {
				parm->nTrafficColor[value] = ((ptbl.val[0] >> 6) & 0x3);
				parm->nPCP_Remark_Enable[value] = ((ptbl.val[0] >> 4) & 0x1);
				parm->nDEI_Remark_Enable[value] = ((ptbl.val[0] >> 5) & 0x1);
			}
		}
	}

	ret = GSW_statusOk;

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_pce);
#endif
	return ret;

}

GSW_return_t GSW_QoS_SVLAN_PCP_ClassSet(void *cdev,
					GSW_QoS_SVLAN_PCP_ClassCfg_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_pce);
#endif

	if ((gswdev->gipver == LTQ_GSWIP_2_2_ETC) ||
	    (gswdev->gipver == LTQ_GSWIP_3_0) ||
	    (IS_VRSN_31(gswdev->gipver))) {
		pctbl_prog_t pcetable;
		u32  value;

		for (value = 0; value < 16; value++) {
			memset(&pcetable, 0, sizeof(pctbl_prog_t));
			pcetable.table = PCE_SPCP_INDEX;
			pcetable.pcindex = value;
			pcetable.val[0] = parm->nTrafficClass[value] & 0xF;

			/*Following are not applicable for GSWIP 3.1*/
			if (IS_VRSN_NOT_31(gswdev->gipver)) {
				pcetable.val[0] |=
					(parm->nTrafficColor[value] & 0x3) << 6;
				pcetable.val[0] |=
					(parm->nPCP_Remark_Enable[value] & 0x1) << 4;
				pcetable.val[0] |=
					(parm->nDEI_Remark_Enable[value] & 0x1) << 5;
			}

			gsw_pce_table_write(cdev, &pcetable);
		}
	}

	ret = GSW_statusOk;

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_pce);
#endif
	return ret;
}

#endif /*CONFIG_LTQ_VLAN */


#if defined(CONFIG_LTQ_QOS) && CONFIG_LTQ_QOS

GSW_return_t GSW_QoS_MeterCfgGet(void *cdev,
				 GSW_QoS_meterCfg_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 mid = parm->nMeterId, value, exp, mant, ibs;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_misc);
#endif

	if (IS_VRSN_30_31(gswdev->gipver)) {
		if (mid > gswdev->num_of_meters) {
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}

		gsw_w32(cdev, GSW_INST_SEL_INST_OFFSET,
			GSW_INST_SEL_INST_SHIFT,
			GSW_INST_SEL_INST_SIZE, mid);
		/* Enable/Disable the meter shaper */
		gsw_r32(cdev, GSW_PCE_TCM_CTRL_TCMEN_OFFSET,
			GSW_PCE_TCM_CTRL_TCMEN_SHIFT,
			GSW_PCE_TCM_CTRL_TCMEN_SIZE, &value);
		parm->bEnable = value;
		/* Committed Burst Size */
		gsw_r32(cdev, GSW_PCE_TCM_CBS_CBS_OFFSET,
			GSW_PCE_TCM_CBS_CBS_SHIFT,
			GSW_PCE_TCM_CBS_CBS_SIZE, &value);
		parm->nCbs = (value * 64);
		/* Excess Burst Size (EBS [bytes]) */
		gsw_r32(cdev, GSW_PCE_TCM_EBS_EBS_OFFSET,
			GSW_PCE_TCM_EBS_EBS_SHIFT,
			GSW_PCE_TCM_EBS_EBS_SIZE, &value);
		parm->nEbs = (value * 64);
		/* Rate Counter Exponent */
		gsw_r32(cdev, GSW_PCE_TCM_CIR_EXP_EXP_OFFSET,
			GSW_PCE_TCM_CIR_EXP_EXP_SHIFT,
			GSW_PCE_TCM_CIR_EXP_EXP_SIZE, &exp);
		/* Rate Counter Mantissa */
		gsw_r32(cdev, GSW_PCE_TCM_CIR_MANT_MANT_OFFSET,
			GSW_PCE_TCM_CIR_MANT_MANT_SHIFT,
			GSW_PCE_TCM_CIR_MANT_MANT_SIZE, &mant);

		/* Rate Counter iBS */
		if (IS_VRSN_31(gswdev->gipver))
			gsw_r32(cdev, GSW_PCE_TCM_IBS_IBS_OFFSET,
				GSW_PCE_TCM_IBS_IBS_SHIFT,
				GSW_PCE_TCM_IBS_IBS_SIZE_GSWIP_3_1, &ibs);
		else
			gsw_r32(cdev, GSW_PCE_TCM_IBS_IBS_OFFSET,
				GSW_PCE_TCM_IBS_IBS_SHIFT,
				GSW_PCE_TCM_IBS_IBS_SIZE, &ibs);

		/* calc the Rate */
		if (IS_VRSN_31(gswdev->gipver))
			parm->nRate = mratecalc_3_1(ibs, exp, mant);
		else
			parm->nRate = mratecalc(ibs, exp, mant);

		/* Rate Counter Exponent */
		gsw_r32(cdev, GSW_PCE_TCM_PIR_EXP_EXP_OFFSET,
			GSW_PCE_TCM_PIR_EXP_EXP_SHIFT,
			GSW_PCE_TCM_PIR_EXP_EXP_SIZE, &exp);
		/* Rate Counter Mantissa */
		gsw_r32(cdev, GSW_PCE_TCM_PIR_MANT_MANT_OFFSET,
			GSW_PCE_TCM_PIR_MANT_MANT_SHIFT,
			GSW_PCE_TCM_PIR_MANT_MANT_SIZE, &mant);

		/* Rate Counter iBS */
		if (IS_VRSN_31(gswdev->gipver))
			gsw_r32(cdev, GSW_PCE_TCM_IBS_IBS_OFFSET,
				GSW_PCE_TCM_IBS_IBS_SHIFT,
				GSW_PCE_TCM_IBS_IBS_SIZE_GSWIP_3_1, &ibs);
		else
			gsw_r32(cdev, GSW_PCE_TCM_IBS_IBS_OFFSET,
				GSW_PCE_TCM_IBS_IBS_SHIFT,
				GSW_PCE_TCM_IBS_IBS_SIZE, &ibs);

		/* calc the Rate */
		if (IS_VRSN_31(gswdev->gipver))
			parm->nPiRate = mratecalc_3_1(ibs, exp, mant);
		else
			parm->nPiRate = mratecalc(ibs, exp, mant);

		/* parm->nPbs=??? how to calculate it*/
		gsw_r32(cdev, GSW_PCE_TCM_CTRL_TMOD_OFFSET,
			GSW_PCE_TCM_CTRL_TMOD_SHIFT,
			GSW_PCE_TCM_CTRL_TMOD_SIZE, &parm->eMtrType);

		if (IS_VRSN_31(gswdev->gipver))
			gsw_r32(cdev, GSW_PCE_TCM_CTRL_BLIND_OFFSET,
				GSW_PCE_TCM_CTRL_BLIND_SHIFT,
				GSW_PCE_TCM_CTRL_BLIND_SIZE, &parm->nColourBlindMode);
	} else {
		if (mid > 7) {
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}

		/* Enable/Disable the meter shaper */
		gsw_r32(cdev, (PCE_TCM_CTRL_TCMEN_OFFSET + (mid * 7)),
			PCE_TCM_CTRL_TCMEN_SHIFT,
			PCE_TCM_CTRL_TCMEN_SIZE, &value);
		parm->bEnable = value;
		/* Committed Burst Size */
		gsw_r32(cdev, (PCE_TCM_CBS_CBS_OFFSET + (mid * 7)),
			PCE_TCM_CBS_CBS_SHIFT,
			PCE_TCM_CBS_CBS_SIZE, &value);
		parm->nCbs = (value * 64);
		/* Excess Burst Size (EBS [bytes]) */
		gsw_r32(cdev, (PCE_TCM_EBS_EBS_OFFSET + (mid * 7)),
			PCE_TCM_EBS_EBS_SHIFT,
			PCE_TCM_EBS_EBS_SIZE, &value);
		parm->nEbs = (value * 64);
		/* Rate Counter Exponent */
		gsw_r32(cdev, (PCE_TCM_CIR_EXP_EXP_OFFSET + (mid * 7)),
			PCE_TCM_CIR_EXP_EXP_SHIFT,
			PCE_TCM_CIR_EXP_EXP_SIZE, &exp);
		/* Rate Counter Mantissa */
		gsw_r32(cdev, (PCE_TCM_CIR_MANT_MANT_OFFSET + (mid * 7)),
			PCE_TCM_CIR_MANT_MANT_SHIFT,
			PCE_TCM_CIR_MANT_MANT_SIZE, &mant);
		/* Rate Counter iBS */
		gsw_r32(cdev, (PCE_TCM_IBS_IBS_OFFSET + (mid * 7)),
			PCE_TCM_IBS_IBS_SHIFT,
			PCE_TCM_IBS_IBS_SIZE, &ibs);
		/* calc the Rate */
		parm->nRate = mratecalc(ibs, exp, mant);
	}

	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_misc);
#endif
	return ret;

}

GSW_return_t GSW_QoS_MeterCfgSet(void *cdev,
				 GSW_QoS_meterCfg_t *parm)
{
	u32 mid, cbs, ebs, exp = 0, mant = 0, rate, ibs = 0;
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_misc);
#endif

	/*Change summary of GSW_QoS_MeterCfgSet for GSWIP 3.1
		1. Meter Color Blind is newly added
		2. IBS is byte based
	*/

	mid = parm->nMeterId;

	if (IS_VRSN_30_31(gswdev->gipver)) {
		if (mid >= gswdev->num_of_meters) {
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}


		if (IS_VRSN_31(gswdev->gipver)) {
			/*Check whether it is InUSE,if not InUse,return ERROR*/
			if (!(gswdev->meter_idx[mid].IndexInUse)) {
				ret = GSW_statusErr;
				goto UNLOCK_AND_RETURN;
			}
		}

		gsw_w32(cdev, GSW_INST_SEL_INST_OFFSET,
			GSW_INST_SEL_INST_SHIFT,
			GSW_INST_SEL_INST_SIZE, mid);

		/* Committed Burst Size */
		if (parm->nCbs > 0xFFC0)
			cbs = 0x3FF;
		else
			cbs = ((parm->nCbs + 63) / 64);

		gsw_w32(cdev, GSW_PCE_TCM_CBS_CBS_OFFSET,
			GSW_PCE_TCM_CBS_CBS_SHIFT,
			GSW_PCE_TCM_CBS_CBS_SIZE, cbs);

		/* Excess Burst Size (EBS [bytes]) */
		if (parm->nEbs > 0xFFC0)
			ebs = 0x3FF;
		else
			ebs = ((parm->nEbs + 63) / 64);

		gsw_w32(cdev, GSW_PCE_TCM_EBS_EBS_OFFSET,
			GSW_PCE_TCM_EBS_EBS_SHIFT,
			GSW_PCE_TCM_EBS_EBS_SIZE, ebs);
		/* Calc the Rate and convert to MANT and EXP*/
		rate = parm->nRate;

		if (rate) {
			if (IS_VRSN_31(gswdev->gipver))
				calc_mtoken_3_1(rate, &ibs, &exp, &mant);
			else
				calc_mtoken(rate, &ibs, &exp, &mant);
		}

		/* Rate Counter Exponent */
		gsw_w32(cdev, GSW_PCE_TCM_CIR_EXP_EXP_OFFSET,
			GSW_PCE_TCM_CIR_EXP_EXP_SHIFT,
			GSW_PCE_TCM_CIR_EXP_EXP_SIZE, exp);
		/* Rate Counter Mantissa */
		gsw_w32(cdev, GSW_PCE_TCM_CIR_MANT_MANT_OFFSET,
			GSW_PCE_TCM_CIR_MANT_MANT_SHIFT,
			GSW_PCE_TCM_CIR_MANT_MANT_SIZE, mant);

		/* Rate Counter iBS */
		if (IS_VRSN_31(gswdev->gipver))
			gsw_w32(cdev, GSW_PCE_TCM_IBS_IBS_OFFSET,
				GSW_PCE_TCM_IBS_IBS_SHIFT,
				GSW_PCE_TCM_IBS_IBS_SIZE_GSWIP_3_1, ibs);
		else
			gsw_w32(cdev, GSW_PCE_TCM_IBS_IBS_OFFSET,
				GSW_PCE_TCM_IBS_IBS_SHIFT,
				GSW_PCE_TCM_IBS_IBS_SIZE_GSWIP_3_1, ibs);

		/* Calc the Rate and convert to MANT and EXP*/
		rate = parm->nPiRate;

		if (rate) {
			if (IS_VRSN_31(gswdev->gipver))
				calc_mtoken_3_1(rate, &ibs, &exp, &mant);
			else
				calc_mtoken(rate, &ibs, &exp, &mant);
		}

		/* Rate Counter Exponent */
		gsw_w32(cdev, GSW_PCE_TCM_PIR_EXP_EXP_OFFSET,
			GSW_PCE_TCM_PIR_EXP_EXP_SHIFT,
			GSW_PCE_TCM_PIR_EXP_EXP_SIZE, exp);
		/* Rate Counter Mantissa */
		gsw_w32(cdev, GSW_PCE_TCM_PIR_MANT_MANT_OFFSET,
			GSW_PCE_TCM_PIR_MANT_MANT_SHIFT,
			GSW_PCE_TCM_PIR_MANT_MANT_SIZE, mant);
		/* Rate Counter iBS */
		/*		gsw_w32(cdev, GSW_PCE_TCM_IBS_IBS_OFFSET, */
		/*			GSW_PCE_TCM_IBS_IBS_SHIFT,*/
		/*			GSW_PCE_TCM_IBS_IBS_SIZE, ibs);*/
		gsw_w32(cdev, GSW_PCE_TCM_CTRL_TMOD_OFFSET,
			GSW_PCE_TCM_CTRL_TMOD_SHIFT,
			GSW_PCE_TCM_CTRL_TMOD_SIZE, parm->eMtrType);
		/* Enable/Disable the meter shaper */
		gsw_w32(cdev, GSW_PCE_TCM_CTRL_TCMEN_OFFSET,
			GSW_PCE_TCM_CTRL_TCMEN_SHIFT,
			GSW_PCE_TCM_CTRL_TCMEN_SIZE, parm->bEnable);

		if (IS_VRSN_31(gswdev->gipver))
			gsw_w32(cdev, GSW_PCE_TCM_CTRL_BLIND_OFFSET,
				GSW_PCE_TCM_CTRL_BLIND_SHIFT,
				GSW_PCE_TCM_CTRL_BLIND_SIZE, parm->nColourBlindMode);
	} else {
		if (mid > 7) {
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}

		/* Committed Burst Size */
		if (parm->nCbs > 0xFFC0)
			cbs = 0x3FF;
		else
			cbs = ((parm->nCbs + 63) / 64);

		gsw_w32(cdev, (PCE_TCM_CBS_CBS_OFFSET +
			       (mid * 7)),
			PCE_TCM_CBS_CBS_SHIFT,
			PCE_TCM_CBS_CBS_SIZE, cbs);

		/* Excess Burst Size (EBS [bytes]) */
		if (parm->nEbs > 0xFFC0)
			ebs = 0x3FF;
		else
			ebs = ((parm->nEbs + 63) / 64);

		gsw_w32(cdev, (PCE_TCM_EBS_EBS_OFFSET +
			       (mid * 7)),
			PCE_TCM_EBS_EBS_SHIFT,
			PCE_TCM_EBS_EBS_SIZE, ebs);
		/* Calc the Rate and convert to MANT and EXP*/
		rate = parm->nRate;

		if (rate)
			calc_mtoken(rate, &ibs, &exp, &mant);

		/* Rate Counter Exponent */
		gsw_w32(cdev, (PCE_TCM_CIR_EXP_EXP_OFFSET +
			       (mid * 7)),
			PCE_TCM_CIR_EXP_EXP_SHIFT,
			PCE_TCM_CIR_EXP_EXP_SIZE, exp);
		/* Rate Counter Mantissa */
		gsw_w32(cdev, (PCE_TCM_CIR_MANT_MANT_OFFSET +
			       (mid * 7)),
			PCE_TCM_CIR_MANT_MANT_SHIFT,
			PCE_TCM_CIR_MANT_MANT_SIZE, mant);
		/* Rate Counter iBS */
		gsw_w32(cdev, (PCE_TCM_IBS_IBS_OFFSET +
			       (mid * 7)),
			PCE_TCM_IBS_IBS_SHIFT,
			PCE_TCM_IBS_IBS_SIZE, ibs);
		/* Enable/Disable the meter shaper */
		gsw_w32(cdev, (PCE_TCM_CTRL_TCMEN_OFFSET +
			       (mid * 7)),
			PCE_TCM_CTRL_TCMEN_SHIFT,
			PCE_TCM_CTRL_TCMEN_SIZE, parm->bEnable);
	}

	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_misc);
#endif
	return ret;

}

GSW_return_t GSW_QoS_MeterPortAssign(void *cdev,
				     GSW_QoS_meterPort_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	pctbl_prog_t pcetable;
	u32 ret;
	u32 eport, iport, value1, value2;
	ltq_bool_t eftbl = 0, nempftbl = 0;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_pce);
#endif

	if (IS_VRSN_31(gswdev->gipver)) {
		/* GSW_QoS_MeterPortAssign should not be called for GSWIP3.1
			Meter should be assigned during CTP/Bridge Port Config*/
		ret = GSW_statusNoSupport;
		goto UNLOCK_AND_RETURN;
	}

	memset(&pcetable, 0, sizeof(pctbl_prog_t));
	value1 = 0;

	while (value1 < 8) {
		pcetable.table = PCE_METER_INS_0_INDEX;
		pcetable.pcindex = value1;
		gsw_pce_table_read(cdev, &pcetable);

		if (pcetable.valid == 1) {
			iport = pcetable.key[0] & 0xF;
			eport = (pcetable.key[0] >> 8) & 0xF;

			if ((eport == parm->nPortEgressId) &&
			    (iport == parm->nPortIngressId)) {
				eftbl = 1;
				value2 = 0;

				while (value2 < 8) {
					pcetable.table = PCE_METER_INS_1_INDEX;
					pcetable.pcindex = value2;
					gsw_pce_table_read(cdev, &pcetable);

					if (pcetable.valid == 1) {
						iport = pcetable.key[0] & 0xF;
						eport = ((pcetable.key[0] >> 8) & 0xF);

						if ((eport == parm->nPortEgressId) &&
						    (iport == parm->nPortIngressId)) {
							ret = GSW_statusErr;
							goto UNLOCK_AND_RETURN;
						}
					}

					value2++;
				}
			}
		}

		value1++;
	}

	/*  Not in the original table, write a new one */
	if (eftbl == 0) {
		value1 = 0;
		memset(&pcetable, 0, sizeof(pctbl_prog_t));

		/* Search for whole Table 1*/
		while (value1 < 8) {
			pcetable.table = PCE_METER_INS_0_INDEX;
			pcetable.pcindex = value1;
			gsw_pce_table_read(cdev, &pcetable);

			/* We found the empty one */
			if (pcetable.valid == 0) {
				switch (parm->eDir) {
				case GSW_DIRECTION_BOTH:
					pcetable.key[0] =
						(((parm->nPortEgressId & 0xF) << 8)
						 | (parm->nPortIngressId & 0xF));
					pcetable.mask[0] = 0;
					break;

				case GSW_DIRECTION_EGRESS:
					pcetable.key[0] =
						(((parm->nPortEgressId & 0xF) << 8)
						 | 0xF);
					pcetable.mask[0] = 1;
					break;

				case GSW_DIRECTION_INGRESS:
					pcetable.key[0] = (0xF00 |
							   (parm->nPortIngressId & 0xF));
					pcetable.mask[0] = 4;
					break;

				default:
					pcetable.key[0] = 0;
					pcetable.mask[0] = 5;
				}

				pcetable.val[0] = parm->nMeterId & 0x3F;
				pcetable.valid = 1;
				gsw_pce_table_write(cdev, &pcetable);
				ret = GSW_statusOk;
				goto UNLOCK_AND_RETURN;
			}

			value1++;
		}

		if (value1 >= 8)
			nempftbl = 1;
	}

	/* The Table 1 is full, We go search table 2 */
	if ((nempftbl == 1) || (eftbl == 1)) {
		value2 = 0;
		memset(&pcetable, 0, sizeof(pctbl_prog_t));

		while (value2 < 8) {
			pcetable.table = PCE_METER_INS_1_INDEX;
			pcetable.pcindex = value2;
			gsw_pce_table_read(cdev, &pcetable);

			/* We found the empty one */
			if (pcetable.valid == 0) {
				switch (parm->eDir) {
				case GSW_DIRECTION_BOTH:
					pcetable.key[0] =
						(((parm->nPortEgressId & 0xF) << 8) |
						 (parm->nPortIngressId & 0xF));
					pcetable.mask[0] = 0;
					break;

				case GSW_DIRECTION_EGRESS:
					pcetable.key[0] =
						(((parm->nPortEgressId & 0xF) << 8)
						 | 0xF);
					pcetable.mask[0] = 1;
					break;

				case GSW_DIRECTION_INGRESS:
					pcetable.key[0] = (0xF00 |
							   (parm->nPortIngressId & 0xF));
					pcetable.mask[0] = 4;
					break;

				default:
					pcetable.key[0] = 0;
					pcetable.mask[0] = 5;
				}

				pcetable.val[0] = parm->nMeterId & 0x3F;
				pcetable.valid = 1;
				gsw_pce_table_write(cdev, &pcetable);
				nempftbl = 0;
				ret = GSW_statusOk;
				goto UNLOCK_AND_RETURN;
			}

			value2++;
		}
	}

	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_pce);
#endif
	return ret;

}

GSW_return_t GSW_QoS_MeterPortDeassign(void *cdev,
				       GSW_QoS_meterPort_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	pctbl_prog_t pcetable;
	u32  eport, iport, mid, i, j;
	ltq_bool_t eftbl = 0;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_pce);
#endif

	for (i = 0; i < 2; i++) {
		memset(&pcetable, 0, sizeof(pctbl_prog_t));

		if (i == 0)
			pcetable.table = PCE_METER_INS_0_INDEX;
		else
			pcetable.table = PCE_METER_INS_1_INDEX;

		for (j = 0; j < 8; j++) {
			pcetable.pcindex = j;
			gsw_pce_table_read(cdev, &pcetable);

			if (pcetable.valid == 1) {
				iport = pcetable.key[0] & 0xF;
				eport = (pcetable.key[0] >> 8) & 0xF;
				mid = pcetable.val[0] & 0x1F;

				if ((eport == parm->nPortEgressId) &&
				    (iport == parm->nPortIngressId) &&
				    (mid == parm->nMeterId)) {
					if (i == 0)
						pcetable.table =
							PCE_METER_INS_0_INDEX;
					else
						pcetable.table =
							PCE_METER_INS_1_INDEX;

					pcetable.key[0] = 0;
					pcetable.val[0] = 0;
					pcetable.valid = 0;
					gsw_pce_table_write(cdev, &pcetable);
					eftbl = 1;
					pr_err("Found the entry, delet it\n");
				}

			}
		}
	}

	ret = GSW_statusOk;

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_pce);
#endif
	return ret;

}

GSW_return_t GSW_QoS_MeterPortGet(void *cdev,
				  GSW_QoS_meterPortGet_t *parm)
{
	pctbl_prog_t pcetable;
	u32 value = 0;
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_pce);
#endif

	memset(&pcetable, 0, sizeof(pctbl_prog_t));
	/*	gsw_r32(cdev, ETHSW_CAP_3_METERS_OFFSET,*/
	/*		ETHSW_CAP_3_METERS_SHIFT,*/
	/*		ETHSW_CAP_3_METERS_SIZE, &value); */
	value = gswdev->num_of_meters;

	if (parm->bInitial == 1) {
		gswdev->meter_cnt = 0;
		parm->bInitial = 0;
	} else {
		if (gswdev->meter_cnt > (value * 2))
			parm->bLast = 1;
	}

	if (gswdev->meter_cnt > value)
		pcetable.table = PCE_METER_INS_1_INDEX;
	else
		pcetable.table = PCE_METER_INS_0_INDEX;

	pcetable.pcindex = gswdev->meter_cnt;
	gsw_pce_table_read(cdev, &pcetable);

	if (pcetable.valid) {
		parm->nMeterId = (pcetable.val[0] & 0x3F);
		parm->nPortEgressId = ((pcetable.key[0] >> 8) & 0xF);
		parm->nPortIngressId = (pcetable.key[0] & 0xF);

		if ((pcetable.mask[0] & 0x5) == 0)
			parm->eDir = GSW_DIRECTION_BOTH;
		else if ((pcetable.mask[0] & 0x5) == 1)
			parm->eDir = GSW_DIRECTION_EGRESS;
		else if ((pcetable.mask[0] & 0x5) == 4)
			parm->eDir = GSW_DIRECTION_INGRESS;
		else
			parm->eDir = GSW_DIRECTION_NONE;
	}

	gswdev->meter_cnt++;
	ret = GSW_statusOk;

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_pce);
#endif
	return ret;

}

GSW_return_t GSW_QoS_DSCP_ClassGet(void *cdev,
				   GSW_QoS_DSCP_ClassCfg_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	pctbl_prog_t pcetable;
	u32  value;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_pce);
#endif

	for (value = 0; value <= 63; value++) {
		pcetable.table = PCE_DSCP_INDEX;
		pcetable.pcindex = value;
		gsw_pce_table_read(cdev, &pcetable);
		parm->nTrafficClass[value] = (pcetable.val[0] & 0xF);
	}

	ret = GSW_statusOk;

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_pce);
#endif
	return ret;

}

GSW_return_t GSW_QoS_DSCP_ClassSet(void *cdev,
				   GSW_QoS_DSCP_ClassCfg_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	pctbl_prog_t pcetable;
	u32  value;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_pce);
#endif


	for (value = 0; value <= 63; value++) {
		pcetable.table = PCE_DSCP_INDEX;
		pcetable.pcindex = value;
		gsw_pce_table_read(cdev, &pcetable);
		pcetable.val[0] &= ~(0xF);
		pcetable.val[0] |= (parm->nTrafficClass[value] & 0xF);
		gsw_pce_table_write(cdev, &pcetable);
	}

	ret = GSW_statusOk;

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_pce);
#endif
	return ret;

}

GSW_return_t GSW_QoS_ClassDSCP_Get(void *cdev,
				   GSW_QoS_ClassDSCP_Cfg_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	pctbl_prog_t pcetable;
	u32  value;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_pce);
#endif

	if (gswdev->gipver != LTQ_GSWIP_3_0 ||
	    IS_VRSN_NOT_31(gswdev->gipver)) {
		for (value = 0; value < 16; value++) {
			pcetable.table = PCE_REMARKING_INDEX;
			pcetable.pcindex = value;
			gsw_pce_table_read(cdev, &pcetable);
			parm->nDSCP[value] = pcetable.val[0] & 0x3F;
		}
	} else {
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_pce);
#endif
	return ret;

}

GSW_return_t GSW_QoS_ClassDSCP_Set(void *cdev,
				   GSW_QoS_ClassDSCP_Cfg_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	pctbl_prog_t pcetable;
	u32  dscp, pcp, value;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_pce);
#endif

	if (gswdev->gipver != LTQ_GSWIP_3_0
	    || IS_VRSN_NOT_31(gswdev->gipver)) {
		for (value = 0; value < 16; value++) {
			pcetable.table = PCE_REMARKING_INDEX;
			pcetable.pcindex = value;
			gsw_pce_table_read(cdev, &pcetable);
			pcp = (pcetable.val[0] >> 8) & 0x7;
			dscp	= parm->nDSCP[value] & 0x3F;
			pcetable.val[0] = ((pcp << 8) | dscp);
			gsw_pce_table_write(cdev, &pcetable);
		}
	} else {
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_pce);
#endif
	return ret;
}

GSW_return_t GSW_QoS_DSCP_DropPrecedenceCfgGet(void *cdev,
		GSW_QoS_DSCP_DropPrecedenceCfg_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	pctbl_prog_t pcetable;
	u32  value;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_pce);
#endif

	for (value = 0; value <= 63; value++) {
		memset(&pcetable, 0, sizeof(pctbl_prog_t));
		pcetable.table = PCE_DSCP_INDEX;
		pcetable.pcindex = value;
		gsw_pce_table_read(cdev, &pcetable);
		parm->nDSCP_DropPrecedence[value]	=
			((pcetable.val[0] >> 4) & 0x3);
	}

	ret = GSW_statusOk;

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_pce);
#endif
	return ret;
}

GSW_return_t GSW_QoS_DSCP_DropPrecedenceCfgSet(void *cdev,
		GSW_QoS_DSCP_DropPrecedenceCfg_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	pctbl_prog_t pcetable;
	u32  value;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_pce);
#endif

	for (value = 0; value <= 63; value++) {
		memset(&pcetable, 0, sizeof(pctbl_prog_t));
		pcetable.table = PCE_DSCP_INDEX;
		pcetable.pcindex = value;
		gsw_pce_table_read(cdev, &pcetable);
		pcetable.val[0] &= ~(0x3 << 4);
		pcetable.val[0] |=
			((parm->nDSCP_DropPrecedence[value] & 0x3) << 4);
		gsw_pce_table_write(cdev, &pcetable);
	}

	ret = GSW_statusOk;

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_pce);
#endif
	return ret;

}

GSW_return_t GSW_QoS_PortRemarkingCfgGet(void *cdev,
		GSW_QoS_portRemarkingCfg_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32  value, vclpen = 0, vdpen = 0, vdscpmod = 0;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_misc);
#endif

	if (parm->nPortId >= gswdev->tpnum) {
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	/*Applicable for 3.1 also*/
	gsw_r32(cdev, (PCE_PCTRL_0_CLPEN_OFFSET +
		       (10 * parm->nPortId)),
		PCE_PCTRL_0_CLPEN_SHIFT,
		PCE_PCTRL_0_CLPEN_SIZE, &vclpen);

	/*Applicable only for 3.0 and below switch version*/
	if (IS_VRSN_NOT_31(gswdev->gipver)) {
		gsw_r32(cdev, (PCE_PCTRL_0_DPEN_OFFSET +
			       (10 * parm->nPortId)),
			PCE_PCTRL_0_DPEN_SHIFT,
			PCE_PCTRL_0_DPEN_SIZE, &vdpen);
		gsw_r32(cdev, (PCE_PCTRL_2_DSCPMOD_OFFSET +
			       (10 * parm->nPortId)),
			PCE_PCTRL_2_DSCPMOD_SHIFT,
			PCE_PCTRL_2_DSCPMOD_SIZE, &vdscpmod);
	}

	/*Applicable only for 3.0 and below switch version*/
	if (IS_VRSN_NOT_31(gswdev->gipver)) {
		if ((vclpen == 0) && (vdpen == 0)
		    && (vdscpmod == 0))
			parm->eDSCP_IngressRemarkingEnable =
				GSW_DSCP_REMARK_DISABLE;
		else if ((vclpen == 1) && (vdpen == 0)
			 && (vdscpmod == 1))
			parm->eDSCP_IngressRemarkingEnable =
				GSW_DSCP_REMARK_TC6;
		else if ((vclpen == 1) && (vdpen == 1) &&
			 (vdscpmod == 1))
			parm->eDSCP_IngressRemarkingEnable =
				GSW_DSCP_REMARK_TC3;
		else if ((vclpen == 0) && (vdpen == 1) &&
			 (vdscpmod == 1))
			parm->eDSCP_IngressRemarkingEnable =
				GSW_DSCP_REMARK_DP3;
		else if ((vclpen == 1) && (vdpen == 1) &&
			 (vdscpmod == 1))
			parm->eDSCP_IngressRemarkingEnable =
				GSW_DSCP_REMARK_DP3_TC3;
	}

	/*Applicable only for 3.1*/
	if (IS_VRSN_31(gswdev->gipver)) {
		if (vclpen == 0)
			parm->eDSCP_IngressRemarkingEnable =
				GSW_DSCP_REMARK_DISABLE;
		else if (vclpen == 1)
			parm->eDSCP_IngressRemarkingEnable =
				GSW_DSCP_REMARK_TC6;
	}

	/*PCP Remarking Enable/Disable,Applicable for 3.1 also*/
	gsw_r32(cdev, (PCE_PCTRL_0_PCPEN_OFFSET +
		       (10 * parm->nPortId)),
		PCE_PCTRL_0_PCPEN_SHIFT,
		PCE_PCTRL_0_PCPEN_SIZE, &value);
	parm->bPCP_IngressRemarkingEnable = value;

	/*DSCP Remarking Enable/Disable,Applicable for 3.1 also*/
	gsw_r32(cdev, (FDMA_PCTRL_DSCPRM_OFFSET +
		       (6 * parm->nPortId)),
		FDMA_PCTRL_DSCPRM_SHIFT,
		FDMA_PCTRL_DSCPRM_SIZE, &value);
	parm->bDSCP_EgressRemarkingEnable = value;

	/*Applicable only for 3.0 and below switch version*/
	if (IS_VRSN_NOT_31(gswdev->gipver)) {
		gsw_r32(cdev, (FDMA_PCTRL_VLANMOD_OFFSET +
			       (6 * parm->nPortId)),
			FDMA_PCTRL_VLANMOD_SHIFT,
			FDMA_PCTRL_VLANMOD_SIZE, &value);

		if (value == 3)
			parm->bPCP_EgressRemarkingEnable = 1;
		else
			parm->bPCP_EgressRemarkingEnable = 0;
	}

	/*Applicable only for 3.1
	  FDMA_PCTRL_VLANMOD is reduced to 1 bit in GSWIP 3.1*/
	if (IS_VRSN_31(gswdev->gipver)) {
		/*JIRA GSWIP-147 Implemented*/
		gsw_r32(cdev, (FDMA_PCTRL_VLANMOD_OFFSET +
			       (6 * parm->nPortId)),
			FDMA_PCTRL_VLANMOD_SHIFT,
			FDMA_GSWIP3_1_PCTRL_VLANMOD_SIZE, &value);

		if (value == 1)
			parm->bPCP_EgressRemarkingEnable = 1;
		else
			parm->bPCP_EgressRemarkingEnable = 0;
	}

	/*The following are not applicable for GSWIP 3.1*/
	if ((gswdev->gipver == LTQ_GSWIP_2_2) ||
	    (gswdev->gipver == LTQ_GSWIP_2_2_ETC) ||
	    (gswdev->gipver == LTQ_GSWIP_3_0)) {
		gsw_r32(cdev, (PCE_PCTRL_2_SPCPEN_OFFSET +
			       (10 * parm->nPortId)),
			PCE_PCTRL_2_SPCPEN_SHIFT,
			PCE_PCTRL_2_SPCPEN_SIZE,
			&parm->bSTAG_PCP_IngressRemarkingEnable);
		gsw_r32(cdev, (PCE_PCTRL_2_SDEIEN_OFFSET +
			       (10 * parm->nPortId)),
			PCE_PCTRL_2_SDEIEN_SHIFT,
			PCE_PCTRL_2_SDEIEN_SIZE,
			&parm->bSTAG_DEI_IngressRemarkingEnable);
		gsw_w32(cdev, (FDMA_PCTRL_SVLANMOD_OFFSET +
			       (6 * parm->nPortId)),
			FDMA_PCTRL_SVLANMOD_SHIFT,
			FDMA_PCTRL_SVLANMOD_SIZE, value);

		if (value == 3)
			parm->bSTAG_PCP_DEI_EgressRemarkingEnable = 1;
		else
			parm->bSTAG_PCP_DEI_EgressRemarkingEnable = 0;
	}

	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_misc);
#endif
	return ret;

}

GSW_return_t GSW_QoS_PortRemarkingCfgSet(void *cdev,
		GSW_QoS_portRemarkingCfg_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32  value, vclpen = 0, vdpen = 0, vdscpmod = 0;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_misc);
#endif

	if (parm->nPortId >= gswdev->tpnum) {
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	switch (parm->eDSCP_IngressRemarkingEnable) {
	case GSW_DSCP_REMARK_DISABLE:
		vclpen = 0;
		vdpen = 0;
		vdscpmod = 0;
		break;

	case GSW_DSCP_REMARK_TC6:
		vclpen = 1;
		vdpen = 0;
		vdscpmod = 1;
		break;

	case GSW_DSCP_REMARK_TC3:
		vclpen = 1;
		vdpen = 1;
		vdscpmod = 1;

		/*Not supported in GSWIP 3.1*/
		if (IS_VRSN_31(gswdev->gipver)) {
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}

		break;

	case GSW_DSCP_REMARK_DP3:
		vclpen = 0;
		vdpen = 1;
		vdscpmod = 1;

		/*Not supported in GSWIP 3.1*/
		if (IS_VRSN_31(gswdev->gipver)) {
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}

		break;

	case GSW_DSCP_REMARK_DP3_TC3:
		vclpen = 1;
		vdpen = 1;
		vdscpmod = 1;

		/*Not supported in GSWIP 3.1*/
		if (IS_VRSN_31(gswdev->gipver)) {
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}

		break;
	}

	/*Applicable for 3.1 also*/
	gsw_w32(cdev, PCE_PCTRL_0_CLPEN_OFFSET +
		(10 * parm->nPortId),
		PCE_PCTRL_0_CLPEN_SHIFT,
		PCE_PCTRL_0_CLPEN_SIZE, vclpen);

	/*The following are not applicable for GSWIP 3.1*/
	if (IS_VRSN_NOT_31(gswdev->gipver)) {
		gsw_w32(cdev, PCE_PCTRL_0_DPEN_OFFSET +
			(10 * parm->nPortId),
			PCE_PCTRL_0_DPEN_SHIFT,
			PCE_PCTRL_0_DPEN_SIZE, vdpen);
		gsw_w32(cdev, PCE_PCTRL_2_DSCPMOD_OFFSET +
			(10 * parm->nPortId),
			PCE_PCTRL_2_DSCPMOD_SHIFT,
			PCE_PCTRL_2_DSCPMOD_SIZE, vdscpmod);
	}

	/*DSCP Remarking Enable/Disable,Applicable for 3.1 also*/
	if (parm->bDSCP_EgressRemarkingEnable > 0)
		value = parm->bDSCP_EgressRemarkingEnable;
	else
		value = 0;

	gsw_w32(cdev, (FDMA_PCTRL_DSCPRM_OFFSET +
		       (6 * parm->nPortId)),
		FDMA_PCTRL_DSCPRM_SHIFT,
		FDMA_PCTRL_DSCPRM_SIZE, value);

	/*PCP Remarking Enable/Disable,Applicable for 3.1 also*/
	if (parm->bPCP_IngressRemarkingEnable > 0)
		value = parm->bPCP_IngressRemarkingEnable;
	else
		value = 0;

	gsw_w32(cdev, (PCE_PCTRL_0_PCPEN_OFFSET +
		       (10 * parm->nPortId)),
		PCE_PCTRL_0_PCPEN_SHIFT,
		PCE_PCTRL_0_PCPEN_SIZE, value);

	/*Applicable only for 3.0 and below switch version*/
	if (IS_VRSN_NOT_31(gswdev->gipver)) {
		if (parm->bPCP_EgressRemarkingEnable > 0)
			value = 3;
		else
			value = 0;

		gsw_w32(cdev, (FDMA_PCTRL_VLANMOD_OFFSET +
			       (6 * parm->nPortId)),
			FDMA_PCTRL_VLANMOD_SHIFT,
			FDMA_PCTRL_VLANMOD_SIZE, value);
	}

	/*FDMA_PCTRL_VLANMOD is reduced to 1 bit in GSWIP 3.1*/
	if (IS_VRSN_31(gswdev->gipver)) {
		if (parm->bPCP_EgressRemarkingEnable > 0)
			value = 1;
		else
			value = 0;

		/*JIRA GSWIP-147 Implemented*/
		gsw_w32(cdev, (FDMA_PCTRL_VLANMOD_OFFSET +
			       (6 * parm->nPortId)),
			FDMA_PCTRL_VLANMOD_SHIFT,
			FDMA_GSWIP3_1_PCTRL_VLANMOD_SIZE, value);
	}

	/*The following are not applicable for GSWIP 3.1*/
	if ((gswdev->gipver == LTQ_GSWIP_2_2) ||
	    (gswdev->gipver == LTQ_GSWIP_2_2_ETC) ||
	    (gswdev->gipver == LTQ_GSWIP_3_0)) {
		gsw_w32(cdev, (PCE_PCTRL_2_SPCPEN_OFFSET +
			       (10 * parm->nPortId)),
			PCE_PCTRL_2_SPCPEN_SHIFT,
			PCE_PCTRL_2_SPCPEN_SIZE,
			parm->bSTAG_PCP_IngressRemarkingEnable);
		gsw_w32(cdev, (PCE_PCTRL_2_SDEIEN_OFFSET +
			       (10 * parm->nPortId)),
			PCE_PCTRL_2_SDEIEN_SHIFT,
			PCE_PCTRL_2_SDEIEN_SIZE,
			parm->bSTAG_DEI_IngressRemarkingEnable);


		if (parm->bSTAG_PCP_DEI_EgressRemarkingEnable > 0)
			value = 3;
		else
			value = 0;

		/*		gsw_w32(cdev, (FDMA_PCTRL_DEIMOD_OFFSET + */
		/*			(6 * parm->nPortId)), */
		/*			FDMA_PCTRL_DEIMOD_SHIFT,*/
		/*			FDMA_PCTRL_DEIMOD_SIZE,*/
		/*			parm->bSTAG_PCP_DEI_EgressRemarkingEnable);	*/
		gsw_w32(cdev, (FDMA_PCTRL_SVLANMOD_OFFSET +
			       (6 * parm->nPortId)),
			FDMA_PCTRL_SVLANMOD_SHIFT,
			FDMA_PCTRL_SVLANMOD_SIZE, value);
	}

	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_misc);
#endif
	return ret;

}

GSW_return_t GSW_QoS_ClassPCP_Get(void *cdev,
				  GSW_QoS_ClassPCP_Cfg_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	pctbl_prog_t pcetable;
	u32  value;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_pce);
#endif

	if (gswdev->gipver != LTQ_GSWIP_3_0 ||
	    IS_VRSN_NOT_31(gswdev->gipver)) {
		for (value = 0; value < 16; value++) {
			pcetable.table = PCE_REMARKING_INDEX;
			pcetable.pcindex = value;
			gsw_pce_table_read(cdev, &pcetable);
			parm->nPCP[value] = (pcetable.val[0] >> 8) & 0x7;
		}
	} else {
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_pce);
#endif
	return ret;

}

GSW_return_t GSW_QoS_ClassPCP_Set(void *cdev,
				  GSW_QoS_ClassPCP_Cfg_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	pctbl_prog_t pcetable;
	u32  dscp, pcp, value;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_pce);
#endif

	if (gswdev->gipver != LTQ_GSWIP_3_0 ||
	    IS_VRSN_NOT_31(gswdev->gipver)) {
		for (value = 0; value < 16; value++) {
			pcetable.table = PCE_REMARKING_INDEX;
			pcetable.pcindex = value;
			gsw_pce_table_read(cdev, &pcetable);
			dscp = pcetable.val[0] & 0x3F;
			pcp = parm->nPCP[value] & 0x7;
			pcetable.val[0] = (pcp << 8) | dscp;
			gsw_pce_table_write(cdev, &pcetable);
		}
	} else {
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_pce);
#endif
	return ret;

}

GSW_return_t GSW_QoS_PCP_ClassGet(void *cdev,
				  GSW_QoS_PCP_ClassCfg_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	pctbl_prog_t pcetable;
	u32  value;
	u32 ret;
	u8 inrPcpEntries = PCE_INNER_PCP_DFL_ENTRIES;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_pce);
#endif



	for (value = 0; value < inrPcpEntries; value++) {
		memset(&pcetable, 0, sizeof(pctbl_prog_t));
		pcetable.table = PCE_PCP_INDEX;
		pcetable.pcindex = value;
		gsw_pce_table_read(cdev, &pcetable);
		parm->nTrafficClass[value] = (pcetable.val[0] & 0xF);
	}

	ret = GSW_statusOk;

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_pce);
#endif
	return ret;
}

GSW_return_t GSW_QoS_PCP_ClassSet(void *cdev,
				  GSW_QoS_PCP_ClassCfg_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	pctbl_prog_t pcetable;
	u32  value;
	u32 ret;
	u8 inrPcpEntries = PCE_INNER_PCP_DFL_ENTRIES;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_pce);
#endif



	for (value = 0; value < inrPcpEntries; value++) {
		memset(&pcetable, 0, sizeof(pctbl_prog_t));
		pcetable.table = PCE_PCP_INDEX;
		pcetable.pcindex = value;
		pcetable.val[0] = (parm->nTrafficClass[value] & 0xF);
		gsw_pce_table_write(cdev, &pcetable);
	}

	ret = GSW_statusOk;

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_pce);
#endif
	return ret;
}

GSW_return_t GSW_QoS_PortCfgGet(void *cdev,
				GSW_QoS_portCfg_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32  value, dscp, cpcp, spcp = 0;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_misc);
#endif

	if (parm->nPortId >= gswdev->tpnum) {
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	gsw_r32(cdev, (PCE_PCTRL_2_DSCP_OFFSET +
		       (10 * parm->nPortId)),
		PCE_PCTRL_2_DSCP_SHIFT,
		PCE_PCTRL_2_DSCP_SIZE, &dscp);
	gsw_r32(cdev, (PCE_PCTRL_2_PCP_OFFSET +
		       (10 * parm->nPortId)),
		PCE_PCTRL_2_PCP_SHIFT,
		PCE_PCTRL_2_PCP_SIZE, &cpcp);

	if ((gswdev->gipver == LTQ_GSWIP_2_2_ETC) ||
	    (IS_VRSN_30_31(gswdev->gipver))) {
		gsw_r32(cdev, (PCE_PCTRL_2_SPCP_OFFSET +
			       (10 * parm->nPortId)),
			PCE_PCTRL_2_SPCP_SHIFT,
			PCE_PCTRL_2_SPCP_SIZE, &spcp);
	}

	if ((dscp == 0) && (cpcp == 0) && (spcp == 0))
		parm->eClassMode = GSW_QOS_CLASS_SELECT_NO;
	else if ((dscp == 2) && (cpcp == 0) && (spcp == 0))
		parm->eClassMode = GSW_QOS_CLASS_SELECT_DSCP;
	else if ((dscp == 0) && (cpcp == 1) && (spcp == 0))
		parm->eClassMode = GSW_QOS_CLASS_SELECT_PCP;
	else if ((dscp == 2) && (cpcp == 1) && (spcp == 0))
		parm->eClassMode = GSW_QOS_CLASS_SELECT_DSCP_PCP;
	else if ((dscp == 1) && (cpcp == 1) && (spcp == 0))
		parm->eClassMode = GSW_QOS_CLASS_SELECT_PCP_DSCP;
	else if ((dscp == 0) && (cpcp == 0) && (spcp == 1))
		parm->eClassMode = GSW_QOS_CLASS_SELECT_SPCP;
	else if ((dscp == 1) && (cpcp == 0) && (spcp == 1))
		parm->eClassMode = GSW_QOS_CLASS_SELECT_SPCP_DSCP;
	else if ((dscp == 2) && (cpcp == 0) && (spcp == 1))
		parm->eClassMode = GSW_QOS_CLASS_SELECT_DSCP_SPCP;
	else if ((dscp == 0) && (cpcp == 1) && (spcp == 1))
		parm->eClassMode = GSW_QOS_CLASS_SELECT_SPCP_PCP;
	else if ((dscp == 1) && (cpcp == 1) && (spcp == 1))
		parm->eClassMode = GSW_QOS_CLASS_SELECT_SPCP_PCP_DSCP;
	else if ((dscp == 2) && (cpcp == 1) && (spcp == 1))
		parm->eClassMode = GSW_QOS_CLASS_SELECT_DSCP_SPCP_PCP;
	else
		parm->eClassMode = GSW_QOS_CLASS_SELECT_NO;

	/*PCLASS is removed in PCE_PCTRL_2 for GSWIP 3.1*/
	if (IS_VRSN_NOT_31(gswdev->gipver)) {
		gsw_r32(cdev, (PCE_PCTRL_2_PCLASS_OFFSET +
			       (10 * parm->nPortId)),
			PCE_PCTRL_2_PCLASS_SHIFT,
			PCE_PCTRL_2_PCLASS_SIZE, &value);
		parm->nTrafficClass = value;
	}

	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_misc);
#endif
	return ret;
}

GSW_return_t GSW_QoS_PortCfgSet(void *cdev,
				GSW_QoS_portCfg_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_misc);
#endif

	if (parm->nPortId >= gswdev->tpnum) {
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	switch (parm->eClassMode) {
	case GSW_QOS_CLASS_SELECT_NO:
		gsw_w32(cdev, (PCE_PCTRL_2_DSCP_OFFSET +
			       (10 * parm->nPortId)),
			PCE_PCTRL_2_DSCP_SHIFT,
			PCE_PCTRL_2_DSCP_SIZE, 0);
		gsw_w32(cdev, (PCE_PCTRL_2_PCP_OFFSET +
			       (10 * parm->nPortId)),
			PCE_PCTRL_2_PCP_SHIFT,
			PCE_PCTRL_2_PCP_SIZE, 0);

		if ((gswdev->gipver == LTQ_GSWIP_2_2_ETC)
		    || (IS_VRSN_30_31(gswdev->gipver))) {
			gsw_w32(cdev, (PCE_PCTRL_2_SPCP_OFFSET
				       + (10 * parm->nPortId)),
				PCE_PCTRL_2_SPCP_SHIFT,
				PCE_PCTRL_2_SPCP_SIZE, 0);
		}

		break;

	case GSW_QOS_CLASS_SELECT_DSCP:
		gsw_w32(cdev, (PCE_PCTRL_2_DSCP_OFFSET +
			       (10 * parm->nPortId)),
			PCE_PCTRL_2_DSCP_SHIFT,
			PCE_PCTRL_2_DSCP_SIZE, 2);
		gsw_w32(cdev, (PCE_PCTRL_2_PCP_OFFSET +
			       (10 * parm->nPortId)),
			PCE_PCTRL_2_PCP_SHIFT,
			PCE_PCTRL_2_PCP_SIZE, 0);

		if ((gswdev->gipver == LTQ_GSWIP_2_2_ETC) ||
		    (IS_VRSN_30_31(gswdev->gipver))) {
			gsw_w32(cdev, (PCE_PCTRL_2_SPCP_OFFSET +
				       (10 * parm->nPortId)),
				PCE_PCTRL_2_SPCP_SHIFT,
				PCE_PCTRL_2_SPCP_SIZE, 0);
		}

		break;

	case GSW_QOS_CLASS_SELECT_PCP:
		gsw_w32(cdev, (PCE_PCTRL_2_DSCP_OFFSET +
			       (10 * parm->nPortId)),
			PCE_PCTRL_2_DSCP_SHIFT,
			PCE_PCTRL_2_DSCP_SIZE, 0);
		gsw_w32(cdev, PCE_PCTRL_2_PCP_OFFSET +
			(10 * parm->nPortId),
			PCE_PCTRL_2_PCP_SHIFT,
			PCE_PCTRL_2_PCP_SIZE, 1);

		if ((gswdev->gipver == LTQ_GSWIP_2_2_ETC) ||
		    (IS_VRSN_30_31(gswdev->gipver))) {
			gsw_w32(cdev, (PCE_PCTRL_2_SPCP_OFFSET +
				       (10 * parm->nPortId)),
				PCE_PCTRL_2_SPCP_SHIFT,
				PCE_PCTRL_2_SPCP_SIZE, 0);
		}

		break;

	case GSW_QOS_CLASS_SELECT_DSCP_PCP:
		gsw_w32(cdev, (PCE_PCTRL_2_DSCP_OFFSET +
			       (10 * parm->nPortId)),
			PCE_PCTRL_2_DSCP_SHIFT,
			PCE_PCTRL_2_DSCP_SIZE, 2);
		gsw_w32(cdev, (PCE_PCTRL_2_PCP_OFFSET +
			       (10 * parm->nPortId)),
			PCE_PCTRL_2_PCP_SHIFT,
			PCE_PCTRL_2_PCP_SIZE, 1);

		if ((gswdev->gipver == LTQ_GSWIP_2_2_ETC) ||
		    (IS_VRSN_30_31(gswdev->gipver))) {
			gsw_w32(cdev, (PCE_PCTRL_2_SPCP_OFFSET +
				       (10 * parm->nPortId)),
				PCE_PCTRL_2_SPCP_SHIFT,
				PCE_PCTRL_2_SPCP_SIZE, 0);
		}

		break;

	case GSW_QOS_CLASS_SELECT_PCP_DSCP:
		gsw_w32(cdev, (PCE_PCTRL_2_DSCP_OFFSET +
			       (10 * parm->nPortId)),
			PCE_PCTRL_2_DSCP_SHIFT,
			PCE_PCTRL_2_DSCP_SIZE, 1);
		gsw_w32(cdev, (PCE_PCTRL_2_PCP_OFFSET +
			       (10 * parm->nPortId)),
			PCE_PCTRL_2_PCP_SHIFT,
			PCE_PCTRL_2_PCP_SIZE, 1);

		if ((gswdev->gipver == LTQ_GSWIP_2_2_ETC) ||
		    (IS_VRSN_30_31(gswdev->gipver))) {
			gsw_w32(cdev, (PCE_PCTRL_2_SPCP_OFFSET +
				       (10 * parm->nPortId)),
				PCE_PCTRL_2_SPCP_SHIFT,
				PCE_PCTRL_2_SPCP_SIZE, 0);
		}

		break;

		if ((gswdev->gipver == LTQ_GSWIP_2_2_ETC) ||
		    (IS_VRSN_30_31(gswdev->gipver))) {
		case GSW_QOS_CLASS_SELECT_SPCP:
			gsw_w32(cdev, (PCE_PCTRL_2_DSCP_OFFSET +
				       (10 * parm->nPortId)),
				PCE_PCTRL_2_DSCP_SHIFT,
				PCE_PCTRL_2_DSCP_SIZE, 0);
			gsw_w32(cdev, (PCE_PCTRL_2_PCP_OFFSET +
				       (10 * parm->nPortId)),
				PCE_PCTRL_2_PCP_SHIFT,
				PCE_PCTRL_2_PCP_SIZE, 0);
			gsw_w32(cdev, (PCE_PCTRL_2_SPCP_OFFSET +
				       (10 * parm->nPortId)),
				PCE_PCTRL_2_SPCP_SHIFT,
				PCE_PCTRL_2_SPCP_SIZE, 1);
			break;

		case GSW_QOS_CLASS_SELECT_SPCP_DSCP:
			gsw_w32(cdev, (PCE_PCTRL_2_DSCP_OFFSET +
				       (10 * parm->nPortId)),
				PCE_PCTRL_2_DSCP_SHIFT,
				PCE_PCTRL_2_DSCP_SIZE, 1);
			gsw_w32(cdev, (PCE_PCTRL_2_PCP_OFFSET +
				       (10 * parm->nPortId)),
				PCE_PCTRL_2_PCP_SHIFT,
				PCE_PCTRL_2_PCP_SIZE, 0);
			gsw_w32(cdev, (PCE_PCTRL_2_SPCP_OFFSET +
				       (10 * parm->nPortId)),
				PCE_PCTRL_2_SPCP_SHIFT,
				PCE_PCTRL_2_SPCP_SIZE, 1);
			break;

		case GSW_QOS_CLASS_SELECT_DSCP_SPCP:
			gsw_w32(cdev, (PCE_PCTRL_2_DSCP_OFFSET +
				       (10 * parm->nPortId)),
				PCE_PCTRL_2_DSCP_SHIFT,
				PCE_PCTRL_2_DSCP_SIZE, 2);
			gsw_w32(cdev, (PCE_PCTRL_2_PCP_OFFSET +
				       (10 * parm->nPortId)),
				PCE_PCTRL_2_PCP_SHIFT,
				PCE_PCTRL_2_PCP_SIZE, 0);
			gsw_w32(cdev, (PCE_PCTRL_2_SPCP_OFFSET +
				       (10 * parm->nPortId)),
				PCE_PCTRL_2_SPCP_SHIFT,
				PCE_PCTRL_2_SPCP_SIZE, 1);
			break;

		case GSW_QOS_CLASS_SELECT_SPCP_PCP:
			gsw_w32(cdev, (PCE_PCTRL_2_DSCP_OFFSET +
				       (10 * parm->nPortId)),
				PCE_PCTRL_2_DSCP_SHIFT,
				PCE_PCTRL_2_DSCP_SIZE, 0);
			gsw_w32(cdev, (PCE_PCTRL_2_PCP_OFFSET +
				       (10 * parm->nPortId)),
				PCE_PCTRL_2_PCP_SHIFT,
				PCE_PCTRL_2_PCP_SIZE, 1);
			gsw_w32(cdev, (PCE_PCTRL_2_SPCP_OFFSET +
				       (10 * parm->nPortId)),
				PCE_PCTRL_2_SPCP_SHIFT,
				PCE_PCTRL_2_SPCP_SIZE, 1);
			break;

		case GSW_QOS_CLASS_SELECT_SPCP_PCP_DSCP:
			gsw_w32(cdev, (PCE_PCTRL_2_DSCP_OFFSET +
				       (10 * parm->nPortId)),
				PCE_PCTRL_2_DSCP_SHIFT,
				PCE_PCTRL_2_DSCP_SIZE, 1);
			gsw_w32(cdev, (PCE_PCTRL_2_PCP_OFFSET +
				       (10 * parm->nPortId)),
				PCE_PCTRL_2_PCP_SHIFT,
				PCE_PCTRL_2_PCP_SIZE, 1);
			gsw_w32(cdev, (PCE_PCTRL_2_SPCP_OFFSET +
				       (10 * parm->nPortId)),
				PCE_PCTRL_2_SPCP_SHIFT,
				PCE_PCTRL_2_SPCP_SIZE, 1);
			break;

		case GSW_QOS_CLASS_SELECT_DSCP_SPCP_PCP:
			gsw_w32(cdev, (PCE_PCTRL_2_DSCP_OFFSET +
				       (10 * parm->nPortId)),
				PCE_PCTRL_2_DSCP_SHIFT,
				PCE_PCTRL_2_DSCP_SIZE, 2);
			gsw_w32(cdev, (PCE_PCTRL_2_PCP_OFFSET +
				       (10 * parm->nPortId)),
				PCE_PCTRL_2_PCP_SHIFT,
				PCE_PCTRL_2_PCP_SIZE, 1);
			gsw_w32(cdev, (PCE_PCTRL_2_SPCP_OFFSET +
				       (10 * parm->nPortId)),
				PCE_PCTRL_2_SPCP_SHIFT,
				PCE_PCTRL_2_SPCP_SIZE, 1);
			break;
		}
	}

	/*PCLASS is removed in PCE_PCTRL_2 for GSWIP 3.1*/
	if (IS_VRSN_NOT_31(gswdev->gipver)) {
		if (parm->nTrafficClass > 0xF) {
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		} else {
			gsw_w32(cdev, PCE_PCTRL_2_PCLASS_OFFSET
				+ (10 * parm->nPortId),
				PCE_PCTRL_2_PCLASS_SHIFT,
				PCE_PCTRL_2_PCLASS_SIZE,
				parm->nTrafficClass);
		}
	}

	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_misc);
#endif
	return ret;
}

GSW_return_t GSW_QoS_QueuePortGet(void *cdev,
				  GSW_QoS_queuePort_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	pctbl_prog_t pcetable;
	bmtbl_prog_t bmtable = {0};
	u32 sPceBpReg = 0;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d\n", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_pce);
#endif

	memset(&pcetable, 0, sizeof(pctbl_prog_t));
	pcetable.table = PCE_QUEUE_MAP_INDEX;
	pcetable.pcindex = ((parm->nPortId << 4) | parm->nTrafficClassId);

	if ((gswdev->gipver == LTQ_GSWIP_3_0 && parm->bRedirectionBypass) ||
	    (IS_VRSN_31(gswdev->gipver) && parm->bExtrationEnable)) {
		pcetable.pcindex |= (1 << 8);
	}

	gsw_pce_table_read(cdev, &pcetable);
	parm->nQueueId = (pcetable.val[0] & 0x3F);

	if (IS_VRSN_31(gswdev->gipver) && parm->bRedirectionBypass) {
		/* PCE Bypass Configuration. GSWIP-3.1 only. */
		gsw_r32_raw(cdev, (SDMA_BYPASS_PCE_REG_OFFSET + (parm->nPortId * 0x6)),
			    &sPceBpReg);
		GET_VAL_FROM_REG(parm->eQMapMode, SDMA_BYPASS_PCE_MD_SHIFT, SDMA_BYPASS_PCE_MD_SIZE, sPceBpReg);

		if (!parm->bExtrationEnable) {
			GET_VAL_FROM_REG(parm->nQueueId, SDMA_BYPASS_PCE_NMQID_SHIFT, SDMA_BYPASS_PCE_NMQID_SIZE, sPceBpReg);
		} else {
			GET_VAL_FROM_REG(parm->nQueueId, SDMA_BYPASS_PCE_EXTQID_SHIFT, SDMA_BYPASS_PCE_EXTQID_SIZE, sPceBpReg);
		}
	}

	if (IS_VRSN_30_31(gswdev->gipver)) {
		bmtable.adr.qMapTbl.nQueueId = parm->nQueueId;
		bmtable.tableID = (BM_Table_ID) BUF_MGR_Q_MAP_TABLE;
		bmtable.numValues = 1;
		gsw_bm_table_read(cdev, &bmtable);
		parm->nRedirectPortId = (bmtable.value[0] & 0x7);
		parm->nRedirectPortId |= ((bmtable.value[0] >> 1) & 0x8);
	}

	ret = GSW_statusOk;

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_pce);
#endif
	return ret;

}

GSW_return_t GSW_QoS_QueuePortSet(void *cdev,
				  GSW_QoS_queuePort_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	pctbl_prog_t pcetable;
	u32  eport = 0, qid, sPceBpReg = 0;
	bmtbl_prog_t bmtable = {0};
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

	if (IS_VRSN_31(gswdev->gipver)) {
		GSW_Freeze();
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_pce);
#endif

	if (IS_VRSN_30_31(gswdev->gipver))
		qid = parm->nQueueId & 0x1F;
	else
		qid = parm->nQueueId & 0x3F;

	if (IS_VRSN_31(gswdev->gipver) && parm->bRedirectionBypass) {
		/* PCE Bypass Configuration. GSWIP-3.1 only. */
		gsw_r32_raw(cdev, (SDMA_BYPASS_PCE_REG_OFFSET + (parm->nPortId * 0x6)),
			    &sPceBpReg);

		//Populate PCE Bypass control register.
		if (!parm->bExtrationEnable) {
			if (parm->eQMapMode == GSW_QOS_QMAP_SINGLE_MODE)
				CLEAR_FILL_CTRL_REG(sPceBpReg, SDMA_BYPASS_PCE_MD_SHIFT,
						    SDMA_BYPASS_PCE_MD_SIZE, 1);
			else
				CLEAR_FILL_CTRL_REG(sPceBpReg, SDMA_BYPASS_PCE_MD_SHIFT,
						    SDMA_BYPASS_PCE_MD_SIZE, 0);

			CLEAR_FILL_CTRL_REG(sPceBpReg, SDMA_BYPASS_PCE_NMQID_SHIFT,
					    SDMA_BYPASS_PCE_NMQID_SIZE, qid);
		} else {
			CLEAR_FILL_CTRL_REG(sPceBpReg, SDMA_BYPASS_PCE_EXTQID_SHIFT,
					    SDMA_BYPASS_PCE_EXTQID_SIZE, qid);
		}

		gsw_w32_raw(cdev, (SDMA_BYPASS_PCE_REG_OFFSET + (parm->nPortId * 0x6)),
			    sPceBpReg);
	} else {
		/*Program Queue Mapping Table*/
		memset(&pcetable, 0, sizeof(pctbl_prog_t));
		pcetable.table = PCE_QUEUE_MAP_INDEX;
		pcetable.pcindex = ((parm->nPortId & 0x0F) << 4) |
				   (parm->nTrafficClassId & 0x0F);

		if ((gswdev->gipver == LTQ_GSWIP_3_0 && parm->bRedirectionBypass) ||
		    (IS_VRSN_31(gswdev->gipver) && parm->bExtrationEnable)) {
			pcetable.pcindex |= 1 << 8;
		}

		pcetable.val[0] = qid;
		gsw_pce_table_write(cdev, &pcetable);
	}

	/*Program BM table*/
	/* Assign the Egress Port Id and Enable the Queue */
	if (gswdev->gipver == LTQ_GSWIP_3_0) {
		eport = (parm->nPortId & 0xF) << 8;
		eport |= (parm->nRedirectPortId & 0x7);
		eport |= (((parm->nRedirectPortId >> 3) & 0x1) << 4);
	} else if (IS_VRSN_31(gswdev->gipver)) {
		eport = (parm->nRedirectPortId & 0x7);
		eport |= (((parm->nRedirectPortId >> 3) & 0x1) << 4);
	} else {
		eport = (parm->nPortId & 0x7);
		eport |= (((parm->nPortId >> 3) & 0x1) << 4);
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_bm);
#endif

	bmtable.value[0] = eport;
	bmtable.adr.qMapTbl.nQueueId = parm->nQueueId;
	bmtable.tableID = BUF_MGR_Q_MAP_TABLE;
	bmtable.numValues = 1;
	ret = gsw_bm_table_write(cdev, &bmtable);

	if (IS_VRSN_31(gswdev->gipver)) {
		GSW_UnFreeze();
	}

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_bm);
#endif


#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_pce);
#endif
	return ret;


}

GSW_return_t GSW_QoS_SchedulerCfgGet(void *cdev,
				     GSW_QoS_schedulerCfg_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	int qid = parm->nQueueId;
	bmtbl_prog_t bmtable = {0};
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_bm);
#endif

	/*	gsw_r32(cdev, ETHSW_CAP_1_QUEUE_OFFSET,*/
	/*		ETHSW_CAP_1_QUEUE_SHIFT, */
	/*		ETHSW_CAP_1_QUEUE_SIZE, &value); */
	if (qid >= gswdev->num_of_queues) {
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	bmtable.adr.wfq.nQueueId = qid;
	bmtable.tableID = (BM_Table_ID) WFQ_PARAM;
	bmtable.numValues = 1;
	gsw_bm_table_read(cdev, &bmtable);

	parm->nWeight = bmtable.value[0];

	if ((bmtable.value[0] == 0xFFFF) || (bmtable.value[0] == 0x1800))
		parm->eType = GSW_QOS_SCHEDULER_STRICT;
	else
		parm->eType = GSW_QOS_SCHEDULER_WFQ;

	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_bm);
#endif
	return ret;
}

GSW_return_t GSW_QoS_SchedulerCfgSet(void *cdev,
				     GSW_QoS_schedulerCfg_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	int qid = parm->nQueueId;
	bmtbl_prog_t bmtable = {0};
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_bm);
#endif

	/*	gsw_r32(cdev, ETHSW_CAP_1_QUEUE_OFFSET, */
	/*		ETHSW_CAP_1_QUEUE_SHIFT, */
	/*		ETHSW_CAP_1_QUEUE_SIZE, &value); */
	if (qid >= gswdev->num_of_queues) {
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	if ((parm->eType == GSW_QOS_SCHEDULER_WFQ) && (parm->nWeight == 0)) {
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	bmtable.adr.wfq.nQueueId = qid;
	bmtable.value[0] = (GSW_QOS_SCHEDULER_STRICT == parm->eType) ? 0xFFFF : parm->nWeight;
	bmtable.tableID = WFQ_PARAM;
	bmtable.numValues = 1;
	gsw_bm_table_write(cdev, &bmtable);

	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_bm);
#endif
	return ret;
}

GSW_return_t GSW_QoS_ShaperCfgGet(void *cdev,
				  GSW_QoS_ShaperCfg_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	int rshid = parm->nRateShaperId;
	u32  value, exp, mant, ibs;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_misc);
#endif

	if (rshid >= gswdev->num_of_shapers) {
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	/* Enable/Disable the rate shaper  */
	gsw_r32(cdev, RS_CTRL_RSEN_OFFSET + (rshid * 0x5),
		RS_CTRL_RSEN_SHIFT, RS_CTRL_RSEN_SIZE, &value);
	parm->bEnable = value;
	/* Committed Burst Size (CBS [bytes]) */
	gsw_r32(cdev, RS_CBS_CBS_OFFSET + (rshid * 0x5),
		RS_CBS_CBS_SHIFT, RS_CBS_CBS_SIZE, &value);
	parm->nCbs = (value * 64);
	/** Rate [Mbit/s] */
	gsw_r32(cdev, RS_CIR_EXP_EXP_OFFSET + (rshid * 0x5),
		RS_CIR_EXP_EXP_SHIFT, RS_CIR_EXP_EXP_SIZE, &exp);
	gsw_r32(cdev, RS_CIR_MANT_MANT_OFFSET + (rshid * 0x5),
		RS_CIR_MANT_MANT_SHIFT, RS_CIR_MANT_MANT_SIZE, &mant);

	if (IS_VRSN_31(gswdev->gipver))
		gsw_r32(cdev, RS_IBS_IBS_OFFSET + (rshid * 0x5),
			RS_IBS_IBS_SHIFT, RS_IBS_IBS_SIZE_GSWIP_3_1, &ibs);
	else
		gsw_r32(cdev, RS_IBS_IBS_OFFSET + (rshid * 0x5),
			RS_IBS_IBS_SHIFT, RS_IBS_IBS_SIZE, &ibs);

	/* calc the Rate */
	if (IS_VRSN_31(gswdev->gipver))
		parm->nRate = mratecalc_3_1(ibs, exp, mant);
	else
		parm->nRate = mratecalc(ibs, exp, mant);

	if ((gswdev->gipver == LTQ_GSWIP_2_2) ||
	    (gswdev->gipver == LTQ_GSWIP_2_2_ETC) ||
	    (IS_VRSN_30_31(gswdev->gipver))) {
		gsw_r32(cdev, RS_CTRL_RSMOD_OFFSET + (rshid * 0x5),
			RS_CTRL_RSMOD_SHIFT, RS_CTRL_RSMOD_SIZE, &parm->bAVB);
	}

	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_misc);
#endif
	return ret;
}

GSW_return_t GSW_QoS_ShaperCfgSet(void *cdev,
				  GSW_QoS_ShaperCfg_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	int rshid = parm->nRateShaperId;
	u32  value, exp = 0, mant = 0, rate = 0, ibs = 0;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_misc);
#endif

	if (rshid >= gswdev->num_of_shapers) {
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	/* Committed Burst Size */
	if (parm->nCbs > 0xFFC0)
		value = 0x3FF;
	else
		value = ((parm->nCbs + 63) / 64);

	/* Committed Burst Size (CBS [bytes]) */
	gsw_w32(cdev, RS_CBS_CBS_OFFSET + (rshid * 0x5),
		RS_CBS_CBS_SHIFT, RS_CBS_CBS_SIZE, value);
	/* Rate [kbit/s] */
	/* Calc the Rate */
	rate = parm->nRate;

	if (rate) {
		if (IS_VRSN_31(gswdev->gipver))
			calc_mtoken_3_1(rate, &ibs, &exp, &mant);
		else
			calc_mtoken(rate, &ibs, &exp, &mant);
	}

	//Set CIR Exponent and Mantissa.
	gsw_w32(cdev, RS_CIR_EXP_EXP_OFFSET + (rshid * 0x5),
		RS_CIR_EXP_EXP_SHIFT, RS_CIR_EXP_EXP_SIZE, exp);
	gsw_w32(cdev, RS_CIR_MANT_MANT_OFFSET + (rshid * 0x5),
		RS_CIR_MANT_MANT_SHIFT, RS_CIR_MANT_MANT_SIZE, mant);

	//Set Instantaneous Burst Size.
	if (IS_VRSN_31(gswdev->gipver))
		gsw_w32(cdev, RS_IBS_IBS_OFFSET + (rshid * 0x5),
			RS_IBS_IBS_SHIFT, RS_IBS_IBS_SIZE_GSWIP_3_1, ibs);
	else
		gsw_w32(cdev, RS_IBS_IBS_OFFSET + (rshid * 0x5),
			RS_IBS_IBS_SHIFT, RS_IBS_IBS_SIZE, ibs);

	/* Enable/Disable the Rate Shaper  */
	gsw_w32(cdev, RS_CTRL_RSEN_OFFSET + (rshid * 0x5),
		RS_CTRL_RSEN_SHIFT, RS_CTRL_RSEN_SIZE, parm->bEnable);

	if ((gswdev->gipver == LTQ_GSWIP_2_2) ||
	    (gswdev->gipver == LTQ_GSWIP_2_2_ETC) ||
	    (IS_VRSN_30_31(gswdev->gipver))) {
		gsw_w32(cdev, RS_CTRL_RSMOD_OFFSET + (rshid * 0x5),
			RS_CTRL_RSMOD_SHIFT, RS_CTRL_RSMOD_SIZE, parm->bAVB);
	}

	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_misc);
#endif
	return ret;

}

GSW_return_t GSW_QoS_ShaperQueueAssign(void *cdev,
				       GSW_QoS_ShaperQueue_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u8 rshid = parm->nRateShaperId, qid = parm->nQueueId;
	u32 value1_RS, value1_ShaperId, value2_RS, value2_ShaperId;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_misc);
#endif

	if (qid >= gswdev->num_of_queues) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	if (rshid >= gswdev->num_of_shapers) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	/* Check Rate Shaper 1 Enable  */
	gsw_r32(cdev, PQM_RS_EN1_OFFSET + (qid * 2),
		PQM_RS_EN1_SHIFT, PQM_RS_EN1_SIZE, &value1_RS);
	gsw_r32(cdev, PQM_RS_RS1_OFFSET + (qid * 2),
		PQM_RS_RS1_SHIFT, PQM_RS_RS1_SIZE, &value1_ShaperId);
	/* Check Rate Shaper 2 Enable  */
	gsw_r32(cdev, PQM_RS_EN2_OFFSET + (qid * 2),
		PQM_RS_EN2_SHIFT, PQM_RS_EN2_SIZE, &value2_RS);
	gsw_r32(cdev, PQM_RS_RS2_OFFSET + (qid * 2),
		PQM_RS_RS2_SHIFT, PQM_RS_RS2_SIZE, &value2_ShaperId);

	if ((value1_RS == 1) && (value1_ShaperId == rshid)) {
		ret = GSW_statusOk;
		goto UNLOCK_AND_RETURN;
	} else if ((value2_RS == 1) && (value2_ShaperId == rshid)) {
		ret = GSW_statusOk;
		goto UNLOCK_AND_RETURN;
	} else if (value1_RS == 0) {
		gswdev->shaper_idx[rshid].usageCount++;
		gsw_w32(cdev, PQM_RS_RS1_OFFSET + (qid * 2),
			PQM_RS_RS1_SHIFT, PQM_RS_RS1_SIZE, rshid);
		gsw_w32(cdev, PQM_RS_EN1_OFFSET + (qid * 2),
			PQM_RS_EN1_SHIFT, PQM_RS_EN1_SIZE, 0x1);
		//Govind - Already enabled in ShaperCfgSet.
		gsw_w32(cdev, RS_CTRL_RSEN_OFFSET + (rshid * 0x5),
			RS_CTRL_RSEN_SHIFT, RS_CTRL_RSEN_SIZE, 0x1);
	} else if (value2_RS == 0) {
		gswdev->shaper_idx[rshid].usageCount++;
		gsw_w32(cdev, PQM_RS_RS2_OFFSET + (qid * 2),
			PQM_RS_RS2_SHIFT, PQM_RS_RS2_SIZE, rshid);
		gsw_w32(cdev, PQM_RS_EN2_OFFSET + (qid * 2),
			PQM_RS_EN2_SHIFT, PQM_RS_EN2_SIZE, 0x1);
		//Govind - Already enabled in ShaperCfgSet.
		gsw_w32(cdev, RS_CTRL_RSEN_OFFSET + (rshid * 0x5),
			RS_CTRL_RSEN_SHIFT, RS_CTRL_RSEN_SIZE, 0x1);
	} else {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_misc);
#endif
	return ret;

}

GSW_return_t GSW_QoS_ShaperQueueDeassign(void *cdev,
		GSW_QoS_ShaperQueue_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u8 rshid = parm->nRateShaperId, qid = parm->nQueueId;
	u32 value1, value2, value3, value4;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_misc);
#endif

	if (qid >= gswdev->num_of_queues) {
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	if (rshid >= gswdev->num_of_shapers) {
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	/* Rate Shaper 1 Read  */
	gsw_r32(cdev, PQM_RS_EN1_OFFSET + (qid * 2),
		PQM_RS_EN1_SHIFT, PQM_RS_EN1_SIZE, &value1);
	gsw_r32(cdev, PQM_RS_RS1_OFFSET + (qid * 2),
		PQM_RS_RS1_SHIFT, PQM_RS_RS1_SIZE, &value2);
	/* Rate Shaper 2 Read  */
	gsw_r32(cdev, PQM_RS_EN2_OFFSET + (qid * 2),
		PQM_RS_EN2_SHIFT, PQM_RS_EN2_SIZE, &value3);
	gsw_r32(cdev, PQM_RS_RS2_OFFSET + (qid * 2),
		PQM_RS_RS2_SHIFT, PQM_RS_RS2_SIZE, &value4);

	if ((value1 == 1) && (value2 == rshid)) {
		gswdev->shaper_idx[rshid].usageCount--;
		gsw_w32(cdev, PQM_RS_EN1_OFFSET + (qid * 2),
			PQM_RS_EN1_SHIFT, PQM_RS_EN1_SIZE, 0);
		gsw_w32(cdev, PQM_RS_RS1_OFFSET + (qid * 2),
			PQM_RS_RS1_SHIFT, PQM_RS_RS1_SIZE, 0);
		ret = GSW_statusOk;
		goto UNLOCK_AND_RETURN;
	} else if ((value3 == 1) && (value4 == rshid)) {
		gswdev->shaper_idx[rshid].usageCount--;
		gsw_w32(cdev, PQM_RS_EN2_OFFSET + (qid * 2),
			PQM_RS_EN2_SHIFT, PQM_RS_EN2_SIZE, 0);
		gsw_w32(cdev, PQM_RS_RS2_OFFSET + (qid * 2),
			PQM_RS_RS2_SHIFT, PQM_RS_RS2_SIZE, 0);
		ret = GSW_statusOk;
		goto UNLOCK_AND_RETURN;
	} else {
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	if ((value1 == 0) && (value3 == 0) &&
	    (0 == gswdev->shaper_idx[rshid].usageCount)) {
		gsw_w32(cdev, RS_CTRL_RSEN_OFFSET + (rshid * 0x5),
			RS_CTRL_RSEN_SHIFT, RS_CTRL_RSEN_SIZE, 0);
	}

	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_misc);
#endif
	return ret;
}

GSW_return_t GSW_QoS_ShaperQueueGet(void *cdev,
				    GSW_QoS_ShaperQueueGet_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 rsh, value;
	u8 qid = parm->nQueueId;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_misc);
#endif

	if (qid >= gswdev->num_of_queues) {
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	parm->bAssigned = 0;
	parm->nRateShaperId = 0;
	gsw_r32(cdev, PQM_RS_EN1_OFFSET + (qid * 2),
		PQM_RS_EN1_SHIFT, PQM_RS_EN1_SIZE, &rsh);

	if (rsh == 1) {
		parm->bAssigned = 1;
		gsw_r32(cdev, PQM_RS_RS1_OFFSET + (qid * 2),
			PQM_RS_RS1_SHIFT, PQM_RS_RS1_SIZE, &value);
		parm->nRateShaperId = value;
	}

	gsw_r32(cdev, PQM_RS_EN2_OFFSET + (qid * 2),
		PQM_RS_EN2_SHIFT, PQM_RS_EN2_SIZE, &rsh);

	if (rsh == 1) {
		gsw_r32(cdev, PQM_RS_RS2_OFFSET + (qid * 2),
			PQM_RS_RS2_SHIFT, PQM_RS_RS2_SIZE, &value);
		parm->nRateShaperId = value;
	}

	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_misc);
#endif
	return ret;

}

GSW_return_t GSW_QoS_StormCfgSet(void *cdev,
				 GSW_QoS_stormCfg_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 SCONBC, SCONMC, SCONUC, mid;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_misc);
#endif

	if ((parm->bBroadcast == 0) &&
	    (parm->bMulticast == 0)	&&
	    (parm->bUnknownUnicast == 0)) {
		/*  Storm Control Mode  */
		gsw_w32(cdev, PCE_GCTRL_0_SCONMOD_OFFSET,
			PCE_GCTRL_0_SCONMOD_SHIFT,
			PCE_GCTRL_0_SCONMOD_SIZE, 0);
		/* Meter instances used for broadcast traffic */
		gsw_w32(cdev, PCE_GCTRL_0_SCONBC_OFFSET,
			PCE_GCTRL_0_SCONBC_SHIFT,
			PCE_GCTRL_0_SCONBC_SIZE, 0);
		/* Meter instances used for multicast traffic */
		gsw_w32(cdev, PCE_GCTRL_0_SCONMC_OFFSET,
			PCE_GCTRL_0_SCONMC_SHIFT,
			PCE_GCTRL_0_SCONMC_SIZE, 0);
		/* Meter instances used for unknown unicast traffic */
		gsw_w32(cdev, PCE_GCTRL_0_SCONUC_OFFSET,
			PCE_GCTRL_0_SCONUC_SHIFT,
			PCE_GCTRL_0_SCONUC_SIZE, 0);
	}

	/*  Meter ID */
	gsw_r32(cdev, PCE_GCTRL_0_SCONMET_OFFSET,
		PCE_GCTRL_0_SCONMET_SHIFT,
		PCE_GCTRL_0_SCONMET_SIZE, &mid);
	/* Meter instances used for broadcast traffic */
	gsw_r32(cdev, PCE_GCTRL_0_SCONBC_OFFSET,
		PCE_GCTRL_0_SCONBC_SHIFT,
		PCE_GCTRL_0_SCONBC_SIZE, &SCONBC);
	/* Meter instances used for multicast traffic */
	gsw_r32(cdev, PCE_GCTRL_0_SCONMC_OFFSET,
		PCE_GCTRL_0_SCONMC_SHIFT,
		PCE_GCTRL_0_SCONMC_SIZE, &SCONMC);
	/* Meter instances used for unknown unicast traffic */
	gsw_r32(cdev, PCE_GCTRL_0_SCONUC_OFFSET,
		PCE_GCTRL_0_SCONUC_SHIFT,
		PCE_GCTRL_0_SCONUC_SIZE, &SCONUC);

	if ((SCONBC == 1) || (SCONMC == 1) || (SCONMC == 1)) {
		if (parm->nMeterId == (mid + 1)) {
			/*  Storm Control Mode  */
			gsw_w32(cdev, PCE_GCTRL_0_SCONMOD_OFFSET,
				PCE_GCTRL_0_SCONMOD_SHIFT,
				PCE_GCTRL_0_SCONMOD_SIZE, 3);
		} else if (parm->nMeterId != mid) {
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}
	} else {
		/*  Storm Control Mode */
		gsw_w32(cdev, PCE_GCTRL_0_SCONMOD_OFFSET,
			PCE_GCTRL_0_SCONMOD_SHIFT,
			PCE_GCTRL_0_SCONMOD_SIZE, 1);
		gsw_w32(cdev, PCE_GCTRL_0_SCONMET_OFFSET,
			PCE_GCTRL_0_SCONMET_SHIFT,
			PCE_GCTRL_0_SCONMET_SIZE, parm->nMeterId);
	}

	/* Meter instances used for broadcast traffic */
	gsw_w32(cdev, PCE_GCTRL_0_SCONBC_OFFSET,
		PCE_GCTRL_0_SCONBC_SHIFT,
		PCE_GCTRL_0_SCONBC_SIZE, parm->bBroadcast);
	/* Meter instances used for multicast traffic */
	gsw_w32(cdev, PCE_GCTRL_0_SCONMC_OFFSET,
		PCE_GCTRL_0_SCONMC_SHIFT,
		PCE_GCTRL_0_SCONMC_SIZE, parm->bMulticast);
	/* Meter instances used for unknown unicast traffic */
	gsw_w32(cdev, PCE_GCTRL_0_SCONUC_OFFSET,
		PCE_GCTRL_0_SCONUC_SHIFT,
		PCE_GCTRL_0_SCONUC_SIZE, parm->bUnknownUnicast);
	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_misc);
#endif
	return ret;

}

GSW_return_t GSW_QoS_StormCfgGet(void *cdev,
				 GSW_QoS_stormCfg_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 value;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_misc);
#endif

	/*  Storm Control Mode  */
	gsw_r32(cdev, PCE_GCTRL_0_SCONMOD_OFFSET,
		PCE_GCTRL_0_SCONMOD_SHIFT,
		PCE_GCTRL_0_SCONMOD_SIZE, &value);

	if (value == 0) {
		parm->nMeterId = 0;
		parm->bBroadcast = 0;
		parm->bMulticast = 0;
		parm->bUnknownUnicast =  0;
	} else {
		gsw_r32(cdev, PCE_GCTRL_0_SCONMET_OFFSET,
			PCE_GCTRL_0_SCONMET_SHIFT,
			PCE_GCTRL_0_SCONMET_SIZE, &value);
		parm->nMeterId = value;
		/* Meter instances used for broadcast traffic */
		gsw_r32(cdev, PCE_GCTRL_0_SCONBC_OFFSET,
			PCE_GCTRL_0_SCONBC_SHIFT,
			PCE_GCTRL_0_SCONBC_SIZE, &value);
		parm->bBroadcast = value;
		/* Meter instances used for multicast traffic */
		gsw_r32(cdev, PCE_GCTRL_0_SCONMC_OFFSET,
			PCE_GCTRL_0_SCONMC_SHIFT,
			PCE_GCTRL_0_SCONMC_SIZE, &value);
		parm->bMulticast = value;
		/* Meter instances used for unknown unicast traffic */
		gsw_r32(cdev, PCE_GCTRL_0_SCONUC_OFFSET,
			PCE_GCTRL_0_SCONUC_SHIFT,
			PCE_GCTRL_0_SCONUC_SIZE, &value);
		parm->bUnknownUnicast = value;
	}

	ret = GSW_statusOk;

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_misc);
#endif
	return ret;
}

GSW_return_t GSW_QoS_WredCfgGet(void *cdev,
				GSW_QoS_WRED_Cfg_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u8 Gl_Mod_Size = 0;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_misc);
#endif

	/* Description: 'Drop Probability Profile' */
	gsw_r32(cdev, BM_QUEUE_GCTRL_DPROB_OFFSET,
		BM_QUEUE_GCTRL_DPROB_SHIFT,
		BM_QUEUE_GCTRL_DPROB_SIZE, &parm->eProfile);

	/*Automatic/Manual - Adaptive Watermark Type */
	if (IS_VRSN_30_31(gswdev->gipver)) {
		gsw_r32(cdev, BM_QUEUE_GCTRL_BUFMOD_OFFSET,
			BM_QUEUE_GCTRL_BUFMOD_SHIFT,
			BM_QUEUE_GCTRL_BUFMOD_SIZE, &parm->eMode);
	}

	if ((gswdev->gipver == LTQ_GSWIP_2_2) ||
	    (IS_VRSN_30_31(gswdev->gipver)))
		Gl_Mod_Size = BM_QUEUE_GCTRL_GL_MOD_SIZE2;
	else
		Gl_Mod_Size = BM_QUEUE_GCTRL_GL_MOD_SIZE;

	/* Get the Local/Global threshold */
	gsw_r32(cdev, BM_QUEUE_GCTRL_GL_MOD_OFFSET,
		BM_QUEUE_GCTRL_GL_MOD_SHIFT,
		Gl_Mod_Size, &parm->eThreshMode);
	/* WRED Red Threshold - Minimum */
	gsw_r32(cdev, BM_WRED_RTH_0_MINTH_OFFSET,
		BM_WRED_RTH_0_MINTH_SHIFT,
		BM_WRED_RTH_0_MINTH_SIZE, &parm->nRed_Min);
	/* WRED Red Threshold - Maximum */
	gsw_r32(cdev, BM_WRED_RTH_1_MAXTH_OFFSET,
		BM_WRED_RTH_1_MAXTH_SHIFT,
		BM_WRED_RTH_1_MAXTH_SIZE, &parm->nRed_Max);
	/* WRED Yellow Threshold - Minimum */
	gsw_r32(cdev, BM_WRED_YTH_0_MINTH_OFFSET,
		BM_WRED_YTH_0_MINTH_SHIFT,
		BM_WRED_YTH_0_MINTH_SIZE, &parm->nYellow_Min);
	/* WRED Yellow Threshold - Maximum */
	gsw_r32(cdev, BM_WRED_YTH_1_MAXTH_OFFSET,
		BM_WRED_YTH_1_MAXTH_SHIFT,
		BM_WRED_YTH_1_MAXTH_SIZE, &parm->nYellow_Max);
	/* WRED Green Threshold - Minimum */
	gsw_r32(cdev, BM_WRED_GTH_0_MINTH_OFFSET,
		BM_WRED_GTH_0_MINTH_SHIFT,
		BM_WRED_GTH_0_MINTH_SIZE, &parm->nGreen_Min);
	/* WRED Green Threshold - Maximum */
	gsw_r32(cdev, BM_WRED_GTH_1_MAXTH_OFFSET,
		BM_WRED_GTH_1_MAXTH_SHIFT,
		BM_WRED_GTH_1_MAXTH_SIZE, &parm->nGreen_Max);
	ret = GSW_statusOk;

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_misc);
#endif
	return ret;

}

GSW_return_t GSW_QoS_WredCfgSet(void *cdev,
				GSW_QoS_WRED_Cfg_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u8 Gl_Mod_Size = 0;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_misc);
#endif

	/* Description: 'Drop Probability Profile' */
	gsw_w32(cdev, BM_QUEUE_GCTRL_DPROB_OFFSET,
		BM_QUEUE_GCTRL_DPROB_SHIFT,
		BM_QUEUE_GCTRL_DPROB_SIZE, parm->eProfile);

	/* Automatic/Manual - Adaptive Watermark Type */
	if (IS_VRSN_30_31(gswdev->gipver)) {
		gsw_w32(cdev, BM_QUEUE_GCTRL_BUFMOD_OFFSET,
			BM_QUEUE_GCTRL_BUFMOD_SHIFT,
			BM_QUEUE_GCTRL_BUFMOD_SIZE, parm->eMode);
	}

	//GL_MOD size
	if ((gswdev->gipver == LTQ_GSWIP_2_2) ||
	    (IS_VRSN_30_31(gswdev->gipver)))
		Gl_Mod_Size = BM_QUEUE_GCTRL_GL_MOD_SIZE2;
	else
		Gl_Mod_Size = BM_QUEUE_GCTRL_GL_MOD_SIZE;

	switch (parm->eThreshMode) {
	case GSW_QOS_WRED_Local_Thresh:
		/* Set the Local threshold */
		gsw_w32(cdev, BM_QUEUE_GCTRL_GL_MOD_OFFSET,
			BM_QUEUE_GCTRL_GL_MOD_SHIFT,
			Gl_Mod_Size, 0x0);
		break;

	case GSW_QOS_WRED_Global_Thresh:
		/* Set the global threshold */
		gsw_w32(cdev, BM_QUEUE_GCTRL_GL_MOD_OFFSET,
			BM_QUEUE_GCTRL_GL_MOD_SHIFT,
			Gl_Mod_Size, 0x1);
		break;

	case GSW_QOS_WRED_Port_Thresh:
		/* Port queue and Port WRED Thresholds */
		gsw_w32(cdev, BM_QUEUE_GCTRL_GL_MOD_OFFSET,
			BM_QUEUE_GCTRL_GL_MOD_SHIFT,
			Gl_Mod_Size, 0x2);
		break;

	default:
		/* Set the Local threshold */
		gsw_w32(cdev, BM_QUEUE_GCTRL_GL_MOD_OFFSET,
			BM_QUEUE_GCTRL_GL_MOD_SHIFT,
			Gl_Mod_Size, 0x0);
	}

	/* WRED Red Threshold - Minimum */
	gsw_w32(cdev, BM_WRED_RTH_0_MINTH_OFFSET,
		BM_WRED_RTH_0_MINTH_SHIFT,
		BM_WRED_RTH_0_MINTH_SIZE, parm->nRed_Min);
	/* WRED Red Threshold - Maximum */
	gsw_w32(cdev, BM_WRED_RTH_1_MAXTH_OFFSET,
		BM_WRED_RTH_1_MAXTH_SHIFT,
		BM_WRED_RTH_1_MAXTH_SIZE, parm->nRed_Max);
	/* WRED Yellow Threshold - Minimum */
	gsw_w32(cdev, BM_WRED_YTH_0_MINTH_OFFSET,
		BM_WRED_YTH_0_MINTH_SHIFT,
		BM_WRED_YTH_0_MINTH_SIZE, parm->nYellow_Min);
	/* WRED Yellow Threshold - Maximum */
	gsw_w32(cdev, BM_WRED_YTH_1_MAXTH_OFFSET,
		BM_WRED_YTH_1_MAXTH_SHIFT,
		BM_WRED_YTH_1_MAXTH_SIZE, parm->nYellow_Max);
	/* WRED Green Threshold - Minimum */
	gsw_w32(cdev, BM_WRED_GTH_0_MINTH_OFFSET,
		BM_WRED_GTH_0_MINTH_SHIFT,
		BM_WRED_GTH_0_MINTH_SIZE, parm->nGreen_Min);
	/* WRED Green Threshold - Maximum */
	gsw_w32(cdev, BM_WRED_GTH_1_MAXTH_OFFSET,
		BM_WRED_GTH_1_MAXTH_SHIFT,
		BM_WRED_GTH_1_MAXTH_SIZE, parm->nGreen_Max);
	ret = GSW_statusOk;

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_misc);
#endif
	return ret;
}

GSW_return_t GSW_QoS_WredQueueCfgGet(void *cdev,
				     GSW_QoS_WRED_QueueCfg_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 qid = parm->nQueueId, addr;
	u8 crcode;
	u32 ret;
	bmtbl_prog_t bmtable = {0};

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_bm);
#endif

	/*	gsw_r32(cdev, ETHSW_CAP_1_QUEUE_OFFSET, */
	/*		ETHSW_CAP_1_QUEUE_SHIFT, */
	/*		ETHSW_CAP_1_QUEUE_SIZE, &value); */
	if (qid >= gswdev->num_of_queues) {
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	/* For different color 0(not drop) 1(Green) 2(Yellow) 3(Red) */
	for (crcode = 1; crcode < 4; crcode++) {
		addr = ((qid << 3) | crcode);

		bmtable.adr.raw = (u16)addr;
		bmtable.numValues = 2;
		bmtable.tableID = (BM_Table_ID) PQM_THRESHOLD;
		gsw_bm_table_read(cdev, &bmtable);

		switch (crcode) {
		case 3:
			parm->nRed_Max = bmtable.value[1];
			parm->nRed_Min = bmtable.value[0];
			break;

		case 2:
			parm->nYellow_Max = bmtable.value[1];
			parm->nYellow_Min = bmtable.value[0];
			break;

		case 1:
			parm->nGreen_Max = bmtable.value[1];
			parm->nGreen_Min = bmtable.value[0];
			break;
		}
	}

	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_bm);
#endif
	return ret;
}

GSW_return_t GSW_QoS_WredQueueCfgSet(void *cdev,
				     GSW_QoS_WRED_QueueCfg_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 qid = parm->nQueueId, addr;
	bmtbl_prog_t bmtable = {0};
	u8 crcode;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_bm);
#endif

	if (qid >= gswdev->num_of_queues) {
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	/* For different color 0(not drop) 1(Green) 2(Yellow) 3(Red) */
	for (crcode = 1; crcode < 4; crcode++) {
		addr = (((qid << 3) & 0xF8) | crcode);

		bmtable.adr.raw = (u16)addr;

		switch (crcode) {
		case 3:
			bmtable.value[1] = parm->nRed_Max;
			bmtable.value[0] = parm->nRed_Min;
			break;

		case 2:
			bmtable.value[1] = parm->nYellow_Max;
			bmtable.value[0] = parm->nYellow_Min;
			break;

		case 1:
			bmtable.value[1] = parm->nGreen_Max;
			bmtable.value[0] = parm->nGreen_Min;
			break;
		}

		bmtable.tableID = PQM_THRESHOLD;
		bmtable.numValues = 2;
		gsw_bm_table_write(cdev, &bmtable);
	}

	/* Set the local threshold */
	/*	gsw_w32(cdev, BM_QUEUE_GCTRL_GL_MOD_OFFSET, */
	/*		BM_QUEUE_GCTRL_GL_MOD_SHIFT, */
	/*		BM_QUEUE_GCTRL_GL_MOD_SIZE, 0); */
	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_bm);
#endif
	return ret;

}

GSW_return_t GSW_QoS_WredPortCfgGet(void *cdev,
				    GSW_QoS_WRED_PortCfg_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_misc);
#endif

	if (parm->nPortId >= gswdev->tpnum) {
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	/* Supported for GSWIP 2.2 and newer and returns with*/
	/*  an error for older hardware revisions. */
	/** WRED Red Threshold Min [number of segments].*/
	if ((gswdev->gipver == LTQ_GSWIP_2_2) ||
	    (gswdev->gipver == LTQ_GSWIP_2_2_ETC) ||
	    (IS_VRSN_30_31(gswdev->gipver))) {
		gsw_r32(cdev, (BM_PWRED_RTH_0_MINTH_OFFSET
			       + (parm->nPortId * 0x6)),
			BM_PWRED_RTH_0_MINTH_SHIFT,
			BM_PWRED_RTH_0_MINTH_SIZE, &parm->nRed_Min);
		/** WRED Red Threshold Max [number of segments].*/
		gsw_r32(cdev, (BM_PWRED_RTH_1_MAXTH_OFFSET
			       + (parm->nPortId * 0x6)),
			BM_PWRED_RTH_1_MAXTH_SHIFT,
			BM_PWRED_RTH_1_MAXTH_SIZE, &parm->nRed_Max);
		/* WRED Yellow Threshold - Minimum */
		gsw_r32(cdev, (BM_PWRED_YTH_0_MINTH_OFFSET
			       + (parm->nPortId * 0x6)),
			BM_PWRED_YTH_0_MINTH_SHIFT,
			BM_PWRED_YTH_0_MINTH_SIZE, &parm->nYellow_Min);
		/* WRED Yellow Threshold - Maximum */
		gsw_r32(cdev, (BM_PWRED_YTH_1_MAXTH_OFFSET
			       + (parm->nPortId * 0x6)),
			BM_PWRED_YTH_1_MAXTH_SHIFT,
			BM_PWRED_YTH_1_MAXTH_SIZE, &parm->nYellow_Max);
		/* WRED Green Threshold - Minimum */
		gsw_r32(cdev, (BM_PWRED_GTH_0_MINTH_OFFSET
			       + (parm->nPortId * 0x6)),
			BM_PWRED_GTH_0_MINTH_SHIFT,
			BM_PWRED_GTH_0_MINTH_SIZE, &parm->nGreen_Min);
		/* WRED Green Threshold - Maximum */
		gsw_r32(cdev, (BM_PWRED_GTH_1_MAXTH_OFFSET
			       + (parm->nPortId * 0x6)),
			BM_PWRED_GTH_1_MAXTH_SHIFT,
			BM_PWRED_GTH_1_MAXTH_SIZE, &parm->nGreen_Max);
		ret = GSW_statusOk;
	} else {
		ret = GSW_statusErr;
	}

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_misc);
#endif
	return ret;
}

GSW_return_t GSW_QoS_WredPortCfgSet(void *cdev,
				    GSW_QoS_WRED_PortCfg_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_misc);
#endif

	if (parm->nPortId >= gswdev->tpnum) {
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	/* Supported for GSWIP 2.2 and newer and returns */
	/* with an error for older hardware revisions. */
	/** WRED Red Threshold Min [number of segments]. */
	if ((gswdev->gipver == LTQ_GSWIP_2_2) ||
	    (gswdev->gipver == LTQ_GSWIP_2_2_ETC) ||
	    (IS_VRSN_30_31(gswdev->gipver))) {
		gsw_w32(cdev, (BM_PWRED_RTH_0_MINTH_OFFSET
			       + (parm->nPortId * 0x6)),
			BM_PWRED_RTH_0_MINTH_SHIFT,
			BM_PWRED_RTH_0_MINTH_SIZE, parm->nRed_Min);
		/** WRED Red Threshold Max [number of segments]. */
		gsw_w32(cdev, (BM_PWRED_RTH_1_MAXTH_OFFSET
			       + (parm->nPortId * 0x6)),
			BM_PWRED_RTH_1_MAXTH_SHIFT,
			BM_PWRED_RTH_1_MAXTH_SIZE, parm->nRed_Max);
		/* WRED Yellow Threshold - Minimum */
		gsw_w32(cdev, (BM_PWRED_YTH_0_MINTH_OFFSET
			       + (parm->nPortId * 0x6)),
			BM_PWRED_YTH_0_MINTH_SHIFT,
			BM_PWRED_YTH_0_MINTH_SIZE, parm->nYellow_Min);
		/* WRED Yellow Threshold - Maximum */
		gsw_w32(cdev, (BM_PWRED_YTH_1_MAXTH_OFFSET
			       + (parm->nPortId * 0x6)),
			BM_PWRED_YTH_1_MAXTH_SHIFT,
			BM_PWRED_YTH_1_MAXTH_SIZE, parm->nYellow_Max);
		/* WRED Green Threshold - Minimum */
		gsw_w32(cdev, (BM_PWRED_GTH_0_MINTH_OFFSET
			       + (parm->nPortId * 0x6)),
			BM_PWRED_GTH_0_MINTH_SHIFT,
			BM_PWRED_GTH_0_MINTH_SIZE, parm->nGreen_Min);
		/* WRED Green Threshold - Maximum */
		gsw_w32(cdev, (BM_PWRED_GTH_1_MAXTH_OFFSET
			       + (parm->nPortId * 0x6)),
			BM_PWRED_GTH_1_MAXTH_SHIFT,
			BM_PWRED_GTH_1_MAXTH_SIZE, parm->nGreen_Max);
		ret = GSW_statusOk;
	} else {
		ret = GSW_statusErr;
	}

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_misc);
#endif
	return ret;
}

GSW_return_t GSW_QoS_FlowctrlCfgGet(void *cdev,
				    GSW_QoS_FlowCtrlCfg_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_misc);
#endif

	/* Supported for GSWIP 2.2 and newer and returns */
	/* with an error for older hardware revisions. */
	/** Global Buffer Non Conforming Flow Control */
	/*  Threshold Minimum [number of segments]. */
	if ((gswdev->gipver == LTQ_GSWIP_2_2) ||
	    (gswdev->gipver == LTQ_GSWIP_2_2_ETC) ||
	    (IS_VRSN_30_31(gswdev->gipver))) {
		gsw_r32(cdev, (SDMA_FCTHR1_THR1_OFFSET),
			SDMA_FCTHR1_THR1_SHIFT,
			SDMA_FCTHR1_THR1_SIZE,
			&parm->nFlowCtrlNonConform_Min);
		/** Global Buffer Non Conforming Flow Control */
		/* Threshold Maximum [number of segments]. */
		gsw_r32(cdev, (SDMA_FCTHR2_THR2_OFFSET),
			SDMA_FCTHR2_THR2_SHIFT,
			SDMA_FCTHR2_THR2_SIZE,
			&parm->nFlowCtrlNonConform_Max);
		/** Global Buffer Conforming Flow Control Threshold */
		/*  Minimum [number of segments]. */
		gsw_r32(cdev, (SDMA_FCTHR3_THR3_OFFSET),
			SDMA_FCTHR3_THR3_SHIFT,
			SDMA_FCTHR3_THR3_SIZE,
			&parm->nFlowCtrlConform_Min);
		/** Global Buffer Conforming Flow Control */
		/* Threshold Maximum [number of segments]. */
		gsw_r32(cdev, (SDMA_FCTHR4_THR4_OFFSET),
			SDMA_FCTHR4_THR4_SHIFT,
			SDMA_FCTHR4_THR4_SIZE,
			&parm->nFlowCtrlConform_Max);
		ret = GSW_statusOk;
	} else {
		ret = GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_misc);
#endif
	return ret;

}

GSW_return_t GSW_QoS_FlowctrlCfgSet(void *cdev,
				    GSW_QoS_FlowCtrlCfg_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_misc);
#endif

	/* Supported for GSWIP 2.2 and newer and returns with */
	/* an error for older hardware revisions. */
	/** Global Buffer Non Conforming Flow Control */
	/* Threshold Minimum [number of segments]. */
	if ((gswdev->gipver == LTQ_GSWIP_2_2) ||
	    (gswdev->gipver == LTQ_GSWIP_2_2_ETC) ||
	    (IS_VRSN_30_31(gswdev->gipver))) {
		gsw_w32(cdev, (SDMA_FCTHR1_THR1_OFFSET),
			SDMA_FCTHR1_THR1_SHIFT,
			SDMA_FCTHR1_THR1_SIZE,
			parm->nFlowCtrlNonConform_Min);
		/** Global Buffer Non Conforming Flow Control */
		/* Threshold Maximum [number of segments]. */
		gsw_w32(cdev, (SDMA_FCTHR2_THR2_OFFSET),
			SDMA_FCTHR2_THR2_SHIFT,
			SDMA_FCTHR2_THR2_SIZE,
			parm->nFlowCtrlNonConform_Max);
		/** Global Buffer Conforming Flow Control Threshold */
		/*  Minimum [number of segments]. */
		gsw_w32(cdev, (SDMA_FCTHR3_THR3_OFFSET),
			SDMA_FCTHR3_THR3_SHIFT,
			SDMA_FCTHR3_THR3_SIZE,
			parm->nFlowCtrlConform_Min);
		/** Global Buffer Conforming Flow Control Threshold */
		/*  Maximum [number of segments]. */
		gsw_w32(cdev, (SDMA_FCTHR4_THR4_OFFSET),
			SDMA_FCTHR4_THR4_SHIFT,
			SDMA_FCTHR4_THR4_SIZE,
			parm->nFlowCtrlConform_Max);
		ret = GSW_statusOk;
	} else {
		ret = GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_misc);
#endif
	return ret;

}

GSW_return_t GSW_QoS_FlowctrlPortCfgGet(void *cdev,
					GSW_QoS_FlowCtrlPortCfg_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_misc);
#endif

	if (parm->nPortId >= gswdev->tpnum) {
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	/* Supported for GSWIP 2.2 and newer and returns with */
	/* an error for older hardware revisions. */
	/** Ingress Port occupied Buffer Flow Control */
	/* Threshold Minimum [number of segments]. */
	if ((gswdev->gipver == LTQ_GSWIP_2_2) ||
	    (gswdev->gipver == LTQ_GSWIP_2_2_ETC)) {
		gsw_r32(cdev, (SDMA_PFCTHR8_THR8_OFFSET22
			       + (parm->nPortId * 0x4)),
			SDMA_PFCTHR8_THR8_SHIFT,
			SDMA_PFCTHR8_THR8_SIZE, &parm->nFlowCtrl_Min);
		/** Ingress Port occupied Buffer Flow Control */
		/* Threshold Maximum [number of segments]. */
		gsw_r32(cdev, (SDMA_PFCTHR9_THR9_OFFSET22
			       + (parm->nPortId * 0x4)),
			SDMA_PFCTHR9_THR9_SHIFT,
			SDMA_PFCTHR9_THR9_SIZE, &parm->nFlowCtrl_Max);
		ret = GSW_statusOk;
	} else if (IS_VRSN_30_31(gswdev->gipver)) {
		/** Ingress Port occupied Buffer Flow Control */
		/* Threshold Minimum [number of segments]. */
		gsw_r32(cdev, (SDMA_PFCTHR8_THR8_OFFSET30
			       + (parm->nPortId * 0x2)),
			SDMA_PFCTHR8_THR8_SHIFT,
			SDMA_PFCTHR8_THR8_SIZE, &parm->nFlowCtrl_Min);
		/** Ingress Port occupied Buffer Flow Control */
		/* Threshold Maximum [number of segments]. */
		gsw_r32(cdev, (SDMA_PFCTHR9_THR9_OFFSET30
			       + (parm->nPortId * 0x2)),
			SDMA_PFCTHR9_THR9_SHIFT,
			SDMA_PFCTHR9_THR9_SIZE,
			&parm->nFlowCtrl_Max);
		ret = GSW_statusOk;
	} else {
		ret = GSW_statusErr;
	}

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_misc);
#endif
	return ret;

}

GSW_return_t GSW_QoS_FlowctrlPortCfgSet(void *cdev,
					GSW_QoS_FlowCtrlPortCfg_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_misc);
#endif

	if (parm->nPortId >= gswdev->tpnum) {
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	/* Supported for GSWIP 2.2 and newer and returns with */
	/*  an error for older hardware revisions. */
	/** Ingress Port occupied Buffer Flow Control */
	/* Threshold Minimum [number of segments]. */
	if ((gswdev->gipver == LTQ_GSWIP_2_2) ||
	    (gswdev->gipver == LTQ_GSWIP_2_2_ETC)) {
		gsw_w32(cdev, (SDMA_PFCTHR8_THR8_OFFSET22
			       + (parm->nPortId * 0x4)),
			SDMA_PFCTHR8_THR8_SHIFT,
			SDMA_PFCTHR8_THR8_SIZE, parm->nFlowCtrl_Min);
		/** Ingress Port occupied Buffer Flow Control Threshold */
		/* Maximum [number of segments]. */
		gsw_w32(cdev, (SDMA_PFCTHR9_THR9_OFFSET22
			       + (parm->nPortId * 0x4)),
			SDMA_PFCTHR9_THR9_SHIFT,
			SDMA_PFCTHR9_THR9_SIZE, parm->nFlowCtrl_Max);
		ret = GSW_statusOk;
	} else if (IS_VRSN_30_31(gswdev->gipver)) {
		/** Ingress Port occupied Buffer Flow Control Threshold*/
		/*  Minimum [number of segments]. */
		gsw_w32(cdev, (SDMA_PFCTHR8_THR8_OFFSET30
			       + (parm->nPortId * 0x2)),
			SDMA_PFCTHR8_THR8_SHIFT,
			SDMA_PFCTHR8_THR8_SIZE, parm->nFlowCtrl_Min);
		/** Ingress Port occupied Buffer Flow Control Threshold */
		/*  Maximum [number of segments]. */
		gsw_w32(cdev, (SDMA_PFCTHR9_THR9_OFFSET30
			       + (parm->nPortId * 0x2)),
			SDMA_PFCTHR9_THR9_SHIFT,
			SDMA_PFCTHR9_THR9_SIZE, parm->nFlowCtrl_Max);
		ret = GSW_statusOk;
	} else {
		ret = GSW_statusErr;
	}

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_misc);
#endif
	return ret;
}

GSW_return_t GSW_QoS_QueueBufferReserveCfgGet(void *cdev,
		GSW_QoS_QueueBufferReserveCfg_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	bmtbl_prog_t bmtable = {0};
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_bm);
#endif

	/*	gsw_r32(cdev, ETHSW_CAP_1_QUEUE_OFFSET, */
	/*		ETHSW_CAP_1_QUEUE_SHIFT, */
	/*		ETHSW_CAP_1_QUEUE_SIZE, &value); */
	if (parm->nQueueId >= gswdev->num_of_queues) {
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	/* Supported for GSWIP 2.2 and newer and returns with an error */
	/*  for older hardware revisions. */
	/* Colourcode = 0  and fixed offset = 0 */
	/* Set the BM RAM ADDR  */
	if ((gswdev->gipver == LTQ_GSWIP_2_2) ||
	    (gswdev->gipver == LTQ_GSWIP_2_2_ETC) ||
	    (IS_VRSN_30_31(gswdev->gipver))) {
		/* What about mode2, mode3 and mode1 with color  */
		bmtable.adr.pqmThr.mode = 0; /*BM_RAM_ADDR.ADDR[2]=0 -> Mode 1*/
		bmtable.adr.pqmThr.color_or_submode = 0; /* color = 0 (Not Drop eligible)*/
		bmtable.adr.pqmThr.nQueueId = parm->nQueueId;
		bmtable.tableID = (BM_Table_ID) PQM_THRESHOLD;
		bmtable.numValues = 2;
		gsw_bm_table_read(cdev, &bmtable);
		/*Minimum threshold (used as reserved buffer threshold when color = 0)*/
		parm->nBufferReserved = bmtable.value[0];
		/* value[1] Maximum threshold (not used when color = 0) */
		ret = GSW_statusOk;
	} else {
		ret = GSW_statusErr;
	}

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_bm);
#endif
	return ret;

}

GSW_return_t GSW_QoS_QueueBufferReserveCfgSet(void *cdev,
		GSW_QoS_QueueBufferReserveCfg_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	bmtbl_prog_t bmtable = {0};
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_bm);
#endif

	/*	gsw_r32(cdev, ETHSW_CAP_1_QUEUE_OFFSET, */
	/*		ETHSW_CAP_1_QUEUE_SHIFT, */
	/*		ETHSW_CAP_1_QUEUE_SIZE, &value); */

	if (parm->nQueueId >= gswdev->num_of_queues) {
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	/* Supported for GSWIP 2.2 and newer and returns with an error */
	/* for older hardware revisions. */
	if ((gswdev->gipver == LTQ_GSWIP_2_2) ||
	    (gswdev->gipver == LTQ_GSWIP_2_2_ETC) ||
	    (IS_VRSN_30_31(gswdev->gipver))) {
		/* What about mode1 with color & mode2, mode3 */
		bmtable.adr.pqmThr.mode = 0; /*BM_RAM_ADDR.ADDR[2]=0 -> Mode 1*/
		bmtable.adr.pqmThr.color_or_submode = 0; /* color = 0 (Not Drop eligible)*/
		bmtable.adr.pqmThr.nQueueId = parm->nQueueId;
		bmtable.value[0] = parm->nBufferReserved;
		/* value[1] Maximum threshold (not used when color = 0) */
		bmtable.tableID = PQM_THRESHOLD;
		bmtable.numValues = 2;
		ret = gsw_bm_table_write(cdev, &bmtable);
	} else {
		ret = GSW_statusErr;
	}

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_bm);
#endif
	return ret;
}
GSW_return_t GSW_QoS_Meter_Act(void *cdev,
			       GSW_QoS_mtrAction_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32  value;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_misc);
#endif

	if (gswdev->gipver == LTQ_GSWIP_3_0) {
		if (parm->nCpuUserId >= 2) {
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}

		value = parm->nMeterId;
		gsw_w32(cdev, (PCE_CPUMETER_MID0_MID_OFFSET
			       + (parm->nCpuUserId * 0x8)),
			PCE_CPUMETER_MID0_MID_SHIFT,
			PCE_CPUMETER_MID0_MID_SIZE, value);
		gsw_w32(cdev, (PCE_CPUMETER_CTRL_MT0EN_OFFSET
			       + (parm->nCpuUserId * 0x8)),
			PCE_CPUMETER_CTRL_MT0EN_SHIFT,
			PCE_CPUMETER_CTRL_MT0EN_SIZE, parm->bMeterEna);

		value = parm->nSecMeterId;
		gsw_w32(cdev, (PCE_CPUMETER_MID1_MID_OFFSET
			       + (parm->nCpuUserId * 0x8)),
			PCE_CPUMETER_MID1_MID_SHIFT,
			PCE_CPUMETER_MID1_MID_SIZE, value);
		gsw_w32(cdev, (PCE_CPUMETER_CTRL_MT1EN_OFFSET
			       + (parm->nCpuUserId * 0x8)),
			PCE_CPUMETER_CTRL_MT1EN_SHIFT,
			PCE_CPUMETER_CTRL_MT1EN_SIZE, parm->bSecMeterEna);

		value = parm->pktLen;
		gsw_w32(cdev, (PCE_CPUMETER_SIZE_SIZE_OFFSET
			       + (parm->nCpuUserId * 0x8)),
			PCE_CPUMETER_SIZE_SIZE_SHIFT,
			PCE_CPUMETER_SIZE_SIZE_SIZE, value);

		gsw_w32(cdev, (PCE_CPUMETER_CTRL_PRECOL_OFFSET
			       + (parm->nCpuUserId * 0x8)),
			PCE_CPUMETER_CTRL_PRECOL_SHIFT,
			PCE_CPUMETER_CTRL_PRECOL_SIZE, parm->ePreColor);
		/*
				gsw_w32(cdev, (PCE_CPUMETER_CTRL_REQ_OFFSET
					+ (parm->nCpuUserId * 0x8)),
					PCE_CPUMETER_CTRL_REQ_SHIFT,
					PCE_CPUMETER_CTRL_REQ_SIZE, 1);

				do {
					gsw_r32(cdev, (PCE_CPUMETER_CTRL_REQ_OFFSET
						+ (parm->nCpuUserId * 0x8)),
						PCE_CPUMETER_CTRL_REQ_SHIFT,
						PCE_CPUMETER_CTRL_REQ_SIZE, &value);
				} while(value);

				gsw_w32(cdev, PCE_CPUMETER_CTRL_AFTCOL_OFFSET
					+ (parm->nCpuUserId * 0x8),
					PCE_CPUMETER_CTRL_AFTCOL_SHIFT,
					PCE_CPUMETER_CTRL_AFTCOL_SIZE, parm->eOutColor);
		*/
	}

	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_misc);
#endif
	return ret;

}

#endif  /* CONFIG_LTQ_QOS */

#if defined(CONFIG_LTQ_MULTICAST) && CONFIG_LTQ_MULTICAST
GSW_return_t GSW_MulticastRouterPortAdd(void *cdev,
					GSW_multicastRouter_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	gsw_igmp_t *hitbl = &gswdev->iflag;
	u32 value = 0;
	u8 pidx = parm->nPortId;
	u32 ret;
	static GSW_PCE_rule_t pcrule;
	int i;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_pce);
#endif

	if (parm->nPortId >= gswdev->tpnum) {
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	if ((IS_VRSN_NOT_31(gswdev->gipver)) ||
	    ((IS_VRSN_31(gswdev->gipver)) && (gswdev->mcsthw_snoop == MCAST_HWSNOOP_EN))) {
		//Govind - Below registers are not supported in 3.1. Need to take care of it?
		/* Read the Default Router Port Map - DRPM */
		gsw_r32(cdev, PCE_IGMP_DRPM_DRPM_OFFSET,
			PCE_IGMP_DRPM_DRPM_SHIFT,
			PCE_IGMP_DRPM_DRPM_SIZE, &value);

		if (((value >> pidx) & 0x1) == 1) {
			pr_err("Error: the prot was already in the member\n");
		} else {
			value = (value | (1 << pidx));
			/* Write the Default Router Port Map - DRPM  */
			gsw_w32(cdev, PCE_IGMP_DRPM_DRPM_OFFSET,
				PCE_IGMP_DRPM_DRPM_SHIFT,
				PCE_IGMP_DRPM_DRPM_SIZE, value);
		}
	}

	if ((IS_VRSN_31(gswdev->gipver)) && (gswdev->mcsthw_snoop == MCAST_HWSNOOP_DIS)) {
		//Govind - Returning an error for this case in read API?
		pr_err("CPU Port for sending traffic %d\n", (1 << gswdev->cport));
		// In PCE rule, need to program 1, for PMAC 0
		value = (1 << gswdev->cport);
	}

	if (hitbl->igmode == GSW_MULTICAST_SNOOP_MODE_FORWARD) {


		for (i = 0; i < 2; i++) {
			memset(&pcrule, 0, sizeof(GSW_PCE_rule_t));
			pcrule.pattern.bEnable = 1;
			pcrule.pattern.bProtocolEnable = 1;

			switch (i) {
			case 0:
				/*	Management port remaining IGMP packets (forwarding */
				/* them to Router Ports) */
				pcrule.pattern.nIndex = MPCE_RULES_INDEX;
				pcrule.pattern.nProtocol = 0x2; /* IGMP */
				pcrule.pattern.bAppMaskRangeMSB_Select = 1;
				pcrule.pattern.bAppDataMSB_Enable	= 1;
				pcrule.pattern.nAppDataMSB = 0x1200;
				pcrule.pattern.nAppMaskRangeMSB	= 0x1DFF;
				break;

			case 1:
				/* Management Port ICMPv6 Multicast Listerner Report */
				/* & Leave (Avoiding Loopback abd Discard) */
				pcrule.pattern.nIndex = MPCE_RULES_INDEX + 3;
				pcrule.pattern.bAppDataMSB_Enable	= 1;
				pcrule.pattern.bAppMaskRangeMSB_Select = 1;
				pcrule.pattern.nAppDataMSB = 0x8300;
				pcrule.pattern.nAppMaskRangeMSB	= 0x1FF;
				pcrule.pattern.nProtocol = 0x3A;  /*for IPv6*/
				break;
			}

			/* Router portmap */
			pcrule.action.ePortMapAction =
				GSW_PCE_ACTION_PORTMAP_ALTERNATIVE;
			pcrule.action.nForwardPortMap[0] = value;

			if (hitbl->igcos != 0) {
				pcrule.action.eTrafficClassAction = 1; //Govind - this should be 2?
				pcrule.action.nTrafficClassAlternate = gswdev->iflag.igcos;
			}

			/*  Set eForwardPort */
			pcrule.pattern.bPortIdEnable = 1;

			if (hitbl->igfport == GSW_PORT_FORWARD_PORT)
				pcrule.pattern.nPortId = hitbl->igfpid;
			else if (hitbl->igfport == GSW_PORT_FORWARD_CPU)
				pcrule.pattern.nPortId = (gswdev->cport);

			//Govind - what if both of above 'if' fails? Port-Id '0' will be written.

			if (hitbl->igxvlan)
				pcrule.action.eVLAN_CrossAction =
					GSW_PCE_ACTION_CROSS_VLAN_CROSS;
			else
				pcrule.action.eVLAN_CrossAction =
					GSW_PCE_ACTION_CROSS_VLAN_DISABLE;

			/* We prepare everything and write into PCE Table */
			if (0 != pce_rule_write(cdev,
						&gswdev->phandler, &pcrule)) {
				ret = GSW_statusErr;
				goto UNLOCK_AND_RETURN;
			}
		}
	}

	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_pce);
#endif
	return ret;

}

GSW_return_t GSW_MulticastRouterPortRead(void *cdev,
		GSW_multicastRouterRead_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 value_1, value_2;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_pce);
#endif

	if ((IS_VRSN_31(gswdev->gipver)) && (gswdev->mcsthw_snoop == MCAST_HWSNOOP_DIS)) {
		pr_err("%s:%s not supported in GSWIP v3.1 :%d\n", __FILE__, __func__, __LINE__);
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	//Govind - Below registers are not supported in 3.1. Need to take care of it?
	if (parm->bInitial == 1) {
		/* Read the Default Router Port Map - DRPM*/
		gsw_r32(cdev, PCE_IGMP_DRPM_DRPM_OFFSET,
			PCE_IGMP_DRPM_DRPM_SHIFT,
			PCE_IGMP_DRPM_DRPM_SIZE, &value_1);
		/* Read the Default Router Port Map - IGPM */
		gsw_r32(cdev, PCE_IGMP_STAT_IGPM_OFFSET,
			PCE_IGMP_STAT_IGPM_SHIFT,
			PCE_IGMP_STAT_IGPM_SIZE, &value_2);
		gswdev->iflag.igrport = (value_1 | value_2);
		parm->bInitial = 0;
		gswdev->mrtpcnt = 0;
	}

	if (parm->bLast == 0) {
		/* Need to clarify the different between DRPM & IGPM */
		while (((gswdev->iflag.igrport >>
			 gswdev->mrtpcnt) & 0x1) == 0) {
			gswdev->mrtpcnt++;

			if (gswdev->mrtpcnt > (gswdev->tpnum - 1)) {
				parm->bLast = 1;
				ret = GSW_statusOk;
				goto UNLOCK_AND_RETURN;
			}
		}

		parm->nPortId = gswdev->mrtpcnt;

		if (gswdev->mrtpcnt < gswdev->tpnum)
			gswdev->mrtpcnt++;
		else
			parm->bLast = 1;
	}

	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_pce);
#endif
	return ret;

}

GSW_return_t GSW_MulticastRouterPortRemove(void *cdev,
		GSW_multicastRouter_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	gsw_igmp_t *hitbl	= &gswdev->iflag;
	u32 value_1 = 0, value_2 = 0;
	u8 pidx = parm->nPortId;
	u32 ret;
	static GSW_PCE_rule_t pcrule;
	int i;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_pce);
#endif

	if (parm->nPortId >= gswdev->tpnum) {
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	if ((IS_VRSN_NOT_31(gswdev->gipver)) ||
	    ((IS_VRSN_31(gswdev->gipver)) && (gswdev->mcsthw_snoop == MCAST_HWSNOOP_EN))) {
		//Govind - Below registers are not supported in 3.1. Need to take care of it?
		/* Read the Default Router Port Map - DRPM */
		gsw_r32(cdev, PCE_IGMP_DRPM_DRPM_OFFSET,
			PCE_IGMP_DRPM_DRPM_SHIFT,
			PCE_IGMP_DRPM_DRPM_SIZE, &value_1);
		/* Read the Default Router Port Map - IGPM */
		gsw_r32(cdev, PCE_IGMP_STAT_IGPM_OFFSET,
			PCE_IGMP_STAT_IGPM_SHIFT,
			PCE_IGMP_STAT_IGPM_SIZE, &value_2);

		if (((value_1 >> pidx) & 0x1) == 0) {
			pr_err("Error: the port was not in the member\n");
			ret = GSW_statusOk;
			goto UNLOCK_AND_RETURN;
		} else {
			value_1 = (value_1 & ~(1 << pidx));
			/* Write the Default Router Port Map - DRPM*/
			gsw_w32(cdev, PCE_IGMP_DRPM_DRPM_OFFSET,
				PCE_IGMP_DRPM_DRPM_SHIFT,
				PCE_IGMP_DRPM_DRPM_SIZE, value_1);
		}
	}

	if ((IS_VRSN_31(gswdev->gipver)) && (gswdev->mcsthw_snoop == MCAST_HWSNOOP_DIS)) {
		value_1 = gsw_check_hashtable_entry(cdev);
	}

	//Govind - Again creating a rule instaed of deleting?
	if ((hitbl->igmode ==
	     GSW_MULTICAST_SNOOP_MODE_FORWARD) & value_1) {

		for (i = 0; i < 2; i++) {
			memset(&pcrule, 0, sizeof(GSW_PCE_rule_t));
			pcrule.pattern.bEnable = 1;
			pcrule.pattern.bProtocolEnable = 1;

			switch (i) {
			case 0:
				/*	Management port remaining IGMP packets */
				/*(forwarding them to Router Ports) */
				pcrule.pattern.nIndex = MPCE_RULES_INDEX;
				/* for IPv4 */
				pcrule.pattern.nProtocol = 0x2;
				pcrule.pattern.bAppMaskRangeMSB_Select = 1;
				pcrule.pattern.bAppDataMSB_Enable	= 1;
				pcrule.pattern.nAppDataMSB = 0x1200;
				pcrule.pattern.nAppMaskRangeMSB	= 0x1DFF;
				break;

			case 1:
				/* Management Port ICMPv6 Multicast Listerner Report */
				/* & Leave (Avoiding Loopback abd Discard) */
				pcrule.pattern.nIndex = MPCE_RULES_INDEX + 3;
				pcrule.pattern.bAppDataMSB_Enable	= 1;
				pcrule.pattern.bAppMaskRangeMSB_Select = 1;
				pcrule.pattern.nAppDataMSB = 0x8300;
				pcrule.pattern.nAppMaskRangeMSB	= 0x1FF;
				/*for IPv6*/
				pcrule.pattern.nProtocol = 0x3A;
				break;
			}

			/* Router portmap */
			pcrule.action.ePortMapAction =
				GSW_PCE_ACTION_PORTMAP_ALTERNATIVE;
			pcrule.action.nForwardPortMap[0] = value_1;

			if (hitbl->igcos != 0) {
				pcrule.action.eTrafficClassAction = 1;
				pcrule.action.nTrafficClassAlternate =
					gswdev->iflag.igcos;
			}

			/*  Set eForwardPort */
			pcrule.pattern.bPortIdEnable = 1;

			if (hitbl->igfport == GSW_PORT_FORWARD_PORT)
				pcrule.pattern.nPortId = hitbl->igfpid;
			else if (hitbl->igfport == GSW_PORT_FORWARD_CPU)
				pcrule.pattern.nPortId = (gswdev->cport);

			if (hitbl->igxvlan)
				pcrule.action.eVLAN_CrossAction =
					GSW_PCE_ACTION_CROSS_VLAN_CROSS;
			else
				pcrule.action.eVLAN_CrossAction =
					GSW_PCE_ACTION_CROSS_VLAN_DISABLE;

			/* We prepare everything and write into PCE Table */
			if (pce_rule_write(cdev,
					   &gswdev->phandler, &pcrule) != 0) {
				ret = GSW_statusErr;
				goto UNLOCK_AND_RETURN;
			}
		}
	} else if ((hitbl->igmode ==
		    GSW_MULTICAST_SNOOP_MODE_FORWARD) & !value_1) {

		for (i = 0; i < 2; i++) {
			memset(&pcrule, 0, sizeof(GSW_PCE_rule_t));

			switch (i) {
			case 0:
				pcrule.pattern.nIndex = MPCE_RULES_INDEX;
				break;

			case 1:
				/* Management Port ICMPv6 Multicast Listerner Report */
				/* & Leave (Avoiding Loopback abd Discard) */
				pcrule.pattern.nIndex =
					MPCE_RULES_INDEX + 3;
				break;
			}

			if (pce_pattern_delete(cdev, &gswdev->phandler, pcrule.pattern.nIndex) != 0) {
				ret = GSW_statusErr;
				goto UNLOCK_AND_RETURN;
			}
		}
	}

	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_pce);
#endif
	return ret;
}

//Govind - Is this API compatible for 3.1?
GSW_return_t GSW_MulticastSnoopCfgGet(void *cdev,
				      GSW_multicastSnoopCfg_t *parm)
{
	u32 data_1, data_2, value;
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_pce);
#endif

	if ((IS_VRSN_31(gswdev->gipver)) && (gswdev->mcsthw_snoop == MCAST_HWSNOOP_DIS)) {
		//pr_err("%s:%s not supported in GSWIP v3.1 :%d\n",__FILE__, __func__, __LINE__);
		ret = GSW_statusOk;
		goto UNLOCK_AND_RETURN;
	}

	parm->eIGMP_Mode = gswdev->iflag.igmode;
	parm->bIGMPv3 = gswdev->iflag.igv3;
	parm->bCrossVLAN = gswdev->iflag.igxvlan;
	parm->eForwardPort = gswdev->iflag.igfport;
	parm->nForwardPortId = gswdev->iflag.igfpid;
	parm->nClassOfService = gswdev->iflag.igcos;
	gsw_r32(cdev, PCE_IGMP_CTRL_ROB_OFFSET,
		PCE_IGMP_CTRL_ROB_SHIFT,
		PCE_IGMP_CTRL_ROB_SIZE, &value);
	parm->nRobust = value;
	gsw_r32(cdev, PCE_IGMP_CTRL_DMRT_OFFSET,
		PCE_IGMP_CTRL_DMRT_SHIFT,
		PCE_IGMP_CTRL_DMRT_SIZE, &value);
	parm->nQueryInterval = value;
	gsw_r32(cdev, PCE_IGMP_CTRL_REPSUP_OFFSET,
		PCE_IGMP_CTRL_REPSUP_SHIFT,
		PCE_IGMP_CTRL_REPSUP_SIZE, &data_1);
	gsw_r32(cdev, PCE_IGMP_CTRL_JASUP_OFFSET,
		PCE_IGMP_CTRL_JASUP_SHIFT,
		PCE_IGMP_CTRL_JASUP_SIZE, &data_2);

	if (data_1 == 0 && data_2 == 0)
		parm->eSuppressionAggregation = GSW_MULTICAST_TRANSPARENT;
	else if (data_1 == 1 && data_2 == 0)
		parm->eSuppressionAggregation = GSW_MULTICAST_REPORT;
	else if (data_1 == 1 && data_2 == 1)
		parm->eSuppressionAggregation = GSW_MULTICAST_REPORT_JOIN;
	else
		parm->eSuppressionAggregation = GSW_MULTICAST_TRANSPARENT;

	gsw_r32(cdev, PCE_IGMP_CTRL_FLEAVE_OFFSET,
		PCE_IGMP_CTRL_FLEAVE_SHIFT,
		PCE_IGMP_CTRL_FLEAVE_SIZE, &value);

	if (value == 1)
		parm->bFastLeave = 1;
	else
		parm->bFastLeave = 0;

	gsw_r32(cdev, PCE_IGMP_CTRL_SRPEN_OFFSET,
		PCE_IGMP_CTRL_SRPEN_SHIFT,
		PCE_IGMP_CTRL_SRPEN_SIZE, &value);
	parm->bLearningRouter = value;

	if ((gswdev->gipver == LTQ_GSWIP_2_2) ||
	    (gswdev->gipver == LTQ_GSWIP_2_2_ETC) ||
	    (gswdev->gipver == LTQ_GSWIP_3_0)) {
		gsw_r32(cdev, PCE_GCTRL_1_UKIPMC_OFFSET,
			PCE_GCTRL_1_UKIPMC_SHIFT,
			PCE_GCTRL_1_UKIPMC_SIZE,
			&parm->bMulticastUnknownDrop);
	}

	if (gswdev->gipver == LTQ_GSWIP_3_0) {
		gsw_r32(cdev, PCE_GCTRL_1_MKFIDEN_OFFSET,
			PCE_GCTRL_1_MKFIDEN_SHIFT,
			PCE_GCTRL_1_MKFIDEN_SIZE,
			&parm->bMulticastFIDmode);
	}

	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_pce);
#endif
	return ret;

}

//Govind - Is this API compatible for 3.1?
GSW_return_t GSW_MulticastSnoopCfgSet(void *cdev,
				      GSW_multicastSnoopCfg_t *parm)
{
	u32 i, data_1 = 0, data_2 = 0, pmcindex = MPCE_RULES_INDEX;
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	static GSW_PCE_rule_t pcrule;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_pce);
#endif

	/* Choose IGMP Mode */
	switch (parm->eIGMP_Mode) {
	case GSW_MULTICAST_SNOOP_MODE_DISABLED:
		/* Snooping of Router Port Disable */
		gsw_w32(cdev, PCE_IGMP_CTRL_SRPEN_OFFSET,
			PCE_IGMP_CTRL_SRPEN_SHIFT,
			PCE_IGMP_CTRL_SRPEN_SIZE, 0);
		gsw_w32(cdev, PCE_GCTRL_0_IGMP_OFFSET,
			PCE_GCTRL_0_IGMP_SHIFT,
			PCE_GCTRL_0_IGMP_SIZE, 0);

		for (i = 0; i <= gswdev->tpnum; i++) {
			gsw_w32(cdev, PCE_PCTRL_0_MCST_OFFSET + (0xA * i),
				PCE_PCTRL_0_MCST_SHIFT,
				PCE_PCTRL_0_MCST_SIZE, 0);
		}

		break;

	case GSW_MULTICAST_SNOOP_MODE_AUTOLEARNING:
		/* Snooping of Router Port Enable */
		gsw_w32(cdev, PCE_GCTRL_0_IGMP_OFFSET,
			PCE_GCTRL_0_IGMP_SHIFT,
			PCE_GCTRL_0_IGMP_SIZE, 0);
		gsw_w32(cdev, PCE_IGMP_CTRL_SRPEN_OFFSET,
			PCE_IGMP_CTRL_SRPEN_SHIFT,
			PCE_IGMP_CTRL_SRPEN_SIZE, 1);

		for (i = 0; i <= gswdev->tpnum; i++) {
			gsw_w32(cdev, PCE_PCTRL_0_MCST_OFFSET + (0xA * i),
				PCE_PCTRL_0_MCST_SHIFT,
				PCE_PCTRL_0_MCST_SIZE, 1);
		}

		break;

	case GSW_MULTICAST_SNOOP_MODE_FORWARD:

		/* Snooping of Router Port Forward */
		if ((IS_VRSN_NOT_31(gswdev->gipver)) ||
		    ((IS_VRSN_31(gswdev->gipver)) && (gswdev->mcsthw_snoop == MCAST_HWSNOOP_EN))) {
			gsw_w32(cdev, PCE_IGMP_CTRL_SRPEN_OFFSET,
				PCE_IGMP_CTRL_SRPEN_SHIFT,
				PCE_IGMP_CTRL_SRPEN_SIZE, 0);
		}

		gsw_w32(cdev, PCE_GCTRL_0_IGMP_OFFSET,
			PCE_GCTRL_0_IGMP_SHIFT,
			PCE_GCTRL_0_IGMP_SIZE, 1);

		if ((IS_VRSN_NOT_31(gswdev->gipver)) ||
		    ((IS_VRSN_31(gswdev->gipver)) && (gswdev->mcsthw_snoop == MCAST_HWSNOOP_EN))) {
			for (i = 0; i < gswdev->tpnum; i++) {
				gsw_w32(cdev, PCE_PCTRL_0_MCST_OFFSET + (0xA * i),
					PCE_PCTRL_0_MCST_SHIFT,
					PCE_PCTRL_0_MCST_SIZE, 1);
			}
		}

		break;

	default:
		pr_err("This Mode doesn't exists\n");
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;

	}

	/* Set the Flag for eIGMP_Mode flag*/
	gswdev->iflag.igmode = parm->eIGMP_Mode;
	/* Set bIGMPv3 flag*/
	gswdev->iflag.igv3 =  parm->bIGMPv3;
	/* Set bCrossVLAN flag*/
	gswdev->iflag.igxvlan = parm->bCrossVLAN;
	/* Set eForwardPort flag */
	gswdev->iflag.igfport = parm->eForwardPort;

	/* Set nForwardPortId */
	if (parm->eForwardPort == GSW_PORT_FORWARD_CPU)
		gswdev->iflag.igfpid = (1 << gswdev->cport);
	else
		gswdev->iflag.igfpid = parm->nForwardPortId;

	gswdev->iflag.igcos = parm->nClassOfService;

	/* If IGMP mode set to AutoLearning then the following Rule have to add it */
	if (parm->eIGMP_Mode ==
	    GSW_MULTICAST_SNOOP_MODE_AUTOLEARNING) {

		if ((IS_VRSN_31(gswdev->gipver)) && (gswdev->mcsthw_snoop == MCAST_HWSNOOP_DIS)) {
			//pr_err("%s:%s not supported in GSWIP v3.1 :%d\n",__FILE__, __func__, __LINE__);
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}

		for (i = pmcindex; i <= (pmcindex + 7); i++) {
			memset(&pcrule, 0, sizeof(GSW_PCE_rule_t));
			pcrule.pattern.nIndex = i;
			pcrule.pattern.bEnable = 1;
			pcrule.pattern.bAppDataMSB_Enable = 1;

			if ((i == pmcindex + 0) ||
			    (i == pmcindex + 1) ||
			    (i == pmcindex + 2))
				pcrule.pattern.nAppDataMSB = 0x1100;
			else if (i == pmcindex + 3)
				pcrule.pattern.nAppDataMSB = 0x1200;
			else if (i == pmcindex + 4)
				pcrule.pattern.nAppDataMSB = 0x1600;
			else if (i == pmcindex + 5)
				pcrule.pattern.nAppDataMSB = 0x1700;
			else if (i == pmcindex + 6)
				pcrule.pattern.nAppDataMSB = 0x3100;
			else if (i == pmcindex + 7)
				pcrule.pattern.nAppDataMSB = 0x3000;

			pcrule.pattern.bAppMaskRangeMSB_Select = 0;
			/*			if (gswdev->gipver == LTQ_GSWIP_3_0) { */
			/*				pcrule.pattern.nAppMaskRangeMSB = 0x0; */
			/*			} else { */
			pcrule.pattern.nAppMaskRangeMSB = 0x3;

			/*		} */
			if ((i == pmcindex + 0) ||
			    (i == pmcindex + 1) ||
			    (i == pmcindex + 6) ||
			    (i == pmcindex + 7))
				pcrule.pattern.eDstIP_Select = 1;

			if ((i == pmcindex + 0) ||
			    (i == pmcindex + 1))
				pcrule.pattern.nDstIP.nIPv4 = 0xE0000001;
			else if (i == pmcindex + 6)
				pcrule.pattern.nDstIP.nIPv4 = 0xE0000002;
			else if (i == pmcindex + 7)
				pcrule.pattern.nDstIP.nIPv4 = 0xE00000A6;

			pcrule.pattern.nDstIP_Mask = 0xFF00;

			if (i == pmcindex + 1)
				pcrule.pattern.eSrcIP_Select = 1;
			else
				pcrule.pattern.eSrcIP_Select = 0;

			if (i == pmcindex + 1)
				pcrule.pattern.nSrcIP_Mask = 0xFF00;
			else
				pcrule.pattern.nSrcIP_Mask = 0xFFFF;

			pcrule.pattern.bProtocolEnable = 1;
			pcrule.pattern.nProtocol = 0x2;

			if (gswdev->iflag.igcos == 0) {
				pcrule.action.eTrafficClassAction = 0;
				pcrule.action.nTrafficClassAlternate = 0;
			} else {
				pcrule.action.eTrafficClassAction = 1;
				pcrule.action.nTrafficClassAlternate =
					gswdev->iflag.igcos;
			}

			if (i == pmcindex + 0)
				pcrule.action.eSnoopingTypeAction =
					GSW_PCE_ACTION_IGMP_SNOOP_QUERY;
			else if (i == pmcindex + 1)
				pcrule.action.eSnoopingTypeAction =
					GSW_PCE_ACTION_IGMP_SNOOP_QUERY_NO_ROUTER;
			else if (i == pmcindex + 2)
				pcrule.action.eSnoopingTypeAction =
					GSW_PCE_ACTION_IGMP_SNOOP_QUERY_GROUP;
			else if (i == pmcindex + 3)
				pcrule.action.eSnoopingTypeAction =
					GSW_PCE_ACTION_IGMP_SNOOP_REPORT;
			else if (i == pmcindex + 4)
				pcrule.action.eSnoopingTypeAction =
					GSW_PCE_ACTION_IGMP_SNOOP_REPORT;
			else if (i == pmcindex + 5)
				pcrule.action.eSnoopingTypeAction =
					GSW_PCE_ACTION_IGMP_SNOOP_LEAVE;
			else if (i == pmcindex + 6)
				pcrule.action.eSnoopingTypeAction =
					GSW_PCE_ACTION_IGMP_SNOOP_AD;
			else if (i == pmcindex + 7)
				pcrule.action.eSnoopingTypeAction =
					GSW_PCE_ACTION_IGMP_SNOOP_AD;

			pcrule.action.ePortMapAction =
				GSW_PCE_ACTION_PORTMAP_MULTICAST_ROUTER;

			if (parm->bCrossVLAN)
				pcrule.action.eVLAN_CrossAction =
					GSW_PCE_ACTION_CROSS_VLAN_CROSS;
			else
				pcrule.action.eVLAN_CrossAction =
					GSW_PCE_ACTION_CROSS_VLAN_DISABLE;

			/* We prepare everything and write into PCE Table */
			if (0 != pce_rule_write(cdev,
						&gswdev->phandler, &pcrule)) {
				ret = GSW_statusErr;
				goto UNLOCK_AND_RETURN;
			}
		}
	}

	/* If IGMP mode set to forwarding then the */
	/* following Rule have to add it */
	if (parm->eIGMP_Mode == GSW_MULTICAST_SNOOP_MODE_FORWARD) {
		for (i = pmcindex; i <= (pmcindex + 7); i++) {
			memset(&pcrule, 0, sizeof(GSW_PCE_rule_t));
			pcrule.pattern.nIndex = i;
			pcrule.pattern.bEnable = 1;
			pcrule.pattern.bProtocolEnable = 1;

			switch (i - pmcindex) {
			/*		case 0: */
			/*Rule added by Router port ADD function based on router port for IPv4*/
			/*					break; */
			case 1:
				/*	Avoid IGMP Packets Redirection when seen on Management Port */
				pcrule.pattern.nProtocol = 0x2; /* for IPv4 */
				pcrule.pattern.bPortIdEnable = 1;
				/* Action Enabled, no redirection (default portmap) */
				pcrule.action.ePortMapAction =
					GSW_PCE_ACTION_PORTMAP_REGULAR;
				break;

			case 2:
				/* IGMPv1/2/3 IPv4 */
				pcrule.pattern.nProtocol = 0x2; /* for IPv4 */
				pcrule.action.ePortMapAction =
					GSW_PCE_ACTION_PORTMAP_ALTERNATIVE;
				break;

			/*		case 3: */
			/*Rules added by Router port ADD function */
			/* based on router port for IPv6 */
			/*			break; */
			case 4:
				/*	Managemnt Port Remaining ICMPv6/MLD packets */
				/* (Avoiding Loopback and Disacard) */
				pcrule.pattern.bPortIdEnable = 1;
				pcrule.pattern.nPortId = parm->nForwardPortId;
				pcrule.pattern.nProtocol = 0x3A;  /*for IPv6*/
				pcrule.pattern.bPortIdEnable = 1;
				pcrule.action.ePortMapAction =
					GSW_PCE_ACTION_PORTMAP_REGULAR;
				break;

			case 5:
				/* ICMPv6 Multicast Listener Query/Report/Done(Leave) */
				pcrule.pattern.bAppDataMSB_Enable	= 1;
				pcrule.pattern.bAppMaskRangeMSB_Select = 1;
				pcrule.pattern.nAppDataMSB = 0x8200;
				pcrule.pattern.nAppMaskRangeMSB	= 0x2FF;
				pcrule.pattern.nProtocol = 0x3A;  /*for IPv6*/
				pcrule.action.ePortMapAction =
					GSW_PCE_ACTION_PORTMAP_ALTERNATIVE;
				break;

			case 6:
				/* ICMPv6 Multicast Listener Report */
				pcrule.pattern.bAppDataMSB_Enable	= 1;
				pcrule.pattern.nAppDataMSB = 0x8F00;
				pcrule.pattern.nAppMaskRangeMSB = 0x3;
				pcrule.pattern.nProtocol = 0x3A;  /*for IPv6*/
				pcrule.action.ePortMapAction =
					GSW_PCE_ACTION_PORTMAP_ALTERNATIVE;
				break;

			case 7:
				/* ICMPv6 Multicast Router Advertisement/Solicitation/Termination */
				pcrule.pattern.bAppDataMSB_Enable	= 1;
				pcrule.pattern.bAppMaskRangeMSB_Select = 1;
				pcrule.pattern.nAppDataMSB = 0x9700;
				pcrule.pattern.nAppMaskRangeMSB	= 0x2FF;
				pcrule.pattern.nProtocol = 0x3A;  /*for IPv6*/
				pcrule.action.ePortMapAction =
					GSW_PCE_ACTION_PORTMAP_ALTERNATIVE;
				break;

			default:
				continue;
			}

			if (gswdev->iflag.igcos != 0) {
				pcrule.action.eTrafficClassAction = 1;
				pcrule.action.nTrafficClassAlternate =
					gswdev->iflag.igcos;
			}

			/*  Set eForwardPort */
			if (parm->eForwardPort == GSW_PORT_FORWARD_PORT) {
				pcrule.action.nForwardPortMap[0] =
					(1 << parm->nForwardPortId);
				pcrule.pattern.nPortId = parm->nForwardPortId;
			} else if (parm->eForwardPort == GSW_PORT_FORWARD_CPU) {

				if (IS_VRSN_31_OR_32(gswdev->gipver)) {
					pcrule.pattern.bInsertionFlag_Enable = 1;
					pcrule.pattern.nInsertionFlag = 0;
				}

				pcrule.action.nForwardPortMap[0] =
					(1 << gswdev->cport);
				pcrule.pattern.nPortId = gswdev->cport;
			}

			if (parm->bCrossVLAN)
				pcrule.action.eVLAN_CrossAction =
					GSW_PCE_ACTION_CROSS_VLAN_CROSS;
			else
				pcrule.action.eVLAN_CrossAction =
					GSW_PCE_ACTION_CROSS_VLAN_DISABLE;

			/* We prepare everything and write into PCE Table */
			if (pce_rule_write(cdev,
					   &gswdev->phandler, &pcrule) != 0) {
				ret = GSW_statusErr;
				goto UNLOCK_AND_RETURN;
			}
		}

	}


	if ((IS_VRSN_31(gswdev->gipver)) && (gswdev->mcsthw_snoop == MCAST_HWSNOOP_DIS)) {
		//pr_err("%s:%s not supported in GSWIP v3.1 :%d\n",__FILE__, __func__, __LINE__);
		ret = GSW_statusOk;
		goto UNLOCK_AND_RETURN;
	}

	if (parm->eIGMP_Mode ==
	    GSW_MULTICAST_SNOOP_MODE_DISABLED) {
		pmcindex = MPCE_RULES_INDEX;

		for (i = pmcindex; i <= (pmcindex + 7); i++) {
			pcrule.pattern.nIndex = i;
			pcrule.pattern.bEnable = 0;

			/* We prepare everything and write into PCE Table */
			if (0 != pce_pattern_delete(cdev, &gswdev->phandler, i)) {
				ret = GSW_statusErr;
				goto UNLOCK_AND_RETURN;
			}
		}
	}

	if (parm->nRobust < 4) {
		gsw_w32(cdev, PCE_IGMP_CTRL_ROB_OFFSET,
			PCE_IGMP_CTRL_ROB_SHIFT,
			PCE_IGMP_CTRL_ROB_SIZE, parm->nRobust);
	} else {
		pr_err("The Robust time would only support 0..3\n");
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	gsw_w32(cdev, PCE_IGMP_CTRL_DMRTEN_OFFSET,
		PCE_IGMP_CTRL_DMRTEN_SHIFT,
		PCE_IGMP_CTRL_DMRTEN_SIZE, 1);
	gsw_w32(cdev, PCE_IGMP_CTRL_DMRT_OFFSET,
		PCE_IGMP_CTRL_DMRT_SHIFT,
		PCE_IGMP_CTRL_DMRT_SIZE,
		parm->nQueryInterval);


	if (parm->eIGMP_Mode ==
	    GSW_MULTICAST_SNOOP_MODE_AUTOLEARNING) {
		switch (parm->eSuppressionAggregation) {
		case GSW_MULTICAST_REPORT_JOIN:
			data_2 = 1;
			data_1 = 1;
			break;

		case GSW_MULTICAST_REPORT:
			data_2 = 0;
			data_1 = 1;
			break;

		case GSW_MULTICAST_TRANSPARENT:
			data_2 = 0;
			data_1 = 0;
			break;
		}

		gsw_w32(cdev, PCE_IGMP_CTRL_REPSUP_OFFSET,
			PCE_IGMP_CTRL_REPSUP_SHIFT,
			PCE_IGMP_CTRL_REPSUP_SIZE, data_1);
		gsw_w32(cdev, PCE_IGMP_CTRL_JASUP_OFFSET,
			PCE_IGMP_CTRL_JASUP_SHIFT,
			PCE_IGMP_CTRL_JASUP_SIZE, data_2);
	}

	if (parm->eIGMP_Mode ==
	    GSW_MULTICAST_SNOOP_MODE_AUTOLEARNING) {
		gsw_w32(cdev, PCE_IGMP_CTRL_SRPEN_OFFSET,
			PCE_IGMP_CTRL_SRPEN_SHIFT,
			PCE_IGMP_CTRL_SRPEN_SIZE,
			parm->bLearningRouter);

		if (parm->bFastLeave == 1)
			gsw_w32(cdev, PCE_IGMP_CTRL_FLEAVE_OFFSET,
				PCE_IGMP_CTRL_FLEAVE_SHIFT,
				PCE_IGMP_CTRL_FLEAVE_SIZE, 1);
		else
			gsw_w32(cdev, PCE_IGMP_CTRL_FLEAVE_OFFSET,
				PCE_IGMP_CTRL_FLEAVE_SHIFT,
				PCE_IGMP_CTRL_FLEAVE_SIZE, 0);
	} else {
		gsw_w32(cdev, PCE_IGMP_CTRL_FLEAVE_OFFSET,
			PCE_IGMP_CTRL_FLEAVE_SHIFT,
			PCE_IGMP_CTRL_FLEAVE_SIZE, 0);
	}

	if ((gswdev->gipver == LTQ_GSWIP_2_2) ||
	    (gswdev->gipver == LTQ_GSWIP_2_2_ETC) ||
	    (gswdev->gipver == LTQ_GSWIP_3_0)) {
		gsw_w32(cdev, PCE_GCTRL_1_UKIPMC_OFFSET,
			PCE_GCTRL_1_UKIPMC_SHIFT,
			PCE_GCTRL_1_UKIPMC_SIZE,
			parm->bMulticastUnknownDrop);
	}

	if (gswdev->gipver == LTQ_GSWIP_3_0) {
		gsw_w32(cdev, PCE_GCTRL_1_MKFIDEN_OFFSET,
			PCE_GCTRL_1_MKFIDEN_SHIFT,
			PCE_GCTRL_1_MKFIDEN_SIZE,
			parm->bMulticastFIDmode);
	}

	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_pce);
#endif
	return ret;

}

GSW_return_t GSW_MulticastTableEntryAdd(void *cdev,
					GSW_multicastTable_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	gsw_igmp_t	*hitbl = &gswdev->iflag;
	u8 pidx = parm->nPortId;
	static pctbl_prog_t ptdata;
	int ret = 0;
	u32 index = 0, i, available = 0;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_pce);
#endif

	if (IS_VRSN_NOT_31(gswdev->gipver)) {
		if (parm->nPortId >= gswdev->tpnum) {
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}
	}

	memset(&ptdata, 0, sizeof(pctbl_prog_t));

	if (hitbl->igmode == GSW_MULTICAST_SNOOP_MODE_AUTOLEARNING) {

		if ((IS_VRSN_31(gswdev->gipver)) && (gswdev->mcsthw_snoop == MCAST_HWSNOOP_DIS)) {
			pr_err("%s:%s not supported in GSWIP v3.1 :%d\n", __FILE__, __func__, __LINE__);
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}

		if ((gswdev->iflag.igv3 == 1) ||
		    (parm->eIPVersion == GSW_IP_SELECT_IPV6)) {
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}

		/* Read Out all of the HW Table */
		for (i = 0; i < gswdev->mctblsize; i++) {
			ptdata.table = PCE_MULTICAST_HW_INDEX;
			ptdata.pcindex = i;
			gsw_pce_table_read(cdev, &ptdata);

			if (ptdata.valid) {
				if ((ptdata.key[0] ==
				     (parm->uIP_Gda.nIPv4 & 0xFFFF)) &&
				    (ptdata.key[1] ==
				     ((parm->uIP_Gda.nIPv4 >> 16) & 0xFFFF))) {
					index = i;
					available = 1;
					break;
				}
			}
		}

		ptdata.table = PCE_MULTICAST_HW_INDEX;

		if (available == 0) {
			index = gswdev->mctblsize;

			for (i = 0; i < gswdev->mctblsize; i++) {
				ptdata.pcindex = i;
				gsw_pce_table_read(cdev, &ptdata);

				if (ptdata.valid == 0) {
					index = i;  /* Free index */
					break;
				}
			}
		}

		if (index < gswdev->mctblsize) {
			ptdata.table = PCE_MULTICAST_HW_INDEX;
			ptdata.pcindex = index;
			ptdata.key[1] = ((parm->uIP_Gda.nIPv4 >> 16) & 0xFFFF);
			ptdata.key[0] = (parm->uIP_Gda.nIPv4 & 0xFFFF);
			ptdata.val[0] |= (1 << pidx);
			ptdata.val[4] |= (1 << 14);
			ptdata.valid = 1;
			gsw_pce_table_write(cdev, &ptdata);
		} else {
			pr_err("Error: (IGMP HW Table is full) %s:%s:%d\n",
			       __FILE__, __func__, __LINE__);
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}

	} else if (hitbl->igmode == GSW_MULTICAST_SNOOP_MODE_FORWARD) {
		/* Program the Multicast SW Table */
		if (gswdev->gipver == LTQ_GSWIP_3_0)
			gsw3x_msw_table_wr(cdev, parm);
		else if (IS_VRSN_31(gswdev->gipver)) {
			ret = gsw_insert_hashtable_entry(cdev, parm);

			if (ret < 0) {
				pr_err("%s:%s Entry Cannot insert :%d %d\n", __FILE__, __func__, __LINE__, ret);
				ret = GSW_statusErr;
				goto UNLOCK_AND_RETURN;
			}
		} else
			gsw2x_msw_table_wr(cdev, parm);
	} else {
		/* Disable All Multicast SW Table */
		pr_err("Please Select the IGMP Mode through Multicast Snooping Configuration API\n");
	}

	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_pce);
#endif
	return ret;

}

GSW_return_t GSW_MulticastTableEntryRead(void *cdev,
		GSW_multicastTableRead_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	gsw_igmp_t	*hitbl = &gswdev->iflag;
	static pctbl_prog_t ptdata;
	u32 ret = 0;
	u32 dlsbid, slsbid, dmsbid, smsbid;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return  GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_pce);
#endif

	if (hitbl->igmode == GSW_MULTICAST_SNOOP_MODE_DISABLED) {
		pr_err("Error: (IGMP snoop is not enabled) %s:%s:%d\n",
		       __FILE__, __func__, __LINE__);
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}


	if (hitbl->igmode == GSW_MULTICAST_SNOOP_MODE_AUTOLEARNING) {

		if ((IS_VRSN_31(gswdev->gipver)) && (gswdev->mcsthw_snoop == MCAST_HWSNOOP_DIS)) {
			pr_err("%s:%s not supported in GSWIP v3.1 :%d\n", __FILE__, __func__, __LINE__);
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}

		if (parm->bInitial == 1) {
			gswdev->mhw_rinx = 0; /*Start from the index 0 */
			parm->bInitial = 0;
		}

		if (gswdev->mhw_rinx >= gswdev->mctblsize) {
			memset(parm, 0, sizeof(GSW_multicastTableRead_t));
			parm->bLast = 1;
			gswdev->mhw_rinx = 0;
			ret = GSW_statusOk;
			goto UNLOCK_AND_RETURN;
		}

		do {
			memset(&ptdata, 0, sizeof(pctbl_prog_t));
			ptdata.table = PCE_MULTICAST_HW_INDEX;
			ptdata.pcindex = gswdev->mhw_rinx;
			gsw_pce_table_read(cdev, &ptdata);
			gswdev->mhw_rinx++;

			if (ptdata.valid != 0)
				break;
		} while (gswdev->mhw_rinx < gswdev->mctblsize);

		if (ptdata.valid != 0) {
			parm->nPortId = ptdata.val[0] | 0x80000000;
			parm->uIP_Gda.nIPv4 =
				((ptdata.key[1] << 16) |
				 ptdata.key[0]);
			parm->uIP_Gsa.nIPv4 = 0;
			parm->eModeMember = GSW_IGMP_MEMBER_DONT_CARE;
			parm->eIPVersion = GSW_IP_SELECT_IPV4;
			parm->bInitial = 0;
			parm->bLast = 0;
		} else {
			memset(parm, 0, sizeof(GSW_multicastTableRead_t));
			parm->bLast = 1;
			gswdev->mhw_rinx = 0;
		}
	}

	/*Snooping in Forward mode */
	if (hitbl->igmode == GSW_MULTICAST_SNOOP_MODE_FORWARD) {

		if (parm->bInitial == 1) {
			gswdev->msw_rinx = 0; /*Start from the index 0 */
			parm->bInitial = 0;
		}

		if (gswdev->msw_rinx >= gswdev->mctblsize) {
			memset(parm, 0, sizeof(GSW_multicastTableRead_t));
			parm->bLast = 1;
			gswdev->msw_rinx = 0;
			ret = GSW_statusOk;
			goto UNLOCK_AND_RETURN;
		}

		if (IS_VRSN_31(gswdev->gipver)) {
			ret = 0;
			while (gswdev->msw_rinx < gswdev->mctblsize && !ret) {
				ret = gsw_get_swmcast_entry(cdev, parm, gswdev->msw_rinx);
				gswdev->msw_rinx++;
			}

			if (gswdev->msw_rinx >= gswdev->mctblsize) {
				memset(parm, 0, sizeof(GSW_multicastTableRead_t));
				parm->bLast = 1;
				gswdev->msw_rinx = 0;
			}

			ret = GSW_statusOk;
			goto UNLOCK_AND_RETURN;
		}

		do {
			memset(&ptdata, 0, sizeof(pctbl_prog_t));
			ptdata.table = PCE_MULTICAST_SW_INDEX;
			ptdata.pcindex = gswdev->msw_rinx;
			gsw_pce_table_read(cdev, &ptdata);
			gswdev->msw_rinx++;

			if (ptdata.valid != 0)
				break;
		} while (gswdev->msw_rinx < gswdev->mctblsize);

		if (ptdata.valid == 1) {
			pctbl_prog_t iptbl;
			parm->nPortId = ptdata.val[0] | 0x80000000;

			if (gswdev->gipver == LTQ_GSWIP_3_0) {
				parm->nSubIfId = (ptdata.val[1] >> 3) & 0xFFFF;
				parm->nFID = (ptdata.key[2] & 0x3F);
				parm->bExclSrcIP =
					((ptdata.key[2] >> 15) & 0x1);
			}

			dlsbid = ptdata.key[0] & 0xFF;
			dmsbid = (ptdata.key[0] >> 8) & 0xFF;
			slsbid = ptdata.key[1] & 0xFF;
			smsbid = (ptdata.key[1] >> 8) & 0xFF;

			if (dlsbid <= 0x3F) {
				memset(&iptbl, 0, sizeof(pctbl_prog_t));
				iptbl.table = PCE_IP_DASA_LSB_INDEX;
				/* Search the DIP */
				iptbl.pcindex = dlsbid;
				gsw_pce_table_read(cdev, &iptbl);

				if (iptbl.valid == 1) {
					if (gswdev->gipver == LTQ_GSWIP_3_0) {
						if ((iptbl.mask[0] == 0x0) &&
						    ((iptbl.mask[1] == 0x0)) &&
						    ((iptbl.mask[2] == 0xFFFF)) &&
						    ((iptbl.mask[3] == 0xFFFF))) {
							parm->uIP_Gda.nIPv4 =
								((iptbl.key[1] << 16)
								 | (iptbl.key[0]));
							parm->eIPVersion =
								GSW_IP_SELECT_IPV4;
						} else if ((iptbl.mask[0] == 0x0) &&
							   ((iptbl.mask[1] == 0x0)) &&
							   ((iptbl.mask[2] == 0x0)) &&
							   ((iptbl.mask[3] == 0x0))) {
							parm->uIP_Gda.nIPv6[4] =
								(iptbl.key[3]);
							parm->uIP_Gda.nIPv6[5] =
								(iptbl.key[2]);
							parm->uIP_Gda.nIPv6[6] =
								(iptbl.key[1]);
							parm->uIP_Gda.nIPv6[7] =
								(iptbl.key[0]);
							parm->eIPVersion =
								GSW_IP_SELECT_IPV6;
						}
					} else {
						if (iptbl.mask[0] == 0xFF00) {
							parm->uIP_Gda.nIPv4 =
								((iptbl.key[1] << 16)
								 | (iptbl.key[0]));
							parm->eIPVersion =
								GSW_IP_SELECT_IPV4;
						} else if (iptbl.mask[0] == 0x0) {
							parm->uIP_Gda.nIPv6[4] =
								(iptbl.key[3]);
							parm->uIP_Gda.nIPv6[5] =
								(iptbl.key[2]);
							parm->uIP_Gda.nIPv6[6] =
								(iptbl.key[1]);
							parm->uIP_Gda.nIPv6[7] =
								(iptbl.key[0]);
							parm->eIPVersion =
								GSW_IP_SELECT_IPV6;
						}
					}
				}
			}

			if (slsbid <= 0x3F) {
				memset(&iptbl, 0, sizeof(pctbl_prog_t));
				iptbl.table = PCE_IP_DASA_LSB_INDEX;
				/* Search the SIP */
				iptbl.pcindex = slsbid;
				gsw_pce_table_read(cdev, &iptbl);

				if (iptbl.valid == 1) {
					if (gswdev->gipver == LTQ_GSWIP_3_0) {
						if ((iptbl.mask[0] == 0x0) &&
						    ((iptbl.mask[1] == 0x0)) &&
						    ((iptbl.mask[2] == 0xFFFF)) &&
						    ((iptbl.mask[3] == 0xFFFF))) {
							parm->uIP_Gsa.nIPv4 =
								((iptbl.key[1] << 16)
								 | (iptbl.key[0]));
							parm->eIPVersion =
								GSW_IP_SELECT_IPV4;
						} else if ((iptbl.mask[0] == 0x0) &&
							   ((iptbl.mask[1] == 0x0)) &&
							   ((iptbl.mask[2] == 0x0)) &&
							   ((iptbl.mask[3] == 0x0))) {
							parm->uIP_Gsa.nIPv6[4] =
								(iptbl.key[3]);
							parm->uIP_Gsa.nIPv6[5] =
								(iptbl.key[2]);
							parm->uIP_Gsa.nIPv6[6] =
								(iptbl.key[1]);
							parm->uIP_Gsa.nIPv6[7] =
								(iptbl.key[0]);
						}
					} else {
						if (iptbl.mask[0] == 0xFF00) {
							parm->uIP_Gsa.nIPv4 =
								((iptbl.key[1] << 16)
								 | (iptbl.key[0]));
							parm->eIPVersion =
								GSW_IP_SELECT_IPV4;
						} else if (iptbl.mask == 0x0) {
							parm->uIP_Gsa.nIPv6[4] =
								(iptbl.key[3]);
							parm->uIP_Gsa.nIPv6[5] =
								(iptbl.key[2]);
							parm->uIP_Gsa.nIPv6[6] =
								(iptbl.key[1]);
							parm->uIP_Gsa.nIPv6[7] =
								(iptbl.key[0]);
						}
					}
				}
			}

			if (dmsbid <= 0xF) {
				memset(&iptbl, 0, sizeof(pctbl_prog_t));
				iptbl.table = PCE_IP_DASA_MSB_INDEX;
				/* Search the DIP */
				iptbl.pcindex = dmsbid;
				gsw_pce_table_read(cdev, &iptbl);

				if (iptbl.valid == 1) {
					if (gswdev->gipver == LTQ_GSWIP_3_0) {
						if ((iptbl.mask[0] == 0) &&
						    ((iptbl.mask[1] == 0)) &&
						    ((iptbl.mask[2] == 0)) &&
						    ((iptbl.mask[3] == 0))) {
							parm->uIP_Gda.nIPv6[0] =
								(iptbl.key[3]);
							parm->uIP_Gda.nIPv6[1] =
								(iptbl.key[2]);
							parm->uIP_Gda.nIPv6[2] =
								(iptbl.key[1]);
							parm->uIP_Gda.nIPv6[3] =
								(iptbl.key[0]);
						}
					} else {
						if (iptbl.mask  == 0) {
							parm->uIP_Gda.nIPv6[0] =
								(iptbl.key[3]);
							parm->uIP_Gda.nIPv6[1] =
								(iptbl.key[2]);
							parm->uIP_Gda.nIPv6[2] =
								(iptbl.key[1]);
							parm->uIP_Gda.nIPv6[3] =
								(iptbl.key[0]);
						}
					}
				}
			}

			if (smsbid <= 0xF) {
				memset(&iptbl, 0, sizeof(pctbl_prog_t));
				iptbl.table = PCE_IP_DASA_MSB_INDEX;
				/* Search the DIP */
				iptbl.pcindex = smsbid;
				gsw_pce_table_read(cdev, &iptbl);

				if (iptbl.valid == 1) {
					if (gswdev->gipver == LTQ_GSWIP_3_0) {
						if ((iptbl.mask[0] == 0) &&
						    ((iptbl.mask[1] == 0)) &&
						    ((iptbl.mask[2] == 0)) &&
						    ((iptbl.mask[3] == 0))) {
							parm->uIP_Gsa.nIPv6[0] =
								(iptbl.key[3]);
							parm->uIP_Gsa.nIPv6[1] =
								(iptbl.key[2]);
							parm->uIP_Gsa.nIPv6[2] =
								(iptbl.key[1]);
							parm->uIP_Gsa.nIPv6[3] =
								(iptbl.key[0]);
						}
					} else {
						if (iptbl.mask == 0) {
							parm->uIP_Gsa.nIPv6[0] =
								(iptbl.key[3]);
							parm->uIP_Gsa.nIPv6[1] =
								(iptbl.key[2]);
							parm->uIP_Gsa.nIPv6[2] =
								(iptbl.key[1]);
							parm->uIP_Gsa.nIPv6[3] =
								(iptbl.key[0]);
						}
					}
				}
			}

			parm->eModeMember =
				hitbl->mctable[gswdev->msw_rinx - 1].mcmode;
			parm->bInitial = 0;
			parm->bLast = 0;
		} else {
			memset(parm, 0, sizeof(GSW_multicastTableRead_t));
			parm->bLast = 1;
			gswdev->msw_rinx = 0;
		}
	}

	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_pce);
#endif
	return ret;
}

GSW_return_t GSW_MulticastTableEntryRemove(void *cdev,
		GSW_multicastTable_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	gsw_igmp_t	*hitbl = &gswdev->iflag;
	u8 pidx = parm->nPortId;
	static pctbl_prog_t ptdata;
	ltq_bool_t dflag = 0;
	u32 port = 0, i;
	int ret = 0;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_pce);
#endif

	if (IS_VRSN_NOT_31(gswdev->gipver)) {
		if (parm->nPortId >= gswdev->tpnum) {
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}
	}

	memset(&ptdata, 0, sizeof(pctbl_prog_t));

	if (hitbl->igmode ==
	    GSW_MULTICAST_SNOOP_MODE_AUTOLEARNING) {

		if ((IS_VRSN_31(gswdev->gipver)) && (gswdev->mcsthw_snoop == MCAST_HWSNOOP_DIS)) {
			pr_err("%s:%s not supported in GSWIP v3.1 :%d\n", __FILE__, __func__, __LINE__);
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}

		if (gswdev->iflag.igv3 == 1) {
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}

		/* Read Out all of the HW Table */
		for (i = 0; i < gswdev->mctblsize; i++) {
			memset(&ptdata, 0, sizeof(pctbl_prog_t));
			ptdata.table = PCE_MULTICAST_HW_INDEX;
			ptdata.pcindex = i;
			gsw_pce_table_read(cdev, &ptdata);

			/* Fill into Structure */
			if (((ptdata.val[0] >> pidx) & 0x1) == 1) {
				if (parm->uIP_Gda.nIPv4 ==
				    ((ptdata.key[1] << 16)
				     | (ptdata.key[0]))) {
					port = (ptdata.val[0] & (~(1 << pidx)));

					if (port == 0) {
						ptdata.val[0] = 0;
						ptdata.key[1] = 0;
						ptdata.val[4] = 0;
					} else {
						ptdata.val[0] &= ~(1 << pidx);
					}

					dflag = 1;
					gsw_pce_table_write(cdev, &ptdata);
				}
			}
		}

		if (dflag == 0)
			pr_err("The input did not found\n");
	} else if (hitbl->igmode ==
		   GSW_MULTICAST_SNOOP_MODE_FORWARD) {
		if (gswdev->gipver == LTQ_GSWIP_3_0)
			gsw3x_msw_table_rm(cdev, parm);
		else if (IS_VRSN_31(gswdev->gipver)) {
			ret = gsw_remove_hashtable_entry(cdev, parm);

			if (ret < 0) {
				pr_err("%s:%s Entry Cannot remove :%d %d\n", __FILE__, __func__, __LINE__, ret);
				ret = GSW_statusErr;
				goto UNLOCK_AND_RETURN;
			}
		} else
			gsw2x_msw_table_rm(cdev, parm);
	}

	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_pce);
#endif
	return ret;

}

#endif /*CONFIG_LTQ_MULTICAST*/

GSW_return_t GSW_CapGet(void *cdev, GSW_cap_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 value, data1, data2;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return  GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_misc);
#endif

	if (parm->nCapType >= GSW_CAP_TYPE_LAST) {
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	} else {
		strcpy(parm->cDesc, capdes[parm->nCapType].desci);
		/* As request, attached the code in the next version*/
	}

	switch (parm->nCapType) {
	case GSW_CAP_TYPE_PORT:

		/*Temp work around for the below external switch model*/
		if (gswdev->gsw_dev == LTQ_FLOW_DEV_EXT_AX3000_F24S) {
			get_gsw_hw_cap(cdev);
		}

		gsw_r32(cdev, ETHSW_CAP_1_PPORTS_OFFSET,
			ETHSW_CAP_1_PPORTS_SHIFT,
			ETHSW_CAP_1_PPORTS_SIZE, &value);
		parm->nCap = value;
		break;

	case GSW_CAP_TYPE_VIRTUAL_PORT:
		gsw_r32(cdev, ETHSW_CAP_1_VPORTS_OFFSET,
			ETHSW_CAP_1_VPORTS_SHIFT,
			ETHSW_CAP_1_VPORTS_SIZE, &value);
		parm->nCap = value;
		break;

	case GSW_CAP_TYPE_BUFFER_SIZE:
		gsw_r32(cdev, ETHSW_CAP_11_BSIZEL_OFFSET,
			ETHSW_CAP_11_BSIZEL_SHIFT,
			ETHSW_CAP_11_BSIZEL_SIZE, &data1);
		gsw_r32(cdev, ETHSW_CAP_12_BSIZEH_OFFSET,
			ETHSW_CAP_12_BSIZEH_SHIFT,
			ETHSW_CAP_12_BSIZEH_SIZE, &data2);
		parm->nCap = (data2 << 16 | data1);
		break;

	case GSW_CAP_TYPE_SEGMENT_SIZE:
		/* This is Hard coded */
		parm->nCap = LTQ_SOC_CAP_SEGMENT;
		break;

	case GSW_CAP_TYPE_PRIORITY_QUEUE:
		parm->nCap = gswdev->num_of_queues;
		break;

	case GSW_CAP_TYPE_METER:
		parm->nCap = gswdev->num_of_meters;
		break;

	case GSW_CAP_TYPE_RATE_SHAPER:
		parm->nCap = gswdev->num_of_shapers;
		break;

	case GSW_CAP_TYPE_VLAN_GROUP:
		parm->nCap = gswdev->avlantsz;
		break;

	case GSW_CAP_TYPE_FID:
		/* This is Hard coded */
		parm->nCap = VRX_PLATFORM_CAP_FID;
		break;

	case GSW_CAP_TYPE_MAC_TABLE_SIZE:
		parm->nCap = gswdev->mactblsize;
		break;

	case GSW_CAP_TYPE_MULTICAST_TABLE_SIZE:
		parm->nCap = gswdev->mctblsize;
		break;

	case GSW_CAP_TYPE_PPPOE_SESSION:
		parm->nCap = gswdev->num_of_pppoe;
		break;

	case GSW_CAP_TYPE_SVLAN_GROUP:
		parm->nCap = gswdev->avlantsz;
		break;

	case GSW_CAP_TYPE_PMAC:
		parm->nCap = gswdev->num_of_pmac;
		break;

	case GSW_CAP_TYPE_PAYLOAD:
		parm->nCap = gswdev->pdtblsize;
		break;

	case GSW_CAP_TYPE_IF_RMON:
		parm->nCap = gswdev->num_of_ifrmon;
		break;

	case GSW_CAP_TYPE_EGRESS_VLAN:
		parm->nCap = gswdev->num_of_egvlan;
		break;

	case GSW_CAP_TYPE_RT_SMAC:
		parm->nCap = gswdev->num_of_rt_smac;
		break;

	case GSW_CAP_TYPE_RT_DMAC:
		parm->nCap = gswdev->num_of_rt_dmac;
		break;

	case GSW_CAP_TYPE_RT_PPPoE:
		parm->nCap = gswdev->num_of_rt_ppoe;
		break;

	case GSW_CAP_TYPE_RT_NAT:
		parm->nCap = gswdev->num_of_rt_nat;
		break;

	case GSW_CAP_TYPE_RT_MTU:
		parm->nCap = gswdev->num_of_rt_mtu;
		break;

	case GSW_CAP_TYPE_RT_TUNNEL:
		parm->nCap = gswdev->num_of_rt_tunnel;
		break;

	case GSW_CAP_TYPE_RT_RTP:
		parm->nCap = gswdev->num_of_rt_rtp;
		break;

	case GSW_CAP_TYPE_CTP:
		parm->nCap = gswdev->num_of_ctp;
		break;

	case GSW_CAP_TYPE_BRIDGE_PORT:
		parm->nCap = gswdev->num_of_bridge_port;
		break;

	case GSW_CAP_TYPE_COMMON_TFLOW_RULES:
		parm->nCap = gswdev->num_of_global_rules;
		break;

	case GSW_CAP_TYPE_LAST:
		parm->nCap = GSW_CAP_TYPE_LAST;
		break;

	default:
		parm->nCap = 0;
		break;
	}

	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_misc);
#endif
	return ret;

}

GSW_return_t GSW_CfgGet(void *cdev, GSW_cfg_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 ret;

#if defined(CONFIG_MAC) && CONFIG_MAC
	struct mac_ops *ops;
#endif
	u32 value, data2;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_misc);
#endif

	/* Aging Counter Mantissa Value */
	gsw_r32(cdev, PCE_AGE_1_MANT_OFFSET,
		PCE_AGE_1_MANT_SHIFT,
		PCE_AGE_1_MANT_SIZE, &data2);

	if (data2 == AGETIMER_1_DAY)
		parm->eMAC_TableAgeTimer = GSW_AGETIMER_1_DAY;
	else if (data2 == AGETIMER_1_HOUR)
		parm->eMAC_TableAgeTimer = GSW_AGETIMER_1_HOUR;
	else if (data2 == AGETIMER_300_SEC)
		parm->eMAC_TableAgeTimer = GSW_AGETIMER_300_SEC;
	else if (data2 == AGETIMER_10_SEC)
		parm->eMAC_TableAgeTimer = GSW_AGETIMER_10_SEC;
	else if (data2 == AGETIMER_1_SEC)
		parm->eMAC_TableAgeTimer = GSW_AGETIMER_1_SEC;
	else
		parm->eMAC_TableAgeTimer = 0;

	if (IS_VRSN_NOT_31(gswdev->gipver)) {
		gsw_r32(cdev, MAC_FLEN_LEN_OFFSET,
			MAC_FLEN_LEN_SHIFT, MAC_FLEN_LEN_SIZE, &value);
		parm->nMaxPacketLen = value;
		gsw_r32(cdev, MAC_PFAD_CFG_SAMOD_OFFSET,
			MAC_PFAD_CFG_SAMOD_SHIFT,
			MAC_PFAD_CFG_SAMOD_SIZE, &value);
		parm->bPauseMAC_ModeSrc = value;
		gsw_r32(cdev, PCE_GCTRL_0_VLAN_OFFSET,
			PCE_GCTRL_0_VLAN_SHIFT,
			PCE_GCTRL_0_VLAN_SIZE, &value);
		parm->bVLAN_Aware = value;
	} else if (IS_VRSN_31(gswdev->gipver)) {
#if defined(CONFIG_MAC) && CONFIG_MAC
		/* Get MTU for MAC2/3/4 */
		ops = get_mac_ops(gswdev, 2);

		if (!ops) {
			pr_err("MAC 2 is not initialized\n");
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}

		parm->nMaxPacketLen = ops->get_mtu(ops);
#endif
	}

	/* MAC Address Learning Limitation Mode */
	gsw_r32(cdev, PCE_GCTRL_0_PLIMMOD_OFFSET,
		PCE_GCTRL_0_PLIMMOD_SHIFT,
		PCE_GCTRL_0_PLIMMOD_SIZE, &value);
	parm->bLearningLimitAction = value;

	/*Accept or discard MAC spoofing and port MAC locking violation packets */
	if ((gswdev->gipver == LTQ_GSWIP_2_2) ||
	    (gswdev->gipver == LTQ_GSWIP_2_2_ETC) ||
	    (IS_VRSN_30_31(gswdev->gipver))) {
		gsw_r32(cdev, PCE_GCTRL_0_PLCKMOD_OFFSET,
			PCE_GCTRL_0_PLCKMOD_SHIFT,
			PCE_GCTRL_0_PLCKMOD_SIZE,
			&parm->bMAC_SpoofingAction);
	}

	if (IS_VRSN_NOT_31(gswdev->gipver)) {
		if (parm->bPauseMAC_ModeSrc) {
			gsw_r32(cdev, MAC_PFSA_0_PFAD_OFFSET,
				MAC_PFSA_0_PFAD_SHIFT, MAC_PFSA_0_PFAD_SIZE, &value);
			parm->nPauseMAC_Src[5] = value & 0xFF;
			parm->nPauseMAC_Src[4] = (value >> 8) & 0xFF;
			gsw_r32(cdev, MAC_PFSA_1_PFAD_OFFSET,
				MAC_PFSA_1_PFAD_SHIFT, MAC_PFSA_1_PFAD_SIZE, &value);
			parm->nPauseMAC_Src[3] = value & 0xFF;
			parm->nPauseMAC_Src[2] = (value >> 8) & 0xFF;
			gsw_r32(cdev, MAC_PFSA_2_PFAD_OFFSET,
				MAC_PFSA_2_PFAD_SHIFT, MAC_PFSA_2_PFAD_SIZE, &value);
			parm->nPauseMAC_Src[1] = value & 0xFF;
			parm->nPauseMAC_Src[0] = (value >> 8) & 0xFF;
		}
	} else if (IS_VRSN_31(gswdev->gipver)) {
#if defined(CONFIG_MAC) && CONFIG_MAC
		/* Get MAC used in pause frame */
		ops = get_mac_ops(gswdev, 2);

		if (!ops) {
			pr_err("MAC 2 is not initialized\n");
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}

		ops->get_pfsa(ops, parm->nPauseMAC_Src, &parm->bPauseMAC_ModeSrc);
#endif
	}

	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_misc);
#endif
	return ret;
}

GSW_return_t GSW_CfgSet(void *cdev, GSW_cfg_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	struct mac_ops *ops;
	u32 MANT = 0, EXP = 0, value, i;
	u32 max_mac;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_misc);
#endif

	switch (parm->eMAC_TableAgeTimer) {
	case GSW_AGETIMER_1_SEC:
		MANT = AGETIMER_1_SEC;
		EXP = 0x2;
		gswdev->matimer = 1;
		break;

	case GSW_AGETIMER_10_SEC:
		MANT = AGETIMER_10_SEC;
		EXP = 0x2;
		gswdev->matimer = 10;
		break;

	case GSW_AGETIMER_300_SEC:
		MANT = AGETIMER_300_SEC;
		EXP = 0x2;
		gswdev->matimer = 300;
		break;

	case GSW_AGETIMER_1_HOUR:
		MANT = AGETIMER_1_HOUR;
		EXP = 0x6;
		gswdev->matimer = 3600;
		break;

	case GSW_AGETIMER_1_DAY:
		MANT = AGETIMER_1_DAY;
		EXP = 0xA;
		gswdev->matimer = 86400;
		break;

	default:
		MANT = AGETIMER_300_SEC;
		EXP = 0x2;
		gswdev->matimer = 300;
	}

	/* Aging Counter Exponent Value */
	gsw_w32(cdev, PCE_AGE_0_EXP_OFFSET,
		PCE_AGE_0_EXP_SHIFT, PCE_AGE_0_EXP_SIZE, EXP);
	/* Aging Counter Mantissa Value */
	gsw_w32(cdev, PCE_AGE_1_MANT_OFFSET,
		PCE_AGE_1_MANT_SHIFT, PCE_AGE_1_MANT_SIZE, MANT);

	/* Maximum Ethernet packet length */
	if (IS_VRSN_NOT_31(gswdev->gipver)) {
		if (parm->nMaxPacketLen < MAX_PACKET_LENGTH)
			value = parm->nMaxPacketLen;
		else
			value = MAX_PACKET_LENGTH;

		gsw_w32(cdev, MAC_FLEN_LEN_OFFSET,
			MAC_FLEN_LEN_SHIFT, MAC_FLEN_LEN_SIZE, value);

		if (parm->nMaxPacketLen > 0x5EE) {
			for (i = 0; i < 6; i++) {
				gsw_w32(cdev, (MAC_CTRL_2_MLEN_OFFSET + (i * 0xC)),
					MAC_CTRL_2_MLEN_SHIFT,
					MAC_CTRL_2_MLEN_SIZE, 1);
			}
		}
	} else if (IS_VRSN_31(gswdev->gipver)) {
		/* Set MTU for MAC2/3/4 */
		if (parm->nMaxPacketLen < V31_MAX_PACKET_LENGTH)
			value = parm->nMaxPacketLen;
		else
			value = V31_MAX_PACKET_LENGTH;

#if defined(CONFIG_MAC) && CONFIG_MAC
		max_mac = gsw_get_mac_subifcnt(0);

		for (i = 2; i < max_mac + 2; i++) {
			ops = get_mac_ops(gswdev, i);

			if (!ops) {
				pr_err("MAC %d is not initialized\n", i);
				ret = GSW_statusErr;
				goto UNLOCK_AND_RETURN;
			}

			ops->set_mtu(ops, value);
		}

#endif
	}

	/* MAC Address Learning Limitation Mode */
	gsw_w32(cdev, PCE_GCTRL_0_PLIMMOD_OFFSET,
		PCE_GCTRL_0_PLIMMOD_SHIFT,
		PCE_GCTRL_0_PLIMMOD_SIZE, parm->bLearningLimitAction);

	/*Accept or discard MAC spoofing and port */
	/* MAC locking violation packets */
	if ((gswdev->gipver == LTQ_GSWIP_2_2) ||
	    (IS_VRSN_30_31(gswdev->gipver)) ||
	    (gswdev->gipver == LTQ_GSWIP_2_2_ETC)) {
		gsw_w32(cdev, PCE_GCTRL_0_PLCKMOD_OFFSET,
			PCE_GCTRL_0_PLCKMOD_SHIFT,
			PCE_GCTRL_0_PLCKMOD_SIZE, parm->bMAC_SpoofingAction);
	}

	/* VLAN-aware Switching */
	if (IS_VRSN_NOT_31(gswdev->gipver)) {
		gsw_w32(cdev, PCE_GCTRL_0_VLAN_OFFSET,
			PCE_GCTRL_0_VLAN_SHIFT,
			PCE_GCTRL_0_VLAN_SIZE, parm->bVLAN_Aware);

		if (parm->bPauseMAC_ModeSrc == 1) {
			gsw_w32(cdev, MAC_PFAD_CFG_SAMOD_OFFSET,
				MAC_PFAD_CFG_SAMOD_SHIFT,
				MAC_PFAD_CFG_SAMOD_SIZE, parm->bPauseMAC_ModeSrc);
			value = parm->nPauseMAC_Src[4] << 8 | parm->nPauseMAC_Src[5];
			gsw_w32(cdev, MAC_PFSA_0_PFAD_OFFSET,
				MAC_PFSA_0_PFAD_SHIFT, MAC_PFSA_0_PFAD_SIZE, value);
			value = parm->nPauseMAC_Src[2] << 8 | parm->nPauseMAC_Src[3];
			gsw_w32(cdev, MAC_PFSA_1_PFAD_OFFSET,
				MAC_PFSA_1_PFAD_SHIFT, MAC_PFSA_1_PFAD_SIZE, value);
			value = parm->nPauseMAC_Src[0] << 8 | parm->nPauseMAC_Src[1];
			gsw_w32(cdev, MAC_PFSA_2_PFAD_OFFSET,
				MAC_PFSA_2_PFAD_SHIFT,
				MAC_PFSA_2_PFAD_SIZE, value);
		}
	} else if (IS_VRSN_31(gswdev->gipver)) {
#if defined(CONFIG_MAC) && CONFIG_MAC
		max_mac = gsw_get_mac_subifcnt(0);

		for (i = 2; i < max_mac + 2; i++) {

			/* Set MAC to use in pause frame */
			ops = get_mac_ops(gswdev, 2);

			if (!ops) {
				pr_err("MAC 2 is not initialized\n");
				ret = GSW_statusErr;
				goto UNLOCK_AND_RETURN;
			}

			ops->set_pfsa(ops, parm->nPauseMAC_Src,
				      parm->bPauseMAC_ModeSrc);
		}

#endif
	}

	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_misc);
#endif
	return ret;
}

GSW_return_t GSW_HW_Init(void *cdev, GSW_HW_Init_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 j = 0;
	u32 ret = 0;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_pce);
#endif

	if (gswdev->gsw_dev == LTQ_FLOW_DEV_EXT_AX3000_F24S) {
		/* Reset the Switch via Switch IP register*/
		get_gsw_hw_cap(cdev);
		/* Software Table Init */
		ltq_ethsw_port_cfg_init(cdev);
		//rst_multi_sw_table(cdev);
		/* HW Init */
		gsw_pmicro_code_init_f24s(cdev);
		/*hardcoded setting for LTQ_FLOW_DEV_EXT_AX3000_F24S*/
		ret = GSW_statusOk;

	} else {

		/* Reset the Switch via Switch IP register*/
		j = 1;
		gsw_w32(cdev, ETHSW_SWRES_R0_OFFSET,
			ETHSW_SWRES_R0_SHIFT, ETHSW_SWRES_R0_SIZE, j);

		do {
//		udelay(100);
			gsw_r32(cdev, ETHSW_SWRES_R0_OFFSET,
				ETHSW_SWRES_R0_SHIFT, ETHSW_SWRES_R0_SIZE, &j);
		} while (j);

#if defined(CONFIG_USE_EMULATOR) && CONFIG_USE_EMULATOR

		if (gswdev->gipver == LTQ_GSWIP_3_0) {
			/* Set Auto-Polling of connected PHYs - For all ports */
			gsw_w32(cdev, (GSWT_MDCCFG_0_PEN_1_OFFSET
				       + GSW30_TOP_OFFSET),
				GSWT_MDCCFG_0_PEN_1_SHIFT, 6, 0x0);
		} else {
			/* Set Auto-Polling of connected PHYs - For all ports */
			gsw_w32(cdev, (MDC_CFG_0_PEN_0_OFFSET
				       + GSW_TREG_OFFSET),
				MDC_CFG_0_PEN_0_SHIFT, 6, 0x0);
		}

#else

		if (gswdev->gipver == LTQ_GSWIP_3_0) {
			if (gswdev->sdev == LTQ_FLOW_DEV_INT_R) {
#ifdef __KERNEL__
				gsw_r_init();
#endif
				gsw_w32(cdev, (GSWT_MDCCFG_0_PEN_1_OFFSET + GSW30_TOP_OFFSET),
					GSWT_MDCCFG_0_PEN_1_SHIFT, 6, 0x1);
			} else {
				gsw_w32(cdev, (GSWT_MDCCFG_0_PEN_1_OFFSET + GSW30_TOP_OFFSET),
					GSWT_MDCCFG_0_PEN_1_SHIFT, 6, 0x1e);
			}
		}

#endif  /* CONFIG_USE_EMULATOR */
		/*	platform_device_init(cdev); */
		gswdev->hwinit = 1;
		/*	get_gsw_hw_cap (cdev); */
		/* Software Table Init */
#if defined(CONFIG_LTQ_VLAN) && CONFIG_LTQ_VLAN
		reset_vlan_sw_table(cdev);
#endif /*CONFIG_LTQ_VLAN */
		ltq_ethsw_port_cfg_init(cdev);
#if defined(CONFIG_LTQ_MULTICAST) && CONFIG_LTQ_MULTICAST
		reset_multicast_sw_table(cdev);
#endif /*CONFIG_LTQ_MULTICAST*/
		pce_table_init(&gswdev->phandler);
		/* HW Init */
		gsw_pmicro_code_init(cdev);

		if (gswdev->gipver == LTQ_GSWIP_3_0) {
			if (gswdev->sdev == LTQ_FLOW_DEV_INT_R) {
				/*suresh*/
				//			rt_table_init();
				gsw_w32(cdev, PCE_TFCR_NUM_NUM_OFFSET, PCE_TFCR_NUM_NUM_SHIFT,
					PCE_TFCR_NUM_NUM_SIZE, 0x80);
			}

			/* EEE auto negotiation overides:*/
			/*  clock disable (ANEG_EEE_0.CLK_STOP_CAPABLE)  */
			for (j = 0; j < gswdev->pnum - 1; j++) {
				gsw_w32(cdev,
					((GSWT_ANEG_EEE_1_CLK_STOP_CAPABLE_OFFSET
					  + (4 * j)) + GSW30_TOP_OFFSET),
					GSWT_ANEG_EEE_1_CLK_STOP_CAPABLE_SHIFT,
					GSWT_ANEG_EEE_1_CLK_STOP_CAPABLE_SIZE, 0x3);
			}
		} else {
			/* Configure the MDIO Clock 97.6 Khz */
			gsw_w32(cdev, (MDC_CFG_1_FREQ_OFFSET + GSW_TREG_OFFSET),
				MDC_CFG_1_FREQ_SHIFT,
				MDC_CFG_1_FREQ_SIZE, 0xFF);

			for (j = 0; j < gswdev->pnum - 1; j++) {
				gsw_w32(cdev, ((ANEG_EEE_0_CLK_STOP_CAPABLE_OFFSET + j)
					       + GSW_TREG_OFFSET),
					ANEG_EEE_0_CLK_STOP_CAPABLE_SHIFT,
					ANEG_EEE_0_CLK_STOP_CAPABLE_SIZE, 0x3);
			}
		}

		ret = GSW_statusOk;
	}

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_pce);
#endif
	return ret;

}

GSW_return_t GSW_MDIO_CfgGet(void *cdev, GSW_MDIO_cfg_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 value;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_mdio);
#endif

	if (gswdev->gipver == LTQ_GSWIP_3_0) {
		gsw_r32(cdev, (GSWT_MDCCFG_1_FREQ_OFFSET
			       + GSW30_TOP_OFFSET),
			GSWT_MDCCFG_1_FREQ_SHIFT,
			GSWT_MDCCFG_1_FREQ_SIZE, &value);
		parm->nMDIO_Speed = value & 0xFF;
		gsw_r32(cdev, (GSWT_MDCCFG_1_MCEN_OFFSET
			       + GSW30_TOP_OFFSET),
			GSWT_MDCCFG_1_MCEN_SHIFT,
			GSWT_MDCCFG_1_MCEN_SIZE, &value);
		parm->bMDIO_Enable = value;
	} else {
		gsw_r32(cdev, (MDC_CFG_1_FREQ_OFFSET + GSW_TREG_OFFSET),
			MDC_CFG_1_FREQ_SHIFT,
			MDC_CFG_1_FREQ_SIZE, &value);
		parm->nMDIO_Speed = value & 0xFF;
		gsw_r32(cdev, (MDC_CFG_1_MCEN_OFFSET
			       + GSW_TREG_OFFSET),
			MDC_CFG_1_MCEN_SHIFT,
			MDC_CFG_1_MCEN_SIZE, &value);
		parm->bMDIO_Enable = value;
	}

	if (value == 1)
		parm->bMDIO_Enable = 1;
	else
		parm->bMDIO_Enable = 0;

	ret = GSW_statusOk;

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_mdio);
#endif
	return ret;
}

GSW_return_t GSW_MDIO_CfgSet(void *cdev, GSW_MDIO_cfg_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 value;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_mdio);
#endif

	pr_debug("**********%s:%s:%d*************\n", __FILE__, __func__, __LINE__);

	if (gswdev->gipver == LTQ_GSWIP_3_0) {
		gsw_w32(cdev, (GSWT_MDCCFG_1_FREQ_OFFSET + GSW30_TOP_OFFSET),
			GSWT_MDCCFG_1_FREQ_SHIFT, GSWT_MDCCFG_1_FREQ_SIZE, parm->nMDIO_Speed);

		if (parm->bMDIO_Enable)
			value = 0x3F;
		else
			value = 0;

		/* Set Auto-Polling of connected PHYs - For all ports */
		gsw_w32(cdev, (GSWT_MDCCFG_0_PEN_1_OFFSET + GSW30_TOP_OFFSET),
			GSWT_MDCCFG_0_PEN_1_SHIFT, 6, value);
		gsw_w32(cdev, (GSWT_MDCCFG_1_MCEN_OFFSET + GSW30_TOP_OFFSET),
			GSWT_MDCCFG_1_MCEN_SHIFT, GSWT_MDCCFG_1_MCEN_SIZE, parm->bMDIO_Enable);
	} else {
		gsw_w32(cdev, (MDC_CFG_1_FREQ_OFFSET + GSW_TREG_OFFSET),
			MDC_CFG_1_FREQ_SHIFT, MDC_CFG_1_FREQ_SIZE, parm->nMDIO_Speed);

		if (parm->bMDIO_Enable)
			value = 0x3F;
		else
			value = 0;

		/* Set Auto-Polling of connected PHYs - For all ports */
		gsw_w32(cdev, (MDC_CFG_0_PEN_0_OFFSET + GSW_TREG_OFFSET),
			MDC_CFG_0_PEN_0_SHIFT, 6, value);
		gsw_w32(cdev, (MDC_CFG_1_MCEN_OFFSET + GSW_TREG_OFFSET),
			MDC_CFG_1_MCEN_SHIFT,
			MDC_CFG_1_MCEN_SIZE, parm->bMDIO_Enable);
	}

	ret = GSW_statusOk;

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_mdio);
#endif
	return ret;
}

GSW_return_t GSW_MDIO_DataRead(void *cdev, GSW_MDIO_data_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 value;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_mdio);
#endif

	if (gswdev->gipver == LTQ_GSWIP_3_0) {

		CHECK_BUSY_MDIO((GSWT_MDCTRL_MBUSY_OFFSET + GSW30_TOP_OFFSET),
				GSWT_MDCTRL_MBUSY_SHIFT, GSWT_MDCTRL_MBUSY_SIZE, RETURN_FROM_FUNCTION);

		value = ((0x2 << 10) | ((parm->nAddressDev & 0x1F) << 5)
			 | (parm->nAddressReg & 0x1F));
		/* Special write command, becouse we need to write */
		/* "MDIO Control Register" once at a time */
		gsw_w32(cdev, (GSWT_MDCTRL_MBUSY_OFFSET + GSW30_TOP_OFFSET), 0, 16, value);

		CHECK_BUSY_MDIO((GSWT_MDCTRL_MBUSY_OFFSET + GSW30_TOP_OFFSET),
				GSWT_MDCTRL_MBUSY_SHIFT, GSWT_MDCTRL_MBUSY_SIZE, RETURN_FROM_FUNCTION);

		gsw_r32(cdev, (GSWT_MDREAD_RDATA_OFFSET + GSW30_TOP_OFFSET),
			GSWT_MDREAD_RDATA_SHIFT, GSWT_MDREAD_RDATA_SIZE, &value);
		parm->nData = (value & 0xFFFF);
	} else {
		CHECK_BUSY_MDIO((MDIO_CTRL_MBUSY_OFFSET + GSW_TREG_OFFSET),
				MDIO_CTRL_MBUSY_SHIFT, MDIO_CTRL_MBUSY_SIZE, RETURN_FROM_FUNCTION);
		value = ((0x2 << 10) | ((parm->nAddressDev & 0x1F) << 5)
			 | (parm->nAddressReg & 0x1F));
		/* Special write command, becouse we need to write */
		/* "MDIO Control Register" once at a time */
		gsw_w32(cdev, (MDIO_CTRL_MBUSY_OFFSET + GSW_TREG_OFFSET), 0, 16, value);
		CHECK_BUSY_MDIO((MDIO_CTRL_MBUSY_OFFSET + GSW_TREG_OFFSET),
				MDIO_CTRL_MBUSY_SHIFT, MDIO_CTRL_MBUSY_SIZE, RETURN_FROM_FUNCTION);

		gsw_r32(cdev, (MDIO_READ_RDATA_OFFSET + GSW_TREG_OFFSET),
			MDIO_READ_RDATA_SHIFT, MDIO_READ_RDATA_SIZE, &value);
		parm->nData = (value & 0xFFFF);
	}

	ret = GSW_statusOk;

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_mdio);
#endif
	return ret;

}

GSW_return_t GSW_MDIO_DataWrite(void *cdev, GSW_MDIO_data_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 value;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_mdio);
#endif

	if (gswdev->gipver == LTQ_GSWIP_3_0) {
		CHECK_BUSY_MDIO((GSWT_MDCTRL_MBUSY_OFFSET + GSW30_TOP_OFFSET),
				GSWT_MDCTRL_MBUSY_SHIFT, GSWT_MDCTRL_MBUSY_SIZE, RETURN_FROM_FUNCTION);
		value = parm->nData & 0xFFFF;
		gsw_w32(cdev, (GSWT_MDWRITE_WDATA_OFFSET + GSW30_TOP_OFFSET),
			GSWT_MDWRITE_WDATA_SHIFT, GSWT_MDWRITE_WDATA_SIZE, value);
		value = ((0x1 << 10) | ((parm->nAddressDev & 0x1F) << 5)
			 | (parm->nAddressReg & 0x1F));
		/* Special write command, becouse we need to write*/
		/* "MDIO Control Register" once at a time */
		gsw_w32(cdev, (GSWT_MDCTRL_MBUSY_OFFSET + GSW30_TOP_OFFSET), 0, 16, value);

		CHECK_BUSY_MDIO((GSWT_MDCTRL_MBUSY_OFFSET + GSW30_TOP_OFFSET),
				GSWT_MDCTRL_MBUSY_SHIFT, GSWT_MDCTRL_MBUSY_SIZE, RETURN_FROM_FUNCTION);
	} else {

		CHECK_BUSY_MDIO((MDIO_CTRL_MBUSY_OFFSET + GSW_TREG_OFFSET),
				MDIO_CTRL_MBUSY_SHIFT, MDIO_CTRL_MBUSY_SIZE, RETURN_FROM_FUNCTION);
		value = parm->nData & 0xFFFF;
		gsw_w32(cdev, (MDIO_WRITE_WDATA_OFFSET + GSW_TREG_OFFSET),
			MDIO_WRITE_WDATA_SHIFT, MDIO_WRITE_WDATA_SIZE, value);
		value = ((0x1 << 10) | ((parm->nAddressDev & 0x1F) << 5)
			 | (parm->nAddressReg & 0x1F));
		/* Special write command, becouse we need to write*/
		/* "MDIO Control Register" once at a time */
		gsw_w32(cdev, (MDIO_CTRL_MBUSY_OFFSET + GSW_TREG_OFFSET), 0, 16, value);

		CHECK_BUSY_MDIO((MDIO_CTRL_MBUSY_OFFSET + GSW_TREG_OFFSET),
				MDIO_CTRL_MBUSY_SHIFT, MDIO_CTRL_MBUSY_SIZE, RETURN_FROM_FUNCTION);
	}

	ret = GSW_statusOk;

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_mdio);
#endif
	return ret;

}

static inline void ltq_mdelay(int delay)
{
	int i;

	for (i = delay; i > 0; i--)
		;

	//udelay(1000);
}

static int force_to_configure_phy_settings(void *cdev, u8 pidx, u8 link_status)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 mdio_stat_reg, phy_addr_reg = 0;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

	if (pidx >= (gswdev->pnum - 1))
		return GSW_statusErr;

//	if ((pidx == GSW_3X_SOC_CPU_PORT) || ((pidx == GSW_2X_SOC_CPU_PORT)))
//		return 1;
	if (gswdev->gipver == LTQ_GSWIP_3_0) {
		gsw_r32(cdev, ((GSWT_PHY_ADDR_1_ADDR_OFFSET
				+ ((pidx - 1) * 4)) + GSW30_TOP_OFFSET),
			GSWT_PHY_ADDR_1_ADDR_SHIFT, 16, &phy_addr_reg);
		gsw_r32(cdev, ((GSWT_MDIO_STAT_1_TXPAUEN_OFFSET
				+ ((pidx - 1) * 4)) + GSW30_TOP_OFFSET),
			GSWT_MDIO_STAT_1_TXPAUEN_SHIFT, 16, &mdio_stat_reg);
	} else {
		gsw_r32(cdev, ((PHY_ADDR_0_ADDR_OFFSET - pidx)
			       + GSW_TREG_OFFSET),
			PHY_ADDR_0_ADDR_SHIFT, 16, &phy_addr_reg);
		gsw_r32(cdev, (MDIO_STAT_0_TXPAUEN_OFFSET
			       + GSW_TREG_OFFSET + pidx),
			MDIO_STAT_0_TXPAUEN_SHIFT, 16, &mdio_stat_reg);
	}

	if (link_status) {
		/* PHY active Status */
		if ((mdio_stat_reg >> 6) & 0x1) {
			u32 temp = 0;

			/* Link Status */
			if ((mdio_stat_reg >> 5) & 0x1) {
				phy_addr_reg &= ~(0xFFE0);
				phy_addr_reg |= (1 << 13); /* Link up */
				temp = ((mdio_stat_reg >> 3) & 0x3); /*Speed */
				phy_addr_reg |= (temp << 11); /*Speed */

				if ((mdio_stat_reg >> 2) & 0x1) { /*duplex */
					phy_addr_reg |= (0x1 << 9); /*duplex */
				} else {
					phy_addr_reg |= (0x3 << 9);
				}

				/*Receive Pause Enable Status */
				if ((mdio_stat_reg >> 1) & 0x1) {
					/*Receive Pause Enable Status */
					phy_addr_reg |= (0x1 << 5);
				} else {
					phy_addr_reg |= (0x3 << 5);
				}

				/*Transmit Pause Enable Status */
				if ((mdio_stat_reg >> 0) & 0x1) {
					/*Transmit Pause Enable Status */
					phy_addr_reg |= (0x1 << 7);
				} else {
					phy_addr_reg |= (0x3 << 7);
				}

				if (gswdev->gipver == LTQ_GSWIP_3_0) {
					if (gswdev->sdev == LTQ_FLOW_DEV_INT_R) {
						//Reddy
					} else {
					}

					gsw_w32(cdev,
						((GSWT_PHY_ADDR_1_ADDR_OFFSET
						  + ((pidx - 1) * 4))
						 + GSW30_TOP_OFFSET),
						GSWT_PHY_ADDR_1_ADDR_SHIFT,
						16, phy_addr_reg);
				} else {
					gsw_w32(cdev,
						((PHY_ADDR_0_ADDR_OFFSET - pidx)
						 + GSW_TREG_OFFSET),
						PHY_ADDR_0_ADDR_SHIFT,
						16, phy_addr_reg);
				}
			}
		}
	} else {
		phy_addr_reg &= ~(0xFFE0);
		phy_addr_reg |= (0x3 << 11);

		if (gswdev->gipver == LTQ_GSWIP_3_0) {
			if (gswdev->sdev == LTQ_FLOW_DEV_INT_R) {
				//Reddy
			} else {
			}

			gsw_w32(cdev, ((GSWT_PHY_ADDR_1_ADDR_OFFSET
					+ ((pidx - 1) * 4)) + GSW30_TOP_OFFSET),
				GSWT_PHY_ADDR_1_ADDR_SHIFT,
				16, phy_addr_reg);
		} else {
			gsw_w32(cdev, ((PHY_ADDR_0_ADDR_OFFSET - pidx)
				       + GSW_TREG_OFFSET),
				PHY_ADDR_0_ADDR_SHIFT, 16, phy_addr_reg);
		}
	}

	return 1;
}

GSW_return_t GSW_MmdDataRead(void *cdev, GSW_MMD_data_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	GSW_MDIO_data_t mmd_data;
	u32 found = 0, rphy, rmdc, phy_addr, mdc_reg, dev, pidx;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_mmd);
#endif

	if (gswdev->gipver == LTQ_GSWIP_3_0) {
		if (gswdev->sdev == LTQ_FLOW_DEV_INT_R) {
			if ((parm->nAddressDev == 1)) {
				gsw_r32(cdev, (GSWT_PHY_ADDR_1_ADDR_OFFSET + GSW30_TOP_OFFSET),
					GSWT_PHY_ADDR_1_ADDR_SHIFT, GSWT_PHY_ADDR_1_ADDR_SIZE, &phy_addr);
				gsw_r32(cdev, (GSWT_PHY_ADDR_1_ADDR_OFFSET + GSW30_TOP_OFFSET),
					GSWT_PHY_ADDR_1_ADDR_SHIFT, 16, &rphy);
				found = 1;
				pidx = 1;
			} else {
				ret = GSW_statusErr;
				goto UNLOCK_AND_RETURN;
			}
		} else {
			for (pidx = 1; pidx < gswdev->pnum; pidx++) {
				gsw_r32(cdev, ((GSWT_PHY_ADDR_1_ADDR_OFFSET + ((pidx - 1) * 4)) + GSW30_TOP_OFFSET),
					GSWT_PHY_ADDR_1_ADDR_SHIFT, GSWT_PHY_ADDR_1_ADDR_SIZE, &phy_addr);

				if (phy_addr == parm->nAddressDev) {
					found = 1;
					gsw_r32(cdev, ((GSWT_PHY_ADDR_1_ADDR_OFFSET + ((pidx - 1) * 4)) + GSW30_TOP_OFFSET),
						GSWT_PHY_ADDR_1_ADDR_SHIFT, 16, &rphy);
					break;
				}
			}
		}
	} else {
		for (pidx = 0; pidx < (gswdev->pnum - 1); pidx++) {
			gsw_r32(cdev, ((PHY_ADDR_0_ADDR_OFFSET - pidx) + GSW_TREG_OFFSET),
				PHY_ADDR_0_ADDR_SHIFT, PHY_ADDR_0_ADDR_SIZE, &phy_addr);

			if (phy_addr == parm->nAddressDev) {
				found = 1;
				gsw_r32(cdev, ((PHY_ADDR_0_ADDR_OFFSET - pidx) + GSW_TREG_OFFSET),
					PHY_ADDR_0_ADDR_SHIFT, 16, &rphy);
				break;
			}
		}
	}

	if (found) {
		force_to_configure_phy_settings(cdev, pidx, 1);

		if (gswdev->gipver == LTQ_GSWIP_3_0) {
			gsw_r32(cdev, (GSWT_MDCCFG_0_PEN_1_OFFSET + GSW30_TOP_OFFSET),
				GSWT_MDCCFG_0_PEN_1_SHIFT, 6, &mdc_reg);
		} else {
			gsw_r32(cdev, (MDC_CFG_0_PEN_0_OFFSET + GSW_TREG_OFFSET),
				MDC_CFG_0_PEN_0_SHIFT, 6, &mdc_reg);
		}

		rmdc = mdc_reg;
		mdc_reg &= ~(1 << pidx);
		dev = ((parm->nAddressReg >> 16) & 0x1F);

		if (gswdev->gipver == LTQ_GSWIP_3_0) {
			gsw_w32(cdev, (GSWT_MDCCFG_0_PEN_1_OFFSET + GSW30_TOP_OFFSET),
				GSWT_MDCCFG_0_PEN_1_SHIFT, 6, mdc_reg);
		} else {
			gsw_w32(cdev, (MDC_CFG_0_PEN_0_OFFSET + GSW_TREG_OFFSET),
				MDC_CFG_0_PEN_0_SHIFT, 6, mdc_reg);
		}

		ltq_mdelay(20);
		mmd_data.nAddressDev = parm->nAddressDev;
		mmd_data.nAddressReg = 0xd;
		mmd_data.nData = dev;
		GSW_MDIO_DataWrite(cdev, &mmd_data);

		mmd_data.nAddressDev = parm->nAddressDev;
		mmd_data.nAddressReg = 0xe;
		mmd_data.nData = parm->nAddressReg & 0xFFFF;
		GSW_MDIO_DataWrite(cdev, &mmd_data);

		mmd_data.nAddressDev = parm->nAddressDev;
		mmd_data.nAddressReg = 0xd;
		mmd_data.nData = ((0x4000) | dev);
		GSW_MDIO_DataWrite(cdev, &mmd_data);

		mmd_data.nAddressDev = parm->nAddressDev;
		mmd_data.nAddressReg = 0xe;
		GSW_MDIO_DataRead(cdev, &mmd_data);
		parm->nData = mmd_data.nData;

//		mdc_reg |= (1 << pidx);
		if (gswdev->gipver == LTQ_GSWIP_3_0) {
			gsw_w32(cdev, ((GSWT_PHY_ADDR_1_ADDR_OFFSET + ((pidx - 1) * 4)) + GSW30_TOP_OFFSET),
				GSWT_PHY_ADDR_1_ADDR_SHIFT, 16, rphy);
			gsw_w32(cdev, (GSWT_MDCCFG_0_PEN_1_OFFSET + GSW30_TOP_OFFSET),
				GSWT_MDCCFG_0_PEN_1_SHIFT, 6, rmdc);
		} else {
			gsw_w32(cdev, ((PHY_ADDR_0_ADDR_OFFSET - pidx) + GSW_TREG_OFFSET),
				PHY_ADDR_0_ADDR_SHIFT, 16, rphy);
			gsw_w32(cdev, (MDC_CFG_0_PEN_0_OFFSET + GSW_TREG_OFFSET),
				MDC_CFG_0_PEN_0_SHIFT, 6, rmdc);
		}

		ltq_mdelay(100);
		force_to_configure_phy_settings(cdev, pidx, 0);
	} else {
		pr_err("(Invalid PHY Address) %s:%s:%d\n",
		       __FILE__, __func__, __LINE__);
	}

	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_mmd);
#endif
	return ret;

}

GSW_return_t GSW_MmdDataWrite(void *cdev, GSW_MMD_data_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	GSW_MDIO_data_t mmd_data;
	u32 found = 0, rphy, rmdc, phy_addr, mdc_reg, dev, pidx;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_mmd);
#endif

	if (gswdev->gipver == LTQ_GSWIP_3_0) {
		if (gswdev->sdev == LTQ_FLOW_DEV_INT_R) {
			if ((parm->nAddressDev == 1)) {
				gsw_r32(cdev, (GSWT_PHY_ADDR_1_ADDR_OFFSET + GSW30_TOP_OFFSET),
					GSWT_PHY_ADDR_1_ADDR_SHIFT, GSWT_PHY_ADDR_1_ADDR_SIZE, &phy_addr);
				gsw_r32(cdev, (GSWT_PHY_ADDR_1_ADDR_OFFSET + GSW30_TOP_OFFSET),
					GSWT_PHY_ADDR_1_ADDR_SHIFT, 16, &rphy);
				found = 1;
				pidx = 1;
			} else {
				ret = GSW_statusErr;
				goto UNLOCK_AND_RETURN;
			}
		} else {
			for (pidx = 1; pidx < gswdev->pnum; pidx++) {
				gsw_r32(cdev, ((GSWT_PHY_ADDR_1_ADDR_OFFSET + ((pidx - 1) * 4)) + GSW30_TOP_OFFSET),
					GSWT_PHY_ADDR_1_ADDR_SHIFT, GSWT_PHY_ADDR_1_ADDR_SIZE, &phy_addr);

				if (phy_addr == parm->nAddressDev) {
					found = 1;
					gsw_r32(cdev, ((GSWT_PHY_ADDR_1_ADDR_OFFSET + ((pidx - 1) * 4)) + GSW30_TOP_OFFSET),
						GSWT_PHY_ADDR_1_ADDR_SHIFT, 16, &rphy);
					break;
				}
			}
		}
	} else {
		for (pidx = 0; pidx < (gswdev->pnum - 1); pidx++) {
			gsw_r32(cdev, ((PHY_ADDR_0_ADDR_OFFSET - pidx) + GSW_TREG_OFFSET),
				PHY_ADDR_0_ADDR_SHIFT, PHY_ADDR_0_ADDR_SIZE, &phy_addr);

			if (phy_addr == parm->nAddressDev) {
				found = 1;
				gsw_r32(cdev, ((PHY_ADDR_0_ADDR_OFFSET - pidx) + GSW_TREG_OFFSET),
					PHY_ADDR_0_ADDR_SHIFT, 16, &rphy);
				break;
			}
		}
	}

	if (found) {
		force_to_configure_phy_settings(cdev, pidx, 1);

		if (gswdev->gipver == LTQ_GSWIP_3_0) {
			gsw_r32(cdev, (GSWT_MDCCFG_0_PEN_1_OFFSET
				       + GSW30_TOP_OFFSET),
				GSWT_MDCCFG_0_PEN_1_SHIFT, 6, &mdc_reg);
		} else {
			gsw_r32(cdev, (MDC_CFG_0_PEN_0_OFFSET
				       + GSW_TREG_OFFSET),
				MDC_CFG_0_PEN_0_SHIFT, 6, &mdc_reg);
		}

		rmdc = mdc_reg;
		mdc_reg &= ~(1 << pidx);
		dev = ((parm->nAddressReg >> 16) & 0x1F);

		if (gswdev->gipver == LTQ_GSWIP_3_0) {
			gsw_w32(cdev, (GSWT_MDCCFG_0_PEN_1_OFFSET
				       + GSW30_TOP_OFFSET),
				GSWT_MDCCFG_0_PEN_1_SHIFT, 6, mdc_reg);
		} else {
			gsw_w32(cdev, (MDC_CFG_0_PEN_0_OFFSET
				       + GSW_TREG_OFFSET),
				MDC_CFG_0_PEN_0_SHIFT, 6, mdc_reg);
		}

		ltq_mdelay(20);
		mmd_data.nAddressDev = parm->nAddressDev;
		mmd_data.nAddressReg = 0xd;
		mmd_data.nData = dev;
		GSW_MDIO_DataWrite(cdev, &mmd_data);

		mmd_data.nAddressDev = parm->nAddressDev;
		mmd_data.nAddressReg = 0xe;
		mmd_data.nData = parm->nAddressReg & 0xFFFF;
		GSW_MDIO_DataWrite(cdev, &mmd_data);

		mmd_data.nAddressDev = parm->nAddressDev;
		mmd_data.nAddressReg = 0xd;
		mmd_data.nData = ((0x4000) | dev);
		GSW_MDIO_DataWrite(cdev, &mmd_data);

		mmd_data.nAddressDev = parm->nAddressDev;
		mmd_data.nAddressReg = 0xe;
		mmd_data.nData = parm->nData;
		GSW_MDIO_DataWrite(cdev, &mmd_data);

//		mdc_reg |= (1 << pidx);
		if (gswdev->gipver == LTQ_GSWIP_3_0) {
			gsw_w32(cdev, ((GSWT_PHY_ADDR_1_ADDR_OFFSET + ((pidx - 1) * 4)) + GSW30_TOP_OFFSET),
				GSWT_PHY_ADDR_1_ADDR_SHIFT, 16, rphy);
			gsw_w32(cdev, (GSWT_MDCCFG_0_PEN_1_OFFSET + GSW30_TOP_OFFSET),
				GSWT_MDCCFG_0_PEN_1_SHIFT, 6, rmdc);
		} else {
			gsw_w32(cdev, ((PHY_ADDR_0_ADDR_OFFSET - pidx) + GSW_TREG_OFFSET),
				PHY_ADDR_0_ADDR_SHIFT, 16, rphy);
			gsw_w32(cdev, (MDC_CFG_0_PEN_0_OFFSET + GSW_TREG_OFFSET),
				MDC_CFG_0_PEN_0_SHIFT, 6, rmdc);
		}

		ltq_mdelay(100);
		force_to_configure_phy_settings(cdev, pidx, 0);
	} else {
		pr_err("(Invalid PHY Address)  %s:%s:%d\n",
		       __FILE__, __func__, __LINE__);
	}

	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_mmd);
#endif
	return ret;

}

static GSW_return_t GSW_MMD_MDIO_DataWrite(void *cdev,
		GSW_MDIO_data_t *parm, u8 pidx, u8 dev)
{
	GSW_MDIO_data_t mmd_data;
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 mdc_reg;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_mmd);
#endif

	if (pidx >= (gswdev->pnum - 1)) {
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	force_to_configure_phy_settings(cdev, pidx, 1);

	if (gswdev->gipver == LTQ_GSWIP_3_0) {
		gsw_r32(cdev, (GSWT_MDCCFG_0_PEN_1_OFFSET
			       + GSW30_TOP_OFFSET),
			GSWT_MDCCFG_0_PEN_1_SHIFT, 6, &mdc_reg);
	} else {
		gsw_r32(cdev, (MDC_CFG_0_PEN_0_OFFSET + GSW_TREG_OFFSET),
			MDC_CFG_0_PEN_0_SHIFT, 6, &mdc_reg);
	}

	mdc_reg &= ~(1 << pidx);

	if (gswdev->gipver == LTQ_GSWIP_3_0) {
		gsw_w32(cdev, (GSWT_MDCCFG_0_PEN_1_OFFSET
			       + GSW30_TOP_OFFSET),
			GSWT_MDCCFG_0_PEN_1_SHIFT, 6, mdc_reg);
	} else {
		gsw_w32(cdev, (MDC_CFG_0_PEN_0_OFFSET + GSW_TREG_OFFSET),
			MDC_CFG_0_PEN_0_SHIFT, 6, mdc_reg);
	}

	ltq_mdelay(20);

	mmd_data.nAddressDev = parm->nAddressDev;
	mmd_data.nAddressReg = 0xd;
	mmd_data.nData = dev;
	GSW_MDIO_DataWrite(cdev, &mmd_data);

	mmd_data.nAddressDev = parm->nAddressDev;
	mmd_data.nAddressReg = 0xe;
	mmd_data.nData = parm->nAddressReg;
	GSW_MDIO_DataWrite(cdev, &mmd_data);

	mmd_data.nAddressDev = parm->nAddressDev;
	mmd_data.nAddressReg = 0xd;
	mmd_data.nData = ((0x4000) | (dev & 0x1F));
	GSW_MDIO_DataWrite(cdev, &mmd_data);

	mmd_data.nAddressDev = parm->nAddressDev;
	mmd_data.nAddressReg = 0xe;
	mmd_data.nData = parm->nData;
	GSW_MDIO_DataWrite(cdev, &mmd_data);

	mdc_reg |= (1 << pidx);

	if (gswdev->gipver == LTQ_GSWIP_3_0) {
		gsw_w32(cdev, (GSWT_MDCCFG_0_PEN_1_OFFSET
			       + GSW30_TOP_OFFSET),
			GSWT_MDCCFG_0_PEN_1_SHIFT, 6, mdc_reg);
	} else {
		gsw_w32(cdev, (MDC_CFG_0_PEN_0_OFFSET + GSW_TREG_OFFSET),
			MDC_CFG_0_PEN_0_SHIFT, 6, mdc_reg);
	}

	ltq_mdelay(100);
	force_to_configure_phy_settings(cdev, pidx, 0);
	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_mmd);
#endif
	return ret;

}

GSW_return_t GSW_MonitorPortCfgGet(void *cdev, GSW_monitorPortCfg_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 pmapReg1 = 0;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_misc);
#endif

	//Validate given Port-Id.
	if (parm->nPortId >= gswdev->tpnum) {
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	//Get monitoring port-map.
	gsw_r32_raw(cdev, PCE_PMAP_1_REG_OFFSET, &pmapReg1);
	parm->nPortId = pmapReg1;

	if (IS_VRSN_31(gswdev->gipver)) {
		parm->bMonitorPort = 1;
		GET_VAL_FROM_REG(parm->nPortId, PCE_PMAP_1_PID_SHIFT, PCE_PMAP_1_PID_SIZE, pmapReg1);
		GET_VAL_FROM_REG(parm->nSubIfId, PCE_PMAP_1_SUBID_SHIFT, PCE_PMAP_1_SUBID_SIZE, pmapReg1);
	}

	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_misc);
#endif
	return ret;
}

GSW_return_t GSW_MonitorPortCfgSet(void *cdev, GSW_monitorPortCfg_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u8 pidx = parm->nPortId;
	u32 pmapReg1 = 0;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_misc);
#endif

	//Validate given Port-Id.
	if (parm->nPortId >= gswdev->tpnum) {
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	//En/disabel monitoring on given port-id. It is logical port-id.
	if (IS_VRSN_NOT_31(gswdev->gipver)) {
		//First read the monitoring register map.
		gsw_r32_raw(cdev, PCE_PMAP_1_REG_OFFSET, &pmapReg1);

		if (parm->bMonitorPort == 1)
			pmapReg1 |= (1 << pidx); //Enable monitoring.
		else
			pmapReg1 = (pmapReg1 & ~(1 << pidx)); ////Disable monitoring.

		//Update the monitoring register map.
		gsw_w32_raw(cdev, PCE_PMAP_1_REG_OFFSET, pmapReg1);
	} else {
		//To test, enable mirroring on ing/egr CTP port and check?.
		FILL_CTRL_REG(pmapReg1, PCE_PMAP_1_PID_SHIFT, parm->nPortId);
		FILL_CTRL_REG(pmapReg1, PCE_PMAP_1_SUBID_SHIFT, parm->nSubIfId);
		gsw_w32_raw(cdev, PCE_PMAP_1_REG_OFFSET, pmapReg1);
	}

	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_misc);
#endif
	return ret;
}

GSW_return_t GSW_PortLinkCfgGet(void *cdev, GSW_portLinkCfg_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 value;
	u8 pidx = parm->nPortId;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_misc);
#endif

	if (IS_VRSN_NOT_31(gswdev->gipver)) {
		if (gswdev->gipver == LTQ_GSWIP_3_0) {
			if (gswdev->sdev == LTQ_FLOW_DEV_INT_R) {
				if (pidx == 15)
					pidx = 0;
				else {
					ret = GSW_statusErr;
					goto UNLOCK_AND_RETURN;
				}
			} else {
				if (!parm->nPortId || (parm->nPortId > (gswdev->pnum - 1))) {
					ret = GSW_statusErr;
					goto UNLOCK_AND_RETURN;
				}

				pidx = pidx - 1;
			}
		} else {
			if (parm->nPortId > (gswdev->pnum - 1)) {
				ret = GSW_statusErr;
				goto UNLOCK_AND_RETURN;
			}
		}
	}

	if (IS_VRSN_31(gswdev->gipver)) {
#if defined(CONFIG_MAC) && CONFIG_MAC
		struct mac_ops *ops = get_mac_ops(gswdev, parm->nPortId);
		int val = 0;

		if (!ops) {
			pr_err("MAC %d is not initialized\n", parm->nPortId);
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}

		/* MAC API's to Get Duplex */
		val = ops->get_duplex(ops);

		if (val == -1)
			parm->bDuplexForce = 0;
		else {
			parm->bDuplexForce = 1;
			parm->eDuplex = val;
		}

		/* MAC API's to Get MII Interface */
		parm->eMII_Mode = ops->get_mii_if(ops);

		/* MAC API's to Get Speed */
		parm->eSpeed = ops->get_speed(ops);

		if (parm->eSpeed != 0)
			parm->bSpeedForce = 1;

		parm->bLPI = ops->get_lpi(ops);

		val = ops->get_link_sts(ops);

		if (val == -1)
			parm->bLinkForce = 0;
		else {
			parm->bLinkForce = 1;
			parm->eLink = val;
		}

#endif
	} else {
		gsw_r32(cdev, (MAC_PSTAT_FDUP_OFFSET + (0xC * pidx)),
			MAC_PSTAT_FDUP_SHIFT, MAC_PSTAT_FDUP_SIZE, &value);

		if (value)
			parm->eDuplex = GSW_DUPLEX_FULL;
		else
			parm->eDuplex = GSW_DUPLEX_HALF;

		gsw_r32(cdev, (MAC_PSTAT_GBIT_OFFSET + (0xC * pidx)),
			MAC_PSTAT_GBIT_SHIFT, MAC_PSTAT_GBIT_SIZE, &value);

		if (value) {
			parm->eSpeed = GSW_PORT_SPEED_1000;
		} else {
			gsw_r32(cdev, (MAC_PSTAT_MBIT_OFFSET + (0xC * pidx)),
				MAC_PSTAT_MBIT_SHIFT, MAC_PSTAT_MBIT_SIZE, &value);

			if (value)
				parm->eSpeed = GSW_PORT_SPEED_100;
			else
				parm->eSpeed = GSW_PORT_SPEED_10;
		}

		/* Low-power Idle Mode  configuration*/
		gsw_r32(cdev, (MAC_CTRL_4_LPIEN_OFFSET + (0xC * pidx)),
			MAC_CTRL_4_LPIEN_SHIFT, MAC_CTRL_4_LPIEN_SIZE, &value);
		parm->bLPI = value;
		gsw_r32(cdev, (MAC_PSTAT_LSTAT_OFFSET + (0xC * pidx)),
			MAC_PSTAT_LSTAT_SHIFT, MAC_PSTAT_LSTAT_SIZE, &value);

		if (value)
			parm->eLink = GSW_PORT_LINK_UP;
		else
			parm->eLink = GSW_PORT_LINK_DOWN;

		if (gswdev->gipver == LTQ_GSWIP_3_0) {
			gsw_r32(cdev, ((GSWT_PHY_ADDR_1_LNKST_OFFSET + ((pidx) * 4)) + GSW30_TOP_OFFSET),
				GSWT_PHY_ADDR_1_LNKST_SHIFT, GSWT_PHY_ADDR_1_LNKST_SIZE, &value);
		} else {
			gsw_r32(cdev, ((PHY_ADDR_0_ADDR_OFFSET - pidx) + GSW_TREG_OFFSET),
				PHY_ADDR_5_LNKST_SHIFT, PHY_ADDR_5_LNKST_SIZE, &value);
		}

		if ((value == 1) || (value == 2)) {
			parm->bLinkForce = 1;
		}

		if (gswdev->gipver == LTQ_GSWIP_3_0) {
			gsw_r32(cdev, (GSWT_MII_CFG_1_MIIMODE_OFFSET
				       + GSW30_TOP_OFFSET),
				GSWT_MII_CFG_1_MIIMODE_SHIFT,
				GSWT_MII_CFG_1_MIIMODE_SIZE, &value);
		} else {
			gsw_r32(cdev, (MII_CFG_0_MIIMODE_OFFSET + (0x2 * pidx)
				       + GSW_TREG_OFFSET),
				MII_CFG_0_MIIMODE_SHIFT,
				MII_CFG_0_MIIMODE_SIZE, &value);
		}

		switch (value) {
		case 0:
			parm->eMII_Mode = GSW_PORT_HW_MII;
			parm->eMII_Type = GSW_PORT_PHY;
			break;

		case 1:
			parm->eMII_Mode = GSW_PORT_HW_MII;
			parm->eMII_Type = GSW_PORT_MAC;
			break;

		case 2:
			parm->eMII_Mode = GSW_PORT_HW_RMII;
			parm->eMII_Type = GSW_PORT_PHY;
			break;

		case 3:
			parm->eMII_Mode = GSW_PORT_HW_RMII;
			parm->eMII_Type = GSW_PORT_MAC;
			break;

		case 4:
			parm->eMII_Mode = GSW_PORT_HW_RGMII;
			parm->eMII_Type = GSW_PORT_MAC;
			break;
		}

		parm->eClkMode = GSW_PORT_CLK_NA;

		if (parm->eMII_Mode == GSW_PORT_HW_RMII) {
			if (gswdev->gipver == LTQ_GSWIP_3_0) {
				gsw_r32(cdev, (GSWT_MII_CFG_1_RMII_OFFSET
					       + GSW30_TOP_OFFSET),
					GSWT_MII_CFG_1_RMII_SHIFT,
					GSWT_MII_CFG_1_RMII_SIZE, &value);
			} else {
				gsw_r32(cdev, (MII_CFG_0_RMII_OFFSET + (0x2 * pidx)
					       + GSW_TREG_OFFSET),
					MII_CFG_0_RMII_SHIFT,
					MII_CFG_0_RMII_SIZE, &value);
			}

			if (value == 1)
				parm->eClkMode = GSW_PORT_CLK_MASTER;
			else
				parm->eClkMode = GSW_PORT_CLK_SLAVE;
		}
	}

	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_misc);
#endif
	return ret;

}

GSW_return_t GSW_PortLinkCfgSet(void *cdev, GSW_portLinkCfg_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 data, phy_addr, phyreg, phy_ctrl = 0, duplex, PEN = 0, PACT = 0;
	u8 pidx = parm->nPortId;
	GSW_MDIO_data_t mddata;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_misc);
#endif

	if (IS_VRSN_NOT_31(gswdev->gipver)) {
		if (gswdev->gipver == LTQ_GSWIP_3_0) {
			if (gswdev->sdev == LTQ_FLOW_DEV_INT_R) {
				if (pidx == 15)
					pidx = 1;
				else {
					ret = GSW_statusErr;
					goto UNLOCK_AND_RETURN;
				}
			} else {
				if (!parm->nPortId || (parm->nPortId > (gswdev->pnum - 1))) {
					ret = GSW_statusErr;
					goto UNLOCK_AND_RETURN;
				}
			}
		} else {
			if (parm->nPortId > (gswdev->pnum - 1)) {
				ret = GSW_statusErr;
				goto UNLOCK_AND_RETURN;
			}
		}
	}

	if (IS_VRSN_31(gswdev->gipver)) {
#if defined(CONFIG_MAC) && CONFIG_MAC
		struct mac_ops *ops = get_mac_ops(gswdev, parm->nPortId);

		if (!ops) {
			pr_err("MAC %d is not initialized\n", parm->nPortId);
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}

		/* MAC API's to Set Duplex */
		if (parm->bDuplexForce == 1)
			ops->set_duplex(ops, parm->eDuplex);
		else
			ops->set_duplex(ops, GSW_DUPLEX_FULL);// Set to Full Duplex

		/* MAC API's to Set MII Interface */
		ops->set_mii_if(ops, parm->eMII_Mode);

		/* MAC API's to Set Speed */
		if (parm->bSpeedForce == 1)
			ops->set_speed(ops, parm->eSpeed);
		else
			ops->set_speed(ops, 0);// Set to Auto

		/* LPI Wait Time for 1G -- 50us */
		/* LPI Wait Time for 100M -- 21us */
		// TODO: Need to see the Wait time for XGMAC
		ops->set_lpi(ops, parm->bLPI, 0x32, 0x15);

		if (parm->bLinkForce == 1)
			ops->set_link_sts(ops, parm->eLink);
		else
			ops->set_link_sts(ops, GSW_PORT_LINK_UP);	// Default to UP

#endif
	} else {
		if (gswdev->gipver == LTQ_GSWIP_3_0) {
			gsw_r32(cdev, (GSWT_MDCCFG_0_PEN_0_OFFSET + GSW30_TOP_OFFSET),
				(GSWT_MDCCFG_0_PEN_0_SHIFT + pidx),	GSWT_MDCCFG_0_PEN_0_SIZE, &PEN);
			gsw_r32(cdev, ((GSWT_MDIO_STAT_1_PACT_OFFSET + ((pidx - 1) * 4)) + GSW30_TOP_OFFSET),
				GSWT_MDIO_STAT_1_PACT_SHIFT, GSWT_MDIO_STAT_1_PACT_SIZE, &PACT);
			gsw_r32(cdev, ((GSWT_PHY_ADDR_1_ADDR_OFFSET + ((pidx - 1) * 4)) + GSW30_TOP_OFFSET),
				0, 16, &phyreg);
		} else {
			gsw_r32(cdev, (MDC_CFG_0_PEN_0_OFFSET + GSW_TREG_OFFSET),
				(MDC_CFG_0_PEN_0_SHIFT + pidx), MDC_CFG_0_PEN_0_SIZE, &PEN);
			gsw_r32(cdev, (MDIO_STAT_0_PACT_OFFSET + GSW_TREG_OFFSET + pidx),
				MDIO_STAT_0_PACT_SHIFT, MDIO_STAT_0_PACT_SIZE, &PACT);
			gsw_r32(cdev, ((PHY_ADDR_0_ADDR_OFFSET - pidx) + GSW_TREG_OFFSET),
				0, 16, &phyreg);
		}

		/*pr_err("%s:%s:%d PEN:%d, PACT:%d,  phyreg:0x%08x\n",
			__FILE__, __func__, __LINE__,PEN, PACT,phyreg); */
		if ((PEN == 1) && (PACT == 1)) {
			if (parm->bDuplexForce == 1) {
				if (parm->eDuplex == GSW_DUPLEX_FULL) {
					data = DUPLEX_EN;
					duplex = DUPLEX_EN;
				} else {
					data = DUPLEX_DIS;
					duplex = DUPLEX_DIS;
				}
			} else {
				data = DUPLEX_AUTO;
				duplex = DUPLEX_AUTO;
			}

			data = 3; /*default value*/

			if (parm->bSpeedForce == 1) {
				switch (parm->eSpeed) {
				case GSW_PORT_SPEED_10:
					data = 0;

					if (duplex == DUPLEX_DIS)
						phy_ctrl = PHY_AN_ADV_10HDX;
					else
						phy_ctrl = PHY_AN_ADV_10FDX;

					break;

				case GSW_PORT_SPEED_100:
					data = 1;

					if (duplex == DUPLEX_DIS)
						phy_ctrl = PHY_AN_ADV_100HDX;
					else
						phy_ctrl = PHY_AN_ADV_100FDX;

					break;

				case GSW_PORT_SPEED_200:
					ret = GSW_statusErr;
					goto UNLOCK_AND_RETURN;

				case GSW_PORT_SPEED_1000:
					data = 2;

					if (duplex == DUPLEX_DIS)
						phy_ctrl = PHY_AN_ADV_1000HDX;
					else
						phy_ctrl = PHY_AN_ADV_1000FDX;

					break;

				default:
					break;
				}
			}

			if (gswdev->gipver == LTQ_GSWIP_3_0) {
				gsw_r32(cdev, ((GSWT_PHY_ADDR_1_ADDR_OFFSET
						+ ((pidx - 1) * 4)) + GSW30_TOP_OFFSET),
					GSWT_PHY_ADDR_1_ADDR_SHIFT,
					GSWT_PHY_ADDR_1_ADDR_SIZE, &phy_addr);
			} else {
				gsw_r32(cdev, ((PHY_ADDR_0_ADDR_OFFSET - pidx)
					       + GSW_TREG_OFFSET),
					PHY_ADDR_0_ADDR_SHIFT,
					PHY_ADDR_0_ADDR_SIZE, &phy_addr);
			}

			mddata.nAddressDev = phy_addr;
			GSW_MDIO_DataRead(cdev, &mddata);

			if ((data == 0) || (data == 1)) {
				mddata.nAddressReg = 4;
				GSW_MDIO_DataRead(cdev, &mddata);
				mddata.nData &= ~(PHY_AN_ADV_10HDX
						  | PHY_AN_ADV_10FDX
						  | PHY_AN_ADV_100HDX
						  | PHY_AN_ADV_100FDX);
				mddata.nData |= phy_ctrl;
				GSW_MDIO_DataWrite(cdev, &mddata);
				mddata.nAddressReg = 9;
				GSW_MDIO_DataRead(cdev, &mddata);
				mddata.nData &= ~(PHY_AN_ADV_1000HDX | PHY_AN_ADV_1000FDX);
				GSW_MDIO_DataWrite(cdev, &mddata);
			}

			if (data == 2) {
				mddata.nAddressReg = 9;
				GSW_MDIO_DataRead(cdev, &mddata);
				mddata.nData &= ~(PHY_AN_ADV_1000HDX | PHY_AN_ADV_1000FDX);
				mddata.nData |= phy_ctrl;
				GSW_MDIO_DataWrite(cdev, &mddata);
				mddata.nAddressReg = 4;
				GSW_MDIO_DataRead(cdev, &mddata);
				mddata.nData &= ~(PHY_AN_ADV_10HDX
						  | PHY_AN_ADV_10FDX
						  | PHY_AN_ADV_100HDX
						  | PHY_AN_ADV_100FDX);
				GSW_MDIO_DataWrite(cdev, &mddata);
			}

			if (data == 3) {
				mddata.nAddressReg = 4;
				GSW_MDIO_DataRead(cdev, &mddata);

				if (duplex == DUPLEX_DIS) {
					mddata.nData &= ~(PHY_AN_ADV_10HDX
							  | PHY_AN_ADV_10FDX
							  | PHY_AN_ADV_100HDX
							  | PHY_AN_ADV_100FDX);
					mddata.nData |= (PHY_AN_ADV_10HDX
							 | PHY_AN_ADV_100HDX);
				} else {
					mddata.nData |= (PHY_AN_ADV_10HDX
							 | PHY_AN_ADV_10FDX
							 | PHY_AN_ADV_100HDX
							 | PHY_AN_ADV_100FDX);
				}

				GSW_MDIO_DataWrite(cdev, &mddata);
				mddata.nAddressReg = 9;
				GSW_MDIO_DataRead(cdev, &mddata);

				if (duplex == DUPLEX_DIS) {
					mddata.nData &= ~(PHY_AN_ADV_1000HDX
							  | PHY_AN_ADV_1000FDX);
					mddata.nData |= (PHY_AN_ADV_1000HDX);
				} else {
					mddata.nData |= (PHY_AN_ADV_1000HDX
							 | PHY_AN_ADV_1000FDX);
				}

				GSW_MDIO_DataWrite(cdev, &mddata);
			}

			if (gswdev->gipver == LTQ_GSWIP_3_0) {
				gsw_w32(cdev, (MAC_CTRL_4_LPIEN_OFFSET + (0xC * (pidx - 1))),
					MAC_CTRL_4_LPIEN_SHIFT,
					MAC_CTRL_4_LPIEN_SIZE, parm->bLPI);
				/* LPI Wait Time for 1G -- 50us */
				gsw_w32(cdev, (MAC_CTRL_4_GWAIT_OFFSET + (0xC * (pidx - 1))),
					MAC_CTRL_4_GWAIT_SHIFT,
					MAC_CTRL_4_GWAIT_SIZE, 0x32);
				/* LPI Wait Time for 100M -- 21us */
				gsw_w32(cdev, (MAC_CTRL_4_WAIT_OFFSET + (0xC * (pidx - 1))),
					MAC_CTRL_4_WAIT_SHIFT,
					MAC_CTRL_4_WAIT_SIZE, 0x15);
			} else {
				gsw_w32(cdev, (MAC_CTRL_4_LPIEN_OFFSET + (0xC * pidx)),
					MAC_CTRL_4_LPIEN_SHIFT,
					MAC_CTRL_4_LPIEN_SIZE, parm->bLPI);
				/* LPI Wait Time for 1G -- 50us */
				gsw_w32(cdev, (MAC_CTRL_4_GWAIT_OFFSET + (0xC * pidx)),
					MAC_CTRL_4_GWAIT_SHIFT,
					MAC_CTRL_4_GWAIT_SIZE, 0x32);
				/* LPI Wait Time for 100M -- 21us */
				gsw_w32(cdev, (MAC_CTRL_4_WAIT_OFFSET + (0xC * pidx)),
					MAC_CTRL_4_WAIT_SHIFT,
					MAC_CTRL_4_WAIT_SIZE, 0x15);
			}

			/* LPI request controlled by data available for  port */
			gsw_w32(cdev, (FDMA_CTRL_LPI_MODE_OFFSET),
				FDMA_CTRL_LPI_MODE_SHIFT,
				FDMA_CTRL_LPI_MODE_SIZE, 0x4);
			mddata.nAddressDev = phy_addr;
			mddata.nAddressReg = 0x3C;

			if (parm->bLPI == 1) {
				mddata.nData = 0x6;
				GSW_MMD_MDIO_DataWrite(cdev, &mddata, pidx, 0x07);
			} else {
				mddata.nData = 0x0;
				GSW_MMD_MDIO_DataWrite(cdev, &mddata, pidx, 0x07);
			}

			mddata.nAddressDev = phy_addr;
			mddata.nAddressReg = 0;
			GSW_MDIO_DataRead(cdev, &mddata);
			mddata.nData = 0x1200;
			data = 0;

			if (parm->bLinkForce == 1) {
				if (parm->eLink == GSW_PORT_LINK_UP) {
					data = 1;
				} else {
					data = 2;
					mddata.nData = 0x800;
				}
			}

			GSW_MDIO_DataWrite(cdev, &mddata);
		} else {
			if (parm->bDuplexForce == 1) {
				phyreg &= ~(3 << 9);

				if (parm->eDuplex == GSW_DUPLEX_FULL) {
					phyreg |= (1 << 9);
				} else {
					phyreg |= (3 << 9);
				}
			}

			if (parm->bLinkForce == 1) {
				phyreg &= ~(3 << 13);

				if (parm->eLink == GSW_PORT_LINK_UP) {
					phyreg |= (1 << 13);
				} else {
					phyreg |= (2 << 13);
				}
			}

			if (parm->bSpeedForce == 1) {
				phyreg &= ~(3 << 11);

				switch (parm->eSpeed) {
				case GSW_PORT_SPEED_10:
					break;

				case GSW_PORT_SPEED_100:
					phyreg |= (1 << 11);
					break;

				case GSW_PORT_SPEED_200:
					ret = GSW_statusErr;
					goto UNLOCK_AND_RETURN;

				case GSW_PORT_SPEED_1000:
					phyreg |= (2 << 11);
					break;

				default:
					break;
				}
			}

			/*		pr_err("%s:%s:%d PEN:%d, PACT:%d,  phyreg:0x%08x\n",
						__FILE__, __func__, __LINE__,PEN, PACT,phyreg);*/
			if (gswdev->gipver == LTQ_GSWIP_3_0) {
				gsw_w32(cdev, ((GSWT_PHY_ADDR_1_ADDR_OFFSET + ((parm->nPortId - 1) * 4)) + GSW30_TOP_OFFSET),
					0, 16, phyreg);
			} else {
				gsw_w32(cdev, ((PHY_ADDR_0_ADDR_OFFSET - pidx) + GSW_TREG_OFFSET),
					0, 16, phyreg);
			}
		}

		data = 4; /*default mode */

		switch (parm->eMII_Mode) {
		case GSW_PORT_HW_MII:
			data = 1;

			if (parm->eMII_Type == GSW_PORT_PHY)
				data = 0;

			break;

		case GSW_PORT_HW_RMII:
			data = 3;

			if (parm->eMII_Type == GSW_PORT_PHY)
				data = 2;

			break;

		case GSW_PORT_HW_RGMII:
			data = 4;
			break;

		case GSW_PORT_HW_GMII:
			/*	data = 1; */
			break;

		default:
			break;
		}

		if (!(parm->eMII_Mode == GSW_PORT_HW_GMII)) {
			if (gswdev->gipver == LTQ_GSWIP_3_0) {
				gsw_w32(cdev, (GSWT_MII_CFG_1_MIIMODE_OFFSET
					       + GSW30_TOP_OFFSET),
					GSWT_MII_CFG_1_MIIMODE_SHIFT,
					GSWT_MII_CFG_1_MIIMODE_SIZE, data);
			} else {
				gsw_w32(cdev, (MII_CFG_0_MIIMODE_OFFSET + (0x2 * pidx)
					       + GSW_TREG_OFFSET),
					MII_CFG_0_MIIMODE_SHIFT,
					MII_CFG_0_MIIMODE_SIZE, data);
			}
		}

		data = 0;

		if (parm->eMII_Mode == GSW_PORT_HW_RMII) {
			if (parm->eClkMode == GSW_PORT_CLK_MASTER)
				data = 1;

			if (gswdev->gipver == LTQ_GSWIP_3_0) {
				gsw_w32(cdev, (GSWT_MII_CFG_1_RMII_OFFSET
					       + GSW30_TOP_OFFSET),
					GSWT_MII_CFG_1_RMII_SHIFT,
					GSWT_MII_CFG_1_RMII_SIZE, data);
			} else {
				gsw_w32(cdev, (MII_CFG_0_RMII_OFFSET + (0x2 * pidx)
					       + GSW_TREG_OFFSET),
					MII_CFG_0_RMII_SHIFT,
					MII_CFG_0_RMII_SIZE, data);
			}
		}
	}

	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_misc);
#endif
	return ret;
}

GSW_return_t GSW_PortPHY_AddrGet(void *cdev, GSW_portPHY_Addr_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u8 pidx = parm->nPortId, num_ports;
	u32 value;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_misc);
#endif

	if (gswdev->gipver == LTQ_GSWIP_3_0) {
		if (!parm->nPortId) {
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}

		if (gswdev->sdev == LTQ_FLOW_DEV_INT_R) {
			num_ports = gswdev->tpnum;

			if (parm->nPortId == 15)
				pidx = 1;
			else {
				ret = GSW_statusErr;
				goto UNLOCK_AND_RETURN;
			}
		} else
			num_ports = (gswdev->pnum - 1);

		if (parm->nPortId > (num_ports)) {
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}

		gsw_r32(cdev, ((GSWT_PHY_ADDR_1_ADDR_OFFSET
				+ ((pidx - 1) * 4)) + GSW30_TOP_OFFSET),
			GSWT_PHY_ADDR_1_ADDR_SHIFT,
			GSWT_PHY_ADDR_1_ADDR_SIZE, &value);
	} else {
		if (parm->nPortId >= (gswdev->pnum - 1)) {
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}

		gsw_r32(cdev, ((PHY_ADDR_0_ADDR_OFFSET - pidx)
			       + GSW_TREG_OFFSET),
			PHY_ADDR_0_ADDR_SHIFT,
			PHY_ADDR_0_ADDR_SIZE, &value);
	}

	parm->nAddressDev = value;
	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_misc);
#endif
	return ret;

}

GSW_return_t GSW_PortPHY_Query(void *cdev, GSW_portPHY_Query_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u8 pidx = parm->nPortId;
	u32 value;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_misc);
#endif

	if (gswdev->gipver == LTQ_GSWIP_3_0) {
		if (!parm->nPortId || (parm->nPortId > (gswdev->pnum))) {
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}
	} else {
		if (parm->nPortId >= (gswdev->pnum - 1)) {
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}
	}

	gsw_r32(cdev, (MAC_PSTAT_PACT_OFFSET + (0xC * pidx)),
		MAC_PSTAT_PACT_SHIFT, MAC_PSTAT_PACT_SIZE, &value);
	parm->bPHY_Present = value;
	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_misc);
#endif
	return ret;

}

GSW_return_t GSW_PortRGMII_ClkCfgGet(void *cdev, GSW_portRGMII_ClkCfg_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 value;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_misc);
#endif

	if (gswdev->gipver == LTQ_GSWIP_3_0) {
		if (!parm->nPortId || (parm->nPortId > (gswdev->pnum))) {
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}
	} else {
		if (parm->nPortId >= (gswdev->pnum - 1)) {
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}
	}

	if (gswdev->gipver == LTQ_GSWIP_3_0) {
		gsw_r32(cdev, GSWT_PCDU_1_RXDLY_OFFSET
			+ GSW30_TOP_OFFSET,
			GSWT_PCDU_1_RXDLY_SHIFT,
			GSWT_PCDU_1_RXDLY_SIZE, &value);
		parm->nDelayRx = value;
		gsw_r32(cdev, GSWT_PCDU_1_TXDLY_OFFSET
			+ GSW30_TOP_OFFSET,
			GSWT_PCDU_1_TXDLY_SHIFT,
			GSWT_PCDU_1_TXDLY_SIZE, &value);
		parm->nDelayTx = value;
	} else {
		gsw_r32(cdev, PCDU_0_RXDLY_OFFSET + (0x2 * parm->nPortId)
			+ GSW_TREG_OFFSET,
			PCDU_0_RXDLY_SHIFT,
			PCDU_0_RXDLY_SIZE, &value);
		parm->nDelayRx = value;
		gsw_r32(cdev, PCDU_0_TXDLY_OFFSET + (0x2 * parm->nPortId)
			+ GSW_TREG_OFFSET,
			PCDU_0_TXDLY_SHIFT,
			PCDU_0_TXDLY_SIZE, &value);
		parm->nDelayTx = value;
	}

	ret = GSW_statusOk;


UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_misc);
#endif
	return ret;
}

GSW_return_t GSW_PortRGMII_ClkCfgSet(void *cdev, GSW_portRGMII_ClkCfg_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_misc);
#endif

	if (gswdev->gipver == LTQ_GSWIP_3_0) {
		if (!parm->nPortId || (parm->nPortId > (gswdev->pnum))) {
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}
	} else {
		if (parm->nPortId >= (gswdev->pnum - 1)) {
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}
	}

	if (gswdev->gipver == LTQ_GSWIP_3_0) {
		gsw_w32(cdev, (GSWT_PCDU_1_RXDLY_OFFSET
			       + GSW30_TOP_OFFSET),
			GSWT_PCDU_1_RXDLY_SHIFT,
			GSWT_PCDU_1_RXDLY_SIZE, parm->nDelayRx);
		gsw_w32(cdev, (GSWT_PCDU_1_TXDLY_OFFSET
			       + GSW30_TOP_OFFSET),
			GSWT_PCDU_1_TXDLY_SHIFT,
			GSWT_PCDU_1_TXDLY_SIZE, parm->nDelayTx);
	} else {
		gsw_w32(cdev, (PCDU_0_RXDLY_OFFSET + (0x2 * parm->nPortId)
			       + GSW_TREG_OFFSET),
			PCDU_0_RXDLY_SHIFT,
			PCDU_0_RXDLY_SIZE, parm->nDelayRx);
		gsw_w32(cdev, (PCDU_0_TXDLY_OFFSET + (0x2 * parm->nPortId)
			       + GSW_TREG_OFFSET),
			PCDU_0_TXDLY_SHIFT,
			PCDU_0_TXDLY_SIZE, parm->nDelayTx);
	}

	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_misc);
#endif
	return ret;
}

GSW_return_t GSW_PortRedirectGet(void *cdev, GSW_portRedirectCfg_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u8 pidx = parm->nPortId;
	u32 value;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_misc);
#endif

	if (parm->nPortId >= gswdev->tpnum) {
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	gsw_r32(cdev, (PCE_PCTRL_3_EDIR_OFFSET + (0xA * pidx)),
		PCE_PCTRL_3_EDIR_SHIFT,
		PCE_PCTRL_3_EDIR_SIZE, &value);
	parm->bRedirectEgress = value;

	if (prdflag > 0)
		parm->bRedirectIngress = 1;
	else
		parm->bRedirectIngress = 0;

	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_misc);
#endif
	return ret;
}

GSW_return_t GSW_PortRedirectSet(void *cdev, GSW_portRedirectCfg_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	static GSW_PCE_rule_t pcrule;
	u8 pidx = parm->nPortId;
	u32 ret;
	u32 rdport = 0, value = 0, i;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_pce);
#endif

	if (parm->nPortId >= gswdev->tpnum) {
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	value |= (1 << gswdev->mpnum);
	rdport = value;
	gsw_w32(cdev, PCE_PMAP_1_MPMAP_OFFSET, PCE_PMAP_1_MPMAP_SHIFT,
		PCE_PMAP_1_MPMAP_SIZE, value);
	value = parm->bRedirectEgress;
	gsw_w32(cdev, (PCE_PCTRL_3_EDIR_OFFSET + (0xA * pidx)),
		PCE_PCTRL_3_EDIR_SHIFT, PCE_PCTRL_3_EDIR_SIZE, value);

	if (parm->bRedirectIngress == 1)
		prdflag |= (1 << parm->nPortId);
	else
		prdflag &= ~(1 << parm->nPortId);

	for (i = 0; i < gswdev->pnum; i++) {
		if (((prdflag >> i) & 0x1) == 1) {
			memset(&pcrule, 0, sizeof(GSW_PCE_rule_t));
			pcrule.pattern.nIndex = (PRD_PRULE_INDEX + i);
			pcrule.pattern.bEnable = 1;
			pcrule.pattern.bPortIdEnable = 1;
			pcrule.pattern.nPortId = i;

			if (parm->bRedirectIngress == 1)
				pcrule.action.ePortMapAction =
					GSW_PCE_ACTION_PORTMAP_ALTERNATIVE;

			pcrule.action.nForwardPortMap[0] = rdport;

			/* We prepare everything and write into PCE Table */
			if (0 != pce_rule_write(cdev,
						&gswdev->phandler, &pcrule)) {
				ret = GSW_statusErr;
				goto UNLOCK_AND_RETURN;
			}
		}  else {
			if (0 != pce_pattern_delete(cdev,
						    &gswdev->phandler, PRD_PRULE_INDEX + i)) {
				ret = GSW_statusErr;
				goto UNLOCK_AND_RETURN;
			}
		}
	}

	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_pce);
#endif
	return ret;

}

GSW_return_t GSW_RMON_Clear(void *cdev, GSW_RMON_clear_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u8 index, num_ports, pidx = parm->nRmonId;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_bm);
#endif

	if ((gswdev->sdev == LTQ_FLOW_DEV_INT_R) &&
	    (gswdev->gipver == LTQ_GSWIP_3_0)) {
		/*if gswip 3.0: tpnum = physical port + virutual port*/
		num_ports = gswdev->tpnum;
	} else if (IS_VRSN_31(gswdev->gipver)) {
		/*if gswpi 3.1: tpnum = physical port + virutual port*/
		num_ports = gswdev->tpnum;
	} else {
		/*pnum is logical port: pnum = physical port*/
		num_ports = gswdev->pnum;
	}

	switch (parm->eRmonType) {
	case GSW_RMON_ALL_TYPE:
		/* NOTE:Software Reset for All Interface RMON RAM in case of 3.0*/
		/* NOTE:Software Reset for All Bridge Port RMON RAM in case of 3.1*/
		gsw_w32(cdev, BM_RMON_GCTRL_ALLITF_RES_OFFSET,
			BM_RMON_GCTRL_ALLITF_RES_SHIFT,
			BM_RMON_GCTRL_ALLITF_RES_SIZE, 0x1);

		/* Software Reset for PMAC RMON RAM*/
		gsw_w32(cdev, BM_RMON_GCTRL_PMAC_RES_OFFSET,
			BM_RMON_GCTRL_PMAC_RES_SHIFT,
			BM_RMON_GCTRL_PMAC_RES_SIZE, 0x1);
		/*  Software Reset for Meter RMON RAM */
		gsw_w32(cdev, BM_RMON_GCTRL_METER_RES_OFFSET,
			BM_RMON_GCTRL_METER_RES_SHIFT,
			BM_RMON_GCTRL_METER_RES_SIZE, 0x1);

		/*Not Applicable for GSWIP 3.1*/
		if (IS_VRSN_NOT_31(gswdev->gipver)) {
			/*  Software Reset for Redirection RMON RAM */
			gsw_w32(cdev, BM_RMON_GCTRL_RED_RES_OFFSET,
				BM_RMON_GCTRL_RED_RES_SHIFT,
				BM_RMON_GCTRL_RED_RES_SIZE, 0x1);
		}

		/* Reset all port based RMON counter */
		for (index = 0; index < num_ports; index++) {
			pr_debug("Rmon clear for CTP RX/TX --Logical Port %u\n", index);
			gsw_w32(cdev,
				BM_RMON_CTRL_RAM1_RES_OFFSET + (index * 2),
				BM_RMON_CTRL_RAM1_RES_SHIFT,
				BM_RMON_CTRL_RAM1_RES_SIZE, 0x1);
			gsw_w32(cdev,
				BM_RMON_CTRL_RAM2_RES_OFFSET + (index * 2),
				BM_RMON_CTRL_RAM2_RES_SHIFT,
				BM_RMON_CTRL_RAM2_RES_SIZE, 0x1);

			/*Not Applicable for GSWIP 3.1*/
			if (IS_VRSN_NOT_31(gswdev->gipver)) {
				/*  Software Reset for Routing RMON RAM */
				gsw_w32(cdev,
					BM_RMON_CTRL_ROUT_RES_OFFSET + (index * 2),
					BM_RMON_CTRL_ROUT_RES_SHIFT,
					BM_RMON_CTRL_ROUT_RES_SIZE, 0x1);
			}
		}

		break;

	case GSW_RMON_PMAC_TYPE:
		/* Software Reset for PMAC RMON RAM*/
		gsw_w32(cdev, BM_RMON_GCTRL_PMAC_RES_OFFSET,
			BM_RMON_GCTRL_PMAC_RES_SHIFT,
			BM_RMON_GCTRL_PMAC_RES_SIZE, 0x1);
		break;

	case GSW_RMON_PORT_TYPE:
	case GSW_RMON_CTP_TYPE:
		if (pidx >= num_ports) {
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}

		/* Reset all RMON counter */
		/* Incase of GSWIP 3.1 Reset CTP pidx RX/TX counter */
		gsw_w32(cdev,
			BM_RMON_CTRL_RAM1_RES_OFFSET + (pidx * 2),
			BM_RMON_CTRL_RAM1_RES_SHIFT,
			BM_RMON_CTRL_RAM1_RES_SIZE, 0x1);
		gsw_w32(cdev,
			BM_RMON_CTRL_RAM2_RES_OFFSET + (pidx * 2),
			BM_RMON_CTRL_RAM2_RES_SHIFT,
			BM_RMON_CTRL_RAM2_RES_SIZE, 0x1);
		break;

	case GSW_RMON_METER_TYPE:
		/*  Software Reset for Meter RMON RAM */
		gsw_w32(cdev, BM_RMON_GCTRL_METER_RES_OFFSET,
			BM_RMON_GCTRL_METER_RES_SHIFT,
			BM_RMON_GCTRL_METER_RES_SIZE, 0x1);
		break;

	case GSW_RMON_IF_TYPE:
	case GSW_RMON_BRIDGE_TYPE:
		/* Interface ID to be Reset 3.0 */
		/* Brideg Port ID to be Reset 3.1 */
		gsw_w32(cdev, BM_RMON_GCTRL_ITFID_OFFSET,
			BM_RMON_GCTRL_ITFID_SHIFT,
			BM_RMON_GCTRL_ITFID_SIZE, pidx);
		/*  Software Reset for a Single Interface RMON RAM */
		gsw_w32(cdev, BM_RMON_GCTRL_INT_RES_OFFSET,
			BM_RMON_GCTRL_INT_RES_SHIFT,
			BM_RMON_GCTRL_INT_RES_SIZE, 0x1);
		break;

	case GSW_RMON_ROUTE_TYPE:
		if (IS_VRSN_31(gswdev->gipver)) {
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}

		/*  Software Reset for Routing RMON RAM */
		gsw_w32(cdev,
			BM_RMON_CTRL_ROUT_RES_OFFSET + (pidx * 2),
			BM_RMON_CTRL_ROUT_RES_SHIFT,
			BM_RMON_CTRL_ROUT_RES_SIZE, 0x1);
		break;

	case GSW_RMON_REDIRECT_TYPE:
		if (IS_VRSN_31(gswdev->gipver)) {
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}

		/*  Software Reset for Redirection RMON RAM */
		gsw_w32(cdev, BM_RMON_GCTRL_RED_RES_OFFSET,
			BM_RMON_GCTRL_RED_RES_SHIFT,
			BM_RMON_GCTRL_RED_RES_SIZE, 0x1);
		break;

	default:
		break;
	}

	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_bm);
#endif
	return ret;
}

static GSW_return_t GSW_RMON_Port_Get_Active(void *cdev, GSW_RMON_Port_cnt_t *parm)
{
	u8 RmonCntOffset = 0;
	u32 data = 0, data1 = 0;
	u32 r_frame = 0, r_unicast = 0, r_multicast = 0,
	    t_frame = 0, t_unicast = 0, t_multicast = 0;
	u32 rgbcl = 0, rbbcl = 0, tgbcl = 0;
	u64 rgbch = 0, rbbch = 0, tgbch = 0;
	u32 Portmode = 0, PortId = 0;
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 count = 0;
	bmtbl_prog_t bmtable = {0};
	u32 ret;
	GSW_CTP_portAssignment_t ctp_get;
	u32 subifidgroup;
	ltq_bool_t pcebypass;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

	Portmode = parm->ePortType;
	PortId = parm->nPortId;
	subifidgroup = parm->nSubIfIdGroup;
	pcebypass = parm->bPceBypass;
	/*Clear the structure before using*/
	memset(parm, 0, sizeof(GSW_RMON_Port_cnt_t));
	/* recover input parameters */
	parm->ePortType = Portmode;
	parm->nPortId = PortId;
	parm->nSubIfIdGroup = subifidgroup;
	parm->bPceBypass = pcebypass;

	switch (Portmode) {
	case GSW_CTP_PORT:

		/*check based on number of logical port*/
		if (PortId > gswdev->tpnum) {
			pr_err("PortId %d >= gswdev->num_of_ctp %d\n", PortId, gswdev->num_of_ctp);
			return GSW_statusErr;
		}

		ctp_get.nLogicalPortId = PortId;
		ret = GSW_CTP_PortAssignmentGet(cdev, &ctp_get);

		if (ret == GSW_statusErr) {
			pr_err("GSW_CTP_PortAssignmentGet returns error\n");
			return ret;
		}

		PortId = ctp_get.nFirstCtpPortId + parm->nSubIfIdGroup;

		if (PortId > gswdev->num_of_ctp) {
			pr_err("PortId %d > gswdev->num_of_ctp\n", PortId);
			return GSW_statusErr;
		}

		if (PortId < ctp_get.nFirstCtpPortId ||
		    PortId >= (ctp_get.nFirstCtpPortId + ctp_get.nNumberOfCtpPort)) {
			pr_err("ERROR :CTP port not with in Allocated range\n");
			pr_err("Check CTP assignment Get\n");
			return GSW_statusErr;
		}

		break;

	case GSW_BRIDGE_PORT:
		if (PortId >= gswdev->num_of_bridge_port) {
			pr_err("PortId %d >= gswdev->num_of_ctp %d\n", PortId, gswdev->num_of_bridge_port);
			return GSW_statusErr;
		}

		break;

	default:
		pr_err("ePortType Not Supported :ERROR (ePortType=%d)\n", Portmode);
		return GSW_statusErr;
	}

	/*GSW_ENABLE should be called before calling RMON_GET
	  In GSW_ENABLE RMON Counter for 0- 15 logical port are enabled*/

	/*Only for RX, As per SAS counter offset 0 - 21 is
	  shared between RX and TX.
	  Depends upon ePortType selection*/

	//One-time populate.
	bmtable.adr.rmon.portOffset = PortId;

	if (Portmode == GSW_CTP_PORT) {
		bmtable.tableID = (BM_Table_ID)GSW_RMON_CTP_PORT_RX;
	} else {
		bmtable.tableID = (BM_Table_ID)GSW_RMON_BRIDGE_PORT_RX;
	}

	bmtable.b64bitMode = 1;
	bmtable.numValues = 4;
	count = RMON_COUNTER_OFFSET_GSWIP3_1;

	for (RmonCntOffset = 0; (RmonCntOffset < count) && !parm->bPceBypass; RmonCntOffset++) {
		bmtable.adr.rmon.counterOffset = RmonCntOffset;
		gsw_bm_table_read(cdev, &bmtable);
		data = (bmtable.value[1] << 16) | (bmtable.value[0] & 0xFFFF);
		data1 = (bmtable.value[3] << 16) | (bmtable.value[2] & 0xFFFF);

		switch (RmonCntOffset) {
		case 0x00: /*RX Size 64 frame count*/
			parm->nRx64BytePkts = data;
			parm->nRx127BytePkts = data1; /* Receive Size 65-127 Frame Count */
			break;

		case 0x02: /* Receive Size 128-255 Frame Count */
			parm->nRx255BytePkts = data;
			parm->nRx511BytePkts = data1; /* Receive Size 256-511 Frame Count */
			break;

		case 0x04: /* Receive Size 512-1023 Frame Count */
			parm->nRx1023BytePkts = data;
			parm->nRxMaxBytePkts = data1; /* Receive Size 1024 - 1518 Frame Count */
			break;

		case 0x06: /* Receive Unicast Frame Count */
			parm->nRxUnicastPkts = r_unicast = data;
			parm->nRxMulticastPkts = r_multicast = data1; /* Receive Multicast Frame Count1 */
			break;

		case 0x08: /* Receive Undersize Good Count */
			parm->nRxUnderSizeGoodPkts = data; /* Less than 64 byes. */
			parm->nRxOversizeGoodPkts = data1; /* Receive Oversize (> 1518) Good Count */
			break;

		case 0x0A: /* Receive Good Byte Count (Low) */
			rgbcl = data;
			rgbch = data1; /* Receive Good Byte Count (High) */
			break;

		case 0x0C: /* Receive Frme Count */
			parm->nRxBroadcastPkts = r_frame = data;
			parm->nRxFilteredPkts = data1; /* Receive Drop (Filter) Frame Count */
			break;

		case 0x0E: /* Receive Extended Vlan Discard Frame Count */
			parm->nRxExtendedVlanDiscardPkts = data;

			/*only applicable for CTP RX*/
			if (Portmode == GSW_RMON_CTP_PORT_RX)
				parm->nRxFCSErrorPkts = data1; /* Receive MAC/FCS Error frame Count */

			break;

		case 0x10: /* Receive Undersize Bad Count */

			/*only applicable for CTP RX*/
			if (Portmode == GSW_RMON_CTP_PORT_RX)
				parm->nRxUnderSizeErrorPkts = data;

			/*only applicable for CTP RX*/
			if (Portmode == GSW_RMON_CTP_PORT_RX)
				parm->nRxOversizeErrorPkts = data1; /* Receive Oversize Bad Count */

			break;

		case 0x12: /* MTU Exceed Discard Count */

			/*only applicable for CTP RX*/
			if (Portmode == GSW_RMON_CTP_PORT_RX)
				parm->nMtuExceedDiscardPkts = data;

			/*only applicable for CTP RX*/
			if (Portmode == GSW_RMON_CTP_PORT_RX)
				parm->nRxDroppedPkts = data1; /* Receive Discard (Tail-Drop) Frame Count */

			break;

		case 0x14: /* Receive Bad Byte Count (Low) */

			/*only applicable for CTP RX*/
			if (Portmode == GSW_RMON_CTP_PORT_RX)
				rbbcl = data;

			/*only applicable for CTP RX*/
			if (Portmode == GSW_RMON_CTP_PORT_RX)
				rbbch = data1; /* Receive Bad Byte Count (High) */

			break;
		}

		RmonCntOffset = RmonCntOffset + 1;
	}


	if ((Portmode == GSW_CTP_PORT ||
	     Portmode == GSW_BRIDGE_PORT) &&
	    !parm->bPceBypass) {
		/* Receive Good Byte Count */
		parm->nRxGoodBytes = (u64)(((rgbch & 0xFFFFFFFF) << 32) | (rgbcl & 0xFFFFFFFF));

		if (Portmode == GSW_CTP_PORT) {
			/* Receive Bad Byte Count */
			parm->nRxBadBytes = (u64)(((rbbch & 0xFFFFFFFF) << 32) | (rbbcl & 0xFFFFFFFF));
		}

		parm->nRxGoodPkts = r_frame + r_unicast + r_multicast;
	}


	/*Only for TX, As per SAS counter offset 0 - 13 is
	  shared between RX and TX.
	  Depends upon ePortType selection*/

	//One-time populate.
	bmtable.adr.rmon.portOffset = PortId;

	if (Portmode == GSW_CTP_PORT) {
		if (parm->bPceBypass)
			bmtable.tableID = (BM_Table_ID)GSW_RMON_CTP_PORT_PCE_BYPASS;
		else
			bmtable.tableID = (BM_Table_ID)GSW_RMON_CTP_PORT_TX;
	} else {
		bmtable.tableID = (BM_Table_ID)GSW_RMON_BRIDGE_PORT_TX;
	}

	bmtable.b64bitMode = 1;
	bmtable.numValues = 4;
	count = 14;

	for (RmonCntOffset = 0; RmonCntOffset < count; RmonCntOffset++) {

		bmtable.adr.rmon.counterOffset = RmonCntOffset;
		gsw_bm_table_read(cdev, &bmtable);
		data = (bmtable.value[1] << 16) | (bmtable.value[0] & 0xFFFF);
		data1 = (bmtable.value[3] << 16) | (bmtable.value[2] & 0xFFFF);

		switch (RmonCntOffset) {
		case 0x00: /* Transmit Size 64 Frame Count */
			parm->nTx64BytePkts = data;
			parm->nTx127BytePkts = data1; /* Transmit Size 65-127 Frame Count */
			break;

		case 0x02: /* Transmit Size 128-255 Frame Count */
			parm->nTx255BytePkts = data;
			parm->nTx511BytePkts = data1; /* Transmit Size 256-511 Frame Count */
			break;

		case 0x04: /* Transmit Size 512-1023 Frame Count */
			parm->nTx1023BytePkts = data;
			parm->nTxMaxBytePkts = data1; /* Transmit Size 1024 - 1518 Frame Count */
			break;

		case 0x06:  /* Transmit Unicast Frame Count */
			parm->nTxUnicastPkts = t_unicast = data;
			parm->nTxMulticastPkts = t_multicast = data1; /* Transmit Multicast Frame Count1 */
			break;

		case 0x08: /* Transmit Undersize Good Count */
			parm->nTxUnderSizeGoodPkts = data; /* Less than 64 byes */
			parm->nTxOversizeGoodPkts = data1; /* Transmit Oversize (> 1518) Good Count */
			break;

		case 0x0A: /* Transmit Good Byte Count (Low) */
			tgbcl = data;
			tgbch = data1; /* Transmit Good Byte Count (High) */
			break;

		case 0x0C: /* Transmit BroadCast Count */
			parm->nTxBroadcastPkts = t_frame = data;
			parm->nTxAcmDroppedPkts = data1; /* Egress Queue Discard,(Active Congestion Management) frame count.*/
			break;
		}

		RmonCntOffset = RmonCntOffset + 1;
	}

	parm->nTxGoodPkts = t_frame + t_unicast + t_multicast;
	/* Transmit Good Byte Count */
	parm->nTxGoodBytes = (u64)(((tgbch & 0xFFFFFFFF) << 32) | (tgbcl & 0xFFFFFFFF));

	return GSW_statusOk;
}

static GSW_return_t GSW_RMON_Port_Get_Legacy(void *cdev, GSW_RMON_Port_cnt_t *parm)
{
	u8 pidx, i;
	u32 data, value, bcast_cnt;
	u32 r_frame = 0, r_unicast = 0, r_multicast = 0,
	    t_frame = 0, t_unicast = 0, t_multicast = 0;
	u32 rgbcl = 0, rbbcl = 0, tgbcl = 0;
	u64 rgbch = 0, rbbch = 0, tgbch = 0;
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	bmtbl_prog_t bmtable = {0};

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

	pidx = parm->nPortId;

	if (gswdev) {
		if (gswdev->gipver == LTQ_GSWIP_3_0) {
			if (parm->nPortId >= gswdev->tpnum)
				return GSW_statusErr;
		} else {
			if (parm->nPortId >= gswdev->pnum)
				return GSW_statusErr;
		}
	}

	gsw_w32(cdev, BM_PCFG_CNTEN_OFFSET + (pidx * 2),
		BM_PCFG_CNTEN_SHIFT, BM_PCFG_CNTEN_SIZE, 1);
	memset(parm, 0, sizeof(GSW_RMON_Port_cnt_t));
	parm->nPortId = pidx;
	gsw_r32(cdev, BM_RMON_CTRL_BCAST_CNT_OFFSET + (pidx * 2),
		BM_RMON_CTRL_BCAST_CNT_SHIFT,
		BM_RMON_CTRL_BCAST_CNT_SIZE, &bcast_cnt);

	for (i = 0; i < RMON_COUNTER_OFFSET; i++) {
		bmtable.adr.raw = (u16)i;

		if (gswdev->gipver == LTQ_GSWIP_3_0) {
			value = (pidx >= 8) ? (pidx + 8) : parm->nPortId;
			bmtable.tableID = (BM_Table_ID)value;
		} else {
			bmtable.tableID = (BM_Table_ID)pidx;
		}

		bmtable.numValues = 2;
		gsw_bm_table_read(cdev, &bmtable);
		data = (bmtable.value[1] << 16) | bmtable.value[0];

		switch (i) {
		case 0x1F: /* Receive Frme Count */
			if (bcast_cnt == 1)
				parm->nRxBroadcastPkts = data;
			else
				parm->nRxGoodPkts = data;

			r_frame = data;
			break;

		case 0x23: /* Receive Unicast Frame Count */
			parm->nRxUnicastPkts = data;
			r_unicast = data;
			break;

		case 0x22: /* Receive Multicast Frame Count1 */
			parm->nRxMulticastPkts = data;
			r_multicast = data;
			break;

		case 0x21: /* Receive CRC Errors Count */
			parm->nRxFCSErrorPkts = data;
			break;

		case 0x1D: /* Receive Undersize Good Count */
			parm->nRxUnderSizeGoodPkts = data;
			break;

		case 0x1B: /* Receive Oversize Good Count */
			parm->nRxOversizeGoodPkts = data;
			break;

		case 0x1E: /* Receive Undersize Bad Count */
			parm->nRxUnderSizeErrorPkts = data;
			break;

		case 0x20: /* Receive Pause Good Count */
			parm->nRxGoodPausePkts = data;
			break;

		case 0x1C: /* Receive Oversize Bad Count */
			parm->nRxOversizeErrorPkts = data;
			break;

		case 0x1A: /* Receive Alignment Errors Count */
			parm->nRxAlignErrorPkts = data;
			break;

		case 0x12: /* Receive Size 64 Frame Count1 */
			parm->nRx64BytePkts = data;
			break;

		case 0x13: /* Receive Size 65-127 Frame Count */
			parm->nRx127BytePkts = data;
			break;

		case 0x14: /* Receive Size 128-255 Frame Count */
			parm->nRx255BytePkts = data;
			break;

		case 0x15: /* Receive Size 256-511 Frame Count */
			parm->nRx511BytePkts = data;
			break;

		case 0x16: /* Receive Size 512-1023 Frame Count */
			parm->nRx1023BytePkts = data;
			break;

		case 0x17: /* Receive Size Greater 1023 Frame Count */
			parm->nRxMaxBytePkts = data;
			break;

		case 0x18: /* Receive Discard (Tail-Drop) Frame Count */
			parm->nRxDroppedPkts = data;
			break;

		case 0x19: /* Receive Drop (Filter) Frame Count */
			parm->nRxFilteredPkts = data;
			break;

		case 0x24: /* Receive Good Byte Count (Low) */
			rgbcl = data;
			break;

		case 0x25: /* Receive Good Byte Count (High) */
			rgbch = data;
			break;

		case 0x26: /* Receive Bad Byte Count (Low) */
			rbbcl = data;
			break;

		case 0x27: /* Receive Bad Byte Count (High) */
			rbbch = data;
			break;

		case 0x0C: /* Transmit Frame Count */
			if (bcast_cnt == 1)
				parm->nTxBroadcastPkts = data;
			else
				parm->nTxGoodPkts = data;

			t_frame = data;
			break;

		case 0x06: /* Transmit Unicast Frame Count */
			parm->nTxUnicastPkts = data;
			t_unicast = data;
			break;

		case 0x07: /* Transmit Multicast Frame Count1 */
			parm->nTxMulticastPkts = data;
			t_multicast = data;
			break;

		case 0x00: /* Transmit Size 64 Frame Count */
			parm->nTx64BytePkts = data;
			break;

		case 0x01: /* Transmit Size 65-127 Frame Count */
			parm->nTx127BytePkts = data;
			break;

		case 0x02: /* Transmit Size 128-255 Frame Count */
			parm->nTx255BytePkts = data;
			break;

		case 0x03: /* Transmit Size 256-511 Frame Count */
			parm->nTx511BytePkts = data;
			break;

		case 0x04: /* Transmit Size 512-1023 Frame Count */
			parm->nTx1023BytePkts = data;
			break;

		case 0x05: /* Transmit Size Greater 1024 Frame Count */
			parm->nTxMaxBytePkts = data;
			break;

		case 0x08: /* Transmit Single Collision Count. */
			parm->nTxSingleCollCount = data;
			break;

		case 0x09: /* Transmit Multiple Collision Count */
			parm->nTxMultCollCount = data;
			break;

		case 0x0A: /* Transmit Late Collision Count */
			parm->nTxLateCollCount = data;
			break;

		case 0x0B: /* Transmit Excessive Collision.*/
			parm->nTxExcessCollCount = data;
			break;

		case 0x0D: /* Transmit Pause Frame Count */
			parm->nTxPauseCount = data;
			break;

		case 0x10: /* Transmit Drop Frame Count */
			parm->nTxDroppedPkts = data;
			break;

		case 0x0E: /* Transmit Good Byte Count (Low) */
			tgbcl = data;
			break;

		case 0x0F: /* Transmit Good Byte Count (High) */
			tgbch = data;
			break;

		case 0x11:
			/* Transmit Dropped Packet Cound, based on Congestion Management.*/
			parm->nTxAcmDroppedPkts = data;
			break;
		}
	}

	if (bcast_cnt == 1) {
		parm->nRxGoodPkts = r_frame + r_unicast + r_multicast;
		parm->nTxGoodPkts = t_frame + t_unicast + t_multicast;
	} else {
		/* Receive Broadcase Frme Count */
		parm->nRxBroadcastPkts = r_frame - r_unicast - r_multicast;
		/* Transmit Broadcase Frme Count */
		parm->nTxBroadcastPkts = t_frame - t_unicast - t_multicast;
	}

	/* Receive Good Byte Count */
	parm->nRxGoodBytes = (u64)(((rgbch & 0xFFFFFFFF) << 32) | (rgbcl & 0xFFFFFFFF));
	/* Receive Bad Byte Count */
	parm->nRxBadBytes = (u64)(((rbbch & 0xFFFFFFFF) << 32) | (rbbcl & 0xFFFFFFFF));
	/* Transmit Good Byte Count */
	parm->nTxGoodBytes = (u64)(((tgbch & 0xFFFFFFFF) << 32) | (tgbcl & 0xFFFFFFFF));
	return GSW_statusOk;
}

GSW_return_t GSW_RMON_Port_Get(void *cdev, GSW_RMON_Port_cnt_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_bm);
#endif

	if (IS_VRSN_31(gswdev->gipver)) {
		ret = GSW_RMON_Port_Get_Active(cdev, parm);

		if (ret == GSW_statusErr)
			goto UNLOCK_AND_RETURN;
	} else {
		ret = GSW_RMON_Port_Get_Legacy(cdev, parm);

		if (ret == GSW_statusErr)
			goto UNLOCK_AND_RETURN;
	}

	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_bm);
#endif
	return ret;

}

GSW_return_t GSW_RMON_Mode_Set(void *cdev, GSW_RMON_mode_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_bm);
#endif

	if (((gswdev->gipver == LTQ_GSWIP_3_0) && (gswdev->sdev == LTQ_FLOW_DEV_INT_R)) ||
	    IS_VRSN_31(gswdev->gipver)) {
		switch (parm->eRmonType) {
		case GSW_RMON_ALL_TYPE:
			break;

		case GSW_RMON_PMAC_TYPE:
			break;

		case GSW_RMON_PORT_TYPE:
			break;

		case GSW_RMON_METER_TYPE:
			if (parm->eCountMode == GSW_RMON_COUNT_BYTES)
				gsw_w32(cdev, BM_RMON_GCTRL_MRMON_OFFSET, BM_RMON_GCTRL_MRMON_SHIFT, \
					BM_RMON_GCTRL_MRMON_SIZE, 1);
			else
				gsw_w32(cdev, BM_RMON_GCTRL_MRMON_OFFSET, BM_RMON_GCTRL_MRMON_SHIFT, \
					BM_RMON_GCTRL_MRMON_SIZE, 0);

			break;

		case GSW_RMON_IF_TYPE:
			if (IS_VRSN_31(gswdev->gipver)) {
				ret = GSW_statusErr;
				goto UNLOCK_AND_RETURN;
			}

			if (parm->eCountMode == GSW_RMON_DROP_COUNT)
				gsw_w32(cdev, BM_RMON_GCTRL_INTMON_OFFSET, BM_RMON_GCTRL_INTMON_SHIFT, \
					BM_RMON_GCTRL_INTMON_SIZE, 1);
			else
				gsw_w32(cdev, BM_RMON_GCTRL_INTMON_OFFSET, BM_RMON_GCTRL_INTMON_SHIFT, \
					BM_RMON_GCTRL_INTMON_SIZE, 0);

			break;

		case GSW_RMON_ROUTE_TYPE:
			if (IS_VRSN_31(gswdev->gipver)) {
				ret = GSW_statusErr;
				goto UNLOCK_AND_RETURN;
			}

			if (parm->eCountMode == GSW_RMON_COUNT_BYTES)
				gsw_w32(cdev, PCE_GCTRL_1_RSCNTMD_OFFSET, PCE_GCTRL_1_RSCNTMD_SHIFT, \
					PCE_GCTRL_1_RSCNTMD_SIZE, 1);
			else
				gsw_w32(cdev, PCE_GCTRL_1_RSCNTMD_OFFSET, PCE_GCTRL_1_RSCNTMD_SHIFT, \
					PCE_GCTRL_1_RSCNTMD_SIZE, 0);

			break;

		case GSW_RMON_REDIRECT_TYPE:
			break;

		default:
			break;
		}
	} else {
		ret = GSW_statusErr;
	}

	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_bm);
#endif
	return ret;
}

static GSW_return_t GSW_RMON_Meter_Get_Legacy(void *cdev, GSW_RMON_Meter_cnt_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

	if ((gswdev->gipver == LTQ_GSWIP_3_0) && (gswdev->sdev == LTQ_FLOW_DEV_INT_R)) {
		u8 index, addr;
		u32 data, data0, data1;
		u8 i, j, m, rpt, loc, hc, crd[MAX_READ] = {0};
		u32 rdcount[MAX_READ], br;
		u8 meterid = parm->nMeterId;

		if (parm->nMeterId >= gswdev->num_of_meters)
			return GSW_statusErr;

		for (index = 0; index <= 3; index++) {
			rpt = 0;
			hc = 0;
			br = 0;
			addr = (meterid | (index << 6));
			gsw_w32(cdev, BM_RAM_ADDR_ADDR_OFFSET,
				BM_RAM_ADDR_ADDR_SHIFT,
				BM_RAM_ADDR_ADDR_SIZE, addr);
repeat:
			rpt++;

			for (i = 0; i < MAX_READ; i++)
				crd[i] = 0;

			loc = 0;

			for (j = 0; j < MAX_READ; j++) {
#ifndef CONFIG_X86_INTEL_CE2700
				/*suresh*/
//				ltq_w32(0x8019, gswr_bm_addr);
//				ltq_w32(0x0019, gswr_bm_addr);
				//			asm("SYNC");
//				ltq_w32(0x8019, gswr_bm_addr);
#endif
				CHECK_BUSY(BM_RAM_CTRL_BAS_OFFSET, BM_RAM_CTRL_BAS_SHIFT,
					   BM_RAM_CTRL_BAS_SIZE, RETURN_FROM_FUNCTION);

				for (i = 0; i < 4; i++) {
					gsw_r32(cdev, BM_RAM_VAL_0_VAL0_OFFSET,
						BM_RAM_VAL_0_VAL0_SHIFT,
						BM_RAM_VAL_0_VAL0_SIZE, &data0);
					gsw_r32(cdev, BM_RAM_VAL_1_VAL1_OFFSET,
						BM_RAM_VAL_1_VAL1_SHIFT,
						BM_RAM_VAL_1_VAL1_SIZE, &data1);
				}

				data = (data1 << 16 | data0);

				switch (index) {
				case 0:
					rdcount[j] = data;
					break;

				case 1:
					rdcount[j] = data;
					break;

				case 2:
					rdcount[j] = data;
					break;

				case 3:
					rdcount[j] = data;
					break;
				}
			}

			switch (index) {
			case 0:
				calc_credit_value(rdcount, &m, &loc);

				if (m > hc) {
					hc = m;
					br = rdcount[loc];
				}

				if ((m < REPEAT_L) && (rpt < REPEAT_M)) {
					goto repeat;
				}

				parm->nResCount = br;
				break;

			case 1:
				calc_credit_value(rdcount, &m, &loc);

				if (m > hc) {
					hc = m;
					br = rdcount[loc];
				}

				if ((m < REPEAT_L) && (rpt < REPEAT_M)) {
					goto repeat;
				}

				parm->nGreenCount = br;
				break;

			case 2:
				calc_credit_value(rdcount, &m, &loc);

				if (m > hc) {
					hc = m;
					br = rdcount[loc];
				}

				if ((m < REPEAT_L) && (rpt < REPEAT_M)) {
					goto repeat;
				}

				parm->nYellowCount = br;
				break;

			case 3:
				calc_credit_value(rdcount, &m, &loc);

				if (m > hc) {
					hc = m;
					br = rdcount[loc];
				}

				if ((m < REPEAT_L) && (rpt < REPEAT_M)) {
					goto repeat;
				}

				parm->nRedCount = br;
				break;
			}
		}
	} else {
		return GSW_statusErr;
	}

	return GSW_statusOk;
}

static GSW_return_t GSW_RMON_Meter_Get_Active(void *cdev, GSW_RMON_Meter_cnt_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 counter, data0, data1;
	u32 meterIndex, ctrlRegData = 0, addrRegData = 0, cntOffset = 0;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d\n", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

	//Temp store.
	meterIndex = parm->nMeterId;
	//Init to zero.
	memset(parm, 0, sizeof(GSW_RMON_Meter_cnt_t));
	//Restire back.
	parm->nMeterId = meterIndex;

	//Validate meter-id.
	if (parm->nMeterId > gswdev->num_of_meters) //Max index check.
		return GSW_statusErr;

	//Populate 'data' register's Meter-Id.
	FILL_CTRL_REG(addrRegData, BM_RAM_ADDR_RMON_METER_NUM_SHIFT, meterIndex);

	//Populate 'control' register data.
	FILL_CTRL_REG(ctrlRegData, BM_RAM_CTRL_ADDR_SHIFT, GSW_RMON_METER);
	//RAM in read mode.
	//FILL_CTRL_REG(ctrlRegData, BM_RAM_CTRL_OPMOD_SHIFT, 0);
	//Set access busy bit.
	FILL_CTRL_REG(ctrlRegData, BM_RAM_CTRL_BAS_SHIFT, 1);

	//Read all Green/Yellow/Red counters.
	for (cntOffset = GSW_RMON_METER_COLOR_GREEN;
	     cntOffset <= GSW_RMON_METER_COLOR_RED; cntOffset++) {
		//Populate 'data' register's Counter-Offset.
		CLEAR_FILL_CTRL_REG(addrRegData, BM_RAM_ADDR_RMON_METER_CLR_SHIFT,
				    BM_RAM_ADDR_RMON_METER_CLR_SIZE, cntOffset);
		//Set RAM address to read.
		gsw_w32_raw(cdev, BM_RAM_ADDR_REG_OFFSET, addrRegData);
		//Set RAM table to read.
		gsw_w32_raw(cdev, BM_RAM_CTRL_REG_OFFSET, ctrlRegData);
		//Wait untill RAM is ready to read.
		CHECK_BUSY(BM_RAM_CTRL_BAS_OFFSET, BM_RAM_CTRL_BAS_SHIFT,
			   BM_RAM_CTRL_BAS_SIZE, RETURN_FROM_FUNCTION);
		gsw_r32_raw(cdev, BM_RAM_VAL_0_VAL0_OFFSET, &data0);
		gsw_r32_raw(cdev, BM_RAM_VAL_1_VAL1_OFFSET, &data1);
		counter = (data1 << 16 | data0);

		switch (cntOffset) {
		case GSW_RMON_METER_COLOR_GREEN: /* Green color */
			parm->nGreenCount = counter;
			break;

		case GSW_RMON_METER_COLOR_YELLOW: /* Yellow color */
			parm->nYellowCount = counter;
			break;

		case GSW_RMON_METER_COLOR_RED: /* Red color */
			parm->nRedCount = counter;
			break;

		default:
			break;
		}

		counter = data0 = data1 = 0;
	}

	return GSW_statusOk;
}

GSW_return_t GSW_RMON_Meter_Get(void *cdev, GSW_RMON_Meter_cnt_t *parm)
{
	u32 ret;
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_bm);
#endif

	if (IS_VRSN_31(gswdev->gipver)) {
		ret = GSW_RMON_Meter_Get_Active(cdev, parm);

		if (ret == GSW_statusErr)
			goto UNLOCK_AND_RETURN;
	} else {
		ret = GSW_RMON_Meter_Get_Legacy(cdev, parm);

		if (ret == GSW_statusErr)
			goto UNLOCK_AND_RETURN;
	}

	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_bm);
#endif
	return ret;

}


GSW_return_t GSW_RMON_Redirect_Get(void *cdev, GSW_RMON_Redirect_cnt_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_bm);
#endif

	if ((gswdev->gipver == LTQ_GSWIP_3_0) && (gswdev->sdev == LTQ_FLOW_DEV_INT_R)) {
		u8 index;
		u8 i, j, m, rpt, loc, hc, crd[MAX_READ] = {0};
		u32 rdcount[MAX_READ] = {0}, br;
		u32 data, data0, data1, bm_ram_ctrl = 0;
		u64  rxbytes_l = 0, rxbytes_h = 0;
		u64 txbytes_l = 0, txbytes_h = 0;

		for (index = 0; index < 8; index++) {
			rpt = 0;
			hc = 0;
			br = 0;
			gsw_w32(cdev, BM_RAM_ADDR_ADDR_OFFSET, BM_RAM_ADDR_ADDR_SHIFT,
				BM_RAM_ADDR_ADDR_SIZE, index);
repeat:
			rpt++;

			for (i = 0; i < MAX_READ; i++)
				crd[i] = 0;

			loc = 0;

			for (j = 0; j < MAX_READ; j++) {
				/* Set Redirection RMON table Adrees */
				FILL_CTRL_REG(bm_ram_ctrl,
					      BM_RAM_CTRL_ADDR_SHIFT,
						  GSW_RMON_REDIRECTION);
				/* Set RAM access busy */
				FILL_CTRL_REG(bm_ram_ctrl,
					      BM_RAM_CTRL_BAS_SHIFT, 1);
				gsw_w32_raw(cdev, BM_RAM_CTRL_REG_OFFSET,
					    bm_ram_ctrl);
				/* Set initiate a RAM access */
				FILL_CTRL_REG(bm_ram_ctrl,
					      BM_RAM_CTRL_BAS_SHIFT, 0);
				gsw_w32_raw(cdev, BM_RAM_CTRL_REG_OFFSET,
					    bm_ram_ctrl);
				/* Set RAM access busy */
				FILL_CTRL_REG(bm_ram_ctrl,
					      BM_RAM_CTRL_BAS_SHIFT, 1);
				gsw_w32_raw(cdev, BM_RAM_CTRL_REG_OFFSET,
					    bm_ram_ctrl);
				/* Wait until RAM is ready to Read */
				CHECK_BUSY(BM_RAM_CTRL_BAS_OFFSET,
					   BM_RAM_CTRL_BAS_SHIFT,
					   BM_RAM_CTRL_BAS_SIZE,
					   RETURN_FROM_FUNCTION);
				for (i = 0; i < 4; i++) {
					gsw_r32(cdev, BM_RAM_VAL_0_VAL0_OFFSET,
						BM_RAM_VAL_0_VAL0_SHIFT,
						BM_RAM_VAL_0_VAL0_SIZE, &data0);
					gsw_r32(cdev, BM_RAM_VAL_1_VAL1_OFFSET,
						BM_RAM_VAL_1_VAL1_SHIFT,
						BM_RAM_VAL_1_VAL1_SIZE, &data1);
				}

				data = (data1 << 16 | data0);

				switch (index) {
				case 0:
					rdcount[j] = data;
					break;

				case 1:
					rdcount[j] = data;
					break;

				case 2:
					rdcount[j] = data;
					break;

				case 3:
					rdcount[j] = data;
					break;

				case 4:
					rdcount[j] = data;
					break;

				case 5:
					rdcount[j] = data;
					break;

				case 6:
					rdcount[j] = data;
					break;

				case 7:
					rdcount[j] = data;
					break;
				}
			}

			switch (index) {
			case 0:
				calc_credit_value(rdcount, &m, &loc);

				if (m > hc) {
					hc = m;
					br = rdcount[loc];
				}

				if ((m < REPEAT_L) && (rpt < REPEAT_M)) {
					goto repeat;
				}

				parm->nRxPktsCount = br;
				break;

			case 1:
				calc_credit_value(rdcount, &m, &loc);

				if (m > hc) {
					hc = m;
					br = rdcount[loc];
				}

				if ((m < REPEAT_L) && (rpt < REPEAT_M)) {
					goto repeat;
				}

				parm->nRxDiscPktsCount = br;
				break;

			case 2:
				calc_credit_byte_value(rdcount, &m, &loc);

				if (m > hc) {
					hc = m;
					br = rdcount[loc];
				}

				if ((m < REPEAT_L) && (rpt < REPEAT_M)) {
					goto repeat;
				}

				rxbytes_l = br;
				break;

			case 3:
				calc_credit_byte_value(rdcount, &m, &loc);

				if (m > hc) {
					hc = m;
					br = rdcount[loc];
				}

				if ((m < REPEAT_L) && (rpt < REPEAT_M)) {
					goto repeat;
				}

				rxbytes_h = br;
				break;

			case 4:
				calc_credit_value(rdcount, &m, &loc);

				if (m > hc) {
					hc = m;
					br = rdcount[loc];
				}

				if ((m < REPEAT_L) && (rpt < REPEAT_M)) {
					goto repeat;
				}

				parm->nTxPktsCount = br;
				break;

			case 5:
				calc_credit_value(rdcount, &m, &loc);

				if (m > hc) {
					hc = m;
					br = rdcount[loc];
				}

				if ((m < REPEAT_L) && (rpt < REPEAT_M)) {
					goto repeat;
				}

				parm->nTxDiscPktsCount = br;
				break;

			case 6:
				calc_credit_byte_value(rdcount, &m, &loc);

				if (m > hc) {
					hc = m;
					br = rdcount[loc];
				}

				if ((m < REPEAT_L) && (rpt < REPEAT_M)) {
					goto repeat;
				}

				txbytes_l = br;
				/*		parm->nTxBytesCount = br; */
				break;

			case 7:
				calc_credit_byte_value(rdcount, &m, &loc);

				if (m > hc) {
					hc = m;
					br = rdcount[loc];
				}

				if ((m < REPEAT_L) && (rpt < REPEAT_M)) {
					goto repeat;
				}

				txbytes_h = br;
				/*		parm->nTxBytesCount |= (br << 32); */
				break;
			}
		}

		parm->nRxBytesCount = (u64)(((rxbytes_h & 0xFFFFFFFF) << 32) | (rxbytes_l & 0xFFFFFFFF));
		parm->nTxBytesCount = (u64)(((txbytes_h & 0xFFFFFFFF) << 32) | (txbytes_l & 0xFFFFFFFF));
	} else {
		ret = GSW_statusErr;
	}

	ret = GSW_statusOk;

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_bm);
#endif
	return ret;

}

GSW_return_t GSW_RMON_If_Get(void *cdev, GSW_RMON_If_cnt_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_bm);
#endif

	if ((gswdev->gipver == LTQ_GSWIP_3_0) && (gswdev->sdev == LTQ_FLOW_DEV_INT_R)) {
		u8 index, i, j, m, rpt, loc, hc, crd[MAX_READ] = {0};
		u32 data, data0, data1, addr, intmon;
		u32 rdcount[MAX_READ] = {0}, br;
		u8 ifid = parm->nIfId;

		if (parm->nIfId >= 256) { /* ETHSW_CAP_13 register*/
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}

		gsw_r32(cdev, BM_RMON_GCTRL_INTMON_OFFSET,
			BM_RMON_GCTRL_INTMON_SHIFT,
			BM_RMON_GCTRL_INTMON_SIZE, &intmon);

		if (intmon)
			parm->countMode = GSW_RMON_DROP_COUNT;
		else
			parm->countMode = GSW_RMON_COUNT_BYTES;

		for (index = 0; index <= 3; index++) {
			rpt = 0;
			hc = 0;
			br = 0;
			addr = (ifid | (index << 8));
			gsw_w32(cdev, BM_RAM_ADDR_ADDR_OFFSET, BM_RAM_ADDR_ADDR_SHIFT,
				BM_RAM_ADDR_ADDR_SIZE, addr);
repeat:
			rpt++;

			for (i = 0; i < MAX_READ; i++)
				crd[i] = 0;

			loc = 0;

			for (j = 0; j < MAX_READ; j++) {
#ifndef CONFIG_X86_INTEL_CE2700
				/*suresh*/
				//			ltq_w32(0x801A, gswr_bm_addr);
				//			ltq_w32(0x001A, gswr_bm_addr);
				//			asm("SYNC");
				//			ltq_w32(0x801A, gswr_bm_addr);
#endif

				if (GSW_statusErr == CHECK_BUSY(BM_RAM_CTRL_BAS_OFFSET,
								BM_RAM_CTRL_BAS_SHIFT, BM_RAM_CTRL_BAS_SIZE, RETURN_ERROR_CODE))
					continue;

				for (i = 0; i < 4; i++) {
					gsw_r32(cdev, BM_RAM_VAL_0_VAL0_OFFSET,
						BM_RAM_VAL_0_VAL0_SHIFT,
						BM_RAM_VAL_0_VAL0_SIZE, &data0);
					gsw_r32(cdev, BM_RAM_VAL_1_VAL1_OFFSET,
						BM_RAM_VAL_1_VAL1_SHIFT,
						BM_RAM_VAL_1_VAL1_SIZE, &data1);
				}

				data = (data1 << 16 | data0);

				switch (index) {
				case 0:
					rdcount[j] = data;
					break;

				case 1:
					rdcount[j] = data;
					break;

				case 2:
					rdcount[j] = data;
					break;

				case 3:
					rdcount[j] = data;
					break;
				}
			}

			switch (index) {
			case 0:
				calc_credit_value(rdcount, &m, &loc);

				if (m > hc) {
					hc = m;
					br = rdcount[loc];
				}

				if ((m < REPEAT_L) && (rpt < REPEAT_M)) {
					goto repeat;
				}

				parm->nRxPktsCount = br;
				break;

			case 1:
				if (intmon)
					calc_credit_value(rdcount, &m, &loc);
				else
					calc_credit_byte_value(rdcount, &m, &loc);

				if (m > hc) {
					hc = m;
					br = rdcount[loc];
				}

				if ((m < REPEAT_L) && (rpt < REPEAT_M)) {
					goto repeat;
				}

				if (intmon) {
					parm->nRxDiscPktsCount = br;
					parm->nRxBytesCount = 0;
				} else {
					parm->nRxBytesCount = br;
					parm->nRxDiscPktsCount = 0;
				}

				break;

			case 2:
				calc_credit_value(rdcount, &m, &loc);

				if (m > hc) {
					hc = m;
					br = rdcount[loc];
				}

				if ((m < REPEAT_L) && (rpt < REPEAT_M)) {
					goto repeat;
				}

				parm->nTxPktsCount = br;
				break;

			case 3:
				if (intmon)
					calc_credit_value(rdcount, &m, &loc);
				else
					calc_credit_byte_value(rdcount, &m, &loc);

				if (m > hc) {
					hc = m;
					br = rdcount[loc];
				}

				if ((m < REPEAT_L) && (rpt < REPEAT_M)) {
					goto repeat;
				}

				if (intmon) {
					parm->nTxDiscPktsCount = br;
					parm->nTxBytesCount = 0;
				} else {
					parm->nTxBytesCount = br;
					parm->nTxDiscPktsCount = 0;
				}

				break;
			}
		}
	} else {
		ret = GSW_statusErr;
	}

	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_bm);
#endif
	return ret;
}

GSW_return_t GSW_RMON_Route_Get(void *cdev, GSW_RMON_Route_cnt_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return  GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_bm);
#endif

	if ((gswdev->gipver == LTQ_GSWIP_3_0) && (gswdev->sdev == LTQ_FLOW_DEV_INT_R)) {
		u8 index, addr;
		u8 i, j, m, rpt, loc, hc, crd[MAX_READ] = {0};
		u32 rdcount[MAX_READ] = {0}, br;
		u32 data, data0, data1;
		u8 rid = parm->nRoutedPortId;

		if (parm->nRoutedPortId >= 16 /* gswdev->tpnum*/) {
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}

		for (index = 0; index <= 13; index++) {
			addr = (rid | (index << 4));
			gsw_w32(cdev, BM_RAM_ADDR_ADDR_OFFSET, BM_RAM_ADDR_ADDR_SHIFT,
				BM_RAM_ADDR_ADDR_SIZE, addr);
			rpt = 0;
			hc = 0;
			br = 0;
repeat:
			rpt++;

			for (i = 0; i < MAX_READ; i++)
				crd[i] = 0;

			loc = 0;

			for (j = 0; j < MAX_READ; j++) {
#ifndef CONFIG_X86_INTEL_CE2700
				/*suresh*/
//				ltq_w32(0x801B, gswr_bm_addr);
//				ltq_w32(0x001B, gswr_bm_addr);
				//			asm("SYNC");
//				ltq_w32(0x801B, gswr_bm_addr);
#endif

				if (GSW_statusErr == CHECK_BUSY(BM_RAM_CTRL_BAS_SIZE,
								BM_RAM_CTRL_BAS_SHIFT, BM_RAM_CTRL_BAS_SIZE, RETURN_ERROR_CODE))
					continue;

				for (i = 0; i < 4; i++) {
					gsw_r32(cdev, BM_RAM_VAL_0_VAL0_OFFSET,
						BM_RAM_VAL_0_VAL0_SHIFT,
						BM_RAM_VAL_0_VAL0_SIZE, &data0);
					gsw_r32(cdev, BM_RAM_VAL_1_VAL1_OFFSET,
						BM_RAM_VAL_1_VAL1_SHIFT,
						BM_RAM_VAL_1_VAL1_SIZE, &data1);
				}

				data = (data1 << 16 | data0);

				switch (index) {
				case 0:
					rdcount[j] = data;
					break;

				case 1:
					rdcount[j] = data;
					break;

				case 2:
					rdcount[j] = data;
					break;

				case 3:
					rdcount[j] = data;
					break;

				case 4:
					rdcount[j] = data;
					break;

				case 5:
					rdcount[j] = data;
					break;

				case 6:
					rdcount[j] = data;
					break;

				case 7:
					rdcount[j] = data;
					break;

				case 8:
					rdcount[j] = data;
					break;

				case 9:
					rdcount[j] = data;
					break;

				case 0x0A:
					rdcount[j] = data;
					break;

				case 0x0B:
					rdcount[j] = data;
					break;

				case 0x0C:
					rdcount[j] = data;
					break;

				case 0x0D:
					rdcount[j] = data;
					break;
				}
			}

			switch (index) {
			case 0:
				calc_credit_value(rdcount, &m, &loc);

				if (m > hc) {
					hc = m;
					br = rdcount[loc];
				}

				if ((m < REPEAT_L) && (rpt < REPEAT_M)) {
					goto repeat;
				}

				parm->nRxUCv4UDPPktsCount = br;
				break;

			case 1:
				calc_credit_value(rdcount, &m, &loc);

				if (m > hc) {
					hc = m;
					br = rdcount[loc];
				}

				if ((m < REPEAT_L) && (rpt < REPEAT_M)) {
					goto repeat;
				}

				parm->nRxUCv4TCPPktsCount = br;
				break;

			case 2:
				calc_credit_value(rdcount, &m, &loc);

				if (m > hc) {
					hc = m;
					br = rdcount[loc];
				}

				if ((m < REPEAT_L) && (rpt < REPEAT_M)) {
					goto repeat;
				}

				parm->nRxMCv4PktsCount = br;
				break;

			case 3:
				calc_credit_value(rdcount, &m, &loc);

				if (m > hc) {
					hc = m;
					br = rdcount[loc];
				}

				if ((m < REPEAT_L) && (rpt < REPEAT_M)) {
					goto repeat;
				}

				parm->nRxUCv6UDPPktsCount = br;
				break;

			case 4:
				calc_credit_value(rdcount, &m, &loc);

				if (m > hc) {
					hc = m;
					br = rdcount[loc];
				}

				if ((m < REPEAT_L) && (rpt < REPEAT_M)) {
					goto repeat;
				}

				parm->nRxUCv6TCPPktsCount = br;
				break;

			case 5:
				calc_credit_value(rdcount, &m, &loc);

				if (m > hc) {
					hc = m;
					br = rdcount[loc];
				}

				if ((m < REPEAT_L) && (rpt < REPEAT_M)) {
					goto repeat;
				}

				parm->nRxMCv6PktsCount = br;
				break;

			case 6:
				calc_credit_byte_value(rdcount, &m, &loc);

				if (m > hc) {
					hc = m;
					br = rdcount[loc];
				}

				if ((m < REPEAT_L) && (rpt < REPEAT_M)) {
					goto repeat;
				}

				parm->nRxIPv4BytesCount = br;
				break;

			case 7:
				calc_credit_byte_value(rdcount, &m, &loc);

				if (m > hc) {
					hc = m;
					br = rdcount[loc];
				}

				if ((m < REPEAT_L) && (rpt < REPEAT_M)) {
					goto repeat;
				}

				parm->nRxIPv6BytesCount = br;
				break;

			case 8:
				calc_credit_value(rdcount, &m, &loc);

				if (m > hc) {
					hc = m;
					br = rdcount[loc];
				}

				if ((m < REPEAT_L) && (rpt < REPEAT_M)) {
					goto repeat;
				}

				parm->nRxCpuPktsCount = br;
				break;

			case 9:
				calc_credit_byte_value(rdcount, &m, &loc);

				if (m > hc) {
					hc = m;
					br = rdcount[loc];
				}

				if ((m < REPEAT_L) && (rpt < REPEAT_M)) {
					goto repeat;
				}

				parm->nRxCpuBytesCount = br;
				break;

			case 0x0A:
				calc_credit_value(rdcount, &m, &loc);

				if (m > hc) {
					hc = m;
					br = rdcount[loc];
				}

				if ((m < REPEAT_L) && (rpt < REPEAT_M)) {
					goto repeat;
				}

				parm->nRxPktsDropCount = br;
				break;

			case 0x0B:
				calc_credit_byte_value(rdcount, &m, &loc);

				if (m > hc) {
					hc = m;
					br = rdcount[loc];
				}

				if ((m < REPEAT_L) && (rpt < REPEAT_M)) {
					goto repeat;
				}

				parm->nRxBytesDropCount = br;
				break;

			case 0x0C:
				calc_credit_value(rdcount, &m, &loc);

				if (m > hc) {
					hc = m;
					br = rdcount[loc];
				}

				if ((m < REPEAT_L) && (rpt < REPEAT_M)) {
					goto repeat;
				}

				parm->nTxPktsCount = br;
				break;

			case 0x0D:
				calc_credit_byte_value(rdcount, &m, &loc);

				if (m > hc) {
					hc = m;
					br = rdcount[loc];
				}

				if ((m < REPEAT_L) && (rpt < REPEAT_M)) {
					goto repeat;
				}

				parm->nTxBytesCount = br;
				break;
			}
		}
	} else {
		ret = GSW_statusErr;
	}

	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_bm);
#endif
	return ret;

}

static GSW_return_t GSW_PMAC_CountGet_v31(void *cdev, GSW_PMAC_Cnt_t *parm)
{
	u8 i = 0;
	u32 data;
	bmtbl_prog_t bmtable = {0};
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return  GSW_statusErr;
	}

	/* For Ingress it is 0 - 16, for Egress 0 - 15. */
	if ((parm->nTxDmaChanId >= 17) || (parm->nPmacId >= 2)) {
		return GSW_statusErr;
	}

	for (i = 0; i < 8; i++) {
		/* Egress csum counter not present with 3.1 */
		if ((i == 2) || (i == 3))
			continue;

		bmtable.adr.pmacRmon.channel_or_port = parm->nTxDmaChanId;
		bmtable.adr.pmacRmon.count = i;
		bmtable.adr.pmacRmon.pmacNo = parm->nPmacId;
		bmtable.b64bitMode = parm->b64BitMode;
		bmtable.tableID = PMAC_RMON_COUNTER;
		bmtable.numValues = 2;
		gsw_bm_table_read(cdev, &bmtable);
		data = (bmtable.value[1] << 16) | (bmtable.value[0] & 0xFFFF);

		switch (i) {
		case 0:
			parm->nDiscPktsCount = data;
			break;

		case 1:
			parm->nDiscBytesCount = data;
			break;

		case 4:
			parm->nIngressPktsCount = data;
			break;

		case 5:
			parm->nIngressBytesCount = data;
			break;

		case 6:
			parm->nEgressPktsCount = data;
			break;

		case 7:
			parm->nEgressBytesCount = data;
			break;

		default:
			break;
		}
	}

	return GSW_statusOk;
}

int GSW_PMAC_CountGet(void *cdev, GSW_PMAC_Cnt_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_bm);
#endif

	if ((gswdev->gipver == LTQ_GSWIP_3_0) && (gswdev->sdev == LTQ_FLOW_DEV_INT_R)) {
		u8 index;
		u8 i, j, m, rpt, loc, hc, crd[MAX_READ] = {0};
		u32 rdcount[MAX_READ] = {0}, br;
		u32 data0, data1, data, value;

		if (parm->nTxDmaChanId >= 16) {
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}

		for (index = 0; index < 4; index++) {
			value = ((index << 4) | (parm->nTxDmaChanId & 0xF));
			gsw_w32(cdev, BM_RAM_ADDR_ADDR_OFFSET, BM_RAM_ADDR_ADDR_SHIFT,
				BM_RAM_ADDR_ADDR_SIZE, value);
			rpt = 0;
			hc = 0;
			br = 0;
repeat:
			rpt++;

			for (i = 0; i < MAX_READ; i++)
				crd[i] = 0;

			loc = 0;

			for (j = 0; j < MAX_READ; j++) {
#ifndef CONFIG_X86_INTEL_CE2700
				//ltq_w32(0x801C, gswr_bm_addr);
				//ltq_w32(0x001C, gswr_bm_addr);
//				asm("SYNC");
				//ltq_w32(0x801C, gswr_bm_addr);
#endif

				if (GSW_statusErr == CHECK_BUSY(BM_RAM_CTRL_BAS_OFFSET,
								BM_RAM_CTRL_BAS_SHIFT, BM_RAM_CTRL_BAS_SIZE, RETURN_ERROR_CODE))
					continue;

				for (i = 0; i < 4; i++) {
					gsw_r32(cdev, BM_RAM_VAL_0_VAL0_OFFSET,
						BM_RAM_VAL_0_VAL0_SHIFT,
						BM_RAM_VAL_0_VAL0_SIZE, &data0);
					gsw_r32(cdev, BM_RAM_VAL_1_VAL1_OFFSET,
						BM_RAM_VAL_1_VAL1_SHIFT,
						BM_RAM_VAL_1_VAL1_SIZE, &data1);
				}

				data = (data1 << 16 | data0);

				switch (index) {
				case 0:
					rdcount[j] = data;
					break;

				case 1:
					rdcount[j] = data;
					break;

				case 2:
					rdcount[j] = data;
					break;

				case 3:
					rdcount[j] = data;
					break;
				}
			}

			switch (index) {
			case 0:
				calc_credit_value(rdcount, &m, &loc);

				if (m > hc) {
					hc = m;
					br = rdcount[loc];
				}

				if ((m < REPEAT_L) && (rpt < REPEAT_M)) {
					goto repeat;
				}

				parm->nDiscPktsCount = br;
				break;

			case 1:
				calc_credit_byte_value(rdcount, &m, &loc);

				if (m > hc) {
					hc = m;
					br = rdcount[loc];
				}

				if ((m < REPEAT_L) && (rpt < REPEAT_M)) {
					goto repeat;
				}

				parm->nDiscBytesCount = br;
				break;

			case 2:
				calc_credit_value(rdcount, &m, &loc);

				if (m > hc) {
					hc = m;
					br = rdcount[loc];
				}

				if ((m < REPEAT_L) && (rpt < REPEAT_M)) {
					goto repeat;
				}

				parm->nChkSumErrPktsCount = br;
				break;

			case 3:
				calc_credit_byte_value(rdcount, &m, &loc);

				if (m > hc) {
					hc = m;
					br = rdcount[loc];
				}

				if ((m < REPEAT_L) && (rpt < REPEAT_M)) {
					goto repeat;
				}

				parm->nChkSumErrBytesCount = br;
				break;
			}
		}
	} else if ((gswdev->gipver == LTQ_GSWIP_3_1)) {
		GSW_PMAC_CountGet_v31(cdev, parm);
	} else {
		ret = GSW_statusErr;
	}

	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_bm);
#endif
	return ret;
}



GSW_return_t GSW_PMAC_GLBL_CfgSet(void *cdev, GSW_PMAC_Glbl_Cfg_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 pmac_ctrl0, pmac_ctrl1, pmac_ctrl2, pmac_ctrl3, pmac_ctrl4;
	u32 pmac_ctrl0_off,
	    pmac_ctrl1_off,
	    pmac_ctrl2_off,
	    pmac_ctrl3_off,
	    pmac_ctrl4_off,
	    pmac_bsl1_off,
	    pmac_bsl2_off,
	    pmac_bsl3_off;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_pmac);
#endif

	pmac_ctrl0_off = pmac_addr_off(PMAC_CTRL_0_PADEN_OFFSET, parm->nPmacId);
	pmac_ctrl1_off = pmac_addr_off(PMAC_CTRL_1_MLEN_OFFSET, parm->nPmacId);
	pmac_ctrl2_off = pmac_addr_off(PMAC_CTRL_2_LCHKL_OFFSET, parm->nPmacId);
	pmac_ctrl3_off = pmac_addr_off(PMAC_CTRL_3_JUMBO_OFFSET, parm->nPmacId);
	pmac_ctrl4_off = pmac_addr_off(PMAC_CTRL_4_FLAGEN_OFFSET, parm->nPmacId);

	pmac_bsl1_off = pmac_addr_off(PMAC_BSL_LEN0_OFFSET, parm->nPmacId);
	pmac_bsl2_off = pmac_addr_off(PMAC_BSL_LEN1_OFFSET, parm->nPmacId);
	pmac_bsl3_off = pmac_addr_off(PMAC_BSL_LEN2_OFFSET, parm->nPmacId);

	gsw_r32_raw(cdev, pmac_ctrl0_off, &pmac_ctrl0);

	if (IS_VRSN_31(gswdev->gipver))
		gsw_r32_raw(cdev, pmac_ctrl1_off, &pmac_ctrl1);

	gsw_r32_raw(cdev, pmac_ctrl2_off, &pmac_ctrl2);
	gsw_r32_raw(cdev, pmac_ctrl3_off, &pmac_ctrl3);
	gsw_r32_raw(cdev, pmac_ctrl4_off, &pmac_ctrl4);

	if (IS_VRSN_30_31(gswdev->gipver)) {
		pmac_ctrl0 = gsw_field_w32(pmac_ctrl0, PMAC_CTRL_0_APADEN_SHIFT,
					   PMAC_CTRL_0_APADEN_SIZE, parm->bAPadEna);
		pmac_ctrl0 = gsw_field_w32(pmac_ctrl0, PMAC_CTRL_0_PADEN_SHIFT,
					   PMAC_CTRL_0_PADEN_SIZE, parm->bPadEna);
		pmac_ctrl0 = gsw_field_w32(pmac_ctrl0, PMAC_CTRL_0_VPADEN_SHIFT,
					   PMAC_CTRL_0_VPADEN_SIZE, parm->bVPadEna);
		pmac_ctrl0 = gsw_field_w32(pmac_ctrl0, PMAC_CTRL_0_VPAD2EN_SHIFT,
					   PMAC_CTRL_0_VPAD2EN_SIZE, parm->bSVPadEna);
		pmac_ctrl0 = gsw_field_w32(pmac_ctrl0, PMAC_CTRL_0_FCS_SHIFT,
					   PMAC_CTRL_0_FCS_SIZE, parm->bTxFCSDis);

		if (IS_VRSN_31(gswdev->gipver)) {
			pmac_ctrl0 = gsw_field_w32(pmac_ctrl0, PMAC_CTRL_0_FCSEN_SHIFT,
						   PMAC_CTRL_0_FCSEN_SIZE, parm->bRxFCSDis);
		}

		if (gswdev->gipver == LTQ_GSWIP_3_0) {
			pmac_ctrl0 = gsw_field_w32(pmac_ctrl0, PMAC_CTRL_0_CHKREG_SHIFT,
						   PMAC_CTRL_0_CHKREG_SIZE, parm->bIPTransChkRegDis);
			pmac_ctrl0 = gsw_field_w32(pmac_ctrl0, PMAC_CTRL_0_CHKVER_SHIFT,
						   PMAC_CTRL_0_CHKVER_SIZE, parm->bIPTransChkVerDis);
			pmac_ctrl4 = gsw_field_w32(pmac_ctrl4, PMAC_CTRL_4_FLAGEN_SHIFT,
						   PMAC_CTRL_4_FLAGEN_SIZE, parm->bProcFlagsEgCfgEna);
		}

		if (parm->bJumboEna == 1) {
			pmac_ctrl2 = gsw_field_w32(pmac_ctrl2, PMAC_CTRL_2_MLEN_SHIFT,
						   PMAC_CTRL_2_MLEN_SIZE, 1);

			if (IS_VRSN_31(gswdev->gipver))
				pmac_ctrl1 = gsw_field_w32(pmac_ctrl1, PMAC_CTRL_1_MLEN_SHIFT,
							   PMAC_CTRL_1_MLEN_SIZE, parm->nMaxJumboLen);

			if (gswdev->gipver == LTQ_GSWIP_3_0)
				pmac_ctrl3 = gsw_field_w32(pmac_ctrl3, PMAC_CTRL_3_JUMBO_SHIFT,
							   PMAC_CTRL_3_JUMBO_SIZE, parm->nMaxJumboLen);
		} else {
			pmac_ctrl2 = gsw_field_w32(pmac_ctrl2, PMAC_CTRL_2_MLEN_SHIFT,
						   PMAC_CTRL_2_MLEN_SIZE, 0);

			if (gswdev->gipver == LTQ_GSWIP_3_0)
				pmac_ctrl3 = gsw_field_w32(pmac_ctrl3, PMAC_CTRL_3_JUMBO_SHIFT,
							   PMAC_CTRL_3_JUMBO_SIZE, 0x8F8);
		}

		pmac_ctrl2 = gsw_field_w32(pmac_ctrl2, PMAC_CTRL_2_LCHKL_SHIFT,
					   PMAC_CTRL_2_LCHKL_SIZE, parm->bLongFrmChkDis);

		switch (parm->eShortFrmChkType) {
		case GSW_PMAC_SHORT_LEN_DIS:
			pmac_ctrl2 = gsw_field_w32(pmac_ctrl2, PMAC_CTRL_2_LCHKS_SHIFT,
						   PMAC_CTRL_2_LCHKS_SIZE, 0);
			break;

		case GSW_PMAC_SHORT_LEN_ENA_UNTAG:
			pmac_ctrl2 = gsw_field_w32(pmac_ctrl2, PMAC_CTRL_2_LCHKS_SHIFT,
						   PMAC_CTRL_2_LCHKS_SIZE, 1);
			break;

		case GSW_PMAC_SHORT_LEN_ENA_TAG:
			pmac_ctrl2 = gsw_field_w32(pmac_ctrl2, PMAC_CTRL_2_LCHKS_SHIFT,
						   PMAC_CTRL_2_LCHKS_SIZE, 2);
			break;

		case GSW_PMAC_SHORT_LEN_RESERVED:
			pmac_ctrl2 = gsw_field_w32(pmac_ctrl2, PMAC_CTRL_2_LCHKS_SHIFT,
						   PMAC_CTRL_2_LCHKS_SIZE, 3);
			break;
		}

		if (IS_VRSN_31(gswdev->gipver)) {
			switch (parm->eProcFlagsEgCfg) {
			case GSW_PMAC_PROC_FLAGS_NONE:
				break;

			case GSW_PMAC_PROC_FLAGS_TC:
				pmac_ctrl4 = gsw_field_w32(pmac_ctrl4, PMAC_CTRL_4_FLAGEN_SHIFT,
							   PMAC_CTRL_4_GSWIP3_1_FLAGEN_SIZE, 0);
				break;

			case GSW_PMAC_PROC_FLAGS_FLAG:
				pmac_ctrl4 = gsw_field_w32(pmac_ctrl4, PMAC_CTRL_4_FLAGEN_SHIFT,
							   PMAC_CTRL_4_GSWIP3_1_FLAGEN_SIZE, 1);
				break;

			case GSW_PMAC_PROC_FLAGS_MIX:
				pmac_ctrl4 = gsw_field_w32(pmac_ctrl4, PMAC_CTRL_4_FLAGEN_SHIFT,
							   PMAC_CTRL_4_GSWIP3_1_FLAGEN_SIZE, 2);
				break;
			}

			if (parm->nBslThreshold[0]) {
				gsw_w32(cdev, pmac_bsl1_off,
					PMAC_BSL_LEN0_SHIFT,
					PMAC_BSL_LEN0_SIZE,
					parm->nBslThreshold[0]);
			}

			if (parm->nBslThreshold[1]) {
				gsw_w32(cdev, pmac_bsl2_off,
					PMAC_BSL_LEN1_SHIFT,
					PMAC_BSL_LEN1_SIZE,
					parm->nBslThreshold[1]);
			}

			if (parm->nBslThreshold[2]) {
				gsw_w32(cdev, pmac_bsl3_off,
					PMAC_BSL_LEN2_SHIFT,
					PMAC_BSL_LEN2_SIZE,
					parm->nBslThreshold[2]);
			}
		}

		gsw_w32_raw(cdev, pmac_ctrl0_off, pmac_ctrl0);

		if (IS_VRSN_31(gswdev->gipver))
			gsw_w32_raw(cdev, pmac_ctrl1_off, pmac_ctrl1);

		gsw_w32_raw(cdev, pmac_ctrl2_off, pmac_ctrl2);
		gsw_w32_raw(cdev, pmac_ctrl3_off, pmac_ctrl3);
		gsw_w32_raw(cdev, pmac_ctrl4_off, pmac_ctrl4);
	}

	ret = GSW_statusOk;

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_pmac);
#endif
	return ret;
}

GSW_return_t GSW_PMAC_GLBL_CfgGet(void *cdev, GSW_PMAC_Glbl_Cfg_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 regval;
	u32 pmac_ctrl0, pmac_ctrl1, pmac_ctrl2, pmac_ctrl3, pmac_ctrl4;
	u32 pmac_ctrl0_off,
	    pmac_ctrl1_off,
	    pmac_ctrl2_off,
	    pmac_ctrl3_off,
	    pmac_ctrl4_off,
	    pmac_bsl1_off,
	    pmac_bsl2_off,
	    pmac_bsl3_off;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_pmac);
#endif

	pmac_ctrl0_off = pmac_addr_off(PMAC_CTRL_0_PADEN_OFFSET, parm->nPmacId);
	pmac_ctrl1_off = pmac_addr_off(PMAC_CTRL_1_MLEN_OFFSET, parm->nPmacId);
	pmac_ctrl2_off = pmac_addr_off(PMAC_CTRL_2_LCHKL_OFFSET, parm->nPmacId);
	pmac_ctrl3_off = pmac_addr_off(PMAC_CTRL_3_JUMBO_OFFSET, parm->nPmacId);
	pmac_ctrl4_off = pmac_addr_off(PMAC_CTRL_4_FLAGEN_OFFSET, parm->nPmacId);

	pmac_bsl1_off = pmac_addr_off(PMAC_BSL_LEN0_OFFSET, parm->nPmacId);
	pmac_bsl2_off = pmac_addr_off(PMAC_BSL_LEN1_OFFSET, parm->nPmacId);
	pmac_bsl3_off = pmac_addr_off(PMAC_BSL_LEN2_OFFSET, parm->nPmacId);


	gsw_r32_raw(cdev, pmac_ctrl0_off, &pmac_ctrl0);

	if (IS_VRSN_31(gswdev->gipver))
		gsw_r32_raw(cdev, pmac_ctrl1_off, &pmac_ctrl1);

	gsw_r32_raw(cdev, pmac_ctrl2_off, &pmac_ctrl2);
	gsw_r32_raw(cdev, pmac_ctrl3_off, &pmac_ctrl3);
	gsw_r32_raw(cdev, pmac_ctrl4_off, &pmac_ctrl4);


	if (IS_VRSN_30_31(gswdev->gipver)) {

		parm->bAPadEna = gsw_field_r32(pmac_ctrl0, PMAC_CTRL_0_APADEN_SHIFT,
					       PMAC_CTRL_0_APADEN_SIZE);
		parm->bPadEna = gsw_field_r32(pmac_ctrl0, PMAC_CTRL_0_PADEN_SHIFT,
					      PMAC_CTRL_0_PADEN_SIZE);
		parm->bVPadEna = gsw_field_r32(pmac_ctrl0, PMAC_CTRL_0_VPADEN_SHIFT,
					       PMAC_CTRL_0_VPADEN_SIZE);
		parm->bSVPadEna = gsw_field_r32(pmac_ctrl0, PMAC_CTRL_0_VPAD2EN_SHIFT,
						PMAC_CTRL_0_VPAD2EN_SIZE);
		parm->bTxFCSDis = gsw_field_r32(pmac_ctrl0, PMAC_CTRL_0_FCS_SHIFT,
						PMAC_CTRL_0_FCS_SIZE);

		if (IS_VRSN_31(gswdev->gipver)) {
			parm->bRxFCSDis = gsw_field_r32(pmac_ctrl0, PMAC_CTRL_0_FCSEN_SHIFT,
							PMAC_CTRL_0_FCSEN_SIZE);
		}

		if (gswdev->gipver == LTQ_GSWIP_3_0) {
			parm->bIPTransChkRegDis = gsw_field_r32(pmac_ctrl0, PMAC_CTRL_0_CHKREG_SHIFT,
								PMAC_CTRL_0_CHKREG_SIZE);
			parm->bIPTransChkVerDis = gsw_field_r32(pmac_ctrl0, PMAC_CTRL_0_CHKVER_SHIFT,
								PMAC_CTRL_0_CHKVER_SIZE);
			parm->bProcFlagsEgCfgEna = gsw_field_r32(pmac_ctrl4, PMAC_CTRL_4_FLAGEN_SHIFT,
						   PMAC_CTRL_4_FLAGEN_SIZE);
			regval = gsw_field_r32(pmac_ctrl3, PMAC_CTRL_3_JUMBO_SHIFT,
					       PMAC_CTRL_3_JUMBO_SIZE);
			parm->nMaxJumboLen = regval;
		}

		parm->bJumboEna = gsw_field_r32(pmac_ctrl2, PMAC_CTRL_2_MLEN_SHIFT,
						PMAC_CTRL_2_MLEN_SIZE);
		parm->bLongFrmChkDis = gsw_field_r32(pmac_ctrl2, PMAC_CTRL_2_LCHKL_SHIFT,
						     PMAC_CTRL_2_LCHKL_SIZE);
		regval = gsw_field_r32(pmac_ctrl2, PMAC_CTRL_2_LCHKS_SHIFT,
				       PMAC_CTRL_2_LCHKS_SIZE);
		parm->eShortFrmChkType = regval;


		if (IS_VRSN_31(gswdev->gipver)) {
			gsw_r32_raw(cdev, pmac_ctrl1_off, &regval);

			parm->nMaxJumboLen = regval;

			parm->eProcFlagsEgCfg = gsw_field_r32(pmac_ctrl4,
							      PMAC_CTRL_4_FLAGEN_SHIFT,
							      PMAC_CTRL_4_GSWIP3_1_FLAGEN_SIZE);

			gsw_r32(cdev,
				pmac_bsl1_off,
				PMAC_BSL_LEN0_SHIFT,
				PMAC_BSL_LEN0_SIZE,
				&parm->nBslThreshold[0]);

			gsw_r32(cdev,
				pmac_bsl2_off,
				PMAC_BSL_LEN1_SHIFT,
				PMAC_BSL_LEN1_SIZE,
				&parm->nBslThreshold[1]);

			gsw_r32(cdev,
				pmac_bsl3_off,
				PMAC_BSL_LEN2_SHIFT,
				PMAC_BSL_LEN2_SIZE,
				&parm->nBslThreshold[2]);
		}
	}

	ret = GSW_statusOk;

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_pmac);
#endif
	return ret;
}


/* Back pressure mapping Table */
int GSW_PMAC_BM_CfgSet(void *cdev, GSW_PMAC_BM_Cfg_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	pmtbl_prog_t pmtbl;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_pmac);
#endif

	if (IS_VRSN_30_31(gswdev->gipver)) {

		memset(&pmtbl, 0, sizeof(pmtbl));

		if (gswdev->gipver == LTQ_GSWIP_3_0)
			pmtbl.ptaddr = (parm->nTxDmaChanId & 0x0F);

		if (IS_VRSN_31(gswdev->gipver))
			pmtbl.ptaddr = (parm->nTxDmaChanId & 0x1F);

		pmtbl.pmacId = parm->nPmacId;

		/* RX Port Congestion Mask (bit 15:0) */
		pmtbl.val[0] = (parm->rxPortMask & 0xFFFF);
		/* TX Queue Congestion Mask (bit 15:0) */
		pmtbl.val[1] = (parm->txQMask & 0xFFFF);
		/* TX Queue Congestion Mask (bit 31:16)  */
		pmtbl.val[2] = ((parm->txQMask >> 16) & 0xFFFF);
		pmtbl.ptcaddr = PMAC_BPMAP_INDEX;

		xwayflow_pmac_table_write(cdev, &pmtbl);
	}

	ret = GSW_statusOk;

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_pmac);
#endif
	return ret;

}

int GSW_PMAC_BM_CfgGet(void *cdev, GSW_PMAC_BM_Cfg_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	pmtbl_prog_t pmtbl;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_pmac);
#endif

	if (IS_VRSN_30_31(gswdev->gipver)) {
		memset(&pmtbl, 0, sizeof(pmtbl));

		if (gswdev->gipver == LTQ_GSWIP_3_0)
			pmtbl.ptaddr = (parm->nTxDmaChanId & 0x0F);

		if (IS_VRSN_31(gswdev->gipver))
			pmtbl.ptaddr = (parm->nTxDmaChanId & 0x1F);

		pmtbl.pmacId = parm->nPmacId;

		pmtbl.ptcaddr	= PMAC_BPMAP_INDEX;
		xwayflow_pmac_table_read(cdev, &pmtbl);

		/* RX Port Congestion Mask (bit 15:0) */
		parm->rxPortMask = (pmtbl.val[0]);
		/* TX Queue Congestion Mask (bit 15:0) */
		parm->txQMask = (pmtbl.val[1]);
		/* TX Queue Congestion Mask (bit 31:16)  */
		parm->txQMask |= (pmtbl.val[2]) << 16;
	}

	ret = GSW_statusOk;

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_pmac);
#endif
	return ret;
}

int GSW_PMAC_IG_CfgSet(void *cdev, GSW_PMAC_Ig_Cfg_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	pmtbl_prog_t pmtbl;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_pmac);
#endif

	if (IS_VRSN_30_31(gswdev->gipver)) {
		memset(&pmtbl, 0, sizeof(pmtbl));

		pmtbl.pmacId = parm->nPmacId;

		if (gswdev->gipver == LTQ_GSWIP_3_0)
			pmtbl.ptaddr = (parm->nTxDmaChanId & 0x0F);

		if (IS_VRSN_31(gswdev->gipver))
			pmtbl.ptaddr = (parm->nTxDmaChanId & 0x1F);

		/* Default PMAC Header Bytes */
		pmtbl.val[0] = (((parm->defPmacHdr[0] & 0xFF) << 8)
				| (parm->defPmacHdr[1] & 0xFF));
		pmtbl.val[1] = (((parm->defPmacHdr[2] & 0xFF) << 8)
				| (parm->defPmacHdr[3] & 0xFF));
		pmtbl.val[2] = (((parm->defPmacHdr[4] & 0xFF) << 8)
				| (parm->defPmacHdr[5] & 0xFF));
		pmtbl.val[3] = (((parm->defPmacHdr[6] & 0xFF) << 8)
				| (parm->defPmacHdr[7] & 0xFF));

		/* Packet has PMAC header or not */
		if (parm->bPmacPresent)
			pmtbl.val[4] |= (1 << 0);

		/* Source Port Id from default PMAC header */
		if (parm->bSpIdDefault)
			pmtbl.val[4] |= (1 << 1);
		else {
			/*GSWIP 3.1: SPPID from packet descriptor*/
			pmtbl.val[4] &= ~(1 << 1);
		}

		/* Sub_Interface Id  Info from default PMAC header  */
		switch (parm->eSubId) {
		case GSW_PMAC_IG_CFG_SRC_DMA_DESC:
			pmtbl.val[4] &= ~(1 << 2);
			break;

		case GSW_PMAC_IG_CFG_SRC_PMAC:
		case GSW_PMAC_IG_CFG_SRC_DEF_PMAC:
			pmtbl.val[4] |= (1 << 2);
			break;

		default:
			break;
		}

		/* Class Enable info from default PMAC header  */
		if (parm->bClassEna)
			pmtbl.val[4] |= (1 << 3);

		if (gswdev->gipver == LTQ_GSWIP_3_0) {
			/* Port Map Enable info from default PMAC header */
			if (parm->bPmapEna)
				pmtbl.val[4] |= (1 << 4);

			/* Port Map info from default PMAC header  */
			if (parm->bPmapDefault)
				pmtbl.val[4] |= (1 << 6);
		}

		/* Class Info from default PMAC header */
		if (parm->bClassDefault)
			pmtbl.val[4] |= (1 << 5);


		/* Error set packets to be discarded  */
		if (parm->bErrPktsDisc)
			pmtbl.val[4] |= (1 << 7);

		pmtbl.ptcaddr	= PMAC_IGCFG_INDEX;
		xwayflow_pmac_table_write(cdev, &pmtbl);
	}

	ret = GSW_statusOk;

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_pmac);
#endif
	return ret;

}


int GSW_PMAC_IG_CfgGet(void *cdev, GSW_PMAC_Ig_Cfg_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	pmtbl_prog_t pmtbl;
	u32 SubidMode;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_pmac);
#endif

	if (IS_VRSN_30_31(gswdev->gipver)) {
		memset(&pmtbl, 0, sizeof(pmtbl));

		pmtbl.pmacId = parm->nPmacId;

		if (gswdev->gipver == LTQ_GSWIP_3_0)
			pmtbl.ptaddr = (parm->nTxDmaChanId & 0x0F);

		if (IS_VRSN_31(gswdev->gipver))
			pmtbl.ptaddr = (parm->nTxDmaChanId & 0x1F);

		pmtbl.ptcaddr	= PMAC_IGCFG_INDEX;
		xwayflow_pmac_table_read(cdev, &pmtbl);

		/* Default PMAC Header Bytes */
		parm->defPmacHdr[0] = ((pmtbl.val[0] >> 8) & 0xFF);
		parm->defPmacHdr[1] = (pmtbl.val[0] & 0xFF);
		parm->defPmacHdr[2] = ((pmtbl.val[1] >> 8) & 0xFF);
		parm->defPmacHdr[3] = (pmtbl.val[1] & 0xFF);
		parm->defPmacHdr[4] = ((pmtbl.val[2] >> 8) & 0xFF);
		parm->defPmacHdr[5] = (pmtbl.val[2] & 0xFF);
		parm->defPmacHdr[6] = ((pmtbl.val[3] >> 8) & 0xFF);
		parm->defPmacHdr[7] = (pmtbl.val[3] & 0xFF);

		/* Packet has PMAC header or not */
		if ((pmtbl.val[4] >> 0) & 0x1)
			parm->bPmacPresent = 1;

		/* Source Port Id from default PMAC header */
		if ((pmtbl.val[4] >> 1) & 0x1)
			parm->bSpIdDefault = 1;

		/* Sub_Interface Id  Info */
		SubidMode = ((pmtbl.val[4] >> 2) & 0x1);

		if (SubidMode == 0) {
			parm->eSubId = GSW_PMAC_IG_CFG_SRC_DMA_DESC;
		} else {
			if (IS_VRSN_31(gswdev->gipver)) {
				parm->eSubId = GSW_PMAC_IG_CFG_SRC_DEF_PMAC;
			} else {
				parm->eSubId = GSW_PMAC_IG_CFG_SRC_PMAC;
			}
		}

		/* Class Enable info from default PMAC header  */
		if ((pmtbl.val[4] >> 3) & 0x1)
			parm->bClassEna = 1;

		if (gswdev->gipver == LTQ_GSWIP_3_0) {
			/* Port Map Enable info from default PMAC header */
			if ((pmtbl.val[4] >> 4) & 0x1)
				parm->bPmapEna = 1;

			/* Port Map info from default PMAC header  */
			if ((pmtbl.val[4] >> 6) & 0x1)
				parm->bPmapDefault = 1;
		}

		/* Class Info from default PMAC header */
		if ((pmtbl.val[4] >> 5) & 0x1)
			parm->bClassDefault = 1;

		/* Error set packets to be discarded  */
		if ((pmtbl.val[4] >> 7) & 0x1)
			parm->bErrPktsDisc = 1;
	}

	ret = GSW_statusOk;

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_pmac);
#endif
	return ret;
}

int GSW_PMAC_EG_CfgSet(void *cdev, GSW_PMAC_Eg_Cfg_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	pmtbl_prog_t pmtbl;
	u32 regval, pmac_ctrl4_off;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_pmac);
#endif

	pmac_ctrl4_off = pmac_addr_off(PMAC_CTRL_4_FLAGEN_OFFSET, parm->nPmacId);

	if (IS_VRSN_30_31(gswdev->gipver)) {
		memset(&pmtbl, 0, sizeof(pmtbl));

		pmtbl.pmacId = parm->nPmacId;

		pmtbl.ptaddr = parm->nDestPortId & 0x0F;
		pmtbl.ptaddr |= (parm->nFlowIDMsb & 0x03) << 8;

		if (gswdev->gipver == LTQ_GSWIP_3_0) {
			gsw_r32(cdev, pmac_ctrl4_off,
				PMAC_CTRL_4_FLAGEN_SHIFT,
				PMAC_CTRL_4_FLAGEN_SIZE,
				&regval);
		}

		if (IS_VRSN_31(gswdev->gipver)) {
			gsw_r32(cdev, pmac_ctrl4_off,
				PMAC_CTRL_4_FLAGEN_SHIFT,
				PMAC_CTRL_4_GSWIP3_1_FLAGEN_SIZE,
				&regval);
		}

		/*if (parm->bTCEnable == 1) { */

		if ((parm->bProcFlagsSelect == 0) && (regval == 0)) {
			/*regval 00 tc use traffic class as part of the adress
			  (bit 7 to bit 4)*/

			pmtbl.ptaddr |= (parm->nTrafficClass & 0x0F) << 4;
		} else if ((parm->bProcFlagsSelect == 1) && (regval == 1)) {
			/* regval 01 Flag use processing flags as part of the address
			   (bit 7 to bit 4)*/

			/* MPE-1 Marked Flag  */
			if (parm->bMpe1Flag)
				pmtbl.ptaddr |= (1 << 4);

			/* MPE-2 Marked Flag  */
			if (parm->bMpe2Flag)
				pmtbl.ptaddr |= (1 << 5);

			/* Cryptography Encryption Action Flag  */
			if (parm->bEncFlag)
				pmtbl.ptaddr |= (1 << 6);

			/* Cryptography Decryption Action Flag  */
			if (parm->bDecFlag)
				pmtbl.ptaddr |= (1 << 7);
		} else if ((parm->bProcFlagsSelect == 1) && (regval == 2) &&
			   (IS_VRSN_31(gswdev->gipver))) {
			/*regval 10 Mixed use MPE flags as part of the address (bit 5 to 4)
			  and reduced traffic class as part of the address
			  (bit 7 to bit 6)*/

			pmtbl.ptaddr |= (parm->nTrafficClass & 0x3) << 6;

			/* MPE-1 Marked Flag  */
			if (parm->bMpe1Flag)
				pmtbl.ptaddr |= (1 << 4);

			/* MPE-2 Marked Flag  */
			if (parm->bMpe2Flag)
				pmtbl.ptaddr |= (1 << 5);
		} else {
			ret = GSW_statusNoSupport;
			goto UNLOCK_AND_RETURN;
		}

		if (gswdev->gipver == LTQ_GSWIP_3_0) {
			/*nRes2DW0 -- DW0 (bit 14 to 13),*/
			/*nRes1DW0 -- DW0 (bit 31 to 29), nResDW1 -- DW1 (bit 7 to 4)*/
			pmtbl.val[0] = ((parm->nRes2DW0 & 0x3) |
					((parm->nRes1DW0 & 0x3) << 2) |
					((parm->nResDW1 & 0x0F) << 5));
		}

		if (IS_VRSN_31(gswdev->gipver)) {
			/*DES RES DW) Bit 31 (For Redir bit set) BIT 4 of VAL 0*/
			if (parm->bRedirEnable)
				pmtbl.val[0] |= (1 << 4);

			/* Redirection bit can be overwritten by nRes1DW0 */
			if (parm->bRes1DW0Enable) {
				/*nRes1DW0 -- DW0 (bit 31 to 29)*/
				pmtbl.val[0] &= ~(0x7 << 2);
				pmtbl.val[0] |= ((parm->nRes1DW0 & 0x7) << 2);
			}

			if (parm->bRes2DW0Enable) {
				/*nRes2DW0 -- DW0 (bit 14 to 13),*/
				pmtbl.val[0] &= ~(0x3);
				pmtbl.val[0] |= (parm->nRes2DW0 & 0x3);
			}

			/*BSL Bit 2 to 0 -- VLA 0 BIT 7:5*/
			pmtbl.val[0] |= ((parm->nBslTrafficClass & 0x3) << 5);
			pmtbl.val[0] |= (parm->bBslSegmentDisable << 7);
		}

		/* Receive DMA Channel Identifier (0..7) */
		pmtbl.val[1] = (parm->nRxDmaChanId & 0x0F);

		/* Packet has PMAC */
		if (parm->bPmacEna == 1)
			pmtbl.val[2] |= (1 << 0);

		/* Packet has FCS */
		if (parm->bFcsEna == 1)
			pmtbl.val[2] |= (1 << 1);

		/* To remove L2 header & additional bytes  */
		if (parm->bRemL2Hdr == 1) {
			pmtbl.val[2] |= (1 << 2);
			/* No. of bytes to be removed after Layer-2 Header,*/
			/*valid when bRemL2Hdr is set  */
			pmtbl.val[2] |= ((parm->numBytesRem & 0xFF) << 8);
		}

		pmtbl.ptcaddr	= PMAC_EGCFG_INDEX;
		xwayflow_pmac_table_write(cdev, &pmtbl);
	}

	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_pmac);
#endif
	return ret;

}

int GSW_PMAC_EG_CfgGet(void *cdev, GSW_PMAC_Eg_Cfg_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	pmtbl_prog_t pmtbl;
	u32 regval, pmac_ctrl4_off;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_pmac);
#endif

	pmac_ctrl4_off = pmac_addr_off(PMAC_CTRL_4_FLAGEN_OFFSET, parm->nPmacId);

	if (IS_VRSN_30_31(gswdev->gipver)) {
		memset(&pmtbl, 0, sizeof(pmtbl));

		pmtbl.pmacId = parm->nPmacId;

		pmtbl.ptaddr = parm->nDestPortId & 0x0F;
		pmtbl.ptaddr |= (parm->nFlowIDMsb & 0x03) << 8;

		if (gswdev->gipver == LTQ_GSWIP_3_0) {
			gsw_r32(cdev, pmac_ctrl4_off,
				PMAC_CTRL_4_FLAGEN_SHIFT,
				PMAC_CTRL_4_FLAGEN_SIZE,
				&regval);
		}

		if (IS_VRSN_31(gswdev->gipver)) {
			gsw_r32(cdev, pmac_ctrl4_off,
				PMAC_CTRL_4_FLAGEN_SHIFT,
				PMAC_CTRL_4_GSWIP3_1_FLAGEN_SIZE,
				&regval);
		}


		/*		if (parm->bTCEnable == 1) { */
		if ((parm->bProcFlagsSelect == 0) && (regval == 0)) {
			pmtbl.ptaddr |= (parm->nTrafficClass & 0x0F) << 4;
		} else if ((parm->bProcFlagsSelect == 1) && (regval == 1)) {
			/* MPE-1 Marked Flag  */
			if (parm->bMpe1Flag)
				pmtbl.ptaddr |= (1 << 4);

			/* MPE-2 Marked Flag  */
			if (parm->bMpe2Flag)
				pmtbl.ptaddr |= (1 << 5);

			/* Cryptography Encryption Action Flag  */
			if (parm->bEncFlag)
				pmtbl.ptaddr |= (1 << 6);

			/* Cryptography Decryption Action Flag  */
			if (parm->bDecFlag)
				pmtbl.ptaddr |= (1 << 7);
		} else if ((parm->bProcFlagsSelect == 1) && (regval == 2) &&
			   (IS_VRSN_31(gswdev->gipver))) {
			pmtbl.ptaddr |= (parm->nTrafficClass & 0x3) << 6;

			/* MPE-1 Marked Flag  */
			if (parm->bMpe1Flag)
				pmtbl.ptaddr |= (1 << 4);

			/* MPE-2 Marked Flag  */
			if (parm->bMpe2Flag)
				pmtbl.ptaddr |= (1 << 5);
		} else {
			ret = GSW_statusNoSupport;
			goto UNLOCK_AND_RETURN;
		}

		pmtbl.ptcaddr	= PMAC_EGCFG_INDEX;
		xwayflow_pmac_table_read(cdev, &pmtbl);

		/*  nRes2DW0 -- DW0 (bit 14 to 13),*/
		/*nRes1DW0 -- DW0 (bit 31 to 29), nResDW1 -- DW1 (bit 7 to 4)*/
		if (gswdev->gipver == LTQ_GSWIP_3_0) {
			parm->nRes2DW0 = pmtbl.val[0] & 0x3;
			parm->nRes1DW0 = (pmtbl.val[0] >> 2) & 0x3;
			parm->nResDW1 = (pmtbl.val[0] >> 5) & 0x0F;
		}

		if (IS_VRSN_31(gswdev->gipver)) {
			/*Redir bit set*/
			parm->nRes2DW0 = pmtbl.val[0] & 0x3;
			parm->nRes1DW0 = ((pmtbl.val[0] >> 2) & 0x7);
			parm->bRedirEnable = ((pmtbl.val[0] & 0x10) >> 4);

			parm->nBslTrafficClass = ((pmtbl.val[0] & 0x60) >> 5);
			parm->bBslSegmentDisable = ((pmtbl.val[0] & 0x80) >> 7);
		}

		/* Receive DMA Channel Identifier (0..7) */
		parm->nRxDmaChanId = (pmtbl.val[1] & 0x0F);

		/* Packet has PMAC */
		if ((pmtbl.val[2] >> 0) & 0x1)
			parm->bPmacEna = 1;

		/* Packet has FCS */
		if ((pmtbl.val[2] >> 1) & 0x1)
			parm->bFcsEna = 1;

		/* To remove L2 header & additional bytes  */
		if ((pmtbl.val[2] >> 2) & 0x1) {
			parm->bRemL2Hdr = 1;
			/* No. of bytes to be removed after Layer-2 Header */
			/* valid when bRemL2Hdr is set  */
			parm->numBytesRem = ((pmtbl.val[2] >> 8) & 0xFF);
		}
	}

	ret  = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_pmac);
#endif
	return ret;
}

#if defined(CONFIG_LTQ_8021X) && CONFIG_LTQ_8021X
GSW_return_t GSW_8021X_EAPOL_RuleGet(void *cdev, GSW_8021X_EAPOL_Rule_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	stp8021x_t *scfg = &gswdev->stpconfig;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_pce);
#endif
	parm->eForwardPort = scfg->sfport;
	parm->nForwardPortId = scfg->fpid8021x;
	ret = GSW_statusOk;

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_pce);
#endif
	return ret;
}

GSW_return_t GSW_8021X_EAPOL_RuleSet(void *cdev, GSW_8021X_EAPOL_Rule_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	stp8021x_t *scfg = &gswdev->stpconfig;
	static GSW_PCE_rule_t pcrule;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_pce);
#endif

	scfg->sfport = parm->eForwardPort;
	scfg->fpid8021x = parm->nForwardPortId;
	memset(&pcrule, 0, sizeof(GSW_PCE_rule_t));
	pcrule.pattern.nIndex = EAPOL_PCE_RULE_INDEX;
	pcrule.pattern.bEnable = 1;
	pcrule.pattern.bMAC_DstEnable	= 1;
	pcrule.pattern.nMAC_Dst[0] = 0x01;
	pcrule.pattern.nMAC_Dst[1] = 0x80;
	pcrule.pattern.nMAC_Dst[2] = 0xC2;
	pcrule.pattern.nMAC_Dst[3] = 0x00;
	pcrule.pattern.nMAC_Dst[4] = 0x00;
	pcrule.pattern.nMAC_Dst[5] = 0x03;
	pcrule.pattern.nMAC_Src[5] = 0;
	pcrule.pattern.bEtherTypeEnable	= 1;
	pcrule.pattern.nEtherType	= 0x888E;
	pcrule.action.eCrossStateAction = GSW_PCE_ACTION_CROSS_STATE_CROSS;

	if ((scfg->sfport < 4) && (scfg->sfport > 0))
		pcrule.action.ePortMapAction = scfg->sfport + 1;
	else
		pr_err("(Incorrect forward port action) %s:%s:%d\n",
		       __FILE__, __func__, __LINE__);

	/*TODO*/
	//Govind - Can occur out of array bounds problem.
	pcrule.action.nForwardPortMap[0] = (1 << parm->nForwardPortId);

	/* We prepare everything and write into PCE Table */
	if (0 != pce_rule_write(cdev, &gswdev->phandler, &pcrule)) {
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_pce);
#endif
	return ret;

}

GSW_return_t GSW_8021X_PortCfgGet(void *cdev, GSW_8021X_portCfg_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 ret;
	port_config_t *pcfg;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return  GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_pce);
#endif

	if (IS_VRSN_NOT_31(gswdev->gipver)) {
		if (parm->nPortId >= gswdev->tpnum) {
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}

		pcfg = &gswdev->pconfig[parm->nPortId];
		parm->eState = pcfg->p8021xs;
	}

	if (IS_VRSN_31(gswdev->gipver)) {
		if (parm->nPortId >= gswdev->num_of_bridge_port) {
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}

		/*If Bridge Port ID is valid,Check whether it is InUSE
		  if not InUse,return ERROR*/
		if (!(gswdev->brdgeportconfig_idx[parm->nPortId].IndexInUse)) {
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}

		parm->eState = gswdev->brdgeportconfig_idx[parm->nPortId].P8021xState;
	}


	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_pce);
#endif
	return ret;


}

GSW_return_t GSW_8021X_PortCfgSet(void *cdev, GSW_8021X_portCfg_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u8 StpState, P8021xState;
	u32 ret;

	port_config_t *pcfg;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_pce);
#endif

	if (IS_VRSN_NOT_31(gswdev->gipver)) {
		if (parm->nPortId >= gswdev->tpnum) {
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}

		pcfg = &gswdev->pconfig[parm->nPortId];
		pcfg->p8021xs	= parm->eState;
		set_port_state(cdev, parm->nPortId, pcfg->pcstate,
			       pcfg->p8021xs);
	}

	if (IS_VRSN_31(gswdev->gipver)) {
		if (parm->nPortId >= gswdev->num_of_bridge_port) {
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}

		/*If Bridge Port ID is valid,Check whether it is InUSE
		  if not InUse,return ERROR*/
		if (!(gswdev->brdgeportconfig_idx[parm->nPortId].IndexInUse)) {
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}

		P8021xState = parm->eState;
		StpState = gswdev->brdgeportconfig_idx[parm->nPortId].StpState;
		gswdev->brdgeportconfig_idx[parm->nPortId].P8021xState = P8021xState;

		/* Config the Table */
		set_port_state(cdev, parm->nPortId, StpState, P8021xState);
	}

	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_pce);
#endif
	return ret;
}

#endif /*CONFIG_LTQ_8021X  */
GSW_return_t GSW_CPU_PortCfgGet(void *cdev, GSW_CPU_PortCfg_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u8 pidx = parm->nPortId;
	u32 value;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_misc);
#endif

	if (parm->nPortId >= gswdev->tpnum) {
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	if (pidx ==  gswdev->cport)
		parm->bCPU_PortValid = 1;
	else
		parm->bCPU_PortValid = 0;

	/* Special Tag Egress*/
	gsw_r32(cdev, (FDMA_PCTRL_STEN_OFFSET + (0x6 * pidx)),
		FDMA_PCTRL_STEN_SHIFT, FDMA_PCTRL_STEN_SIZE, &value);
	parm->bSpecialTagEgress = value;
	/* Special Tag Igress*/
	gsw_r32(cdev, (PCE_PCTRL_0_IGSTEN_OFFSET + (0xa * pidx)),
		PCE_PCTRL_0_IGSTEN_SHIFT, PCE_PCTRL_0_IGSTEN_SIZE, &value);
	parm->bSpecialTagIngress = value;
	/* FCS Check */
	gsw_r32(cdev, (SDMA_PCTRL_FCSIGN_OFFSET + (0x6 * pidx)),
		SDMA_PCTRL_FCSIGN_SHIFT,
		SDMA_PCTRL_FCSIGN_SIZE, &value);
	parm->bFcsCheck = value;

	/* FCS Generate */
	if (IS_VRSN_30_31(gswdev->gipver)) {

		if (gswdev->gipver == LTQ_GSWIP_3_0) {
			if (pidx != 0) {
				gsw_r32(cdev, (MAC_CTRL_0_FCS_OFFSET + (0xC * (pidx - 1))),
					MAC_CTRL_0_FCS_SHIFT, MAC_CTRL_0_FCS_SIZE, &value);
				parm->bFcsGenerate = value;
			}
		}

		if (IS_VRSN_31(gswdev->gipver)) {
			if ((1 < pidx) && (pidx < 5)) {
#if defined(CONFIG_MAC) && CONFIG_MAC
				/* MAC API's to set FCS Generation */
				struct mac_ops *ops = get_mac_ops(gswdev, pidx);

				if (!ops) {
					pr_err("MAC %d is not initialized\n", pidx);
					ret = GSW_statusErr;
					goto UNLOCK_AND_RETURN;
				}

				parm->bFcsTxOps = ops->get_fcsgen(ops);
#endif
			}
		}

		gsw_r32(cdev, FDMA_PASR_CPU_OFFSET,	FDMA_PASR_CPU_SHIFT,
			FDMA_PASR_CPU_SIZE, &value);
		parm->eNoMPEParserCfg = value;
		gsw_r32(cdev, FDMA_PASR_MPE1_OFFSET,	FDMA_PASR_MPE1_SHIFT,
			FDMA_PASR_MPE1_SIZE, &value);
		parm->eMPE1ParserCfg = value;
		gsw_r32(cdev, FDMA_PASR_MPE2_OFFSET,	FDMA_PASR_MPE2_SHIFT,
			FDMA_PASR_MPE2_SIZE, &value);
		parm->eMPE2ParserCfg = value;
		gsw_r32(cdev, FDMA_PASR_MPE3_OFFSET,	FDMA_PASR_MPE3_SHIFT,
			FDMA_PASR_MPE3_SIZE, &value);
		parm->eMPE1MPE2ParserCfg = value;
	} else {
		gsw_r32(cdev, (MAC_CTRL_0_FCS_OFFSET + (0xC * pidx)),
			MAC_CTRL_0_FCS_SHIFT, MAC_CTRL_0_FCS_SIZE, &value);
		parm->bFcsGenerate = value;
	}

	if (gswdev->gipver == LTQ_GSWIP_3_0) {
		gsw_r32(cdev, (FDMA_PCTRL_ST_TYPE_OFFSET + (0x6 * pidx)),
			FDMA_PCTRL_ST_TYPE_SHIFT, FDMA_PCTRL_ST_TYPE_SIZE, &value);
	}

	parm->bSpecialTagEthType = value;
	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_misc);
#endif
	return ret;


}

GSW_return_t GSW_CPU_PortCfgSet(void *cdev, GSW_CPU_PortCfg_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u8 pidx = parm->nPortId;
	u32 RST, AS, AST, RXSH;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_misc);
#endif

	if (parm->nPortId >= gswdev->tpnum) {
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	if (pidx == gswdev->cport)
		parm->bCPU_PortValid = 1;
	else
		parm->bCPU_PortValid = 0;

	gswdev->mpnum = pidx;
	/* Special Tag Egress*/
	gsw_w32(cdev, (FDMA_PCTRL_STEN_OFFSET + (0x6 * pidx)),
		FDMA_PCTRL_STEN_SHIFT,
		FDMA_PCTRL_STEN_SIZE, parm->bSpecialTagEgress);

	/* xRX CPU port */
	if ((pidx == gswdev->cport) && !(gswdev->gipver == LTQ_GSWIP_3_0)) {
		if (parm->bSpecialTagEgress == 0) {
			RST = 1;
			AS = 0;
		} else {
			RST = 0;
			AS = 1;
		}

		gsw_w32(cdev, (PMAC_HD_CTL_RST_OFFSET + GSW_TREG_OFFSET),
			PMAC_HD_CTL_RST_SHIFT, PMAC_HD_CTL_RST_SIZE, RST);
		gsw_w32(cdev, (PMAC_HD_CTL_AS_OFFSET + GSW_TREG_OFFSET),
			PMAC_HD_CTL_AS_SHIFT, PMAC_HD_CTL_AS_SIZE, AS);
	}

	/* Special Tag Igress*/
	gsw_w32(cdev, (PCE_PCTRL_0_IGSTEN_OFFSET + (0xa * pidx)),
		PCE_PCTRL_0_IGSTEN_SHIFT,
		PCE_PCTRL_0_IGSTEN_SIZE, parm->bSpecialTagIngress);

	if ((pidx == gswdev->cport) && !(gswdev->gipver == LTQ_GSWIP_3_0)) {
		if (parm->bSpecialTagIngress == 0) {
			AST = 0;
			RXSH = 0;
		} else {
			AST = 1;
			RXSH = 1;
		}

		gsw_w32(cdev, (PMAC_HD_CTL_AST_OFFSET + GSW_TREG_OFFSET),
			PMAC_HD_CTL_AST_SHIFT, PMAC_HD_CTL_AST_SIZE, AST);
		gsw_w32(cdev, (PMAC_HD_CTL_RXSH_OFFSET + GSW_TREG_OFFSET),
			PMAC_HD_CTL_RXSH_SHIFT, PMAC_HD_CTL_RXSH_SIZE, RXSH);
		/* FCS Generate */
		gsw_w32(cdev, (MAC_CTRL_0_FCS_OFFSET + (0xC * pidx)),
			MAC_CTRL_0_FCS_SHIFT, MAC_CTRL_0_FCS_SIZE, parm->bFcsGenerate);
	}

	if (IS_VRSN_30_31(gswdev->gipver)) {
		/* FCS Generate */
		if (gswdev->gipver == LTQ_GSWIP_3_0) {
			if (pidx != 0) {
				gsw_w32(cdev, (MAC_CTRL_0_FCS_OFFSET + (0xC * (pidx - 1))),
					MAC_CTRL_0_FCS_SHIFT, MAC_CTRL_0_FCS_SIZE, parm->bFcsGenerate);
			}
		}

		if (IS_VRSN_31(gswdev->gipver)) {
			if ((1 < pidx) && (pidx < 5)) {
#if defined(CONFIG_MAC) && CONFIG_MAC
				/* MAC API's to set FCS Generation */
				struct mac_ops *ops = get_mac_ops(gswdev, pidx);

				if (!ops) {
					pr_err("MAC %d is not initialized\n", pidx);
					ret = GSW_statusErr;
					goto UNLOCK_AND_RETURN;
				}

				ops->set_fcsgen(ops, parm->bFcsTxOps);
#endif
			}
		}

		switch (parm->eNoMPEParserCfg) {
		case GSW_CPU_PARSER_NIL:
			gsw_w32(cdev, FDMA_PASR_CPU_OFFSET,	FDMA_PASR_CPU_SHIFT,
				FDMA_PASR_CPU_SIZE, 0);
			break;

		case GSW_CPU_PARSER_FLAGS:
			gsw_w32(cdev, FDMA_PASR_CPU_OFFSET,	FDMA_PASR_CPU_SHIFT,
				FDMA_PASR_CPU_SIZE, 1);
			break;

		case GSW_CPU_PARSER_OFFSETS_FLAGS:
			gsw_w32(cdev, FDMA_PASR_CPU_OFFSET,	FDMA_PASR_CPU_SHIFT,
				FDMA_PASR_CPU_SIZE, 2);
			break;

		case GSW_CPU_PARSER_RESERVED:
			gsw_w32(cdev, FDMA_PASR_CPU_OFFSET,	FDMA_PASR_CPU_SHIFT,
				FDMA_PASR_CPU_SIZE, 3);
			break;
		}

		switch (parm->eMPE1ParserCfg) {
		case GSW_CPU_PARSER_NIL:
			gsw_w32(cdev, FDMA_PASR_MPE1_OFFSET,	FDMA_PASR_MPE1_SHIFT,
				FDMA_PASR_MPE1_SIZE, 0);
			break;

		case GSW_CPU_PARSER_FLAGS:
			gsw_w32(cdev, FDMA_PASR_MPE1_OFFSET,	FDMA_PASR_MPE1_SHIFT,
				FDMA_PASR_MPE1_SIZE, 1);
			break;

		case GSW_CPU_PARSER_OFFSETS_FLAGS:
			gsw_w32(cdev, FDMA_PASR_MPE1_OFFSET,	FDMA_PASR_MPE1_SHIFT,
				FDMA_PASR_MPE1_SIZE, 2);
			break;

		case GSW_CPU_PARSER_RESERVED:
			gsw_w32(cdev, FDMA_PASR_MPE1_OFFSET,	FDMA_PASR_MPE1_SHIFT,
				FDMA_PASR_MPE1_SIZE, 3);
			break;
		}

		switch (parm->eMPE2ParserCfg) {
		case GSW_CPU_PARSER_NIL:
			gsw_w32(cdev, FDMA_PASR_MPE2_OFFSET,	FDMA_PASR_MPE2_SHIFT,
				FDMA_PASR_MPE2_SIZE, 0);
			break;

		case GSW_CPU_PARSER_FLAGS:
			gsw_w32(cdev, FDMA_PASR_MPE2_OFFSET,	FDMA_PASR_MPE2_SHIFT,
				FDMA_PASR_MPE2_SIZE, 1);
			break;

		case GSW_CPU_PARSER_OFFSETS_FLAGS:
			gsw_w32(cdev, FDMA_PASR_MPE2_OFFSET,	FDMA_PASR_MPE2_SHIFT,
				FDMA_PASR_MPE2_SIZE, 2);
			break;

		case GSW_CPU_PARSER_RESERVED:
			gsw_w32(cdev, FDMA_PASR_MPE2_OFFSET,	FDMA_PASR_MPE2_SHIFT,
				FDMA_PASR_MPE2_SIZE, 3);
			break;
		}

		switch (parm->eMPE1MPE2ParserCfg) {
		case GSW_CPU_PARSER_NIL:
			gsw_w32(cdev, FDMA_PASR_MPE3_OFFSET,	FDMA_PASR_MPE3_SHIFT,
				FDMA_PASR_MPE3_SIZE, 0);
			break;

		case GSW_CPU_PARSER_FLAGS:
			gsw_w32(cdev, FDMA_PASR_MPE3_OFFSET,	FDMA_PASR_MPE3_SHIFT,
				FDMA_PASR_MPE3_SIZE, 1);
			break;

		case GSW_CPU_PARSER_OFFSETS_FLAGS:
			gsw_w32(cdev, FDMA_PASR_MPE3_OFFSET,	FDMA_PASR_MPE3_SHIFT,
				FDMA_PASR_MPE3_SIZE, 2);
			break;

		case GSW_CPU_PARSER_RESERVED:
			gsw_w32(cdev, FDMA_PASR_MPE3_OFFSET,	FDMA_PASR_MPE3_SHIFT,
				FDMA_PASR_MPE3_SIZE, 3);
			break;
		}
	}

	/* FCS Check */
	gsw_w32(cdev, (SDMA_PCTRL_FCSIGN_OFFSET + (0x6 * pidx)),
		SDMA_PCTRL_FCSIGN_SHIFT,
		SDMA_PCTRL_FCSIGN_SIZE, parm->bFcsCheck);

	if (gswdev->gipver == LTQ_GSWIP_3_0) {
		if (parm->bSpecialTagEthType == GSW_CPU_ETHTYPE_FLOWID) {
			gsw_w32(cdev, (FDMA_PCTRL_ST_TYPE_OFFSET + (0x6 * pidx)),
				FDMA_PCTRL_ST_TYPE_SHIFT, FDMA_PCTRL_ST_TYPE_SIZE, 1);
		} else {
			gsw_w32(cdev, (FDMA_PCTRL_ST_TYPE_OFFSET + (0x6 * pidx)),
				FDMA_PCTRL_ST_TYPE_SHIFT, FDMA_PCTRL_ST_TYPE_SIZE, 0);
		}
	}

	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_misc);
#endif
	return ret;
}

GSW_return_t GSW_CPU_PortExtendCfgGet(void *cdev, GSW_CPU_PortExtendCfg_t *parm)
{
	u32  value, value_add, value_vlan;
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_misc);
#endif

	if (IS_VRSN_30_31(gswdev->gipver)) {
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	} else {
		gsw_r32(cdev,
			(PMAC_HD_CTL_ADD_OFFSET + GSW_TREG_OFFSET),
			PMAC_HD_CTL_ADD_SHIFT,
			PMAC_HD_CTL_ADD_SIZE, &value_add);
		gsw_r32(cdev,
			(PMAC_HD_CTL_TAG_OFFSET + GSW_TREG_OFFSET),
			PMAC_HD_CTL_TAG_SHIFT,
			PMAC_HD_CTL_TAG_SIZE, &value_vlan);
	}

	if (value_add == 0 && value_vlan == 0)
		parm->eHeaderAdd = 0;
	else if (value_add == 1 && value_vlan == 0)
		parm->eHeaderAdd = 1;
	else if (value_add == 1 && value_vlan == 1)
		parm->eHeaderAdd = 2;
	else
		parm->eHeaderAdd = 0;

	gsw_r32(cdev, (PMAC_HD_CTL_RL2_OFFSET + GSW_TREG_OFFSET),
		PMAC_HD_CTL_RL2_SHIFT, PMAC_HD_CTL_RL2_SIZE, &value);
	parm->bHeaderRemove = value;
	memset(&parm->sHeader, 0, sizeof(GSW_CPU_Header_t));

	if (value_add == 1) {
		/* Output the Src MAC */
		gsw_r32(cdev,
			(PMAC_SA3_SA_15_0_OFFSET + GSW_TREG_OFFSET),
			PMAC_SA3_SA_15_0_SHIFT,
			PMAC_SA3_SA_15_0_SIZE, &value);
		parm->sHeader.nMAC_Src[0] = value & 0xFF;
		parm->sHeader.nMAC_Src[1] = ((value >> 8) & 0xFF);
		gsw_r32(cdev,
			(PMAC_SA2_SA_31_16_OFFSET + GSW_TREG_OFFSET),
			PMAC_SA2_SA_31_16_SHIFT,
			PMAC_SA2_SA_31_16_SIZE, &value);
		parm->sHeader.nMAC_Src[2] = value & 0xFF;
		parm->sHeader.nMAC_Src[3] = ((value >> 8) & 0xFF);
		gsw_r32(cdev,
			(PMAC_SA1_SA_47_32_OFFSET + GSW_TREG_OFFSET),
			PMAC_SA1_SA_47_32_SHIFT,
			PMAC_SA1_SA_47_32_SIZE, &value);
		parm->sHeader.nMAC_Src[4] = value & 0xFF;
		parm->sHeader.nMAC_Src[5] = ((value >> 8) & 0xFF);
		/* Output the Dst MAC */
		gsw_r32(cdev,
			(PMAC_DA3_DA_15_0_OFFSET + GSW_TREG_OFFSET),
			PMAC_DA3_DA_15_0_SHIFT, PMAC_DA3_DA_15_0_SIZE, &value);
		parm->sHeader.nMAC_Dst[0] = value & 0xFF;
		parm->sHeader.nMAC_Dst[1] = ((value >> 8) & 0xFF);
		gsw_r32(cdev,
			(PMAC_DA2_DA_31_16_OFFSET + GSW_TREG_OFFSET),
			PMAC_DA2_DA_31_16_SHIFT,
			PMAC_DA2_DA_31_16_SIZE, &value);
		parm->sHeader.nMAC_Dst[2] = value & 0xFF;
		parm->sHeader.nMAC_Dst[3] = ((value >> 8) & 0xFF);
		gsw_r32(cdev,
			(PMAC_DA1_SA_47_32_OFFSET + GSW_TREG_OFFSET),
			PMAC_DA1_SA_47_32_SHIFT,
			PMAC_DA1_SA_47_32_SIZE, &value);
		parm->sHeader.nMAC_Dst[4] = value & 0xFF;
		parm->sHeader.nMAC_Dst[5] = ((value >> 8) & 0xFF);
		/* Input the Ethernet Type */
		gsw_r32(cdev,
			(PMAC_TL_TYPE_LEN_OFFSET + GSW_TREG_OFFSET),
			PMAC_TL_TYPE_LEN_SHIFT, PMAC_TL_TYPE_LEN_SIZE, &value);
		parm->sHeader.nEthertype = value;
	}

	if (value_vlan == 1) {
		gsw_r32(cdev, (PMAC_VLAN_PRI_OFFSET + GSW_TREG_OFFSET),
			PMAC_VLAN_PRI_SHIFT, PMAC_VLAN_PRI_SIZE, &value);
		parm->sHeader.nVLAN_Prio = value;
		gsw_r32(cdev, (PMAC_VLAN_CFI_OFFSET + GSW_TREG_OFFSET),
			PMAC_VLAN_CFI_SHIFT, PMAC_VLAN_CFI_SIZE, &value);
		parm->sHeader.nVLAN_CFI = value;
		gsw_r32(cdev,
			(PMAC_VLAN_VLAN_ID_OFFSET + GSW_TREG_OFFSET),
			PMAC_VLAN_VLAN_ID_SHIFT,
			PMAC_VLAN_VLAN_ID_SIZE, &value);
		parm->sHeader.nVLAN_ID = value;
	}

	gsw_r32(cdev, (PMAC_HD_CTL_FC_OFFSET + GSW_TREG_OFFSET),
		PMAC_HD_CTL_FC_SHIFT, PMAC_HD_CTL_FC_SIZE, &value);
	parm->ePauseCtrl = value;
	gsw_r32(cdev, (PMAC_HD_CTL_RC_OFFSET + GSW_TREG_OFFSET),
		PMAC_HD_CTL_RC_SHIFT, PMAC_HD_CTL_RC_SIZE, &value);
	parm->bFcsRemove = value;
	gsw_r32(cdev, (PMAC_EWAN_EWAN_OFFSET + GSW_TREG_OFFSET),
		PMAC_EWAN_EWAN_SHIFT, PMAC_EWAN_EWAN_SIZE, &value);
	parm->nWAN_Ports = value;
	ret = GSW_statusOk;


UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_misc);
#endif
	return ret;
}

GSW_return_t GSW_CPU_PortExtendCfgSet(void *cdev, GSW_CPU_PortExtendCfg_t *parm)
{
	u32 value_add = 0, value_vlan = 0;
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_misc);
#endif

	if (IS_VRSN_30_31(gswdev->gipver)) {
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	switch (parm->eHeaderAdd) {
	case GSW_CPU_HEADER_NO:
		value_add = 0;
		value_vlan = 0;
		break;

	case GSW_CPU_HEADER_MAC:
		value_add = 1;
		value_vlan = 0;
		break;

	case GSW_CPU_HEADER_VLAN:
		value_add = 1;
		value_vlan = 1;
		break;
	}

	if ((parm->bHeaderRemove == 1)
	    && (parm->eHeaderAdd != GSW_CPU_HEADER_NO)) {
		pr_err("The Header Can't be remove because the Header Add parameter is not 0");
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}  else {
		gsw_w32(cdev, (PMAC_HD_CTL_RL2_OFFSET + GSW_TREG_OFFSET),
			PMAC_HD_CTL_RL2_SHIFT,
			PMAC_HD_CTL_RL2_SIZE, parm->bHeaderRemove);
	}

	gsw_w32(cdev, (PMAC_HD_CTL_ADD_OFFSET + GSW_TREG_OFFSET),
		PMAC_HD_CTL_ADD_SHIFT, PMAC_HD_CTL_ADD_SIZE, value_add);
	gsw_w32(cdev, (PMAC_HD_CTL_TAG_OFFSET + GSW_TREG_OFFSET),
		PMAC_HD_CTL_TAG_SHIFT, PMAC_HD_CTL_TAG_SIZE, value_vlan);

	if (parm->eHeaderAdd == GSW_CPU_HEADER_MAC) {
		u32 macdata;
		/* Input the Src MAC */
		macdata = ((parm->sHeader.nMAC_Src[0])
			   | (parm->sHeader.nMAC_Src[1] << 8));
		gsw_w32(cdev,
			(PMAC_SA3_SA_15_0_OFFSET + GSW_TREG_OFFSET),
			PMAC_SA3_SA_15_0_SHIFT,
			PMAC_SA3_SA_15_0_SIZE, macdata);
		macdata = (parm->sHeader.nMAC_Src[2]
			   | parm->sHeader.nMAC_Src[3] << 8);
		gsw_w32(cdev,
			(PMAC_SA2_SA_31_16_OFFSET + GSW_TREG_OFFSET),
			PMAC_SA2_SA_31_16_SHIFT,
			PMAC_SA2_SA_31_16_SIZE, macdata);
		macdata = (parm->sHeader.nMAC_Src[4]
			   | parm->sHeader.nMAC_Src[5] << 8);
		gsw_w32(cdev,
			(PMAC_SA1_SA_47_32_OFFSET + GSW_TREG_OFFSET),
			PMAC_SA1_SA_47_32_SHIFT,
			PMAC_SA1_SA_47_32_SIZE, macdata);
		/* Input the Dst MAC */
		macdata = (parm->sHeader.nMAC_Dst[0]
			   | parm->sHeader.nMAC_Dst[1] << 8);
		gsw_w32(cdev,
			(PMAC_DA3_DA_15_0_OFFSET + GSW_TREG_OFFSET),
			PMAC_DA3_DA_15_0_SHIFT,
			PMAC_DA3_DA_15_0_SIZE, macdata);
		macdata = (parm->sHeader.nMAC_Dst[2]
			   | parm->sHeader.nMAC_Dst[3] << 8);
		gsw_w32(cdev,
			(PMAC_DA2_DA_31_16_OFFSET + GSW_TREG_OFFSET),
			PMAC_DA2_DA_31_16_SHIFT,
			PMAC_DA2_DA_31_16_SIZE, macdata);
		macdata = ((parm->sHeader.nMAC_Dst[4])
			   | (parm->sHeader.nMAC_Dst[5] << 8));
		gsw_w32(cdev,
			(PMAC_DA1_SA_47_32_OFFSET + GSW_TREG_OFFSET),
			PMAC_DA1_SA_47_32_SHIFT,
			PMAC_DA1_SA_47_32_SIZE, macdata);
		/* Input the Ethernet Type */
		gsw_w32(cdev,
			(PMAC_TL_TYPE_LEN_OFFSET + GSW_TREG_OFFSET),
			PMAC_TL_TYPE_LEN_SHIFT,
			PMAC_TL_TYPE_LEN_SIZE, parm->sHeader.nEthertype);
	}

	if (parm->eHeaderAdd == GSW_CPU_HEADER_VLAN) {
		gsw_w32(cdev, (PMAC_VLAN_PRI_OFFSET + GSW_TREG_OFFSET),
			PMAC_VLAN_PRI_SHIFT,
			PMAC_VLAN_PRI_SIZE, parm->sHeader.nVLAN_Prio);
		gsw_w32(cdev, (PMAC_VLAN_CFI_OFFSET + GSW_TREG_OFFSET),
			PMAC_VLAN_CFI_SHIFT,
			PMAC_VLAN_CFI_SIZE, parm->sHeader.nVLAN_CFI);
		gsw_w32(cdev, (PMAC_VLAN_VLAN_ID_OFFSET + GSW_TREG_OFFSET),
			PMAC_VLAN_VLAN_ID_SHIFT,
			PMAC_VLAN_VLAN_ID_SIZE, parm->sHeader.nVLAN_ID);
	}

	gsw_w32(cdev, (PMAC_HD_CTL_FC_OFFSET + GSW_TREG_OFFSET),
		PMAC_HD_CTL_FC_SHIFT, PMAC_HD_CTL_FC_SIZE, parm->ePauseCtrl);
	gsw_w32(cdev, (PMAC_HD_CTL_RC_OFFSET + GSW_TREG_OFFSET),
		PMAC_HD_CTL_RC_SHIFT, PMAC_HD_CTL_RC_SIZE, parm->bFcsRemove);
	gsw_w32(cdev, (PMAC_EWAN_EWAN_OFFSET + GSW_TREG_OFFSET),
		PMAC_EWAN_EWAN_SHIFT, PMAC_EWAN_EWAN_SIZE, parm->nWAN_Ports);
	ret = GSW_statusOk;


UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_misc);
#endif
	return ret;

}

#if defined(CONFIG_LTQ_WOL) && CONFIG_LTQ_WOL
GSW_return_t GSW_WoL_CfgGet(void *cdev, GSW_WoL_Cfg_t *parm)
{
	u32 value;
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_misc);
#endif

	gsw_r32(cdev, WOL_GLB_CTRL_PASSEN_OFFSET, WOL_GLB_CTRL_PASSEN_SHIFT,
		WOL_GLB_CTRL_PASSEN_SIZE, &value);
	parm->bWolPasswordEnable = value;
	gsw_r32(cdev, WOL_DA_2_DA2_OFFSET, WOL_DA_2_DA2_SHIFT,
		WOL_DA_2_DA2_SIZE, &value);
	parm->nWolMAC[0] = (value >> 8 & 0xFF);
	parm->nWolMAC[1] = (value & 0xFF);
	gsw_r32(cdev, WOL_DA_1_DA1_OFFSET, WOL_DA_1_DA1_SHIFT,
		WOL_DA_1_DA1_SIZE, &value);
	parm->nWolMAC[2] = (value >> 8 & 0xFF);
	parm->nWolMAC[3] = (value & 0xFF);
	gsw_r32(cdev, WOL_DA_0_DA0_OFFSET, WOL_DA_0_DA0_SHIFT,
		WOL_DA_0_DA0_SIZE, &value);
	parm->nWolMAC[4] = (value >> 8 & 0xFF);
	parm->nWolMAC[5] = (value & 0xFF);
	gsw_r32(cdev, WOL_PW_2_PW2_OFFSET, WOL_PW_2_PW2_SHIFT,
		WOL_PW_2_PW2_SIZE, &value);
	parm->nWolPassword[0] = (value >> 8 & 0xFF);
	parm->nWolPassword[1] = (value & 0xFF);
	gsw_r32(cdev, WOL_PW_1_PW1_OFFSET, WOL_PW_1_PW1_SHIFT,
		WOL_PW_1_PW1_SIZE, &value);
	parm->nWolPassword[2] = (value >> 8 & 0xFF);
	parm->nWolPassword[3] = (value & 0xFF);
	gsw_r32(cdev, WOL_PW_0_PW0_OFFSET, WOL_PW_0_PW0_SHIFT,
		WOL_PW_0_PW0_SIZE, &value);
	parm->nWolPassword[4] = (value >> 8 & 0xFF);
	parm->nWolPassword[5] = (value & 0xFF);
	ret = GSW_statusOk;

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_misc);
#endif
	return ret;

}

GSW_return_t GSW_WoL_CfgSet(void *cdev, GSW_WoL_Cfg_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_misc);
#endif

	gsw_w32(cdev, WOL_GLB_CTRL_PASSEN_OFFSET, WOL_GLB_CTRL_PASSEN_SHIFT,
		WOL_GLB_CTRL_PASSEN_SIZE, parm->bWolPasswordEnable);
	gsw_w32(cdev, WOL_DA_2_DA2_OFFSET,
		WOL_DA_2_DA2_SHIFT, WOL_DA_2_DA2_SIZE,
		(((parm->nWolMAC[0] & 0xFF) << 8)
		 | (parm->nWolMAC[1] & 0xFF)));
	gsw_w32(cdev, WOL_DA_1_DA1_OFFSET,
		WOL_DA_1_DA1_SHIFT, WOL_DA_1_DA1_SIZE,
		(((parm->nWolMAC[2] & 0xFF) << 8)
		 | (parm->nWolMAC[3] & 0xFF)));
	gsw_w32(cdev, WOL_DA_0_DA0_OFFSET,
		WOL_DA_0_DA0_SHIFT, WOL_DA_0_DA0_SIZE,
		(((parm->nWolMAC[4] & 0xFF) << 8)
		 | (parm->nWolMAC[5] & 0xFF)));
	gsw_w32(cdev, WOL_PW_2_PW2_OFFSET,
		WOL_PW_2_PW2_SHIFT, WOL_PW_2_PW2_SIZE,
		(((parm->nWolPassword[0] & 0xFF) << 8)
		 | (parm->nWolPassword[1] & 0xFF)));
	gsw_w32(cdev, WOL_PW_1_PW1_OFFSET,
		WOL_PW_1_PW1_SHIFT, WOL_PW_1_PW1_SIZE,
		(((parm->nWolPassword[2] & 0xFF) << 8)
		 | (parm->nWolPassword[3] & 0xFF)));
	gsw_w32(cdev, WOL_PW_0_PW0_OFFSET,
		WOL_PW_0_PW0_SHIFT, WOL_PW_0_PW0_SIZE,
		(((parm->nWolPassword[4] & 0xFF) << 8)
		 | (parm->nWolPassword[5] & 0xFF)));
	ret = GSW_statusOk;

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_misc);
#endif
	return ret;
}

GSW_return_t GSW_WoL_PortCfgGet(void *cdev, GSW_WoL_PortCfg_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 value;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_misc);
#endif

	if (parm->nPortId >= gswdev->tpnum) {
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	gsw_r32(cdev, (WOL_CTRL_PORT_OFFSET + (0xA * parm->nPortId)),
		WOL_CTRL_PORT_SHIFT, WOL_CTRL_PORT_SIZE, &value);
	parm->bWakeOnLAN_Enable = value;

	if (IS_VRSN_31(gswdev->gipver)) {
		gsw_r32(cdev, (WOL_CTRL_ADDRDIS_OFFSET + (0xA * parm->nPortId)),
			WOL_CTRL_ADDRDIS_SHIFT, WOL_CTRL_ADDRDIS_SIZE, &value);
		parm->bIgnoreAdrCheck = value;
	}

	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_misc);
#endif
	return ret;
}

GSW_return_t GSW_WoL_PortCfgSet(void *cdev, GSW_WoL_PortCfg_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_misc);
#endif

	if (parm->nPortId >= gswdev->tpnum) {
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	gsw_w32(cdev, (WOL_CTRL_PORT_OFFSET + (0xA * parm->nPortId)),
		WOL_CTRL_PORT_SHIFT,
		WOL_CTRL_PORT_SIZE, parm->bWakeOnLAN_Enable);

	if (IS_VRSN_31(gswdev->gipver)) {
		gsw_w32(cdev, (WOL_CTRL_ADDRDIS_OFFSET + (0xA * parm->nPortId)),
			WOL_CTRL_ADDRDIS_SHIFT,
			WOL_CTRL_ADDRDIS_SIZE, parm->bIgnoreAdrCheck);
	}

	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_misc);
#endif
	return ret;
}

#endif /* CONFIG_LTQ_WOL */
GSW_return_t GSW_RegisterGet(void *cdev, GSW_register_t *parm)
{
	u32 rvalue, raddr = parm->nRegAddr;
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_misc);
#endif

	gsw_r32(cdev, raddr,	0, 16, &rvalue);
	parm->nData = rvalue;

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_misc);
#endif

	return GSW_statusOk;
}

GSW_return_t GSW_RegisterSet(void *cdev, GSW_register_t *parm)
{
	u32 rvalue = parm->nData, raddr = parm->nRegAddr;
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_misc);
#endif

	gsw_w32(cdev, raddr, 0, 16, rvalue);

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_misc);
#endif

	return GSW_statusOk;
}

GSW_return_t GSW_IrqGet(void *cdev, GSW_irq_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_misc);
#endif

	if (parm->nPortId >= gswdev->tpnum) {
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	/* ToDo: Require future clarify for how to display */
	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_misc);
#endif
	return ret;
}

GSW_return_t GSW_IrqMaskGet(void *cdev, GSW_irq_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u8 pidx = parm->nPortId;
	u32 value;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_misc);
#endif

	if (parm->nPortId >= gswdev->tpnum) {
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	if (parm->eIrqSrc == GSW_IRQ_WOL) {
		gsw_r32(cdev, (PCE_PIER_WOL_OFFSET + (0xA * pidx)),
			PCE_PIER_WOL_SHIFT, PCE_PIER_WOL_SIZE, &value);
	} else if (parm->eIrqSrc == GSW_IRQ_LIMIT_ALERT) {
		gsw_r32(cdev, (PCE_PIER_LOCK_OFFSET + (0xA * pidx)),
			PCE_PIER_LOCK_SHIFT, PCE_PIER_LOCK_SIZE, &value);
	} else if (parm->eIrqSrc == GSW_IRQ_LOCK_ALERT) {
		gsw_r32(cdev, (PCE_PIER_LIM_OFFSET + (0xA * pidx)),
			PCE_PIER_LIM_SHIFT, PCE_PIER_LIM_SIZE, &value);
	}

	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_misc);
#endif
	return ret;

}

GSW_return_t GSW_IrqMaskSet(void *cdev, GSW_irq_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u8 pidx = parm->nPortId;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_misc);
#endif

	if (parm->nPortId >= gswdev->tpnum) {
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	if (parm->eIrqSrc == GSW_IRQ_WOL) {
		gsw_w32(cdev, (PCE_PIER_WOL_OFFSET + (0xA * pidx)),
			PCE_PIER_WOL_SHIFT, PCE_PIER_WOL_SIZE, 1);
	} else if (parm->eIrqSrc == GSW_IRQ_LIMIT_ALERT) {
		gsw_w32(cdev, (PCE_PIER_LOCK_OFFSET + (0xA * pidx)),
			PCE_PIER_LOCK_SHIFT, PCE_PIER_LOCK_SIZE, 1);
	} else if (parm->eIrqSrc == GSW_IRQ_LOCK_ALERT) {
		gsw_w32(cdev, (PCE_PIER_LIM_OFFSET + (0xA * pidx)),
			PCE_PIER_LIM_SHIFT, PCE_PIER_LIM_SIZE, 1);
	}

	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_misc);
#endif
	return ret;
}

GSW_return_t GSW_IrqStatusClear(void *cdev, GSW_irq_t *parm)
{
	/* ToDo: Request future clarify */
	return GSW_statusOk;
}

/**
 * GSW_PceRuleManage - To Manage the TFLOW Table Rule
 * @cdev: device pointer
 * @parm: parm Pointer to \ref GSW_PCE_rule_t
 * Description: this is to Manage TFLOW Table Rule
 */
static GSW_return_t gsw_pcerulemanage(void *cdev, GSW_PCE_rule_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 idx, firstidx, lastidx;
	int ret = GSW_statusOk;
	GSW_CTP_portConfig_t ctpget;
	/*TFlow table index */
	idx = parm->pattern.nIndex;

	if (!gswdev) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

	switch (parm->region) {
	case GSW_TFLOW_COMMMON_REGION:
		if (idx >= gswdev->num_of_global_rules) {
			pr_err("\n\tERROR: Index %d is greater than common region Entries = %d\n",
			       idx, gswdev->num_of_global_rules);
			return GSW_statusErr;
		}

		break;

	case GSW_TFLOW_CTP_REGION:
		ctpget.nLogicalPortId = parm->logicalportid;
		ctpget.nSubIfIdGroup = parm->subifidgroup;
		ctpget.eMask =  GSW_CTP_PORT_CONFIG_FLOW_ENTRY;
		GSW_CtpPortConfigGet(cdev, &ctpget);
		firstidx = ctpget.nFirstFlowEntryIndex;
		lastidx = ctpget.nFirstFlowEntryIndex + ctpget.nNumberOfFlowEntries;

		if (idx < firstidx || idx >= lastidx) {
			pr_err("\n\tERROR: Index %d is Didn't match CTP region Entries, StartEntryIndex = %d NumberofEntries = %d\n",
			       idx, ctpget.nFirstFlowEntryIndex,
				   ctpget.nNumberOfFlowEntries);
			return GSW_statusErr;
		}

		break;

	case GSW_TFLOW_DEBUG:
		PCE_ASSERT(idx >= (gswdev->tftblsize));
		break;

	default:
		pr_err("\n\tERROR: TFLOW Table doesn't support This Region\n");
		return GSW_statusErr;
	}

	return ret;
}

GSW_return_t GSW_PceRuleRead(void *cdev, GSW_PCE_rule_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return  GSW_statusErr;
	}

	if (GSW_statusOk != gsw_pcerulemanage(cdev, parm))
		return GSW_statusErr;
#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_pce);
#endif

	if (0 != pce_rule_read(cdev, &gswdev->phandler, parm)) {
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_pce);
#endif
	return ret;
}

GSW_return_t GSW_PceRuleWrite(void *cdev, GSW_PCE_rule_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

	if (GSW_statusOk != gsw_pcerulemanage(cdev, parm))
		return GSW_statusErr;
#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_pce);
#endif

	if (0 != pce_rule_write(cdev, &gswdev->phandler, parm)) {
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_pce);
#endif
	return ret;
}

GSW_return_t GSW_PceRuleDelete(void *cdev, GSW_PCE_ruleDelete_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 index; //Traffic-Flow table index.
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_pce);
#endif

	index = parm->nIndex;

	if (0 != pce_pattern_delete(cdev, &gswdev->phandler, index)) {
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_pce);
#endif
	return ret;
}


/*Applicable only for GSWIP 3.1*/
static u8 GSW_SearchContiguousCtp(ethsw_api_dev_t *gswdev, u32 ctp, u32 NumberOfEntries)
{
	u32 i;

	for (i = ctp; i < (ctp + NumberOfEntries); i++) {
		if (gswdev->ctpportconfig_idx[i].IndexInUse)
			return 0;
	}

	return 1;
}

GSW_return_t GSW_CTP_PortAssignmentAlloc(void *cdev, GSW_CTP_portAssignment_t *parm)
{
	static GSW_CTP_portConfig_t CtpConfig;
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 i, ret, ctp;
	u8 ContiguousCtpFound = 0;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_ctp_map);
#endif

	if (!(gswdev->brdgeportconfig_idx[parm->nBridgePortId].IndexInUse)) {
		pr_err("ERROR :BridgePortId %d index not in use,Please allocate the BridgePortId\n", parm->nBridgePortId);
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	/*CTP 0 is for special use, so allocation search will start from CTP 1 to CTP 287*/
	for (ctp = 1; ctp < gswdev->num_of_ctp && !ContiguousCtpFound; ctp++) {
		/*If CTP Index not in use*/
		if (!gswdev->ctpportconfig_idx[ctp].IndexInUse) {
			/*Note: CTP allocation should be contiguous as per the requested nNumberOfCtpPort*/
			ContiguousCtpFound = GSW_SearchContiguousCtp(gswdev, ctp, parm->nNumberOfCtpPort);
		}

		/*Contiguous CTP indexes which is not in use is found*/
		if (ContiguousCtpFound) {
			parm->nFirstCtpPortId = ctp;

			/*Mark the contiguous ctp indexes as InUse and tag it with the logical port*/
			for (i = parm->nFirstCtpPortId; i < (parm->nFirstCtpPortId + parm->nNumberOfCtpPort); i++) {
				gswdev->ctpportconfig_idx[i].IndexInUse = 1;
				gswdev->ctpportconfig_idx[i].AssociatedLogicalPort = parm->nLogicalPortId;
			}
		}
	}

	/*Contiguous CTP indexes not found*/
	if (!ContiguousCtpFound) {
		pr_err("ERROR:No Contiguous CTP indexes Found as per the requested nNumberOfCtpPort\n");
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	/*Configure CTP StartIdx,EndIdx and Mode to the
	  corresponding logical port*/
	ret = GSW_CTP_PortAssignmentSet(cdev, parm);

	if (ret == GSW_statusErr) {
		pr_err("GSW_CTP_PortAssignmentSet returns ERROR\n");
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	memset(&CtpConfig, 0, sizeof(GSW_CTP_portConfig_t));
	CtpConfig.eMask = GSW_CTP_PORT_CONFIG_MASK_BRIDGE_PORT_ID;
	/*Associate the bridge port to the allocated CTP*/
	CtpConfig.nBridgePortId = parm->nBridgePortId;
	/*Logical Port and SubInterfaceID will fetch the CTP index
	  in GSW_CtpPortConfigSet */
	CtpConfig.nLogicalPortId = parm->nLogicalPortId;
	CtpConfig.nSubIfIdGroup = 0;

	for (i = parm->nFirstCtpPortId; i < (parm->nFirstCtpPortId + parm->nNumberOfCtpPort); i++) {
		ret = GSW_CtpPortConfigSet(cdev, &CtpConfig);

		if (ret == GSW_statusErr) {
			pr_err("GSW_CtpPortConfigSet returns ERROR\n");
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}

		CtpConfig.nSubIfIdGroup++;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_misc);
#endif

	/*Enable SDMA for the corresponding logical port*/
	gsw_w32(cdev, (SDMA_PCTRL_PEN_OFFSET + (parm->nLogicalPortId * 0x6)),
		SDMA_PCTRL_PEN_SHIFT, SDMA_PCTRL_PEN_SIZE, 1);

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_misc);
#endif

	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_ctp_map);
#endif
	return ret;
}

GSW_return_t GSW_CTP_PortAssignmentFree(void *cdev, GSW_CTP_portAssignment_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 i, ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_ctp_map);
#endif

	ret = GSW_CTP_PortAssignmentGet(cdev, parm);

	if (ret == GSW_statusErr) {
		pr_err("GSW_CTP_PortAssignmentGet returns ERROR\n");
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	/*Set Index not in-Use to the CTPs assoicated with this logical port*/
	for (i = parm->nFirstCtpPortId; i < (parm->nFirstCtpPortId + parm->nNumberOfCtpPort); i++)
		gswdev->ctpportconfig_idx[i].IndexInUse = 0;

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_misc);
#endif

	/*Disable the SDMA for this logical port*/
	gsw_w32(cdev, (SDMA_PCTRL_PEN_OFFSET + (parm->nLogicalPortId * 0x6)),
		SDMA_PCTRL_PEN_SHIFT, SDMA_PCTRL_PEN_SIZE, 0);

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_misc);
#endif

	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_ctp_map);
#endif
	return ret;
}


GSW_return_t GSW_CTP_PortAssignmentSet(void *cdev, GSW_CTP_portAssignment_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_misc);
#endif

	/*Checking based on number of logical port */
	if (parm->nLogicalPortId >= gswdev->tpnum) {
		pr_err("ERROR: logical port id %d >= than gswdev->tpnum %d\n",
		       parm->nLogicalPortId, gswdev->tpnum);
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	if (parm->nFirstCtpPortId >= gswdev->num_of_ctp) {
		pr_err("ERROR: nFirstCtpPortId %d >= than gswdev->num_of_ctp %d\n",
		       parm->nFirstCtpPortId, gswdev->num_of_ctp);
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	if ((parm->nFirstCtpPortId + (parm->nNumberOfCtpPort - 1)) >= gswdev->num_of_ctp) {
		pr_err("ERROR: nNumberOfCtpPort %d >= than gswdev->num_of_ctp %d\n",
		       (parm->nFirstCtpPortId + parm->nNumberOfCtpPort), gswdev->num_of_ctp);
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	switch (parm->eMode) {
	case GSW_LOGICAL_PORT_8BIT_WLAN:
		gsw_w32(cdev, ETHSW_CTP_STARTID_GET(parm->nLogicalPortId),
			ETHSW_CTP_STARTID_MD_SHIFT, ETHSW_CTP_STARTID_MD_SIZE, 0);
		break;

	case GSW_LOGICAL_PORT_9BIT_WLAN:
		gsw_w32(cdev, ETHSW_CTP_STARTID_GET(parm->nLogicalPortId),
			ETHSW_CTP_STARTID_MD_SHIFT, ETHSW_CTP_STARTID_MD_SIZE, 1);
		break;

	default:
		gsw_w32(cdev, ETHSW_CTP_STARTID_GET(parm->nLogicalPortId),
			ETHSW_CTP_STARTID_MD_SHIFT, ETHSW_CTP_STARTID_MD_SIZE, 2);
	}

	gsw_w32(cdev, ETHSW_CTP_STARTID_GET(parm->nLogicalPortId),
		ETHSW_CTP_STARTID_SHIFT, ETHSW_CTP_STARTID_SIZE, parm->nFirstCtpPortId);

	gsw_w32(cdev, ETHSW_CTP_ENDID_GET(parm->nLogicalPortId),
		ETHSW_CTP_ENDID_SHIFT, ETHSW_CTP_ENDID_SIZE, (parm->nFirstCtpPortId + parm->nNumberOfCtpPort - 1));

	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_misc);
#endif
	return ret;
}

GSW_return_t GSW_CTP_PortAssignmentGet(void *cdev, GSW_CTP_portAssignment_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 value;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_misc);
#endif

	/*Checking based on number of logical port*/
	if (parm->nLogicalPortId >= gswdev->tpnum) {
		pr_err("ERROR: logical port id %u >= than gswdev->tpnum %u\n",
		       parm->nLogicalPortId, gswdev->tpnum);
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	gsw_r32(cdev, ETHSW_CTP_STARTID_GET(parm->nLogicalPortId),
		ETHSW_CTP_STARTID_MD_SHIFT, ETHSW_CTP_STARTID_MD_SIZE, &value);
	parm->eMode = value;

	gsw_r32(cdev, ETHSW_CTP_STARTID_GET(parm->nLogicalPortId),
		ETHSW_CTP_STARTID_SHIFT, ETHSW_CTP_STARTID_SIZE, &value);
	parm->nFirstCtpPortId = value;

	gsw_r32(cdev, ETHSW_CTP_ENDID_GET(parm->nLogicalPortId),
		ETHSW_CTP_ENDID_SHIFT, ETHSW_CTP_ENDID_SIZE, &value);

	if (parm->nFirstCtpPortId < value)
		parm->nNumberOfCtpPort = value - parm->nFirstCtpPortId + 1;
	else if (parm->nFirstCtpPortId == value)
		parm->nNumberOfCtpPort = 1;
	else
		parm->nNumberOfCtpPort = 0;

	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_misc);
#endif
	return ret;

}


GSW_return_t GSW_QOS_ColorMarkingTableSet(void *cdev, GSW_QoS_colorMarkingEntry_t *parm)
{
	pctbl_prog_t tbl_prog;
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u8 pcp, dscp;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_pce);
#endif

	memset(&tbl_prog, 0, sizeof(pctbl_prog_t));
	tbl_prog.table = PCE_MAMRK_INDEX;

	switch (parm->eMode) {
	case GSW_MARKING_ALL_GREEN:
		break;

	case GSW_MARKING_INTERNAL_MARKING:
		break;

	case GSW_MARKING_DEI:
		break;

	case GSW_MARKING_PCP_8P0D:

	/*	For mode 3 there are 16 entries corresponding to PCP (bit3 to 1) + DEI (bit 0) */
	case GSW_MARKING_PCP_7P1D:

	/*	For mode 4 there are 16 entries corresponding to PCP (bit3 to 1) + DEI (bit 0) */
	case GSW_MARKING_PCP_6P2D:

	/*	For mode 5 there are 16 entries corresponding to PCP (bit3 to 1) + DEI (bit 0) */
	case GSW_MARKING_PCP_5P3D:

		/*	For mode 6 there are 16 entries corresponding to PCP (bit3 to 1) + DEI (bit 0) */

		for (pcp = 0; pcp <= 15; pcp++) {
			/*Set Table Entry Index*/
			CLEAR_U16(tbl_prog.pcindex);
			/*Set Color Marking Mode Entry address in PCE_TBL_ADDR 2:0*/
			/*Set DEI Entry address in PCE_TBL_ADDR (bit 4 set to 0)*/
			/*Set PCP Entry address in PCE_TBL_ADDR*/
			tbl_prog.pcindex |= (pcp << 4);
			/*Set Color Marking Mode Entry address in PCE_TBL_ADDR*/
			tbl_prog.pcindex |= parm->eMode;

			/*Set PCE_TBL_VAL 0*/
			CLEAR_U16(tbl_prog.val[0]);
			/*Set Priority in PCE_TBL_VAL 0 (bit 6:4)*/
			tbl_prog.val[0] |= ((parm->nPriority[pcp] & 0x7) << 4);
			/*Set Color in PCE_TBL_VAL 0 (bit 8:7)*/
			tbl_prog.val[0] |= ((parm->nColor[pcp] & 0x3) << 7);
			/*Address-based write*/
			gsw_pce_table_write(cdev, &tbl_prog);
		}

		break;

	case GSW_MARKING_DSCP_AF:

		/*	For mode 7 there are 64 entries corresponding to DSCP */

		for (dscp = 0; dscp <= 63; dscp++) {
			/*Set Table Entry Index*/
			CLEAR_U16(tbl_prog.pcindex);
			/*Set DSCP Entry address in PCE_TBL_ADDR*/
			tbl_prog.pcindex |= (dscp << 4);
			/*Set Color Marking Mode Entry address in PCE_TBL_ADDR*/
			tbl_prog.pcindex |= GSW_MARKING_DSCP_AF;

			/*Set PCE_TBL_VAL 0*/
			CLEAR_U16(tbl_prog.val[0]);
			/*Set Priority in PCE_TBL_VAL 0 (bit 6:4)*/
			tbl_prog.val[0] |= ((parm->nPriority[dscp] & 0x7) << 4);
			/*Set Color in PCE_TBL_VAL 0 (bit 8:7)*/
			tbl_prog.val[0] |= ((parm->nColor[dscp] & 0x3) << 7);
			/*Address-based write*/
			gsw_pce_table_write(cdev, &tbl_prog);
		}

		break;
	}

	ret = GSW_statusOk;

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_pce);
#endif
	return ret;
}

GSW_return_t GSW_QOS_ColorMarkingTableGet(void *cdev, GSW_QoS_colorMarkingEntry_t *parm)
{
	pctbl_prog_t tbl_prog;
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u8 pcp, dscp;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_pce);
#endif

	memset(&tbl_prog, 0, sizeof(pctbl_prog_t));
	tbl_prog.table = PCE_MAMRK_INDEX;

	switch (parm->eMode) {
	case GSW_MARKING_ALL_GREEN:
		break;

	case GSW_MARKING_INTERNAL_MARKING:
		break;

	case GSW_MARKING_DEI:
		break;

	case GSW_MARKING_PCP_8P0D:

	/*	For mode 3 there are 16 entries corresponding to PCP (bit3 to 1) + DEI (bit 0) */
	case GSW_MARKING_PCP_7P1D:

	/*	For mode 4 there are 16 entries corresponding to PCP (bit3 to 1) + DEI (bit 0) */
	case GSW_MARKING_PCP_6P2D:

	/*	For mode 5 there are 16 entries corresponding to PCP (bit3 to 1) + DEI (bit 0) */
	case GSW_MARKING_PCP_5P3D:

		/*	For mode 6 there are 16 entries corresponding to PCP (bit3 to 1) + DEI (bit 0) */

		for (pcp = 0; pcp <= 15; pcp++) {
			tbl_prog.table = PCE_MAMRK_INDEX;
			/*Set Table Entry Index*/
			CLEAR_U16(tbl_prog.pcindex);
			/*Set PCP Entry address in PCE_TBL_ADDR*/
			tbl_prog.pcindex |= (pcp << 4);
			/*Set Color Marking Mode Entry address in PCE_TBL_ADDR*/
			tbl_prog.pcindex |= parm->eMode;
			/*Address-based read*/
			gsw_pce_table_read(cdev, &tbl_prog);

			/*Get PCE_TBL_VAL 0*/
			/*Get Priority in PCE_TBL_VAL 0 (bit 6:4)*/
			parm->nPriority[pcp] = ((tbl_prog.val[0] & 0x70) >> 4);
			/*Get Color in PCE_TBL_VAL 0 (bit 8:7)*/
			parm->nColor[pcp] |= ((tbl_prog.val[0] & 0x180) >> 7);
		}

		break;

	case GSW_MARKING_DSCP_AF:

		/*	For mode 7 there are 64 entries corresponding to DSCP */

		for (dscp = 0; dscp <= 63; dscp++) {
			tbl_prog.table = PCE_MAMRK_INDEX;
			/*Set Table Entry Index*/
			CLEAR_U16(tbl_prog.pcindex);
			/*Set DSCP Entry address in PCE_TBL_ADDR*/
			tbl_prog.pcindex |= (dscp << 4);
			/*Set Color Marking Mode Entry address in PCE_TBL_ADDR*/
			tbl_prog.pcindex |= GSW_MARKING_DSCP_AF;
			/*Address-based read*/
			gsw_pce_table_read(cdev, &tbl_prog);

			/*Get PCE_TBL_VAL 0*/
			/*Get Priority in PCE_TBL_VAL 0 (bit 6:4)*/
			parm->nPriority[dscp] = ((tbl_prog.val[0] & 0x70) >> 4);
			/*Get Color in PCE_TBL_VAL 0 (bit 8:7)*/
			parm->nColor[dscp] |= ((tbl_prog.val[0] & 0x180) >> 7);
		}

		break;
	}

	ret = GSW_statusOk;

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_pce);
#endif
	return ret;
}

GSW_return_t GSW_QOS_ColorReMarkingTableSet(void *cdev, GSW_QoS_colorRemarkingEntry_t *parm)
{
	pctbl_prog_t tbl_prog;
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u8 priority, traffic_class, index;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_pce);
#endif

	memset(&tbl_prog, 0, sizeof(pctbl_prog_t));
	tbl_prog.table = PCE_REMARK_INDEX;

	switch (parm->eMode) {
	case GSW_REMARKING_NONE:
		break;

	case GSW_REMARKING_DEI:
		break;

	case GSW_REMARKING_PCP_8P0D:

	/*	For mode 3 there are 16 entries corresponding to Priority + Color Bit*/
	case GSW_REMARKING_PCP_7P1D:

	/*	For mode 4 there are 16 entries corresponding to Priority + Color Bit*/
	case GSW_REMARKING_PCP_6P2D:

	/*	For mode 5 there are 16 entries corresponding to Priority + Color Bit*/
	case GSW_REMARKING_PCP_5P3D:
		/*	For mode 6 there are 16 entries corresponding to Priority + Color Bit*/

		index = 0;

		/*Set Color Bit 0's entries from 0 t0 7
			  Color Bit 1's entries from 8 t0 15*/
		for (priority = 0; priority <= 15; priority++, index++) {
			/*Set Table Entry Index*/
			CLEAR_U16(tbl_prog.pcindex);
			/*Set Priority Entry address in PCE_TBL_ADDR*/
			tbl_prog.pcindex |= (priority << 4);
			/*Set Color ReMarking Mode Entry address in PCE_TBL_ADDR*/
			tbl_prog.pcindex |= parm->eMode;

			/*Set PCE_TBL_VAL 0*/
			CLEAR_U16(tbl_prog.val[0]);
			/*Set New DEI in PCE_TBL_VAL 0 (bit 0)*/
			tbl_prog.val[0] |= (parm->nVal[index] & 0x1);
			/*Set New PCP in PCE_TBL_VAL 0 (bit 3:1)*/
			tbl_prog.val[0] |= (parm->nVal[index] & 0xE);

			/*Address-based write*/
			gsw_pce_table_write(cdev, &tbl_prog);
		}

		break;

	case GSW_REMARKING_DSCP_AF:
		/*	For mode 7 there are 16 entries corresponding to Traffic Class + Color Bit
			Table Entry index from 64 to 79*/
		index = 0;

		/*Set Color Bit 0's entries from 0 t0 7
			  Color Bit 1's entries from 8 t0 15*/
		for (traffic_class = 0; traffic_class <= 15; traffic_class++, index++) {
			/*Set Table Entry Index*/
			CLEAR_U16(tbl_prog.pcindex);
			/*Set Traffic Class Entry address in PCE_TBL_ADDR*/
			tbl_prog.pcindex |= (traffic_class << 4);
			/*Set Color ReMarking Mode Entry address in PCE_TBL_ADDR*/
			tbl_prog.pcindex |= parm->eMode;

			/*Set PCE_TBL_VAL 0*/
			CLEAR_U16(tbl_prog.val[0]);
			/*Set New DSCP in PCE_TBL_VAL 0 (bit 5:0)*/
			tbl_prog.val[0] |= (parm->nVal[index] & 0x3F);

			/*Address-based write*/
			gsw_pce_table_write(cdev, &tbl_prog);
		}

		break;
	}

	ret = GSW_statusOk;

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_pce);
#endif
	return ret;
}

GSW_return_t GSW_QOS_ColorReMarkingTableGet(void *cdev, GSW_QoS_colorRemarkingEntry_t *parm)
{
	pctbl_prog_t tbl_prog;
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u8 priority, traffic_class, index;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_pce);
#endif

	memset(&tbl_prog, 0, sizeof(pctbl_prog_t));

	switch (parm->eMode) {
	case GSW_REMARKING_NONE:
		break;

	case GSW_REMARKING_DEI:
		break;

	case GSW_REMARKING_PCP_8P0D:

	/*	For mode 3 there are 16 entries corresponding to Priority + Color Bit
		Table Entry index from 0 to 15	*/
	case GSW_REMARKING_PCP_7P1D:

	/*	For mode 4 there are 16 entries corresponding to Priority + Color Bit
		Table Entry index from 16 to 31	*/
	case GSW_REMARKING_PCP_6P2D:

	/*	For mode 5 there are 16 entries corresponding to Priority + Color Bit
		Table Entry index from 32 to 47	*/
	case GSW_REMARKING_PCP_5P3D:
		/*	For mode 6 there are 16 entries corresponding to Priority + Color Bit
			Table Entry index from 48 to 63	*/
		index = 0;

		/*Get Color Bit 0's entries from 0 t0 7
			  Color Bit 1's entries from 8 t0 15*/
		for (priority = 0; priority <= 15; priority++, index++) {
			tbl_prog.table = PCE_REMARK_INDEX;
			/*Set Table Entry Index*/
			CLEAR_U16(tbl_prog.pcindex);
			/*Set Priority Entry address in PCE_TBL_ADDR*/
			tbl_prog.pcindex |= (priority << 4);
			/*Set Color ReMarking Mode Entry address in PCE_TBL_ADDR*/
			tbl_prog.pcindex |= parm->eMode;
			/*Address-based read*/
			gsw_pce_table_read(cdev, &tbl_prog);

			/*Get New DEI in PCE_TBL_VAL 0 (bit 0)*/
			parm->nVal[index] |= (tbl_prog.val[0] & 0x1);
			/*Get New PCP in PCE_TBL_VAL 0 (bit 3:1)*/
			parm->nVal[index] |= (tbl_prog.val[0] & 0xE);
		}

		break;

	case GSW_REMARKING_DSCP_AF:
		/*	For mode 7 there are 16 entries corresponding to Traffic Class + Color Bit
			Table Entry index from 64 to 79*/
		index = 0;

		/*Get Color Bit 0's entries from 0 t0 7
			  Color Bit 1's entries from 8 t0 15*/
		for (traffic_class = 0; traffic_class <= 15; traffic_class++, index++) {
			tbl_prog.table = PCE_REMARK_INDEX;
			/*Set Table Entry Index*/
			CLEAR_U16(tbl_prog.pcindex);
			/*Set Traffic Class Entry address in PCE_TBL_ADDR*/
			tbl_prog.pcindex |= (traffic_class << 4);
			/*Set Color ReMarking Mode Entry address in PCE_TBL_ADDR*/
			tbl_prog.pcindex |= parm->eMode;

			/*Address-based read*/
			gsw_pce_table_read(cdev, &tbl_prog);

			/*Get New DSCP in PCE_TBL_VAL 0 (bit 3:1)*/
			parm->nVal[index] = (tbl_prog.val[0] & 0x3F);
		}

		break;
	}

	ret = GSW_statusOk;

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_pce);
#endif
	return ret;
}

GSW_return_t GSW_QOS_Dscp2PcpTableSet(void *cdev, GSW_DSCP2PCP_map_t *parm)
{
	pctbl_prog_t tbl_prog;
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u8 reg_index, dscp, bitshift;

	/*This API is used internally,so no spin lock needed*/
	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

	if (parm->nIndex > 7) {
		return GSW_statusErr;
	}

	memset(&tbl_prog, 0, sizeof(pctbl_prog_t));
	tbl_prog.table = PCE_DSCP2PCP_INDEX;

	switch (parm->nIndex) {
	case 0:
	default:
		reg_index = 0;
		bitshift = 0;
		break;

	case 1:
		reg_index = 0;
		bitshift = 4;
		break;

	case 2:
		reg_index = 0;
		bitshift = 8;
		break;

	case 3:
		reg_index = 0;
		bitshift = 12;
		break;

	case 4:
		reg_index = 1;
		bitshift = 0;
		break;

	case 5:
		reg_index = 1;
		bitshift = 4;
		break;

	case 6:
		reg_index = 1;
		bitshift = 8;
		break;

	case 7:
		reg_index = 1;
		bitshift = 12;
		break;
	}

	for (dscp = 0; dscp < 64; dscp++) {
		CLEAR_U16(tbl_prog.pcindex);
		/*Table Entry address (DSCP value)*/
		tbl_prog.pcindex |= (dscp & 0x3F);
		/*Address-based read - make sure the other pointer values are not disturbed*/
		tbl_prog.table = PCE_DSCP2PCP_INDEX;
		gsw_pce_table_read(cdev, &tbl_prog);
		/*Set map DSCP value to PCP value in PCE_TBL_VAL 0 or PCE_TBL_VAL 1
		based on DSCP2PCP pointer index*/
		tbl_prog.val[reg_index] |= ((parm->nMap[dscp] & 0x7) << bitshift);
		/*Address-based write*/
		tbl_prog.table = PCE_DSCP2PCP_INDEX;
		gsw_pce_table_write(cdev, &tbl_prog);
	}

	return GSW_statusOk;
}

GSW_return_t GSW_QOS_Dscp2PcpTableGet(void *cdev, GSW_DSCP2PCP_map_t *parm)
{
	pctbl_prog_t tbl_prog;
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u8 reg_index, dscp, bitshift;

	/*This API is used internally,so no spin lock needed*/
	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

	if (parm->nIndex > 7) {
		return GSW_statusErr;
	}

	memset(&tbl_prog, 0, sizeof(pctbl_prog_t));

	switch (parm->nIndex) {
	case 0:
	default:
		reg_index = 0;
		bitshift = 0;
		break;

	case 1:
		reg_index = 0;
		bitshift = 4;
		break;

	case 2:
		reg_index = 0;
		bitshift = 8;
		break;

	case 3:
		reg_index = 0;
		bitshift = 12;
		break;

	case 4:
		reg_index = 1;
		bitshift = 0;
		break;

	case 5:
		reg_index = 1;
		bitshift = 4;
		break;

	case 6:
		reg_index = 1;
		bitshift = 8;
		break;

	case 7:
		reg_index = 1;
		bitshift = 12;
		break;
	}

	for (dscp = 0; dscp < 64; dscp++) {
		tbl_prog.table = PCE_DSCP2PCP_INDEX;
		CLEAR_U16(tbl_prog.pcindex);
		/*Table Entry address (DSCP value) Bit 5:0 in PCE_TBL_ADDR*/
		tbl_prog.pcindex |= (dscp & 0x3F);
		/*Address-based read*/
		gsw_pce_table_read(cdev, &tbl_prog);

		/*Get Mapped PCP value from PCE_TBL_VAL 0 or PCE_TBL_VAL 1
		based on DSCP2PCP pointer index*/
		parm->nMap[dscp] = ((tbl_prog.val[reg_index] >> bitshift) & 0x7);
	}

	return GSW_statusOk;
}

GSW_return_t GSW_QOS_PmapperTableSet(void *cdev, GSW_PMAPPER_t *parm)
{
	pctbl_prog_t tbl_prog;
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u8 pcp, dscp, entry_index;
	u32 idx, freeidxfound = 0;

	/*This API is used internally,so no spin lock needed*/
	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

	/*If P-Mapper ID is invalid ,find a free P-Mapper table index and allocate
	  New P-Mapper configuration table index*/
	if (parm->nPmapperId == PMAPPER_ENTRY_INVALID) {
		for (idx = 0; idx < gswdev->num_of_pmapper && !freeidxfound; idx++) {
			if (!gswdev->pmapper_idx[idx].IndexInUse) {
				gswdev->pmapper_idx[idx].IndexInUse = 1;
				parm->nPmapperId = idx;
				freeidxfound = 1;
			}
		}

		/*No free Slot return Error*/
		if (!freeidxfound) {
			pr_err("No P-mapper slot found\n");
			return GSW_statusErr;
		}
	}

	if (parm->nPmapperId >= gswdev->num_of_pmapper) {
		pr_err("parm->nPmapperId >= gswdev->num_of_pmapper\n");
		return GSW_statusErr;
	}

	/*If P-Mapper ID is valid,Check whether it is InUSE
	  if not InUse,return ERROR*/
	if (!gswdev->pmapper_idx[parm->nPmapperId].IndexInUse) {
		pr_err("gswdev->pmapper_idx[parm->nPmapperId].IndexInUse not in use\n");
		return GSW_statusErr;
	}

	memset(&tbl_prog, 0, sizeof(pctbl_prog_t));
	tbl_prog.table = PCE_PMAP_INDEX;

	/*P-mapper MODE 00:The First Entry of each P-mapper index is for Non-IP and Non-VLAN tagging packets
	  BIT 7:2 in PCE_TBL_ADDR is 0 and BIT 1:0 is 0 for MODE 00
	*/
	entry_index = 0;
	CLEAR_U16(tbl_prog.pcindex);
	/*Index of P-mapper. */
	tbl_prog.pcindex |= ((parm->nPmapperId & 0x3f) << 8) ;
	CLEAR_U16(tbl_prog.val[0]);
	/*Set Destination sub-interface ID group field in PCE_TBL_VAL 0 (Bit 7:0)*/
	tbl_prog.val[0] |= (parm->nDestSubIfIdGroup[entry_index] & 0xFF);
	/*Address-based write*/
	gsw_pce_table_write(cdev, &tbl_prog);

	/*P-mapper MODE 01:The Entry 8 to 1 of each P-mapper index is for PCP mapping entries
	  applies to VLAN tagging packet whenP-mapper mapping mode is PCP
	  BIT 4:2 in PCE_TBL_ADDR is for MODE 01
	*/
	entry_index = 1;

	for (pcp = 0; entry_index <= 8; pcp++, entry_index++) {
		CLEAR_U16(tbl_prog.pcindex);
		/*P-mapper Mode- PCP mapping 01*/
		tbl_prog.pcindex |= 1 ;
		/*Index of P-mapper. */
		tbl_prog.pcindex |= ((parm->nPmapperId & 0x3f) << 8) ;
		/*Table Entry address (PCP value)*/
		tbl_prog.pcindex |= ((pcp & 0x7) << 2);
		/*Set Destination sub-interface ID group field in PCE_TBL_VAL 0 (Bit 7:0)*/
		CLEAR_U16(tbl_prog.val[0]);
		tbl_prog.val[0] |= (parm->nDestSubIfIdGroup[entry_index] & 0xFF);
		/*Address-based write*/
		gsw_pce_table_write(cdev, &tbl_prog);
	}

	/*P-mapper MODE 10:The Entry 72 to 9 of each P-mapper index is for DSCP mapping entries
	  applies to IP packets without VLAN tag or IP packet with VLAN
	  BIT 7:2 in PCE_TBL_ADDR is for MODE 10
	*/
	entry_index = 9;

	/* TODO: use dscp as for loop index */
	for (dscp = 0; entry_index <= 72; dscp++, entry_index++) {
		CLEAR_U16(tbl_prog.pcindex);
		/*P-mapper Mode- DSCP mapping 10*/
		tbl_prog.pcindex |= 2 ;
		/*Index of P-mapper. */
		tbl_prog.pcindex |= ((parm->nPmapperId & 0x3f) << 8) ;
		/*Table Entry address (DSCP value)*/
		tbl_prog.pcindex |= ((dscp & 0x3F) << 2);
		/*Set Destination sub-interface ID group field in PCE_TBL_VAL 0 (Bit 7:0)*/
		CLEAR_U16(tbl_prog.val[0]);
		tbl_prog.val[0] |= (parm->nDestSubIfIdGroup[entry_index] & 0xFF);
		/*Address-based write*/
		gsw_pce_table_write(cdev, &tbl_prog);
	}

	return GSW_statusOk;
}

GSW_return_t GSW_QOS_PmapperTableGet(void *cdev, GSW_PMAPPER_t *parm)
{
	pctbl_prog_t tbl_prog;
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u8 pcp, dscp, entry_index;

	/*This API is used internally,so no spin lock needed*/
	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

	/*If P-Mapper ID is valid,Check whether it is InUSE
	  if not InUse,return ERROR*/
	if (!gswdev->pmapper_idx[parm->nPmapperId].IndexInUse) {
		pr_err("gswdev->pmapper_idx[parm->nPmapperId].IndexInUse not in use\n");
		return GSW_statusErr;
	}

	memset(&tbl_prog, 0, sizeof(pctbl_prog_t));

	/*P-mapper MODE 00:The First Entry of each P-mapper index is for Non-IP and Non-VLAN tagging packets
	  BIT 7:2 in PCE_TBL_ADDR is 0 and BIT 1:0 is 0 for MODE 00
	*/
	entry_index = 0;
	CLEAR_U16(tbl_prog.pcindex);
	tbl_prog.table = PCE_PMAP_INDEX;
	/*Index of P-mapper. */
	tbl_prog.pcindex |= ((parm->nPmapperId & 0x3f) << 8) ;
	CLEAR_U16(tbl_prog.val[0]);
	/*Address-based read*/
	gsw_pce_table_read(cdev, &tbl_prog);
	/*Get Destination sub-interface ID group field in PCE_TBL_VAL 0 (Bit 7:0)*/
	parm->nDestSubIfIdGroup[entry_index] = (tbl_prog.val[0] & 0xFF);

	/*P-mapper MODE 01:The Entry 8 to 1 of each P-mapper index is for PCP mapping entries
	  applies to VLAN tagging packet whenP-mapper mapping mode is PCP
	  BIT 4:2 in PCE_TBL_ADDR is for MODE 01
	*/
	entry_index = 1;

	for (pcp = 0; entry_index <= 8; pcp++, entry_index++) {
		tbl_prog.table = PCE_PMAP_INDEX;
		CLEAR_U16(tbl_prog.pcindex);
		/*P-mapper Mode- PCP mapping 01*/
		tbl_prog.pcindex |= 1 ;
		/*Index of P-mapper. */
		tbl_prog.pcindex |= ((parm->nPmapperId & 0x3f) << 8) ;
		/*Table Entry address (PCP value)*/
		tbl_prog.pcindex |= ((pcp & 0x7) << 2);
		/*Address-based read*/
		gsw_pce_table_read(cdev, &tbl_prog);
		/*Get Destination sub-interface ID group field in PCE_TBL_VAL 0 (Bit 7:0)*/
		parm->nDestSubIfIdGroup[entry_index] = (tbl_prog.val[0] & 0xFF);
	}

	/*P-mapper MODE 10:The Entry 72 to 9 of each P-mapper index is for DSCP mapping entries
	  applies to IP packets without VLAN tag or IP packet with VLAN
	  BIT 7:2 in PCE_TBL_ADDR is for MODE 10
	*/
	entry_index = 9;

	for (dscp = 0; entry_index <= 72; dscp++, entry_index++) {
		tbl_prog.table = PCE_PMAP_INDEX;
		CLEAR_U16(tbl_prog.pcindex);
		/*P-mapper Mode- DSCP mapping 10*/
		tbl_prog.pcindex |= 2 ;
		/*Index of P-mapper. */
		tbl_prog.pcindex |= ((parm->nPmapperId & 0x3f) << 8) ;
		/*Table Entry address (DSCP value)*/
		tbl_prog.pcindex |= ((dscp & 0x3F) << 2);
		/*Address-based read*/
		gsw_pce_table_read(cdev, &tbl_prog);
		/*Get Destination sub-interface ID group field in PCE_TBL_VAL 0 (Bit 7:0)*/
		parm->nDestSubIfIdGroup[entry_index] = (tbl_prog.val[0] & 0xFF);
	}

	return GSW_statusOk;
}

static u8 GSW_SearchExVlanContiguousBlock(ethsw_api_dev_t *gswdev, u32 BlockId, u32 NumberOfEntries)
{
	u32 i;

	for (i = BlockId; i <= (BlockId + NumberOfEntries); i++) {
		if (gswdev->extendvlan_idx.vlan_idx[i].IndexInUse)
			return 0;
	}

	return 1;
}


GSW_return_t GSW_ExtendedVlanAlloc(void *cdev, GSW_EXTENDEDVLAN_alloc_t *parm)
{

	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u16 ExVlanIndex = 0;
	u8 ContiguousBlockFound = 0;
	u32 i, ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_pce);
#endif

	/*
		Allocate New Block as per the number of table Entries requested
		The Block must be allocated with contiguous table index
	*/

	if (parm->nNumberOfEntries > (gswdev->num_of_extendvlan - gswdev->extendvlan_idx.nUsedEntry)) {
		pr_err(" nNumberOfEntries requested is more than Exvlan index limit  %s:%s:%d",
		       __FILE__, __func__, __LINE__);
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	for (ExVlanIndex = 0; ExVlanIndex < gswdev->num_of_extendvlan && !ContiguousBlockFound; ExVlanIndex++) {
		/*Table Index not in use*/
		if (!gswdev->extendvlan_idx.vlan_idx[ExVlanIndex].IndexInUse) {
			ContiguousBlockFound = GSW_SearchExVlanContiguousBlock(gswdev, ExVlanIndex, parm->nNumberOfEntries);
		}

		/*Contiguous block found in the table*/
		if (ContiguousBlockFound) {
			parm->nExtendedVlanBlockId = ExVlanIndex;

			/*Mark the contiguous table indexes as InUse and tag it with block id*/
			for (i = parm->nExtendedVlanBlockId; i < (parm->nExtendedVlanBlockId + parm->nNumberOfEntries); i++) {
				gswdev->extendvlan_idx.vlan_idx[i].IndexInUse = 1;
				gswdev->extendvlan_idx.vlan_idx[i].VlanBlockId = parm->nExtendedVlanBlockId;
				gswdev->extendvlan_idx.nUsedEntry++;
			}
		}
	}

	/*Contiguous block not found in the table*/
	if (!ContiguousBlockFound) {
		pr_err(" ContiguousBlockFound %s:%s:%d", __FILE__, __func__, __LINE__);
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_pce);
#endif
	return ret;
}

GSW_return_t GSW_ExtendedVlanSet(void *cdev, GSW_EXTENDEDVLAN_config_t *parm)
{
	static pctbl_prog_t tbl_prog;
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u8 ret, all_entry_match = 0;
	u8 dscp2pcpmatch_found = 0;
	u32 idx = 0, dscp2pcp_pointer = 0, dscp = 0, meterid = 0;
	u32 empty_dscp2pcptblslot_found = 0, bDscp2PcpMapEnable = 0;
	static GSW_DSCP2PCP_map_t dscp2pcp_get, dscp2pcp_set;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_pce);
#endif

	/* Once the New blk allocated,param->nEntryIndex will decide which index with in this block
	   has to be set.only one index will be set at a time.
	   It is users responsibility to set all the index with in this block
	   by calling GSW_ExtendedVlanSet as many times as required (i.e) param->nNumberOfEntries*/
	idx = parm->nExtendedVlanBlockId + parm->nEntryIndex;

	if (idx >= gswdev->num_of_extendvlan) {
		pr_err("ERROR : idx %d >= gswdev->num_of_extendvlan \n", idx);
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	if (gswdev->extendvlan_idx.vlan_idx[idx].VlanBlockId != parm->nExtendedVlanBlockId) {
		pr_err("ERROR : VlanBlockId %d != parm->nExtendedVlanBlockId %d \n",
		       gswdev->extendvlan_idx.vlan_idx[idx].VlanBlockId, parm->nExtendedVlanBlockId);
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	if (!gswdev->extendvlan_idx.vlan_idx[idx].IndexInUse) {
		pr_err("ERROR : idx %d not in Use \n", idx);
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	memset(&tbl_prog, 0, sizeof(pctbl_prog_t));
	tbl_prog.table = PCE_EXTVLAN_INDEX;
	CLEAR_U16(tbl_prog.pcindex);
	/*Table Entry address (Egress VLAN treatment index) Bit 9:0 in PCE_TBL_ADDR*/
	tbl_prog.pcindex |= (idx & 0x3FF);

	/** Extended VLAN Table Filter **/
	/*Program Outer VLAN Filter*/
	switch (parm->sFilter.sOuterVlan.eType) {
	/** There is tag and criteria applies. */
	case GSW_EXTENDEDVLAN_FILTER_TYPE_NORMAL:
		CLEAR_U16(tbl_prog.key[1]);
		CLEAR_U16(tbl_prog.key[0]);

		if (parm->sFilter.sOuterVlan.bPriorityEnable) {
			/*Filter on outer priority
			  PCP value key 1 BIT 15:12*/
			tbl_prog.key[1] |= ((parm->sFilter.sOuterVlan.nPriorityVal & 0x7) << 12);
		} else {
			/*Do not filter on outer priority
			Filter Outer PCP key 1 BIT 15:12 set to 8 */
			tbl_prog.key[1] |= (8 << 12);
		}

		if (parm->sFilter.sOuterVlan.bVidEnable) {
			/*Do filter on outer VID
			  Filter Outer VID (Total 13 bits)
			  key 0 BIT 15
			  key 1 BIT 11:0
			  Set nVidVal
			*/
			tbl_prog.key[0] |= ((parm->sFilter.sOuterVlan.nVidVal & 0x1) << 15);
			tbl_prog.key[1] |= ((parm->sFilter.sOuterVlan.nVidVal & 0x1FFE) >> 1);
		} else {
			/*Do not filter on outer VID
			Filter Outer VID (Total 13 bits)
			key 0 BIT 15
			key 1 BIT 11:0
			Set 4096
			*/
			tbl_prog.key[0] |= ((4096 & 0x1) << 15);
			tbl_prog.key[1] |= ((4096 & 0x1FFE) >> 1);
		}

		/*Filter out outer TPID/DEI*/
		switch (parm->sFilter.sOuterVlan.eTpid) {
		/** Do not filter TPID. */
		case GSW_EXTENDEDVLAN_FILTER_TPID_NO_FILTER:
			/*Do not filter on outer TPID
			  Filter Outer TPID key 0 BIT 14:12 set to 0 */
			tbl_prog.key[0] &= ~(7 << 12);
			break;

		/** TPID is 0x8100. key 0 BIT 14:12 set 4*/
		case GSW_EXTENDEDVLAN_FILTER_TPID_8021Q:
			tbl_prog.key[0] |= (4 << 12);
			break;

		/** TPID is global configured value. */
		case GSW_EXTENDEDVLAN_FILTER_TPID_VTETYPE:
			switch (parm->sFilter.sOuterVlan.eDei) {
			/** Do not filter (don't care) key 0 BIT 14:12 set 5 */
			case GSW_EXTENDEDVLAN_FILTER_DEI_NO_FILTER:
				tbl_prog.key[0] |= (5 << 12);
				break;

			/** DEI=0 key 0 BIT 14:12 set 6 */
			case GSW_EXTENDEDVLAN_FILTER_DEI_0:
				tbl_prog.key[0] |= (6 << 12);
				break;

			/** DEI=1 key 0 BIT 14:12 set 7 */
			case GSW_EXTENDEDVLAN_FILTER_DEI_1:
				tbl_prog.key[0] |= (7 << 12);
				break;
			}

			break;
		}

		break;

	/** There is tag but no criteria. */
	case GSW_EXTENDEDVLAN_FILTER_TYPE_NO_FILTER:
		CLEAR_U16(tbl_prog.key[1]);
		CLEAR_U16(tbl_prog.key[0]);

		/*Do not filter on outer priority
		  Filter Outer PCP key 1 BIT 15:12 set to 8 */
		tbl_prog.key[1] |= (8 << 12);

		/*Do not filter on outer VID
		  Filter Outer VID (Total 13 bits)
		  key 0 BIT 15
		  key 1 BIT 11:0
		  Set 4096
		*/
		tbl_prog.key[0] |= ((4096 & 0x1) << 15);
		tbl_prog.key[1] |= ((4096 & 0x1FFE) >> 1);

		/*Do not filter on outer TPID
		  Filter Outer PCP key 0 BIT 14:12 set to 0 */
		tbl_prog.key[0] &= ~(7 << 12);
		break;

	/** Default entry if no other rule applies. */
	case GSW_EXTENDEDVLAN_FILTER_TYPE_DEFAULT:
		CLEAR_U16(tbl_prog.key[1]);
		/*Default entry if no other rule applies
		  Filter Outer PCP key 1 BIT 15:12 set to 14 */
		tbl_prog.key[1] |= (14 << 12);
		break;

	/** There is no tag. */
	case GSW_EXTENDEDVLAN_FILTER_TYPE_NO_TAG:
		CLEAR_U16(tbl_prog.key[1]);
		/*No Outer Tag
		  Filter Outer PCP key 1 BIT 15:12 set to 15 */
		tbl_prog.key[1] |= (15 << 12);
		break;

	case GSW_EXTENDEDVLAN_BLOCK_INVALID:
		CLEAR_U16(tbl_prog.key[1]);
		/*SET BLOCK INVALID - should be set only on outer priority filter
		  Filter Outer PCP key 1 BIT 15:12 set to 13 */
		tbl_prog.key[1] |= (13 << 12);
		break;

	default:
		break;
	}

	/*Program inner VLAN Filter*/
	switch (parm->sFilter.sInnerVlan.eType) {
	/** There is tag and criteria applies. */
	case GSW_EXTENDEDVLAN_FILTER_TYPE_NORMAL:
		CLEAR_U16(tbl_prog.key[3]);
		CLEAR_U16(tbl_prog.key[2]);

		if (parm->sFilter.sInnerVlan.bPriorityEnable) {
			/*Filter on inner priority
			  PCP value key 3 BIT 15:12*/
			tbl_prog.key[3] |= ((parm->sFilter.sInnerVlan.nPriorityVal & 0x7) << 12);
		} else {
			/*Do not filter on inner priority
			Filter inner PCP key 3 BIT 15:12 set to 8 */
			tbl_prog.key[3] |= (8 << 12);
		}

		if (parm->sFilter.sInnerVlan.bVidEnable) {
			/*Do not filter on inner VID
			  Filter inner VID (Total 13 bits)
			  key 2 BIT 15
			  key 3 BIT 11:0
			  Set nVidVal
			*/
			tbl_prog.key[2] |= ((parm->sFilter.sInnerVlan.nVidVal & 0x1) << 15);
			tbl_prog.key[3] |= ((parm->sFilter.sInnerVlan.nVidVal & 0x1FFE) >> 1);
		} else {
			/*Do not filter on inner VID
			 Filter inner VID (Total 13 bits)
			 key 2 BIT 15
			 key 3 BIT 11:0
			 Set 4096
			*/
			tbl_prog.key[2] |= ((4096 & 0x1) << 15);
			tbl_prog.key[3] |= ((4096 & 0x1FFE) >> 1);
		}

		/*Filter out inner TPID/DEI*/
		switch (parm->sFilter.sInnerVlan.eTpid) {
		/** Do not filter TPID. */
		case GSW_EXTENDEDVLAN_FILTER_TPID_NO_FILTER:
			/*Do not filter on inner TPID
			  Filter inner TPID key 2 BIT 14:12 set to 0 */
			tbl_prog.key[2] &= ~(7 << 12);
			break;

		/** TPID is 0x8100. key 2 BIT 14:12 set 4*/
		case GSW_EXTENDEDVLAN_FILTER_TPID_8021Q:
			tbl_prog.key[2] |= (4 << 12);
			break;

		/** TPID is global configured value. */
		case GSW_EXTENDEDVLAN_FILTER_TPID_VTETYPE:
			switch (parm->sFilter.sInnerVlan.eDei) {
			/** Do not filter (don't care) key 2 BIT 14:12 set 5 */
			case GSW_EXTENDEDVLAN_FILTER_DEI_NO_FILTER:
				tbl_prog.key[2] |= (5 << 12);
				break;

			/** DEI=0 key 2 BIT 14:12 set 6 */
			case GSW_EXTENDEDVLAN_FILTER_DEI_0:
				tbl_prog.key[2] |= (6 << 12);
				break;

			/** DEI=1 key 2 BIT 14:12 set 7 */
			case GSW_EXTENDEDVLAN_FILTER_DEI_1:
				tbl_prog.key[2] |= (7 << 12);
				break;
			}

			break;
		}

		break;

	/** There is tag but no criteria. */
	case GSW_EXTENDEDVLAN_FILTER_TYPE_NO_FILTER:
		CLEAR_U16(tbl_prog.key[3]);
		CLEAR_U16(tbl_prog.key[2]);

		/*Do not filter on inner priority
		  Filter inner PCP key 3 BIT 15:12 set to 8 */
		tbl_prog.key[3] |= (8 << 12);

		/*Do not filter on inner VID
		  Filter inner VID (Total 13 bits)
		  key 2 BIT 15
		  key 3 BIT 11:0
		  Set 4096
		*/
		tbl_prog.key[2] |= ((4096 & 0x1) << 15);
		tbl_prog.key[3] |= ((4096 & 0x1FFE) >> 1);

		/*Do not filter on inner TPID
		  Filter inner PCP key 2 BIT 14:12 set to 0 */
		tbl_prog.key[2] &= ~(7 << 12);
		break;

	/** Default entry if no other rule applies. */
	case GSW_EXTENDEDVLAN_FILTER_TYPE_DEFAULT:
		CLEAR_U16(tbl_prog.key[3]);

		/*Default entry if no other rule applies
		  Filter inner PCP key 3 BIT 15:12 set to 14 */
		tbl_prog.key[3] |= (14 << 12);
		break;

	/** There is no tag. */
	case GSW_EXTENDEDVLAN_FILTER_TYPE_NO_TAG:
		CLEAR_U16(tbl_prog.key[3]);
		/*No inner Tag
		  Filter inner PCP key 3 BIT 15:12 set to 15 */
		tbl_prog.key[3] |= (15 << 12);
		break;

	default:
		break;
	}

	/*Filter Ether type key 2 BIT 4:0*/
	tbl_prog.key[2] &= ~(0x1F);

	switch (parm->sFilter.eEtherType) {
	/** Do not filter Ether Type*/
	case GSW_EXTENDEDVLAN_FILTER_ETHERTYPE_NO_FILTER:
		tbl_prog.key[2] |= 0;
		break;

	/** IPoE frame (Ethertyp is 0x0800). */
	case GSW_EXTENDEDVLAN_FILTER_ETHERTYPE_IPOE:
		tbl_prog.key[2] |= 1;
		break;

	/** PPPoE frame (Ethertyp is 0x8863 or 0x8864). */
	case GSW_EXTENDEDVLAN_FILTER_ETHERTYPE_PPPOE:
		tbl_prog.key[2] |= 2;
		break;

	/** ARP frame (Ethertyp is 0x0806). */
	case GSW_EXTENDEDVLAN_FILTER_ETHERTYPE_ARP:
		tbl_prog.key[2] |= 3;
		break;

	/** IPv6 IPoE frame (Ethertyp is 0x86DD). */
	case GSW_EXTENDEDVLAN_FILTER_ETHERTYPE_IPV6IPOE:
		tbl_prog.key[2] |= 4;
		break;

	/** EAPOL (Ethertyp is 0x888E). */
	case GSW_EXTENDEDVLAN_FILTER_ETHERTYPE_EAPOL :
		tbl_prog.key[2] |= 5;
		break;

	}



	/** Extended VLAN Table Treatment **/

	/** Number of VLAN tag to remove. val 1 BIT 15:14*/
	tbl_prog.val[1] &= ~(0xC000);

	switch (parm->sTreatment.eRemoveTag) {
	/* Do not remove VLAN tag. set 0 */
	case GSW_EXTENDEDVLAN_TREATMENT_NOT_REMOVE_TAG:
		tbl_prog.val[1] &= ~(0xC000);
		break;

	/* Remove 1 VLAN tag following DA/SA. set 1*/
	case GSW_EXTENDEDVLAN_TREATMENT_REMOVE_1_TAG:
		tbl_prog.val[1] |= (1 << 14);
		break;

	/* Remove 2 VLAN tag following DA/SA. set 2*/
	case GSW_EXTENDEDVLAN_TREATMENT_REMOVE_2_TAG:
		tbl_prog.val[1] |= (2 << 14);
		break;

	/* Discard upstream traffic. set 3*/
	case GSW_EXTENDEDVLAN_TREATMENT_DISCARD_UPSTREAM:
		tbl_prog.val[1] |= (3 << 14);
		break;
	}

	/** Enable outer VLAN tag add/modification. */
	if (parm->sTreatment.bAddOuterVlan) {
		/*Treatment for outer Priority val 1 BIT 3:0*/
		tbl_prog.val[1] &= ~(0xF);

		switch (parm->sTreatment.sOuterVlan.ePriorityMode) {
		/* Add an Outer Tag and set priority with given value. */
		case GSW_EXTENDEDVLAN_TREATMENT_PRIORITY_VAL:
			tbl_prog.val[1] |= (parm->sTreatment.sOuterVlan.ePriorityVal & 0x7);
			break;

		/* Add an Outer Tag and Priority value is copied from inner VLAN tag of received packet. set 8*/
		case GSW_EXTENDEDVLAN_TREATMENT_INNER_PRORITY:
			tbl_prog.val[1] |= 8;
			break;

		/*Add an Outer Tag and Priority value is copied from outer VLAN tag of received packet. set 9 */
		case GSW_EXTENDEDVLAN_TREATMENT_OUTER_PRORITY:
			tbl_prog.val[1] |= 9;
			break;

		/* Add an Outer Tag and Priority value is derived from DSCP field of received packet. set 10
		   Priority Value is set as per DSCP to PCP mapping attribute*/
		case GSW_EXTENDEDVLAN_TREATMENT_DSCP:
			tbl_prog.val[1] |= 10;
			bDscp2PcpMapEnable = 1;
			break;
		}

		/*Treatment for outer VID val 0 BIT 15:3*/
		tbl_prog.val[0] &= ~(0xFFF8);

		switch (parm->sTreatment.sOuterVlan.eVidMode) {
		/* Set VID with given value. */
		case GSW_EXTENDEDVLAN_TREATMENT_VID_VAL:
			tbl_prog.val[0] |= ((parm->sTreatment.sOuterVlan.eVidVal & 0xFFF) << 3);
			break;

		/* VID is copied from inner VLAN tag of received packet. set 4096*/
		case GSW_EXTENDEDVLAN_TREATMENT_INNER_VID:
			tbl_prog.val[0] |= (4096 << 3);
			break;

		/* VID is copied from outer VLAN tag of received packet.set 4097 */
		case GSW_EXTENDEDVLAN_TREATMENT_OUTER_VID:
			tbl_prog.val[0] |= (4097 << 3);
			break;
		}

		/*Treatment for outer TPID/DEI val 0 BIT 2:0*/
		tbl_prog.val[0] &= ~(0x7);

		switch (parm->sTreatment.sOuterVlan.eTpid) {
		/* TPID is copied from inner VLAN tag of received packet.set 0 */
		case GSW_EXTENDEDVLAN_TREATMENT_INNER_TPID:
			tbl_prog.val[0] &= ~(0x7);
			break;

		/* TPID is copied from outer VLAN tag of received packet. set 1*/
		case GSW_EXTENDEDVLAN_TREATMENT_OUTER_TPID:
			tbl_prog.val[0] |= 1;
			break;

		/* TPID is 0x8100. */
		case GSW_EXTENDEDVLAN_TREATMENT_8021Q:
			tbl_prog.val[0] |= 4;
			break;

		/* TPID is global configured value. */
		case GSW_EXTENDEDVLAN_TREATMENT_VTETYPE:
			switch (parm->sTreatment.sOuterVlan.eDei) {
			/* DEI (if applicable) is copied from inner VLAN tag of received packet. set 2*/
			case GSW_EXTENDEDVLAN_TREATMENT_INNER_DEI:
				tbl_prog.val[0] |= 2;
				break;

			/* DEI (if applicable) is copied from outer VLAN tag of received packet. set 3*/
			case GSW_EXTENDEDVLAN_TREATMENT_OUTER_DEI:
				tbl_prog.val[0] |= 3;
				break;

			/* DEI is 0. set 6*/
			case GSW_EXTENDEDVLAN_TREATMENT_DEI_0:
				tbl_prog.val[0] |= 6;
				break;

			/* DEI is 1. set 7*/
			case GSW_EXTENDEDVLAN_TREATMENT_DEI_1:
				tbl_prog.val[0] |= 7;
				break;
			}

			break;
		}

		/*	}*/
	} else {
		/*DO Not Add an Outer Tag. val 1 BIT 3:0 set 15*/
		tbl_prog.val[1] &= ~(0xF);
		tbl_prog.val[1] |= 15;
	}

	/** Enable inner VLAN tag add/modification. */
	if (parm->sTreatment.bAddInnerVlan) {
		/*Treatment for inner Priority val 3 BIT 3:0*/
		tbl_prog.val[3] &= ~(0xF);

		switch (parm->sTreatment.sInnerVlan.ePriorityMode) {
		/* Add an inner Tag and set priority with given value. */
		case GSW_EXTENDEDVLAN_TREATMENT_PRIORITY_VAL:
			tbl_prog.val[3] |= (parm->sTreatment.sInnerVlan.ePriorityVal & 0x7);
			break;

		/* Add an inner Tag and Priority value is copied from inner VLAN tag of received packet. set 8*/
		case GSW_EXTENDEDVLAN_TREATMENT_INNER_PRORITY:
			tbl_prog.val[3] |= 8;
			break;

		/*Add an inner Tag and Priority value is copied from outer VLAN tag of received packet. set 9 */
		case GSW_EXTENDEDVLAN_TREATMENT_OUTER_PRORITY:
			tbl_prog.val[3] |= 9;
			break;

		/* Add an inner Tag and Priority value is derived from DSCP field of received packet. set 10
		   Priority Value is set as per DSCP to PCP mapping attribute*/
		case GSW_EXTENDEDVLAN_TREATMENT_DSCP:
			tbl_prog.val[3] |= 10;
			bDscp2PcpMapEnable = 1;
			break;
		}

		/*Treatment for inner VID val 2 BIT 15:3*/
		tbl_prog.val[2] &= ~(0xFFF8);

		switch (parm->sTreatment.sInnerVlan.eVidMode) {
		/* Set VID with given value. */
		case GSW_EXTENDEDVLAN_TREATMENT_VID_VAL:
			tbl_prog.val[2] |= ((parm->sTreatment.sInnerVlan.eVidVal & 0xFFF) << 3);
			break;

		/* VID is copied from inner VLAN tag of received packet. set 4096*/
		case GSW_EXTENDEDVLAN_TREATMENT_INNER_VID:
			tbl_prog.val[2] |= (4096 << 3);
			break;

		/* VID is copied from outer VLAN tag of received packet.set 4097 */
		case GSW_EXTENDEDVLAN_TREATMENT_OUTER_VID:
			tbl_prog.val[2] |= (4097 << 3);
			break;
		}

		/*Treatment for inner TPID/DEI val 2 BIT 2:0*/
		tbl_prog.val[2] &= ~(0x7);

		switch (parm->sTreatment.sInnerVlan.eTpid) {
		/* TPID is copied from inner VLAN tag of received packet.set 0 */
		case GSW_EXTENDEDVLAN_TREATMENT_INNER_TPID:
			tbl_prog.val[2] &= ~(0x7);
			break;

		/* TPID is copied from outer VLAN tag of received packet. set 1*/
		case GSW_EXTENDEDVLAN_TREATMENT_OUTER_TPID:
			tbl_prog.val[2] |= 1;
			break;

		/* TPID is 0x8100. */
		case GSW_EXTENDEDVLAN_TREATMENT_8021Q:
			tbl_prog.val[2] |= 4;
			break;

		/* TPID is global configured value. */
		case GSW_EXTENDEDVLAN_TREATMENT_VTETYPE:
			switch (parm->sTreatment.sInnerVlan.eDei) {
			/* DEI (if applicable) is copied from inner VLAN tag of received packet. set 2*/
			case GSW_EXTENDEDVLAN_TREATMENT_INNER_DEI:
				tbl_prog.val[2] |= 2;
				break;

			/* DEI (if applicable) is copied from outer VLAN tag of received packet. set 3*/
			case GSW_EXTENDEDVLAN_TREATMENT_OUTER_DEI:
				tbl_prog.val[2] |= 3;
				break;

			/* DEI is 0. set 6*/
			case GSW_EXTENDEDVLAN_TREATMENT_DEI_0:
				tbl_prog.val[2] |= 6;
				break;

			/* DEI is 1. set 7*/
			case GSW_EXTENDEDVLAN_TREATMENT_DEI_1:
				tbl_prog.val[2] |= 7;
				break;
			}

			break;
		}

		/*}*/
	} else {
		/*DO Not Add an inner Tag. val 3 BIT 3:0 set 15*/
		tbl_prog.val[3] &= ~(0xF);
		tbl_prog.val[3] |= 15;
	}

	/** Treatment Bridge port
		val 4 BIT 8 to enable new bridge port treatment
		val 4 BIT 7:0 for new bridge port id
	**/
	tbl_prog.val[4] &= ~(0x1FF);

	/*(0..127 no action and 128..255 re-assign bridge port id*/
	if (parm->sTreatment.bReassignBridgePort) {
		tbl_prog.val[4] |= (1 << 8);
		tbl_prog.val[4] |= (parm->sTreatment.nNewBridgePortId & 0xFF);
	}

	/** Treatment DSCP
		val 4 BIT 15 to enable new DSCP treatment
		val 4 BIT 14:9 for new DSCP value
	**/
	tbl_prog.val[4] &= ~(0xFE00);

	/*(0..63 no action and 64..127 New DSCP in bit 5 to 0*/
	if (parm->sTreatment.bNewDscpEnable) {
		tbl_prog.val[4] |= (1 << 15);
		tbl_prog.val[4] |= ((parm->sTreatment.nNewDscp & 0x3F) << 9);
	}

	/** Treatment Traffic Class
		val 5 BIT 13 to enable new Traffic class treatment
		val 5 BIT 12:9 for new Traffic class value
	**/
	tbl_prog.val[5] &= ~(0x3E00);

	/*(0..15 no action and 16..31 New Traffic class in bit 3 to 0*/
	if (parm->sTreatment.bNewTrafficClassEnable) {
		tbl_prog.val[5] |= (1 << 13);
		tbl_prog.val[5] |= ((parm->sTreatment.nNewTrafficClass & 0xF) << 9);
	}

	/** Treatment Metering
		val 5 BIT 8 to enable Metering
		val 5 BIT 7:0 for New Meter ID
	**/
	tbl_prog.val[5] &= ~(0x1FF);

	/* New meter ID TODO - KEEP CHECK on METERID.
	Meter should be allocated with \ref GSW_QOS_METER_ALLOC before extended
	VLAN treatment is added. If this extended VLAN treatment is deleted,
	this meter should be released with \ref GSW_QOS_METER_FREE.
	*/
	if (parm->sTreatment.bNewMeterEnable) {
		meterid = parm->sTreatment.sNewTrafficMeterId;
		/*This below field enable indicates that this meter id is
		 already assigned and it's idx recorded for this Exvlan tbl.
		 If next time the user update this meter field with different meter idx !!
		 or wanted to allocate a new meter idx ??!! may happen by mistake ??!!
		 the previous meter idx must be released from this Exvlan tbl*/

		if (gswdev->extendvlan_idx.vlan_idx[idx].MeterAssigned) {
			if (gswdev->extendvlan_idx.vlan_idx[idx].MeterId !=
			    meterid) {
				/*release the usage of previous meter idx*/
				gswdev->meter_idx[gswdev->extendvlan_idx.vlan_idx[idx].MeterId].IndexInUsageCnt--;
				gswdev->extendvlan_idx.vlan_idx[idx].MeterAssigned = 0;
				gswdev->extendvlan_idx.vlan_idx[idx].MeterId = 0;
			}
		}

		/*This Meter ID should be in use (i.e) that is this Meter ID should be allocated
		before calling Exvlan configuration
		If not in use return ERROR
		*/
		if (gswdev->meter_idx[meterid].IndexInUse) {
			/*Usage count will be incremented only once during meter idx assignment
			for this Exvlan id*/
			if (!gswdev->extendvlan_idx.vlan_idx[idx].MeterAssigned) {
				gswdev->extendvlan_idx.vlan_idx[idx].MeterAssigned = 1;
				gswdev->extendvlan_idx.vlan_idx[idx].MeterId
					= meterid;
				/*Since this meter id can be shared,Increment the it's usage count*/
				gswdev->meter_idx[meterid].IndexInUsageCnt++;
			}

			tbl_prog.val[5] |= (1 << 8);
			tbl_prog.val[5] |= (parm->sTreatment.sNewTrafficMeterId & 0xFF);
		} else {
			pr_err("ERROR :Meter Id %d not alocated,Call Meter Alloc before Exvlan configuration\n", meterid);
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;

		}
	}

	/** Enable loopback. val 5 BIT 14*/
	tbl_prog.val[5] &= ~(0x4000);

	if (parm->sTreatment.bLoopbackEnable)
		tbl_prog.val[5] |= (1 << 14);

	/** Enable destination/source MAC address swap. val 5 BIT 15*/
	tbl_prog.val[5] &= ~(0x8000);

	if (parm->sTreatment.bDaSaSwapEnable)
		tbl_prog.val[5] |= (1 << 15);

	/** Enable traffic mirrored to the monitoring port. val 3 BIT 15 */
	tbl_prog.val[3] &= ~(0x8000);

	if (parm->sTreatment.bMirrorEnable)
		tbl_prog.val[3] |= (1 << 15);

	/** Treatment for DSCP2PCP **/
	if (bDscp2PcpMapEnable) {
		/* Check for match in DSCP2PCP Map for each index - 0 to 7 DSCP2PCP pointers*/
		dscp2pcpmatch_found = 0;

		for (idx = 0; idx <= 7 && !dscp2pcpmatch_found; idx++) {
			/*Check only the dscp2pcp pointer which is in InUse*/
			if (gswdev->dscp2pcp_idx[idx].IndexInUse &&
			    gswdev->dscp2pcp_idx[idx].IndexInUsageCnt) {
				memset(&dscp2pcp_get, 0, sizeof(GSW_DSCP2PCP_map_t));
				dscp2pcp_get.nIndex = idx;
				ret = GSW_QOS_Dscp2PcpTableGet(cdev, &dscp2pcp_get);

				if (ret == GSW_statusErr) {
					pr_err("ERROR:%s, GSW_QOS_Dscp2PcpTableGet failed\n",
						__func__);
					goto UNLOCK_AND_RETURN;
				}

				/*check and compare DSCP 0 to 63 entry address's pcp value*/
				all_entry_match = 1;

				for (dscp = 0; dscp < 64 && all_entry_match; dscp++) {
					if (parm->sTreatment.nDscp2PcpMap[dscp] != dscp2pcp_get.nMap[dscp])
						all_entry_match = 0;
				}

				/*All dscp 0 to 63 entry address's pcp value match for this pointer/idx
				 Record this dscp2pcp pointer*/
				if (all_entry_match) {
					dscp2pcp_pointer = idx;
					dscp2pcpmatch_found = 1;
					pr_debug("INFO:%s, dscp2pcpmatch_found-dscp2pcp_pointer:(%d)\n",
						__func__, dscp2pcp_pointer);
				}
			}
		}

		if (!dscp2pcpmatch_found) {
			/*If no match found in the dscp2pcp table,search for an empty slot*/
			for (idx = 0; idx <= 7 && !empty_dscp2pcptblslot_found; idx++) {
				if (!gswdev->dscp2pcp_idx[idx].IndexInUse &&
				    gswdev->dscp2pcp_idx[idx].IndexInUsageCnt == 0) {
					empty_dscp2pcptblslot_found = 1;
					dscp2pcp_pointer = idx;
				}
			}

			/*If the empty slot found,allocate and set the sTreatment.nDscp2PcpMap[64] to
			  corresponding dscp2pcp table index
			*/
			if (empty_dscp2pcptblslot_found) {
				pr_debug("INFO:%s, empty_dscp2pcptblslot_found-dscp2pcp_pointer:(%d)\n",
					__func__, dscp2pcp_pointer);
				memcpy(dscp2pcp_set.nMap, parm->sTreatment.nDscp2PcpMap, (sizeof(u8) * 64));
				dscp2pcp_set.nIndex = dscp2pcp_pointer;
				ret = GSW_QOS_Dscp2PcpTableSet(cdev, &dscp2pcp_set);

				if (ret == GSW_statusErr) {
					pr_err("ERROR:%s, GSW_QOS_Dscp2PcpTableSet failed\n",
						__func__);
					goto UNLOCK_AND_RETURN;
				}
			} else {
				pr_err("ERROR:%s, DSCP2PCP table is full\n",
					__func__);
				ret = GSW_statusErr;
				goto UNLOCK_AND_RETURN;
			}
		}

		/*This below field enable indicates that this dscp2pcp pointer is
		 already assigned and it's idx recorded for this Extended Vlan index.
		 If next time user calls EXVlanAdd again for same idx with different
		 nDscp2PcpMap array value!!
		 may happen by mistake ??!!
		 the search may give different pointer or allocate new pointer ??!!
		 so the previous dscp2pcp pointer must be released from this
		 Extended VLAN idx*/
		idx = parm->nExtendedVlanBlockId + parm->nEntryIndex;

		if (gswdev->extendvlan_idx.vlan_idx[idx].Dscp2PcpPointerAssigned) {
			int old_dscp2pcp_pointer;

			if (gswdev->extendvlan_idx.vlan_idx[idx].Dscp2PcpPointer !=
			    dscp2pcp_pointer) {
				old_dscp2pcp_pointer = gswdev->extendvlan_idx.vlan_idx[idx].Dscp2PcpPointer;
				/*release the usage of previous DSCP2PCP pointer*/
				gswdev->dscp2pcp_idx[old_dscp2pcp_pointer].IndexInUsageCnt--;

				if (gswdev->dscp2pcp_idx[old_dscp2pcp_pointer].IndexInUsageCnt
				    == 0) {
					gswdev->dscp2pcp_idx[old_dscp2pcp_pointer].IndexInUse = 0;
				}

				gswdev->extendvlan_idx.vlan_idx[idx].Dscp2PcpPointerAssigned = 0;
				gswdev->extendvlan_idx.vlan_idx[idx].Dscp2PcpPointer = 0;
			}
		}

		/*Treatment DSCP2PCP pointer-VAL 3 BIT 6:4.
		  Set the recorded 0..7 dscp2pcp pointer*/
		tbl_prog.val[3] &= ~(0x70);
		tbl_prog.val[3] |= ((dscp2pcp_pointer & 0x7) << 4);

		/* dscp2pcp pointer usage count can be incremented only once for this vlan idx*/
		if (!gswdev->extendvlan_idx.vlan_idx[idx].Dscp2PcpPointerAssigned) {
			gswdev->extendvlan_idx.vlan_idx[idx].Dscp2PcpPointerAssigned = 1;
			gswdev->extendvlan_idx.vlan_idx[idx].Dscp2PcpPointer
				= dscp2pcp_pointer;
			/*Since this DSCP2PCP can be shared,Increment the it's usage count*/
			gswdev->dscp2pcp_idx[dscp2pcp_pointer].IndexInUse = 1;
			gswdev->dscp2pcp_idx[dscp2pcp_pointer].IndexInUsageCnt++;
		}
	}

	/*Address-based write*/
	gsw_pce_table_write(cdev, &tbl_prog);
	ret = GSW_statusOk;
UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_pce);
#endif
	return ret;

}

GSW_return_t GSW_ExtendedVlanGet(void *cdev, GSW_EXTENDEDVLAN_config_t *parm)
{
	pctbl_prog_t tbl_prog;
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u8 ret = 0;
	u32 idx = 0, dscp2pcp_pointer, tpid_dei, ethtype, removetag;
	u32 bDscp2PcpMapEnable = 0;
	GSW_DSCP2PCP_map_t dscp2pcp_get;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_pce);
#endif

	idx = parm->nExtendedVlanBlockId + parm->nEntryIndex;

	if (idx >= gswdev->num_of_extendvlan) {
		pr_err("ERROR : idx %d >= gswdev->num_of_extendvlan \n", idx);
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	/*Index does not belongs to this ExtendedVlanBlock*/
	if (gswdev->extendvlan_idx.vlan_idx[idx].VlanBlockId != parm->nExtendedVlanBlockId) {
		pr_err("ERROR : VlanBlockId %d != parm->nExtendedVlanBlockId %d \n",
		       gswdev->extendvlan_idx.vlan_idx[idx].VlanBlockId, parm->nExtendedVlanBlockId);
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	/*Index is not in USE*/
	if (!gswdev->extendvlan_idx.vlan_idx[idx].IndexInUse) {
		pr_err("ERROR : idx %d not in Use \n", idx);
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	memset(&tbl_prog, 0, sizeof(pctbl_prog_t));
	tbl_prog.table = PCE_EXTVLAN_INDEX;
	CLEAR_U16(tbl_prog.pcindex);
	/*Table Entry address (Egress VLAN treatment index) Bit 9:0 in PCE_TBL_ADDR*/
	tbl_prog.pcindex |= (idx & 0x3FF);
	/*Address-based read*/
	gsw_pce_table_read(cdev, &tbl_prog);

	/*Get Outer VLAn Filter*/
	parm->sFilter.sOuterVlan.nPriorityVal = ((tbl_prog.key[1] >> 12) & 0xf);
	parm->sFilter.sOuterVlan.nVidVal |= ((tbl_prog.key[0] & 0x8000) >> 15);
	parm->sFilter.sOuterVlan.nVidVal |= ((tbl_prog.key[1] & 0xFFF) << 1);
	tpid_dei = ((tbl_prog.key[0] & 0x7000) >> 12);

	if (parm->sFilter.sOuterVlan.nPriorityVal == 8 &&
	    parm->sFilter.sOuterVlan.nVidVal == 4096 &&
	    tpid_dei == 0) {
		parm->sFilter.sOuterVlan.eType = GSW_EXTENDEDVLAN_FILTER_TYPE_NO_FILTER;
		parm->sFilter.sOuterVlan.eTpid = GSW_EXTENDEDVLAN_FILTER_TPID_NO_FILTER;
		parm->sFilter.sOuterVlan.eDei = GSW_EXTENDEDVLAN_FILTER_DEI_NO_FILTER;
		parm->sFilter.sOuterVlan.bVidEnable = 0;
		parm->sFilter.sOuterVlan.bPriorityEnable = 0;
	} else if (parm->sFilter.sOuterVlan.nPriorityVal == 14) {
		parm->sFilter.sOuterVlan.eType = GSW_EXTENDEDVLAN_FILTER_TYPE_DEFAULT;
		parm->sFilter.sOuterVlan.eTpid = GSW_EXTENDEDVLAN_FILTER_TPID_NO_FILTER;
		parm->sFilter.sOuterVlan.eDei = GSW_EXTENDEDVLAN_FILTER_DEI_NO_FILTER;
		parm->sFilter.sOuterVlan.bVidEnable = 0;
		parm->sFilter.sOuterVlan.bPriorityEnable = 0;
	} else if (parm->sFilter.sOuterVlan.nPriorityVal == 15) {
		parm->sFilter.sOuterVlan.eType = GSW_EXTENDEDVLAN_FILTER_TYPE_NO_TAG;
		parm->sFilter.sOuterVlan.eTpid = GSW_EXTENDEDVLAN_FILTER_TPID_NO_FILTER;
		parm->sFilter.sOuterVlan.eDei = GSW_EXTENDEDVLAN_FILTER_DEI_NO_FILTER;
		parm->sFilter.sOuterVlan.bVidEnable = 0;
		parm->sFilter.sOuterVlan.bPriorityEnable = 0;
	} else if (parm->sFilter.sOuterVlan.nPriorityVal == 13) {
		parm->sFilter.sOuterVlan.eType = GSW_EXTENDEDVLAN_BLOCK_INVALID;
		parm->sFilter.sOuterVlan.eTpid = GSW_EXTENDEDVLAN_FILTER_TPID_NO_FILTER;
		parm->sFilter.sOuterVlan.eDei = GSW_EXTENDEDVLAN_FILTER_DEI_NO_FILTER;
		parm->sFilter.sOuterVlan.bVidEnable = 0;
		parm->sFilter.sOuterVlan.bPriorityEnable = 0;
	} else {
		parm->sFilter.sOuterVlan.eType = GSW_EXTENDEDVLAN_FILTER_TYPE_NORMAL;

		if (parm->sFilter.sOuterVlan.nPriorityVal != 8)
			parm->sFilter.sOuterVlan.bPriorityEnable = 1;
		else
			parm->sFilter.sOuterVlan.bPriorityEnable = 0;

		if (parm->sFilter.sOuterVlan.nVidVal != 4096)
			parm->sFilter.sOuterVlan.bVidEnable = 1;
		else
			parm->sFilter.sOuterVlan.bVidEnable = 0;

		switch (tpid_dei) {
		case 0:
			parm->sFilter.sOuterVlan.eTpid = GSW_EXTENDEDVLAN_FILTER_TPID_NO_FILTER;
			parm->sFilter.sOuterVlan.eDei = GSW_EXTENDEDVLAN_FILTER_DEI_NO_FILTER;
			break;

		case 4:
			parm->sFilter.sOuterVlan.eTpid = GSW_EXTENDEDVLAN_FILTER_TPID_8021Q;
			parm->sFilter.sOuterVlan.eDei = GSW_EXTENDEDVLAN_FILTER_DEI_NO_FILTER;
			break;

		case 5:
			parm->sFilter.sOuterVlan.eTpid = GSW_EXTENDEDVLAN_FILTER_TPID_VTETYPE;
			parm->sFilter.sOuterVlan.eDei = GSW_EXTENDEDVLAN_FILTER_DEI_NO_FILTER;
			break;

		case 6:
			parm->sFilter.sOuterVlan.eTpid = GSW_EXTENDEDVLAN_FILTER_TPID_VTETYPE;
			parm->sFilter.sOuterVlan.eDei = GSW_EXTENDEDVLAN_FILTER_DEI_0;
			break;

		case 7:
			parm->sFilter.sOuterVlan.eTpid = GSW_EXTENDEDVLAN_FILTER_TPID_VTETYPE;
			parm->sFilter.sOuterVlan.eDei = GSW_EXTENDEDVLAN_FILTER_DEI_1;
			break;
		}
	}

	/*Get Inner VLAn Filter*/
	parm->sFilter.sInnerVlan.nPriorityVal = ((tbl_prog.key[3] >> 12) & 0xf);
	parm->sFilter.sInnerVlan.nVidVal |= ((tbl_prog.key[2] & 0x8000) >> 15);
	parm->sFilter.sInnerVlan.nVidVal |= ((tbl_prog.key[3] & 0xFFF) << 1);
	tpid_dei = ((tbl_prog.key[2] & 0x7000) >> 12);

	if (parm->sFilter.sInnerVlan.nPriorityVal == 8 &&
	    parm->sFilter.sInnerVlan.nVidVal == 4096 &&
	    tpid_dei == 0) {
		parm->sFilter.sInnerVlan.eType = GSW_EXTENDEDVLAN_FILTER_TYPE_NO_FILTER;
		parm->sFilter.sInnerVlan.eTpid = GSW_EXTENDEDVLAN_FILTER_TPID_NO_FILTER;
		parm->sFilter.sInnerVlan.eDei = GSW_EXTENDEDVLAN_FILTER_DEI_NO_FILTER;
		parm->sFilter.sInnerVlan.bVidEnable = 0;
		parm->sFilter.sInnerVlan.bPriorityEnable = 0;
	} else if (parm->sFilter.sInnerVlan.nPriorityVal == 14) {
		parm->sFilter.sInnerVlan.eType = GSW_EXTENDEDVLAN_FILTER_TYPE_DEFAULT;
		parm->sFilter.sInnerVlan.eTpid = GSW_EXTENDEDVLAN_FILTER_TPID_NO_FILTER;
		parm->sFilter.sInnerVlan.eDei = GSW_EXTENDEDVLAN_FILTER_DEI_NO_FILTER;
		parm->sFilter.sInnerVlan.bVidEnable = 0;
		parm->sFilter.sInnerVlan.bPriorityEnable = 0;
	} else if (parm->sFilter.sInnerVlan.nPriorityVal == 15) {
		parm->sFilter.sInnerVlan.eType = GSW_EXTENDEDVLAN_FILTER_TYPE_NO_TAG;
		parm->sFilter.sInnerVlan.eTpid = GSW_EXTENDEDVLAN_FILTER_TPID_NO_FILTER;
		parm->sFilter.sInnerVlan.eDei = GSW_EXTENDEDVLAN_FILTER_DEI_NO_FILTER;
		parm->sFilter.sInnerVlan.bVidEnable = 0;
		parm->sFilter.sInnerVlan.bPriorityEnable = 0;
	} else {
		parm->sFilter.sInnerVlan.eType = GSW_EXTENDEDVLAN_FILTER_TYPE_NORMAL;

		if (parm->sFilter.sInnerVlan.nPriorityVal != 8)
			parm->sFilter.sInnerVlan.bPriorityEnable = 1;
		else
			parm->sFilter.sInnerVlan.bPriorityEnable = 0;

		if (parm->sFilter.sInnerVlan.nVidVal != 4096)
			parm->sFilter.sInnerVlan.bVidEnable = 1;
		else
			parm->sFilter.sInnerVlan.bVidEnable = 0;

		switch (tpid_dei) {
		case 0:
			parm->sFilter.sInnerVlan.eTpid = GSW_EXTENDEDVLAN_FILTER_TPID_NO_FILTER;
			parm->sFilter.sInnerVlan.eDei = GSW_EXTENDEDVLAN_FILTER_DEI_NO_FILTER;
			break;

		case 4:
			parm->sFilter.sInnerVlan.eTpid = GSW_EXTENDEDVLAN_FILTER_TPID_8021Q;
			parm->sFilter.sInnerVlan.eDei = GSW_EXTENDEDVLAN_FILTER_DEI_NO_FILTER;
			break;

		case 5:
			parm->sFilter.sInnerVlan.eTpid = GSW_EXTENDEDVLAN_FILTER_TPID_VTETYPE;
			parm->sFilter.sInnerVlan.eDei = GSW_EXTENDEDVLAN_FILTER_DEI_NO_FILTER;
			break;

		case 6:
			parm->sFilter.sInnerVlan.eTpid = GSW_EXTENDEDVLAN_FILTER_TPID_VTETYPE;
			parm->sFilter.sInnerVlan.eDei = GSW_EXTENDEDVLAN_FILTER_DEI_0;
			break;

		case 7:
			parm->sFilter.sInnerVlan.eTpid = GSW_EXTENDEDVLAN_FILTER_TPID_VTETYPE;
			parm->sFilter.sInnerVlan.eDei = GSW_EXTENDEDVLAN_FILTER_DEI_1;
			break;
		}
	}

	/*Get Ether type filter*/
	ethtype = (tbl_prog.key[2] & 0x1F);

	switch (ethtype) {
	case 0:
		parm->sFilter.eEtherType = GSW_EXTENDEDVLAN_FILTER_ETHERTYPE_NO_FILTER;
		break;

	case 1:
		parm->sFilter.eEtherType = GSW_EXTENDEDVLAN_FILTER_ETHERTYPE_IPOE;
		break;

	case 2:
		parm->sFilter.eEtherType = GSW_EXTENDEDVLAN_FILTER_ETHERTYPE_PPPOE;
		break;

	case 3:
		parm->sFilter.eEtherType = GSW_EXTENDEDVLAN_FILTER_ETHERTYPE_ARP;
		break;

	case 4:
		parm->sFilter.eEtherType = GSW_EXTENDEDVLAN_FILTER_ETHERTYPE_IPV6IPOE;
		break;

	case 5:
		parm->sFilter.eEtherType = GSW_EXTENDEDVLAN_FILTER_ETHERTYPE_EAPOL;
		break;
	}

	/**Treatment Get**/
	/** Get Number of VLAN tag to remove treatment.*/
	removetag = ((tbl_prog.val[1] & 0xC000) >> 14);

	switch (removetag) {
	case 0:
		parm->sTreatment.eRemoveTag = GSW_EXTENDEDVLAN_TREATMENT_NOT_REMOVE_TAG;
		break;

	case 1:
		parm->sTreatment.eRemoveTag = GSW_EXTENDEDVLAN_TREATMENT_REMOVE_1_TAG;
		break;

	case 2:
		parm->sTreatment.eRemoveTag = GSW_EXTENDEDVLAN_TREATMENT_REMOVE_2_TAG;
		break;

	case 3:
		parm->sTreatment.eRemoveTag = GSW_EXTENDEDVLAN_TREATMENT_DISCARD_UPSTREAM;
		break;
	}

	/** Get outer VLAN tag treatment. */
	parm->sTreatment.sOuterVlan.ePriorityVal = (tbl_prog.val[1] & 0xF);

	if (parm->sTreatment.sOuterVlan.ePriorityVal == 15) {
		/*no outer VLAN tag added*/
		parm->sTreatment.bAddOuterVlan = 0;
	} else {
		parm->sTreatment.bAddOuterVlan = 1;

		switch (parm->sTreatment.sOuterVlan.ePriorityVal) {
		case 8:
			parm->sTreatment.sOuterVlan.ePriorityMode = GSW_EXTENDEDVLAN_TREATMENT_INNER_PRORITY;
			break;

		case 9:
			parm->sTreatment.sOuterVlan.ePriorityMode = GSW_EXTENDEDVLAN_TREATMENT_OUTER_PRORITY;
			break;

		case 10:
			parm->sTreatment.sOuterVlan.ePriorityMode = GSW_EXTENDEDVLAN_TREATMENT_DSCP;
			bDscp2PcpMapEnable = 1;
			break;

		default:
			parm->sTreatment.sOuterVlan.ePriorityMode = GSW_EXTENDEDVLAN_TREATMENT_PRIORITY_VAL;
			break;
		}

		/*Get Treatment for outer VID*/
		parm->sTreatment.sOuterVlan.eVidVal = ((tbl_prog.val[0] & 0xFFF8) >> 3);

		if (parm->sTreatment.sOuterVlan.eVidVal == 4096)
			parm->sTreatment.sOuterVlan.eVidMode = GSW_EXTENDEDVLAN_TREATMENT_INNER_VID;
		else if (parm->sTreatment.sOuterVlan.eVidVal == 4097)
			parm->sTreatment.sOuterVlan.eVidMode = GSW_EXTENDEDVLAN_TREATMENT_OUTER_VID;
		else
			parm->sTreatment.sOuterVlan.eVidMode = GSW_EXTENDEDVLAN_TREATMENT_VID_VAL;

		/*Treatment for outer TPID/DEI*/
		tpid_dei = (tbl_prog.val[0] & 0x7);

		switch (tpid_dei) {
		case 0:
			parm->sTreatment.sOuterVlan.eTpid = GSW_EXTENDEDVLAN_TREATMENT_INNER_TPID;
			parm->sTreatment.sOuterVlan.eDei = GSW_EXTENDEDVLAN_TREATMENT_INNER_DEI;
			break;

		case 1:
			parm->sTreatment.sOuterVlan.eTpid = GSW_EXTENDEDVLAN_TREATMENT_OUTER_TPID;
			parm->sTreatment.sOuterVlan.eDei = GSW_EXTENDEDVLAN_TREATMENT_OUTER_DEI;
			break;

		case 2:
			parm->sTreatment.sOuterVlan.eTpid = GSW_EXTENDEDVLAN_TREATMENT_VTETYPE;
			parm->sTreatment.sOuterVlan.eDei = GSW_EXTENDEDVLAN_TREATMENT_INNER_DEI;
			break;

		case 3:
			parm->sTreatment.sOuterVlan.eTpid = GSW_EXTENDEDVLAN_TREATMENT_VTETYPE;
			parm->sTreatment.sOuterVlan.eDei = GSW_EXTENDEDVLAN_TREATMENT_OUTER_DEI;
			break;

		case 4:
			parm->sTreatment.sOuterVlan.eTpid = GSW_EXTENDEDVLAN_TREATMENT_8021Q;
			parm->sTreatment.sOuterVlan.eDei = GSW_EXTENDEDVLAN_TREATMENT_DEI_0;

			break;

		case 6:
			parm->sTreatment.sOuterVlan.eTpid = GSW_EXTENDEDVLAN_TREATMENT_VTETYPE;
			parm->sTreatment.sOuterVlan.eDei = GSW_EXTENDEDVLAN_TREATMENT_DEI_0;
			break;

		case 7:
			parm->sTreatment.sOuterVlan.eTpid = GSW_EXTENDEDVLAN_TREATMENT_VTETYPE;
			parm->sTreatment.sOuterVlan.eDei = GSW_EXTENDEDVLAN_TREATMENT_DEI_1;
			break;
		}
	}

	/** Get inner VLAN tag treatment. */
	parm->sTreatment.sInnerVlan.ePriorityVal = (tbl_prog.val[3] & 0xF);

	if (parm->sTreatment.sInnerVlan.ePriorityVal == 15) {
		/*no inner VLAN tag added*/
		parm->sTreatment.bAddInnerVlan = 0;
	} else {
		parm->sTreatment.bAddInnerVlan = 1;
		parm->sTreatment.sInnerVlan.ePriorityVal = (tbl_prog.val[3] & 0xF);

		switch (parm->sTreatment.sInnerVlan.ePriorityVal) {
		case 8:
			parm->sTreatment.sInnerVlan.ePriorityMode = GSW_EXTENDEDVLAN_TREATMENT_INNER_PRORITY;
			break;

		case 9:
			parm->sTreatment.sInnerVlan.ePriorityMode = GSW_EXTENDEDVLAN_TREATMENT_OUTER_PRORITY;
			break;

		case 10:
			parm->sTreatment.sInnerVlan.ePriorityMode = GSW_EXTENDEDVLAN_TREATMENT_DSCP;
			bDscp2PcpMapEnable = 1;
			break;

		default:
			parm->sTreatment.sInnerVlan.ePriorityMode = GSW_EXTENDEDVLAN_TREATMENT_PRIORITY_VAL;
			break;
		}

		/*Get Treatment for inner VID*/
		parm->sTreatment.sInnerVlan.eVidVal = ((tbl_prog.val[2] & 0xFFF8) >> 3);

		if (parm->sTreatment.sInnerVlan.eVidVal == 4096)
			parm->sTreatment.sInnerVlan.eVidMode = GSW_EXTENDEDVLAN_TREATMENT_INNER_VID;
		else if (parm->sTreatment.sInnerVlan.eVidVal == 4097)
			parm->sTreatment.sInnerVlan.eVidMode = GSW_EXTENDEDVLAN_TREATMENT_OUTER_VID;
		else
			parm->sTreatment.sInnerVlan.eVidMode = GSW_EXTENDEDVLAN_TREATMENT_VID_VAL;

		/*Treatment for inner TPID/DEI*/
		tpid_dei = (tbl_prog.val[2] & 0x7);

		switch (tpid_dei) {
		case 0:
			parm->sTreatment.sInnerVlan.eTpid = GSW_EXTENDEDVLAN_TREATMENT_INNER_TPID;
			parm->sTreatment.sInnerVlan.eDei = GSW_EXTENDEDVLAN_TREATMENT_INNER_DEI;
			break;

		case 1:
			parm->sTreatment.sInnerVlan.eTpid = GSW_EXTENDEDVLAN_TREATMENT_OUTER_TPID;
			parm->sTreatment.sInnerVlan.eDei = GSW_EXTENDEDVLAN_TREATMENT_OUTER_DEI;
			break;

		case 2:
			parm->sTreatment.sInnerVlan.eTpid = GSW_EXTENDEDVLAN_TREATMENT_VTETYPE;
			parm->sTreatment.sInnerVlan.eDei = GSW_EXTENDEDVLAN_TREATMENT_INNER_DEI;
			break;

		case 3:
			parm->sTreatment.sInnerVlan.eTpid = GSW_EXTENDEDVLAN_TREATMENT_VTETYPE;
			parm->sTreatment.sInnerVlan.eDei = GSW_EXTENDEDVLAN_TREATMENT_OUTER_DEI;
			break;

		case 4:
			parm->sTreatment.sInnerVlan.eTpid = GSW_EXTENDEDVLAN_TREATMENT_8021Q;
			parm->sTreatment.sInnerVlan.eDei = GSW_EXTENDEDVLAN_TREATMENT_DEI_0;

			break;

		case 6:
			parm->sTreatment.sInnerVlan.eTpid = GSW_EXTENDEDVLAN_TREATMENT_VTETYPE;
			parm->sTreatment.sInnerVlan.eDei = GSW_EXTENDEDVLAN_TREATMENT_DEI_0;
			break;

		case 7:
			parm->sTreatment.sInnerVlan.eTpid = GSW_EXTENDEDVLAN_TREATMENT_VTETYPE;
			parm->sTreatment.sInnerVlan.eDei = GSW_EXTENDEDVLAN_TREATMENT_DEI_1;
			break;
		}
	}

	/** Get Treatment Bridge port **/
	parm->sTreatment.bReassignBridgePort = ((tbl_prog.val[4] &  0x100) >> 8);

	if (parm->sTreatment.bReassignBridgePort)
		parm->sTreatment.nNewBridgePortId = (tbl_prog.val[4] & 0xFF);

	/** Get Treatment DSCP **/
	parm->sTreatment.bNewDscpEnable = ((tbl_prog.val[4] & 0x8000) >> 15);

	if (parm->sTreatment.bNewDscpEnable)
		parm->sTreatment.nNewDscp = ((tbl_prog.val[4] & 0x7E00) >> 9);

	/** Get Treatment Traffic Class **/
	parm->sTreatment.bNewTrafficClassEnable	= ((tbl_prog.val[5] & 0x2000) >> 13);

	if (parm->sTreatment.bNewTrafficClassEnable)
		parm->sTreatment.nNewTrafficClass = ((tbl_prog.val[5] & 0x1E00) >> 9);

	/** Treatment Metering **/
	parm->sTreatment.bNewMeterEnable = ((tbl_prog.val[5] &  0x100) >> 8);

	if (parm->sTreatment.bNewMeterEnable)
		parm->sTreatment.sNewTrafficMeterId = (tbl_prog.val[5] & 0xFF);

	/** Get loopback. val 5 BIT 14*/
	parm->sTreatment.bLoopbackEnable = ((tbl_prog.val[5] & 0x4000) >> 14);
	/** Get destination/source MAC address swap. val 5 BIT 15*/
	parm->sTreatment.bDaSaSwapEnable = ((tbl_prog.val[5] & 0x8000) >> 15);
	/** Get traffic mirrored to the monitoring port. val 3 BIT 15 */
	parm->sTreatment.bMirrorEnable = ((tbl_prog.val[3] & 0x8000) >> 15);

	if (bDscp2PcpMapEnable) {
		dscp2pcp_pointer = ((tbl_prog.val[3] & 0x70) >> 4);
		pr_debug("INFO:%s, dscp2pcp_pointer:(%d)\n", __func__,
			dscp2pcp_pointer);

		/** Get DSCP2PCP map **/
		/*This dscp2pcp idx should be in use*/
		if (!gswdev->dscp2pcp_idx[dscp2pcp_pointer].IndexInUse) {
			pr_err("ERROR:%s, dscp2pcp_pointer not in use:(%d)\n",
				__func__, dscp2pcp_pointer);
			goto UNLOCK_AND_RETURN;
		}

		dscp2pcp_get.nIndex = dscp2pcp_pointer;
		ret = GSW_QOS_Dscp2PcpTableGet(cdev, &dscp2pcp_get);

		if (ret == GSW_statusErr) {
			pr_err("ERROR:%s, GSW_QOS_Dscp2PcpTableGet failed\n",
				__func__);
			goto UNLOCK_AND_RETURN;
		}

		memcpy(parm->sTreatment.nDscp2PcpMap, dscp2pcp_get.nMap, (sizeof(u8) * 64));
	}

	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_pce);
#endif
	return ret;
}

GSW_return_t GSW_ExtendedVlanFree(void *cdev, GSW_EXTENDEDVLAN_alloc_t *param)
{
	pctbl_prog_t tbl_prog;
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 idx = 0, dscp2pcp_pointer = 0, meterid = 0, meterenable = 0;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_pce);
#endif

	if (param->nExtendedVlanBlockId >= gswdev->num_of_extendvlan) {
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	/*nExtendedVlanBlockId should be in use,if not in use return error*/
	if (!gswdev->extendvlan_idx.vlan_idx[param->nExtendedVlanBlockId].IndexInUse) {
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	/*if this nExtendedVlanBlockId usage count is not zero that means it is used by some one.
	  This Block can be deleted, only if that some one release this block*/
	if (gswdev->extendvlan_idx.vlan_idx[param->nExtendedVlanBlockId].IndexInUsageCnt) {
		pr_err("ERROR :Extended Vlan block %u is used by some resource,can not delete\n", param->nExtendedVlanBlockId);
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	idx = param->nExtendedVlanBlockId;
	param->nNumberOfEntries = 0;

	if (gswdev->extendvlan_idx.vlan_idx[idx].VlanBlockId != param->nExtendedVlanBlockId) {
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	/*Condition to delete idx
		1. idx should belong to this block
		2. idx should be in Use
		3. idx should be with in valid Extended VLAN table range (Entry 0-1023 is valid)
	*/
	while (gswdev->extendvlan_idx.vlan_idx[idx].IndexInUse && idx < gswdev->num_of_extendvlan
	       && (gswdev->extendvlan_idx.vlan_idx[idx].VlanBlockId == param->nExtendedVlanBlockId)) {


		memset(&tbl_prog, 0, sizeof(pctbl_prog_t));
		tbl_prog.table = PCE_EXTVLAN_INDEX;
		CLEAR_U16(tbl_prog.pcindex);
		/*Table Entry address (Egress VLAN treatment index) Bit 9:0 in PCE_TBL_ADDR*/
		tbl_prog.pcindex |= (idx & 0x3FF);
		/*Address-based read*/
		gsw_pce_table_read(cdev, &tbl_prog);

		if (gswdev->extendvlan_idx.vlan_idx[idx].Dscp2PcpPointerAssigned) {
			/*get dscp2pcp table pointer*/
			dscp2pcp_pointer = ((tbl_prog.val[3] & 0x70) >> 4);

			/*Check whether the dscp2pcp pointer in use.
			  It should be in use,since it got allocated or assigned in GSW_ExtendedVlanAdd
			  for this ExtendedVlan table idx (i.e) it is marked as InUse and Usage count
			  has been incremented during allocation or Usage count has been incremented
			  if assigned.
			*/
			if (!gswdev->dscp2pcp_idx[dscp2pcp_pointer].IndexInUse) {
				ret = GSW_statusErr;
				goto UNLOCK_AND_RETURN;
			}

			/*Decrement the dscp2pcp index usage count.
			  If it becomes zero after decrement,then this index is free
			  for allocation by others.
			  In next allocation this dscp2pcp pointer's value
			  will be over written.
			 */
			gswdev->dscp2pcp_idx[dscp2pcp_pointer].IndexInUsageCnt--;

			if (gswdev->dscp2pcp_idx[dscp2pcp_pointer].IndexInUsageCnt == 0)
				gswdev->dscp2pcp_idx[dscp2pcp_pointer].IndexInUse = 0;

			gswdev->extendvlan_idx.vlan_idx[idx].Dscp2PcpPointerAssigned = 0;
			gswdev->extendvlan_idx.vlan_idx[idx].Dscp2PcpPointer = 0;
		}

		/** Treatment Metering **/
		if (gswdev->extendvlan_idx.vlan_idx[idx].MeterAssigned) {
			meterenable = ((tbl_prog.val[5] &  0x100) >> 8);
			meterid = (tbl_prog.val[5] & 0xFF);

			if (!meterenable) {
				pr_err("Meterid %d is assigned while allocation for this ExVlan idx=%d\n",
				       meterid, idx);
				pr_err("ERROR : why it is not enabled in Exvlan Table\n");
				ret = GSW_statusErr;
				goto UNLOCK_AND_RETURN;
			}

			gswdev->extendvlan_idx.vlan_idx[idx].MeterAssigned = 0;
			gswdev->extendvlan_idx.vlan_idx[idx].MeterId = 0;
			gswdev->meter_idx[meterid].IndexInUsageCnt--;
		}

		/*Zero the Key/Value REG and write to this ExtendedVlan table idx*/
		memset(&tbl_prog, 0, sizeof(pctbl_prog_t));
		tbl_prog.table = PCE_EXTVLAN_INDEX;
		CLEAR_U16(tbl_prog.pcindex);
		/*Table Entry address (Egress VLAN treatment index) Bit 9:0 in PCE_TBL_ADDR*/
		tbl_prog.pcindex |= (idx & 0x3FF);
		/*Address-based write*/
		gsw_pce_table_write(cdev, &tbl_prog);

		/*free this idx and decrement the the number of used table entries/idx*/
		gswdev->extendvlan_idx.vlan_idx[idx].IndexInUse = 0;
		/*(Entry 0-1023 is valid) EXVLAN_ENTRY_INVALID is 1024*/
		gswdev->extendvlan_idx.vlan_idx[idx].VlanBlockId = EXVLAN_ENTRY_INVALID;
		gswdev->extendvlan_idx.nUsedEntry--;
		/*A reference for the user, how many entries has been deleted in this block
		  debugging purpose*/
		param->nNumberOfEntries++;
		/*increment the idx,since the block's allocation in ADD is Contiguous*/
		idx++;
	}

	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_pce);
#endif
	return ret;


}

static u8 GSW_SearchVlanFilterContiguousBlock(ethsw_api_dev_t *gswdev, u32 BlockId, u32 NumberOfEntries)
{
	u32 i;

	for (i = BlockId; i <= (BlockId + NumberOfEntries); i++) {
		if (gswdev->vlanfilter_idx.filter_idx[i].IndexInUse)
			return 0;
	}

	return 1;
}

GSW_return_t GSW_VlanFilterAlloc(void *cdev, GSW_VLANFILTER_alloc_t *param)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u16 VlanFilterIndex;
	u8 ContiguousBlockFound = 0;
	u32 i, ret;

	/*
		Allocate New Block as per the number of table Entries requested
		The Block must be allocated with contiguous table index
	*/

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_pce);
#endif

	if (param->nNumberOfEntries > (gswdev->num_of_vlanfilter - gswdev->vlanfilter_idx.nUsedEntry)) {
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	for (VlanFilterIndex = 0; VlanFilterIndex < gswdev->num_of_vlanfilter && !ContiguousBlockFound; VlanFilterIndex++) {
		/*Table Index not in use*/
		if (!gswdev->vlanfilter_idx.filter_idx[VlanFilterIndex].IndexInUse) {
			ContiguousBlockFound = GSW_SearchVlanFilterContiguousBlock(gswdev, VlanFilterIndex, param->nNumberOfEntries);
		}

		/*Contiguous block found in the table*/
		if (ContiguousBlockFound) {
			param->nVlanFilterBlockId = VlanFilterIndex;

			/*Mark the contiguous table indexes as InUse and tag it with block id*/
			for (i = param->nVlanFilterBlockId; i < (param->nVlanFilterBlockId + param->nNumberOfEntries); i++) {
				gswdev->vlanfilter_idx.filter_idx[i].IndexInUse = 1;
				gswdev->vlanfilter_idx.filter_idx[i].FilterBlockId = param->nVlanFilterBlockId;
				/* The following are stored as part of Index information,
				   Since it will be used as part of Bridge Port Configuration (GSW_BRIDGE_PORT_CONFIG_SET).
				   It will be used to enable/disable the following in Bridge Port Configuration Table.
				   1. VLAN filtering untagged traffic forwarding mode
				   2. VLAN filtering tagged unmatched traffic forwarding mode
				   It is not part of VLAN Filter Table Configuration
				*/
				gswdev->vlanfilter_idx.filter_idx[i].DiscardUntagged = param->bDiscardUntagged;
				gswdev->vlanfilter_idx.filter_idx[i].DiscardUnMatchedTagged = param->bDiscardUnmatchedTagged;
				gswdev->vlanfilter_idx.nUsedEntry++;
			}
		}
	}

	/*Contiguous block not found in the table*/
	if (!ContiguousBlockFound) {
		ret = GSW_statusErr;
		pr_err(" ContiguousBlockFound %s:%s:%d", __FILE__, __func__, __LINE__);
		goto UNLOCK_AND_RETURN;
	}

	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_pce);
#endif
	return ret;

}

GSW_return_t GSW_VlanFilterSet(void *cdev, GSW_VLANFILTER_config_t *param)
{
	pctbl_prog_t tbl_prog;
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 idx = 0;
	u32 ret = 0;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_pce);
#endif

	/* Once the New blk allocated,param->nEntryIndex will decide which index with in this block
	   has to be set.only one index will be set at a time.
	   It is users responsibility to set all the index with in this block
	   by calling GSW_ExtendedVlanAdd as many times as required (i.e) param->nNumberOfEntries*/
	idx = param->nVlanFilterBlockId + param->nEntryIndex;

	if (idx >= gswdev->num_of_vlanfilter) {
		pr_err("ERROR: nVlanFilterBlockId idx %d  out of range [supported num_of_vlanfilter %d]\n", idx, (gswdev->num_of_vlanfilter - 1));
		goto UNLOCK_AND_RETURN;
	}

	if (!gswdev->vlanfilter_idx.filter_idx[idx].IndexInUse) {
		pr_err("ERROR: Index not InUse\n");
		goto UNLOCK_AND_RETURN;
	}

	if (gswdev->vlanfilter_idx.filter_idx[idx].FilterBlockId != param->nVlanFilterBlockId) {
		pr_err("gswdev->vlanfilter_idx.filter_idx[idx].FilterBlockId != param->nVlanFilterBlockId\n");
		goto UNLOCK_AND_RETURN;
	}

	memset(&tbl_prog, 0, sizeof(pctbl_prog_t));
	tbl_prog.table = PCE_VLANFILTER_INDEX;
	CLEAR_U16(tbl_prog.pcindex);
	/*Table Entry address (VLAN Filter index) Bit 11:0 in PCE_TBL_ADDR*/
	tbl_prog.pcindex |= (idx & 0xFFF);

	/*This is the Filter mask which will be applied for all the Index with in
	 this Allocated Block*/
	gswdev->vlanfilter_idx.filter_idx[idx].FilterMask = param->eVlanFilterMask;

	CLEAR_U16(tbl_prog.key[0]);

	/* KEY REG0 BIT 11:0 -VID,BIT 15:13-PCP,BIT 12-DEI*/
	switch (gswdev->vlanfilter_idx.filter_idx[idx].FilterMask) {
	case GSW_VLAN_FILTER_TCI_MASK_VID:
		tbl_prog.key[0] |= (param->nVal & 0xFFF);
		break;

	case GSW_VLAN_FILTER_TCI_MASK_PCP:
		tbl_prog.key[0] |= ((param->nVal & 0x7) << 13);
		break;

	case GSW_VLAN_FILTER_TCI_MASK_TCI:
		tbl_prog.key[0] |= (param->nVal & 0xFFFF);
		break;

	default:
		goto UNLOCK_AND_RETURN;
	}

	/*VLAN Filter Match action*/
	/* VAL REG0 BIT 0
	   0 - Traffic Allowed
	   1 - Traffic Blocked
	*/
	if (param->bDiscardMatched)
		tbl_prog.val[0] |= 1;
	else
		tbl_prog.val[0] &= ~(1);

	/*Address-based write*/
	gsw_pce_table_write(cdev, &tbl_prog);
	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_pce);
#endif
	return ret;
}

GSW_return_t GSW_VlanFilterGet(void *cdev, GSW_VLANFILTER_config_t *parm)
{
	pctbl_prog_t tbl_prog;
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 idx = 0;
	u32 ret = GSW_statusOk;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_pce);
#endif

	if (parm->nVlanFilterBlockId > gswdev->num_of_vlanfilter) {
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	idx = parm->nVlanFilterBlockId + parm->nEntryIndex;

	if (idx >= gswdev->num_of_vlanfilter) {
		pr_err("ERROR: nVlanFilterBlockId idx %d  out of range [supported num_of_vlanfilter %d]\n", idx, (gswdev->num_of_vlanfilter - 1));
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	/*Index is not in USE*/
	if (!gswdev->vlanfilter_idx.filter_idx[idx].IndexInUse) {
		pr_err("Error :Index not in use\n");
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	/*Index does not belongs to this VlanBlock*/
	if (gswdev->vlanfilter_idx.filter_idx[idx].FilterBlockId != parm->nVlanFilterBlockId) {
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	memset(&tbl_prog, 0, sizeof(pctbl_prog_t));
	tbl_prog.table = PCE_VLANFILTER_INDEX;
	CLEAR_U16(tbl_prog.pcindex);
	/*Table Entry address (VLAN Filter index) Bit 11:0 in PCE_TBL_ADDR*/
	tbl_prog.pcindex |= (idx & 0xFFF);
	/*Address-based read*/
	gsw_pce_table_read(cdev, &tbl_prog);

	parm->eVlanFilterMask = gswdev->vlanfilter_idx.filter_idx[idx].FilterMask;
//	parm->bDiscardUntagged=gswdev->vlanfilter_idx.filter_idx[idx].DiscardUntagged;
//	parm->bDiscardUnmatchedTagged=gswdev->vlanfilter_idx.filter_idx[idx].DiscardUnMatchedTagged;

	/* KEY REG0 BIT 11:0 -VID,BIT 15:13-PCP,BIT 12-DEI*/
	switch (gswdev->vlanfilter_idx.filter_idx[idx].FilterMask) {
	case GSW_VLAN_FILTER_TCI_MASK_VID:
		parm->nVal = (tbl_prog.key[0] & 0xFFF);
		break;

	case GSW_VLAN_FILTER_TCI_MASK_PCP:
		parm->nVal = ((tbl_prog.key[0] & 0xE000) >> 13);
		break;

	case GSW_VLAN_FILTER_TCI_MASK_TCI:
		parm->nVal = (tbl_prog.key[0] & 0xFFFF);
		break;

	default:
		goto UNLOCK_AND_RETURN;
	}

	parm->bDiscardMatched = (tbl_prog.val[0] & 0x1);

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_pce);
#endif
	return ret;
}

GSW_return_t GSW_VlanFilterFree(void *cdev, GSW_VLANFILTER_alloc_t *parm)
{
	pctbl_prog_t tbl_prog;
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 idx = 0;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_pce);
#endif

	if (parm->nVlanFilterBlockId >= gswdev->num_of_vlanfilter) {
		pr_err("ERROR: nVlanFilterBlockId idx %d  out of range [supported num_of_vlanfilter %d]\n", idx, (gswdev->num_of_vlanfilter - 1));
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	/*nVlanFilterBlockId should be in use,if not in use return error*/
	if (!gswdev->vlanfilter_idx.filter_idx[parm->nVlanFilterBlockId].IndexInUse) {
		pr_err("Error :Index not in use\n");
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	/*If this nVlanFilterBlockId usage count is not zero, that means it is still used by some one.
	  This Block can be deleted, only if that some one release this block*/
	if (gswdev->vlanfilter_idx.filter_idx[parm->nVlanFilterBlockId].IndexInUsageCnt) {
		pr_err("ERROR :Vlan Filter block %u is used by some resource,can not delete\n", parm->nVlanFilterBlockId);
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	idx = parm->nVlanFilterBlockId;
	parm->nNumberOfEntries = 0;

	if (gswdev->vlanfilter_idx.filter_idx[idx].FilterBlockId != parm->nVlanFilterBlockId) {
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	/*Condition to delete idx
		1. idx should belong to this block
		2. idx should be in Use
		3. idx should be with in valid VLAN Filter table range (Entry 0-1023 is valid)
	*/
	while (gswdev->vlanfilter_idx.filter_idx[idx].IndexInUse && idx < gswdev->num_of_vlanfilter
	       && (gswdev->vlanfilter_idx.filter_idx[idx].FilterBlockId == parm->nVlanFilterBlockId)) {
		/*Zero the Key/Value REG and write to this VLAN Filter table idx*/
		memset(&tbl_prog, 0, sizeof(pctbl_prog_t));
		tbl_prog.table = PCE_VLANFILTER_INDEX;
		CLEAR_U16(tbl_prog.pcindex);
		/*Table Entry address (VLAN Filter treatment index) Bit 9:0 in PCE_TBL_ADDR*/
		tbl_prog.pcindex |= (idx & 0xFFF);
		/*Address-based write*/
		gsw_pce_table_write(cdev, &tbl_prog);
		/*free this idx and decrement the the number of used table entries/idx*/
		gswdev->vlanfilter_idx.filter_idx[idx].IndexInUse = 0;
		/*(Entry 0-1023 is valid) VLANFILTER_ENTRY_INVALID is 1024*/
		gswdev->vlanfilter_idx.filter_idx[idx].FilterBlockId = VLANFILTER_ENTRY_INVALID;
		gswdev->vlanfilter_idx.nUsedEntry--;
		/*A reference for the user, how many entries has been deleted in this block
		  debugging purpose*/
		parm->nNumberOfEntries++;
		/*increment the idx,since the block's allocation in ADD is Contiguous*/
		idx++;
	}

	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_pce);
#endif
	return ret;
}

GSW_return_t GSW_BridgeAlloc(void *cdev, GSW_BRIDGE_alloc_t *param)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 idx = 0, freeidxfound = 0;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_pce);
#endif

	/*find a free Bridge configuration table index and allocate
	 New Bridge configuration table index
	 note: bridge port 0 is special purpose-can not be allocated*/
	for (idx = 1; idx < gswdev->num_of_bridge && !freeidxfound; idx++) {
		if (!(gswdev->brdgeconfig_idx[idx].IndexInUse)) {
			gswdev->brdgeconfig_idx[idx].IndexInUse = 1;
			param->nBridgeId = idx;
			freeidxfound = 1;
		}
	}

	/*No free Slot return Error*/
	if (!freeidxfound) {
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_pce);
#endif
	return ret;
}

GSW_return_t GSW_BridgeConfigSet(void *cdev, GSW_BRIDGE_config_t *param)
{
	pctbl_prog_t tbl_prog;
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 idx = 0, meterid;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_pce);
#endif


	if (param->nBridgeId >= gswdev->num_of_bridge) {
		pr_err("nBridgeId %d out of range [Suported num_of_bridge = %d]\n", param->nBridgeId, (gswdev->num_of_bridge - 1));
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}


	/*GSW_BRIDGE_CONFIG_MASK_FORCE is for debugging purpose only
	 if this mask is enabled , there is no check on index in-use*/
	if (!(param->eMask & GSW_BRIDGE_CONFIG_MASK_FORCE)) {
		/*If Bridge ID is valid,Check whether it is InUSE
		  if not InUse,return ERROR*/
		if (!(gswdev->brdgeconfig_idx[param->nBridgeId].IndexInUse)) {
			pr_err("ERROR nBridgeId %d: Index not in use\n", param->nBridgeId);
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}
	}

	memset(&tbl_prog, 0, sizeof(pctbl_prog_t));
	tbl_prog.table = PCE_BRGCFG_INDEX;
	CLEAR_U16(tbl_prog.pcindex);
	/*Table Entry address (Bridge Config Table index) Bit 9:0 in PCE_TBL_ADDR*/
	tbl_prog.pcindex |= (param->nBridgeId & 0x3F);

	/*Address-based read - Do Not disturb the existing value in case of
	 configuration update, so read first,then update only the required field*/
	gsw_pce_table_read(cdev, &tbl_prog);

	if (param->eMask & GSW_BRIDGE_CONFIG_MASK_MAC_LEARNING_LIMIT) {
		/*If Learning limit is enabled,Set the user defined limit
		  If Learning limit not enable,Set default limit -255*/
		if (param->bMacLearningLimitEnable) {
			tbl_prog.val[0] &= ~0xFF;
			tbl_prog.val[0] |= (param->nMacLearningLimit & 0xFF);
		} else
			/*Set default limit -255*/
			tbl_prog.val[0] |= (0xFF);
	}

	if (param->eMask & GSW_BRIDGE_CONFIG_MASK_SUB_METER) {
		/**
		Bridge Meter for bridge process with specific type
		As defined in GSW_BridgePortEgressMeter_t

		Index of broadcast traffic meter
		   GSW_BRIDGE_PORT_EGRESS_METER_BROADCAST = 0,
		Index of known multicast traffic meter
		   GSW_BRIDGE_PORT_EGRESS_METER_MULTICAST = 1,
		Index of unknown multicast IP traffic meter
		   GSW_BRIDGE_PORT_EGRESS_METER_UNKNOWN_MC_IP = 2,
		Index of unknown multicast non-IP traffic meter
		   GSW_BRIDGE_PORT_EGRESS_METER_UNKNOWN_MC_NON_IP = 3,
		Index of unknown unicast traffic meter
		   GSW_BRIDGE_PORT_EGRESS_METER_UNKNOWN_UC = 4,
		**/
		/*GSW_BRIDGE_PORT_EGRESS_METER_BROADCAST*/
		if (param->bSubMeteringEnable[0]) {
			meterid = param->nTrafficSubMeterId[0];

			/*This below field enable indicates that this meter id is
			 already assigned and it's idx recorded for this Bridge Port id.
			 If next time the user update this meter field with different meter idx !!
			 or wanted to allocate a new meter idx ??!! may happen by mistake ??!!
			 the previous meter idx must be released from this Bridge Port*/
			if (!(param->eMask & GSW_BRIDGE_CONFIG_MASK_FORCE)) {
				if (gswdev->brdgeconfig_idx[param->nBridgeId].BroadcastMeteringAssigned) {
					if (gswdev->brdgeconfig_idx[param->nBridgeId].BroadcastMeteringId !=
					    meterid) {
						idx = gswdev->brdgeconfig_idx[param->nBridgeId].BroadcastMeteringId;
						/*release the usage of previous meter idx*/
						gswdev->meter_idx[idx].IndexInUsageCnt--;
						gswdev->brdgeconfig_idx[param->nBridgeId].BroadcastMeteringAssigned = 0;
						gswdev->brdgeconfig_idx[param->nBridgeId].BroadcastMeteringId = 0;
					}
				}

				/*This Meter ID should be in use (i.e) that is this Meter ID should be allocated
				before calling bridge port configuration
				If not in use return ERROR
				*/
				if (gswdev->meter_idx[meterid].IndexInUse) {
					/*Usage count will be incremented only once during meter idx assignment
					for this bridge port id*/
					if (!gswdev->brdgeconfig_idx[param->nBridgeId].BroadcastMeteringAssigned) {
						gswdev->brdgeconfig_idx[param->nBridgeId].BroadcastMeteringAssigned = 1;
						gswdev->brdgeconfig_idx[param->nBridgeId].BroadcastMeteringId
							= meterid;
						/*Since this meter id can be shared,Increment the it's usage count*/
						gswdev->meter_idx[meterid].IndexInUsageCnt++;
					}

				} else {
					pr_err("BroadcastMeter Not allocated\n");
					ret = GSW_statusErr;
					goto UNLOCK_AND_RETURN;
				}
			}

			/*Clear the field first*/
			tbl_prog.val[4] &= ~(0x7F << 8);
			tbl_prog.val[4] |= 1;
			tbl_prog.val[4] |= ((param->nTrafficSubMeterId[0] & 0x7F) << 8);
		} else {
			/*Disable Broadcast sub meter*/
			tbl_prog.val[4] &= ~1;
		}

		/*GSW_BRIDGE_PORT_EGRESS_METER_MULTICAST*/
		if (param->bSubMeteringEnable[1]) {
			meterid = param->nTrafficSubMeterId[1];

			/*This below field enable indicates that this meter id is
			 already assigned and it's idx recorded for this Bridge Port id.
			 If next time the user update this meter field with different meter idx !!
			 or wanted to allocate a new meter idx ??!! may happen by mistake ??!!
			 the previous meter idx must be released from this Bridge Port*/
			if (!(param->eMask & GSW_BRIDGE_CONFIG_MASK_FORCE)) {
				if (gswdev->brdgeconfig_idx[param->nBridgeId].MulticastMeteringAssigned) {
					if (gswdev->brdgeconfig_idx[param->nBridgeId].MulticastMeteringId !=
					    meterid) {
						idx = gswdev->brdgeconfig_idx[param->nBridgeId].MulticastMeteringId;
						/*release the usage of previous meter idx*/
						gswdev->meter_idx[idx].IndexInUsageCnt--;
						gswdev->brdgeconfig_idx[param->nBridgeId].MulticastMeteringAssigned = 0;
						gswdev->brdgeconfig_idx[param->nBridgeId].MulticastMeteringId = 0;
					}
				}

				/*This Meter ID should be in use (i.e) that is this Meter ID should be allocated
				before calling bridge port configuration
				If not in use return ERROR
				*/
				if (gswdev->meter_idx[meterid].IndexInUse) {
					/*Usage count will be incremented only once during meter idx assignment
					for this bridge port id*/
					if (!gswdev->brdgeconfig_idx[param->nBridgeId].MulticastMeteringAssigned) {
						gswdev->brdgeconfig_idx[param->nBridgeId].MulticastMeteringAssigned = 1;
						gswdev->brdgeconfig_idx[param->nBridgeId].MulticastMeteringId
							= meterid;
						/*Since this meter id can be shared,Increment the it's usage count*/
						gswdev->meter_idx[meterid].IndexInUsageCnt++;
					}

				} else {
					pr_err("MulticastMeter Not allocated\n");
					ret = GSW_statusErr;
					goto UNLOCK_AND_RETURN;
				}
			}

			/*Clear the field first*/
			tbl_prog.val[5] &= ~(0x7F << 8);
			tbl_prog.val[5] |= 1;
			tbl_prog.val[5] |= ((param->nTrafficSubMeterId[1] & 0x7F) << 8);
		} else {
			/*Disable Multicast sub meter*/
			tbl_prog.val[5] &= ~1;
		}

		/*GSW_BRIDGE_PORT_EGRESS_METER_UNKNOWN_MC_IP*/
		if (param->bSubMeteringEnable[2]) {
			meterid = param->nTrafficSubMeterId[2];

			/*This below field enable indicates that this meter id is
			 already assigned and it's idx recorded for this Bridge Port id.
			 If next time the user update this meter field with different meter idx !!
			 or wanted to allocate a new meter idx ??!! may happen by mistake ??!!
			 the previous meter idx must be released from this Bridge Port*/
			if (!(param->eMask & GSW_BRIDGE_CONFIG_MASK_FORCE)) {
				if (gswdev->brdgeconfig_idx[param->nBridgeId].UnknownMultiIpMeteringAssigned) {
					if (gswdev->brdgeconfig_idx[param->nBridgeId].UnknownMultiIpMeteringId !=
					    meterid) {
						idx = gswdev->brdgeconfig_idx[param->nBridgeId].UnknownMultiIpMeteringId;
						/*release the usage of previous meter idx*/
						gswdev->meter_idx[idx].IndexInUsageCnt--;
						gswdev->brdgeconfig_idx[param->nBridgeId].UnknownMultiIpMeteringAssigned = 0;
						gswdev->brdgeconfig_idx[param->nBridgeId].UnknownMultiIpMeteringId = 0;
					}
				}

				/*This Meter ID should be in use (i.e) that is this Meter ID should be allocated
				before calling bridge port configuration
				If not in use return ERROR
				*/
				if (gswdev->meter_idx[meterid].IndexInUse) {
					/*Usage count will be incremented only once during meter idx assignment
					for this bridge port id*/
					if (!gswdev->brdgeconfig_idx[param->nBridgeId].UnknownMultiIpMeteringAssigned) {
						gswdev->brdgeconfig_idx[param->nBridgeId].UnknownMultiIpMeteringAssigned = 1;
						gswdev->brdgeconfig_idx[param->nBridgeId].UnknownMultiIpMeteringId
							= meterid;
						/*Since this meter id can be shared,Increment the it's usage count*/
						gswdev->meter_idx[meterid].IndexInUsageCnt++;
					}
				} else {
					pr_err("UnknownMultiIpMeter Not allocated\n");
					ret = GSW_statusErr;
					goto UNLOCK_AND_RETURN;
				}
			}

			/*Clear the field first*/
			tbl_prog.val[8] &= ~(0x7F << 8);
			tbl_prog.val[8] |= 1;
			tbl_prog.val[8] |= ((param->nTrafficSubMeterId[2] & 0x7F) << 8);
		} else {
			/*Disable Unknown Multicast ip sub meter*/
			tbl_prog.val[8] &= ~1;
		}

		/*GSW_BRIDGE_PORT_EGRESS_METER_UNKNOWN_MC_NON_IP*/
		if (param->bSubMeteringEnable[3]) {
			meterid = param->nTrafficSubMeterId[3];

			/*This below field enable indicates that this meter id is
			 already assigned and it's idx recorded for this Bridge Port id.
			 If next time the user update this meter field with different meter idx !!
			 or wanted to allocate a new meter idx ??!! may happen by mistake ??!!
			 the previous meter idx must be released from this Bridge Port*/
			if (!(param->eMask & GSW_BRIDGE_CONFIG_MASK_FORCE)) {
				if (gswdev->brdgeconfig_idx[param->nBridgeId].UnknownMultiNonIpMeteringAssigned) {
					if (gswdev->brdgeconfig_idx[param->nBridgeId].UnknownMultiNonIpMeteringId !=
					    meterid) {
						idx = gswdev->brdgeconfig_idx[param->nBridgeId].UnknownMultiNonIpMeteringId;
						/*release the usage of previous meter idx*/
						gswdev->meter_idx[meterid].IndexInUsageCnt--;
						gswdev->brdgeconfig_idx[param->nBridgeId].UnknownMultiNonIpMeteringAssigned = 0;
						gswdev->brdgeconfig_idx[param->nBridgeId].UnknownMultiNonIpMeteringId = 0;
					}
				}

				/*This Meter ID should be in use (i.e) that is this Meter ID should be allocated
				before calling bridge port configuration
				If not in use return ERROR
				*/
				if (gswdev->meter_idx[meterid].IndexInUse) {
					/*Usage count will be incremented only once during meter idx assignment
					for this bridge port id*/
					if (!gswdev->brdgeconfig_idx[param->nBridgeId].UnknownMultiNonIpMeteringAssigned) {
						gswdev->brdgeconfig_idx[param->nBridgeId].UnknownMultiNonIpMeteringAssigned = 1;
						gswdev->brdgeconfig_idx[param->nBridgeId].UnknownMultiNonIpMeteringId
							= meterid;
						/*Since this meter id can be shared,Increment the it's usage count*/
						gswdev->meter_idx[meterid].IndexInUsageCnt++;
					}

				} else {
					pr_err("UnknownMultiNonIpMeter Not allocated\n");
					ret = GSW_statusErr;
					goto UNLOCK_AND_RETURN;
				}
			}

			/*Clear the field first*/
			tbl_prog.val[7] &= ~(0x7F << 8);
			tbl_prog.val[7] |= 1;
			tbl_prog.val[7] |= ((param->nTrafficSubMeterId[3] & 0x7F) << 8);
		} else {
			/*Disable Unknown Multicast Non-ip sub meter*/
			tbl_prog.val[7] &= ~1;
		}

		/*GSW_BRIDGE_PORT_EGRESS_METER_UNKNOWN_UC*/
		if (param->bSubMeteringEnable[4]) {
			meterid = param->nTrafficSubMeterId[4];

			/*This below field enable indicates that this meter id is
			 already assigned and it's idx recorded for this Bridge Port id.
			 If next time the user update this meter field with different meter idx !!
			 or wanted to allocate a new meter idx ??!! may happen by mistake ??!!
			 the previous meter idx must be released from this Bridge Port*/
			if (!(param->eMask & GSW_BRIDGE_CONFIG_MASK_FORCE)) {
				if (gswdev->brdgeconfig_idx[param->nBridgeId].UnknownUniCastMeteringAssigned) {
					if (gswdev->brdgeconfig_idx[param->nBridgeId].UnknownUniCastMeteringId !=
					    meterid) {
						idx = gswdev->brdgeconfig_idx[param->nBridgeId].UnknownUniCastMeteringId;
						/*release the usage of previous meter idx*/
						gswdev->meter_idx[meterid].IndexInUsageCnt--;
						gswdev->brdgeconfig_idx[param->nBridgeId].UnknownUniCastMeteringAssigned = 0;
						gswdev->brdgeconfig_idx[param->nBridgeId].UnknownUniCastMeteringId = 0;
					}
				}

				/*This Meter ID should be in use (i.e) that is this Meter ID should be allocated
				before calling bridge port configuration
				If not in use return ERROR
				*/
				if (gswdev->meter_idx[meterid].IndexInUse) {
					/*Usage count will be incremented only once during meter idx assignment
					for this bridge port id*/
					if (!gswdev->brdgeconfig_idx[param->nBridgeId].UnknownUniCastMeteringAssigned) {
						gswdev->brdgeconfig_idx[param->nBridgeId].UnknownUniCastMeteringAssigned = 1;
						gswdev->brdgeconfig_idx[param->nBridgeId].UnknownUniCastMeteringId
							= meterid;
						/*Since this meter id can be shared,Increment the it's usage count*/
						gswdev->meter_idx[meterid].IndexInUsageCnt++;
					}
				} else {
					pr_err("UnknownUniCastMeter Not allocated\n");
					ret = GSW_statusErr;
					goto UNLOCK_AND_RETURN;
				}
			}

			/*Clear the field first*/
			tbl_prog.val[6] &= ~(0x7F << 8);
			tbl_prog.val[6] |= 1;
			tbl_prog.val[6] |= ((param->nTrafficSubMeterId[4] & 0x7F) << 8);
		} else {
			/*Disable Unknown Unicast sub meter*/
			tbl_prog.val[6] |= ~1;
		}
	}

	if (param->eMask & GSW_BRIDGE_CONFIG_MASK_FORWARDING_MODE) {
		/*Clear the field first*/
		tbl_prog.val[1] &= ~0x3;

		switch (param->eForwardBroadcast) {
		case GSW_BRIDGE_FORWARD_FLOOD:
			break;

		case GSW_BRIDGE_FORWARD_DISCARD:
			tbl_prog.val[1] |= 0x1;
			break;

		case GSW_BRIDGE_FORWARD_CPU:
			tbl_prog.val[1] |= 0x2;
			break;
		}

		/*Clear the field first*/
		tbl_prog.val[1] &= ~0xC;

		switch (param->eForwardUnknownUnicast) {
		case GSW_BRIDGE_FORWARD_FLOOD:
			break;

		case GSW_BRIDGE_FORWARD_DISCARD:
			tbl_prog.val[1] |= (0x1 << 2);
			break;

		case GSW_BRIDGE_FORWARD_CPU:
			tbl_prog.val[1] |= (0x2 << 2);
			break;
		}

		/*Clear the field first*/
		tbl_prog.val[1] &= ~0x30;

		switch (param->eForwardUnknownMulticastNonIp) {
		case GSW_BRIDGE_FORWARD_FLOOD:
			break;

		case GSW_BRIDGE_FORWARD_DISCARD:
			tbl_prog.val[1] |= (0x1 << 4);
			break;

		case GSW_BRIDGE_FORWARD_CPU:
			tbl_prog.val[1] |= (0x2 << 4);
			break;
		}

		/*Clear the field first*/
		tbl_prog.val[1] &= ~0xC0;

		switch (param->eForwardUnknownMulticastIp) {
		case GSW_BRIDGE_FORWARD_FLOOD:
			break;

		case GSW_BRIDGE_FORWARD_DISCARD:
			tbl_prog.val[1] |= (0x1 << 6);
			break;

		case GSW_BRIDGE_FORWARD_CPU:
			tbl_prog.val[1] |= (0x2 << 6);
			break;
		}
	}

	tbl_prog.table = PCE_BRGCFG_INDEX;
	CLEAR_U16(tbl_prog.pcindex);
	/*Table Entry address (Bridge Config Table index) Bit 5:0 in PCE_TBL_ADDR*/
	tbl_prog.pcindex |= (param->nBridgeId & 0x3F);

	/*Address-based write*/
	gsw_pce_table_write(cdev, &tbl_prog);
	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_pce);
#endif
	return ret;

}

GSW_return_t GSW_BridgeConfigGet(void *cdev, GSW_BRIDGE_config_t *param)
{
	pctbl_prog_t tbl_prog;
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 ForwardMode;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_pce);
#endif

	if (param->nBridgeId >= gswdev->num_of_bridge) {
		pr_err("nBridgeId %d out of range [Suported num_of_bridge = %d]\n", param->nBridgeId, (gswdev->num_of_bridge - 1));
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	/*GSW_BRIDGE_CONFIG_MASK_FORCE is for debugging purpose only
	 if this mask is enabled , there is no check on index in-use*/
	if (!(param->eMask & GSW_BRIDGE_CONFIG_MASK_FORCE)) {
		/*If Bridge ID is valid,Check whether it is InUSE
		  if not InUse,return ERROR*/
		if (!gswdev->brdgeconfig_idx[param->nBridgeId].IndexInUse) {
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}
	}

	memset(&tbl_prog, 0, sizeof(pctbl_prog_t));
	tbl_prog.table = PCE_BRGCFG_INDEX;
	CLEAR_U16(tbl_prog.pcindex);
	/*Table Entry address (Bridge Config Table index) Bit 5:0 in PCE_TBL_ADDR*/
	tbl_prog.pcindex |= (param->nBridgeId & 0x3F);
	/*Address-based read*/
	gsw_pce_table_read(cdev, &tbl_prog);

	if (param->eMask & GSW_BRIDGE_CONFIG_MASK_MAC_LEARNING_LIMIT) {
		param->nMacLearningLimit = (tbl_prog.val[0] & 0xFF);

		if (param->nMacLearningLimit != 255)
			param->bMacLearningLimitEnable = 1;
	}

	if (param->eMask & GSW_BRIDGE_CONFIG_MASK_SUB_METER) {
		/**
		Bridge Meter for bridge process with specific type
		As defined in GSW_BridgePortEgressMeter_t

		Index of broadcast traffic meter
		   GSW_BRIDGE_PORT_EGRESS_METER_BROADCAST = 0,
		Index of known multicast traffic meter
		   GSW_BRIDGE_PORT_EGRESS_METER_MULTICAST = 1,
		Index of unknown multicast IP traffic meter
		   GSW_BRIDGE_PORT_EGRESS_METER_UNKNOWN_MC_IP = 2,
		Index of unknown multicast non-IP traffic meter
		   GSW_BRIDGE_PORT_EGRESS_METER_UNKNOWN_MC_NON_IP = 3,
		Index of unknown unicast traffic meter
		   GSW_BRIDGE_PORT_EGRESS_METER_UNKNOWN_UC = 4,
		**/
		/*Broadcast*/
		param->bSubMeteringEnable[0] = (tbl_prog.val[4] & 0x1);

		/*Meter ID*/
		if (param->bSubMeteringEnable[0])
			param->nTrafficSubMeterId[0] = ((tbl_prog.val[4] & 0x7F00) >> 8);

		/*Multicast*/
		param->bSubMeteringEnable[1] = (tbl_prog.val[5] & 0x1);

		/*Meter ID*/
		if (param->bSubMeteringEnable[1])
			param->nTrafficSubMeterId[1] = ((tbl_prog.val[5] & 0x7F00) >> 8);

		/*Unknown Multicast IP*/
		param->bSubMeteringEnable[2] = (tbl_prog.val[8] & 0x1);

		/*Meter ID*/
		if (param->bSubMeteringEnable[2])
			param->nTrafficSubMeterId[2] = ((tbl_prog.val[8] & 0x7F00) >> 8);

		/*Unknown Multicast NON-IP*/
		param->bSubMeteringEnable[3] = (tbl_prog.val[7] & 0x1);

		/*Meter ID*/
		if (param->bSubMeteringEnable[3])
			param->nTrafficSubMeterId[3] = ((tbl_prog.val[7] & 0x7F00) >> 8);

		/*Unknown Uni-Cast*/
		param->bSubMeteringEnable[4] = (tbl_prog.val[6] & 0x1);

		/*Meter ID*/
		if (param->bSubMeteringEnable[4])
			param->nTrafficSubMeterId[4] = ((tbl_prog.val[6] & 0x7F00) >> 8);
	}

	if (param->eMask & GSW_BRIDGE_CONFIG_MASK_FORWARDING_MODE) {
		ForwardMode = (tbl_prog.val[1] & 0x3);

		switch (ForwardMode) {
		case 0:
			param->eForwardBroadcast = GSW_BRIDGE_FORWARD_FLOOD;
			break;

		case 1:
			param->eForwardBroadcast = GSW_BRIDGE_FORWARD_DISCARD;
			break;

		case 2:
			param->eForwardBroadcast = GSW_BRIDGE_FORWARD_CPU;
			break;
		}

		ForwardMode = ((tbl_prog.val[1] & 0xC0) >> 6);

		switch (ForwardMode) {
		case 0:
			param->eForwardUnknownMulticastIp = GSW_BRIDGE_FORWARD_FLOOD;
			break;

		case 1:
			param->eForwardUnknownMulticastIp = GSW_BRIDGE_FORWARD_DISCARD;
			break;

		case 2:
			param->eForwardUnknownMulticastIp = GSW_BRIDGE_FORWARD_CPU;
			break;
		}

		ForwardMode = ((tbl_prog.val[1] & 0x30) >> 4);

		switch (ForwardMode) {
		case 0:
			param->eForwardUnknownMulticastNonIp = GSW_BRIDGE_FORWARD_FLOOD;
			break;

		case 1:
			param->eForwardUnknownMulticastNonIp = GSW_BRIDGE_FORWARD_DISCARD;
			break;

		case 2:
			param->eForwardUnknownMulticastNonIp = GSW_BRIDGE_FORWARD_CPU;
			break;
		}

		ForwardMode = ((tbl_prog.val[1] & 0xC) >> 2);

		switch (ForwardMode) {
		case 0:
			param->eForwardUnknownUnicast = GSW_BRIDGE_FORWARD_FLOOD;
			break;

		case 1:
			param->eForwardUnknownUnicast = GSW_BRIDGE_FORWARD_DISCARD;
			break;

		case 2:
			param->eForwardUnknownUnicast = GSW_BRIDGE_FORWARD_CPU;
			break;
		}
	}

	if (param->eMask & GSW_BRIDGE_CONFIG_MASK_MAC_LEARNED_COUNT) {
		param->nMacLearningCount = (tbl_prog.val[9] & 0x1FFF);
	}

	if (param->eMask & GSW_BRIDGE_CONFIG_MASK_MAC_DISCARD_COUNT) {
		param->nLearningDiscardEvent |= ((tbl_prog.val[2] & 0xFFFF));
		param->nLearningDiscardEvent |= (((tbl_prog.val[3] & 0xFFFF) << 16));
	}

	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_pce);
#endif
	return ret;
}

GSW_return_t GSW_BridgeFree(void *cdev, GSW_BRIDGE_alloc_t *param)
{
	pctbl_prog_t tbl_prog;
	GSW_BRIDGE_config_t temp;
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 count;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_pce);
#endif

	if (param->nBridgeId >= gswdev->num_of_bridge) {
		pr_err("nBridgeId %d out of range [Suported num_of_bridge = %d]\n", param->nBridgeId, gswdev->num_of_bridge);
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	/*Bridge ID should be in use,if not in use return error*/
	if (!gswdev->brdgeconfig_idx[param->nBridgeId].IndexInUse) {
		pr_err("nBridge %u is Not inUse -> need to allocate before freeing\n", param->nBridgeId);
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	/*If this Bridge usage count is not zero that means it is still used by some one.
	  This Bridge configuration can be deleted, only if that some one release this Bridge*/
	if (gswdev->brdgeconfig_idx[param->nBridgeId].IndexInUsageCnt) {
		count = gswdev->brdgeconfig_idx[param->nBridgeId].IndexInUsageCnt;
		pr_err("nBridge %u IndexInUsageCnt = %u is not zero ?!,Some Bridge Ports is holding this Bridge ?\n",
		       param->nBridgeId, count);
		pr_err("Free that Bridge Ports first,which will detach this association\n");
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	/** IMPROTANT : Release the Enabled Sub Meters associated with this bridge
	    by Decrementing it's Usage count **/

	memset(&tbl_prog, 0, sizeof(pctbl_prog_t));
	tbl_prog.table = PCE_BRGCFG_INDEX;
	CLEAR_U16(tbl_prog.pcindex);
	/*Table Entry address (Bridge Configuration Table index) Bit 5:0 in PCE_TBL_ADDR*/
	tbl_prog.pcindex |= (param->nBridgeId & 0x3F);
	/*Address-based read*/
	gsw_pce_table_read(cdev, &tbl_prog);


	/*Egress Broadcast Meter*/
	temp.nTrafficSubMeterId[0] = ((tbl_prog.val[4] & 0x7F00) >> 8);

	if (gswdev->brdgeconfig_idx[param->nBridgeId].BroadcastMeteringAssigned) {
		if (gswdev->brdgeconfig_idx[param->nBridgeId].BroadcastMeteringId ==
		    temp.nTrafficSubMeterId[0]) {
			/*Release this meter id from this Bridge*/
			gswdev->meter_idx[temp.nTrafficSubMeterId[0]].IndexInUsageCnt--;
			gswdev->brdgeconfig_idx[param->nBridgeId].BroadcastMeteringAssigned = 0;
			gswdev->brdgeconfig_idx[param->nBridgeId].BroadcastMeteringId = 0;

		} else {
			/*if assigned and idx does not match with recorded idx.
			 some thing is wrong.This should never happen*/
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}
	}

	/*Egress Multicast Meter*/
	temp.nTrafficSubMeterId[1] = ((tbl_prog.val[5] & 0x7F00) >> 8);

	if (gswdev->brdgeconfig_idx[param->nBridgeId].MulticastMeteringAssigned) {
		if (gswdev->brdgeconfig_idx[param->nBridgeId].MulticastMeteringId ==
		    temp.nTrafficSubMeterId[1]) {
			/*Release this meter id from this Bridge*/
			gswdev->meter_idx[temp.nTrafficSubMeterId[1]].IndexInUsageCnt--;
			gswdev->brdgeconfig_idx[param->nBridgeId].MulticastMeteringAssigned = 0;
			gswdev->brdgeconfig_idx[param->nBridgeId].MulticastMeteringId = 0;

		} else {
			/*if assigned and idx does not match with recorded idx.
			 some thing is wrong.This should never happen*/
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}
	}

	/*Egress Unknown Multicast IP Meter*/
	temp.nTrafficSubMeterId[2] = ((tbl_prog.val[8] & 0x7F00) >> 8);

	if (gswdev->brdgeconfig_idx[param->nBridgeId].UnknownMultiIpMeteringAssigned) {
		if (gswdev->brdgeconfig_idx[param->nBridgeId].UnknownMultiIpMeteringId ==
		    temp.nTrafficSubMeterId[2]) {
			/*Release this meter id from this Bridge*/
			gswdev->meter_idx[temp.nTrafficSubMeterId[2]].IndexInUsageCnt--;
			gswdev->brdgeconfig_idx[param->nBridgeId].UnknownMultiIpMeteringAssigned = 0;
			gswdev->brdgeconfig_idx[param->nBridgeId].UnknownMultiIpMeteringId = 0;

		} else {
			/*if assigned and idx does not match with recorded idx.
			 some thing is wrong.This should never happen*/
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}
	}

	/*Egress Unknown Multicast NON IP Meter*/
	temp.nTrafficSubMeterId[3] = ((tbl_prog.val[7] & 0x7F00) >> 8);

	if (gswdev->brdgeconfig_idx[param->nBridgeId].UnknownMultiNonIpMeteringAssigned) {
		if (gswdev->brdgeconfig_idx[param->nBridgeId].UnknownMultiNonIpMeteringId ==
		    temp.nTrafficSubMeterId[3]) {
			/*Release this meter id from this Bridge*/
			gswdev->meter_idx[temp.nTrafficSubMeterId[3]].IndexInUsageCnt--;
			gswdev->brdgeconfig_idx[param->nBridgeId].UnknownMultiNonIpMeteringAssigned = 0;
			gswdev->brdgeconfig_idx[param->nBridgeId].UnknownMultiNonIpMeteringId = 0;

		} else {
			/*if assigned and idx does not match with recorded idx.
			 some thing is wrong.This should never happen*/
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}
	}

	/*Egress Unknown UniCast Meter*/
	temp.nTrafficSubMeterId[4] = ((tbl_prog.val[6] & 0x7F00) >> 8);

	if (gswdev->brdgeconfig_idx[param->nBridgeId].UnknownUniCastMeteringAssigned) {
		if (gswdev->brdgeconfig_idx[param->nBridgeId].UnknownUniCastMeteringId ==
		    temp.nTrafficSubMeterId[4]) {
			/*Release this meter id from this Bridge*/
			gswdev->meter_idx[temp.nTrafficSubMeterId[4]].IndexInUsageCnt--;
			gswdev->brdgeconfig_idx[param->nBridgeId].UnknownUniCastMeteringAssigned = 0;
			gswdev->brdgeconfig_idx[param->nBridgeId].UnknownUniCastMeteringId = 0;

		} else {
			/*if assigned and idx does not match with recorded idx.
			 some thing is wrong.This should never happen*/
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}
	}

	/** Clear the table idx**/
	/*reset to default this bridge idx*/
	memset(&tbl_prog, 0, sizeof(pctbl_prog_t));
	tbl_prog.table = PCE_BRGCFG_INDEX;
	CLEAR_U16(tbl_prog.pcindex);
	/*Table Entry address (Bridge Configuration Table index) Bit 5:0 in PCE_TBL_ADDR*/
	tbl_prog.pcindex |= (param->nBridgeId & 0x3F);

	/*Set default limit -255*/
	tbl_prog.val[0] |= (0xFF);
	gsw_pce_table_write(cdev, &tbl_prog);
	/*Free this bridge Idx*/
	gswdev->brdgeconfig_idx[param->nBridgeId].IndexInUse = 0;

	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_pce);
#endif
	return ret;
}

GSW_return_t GSW_BridgePortAlloc(void *cdev, GSW_BRIDGE_portAlloc_t *param)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	pctbl_prog_t tbl_prog_brdgeport_ingress;
	pctbl_prog_t tbl_prog_brdgeport_egress;
	u32 idx = 0;
	u16 freeidxfound = 0;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_pce);
#endif

	/*Find a free Bridge port configuration table index
	  and allocate
	  New Bridge Port configuration table index
	  Bridge Port 0,1 and 127 is reserved ,
	  So will not be allocated to user*/
	for (idx = 2; idx < (gswdev->num_of_bridge_port - 1) && !freeidxfound; idx++) {
		if (!(gswdev->brdgeportconfig_idx[idx].IndexInUse)) {
			gswdev->brdgeportconfig_idx[idx].IndexInUse = 1;
			param->nBridgePortId = idx;
			freeidxfound = 1;
		}
	}

	/*No free Slot return Error*/
	if (!freeidxfound) {
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	memset(&tbl_prog_brdgeport_ingress, 0, sizeof(pctbl_prog_t));
	memset(&tbl_prog_brdgeport_egress, 0, sizeof(pctbl_prog_t));

	/*Learning Limit 255*/
	tbl_prog_brdgeport_ingress.val[4] |= 0xFF;
	gswdev->brdgeportconfig_idx[param->nBridgePortId].LearningLimit = 0xFF;
	/*STP state Forward enable*/
	tbl_prog_brdgeport_ingress.val[0] |= 0x7;
	tbl_prog_brdgeport_egress.val[0] |= 0x7;

	/*Default STP State is GSW_STP_PORT_STATE_FORWARD
	  it can be changed using GSW_Stp_PortCfgSet*/
	gswdev->brdgeportconfig_idx[param->nBridgePortId].StpState =
		GSW_STP_PORT_STATE_FORWARD;
	/*By Default 8021X State is GSW_8021X_PORT_STATE_AUTHORIZED
	  it can be changed using GSW_8021X_PortCfgSet*/
	gswdev->brdgeportconfig_idx[param->nBridgePortId].P8021xState =
		GSW_8021X_PORT_STATE_AUTHORIZED;
	/*NOTE :Bridge Port ingrees port map and Egress destination
	  logical port will be zero,It is up to the User to configure
	  these fields using GSW_BridgePortConfigSet */

	/*Same bridge port idx for ingress and egress bridge port configuration*/
	tbl_prog_brdgeport_ingress.table = PCE_IGBGP_INDEX;
	CLEAR_U16(tbl_prog_brdgeport_ingress.pcindex);
	/*Table Entry address (Bridge port ingress Table index) Bit 7:0 in PCE_TBL_ADDR*/
	tbl_prog_brdgeport_ingress.pcindex |= (param->nBridgePortId & 0xFF);

	tbl_prog_brdgeport_egress.table = PCE_EGBGP_INDEX;
	CLEAR_U16(tbl_prog_brdgeport_egress.pcindex);
	/*Table Entry address (Bridge port egress Table index) Bit 7:0 in PCE_TBL_ADDR*/
	tbl_prog_brdgeport_egress.pcindex |= (param->nBridgePortId & 0xFF);

	/*Address-based write for ingress bridge port configuration*/
	gsw_pce_table_write(cdev, &tbl_prog_brdgeport_ingress);
	/*Address-based write for egress bridge port configuration*/
	gsw_pce_table_write(cdev, &tbl_prog_brdgeport_egress);

	ret = GSW_statusOk;


UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_pce);
#endif
	return ret;
}

GSW_return_t GSW_BridgePortConfigSet(void *cdev, GSW_BRIDGE_portConfig_t *param)
{
	pctbl_prog_t tbl_prog_brdgeport_ingress;
	pctbl_prog_t tbl_prog_brdgeport_egress;
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 idx = 0, FirstIdx, LastIdx, pmapper_idx, meterid;
	u16 i, val_reg_idx, ret, IngressStpState, EgressStpState;
	u32 BlkSize = 0;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_pce);
#endif

	if (param->nBridgePortId >= gswdev->num_of_bridge_port) {
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	/*GSW_BRIDGE_PORT_CONFIG_MASK_FORCE is for debugging purpose only
	 if this mask is enabled , there is no check on index in-use*/
	if (!(param->eMask & GSW_BRIDGE_PORT_CONFIG_MASK_FORCE)) {
		/*If Bridge Port ID is valid,Check whether it is InUSE
		  if not InUse,return ERROR*/
		if (!(gswdev->brdgeportconfig_idx[param->nBridgePortId].IndexInUse)) {
			pr_err("nBridgePortId %d Index not InUse\n", param->nBridgePortId);
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}
	}

	/*Same bridge port idx for ingress and egress bridge port configuration*/
	memset(&tbl_prog_brdgeport_ingress, 0, sizeof(pctbl_prog_t));
	tbl_prog_brdgeport_ingress.table = PCE_IGBGP_INDEX;

	CLEAR_U16(tbl_prog_brdgeport_ingress.pcindex);
	/*Table Entry address (Bridge port ingress Table index) Bit 7:0 in PCE_TBL_ADDR*/
	tbl_prog_brdgeport_ingress.pcindex |= (param->nBridgePortId & 0xFF);

	memset(&tbl_prog_brdgeport_egress, 0, sizeof(pctbl_prog_t));
	tbl_prog_brdgeport_egress.table = PCE_EGBGP_INDEX;
	CLEAR_U16(tbl_prog_brdgeport_egress.pcindex);
	/*Table Entry address (Bridge port egress Table index) Bit 7:0 in PCE_TBL_ADDR*/
	tbl_prog_brdgeport_egress.pcindex |= (param->nBridgePortId & 0xFF);

	/*Address-based read for ingress bridge port configuration
	  read before set, so that previous value of this table index
	  is not disturbed in the case of update specific fields.
	*/
	gsw_pce_table_read(cdev, &tbl_prog_brdgeport_ingress);

	/*Address-based read for egress bridge port configuration
	  read before set, so that previous value of this table index
	  is not disturbed in the case of update specific fields.
	*/
	gsw_pce_table_read(cdev, &tbl_prog_brdgeport_egress);

	/**Default Bridge Port Configuration Settings**/
	/*By Default STP State are GSW_STP_PORT_STATE_FORWARD.
	  it can be changed using GSW_STP_PortCfgSet
	  GSW_BridgePortConfigSet will not set any STP related
	  configuration in BridgePort Table*/
	/*STP State - ingress*/
	IngressStpState = (tbl_prog_brdgeport_ingress.val[0] & 0x7);
	/*STP State - egress*/
	EgressStpState = (tbl_prog_brdgeport_egress.val[0] & 0x7);

	/*Both ingress/egress states must be same
	 if not equal return error*/
	if (IngressStpState != EgressStpState) {
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	/*By default MAC Learning Limit default 255
	  It can be changed using eMask
	  GSW_BRIDGE_PORT_CONFIG_MASK_MAC_LEARNING_LIMIT
	  and bMacLearningLimitEnable in GSW_BridgePortConfigSet*/
	gswdev->brdgeportconfig_idx[param->nBridgePortId].LearningLimit =
		(tbl_prog_brdgeport_ingress.val[4] & 0xFF);

	if (param->eMask & GSW_BRIDGE_PORT_CONFIG_MASK_BRIDGE_ID) {
		if (param->nBridgeId >= gswdev->num_of_bridge) {
			pr_err("Error : nBridgeId %d >= gswdev->num_of_bridge %d\n",
			       param->nBridgeId, gswdev->num_of_bridge);
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}

		/*GSW_BRIDGE_PORT_CONFIG_MASK_FORCE is for debugging purpose only
		if this mask is enabled , there is no check on index in-use*/
		if (!(param->eMask & GSW_BRIDGE_PORT_CONFIG_MASK_FORCE)) {
			/*This below field enable indicates that this bridge id is
			 already assigned and it's idx recorded for this Bridge Port id.
			 If next time the user update this meter field with different bridge idx !!
			 or wanted to allocate a new bridge idx ??!! may happen by mistake ??!!
			 the previous bridge idx must be released from this Bridge Port*/
			if (gswdev->brdgeportconfig_idx[param->nBridgePortId].BrdgIdAssigned) {
				if (gswdev->brdgeportconfig_idx[param->nBridgePortId].BrdgId !=
				    param->nBridgeId) {
					idx = gswdev->brdgeportconfig_idx[param->nBridgePortId].BrdgId;
					/*release the usage of previous bridge idx*/
					gswdev->brdgeconfig_idx[idx].IndexInUsageCnt--;
					gswdev->brdgeportconfig_idx[param->nBridgePortId].BrdgIdAssigned = 0;
					gswdev->brdgeportconfig_idx[param->nBridgePortId].BrdgId = 0;
				}
			}

			/*This Bridge ID should be in use (i.e) that is this Bridge ID should be allocated
			  before calling bridge port configuration
			  If not in use return ERROR
			  */
			if (gswdev->brdgeconfig_idx[param->nBridgeId].IndexInUse) {
				/*Usage count will be incremented only once during bridge idx assignment
				for this bridge port id*/
				if (!gswdev->brdgeportconfig_idx[param->nBridgePortId].BrdgIdAssigned) {
					gswdev->brdgeportconfig_idx[param->nBridgePortId].BrdgIdAssigned = 1;
					gswdev->brdgeportconfig_idx[param->nBridgePortId].BrdgId
						= param->nBridgeId;
					/*Since this bridge id can be shared,Increment it's usage count*/
					gswdev->brdgeconfig_idx[param->nBridgeId].IndexInUsageCnt++;
				}
			} else {
				ret = GSW_statusErr;
				goto UNLOCK_AND_RETURN;
			}

		}

		/*Clear the Fields*/
		tbl_prog_brdgeport_ingress.val[4] &= ~(0x3F << 8);
		tbl_prog_brdgeport_ingress.val[4] |= ((param->nBridgeId & 0x3F) << 8);
	}

	if (param->eMask & GSW_BRIDGE_PORT_CONFIG_MASK_INGRESS_VLAN) {
		if (param->bIngressExtendedVlanEnable) {
			if (!(param->eMask & GSW_BRIDGE_PORT_CONFIG_MASK_FORCE)) {
				/*This below field enable indicates that this vlan block is
				 already assigned and it's Blk id recorded for this Bridge Port id.
				 If next time the user update this Vlan blk field with different blk id !!
				 or wanted to allocate a new vlan blk ??!! may happen by mistake ??!!
				 the previous blk id must be released from this Bridge Port*/
				if (gswdev->brdgeportconfig_idx[param->nBridgePortId].IngressExVlanBlkAssigned) {
					if (gswdev->brdgeportconfig_idx[param->nBridgePortId].IngressExVlanBlkId !=
					    param->nIngressExtendedVlanBlockId) {
						idx = gswdev->brdgeportconfig_idx[param->nBridgePortId].IngressExVlanBlkId;
						/*release the usage of previous vlan block id*/
						gswdev->extendvlan_idx.vlan_idx[idx].IndexInUsageCnt--;
						gswdev->brdgeportconfig_idx[param->nBridgePortId].IngressExVlanBlkAssigned = 0;
						gswdev->brdgeportconfig_idx[param->nBridgePortId].IngressExVlanBlkId = EXVLAN_ENTRY_INVALID;
					}
				}
			}

			/*Note: First Index of the Block is the BlockID*/
			FirstIdx = param->nIngressExtendedVlanBlockId;

			if (FirstIdx >= gswdev->num_of_extendvlan) {
				ret = GSW_statusErr;
				goto UNLOCK_AND_RETURN;
			}

			/*Check whether this index belongs is InUse
			  Since it will be marked as InUse during allocation.
			*/
			if (!gswdev->extendvlan_idx.vlan_idx[FirstIdx].IndexInUse) {
				ret = GSW_statusErr;
				goto UNLOCK_AND_RETURN;
			}

			/*Check whether this index belongs to this  BlockID
			  Since it will be marked during allocation.
			*/
			if (gswdev->extendvlan_idx.vlan_idx[FirstIdx].VlanBlockId
			    != param->nIngressExtendedVlanBlockId) {
				ret = GSW_statusErr;
				goto UNLOCK_AND_RETURN;
			}

			/*Search for the Last Index of this block.
			  Note: The Blocks are always allocated contiguously.
			*/

			LastIdx = FirstIdx;

			while (gswdev->extendvlan_idx.vlan_idx[LastIdx].VlanBlockId == FirstIdx
			       && gswdev->extendvlan_idx.vlan_idx[LastIdx].IndexInUse) {
				LastIdx++;
			}

			BlkSize = param->nIngressExtendedVlanBlockSize;

			if (BlkSize && ((FirstIdx + BlkSize) <= LastIdx))
				LastIdx = (FirstIdx + BlkSize);

			/*Enable Extended VLAN operation*/
			tbl_prog_brdgeport_ingress.val[1] |= (1 << 14);
			/*Set First Index of the Block*/
			tbl_prog_brdgeport_ingress.val[1] &= ~(0x3FF);
			tbl_prog_brdgeport_ingress.val[1] |= (FirstIdx & 0x3FF);
			/*Set Last Index of the Block*/
			tbl_prog_brdgeport_ingress.val[2] &= ~(0x3FF);
			tbl_prog_brdgeport_ingress.val[2] |= ((LastIdx - 1) & 0x3FF);
			pr_debug("Bridge port ExVlan FirstIdx = %d\n", FirstIdx);
			pr_debug("Bridge port ExVlan LastIdx  = %d\n", LastIdx - 1);

			if (!(param->eMask & GSW_BRIDGE_PORT_CONFIG_MASK_FORCE)) {
				/*Usage count will be incremented only once during vlan blk assignment
				for this bridge port id*/
				if (!gswdev->brdgeportconfig_idx[param->nBridgePortId].IngressExVlanBlkAssigned) {
					gswdev->brdgeportconfig_idx[param->nBridgePortId].IngressExVlanBlkAssigned = 1;
					gswdev->brdgeportconfig_idx[param->nBridgePortId].IngressExVlanBlkId
						= param->nIngressExtendedVlanBlockId;
					/*Since this vlan blk can be shared,Increment it's usage count*/
					gswdev->extendvlan_idx.vlan_idx[FirstIdx].IndexInUsageCnt++;
				}
			}
		} else {
			/*Disable ingress Extended VLAN*/
			tbl_prog_brdgeport_ingress.val[1] &= ~(1 << 14);

			if (gswdev->brdgeportconfig_idx[param->nBridgePortId].IngressExVlanBlkAssigned) {
				idx = gswdev->brdgeportconfig_idx[param->nBridgePortId].IngressExVlanBlkId;
				/*release the usage of vlan block id, if it is assigned previously
				 when Extended vlan operation is disabled,it is must to release
				 the mapped exvlan resource*/
				gswdev->extendvlan_idx.vlan_idx[idx].IndexInUsageCnt--;
				gswdev->brdgeportconfig_idx[param->nBridgePortId].IngressExVlanBlkAssigned = 0;
				gswdev->brdgeportconfig_idx[param->nBridgePortId].IngressExVlanBlkId = EXVLAN_ENTRY_INVALID;
			}
		}
	}

	if (param->eMask & GSW_BRIDGE_PORT_CONFIG_MASK_EGRESS_VLAN) {
		if (param->bEgressExtendedVlanEnable) {
			if (!(param->eMask & GSW_BRIDGE_PORT_CONFIG_MASK_FORCE)) {
				/*This below field enable indicates that this vlan block is
				 already assigned and it's Blk id recorded for this Bridge Port id.
				 If next time the user update this Vlan blk field with different blk id !!
				 or wanted to allocate a new vlan blk ??!! may happen by mistake ??!!
				 the previous blk id must be released from this Bridge Port*/
				if (gswdev->brdgeportconfig_idx[param->nBridgePortId].EgressExVlanBlkAssigned) {
					if (gswdev->brdgeportconfig_idx[param->nBridgePortId].EgressExVlanBlkId !=
					    param->nEgressExtendedVlanBlockId) {
						idx = gswdev->brdgeportconfig_idx[param->nBridgePortId].EgressExVlanBlkId;
						/*release the usage of previous vlan block id*/
						gswdev->extendvlan_idx.vlan_idx[idx].IndexInUsageCnt--;
						gswdev->brdgeportconfig_idx[param->nBridgePortId].EgressExVlanBlkAssigned = 0;
						gswdev->brdgeportconfig_idx[param->nBridgePortId].EgressExVlanBlkId = EXVLAN_ENTRY_INVALID;
					}
				}
			}

			/*Note: First Index of the Block is the BlockID*/
			FirstIdx = param->nEgressExtendedVlanBlockId;

			if (FirstIdx >= gswdev->num_of_extendvlan) {
				ret = GSW_statusErr;
				goto UNLOCK_AND_RETURN;
			}

			/*Check whether this index belongs is InUse
			  Since it will be marked as InUse during allocation.
			*/
			if (!gswdev->extendvlan_idx.vlan_idx[FirstIdx].IndexInUse) {
				ret = GSW_statusErr;
				goto UNLOCK_AND_RETURN;
			}

			/*Check whether this index belongs to this  BlockID
			  Since it will be marked during allocation.
			*/
			if (gswdev->extendvlan_idx.vlan_idx[FirstIdx].VlanBlockId
			    != param->nEgressExtendedVlanBlockId) {
				ret = GSW_statusErr;
				goto UNLOCK_AND_RETURN;
			}

			/*Search for the Last Index of this block.
			  Note: The Blocks are always allocated contiguously.
			*/

			LastIdx = FirstIdx;

			while (gswdev->extendvlan_idx.vlan_idx[LastIdx].VlanBlockId == FirstIdx
			       && gswdev->extendvlan_idx.vlan_idx[LastIdx].IndexInUse) {
				LastIdx++;
			}

			BlkSize = param->nEgressExtendedVlanBlockSize;

			if (BlkSize && ((FirstIdx + BlkSize) <= LastIdx))
				LastIdx = (FirstIdx + BlkSize);

			/*Enable Extended VLAN operation*/
			tbl_prog_brdgeport_egress.val[1] |= (1 << 14);
			/*Set First Index of the Block*/
			tbl_prog_brdgeport_egress.val[1] &= ~(0x3FF);
			tbl_prog_brdgeport_egress.val[1] |= (FirstIdx & 0x3FF);
			/*Set Last Index of the Block*/
			tbl_prog_brdgeport_egress.val[2] &= ~(0x3FF);
			tbl_prog_brdgeport_egress.val[2] |= ((LastIdx - 1) & 0x3FF);

			if (!(param->eMask & GSW_BRIDGE_PORT_CONFIG_MASK_FORCE)) {
				/*Usage count will be incremented only once during vlan blk assignment
				for this bridge port id*/
				if (!gswdev->brdgeportconfig_idx[param->nBridgePortId].EgressExVlanBlkAssigned) {
					gswdev->brdgeportconfig_idx[param->nBridgePortId].EgressExVlanBlkAssigned = 1;
					gswdev->brdgeportconfig_idx[param->nBridgePortId].EgressExVlanBlkId
						= param->nEgressExtendedVlanBlockId;
					/*Since this vlan blk can be shared,Increment it's usage count*/
					gswdev->extendvlan_idx.vlan_idx[FirstIdx].IndexInUsageCnt++;
				}
			}
		} else {
			/*Disable Egress Extended VLAN*/
			tbl_prog_brdgeport_egress.val[1] &= ~(1 << 14);

			if (gswdev->brdgeportconfig_idx[param->nBridgePortId].EgressExVlanBlkAssigned) {
				idx = gswdev->brdgeportconfig_idx[param->nBridgePortId].EgressExVlanBlkId;
				/*release the usage of vlan block id, if it is assigned previously
				 when Extended vlan operation is disabled,it is must to release
				 the mapped exvlan resource*/
				gswdev->extendvlan_idx.vlan_idx[idx].IndexInUsageCnt--;
				gswdev->brdgeportconfig_idx[param->nBridgePortId].EgressExVlanBlkAssigned = 0;
				gswdev->brdgeportconfig_idx[param->nBridgePortId].EgressExVlanBlkId = EXVLAN_ENTRY_INVALID;
			}
		}
	}

	if (param->eMask & GSW_BRIDGE_PORT_CONFIG_MASK_INGRESS_MARKING) {
		tbl_prog_brdgeport_ingress.val[3] &= ~(7);

		switch (param->eIngressMarkingMode) {
		case GSW_MARKING_ALL_GREEN:
			break;

		case GSW_MARKING_INTERNAL_MARKING:
			tbl_prog_brdgeport_ingress.val[3] |= 0x1;
			break;

		case GSW_MARKING_DEI:
			tbl_prog_brdgeport_ingress.val[3] |= 0x2;
			break;

		case GSW_MARKING_PCP_8P0D:
			tbl_prog_brdgeport_ingress.val[3] |= 0x3;
			break;

		case GSW_MARKING_PCP_7P1D:
			tbl_prog_brdgeport_ingress.val[3] |= 0x4;
			break;

		case GSW_MARKING_PCP_6P2D:
			tbl_prog_brdgeport_ingress.val[3] |= 0x5;
			break;

		case GSW_MARKING_PCP_5P3D:
			tbl_prog_brdgeport_ingress.val[3] |= 0x6;
			break;

		case GSW_MARKING_DSCP_AF:
			tbl_prog_brdgeport_ingress.val[3] |= 0x7;
			break;
		}
	}

	if (param->eMask & GSW_BRIDGE_PORT_CONFIG_MASK_EGRESS_REMARKING) {
		tbl_prog_brdgeport_egress.val[3] &= ~(7);

		switch (param->eEgressRemarkingMode) {
		case GSW_REMARKING_NONE:
			break;

		case GSW_REMARKING_DEI:
			tbl_prog_brdgeport_egress.val[3] |= 0x2;
			break;

		case GSW_REMARKING_PCP_8P0D:
			tbl_prog_brdgeport_egress.val[3] |= 0x3;
			break;

		case GSW_REMARKING_PCP_7P1D:
			tbl_prog_brdgeport_egress.val[3] |= 0x4;
			break;

		case GSW_REMARKING_PCP_6P2D:
			tbl_prog_brdgeport_egress.val[3] |= 0x5;
			break;

		case GSW_REMARKING_PCP_5P3D:
			tbl_prog_brdgeport_egress.val[3] |= 0x6;
			break;

		case GSW_REMARKING_DSCP_AF:
			tbl_prog_brdgeport_egress.val[3] |= 0x7;
			break;
		}
	}

	if (param->eMask & GSW_BRIDGE_PORT_CONFIG_MASK_INGRESS_METER) {
		if (param->bIngressMeteringEnable) {
			if (param->nIngressTrafficMeterId >= gswdev->num_of_meters) {
				ret = GSW_statusErr;
				goto UNLOCK_AND_RETURN;
			}

			if (!(param->eMask & GSW_BRIDGE_PORT_CONFIG_MASK_FORCE)) {
				/*This below field enable indicates that this meter id is
				 already assigned and it's idx recorded for this Bridge Port id.
				 If next time the user update this meter field with different meter idx !!
				 or wanted to allocate a new meter idx ??!! may happen by mistake ??!!
				 the previous meter idx  must be released from this Bridge Port*/

				if (gswdev->brdgeportconfig_idx[param->nBridgePortId].IngressMeteringAssigned) {
					if (gswdev->brdgeportconfig_idx[param->nBridgePortId].IngressTrafficMeterId !=
					    param->nIngressTrafficMeterId) {
						idx = gswdev->brdgeportconfig_idx[param->nBridgePortId].IngressTrafficMeterId;
						/*release the usage of previous meter idx*/
						gswdev->meter_idx[idx].IndexInUsageCnt--;
						gswdev->brdgeportconfig_idx[param->nBridgePortId].IngressMeteringAssigned = 0;
						gswdev->brdgeportconfig_idx[param->nBridgePortId].IngressTrafficMeterId = 0;
					}
				}

				/*This Meter ID should be in use (i.e) that is this Meter ID should be allocated
				before calling bridge port configuration
				If not in use return ERROR
				*/
				if (gswdev->meter_idx[param->nIngressTrafficMeterId].IndexInUse) {
					/*Usage count will be incremented only once during meter idx assignment
					for this bridge port id*/
					if (!gswdev->brdgeportconfig_idx[param->nBridgePortId].IngressMeteringAssigned) {
						gswdev->brdgeportconfig_idx[param->nBridgePortId].IngressMeteringAssigned = 1;
						gswdev->brdgeportconfig_idx[param->nBridgePortId].IngressTrafficMeterId
							= param->nIngressTrafficMeterId;
						/*Since this meter id can be shared,Increment the it's usage count*/
						gswdev->meter_idx[param->nIngressTrafficMeterId].IndexInUsageCnt++;
					}
				} else {
					pr_err("gswdev->meter_idx[param->nIngressTrafficMeterId].IndexInUse\n");
					ret = GSW_statusErr;
					goto UNLOCK_AND_RETURN;
				}
			}

			/*Enable Meter Id*/
			tbl_prog_brdgeport_ingress.val[3] |= (1 << 7);
			tbl_prog_brdgeport_ingress.val[3] &= ~(0x7F << 8);
			tbl_prog_brdgeport_ingress.val[3] |= ((param->nIngressTrafficMeterId & 0x7F) << 8);
		} else {
			/*Disable Ingress Meter*/
			tbl_prog_brdgeport_ingress.val[3] &= ~(1 << 7);
		}
	}

	if (param->eMask & GSW_BRIDGE_PORT_CONFIG_MASK_EGRESS_SUB_METER) {
		/*GSW_BRIDGE_PORT_EGRESS_METER_BROADCAST*/
		if (param->bEgressSubMeteringEnable[0]) {
			meterid = param->nEgressTrafficSubMeterId[0];

			if (!(param->eMask & GSW_BRIDGE_PORT_CONFIG_MASK_FORCE)) {
				/*This below field enable indicates that this meter id is
				 already assigned and it's idx recorded for this Bridge Port id.
				 If next time the user update this meter field with different meter idx !!
				 or wanted to allocate a new meter idx ??!! may happen by mistake ??!!
				 the previous meter idx must be released from this Bridge Port*/

				if (gswdev->brdgeportconfig_idx[param->nBridgePortId].BroadcastMeteringAssigned) {
					if (gswdev->brdgeportconfig_idx[param->nBridgePortId].BroadcastMeteringId !=
					    meterid) {
						idx = gswdev->brdgeportconfig_idx[param->nBridgePortId].BroadcastMeteringId;
						/*release the usage of previous meter idx*/
						gswdev->meter_idx[idx].IndexInUsageCnt--;
						gswdev->brdgeportconfig_idx[param->nBridgePortId].BroadcastMeteringAssigned = 0;
						gswdev->brdgeportconfig_idx[param->nBridgePortId].BroadcastMeteringId = 0;
					}
				}

				/*This Meter ID should be in use (i.e) that is this Meter ID should be allocated
				before calling bridge port configuration
				If not in use return ERROR
				*/
				if (gswdev->meter_idx[meterid].IndexInUse) {
					/*Usage count will be incremented only once during meter idx assignment
					for this bridge port id*/
					if (!gswdev->brdgeportconfig_idx[param->nBridgePortId].BroadcastMeteringAssigned) {
						gswdev->brdgeportconfig_idx[param->nBridgePortId].BroadcastMeteringAssigned = 1;
						gswdev->brdgeportconfig_idx[param->nBridgePortId].BroadcastMeteringId
							= meterid;
						/*Since this meter id can be shared,Increment the it's usage count*/
						gswdev->meter_idx[meterid].IndexInUsageCnt++;
					}
				} else {
					pr_err("gswdev->meter_idx[meterid].IndexInUse\n");
					ret = GSW_statusErr;
					goto UNLOCK_AND_RETURN;
				}
			}

			tbl_prog_brdgeport_egress.val[5] |= (1 << 7);
			tbl_prog_brdgeport_egress.val[5] &= ~(0x7F << 8);
			tbl_prog_brdgeport_egress.val[5] |= ((param->nEgressTrafficSubMeterId[0] & 0x7F) << 8);
		} else {
			/*Disable Broadcast sub meter*/
			tbl_prog_brdgeport_egress.val[5] &= ~(1 << 7);
		}

		/*GSW_BRIDGE_PORT_EGRESS_METER_MULTICAST*/
		if (param->bEgressSubMeteringEnable[1]) {
			meterid = param->nEgressTrafficSubMeterId[1];

			if (!(param->eMask & GSW_BRIDGE_PORT_CONFIG_MASK_FORCE)) {
				/*This below field enable indicates that this meter id is
				 already assigned and it's idx recorded for this Bridge Port id.
				 If next time the user update this meter field with different meter idx !!
				 or wanted to allocate a new meter idx ??!! may happen by mistake ??!!
				 the previous meter idx must be released from this Bridge Port*/

				if (gswdev->brdgeportconfig_idx[param->nBridgePortId].MulticastMeteringAssigned) {
					if (gswdev->brdgeportconfig_idx[param->nBridgePortId].MulticastMeteringId !=
					    meterid) {
						idx = gswdev->brdgeportconfig_idx[param->nBridgePortId].MulticastMeteringId;
						/*release the usage of previous meter idx*/
						gswdev->meter_idx[idx].IndexInUsageCnt--;
						gswdev->brdgeportconfig_idx[param->nBridgePortId].MulticastMeteringAssigned = 0;
						gswdev->brdgeportconfig_idx[param->nBridgePortId].MulticastMeteringId = 0;
					}
				}

				/*This Meter ID should be in use (i.e) that is this Meter ID should be allocated
				before calling bridge port configuration
				If not in use return ERROR
				*/
				if (gswdev->meter_idx[meterid].IndexInUse) {
					/*Usage count will be incremented only once during meter idx assignment
					for this bridge port id*/
					if (!gswdev->brdgeportconfig_idx[param->nBridgePortId].MulticastMeteringAssigned) {
						gswdev->brdgeportconfig_idx[param->nBridgePortId].MulticastMeteringAssigned = 1;
						gswdev->brdgeportconfig_idx[param->nBridgePortId].MulticastMeteringId
							= meterid;
						/*Since this meter id can be shared,Increment the it's usage count*/
						gswdev->meter_idx[meterid].IndexInUsageCnt++;
					}
				} else {
					pr_err("gswdev->meter_idx[meterid].IndexInUse\n");
					ret = GSW_statusErr;
					goto UNLOCK_AND_RETURN;
				}
			}

			tbl_prog_brdgeport_egress.val[6] |= (1 << 7);
			tbl_prog_brdgeport_egress.val[6] &= ~(0x7F << 8);
			tbl_prog_brdgeport_egress.val[6] |= ((param->nEgressTrafficSubMeterId[1] & 0x7F) << 8);
		} else {
			/*Disable Multicast sub meter*/
			tbl_prog_brdgeport_egress.val[6] &= ~(1 << 7);
		}

		/*GSW_BRIDGE_PORT_EGRESS_METER_UNKNOWN_MC_IP*/
		if (param->bEgressSubMeteringEnable[2]) {
			meterid = param->nEgressTrafficSubMeterId[2];

			if (!(param->eMask & GSW_BRIDGE_PORT_CONFIG_MASK_FORCE)) {
				/*This below field enable indicates that this meter id is
				 already assigned and it's idx recorded for this Bridge Port id.
				 If next time the user update this meter field with different meter idx !!
				 or wanted to allocate a new meter idx ??!! may happen by mistake ??!!
				 the previous meter idx must be released from this Bridge Port*/

				if (gswdev->brdgeportconfig_idx[param->nBridgePortId].UnknownMultiIpMeteringAssigned) {
					if (gswdev->brdgeportconfig_idx[param->nBridgePortId].UnknownMultiIpMeteringId !=
					    meterid) {
						idx = gswdev->brdgeportconfig_idx[param->nBridgePortId].UnknownMultiIpMeteringId;
						/*release the usage of previous meter idx*/
						gswdev->meter_idx[idx].IndexInUsageCnt--;
						gswdev->brdgeportconfig_idx[param->nBridgePortId].UnknownMultiIpMeteringAssigned = 0;
						gswdev->brdgeportconfig_idx[param->nBridgePortId].UnknownMultiIpMeteringId = 0;
					}
				}

				/*This Meter ID should be in use (i.e) that is this Meter ID should be allocated
				before calling bridge port configuration
				If not in use return ERROR
				*/
				if (gswdev->meter_idx[meterid].IndexInUse) {
					/*Usage count will be incremented only once during meter idx assignment
					for this bridge port id*/
					if (!gswdev->brdgeportconfig_idx[param->nBridgePortId].UnknownMultiIpMeteringAssigned) {
						gswdev->brdgeportconfig_idx[param->nBridgePortId].UnknownMultiIpMeteringAssigned = 1;
						gswdev->brdgeportconfig_idx[param->nBridgePortId].UnknownMultiIpMeteringId
							= meterid;
						/*Since this meter id can be shared,Increment the it's usage count*/
						gswdev->meter_idx[meterid].IndexInUsageCnt++;
					}

				} else {
					pr_err("gswdev->meter_idx[meterid].IndexInUse\n");
					ret = GSW_statusErr;
					goto UNLOCK_AND_RETURN;
				}
			}

			tbl_prog_brdgeport_egress.val[9] |= (1 << 7);
			tbl_prog_brdgeport_egress.val[9] &= ~(0x7F << 8);
			tbl_prog_brdgeport_egress.val[9] |= ((param->nEgressTrafficSubMeterId[2] & 0x7F) << 8);
		} else {
			/*Disable Unknown Multicast ip sub meter*/
			tbl_prog_brdgeport_egress.val[9] &= ~(1 << 7);
		}

		/*GSW_BRIDGE_PORT_EGRESS_METER_UNKNOWN_MC_NON_IP*/
		if (param->bEgressSubMeteringEnable[3]) {
			meterid = param->nEgressTrafficSubMeterId[3];

			if (!(param->eMask & GSW_BRIDGE_PORT_CONFIG_MASK_FORCE)) {
				/*This below field enable indicates that this meter id is
				 already assigned and it's idx recorded for this Bridge Port id.
				 If next time the user update this meter field with different meter idx !!
				 or wanted to allocate a new meter idx ??!! may happen by mistake ??!!
				 the previous meter idx must be released from this Bridge Port*/

				if (gswdev->brdgeportconfig_idx[param->nBridgePortId].UnknownMultiNonIpMeteringAssigned) {
					if (gswdev->brdgeportconfig_idx[param->nBridgePortId].UnknownMultiNonIpMeteringId !=
					    meterid) {
						idx = gswdev->brdgeportconfig_idx[param->nBridgePortId].UnknownMultiNonIpMeteringId;
						/*release the usage of previous meter idx*/
						gswdev->meter_idx[idx].IndexInUsageCnt--;
						gswdev->brdgeportconfig_idx[param->nBridgePortId].UnknownMultiNonIpMeteringAssigned = 0;
						gswdev->brdgeportconfig_idx[param->nBridgePortId].UnknownMultiNonIpMeteringId = 0;
					}
				}

				/*This Meter ID should be in use (i.e) that is this Meter ID should be allocated
				before calling bridge port configuration
				If not in use return ERROR
				*/
				if (gswdev->meter_idx[meterid].IndexInUse) {
					/*Usage count will be incremented only once during meter idx assignment
					for this bridge port id*/
					if (!gswdev->brdgeportconfig_idx[param->nBridgePortId].UnknownMultiNonIpMeteringAssigned) {
						gswdev->brdgeportconfig_idx[param->nBridgePortId].UnknownMultiNonIpMeteringAssigned = 1;
						gswdev->brdgeportconfig_idx[param->nBridgePortId].UnknownMultiNonIpMeteringId
							= meterid;
						/*Since this meter id can be shared,Increment the it's usage count*/
						gswdev->meter_idx[meterid].IndexInUsageCnt++;
					}
				} else {
					pr_err("gswdev->meter_idx[meterid].IndexInUse\n");
					ret = GSW_statusErr;
					goto UNLOCK_AND_RETURN;
				}
			}

			tbl_prog_brdgeport_egress.val[8] |= (1 << 7);
			tbl_prog_brdgeport_egress.val[8] &= ~(0x7F << 8);
			tbl_prog_brdgeport_egress.val[8] |= ((param->nEgressTrafficSubMeterId[3] & 0x7F) << 8);
		} else {
			/*Disable Unknown Multicast Non-ip sub meter*/
			tbl_prog_brdgeport_egress.val[8] &= ~(1 << 7);
		}

		/*GSW_BRIDGE_PORT_EGRESS_METER_UNKNOWN_UC*/
		if (param->bEgressSubMeteringEnable[4]) {
			meterid = param->nEgressTrafficSubMeterId[4];

			if (!(param->eMask & GSW_BRIDGE_PORT_CONFIG_MASK_FORCE)) {
				/*This below field enable indicates that this meter id is
				 already assigned and it's idx recorded for this Bridge Port id.
				 If next time the user update this meter field with different meter idx !!
				 or wanted to allocate a new meter idx ??!! may happen by mistake ??!!
				 the previous meter idx must be released from this Bridge Port*/

				if (gswdev->brdgeportconfig_idx[param->nBridgePortId].UnknownUniCastMeteringAssigned) {
					if (gswdev->brdgeportconfig_idx[param->nBridgePortId].UnknownUniCastMeteringId !=
					    meterid) {
						idx = gswdev->brdgeportconfig_idx[param->nBridgePortId].UnknownUniCastMeteringId;
						/*release the usage of previous meter idx*/
						gswdev->meter_idx[idx].IndexInUsageCnt--;
						gswdev->brdgeportconfig_idx[param->nBridgePortId].UnknownUniCastMeteringAssigned = 0;
						gswdev->brdgeportconfig_idx[param->nBridgePortId].UnknownUniCastMeteringId = 0;
					}
				}

				/*This Meter ID should be in use (i.e) that is this Meter ID should be allocated
				before calling bridge port configuration
				If not in use return ERROR
				*/
				if (gswdev->meter_idx[meterid].IndexInUse) {
					/*Usage count will be incremented only once during meter idx assignment
					for this bridge port id*/
					if (!gswdev->brdgeportconfig_idx[param->nBridgePortId].UnknownUniCastMeteringAssigned) {
						gswdev->brdgeportconfig_idx[param->nBridgePortId].UnknownUniCastMeteringAssigned = 1;
						gswdev->brdgeportconfig_idx[param->nBridgePortId].UnknownUniCastMeteringId
							= meterid;
						/*Since this meter id can be shared,Increment the it's usage count*/
						gswdev->meter_idx[meterid].IndexInUsageCnt++;
					}
				} else {
					pr_err("gswdev->meter_idx[meterid].IndexInUse\n");
					ret = GSW_statusErr;
					goto UNLOCK_AND_RETURN;
				}
			}

			tbl_prog_brdgeport_egress.val[7] |= (1 << 7);
			tbl_prog_brdgeport_egress.val[7] &= ~(0x7F << 8);
			tbl_prog_brdgeport_egress.val[7] |= ((param->nEgressTrafficSubMeterId[4] & 0x7F) << 8);
		} else {
			/*Disable Unknown Unicast sub meter*/
			tbl_prog_brdgeport_egress.val[7] &= ~(1 << 7);
		}

		/*GSW_BRIDGE_PORT_EGRESS_METER_OTHERS*/
		if (param->bEgressSubMeteringEnable[5]) {
			meterid = param->nEgressTrafficSubMeterId[5];

			if (!(param->eMask & GSW_BRIDGE_PORT_CONFIG_MASK_FORCE)) {
				/*This below field enable indicates that this meter id is
				 already assigned and it's idx recorded for this Bridge Port id.
				 If next time the user update this meter field with different meter idx !!
				 or wanted to allocate a new meter idx ??!! may happen by mistake ??!!
				 the previous meter idx must be released from this Bridge Port*/

				if (gswdev->brdgeportconfig_idx[param->nBridgePortId].EgressMeteringAssigned) {
					if (gswdev->brdgeportconfig_idx[param->nBridgePortId].EgressTrafficMeterId !=
					    meterid) {
						idx = gswdev->brdgeportconfig_idx[param->nBridgePortId].EgressTrafficMeterId;
						/*release the usage of previous meter idx*/
						gswdev->meter_idx[idx].IndexInUsageCnt--;
						gswdev->brdgeportconfig_idx[param->nBridgePortId].EgressMeteringAssigned = 0;
						gswdev->brdgeportconfig_idx[param->nBridgePortId].EgressTrafficMeterId = 0;
					}
				}

				/*This Meter ID should be in use (i.e) that is this Meter ID should be allocated
				before calling bridge port configuration
				If not in use return ERROR
				*/
				if (gswdev->meter_idx[meterid].IndexInUse) {
					/*Usage count will be incremented only once during meter idx assignment
					for this bridge port id*/
					if (!gswdev->brdgeportconfig_idx[param->nBridgePortId].EgressMeteringAssigned) {
						gswdev->brdgeportconfig_idx[param->nBridgePortId].EgressMeteringAssigned = 1;
						gswdev->brdgeportconfig_idx[param->nBridgePortId].EgressTrafficMeterId
							= meterid;
						/*Since this meter id can be shared,Increment the it's usage count*/
						gswdev->meter_idx[meterid].IndexInUsageCnt++;
					}

				} else {
					pr_err("Meter Id not In Use\n");
					ret = GSW_statusErr;
					goto UNLOCK_AND_RETURN;
				}

			}

			/*Enable Meter Id*/
			tbl_prog_brdgeport_egress.val[3] |= (1 << 7);
			tbl_prog_brdgeport_egress.val[3] &= ~(0x7F << 8);
			tbl_prog_brdgeport_egress.val[3] |= ((param->nEgressTrafficSubMeterId[5] & 0x7F) << 8);
		} else {
			/*Disable Egress Meter*/
			tbl_prog_brdgeport_egress.val[3] &= ~(1 << 7);
		}
	}

	if (param->eMask & GSW_BRIDGE_PORT_CONFIG_MASK_EGRESS_CTP_MAPPING) {
		/** This field defines destination logical port. */
		tbl_prog_brdgeport_egress.val[4] &= ~(0xF << 8);
		tbl_prog_brdgeport_egress.val[4] |= ((param->nDestLogicalPortId & 0xF) << 8);

		if (param->bPmapperEnable) {
			/*This below field indicates that one P-mapper idx is already allocated and
			 it's idx recorded for this Bridge Port id.
			 If next time the user update this p-mapper field with different p-mapper idx !!
			 or wanted to allocate a new p-mapper idx ??!! may happen by mistake ??!!
			 the previous p-mapper idx should be released*/

			/*TODO:changed as per code review
			 As per code review - user is not responsible pmapper allocation*/
			if (gswdev->brdgeportconfig_idx[param->nBridgePortId].PmapperAssigned)
				param->sPmapper.nPmapperId = gswdev->brdgeportconfig_idx[param->nBridgePortId].PmappperIdx;
			else
				param->sPmapper.nPmapperId = PMAPPER_ENTRY_INVALID;

			/** When bPmapperEnable is TRUE, this field selects either DSCP or PCP to
			 derive sub interface ID. */
			/*PCP-P-mapper table entry 1-8*/
			/*DSCP-P-mapper table entry 9-72. */
			switch (param->ePmapperMappingMode) {
			case GSW_PMAPPER_MAPPING_PCP:
				tbl_prog_brdgeport_egress.val[4] &= ~(1 << 13);
				break;

			case GSW_PMAPPER_MAPPING_DSCP:
				tbl_prog_brdgeport_egress.val[4] |= (1 << 13);
				break;
			}

			/* If the p-mapper id is invalid then the GSW_QOS_PmapperTableSet will
			   Allocate/program P-mapper index.
			   If p-mapper index is valid the GSW_QOS_PmapperTableSet will just program
			   the table
			 */
			ret = GSW_QOS_PmapperTableSet(cdev, &param->sPmapper);

			if (ret == GSW_statusErr) {
				pr_err("GSW_QOS_PmapperTableSet ERROR\n");
				goto UNLOCK_AND_RETURN;
			}

			pmapper_idx = param->sPmapper.nPmapperId;

			/*Usage count will be incremented only once during p-mapper idx allocation
			 or assignment for this bridge port id*/
			if (!gswdev->brdgeportconfig_idx[param->nBridgePortId].PmapperAssigned) {
				gswdev->brdgeportconfig_idx[param->nBridgePortId].PmapperAssigned = 1;
				gswdev->brdgeportconfig_idx[param->nBridgePortId].PmappperIdx = pmapper_idx;
			}

			tbl_prog_brdgeport_egress.val[4] |= (1 << 14);
			tbl_prog_brdgeport_egress.val[4] &= ~(0xFF);
			tbl_prog_brdgeport_egress.val[4] |= (pmapper_idx & 0xFF);
		} else {
			/** When bPmapperEnable is FALSE, this field defines destination sub
				interface ID group. */
			/*Disable p-mapper*/
			tbl_prog_brdgeport_egress.val[4] &= ~(1 << 14);
			tbl_prog_brdgeport_egress.val[4] &= ~(0xFF);
			tbl_prog_brdgeport_egress.val[4] |= (param->nDestSubIfIdGroup & 0xFF);
		}
	}

	if (param->eMask & GSW_BRIDGE_PORT_CONFIG_MASK_BRIDGE_PORT_MAP) {
		val_reg_idx = 10;

		for (i = 0; i < 8; i++) {
			tbl_prog_brdgeport_ingress.val[val_reg_idx] = (param->nBridgePortMap[i] & 0xFFFF);
			val_reg_idx++;
		}
	}

	if (param->eMask & GSW_BRIDGE_PORT_CONFIG_MASK_MC_DEST_IP_LOOKUP) {
		/* 0- disable , 1 - enable*/
		if (param->bMcDestIpLookupDisable)
			tbl_prog_brdgeport_ingress.val[0] &= ~(1 << 13);
		else
			tbl_prog_brdgeport_ingress.val[0] |= (1 << 13);
	}

#ifdef GSW_BRIDGE_PORT_SRC_IP_LOOKUP_DISABLE
	tbl_prog_brdgeport_ingress.val[0] &= ~(1 << 9);
#else
	if (param->eMask & GSW_BRIDGE_PORT_CONFIG_MASK_MC_SRC_IP_LOOKUP) {
		/* 0- disable , 1 - enable*/
		if (param->bMcSrcIpLookupEnable)
			tbl_prog_brdgeport_ingress.val[0] |= (1 << 9);
		else
			tbl_prog_brdgeport_ingress.val[0] &= ~(1 << 9);
	}
#endif

	if (param->eMask & GSW_BRIDGE_PORT_CONFIG_MASK_MC_DEST_MAC_LOOKUP) {
		/* 1- disable , 0 - enable*/
		if (param->bDestMacLookupDisable)
			tbl_prog_brdgeport_ingress.val[0] |= (1 << 14);
		else
			tbl_prog_brdgeport_ingress.val[0] &= ~(1 << 14);
	}

	if (param->eMask & GSW_BRIDGE_PORT_CONFIG_MASK_MC_SRC_MAC_LEARNING) {
		/* 1- disable , 0 - enable*/
		if (param->bSrcMacLearningDisable)
			tbl_prog_brdgeport_ingress.val[0] |= (1 << 15);
		else
			tbl_prog_brdgeport_ingress.val[0] &= ~(1 << 15);
	}

	if (param->eMask & GSW_BRIDGE_PORT_CONFIG_MASK_MAC_SPOOFING) {
		/* 0- disable , 1 - enable*/
		if (param->bMacSpoofingDetectEnable)
			tbl_prog_brdgeport_ingress.val[0] |= (1 << 11);
		else
			tbl_prog_brdgeport_ingress.val[0] &= ~(1 << 11);
	}

	if (param->eMask & GSW_BRIDGE_PORT_CONFIG_MASK_PORT_LOCK) {
		/* 0- disable , 1 - enable*/
		if (param->bPortLockEnable)
			tbl_prog_brdgeport_ingress.val[0] |= (1 << 12);
		else
			tbl_prog_brdgeport_ingress.val[0] &= ~(1 << 12);
	}

	if (param->eMask & GSW_BRIDGE_PORT_CONFIG_MASK_INGRESS_VLAN_FILTER) {
		/*BypassEgress vlan filter range 1*/
		if (param->bBypassEgressVlanFilter1)
			tbl_prog_brdgeport_ingress.val[5] |= (1 << 15);
		else
			tbl_prog_brdgeport_ingress.val[5] &= ~(1 << 15);

		if (param->bIngressVlanFilterEnable) {
			if (!(param->eMask & GSW_BRIDGE_PORT_CONFIG_MASK_FORCE)) {
				/*This below field enable indicates that this vlan block is
				 already assigned and it's Blk id recorded for this Bridge Port id.
				 If next time the user update this Vlan blk field with different blk id !!
				 or wanted to allocate a new vlan blk ??!! may happen by mistake ??!!
				 the previous blk id must be released from this Bridge Port*/

				if (gswdev->brdgeportconfig_idx[param->nBridgePortId].IngressVlanFilterAssigned) {
					if (gswdev->brdgeportconfig_idx[param->nBridgePortId].IngressVlanFilterBlkId !=
					    param->nIngressVlanFilterBlockId) {
						idx = gswdev->brdgeportconfig_idx[param->nBridgePortId].IngressVlanFilterBlkId;
						/*release the usage of previous vlan block id*/
						gswdev->vlanfilter_idx.filter_idx[idx].IndexInUsageCnt--;
						gswdev->brdgeportconfig_idx[param->nBridgePortId].IngressVlanFilterAssigned = 0;
						gswdev->brdgeportconfig_idx[param->nBridgePortId].IngressVlanFilterBlkId = VLANFILTER_ENTRY_INVALID;
					}
				}
			}

			/*Note: First Index of the Block is the BlockID*/
			FirstIdx = param->nIngressVlanFilterBlockId;

			if (FirstIdx > gswdev->num_of_vlanfilter) {
				pr_err("FirstIdx > gswdev->num_of_vlanfilter\n");
				ret = GSW_statusErr;
				goto UNLOCK_AND_RETURN;
			}

			/*Check whether this index belongs is InUse
			  Since it will be marked as InUse during allocation.
			*/
			if (!gswdev->vlanfilter_idx.filter_idx[FirstIdx].IndexInUse) {
				pr_err("!gswdev->vlanfilter_idx.filter_idx[FirstIdx].IndexInUse\n");
				ret = GSW_statusErr;
				goto UNLOCK_AND_RETURN;
			}

			/*Check whether this index belongs to this  BlockID
			  Since it will be marked during allocation.
			*/
			if (gswdev->vlanfilter_idx.filter_idx[FirstIdx].FilterBlockId
			    != param->nIngressVlanFilterBlockId) {
				ret = GSW_statusErr;
				goto UNLOCK_AND_RETURN;
			}

			/*Search for the Last Index of this block.
			  Note: The Blocks are always allocated contiguously.
			*/

			LastIdx = FirstIdx;

			while (gswdev->vlanfilter_idx.filter_idx[LastIdx].FilterBlockId == FirstIdx
			       && gswdev->vlanfilter_idx.filter_idx[LastIdx].IndexInUse) {
				LastIdx++;
			}

			BlkSize = param->nIngressVlanFilterBlockSize;

			if (BlkSize && ((FirstIdx + BlkSize) <= LastIdx))
				LastIdx = (FirstIdx + BlkSize);

			/* enable ingress VLAN filtering*/
			tbl_prog_brdgeport_ingress.val[5] |= (1 << 14);
			/*Set First Index of the Block*/
			tbl_prog_brdgeport_ingress.val[5] &= ~(0x3FF);
			tbl_prog_brdgeport_ingress.val[5] |= (FirstIdx & 0x3FF);
			/*Set Last Index of the Block*/
			tbl_prog_brdgeport_ingress.val[6] &= ~(0x3FF);
			tbl_prog_brdgeport_ingress.val[6] |= ((LastIdx - 1) & 0x3FF);

			if (gswdev->vlanfilter_idx.filter_idx[FirstIdx].DiscardUntagged)
				tbl_prog_brdgeport_ingress.val[5] |= (1 << 12);
			else
				tbl_prog_brdgeport_ingress.val[5] &= ~(1 << 12);

			if (gswdev->vlanfilter_idx.filter_idx[FirstIdx].DiscardUnMatchedTagged)
				tbl_prog_brdgeport_ingress.val[5] |= (1 << 13);
			else
				tbl_prog_brdgeport_ingress.val[5] &= ~(1 << 13);


			/* mask mode
				00 :VID Only
				01 :RESERVED
				10 :PCP
				11 :TCI
			*/

			tbl_prog_brdgeport_ingress.val[6] &= ~(0x3000);

			switch (gswdev->vlanfilter_idx.filter_idx[FirstIdx].FilterMask) {
			case GSW_VLAN_FILTER_TCI_MASK_VID:
				break;

			case GSW_VLAN_FILTER_TCI_MASK_PCP:
				tbl_prog_brdgeport_ingress.val[6] |= (2 << 12);
				break;

			case GSW_VLAN_FILTER_TCI_MASK_TCI:
				tbl_prog_brdgeport_ingress.val[6] |= (3 << 12);
				break;
			}

			if (!(param->eMask & GSW_BRIDGE_PORT_CONFIG_MASK_FORCE)) {
				/*Usage count will be incremented only once during vlan blk assignment
				for this bridge port id*/
				if (!gswdev->brdgeportconfig_idx[param->nBridgePortId].IngressVlanFilterAssigned) {
					gswdev->brdgeportconfig_idx[param->nBridgePortId].IngressVlanFilterAssigned = 1;
					gswdev->brdgeportconfig_idx[param->nBridgePortId].IngressVlanFilterBlkId
						= param->nIngressVlanFilterBlockId;
					/*Since this vlan blk can be shared,Increment it's usage count*/
					gswdev->vlanfilter_idx.filter_idx[FirstIdx].IndexInUsageCnt++;;
				}
			}
		} else {
			/* disable ingress VLAN filtering*/
			tbl_prog_brdgeport_ingress.val[5] &= ~(1 << 14);

			if (gswdev->brdgeportconfig_idx[param->nBridgePortId].IngressVlanFilterAssigned) {
				idx = gswdev->brdgeportconfig_idx[param->nBridgePortId].IngressVlanFilterBlkId;
				/*release the usage of vlan block id, if it is assigned previously
				 when vlan filter operation is disabled,it is must to release
				 the mapped vlan filter resource*/
				gswdev->vlanfilter_idx.filter_idx[idx].IndexInUsageCnt--;
				gswdev->brdgeportconfig_idx[param->nBridgePortId].IngressVlanFilterAssigned = 0;
				gswdev->brdgeportconfig_idx[param->nBridgePortId].IngressVlanFilterBlkId = VLANFILTER_ENTRY_INVALID;
			}
		}
	}

	if (param->eMask & GSW_BRIDGE_PORT_CONFIG_MASK_EGRESS_VLAN_FILTER1) {
		if (param->bEgressVlanFilter1Enable) {
			if (!(param->eMask & GSW_BRIDGE_PORT_CONFIG_MASK_FORCE)) {
				/*This below field enable indicates that this vlan block is
				 already assigned and it's Blk id recorded for this Bridge Port id.
				 If next time the user update this Vlan blk field with different blk id !!
				 or wanted to allocate a new vlan blk ??!! may happen by mistake ??!!
				 the previous blk id must be released from this Bridge Port*/

				if (gswdev->brdgeportconfig_idx[param->nBridgePortId].EgressVlanFilter1Assigned) {
					if (gswdev->brdgeportconfig_idx[param->nBridgePortId].EgressVlanFilter1BlkId !=
					    param->nEgressVlanFilter1BlockId) {
						idx = gswdev->brdgeportconfig_idx[param->nBridgePortId].EgressVlanFilter1BlkId;
						/*release the usage of previous vlan block id*/
						gswdev->vlanfilter_idx.filter_idx[idx].IndexInUsageCnt--;
						gswdev->brdgeportconfig_idx[param->nBridgePortId].EgressVlanFilter1Assigned = 0;
						gswdev->brdgeportconfig_idx[param->nBridgePortId].EgressVlanFilter1BlkId = VLANFILTER_ENTRY_INVALID;
					}
				}
			}

			/*Note: First Index of the Block is the BlockID*/
			FirstIdx = param->nEgressVlanFilter1BlockId;

			if (FirstIdx > gswdev->num_of_vlanfilter) {
				ret = GSW_statusErr;
				goto UNLOCK_AND_RETURN;
			}

			/*Check whether this index belongs is InUse
			  Since it will be marked as InUse during allocation.
			*/
			if (!gswdev->vlanfilter_idx.filter_idx[FirstIdx].IndexInUse) {
				ret = GSW_statusErr;
				goto UNLOCK_AND_RETURN;
			}

			/*Check whether this index belongs to this  BlockID
			  Since it will be marked during allocation.
			*/
			if (gswdev->vlanfilter_idx.filter_idx[FirstIdx].FilterBlockId
			    != param->nEgressVlanFilter1BlockId) {
				ret = GSW_statusErr;
				goto UNLOCK_AND_RETURN;
			}

			/*Search for the Last Index of this block.
			  Note: The Blocks are always allocated contiguously.
			*/

			LastIdx = FirstIdx;

			while (gswdev->vlanfilter_idx.filter_idx[LastIdx].FilterBlockId == FirstIdx
			       && gswdev->vlanfilter_idx.filter_idx[LastIdx].IndexInUse) {
				LastIdx++;
			}

			BlkSize = param->nEgressVlanFilter1BlockSize;

			if (BlkSize && ((FirstIdx + BlkSize) <= LastIdx))
				LastIdx = (FirstIdx + BlkSize);

			/* enable egress VLAN filtering -Range 1*/
			tbl_prog_brdgeport_egress.val[10] |= (1 << 14);
			/*Set First Index of the Block*/
			tbl_prog_brdgeport_egress.val[10] &= ~(0x3FF);
			tbl_prog_brdgeport_egress.val[10] |= (FirstIdx & 0x3FF);
			/*Set Last Index of the Block*/
			tbl_prog_brdgeport_egress.val[11] &= ~(0x3FF);
			tbl_prog_brdgeport_egress.val[11] |= ((LastIdx - 1) & 0x3FF);



			if (gswdev->vlanfilter_idx.filter_idx[FirstIdx].DiscardUntagged)
				tbl_prog_brdgeport_egress.val[10] |= (1 << 12);
			else
				tbl_prog_brdgeport_egress.val[10] &= ~(1 << 12);

			if (gswdev->vlanfilter_idx.filter_idx[FirstIdx].DiscardUnMatchedTagged)
				tbl_prog_brdgeport_egress.val[10] |= (1 << 13);
			else
				tbl_prog_brdgeport_egress.val[10] &= ~(1 << 13);

			/* mask mode
				00 :VID Only
				01 :RESERVED
				10 :PCP
				11 :TCI
			*/
			tbl_prog_brdgeport_egress.val[11] &= ~(0x3000);

			switch (gswdev->vlanfilter_idx.filter_idx[FirstIdx].FilterMask) {
			case GSW_VLAN_FILTER_TCI_MASK_VID:
				break;

			case GSW_VLAN_FILTER_TCI_MASK_PCP:
				tbl_prog_brdgeport_egress.val[11] |= (2 << 12);
				break;

			case GSW_VLAN_FILTER_TCI_MASK_TCI:
				tbl_prog_brdgeport_egress.val[11] |= (3 << 12);
				break;
			}

			if (!(param->eMask & GSW_BRIDGE_PORT_CONFIG_MASK_FORCE)) {
				/*Usage count will be incremented only once during vlan blk assignment
				for this bridge port id*/
				if (!gswdev->brdgeportconfig_idx[param->nBridgePortId].EgressVlanFilter1Assigned) {
					gswdev->brdgeportconfig_idx[param->nBridgePortId].EgressVlanFilter1Assigned = 1;
					gswdev->brdgeportconfig_idx[param->nBridgePortId].EgressVlanFilter1BlkId
						= param->nEgressVlanFilter1BlockId;
					/*Since this vlan blk can be shared,Increment it's usage count*/
					gswdev->vlanfilter_idx.filter_idx[FirstIdx].IndexInUsageCnt++;;
				}
			}
		} else {
			/* disable Egress VLAN filtering - Range 1*/
			tbl_prog_brdgeport_egress.val[10] &= ~(1 << 14);

			if (gswdev->brdgeportconfig_idx[param->nBridgePortId].EgressVlanFilter1Assigned) {
				idx = gswdev->brdgeportconfig_idx[param->nBridgePortId].EgressVlanFilter1BlkId;
				/*release the usage of vlan block id, if it is assigned previously
				 when vlan filter operation is disabled,it is must to release
				 the mapped vlan filter resource*/
				gswdev->vlanfilter_idx.filter_idx[idx].IndexInUsageCnt--;
				gswdev->brdgeportconfig_idx[param->nBridgePortId].EgressVlanFilter1Assigned = 0;
				gswdev->brdgeportconfig_idx[param->nBridgePortId].EgressVlanFilter1BlkId = VLANFILTER_ENTRY_INVALID;
			}
		}
	}

	if (param->eMask & GSW_BRIDGE_PORT_CONFIG_MASK_EGRESS_VLAN_FILTER2) {
		if (param->bEgressVlanFilter2Enable) {
			if (!(param->eMask & GSW_BRIDGE_PORT_CONFIG_MASK_FORCE)) {
				/*This below field enable indicates that this vlan block is
				 already assigned and it's Blk id recorded for this Bridge Port id.
				 If next time the user update this Vlan blk field with different blk id !!
				 or wanted to allocate a new vlan blk ??!! may happen by mistake ??!!
				 the previous blk id must be released from this Bridge Port*/

				if (gswdev->brdgeportconfig_idx[param->nBridgePortId].EgressVlanFilter2Assigned) {
					if (gswdev->brdgeportconfig_idx[param->nBridgePortId].EgressVlanFilter2BlkId !=
					    param->nEgressVlanFilter2BlockId) {
						idx = gswdev->brdgeportconfig_idx[param->nBridgePortId].EgressVlanFilter2BlkId;
						/*release the usage of previous vlan block id*/
						gswdev->vlanfilter_idx.filter_idx[idx].IndexInUsageCnt--;
						gswdev->brdgeportconfig_idx[param->nBridgePortId].EgressVlanFilter2Assigned = 0;
						gswdev->brdgeportconfig_idx[param->nBridgePortId].EgressVlanFilter2BlkId = VLANFILTER_ENTRY_INVALID;
					}
				}
			}

			/*Note: First Index of the Block is the BlockID*/
			FirstIdx = param->nEgressVlanFilter2BlockId;

			if (FirstIdx > gswdev->num_of_vlanfilter) {
				ret = GSW_statusErr;
				goto UNLOCK_AND_RETURN;
			}

			/*Check whether this index belongs is InUse
			  Since it will be marked as InUse during allocation.
			*/
			if (!gswdev->vlanfilter_idx.filter_idx[FirstIdx].IndexInUse) {
				ret = GSW_statusErr;
				goto UNLOCK_AND_RETURN;
			}

			/*Check whether this index belongs to this  BlockID
			  Since it will be marked during allocation.
			*/
			if (gswdev->vlanfilter_idx.filter_idx[FirstIdx].FilterBlockId
			    != param->nEgressVlanFilter2BlockId) {
				ret = GSW_statusErr;
				goto UNLOCK_AND_RETURN;
			}

			/*Search for the Last Index of this block.
			  Note: The Blocks are always allocated contiguously.
			*/

			LastIdx = FirstIdx;

			while (gswdev->vlanfilter_idx.filter_idx[LastIdx].FilterBlockId == FirstIdx
			       && gswdev->vlanfilter_idx.filter_idx[LastIdx].IndexInUse) {
				LastIdx++;
			}

			BlkSize = param->nEgressVlanFilter2BlockSize;

			if (BlkSize && ((FirstIdx + BlkSize) <= LastIdx))
				LastIdx = (FirstIdx + BlkSize);

			/* enable egress VLAN filtering -Range 2*/
			tbl_prog_brdgeport_egress.val[12] |= (1 << 14);
			/*Set First Index of the Block*/
			tbl_prog_brdgeport_egress.val[12] &= ~(0x3FF);
			tbl_prog_brdgeport_egress.val[12] |= (FirstIdx & 0x3FF);
			/*Set Last Index of the Block*/
			tbl_prog_brdgeport_egress.val[13] &= ~(0x3FF);
			tbl_prog_brdgeport_egress.val[13] |= ((LastIdx - 1) & 0x3FF);



			if (gswdev->vlanfilter_idx.filter_idx[FirstIdx].DiscardUntagged)
				tbl_prog_brdgeport_egress.val[12] |= (1 << 12);
			else
				tbl_prog_brdgeport_egress.val[12] &= ~(1 << 12);

			if (gswdev->vlanfilter_idx.filter_idx[FirstIdx].DiscardUnMatchedTagged)
				tbl_prog_brdgeport_egress.val[12] |= (1 << 13);
			else
				tbl_prog_brdgeport_egress.val[12] &= ~(1 << 13);

			/* mask mode
				00 :VID Only
				01 :RESERVED
				10 :PCP
				11 :TCI
			*/
			tbl_prog_brdgeport_egress.val[13] &= ~(0x3000);

			switch (gswdev->vlanfilter_idx.filter_idx[FirstIdx].FilterMask) {
			case GSW_VLAN_FILTER_TCI_MASK_VID:
				break;

			case GSW_VLAN_FILTER_TCI_MASK_PCP:
				tbl_prog_brdgeport_egress.val[13] |= (2 << 12);
				break;

			case GSW_VLAN_FILTER_TCI_MASK_TCI:
				tbl_prog_brdgeport_egress.val[13] |= (3 << 12);
				break;
			}

			if (!(param->eMask & GSW_BRIDGE_PORT_CONFIG_MASK_FORCE)) {
				/*Usage count will be incremented only once during vlan blk assignment
				for this bridge port id*/
				if (!gswdev->brdgeportconfig_idx[param->nBridgePortId].EgressVlanFilter2Assigned) {
					gswdev->brdgeportconfig_idx[param->nBridgePortId].EgressVlanFilter2Assigned = 1;
					gswdev->brdgeportconfig_idx[param->nBridgePortId].EgressVlanFilter2BlkId
						= param->nEgressVlanFilter2BlockId;
					/*Since this vlan blk can be shared,Increment it's usage count*/
					gswdev->vlanfilter_idx.filter_idx[FirstIdx].IndexInUsageCnt++;
				}
			}
		} else {
			/* disable Egress VLAN filtering - Range 2*/
			tbl_prog_brdgeport_egress.val[12] &= ~(1 << 14);

			if (gswdev->brdgeportconfig_idx[param->nBridgePortId].EgressVlanFilter2Assigned) {
				idx = gswdev->brdgeportconfig_idx[param->nBridgePortId].EgressVlanFilter2BlkId;
				/*release the usage of vlan block id, if it is assigned previously
				 when vlan filter operation is disabled,it is must to release
				 the mapped vlan filter resource*/
				gswdev->vlanfilter_idx.filter_idx[idx].IndexInUsageCnt--;
				gswdev->brdgeportconfig_idx[param->nBridgePortId].EgressVlanFilter2Assigned = 0;
				gswdev->brdgeportconfig_idx[param->nBridgePortId].EgressVlanFilter2BlkId = VLANFILTER_ENTRY_INVALID;
			}
		}
	}

	if (param->eMask & GSW_BRIDGE_PORT_CONFIG_MASK_MAC_LEARNING_LIMIT) {
		if (param->bMacLearningLimitEnable) {
			tbl_prog_brdgeport_ingress.val[4] &= ~0xFF;
			tbl_prog_brdgeport_ingress.val[4] |= (param->nMacLearningLimit & 0xFF);
			gswdev->brdgeportconfig_idx[param->nBridgePortId].LearningLimit =
				param->nMacLearningLimit;
		}
	}

	/*Same bridge port idx for ingress and egress bridge port configuration*/
	tbl_prog_brdgeport_ingress.table = PCE_IGBGP_INDEX;
	CLEAR_U16(tbl_prog_brdgeport_ingress.pcindex);
	/*Table Entry address (Bridge port ingress Table index) Bit 7:0 in PCE_TBL_ADDR*/
	tbl_prog_brdgeport_ingress.pcindex |= (param->nBridgePortId & 0xFF);

	tbl_prog_brdgeport_egress.table = PCE_EGBGP_INDEX;
	CLEAR_U16(tbl_prog_brdgeport_egress.pcindex);
	/*Table Entry address (Bridge port egress Table index) Bit 7:0 in PCE_TBL_ADDR*/
	tbl_prog_brdgeport_egress.pcindex |= (param->nBridgePortId & 0xFF);

	/*Address-based write for ingress bridge port configuration*/
	gsw_pce_table_write(cdev, &tbl_prog_brdgeport_ingress);
	/*Address-based write for egress bridge port configuration*/
	gsw_pce_table_write(cdev, &tbl_prog_brdgeport_egress);

	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_pce);
#endif
	return ret;
}

GSW_return_t GSW_BridgePortFree(void *cdev, GSW_BRIDGE_portAlloc_t *param)
{
	pctbl_prog_t tbl_prog_brdgeport_ingress;
	pctbl_prog_t tbl_prog_brdgeport_egress;
	GSW_BRIDGE_portConfig_t temp;
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 FirstIdx, count;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_pce);
#endif

	if (param->nBridgePortId >= gswdev->num_of_bridge_port) {
		pr_err("nBridgePortId %d is out of range [num_of_bridge_port supported =%d]\n", param->nBridgePortId, (gswdev->num_of_bridge_port - 1));
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	/*If Bridge Port ID is valid,Check whether it is InUSE
	  if not InUse,return ERROR*/
	if (!gswdev->brdgeportconfig_idx[param->nBridgePortId].IndexInUse) {
		pr_err("nBridgePortId %u is Not inUse -> need to allocate before freeing\n", param->nBridgePortId);
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	/*If this Bridge port usage count is not zero that means it is still used by some one.
	  This Bridge configuration can be deleted, only if that some one release this Bridge
	  port*/
	if (gswdev->brdgeportconfig_idx[param->nBridgePortId].IndexInUsageCnt) {
		count = gswdev->brdgeportconfig_idx[param->nBridgePortId].IndexInUsageCnt;
		pr_err("BridgePortId %u's IndexInUsageCnt=%u is not Zero , some CTP is holding this BP \n",
		       param->nBridgePortId, count);
		pr_err("Free that CTP first,which will detach this association\n");
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	/*Same Bridge port idx for ingress and egress bridge port configuration*/
	memset(&tbl_prog_brdgeport_ingress, 0, sizeof(pctbl_prog_t));
	tbl_prog_brdgeport_ingress.table = PCE_IGBGP_INDEX;
	CLEAR_U16(tbl_prog_brdgeport_ingress.pcindex);
	/*Table Entry address (Bridge port ingress Table index) Bit 7:0 in PCE_TBL_ADDR*/
	tbl_prog_brdgeport_ingress.pcindex |= (param->nBridgePortId & 0xFF);

	memset(&tbl_prog_brdgeport_egress, 0, sizeof(pctbl_prog_t));
	tbl_prog_brdgeport_egress.table = PCE_EGBGP_INDEX;
	CLEAR_U16(tbl_prog_brdgeport_egress.pcindex);
	/*Table Entry address (Bridge port egress Table index) Bit 7:0 in PCE_TBL_ADDR*/
	tbl_prog_brdgeport_egress.pcindex |= (param->nBridgePortId & 0xFF);

	/*Address-based read for ingress bridge port configuration*/
	gsw_pce_table_read(cdev, &tbl_prog_brdgeport_ingress);

	/*Address-based read for egress bridge port configuration*/
	gsw_pce_table_read(cdev, &tbl_prog_brdgeport_egress);

	temp.nBridgeId = ((tbl_prog_brdgeport_ingress.val[4] & 0x3F00) >> 8);

	if (gswdev->brdgeportconfig_idx[param->nBridgePortId].BrdgIdAssigned) {
		if (gswdev->brdgeportconfig_idx[param->nBridgePortId].BrdgId ==
		    temp.nBridgeId) {
			/*Release this Bridge from this Bridge Port*/
			gswdev->brdgeconfig_idx[temp.nBridgeId].IndexInUsageCnt--;
			gswdev->brdgeportconfig_idx[param->nBridgePortId].BrdgIdAssigned = 0;
			gswdev->brdgeportconfig_idx[param->nBridgePortId].BrdgId = 0;
		} else {
			/*if assigned and idx does not match with recorded idx.
			 some thing is wrong.This should never happen*/
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}
	}

	temp.nIngressExtendedVlanBlockId = (tbl_prog_brdgeport_ingress.val[1] & 0x3FFF);

	if (gswdev->brdgeportconfig_idx[param->nBridgePortId].IngressExVlanBlkAssigned) {
		if (gswdev->brdgeportconfig_idx[param->nBridgePortId].IngressExVlanBlkId ==
		    temp.nIngressExtendedVlanBlockId) {
			/*Release this vlan blk from this Bridge Port*/
			FirstIdx = temp.nIngressExtendedVlanBlockId;
			gswdev->extendvlan_idx.vlan_idx[FirstIdx].IndexInUsageCnt--;
			gswdev->brdgeportconfig_idx[param->nBridgePortId].IngressExVlanBlkAssigned = 0;
			gswdev->brdgeportconfig_idx[param->nBridgePortId].IngressExVlanBlkId = EXVLAN_ENTRY_INVALID;
		} else {
			/*if assigned and idx does not match with recorded idx.
			 some thing is wrong.This should never happen*/
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}
	}

	temp.nEgressExtendedVlanBlockId = (tbl_prog_brdgeport_egress.val[1] & 0x3FFF);

	if (gswdev->brdgeportconfig_idx[param->nBridgePortId].EgressExVlanBlkAssigned) {
		if (gswdev->brdgeportconfig_idx[param->nBridgePortId].EgressExVlanBlkId ==
		    temp.nEgressExtendedVlanBlockId) {
			/*Release this vlan blk from this Bridge Port*/
			FirstIdx = temp.nEgressExtendedVlanBlockId;
			gswdev->extendvlan_idx.vlan_idx[FirstIdx].IndexInUsageCnt--;
			gswdev->brdgeportconfig_idx[param->nBridgePortId].EgressExVlanBlkAssigned = 0;
			gswdev->brdgeportconfig_idx[param->nBridgePortId].EgressExVlanBlkId = EXVLAN_ENTRY_INVALID;

		} else {
			/*if assigned and idx does not match with recorded idx.
			 some thing is wrong.This should never happen*/
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}
	}

	temp.nIngressTrafficMeterId = ((tbl_prog_brdgeport_ingress.val[3] & 0x7F00) >> 8);

	if (gswdev->brdgeportconfig_idx[param->nBridgePortId].IngressMeteringAssigned) {
		if (gswdev->brdgeportconfig_idx[param->nBridgePortId].IngressTrafficMeterId ==
		    temp.nIngressTrafficMeterId) {
			/*Release this meter id from this Bridge Port*/
			gswdev->meter_idx[temp.nIngressTrafficMeterId].IndexInUsageCnt--;
			gswdev->brdgeportconfig_idx[param->nBridgePortId].IngressMeteringAssigned = 0;
			gswdev->brdgeportconfig_idx[param->nBridgePortId].IngressTrafficMeterId = 0;

		} else {
			/*if assigned and idx does not match with recorded idx.
			 some thing is wrong.This should never happen*/
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}
	}

	/*other meter*/
	temp.nEgressTrafficSubMeterId[5] = ((tbl_prog_brdgeport_egress.val[3] & 0x7F00) >> 8);

	if (gswdev->brdgeportconfig_idx[param->nBridgePortId].EgressMeteringAssigned) {
		if (gswdev->brdgeportconfig_idx[param->nBridgePortId].EgressTrafficMeterId ==
		    temp.nEgressTrafficSubMeterId[5]) {
			/*Release this meter id from this Bridge Port*/
			gswdev->meter_idx[temp.nEgressTrafficSubMeterId[5]].IndexInUsageCnt--;
			gswdev->brdgeportconfig_idx[param->nBridgePortId].EgressMeteringAssigned = 0;
			gswdev->brdgeportconfig_idx[param->nBridgePortId].EgressTrafficMeterId = 0;

		} else {
			/*if assigned and idx does not match with recorded idx.
			 some thing is wrong.This should never happen*/
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}
	}

	/*Egress Broadcast Meter*/
	temp.nEgressTrafficSubMeterId[0] = ((tbl_prog_brdgeport_egress.val[5] & 0x7F00) >> 8);

	if (gswdev->brdgeportconfig_idx[param->nBridgePortId].BroadcastMeteringAssigned) {
		if (gswdev->brdgeportconfig_idx[param->nBridgePortId].BroadcastMeteringId ==
		    temp.nEgressTrafficSubMeterId[0]) {
			/*Release this meter id from this Bridge Port*/
			gswdev->meter_idx[temp.nEgressTrafficSubMeterId[0]].IndexInUsageCnt--;
			gswdev->brdgeportconfig_idx[param->nBridgePortId].BroadcastMeteringAssigned = 0;
			gswdev->brdgeportconfig_idx[param->nBridgePortId].BroadcastMeteringId = 0;

		} else {
			/*if assigned and idx does not match with recorded idx.
			 some thing is wrong.This should never happen*/
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}
	}

	/*Egress Multicast Meter*/
	temp.nEgressTrafficSubMeterId[1] = ((tbl_prog_brdgeport_egress.val[6] & 0x7F00) >> 8);

	if (gswdev->brdgeportconfig_idx[param->nBridgePortId].MulticastMeteringAssigned) {
		if (gswdev->brdgeportconfig_idx[param->nBridgePortId].MulticastMeteringId ==
		    temp.nEgressTrafficSubMeterId[1]) {
			/*Release this meter id from this Bridge Port*/
			gswdev->meter_idx[temp.nEgressTrafficSubMeterId[1]].IndexInUsageCnt--;
			gswdev->brdgeportconfig_idx[param->nBridgePortId].MulticastMeteringAssigned = 0;
			gswdev->brdgeportconfig_idx[param->nBridgePortId].MulticastMeteringId = 0;

		} else {
			/*if assigned and idx does not match with recorded idx.
			 some thing is wrong.This should never happen*/
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}
	}

	/*Egress Unknown Multicast IP Meter*/
	temp.nEgressTrafficSubMeterId[2] = ((tbl_prog_brdgeport_egress.val[9] & 0x7F00) >> 8);

	if (gswdev->brdgeportconfig_idx[param->nBridgePortId].UnknownMultiIpMeteringAssigned) {
		if (gswdev->brdgeportconfig_idx[param->nBridgePortId].UnknownMultiIpMeteringId ==
		    temp.nEgressTrafficSubMeterId[2]) {
			/*Release this meter id from this Bridge Port*/
			gswdev->meter_idx[temp.nEgressTrafficSubMeterId[2]].IndexInUsageCnt--;
			gswdev->brdgeportconfig_idx[param->nBridgePortId].UnknownMultiIpMeteringAssigned = 0;
			gswdev->brdgeportconfig_idx[param->nBridgePortId].UnknownMultiIpMeteringId = 0;

		} else {
			/*if assigned and idx does not match with recorded idx.
			 some thing is wrong.This should never happen*/
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}
	}

	/*Egress Unknown Multicast NON IP Meter*/
	temp.nEgressTrafficSubMeterId[3] = ((tbl_prog_brdgeport_egress.val[8] & 0x7F00) >> 8);

	if (gswdev->brdgeportconfig_idx[param->nBridgePortId].UnknownMultiNonIpMeteringAssigned) {
		if (gswdev->brdgeportconfig_idx[param->nBridgePortId].UnknownMultiNonIpMeteringId ==
		    temp.nEgressTrafficSubMeterId[3]) {
			/*Release this meter id from this Bridge Port*/
			gswdev->meter_idx[temp.nEgressTrafficSubMeterId[3]].IndexInUsageCnt--;
			gswdev->brdgeportconfig_idx[param->nBridgePortId].UnknownMultiNonIpMeteringAssigned = 0;
			gswdev->brdgeportconfig_idx[param->nBridgePortId].UnknownMultiNonIpMeteringId = 0;

		} else {
			/*if assigned and idx does not match with recorded idx.
			 some thing is wrong.This should never happen*/
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}
	}

	/*Egress Unknown UniCast Meter*/
	temp.nEgressTrafficSubMeterId[4] = ((tbl_prog_brdgeport_egress.val[7] & 0x7F00) >> 8);

	if (gswdev->brdgeportconfig_idx[param->nBridgePortId].UnknownUniCastMeteringAssigned) {
		if (gswdev->brdgeportconfig_idx[param->nBridgePortId].UnknownUniCastMeteringId ==
		    temp.nEgressTrafficSubMeterId[4]) {
			/*Release this meter id from this Bridge Port*/
			gswdev->meter_idx[temp.nEgressTrafficSubMeterId[4]].IndexInUsageCnt--;
			gswdev->brdgeportconfig_idx[param->nBridgePortId].UnknownUniCastMeteringAssigned = 0;
			gswdev->brdgeportconfig_idx[param->nBridgePortId].UnknownUniCastMeteringId = 0;

		} else {
			/*if assigned and idx does not match with recorded idx.
			 some thing is wrong.This should never happen*/
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}
	}

	temp.sPmapper.nPmapperId = (tbl_prog_brdgeport_egress.val[4] & 0xFF);

	if (gswdev->brdgeportconfig_idx[param->nBridgePortId].PmapperAssigned) {
		/*Release this p-mapper idx from this Bridge Port*/
		gswdev->pmapper_idx[temp.sPmapper.nPmapperId].IndexInUse = 0;
		gswdev->brdgeportconfig_idx[param->nBridgePortId].PmapperAssigned = 0;
		gswdev->brdgeportconfig_idx[param->nBridgePortId].PmappperIdx = 0;
	}

	temp.nIngressVlanFilterBlockId = (tbl_prog_brdgeport_ingress.val[5] & 0x3FF);

	if (gswdev->brdgeportconfig_idx[param->nBridgePortId].IngressVlanFilterAssigned) {
		if (gswdev->brdgeportconfig_idx[param->nBridgePortId].IngressVlanFilterBlkId ==
		    temp.nIngressVlanFilterBlockId) {
			/*Release this vlan blk from this Bridge Port*/
			FirstIdx = temp.nIngressVlanFilterBlockId;
			gswdev->vlanfilter_idx.filter_idx[FirstIdx].IndexInUsageCnt--;
			gswdev->brdgeportconfig_idx[param->nBridgePortId].IngressVlanFilterAssigned = 0;
			gswdev->brdgeportconfig_idx[param->nBridgePortId].IngressVlanFilterBlkId = VLANFILTER_ENTRY_INVALID;

		} else {
			/*if assigned and idx does not match with recorded idx.
			 some thing is wrong.This should never happen*/
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}
	}

	temp.nEgressVlanFilter1BlockId = (tbl_prog_brdgeport_egress.val[10] & 0x3FF);

	if (gswdev->brdgeportconfig_idx[param->nBridgePortId].EgressVlanFilter1Assigned) {
		if (gswdev->brdgeportconfig_idx[param->nBridgePortId].EgressVlanFilter1BlkId ==
		    temp.nEgressVlanFilter1BlockId) {
			/*Release this vlan blk from this Bridge Port*/
			FirstIdx = temp.nEgressVlanFilter1BlockId;
			gswdev->vlanfilter_idx.filter_idx[FirstIdx].IndexInUsageCnt--;
			gswdev->brdgeportconfig_idx[param->nBridgePortId].EgressVlanFilter1Assigned = 0;
			gswdev->brdgeportconfig_idx[param->nBridgePortId].EgressVlanFilter1BlkId = VLANFILTER_ENTRY_INVALID;

		} else {
			/*if assigned and idx does not match with recorded idx.
			 some thing is wrong.This should never happen*/
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}
	}

	temp.nEgressVlanFilter2BlockId = (tbl_prog_brdgeport_egress.val[12] & 0x3FF);

	if (gswdev->brdgeportconfig_idx[param->nBridgePortId].EgressVlanFilter2Assigned) {
		if (gswdev->brdgeportconfig_idx[param->nBridgePortId].EgressVlanFilter2BlkId ==
		    temp.nEgressVlanFilter2BlockId) {
			/*Release this vlan blk from this Bridge Port*/
			FirstIdx = temp.nEgressVlanFilter2BlockId;
			gswdev->vlanfilter_idx.filter_idx[FirstIdx].IndexInUsageCnt--;
			gswdev->brdgeportconfig_idx[param->nBridgePortId].EgressVlanFilter2Assigned = 0;
			gswdev->brdgeportconfig_idx[param->nBridgePortId].EgressVlanFilter2BlkId = VLANFILTER_ENTRY_INVALID;

		} else {
			/*if assigned and idx does not match with recorded idx.
			 some thing is wrong.This should never happen*/
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}
	}

	/*Zero the ingress/egress bridge port table index*/
	/*Same Bridge port idx for ingress and egress bridge port configuration*/
	memset(&tbl_prog_brdgeport_ingress, 0, sizeof(pctbl_prog_t));
	tbl_prog_brdgeport_ingress.table = PCE_IGBGP_INDEX;
	CLEAR_U16(tbl_prog_brdgeport_ingress.pcindex);
	/*Table Entry address (Bridge port ingress Table index) Bit 7:0 in PCE_TBL_ADDR*/
	tbl_prog_brdgeport_ingress.pcindex |= (param->nBridgePortId & 0xFF);

	memset(&tbl_prog_brdgeport_egress, 0, sizeof(pctbl_prog_t));
	tbl_prog_brdgeport_egress.table = PCE_EGBGP_INDEX;
	CLEAR_U16(tbl_prog_brdgeport_egress.pcindex);
	/*Table Entry address (Bridge port egress Table index) Bit 7:0 in PCE_TBL_ADDR*/
	tbl_prog_brdgeport_egress.pcindex |= (param->nBridgePortId & 0xFF);

	/*Restore the Learing Limit 255*/
	tbl_prog_brdgeport_ingress.val[4] |= 0xFF;
	gswdev->brdgeportconfig_idx[param->nBridgePortId].LearningLimit = 0xFF;
	/*Stp State disabled for all traffic*/
	tbl_prog_brdgeport_ingress.val[0] |= 0x3;
	tbl_prog_brdgeport_egress.val[0] |= 0x3;
	gswdev->brdgeportconfig_idx[param->nBridgePortId].StpState =
		GSW_STP_PORT_STATE_DISABLE;
	gswdev->brdgeportconfig_idx[param->nBridgePortId].P8021xState =
		GSW_8021X_PORT_STATE_UNAUTHORIZED;

	/*Address-based write for ingress bridge port configuration*/
	gsw_pce_table_write(cdev, &tbl_prog_brdgeport_ingress);
	/*Address-based write for egress bridge port configuration*/
	gsw_pce_table_write(cdev, &tbl_prog_brdgeport_egress);

	/*Free this bridge port idx*/
	gswdev->brdgeportconfig_idx[param->nBridgePortId].IndexInUse = 0;

	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_pce);
#endif
	return ret;
}

GSW_return_t GSW_BridgePortConfigGet(void *cdev, GSW_BRIDGE_portConfig_t *param)
{
	pctbl_prog_t tbl_prog_brdgeport_ingress;
	pctbl_prog_t tbl_prog_brdgeport_egress;
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 value, FirstIdx, LastIdx, BlkSize;
	u16 i, val_reg_idx, ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_pce);
#endif

	if (param->nBridgePortId >= gswdev->num_of_bridge_port) {
		pr_err("nBridgePortId %d is out of range [num_of_bridge_port supported =%d]\n", param->nBridgePortId, (gswdev->num_of_bridge_port - 1));
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	/*GSW_BRIDGE_PORT_CONFIG_MASK_FORCE is for debugging purpose only
	 if this mask is enabled , there is no check on index in-use*/
	if (!(param->eMask & GSW_BRIDGE_PORT_CONFIG_MASK_FORCE)) {
		/*If Bridge Port ID is valid,Check whether it is InUSE
		  if not InUse,return ERROR*/
		if (!(gswdev->brdgeportconfig_idx[param->nBridgePortId].IndexInUse)) {
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}
	}

	/*Same bridge port idx for ingress and egress bridge port configuration*/
	memset(&tbl_prog_brdgeport_ingress, 0, sizeof(pctbl_prog_t));
	tbl_prog_brdgeport_ingress.table = PCE_IGBGP_INDEX;
	CLEAR_U16(tbl_prog_brdgeport_ingress.pcindex);
	/*Table Entry address (Bridge port ingress Table index) Bit 7:0 in PCE_TBL_ADDR*/
	tbl_prog_brdgeport_ingress.pcindex |= (param->nBridgePortId & 0xFF);

	memset(&tbl_prog_brdgeport_egress, 0, sizeof(pctbl_prog_t));
	tbl_prog_brdgeport_egress.table = PCE_EGBGP_INDEX;
	CLEAR_U16(tbl_prog_brdgeport_egress.pcindex);
	/*Table Entry address (Bridge port egress Table index) Bit 7:0 in PCE_TBL_ADDR*/
	tbl_prog_brdgeport_egress.pcindex |= (param->nBridgePortId & 0xFF);

	/*Address-based read for ingress bridge port configuration*/
	gsw_pce_table_read(cdev, &tbl_prog_brdgeport_ingress);

	/*Address-based read for egress bridge port configuration*/
	gsw_pce_table_read(cdev, &tbl_prog_brdgeport_egress);

	if (param->eMask & GSW_BRIDGE_PORT_CONFIG_MASK_BRIDGE_ID)
		param->nBridgeId = ((tbl_prog_brdgeport_ingress.val[4] & 0x3F00) >> 8);

	if (param->eMask & GSW_BRIDGE_PORT_CONFIG_MASK_MAC_LEARNING_LIMIT)
		param->nMacLearningLimit = (tbl_prog_brdgeport_ingress.val[4] & 0xFF);

	if (param->eMask & GSW_BRIDGE_PORT_CONFIG_MASK_INGRESS_VLAN) {
		param->bIngressExtendedVlanEnable = ((tbl_prog_brdgeport_ingress.val[1] & 0x4000) >> 14);

		if (param->bIngressExtendedVlanEnable) {
			param->nIngressExtendedVlanBlockId = ((tbl_prog_brdgeport_ingress.val[1] & 0x3FF));
			FirstIdx	= param->nIngressExtendedVlanBlockId;
			LastIdx 	= (tbl_prog_brdgeport_ingress.val[2]  & 0x3FF);

			if (FirstIdx == LastIdx)
				BlkSize = 1;
			else
				BlkSize = ((LastIdx - FirstIdx) + 1);

			param->nIngressExtendedVlanBlockSize = BlkSize;
		}
	}

	if (param->eMask & GSW_BRIDGE_PORT_CONFIG_MASK_EGRESS_VLAN) {
		param->bEgressExtendedVlanEnable = ((tbl_prog_brdgeport_egress.val[1] & 0x4000) >> 14);

		if (param->bEgressExtendedVlanEnable) {
			param->nEgressExtendedVlanBlockId = (tbl_prog_brdgeport_egress.val[1] & 0x3FF);
			FirstIdx	= param->nEgressExtendedVlanBlockId;
			LastIdx 	= (tbl_prog_brdgeport_egress.val[2]  & 0x3FF);

			if (FirstIdx == LastIdx)
				BlkSize = 1;
			else
				BlkSize = ((LastIdx - FirstIdx) + 1);

			param->nEgressExtendedVlanBlockSize = BlkSize;
		}
	}

	if (param->eMask & GSW_BRIDGE_PORT_CONFIG_MASK_INGRESS_MARKING) {
		value = (tbl_prog_brdgeport_ingress.val[3] & 0x7);

		switch (value) {
		case 0:
			param->eIngressMarkingMode = GSW_MARKING_ALL_GREEN;
			break;

		case 1:
			param->eIngressMarkingMode = GSW_MARKING_INTERNAL_MARKING;
			break;

		case 2:
			param->eIngressMarkingMode = GSW_MARKING_DEI;
			break;

		case 3:
			param->eIngressMarkingMode = GSW_MARKING_PCP_8P0D;
			break;

		case 4:
			param->eIngressMarkingMode = GSW_MARKING_PCP_7P1D;
			break;

		case 5:
			param->eIngressMarkingMode = GSW_MARKING_PCP_6P2D;
			break;

		case 6:
			param->eIngressMarkingMode = GSW_MARKING_PCP_5P3D;
			break;

		case 7:
			param->eIngressMarkingMode = GSW_MARKING_DSCP_AF;
			break;
		}
	}

	if (param->eMask & GSW_BRIDGE_PORT_CONFIG_MASK_EGRESS_REMARKING) {
		value = (tbl_prog_brdgeport_egress.val[3] & 0x7);

		switch (value) {
		case 0:
			param->eEgressRemarkingMode = GSW_REMARKING_NONE;
			break;

		case 1:
			param->eEgressRemarkingMode = GSW_REMARKING_NONE;
			break;

		case 2:
			param->eEgressRemarkingMode = GSW_REMARKING_DEI;
			break;

		case 3:
			param->eEgressRemarkingMode = GSW_REMARKING_PCP_8P0D;
			break;

		case 4:
			param->eEgressRemarkingMode = GSW_REMARKING_PCP_7P1D;
			break;

		case 5:
			param->eEgressRemarkingMode = GSW_REMARKING_PCP_6P2D;
			break;

		case 6:
			param->eEgressRemarkingMode = GSW_REMARKING_PCP_5P3D;
			break;

		case 7:
			param->eEgressRemarkingMode = GSW_REMARKING_DSCP_AF;
			break;
		}
	}

	if (param->eMask & GSW_BRIDGE_PORT_CONFIG_MASK_INGRESS_METER) {
		param->bIngressMeteringEnable = ((tbl_prog_brdgeport_ingress.val[3] & 0x80) >> 7);

		if (param->bIngressMeteringEnable)
			param->nIngressTrafficMeterId = ((tbl_prog_brdgeport_ingress.val[3] & 0x7F00) >> 8);
	}

	if (param->eMask & GSW_BRIDGE_PORT_CONFIG_MASK_EGRESS_SUB_METER) {
		/*Broadcast meter*/
		param->bEgressSubMeteringEnable[0] = ((tbl_prog_brdgeport_egress.val[5] & 0x80) >> 7);

		if (param->bEgressSubMeteringEnable[0])
			param->nEgressTrafficSubMeterId[0] = ((tbl_prog_brdgeport_egress.val[5] & 0x7F00) >> 8);

		/*multicast meter*/
		param->bEgressSubMeteringEnable[1] = ((tbl_prog_brdgeport_egress.val[6] & 0x80) >> 7);

		if (param->bEgressSubMeteringEnable[1])
			param->nEgressTrafficSubMeterId[1] = ((tbl_prog_brdgeport_egress.val[6] & 0x7F00) >> 8);

		/*unknown multicast ip meter*/
		param->bEgressSubMeteringEnable[2] = ((tbl_prog_brdgeport_egress.val[9] & 0x80) >> 7);

		if (param->bEgressSubMeteringEnable[2])
			param->nEgressTrafficSubMeterId[2] = ((tbl_prog_brdgeport_egress.val[9] & 0x7F00) >> 8);

		/*unknown multicast Non ip meter*/
		param->bEgressSubMeteringEnable[3] = ((tbl_prog_brdgeport_egress.val[8] & 0x80) >> 7);

		if (param->bEgressSubMeteringEnable[3])
			param->nEgressTrafficSubMeterId[3] = ((tbl_prog_brdgeport_egress.val[8] & 0x7F00) >> 8);


		/*unknown unicast meter*/
		param->bEgressSubMeteringEnable[4] = ((tbl_prog_brdgeport_egress.val[7] & 0x80) >> 7);

		if (param->bEgressSubMeteringEnable[4])
			param->nEgressTrafficSubMeterId[4] = ((tbl_prog_brdgeport_egress.val[7] & 0x7F00) >> 8);

		/*other meter*/
		param->bEgressSubMeteringEnable[5] = ((tbl_prog_brdgeport_egress.val[3] & 0x80) >> 7);

		if (param->bEgressSubMeteringEnable[5])
			param->nEgressTrafficSubMeterId[5] = ((tbl_prog_brdgeport_egress.val[3] & 0x7F00) >> 8);
	}

	if (param->eMask & GSW_BRIDGE_PORT_CONFIG_MASK_EGRESS_CTP_MAPPING) {
		param->nDestLogicalPortId = ((tbl_prog_brdgeport_egress.val[4] & 0xF00) >> 8);

		param->bPmapperEnable = ((tbl_prog_brdgeport_egress.val[4] & 0x4000) >> 14);
		param->ePmapperMappingMode = ((tbl_prog_brdgeport_egress.val[4] & 0x2000) >> 13);

		if (param->bPmapperEnable) {
			param->sPmapper.nPmapperId = (tbl_prog_brdgeport_egress.val[4] & 0xFF);
			ret = GSW_QOS_PmapperTableGet(cdev, &param->sPmapper);

			if (ret == GSW_statusErr) {
				pr_err("ERROR :P-mapper Get return Error\n");
				goto UNLOCK_AND_RETURN;
			}
		} else {
			param->nDestSubIfIdGroup = (tbl_prog_brdgeport_egress.val[4] & 0xFF);
		}
	}

	if (param->eMask & GSW_BRIDGE_PORT_CONFIG_MASK_BRIDGE_PORT_MAP) {
		val_reg_idx = 10;

		for (i = 0; i < 8; i++) {
			param->nBridgePortMap[i] = (tbl_prog_brdgeport_ingress.val[val_reg_idx] & 0xFFFF);
			val_reg_idx++;
		}
	}

	if (param->eMask & GSW_BRIDGE_PORT_CONFIG_MASK_MC_DEST_IP_LOOKUP)
		param->bMcDestIpLookupDisable = ((~tbl_prog_brdgeport_ingress.val[0] & 0x2000) >> 13);

	if (param->eMask & GSW_BRIDGE_PORT_CONFIG_MASK_MC_SRC_IP_LOOKUP)
		param->bMcSrcIpLookupEnable = ((tbl_prog_brdgeport_ingress.val[0] & 0x200) >> 9);

	if (param->eMask & GSW_BRIDGE_PORT_CONFIG_MASK_MC_DEST_MAC_LOOKUP)
		param->bDestMacLookupDisable = ((tbl_prog_brdgeport_ingress.val[0] & 0x4000) >> 14);

	if (param->eMask & GSW_BRIDGE_PORT_CONFIG_MASK_MC_SRC_MAC_LEARNING)
		param->bSrcMacLearningDisable = ((tbl_prog_brdgeport_ingress.val[0] & 0x8000) >> 15);

	if (param->eMask & GSW_BRIDGE_PORT_CONFIG_MASK_MAC_SPOOFING)
		param->bMacSpoofingDetectEnable = ((tbl_prog_brdgeport_ingress.val[0] & 0x800) >> 11);

	if (param->eMask & GSW_BRIDGE_PORT_CONFIG_MASK_PORT_LOCK)
		param->bPortLockEnable = ((tbl_prog_brdgeport_ingress.val[0] & 0x1000) >> 12);


	if (param->eMask & GSW_BRIDGE_PORT_CONFIG_MASK_INGRESS_VLAN_FILTER) {
		param->bIngressVlanFilterEnable = ((tbl_prog_brdgeport_ingress.val[5] & 0x4000) >> 14);

		if (param->bIngressVlanFilterEnable) {
			param->nIngressVlanFilterBlockId = (tbl_prog_brdgeport_ingress.val[5] & 0x3FF);
			FirstIdx	= param->nIngressVlanFilterBlockId;
			LastIdx 	= (tbl_prog_brdgeport_ingress.val[6]  & 0x3FF);

			if (FirstIdx == LastIdx)
				BlkSize = 1;
			else
				BlkSize = ((LastIdx - FirstIdx) + 1);

			param->nIngressVlanFilterBlockSize = BlkSize;
		}

		param->bBypassEgressVlanFilter1 = ((tbl_prog_brdgeport_ingress.val[5] & 0x8000) >> 15);
	}

	if (param->eMask & GSW_BRIDGE_PORT_CONFIG_MASK_EGRESS_VLAN_FILTER1) {
		param->bEgressVlanFilter1Enable = ((tbl_prog_brdgeport_egress.val[10] & 0x4000) >> 14);

		if (param->bEgressVlanFilter1Enable) {
			param->nEgressVlanFilter1BlockId = (tbl_prog_brdgeport_egress.val[10] & 0x3FF);
			FirstIdx	= param->nEgressVlanFilter1BlockId;
			LastIdx 	= (tbl_prog_brdgeport_egress.val[11]  & 0x3FF);

			if (FirstIdx == LastIdx)
				BlkSize = 1;
			else
				BlkSize = ((LastIdx - FirstIdx) + 1);

			param->nEgressVlanFilter1BlockSize = BlkSize;
		}
	}

	if (param->eMask & GSW_BRIDGE_PORT_CONFIG_MASK_EGRESS_VLAN_FILTER2) {
		param->bEgressVlanFilter2Enable = ((tbl_prog_brdgeport_egress.val[12] & 0x4000) >> 14);

		if (param->bEgressVlanFilter2Enable) {
			param->nEgressVlanFilter2BlockId = (tbl_prog_brdgeport_egress.val[12] & 0x3FF);
			FirstIdx	= param->nEgressVlanFilter2BlockId;
			LastIdx 	= (tbl_prog_brdgeport_egress.val[13]  & 0x3FF);

			if (FirstIdx == LastIdx)
				BlkSize = 1;
			else
				BlkSize = ((LastIdx - FirstIdx) + 1);

			param->nEgressVlanFilter2BlockSize = BlkSize;
		}
	}

	if (param->eMask & GSW_BRIDGE_PORT_CONFIG_MASK_MAC_LEARNED_COUNT)
		param->nMacLearningCount = (tbl_prog_brdgeport_ingress.val[9] & 0x1FFF);

	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_pce);
#endif
	return ret;
}

GSW_return_t GSW_CtpPortConfigSet(void *cdev, GSW_CTP_portConfig_t *param)
{
	pctbl_prog_t tbl_prog_ctpport_ingress;
	pctbl_prog_t tbl_prog_ctpport_egress;
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 idx = 0, FirstIdx, LastIdx, pmapper_idx, ctp_port, ctplastidx;
	u32 BlkSize = 0;
	u32 ret;
	GSW_CTP_portAssignment_t ctp_get;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_pce);
#endif

	/*Checking based on number of logical port*/
	if (param->nLogicalPortId >= gswdev->tpnum) {
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	/*CTP ports has to assigned/mapped to Logical port before calling
	  CTP port configuration.
	  GSW_CTP_PortAssignmentGet will get the info on the assignment
	  Use this info to find whether this CTP port is with in the range
	  this logical port
	*/
	ctp_get.nLogicalPortId = param->nLogicalPortId;
	ret = GSW_CTP_PortAssignmentGet(cdev, &ctp_get);

	if (ret == GSW_statusErr) {
		pr_err("GSW_CTP_PortAssignmentGet returns ERROR\n");
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	/*IMPORTANT NOTE : :As per the CTP concept  nFirstCtpPortId + nSubIfIdGroup is the expected CTP port*/
	ctp_port = ctp_get.nFirstCtpPortId  + param->nSubIfIdGroup;

	if (ctp_port >= gswdev->num_of_ctp) {
		pr_err("ctp_port %d  >=  gswdev->num_of_ctp %d\n", ctp_port, gswdev->num_of_ctp);
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	/*If not with in the Assigned range Return Error -This check is just extra*/
	if (ctp_port < ctp_get.nFirstCtpPortId ||
	    ctp_port >= (ctp_get.nFirstCtpPortId + ctp_get.nNumberOfCtpPort)) {
		pr_err("ERROR:ctp_port %d  Assigned range not correct\n", ctp_port);
		pr_err("ERROR:Check using CTP assignment get\n");
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	/*NOTE : This Mask is for debugging purpose only
	  If GSW_CTP_PORT_CONFIG_MASK_FORCE mask is set
	  It will not check index in-use */
	if (!(param->eMask & GSW_CTP_PORT_CONFIG_MASK_FORCE)) {
		/*Check whether CTP is allocated by GSW_CTP_PortAssignmentAlloc*/
		if (!gswdev->ctpportconfig_idx[ctp_port].IndexInUse) {
			pr_err("ERROR :ctp_port %d Index not in use,CTP not allocated\n", ctp_port);
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}
	}

	/*Same ctp port idx for ingress and egress ctp port configuration*/
	memset(&tbl_prog_ctpport_ingress, 0, sizeof(pctbl_prog_t));
	tbl_prog_ctpport_ingress.table = PCE_IGCTP_INDEX;
	CLEAR_U16(tbl_prog_ctpport_ingress.pcindex);
	/*Table Entry address (ctp port ingress Table index) Bit 8:0 in PCE_TBL_ADDR*/
	tbl_prog_ctpport_ingress.pcindex |= (ctp_port & 0x1FF);

	memset(&tbl_prog_ctpport_egress, 0, sizeof(pctbl_prog_t));
	tbl_prog_ctpport_egress.table = PCE_EGCTP_INDEX;
	CLEAR_U16(tbl_prog_ctpport_egress.pcindex);
	/*Table Entry address (ctp port egress Table index) Bit 8:0 in PCE_TBL_ADDR*/
	tbl_prog_ctpport_egress.pcindex |= (ctp_port & 0x1FF);

	/*Address-based read for ingress ctp port configuration
	  read before set, so that previous value of this table index
	  is not disturbed in the case of update specific fields.
	*/
	gsw_pce_table_read(cdev, &tbl_prog_ctpport_ingress);

	/*Address-based read for egress ctp port configuration
	  read before set, so that previous value of this table index
	  is not disturbed in the case of update specific fields.
	*/
	gsw_pce_table_read(cdev, &tbl_prog_ctpport_egress);

	if (param->eMask & GSW_CTP_PORT_CONFIG_MASK_BRIDGE_PORT_ID) {
		if (param->nBridgePortId >= gswdev->num_of_bridge_port) {
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}

		if (!(param->eMask & GSW_CTP_PORT_CONFIG_MASK_FORCE)) {

			/*This below field enable indicates that this bridge port id is
			 already assigned and it's idx recorded for this CTP Port id.
			 If next time the user update this field with different
			 bridge port idx !!or wanted to allocate a new bridge port idx ??!!
			 may happen by mistake ??!!
			 the previous bridge port idx must be released from this CTP Port*/

			if (gswdev->ctpportconfig_idx[ctp_port].BrdgIdPortAssigned) {
				if (gswdev->ctpportconfig_idx[ctp_port].BrdgPortId !=
				    param->nBridgePortId) {
					idx = gswdev->ctpportconfig_idx[ctp_port].BrdgPortId;
					/*release the usage of previous bridge port idx*/
					gswdev->brdgeportconfig_idx[idx].IndexInUsageCnt--;
					gswdev->ctpportconfig_idx[ctp_port].BrdgIdPortAssigned = 0;
					gswdev->ctpportconfig_idx[ctp_port].BrdgPortId = 0;
				}
			}

			/*This Bridge Port ID should be in use (i.e) that is this Bridge Port ID
			  should be allocated
			  before calling CTP port configuration
			  If not in use return ERROR
			  */
			if (gswdev->brdgeportconfig_idx[param->nBridgePortId].IndexInUse) {
				/*Usage count will be incremented only once during bridge port idx assignment
				for this CTP port id*/
				if (!gswdev->ctpportconfig_idx[ctp_port].BrdgIdPortAssigned) {
					gswdev->ctpportconfig_idx[ctp_port].BrdgIdPortAssigned = 1;
					gswdev->ctpportconfig_idx[ctp_port].BrdgPortId
						= param->nBridgePortId;
					/*Since this bridge port id can be shared,Increment it's usage count*/
					gswdev->brdgeportconfig_idx[param->nBridgePortId].IndexInUsageCnt++;
				}
			} else {
				ret = GSW_statusErr;
				goto UNLOCK_AND_RETURN;
			}
		}

		tbl_prog_ctpport_ingress.val[0] &= ~(0xFF);
		tbl_prog_ctpport_ingress.val[0] |= (param->nBridgePortId & 0xFF);
	}

	if (param->eMask & GSW_CTP_PORT_CONFIG_MASK_FORCE_TRAFFIC_CLASS) {
		/** Default traffic class associated with all traffic from this CTP Port. */
		tbl_prog_ctpport_ingress.val[0] &= ~(0xF << 8);
		tbl_prog_ctpport_ingress.val[0] |= ((param->nDefaultTrafficClass & 0xF) << 8);

		/** Default traffic class can not be overridden by other rules (except
			traffic flow table and special tag) in following stages. */
		/* 0 -Traffic class can be over written by the following processing stage
		   1 -Traffic class can not be over written by the following processing stage
		*/
		if (param->bForcedTrafficClass)
			tbl_prog_ctpport_ingress.val[0] |= (1 << 12);
		else
			tbl_prog_ctpport_ingress.val[0] &= ~(1 << 12);
	}

	if (param->eMask & GSW_CTP_PORT_CONFIG_MASK_INGRESS_VLAN) {
		if (param->bIngressExtendedVlanEnable) {
			/*This below field enable indicates that this vlan block is
			 already assigned and it's Blk id recorded for this CTP Port id.
			 If next time the user update this Vlan blk field with different blk id !!
			 or wanted to allocate a new vlan blk ??!! may happen by mistake ??!!
			 the previous blk id must be released from this CTP Port*/

			if (!(param->eMask & GSW_CTP_PORT_CONFIG_MASK_FORCE)) {
				if (gswdev->ctpportconfig_idx[ctp_port].IngressExVlanNonIgmpBlkAssigned) {
					if (gswdev->ctpportconfig_idx[ctp_port].IngressExVlanNonIgmpBlkId !=
					    param->nIngressExtendedVlanBlockId) {
						idx = gswdev->ctpportconfig_idx[ctp_port].IngressExVlanNonIgmpBlkId;
						/*release the usage of previous vlan block id*/
						gswdev->extendvlan_idx.vlan_idx[idx].IndexInUsageCnt--;
						gswdev->ctpportconfig_idx[ctp_port].IngressExVlanNonIgmpBlkAssigned = 0;
						gswdev->ctpportconfig_idx[ctp_port].IngressExVlanNonIgmpBlkId = EXVLAN_ENTRY_INVALID;
					}
				}
			}

			/*Note: First Index of the Block is the BlockID*/
			FirstIdx = param->nIngressExtendedVlanBlockId;

			if (FirstIdx > gswdev->num_of_extendvlan) {
				ret = GSW_statusErr;
				goto UNLOCK_AND_RETURN;
			}

			/*Check whether this index is InUse
			  Since it will be marked as InUse during allocation.
			*/
			if (!gswdev->extendvlan_idx.vlan_idx[FirstIdx].IndexInUse) {
				ret = GSW_statusErr;
				goto UNLOCK_AND_RETURN;
			}

			/*Check whether this index belongs to this  BlockID
			  Since it will be marked during allocation.
			*/
			if (gswdev->extendvlan_idx.vlan_idx[FirstIdx].VlanBlockId
			    != param->nIngressExtendedVlanBlockId) {
				ret = GSW_statusErr;
				goto UNLOCK_AND_RETURN;
			}

			/*Search for the Last Index of this block.
			  Note: The Blocks are always allocated contiguously.
			*/

			LastIdx = FirstIdx;

			while (gswdev->extendvlan_idx.vlan_idx[LastIdx].VlanBlockId == FirstIdx
			       && gswdev->extendvlan_idx.vlan_idx[LastIdx].IndexInUse) {
				LastIdx++;
			}

			BlkSize = param->nIngressExtendedVlanBlockSize;

			if (BlkSize && ((FirstIdx + BlkSize) <= LastIdx))
				LastIdx = (FirstIdx + BlkSize);

			/*Enable Extended VLAN operation*/
			tbl_prog_ctpport_ingress.val[1] |= (1 << 14);
			/*Set First Index of the Block*/
			tbl_prog_ctpport_ingress.val[1] &= ~(0x3FF);
			tbl_prog_ctpport_ingress.val[1] |= (FirstIdx & 0x3FF);
			/*Set Last Index of the Block*/
			tbl_prog_ctpport_ingress.val[2] &= ~(0x3FF);
			tbl_prog_ctpport_ingress.val[2] |= ((LastIdx - 1) & 0x3FF);

			if (!(param->eMask & GSW_CTP_PORT_CONFIG_MASK_FORCE)) {
				/*Usage count will be incremented only once during vlan blk assignment
				for this CTP port id*/
				if (!gswdev->ctpportconfig_idx[ctp_port].IngressExVlanNonIgmpBlkAssigned) {
					gswdev->ctpportconfig_idx[ctp_port].IngressExVlanNonIgmpBlkAssigned = 1;
					gswdev->ctpportconfig_idx[ctp_port].IngressExVlanNonIgmpBlkId
						= param->nIngressExtendedVlanBlockId;
					/*Since this vlan blk can be shared,Increment it's usage count*/
					gswdev->extendvlan_idx.vlan_idx[FirstIdx].IndexInUsageCnt++;
				}
			}
		} else {
			/*Disable ingress Extended VLAN non igmp*/
			tbl_prog_ctpport_ingress.val[1] &= ~(1 << 14);

			if (gswdev->ctpportconfig_idx[ctp_port].IngressExVlanNonIgmpBlkAssigned) {
				idx = gswdev->ctpportconfig_idx[ctp_port].IngressExVlanNonIgmpBlkId;
				/*release the usage of vlan block id, if it is assigned previously
				 when Extended vlan operation is disabled,it is must to release
				 the mapped exvlan resource*/
				gswdev->extendvlan_idx.vlan_idx[idx].IndexInUsageCnt--;
				gswdev->ctpportconfig_idx[ctp_port].IngressExVlanNonIgmpBlkAssigned = 0;
				gswdev->ctpportconfig_idx[ctp_port].IngressExVlanNonIgmpBlkId = EXVLAN_ENTRY_INVALID;
			}
		}
	}

	if (param->eMask & GSW_CTP_PORT_CONFIG_MASK_INGRESS_VLAN_IGMP) {
		if (param->bIngressExtendedVlanIgmpEnable) {
			/*This below field enable indicates that this vlan block is
			 already assigned and it's Blk id recorded for this CTP Port id.
			 If next time the user update this Vlan blk field with different blk id !!
			 or wanted to allocate a new vlan blk ??!! may happen by mistake ??!!
			 the previous blk id must be released from this CTP Port*/

			if (!(param->eMask & GSW_CTP_PORT_CONFIG_MASK_FORCE)) {
				if (gswdev->ctpportconfig_idx[ctp_port].IngressExVlanIgmpBlkAssigned) {
					if (gswdev->ctpportconfig_idx[ctp_port].IngressExVlanIgmpBlkId !=
					    param->nIngressExtendedVlanBlockIdIgmp) {
						idx = gswdev->ctpportconfig_idx[ctp_port].IngressExVlanIgmpBlkId;
						/*release the usage of previous vlan block id*/
						gswdev->extendvlan_idx.vlan_idx[idx].IndexInUsageCnt--;
						gswdev->ctpportconfig_idx[ctp_port].IngressExVlanIgmpBlkAssigned = 0;
						gswdev->ctpportconfig_idx[ctp_port].IngressExVlanIgmpBlkId = EXVLAN_ENTRY_INVALID;
					}
				}
			}

			/*Note: First Index of the Block is the BlockID*/
			FirstIdx = param->nIngressExtendedVlanBlockIdIgmp;

			if (FirstIdx > gswdev->num_of_extendvlan) {
				ret = GSW_statusErr;
				goto UNLOCK_AND_RETURN;
			}

			/*Check whether this index is InUse
			  Since it will be marked as InUse during allocation.
			*/
			if (!gswdev->extendvlan_idx.vlan_idx[FirstIdx].IndexInUse) {
				ret = GSW_statusErr;
				goto UNLOCK_AND_RETURN;
			}

			/*Check whether this index belongs to this  BlockID
			  Since it will be marked during allocation.
			*/
			if (gswdev->extendvlan_idx.vlan_idx[FirstIdx].VlanBlockId
			    != param->nIngressExtendedVlanBlockIdIgmp) {
				ret = GSW_statusErr;
				goto UNLOCK_AND_RETURN;
			}

			/*Search for the Last Index of this block.
			  Note: The Blocks are always allocated contiguously.
			*/

			LastIdx = FirstIdx;

			while (gswdev->extendvlan_idx.vlan_idx[LastIdx].VlanBlockId == FirstIdx
			       && gswdev->extendvlan_idx.vlan_idx[LastIdx].IndexInUse) {
				LastIdx++;
			}

			BlkSize = param->nIngressExtendedVlanBlockSizeIgmp;

			if (BlkSize && ((FirstIdx + BlkSize) <= LastIdx))
				LastIdx = (FirstIdx + BlkSize);

			/*Enable Extended VLAN operation*/
			tbl_prog_ctpport_ingress.val[5] |= (1 << 14);
			/*Set First Index of the Block*/
			tbl_prog_ctpport_ingress.val[5] &= ~(0x3FF);
			tbl_prog_ctpport_ingress.val[5] |= (FirstIdx & 0x3FF);
			/*Set Last Index of the Block*/
			tbl_prog_ctpport_ingress.val[6] &= ~(0x3FF);
			tbl_prog_ctpport_ingress.val[6] |= ((LastIdx - 1) & 0x3FF);

			if (!(param->eMask & GSW_CTP_PORT_CONFIG_MASK_FORCE)) {
				/*Usage count will be incremented only once during vlan blk assignment
				for this CTP port id*/
				if (!gswdev->ctpportconfig_idx[ctp_port].IngressExVlanIgmpBlkAssigned) {
					gswdev->ctpportconfig_idx[ctp_port].IngressExVlanIgmpBlkAssigned = 1;
					gswdev->ctpportconfig_idx[ctp_port].IngressExVlanIgmpBlkId
						= param->nIngressExtendedVlanBlockIdIgmp;
					/*Since this vlan blk can be shared,Increment it's usage count*/
					gswdev->extendvlan_idx.vlan_idx[FirstIdx].IndexInUsageCnt++;
				}
			}
		} else {
			/*Disable ingress Extended VLAN igmp*/
			tbl_prog_ctpport_ingress.val[5] &= ~(1 << 14);

			if (gswdev->ctpportconfig_idx[ctp_port].IngressExVlanIgmpBlkAssigned) {
				idx = gswdev->ctpportconfig_idx[ctp_port].IngressExVlanIgmpBlkId;
				/*release the usage of vlan block id, if it is assigned previously
				 when Extended vlan operation is disabled,it is must to release
				 the mapped exvlan resource*/
				gswdev->extendvlan_idx.vlan_idx[idx].IndexInUsageCnt--;
				gswdev->ctpportconfig_idx[ctp_port].IngressExVlanIgmpBlkAssigned = 0;
				gswdev->ctpportconfig_idx[ctp_port].IngressExVlanIgmpBlkId = EXVLAN_ENTRY_INVALID;
			}
		}
	}

	if (param->eMask & GSW_CTP_PORT_CONFIG_MASK_EGRESS_VLAN) {
		if (param->bEgressExtendedVlanEnable) {
			if (!(param->eMask & GSW_CTP_PORT_CONFIG_MASK_FORCE)) {
				/*This below field enable indicates that this vlan block is
				 already assigned and it's Blk id recorded for this CTP Port id.
				 If next time the user update this Vlan blk field with different blk id !!
				 or wanted to allocate a new vlan blk ??!! may happen by mistake ??!!
				 the previous blk id must be released from this CTP Port*/

				if (gswdev->ctpportconfig_idx[ctp_port].EgressExVlanNonIgmpBlkAssigned) {
					if (gswdev->ctpportconfig_idx[ctp_port].EgressExVlanNonIgmpBlkId !=
					    param->nEgressExtendedVlanBlockId) {
						idx = gswdev->ctpportconfig_idx[ctp_port].EgressExVlanNonIgmpBlkId;
						/*release the usage of previous vlan block id*/
						gswdev->extendvlan_idx.vlan_idx[idx].IndexInUsageCnt--;
						gswdev->ctpportconfig_idx[ctp_port].EgressExVlanNonIgmpBlkAssigned = 0;
						gswdev->ctpportconfig_idx[ctp_port].EgressExVlanNonIgmpBlkId = EXVLAN_ENTRY_INVALID;
					}
				}

			}

			/*Note: First Index of the Block is the BlockID*/
			FirstIdx = param->nEgressExtendedVlanBlockId;

			if (FirstIdx > gswdev->num_of_extendvlan) {
				ret = GSW_statusErr;
				goto UNLOCK_AND_RETURN;
			}

			/*Check whether this index is InUse
			  Since it will be marked as InUse during allocation.
			*/
			if (!gswdev->extendvlan_idx.vlan_idx[FirstIdx].IndexInUse) {
				ret = GSW_statusErr;
				goto UNLOCK_AND_RETURN;
			}

			/*Check whether this index belongs to this  BlockID
			  Since it will be marked during allocation.
			*/
			if (gswdev->extendvlan_idx.vlan_idx[FirstIdx].VlanBlockId
			    != param->nEgressExtendedVlanBlockId) {
				ret = GSW_statusErr;
				goto UNLOCK_AND_RETURN;
			}

			/*Search for the Last Index of this block.
			  Note: The Blocks are always allocated contiguously.
			*/

			LastIdx = FirstIdx;

			while (gswdev->extendvlan_idx.vlan_idx[LastIdx].VlanBlockId == FirstIdx
			       && gswdev->extendvlan_idx.vlan_idx[LastIdx].IndexInUse) {
				LastIdx++;
			}

			BlkSize = param->nEgressExtendedVlanBlockSize;

			if (BlkSize && ((FirstIdx + BlkSize) <= LastIdx))
				LastIdx = (FirstIdx + BlkSize);

			/*Enable Extended VLAN operation*/
			tbl_prog_ctpport_egress.val[1] |= (1 << 14);
			/*Set First Index of the Block*/
			tbl_prog_ctpport_egress.val[1] &= ~(0x3FF);
			tbl_prog_ctpport_egress.val[1] |= (FirstIdx & 0x3FF);
			/*Set Last Index of the Block*/
			tbl_prog_ctpport_egress.val[2] &= ~(0x3FF);
			tbl_prog_ctpport_egress.val[2] |= ((LastIdx - 1) & 0x3FF);

			if (!(param->eMask & GSW_CTP_PORT_CONFIG_MASK_FORCE)) {
				/*Usage count will be incremented only once during vlan blk assignment
				for this CTP port id*/
				if (!gswdev->ctpportconfig_idx[ctp_port].EgressExVlanNonIgmpBlkAssigned) {
					gswdev->ctpportconfig_idx[ctp_port].EgressExVlanNonIgmpBlkAssigned = 1;
					gswdev->ctpportconfig_idx[ctp_port].EgressExVlanNonIgmpBlkId
						= param->nEgressExtendedVlanBlockId;
					/*Since this vlan blk can be shared,Increment it's usage count*/
					gswdev->extendvlan_idx.vlan_idx[FirstIdx].IndexInUsageCnt++;
				}
			}
		} else {
			/*Disable egress Extended VLAN non igmp*/
			tbl_prog_ctpport_egress.val[1] &= ~(1 << 14);

			if (gswdev->ctpportconfig_idx[ctp_port].EgressExVlanNonIgmpBlkAssigned) {
				idx = gswdev->ctpportconfig_idx[ctp_port].EgressExVlanNonIgmpBlkId;
				/*release the usage of vlan block id, if it is assigned previously
				 when Extended vlan operation is disabled,it is must to release
				 the mapped exvlan resource*/
				gswdev->extendvlan_idx.vlan_idx[idx].IndexInUsageCnt--;
				gswdev->ctpportconfig_idx[ctp_port].EgressExVlanNonIgmpBlkAssigned = 0;
				gswdev->ctpportconfig_idx[ctp_port].EgressExVlanNonIgmpBlkId = EXVLAN_ENTRY_INVALID;
			}
		}
	}

	if (param->eMask & GSW_CTP_PORT_CONFIG_MASK_EGRESS_VLAN_IGMP) {
		if (param->bEgressExtendedVlanIgmpEnable) {
			if (!(param->eMask & GSW_CTP_PORT_CONFIG_MASK_FORCE)) {
				/*This below field enable indicates that this vlan block is
				 already assigned and it's Blk id recorded for this CTP Port id.
				 If next time the user update this Vlan blk field with different blk id !!
				 or wanted to allocate a new vlan blk ??!! may happen by mistake ??!!
				 the previous blk id must be released from this CTP Port*/

				if (gswdev->ctpportconfig_idx[ctp_port].EgressExVlanIgmpBlkAssigned) {
					if (gswdev->ctpportconfig_idx[ctp_port].EgressExVlanIgmpBlkId !=
					    param->nEgressExtendedVlanBlockIdIgmp) {
						idx = gswdev->ctpportconfig_idx[ctp_port].EgressExVlanIgmpBlkId;
						/*release the usage of previous vlan block id*/
						gswdev->extendvlan_idx.vlan_idx[idx].IndexInUsageCnt--;
						gswdev->ctpportconfig_idx[ctp_port].EgressExVlanIgmpBlkAssigned = 0;
						gswdev->ctpportconfig_idx[ctp_port].EgressExVlanIgmpBlkId = EXVLAN_ENTRY_INVALID;
					}
				}
			}

			/*Note: First Index of the Block is the BlockID*/
			FirstIdx = param->nEgressExtendedVlanBlockIdIgmp;

			if (FirstIdx > gswdev->num_of_extendvlan) {
				ret = GSW_statusErr;
				goto UNLOCK_AND_RETURN;
			}

			/*Check whether this index is InUse
			  Since it will be marked as InUse during allocation.
			*/
			if (!gswdev->extendvlan_idx.vlan_idx[FirstIdx].IndexInUse) {
				ret = GSW_statusErr;
				goto UNLOCK_AND_RETURN;
			}

			/*Check whether this index belongs to this  BlockID
			  Since it will be marked during allocation.
			*/
			if (gswdev->extendvlan_idx.vlan_idx[FirstIdx].VlanBlockId
			    != param->nEgressExtendedVlanBlockIdIgmp) {
				ret = GSW_statusErr;
				goto UNLOCK_AND_RETURN;
			}

			/*Search for the Last Index of this block.
			  Note: The Blocks are always allocated contiguously.
			*/

			LastIdx = FirstIdx;

			while (gswdev->extendvlan_idx.vlan_idx[LastIdx].VlanBlockId == FirstIdx
			       && gswdev->extendvlan_idx.vlan_idx[LastIdx].IndexInUse) {
				LastIdx++;
			}

			BlkSize = param->nEgressExtendedVlanBlockSizeIgmp;

			if (BlkSize && ((FirstIdx + BlkSize) <= LastIdx))
				LastIdx = (FirstIdx + BlkSize);

			/*Enable Extended VLAN operation*/
			tbl_prog_ctpport_egress.val[5] |= (1 << 14);
			/*Set First Index of the Block*/
			tbl_prog_ctpport_egress.val[5] &= ~(0x3FF);
			tbl_prog_ctpport_egress.val[5] |= (FirstIdx & 0x3FF);
			/*Set Last Index of the Block*/
			tbl_prog_ctpport_egress.val[6] &= ~(0x3FF);
			tbl_prog_ctpport_egress.val[6] |= ((LastIdx - 1) & 0x3FF);

			if (!(param->eMask & GSW_CTP_PORT_CONFIG_MASK_FORCE)) {
				/*Usage count will be incremented only once during vlan blk assignment
				for this CTP port id*/
				if (!gswdev->ctpportconfig_idx[ctp_port].EgressExVlanIgmpBlkAssigned) {
					gswdev->ctpportconfig_idx[ctp_port].EgressExVlanIgmpBlkAssigned = 1;
					gswdev->ctpportconfig_idx[ctp_port].EgressExVlanIgmpBlkId
						= param->nEgressExtendedVlanBlockIdIgmp;
					/*Since this vlan blk can be shared,Increment it's usage count*/
					gswdev->extendvlan_idx.vlan_idx[FirstIdx].IndexInUsageCnt++;
				}
			}
		} else {
			/*Disable ingress Extended VLAN igmp*/
			tbl_prog_ctpport_egress.val[5] &= ~(1 << 14);

			if (gswdev->ctpportconfig_idx[ctp_port].EgressExVlanIgmpBlkAssigned) {
				idx = gswdev->ctpportconfig_idx[ctp_port].EgressExVlanIgmpBlkId;
				/*release the usage of vlan block id, if it is assigned previously
				 when Extended vlan operation is disabled,it is must to release
				 the mapped exvlan resource*/
				gswdev->extendvlan_idx.vlan_idx[idx].IndexInUsageCnt--;
				gswdev->ctpportconfig_idx[ctp_port].EgressExVlanIgmpBlkAssigned = 0;
				gswdev->ctpportconfig_idx[ctp_port].EgressExVlanIgmpBlkId = EXVLAN_ENTRY_INVALID;
			}
		}
	}

	if (param->eMask & GSW_CTP_PORT_CONFIG_MASK_INRESS_NTO1_VLAN) {
		if (param->bIngressNto1VlanEnable)
			tbl_prog_ctpport_ingress.val[1] |= (1 << 15);
		else
			tbl_prog_ctpport_ingress.val[1] &= ~(1 << 15);
	}

	if (param->eMask & GSW_CTP_PORT_CONFIG_MASK_EGRESS_NTO1_VLAN) {
		if (param->bEgressNto1VlanEnable)
			tbl_prog_ctpport_egress.val[1] |= (1 << 15);
		else
			tbl_prog_ctpport_egress.val[1] &= ~(1 << 15);
	}

	if (param->eMask & GSW_CTP_PORT_CONFIG_INGRESS_METER) {
		if (param->bIngressMeteringEnable) {
			if (param->nIngressTrafficMeterId > gswdev->num_of_meters) {
				ret = GSW_statusErr;
				goto UNLOCK_AND_RETURN;
			}

			if (!(param->eMask & GSW_CTP_PORT_CONFIG_MASK_FORCE)) {
				/*This below field enable indicates that this meter id is
				 already assigned and it's idx recorded for this CTP port id.
				 If next time the user update this meter field with different meter idx !!
				 or wanted to allocate a new meter idx ??!! may happen by mistake ??!!
				 the previous meter idx  must be released from this CTP Port*/

				if (gswdev->ctpportconfig_idx[ctp_port].IngressMeteringAssigned) {
					if (gswdev->ctpportconfig_idx[ctp_port].IngressTrafficMeterId !=
					    param->nIngressTrafficMeterId) {
						idx = gswdev->ctpportconfig_idx[ctp_port].IngressTrafficMeterId;
						/*release the usage of previous meter idx*/
						gswdev->meter_idx[idx].IndexInUsageCnt--;
						gswdev->ctpportconfig_idx[ctp_port].IngressMeteringAssigned = 0;
						gswdev->ctpportconfig_idx[ctp_port].IngressTrafficMeterId = 0;
					}
				}

				/*This Meter ID should be in use (i.e) that is this Meter ID should be allocated
				before calling bridge port configuration
				If not in use return ERROR
				*/
				if (gswdev->meter_idx[param->nIngressTrafficMeterId].IndexInUse) {
					/*Usage count will be incremented only once during meter idx assignment
					for this ctp port id*/
					if (!gswdev->ctpportconfig_idx[ctp_port].IngressMeteringAssigned) {
						gswdev->ctpportconfig_idx[ctp_port].IngressMeteringAssigned = 1;
						gswdev->ctpportconfig_idx[ctp_port].IngressTrafficMeterId
							= param->nIngressTrafficMeterId;
						/*Since this meter id can be shared,Increment the it's usage count*/
						gswdev->meter_idx[param->nIngressTrafficMeterId].IndexInUsageCnt++;
					}
				} else {
					ret = GSW_statusErr;
					goto UNLOCK_AND_RETURN;
				}
			}

			/*Enable Meter Id*/
			tbl_prog_ctpport_ingress.val[3] |= (1 << 7);
			tbl_prog_ctpport_ingress.val[3] &= ~(0x7F << 8);
			tbl_prog_ctpport_ingress.val[3] |= ((param->nIngressTrafficMeterId & 0x7F) << 8);
		} else {
			/*Disable Ingress Meter*/
			tbl_prog_ctpport_ingress.val[3] &= ~(1 << 7);
		}
	}

	if (param->eMask & GSW_CTP_PORT_CONFIG_EGRESS_METER) {
		if (param->bEgressMeteringEnable) {
			if (param->nEgressTrafficMeterId > gswdev->num_of_meters) {
				ret = GSW_statusErr;
				goto UNLOCK_AND_RETURN;
			}

			if (!(param->eMask & GSW_CTP_PORT_CONFIG_MASK_FORCE)) {
				/*This below field enable indicates that this meter id is
				 already assigned and it's idx recorded for this CTP Port id.
				 If next time the user update this meter field with different meter idx !!
				 or wanted to allocate a new meter idx ??!! may happen by mistake ??!!
				 the previous meter idx must be released from this CTP Port*/

				if (gswdev->ctpportconfig_idx[ctp_port].EgressMeteringAssigned) {
					if (gswdev->ctpportconfig_idx[ctp_port].EgressTrafficMeterId !=
					    param->nIngressTrafficMeterId) {
						idx = gswdev->ctpportconfig_idx[ctp_port].EgressTrafficMeterId;
						/*release the usage of previous meter idx*/
						gswdev->meter_idx[idx].IndexInUsageCnt--;
						gswdev->ctpportconfig_idx[ctp_port].EgressMeteringAssigned = 0;
						gswdev->ctpportconfig_idx[ctp_port].EgressTrafficMeterId = 0;
					}
				}

				/*This Meter ID should be in use (i.e) that is this Meter ID should be allocated
				 before calling ctp port configuration
				 If not in use return ERROR
				*/
				if (gswdev->meter_idx[param->nEgressTrafficMeterId].IndexInUse) {
					/*Usage count will be incremented only once during meter idx assignment
					for this bridge port id*/
					if (!gswdev->ctpportconfig_idx[ctp_port].EgressMeteringAssigned) {
						gswdev->ctpportconfig_idx[ctp_port].EgressMeteringAssigned = 1;
						gswdev->ctpportconfig_idx[ctp_port].EgressTrafficMeterId
							= param->nEgressTrafficMeterId;
						/*Since this meter id can be shared,Increment the it's usage count*/
						gswdev->meter_idx[param->nEgressTrafficMeterId].IndexInUsageCnt++;
					}
				} else {
					ret = GSW_statusErr;
					goto UNLOCK_AND_RETURN;
				}
			}

			/*Enable Meter Id*/
			tbl_prog_ctpport_egress.val[3] |= (1 << 7);
			tbl_prog_ctpport_egress.val[3] &= ~(0x7F << 8);
			tbl_prog_ctpport_egress.val[3] |= ((param->nEgressTrafficMeterId & 0x7F) << 8);
		} else {
			/*Disable Egress Meter*/
			tbl_prog_ctpport_egress.val[3] &= ~(1 << 7);
		}
	}

	if (param->eMask & GSW_CTP_PORT_CONFIG_BRIDGING_BYPASS) {
		if (param->bBridgingBypass) {
			/** By pass bridging/multicast processing. Following parameters are used
			  to determine destination. Traffic flow table is not bypassed. */
			/*Enable Bridge Bypass*/
			tbl_prog_ctpport_ingress.val[4] |= (1 << 15);

			/** This field defines destination logical port. */
			tbl_prog_ctpport_ingress.val[4] &= ~(0xF << 8);
			tbl_prog_ctpport_ingress.val[4] |= ((param->nDestLogicalPortId & 0xF) << 8);

			if (param->bPmapperEnable) {
				/*TODO : as per code review - user is not responsible to allocate p-mapper*/
				if (gswdev->ctpportconfig_idx[ctp_port].IngressBridgeBypassPmapperAssigned)
					param->sPmapper.nPmapperId = gswdev->ctpportconfig_idx[ctp_port].IngressBridgeBypassPmappperIdx;
				else
					param->sPmapper.nPmapperId = PMAPPER_ENTRY_INVALID;


				/** When bPmapperEnable is TRUE, this field selects either DSCP or PCP to
				 derive sub interface ID. */
				/*PCP-P-mapper table entry 1-8*/
				/*DSCP-P-mapper table entry 9-72. */
				switch (param->ePmapperMappingMode) {
				case GSW_PMAPPER_MAPPING_PCP:
					tbl_prog_ctpport_ingress.val[4] &= ~(1 << 13);
					break;

				case GSW_PMAPPER_MAPPING_DSCP:
					tbl_prog_ctpport_ingress.val[4] |= (1 << 13);
					break;
				}

				/* If the p-mapper id is invalid then the GSW_QOS_PmapperTableSet will
				   Allocate/program P-mapper index.
				   If p-mapper index is valid the GSW_QOS_PmapperTableSet will just program
				   the table
				 */
				ret = GSW_QOS_PmapperTableSet(cdev, &param->sPmapper);

				if (ret == GSW_statusErr) {
					pr_err("ERROR : GSW_QOS_PmapperTableSet return erro\n");
					goto UNLOCK_AND_RETURN;
				}

				pmapper_idx = param->sPmapper.nPmapperId;

				/*Usage count will be incremented only once during p-mapper idx allocation
				 or assignment for this ctp port id*/
				if (!gswdev->ctpportconfig_idx[ctp_port].IngressBridgeBypassPmapperAssigned) {
					gswdev->ctpportconfig_idx[ctp_port].IngressBridgeBypassPmapperAssigned = 1;
					gswdev->ctpportconfig_idx[ctp_port].IngressBridgeBypassPmappperIdx = pmapper_idx;
				}

				tbl_prog_ctpport_ingress.val[4] |= (1 << 14);
				tbl_prog_ctpport_ingress.val[4] &= ~(0xFF);
				tbl_prog_ctpport_ingress.val[4] |= (pmapper_idx & 0xFF);
			} else {
				/** When bPmapperEnable is FALSE, this field defines destination sub
					interface ID group. */
				/*Disable p-mapper*/
				tbl_prog_ctpport_ingress.val[4] &= ~(1 << 14);
				tbl_prog_ctpport_ingress.val[4] &= ~(0xFF);
				tbl_prog_ctpport_ingress.val[4] |= (param->nDestSubIfIdGroup & 0xFF);
			}
		} else {
			/*Disable Bridge Bypass*/
			tbl_prog_ctpport_ingress.val[4] &= ~(1 << 15);
		}
	}

	if (param->eMask & GSW_CTP_PORT_CONFIG_INGRESS_MARKING) {
		tbl_prog_ctpport_ingress.val[3] &= ~(7);

		switch (param->eIngressMarkingMode) {
		case GSW_MARKING_ALL_GREEN:
			break;

		case GSW_MARKING_INTERNAL_MARKING:
			tbl_prog_ctpport_ingress.val[3] |= 0x1;
			break;

		case GSW_MARKING_DEI:
			tbl_prog_ctpport_ingress.val[3] |= 0x2;
			break;

		case GSW_MARKING_PCP_8P0D:
			tbl_prog_ctpport_ingress.val[3] |= 0x3;
			break;

		case GSW_MARKING_PCP_7P1D:
			tbl_prog_ctpport_ingress.val[3] |= 0x4;
			break;

		case GSW_MARKING_PCP_6P2D:
			tbl_prog_ctpport_ingress.val[3] |= 0x5;
			break;

		case GSW_MARKING_PCP_5P3D:
			tbl_prog_ctpport_ingress.val[3] |= 0x6;
			break;

		case GSW_MARKING_DSCP_AF:
			tbl_prog_ctpport_ingress.val[3] |= 0x7;
			break;
		}
	}

	if (param->eMask & GSW_CTP_PORT_CONFIG_EGRESS_MARKING) {
		tbl_prog_ctpport_ingress.val[3] &= ~(0x70);

		switch (param->eEgressMarkingMode) {
		case GSW_MARKING_ALL_GREEN:
			break;

		case GSW_MARKING_INTERNAL_MARKING:
			tbl_prog_ctpport_ingress.val[3] |= (0x1 << 4);
			break;

		case GSW_MARKING_DEI:
			tbl_prog_ctpport_ingress.val[3] |= (0x2 << 4);
			break;

		case GSW_MARKING_PCP_8P0D:
			tbl_prog_ctpport_ingress.val[3] |= (0x3 << 4);
			break;

		case GSW_MARKING_PCP_7P1D:
			tbl_prog_ctpport_ingress.val[3] |= (0x4 << 4);
			break;

		case GSW_MARKING_PCP_6P2D:
			tbl_prog_ctpport_ingress.val[3] |= (0x5 << 4);
			break;

		case GSW_MARKING_PCP_5P3D:
			tbl_prog_ctpport_ingress.val[3] |= (0x6 << 4);
			break;

		case GSW_MARKING_DSCP_AF:
			tbl_prog_ctpport_ingress.val[3] |= (0x7 << 4);
			break;
		}
	}

	if (param->eMask & GSW_CTP_PORT_CONFIG_EGRESS_REMARKING) {
		tbl_prog_ctpport_egress.val[3] &= ~(0x7);

		switch (param->eEgressRemarkingMode) {
		case GSW_REMARKING_NONE:
			break;

		case GSW_REMARKING_DEI:
			tbl_prog_ctpport_egress.val[3] |= 0x2;
			break;

		case GSW_REMARKING_PCP_8P0D:
			tbl_prog_ctpport_egress.val[3] |= 0x3;
			break;

		case GSW_REMARKING_PCP_7P1D:
			tbl_prog_ctpport_egress.val[3] |= 0x4;
			break;

		case GSW_REMARKING_PCP_6P2D:
			tbl_prog_ctpport_egress.val[3] |= 0x5;
			break;

		case GSW_REMARKING_PCP_5P3D:
			tbl_prog_ctpport_egress.val[3] |= 0x6;
			break;

		case GSW_REMARKING_DSCP_AF:
			tbl_prog_ctpport_egress.val[3] |= 0x7;
			break;
		}
	}

	if (param->eMask & GSW_CTP_PORT_CONFIG_EGRESS_MARKING_OVERRIDE) {
		if (param->bEgressMarkingOverrideEnable) {
			/*Enable OverRide*/
			tbl_prog_ctpport_egress.val[3] |= (1 << 3);
			tbl_prog_ctpport_egress.val[3] &= ~(0x70);

			switch (param->eEgressMarkingModeOverride) {
			case GSW_MARKING_ALL_GREEN:
				break;

			case GSW_MARKING_INTERNAL_MARKING:
				tbl_prog_ctpport_egress.val[3] |= (0x1 << 4);
				break;

			case GSW_MARKING_DEI:
				tbl_prog_ctpport_egress.val[3] |= (0x2 << 4);
				break;

			case GSW_MARKING_PCP_8P0D:
				tbl_prog_ctpport_egress.val[3] |= (0x3 << 4);
				break;

			case GSW_MARKING_PCP_7P1D:
				tbl_prog_ctpport_egress.val[3] |= (0x4 << 4);
				break;

			case GSW_MARKING_PCP_6P2D:
				tbl_prog_ctpport_egress.val[3] |= (0x5 << 4);
				break;

			case GSW_MARKING_PCP_5P3D:
				tbl_prog_ctpport_egress.val[3] |= (0x6 << 4);
				break;

			case GSW_MARKING_DSCP_AF:
				tbl_prog_ctpport_egress.val[3] |= (0x7 << 4);
				break;
			}
		} else {
			/*Disable OverRide*/
			tbl_prog_ctpport_egress.val[3] &= ~(1 << 3);
		}
	}

	if (param->eMask & GSW_CTP_PORT_CONFIG_FLOW_ENTRY) {
		if (param->nFirstFlowEntryIndex > gswdev->tftblsize) {
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}

		/*Check whether this index is InUse
		 *Since it will be marked as InUse during allocation.
		 */
		if (!gswdev->tflow_idx.flow_idx[param->nFirstFlowEntryIndex].indexinuse) {
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}

		/*Check whether this index belongs to this BlockID
		 *Since it will be marked during allocation.
		 */
		if (gswdev->tflow_idx.flow_idx[param->nFirstFlowEntryIndex].tflowblockid
		    != param->nFirstFlowEntryIndex) {
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}

		/*Search for the Last Index of this block.
		 *Note: The Blocks are always allocated contiguously.
		 */
		ctplastidx = param->nFirstFlowEntryIndex;

		while (gswdev->tflow_idx.flow_idx[ctplastidx].tflowblockid ==
			   param->nFirstFlowEntryIndex &&
			   gswdev->tflow_idx.flow_idx[ctplastidx].indexinuse) {
			   ctplastidx++;
		}

		param->nNumberOfFlowEntries = ctplastidx - param->nFirstFlowEntryIndex;
		gswdev->tflow_idx.flow_idx[param->nFirstFlowEntryIndex].indexinuse = 1;
		gswdev->tflow_idx.flow_idx[param->nFirstFlowEntryIndex].indexinusagecnt++;
		tbl_prog_ctpport_ingress.val[7] &= ~(0x7F << 2);
		tbl_prog_ctpport_ingress.val[7] |= (param->nFirstFlowEntryIndex & 0x1FC);
		tbl_prog_ctpport_ingress.val[8] &= ~(0xFF << 2);
		tbl_prog_ctpport_ingress.val[8] |= (param->nNumberOfFlowEntries & 0x3FC);
	}

	if (param->eMask & GSW_CTP_PORT_CONFIG_LOOPBACK_AND_MIRROR) {
		if (param->bIngressLoopbackEnable)
			tbl_prog_ctpport_ingress.val[2] |= (1 << 14);
		else
			tbl_prog_ctpport_ingress.val[2] &= ~(1 << 14);

		if (param->bEgressLoopbackEnable)
			tbl_prog_ctpport_egress.val[4] |= (1 << 12);
		else
			tbl_prog_ctpport_egress.val[4] &= ~(1 << 12);

		if (param->bIngressMirrorEnable)
			tbl_prog_ctpport_ingress.val[5] |= (1 << 15);
		else
			tbl_prog_ctpport_ingress.val[5] &= ~(1 << 15);

		if (param->bEgressMirrorEnable)
			tbl_prog_ctpport_egress.val[5] |= (1 << 15);
		else
			tbl_prog_ctpport_egress.val[5] &= ~(1 << 15);
	}

	if (param->eMask & GSW_CTP_PORT_CONFIG_LOOPBACK_AND_MIRROR) {
		if (param->bIngressDaSaSwapEnable)
			tbl_prog_ctpport_ingress.val[4] |= (1 << 12);
		else
			tbl_prog_ctpport_ingress.val[4] &= ~(1 << 12);

		if (param->bEgressDaSaSwapEnable)
			tbl_prog_ctpport_egress.val[4] |= (1 << 13);
		else
			tbl_prog_ctpport_egress.val[4] &= ~(1 << 13);
	}

	/*Same ctp port idx for ingress and egress ctp port configuration*/
	tbl_prog_ctpport_ingress.table = PCE_IGCTP_INDEX;
	CLEAR_U16(tbl_prog_ctpport_ingress.pcindex);
	/*Table Entry address (ctp port ingress Table index) Bit 8:0 in PCE_TBL_ADDR*/
	tbl_prog_ctpport_ingress.pcindex |= (ctp_port & 0x1FF);

	tbl_prog_ctpport_egress.table = PCE_EGCTP_INDEX;
	CLEAR_U16(tbl_prog_ctpport_egress.pcindex);
	/*Table Entry address (ctp port egress Table index) Bit 8:0 in PCE_TBL_ADDR*/
	tbl_prog_ctpport_egress.pcindex |= (ctp_port & 0x1FF);


	/*Address-based write for ingress ctp port configuration*/
	gsw_pce_table_write(cdev, &tbl_prog_ctpport_ingress);
	/*Address-based write for egress ctp port configuration*/
	gsw_pce_table_write(cdev, &tbl_prog_ctpport_egress);

	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_pce);
#endif
	return ret;
}

GSW_return_t GSW_CtpPortConfigGet(void *cdev, GSW_CTP_portConfig_t *param)
{
	pctbl_prog_t tbl_prog_ctpport_ingress;
	pctbl_prog_t tbl_prog_ctpport_egress;
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 ctp_port, FirstIdx, LastIdx, BlkSize;
	u16 ret, value;
	GSW_CTP_portAssignment_t ctp_get;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_pce);
#endif

	/*check based on number of logical port*/
	if (param->nLogicalPortId > gswdev->tpnum) {
		pr_err("param->nLogicalPortId > gswdev->pnum\n");
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	/*CTP ports has to assigned/mapped to Logical port before calling
	  CTP port configuration.
	  GSW_CTP_PortAssignmentGet will get the info on the assignment
	  Use this info to find whether this CTP port is with in the range
	  this logical port
	*/
	ctp_get.nLogicalPortId = param->nLogicalPortId;
	ret = GSW_CTP_PortAssignmentGet(cdev, &ctp_get);

	if (ret == GSW_statusErr)
		goto UNLOCK_AND_RETURN;

	/*IMPORTANT NOTE : :As per the CTP concept  nFirstCtpPortId + nSubIfIdGroup is the expected CTP port*/
	ctp_port = ctp_get.nFirstCtpPortId + param->nSubIfIdGroup;

	if (ctp_port > gswdev->num_of_ctp) {
		pr_err("ctp_port %d > gswdev->num_of_ctp\n", ctp_port);
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	/*If not with in the Assigned range Return Error*/
	if (ctp_port < ctp_get.nFirstCtpPortId ||
	    ctp_port >= (ctp_get.nFirstCtpPortId + ctp_get.nNumberOfCtpPort)) {
		pr_err("ERROR :CTP port not with in Allocated range\n");
		pr_err("Check CTP assignment Get\n");
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	/*NOTE : This Mask is for debugging purpose only
	  If GSW_CTP_PORT_CONFIG_MASK_FORCE mask is set
	  It will not check index in-use */
	if (!(param->eMask & GSW_CTP_PORT_CONFIG_MASK_FORCE)) {
		/*Check whether CTP is allocated by GSW_CTP_PortAssignmentAlloc*/
		if (!gswdev->ctpportconfig_idx[ctp_port].IndexInUse) {
			pr_err("ctp_port %d  Index not in use,CTP not allocated\n", ctp_port);
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}
	}

	/*Same ctp port idx for ingress and egress ctp port configuration*/
	memset(&tbl_prog_ctpport_ingress, 0, sizeof(pctbl_prog_t));
	tbl_prog_ctpport_ingress.table = PCE_IGCTP_INDEX;
	CLEAR_U16(tbl_prog_ctpport_ingress.pcindex);
	/*Table Entry address (ctp port ingress Table index) Bit 8:0 in PCE_TBL_ADDR*/
	tbl_prog_ctpport_ingress.pcindex |= (ctp_port & 0x1FF);

	memset(&tbl_prog_ctpport_egress, 0, sizeof(pctbl_prog_t));
	tbl_prog_ctpport_egress.table = PCE_EGCTP_INDEX;
	CLEAR_U16(tbl_prog_ctpport_egress.pcindex);
	/*Table Entry address (ctp port egress Table index) Bit 8:0 in PCE_TBL_ADDR*/
	tbl_prog_ctpport_egress.pcindex |= (ctp_port & 0x1FF);

	/*Address-based read for ingress ctp port configuration*/
	gsw_pce_table_read(cdev, &tbl_prog_ctpport_ingress);

	/*Address-based read for egress ctp port configuration*/
	gsw_pce_table_read(cdev, &tbl_prog_ctpport_egress);

	if (param->eMask & GSW_CTP_PORT_CONFIG_MASK_BRIDGE_PORT_ID) {
		param->nBridgePortId = (tbl_prog_ctpport_ingress.val[0] & 0xFF);
	}

	if (param->eMask & GSW_CTP_PORT_CONFIG_MASK_FORCE_TRAFFIC_CLASS) {
		/** Default traffic class associated with all traffic from this CTP Port. */
		param->nDefaultTrafficClass = ((tbl_prog_ctpport_ingress.val[0] & 0xF00) >> 8);
		/* 0 -Traffic class can be over written by the following processing stage
		   1 -Traffic class can not be over written by the following processing stage
		*/
		param->bForcedTrafficClass  = ((tbl_prog_ctpport_ingress.val[0] & 0x1000) >> 12);
	}

	if (param->eMask & GSW_CTP_PORT_CONFIG_MASK_INGRESS_VLAN) {
		param->bIngressExtendedVlanEnable = ((tbl_prog_ctpport_ingress.val[1] & 0x4000) >> 14);

		if (param->bIngressExtendedVlanEnable) {
			param->nIngressExtendedVlanBlockId = (tbl_prog_ctpport_ingress.val[1] & 0x3FF);
			FirstIdx 	= param->nIngressExtendedVlanBlockId;
			LastIdx		= (tbl_prog_ctpport_ingress.val[2] & 0x3FF);

			if (FirstIdx == LastIdx)
				BlkSize	= 1;
			else
				BlkSize = ((LastIdx - FirstIdx) + 1);

			param->nIngressExtendedVlanBlockSize = BlkSize;
		}
	}

	if (param->eMask & GSW_CTP_PORT_CONFIG_MASK_INGRESS_VLAN_IGMP) {
		param->bIngressExtendedVlanIgmpEnable = ((tbl_prog_ctpport_ingress.val[5] & 0x4000) >> 14);

		if (param->bIngressExtendedVlanIgmpEnable) {
			param->nIngressExtendedVlanBlockIdIgmp = (tbl_prog_ctpport_ingress.val[5] & 0x3FF);
			FirstIdx	= param->nIngressExtendedVlanBlockIdIgmp;
			LastIdx 	= (tbl_prog_ctpport_ingress.val[6] & 0x3FF);

			if (FirstIdx == LastIdx)
				BlkSize = 1;
			else
				BlkSize = ((LastIdx - FirstIdx) + 1);

			param->nIngressExtendedVlanBlockSizeIgmp = BlkSize;
		}
	}

	if (param->eMask & GSW_CTP_PORT_CONFIG_MASK_EGRESS_VLAN) {
		param->bEgressExtendedVlanEnable = ((tbl_prog_ctpport_egress.val[1] & 0x4000) >> 14);

		if (param->bEgressExtendedVlanEnable) {
			param->nEgressExtendedVlanBlockId = (tbl_prog_ctpport_egress.val[1] & 0x3FF);
			FirstIdx	= param->nEgressExtendedVlanBlockId;
			LastIdx 	= (tbl_prog_ctpport_egress.val[2]  & 0x3FF);

			if (FirstIdx == LastIdx)
				BlkSize = 1;
			else
				BlkSize = ((LastIdx - FirstIdx) + 1);

			param->nEgressExtendedVlanBlockSize = BlkSize;
		}
	}

	if (param->eMask & GSW_CTP_PORT_CONFIG_MASK_EGRESS_VLAN_IGMP) {
		param->bEgressExtendedVlanIgmpEnable = ((tbl_prog_ctpport_egress.val[5] & 0x4000) >> 14);

		if (param->bEgressExtendedVlanIgmpEnable) {
			param->nEgressExtendedVlanBlockIdIgmp = (tbl_prog_ctpport_egress.val[5] & 0x3FF);
			FirstIdx	= param->nEgressExtendedVlanBlockIdIgmp;
			LastIdx 	= (tbl_prog_ctpport_egress.val[6]  & 0x3FF);

			if (FirstIdx == LastIdx)
				BlkSize = 1;
			else
				BlkSize = ((LastIdx - FirstIdx) + 1);

			param->nEgressExtendedVlanBlockSizeIgmp = BlkSize;
		}
	}

	if (param->eMask & GSW_CTP_PORT_CONFIG_MASK_INRESS_NTO1_VLAN)
		param->bIngressNto1VlanEnable = ((tbl_prog_ctpport_ingress.val[1] & 0x8000) >> 15);

	if (param->eMask & GSW_CTP_PORT_CONFIG_MASK_EGRESS_NTO1_VLAN)
		param->bEgressNto1VlanEnable = ((tbl_prog_ctpport_egress.val[1] & 0x8000) >> 15);

	if (param->eMask & GSW_CTP_PORT_CONFIG_INGRESS_METER) {
		param->bIngressMeteringEnable = ((tbl_prog_ctpport_ingress.val[3] & 0x80) >> 7);

		if (param->bIngressMeteringEnable)
			param->nIngressTrafficMeterId = ((tbl_prog_ctpport_ingress.val[3] & 0x7F00) >> 8);
	}

	if (param->eMask & GSW_CTP_PORT_CONFIG_EGRESS_METER) {
		param->bEgressMeteringEnable = ((tbl_prog_ctpport_egress.val[3] & 0x80) >> 7);

		if (param->bEgressMeteringEnable)
			param->nEgressTrafficMeterId = ((tbl_prog_ctpport_egress.val[3] & 0x7F00) >> 8);
	}

	if (param->eMask & GSW_CTP_PORT_CONFIG_BRIDGING_BYPASS) {
		param->bBridgingBypass = ((tbl_prog_ctpport_ingress.val[4] & 0x8000) >> 15);

		if (param->bBridgingBypass) {
			param->nDestLogicalPortId = ((tbl_prog_ctpport_ingress.val[4] & 0xF00) >> 8);
			param->bPmapperEnable = ((tbl_prog_ctpport_ingress.val[4] & 0x4000) >> 14);

			if (param->bPmapperEnable) {
				param->sPmapper.nPmapperId = (tbl_prog_ctpport_ingress.val[4] & 0xFF);
				ret = GSW_QOS_PmapperTableGet(cdev, &param->sPmapper);

				if (ret == GSW_statusErr)
					goto UNLOCK_AND_RETURN;

				value = ((tbl_prog_ctpport_ingress.val[4] & 0x2000) >> 13);

				switch (value) {
				case 0:
					param->ePmapperMappingMode = GSW_PMAPPER_MAPPING_PCP;
					break;

				case 1:
					param->ePmapperMappingMode = GSW_PMAPPER_MAPPING_DSCP;
					break;
				}
			} else {
				param->nDestSubIfIdGroup = (tbl_prog_ctpport_ingress.val[4] & 0xFF);
			}
		}
	}

	if (param->eMask & GSW_CTP_PORT_CONFIG_INGRESS_MARKING) {
		value = (tbl_prog_ctpport_ingress.val[3] & 0x7);

		switch (value) {
		case 0:
			param->eIngressMarkingMode = GSW_MARKING_ALL_GREEN;
			break;

		case 1:
			param->eIngressMarkingMode = GSW_MARKING_INTERNAL_MARKING;
			break;

		case 2:
			param->eIngressMarkingMode = GSW_MARKING_DEI;
			break;

		case 3:
			param->eIngressMarkingMode = GSW_MARKING_PCP_8P0D;
			break;

		case 4:
			param->eIngressMarkingMode = GSW_MARKING_PCP_7P1D;
			break;

		case 5:
			param->eIngressMarkingMode = GSW_MARKING_PCP_6P2D;
			break;

		case 6:
			param->eIngressMarkingMode = GSW_MARKING_PCP_5P3D;
			break;

		case 7:
			param->eIngressMarkingMode = GSW_MARKING_DSCP_AF;
			break;
		}
	}

	if (param->eMask & GSW_CTP_PORT_CONFIG_EGRESS_MARKING) {
		value = ((tbl_prog_ctpport_ingress.val[3] & 0x70) >> 4);

		switch (value) {
		case 0:
			param->eEgressMarkingMode = GSW_MARKING_ALL_GREEN;
			break;

		case 1:
			param->eEgressMarkingMode = GSW_MARKING_INTERNAL_MARKING;
			break;

		case 2:
			param->eEgressMarkingMode = GSW_MARKING_DEI;
			break;

		case 3:
			param->eEgressMarkingMode = GSW_MARKING_PCP_8P0D;
			break;

		case 4:
			param->eEgressMarkingMode = GSW_MARKING_PCP_7P1D;
			break;

		case 5:
			param->eEgressMarkingMode = GSW_MARKING_PCP_6P2D;
			break;

		case 6:
			param->eEgressMarkingMode = GSW_MARKING_PCP_5P3D;
			break;

		case 7:
			param->eEgressMarkingMode = GSW_MARKING_DSCP_AF;
			break;
		}
	}

	if (param->eMask & GSW_CTP_PORT_CONFIG_EGRESS_REMARKING) {
		value = (tbl_prog_ctpport_egress.val[3] & 0x7);

		switch (value) {
		case 0:
		case 1:
			param->eEgressRemarkingMode = GSW_REMARKING_NONE;
			break;

		case 2:
			param->eEgressRemarkingMode = GSW_REMARKING_DEI;
			break;

		case 3:
			param->eEgressRemarkingMode = GSW_REMARKING_PCP_8P0D;
			break;

		case 4:
			param->eEgressRemarkingMode = GSW_REMARKING_PCP_7P1D;
			break;

		case 5:
			param->eEgressRemarkingMode = GSW_REMARKING_PCP_6P2D;
			break;

		case 6:
			param->eEgressRemarkingMode = GSW_REMARKING_PCP_5P3D;
			break;

		case 7:
			param->eEgressRemarkingMode = GSW_REMARKING_DSCP_AF;
			break;
		}
	}

	if (param->eMask & GSW_CTP_PORT_CONFIG_EGRESS_MARKING_OVERRIDE) {
		param->bEgressMarkingOverrideEnable = ((tbl_prog_ctpport_egress.val[3] & 0x8) >> 3);

		if (param->bEgressMarkingOverrideEnable) {
			value = ((tbl_prog_ctpport_egress.val[3] & 0x70) >> 4);

			switch (value) {
			case 0:
				param->eEgressMarkingModeOverride = GSW_MARKING_ALL_GREEN;
				break;

			case 1:
				param->eEgressMarkingModeOverride = GSW_MARKING_INTERNAL_MARKING;
				break;

			case 2:
				param->eEgressMarkingModeOverride = GSW_MARKING_DEI;
				break;

			case 3:
				param->eEgressMarkingModeOverride = GSW_MARKING_PCP_8P0D;
				break;

			case 4:
				param->eEgressMarkingModeOverride = GSW_MARKING_PCP_7P1D;
				break;

			case 5:
				param->eEgressMarkingModeOverride = GSW_MARKING_PCP_6P2D;
				break;

			case 6:
				param->eEgressMarkingModeOverride = GSW_MARKING_PCP_5P3D;
				break;

			case 7:
				param->eEgressMarkingModeOverride = GSW_MARKING_DSCP_AF;
				break;
			}
		}
	}

	if (param->eMask & GSW_CTP_PORT_CONFIG_FLOW_ENTRY) {
		param->nFirstFlowEntryIndex = (tbl_prog_ctpport_ingress.val[7] & 0x1FC);
		param->nNumberOfFlowEntries = (tbl_prog_ctpport_ingress.val[8] & 0x3FC);
	}

	if (param->eMask & GSW_CTP_PORT_CONFIG_LOOPBACK_AND_MIRROR) {
		param->bIngressLoopbackEnable = ((tbl_prog_ctpport_ingress.val[2] & 0x4000) >> 14);
		param->bEgressLoopbackEnable = ((tbl_prog_ctpport_egress.val[4] & 0x1000) >> 12);
		param->bIngressMirrorEnable = ((tbl_prog_ctpport_ingress.val[5] & 0x8000) >> 15);
		param->bEgressMirrorEnable = ((tbl_prog_ctpport_egress.val[5] & 0x8000) >> 15);
	}

	if (param->eMask & GSW_CTP_PORT_CONFIG_LOOPBACK_AND_MIRROR) {
		param->bIngressDaSaSwapEnable = ((tbl_prog_ctpport_ingress.val[4] & 0x1000) >> 12);
		param->bEgressDaSaSwapEnable = ((tbl_prog_ctpport_egress.val[4] & 0x2000) >> 13);
	}

	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_pce);
#endif
	return ret;
}

GSW_return_t GSW_CtpPortConfigReset(void *cdev, GSW_CTP_portConfig_t *param)
{
	pctbl_prog_t tbl_prog_ctpport_ingress;
	pctbl_prog_t tbl_prog_ctpport_egress;
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 FirstIdx, ctp_port;
	u16 ret;
	GSW_CTP_portAssignment_t ctp_get;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_pce);
#endif

	/*check based on number of logical port*/
	if (param->nLogicalPortId >= gswdev->tpnum) {
		pr_err("param->nLogicalPortId > gswdev->pnum\n");
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	/*CTP ports has to assigned/mapped to Logical port before calling
	  CTP port configuration.
	  GSW_CTP_PortAssignmentGet will get the info on the assignment
	  Use this info to find whether this CTP port is with in the range
	  this logical port
	*/
	ctp_get.nLogicalPortId = param->nLogicalPortId;
	ret = GSW_CTP_PortAssignmentGet(cdev, &ctp_get);

	if (ret == GSW_statusErr)
		goto UNLOCK_AND_RETURN;

	/*IMPORTANT NOTE : :As per the CTP concept  nFirstCtpPortId + nSubIfIdGroup is the expected CTP port*/
	ctp_port = ctp_get.nFirstCtpPortId + param->nSubIfIdGroup;

	if (ctp_port > gswdev->num_of_ctp) {
		pr_err("ctp_port %d > gswdev->num_of_ctp\n", ctp_port);
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	/*If not with in the Assigned range Return Error*/
	if (ctp_port < ctp_get.nFirstCtpPortId ||
	    ctp_port >= (ctp_get.nFirstCtpPortId + ctp_get.nNumberOfCtpPort)) {
		pr_err("ERROR :CTP port not with in Allocated range\n");
		pr_err("Check CTP assignment Get\n");
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	/*This Mask is for DEBUGGING purpose only
	  If GSW_CTP_PORT_CONFIG_MASK_FORCE mask is set
	  It not check index to in-use*/
	if (!(param->eMask & GSW_CTP_PORT_CONFIG_MASK_FORCE)) {
		/*Check whether CTP is allocated by GSW_CTP_PortAssignmentAlloc*/
		if (!gswdev->ctpportconfig_idx[ctp_port].IndexInUse) {
			pr_err("ERROR :ctp_port %d  Index not in use,CTP not allocated\n", ctp_port);
			pr_err("ERROR :ctp_port %d  Index can not be deleted", ctp_port);
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}

	}

	/*Some one is still using this CTP port -configuration
	  can be deleted only if that some one releases this CTP port*/
	if (gswdev->ctpportconfig_idx[ctp_port].IndexInUsageCnt) {
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	/*Same ctp port idx for ingress and egress ctp port configuration*/
	memset(&tbl_prog_ctpport_ingress, 0, sizeof(pctbl_prog_t));
	tbl_prog_ctpport_ingress.table = PCE_IGCTP_INDEX;
	CLEAR_U16(tbl_prog_ctpport_ingress.pcindex);
	/*Table Entry address (ctp port ingress Table index) Bit 8:0 in PCE_TBL_ADDR*/
	tbl_prog_ctpport_ingress.pcindex |= (ctp_port & 0x1FF);

	memset(&tbl_prog_ctpport_egress, 0, sizeof(pctbl_prog_t));
	tbl_prog_ctpport_egress.table = PCE_EGCTP_INDEX;
	CLEAR_U16(tbl_prog_ctpport_egress.pcindex);
	/*Table Entry address (ctp port egress Table index) Bit 8:0 in PCE_TBL_ADDR*/
	tbl_prog_ctpport_egress.pcindex |= (ctp_port & 0x1FF);

	/*Address-based read for ingress ctp port configuration*/
	gsw_pce_table_read(cdev, &tbl_prog_ctpport_ingress);

	/*Address-based read for egress ctp port configuration*/
	gsw_pce_table_read(cdev, &tbl_prog_ctpport_egress);

	param->nBridgePortId = (tbl_prog_ctpport_ingress.val[0] & 0xFF);

	if (gswdev->ctpportconfig_idx[ctp_port].BrdgIdPortAssigned) {
		if (gswdev->ctpportconfig_idx[ctp_port].BrdgPortId ==
		    param->nBridgePortId) {
			/*Release this Bridge port from this CTP Port*/
			gswdev->brdgeportconfig_idx[param->nBridgePortId].IndexInUsageCnt--;
			gswdev->ctpportconfig_idx[ctp_port].BrdgIdPortAssigned = 0;
			gswdev->ctpportconfig_idx[ctp_port].BrdgPortId = 0;
		} else {
			/*if assigned and idx does not match with recorded idx.
			 some thing is wrong.This should never happen*/
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}
	}

	param->nIngressExtendedVlanBlockId = (tbl_prog_ctpport_ingress.val[1] & 0x3FF);

	if (gswdev->ctpportconfig_idx[ctp_port].IngressExVlanNonIgmpBlkAssigned) {
		if (gswdev->ctpportconfig_idx[ctp_port].IngressExVlanNonIgmpBlkId ==
		    param->nIngressExtendedVlanBlockId) {
			/*Release this vlan blk from this CTP Port*/
			FirstIdx = param->nIngressExtendedVlanBlockId;
			gswdev->extendvlan_idx.vlan_idx[FirstIdx].IndexInUsageCnt--;
			gswdev->ctpportconfig_idx[ctp_port].IngressExVlanNonIgmpBlkAssigned = 0;
			gswdev->ctpportconfig_idx[ctp_port].IngressExVlanNonIgmpBlkId = EXVLAN_ENTRY_INVALID;
		} else {
			/*if assigned and idx does not match with recorded idx.
			 some thing is wrong.This should never happen*/
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}
	}

	param->nIngressExtendedVlanBlockIdIgmp = (tbl_prog_ctpport_ingress.val[5] & 0x3FF);

	if (gswdev->ctpportconfig_idx[ctp_port].IngressExVlanIgmpBlkAssigned) {
		if (gswdev->ctpportconfig_idx[ctp_port].IngressExVlanIgmpBlkId ==
		    param->nIngressExtendedVlanBlockIdIgmp) {
			/*Release this vlan blk from this CTP Port*/
			FirstIdx = param->nIngressExtendedVlanBlockIdIgmp;
			gswdev->extendvlan_idx.vlan_idx[FirstIdx].IndexInUsageCnt--;
			gswdev->ctpportconfig_idx[ctp_port].IngressExVlanIgmpBlkAssigned = 0;
			gswdev->ctpportconfig_idx[ctp_port].IngressExVlanIgmpBlkId = EXVLAN_ENTRY_INVALID;
		} else {
			/*if assigned and idx does not match with recorded idx.
			 some thing is wrong.This should never happen*/
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}
	}

	param->nEgressExtendedVlanBlockId = (tbl_prog_ctpport_egress.val[1] & 0x3FF);

	if (gswdev->ctpportconfig_idx[ctp_port].EgressExVlanNonIgmpBlkAssigned) {
		if (gswdev->ctpportconfig_idx[ctp_port].EgressExVlanNonIgmpBlkId ==
		    param->nEgressExtendedVlanBlockId) {
			/*Release this vlan blk from this CTP Port*/
			FirstIdx = param->nEgressExtendedVlanBlockId;
			gswdev->extendvlan_idx.vlan_idx[FirstIdx].IndexInUsageCnt--;
			gswdev->ctpportconfig_idx[ctp_port].EgressExVlanNonIgmpBlkAssigned = 0;
			gswdev->ctpportconfig_idx[ctp_port].EgressExVlanNonIgmpBlkId = EXVLAN_ENTRY_INVALID;
		} else {
			/*if assigned and idx does not match with recorded idx.
			 some thing is wrong.This should never happen*/
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}
	}

	param->nEgressExtendedVlanBlockIdIgmp = (tbl_prog_ctpport_egress.val[5] & 0x3FF);

	if (gswdev->ctpportconfig_idx[ctp_port].EgressExVlanIgmpBlkAssigned) {
		if (gswdev->ctpportconfig_idx[ctp_port].EgressExVlanIgmpBlkId ==
		    param->nEgressExtendedVlanBlockIdIgmp) {
			/*Release this vlan blk from this CTP Port*/
			FirstIdx = param->nEgressExtendedVlanBlockIdIgmp;
			gswdev->extendvlan_idx.vlan_idx[FirstIdx].IndexInUsageCnt--;
			gswdev->ctpportconfig_idx[ctp_port].EgressExVlanIgmpBlkAssigned = 0;
			gswdev->ctpportconfig_idx[ctp_port].EgressExVlanIgmpBlkId = EXVLAN_ENTRY_INVALID;
		} else {
			/*if assigned and idx does not match with recorded idx.
			 some thing is wrong.This should never happen*/
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}
	}

	param->nIngressTrafficMeterId = ((tbl_prog_ctpport_ingress.val[3] & 0x7F00) >> 8);

	if (gswdev->ctpportconfig_idx[ctp_port].IngressMeteringAssigned) {
		if (gswdev->ctpportconfig_idx[ctp_port].IngressTrafficMeterId ==
		    param->nIngressTrafficMeterId) {
			/*Release this meter id from this CTP Port*/
			gswdev->meter_idx[param->nIngressTrafficMeterId].IndexInUsageCnt--;
			gswdev->ctpportconfig_idx[ctp_port].IngressMeteringAssigned = 0;
			gswdev->ctpportconfig_idx[ctp_port].IngressTrafficMeterId = 0;

		} else {
			/*if assigned and idx does not match with recorded idx.
			 some thing is wrong.This should never happen*/
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}
	}

	param->nEgressTrafficMeterId = ((tbl_prog_ctpport_egress.val[3] & 0x7F00) >> 8);

	if (gswdev->ctpportconfig_idx[ctp_port].EgressMeteringAssigned) {
		if (gswdev->ctpportconfig_idx[ctp_port].EgressTrafficMeterId ==
		    param->nEgressTrafficMeterId) {
			/*Release this meter id from this CTP Port*/
			gswdev->meter_idx[param->nEgressTrafficMeterId].IndexInUsageCnt--;
			gswdev->ctpportconfig_idx[ctp_port].EgressMeteringAssigned = 0;
			gswdev->ctpportconfig_idx[ctp_port].EgressTrafficMeterId = 0;

		} else {
			/*if assigned and idx does not match with recorded idx.
			 some thing is wrong.This should never happen*/
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}
	}

	param->sPmapper.nPmapperId = (tbl_prog_ctpport_ingress.val[4] & 0xFF);

	if (gswdev->ctpportconfig_idx[ctp_port].IngressBridgeBypassPmapperAssigned) {
		/*Release this p-mapper idx from this CTP Port*/
		gswdev->pmapper_idx[param->sPmapper.nPmapperId].IndexInUse = 0;
		gswdev->ctpportconfig_idx[ctp_port].IngressBridgeBypassPmapperAssigned = 0;
		gswdev->ctpportconfig_idx[ctp_port].IngressBridgeBypassPmappperIdx = 0;
	}

	param->nFirstFlowEntryIndex = (tbl_prog_ctpport_ingress.val[7] & 0x1FC);
	param->nNumberOfFlowEntries = (tbl_prog_ctpport_ingress.val[8] & 0x3FC);

	if (param->nNumberOfFlowEntries)
		/*Release this TFLOW blk from this CTP Port*/
		gswdev->tflow_idx.flow_idx[param->nFirstFlowEntryIndex].indexinusagecnt--;

	/*Zero the ingress/egress ctp port table index*/
	/*Same ctp port idx for ingress and egress ctp port configuration*/
	memset(&tbl_prog_ctpport_ingress, 0, sizeof(pctbl_prog_t));
	tbl_prog_ctpport_ingress.table = PCE_IGCTP_INDEX;
	CLEAR_U16(tbl_prog_ctpport_ingress.pcindex);
	/*Table Entry address (ctp port ingress Table index) Bit 8:0 in PCE_TBL_ADDR*/
	tbl_prog_ctpport_ingress.pcindex |= (ctp_port & 0x1FF);

	/*Set Bridge port 127 - The ingress traffic is never assigned to port 127.
	The packet may have egress bridge port ID 127 if the traffic is ingress loopback traffic,
	bridge bypass traffic, or mirror copy (either ingress copyor egress copy).*/
	tbl_prog_ctpport_ingress.val[0] |= (0x7F);

	memset(&tbl_prog_ctpport_egress, 0, sizeof(pctbl_prog_t));
	tbl_prog_ctpport_egress.table = PCE_EGCTP_INDEX;
	CLEAR_U16(tbl_prog_ctpport_egress.pcindex);
	/*Table Entry address (ctp port egress Table index) Bit 8:0 in PCE_TBL_ADDR*/
	tbl_prog_ctpport_egress.pcindex |= (ctp_port & 0x1FF);

	/*Address-based write for ingress ctp port configuration*/
	gsw_pce_table_write(cdev, &tbl_prog_ctpport_ingress);
	/*Address-based write for egress ctp port configuration*/
	gsw_pce_table_write(cdev, &tbl_prog_ctpport_egress);

	/*Free this ctp port idx*/
	//gswdev->ctpportconfig_idx[ctp_port].IndexInUse=0;

	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_pce);
#endif
	return ret;
}

GSW_return_t GSW_DefaultMacFilterSet(void *cdev, GSW_MACFILTER_default_t *param)
{
	u16 i = 0;
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_misc);
#endif

	/*   Each bit stands for 1 bridge port. For PRX300 (GSWIP-3.1 integrated),
	   only index 0-7 is valid. */
	switch (param->eType) {
	case GSW_MACFILTERTYPE_SRC:
		for (i = 0; i <= 7; i++) {
			gsw_w32(cdev, PCE_SA_FILTER_OFFSET_GET(i),
				PCE_SA_FILTER_SHIFT,
				PCE_SA_FILTER_SIZE, param->nPortmap[i]);
		}

		break;

	case GSW_MACFILTERTYPE_DEST:
		for (i = 0; i <= 7; i++) {
			gsw_w32(cdev, PCE_DA_FILTER_OFFSET_GET(i),
				PCE_DA_FILTER_SHIFT,
				PCE_DA_FILTER_SIZE, param->nPortmap[i]);
		}

		break;
	}

	ret = GSW_statusOk;

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_misc);
#endif
	return ret;
}

GSW_return_t GSW_DefaultMacFilterGet(void *cdev, GSW_MACFILTER_default_t *param)
{
	u32 i = 0, value = 0;
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_misc);
#endif

	/*   Each bit stands for 1 bridge port. For PRX300 (GSWIP-3.1 integrated),
	   only index 0-7 is valid. */
	switch (param->eType) {
	case GSW_MACFILTERTYPE_SRC:
		for (i = 0; i <= 7; i++) {
			gsw_r32(cdev, PCE_SA_FILTER_OFFSET_GET(i),
				PCE_SA_FILTER_SHIFT,
				PCE_SA_FILTER_SIZE, &value);
			param->nPortmap[i] = value;
		}

		break;

	case GSW_MACFILTERTYPE_DEST:
		for (i = 0; i <= 7; i++) {
			gsw_r32(cdev, PCE_DA_FILTER_OFFSET_GET(i),
				PCE_DA_FILTER_SHIFT,
				PCE_DA_FILTER_SIZE, &value);
			param->nPortmap[i] = value;
		}

		break;
	}

	ret = GSW_statusOk;

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_misc);
#endif
	return ret;

}

GSW_return_t GSW_QOS_MeterAlloc(void *cdev, GSW_QoS_meterCfg_t *param)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u16 idx = 0, freeidxfound = 0, ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_misc);
#endif

	/*Find free meter indexand allocate*/
	for (idx = 0; idx < gswdev->num_of_meters && !freeidxfound; idx++) {
		if (!(gswdev->meter_idx[idx].IndexInUse)) {
			gswdev->meter_idx[idx].IndexInUse = 1;
			param->nMeterId = idx;
			freeidxfound = 1;
		}
	}

	/*No free Slot return Error*/
	if (!freeidxfound) {
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	if (param->nMeterId >= gswdev->num_of_meters) {
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	/*Check whether it is InUSE,if not InUse,return ERROR*/
	if (!(gswdev->meter_idx[param->nMeterId].IndexInUse)) {
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_misc);
#endif
	return ret;
}

GSW_return_t GSW_QOS_MeterFree(void *cdev, GSW_QoS_meterCfg_t *param)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_misc);
#endif

	if (param->nMeterId >= gswdev->num_of_meters) {
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	if (param->nMeterId == METER_ENTRY_INVALID) {
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	/*If Meter ID is valid,Check whether it is InUSE
	  if not InUse,return ERROR*/
	if (!gswdev->meter_idx[param->nMeterId].IndexInUse) {
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	/*If this Meter idx usage count is not zero that means it is still used by some one.
	  This Meter idx can be freed, only if that some one release this Meter ID*/
	if (gswdev->meter_idx[param->nMeterId].IndexInUsageCnt) {
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	/*Just mark the Meter idx as free*/
	gswdev->meter_idx[param->nMeterId].IndexInUse = 0;
	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_misc);
#endif
	return ret;
}

GSW_return_t GSW_RMON_ExtendGet(void *cdev, GSW_RMON_extendGet_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u8 pidx = parm->nPortId, i;
	u32 value, data0, data1;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_bm);
#endif

	if (IS_VRSN_31_OR_32(gswdev->gipver)) {
		/* GSW_RMON_ExtendGet will not support above 3.0 */
		ret = GSW_statusNoSupport;
		goto UNLOCK_AND_RETURN;
	}

	if (parm->nPortId >= gswdev->tpnum) {
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}


	memset(parm, 0, sizeof(GSW_RMON_extendGet_t));

	for (i = 0; i < GSW_RMON_EXTEND_NUM; i++) {
		gsw_w32(cdev, BM_RAM_ADDR_ADDR_OFFSET,
			BM_RAM_ADDR_ADDR_SHIFT,
			BM_RAM_ADDR_ADDR_SIZE,
			(i + REX_TFLOW_CNT_1));

		if (gswdev->gipver == LTQ_GSWIP_3_0) {
			if (pidx >= 8)
				value = (pidx + 8);
			else
				value = pidx;

			gsw_w32(cdev, BM_RAM_CTRL_ADDR_OFFSET,
				BM_RAM_CTRL_ADDR_SHIFT,
				BM_RAM_CTRL_ADDR_SIZE, value);
		} else {
			gsw_w32(cdev, BM_RAM_CTRL_ADDR_OFFSET,
				BM_RAM_CTRL_ADDR_SHIFT,
				BM_RAM_CTRL_ADDR_SIZE, pidx);
		}

		gsw_w32(cdev, BM_RAM_CTRL_OPMOD_OFFSET,
			BM_RAM_CTRL_OPMOD_SHIFT,
			BM_RAM_CTRL_OPMOD_SIZE, 0);
		value = 1;
		gsw_w32(cdev, BM_RAM_CTRL_BAS_OFFSET,
			BM_RAM_CTRL_BAS_SHIFT,
			BM_RAM_CTRL_BAS_SIZE, value);
		CHECK_BUSY(BM_RAM_CTRL_BAS_OFFSET, BM_RAM_CTRL_BAS_SHIFT,
			   BM_RAM_CTRL_BAS_SHIFT, RETURN_FROM_FUNCTION);
		gsw_r32(cdev, BM_RAM_VAL_0_VAL0_OFFSET,
			BM_RAM_VAL_0_VAL0_SHIFT,
			BM_RAM_VAL_0_VAL0_SIZE, &data0);
		gsw_r32(cdev, BM_RAM_VAL_1_VAL1_OFFSET,
			BM_RAM_VAL_1_VAL1_SHIFT,
			BM_RAM_VAL_1_VAL1_SIZE, &data1);
		parm->nTrafficFlowCnt[i] = (data1 << 16 | data0);
	}

	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_bm);
#endif
	return ret;
}

GSW_return_t GSW_RMON_FlowGet(void *cdev, GSW_RMON_flowGet_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 data0, data1, data2, data3, counterMsb = 0, counterLsb = 0;
	u32 tflowIndex = 0, ctrlRegData = 0, addrRegData = 0;
	u8 portType, nBits;  //Number of RMON valid bits.
	GSW_TflowCmodeConf_t cMode;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

	//Given absolute couter index to read.
	if (parm->bIndex)
		tflowIndex = parm->nIndex;
	else {
		//Compute absolute couter index to read.
		memset(&cMode, 0, sizeof(GSW_TflowCmodeConf_t));
		GSW_TflowCountModeGet(cdev, &cMode);

		if (cMode.eCountMode == GSW_TFLOW_CMODE_LOGICAL)
			nBits = 5;
		else
			nBits = (cMode.nBrpLsb | cMode.nCtpLsb); //Only one of is valiad.

		//Is the given port is among configured MSB port group?
		if ((parm->nPortId >> (9 - nBits)) == cMode.nPortMsb) {
			//LS (9-nBits) bits of given port become MS bits of counter index to read.
			counterMsb = ((parm->nPortId) & ((1 << (9 - nBits)) - 1));
			//LS (nBits) bits of given RMON-Id become LS bits of counter index to read.
			counterLsb = ((parm->nFlowId) & ((1 << nBits) - 1));
			//Absolute counter index to read is.
			tflowIndex = ((counterMsb << nBits) | counterLsb);
			parm->nIndex = tflowIndex;
		} else {
			pr_err("ERR:nPortId's MS %d bits are not matching the group %d\n",
			       (9 - (nBits - 1)), cMode.nPortMsb);
			ret = GSW_statusErr;
			return ret;
		}
	}

	//Validate absolute couter index.
	if (tflowIndex >= PCE_TABLE_SIZE) { //Max index check.
		ret = GSW_statusErr;
		return ret;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_bm);
#endif

	//1.Get PCE Rx counters.
	portType = GSW_RMON_TFLOW_RX;
	//Populate 'data' register's Port-Offset.
	FILL_CTRL_REG(addrRegData, BM_RAM_ADDR_RMON_PORT_SHIFT, tflowIndex);
	//Populate 'data' register's Counter-Offset.
	//FILL_CTRL_REG(addrRegData, BM_RAM_ADDR_RMON_COUNTER_SHIFT, 0);
	//Populate 'control' register data.
	//RMON TFLOW Rx table to read.
	FILL_CTRL_REG(ctrlRegData, BM_RAM_CTRL_ADDR_SHIFT, portType);
	//RAM in read mode.
	//FILL_CTRL_REG(ctrlRegData, BM_RAM_CTRL_OPMOD_SHIFT, 0);
	//Set access busy bit.
	FILL_CTRL_REG(ctrlRegData, BM_RAM_CTRL_BAS_SHIFT, 1);
	//Set RAM address to read.
	gsw_w32_raw(cdev, BM_RAM_ADDR_REG_OFFSET, addrRegData);
	//Set RAM table to read.
	gsw_w32_raw(cdev, BM_RAM_CTRL_REG_OFFSET, ctrlRegData);
	//Wait untill RAM is ready to read.
	CHECK_BUSY(BM_RAM_CTRL_BAS_OFFSET, BM_RAM_CTRL_BAS_SHIFT,
		   BM_RAM_CTRL_BAS_SIZE, RETURN_FROM_FUNCTION);
	gsw_r32_raw(cdev, BM_RAM_VAL_0_VAL0_OFFSET, &data0);
	gsw_r32_raw(cdev, BM_RAM_VAL_1_VAL1_OFFSET, &data1);
	parm->nRxPkts = (data1 << 16 | data0);

	//2.Get PCE Tx counters.
	portType = GSW_RMON_TFLOW_TX;
	//RMON TFLOW Tx table to read.
	CLEAR_FILL_CTRL_REG(ctrlRegData, BM_RAM_CTRL_ADDR_SHIFT,
			    BM_RAM_CTRL_ADDR_SIZE, portType);
	//64 bit counter mode. It is not supported in HW at this moment.
	//FILL_CTRL_REG(ctrlRegData, BM_RAM_CTRL_64BIT_OPMOD_SHIFT, 1);
	//Set RAM address to read.
	gsw_w32_raw(cdev, BM_RAM_ADDR_REG_OFFSET, addrRegData);
	//Set RAM table to read.
	gsw_w32_raw(cdev, BM_RAM_CTRL_REG_OFFSET, ctrlRegData);
	//Wait untill RAM is ready to read.
	CHECK_BUSY(BM_RAM_CTRL_BAS_OFFSET, BM_RAM_CTRL_BAS_SHIFT,
		   BM_RAM_CTRL_BAS_SIZE, RETURN_FROM_FUNCTION);
	gsw_r32_raw(cdev, BM_RAM_VAL_0_VAL0_OFFSET, &data0);
	gsw_r32_raw(cdev, BM_RAM_VAL_1_VAL1_OFFSET, &data1);
	parm->nTxPkts = (data1 << 16 | data0);
	//3.Get PCE-Bypass Tx counters.
	//Set RAM address to read.
	addrRegData = addrRegData + 1;
	gsw_w32_raw(cdev, BM_RAM_ADDR_REG_OFFSET, addrRegData);
	//Set RAM table to read.
	gsw_w32_raw(cdev, BM_RAM_CTRL_REG_OFFSET, ctrlRegData);
	//Wait untill RAM is ready to read.
	CHECK_BUSY(BM_RAM_CTRL_BAS_OFFSET, BM_RAM_CTRL_BAS_SHIFT,
		   BM_RAM_CTRL_BAS_SIZE, RETURN_FROM_FUNCTION);
	gsw_r32_raw(cdev, BM_RAM_VAL_0_VAL0_OFFSET, &data2);
	gsw_r32_raw(cdev, BM_RAM_VAL_1_VAL1_OFFSET, &data3);
	parm->nTxPceBypassPkts = (data3 << 16 | data2);

	ret = GSW_statusOk;

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_bm);
#endif
	return ret;
}

GSW_return_t GSW_RmonTflowClear(void *cdev, GSW_RMON_flowGet_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 ctrlRegData = 0, addrRegData = 0, tflowIndex = 0;
	u32 counterMsb = 0, counterLsb = 0;
	u8 portType, nBits;  //Number of RMON valid bits.
	GSW_TflowCmodeConf_t cMode;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d\n", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_bm);
#endif

	//Given absolute couter index to read.
	if (parm->bIndex)
		tflowIndex = parm->nIndex;
	else {
		//Compute absolute couter index to read.
		memset(&cMode, 0, sizeof(GSW_TflowCmodeConf_t));
		GSW_TflowCountModeGet(cdev, &cMode);

		if (cMode.eCountMode == GSW_TFLOW_CMODE_LOGICAL)
			nBits = 5;
		else
			nBits = (cMode.nBrpLsb | cMode.nCtpLsb); //Only one of is valiad.

		//Is the given port is among configured MSB port group?
		if ((parm->nPortId >> (9 - nBits)) == cMode.nPortMsb) {
			//LS (9-nBits) bits of given port become MS bits of counter index to read.
			counterMsb = ((parm->nPortId) & ((1 << (9 - nBits)) - 1));
			//LS (nBits) bits of given RMON-Id become LS bits of counter index to read.
			counterLsb = ((parm->nFlowId) & ((1 << nBits) - 1));
			//Absolute counter index to read is.
			tflowIndex = ((counterMsb << nBits) | counterLsb);
			parm->nIndex = tflowIndex;
		} else {
			pr_err("ERR:nPortId's MS %d bits are not matching the group %d\n",
			       (9 - (nBits - 1)), cMode.nPortMsb);
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}
	}

	//Validate absolute couter index.
	if (tflowIndex >= PCE_TABLE_SIZE) { //Max index check.
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	//1.Clear PCE Rx counters.
	//Populate 'data' register data.
	portType = GSW_RMON_TFLOW_RX;
	FILL_CTRL_REG(addrRegData, BM_RAM_ADDR_RMON_PORT_SHIFT, tflowIndex);
	gsw_w32_raw(cdev, BM_RAM_VAL_0_VAL0_OFFSET, 0);
	gsw_w32_raw(cdev, BM_RAM_VAL_1_VAL1_OFFSET, 0);
	//Set RAM address to write.
	gsw_w32_raw(cdev, BM_RAM_ADDR_REG_OFFSET, addrRegData);
	//Populate 'control' register data.
	//RMON TFLOW Rx table to write.
	FILL_CTRL_REG(ctrlRegData, BM_RAM_CTRL_ADDR_SHIFT, portType);
	//RAM in write mode.
	FILL_CTRL_REG(ctrlRegData, BM_RAM_CTRL_OPMOD_SHIFT, 1);
	//64 bit counter mode.
	//FILL_CTRL_REG(ctrlRegData, BM_RAM_CTRL_64BIT_OPMOD_SHIFT, 0);
	//Set access busy bit.
	FILL_CTRL_REG(ctrlRegData, BM_RAM_CTRL_BAS_SHIFT, 1);
	//Set RAM table counters to clear.
	gsw_w32_raw(cdev, BM_RAM_CTRL_REG_OFFSET, ctrlRegData);
	//Wait untill RAM is ready to write.
	CHECK_BUSY(BM_RAM_CTRL_BAS_OFFSET, BM_RAM_CTRL_BAS_SHIFT,
		   BM_RAM_CTRL_BAS_SIZE, RETURN_FROM_FUNCTION);

	//2.Clear PCE Tx counters.
	//Populate 'data' register data.
	portType = GSW_RMON_TFLOW_TX;
	//RMON TFLOW Tx table to write.
	CLEAR_FILL_CTRL_REG(ctrlRegData, BM_RAM_CTRL_ADDR_SHIFT,
			    BM_RAM_CTRL_ADDR_SIZE, portType);
	//64 bit counter mode. It is not supported in HW at this moment.
	//FILL_CTRL_REG(ctrlRegData, BM_RAM_CTRL_64BIT_OPMOD_SHIFT, 1);
	gsw_w32_raw(cdev, BM_RAM_VAL_0_VAL0_OFFSET, 0);
	gsw_w32_raw(cdev, BM_RAM_VAL_1_VAL1_OFFSET, 0);
	//Set RAM address to write.
	gsw_w32_raw(cdev, BM_RAM_ADDR_REG_OFFSET, addrRegData);
	//Set RAM table to write.
	gsw_w32_raw(cdev, BM_RAM_CTRL_REG_OFFSET, ctrlRegData);
	//Wait untill RAM is ready to write.
	CHECK_BUSY(BM_RAM_CTRL_BAS_OFFSET, BM_RAM_CTRL_BAS_SHIFT,
		   BM_RAM_CTRL_BAS_SIZE, RETURN_FROM_FUNCTION);
	//3.Clear PCE-Bypass Tx counters.
	gsw_w32_raw(cdev, BM_RAM_VAL_0_VAL0_OFFSET, 0);
	gsw_w32_raw(cdev, BM_RAM_VAL_1_VAL1_OFFSET, 0);
	//Set RAM address to write.
	addrRegData = addrRegData + 1;
	gsw_w32_raw(cdev, BM_RAM_ADDR_REG_OFFSET, addrRegData);
	//Set RAM table to write.
	gsw_w32_raw(cdev, BM_RAM_CTRL_REG_OFFSET, ctrlRegData);
	//Wait untill RAM is ready to write.
	CHECK_BUSY(BM_RAM_CTRL_BAS_OFFSET, BM_RAM_CTRL_BAS_SHIFT,
		   BM_RAM_CTRL_BAS_SIZE, RETURN_FROM_FUNCTION);

	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_bm);
#endif
	return ret;
}

GSW_return_t GSW_TflowCountModeSet(void *cdev, GSW_TflowCmodeConf_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 numValBits = 0, rxTflowReg = 0, txTflowReg = 0, txBpTflowReg = 0;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d\n", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_bm);
#endif

	//Validate TFLOW RMON counter type.
	if (parm->eCountType > GSW_TFLOW_COUNTER_PCE_BP_Tx) {
		pr_err("ERR:Invalid couter type\n");
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	//Validate TFLOW RMON counter mode type.
	if (parm->eCountMode > GSW_TFLOW_CMODE_BRIDGE) {
		pr_err("ERR:Invalid couter mode type\n");
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	//Validate 'n' for CTP or Br port type.
	if (parm->eCountMode == GSW_TFLOW_CMODE_BRIDGE) {
		numValBits = parm->nBrpLsb;

		if ((numValBits < GSW_TCM_BRP_VAL_BITS_2) ||
		    (numValBits > GSW_TCM_BRP_VAL_BITS_6)) {
			pr_err("ERR:Invalid Br-Port type LSB bits\n");
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}
	} else if (parm->eCountMode == GSW_TFLOW_CMODE_CTP) {
		numValBits = parm->nCtpLsb;

		if ((numValBits > GSW_TCM_CTP_VAL_BITS_6)) {
			pr_err("ERR:Invalid CTP-Port type LSB bits\n");
			ret = GSW_statusErr;
			goto UNLOCK_AND_RETURN;
		}
	}

	//Configure registers.
	switch (parm->eCountMode) {
	case GSW_TFLOW_CMODE_BRIDGE:
	case GSW_TFLOW_CMODE_CTP:
		//1.PCE TFLOW Rx setting.
		//Populate PCE TFLOW Rx RMON register.
		//Set 'n' i.e port's LSB bits.
		FILL_CTRL_REG(rxTflowReg, BM_RXFLOW_RMON_LSB_SHIFT, numValBits);
		//Set port's MSB bits.
		FILL_CTRL_REG(rxTflowReg, BM_RXFLOW_RMON_MSB_SHIFT, parm->nPortMsb);

		//2.PCE TFLOW Tx setting.
		//Populate PCE TFLOW Tx RMON register.
		//Set 'n' i.e port's LSB bits.
		FILL_CTRL_REG(txTflowReg, BM_TXFLOW_RMON_LSB_SHIFT, numValBits);
		//Set port's MSB bits.
		FILL_CTRL_REG(txTflowReg, BM_TXFLOW_RMON_MSB_SHIFT, parm->nPortMsb);

		//3.PCE-Bypass TFLOW Tx setting.
		//Populate PCE-Bypass TFLOW Tx RMON register.
		//Set 'n' i.e port's LSB bits.
		FILL_CTRL_REG(txBpTflowReg, BM_TXBPFLOW_RMON_LSB_SHIFT, numValBits);
		//Set port's MSB bits.
		FILL_CTRL_REG(txBpTflowReg, BM_TXBPFLOW_RMON_MSB_SHIFT, parm->nPortMsb);

	//break; //Fall through to set mode.
	case GSW_TFLOW_CMODE_LOGICAL:
		//1.PCE TFLOW Rx setting.
		//Populate PCE TFLOW Rx RMON register.
		//Set counter mode.
		FILL_CTRL_REG(rxTflowReg, BM_RXFLOW_RMON_MD_SHIFT, parm->eCountMode);

		//2.PCE TFLOW Tx setting
		//Populate PCE TFLOW Tx RMON register.
		//Set counter mode.
		FILL_CTRL_REG(txTflowReg, BM_TXFLOW_RMON_MD_SHIFT, parm->eCountMode);

		//3.PCE-Bypass TFLOW Tx setting
		//Populate PCE-Bypass TFLOW Tx RMON register.
		//Set counter mode.
		FILL_CTRL_REG(txBpTflowReg, BM_TXBPFLOW_RMON_MD_SHIFT, parm->eCountMode);

	//break; //Fall through to write into register.
	case GSW_TFLOW_CMODE_GLOBAL:

		//Write PCE TFLOW Rx RMON register.
		if ((parm->eCountType == GSW_TFLOW_COUNTER_ALL) ||
		    (parm->eCountType == GSW_TFLOW_COUNTER_PCE_Rx))
			gsw_w32_raw(cdev, BM_RXFLOW_RMON_REG_OFFSET, rxTflowReg);

		//Write PCE TFLOW Tx RMON register.
		if ((parm->eCountType == GSW_TFLOW_COUNTER_ALL) ||
		    (parm->eCountType == GSW_TFLOW_COUNTER_PCE_Tx))
			gsw_w32_raw(cdev, BM_TXFLOW_RMON_REG_OFFSET, txTflowReg);

		//Write PCE-Bypass TFLOW Tx RMON register.
		if ((parm->eCountType == GSW_TFLOW_COUNTER_ALL) ||
		    (parm->eCountType == GSW_TFLOW_COUNTER_PCE_BP_Tx))
			gsw_w32_raw(cdev, BM_TXBPFLOW_RMON_REG_OFFSET, txBpTflowReg);

		break;

	default:
		break;
	}

	ret = GSW_statusOk;

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_bm);
#endif
	return ret;
}

GSW_return_t GSW_TflowCountModeGet(void *cdev, GSW_TflowCmodeConf_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 rxTflowReg = 0, txTflowReg = 0, txBpTflowReg = 0;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d\n", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_bm);
#endif

	switch (parm->eCountType) {
	case GSW_TFLOW_COUNTER_ALL:
	case GSW_TFLOW_COUNTER_PCE_Rx:
		//1.PCE TFLOW Rx config.
		//Read PCE TFLOW Rx RMON register.
		gsw_r32_raw(cdev, BM_RXFLOW_RMON_REG_OFFSET, &rxTflowReg);
		//Populate PCE TFLOW Rx RMON structure.
		//Get counter mode.
		GET_VAL_FROM_REG(parm->eCountMode, BM_RXFLOW_RMON_MD_SHIFT, BM_RXFLOW_RMON_MD_SIZE, rxTflowReg);

		//Get port's LSB bits.
		if (parm->eCountMode == GSW_TFLOW_CMODE_BRIDGE)
			GET_VAL_FROM_REG(parm->nBrpLsb, BM_RXFLOW_RMON_LSB_SHIFT, BM_RXFLOW_RMON_LSB_SIZE, rxTflowReg);

		if (parm->eCountMode == GSW_TFLOW_CMODE_CTP)
			GET_VAL_FROM_REG(parm->nCtpLsb, BM_RXFLOW_RMON_LSB_SHIFT, BM_RXFLOW_RMON_LSB_SIZE, rxTflowReg);

		//Get port's MSB bits.
		GET_VAL_FROM_REG(parm->nPortMsb, BM_RXFLOW_RMON_MSB_SHIFT, BM_RXFLOW_RMON_MSB_SIZE, rxTflowReg);
		break;

	case GSW_TFLOW_COUNTER_PCE_Tx:
		//2.PCE TFLOW Tx config
		//Read PCE TFLOW Tx RMON register.
		gsw_r32_raw(cdev, BM_TXFLOW_RMON_REG_OFFSET, &txTflowReg);
		//Populate PCE TFLOW Tx RMON structure.
		//Get counter mode.
		GET_VAL_FROM_REG(parm->eCountMode, BM_TXFLOW_RMON_MD_SHIFT, BM_TXFLOW_RMON_MD_SIZE, txTflowReg);

		//Get port's LSB bits.
		if (parm->eCountMode == GSW_TFLOW_CMODE_BRIDGE)
			GET_VAL_FROM_REG(parm->nBrpLsb, BM_TXFLOW_RMON_LSB_SHIFT, BM_TXFLOW_RMON_LSB_SIZE, txTflowReg);

		if (parm->eCountMode == GSW_TFLOW_CMODE_CTP)
			GET_VAL_FROM_REG(parm->nCtpLsb, BM_TXFLOW_RMON_LSB_SHIFT, BM_TXFLOW_RMON_LSB_SIZE, txTflowReg);

		//Get port's MSB bits.
		GET_VAL_FROM_REG(parm->nPortMsb, BM_TXFLOW_RMON_MSB_SHIFT, BM_TXFLOW_RMON_MSB_SIZE, txTflowReg);
		break;

	case GSW_TFLOW_COUNTER_PCE_BP_Tx:
		//3.PCE-Bypass TFLOW Tx config
		//Write PCE-Bypass TFLOW Tx RMON register.
		gsw_r32_raw(cdev, BM_TXBPFLOW_RMON_REG_OFFSET, &txBpTflowReg);
		//Populate PCE-Bypass TFLOW Tx RMON structure.
		//Get port's LSB bits.
		GET_VAL_FROM_REG(parm->eCountMode, BM_TXBPFLOW_RMON_MD_SHIFT, BM_TXBPFLOW_RMON_MD_SIZE, txBpTflowReg);

		//Get port's LSB bits.
		if (parm->eCountMode == GSW_TFLOW_CMODE_BRIDGE)
			GET_VAL_FROM_REG(parm->nBrpLsb, BM_TXBPFLOW_RMON_LSB_SHIFT, BM_TXBPFLOW_RMON_LSB_SIZE, txBpTflowReg);

		if (parm->eCountMode == GSW_TFLOW_CMODE_CTP)
			GET_VAL_FROM_REG(parm->nCtpLsb, BM_TXBPFLOW_RMON_LSB_SHIFT, BM_TXBPFLOW_RMON_LSB_SIZE, txBpTflowReg);

		//Get port's MSB bits.
		GET_VAL_FROM_REG(parm->nPortMsb, BM_TXBPFLOW_RMON_MSB_SHIFT, BM_TXBPFLOW_RMON_MSB_SIZE, txBpTflowReg);
		break;
	}

	ret = GSW_statusOk;

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_bm);
#endif
	return ret;
}

GSW_return_t GSW_Reset(void *cdev, GSW_reset_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 value;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_misc);
#endif

	gswdev->rst = 1;
	gswdev->hwinit = 0;
	/* Reset the Switch via Switch IP register*/
	value = 1;
	gsw_w32(cdev, ETHSW_SWRES_R0_OFFSET,
		ETHSW_SWRES_R0_SHIFT, ETHSW_SWRES_R0_SIZE, value);
	ret = CHECK_BUSY(ETHSW_SWRES_R0_OFFSET, ETHSW_SWRES_R0_SHIFT,
			 ETHSW_SWRES_R0_SIZE, RETURN_ERROR_CODE);

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_misc);
#endif
	return ret;

}

GSW_return_t GSW_VersionGet(void *cdev, GSW_version_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_misc);
#endif

	if (parm->nId == 0) {
		memcpy(parm->cName, VERSION_NAME, sizeof(VERSION_NAME));
		memcpy(parm->cVersion, VERSION_NUMBER, sizeof(VERSION_NUMBER));
	} else if (parm->nId == 1) {
		memcpy(parm->cName, MICRO_CODE_VERSION_NAME,
		       sizeof(MICRO_CODE_VERSION_NAME));
		memcpy(parm->cVersion, MICRO_CODE_VERSION_NUMBER,
		       sizeof(MICRO_CODE_VERSION_NUMBER));
	} else {
		memcpy(parm->cName, "", 0);
		memcpy(parm->cVersion, "", 0);
	}

	ret = GSW_statusOk;

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_misc);
#endif

	return ret;

}

static GSW_return_t GSW_Enable_Legacy(void *cdev)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u8 j;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return  GSW_statusErr;
	}


#if defined(CONFIG_USE_EMULATOR) && defined(__KERNEL__)

	for (j = 0; j < gswdev->tpnum; j++) {
		if (gswdev->gipver == LTQ_GSWIP_3_0) {
			gsw_w32(cdev,
				(MAC_CTRL_0_FDUP_OFFSET + (0xC * j)),
				MAC_CTRL_0_FDUP_SHIFT, MAC_CTRL_0_FDUP_SIZE, 1);
			gsw_w32(cdev,
				(MAC_CTRL_0_GMII_OFFSET + (0xC * j)),
				MAC_CTRL_0_GMII_SHIFT, MAC_CTRL_0_GMII_SIZE, 2);
			gsw_w32(cdev,
				((GSWT_PHY_ADDR_1_SPEED_OFFSET
				  + (j * 4)) + GSW30_TOP_OFFSET),
				GSWT_PHY_ADDR_1_SPEED_SHIFT,
				GSWT_PHY_ADDR_1_SPEED_SIZE, 2);
			gsw_w32(cdev,
				((GSWT_PHY_ADDR_1_FDUP_OFFSET
				  + (j * 4)) + GSW30_TOP_OFFSET),
				GSWT_PHY_ADDR_1_FDUP_SHIFT,
				GSWT_PHY_ADDR_1_FDUP_SIZE, 1);
			gsw_w32(cdev,
				((GSWT_PHY_ADDR_1_LNKST_OFFSET
				  + (j * 4)) + GSW30_TOP_OFFSET),
				GSWT_PHY_ADDR_1_LNKST_SHIFT,
				GSWT_PHY_ADDR_1_LNKST_SIZE, 1);
		} else {
			gsw_w32(cdev,
				((PHY_ADDR_0_SPEED_OFFSET - j)
				 + GSW_TREG_OFFSET),
				PHY_ADDR_0_SPEED_SHIFT,
				PHY_ADDR_0_SPEED_SIZE, 2);
			gsw_w32(cdev,
				((PHY_ADDR_0_FDUP_OFFSET - j)
				 + GSW_TREG_OFFSET),
				PHY_ADDR_0_FDUP_SHIFT,
				PHY_ADDR_0_FDUP_SIZE, 1);
			gsw_w32(cdev,
				((PHY_ADDR_0_LNKST_OFFSET - j)
				 + GSW_TREG_OFFSET),
				PHY_ADDR_0_LNKST_SHIFT,
				PHY_ADDR_0_LNKST_SIZE, 1);
		}
	}

#endif

	if (gswdev->gipver == LTQ_GSWIP_3_0) {
		for (j = 0; j < gswdev->tpnum; j++) {
			gsw_w32(cdev, (BM_PCFG_CNTEN_OFFSET + (j * 2)),
				BM_PCFG_CNTEN_SHIFT, BM_PCFG_CNTEN_SIZE, 1);
			gsw_w32(cdev, (BM_RMON_CTRL_BCAST_CNT_OFFSET + (j * 0x2)),
				BM_RMON_CTRL_BCAST_CNT_SHIFT, BM_RMON_CTRL_BCAST_CNT_SIZE, 1);
		}

		if (gswdev->sdev == LTQ_FLOW_DEV_INT_R) {
			gsw_w32(cdev, PCE_TFCR_NUM_NUM_OFFSET,
				PCE_TFCR_NUM_NUM_SHIFT,
				PCE_TFCR_NUM_NUM_SIZE, 0x80);
		}
	}

	return GSW_statusOk;

}

GSW_return_t GSW_Enable(void *cdev)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u8 j;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_misc);
#endif

	for (j = 0; j < gswdev->tpnum; j++) {
		gsw_w32(cdev,
			(FDMA_PCTRL_EN_OFFSET + (j * 0x6)),
			FDMA_PCTRL_EN_SHIFT,
			FDMA_PCTRL_EN_SIZE, 1);
		gsw_w32(cdev,
			(SDMA_PCTRL_PEN_OFFSET + (j * 0x6)),
			SDMA_PCTRL_PEN_SHIFT,
			SDMA_PCTRL_PEN_SIZE, 1);
		/*Enable All RMON Counters*/
		gsw_w32(cdev,
			(BM_PCFG_CNTEN_OFFSET + (j * 2)),
			BM_PCFG_CNTEN_SHIFT,
			BM_PCFG_CNTEN_SIZE, 1);
	}

	if (IS_VRSN_NOT_31(gswdev->gipver)) {
		GSW_Enable_Legacy(cdev);
	}

#if defined(CONFIG_MAC) && CONFIG_MAC

	if (IS_VRSN_31(gswdev->gipver)) {
		struct adap_ops *ops = get_adap_ops(gswdev);

		if (ops == NULL) {
			pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
			return GSW_statusErr;
		}

		ops->ss_core_en(ops, 1);
	}

#endif
	ret = GSW_statusOk;

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_misc);
#endif

	return ret;

}

GSW_return_t GSW_Disable(void *cdev)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u8 j;
	u32 ret;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_misc);
#endif

	/* Disable all physical port  */
	for (j = 0; j < gswdev->tpnum; j++) {
		gsw_w32(cdev, (FDMA_PCTRL_EN_OFFSET + (j * 0x6)),
			FDMA_PCTRL_EN_SHIFT, FDMA_PCTRL_EN_SIZE, 0);
		gsw_w32(cdev, (SDMA_PCTRL_PEN_OFFSET + (j * 0x6)),
			SDMA_PCTRL_PEN_SHIFT, SDMA_PCTRL_PEN_SIZE, 0);
	}

	ret = GSW_statusOk;

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_misc);
#endif

	return ret;
}

/* Freeze switch Ingress traffic */
GSW_return_t GSW_Freeze(void)
{
	struct adap_ops *ops = gsw_get_adap_ops(0);

	if (ops == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

	ops->ss_core_en(ops, 0);

	return 0;
}

/* UnFreeze switch Ingress traffic */
GSW_return_t GSW_UnFreeze(void)
{
	struct adap_ops *ops = gsw_get_adap_ops(0);

	if (ops == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

	ops->ss_core_en(ops, 1);

	return 0;
}

static u8 gsw_search_tflowcontiguousblock
		(ethsw_api_dev_t *gswdev,
		u32 blockid, u32 numberofentries)
{
	u32 i;

	for (i = blockid; i <= (blockid + numberofentries); i++) {
		if (gswdev->tflow_idx.flow_idx[i].indexinuse)
			return 0;
	}

	return 1;
}

GSW_return_t gsw_tflow_alloc(void *cdev, gsw_tflow_alloc_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u16 tflowindex = 0;
	u8 contiguousblockfound = 0;
	u32 i, ret = GSW_statusOk;

	if (!gswdev) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_pce);
#endif

	parm->num_of_pcerules = (parm->num_of_pcerules + 3) & ~0x03;
	if (parm->num_of_pcerules >
		(gswdev->tftblsize -
		gswdev->tflow_idx.usedentry)) {
		pr_err(" nNumberOfRules requested is more than TFLOW index limit  %s:%s:%d",
		       __FILE__, __func__, __LINE__);
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	/*Allocate New Block as per the number of table Entries requested
	 *The Block must be allocated with contiguous table index
	 */
	for (tflowindex = gswdev->num_of_global_rules;
		tflowindex < gswdev->tftblsize  &&
		!contiguousblockfound; tflowindex++) {
		/*Table Index not in use*/
		if (!gswdev->tflow_idx.flow_idx[tflowindex].indexinuse) {
			contiguousblockfound =
				gsw_search_tflowcontiguousblock
				(gswdev, tflowindex, parm->num_of_pcerules);
		}

		/*Contiguous block found in the table*/
		if (contiguousblockfound) {
			parm->tflowblockid = tflowindex;

			/*Mark the contiguous table indexes as InUse
			 *and tag it with block id
			 */
			for (i = parm->tflowblockid; i <
			     (parm->tflowblockid + parm->num_of_pcerules);
				 i++) {
				gswdev->tflow_idx.flow_idx[i].indexinuse = 1;
				gswdev->tflow_idx.flow_idx[i].tflowblockid =
				   parm->tflowblockid;
				gswdev->tflow_idx.usedentry++;
			}
		}
	}

	/*Contiguous block not found in the table*/
	if (!contiguousblockfound) {
		pr_err(" ContiguousBlock NotFound %s:%s:%d",
		       __FILE__, __func__, __LINE__);
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_pce);
#endif
	return ret;
}

GSW_return_t gsw_tflow_free(void *cdev, gsw_tflow_alloc_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 ret = GSW_statusOk, idx = 0;

	if (!gswdev) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_pce);
#endif

	if (parm->tflowblockid >= gswdev->tftblsize) {
		pr_err("tflowblockid %d out of range Supported num_of_tflowrules = %d]\n",
		       parm->tflowblockid, gswdev->tftblsize);
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	/*tflow blockid should be in use,if not in use return error*/
	if (!gswdev->tflow_idx.flow_idx[parm->tflowblockid].indexinuse) {
		pr_err("Blockid %u is Not inUse -> need to allocate before freeing\n",
		       parm->tflowblockid);
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	/*if this ntflowblockid usage count is not zero,
	 *that means it is used by some one.
	 * This Block can be deleted,
	 *only if that some one release this block
	 */
	if (gswdev->tflow_idx.flow_idx[parm->tflowblockid].indexinusagecnt) {
		pr_err("ERROR :TFLOW block %u is used by some resource,can not delete\n",
		       parm->tflowblockid);
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	idx = parm->tflowblockid;
	parm->num_of_pcerules = 0;

	if (gswdev->tflow_idx.flow_idx[idx].tflowblockid !=
	    parm->tflowblockid) {
		ret = GSW_statusErr;
		goto UNLOCK_AND_RETURN;
	}

	/*Condition to delete idx
	 *	1. idx should belong to this block
	 *	2. idx should be in Use
	 *	3. idx should be with in valid TFLOW
	 *	table range (Entry 0-511 is valid)
	 */
	while (gswdev->tflow_idx.flow_idx[idx].indexinuse &&
	       idx < gswdev->tftblsize &&
		   (gswdev->tflow_idx.flow_idx[idx].tflowblockid ==
		   parm->tflowblockid)) {
		/*free this idx and decrement the the number
		 *of used table entries/idx
		 */
		gswdev->tflow_idx.flow_idx[idx].indexinuse = 0;
		gswdev->tflow_idx.flow_idx[idx].tflowblockid =
				TFLOW_ENTRY_INVALID;
		gswdev->tflow_idx.usedentry--;
		/*A reference for the user, how many entries has
		 *been deleted in this block
		 *debugging purpose
		 */
		parm->num_of_pcerules++;
		/*increment the idx,since the block's allocation
		 *in ADD is Contiguous
		 */
		idx++;
	}

UNLOCK_AND_RETURN:

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_pce);
#endif
	return ret;
}
static GSW_return_t gsw_init_fn_ptrs(struct core_ops *ops)
{

	/*Switch Operations*/
	/*RMON operation*/
	ops->gsw_rmon_ops.RMON_Port_Get 				= GSW_RMON_Port_Get;
	ops->gsw_rmon_ops.RMON_Mode_Set					= GSW_RMON_Mode_Set;
	ops->gsw_rmon_ops.RMON_Meter_Get				= GSW_RMON_Meter_Get;
	ops->gsw_rmon_ops.RMON_Redirect_Get				= GSW_RMON_Redirect_Get;
	ops->gsw_rmon_ops.RMON_If_Get					= GSW_RMON_If_Get;
	ops->gsw_rmon_ops.RMON_Route_Get				= GSW_RMON_Route_Get;
	ops->gsw_rmon_ops.RMON_Clear					= GSW_RMON_Clear;
	ops->gsw_rmon_ops.RMON_ExtendGet				= GSW_RMON_ExtendGet;
	ops->gsw_rmon_ops.RMON_TflowGet					= GSW_RMON_FlowGet;
	ops->gsw_rmon_ops.RMON_TflowClear				= GSW_RmonTflowClear;
	ops->gsw_rmon_ops.RMON_TflowCountModeSet		= GSW_TflowCountModeSet;
	ops->gsw_rmon_ops.RMON_TflowCountModeGet		= GSW_TflowCountModeGet;

	/*Switch MAc operations*/
	ops->gsw_swmac_ops.MAC_TableClear 				= GSW_MAC_TableClear;
	ops->gsw_swmac_ops.MAC_TableEntryAdd 			= GSW_MAC_TableEntryAdd;
	ops->gsw_swmac_ops.MAC_TableEntryRead 			= GSW_MAC_TableEntryRead;
	ops->gsw_swmac_ops.MAC_TableEntryQuery 			= GSW_MAC_TableEntryQuery;
	ops->gsw_swmac_ops.MAC_TableEntryRemove 		= GSW_MAC_TableEntryRemove;
	ops->gsw_swmac_ops.MAC_DefaultFilterSet 		= GSW_DefaultMacFilterSet;
	ops->gsw_swmac_ops.MAC_DefaultFilterGet 		= GSW_DefaultMacFilterGet;

	/*Extended Vlan operations*/
	ops->gsw_extvlan_ops.ExtendedVlan_Alloc 		= GSW_ExtendedVlanAlloc;
	ops->gsw_extvlan_ops.ExtendedVlan_Set 			= GSW_ExtendedVlanSet;
	ops->gsw_extvlan_ops.ExtendedVlan_Get 			= GSW_ExtendedVlanGet;
	ops->gsw_extvlan_ops.ExtendedVlan_Free 			= GSW_ExtendedVlanFree;

	/*Vlan Filter operations*/
	ops->gsw_vlanfilter_ops.VlanFilter_Alloc 		= GSW_VlanFilterAlloc;
	ops->gsw_vlanfilter_ops.VlanFilter_Set 			= GSW_VlanFilterSet;
	ops->gsw_vlanfilter_ops.VlanFilter_Get 			= GSW_VlanFilterGet;
	ops->gsw_vlanfilter_ops.VlanFilter_Free 		= GSW_VlanFilterFree;

	/*CTP operations*/
	ops->gsw_ctp_ops.CTP_PortAssignmentAlloc		= GSW_CTP_PortAssignmentAlloc;
	ops->gsw_ctp_ops.CTP_PortAssignmentFree 		= GSW_CTP_PortAssignmentFree;
	ops->gsw_ctp_ops.CTP_PortAssignmentSet 			= GSW_CTP_PortAssignmentSet;
	ops->gsw_ctp_ops.CTP_PortAssignmentGet 			= GSW_CTP_PortAssignmentGet;
	ops->gsw_ctp_ops.CTP_PortConfigSet 				= GSW_CtpPortConfigSet;
	ops->gsw_ctp_ops.CTP_PortConfigGet 				= GSW_CtpPortConfigGet;
	ops->gsw_ctp_ops.CTP_PortConfigReset 			= GSW_CtpPortConfigReset;

	/*Bridge Port operations*/
	ops->gsw_brdgport_ops.BridgePort_Alloc			= GSW_BridgePortAlloc;
	ops->gsw_brdgport_ops.BridgePort_ConfigSet		= GSW_BridgePortConfigSet;
	ops->gsw_brdgport_ops.BridgePort_ConfigGet		= GSW_BridgePortConfigGet;
	ops->gsw_brdgport_ops.BridgePort_Free			= GSW_BridgePortFree;

	/*Bridge Operations*/
	ops->gsw_brdg_ops.Bridge_Alloc					= GSW_BridgeAlloc;
	ops->gsw_brdg_ops.Bridge_ConfigSet				= GSW_BridgeConfigSet;
	ops->gsw_brdg_ops.Bridge_ConfigGet				= GSW_BridgeConfigGet;
	ops->gsw_brdg_ops.Bridge_Free					= GSW_BridgeFree;

	/*TFlow operations*/
	ops->gsw_tflow_ops.TFLOW_PceRuleDelete			= GSW_PceRuleDelete;
	ops->gsw_tflow_ops.TFLOW_PceRuleRead			= GSW_PceRuleRead;
	ops->gsw_tflow_ops.TFLOW_PceRuleWrite			= GSW_PceRuleWrite;
	ops->gsw_tflow_ops.tflow_pcealloc				=
				gsw_tflow_alloc;
	ops->gsw_tflow_ops.tflow_pcefree				=
				gsw_tflow_free;

	/*QOS operations*/
	ops->gsw_qos_ops.QoS_MeterCfgGet				= GSW_QoS_MeterCfgGet;
	ops->gsw_qos_ops.QoS_MeterCfgSet				= GSW_QoS_MeterCfgSet;
	ops->gsw_qos_ops.QoS_MeterPortAssign			= GSW_QoS_MeterPortAssign;
	ops->gsw_qos_ops.QoS_MeterPortDeassign			= GSW_QoS_MeterPortDeassign;
	ops->gsw_qos_ops.QoS_MeterPortGet				= GSW_QoS_MeterPortGet;
	ops->gsw_qos_ops.QoS_DSCP_ClassGet				= GSW_QoS_DSCP_ClassGet;
	ops->gsw_qos_ops.QoS_DSCP_ClassSet				= GSW_QoS_DSCP_ClassSet;
	ops->gsw_qos_ops.QoS_ClassDSCP_Get				= GSW_QoS_ClassDSCP_Get;
	ops->gsw_qos_ops.QoS_ClassDSCP_Set				= GSW_QoS_ClassDSCP_Set;
	ops->gsw_qos_ops.QoS_DSCP_DropPrecedenceCfgGet	= GSW_QoS_DSCP_DropPrecedenceCfgGet;
	ops->gsw_qos_ops.QoS_DSCP_DropPrecedenceCfgSet	= GSW_QoS_DSCP_DropPrecedenceCfgSet;
	ops->gsw_qos_ops.QoS_PortRemarkingCfgGet		= GSW_QoS_PortRemarkingCfgGet;
	ops->gsw_qos_ops.QoS_PortRemarkingCfgSet		= GSW_QoS_PortRemarkingCfgSet;
	ops->gsw_qos_ops.QoS_ClassPCP_Get				= GSW_QoS_ClassPCP_Get;
	ops->gsw_qos_ops.QoS_ClassPCP_Set				= GSW_QoS_ClassPCP_Set;
	ops->gsw_qos_ops.QoS_PCP_ClassGet				= GSW_QoS_PCP_ClassGet;
	ops->gsw_qos_ops.QoS_PCP_ClassSet				= GSW_QoS_PCP_ClassSet;
	ops->gsw_qos_ops.QoS_PortCfgGet					= GSW_QoS_PortCfgGet;
	ops->gsw_qos_ops.QoS_PortCfgSet					= GSW_QoS_PortCfgSet;
	ops->gsw_qos_ops.QoS_QueuePortGet				= GSW_QoS_QueuePortGet;
	ops->gsw_qos_ops.QoS_QueuePortSet				= GSW_QoS_QueuePortSet;
	ops->gsw_qos_ops.QoS_SchedulerCfgGet			= GSW_QoS_SchedulerCfgGet;
	ops->gsw_qos_ops.QoS_SchedulerCfgSet			= GSW_QoS_SchedulerCfgSet;
	ops->gsw_qos_ops.QoS_ShaperCfgGet				= GSW_QoS_ShaperCfgGet;
	ops->gsw_qos_ops.QoS_ShaperCfgSet				= GSW_QoS_ShaperCfgSet;
	ops->gsw_qos_ops.QoS_ShaperQueueAssign			= GSW_QoS_ShaperQueueAssign;
	ops->gsw_qos_ops.QoS_ShaperQueueDeassign		= GSW_QoS_ShaperQueueDeassign;
	ops->gsw_qos_ops.QoS_ShaperQueueGet				= GSW_QoS_ShaperQueueGet;
	ops->gsw_qos_ops.QoS_StormCfgSet				= GSW_QoS_StormCfgSet;
	ops->gsw_qos_ops.QoS_StormCfgGet				= GSW_QoS_StormCfgGet;
	ops->gsw_qos_ops.QoS_WredCfgGet					= GSW_QoS_WredCfgGet;
	ops->gsw_qos_ops.QoS_WredCfgSet					= GSW_QoS_WredCfgSet;
	ops->gsw_qos_ops.QoS_WredQueueCfgGet			= GSW_QoS_WredQueueCfgGet;
	ops->gsw_qos_ops.QoS_WredQueueCfgSet			= GSW_QoS_WredQueueCfgSet;
	ops->gsw_qos_ops.QoS_WredPortCfgGet				= GSW_QoS_WredPortCfgGet;
	ops->gsw_qos_ops.QoS_WredPortCfgSet				= GSW_QoS_WredPortCfgSet;
	ops->gsw_qos_ops.QoS_FlowctrlCfgGet				= GSW_QoS_FlowctrlCfgGet;
	ops->gsw_qos_ops.QoS_FlowctrlCfgSet				= GSW_QoS_FlowctrlCfgSet;
	ops->gsw_qos_ops.QoS_FlowctrlPortCfgGet			= GSW_QoS_FlowctrlPortCfgGet;
	ops->gsw_qos_ops.QoS_FlowctrlPortCfgSet			= GSW_QoS_FlowctrlPortCfgSet;
	ops->gsw_qos_ops.QoS_QueueBufferReserveCfgGet	= GSW_QoS_QueueBufferReserveCfgGet;
	ops->gsw_qos_ops.QoS_QueueBufferReserveCfgSet	= GSW_QoS_QueueBufferReserveCfgSet;
	ops->gsw_qos_ops.QoS_Meter_Act					= GSW_QoS_Meter_Act;
	ops->gsw_qos_ops.QOS_ColorMarkingTableGet		= GSW_QOS_ColorMarkingTableGet;
	ops->gsw_qos_ops.QOS_ColorMarkingTableSet		= GSW_QOS_ColorMarkingTableSet;
	ops->gsw_qos_ops.QOS_ColorReMarkingTableSet		= GSW_QOS_ColorReMarkingTableSet;
	ops->gsw_qos_ops.QOS_ColorReMarkingTableGet		= GSW_QOS_ColorReMarkingTableGet;
	ops->gsw_qos_ops.QOS_MeterAlloc					= GSW_QOS_MeterAlloc;
	ops->gsw_qos_ops.QOS_MeterFree					= GSW_QOS_MeterFree;
	ops->gsw_qos_ops.QOS_Dscp2PcpTableSet			= GSW_QOS_Dscp2PcpTableSet;
	ops->gsw_qos_ops.QOS_Dscp2PcpTableGet			= GSW_QOS_Dscp2PcpTableGet;
	ops->gsw_qos_ops.QOS_PmapperTableSet			= GSW_QOS_PmapperTableSet;
	ops->gsw_qos_ops.QOS_PmapperTableGet			= GSW_QOS_PmapperTableGet;
	ops->gsw_qos_ops.QoS_SVLAN_ClassPCP_PortGet		= GSW_QoS_SVLAN_ClassPCP_PortGet;
	ops->gsw_qos_ops.QoS_SVLAN_ClassPCP_PortSet		= GSW_QoS_SVLAN_ClassPCP_PortSet;
	ops->gsw_qos_ops.QoS_SVLAN_PCP_ClassGet			= GSW_QoS_SVLAN_PCP_ClassGet;
	ops->gsw_qos_ops.QoS_SVLAN_PCP_ClassSet			= GSW_QoS_SVLAN_PCP_ClassSet;

	/*STP operations*/
	ops->gsw_stp_ops.STP_BPDU_RuleGet				= GSW_STP_BPDU_RuleGet;
	ops->gsw_stp_ops.STP_BPDU_RuleSet				= GSW_STP_BPDU_RuleSet;
	ops->gsw_stp_ops.STP_PortCfgGet					= GSW_STP_PortCfgGet;
	ops->gsw_stp_ops.STP_PortCfgSet					= GSW_STP_PortCfgSet;

	/*8021x operations*/
	ops->gsw_8021x_ops.EAPOL_RuleGet				= GSW_8021X_EAPOL_RuleGet;
	ops->gsw_8021x_ops.EAPOL_RuleGet_RuleSet		= GSW_8021X_EAPOL_RuleSet;
	ops->gsw_8021x_ops.EAPOL_RuleGet_PortCfgGet		= GSW_8021X_PortCfgGet;
	ops->gsw_8021x_ops.EAPOL_RuleGet_PortCfgSet		= GSW_8021X_PortCfgSet;

	/*multicast operations*/
	ops->gsw_multicast_ops.Multicast_RouterPortAdd	= GSW_MulticastRouterPortAdd;
	ops->gsw_multicast_ops.Multicast_RouterPortRead	= GSW_MulticastRouterPortRead;
	ops->gsw_multicast_ops.Multicast_RouterPortRemove = GSW_MulticastRouterPortRemove;
	ops->gsw_multicast_ops.Multicast_SnoopCfgGet	= GSW_MulticastSnoopCfgGet;
	ops->gsw_multicast_ops.Multicast_SnoopCfgSet	= GSW_MulticastSnoopCfgSet;
	ops->gsw_multicast_ops.Multicast_TableEntryAdd	= GSW_MulticastTableEntryAdd;
	ops->gsw_multicast_ops.Multicast_TableEntryRead	= GSW_MulticastTableEntryRead;
	ops->gsw_multicast_ops.Multicast_TableEntryRemove = GSW_MulticastTableEntryRemove;

	/*Trunking operations*/
	ops->gsw_trunking_ops.Trunking_CfgGet			= GSW_TrunkingCfgGet;
	ops->gsw_trunking_ops.Trunking_CfgSet			= GSW_TrunkingCfgSet;
	ops->gsw_trunking_ops.Trunking_PortCfgGet		= GSW_TrunkingPortCfgGet;
	ops->gsw_trunking_ops.Trunking_PortCfgSet		= GSW_TrunkingPortCfgSet;

	/*WOL operations*/
	ops->gsw_wol_ops.WoL_CfgGet						= GSW_WoL_CfgGet;
	ops->gsw_wol_ops.WoL_CfgSet						= GSW_WoL_CfgSet;
	ops->gsw_wol_ops.WoL_PortCfgGet					= GSW_WoL_PortCfgGet;
	ops->gsw_wol_ops.WoL_PortCfgSet					= GSW_WoL_PortCfgSet;

	/*Common switch operations*/
	ops->gsw_common_ops.RegisterGet					= GSW_RegisterGet;
	ops->gsw_common_ops.RegisterSet					= GSW_RegisterSet;
	ops->gsw_common_ops.IrqGet						= GSW_IrqGet;
	ops->gsw_common_ops.IrqMaskGet					= GSW_IrqMaskGet;
	ops->gsw_common_ops.IrqMaskSet					= GSW_IrqMaskSet;
	ops->gsw_common_ops.IrqStatusClear				= GSW_IrqStatusClear;
	ops->gsw_common_ops.Enable						= GSW_Enable;
	ops->gsw_common_ops.Reset						= GSW_Reset;
	ops->gsw_common_ops.Disable						= GSW_Disable;
	ops->gsw_common_ops.VersionGet					= GSW_VersionGet;
	ops->gsw_common_ops.CapGet						= GSW_CapGet;
	ops->gsw_common_ops.CfgGet						= GSW_CfgGet;
	ops->gsw_common_ops.CfgSet						= GSW_CfgSet;
	ops->gsw_common_ops.HW_Init						= GSW_HW_Init;
	ops->gsw_common_ops.PortLinkCfgGet				= GSW_PortLinkCfgGet;
	ops->gsw_common_ops.PortLinkCfgSet				= GSW_PortLinkCfgSet;
	ops->gsw_common_ops.PortPHY_AddrGet				= GSW_PortPHY_AddrGet;
	ops->gsw_common_ops.PortPHY_Query				= GSW_PortPHY_Query;
	ops->gsw_common_ops.PortRGMII_ClkCfgGet			= GSW_PortRGMII_ClkCfgGet;
	ops->gsw_common_ops.PortRGMII_ClkCfgSet			= GSW_PortRGMII_ClkCfgSet;
	ops->gsw_common_ops.PortRedirectGet				= GSW_PortRedirectGet;
	ops->gsw_common_ops.PortRedirectSet				= GSW_PortRedirectSet;
	ops->gsw_common_ops.CPU_PortCfgGet				= GSW_CPU_PortCfgGet;
	ops->gsw_common_ops.CPU_PortCfgSet				= GSW_CPU_PortCfgSet;
	ops->gsw_common_ops.CPU_PortExtendCfgGet		= GSW_CPU_PortExtendCfgGet;
	ops->gsw_common_ops.CPU_PortExtendCfgSet		= GSW_CPU_PortExtendCfgSet;
	ops->gsw_common_ops.MonitorPortCfgGet			= GSW_MonitorPortCfgGet;
	ops->gsw_common_ops.MonitorPortCfgSet			= GSW_MonitorPortCfgSet;
	ops->gsw_common_ops.Timestamp_TimerSet			= GSW_TimestampTimerSet;
	ops->gsw_common_ops.Timestamp_TimerGet			= GSW_TimestampTimerGet;
	ops->gsw_common_ops.Timestamp_PortRead			= GSW_TimestampPortRead;
	ops->gsw_common_ops.PortCfgGet					= GSW_PortCfgGet;
	ops->gsw_common_ops.PortCfgSet					= GSW_PortCfgSet;
	ops->gsw_common_ops.MDIO_CfgGet					= GSW_MDIO_CfgGet;
	ops->gsw_common_ops.MDIO_CfgSet					= GSW_MDIO_CfgSet;
	ops->gsw_common_ops.MDIO_DataRead				= GSW_MDIO_DataRead;
	ops->gsw_common_ops.MDIO_DataWrite				= GSW_MDIO_DataWrite;
	ops->gsw_common_ops.MmdDataRead					= GSW_MmdDataRead;
	ops->gsw_common_ops.MmdDataWrite				= GSW_MmdDataWrite;


	/*PMAC operations*/
	ops->gsw_pmac_ops.Pmac_CountGet					= GSW_PMAC_CountGet;
	ops->gsw_pmac_ops.Pmac_Gbl_CfgSet				= GSW_PMAC_GLBL_CfgSet;
	ops->gsw_pmac_ops.Pmac_Gbl_CfgGet				= GSW_PMAC_GLBL_CfgGet;
	ops->gsw_pmac_ops.Pmac_Bm_CfgSet				= GSW_PMAC_BM_CfgSet;
	ops->gsw_pmac_ops.Pmac_Bm_CfgGet				= GSW_PMAC_BM_CfgGet;
	ops->gsw_pmac_ops.Pmac_Ig_CfgSet				= GSW_PMAC_IG_CfgSet;
	ops->gsw_pmac_ops.Pmac_Ig_CfgGet				= GSW_PMAC_IG_CfgGet;
	ops->gsw_pmac_ops.Pmac_Eg_CfgSet				= GSW_PMAC_EG_CfgSet;
	ops->gsw_pmac_ops.Pmac_Eg_CfgGet				= GSW_PMAC_EG_CfgGet;

	/*VLAN operation*/
	ops->gsw_vlan_ops.VLAN_Member_Init				= GSW_VLAN_Member_Init;
	ops->gsw_vlan_ops.VLAN_IdCreate					= GSW_VLAN_IdCreate;
	ops->gsw_vlan_ops.VLAN_IdDelete					= GSW_VLAN_IdDelete;
	ops->gsw_vlan_ops.VLAN_IdGet					= GSW_VLAN_IdGet;
	ops->gsw_vlan_ops.VLAN_PortCfgGet				= GSW_VLAN_PortCfgGet;
	ops->gsw_vlan_ops.VLAN_PortCfgSet				= GSW_VLAN_PortCfgSet;
	ops->gsw_vlan_ops.VLAN_PortMemberAdd			= GSW_VLAN_PortMemberAdd;
	ops->gsw_vlan_ops.VLAN_PortMemberRead			= GSW_VLAN_PortMemberRead;
	ops->gsw_vlan_ops.VLAN_PortMemberRemove			= GSW_VLAN_PortMemberRemove;
	ops->gsw_vlan_ops.VLAN_ReservedAdd				= GSW_VLAN_ReservedAdd;
	ops->gsw_vlan_ops.VLAN_ReservedRemove			= GSW_VLAN_ReservedRemove;
	ops->gsw_vlan_ops.VLAN_PCE_EG_CfgSet			= GSW_PCE_EG_VLAN_CfgSet;
	ops->gsw_vlan_ops.VLAN_PCE_EG_CfgGet			= GSW_PCE_EG_VLAN_CfgGet;
	ops->gsw_vlan_ops.VLAN_PCE_EG_EntryWrite		= GSW_PCE_EG_VLAN_EntryWrite;
	ops->gsw_vlan_ops.VLAN_PCE_EG_EntryRead			= GSW_PCE_EG_VLAN_EntryRead;
	ops->gsw_vlan_ops.SVLAN_CfgGet					= GSW_SVLAN_CfgGet;
	ops->gsw_vlan_ops.SVLAN_CfgSet					= GSW_SVLAN_CfgSet;
	ops->gsw_vlan_ops.SVLAN_PortCfgGet				= GSW_SVLAN_PortCfgGet;
	ops->gsw_vlan_ops.SVLAN_PortCfgSet				= GSW_SVLAN_PortCfgSet;

#ifdef __KERNEL__
	/*PAE operation*/
	ops->gsw_pae_ops.ROUTE_SessionEntryAdd			= GSW_ROUTE_SessionEntryAdd;
	ops->gsw_pae_ops.ROUTE_SessionEntryDel			= GSW_ROUTE_SessionEntryDel;
	ops->gsw_pae_ops.ROUTE_SessionEntryRead			= GSW_ROUTE_SessionEntryRead;
	ops->gsw_pae_ops.ROUTE_TunnelEntryAdd			= GSW_ROUTE_TunnelEntryAdd;
	ops->gsw_pae_ops.ROUTE_TunnelEntryRead			= GSW_ROUTE_TunnelEntryRead;
	ops->gsw_pae_ops.ROUTE_TunnelEntryDel			= GSW_ROUTE_TunnelEntryDel;
	ops->gsw_pae_ops.ROUTE_L2NATCfgWrite			= GSW_ROUTE_L2NATCfgWrite;
	ops->gsw_pae_ops.ROUTE_L2NATCfgRead				= GSW_ROUTE_L2NATCfgRead;
	ops->gsw_pae_ops.ROUTE_SessHitOp				= GSW_ROUTE_SessHitOp;
	ops->gsw_pae_ops.ROUTE_SessDestModify			= GSW_ROUTE_SessDestModify;
#endif

	/*Debug operation*/
	ops->gsw_debug_ops.DEBUG_CtpTableStatus			= GSW_Debug_CtpTableStatus;
	ops->gsw_debug_ops.DEBUG_BrgPortTableStatus		= GSW_Debug_BrgPortTableStatus;
	ops->gsw_debug_ops.DEBUG_BrgTableStatus			= GSW_Debug_BrgTableStatus;
	ops->gsw_debug_ops.DEBUG_ExvlanTableStatus		= GSW_Debug_ExvlanTableStatus;
	ops->gsw_debug_ops.DEBUG_VlanFilterTableStatus	= GSW_Debug_VlanFilterTableStatus;
	ops->gsw_debug_ops.DEBUG_MeterTableStatus		= GSW_Debug_MeterTableStatus;
	ops->gsw_debug_ops.DEBUG_Dscp2PcpTableStatus	= GSW_Debug_Dscp2PcpTableStatus;
	ops->gsw_debug_ops.DEBUG_PmapperTableStatus		= GSW_Debug_PmapperTableStatus;
	ops->gsw_debug_ops.DEBUG_PmacEg					= GSW_Debug_PmacEgTable;
	ops->gsw_debug_ops.DEBUG_PmacIg					= GSW_Debug_PmacIgTable;
	ops->gsw_debug_ops.DEBUG_PmacBp					= GSW_Debug_PmacBpTable;
	ops->gsw_debug_ops.DEBUG_Def_PceQmap			= GSW_Debug_PceQTable;
	ops->gsw_debug_ops.DEBUG_Def_PceBypQmap			= GSW_Debug_PceBypassTable;
	ops->gsw_debug_ops.DEBUG_GetLpStatistics		= GSW_Debug_GetLpStatistics;
	ops->gsw_debug_ops.DEBUG_GetCtpStatistics 		= GSW_Debug_GetCtpStatistics;
	ops->gsw_debug_ops.debug_tflowtablestatus       =
				gsw_debug_tflow_tablestatus;
	ops->gsw_debug_ops.Xgmac						= GSW_XgmacCfg;
	ops->gsw_debug_ops.Gswss						= GSW_GswssCfg;
	ops->gsw_debug_ops.Lmac							= GSW_LmacCfg;
	ops->gsw_debug_ops.DEBUG_PrintPceIrqList		= GSW_Debug_PrintPceIrqList;
	ops->gsw_debug_ops.Macsec				        = GSW_MacsecCfg;
	ops->gsw_debug_ops.DEBUG_RMON_Port_Get			= GSW_Debug_RMON_Port_Get;
	ops->gsw_debug_ops.DumpMem				= GSW_DumpTable;

	/*IRQ Operation*/
	ops->gsw_irq_ops.IRQ_Register					= GSW_Irq_register;
	ops->gsw_irq_ops.IRQ_UnRegister					= GSW_Irq_unregister;
	ops->gsw_irq_ops.IRQ_Enable						= GSW_Irq_enable;
	ops->gsw_irq_ops.IRQ_Disable					= GSW_Irq_disable;

	return GSW_statusOk;
}

