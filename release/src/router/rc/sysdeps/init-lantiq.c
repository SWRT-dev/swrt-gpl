/*
 * Copyright 2021-2022, ASUS
 * Copyright 2021-2026, SWRTdev
 * Copyright 2021-2026, paldier <paldier@hotmail.com>.
 * Copyright 2021-2026, lostlonger<lostlonger.g@gmail.com>.
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
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sysinfo.h>
#include <wlutils.h>
#include <bcmdevs.h>
#include <string.h>
#include <stdint.h>
#include <sys/sysmacros.h>
#include <shared.h>
#include <swrt.h>
#include <lantiq.h>
#include <flash_mtd.h>

#define MKNOD(name,mode,dev)		if (mknod(name,mode,dev))		perror("## mknod " name)

extern void check_ubi_partition(void);
extern int init_gpio_again(void);

static void __load_wifi_driver(int testmode)
{
	char country[FACTORY_COUNTRY_CODE_LEN + 1], prefix[sizeof("wlXXXXX_")]/*, index[2]*/;
	char fastpath[] = "fastpath=1,1";/* disable=0,fastpath=1,dcdp=2 */
	int unit;
	pid_t pid;
	FILE *fp_wifi;
	char vphy[IFNAMSIZ] = { 0 }, path_wifi[sizeof("/tmp/wifiXXXX.sh")];
	char macaddr[] = "00:11:22:33:44:55";
	unsigned char mac_binary[6];
//	char *handler_argv[]= { "/opt/intel/bin/dump_handler", "-i", index, "-f", "/opt/intel/wave/", NULL };
#if !defined(RAX40)
	char *dwpald_argv[]= { "dwpal_daemon", NULL };
#endif
	eval("insmod", "/lib/modules/4.9.276/compat.ko");
	eval("insmod", "/lib/modules/4.9.276/cfg80211.ko");
	eval("insmod", "/lib/modules/4.9.276/mac80211.ko");
	eval("insmod", "/lib/modules/4.9.276/mtlkroot.ko");
	eval("rmmod", "mtlk");
	eval("insmod", "mtlk", fastpath, "loggersid=255,255"/*, "ap=1,1", "ahb_off=1"*/);
	setenv("PATH", "/bin:/sbin:/usr/bin:/usr/sbin:/opt/intel/sbin:/opt/intel/usr/sbin:/opt/intel/bin", 1);
	setenv("LD_LIBRARY_PATH", "/opt/intel/lib:/opt/intel/usr/lib", 1);
#if !defined(RAX40)
	_eval(dwpald_argv, NULL, 0, &pid);
#endif
	///proc/net/mtlk/card*
	//strlcpy(index, "0", sizeof(index));
	//_eval(handler_argv, NULL, 0, &pid);
	//strlcpy(index, "1", sizeof(index));
	//_eval(handler_argv, NULL, 0, &pid);

	eval("mkdir", "-p", "/etc/Wireless/sh");
	for (unit = WL_2G_BAND; unit < MAX_NR_WL_IF; ++unit) {
		SKIP_ABSENT_BAND(unit);

		snprintf(prefix, sizeof(prefix), "wl%d_", unit);
		strlcpy(vphy, get_vphyifname(unit), sizeof(vphy));
		snprintf(path_wifi, sizeof(path_wifi), "/tmp/%s.sh", vphy);
		if (!(fp_wifi = fopen(path_wifi, "w"))) {
			dbg("%s: Can't open %s for writing!\n", __func__, path_wifi);
			continue;
		}

		fprintf(fp_wifi, "#!/bin/sh\n");
		switch (unit) {
		default:
			/* Wireless Extension or cfg80211 + nl80211 based band. */

			/* Country */
			strlcpy(country, nvram_pf_safe_get(prefix, "country_code"), sizeof(country));
			fprintf(fp_wifi, "iw reg set %s\n", country);
			nvram_set_int("wl_country_changed", 7);
			/* TX power adjustment. */
			if (find_word(nvram_safe_get("rc_support"), "pwrctrl")) {
				if(nvram_pf_get_int(prefix, "txpower") > 100 || nvram_pf_get_int(prefix, "txpower") <= 0)
					fprintf(fp_wifi, "iw phy %s set txpower auto\n", vphy);
				else
					fprintf(fp_wifi, "iw phy %s set txpower fixed %d\n",
						vphy, nvram_pf_get_int(prefix, "txpower") * 30);//0-3000mBm
			}
			//create master ap/vphy and initial data
			fprintf(fp_wifi, "iw phy %s interface add %s type __ap\n", get_realphyifname(unit), vphy);
			strlcpy(macaddr, nvram_pf_safe_get(prefix, "hwaddr"), sizeof(macaddr));
			ether_atoe(macaddr, mac_binary);
			mac_binary[5] += 0xa;
			ether_etoa(mac_binary, macaddr);
			fprintf(fp_wifi, "ifconfig %s hw ether %s\n", vphy, macaddr);
			fprintf(fp_wifi, "ifconfig %s up\n", vphy);
			fprintf(fp_wifi, "ifconfig %s down\n", vphy);
			fprintf(fp_wifi, "ppacmd addlan -i %s", vphy);
		}

		fclose(fp_wifi);
		chmod(path_wifi, 0777);
	}
}

void init_devs(void)
{
	int status;
	MKNOD("/dev/nvram", S_IFCHR | 0666, makedev(228, 0));

	if((status = WEXITSTATUS(modprobe("nvram_linux"))))
		printf("## modprobe(nvram_linux) fail status(%d)\n", status);
	if(patch_Factory)
		patch_Factory();
}

