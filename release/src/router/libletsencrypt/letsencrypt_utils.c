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

#include <bcmnvram.h>
#include <bcmparams.h>
#include <utils.h>
#include <shutils.h>
#include <shared.h>
#include <openssl/pem.h>
#include <openssl/conf.h>
#include <openssl/x509v3.h>
#include <openssl/asn1.h>
#include "letsencrypt_config.h"
#include "letsencrypt_control.h"
#include "letsencrypt_utils.h"

int f_diff(const char *path1, const char *path2)
{
	int ch1, ch2;
	FILE *fp1, *fp2;
	fp1 = fopen(path1, "r");
	if(fp1 == NULL)
		return 1;
	fp2 = fopen(path2, "r");
	if(fp2 == NULL){
		fclose(fp1);
		return 1;
	}
	while(1){
		ch1 = getc(fp1);
		ch2 = getc(fp2);
		if((ch1 == -1 || ch2 == -1) || ch1 != ch2)
			break;
	}
	fclose(fp1);
	fclose(fp2);
	return ch1 != ch2;
}

void parse_cert_cn(char *src, char *dst, size_t len)
{
	char *pt1, *pt2;
	pt1 = strstr(src, "CN=");
	if(pt1){
		strlcpy(dst, pt1 + 3, len);
		pt2 = strchr(dst, '/');
		if(pt2)
			*pt2 = 0;
	}
}

struct tm *ASN1_TimeToTM(ASN1_TIME* atime, struct tm *t)
{
	unsigned char* data = atime->data;
	int i = 0;

	if (atime->type == V_ASN1_UTCTIME)
	{
		t->tm_year =
			(data[0] - '0') * 10 +
			(data[1] - '0');
		if(t->tm_year < 70)
			t->tm_year += 100;
		i = 2;
	}
	else if (atime->type == V_ASN1_GENERALIZEDTIME)
	{
		t->tm_year =
			(data[0] - '0') * 1000 +
			(data[1] - '0') * 100 +
			(data[2] - '0') * 10 +
			(data[3] - '0');
		t->tm_year -= 1900;
		i = 4;
	}
	t->tm_mon = (data[i] - '0') * 10 + (data[i+1] - '0') - 1;
	i += 2;
	t->tm_mday = (data[i] - '0') * 10 + (data[i+1] - '0');
	i += 2;
	t->tm_hour = (data[i] - '0') * 10 + (data[i+1] - '0');
	i += 2;
	t->tm_min  = (data[i] - '0') * 10 + (data[i+1] - '0');
	i += 2;
	t->tm_sec  = (data[i] - '0') * 10 + (data[i+1] - '0');
	return t;
}

u_int16_t pick_random_port(void)
{
	int skfd;
	socklen_t alen;
	struct sockaddr_in addr;
	skfd = socket(AF_INET, SOCK_STREAM, 0);
	if(skfd != 0)
		return 0;
	addr.sin_family = AF_INET; /* host byte order */
	addr.sin_port = htons(0); /* short, network byte order */
	addr.sin_addr.s_addr = INADDR_ANY;
	bzero(&(addr.sin_zero), sizeof(addr.sin_zero)); /* zero the rest of the struct */
	alen = sizeof(addr);
	if(bind(skfd, (struct sockaddr *)&addr, sizeof(addr)) != 0 || getsockname(skfd, (struct sockaddr *)&addr, &alen) != 0){
		close(skfd);
		return 0;
	}
	close(skfd);
	return ntohs(addr.sin_port);
}
