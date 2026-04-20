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
 * Copyright 2024, ASUSTeK Inc.
 * Copyright 2024, SWRTdev.
 * Copyright 2024, paldier <paldier@hotmail.com>.
 * All Rights Reserved.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <json.h>
#include <libasc.h>
#include "networkmap.h"
#include "sm.h"
#include "download.h"
#include "protect_name.h"
#include "shared_func.h"
#include "JSON_checker.h"

int cloud_db_process()
{
	char version[11], checksum[65];
	if(download_cloud_version(1, 1) == 1){
		get_nmp_db_from_local_version_file(1, 1, version, sizeof(version), checksum, sizeof(checksum));
		printf("[%s] download_version, flag = %d, success \n", __func__, 1);
	}else
		printf("[%s] download_version, flag = %d, failure \n", __func__, 0);
	return 0;
}

int get_version_from_file(const char *file, int check_sig, int file_enc, char *version)
{
	int ret = 0;
	char *buf, *pt;
	struct json_object *root = NULL, *o_version;
	buf = read_file(file, check_sig, file_enc);
	if(buf){
		root = json_tokener_parse(buf);
		if(root){
			json_object_object_get_ex(root, "version", &o_version);
			if(o_version){
				ret = 1;
				strlcpy(version, json_object_get_string(o_version), 11);
			}
			json_object_put(root);
		}
		free(buf);
	}
	return ret;
}

int download_cloud_version(const int check_sig, const int file_enc)
{
	int ret_dl, ret_cur;
	char command[256], cur_ver[12], dl_ver[12], path[256];
	memset(path, 0, sizeof(path));
	memset(cur_ver, 0, sizeof(cur_ver));
	memset(dl_ver, 0, sizeof(dl_ver));
	if(!check_if_dir_exist(local_nmp_dir[0])){
		if(check_if_file_exist(local_nmp_dir[0])){
			snprintf(command, sizeof(command), "rm -rf %s", local_nmp_dir[0]);
			system(command);
		}
		mkdir(local_nmp_dir[0], 0744);
		printf("[%s]mkdir local_nmp_dir = %s \n", __func__, local_nmp_dir[0]);
	}
	if(download_file(version_name[0], cloud_ver_path[0]) != 1){
		printf("[%s], download_file error\n", __func__);
		return NMP_FAIL;
	}
	printf("[%s] download_file = %s \n", __func__, version_name[0]);
	ret_dl = get_version_from_file(cloud_ver_path[0], check_sig, 0, dl_ver);
	if(ret_dl == 1){
		printf("[%s] cloud_ver_path = %s, dl_ver = %s, parse NMP_SUCCESS \n", __func__, cloud_ver_path[0], dl_ver);
		ret_cur = get_version_from_file(local_ver_path[0], check_sig, file_enc, cur_ver);
		printf("[%s] local_ver_path = %s, dl_ver = %s, cur_ver = %s, r = %d \n", __func__, local_ver_path[0], 
			dl_ver, cur_ver, ret_cur);
		if(ret_cur == 1){
			if(atoi(dl_ver) == atoi(cur_ver)){
				unlink(cloud_ver_path[0]);
				printf("[%s], same version >> dl_ver = [%s] == cur_ver = [%s]\n", __func__, dl_ver, cur_ver);
			}
			return NMP_FAIL;
		}else if(ret_cur == 0){
			if(file_enc){
				snprintf(path, sizeof(path), "%s_enc", local_ver_path[0]);
				if(encrypt_file(cloud_ver_path[0], path, check_sig) == 1){
					unlink(local_ver_path[0]);
					eval("mv", path, (char *)local_ver_path[0]);
					return NMP_SUCCESS;
				}
			}else{
				unlink(local_ver_path[0]);
				eval("cp", (char *)cloud_ver_path[0], (char *)local_ver_path[0]);
				return NMP_SUCCESS;
			}
		}
	}
	return NMP_FAIL;
}

int parse_nmp_db(const char *parse_db_path, struct json_object *create_nmp_db_array)
{
	int len, i;
	char keyword[64], type[16], os_type[16];
	struct json_object *root = NULL, *o_array, *o_keyword, *o_type, *o_os_type, *o_tmp;

	root = json_object_from_file(parse_db_path);
	if(root){
		len = json_object_array_length(root);
		if(len > 0){
			for(i = 0; i < len; i++){
				memset(keyword, 0, sizeof(keyword));
				memset(type, 0, sizeof(type));
				memset(os_type, 0, sizeof(os_type));
				o_array = json_object_array_get_idx(root, i);
				json_object_object_get_ex(o_array, "keyword", &o_keyword);
				json_object_object_get_ex(o_array, "type", &o_type);
				json_object_object_get_ex(o_array, "os_type", &o_os_type);
				if(o_keyword)
					snprintf(keyword, sizeof(keyword), "%s", json_object_get_string(o_keyword));
				if(o_type)
					snprintf(type, sizeof(type), "%s", json_object_get_string(o_type));
				if(o_os_type)
					snprintf(os_type, sizeof(os_type), "%s", json_object_get_string(o_os_type));
				o_tmp = json_object_new_object();
				json_object_object_add(o_tmp, "keyword", json_object_new_string(keyword));
				json_object_object_add(o_tmp, "type", json_object_new_string(type));
				json_object_object_add(o_tmp, "os_type", json_object_new_string(os_type));
				json_object_array_add(create_nmp_db_array, o_tmp);
			}
		}
		return NMP_SUCCESS;
	}
	printf("[%s] json_tokener_parse failure, filename = %s \n", __func__, parse_db_path);
	return -1;
}

void merge_json_file(const char *nmp_db_path, const char *local_db_path, const char *cloud_db_path)
{
	char path[256];
	struct json_object *create_nmp_db_array = NULL;
	memset(path, 0, sizeof(path));
	snprintf(path, sizeof(path), "%s/merge_%s", local_nmp_dir[0], nmp_db_path);
	printf("[%s] merge_nmp_db_path : %s  \n", __func__, path);
	create_nmp_db_array = json_object_new_array();
	if(!create_nmp_db_array)
		printf("[%s] Cannot create array object \n", __func__);
	printf("[%s][%i] create_nmp_db_array len 1 = %d \n", __func__, __LINE__, strlen(json_object_get_string(create_nmp_db_array)));
	printf("[%s][%i] parse keep_local_db_path = %s \n", __func__, __LINE__, local_db_path);
	parse_nmp_db(local_db_path, create_nmp_db_array);
	printf("[%s][%i] create_nmp_db_array len 2 = %d \n", __func__, __LINE__, strlen(json_object_get_string(create_nmp_db_array)));
	printf("[%s][%i] parse cloud_db_path = %s \n", __func__, __LINE__, cloud_db_path);
	parse_nmp_db(cloud_db_path, create_nmp_db_array);
	printf("[%s][%i] create_nmp_db_array len 3 = %d \n", __func__, __LINE__, strlen(json_object_get_string(create_nmp_db_array)));
	json_object_to_file(path, create_nmp_db_array);
	json_object_put(create_nmp_db_array);
}

