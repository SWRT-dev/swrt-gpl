/*
 * Copyright 2021, ASUS
 * Copyright 2021-2023, SWRTdev
 * Copyright 2021-2023, paldier <paldier@hotmail.com>.
 * Copyright 2021-2023, lostlonger<lostlonger.g@gmail.com>.
 * All Rights Reserved.
 */

#include "rc.h"

#include <termios.h>
#include <dirent.h>
#include <sys/ioctl.h>
#include <sys/mount.h>
#include <time.h>
#include <errno.h>
#include <paths.h>
#include <sys/wait.h>
#include <sys/reboot.h>
#include <sys/klog.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sysinfo.h>
#include <wlutils.h>
#include <bcmdevs.h>
#include <shared.h>
#include <ralink.h>
#include <flash_mtd.h>
#include <swrt.h>
#if defined(RTCONFIG_EASYMESH)
#include <swrtmesh.h>
#endif

void init_devs(void)
{
#define MKNOD(name,mode,dev)	if(mknod(name,mode,dev)) perror("## mknod " name)
	MKNOD("/dev/video0", S_IFCHR | 0666, makedev(81, 0));
	MKNOD("/dev/spiS0", S_IFCHR | 0666, makedev(217, 0));
	MKNOD("/dev/i2cM0", S_IFCHR | 0666, makedev(218, 0));
#if defined(RTCONFIG_RALINK_MT7621) || defined(RTCONFIG_RALINK_EN7561)
	MKNOD("/dev/rdm0", S_IFCHR | 0x666, makedev(253, 0));
#else
	MKNOD("/dev/rdm0", S_IFCHR | 0666, makedev(254, 0));
#endif
	MKNOD("/dev/flash0", S_IFCHR | 0666, makedev(200, 0));
	MKNOD("/dev/swnat0", S_IFCHR | 0666, makedev(210, 0));
	MKNOD("/dev/hwnat0", S_IFCHR | 0666, makedev(220, 0));
	MKNOD("/dev/acl0", S_IFCHR | 0666, makedev(230, 0));
	MKNOD("/dev/ac0", S_IFCHR | 0666, makedev(240, 0));
	MKNOD("/dev/mtr0", S_IFCHR | 0666, makedev(250, 0));
	MKNOD("/dev/gpio0", S_IFCHR | 0666, makedev(252, 0));
	MKNOD("/dev/nvram", S_IFCHR | 0666, makedev(228, 0));
	MKNOD("/dev/PCM", S_IFCHR | 0666, makedev(233, 0));
	MKNOD("/dev/I2S", S_IFCHR | 0666, makedev(234, 0));
	{
		int status;
		if((status = WEXITSTATUS(modprobe("nvram_linux"))))	printf("## modprove(nvram_linux) fail status(%d)\n", status);
	}
	if(patch_Factory)
		patch_Factory();
}

void init_others(void)
{
#if defined(RTACRH18)
    // mii_mgr_cl45 -s -p 0 -d 0x1f -r 21 -v 8009 --> WAN LED 0x21 LED Basic Control Register Set 0x8009
    eval("mii_mgr_cl45", "-s", "-p", "0", "-d", "0x1f", "-r", "21", "-v", "8009");
    // mii_mgr_cl45 -s -p 0 -d 0x1f -r 24 -v c007 --> WAN LED LED0 On Control Register
    eval("mii_mgr_cl45", "-s", "-p", "0", "-d", "0x1f", "-r", "24", "-v", "c007");
    // mii_mgr_cl45 -s -p 0 -d 0x1f -r 25 -v C13F --> WAN LED LED0 Blinking Control Register
    eval("mii_mgr_cl45", "-s", "-p", "0", "-d", "0x1f", "-r", "25", "-v", "c13f");
#elif defined(RTAX53U) || defined(RTAX54) || defined(XD4S)
	eval("mii_mgr", "-s", "-p", "0", "-r", "13", "-v", "0x1f");
	eval("mii_mgr", "-s", "-p", "0", "-r", "14", "-v", "0x24");
	eval("mii_mgr", "-s", "-p", "0", "-r", "13", "-v", "0x401f");
	eval("mii_mgr", "-s", "-p", "0", "-r", "14", "-v", "0xc007");
	eval("mii_mgr", "-s", "-p", "0", "-r", "13", "-v", "0x1f");
	eval("mii_mgr", "-s", "-p", "0", "-r", "14", "-v", "0x25");
	eval("mii_mgr", "-s", "-p", "0", "-r", "13", "-v", "0x401f");
	eval("mii_mgr", "-s", "-p", "0", "-r", "14", "-v", "0x3f");
#elif defined(RTAC85U) || defined(RTAC85P) || defined(R6800) || defined(RMAC2100)
//fix me
#endif
#if defined(TUFAX4200)
	if (nvram_match("HwId", "B")) {
		mount("overlay", "/www/images", "overlay", MS_MGC_VAL, "lowerdir=/TUF-AX4200Q/images:/www/images");
		mount("/rom/dlna.TUF-AX4200Q", "/rom/dlna", "none", MS_BIND, NULL);
	}
#endif
#if defined(RTCONFIG_MT798X)
	if (nvram_match("lacp_enabled", "1"))
		f_write_string("/sys/kernel/no_dsa_offload", "1", 0, 0);
	nvram_unset("wifidat_dbg");
#endif
}

int is_if_up(char *ifname)
{
    int s;
    struct ifreq ifr;

    /* Open a raw socket to the kernel */
    if ((s = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) < 0) return -1;

    /* Set interface name */
    strlcpy(ifr.ifr_name, ifname, IFNAMSIZ);

    /* Get interface flags */
    if (ioctl(s, SIOCGIFFLAGS, &ifr) < 0) {
        fprintf(stderr, "SIOCGIFFLAGS error\n");
    } else {
        if (ifr.ifr_flags & IFF_UP) {
            fprintf(stderr, "%s is up\n", ifname);
            close(s);
            return 1;
        }
    }
    close(s);
    return 0;
}

//void init_gpio(void)
//{
//	ralink_gpio_init(0, GPIO_DIR_OUT); // Power
//	ralink_gpio_init(13, GPIO_DIR_IN); // RESET
//	ralink_gpio_init(26, GPIO_DIR_IN); // WPS
//}

void generate_switch_para(void)
{
	int model;
	int wans_cap = get_wans_dualwan() & WANSCAP_WAN;
	int wanslan_cap = get_wans_dualwan() & WANSCAP_LAN;

	// generate nvram nvram according to system setting
	model = get_model();

	switch(model) {
		case MODEL_RTAC85U:
		case MODEL_RTAC85P:
		case MODEL_RMAC2100:
		case MODEL_R6800:
		case MODEL_TUFAC1750:
		case MODEL_RTAX53U:
		case MODEL_RTAX54:
		case MODEL_PGBM1:
		case MODEL_JCGQ10PRO:
		case MODEL_H3CTX1801:
		case MODEL_XMCR660X:
		case MODEL_JCGQ20:
			nvram_unset("vlan3hwname");
			if ((wans_cap && wanslan_cap) ||
			    (wanslan_cap && (!nvram_match("switch_wantag", "none") && !nvram_match("switch_wantag", "")))
			   )
				nvram_set("vlan3hwname", "et0");
			break;

		case MODEL_RT4GAC86U:
			nvram_unset("vlan3hwname");
			nvram_unset("vlan1hwname");
			if ((wans_cap && wanslan_cap) ||
				(wanslan_cap && (!nvram_match("switch_wantag", "none") && !nvram_match("switch_wantag", "")))
				) {
					nvram_set("vlan3hwname", "et0");
					nvram_set("vlan1hwname", "et0");
				}
			break;

		default:
			nvram_unset("vlan3hwname");
			if ((wans_cap && wanslan_cap) ||
			    (wanslan_cap && (!nvram_match("switch_wantag", "none") && !nvram_match("switch_wantag", "")))
			   )
				nvram_set("vlan3hwname", "et0");
	}
}

static void init_switch_ralink(void)
{
	generate_switch_para();

	// TODO: replace to nvram controlled procedure later
#if 0
	eval("ifconfig", "eth2", "hw", "ether", get_lan_hwaddr());
#else
	eval("ifconfig", "eth0", "hw", "ether", get_lan_hwaddr());
#endif
#if !defined(RTCONFIG_CONCURRENTREPEATER)		
#if defined(RTCONFIG_AMAS) || defined(RTCONFIG_EASYMESH)
	if (sw_mode() == SW_MODE_AP && nvram_match("re_mode", "1")) {
		if(strlen(nvram_safe_get("eth_ifnames"))) {
			if (!nvram_match("et1macaddr", ""))
				eval("ifconfig", nvram_safe_get("eth_ifnames"), "hw", "ether", nvram_safe_get("et1macaddr"));
			else
				eval("ifconfig", nvram_safe_get("eth_ifnames"), "hw", "ether", nvram_safe_get("et0macaddr"));
		}
	}
	else
#endif
	{
		if(strlen(nvram_safe_get("wan0_ifname"))) {
			if (!nvram_match("et1macaddr", ""))
				eval("ifconfig", nvram_safe_get("wan0_ifname"), "hw", "ether", nvram_safe_get("et1macaddr"));
			else
				eval("ifconfig", nvram_safe_get("wan0_ifname"), "hw", "ether", nvram_safe_get("et0macaddr"));
		}
	}
#endif
//workaround, let network device initialize before config_switch()
#if 0
	eval("ifconfig", "eth2", "up");
#else
	eval("ifconfig", "eth0", "up");
#endif
	sleep(1);


#if !defined(RTCONFIG_RALINK_MT7628)
	config_switch();
#endif

#ifdef RTCONFIG_SHP
	if (nvram_get_int("qos_enable") == 1 || nvram_get_int("lfp_disable_force")) {
		nvram_set("lfp_disable", "1");
	} else {
		nvram_set("lfp_disable", "0");
	}

	if(nvram_get_int("lfp_disable")==0) {
		restart_lfp();
	}
#endif
#if defined(RTCONFIG_WLMODULE_MT7629_AP) || defined(RTCONFIG_SWITCH_MT7986_MT7531)
	if(nvram_get_int("jumbo_frame_enable")==0)
		eval("switch", "reg", "w", "30e0", "3e3d");		// MAX_RX_PKT_LEN: 1:1536 bytes
	else
		eval("switch", "reg", "w", "30e0", "3e3f");		// MAX_RX_PKT_LEN: 3:MAX_RX_JUMBO
#endif
//	reinit_hwnat(-1);

}

void init_switch()
{
#ifdef RTCONFIG_DSL
	init_switch_dsl();
	config_switch_dsl();	
#else
	init_switch_ralink();
#endif
}

/* Configure LED controlled by hardware. Turn off LED by default. */
void config_hwctl_led(void)
{
#if defined(TUFAX4200) || defined(TUFAX6000)
	/* GPY211 PHY LED */
	/* 2.5G LAN: LED0, port5, active high, disable LED function and turn it OFF */
	eval("mii_mgr", "-s", "-p", "5", "-d", "0", "-r", "0x1b", "-v",  "0x000");	/* Reg 0.27 */
	/* Blinks on TX/RX, ON on 10M/100M/1G/2.5G link */
	eval("mii_mgr", "-s", "-p", "5", "-d", "0x1e", "-r", "1", "-v",  "0x3f0");	/* Reg 30.1 */

	/* 2.5G WAN: LED0, port6, active low, disable LED function and turn it OFF */
	eval("mii_mgr", "-s", "-p", "6", "-d", "0", "-r", "0x1b", "-v", "0x1000");	/* Reg 0.27 */
	/* Blinks on TX/RX, ON on 10M/100M/1G/2.5G link */
	eval("mii_mgr", "-s", "-p", "6", "-d", "0x1e", "-r", "1", "-v",  "0x3f0");	/* Reg 30.1 */

	/* MT7531 switch LED */
	eval("switch", "phy", "cl45", "w", "0", "0x1f", "0x21",    "0x8");	/* Use LED_MODE and disabled LED */
	eval("switch", "phy", "cl45", "w", "0", "0x1f", "0x22",  "0xc00");	/* Link on duration */
	eval("switch", "phy", "cl45", "w", "0", "0x1f", "0x23", "0x1400");	/* Blink duration */
	eval("switch", "phy", "cl45", "w", "0", "0x1f", "0x24", "0x8000");	/* Enable LED0, active low, ON no events */
	eval("switch", "phy", "cl45", "w", "0", "0x1f", "0x25",    "0x0");	/* LED0, none of any events blink */
	eval("switch", "phy", "cl45", "w", "0", "0x1f", "0x26", "0xc007");	/* Enable LED1, active high, link 10M/100M/1G ON */
	eval("switch", "phy", "cl45", "w", "0", "0x1f", "0x27",   "0x3f");	/* LED1, blinks on 10M/100M/1G TX/RX activity */
#endif
}

#ifndef RTCONFIG_SWCONFIG
/**
 * Setup a VLAN.
 * @vid:	VLAN ID
 * @prio:	VLAN PRIO
 * @mask:	bit31~16:	untag mask
 * 		bit15~0:	port member mask
 * @return:
 * 	0:	success
 *  otherwise:	fail
 *
 * bit definition of untag mask/port member mask
 * 0:	Port 0, LANx port which is closed to WAN port in visual.
 * 1:	Port 1
 * 2:	Port 2
 * 3:	Port 3
 * 4:	Port 4, WAN port
 * 9:	Port 9, RGMII/MII port that is used to connect CPU and WAN port.
 * 	a. If you only have one RGMII/MII port and it is shared by WAN/LAN ports,
 * 	   you have to define two VLAN interface for WAN/LAN ports respectively.
 * 	b. If your switch chip choose another port as same feature, convert bit9
 * 	   to your own port in low-level driver.
 */
static int __setup_vlan(int vid, int prio, unsigned int mask)
{
	char vlan_str[] = "4096XXX";
	char prio_str[] = "7XXX";
	char mask_str[] = "0x00000000XXX";
	char *set_vlan_argv[] = { "rtkswitch", "36", vlan_str , NULL };
	char *set_prio_argv[] = { "rtkswitch", "37", prio_str , NULL };
	char *set_mask_argv[] = { "rtkswitch", "39", mask_str , NULL };

	if (vid > 4096) {
		_dprintf("%s: invalid vid %d\n", __func__, vid);
		return -1;
	}

	if (prio > 7)
		prio = 0;

	_dprintf("%s: vid %d prio %d mask 0x%08x\n", __func__, vid, prio, mask);

	if (vid >= 0) {
		sprintf(vlan_str, "%d", vid);
		_eval(set_vlan_argv, NULL, 0, NULL);
	}

	if (prio >= 0) {
		sprintf(prio_str, "%d", prio);
		_eval(set_prio_argv, NULL, 0, NULL);
	}

	sprintf(mask_str, "0x%08x", mask);
	_eval(set_mask_argv, NULL, 0, NULL);

	return 0;
}
#endif

#if defined(RT4GAC86U)
/*set internal  vlan id and associated member on port 5/6/7*/
static int _set_vlan_mbr(int vid)
{
	char vlan_str[] = "4096XXX";
	char *set_vlan_argv[] = { "mtkswitch", "1", vlan_str , NULL };

	if (vid > 4096) {
		_dprintf("%s: invalid vid %d\n", __func__, vid);
		return -1;
	}

	_dprintf("%s: vid %d \n", __func__, vid);

	if (vid >= 0) {
		sprintf(vlan_str, "%d", vid);
		_eval(set_vlan_argv, NULL, 0, NULL);
	}

	return 0;
}

/*set port accept tag only frame type for VoIP */
static int _set_portAcceptFrameType(int port)
{
	char port_str[] = "7XXX";
	char *set_portAcceptFrameType_argv[] = { "rtkswitch", "35", port_str , NULL };

	if (port >= 0) {
		sprintf(port_str, "%d", port);
		_eval(set_portAcceptFrameType_argv, NULL, 0, NULL);
	}
}
#endif

int config_switch_for_first_time = 1;
#ifndef RTCONFIG_SWCONFIG

#if defined(RTCONFIG_3LANPORT_DEVICE)
int lan_port_bit_shift = 1;
#else
#if defined(RTCONFIG_PORT2_DEVICE)
#if defined(XD4S)
int lan_port_bit_shift = 3; //in order to specify LAN1 of switch_port_mapping
#else
//TBD
#endif
#else
int lan_port_bit_shift = 0;
#endif
#endif

static int stb_bitmask_shift(int input)
{
	int output = input << lan_port_bit_shift;
#if defined(RTCONFIG_PORT2_DEVICE)
	if(input!=0)
		output=1<< lan_port_bit_shift; //match LAN1 of switch_port_mapping
#endif	
	return output;
}

static int vlan_bitmask_shift(unsigned int input)
{
#define NR_WANLAN_PORT 5	
	unsigned int output = (input & 0xFFF0FFF0) | ((input & 0x000F000F) << lan_port_bit_shift);
#if defined(RTCONFIG_PORT2_DEVICE)
	int i;
	int mask=input & 0xFFF0FFF0;
	int lan_untag=(input & 0x000F0000)>>16;
	int lan_mbr  =input & 0x0000000F;
	for(i = 0; i < NR_WANLAN_PORT-1; i++) 
	{
		lan_untag=((input & 0x000F0000)>>(16+i)) & 0x1;
		lan_mbr  =((input & 0x0000000F) >>i) & 0x1;
		if(lan_mbr)
		{
			output=mask | ((lan_untag <<16) | lan_mbr) << lan_port_bit_shift;
			return output;
		}	
	}	
#endif
	return output;
}

