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
 * Copyright 2018-2024, SWRTdev.
 * Copyright 2018-2024, paldier <paldier@hotmail.com>.
 * All Rights Reserved.
 * 
 */
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <limits.h>
#include <dirent.h>
#include <sys/types.h>
#include <typedefs.h>
#include <bcmutils.h>
#include <httpd.h>
#include <swrt.h>
#include <shared.h>
#if defined(RTCONFIG_MULTILAN_CFG)
#include <amas_apg_shared.h>
#endif
#if defined(RTCONFIG_SOFTCENTER)
#include <dbapi.h>
#endif

#ifdef RTCONFIG_ODMPID
extern void replace_productid(char *GET_PID_STR, char *RP_PID_STR, int len);
#endif

//unlock all languages for cn
int get_lang_num_swrt()
{
	return 9999;
}

int check_lang_support_swrt(char *lang)
{
	if(strstr("BR CN CZ DE EN ES FR HU IT JP KR MS NL PL RU RO SL TH TR TW UK", lang))
		return 1;
	return 0;
}
/*
int change_preferred_lang_swrt(int finish)
{
	return 1;
}
*/

#if defined(RTCONFIG_SOFTCENTER)
extern int do_ssl;
extern int ssl_stream_fd;
extern char post_buf[65535];
extern char post_buf_backup[65535];
extern char post_json_buf[65535];
extern char SystemCmd[128];
extern void sys_script(char *name);
extern void substr(char *dest, const char* src, unsigned int start, unsigned int cnt);
extern void do_html_get(char *url, int len, char *boundary);

static char db_buf[512];
int ej_dbus_get(int eid, webs_t wp, int argc, char_t **argv)
{
	char *name;
	int ret = 0;

	if (ejArgs(argc, argv, "%s", &name) < 1) {
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}
	memset(db_buf, 0, sizeof(db_buf));
	dbclient client;
	dbclient_start(&client);
	ret = dbclient_get(&client, name, db_buf, sizeof(db_buf));
	dbclient_end(&client);
	if(ret == 0)
		ret += websWrite(wp, "%s", db_buf);
	else
		ret = 0;

	return ret;
}

int ej_dbus_get_def(int eid, webs_t wp, int argc, char_t **argv)
{
	char *name, *output;
	int ret = 0;

	if (ejArgs(argc, argv, "%s %s", &name, &output) < 2) {
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}
	memset(db_buf, 0, sizeof(db_buf));
	dbclient client;
	dbclient_start(&client);
	ret = dbclient_get(&client, name, db_buf, sizeof(db_buf));
	dbclient_end(&client);
	if(ret == 0)
		ret += websWrite(wp, "%s", db_buf);
	else
		ret = websWrite(wp, "%s", output);
	return ret;
}

int ej_dbus_match(int eid, webs_t wp, int argc, char_t **argv)
{
	char *name, *match, *output;
	int ret = 0;

	if (ejArgs(argc, argv, "%s %s %s", &name, &match, &output) < 3) {
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}
	memset(db_buf, 0, sizeof(db_buf));
	dbclient client;
	dbclient_start(&client);
	ret = dbclient_get(&client, name, db_buf, sizeof(db_buf));
	dbclient_end(&client);
	if(ret == 0 && strcmp(db_buf,match) == 0)
		return websWrite(wp, "%s", output);
	return 0;
}

static int swrtupload = 0;
void do_ssupload_post(char *url, FILE *stream, int len, char *boundary)
{
	do_html_get(url, len, boundary);
	char *name = websGetVar(wp, "a","");
	char upload_fifo[64];
	memset(upload_fifo, 0, 64);
	strlcpy(upload_fifo, name, sizeof(upload_fifo));
	FILE *fifo = NULL;
	char buf[4096];
	int ch, ret = EINVAL;
	int count, cnt;
	long filelen;
	int offset;

	swrtupload = 0;
	/* Look for our part */
	while (len > 0)
	{
		if (!fgets(buf, MIN(len + 1, sizeof(buf)), stream))
		{
			goto err;
		}

		len -= strlen(buf);

		if (!strncasecmp(buf, "Content-Disposition:", 20) && strstr(buf, "name=\"file\""))
			break;
	}

	/* Skip boundary and headers */
	while (len > 0) {
		if (!fgets(buf, MIN(len + 1, sizeof(buf)), stream))
		{
			goto err;
		}
		len -= strlen(buf);
		if (!strcmp(buf, "\n") || !strcmp(buf, "\r\n"))
		{
			break;
		}
	}


	if (!(fifo = fopen(upload_fifo, "a+"))) goto err;
	filelen = len;
	cnt = 0;
	offset = 0;

	/* Pipe the rest to the FIFO */
	while (len>0 && filelen>0)
	{

#ifdef RTCONFIG_HTTPS
		if(do_ssl){
			if (waitfor(ssl_stream_fd, (len >= 0x4000)? 3 : 1) <= 0)
				break;
		}
		else{
			if (waitfor (fileno(stream), 10) <= 0)
			{
				break;
			}
		}
#else
		if (waitfor (fileno(stream), 10) <= 0)
		{
			break;
		}
#endif

		count = fread(buf + offset, 1, MIN(len, sizeof(buf)-offset), stream);

		if(count <= 0)
			goto err;

		len -= count;

		if(cnt==0) {
			if(count + offset < 8)
			{
				offset += count;
				continue;
			}

			count += offset;
			offset = 0;
			_dprintf("read from stream: %d\n", count);
			cnt++;
		}
		filelen-=count;
		fwrite(buf, 1, count, fifo);
	}

	/* Slurp anything remaining in the request */
	while (len-- > 0)
	{
		if((ch = fgetc(stream)) == EOF)
			break;

		if (filelen>0)
		{
			fputc(ch, fifo);
			filelen--;
		}
	}
	fclose(fifo);
	fifo = NULL;
	swrtupload = 1;
err:
	if (fifo)
		fclose(fifo);

	/* Slurp anything remaining in the request */
	while (len-- > 0)
		if((ch = fgetc(stream)) == EOF)
			break;
}

void do_ssupload_cgi(char *url, FILE *stream)
{
	int i;
	for (i=0; i<10; i++)
	{
		if(swrtupload == 1)
		{
			websWrite(stream,"<script>parent.upload_ok(1);</script>\n" );
			break;
		} else
			sleep(1);
	}
	if (i == 10)
		websWrite(stream,"<script>parent.upload_ok(0);</script>\n" );
}