int init_devs_defer(void)
{
#if defined(K3C)
	fprintf(stderr, "K3C: init gpio\n");
	gpio_dir(34, GPIO_DIR_OUT);
	gpio_dir(35, GPIO_DIR_OUT);
	gpio_dir(36, GPIO_DIR_OUT);

	set_gpio(34, 0);
	set_gpio(35, 1);
	set_gpio(36, 1);
	f_write_string("/sys/class/leds/wan/trigger", "netdev", 0, 0);
	f_write_string("/sys/class/leds/wan/device_name", "eth1", 0, 0);
	f_write_string("/sys/class/leds/wan/mode", "link tx rx", 0, 0);
	f_write_string("/sys/class/leds/lan1/trigger", "netdev", 0, 0);
	f_write_string("/sys/class/leds/lan1/device_name", "eth0_3", 0, 0);
	f_write_string("/sys/class/leds/lan1/mode", "link tx rx", 0, 0);
	f_write_string("/sys/class/leds/lan2/trigger", "netdev", 0, 0);
	f_write_string("/sys/class/leds/lan2/device_name", "eth0_2", 0, 0);
	f_write_string("/sys/class/leds/lan2/mode", "link tx rx", 0, 0);
	f_write_string("/sys/class/leds/lan3/trigger", "netdev", 0, 0);
	f_write_string("/sys/class/leds/lan3/device_name", "eth0_4", 0, 0);
	f_write_string("/sys/class/leds/lan3/mode", "link tx rx", 0, 0);
#elif defined(BLUECAVE)
	fprintf(stderr, "BLUECAVE: init gpio\n");
	gpio_dir(0, GPIO_DIR_IN);
	gpio_dir(30, GPIO_DIR_IN);
	gpio_dir(1, GPIO_DIR_OUT);
	set_gpio(1, 0);
	gpio_dir(4, GPIO_DIR_OUT);
	set_gpio(4, 0);
	gpio_dir(6, GPIO_DIR_OUT);
	set_gpio(6, 1);
	gpio_dir(8, GPIO_DIR_OUT);
	set_gpio(8, 0);
	system("mem -s 0x16c80128 -uw 0x00; mem -s 0x16c80194 -uw 0x00000003");
	gpio_dir(42, GPIO_DIR_OUT);
	set_gpio(42, 0);
	gpio_dir(43, GPIO_DIR_OUT);
	set_gpio(43, 1);
#elif defined(RAX40)
	if(nvram_match("AllLED", "1")){
		f_write_string("/sys/class/leds/lan1/trigger", "netdev", 0, 0);
		f_write_string("/sys/class/leds/lan1/device_name", "eth0_1", 0, 0);
		f_write_string("/sys/class/leds/lan1/link", "1", 0, 0);
		f_write_string("/sys/class/leds/lan1/rx", "1", 0, 0);
		f_write_string("/sys/class/leds/lan1/tx", "1", 0, 0);
		f_write_string("/sys/class/leds/lan2/trigger", "netdev", 0, 0);
		f_write_string("/sys/class/leds/lan2/device_name", "eth0_2", 0, 0);
		f_write_string("/sys/class/leds/lan2/link", "1", 0, 0);
		f_write_string("/sys/class/leds/lan2/rx", "1", 0, 0);
		f_write_string("/sys/class/leds/lan2/tx", "1", 0, 0);
		f_write_string("/sys/class/leds/lan3/trigger", "netdev", 0, 0);
		f_write_string("/sys/class/leds/lan3/device_name", "eth0_3", 0, 0);
		f_write_string("/sys/class/leds/lan3/link", "1", 0, 0);
		f_write_string("/sys/class/leds/lan3/rx", "1", 0, 0);
		f_write_string("/sys/class/leds/lan3/tx", "1", 0, 0);
		f_write_string("/sys/class/leds/lan4/trigger", "netdev", 0, 0);
		f_write_string("/sys/class/leds/lan4/device_name", "eth0_4", 0, 0);
		f_write_string("/sys/class/leds/lan4/link", "1", 0, 0);
		f_write_string("/sys/class/leds/lan4/rx", "1", 0, 0);
		f_write_string("/sys/class/leds/lan4/tx", "1", 0, 0);
	} else {
		f_write_string("/sys/class/leds/lan1/trigger", "none", 0, 0);
		f_write_string("/sys/class/leds/lan2/trigger", "none", 0, 0);
		f_write_string("/sys/class/leds/lan3/trigger", "none", 0, 0);
		f_write_string("/sys/class/leds/lan4/trigger", "none", 0, 0);
	}
#endif
	check_ubi_partition();
	_dprintf("start_jffs2() start\n");
	start_ubifs();
	_dprintf("start_jffs2() end\n");
	return 0;
}

void init_others(void)
{
//	struct stat cal_stat;
	pid_t pid;

	init_gpio_again();
#if defined(BLUECAVE)
	system("cp -R /lib/firmware/ar3k /tmp/");
#endif
	system("ln -sf /opt/intel/etc/wave_components.ver /etc/");
	system("read_img wlanconfig /tmp/cal_eeprom.tar.gz");
//	if(stat("/tmp/cal_eeprom.tar.gz", &cal_stat) || cal_stat.st_blocks < 801)
//		write_default_cal();
//	else
//	{
		system("rm -f /tmp/cal_wlan*.bin");
		system("tar zxf /tmp/cal_eeprom.tar.gz -C /tmp/");
//	}
	system("rm -f /tmp/cal_eeprom.tar.gz");
//	if(!stat("/tmp/cal_wlan1.bin", &cal_stat))
//		system("rm -f /tmp/cal_wlan2.bin; mv /tmp/cal_wlan1.bin /tmp/cal_wlan2.bin");
	if(!f_exists("/dev/switch_api/0"))
		MKNOD("/dev/switch_api/0", S_IFCHR | 0640, makedev(81, 0));
	if(!f_exists("/dev/switch_api/1"))
		MKNOD("/dev/switch_api/1", S_IFCHR | 0640, makedev(81, 1));
	system("insmod xrx500_phy_fw");
//	system("load_gphy_firmware_preinit.sh");
//	MKNOD("/dev/ifx_mei", S_IFCHR | 0666, makedev(105, 0));
//	MKNOD("/dev/ifx_ppa", S_IFCHR | 0666, makedev(181, 0));
//	system("insmod drv_ifxos");
//	system("insmod drv_event_logger");
	system("insmod directconnect_datapath");
	system("insmod ltq_eth_drv_xrx500");
	system("insmod dc_mode-hw");
//	system("insmod mcast_helper");
	system("insmod macvlan");
	system("insmod ppa_drv_stack_al");
	system("insmod ltq_pae_hal");
	system("insmod ltq_tmu_hal_drv");
	system("insmod ip_tables");
	system("insmod ppa_api");
	system("insmod esp4");
	system("insmod ltq_crypto");
	system("insmod ltq_mpe_hal_drv");
//	system("insmod ppa_api_tmplbuf");
	system("insmod ppa_api_sw_accel_mod");
	system("insmod hw_tcp_litepath");
	system("insmod mac_violation_mirror");
//	system("insmod ltq_pmcu");
	nvram_set("ctf_disable", nvram_safe_get("ctf_disable_force"));
	system("ppacmd init -n 30");
	system("ppacmd addlan -i eth0_1");
	system("ppacmd addlan -i eth0_2");
	system("ppacmd addlan -i eth0_3");
#if !defined(K3C)
	system("ppacmd addlan -i eth0_4");
#endif
	system("ppacmd addlan -i br0");
	doSystem("ethtool -K eth1 gro off\n");
#if defined(RTCONFIG_AMAS)
	if(!aimesh_re_mode())
		eval("iptables", "-t", "mangle", "-A", "INPUT", "-p", "tcp", "-m", "state", "--state", "ESTABLISHED", "-j", "EXTMARK", "--set-mark", "0x80000000/0x80000000");
#endif
	nvram_set("wave_action", "1");

//	if (repeater_mode() || mediabridge_mode() || aimesh_re_node())
//	{
//		modprobe("l2nat");
//		f_write_string("/proc/sys/net/bridge/bridge-nf-call-iptables", "0", 0, 0);
//	}
	if(nvram_get_int("ATEMODE") != 1)
	{
		_dprintf("poweroff usb\n");
		usb_pwr_ctl(0);
	}
	fprintf(stderr, "init_others() End.\n");
}

void init_others_defer(void)
{
	;
}

int is_if_up(char *ifname)
{
	return is_intf_up(ifname) == 1 ? 1 : 0;
}

