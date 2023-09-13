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
 *
 * Copyright 2018-2023, SWRT.
 * Copyright 2018-2023, paldier <paldier@hotmail.com>.
 * Copyright 2018-2023, lostlonger<lostlonger.g@gmail.com>.
 * All Rights Reserved.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "rc.h"
#include <shared.h>
#include <version.h>
#include <shutils.h>
#include <json.h>
#include "swrt.h"
#include <curl/curl.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#ifndef MNT_DETACH
#define MNT_DETACH	0x00000002
#endif

#if defined(RTAC82U)
void fix_jffs_size(void);
#endif

#if defined(RTCONFIG_SOFTCENTER)
static void firmware_ver(void)
{
    char tmp[6] = {0};
    strncpy(tmp, RT_FWVER, 5);//5.x.x[beta]
	doSystem("dbus set softcenter_firmware_version='%s'",tmp);
}
#endif

void swrt_insmod(){
	eval("insmod", "nfnetlink");
	eval("insmod", "ip_set");
	eval("insmod", "ip_set_bitmap_ip");
	eval("insmod", "ip_set_bitmap_ipmac");
	eval("insmod", "ip_set_bitmap_port");
	eval("insmod", "ip_set_hash_ip");
	eval("insmod", "ip_set_hash_ipport");
	eval("insmod", "ip_set_hash_ipportip");
	eval("insmod", "ip_set_hash_ipportnet");
	eval("insmod", "ip_set_hash_ipmac");
	eval("insmod", "ip_set_hash_ipmark");
	eval("insmod", "ip_set_hash_net");
	eval("insmod", "ip_set_hash_netport");
	eval("insmod", "ip_set_hash_netiface");
	eval("insmod", "ip_set_hash_netnet");
	eval("insmod", "ip_set_hash_netportnet");
	eval("insmod", "ip_set_hash_mac");
	eval("insmod", "ip_set_list_set");
	eval("insmod", "nf_tproxy_core");
	eval("insmod", "nf_tproxy_ipv4");
	eval("insmod", "nf_tproxy_ipv6");
	eval("insmod", "xt_TPROXY");
	eval("insmod", "xt_set");
	modprobe("cifs");
}

void swrt_init_pre()
{
	_dprintf("############################ SWRT init #################################\n");
#if defined(RTCONFIG_SOFTCENTER)
	nvram_set("sc_wan_sig", "0");
	nvram_set("sc_nat_sig", "0");
	nvram_set("sc_mount_sig", "0");
	nvram_set("sc_services_start_sig", "0");
	nvram_set("sc_services_stop_sig", "0");
	nvram_set("sc_unmount_sig", "0");
	mkdir("/tmp/upload", 0777);
#endif
#if defined(RTCONFIG_ENTWARE)
	nvram_set("entware_wan_sig", "0");
	nvram_set("entware_stop_sig", "0");
#endif
#if defined(RTAC82U)
	fix_jffs_size();
#endif
	swrt_insmod();
	if(swrt_init_model)
		swrt_init_model();
#if defined(RTCONFIG_ROG_UI)
	nvram_set("swrt_rog", "1");
#endif
#if defined(RTCONFIG_TUF_UI)
	nvram_set("swrt_tuf", "1");
#endif
	if(!nvram_get("modelname"))
//non asus
#if defined(SBRAC1900P)
		nvram_set("modelname", "SBRAC1900P");
#elif defined(EA6700)
		nvram_set("modelname", "EA6700");
#elif defined(DIR868L)
		nvram_set("modelname", "DIR868L");
#elif defined(R6300V2)
		nvram_set("modelname", "R6300V2");
#elif defined(F9K1118)
		nvram_set("modelname", "F9K1118");
#elif defined(SBRAC3200P)
		nvram_set("modelname", "SBRAC3200P");
#elif defined(K3)
		nvram_set("modelname", "K3");
#elif defined(XWR3100)
		nvram_set("modelname", "XWR3100");
#elif defined(R7000P)
		nvram_set("modelname", "R7000P");
#elif defined(R8500)
		nvram_set("modelname", "R8500");
#elif defined(RAC2V1S)
		nvram_set("modelname", "RAC2V1S");
#elif defined(R8000P)
		nvram_set("modelname", "R8000P");
#elif defined(RAX20)
		nvram_set("modelname", "RAX20");
#elif defined(RAX70)
		nvram_set("modelname", "RAX70");
#elif defined(RAX80)
		nvram_set("modelname", "RAX80");
#elif defined(RAX120)
		nvram_set("modelname", "RAX120");
#elif defined(RAX200)
		nvram_set("modelname", "RAX200");
#elif defined(TY6201_BCM)
		nvram_set("modelname", "TY6201_BCM");
#elif defined(TY6201_RTK)
		nvram_set("modelname", "TY6201_RTK");
#elif defined(TY6202)
		nvram_set("modelname", "TY6202");
#elif defined(RGMA2820A)
		nvram_set("modelname", "RGMA2820A");
#elif defined(RGMA2820B)
		nvram_set("modelname", "RGMA2820B");
#elif defined(TYAX5400)
		nvram_set("modelname", "TYAX5400");
#elif defined(K3C)
		nvram_set("modelname", "K3C");
#elif defined(MR60)
		nvram_set("modelname", "MR60");
#elif defined(MS60)
		nvram_set("modelname", "MS60");
#elif defined(SWRT360V6)
		nvram_set("modelname", "360V6");
#elif defined(GLAX1800)
		nvram_set("modelname", "GLAX1800");
#elif defined(JDCAX1800)
		nvram_set("modelname", "JDCAX1800");
#elif defined(RMAX6000)
		nvram_set("modelname", "RMAX6000");
#elif defined(RMAC2100)
		nvram_set("modelname", "RMAC2100");
#elif defined(R6800)
		nvram_set("modelname", "R6800");
#elif defined(PGBM1)
		nvram_set("modelname", "PGBM1");
#elif defined(JCGQ10PRO)
		nvram_set("modelname", "JCGQ10PRO");
#elif defined(H3CTX1801)
		nvram_set("modelname", "H3CTX1801");
#elif defined(XMCR660X)
		nvram_set("modelname", "XMCR660X");
#elif defined(TY6201PRO)
		nvram_set("modelname", "TY6201PRO");
#elif defined(RGMA3062)
		nvram_set("modelname", "RGMA3062");
#elif defined(JCGQ20)
		nvram_set("modelname", "JCGQ20");
//asus
#elif defined(RTAC68U)
		nvram_set("modelname", "RTAC68U");
#elif defined(RTAC3200)
		nvram_set("modelname", "RTAC3200");
#elif defined(RTAC3100)
		nvram_set("modelname", "RTAC3100");
#elif defined(RTAC88U)
		nvram_set("modelname", "RTAC88U");
#elif defined(RTAC5300)
		nvram_set("modelname", "RTAC5300");
#elif defined(RTAC86U)
		nvram_set("modelname", "RTAC86U");
#elif defined(GTAC2900)
		nvram_set("modelname", "GTAC2900");
#elif defined(GTAC5300)
		nvram_set("modelname", "GTAC5300");
#elif defined(RTAX53U)
		nvram_set("modelname", "RTAX53U");
#elif defined(RTAX54)
		nvram_set("modelname", "RTAX54");
#elif defined(RTAX55) || defined(RTAX1800)
		nvram_set("modelname", "RTAX55");
#elif defined(RTAX56U)
		nvram_set("modelname", "RTAX56U");
#elif defined(RTAX57)
		nvram_set("modelname", "RTAX57");
#elif defined(RTAX58U) || defined(RTAX3000)
		nvram_set("modelname", "RTAX58U");
#elif defined(RTAX58U_V2)
		nvram_set("modelname", "RTAX58UV2");
#elif defined(TUFAX3000)
		nvram_set("modelname", "TUFAX3000");
#elif defined(TUFAX3000_V2)
		nvram_set("modelname", "TUFAX3000V2");
#elif defined(TUFAX5400)
		nvram_set("modelname", "TUFAX5400");
#elif defined(TUFAX5400_V2)
		nvram_set("modelname", "TUFAX5400V2");
#elif defined(GSAX3000)
		nvram_set("modelname", "GSAX3000");
#elif defined(GSAX5400)
		nvram_set("modelname", "GSAX5400");
#elif defined(RTAX68U)
		nvram_set("modelname", "RTAX68U");
#elif defined(RTAX82U)
		nvram_set("modelname", "RTAX82U");
#elif defined(RTAX82U_V2)
		nvram_set("modelname", "RTAX82UV2");
#elif defined(RTAX86U)
		nvram_set("modelname", "RTAX86U");
#elif defined(RTAX86U_PRO)
		nvram_set("modelname", "RTAX86UPRO");
#elif defined(RTAX88U)
		nvram_set("modelname", "RTAX88U");
#elif defined(RTAX88U_PRO)
		nvram_set("modelname", "RTAX88UPRO");
#elif defined(RTAX92U)
		nvram_set("modelname", "RTAX92U");
#elif defined(GTAX6000)
		nvram_set("modelname", "GTAX6000");
#elif defined(GTAX11000)
		nvram_set("modelname", "GTAX11000");
#elif defined(GTAX11000_PRO)
		nvram_set("modelname", "GTAX11000PRO");
#elif defined(GTAXE11000)
		nvram_set("modelname", "GTAXE11000");
#elif defined(GTAXE16000)
		nvram_set("modelname", "GTAXE16000");
#elif defined(BLUECAVE)
		nvram_set("modelname", "BLUECAVE");
#elif defined(RTAC82U)
		nvram_set("modelname", "RTACRH17");
#elif defined(RTAX89U)
		nvram_set("modelname", "RTAX89X");
#elif defined(RTAC85P)
		nvram_set("modelname", "RTAC85P");
#elif defined(TUFAC1750)
		nvram_set("modelname", "TUFAC1750");
#elif defined(RTAC95U)
		nvram_set("modelname", "ZENWIFICT8");
#elif defined(RTAX56XD4)
		nvram_set("modelname", "ZENWIFIXD4");
#elif defined(RTAX82_XD6)
		nvram_set("modelname", "ZENWIFIXD6");
#elif defined(RTAX82_XD6S)
		nvram_set("modelname", "ZENWIFIXD6S");
#elif defined(XD6_V2)
		nvram_set("modelname", "ZENWIFIXD6V2");
#elif defined(RTAX95Q)
		nvram_set("modelname", "ZENWIFIXT8");
#elif defined(RTAXE95Q)
		nvram_set("modelname", "ZENWIFIET8");
#elif defined(XT12)
		nvram_set("modelname", "ZENWIFIXT12");
#elif defined(ET12)
		nvram_set("modelname", "ZENWIFIET12");
#elif defined(XD4PRO)
		nvram_set("modelname", "ZENWIFIXD4PRO");
#elif defined(XT8PRO)
		nvram_set("modelname", "ZENWIFIXT9");
#elif defined(XT8_V2)
		nvram_set("modelname", "ZENWIFIXT8V2");
#elif defined(ET8PRO)
		nvram_set("modelname", "ZENWIFIET9");
#elif defined(ET8_V2)
		nvram_set("modelname", "ZENWIFIET8V2");
#elif defined(PLAX56_XP4)
		nvram_set("modelname", "ZENWIFIXP4");
#elif defined(ETJ)
		nvram_set("modelname", "ZENWIFIETJ");
#elif defined(GT10)
		nvram_set("modelname", "GT6");
#elif defined(RTAXE7800)
		nvram_set("modelname", "RTAXE7800");
#endif
	if(!nvram_get("swrt_beta"))
		nvram_set("swrt_beta", "0");
}

#if defined(K3)
static int k3screena(void){
	_dprintf("....k3screen start a....\n");
	char fwver[32];
	FILE *fpu;
	if (!nvram_get("hd_version"))
		nvram_set("hd_version", "A1/A2");

	if (!nvram_get("product"))
		nvram_set("product", "K3");
	if(nvram_get_int("sw_mode") == 1){
		sprintf(fwver, "%s_%s", nvram_get("buildno"), nvram_get("extendno"));
		nvram_set("fw_version", fwver);
		nvram_set("sw_version", fwver);
		nvram_set("wan_ifname", "vlan2");
	} else {
		sprintf(fwver, "AP:%s", nvram_safe_get("lan_ipaddr"));
		nvram_set("fw_version", fwver);
		nvram_set("sw_version", fwver);
		nvram_set("wan_ifname", "vlan1");
	}
	if (!nvram_get("wan_pppoe_ifname"))
		nvram_set("wan_pppoe_ifname", "ppp0");
	if (nvram_get_int("bsd_role") != 0)
		nvram_set_int("bsd_role", 0);
	if (!nvram_get("pingcheck"))
		nvram_set("pingcheck", "1");
	if (nvram_get_int("get_wan_port_status") != 1)
		nvram_set_int("get_wan_port_status", 1);
	if (!nvram_get("screen_time"))
		nvram_set("screen_time", "20");
	if (!nvram_get("screen_2G5G_pwd_en"))
		nvram_set("screen_2G5G_pwd_en", "1");
	if (!nvram_get("city_ch"))
		nvram_set("city_ch", "北京");
	if (!nvram_get("county_ch"))
		nvram_set("county_ch", "北京");
	if (!nvram_get("city_id"))
		nvram_set("city_id", "WX4FBXXFKE4F");
	if (!nvram_get("vis_ssid_enable"))
		nvram_set("vis_ssid_enable", "1");
	if (!nvram_get("screen_guest_pwd_en"))
		nvram_set("screen_guest_pwd_en", "1");
	if (!nvram_get("vis_ssid"))
		nvram_set("vis_ssid", "ASUS_GUEST");
	if (!nvram_get("vis_ssid_pwd"))
		nvram_set("vis_ssid_pwd", "1234567890");
	nvram_commit();
	doSystem("killall -q -9 phi_speed wl_cr uhmi k3screenctrl update_weather 2>/dev/null");
	if ((fpu = fopen("/tmp/uhmi.sh", "w"))){
		fprintf(fpu, "#!/bin/sh\n");
		fprintf(fpu, "mkdir -p /jffs/softcenter/lib\n");
		fprintf(fpu, "ln -sf /usr/share/libwlcr.so /jffs/softcenter/lib/libwlcr.so\n");
		fprintf(fpu, "devmem 0x1800c1c1 32 0x00001f0f\n");
		fprintf(fpu, "[ -n \"$(echo $LD_LIBRARY_PATH |grep \"jffs\")\" ] || export LD_LIBRARY_PATH=/jffs/softcenter/lib:/lib:/usr/lib\n");
		fprintf(fpu, "phi_speed &\n");
		fprintf(fpu, "wl_cr &\n");
		fprintf(fpu, "uhmi &\n");
		fclose(fpu);
	}
	doSystem("chmod +x /tmp/uhmi.sh");
	doSystem("/tmp/uhmi.sh &");
	doSystem("echo '#!/bin/sh' > /tmp/update_weather");
	doSystem("echo 'while [ 1 ]' >> /tmp/update_weather");
	doSystem("echo 'do' >> /tmp/update_weather");
	doSystem("echo 'weather update_weather' >> /tmp/update_weather");
	doSystem("echo 'sleep 3600' >> /tmp/update_weather");
	doSystem("echo 'done' >> /tmp/update_weather");
	doSystem("chmod +x /tmp/update_weather");
	doSystem("/tmp/update_weather &");
	_dprintf("....k3screen ok....\n");
	return 0;
}

static int k3screenb(void){
	_dprintf("....k3screen start b....\n");
	char *timeout;
	doSystem("mkdir -p /tmp/k3screenctrl");
	doSystem("killall -q -9 phi_speed wl_cr uhmi k3screenctrl update_weather k3screend 2>/dev/null");
	//doSystem("/usr/sbin/k3screend &");
	//doSystem("chmod +x /tmp/k3screenctrl/*.sh");
	if (nvram_get_int("k3screen_timeout")==1)
		timeout = "-m0";
	else
		timeout = "-m30";
	char *k3screenctrl_argv[] = { "k3screenctrl", timeout,NULL };
	char *k3screend_argv[] = { "k3screend",NULL };
	pid_t pid;
	_eval(k3screend_argv, NULL, 0, &pid);
	_eval(k3screenctrl_argv, NULL, 0, &pid);
	_dprintf("....k3screen ok....\n");
	return 0;
}
int start_k3screen(void){
	logmessage("K3", "屏幕支援程序开始启动");
	if (!nvram_get("k3screen")){
		nvram_set("k3screen", "b");
		nvram_commit();
	}
	_dprintf("....k3screen start....\n");
	if ((strcmp(nvram_get("k3screen"), "A")==0) || (strcmp(nvram_get("k3screen"), "a")==0))
		return k3screena();
	else if ((strcmp(nvram_get("k3screen"), "B")==0) || (strcmp(nvram_get("k3screen"), "b")==0))
		return k3screenb();
	else {
		nvram_set("k3screen", "b");
		nvram_commit();
		return k3screenb();
		}
	return 0;
}
#endif

#if defined(TUFAX3000) || defined(RTAX58U) || defined(RTAX82U)
void enable_4t4r_ax58(void)
{
//ensure that the hardware support 4t4r
	if(!strcmp(nvram_get("1:sw_rxchain_mask"), "0xf") ){
//4t4r
		nvram_set("1:sw_txchain_mask", "0xf");
		nvram_commit();
	} else {
//2t2r
		nvram_set("1:sw_txchain_mask", "0x9");
		nvram_commit();
	}
}
void enable_4t4r(void)
{
//hack asus watchdog and modify this to enable 4t4r
//ensure that the hardware support 4t4r
	if(!strcmp(cfe_nvram_get("1:sw_rxchain_mask"), "0xf") && strcmp(cfe_nvram_get("1:sw_txchain_mask"), "0xf")){
		doSystem("envrams");
		doSystem("envram set 1:sw_txchain_mask=0xf");
		doSystem("envram commit");
	}
}
#endif

