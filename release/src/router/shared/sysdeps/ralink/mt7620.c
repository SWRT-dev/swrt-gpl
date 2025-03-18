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

#include <shutils.h>
#include <shared.h>
#include <utils.h>
#if defined(RTCONFIG_WLMODULE_MT7615E_AP) || defined(RTCONFIG_WLMODULE_MT7915D_AP)
#include <linux/autoconf.h>
#else
#include <linux/config.h>
#endif
#include <ralink.h>
#include "mt7620.h"
#include "ra_ioctl.h"

#define GPIO_DEV	"/dev/gpio0"
#if defined(RTCONFIG_WLMODULE_MT7615E_AP) || defined(RTCONFIG_WLMODULE_MT7915D_AP)
#define ETH_DEVNAME     "eth1"
#else
#define ETH_DEVNAME     "eth2"
#endif
#define NR_WANLAN_PORT	5
#if defined(RTCONFIG_RALINK_MT7621)
#define dv 0
#else //MT7620
#define dv 1
#endif

struct wifi_if_vid_s {
	int wl_vid[2];
	int wl_wds_vid[2];
};

enum {
	gpio_in,
	gpio_out,
};

#if defined(RTN14U) || defined(RTAC51U) ||defined(RTN54U) || defined(RTAC54U)
/// RT-N14U mapping
enum {
	WAN_PORT=0,
	LAN1_PORT=1,
	LAN2_PORT=2,
	LAN3_PORT=3,
	LAN4_PORT=4,
	P5_PORT=5,
	CPU_PORT=6,
	P7_PORT=7,
};
#elif defined(RTAC52U)
/// RT-AC52U mapping
enum {
	WAN_PORT=0,
	LAN1_PORT=3,
	LAN2_PORT=4,
	LAN3_PORT=2,
	LAN4_PORT=1,
	P5_PORT=5,
	CPU_PORT=6,
	P7_PORT=7,
};
#define PORT_W4321

#elif defined(RTN11P) || defined(RTN300)
/// RT-N11P/RTN300 mapping
enum {
	WAN_PORT=4,
	LAN1_PORT=3,
	LAN2_PORT=2,
	LAN3_PORT=1,
	LAN4_PORT=0,
	P5_PORT=5,
	CPU_PORT=6,
	P7_PORT=7,
};
#elif defined(RTAC1200HP)
enum {
	WAN_PORT=5,
	LAN1_PORT=0,
	LAN2_PORT=1,
	LAN3_PORT=2,
	LAN4_PORT=3,
	P5_PORT=4,
	CPU_PORT=6,
	P7_PORT=7,
};
#elif defined(RTN56UB1) || defined(RTN56UB2) || defined(RTAC65U)
enum {
	WAN_PORT=4,
	LAN1_PORT=3,
	LAN2_PORT=2,
	LAN3_PORT=1,
	LAN4_PORT=0,
	P5_PORT=5,
	CPU_PORT=6,
	P7_PORT=7,
};
#define MT7621_GSW
#elif defined(RTAC1200GA1)
enum {
	WAN_PORT=0,
	LAN1_PORT=1,
	LAN2_PORT=2,
	LAN3_PORT=3,
	LAN4_PORT=4,
	P5_PORT=5,
	CPU_PORT=6,
	P7_PORT=7,
};
#define MT7621_GSW
#elif defined(RTAC1200GU)
enum {
	WAN_PORT=0,
	LAN1_PORT=1,
	LAN2_PORT=2,
	LAN3_PORT=3,
	LAN4_PORT=4,
	P5_PORT=5,
	CPU_PORT=6,
	P7_PORT=7,
};
#define MT7621_GSW
#elif defined(RPAC87)
enum {
	WAN_PORT=4,
	LAN1_PORT=3,
	LAN2_PORT=2,
	LAN3_PORT=1,
	LAN4_PORT=0,
	P5_PORT=5,
	CPU_PORT=6,
	P7_PORT=7,
};
#elif defined(RTAC85U) || defined(PGBM1)
enum {
	WAN_PORT=4,
	LAN1_PORT=0,
	LAN2_PORT=1,
	LAN3_PORT=2,
	LAN4_PORT=3,
	P5_PORT=5,
	CPU_PORT=6,
	P7_PORT=7,
};
#define MT7621_GSW
#elif defined(RTAC85P)  || defined(RTACRH26) || defined(TUFAC1750) || defined(RT4GAX56) || defined(RTAX53U)
enum {
	WAN_PORT=0,
	LAN1_PORT=1,
	LAN2_PORT=2,
	LAN3_PORT=3,
	LAN4_PORT=4,
	P5_PORT=5,
	CPU_PORT=6,
	P7_PORT=7,
};
#define MT7621_GSW
#elif defined(RMAC2100)
enum {
	WAN_PORT=0,
	LAN1_PORT=2,
	LAN2_PORT=3,
	LAN3_PORT=4,
	LAN4_PORT=1,
	P5_PORT=5,
	CPU_PORT=6,
	P7_PORT=7,
};
#define MT7621_GSW
#elif defined(R6800)
enum {
	WAN_PORT=4,
	LAN1_PORT=3,
	LAN2_PORT=2,
	LAN3_PORT=1,
	LAN4_PORT=0,
	P5_PORT=5,
	CPU_PORT=6,
	P7_PORT=7,
};
#define MT7621_GSW
#elif defined(JCGQ10PRO) || defined(JCGQ20)
enum {
	WAN_PORT=0,
	LAN1_PORT=1,
	LAN2_PORT=4,
	LAN3_PORT=2,
	LAN4_PORT=3,
	P5_PORT=5,
	CPU_PORT=6,
	P7_PORT=7,
};
#define MT7621_GSW
#elif defined(CMCCA9)
enum {
	WAN_PORT=1,
	LAN1_PORT=4,
	LAN2_PORT=3,
	LAN3_PORT=2,
	LAN4_PORT=0,
	P5_PORT=5,
	CPU_PORT=6,
	P7_PORT=7,
};
#define MT7621_GSW
#elif defined(H3CTX1801)
enum {
	WAN_PORT=4,
	LAN1_PORT=1,
	LAN2_PORT=2,
	LAN3_PORT=3,
	LAN4_PORT=0,
	P5_PORT=5,
	CPU_PORT=6,
	P7_PORT=7,
};
#define MT7621_GSW
#elif  defined(XMCR660X)
enum {
	WAN_PORT=4,
	LAN1_PORT=3,
	LAN2_PORT=2,
	LAN3_PORT=1,
	LAN4_PORT=0,
	P5_PORT=5,
	CPU_PORT=6,
	P7_PORT=7,
};
#define MT7621_GSW
#elif defined(XD4S)
enum {
	WAN_PORT=4,
	LAN1_PORT=2,
	LAN2_PORT=0, //not used
	LAN3_PORT=1, //not used
	LAN4_PORT=3, //not used
	P5_PORT=5,
	CPU_PORT=6,
	P7_PORT=7,
};
#define MT7621_GSW
#elif defined(RTAX54)
enum {
	WAN_PORT=4,
	LAN1_PORT=3,
	LAN2_PORT=2,
	LAN3_PORT=1,
	LAN4_PORT=0,
	P5_PORT=5,
	CPU_PORT=6,
	P7_PORT=7,
};
#define MT7621_GSW
#elif defined(RTN800HP)
enum {
	WAN_PORT=4,
	LAN1_PORT=3,
	LAN2_PORT=2,
	LAN3_PORT=1,
	LAN4_PORT=0,
	P5_PORT=5,
	CPU_PORT=6,
	P7_PORT=7,
};
#define MT7621_GSW
#endif

#define BIT(n)	(1 << (n))
/* 0: LAN, 1:WAN or STB */
static const int lan_wan_partition[] = {
#ifdef MT7621_GSW
	BIT( WAN_PORT ) | BIT( P5_PORT ),					// WAN
	BIT( WAN_PORT ) | BIT( P5_PORT ) | BIT( LAN1_PORT ),			// WAN + LAN1
	BIT( WAN_PORT ) | BIT( P5_PORT ) | BIT( LAN2_PORT ),			// WAN + LAN2
	BIT( WAN_PORT ) | BIT( P5_PORT ) | BIT( LAN3_PORT ),			// WAN + LAN3
	BIT( WAN_PORT ) | BIT( P5_PORT ) | BIT( LAN4_PORT ),			// WAN + LAN4
	BIT( WAN_PORT ) | BIT( P5_PORT ) | BIT( LAN1_PORT ) | BIT( LAN2_PORT ),	// WAN + LAN1+2
#if defined(RTCONFIG_3LANPORT_DEVICE)
	BIT( WAN_PORT ) | BIT( P5_PORT ) | BIT( LAN2_PORT ) | BIT( LAN3_PORT ),	// WAN + LAN2+3
#else
	BIT( WAN_PORT ) | BIT( P5_PORT ) | BIT( LAN3_PORT ) | BIT( LAN4_PORT ),	// WAN + LAN3+4
#endif
#else
	BIT( WAN_PORT ),					// WAN
	BIT( WAN_PORT ) | BIT( LAN1_PORT ),			// WAN + LAN1
	BIT( WAN_PORT ) | BIT( LAN2_PORT ),			// WAN + LAN2
	BIT( WAN_PORT ) | BIT( LAN3_PORT ),			// WAN + LAN3
	BIT( WAN_PORT ) | BIT( LAN4_PORT ),			// WAN + LAN4
	BIT( WAN_PORT ) | BIT( LAN1_PORT ) | BIT( LAN2_PORT ),	// WAN + LAN1+2
	BIT( WAN_PORT ) | BIT( LAN3_PORT ) | BIT( LAN4_PORT ),	// WAN + LAN3+4
#endif
	0,							// ALL LAN
};

/* Final model-specific LAN/WAN/WANS_LAN partition definitions.
 * bit0: P0, bit1: P1, bit2: P2, bit3: P3, bit4: P4
 */
static unsigned int lan_mask = 0;	/* LAN only. Exclude WAN, WANS_LAN, and generic IPTV port. */
static unsigned int wan_mask = 0;	/* wan_type = WANS_DUALWAN_IF_WAN. Include generic IPTV port. */
static unsigned int wans_lan_mask = 0;	/* wan_type = WANS_DUALWAN_IF_LAN. */

/* RT-N56U's P0, P1, P2, P3, P4 = LAN4, LAN3, LAN2, LAN1, WAN
 * ==> Model-specific port number.
 */
static int switch_port_mapping[] = {
	LAN4_PORT,	//0000 0000 0001 LAN4
	LAN3_PORT,	//0000 0000 0010 LAN3
	LAN2_PORT,	//0000 0000 0100 LAN2
	LAN1_PORT,	//0000 0000 1000 LAN1
	WAN_PORT,	//0000 0001 0000 WAN
	P5_PORT,	//0000 0010 0000 -
	P5_PORT,	//0000 0100 0000 -
	P5_PORT,	//0000 1000 0000 -
	CPU_PORT,	//0001 0000 0000 RGMII LAN
	P7_PORT,	//0010 0000 0000 RGMII WAN
};

int esw_fd;

/* Model-specific LANx ==> Model-specific PortX mapping */
const int lan_id_to_port_mapping[NR_WANLAN_PORT] = {
	WAN_PORT,	/* not used */
	LAN1_PORT,
	LAN2_PORT,
	LAN3_PORT,
	LAN4_PORT,
};

/* Model-specific LANx ==> Model-specific PortX */
static inline int lan_id_to_port_nr(int id)
{
	return lan_id_to_port_mapping[id];
}

/**
 * Get WAN port mask
 * @wan_unit:	wan_unit, if negative, select WANS_DUALWAN_IF_WAN
 * @return:	port bitmask
 */
static unsigned int get_wan_port_mask(int wan_unit)
{
	char nv[] = "wanXXXports_maskXXXXXX";

	if (sw_mode() == SW_MODE_REPEATER)
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

	if ((!__aimesh_re_node(sw_mode)) && (sw_mode == SW_MODE_AP || __mediabridge_mode(sw_mode)))
		m = 0x1F;

#if defined(RTCONFIG_RALINK_MT7621)
	#ifdef RTCONFIG_CONCURRENTREPEATER
	if (sw_mode == SW_MODE_REPEATER)
		m = 0x3F;
	#endif
#endif
	return m;
}

/**
 * Create string based portmap base on mask parameter.
 * @mask:	port bit mask.
 * 		bit0: P0, bit1: P1, etc
 * @portmap:	pointer to char array. minimal length is 9 bytes.
 */
static void __create_port_map(unsigned int mask, char *portmap)
{
	int i;
	char *p;
	unsigned int m;

	for (i = 0, m = mask, p = portmap; i < 8; ++i, m >>= 1, ++p)
		*p = '0' + (m & 1);
	*p = '\0';
}

int switch_init(void)
{
	esw_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (esw_fd < 0) {
		perror("socket");
		return -1;
	}
	return 0;
}

void switch_fini(void)
{
	close(esw_fd);
}

#if defined(RTCONFIG_RALINK_MT7620)
int mt7620_reg_read(int offset, unsigned int *value)
#elif defined(RTCONFIG_RALINK_MT7621)
int mt7621_esw_read(int offset, unsigned int *value)
#endif
{
	struct ifreq ifr;
	esw_reg reg;

	if (value == NULL)
		return -1;
	reg.off = offset;
	strlcpy(ifr.ifr_name, ETH_DEVNAME, IFNAMSIZ);
	ifr.ifr_data = (void*) &reg;
	if (-1 == ioctl(esw_fd, RAETH_ESW_REG_READ, &ifr)) {
		_dprintf("%s: read esw register fail. errno %d (%s)\n", __func__, errno, strerror(errno));
		close(esw_fd);
		return -1;
	}
	*value = reg.val;

	//printf("mt7620_reg_read()...offset=%4x, value=%8x\n", offset, *value);

	return 0;
}

