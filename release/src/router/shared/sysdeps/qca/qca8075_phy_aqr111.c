/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <bcmnvram.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <netinet/if_ether.h>	//have in front of <linux/mii.h> to avoid redefinition of 'struct ethhdr'
#include <linux/mii.h>
#include <dirent.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <shutils.h>
#include <shared.h>
#include <utils.h>
#include <qca.h>
#include <flash_mtd.h>

#define DBGOUT		NULL			/* "/dev/console" */
enum {
	VLAN_TYPE_STB_VOIP = 0,
	VLAN_TYPE_WAN,
	VLAN_TYPE_WAN_NO_VLAN,	/* Used to bridge WAN/STB for Hinet MOD. */

	VLAN_TYPE_MAX
};



static const char *upstream_iptv_ifaces[16] = {
#if defined(RAX120)
	[WANS_DUALWAN_IF_WAN] = "eth4",
	[WANS_DUALWAN_IF_WAN2] = "eth5",
#else
#error Define WAN interfaces that can be used as upstream port of IPTV.
#endif
};

/* 0:WAN, 1:LAN, 2:WAN2(5G RJ-45), first index is switch_stb_x nvram variable.
 * lan_wan_partition[switch_stb_x][0] is virtual port0,
 * lan_wan_partition[switch_stb_x][1] is virtual port1, etc.
 * If it's 2, check it with is_aqr_phy_exist() before using it on RAX120.
 */
static const int lan_wan_partition[9][MAX_WANLAN_PORT] = {
	/* L1, L2, L3, L4, W1G, W5GR */
	{1,1,1,1,0,2}, // Normal
	{0,1,1,1,0,2}, // IPTV STB port = LAN1
	{1,0,1,1,0,2}, // IPTV STB port = LAN2
	{1,1,0,1,0,2}, // IPTV STB port = LAN3
	{1,1,1,0,0,2}, // IPTV STB port = LAN4
	{0,0,1,1,0,2}, // IPTV STB port = LAN1 & LAN2
	{1,1,0,0,0,2}, // IPTV STB port = LAN3 & LAN4
	{1,1,1,1,1,2}  // ALL
};

#if defined(RTCONFIG_BONDING_WAN) || defined(RTCONFIG_LACP)
/* array index:		port number used in wanports_bond, enum bs_port_id.
 * 			0: WAN, 1~4: LAN1~4, 30: 5G base-T (RJ-45)
 * array element:	virtual port
 * 			e.g. LAN1_PORT ~ LAN4_PORT, WAN_PORT, etc.
 */
static const int bsport_to_vport[32] = {
	WAN_PORT, LAN1_PORT, LAN2_PORT, LAN3_PORT, LAN4_PORT,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	WAN5GR_PORT, -1
};
#endif

/* array index:		virtual port mapping enumeration.
 * 			e.g. LAN1_PORT, LAN2_PORT, etc.
 * array element:	PHY address, negative value means absent PHY.
 * 			0x10? means Aquantia AQR111,
 * 			      check it with is_aqr_phy_exist before using it on RAX120.
 * 			0x20? means ethX, ethtool ioctl
 */
#if defined(RAX120)
/* HwId: A */
static const int vport_to_phy_addr_hwid_a[MAX_WANLAN_PORT] = {
	3, 2, 1, 0, 4, 0x107
};
static const int *vport_to_phy_addr = vport_to_phy_addr_hwid_a;
#else
#error FIXME
#endif

/**
 * The vport_to_iface array is used to get interface name of each virtual
 * port.  If bled need to know TX/RX statistics of LAN1~2, WAN1, WAN2 (AQR111),
 *  bled has to find this information from netdev.  So, define
 * this array and implement vport_to_iface_name() function which is used by
 * bled in update_swports_bled().
 *
 * array index:		virtual port mapping enumeration.
 * 			e.g. LAN1_PORT, LAN2_PORT, etc.
 * array element:	Interface name of specific virtual port.
 */
static const char *vport_to_iface[MAX_WANLAN_PORT] = {
	"eth3", "eth2", "eth1", "eth0", "eth4", "eth5"
};

/* IPTV virtual port bitmask */
static const unsigned int stb_to_mask[8] = { 
					0,
	(1U << LAN1_PORT),
	(1U << LAN2_PORT),
	(1U << LAN3_PORT),
	(1U << LAN4_PORT),
	(1U << LAN1_PORT) | (1U << LAN2_PORT),
	(1U << LAN3_PORT) | (1U << LAN4_PORT),
	(1U << LAN1_PORT) | (1U << LAN2_PORT) | (1U << LAN3_PORT) | (1U << LAN4_PORT)
};

void reset_qca_switch(void);

#define	CPU_PORT_WAN_MASK	(1U << WAN_PORT)
#define CPU_PORT_LAN_MASK	(0xF)

/* ALL WAN/LAN virtual port bit-mask */
static unsigned int wanlanports_mask = ((1U << WAN_PORT) | (1U << WAN5GR_PORT) | \
	(1U << LAN1_PORT) | (1U << LAN2_PORT) | (1U << LAN3_PORT) | (1U << LAN4_PORT));


/* Final model-specific LAN/WAN/WANS_LAN partition definitions.
 * Because LAN/WAN ports of RAX120 are distributed on several switches and phys.
 * These ports bitmask below are virtual port bitmask for a pseudo switch covers
 * QCA8075, AQR111.
 * bit0: VP0, bit1: VP1, bit2: VP2, bit3: VP3, bit4: VP4, bit5: VP5
 */
static unsigned int lan_mask = 0;	/* LAN only. Exclude WAN, WANS_LAN, and generic IPTV port. */
static unsigned int wan_mask = 0;	/* wan_type = WANS_DUALWAN_IF_WAN. Include generic IPTV port. */
static unsigned int wan2_mask = 0;	/* wan_type = WANS_DUALWAN_IF_WAN2. Include generic IPTV port. */
static unsigned int sfpp_mask = 0;	/* wan_type = WANS_DUALWAN_IF_SFPP. Include generic IPTV port. */
static unsigned int wans_lan_mask = 0;	/* wan_type = WANS_DUALWAN_IF_LAN. */

/* RT-N56U's P0, P1, P2, P3, P4 = LAN4, LAN3, LAN2, LAN1, WAN
 * ==> Model-specific virtual port number.
 * array inex:	RT-N56U's port number.
 * array value:	Model-specific virtual port number
 */
static int n56u_to_model_port_mapping[] = {
	LAN1_PORT,	//0000 0000 0001 LAN1
	LAN2_PORT,	//0000 0000 0010 LAN2
	LAN3_PORT,	//0000 0000 0100 LAN3
	LAN4_PORT,	//0000 0000 1000 LAN4
	WAN_PORT,	//0000 0001 0000 WAN
};

#define RTN56U_WAN_GMAC	(1U << 9)

int esw_stb;

/* Model-specific LANx ==> Model-specific virtual PortX.
 * array index:	Model-specific LANx (started from 0).
 * array value:	Model-specific virtual port number.
 */
const int lan_id_to_vport[NR_WANLAN_PORT] = {
	LAN1_PORT,
	LAN2_PORT,
	LAN3_PORT,
	LAN4_PORT,
	WAN_PORT,
	WAN5GR_PORT
};


/* Model-specific LANx (started from 0) ==> Model-specific virtual PortX */
static inline int lan_id_to_vport_nr(int id)
{
	//printf("===============id %d is %d\n", id, lan_id_to_vport[id]);
	return lan_id_to_vport[id];
}

/**
 * Get WAN port mask
 * @wan_unit:	wan_unit, if negative, select WANS_DUALWAN_IF_WAN
 * @return:	port bitmask
 */
static unsigned int get_wan_port_mask(int wan_unit)
{
	int sw_mode = sw_mode();
	char nv[] = "wanXXXports_maskXXXXXX";

	if (sw_mode == SW_MODE_AP || sw_mode == SW_MODE_REPEATER)
#if defined(RTCONFIG_AMAS)
		if (!nvram_match("re_mode", "1"))
#endif	/* RTCONFIG_AMAS */
		return 0;

	if (wan_unit <= 0 || wan_unit >= WAN_UNIT_MAX)
		strlcpy(nv, "wanports_mask", sizeof(nv));
	else
		snprintf(nv, sizeof(nv), "wan%dports_mask", wan_unit);
	return nvram_get_int(nv);
}

/**
 * Get LAN port mask
 * @return:	port bitmask
 */
static unsigned int get_lan_port_mask(void)
{
	int sw_mode = sw_mode();
	unsigned int m = nvram_get_int("lanports_mask");

	if (sw_mode == SW_MODE_AP || __mediabridge_mode(sw_mode))
		m = wanlanports_mask;

	return m;
}

#define IPQ807X_MDIO_SYS_DIR	"/sys/devices/platform/soc/90000.mdio/mdio_bus/90000.mdio/"
#define AQR_PHY_SYS_DIR		IPQ807X_MDIO_SYS_DIR "90000.mdio:07"
int is_aqr_phy_exist(void)
{
	static int aqr_phy_absent = -1;

	if (aqr_phy_absent >= 0)
		return !aqr_phy_absent;

	if (!d_exists(IPQ807X_MDIO_SYS_DIR)) {
		dbg("%s hasn't been created, assume AQR PHY exist!\n", IPQ807X_MDIO_SYS_DIR);
		return 1;
	}
	if (d_exists(AQR_PHY_SYS_DIR))
		aqr_phy_absent = 0;
	else
		aqr_phy_absent = 1;

	return !aqr_phy_absent;
}