int nmp_db_cmp(const char* cloud_db_path, char* local_db_keyword, char* local_db_type, char* local_db_os_type)
{
	int len, i;
	char keyword[64], type[16], os_type[16];
	struct json_object *root = NULL, *o_array, *o_keyword, *o_type, *o_os_type, *o_tmp;
	root = json_object_from_file(cloud_db_path);
	if(root){
		len = json_object_array_length(root);
		if(len > 0){
			for(i = 0; i < len; i++){
				memset(keyword, 0, sizeof(keyword));
				memset(type, 0, sizeof(type));
				memset(os_type, 0, sizeof(os_type));
				o_array = json_object_array_get_idx(root, i);
				json_object_object_get_ex(o_array, "keyword", &o_keyword);
				json_object_object_get_ex(o_array, "type", &o_type);
				json_object_object_get_ex(o_array, "os_type", &o_os_type);
				if(o_keyword)
					snprintf(keyword, sizeof(keyword), "%s", json_object_get_string(o_keyword));
				if(o_type)
					snprintf(type, sizeof(type), "%s", json_object_get_string(o_type));
				if(o_os_type)
					snprintf(os_type, sizeof(os_type), "%s", json_object_get_string(o_os_type));
				if(!strcmp(keyword, local_db_keyword)){
					snprintf(local_db_keyword, 64, "%s", keyword);
					snprintf(local_db_type, 16, "%s", type);
					snprintf(local_db_os_type, 16, "%s", os_type);
					printf("[%s] get cloud db type = local db type = %s, local_db_keyword = %s \n", __func__, 
						local_db_type, local_db_keyword);
					json_object_put(root);
					return NMP_SUCCESS;
				}
			}
		}
		json_object_put(root);
		return -1;
	}
	printf("[%s] json_tokener_parse failure, filename = %s \n", __func__, cloud_db_path);
	return -1;
}

int keep_local_nmp_db(const char * db_type, const char * keep_local_db_path,  const char * cloud_db_path)
{
	int len, i, ret;
	char keyword[64], type[16], os_type[16], path[64];
	struct json_object *root = NULL, *o_array, *o_keyword, *o_type, *o_os_type, *o_array_new = NULL, *o_tmp;
	if(strstr("/usr/networkmap/nmp_conv_type.js", db_type))
		strcpy(path, "/usr/networkmap/nmp_conv_type.js");
	else if(strstr("/usr/networkmap/nmp_vendor_type.js", db_type))
		strcpy(path, "/usr/networkmap/nmp_vendor_type.js");
	else if(strstr("/usr/networkmap/nmp_bwdpi_type.js", db_type))
		strcpy(path, "/usr/networkmap/nmp_bwdpi_type.js");
	printf("[%s] db_type = %s, db_type_path : %s  \n", __func__, db_type, path);
	root = json_object_from_file(path);
	if(root){
		len = json_object_array_length(root);
		o_array_new = json_object_new_array();
		if(!o_array_new)
			printf("[%s] Cannot create array object \n", __func__);
		if(len > 0){
			for(i = 0; i < len; i++){
				memset(keyword, 0, sizeof(keyword));
				memset(type, 0, sizeof(type));
				memset(os_type, 0, sizeof(os_type));
				o_array = json_object_array_get_idx(root, i);
				json_object_object_get_ex(o_array, "keyword", &o_keyword);
				json_object_object_get_ex(o_array, "type", &o_type);
				json_object_object_get_ex(o_array, "os_type", &o_os_type);
				if(o_keyword)
					snprintf(keyword, sizeof(keyword), "%s", json_object_get_string(o_keyword));
				if(o_type)
					snprintf(type, sizeof(type), "%s", json_object_get_string(o_type));
				if(o_os_type)
					snprintf(os_type, sizeof(os_type), "%s", json_object_get_string(o_os_type));
				printf("[%s] before db_keyword = %s \n", __func__, keyword);
				printf("[%s] before db_type = %s \n", __func__, type);
				printf("[%s] before db_os_type = %s \n", __func__, os_type);
				ret = nmp_db_cmp(cloud_db_path, keyword, type, os_type);
				if(ret == 1)
					printf("[%s] nmp_db_cmp  status success  = %d \n", __func__, ret);
				else{
					printf("[%s] nmp_db_cmp  status failure  = %d \n", __func__, ret);
					o_tmp = json_object_new_object();
					json_object_object_add(o_tmp, "keyword", json_object_new_string(keyword));
					json_object_object_add(o_tmp, "type", json_object_new_string(type));
					json_object_object_add(o_tmp, "os_type", json_object_new_string(os_type));
					json_object_array_add(o_array_new, o_tmp);
				}
				printf("[%s] after db_keyword = %s \n", __func__, keyword);
				printf("[%s] after db_type = %s \n", __func__, type);
				printf("[%s] after db_os_type = %s \n", __func__, os_type);
			}

		}
		json_object_to_file(keep_local_db_path, o_array_new);
		json_object_put(o_array_new);
		json_object_put(root);
		return 0;
	}
	printf("[%s] json_tokener_parse failure, filename = %s \n", __func__, path);
	return -1;
}

int merge_local_db_and_cloud_db(const char *cur_nmp_db_type, const char *cloud_db_path)
{
	char path[256];
	memset(path, 0, sizeof(path));
	snprintf(path, sizeof(path), "%s/keep_%s", local_nmp_dir[0], cur_nmp_db_type);
	printf("[%s] keep_local_db_path : %s  \n", __func__, path);
	printf("[%s] cloud_db_path : %s  \n", __func__, cloud_db_path);
	keep_local_nmp_db(cur_nmp_db_type, path, cloud_db_path);
	merge_json_file(cur_nmp_db_type, path, cloud_db_path);
	unlink(path);
	unlink(cloud_db_path);
	return 0;
}