void config_switch()
{
	int model = get_model();
	int stbport;
	int controlrate_unknown_unicast;
	int controlrate_unknown_multicast;
	int controlrate_multicast;
	int controlrate_broadcast;
	int merge_wan_port_into_lan_ports;
	int stb_bitmask;
	unsigned int vlan_bitmask;
	char stb_bitmask_str[sizeof("0xXXXXXXXXYYY")];

	dbG("link down all ports\n");
	eval("rtkswitch", "17");	// link down all ports

	switch (model) {
	case MODEL_RTAC85U:
	case MODEL_RTAC85P:
	case MODEL_RMAC2100:
	case MODEL_R6800:
	case MODEL_TUFAC1750:
	case MODEL_RTAX53U:
	case MODEL_RTAX54:
	case MODEL_PGBM1:
	case MODEL_JCGQ10PRO:
	case MODEL_H3CTX1801:
	case MODEL_XMCR660X:
	case MODEL_JCGQ20:
		merge_wan_port_into_lan_ports = 1;
		break;
	default:
		merge_wan_port_into_lan_ports = 0;
	}

	if (config_switch_for_first_time){
			config_switch_for_first_time = 0;
	}
	else
	{
		dbG("software reset\n");
		eval("rtkswitch", "27");	// software reset
	}
	pre_config_switch();
	system("rtkswitch 8 0"); //Barton add

	if (is_routing_enabled())
	{
		char parm_buf[] = "XXX";

		stbport = nvram_get_int("switch_stb_x");
		if (stbport < 0 || stbport > 6) stbport = 0;
#if defined(RTCONFIG_PORT2_DEVICE)
		if(stbport!=0)
			stbport=1;
#endif			
		dbG("ISP Profile/STB: %s/%d\n", nvram_safe_get("switch_wantag"), stbport);
#if defined(RTCONFIG_RALINK_MT7628)
		/* P0    P1    P2    P3    P4    P6  */
		/* WAN   L1    L2    L3    L4    CPU */
#else
		/* stbport:	Model-independent	unifi_malaysia=1	otherwise
		 * 		IPTV STB port		(RT-N56U)		(RT-N56U)
		 * -----------------------------------------------------------------------
		 *	0:	N/A			LLLLW
		 *	1:	LAN1			LLLTW			LLLWW
		 *	2:	LAN2			LLTLW			LLWLW
		 *	3:	LAN3			LTLLW			LWLLW
		 *	4:	LAN4			TLLLW			WLLLW
		 *	5:	LAN1 + LAN2		LLTTW			LLWWW
		 *	6:	LAN3 + LAN4		TTLLW			WWLLW
		 */
#endif
		if(!nvram_match("switch_wantag", "none")&&!nvram_match("switch_wantag", ""))//2012.03 Yau modify
		{
			int voip_port __attribute__((unused))= 0;
			int t, vlan_val = -1, prio_val = -1;
			unsigned int mask = 0;

#if defined(RTCONFIG_RALINK_MT7628)
			/* Create WAN VLAN interface */
			if (nvram_get_int("switch_wan0tagid") != 0) {
				char wan_dev[10];
				//eval("vconfig", "rem", "vlan2");
				eval("vconfig", "add", "eth2", nvram_safe_get("switch_wan0tagid"));

				snprintf(wan_dev, sizeof(wan_dev), "vlan%d", nvram_get_int("switch_wan0tagid"));

				prio_val = nvram_get_int("switch_wan1prio");
				if (prio_val >= 0 && prio_val <= 7)
					eval("vconfig", "set_egress_map", wan_dev, "0", nvram_get("switch_wan0prio"));
			}
#endif
#if defined(RTCONFIG_RALINK_MT7629)
			/* Create WAN VLAN interface */
			if (nvram_get_int("switch_wan0tagid") != 0) {
				char wan_dev[10];
				eval("vconfig", "rem", "vlan2");
				eval("vconfig", "add", "eth2", nvram_safe_get("switch_wan0tagid"));

				snprintf(wan_dev, sizeof(wan_dev), "vlan%d", nvram_get_int("switch_wan0tagid"));

				prio_val = nvram_get_int("switch_wan1prio");
				if (prio_val >= 0 && prio_val <= 7)
					eval("vconfig", "set_egress_map", wan_dev, "0", nvram_get("switch_wan0prio"));
			}
#endif
			switch (model) {
			default:

				/* Fixed Ports Now*/
#if defined(RTCONFIG_PORT2_DEVICE)
				stbport= 1;
#else
				stbport = 4;	
#endif				
				voip_port = 3;

				if(!strncmp(nvram_safe_get("switch_wantag"), "unifi", 5)) {
					/* Added for Unifi. Cherry Cho modified in 2011/6/28.*/
					if(strstr(nvram_safe_get("switch_wantag"), "home")) {
						stb_bitmask = stb_bitmask_shift(1);
						snprintf(stb_bitmask_str, sizeof(stb_bitmask_str), "0x%x", stb_bitmask);
						eval("rtkswitch", "38", stb_bitmask_str);	/* IPTV: P0 */

						/* Internet:	untag: P9;   port: P4, P9 */
						vlan_bitmask = vlan_bitmask_shift(0x02000210);
						__setup_vlan(500, 0, vlan_bitmask);

						/* IPTV:	untag: P0;   port: P0, P4 */
						vlan_bitmask = vlan_bitmask_shift(0x00010011);
						__setup_vlan(600, 0, vlan_bitmask);
					}
					else {
						/* No IPTV. Business package */
						/* Internet:	untag: P9;   port: P4, P9 */
						stb_bitmask = stb_bitmask_shift(0);
						snprintf(stb_bitmask_str, sizeof(stb_bitmask_str), "0x%x", stb_bitmask);
						eval("rtkswitch", "38", stb_bitmask_str);

						vlan_bitmask = vlan_bitmask_shift(0x02000210);
						__setup_vlan(500, 0, vlan_bitmask);
					}
				}
				else if(!strncmp(nvram_safe_get("switch_wantag"), "singtel", 7)) {
					/* Added for SingTel's exStream issues. Cherry Cho modified in 2011/7/19. */
					if(strstr(nvram_safe_get("switch_wantag"), "mio")) {
						/* Connect Singtel MIO box to P3 */
						system("rtkswitch 40 1");		/* admin all frames on all ports */
						stb_bitmask = stb_bitmask_shift(3);
						snprintf(stb_bitmask_str, sizeof(stb_bitmask_str), "0x%x", stb_bitmask);
						eval("rtkswitch", "38", stb_bitmask_str);	/* IPTV: P0  VoIP: P1 */

						/* Internet:	untag: P9;   port: P4, P9 */
						vlan_bitmask = vlan_bitmask_shift(0x02000210);
						__setup_vlan(10, 0, vlan_bitmask);

						/* VoIP:	untag: N/A;  port: P1, P4 */
						//VoIP Port: P1 tag
						vlan_bitmask = vlan_bitmask_shift(0x00000012);
						__setup_vlan(30, 4, vlan_bitmask);
					}
					else {
						//Connect user's own ATA to lan port and use VoIP by Singtel WAN side VoIP gateway at voip.singtel.com
						stb_bitmask = stb_bitmask_shift(1);
						eval("rtkswitch", "38", stb_bitmask_str);	/* IPTV: P0 */

						/* Internet:	untag: P9;   port: P4, P9 */
						vlan_bitmask = vlan_bitmask_shift(0x02000210);
						__setup_vlan(10, 0, vlan_bitmask);
					}

					/* IPTV */
					if(strstr(nvram_safe_get("switch_wantag"), "mstb"))
						vlan_bitmask = vlan_bitmask_shift(0x00030013);	/* untag: P0, P1;   port: P0, P1, P4 */
					else
						vlan_bitmask = vlan_bitmask_shift(0x00010011);	/* untag: P0;   port: P0, P4 */

					__setup_vlan(20, 4, vlan_bitmask);
				}
				else if(!strcmp(nvram_safe_get("switch_wantag"), "m1_fiber")) {
					//VoIP: P1 tag. Cherry Cho added in 2012/1/13.
					system("rtkswitch 40 1");			/* admin all frames on all ports */
					stb_bitmask = stb_bitmask_shift(2);
					snprintf(stb_bitmask_str, sizeof(stb_bitmask_str), "0x%x", stb_bitmask);
					eval("rtkswitch", "38", stb_bitmask_str);		/* VoIP: P1  2 = 0x10 */

					/* Internet:	untag: P9;   port: P4, P9 */
					vlan_bitmask = vlan_bitmask_shift(0x02000210);
					__setup_vlan(1103, 1, vlan_bitmask);

					/* VoIP:	untag: N/A;  port: P1, P4 */
					//VoIP Port: P1 tag
					vlan_bitmask = vlan_bitmask_shift(0x00000012);
					__setup_vlan(1107, 1, vlan_bitmask);
				}
				else if(!strcmp(nvram_safe_get("switch_wantag"), "maxis_fiber")) {
					//VoIP: P1 tag. Cherry Cho added in 2012/11/6.
					system("rtkswitch 40 1");			/* admin all frames on all ports */
					stb_bitmask = stb_bitmask_shift(2);
					snprintf(stb_bitmask_str, sizeof(stb_bitmask_str), "0x%x", stb_bitmask);
					eval("rtkswitch", "38", stb_bitmask_str);		/* VoIP: P1  2 = 0x10 */

					/* Internet:	untag: P9;   port: P4, P9 */
					vlan_bitmask = vlan_bitmask_shift(0x02000210);
					__setup_vlan(621, 0, vlan_bitmask);

					/* VoIP:	untag: N/A;  port: P1, P4 */
					vlan_bitmask = vlan_bitmask_shift(0x00000012);
					__setup_vlan(821, 0, vlan_bitmask);

					vlan_bitmask = vlan_bitmask_shift(0x00000012);
					__setup_vlan(822, 0, vlan_bitmask);		/* untag: N/A;  port: P1, P4 */ //VoIP Port: P1 tag
				}
				else if(!strcmp(nvram_safe_get("switch_wantag"), "maxis_fiber_sp")) {
					//VoIP: P1 tag. Cherry Cho added in 2012/11/6.
					system("rtkswitch 40 1");			/* admin all frames on all ports */
					stb_bitmask = stb_bitmask_shift(2);
					snprintf(stb_bitmask_str, sizeof(stb_bitmask_str), "0x%x", stb_bitmask);
					eval("rtkswitch", "38", stb_bitmask_str);		/* VoIP: P1  2 = 0x10 */

					/* Internet:	untag: P9;   port: P4, P9 */
					vlan_bitmask = vlan_bitmask_shift(0x02000210);
					__setup_vlan(11, 0, vlan_bitmask);

					/* VoIP:	untag: N/A;  port: P1, P4 */
					//VoIP Port: P1 tag
					vlan_bitmask = vlan_bitmask_shift(0x00000012);
					__setup_vlan(14, 0, vlan_bitmask);
				}
				else if(!strcmp(nvram_safe_get("switch_wantag"), "maxis_cts")) {
					//VoIP: P1 tag. Cherry Cho added in 2012/11/6.
					system("rtkswitch 40 1");			/* admin all frames on all ports */
					stb_bitmask = stb_bitmask_shift(2);
					snprintf(stb_bitmask_str, sizeof(stb_bitmask_str), "0x%x", stb_bitmask);
					eval("rtkswitch", "38", stb_bitmask_str);		/* VoIP: P1  2 = 0x10 */

					/* Internet:	untag: P9;   port: P4, P9 */
					vlan_bitmask = vlan_bitmask_shift(0x02000210);
					__setup_vlan(41, 0, vlan_bitmask);

					/* VoIP:	untag: N/A;  port: P1, P4 */
					//VoIP Port: P1 tag
					vlan_bitmask = vlan_bitmask_shift(0x00000012);
					__setup_vlan(44, 0, vlan_bitmask);
				}
				else if(!strcmp(nvram_safe_get("switch_wantag"), "maxis_sacofa")) {
					//VoIP: P1 tag. Cherry Cho added in 2012/11/6.
					system("rtkswitch 40 1");			/* admin all frames on all ports */
					stb_bitmask = stb_bitmask_shift(2);
					snprintf(stb_bitmask_str, sizeof(stb_bitmask_str), "0x%x", stb_bitmask);
					eval("rtkswitch", "38", stb_bitmask_str);		/* VoIP: P1  2 = 0x10 */

					/* Internet:	untag: P9;   port: P4, P9 */
					vlan_bitmask = vlan_bitmask_shift(0x02000210);
					__setup_vlan(31, 0, vlan_bitmask);

					/* VoIP:	untag: N/A;  port: P1, P4 */
					//VoIP Port: P1 tag
					vlan_bitmask = vlan_bitmask_shift(0x00000012);
					__setup_vlan(34, 0, vlan_bitmask);
				}
				else if(!strcmp(nvram_safe_get("switch_wantag"), "maxis_tnb")) {
					//VoIP: P1 tag. Cherry Cho added in 2012/11/6.
					system("rtkswitch 40 1");			/* admin all frames on all ports */
					stb_bitmask = stb_bitmask_shift(2);
					snprintf(stb_bitmask_str, sizeof(stb_bitmask_str), "0x%x", stb_bitmask);
					eval("rtkswitch", "38", stb_bitmask_str);			/* VoIP: P1  2 = 0x10 */

					/* Internet:	untag: P9;   port: P4, P9 */
					vlan_bitmask = vlan_bitmask_shift(0x02000210);
					__setup_vlan(51, 0, vlan_bitmask);

					/* VoIP:	untag: N/A;  port: P1, P4 */
					//VoIP Port: P1 tag
					vlan_bitmask = vlan_bitmask_shift(0x00000012);
					__setup_vlan(54, 0, vlan_bitmask);
				}
#ifdef RTCONFIG_MULTICAST_IPTV
			else if (!strcmp(nvram_safe_get("switch_wantag"), "movistar")) {
#if 0	//set in set_wan_tag() since (switch_stb_x > 6) and need vlan interface by vconfig.
				system("rtkswitch 40 1");			/* admin all frames on all ports */
				/* Internet/STB/VoIP:	untag: N/A;   port: P4, P9 */
				__setup_vlan(6, 0, 0x00000210);
				__setup_vlan(2, 0, 0x00000210);
				__setup_vlan(3, 0, 0x00000210);
#endif
			}
				else if (!strcmp(nvram_safe_get("switch_wantag"), "starhub")) {
					//skip setting any lan port to IPTV port.
					stb_bitmask = stb_bitmask_shift(0);
					snprintf(stb_bitmask_str, sizeof(stb_bitmask_str), "0x%x", stb_bitmask);
					eval("rtkswitch", "38", stb_bitmask_str);
#if defined(RTCONFIG_RALINK_MT7620) || defined(RTCONFIG_RALINK_MT7621) || defined(RTCONFIG_RALINK_EN7561)
					vlan_bitmask = vlan_bitmask_shift(0x02100210);
					__setup_vlan(2, 0, vlan_bitmask);
#endif
				}
#endif
				else if (!strcmp(nvram_safe_get("switch_wantag"), "meo")) {
					system("rtkswitch 40 1");			/* admin all frames on all ports */
					stb_bitmask = stb_bitmask_shift(1);
					snprintf(stb_bitmask_str, sizeof(stb_bitmask_str), "0x%x", stb_bitmask);
					eval("rtkswitch", "38", stb_bitmask_str);			/* VoIP: P0 */

					/* Internet/VoIP:	untag: P9;   port: P0, P4, P9 */
					vlan_bitmask = vlan_bitmask_shift(0x02000211);
					__setup_vlan(12, 0, vlan_bitmask);
				}
				else if (!strcmp(nvram_safe_get("switch_wantag"), "vodafone")) {
					system("rtkswitch 40 1");			/* admin all frames on all ports */
					stb_bitmask = stb_bitmask_shift(3);
					snprintf(stb_bitmask_str, sizeof(stb_bitmask_str), "0x%x", stb_bitmask);
					eval("rtkswitch", "38", stb_bitmask_str);		/* Vodafone: P0  IPTV: P1 */

					/* Internet:	untag: P9;   port: P4, P9 */
					vlan_bitmask = vlan_bitmask_shift(0x02000211);
					__setup_vlan(100, 1, vlan_bitmask);

					/* IPTV:	untag: N/A;  port: P0, P4 */
					vlan_bitmask = vlan_bitmask_shift(0x00000011);
					__setup_vlan(101, 0, vlan_bitmask);

					/* Vodafone:	untag: P1;   port: P0, P1, P4 */
					vlan_bitmask = vlan_bitmask_shift(0x00020013);
					__setup_vlan(105, 1, vlan_bitmask);
				}
				else if (!strcmp(nvram_safe_get("switch_wantag"), "hinet")) { /* Hinet MOD */
					if (sw_bridge_iptv_different_switches()) {
						/* Bridge:	untag: P0, P4, P9;	port: P0, P4, P9
						 * WAN:		no VLAN (hacked in API for SW based IPTV)
						 * STB:		Ctag, return value of get_sw_bridge_iptv_vid().
						 */
						__setup_vlan(get_sw_bridge_iptv_vid(), 0, 0x02110211);
					} else {
#if defined(RTCONFIG_3LANPORT_DEVICE)
					eval("rtkswitch", "8", "3");			/* LAN3 with WAN */
#elif defined(RTCONFIG_PORT2_DEVICE)
					eval("rtkswitch", "8", "1");			/* LAN1 with WAN */
#else					
					eval("rtkswitch", "8", "4");			/* LAN4 with WAN */
#endif
					}
				}
				else if (!strcmp(nvram_safe_get("switch_wantag"), "hinet_mesh")) { /* Hinet MOD Mesh */
					/* Nothing to do. */
				}
				else {
					/* Cherry Cho added in 2011/7/11. */
					/* Initialize VLAN and set Port Isolation */
					if(strcmp(nvram_safe_get("switch_wan1tagid"), "") && strcmp(nvram_safe_get("switch_wan2tagid"), ""))
						stb_bitmask = stb_bitmask_shift(3);		// 3 = 0x11 IPTV: P0  VoIP: P1
					else if(strcmp(nvram_safe_get("switch_wan1tagid"), ""))
						stb_bitmask = stb_bitmask_shift(1);		// 1 = 0x01 IPTV: P0
					else if(strcmp(nvram_safe_get("switch_wan2tagid"), ""))
						stb_bitmask = stb_bitmask_shift(2);		// 2 = 0x10 VoIP: P1
					else
						stb_bitmask = stb_bitmask_shift(0);		//No IPTV and VoIP ports
					snprintf(stb_bitmask_str, sizeof(stb_bitmask_str), "0x%x", stb_bitmask);
					eval("rtkswitch", "38", stb_bitmask_str);

					/*++ Get and set Vlan Information */
					t = nvram_get_int("switch_wan0tagid") & 0x0fff;
					if (t != 0) {

						// Internet on WAN (port 4)
						if (t >= 2 && t <= 4094)
							vlan_val = t;

						prio_val = nvram_get_int("switch_wan0prio") & 0x7;

						vlan_bitmask = vlan_bitmask_shift(0x02000210);

						__setup_vlan(vlan_val, prio_val, vlan_bitmask);
					}
#if defined(RTCONFIG_RALINK_MT7620) || defined(RTCONFIG_RALINK_MT7621) || defined(RTCONFIG_RALINK_EN7561)
					else {
						/* Internet: untag: P4, P9; port: P4, P9 */
						vlan_bitmask = vlan_bitmask_shift(0x02100210);
						__setup_vlan(2, 0, vlan_bitmask);
					}
#endif
	
					t = nvram_get_int("switch_wan1tagid") & 0x0fff;
					if (t != 0) {
						// IPTV on LAN4 (port 0)
						if (t >= 2 && t <= 4094)
							vlan_val = t;

						prio_val = nvram_get_int("switch_wan1prio") & 0x7;

						if (t == nvram_get_int("switch_wan2tagid"))
							mask = 0x00030013;	//IPTV=VOIP
						else
							mask = 0x00010011;	//IPTV Port: P0 untag 65553 = 0x10 011
						vlan_bitmask = vlan_bitmask_shift(mask);
						__setup_vlan(vlan_val, prio_val, vlan_bitmask);
					}

					t = nvram_get_int("switch_wan2tagid") & 0x0fff;
					if (t != 0) {
						// VoIP on LAN3 (port 1)
						if (t >= 2 && t <= 4094)
							vlan_val = t;

						prio_val = nvram_get_int("switch_wan2prio") & 0x7;

						if (t == nvram_get_int("switch_wan1tagid"))
							mask = 0x00030013;	//IPTV=VOIP
						else
							mask = 0x00020012;	//VoIP Port: P1 untag

						vlan_bitmask = vlan_bitmask_shift(mask);
						__setup_vlan(vlan_val, prio_val, vlan_bitmask);
					}
				}
			}
		}
		else /* switch_wantag empty case */
		{
			const int sw_br_vid = get_sw_bridge_iptv_vid();

			if (stbport) {
				sprintf(parm_buf, "%d", stbport);
				eval("rtkswitch", "8", parm_buf);
			}
			if (sw_based_iptv()) {
				/* WAN:	no VLAN (hacked in API for SW based IPTV)
				 * STB:	according to switch_stb_x nvram variable.
				 */
				switch (stbport) {
				case 0:	/* none */
					break;
				case 1:	/* LAN1 */
					/* untag: P3, P4, P9;	port: P3, P4, P9 */
					__setup_vlan(sw_br_vid, 0, 0x02180218);
					break;
				case 2:	/* LAN2 */
					/* untag: P2, P4, P9;	port: P2, P4, P9 */
					__setup_vlan(sw_br_vid, 0, 0x02140214);
					break;
				case 3:	/* LAN3 */
					/* untag: P1, P4, P9;	port: P1, P4, P9 */
					__setup_vlan(sw_br_vid, 0, 0x02120212);
					break;
				case 4:	/* LAN4 */
					/* untag: P0, P4, P9;	port: P0, P4, P9 */
					__setup_vlan(sw_br_vid, 0, 0x02110211);
					break;
				case 5:	/* LAN1 & LAN2 */
					/* untag: P3, P2, P4, P9;	port: P3, P2, P4, P9 */
					__setup_vlan(sw_br_vid, 0, 0x021C021C);
					break;
				case 6:	/* LAN3 & LAN4 */
					/* untag: P1, P0, P4, P9;	port: P1, P0, P4, P9 */
					__setup_vlan(sw_br_vid, 0, 0x02130213);
					break;
				default:
					dbg("%s: unknown stb_stb_x %d\n", __func__, nvram_get_int("switch_stb_x"));
				}
			}
		}

		/* unknown unicast storm control */
		if (!nvram_get("switch_ctrlrate_unknown_unicast"))
			controlrate_unknown_unicast = 0;
		else
			controlrate_unknown_unicast = nvram_get_int("switch_ctrlrate_unknown_unicast");
		if (controlrate_unknown_unicast < 0 || controlrate_unknown_unicast > 1024)
			controlrate_unknown_unicast = 0;
		if (controlrate_unknown_unicast)
		{
			sprintf(parm_buf, "%d", controlrate_unknown_unicast);
			eval("rtkswitch", "22", parm_buf);
		}
	
		/* unknown multicast storm control */
		if (!nvram_get("switch_ctrlrate_unknown_multicast"))
			controlrate_unknown_multicast = 0;
		else
			controlrate_unknown_multicast = nvram_get_int("switch_ctrlrate_unknown_multicast");
		if (controlrate_unknown_multicast < 0 || controlrate_unknown_multicast > 1024)
			controlrate_unknown_multicast = 0;
		if (controlrate_unknown_multicast)
		{
			sprintf(parm_buf, "%d", controlrate_unknown_multicast);
			eval("rtkswitch", "23", parm_buf);
		}
	
		/* multicast storm control */
		if (!nvram_get("switch_ctrlrate_multicast"))
			controlrate_multicast = 0;
		else
			controlrate_multicast = nvram_get_int("switch_ctrlrate_multicast");
		if (controlrate_multicast < 0 || controlrate_multicast > 1024)
			controlrate_multicast = 0;
		if (controlrate_multicast)
		{
			sprintf(parm_buf, "%d", controlrate_multicast);
			eval("rtkswitch", "24", parm_buf);
		}
	
		/* broadcast storm control */
		if (!nvram_get("switch_ctrlrate_broadcast"))
			controlrate_broadcast = 0;
		else
			controlrate_broadcast = nvram_get_int("switch_ctrlrate_broadcast");
		if (controlrate_broadcast < 0 || controlrate_broadcast > 1024)
			controlrate_broadcast = 0;
		if (controlrate_broadcast)
		{
			sprintf(parm_buf, "%d", controlrate_broadcast);
			eval("rtkswitch", "25", parm_buf);
		}
	}
	else if (access_point_mode())
	{
		if (merge_wan_port_into_lan_ports)
			eval("rtkswitch", "8", "100");
	}
#if defined(RTCONFIG_WIRELESSREPEATER) && defined(RTCONFIG_PROXYSTA)
	else if (mediabridge_mode())
	{
		if (merge_wan_port_into_lan_ports)
			eval("rtkswitch", "8", "100");
	}
#endif

#ifdef RTCONFIG_DSL
	dbG("link up all ports\n");
	eval("rtkswitch", "16");	// link up all ports
#else
	dbG("link up wan port(s)\n");
	eval("rtkswitch", "114");	// link up wan port(s)
#endif

	post_config_switch();

#if defined(RTCONFIG_BLINK_LED)
	if (is_swports_bled("led_lan_gpio")) {
		update_swports_bled("led_lan_gpio", nvram_get_int("lanports_mask"));
	}
	if (is_swports_bled("led_wan_gpio")) {
		update_swports_bled("led_wan_gpio", nvram_get_int("wanports_mask"));
	}
#if defined(RTCONFIG_WANLEDX2)
	if (is_swports_bled("led_wan2_gpio")) {
		update_swports_bled("led_wan2_gpio", nvram_get_int("wan1ports_mask"));
	}
#endif
#endif
}
#endif //RTCONFIG_SWCONFIG

