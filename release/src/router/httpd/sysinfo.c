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
#include <ctype.h>
#include <errno.h>
#include <unistd.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <httpd.h>
#include <fcntl.h>
#include <signal.h>
#include <stdarg.h>
#include <sys/wait.h>
#include <dirent.h>
#include <sys/ioctl.h>

#include <typedefs.h>
#include <bcmutils.h>
#include <shutils.h>
#include <bcmnvram.h>
#include <bcmnvram_f.h>
#include <common.h>
#include <shared.h>
#include <rtstate.h>

#if defined(HND_ROUTER)
#include "bcmwifi_rates.h"
#include "wlioctl_defs.h"
#endif
#if defined(RTCONFIG_RALINK)
#include <ralink.h>
#else
#include <wlioctl.h>
#endif
#include <wlutils.h>
#include <sys/sysinfo.h>
#include <sys/statvfs.h>
#include <linux/version.h>

#ifdef RTCONFIG_USB
#include <disk_io_tools.h>
#include <disk_initial.h>
#include <disk_share.h>

#else
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
#endif

#include "sysinfo.h"

#ifdef RTCONFIG_QTN
#include "web-qtn.h"
#endif

#ifdef RTCONFIG_EXT_RTL8365MB
#include <linux/major.h>
#include <rtk_switch.h>
#include <rtk_types.h>

#define RTKSWITCH_DEV   "/dev/rtkswitch"

typedef struct {
        unsigned int link[4];
        unsigned int speed[4];
} phyState;
#endif



unsigned int get_phy_temperature(int radio);
unsigned int get_wifi_clients(int unit, int querytype);

#ifdef RTCONFIG_QTN
unsigned int get_qtn_temperature(void);
unsigned int get_qtn_version(char *version, int len);
int GetPhyStatus_qtn(void);
#endif

#ifdef RTCONFIG_EXT_RTL8365MB
void GetPhyStatus_rtk(int *states);
#endif


#define MBYTES (1024 * 1024)
#define KBYTES 1024

#define SI_WL_QUERY_ASSOC 1
#define SI_WL_QUERY_AUTHE 2
#define SI_WL_QUERY_AUTHO 3