/* Return PHY address of AQR111 based on HwId
 * @return:
 * 	-1: error
 *  0 ~ 31: PHY address
 */
int aqr_phy_addr(void)
{
#if defined(RAX120)
	return 7;
#else
#warning FIXME
#endif
}

/**
 * Convert (v)port to interface name.
 * @vport:	(virtual) port number
 * @return:
 * 	NULL:	@vport doesn't map to specific interface name.
 *  otherwise:	@vport do map to a specific interface name.
 */
const char *vport_to_iface_name(unsigned int vport)
{
	if (vport >= ARRAY_SIZE(vport_to_iface)) {
		dbg("%s: don't know vport %d\n", __func__, vport);
		return NULL;
	}

	return vport_to_iface[vport];
}

/**
 * Convert interface name to (v)port.
 * @iface:	interface name
 * @return:	(virtual) port number
 *  >=0:	(virtual) port number
 *  < 0:	can't find (virtual) port number for @iface.
 */
int iface_name_to_vport(const char *iface)
{
	int ret = -2, i;

	if (!iface)
		return -1;

	for (i = 0; ret < 0 && i < ARRAY_SIZE(vport_to_iface); ++i) {
		if (!vport_to_iface[i] || strcmp(vport_to_iface[i], iface))
			continue;
		ret = i;
	}

	return ret;
}

static inline int phy_addr_to_sw_port(int phy)
{
	return phy + 1;
}

/**
 * Convert vportmask to real portmask (QCA8337 only).
 * In most platform, vportmask = rportmask.
 * @vportmask:	virtual port mask
 * @return:	real portmask (QCA8337 only)
 */
unsigned int vportmask_to_rportmask(unsigned int vportmask)
{
	return vportmask;
}

/**
 * Set a VLAN
 * @vtype:	VLAN type
 * 	0:	VLAN for STB/VoIP
 * 	1:	VLAN for WAN
 * @upstream_if:upstream interface name
 * @vid:	VLAN ID, 0 ~ 4095.
 * @prio:	VLAN Priority
 * @mbr:	VLAN members
 * @untag:	VLAN members that need untag
 *
 * @return
 * 	0:	success
 *     -1:	invalid parameter
 */
int qca8075_aqr111_vlan_set(int vtype, char *upstream_if, int vid, int prio, unsigned int mbr, unsigned int untag)
{
	unsigned int upstream_mask = 0;
	int upstream_vport, wan_vlan_br = 0, wan_br = 0;
	char wvlan_if[IFNAMSIZ], vid_str[6], prio_str[4], brv_if[IFNAMSIZ];
	char *set_upst_viface_egress_map[] = { "vconfig", "set_egress_map", wvlan_if, "0", prio_str, NULL };

	dbg("%s: vtype %d upstream_if %s vid %d prio %d mbr 0x%x untag 0x%x\n",
		__func__, vtype, upstream_if, vid, prio, mbr, untag);
	if (!upstream_if || vtype < 0 || vtype >= VLAN_TYPE_MAX) {
		dbg("%s: invalid parameter\n", __func__);
		return -1;
	}

	if (bond_wan_enabled() && !strcmp(upstream_if, "bond1")) {
		uint32_t wmask = nums_str_to_u32_mask(nvram_safe_get("wanports_bond"));
		int b;
		const char *p;

		while ((b = ffs(wmask)) > 0) {
			b--;
			if ((p = bs_port_id_to_iface(b)) != NULL) {
				upstream_vport = iface_name_to_vport(p);
				if (upstream_vport < 0 || upstream_vport >= MAX_WANLAN_PORT) {
					dbg("%s: Can't find vport for upstream iface [%s] of bond1\n", __func__, upstream_if);
					return -1;
				}
				upstream_mask |= 1U << upstream_vport;
			}
			wmask &= ~(1U << b);
		}
	} else {
		upstream_vport = iface_name_to_vport(upstream_if);
		if (upstream_vport < 0 || upstream_vport >= MAX_WANLAN_PORT) {
			dbg("%s: Can't find vport for upstream iface [%s]\n", __func__, upstream_if);
			return -1;
		}
		upstream_mask = 1U << upstream_vport;
	}

	if (vtype == VLAN_TYPE_WAN_NO_VLAN) {
		wan_br = 1;
		vtype = VLAN_TYPE_WAN;
	}
	if (vtype == VLAN_TYPE_WAN && (mbr & ~(1U << WAN_PORT)) != 0)
		wan_vlan_br = 1;

	/* Replace WAN port as selected upstream port. */
	if (mbr & (1U << WAN_PORT)) {
		mbr &= ~(1U << WAN_PORT);
		mbr |= upstream_mask;
	}
	if (untag & (1U << WAN_PORT)) {
		untag &= ~(1U << WAN_PORT);
		untag |= upstream_mask;
	}

	snprintf(vid_str, sizeof(vid_str), "%d", vid);
	snprintf(prio_str, sizeof(prio_str), "%d", prio);
	snprintf(brv_if, sizeof(brv_if), "brv%d", vid);

	if ((vtype == VLAN_TYPE_WAN && wan_vlan_br) || vtype == VLAN_TYPE_STB_VOIP) {
		/* Use bridge to connect WAN and STB/VoIP. */
		eval("brctl", "addbr", brv_if);
		eval("ifconfig", brv_if, "0.0.0.0", "up");

		set_netdev_sysfs_param(brv_if, "bridge/multicast_querier", "1");
		set_netdev_sysfs_param(brv_if, "bridge/multicast_snooping",
			nvram_match("switch_br_no_snooping", "1")? "0" : "1");
	}

	if (vtype == VLAN_TYPE_WAN) {
		if (wan_br) {
			/* In this case, no VLAN on WAN port. */
			strlcpy(wvlan_if, upstream_if, sizeof(wvlan_if));
		} else {
			/* Follow naming rule in set_basic_ifname_vars() on upstream interface. */
			snprintf(wvlan_if, sizeof(wvlan_if), "vlan%d", vid);
			_eval(set_upst_viface_egress_map, DBGOUT, 0, NULL);
			eval("ifconfig", upstream_if, "0.0.0.0", "up");
		}
		if (wan_vlan_br) {
			eval("brctl", "addif", brv_if, wvlan_if);
		}
		eval("ifconfig", wvlan_if, "0.0.0.0", "up");
	} else if (vtype == VLAN_TYPE_STB_VOIP) {
		snprintf(wvlan_if, sizeof(wvlan_if), "%s.%d", upstream_if, vid);
		_eval(set_upst_viface_egress_map, DBGOUT, 0, NULL);
		eval("brctl", "addif", brv_if, wvlan_if);
		eval("ifconfig", wvlan_if, "0.0.0.0", "up");
	}

	return 0;
}

/**
 * Get link status and/or phy speed of a traditional PHY.
 * @phy:	PHY address
 * @link:	pointer to unsigned integer.
 * @speed:	pointer to unsigned integer.
 * 		If speed != NULL,
 * 			*speed = 1 means 100Mbps
 * 			*speed = 2 means 1000Mbps
 * 			*speed = 3 means 10Gbps
 * 			*speed = 4 means 2.5Gbps
 * 			*speed = 5 means 5Gbps
 * @link_speed:	pointer to unsigned integer which is used to store normal link speed.
 * 		e.g. 100 = 100Mbps, 10000 = 10Gbps, etc.
 * @return
 * 	0:	success
 *     -1:	invalid parameter.
 *     -2:	miireg_read() MII_BMSR failed
 */
static int get_phy_info(unsigned int phy, unsigned int *link, unsigned int *speed, unsigned int *link_speed, phy_info *info)
{
	int r = 0, l = 1, s = 0, lspd = 0;

	/* Use ssdk_sh port linkstatus/speed get command to minimize number of executed ssdk_sh
	 * and number of accessing MDIO bus.
	 */
	if (phy >= 0 && phy <= 4) {
		r = ipq8074_port_speed(phy_addr_to_sw_port(phy));
	} else if (phy < 0x20) {
		r = mdio_phy_speed(phy);
	} else if (phy >= 0x100 && phy < 0x120) {
		r = aqr_phy_speed(phy & 0x1F);
	} else {
		dbg("%s: Unknown PHY address 0x%x\n", __func__, phy);
	}

	lspd = r;
	switch (r) {
	case 5000:
		s = 5;
		break;
	case 2500:
		s = 4;
		break;
	case 10000:
		s = 3;
		break;
	case 1000:
		s = 2;
		break;
	case 100:
		s = 1;
		break;
	case 10:
		s = 0;
		break;
	default:
		l  = lspd = 0;
		break;
	}

	if (link)
		*link = l;
	if (speed)
		*speed = s;
	if (link_speed)
		*link_speed = lspd;
	if (info) {
		if (l) {
			info->link_rate = lspd;
			snprintf(info->state, sizeof(info->state), "up");
			// TODO: need to retreive duplex from driver
			//snprintf(info->duplex, sizeof(info->duplex), "none"); 
		} else {
			snprintf(info->state, sizeof(info->state), "down");
			snprintf(info->duplex, sizeof(info->duplex), "none");
		}
	}

	return 0;
}

/**
 * Get link status and/or phy speed of a virtual port.
 * @vport:	virtual port number
 * @link:	pointer to unsigned integer.
 * 		If link != NULL,
 * 			*link = 0 means link-down
 * 			*link = 1 means link-up.
 * @speed:	pointer to unsigned integer.
 * 		If speed != NULL,
 * 			*speed = 1 means 100Mbps
 * 			*speed = 2 means 1000Mbps
 * @return:
 * 	0:	success
 *     -1:	invalid parameter
 *  otherwise:	fail
 */