void swrt_init_post(){
	_dprintf("############################ SWRT init done #################################\n");
#if defined(RTCONFIG_SOFTCENTER)
//cifs
	if(nvram_match("sc_mount","2")){
		char *path = nvram_get("sc_cifs_url");
		char *user = nvram_safe_get("sc_cifs_user");
		char *pw = nvram_safe_get("sc_cifs_pw");
		char opt[410] = {0};
		snprintf(opt, sizeof(opt), "username=%s%s%s", *user ? user : "guest", *pw ? ",password=" : "", pw);
		mount(path, "/jffs/softcenter", "cifs", MS_NOATIME|MS_NODIRATIME, opt);
//jffs > 30MB
	}else if(!f_exists("/jffs/softcenter/scripts/ks_tar_install.sh") && nvram_match("sc_mount","0")){
		doSystem("/usr/sbin/jffsinit.sh &");
		logmessage("Softcenter/软件中心", "Installing/开始安装......");
		logmessage("Softcenter/软件中心", "Wait a minute/1分钟后完成安装");
		_dprintf("....softcenter ok....\n");
	}else if(f_exists("/jffs/softcenter/scripts/ks_tar_install.sh") && nvram_match("sc_mount","0"))
		nvram_set("sc_installed","1");
//usb
	//else if (!f_exists("/jffs/softcenter/scripts/ks_tar_intall.sh") && nvram_match("sc_mount","1"))
		//nvram_set("sc_installed","0");
	if(f_exists("/jffs/.asusrouter")){
		unlink("/jffs/.asusrouter");
		doSystem("sed -i '/softcenter-wan.sh/d' /jffs/scripts/wan-start");
		doSystem("sed -i '/softcenter-net.sh/d' /jffs/scripts/nat-start");
		doSystem("sed -i '/softcenter-mount.sh/d' /jffs/scripts/post-mount");
	}
	firmware_ver();
#endif
#if defined(RTCONFIG_QCA) || defined(RTCONFIG_RALINK)
	if(!nvram_get("bl_ver"))
		nvram_set("bl_ver", nvram_safe_get("blver"));
#elif defined(RTCONFIG_LANTIQ)
#if !defined(K3C)
	if(!nvram_get("bl_ver"))
		doSystem("nvram set bl_ver=`uboot_env --get --name bl_ver`");
#endif
#endif
#if defined(R8000P)
	nvram_set("ping_target", "www.taobao.com");
#endif
	nvram_commit();
#if defined(K3)
	start_k3screen();
#endif
#if defined(SWRT_VER_MAJOR_B)
	del_rc_support("amasRouter");
	del_rc_support("amas");
#endif
	gen_swrtid();
#if defined(R8000P)
    add_rc_support("uu_accel");
#endif
#if defined(RAX120)
	rax120_lanled();
#endif
#if defined(RTCONFIG_SWRT_LED)
	swrt_ledon();//to fix LED state
#endif
#if defined(RTCONFIG_ENTWARE)
	gen_arch_conf();
#endif
#if defined(MS60)
	if(aimesh_re_node() && !d_exists("/sys/class/net/br0/brif/eth0")){
		eval("brctl", "addif", "br0", "eth0");
	}
#endif
}


#define FWUPDATE_DBG(fmt,args...) \
        if(1) { \
                char info[1024]; \
                snprintf(info, sizeof(info), "echo \"[FWUPDATE][%s:(%d)]"fmt"\" >> /tmp/webs_upgrade.log", __FUNCTION__, __LINE__, ##args); \
                system(info); \
        }

int str_split(char* buf, char** s, int s_size) {
	int curr = 0;
	char* token = strtok(buf, ".");
	while(token && curr < s_size) {
		s[curr++] = token;
		token = strtok(NULL, ".");
	}
	return curr;
}

int versioncmp(char *cur_fwver, char *fwver) {
	char buf1[20] = {0}, buf2[20] = {0};
	char* s1[20] = {0};
	char* s2[20] = {0};
	char* end;
	int i, n1, n2, s1_len, s2_len, s_max, rlt;
	strncpy(buf1, cur_fwver, strlen(cur_fwver));
	strncpy(buf2, fwver, strlen(fwver));
	//fprintf(stderr, "%s\n", buf1);
	//fprintf(stderr, "%s\n", buf2);

	s1_len = str_split(buf1, s1, 10);
	s2_len = str_split(buf2, s2, 10);

	//fprintf(stderr, "s1=%d, s2=%d\n", s1_len, s2_len);

	s_max = (s1_len > s2_len ? s1_len: s2_len);

	rlt = 0;
	for(i = 0; i < s_max; i++) {
		if(!s1[i]) {
			s1[i] = "0";
		}
		if(!s2[i]) {
			s2[i] = "0";
		}

		n1 = strtol(s1[i], &end, 10);
		if(*end) {
			//parse error, but not break hear
			rlt = 1;
		}

		n2 = strtol(s2[i], &end, 10);
		if(*end) {
			if (1 == rlt) {
				//both error
				rlt = 0;
				break;
			}
			rlt = -1;
			break;
		}

		if(1 == rlt) {
			break;
		}

		if (n1 == n2) {
			continue;
		} else if(n1 < n2) {
			rlt = 1;
			break;
		} else {
			rlt = -1;
			break;
		}
	}
	return rlt;
}

size_t wirtefunc(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	return fwrite(ptr, size, nmemb, stream);
}

int curl_download_swrt(const char *url, const char *file_path, long timeout, const int feature)
{
	FILE *fp;
	CURL *curl = NULL;
	CURLcode ret;
	struct curl_httppost *post = NULL;
	struct curl_httppost *last = NULL;

	if(url == NULL || file_path == NULL)
		return -1;

	curl_global_init(CURL_GLOBAL_ALL);
	curl = curl_easy_init();

	if(!curl)
		return -3;

	unlink(file_path);
	if ((fp = fopen(file_path, "wb")) == NULL)
		return -2;

	if(feature == CURL_FIRMWARE){
		curl_formadd(&post, &last, CURLFORM_COPYNAME, "modelname", CURLFORM_COPYCONTENTS, nvram_safe_get("modelname"), CURLFORM_END);
		curl_formadd(&post, &last, CURLFORM_COPYNAME, "beta", CURLFORM_COPYCONTENTS, nvram_safe_get("swrt_beta"), CURLFORM_END);
	}
	if(post != NULL)
		curl_easy_setopt(curl, CURLOPT_HTTPPOST, post);
	curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_setopt(curl, CURLOPT_PROTOCOLS, CURLPROTO_HTTPS);
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, timeout);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, wirtefunc);

	//curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1);
	curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);

	ret = curl_easy_perform(curl);
	fclose(fp);
	curl_easy_cleanup(curl);

	if (ret != CURLE_OK)
		return -4;
	return 0;
}

int swrt_firmware_check_update_main(int argc, char *argv[])
{
	int download;
	char url[100];
	char log[200];
	char serverurl[]="https://update.paldier.com";
	char serverupdate[]="firmware.php";
	char localupdate[]="/tmp/wlan_update.txt";
	char releasenote[]="/tmp/release_note0.txt";
	char fwver[10], commitnum[10], tag[10];
	char cur_fwver[10];
	char info[100];
	char *modelname = nvram_safe_get("modelname");
	nvram_set("webs_state_update", "0");
	nvram_set("webs_state_flag", "0");
	nvram_set("webs_state_error", "0");
	nvram_set("webs_state_odm", "0");
	nvram_set("webs_state_url", "");
#ifdef RTCONFIG_AMAS
	nvram_set("cfg_check", "0");
	nvram_set("cfg_upgrade", "0");
#endif
	unlink("/tmp/webs_upgrade.log");
	unlink("/tmp/wlan_update.txt");
	unlink("/tmp/release_note0.txt");
	memset(cur_fwver, 0, sizeof(cur_fwver));
    strncpy(cur_fwver, RT_FWVER, 5);//5.x.x[beta]

	snprintf(url, sizeof(url), "%s/%s", serverurl, serverupdate);

	FWUPDATE_DBG("---- update dl_path_info for general %s/%s ----", serverurl, serverupdate);
	download=curl_download_swrt(url, localupdate, 8, CURL_FIRMWARE);
	if(!download)
	{
		char md5[33] = {0}, fwname[100] = {0};
		char *buffer = read_whole_file(localupdate);
		//1aae93b2b84bf87ba000125740ec4397  GTAX11000_R5.1.8_30041-gab7352c_cferom_ubi.w 
		if(NULL != buffer){
			sscanf(buffer, "%s %s", md5, fwname);
			free(buffer);
			if(!strncmp(fwname, "beta/", 5)){
				buffer = &fwname[5 + strlen(modelname) + 1];
			}else{
				buffer = &fwname[strlen(modelname) + 1];
			}
			sscanf(buffer, "%[BRX0-9.]_%[0-9]-%[a-z0-9]", fwver, commitnum, tag);
			_dprintf("%s#%s#%s#%s\n", modelname, fwver, commitnum, tag);
			if(!strcmp(modelname, nvram_safe_get("modelname")) && ((nvram_match("swrt_beta", "0") && !strstr(buffer, "beta")) 
				|| (nvram_match("swrt_beta", "1") && strstr(buffer, "beta")))){
				if(strstr(fwver, "B") || strstr(fwver, "R") || strstr(fwver, "X")){
					_dprintf("%s#%s\n", fwver, cur_fwver);
					if(versioncmp(cur_fwver, fwver + 1) == 1 || versioncmp(RT_FWEXTENDNO, commitnum) == 1){
						nvram_set("webs_state_url", "");
						if(!strcmp(nvram_get("firmver"), "3.0.0.4"))
							snprintf(info, sizeof(info), "3004_%s_%s_%s_%s-%s", nvram_get("buildno"), modelname, fwver, commitnum, tag);
						else if(!strcmp(nvram_get("firmver"), "4.0.0.4"))
							snprintf(info, sizeof(info), "4004_%s_%s_%s_%s-%s", nvram_get("buildno"), modelname, fwver, commitnum, tag);
						else
							snprintf(info, sizeof(info), "5004_%s_%s_%s_%s-%s", nvram_get("buildno"), modelname, fwver, commitnum, tag);
						FWUPDATE_DBG("---- current version : %s ----", nvram_get("extendno"));
						FWUPDATE_DBG("---- productid : %s_%s_%s-%s ----", modelname, fwver, commitnum, tag);
						nvram_set("webs_state_info", info);
						nvram_set("webs_state_REQinfo", info);
						nvram_set("webs_state_flag", "1");
						nvram_set("webs_state_update", "1");
						nvram_set("swrt_fw_name", fwname);
						nvram_set("swrt_fw_md5", md5);
#ifdef RTCONFIG_AMAS
//						nvram_set("cfg_check", "9");
//						nvram_set("cfg_upgrade", "0");
#endif
						memset(url, 0, sizeof(url));
						memset(log, 0, sizeof(log));
						char releasenote_file[100];
						snprintf(releasenote_file, sizeof(releasenote_file), "%s_%s_%s_note.zip", nvram_safe_get("productid"), nvram_get("webs_state_info"), nvram_safe_get("preferred_lang"));
						snprintf(url, sizeof(url), "%s/%s", serverurl, releasenote_file);
						//snprintf(log, sizeof(log), "echo \"[FWUPDATE]---- download real release note %s/%s ----\" >> /tmp/webs_upgrade.log", serverurl, releasenote_file);
						//system(log);
						FWUPDATE_DBG("---- download real release note %s/%s ----", serverurl, releasenote_file);
						download = curl_download_swrt(url, releasenote, 8, CURL_OTHER);
						if(download != 0 ){
							memset(url, 0, sizeof(url));
							snprintf(releasenote_file, sizeof(releasenote_file), "%s_%s_US_note.zip", nvram_safe_get("productid"), nvram_get("webs_state_info"));
							snprintf(url, sizeof(url), "%s/%s", serverurl, releasenote_file);
							//snprintf(log, sizeof(log), "echo \"[FWUPDATE]---- download real release note %s/%s ----\" >> /tmp/webs_upgrade.log", serverurl, releasenote_file);
							//system(log);
							FWUPDATE_DBG("---- download real release note %s/%s ----", serverurl, releasenote_file);
							curl_download_swrt(url, releasenote, 8, CURL_OTHER);
						}
						FWUPDATE_DBG("---- firmware check update finish ----");
						return 0;
#if 0
						if(strstr(nt_center,"nt_center")){
							if((!nvram_get("webs_last_info"))||(strcmp(nvram_get("webs_last_info"), nvram_get("webs_state_info")))){
								system("Notify_Event2NC \"$SYS_FW_NWE_VERSION_AVAILABLE_EVENT\" \"{\\\"fw_ver\\\":\\\"$update_webs_state_info\\\"}\"");
								nvram_set("webs_last_info", nvram_get("webs_state_info"));
							}
						}
#endif
					}
				}
			}
		}
	}

	if(!strcmp(nvram_get("firmver"), "3.0.0.4"))
		snprintf(info, sizeof(info), "3004_%s_%s_%s-%s", nvram_get("buildno"), modelname, fwver, tag);
	else if(!strcmp(nvram_get("firmver"), "4.0.0.4"))
		snprintf(info, sizeof(info), "4004_%s_%s_%s-%s", nvram_get("buildno"), modelname, fwver, tag);
	else
		snprintf(info, sizeof(info), "5004_%s_%s_%s-%s", nvram_get("buildno"), modelname, fwver, tag);
	nvram_set("webs_state_url", "");
	nvram_set("webs_state_flag", "3");
	nvram_set("webs_state_error", "2");
	nvram_set("webs_state_odm", "0");
	nvram_set("webs_state_update", "1");
	nvram_set("webs_state_info", info);
	nvram_set("webs_state_REQinfo", info);
	nvram_set("webs_state_upgrade", "");
#ifdef RTCONFIG_AMAS
	nvram_set("cfg_check", "9");
	nvram_set("cfg_upgrade", "0");
#endif
	nvram_unset("swrt_fw_name");
	nvram_unset("swrt_fw_md5");
	FWUPDATE_DBG("---- firmware check update finish ----");
	return 0;
}

#ifdef RTCONFIG_UUPLUGIN
void start_uu(void)
{
	stop_uu();

	if(getpid()!=1) {
		notify_rc("start_uu");
		return;
	}

	if(nvram_get_int("uu_enable"))
#if defined(RTCONFIG_SWRT_UU)
		exec_uu_swrt();
#else
		exec_uu();
#endif
}

void stop_uu(void)
{
	doSystem("killall uuplugin_monitor.sh");
	if (pidof("uuplugin") > 0)
		doSystem("killall uuplugin");
}
#endif
#if defined(RTCONFIG_SWRT_UU)
void exec_uu_swrt()
{
	FILE *fp;
	char buf[128];
	int download,i;
	char *g, *gg;
	char p[2][100];
	if(nvram_get_int("sw_mode") == 1){
		add_rc_support("uu_accel");
		mkdir("/tmp/uu", 0755);
		download = system("wget -t 2 -T 30 --dns-timeout=120 --header=Accept:text/plain -q --no-check-certificate 'https://router.uu.163.com/api/script/monitor?type=asuswrt-merlin' -O /tmp/uu/script_url");
		if (!download){
			_dprintf("download uuplugin script info successfully\n");
			if ((fp = fopen("/tmp/uu/script_url", "r"))!=NULL){
				fgets(buf, 128, fp);
				fclose(fp);
				unlink("/tmp/uu/script_url");
				i=0;
				g = strdup(buf);
				gg = strtok(g, ",");
				while(gg != NULL){
						strcpy(p[i], gg);
						i++;
						++download;
						gg = strtok(NULL, ",");
				}
				if (download > 0){
					_dprintf("URL: %s\n",p[0]);
					_dprintf("MD5: %s\n",p[1]);
					if ( !doSystem("wget -t 2 -T 30 --dns-timeout=120 --header=Accept:text/plain -q --no-check-certificate %s -O /tmp/uu/uuplugin_monitor.sh", p[0]))
					{
						_dprintf("download uuplugin script successfully\n");
						if ((fp = fopen("/tmp/uu/uuplugin_monitor.config", "w"))){
							fprintf(fp, "router=asuswrt-merlin\n");
							fprintf(fp, "model=\n");
							fclose(fp);
						}
						if((fp=popen("md5sum /tmp/uu/uuplugin_monitor.sh | sed 's/[ ][ ]*/ /g' | cut -d' ' -f1", "r")))
						{
							memset(buf,'\0',sizeof(buf));
							if((fread(buf, 1, 128, fp)))
							{
								buf[32]='\0';
								buf[33]='\0';
								if ( !strcasecmp(buf, p[1]))
								{
									pid_t pid;
									char *uu_argv[] = { "/tmp/uu/uuplugin_monitor.sh", NULL };
									_dprintf("prepare to execute uuplugin stript...\n");
									chmod("/tmp/uu/uuplugin_monitor.sh", 0755);
									_eval(uu_argv, NULL, 0, &pid);
								}
							}
							pclose(fp);
						}
					}
				}
			}
			free(g);
		}
	}
}
#endif

#if defined(RTCONFIG_SOFTCENTER)
void softcenter_trigger(int sig)
{
	pid_t pid;
	char path[100], action[10], sc[]="/jffs/softcenter/bin";
	if(SOFTCENTER_WAN == sig){
		snprintf(path, sizeof(path), "%s/softcenter-wan.sh", sc);
		snprintf(action, sizeof(action), "start");
	} else if (SOFTCENTER_NAT == sig){
		snprintf(path, sizeof(path), "%s/softcenter-net.sh", sc);
		snprintf(action, sizeof(action), "start_nat");
	} else if (SOFTCENTER_MOUNT == sig){
		snprintf(path, sizeof(path), "%s/softcenter-mount.sh", sc);
		snprintf(action, sizeof(action), "start");
	} else if (SOFTCENTER_SERVICES_START == sig){
		snprintf(path, sizeof(path), "%s/softcenter-services.sh", sc);
		snprintf(action, sizeof(action), "start");
	} else if (SOFTCENTER_SERVICES_STOP == sig){//only reboot
		char *eval_argv[] = { "/jffs/softcenter/bin/softcenter-mount.sh", "stop", NULL };
		_eval(eval_argv, NULL, 0, &pid);
		eval_argv[0] = "/jffs/softcenter/bin/softcenter-services.sh";
		_eval(eval_argv, NULL, 0, &pid);
		eval_argv[0] = "/jffs/softcenter/bin/softcenter-wan.sh";
		_eval(eval_argv, NULL, 0, &pid);
		return;
	} else if (SOFTCENTER_UNMOUNT == sig){
		snprintf(path, sizeof(path), "%s/softcenter-unmount.sh", sc);
		snprintf(action, sizeof(action), "unmount");
	} else if (SOFTCENTER_CIFS_MOUNT == sig){
		char *path = nvram_get("sc_cifs_url");
		char *user = nvram_safe_get("sc_cifs_user");
		char *pw = nvram_safe_get("sc_cifs_pw");
		char opt[999] = {0};
		snprintf(opt, sizeof(opt), "username=%s%s%s", *user ? user : "guest", *pw ? ",password=" : "", pw);
		mount(path, "/jffs/softcenter", "cifs", MS_NOATIME|MS_NODIRATIME, opt);
		return;
	} else {
		logmessage("Softcenter", "sig=%d, bug?",sig);
		return;
	}
	char *eval_argv[] = { path, action, NULL };
	_eval(eval_argv, NULL, 0, &pid);
}
#endif