int init_gpio_again(void)
{
#if defined(K3C)
	fprintf(stderr, "K3C: init gpio again\n");
	gpio_dir(34, GPIO_DIR_OUT);
	gpio_dir(35, GPIO_DIR_OUT);
	gpio_dir(36, GPIO_DIR_OUT);

	set_gpio(34, 0);
	set_gpio(35, 1);
	set_gpio(36, 1);
#elif defined(BLUECAVE)
	fprintf(stderr, "BLUECAVE: init gpio again\n");
	gpio_dir(0, GPIO_DIR_IN);
	gpio_dir(30, GPIO_DIR_IN);
	gpio_dir(1, GPIO_DIR_OUT);
	set_gpio(1, 0);
	gpio_dir(4, GPIO_DIR_OUT);
	set_gpio(4, 0);
	gpio_dir(6, GPIO_DIR_OUT);
	set_gpio(6, 1);
	gpio_dir(8, GPIO_DIR_OUT);
	set_gpio(8, 0);
	system("mem -s 0x16c80128 -uw 0x00; mem -s 0x16c80194 -uw 0x00000003");
	gpio_dir(42, GPIO_DIR_OUT);
	set_gpio(42, 0);
	gpio_dir(43, GPIO_DIR_OUT);
	set_gpio(43, 1);
#endif
	return 0;
}

void generate_switch_para(void)
{
	fprintf(stderr, "Raymond: skip generate_switch_para()\n");
}

int write_default_cal(void)
{
#if defined(K3C)
#elif defined(BLUECAVE)
//	system("cp -f /opt/lantiq/wave/images/backup/cal_wlan0.bin /tmp/cal_wlan0.bin");
//	system("cp -f /opt/lantiq/wave/images/backup/cal_wlan1.bin /tmp/cal_wlan1.bin");
//	system("rm -f /tmp/cal_eeprom.tar.gz");
//	system("cd /tmp; tar czf cal_eeprom.tar.gz cal_wlan0.bin cal_wlan1.bin");
//	system("upgrade /tmp/cal_eeprom.tar.gz wlanconfig 0 0");
//	system("rm -f /tmp/cal_eeprom.tar.gz");
#elif defined(RAX40)
#else
#error fix me
#endif
	return 0;
}



void enable_jumbo_frame(void)
{
	if(nvram_contains_word("rc_support", "switchctrl"))
	{
		if(nvram_get_int("jumbo_frame_enable"))
		{
			ifconfig_mtu("eth0_1", 8000);
			ifconfig_mtu("eth0_2", 8000);
			ifconfig_mtu("eth0_3", 8000);
#if !defined(K3C)
			ifconfig_mtu("eth0_4", 8000);
#endif
		}
	}
}

void init_switch(void)
{
	enable_jumbo_frame();
}

int switch_exist(void)
{
	int i, n;
#if defined(K3C)
	char *eth[] = {"eth1", "eth0_1", "eth0_2", "eth0_3"};
#else
	char *eth[] = {"eth1", "eth0_1", "eth0_2", "eth0_3", "eth0_4"};
#endif
	n = sizeof(eth) / sizeof(eth[0]);
	for(i = 0; i < n; i++){
		if(!is_if_up(eth[i]))
			break;
	}
	if(i == n)	
		return 1;
	return 0;
}

void load_wifi_driver(void)
{
	__load_wifi_driver(0);
}

void load_testmode_wifi_driver(void)
{
	__load_wifi_driver(1);
}

void set_uuid(void)
{
	FILE *fp;
	int len;
	char buf[60], *p;
	if((fp = popen("cat /proc/sys/kernel/random/uuid", "r")))
	{
		memset(buf, 0, sizeof(buf));
		fread(buf, 1, sizeof(buf), fp);
		len = strlen(buf);
		for(p = buf; *p && len ; ++p){
			if(!isxdigit(*p) && *p != '-'){
				*p = 0x0;
				break;
			}
			len--;
		}
		nvram_set("uuid", buf);
		pclose(fp);
	}
}

/**
 * Down all VAP interfaces, kill all related hostapd instance and
 * delete VAP interface if @unregister_vap is true.
 * @unregister_vap:	Unregister VAP interface if true, except WiGig interface.
 */
void deinit_all_vaps(const int unregister_vap)
{
	int unit, sunit, max_sunit;
	char wif[IFNAMSIZ], prefix[sizeof("wlX_YYY")];
	char hconf_path[sizeof("/etc/Wireless/conf/hostapd_X.confXXX") + IFNAMSIZ];
	char sock_path[sizeof("/var/run/hostapd/XXXXXX") + IFNAMSIZ];

	for (unit = 0; unit < MAX_NR_WL_IF; ++unit) {
		SKIP_ABSENT_BAND(unit);
		max_sunit = num_of_mssid_support(unit);
		snprintf(prefix, sizeof(prefix), "wl%d_", unit);
		for (sunit = 0; sunit <= max_sunit; ++sunit) {
			get_wlxy_ifname(unit, sunit, wif);
			snprintf(hconf_path, sizeof(hconf_path), "/etc/Wireless/conf/hostapd_%s.conf", wif);
			snprintf(sock_path, sizeof(sock_path), "/var/run/hostapd/%s", wif);
			if (f_exists(sock_path))
				eval(QWPA_CLI, "-g", QHOSTAPD_CTRL_IFACE, "raw", "REMOVE", wif);
			if (unregister_vap && f_exists(hconf_path))
				unlink(hconf_path);
			if (!iface_exist(wif))
				continue;
			ifconfig(wif, 0, NULL, NULL);
		}
	}

	kill_pidfile_tk(QHOSTAPD_PID_PATH);

	/* in case of pid file is gone...*/
	eval("killall", "hostapd");
}

/**
 * Rebuild main VAP of each bands.
 * @return:
 * 	0:	success
 *  otherwise:	error
 */
int rebuild_main_vap(void)
{
	int band;
	char vap[IFNAMSIZ];
	char sock_path[sizeof("/var/run/hostapd/XXXXXX") + IFNAMSIZ];

	for (band = WL_2G_BAND; band < MAX_NR_WL_IF; ++band) {
		SKIP_ABSENT_BAND(band);
		__get_wlifname(band, 0, vap);
		snprintf(sock_path, sizeof(sock_path), "/var/run/hostapd/%s", vap);
		if (f_exists(sock_path))
			eval(QWPA_CLI, "-g", QHOSTAPD_CTRL_IFACE, "raw", "REMOVE", vap);
		if (band == WL_60G_BAND)
			continue;
		if (iface_exist(vap)){
			eval("ppacmd", "dellan", "-i", vap);
			destroy_vap(vap);
		}
		create_vap(vap, band, "ap");
		eval("ppacmd", "addlan", "-i", vap);
	}

	return 0;
}