#if defined(RTCONFIG_RALINK_MT7621)
int mt7621_reg_read(int offset, unsigned int *value)
{
         struct ifreq ifr;
         esw_reg reg;
         ra_mii_ioctl_data mii;
         strlcpy(ifr.ifr_name, ETH_DEVNAME, IFNAMSIZ);
         ifr.ifr_data = &mii;
 
         mii.phy_id = 0x1f;
         mii.reg_num = offset;
 
         if (-1 == ioctl(esw_fd, RAETH_MII_READ, &ifr)) {
              perror("ioctl");
              close(esw_fd);
              return -1;
         }
         *value = mii.val_out;
	//printf("mt7621_reg_read()...offset=%4x, value=%8x\n", offset, *value);
         return 0;
}   

#if defined(RTCONFIG_WLMODULE_MT7615E_AP) || defined(RTCONFIG_WLMODULE_MT7915D_AP)
int mt7621_phy_read(int offset, unsigned int *value)
{
         struct ifreq ifr;
         esw_reg reg;
         ra_mii_ioctl_data mii;
         strlcpy(ifr.ifr_name, ETH_DEVNAME, IFNAMSIZ);
         ifr.ifr_data = &mii;
 
         mii.phy_id = offset;
         mii.reg_num = 1;
 
         if (-1 == ioctl(esw_fd, RAETH_MII_READ, &ifr)) {
              perror("ioctl");
              close(esw_fd);
              return -1;
         }
         *value = mii.val_out;
	 return 0;
}   
#endif
#endif

#if defined(RTCONFIG_RALINK_MT7620)
int mt7620_reg_write(int offset, int value)
{
	struct ifreq ifr;
	esw_reg reg;

	//printf("mt7620_reg_write()..offset=%4x, value=%8x\n", offset, value);

	reg.off = offset;
	reg.val = value;
	strlcpy(ifr.ifr_name, "eth2", IFNAMSIZ);
	ifr.ifr_data = (void*) &reg;
	if (-1 == ioctl(esw_fd, RAETH_ESW_REG_WRITE, &ifr)) {
		perror("ioctl");
		close(esw_fd);
		exit(0);
	}
	return 0;
}
#elif defined(RTCONFIG_RALINK_MT7621)
int mt7621_reg_write(int offset, int value)
{
	struct ifreq ifr;
	esw_reg reg;
	ra_mii_ioctl_data mii;
     
	//printf("mt7621_reg_write()..offset=%4x, value=%8x\n", offset, value);
	strlcpy(ifr.ifr_name, ETH_DEVNAME, IFNAMSIZ);
	ifr.ifr_data = &mii;
	mii.phy_id = 0x1f;
	mii.reg_num = offset;
	mii.val_in = value;
    
	if (-1 == ioctl(esw_fd, RAETH_MII_WRITE, &ifr)) {
		perror("ioctl");
		close(esw_fd);
		return -1;
	}
	return 0;
}	
#endif
#if defined(RTCONFIG_WLMODULE_MT7615E_AP) || defined(RTCONFIG_WLMODULE_MT7915D_AP)
int mt7621_phy_write(int offset, int reg, int value)
{
	struct ifreq ifr;
	ra_mii_ioctl_data mii;

	//printf("mt7621_phy_write()..offset=%4x, value=%8x\n", offset, value);
	strlcpy(ifr.ifr_name, ETH_DEVNAME, IFNAMSIZ);
	ifr.ifr_data = &mii;
	mii.phy_id = offset;
	mii.reg_num = reg;
	mii.val_in = value;

	if (-1 == ioctl(esw_fd, RAETH_MII_WRITE, &ifr)) {
		perror("ioctl");
		close(esw_fd);
		return -1;
	}
	return 0;
}
#endif 

static void write_VTCR(unsigned int value)
{
	int i;
	unsigned int check;

	value |= 0x80000000;
#if defined(RTCONFIG_RALINK_MT7620)
	mt7620_reg_write(REG_ESW_VLAN_VTCR, value);
#elif defined(RTCONFIG_RALINK_MT7621)
	mt7621_reg_write(REG_ESW_VLAN_VTCR, value);
#endif	
	for (i = 0; i < 20; i++) {
#if defined(RTCONFIG_RALINK_MT7620)	   
		mt7620_reg_read(REG_ESW_VLAN_VTCR, &check);
#elif defined(RTCONFIG_RALINK_MT7621)
		mt7621_reg_read(REG_ESW_VLAN_VTCR, &check);
#endif		
		if ((check & 0x80000000) == 0 ) //table busy
			break;
		usleep(1000);
	}
	if (i == 20)
	{
		cprintf("VTCR timeout.\n");
	}
	else if(check & (1<<16))
	{
		cprintf("%s(%08x) invalid\n", __func__, value);
	}
}

/**
 * Find first available vlan slot or find vlan slot by vid.
 * VLAN slot 0~2 are reserved.
 * If VLAN ID is not equal to index + 1, it is assumed unavailable.
 * @vid:
 * 	>0:	Find vlan slot by vid.
 * 	<=0:	Find first available vlan slot.
 * @vawd1:	pointer to a unsigned integer.
 * 		If vlan slot found and vawd1 is not null, save VAWD1 value here.
 * @return:
 * 	0~15:	vlan slot index.
 * 	<0:	not found
 */
#if defined(RTCONFIG_RALINK_MT7621)
int mt7621_vtim[8][2]={{0,1},{2,3},{6,7},{8,9},{10,11},{12,13},{14,15},{16,17}};
#endif
static int find_vlan_slot(int vid, unsigned int *vawd1)
{
	int idx;
	unsigned int i, r, v, value;

#if defined(RTCONFIG_RALINK_MT7620)
	i = 0;				/* search vlan index from 0 */
#elif defined(RTCONFIG_RALINK_MT7621)
	i = 4;				/* skip vlan index 1~3. MT7621 donot use vid=0 */
#endif	/* RTCONFIG_RALINK_MT7621 */

	for(idx = -1; idx < 0 && i < 16; ++i) {
#if defined(RTCONFIG_RALINK_MT7620)
		if ((r = mt7620_reg_read(REG_ESW_VLAN_ID_BASE + 4*(i>>1), &value))) {
			_dprintf("read VTIM1~8, i %d offset %x fail. (r = %d)\n", i, REG_ESW_VLAN_ID_BASE + 4*(i>>1), r);
			continue;
		}

		if (!(i&1))
			v = value & 0xfff;
		else
			v = (value >> 12) & 0xfff;		

		if ((vid <= 0 && v != (i + 1)) && (vid > 0 && v != vid))
			continue;
#elif defined(RTCONFIG_RALINK_MT7621)
		if (!(i&1))
			v=mt7621_vtim[(i>>1)][0];
		else
		   	v=mt7621_vtim[(i>>1)][1];
		if(v<1 || v>4094) //MT7621 don't use 0 and 4095 for VID
		{
			_dprintf("illegal vtim value\n");
			 continue;
		}

		if ((vid <= 0 && v != (i + 2)) && (vid > 0 && v != vid))
			continue;

#endif		


		value = (0x0 << 12) + i; //read VAWD#
		write_VTCR(value);
#if defined(RTCONFIG_RALINK_MT7620)		
		mt7620_reg_read(REG_ESW_VLAN_VAWD1, &value);
		if ((vid <= 0 ) && v == (i + 1) && !(value & 1))	/* find available vlan slot */
#elif defined(RTCONFIG_RALINK_MT7621)
		mt7621_reg_read(REG_ESW_VLAN_VAWD1, &value);
		if ((vid <= 0 ) && v == (i + 2) && !(value & 1))	/* find available vlan slot */
#endif		
			idx = i;
		else if (vid > 0 && v == vid)				/* find vlan slot by vid */
			idx = i;

		if (idx >= 0 && vawd1)
			*vawd1 = value;
	}

	return idx;
}

/**
 * Set a VLAN
 * @idx:	VLAN table index
 *  >= 0:	specify VLAN table index.
 *  <  0:	find available VLAN table.
 * @vid:	VLAN ID
 * @portmap:	Port member. string length must greater than or equal to 8.
 * 		Only '0' and '1' are accepted.
 * 		First digit means port 0, second digit means port1, etc.
 * 		P0, P1, P2, P3, P4, P5, P6, P7
 * @stag:	Service tag
 * @return
 * 	0:	success
 *     -1:	no vlan entry available
 *     -2:	invalid parameter
 */