int ej_show_sysinfo(int eid, webs_t wp, int argc, char_t ** argv)
{
	char *type;
	char result[2048];
	int retval = 0;
	struct sysinfo sys;
	char *tmp;

	strcpy(result,"None");

	if (ejArgs(argc, argv, "%s", &type) < 1) {
		websError(wp, 400, "Insufficient args\n");
		return retval;
	}

	if (type) {
		if (strcmp(type,"cpu.model") == 0) {
			char *buffer = read_whole_file("/proc/cpuinfo");

			if (buffer) {
				int count = 0;
				char model[64];
#if defined(BCM4912)
					strcpy(model, "BCM4912 - Cortex A53 ARMv8");
#elif defined(RTCONFIG_BCMARM) || defined(RTCONFIG_HND_ROUTER) || defined(RTCONFIG_QCA) || defined(RTCONFIG_RALINK_MT7622) || defined(RTCONFIG_MT798X)
					char impl[8], arch[8], variant[8], part[10], revision[4];
					impl[0]='\0'; arch[0]='\0'; variant[0]='\0'; part[0]='\0';
					strcpy(revision,"0");

					tmp = strstr(buffer, "CPU implementer");
					if (tmp) sscanf(tmp, "CPU implementer  :  %7[^\n]s", impl);
					tmp = strstr(buffer, "CPU architecture");
					if (tmp) sscanf(tmp, "CPU architecture  :  %7[^\n]s", arch);
					tmp = strstr(buffer, "CPU variant");
					if (tmp) sscanf(tmp, "CPU variant  :  %7[^\n]s", variant);
					tmp = strstr(buffer, "CPU part");
					if (tmp) sscanf(tmp, "CPU part  :  %9[^\n]s", part);
					tmp = strstr(buffer,"CPU revision");
					if (tmp) sscanf(tmp, "CPU revision  :  %3[^\n]s", revision);
#if defined(RTCONFIG_QCA)
					if (!strcmp(impl, "0x41")
					    && !strcmp(variant, "0x0")
					    && !strcmp(part, "0xc07")
					    && !strcmp(arch, "7"))
						sprintf(model, "IPQ401x - Cortex A7 ARMv7 revision %s", revision);
					else if (!strcmp(impl, "0x41")//kernel:32/64
					    && !strcmp(variant, "0x0")
					    && !strcmp(part, "0xd03")
					    && (!strcmp(arch, "7") || !strcmp(arch, "8")))
						sprintf(model, "IPQ807x - Cortex A53 ARMv8 revision %s", revision);
					else if (!strcmp(impl, "0x51")//kernel:32/64
					    && !strcmp(variant, "0x0")
					    && !strcmp(part, "0x801")
					    && (!strcmp(arch, "7") || !strcmp(arch, "8")))
						sprintf(model, "IPQ50xx - Cortex A53 ARMv8 revision %s", revision);
					else if (!strcmp(impl, "0x51")//kernel:32/64
					    && !strcmp(variant, "0xa")
					    && !strcmp(part, "0x801")
					    && (!strcmp(arch, "7") || !strcmp(arch, "8")))
						sprintf(model, "IPQ60xx - Cortex A53 ARMv8 revision %s", revision);
					else if (!strcmp(variant, "0x2")
					    && !strcmp(part, "0xd03")
					    && !strcmp(arch, "7"))
						sprintf(model, "IPQ806x - Cortex A15 ARMv7 revision %s", revision);
#elif defined(RTCONFIG_RALINK_MT7622) || defined(RTCONFIG_MT798X)
					if (!strcmp(impl, "0x41")//kernel:32/64
					    && !strcmp(variant, "0x0")
					    && !strcmp(part, "0xd03")
					    && (!strcmp(arch, "7") || !strcmp(arch, "8"))){
#if defined(RTCONFIG_RALINK_MT7622)
					sprintf(model, "MT7622 - Cortex A53 ARMv8 revision %s", revision);
#elif defined(RTCONFIG_MT798X)
						char *buffer;
						doSystem("devmem 0x8000000 16 > /tmp/mtk_chip");
						buffer = read_whole_file("/tmp/mtk_chip");
						if(buffer){
							if(strstr(buffer, "7986"))
								sprintf(model, "MT7986X - Cortex A53 ARMv8 revision %s", revision);
							else if(strstr(buffer, "7981"))
								sprintf(model, "MT7981X - Cortex A53 ARMv8 revision %s", revision);
							else
								sprintf(model, "MT%s - Cortex A53 ARMv8 revision %s", buffer + 2, revision);
							free(buffer);
						}
						unlink("/tmp/mtk_chip");
#endif
					}
#else
					if (!strcmp(impl, "0x42")
					    && !strcmp(variant, "0x0")
					    && !strcmp(part, "0x100")
					    && !strcmp(arch, "8"))
						sprintf(model, "BCM490x - Cortex A53 ARMv8 revision %s", revision);
					else if (!strcmp(impl, "0x41")
					    && !strcmp(variant, "0x0")
					    && !strcmp(part, "0xc07")
					    && !strcmp(arch, "7"))
						sprintf(model, "BCM675x - Cortex A7 ARMv7 revision %s", revision);
					else if (!strcmp(impl, "0x41")
					    && !strcmp(variant, "0x3")
					    && !strcmp(part, "0xc09")
					    && !strcmp(arch, "7"))
						sprintf(model, "BCM470x - Cortex A7 ARMv7 revision %s", revision);
#endif
					else
						sprintf(model, "Implementer: %s, Part: %s, Variant: %s, Arch: %s, Rev: %s",impl, part, variant, arch, revision);
#elif defined(RTCONFIG_LANTIQ) || defined(RTCONFIG_RALINK)
				tmp = strstr(buffer, "system type");
				if (tmp)
					sscanf(tmp, "system type  :  %[^\n]", model);
				else
#if defined(RTCONFIG_LANTIQ)
					strcpy(model, "GRX500 rev 1.2");
#elif defined(RTCONFIG_RALINK)
					strcpy(model, "mt7621a");
#endif
#endif
				count = sysconf(_SC_NPROCESSORS_CONF);
				if (count > 1) {
					tmp = nvram_safe_get("cpurev");
					if (*tmp)
						sprintf(result, "%s&nbsp;&nbsp;-&nbsp;&nbsp; Rev. %s (Cores: %d)", model, tmp, count);
					else
						sprintf(result, "%s&nbsp;&nbsp; (Cores: %d)", model, count);
				} else {
					strcpy(result, model);
				}

				free(buffer);
			}

		} else if(strcmp(type,"cpu.freq") == 0) {
#if defined(RTCONFIG_HND_ROUTER) || defined(RTCONFIG_BCMARM)
#if defined(BCM4912)
			if (1)
				strcpy(result, "2000");
			else
#elif defined(RTCONFIG_HND_ROUTER)
			int freq = 0;
			char *buffer;

			buffer = read_whole_file("/sys/devices/system/cpu/bcm_arm_cpuidle/admin_max_freq");

			if (buffer) {
				sscanf(buffer, "%d", &freq);
				free(buffer);
				sprintf(result, "%d", freq);
			}
#if (defined(RTCONFIG_HND_ROUTER_AX_675X) && !defined(RTCONFIG_HND_ROUTER_AX_6710))
			else if (1)
				strcpy(result, "1500");
#elif defined(RTCONFIG_HND_ROUTER_AX_6756)
			else if (1)
				strcpy(result, "1700");
#endif
			else
#endif
			{
				tmp = nvram_safe_get("clkfreq");
				if (*tmp)
					sscanf(tmp,"%[^,]s", result);
			}
#elif defined(RTCONFIG_LANTIQ) || defined(RTCONFIG_QCA) || defined(RTCONFIG_RALINK_MT7622)
			int freq = 0;
			char *buffer;

			buffer = read_whole_file("/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_max_freq");

			if (buffer) {
				sscanf(buffer, "%d", &freq);
				free(buffer);
				sprintf(result, "%d", (freq/1000));
			}
			else
				strcpy(result, "0");//bug?
#elif defined(RTCONFIG_SOC_MT7981)
			strcpy(result, "1800");//1800/1300
#elif defined(RTCONFIG_MT798X)
			strcpy(result, "2000");//2000/1600
#elif defined(RTCONFIG_RALINK)
			int freq = 0;
			char *buffer = read_whole_file("/sys/kernel/debug/clk/cpuclock/clk_rate");
			if (buffer) {
				sscanf(buffer, "%d", &freq);
				free(buffer);
				sprintf(result, "%d", (freq/1000000));
			}
			else
				strcpy(result, "0");//bug?
#endif
		} else if(strcmp(type,"memory.total") == 0) {
			sysinfo(&sys);
			sprintf(result,"%.2f",(float) sys.totalram * sys.mem_unit / MBYTES);
		} else if(strcmp(type,"memory.free") == 0) {
			sysinfo(&sys);
			sprintf(result,"%.2f",(float) sys.freeram * sys.mem_unit / MBYTES);
		} else if(strcmp(type,"memory.buffer") == 0) {
			sysinfo(&sys);
			sprintf(result,"%.2f",(float) sys.bufferram * sys.mem_unit / MBYTES);
		} else if(strcmp(type,"memory.swap.total") == 0) {
			sysinfo(&sys);
			sprintf(result,"%.2f",(float) sys.totalswap * sys.mem_unit / MBYTES);
		} else if(strcmp(type,"memory.swap.used") == 0) {
			sysinfo(&sys);
			sprintf(result,"%.2f",(float) (sys.totalswap - sys.freeswap) * sys.mem_unit / MBYTES);
		} else if(strcmp(type,"memory.cache") == 0) {
			int size = 0;
			char *buffer = read_whole_file("/proc/meminfo");

			if (buffer) {
				tmp = strstr(buffer, "Cached");
				if (tmp)
					sscanf(tmp, "Cached:            %d kB\n", &size);
				free(buffer);
				sprintf(result,"%.2f", (size / (float)KBYTES));
			} else {
				strcpy(result,"??");
			}
		} else if(strcmp(type,"cpu.load.1") == 0) {
			sysinfo(&sys);
			sprintf(result,"%.2f",(sys.loads[0] / (float)(1<<SI_LOAD_SHIFT)));
		} else if(strcmp(type,"cpu.load.5") == 0) {
			sysinfo(&sys);
			sprintf(result,"%.2f",(sys.loads[1] / (float)(1<<SI_LOAD_SHIFT)));
		} else if(strcmp(type,"cpu.load.15") == 0) {
			sysinfo(&sys);
			sprintf(result,"%.2f",(sys.loads[2] / (float)(1<<SI_LOAD_SHIFT)));
		} else if(strcmp(type,"nvram.total") == 0) {
			sprintf(result,"%d",MAX_NVRAM_SPACE);
		} else if(strcmp(type,"nvram.used") == 0) {
			int size = 0;
#ifdef HND_ROUTER
			size = f_size("/data/.kernel_nvram.setting");
			if (size == -1)
#endif
			{
				char *buf;

			buf = malloc(MAX_NVRAM_SPACE);
			if (buf) {
#if defined(RTCONFIG_HND_ROUTER) || defined(RTCONFIG_LANTIQ) || defined(RTCONFIG_QCA) || defined(RTCONFIG_RALINK)
				nvram_getall(buf, MAX_NVRAM_SPACE);
#else
				dev_nvram_getall(buf, MAX_NVRAM_SPACE);
#endif
				tmp = buf;
				while (*tmp) tmp += strlen(tmp) +1;

					size = sizeof(struct nvram_header) + (int) tmp - (int) buf;
					free(buf);
				}
			}
			sprintf(result,"%d",size);

		} else if(strcmp(type,"jffs.usage") == 0) {
			struct statvfs fiData;

			char *mount_info = read_whole_file("/proc/mounts");

			if ((mount_info) && (strstr(mount_info, "/jffs")) && (statvfs("/jffs",&fiData) == 0 )) {
				sprintf(result,"%.2f / %.2f MB",((float) (fiData.f_blocks-fiData.f_bfree) * fiData.f_frsize / MBYTES) ,((float) fiData.f_blocks * fiData.f_frsize / MBYTES));
			} else {
				strcpy(result,"<i>Unmounted</i>");
			}

			if (mount_info) free(mount_info);

		} else if(strncmp(type,"temperature",11) == 0) {
			unsigned int temperature;
			int radio;

			if (sscanf(type,"temperature.%d", &radio) != 1)
				temperature = 0;
			else
			{
#ifdef RTCONFIG_QTN
				if (radio == 1)
					temperature = get_qtn_temperature();
				else
#endif
					temperature = get_phy_temperature(radio);
			}
			if (temperature == 0)
				strcpy(result,"<i>disabled</i>");
			else
				sprintf(result,"%u&deg;C", temperature);

		} else if(strcmp(type,"conn.total") == 0) {
			FILE* fp;
#if defined(RTCONFIG_HND_ROUTER) || defined(RTCONFIG_QCA) || defined(RTCONFIG_RALINK) || defined(RTCONFIG_LANTIQ)
			fp = fopen("/proc/sys/net/netfilter/nf_conntrack_count", "r");//kernel3+
#else
			fp = fopen("/proc/sys/net/ipv4/netfilter/ip_conntrack_count", "r");//kernel2.6
#endif
			if (fp) {
				if (fgets(result, sizeof(result), fp) == NULL)
					strcpy(result, "error");
				else
					result[strcspn(result, "\n")] = 0;
				fclose(fp);
			}
		} else if(strcmp(type,"conn.active") == 0) {
			char buf[256];
			FILE* fp;
			unsigned int established = 0;

			fp = fopen("/proc/net/nf_conntrack", "r");
			if (fp) {
				while (fgets(buf, sizeof(buf), fp) != NULL) {
				if (strstr(buf,"ESTABLISHED") || ((strstr(buf,"udp")) && (strstr(buf,"ASSURED"))))
					established++;
				}
				fclose(fp);
			}
			sprintf(result,"%u",established);

		} else if(strcmp(type,"conn.max") == 0) {
			FILE* fp;
#if defined(RTCONFIG_HND_ROUTER) || defined(RTCONFIG_QCA) || defined(RTCONFIG_RALINK) || defined(RTCONFIG_LANTIQ)
			fp = fopen("/proc/sys/net/netfilter/nf_conntrack_max", "r");//kernel3+
#else
			fp = fopen("/proc/sys/net/ipv4/netfilter/ip_conntrack_max", "r");//kernel2.6
#endif
			if (fp) {
				if (fgets(result, sizeof(result), fp) == NULL)
					strcpy(result, "error");
				else
					result[strcspn(result, "\n")] = 0;
				fclose(fp);
			}
		} else if(strncmp(type,"conn.wifi",9) == 0) {
#if defined(RTCONFIG_LANTIQ) || defined(RTCONFIG_QCA) || defined(RTCONFIG_RALINK)
			strcpy(result,"<i>off</i>");
#elif defined(RTCONFIG_BCMARM) || defined(RTCONFIG_HND_ROUTER)
			int count, radio;
			char command[10];

			sscanf(type,"conn.wifi.%d.%9s", &radio, command);

			if (strcmp(command,"autho") == 0) {
				count = get_wifi_clients(radio,SI_WL_QUERY_AUTHO);
			} else if (strcmp(command,"authe") == 0) {
				count = get_wifi_clients(radio,SI_WL_QUERY_AUTHE);
			} else if (strcmp(command,"assoc") == 0) {
				count = get_wifi_clients(radio,SI_WL_QUERY_ASSOC);
			} else {
				count = 0;
			}
			if (count == -1)
				strcpy(result,"<i>off</i>");
			else
				sprintf(result,"%d",count);
#endif
		} else if(strcmp(type,"driver_version") == 0 ) {
#if defined(RTCONFIG_BCMARM) || defined(RTCONFIG_HND_ROUTER)
			system("/usr/sbin/wl ver >/tmp/output.txt");

			char *buffer = read_whole_file("/tmp/output.txt");

			if (buffer) {
				if ((tmp = strstr(buffer, "\n")))
					strlcpy(result, tmp+1, sizeof result);
				else
					strlcpy(result, buffer, sizeof result);

				free(buffer);
			}
			unlink("/tmp/output.txt");
#elif defined(RTCONFIG_LANTIQ)
			char *buffer = read_whole_file("/rom/opt/lantiq/etc/wave_components.ver");

			if (buffer) {
				tmp = strstr(buffer, "wave_release_minor=");
				if (tmp)
					sscanf(tmp, "wave_release_minor=%s", result);
				else
					strcpy(result,"Unknow");

				free(buffer);
			}
			unlink("/rom/opt/lantiq/etc/wave_components.ver");
#elif defined(RTCONFIG_QCA)
			char *buffer = read_whole_file("/proc/athversion");

			if (buffer) {
				strlcpy(result, buffer, sizeof(result));
				free(buffer);
			}
#elif defined(RTCONFIG_RALINK)
			char buffer[16] = {0};
			if(get_mtk_wifi_driver_version(buffer, sizeof(buffer))>0){
				if(*buffer)
					strcpy(result,buffer);
			} else
				strcpy(result,"Unknow");
#endif
#ifdef RTCONFIG_QTN
                } else if(strcmp(type,"qtn_version") == 0 ) {

			if (!get_qtn_version(result, sizeof(result)))
				strcpy(result,"<unknown>");
#endif
#if defined(RTCONFIG_BCMARM) || defined(RTCONFIG_HND_ROUTER)
		} else if(strcmp(type,"cfe_version") == 0 ) {
#if defined(RTCONFIG_CFEZ)
			snprintf(result, sizeof result, "%s", nvram_get("bl_version"));
#else
			system("cat /dev/mtd0ro | grep bl_version >/tmp/output.txt");
			char *buffer = read_whole_file("/tmp/output.txt");

			strcpy(result,"Unknown");	// Default
			if (buffer) {
				tmp = strstr(buffer, "bl_version=");

				if (tmp) {
					sscanf(tmp, "bl_version=%s", result);
				} else {
					snprintf(result, sizeof result, "%s", nvram_get("bl_version"));
				}
				free(buffer);
			}
			unlink("/tmp/output.txt");
#endif
#endif
		} else if(strncmp(type,"pid",3) ==0 ) {
			char service[32];
			sscanf(type, "pid.%31s", service);

			if (*service)
				sprintf(result, "%d", pidof(service));

		} else if(strncmp(type, "vpnip",5) == 0 ) {
			int instance = 1;
			int fd;
			struct ifreq ifr;
			char buf[18];

			strcpy(result, "0.0.0.0");

			fd = socket(AF_INET, SOCK_DGRAM, 0);
			if (fd) {
				ifr.ifr_addr.sa_family = AF_INET;
				sscanf(type,"vpnip.%d", &instance);
				snprintf(ifr.ifr_name, IFNAMSIZ - 1, "tun1%d", instance);
				if (ioctl(fd, SIOCGIFADDR, &ifr) == 0) {
					strlcpy(result, inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr), sizeof result);

					snprintf(buf, sizeof buf, "vpn_client%d_rip", instance);
					if (!strlen(nvram_safe_get(buf))) {
						sprintf(buf, "%d", instance);
						eval("/usr/sbin/gettunnelip.sh", buf);
					}
				}
				close(fd);
			}

		} else if(strncmp(type,"vpnstatus",9) == 0 ) {
			int num = 0;
			char service[10], buf[256];

			sscanf(type,"vpnstatus.%9[^.].%d", service, &num);

			if ( (*service) && (num > 0) )
			{
				snprintf(buf, sizeof(buf), "vpn%s%d", service, num);
				if (pidof(buf) > 0) {

					// Read the status file and repeat it verbatim to the caller
					sprintf(buf,"/etc/openvpn/%s%d/status", service, num);

					// Give it some time if it doesn't exist yet
					if (!check_if_file_exist(buf))
					{
						sleep(5);
					}

					char *buffer = read_whole_file(buf);
					if (buffer)
					{
						replace_char(buffer, '\n', '>');
						strlcpy(result, buffer, sizeof(result));
						free(buffer);
					}
				}
			}
		} else if(strcmp(type,"ethernet.rtk") == 0 ) {
#ifdef RTCONFIG_EXT_RTL8365MB
			int states[4];

			states[0] = states[1] = states[2] = states[3] = 0;

			GetPhyStatus_rtk((int *)&states);

			snprintf(result, sizeof result, "[[\"%d\", \"%d\"],"
			                                " [\"%d\", \"%d\"],"
			                                " [\"%d\", \"%d\"],"
			                                " [\"%d\", \"%d\"]]",
			                                 5, states[0],
			                                 6, states[1],
			                                 7, states[2],
			                                 8, states[3]);
#else
			strcpy(result, "[]");
#endif
		} else if(strcmp(type,"ethernet") == 0 ) {
#if defined(RTCONFIG_BCMARM) || defined(RTCONFIG_HND_ROUTER)
#ifndef HND_ROUTER
			int len, j;

			system("/usr/sbin/robocfg showports >/tmp/output.txt");

			char *buffer = read_whole_file("/tmp/output.txt");
			if (buffer) {
				len = strlen(buffer);

				for (j=0; (j < len); j++) {
					if (buffer[j] == '\n') buffer[j] = '>';
				}
#ifdef RTCONFIG_QTN
				j = GetPhyStatus_qtn();
				snprintf(result, sizeof result, (j > 0 ? "%sPort 10: %dFD enabled stp: none vlan: 1 jumbo: off mac: 00:00:00:00:00:00>" :
							 "%sPort 10: DOWN enabled stp: none vlan: 1 jumbo: off mac: 00:00:00:00:00:00>"),
							  buffer, j);
#else
                                strlcpy(result, buffer, sizeof result);
#endif
                                free(buffer);

			}
			unlink("/tmp/output.txt");
#else // HND lacks robocfg support
			strcpy(result, "[]");
#endif
#elif defined(RTCONFIG_LANTIQ) || defined(RTCONFIG_QCA) || defined(RTCONFIG_RALINK)
			strcpy(result,"[]");
#endif
#if defined(RTCONFIG_BCMARM) || defined(RTCONFIG_HND_ROUTER)
		} else if(strlen(type) > 8 && strncmp(type,"hwaccel", 7) == 0 ) {
			if (!strcmp(&type[8], "runner"))	// Also query Archer on 675x
#if defined(RTAC86U) || defined(GTAC2900) || defined(R8000P)
				system("cat /proc/modules | grep -m 1 -c pktrunner | sed -e \"s/0/Disabled/\" -e \"s/1/Enabled/\" >/tmp/output.txt");
#else
				system("/bin/fc status | grep \"HW Acceleration\" >/tmp/output.txt");
#endif
			else if (!strcmp(&type[8], "fc"))
#if defined(RTCONFIG_HND_ROUTER_AX_6756)
				system("/bin/fc status | grep \"Flow Ucast Learning\" >/tmp/output.txt");
#else
				system("/bin/fc status | grep \"Flow Learning\" >/tmp/output.txt");
#endif
			char *buffer = read_whole_file("/tmp/output.txt");
			if (buffer) {
				if (strstr(buffer, "Enabled"))
					strcpy(result,"Enabled");
				else if (strstr(buffer, "Disabled"))
					strcpy(result, "Disabled");
				else
					strcpy(result, "&lt;unknown&gt;");
				free(buffer);
			} else {
				strcpy(result, "&lt;unknown&gt;");
			}
			unlink("/tmp/output.txt");
#endif
		} else {
			strcpy(result,"Not implemented");
		}
	}

	retval += websWrite(wp, result);
	return retval;
}