static int get_qca8075_aqr111_vport_info(unsigned int vport, unsigned int *link, unsigned int *speed, phy_info *info)
{
	int phy;

	if (vport >= MAX_WANLAN_PORT || (!link && !speed))
		return -1;

	if (link)
		*link = 0;
	if (speed)
		*speed = 0;

	if (vport == WAN5GR_PORT && !is_aqr_phy_exist())
		return 0;

	phy = *(vport_to_phy_addr + vport);
	if (phy < 0) {
		dbg("%s: can't get PHY address of vport %d\n", __func__, vport);
		return -1;
	}

	get_phy_info(phy, link, speed, NULL, info);

	return 0;
}

/**
 * Get linkstatus in accordance with port bit-mask.
 * @mask:	port bit-mask.
 * 		bit0 = VP0, bit1 = VP1, etc.
 * @linkStatus:	link status of all ports that is defined by mask.
 * 		If one port of mask is linked-up, linkStatus is true.
 * @return:	speed.
 * 	0:	not connected.
 *  non-zero:	linkrate.
 */
static int get_qca8075_aqr111_phy_linkStatus(unsigned int mask, unsigned int *linkStatus)
{
	int i,t,speed=0;
	unsigned int value = 0, m;

	m = mask & wanlanports_mask;
	for (i = 0; m > 0 && !value; ++i, m >>= 1) {
		if (!(m & 1))
			continue;

		get_qca8075_aqr111_vport_info(i, &value, (unsigned int*) &t, NULL);
		value &= 0x1;
	}
	*linkStatus = value;

	switch (t) {
	case 0x0:
		speed = 10;
		break;
	case 0x1:
		speed = 100;
		break;
	case 0x2:
		speed = 1000;
		break;
	case 0x3:
		speed = 10000;
		break;
	case 0x4:
		speed = 2500;
		break;
	case 0x5:
		speed = 5000;
		break;
	default:
		speed=0;
		if ((mask & wanlanports_mask) != 0) {
			_dprintf("%s: mask %8x t %8x invalid speed!\n", __func__, mask, t);
		}
	}
	return speed;
}


/**
 * Set wanports_mask, wanXports_mask, and lanports_mask based on
 * @stb：
 * 	0:	Default.
 * 	1:	LAN1
 * 	2:	LAN2
 * 	3:	LAN3
 * 	4:	LAN4
 * 	5:	LAN1+LAN2
 * 	6:	LAN3+LAN4
 * @stb_bitmask parameters.
 * @stb_bitmask should be platform-specific (v)port bitmask.
 */
static void build_wan_lan_mask(int stb, int stb_bitmask)
{
	int i, unit, type, m, upstream_unit = __get_upstream_wan_unit();
	int wanscap_lan = get_wans_dualwan() & WANSCAP_LAN;
	int wans_lanport = nvram_get_int("wans_lanport");
	int sw_mode = sw_mode();
	char prefix[8], nvram_ports[20];
	unsigned int unused_wan_mask = 0, iptv_mask = 0;

	if (stb < 0 || stb >= ARRAY_SIZE(stb_to_mask)) {
		return;
	}
	if (sw_mode == SW_MODE_AP || sw_mode == SW_MODE_REPEATER)
		wanscap_lan = 0;

	if (wanscap_lan && (wans_lanport < 1 || wans_lanport > 4)) {
		_dprintf("%s: invalid wans_lanport %d!\n", __func__, wans_lanport);
		wanscap_lan = 0;
	}

	/* To compatible to original architecture, @stb and @stb_bitmask are exclusive.
	 * @stb is assumed as zero if @stb_bitmask is non-zero value.  Because
	 * "rtkswitch  8 X" specifies STB port configuration via 0 ~ 6 and
	 * "rtkswitch 38 X" specifies VoIP/STB port via RT-N56U's port bitmask.
	 */
	if (stb_bitmask != 0)
		stb = 0;
	if (sw_mode == SW_MODE_ROUTER) {
		if (stb_bitmask != 0)
			iptv_mask = stb_bitmask;
		else
			iptv_mask = stb_to_mask[stb];
	}

	lan_mask = wan_mask = wans_lan_mask = 0;
	for (i = 0; i < NR_WANLAN_PORT; ++i) {
		switch (lan_wan_partition[stb][i]) {
		case 0:
			wan_mask |= 1U << lan_id_to_vport_nr(i);
			break;
		case 1:
			lan_mask |= 1U << lan_id_to_vport_nr(i);
			break;
		case 2:
			if (is_aqr_phy_exist())
				wan2_mask |= 1U << lan_id_to_vport_nr(i);
			break;
		case 3:
			sfpp_mask |= 1U << lan_id_to_vport_nr(i);
			break;
		default:
			_dprintf("%s: Unknown LAN/WAN port definition. (stb %d i %d val %d)\n",
				__func__, stb, i, lan_wan_partition[stb][i]);
		}
	}

	/* One of LAN port is acting as WAN. */
	if (wanscap_lan) {
		wans_lan_mask = 1U << lan_id_to_vport_nr(wans_lanport - 1);
		lan_mask &= ~wans_lan_mask;
	}

	unused_wan_mask = wan_mask | wan2_mask | sfpp_mask;
	for (unit = WAN_UNIT_FIRST; unit < WAN_UNIT_MAX; ++unit) {
		snprintf(prefix, sizeof(prefix), "%d", unit);
		snprintf(nvram_ports, sizeof(nvram_ports), "wan%sports_mask", (unit == WAN_UNIT_FIRST)?"":prefix);
		m = 0;	/* In AP/RP/MB mode, all WAN ports are bridged to LAN. */

		if (sw_mode == SW_MODE_ROUTER
#ifdef RTCONFIG_AMAS
		 ||(sw_mode == SW_MODE_AP && nvram_match("re_mode", "1"))
#endif	/* RTCONFIG_AMAS */
		 ) {
			type = get_dualwan_by_unit(unit);

			switch (type) {
			case WANS_DUALWAN_IF_WAN:
				m = wan_mask;
				unused_wan_mask &= ~wan_mask;
				break;
			case WANS_DUALWAN_IF_WAN2:
				m = wan2_mask;
				unused_wan_mask &= ~wan2_mask;
				break;
			case WANS_DUALWAN_IF_SFPP:
				m = sfpp_mask;
				unused_wan_mask &= ~sfpp_mask;
				break;
			case WANS_DUALWAN_IF_LAN:
				m = wans_lan_mask;
				break;
			default:
				nvram_unset(nvram_ports);
				break;
			}

			if (m == 0)
				continue;

			if (unit == upstream_unit)
				m |= iptv_mask;
		}
		nvram_set_int(nvram_ports, m);
	}

	/* Let all unused WAN ports become LAN ports.
	 * 1. 10G RJ-45 and 10G SFP+ can be WAN or LAN, depends on dualwan configuration.
	 * 2. 1G WAN can aggreate with LAN1/LAN2, creates 3Gbps bandwidth. (TODO)
	 */
	lan_mask = (lan_mask | unused_wan_mask) & ~iptv_mask;
	nvram_set_int("lanports_mask", lan_mask);
}

/**
 * Configure LAN/WAN partition base on generic IPTV type.
 * @type:
 * 	0:	Default.
 * 	1:	LAN1
 * 	2:	LAN2
 * 	3:	LAN3
 * 	4:	LAN4
 * 	5:	LAN1+LAN2
 * 	6:	LAN3+LAN4
 */
static void config_qca8075_aqr111_LANWANPartition(int type)
{
	build_wan_lan_mask(type, 0);
	reset_qca_switch();
	dbg("%s: LAN/WAN1/WAN2 portmask %08x/%08x/%08x Upstream %s (unit %d)\n",
		__func__, lan_mask, nvram_get_int("wanports_mask"), nvram_get_int("wan1ports_mask"),
		get_wan_base_if(), __get_upstream_wan_unit());
}

static void get_qca8075_aqr111_WAN_Speed(unsigned int *speed)
{
	int i, v = -1, t;
	unsigned int m;

	m = (get_wan_port_mask(0) | get_wan_port_mask(1)) & wanlanports_mask;
	for (i = 0; m; ++i, m >>= 1) {
		if (!(m & 1))
			continue;

		get_qca8075_aqr111_vport_info(i, NULL, (unsigned int*) &t, NULL);
		t &= 0x3;
		if (t > v)
			v = t;
	}

	switch (v) {
	case 0x0:
		*speed = 10;
		break;
	case 0x1:
		*speed = 100;
		break;
	case 0x2:
		*speed = 1000;
		break;
	case 0x3:
		*speed = 10000;
		break;
	case 0x4:
		*speed = 2500;
		break;
	case 0x5:
		*speed = 5000;
		break;
	default:
		_dprintf("%s: invalid speed! (%d)\n", __func__, v);
	}
}

/**
 * @vpmask:	Virtual port mask
 * @status:	0: power down PHY; otherwise: power up PHY
 */