#if defined(RTCONFIG_ENTWARE)
void stop_entware(void)
{
	nvram_set_int("entware_busy", 0);
	nvram_unset("entware_app");
	nvram_unset("entware_action");
	nvram_unset("entware_arg");
}

void init_entware(void)
{
	if(strlen(nvram_safe_get("apps_state_install")) || nvram_match("webdav_aidisk", "1")){
		logmessage("[Entware]", "Downloadmaster/Aicloud is installed already! Entware can't install!\n");
		return;
	}
	if(nvram_get_int("entware_mount") == 0)
		return;
	/* /opt->/tmp/opt->/jffs/opt->/tmp/mnt/sda/opt */
	unlink("/tmp/opt");
	symlink("/jffs/opt", "/tmp/opt");
#if defined(RTCONFIG_LANTIQ)
	doSystem("mount --bind /tmp/opt /opt");
#endif
	stop_entware();
}

#define ENTWARE_ACT_INSTALL		1
#define ENTWARE_ACT_UPDATE		2
#define ENTWARE_ACT_REMOVE		4
#define ENTWARE_ACT_START		8
#define ENTWARE_ACT_STOP		16
#define	ENTWARE_ACT_MASK (ENTWARE_ACT_INSTALL | ENTWARE_ACT_UPDATE | ENTWARE_ACT_REMOVE)
#define	ENTWARE_ACT_MASK2 (ENTWARE_ACT_START | ENTWARE_ACT_STOP)
#define ENTWARE_SERVER "bin.entware.net"
#define ENTWARE_MIRROR "mirrors.bfsu.edu.cn/entware"

void start_entware(void)
{
	char *ent_app, *ent_arg;
	int ent_action;
	char cmd[128], app[32];
	
	if (nvram_get_int("entware_busy") != 1)
		return;
	
	nvram_set_int("entware_busy", 2);
	ent_action = nvram_get_int("entware_action");
	ent_app = nvram_safe_get("entware_app");
	ent_arg = nvram_safe_get("entware_arg");
	
	if (strcmp(ent_app, "entware") == 0)
	{
		if (ent_action & ENTWARE_ACT_INSTALL)
		{
			FILE *fp = NULL;
			if(nvram_get_int("entware_mount") == 0){//uninstall
#if defined(RTCONFIG_LANTIQ)
#ifndef MNT_DETACH
#define MNT_DETACH	0x00000002
#endif
			if (umount("/opt"))
				umount2("/opt", MNT_DETACH);
#endif
				doSystem("rm -rf /jffs/opt");
				doSystem("rm -rf /tmp/mnt/%s/opt", nvram_safe_get("entware_disk"));
			}else if((fp = fopen("/tmp/installentware.sh", "w"))){
				fprintf(fp, "#!/bin/sh\n");
				fprintf(fp, "TYPE='%s'\n", ent_arg);
				fprintf(fp, "unset LD_LIBRARY_PATH\n");
				fprintf(fp, "unset LD_PRELOAD\n");
				fprintf(fp, "ARCH=%s\n", nvram_safe_get("entware_arch"));
				fprintf(fp, "LOADER=ld-linux.so.3\n");
				fprintf(fp, "GLIBC=2.27\n");
				fprintf(fp, "for folder in bin etc lib/opkg tmp var/lock\n");
				fprintf(fp, "do\n");
				fprintf(fp, "  if [ -d \"/opt/$folder\" ]; then\n");
				fprintf(fp, "    echo \"Warning: Folder /opt/$folder exists!\"\n");
				fprintf(fp, "    echo 'Warning: If something goes wrong please clean /opt folder and try again.'\n");
				fprintf(fp, "  else\n");
				fprintf(fp, "    mkdir -p /opt/$folder\n");
				fprintf(fp, "  fi\n");
				fprintf(fp, "done\n");
				fprintf(fp, "echo 'Info: Opkg package manager deployment...'\n");
				fprintf(fp, "URL=http://%s/${ARCH}/installer\n", nvram_match("preferred_lang", "CN") ? ENTWARE_MIRROR : ENTWARE_SERVER);
				fprintf(fp, "wget $URL/opkg -O /opt/bin/opkg\n");
				fprintf(fp, "chmod 755 /opt/bin/opkg\n");
				fprintf(fp, "echo 'Info: Basic packages installation...'\n");
				fprintf(fp, "/opt/bin/opkg update\n");
				fprintf(fp, "if [ $TYPE = 'alternative' ]; then\n");
				fprintf(fp, "  /opt/bin/opkg install --force-space busybox\n");
				fprintf(fp, "fi\n");
				fprintf(fp, "/opt/bin/opkg install --force-space entware-opt\n");
				fprintf(fp, "chmod 777 /opt/tmp\n");
				fprintf(fp, "for file in passwd group shells shadow gshadow; do\n");
				fprintf(fp, "  if [ $TYPE = 'generic' ]; then\n");
				fprintf(fp, "    if [ -f /etc/$file ]; then\n");
				fprintf(fp, "      ln -sf /etc/$file /opt/etc/$file\n");
				fprintf(fp, "    else");
				fprintf(fp, "      [ -f /opt/etc/$file.1 ] && cp /opt/etc/$file.1 /opt/etc/$file\n");
				fprintf(fp, "    fi\n");
				fprintf(fp, "   else\n");
				fprintf(fp, "    if [ -f /opt/etc/$file.1 ]; then\n");
				fprintf(fp, "      cp /opt/etc/$file.1 /opt/etc/$file\n");
				fprintf(fp, "    fi\n");
				fprintf(fp, "  fi\n");
				fprintf(fp, "done\n");
				fprintf(fp, "[ -f /etc/localtime ] && ln -sf /etc/localtime /opt/etc/localtime\n");
				fprintf(fp, "echo 'Info: Congratulations!'\n");
				fprintf(fp, "echo 'Info: If there are no errors above then Entware was successfully initialized.'\n");
				fprintf(fp, "echo 'Info: Add /opt/bin & /opt/sbin to $PATH variable'\n");
				fprintf(fp, "echo 'Info: Add \"/opt/etc/init.d/rc.unslung start\" to startup script for Entware services to start'\n");
				fprintf(fp, "if [ $TYPE = 'alternative' ]; then\n");
				fprintf(fp, "  echo 'Info: Use ssh server from Entware for better compatibility.'\n");
				fprintf(fp, "fi\n");
				fprintf(fp, "echo 'Info: Found a Bug? Please report at https://github.com/Entware/Entware/issues'\n");
				fclose(fp);
				system("chmod +x /tmp/installentware.sh");
				system("mkdir -p /opt/etc");
				snprintf(cmd, sizeof(cmd), "wget http://%s/%s/installer/opkg.conf -O /opt/etc/opkg.conf", 
					nvram_match("preferred_lang", "CN") ? ENTWARE_MIRROR : ENTWARE_SERVER, nvram_get("entware_arch"));
				system(cmd);
				if(nvram_match("preferred_lang", "CN"))
					system("sed -i 's|http://bin.entware.net|http://mirrors.bfsu.edu.cn/entware|g' /opt/etc/opkg.conf");
				system("/tmp/installentware.sh");
				nvram_set("entware_installed", "1");
#if defined(RTCONFIG_HND_ROUTER_AX_6756)
				eval("ln", "-sf", "/bin/rm", "/opt/bin/rm");//protect system files
#endif
			}else
				nvram_set("entware_installed", "0");
		}
		else if (ent_action & ENTWARE_ACT_UPDATE)
		{
			system("/opt/bin/opkg update");
			system("/opt/bin/opkg upgrade");
		}
		else
		{
			logmessage("[Entware]", "Unregistered action\n");
		}
	}
	else if (ent_action & ENTWARE_ACT_MASK2)
	{
		snprintf(app, sizeof(app), "/opt/etc/init.d/%s", ent_app);
		if (f_exists(app))
		{
			if (ent_action & ENTWARE_ACT_STOP)
			{
				snprintf(cmd, sizeof(cmd), "%s stop", app);
				system(cmd);
			}
			if (ent_action & ENTWARE_ACT_START)
			{
				snprintf(cmd, sizeof(cmd), "%s start", app);
				system(cmd);
			}
		}
		else
		{
			logmessage("[Entware]", "Nonexistent service\n");
		}
	}
	else if (ent_action & ENTWARE_ACT_MASK)
	{
		if (ent_app)
		{
			if (ent_action & ENTWARE_ACT_REMOVE)
			{
				snprintf(cmd, sizeof(cmd), "/opt/bin/opkg %s remove %s --force-space", ent_arg, ent_app);
				system(cmd);
			}
			if (ent_action & ENTWARE_ACT_UPDATE)
			{
				system("/opt/bin/opkg update");
				snprintf(cmd, sizeof(cmd), "/opt/bin/opkg %s upgrade %s --force-space", ent_arg, ent_app);
				system(cmd);
			}
			if (ent_action & ENTWARE_ACT_INSTALL)
			{
				system("/opt/bin/opkg update");
				snprintf(cmd, sizeof(cmd), "/opt/bin/opkg %s install %s --force-space", ent_arg, ent_app);
				system(cmd);
			}
		}
	}
	else
	{
		logmessage("[Entware]", "Nonexistent app and Unregistered action\n");
	}
	nvram_set_int("entware_busy", 0);
	nvram_unset("entware_app");
	nvram_unset("entware_action");
	nvram_unset("entware_arg");
}

void gen_arch_conf(void)
{
	struct utsname uts;
	if(nvram_match("entware_arch", "") && uname(&uts) == 0){
		if(!strncmp(uts.machine, "armv7", 5)){
//			if(!strcmp(uts.release, "2.6.36.4brcmarm"))
//				nvram_set("entware_arch", "armv7sf-k2.6");
//			else
				nvram_set("entware_arch", "armv7sf-k3.2");
		}else if(!strcmp(uts.machine, "mips")){
			if(!strncmp(uts.release, "4.4", 3) || !strncmp(uts.release, "5.4", 3))
				nvram_set("entware_arch", "mipselsf-k3.4");
			else
				nvram_set("entware_arch", "mipssf-k3.4");//lantiq: 3.10/4.9
		}else if(!strcmp(uts.machine, "aarch64"))
			nvram_set("entware_arch", "aarch64-k3.10");
		else if(!strcmp(uts.machine, "x86_64"))
			nvram_set("entware_arch", "x64-k3.2");
	}
}
#endif

#if defined(R6800)
#define NETGEAR_BOARD_MAC 0xB0		//6
#define NETGEAR_BOARD_SN 0xB7		//42
#define NETGEAR_BOARD_PIN 0xE1		//12
#define NETGEAR_BOARD_DOMAIN 0xF2	//2
#define NETGEAR_BOARD_PCBA_SN 0xF6	//12
#define NETGEAR_BOARD_SSID 0x106	//20
#define NETGEAR_BOARD_PASSWD 0x120	//64
#define NETGEAR_BOARD_MODULE 0x200	//2
void show_boarddata(void)
{
	FILE *fp = NULL;
	int mtd_part = 0, mtd_size = 0, i;
	char dev_mtd[] = "/dev/mtdblockXXX";
	unsigned char factory_var_buf[256];

	mtd_getinfo("boarddata", &mtd_part, &mtd_size);
	snprintf(dev_mtd, sizeof(dev_mtd), "/dev/mtdblock%d", mtd_part);
	if((fp = fopen(dev_mtd, "rb")) != NULL){
		memset(factory_var_buf, 0, sizeof(factory_var_buf));
		fseek(fp, NETGEAR_BOARD_MAC, SEEK_SET);
		fread(factory_var_buf, 1, 6, fp);
		if(factory_var_buf[0] == 0x0 || factory_var_buf[0] == 0xff)
			printf("mac is invalid, type 'toolbox fix MAC AA1122334455' to fix it, 17 bytes\n");
		else
			printf("mac:%02X%02X%02X%02X%02X%02X\n", factory_var_buf[0], factory_var_buf[1], factory_var_buf[2], factory_var_buf[3], factory_var_buf[4], factory_var_buf[5]);
		memset(factory_var_buf, 0, sizeof(factory_var_buf));
		fseek(fp, NETGEAR_BOARD_SN, SEEK_SET);
		fread(factory_var_buf, 1, 42, fp);
		if(factory_var_buf[0] == 0x0 || factory_var_buf[0] == 0xff)
			printf("sn is invalid, type 'toolbox fix SN 123456789' to fix it\n");
		else{
			for(i = 0; i < 42; i++){
				if(factory_var_buf[i] == 0xff)
					factory_var_buf[i] = 0x0;
			}
			printf("sn:%s\n", factory_var_buf);
		}
		memset(factory_var_buf, 0, sizeof(factory_var_buf));
		fseek(fp, NETGEAR_BOARD_PIN, SEEK_SET);
		fread(factory_var_buf, 1, 12, fp);
		if(factory_var_buf[0] == 0x0 || factory_var_buf[0] == 0xff)
			printf("pin is invalid, type 'toolbox fix PIN 12345678' to fix it, 8 bytes\n");
		else{
			for(i = 0; i < 12; i++){
				if(factory_var_buf[i] == 0xff)
					factory_var_buf[i] = 0x0;
			}
			printf("pin:%s\n", factory_var_buf);
		}
		memset(factory_var_buf, 0, sizeof(factory_var_buf));
		fseek(fp, NETGEAR_BOARD_DOMAIN, SEEK_SET);
		fread(factory_var_buf, 1, 2, fp);
		if(factory_var_buf[0] == 0xff)
			printf("domain is invalid, type 'toolbox fix DOMAIN 11' to fix it, 2 bytes\n");
		else{
			for(i = 0; i < 2; i++){
				if(factory_var_buf[i] == 0xff)
					factory_var_buf[i] = 0x0;
			}
			printf("domain:%d\n", factory_var_buf[1]);
		}
		memset(factory_var_buf, 0, sizeof(factory_var_buf));
		fseek(fp, NETGEAR_BOARD_PCBA_SN, SEEK_SET);
		fread(factory_var_buf, 1, 12, fp);
		if(factory_var_buf[0] == 0x0 || factory_var_buf[0] == 0xff)
			printf("PCBA_SN is invalid, type 'toolbox fix PCBA_SN R.BZV8AE0A4D' to fix it, 12 bytes\n");
		else{
			for(i = 0; i < 12; i++){
				if(factory_var_buf[i] == 0xff)
					factory_var_buf[i] = 0x0;
			}
			printf("PCBA_SN:%s\n", factory_var_buf);
		}
		memset(factory_var_buf, 0, sizeof(factory_var_buf));
		fseek(fp, NETGEAR_BOARD_SSID, SEEK_SET);
		fread(factory_var_buf, 1, 20, fp);
		if(factory_var_buf[0] == 0x0 || factory_var_buf[0] == 0xff)
			printf("ssid is invalid, type 'toolbox fix SSID NETGEAR28' to fix it\n");
		else{
			for(i = 0; i < 20; i++){
				if(factory_var_buf[i] == 0xff)
					factory_var_buf[i] = 0x0;
			}
			printf("ssid:%s\n", factory_var_buf);
		}
		memset(factory_var_buf, 0, sizeof(factory_var_buf));
		fseek(fp, NETGEAR_BOARD_PASSWD, SEEK_SET);
		fread(factory_var_buf, 1, 64, fp);
		if(factory_var_buf[0] == 0x0 || factory_var_buf[0] == 0xff)
			printf("password is invalid, type 'toolbox fix PSWD slowjungle499' to fix it\n");
		else{
			for(i = 0; i < 64; i++){
				if(factory_var_buf[i] == 0xff)
					factory_var_buf[i] = 0x0;
			}
			printf("password:%s\n", factory_var_buf);
		}
		memset(factory_var_buf, 0, sizeof(factory_var_buf));
		fseek(fp, NETGEAR_BOARD_MODULE, SEEK_SET);
		fread(factory_var_buf, 1, 2, fp);
		if(factory_var_buf[0] == 0x0 || factory_var_buf[0] == 0xff)
			printf("MODULE is invalid, type 'toolbox fix MODULE 09' to fix it, 2 bytes\n");
		else{
			for(i = 0; i < 2; i++){
				if(factory_var_buf[i] == 0xff)
					factory_var_buf[i] = 0x0;
			}
			printf("MODULE:%s\n", factory_var_buf);
		}
		fclose(fp);
	}else
		printf("can't open boarddata\n");
}