#ifdef RTCONFIG_QTN
unsigned int get_qtn_temperature(void)
{
        int temp_external, temp_internal, temp_bb;
	if (!rpc_qtn_ready())
		return 0;

        if (qcsapi_get_temperature_info(&temp_external, &temp_internal, &temp_bb) >= 0)
		return temp_internal / 1000000.0f;

	return 0;
}

int GetPhyStatus_qtn(void)
{
	int ret;

	if (!rpc_qtn_ready()) {
		return -1;
	}
	ret = qcsapi_wifi_run_script("set_test_mode", "get_eth_1000m");
	if (ret < 0) {
		ret = qcsapi_wifi_run_script("set_test_mode", "get_eth_100m");
		if (ret < 0) {
			ret = qcsapi_wifi_run_script("set_test_mode", "get_eth_10m");
			if (ret < 0) {
				// fprintf(stderr, "ATE command error\n");
				return 0;
			}else{
				return 10;
			}
		}else{
			return 100;
		}
		return -1;
	}else{
		return 1000;
	}
	return 0;
}

unsigned int get_qtn_version(char *version, int len)
{
        if (!rpc_qtn_ready())
                return 0;

        if (qcsapi_firmware_get_version(version, len) >= 0)
                return 1;

        return 0;
}
#endif