int download_nmp_db(const char *cur_nmp_db_type, const char *cur_nmp_db_ver, const char *cur_nmp_db_checksum, const int check_sig)
{
	int ret = 0, len, i;
	char path[128], file_checksum[8], local_file_path[128], *buf;
	const char *checksum_pt = NULL, *db_pt = NULL, *version_pt = NULL;
	struct json_object *root = NULL, *o_array = NULL, *o_db_list, *o_db, *o_version, *o_checksum;
	memset(path, 0, sizeof(path));
	printf("[%s] cloud_ver_path : %s  \n", __func__, cloud_ver_path[0]);
	buf = read_file(cloud_ver_path[0], check_sig, 0);
	printf("[%s] buf : %s \n", __func__, buf);
	if(buf){
		root = json_tokener_parse(buf);
		if(root){
			json_object_object_get_ex(root, "db_list", &o_db_list);
			len = json_object_array_length(o_db_list);
			printf("db_number : %d \n", len);
			if(len > 0){
				for(i = 0; i < len; i++){
					o_array = json_object_array_get_idx(root, i);
					json_object_object_get_ex(o_array, "db", &o_db);
					json_object_object_get_ex(o_array, "version", &o_version);
					json_object_object_get_ex(o_array, "checksum", &o_checksum);
					if(o_db)
						db_pt = json_object_get_string(o_db);
					if(o_version)
						version_pt = json_object_get_string(o_version);
					if(o_checksum)
						checksum_pt = json_object_get_string(o_checksum);
					printf("[%s] dl_nmp_db_type : %s \n", __func__, db_pt);
					printf("[%s] dl_nmp_db_ver : %s \n", __func__, version_pt);
					printf("[%s] dl_nmp_db_checksum : %s \n", __func__, checksum_pt);
					if(!strcmp(cur_nmp_db_type, db_pt)){
						printf("[%s] i = %d, cur_nmp_db_type : %s, dl_nmp_db_type : %s \n", __func__, i,
							cur_nmp_db_type, db_pt);
						snprintf(path, sizeof(path), "%s/%s", local_nmp_dir[0], "");
						printf("[%s] db_type_path : %s \n", __func__, path);
						if(version_pt && checksum_pt){
							if(access(path, F_OK) == -1 || atoi(version_pt) > atoi(cur_nmp_db_ver)){
								memset(local_file_path, 0, sizeof(local_file_path));
								snprintf(local_file_path, sizeof(local_file_path), "%s/cloud_%s", local_nmp_dir[0], cur_nmp_db_type);
								printf("[%s] cloud_db_path : %s  \n", __func__, local_file_path);
								if(download_file(cur_nmp_db_type, local_file_path) == 1 && get_file_sha256_checksum(local_file_path, file_checksum, sizeof(file_checksum), 0) == 1){
									printf("[%s] dl_nmp_db_checksum = %s, file_checksum : %s  \n", __func__,
										checksum_pt, file_checksum);
									if(!strcmp(file_checksum, checksum_pt)){
										ret = 1;
										merge_local_db_and_cloud_db(cur_nmp_db_type, local_file_path);
									}
								}
							}
						}
					}else{
						printf("[%s] i = %d, db type diff, cur_nmp_db_type : %s, dl_nmp_db_type : %s \n", 
							__func__, i, cur_nmp_db_type, db_pt);
					}
				}
			}
			json_object_put(root);
		}
		free(buf);
	}
	return ret;
}

int get_nmp_db_from_local_version_file(const int check_sig, const int file_enc, char *version,
	const size_t ver_len, char *checksum, const size_t checksum_len)
{
	int ret = 1, len, i;
	char path[128], file_checksum[8], local_file_path[128], *buf;
	const char *checksum_pt = NULL, *db_pt = NULL, *version_pt = NULL;
	struct json_object *root = NULL, *o_array, *o_db_list, *o_db, *o_version, *o_checksum = NULL;
	printf("[%s] local_ver_path : %s  \n", __func__, local_ver_path[0]);
	buf = read_file(local_ver_path[0], check_sig, file_enc);
	if(buf){
		root = json_tokener_parse(buf);
		if(root){
			printf("file_obj : %s \n", json_object_get_string(root));
			json_object_object_get_ex(root, "db_list", &o_db_list);
			printf("[%s] db_obj : %s \n", __func__, json_object_get_string(o_db_list));
			len = json_object_array_length(o_db_list);
			printf("db_number : %d \n", len);
			if(len > 0){
				for(i = 0; i < len; i++){
					o_array = json_object_array_get_idx(root, i);
					json_object_object_get_ex(o_array, "db", &o_db);
					json_object_object_get_ex(o_array, "version", &o_version);
					json_object_object_get_ex(o_array, "checksum", &o_checksum);
					if(o_db)
						db_pt = json_object_get_string(o_db);
					if(o_version)
						version_pt = json_object_get_string(o_version);
					if(o_checksum)
						checksum_pt = json_object_get_string(o_checksum);
					printf("[%s] i = %d, db_type_str : %s \n", __func__, i, db_pt);
					printf("[%s] i = %d, ver_str : %s \n", __func__, i, version_pt);
					printf("[%s] i = %d, checksum_str : %s \n", __func__, i, checksum_pt);
					if(version_pt && checksum_pt && db_pt){
						ret = 0;
						strlcpy(version, version_pt, ver_len);
						strlcpy(checksum, checksum_pt, checksum_len);
						download_nmp_db(db_pt, version_pt, checksum_pt, check_sig);
					}
				}
			}
			json_object_put(root);
		}
		free(buf);
	}
	return ret;
}

//networkmap=> stay closed
extern int networkmap_lock;
extern int clients_start_ts;
extern int check_clients_interval;
extern int update_oui;
extern int client_updated;
extern unsigned char my_ipaddr[4];
extern struct json_object *G_OUI_JSON;
extern ac_state *g_vendor_state;
extern CLIENT_DETAIL_INFO_TABLE *G_CLIENT_TAB;
#ifdef RTCONFIG_MULTILAN_CFG
extern unsigned char mtlan_ip[4];
extern int mtlan_map;
extern int mtlan_updated;
extern struct sockaddr_in mtlan_ipaddr[MTLAN_MAXINUM];
#endif
extern int check_vendorclass(const char *mac, CLIENT_DETAIL_INFO_TABLE *tab, int x, char *file);
extern int nmp_wl_offline_check(CLIENT_DETAIL_INFO_TABLE *p_client_tab, int offline);

