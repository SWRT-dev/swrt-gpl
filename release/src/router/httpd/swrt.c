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
 * Copyright 2018-2023, SWRTdev.
 * Copyright 2018-2023, paldier <paldier@hotmail.com>.
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
#if defined(RTCONFIG_SOFTCENTER)
#include <shared.h>
#include <dbapi.h>
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
			json_object_put(idObj);
			json_object_put(methodObj);
			json_object_put(paramsObj);
			json_object_put(fieldsObj);
			json_object_put(arrayObj);
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
							websWrite(wp,"{\"result\":\"%s\"}\n",buf);
							unlink(scPath);
							return 0;
						}
					}
				}
			}
			websWrite(wp,"{\"result\":%d}\n",id);
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