unsigned int get_phy_temperature(int radio)
{
#if defined(RTCONFIG_BCMARM) || defined(RTCONFIG_HND_ROUTER)
	int ret = 0;
	unsigned int *temp;
	char buf[WLC_IOCTL_SMLEN];
	char *interface;

	strcpy(buf, "phy_tempsense");

	if (radio == 0) {
		interface = nvram_safe_get("wl0_ifname");
	} else if (radio == 1) {
		interface = nvram_safe_get("wl1_ifname");
#if defined(RTCONFIG_HAS_5G_2) || defined(RTCONFIG_WIFI6E)
	} else if (radio == 2) {
		interface = nvram_safe_get("wl2_ifname");
#endif
#if defined(GTAXE16000)
	} else if (radio == 3) {
		interface = nvram_safe_get("wl3_ifname");
#endif
	} else {
		return 0;
	}

	if ((ret = wl_ioctl(interface, WLC_GET_VAR, buf, sizeof(buf)))) {
		return 0;
	} else {
		temp = (unsigned int *)buf;
		return *temp / 2 + 20;
	}
#elif defined(RTCONFIG_LANTIQ)
	int temp = 0, retval = 0;
	FILE *fp;

	if (radio == 0 || radio == 1) {
		char buffer[99];
		char iw[]="iwpriv wlan0 gTemperature";
		char s[]="wlan0     gTemperature:%%d %%*[0-9 ]";
		if (radio == 0) {
			snprintf(iw, sizeof(iw), "iwpriv wlan0 gTemperature");
			snprintf(s, sizeof(s), "wlan0     gTemperature:%%d %%*[0-9 ]");
		} else if (radio == 1) {
			snprintf(iw, sizeof(iw), "iwpriv wlan2 gTemperature");
			snprintf(s, sizeof(s), "wlan2     gTemperature:%%d %%*[0-9 ]");
		}
		if ((fp = popen(iw, "r")) != NULL) {
			if(fgets(buffer, 99, fp) != NULL) {
				sscanf(buffer, s, &temp);
			}
			pclose(fp);
			retval = temp;
		}
	}
	return retval;
#elif defined(RTCONFIG_QCA)
	char thermal_path[64];
	char value[16];
	char *wifi_if = NULL;
	int len, band;

    switch(radio){
        case 0:
            band = 0;
            break;
        case 1:
            band = 1;
            break;
#if defined(RTCONFIG_HAS_5G_2) || defined(RTCONFIG_WIFI6E)
        case 2:
            band = 2;
            break;
#endif
        default:
			return 0;
    }

	if((wifi_if = get_vphyifname(band)) == NULL)
		return 0;

	snprintf(thermal_path, sizeof(thermal_path), "/sys/class/net/%s/thermal/temp", wifi_if);
	if((len = f_read_string(thermal_path, value, sizeof(value))) <= 0)
		return 0;

	return atoi(value);
#elif defined(RTCONFIG_RALINK)
	struct iwreq wrq;
	char temp[18];
	char *interface = NULL;

	if (radio == 0)
		interface = nvram_get("wl0_ifname");
	else if (radio == 1)
		interface = nvram_get("wl1_ifname");
#if defined(RTCONFIG_HAS_5G_2) || defined(RTCONFIG_WIFI6E)
	else if (radio == 2)
		interface = nvram_get("wl2_ifname");
#endif

	if(interface == NULL || *interface == 0)
		return 0;
	memset(temp, 0, 18);
	memset(&wrq, 0, sizeof(wrq));
	wrq.u.data.pointer = &temp;
	wrq.u.data.length  = 18;
	wrq.u.data.flags   = ASUS_SUBCMD_RADIO_TEMPERATURE;
	if (wl_ioctl(interface, RTPRIV_IOCTL_ASUSCMD, &wrq) < 0)
		return 0;
	else {
		unsigned int *i;
		i=(unsigned int *)temp;
		return *i;
	}
#endif
}