void do_dbupload_post(char *url, FILE *stream, int len, char *boundary)
{
	char upload_fifo[64] = {0};
	FILE *fifo = NULL;
	int ch;
	int count;
	long filelen;
	char org_file_name[64] = {0};
	char file_name[64] = {0};
	int boundary_len = ((boundary != NULL) ? strlen(boundary) : 0);
	memset(post_buf, 0, sizeof(post_buf));
	swrtupload = 0;
	/* Look for our part */
	while (len > 0)
	{
		if (!fgets(post_buf, MIN(len + 1, sizeof(post_buf)), stream))
		{
			if(errno == EINTR)
				continue;
			goto err;
		}
		len -= strlen(post_buf);

		if (!strncasecmp(post_buf, "Content-Disposition:", 20)){
			if(strstr(post_buf, "filename=")) {
				snprintf(org_file_name, sizeof(org_file_name), "%s", strstr(post_buf, "filename="));
				sscanf(org_file_name, "filename=\"%[^\"]\"", file_name);
				snprintf(upload_fifo, sizeof(upload_fifo), "/tmp/upload/%s", file_name);
				break;
			}
		}
	}
	/* Skip boundary and headers */
	while (len > 0) {
		if (!fgets(post_buf, MIN(len + 1, sizeof(post_buf)), stream))
		{
			if(errno == EINTR)
				continue;
			goto err;
		}
		len -= strlen(post_buf);
		if (!strcmp(post_buf, "\n") || !strcmp(post_buf, "\r\n"))
		{
			break;
		}
	}
	len = len - (2 + 2 + boundary_len + 4);
	if (!(fifo = fopen(upload_fifo, "wb"))) goto err;
	filelen = len;

	/* Pipe the rest to the FIFO */
	while (len>0 && filelen>0)
	{
		int ret;
#ifdef RTCONFIG_HTTPS
		if(do_ssl){
			ret = waitfor(ssl_stream_fd, (len >= 0x4000)? 3 : 1);
		}
		else
#endif
			ret = waitfor (fileno(stream), 10);

		if (ret <= 0)
		{
			if(ret < 0 && errno == EINTR)
				continue;
			_dprintf("waitfor() fail errno: %d (%s)\n", errno, strerror(errno));
			break;
		}
		count = fread(post_buf, 1, MIN(len, sizeof(post_buf)), stream);

		if(count <= 0)
			goto err;

		len -= count;
		filelen -= count;
		fwrite(post_buf, 1, count, fifo);
	}
	len += len + (2 + 2 + boundary_len + 4);
	/* Slurp anything remaining in the request */
	while (len > 0)
	{
		len--;
		if((ch = fgetc(stream)) == EOF)
			break;

		if (filelen>0)
		{
			fputc(ch, fifo);
			filelen--;
		}
	}
	fclose(fifo);
	fifo = NULL;
	swrtupload = 1;
err:
	if (fifo)
		fclose(fifo);

	/* Slurp anything remaining in the request */
	while (len > 0) {
		len--;
		if((ch = fgetc(stream)) == EOF)
			break;
	}
}

void do_dbupload_cgi(char *url, FILE *stream)
{
	if(swrtupload == 1)
		websWrite(stream, "{\"status\":200}\n");
	else
		websWrite(stream, "{\"status\":0}\n");
}

static int applydb_cgi(webs_t wp, char_t *urlPrefix, char_t *webDir, int arg, char_t *url, char_t *path, char_t *query)
{
	char *action_mode = NULL;
	char *action_script = NULL;
	char dbjson[100][9999];
	char dbvar[2048];
	char dbval[9999];
	char notify_cmd[128];
	char db_cmd[128];
	int i, j;
	char *result = NULL;
	char *temp = NULL;
	char *name = websGetVar(wp, "p","");
	char *userm = websGetVar(wp, "use_rm", "");
	char scPath[128];
	char *post_db_buf = post_json_buf;

	action_mode = websGetVar(wp, "action_mode", "");
	action_script = websGetVar(wp, "action_script", "");
	
	dbclient client;
	dbclient_start(&client);
	if (strlen(name) <= 0) {
		printf("No \"name\"!\n");
	}
	if ( !strcmp("", post_db_buf)){
		//get
		snprintf(post_db_buf, sizeof(post_json_buf), "%s", post_buf_backup + 1);
		unescape(post_db_buf, sizeof(post_json_buf));
		//logmessage("HTTPD", "url: %s,%s", post_db_buf, name);
		strlcpy(post_json_buf, post_db_buf, sizeof(post_json_buf));
		result = strtok( post_json_buf, "&" );
		i =0;
	while( result != NULL )
	{
		if (result!=NULL)
		{
		strlcpy(dbjson[i], result, sizeof(dbjson[i]));
		i++;
			result = strtok( NULL, "&" );
		}
	}
	for (j =0; j < i; j++)
	{
		if(!strncasecmp(dbjson[j], name, strlen(name))){
				memset(dbvar,'\0',sizeof(dbvar));
				memset(dbval,'\0',sizeof(dbval));
				temp=strstr(dbjson[j], "=");
				strlcpy(dbval, temp+1, sizeof(dbval));
				strncpy(dbvar, dbjson[j], strlen(dbjson[j])-strlen(temp));
			//logmessage("HTTPD", "name: %s post: %s", dbvar, dbval);
			if(*userm || dbval[0]=='\0')
				dbclient_rm(&client, dbvar, strlen(dbvar));
			else
				dbclient_bulk(&client, "set", dbvar, strlen(dbvar), dbval, strlen(dbval));
		}
	}
	} else {
	//post
	unescape(post_db_buf, sizeof(post_json_buf));
	//logmessage("HTTPD", "name: %s post: %s", name, post_json_buf);
	//logmessage("HTTPD", "name: %s post: %s", name, post_db_buf);
	strlcpy(post_json_buf, post_db_buf, sizeof(post_json_buf));
	result = strtok( post_json_buf, "&" );
	i =0;
	while( result != NULL )
	{
		if (result!=NULL)
		{
		strlcpy(dbjson[i], result, sizeof(dbjson[i]));
		i++;
			result = strtok( NULL, "&" );
		}
	}
	for (j =0; j < i; j++)
	{
		if(!strncasecmp(dbjson[j], name, strlen(name))){
				memset(dbvar,'\0',sizeof(dbvar));
				memset(dbval,'\0',sizeof(dbval));
				temp=strstr(dbjson[j], "=");
				strlcpy(dbval, temp+1, sizeof(dbval));
				strncpy(dbvar, dbjson[j], strlen(dbjson[j])-strlen(temp));
			//logmessage("HTTPD", "dbvar: %s dbval: %s", dbvar, dbval);
			if(*userm || dbval[0]=='\0')
				dbclient_rm(&client, dbvar, strlen(dbvar));
			else
				dbclient_bulk(&client, "set", dbvar, strlen(dbvar), dbval, strlen(dbval));
		}
	}
	}
	dbclient_end(&client);
	memset(db_cmd,'\0',sizeof(db_cmd));
	if(!strcmp(action_mode, "toolscript") || !strcmp(action_mode, " Refresh ")){
		snprintf(scPath, sizeof(scPath), "/jffs/softcenter/scripts/");
		strncpy(notify_cmd, action_script, 128);
		strlcat(scPath, notify_cmd, sizeof(scPath));
		strlcpy(SystemCmd, scPath, sizeof(SystemCmd));
		sys_script("syscmd.sh");
	}
	else if(!strcmp(action_mode, "dummy_script") || !strcmp(action_mode, "dummy")){
		//dummy script
	}
	else if(*action_mode){
		snprintf(scPath, sizeof(scPath), "/jffs/softcenter/scripts/");
		strlcpy(notify_cmd, action_script, sizeof(notify_cmd));
		strlcat(scPath, notify_cmd, sizeof(scPath));
		snprintf(db_cmd, sizeof(db_cmd), " %s", action_mode);
		strlcat(scPath, db_cmd, sizeof(scPath));
		strlcpy(SystemCmd, scPath, sizeof(SystemCmd));
		sys_script("syscmd.sh");
	}
	websWrite(wp,"<script>parent.done_validating('', '');</script>\n" );
	return 0;
}