void fix_boarddata(char *key, char *value)
{
	FILE *fp = NULL;
	int mtd_part = 0, mtd_size = 0, i;
	char dev_mtd[] = "/dev/mtdblockXXX";
	unsigned char factory_var_buf[256];

	if(key == NULL || value == NULL){
		printf("key or value is null\n");
		printf("for example:toolbox fix MODULE 09\n");
		return;
	}
	memset(factory_var_buf, 0, sizeof(factory_var_buf));
	mtd_getinfo("boarddata", &mtd_part, &mtd_size);
	snprintf(dev_mtd, sizeof(dev_mtd), "/dev/mtdblock%d", mtd_part);
	if(!strcmp(key, "MAC")){
		int l;
		unsigned char mac_binary[6];
		l = strlen(value);
		if(l != 17 && l != 12){
			printf("The mac is wrong:%s\ntoolbox fix MAC AABB22334455\n", value);
			return -1;
		}
		for(i=0; i < l; i++){
			if(!isxdigit(value[i]) && value[i] != ':' && value[i] != '-'){
				printf("The mac is wrong:%s\ntoolbox fix MAC AABB22334455\n", value);
				return -1;
			}
		}
		ether_atoe(value, mac_binary);
		if((fp = fopen(dev_mtd, "rb")) != NULL){
			fseek(fp, NETGEAR_BOARD_MAC, SEEK_SET);
			fwrite(mac_binary, 1, 6, fp);
			fclose(fp);
			
		}
	}else if(!strcmp(key, "SN")){
		snprintf(factory_var_buf, sizeof(factory_var_buf), "%s", value);
		for(i = 0; i < 42; i++){
			if(factory_var_buf[i] == 0x0)
				factory_var_buf[i] = 0xff;
		}
		if((fp = fopen(dev_mtd, "rb")) != NULL){
			fseek(fp, NETGEAR_BOARD_SN, SEEK_SET);
			fwrite(factory_var_buf, 1, 42, fp);
			fclose(fp);
		}
	}else if(!strcmp(key, "PIN")){
		snprintf(factory_var_buf, sizeof(factory_var_buf), "%s", value);
		for(i = 0; i < 12; i++){
			if(factory_var_buf[i] == 0x0 || i > 7)
				factory_var_buf[i] = 0xff;
		}
		if((fp = fopen(dev_mtd, "rb")) != NULL){
			fseek(fp, NETGEAR_BOARD_PIN, SEEK_SET);
			fwrite(factory_var_buf, 1, 12, fp);
			fclose(fp);
		}
	}else if(!strcmp(key, "DOMAIN")){
		i = atoi(value);
		if(i > 254)
			i = 254;
		factory_var_buf[1] = i;
		if((fp = fopen(dev_mtd, "rb")) != NULL){
			fseek(fp, NETGEAR_BOARD_DOMAIN, SEEK_SET);
			fwrite(factory_var_buf, 1, 2, fp);
			fclose(fp);
		}
	}else if(!strcmp(key, "PCBA_SN")){
		snprintf(factory_var_buf, sizeof(factory_var_buf), "%s", value);
		for(i = 0; i < 12; i++){
			if(factory_var_buf[i] == 0x0)
				factory_var_buf[i] = 0xff;
		}
		if((fp = fopen(dev_mtd, "rb")) != NULL){
			fseek(fp, NETGEAR_BOARD_PCBA_SN, SEEK_SET);
			fwrite(factory_var_buf, 1, 12, fp);
			fclose(fp);
		}
	}else if(!strcmp(key, "SSID")){
		snprintf(factory_var_buf, sizeof(factory_var_buf), "%s", value);
		for(i = 0; i < 20; i++){
			if(factory_var_buf[i] == 0x0)
				factory_var_buf[i] = 0xff;
		}
		if((fp = fopen(dev_mtd, "rb")) != NULL){
			fseek(fp, NETGEAR_BOARD_SSID, SEEK_SET);
			fwrite(factory_var_buf, 1, 20, fp);
			fclose(fp);
		}
	}else if(!strcmp(key, "PSWD")){
		snprintf(factory_var_buf, sizeof(factory_var_buf), "%s", value);
		for(i = 0; i < 64; i++){
			if(factory_var_buf[i] == 0x0)
				factory_var_buf[i] = 0xff;
		}
		if((fp = fopen(dev_mtd, "rb")) != NULL){
			fseek(fp, NETGEAR_BOARD_PASSWD, SEEK_SET);
			fwrite(factory_var_buf, 1, 64, fp);
			fclose(fp);
		}
	}else if(!strcmp(key, "MODULE")){
		snprintf(factory_var_buf, sizeof(factory_var_buf), "%s", value);
		for(i = 0; i < 2; i++){
			if(factory_var_buf[i] == 0x0)
				factory_var_buf[i] = 0xff;
		}
		if((fp = fopen(dev_mtd, "rb")) != NULL){
			fseek(fp, NETGEAR_BOARD_MODULE, SEEK_SET);
			fwrite(factory_var_buf, 1, 2, fp);
			fclose(fp);
		}
	}
}
#endif
#if defined(PGBM1)
int check_bwdpi_nvram_setting(){ return 0; }
int check_wrs_switch(){ return 0; }
#endif

#if defined(RTCONFIG_BCMARM) && !defined(RTCONFIG_HND_ROUTER_AX)
#define	HAPD_MAX_BUF			512
void __attribute__((weak)) wl_apply_akm_by_auth_mode(int unit, int subunit, char *sp_prefix_auth)
{
	char prefix[] = "wlXXXXXXX_", sp_prefix[] = "wlXXXXXXX_";
	char auth_mode[32] = {0}, tmp[32];

	_dprintf("%s, unit=%d, subunit=%d, sp_prefix:%s\n", __func__, unit, subunit, sp_prefix_auth);
	if(subunit == -1)
		snprintf(prefix, sizeof(prefix), "wl%d_", unit);
	else
		snprintf(prefix, sizeof(prefix), "wl%d.%d_", unit, subunit);
	if(sp_prefix_auth && strlen(sp_prefix_auth) > 3){
		snprintf(sp_prefix, sizeof(sp_prefix), "%s", sp_prefix_auth);
		strlcpy(auth_mode, "auth_mode", sizeof(auth_mode));
	}else{
		snprintf(sp_prefix, sizeof(sp_prefix), "%s", prefix);
		strlcpy(auth_mode, "auth_mode_x", sizeof(auth_mode));
	}
	memset(tmp, 0, sizeof(tmp));
	if(nvram_match(strcat_r(sp_prefix, auth_mode, tmp), "shared"))
		nvram_set(strcat_r(sp_prefix, "auth", tmp), "1");
	else
		nvram_set(strcat_r(sp_prefix, "auth", tmp), "0");
#ifdef RTAC68U_V4
	if (strstr(nvram_safe_get(strcat_r(sp_prefix, "auth_mode_x", tmp)), "sae"))
		nvram_set(strcat_r(sp_prefix, "auth_mode_x", tmp), "psk2");
#endif
	if(nvram_match(strcat_r(sp_prefix, auth_mode, tmp), "psk"))
		nvram_set(strcat_r(sp_prefix, "akm", tmp), "psk");
	else if(nvram_match(strcat_r(sp_prefix, "auth_mode_x", tmp), "psk2")){
#if defined(HND_ROUTER) && defined(WLHOSTFBT)
		nvram_set(strcat_r(sp_prefix, "akm", tmp), "psk2 psk2ft");
#else
		nvram_set(strcat_r(sp_prefix, "akm", tmp), "psk2");
#endif
	}
	else if (nvram_match(strcat_r(sp_prefix, "auth_mode_x", tmp), "pskpsk2"))
	{
#if defined(HND_ROUTER) && defined(WLHOSTFBT)
		nvram_set(strcat_r(sp_prefix, "akm", tmp), "psk psk2 psk2ft");
#else
		if((is_psta(unit) || is_psr(unit)) && subunit == -1)
			nvram_set(strcat_r(sp_prefix, "akm", tmp), "psk2");
		else{
			nvram_set(strcat_r(sp_prefix, "akm", tmp), "psk psk2");
			if(!nvram_match(strcat_r(sp_prefix, "mfp", tmp), "2"))
				return;
			else
				nvram_set(strcat_r(sp_prefix, "mfp", tmp), "1");
		}
#endif
	}
#ifndef RTAC68U_V4
	else if (nvram_match(strcat_r(sp_prefix, "auth_mode_x", tmp), "sae"))
	{
		if((is_psta(unit) || is_psr(unit)) && subunit == -1)
			nvram_set(strcat_r(sp_prefix, "akm", tmp), "sae");
		else
			nvram_set(strcat_r(sp_prefix, "akm", tmp), "psk2 sae");
	}
	else if (nvram_match(strcat_r(sp_prefix, "auth_mode_x", tmp), "psk2sae"))
	{
		if((is_psta(unit) || is_psr(unit)) && subunit == -1)
			nvram_set(strcat_r(sp_prefix, "akm", tmp), "sae");
		else{
			nvram_set(strcat_r(sp_prefix, "akm", tmp), "psk2 sae");
			if(!nvram_match(strcat_r(sp_prefix, "mfp", tmp), "2"))
				return;
			else
				nvram_set(strcat_r(sp_prefix, "mfp", tmp), "1");
		}
	}
#endif
	else if (nvram_match(strcat_r(sp_prefix, "auth_mode_x", tmp), "wpa"))
	{
		nvram_set(strcat_r(sp_prefix, "akm", tmp), "wpa");
	}
	else if (nvram_match(strcat_r(sp_prefix, "auth_mode_x", tmp), "wpa2"))
	{
		nvram_set(strcat_r(sp_prefix, "akm", tmp), "wpa2");
	}
	else if (nvram_match(strcat_r(sp_prefix, "auth_mode_x", tmp), "wpawpa2"))
	{
		nvram_set(strcat_r(sp_prefix, "akm", tmp), "wpa wpa2");
	}
	else if (nvram_match(strcat_r(sp_prefix, "auth_mode_x", tmp), "owe"))
	{
		nvram_set(strcat_r(sp_prefix, "akm", tmp), "owe");
	}
#ifdef RTCONFIG_OWE_TRANS
	else if (nvram_match(strcat_r(sp_prefix, "auth_mode_x", tmp), "openowe")) //OWE-Trainsition mode
	{
		// early convert owe-transition settings in wl_defaults()::set_owe_transition_bss_enabled(),
		// in order to bring up owe vif properly
	}
#endif
	else 
		nvram_set(strcat_r(sp_prefix, "akm", tmp), "");
}

int __attribute__((weak)) get_hostapd_pid(int radio_idx, pid_t *pid)
{
	DIR *dir;
	struct dirent *ent;
	char path[PATH_MAX];
	FILE *fp = NULL;
	char *pStr = NULL, *pStrTmp = NULL;
	char cmdline[HAPD_MAX_BUF] = {0};
	bool bFound = FALSE;
	char nvi_ifname[IFNAMSIZ];

	snprintf(nvi_ifname, sizeof(nvi_ifname), "wl%d", radio_idx);

	dir = opendir("/proc");
	if (dir == NULL) {
		cprintf("Err: %s fail to open /proc\n", __FUNCTION__);
		return -1;
	}

	/* loops over all dir entries in /proc */
	while (NULL != (ent = readdir(dir))) {
		/* proceed only when dir name is a number, i.e., pid */
		if (strlen(ent->d_name) != strspn(ent->d_name, "0123456789"))
			continue;

		/* open file /proc/<pid>/cmdline */
		snprintf(path, sizeof(path), "/proc/%s/cmdline", ent->d_name);
		fp = fopen(path, "r");
		if (!fp) {
			continue;
		}

		/* read from the file /proc/<pid>/cmdline */
		memset(cmdline, '\0', sizeof(cmdline));
		if (fgets(cmdline, sizeof(cmdline), fp) != NULL) {
			/* go further only if the process name is 'hostapd' */
			if (strcmp(cmdline, "hostapd")) {
				fclose(fp);
				continue;
			}
			pStr = cmdline;

			/*
			 * cmdline holds command-line arguments of process seperated by
			 * null byte '\0', loop on each argument here.
			 */
			while (TRUE) {
				if ((pStr >= (cmdline + sizeof(cmdline))) || (*pStr == '\0'))
					break;

				/* keep skipping to the next argument till the one for
				 * hostapd config filename.
				 */
				if (!strstr(pStr, ".conf")) {
					pStr += strlen(pStr);
					/* skip '\0' */
					++pStr;
				}
				else {
					/* proceed if config filename has wl<radio_num> in it */
					pStrTmp = strstr(pStr, nvi_ifname);
					if (!pStrTmp)
						break;
					pStr = pStrTmp + strlen(nvi_ifname);
					/*
					 * hostapd config filename is wlx_hapd.conf or
					 * wlx.y_hapd.conf, pStr points to the character after
					 * wl<radio_num> must be a '.' or '_'
					 */
					if (*pStr == '.' || *pStr == '_') {
						bFound = TRUE;
						*pid = atoi(ent->d_name);
					}
					break;
				}
			}
		}
		fclose(fp);
		if (bFound)
			break;
	}
	closedir(dir);
	return bFound ? 0 : -1;
}

void __attribute__((weak)) hapd_wpasupp_get_radio_list(char *ifnames_list, char *radio_iflist, int rlistsz, int idx)
{
	char ifname[IFNAMSIZ] = {0}, *next;
	char nv_ifname[IFNAMSIZ] = {0}, nvram_name[HAPD_MAX_BUF] = {0};
	int unit = -1;

	snprintf(nvram_name, sizeof(nvram_name),  "wl%d_radio", idx);

	if ((nvram_match(nvram_name, "0")) == TRUE) {
		cprintf("Err: rc: %d: radio %d disabled\n", __LINE__, idx);
		return;
	}

	foreach(ifname, ifnames_list, next) {
		if (wl_probe(ifname)) {
			dprintf("Err: rc: %d: Skipping %s - non wireless interface\n",
					__LINE__, ifname);
			continue;
		}
		if (osifname_to_nvifname(ifname, nv_ifname, IFNAMSIZ) < 0) {
			cprintf("Err: rc: %d: osname to ifname\n", __LINE__);
			return;
		}
		if (get_ifname_unit(nv_ifname, &unit, NULL) < 0) {
			cprintf("Err: rc: %d: get_ifname_unit\n", __LINE__);
			return;
		}

		if (unit == idx) {
			add_to_list(ifname, radio_iflist, rlistsz);
		} else {
			dprintf("Info: rc: %d: skipped adding %s to radio_iflist[%d]\n",
					__LINE__, ifname, idx);
		}
	} /* foreach */

	return;
}

int __attribute__((weak)) hapd_wpasupp_is_bss_enabled(const char* ifname)
{
	char nv_ifname[IFNAMSIZ] = {0};
	char nvram_name[HAPD_MAX_BUF] = {0};
	bool ret = 0;

	if (osifname_to_nvifname(ifname, nv_ifname, sizeof(nv_ifname))) {
		dprintf("Err: rc: %d: converting os ifname %s to nv ifname\n", __LINE__, ifname);
		return -1;
	}

	snprintf(nvram_name, sizeof(nvram_name),  "%s_bss_enabled", nv_ifname);

	ret =  nvram_match(nvram_name, "1");

	return ret;
}

int __attribute__((weak)) hapd_wpasupp_is_primary_ifce(const char *ifname)
{
	int unit = -1, subunit = -1;
	char nv_ifname[IFNAMSIZ] = {0};

	if (osifname_to_nvifname(ifname, nv_ifname, sizeof(nv_ifname))) {
		dprintf("Err: rc: %d: converting os ifname %s to nv ifname\n", __LINE__, ifname);
		return 0;
	}

	if (get_ifname_unit(nv_ifname, &unit, &subunit) < 0) {
		dprintf("Err: rc: %d: get_ifname_unit %s\n", __LINE__, nv_ifname);
		return 0;
	}

	if (subunit > 0) {
		dprintf("Err: rc: %d: Non primary interface %s\n", __LINE__, nv_ifname);
		return 0;
	}

	return 1;
}

void __attribute__((weak)) hapd_wpasupp_get_primary_virtual_iflist(char *filtered_list, char *pr_iflist, char *sec_iflist, int listsz)
{
	char ifname[IFNAMSIZ] = {0}, *next;

	foreach(ifname, filtered_list, next) {
		((hapd_wpasupp_is_primary_ifce(ifname) == TRUE) ?
			add_to_list(ifname, pr_iflist, listsz) :
			add_to_list(ifname, sec_iflist, listsz));
	}
}

int __attribute__((weak)) hapd_wpasupp_is_ifce_monitormode(const char *ifname)
{
	char nv_ifname[IFNAMSIZ] = {0}, nvram_name[HAPD_MAX_BUF] = {0};
	bool ret = 0;

	if (osifname_to_nvifname(ifname, nv_ifname, sizeof(nv_ifname))) {
		dprintf("Err: rc: %d: converting os ifname %s to nv ifname\n", __LINE__, ifname);
		return -1;
	}

	snprintf(nvram_name, sizeof(nvram_name),  "%s_mode", nv_ifname);
	ret = nvram_match(nvram_name, "monitor");
	return ret;
}

int __attribute__((weak)) hapd_wpasupp_is_ifce_ap(const char *ifname)
{
	char nv_ifname[IFNAMSIZ] = {0}, nvram_name[HAPD_MAX_BUF] = {0};
	bool ret = 0;

	if (osifname_to_nvifname(ifname, nv_ifname, sizeof(nv_ifname))) {
		dprintf("Err: rc: %d: converting os ifname %s to nv ifname\n", __LINE__, ifname);
		return -1;
	}

	snprintf(nvram_name, sizeof(nvram_name),  "%s_mode", nv_ifname);

	ret = nvram_match(nvram_name, "ap");

	return ret;
}

void __attribute__((weak)) hapd_wpasupp_get_filtered_ifnames_list(char *ifnames_list, char *filtered_list, int flist_sz)
{
	char pr_ifname[IFNAMSIZ+1] = {0}; /* primary ifname */
	char ifname[IFNAMSIZ] = {0}, tmp_list[HAPD_MAX_BUF] = {0};
	char *next;

	foreach(ifname, ifnames_list, next) {
		if (!wl_probe(ifname)) { /* only wireless interfaces */
			if (hapd_wpasupp_is_primary_ifce(ifname) == TRUE) {
				/* copy primary BSS ifname */
				strncpy(pr_ifname, ifname, IFNAMSIZ);
			} else { /* add all vritual BSS' to temporary list */
				add_to_list(ifname, tmp_list, HAPD_MAX_BUF);
			}
		}
	}

	/* if BSS on primary ifce is enabled, add it to filtered list */
	if (hapd_wpasupp_is_bss_enabled(pr_ifname)) {
		add_to_list(pr_ifname, filtered_list, flist_sz);
	}

	/* Check virtual BSS'. If any of the virtual BSS' enabled, then, blindly
	 * add the primary BSS to the filtered list even if it is not enabled.
	 */
	foreach(ifname, tmp_list, next) {
		if (hapd_wpasupp_is_bss_enabled(ifname)) {
			add_to_list(ifname, filtered_list, flist_sz);
		}
	}
}