int check_oui(const char *mac_buf, CLIENT_DETAIL_INFO_TABLE *tab, unsigned int x)
{
	int index;
	char oui[8], type = 0, base = 0, *oui_pt;
	struct json_object *o_oui;
	NMP_DEBUG("%s >> mac_buf = %s \n", __func__, mac_buf);
	if(tab->type[x] == 24)//fixme
		return -1;
	snprintf(oui, sizeof(oui), "%02X%02X%02X", tab->mac_addr[x][0], tab->mac_addr[x][1], tab->mac_addr[x][2]);
	json_object_object_get_ex(G_OUI_JSON, oui, &o_oui);
	oui_pt = (char *)json_object_get_string(o_oui);
	if(!oui_pt){
		NMP_DEBUG("*** mac[ %s ] Not Find OUI data >> [%s] , typeID = %d , os_type = %d\n", mac_buf, "", type, base);
		return -1;
	}
	NMP_DEBUG("*** mac[ %s ] Find OUI data >> [%s] , typeID = %d , os_type = %d\n", mac_buf, oui_pt, type, base);
	strlcpy(tab->vendorClass[x], oui_pt, sizeof(tab->vendorClass[0]));
	NMP_DEBUG("%s >> OUI data >> vendor db keyword search >> prefix_search_index [%s] \n", __func__, oui_pt);
	index = prefix_search_index(g_vendor_state, oui_pt, &base, &type);
	if(index){
		strlcpy(tab->vendor_name[x], oui_pt, sizeof(tab->vendor_name[0]));
		strlcpy(tab->device_name[x], oui_pt, index);
		strlcpy(tab->vendorClass[x], oui_pt, index);
		type_filter(G_CLIENT_TAB, x, type, base, 0);
		NMP_DEBUG("%s >> mac[%s] >> OUI data >> Find vendorType >> device name = %s, vendor_name = %s, index = %d, typeID = %d,  os_type = %d \n", __func__, mac_buf, tab->device_name[x], tab->vendor_name[x], index, type, base);
		return 0;
	}else{
		strlcpy(tab->device_name[x], oui_pt, sizeof(tab->device_name[0]));
		strlcpy(tab->vendor_name[x], oui_pt, sizeof(tab->vendor_name[0]));
		NMP_DEBUG("%s >> mac[%s] >> OUI data >> Not Find vendorType >> device name = %s, vendor_name = %s, index = %d, typeID = %d,  os_type = %d \n", __func__, mac_buf, tab->device_name[x], tab->vendor_name[x], index, type, base);
	}
	return -1;
}

int QueryVendorOuiInfo(CLIENT_DETAIL_INFO_TABLE *tab, int x)
{
	int ret;
	char mac_buf[18] = {0};
	snprintf(mac_buf, sizeof(mac_buf), "%02X:%02X:%02X:%02X:%02X:%02X", tab->mac_addr[x][0], tab->mac_addr[x][1],
		tab->mac_addr[x][2], tab->mac_addr[x][3], tab->mac_addr[x][4], tab->mac_addr[x][5]);
	NMP_DEBUG("%s >> mac_buf : %s, check_vendorclass > %s \n", __func__, mac_buf, NMP_VC_JSON_FILE);
	ret = check_vendorclass(mac_buf, tab, x, NMP_VC_JSON_FILE);
	if(ret == 1){
		NMP_DEBUG("%s >> mac_buf : %s, check_vendorclass > %s \n", __func__, mac_buf, NMP_CL_JSON_FILE);
		ret = check_vendorclass(mac_buf, tab, x, NMP_CL_JSON_FILE);
	}
	NMP_DEBUG("%s(): vendor_class_status = %d, vendorClass =  %s, os_type = %d, type = %d\n", __func__, ret, tab->vendorClass[x], tab->os_type[x], tab->type[x]);
	if(update_oui && ret == 1)
		check_oui(mac_buf, tab, x);
	if(!strcmp(tab->vendor_name[x], "ASUS") || !strcmp(tab->device_name[x], "ASUS")){
		tab->device_flag[x] |= (1 << FLAG_ASUS);
		NMP_DEBUG("%s(): get asus device, p_client_detail_info_tab->device_flag[i] = %d\n", __func__, tab->device_flag[x]);
	}
	return 1;
}

int get_brctl_macs(char * mac)
{
	FILE *fp;
	int port = -1;
	char cmd[64] = {0}, line[128], *next, macaddr[18] = {0};
	toLowerCase(mac);
	snprintf(cmd, sizeof(cmd), "brctl showmacs %s > %s", nvram_safe_get("lan_ifname"), BRCTL_TABLE_PATH);
	system(cmd);
	NMP_DEBUG("mac = %s, cmd = %s\n", mac, cmd);
	fp = fopen(BRCTL_TABLE_PATH, "r");
	if(fp){
		fgets(line, sizeof(line), fp);//skip first line
		while(fgets(line, sizeof(line), fp) != NULL){
			if(sscanf(line, "%d %17s", &port, macaddr) == 2 && !strcmp(mac, macaddr)){
				fclose(fp);
				unlink(BRCTL_TABLE_PATH);
				return 1;
			}
		}
		fclose(fp);
		unlink(BRCTL_TABLE_PATH);
	}
	return 0;
}

int check_wrieless_info(P_CLIENT_DETAIL_INFO_TABLE p_client_detail_info_tab, const int i, const int is_file, struct json_object *clients)
{
	int ret = 1, wireless, sdn_idx = 0, lock;
	char macaddr[18], papMac[18];
	P_CLIENT_DETAIL_INFO_TABLE tab = p_client_detail_info_tab;
	if(is_file){
    	if(!check_if_file_exist("/tmp/allwclientlist.json"))
			return 0;
		lock = file_lock("allwevent");
		clients = json_object_from_file("/tmp/allwclientlist.json");
		file_unlock(lock);
	}
	snprintf(macaddr, sizeof(macaddr), "%02X:%02X:%02X:%02X:%02X:%02X", tab->mac_addr[i][0], tab->mac_addr[i][1],
		tab->mac_addr[i][2], tab->mac_addr[i][3], tab->mac_addr[i][4], tab->mac_addr[i][5]);
	if(!clients)
		return 0;
	json_object_object_foreach(clients, key, val){
		snprintf(papMac, sizeof(papMac), "%s", key);
		if(strcmp(key, macaddr)){
			json_object_object_foreach(val, key2, val2){
				if(strstr(key2, "2G"))
					wireless = 1;
				else if(strstr(key2, "5G1"))
					wireless = 3;
				else if(strstr(key2, "5G"))
					wireless = 2;
				else if(strstr(key2, "6G1"))
					wireless = 5;
				else if(strstr(key2, "6G"))
					wireless = 4;
				else
					wireless = 0;
				json_object_object_foreach(val2, key3, val3){
					if(!strcmp(key3, macaddr)){
						struct json_object *o_ts;
						json_object_object_get_ex(val3, "ts", &o_ts);
						if(o_ts){
							int ts = json_object_get_int(o_ts);
							if(tab->conn_ts[i] != ts)
								client_updated = 1;
							if(ts > 0){
								tab->conn_ts[i] = ts;
								tab->wireless[i] = wireless;
								tab->is_wireless[i] = wireless;
							}
						}
#ifdef RTCONFIG_MULTILAN_CFG
						if(nvram_get_int("re_mode") == 0){
							int subnet_idx;
							struct json_object *o_sdn_idx, *o_ifname;
							json_object_object_get_ex(val3, "sdn_idx", &o_sdn_idx);
							json_object_object_get_ex(val3, "ifname", &o_ifname);
							sdn_idx = 0;
							if(o_sdn_idx && o_ifname){
								sdn_idx = json_object_get_int(o_sdn_idx);
								if(sdn_idx == 0)
									sdn_idx = get_sdn_idx_form_apg(papMac, (char *)json_object_get_string(o_ifname));
							}
							tab->sdn_idx[i] = sdn_idx;
							subnet_idx = get_sdn_type(sdn_idx, tab->sdn_type[i], sizeof(tab->sdn_type[i]));
							NMP_DEBUG("sdn_idx = %d, subnet_idx = %d, sdn_type = %s, papMac = %s, mac = %s, ifname = %s\n", tab->sdn_idx[i], subnet_idx, tab->sdn_type[i], papMac, macaddr, json_object_get_string(o_ifname));
							ret = 0;
						}
#endif
					}
				}
			}
		}
	}
	if(is_file)
		json_object_put(clients);
	return ret;
}

