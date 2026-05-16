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
 * Copyright 2026, SWRTdev.
 * Copyright 2026, paldier <paldier@hotmail.com>.
 * All Rights Reserved.
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
#include <arpa/inet.h>
#include <shared.h>
#include <cosql_utils.h>

#define NUM(a)    (sizeof (a) / sizeof (a[0]))

char g_alias[32], g_dnsip[40];

typedef struct nf_node_s {
	char name[32];
	char ip[40];
} nf_node_t;

typedef struct ping_data_s {
	char alias[32];
	char dns_ip[40];
	int valid;
	uint32_t sent;
	uint32_t recv;
	double loss_rate;
	double min;
	double avg;
	double max;
} ping_data_t;

sql_column_prototype_t dns_columns[] = {
	{"alias",			COLUMN_TYPE_TEXT	},
	{"dns_ip",			COLUMN_TYPE_TEXT	},
	{"valid",			COLUMN_TYPE_INT		},
	{"min",				COLUMN_TYPE_FLOAT	},
	{"avg",				COLUMN_TYPE_FLOAT	},
	{"max",				COLUMN_TYPE_FLOAT	},
	{"pkt_sent",		COLUMN_TYPE_UINT	},
	{"pkt_recv",		COLUMN_TYPE_UINT	},
	{"pkt_loss_rate",	COLUMN_TYPE_FLOAT	},
//	{"data_time",		COLUMN_TYPE_INT64	},
};

static void dbgprint(char *path, const char * format, ...)
{
	FILE *fp;
	int flag, fd;
	va_list args;
	struct stat st;
	char timestamp[32], path2[128];
	time_t now;

	va_start(args, format);
	flag = nvram_get_int("dns_ping_debug_flag");
	if(flag == -1 || flag == 0)
		return;
	if(stat(path, &st) != -1 && st.st_size > 0x20000){
		snprintf(path2, sizeof(path2), "%s.1", path);
		rename(path, path2);
	}
	time(&now);
	snprintf(timestamp, sizeof(timestamp), "%s", ctime(&now) + 4);
	trimNL(timestamp);
	fd = open(path, O_WRONLY | O_NONBLOCK);
	if((fd = open(path, O_WRONLY | O_NONBLOCK | O_CREAT)) > 0){
		if((fp = fdopen(fd, "a"))){
			fprintf(fp, "%s ", timestamp);
			vfprintf(fp, format, args);
			fclose(fp);
		}else
			close(fd);
	}else{
		if (fd != -1)
			close(fd);
	}
	if(flag == 1){
		if(((fd = open("/dev/console", O_WRONLY | O_NONBLOCK)) > 0) &&
			(fp = fdopen(fd, "w"))){
			fprintf(fp, "%s ", timestamp);
			vfprintf(fp, format, args);
			fclose(fp);
		}else{
			fprintf(stderr, "%s ", timestamp);
			vfprintf(stderr, format, args);
			if(fd != -1)
				close(fd);
		}
	}
}