int
switch_exist(void)
{
	int ret;
#ifdef RTCONFIG_DSL
	// 0 means switch exist
	ret = 0;
#else
	ret = eval("rtkswitch", "41");
	_dprintf("eval(rtkswitch, 41) ret(%d)\n", ret);
#endif
	return (ret == 0);
}

void init_wl(void)
{
	unsigned char buffer[16];
	unsigned char *dst;
	char tmpStr1[16];
	char tmpStr2[24];
	char tmpStr3[24];
	char cmd[1024];
	int i;

	memset(tmpStr1, 0, sizeof(tmpStr1));
	memset(tmpStr2, 0, sizeof(tmpStr2));
	memset(tmpStr3, 0, sizeof(tmpStr3));
	memset(cmd, 0, sizeof(cmd));
	dst = buffer;
	memset(buffer, 0, sizeof(buffer));
	memset(dst, 0, MAX_REGSPEC_LEN+1);
	
	if(FRead(dst, REGSPEC_ADDR, MAX_REGSPEC_LEN) < 0)
	{
		_dprintf("READ REGSPEC_ADDR ERROR\n");
	}
	else
	{
		for(i = 0; i < MAX_REGSPEC_LEN && dst[i] != '\0'; i++) {
			if (dst[i] == 0xff)
			{
				dst[i] = '\0';
				break;
			}
		}
	}
	sprintf(tmpStr1, "regspec=%s", dst);
	
	memset(dst, 0, MAX_REGDOMAIN_LEN+1);
	if(FRead(dst, REG2G_EEPROM_ADDR, MAX_REGDOMAIN_LEN) < 0)
	{
		_dprintf("READ REG2G_EEPROM_ADDR ERROR\n");
	}
	else
	{
		for(i = 0; i < MAX_REGDOMAIN_LEN && dst[i] != '\0'; i++) {
			if (dst[i] == 0xff)
			{
				dst[i] = '\0';
				break;
			}
		}
	}
	sprintf(tmpStr2, "regspec_2g=%s", dst);

	memset(dst, 0, MAX_REGDOMAIN_LEN+1);
	if(FRead(dst, REG5G_EEPROM_ADDR, MAX_REGDOMAIN_LEN) < 0)
	{
		_dprintf("READ REG5G_EEPROM_ADDR ERROR\n");
	}
	else
	{
		for(i = 0; i < MAX_REGDOMAIN_LEN && dst[i] != '\0'; i++) {
			if (dst[i] == 0xff)
			{
				dst[i] = '\0';
				break;
			}
		}
	}
	sprintf(tmpStr3, "regspec_5g=%s", dst);

	if (!module_loaded("rt2860v2_ap"))
		modprobe("rt2860v2_ap");

#if defined (RTCONFIG_WLMODULE_MT7610_AP)
	if (!module_loaded("MT7610_ap"))
		modprobe("MT7610_ap");
#endif

#if defined (RTCONFIG_WLMODULE_MT7628_AP)
	if (!module_loaded("mt_wifi_7628"))
		modprobe("mt_wifi_7628");
#endif

#if defined (RTCONFIG_WLMODULE_RLT_WIFI)
	if (!module_loaded("rlt_wifi"))
	{   
		modprobe("rlt_wifi");
	}
#endif
#if defined(RTCONFIG_WLMODULE_MT7603E_AP) || defined(RTCONFIG_WLMODULE_MT7615E_AP)
	int mtd_part = 0, mtd_size = 0;
	if (mtd_getinfo("Factory", &mtd_part, &mtd_size)){
		snprintf(cmd, sizeof(cmd), "dd if=/dev/mtdblock%d of=/lib/firmware/e2p bs=131072 skip=0 count=1", mtd_part);
		system(cmd);
		system("ln -sf /rom/etc/wireless/mediatek /etc/wireless/");
	}else
		printf("init_devs: can't find Factory MTD partition\n");
#endif
#if defined (RTCONFIG_WLMODULE_MT7603E_AP)
	if (!module_loaded("rlt_wifi_7603e"))
		modprobe("rlt_wifi_7603e");
#endif

#if defined (RTCONFIG_WLMODULE_MT7615E_AP)
	if (!module_loaded("mt_wifi_7615E"))
		//modprobe("mt_wifi_7615E", tmpStr1, tmpStr2, tmpStr3);
		modprobe("mt_wifi_7615E");
#if !defined(RTCONFIG_RALINK_MT7621)
	if (!module_loaded("mt_whnat"))
		modprobe("mt_whnat");
#endif
#endif
#if defined(RTCONFIG_WLMODULE_MT7915D_AP)
	int mtd_part = 0, mtd_size = 0;
	if (mtd_getinfo("Factory", &mtd_part, &mtd_size)){
		snprintf(cmd, sizeof(cmd), "dd if=/dev/mtdblock%d of=/lib/firmware/e2p bs=131072 skip=0 count=1", mtd_part);
		system(cmd);
		system("ln -sf /rom/etc/wireless/mediatek /etc/wireless/");
	}else
		printf("init_devs: can't find Factory MTD partition\n");
	//if (!module_loaded("mt_wifi"))
		//modprobe("mt_wifi", tmpStr1, tmpStr2, tmpStr3);
	if (!module_loaded("mt_wifi"))
		modprobe("mt_wifi");
#if !defined(RTCONFIG_RALINK_MT7621)
	if (!module_loaded("mt_whnat"))
		modprobe("mt_whnat");
#endif
	snprintf(cmd, sizeof(cmd), "iwpriv %s set RuntimePara_%s\n", get_wifname(0), tmpStr1);
	system(cmd);
	snprintf(cmd, sizeof(cmd), "iwpriv %s set RuntimePara_%s\n", get_wifname(0), tmpStr2);
	system(cmd);
	snprintf(cmd, sizeof(cmd), "iwpriv %s set RuntimePara_%s\n", get_wifname(0), tmpStr3);
	system(cmd);
#endif
#if defined(RTCONFIG_MT798X)
	int mtd_part = 0, mtd_size = 0;
	if (mtd_getinfo("Factory", &mtd_part, &mtd_size)){
		snprintf(cmd, sizeof(cmd), "dd if=/dev/mtdblock%d of=/lib/firmware/e2p bs=720896 skip=0 count=1", mtd_part);
		system(cmd);
		system("ln -sf /rom/etc/wireless/mediatek /etc/wireless/");
		doSystem("cp -s /rom/firmware/* /lib/firmware/");
	}else
		printf("init_devs: can't find Factory MTD partition\n");
	if (!module_loaded("conninfra"))
		modprobe("conninfra");
	if (!module_loaded("mt_wifi"))
		modprobe("mt_wifi");
	if (!module_loaded("mtk_warp"))
		modprobe("mtk_warp");
	if (!module_loaded("mtk_warp_proxy"))
		modprobe("mtk_warp_proxy");
#endif
	sleep(1);
}


void wl_ifdown(void)
{
	char *wl_ifnames;
	char nv[32], vif[IFNAMSIZ];
	int unit;
	char word[8], *next = NULL;
	int vidx;

#if defined(RTCONFIG_AMAS) && defined(RTCONFIG_BLINK_LED)
	if (aimesh_re_node()) {
		for (unit = 0; unit < MAX_NR_WL_IF; ++unit) {
			remove_netdev_bled_if(get_wl_led_gpio_nv(unit), get_staifname(unit));
		}
	}
#endif

	unit = 0;
	wl_ifnames = strdup(nvram_safe_get("wl_ifnames"));
	if (wl_ifnames) {
		foreach (word, wl_ifnames, next)
		{
			ifconfig(word, 0, NULL, NULL);
#if defined(RTCONFIG_AMAS_WGN)
#if defined(RTCONFIG_RALINK) && defined(RALINK_DBDC_MODE)
			for (vidx = 1; vidx < MAX_SUBIF_NUM; vidx++) {
				snprintf(nv, sizeof(nv), "wl%d.%d_ifname", unit, vidx);
				snprintf(vif, sizeof(vif), "%s", nvram_safe_get(nv));
				if (strlen(vif))
					ifconfig(vif, 0, NULL, NULL);
			}
#endif
#endif	/* RTCONFIG_AMAS_WGN */
			unit++;
		}
		free(wl_ifnames);
	}

#if defined(RTCONFIG_RALINK) && defined(RTCONFIG_AMAS)
	for (unit = 0; unit < MAX_NR_WL_IF; unit++) {
		snprintf(nv, sizeof(nv), "wl%d_vifs", unit);
		snprintf(vif, sizeof(vif), "%s", nvram_safe_get(nv));
		if (strlen(vif))
			ifconfig(vif, 0, NULL, NULL);
	}
#endif

#if defined(RTCONFIG_RALINK)
	/* Turn off WDS interfaces. */
	for (vidx = 0; vidx < 8; ++vidx) {
		snprintf(vif, sizeof(vif), "wds%d", vidx);
		if (iface_exist(vif))
			ifconfig(vif, 0, NULL, NULL);
		snprintf(vif, sizeof(vif), "wdsx%d", vidx);
		if (iface_exist(vif))
			ifconfig(vif, 0, NULL, NULL);
	}
#endif
}

void fini_wl(void)
{
	wl_ifdown();	/* all wireless interface down before rmmod wifi modules */
#if defined(RTCONFIG_EASYMESH)
	if (module_loaded("mapfilter"))
		modprobe_r("mapfilter");
#endif
	if (module_loaded("mtkhnat")){
		unregister_hnat_wlifaces();
		modprobe_r("mtkhnat");
	}
#if defined (RTCONFIG_WLMODULE_MT7615E_AP)
	if (module_loaded("mt_wifi_7615E"))
		modprobe_r("mt_wifi_7615E");
#endif
#if defined(RTCONFIG_WLMODULE_MT7915D_AP)
	if (module_loaded("mt_wifi"))
		modprobe_r("mt_wifi");
#endif
#if defined(RTCONFIG_MT798X)
	if (module_loaded("mtk_warp_proxy"))
		modprobe_r("mtk_warp_proxy");
	if (module_loaded("mtk_warp"))
		modprobe_r("mtk_warp");
	if (module_loaded("mt_wifi"))
		modprobe_r("mt_wifi");
	if (module_loaded("conninfra"))
		modprobe_r("conninfra");
#endif
#if defined (RTCONFIG_WLMODULE_MT7610_AP)
	if (module_loaded("MT7610_ap"))
		modprobe_r("MT7610_ap");
#endif
#if defined (RTCONFIG_WLMODULE_MT7628_AP)
	if (module_loaded("mt_wifi_7628"))
		modprobe_r("mt_wifi_7628");
#endif
#if defined (RTCONFIG_WLMODULE_RLT_WIFI)
	if (module_loaded("rlt_wifi"))
	{   
		modprobe_r("rlt_wifi");
	}
#endif
#if defined (RTCONFIG_WLMODULE_MT7603E_AP)
	if (module_loaded("rlt_wifi_7603e"))
		modprobe_r("rlt_wifi_7603e");
#endif

	if (module_loaded("rt2860v2_ap"))
		modprobe_r("rt2860v2_ap");
}


#if ! defined(RTCONFIG_NEW_REGULATION_DOMAIN)
static void chk_valid_country_code(char *country_code)
{
	if ((unsigned char)country_code[0]!=0xff)
	{
		//for specific power
		if     (memcmp(country_code, "Z1", 2) == 0)
			strcpy(country_code, "US");
		else if(memcmp(country_code, "Z2", 2) == 0)
			strcpy(country_code, "GB");
		else if(memcmp(country_code, "Z3", 2) == 0)
			strcpy(country_code, "TW");
		else if(memcmp(country_code, "Z4", 2) == 0)
			strcpy(country_code, "CN");
		//for normal
		if(memcmp(country_code, "BR", 2) == 0)
			strcpy(country_code, "UZ");
	}
	else
	{
		strcpy(country_code, "DB");
	}
}
#endif

#ifdef RA_SINGLE_SKU
static void create_SingleSKU(const char *path, const char *pBand, const char *reg_spec, const char *pFollow)
{
	char src[128];
	char dest[128];

	sprintf(src , "/ra_SKU/SingleSKU%s_%s%s.dat", pBand, reg_spec, pFollow);
	sprintf(dest, "%s/SingleSKU%s.dat", path, pBand);

	eval("mkdir", "-p", (char*)path);
	unlink(dest);
	eval("ln", "-s", src, dest);
}

void gen_ra_sku(const char *reg_spec)
{
	create_SingleSKU("/etc/Wireless/RT2860", "", reg_spec, "");

#ifdef RTCONFIG_HAS_5G
	create_SingleSKU("/etc/Wireless/iNIC", "_5G", reg_spec, "");
#endif	/* RTCONFIG_HAS_5G */
}
#endif	/* RA_SINGLE_SKU */