static int create_node=0;
void init_wl(void)
{
	int unit;
	char *p, *ifname;
	char *wl_ifnames;
	char path_wifi[sizeof("/tmp/wifiXXXX.sh")];
	char prefix[] = "wl1_1xxxxxxx";
#ifdef RTCONFIG_WIRELESSREPEATER
	int wlc_band;
#endif
	handle_location_code_for_wl();
	if(!create_node)
	{ 
		load_wifi_driver();
		sleep(2);
		for (unit = WL_2G_BAND; unit < MAX_NR_WL_IF; ++unit) {
			SKIP_ABSENT_BAND(unit);

			snprintf(path_wifi, sizeof(path_wifi), "/tmp/%s.sh", get_vphyifname(unit));
			eval(path_wifi);
			unlink(path_wifi);
		}
		dbG("init_wl:create wi node\n");
		if ((wl_ifnames = strdup(nvram_safe_get("lan_ifnames"))) != NULL) 
		{
			p = wl_ifnames;
			while ((ifname = strsep(&p, " ")) != NULL) {
				while (*ifname == ' ') ++ifname;
				if (*ifname == 0) break;
				SKIP_ABSENT_FAKE_IFACE(ifname);

#if defined(RTCONFIG_BONDING_WAN) || defined(RTCONFIG_LACP)
				if (!strncmp(ifname, "bond", 4))
					continue;
#endif

				//create ath00x & ath10x 
				if (!strncmp(ifname, WIF_2G, strlen(WIF_2G)))
					unit = 0;
				else if (!strncmp(ifname, WIF_5G, strlen(WIF_5G)))
					unit = 1;
				else
					unit=-99;	

				switch (unit) {
				case WL_2G_BAND:	/* fall-through */
				case WL_5G_BAND:	/* fall-through */
					create_vap(ifname, unit, "ap");
					snprintf(prefix, sizeof(prefix), "wl%d_", unit);
					set_hwaddr(ifname, nvram_pf_safe_get(prefix, "hwaddr"));
					eval("ppacmd", "addlan", "-i", ifname);
					sleep(1);

#if defined(RTCONFIG_REPEATER_STAALLBAND)
					if (sw_mode() == SW_MODE_REPEATER && nvram_get_int("x_Setting")) {
						dbG("\ncreate a STA node %s from %s\n", get_staifname(unit), get_vphyifname(unit));
						create_vap(get_staifname(unit), unit, "sta");
						set_hwaddr(get_staifname(unit), nvram_pf_safe_get(prefix, "hwaddr"));
						eval("ppacmd", "addlan", "-i", get_staifname(wlc_band));
						sleep(1);
					}
#endif

					break;
				default:
					if (!strncmp(ifname, "eth", 3))
						break;
					dbg("%s: Unknown wl%d band, ifname [%s]!\n", __func__, unit, ifname);
				}
			}
			free(wl_ifnames);
		}
		create_node=1;
#ifdef RTCONFIG_WIRELESSREPEATER
#if !defined(RTCONFIG_REPEATER_STAALLBAND)
		if ((sw_mode() == SW_MODE_REPEATER || wisp_mode()) && nvram_get_int("x_Setting")) {
			wlc_band=nvram_get_int("wlc_band");
			create_vap(get_staifname(wlc_band), wlc_band, "sta");
			snprintf(prefix, sizeof(prefix), "wl%d_", wlc_band);
			set_hwaddr(get_staifname(wlc_band), nvram_pf_safe_get(prefix, "hwaddr"));
#if defined(RTCONFIG_WISP)
			if(wisp_mode())
				eval("ppacmd", "addwan", "-i", get_staifname(wlc_band));
			else
#endif
				eval("ppacmd", "addlan", "-i", get_staifname(wlc_band));
		}
#endif
#endif
		/* Calculate 40/80/160MHz and 4.32/6.48/8.64GHz bandwidth capability based on channel list.
		 * Main VAP must ready before doing this.
		 */
		for (unit = WL_2G_BAND; unit < MAX_NR_WL_IF; ++unit) {
			SKIP_ABSENT_BAND(unit);
			calculate_bw_of_each_channel(unit);
		}
	}
	nvram_set("wave_action", "0");
}

void fini_wl(void)
{
	int i;
	char ifname[IFNAMSIZ];
	char path_pid[sizeof("/var/run/hostapd_cli-athX.pid") + IFNAMSIZ];

#if defined(RTCONFIG_WISP)
	if (wisp_mode())
		ifconfig(get_staifname(nvram_get_int("wlc_band")), 0, NULL, NULL);
#endif

	dbG("fini_wl:destroy wi node\n");
	for (i = WL_2G_BAND; i < MAX_NR_WL_IF; ++i) {
		SKIP_ABSENT_BAND(i);

		get_wlxy_ifname(i, 0, ifname);
		snprintf(path_pid, sizeof(path_pid), "/var/run/hostapd_cli-%s.pid", ifname);
		if (f_exists(path_pid))
			kill_pidfile(path_pid);
	}

	deinit_all_vaps(1);

	/* Delete all STA interfaces. */
	kill_wifi_wpa_supplicant(-1);
	for (i = 0; i < MAX_NR_WL_IF; ++i) {
		SKIP_ABSENT_BAND(i);

		strlcpy(ifname, get_staifname(i), sizeof(ifname));
		if (!iface_exist(ifname))
			continue;

		ifconfig(ifname, 0, NULL, NULL);
#if defined(RTCONFIG_WISP)
		if(wisp_mode() && nvram_get_int("wlc_band") == i)
			eval("ppacmd", "delwan", "-i", ifname);
		else
#endif
			eval("ppacmd", "dellan", "-i", ifname);

		destroy_vap(ifname);
	}
	//destroy vphy wlan0&wlan2
	ifconfig(get_vphyifname(0), 0, NULL, NULL);
	ifconfig(get_vphyifname(1), 0, NULL, NULL);
	eval("ppacmd", "dellan", "-i", get_vphyifname(0));
	eval("ppacmd", "dellan", "-i", get_vphyifname(1));
	eval("iw", get_vphyifname(0), "del");
	eval("iw", get_vphyifname(1), "del");
	create_node=0;
}