void do_applydb_cgi(char *url, FILE *stream)
{
    applydb_cgi(stream, NULL, NULL, 0, url, NULL, NULL);
}

static int db_print(dbclient* client, webs_t wp, char* prefix, char* key, char* value) {
	websWrite(wp,"o[\"%s\"]=\"%s\";\n", key, value);
	return 0;
}

void do_dbconf(char *url, FILE *stream)
{
	char *name = NULL;
	char * delim = ",";
	char *pattern = websGetVar(wp, "p","");
	char *dup_pattern = strdup(pattern);
	char *sepstr = dup_pattern;
	dbclient client;
	dbclient_start(&client);
	if(strstr(sepstr,delim)) {
		for(name = strsep(&sepstr, delim); name != NULL; name = strsep(&sepstr, delim)) {
			websWrite(stream,"var db_%s=(function() {\nvar o={};\n", name);

			dbclient_list(&client, name, stream, db_print);
			websWrite(stream,"return o;\n})();\n" );
		}
	} else {
		name= strdup(pattern);
		websWrite(stream,"var db_%s=(function() {\nvar o={};\n", name);
		dbclient_list(&client, name, stream, db_print);
		websWrite(stream,"return o;\n})();\n" );
	}
	free(dup_pattern);
	dbclient_end(&client);
}

void do_ss_status(char *url, FILE *stream)
{

	if(check_if_file_exist("/jffs/softcenter/ss/cru/china.sh"))
	{
		doSystem("sh /jffs/softcenter/ss/cru/china.sh &");
		doSystem("sh /jffs/softcenter/ss/cru/foreign.sh &");
	}
	else
	{
		nvram_set("ss_foreign_state", "USB Problem detected!");
		nvram_set("ss_china_state", "USB Problem detected!");
	}
	sleep(1);
	websWrite(stream,"[\"%s\", ", nvram_get("ss_foreign_state") );
	websWrite(stream,"\"%s\"]", nvram_get("ss_china_state") );
}

static int do_logread(webs_t wp, char_t *urlPrefix, char_t *webDir, int arg, char_t *url, char_t *path, char_t *query)
{
	char buf[4096];
	char logpath[128], scPath[128];
	FILE *fp = NULL;
	//char filename[100];
	//sscanf(url, "logreaddb.cgi?%s", filename);
	char *filename = websGetVar(wp, "p","");
	char *script = websGetVar(wp, "script", "");
	if(*script){
		snprintf(scPath, sizeof(scPath), "/jffs/softcenter/scripts/%s", script);
		strlcpy(SystemCmd, scPath, sizeof(SystemCmd));
		sys_script("syscmd.sh");
	}
	snprintf(logpath, sizeof(logpath), "/tmp/%s", filename);
	//logmessage("HTTPD", "logread: %s, url: %s", logpath, url);
	//sleep(1);//
	if(check_if_file_exist(logpath)){
		if((fp = fopen(logpath, "r"))!= NULL){
			while(fgets(buf, sizeof(buf),fp) != NULL){
				websWrite(wp,"%s\n", buf);
			}
		}
		fclose(fp);
		//doSystem("rm -rf %s", path);
	} else
		websWrite(wp,"not found log file\n");
	return 0;
}

void do_logread_cgi(char *url, FILE *stream)
{
    do_logread(stream, NULL, NULL, 0, url, NULL, NULL);
}
//1.5
static int db_print2(dbclient* client,  json_object *result, char* prefix, char* key, char* value) {
	json_object_object_add(result, key, json_object_new_string(value));
	return 0;
}

static int dbapi_isid(int id, char *result)
{
	char str[20] = {0};
	if(id > 0 && result){
		itoa(id, str, 10);
		if(!strcmp(str, result))
			return 1;
	}
	return 0;
}