#if defined(RTCONFIG_BCMARM) || defined(RTCONFIG_HND_ROUTER)
unsigned int get_wifi_clients(int unit, int querytype)
{
	char *name, prefix[8];
	struct maclist *clientlist;
	int max_sta_count, maclist_size;
	int val, count = 0, subunit;
#ifdef RTCONFIG_QTN
	qcsapi_unsigned_int association_count = 0;
#endif
#ifdef RTCONFIG_WIRELESSREPEATER
	int isrepeater = 0;
#endif

	/* buffers and length */
	max_sta_count = 128;
	maclist_size = sizeof(clientlist->count) + max_sta_count * sizeof(struct ether_addr);
	clientlist = malloc(maclist_size);

	if (!clientlist)
		return 0;

	for (subunit = 0; subunit < 4; subunit++) {
#ifdef RTCONFIG_WIRELESSREPEATER
		if ((nvram_get_int("sw_mode") == SW_MODE_REPEATER) && (unit == nvram_get_int("wlc_band"))) {
			if (subunit == 0)
				continue;
			else if (subunit == 1)
				isrepeater = 1;
			else
				break;
		}
#endif

		if (subunit == 0)
			snprintf(prefix, sizeof(prefix), "wl%d_", unit);
		else
			snprintf(prefix, sizeof(prefix), "wl%d.%d_", unit, subunit);

		name = nvram_pf_safe_get(prefix, "ifname");
		if (*name == '\0') continue;

#ifdef RTCONFIG_QTN
		if (unit == 1) {
			if ((nvram_match("wl1_radio", "0")) || (!rpc_qtn_ready()))
				count = -1;
			else if ((querytype == SI_WL_QUERY_ASSOC) &&
				 (qcsapi_wifi_get_count_associations(name, &association_count) >= 0))
					count = association_count;
			else	// All other queries aren't support by QTN
				count = -1;

			goto exit;
		}
#endif

		if (subunit == 0) {
			wl_ioctl(name, WLC_GET_RADIO, &val, sizeof(val));
			if (val == 1) {
				count = -1;	// Radio is disabled
				goto exit;
			}
		}

		if ((subunit > 0) &&
#ifdef RTCONFIG_WIRELESSREPEATER
			!isrepeater &&
#endif
			!nvram_pf_get_int(prefix, "bss_enabled"))
				continue;	// Guest interface disabled

		switch (querytype) {
			case SI_WL_QUERY_AUTHE:
				strcpy((char*)clientlist, "authe_sta_list");
				if (!wl_ioctl(name, WLC_GET_VAR, clientlist, maclist_size))
					count += clientlist->count;
				break;
			case SI_WL_QUERY_AUTHO:
				strcpy((char*)clientlist, "autho_sta_list");
				if (!wl_ioctl(name, WLC_GET_VAR, clientlist, maclist_size))
					count += clientlist->count;
				break;
			case SI_WL_QUERY_ASSOC:
				clientlist->count = max_sta_count;
				if (!wl_ioctl(name, WLC_GET_ASSOCLIST, clientlist, maclist_size))
					count += clientlist->count;
				break;
		}
	}

exit:
	free(clientlist);
	return count;
}