void init_syspara(void)
{
	unsigned char buffer[16];
	unsigned char *dst, *p;
	unsigned int bytes;
	char macaddr[] = "00:11:22:33:44:55";
	char macaddr2[] = "00:11:22:33:44:58";
	char country_code[FACTORY_COUNTRY_CODE_LEN+1];
	char pin[9];
	char productid[13];
	char fwver[8];
	char blver[32];
#ifdef RTCONFIG_ODMPID
#ifdef RTCONFIG_32BYTES_ODMPID
	char modelname[32];
#else
	char modelname[16];
#endif
#endif
	char ipaddr_lan[16];

	set_basic_fw_name();
	/* /dev/mtd/2, RF parameters, starts from 0x40000 */
	dst = buffer;
	bytes = 6;
	memset(buffer, 0, sizeof(buffer));
	memset(country_code, 0, sizeof(country_code));
	memset(pin, 0, sizeof(pin));
	memset(productid, 0, sizeof(productid));
	memset(fwver, 0, sizeof(fwver));
	if (FRead(dst, OFFSET_MAC_ADDR_2G, bytes) < 0) {  // ET0/WAN is same as 2.4G
		_dprintf("READ MAC address 2G: Out of scope\n");
	} else {
		if (buffer[0] != 0xff)
			ether_etoa(buffer, macaddr);
	}

	if (FRead(dst, OFFSET_MAC_ADDR, bytes) < 0) { // ET1/LAN is same as 5G
		_dprintf("READ MAC address : Out of scope\n");
	} else {
		if (buffer[0] != 0xff)
			ether_etoa(buffer, macaddr2);
	}
	if (!mssid_mac_validate(macaddr) || !mssid_mac_validate(macaddr2))
		nvram_set("wl_mssid", "0");
	else
		nvram_set("wl_mssid", "1");
#if defined(K3C) || defined(RAX40)
	nvram_set("wl_mssid", "1");
#endif
	nvram_set("et0macaddr", macaddr);
	nvram_set("wl0_hwaddr", macaddr);
	nvram_set("et1macaddr", macaddr2);
	nvram_set("wl1_hwaddr", macaddr2);

	country_code[0] = '\0';
	dst = (unsigned char*) country_code;
	bytes = FACTORY_COUNTRY_CODE_LEN;
	if (FRead(dst, OFFSET_COUNTRY_CODE, bytes)<0)
	{
		_dprintf("READ ASUS country code: Out of scope\n");
		nvram_set("wl_country_code", "DB");
		nvram_set("wl0_country_code", "DB");
		nvram_set("wl1_country_code", "DB");
	}
	else
	{
		dst[FACTORY_COUNTRY_CODE_LEN]='\0';
		if(*dst == 0xff)
			strcpy(country_code, "DB");
		nvram_set("wl_country_code", country_code);
		nvram_set("wl0_country_code", country_code);
		nvram_set("wl1_country_code", country_code);
	}

	/* reserved for Ralink. used as ASUS pin code. */
	dst = (char *)pin;
	bytes = 8;
	if (FRead(dst, OFFSET_PIN_CODE, bytes) < 0) {
		_dprintf("READ ASUS pin code: Out of scope\n");
		nvram_set("wl_pin_code", "");
	} else {
		if (((unsigned char)pin[0] == 0xff) || !strcmp(pin, "12345670") || !strcmp(pin, "12345678")) {
			char devPwd[9];
			nvram_set("secret_code", wps_gen_pin(devPwd, sizeof(devPwd)) ? devPwd : "12345670");
		}
		else
			nvram_set("secret_code", pin);
	}
#if defined(BLUECAVE)
	dst = buffer;
	bytes = 16;
	if (linuxRead(dst, 0x20, bytes) < 0) {	/* The "linux" MTD partition, offset 0x20. */
		fprintf(stderr, "READ firmware header: Out of scope\n");
		nvram_set("productid", "BLUECAVE");
		nvram_set("firmver", "3.0.0.4");
	} else {
		strncpy(productid, buffer + 4, 12);
		productid[12] = 0;
		snprintf(fwver, sizeof(fwver), "%d.%d.%d.%d", buffer[0], buffer[1], buffer[2],
			buffer[3]);
		nvram_set("productid", trim_r(productid));
		nvram_set("firmver", trim_r(fwver));
	}
#else
	nvram_set("firmver", rt_version);
	nvram_set("productid", rt_buildname);
	strncpy(productid, rt_buildname, sizeof(productid));
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
		if ((buffer[0] == 0xff)|| !strcmp(buffer,"NONE"))
			nvram_set("wifi_psk", "");
		else
			nvram_set("wifi_psk", buffer);
	}
#endif
#if defined(BLUECAVE)
	memset(buffer, 0, sizeof(buffer));
	FRead(buffer, OFFSET_BOOT_VER, 4);
	snprintf(blver, sizeof(blver), "%s-0%c-0%c-0%c-0%c", trim_r(productid), buffer[0],
		buffer[1], buffer[2], buffer[3]);
	nvram_set("blver", trim_r(blver));

	system("nvram set blver=`uboot_env --get --name bl_ver`");
	system("nvram set bl_ver=`uboot_env --get --name ver`");
#else
	system("nvram set blver=`uboot_env --get --name ver`");
	system("nvram set bl_ver=`uboot_env --get --name ver`");
#endif
	_dprintf("bootloader version: %s\n", nvram_safe_get("blver"));
	_dprintf("firmware version: %s\n", nvram_safe_get("firmver"));

	nvram_set("wl1_txbf_en", "0");
#ifdef RTCONFIG_ODMPID
	FRead(modelname, OFFSET_ODMPID, 16);
	modelname[15] = '\0';
	if (modelname[0] != 0 && (unsigned char)(modelname[0]) != 0xff && is_valid_hostname(modelname) && strcmp(modelname, "ASUS")) {
		if(!strcmp(modelname, "BLUECAVE"))
			nvram_set("odmpid", "BLUE_CAVE");
		else
			nvram_set("odmpid", modelname);
	} else
		nvram_unset("odmpid");
#endif
	nvram_set("firmver", rt_version);
	nvram_set("productid", rt_buildname);

	FRead(ipaddr_lan, OFFSET_IPADDR_LAN, sizeof(ipaddr_lan));
	ipaddr_lan[sizeof(ipaddr_lan)-1] = '\0';
	if ((unsigned char)(ipaddr_lan[0]) != 0xff && !illegal_ipv4_address(ipaddr_lan))
		nvram_set("IpAddr_Lan", ipaddr_lan);
	else
		nvram_unset("IpAddr_Lan");
}

#if defined(RTCONFIG_ATEUSB3_FORCE)
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
#if 0
	char prefix[] = "wlXXXXXXX_", prefix_wlc[] = "wlXXXXXXX_";
	char tmp[128];

	if(aimesh_re_node())
	{
		wl_nvprefix(prefix, sizeof(prefix), unit, subunit);
		nvram_pf_set(prefix, "bss_enabled", "1");
		if(subunit == -1)
		{
			snprintf(prefix, sizeof(prefix), "wl%d_", unit);
			snprintf(prefix_wlc, sizeof(prefix_wlc), "wlc%d_", unit);
			nvram_pf_set(prefix, "ssid", nvram_pf_safe_get(prefix_wlc, "ssid"));
			nvram_pf_set(prefix, "auth_mode_x", nvram_pf_safe_get(prefix_wlc, "auth_mode"));
			nvram_pf_set(prefix, "wep_x", nvram_pf_safe_get(prefix_wlc, "wep"));
			nvram_pf_set(prefix, "wep_x", nvram_pf_safe_get(prefix_wlc, "wep"));
			if(nvram_pf_get_int(prefix_wlc, "wep")){
				nvram_pf_set(prefix, "key", nvram_pf_safe_get(prefix_wlc, "key"));
				nvram_pf_set(prefix, "key1", nvram_pf_safe_get(prefix_wlc, "wep_key"));
				nvram_pf_set(prefix, "key2", nvram_pf_safe_get(prefix_wlc, "wep_key"));
				nvram_pf_set(prefix, "key3", nvram_pf_safe_get(prefix_wlc, "wep_key"));
				nvram_pf_set(prefix, "key4", nvram_pf_safe_get(prefix_wlc, "wep_key"));
			}
			nvram_pf_set(prefix, "crypto", nvram_pf_safe_get(prefix_wlc, "crypto"));
			nvram_pf_set(prefix, "wpa_psk", nvram_pf_safe_get(prefix_wlc, "wpa_psk"));
			nvram_pf_set(prefix, "radius_ipaddr", nvram_pf_safe_get(prefix_wlc, "radius_ipaddr"));
			nvram_pf_set(prefix, "radius_key", nvram_pf_safe_get(prefix_wlc, "radius_key"));
			nvram_pf_set(prefix, "radius_port", nvram_pf_safe_get(prefix_wlc, "radius_port"));
		}
	}
#endif
}

int wl_exist(char *ifname, int band)
{
	if(is_if_up(get_wififname(0)) && is_if_up(get_wififname(1)))
		return 1;
	return 0;
}