static int dbapi_cgi(webs_t wp, char_t *urlPrefix, char_t *webDir, int arg, char_t *url, char_t *path, char_t *query)
{
	int i, id=0, count;
	char script[99], p[15], scPath[256];
	char *post_db_buf = post_json_buf;
	char *name = NULL;
	FILE *fp;
	char buf[4096];
	dbclient client;
	dbclient_start(&client);

	if ( !strcmp("", post_db_buf)){//get
		json_object *root = NULL;
		json_object *result = NULL;
		json_object *db = NULL;
		char dbname[100];
		sscanf(url, "_api/%s", dbname);
		char * delim = ",";
		char *dup_pattern = strdup(dbname);
		char *sepstr = dup_pattern;
		root = json_object_new_object();
		result = json_object_new_array();

		for(name = strsep(&sepstr, delim); name != NULL; name = strsep(&sepstr, delim)) {
			db = json_object_new_object();
			dbclient_list_json(&client, name, db, db_print2);
			json_object_array_add(result, db);
		}
		json_object_object_add(root, "result", result);
		websWrite(wp, "%s\n", json_object_to_json_string(root));
		free(dup_pattern);
		json_object_put(root);
		dbclient_end(&client);
	}else{//post
		json_object *root = NULL;
		json_object *idObj = NULL;
		json_object *methodObj = NULL;
		json_object *paramsObj = NULL;
		json_object *fieldsObj = NULL;
		json_object *arrayObj = NULL;
		root = json_tokener_parse(post_db_buf);
		//printf("%s\n",json_object_to_json_string(root));
		if (root) {
			json_object_object_get_ex(root,"id", &idObj);
			id = json_object_get_int(idObj);
			json_object_object_get_ex(root,"method", &methodObj);
			memset(script, 0, sizeof(script));
			snprintf(script, sizeof(script), "%s", json_object_get_string(methodObj));
			json_object_object_get_ex(root,"params", &paramsObj);
			count  = json_object_array_length(paramsObj);
			memset(p, 0, sizeof(p));
			for (i = 0; i < count; i++){
				arrayObj = json_object_array_get_idx(paramsObj, i);
				if (json_object_get_type(arrayObj) == json_type_int)
					snprintf(p, sizeof(p), "%d", json_object_get_int(arrayObj));
				else if (json_object_get_type(arrayObj) == json_type_string)
					snprintf(p, sizeof(p), "%s", json_object_get_string(arrayObj));
			}
			json_object_object_get_ex(root,"fields", &fieldsObj);
			if(json_object_get_type(fieldsObj)==json_type_object){
				json_object_object_foreach(fieldsObj, key, val){
					if(strlen(json_object_get_string(val)))
						dbclient_bulk(&client, "set", key, strlen((char *)key), json_object_get_string(val), strlen(json_object_get_string(val)));
					else
						dbclient_rm(&client, key, strlen(key));
				}
			}
			if(!check_if_dir_exist("/tmp/upload"))
				mkdir("/tmp/upload", 0755);
			memset(scPath, 0, sizeof(scPath));
			if(*p){
				snprintf(scPath, sizeof(scPath), "/jffs/softcenter/scripts/%s %d %s &", script, id, p);
			} else {
				snprintf(scPath, sizeof(scPath), "/jffs/softcenter/scripts/%s %d &", script, id);
			}
			doSystem(scPath);
			json_object_put(root);
			dbclient_end(&client);
			memset(scPath, 0, sizeof(scPath));
			snprintf(scPath, sizeof(scPath), "/tmp/upload/%d",id);
			for(i=0; i<10; i++){
				usleep(100000);//wait for script
				if(check_if_file_exist(scPath)){
					if((fp = fopen(scPath, "r"))!= NULL){
						while(fgets(buf, sizeof(buf),fp) != NULL){
							buf[strlen(buf)-1]=0;
							if(dbapi_isid(id, buf))
								websWrite(wp,"{\"result\":%d}\n", id);
							else
								websWrite(wp,"{\"result\":\"%s\"}\n", buf);
							unlink(scPath);
							return 0;
						}
					}
				}
			}
			websWrite(wp,"{\"result\":%d}\n", id);
		}
	}
	return 0;
}

void do_dbapi_cgi(char *url, FILE *stream)
{
	dbapi_cgi(stream, NULL, NULL, 0, url, NULL, NULL);
}

void do_dbtemp_cgi(char *url, FILE *stream)
{
	char dbname[100];
	char logpath[128];
	sscanf(url, "_temp/%s", dbname);
	memset(logpath, 0, sizeof(logpath));
	snprintf(logpath, sizeof(logpath), "/tmp/upload/%s", dbname);
	if(check_if_file_exist(logpath))
		do_file(logpath, stream);
}

void do_dbroot_cgi(char *url, FILE *stream)
{
	char dbname[100];
	char logpath[128];
	sscanf(url, "_root/%s", dbname);
	memset(logpath, 0, sizeof(logpath));
	snprintf(logpath, sizeof(logpath), "/jffs/softcenter/webs/%s", dbname);
	if(check_if_file_exist(logpath))
		do_file(logpath, stream);
}

void do_dbresp_cgi(char *url, FILE *stream)
{
	int resultid;
	char path[50];
	FILE *fp = NULL;
	sscanf(url, "_resp/%d", &resultid);
	snprintf(path, sizeof(path), "/tmp/upload/%d", resultid);

	if((fp = fopen(path, "w")) != NULL){
		fprintf(fp, "%s", post_json_buf);
      	fclose(fp);
	}
}

void do_result_cgi(char *url, FILE *stream)
{
	int resultid;
	char path[50],buf[2048];
	FILE *fp = NULL;
	sscanf(url, "_result/%d", &resultid);
	snprintf(path, sizeof(path), "/tmp/upload/%d",resultid);
	if(check_if_file_exist(path)){
		if((fp = fopen(path, "r"))!= NULL){
			while(fgets(buf, sizeof(buf),fp) != NULL){
				buf[strlen(buf)-1]=0;
				websWrite(stream,"{\"result\":\"%s\"}\n",buf);
				unlink(path);
				return ;
			}
		}
	}
	websWrite(stream,"{\"result\":%d}\n",resultid);
}

#endif

#if defined(RTCONFIG_ENTWARE)