int __attribute__((weak)) restart_hostapd_per_radio_wps_ob(int radio_idx)
{
#if defined(RTCONFIG_HND_ROUTER_AX)
	uint32 *o_flgs = 0;
	size_t size;
	int pid, retry, ret = 0, mode = 0;
	char *wgn_ifnames;
	char *lan_ifnames;
	char *lan1_ifnames;
	char *flist;
	char *plist;
	char *slist;
	char *iflist;
	char *ptr;
	char prefix[16];
	char file[512];
	char wgn_ifbuf[2048];
	char cmd[2080];
	char word[16];
	char *next;

	wgn_ifnames = wgn_guest_lan_ifnames(wgn_ifbuf, sizeof(wgn_ifbuf));
	memset(prefix, 0, sizeof(prefix));
	memset(file, 0, sizeof(file));
	memset(cmd, 0, sizeof(cmd));

	lan_ifnames = nvram_safe_get("lan_ifnames");
	lan1_ifnames = nvram_safe_get("lan1_ifnames");
	if ( !*lan_ifnames && !*lan1_ifnames ){
		_dprintf("Err: rc: %d:  No interfaces in LAN and GUEST bridges\n", __LINE__);
		return -1;
	}

	if ( wgn_ifnames )
		size = strlen(lan_ifnames) + strlen(lan1_ifnames) + 4 + strlen(wgn_ifnames);
	else
		size = strlen(lan_ifnames) + strlen(lan1_ifnames) + 2;
	ptr = calloc(1, size);
	if ( ptr ) {
		flist = calloc(1, size);
		if ( flist ) {
			plist = calloc(1, size);
			if ( plist ) {
				slist = calloc(1, size);
				if ( slist ) {
					iflist = calloc(1, size);
					if ( iflist ) {
						strcat_r(lan_ifnames, " ", ptr);
						strcat_r(ptr, lan1_ifnames, ptr);
						if ( wgn_ifnames ) {
							strcat_r(ptr, " ", ptr);
							strcat_r(ptr, wgn_ifnames, ptr);
						}
						hapd_wpasupp_get_radio_list(ptr, iflist, size, radio_idx);
						if ( *iflist ) {
							memset(flist, 0, size);
							memset(plist, 0, size);
							memset(slist, 0, size);
							hapd_wpasupp_get_filtered_ifnames_list(iflist, flist, size);
							hapd_wpasupp_get_primary_virtual_iflist(flist, plist, slist, size);
							_dprintf("radio_iflist: ");
							foreach(word, iflist, next)
								printf("%s ", word);
							printf("\n");
							printf("filtered_iflist: ");
							foreach(word, flist, next)
								printf("%s ", word);
							printf("\n");
							printf("primary_iflist: ");
							foreach(word, plist, next)
								printf("%s ", word);
							printf("\n");
							printf("virtual_iflist: ");
							foreach(word, slist, next)
								printf("%s ", word);
							printf("\n");
							foreach(word, slist, next){
								if ( hapd_wpasupp_is_ifce_monitormode(word) == 1 ){
									_dprintf("start_hostapd  %d: %s monitor skip hostapd \n", __LINE__, word);
									ret = -1;
									goto LABEL_86;
								}
								mode++;
								if(hapd_wpasupp_is_ifce_ap(word))
									break;
 							}
							if(mode != 0)
								mode = 1;
							foreach(word, slist, next)
								printf("%s ", word);
							printf("\n");
							foreach(word, slist, next)
							{
								if ( osifname_to_nvifname(word, prefix, 16) )
								{
									_dprintf("Err: rc: %d: converting os ifname %s to nv ifname\n", __LINE__, word);
									ret = -1;
									goto LABEL_86;
								}
								if ( hapd_get_config_filename(prefix, file, sizeof(file), &o_flgs, mode) >= 0 )
								{
									if ( hapd_create_config_file(prefix, file) < 0 )
										_dprintf("Err: rc: %d: config file %s creation for %s failed. skip\n", __LINE__, file, prefix);
									else
  									_dprintf("Err: rc: %d: get config file name for %s failed. skip\n", __LINE__, prefix);
								}
							}
							snprintf(prefix, sizeof(prefix), "wl%d", radio_idx);
							if ( get_hostapd_pid(radio_idx, &pid) )
								_dprintf("Err: %s hostapd instance for %s does not exist\n", __func__, prefix);
	 						else
							{
								dm_unregister_app_restart_info(pid);
								_dprintf("%s kill hostapd instance %d for %s\n", __func__, pid, prefix);
								snprintf(cmd, sizeof(cmd), "%d", pid);
 								eval("kill", "-9", cmd);
								for(retry = 5; ; retry--)
 								{
									if ( !get_hostapd_pid(radio_idx, &pid) )
										break;
									_dprintf("%s hostapd is still alive, pid=%d, radio_idx=%d, keep waiting\n", __func__, pid, radio_idx);
									sleep(1);
									if ( retry == 0 )
  									{
										_dprintf("Err: %s exit with hostapd still alive\n", __func__);
										ret = -1;
										goto LABEL_86;
									}
								}
								_dprintf("%s hostapd on radio %d exited\n", __func__, radio_idx);
							}
							_dprintf("Info: rc: %d: Running hostapd instance using %s configuration\n", __LINE__, file);
							if ( nvram_match("hapd_dbg", "4") )
								snprintf(cmd, sizeof(cmd), "hostapd %s %s &", "-sss", file);
							else if ( nvram_match("hapd_dbg", "3") )
								snprintf(cmd, sizeof(cmd), "hostapd %s %s &", "-ss", file);
							else if ( nvram_match("hapd_dbg", "2") )
								snprintf(cmd, sizeof(cmd), "hostapd %s %s &", "-s", file);
							else if ( nvram_match("hapd_dbg", "1") )
								snprintf(cmd, sizeof(cmd), "hostapd %s %s &", "-ddt", file);
							else
								snprintf(cmd, sizeof(cmd), "hostapd %s %s &", "-B", file);
							_dprintf("Info: rc: %d: Running cmd: %s.\n", __LINE__, cmd);
							system(cmd);
							pid = radio_idx - 3;
							for(retry = 5; ; retry--)
 							{
								if ( !get_hostapd_pid(radio_idx, &pid) )
									break;
								sleep(1);
								if ( retry == 0 )
								{
									_dprintf( "Err: %s Fail to start hostapd, radio_idx=%d\n", __func__, radio_idx);
									ret = -1;
									goto LABEL_86;
								}
							}
							_dprintf("Info: %s hostapd is running pid=%d, radio_idx=%d\n", __func__, __LINE__, radio_idx);
							goto LABEL_86;
						}
						_dprintf("Info: rc: %d: radio_iflist[%d] is empty. continue\n", __LINE__, radio_idx);
					}
					else
					{
						_dprintf("Err: rc: %d: calloc()\n", __LINE__);
					}
					ret = -1;
				}
				else
				{
					ret = -1;
					_dprintf("Err: rc: %d: calloc()\n", __LINE__);
					iflist = NULL;
				}
			}
			else
			{
				ret = -1;
				_dprintf("Err: rc: %d: calloc()\n", __LINE__);
				iflist = NULL;
				slist = NULL;
			}
		}
		else
		{
			ret = -1;
			_dprintf("Err: rc: %d: calloc()\n", __LINE__);
			slist = NULL;
			iflist = NULL;
			plist = NULL;
		}
LABEL_86:
		free(ptr);
		if ( flist )
			free(flist);
		if ( plist )
			free(plist);
		if ( slist )
			free(slist);
		if ( iflist )
			free(iflist);
		return ret;
	}
	_dprintf("Err: rc: %d: calloc()\n", __LINE__);
	return -1;
#else
	return 0;
#endif
}
#endif

#if defined(RTAC82U)
//fix jffs rebuilt by pb-boot is too large
void fix_jffs_size(void)
{
#define JFFS_SIZE_MAX 39*1024*1024 //asus:27MB
	FILE *fp = NULL;
	char buf[10] = {0};
	if((fp = fopen("/sys/devices/virtual/ubi/ubi0/ubi0_5/data_bytes", "r"))){
		fread(buf, 1, 9, fp);
		fclose(fp);
		if(JFFS_SIZE_MAX < strtol(buf, NULL, 10)){
			eval("ubirmvol", "/dev/ubi0", "-N", "jffs2");
			eval("ubimkvol", "/dev/ubi0", "-s", "38MiB", "-N", "jffs2");
		}
	}
}
#endif

#if defined(RTCONFIG_HND_ROUTER_AX)
void __attribute__((weak)) misc_ctrl_post()
{
	syslog(LOG_NOTICE, "fwver: %s_%s_%s (sn:%s /ha:%s )\n", rt_version, rt_serialno, rt_extendno, nvram_safe_get("serial_no"), nvram_safe_get("et0macaddr"));
}

int  __attribute__((weak)) resetNetLed()
{
	return 0;
}

int  __attribute__((weak)) setNetLed(void)
{
	return 0;
}
#endif

#if defined(RAX200)
void fan_watchdog(void)
{
	FILE *fp;
	int temperature, status;
	static int gpio_init = 0;
	const uint32_t gpio1 = 52;
	const uint32_t gpio2 = 55;
	const uint32_t gpio3 = 56;
	if ((fp = fopen("/sys/class/thermal/thermal_zone0/temp", "r")) != NULL) {
		fscanf(fp, "%d", &temperature);
		fclose(fp);
	}
	temperature = temperature / 1000;
	if(gpio_init == 0){
		set_gpio(gpio1, 0);
		set_gpio(gpio2, 0);
		set_gpio(gpio3, 0);
		gpio_init = 1;
	}
	if((status = get_gpio(gpio1)) < 0)
		return;
	if(nvram_get("fan_en") == NULL || nvram_match("fan_en", "1")){
		if(temperature > 95 || nvram_match("fan_lv", "4")){
			printf("Turn on FAN with level 4");
			set_gpio(gpio1, 1);
			set_gpio(gpio2, 1);
			set_gpio(gpio3, 1);
		}else if(temperature > 85 || nvram_match("fan_lv", "3")){
			printf("Turn on FAN with level 3");
			set_gpio(gpio1, 1);
			set_gpio(gpio2, 1);
			set_gpio(gpio3, 0);
		}else if(temperature > 75 || nvram_match("fan_lv", "2")){
			printf("Turn on FAN with level 2");
			set_gpio(gpio1, 1);
			set_gpio(gpio2, 0);
			set_gpio(gpio3, 1);
		}else if(temperature > 65 || nvram_match("fan_lv", "1")){
			printf("Turn on FAN with level 1");
			set_gpio(gpio1, 1);
			set_gpio(gpio2, 0);
			set_gpio(gpio3, 0);
		}else{
			printf("Turn off FAN");
			set_gpio(gpio1, 0);
			set_gpio(gpio2, 0);
			set_gpio(gpio3, 0);
		}
	}else if(nvram_match("fan_en", "0") && status == 1){
		printf("Turn off FAN");
		set_gpio(gpio1, 0);
		set_gpio(gpio2, 0);
		set_gpio(gpio3, 0);
	}
}
#endif

#if defined(RTCONFIG_BCMARM)
void get_nvramstr(int unit, char *buf, size_t len, int which)
{
	char *str = NULL;
#if defined(GTAXE16000)
const unsigned int devpath_idx[4] = {3, 4, 2, 1};    // 2.4G, 5G-1, 5G-2, 6G
#elif defined(GTAX11000_PRO)
const unsigned int devpath_idx[4] = {3, 4, 1, 2};    // 2.4G, 5G-1, 5G-2
#elif defined(GT10)
const unsigned int devpath_idx[4] = {0, 1, 2};    // 2.4G, 5G-1, 5G-2
#endif
	if(which == 1){
		if(unit == 0)
			str = "maxp2ga0";
		else
			str = "maxp5gb0a0";
	}else
		str = "macaddr";
	switch(get_model()){
		case MODEL_RTAC68U:
		case MODEL_RTAC3200:
#if defined(SBRAC3200P)
		case MODEL_SBRAC3200P:
#endif
		case MODEL_RTAC5300:
		case MODEL_RTAC88U:
		case MODEL_RTAC86U:
		case MODEL_RTAC3100:
		case MODEL_RTAX95Q:
		case MODEL_XT8PRO:
#if !defined(RTAX55)
		case MODEL_BM68:
#endif
		case MODEL_XT8_V2:
		case MODEL_RTAXE95Q:
		case MODEL_ET8PRO:
#if !defined(RTAX55)
		case MODEL_ET8_V2:
#endif
		case MODEL_RTAX56_XD4:
		case MODEL_XD4PRO:
		case MODEL_CTAX56_XD4:
		case MODEL_RTAX58U:
		case MODEL_RTAX82U_V2:
#if !defined(RTAX55)
		case MODEL_TUFAX5400_V2:
		case MODEL_RTAX5400:
#endif
		case MODEL_RTAX82_XD6S:
#if !defined(RTAX55)
		case MODEL_XD6_V2:
#endif
		case MODEL_RTAX58U_V2:
		case MODEL_RTAXE7800:
		case MODEL_RTAX86U:
		case MODEL_RTAX68U:
		case MODEL_RTAC68U_V4:
		case MODEL_RTAX86U_PRO:
#ifdef RTAC3200
			if (unit < 2)
				snprintf(buf, len, "%d:%s", 1 - unit, str);
			else
#endif
			snprintf(buf, len, "%d:%s", unit, str);
			break;
		case MODEL_GTAC5300:
			snprintf(buf, len, "%d:%s", unit + 1, str);
			break;
		case MODEL_RTAX55:
		case MODEL_TUFAX3000_V2:
		case MODEL_RTAX3000N:
#if !defined(RTAX55)
		case MODEL_BR63:
#endif
		case MODEL_RPAX56:
		case MODEL_RPAX58:
		case MODEL_RTAX56U:
			snprintf(buf, len, "sb/%d/%s", unit, str);
			break;
		case MODEL_RTAX88U:
		case MODEL_GTAX11000:
		case MODEL_RTAX92U:
		case MODEL_GTAXE11000:
		case MODEL_GTAX6000:
		case MODEL_ET12:
		case MODEL_XT12:
#if !defined(RTAX55)
		case MODEL_RTAX88U_PRO:
#endif
			snprintf(buf, len, "%d:%s", unit + 1, str);
			break;
#if defined(GTAXE16000) || defined(GTAX11000_PRO) || defined(GT10)
		case MODEL_GTAX11000_PRO:
		case MODEL_GTAXE16000:
		case MODEL_GT10:
			snprintf(buf, len, "%d:%s", devpath_idx[unit], str);
			break;
#endif
		default:
			snprintf(buf, len, "%d:%s", unit, str);
			break;
	}
}
#endif
#if defined(RTAX89U)
//workaround to fix smart connect
void pre_syspara_swrt(void)
{
	if(!nvram_match("pwrsave_mode", "1") && !nvram_match("pwrsave_mode", "2")){
		nvram_set("pwrsave_mode", "1");
	}
}
#endif