static void link_down_up_qca8075_aqr111_PHY(unsigned int vpmask, int status)
{
	int vport, phy, r;
	unsigned int m;
	char iface[IFNAMSIZ];

	vpmask &= wanlanports_mask;
	for (vport = 0, m = vpmask; m; ++vport, m >>= 1) {
		if (!(m & 1))
			continue;
		if (vport >= MAX_WANLAN_PORT) {
			dbg("%s: PHY address is not defined for vport %d\n", __func__, vport);
			continue;
		}

		if (vport == WAN5GR_PORT && !is_aqr_phy_exist())
			return;
		phy = *(vport_to_phy_addr + vport);
		if (phy < 0) {
			dbg("%s: can't get PHY address of vport %d\n", __func__, vport);
			return;
		}

		if (phy < 0x20) {
			/* Legacy PHY and QCA8337 PHY */
			if ((r = read_phy_reg(phy, MII_BMCR)) < 0)
				r = 0;
			if (!status)	/* power down PHY */
				r |= BMCR_PDOWN;
			else
				r &= ~(BMCR_PDOWN);
			write_phy_reg(phy, MII_BMCR, r);
		} else if (phy >= 0x100 && phy < 0x120) {
			/* Aquantia PHY */
			if ((r = read_phy_reg(phy & 0x1F, 0x40010009)) < 0)
				r = 0;
			if (!status)	/* power down PHY */
				r |= 1;
			else
				r &= ~(1);
			write_phy_reg(phy & 0x1F, 0x40010009, r);
		} else if (phy >= 0x200 && phy < 0x210) {
			snprintf(iface, sizeof(iface), "eth%d", phy & 0xF);
			eval("ifconfig", iface, (!status)? "down" : "up");
		} else {
			dbg("%s: Unknown PHY address 0x%x\n", __func__, phy);
		}
	}
}

#if 0
/*define structure for software with 64bit*/
typedef struct
{
	uint64_t RxBroad;
	uint64_t RxPause;
	uint64_t RxMulti;
	uint64_t RxFcsErr;
	uint64_t RxAllignErr;
	uint64_t RxRunt;
	uint64_t RxFragment;
	uint64_t Rx64Byte;
	uint64_t Rx128Byte;
	uint64_t Rx256Byte;
	uint64_t Rx512Byte;
	uint64_t Rx1024Byte;
	uint64_t Rx1518Byte;
	uint64_t RxMaxByte;
	uint64_t RxTooLong;
	uint64_t RxGoodByte;
	uint64_t RxBadByte;
	uint64_t RxOverFlow;		/* no this counter for Hawkeye*/
	uint64_t Filtered;			/*no this counter for Hawkeye*/
	uint64_t TxBroad;
	uint64_t TxPause;
	uint64_t TxMulti;
	uint64_t TxUnderRun;
	uint64_t Tx64Byte;
	uint64_t Tx128Byte;
	uint64_t Tx256Byte;
	uint64_t Tx512Byte;
	uint64_t Tx1024Byte;
	uint64_t Tx1518Byte;
	uint64_t TxMaxByte;
	uint64_t TxOverSize;	/*no this counter for Hawkeye*/
	uint64_t TxByte;
	uint64_t TxCollision;
	uint64_t TxAbortCol;
	uint64_t TxMultiCol;
	uint64_t TxSingalCol;
	uint64_t TxExcDefer;
	uint64_t TxDefer;
	uint64_t TxLateCol;
	uint64_t RxUniCast;
	uint64_t TxUniCast;
	uint64_t RxJumboFcsErr;	/* add for  Hawkeye*/
	uint64_t RxJumboAligenErr;	/* add for Hawkeye*/
} fal_mib_counter_t;

#define MISC_CHR_DEV           10
#define UK_MINOR_DEV           254
#define DEV_SWITH_SSDK_PATH    "/dev/switch_ssdk"
#define SW_MAX_API_BUF         2048
#define SW_MAX_API_PARAM       12 /* cmd type + return value + ten parameters */
#define SW_API_PT_MIB_COUNTER_GET        1107
#endif


#if 0
/**
 * Return MiB(tx_bytes/rx_bytes/tx_pakcets/rx_packets/crc_errors) of @phy.
 * @port:	port id
 * @info:	sruct phy_info to store MiB(tx_bytes/rx_bytes/tx_pakcets/rx_packets/crc_errors)
 * @return:	0 for success
     non-zero for fail
 */
static int get_qca8075_aqr111_port_mib(unsigned int port, phy_info *info)
{
	int fd;
	unsigned long arg_val[SW_MAX_API_PARAM] = {0};
	unsigned long rtn = 0;
	fal_mib_counter_t mib_counter = {0};

	if (!info)
		return -1;

	/* even mknod fail we not quit, perhaps the device node exist already */
	mknod(DEV_SWITH_SSDK_PATH, S_IFCHR, makedev(MISC_CHR_DEV, UK_MINOR_DEV));
	if ((fd = open(DEV_SWITH_SSDK_PATH, O_RDWR)) < 0) {
		return -1;
	}
	arg_val[0] = (unsigned long)SW_API_PT_MIB_COUNTER_GET;  // API
	arg_val[1] = (unsigned long)&rtn;
	arg_val[2] = (unsigned long)0;    //device id
	arg_val[3] = (unsigned long)port; //port
	arg_val[4] = (unsigned long)(&mib_counter);

	ioctl(fd, SIOCDEVPRIVATE, arg_val);
	//fprintf(stderr, "rtn=%d, port=%d\n", rtn, port);
	if (rtn == 0) {
		info->tx_bytes = mib_counter.TxByte;
		info->rx_bytes = mib_counter.RxGoodByte;
		info->tx_packets = mib_counter.TxBroad + mib_counter.TxMulti + mib_counter.TxUniCast;
		info->rx_packets = mib_counter.RxBroad + mib_counter.RxMulti + mib_counter.RxUniCast;
		info->crc_errors = mib_counter.RxFcsErr;
		//fprintf(stderr, "tx_bytes=%llu, rx_bytes=%llu, tx_packets=%lu, rx_packets=%lu, crc_errors=%lu\n", 
		//	info->tx_bytes, info->rx_bytes, info->tx_packets, info->rx_packets, info->crc_errors);
	}
	close(fd);
	return 0;
}
#endif

void reset_qca_switch(void)
{
	nvram_unset("vlan_idx");
}

static void set_Vlan_VID(int vid)
{
	char tmp[8];

	snprintf(tmp, sizeof(tmp), "%d", vid);
	nvram_set("vlan_vid", tmp);
}

static void set_Vlan_PRIO(int prio)
{
	char tmp[2];

	snprintf(tmp, sizeof(tmp), "%d", prio);
	nvram_set("vlan_prio", tmp);
}

static int convert_n56u_portmask_to_model_portmask(unsigned int orig)
{
	int i, bit, bitmask;
	bitmask = 0;
	for(i = 0; i < ARRAY_SIZE(n56u_to_model_port_mapping); i++) {
		bit = (1 << i);
		if (orig & bit)
			bitmask |= (1 << n56u_to_model_port_mapping[i]);
	}
	return bitmask;
}

/**
 * @stb_bitmask:	bitmask of STB port(s)
 * 			e.g. bit0 = P0, bit1 = P1, etc.
 */
static void initialize_Vlan(int stb_bitmask)
{
	char *p, wan_base_if[IFNAMSIZ] = "N/A";

	stb_bitmask = convert_n56u_portmask_to_model_portmask(stb_bitmask);
	build_wan_lan_mask(0, stb_bitmask);
	if ((p = get_wan_base_if()) != NULL)
		strlcpy(wan_base_if, p, sizeof(wan_base_if));

	dbg("%s: LAN/P.WAN/S.WAN portmask %08x/%08x/%08x Upstream %s (unit %d)\n",
		__func__, lan_mask, nvram_get_int("wanports_mask"), nvram_get_int("wan1ports_mask"),
		get_wan_base_if(), __get_upstream_wan_unit());
	reset_qca_switch();
}

/**
 * Create VLAN for LAN and/or WAN in accordance with bitmask parameter.
 * @bitmask:
 *  bit15~bit0:		member port bitmask.
 * 	bit0:		RT-N56U port0, LAN4
 * 	bit1:		RT-N56U port1, LAN3
 * 	bit2:		RT-N56U port2, LAN2
 * 	bit3:		RT-N56U port3, LAN1
 * 	bit4:		RT-N56U port4, WAN
 * 	bit8:		RT-N56U port8, LAN_CPU port
 * 	bit9:		RT-N56U port9, WAN_CPU port
 *  bit31~bit16:	untag port bitmask.
 * 	bit16:		RT-N56U port0, LAN4
 * 	bit17:		RT-N56U port1, LAN3
 * 	bit18:		RT-N56U port2, LAN2
 * 	bit19:		RT-N56U port3, LAN1
 * 	bit20:		RT-N56U port4, WAN
 * 	bit24:		RT-N56U port8, LAN_CPU port
 * 	bit25:		RT-N56U port9, WAN_CPU port
 * First Ralink-based model is RT-N56U.
 * Convert RT-N56U-specific bitmask to physical port of your model,
 * base on relationship between physical port and visual WAN/LAN1~4 of that model first.
 */
static void create_Vlan(int bitmask)
{
	const int vid = nvram_get_int("vlan_vid");
	const int prio = nvram_get_int("vlan_prio") & 0x7;
	unsigned int mbr = bitmask & 0xffff;
	unsigned int untag = (bitmask >> 16) & 0xffff;
	unsigned int mbr_qca, untag_qca;
	int vtype = VLAN_TYPE_STB_VOIP;
	char upstream_if[IFNAMSIZ];

	//convert port mapping
	dbg("%s: bitmask:%08x, mbr:%08x, untag:%08x\n", __func__, bitmask, mbr, untag);
	mbr_qca   = convert_n56u_portmask_to_model_portmask(mbr);
	untag_qca = convert_n56u_portmask_to_model_portmask(untag);
	dbg("%s: after conversion mbr:%08x, untag:%08x\n", __func__, mbr_qca, untag_qca);
	if (untag & 0x10) {
		vtype = VLAN_TYPE_WAN_NO_VLAN;
	} else if (mbr & RTN56U_WAN_GMAC) {
		/* setup VLAN for WAN (WAN1 or WAN2), not VoIP/STB */
		vtype = VLAN_TYPE_WAN;
	}

	/* selecet upstream port for IPTV port. */
	strlcpy(upstream_if, get_wan_base_if(), sizeof(upstream_if));
	qca8075_aqr111_vlan_set(vtype, upstream_if, vid, prio, mbr_qca, untag_qca);
}