#if defined(TUFAX4200) || defined(TUFAX6000) // EEPROM runtime fix
void eeprom_check(void);
#endif
void init_syspara(void)
{
	unsigned char buffer[16];
	unsigned char *dst, reg_spec[MAX_REGSPEC_LEN + 1] = { 0 }, reg_2g[MAX_REGDOMAIN_LEN + 1] = { 0 }, reg_5g[MAX_REGDOMAIN_LEN + 1] = { 0 };
	unsigned int bytes;
	int i;
	char macaddr[]="00:11:22:33:44:55";
	char macaddr2[]="00:11:22:33:44:58";
#if defined(RTCONFIG_EASYMESH)
	char macaddrbh1[]="00:11:22:33:44:55";
	char macaddrbh2[]="00:11:22:33:44:58";
#endif
	char country_code[3];
	char pin[9];
	char productid[13];
	char fwver[8];
	char blver[20];
	unsigned char txbf_para[33];
	char ea[ETHER_ADDR_LEN] __attribute__((unused));
#ifdef RTCONFIG_ODMPID
#ifdef RTCONFIG_32BYTES_ODMPID
        char modelname[32];
#else
	char modelname[16];
#endif
#endif
	const char *reg_spec_def;
	const int NEED_eu2cn = 0;

	char brstp;
	char value_str[MAX_REGSPEC_LEN+1];
	memset(value_str, 0, sizeof(value_str));

#if defined(RTCONFIG_ASUSCTRL)
	fix_location_code();
#endif

#if defined(TUFAX4200) || defined(TUFAX6000)
	config_hwctl_led();
#endif
#if defined(TUFAX4200) || defined(TUFAX6000) // EEPROM runtime fix
	if (nvram_get_int("no_e2pfix") == 0)
		eeprom_check();
#endif
	set_basic_fw_name();

	/* /dev/mtd/2, RF parameters, starts from 0x40000 */
	dst = buffer;
	bytes = 6;
	memset(buffer, 0, sizeof(buffer));
	memset(country_code, 0, sizeof(country_code));
	memset(pin, 0, sizeof(pin));
	memset(productid, 0, sizeof(productid));
	memset(fwver, 0, sizeof(fwver));
	memset(txbf_para, 0, sizeof(txbf_para));

	if (FRead(dst, OFFSET_MAC_ADDR, bytes)<0)
	{
		_dprintf("READ MAC address: Out of scope\n");
	}
	else
	{
		if (buffer[0]!=0xff)
			ether_etoa(buffer, macaddr);
	}

	if (FRead(dst, OFFSET_MAC_ADDR_2G, bytes)<0)
	{
		_dprintf("READ MAC address 2G: Out of scope\n");
	}
	else
	{
		if (buffer[0]!=0xff)
			ether_etoa(buffer, macaddr2);
#if defined(JCGQ10PRO) || defined(H3CTX1801) || defined(PGBM1) || defined(XMCR660X) || defined(JCGQ20)
		ether_cal_b(buffer, macaddr, 4);
#endif
	}

#if defined(RTCONFIG_WLMODULE_MT7615E_AP)
	brstp='0';
	FRead(&brstp, OFFSET_BR_STP, 1);
	if(brstp=='1')
	{
		_dprintf("Disable br0's STP\n");
		nvram_set("lan_stp","0");
	} 
#endif

	FRead(value_str, REGSPEC_ADDR, MAX_REGSPEC_LEN);
	for(i = 0; i < MAX_REGSPEC_LEN && value_str[i] != '\0'; i++) {
		if ((unsigned char)value_str[i] == 0xff)
		{
			value_str[i] = '\0';
			break;
		}
	}
	if(!strcmp(value_str,"JP"))
	   nvram_set("JP_CS","1");
	else
	   nvram_set("JP_CS","0");

//	if (!mssid_mac_validate(macaddr) || !mssid_mac_validate(macaddr2))
//		nvram_set("wl_mssid", "0");
//	else
		nvram_set("wl_mssid", "1");

#if defined(RTAC1200V2) || defined(RTACRH18) || defined(RT4GAC86U) || defined(RTAX53U) || defined(RT4GAX56) || defined(RTAX54) || defined(XD4S) || defined(RTCONFIG_MT798X)
	/* set et1macaddr the same as et0macaddr for spec. */
	strcpy(macaddr, macaddr2);
#endif
#if defined(RTAC1200) || defined(RTAC1200V2) || defined(RTAC53) || defined(RTACRH18) || defined(RT4GAC86U) || defined(RTAX53U) || defined(RT4GAX56) || defined(RTAX54) ||defined(XD4S)
	nvram_set("et0macaddr", macaddr2);
	nvram_set("et1macaddr", macaddr);
#else
	//TODO: separate for different chipset solution
	nvram_set("et0macaddr", macaddr);
	nvram_set("et1macaddr", macaddr2);
#endif
#if defined(RTCONFIG_EASYMESH)
	ether_cal(macaddr, macaddrbh1, 14);
	ether_cal(macaddr2, macaddrbh2, 18);
	nvram_set("bh0macaddr", macaddrbh1);
	nvram_set("bh1macaddr", macaddrbh2);
#endif

#if !defined(RTCONFIG_MT798X)
	if (FRead(dst, OFFSET_MAC_GMAC0, bytes)<0)
		dbg("READ MAC address GMAC0: Out of scope\n");
	else
	{
		if (buffer[0]==0xff)
		{
			if (ether_atoe(macaddr, ea))
				FWrite(ea, OFFSET_MAC_GMAC0, 6);
		}
	}
	if (FRead(dst, OFFSET_MAC_GMAC2, bytes)<0)
		dbg("READ MAC address GMAC2: Out of scope\n");
	else
	{
		if (buffer[0]==0xff)
		{
			if (ether_atoe(macaddr2, ea))
				FWrite(ea, OFFSET_MAC_GMAC2, 6);
		}
	}
#endif

#ifdef RTCONFIG_ODMPID
#ifdef RTCONFIG_32BYTES_ODMPID
        FRead(modelname, OFFSET_32BYTES_ODMPID, 32);
        modelname[31] = '\0';
	if (modelname[0] != 0 && (unsigned char)(modelname[0]) != 0xff && is_valid_hostname(modelname) && strcmp(modelname, "ASUS")) {
                nvram_set("odmpid", modelname);
        } else
#endif
        {

		FRead(modelname, OFFSET_ODMPID, sizeof(modelname));
		modelname[sizeof(modelname)-1] = '\0';
		if (modelname[0] != 0 && (unsigned char)(modelname[0]) != 0xff && is_valid_hostname(modelname) && strcmp(modelname, "ASUS")) {
				nvram_set("odmpid", modelname);
		
                } else
                        nvram_unset("odmpid");
        }
#else
        nvram_unset("odmpid");
#endif

	if (FRead(reg_spec, REGSPEC_ADDR, MAX_REGSPEC_LEN) < 0)
		*reg_spec = '\0';
	if (FRead(reg_2g, REG2G_EEPROM_ADDR, MAX_REGDOMAIN_LEN) < 0)
		*reg_2g = '\0';
	if (FRead(reg_5g, REG5G_EEPROM_ADDR, MAX_REGDOMAIN_LEN) < 0)
		*reg_5g = '\0';
	trim_char(reg_spec, 0xFF);
	trim_char(reg_2g, 0xFF);
	trim_char(reg_5g, 0xFF);

#if defined(RTCONFIG_TCODE)
	/* Territory code */
	memset(buffer, 0, sizeof(buffer));
	if(NEED_eu2cn) {
		nvram_set("territory_code", "CN/01");	/* RT-AC51U: FIX EU2CN */
	} else {
		if (FRead(buffer, OFFSET_TERRITORY_CODE, 5) < 0) {
			_dprintf("READ ASUS territory code: Out of scope\n");
			nvram_unset("territory_code");
		} else {
			/* [A-Z][A-Z]/[0-9][0-9] */
			if (buffer[2] != '/' ||
			    !isupper(buffer[0]) || !isupper(buffer[1]) ||
			    !isdigit(buffer[3]) || !isdigit(buffer[4]))
			{
				nvram_unset("territory_code");
			} else {
				nvram_set("territory_code", buffer);
			}
		}
	}

#if defined(RTCONFIG_ASUSCTRL)
	nvram_unset("ctl_reg_spec");
	nvram_unset("ctl_wl_reg_2g");
	nvram_unset("ctl_wl_reg_5g");

	asus_ctrl_sku_check();

	if (*nvram_safe_get("ctl_reg_spec") != '\0')
		strlcpy(reg_spec, nvram_safe_get("ctl_reg_spec"), sizeof(reg_spec));
	if (*nvram_safe_get("ctl_wl_reg_2g") != '\0')
		strlcpy(reg_2g, nvram_safe_get("ctl_wl_reg_2g"), sizeof(reg_2g));
	if (*nvram_safe_get("ctl_wl_reg_5g") != '\0')
		strlcpy(reg_5g, nvram_safe_get("ctl_wl_reg_5g"), sizeof(reg_5g));
#endif
#endif	/* RTCONFIG_TCODE */

	/* reserved for Ralink. used as ASUS country code. */
#if !defined(RTCONFIG_NEW_REGULATION_DOMAIN)
	dst = (unsigned char*) country_code;
	bytes = 2;
	if (FRead(dst, OFFSET_COUNTRY_CODE, bytes)<0)
	{
		_dprintf("READ ASUS country code: Out of scope\n");
		nvram_set("wl_country_code", "");
	}
	else
	{
		chk_valid_country_code(country_code);
		nvram_set("wl_country_code", country_code);
		nvram_set("wl0_country_code", country_code);
#ifdef RTCONFIG_HAS_5G
		nvram_set("wl1_country_code", country_code);
#endif
	}
#else	/* ! RTCONFIG_NEW_REGULATION_DOMAIN */
	dst = buffer;

#if defined(RTCONFIG_MT798X)
	reg_spec_def = "CE";
#else
	reg_spec_def = "FCC";
#endif

	if (NEED_eu2cn) {
		nvram_set("reg_spec", "CN");	/* RT-AC51U: FIX EU2CN */
	} else {
		nvram_set("reg_spec", (*reg_spec)? (char*) reg_spec : reg_spec_def);
	}

	if (NEED_eu2cn) {
		nvram_set("wl0_country_code", "CN");	/* RT-AC51U: FIX EU2CN */
	} else {
		if (*reg_2g == '\0' || memcmp(reg_2g, "2G_CH", 5) != 0) {
			_dprintf("Read REG2G_EEPROM_ADDR fail or invalid value\n");
			nvram_set("wl_country_code", "");
			nvram_set("wl0_country_code", "DB");
			nvram_set("wl_reg_2g", "2G_CH14");
		} else {
			nvram_set("wl_reg_2g", reg_2g);
			if (strcmp(reg_2g, "2G_CH11") == 0)
				nvram_set("wl0_country_code", "US");
			else if (strcmp(reg_2g, "2G_CH13") == 0) {
					nvram_set("wl0_country_code", "GB");
			}
			else if (strcmp(reg_2g, "2G_CH13") == 0) {
				nvram_set("wl0_country_code", "GB");
			}
			else if (strcmp(reg_2g, "2G_CH14") == 0)
				nvram_set("wl0_country_code", "DB");
			else
				nvram_set("wl0_country_code", "DB");
		}
	}

#ifdef RTCONFIG_HAS_5G
	if (NEED_eu2cn) {
		nvram_set("wl1_country_code", "US");	/* RT-AC51U: FIX EU2CN */
	} else {
		if (*reg_5g == '\0' || memcmp(reg_5g, "5G_", 3) != 0) {
			_dprintf("Read REG5G_EEPROM_ADDR fail or invalid value\n");
			nvram_set("wl_country_code", "");
			nvram_set("wl1_country_code", "DB");
			nvram_set("wl_reg_5g", "5G_ALL");
		} else {
			nvram_set("wl_reg_5g", reg_5g);
			nvram_set("wl1_IEEE80211H", "0");
			if (strcmp(reg_5g, "5G_BAND1") == 0)
				nvram_set("wl1_country_code", "GB");
#if defined(RTCONFIG_MT798X)
			else if (strcmp(reg_5g, "5G_ALL") == 0) {
				if(strncmp(nvram_safe_get("territory_code"), "TW/01", 2) == 0) {
					nvram_set("wl1_IEEE80211H", "1");
					nvram_set("wl1_country_code", "TW");
				}
				else if(strncmp(nvram_safe_get("territory_code"), "US/01", 2) == 0) {
					nvram_set("wl1_IEEE80211H", "1");
					nvram_set("wl1_country_code", "US");
				}
				else if(strncmp(nvram_safe_get("territory_code"), "AA/01", 2) == 0) {
					nvram_set("wl1_IEEE80211H", "1");
					nvram_set("wl1_country_code", "AA");
				}
				else {
					nvram_set("wl1_country_code", "DB");
				}
			}
			else if (strcmp(reg_5g, "5G_BAND123") == 0) {
				if(strncmp(nvram_safe_get("territory_code"), "JP/01", 2) == 0) {
					nvram_set("wl1_IEEE80211H", "1");
					nvram_set("wl1_country_code", "JP");
				}
				else {
					nvram_set("wl1_IEEE80211H", "1");
					nvram_set("wl1_country_code", "EU");
				}
			}
			else if (strcmp(reg_5g, "5G_BAND124") == 0) {
					nvram_set("wl1_IEEE80211H", "1");
					nvram_set("wl1_country_code", "CN");
			}
#endif
			else if (strcmp(reg_5g, "5G_BAND123") == 0) {
				nvram_set("wl1_country_code", "GB");
#ifdef RTCONFIG_RALINK_DFS
				nvram_set("wl1_IEEE80211H", "1");
#endif	/* RTCONFIG_RALINK_DFS */
			}
			else if (strcmp(reg_5g, "5G_BAND14") == 0)
				nvram_set("wl1_country_code", "US");
			else if (strcmp(reg_5g, "5G_BAND24") == 0)
				nvram_set("wl1_country_code", "TW");
			else if (strcmp(reg_5g, "5G_BAND4") == 0)
				nvram_set("wl1_country_code", "CN");
			else if (strcmp(reg_5g, "5G_BAND124") == 0)
				nvram_set("wl1_country_code", "IN");
			else if (strcmp(reg_5g, "5G_BAND12") == 0)	{
				nvram_set("wl1_country_code", "IL");
#ifdef RTCONFIG_RALINK_DFS
				nvram_set("wl1_IEEE80211H", "1");
#endif	/* RTCONFIG_RALINK_DFS */
			}
			else if (strcmp(reg_5g, "5G_ALL") == 0)	{
#if defined(RTAC85P)
				if(nvram_match("reg_spec","EAC"))
					nvram_set("wl1_country_code", "RU");
				else
#elif defined(RT4GAX56)
					nvram_set("wl1_country_code", "AA");
#else
					nvram_set("wl1_country_code", "DB");
#endif
#ifdef RTCONFIG_RALINK_DFS
				nvram_set("wl1_IEEE80211H", "1");
#endif	/* RTCONFIG_RALINK_DFS */
			}
			else
				nvram_set("wl1_country_code", "DB");
		}
	}
#endif	/* RTCONFIG_HAS_5G */
#endif	/* ! RTCONFIG_NEW_REGULATION_DOMAIN */
#if defined(RTCONFIG_DSL)
	if (nvram_match("wl_country_code", "BR")) {
		nvram_set("wl_country_code", "UZ");
		nvram_set("wl0_country_code", "UZ");
#ifdef RTCONFIG_HAS_5G
		nvram_set("wl1_country_code", "UZ");
#endif	/* RTCONFIG_HAS_5G */
	}
#endif
	if (nvram_match("wl_country_code", "HK") && nvram_match("preferred_lang", ""))
		nvram_set("preferred_lang", "TW");

	/* reserved for Ralink. used as ASUS pin code. */
	dst = (char*)pin;
	bytes = 8;
	if (FRead(dst, OFFSET_PIN_CODE, bytes)<0)
	{
		_dprintf("READ ASUS pin code: Out of scope\n");
		nvram_set("wl_pin_code", "");
	}
	else
	{
		if (((unsigned char)pin[0] == 0xff)
		 || !strcmp(pin, "12345678")) {
			char devPwd[9];
			nvram_set("secret_code", wps_gen_pin(devPwd, sizeof(devPwd)) ? devPwd : "12345670");
		}
		else
			nvram_set("secret_code", pin);
	}
#if defined(RTCONFIG_MT798X)
	strlcpy(productid, rt_buildname, sizeof(productid));
#else
	dst = buffer;
	bytes = 16;
	if (linuxRead(dst, 0x20, bytes)<0)	/* The "linux" MTD partition, offset 0x20. */
	{
		fprintf(stderr, "READ firmware header: Out of scope\n");
		nvram_set("productid", "unknown");
		nvram_set("firmver", "unknown");
	}
	else
	{
		strncpy(productid, buffer + 4, 12);
		productid[12] = 0;
		sprintf(fwver, "%d.%d.%d.%d", buffer[0], buffer[1], buffer[2], buffer[3]);
		nvram_set("productid", trim_r(productid));
		nvram_set("firmver", trim_r(fwver));
	}
#endif
#if defined(RTCONFIG_TCODE)
	
	/* PSK */
        memset(buffer, 0, sizeof(buffer));
	if (FRead(buffer, OFFSET_PSK, 14) < 0) {
	_dprintf("READ ASUS PSK: Out of scope\n");
		nvram_set("wifi_psk", "");
	 } else {
		if (buffer[0] == 0xff)
			nvram_set("wifi_psk", "");
		else
		{
			for(i = 0; i < 14 && buffer[i] != '\0'; i++) {
				if ((unsigned char)buffer[i] == 0xff)
				{
					buffer[i] = '\0';
					break;
				}
			}
			nvram_set("wifi_psk", buffer);
		}
	}
#endif /* RTCONFIG_TCODE */

	memset(buffer, 0, sizeof(buffer));
	FRead(buffer, OFFSET_BOOT_VER, 4);
//	sprintf(blver, "%c.%c.%c.%c", buffer[0], buffer[1], buffer[2], buffer[3]);
	sprintf(blver, "%s-0%c-0%c-0%c-0%c", trim_r(productid), buffer[0], buffer[1], buffer[2], buffer[3]);
	nvram_set("blver", trim_r(blver));

	_dprintf("mtd productid: %s\n", nvram_safe_get("productid"));
	_dprintf("bootloader version: %s\n", nvram_safe_get("blver"));
	_dprintf("firmware version: %s\n", nvram_safe_get("firmver"));

#if !defined (RTCONFIG_WLMODULE_MT7615E_AP)
	dst = txbf_para;
	int count_0xff = 0;
	if (FRead(dst, OFFSET_TXBF_PARA, 33) < 0)
	{
		fprintf(stderr, "READ TXBF PARA address: Out of scope\n");
	}
	else
	{
		for (i = 0; i < 33; i++)
		{
			if (txbf_para[i] == 0xff)
				count_0xff++;
/*
			if ((i % 16) == 0) fprintf(stderr, "\n");
			fprintf(stderr, "%02x ", (unsigned char) txbf_para[i]);
*/
		}
/*
		fprintf(stderr, "\n");

		fprintf(stderr, "TxBF parameter 0xFF count: %d\n", count_0xff);
*/
	}

	if (count_0xff == 33)
		nvram_set("wl1_txbf_en", "0");
	else
		nvram_set("wl1_txbf_en", "1");
#endif

	{
		char ipaddr_lan[16];
		FRead(ipaddr_lan, OFFSET_IPADDR_LAN, sizeof(ipaddr_lan));
		ipaddr_lan[sizeof(ipaddr_lan)-1] = '\0';
		if((unsigned char)(ipaddr_lan[0]) != 0xff)
		{
			nvram_set("IpAddr_Lan", ipaddr_lan);
		} else {
			nvram_unset("IpAddr_Lan");
		}
	}


#if defined(RTCONFIG_COBRAND)
        unsigned char color=0xff;
        if (FRead(&color, OFFSET_HW_COBRAND, 1)<0)
        {
                _dprintf("Read COBRAND value fail\n");
                nvram_set("CoBrand", "");
        }
        else
        {
                if(color!=0xff)
                        nvram_set_int("CoBrand",color);
                else
                        nvram_unset("CoBrand");
        }
#endif

	nvram_set("firmver", rt_version);
	nvram_set("productid", rt_buildname);

	_dprintf("odmpid: %s\n", nvram_safe_get("odmpid"));
	_dprintf("current FW productid: %s\n", nvram_safe_get("productid"));
	_dprintf("current FW firmver: %s\n", nvram_safe_get("firmver"));

	getSN();
}

#ifdef RTCONFIG_ATEUSB3_FORCE
void post_syspara(void)
{
	unsigned char buffer[16];
	buffer[0]='0';
	if (FRead(&buffer[0], OFFSET_FORCE_USB3, 1) < 0) {
		fprintf(stderr, "READ FORCE_USB3 address: Out of scope\n");
	}
	if (buffer[0]=='1')
		nvram_set("usb_usb3", "1");
}
#endif

void generate_wl_para(int unit, int subunit)
{
}

//#if defined(RTAC85U) || defined(RTAC85P) || defined(RTACRH26) || defined(RMAC2100) || defined(R6800)
#if 0
#define HW_NAT_WIFI_OFFLOADING		(0xFF00)
#define HW_NAT_DEVNAME			"hwnat0"
static void adjust_hwnat_wifi_offloading(void)
{
	int enable_hwnat_wifi = 1, fd;

	if (!nvram_match("switch_wantag", "none") && !nvram_match("switch_wantag", "")) {
		nvram_unset("isp_profile_hwnat_not_safe");
		eval("rtkswitch", "50");
		if (nvram_get_int("isp_profile_hwnat_not_safe") == 1)
			enable_hwnat_wifi = 0;
	}
	
#if defined (RTCONFIG_WLMODULE_MT7615E_AP) && !defined(RTCONFIG_MTK_8021XD3000)
		if (get_ipv6_service() == IPV6_PASSTHROUGH)
				enable_hwnat_wifi = 0;
		doSystem("iwpriv ra0 set wifi_hwnat=%d", enable_hwnat_wifi);	
#else

	if ((fd = open("/dev/" HW_NAT_DEVNAME, O_RDONLY)) < 0) {
		_dprintf("Open /dev/%s fail. errno %d (%s)\n", HW_NAT_DEVNAME, errno, strerror(errno));
		return;
	}

	_dprintf("hwnat_wifi = %d\n", enable_hwnat_wifi);
	if (ioctl(fd, HW_NAT_WIFI_OFFLOADING, &enable_hwnat_wifi) < 0)
		_dprintf("ioctl error. errno %d (%s)\n", errno, strerror(errno));

	close(fd);
#endif	
}
#else
static inline void adjust_hwnat_wifi_offloading(void) { }
#endif