#define ENTWARE_ACT_INSTALL		1
#define ENTWARE_ACT_UPDATE		2
#define ENTWARE_ACT_REMOVE		4
#define ENTWARE_ACT_START		8
#define ENTWARE_ACT_STOP		16
#define ENTWARE_ACT_ALL			(ENTWARE_ACT_INSTALL | ENTWARE_ACT_UPDATE | ENTWARE_ACT_REMOVE | ENTWARE_ACT_START | ENTWARE_ACT_STOP)
void do_entware_cgi(char *url, FILE *stream){
	struct json_object *root = NULL;
	char *app = NULL, *action = NULL, *type = NULL, *mount = NULL, *disk = NULL;
	char arg[128], buf[128];
	struct dirent **dl;
	FILE *fp;
	char *pwrite;
	int i, n, ent_act = 0;

	do_json_decode(&root);
	memset(arg, 0, sizeof(arg));
	
	app = get_cgi_json("entware_app", root);
	if (!app)
		app = "entware";
	if (!strcmp(app, "entware"))
	{
		type = get_cgi_json("entware_type", root);
		if (!type)
			type = "generic";
		snprintf(arg, sizeof(arg), "%s", type);
		mount = get_cgi_json("entware_mount", root);
		disk = get_cgi_json("entware_disk", root);
	}
	
	action = get_cgi_json("entware_action", root);
	if (!action)
		action = "list_start";
	if (!strcmp(action, "install"))
		ent_act = ENTWARE_ACT_INSTALL;
	else if (!strcmp(action, "update"))
		ent_act = ENTWARE_ACT_UPDATE;
	else if (!strcmp(action, "remove"))
		ent_act = ENTWARE_ACT_REMOVE;
	else if (!strcmp(action, "start"))
		ent_act = ENTWARE_ACT_START;
	else if (!strcmp(action, "stop"))
		ent_act = ENTWARE_ACT_STOP;
	
	if (!strcmp(app, "entware") && !strcmp(action, "list_start"))
	{
		if (nvram_get_int("entware_busy"))
			websWrite(stream, "{\"entware_is_install\":2,");//2 to do something
		else if (f_exists("/opt/etc/entware_release"))
			websWrite(stream, "{\"entware_is_install\":1,");//1 Installed
		else
			websWrite(stream, "{\"entware_is_install\":0,");//0 Not installed
		websWrite(stream, "\"entware_start_list\":[");
		n = scandir( "/opt/etc/init.d", &dl, NULL, alphasort );
		if ( n < 0 )
	    	websWrite(stream, "]");
		else
		{
			for ( i = 0; i < n; ++i )
				if (dl[i]->d_name[0] == 'S'){
					websWrite(stream, "[\"%s\",\"%d\"],", dl[i]->d_name, pidof(dl[i]->d_name + 3) > 0 ? 1 : 0);
				}
			websWrite(stream, "[\"\",\"\"]]");
			free(dl);
		}
		websWrite(stream, "}\n");
	}
	else if (!strcmp(app, "entware") && !strcmp(action, "list_installed"))
	{
		if (nvram_get_int("entware_busy"))
			websWrite(stream, "{\"entware_is_install\":2,");//2 to do something
		else if (f_exists("/opt/etc/entware_release"))
			websWrite(stream, "{\"entware_is_install\":1,");//1 Installed
		else
			websWrite(stream, "{\"entware_is_install\":0,");//0 Not installed
		websWrite(stream, "\"entware_app_list\":[");
		system("opkg list-installed > /tmp/entwarelist");
		if ((fp = fopen("/tmp/entwarelist", "r")))
		{
			memset(buf, 0, 128);
			while (fgets(buf, 128, fp))
				{
					buf[strlen(buf)-1] = 0;
					websWrite(stream, "\"%s\",", buf);
				}
			fclose(fp);
		}
		websWrite(stream, "\"\"],\"entware_update_list\":[");
		system("opkg list-upgradable > /tmp/entware.upgradable");
		if ((fp = fopen("/tmp/entware.upgradable", "r")))
		{
			memset(buf, 0, 128);
			while (fgets(buf, 128, fp))
				{
					buf[strlen(buf)-1] = 0;
					websWrite(stream, "\"%s\",", buf);
				}
			fclose(fp);
		}
		websWrite(stream, "\"\"]}\n");
	}
	else if (ent_act & ENTWARE_ACT_ALL)
	{
		if (nvram_get_int("entware_busy"))
			websWrite(stream, "{\"entware_is_install\":3}");//3 doing something
		else
		{
			if(mount && strcmp(mount, nvram_get("entware_mount"))){
				nvram_set("entware_mount", mount);
				nvram_set("entware_disk", disk);
				system("entware.sh");
			}
			websWrite(stream, "{\"entware_is_install\":2}");//2 to do something
			nvram_set_int("entware_busy", 1);
			nvram_set("entware_app", app);
			nvram_set_int("entware_action", ent_act);
			nvram_set("entware_arg", arg);
			notify_rc("start_entware");
		}
	}
	else
	{
		websWrite(stream, "{\"entware_is_install\":4}");//4 unknown
	}
	if(root)
		json_object_put(root);
}
#endif

#if defined(RTCONFIG_NLBWMON)
/*
	if type = all 		=> [TX, RX]
	if type = "" && name = ""	=> [[MAC0, TX, RX], [MAC1, TX, RX], ...]
	if type = "" && name = "MAC"	=> [[APP0, TX, RX], [APP1, TX, RX], ...]
*/
static void get_client_hook(int *retval, webs_t wp, char *type, char *name)
{
#if 0
	int first_row = 1;
	websWrite(wp, "[");
	websWrite(wp, "[\"%s\", \"%llu\", \"%llu\", \"%u\", \"%u\"]");
	if(first_row == 1){
		first_row = 0;
		websWrite(wp, "[\"%s\", \"%lu\", \"%lu\"]");
	}else
		websWrite(wp, ", [\"%s\", \"%lu\", \"%lu\"]");
	websWrite(wp, "]");
#else
	websWrite(wp, "[]");
#endif
}

void get_traffic_hook(char *mode, char *name, char *dura, char *date, int *retval, webs_t wp)
{
	if(!strcasecmp(mode, "traffic_wan") && !strcasecmp(dura, "realtime"))
		return get_client_hook(retval, wp, "all", "");
	else if(!strcasecmp(mode, "traffic") && !strcasecmp(dura, "realtime"))
		return get_client_hook(retval, wp, "", name);
	else
		websWrite(wp, "[]");
}
#endif