int qca8075_aqr111_ioctl(int val, int val2)
{
	unsigned int value2 = 0;
	int i, max_wan_unit = 0;

#if defined(RTCONFIG_DUALWAN)
	max_wan_unit = 1;
#endif

	switch (val) {
	case 0:
		value2 = rtkswitch_wanPort_phyStatus(-1);
		printf("WAN link status : %u\n", value2);
		break;
	case 3:
		value2 = rtkswitch_lanPorts_phyStatus();
		printf("LAN link status : %u\n", value2);
		break;
	case 8:
		config_qca8075_aqr111_LANWANPartition(val2);
		break;
	case 13:
		get_qca8075_aqr111_WAN_Speed(&value2);
		printf("WAN speed : %u Mbps\n", value2);
		break;
	case 14: // Link up LAN ports
		link_down_up_qca8075_aqr111_PHY(get_lan_port_mask(), 1);
		break;
	case 15: // Link down LAN ports
		link_down_up_qca8075_aqr111_PHY(get_lan_port_mask(), 0);
		break;
	case 16: // Link up ALL ports
		link_down_up_qca8075_aqr111_PHY(wanlanports_mask, 1);
		break;
	case 17: // Link down ALL ports
		link_down_up_qca8075_aqr111_PHY(wanlanports_mask, 0);
		break;
	case 27:
		reset_qca_switch();
		break;
	case 36:
		set_Vlan_VID(val2);
		break;
	case 37:
		set_Vlan_PRIO(val2);
		break;
	case 38:
		initialize_Vlan(val2);
		break;
	case 39:
		create_Vlan(val2);
		break;
	case 114: // link up WAN ports
		for (i = WAN_UNIT_FIRST; i <= max_wan_unit; ++i)
			link_down_up_qca8075_aqr111_PHY(get_wan_port_mask(i), 1);
		break;
	case 115: // link down WAN ports
		for (i = WAN_UNIT_FIRST; i <= max_wan_unit; ++i)
			link_down_up_qca8075_aqr111_PHY(get_wan_port_mask(i), 0);
		break;
	case 200:	/* set LAN port number that is used as WAN port */
		/* Nothing to do, nvram_get_int("wans_lanport ") is enough. */
		break;

	/* unused ioctl command. */
	case 21:	/* reset storm control rate, only Realtek switch platform need. */
	case 22:	/* set unknown unicast storm control rate. RTK switch only. */
	case 23:	/* set unknown multicast storm control rate. RTK switch only. */
	case 24:	/* set multicast storm control rate. RTK switch only. */
	case 25:	/* set broadcast storm rate. RTK switch only. */
	case 29:	/* Set VoIP port.  Not using any more. */
	case 40:
	case 50:	/* Fix-up hwnat for WiFi interface on MTK platform. */
		break;
	default:
		printf("wrong ioctl cmd: %d\n", val);
	}

	return 0;
}

int config_rtkswitch(int argc, char *argv[])
{
	int val;
	int val2 = 0;
	char *cmd = NULL;
	char *cmd2 = NULL;

	if (argc >= 2)
		cmd = argv[1];
	else
		return -1;
	if (argc >= 3)
		cmd2 = argv[2];

	val = (int) strtol(cmd, NULL, 0);
	if (cmd2)
		val2 = (int) strtol(cmd2, NULL, 0);
	return qca8075_aqr111_ioctl(val, val2);
}

unsigned int
rtkswitch_Port_phyStatus(unsigned int port_mask)
{
	unsigned int status = 0;

	get_qca8075_aqr111_phy_linkStatus(port_mask, &status);

	return status;
}

unsigned int
rtkswitch_Port_phyLinkRate(unsigned int port_mask)
{
	unsigned int speed = 0 ,status = 0;

	speed=get_qca8075_aqr111_phy_linkStatus(port_mask, &status);

	return speed;
}

/* PHY address => switch port */

unsigned int
rtkswitch_wanPort_phyStatus(int wan_unit)
{
	unsigned int status = 0;

#if defined(RTCONFIG_BONDING_WAN)
	if (bond_wan_enabled() && sw_mode() == SW_MODE_ROUTER
	 && get_dualwan_by_unit(wan_unit) == WANS_DUALWAN_IF_WAN)
	{
		int r = ethtool_glink("bond1");
		if (r >= 0)
			status = r;
		return status;
	}
#endif

	get_qca8075_aqr111_phy_linkStatus(get_wan_port_mask(wan_unit), &status);

	return status;
}

unsigned int
rtkswitch_lanPorts_phyStatus(void)
{
	unsigned int status = 0;

	get_qca8075_aqr111_phy_linkStatus(get_lan_port_mask(), &status);

	return status;
}

unsigned int
rtkswitch_WanPort_phySpeed(void)
{
	unsigned int speed;

	get_qca8075_aqr111_WAN_Speed(&speed);

	return speed;
}

int rtkswitch_WanPort_linkUp(void)
{
	eval("rtkswitch", "114");

	return 0;
}

int rtkswitch_WanPort_linkDown(void)
{
	eval("rtkswitch", "115");

	return 0;
}

int
rtkswitch_LanPort_linkUp(void)
{
	system("rtkswitch 14");

	return 0;
}

int
rtkswitch_LanPort_linkDown(void)
{
	system("rtkswitch 15");

	return 0;
}

int
rtkswitch_AllPort_linkUp(void)
{
	system("rtkswitch 16");

	return 0;
}

int
rtkswitch_AllPort_linkDown(void)
{
	system("rtkswitch 17");

	return 0;
}

int
rtkswitch_Reset_Storm_Control(void)
{
	system("rtkswitch 21");

	return 0;
}

/**
 * @link:
 * 	0:	no-link
 * 	1:	link-up
 * @speed:
 * 	0,10:		10Mbps		==> 'M'
 * 	1,100:		100Mbps		==> 'M'
 * 	2,1000:		1000Mbps	==> 'G'
 * 	3,10000:	10Gbps		==> 'T'
 * 	4,2500:		2.5Gbps		==> 'Q'
 * 	5,5000:		5Gbps		==> 'F'
 */
static char conv_speed(unsigned int link, unsigned int speed)
{
	char ret = 'X';

	if (link != 1)
		return ret;

	if (speed == 2 || speed == 1000)
		ret = 'G';
	else if (speed == 3 || speed == 10000)
		ret = 'T';
	else if (speed == 4 || speed == 2500)
		ret = 'Q';
	else if (speed == 5 || speed == 5000)
		ret = 'F';
	else
		ret = 'M';

	return ret;
}

void ATE_port_status(int verbose, phy_info_list *list)
{
	int i;
	char buf[6 * 11], wbuf[6 * 3], lbuf[6 * 8];
#ifdef RTCONFIG_NEW_PHYMAP
	char cap_buf[64] = {0};
	char wlen = 0, llen = 0;
#endif
	phyState pS;

#ifdef RTCONFIG_NEW_PHYMAP
	phy_port_mapping port_mapping;
	get_phy_port_mapping(&port_mapping);

	for (i = 0; i < port_mapping.count; i++) {
		// Only handle WAN/LAN ports
		if (((port_mapping.port[i].cap & PHY_PORT_CAP_WAN) == 0) && ((port_mapping.port[i].cap & PHY_PORT_CAP_LAN) == 0))
			continue;
		pS.link[i] = 0;
		pS.speed[i] = 0;
		get_qca8075_aqr111_vport_info(lan_id_to_vport_nr(i), &pS.link[i], &pS.speed[i], list ? &list->phy_info[i] : NULL);

		if (list) {
			list->phy_info[i].phy_port_id = port_mapping.port[i].phy_port_id;
			snprintf(list->phy_info[i].label_name, sizeof(list->phy_info[i].label_name), "%s", 
				port_mapping.port[i].label_name);
			list->phy_info[i].cap = port_mapping.port[i].cap;
			snprintf(list->phy_info[i].cap_name, sizeof(list->phy_info[i].cap_name), "%s", 
				get_phy_port_cap_name(port_mapping.port[i].cap, cap_buf, sizeof(cap_buf)));
			/*if (pS.link[i] == 1 && !list->status_and_speed_only)
				get_qca8075_aqr111_port_mib(port_mapping.port[i].phy_port_id, &list->phy_info[i]);*/
			list->phy_info[i].flag = port_mapping.port[i].flag;

			list->count++;
		}
		if ((port_mapping.port[i].cap & PHY_PORT_CAP_WAN) > 0)
			wlen += sprintf(wbuf+wlen, "%s=%C;", port_mapping.port[i].label_name,
						conv_speed(pS.link[i], pS.speed[i]));
		else
			llen += sprintf(lbuf+llen, "%s=%C;", port_mapping.port[i].label_name,
						conv_speed(pS.link[i], pS.speed[i]));
	}

#else
	memset(&pS, 0, sizeof(pS));
	for (i = 0; i < NR_WANLAN_PORT; i++) {
		get_qca8075_aqr111_vport_info(lan_id_to_vport_nr(i), &pS.link[i], &pS.speed[i], list ? &list->phy_info[i] : NULL);

		if (list) {
			list->phy_info[list->count].phy_port_id = lan_id_to_vport_nr(i);
			if (!list->count) {
				list->phy_info[list->count].cap = PHY_PORT_CAP_WAN;
				snprintf(list->phy_info[list->count].cap_name, sizeof(list->phy_info[i].cap_name), "wan");
				snprintf(list->phy_info[list->count].label_name, sizeof(list->phy_info[list->count].label_name), "W0");
			}
			else {
				list->phy_info[list->count].cap = PHY_PORT_CAP_LAN;
				snprintf(list->phy_info[list->count].cap_name, sizeof(list->phy_info[i].cap_name), "lan");
				snprintf(list->phy_info[list->count].label_name, sizeof(list->phy_info[list->count].label_name), "L%d", 
					list->count);
			}
			/*if (pS.link[i] == 1 && !list->status_and_speed_only)
				get_ipq40xx_port_mib(lan_id_to_port_nr(i), &list->phy_info[i]);*/
			list->phy_info[i].flag = 0;

			list->count++;
		}
	}

	snprintf(lbuf, sizeof(lbuf), "L1=%C;L2=%C;L3=%C;L4=%C;",
		conv_speed(pS.link[LAN1_PORT], pS.speed[LAN1_PORT]),
		conv_speed(pS.link[LAN2_PORT], pS.speed[LAN2_PORT]),
		conv_speed(pS.link[LAN3_PORT], pS.speed[LAN3_PORT]),
		conv_speed(pS.link[LAN4_PORT], pS.speed[LAN4_PORT]));
	snprintf(wbuf, sizeof(wbuf), "W0=%C;W1=%C;",
		conv_speed(pS.link[WAN_PORT], pS.speed[WAN_PORT]),
		conv_speed(pS.link[WAN5GR_PORT], pS.speed[WAN5GR_PORT]));
#endif // #ifdef RTCONFIG_NEW_PHYMAP

	strlcpy(buf, wbuf, sizeof(buf));
	strlcat(buf, lbuf, sizeof(buf));

	if (verbose)
		puts(buf);
}

