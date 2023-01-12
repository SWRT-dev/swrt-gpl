#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/if_packet.h>
#include <stdio.h>
#include <netinet/in.h>
#if !defined(RTCONFIG_MUSL_LIBC) && !defined(MUSL_LIBC)
#include <linux/if_ether.h>
#endif
#include <arpa/inet.h>
#include <net/if.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include "../shared/shutils.h"    // for eval()
#include "../shared/rtstate.h"
#include <bcmnvram.h>
#include <stdlib.h>
#include <asm/byteorder.h>
#include "networkmap.h"
//#include "endianness.h"
//2011.02 Yau add shard memory
#include <sys/ipc.h>
#include <sys/shm.h>
#include <rtconfig.h>
//#include "asusdiscovery.h"


CLIENT_DETAIL_INFO_TABLE *init_shared_memory(CLIENT_DETAIL_INFO_TABLE *p_client_detail_info_tab, int key)
{
	int lock, shm_client_detail_info_id;
	lock = file_lock("networkmap");
	shm_client_detail_info_id = shmget((key_t)key, sizeof(CLIENT_DETAIL_INFO_TABLE), 0666|IPC_CREAT);
	if (shm_client_detail_info_id == -1){
		fprintf(stderr,"client info shmget failed\n");
	    file_unlock(lock);
		exit(1);
	}

	p_client_detail_info_tab = (P_CLIENT_DETAIL_INFO_TABLE)shmat(shm_client_detail_info_id,(void *) 0,0);
	file_unlock(lock);
	return p_client_detail_info_tab;
}

int main(int argc, char *argv[])
{
	int i,n;
	CLIENT_DETAIL_INFO_TABLE *p_client_tab = NULL;
	p_client_tab = init_shared_memory(p_client_tab, SHMKEY_LAN);
	n = p_client_tab->ip_mac_num;
	printf("p_client_tab->ip_mac_num=%d\n", n);
	if(n == 0)
		n = 5;
	for(i = 0; i < n; i++){
		printf("idx=%d,", i);
		if (*p_client_tab->user_define[i])
			printf("p_client_tab->hostname=%s,",p_client_tab->user_define[i]);
		else
			printf("p_client_tab->hostname=%s,",p_client_tab->device_name[i]);
		printf("p_client_tab->wireless=%d,",p_client_tab->wireless[i]);
		if(p_client_tab->wireless[i] == 0)
			printf("wired,");
		else if(p_client_tab->wireless[i] == 1)
			printf("2g,");
		else if(p_client_tab->wireless[i] == 2)
			printf("5g,");
		else if(p_client_tab->wireless[i] == 3)
			printf("5g2,");
		else
			printf("???,");
		printf("p_client_tab->ip=%d.%d.%d.%d,", p_client_tab->ip_addr[i][0],p_client_tab->ip_addr[i][1],p_client_tab->ip_addr[i][2],p_client_tab->ip_addr[i][3]);
		printf("p_client_tab->mac=%02x:%02x:%02x:%02x:%02x:%02x,", p_client_tab->mac_addr[i][0],p_client_tab->mac_addr[i][1],p_client_tab->mac_addr[i][2],p_client_tab->mac_addr[i][3], p_client_tab->mac_addr[i][4],p_client_tab->mac_addr[i][5]);
		printf("p_client_tab->flag=%d,%s,",p_client_tab->device_flag[i], p_client_tab->device_flag[i]&(1<<FLAG_EXIST) ? "online" : "offline");
		printf("p_client_tab->rssi=%d\n",p_client_tab->rssi[i]);
		
	}
	return 0;
}