// only ralink solution can reload it dynamically
// only happened when hwnat=1
// only loaded when unloaded, and unloaded when loaded
// in restart_firewall for fw_pt_l2tp/fw_pt_ipsec
// in restart_qos for qos_enable
// in restart_wireless for wlx_mrate_x, etc
void reinit_hwnat(int unit)
{
	int prim_unit = wan_primary_ifunit();
	int act = 1;	/* -1/0/otherwise: ignore/remove hwnat/load hwnat */
	int nat_x = -1;
	char tmp[32], prefix[] = "wanXXX_";
#if defined(RTCONFIG_DUALWAN) && !defined(RTCONFIG_MT798X)
	int i, l, t, link_wan = 1, link_wans_lan = 1;
	int wans_cap = get_wans_dualwan() & WANSCAP_WAN;
	int wanslan_cap = get_wans_dualwan() & WANSCAP_LAN;
#endif

	if (!nvram_get_int("hwnat"))
	{
		disable_hwnat();
		return;
	}

	if(!is_wan_connect(prim_unit))
		return;

	/* If QoS is enabled, disable hwnat. */
	if (nvram_get_int("qos_enable") == 1)
		act = 0;
#if defined(RTCONFIG_MT798X) && defined(RTCONFIG_BWDPI)
	/* MTK798X pure software for DPI */
	else if (check_bwdpi_nvram_setting() == 1)
		act = 0;
#endif

#if defined(RTCONFIG_MT798X)
	if (act > 0) {
		snprintf(prefix, sizeof(prefix), "wan%d_", prim_unit);
		nat_x = nvram_get_int(strcat_r(prefix, "nat_x", tmp));

#if defined(RTCONFIG_DUALWAN)
		/* If secorand WAN up, ignore... */
		if (unit != -1 && unit != prim_unit)
			act = -1;
		/* Load Balance */
		else if (nvram_match("wans_mode", "lb") && (nvram_match("wan0_proto", "pptp")
							 || nvram_match("wan0_proto", "l2tp")
							 || nvram_match("wan1_proto", "pptp")
							 || nvram_match("wan1_proto", "l2tp")))
			act = 0;
		else
#endif
		/* Enable NAT */
		if (!nat_x)
			act = 0;
		/* Single WAN/Dual WAN Fail Over */
		else if (nvram_match(strcat_r(prefix, "proto", tmp), "pptp")
		      || nvram_match(strcat_r(prefix, "proto", tmp), "l2tp"))
			act = 0;
		/* If VPN server(PPTP) is enabled, disable hwnat. */
		else if (nvram_get_int("pptpd_enable"))
			act = 0;
		/* If VPN client(PPTP/L2TP) activated, disable hwnat. */
		else if (!nvram_match("vpnc_clientlist", "")) {
			char *nv = NULL, *nvp = NULL, *b = NULL;
			char *desc, *proto, *server, *username, *passwd, *active;

			nv = nvp = strdup(nvram_safe_get("vpnc_clientlist"));
			while (nv && (b = strsep(&nvp, "<")) != NULL) {
				if (vstrsep(b, ">", &desc, &proto, &server, &username, &passwd, &active) < 6)
					continue;
				if (atoi(active) == 1 && (!strcmp(proto, "PPTP")
						       || !strcmp(proto, "L2TP"))) {
					act = 0;
					break;
				}
			}
			free(nv);
		}
	}
#else /* RTCONFIG_MT798X */

	if (act > 0) {
#if defined(RTCONFIG_DUALWAN)
		if (unit < 0 || unit > WAN_UNIT_SECOND) {
			if ((wans_cap && wanslan_cap) ||
			    (wanslan_cap && (!nvram_match("switch_wantag", "none") && !nvram_match("switch_wantag", "")))
			   )
				act = 0;
		} else {
			snprintf(prefix, sizeof(prefix), "wan%d_", unit);
			nat_x = nvram_get_int(strcat_r(prefix, "nat_x", tmp));

			if (unit == prim_unit && !nat_x)
				act = 0;
			else if ((wans_cap && wanslan_cap) ||
				 (wanslan_cap && (!nvram_match("switch_wantag", "none") && !nvram_match("switch_wantag", "")))
				)
				act = 0;
			else if (unit != prim_unit)
				act = -1;
		}
#else
		nat_x = is_nat_enabled();
		if (!nat_x)
			act = 0;
#endif
	}

	if (act > 0) {
#if defined(RTCONFIG_DUALWAN)
		if (unit < 0 || unit > WAN_UNIT_SECOND || nvram_match("wans_mode", "lb")) {
			if (get_wans_dualwan() & WANSCAP_USB)
				act = 0;
		} else {
			if (unit == prim_unit && get_dualwan_by_unit(unit) == WANS_DUALWAN_IF_USB)
				act = 0;
		}
#else
		if (dualwan_unit__usbif(prim_unit))
			act = 0;
#endif
	}
#endif /* RTCONFIG_MT798X */

#if defined(RTCONFIG_DUALWAN) && !defined(RTCONFIG_RALINK_MT7622) && !defined(RTCONFIG_MT798X)
	if (act != 0 &&
	    ((wans_cap && wanslan_cap) || (wanslan_cap && (!nvram_match("switch_wantag", "none") && !nvram_match("switch_wantag", ""))))
	   )
	{
		/* If WANS_LAN and WAN is enabled, WANS_LAN is link-up and WAN is not link-up, hw_nat MUST be removed.
		 * If hw_nat exists in such scenario, LAN PC can't connect to Internet through WANS_LAN correctly.
		 *
		 * FIXME:
		 * If generic IPTV feature is enabled, STB port and VoIP port are recognized as WAN port(s).
		 * In such case, we don't know whether real WAN port is link-up/down.
		 * Thus, if WAN is link-up and primary unit is not WAN, assume WAN is link-down.
		 */
		for (i = WAN_UNIT_FIRST; i < WAN_UNIT_MAX; ++i) {
			if ((t = get_dualwan_by_unit(i)) == WANS_DUALWAN_IF_USB)
				continue;

			l = wanport_status(i);
			switch (t) {
			case WANS_DUALWAN_IF_WAN:
				link_wan = l && (i == prim_unit);
				break;
			case WANS_DUALWAN_IF_DSL:
				link_wan = l;
				break;
			case WANS_DUALWAN_IF_LAN:
				link_wans_lan = l;
				break;
			default:
				_dprintf("%s: Unknown WAN type %d\n", __func__, t);
			}
		}

		if (!link_wan && link_wans_lan)
			act = 0;
	}

	_dprintf("%s:DUALWAN: unit %d,%d type %d iptv [%s] nat_x %d qos %d wans_mode %s link %d,%d: action %d.\n",
		__func__, unit, prim_unit, get_dualwan_by_unit(unit), nvram_safe_get("switch_wantag"), nat_x,
		nvram_get_int("qos_enable"), nvram_safe_get("wans_mode"),
		link_wan, link_wans_lan, act);
#else
	_dprintf("%s:WAN: unit %d,%d type %d nat_x %d qos %d: action %d.\n",
		__func__, unit, prim_unit, get_dualwan_by_unit(unit),
		nat_x, nvram_get_int("qos_enable"), act);
#endif

	if (act < 0)
		return;
#if 0 //3.10.x
	switch (act) {
	case 0:		/* remove hwnat */

		if (module_loaded("hw_nat")) {
#if defined (RTCONFIG_WLMODULE_MT7615E_AP)
			doSystem("iwpriv %s set hw_nat_register=%d", get_wifname(0), 0);
#ifdef RTCONFIG_HAS_5G
			doSystem("iwpriv %s set hw_nat_register=%d", get_wifname(1), 0);
#endif
#endif
			modprobe_r("hw_nat");
			sleep(1);
		}
		break;
	default:	/* load hwnat */
		if (!module_loaded("hw_nat")) {
			modprobe("hw_nat");
			sleep(1);
		}
#if defined (RTCONFIG_WLMODULE_MT7615E_AP)
		doSystem("iwpriv %s set hw_nat_register=%d", get_wifname(0), 1);
#ifdef RTCONFIG_HAS_5G
		doSystem("iwpriv %s set hw_nat_register=%d", get_wifname(1), 1);
#endif
#endif
#if defined (RTCONFIG_WLMODULE_MT7615E_AP)
		doSystem("iwpriv %s set LanNatSpeedUpEn=%d", get_wifname(0), 1);
#ifdef RTCONFIG_HAS_5G
		doSystem("iwpriv %s set LanNatSpeedUpEn=%d", get_wifname(1), 1);
#endif
#endif		
		adjust_hwnat_wifi_offloading();
	}
#else //4.4.x
	if (act){
		if (!module_loaded("mtkhnat")) {
			modprobe("mtkhnat");
		}		
	} else {
		if (module_loaded("mtkhnat")) {
			modprobe_r("mtkhnat");
			sleep(1);
		}
	}
#endif
}

int wl_exist(char *ifname, int band)
{
	int ret = 0;
	ret = eval("iwpriv", ifname, "stat");
	_dprintf("eval(iwpriv, %s, stat) ret(%d)\n", ifname, ret);
	return !ret;
}

#ifdef RTCONFIG_SWCONFIG
void set_wan_tag(char *interface)
{
	int model, wan_vid, iptv_vid, voip_vid, switch_stb;
	char wan_dev[10], port_id[7], vid_dev[10];
	
	model = get_model();
	wan_vid = nvram_get_int("switch_wan0tagid");
	iptv_vid = nvram_get_int("switch_wan1tagid");
	voip_vid = nvram_get_int("switch_wan2tagid");

	switch_stb = nvram_get_int("switch_stb_x");

	eval("vconfig", "set_name_type", "VLAN_PLUS_VID_NO_PAD");
#ifdef RTCONFIG_MULTICAST_IPTV
		if (switch_stb >= 7) {
			if (iptv_vid) { /* config IPTV on wan port */
_dprintf("*** Multicast IPTV: config IPTV on wan port ***\n");
				/* Handle wan(IPTV) vlan traffic */
				sprintf(port_id, "%d", iptv_vid);
				eval("vconfig", "add", interface, port_id);
				sprintf(vid_dev, "vlan%d", iptv_vid);
				eval("ifconfig", vid_dev, "up");
				nvram_set("wan10_ifname", vid_dev);
			}
		}
		if (switch_stb >= 8) {
			if (voip_vid) { /* config voip on wan port */
_dprintf("*** Multicast IPTV: config VOIP on wan port ***\n");
				/* Handle wan(VOIP) vlan traffic */
				sprintf(port_id, "%d", voip_vid);
				eval("vconfig", "add", interface, port_id);
				sprintf(vid_dev, "vlan%d", voip_vid);
				eval("ifconfig", vid_dev, "up");
				nvram_set("wan11_ifname", vid_dev);
			}
		}
#endif

}
#else	/* !RTCONFIG_SWCONFIG */
void set_wan_tag(char *interface) {
	int model, wan_vid; //, iptv_vid, voip_vid, wan_prio, iptv_prio, voip_prio;
	char wan_dev[10], port_id[7];

	model = get_model();
	wan_vid = nvram_get_int("switch_wan0tagid") & 0x0fff;

	snprintf(wan_dev, sizeof(wan_dev), "vlan%d", wan_vid);

	switch(model) {
	case MODEL_RTAC85U:
	case MODEL_RTAC85P:
	case MODEL_RMAC2100:
	case MODEL_R6800:
	case MODEL_TUFAC1750:
	case MODEL_RTAX53U:
	case MODEL_RTAX54:
	case MODEL_PGBM1:
	case MODEL_JCGQ10PRO:
	case MODEL_H3CTX1801:
	case MODEL_XMCR660X:
	case MODEL_JCGQ20:
		ifconfig(interface, IFUP, 0, 0);
		if(wan_vid) { /* config wan port */
			eval("vconfig", "rem", "vlan2");
			sprintf(port_id, "%d", wan_vid);
			eval("vconfig", "add", interface, port_id);

			/* Set Wan port PRIO */
			if (nvram_get_int("switch_wan0prio") != 0)
				eval("vconfig", "set_egress_map", wan_dev, "0", nvram_get("switch_wan0prio"));
		}
		break;
	}

#ifdef RTCONFIG_MULTICAST_IPTV
	{
		int iptv_vid, voip_vid, iptv_prio, voip_prio, switch_stb;
		int mang_vid, mang_prio;
		char iptv_prio_str[4] = "4";
		char voip_prio_str[4], mang_prio_str[4];

		iptv_vid  = nvram_get_int("switch_wan1tagid") & 0x0fff;
		voip_vid  = nvram_get_int("switch_wan2tagid") & 0x0fff;
		iptv_prio = nvram_get_int("switch_wan1prio") & 0x7;
		voip_prio = nvram_get_int("switch_wan2prio") & 0x7;
		mang_vid  = nvram_get_int("switch_wan3tagid") & 0x0fff;
		mang_prio = nvram_get_int("switch_wan3prio") & 0x7;

		switch_stb = nvram_get_int("switch_stb_x");
		if (switch_stb >= 7) {
			system("rtkswitch 40 1");			/* admin all frames on all ports */
			if(wan_vid) { /* config wan port */
				__setup_vlan(wan_vid, 0, 0x00000210);	/* config WAN & WAN_MAC port */
			}

			if (iptv_vid) { /* config IPTV on wan port */
				snprintf(wan_dev, sizeof(wan_dev), "vlan%d", iptv_vid);
				nvram_set("wan10_ifname", wan_dev);
				sprintf(port_id, "%d", iptv_vid);
				eval("vconfig", "add", interface, port_id);

				__setup_vlan(iptv_vid, iptv_prio, 0x00000210);	/* config WAN & WAN_MAC port */

				if (iptv_prio) { /* config priority */
					snprintf(iptv_prio_str, sizeof(iptv_prio_str), "%d", iptv_prio);
					eval("vconfig", "set_egress_map", wan_dev, "0", iptv_prio_str);
				}
			}
		}
		if (switch_stb >= 8) {
			if (voip_vid) { /* config voip on wan port */
				snprintf(wan_dev, sizeof(wan_dev), "vlan%d", voip_vid);
				nvram_set("wan11_ifname", wan_dev);
				sprintf(port_id, "%d", voip_vid);
				eval("vconfig", "add", interface, port_id);

				__setup_vlan(voip_vid, voip_prio, 0x00000210);	/* config WAN & WAN_MAC port */

				if (voip_prio) { /* config priority */
					snprintf(voip_prio_str, sizeof(voip_prio_str), "%d", voip_prio);
					eval("vconfig", "set_egress_map", wan_dev, "0", voip_prio_str);
				}
			}
		}
		if (switch_stb >=9 ) {
			if (mang_vid) { /* config tr069 on wan port */
				snprintf(wan_dev, sizeof(wan_dev), "vlan%d", mang_vid);
				nvram_set("wan12_ifname", wan_dev);
				sprintf(port_id, "%d", mang_vid);
				eval("vconfig", "add", interface, port_id);

				__setup_vlan(mang_vid, mang_prio, 0x00000210);	/* config WAN & WAN_MAC port */

				if (mang_prio) { /* config priority */
					snprintf(mang_prio_str, sizeof(mang_prio_str), "%d", mang_prio);
					eval("vconfig", "set_egress_map", wan_dev, "0", mang_prio_str);
				}
			}
		}
	}
#endif
}
#endif //RTCONFIG_SWCONFIG

#ifdef RA_SINGLE_SKU
void reset_ra_sku(const char *location, const char *country, const char *reg_spec)
{
	const char *try_list[] = { reg_spec, location, country, "CE", "FCC"};
	int i;
	for (i = 0; i < ARRAY_SIZE(try_list); i++) {
		if(try_list[i] != NULL && setRegSpec(try_list[i], 0) == 0)
			break;
	}

	if(i >= ARRAY_SIZE(try_list)) {
		cprintf("## NO SKU suit for %s\n", location);
		return;
	}

	cprintf("using %s SKU for %s\n", try_list[i], location);
	gen_ra_sku(try_list[i]);
}
#endif	/* RA_SINGLE_SKU */


/*=============================================================================
 smp_affinity: 1 = CPU1, 2 = CPU2, 3 = CPU3, 4 = CPU4
 rps_cpus: wxyz = CPU3 CPU2 CPU1 CPU0 (ex:0xd = 0'b1101 = CPU1, CPU3, CPU4)
=============================================================================*/


void setup_smp(int which)
{
#if !defined(RTCONFIG_NO_USBPORT)
	if(which == 1)
		doSystem("pbr-optimizer -m 1 &");//nas
	else
#endif
		//doSystem("pbr-optimizer -m 2 &");//lan
		doSystem("pbr-optimizer -m 3 &");//wifi
}