/* Callback function which is used to fin brvX interface, X must be number.
 * @return:
 * 	0:	d->d_name is not brvX interface.
 *  non-zero:	d->d_name is brvx interface.
 */
static int brvx_filter(const struct dirent *d)
{
	const char *p;

	if (!d || strncmp(d->d_name, "brv", 3))
		return 0;

	p = d->d_name + 3;
	while (*p != '\0') {
		if (!isdigit(*p))
			return 0;
		p++;
	}

	return 1;
}

void upgrade_aqr113c_fw(void)
{
	char *fw_name = "/lib/firmware/RAX120_aqr111.cld";
	int id1, id2, fw, build, r, aqr_addr = aqr_phy_addr();
	char addr[sizeof("32XX")], ver[sizeof("255.255.15XXX")];
	time_t t1;

	if (!is_aqr_phy_exist() || aqr_addr < 0 || pidof("aq-fw-download") > 0)
		return;

	id1 = read_phy_reg(aqr_addr, 0x40070002);
	id2 = read_phy_reg(aqr_addr, 0x40070003);
	if (id1 != 0x31c3 || (id2 & 0xFFF0) != 0x1c10){
		printf("id1=%x,id2=%x\n", id1, id2);
		return;
	}
	nvram_set("aqr_act_swver", "");
	snprintf(addr, sizeof(addr), "%d", aqr_addr);
	eval("aq-fw-download", "-w", fw_name, "miireg", addr);

	t1 = uptime();
	do {
		fw = read_phy_reg(aqr_addr, 0x401e0020);
		build = read_phy_reg(aqr_addr, 0x401ec885);
		if (fw < 0  || build < 0) {
			dbg("%s: Can't get AQR PHY firmware version.\n", __func__);
			sleep(1);
			continue;
		} else if (fw == 0xFFFF || build == 0xFFFF) {
			dbg("wait 1s\n");
			sleep(1);
			continue;
		}
		break;

	} while ((uptime() - t1) < 5);

	r = read_phy_reg(aqr_addr, 0x40010009);
	if (r >= 0 && (r & 1)) {
		r &= ~(1U);
		write_phy_reg(aqr_addr, 0x40010009, r);
	}

	dbg("AQR PHY @ %d firmware %d.%d build %X.%X\n", aqr_addr,
		(fw >> 8) & 0xFF, fw & 0xFF, (build >> 4) & 0xF, build & 0xF);
	snprintf(ver, sizeof(ver), "%d.%d.%X", (fw >> 8) & 0xFF, fw & 0xFF, (build >> 4) & 0xF);
	nvram_set("aqr_act_swver", ver);
}

void __pre_config_switch(void)
{
	const int *paddr;
	int i, j, r1, nr_brvx, nr_brif;
	struct dirent **brvx = NULL, **brif = NULL;
	char brif_path[sizeof("/sys/class/net/X/brifXXXXX") + IFNAMSIZ];
	char *aqr_ssdk_port = "6"; /* GMAC6, AQR111 */
	char *autoneg[] = { "ssdk_sh", SWID_IPQ807X, "port", "autoNeg", "restart", aqr_ssdk_port, NULL };

	_eval(autoneg, DBGOUT, 0, NULL);

	int id1, id2, aqr_addr = aqr_phy_addr();
	char id_str[sizeof("0xXXXXXXXXYYY")];

	id1 = read_phy_reg(aqr_addr, 0x40070002);
	id2 = read_phy_reg(aqr_addr, 0x40070003);
	if (id1 >= 0 && id2 >= 0) {
		snprintf(id_str, sizeof(id_str), "0x%08x", (id1 & 0xFFFF) << 16 | (id2 & 0xFFF0));
		nvram_set("aqr_chip_id", id_str);
	}
	/* Print AQR firmware version. */
	for (i = 0, paddr = vport_to_phy_addr; i < MAX_WANLAN_PORT; ++i, ++paddr) {
		int fw, build;

		/* only accept AQR phy */
		if (*paddr < 0x100 || *paddr >= 0x120)
			continue;

		fw = read_phy_reg(*paddr & 0xFF, 0x401e0020);
		build = read_phy_reg(*paddr & 0xFF, 0x401ec885);
		if (fw < 0  || build < 0) {
			dbg("%s: Can't get AQR PHY firmware version.\n", __func__);
		} else {
			char ver[sizeof("255.255.15XXX")];
			dbg("AQR PHY @ %d firmware %d.%d build %X.%X\n", *paddr & 0xFF,
				(fw >> 8) & 0xFF, fw & 0xFF, (build >> 4) & 0xF, build & 0xF);
			snprintf(ver, sizeof(ver), "%d.%d.%X",
				(fw >> 8) & 0xFF, fw & 0xFF, (build >> 4) & 0xF);
			nvram_set("aqr_act_swver", ver);
		}
	}

	/* Remove all brvXXX bridge interfaces that are used to bridge WAN and STB/VoIP. */
	nr_brvx = scandir(SYS_CLASS_NET, &brvx, brvx_filter, alphasort);
	for (i = 0; i < nr_brvx; ++i) {
		snprintf(brif_path, sizeof(brif_path), "%s/%s/brif", SYS_CLASS_NET, brvx[i]->d_name);
		nr_brif = scandir(brif_path, &brif, NULL, alphasort);
		if (nr_brif <= 0) {
			free(brvx[i]);
			continue;
		}

		for (j = 0; j < nr_brif; ++j) {
			eval("brctl", "delif", brvx[i]->d_name, brif[j]->d_name);
		}
		free(brif);
		free(brvx[i]);
	}
	free(brvx);

}
/*
	Force the AQR111 work in speed 2500
	ssdk_sh debug phy set 7 0x40070010 0x9001  # Set bits[8:7] = 00
	ssdk_sh debug phy set 7 0x40070020 0x00e1  # Set bits[C] = 0, bits[8:7] = 01
	ssdk_sh debug phy set 7 0x4007c400 0x1440  # Set bits[F] = 0, bits[C:A] = 101
	sleep 1
	ssdk_sh debug phy set 7 0x4004c441 0x8
*/
void __post_config_switch(void)
{
	int i, speed, r, ipg = 0;
	char *aqr_ssdk_port = "6"; /* GMAC6, AQR111 */
	char port[sizeof("1XX")], speed_str[sizeof("10000XXX")], adv_str[sizeof("0xVVVVXXX")];
	char *ipq807x_p1_8023az[] = { "ssdk_sh", SWID_IPQ807X, "port", "ieee8023az", "set", "1", "disable", NULL };
	char *autoadv[] = { "ssdk_sh", SWID_IPQ807X, "port", "autoAdv", "set", aqr_ssdk_port, adv_str, NULL };
	char *fix_aqr_speed[] = { "ssdk_sh", SWID_IPQ807X, "port", "speed", "set", aqr_ssdk_port, speed_str, NULL };
	char read_ipg_cmd[] = "ssdk_sh " SWID_IPQ807X " debug reg get 0x7000 4";
	char write_ipg_cmd[sizeof("ssdk_sh sw0 debug reg set 0x7000 0xXXXXXXXX 4XXX")];
	/* Always turn off IEEE 802.3az support on IPQ8074 port 1 and QCA8337 port 1~5. */
	_eval(ipq807x_p1_8023az, DBGOUT, 0, NULL);

	/* Fixed 5G base-T link-speed. */
	speed = nvram_get_int("aqr_link_speed");
	if (speed == 1000)
		snprintf(adv_str, sizeof(adv_str), "0x%x", 1U << 9);
	else if (speed == 2500)
		snprintf(adv_str, sizeof(adv_str), "0x%x", 1U << 12);
	else if (speed == 5000)
		snprintf(adv_str, sizeof(adv_str), "0x%x", 1U << 13);
	else
		*adv_str = '\0';

	if (*adv_str) {
		_eval(autoadv, DBGOUT, 0, NULL);

		snprintf(speed_str, sizeof(speed_str), "%d", speed);
		_eval(fix_aqr_speed, DBGOUT, 0, NULL);
		dbg("Fixed 5G base-T link-speed as %dMbps, advertise mask %s!\n", speed, adv_str);
	}

	/* Set IPG of port 6 that is wired to Aquantia 5G. */
	if (!(r = parse_ssdk_sh(read_ipg_cmd, "%*[^:]:%x", 1, &ipg))) {
		if (nvram_match("aqr_ipg", "128"))
			ipg = (ipg & 0xFFFFF0FF) | 0x900;	/* 128 bit times */
		else {
			ipg &= 0xFFFFF0FF;			/*  96 bit times */
			if (!nvram_match("aqr_ipg", "96"))
				nvram_set("aqr_ipg", "96");
		}

		snprintf(write_ipg_cmd, sizeof(write_ipg_cmd), "ssdk_sh " SWID_IPQ807X " debug reg set 0x7000 0x%x 4", ipg);
		if ((r = parse_ssdk_sh(write_ipg_cmd, NULL, 0)) != 0) {
			dbg("%s: write IPG failed, cmd [%s], return %d\n", __func__, write_ipg_cmd, r);
		}
	}
}

