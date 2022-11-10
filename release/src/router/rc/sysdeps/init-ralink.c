/*
 * Copyright 2021, ASUS
 * Copyright 2021-2022, SWRTdev
 * Copyright 2021-2022, paldier <paldier@hotmail.com>.
 * Copyright 2021-2022, lostlonger<lostlonger.g@gmail.com>.
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
#include <swrtmesh.h>

void init_devs(void)
{
#define MKNOD(name,mode,dev)	if(mknod(name,mode,dev)) perror("## mknod " name)
	MKNOD("/dev/video0", S_IFCHR | 0666, makedev(81, 0));
	MKNOD("/dev/spiS0", S_IFCHR | 0666, makedev(217, 0));
	MKNOD("/dev/i2cM0", S_IFCHR | 0666, makedev(218, 0));
#if defined(RTCONFIG_RALINK_MT7621)
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
	patch_Factory();
}

void init_others(void)
{
#if defined(RTAX53U)
//mii_mgr -s -p [phy number] -r [register number] -v [0xvalue]
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
			nvram_unset("vlan3hwname");
			if ((wans_cap && wanslan_cap) ||
			    (wanslan_cap && (!nvram_match("switch_wantag", "none") && !nvram_match("switch_wantag", "")))
			   )
				nvram_set("vlan3hwname", "et0");
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
	if(strlen(nvram_safe_get("wan0_ifname"))) {
		if (!nvram_match("et1macaddr", ""))
			eval("ifconfig", nvram_safe_get("wan0_ifname"), "hw", "ether", nvram_safe_get("et1macaddr"));
		else
			eval("ifconfig", nvram_safe_get("wan0_ifname"), "hw", "ether", nvram_safe_get("et0macaddr"));
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

int config_switch_for_first_time = 1;
#if defined(RTCONFIG_WLMODULE_MT7915D_AP)
int lan_port_bit_shift = 1;
#else
int lan_port_bit_shift = 0;
#endif
void config_switch()
{
	int model = get_model();
	int stbport;
	int controlrate_unknown_unicast;
	int controlrate_unknown_multicast;
	int controlrate_multicast;
	int controlrate_broadcast;
	int merge_wan_port_into_lan_ports;

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
	system("rtkswitch 8 0"); //Barton add

	if (is_routing_enabled())
	{
		char parm_buf[] = "XXX";

		stbport = nvram_get_int("switch_stb_x");
		if (stbport < 0 || stbport > 6) stbport = 0;
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
			int voip_port = 0;
			int t, vlan_val = -1, prio_val = -1;
			unsigned int mask = 0;

#if defined(RTCONFIG_RALINK_MT7628)
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
				stbport = 4;	
				voip_port = 3;

				if(!strncmp(nvram_safe_get("switch_wantag"), "unifi", 5)) {
					/* Added for Unifi. Cherry Cho modified in 2011/6/28.*/
					if(strstr(nvram_safe_get("switch_wantag"), "home")) {
						//system("rtkswitch 38 1");		/* IPTV: P0 */
						snprintf(parm_buf, sizeof(parm_buf), "%d", 1 << lan_port_bit_shift);
						eval("rtkswitch", "38", parm_buf);
						/* Internet:	untag: P9;   port: P4, P9 */
						__setup_vlan(500, 0, 0x02000210);
						/* IPTV:	untag: P0;   port: P0, P4 */
						__setup_vlan(600, 0, 0x00010011);
					}
					else {
						/* No IPTV. Business package */
						/* Internet:	untag: P9;   port: P4, P9 */
						system("rtkswitch 38 0");
						__setup_vlan(500, 0, 0x02000210);
					}
				}
				else if(!strncmp(nvram_safe_get("switch_wantag"), "singtel", 7)) {
					/* Added for SingTel's exStream issues. Cherry Cho modified in 2011/7/19. */
					if(strstr(nvram_safe_get("switch_wantag"), "mio")) {
						/* Connect Singtel MIO box to P3 */
						system("rtkswitch 40 1");		/* admin all frames on all ports */
						//system("rtkswitch 38 3");		/* IPTV: P0  VoIP: P1 */
						snprintf(parm_buf, sizeof(parm_buf), "%d", 3 << lan_port_bit_shift);
						eval("rtkswitch", "38", parm_buf);
						/* Internet:	untag: P9;   port: P4, P9 */
						__setup_vlan(10, 0, 0x02000210);
						/* VoIP:	untag: N/A;  port: P1, P4 */
						//VoIP Port: P1 tag
						//__setup_vlan(30, 4, 0x00000012);
						__setup_vlan(30, 4, ((2 << lan_port_bit_shift) | 0x10));
					}
					else {
						//Connect user's own ATA to lan port and use VoIP by Singtel WAN side VoIP gateway at voip.singtel.com
						//system("rtkswitch 38 1");		/* IPTV: P0 */
						snprintf(parm_buf, sizeof(parm_buf), "%d", 1 << lan_port_bit_shift);
						eval("rtkswitch", "38", parm_buf);
						/* Internet:	untag: P9;   port: P4, P9 */
						__setup_vlan(10, 0, 0x02000210);
					}
					if(strstr(nvram_safe_get("switch_wantag"), "mstb"))
						__setup_vlan(20, 4, ((0x30003 << lan_port_bit_shift) | 0x10));		/* untag: P0;   port: P0, P4 */
					else
						__setup_vlan(20, 4, ((0x10001 << lan_port_bit_shift) | 0x10));		/* untag: P0;   port: P0, P4 */

				}
				else if(!strcmp(nvram_safe_get("switch_wantag"), "m1_fiber")) {
					//VoIP: P1 tag. Cherry Cho added in 2012/1/13.
					system("rtkswitch 40 1");			/* admin all frames on all ports */
					//system("rtkswitch 38 2");			/* VoIP: P1  2 = 0x10 */
					snprintf(parm_buf, sizeof(parm_buf), "%d", 2 << lan_port_bit_shift);
					eval("rtkswitch", "38", parm_buf);
					/* Internet:	untag: P9;   port: P4, P9 */
					__setup_vlan(1103, 1, 0x02000210);
					/* VoIP:	untag: N/A;  port: P1, P4 */
					//VoIP Port: P1 tag
					//__setup_vlan(1107, 1, 0x00000012);
					__setup_vlan(1107, 1, ((2 << lan_port_bit_shift) | 0x10));
				}
				else if(!strcmp(nvram_safe_get("switch_wantag"), "maxis_fiber")) {
					//VoIP: P1 tag. Cherry Cho added in 2012/11/6.
					system("rtkswitch 40 1");			/* admin all frames on all ports */
					//system("rtkswitch 38 2");			/* VoIP: P1  2 = 0x10 */
					snprintf(parm_buf, sizeof(parm_buf), "%d", 2 << lan_port_bit_shift);
					eval("rtkswitch", "38", parm_buf);
					/* Internet:	untag: P9;   port: P4, P9 */
					__setup_vlan(621, 0, 0x02000210);
					/* VoIP:	untag: N/A;  port: P1, P4 */
					__setup_vlan(821, 0, ((2 << lan_port_bit_shift) | 0x10));

					__setup_vlan(822, 0, ((2 << lan_port_bit_shift) | 0x10));		/* untag: N/A;  port: P1, P4 */ //VoIP Port: P1 tag
				}
				else if(!strcmp(nvram_safe_get("switch_wantag"), "maxis_fiber_sp")) {
					//VoIP: P1 tag. Cherry Cho added in 2012/11/6.
					system("rtkswitch 40 1");			/* admin all frames on all ports */
					//system("rtkswitch 38 2");			/* VoIP: P1  2 = 0x10 */
					snprintf(parm_buf, sizeof(parm_buf), "%d", 2 << lan_port_bit_shift);
					eval("rtkswitch", "38", parm_buf);
					/* Internet:	untag: P9;   port: P4, P9 */
					__setup_vlan(11, 0, 0x02000210);
					/* VoIP:	untag: N/A;  port: P1, P4 */
					//VoIP Port: P1 tag
					//__setup_vlan(14, 0, 0x00000012);
					__setup_vlan(14, 0, ((2 << lan_port_bit_shift) | 0x10));
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
					system("rtkswitch 38 0");
#if defined(RTCONFIG_RALINK_MT7620) || defined(RTCONFIG_RALINK_MT7621)
					__setup_vlan(2, 0, 0x02100210);
#endif
				}