void check_brctl_mac_online(CLIENT_DETAIL_INFO_TABLE *p_client_detail_info_tab)
{
	char macaddr[18] = {0};
	CLIENT_DETAIL_INFO_TABLE *tab = p_client_detail_info_tab;
	snprintf(macaddr, sizeof(macaddr), "%02X:%02X:%02X:%02X:%02X:%02X", tab->mac_addr[tab->detail_info_num][0], tab->mac_addr[tab->detail_info_num][1], tab->mac_addr[tab->detail_info_num][2], tab->mac_addr[tab->detail_info_num][3], tab->mac_addr[tab->detail_info_num][4], tab->mac_addr[tab->detail_info_num][5]);
	if(get_brctl_macs(macaddr)){
		NMP_DEBUG("check_brctl_mac_online,  mac = %s, online = 1\n", macaddr);
		tab->online[tab->detail_info_num] = 1;
		tab->device_flag[tab->ip_mac_num] |= (1<<FLAG_EXIST);
	}else{
		NMP_DEBUG("check_brctl_mac_online,  mac = %s, online = 0\n", macaddr);
		tab->online[tab->detail_info_num] = 0;
		tab->device_flag[tab->ip_mac_num] &= 0xF7;
	}
}

void network_ip_scan()
{
	int n, i, j;
	uint32_t ip;
	char command[128], ipaddr[16], *netmask;
	struct in_addr addr, ip_addr;
	netmask = nvram_get("lan_netmask");
	snprintf(ipaddr, sizeof(ipaddr), "%hhu.%hhu.%hhu.%d", my_ipaddr[0], my_ipaddr[1], my_ipaddr[2], my_ipaddr[3]);
	if(!netmask)
		netmask = nvram_default_get("lan_netmask");
	if(!inet_aton(ipaddr, &ip_addr)){
		NMP_DEBUG("ipaddr  = NULL \n");
		return;
	}
	if(!inet_aton(netmask, &addr)){
		NMP_DEBUG("lan_netmask  = NULL \n");
		return;
	}
	NMP_DEBUG("lan_netmask = %s\n", netmask);
	if(!strcmp(netmask, "255.255.255.0"))
		n = 1;
	else if(strcmp(netmask, "255.255.254.0"))
		n = 2;
	else if(strcmp(netmask, "255.255.252.0"))
		n = 4;
	else
		n = 1;
	ip = htonl(ntohl(ip_addr.s_addr) & ntohl(addr.s_addr));
	NMP_DEBUG("ipaddr = %hhu.%hhu.%hhu.%hhu, lan_netmask = %s\n", (ip>>0) & 0xff, (ip>>8) & 0xff,
		(ip>>16) & 0xff, (ip>>24) & 0xff, netmask);
	for(i = ((ip>>16) & 0xff); i < n + ((ip>>16) & 0xff); i++){
		for(j = 1; j < 255; j++){
			snprintf(ipaddr, sizeof(ipaddr), "%hhu.%hhu.%hhu.%d", (ip>>0) & 0xff, (ip>>8) & 0xff, i, j);
			snprintf(command, sizeof(command), "ping -c1 -w1 -s32 %s >/dev/null &", ipaddr);
			NMP_DEBUG("ping cmd = %s\n", command);
			system(command);
		}
	}
#if 0//def RTCONFIG_MULTILAN_CFG
	inet_aton("255.255.255.0", &addr);
	n = 1;
	for(i = 0; i < MTLAN_MAXINUM; i++){
		if(((mtlan_map >> i) & 0x1) == 1){
			ip = htonl(ntohl(mtlan_ipaddr[i].sin_addr.s_addr) & ntohl(addr.s_addr));
			NMP_DEBUG("ipaddr = %hhu.%hhu.%hhu.%hhu, lan_netmask = %s\n", (ip>>0) & 0xff, (ip>>8) & 0xff,
				(ip>>16) & 0xff, (ip>>24) & 0xff, "255.255.255.0");
			for(j = 1; j < 255; j++){
				snprintf(ipaddr, sizeof(ipaddr), "%hhu.%hhu.%hhu.%d", (ip>>0) & 0xff, (ip>>8) & 0xff,
					(ip>>16) & 0xff, j);
				snprintf(command, sizeof(command), "ping -c1 -w1 -s32 %s >/dev/null &", ipaddr);
				NMP_DEBUG("ping cmd = %s\n", command);
				system(command);
			}
		}
	}
#endif
}

int json_checker(const char *json_str)
{
	int i, next_char;
	size_t len;
    JSON_checker jc = new_JSON_checker(20);
	len = strlen(json_str);
	if(len > 0){
		for(i = 0; i < len; i++){
			next_char = *(json_str + i);
			if(!JSON_checker_char(jc, next_char)){
				NMP_DEBUG("JSON_checker_end: syntax error\n");
				return -1;
			}
		}
	}
	JSON_checker_done(jc);
		return 0;
	NMP_DEBUG("JSON_checker_end: syntax error\n");
	return -1;
}