/* Set acceleration type of 5G base-T as "PPE + NSS" or "NSS only"
 * by setting MAC learning/packet action as "ENABLE"/"FORWARD" or "DISABLE"/"RDTCPU".
 * When hardware NAT ON, 1Gbps LAN may get unstable download throughput, about 600Mbps,
 * upload throughput is stable at 940Mbps at same time. Wireless TPT seems okay at same time.
 * Some environment get stable 940Mbps download throughput after enable flowcontrol (pause-frame)
 * manually after negotiation done. Another environment do need to change acceleration type of
 * XGMAC from "PPE + NSS" to "NSS only".
 */
void __post_ecm(void)
{
	const char *enable_ppe_str[] = { "ENABLE", "FORWARD" };
	const char *disable_ppe_str[] = { "DISABLE", "RDTCPU" };
	int flush = 0, act, enable_ppe;
	char xgmac_port[4], mac_learn[sizeof("DISABLEXXX")], pkt_act[sizeof("FORWARDXXX")];
	char mac_learn_result[sizeof("DISABLEXXX")], pkt_act_result[sizeof("FORWARDXXX")];
	char *set_cmd[] = { "ssdk_sh", SWID_IPQ807X, "fdb", "ptLearnCtrl", "set", xgmac_port, mac_learn, pkt_act, NULL };
	char *flush_cmd[] = { "ssdk_sh", SWID_IPQ807X, "fdb", "entry", "flush", "1", NULL };
	char get_cmd[sizeof("ssdk_sh " SWID_IPQ807X " fdb ptLearnCtrl get XXX")];
	struct xgmac_defs_s {
		int xgmac_port;
		int wanscap;
		char *nv;
		int (*exist_func)(void);
	} xgmac_defs_tbl[] = {
		{ 6, WANSCAP_WAN2, "aqr_hwnat_type", is_aqr_phy_exist },

		{ -1, 0, NULL, NULL }
	}, *p;

	if (nvram_match("qca_sfe", "0"))
		return;

	for (p = &xgmac_defs_tbl[0]; p->nv != NULL; ++p) {
		if (p->exist_func && !p->exist_func())
			continue;

		enable_ppe = act = nvram_get_int(p->nv);
		if (act < 0 || act > 2) {
			act = 0;
			nvram_set_int(p->nv, act);
		}
		if (act == 2)
			enable_ppe = 0;

		if (!act) {
			/* If 5G base-T is one of WAN port and acceleration type is auto, select "NSS only". */
			if ((get_wans_dualwan() & p->wanscap) != 0)
				enable_ppe = 0;
			else
				enable_ppe = 1;
		}

		/* Example:
		 * SSDK Init OK![Learn Ctrl]:ENABLE[Action]:FORWARD
		 *operation done.
		 *
		 * SSDK Init OK![Learn Ctrl]:DISABLE[Action]:RDTCPU
		 *operation done.
		 */
		*mac_learn_result = *pkt_act_result = '\0';
		snprintf(get_cmd, sizeof(get_cmd), "ssdk_sh " SWID_IPQ807X " fdb ptLearnCtrl get %d", p->xgmac_port);
		if (parse_ssdk_sh(get_cmd, "%*[^:]:%[^[]%*[^:]:%s", 2, mac_learn_result, pkt_act_result)) {
			dbg("%s: Execute and parse [%s] failed. (mac_learn_result [%s] pkt_act_result [%s])\n",
				__func__, mac_learn_result, pkt_act_result);
			continue;
		}

		snprintf(xgmac_port, sizeof(xgmac_port), "%d", p->xgmac_port);
		if (enable_ppe) {
			strlcpy(mac_learn, enable_ppe_str[0], sizeof(mac_learn));
			strlcpy(pkt_act, enable_ppe_str[1], sizeof(pkt_act));
		} else {
			strlcpy(mac_learn, disable_ppe_str[0], sizeof(mac_learn));
			strlcpy(pkt_act, disable_ppe_str[1], sizeof(pkt_act));
		}

		/* Don't set setting if it same as current. */
		if (!strcmp(mac_learn, mac_learn_result) && !strcmp(pkt_act, pkt_act_result))
			continue;

		_eval(set_cmd, DBGOUT, 0, NULL);
		flush++;
	}

	if (flush > 0)
		_eval(flush_cmd, DBGOUT, 0, NULL);
}

static void set_ip_alias_for_gpon_module(void)
{
//rax120 don't have sfp
}

void __post_start_lan(void)
{
	char br_if[IFNAMSIZ];

	strlcpy(br_if, nvram_get("lan_ifname")? : nvram_default_get("lan_ifname"), sizeof(br_if));
	set_netdev_sysfs_param(br_if, "bridge/multicast_querier", "1");
	set_netdev_sysfs_param(br_if, "bridge/multicast_snooping",
		nvram_match("switch_br0_no_snooping", "1")? "0" : "1");
	set_ip_alias_for_gpon_module();
}

void __post_start_lan_wl(void)
{
	__post_start_lan();
}

int __sw_based_iptv(void)
{
	/* GT-AXY16000 always use software bridge to implement IPTV feature.
	 * If we support LAN3~8 as upstream port of IPTV and all VoIP/STB
	 * ports on LAN3~8, we can support IPTV by just configure QCA8337
	 * switch.  Return zero if all above conditions true.
	 */
	return 1;
}

/**
 * GT-AXY16000's WAN1~2/LAN1~2 are IPQ807x ports and LAN3~8 are QCA8337 ports.
 * If IPTV upstream port is WAN1~2 and QCA8337 port is selected as STB port,
 * e.g. all ISP IPTV profiles or pure STB port(s) on LAN3/4, or IPTV upstream
 * port is LAN5~8 (not supported yet) and IPQ807X port is selected as STB
 * port, we need to use VLAN to seperate ingress frames of STB port.  VLAN ID
 * is returned by get_sw_bridge_iptv_vid().
 * @return:
 * 	0:	WAN/IPTV ports on same switch or IPTV is not enabled.
 * 	1:	IPTV is enabled, WAN and IPTV port are different switch.
 */
int __sw_bridge_iptv_different_switches(void)
{
	int stb_x = nvram_get_int("switch_stb_x");
	const int upstream_wanif = get_dualwan_by_unit(get_upstream_wan_unit()); /* WANS_DUALWAN_IF_XXX */
	const char *no_lan_port_profiles[] = { "spark", "2degrees", "slingshot", "orcon", "voda_nz",
		"tpg", "iinet", "aapt", "intronode", "amaysim", "dodo", "iprimus", NULL
	}, **p;
	char switch_wantag[65];

	strlcpy(switch_wantag, nvram_safe_get("switch_wantag"), sizeof(switch_wantag));
	if (*switch_wantag == '\0')
		return 0;

	if (stb_x < 0 || stb_x > 6)
		stb_x = 0;

	/* If ISP IPTV profile doesn't need any LAN port, return 0. */
	for (p = &no_lan_port_profiles[0]; *p != NULL; ++p) {
		if (!strcmp(switch_wantag, *p))
			return 0;
	}

	if (upstream_wanif == WANS_DUALWAN_IF_WAN || upstream_wanif == WANS_DUALWAN_IF_WAN2) {
		if (!strcmp(switch_wantag, "none") &&
		    (stb_x == 1 || stb_x == 2 || stb_x == 5))	/* LAN1,2,1+2 */
			return 0;
		return 1;					/* LAN3,4,3+4 and all ISP IPTV profiles */
	} else if (upstream_wanif == WANS_DUALWAN_IF_LAN) {
		if (!strcmp(switch_wantag, "none") &&
		    (stb_x == 1 || stb_x == 2 || stb_x == 5))	/* LAN1,2,1+2 */
			return 1;
		return 0;					/* LAN3,4,3+4 and all ISP IPTV profiles */
	}

	dbg("%s: unknown iptv upstream wanif type [%d]\n", __func__, upstream_wanif);

	return 0;
}

/* Return wan_base_if for start_vlan() and selectable upstream port for IPTV.
 * @wan_base_if:	pointer to buffer, minimal length is IFNAMSIZ.
 * @return:		pointer to base interface name for start_vlan().
 */