void set_wan_tag(char *interface)
{
#if defined(RTCONFIG_MULTICAST_IPTV)
//	const int sw_iptv = sw_based_iptv();
#endif
	int model, wan_vid;
	char wan_dev[10], port_id[7];

	model = get_model();
	wan_vid = nvram_get_int("switch_wan0tagid") & 0x0fff;

	snprintf(wan_dev, sizeof(wan_dev), "eth1.%d", wan_vid);

	switch(model) {
	case MODEL_BLUECAVE:
	case MODEL_RAX40:
		if(wan_vid && !nvram_match("switch_wantag", "none")) { /* config wan port */
			sprintf(port_id, "%d", wan_vid);
			eval("vconfig", "add", "eth1", port_id);
			sprintf(wan_dev, "eth1.%d", wan_vid);
			/* Set Wan port PRIO */
			if(nvram_invmatch("switch_wan0prio" , "0"))
				eval("vconfig", "set_egress_map", wan_dev, "0", nvram_get("switch_wan0prio"));
			set_wan_phy("");
			add_wan_phy(wan_dev);
			nvram_set("wan0_ifname", wan_dev);
			nvram_set("wan0_gw_ifname", wan_dev);
		}
		break;
	default:
		return;
	}

#ifdef RTCONFIG_MULTICAST_IPTV
	{
		int iptv_vid, voip_vid, iptv_prio, voip_prio, switch_stb;
		int mang_vid, mang_prio;
		char prio_str[16];

		iptv_vid  = nvram_get_int("switch_wan1tagid") & 0x0fff;
		voip_vid  = nvram_get_int("switch_wan2tagid") & 0x0fff;
		iptv_prio = nvram_get_int("switch_wan1prio") & 0x7;
		voip_prio = nvram_get_int("switch_wan2prio") & 0x7;
		mang_vid  = nvram_get_int("switch_wan3tagid") & 0x0fff;
		mang_prio = nvram_get_int("switch_wan3prio") & 0x7;
		switch_stb = nvram_get_int("switch_stb_x");
		if(nvram_match("switch_stb_x", "1") && nvram_match("switch_wantag", "none")){
			set_wan_phy("");
			add_wan_phy("br1");
			nvram_set("wan0_ifname", "br1");
			nvram_set("wan0_gw_ifname", "br1");
			eval("brctl", "addbr", "br1");
			eval("ifconfig", "br1", "up");
			eval("brctl", "addif", "br1", "eth1");
			eval("brctl", "delif", "br0", "eth0_4");
			eval("brctl", "addif", "br1", "eth0_4");
		}
		else if(!nvram_match("switch_stb_x", "2") || !nvram_match("switch_wantag", "none")){
			if(nvram_match("switch_stb_x", "3")){
				if(nvram_match("switch_wantag", "vodafone")){
					set_wan_phy("");
					add_wan_phy("br1");
					nvram_set("wan0_ifname", "br1");
					nvram_set("wan0_gw_ifname", "br1");
					eval("brctl", "addbr", "br1");
					eval("ifconfig", "br1", "up");
					eval("brctl", "delif", "br0", "eth0_1");
					sprintf(wan_dev, "eth1.%d", wan_vid);
					eval("ifconfig", wan_dev, "up");
					eval("ppacmd", "addwan", "-i", wan_dev);
					eval("brctl", "addif", "br1", wan_dev);
					eval("vconfig", "add", "eth0_1", port_id);
					sprintf(wan_dev, "eth0_1.%d", wan_vid);
					eval("ifconfig", wan_dev, "up");
					eval("brctl", "delif", "br0", "eth0_1");
					eval("ppacmd", "addlan", "-i", wan_dev);
					eval("brctl", "addif", "br1", wan_dev);
					sprintf(port_id, "%d", 101);
					eval("brctl", "addbr", "br2");
					eval("ppacmd", "addlan", "-i", "br2");
					eval("brctl", "stp", "br2", "on");
					eval("ifconfig", "br2", "up");
					eval("vconfig", "add", "eth1", port_id);
					sprintf(wan_dev, "eth1.%d", 101);
					eval("ifconfig", wan_dev, "up");
					eval("ppacmd", "addwan", "-i", wan_dev);
					eval("brctl", "addif", "br2", wan_dev);
					eval("vconfig", "add", "eth0_1", port_id);
					sprintf(wan_dev, "eth0_1.%d", 101);
					eval("ifconfig", wan_dev, "up");
					eval("ppacmd", "addlan", "-i", wan_dev);
					eval("brctl", "addif", "br2", wan_dev);
					sprintf(port_id, "%d", voip_vid);
					eval("brctl", "addbr", "br3");
					eval("ppacmd", "addlan", "-i", "br3");
					eval("brctl", "stp", "br3", "on");
					eval("ifconfig", "br3", "up");
					eval("vconfig", "add", "eth1", port_id);
					sprintf(wan_dev, "eth1.%d", voip_vid);
					if(nvram_invmatch("switch_wan2prio", "0"))
						eval("vconfig", "set_egress_map", wan_dev, "0", nvram_get("switch_wan2prio"));
					eval("ifconfig", wan_dev, "up");
					eval("ppacmd", "addwan", "-i", wan_dev);
					eval("brctl", "addif", "br3", wan_dev);
					eval("vconfig", "add", "eth0_1", port_id);
					sprintf(wan_dev, "eth0_1.%d", voip_vid);
					eval("ifconfig", wan_dev, "up");
					eval("ppacmd", "addlan", "-i", wan_dev);
					eval("brctl", "addif", "br3", wan_dev);
					eval("brctl", "delif", "br0", "eth0_2");
					eval("brctl", "addif", "br3", "eth0_2");
				}else if(nvram_match("switch_wantag", "m1_fiber") || nvram_match("switch_wantag", "maxis_fiber_sp")){
					sprintf(port_id, "%d", voip_vid);
					_dprintf("vlan entry: %s\n", port_id);
					eval("vconfig", "add", "eth1", port_id);
					eval("brctl", "addbr", "br1");
					eval("ppacmd", "addlan", "-i", "br1");
					eval("brctl", "stp", "br1", "on");
					eval("ifconfig", "br1", "up");
					sprintf(wan_dev, "eth1.%d", voip_vid);
					if(nvram_invmatch("switch_wan2prio", "0"))
						eval("vconfig", "set_egress_map", wan_dev, "0", nvram_get("switch_wan2prio"));
					eval("ifconfig", wan_dev, "up");
					eval("ppacmd", "addwan", "-i", wan_dev);
					eval("brctl", "addif", "br1", wan_dev);
					eval("vconfig", "add", "eth0_2", port_id);
					sprintf(wan_dev, "eth0_2.%d", voip_vid);
					eval("ifconfig", wan_dev, "up");
					eval("ppacmd", "addlan", "-i", wan_dev);
					eval("brctl", "delif", "br0", "eth0_2");
					eval("brctl", "addif", "br1", wan_dev);
				}else{
					if(nvram_match("switch_wantag", "maxis_fiber")){
						sprintf(port_id, "%d", voip_vid);
						_dprintf("vlan entry: %s\n", port_id);
						eval("vconfig", "add", "eth1", port_id);
						eval("brctl", "addbr", "br1");
						eval("ppacmd", "addlan", "-i", "br1");
						eval("brctl", "stp", "br1", "on");
						eval("ifconfig", "br1", "up");
						sprintf(wan_dev, "eth1.%d", 821);
						eval("ifconfig", wan_dev, "up");
						eval("ppacmd", "addwan", "-i", wan_dev);
						eval("brctl", "addif", "br1", wan_dev);
						eval("vconfig", "add", "eth0_2", port_id);
						sprintf(wan_dev, "eth0_2.%d", 821);
						eval("ifconfig", wan_dev, "up");
						eval("ppacmd", "addlan", "-i", wan_dev);
						eval("brctl", "delif", "br0", "eth0_2");
						eval("brctl", "addif", "br1", wan_dev);
						sprintf(port_id, "%d", 822);
						_dprintf("vlan entry: %s\n", port_id);
						eval("vconfig", "add", "eth1", port_id);
						eval("brctl", "addbr", "br2");
						eval("ppacmd", "addlan", "-i", "br2");
						eval("brctl", "stp", "br2", "on");
						eval("ifconfig", "br2", "up");
						sprintf(wan_dev, "eth1.%d", 822);
						eval("ifconfig", wan_dev, "up");
						eval("ppacmd", "addwan", "-i", wan_dev);
						eval("brctl", "addif", "br2", wan_dev);
						eval("vconfig", "add", "eth0_2", port_id);
						sprintf(wan_dev, "eth0_2.%d", 822);
						eval("ifconfig", wan_dev, "up");
						eval("ppacmd", "addlan", "-i", wan_dev);
						eval("brctl", "addif", "br2", wan_dev);
					}else{
						if(nvram_match("switch_wantag", "none")){
							set_wan_phy("");
							add_wan_phy("br1");
							nvram_set("wan0_ifname", "br1");
							nvram_set("wan0_gw_ifname", "br1");
							eval("brctl", "addbr", "br1");
							eval("ifconfig", "br1", "up");
							eval("brctl", "addif", "br1", "eth1");
						}else{
							sprintf(port_id, "%d", voip_vid);
							_dprintf("vlan entry: %s\n", port_id);
							eval("vconfig", "add", "eth1", port_id);
							eval("brctl", "addbr", "br1");
							eval("ppacmd", "addlan", "-i", "br1");
							eval("brctl", "stp", "br1", "on");
							eval("ifconfig", "br1", "up");
							sprintf(wan_dev, "eth1.%d", voip_vid);
							if(nvram_invmatch("switch_wan2prio", "0"))
								eval("vconfig", "set_egress_map", wan_dev, "0", nvram_get("switch_wan2prio"));
							eval("ifconfig", wan_dev, "up");
							eval("ppacmd", "addwan", "-i", wan_dev);
							eval("brctl", "addif", "br1", wan_dev);
						}
						eval("brctl", "delif", "br0", "eth0_2");
						eval("brctl", "addif", "br1", "eth0_2");
					}
				}
			}else if(nvram_match("switch_stb_x", "4")){
				if(nvram_match("switch_wantag", "meo")){
					set_wan_phy("");
					add_wan_phy("br1");
					nvram_set("wan0_ifname", "br1");
					nvram_set("wan0_gw_ifname", "br1");
					eval("brctl", "addbr", "br1");
					eval("ifconfig", "br1", "up");
					sprintf(wan_dev, "eth1.%d", wan_vid);
					eval("ifconfig", wan_dev, "up");
					eval("brctl", "addif", "br1", wan_dev);
					eval("vconfig", "add", "eth0_1", port_id);
					sprintf(wan_dev, "eth0_1.%d", wan_vid);
					eval("ifconfig", wan_dev, "up");
					eval("ppacmd", "addlan", "-i", wan_dev);
					eval("brctl", "delif", "br0", "eth0_1");
					eval("brctl", "addif", "br1", wan_dev);
				}else{
					if(nvram_match("switch_wantag", "none") || nvram_match("switch_wantag", "hinet")){
						set_wan_phy("");
						add_wan_phy("br1");
						nvram_set("wan0_ifname", "br1");
						nvram_set("wan0_gw_ifname", "br1");
						eval("brctl", "addbr", "br1");
						eval("ifconfig", "br1", "up");
						eval("brctl", "addif", "br1", "eth1");
					}else{
						sprintf(port_id, "%d", iptv_vid);
						_dprintf("vlan entry: %s\n", port_id);
						eval("vconfig", "add", "eth1", port_id);
						eval("brctl", "addbr", "br1");
						eval("ppacmd", "addlan", "-i", "br1");
						if(!nvram_match("switch_wantag", "unifi_home"))
							eval("brctl", "stp", "br1", "on");
						eval("ifconfig", "br1", "up");
						sprintf(wan_dev, "eth1.%d", iptv_vid);
						if(nvram_invmatch("switch_wan1prio", "0"))
							eval("vconfig", "set_egress_map", wan_dev, "0", nvram_get("switch_wan1prio"));
						eval("ifconfig", wan_dev, "up");
						eval("ppacmd", "addwan", "-i", wan_dev);
						eval("brctl", "addif", "br1", wan_dev);
					}
					eval("brctl", "delif", "br0", "eth0_1");
					eval("brctl", "addif", "br1", "eth0_1");
				}
			}else if(nvram_match("switch_stb_x", "5")){
				if(nvram_match("switch_wantag", "none")){
					set_wan_phy("");
					add_wan_phy("br1");
					nvram_set("wan0_ifname", "br1");
					nvram_set("wan0_gw_ifname", "br1");
					eval("brctl", "addbr", "br1");
					eval("ifconfig", "br1", "up");
					eval("brctl", "addif", "br1", "eth1");
					eval("brctl", "delif", "br0", "eth0_3");
					eval("brctl", "addif", "br1", "eth0_3");
					eval("brctl", "delif", "br0", "eth0_4");
					eval("brctl", "addif", "br1", "eth0_4");
				}
			}else if(nvram_match("switch_stb_x", "6")){
			}else if(nvram_match("switch_stb_x", "2")){
				set_wan_phy("");
				add_wan_phy("br1");
				nvram_set("wan0_ifname", "br1");
				nvram_set("wan0_gw_ifname", "br1");
				eval("brctl", "addbr", "br1");
				eval("ifconfig", "br1", "up");
				eval("brctl", "addif", "br1", "eth1");
				eval("brctl", "delif", "br0", "eth0_3");
				eval("brctl", "addif", "br1", "eth0_3");
			}else{
				if(nvram_match("switch_wantag", "singtel_mio")){
					sprintf(port_id, "%d", voip_vid);
					_dprintf("vlan entry: %s\n", port_id);
					eval("vconfig", "add", "eth1", port_id);
					eval("brctl", "addbr", "br1");
					eval("ppacmd", "addlan", "-i", "br1");
					eval("brctl", "stp", "br1", "on");
					eval("ifconfig", "br1", "up");
					sprintf(wan_dev, "eth1.%d", voip_vid);
					if(nvram_invmatch("switch_wan2prio", "0"))
						eval("vconfig", "set_egress_map", wan_dev, "0", nvram_get("switch_wan2prio"));
					eval("ifconfig", wan_dev, "up");
					eval("ppacmd", "addwan", "-i", wan_dev);
					eval("brctl", "addif", "br1", wan_dev);
					eval("vconfig", "add", "eth0_2", port_id);
					sprintf(wan_dev, "eth0_2.%d", voip_vid);
					eval("ifconfig", wan_dev, "up");
					eval("ppacmd", "addlan", "-i", wan_dev);
					eval("brctl", "delif", "br0", "eth0_2");
					eval("brctl", "addif", "br1", wan_dev);
					if(iptv_vid){
						if(iptv_vid != voip_vid){
							sprintf(port_id, "%d", iptv_vid);
							_dprintf("vlan entry: %s\n", port_id);
							eval("vconfig", "add", "eth1", port_id);
							eval("brctl", "addbr", "br2");
							eval("ppacmd", "addlan", "-i", "br2");
							eval("brctl", "stp", "br2", "on");
							eval("ifconfig", "br2", "up");
							sprintf(wan_dev, "eth1.%d", iptv_vid);
							if(nvram_invmatch("switch_wan1prio", "0"))
								eval("vconfig", "set_egress_map", wan_dev, "0", nvram_get("switch_wan1prio"));
							eval("ifconfig", wan_dev, "up");
							eval("ppacmd", "addwan", "-i", wan_dev);
							eval("brctl", "addif", "br2", wan_dev);
						}
						eval("brctl", "delif", "br0", "eth0_1");
						eval("brctl", "addif", "br2", "eth0_1");
					}
				}else{
					if(nvram_match("switch_wantag", "none")){
						set_wan_phy("");
						add_wan_phy("br1");
						nvram_set("wan0_ifname", "br1");
						nvram_set("wan0_gw_ifname", "br1");
						eval("brctl", "addbr", "br1");
						eval("ifconfig", "br1", "up");
						eval("brctl", "addif", "br1", "eth1");
						eval("brctl", "delif", "br0", "eth0_1");
						eval("brctl", "addif", "br1", "eth0_1");
						if(iptv_vid){
							if(iptv_vid != voip_vid){
								sprintf(port_id, "%d", iptv_vid);
								_dprintf("vlan entry: %s\n", port_id);
								eval("vconfig", "add", "eth1", port_id);
								eval("brctl", "addbr", "br2");
								eval("ppacmd", "addlan", "-i", "br2");
								eval("brctl", "stp", "br2", "on");
								eval("ifconfig", "br2", "up");
								sprintf(wan_dev, "eth1.%d", iptv_vid);
								if(nvram_invmatch("switch_wan1prio", "0"))
									eval("vconfig", "set_egress_map", wan_dev, "0", nvram_get("switch_wan1prio"));
								eval("ifconfig", wan_dev, "up");
								eval("ppacmd", "addwan", "-i", wan_dev);
								eval("brctl", "addif", "br2", wan_dev);
							}
							eval("brctl", "delif", "br0", "eth0_1");
							eval("brctl", "addif", "br2", "eth0_1");
						}
					}else{
						if(voip_vid == 0){
							if(iptv_vid){
								if(iptv_vid != voip_vid){
									sprintf(port_id, "%d", iptv_vid);
									_dprintf("vlan entry: %s\n", port_id);
									eval("vconfig", "add", "eth1", port_id);
									eval("brctl", "addbr", "br1");
									eval("ppacmd", "addlan", "-i", "br1");
									eval("brctl", "stp", "br1", "on");
									eval("ifconfig", "br1", "up");
									sprintf(wan_dev, "eth1.%d", iptv_vid);
									if(nvram_invmatch("switch_wan1prio", "0"))
										eval("vconfig", "set_egress_map", wan_dev, "0", nvram_get("switch_wan1prio"));
									eval("ifconfig", wan_dev, "up");
									eval("ppacmd", "addwan", "-i", wan_dev);
									eval("brctl", "addif", "br1", wan_dev);
								}
								eval("brctl", "delif", "br0", "eth0_1");
								eval("brctl", "addif", "br1", "eth0_1");
								eval("brctl", "delif", "br0", "eth0_2");
								eval("brctl", "addif", "br1", "eth0_2");
							}
						}else{
							sprintf(port_id, "%d", voip_vid);
							_dprintf("vlan entry: %s\n", port_id);
							eval("vconfig", "add", "eth1", port_id);
							eval("brctl", "addbr", "br1");
							eval("ppacmd", "addlan", "-i", "br1");
							eval("brctl", "stp", "br1", "on");
							eval("ifconfig", "br1", "up");
							sprintf(wan_dev, "eth1.%d", voip_vid);
							if(nvram_invmatch("switch_wan2prio", "0"))
								eval("vconfig", "set_egress_map", wan_dev, "0", nvram_get("switch_wan2prio"));
							eval("ifconfig", wan_dev, "up");
							eval("ppacmd", "addwan", "-i", wan_dev);
							eval("brctl", "addif", "br1", wan_dev);
							if(iptv_vid){
								if(iptv_vid != voip_vid){
									sprintf(port_id, "%d", iptv_vid);
									_dprintf("vlan entry: %s\n", port_id);
									eval("vconfig", "add", "eth1", port_id);
									eval("brctl", "addbr", "br2");
									eval("ppacmd", "addlan", "-i", "br2");
									eval("brctl", "stp", "br2", "on");
									eval("ifconfig", "br2", "up");
									sprintf(wan_dev, "eth1.%d", iptv_vid);
									if(nvram_invmatch("switch_wan1prio", "0"))
										eval("vconfig", "set_egress_map", wan_dev, "0", nvram_get("switch_wan1prio"));
									eval("ifconfig", wan_dev, "up");
									eval("ppacmd", "addwan", "-i", wan_dev);
									eval("brctl", "addif", "br2", wan_dev);
								}
								eval("brctl", "delif", "br0", "eth0_1");
								eval("brctl", "addif", "br2", "eth0_1");
								eval("brctl", "delif", "br0", "eth0_2");
								eval("brctl", "addif", "br2", "eth0_2");
							}
						}
					}
				}
			}
		}

		if (switch_stb >= 7) {
			if (iptv_vid) { /* config IPTV on wan port */
				_dprintf("*** Multicast IPTV: config IPTV on wan port ***\n");
				sprintf(port_id, "%d", iptv_vid);
				eval("vconfig", "add", "eth1", port_id);
				snprintf(wan_dev, sizeof(wan_dev), "eth1.%d", iptv_vid);
				eval("ifconfig", wan_dev, "up");
				eval("ppacmd", "addwan", "-i", wan_dev);
				nvram_set("wan10_ifname", wan_dev);
			}
		}
		if (switch_stb >= 8) {
			if (voip_vid) { /* config voip on wan port */
				_dprintf("*** Multicast IPTV: config VOIP on wan port ***\n");
				sprintf(port_id, "%d", voip_vid);
				eval("vconfig", "add", "eth1", port_id);
				snprintf(wan_dev, sizeof(wan_dev), "eth1.%d", voip_vid);
				eval("ifconfig", wan_dev, "up");
				eval("ppacmd", "addwan", "-i", wan_dev);
				nvram_set("wan11_ifname", wan_dev);
			}
		}
	}
#endif
}

int start_thermald(void)
{
	return 0;
}