int check_brctl_macs(CLIENT_DETAIL_INFO_TABLE *p_client_detail_info_tab)
{
	FILE *fp;
	int count = 0, online = 0, offline = 0, port = -1, i;
	char tmp[5000], macaddr[18], command[64], br_names[64], br_name[8], line[128], *next;
	char local[8] = {0}, ipaddr[16];
	CLIENT_DETAIL_INFO_TABLE *tab = p_client_detail_info_tab;
	memset(tmp, 0, sizeof(tmp));
	memset(macaddr, 0, sizeof(macaddr));
	memset(command, 0, sizeof(command));
	memset(br_names, 0, sizeof(br_names));
	snprintf(command, sizeof(command), "brctl show | grep br > %s", BRCTL_TABLE_PATH);
	system(command);
	fp = fopen(BRCTL_TABLE_PATH, "r");
	if(fp){
		fgets(line, sizeof(line), fp);//skip first line
		while(fgets(line, sizeof(line), fp) != NULL){
			if(sscanf(line, "%4s", br_name) == 1){
				strlcat(br_names, br_name, sizeof(br_names));
				strlcat(br_names, " ", sizeof(br_names));
			}
		}
		fclose(fp);
		unlink(BRCTL_TABLE_PATH);
	}
	NMP_DEBUG("check_brctl_macs, br_names = %s\n", br_names);
	foreach(br_name, br_names, next){
		snprintf(command, sizeof(command), "brctl showmacs %s | grep no > %s", br_name, BRCTL_TABLE_PATH);
		NMP_DEBUG("check_brctl_macs, cmd = %s\n", command);
		system(command);
		fp = fopen(BRCTL_TABLE_PATH, "r");
		if(fp){
			fgets(line, sizeof(line), fp);//skip first line
			count++;
			while(fgets(line, sizeof(line), fp) != NULL){
				count++;
				if(count <= 255){
					if(sscanf(line, "%d %17s", &port, macaddr) == 2 && !strstr(tmp, macaddr)){
						strlcat(tmp, macaddr, sizeof(tmp));
						strlcat(tmp, " ", sizeof(tmp));
					}
				}
			}
			fclose(fp);
			unlink(BRCTL_TABLE_PATH);
		}
	}
	NMP_DEBUG("ip_mac_num = %d, bridge mac count = %d, br mac_str = %s\n", tab->ip_mac_num, count, tmp);
	if(tab->ip_mac_num > 0){
		for(i = 0; i < tab->ip_mac_num; i++){
			snprintf(ipaddr, sizeof(ipaddr), "%d.%d.%d.%d", tab->ip_addr[i][0], tab->ip_addr[i][1], tab->ip_addr[i][2], tab->ip_addr[i][3]);
			snprintf(macaddr, sizeof(macaddr), "%02x:%02x:%02x:%02x:%02x:%02x", tab->mac_addr[i][0], tab->mac_addr[i][1], tab->mac_addr[i][2], tab->mac_addr[i][3], tab->mac_addr[i][4], tab->mac_addr[i][5]);
			if(strstr(tmp, macaddr)){
				online++;
				tab->online[i] = 1;
				tab->device_flag[i] |= 1<<FLAG_EXIST;
				NMP_DEBUG("ip/mac[%d] = [%s / %s],  online\n", i, ipaddr, macaddr);
			}else{
				offline++;
				tab->online[i] = 0;
				tab->device_flag[i] &= 0xF7;
				NMP_DEBUG("ip/mac[%d] = [%s / %s],  offline\n", i, ipaddr, macaddr);
			}
		}
	}
	NMP_DEBUG("macs online count = [%d], offline count = [%d]\n", online, offline);
	return 0;
}

void regularly_check_devices(P_CLIENT_DETAIL_INFO_TABLE p_client_detail_info_tab)
{
	int i;
	char ipaddr[16], macaddr[18];
	P_CLIENT_DETAIL_INFO_TABLE tab = p_client_detail_info_tab;
	networkmap_lock = file_lock("networkmap");
	if(tab->ip_mac_num > 0){
		for(i = 0; i < tab->ip_mac_num; i++){
			snprintf(ipaddr, sizeof(ipaddr), "%d.%d.%d.%d", tab->ip_addr[i][0], tab->ip_addr[i][1], tab->ip_addr[i][2], tab->ip_addr[i][3]);
			snprintf(macaddr, sizeof(macaddr), "%02x:%02x:%02x:%02x:%02x:%02x", tab->mac_addr[i][0], tab->mac_addr[i][1], tab->mac_addr[i][2], tab->mac_addr[i][3], tab->mac_addr[i][4], tab->mac_addr[i][5]);
			NMP_DEBUG("regularly_check_devices, No.%d >> ip = %s, mac = %s, wireless = %d, is_wireless = %d, online = %d, conn_ts = %d, offline_time = %d\n", i, ipaddr, macaddr, tab->wireless[i], tab->is_wireless[i], tab->online[i], tab->conn_ts[i], tab->offline_time[i]);
			if(get_brctl_macs(macaddr)){
				tab->online[i] = 1;
				tab->offline_time[i] = 0;
				NMP_DEBUG("get brctl table mac,  mac = %s, online = 1, offline_time = %d\n", macaddr, tab->offline_time[i]);
			}else{
				NMP_DEBUG("brctl table not mac, mac = %s, offline_time = %d\n", macaddr, tab->offline_time[i]);
				tab->online[i] = 0;
				tab->offline_time[i] += check_clients_interval;
			}
		}
	}
	file_unlock(networkmap_lock);
}

void check_clientlist_offline(CLIENT_DETAIL_INFO_TABLE *p_client_detail_info_tab)
{
#ifdef RTCONFIG_MULTILAN_CFG
	if(mtlan_updated){
		mtlan_updated = 0;
		if(p_client_detail_info_tab->detail_info_num == p_client_detail_info_tab->ip_mac_num)
			check_brctl_macs(p_client_detail_info_tab);
	}else{
		time_t now = time(NULL);
		if((now - clients_start_ts) > check_clients_interval){
			NMP_DEBUG("clients_end_ts(%d) - clients_start_ts(%d) = %d > check_clients_interval(%d)\n", now, clients_start_ts, now - clients_start_ts, check_clients_interval);
			mtlan_updated = 1;
			clients_start_ts = time(NULL);
		}
	}
#endif
}