#ifdef RTCONFIG_SWCONFIG
extern void default_LANWANPartition();
void config_switch()
{
	char *switch_wantag = NULL;
	int stbport = 0;

	rtkswitch_AllPort_linkDown();

	if (config_switch_for_first_time){
			config_switch_for_first_time = 0;
	}
	else
	{
		dbG("software reset\n");
		eval("swconfig", "dev", "switch0", "set", "reset");
	}

	if (is_routing_enabled())
	{
		switch_wantag = nvram_safe_get("switch_wantag");
		stbport = nvram_get_int("switch_stb_x");

	extern void config_esw_LANWANPartition(int type);
	config_esw_LANWANPartition(0);
//CPU PORT is 5, LAN4 is 0 LAN3 is 1

		switch(stbport) {
			case 1:
				eval("swconfig", "dev", "switch0", "vlan","2","set", "ports", "3 4 5"); // set wan port
				break;
			case 2:
				eval("swconfig", "dev", "switch0", "vlan","2","set", "ports", "2 4 5"); // set wan port
				break;
			case 3:
				if (!strcmp(switch_wantag, "m1_fiber") || !strcmp(switch_wantag, "maxis_fiber_sp")) {
					//VLAN for internet, untag in switch
					eval("swconfig", "dev", "switch0", "vlan", nvram_safe_get("switch_wan0tagid"), "set", "ports", "4t 5");
					//PORT3 need to keep tag
					eval("swconfig", "dev", "switch0", "vlan", nvram_safe_get("switch_wan1tagid"), "set", "ports", "4t 1t");
				} else 
				if (!strcmp(switch_wantag, "maxis_fiber")) {
					//VLAN for internet, untag in switch
					eval("swconfig", "dev", "switch0", "vlan", nvram_safe_get("switch_wan0tagid"), "set", "ports", "4t 5");
					eval("swconfig", "dev", "switch0", "vlan", "821", "set", "ports", "4t 1t");
					eval("swconfig", "dev", "switch0", "vlan", "822", "set", "ports", "4t 1t");
				} else if (!strcmp(switch_wantag, "manual")) {
					if((strlen(nvram_safe_get("switch_wan0tagid")) != 0)){
						if(nvram_get_int("switch_wan0tagid") != nvram_get_int("switch_wan2tagid")) {
							eval("swconfig", "dev", "switch0", "vlan", nvram_safe_get("switch_wan0tagid"), "set", "ports", "4t 5");
							eval("swconfig", "dev", "switch0", "vlan", nvram_safe_get("switch_wan2tagid"), "set", "ports", "4t 0");
						} else {
							eval("swconfig", "dev", "switch0", "vlan", nvram_safe_get("switch_wan0tagid"), "set", "ports", "4t 5 0");
						}
					}else {
							eval("swconfig", "dev", "switch0", "vlan", nvram_safe_get("switch_wan2tagid"), "set", "ports", "4t 0");
							eval("swconfig", "dev", "switch0", "vlan", "2", "set", "ports", "4 5");
					}

				} else
				if(!strcmp(switch_wantag, "vodafone")) {
					//VLAN for internet, untag in switch
					eval("swconfig", "dev", "switch0", "vlan", "1", "set", "ports", "2 3 6");
					//PORT4
					eval("swconfig", "dev", "switch0", "vlan", "100", "set", "ports", "4t 0t 5");
					eval("swconfig", "dev", "switch0", "vlan", "101", "set", "ports", "4t 0t");
					eval("swconfig", "dev", "switch0", "vlan", "105", "set", "ports", "4t 0t 1");
				} else {
					eval("swconfig", "dev", "switch0", "vlan","2","set", "ports", "1 4 5"); // set wan port
				}

				break;
			case 4://LAN4 only
				if(!strcmp(switch_wantag, "unifi_home") || !strcmp(switch_wantag, "singtel_others") || !strcmp(switch_wantag, "orange")) {
					//VLAN for internet, untag in switch
					eval("swconfig", "dev", "switch0", "vlan", nvram_safe_get("switch_wan0tagid"), "set", "ports", "4t 5");
					//PORT4 need to untag in switch
					eval("swconfig", "dev", "switch0", "vlan", nvram_safe_get("switch_wan1tagid"), "set", "ports", "4t 0");
				} else 
				if(!strcmp(switch_wantag, "meo")) {
					//VLAN for internet, untag in switch
					eval("swconfig", "dev", "switch0", "vlan", nvram_safe_get("switch_wan0tagid"), "set", "ports", "4t 5");
					//PORT4 need to keep tag
					eval("swconfig", "dev", "switch0", "vlan", nvram_safe_get("switch_wan1tagid"), "set", "ports", "4t 0t");
					//PROT3 do nothing
				} else if (!strcmp(switch_wantag, "manual")) {
					if((strlen(nvram_safe_get("switch_wan0tagid")) != 0)){
						if(nvram_get_int("switch_wan0tagid") != nvram_get_int("switch_wan1tagid")) {
							eval("swconfig", "dev", "switch0", "vlan", nvram_safe_get("switch_wan0tagid"), "set", "ports", "4t 5");
							eval("swconfig", "dev", "switch0", "vlan", nvram_safe_get("switch_wan1tagid"), "set", "ports", "4t 0");
						} else {
							eval("swconfig", "dev", "switch0", "vlan", nvram_safe_get("switch_wan0tagid"), "set", "ports", "4t 5 0");
						}
					}else{
							eval("swconfig", "dev", "switch0", "vlan", nvram_safe_get("switch_wan1tagid"), "set", "ports", "4t 0");
							eval("swconfig", "dev", "switch0", "vlan", "2", "set", "ports", "4 5");
					}
				} else {
					eval("swconfig", "dev", "switch0", "vlan","2","set", "ports", "0 4 5"); // set wan port
				}

				break;
			case 5://LAN1+LAN2
				eval("swconfig", "dev", "switch0", "vlan","2","set", "ports", "2 3 4 5"); // set wan port
				break;
			case 6://LAN3+LAN4
				if(!strcmp(switch_wantag, "singtel_mio")) {
					//VLAN for internet, untag in switch
					eval("swconfig", "dev", "switch0", "vlan", nvram_safe_get("switch_wan0tagid"), "set", "ports", "4t 5");
					//PORT4 need to untag in switch
					eval("swconfig", "dev", "switch0", "vlan", nvram_safe_get("switch_wan1tagid"), "set", "ports", "4t 0");
					//PORT3 need to keep tag
					eval("swconfig", "dev", "switch0", "vlan", nvram_safe_get("switch_wan2tagid"), "set", "ports", "4t 1t");
				} else
				 if (!strcmp(switch_wantag, "manual")) {
					if((strlen(nvram_safe_get("switch_wan0tagid")) != 0)){
						if(nvram_get_int("switch_wan0tagid") != nvram_get_int("switch_wan1tagid") && nvram_get_int("switch_wan0tagid") != nvram_get_int("switch_wan2tagid")
							&& nvram_get_int("switch_wan1tagid") != nvram_get_int("switch_wan2tagid")) {
							eval("swconfig", "dev", "switch0", "vlan", nvram_safe_get("switch_wan0tagid"), "set", "ports", "4t 5");
							eval("swconfig", "dev", "switch0", "vlan", nvram_safe_get("switch_wan1tagid"), "set", "ports", "4t 0");
							eval("swconfig", "dev", "switch0", "vlan", nvram_safe_get("switch_wan2tagid"), "set", "ports", "4t 1");
						} else if(nvram_get_int("switch_wan0tagid") != nvram_get_int("switch_wan1tagid") && nvram_get_int("switch_wan1tagid") == nvram_get_int("switch_wan2tagid")){
							eval("swconfig", "dev", "switch0", "vlan", nvram_safe_get("switch_wan0tagid"), "set", "ports", "4t 5");
							eval("swconfig", "dev", "switch0", "vlan", nvram_safe_get("switch_wan1tagid"), "set", "ports", "4t 0 1");
						}else if(nvram_get_int("switch_wan0tagid") == nvram_get_int("switch_wan2tagid") && nvram_get_int("switch_wan1tagid") != nvram_get_int("switch_wan2tagid")){
							eval("swconfig", "dev", "switch0", "vlan", nvram_safe_get("switch_wan0tagid"), "set", "ports", "4t 5 1");
							eval("swconfig", "dev", "switch0", "vlan", nvram_safe_get("switch_wan1tagid"), "set", "ports", "4t 0");
						} else if(nvram_get_int("switch_wan0tagid") == nvram_get_int("switch_wan1tagid") && nvram_get_int("switch_wan1tagid") != nvram_get_int("switch_wan2tagid")){
							eval("swconfig", "dev", "switch0", "vlan", nvram_safe_get("switch_wan0tagid"), "set", "ports", "4t 5 0");
							eval("swconfig", "dev", "switch0", "vlan", nvram_safe_get("switch_wan2tagid"), "set", "ports", "4t 1");
						} else {
							eval("swconfig", "dev", "switch0", "vlan", nvram_safe_get("switch_wan0tagid"), "set", "ports", "4t 5 0 1");
						}
					}else{
						if(nvram_get_int("switch_wan1tagid") != nvram_get_int("switch_wan2tagid")) {
							eval("swconfig", "dev", "switch0", "vlan", nvram_safe_get("switch_wan1tagid"), "set", "ports", "4t 0");
							eval("swconfig", "dev", "switch0", "vlan", nvram_safe_get("switch_wan2tagid"), "set", "ports", "4t 1");
							eval("swconfig", "dev", "switch0", "vlan", "2", "set", "ports", "4 5");
						}else {
							eval("swconfig", "dev", "switch0", "vlan", nvram_safe_get("switch_wan1tagid"), "set", "ports", "4t 0 1");
							eval("swconfig", "dev", "switch0", "vlan", "2", "set", "ports", "4 5");
						}
					}

				} else
					eval("swconfig", "dev", "switch0", "vlan","2","set", "ports", "0 1 4 5"); // set wan port

				break;
			default:
				if(!strcmp(switch_wantag, "movistar")) {
					eval("swconfig", "dev", "switch0", "vlan", "6", "set", "ports", "4t 5");
					eval("swconfig", "dev", "switch0", "vlan", "3", "set", "ports", "4t 5t");
					eval("swconfig", "dev", "switch0", "vlan", "2", "set", "ports", "4t 5t");
#if defined(RT4GAC86U)
					set_wan_tag("eth1");
#endif
				}
				if(nvram_get_int("switch_wan0tagid"))
					eval("swconfig", "dev", "switch0", "vlan", nvram_safe_get("switch_wan0tagid"), "set", "ports", "4t 5");
				else
					eval("swconfig", "dev", "switch0", "vlan","2","set", "ports", "4 5"); // set wan port
				break;
		}

		eval("swconfig", "dev", "switch0", "port","4","set", "vlan_prio", nvram_safe_get("switch_wan0prio"));
		eval("swconfig", "dev", "switch0", "port","0","set", "vlan_prio", nvram_safe_get("switch_wan1prio"));
		eval("swconfig", "dev", "switch0", "port","1","set", "vlan_prio", nvram_safe_get("switch_wan2prio"));
	}
	else
	{
		default_LANWANPartition();
	}

	rtkswitch_AllPort_linkUp();
}
#endif	/* RTCONFIG_SWCONFIG */

#if defined(RTCONFIG_SWRT_KVR)
void gen_wapp_config(void)
{
	FILE *fp = NULL;
	if(!f_exists("/etc/wapp_ap_ra0.conf"))
		system("cp /etc/wappd/wapp_ap_ra0_default.conf /etc/wapp_ap_ra0.conf");
#if defined(RTCONFIG_MT798X)
	if(!f_exists("/etc/wapp_ap_rax0.conf"))
		system("cp /etc/wappd/wapp_ap_rax0_default.conf /etc/wapp_ap_rax0.conf");
#else
	if(!f_exists("/etc/wapp_ap_rai0.conf"))
		system("cp /etc/wappd/wapp_ap_rai0_default.conf /etc/wapp_ap_rai0.conf");
#endif
	system("rm -rf /tmp/wapp*");
	if((fp = fopen("/etc/wapp_ap.conf", "w")))
	{
		fprintf(fp, "all_main_inf=");
		if(nvram_match("wl0_radio", "1"))
			fprintf(fp, "%s;", WIF_2G);
		if(nvram_match("wl1_radio", "1"))
			fprintf(fp, "%s;", WIF_5G);
		fprintf(fp, "\n");
		fclose(fp);
	}else
		_dprintf("Can't open /etc/wapp_ap.conf\n");
	if((fp = fopen("/etc/wapp_main_inf.conf", "w")))
	{
		fprintf(fp, "conf_list=");
		if(nvram_match("wl0_radio", "1"))
			fprintf(fp, "wapp_ap_%s.conf;", WIF_2G);
		if(nvram_match("wl1_radio", "1"))
			fprintf(fp, "wapp_ap_%s.conf;", WIF_5G);
		fprintf(fp, "\n");
		fclose(fp);
	}else
		_dprintf("Can't open /etc/wapp_main_inf.conf\n");
}

void start_wapp(void)
{
	char *wapp_argv[] = { "wapp", "-d1", "-v2", NULL, NULL, NULL };
	pid_t pid;
	int if_index = 3;

	if (getpid() != 1) {
		notify_rc("start_wapp");
		return;
	}

	gen_wapp_config();
	if(nvram_match("wl0_radio", "1"))
		wapp_argv[if_index++] = "-cra0";
	if(nvram_match("wl1_radio", "1"))
#if defined(RTCONFIG_MT798X)
		wapp_argv[if_index++] = "-crax0";
#else
		wapp_argv[if_index++] = "-crai0";
#endif
	if(pids("wapp"))
		killall_tk("wapp");
	_eval(wapp_argv, NULL, 0, &pid);
	_dprintf("###wapp start:%d\n", pid);
}

void stop_wapp(void)
{
	if (getpid() != 1) {
		notify_rc("stop_wapp");
		return;
	}

	if (pids("wapp"))
		killall_tk("wapp");
}
#endif
#if defined(RTCONFIG_EASYMESH)
#define MAP_DISABLED		0
#define MAP_TURNKEY			1
#define MAP_BS_2_0			2
#define MAP_API_MODE		3
#define MAP_CERT_MODE		4
void gen_1905d_config(void)
{
	int sw_mode = sw_mode();
	FILE *fp = NULL;
	system("mkdir -p /etc/map");
	if((fp = fopen("/etc/map/1905d.cfg", "w")))
	{
		if(nvram_get_int("easymesh_role") == EASYMESH_ROLE_MASTER && !mesh_re_node()){
// This device is a MAP root
			fprintf(fp, "map_root=1\n");
// Has MAP agent on this device
			fprintf(fp, "map_agent=0\n");
		}else{
			fprintf(fp, "map_root=0\n");
			fprintf(fp, "map_agent=1\n");
		}
// Controller's ALID
		fprintf(fp, "map_controller_alid=%s\n", get_lan_hwaddr());
// Agent's ALID
		fprintf(fp, "map_agent_alid=%s\n", get_lan_hwaddr());
// Default Backhault Type
//		fprintf(fp, "bh_type=wifi\n");
		fprintf(fp, "bh_type=eth\n");
//Config Band setting of each Radio
#if defined(RTCONFIG_HAS_5G_2)
		fprintf(fp, "radio_band=24G;5GL;5GH\n");
#else
		fprintf(fp, "radio_band=24G;5G;5G\n");
#endif
//bridge interface
		fprintf(fp, "br_inf=br0\n");
//lan interface
		if(nvram_match("easymesh_perportpervlan", "1")){
#if defined(RTCONFIG_RALINK_MT7621) || defined(RTCONFIG_RALINK_EN7561)
			char *lan_ifname = "vlan1";
#else
			char *lan_ifname = "eth0";
#endif
			char lan1[9], lan2[9], lan3[9], lan4[9];
			snprintf(lan1, sizeof(lan1), "%s.1", lan_ifname);
			snprintf(lan2, sizeof(lan2), "%s.2", lan_ifname);
			snprintf(lan3, sizeof(lan3), "%s.3", lan_ifname);
			snprintf(lan4, sizeof(lan4), "%s.4", lan_ifname);
			fprintf(fp, "lan=%s\n", lan1);
			fprintf(fp, "lan=%s\n", lan2);
			fprintf(fp, "lan=%s\n", lan3);
			fprintf(fp, "lan=%s\n", lan4);
			fprintf(fp, "lan_vid=1;2;3;4;\n");
			fprintf(fp, "wan_vid=5;\n");
			fprintf(fp, "wan=%s\n", nvram_safe_get("wan0_ifname"));
		}else{
#if defined(RTCONFIG_RALINK_MT7621) || defined(RTCONFIG_RALINK_EN7561)
			fprintf(fp, "lan=vlan1\n");
#else
			fprintf(fp, "lan=lan1\n");
			fprintf(fp, "lan=lan2\n");
			fprintf(fp, "lan=lan3\n");
#if !defined(RMAX6000)
			fprintf(fp, "lan=lan4\n");//1G
#if 0
			fprintf(fp, "lan=lan5\n");//2.5G
#endif
#endif
#endif
			fprintf(fp, "lan_vid=1;\n");
			fprintf(fp, "wan_vid=2;\n");
			fprintf(fp, "wan=%s\n", nvram_safe_get("wan0_ifname"));
		}
//		fprintf(fp, "veth=xxx\n");
		fprintf(fp, "map_ver=%s\n", get_easymesh_ver_str(get_easymesh_max_ver()));
//fixed al_mac of al_inf(only wifi inf) mac
//		fprintf(fp, "al_inf=ra0\n");
		fprintf(fp, "bss_config_priority=");
#if defined(RTCONFIG_HAS_5G_2)
		fprintf(fp, "%s;", WIF_5G2);
#endif
		fprintf(fp, "%s;%s;", WIF_5G, WIF_2G);
#if defined(RTCONFIG_HAS_5G_2)
//		fprintf(fp, "%s;", APCLI_5G2);
#endif
//		fprintf(fp, "%s;%s;", APCLI_2G, APCLI_5G);
		fprintf(fp, "\n");
		fprintf(fp, "config_agent_port=9008\n");
//ethernet device name used to read the switch table
//do not set if not understanding
//		fprintf(fp, "ethernet_dev_name=\n");
//		if(is_router_mode())
//			fprintf(fp, "transparent_vids=\n");
//		fprintf(fp, "discovery_cnt=\n");
		if(get_easymesh_max_ver() > 2){
			fprintf(fp, "decrypt_fail_threshold=10\n");
			fprintf(fp, "gtk_rekey_interval=3600\n");
//Enable Ethernet Onboarding on WAN PORT only
			fprintf(fp, "ob_wan_only=0\n");
		}
		fclose(fp);
	}else
		_dprintf("Can't open /etc/map/1905d.cfg\n");
	if((fp = fopen("/etc/map/ethernet_cfg.txt", "w")))
	{
#if defined(RTCONFIG_RALINK_MT7621) || defined(RTCONFIG_RALINK_EN7561)
//set the vid of the LAN group, the range is 1-4094, and not repeat with wan_vid
		fprintf(fp, "lan_vid=1;\n");
		fprintf(fp, "lan=vlan1;\n");
//set the vid of the WAN group, the range is 1-4094, and not repeat with lan_vid
		fprintf(fp, "wan_vid=2;\n");
#else
		fprintf(fp, "lan=lan1");
		fprintf(fp, " lan2");
		fprintf(fp, " lan3");
#if !defined(RMAX6000)
		fprintf(fp, " lan4");//1G
#if 0
		fprintf(fp, " lan5");//2.5G
#endif
#endif
		fprintf(fp, "\n");
		fprintf(fp, "br_lan=br0\n");
#endif
//set the CPU port number, where the switch and CPU are connected.
//The witch-7530 can be selected as 5 or 6.
//The switch-7531 can only set one of 5 and 6, or you can set them at the same time
//		fprintf(fp, "cpu_port=6;\n");
#if defined(RTCONFIG_MT798X)
//specify external phy port, used as lan port while bridge mode
//panther with one 2.5G phy port, ext_phy_port=6
//panther with two 2.5G phy port, ext_phy_port=5 6
#if 0
		fprintf(fp, "ext_phy_port=5 6\n");
#else
		fprintf(fp, "ext_phy_port=\n");
#endif
#endif
		fclose(fp);
		symlink("/etc/map/ethernet_cfg.txt", "/etc/ethernet_cfg.txt");
	}else
		_dprintf("Can't open /etc/map/ethernet_cfg.txt\n");
}

void gen_mapd_strng(void)
{
	FILE *fp = NULL;
	if((fp = fopen("/etc/mapd_strng.conf", "w")))
	{
		fprintf(fp, "LowRSSIAPSteerEdge_RE=40\n");
		fprintf(fp, "CUOverloadTh_2G=70\n");
		fprintf(fp, "CUOverloadTh_5G_L=80\n");
		fprintf(fp, "CUOverloadTh_5G_H=80\n");
		fprintf(fp, "MetricPolicyChUtilThres_24G=70\n");
		fprintf(fp, "MetricPolicyChUtilThres_5GL=80\n");
		fprintf(fp, "MetricPolicyChUtilThres_5GH=80\n");
		fprintf(fp, "ChPlanningChUtilThresh_24G=70\n");
		fprintf(fp, "ChPlanningChUtilThresh_5GL=80\n");
		fprintf(fp, "ChPlanningEDCCAThresh_24G=200\n");
		fprintf(fp, "ChPlanningEDCCAThresh_5GL=200\n");
		fprintf(fp, "ChPlanningOBSSThresh_24G=200\n");
		fprintf(fp, "ChPlanningOBSSThresh_5GL=200\n");
		fprintf(fp, "ChPlanningR2MonitorTimeoutSecs=300\n");
		fprintf(fp, "ChPlanningR2MonitorProhibitSecs=900\n");
		fprintf(fp, "ChPlanningR2MetricReportingInterval=10\n");
		fprintf(fp, "ChPlanningR2MinScoreMargin=10\n");
		fprintf(fp, "MetricRepIntv=10\n");
		fprintf(fp, "MetricPolicyRcpi_24G=100\n");
		fprintf(fp, "MetricPolicyRcpi_5GL=100\n");
		fprintf(fp, "MetricPolicyRcpi_5GH=100\n");
		fclose(fp);
	}else
		_dprintf("Can't open /etc/mapd_strng.conf\n");
}