#if defined(RTCONFIG_TCODE) && defined(RTCONFIG_BCMARM)
int set_wltxpower_swrt(void)
{
	char prefix[16] __attribute__((unused));
	char tmp1[50] = {0}, tmp2[50] = {0};
	int txpower1, txpower2, txpower3, txpower4 __attribute__((unused));
	int p1, p2, p3, p4 __attribute__((unused));
	int commit_needed = 0, unit = 0;
	int model __attribute__((unused)) = get_model();
	int max2g = 5, max5g = 5, max5g2 = 5, max6g __attribute__((unused)) = 5;
	get_nvramstr(0, tmp1, sizeof(tmp1), 1);
	max2g = nvram_get_int(tmp1);
	memset(tmp1, 0, sizeof(tmp1));
	get_nvramstr(1, tmp1, sizeof(tmp1), 1);
	max5g = nvram_get_int(tmp1);
#if defined(RTCONFIG_HAS_5G_2)
	memset(tmp1, 0, sizeof(tmp1));
	get_nvramstr(2, tmp1, sizeof(tmp1), 1);
	max5g2 = nvram_get_int(tmp1);
#endif
#if defined(RTCONFIG_QUADBAND)
#if defined(GTAXE16000)
	memset(tmp1, 0, sizeof(tmp1));
	get_nvramstr(3, tmp1, sizeof(tmp1), 1);
	max6g = nvram_get_int(tmp1);
#endif
#endif
	if(auth_unlock_power)
		auth_unlock_power(&max2g, &max5g, &max5g2, &max6g);
	if(nvram_get_int("wl0_cpenable") == 1){
		unit |= 1;
		nvram_set("wl0_txpower", "100");
	}
	if(nvram_get_int("wl1_cpenable") == 1){
		unit |= 2;
		nvram_set("wl1_txpower", "100");
	}
#if defined(RTCONFIG_HAS_5G_2)
	if(nvram_get_int("wl2_cpenable") == 1){
		unit |= 4;
		nvram_set("wl2_txpower", "100");
	}
#endif
#if defined(RTCONFIG_QUADBAND)
	if(nvram_get_int("wl3_cpenable") == 1){
		unit |= 8;
		nvram_set("wl3_txpower", "100");
	}
#endif
	if((unit & 1) == 0 || (unit & 2) == 0
#if defined(RTCONFIG_HAS_5G_2)
	|| (unit & 4) == 0
#endif
#if defined(RTCONFIG_QUADBAND)
	|| (unit & 8) == 0
#endif
	){
		set_wltxpower();
		if(unit == 0)
			return 0;
	}

	if (!nvram_contains_word("rc_support", "pwrctrl")){
		dbg("%s(%d): [rc] no Power Control on this model\n", __func__, __LINE__);
		return -1;
	}
	txpower1 = nvram_get_int("wl0_custompower");
	txpower2 = nvram_get_int("wl1_custompower");
	txpower3 = nvram_get_int("wl2_custompower");
#if defined(RTCONFIG_QUADBAND)
	txpower4 = nvram_get_int("wl3_custompower");
#endif
	if(txpower1 < 6 || txpower1 > 118)
		txpower1 = 98;
	if(txpower2 < 6 || txpower2 > 118)
		txpower2 = 98;
	if(txpower3 < 6 || txpower3 > 118)
		txpower3 = 98;
#if defined(RTCONFIG_QUADBAND)
	if(txpower4 < 6 || txpower4 > 118)
		txpower4 = 98;
#endif
	logmessage("SWRT", "[%s] custom txpower", __func__);
	p1 = txpower1;
	p2 = txpower2;
	p3 = txpower3;
#if defined(RTCONFIG_QUADBAND)
	p4 = txpower4;
#endif
#if defined(RTCONFIG_HND_ROUTER_AX)
	if(max2g < p1)
		p1 = max2g;
	if(max5g < p2)
		p2 = max5g;
	if(max5g2 < p3)
		p3 = max5g2;
#if defined(RTCONFIG_QUADBAND)
	if(max6g < p4)
		p4 = max6g;
#endif
#endif
#if defined(RTCONFIG_HND_ROUTER_AX)
	if(unit & 1){
		logmessage("SWRT", "[%s], unit: %d, txpower: %d, max: %d\n", __func__, 0, p1, max2g);
		snprintf(tmp2, sizeof(tmp2), "%d", max2g - p1);
		eval("wl", "-i", nvram_get("wl0_ifname"), "txpwr_degrade", tmp2);
	}
	if(unit & 2){
		logmessage("SWRT", "[%s], unit: %d, txpower: %d, max: %d\n", __func__, 1, p2, max5g);
		snprintf(tmp2, sizeof(tmp2), "%d", max5g - p2);
		eval("wl", "-i", nvram_get("wl1_ifname"), "txpwr_degrade", tmp2);
	}
#if defined(RTCONFIG_HAS_5G_2)
	if(unit & 4){
		logmessage("SWRT", "[%s], unit: %d, txpower: %d, max: %d\n", __func__, 2, p3, max5g2);
		snprintf(tmp2, sizeof(tmp2), "%d", max5g2 - p3);
		eval("wl", "-i", nvram_get("wl2_ifname"), "txpwr_degrade", tmp2);
	}
#endif
#if defined(RTCONFIG_QUADBAND)
	if(unit & 8){
		logmessage("SWRT", "[%s], unit: %d, txpower: %d, max: %d\n", __func__, 3, p4, max6g);
		snprintf(tmp2, sizeof(tmp2), "%d", max6g - p4);
		eval("wl", "-i", nvram_get("wl3_ifname"), "txpwr_degrade", tmp2);
	}
#endif
#else
	switch(model)
	{
		case MODEL_RTAC68U:
			if(unit & 1){
				snprintf(prefix, sizeof(prefix), "0:");
				snprintf(tmp2, sizeof(tmp2), "%d", p1);
				nvram_set(strcat_r(prefix,"maxp2ga0", tmp1), tmp2);
				nvram_set(strcat_r(prefix,"maxp2ga1", tmp1), tmp2);
				nvram_set(strcat_r(prefix,"maxp2ga2", tmp1), tmp2);
				commit_needed++;
			}
			if(unit & 2){
				snprintf(prefix, sizeof(prefix), "1:");
				snprintf(tmp2, sizeof(tmp2), "%d,%d,%d,%d", p2, p2, p2, p2);
				nvram_set(strcat_r(prefix, "maxp5ga0", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5ga1", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5ga2", tmp1), tmp2);
				commit_needed++;
			}
			break;
		case MODEL_RTAC3100:
		case MODEL_RTAC88U:
#if defined(R7000P)
		case MODEL_R7000P:
#endif
			if(unit & 1){
				snprintf(prefix, sizeof(prefix), "0:");
				snprintf(tmp2, sizeof(tmp2), "%d", p1);
				nvram_set(strcat_r(prefix,"maxp2ga0", tmp1), tmp2);
				nvram_set(strcat_r(prefix,"maxp2ga1", tmp1), tmp2);
				nvram_set(strcat_r(prefix,"maxp2ga2", tmp1), tmp2);
#if !defined(R7000P)
				nvram_set(strcat_r(prefix,"maxp2ga3", tmp1), tmp2);
#endif
#if defined(K3)
				nvram_set(strcat_r(prefix,"cckbw202gpo", tmp1), "0");
				nvram_set(strcat_r(prefix,"cckbw20ul2gpo", tmp1), "0");
				nvram_set(strcat_r(prefix,"ofdmlrbw202gpo", tmp1), "0");
				nvram_set(strcat_r(prefix,"dot11agofdmhrbw202gpo", tmp1), "0x8420");
				nvram_set(strcat_r(prefix,"mcsbw202gpo", tmp1), "0xCCA86400");
				nvram_set(strcat_r(prefix,"mcsbw402gpo", tmp1), "0xCCA86400");
				nvram_set(strcat_r(prefix,"mcs1024qam2gpo", tmp1), "0");
				nvram_set(strcat_r(prefix,"mcs8poexp", tmp1), "0");
				nvram_set(strcat_r(prefix,"mcs9poexp", tmp1), "0");
				nvram_set(strcat_r(prefix,"mcs10poexp", tmp1), "0x11111111");
				nvram_set(strcat_r(prefix,"mcs11poexp", tmp1), "0x11111111");
#elif defined(XWR3100)
				nvram_set(strcat_r(prefix,"cckbw202gpo", tmp1), "0");
				nvram_set(strcat_r(prefix,"cckbw20ul2gpo", tmp1), "0");
				nvram_set(strcat_r(prefix,"ofdmlrbw202gpo", tmp1), "0");
				nvram_set(strcat_r(prefix,"dot11agofdmhrbw202gpo", tmp1), "0");
				nvram_set(strcat_r(prefix,"mcsbw202gpo", tmp1), "0xCCA84200");
				nvram_set(strcat_r(prefix,"mcsbw402gpo", tmp1), "0xCCA84200");
				nvram_set(strcat_r(prefix,"mcs1024qam2gpo", tmp1), "0xeecc");
				nvram_set(strcat_r(prefix,"mcs8poexp", tmp1), "0");
				nvram_set(strcat_r(prefix,"mcs9poexp", tmp1), "0");
				nvram_set(strcat_r(prefix,"mcs10poexp", tmp1), "0");
				nvram_set(strcat_r(prefix,"mcs11poexp", tmp1), "0");
#elif defined(R7000P)
				nvram_set(strcat_r(prefix,"cckbw202gpo", tmp1), "0");
				nvram_set(strcat_r(prefix,"cckbw20ul2gpo", tmp1), "0");
				nvram_set(strcat_r(prefix,"ofdmlrbw202gpo", tmp1), "0");
				nvram_set(strcat_r(prefix,"dot11agofdmhrbw202gpo", tmp1), "0xAECA");
				nvram_set(strcat_r(prefix,"mcsbw202gpo", tmp1), "0xBA76A860");
				nvram_set(strcat_r(prefix,"mcsbw402gpo", tmp1), "0xBA88FFFF");
#else
#if 0
				nvram_set(strcat_r(prefix,"cckbw202gpo", tmp1), "0");
				nvram_set(strcat_r(prefix,"cckbw20ul2gpo", tmp1), "0");
				nvram_set(strcat_r(prefix,"ofdmlrbw202gpo", tmp1), "0");
				nvram_set(strcat_r(prefix,"dot11agofdmhrbw202gpo", tmp1), "0x3210");
				nvram_set(strcat_r(prefix,"mcsbw202gpo", tmp1), "0x66532100");
				nvram_set(strcat_r(prefix,"mcsbw402gpo", tmp1), "0x66532100");
				nvram_set(strcat_r(prefix,"mcs1024qam2gpo", tmp1), "0x66666666");
				nvram_set(strcat_r(prefix,"mcs8poexp", tmp1), "0");
				nvram_set(strcat_r(prefix,"mcs9poexp", tmp1), "0");
				nvram_set(strcat_r(prefix,"mcs10poexp", tmp1), "0");
				nvram_set(strcat_r(prefix,"mcs11poexp", tmp1), "0");
#endif
#endif
				commit_needed++;
			}
			if(unit & 2){
				snprintf(prefix, sizeof(prefix), "1:");
				snprintf(tmp2, sizeof(tmp2), "%d", p2);
				nvram_set(strcat_r(prefix, "maxp5gb0a0", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb1a0", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb2a0", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb3a0", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb4a0", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb0a1", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb1a1", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb2a1", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb3a1", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb4a1", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb0a2", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb1a2", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb2a2", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb3a2", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb4a2", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb0a3", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb1a3", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb2a3", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb3a3", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb4a3", tmp1), tmp2);
#if defined(K3)
				nvram_set(strcat_r(prefix,"mcsbw205glpo", tmp1), "0xCBA87642");
				nvram_set(strcat_r(prefix,"mcsbw405glpo", tmp1), "0xCBA87642");
				nvram_set(strcat_r(prefix,"mcsbw805glpo", tmp1), "0xCBA87642");
				nvram_set(strcat_r(prefix,"mcsbw1605glpo", tmp1), "0xFFFFFFFF");
				nvram_set(strcat_r(prefix,"mcs1024qam5glpo", tmp1), "0x00000000");
				nvram_set(strcat_r(prefix,"mcslr5glpo", tmp1), "0");
				nvram_set(strcat_r(prefix,"mcsbw205gmpo", tmp1), "0xCBA87642");
				nvram_set(strcat_r(prefix,"mcsbw405gmpo", tmp1), "0xCBA87642");
				nvram_set(strcat_r(prefix,"mcsbw805gmpo", tmp1), "0xCBA87642");
				nvram_set(strcat_r(prefix,"mcsbw1605gmpo", tmp1), "0xFFFFFFFF");
				nvram_set(strcat_r(prefix,"mcs1024qam5gmpo", tmp1), "0x00000000");
				nvram_set(strcat_r(prefix,"mcslr5gmpo", tmp1), "0");
				nvram_set(strcat_r(prefix,"mcsbw205ghpo", tmp1), "0xCBA87642");
				nvram_set(strcat_r(prefix,"mcsbw405ghpo", tmp1), "0xCBA87642");
				nvram_set(strcat_r(prefix,"mcsbw805ghpo", tmp1), "0xCBA87642");
				nvram_set(strcat_r(prefix,"mcsbw1605ghpo", tmp1), "0xFFFFFFFF");
				nvram_set(strcat_r(prefix,"mcs1024qam5ghpo", tmp1), "0x00000000");
				nvram_set(strcat_r(prefix,"mcslr5ghpo", tmp1), "0");
				nvram_set(strcat_r(prefix,"mcsbw205gx1po", tmp1), "0xCBA87642");
				nvram_set(strcat_r(prefix,"mcsbw405gx1po", tmp1), "0xCBA87642");
				nvram_set(strcat_r(prefix,"mcsbw805gx1po", tmp1), "0xCBA87642");
				nvram_set(strcat_r(prefix,"mcsbw1605gx1po", tmp1), "0xFFFFFFFF");
				nvram_set(strcat_r(prefix,"mcs1024qam5gx1po", tmp1), "0x00000000");
				nvram_set(strcat_r(prefix,"mcslr5gx1po", tmp1), "0");
				nvram_set(strcat_r(prefix,"mcsbw205gx2po", tmp1), "0xCBA87642");
				nvram_set(strcat_r(prefix,"mcsbw405gx2po", tmp1), "0xCBA87642");
				nvram_set(strcat_r(prefix,"mcsbw805gx2po", tmp1), "0xCBA87642");
				nvram_set(strcat_r(prefix,"mcsbw1605gx2po", tmp1), "0xFFFFFFFF");
				nvram_set(strcat_r(prefix,"mcs1024qam5gx2po", tmp1), "0x00000000");
				nvram_set(strcat_r(prefix,"mcslr5gx2po", tmp1), "0");
				nvram_set(strcat_r(prefix,"mcs8poexp", tmp1), "0");
				nvram_set(strcat_r(prefix,"mcs9poexp", tmp1), "0");
				nvram_set(strcat_r(prefix,"mcs10poexp", tmp1), "0x3E3E3E3E");
				nvram_set(strcat_r(prefix,"mcs11poexp", tmp1), "0x3E3E3E3E");
#elif defined(XWR3100)
				nvram_set(strcat_r(prefix,"mcsbw205glpo", tmp1), "0x99666000");
				nvram_set(strcat_r(prefix,"mcsbw405glpo", tmp1), "0x99666222");
				nvram_set(strcat_r(prefix,"mcsbw805glpo", tmp1), "0x99666222");
				nvram_set(strcat_r(prefix,"mcsbw1605glpo", tmp1), "0xFFFFFFFF");
				nvram_set(strcat_r(prefix,"mcs1024qam5glpo", tmp1), "0xFFFFFFFF");
				nvram_set(strcat_r(prefix,"mcslr5glpo", tmp1), "0");
				nvram_set(strcat_r(prefix,"mcsbw205gmpo", tmp1), "0x99666000");
				nvram_set(strcat_r(prefix,"mcsbw405gmpo", tmp1), "0x99666222");
				nvram_set(strcat_r(prefix,"mcsbw805gmpo", tmp1), "0x99666444");
				nvram_set(strcat_r(prefix,"mcsbw1605gmpo", tmp1), "0xFFFFFFFF");
				nvram_set(strcat_r(prefix,"mcs1024qam5gmpo", tmp1), "0xFFFFFFFF");
				nvram_set(strcat_r(prefix,"mcslr5gmpo", tmp1), "0");
				nvram_set(strcat_r(prefix,"mcsbw205ghpo", tmp1), "0xAA666000");
				nvram_set(strcat_r(prefix,"mcsbw405ghpo", tmp1), "0xAA666222");
				nvram_set(strcat_r(prefix,"mcsbw805ghpo", tmp1), "0xAA666444");
				nvram_set(strcat_r(prefix,"mcsbw1605ghpo", tmp1), "0xFFFFFFFF");
				nvram_set(strcat_r(prefix,"mcs1024qam5ghpo", tmp1), "0xFFFFFFFF");
				nvram_set(strcat_r(prefix,"mcslr5ghpo", tmp1), "0");
				nvram_set(strcat_r(prefix,"mcsbw205gx1po", tmp1), "0xEE888220");
				nvram_set(strcat_r(prefix,"mcsbw405gx1po", tmp1), "0xEE888220");
				nvram_set(strcat_r(prefix,"mcsbw805gx1po", tmp1), "0xEE888220");
				nvram_set(strcat_r(prefix,"mcsbw1605gx1po", tmp1), "0xFFFFFFFF");
				nvram_set(strcat_r(prefix,"mcs1024qam5gx1po", tmp1), "0xFFFFFFFF");
				nvram_set(strcat_r(prefix,"mcslr5gx1po", tmp1), "0");
				nvram_set(strcat_r(prefix,"mcsbw205gx2po", tmp1), "0xEE888220");
				nvram_set(strcat_r(prefix,"mcsbw405gx2po", tmp1), "0xEE888220");
				nvram_set(strcat_r(prefix,"mcsbw805gx2po", tmp1), "0xEE888220");
				nvram_set(strcat_r(prefix,"mcsbw1605gx2po", tmp1), "0xFFFFFFFF");
				nvram_set(strcat_r(prefix,"mcs1024qam5gx2po", tmp1), "0xFFFFFFFF");
				nvram_set(strcat_r(prefix,"mcslr5gx2po", tmp1), "0");
				nvram_set(strcat_r(prefix,"mcs8poexp", tmp1), "0");
				nvram_set(strcat_r(prefix,"mcs9poexp", tmp1), "0");
				nvram_set(strcat_r(prefix,"mcs10poexp", tmp1), "0");
				nvram_set(strcat_r(prefix,"mcs11poexp", tmp1), "0");
#elif defined(R7000P)
				nvram_set(strcat_r(prefix,"mcsbw205glpo", tmp1), "0xECA86420");
				nvram_set(strcat_r(prefix,"mcsbw405glpo", tmp1), "0xECA86420");
				nvram_set(strcat_r(prefix,"mcsbw805glpo", tmp1), "0xECA86420");
				nvram_set(strcat_r(prefix,"mcsbw1605glpo", tmp1), "0xECA86420");
				nvram_set(strcat_r(prefix,"mcs1024qam5glpo", tmp1), "0x22222222");
				nvram_set(strcat_r(prefix,"mcslr5glpo", tmp1), "0");
				nvram_set(strcat_r(prefix,"mcsbw205gmpo", tmp1), "0xECA86420");
				nvram_set(strcat_r(prefix,"mcsbw405gmpo", tmp1), "0xECA86420");
				nvram_set(strcat_r(prefix,"mcsbw805gmpo", tmp1), "0xECA86420");
				nvram_set(strcat_r(prefix,"mcsbw1605gmpo", tmp1), "0xECA86420");
				nvram_set(strcat_r(prefix,"mcs1024qam5gmpo", tmp1), "0x22222222");
				nvram_set(strcat_r(prefix,"mcslr5gmpo", tmp1), "0");
				nvram_set(strcat_r(prefix,"mcsbw205ghpo", tmp1), "0xECA86420");
				nvram_set(strcat_r(prefix,"mcsbw405ghpo", tmp1), "0xECA86420");
				nvram_set(strcat_r(prefix,"mcsbw805ghpo", tmp1), "0xECA86420");
				nvram_set(strcat_r(prefix,"mcsbw1605ghpo", tmp1), "0xECA86420");
				nvram_set(strcat_r(prefix,"mcs1024qam5ghpo", tmp1), "0x22222222");
				nvram_set(strcat_r(prefix,"mcslr5ghpo", tmp1), "0");
				nvram_set(strcat_r(prefix,"mcsbw205gx1po", tmp1), "0xECA86420");
				nvram_set(strcat_r(prefix,"mcsbw405gx1po", tmp1), "0xECA86420");
				nvram_set(strcat_r(prefix,"mcsbw805gx1po", tmp1), "0xECA86420");
				nvram_set(strcat_r(prefix,"mcsbw1605gx1po", tmp1), "0xECA86420");
				nvram_set(strcat_r(prefix,"mcs1024qam5gx1po", tmp1), "0x22222222");
				nvram_set(strcat_r(prefix,"mcslr5gx1po", tmp1), "0");
				nvram_set(strcat_r(prefix,"mcsbw205gx2po", tmp1), "0xECA86420");
				nvram_set(strcat_r(prefix,"mcsbw405gx2po", tmp1), "0xECA86420");
				nvram_set(strcat_r(prefix,"mcsbw805gx2po", tmp1), "0xECA86420");
				nvram_set(strcat_r(prefix,"mcsbw1605gx2po", tmp1), "0xECA86420");
				nvram_set(strcat_r(prefix,"mcs1024qam5gx2po", tmp1), "0x22222222");
				nvram_set(strcat_r(prefix,"mcslr5gx2po", tmp1), "0");
				nvram_set(strcat_r(prefix,"mcs8poexp", tmp1), "0");
				nvram_set(strcat_r(prefix,"mcs9poexp", tmp1), "0");
				nvram_set(strcat_r(prefix,"mcs10poexp", tmp1), "0xFFFFFFFF");
				nvram_set(strcat_r(prefix,"mcs11poexp", tmp1), "0xFFFFFFFF");

#else
#if 0
				nvram_set(strcat_r(prefix,"mcsbw205glpo", tmp1), "0xCBA97530");
				nvram_set(strcat_r(prefix,"mcsbw405glpo", tmp1), "0xCBA97530");
				nvram_set(strcat_r(prefix,"mcsbw805glpo", tmp1), "0xCBA97530");
				nvram_set(strcat_r(prefix,"mcsbw1605glpo", tmp1), "0xCBA97530");
				nvram_set(strcat_r(prefix,"mcs1024qam5glpo", tmp1), "0xEDEDEDED");
				nvram_set(strcat_r(prefix,"mcslr5glpo", tmp1), "0");
				nvram_set(strcat_r(prefix,"mcsbw205gmpo", tmp1), "0xCBA97530");
				nvram_set(strcat_r(prefix,"mcsbw405gmpo", tmp1), "0xCBA97530");
				nvram_set(strcat_r(prefix,"mcsbw805gmpo", tmp1), "0xCBA97530");
				nvram_set(strcat_r(prefix,"mcsbw1605gmpo", tmp1), "0xCBA97530");
				nvram_set(strcat_r(prefix,"mcs1024qam5gmpo", tmp1), "0xEDEDEDED");
				nvram_set(strcat_r(prefix,"mcslr5gmpo", tmp1), "0");
				nvram_set(strcat_r(prefix,"mcsbw205ghpo", tmp1), "0xCBA97530");
				nvram_set(strcat_r(prefix,"mcsbw405ghpo", tmp1), "0xCBA97530");
				nvram_set(strcat_r(prefix,"mcsbw805ghpo", tmp1), "0xCBA97530");
				nvram_set(strcat_r(prefix,"mcsbw1605ghpo", tmp1), "0xCBA97530");
				nvram_set(strcat_r(prefix,"mcs1024qam5ghpo", tmp1), "0xEDEDEDED");
				nvram_set(strcat_r(prefix,"mcslr5ghpo", tmp1), "0");
				nvram_set(strcat_r(prefix,"mcsbw205gx1po", tmp1), "0xCBA97530");
				nvram_set(strcat_r(prefix,"mcsbw405gx1po", tmp1), "0xCBA97530");
				nvram_set(strcat_r(prefix,"mcsbw805gx1po", tmp1), "0xCBA97530");
				nvram_set(strcat_r(prefix,"mcsbw1605gx1po", tmp1), "0xCBA97530");
				nvram_set(strcat_r(prefix,"mcs1024qam5gx1po", tmp1), "0xEDEDEDED");
				nvram_set(strcat_r(prefix,"mcslr5gx1po", tmp1), "0");
				nvram_set(strcat_r(prefix,"mcsbw205gx2po", tmp1), "0xCBA97530");
				nvram_set(strcat_r(prefix,"mcsbw405gx2po", tmp1), "0xCBA97530");
				nvram_set(strcat_r(prefix,"mcsbw805gx2po", tmp1), "0xCBA97530");
				nvram_set(strcat_r(prefix,"mcsbw1605gx2po", tmp1), "0xCBA97530");
				nvram_set(strcat_r(prefix,"mcs1024qam5gx2po", tmp1), "0xEDEDEDED");
				nvram_set(strcat_r(prefix,"mcslr5gx2po", tmp1), "0");
				nvram_set(strcat_r(prefix,"mcs8poexp", tmp1), "0");
				nvram_set(strcat_r(prefix,"mcs9poexp", tmp1), "0");
				nvram_set(strcat_r(prefix,"mcs10poexp", tmp1), "0");
				nvram_set(strcat_r(prefix,"mcs11poexp", tmp1), "0");
#endif
#endif
				commit_needed++;
			}
			break;
		case MODEL_RTAC3200:
		case MODEL_SBRAC3200P:
			if(unit & 1){
				snprintf(prefix, sizeof(prefix), "1:");
				snprintf(tmp2, sizeof(tmp2), "%d", p1);
				nvram_set(strcat_r(prefix, "maxp2ga0", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp2ga1", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp2ga2", tmp1), tmp2);
#if defined(SBRAC3200P)
				nvram_set(strcat_r(prefix,"cckbw202gpo", tmp1), "0");
				nvram_set(strcat_r(prefix,"cckbw20ul2gpo", tmp1), "0");
				nvram_set(strcat_r(prefix,"mcsbw202gpo", tmp1), "0x86422222");
				nvram_set(strcat_r(prefix,"mcsbw402gpo", tmp1), "0x88644444");
				nvram_set(strcat_r(prefix,"dot11agofdmhrbw202gpo", tmp1), "0x4222");
				nvram_set(strcat_r(prefix,"ofdmlrbw202gpo", tmp1), "0x2222");
				nvram_set(strcat_r(prefix,"dot11agduphrpo", tmp1), "0");
				nvram_set(strcat_r(prefix,"dot11agduplrpo", tmp1), "0");
#else
#if 0
				nvram_set(strcat_r(prefix,"cckbw202gpo", tmp1), "0");
				nvram_set(strcat_r(prefix,"cckbw20ul2gpo", tmp1), "0");
				nvram_set(strcat_r(prefix,"mcsbw202gpo", tmp1), "0x87542000");
				nvram_set(strcat_r(prefix,"mcsbw402gpo", tmp1), "0x87542000");
				nvram_set(strcat_r(prefix,"dot11agofdmhrbw202gpo", tmp1), "0x2000");
				nvram_set(strcat_r(prefix,"ofdmlrbw202gpo", tmp1), "0");
				nvram_set(strcat_r(prefix,"dot11agduphrpo", tmp1), "0");
				nvram_set(strcat_r(prefix,"dot11agduplrpo", tmp1), "0");
#endif
#endif
				commit_needed++;
			}
			if(unit & 2){
				snprintf(prefix, sizeof(prefix), "0:");
				snprintf(tmp2, sizeof(tmp2), "%d,%d,%d,%d", p2, p2, p2, p2);
				nvram_set(strcat_r(prefix, "maxp5ga0", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5ga1", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5ga2", tmp1), tmp2);
#if defined(SBRAC3200P)
				nvram_set(strcat_r(prefix,"mcsbw205glpo", tmp1), "0");
				nvram_set(strcat_r(prefix,"mcsbw405glpo", tmp1), "0");
				nvram_set(strcat_r(prefix,"mcsbw805glpo", tmp1), "0");
				nvram_set(strcat_r(prefix,"mcsbw1605glpo", tmp1), "0");
				nvram_set(strcat_r(prefix,"mcsbw205gmpo", tmp1), "0xfffda844");
				nvram_set(strcat_r(prefix,"mcsbw405gmpo", tmp1), "0xfffda844");
				nvram_set(strcat_r(prefix,"mcsbw805gmpo", tmp1), "0xfffda844");
				nvram_set(strcat_r(prefix,"mcsbw1605gmpo", tmp1), "0");
				nvram_set(strcat_r(prefix,"mcsbw205ghpo", tmp1), "0xfffda844");
				nvram_set(strcat_r(prefix,"mcsbw405ghpo", tmp1), "0xfffda844");
				nvram_set(strcat_r(prefix,"mcsbw805ghpo", tmp1), "0xfffda844");
				nvram_set(strcat_r(prefix,"mcsbw1605ghpo", tmp1), "0");
#else
#if 0
				nvram_set(strcat_r(prefix,"mcsbw205glpo", tmp1), "0x66664200");
				nvram_set(strcat_r(prefix,"mcsbw405glpo", tmp1), "0x66663200");
				nvram_set(strcat_r(prefix,"mcsbw805glpo", tmp1), "0xA8643200");
				nvram_set(strcat_r(prefix,"mcsbw1605glpo", tmp1), "0");
				nvram_set(strcat_r(prefix,"mcsbw205gmpo", tmp1), "0x66664200");
				nvram_set(strcat_r(prefix,"mcsbw405gmpo", tmp1), "0x66663200");
				nvram_set(strcat_r(prefix,"mcsbw805gmpo", tmp1), "0x66663200");
				nvram_set(strcat_r(prefix,"mcsbw1605gmpo", tmp1), "0");
				nvram_set(strcat_r(prefix,"mcsbw205ghpo", tmp1), "0xfffda844");
				nvram_set(strcat_r(prefix,"mcsbw405ghpo", tmp1), "0xfffda844");
				nvram_set(strcat_r(prefix,"mcsbw805ghpo", tmp1), "0xfffda844");
				nvram_set(strcat_r(prefix,"mcsbw1605ghpo", tmp1), "0");
#endif
#endif
				commit_needed++;
			}
			if(unit & 4){
				snprintf(prefix, sizeof(prefix), "2:");
				snprintf(tmp2, sizeof(tmp2), "%d,%d,%d,%d", p3, p3, p3, p3);
				nvram_set(strcat_r(prefix, "maxp5ga0", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5ga1", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5ga2", tmp1), tmp2);
#if defined(SBRAC3200P)
				nvram_set(strcat_r(prefix,"mcsbw205glpo", tmp1), "0");
				nvram_set(strcat_r(prefix,"mcsbw405glpo", tmp1), "0");
				nvram_set(strcat_r(prefix,"mcsbw805glpo", tmp1), "0");
				nvram_set(strcat_r(prefix,"mcsbw1605glpo", tmp1), "0");
				nvram_set(strcat_r(prefix,"mcsbw205gmpo", tmp1), "0xfffda844");
				nvram_set(strcat_r(prefix,"mcsbw405gmpo", tmp1), "0xfffda844");
				nvram_set(strcat_r(prefix,"mcsbw805gmpo", tmp1), "0xfffda844");
				nvram_set(strcat_r(prefix,"mcsbw1605gmpo", tmp1), "0");
				nvram_set(strcat_r(prefix,"mcsbw205ghpo", tmp1), "0xAA975420");
				nvram_set(strcat_r(prefix,"mcsbw405ghpo", tmp1), "0xfffda844");
				nvram_set(strcat_r(prefix,"mcsbw805ghpo", tmp1), "0xfffda844");
				nvram_set(strcat_r(prefix,"mcsbw1605ghpo", tmp1), "0");
#else
#if 0
				nvram_set(strcat_r(prefix,"mcsbw205glpo", tmp1), "0");
				nvram_set(strcat_r(prefix,"mcsbw405glpo", tmp1), "0");
				nvram_set(strcat_r(prefix,"mcsbw805glpo", tmp1), "0");
				nvram_set(strcat_r(prefix,"mcsbw1605glpo", tmp1), "0");
				nvram_set(strcat_r(prefix,"mcsbw205gmpo", tmp1), "0xAA975420");
				nvram_set(strcat_r(prefix,"mcsbw405gmpo", tmp1), "0xAA975420");
				nvram_set(strcat_r(prefix,"mcsbw805gmpo", tmp1), "0xAA975420");
				nvram_set(strcat_r(prefix,"mcsbw1605gmpo", tmp1), "0");
				nvram_set(strcat_r(prefix,"mcsbw205ghpo", tmp1), "0xAA975420");
				nvram_set(strcat_r(prefix,"mcsbw405ghpo", tmp1), "0xAA975420");
				nvram_set(strcat_r(prefix,"mcsbw805ghpo", tmp1), "0xAA975420");
				nvram_set(strcat_r(prefix,"mcsbw1605ghpo", tmp1), "0");
#endif
#endif
				commit_needed++;
			}
			break;
		case MODEL_RTAC5300:
			if(unit & 1){
				snprintf(prefix, sizeof(prefix), "0:");
				snprintf(tmp2, sizeof(tmp2), "%d", p1);
				nvram_set(strcat_r(prefix,"maxp2ga0", tmp1), tmp2);
				nvram_set(strcat_r(prefix,"maxp2ga1", tmp1), tmp2);
				nvram_set(strcat_r(prefix,"maxp2ga2", tmp1), tmp2);
				nvram_set(strcat_r(prefix,"maxp2ga3", tmp1), tmp2);
#if defined(R8500)
				nvram_set(strcat_r(prefix,"cckbw202gpo", tmp1), "0");
				nvram_set(strcat_r(prefix,"cckbw20ul2gpo", tmp1), "0");
				nvram_set(strcat_r(prefix,"ofdmlrbw202gpo", tmp1), "0");
				nvram_set(strcat_r(prefix,"dot11agofdmhrbw202gpo", tmp1), "0x8642");
				nvram_set(strcat_r(prefix,"mcsbw202gpo", tmp1), "0x0EA86300");
				nvram_set(strcat_r(prefix,"mcsbw402gpo", tmp1), "0x0EA86300");
				nvram_set(strcat_r(prefix,"mcs1024qam2gpo", tmp1), "0x8888");
				nvram_set(strcat_r(prefix,"mcs10poexp", tmp1), "0xFFFFFFFF");
				nvram_set(strcat_r(prefix,"mcs11poexp", tmp1), "0xFFFFFFFF");
#else
#if 0
				nvram_set(strcat_r(prefix,"cckbw202gpo", tmp1), "0");
				nvram_set(strcat_r(prefix,"cckbw20ul2gpo", tmp1), "0");
				nvram_set(strcat_r(prefix,"ofdmlrbw202gpo", tmp1), "0");
				nvram_set(strcat_r(prefix,"dot11agofdmhrbw202gpo", tmp1), "0x4210");
				nvram_set(strcat_r(prefix,"mcsbw202gpo", tmp1), "0xB9872100");
				nvram_set(strcat_r(prefix,"mcsbw402gpo", tmp1), "0xB9872100");
				nvram_set(strcat_r(prefix,"mcs1024qam2gpo", tmp1), "0xEDEDEDED");
				nvram_set(strcat_r(prefix,"mcs10poexp", tmp1), "0");
				nvram_set(strcat_r(prefix,"mcs11poexp", tmp1), "0");
#endif
#endif
				commit_needed++;
			}
			if(unit & 2){
				snprintf(prefix, sizeof(prefix), "1:");
				snprintf(tmp2, sizeof(tmp2), "%d", p2);
				nvram_set(strcat_r(prefix, "maxp5gb0a0", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb1a0", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb2a0", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb3a0", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb4a0", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb0a1", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb1a1", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb2a1", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb3a1", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb4a1", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb0a2", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb1a2", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb2a2", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb3a2", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb4a2", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb0a3", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb1a3", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb2a3", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb3a3", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb4a3", tmp1), tmp2);
#if defined(R8500)
				nvram_set(strcat_r(prefix,"mcsbw205glpo", tmp1), "0x86FC8666");
				nvram_set(strcat_r(prefix,"mcsbw405glpo", tmp1), "0x86FC8666");
				nvram_set(strcat_r(prefix,"mcsbw805glpo", tmp1), "0x86FC8666");
				nvram_set(strcat_r(prefix,"mcsbw1605glpo", tmp1), "0x86FC8666");
				nvram_set(strcat_r(prefix,"mcs1024qam5glpo", tmp1), "0x88888888");
				nvram_set(strcat_r(prefix,"mcslr5glpo", tmp1), "0");
				nvram_set(strcat_r(prefix,"mcsbw205gmpo", tmp1), "0x86FC8666");
				nvram_set(strcat_r(prefix,"mcsbw405gmpo", tmp1), "0x86FC8666");
				nvram_set(strcat_r(prefix,"mcsbw805gmpo", tmp1), "0x86FC8666");
				nvram_set(strcat_r(prefix,"mcsbw1605gmpo", tmp1), "0x86FC8666");
				nvram_set(strcat_r(prefix,"mcs1024qam5gmpo", tmp1), "0x88888888");
				nvram_set(strcat_r(prefix,"mcslr5gmpo", tmp1), "0");
				nvram_set(strcat_r(prefix,"mcs10poexp", tmp1), "0xFFFFFFFF");
				nvram_set(strcat_r(prefix,"mcs11poexp", tmp1), "0xFFFFFFFF");
#else
#if 0
				nvram_set(strcat_r(prefix,"mcsbw205glpo", tmp1), "0x98653210");
				nvram_set(strcat_r(prefix,"mcsbw405glpo", tmp1), "0x98653210");
				nvram_set(strcat_r(prefix,"mcsbw805glpo", tmp1), "0x98653210");
				nvram_set(strcat_r(prefix,"mcsbw1605glpo", tmp1), "0x00000000");
				nvram_set(strcat_r(prefix,"mcs1024qam5glpo", tmp1), "0xBABABABA");
				nvram_set(strcat_r(prefix,"mcslr5glpo", tmp1), "0");
				nvram_set(strcat_r(prefix,"mcsbw205gmpo", tmp1), "0x98653210");
				nvram_set(strcat_r(prefix,"mcsbw405gmpo", tmp1), "0x98653210");
				nvram_set(strcat_r(prefix,"mcsbw805gmpo", tmp1), "0x98653210");
				nvram_set(strcat_r(prefix,"mcsbw1605gmpo", tmp1), "0x00000000");
				nvram_set(strcat_r(prefix,"mcs1024qam5gmpo", tmp1), "0xBABABABA");
				nvram_set(strcat_r(prefix,"mcslr5gmpo", tmp1), "0");
				nvram_set(strcat_r(prefix,"mcs10poexp", tmp1), "0");
				nvram_set(strcat_r(prefix,"mcs11poexp", tmp1), "0");
#endif
#endif
				commit_needed++;
			}
			if(unit & 4){
				snprintf(prefix, sizeof(prefix), "2:");
				snprintf(tmp2, sizeof(tmp2), "%d", p3);
				nvram_set(strcat_r(prefix, "maxp5gb0a0", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb1a0", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb2a0", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb3a0", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb4a0", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb0a1", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb1a1", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb2a1", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb3a1", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb4a1", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb0a2", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb1a2", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb2a2", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb3a2", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb4a2", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb0a3", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb1a3", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb2a3", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb3a3", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb4a3", tmp1), tmp2);
#if defined(R8500)
				nvram_set(strcat_r(prefix,"mcsbw205ghpo", tmp1), "0x86FC8666");
				nvram_set(strcat_r(prefix,"mcsbw405ghpo", tmp1), "0x86FC8666");
				nvram_set(strcat_r(prefix,"mcsbw805ghpo", tmp1), "0x86FC8666");
				nvram_set(strcat_r(prefix,"mcsbw1605ghpo", tmp1), "0x86FC8666");
				nvram_set(strcat_r(prefix,"mcs1024qam5ghpo", tmp1), "0x88888888");
				nvram_set(strcat_r(prefix,"mcslr5ghpo", tmp1), "0");
				nvram_set(strcat_r(prefix,"mcsbw205gx1po", tmp1), "0x86FC8666");
				nvram_set(strcat_r(prefix,"mcsbw405gx1po", tmp1), "0x86FC8666");
				nvram_set(strcat_r(prefix,"mcsbw805gx1po", tmp1), "0x86FC8666");
				nvram_set(strcat_r(prefix,"mcsbw1605gx1po", tmp1), "0x86FC8666");
				nvram_set(strcat_r(prefix,"mcs1024qam5gx1po", tmp1), "0x88888888");
				nvram_set(strcat_r(prefix,"mcslr5gx1po", tmp1), "0");
				nvram_set(strcat_r(prefix,"mcsbw205gx2po", tmp1), "0x86FC8666");
				nvram_set(strcat_r(prefix,"mcsbw405gx2po", tmp1), "0x86FC8666");
				nvram_set(strcat_r(prefix,"mcsbw805gx2po", tmp1), "0x86FC8666");
				nvram_set(strcat_r(prefix,"mcsbw1605gx2po", tmp1), "0x86FC8666");
				nvram_set(strcat_r(prefix,"mcs1024qam5gx2po", tmp1), "0x88888888");
				nvram_set(strcat_r(prefix,"mcslr5gx2po", tmp1), "0");
				nvram_set(strcat_r(prefix,"mcs10poexp", tmp1), "0xFFFFFFFF");
				nvram_set(strcat_r(prefix,"mcs11poexp", tmp1), "0xFFFFFFFF");
#else
#if 0
				nvram_set(strcat_r(prefix,"mcsbw205ghpo", tmp1), "0x44443210");
				nvram_set(strcat_r(prefix,"mcsbw405ghpo", tmp1), "0x44443210");
				nvram_set(strcat_r(prefix,"mcsbw805ghpo", tmp1), "0x44443210");
				nvram_set(strcat_r(prefix,"mcsbw1605ghpo", tmp1), "0x00000000");
				nvram_set(strcat_r(prefix,"mcs1024qam5ghpo", tmp1), "0x44444444");
				nvram_set(strcat_r(prefix,"mcslr5ghpo", tmp1), "0");
				nvram_set(strcat_r(prefix,"mcsbw205gx1po", tmp1), "0x44443210");
				nvram_set(strcat_r(prefix,"mcsbw405gx1po", tmp1), "0x44443210");
				nvram_set(strcat_r(prefix,"mcsbw805gx1po", tmp1), "0x44443210");
				nvram_set(strcat_r(prefix,"mcsbw1605gx1po", tmp1), "0x00000000");
				nvram_set(strcat_r(prefix,"mcs1024qam5gx1po", tmp1), "0x44444444");
				nvram_set(strcat_r(prefix,"mcslr5gx1po", tmp1), "0");
				nvram_set(strcat_r(prefix,"mcsbw205gx2po", tmp1), "0x66665430");
				nvram_set(strcat_r(prefix,"mcsbw405gx2po", tmp1), "0x66665430");
				nvram_set(strcat_r(prefix,"mcsbw805gx2po", tmp1), "0x66665430");
				nvram_set(strcat_r(prefix,"mcsbw1605gx2po", tmp1), "0x00000000");
				nvram_set(strcat_r(prefix,"mcs1024qam5gx2po", tmp1), "0x66666666");
				nvram_set(strcat_r(prefix,"mcslr5gx2po", tmp1), "0");
				nvram_set(strcat_r(prefix,"mcs10poexp", tmp1), "0");
				nvram_set(strcat_r(prefix,"mcs11poexp", tmp1), "0");
#endif
#endif
				commit_needed++;
			}
			break;
		case MODEL_RTAC86U:
			if(unit & 1){
				snprintf(prefix, sizeof(prefix), "0:");
				snprintf(tmp2, sizeof(tmp2), "%d", p1);
				nvram_set(strcat_r(prefix,"maxp2ga0", tmp1), tmp2);
				nvram_set(strcat_r(prefix,"maxp2ga1", tmp1), tmp2);
				nvram_set(strcat_r(prefix,"maxp2ga2", tmp1), tmp2);
				nvram_set(strcat_r(prefix,"maxp2ga3", tmp1), tmp2);
#if 0
				nvram_set(strcat_r(prefix,"cckbw202gpo", tmp1), "0");
				nvram_set(strcat_r(prefix,"cckbw20ul2gpo", tmp1), "0");
				nvram_set(strcat_r(prefix,"ofdmlrbw202gpo", tmp1), "0");
				nvram_set(strcat_r(prefix,"dot11agofdmhrbw202gpo", tmp1), "0x4210");
				nvram_set(strcat_r(prefix,"mcsbw202gpo", tmp1), "0xB9872100");
				nvram_set(strcat_r(prefix,"mcsbw402gpo", tmp1), "0xB9872100");
				nvram_set(strcat_r(prefix,"mcs1024qam2gpo", tmp1), "0xEDEDEDED");
				nvram_set(strcat_r(prefix,"mcs10poexp", tmp1), "0");
				nvram_set(strcat_r(prefix,"mcs11poexp", tmp1), "0");
#endif
				commit_needed++;
			}
			if(unit & 2){
				snprintf(prefix, sizeof(prefix), "1:");
				snprintf(tmp2, sizeof(tmp2), "%d", p2);
				nvram_set(strcat_r(prefix, "maxp5gb0a0", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb1a0", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb2a0", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb3a0", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb4a0", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb0a1", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb1a1", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb2a1", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb3a1", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb4a1", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb0a2", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb1a2", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb2a2", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb3a2", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb4a2", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb0a3", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb1a3", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb2a3", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb3a3", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb4a3", tmp1), tmp2);
#if 0
				nvram_set(strcat_r(prefix,"mcsbw205glpo", tmp1), "0x98653210");
				nvram_set(strcat_r(prefix,"mcsbw405glpo", tmp1), "0x98653210");
				nvram_set(strcat_r(prefix,"mcsbw805glpo", tmp1), "0x98653210");
				nvram_set(strcat_r(prefix,"mcsbw1605glpo", tmp1), "0x00000000");
				nvram_set(strcat_r(prefix,"mcs1024qam5glpo", tmp1), "0xBABABABA");
				nvram_set(strcat_r(prefix,"mcslr5glpo", tmp1), "0");
				nvram_set(strcat_r(prefix,"mcsbw205gmpo", tmp1), "0x98653210");
				nvram_set(strcat_r(prefix,"mcsbw405gmpo", tmp1), "0x98653210");
				nvram_set(strcat_r(prefix,"mcsbw805gmpo", tmp1), "0x98653210");
				nvram_set(strcat_r(prefix,"mcsbw1605gmpo", tmp1), "0x00000000");
				nvram_set(strcat_r(prefix,"mcs1024qam5gmpo", tmp1), "0xBABABABA");
				nvram_set(strcat_r(prefix,"mcslr5gmpo", tmp1), "0");
				nvram_set(strcat_r(prefix,"mcs10poexp", tmp1), "0");
				nvram_set(strcat_r(prefix,"mcs11poexp", tmp1), "0");
#endif
				commit_needed++;
			}
			break;
		case MODEL_GTAC5300:
			if(unit & 1){
				snprintf(prefix, sizeof(prefix), "1:");
				snprintf(tmp2, sizeof(tmp2), "%d", p1);
				nvram_set(strcat_r(prefix,"maxp2ga0", tmp1), tmp2);
				nvram_set(strcat_r(prefix,"maxp2ga1", tmp1), tmp2);
				nvram_set(strcat_r(prefix,"maxp2ga2", tmp1), tmp2);
				nvram_set(strcat_r(prefix,"maxp2ga3", tmp1), tmp2);
#if defined(R8000P)
				nvram_set(strcat_r(prefix,"cckbw202gpo", tmp1), "0");
				nvram_set(strcat_r(prefix,"cckbw20ul2gpo", tmp1), "0");
				nvram_set(strcat_r(prefix,"ofdmlrbw202gpo", tmp1), "0");
				nvram_set(strcat_r(prefix,"dot11agofdmhrbw202gpo", tmp1), "0x6420");
				nvram_set(strcat_r(prefix,"mcsbw202gpo", tmp1), "0xEC864200");
				nvram_set(strcat_r(prefix,"mcsbw402gpo", tmp1), "0xEC864200");
				nvram_set(strcat_r(prefix,"mcs1024qam2gpo", tmp1), "0x5252");
				nvram_set(strcat_r(prefix,"mcs10poexp", tmp1), "0x11111111");
				nvram_set(strcat_r(prefix,"mcs11poexp", tmp1), "0x11111111");
#else
#if 0
				nvram_set(strcat_r(prefix,"cckbw202gpo", tmp1), "0");
				nvram_set(strcat_r(prefix,"cckbw20ul2gpo", tmp1), "0");
				nvram_set(strcat_r(prefix,"ofdmlrbw202gpo", tmp1), "0");
				nvram_set(strcat_r(prefix,"dot11agofdmhrbw202gpo", tmp1), "0x4210");
				nvram_set(strcat_r(prefix,"mcsbw202gpo", tmp1), "0xB9872100");
				nvram_set(strcat_r(prefix,"mcsbw402gpo", tmp1), "0xB9872100");
				nvram_set(strcat_r(prefix,"mcs1024qam2gpo", tmp1), "0xEDEDEDED");
				nvram_set(strcat_r(prefix,"mcs10poexp", tmp1), "0");
				nvram_set(strcat_r(prefix,"mcs11poexp", tmp1), "0");
#endif
#endif
				commit_needed++;
			}
			if(unit & 2){
				snprintf(prefix, sizeof(prefix), "2:");
				snprintf(tmp2, sizeof(tmp2), "%d", p2);
				nvram_set(strcat_r(prefix, "maxp5gb0a0", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb1a0", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb2a0", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb3a0", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb4a0", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb0a1", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb1a1", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb2a1", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb3a1", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb4a1", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb0a2", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb1a2", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb2a2", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb3a2", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb4a2", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb0a3", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb1a3", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb2a3", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb3a3", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb4a3", tmp1), tmp2);
#if defined(R8000P)
				nvram_set(strcat_r(prefix,"mcsbw205glpo", tmp1), "0xECA86420");
				nvram_set(strcat_r(prefix,"mcsbw405glpo", tmp1), "0xECA86420");
				nvram_set(strcat_r(prefix,"mcsbw805glpo", tmp1), "0xECA86420");
				nvram_set(strcat_r(prefix,"mcsbw1605glpo", tmp1), "0xECA86420");
				nvram_set(strcat_r(prefix,"mcs1024qam5glpo", tmp1), "0x52525252");
				nvram_set(strcat_r(prefix,"mcslr5glpo", tmp1), "0");
				nvram_set(strcat_r(prefix,"mcsbw205gmpo", tmp1), "0xECA86420");
				nvram_set(strcat_r(prefix,"mcsbw405gmpo", tmp1), "0xECA86420");
				nvram_set(strcat_r(prefix,"mcsbw805gmpo", tmp1), "0xECA86420");
				nvram_set(strcat_r(prefix,"mcsbw1605gmpo", tmp1), "0xECA86420");
				nvram_set(strcat_r(prefix,"mcs1024qam5gmpo", tmp1), "0x52525252");
				nvram_set(strcat_r(prefix,"mcslr5gmpo", tmp1), "0");
				nvram_set(strcat_r(prefix,"mcs10poexp", tmp1), "0x11111111");
				nvram_set(strcat_r(prefix,"mcs11poexp", tmp1), "0x11111111");
#else
#if 0
				nvram_set(strcat_r(prefix,"mcsbw205glpo", tmp1), "0x98653210");
				nvram_set(strcat_r(prefix,"mcsbw405glpo", tmp1), "0x98653210");
				nvram_set(strcat_r(prefix,"mcsbw805glpo", tmp1), "0x98653210");
				nvram_set(strcat_r(prefix,"mcsbw1605glpo", tmp1), "0x00000000");
				nvram_set(strcat_r(prefix,"mcs1024qam5glpo", tmp1), "0xBABABABA");
				nvram_set(strcat_r(prefix,"mcslr5glpo", tmp1), "0");
				nvram_set(strcat_r(prefix,"mcsbw205gmpo", tmp1), "0x98653210");
				nvram_set(strcat_r(prefix,"mcsbw405gmpo", tmp1), "0x98653210");
				nvram_set(strcat_r(prefix,"mcsbw805gmpo", tmp1), "0x98653210");
				nvram_set(strcat_r(prefix,"mcsbw1605gmpo", tmp1), "0x00000000");
				nvram_set(strcat_r(prefix,"mcs1024qam5gmpo", tmp1), "0xBABABABA");
				nvram_set(strcat_r(prefix,"mcslr5gmpo", tmp1), "0");
				nvram_set(strcat_r(prefix,"mcs10poexp", tmp1), "0");
				nvram_set(strcat_r(prefix,"mcs11poexp", tmp1), "0");
#endif
#endif
				commit_needed++;
			}
			if(unit & 4){
				snprintf(prefix, sizeof(prefix), "3:");
				snprintf(tmp2, sizeof(tmp2), "%d", p3);
				nvram_set(strcat_r(prefix, "maxp5gb0a0", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb1a0", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb2a0", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb3a0", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb4a0", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb0a1", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb1a1", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb2a1", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb3a1", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb4a1", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb0a2", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb1a2", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb2a2", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb3a2", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb4a2", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb0a3", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb1a3", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb2a3", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb3a3", tmp1), tmp2);
				nvram_set(strcat_r(prefix, "maxp5gb4a3", tmp1), tmp2);
#if defined(R8000P)
				nvram_set(strcat_r(prefix,"mcsbw205ghpo", tmp1), "0xECA86420");
				nvram_set(strcat_r(prefix,"mcsbw405ghpo", tmp1), "0xECA86420");
				nvram_set(strcat_r(prefix,"mcsbw805ghpo", tmp1), "0xECA86420");
				nvram_set(strcat_r(prefix,"mcsbw1605ghpo", tmp1), "0xECA86420");
				nvram_set(strcat_r(prefix,"mcs1024qam5ghpo", tmp1), "0x52525252");
				nvram_set(strcat_r(prefix,"mcslr5ghpo", tmp1), "0");
				nvram_set(strcat_r(prefix,"mcsbw205gx1po", tmp1), "0xECA86420");
				nvram_set(strcat_r(prefix,"mcsbw405gx1po", tmp1), "0xECA86420");
				nvram_set(strcat_r(prefix,"mcsbw805gx1po", tmp1), "0xECA86420");
				nvram_set(strcat_r(prefix,"mcsbw1605gx1po", tmp1), "0xECA86420");
				nvram_set(strcat_r(prefix,"mcs1024qam5gx1po", tmp1), "0x52525252");
				nvram_set(strcat_r(prefix,"mcslr5gx1po", tmp1), "0");
				nvram_set(strcat_r(prefix,"mcsbw205gx2po", tmp1), "0xECA86420");
				nvram_set(strcat_r(prefix,"mcsbw405gx2po", tmp1), "0xECA86420");
				nvram_set(strcat_r(prefix,"mcsbw805gx2po", tmp1), "0xECA86420");
				nvram_set(strcat_r(prefix,"mcsbw1605gx2po", tmp1), "0xECA86420");
				nvram_set(strcat_r(prefix,"mcs1024qam5gx2po", tmp1), "0x52525252");
				nvram_set(strcat_r(prefix,"mcslr5gx2po", tmp1), "0");
				nvram_set(strcat_r(prefix,"mcs10poexp", tmp1), "0xFFFFFFFF");
				nvram_set(strcat_r(prefix,"mcs11poexp", tmp1), "0xFFFFFFFF");
#else
#if 0
				nvram_set(strcat_r(prefix,"mcsbw205ghpo", tmp1), "0x98653210");
				nvram_set(strcat_r(prefix,"mcsbw405ghpo", tmp1), "0x98653210");
				nvram_set(strcat_r(prefix,"mcsbw805ghpo", tmp1), "0x98653210");
				nvram_set(strcat_r(prefix,"mcsbw1605ghpo", tmp1), "0x00000000");
				nvram_set(strcat_r(prefix,"mcs1024qam5ghpo", tmp1), "0xBABABABA");
				nvram_set(strcat_r(prefix,"mcslr5ghpo", tmp1), "0");
				nvram_set(strcat_r(prefix,"mcsbw205gx1po", tmp1), "0x98653210");
				nvram_set(strcat_r(prefix,"mcsbw405gx1po", tmp1), "0x98653210");
				nvram_set(strcat_r(prefix,"mcsbw805gx1po", tmp1), "0x98653210");
				nvram_set(strcat_r(prefix,"mcsbw1605gx1po", tmp1), "0x00000000");
				nvram_set(strcat_r(prefix,"mcs1024qam5gx1po", tmp1), "0xBABABABA");
				nvram_set(strcat_r(prefix,"mcslr5gx1po", tmp1), "0");
				nvram_set(strcat_r(prefix,"mcsbw205gx2po", tmp1), "0xBA875430");
				nvram_set(strcat_r(prefix,"mcsbw405gx2po", tmp1), "0xBA875430");
				nvram_set(strcat_r(prefix,"mcsbw805gx2po", tmp1), "0xBA875430");
				nvram_set(strcat_r(prefix,"mcsbw1605gx2po", tmp1), "0x00000000");
				nvram_set(strcat_r(prefix,"mcs1024qam5gx2po", tmp1), "0xDCDCDCDC");
				nvram_set(strcat_r(prefix,"mcslr5gx2po", tmp1), "0");
				nvram_set(strcat_r(prefix,"mcs10poexp", tmp1), "0");
				nvram_set(strcat_r(prefix,"mcs11poexp", tmp1), "0");
#endif
#endif
				commit_needed++;
			}
			break;
		default:
			_dprintf("############################ unknown model(%s:%d) #################################\n",__func__, __LINE__);
			break;
	}
	if(commit_needed)
		nvram_commit();
#endif
	return 0;
}
#endif