int check_wireless_clientlist(CLIENT_DETAIL_INFO_TABLE *p_client_detail_info_tab)
{
	struct json_object *clientlist;
	CLIENT_DETAIL_INFO_TABLE *tab = p_client_detail_info_tab;
	NMP_DEBUG("check_wireless_clientlist, web server trigger wireless list offline check\n");
	networkmap_lock = file_lock("allwevent");
	clientlist = json_object_from_file("/tmp/allwclientlist.json");
	file_unlock(networkmap_lock);
	networkmap_lock = file_lock("networkmap");
	if(clientlist){
		int i;
		char ipaddr[16], macaddr[18];
		for(i = 0; i < tab->ip_mac_num; i++){
			snprintf(ipaddr, sizeof(ipaddr), "%d.%d.%d.%d", tab->ip_addr[i][0], tab->ip_addr[i][1], tab->ip_addr[i][2], tab->ip_addr[i][3]);
			snprintf(macaddr, sizeof(macaddr), "%02X:%02X:%02X:%02X:%02X:%02X", tab->mac_addr[i][0], tab->mac_addr[i][1], tab->mac_addr[i][2], tab->mac_addr[i][3], tab->mac_addr[i][4], tab->mac_addr[i][5]);
			NMP_DEBUG("check_wireless_clientlist, No.%d >> ip = %s, mac = %s, wireless = %d, is_wireless = %d, online = %d, conn_ts = %d\n", i, ipaddr, macaddr, tab->wireless[i], tab->is_wireless[i], tab->online[i], tab->conn_ts[i]);
			if(!check_wrieless_info(tab, i, 0, clientlist)){
				if(tab->wireless[i])
					tab->online[i] = 0;
#ifdef RTCONFIG_MULTILAN_CFG
				else if(!nvram_get_int("re_mode"))
					check_wrie_client_sdn_idx(tab, i);
#endif
			}else{
				tab->online[i] = 1;
				tab->offline_time[i] = 0;
				if((nvram_get_int("sw_mode") == 3 || nvram_get_int("sw_mode") == 1) && !nvram_get_int("re_mode"))
					rc_diag_stainfo(tab, i);
			}
			NMP_DEBUG("after, online = %d\n", tab->online[i]);
		}
		json_object_put(clientlist);
	}
	nmp_wl_offline_check(tab, 1);
	file_unlock(networkmap_lock);
	return 0;
}

int query_stainfo(char *sta_mac,char **buf)
{
	return 0;
}

void free_stainfo(char **buf)
{
	char *p;
	if(buf){
		p = *buf;
		if(p)
			free(p);
	}
}

void rc_diag_stainfo(P_CLIENT_DETAIL_INFO_TABLE p_client_detail_info_tab, int i)
{
	int n;
	char macaddr[18], *stainfo_buf = NULL;
	CLIENT_DETAIL_INFO_TABLE *tab = p_client_detail_info_tab;
	struct json_object *root = NULL, *o_stainfo, *o_macaddr, *o_tx, *o_rx, *o_conn_time;
	snprintf(macaddr, sizeof(macaddr), "%02X:%02X:%02X:%02X:%02X:%02X", tab->mac_addr[i][0], tab->mac_addr[i][1], tab->mac_addr[i][2], tab->mac_addr[i][3], tab->mac_addr[i][4], tab->mac_addr[i][5]);
	if(query_stainfo(macaddr, &stainfo_buf) > 0){
		NMP_DEBUG("stainfo_buf : %s\n", stainfo_buf);
		root = json_tokener_parse(stainfo_buf);
		if(root){
			json_object_object_get_ex(root, "stainfo", &o_stainfo);
			if(o_stainfo){
				json_object_object_get_ex(o_stainfo, macaddr, &o_macaddr);
				if(o_macaddr){
					json_object_object_get_ex(o_macaddr, "tx_rate", &o_tx);
					json_object_object_get_ex(o_macaddr, "rx_rate", &o_rx);
					json_object_object_get_ex(o_macaddr, "conn_time", &o_conn_time);
					if(o_tx)
						strlcpy(p_client_detail_info_tab->txrate[i], (char *)json_object_get_string(o_tx), sizeof(p_client_detail_info_tab->txrate[0]));
					if(o_rx)
						strlcpy(p_client_detail_info_tab->rxrate[i], (char *)json_object_get_string(o_rx), sizeof(p_client_detail_info_tab->rxrate[0]));
					if(o_conn_time){
						n = json_object_get_int(o_conn_time);
						snprintf(p_client_detail_info_tab->conn_time[i], sizeof(p_client_detail_info_tab->conn_time[0]), "%02d:%02d:%02d", n / 3600, (n % 3600) / 60, n % 3600 % 60);
					}
				}
			}
			json_object_put(root);
		}
		free_stainfo(&stainfo_buf);
	}
}

#ifdef RTCONFIG_MULTILAN_CFG
void get_subnet_ifname(const int subnet_idx, char * subnet_ifname, int ifname_len)
{
	int i = 0, found = 0;
	char *nv, *nvp, *b, *idx;
	nvp = nv = strdup(nvram_safe_get("subnet_rl"));
	if(nv){
		if(*nv){
			while ((b = strsep(&nvp, "<")) != NULL) {
				while ((idx = strsep(&b, ">")) != NULL && i < 9) {
					if(i == 0 && *idx){
						if(atoi(idx) == subnet_idx)
							found = 1;
						else
							break;
					}else if(i == 1 && *idx && found){
						snprintf(subnet_ifname, ifname_len, "%s", idx);
						free(nv);
						return;
					}
				}
			}
		}
		free(nv);
		snprintf(subnet_ifname, ifname_len, "%s", "");
	}
}

void get_ip_from_arp_table(P_CLIENT_DETAIL_INFO_TABLE p_client_detail_info_tab, const int i, const char *subnet)
{
	FILE *fp;
	int arp_count = 0, lock;
	char ipaddr_tab[18], macaddr_tab[32];
	char ipaddr[18], macaddr[18], flag[4], dev[8], line[300];
	unsigned char ip_binary[4] = {0};
	unsigned char *pIP = ip_binary;
	P_CLIENT_DETAIL_INFO_TABLE tab = p_client_detail_info_tab;
	memset(ipaddr_tab, 0x0, sizeof(ipaddr_tab));
	memset(macaddr_tab, 0x0, sizeof(macaddr_tab));
	snprintf(ipaddr_tab, sizeof(ipaddr_tab), "%d.%d.%d.%d", tab->ip_addr[i][0], tab->ip_addr[i][1], tab->ip_addr[i][2], tab->ip_addr[i][3]);
	snprintf(macaddr_tab, sizeof(macaddr_tab), "%02x:%02x:%02x:%02x:%02x:%02x", tab->mac_addr[i][0], tab->mac_addr[i][1],
		tab->mac_addr[i][2], tab->mac_addr[i][3], tab->mac_addr[i][4], tab->mac_addr[i][5]);
	NMP_DEBUG("%s >> ipaddr =  %s, mac_addr : %s\n", __func__, ipaddr_tab, macaddr_tab);
	fp = fopen(ARP_PATH, "r");
	if(fp){
		while(fgets(line, sizeof(line), fp) != NULL){
			sscanf(line, "%s%*s%s%s%*s%s\n", ipaddr, flag, macaddr, dev);
			arp_count++;
			if(strcmp(macaddr, "00:00:00:00:00:00")){
				NMP_DEBUG("arp check arp_count = %d : ipBuff = %s, flags = %s, macBuff = %s, lanIf = %s, subnet = %s\n", __func__, arp_count, ipaddr, flag, macaddr, dev, subnet);
				if(!strcmp(dev, subnet) && !strcmp(macaddr, macaddr_tab)){
					NMP_DEBUG("match arp connect MAC:%s,  IP:%s\n", macaddr, ipaddr);
					sscanf(ipaddr, "%hhu.%hhu.%hhu.%hhu", &pIP[0], &pIP[1], &pIP[2], &pIP[3]);
					lock = file_lock("networkmap");
					tab->ip_addr[i][0] = ip_binary[0];
					tab->ip_addr[i][1] = ip_binary[1];
					tab->ip_addr[i][2] = ip_binary[2];
					tab->ip_addr[i][3] = ip_binary[3];
					file_unlock(lock);
					break;
				}
			}
		}
		fclose(fp);
	}
}

