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
 */

/*
 * Copyright 2021-2022, ASUS
 * Copyright 2023, SWRTdev
 * Copyright 2023, paldier <paldier@hotmail.com>.
 * All Rights Reserved.
 */

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <string.h>
#include <time.h>

#include <rtconfig.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <bcmnvram.h>
#include <bcmparams.h>
#include <utils.h>
#include <shutils.h>
#include <shared.h>

#include "openvpn_config.h"
#include "openvpn_control.h"
#include "openvpn_utils.h"

int current_addr(in_addr_t addr)
{
	int fd;
	struct ifconf ifconf;
	struct ifreq *ifreq;
	char buf[8192];

	fd = socket(AF_INET, SOCK_DGRAM, 0);
	if(fd < 0){
		perror("socket()");
		return 0;
	}

	ifconf.ifc_len = sizeof(buf);
	ifconf.ifc_buf = buf;
	if (ioctl(fd, SIOCGIFCONF, &ifconf) != 0) {
		perror("ioctl(SIOCGIFCONF)");
		return 0;
	}
	ifreq = ifconf.ifc_req;
	while ( (char*)ifreq < buf + ifconf.ifc_len ) {
		if( ((struct sockaddr_in*)&ifreq->ifr_addr)->sin_addr.s_addr == addr ) {
			char text[16];
			_dprintf("ifr %s: %8X(%s)\n", ifreq->ifr_name, addr, inet_ntop(AF_INET, &(((struct sockaddr_in*)&ifreq->ifr_addr)->sin_addr), text, sizeof(text)));
			return 1;
		}

		ifreq = (struct ifreq*)((char*)ifreq + sizeof(*ifreq));
	}

	return 0;
}

int current_route(in_addr_t network, in_addr_t netmask)
{
	unsigned int dest = 0, mask = 0;
	char buf[256] = {0};
	FILE *fp;
	if((fp = fopen("/proc/net/route", "r")) == NULL)
		return 0;
	while(fgets(buf, sizeof(buf), fp) != NULL){
		if(strncmp(buf, "Iface", 5)){
			if(sscanf(buf, "%*s %x %*s %*s %*s %*s %*s %x", &dest, &mask) != 2)
				continue;
			if(dest == network && netmask >= mask){
				fclose(fp);
				return 1;
			}
		}
	}
	fclose(fp);
	return 0;
}

char* get_default_gateway_dev(char *iface, size_t len)
{
	int count = 0, found = 0;
	unsigned int dest = 0, gate = 0, mask = 0, flags = 0, metric = 0;
	char buf[256] = {0}, device[16] = {0};
	FILE *fp;
	if((fp = fopen("/proc/net/route", "r")) == NULL)
		return NULL;
	while(fgets(buf, sizeof(buf), fp) != NULL){
		if(count){
			if(sscanf(buf, "%15s\t%x\t%x\t%x\t%*s\t%*s\t%d\t%x", device, &dest, &gate, &flags, &metric, &mask) == 6 && flags){
				if(ntohl(dest) == 0 && ntohl(mask) == 0 && metric < 255){
					found = 1;
					break;
				}
			}
		}
		count++;
	}
	fclose(fp);
	if(found){
		strlcpy(iface, device, len);
		return iface;
	}
	return NULL;
}