#ifdef RTCONFIG_EXT_RTL8365MB
void GetPhyStatus_rtk(int *states)
{
	int model;
	const int *o;
	int fd = open(RTKSWITCH_DEV, O_RDONLY);

	if (fd < 0) {
		perror(RTKSWITCH_DEV);
		return;
	}

	phyState pS;

	pS.link[0] = pS.link[1] = pS.link[2] = pS.link[3] = 0;
	pS.speed[0] = pS.speed[1] = pS.speed[2] = pS.speed[3] = 0;

        switch(model = get_model()) {
        case MODEL_RTAC5300:
		{
		/* RTK_LAN  BRCM_LAN  WAN  POWER */
		/* R0 R1 R2 R3 B4 B0 B1 B2 B3 */
		/* L8 L7 L6 L5 L4 L3 L2 L1 W0 */

		const int porder[4] = {3,2,1,0};
		o = porder;

		break;
		}
        case MODEL_RTAC88U:
		{
		/* RTK_LAN  BRCM_LAN  WAN  POWER */
		/* R3 R2 R1 R0 B3 B2 B1 B0 B4 */
		/* L8 L7 L6 L5 L4 L3 L2 L1 W0 */

		const int porder[4] = {0,1,2,3};
		o = porder;

		break;
		}
	default:
		{
		const int porder[4] = {0,1,2,3};
		o = porder;

		break;
		}
	}


	if (ioctl(fd, GET_RTK_PHYSTATES, &pS) < 0) {
		perror("rtkswitch ioctl");
		close(fd);
		return;
	}

	close(fd);

	states[0] = (pS.link[o[0]] == 1) ? (pS.speed[o[0]] == 2) ? 1000 : 100 : 0;
	states[1] = (pS.link[o[1]] == 1) ? (pS.speed[o[1]] == 2) ? 1000 : 100 : 0;
	states[2] = (pS.link[o[2]] == 1) ? (pS.speed[o[2]] == 2) ? 1000 : 100 : 0;
	states[3] = (pS.link[o[3]] == 1) ? (pS.speed[o[3]] == 2) ? 1000 : 100 : 0;
}
#endif
#endif