#define DBGLOG(fmt,args...) \
	dbgprint("/tmp/dns_ping.log", "[%s:(%d)]"fmt, __func__, __LINE__, ##args);

static int parse_ping_result(const char *path, ping_data_t *data)
{
	FILE *fp;
	int send, recv;
	char line[256];
	double min, avg, max;

	memset(line, 0, sizeof(line));
	if(!path){
		DBGLOG("Null filename.\n");
		return -1;
	}
	fp = fopen(path, "r");
	if(fp){
		while(fgets(line, sizeof(line), fp)){
			if(strstr(line, "packet loss")){
				DBGLOG("linebuf=[%s]\n", line);
				if(sscanf(line, "%d packets transmitted, %d packets received", &send, &recv) != 2){
					DBGLOG("Failed to parse the [%s].\n", line);
					break;
				}
				data->sent = send;
				data->recv = recv;
				if(send > 0){
					if(recv > 0)
						data->valid = 1;
					data->loss_rate = (double)(send - recv) * 100.0 / (double)send;
				}
			}else if(strstr(line, "round-trip min/avg/max")){
				DBGLOG("linebuf=[%s]\n", line);
				if(sscanf(line, "round-trip min/avg/max = %lf/%lf/%lf ms", &min, &avg, &max) == 3){
					data->min = min;
					data->avg = avg;
					data->max = max;
					data->valid = 1;
				}else
					DBGLOG("Failed to parse the [%s].\n", line);
				break;
			}
		}
		fclose(fp);
	}else
		DBGLOG("Cannot open [%s].\n", path);
	return 0;
}

static int nf_node_dns_ping_columns_format(sql_column_t *cols, ping_data_t *data)
{
	if(!cols || !data){
		DBGLOG("Null pointer...\n");
		return -1;
	}
	snprintf(g_alias, sizeof(g_alias), "%s", data->alias);
	snprintf(g_dnsip, sizeof(g_dnsip), "%s", data->dns_ip);
	cols[0].name = "alias";        cols[0].type = COLUMN_TYPE_TEXT;    cols[0].value.t    = g_alias;
	cols[1].name = "dns_ip";       cols[1].type = COLUMN_TYPE_TEXT;    cols[1].value.t    = g_dnsip;
	cols[2].name = "valid";        cols[2].type = COLUMN_TYPE_INT;     cols[2].value.i    = data->valid;
	cols[3].name = "min";          cols[3].type = COLUMN_TYPE_FLOAT;   cols[3].value.f    = data->min;
	cols[4].name = "avg";          cols[4].type = COLUMN_TYPE_FLOAT;   cols[4].value.f    = data->avg;
	cols[5].name = "max";          cols[5].type = COLUMN_TYPE_FLOAT;   cols[5].value.f    = data->max;
	cols[6].name = "pkt_sent";     cols[6].type = COLUMN_TYPE_UINT;    cols[6].value.ui   = data->sent;
	cols[7].name = "pkt_recv";     cols[7].type = COLUMN_TYPE_UINT;    cols[7].value.ui   = data->recv;
	cols[8].name = "pkt_loss_rate";cols[8].type = COLUMN_TYPE_FLOAT;   cols[8].value.f   = data->loss_rate;
	DBGLOG("'%s'=[%s]\n", "dns_ip", g_dnsip);
	return 0;
}

int main(int argc, char *argv[])
{
	int char_count, i = 0, n, j, ret;
	char *pt, *name, *ip, *next;
	char path[256], this_db_version[MAX_VERSION_LEN], cmd[256], word[64];
	sqlite3* pdb;
	nf_node_t *list, *lp;
	sql_column_t cols[NUM(dns_columns)];
	ping_data_t data;
	sql_column_match_t match_and_columns[0];

	DBGLOG("Program starts here...\n");
	nvram_set("dns_ping_state", "0");
	if(!d_exists("/tmp/dns_ping_result")){
		mkdir("/tmp/dns_ping_result", 0777);
		if(!d_exists("/tmp/dns_ping_result")){
			DBGLOG("Cannot create folder [%s]\n", "/tmp/dns_ping_result");
			DBGLOG("Program is terminated...\n");
			nvram_set("dns_ping_state", "-1");
			return -1;
		}
	}
	if(!d_exists("/tmp/.diag"))
		mkdir("/tmp/.diag", 0777);
	pt = nvram_get("dns_ping_db_path");
	if(!pt || !*pt){
		snprintf(path, sizeof(path), "%s/%s.db", "/tmp/.diag", "dns_ping");
		nvram_set("dns_ping_db_path", path);
	}
	pdb = cosql_open(nvram_safe_get("dns_ping_db_path"));
	if(!pdb){
		DBGLOG("Cannot open database [%s]\n", nvram_safe_get("dns_ping_db_path"));
		DBGLOG("Program is terminated...\n");
		nvram_set("dns_ping_state", "-2");
		return -1;
	}
	if(nvram_match("dns_ping_libcodb_debug", "1"))
		cosql_enable_debug(pdb, 1);
	cosql_get_db_version(pdb, this_db_version);
	if(strcmp(this_db_version, "v0.2")){
		cosql_drop_db(pdb);
		cosql_create_table(pdb, "v0.2", NUM(dns_columns), dns_columns);
	}
	cosql_truncate_data_db(pdb);
	if(argc > 1){
		pt = argv[1];
		if(!*pt)
			pt = nvram_safe_get("dns_ping_list");
	}else
		pt = nvram_safe_get("dns_ping_list");
	DBGLOG("dns_ping_list=[%s]\n", pt);
	char_count = get_char_count(pt, '<');
	list = malloc(sizeof(nf_node_t) * char_count);
	if(!list){
		DBGLOG("Failed to malloc!\n");
		return -1;
	}
	nvram_set("dns_ping_state", "1");
	foreach_60(word, pt, next){
		if(vstrsep(word, ">", &name, &ip) == 2){
			if(is_valid_ip4(ip) || is_valid_ip6(ip)){
				if(name){
					if(ip && i < char_count){
						lp = &list[i];
						snprintf(lp->name, 32, "%s", name);
						snprintf(lp->ip, 40, "%s", ip);
						i++;
						DBGLOG("<%d>[%s]=[%s]\n", i, lp->name, lp->ip);
						memset(path, 0, sizeof(path));
						memset(cmd, 0, sizeof(cmd));
						snprintf(path, sizeof(path), "%s/%s", "/tmp/dns_ping_result", lp->ip);
						snprintf(cmd, sizeof(cmd), "rm -f %s.done;rm -f %s", path, path);
						system(cmd);
						memset(cmd, 0, sizeof(cmd));
						snprintf(cmd, sizeof(cmd), 
							"ping -c %d -W %d %s > %s 2>&1 || true && echo \"\" >> %s.done &", 5, 1,
							lp->ip, path, path);
						system(cmd);
					}
				}
			}else
				DBGLOG("IP format is wrong:[%s]\n", ip);
		}
	}
	for(j = 10; j > 0; j--){
		for(n = 0; n < i; n++){
			lp = &list[n];
			memset(path, 0, sizeof(path));
			snprintf(path, sizeof(path), "%s/%s.done", "/tmp/dns_ping_result", lp->ip);
			if(!f_exists(path)){
				DBGLOG("Wait for ping result for [%s]\n", lp->ip);
				sleep(1);
				break;
			}
		}
	}
	nvram_set("dns_ping_state", "2");
	for(j = 0; j< i; j++){
		lp = &list[j];
		memset(path, 0, sizeof(path));
		snprintf(path, sizeof(path), "%s/%s", "/tmp/dns_ping_result", lp->ip);
		if(f_exists(path)){
			memset(&data, 0, sizeof(data));
			memset(cols, 0, sizeof(cols));
			snprintf(data.alias, sizeof(data.alias), "%s", lp->name);
			snprintf(data.dns_ip, sizeof(data.dns_ip), "%s", lp->ip);
			DBGLOG("data.dns_ip=[%s]\n", data.dns_ip);
			if(parse_ping_result(path, &data))
				ret = -1;
			else{
				nf_node_dns_ping_columns_format(cols, &data);
				match_and_columns[0].type = 0;
				match_and_columns[0].operation = 0;
				match_and_columns[0].value.t = g_dnsip;
				match_and_columns[0].name = "dns_ip";
				ret = cosql_upsert_table(pdb, NUM(match_and_columns), &match_and_columns[0], 0, NULL, NUM(cols), cols);
			}
			DBGLOG("alias[%s],dns_ip[%s],valid[%d],sent[%ld],recv[%ld],"
				"loss_rate[%lf],min[%lf],avg[%lf],max[%lf],ret[%d]\n", 
				data.alias, data.dns_ip, data.valid, data.sent, data.recv, 
				data.loss_rate, data.min, data.avg, data.max, ret);
		}
	}
	cosql_close(pdb);
	free(list);
	DBGLOG("Program ends here...\n");
	nvram_set("dns_ping_state", "3");
	return 0;
}