#if defined(RTCONFIG_MULTILAN_CFG)
void escape_json_char(char *orig, char *conv, int percent_only)
{
	int i = 0, j = 0;

	while(orig[i] != '\0')
	{
		if (percent_only) {
			switch(orig[i])
			{
				case '%':
					conv[j]='%';
					++j;
					conv[j]='%';
					break;
				default:
					conv[j] = orig[i];
					break;
			}
		}
		else
		{
			switch(orig[i])
			{
				case '"':
					conv[j] = '\\';
					++j;
					conv[j] = '\"';
					break;
				case '\\':
					conv[j]='\\';
					++j;
					conv[j]='\\';
					break;
				case '%':
					conv[j]='%';
					++j;
					conv[j]='%';
					break;
				default:
					conv[j] = orig[i];
					break;
			}
		}
		++i;
		++j;
	}

	conv[j] = '\0';
}
/*
{
"get_cfg_clientlist":[{"alias":"9C:C9:EB:F5:06:BE","model_name":"RAX120","ui_model_name":"RAX120","icon_model_name":"","product_id":"RT-AX89U","frs_model_name":"","fwver":"3.0.0.4.388_RAX120_R5.2.5-gd91c7","newfwver":"","ip":"192.168.50.1","mac":"9C:C9:EB:F5:06:BE","online":"1","ap2g":"9C:C9:EB:F5:06:B9","ap5g":"9C:C9:EB:F5:06:BD","ap5g1":"","apdwb":"","ap6g":"","wired_mac":["C8:5B:A0:E8:8C:6D"],"pap2g":"","rssi2g":"","pap5g":"","rssi5g":"","pap6g":"","rssi6g":"","level":"0","re_path":"0","config":{},"sta2g":"","sta5g":"","sta6g":"","capability":{"3":3,"2":1,"16":1,"17":1,"19":1,"20":1,"1":31,"4":10111,"22":3,"23":1},"ap2g_ssid":"DreamBox","ap5g_ssid":"DreamBox_5G","ap5g1_ssid":"","ap6g_ssid":"","pap2g_ssid":"","pap5g_ssid":"","pap6g_ssid":"","wired_port":{},"plc_status":{},"band_num":"2","tcode":"CN/01","misc_info":{"2":"1"},"ap2g_fh":"9C:C9:EB:F5:06:B9","ap5g_fh":"9C:C9:EB:F5:06:BD","ap5g1_fh":"","ap6g_fh":"","ap2g_ssid_fh":"DreamBox","ap5g_ssid_fh":"DreamBox_5G","ap5g1_ssid_fh":"","ap6g_ssid_fh":"","band_info":{"0":{"unit":0},"1":{"unit":1}}}]
}
*/
int ej_get_cfg_clientlist(int eid, webs_t wp, int argc, char **argv)
{
	int lock;
	int i, j = 0;
	char ip_buf[16] = {0};
	char alias_buf[33] = {0};
	char rmac_buf[32] = {0};
	char ap2g_buf[32], ap5g_buf[32], ap5g1_buf[32], apdwb_buf[32], ap6g_buf[32];
	char pap2g_buf[32], pap5g_buf[32], pap6g_buf[32];
	char rssi2g_buf[8], rssi5g_buf[8], rssi6g_buf[8];
	char model_name_buf[33] = {0}, product_id_buf[33] = {0}, frs_model_name_buf[33] = {0};
	char ui_model_name_buf[33] = {0};
	char alias_conv_buf[65];
	json_object *allBrMacListObj = NULL;
	json_object *macEntryObj = NULL;
	json_object *capabilityObj = NULL, *wiredPortObj = NULL;
	json_object *miscInfoObj = NULL, *bandInfoObj = NULL;
	int online = 0;
	int rePath = 0;
	char macList[1024] = {0};
	char *p = NULL;
	struct json_object *entry;
	char sta2g_buf[32], sta5g_buf[32], sta6g_buf[32];
	char capability_file_name[64] = {0};
	char capability_buf[4096] = {0};
	char ap2g_ssid_buf[33], ap5g_ssid_buf[33], ap5g1_ssid_buf[33], ap6g_ssid_buf[33];
	char ap2g_ssid_conv_buf[65], ap5g_ssid_conv_buf[65], ap5g1_ssid_conv_buf[65], ap6g_ssid_conv_buf[65];
	char pap2g_ssid_conv_buf[65], pap5g_ssid_conv_buf[65], pap6g_ssid_conv_buf[65];
	char word[256], *next = NULL, prefix[16];
	int unit = 0, subunit, bandNum = 0;
	char file_name[64] = {0}, wired_port_buf[512] = {0};
	char tcode_buf[16] = {0};
	int nband = 0;
	char misc_info_buf[256] = {0};
	int num5g = 0;
	char band_info_buf[256] = {0};

	lock = file_lock("cfg_clientlist");

	allBrMacListObj = json_object_from_file("/tmp/maclist.json");

	websWrite(wp, "[");
	p = NULL;
	memset(macList, 0, sizeof(macList));
	memset(alias_buf, 0, sizeof(alias_buf));
	memset(ip_buf, 0, sizeof(ip_buf));
	memset(rmac_buf, 0, sizeof(rmac_buf));
	memset(ap2g_buf, 0, sizeof(ap2g_buf));
	memset(ap5g_buf, 0, sizeof(ap5g_buf));
	memset(ap5g1_buf, 0, sizeof(ap5g1_buf));
	memset(apdwb_buf, 0, sizeof(apdwb_buf));
	memset(ap6g_buf, 0, sizeof(ap6g_buf));
	memset(pap2g_buf, 0, sizeof(pap2g_buf));
	memset(pap5g_buf, 0, sizeof(pap5g_buf));
	memset(pap6g_buf, 0, sizeof(pap6g_buf));
	memset(rssi2g_buf, 0, sizeof(rssi2g_buf));
	memset(rssi5g_buf, 0, sizeof(rssi5g_buf));
	memset(rssi6g_buf, 0, sizeof(rssi6g_buf));
	memset(sta2g_buf, 0, sizeof(sta2g_buf));
	memset(sta5g_buf, 0, sizeof(sta5g_buf));
	memset(sta6g_buf, 0, sizeof(sta6g_buf));
	memset(alias_conv_buf, 0, sizeof(alias_conv_buf));
	memset(ap2g_ssid_buf, 0, sizeof(ap2g_ssid_buf));
	memset(ap5g_ssid_buf, 0, sizeof(ap5g_ssid_buf));
	memset(ap5g1_ssid_buf, 0, sizeof(ap5g1_ssid_buf));
	memset(ap6g_ssid_buf, 0, sizeof(ap6g_ssid_buf));
	memset(ap2g_ssid_conv_buf, 0, sizeof(ap2g_ssid_conv_buf));
	memset(ap5g_ssid_conv_buf, 0, sizeof(ap5g_ssid_conv_buf));
	memset(ap5g1_ssid_conv_buf, 0, sizeof(ap5g1_ssid_conv_buf));
	memset(ap6g_ssid_conv_buf, 0, sizeof(ap6g_ssid_conv_buf));
	memset(pap2g_ssid_conv_buf, 0, sizeof(pap2g_ssid_conv_buf));
	memset(pap5g_ssid_conv_buf, 0, sizeof(pap5g_ssid_conv_buf));
	memset(pap6g_ssid_conv_buf, 0, sizeof(pap6g_ssid_conv_buf));

	//strlcpy(alias_buf, get_own_mac(), sizeof(alias_buf));
	/* get ssid of 2g & 5g & 5g1 & 6g */
	unit = 0;
	foreach (word, nvram_safe_get("wl_ifnames"), next) {
		SKIP_ABSENT_BAND_AND_INC_UNIT(unit);
		snprintf(prefix, sizeof(prefix), "wl%d_", unit);
		nband = nvram_pf_get_int(prefix, "nband");
		if (nband == 2) {
			strlcpy(ap2g_ssid_buf, nvram_pf_safe_get(prefix, "ssid"), sizeof(ap2g_ssid_buf));
		}
		else if (nband == 1)
		{
			num5g++;
			if (num5g == 1) {
				strlcpy(ap5g_ssid_buf, nvram_pf_safe_get(prefix, "ssid"), sizeof(ap5g_ssid_buf));
			}
			else if (num5g == 2)
			{
				strlcpy(ap5g1_ssid_buf, nvram_pf_safe_get(prefix, "ssid"), sizeof(ap5g1_ssid_buf));
			}
		}
		else if (nband == 4)
		{
			strlcpy(ap6g_ssid_buf, nvram_pf_safe_get(prefix, "ssid"), sizeof(ap6g_ssid_buf));
		}
		unit++;
	}
	escape_json_char(alias_buf, alias_conv_buf, 0);
	escape_json_char(ap2g_ssid_buf, ap2g_ssid_conv_buf, 0);
	escape_json_char(ap5g_ssid_buf, ap5g_ssid_conv_buf, 0);
	escape_json_char(ap5g1_ssid_buf, ap5g1_ssid_conv_buf, 0);
	escape_json_char(ap6g_ssid_buf, ap6g_ssid_conv_buf, 0);
	strlcpy(ip_buf, nvram_safe_get("lan_ipaddr"), sizeof(ip_buf));
	strlcpy(rmac_buf, get_own_mac(), sizeof(rmac_buf));

	/* get capability */
	memset(capability_buf, 0, sizeof(capability_buf));
	snprintf(capability_file_name, sizeof(capability_file_name), "/tmp/%s.cap", rmac_buf);
	capabilityObj = json_object_from_file(capability_file_name);
	if (capabilityObj) {
#ifdef RTCONFIG_BHCOST_OPT
		/* Transfer capbality CONN_UPLINK_PORTS(21) value to easy-to-read */
		covert_bitmap_describe(capabilityObj);
#endif
		snprintf(capability_buf, sizeof(capability_buf), "%s", json_object_to_json_string_ext(capabilityObj, 0));
		json_object_put(capabilityObj);
	}else{
		char tmp[16];
		phy_port_mapping port_mapping;
		json_object *wificap = NULL, *lancap = NULL, *portinfo = NULL, *portobj = NULL;
		json_object *wifiinfo = NULL, *wifiobj = NULL, *vifobj = NULL, *vifinfoobj = NULL;
		capabilityObj = json_object_new_object();
		wificap = json_object_new_object();
		lancap = json_object_new_object();
		portinfo = json_object_new_object();
		wifiinfo = json_object_new_object();
		unit = 0;
		foreach (word, nvram_safe_get("wl_ifnames"), next) {
			wifiobj = json_object_new_object();
			SKIP_ABSENT_BAND_AND_INC_UNIT(unit);
			snprintf(prefix, sizeof(prefix), "wl%d_", unit);
			nband = nvram_pf_get_int(prefix, "nband");
			if (nband == 2) {
				json_object_object_add(wifiobj, "band", json_object_new_int(WIFI_BAND_2G));
			}
			else if (nband == 1)
			{
#if defined(RTCONFIG_HAS_5G_2)
				num5g++;
				if (num5g == 1) {
					json_object_object_add(wifiobj, "band", json_object_new_int(WIFI_BAND_5GL));
				}
				else if (num5g == 2)
				{
					json_object_object_add(wifiobj, "band", json_object_new_int(WIFI_BAND_5GH));
				}
#else
				json_object_object_add(wifiobj, "band", json_object_new_int(WIFI_BAND_5G));
#endif
			}
			else if (nband == 4)
			{
				json_object_object_add(wifiobj, "band", json_object_new_int(WIFI_BAND_6G));
			}
			for(subunit = 0; subunit < num_of_mssid_support(unit) && subunit < MAX_DUT_LIST_SIZE; subunit++){
				vifobj = json_object_new_object();
				vifinfoobj = json_object_new_object();
				memset(tmp, 0, sizeof(tmp));
				if(subunit == 0){
					snprintf(tmp, sizeof(tmp), "%s", get_wififname(unit));
					json_object_object_add(vifinfoobj, "type", json_object_new_int(VIF_TYPE_MAIN));
				}else{
					snprintf(tmp, sizeof(tmp), "wl%d.%d_ifname", unit, subunit);
					json_object_object_add(vifinfoobj, "type", json_object_new_int(VIF_TYPE_NO_USED));
				}
				json_object_object_add(vifinfoobj, "prefix", json_object_new_string(nvram_safe_get(tmp)));
				json_object_object_add(vifobj, nvram_safe_get(tmp), vifinfoobj);
				json_object_object_add(wifiobj, "vif", vifobj);
			}
			memset(tmp, 0, sizeof(tmp));
			snprintf(tmp, sizeof(tmp), "%d", unit);
			json_object_object_add(wifiobj, "count", json_object_new_int(subunit));
			json_object_object_add(wifiinfo, tmp, wifiobj);
			unit++;
		}
		json_object_object_add(wificap, "wifi_band", wifiinfo);
		json_object_object_add(capabilityObj, "26", wificap);

		get_phy_port_mapping(&port_mapping);
		for(i = 1; i < port_mapping.count; i++){//skip wan
			memset(tmp, 0, sizeof(tmp));
			snprintf(tmp, sizeof(tmp), "%d", i);
			portobj = json_object_new_object();
			json_object_object_add(portobj, "index", json_object_new_string(tmp));
			json_object_object_add(portobj, "if_name", json_object_new_string(port_mapping.port[i].ifname ? : ""));
			json_object_object_add(portobj, "phy_port_id", json_object_new_int(port_mapping.port[i].phy_port_id));
			json_object_object_add(portobj, "label_name", json_object_new_string(port_mapping.port[i].label_name));
			json_object_object_add(portobj, "max_rate", json_object_new_int(port_mapping.port[i].max_rate));

			json_object_object_add(portinfo, tmp, portobj);
		}
		json_object_object_add(lancap, "lan_port", portinfo);
		json_object_object_add(capabilityObj, "27", lancap);
		snprintf(capability_buf, sizeof(capability_buf), "%s", json_object_to_json_string_ext(capabilityObj, 0));
		json_object_put(capabilityObj);
	}
	strlcpy(ap2g_buf, nvram_safe_get("wl0_hwaddr"), sizeof(ap2g_buf));
	strlcpy(ap5g_buf, nvram_safe_get("wl1_hwaddr"), sizeof(ap5g_buf));
	strlcpy(ap5g1_buf, nvram_safe_get("wl2_hwaddr"), sizeof(ap5g1_buf));
	strlcpy(ap6g_buf, nvram_safe_get("wl2_hwaddr"), sizeof(ap6g_buf));

	/* modle name */
	strlcpy(model_name_buf, get_productid(), sizeof(model_name_buf));
	memset(ui_model_name_buf, 0, sizeof(ui_model_name_buf));
#ifdef RTCONFIG_ODMPID
	replace_productid(model_name_buf, ui_model_name_buf, sizeof(ui_model_name_buf));
#else
	snprintf(ui_model_name_buf, sizeof(ui_model_name_buf), "%s", model_name_buf);
#endif

	/* product id */
	strlcpy(product_id_buf, rt_buildname, sizeof(product_id_buf));

	if (allBrMacListObj) {
		json_object_object_get_ex(allBrMacListObj, rmac_buf, &macEntryObj);
		if (macEntryObj) {
			int macEntryLen = json_object_array_length(macEntryObj);
			if (macEntryLen) {
				memset(macList, 0, sizeof(macList));
				p = macList;
				p += snprintf(p, sizeof(macList), "[");
				for (j = 0; j < macEntryLen; j++) {
					entry = json_object_array_get_idx(macEntryObj, j);
					if(strlen(macList)+3+strlen(json_object_get_string(entry)) > sizeof(macList) -2)
					{
						_dprintf("too many macList entries. \n");
						break;
					}
					if (j) p += snprintf(p, sizeof(macList), ",");
					p += snprintf(p, sizeof(macList), "\"%s\"", json_object_get_string(entry));
				}
				p += snprintf(p, sizeof(macList), "]");
			}
		}
	}

	/* wired port */
	memset(wired_port_buf, 0, sizeof(wired_port_buf));
	snprintf(file_name, sizeof(file_name), "/tmp/%s.port", rmac_buf);
	wiredPortObj = json_object_from_file(file_name);
	if (wiredPortObj) {
		snprintf(wired_port_buf, sizeof(wired_port_buf), "%s", json_object_to_json_string_ext(wiredPortObj, 0));
		json_object_put(wiredPortObj);
	}

	/* get misc info */
	memset(misc_info_buf, 0, sizeof(misc_info_buf));
	snprintf(file_name, sizeof(file_name), "/tmp/misc.json");
	if (check_if_file_exist(file_name)) {
		miscInfoObj = json_object_from_file(file_name);
		if (miscInfoObj) {
			snprintf(misc_info_buf, sizeof(misc_info_buf), "%s", json_object_to_json_string_ext(miscInfoObj, 0));
			json_object_put(miscInfoObj);
		}
	}

	online = 1;

	/* band num */
#if defined(RTCONFIG_HAS_5G_2) || defined(RTCONFIG_WIFI6E)
	bandNum = 3;
#else
	bandNum = 2;
#endif

	/* territory code */
	strlcpy(tcode_buf, nvram_safe_get("territory_code"), sizeof(tcode_buf));

	/* get band index */
	snprintf(file_name, sizeof(file_name), "/tmp/%s.bi", rmac_buf);
	bandInfoObj = json_object_from_file(file_name);
	if (bandInfoObj) {
		memset(band_info_buf, 0, sizeof(band_info_buf));
		snprintf(band_info_buf, sizeof(band_info_buf), "%s", json_object_to_json_string_ext(bandInfoObj, 0));
		json_object_put(bandInfoObj);
	}

	websWrite(wp, "{");
	websWrite(wp, "\"alias\":\"%s\",", strlen(alias_conv_buf) ? alias_conv_buf : rmac_buf);
	websWrite(wp, "\"model_name\":\"%s\",", model_name_buf);
	websWrite(wp, "\"ui_model_name\":\"%s\",", ui_model_name_buf);
	websWrite(wp, "\"product_id\":\"%s\",", product_id_buf);
	websWrite(wp, "\"frs_model_name\":\"%s\",", frs_model_name_buf);
	websWrite(wp, "\"ip\":\"%s\",", ip_buf);
	websWrite(wp, "\"mac\":\"%s\",", rmac_buf);
	websWrite(wp, "\"online\":\"%d\",", online);
	websWrite(wp, "\"ap2g\":\"%s\",", strcmp(ap2g_buf, "00:00:00:00:00:00") ? ap2g_buf : "");
	websWrite(wp, "\"ap5g\":\"%s\",", strcmp(ap5g_buf, "00:00:00:00:00:00") ? ap5g_buf : "");
	websWrite(wp, "\"ap5g1\":\"%s\",", strcmp(ap5g1_buf, "00:00:00:00:00:00") ? ap5g1_buf : "");
	websWrite(wp, "\"apdwb\":\"%s\",", strcmp(apdwb_buf, "00:00:00:00:00:00") ? apdwb_buf : "");
	websWrite(wp, "\"ap6g\":\"%s\",", strcmp(ap6g_buf, "00:00:00:00:00:00") ? ap6g_buf : "");
	websWrite(wp, "\"wired_mac\":%s,", strlen(macList) ? macList : "[]");
	websWrite(wp, "\"pap2g\":\"%s\",", strlen(pap2g_buf) ? pap2g_buf : "");
	websWrite(wp, "\"rssi2g\":\"%s\",", strlen(rssi2g_buf) ? rssi2g_buf : "");
	websWrite(wp, "\"pap5g\":\"%s\",", strlen(pap5g_buf) ? pap5g_buf : "");
	websWrite(wp, "\"rssi5g\":\"%s\",", strlen(rssi5g_buf) ? rssi5g_buf : "");
	websWrite(wp, "\"pap6g\":\"%s\",", strlen(pap6g_buf) ? pap6g_buf : "");
	websWrite(wp, "\"rssi6g\":\"%s\",", strlen(rssi6g_buf) ? rssi6g_buf : "");
	websWrite(wp, "\"re_path\":\"%d\",", rePath);
	websWrite(wp, "\"config\":%s,", "{}");
	websWrite(wp, "\"sta2g\":\"%s\",", strcmp(sta2g_buf, "00:00:00:00:00:00") ? sta2g_buf : "");
	websWrite(wp, "\"sta5g\":\"%s\",", strcmp(sta5g_buf, "00:00:00:00:00:00") ? sta5g_buf : "");
	websWrite(wp, "\"sta6g\":\"%s\",", strcmp(sta6g_buf, "00:00:00:00:00:00") ? sta6g_buf : "");
	websWrite(wp, "\"capability\":%s,", strlen(capability_buf) ? capability_buf : "{}");
	websWrite(wp, "\"ap2g_ssid\":\"%s\",", strlen(ap2g_ssid_conv_buf) ? ap2g_ssid_conv_buf : "");
	websWrite(wp, "\"ap5g_ssid\":\"%s\",", strlen(ap5g_ssid_conv_buf) ? ap5g_ssid_conv_buf : "");
	websWrite(wp, "\"ap5g1_ssid\":\"%s\",", strlen(ap5g1_ssid_conv_buf) ? ap5g1_ssid_conv_buf : "");
	websWrite(wp, "\"ap6g_ssid\":\"%s\",", strlen(ap6g_ssid_conv_buf) ? ap6g_ssid_conv_buf : "");
	websWrite(wp, "\"pap2g_ssid\":\"%s\",", strlen(pap2g_ssid_conv_buf) ? pap2g_ssid_conv_buf : "");
	websWrite(wp, "\"pap5g_ssid\":\"%s\",", strlen(pap5g_ssid_conv_buf) ? pap5g_ssid_conv_buf : "");
	websWrite(wp, "\"pap6g_ssid\":\"%s\",", strlen(pap6g_ssid_conv_buf) ? pap6g_ssid_conv_buf : "");
	websWrite(wp, "\"wired_port\":%s,", strlen(wired_port_buf) ? wired_port_buf : "{}");
	websWrite(wp, "\"band_num\":\"%d\",", bandNum);
	websWrite(wp, "\"tcode\":\"%s\",", strlen(tcode_buf) ? tcode_buf : "");
	websWrite(wp, "\"misc_info\":%s,", strlen(misc_info_buf) ? misc_info_buf : "{}");
	websWrite(wp, "\"band_info\":%s", strlen(band_info_buf) ? band_info_buf : "{}");
	websWrite(wp, "}");
	websWrite(wp, "]");

	if (allBrMacListObj)
		json_object_put(allBrMacListObj);

	file_unlock(lock);
 
	return 0;
}
#endif