#endif
				else if (!strcmp(nvram_safe_get("switch_wantag"), "meo")) {
					system("rtkswitch 40 1");			/* admin all frames on all ports */
					//system("rtkswitch 38 1");			/* VoIP: P0 */
					snprintf(parm_buf, sizeof(parm_buf), "%d", 1 << lan_port_bit_shift);
					eval("rtkswitch", "38", parm_buf);
					/* Internet/VoIP:	untag: P9;   port: P0, P4, P9 */
					//__setup_vlan(12, 0, 0x02000211);
					__setup_vlan(12, 0, ((1 << lan_port_bit_shift) | 0x2000210));
				}
				else if (!strcmp(nvram_safe_get("switch_wantag"), "vodafone")) {
					system("rtkswitch 40 1");			/* admin all frames on all ports */
					//system("rtkswitch 38 3");			/* Vodafone: P0  IPTV: P1 */
					snprintf(parm_buf, sizeof(parm_buf), "%d", 3 << lan_port_bit_shift);
					eval("rtkswitch", "38", parm_buf);
					/* Internet:	untag: P9;   port: P4, P9 */
					//__setup_vlan(100, 1, 0x02000211);
					__setup_vlan(100, 1, ((1 << lan_port_bit_shift) | 0x2000210));
					/* IPTV:	untag: N/A;  port: P0, P4 */
					//__setup_vlan(101, 0, 0x00000011);
					__setup_vlan(101, 0, ((1 << lan_port_bit_shift) | 0x10));
					/* Vodafone:	untag: P1;   port: P0, P1, P4 */
					//__setup_vlan(105, 1, 0x00020013);
					__setup_vlan(105, 1, ((0x20003 << lan_port_bit_shift) | 0x10));
				}
				else if (!strcmp(nvram_safe_get("switch_wantag"), "hinet")) { /* Hinet MOD */
#if defined(RTAX53U)
					eval("rtkswitch", "8", "3");			/* LAN4 with WAN */
#else
					eval("rtkswitch", "8", "4");			/* LAN4 with WAN */
#endif
				}
				else {
					/* Cherry Cho added in 2011/7/11. */
					/* Initialize VLAN and set Port Isolation */
					if(strcmp(nvram_safe_get("switch_wan1tagid"), "") && strcmp(nvram_safe_get("switch_wan2tagid"), "")){
						//system("rtkswitch 38 3");		// 3 = 0x11 IPTV: P0  VoIP: P1
						snprintf(parm_buf, sizeof(parm_buf), "%d", 3 << lan_port_bit_shift);
						eval("rtkswitch", "38", parm_buf);
					}else if(strcmp(nvram_safe_get("switch_wan1tagid"), "")){
						//system("rtkswitch 38 1");		// 1 = 0x01 IPTV: P0
						snprintf(parm_buf, sizeof(parm_buf), "%d", 1 << lan_port_bit_shift);
						eval("rtkswitch", "38", parm_buf);
					}else if(strcmp(nvram_safe_get("switch_wan2tagid"), "")){
						//system("rtkswitch 38 2");		// 2 = 0x10 VoIP: P1
						snprintf(parm_buf, sizeof(parm_buf), "%d", 2 << lan_port_bit_shift);
						eval("rtkswitch", "38", parm_buf);
					}else
						system("rtkswitch 38 0");		//No IPTV and VoIP ports

					/*++ Get and set Vlan Information */
					t = nvram_get_int("switch_wan0tagid") & 0x0fff;
					if (t != 0) {
						// Internet on WAN (port 4)
						if (t >= 2 && t <= 4094)
							vlan_val = t;

						prio_val = nvram_get_int("switch_wan0prio") & 0x7;

						__setup_vlan(vlan_val, prio_val, 0x02000210);
					}
#if defined(RTCONFIG_RALINK_MT7621)
					else {
						/* Internet: untag: P4, P9; port: P4, P9 */
						__setup_vlan(2, 0, 0x02100210);
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
						//__setup_vlan(vlan_val, prio_val, mask);
						__setup_vlan(vlan_val, prio_val, (((mask & 0xf000f) << lan_port_bit_shift) | 0x10));
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

						//__setup_vlan(vlan_val, prio_val, mask);
						__setup_vlan(vlan_val, prio_val, (((mask & 0xf000f) << lan_port_bit_shift) | 0x10));
					}
				}
			}
		}
		else
		{
			sprintf(parm_buf, "%d", stbport);
			if (stbport)
				eval("rtkswitch", "8", parm_buf);
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
		snprintf(cmd, sizeof(cmd), "dd if=/dev/mtdblock%d of=/lib/firmware/e2p bs=655360 skip=0 count=1", mtd_part);
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

void fini_wl(void)
{
#if 0 //3.10.x
	if (module_loaded("hw_nat")) {
#if defined (RTCONFIG_WLMODULE_MT7615E_AP)
		doSystem("iwpriv %s set hw_nat_register=%d", get_wifname(0), 0);
#ifdef RTCONFIG_HAS_5G
		doSystem("iwpriv %s set hw_nat_register=%d", get_wifname(1), 0);
#endif
#endif
		modprobe_r("hw_nat");
	}
#else //4.4.x
	if (module_loaded("mtkhnat"))
		modprobe_r("mtkhnat");
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

void init_syspara(void)
{
	unsigned char buffer[16];
	unsigned char *dst;
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
	char ea[ETHER_ADDR_LEN];
	const char *reg_spec_def;

	char brstp;
	char value_str[MAX_REGSPEC_LEN+1];
	memset(value_str, 0, sizeof(value_str));

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
#if defined(JCGQ10PRO) || defined(H3CTX1801) || defined(PGBM1)
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

	if (!mssid_mac_validate(macaddr) || !mssid_mac_validate(macaddr2))
		nvram_set("wl_mssid", "0");
	else
		nvram_set("wl_mssid", "1");
#if defined(R6800) || defined(RMAC2100) || defined(JCGQ10PRO) || defined(H3CTX1801) || defined(PGBM1)
	nvram_set("wl_mssid", "1");//fix guest wifi
#endif

	//TODO: separate for different chipset solution
	nvram_set("et0macaddr", macaddr);
	nvram_set("et1macaddr", macaddr2);
#if defined(RTCONFIG_EASYMESH)
	ether_cal(macaddr, macaddrbh1, 14);
	ether_cal(macaddr2, macaddrbh2, 14);
	nvram_set("bh0macaddr", macaddr);
	nvram_set("bh1macaddr", macaddr2);
#endif

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
#if defined(RTCONFIG_RALINK_MT7621)
	if (FRead(dst, OFFSET_MAC_GMAC2, bytes)<0)
		dbg("READ MAC address GMAC2: Out of scope\n");
#elif defined(RTCONFIG_MT798X)
	if (FRead(dst, OFFSET_MAC_GMAC1, bytes)<0)
		dbg("READ MAC address GMAC1: Out of scope\n");
#endif
	else
	{
		if (buffer[0]==0xff)
		{
			if (ether_atoe(macaddr2, ea))
#if defined(RTCONFIG_RALINK_MT7621)
				FWrite(ea, OFFSET_MAC_GMAC2, 6);
#elif defined(RTCONFIG_MT798X)
				FWrite(ea, OFFSET_MAC_GMAC1, 6);
#endif
		}
	}
	{
#ifdef RTCONFIG_ODMPID
		char modelname[16];
		FRead(modelname, OFFSET_ODMPID, sizeof(modelname));
		modelname[sizeof(modelname)-1] = '\0';
		if(modelname[0] != 0 && (unsigned char)(modelname[0]) != 0xff && is_valid_hostname(modelname) && strcmp(modelname, "ASUS"))
		{
			nvram_set("odmpid", modelname);
		}
		else
#endif
			nvram_unset("odmpid");
	}

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

	reg_spec_def = "FCC";
	bytes = MAX_REGSPEC_LEN;
	memset(dst, 0, MAX_REGSPEC_LEN+1);

	if(FRead(dst, REGSPEC_ADDR, bytes) < 0)
		nvram_set("reg_spec", reg_spec_def); // DEFAULT
	else
	{
		for (i=(MAX_REGSPEC_LEN-1);i>=0;i--) {
			if ((dst[i]==0xff) || (dst[i]=='\0'))
				dst[i]='\0';
		}
		if (dst[0]!=0x00)
			nvram_set("reg_spec", dst);
		else
			nvram_set("reg_spec", reg_spec_def); // DEFAULT
	}

	if (FRead(dst, REG2G_EEPROM_ADDR, MAX_REGDOMAIN_LEN)<0 || memcmp(dst,"2G_CH", 5) != 0)
	{
		_dprintf("Read REG2G_EEPROM_ADDR fail or invalid value\n");
		nvram_set("wl_country_code", "");
		nvram_set("wl0_country_code", "DB");
		nvram_set("wl_reg_2g", "2G_CH14");
	}
	else
	{
		for(i = 0; i < MAX_REGDOMAIN_LEN; i++)
			if(dst[i] == 0xff || dst[i] == 0)
				break;

		dst[i] = 0;
		nvram_set("wl_reg_2g", dst);
		if (strcmp(dst, "2G_CH11") == 0)
			nvram_set("wl0_country_code", "US");
		else if (strcmp(dst, "2G_CH13") == 0)
			nvram_set("wl0_country_code", "GB");
#if defined(RTCONFIG_WLMODULE_MT7615E_AP)
		else if (strcmp(dst, "2G_CH14") == 0)
			nvram_set("wl0_country_code", "DB");
#endif
		else
			nvram_set("wl0_country_code", "DB");
	}
#ifdef RTCONFIG_HAS_5G
	if (FRead(dst, REG5G_EEPROM_ADDR, MAX_REGDOMAIN_LEN)<0 || memcmp(dst,"5G_", 3) != 0)
	{
		_dprintf("Read REG5G_EEPROM_ADDR fail or invalid value\n");
		nvram_set("wl_country_code", "");
		nvram_set("wl1_country_code", "DB");
		nvram_set("wl_reg_5g", "5G_ALL");
	}
	else
	{
		for(i = 0; i < MAX_REGDOMAIN_LEN; i++)
			if(dst[i] == 0xff || dst[i] == 0)
				break;

		dst[i] = 0;
		nvram_set("wl_reg_5g", dst);
		nvram_set("wl1_IEEE80211H", "0");
		if (strcmp(dst, "5G_BAND1") == 0)
			nvram_set("wl1_country_code", "GB");
		else if (strcmp(dst, "5G_BAND123") == 0)
		{
			nvram_set("wl1_country_code", "GB");
#ifdef RTCONFIG_RALINK_DFS
			nvram_set("wl1_IEEE80211H", "1");
#endif	/* RTCONFIG_RALINK_DFS */
		}
		else if (strcmp(dst, "5G_BAND14") == 0)
			nvram_set("wl1_country_code", "US");
		else if (strcmp(dst, "5G_BAND12") == 0)
			nvram_set("wl1_country_code", "IL");
		else if (strcmp(dst, "5G_BAND24") == 0)
			nvram_set("wl1_country_code", "TW");
		else if (strcmp(dst, "5G_BAND4") == 0)
			nvram_set("wl1_country_code", "CN");
		else if (strcmp(dst, "5G_BAND124") == 0)
			nvram_set("wl1_country_code", "IN");
#if defined(RTCONFIG_WLMODULE_MT7615E_AP)
		else if (strcmp(dst, "5G_BAND12") == 0)	{
			nvram_set("wl1_country_code", "IL");
#ifdef RTCONFIG_RALINK_DFS
			nvram_set("wl1_IEEE80211H", "1");
#endif	/* RTCONFIG_RALINK_DFS */
		}
#endif
		else if (strcmp(dst, "5G_ALL") == 0)	{
#if defined(RTAC85P)
			nvram_set("wl1_country_code", "RU");
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
#endif	/* RTCONFIG_HAS_5G */
#endif	/* ! RTCONFIG_NEW_REGULATION_DOMAIN */
#if defined(RTCONFIG_DSL)
		if (nvram_match("wl_country_code", "BR"))
		{
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
		if ((unsigned char)pin[0]!=0xff)
			nvram_set("secret_code", pin);
		else
			nvram_set("secret_code", "12345670");
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
	/* Territory code */
	memset(buffer, 0, sizeof(buffer));

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

#if defined(RTCONFIG_WLMODULE_MT7915D_AP) || defined(RTCONFIG_MT798X)
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

	nvram_set("firmver", rt_version);
	nvram_set("productid", rt_buildname);

	_dprintf("odmpid: %s\n", nvram_safe_get("odmpid"));
	_dprintf("current FW productid: %s\n", nvram_safe_get("productid"));
	_dprintf("current FW firmver: %s\n", nvram_safe_get("firmver"));
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
#if defined(RTCONFIG_DUALWAN)
	int nat_x = -1, i, l, t, link_wan = 1, link_wans_lan = 1;
	int wans_cap = get_wans_dualwan() & WANSCAP_WAN;
	int wanslan_cap = get_wans_dualwan() & WANSCAP_LAN;
	char nat_x_str[] = "wanX_nat_xXXXXXX";
#endif
	if (!nvram_get_int("hwnat"))
		return;

	if(!is_wan_connect(prim_unit))
		return;

	/* If QoS is enabled, disable hwnat. */
	if (nvram_get_int("qos_enable") == 1)
		act = 0;

	if (act > 0 && !nvram_match("switch_wantag", "none") && !nvram_match("switch_wantag", ""))
		act = 0;

	if (act > 0) {
#if defined(RTCONFIG_DUALWAN)
		if (unit < 0 || unit > WAN_UNIT_SECOND) {
			if ((wans_cap && wanslan_cap) ||
			    (wanslan_cap && (!nvram_match("switch_wantag", "none") && !nvram_match("switch_wantag", "")))
			   )
				act = 0;
		} else {
			sprintf(nat_x_str, "wan%d_nat_x", unit);
			nat_x = nvram_get_int(nat_x_str);
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
		if (!is_nat_enabled())
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

#if defined(RTCONFIG_DUALWAN)
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
		nvram_get_int("wan0_nat_x"), nvram_get_int("qos_enable"), act);
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
					eval("vconfig", "set_egress_map", wan_dev, "0", (char *)voip_prio);
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
					eval("vconfig", "set_egress_map", wan_dev, "0", (char *)iptv_prio);
				}
			}
		}
	}
#endif
}

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
#if defined(RTCONFIG_MT798X)
	eval("/sbin/smp.sh");
#else
	if(which == 1)
		eval("/sbin/smp.sh", "usb0", NULL);
	else
		eval("/sbin/smp.sh", "wifi", NULL);
#endif
	//nas or auto
	//doSystem("pbr-optimizer -m 1 -o &");
	//nat
	//doSystem("pbr-optimizer -m 2 -o &");
	//wifi
	//doSystem("pbr-optimizer -m 3 -o &");
}

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
#endif
#if defined(RTCONFIG_EASYMESH)
#define MAP_DISABLED		0
#define MAP_TURNKEY			1
#define MAP_BS_2_0			2
#define MAP_API_MODE		3
#define MAP_CERT_MODE		4
void gen_1905d_config(void)
{
	FILE *fp = NULL;
	system("mkdir -p /etc/map");
	if((fp = fopen("/etc/map/1905d.cfg", "w")))
	{
// Has MAP agent on this device
		fprintf(fp, "map_agent=1\n");
		if(nvram_get_int("easymesh_role") == EASYMESH_ROLE_MASTER){
// This device is a MAP root
			fprintf(fp, "map_root=1\n");
		}else{
			fprintf(fp, "map_root=0\n");
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
#if defined(RTCONFIG_RALINK_MT7621)
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
		}else{
#if defined(RTCONFIG_RALINK_MT7621)
			fprintf(fp, "lan=vlan1\n");
#else
			fprintf(fp, "lan=eth0\n");
#endif
			fprintf(fp, "lan_vid=1;\n");
			fprintf(fp, "wan_vid=2;\n");
		}
//wan interface
		fprintf(fp, "wan=%s\n", nvram_safe_get("wan0_ifname"));
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
		}
		fclose(fp);
	}else
		_dprintf("Can't open /etc/map/1905d.cfg\n");
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
	FILE *fp = NULL;
	if((fp = fopen("/etc/map/mapd_default.cfg", "w")))
	{
		if(get_easymesh_max_ver() > 2)
			fprintf(fp, "mode=1\n");
		fprintf(fp, "lan_interface=br0\n");
		fprintf(fp, "wan_interface=%s\n", nvram_safe_get("wan0_ifname"));
		if(nvram_get_int("easymesh_role") == EASYMESH_ROLE_AGENT)
			fprintf(fp, "DeviceRole=2\n");
		else if(nvram_get_int("easymesh_role") == EASYMESH_ROLE_MASTER)
			fprintf(fp, "DeviceRole=1\n");
		else//auto
			fprintf(fp, "DeviceRole=0\n");
		fprintf(fp, "APSteerRssiTh=-54\n");
		fprintf(fp, "BhPriority2G=3\n");
		fprintf(fp, "BhPriority5GL=1\n");
		fprintf(fp, "BhPriority5GH=1\n");
#if defined(RTCONFIG_WIFI6E)
		fprintf(fp, "BhPriority6G=1\n");
#endif
		fprintf(fp, "ChPlanningIdleByteCount=\n");
		fprintf(fp, "ChPlanningIdleTime=\n");
		fprintf(fp, "ChPlanningUserPreferredChannel5G=\n");
		fprintf(fp, "ChPlanningUserPreferredChannel5GH=\n");
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
			fprintf(fp, "%s;%s;", WIF_5G2, APCLI_5G2);
#endif
		if(nvram_match("wl1_radio", "1"))
			fprintf(fp, "%s;%s;", WIF_5G, APCLI_5G);
		if(nvram_match("wl0_radio", "1"))
			fprintf(fp, "%s;%s;", WIF_2G, APCLI_2G);
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
		system("ln -sf /etc/map/mapd_default.cfg /etc/map/mapd_cfg");
	}else
		_dprintf("Can't open /etc/map/mapd_default.cfg\n");
	if((fp = fopen("/etc/map/mapd_user.cfg", "w")))
	{
		fprintf(fp, "###!UserConfigs!!!\n");
		fclose(fp);
	}else
		_dprintf("Can't open /etc/map/mapd_user.cfg\n");
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

char *getauthmode(int band)
{
	char prefix[] = "wlx_xxxxxxxxxxxxxxxx";
	char *authmode = NULL;
	if(band > WL_5G_2_BAND)
		snprintf(prefix, sizeof(prefix), "wl%d.1_auth_mode_x", WL_5GBH_BAND ? 1 : 0);
	else
		snprintf(prefix, sizeof(prefix), "wl%d_auth_mode_x", band);
	authmode = nvram_safe_get(prefix);
	if(!strcmp(authmode, "open"))
		return "0x0001";
	else if(!strcmp(authmode, "psk"))
		return "0x0002";
	else if(!strcmp(authmode, "psk2"))
		return "0x0020";
	else if(!strcmp(authmode, "pskpsk2"))
		return "0x0022";
	else if(!strcmp(authmode, "sae"))
		return "0x0040";
	else if(!strcmp(authmode, "psk2sae"))
		return "0x0060";
#if defined(RTCONFIG_EASYMESH_R3)
	else if(!strcmp(authmode, "dpp"))//dpp only
		return "0x0080";
	else if(!strcmp(authmode, "dpppsk2"))//dpp + psk2
		return "0x00A0";
	else if(!strcmp(authmode, "dppsae"))//dpp + sae
		return "0x00C0";
	else if(!strcmp(authmode, "dpppsk2sae"))//dpp + psk2sae
		return "0x00E0";
#endif
	else//bug
		return "0x0020";
}

char *getencrypttype(int band, int isbh)
{
	char prefix[] = "wlx_xxxxxxxxxxxxxxxx";
	char *authmode = NULL, *wep = NULL, *crypto = NULL;
	if(band > WL_5G_2_BAND)
		snprintf(prefix, sizeof(prefix), "wl%d.1_auth_mode_x", WL_5GBH_BAND ? 1 : 0);
	else
		snprintf(prefix, sizeof(prefix), "wl%d_auth_mode_x", band);
	authmode = nvram_safe_get(prefix);
	if(band > WL_5G_2_BAND)
		snprintf(prefix, sizeof(prefix), "wl%d.1_wep_x", WL_5GBH_BAND ? 1 : 0);
	else
		snprintf(prefix, sizeof(prefix), "wl%d_wep_x", band);
	wep = nvram_safe_get(prefix);
	if(band > WL_5G_2_BAND)
		snprintf(prefix, sizeof(prefix), "wl%d.1_crypto", WL_5GBH_BAND ? 1 : 0);
	else
		snprintf(prefix, sizeof(prefix), "wl%d_crypto", band);
	crypto = nvram_safe_get(prefix);
	if(!strcmp(authmode, "open") && nvram_match(wep, "0"))
		return "0x0001";
	else if(!strcmp(crypto, "aes"))
		return "0x0008";
	else if(!strcmp(authmode, "sae"))
		return "0x0008";
	else if(!strcmp(authmode, "psk2sae"))
		return "0x0008";
#if !defined(RTCONFIG_EASYMESH_R3)
	else if(!strcmp(authmode, "psk") && !strcmp(crypto, "tkip") && isbh == 0)
		return "0x0004";
	else if(!strcmp(crypto, "tkip+aes"))
		return "0x000c";
#else
	else if(!strcmp(authmode, "dpp") && isbh == 1)
		return "0x0008";
	else if(!strcmp(authmode, "dpppsk2") && isbh == 1)
		return "0x0008";
	else if(!strcmp(authmode, "dppsae") && isbh == 1)
		return "0x0008";
	else if(!strcmp(authmode, "dpppsk2sae") && isbh == 1)
		return "0x0008";
#endif
	else//bug
		return "0x0008";
}

char *getpsk(int band)
{
	char prefix[] = "wlx_xxxxxxxxxxxxxxxx";
	if(band > WL_5G_2_BAND)
		snprintf(prefix, sizeof(prefix), "wl%d.1_wpa_psk", WL_5GBH_BAND ? 1 : 0);
	else
		snprintf(prefix, sizeof(prefix), "wl%d_wpa_psk", band);
	return nvram_safe_get(prefix);

}

void gen_bhwifi_conf()
{
	int index = 1;
	FILE *fp = NULL;
	if((fp = fopen("/etc/map/wts_bss_info_config", "w"))){
		fprintf(fp, "#ucc_bss_info\n");
		fprintf(fp, "%d,%s 8x %s %s %s %s 1 1 hidden-N\n", index, nvram_safe_get("wl0_hwaddr"), nvram_safe_get("wl0_ssid"), getauthmode(WL_2G_BAND), getencrypttype(WL_2G_BAND, 0), getpsk(WL_2G_BAND));
		index++;
		fprintf(fp, "%d,ff:ff:ff:ff:ff:ff 8x %s %s %s %s 1 0 hidden-Y\n", index, nvram_safe_get("wl0.4_ssid"), getauthmode(WL_2GBH_BAND), getencrypttype(WL_2GBH_BAND, 0), getpsk(WL_2GBH_BAND));
		index++;
		fprintf(fp, "%d,%s 11x %s %s %s %s 1 1 hidden-N\n", index, nvram_safe_get("wl1_hwaddr"), nvram_safe_get("wl1_ssid"), getauthmode(WL_5G_BAND), getencrypttype(WL_5G_BAND, 0), getpsk(WL_5G_BAND));
		index++;
		fprintf(fp, "%d,%s 12x %s %s %s %s 1 1 hidden-N\n", index, nvram_safe_get("wl1_hwaddr"), nvram_safe_get("wl1_ssid"), getauthmode(WL_5G_BAND), getencrypttype(WL_5G_BAND, 0), getpsk(WL_5G_BAND));
		index++;
		fprintf(fp, "%d,ff:ff:ff:ff:ff:ff 11x %s %s %s %s 1 0 hidden-Y\n", index, nvram_safe_get("wl1.4_ssid"), getauthmode(WL_5GBH_BAND), getencrypttype(WL_5GBH_BAND, 0), getpsk(WL_5GBH_BAND));
		index++;
		fprintf(fp, "%d,ff:ff:ff:ff:ff:ff 12x %s %s %s %s 1 0 hidden-Y\n", index, nvram_safe_get("wl1.4_ssid"), getauthmode(WL_5GBH_BAND), getencrypttype(WL_5GBH_BAND, 0), getpsk(WL_5GBH_BAND));
		fclose(fp);
	}else
		printf("failed to open %s\n", "/etc/map/wts_bss_info_config");
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

	mkdir("/etc/map", 0777);
	if(sw_mode == SW_MODE_AP || sw_mode == SW_MODE_ROUTER){
		if(enable == 0){
			nvram_set("easymesh_enable", "1");
			nvram_set("easymesh_mode", "1");
			nvram_set("easymesh_role", "0");
			enable = 1;
			mode = MAP_TURNKEY;
			role = EASYMESH_ROLE_NONE;//auto
		}
		if(mesh_re_node()){
			nvram_set("easymesh_role", "2");//agent
			role = EASYMESH_ROLE_AGENT;
		}
	}else{//same as aimesh, controller/agent only works in ap/router mode
		nvram_set("easymesh_enable", "0");
		enable = 0;
	}
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
		if(sw_mode() == SW_MODE_ROUTER)
			doSystem("fwdd -p %s %s -p %s %s -e %s 5G &", get_wififname(WL_2G_BAND), get_staifname(WL_2G_BAND), get_wififname(WL_5G_BAND), get_staifname(WL_5G_BAND), nvram_safe_get("wan0_ifname"));
		else
#if defined(RTCONFIG_MT798X)
			doSystem("fwdd -p %s %s -p %s %s -e %s 5G &", get_wififname(WL_2G_BAND), get_staifname(WL_2G_BAND), get_wififname(WL_5G_BAND), get_staifname(WL_5G_BAND), "br0");
#else
			doSystem("fwdd -p %s %s -p %s %s -e %s 5G &", get_wififname(WL_2G_BAND), get_staifname(WL_2G_BAND), get_wififname(WL_5G_BAND), get_staifname(WL_5G_BAND), "vlan1");
#endif
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
			doSystem("fwdd -p %s %s -p %s %s -e %s 5G &", get_wififname(WL_2G_BAND), get_staifname(WL_2G_BAND), get_wififname(WL_5G_BAND), get_staifname(WL_5G_BAND), nvram_safe_get("wan0_ifname"));
		}
	} else if(mode == MAP_TURNKEY){
#if defined(RTCONFIG_RALINK_MT7621)
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
#if defined(RTCONFIG_RALINK_MT7621) || defined(RTCONFIG_RALINK_MT7622)
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
		eval("iwpriv", (char*) WIF_2G, "set", "VLANTag=1");
		eval("iwpriv", (char*) WIF_5G, "set", "VLANTag=1");
		eval("iwpriv", (char*) WIF_2G, "set", "VLANPolicy=0:4");
		eval("iwpriv", (char*) WIF_5G, "set", "VLANPolicy=0:4");
		eval("iwpriv", (char*) WIF_2G, "set", "VLANPolicy=1:2");
		eval("iwpriv", (char*) WIF_5G, "set", "VLANPolicy=1:2");
		eval("iwpriv", bh2gifname, "set", "VLANTag=1");
		eval("iwpriv", bh5gifname, "set", "VLANTag=1");
		eval("iwpriv", bh2gifname, "set", "VLANPolicy=0:4");
		eval("iwpriv", bh5gifname, "set", "VLANPolicy=0:4");
		eval("iwpriv", bh2gifname, "set", "VLANPolicy=1:2");
		eval("iwpriv", bh5gifname, "set", "VLANPolicy=1:2");
		eval("iwpriv", (char*) APCLI_2G, "set", "VLANTag=1");
		eval("iwpriv", (char*) APCLI_5G, "set", "VLANTag=1");
		eval("iwpriv", (char*) APCLI_2G, "set", "VLANPolicy=0:4");
		eval("iwpriv", (char*) APCLI_5G, "set", "VLANPolicy=0:4");
		eval("iwpriv", (char*) APCLI_2G, "set", "VLANPolicy=1:2");
		eval("iwpriv", (char*) APCLI_5G, "set", "VLANPolicy=1:2");
		start_wapp();
		if(role == EASYMESH_ROLE_MASTER)
			doSystem("p1905_managerd -r0 -f /etc/map/1905d.cfg -F /etc/map/wts_bss_info_config > /var/log/1905.log&");
		else
			doSystem("p1905_managerd -r1 -f /etc/map/1905d.cfg -F /etc/map/wts_bss_info_config > /var/log/1905.log&");
		sleep(1);
//		doSystem("mapd -I /etc/map/mapd_cfg -O /etc/mapd_strng.conf -c /jffs/client_db.txt -G /etc/map/wts_bss_info_config > /var/log/log.mapd&");
		doSystem("mapd -I /etc/map/mapd_cfg -O /etc/mapd_strng.conf -c /jffs/client_db.txt > /var/log/log.mapd&");
#if defined(RTCONFIG_RALINK_MT7621) || defined(RTCONFIG_RALINK_MT7622)
		doSystem("switch reg w 10 ffffffe0");
		doSystem("switch reg w 34 8160816");
#endif
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
		doSystem("fwdd -p %s %s -p %s %s -e %s 5G &", get_wififname(WL_2G_BAND), get_staifname(WL_2G_BAND), get_wififname(WL_5G_BAND), get_staifname(WL_5G_BAND), nvram_safe_get("wan0_ifname"));
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
#if defined(RTCONFIG_RALINK_MT7621)
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
	start_mapd();
	if(nvram_match("easymesh_enable", "1") && nvram_get_int("easymesh_role") == EASYMESH_ROLE_AGENT)
		_eval(agent_argv, NULL, 0, &pid);
	return 0;
}

void easymesh_agent(void)
{

}

#endif
