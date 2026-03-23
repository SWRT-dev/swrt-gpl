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
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sysinfo.h>
#include <wlutils.h>
#include <bcmdevs.h>
#include <shared.h>
#include <lantiq.h>
#include <flash_mtd.h>

#define MKNOD(name,mode,dev)		if (mknod(name,mode,dev))		perror("## mknod " name)

extern void check_ubi_partition(void);
extern void gen_config_sh(void);

static void __load_wifi_driver(int testmode)
{
//deprecated function
	;
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

void init_devs(void)
{
	;
}

int init_devs_defer(void)
{
	int status;
	MKNOD("/dev/nvram", S_IFCHR | 0666, makedev(228, 0));

	if((status = WEXITSTATUS(modprobe("nvram_linux"))))
		printf("## modprobe(nvram_linux) fail status(%d)\n", status);
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
	fprintf(stderr, "RAX40: init gpio\n");
	gpio_dir(0, GPIO_DIR_IN);
	gpio_dir(30, GPIO_DIR_IN);
	gpio_dir(10, GPIO_DIR_OUT);
	set_gpio(10, 0);
	gpio_dir(14, GPIO_DIR_OUT);
	set_gpio(14, 0);
	gpio_dir(21, GPIO_DIR_OUT);
	set_gpio(21, 0);
	gpio_dir(22, GPIO_DIR_OUT);
	set_gpio(22, 0);
	gpio_dir(31, GPIO_DIR_OUT);
	set_gpio(31, 0);
	gpio_dir(34, GPIO_DIR_OUT);
	set_gpio(34, 0);
	gpio_dir(35, GPIO_DIR_OUT);
	set_gpio(35, 0);
	gpio_dir(43, GPIO_DIR_OUT);
	set_gpio(43, 0);
#else
#error fix me
#endif
	check_ubi_partition();
	_dprintf("start_jffs2() start\n");
	start_ubifs();
	_dprintf("start_jffs2() end\n");
	return 0;
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
#elif defined(RAX40)
	fprintf(stderr, "RAX40: init gpio again\n");
	gpio_dir(0, GPIO_DIR_IN);
	gpio_dir(30, GPIO_DIR_IN);
	gpio_dir(10, GPIO_DIR_OUT);
	set_gpio(10, 0);
	gpio_dir(14, GPIO_DIR_OUT);
	set_gpio(14, 0);
	gpio_dir(21, GPIO_DIR_OUT);
	set_gpio(21, 0);
	gpio_dir(22, GPIO_DIR_OUT);
	set_gpio(22, 0);
	gpio_dir(31, GPIO_DIR_OUT);
	set_gpio(31, 0);
	gpio_dir(34, GPIO_DIR_OUT);
	set_gpio(34, 0);
	gpio_dir(35, GPIO_DIR_OUT);
	set_gpio(35, 0);
	gpio_dir(43, GPIO_DIR_OUT);
	set_gpio(43, 0);
#else
#error fix me
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
	system("cp -f /opt/lantiq/wave/images/backup/cal_wlan0.bin /tmp/cal_wlan0.bin");
	system("cp -f /opt/lantiq/wave/images/backup/cal_wlan1.bin /tmp/cal_wlan1.bin");
	system("rm -f /tmp/cal_eeprom.tar.gz");
	system("cd /tmp; tar czf cal_eeprom.tar.gz cal_wlan0.bin cal_wlan1.bin");
	system("upgrade /tmp/cal_eeprom.tar.gz wlanconfig 0 0");
	system("rm -f /tmp/cal_eeprom.tar.gz");
#elif defined(RAX40)
#else
#error fix me
#endif
	return 0;
}

void init_others(void)
{
	struct stat cal_stat;
	pid_t pid;

	gen_config_sh();
	init_gpio_again();
#if defined(BLUECAVE)
	system("cp -R /lib/firmware/ar3k /tmp/");
#endif
	_dprintf("--------- create link /tmp/wireless/ to /rom/opt/ -----------\n");
	system("cd /tmp/wireless; ln -s /rom/opt/beerocks beerocks");
	system("cd /tmp/wireless; ln -s /rom/opt/errorhd.cfg errorhd.cfg");
	system("cd /tmp/wireless; ln -s /rom/opt/lantiq lantiq");
	_dprintf("--------- extract fapi database -----------\n");
	system("cd /tmp/; rm -rf lantiq_wave; tar zxf /rom/opt/lantiq/wave.tgz; mv wave lantiq_wave");
	_dprintf("--------- create link /tmp/wireless/ to /rom/opt/ done -----------\n");
	system("cp /opt/lantiq/wave/scripts/fapi_wlan_wave_lib_common.sh /tmp/");
	system("cp /opt/lantiq/wave/images/* /tmp/");
	system("cp /rom/opt/lantiq/etc/wave_components.ver /etc/");
	system("read_img wlanconfig /tmp/cal_eeprom.tar.gz");
	if(stat("/tmp/cal_eeprom.tar.gz", &cal_stat) || cal_stat.st_blocks < 801)
		write_default_cal();
	else
	{
#if defined(K3C)
#elif defined(BLUECAVE)
		system("rm -f /tmp/cal_wlan*.bin");
		system("tar zxf /tmp/cal_eeprom.tar.gz -C /tmp/");
#elif defined(RAX40)
#else
#error fix me
#endif
	}
	system("rm -f /tmp/cal_eeprom.tar.gz");
	if(!stat("/tmp/cal_wlan1.bin", &cal_stat))
		system("rm -f /tmp/cal_wlan2.bin; mv /tmp/cal_wlan1.bin /tmp/cal_wlan2.bin");
	MKNOD("/dev/switch_api/0", S_IFCHR | 0640, makedev(81, 0));
	MKNOD("/dev/switch_api/1", S_IFCHR | 0640, makedev(81, 1));
	system("load_gphy_firmware_preinit.sh");
	MKNOD("/dev/ifx_mei", S_IFCHR | 0666, makedev(105, 0));
	MKNOD("/dev/ifx_ppa", S_IFCHR | 0666, makedev(181, 0));
	system("insmod drv_ifxos");
	system("insmod drv_event_logger");
	system("insmod directconnect_datapath");
	system("insmod ltq_eth_drv_xrx500");
	system("insmod dc_mode0-xrx500");
	system("insmod mcast_helper");
	system("insmod macvlan");
	system("insmod ltq_pae_hal");
	system("insmod ltq_tmu_hal_drv");
	system("insmod ltq_directpath_datapath");
	system("insmod ppa_api");
	system("insmod ppa_api_proc");
	system("insmod ltq_mpe_hal_drv");
	system("insmod ppa_api_tmplbuf");
	system("insmod ppa_api_sw_accel_mod");
	system("insmod ltq_temp");
	system("insmod ltq_pmcu");
	//modprobe("tntfs");
	if(!pids("udevd"))
	{
		char *udevd_argv[] = {"udevd", "--daemon", NULL};
		_eval(udevd_argv, NULL, 0, &pid);
		logmessage("udevd", "daemon is started");
	}
	nvram_set("ctf_disable", nvram_safe_get("ctf_disable_force"));
	system("ppacmd init -n 30");
	system("ppacmd addlan -i eth0_1");
	system("ppacmd addlan -i eth0_2");
	system("ppacmd addlan -i eth0_3");
#if !defined(K3C)
	system("ppacmd addlan -i eth0_4");
#endif
	system("ppacmd addlan -i br0");
#if defined(RTCONFIG_AMAS)
	if(!aimesh_re_mode())
    	eval("iptables", "-t", "mangle", "-A", "INPUT", "-p", "tcp", "-m", "state", "--state", "ESTABLISHED", "-j", "EXTMARK", "--set-mark", "0x80000000/0x80000000");
#endif
	nvram_set("wave_action", "1");

	if (repeater_mode() || mediabridge_mode() || aimesh_re_node())
	{
		modprobe("l2nat");
		f_write_string("/proc/sys/net/bridge/bridge-nf-call-iptables", "0", 0, 0);
	}
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

void enable_jumbo_frame(void)
{
	char buf[] = "8000XXX";

	if(nvram_contains_word("rc_support", "switchctrl"))
	{
		if(nvram_get_int("jumbo_frame_enable"))
		{
			snprintf(buf, sizeof(buf), "%d", 8000);
			eval("ifconfig", "eth0_1", "mtu", buf);
			eval("ifconfig", "eth0_2", "mtu", buf);
			eval("ifconfig", "eth0_3", "mtu", buf);
#if !defined(K3C)
			eval("ifconfig", "eth0_4", "mtu", buf);
#endif
		}
	}
}

void init_switch(void)
{
	_dprintf("init_switch: for now handle jumbo frame only\n");
	enable_jumbo_frame();
}

int switch_exist(void)
{
	int i, n;
#if defined(K3C)
	const char *eth[] = {"eth1", "eth0_1", "eth0_2", "eth0_3"};
#else
	const char *eth[] = {"eth1", "eth0_1", "eth0_2", "eth0_3", "eth0_4"};
#endif
	n = sizeof(eth) / sizeof(eth[0]);
	for(i = 0; i < n; i++){
		if(!is_if_up(eth[i]));
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
	int len, i;
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

void init_wl(void)
{
	_dprintf("[%s][%d] skip\n", __func__, __LINE__);
}

void fini_wl(void)
{
	_dprintf("[%s][%d] skip\n", __func__, __LINE__);
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
	unsigned char factory_var_buf[256];

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

	memset(buffer, 0, sizeof(buffer));
	FRead(buffer, OFFSET_BOOT_VER, 4);
	snprintf(blver, sizeof(blver), "%s-0%c-0%c-0%c-0%c", trim_r(productid), buffer[0],
		buffer[1], buffer[2], buffer[3]);
	nvram_set("blver", trim_r(blver));
	system("nvram set blver=`uboot_env --get --name bl_ver`");
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
	char prefix[] = "wlXXXXXXX_", prefix_wlc[] = "wlXXXXXXX_";
	char tmp[128];

	if(aimesh_re_mode())
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