char *__get_wan_base_if(char *wan_base_if)
{
	int unit, wanif_type;

	if (!wan_base_if)
		return NULL;

	/* Select upstream port of IPTV profile based on configuration at run-time. */
	*wan_base_if = '\0';
	for (unit = WAN_UNIT_FIRST; *wan_base_if == '\0' && unit < WAN_UNIT_MAX; ++unit) {
		wanif_type = get_dualwan_by_unit(unit);
		if (!upstream_iptv_ifaces[wanif_type] || *upstream_iptv_ifaces[wanif_type] == '\0')
			continue;

#if defined(RTCONFIG_BONDING_WAN)
		if (wanif_type == WANS_DUALWAN_IF_WAN && sw_mode() == SW_MODE_ROUTER && bond_wan_enabled()) {
			strlcpy(wan_base_if, "bond1", IFNAMSIZ);
		} else
#endif
			strlcpy(wan_base_if, upstream_iptv_ifaces[wanif_type], IFNAMSIZ);
	}

	return wan_base_if;
}

/* Return wan unit of upstream port for IPTV.
 * This function must same wan unit that returned by get_wan_base_if()!
 * @return:	wan unit of upstream port of IPTV.
 */
int __get_upstream_wan_unit(void)
{
	int i, wanif_type, unit = -1;

	for (i = WAN_UNIT_FIRST; unit < 0 && i < WAN_UNIT_MAX; ++i) {
		wanif_type = get_dualwan_by_unit(i);
		if (wanif_type != WANS_DUALWAN_IF_WAN &&
		    wanif_type != WANS_DUALWAN_IF_WAN2)
			continue;

		unit = i;
	}

	return unit;
}

/* Return IP address for interface of SFP+ port, it will be used to connect to SFP+ GPON's telnet/web server.
 * @ipaddr:	char array that big enough to save a IPv4 address string.
 * 		If it's NULL, internal buffer is used instead.
 * @return:	Pointer to a buffer that keep IP address for interface of SFP+ port or empty string.
 */
char *calc_sfpp_iface_ipaddr(char *ipaddr, size_t ipaddr_len)
{
	return NULL;
}

/* Return true if interface of SFP+ port is primary WAN or secondary WAN
 * @return:
 * 	0:	Interface of SFP+ port is not in any WAN.
 *  otherwise:	Interface of SFP+ port is in WAN.
 */
int sfpp_iface_in_wan(void)
{
	return 0;
}

/* Add NAT rules that are used to allow LAN/WLAN devices to access telnet/web server of GPON SFP module.
 * @fp:		FILE pointer to REDIRECT_RULES or NAT_RULES
 * @return:
 * 	0:	success
 * 	-1:	error
 */
int add_nat_rule_for_gpon_sfp_module(FILE *fp)
{
	return 0;
}

/* Add filter rules that are used to allow LAN/WLAN devices to access telnet/web server of GPON SFP module.
 * @fp:		FILE pointer to /tmp/filter.default or /tmp/filter_rules
 * @return:
 * 	0:	success
 * 	-1:	error
 */
int add_filter_rule_for_gpon_sfp_module(FILE *fp)
{
	return 0;
}

#if defined(RTCONFIG_BONDING_WAN)
/** Helper function of get_bonding_port_status().
 * Convert bonding slave port definition that is used in wanports_bond to our virtual port definition
 * and get link status/speed of it.
 * @bs_port:	bonding slave port number, 0: WAN, 1~8: LAN1~8, 30: 10G base-T (RJ-45), 31: 10G SFP+
 * @return:
 *  <= 0:	disconnected
 *  otherwise:	link speed
 */
int __get_bonding_port_status(enum bs_port_id bs_port)
{
	int vport, phy, link = 0, speed = 0;

	if (bs_port < 0 || bs_port >= ARRAY_SIZE(bsport_to_vport))
		return 0;

	vport = bsport_to_vport[bs_port];
	if (vport == WAN5GR_PORT && !is_aqr_phy_exist())
		return 0;

	phy = vport_to_phy_addr[vport];
	if (phy < 0) {
		dbg("%s: can't get PHY address of vport %d\n", __func__, vport);
		return 0;
	}
	get_phy_info(phy, &link, NULL, &speed, NULL);

	return link? speed : 0;
}
#endif

#if defined(RTCONFIG_BONDING_WAN) || defined(RTCONFIG_LACP)
/** Convert bs_port_id to interface name.
 * @bs_port:	enum bs_port_id
 * @return:	pointer to interface name or NULL.
 *  NULL:	@bs_port doesn't have interface name or error.
 *  otherwise:	interface name.
 */
const char *bs_port_id_to_iface(enum bs_port_id bs_port)
{
	int vport;

	if (bs_port < 0 || bs_port >= ARRAY_SIZE(bsport_to_vport))
		return NULL;

	vport = bsport_to_vport[bs_port];
	if (vport == WAN5GR_PORT && !is_aqr_phy_exist())
		return NULL;

	return vport_to_iface_name(vport);
}
#endif

void set_jumbo_frame(void)
{
	unsigned int m, vport, mtu = 1500;
	const char *p;
	char ifname[IFNAMSIZ], mtu_iface[sizeof("9000XXX")], mtu_frame[sizeof("9000XX")];
	char *ifconfig_argv[] = { "ifconfig", ifname, "mtu", mtu_iface, NULL };

	if (!nvram_contains_word("rc_support", "switchctrl"))
		return;

	if (nvram_get_int("jumbo_frame_enable"))
		mtu = 9000;

	snprintf(mtu_iface, sizeof(mtu_iface), "%d", mtu);
	snprintf(mtu_frame, sizeof(mtu_frame), "%d", mtu + 18);

	m = get_lan_port_mask();
	for (vport = 0; m > 0 && vport < MAX_WANLAN_PORT ; vport++, m >>= 1) {
		if (!(m & 1) || !(p = vport_to_iface_name(vport)))
			continue;
		strlcpy(ifname, p, sizeof(ifname));
		_eval(ifconfig_argv, NULL, 0, NULL);
	}
}

/* Platform-specific function of wgn_sysdep_swtich_unset()
 * Unconfigure VLAN settings that is used to connect AiMesh guest network.
 * @vid:	VLAN ID
 */
void __wgn_sysdep_swtich_unset(int vid)
{

}

/* Platform-specific function of wgn_sysdep_swtich_set()
 * Unconfigure VLAN settings that is used to connect AiMesh guest network.
 * @vid:	VLAN ID
 */
void __wgn_sysdep_swtich_set(int vid)
{

}

#ifdef RTCONFIG_NEW_PHYMAP
extern int get_trunk_port_mapping(int trunk_port_value)
{
	return trunk_port_value;
}

/* phy port related start */
void get_phy_port_mapping(phy_port_mapping *port_mapping)
{
	static phy_port_mapping port_mapping_static = {
#if defined(RAX120)
		.count = 8,
		.port[0] = { .phy_port_id = LAN1_PORT, .ext_port_id = -1, .label_name = "L1", .cap = PHY_PORT_CAP_LAN, .max_rate = 1000, .ifname = NULL, .flag = 0 },
		.port[1] = { .phy_port_id = LAN2_PORT, .ext_port_id = -1, .label_name = "L2", .cap = PHY_PORT_CAP_LAN, .max_rate = 1000, .ifname = NULL, .flag = 0 },
		.port[2] = { .phy_port_id = LAN3_PORT, .ext_port_id = -1, .label_name = "L3", .cap = PHY_PORT_CAP_LAN, .max_rate = 1000, .ifname = NULL, .flag = 0 },
		.port[3] = { .phy_port_id = LAN4_PORT, .ext_port_id = -1, .label_name = "L4", .cap = PHY_PORT_CAP_LAN, .max_rate = 1000, .ifname = NULL, .flag = 0 },
		.port[4] = { .phy_port_id = WAN_PORT, .ext_port_id = -1, .label_name = "W0", .cap = PHY_PORT_CAP_WAN, .max_rate = 1000, .ifname = NULL, .flag = 0 },
		.port[5] = { .phy_port_id = WAN5GR_PORT, .ext_port_id = -1, .label_name = "W1", .cap = (PHY_PORT_CAP_WAN | PHY_PORT_CAP_WAN2), .max_rate = 5000, .ifname = NULL, .flag = 0 },
		.port[6] = { .phy_port_id = -1, .ext_port_id = -1, .label_name = "U1", .cap = PHY_PORT_CAP_USB, .max_rate = 5000, .ifname = NULL, .flag = 0 },
		.port[7] = { .phy_port_id = -1, .ext_port_id = -1, .label_name = "U2", .cap = PHY_PORT_CAP_USB, .max_rate = 5000, .ifname = NULL, .flag = 0 }

#else
		#error "port_mapping is not defined."
#endif
	};

	if (!port_mapping)
		return;

	memcpy(port_mapping, &port_mapping_static, sizeof(phy_port_mapping));

	add_sw_cap(port_mapping);
	swap_wanlan(port_mapping);
	return;
}

/* phy port related end.*/
#endif

#if defined(RTCONFIG_FRS_FEEDBACK)
void __gen_switch_log(char *fn)
{
	char path[64], *ate_cmd[] = { "ATE", "Get_WanLanStatus", NULL };
	FILE *fp;
	if (!fn || *fn == '\0')
		return;

	snprintf(path, sizeof(path), ">%s", fn);
	_eval(ate_cmd, path, 0, NULL);

	if (!(fp = fopen(fn, "a")))
		return;
	fclose(fp);
}
#endif