int get_sdn_type(const int sdn_idx, char *sdn_type, int sdn_type_len)
{
	int i = 0, found = 0;
	char *nv, *nvp, *b, *idx;
	nvp = nv = strdup(nvram_safe_get("sdn_rl"));
	if(nv){
		if(*nv){
			while ((b = strsep(&nvp, "<")) != NULL) {
				found = 0;
				while ((idx = strsep(&b, ">")) != NULL && i < SDN_LIST_BASIC_PARAM) {
					if(i == 0 && *idx){// sdn_idx is the 1th field.
						if(atoi(idx) == sdn_idx)
							found = 1;
						else
							break;
					}else if(i == 1 && *idx && found)// sdn_name is the 2th field.
						snprintf(sdn_type, sdn_type_len, "%s", idx);
					else if(i == 4 && *idx && found){// subnet_idx is the 5th field.
						free(nv);
						return atoi(idx);
					}
				}
				if(found)//bug, not found sdn_name/subnet_idx
					break;
			}
		}
		free(nv);
	}
	snprintf(sdn_type, sdn_type_len, "%s", "null");
	return 0;
}

int get_sdn_idx_form_apg(char *papMac, char *ifname)
{
	int len, len2, i, j;
	struct json_object *root, *o_vif_used, *o_mac, *o_mac_content, *o_sdn_idx, *o_sdn_vid, *o_sdn_band;
	struct json_object *o_band_content, *o_band_idx, *o_prefix, *o_ifname;
	if(!check_if_file_exist(APG_IFNAMES_USED_FILE))
		return 0;
	root = json_object_from_file(APG_IFNAMES_USED_FILE);
	if(root){
		json_object_object_get_ex(root, "vif_used", &o_vif_used);
		if(o_vif_used){
			json_object_object_get_ex(o_vif_used, papMac, &o_mac);
			if(o_mac){
				len = json_object_array_length(o_mac);
				if(len > 0){
					for(i = 0; i < len; i++){
						o_mac_content = json_object_array_get_idx(o_mac, i);
						json_object_object_get_ex(o_mac_content, "sdn_idx", &o_sdn_idx);
						json_object_object_get_ex(o_mac_content, "sdn_vid", &o_sdn_vid);
						json_object_object_get_ex(o_mac_content, "sdn_band", &o_sdn_band);
						len2 = json_object_array_length(o_sdn_band);
						if(len > 0){
							for(j = 0; j < len2; j++){
								o_band_content = json_object_array_get_idx(o_sdn_band, j);
								json_object_object_get_ex(o_band_content, "band_idx", &o_band_idx);
								json_object_object_get_ex(o_band_content, "wl_prefix", &o_prefix);
								json_object_object_get_ex(o_band_content, "wl_ifname", &o_ifname);
								if(!strcmp(ifname, json_object_get_string(o_ifname))){
									int ret = atoi(json_object_get_string(o_sdn_idx));
									json_object_put(root);
									return ret;
								}
							}
						}
					}
				}
			}
		}
		json_object_put(root);
		return 0;
	}
	return 0;
}

int check_wrie_client_sdn_idx(P_CLIENT_DETAIL_INFO_TABLE p_client_detail_info_tab, const int i)
{
	int lock, ret = 0;
	char macaddr[18];
	struct json_object *root;
	P_CLIENT_DETAIL_INFO_TABLE tab = p_client_detail_info_tab;
	if(!check_if_file_exist(WIRED_CLIENT_LIST_JSON_PATH))
		return ret;
	lock = file_lock(WIREDCLIENTLIST_FILE_LOCK);
	root = json_object_from_file(WIRED_CLIENT_LIST_JSON_PATH);
	file_unlock(lock);
	snprintf(macaddr, sizeof(macaddr), "%02X:%02X:%02X:%02X:%02X:%02X", tab->mac_addr[i][0], tab->mac_addr[i][1],
		tab->mac_addr[i][2], tab->mac_addr[i][3], tab->mac_addr[i][4], tab->mac_addr[i][5]);
	if(!root)
		return ret;
	json_object_object_foreach(root, key, val){
		json_object_object_foreach(val, key2, val2){
			if(!strcmp(key2, macaddr)){
				NMP_DEBUG("key = mac = %s, i = %d\n", key2, i);
				ret = 1;
				if(nvram_get_int("re_mode") == 0){
					int subnet_idx, sdn_idx = 0;
					char br_name[7];
					struct json_object *o_sdn_idx;
					json_object_object_get_ex(val2, "sdn_idx", &o_sdn_idx);
					if(o_sdn_idx)
						sdn_idx = json_object_get_int(o_sdn_idx);
					tab->sdn_idx[i] = sdn_idx;
					tab->wireless[i] = 0;
					tab->is_wireless[i] = 0;
					tab->online[i] = 1;
					subnet_idx = get_sdn_type(sdn_idx, tab->sdn_type[i], sizeof(tab->sdn_type[i]));
					NMP_DEBUG("sdn_idx = %d, subnet_idx = %d, sdn_type = %s, mac = %s\n", sdn_idx, subnet_idx, tab->sdn_type[i], macaddr);
					if(subnet_idx > 0){
						get_subnet_ifname(subnet_idx, br_name, 7);
						NMP_DEBUG("subnet_ifname = %s\n", br_name);
						if(strstr(br_name, "br"))
							get_ip_from_arp_table(tab, i, br_name);
					}
				}
			}
		}
	}
	json_object_put(root);
	return ret;
}

#endif