void gen_mapd_config(void)
{
	int sw_mode = sw_mode();
	char chList[256];
	char *channel = NULL;
	FILE *fp = NULL;
	if((fp = fopen("/etc/map/mapd_default.cfg", "w")))
	{
		if(get_easymesh_max_ver() > 2){
			if(sw_mode == SW_MODE_ROUTER)
				fprintf(fp, "mode=1\n");
			else
				fprintf(fp, "mode=2\n");
		}
		fprintf(fp, "lan_interface=br0\n");
		if(sw_mode != SW_MODE_ROUTER)
			fprintf(fp, "wan_interface=\n");
		else
			fprintf(fp, "wan_interface=%s\n", nvram_safe_get("wan0_ifname"));
		if(nvram_get_int("easymesh_role") == EASYMESH_ROLE_AGENT)
			fprintf(fp, "DeviceRole=2\n");
		else if(nvram_get_int("easymesh_role") == EASYMESH_ROLE_MASTER)
			fprintf(fp, "DeviceRole=1\n");
		else//auto
			fprintf(fp, "DeviceRole=0\n");
		fprintf(fp, "APSteerRssiTh=-54\n");
#if defined(RTCONFIG_WIFI6E)
		fprintf(fp, "BhPriority6G=1\n");
		fprintf(fp, "BhPriority2G=4\n");
		fprintf(fp, "BhPriority5GL=2\n");
		fprintf(fp, "BhPriority5GH=2\n");
#else
		fprintf(fp, "BhPriority2G=3\n");
		fprintf(fp, "BhPriority5GL=1\n");
		fprintf(fp, "BhPriority5GH=1\n");
#endif
		fprintf(fp, "ChPlanningIdleByteCount=\n");
		fprintf(fp, "ChPlanningIdleTime=\n");
		if(!nvram_match("wl1_channel", "0")){
			if(nvram_get_int("wl1_channel") < 100){
				fprintf(fp, "ChPlanningUserPreferredChannel5G=%s\n", nvram_get("wl1_channel"));
				fprintf(fp, "ChPlanningUserPreferredChannel5GH=\n");
			}else{
				fprintf(fp, "ChPlanningUserPreferredChannel5G=\n");
				fprintf(fp, "ChPlanningUserPreferredChannel5GH=%s\n", nvram_get("wl1_channel"));
			}
		}else{
			fprintf(fp, "ChPlanningUserPreferredChannel5G=\n");
			fprintf(fp, "ChPlanningUserPreferredChannel5GH=\n");
		}
		if(!nvram_match("wl0_channel", "0"))
			fprintf(fp, "ChPlanningUserPreferredChannel2G=%s\n", nvram_get("wl0_channel"));
		else
			fprintf(fp, "ChPlanningUserPreferredChannel2G=\n");
		fprintf(fp, "ChPlanningInitTimeout=120\n");
		fprintf(fp, "NtwrkOptBootupWaitTime=45\n");
		fprintf(fp, "NtwrkOptConnectWaitTime=45\n");
		fprintf(fp, "NtwrkOptDisconnectWaitTime=45\n");
		fprintf(fp, "NtwrkOptPeriodicity=3600\n");
		fprintf(fp, "NetworkOptimizationScoreMargin=100\n");
		fprintf(fp, "BandSwitchTime=\n");
		fprintf(fp, "ScanThreshold2g=-75\n");
		fprintf(fp, "ScanThreshold5g=-75\n");
#if defined(RTCONFIG_WIFI6E)
		fprintf(fp, "ScanThreshold6g=-75\n");
#endif
		fprintf(fp, "LowRSSIAPSteerEdge_RE=40\n");
		fprintf(fp, "CUOverloadTh_2G=70\n");
		fprintf(fp, "CUOverloadTh_5G_L=80\n");
		fprintf(fp, "CUOverloadTh_5G_H=80\n");
		fprintf(fp, "BhProfile0Valid=\n");
		fprintf(fp, "BhProfile0Ssid=\n");
		fprintf(fp, "BhProfile0AuthMode=\n");
		fprintf(fp, "BhProfile0EncrypType=\n");
		fprintf(fp, "BhProfile0WpaPsk=\n");
		fprintf(fp, "BhProfile0RaID=\n");
		fprintf(fp, "BhProfile1Ssid=\n");
		fprintf(fp, "BhProfile1AuthMode=\n");
		fprintf(fp, "BhProfile1EncrypType=\n");
		fprintf(fp, "BhProfile1WpaPsk=\n");
		fprintf(fp, "BhProfile1Valid=\n");
		fprintf(fp, "BhProfile1RaID=\n");
		fprintf(fp, "BhProfile2Ssid=\n");
		fprintf(fp, "BhProfile2AuthMode=\n");
		fprintf(fp, "BhProfile2EncrypType=\n");
		fprintf(fp, "BhProfile2WpaPsk=\n");
		fprintf(fp, "BhProfile2Valid=\n");
		fprintf(fp, "BhProfile2RaID=\n");
		fprintf(fp, "ChPlanningEnable=1\n");
		fprintf(fp, "ChPlanningEnableR2=1\n");
		fprintf(fp, "ChPlanningEnableR2withBW=1\n");
		fprintf(fp, "SteerEnable=1\n");
		fprintf(fp, "NetworkOptimizationEnabled=1\n");
		fprintf(fp, "AutoBHSwitching=1\n");
		fprintf(fp, "DhcpCtl=%s\n", nvram_get("easymesh_dhcpctl"));
		fprintf(fp, "ThirdPartyConnection=%s\n", nvram_get("easymesh_tpcon"));
		fprintf(fp, "MAP_QuickChChange=1\n");
		fprintf(fp, "bss_config_priority=");
#if defined(RTCONFIG_HAS_5G_2)
		if(nvram_match("wl2_radio", "1"))
			fprintf(fp, "%s;", WIF_5G2);
#endif
		if(nvram_match("wl1_radio", "1"))
			fprintf(fp, "%s;", WIF_5G);
		if(nvram_match("wl0_radio", "1"))
			fprintf(fp, "%s;", WIF_2G);
		fprintf(fp, "\n");
		fprintf(fp, "DualBH=\n");
		fprintf(fp, "MetricRepIntv=60\n");
		fprintf(fp, "MaxAllowedScan=\n");
		fprintf(fp, "BHSteerTimeout=120\n");
		if(get_easymesh_max_ver() > 2)
			fprintf(fp, "NtwrkOptPostCACTriggerTime=30\n");
		else
			fprintf(fp, "NtwrkOptPostCACTriggerTime=25\n");
		fprintf(fp, "role_detection_external=0\n");
		fprintf(fp, "NetworkOptPrefer5Gover2G=0\n");
		fprintf(fp, "NetworkOptPrefer5Gover2GRetryCnt=0\n");
		fprintf(fp, "NonMAPAPEnable=1\n");
		fprintf(fp, "CentralizedSteering=1\n");
		fprintf(fp, "DivergentChPlanning=0\n");
		fprintf(fp, "LastMapMode=%s\n", nvram_get("easymesh_lastmode") ? : nvram_get("easymesh_mode"));
		fprintf(fp, "NtwrkOptDataCollectionTime=300\n");
		if(get_easymesh_max_ver() > 2){
			fprintf(fp, "ChPlanningScanValidTime=14400\n");
			fprintf(fp, "NetOptUserSetPriority=0\n");
			fprintf(fp, "DESerialNumber=v3.0\n");
			fprintf(fp, "DESoftwareVersion=v3.0\n");
			fprintf(fp, "DEExecutionEnv=SWRT\n");
			fprintf(fp, "DEChipsetVendor=MediaTek\n");
			fprintf(fp, "DEStaConEventPath=\n");
			fprintf(fp, "MaxVHT_BW5G==1\n");//20/40/80
			fprintf(fp, "MaxHT_BW5G==1\n");//40
			fprintf(fp, "MaxHT_BW2G==1\n");//40
		}
		fclose(fp);
		symlink("/etc/map/mapd_default.cfg", "/etc/map/mapd_cfg");
	}else
		_dprintf("Can't open /etc/map/mapd_default.cfg\n");
	if(!f_exists("/jffs/swrtmesh/mapd_user.cfg")){
		eval("mkdir", "-p", "/jffs/swrtmesh");
		if((fp = fopen("/jffs/swrtmesh/mapd_user.cfg", "w"))){
			fprintf(fp, "###!UserConfigs!!!\n");
			fclose(fp);
		}
	}
	symlink("/jffs/swrtmesh/mapd_user.cfg", "/etc/map/mapd_user.cfg");
	if(!f_exists("/jffs/swrtmesh/client_db.txt"))
		doSystem("touch /jffs/swrtmesh/client_db.txt");
	gen_mapd_strng();
}

static inline void perportpervlan_conf(void)
{
	char *lan_hwaddr = get_lan_hwaddr();
	char *lan_ifname = "vlan1"; //nvram_get("lan_ifname");
	char *bridge_name = nvram_get("lan_ifname");
	char lan1[9], lan2[9], lan3[9], lan4[9];
	char lan1_mac[18], lan2_mac[18], lan3_mac[18], lan4_mac[18];
	snprintf(lan1, sizeof(lan1), "%s.1", lan_ifname);
	snprintf(lan2, sizeof(lan2), "%s.2", lan_ifname);
	snprintf(lan3, sizeof(lan3), "%s.3", lan_ifname);
	snprintf(lan4, sizeof(lan4), "%s.4", lan_ifname);
	ether_cal(lan_hwaddr, lan1_mac, 11);
	ether_cal(lan_hwaddr, lan2_mac, 12);
	ether_cal(lan_hwaddr, lan3_mac, 13);
	ether_cal(lan_hwaddr, lan4_mac, 14);
	eval("brctl", "delif", bridge_name, lan_ifname);
	eval("vconfig", "add",lan1,"1");
	eval("vconfig", "add",lan2,"2");
	eval("vconfig", "add",lan3,"3");
	eval("vconfig", "add",lan4,"4");
	eval("ifconfig", lan1, "hw", "ether", lan1_mac);
	eval("ifconfig", lan1, "up");
	eval("ifconfig", lan2, "hw", "ether", lan2_mac);
	eval("ifconfig", lan2, "up");
	eval("ifconfig", lan3, "hw", "ether", lan3_mac);
	eval("ifconfig", lan3, "up");
	eval("ifconfig", lan4, "hw", "ether", lan4_mac);
	eval("ifconfig", lan4, "up");
	doSystem("switch vlan set 0 1 10000011");
	doSystem("switch vlan set 1 2 01000011");
	doSystem("switch vlan set 2 3 00100011");
	doSystem("switch vlan set 3 4 00010011");
	doSystem("switch vlan set 5 5 00001101");
	doSystem("switch vlan pvid 0 1");
	doSystem("switch vlan pvid 1 2");
	doSystem("switch vlan pvid 2 3");
	doSystem("switch vlan pvid 3 4");
	doSystem("switch vlan pvid 4 5");
	doSystem("switch vlan pvid 5 5");
	doSystem("switch reg w 2610 81000000");
//tag mode
	doSystem("switch reg w 2604 20ff0003");
	eval("brctl", "addif", bridge_name, lan1);
	eval("brctl", "addif", bridge_name, lan2);
	eval("brctl", "addif", bridge_name, lan3);
	eval("brctl", "addif", bridge_name, lan4);
}

int getauthmode(int band)
{
	char prefix[] = "wlx_xxxxxxxxxxxxxxxx";
	char *authmode = NULL;
	if(band > WL_60G_BAND)
		snprintf(prefix, sizeof(prefix), "wl%d.4_auth_mode_x", band == WL_2GBH_BAND ? 0 : 1);
	else
		snprintf(prefix, sizeof(prefix), "wl%d_auth_mode_x", band);
	authmode = nvram_safe_get(prefix);
	if(!strcmp(authmode, "open"))
		return 0x1;
	else if(!strcmp(authmode, "psk"))
		return 0x2;
	else if(!strcmp(authmode, "psk2"))
		return 0x20;
	else if(!strcmp(authmode, "pskpsk2"))
		return 0x22;
	else if(!strcmp(authmode, "sae"))
		return 0x40;
	else if(!strcmp(authmode, "psk2sae"))
		return 0x60;
#if defined(RTCONFIG_EASYMESH_R3)
	else if(!strcmp(authmode, "dpp"))//dpp only
		return 0x80;
	else if(!strcmp(authmode, "dpppsk2"))//dpp + psk2
		return 0xA0;
	else if(!strcmp(authmode, "dppsae"))//dpp + sae
		return 0xC0;
	else if(!strcmp(authmode, "dpppsk2sae"))//dpp + psk2sae
		return 0xE0;
#endif
	else//bug
		return 0x20;
}

int getencrypttype(int band)
{
	char prefix[] = "wlx_xxxxxxxxxxxxxxxx";
	char *authmode = NULL, *wep = NULL, *crypto = NULL;
	if(band > WL_60G_BAND)
		snprintf(prefix, sizeof(prefix), "wl%d.4_auth_mode_x", band == WL_2GBH_BAND ? 0 : 1);
	else
		snprintf(prefix, sizeof(prefix), "wl%d_auth_mode_x", band);
	authmode = nvram_safe_get(prefix);
	if(band > WL_60G_BAND)
		snprintf(prefix, sizeof(prefix), "wl%d.4_wep_x", band == WL_2GBH_BAND ? 0 : 1);
	else
		snprintf(prefix, sizeof(prefix), "wl%d_wep_x", band);
	wep = nvram_safe_get(prefix);
	if(band > WL_60G_BAND)
		snprintf(prefix, sizeof(prefix), "wl%d.4_crypto", band == WL_2GBH_BAND ? 0 : 1);
	else
		snprintf(prefix, sizeof(prefix), "wl%d_crypto", band);
	crypto = nvram_safe_get(prefix);
	if(!strcmp(authmode, "open") && nvram_match(wep, "0"))
		return 0x1;
	else if(!strcmp(authmode, "sae") || !strcmp(authmode, "psk2sae") || !strcmp(crypto, "aes"))
		return 0x8;
#if !defined(RTCONFIG_EASYMESH_R3)
	else if(!strcmp(authmode, "psk") && !strcmp(crypto, "tkip"))
		return 0x4;
	else if(!strcmp(crypto, "tkip+aes"))
		return 0xc;
#else
	else if(!strcmp(authmode, "dpp") || !strcmp(authmode, "dpppsk2") || !strcmp(authmode, "dppsae") || !strcmp(authmode, "dpppsk2sae"))
		return 0x8;
#endif
	else//bug
		return 8;
}

char *getpsk(int band)
{
	char prefix[] = "wlx_xxxxxxxxxxxxxxxx";
	if(band > WL_60G_BAND)
		snprintf(prefix, sizeof(prefix), "wl%d.4_wpa_psk", band == WL_2GBH_BAND ? 0 : 1);
	else
		snprintf(prefix, sizeof(prefix), "wl%d_wpa_psk", band);
	return nvram_safe_get(prefix);

}

void gen_bhwifi_conf()
{
	int index = 1;
	FILE *fp = NULL;
	char ssid[128] = {0};
	if((fp = fopen("/etc/map/wts_bss_info_config", "w"))){
		fprintf(fp, "#ucc_bss_info\n");
		snprintf(ssid, sizeof(ssid), "%s", nvram_safe_get("wl0_ssid"));
		fprintf(fp, "%d,ff:ff:ff:ff:ff:ff 8x %s 0x%04x 0x%04x %s 0 1 hidden-N\n", index, mesh_format_ssid(ssid, sizeof(ssid)), getauthmode(WL_2G_BAND), getencrypttype(WL_2G_BAND), getpsk(WL_2G_BAND));
		index++;
		fprintf(fp, "%d,ff:ff:ff:ff:ff:ff 8x %s 0x%04x 0x%04x %s 1 0 hidden-Y\n", index, nvram_safe_get("wl0.4_ssid"), getauthmode(WL_2GBH_BAND), getencrypttype(WL_2GBH_BAND), getpsk(WL_2GBH_BAND));
		index++;
		snprintf(ssid, sizeof(ssid), "%s", nvram_safe_get("wl1_ssid"));
		fprintf(fp, "%d,ff:ff:ff:ff:ff:ff 11x %s 0x%04x 0x%04x %s 0 1 hidden-N\n", index, mesh_format_ssid(ssid, sizeof(ssid)), getauthmode(WL_5G_BAND), getencrypttype(WL_5G_BAND), getpsk(WL_5G_BAND));
		index++;
		fprintf(fp, "%d,ff:ff:ff:ff:ff:ff 12x %s 0x%04x 0x%04x %s 0 1 hidden-N\n", index, mesh_format_ssid(ssid, sizeof(ssid)), getauthmode(WL_5G_BAND), getencrypttype(WL_5G_BAND), getpsk(WL_5G_BAND));
		index++;
		fprintf(fp, "%d,ff:ff:ff:ff:ff:ff 11x %s 0x%04x 0x%04x %s 1 0 hidden-Y\n", index, nvram_safe_get("wl1.4_ssid"), getauthmode(WL_5GBH_BAND), getencrypttype(WL_5GBH_BAND), getpsk(WL_5GBH_BAND));
		index++;
		fprintf(fp, "%d,ff:ff:ff:ff:ff:ff 12x %s 0x%04x 0x%04x %s 1 0 hidden-Y\n", index, nvram_safe_get("wl1.4_ssid"), getauthmode(WL_5GBH_BAND), getencrypttype(WL_5GBH_BAND), getpsk(WL_5GBH_BAND));
		fclose(fp);
	}else
		printf("failed to open %s\n", "/etc/map/wts_bss_info_config");
}

static void setup_wifi_vlan(void){
	int i;
	char ifname[16];
	char path[256];
	for(i = 0; i < MAX_NO_MSSID; i++){
		snprintf(ifname, sizeof(ifname), "ra%d", i);
		snprintf(path, sizeof(path), "/sys/class/net/ra%d/ifindex", i);
		if(f_exists(path)){
			eval("iwpriv", ifname, "set", "VLANTag=1");
			eval("iwpriv", ifname, "set", "VLANPolicy=0:4");
			eval("iwpriv", ifname, "set", "VLANPolicy=1:2");
		}
#if defined(RTCONFIG_MT798X)
		snprintf(ifname, sizeof(ifname), "rax%d", i);
		snprintf(path, sizeof(path), "/sys/class/net/rax%d/ifindex", i);
#else
		snprintf(ifname, sizeof(ifname), "rai%d", i);
		snprintf(path, sizeof(path), "/sys/class/net/rai%d/ifindex", i);
#endif
		if(f_exists(path)){
			eval("iwpriv", ifname, "set", "VLANTag=1");
			eval("iwpriv", ifname, "set", "VLANPolicy=0:4");
			eval("iwpriv", ifname, "set", "VLANPolicy=1:2");
		}
	}
	eval("iwpriv", (char*) APCLI_2G, "set", "VLANTag=1");
	eval("iwpriv", (char*) APCLI_5G, "set", "VLANTag=1");
	eval("iwpriv", (char*) APCLI_2G, "set", "VLANPolicy=0:4");
	eval("iwpriv", (char*) APCLI_5G, "set", "VLANPolicy=0:4");
	eval("iwpriv", (char*) APCLI_2G, "set", "VLANPolicy=1:2");
	eval("iwpriv", (char*) APCLI_5G, "set", "VLANPolicy=1:2");
}