int mt762x_vlan_set(int idx, int vid, char *portmap, int stag, int untag)
{
	unsigned int i, mbr, value, value2, vawd1;
#if defined(RTCONFIG_RALINK_MT7621)
	idx++;
	if (idx <= 0) { //auto
#else	   
	if (idx < 0) { //auto
#endif	   
		if ((idx = find_vlan_slot(vid, &vawd1)) < 0 && (idx = find_vlan_slot(-1, &vawd1)) < 0) {
			_dprintf("%s: no empty vlan entry for vid %d portmap %s stag %d!\n",
				__func__, vid, portmap, stag);
			return -1;
		}
	}

	_dprintf("%s: idx=%d, vid=%d, portmap=%s, stag=%d\n", __func__, idx, vid, portmap, stag);

	//set vlan identifier
#if defined(RTCONFIG_RALINK_MT7620)
	mt7620_reg_read(REG_ESW_VLAN_ID_BASE + 4*(idx/2), &value);	
	if (idx % 2 == 0) {
		value &= 0xfff000;
		value |= vid;
	}
	else {
		value &= 0xfff;
		value |= (vid << 12);
	}

	mt7620_reg_write(REG_ESW_VLAN_ID_BASE + 4*(idx/2), value);
#elif defined(RTCONFIG_RALINK_MT7621)
	mt7621_vtim[(idx>>1)][!(idx&1)]=vid;
#endif	

	//set vlan member
	mbr = 0;
	for (i = 0; i < 8; i++)
		mbr += (*(portmap + i) - '0') * (1 << i);
	value = (mbr << 16);		//PORT_MEM
	value |= (1 << 30);		//IVL
	value |= (1 << 27);		//COPY_PRI
	value |= ((stag & 0xfff) << 4);	//S_TAG
	value |= 1;			//VALID

	value2 = 0;
	if (untag >= 0) {
		value |= (1 << 28);	//VTAG_EN
		for (i = 0; i < 8; i++) {
			if ((mbr & (1 << i)) == 0)
				continue;
			if (untag & (1 << i))
				value2 |= 0x0 << (i * 2); //Untag
			else
				value2 |= 0x2 << (i * 2); //Tag
		}
	}

#if defined(RTCONFIG_RALINK_MT7620)	
	mt7620_reg_write(REG_ESW_VLAN_VAWD1, value);
	mt7620_reg_write(REG_ESW_VLAN_VAWD2, value2);
	value = (0x80001000 + idx); //w_vid_cmd
#elif defined(RTCONFIG_RALINK_MT7621)	
	mt7621_reg_write(REG_ESW_VLAN_VAWD1, value);
	mt7621_reg_write(REG_ESW_VLAN_VAWD2, value2);
	value = (0x80001000 + vid); //w_vid_cmd
#endif	
	write_VTCR(value);

	return 0;
}

#if defined(RTCONFIG_RALINK_MT7620)
int mt7620_vlan_set(int idx, int vid, char *portmap, int stag)
{
	return mt762x_vlan_set(idx, vid, portmap, stag, -1);
}
#elif defined(RTCONFIG_RALINK_MT7621)
int mt7621_vlan_set(int idx, int vid, char *portmap, int stag)
{
	return mt762x_vlan_set(idx, vid, portmap, stag, -1);
}
#endif

/**
 * Disable a VLAN by vid and restore VID to initial value.
 * @vid:	VLAN ID to be deleted.
 * @return:
 * 	0:	success
 */
#if defined(RTCONFIG_RALINK_MT7620)     
int mt7620_vlan_unset(int vid)
#elif defined(RTCONFIG_RALINK_MT7621)     
int mt7621_vlan_unset(int vid)
#endif   
{
	int idx = -1;
	unsigned int value, vawd1;

	if ((idx = find_vlan_slot(vid, &vawd1)) < 0)
		return -1;

	//_dprintf("%s: delete vid=%d at idx=%d\n", __func__, vid, idx, vid);

	/* disable VLAN */
#if defined(RTCONFIG_RALINK_MT7620)     
	mt7620_reg_write(REG_ESW_VLAN_VAWD1, 0);
	value = (0x80001000 + idx); //w_vid_cmd
#elif defined(RTCONFIG_RALINK_MT7621)     
	mt7621_reg_write(REG_ESW_VLAN_VAWD1, 0);
	value = (0x80001000 + vid); //w_vid_cmd
#endif	
	write_VTCR(value);

	/* restore vlan identifier */
#if defined(RTCONFIG_RALINK_MT7620)     
	mt7620_reg_read(REG_ESW_VLAN_ID_BASE + 4*(idx/2), &value);
	if (idx % 2 == 0) {
		value &= 0xfff000;
		value |= idx+1;
	}
	else {
		value &= 0xfff;
		value |= ((idx+1) << 12);
	}

	mt7620_reg_write(REG_ESW_VLAN_ID_BASE + 4*(idx/2), value);
#elif defined(RTCONFIG_RALINK_MT7621)     	
	mt7621_vtim[(idx>>1)][!(idx&1)]=idx+2;
#endif	
	return 0;
}


#if defined(RTCONFIG_WLMODULE_MT7615E_AP) || defined(RTCONFIG_WLMODULE_MT7915D_AP)
 /**
 * Get TX or RX byte count of WAN and WANS_LAN
 * @unit:	WAN unit.
 * @tx:
 * @rx:
 * @return:
 * 	-1:	invalid parameter.
 * 	0:	success
 */
#if defined(RTCONFIG_RALINK_MT7620)             
int __mt7620_wan_bytecount(int unit, unsigned long long *tx, unsigned long long *rx)
#elif defined(RTCONFIG_RALINK_MT7621)             
int __mt7621_wan_bytecount(int unit, unsigned long long *tx, unsigned long long *rx)
#endif   
{
	int dir, i;
	unsigned long long count[2];
	unsigned int m, v, addr;

	if (unit < 0 || unit >= WAN_UNIT_MAX || !tx || !rx) {
		_dprintf("%s: invalid parameter! (%d, %p, %p)", unit, tx, rx);
		return -1;
	}
 
	if (switch_init() < 0)
		return -1;

	for (dir = 0; dir <= 1; ++dir) {
		count[dir] = 0;
		addr = dir? REG_ESW_PORT_RGOCN_P0:REG_ESW_PORT_TGOCN_P0;
		m = get_wan_port_mask(unit) & ((1U << (NR_WANLAN_PORT+dv)) - 1);
		for (i = 0; m && i < (NR_WANLAN_PORT+dv); ++i, m >>= 1, addr += 0x100) {
			if (!(m & 1))
				continue;

			v = 0;
#if defined(RTCONFIG_RALINK_MT7620)
			mt7620_reg_read(addr, &v);
#elif defined(RTCONFIG_RALINK_MT7621)
			mt7621_reg_read(addr, &v);
#endif			
			count[dir] += v;
		}
	}

	switch_fini();

	*tx = count[0];
	*rx = count[1];

	return 0;
}   
#endif

/**
 * Get linkstatus in accordance with port bit-mask.
 * @mask:	port bit-mask.
 * 		bit0 = P0, bit1 = P1, etc.
 * @linkStatus:	link status of all ports that is defined by mask.
 * 		If one port of mask is linked-up, linkStatus is true.
 */
#if defined(RTCONFIG_RALINK_MT7620)
static void get_mt7620_esw_phy_linkStatus(unsigned int mask, unsigned int *linkStatus)
#elif defined(RTCONFIG_RALINK_MT7621)
static void get_mt7621_esw_phy_linkStatus(unsigned int mask, unsigned int *linkStatus)
#endif   
{
	int i;
	unsigned int value = 0, m;

	if (switch_init() < 0)
		return;

	m = mask & ((1U << (NR_WANLAN_PORT+dv)) - 1);
	for (i = 0; m && !value && i < (NR_WANLAN_PORT+dv); ++i, m >>= 1) {
		if (!(m & 1))
			continue;
#if defined(RTCONFIG_RALINK_MT7620)
		mt7620_reg_read((REG_ESW_MAC_PMSR_P0 + 0x100*i), &value);
		value &= 0x1;
#elif defined(RTCONFIG_RALINK_MT7621)
//#if defined(RTAC85U) || defined(RTAC85P) || defined(RTN800HP)  || defined(RTACRH26) || defined(TUFAC1750)
#if 0
		mt7621_phy_read(i, &value);
		value = (value >> 2) & 0x1;
#else
		mt7621_reg_read((REG_ESW_MAC_PMSR_P0 + 0x100*i), &value);
		value &= 0x1;
#endif
#endif
	}
	*linkStatus = value;

	switch_fini();
}

static void build_wan_lan_mask(int stb)
{
	int unit;
	int wanscap_lan = get_wans_dualwan() & WANSCAP_LAN;
	int wans_lanport = nvram_get_int("wans_lanport");
	int sw_mode = sw_mode();
	char prefix[8], nvram_ports[20];

	if (sw_mode == SW_MODE_AP || sw_mode == SW_MODE_REPEATER)
		wanscap_lan = 0;

	if (stb == 100 && (sw_mode == SW_MODE_AP || __mediabridge_mode(sw_mode)))
		stb = 7;	/* Don't create WAN port. */

	if (wanscap_lan && (wans_lanport < 0 || wans_lanport > 4)) {
		_dprintf("%s: invalid wans_lanport %d!\n", __func__, wans_lanport);
		wanscap_lan = 0;
	}

	lan_mask = wan_mask = wans_lan_mask = 0;
	if(stb < 0 || stb >= ARRAY_SIZE(lan_wan_partition))
	{
		_dprintf("%s: invalid partition index: %d\n", __func__, stb);
		stb = 0;
	}

	wan_mask = lan_wan_partition[stb];	
#if defined(RTCONFIG_RALINK_MT7621)
		//discard port 5
	if (sw_mode == SW_MODE_AP || sw_mode == SW_MODE_REPEATER)
		lan_mask = ((1<<(NR_WANLAN_PORT)) -1) & ~lan_wan_partition[stb];
	else
#endif		//for MT7621, port 5 is the part of wan-ports
		lan_mask = ((1<<(NR_WANLAN_PORT+1)) -1) & ~lan_wan_partition[stb];
	//DUALWAN
	if (wanscap_lan) {
		wans_lan_mask = 1U << lan_id_to_port_nr(wans_lanport);
		lan_mask &= ~wans_lan_mask;
	}

	for (unit = WAN_UNIT_FIRST; unit < WAN_UNIT_MAX; ++unit) {
		snprintf(prefix, sizeof(prefix), "%d", unit);
		snprintf(nvram_ports, sizeof(nvram_ports), "wan%sports_mask", (unit == WAN_UNIT_FIRST)?"":prefix);

		if (get_dualwan_by_unit(unit) == WANS_DUALWAN_IF_WAN) {
			nvram_set_int(nvram_ports, wan_mask);
		}
		else if (get_dualwan_by_unit(unit) == WANS_DUALWAN_IF_LAN) {
			nvram_set_int(nvram_ports, wans_lan_mask);
		}
		else
			nvram_unset(nvram_ports);
	}
	nvram_set_int("lanports_mask", lan_mask);
}

void set_switch_pvid(int port, int pvid)
{
	unsigned int reg = 0;
	unsigned int value = 0;

	if (switch_init() < 0)
		return;

	reg = REG_ESW_PORT_PPBV1_P0 + port * 0x100;

#if defined(RTCONFIG_RALINK_MT7620)
	mt7620_reg_read(reg, &value);
#elif defined(RTCONFIG_RALINK_MT7621)
	mt7621_reg_read(reg, &value);
#endif

	value &= 0xfffff000;
	value |= pvid;

#if defined(RTCONFIG_RALINK_MT7620)
	mt7620_reg_write(reg, value);
#elif defined(RTCONFIG_RALINK_MT7621)
	mt7621_reg_write(reg, value);
#endif

	switch_fini();
}

void set_switch_isolate(unsigned int portmask)
{
#define MAX_PORTMAP 16

	int i, j;
    unsigned int n = 0;
    unsigned int v = 0;
    unsigned int offset = 0;
    unsigned int value = 0;

	if (switch_init() < 0)
		return;

	for (i=0; i<MAX_PORTMAP; i++) {
		if (portmask & (1<<i)) {
			offset = REG_ESW_PORT_PCR_P0 + i * 0x100;
#if defined(RTCONFIG_RALINK_MT7620)
			mt7620_reg_read(offset, &value);
#elif defined(RTCONFIG_RALINK_MT7621)
			mt7621_reg_read(offset, &value);
#endif

			for (j=0; j<MAX_PORTMAP; j++) {
				if (portmask & (1<<j)) {
					v = 1 << (j+16);
					value &= ~v;
				}
			}

#if defined(RTCONFIG_RALINK_MT7620)
			mt7620_reg_write(offset, value);
#elif defined(RTCONFIG_RALINK_MT7621)
			mt7621_reg_write(offset, value);
#endif
		}
	}

	switch_fini();
	return;	

}

void unset_switch_vlan(int vid, unsigned int portmask)
{
	unsigned int value = 0;
	char portmap[MAX_PORTMAP+1];

	if (switch_init() < 0)
		return;

	build_wan_lan_mask(0);
	// detele VLAN
#if defined(RTCONFIG_RALINK_MT7620)     
	mt7620_reg_write(REG_ESW_VLAN_VAWD1, 0);
	value = (0x80001000 + vid); //w_vid_cmd
#elif defined(RTCONFIG_RALINK_MT7621)     
	mt7621_reg_write(REG_ESW_VLAN_VAWD1, 0);
	value = (0x80001000 + vid); //w_vid_cmd
#endif	
	write_VTCR(value);

	// initialize Vlan
	memset(portmap, 0, sizeof(portmap));
	__create_port_map(0xC0 | lan_mask, portmap);	
	mt7621_vlan_set(0, 1, portmap, 1);

	switch_fini();
}
/* bit 0:1
b00 : admit all frames
b01 : admit only vlan-tagged frames
b10 : admit only untagged or priority-tagged frames
b11 : reserved
*/
void set_acceptable_frame_type(int port, int type)
{
	unsigned int value;
	unsigned int reg;

	reg = REG_ESW_PORT_PVC_P0 + 0x100*port;

	if (switch_init() < 0)
		return;

#if defined(RTCONFIG_RALINK_MT7620)
	mt7620_reg_read(reg, &value);
#elif defined(RTCONFIG_RALINK_MT7621)
	mt7621_reg_read(reg, &value);
#endif

	value &= 0xfffffffc;
	value |= type;
#if defined(RTCONFIG_RALINK_MT7620)
	mt7620_reg_write(reg, value);
#elif defined(RTCONFIG_RALINK_MT7621)
	mt7621_reg_write(reg, value);
#endif
	switch_fini();
}

void set_admit_all_frames()
{
	unsigned int value;
	for (int i = 0; i <= NR_WANLAN_PORT; i++)
		set_acceptable_frame_type(i, 0);
}

void set_admit_untag_frames()
{
	unsigned int value;
	for (int i = 0; i <= NR_WANLAN_PORT; i++)
		set_acceptable_frame_type(i, 2);
}

/*
type
0: user port
1: statck port
2: translation port
3: transparent port
*/
void set_switch_vlan_port_attribute(int port, int type)
{
	unsigned int value;
	unsigned int reg;

	reg = REG_ESW_PORT_PVC_P0 + 0x100*port;

	if (switch_init() < 0)
		return;

#if defined(RTCONFIG_RALINK_MT7620)
	mt7620_reg_read(reg, &value);
#elif defined(RTCONFIG_RALINK_MT7621)
	mt7621_reg_read(reg, &value);
#endif

	value &= (0xffffff3f);
	value |= (type << 6);
#if defined(RTCONFIG_RALINK_MT7620)
	mt7620_reg_write(reg, value);
#elif defined(RTCONFIG_RALINK_MT7621)
	mt7621_reg_write(reg, value);
#endif
	switch_fini();
}

/*
eg_tag
0: untagged
1: swap
2: tagged
3: stack
*/
void set_switch_vlan_egresstag(int port, int eg_tag)
{
	unsigned int value;
	unsigned int reg;

	reg = REG_ESW_PORT_PCR_P0 + 0x100*port;

	if (switch_init() < 0)
		return;

#if defined(RTCONFIG_RALINK_MT7620)
	mt7620_reg_read(reg, &value);
#elif defined(RTCONFIG_RALINK_MT7621)
	mt7621_reg_read(reg, &value);
#endif

	value &= (~REG_PCR_EG_TAG_MASK);
	value |= 0xff0003;
	value |= ((unsigned int)eg_tag << REG_PCR_EG_TAG_OFFT);
#if defined(RTCONFIG_RALINK_MT7620)
	mt7620_reg_write(reg, value);
#elif defined(RTCONFIG_RALINK_MT7621)
	mt7621_reg_write(reg, value);
#endif
	switch_fini();
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
#if defined(RTCONFIG_RALINK_MT7620)
static void config_mt7620_esw_LANWANPartition(int type)
#elif defined(RTCONFIG_RALINK_MT7621)
static void config_mt7621_esw_LANWANPartition(int type)
#endif
{
	char portmap[16];
	unsigned int t;
	int i, v, wans_lan_vid = 3, wanscap_wanlan = get_wans_dualwan() & (WANSCAP_WAN | WANSCAP_LAN);
	int wanscap_lan = get_wans_dualwan() & WANSCAP_LAN;
	int wans_lanport = nvram_get_int("wans_lanport");
	int sw_mode = sw_mode();
	unsigned int m;

	if (sw_mode == SW_MODE_AP || sw_mode == SW_MODE_REPEATER)
		wanscap_lan = 0;

	if (wanscap_lan && (wans_lanport < 0 || wans_lanport > 4)) {
		_dprintf("%s: invalid wans_lanport %d!\n", __func__, wans_lanport);
		wanscap_lan = 0;
		wanscap_wanlan &= ~WANSCAP_LAN;
	}

	if (wanscap_lan && !(get_wans_dualwan() & WANSCAP_WAN))
		wans_lan_vid = 2;

	build_wan_lan_mask(type);
	_dprintf("%s: LAN/WAN/WANS_LAN portmask %08x/%08x/%08x\n", __func__, lan_mask, wan_mask, wans_lan_mask);

	//LAN/WAN ports as security mode
	for (i = 0; i < 6; i++)
		set_switch_vlan_egresstag(i, 0);

	//LAN/WAN ports as transparent port
	for (i = 0; i < 6; i++)
	{
		set_switch_vlan_port_attribute(i, 3); //transparent port
		set_acceptable_frame_type(i, 2); //admit untagged frames
	}

	//set CPU/P7 port as user port
	set_switch_vlan_port_attribute(CPU_PORT, 0); //user port
	set_switch_vlan_port_attribute(P7_PORT, 0); //user port
	set_acceptable_frame_type(CPU_PORT, 0); //admit all frames
	set_acceptable_frame_type(P7_PORT, 0); //admit all frames
	set_switch_vlan_egresstag(CPU_PORT, 2); //Egress VLAN Tag Attribution=tagged
	set_switch_vlan_egresstag(P7_PORT, 2); //Egress VLAN Tag Attribution=tagged

	if (switch_init() < 0)
		return;
	//set PVID
	//for MT7621, port 5 is the part of wan-ports. we should set its PVID. 
	for (i = 0, m = 1; i < (NR_WANLAN_PORT+1); ++i, m <<= 1) {
		if (lan_mask & m)
			v = 1;	//LAN
		else if (wanscap_lan && (wans_lan_mask & m))
			v = wans_lan_vid;	//WANSLAN
		else
			v = 2;	//WAN
		_dprintf("%s: P%d PVID %d\n", __func__, i, v);
#if defined(RTCONFIG_RALINK_MT7620)		
		mt7620_reg_write((REG_ESW_PORT_PPBV1_P0 + 0x100*i), 0x10000 | v);
#elif defined(RTCONFIG_RALINK_MT7621)		
		mt7621_reg_read((REG_ESW_PORT_PPBV1_P0 + 0x100*i), &t);
		t  &= 0xfffff000;
		mt7621_reg_write((REG_ESW_PORT_PPBV1_P0 + 0x100*i), t | v);
#endif		
	}
#if defined(RTCONFIG_RALINK_MT7620)
	mt7620_reg_write((REG_ESW_PORT_PPBV1_P0 + 0x100*P5_PORT), 0x10001);
#elif defined(RTCONFIG_RALINK_MT7621)
	//ignore P5 setting
	//mt7621_reg_write((REG_ESW_PORT_PPBV1_P0 + 0x100*P5_PORT), 0x10001);
#endif	
	//VLAN member port: WAN, LAN, WANS_LAN
	//LAN: P7, P6, lan_mask
	__create_port_map(0xC0 | lan_mask, portmap);
#if defined(RTCONFIG_RALINK_MT7620)
	mt7620_vlan_set(0, 1, portmap, 0);
#elif defined(RTCONFIG_RALINK_MT7621)
	mt7621_vlan_set(0, 1, portmap, 0);
#endif	
	if (sw_mode == SW_MODE_ROUTER
#if defined(RTCONFIG_AMAS) || defined(RTCONFIG_EASYMESH)
		|| (sw_mode == SW_MODE_AP && nvram_match("re_mode", "1"))
#endif
	) {
		switch (wanscap_wanlan) {
		case WANSCAP_WAN | WANSCAP_LAN:
#if defined(RTCONFIG_RALINK_MT7620)
			//WAN: P7, P6, wan_mask
			__create_port_map(0xC0 | wan_mask, portmap);
			mt7620_vlan_set(1, 2, portmap, 0);
#elif defined(RTCONFIG_RALINK_MT7621)
			__create_port_map(wan_mask, portmap);
			mt7621_vlan_set(1, 2, portmap, 0);
#endif			
			//WANSLAN: P6, wans_lan_mask
			__create_port_map(0x40 | wans_lan_mask, portmap);
#if defined(RTCONFIG_RALINK_MT7620)
			mt7620_vlan_set(2, wans_lan_vid, portmap, 0);
#elif defined(RTCONFIG_RALINK_MT7621)
			mt7621_vlan_set(2, wans_lan_vid, portmap, 0);
#endif			
			break;
		case WANSCAP_LAN:
#if defined(RTCONFIG_RALINK_MT7620)
			//WANSLAN: P7, P6, wans_lan_mask
			__create_port_map(0xC0 | wans_lan_mask, portmap);
			mt7620_vlan_set(1, 2, portmap, 0);
#elif defined(RTCONFIG_RALINK_MT7621)
			//P6
			__create_port_map(0x20 | wans_lan_mask, portmap);
			mt7621_vlan_set(1, 2, portmap, 0);
#endif			
			break;
		case WANSCAP_WAN:
#if defined(RTCONFIG_RALINK_MT7620)
			//WAN: P7, P6, wan_mask
			__create_port_map(0xC0 | wan_mask, portmap);
			mt7620_vlan_set(1, 2, portmap, 0);
#elif defined(RTCONFIG_RALINK_MT7621)
			__create_port_map(wan_mask, portmap);
			mt7621_vlan_set(1, 2, portmap, 0);
#endif			
			break;
		default:
			_dprintf("%s: Unknown WANSCAP %x\n", __func__, wanscap_wanlan);
		}
	}
	switch_fini();
}

#ifdef RTCONFIG_MULTILAN_CFG

void __apg_switch_vlan_set(int vid, unsigned int default_portmask, unsigned int trunk_portmask, unsigned int access_portmask)
{
#define MAX_PORTMAP 	16
	
	int i;
	char portmap[MAX_PORTMAP+1] = "0000000000000000\0", *p = &portmap[0];

	portmap[6] = '1';	// CPU port 6
	for (i=0; i<MAX_PORTMAP; i++, p++) {
		// default mode
		if (default_portmask & (1<<i)) {
			*p = '1';
			// port attribute:user port
			set_switch_vlan_port_attribute(i, 0); 
			// egresstag:tag
			set_switch_vlan_egresstag(i, 2);
			// acceptable_frame:admit all frames
			set_acceptable_frame_type(i, 0);
			// pvid:1 (forward to VLAN1(vid=1)) 
			set_switch_pvid(i, 1);
		}

		// trunk mode
		if (trunk_portmask & (1<<i)) {
			*p = '1';
			// port attribute:user port
			set_switch_vlan_port_attribute(i, 0); 
			// egresstag:tag
			set_switch_vlan_egresstag(i, 2);
			// acceptable_frame:admit only vlan-tagged frames
			set_acceptable_frame_type(i, 1);
			// pvid
			set_switch_pvid(i, vid);
		}

		// access mode 
		if (access_portmask & (1<<i)) {
			*p = '1';
			// port attribute:user port
			set_switch_vlan_port_attribute(i, 0); 
			// egresstag:untag
			set_switch_vlan_egresstag(i, 0);
			// acceptable_frame:admit only untagged or priority-tagged frames
			set_acceptable_frame_type(i, 2);
			// pvid
			set_switch_pvid(i, vid);
		}
	}

	if (switch_init() >= 0) {
#if defined(RTCONFIG_RALINK_MT7620)
		mt7620_vlan_set(0, vid, portmap, 0);
#elif defined(RTCONFIG_RALINK_MT7621)
		mt7621_vlan_set(0, vid, portmap, 0);
#endif
		mt762x_vlan_set(-1, 1, portmap, 0, default_portmask);
		switch_fini();
	}

	return;
}

void __apg_switch_vlan_unset(int vid, unsigned int portmask)
{
	int i;
	for(i=0; i<MAX_PORTMAP; i++)
	{
		if(portmask & (1<<i))
		{
			// transparent port
			set_switch_vlan_port_attribute(i, 0);
			// eg untag
			set_switch_vlan_egresstag(i, 0);
			// acceptable_frame:admit only untagged or priority-tagged frames
			set_acceptable_frame_type(i, 2);
			// pvid 1
			set_switch_pvid(i, 1);
		}
	}

	// detele VLAN
	unset_switch_vlan(vid, portmask);

	return;
}

void __apg_switch_isolation(int enable, unsigned int portmask)
{
	int i, j;	
    unsigned int n = 0;
    unsigned int v = 0;
    unsigned int offset = 0;
    unsigned int value = 0;

	if (!enable)
		return;

	set_switch_isolate(portmask);
	return;	
}

#endif	// RTCONFIG_MULTILAN_CFG

#ifdef RTCONFIG_AMAS_WGN
void __wgn_sysdep_swtich_set(int vid)
{
	char portmap[16];
	build_wan_lan_mask(0);

	for(int i=0; i<7; i++)
	{
		if(lan_mask & (1<<i))
		{
			set_switch_vlan_port_attribute(i, 0);// user port
			set_switch_vlan_egresstag(i, 2);// eg tag
		}
	}

	if (switch_init() < 0)
		return;

	if (sw_mode() == SW_MODE_ROUTER
	|| (sw_mode() == SW_MODE_AP && nvram_match("re_mode", "1")))
	{
		//VLAN member port: LAN
		//LAN: P7, P6, lan_mask
		__create_port_map(0xC0 | lan_mask, portmap);
#if defined(RTCONFIG_RALINK_MT7620)
		mt7620_vlan_set(0, vid, portmap, 0);
#elif defined(RTCONFIG_RALINK_MT7621)
		mt7621_vlan_set(0, vid, portmap, 0);
#endif
		//modify vlan1 for lan port per port egress untag
		__create_port_map(0xC0 | lan_mask, portmap);
		mt762x_vlan_set(-1, 1, portmap, 0, lan_mask);
	}
	switch_fini();
	set_admit_all_frames();
}

void __wgn_sysdep_swtich_unset(int vid)
{
	build_wan_lan_mask(0);

	for(int i=0; i<7; i++)
	{
		if(lan_mask & (1<<i))
		{
			set_switch_vlan_port_attribute(i, 3);// transparent port
			set_switch_vlan_egresstag(i, 0);// eg untag
		}
	}
	if (switch_init() < 0)
		return;
#if defined(RTCONFIG_RALINK_MT7620)
	mt7620_vlan_unset(vid);
#elif defined(RTCONFIG_RALINK_MT7621)
	mt7621_vlan_unset(vid);
#endif
	switch_fini();
	set_admit_untag_frames();
}
#endif

#if defined(RTCONFIG_RALINK_MT7620)
static void get_mt7620_esw_WAN_Speed(unsigned int *speed)
#elif defined(RTCONFIG_RALINK_MT7621)
static void get_mt7621_esw_WAN_Speed(unsigned int *speed)
#endif
{
	int i, v = -1, t;
	unsigned int m;

	if (switch_init() < 0)
		return;

	m = (get_wan_port_mask(0) | get_wan_port_mask(1)) & ((1U << (NR_WANLAN_PORT+dv)) - 1);
	for (i = 0; m && i < (NR_WANLAN_PORT+dv); ++i, m >>= 1) {
		if (!(m & 1))
			continue;
#if defined(RTCONFIG_RALINK_MT7620)
		mt7620_reg_read((REG_ESW_MAC_PMSR_P0 + 4*i), (unsigned int*) &t);
#elif defined(RTCONFIG_RALINK_MT7621)
		mt7621_reg_read((REG_ESW_MAC_PMSR_P0 + 0x100*i), &t);
#endif		
		t = (t >> 2) & 0x3;
		if (t < 3 && t > v)
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
	default:
		_dprintf("%s: invalid speed!\n", __func__);
	}
	switch_fini();
}

#if defined(RTCONFIG_RALINK_MT7620)
static void link_down_up_mt7620_PHY(unsigned int mask, int status, int inverse)
#elif defined(RTCONFIG_RALINK_MT7621)
static void link_down_up_mt7621_PHY(unsigned int mask, int status, int inverse)
#endif   
{
#if defined(RTCONFIG_WLMODULE_MT7615E_AP) || defined(RTCONFIG_WLMODULE_MT7915D_AP)
 	int i;
	unsigned int m;

	if (switch_init() < 0)
		return;
	//for MT7621, port5 is the part of WAN-ports
	for (i = 0, m = mask; m && i < (NR_WANLAN_PORT+1); ++i, m >>= 1) {
		if (!(m & 1))
			continue;
		if (!status)
			mt7621_phy_write(i, 0x0, 0x3900);//power down PHY
		else
			mt7621_phy_write(i, 0x0, 0x3300);//power up PHY
	}

	switch_fini();
#else
	int i;
	char idx[2];
	char value[5] = "3300";	//power up PHY
	unsigned int m;

	if (!status)		//power down PHY
		value[1] = '9';
	//for MT7621, port5 is the part of WAN-ports
	for (i = 0, m = mask; m && i < (NR_WANLAN_PORT+1); ++i, m >>= 1) {
		if (!(m & 1))
			continue;
		snprintf(idx, sizeof(idx), "%d", i);
		eval("mii_mgr", "-s", "-p", idx, "-r", "0", "-v", value);
	}
#endif
}

#if defined(RTCONFIG_RALINK_MT7620)
void set_mt7620_esw_broadcast_rate(int bsr)
#elif defined(RTCONFIG_RALINK_MT7621)
void set_mt7621_esw_broadcast_rate(int bsr)
#endif   
{
#if 0
	int i;
	unsigned int value;
#endif

	if ((bsr < 0) || (bsr > 255))
		return;

	if (switch_init() < 0)
		return;

	printf("set broadcast strom control rate as: %d\n", bsr);
#if 0
	for (i = 0; i < NR_WANLAN_PORT; i++) {
		mt7620_reg_read((REG_ESW_PORT_BSR_P0 + 0x100*i), &value);
		value |= 0x1 << 31; //STRM_MODE=Rate-based
		value |= (bsr << 16) | (bsr << 8) | bsr;
		mt7620_reg_write((REG_ESW_PORT_BSR_P0 + 0x100*i), value);
	}
#endif
	switch_fini();
}
#if defined(RTCONFIG_RALINK_MT7620)
void reset_mt7620_esw(void)
#elif defined(RTCONFIG_RALINK_MT7621)
void reset_mt7621_esw(void)
#endif   
{
	unsigned int value;

	if (switch_init() < 0)
		return;

	//Reset ARL engine (clear vlan table)
#if defined(RTCONFIG_RALINK_MT7620)	
	mt7620_reg_read(0xc, &value);
#elif defined(RTCONFIG_RALINK_MT7621)	
	mt7621_reg_read(0xc, &value);
#endif	
	value &= 0xfffffffe;
#if defined(RTCONFIG_RALINK_MT7620)
	mt7620_reg_write(0xc, value);
#elif defined(RTCONFIG_RALINK_MT7621)
	mt7621_reg_write(0xc, value);
#endif	
	usleep(3000);
#if defined(RTCONFIG_RALINK_MT7620)
	mt7620_reg_read(0xc, &value);
#elif defined(RTCONFIG_RALINK_MT7621)
	mt7621_reg_read(0xc, &value);
#endif	
	value |= 0x1;
#if defined(RTCONFIG_RALINK_MT7620)
	mt7620_reg_write(0xc, value);
#elif defined(RTCONFIG_RALINK_MT7621)
	mt7621_reg_write(0xc, value);
#endif	

	//set to default value
	{
		int i;
		for(i = 0; i < 8; i++)
		{
#if defined(RTCONFIG_RALINK_MT7620)
			value = ((i<<1)+1) | (((i<<1)+2) << 12);
			mt7620_reg_write(REG_ESW_VLAN_ID_BASE + 4*i, value);
#elif defined(RTCONFIG_RALINK_MT7621)
			if(i<=1)
			{  
			    mt7621_vtim[i][0]=2*i;
			    mt7621_vtim[i][1]=2*i+1;
			}
			else
			{   
			   mt7621_vtim[i][0]=2*(i+1);
			   mt7621_vtim[i][1]=2*(i+1)+1;
			}   
#endif			
		}
		for(i = 0; i < 16; i++)
		{
			value = (0x80000000 | (2<<12)) + i;	//set VLAN to invalid
			write_VTCR(value);
		}
	}

	switch_fini();
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

//convert port mapping from  RT-N56U   to   RT-N14U / RT-AC52U / RT-AC51U (MT7620) /RT-N54U /RT-AC54U /RT-AC1200HP /RT-N56UB1 /RT-N56UB2
static int convert_port_bitmask(int orig)
{
	int i, mask, result;
	result = 0;
	//for MT7621, port 5 is the part of wan-ports
	for(i = 0; i < (NR_WANLAN_PORT+1); i++) {
		mask = (1 << i);
		if (orig & mask)
			result |= (1 << switch_port_mapping[i]);
	}
	return result;
}


/**
 * @stb_bitmask:	bitmask of STB port(s)
 * 			e.g. bit0 = P0, bit1 = P1, etc.
 */
static void initialize_Vlan(int stb_bitmask)
{
	char portmap[16];
	int wans_lan_vid = 3, wanscap_wanlan = get_wans_dualwan() & (WANSCAP_WAN | WANSCAP_LAN);
	int wanscap_lan = get_wans_dualwan() & WANSCAP_LAN;
	int wans_lanport = nvram_get_int("wans_lanport");
	int sw_mode = sw_mode();

	if (switch_init() < 0)
		return;

	if (sw_mode == SW_MODE_AP || sw_mode == SW_MODE_REPEATER)
		wanscap_lan = 0;

	if (wanscap_lan && (wans_lanport < 0 || wans_lanport > 4)) {
		_dprintf("%s: invalid wans_lanport %d!\n", __func__, wans_lanport);
		wanscap_lan = 0;
		wanscap_wanlan &= ~WANSCAP_LAN;
	}

	if (wanscap_lan && (!(get_wans_dualwan() & WANSCAP_WAN)) && !(!nvram_match("switch_wantag", "none") && !nvram_match("switch_wantag", "")))
		wans_lan_vid = 2;

	build_wan_lan_mask(0);
	stb_bitmask = convert_port_bitmask(stb_bitmask);
	lan_mask &= ~stb_bitmask;
	wan_mask |= stb_bitmask;
	_dprintf("%s: LAN/WAN/WANS_LAN portmask %08x/%08x/%08x\n", __func__, lan_mask, wan_mask, wans_lan_mask);

	//VLAN member port: LAN, WANS_LAN
#if defined(RTCONFIG_RALINK_MT7620)
	//LAN: P7, P6, P5, lan_mask
	__create_port_map(0xE0 | lan_mask, portmap);
	mt7620_vlan_set(0, 1, portmap, 1);
#elif defined(RTCONFIG_RALINK_MT7621)
	__create_port_map(0xC0 | lan_mask, portmap);
	mt7621_vlan_set(0, 1, portmap, 1);
#endif	
	if (wanscap_lan) {
		switch (wanscap_wanlan) {
		case WANSCAP_WAN | WANSCAP_LAN:
			//WANSLAN: P6, wans_lan_mask
			__create_port_map(0x40 | wans_lan_mask, portmap);
#if defined(RTCONFIG_RALINK_MT7620)			
			mt7620_vlan_set(3, wans_lan_vid, portmap, wans_lan_vid);
#elif defined(RTCONFIG_RALINK_MT7621)			
			mt7621_vlan_set(3, wans_lan_vid, portmap, wans_lan_vid);
#endif			
			break;
		case WANSCAP_LAN:
			if (!nvram_match("switch_wantag", "none") && !nvram_match("switch_wantag", "")) {
				//WANSLAN: P6, wans_lan_mask
				__create_port_map(0x40 | wans_lan_mask, portmap);
			} else {
				//WANSLAN: P7, P6, wans_lan_mask
				__create_port_map(0xC0 | wans_lan_mask, portmap);
			}
#if defined(RTCONFIG_RALINK_MT7620)
			mt7620_vlan_set(3, wans_lan_vid, portmap, wans_lan_vid);
#elif defined(RTCONFIG_RALINK_MT7621)
			mt7621_vlan_set(3, wans_lan_vid, portmap, wans_lan_vid);
#endif			
			break;
		case WANSCAP_WAN:
			/* Do nothing. */
			break;
		default:
			_dprintf("%s: Unknown WANSCAP %x\n", __func__, wanscap_wanlan);
		}
	}

	switch_fini();
}

#if defined(RTCONFIG_WLMODULE_MT7615E_AP)
static void fix_up_hwnat_for_wifi(void)
{
	int i, j, m, r, v, isp_profile_hwnat_not_safe = 0;
	char portmap[10];
	char bss[] = "wl0.1_bss_enabledXXXXXX";
	char mode_x[] = "wl0_mode_xXXXXXX";
	struct wifi_if_vid_s w = {
#if defined(RTCONFIG_WLMODULE_MT7615E_AP)
		.wl_vid = { 21, 43 },		/* DP_RA0  ~ DP_RA3:  21, 22, 23, 24;	DP_RAI0  ~ DP_RAI3:  43, 44, 45, 46 */
		.wl_wds_vid = { 37, 59 },	/* DP_WDS0 ~ DP_WDS3: 37, 38, 39, 40;	DP_WDSI0 ~ DP_WDSI3: 59, 60, 61, 62 */
#elif defined(RTN14U) || defined(RTN11P) || defined(RTN300)
		.wl_vid = { 21, -1 },		/* DP_RA0  ~ DP_RA3:  21, 22, 23, 24;	DP_RAI0  ~ DP_RAI3:  43, 44, 45, 46 */
		.wl_wds_vid = { 37, -1 },	/* DP_WDS0 ~ DP_WDS3: N/A;           	DP_WDSI0 ~ DP_WDSI3: N/A */
#else
#error Define VLAN ID of WiFi interface!
#endif
	};

	if (nvram_match("switch_wantag", "none") || nvram_match("switch_wantag", "")) {
		nvram_unset("isp_profile_hwnat_not_safe");
		return;
	}

	if (switch_init() < 0)
		return;

	/* Create VLANs on P6 and P7 for WiFi interfaces to make sure VLAN ID of skbs
	 * that come from WiFi interface and are injected to PPE is not swapped as zero.
	 * This is used to workaround VirIfIdx=0 problem.
	 */
	strlcpy(portmap, "00000011", sizeof(portmap));	/* P6, P7 */
	/* Initial state, 2G/5G interface only. */
	for (i = 0; i <= 1; ++i) {
		if ((v = w.wl_vid[i]) <= 0)
			continue;
#if defined(RTCONFIG_RALINK_MT7620)
		if ((r = mt7620_vlan_set(-1, v, portmap, v)) != 0)
#elif defined(RTCONFIG_RALINK_MT7621)
		if ((r = mt7621_vlan_set(-1, v, portmap, v)) != 0)
#endif		   
			isp_profile_hwnat_not_safe = 1;
		for (j = 1; j <= 3; ++j)
#if defined(RTCONFIG_RALINK_MT7620)		   
			mt7620_vlan_unset(v + j);
#elif defined(RTCONFIG_RALINK_MT7621)		   
			mt7621_vlan_unset(v + j);
#endif
		if ((v = w.wl_wds_vid[i]) <= 0)
			continue;
		for (j = 0; j <= 3; ++j, ++v)
#if defined(RTCONFIG_RALINK_MT7620)
			mt7620_vlan_unset(v);
#elif defined(RTCONFIG_RALINK_MT7621)
			mt7621_vlan_unset(v);
#endif			
	}

	/* 2G/5G guest network i/f */
	for (i = 0; !isp_profile_hwnat_not_safe && i <= 1; ++i) {
		if ((v = w.wl_vid[i]) <= 0)
			continue;

		snprintf(mode_x, sizeof(mode_x), "wl%d_mode_x", i);
		m = nvram_get_int(mode_x);	/* 1: WDS only */
		for (j = 1; !isp_profile_hwnat_not_safe && j <= 3; ++j) {
			snprintf(bss, sizeof(bss), "wl%d.%d_bss_enabled", i, j);
			if (m == 1 || !nvram_get_int(bss))
				continue;

			v++;
#if defined(RTCONFIG_RALINK_MT7620)
			if ((r = mt7620_vlan_set(-1, v, portmap, v)) != 0)
#elif defined(RTCONFIG_RALINK_MT7621)
			if ((r = mt7621_vlan_set(-1, v, portmap, v)) != 0)
#endif			   
				isp_profile_hwnat_not_safe = 1;
		}
	}

	/* 2G/5G WDS i/f */
	for (i = 0; !isp_profile_hwnat_not_safe && i <= 1; ++i) {
		if ((v = w.wl_wds_vid[i]) <= 0)
			continue;

		snprintf(mode_x, sizeof(mode_x), "wl%d_mode_x", i);
		m = nvram_get_int(mode_x);
		if (m != 1 && m != 2)
			continue;
		for (j = 0; !isp_profile_hwnat_not_safe && j <= 3; ++j, ++v) {
#if defined(RTCONFIG_RALINK_MT7620)
			if ((r = mt7620_vlan_set(-1, v, portmap, v)) != 0)
#elif defined(RTCONFIG_RALINK_MT7621)
			if ((r = mt7621_vlan_set(-1, v, portmap, v)) != 0)
#endif			   
				isp_profile_hwnat_not_safe = 1;
		}
	}
	nvram_set_int("isp_profile_hwnat_not_safe", isp_profile_hwnat_not_safe);

	switch_fini();
}
#else
static inline void fix_up_hwnat_for_wifi(void) { }
#endif

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
	unsigned int value;
	char portmap[16];
	int vid = nvram_get_int("vlan_vid") & 0xFFF;
	int prio = nvram_get_int("vlan_prio") & 0x7;
	int mbr = bitmask & 0xffff;
	int untag = (bitmask >> 16) & 0xffff;
	int i, mask, untagmap;

	if (switch_init() < 0)
		return;

	//set PVID & VLAN member port
	value = (0x1 << 16) | (prio << 13) | vid;

	strlcpy(portmap, "00000000", sizeof(portmap)); // init
	untagmap = 0;
	//convert port mapping
	//for MT7621, port 5 is the part of wan-ports
	for(i = 0; i < (NR_WANLAN_PORT+1); i++) {
		mask = (1 << i);
		if (mbr & mask)
			portmap[ switch_port_mapping[i] ]='1';
		if (untag & mask)
			untagmap |= 1 << switch_port_mapping[i];
	}

	for(i = 0; i < 4; i++) //LAN port only
	{
		mask = (1 << i);
		if (mbr & mask) {
			if ((untag & mask) == 0) {	//need tag
#if defined(RTCONFIG_RALINK_MT7620)
				mt7620_reg_write((REG_ESW_PORT_PCR_P0 + 0x100 * switch_port_mapping[i]), 0x20ff0003); //Egress VLAN Tag Attribution=tagged
				mt7620_reg_write((REG_ESW_PORT_PVC_P0 + 0x100 * switch_port_mapping[i]), 0x81000000); //user port, admit all frames
#elif defined(RTCONFIG_RALINK_MT7621)
				mt7621_reg_write((REG_ESW_PORT_PCR_P0 + 0x100 * switch_port_mapping[i]), 0x20ff0003); //Egress VLAN Tag Attribution=tagged
				mt7621_reg_write((REG_ESW_PORT_PVC_P0 + 0x100 * switch_port_mapping[i]), 0x81000000); //user port, admit all frames
#endif
			}
			else {
#if defined(RTCONFIG_RALINK_MT7620)
				mt7620_reg_write((REG_ESW_PORT_PPBV1_P0 + 0x100 * switch_port_mapping[i]), value);
#elif defined(RTCONFIG_RALINK_MT7621)
				mt7621_reg_write((REG_ESW_PORT_PPBV1_P0 + 0x100 * switch_port_mapping[i]), value);
#endif
			}
		}
	}

	if (mbr & 0x200) {	//Internet && WAN port
#if defined(RTCONFIG_RALINK_MT7620)
		portmap[CPU_PORT]='1';
		portmap[P7_PORT]='1';
		mt7620_reg_write((REG_ESW_PORT_PCR_P0 + 0x100*WAN_PORT), 0x20ff0003); //Egress VLAN Tag Attribution=tagged
		mt7620_reg_write((REG_ESW_PORT_PCR_P0 + 0x100*CPU_PORT), 0x20ff0003); //port6(CPU), Egress VLAN Tag Attribution=tagged
		mt7620_reg_write((REG_ESW_PORT_PVC_P0 + 0x100*WAN_PORT), 0x81000000); //user port, admit all frames
		mt762x_vlan_set(-1, vid, portmap, vid, untagmap ? : -1);
#elif defined(RTCONFIG_RALINK_MT7621)
		portmap[P5_PORT]='1';
		mt7621_reg_write((REG_ESW_PORT_PCR_P0 + 0x100*WAN_PORT), 0x20ff0003); //Egress VLAN Tag Attribution=tagged
		mt7621_reg_write((REG_ESW_PORT_PCR_P0 + 0x100*P5_PORT), 0x20ff0003);  //port5(GE2), Egress VLAN Tag Attribution=tagged
		mt7621_reg_write((REG_ESW_PORT_PCR_P0 + 0x100*CPU_PORT), 0x20ff0003); //port6(GE1), Egress VLAN Tag Attribution=tagged
		mt7621_reg_write((REG_ESW_PORT_PVC_P0 + 0x100*WAN_PORT), 0x81000000); //user port, admit all frames
		mt7621_reg_write((REG_ESW_PORT_PVC_P0 + 0x100*P5_PORT), 0x81000000);  //user port, admit all frames
		if (untagmap & (1 << WAN_PORT))
			untagmap |= (1 << P5_PORT);
		mt762x_vlan_set(-1, vid, portmap, vid, untagmap ? : -1);
#endif
	}
	else {	//IPTV, VoIP port
#if defined(RTCONFIG_RALINK_MT7620)
		mt7620_vlan_set(-1, vid, portmap, vid);
#elif defined(RTCONFIG_RALINK_MT7621)
		mt7621_vlan_set(-1, vid, portmap, vid);
#endif		
		fix_up_hwnat_for_wifi();
	}

	switch_fini();
}

static void is_singtel_mio(int is)
{
	int i;
	unsigned int value;

	if (!is)
		return;

	if (switch_init() < 0)
		return;
 //for MT7621, port 5 is the part of wan-ports
	for (i = 0; i < (NR_WANLAN_PORT+1); i++) { //WAN/LAN, admit all frames
#if defined(RTCONFIG_RALINK_MT7620)	   
		mt7620_reg_read((REG_ESW_PORT_PVC_P0 + 0x100*i), &value);
		value &= 0xfffffffc;
		mt7620_reg_write((REG_ESW_PORT_PVC_P0 + 0x100*i), value);
#elif defined(RTCONFIG_RALINK_MT7621)	   
		mt7621_reg_read((REG_ESW_PORT_PVC_P0 + 0x100*i), &value);
		value &= 0xfffffffc;
		mt7621_reg_write((REG_ESW_PORT_PVC_P0 + 0x100*i), value);
#endif		
	}

	switch_fini();
}
#if defined(RTCONFIG_RALINK_MT7620)
int mt7620_ioctl(int val, int val2)
#elif defined(RTCONFIG_RALINK_MT7621)
int mt7621_ioctl(int val, int val2)
#endif   
{
//	int value = 0;
	unsigned int value2 = 0;
	int i, max_wan_unit = 0;

#if defined(RTCONFIG_DUALWAN)
	max_wan_unit = 1;
#endif
	printf("rtkswitch!!!=%d\n",val);
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
#if defined(RTCONFIG_RALINK_MT7620)
		config_mt7620_esw_LANWANPartition(val2);
#elif defined(RTCONFIG_RALINK_MT7621)
		config_mt7621_esw_LANWANPartition(val2);
#endif		
		break;
	case 13:
#if defined(RTCONFIG_RALINK_MT7620)
		get_mt7620_esw_WAN_Speed(&value2);
#elif defined(RTCONFIG_RALINK_MT7621)
		get_mt7621_esw_WAN_Speed(&value2);
#endif		
		printf("WAN speed : %u Mbps\n", value2);
		break;
	case 14: // Link up LAN ports
#if defined(RTCONFIG_RALINK_MT7620)
		link_down_up_mt7620_PHY(get_lan_port_mask(), 1, 0);
#elif defined(RTCONFIG_RALINK_MT7621)
		link_down_up_mt7621_PHY(get_lan_port_mask(), 1, 0);
#endif		
		break;
	case 15: // Link down LAN ports
#if defined(RTCONFIG_RALINK_MT7620)
		link_down_up_mt7620_PHY(get_lan_port_mask(), 0, 0);
#elif defined(RTCONFIG_RALINK_MT7621)
		link_down_up_mt7621_PHY(get_lan_port_mask(), 0, 0);
#endif		
		break;
	case 16: // Link up ALL ports
#if defined(RTCONFIG_RALINK_MT7620)
		link_down_up_mt7620_PHY(0x1F, 1, 0);
#elif defined(RTCONFIG_RALINK_MT7621)
		link_down_up_mt7621_PHY(0x3F, 1, 0);
#endif		
		break;
	case 17: // Link down ALL ports
#if defined(RTCONFIG_RALINK_MT7620)		
		link_down_up_mt7620_PHY(0x1F, 0, 0);
#elif defined(RTCONFIG_RALINK_MT7621)		
		link_down_up_mt7621_PHY(0x3F, 0, 0);
#endif		
		break;
	case 21:
		break;
	case 22:
		break;
	case 23:
		break;
	case 24:
		break;
	case 25:
#if defined(RTCONFIG_RALINK_MT7620)
		set_mt7620_esw_broadcast_rate(val2);
#elif defined(RTCONFIG_RALINK_MT7621)
		set_mt7621_esw_broadcast_rate(val2);
#endif		
		break;
	case 27:
#if defined(RTCONFIG_RALINK_MT7620)
		reset_mt7620_esw();
#elif defined(RTCONFIG_RALINK_MT7621)
		reset_mt7621_esw();
#endif		
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
	case 40:
		is_singtel_mio(val2);
		break;
	case 50:
		fix_up_hwnat_for_wifi();
		break;
	case 114: // link up WAN ports
		for (i = WAN_UNIT_FIRST; i <= max_wan_unit; ++i)
#if defined(RTCONFIG_RALINK_MT7620)
			link_down_up_mt7620_PHY(get_wan_port_mask(i), 1, 1);
#elif defined(RTCONFIG_RALINK_MT7621)
			link_down_up_mt7621_PHY(get_wan_port_mask(i), 1, 1);
#endif			
		break;
	case 115: // link down WAN ports
		for (i = WAN_UNIT_FIRST; i <= max_wan_unit; ++i)
#if defined(RTCONFIG_RALINK_MT7620)
			link_down_up_mt7620_PHY(get_wan_port_mask(i), 0, 1);
#elif defined(RTCONFIG_RALINK_MT7621)
			link_down_up_mt7621_PHY(get_wan_port_mask(i), 0, 1);
#endif			
		break;
	case 200:	/* set LAN port number that is used as WAN port */
		/* Nothing to do, nvram_get_int("wans_lanport ") is enough. */
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
#if defined(RTCONFIG_RALINK_MT7620)
	return mt7620_ioctl(val, val2);
#elif defined(RTCONFIG_RALINK_MT7621)
	return mt7621_ioctl(val, val2);
#endif	
}

int
ralink_set_gpio_mode(unsigned int group)
{
	int fd;
 	unsigned long value;
	unsigned int req;
	
	fd = open(GPIO_DEV, O_RDONLY);
	if (fd < 0) {
		perror(GPIO_DEV);
		return -1;
	}
	req = RALINK_SET_GPIO_MODE;
	//_dprintf("set gpio group =%d  as gpio mode\n",group);
	req= req | (group<<24); 
	value=0;
	if (ioctl(fd, req, &value) < 0) {
		perror("ioctl");
		close(fd);
		return -1;
	}
	close(fd);
	return 0;
	
}

/*
typedef struct {
	unsigned int idx;
	unsigned int value;
} asus_gpio_info;
*/

#if defined(RTCONFIG_SYSCLASSGPIO)
#if defined(RTCONFIG_WLMODULE_MT7915D_AP)
#define RALINK_MT7621_GPIO_PIN_BASE 	0
#define RALINK_MT7621_GPIO_NUM_BASE 	0
#else
#define RALINK_MT7621_GPIO_PIN_BASE 	0
#define RALINK_MT7621_GPIO_NUM_BASE 	0
#endif
#define GPIO_BASE RALINK_MT7621_GPIO_PIN_BASE
#define GPIOLIB_DIR	"/sys/class/gpio"
#endif

#ifndef PATH_MAX
#define PATH_MAX 1024
#endif

#if defined(RTCONFIG_SYSCLASSGPIO)
/* Export specified GPIO
 * @return:
 * 	0:	success
 *  otherwise:	fail
 */
static int __export_gpio(uint32_t gpio)
{
	char gpio_path[PATH_MAX], export_path[PATH_MAX], gpio_str[] = "999XXX";

	if (!d_exists(GPIOLIB_DIR)) {
		_dprintf("%s does not exist!\n", __func__);
		return -1;
	}
	snprintf(gpio_path, sizeof(gpio_path),"%s/gpio%d", GPIOLIB_DIR, gpio);
	if (d_exists(gpio_path))
		return 0;

	snprintf(export_path, sizeof(export_path), "%s/export", GPIOLIB_DIR);
	snprintf(gpio_str, sizeof(gpio_str), "%d", gpio);
	f_write_string(export_path, gpio_str, 0, 0);

	return 0;
}
#endif

int
ralink_gpio_write_bit(int idx, int value)
{
#if defined(RTCONFIG_SYSCLASSGPIO)
	char path[PATH_MAX], val_str[10];

	idx += GPIO_BASE;
	snprintf(val_str, sizeof(val_str), "%d", !!value);
	snprintf(path, sizeof(path), "%s/gpio%d/value", GPIOLIB_DIR, idx);
	f_write_string(path, val_str, 0, 0);
	return -1;
#else
	int fd;
       unsigned int req;

	fd = open(GPIO_DEV, O_RDONLY);
	if (fd < 0) {
		perror(GPIO_DEV);
		return -1;
	}
#if defined(RTCONFIG_RALINK_MT7620)	
	if (idx<=23)
	{
		if(value)
		   req=RALINK_GPIO2300_SET;
		else
		   req=RALINK_GPIO2300_CLEAR;
	
	}   
	else if (idx>23 && idx<=39)
	{	
  	  	idx -=24;
		if(value)
		   req=RALINK_GPIO3924_SET;
		else
		   req=RALINK_GPIO3924_CLEAR;
	}	
	else if (idx>39 && idx <=71)
	{	
  	 	idx -=40;
		if(value)
		   req=RALINK_GPIO7140_SET;
		else
		   req=RALINK_GPIO7140_CLEAR;
	}	
	else if (idx==72) 
	{              
#if defined(RTN14U) || defined(RTAC52U) || defined(RTAC51U) || defined(RTN11P) || defined(RTN300) || defined(RTN54U) || defined(RTAC1200HP) || defined(RTAC54U)	//wlan led
		req=RALINK_ATE_GPIO72;
		idx=value;
#else
#error invalid product!!
#endif		
	}   
#elif defined(RTCONFIG_RALINK_MT7621)
	if (idx<=31)
	{
		if(value)
		   req=RALINK_GPIO3100_SET;
		else
		   req=RALINK_GPIO3100_CLEAR;
	
	}   
	else if (idx>31 && idx<=63)
	{	
  	  	idx -=32;
		if(value)
		   req=RALINK_GPIO6332_SET;
		else
		   req=RALINK_GPIO6332_CLEAR;
	}	
	else if (idx>63 && idx <=95)
	{	
  	 	idx -=64;
		if(value)
		   req=RALINK_GPIO9564_SET;
		else
		   req=RALINK_GPIO9564_CLEAR;
	}	
#endif
	else
		return -1;

	if (ioctl(fd, req, (1<<idx)) < 0) {
		perror("ioctl");
		close(fd);
		return -1;
	}
	close(fd);
	return 0;
#endif
}

int
ralink_gpio_read_bit(int idx)
{
#if defined(RTCONFIG_SYSCLASSGPIO)
	char path[PATH_MAX], value[10];

	idx += GPIO_BASE;
	snprintf(path, sizeof(path), "%s/gpio%d/value", GPIOLIB_DIR, idx);
	f_read_string(path, value, sizeof(value));

	return safe_atoi(value);
	return -1;
#else
 	unsigned long value;
	int fd;
	unsigned int req;

	value = 0;
	fd = open(GPIO_DEV, O_RDONLY);
	if (fd < 0) {
		perror(GPIO_DEV);
		return -1;
	}

#if defined(RTCONFIG_RALINK_MT7620)
	if(idx<=23)   
		 req = RALINK_GPIO2300_READ;
	else if (idx>23 && idx<=39)
	{
		idx-=24;   
	   	 req= RALINK_GPIO3924_READ;
	}	 
	else if (idx>39 && idx <=71)
	{	 
	   	idx-=40;
   	 	req= RALINK_GPIO7140_READ;
	}	
#elif defined(RTCONFIG_RALINK_MT7621)	
	if(idx<=31)   
		 req = RALINK_GPIO3100_READ;
	else if (idx>31 && idx<=63)
	{
		idx-=32;   
	   	 req = RALINK_GPIO6332_READ;
	}	 
	else if (idx>63 && idx <=95)
	{	 
	   	idx-=64;
   	 	req = RALINK_GPIO9564_READ;
	}	

#endif	
	else
		return -1;

	if (ioctl(fd, req, &value) < 0) {
		perror("ioctl");
		close(fd);
		return -1;
	}
	close(fd);

	value=(value>>idx)&1;

  	return value;
#endif
}

int
ralink_gpio_set_gpiomode(unsigned int idx, int isgpio)
{
	int fd;
	unsigned long arg;


	fd = open(GPIO_DEV, O_RDONLY);
	if (fd < 0) {
		perror(GPIO_DEV);
		return -1;
	}

	arg = ((isgpio & 0xffff) << 16) | (idx & 0xffff);

	if (ioctl(fd, RALINK_GPIO_SET_MODE, arg) < 0) {
		perror("ioctl(RALINK_GPIO_SET_MODE)");
		close(fd);
		return -1;
	}
	close(fd);
	return 0;
}

int
ralink_gpio_init(unsigned int idx, int dir)
{
#if defined(RTCONFIG_SYSCLASSGPIO)
	char path[PATH_MAX], v[10], *dir_str = "in";

	idx += GPIO_BASE;
	if (dir == GPIO_DIR_OUT) {
		dir_str = "out";		/* output, low voltage */
		*v = '\0';
		snprintf(path, sizeof(path), "%s/gpio%d/value", GPIOLIB_DIR, idx);
		if (f_read_string(path, v, sizeof(v)) > 0 && safe_atoi(v) == 1)
			dir_str = "high";	/* output, high voltage */
	}

	__export_gpio(idx);
	snprintf(path, sizeof(path), "%s/gpio%d/direction", GPIOLIB_DIR, idx);
	f_write_string(path, dir_str, 0, 0);

	return 0;
#else
	int fd, req;
	unsigned long arg;
	
#if defined(RTN14U) || defined(RTAC52U) || defined(RTAC51U) || defined(RTN11P) || defined(RTN300) || defined(RTN54U) || defined(RTAC1200HP) || defined(RTAC54U)
	if(idx==72) //discard gpio72
		return 0;
#endif

	ralink_gpio_set_gpiomode(idx, 1);

	fd = open(GPIO_DEV, O_RDONLY);
	if (fd < 0) {
		perror(GPIO_DEV);
		return -1;
	}
	if (dir == gpio_in) {
#if defined(RTCONFIG_RALINK_MT7620)	   
		if (idx <= 23)  //gpio 0~23
		{   
			req = RALINK_GPIO2300_SET_DIR_IN;
			arg=1<<idx;
		}	
		else if ((idx > 23) && (idx <= 39))  //gpio 24~39
		{	
	   	 	  req = RALINK_GPIO3924_SET_DIR_IN;
			  arg=1<<(idx-24);
		}	  
		else if ((idx > 39) && (idx <= 71))  //gpio 40~71
		{	
	   		  req = RALINK_GPIO7140_SET_DIR_IN;
			  arg=1<<(idx-40);
		}	  
#elif defined(RTCONFIG_RALINK_MT7621)		
		if (idx <= 31)  //gpio 0~31
		{   
			req = RALINK_GPIO3100_SET_DIR_IN;
			arg=1<<idx;
		}	
		else if ((idx > 31) && (idx <= 63))  //gpio 31~63
		{	
	   	 	  req = RALINK_GPIO6332_SET_DIR_IN;
			  arg=1<<(idx-32);
		}	  
		else if ((idx > 63) && (idx <= 95))  //gpio 64~95
		{	
	   		  req = RALINK_GPIO9564_SET_DIR_IN;
			  arg=1<<(idx-64);
		}	  
#endif		
		else
			return -1;
	}
	else {
#if defined(RTCONFIG_RALINK_MT7620)
		if (idx <= 23)  //gpio 0~23
		{   
			req = RALINK_GPIO2300_SET_DIR_OUT;
			arg=1<<idx;
		}	
		else if ((idx > 23) && (idx <= 39))  //gpio 24~39
		{	
	   	 	 req = RALINK_GPIO3924_SET_DIR_OUT;
			 arg=1<<(idx-24);
		}	 
		else if ((idx > 39) && (idx <= 71))  //gpio 40~71
		{	
	   	 	  req = RALINK_GPIO7140_SET_DIR_OUT;
			  arg=1<<(idx-40);
		}
#elif defined(RTCONFIG_RALINK_MT7621)
		if (idx <= 31)  //gpio 0~31
		{   
			req = RALINK_GPIO3100_SET_DIR_OUT;
			arg=1<<idx;
		}	
		else if ((idx > 31) && (idx <= 63))  //gpio 32~63
		{	
	   	 	 req = RALINK_GPIO6332_SET_DIR_OUT;
			 arg=1<<(idx-32);
		}	 
		else if ((idx > 63) && (idx <= 95))  //gpio 64~95
		{	
	   	 	  req = RALINK_GPIO9564_SET_DIR_OUT;
			  arg=1<<(idx-64);
		}
#endif
		else
			return -1;
	}

	if (ioctl(fd, req, arg) < 0) {
		perror("ioctl");
		close(fd);
		return -1;
	}
	close(fd);
	return 0;
#endif
}

int ralink_gpio_init2(unsigned int idx, int dir)
{
#if defined(RTCONFIG_SYSCLASSGPIO)
	char path[PATH_MAX], *dir_str = "in";

	idx += GPIO_BASE;
	if (dir == GPIO_DIR_OUT) {
		dir_str = "high";	/* output, high voltage */
	}

	__export_gpio(idx);
	snprintf(path, sizeof(path), "%s/gpio%d/direction", GPIOLIB_DIR, idx);
	f_write_string(path, dir_str, 0, 0);

	return 0;
#endif
}

// a wrapper to broadcom world
// bit 1: LED_POWER
// bit 6: BTN_RESET
// bit 8: BTN_WPS

uint32_t ralink_gpio_read(void)
{
	uint32_t r=0;
         if(ralink_gpio_read_bit(RA_BTN_RESET)) r|=(1<<6);
	 if(ralink_gpio_read_bit(RA_BTN_WPS)) r|=(1<<8);

	return r;
}

int ralink_gpio_write(uint32_t bit, int en)
{
       if(bit&(1<<1))
       {
            if(!en) ralink_gpio_write_bit(RA_LED_POWER, RA_LED_OFF);
            else ralink_gpio_write_bit(RA_LED_POWER, RA_LED_ON);
       }
	return 0;
}

unsigned int
rtkswitch_wanPort_phyStatus(int wan_unit)
{
	unsigned int status = 0;
#if defined(RTCONFIG_RALINK_MT7620)
	get_mt7620_esw_phy_linkStatus(get_wan_port_mask(wan_unit), &status);
#elif defined(RTCONFIG_RALINK_MT7621)
	get_mt7621_esw_phy_linkStatus(get_wan_port_mask(wan_unit), &status);
#endif	
	return status;
}

unsigned int
rtkswitch_lanPorts_phyStatus(void)
{
	unsigned int status = 0;
#if defined(RTCONFIG_RALINK_MT7620)
	get_mt7620_esw_phy_linkStatus(get_lan_port_mask(), &status);
#elif defined(RTCONFIG_RALINK_MT7621)
	get_mt7621_esw_phy_linkStatus(get_lan_port_mask(), &status);
#endif	
	return status;
}

unsigned int
rtkswitch_WanPort_phySpeed(void)
{
	unsigned int speed;
#if defined(RTCONFIG_RALINK_MT7620)
	get_mt7620_esw_WAN_Speed(&speed);
#elif defined(RTCONFIG_RALINK_MT7621)
	get_mt7621_esw_WAN_Speed(&speed);
#endif

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
 *  0:  no-link
 *  1:  link-up
 * @speed:
 *  0,10:       10Mbps      ==> 'M'
 *  1,100:      100Mbps     ==> 'M'
 *  2,1000:     1000Mbps    ==> 'G'
 *  3,10000:    10Gbps      ==> 'T'
 *  4,2500:     2.5Gbps     ==> 'Q'
 *  5,5000:     5Gbps       ==> 'F'
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

#ifdef RTCONFIG_NEW_PHYMAP

void ATE_port_status(int verbose, phy_info_list *list)
{   
    int i, len;
    char buf[64];
	unsigned int value;
    char cap_buf[64] = {0};
    phyState pS;

    phy_port_mapping port_mapping;
    get_phy_port_mapping(&port_mapping);
	if (list)
		list->count = 0;

    if (switch_init() < 0)
        return;

    for (i = 0; i < (NR_WANLAN_PORT+dv); i++) {
        pS.link[i] = 0;
        pS.speed[i] = 0;
#if defined(RTCONFIG_RALINK_MT7620)
        mt7620_reg_read((REG_ESW_MAC_PMSR_P0 + 0x100*i), &value);
#elif defined(RTCONFIG_RALINK_MT7621)
        mt7621_reg_read((REG_ESW_MAC_PMSR_P0 + 0x100*i), &value);
#endif
        pS.link[i] = value & 0x1;
        pS.speed[i] = (value >> 2) & 0x3;
    }

    len = 0; 
    for (i = 0; i < port_mapping.count && i<MAX_PORT; i++) {
        // Only handle WAN/LAN ports
        if (((port_mapping.port[i].cap & PHY_PORT_CAP_WAN) == 0) && ((port_mapping.port[i].cap & PHY_PORT_CAP_LAN) == 0))
            continue;
        if (list) {
            list->phy_info[i].phy_port_id = port_mapping.port[i].phy_port_id;
			if(pS.link[port_mapping.port[i].phy_port_id] == 1){
				list->phy_info[i].link_rate = pS.speed[port_mapping.port[i].phy_port_id] == 2 ? 1000 : 100;
				snprintf(list->phy_info[i].state, sizeof(list->phy_info[i].state), "%s", "up");
			}else{
				list->phy_info[i].link_rate = 0;
				snprintf(list->phy_info[i].state, sizeof(list->phy_info[i].state), "%s", "down");
			}
			snprintf(list->phy_info[i].label_name, sizeof(list->phy_info[i].label_name), "%s",
                port_mapping.port[i].label_name);
            list->phy_info[i].cap = port_mapping.port[i].cap;
            snprintf(list->phy_info[i].cap_name, sizeof(list->phy_info[i].cap_name), "%s",
                get_phy_port_cap_name(port_mapping.port[i].cap, cap_buf, sizeof(cap_buf)));
            if (pS.link[port_mapping.port[i].phy_port_id] == 1 && !list->status_and_speed_only) {
				// TODO not complete
				//get_mt7621_port_mib(port_mapping.port[i].phy_port_id, &list->phy_info[i]);
            }

            list->count++;
        }
        len += sprintf(buf+len, "%s=%C;", port_mapping.port[i].label_name,
            conv_speed(pS.link[port_mapping.port[i].phy_port_id], pS.speed[port_mapping.port[i].phy_port_id]));
    }
	
	switch_fini();

    if (verbose)
        puts(buf);
}

#endif

#if defined(RTCONFIG_RALINK_MT7620)
void ATE_mt7620_esw_port_status(void)
#elif defined(RTCONFIG_RALINK_MT7621)
void ATE_mt7621_esw_port_status(void)
#endif
{
	int i;
	unsigned int value;
	char buf[32];
	phyState pS;

	if (switch_init() < 0)
		return;

	for (i = 0; i < (NR_WANLAN_PORT+dv); i++) {
		pS.link[i] = 0;
		pS.speed[i] = 0;
#if defined(RTCONFIG_RALINK_MT7620)
		mt7620_reg_read((REG_ESW_MAC_PMSR_P0 + 0x100*i), &value);
#elif defined(RTCONFIG_RALINK_MT7621)
		mt7621_reg_read((REG_ESW_MAC_PMSR_P0 + 0x100*i), &value);
#endif		
		pS.link[i] = value & 0x1;
		pS.speed[i] = (value >> 2) & 0x3;
	}

#if defined(PORT_W4321)
	snprintf(buf, sizeof(buf), "W0=%C;L4=%C;L3=%C;L2=%C;L1=%C;",
		(pS.link[ WAN_PORT] == 1) ? (pS.speed[ WAN_PORT] == 2) ? 'G' : 'M': 'X',
		(pS.link[LAN4_PORT] == 1) ? (pS.speed[LAN4_PORT] == 2) ? 'G' : 'M': 'X',
		(pS.link[LAN3_PORT] == 1) ? (pS.speed[LAN3_PORT] == 2) ? 'G' : 'M': 'X',
		(pS.link[LAN2_PORT] == 1) ? (pS.speed[LAN2_PORT] == 2) ? 'G' : 'M': 'X',
		(pS.link[LAN1_PORT] == 1) ? (pS.speed[LAN1_PORT] == 2) ? 'G' : 'M': 'X');
#else
#if defined(RTCONFIG_CONCURRENTREPEATER) && defined(RPAC87)
		snprintf(buf, sizeof(buf), "L1=%C",
		(pS.link[ WAN_PORT] == 1) ? (pS.speed[ WAN_PORT] == 2) ? 'G' : 'M': 'X');
#elif defined(RTAX53U) || defined(RTCONFIG_3LANPORT_DEVICE)
	snprintf(buf, sizeof(buf), "W0=%C;L1=%C;L2=%C;L3=%C;",
		(pS.link[ WAN_PORT] == 1) ? (pS.speed[ WAN_PORT] == 2) ? 'G' : 'M': 'X',
		(pS.link[LAN1_PORT] == 1) ? (pS.speed[LAN1_PORT] == 2) ? 'G' : 'M': 'X',
		(pS.link[LAN2_PORT] == 1) ? (pS.speed[LAN2_PORT] == 2) ? 'G' : 'M': 'X',
		(pS.link[LAN3_PORT] == 1) ? (pS.speed[LAN3_PORT] == 2) ? 'G' : 'M': 'X');

#elif defined(XD4S)
	snprintf(buf, sizeof(buf), "W0=%C;L1=%C;",
		(pS.link[ WAN_PORT] == 1) ? (pS.speed[ WAN_PORT] == 2) ? 'G' : 'M': 'X',
		(pS.link[LAN1_PORT] == 1) ? (pS.speed[LAN1_PORT] == 2) ? 'G' : 'M': 'X');
#else
	snprintf(buf, sizeof(buf), "W0=%C;L1=%C;L2=%C;L3=%C;L4=%C;",
		(pS.link[ WAN_PORT] == 1) ? (pS.speed[ WAN_PORT] == 2) ? 'G' : 'M': 'X',
		(pS.link[LAN1_PORT] == 1) ? (pS.speed[LAN1_PORT] == 2) ? 'G' : 'M': 'X',
		(pS.link[LAN2_PORT] == 1) ? (pS.speed[LAN2_PORT] == 2) ? 'G' : 'M': 'X',
		(pS.link[LAN3_PORT] == 1) ? (pS.speed[LAN3_PORT] == 2) ? 'G' : 'M': 'X',
		(pS.link[LAN4_PORT] == 1) ? (pS.speed[LAN4_PORT] == 2) ? 'G' : 'M': 'X');
#endif
#endif
	puts(buf);

	switch_fini();
}

#if defined(RTCONFIG_SWRT_I2CLED) || defined(CMCCA9)
void swrt_esw_port_status(int port, int *mode, int *speed)
{
	int i;
	unsigned int value;
	phyState pS;

	if (switch_init() < 0)
		return;

	for (i = 0; i < (NR_WANLAN_PORT+dv); i++) {
		pS.link[i] = 0;
		pS.speed[i] = 0;
#if defined(RTCONFIG_RALINK_MT7620)
		mt7620_reg_read((REG_ESW_MAC_PMSR_P0 + 0x100*i), &value);
#elif defined(RTCONFIG_RALINK_MT7621)
		mt7621_reg_read((REG_ESW_MAC_PMSR_P0 + 0x100*i), &value);
#endif		
		pS.link[i] = value & 0x1;
		pS.speed[i] = (value >> 2) & 0x3;
	}
	switch(port){
		case WAN_PORT:
			*mode = (pS.link[WAN_PORT] == 1);
			*speed = (pS.speed[WAN_PORT] == 2);
			break;
		case LAN1_PORT:
			*mode = (pS.link[LAN1_PORT] == 1);
			*speed = (pS.speed[LAN1_PORT] == 2);
			break;
		case LAN2_PORT:
			*mode = (pS.link[LAN2_PORT] == 1);
			*speed = (pS.speed[LAN2_PORT] == 2);
			break;
		case LAN3_PORT:
			*mode = (pS.link[LAN3_PORT] == 1);
			*speed = (pS.speed[LAN3_PORT] == 2);
			break;
		case LAN4_PORT:
			*mode = (pS.link[LAN4_PORT] == 1);
			*speed = (pS.speed[LAN4_PORT] == 2);
			break;
		default:
			*mode = 0;
			*speed = 0;
			break;
	}
	switch_fini();
}
#endif

void restore_esw(void)
{
	int i;

	if (switch_init() < 0)
		return;

	for (i = 0; i < 8; i++) {
#if defined(RTCONFIG_RALINK_MT7620)
		mt7620_reg_write((REG_ESW_PORT_PCR_P0 + 0x100*i), 0xff0000);
		mt7620_reg_write((REG_ESW_PORT_PVC_P0 + 0x100*i), 0x810000c0);
#elif defined(RTCONFIG_RALINK_MT7621)
		mt7621_reg_write((REG_ESW_PORT_PCR_P0 + 0x100*i), 0xff0000);
		mt7621_reg_write((REG_ESW_PORT_PVC_P0 + 0x100*i), 0x810000c0);
#endif		
	}

	//Clear mac table
#if defined(RTCONFIG_RALINK_MT7620)
	mt7620_reg_write(REG_ESW_WT_MAC_ATC, 0x8002);
#elif defined(RTCONFIG_RALINK_MT7621)
	mt7621_reg_write(REG_ESW_WT_MAC_ATC, 0x8002);
#endif
	usleep(5000);

	switch_fini();
}

#ifdef RTCONFIG_NEW_PHYMAP
void mt7621_get_phy_port_mapping(phy_port_mapping *port_mapping)
{
	int i, id;
	static phy_port_mapping port_mapping_static = {
#if defined(RT4GAX56)
		.count = 6,
		.is_mobile_router = 1,
		.port[0] = { .phy_port_id = -1, .ext_port_id = 0, .label_name = "W0", .cap = PHY_PORT_CAP_WAN, .max_rate = 1000, .ifname = "eth1", .flag = 0, .seq_no = -1, .ui_display = NULL },
		.port[1] = { .phy_port_id = -1, .ext_port_id = 1, .label_name = "L1", .cap = PHY_PORT_CAP_LAN, .max_rate = 1000, .ifname = "eth0", .flag = 0, .seq_no = -1, .ui_display = NULL },
		.port[2] = { .phy_port_id = -1, .ext_port_id = 2, .label_name = "L2", .cap = PHY_PORT_CAP_LAN, .max_rate = 1000, .ifname = "eth0", .flag = 0, .seq_no = -1, .ui_display = NULL },
		.port[3] = { .phy_port_id = -1, .ext_port_id = 3, .label_name = "L3", .cap = PHY_PORT_CAP_LAN, .max_rate = 1000, .ifname = "eth0", .flag = 0, .seq_no = -1, .ui_display = NULL },
		.port[4] = { .phy_port_id = -1, .ext_port_id = 4, .label_name = "L4", .cap = PHY_PORT_CAP_LAN, .max_rate = 1000, .ifname = "eth0", .flag = 0, .seq_no = -1, .ui_display = NULL },
		.port[5] = { .phy_port_id = -1, .ext_port_id = -1, .label_name = "M1", .cap = PHY_PORT_CAP_MOBILE, .max_rate = 480, .ifname = "usb0", .flag = 0, .seq_no = -1, .ui_display = NULL }
#elif defined(RT4GAC86U)
		.count = 7,
		.is_mobile_router = 1,
		.port[0] = { .phy_port_id = -1, .ext_port_id = -1, .label_name = "W0", .cap = PHY_PORT_CAP_LAN, .max_rate = 1000, .ifname = NULL, .flag = 0, .seq_no = -1, .ui_display = NULL },
		.port[1] = { .phy_port_id = -1, .ext_port_id = -1, .label_name = "L1", .cap = PHY_PORT_CAP_LAN, .max_rate = 1000, .ifname = NULL, .flag = 0, .seq_no = -1, .ui_display = NULL },
		.port[2] = { .phy_port_id = -1, .ext_port_id = -1, .label_name = "L2", .cap = PHY_PORT_CAP_LAN, .max_rate = 1000, .ifname = NULL, .flag = 0, .seq_no = -1, .ui_display = NULL },
		.port[3] = { .phy_port_id = -1, .ext_port_id = -1, .label_name = "L3", .cap = PHY_PORT_CAP_LAN, .max_rate = 1000, .ifname = NULL, .flag = 0, .seq_no = -1, .ui_display = NULL },
		.port[4] = { .phy_port_id = -1, .ext_port_id = -1, .label_name = "L4", .cap = PHY_PORT_CAP_LAN, .max_rate = 1000, .ifname = NULL, .flag = 0, .seq_no = -1, .ui_display = NULL },
		.port[5] = { .phy_port_id = -1, .ext_port_id = -1, .label_name = "U1", .cap = PHY_PORT_CAP_USB, .max_rate = 480, .ifname = NULL, .flag = 0, .seq_no = -1, .ui_display = NULL },
		.port[6] = { .phy_port_id = -1, .ext_port_id = -1, .label_name = "M1", .cap = PHY_PORT_CAP_MOBILE, .max_rate = 480, .ifname = NULL, .flag = 0, .seq_no = -1, .ui_display = NULL }
#elif defined(RTACRH18)
		.count = 6,
		.port[0] = { .phy_port_id = -1, .ext_port_id = -1, .label_name = "W0", .cap = PHY_PORT_CAP_LAN, .max_rate = 1000, .ifname = NULL, .flag = 0, .seq_no = -1, .ui_display = NULL },
		.port[1] = { .phy_port_id = -1, .ext_port_id = -1, .label_name = "L1", .cap = PHY_PORT_CAP_LAN, .max_rate = 1000, .ifname = NULL, .flag = 0, .seq_no = -1, .ui_display = NULL },
		.port[2] = { .phy_port_id = -1, .ext_port_id = -1, .label_name = "L2", .cap = PHY_PORT_CAP_LAN, .max_rate = 1000, .ifname = NULL, .flag = 0, .seq_no = -1, .ui_display = NULL },
		.port[3] = { .phy_port_id = -1, .ext_port_id = -1, .label_name = "L3", .cap = PHY_PORT_CAP_LAN, .max_rate = 1000, .ifname = NULL, .flag = 0, .seq_no = -1, .ui_display = NULL },
		.port[4] = { .phy_port_id = -1, .ext_port_id = -1, .label_name = "L4", .cap = PHY_PORT_CAP_LAN, .max_rate = 1000, .ifname = NULL, .flag = 0, .seq_no = -1, .ui_display = NULL },
		.port[5] = { .phy_port_id = -1, .ext_port_id = -1, .label_name = "U1", .cap = PHY_PORT_CAP_USB, .max_rate = 5000, .ifname = NULL, .flag = 0, .seq_no = -1, .ui_display = NULL },
#elif defined(XD4S)
		.count = 2,
		.port[0] = { .phy_port_id = -1, .ext_port_id = -1, .label_name = "W0", .cap = PHY_PORT_CAP_LAN, .max_rate = 1000, .ifname = NULL, .flag = 0, .seq_no = -1, .ui_display = NULL },
		.port[1] = { .phy_port_id = -1, .ext_port_id = -1, .label_name = "L1", .cap = PHY_PORT_CAP_LAN, .max_rate = 1000, .ifname = NULL, .flag = 0, .seq_no = -1, .ui_display = NULL },
#elif defined(RTCONFIG_3LANPORT_DEVICE)
		.count = 4,
		.port[0] = { .phy_port_id = WAN_PORT, .ext_port_id = -1, .label_name = "W0", .cap = PHY_PORT_CAP_WAN, .max_rate = 1000, .ifname = NULL, .flag = 0, .seq_no = -1, .ui_display = NULL },
		.port[1] = { .phy_port_id = LAN1_PORT, .ext_port_id = -1, .label_name = "L1", .cap = PHY_PORT_CAP_LAN, .max_rate = 1000, .ifname = NULL, .flag = 0, .seq_no = -1, .ui_display = NULL },
		.port[2] = { .phy_port_id = LAN2_PORT, .ext_port_id = -1, .label_name = "L2", .cap = PHY_PORT_CAP_LAN, .max_rate = 1000, .ifname = NULL, .flag = 0, .seq_no = -1, .ui_display = NULL },
		.port[3] = { .phy_port_id = LAN3_PORT, .ext_port_id = -1, .label_name = "L3", .cap = PHY_PORT_CAP_LAN, .max_rate = 1000, .ifname = NULL, .flag = 0, .seq_no = -1, .ui_display = NULL },
#else
		.count = 5,
		.port[0] = { .phy_port_id = WAN_PORT, .ext_port_id = -1, .label_name = "W0", .cap = PHY_PORT_CAP_WAN, .max_rate = 1000, .ifname = NULL, .flag = 0, .seq_no = -1, .ui_display = NULL },
		.port[1] = { .phy_port_id = LAN1_PORT, .ext_port_id = -1, .label_name = "L1", .cap = PHY_PORT_CAP_LAN, .max_rate = 1000, .ifname = NULL, .flag = 0, .seq_no = -1, .ui_display = NULL },
		.port[2] = { .phy_port_id = LAN2_PORT, .ext_port_id = -1, .label_name = "L2", .cap = PHY_PORT_CAP_LAN, .max_rate = 1000, .ifname = NULL, .flag = 0, .seq_no = -1, .ui_display = NULL },
		.port[3] = { .phy_port_id = LAN3_PORT, .ext_port_id = -1, .label_name = "L3", .cap = PHY_PORT_CAP_LAN, .max_rate = 1000, .ifname = NULL, .flag = 0, .seq_no = -1, .ui_display = NULL },
		.port[4] = { .phy_port_id = LAN4_PORT, .ext_port_id = -1, .label_name = "L4", .cap = PHY_PORT_CAP_LAN, .max_rate = 1000, .ifname = NULL, .flag = 0, .seq_no = -1, .ui_display = NULL },
#endif
	};
	if (!port_mapping)
		return;

///////////////// copy all Ethernet port here ////////////////////////
	memcpy(port_mapping, &port_mapping_static, sizeof(phy_port_mapping));

///////////////// finetune Ethernet port here ////////////////////////
#if defined(TUFAX4200)
	if (nvram_match("HwId", "A")) { // disable 2.5G LAN
		port_mapping->count = NR_WANLAN_PORT-1;
		port_mapping->port[port_mapping->count].phy_port_id = -1;
	}
#endif

///////////////// Add USB port define here ////////////////////////
#if defined(RTAX53U)
	i = port_mapping->count++;
	port_mapping->port[i].phy_port_id = -1;
	port_mapping->port[i].label_name = "U1";
	port_mapping->port[i].cap = PHY_PORT_CAP_USB;
	port_mapping->port[i].max_rate = 480;
	port_mapping->port[i].ifname = NULL;
	port_mapping->port[i].seq_no = -1;
	port_mapping->port[i].ui_display = NULL;
#elif defined(RTAC85P) || defined(R6800)
	i = port_mapping->count++;
	port_mapping->port[i].phy_port_id = -1;
	port_mapping->port[i].label_name = "U1";
	port_mapping->port[i].cap = PHY_PORT_CAP_USB;
	port_mapping->port[i].max_rate = 5000;
	port_mapping->port[i].ifname = NULL;
	port_mapping->port[i].seq_no = -1;
	port_mapping->port[i].ui_display = NULL;
#elif defined(PANTHERA) || defined(RTAX59U)
////  1 USB3 + 1 USB2 port device
	i = port_mapping->count++;
	port_mapping->port[i].phy_port_id = -1;
	port_mapping->port[i].label_name = "U1";
	port_mapping->port[i].cap = PHY_PORT_CAP_USB;
	port_mapping->port[i].max_rate = 5000;
	port_mapping->port[i].ifname = NULL;
	port_mapping->port[i].seq_no = -1;
	port_mapping->port[i].ui_display = NULL;
	i = port_mapping->count++;
	port_mapping->port[i].phy_port_id = -1;
	port_mapping->port[i].label_name = "U2";
	port_mapping->port[i].cap = PHY_PORT_CAP_USB;
	port_mapping->port[i].max_rate = 480;
	port_mapping->port[i].ifname = NULL;
	port_mapping->port[i].seq_no = -1;
	port_mapping->port[i].ui_display = NULL;
#endif

	add_sw_cap(port_mapping);
	swap_wanlan(port_mapping);
	return;
}
#endif // end of RTCONFIG_NEW_PHYMAP

#if 0
void usage(char *cmd)
{
	printf("Usage: %s 1 - set (SCK, SD) as (0, 1)\n", cmd);
	printf("       %s 2 - set (SCK, SD) as (1, 0)\n", cmd);
	printf("       %s 3 - set (SCK, SD) as (1, 1)\n", cmd);
	printf("       %s 0 - set (SCK, SD) as (0, 0)\n", cmd);
	printf("       %s 4 - init vlan\n", cmd);
	printf("       %s 5 - set cpu port 0 0\n", cmd);
	printf("       %s 6 - set cpu port 0 1\n", cmd);
	printf("       %s 7 - set cpu port 1 0\n", cmd);
	printf("       %s 8 - set cpu port 1 1\n", cmd);
	printf("       %s 10 - set vlan entry, no cpu port, but mbr\n", cmd);
	printf("       %s 11 - set vlan entry, no cpu port, no mbr\n", cmd);
	printf("       %s 15 - set vlan PVID, no cpu port\n", cmd);
	printf("       %s 20 - set vlan entry, with cpu port\n", cmd);
	printf("       %s 21 - set vlan entry, with cpu port and no cpu port in untag sets\n", cmd);
	printf("       %s 25 - set vlan PVID, with cpu port\n", cmd);
	printf("       %s 26 - set vlan PVID, not set cpu port\n", cmd);
	printf("       %s 90 - accept all frmaes\n", cmd);
	printf("       %s 66 - setup default vlan\n", cmd);
	printf("       %s 61 - setup vlan type1\n", cmd);
	printf("       %s 62 - setup vlan type2\n", cmd);
	printf("       %s 63 - setup vlan type3\n", cmd);
	printf("       %s 64 - setup vlan type4\n", cmd);
	printf("       %s 65 - setup vlan type34\n", cmd);
	printf("       %s 70 - disable multicast snooping\n", cmd);
	printf("       %s 81 - setRtctTesting on port x\n", cmd);
	printf("       %s 82 - getRtctResult on port x\n", cmd);
	printf("       %s 83 - setGreenEthernet x(green, powsav)\n", cmd);
	printf("       %s 84 - setAsicGreenFeature x(txGreen, rxGreen)\n", cmd);
	printf("       %s 85 - getAsicGreenFeature\n", cmd);
	printf("       %s 86 - enable GreenEthernet on port x\n", cmd);
	printf("       %s 87 - disable GreenEthernet on port x\n", cmd);
	printf("       %s 88 - getAsicPowerSaving x\n", cmd);
	printf("       %s 50 - getPortLinkStatus x\n", cmd);
	exit(0);
}
#endif
