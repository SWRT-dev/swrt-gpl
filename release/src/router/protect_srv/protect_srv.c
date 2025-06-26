 /*
 * Copyright 2017, ASUSTeK Inc.
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND ASUS GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 */
 
#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <syslog.h>
#include <limits.h>
/*--*/
#include <libptcsrv.h>
#include "nvram.cc"

#ifdef ASUSWRT_SDK /* ASUSWRT SDK */
#else /* DSL_ASUSWRT SDK */
#endif

#ifdef RTCONFIG_NOTIFICATION_CENTER
#include <libnt.h>
#endif

#define MyDBG(fmt,args...) \
	if(isFileExist(PROTECT_SRV_DEBUG) > 0) { \
		syslog(LOG_ERR, "[ProtectionSrv][%s:(%d)]"fmt, __FUNCTION__, __LINE__, ##args); \
	}
#define ErrorMsg(fmt,args...) \
	syslog(LOG_ERR, "[ProtectionSrv][%s:(%d)]"fmt, __FUNCTION__, __LINE__, ##args);

enum {
	F_OFF=0,
	F_ON
};

typedef struct __ptcsrv_lock_data_t_
{
	PTCSRV_STAT_DATA_T stat;
	int maxcnt;
	long cnt;
	PTCSRV_LOGIN_RECORD_T *record;
	char *log_path;
#ifdef RTCONFIG_NOTIFICATION_CENTER
	int event;
#endif
	int field_2C;
} PTCSRV_LOCK_DATA_T;

static int RECV_SIG_FLAG = 0;
static int terminated = 1;

PTCSRV_LOGIN_RECORD_T *wan_list = NULL;
PTCSRV_LOGIN_RECORD_T *ssh_list = NULL;
PTCSRV_LOGIN_RECORD_T *telnet_list = NULL;

PTCSRV_LOCK_DATA_T ssh_lock_t = {0};
PTCSRV_LOCK_DATA_T telnet_lock_t = {0};
PTCSRV_LOCK_DATA_T wan_lock_t = {0};

int time_list[] = {0, 0, 0, 300, 600, 1800, 3600};

#ifdef RTCONFIG_NOTIFICATION_CENTER
static void SEND_FAIL_LOGIN_EVENT(int e, char *ip)
{
		char msg[100];
		snprintf(msg, sizeof(msg), "{\"IP\":\"%s\",\"msg\":\"\"}", ip);
		SEND_NT_EVENT(e, msg);
}
#endif

static uint32_t parseIpaddrstr(char const *ipAddress)
{
	unsigned int ip[4];
	
	if ( 4 != sscanf(ipAddress, "%u.%u.%u.%u", &ip[0], &ip[1], &ip[2], &ip[3]) ) {
		return 0;
	}
	
	return ip[3] + ip[2] * 0x100 + ip[1] * 0x10000ul + ip[0] * 0x1000000ul;
}
static int IsSameSubnet(uint32_t ip, uint32_t netIp, uint32_t netMask)
{
	if ((netIp & netMask) == (ip & netMask)) {
		/* on same subnet */
		return 1;
	}
	return 0;
	
}
static int IsLanSide(uint32_t ip)
{
	char lan_ip[64], lan_netmask[64];
//	uint8_t p1, p2, p3, p4;
	
//	p1 = (uint8_t) (ip >> 24);
//	p2 = (uint8_t)((ip >> 16) & 0x0ff);
//	p3 = (uint8_t)((ip >> 8 ) & 0x0ff);
//	p4 = (uint8_t) (ip & 0x0ff);
	
	
	if (GetLanIpaddr(lan_ip, sizeof(lan_ip)) && GetLanNetmask(lan_netmask, sizeof(lan_netmask))) {
		if (IsSameSubnet(ip, parseIpaddrstr(lan_ip), parseIpaddrstr(lan_netmask))) {
			MyDBG("From LAN side.\n");
			return 1;
		}
	}
	
	MyDBG("From WAN side.\n");
	return 0;
}

static int add_lock_rule(char *wanType, int serType, char *ipaddr)
{
	char chain[32], command[128];
	
	snprintf(chain, sizeof(chain), "%s%s", PROTECT_SRV_RULE_CHAIN, wanType);
	if(serType == PROTECTION_SERVICE_TELNET){
		snprintf(command, sizeof(command), "iptables -A %s -p tcp --dport %d -s %s -j DROP", chain, 23, ipaddr);
		system(command);
		MyDBG("Add %s rules of %s into %s chain.\n", "TELNET", ipaddr, chain);
	}else{
		snprintf(command, sizeof(command), "iptables -A %s -p tcp --dport %d -s %s -j DROP", chain, GetSSHport(), ipaddr);
		system(command);
		MyDBG("Add %s rules of %s into %s chain.\n", "SSH", ipaddr, chain);
	}
	return 0;
}

static int add_lockall_rule(char *wanType, int serType)
{
	char chain[32], command[128];
	
	snprintf(chain, sizeof(chain), "%s%s", PROTECT_SRV_RULE_CHAIN, wanType);
	if(serType == PROTECTION_SERVICE_TELNET){
		snprintf(command, sizeof(command), "iptables -I %s -p tcp --dport %d -j DROP", chain, 23);
		system(command);
		MyDBG("Insert locking %s rules into %s chain.\n", "TELNET", chain);
	}else{
		snprintf(command, sizeof(command), "iptables -I %s -p tcp --dport %d -j DROP", chain, GetSSHport());
		system(command);
		MyDBG("Insert locking %s rules into %s chain.\n", "SSH", chain);
	}
	return 0;
}

PTCSRV_LOCK_DATA_T* _load_record_info(const char *wanType, int serType)
{
	if(!strcmp(wanType, "WAN")){
		if(serType == PROTECTION_SERVICE_SSH)
			return &wan_lock_t;
		MyDBG("Service not support at WAN\n");
		return NULL;
	}else if(!strcmp(wanType, "LAN")){
		if(serType == PROTECTION_SERVICE_SSH)
			return &ssh_lock_t;
		else if(serType == PROTECTION_SERVICE_TELNET)
			return &telnet_lock_t;
		MyDBG("Service not support at LAN\n");
		return NULL;
	}
	MyDBG("Not support\n");
	return NULL;
}

static int insert_lock_rule(char *wanType, int serType)
{
	int i, j, n, ret = 0;
	PTCSRV_LOCK_DATA_T *plock;
	PTCSRV_LOGIN_RECORD_T *record;
	struct sysinfo info;

	plock = _load_record_info(wanType, serType);
	if(plock){
		sysinfo(&info);
		plock->stat.lock_rule_cnt = 0;
		if(plock->cnt > 0){
			record = plock->record;
			for(i = 0, j = 0; i < plock->cnt; i++, j++){
				if(record[j].addr[0]){
					n = record[j].fail_cnt;
					if(n > 6)
						n = 6;
					if(record[j].fail_cnt){
						if(time_list[n] > 0 && (info.uptime - record[j].lock_time) < time_list[n]){
							add_lock_rule(wanType, serType, record[j].addr);
							plock->stat.lock_rule_cnt++;
							if(!record[j].lock_time)
								record[j].lock_time = info.uptime;
							if(!record[j].locked)
								record[j].locked = 1;
						}
					}
				}
			}
			ret = plock->stat.lock_rule_cnt;
		}
		if(plock->stat.lock_rule_peak < ret)
			plock->stat.lock_rule_peak = ret;
		if(plock->maxcnt < ret)
			return add_lockall_rule(wanType, serType);
	}
	return ret;
}

static int del_lock_rule(char *wanType, int serType, char *ipaddr)
{
	char chain[32], command[128];
	
	snprintf(chain, sizeof(chain), "%s%s", PROTECT_SRV_RULE_CHAIN, wanType);
	if(serType == PROTECTION_SERVICE_TELNET){
		snprintf(command, sizeof(command), "iptables -D %s -p tcp --dport %d -s %s -j DROP", chain, 23, ipaddr);
		system(command);
		MyDBG("Remove locking %s rules of %s from %s chain.\n", "TELNET", ipaddr, chain);
	}else{
		snprintf(command, sizeof(command), "iptables -D %s -p tcp --dport %d -s %s -j DROP", chain, GetSSHport(), ipaddr);
		system(command);
		MyDBG("Remove locking %s rules of %s from %s chain.\n", "SSH", ipaddr, chain);
	}
	return 0;
}

static int del_lockall_rule(char *wanType, int serType)
{
	char chain[32], command[128];
	
	snprintf(chain, sizeof(chain), "%s%s", PROTECT_SRV_RULE_CHAIN, wanType);
	if(serType == PROTECTION_SERVICE_TELNET){
		snprintf(command, sizeof(command), "iptables -D %s -p tcp --dport %d -j DROP", chain, 23);
		system(command);
		MyDBG("Remove locking %s rules from %s chain.\n", "TELNET", chain);
	}else{
		snprintf(command, sizeof(command), "iptables -D %s -p tcp --dport %d -j DROP", chain, GetSSHport());
		system(command);
		MyDBG("Remove locking %s rules from %s chain.\n", "SSH", chain);
	}
	return 0;
}

static void update_lock_rule(char *wanType, int serType)
{
	int i, j __attribute__ ((unused)), n, ret __attribute__ ((unused)) = 0;
	PTCSRV_LOCK_DATA_T *plock;
	PTCSRV_LOGIN_RECORD_T *record;
	struct sysinfo info;
	plock = _load_record_info(wanType, serType);
	if(plock->cnt > 0){
		sysinfo(&info);
		record = plock->record;
		for(i = 0; i < plock->cnt; i++, plock->stat.record_cnt--){
			if(record[i].addr[0]){
				n = record[i].fail_cnt;
				if(n > 6)
					n = 6;
				if(record[i].fail_cnt){
					if(time_list[n] > 0 && record[i].lock_time && record[i].locked != 0){
						if((info.uptime - record[i].lock_time) > time_list[n] && record[i].locked == 1){
							if(plock->stat.lock_rule_cnt == plock->maxcnt)
								del_lockall_rule(wanType, serType);
							add_lock_rule(wanType, serType, record[i].addr);
							plock->stat.lock_rule_cnt--;
							record[i].locked = 0;
						}
					}else{
						add_lock_rule(wanType, serType, record[i].addr);
						record[i].lock_time = info.uptime;
						record[i].locked = 1;
						plock->stat.lock_rule_cnt++;
						if(plock->stat.lock_rule_cnt > plock->stat.lock_rule_peak)
							plock->stat.lock_rule_peak = plock->stat.lock_rule_cnt;
						if(plock->stat.lock_rule_cnt > plock->maxcnt)
							add_lockall_rule(wanType, serType);
					}
				}else{
					if(record[i].locked){
						del_lock_rule(wanType, serType, record[i].addr);
						plock->stat.lock_rule_cnt--;
						if(plock->stat.lock_rule_cnt == plock->maxcnt)
							del_lockall_rule(wanType, serType);
					}else if(plock->stat.lock_rule_cnt == plock->maxcnt)
						del_lockall_rule(wanType, serType);
					record[i].addr[0] = 0;
					record[i].locked = 0;
					record[i].fail_cnt = 0;
					record[i].fail_time = 0;
					record[i].lock_time = 0;
				}
			}
		}
	}
}
static void check_wanlan_lock()
{
	if(RECV_SIG_FLAG){
		RECV_SIG_FLAG = F_OFF;
		insert_lock_rule("WAN", PROTECTION_SERVICE_SSH);
		insert_lock_rule("LAN", PROTECTION_SERVICE_SSH);
		insert_lock_rule("LAN", PROTECTION_SERVICE_TELNET);
		sleep(1);
		if(!terminated)
			return;
	}
	update_lock_rule("WAN", PROTECTION_SERVICE_SSH);
	update_lock_rule("LAN", PROTECTION_SERVICE_SSH);
	update_lock_rule("LAN", PROTECTION_SERVICE_TELNET);
}

static void _add_record(char *wanType, PTCSRV_STATE_REPORT_T *report)
{
	int i, j, status = 0;
	long fail_max, n = 0;
	char cmd[256];
	PTCSRV_LOCK_DATA_T *plock;
	PTCSRV_LOGIN_RECORD_T *record;
	struct sysinfo info;
	plock = _load_record_info(wanType, report->s_type);
	if(!plock)
		return;
	sysinfo(&info);
	if(plock->cnt <= 0){
		if(report->status)
			return;
		MyDBG("FULL. Ignore this case: %s\n", report->addr);
		return;
	}
	record = plock->record;
	for(i = 0; i < plock->cnt; i++){
		if(strcmp(record[i].addr, report->addr))
			continue;
		status = report->status;
		if(status){
			if(status != RPT_SUCCESS){
				status = 1;
				continue;
			}
			snprintf(cmd, sizeof(cmd), "logger -t %s [%s] login succeeded from %s after %zd attempts",
				PROTECT_SRV_RULE_CHAIN, report->s_type ? "TELNET" : "SSH", record[i].addr, record[i].fail_cnt);
			system(cmd);
			record[i].fail_cnt = 0;
			record[i].fail_time = 0;
		}else{
			record[i].fail_cnt++;
			record[i].fail_time = info.uptime;
			j = record[i].fail_cnt;
			if(j > 6)
				j = 6;
			if(time_list[j] && record[i].locked == 0){
				record[i].lock_it = 1;
#ifdef RTCONFIG_NOTIFICATION_CENTER
				snprintf(cmd, sizeof(cmd), "{\"IP\":\"%s\",\"msg\":\"\"}", report->addr);
				SEND_NT_EVENT(plock->event, cmd);
#endif
			}
			status = 1;
		}
	}
	if(!status){
		i = report->status;
		if(i == 0){
			if(plock->cnt <= 0){
				MyDBG("FULL. Ignore this case: %s\n", report->addr);
				return;
			}
			if(record[0].addr[0]){
				fail_max = LONG_MAX;
				for(i = 0, j = -1; i < plock->cnt; i++){
					if(record[i].locked == 0){
						n = i;
						if(n >= fail_max)
							n = fail_max;
						if(record[i].fail_time < fail_max)
							fail_max = record[i].fail_time;
					}
					if(!record[i+1].addr[0]){
						strlcpy(record[i+1].addr, report->addr, sizeof(record[0].addr));
						plock->stat.record_cnt++;
						record[i+1].fail_cnt = 1;
						record[i+1].fail_time = info.uptime;
						return;
					}
				}
				if(n == -1){
					MyDBG("FULL. Ignore this case: %s\n", report->addr);
				}else{
					strlcpy(record[n].addr, report->addr, sizeof(record[0].addr));
					record[n].fail_cnt = 1;
					record[n].fail_time = info.uptime;
				}
			}else{
				strlcpy(record[0].addr, report->addr, sizeof(record[0].addr));
				record[0].fail_cnt = 1;
				record[0].fail_time = info.uptime;
			}
		}
	}
}

void receive_s(int newsockfd)
{
	PTCSRV_SOCK_DATA_T ptcsrv;
	PTCSRV_LOCK_DATA_T *pt = NULL;
	PTCSRV_LOGIN_RECORD_T *record;
	struct sysinfo info;
	int i = 0, n, ret = 0;

	memset(&ptcsrv, 0, sizeof(PTCSRV_SOCK_DATA_T));
	n = read(newsockfd, &ptcsrv, sizeof(PTCSRV_SOCK_DATA_T));
	if(n < 0){
		MyDBG("ERROR reading from socket.\n");
		return;
	}

	switch(ptcsrv.d_type){
		case PTCSRV_S_RPT:
			MyDBG("[receive report] addr:[%s] s_type:[%d] status:[%d] msg:[%s]\n", ptcsrv.report.addr, ptcsrv.report.s_type, ptcsrv.report.status, ptcsrv.report.msg);
			
			if (GetDebugValue(PROTECT_SRV_DEBUG)) {
				char info[440];
				snprintf(info, sizeof(info), "echo \"[ProtectionSrv][receive report] addr:[%s] s_type:[%d] status:[%d] msg:[%s]\" >> %s",
				ptcsrv.report.addr, ptcsrv.report.s_type, ptcsrv.report.status, ptcsrv.report.msg, PROTECT_SRV_LOG_FILE);
				system(info);
			}
			if(IsLanSide(parseIpaddrstr(ptcsrv.report.addr)))
				_add_record("LAN", &ptcsrv.report);
			else
				_add_record("WAN", &ptcsrv.report);
			return;
		case PTCSRV_G_WAN_STAT:
		case PTCSRV_G_LAN_STAT:
		case PTCSRV_G_WAN_RECORD:
		case PTCSRV_G_LAN_RECORD:
			//it only works with asuswrt
			return;
		case PTCSRV_G_LOCK:
			if(ptcsrv.report.s_type == PROTECTION_SERVICE_SSH){
				if(IsLanSide(parseIpaddrstr(ptcsrv.report.addr)))
					pt = &ssh_lock_t;
				else
					pt = &wan_lock_t;
			}else if(ptcsrv.report.s_type == PROTECTION_SERVICE_TELNET){
				if(IsLanSide(parseIpaddrstr(ptcsrv.report.addr)))
					pt = &telnet_lock_t;
				else
					return;
			}else
				return;
			sysinfo(&info);
			if(pt->stat.lock_rule_cnt < pt->maxcnt){
				if(pt->cnt <= 0){
					n = write(newsockfd, &ret, sizeof(int));
					if(n < 0){
						MyDBG("ERROR write.\n");
						return;
					}
				}else{
					record = pt->record;
					for(i = 0; i < pt->cnt; i++){
						if(record[i].addr[0] && !strncmp(record[i].addr, ptcsrv.ilk.addr, 15)){
							if(record[i].locked)
								break;
							if(record[i].lock_it == 1){
								if(record[i].lock_time == 0)
									break;
								n = record[i].fail_cnt;
								if(n > 6)
									n = 6;
								if(info.uptime - record[i].lock_time > time_list[n])
									break;
							}
						}
					}
				}
			}
			if(i != pt->cnt)
				ret = 1;
			n = write(newsockfd, &ret, sizeof(int));
			if(n < 0){
				MyDBG("ERROR write.\n");
			}
			return;
		default:
			MyDBG("Unknown message.\n");
			return;
	}
}

static void start_local_socket(void)
{
	struct sockaddr_un addr;
	int sockfd, newsockfd;
	
	if ( (sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
		MyDBG("socket error\n");
		perror("socket error");
		exit(-1);
	}
	
	memset(&addr, 0, sizeof(addr));
	addr.sun_family = AF_UNIX;
	strlcpy(addr.sun_path, PROTECT_SRV_SOCKET_PATH, sizeof(addr.sun_path));
	
	unlink(PROTECT_SRV_SOCKET_PATH);
	
	if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
		MyDBG("socket bind error\n");
		perror("socket bind error");
		exit(-1);
	}
	
	if (listen(sockfd, 3) == -1) {
		MyDBG("listen error\n");
		perror("listen error");
		exit(-1);
	}
	
	while (1) {
		if ( (newsockfd = accept(sockfd, NULL, NULL)) == -1) {
			MyDBG("accept error\n");
			perror("accept error");
			continue;
		}
		
		receive_s(newsockfd);
		close(newsockfd);
	}
}

static void local_socket_thread(void)
{
	pthread_t thread;
	pthread_attr_t attr;
	
	MyDBG("Start local socket thread.\n");
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	pthread_create(&thread,NULL,(void *)&start_local_socket,NULL);
	pthread_attr_destroy(&attr);
}

static void dump_record_to_file(PTCSRV_LOCK_DATA_T *plock)
{
	int record_cnt = 0, rule_cnt = 0, i;
	size_t fail_cnt;
	FILE *fp;
	PTCSRV_LOGIN_RECORD_T *record;
	struct sysinfo info __attribute__ ((unused));
	sysinfo(&info);
	fp = fopen(plock->log_path, "w");
	if(fp){
		fprintf(fp, "        address     retry  since_last_try   block_time   block_status\n");
		if(plock->cnt > 0){
			record = plock->record;
			for(i = 0; i < plock->cnt; i++, rule_cnt++){
				if(record[i].addr[0]){
					fail_cnt = record[i].fail_cnt;
					record_cnt++;
					if(fail_cnt > 6)
						fail_cnt = 6;
					fprintf(fp, "%15s%10zu%16ld%13d%15d\n", record[i].addr, record[i].fail_cnt,
						info.uptime - record[i].fail_time, time_list[fail_cnt], record[i].locked);
					if(record[i].locked)
						break;
				}
			}
		}
		if(plock->stat.lock_rule_cnt != rule_cnt)
			MyDBG("BUG !! wrong iptables rule count, %d != %d\n", rule_cnt, plock->stat.lock_rule_cnt);
		if(plock->stat.record_cnt != record_cnt)
			MyDBG("BUG !! wrong record count, %d != %d\n", record_cnt, plock->stat.record_cnt);
		fclose(fp);
	}
}

void dump_all_record(int signum)
{
	if(signum == SIGUSR1){
		RECV_SIG_FLAG = F_ON;
	}else if (signum == SIGUSR2){
		MyDBG("WAN SSH list count: %d\niptables rule count: %d\niptables rule peak: %d\nPlease refer to %s\n"
		"--------------------------\n"
		"LAN SSH list count: %d\niptables rule count: %d\niptables rule peak: %d\nPlease refer to %s\n"
		"--------------------------\n"
		"LAN TELNET list count: %d\niptables rule count: %d\niptables rule peak: %d\nPlease refer to %s\n",
		wan_lock_t.stat.record_cnt, wan_lock_t.stat.lock_rule_cnt, wan_lock_t.stat.lock_rule_peak,
		wan_lock_t.log_path, ssh_lock_t.stat.record_cnt, ssh_lock_t.stat.lock_rule_cnt,
		ssh_lock_t.stat.lock_rule_peak, ssh_lock_t.log_path, telnet_lock_t.stat.record_cnt,
		telnet_lock_t.stat.lock_rule_cnt, telnet_lock_t.stat.lock_rule_peak, telnet_lock_t.log_path);
		dump_record_to_file(&wan_lock_t);
		dump_record_to_file(&ssh_lock_t);
		dump_record_to_file(&telnet_lock_t);
	}else if (signum == SIGTERM){
		terminated = 0;
	}else
		MyDBG("Unknown SIGNAL\n");
}

static void signal_register(void)
{
	struct sigaction sa;
	
	memset(&sa, 0, sizeof(sa));
	sa.sa_handler =  &dump_all_record;
	sigaction(SIGUSR1, &sa, NULL);
	sigaction(SIGUSR2, &sa, NULL);
	sigaction(SIGTERM, &sa, NULL);    
}

static void free_list()
{
	if(wan_list)
		free((void *)wan_list);
	if(ssh_list)
		free((void *)ssh_list);
	if(telnet_list)
		free((void *)telnet_list);
}

int main(void)
{
	char cmd[64];
	int pid;
	
	MyDBG("[Start ProtectionSrv]\n");
	
	pid = getpid();
	snprintf(cmd, sizeof(cmd), "echo %d > %s",pid, PROTECT_SRV_PID_PATH);
	system(cmd);
	wan_list = (PTCSRV_LOGIN_RECORD_T *)calloc(500, sizeof(PTCSRV_LOGIN_RECORD_T));
	ssh_list = (PTCSRV_LOGIN_RECORD_T *)calloc(50, sizeof(PTCSRV_LOGIN_RECORD_T));
	telnet_list = (PTCSRV_LOGIN_RECORD_T *)calloc(50, sizeof(PTCSRV_LOGIN_RECORD_T));
	if(wan_list == NULL || ssh_list == NULL || telnet_list == NULL){
		free_list();
		exit(-1);
	}
	ssh_lock_t.stat.lock_rule_cnt = 0;
	ssh_lock_t.stat.record_cnt = 0;
	ssh_lock_t.cnt = 0;
#ifdef RTCONFIG_NOTIFICATION_CENTER
	ssh_lock_t.event = ADMIN_LOGIN_FAIL_SSH_EVENT;
#endif
	ssh_lock_t.maxcnt = 50;
	ssh_lock_t.record = ssh_list;
	ssh_lock_t.log_path = "/tmp/protect_srv_lan_ssh.log";
	wan_lock_t.stat.lock_rule_cnt = 0;
	wan_lock_t.stat.record_cnt = 0;
	wan_lock_t.cnt = 0;
#ifdef RTCONFIG_NOTIFICATION_CENTER
	wan_lock_t.event = ADMIN_LOGIN_FAIL_SSH_EVENT;
#endif
	wan_lock_t.maxcnt = 500;
	wan_lock_t.record = wan_list;
	wan_lock_t.log_path = "/tmp/protect_srv_wan_ssh.log";
	telnet_lock_t.stat.lock_rule_cnt = 0;
	telnet_lock_t.stat.record_cnt = 0;
	telnet_lock_t.cnt = 0;
#ifdef RTCONFIG_NOTIFICATION_CENTER
	telnet_lock_t.event = ADMIN_LOGIN_FAIL_TELNET_EVENT;
#endif
	telnet_lock_t.maxcnt = 50;
	telnet_lock_t.record = telnet_list;
	telnet_lock_t.log_path = "/tmp/protect_srv_lan_telnet.log";
	/* Signal */
	signal_register();
	
	/* start unix socket */
	local_socket_thread();
	
	while (terminated) {
		check_wanlan_lock();
		sleep(1);
	}
	free_list();
	MyDBG("ProtectionSrv Terminated\n");
	
	return 0;
}