void start_mapd(void)
{
	int sw_mode = nvram_get_int("sw_mode");
	int enable = nvram_get_int("easymesh_enable");
	int mode = nvram_get_int("easymesh_mode");
	int role = nvram_get_int("easymesh_role"); /* GW:1, AP:1/2, RP:1 */
	int lastmode = nvram_get_int("easymesh_lastmode");
	int dhcpctl = nvram_get_int("easymesh_dhcpctl"); /* AP/RP:1, GW:0 */
	int tpcon = nvram_get_int("easymesh_tpcon"); /* AP/RP:1, GW:0 */
	char *bh2gifname = get_mesh_bh_ifname(WL_2G_BAND);
	char *bh5gifname = get_mesh_bh_ifname(WL_5G_BAND);
	char *ap2gifname = get_wififname(WL_2G_BAND);
	char *ap5gifname = get_wififname(WL_5G_BAND);
	char *apcli2gifname = get_staifname(WL_2G_BAND);
	char *apcli5gifname = get_staifname(WL_5G_BAND);

	pid_t pid;
	char *mapd_argv[] = { "/usr/sbin/mapd", "-I", "/etc/map/mapd_cfg", "-O", "/etc/mapd_strng.conf", "-c", "/jffs/swrtmesh/client_db.txt", NULL, NULL };
	char *p1905_argv[] = { "/usr/sbin/p1905_managerd", "-r0", "-f", "/etc/map/1905d.cfg", "-F", "/etc/map/wts_bss_info_config", NULL };
#if defined(RTCONFIG_RALINK_MT7621) || defined(RTCONFIG_RALINK_EN7561)
	char *fwdd_argv[] = { "/usr/sbin/fwdd", "-p", ap2gifname, apcli2gifname, "-p", ap5gifname, apcli5gifname, "-e", "vlan1", "5G", NULL };
#else
	char *fwdd_argv[] = { "/usr/sbin/fwdd", "-d1", "-e", "eth0", "5G", "-e", "lan1", "5G", "-e", "lan2", "5G", "-e", "lan3", "5G",
#if !defined(RMAX6000)
		"-e", "lan4", "5G",
#if 0
		"-e", "lan5", "5G",
		"-e", "lan6", "5G",
#endif
#endif
		NULL };
#endif

	mkdir("/etc/map", 0777);
	if(enable == 0){
		eval("iwpriv", (char*) WIF_2G, "set", "mapEnable=0");
		eval("iwpriv", (char*) WIF_5G, "set", "mapEnable=0");
		eval("iwpriv", (char*) WIF_2G, "set", "mapTurnKey=0");
		eval("iwpriv", (char*) WIF_5G, "set", "mapTurnKey=0");
        doSystem("rm -rf /tmp/wapp_ctrl");
		if(pids("wapp"))
			killall_tk("wapp");
		if(pids("p1905_managerd"))
			killall_tk("p1905_managerd");
		if(pids("mapd"))
			killall_tk("mapd");
		start_wapp();
		if(!module_loaded("mtfwd"))
			modprobe("mtfwd");
		if(pids("fwdd"))
			killall_tk("fwdd");
		_eval(fwdd_argv, NULL, 0, &pid);
		return;
	}else{
		doSystem("rm -rf /tmp/wapp_ctrl");
		if(pids("wapp"))
			killall_tk("wapp");
		if(pids("p1905_managerd"))
			killall_tk("p1905_managerd");
		if(pids("mapd"))
			killall_tk("mapd");
		if(pids("bs20"))
			killall_tk("bs20");
		if(module_loaded("mapfilter"))
			modprobe_r("mapfilter");
#if defined(RTCONFIG_EASYMESH_R3)
		eval("iwpriv", (char*) WIF_2G, "set", "mapR3Enable=0");
		eval("iwpriv", (char*) WIF_2G, "set", "DppEnable=0");
		eval("iwpriv", (char*) WIF_2G, "set", "mapTSEnable=0");
		eval("iwpriv", (char*) WIF_5G, "set", "mapR3Enable=0");
		eval("iwpriv", (char*) WIF_5G, "set", "DppEnable=0");
		eval("iwpriv", (char*) WIF_5G, "set", "mapTSEnable=0");
		eval("iwpriv", bh2gifname, "set", "mapR3Enable=0");
		eval("iwpriv", bh5gifname, "set", "mapR3Enable=0");
		eval("iwpriv", bh2gifname, "set", "DppEnable=0");
		eval("iwpriv", bh5gifname, "set", "DppEnable=0");
		eval("iwpriv", bh2gifname, "set", "mapTSEnable=0");
		eval("iwpriv", bh5gifname, "set", "mapTSEnable=0");
#else
		eval("iwpriv", (char*) WIF_2G, "set", "mapR2Enable=0");
		eval("iwpriv", (char*) WIF_2G, "set", "mapTSEnable=0");
		eval("iwpriv", (char*) WIF_5G, "set", "mapR2Enable=0");
		eval("iwpriv", (char*) WIF_5G, "set", "mapTSEnable=0");
		eval("iwpriv", bh2gifname, "set", "mapR2Enable=0");
		eval("iwpriv", bh5gifname, "set", "mapR2Enable=0");
		eval("iwpriv", bh2gifname, "set", "mapTSEnable=0");
		eval("iwpriv", bh5gifname, "set", "mapTSEnable=0");
#endif
		gen_1905d_config();
		gen_mapd_config();
		eval("iwpriv", (char*) WIF_2G, "set", "mapEnable=1");
		eval("iwpriv", (char*) WIF_5G, "set", "mapEnable=1");
		eval("iwpriv", (char*) WIF_2G, "set", "mapTurnKey=1");
		eval("iwpriv", (char*) WIF_5G, "set", "mapTurnKey=1");
		eval("iwpriv", bh2gifname, "set", "mapEnable=1");
		eval("iwpriv", bh5gifname, "set", "mapEnable=1");
		eval("iwpriv", bh2gifname, "set", "mapTurnKey=1");
		eval("iwpriv", bh5gifname, "set", "mapTurnKey=1");
	}

	if(mode == MAP_DISABLED){
		_dprintf("dhcp starting...\n");
		if(lastmode == 1){
			_dprintf("default dhcp server enable...\n");
			if(dhcpctl == 1 && nvram_match("dhcp_enable_x", "0")){
				nvram_set("dhcp_enable_x", "1");
				stop_dnsmasq();
				sleep(1);
				start_dnsmasq();
			}
			doSystem("ifconfig %s down", get_staifname(WL_2G_BAND));
			doSystem("ifconfig %s down", get_staifname(WL_5G_BAND));
			eval("iwpriv", (char*) WIF_2G, "set", "ApCliEnable=0");
			eval("iwpriv", (char*) WIF_5G, "set", "ApCliEnable=0");
			_dprintf("Non mesh mode");
			eval("iwpriv", (char*) WIF_2G, "set", "mapEnable=0");
			eval("iwpriv", (char*) WIF_5G, "set", "mapEnable=0");
			start_wapp();
			if(!module_loaded("mtfwd"))
				modprobe("mtfwd");
			if(pids("fwdd"))
				killall_tk("fwdd");
			_eval(fwdd_argv, NULL, 0, &pid);
		}
	} else if(mode == MAP_TURNKEY){
#if defined(RTCONFIG_RALINK_MT7621) || defined(RTCONFIG_RALINK_EN7561)
		char *lanifname = "vlan1"; //nvram_get("lan_ifname");
#else
		char *lanifname = "eth0"; //nvram_get("lan_ifname");
#endif
		_dprintf("TurnKey mode");
		gen_bhwifi_conf();
		if(pids("fwdd"))
			killall_tk("fwdd");
		if(module_loaded("mtfwd"))
			modprobe_r("mtfwd");
		if(!module_loaded("mapfilter"))
			modprobe("mapfilter");
		doSystem("ifconfig %s up", get_staifname(WL_2G_BAND));
		doSystem("ifconfig %s up", get_staifname(WL_5G_BAND));
		doSystem("ifconfig %s up", bh2gifname);
		doSystem("ifconfig %s up", bh5gifname);
		eval("iwpriv", (char*) WIF_2G, "set", "ApCliEnable=0");
		eval("iwpriv", (char*) WIF_5G, "set", "ApCliEnable=0");
		eval("iwpriv", (char*) WIF_2G, "set", "mapEnable=1");
		eval("iwpriv", (char*) WIF_5G, "set", "mapEnable=1");
		eval("iwpriv", bh2gifname, "set", "ApCliEnable=0");
		eval("iwpriv", bh5gifname, "set", "ApCliEnable=0");
		eval("iwpriv", bh2gifname, "set", "mapEnable=1");
		eval("iwpriv", bh5gifname, "set", "mapEnable=1");
#if defined(RTCONFIG_EASYMESH_R3)
		eval("iwpriv", (char*) WIF_2G, "set", "QoSR1Enable=0");
		eval("iwpriv", (char*) WIF_5G, "set", "QoSR1Enable=0");
		eval("iwpriv", bh2gifname, "set", "QoSR1Enable=0");
		eval("iwpriv", bh5gifname, "set", "QoSR1Enable=0");
#endif
		eval("brctl", "addif", "br0", bh2gifname);
		eval("brctl", "addif", "br0", bh5gifname);
#if defined(RTCONFIG_RALINK_MT7621) || defined(RTCONFIG_RALINK_MT7622) || defined(RTCONFIG_RALINK_EN7561)
//Rule 1:  P5 1905 multicast forwarding to WAN port(P4)
//step 1: enable port 5 ACL function
		doSystem("switch reg w 2504 ff0403");
//step 2:ACL pattern
//pattern 16bit
		doSystem("switch reg w 94 ffff0180");
//check MAC header OFST_TP=000   offset=0  check p5 frame
		doSystem("switch reg w 98 82000");
//function:0101 ACL rule  rule0
		doSystem("switch reg w 90 80005004");
		doSystem("switch reg w 94 ffffc200");
//check MAC header OFST_TP=000   offset=2  check p5 frame
		doSystem("switch reg w 98 82002");
//function:0101 ACL rule1
		doSystem("switch reg w 90 80005005");
		doSystem("switch reg w 94 ffff0013");
//check MAC header offset=4  check p5 frame 
		doSystem("switch reg w 98 82004");
//function:0101 ACL rule2
		doSystem("switch reg w 90 80005006");
		doSystem("switch reg w 94 ffff893A");
		doSystem("switch reg w 98 8200c");
		doSystem("switch reg w 90 80005007");
//step3: ACL mask entry
		doSystem("switch reg w 94 0xf0");
		doSystem("switch reg w 98 0");
//FUNC= 0x0101
		doSystem("switch reg w 90 80009001");
	 
//step4: ACL rule control :force forward to P4 or P1
//PORT_EN = 1 forward to P4; if forward to P1, 18000284
		doSystem("switch reg w 94 18001084");
		doSystem("switch reg w 98 0");
//func=1011   ACL rule control
		doSystem("switch reg w 90 8000B001");
	  
//Rule 2:  P0/P4 1905 multicast forwarding to P5 port
//step 1:  enable port 4 function, if you want to enable port 0. Switch reg w 2004 ff403
		doSystem("switch  reg w 2404 ff0403");

//step 2:ACL pattern
//pattern 16bit
		doSystem("switch reg w 94 ffff0180");
//check MAC header OFST_TP=000   offset=0  check p4 frame, if P0, 80100
		doSystem("switch reg w 98 81000");
//function:0101 ACL rule  rule0
		doSystem("switch reg w 90 80005000");
		doSystem("switch reg w 94 ffffc200");
//check MAC header OFST_TP=000   offset=2  check p4 frame, if P0, 80102
		doSystem("switch reg w 98 81002");
//function:0101 ACL rule1
		doSystem("switch reg w 90 80005001");
		doSystem("switch reg w 94 ffff0013");
//check MAC header offset=4  check p4 frame, if P0, 80104 
		doSystem("switch reg w 98 81004");
//function:0101 ACL rule2
		doSystem("switch reg w 90 80005002");

//step 3: ACL mask entry
		doSystem("switch reg w 94 7");
		doSystem("switch reg w 98 0");
//FUNC= 0x0101 
		doSystem("switch reg w 90 80009000");
	 
//step 4: ACL rule control :force forward to P5
//PORT_EN = 1 dp=5
		doSystem("switch reg w 94 18002084");
		doSystem("switch reg w 98 0");
//func=1011   ACL rule control
		doSystem("switch reg w 90 8000B000");
#endif
		f_write_string("/sys/kernel/debug/hnat/hnat_ppd_if", lanifname, 0, 0);
		if(nvram_match("easymesh_perportpervlan", "1")){
			char tmp[20] = {0};
			_dprintf("StartMapTurnkey perportpervlan enabled\n");
			snprintf(tmp, sizeof(tmp), "%s.1", lanifname);
			f_write_string("/sys/kernel/debug/hnat/hnat_ppd_if", tmp, 0, 0);
			perportpervlan_conf();
		}
#if defined(RTCONFIG_EASYMESH_R3)
		eval("iwpriv", (char*) WIF_2G, "set", "mapR3Enable=1");
		eval("iwpriv", (char*) WIF_2G, "set", "DppEnable=1");
		eval("iwpriv", (char*) WIF_2G, "set", "mapTSEnable=1");
		eval("iwpriv", (char*) WIF_2G, "set", "cp_support=1");
		eval("iwpriv", (char*) WIF_5G, "set", "mapR3Enable=1");
		eval("iwpriv", (char*) WIF_5G, "set", "DppEnable=1");
		eval("iwpriv", (char*) WIF_5G, "set", "mapTSEnable=1");
		eval("iwpriv", (char*) WIF_5G, "set", "cp_support=1");
		eval("iwpriv", bh2gifname, "set", "mapR3Enable=1");
		eval("iwpriv", bh5gifname, "set", "mapR3Enable=1");
		eval("iwpriv", bh2gifname, "set", "mapTSEnable=1");
		eval("iwpriv", bh5gifname, "set", "mapTSEnable=1");
		eval("iwpriv", bh2gifname, "set", "cp_support=1");
		eval("iwpriv", bh5gifname, "set", "cp_support=1");
#else
		eval("iwpriv", (char*) WIF_2G, "set", "mapR2Enable=1");
		eval("iwpriv", (char*) WIF_2G, "set", "mapTSEnable=1");
		eval("iwpriv", (char*) WIF_5G, "set", "mapR2Enable=1");
		eval("iwpriv", (char*) WIF_5G, "set", "mapTSEnable=1");
		eval("iwpriv", bh2gifname, "set", "mapR2Enable=1");
		eval("iwpriv", bh5gifname, "set", "mapR2Enable=1");
		eval("iwpriv", bh2gifname, "set", "mapTSEnable=1");
		eval("iwpriv", bh5gifname, "set", "mapTSEnable=1");
#endif
		doSystem("ulimit -c unlimited");
//Controller
		_dprintf("dhcp starting...\n");
		if(dhcpctl == 1){
			if(tpcon == 1 && role == EASYMESH_ROLE_MASTER){
				nvram_set("dhcp_enable_x", "0");
				stop_dnsmasq();
				sleep(1);
				start_dnsmasq();
			}else{
				nvram_set("dhcp_enable_x", "1");
				stop_dnsmasq();
				sleep(1);
				start_dnsmasq();
			}
		}
		setup_wifi_vlan();
		start_wapp();
		if(role == EASYMESH_ROLE_MASTER)
			_eval(p1905_argv, "/var/log/1905.log", 0, &pid);
		else{
			p1905_argv[1] = "-r1";
			_eval(p1905_argv, "/var/log/1905.log", 0, &pid);
		}
		sleep(1);
		if(nvram_match("easymesh_debug", "1")){
			mapd_argv[7] = "-dddd";
			_eval(mapd_argv, "/var/log/log.mapd", 0, &pid);
		}else
			_eval(mapd_argv, NULL, 0, &pid);
#if defined(RTCONFIG_RALINK_MT7621) || defined(RTCONFIG_RALINK_MT7622) || defined(RTCONFIG_RALINK_EN7561)
		doSystem("switch reg w 10 ffffffe0");
		doSystem("switch reg w 34 8160816");
#endif
		nvram_set("easymesh_lastmode", "1");
	} else if(mode == MAP_BS_2_0){
		if(module_loaded("mapfilter"))
			modprobe_r("mapfilter");
		_dprintf("BS2.0 mode\n");
		eval("iwpriv", (char*) WIF_2G, "set", "mapEnable=2");
		eval("iwpriv", (char*) WIF_5G, "set", "mapEnable=2");
		start_wapp();
		_dprintf("BS2.0 Daemon starting...\n");
		doSystem("bs20 &");
		eval("iwpriv", (char*) WIF_2G, "set", "DisConnectAllSta=");
		eval("iwpriv", (char*) WIF_5G, "set", "DisConnectAllSta=");
		_dprintf("Stand Alone BS2.0 is ready\n");
		if(!module_loaded("mtfwd"))
			modprobe("mtfwd");
		if(pids("fwdd"))
			killall_tk("fwdd");
		_eval(fwdd_argv, NULL, 0, &pid);
	} else if(mode == MAP_API_MODE){
		_dprintf("unsupport mode\n");
	} else if(mode == MAP_CERT_MODE){
		pid_t pid;
		char *agent_argv[] = { "easymesh_agent", NULL };
		if(pids("fwdd"))
				killall_tk("fwdd");
		if(module_loaded("mtfwd"))
			modprobe_r("mtfwd");
		_dprintf("Certification\n");
// GMAC1 need to config swith vlan
// platform mt7621(only for certification mode use)
#if defined(RTCONFIG_RALINK_MT7621) || defined(RTCONFIG_RALINK_EN7561)
		_dprintf("mapmode is certification mode, config switch\n");
		//init_switch();
		doSystem("switch reg w 34 8160816");
		doSystem("switch reg w 4 60");
		doSystem("switch reg w 10 ffffffff");
#endif
		if(!module_loaded("mapfilter"))
			modprobe("mapfilter");
		doSystem("ifconfig %s up", get_staifname(WL_2G_BAND));
		doSystem("ifconfig %s up", get_staifname(WL_5G_BAND));
		eval("iwpriv", (char*) APCLI_2G, "set", "mapEnable=4");
		eval("iwpriv", (char*) APCLI_5G, "set", "mapEnable=4");
		eval("iwpriv", (char*) APCLI_2G, "set", "ApCliEnable=0");
		eval("iwpriv", (char*) APCLI_5G, "set", "ApCliEnable=0");
		_eval(agent_argv, NULL, 0, &pid);
		doSystem("echo 458752 > /proc/sys/net/core/rmem_max");
		eval("iwpriv", (char*) WIF_2G, "set", "VLANEn=0");
		eval("iwpriv", (char*) WIF_5G, "set", "VLANEn=0");
#if defined(RTCONFIG_EASYMESH_R3)
		eval("iwpriv", (char*) WIF_2G, "set", "mapR3Enable=1");
		eval("iwpriv", (char*) WIF_2G, "set", "DppEnable=1");
		eval("iwpriv", (char*) WIF_2G, "set", "mapTSEnable=1");
		eval("iwpriv", (char*) WIF_2G, "set", "cp_support=1");
		eval("iwpriv", (char*) WIF_5G, "set", "mapR3Enable=1");
		eval("iwpriv", (char*) WIF_5G, "set", "DppEnable=1");
		eval("iwpriv", (char*) WIF_5G, "set", "mapTSEnable=1");
		eval("iwpriv", (char*) WIF_5G, "set", "cp_support=1");
#else
		eval("iwpriv", (char*) WIF_2G, "set", "mapR2Enable=1");
		eval("iwpriv", (char*) WIF_2G, "set", "mapTSEnable=1");
		eval("iwpriv", (char*) WIF_5G, "set", "mapR2Enable=1");
		eval("iwpriv", (char*) WIF_5G, "set", "mapTSEnable=1");
#endif
	}
}

int start_easymesh(void)
{
	pid_t pid;
	char *agent_argv[] = { "easymesh_agent", NULL };
	if (getpid() != 1) {
		notify_rc("start_easymesh");
		return 0;
	}
	swrtmesh_autoconf();
	start_mapd();
	if(nvram_match("easymesh_enable", "1") && nvram_get_int("easymesh_role") == EASYMESH_ROLE_AGENT)
		_eval(agent_argv, NULL, 0, &pid);
	return 0;
}

void stop_easymesh(void)
{
	if (getpid() != 1) {
		notify_rc("stop_easymesh");
		return;
	}

	if (pids("mapd"))
		killall_tk("mapd");
	if (pids("p1905_managerd"))
		killall_tk("p1905_managerd");
	doSystem("ifconfig %s down", get_staifname(WL_2G_BAND));
	doSystem("ifconfig %s down", get_staifname(WL_5G_BAND));
	doSystem("ifconfig %s down", get_mesh_bh_ifname(WL_2G_BAND));
	doSystem("ifconfig %s down", get_mesh_bh_ifname(WL_5G_BAND));
}

void easymesh_agent(void)
{

}

#endif

