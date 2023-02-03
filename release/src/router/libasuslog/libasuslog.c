/*
 * Copyright 2021, ASUS
 * Copyright 2021-2023, SWRTdev
 * Copyright 2021-2023, paldier <paldier@hotmail.com>.
 * Copyright 2021-2023, lostlonger<lostlonger.g@gmail.com>.
 * All Rights Reserved.
 */

#include "libasuslog.h"

const char *fdata[] = {"emergency", "alert", "critical", "error", "warning", "notice", "info", "debug"};
int ftext(char *path)
{
	int i, len;
	char filepath[512];
	strncpy(filepath, path, sizeof(filepath));
	len = strlen(filepath);
	for(i = 0; i < len; ++i){
		if(filepath[i] == '/'){
			filepath[i] = 0x0;
			if(access(filepath, F_OK))
				mkdir(filepath, 0777);
			filepath[i] = '/';
		}
	}
	if(len > 0){
		if(access(filepath, F_OK))
			mkdir(filepath, 0777);
	}
	return 0;
}

int asusdebuglog(int level, char *path, int conlog, int showtime, unsigned filesize, const char *msgfmt, ...)
{
	FILE *fp = NULL;
	time_t curtime;
	struct stat log_stat;
	size_t file_size, maxsize;
	int fbss = 0, count;
	char log[256];
	char logpath[300];
	char logpath2[300];
	char *asuslog_path = nvram_safe_get("asuslog_path");
	char *time_str = NULL;
	va_list va;
	va_list arg;
	va_start(va, msgfmt);
	switch(conlog)
	{
		case LOG_NOTHING:
			return 1;
		case LOG_SYSTEMLOG:
			va_copy(arg, va);
			count = vsnprintf(log, sizeof(log), msgfmt, va);
			if(count < 257)
				log[count] = 0x0;
			else
				log[256] = 0x0;
			if(level)
				syslog(level, "%s", log);
			else
				syslog(6, "%s", log);
			return 1;
		case LOG_CONSOLE:
			va_copy(arg, va);
			count = vsnprintf(log, sizeof(log), msgfmt, va);
			if(count < 257)
				log[count] = 0x0;
			else
				log[256] = 0x0;
			_dprintf("\n (%s) \n", log);
			return 1;
		default:
			break;
	}
	memset(logpath, 0, sizeof(logpath));
	if(*asuslog_path){
		if (filesize)
			maxsize = filesize << 10;
		else
			maxsize = 0x200000;
		if(strchr(path, '/')){
			snprintf(logpath, sizeof(logpath), "%s", path);
		}else{
			ftext(asuslog_path);
			snprintf(logpath, sizeof(logpath), "%s/%s", asuslog_path, path);
		}
	}else{
		if(filesize)
			maxsize = filesize < 1025 ? filesize << 10 : 0x100000;
		else
			maxsize = 204800;
		if(strchr(path, '/')){
			snprintf(logpath, sizeof(logpath), "%s", path);
		}else{
			ftext("/tmp/asusdebuglog");
			snprintf(logpath, sizeof(logpath), "%s/%s", "/tmp/asusdebuglog", path);
		}
	}
	fp = fopen(logpath, "a+");
	if(*asuslog_path){
		if(!fp){
			memset(logpath, 0, sizeof(logpath));
			ftext("/tmp/asusdebuglog");
			if(filesize)
				maxsize = filesize < 1025 ? filesize << 10 : 0x100000;
			else
				maxsize = 204800;
			snprintf(logpath, sizeof(logpath), "%s/%s", "/tmp/asusdebuglog", path);
			fp = fopen(logpath, "a+");
			if(!fp)
				return 0;;
		}
	}else if(!fp)
		return 0;
	if(stat(logpath, &log_stat))
		file_size = 0;
	else
		file_size = log_stat.st_size;
	time(&curtime);
	time_str = ctime(&curtime);
	time_str[strlen(time_str) - 1] = ' ';
	va_copy(arg, va);
	count = vsnprintf(log, sizeof(log), msgfmt, va);
	if(count < 257)
		log[count] = 0;
	else
		log[256] = 0;
	if(file_size < maxsize){
		if(*asuslog_path){
			if(level)
				fprintf(fp, "%s [pid=%d] [%s] %s", time_str, getpid(), fdata[level], log);
			else
				fprintf(fp, "%s [pid=%d] %s", time_str, getpid(), log);
		}else if(showtime){
			if(level)
				fprintf(fp, "%s [%s] %s", time_str, fdata[level], log);
			else
				fprintf(fp, "[%s] %s", time_str, log);
		}else if(level)
			fprintf(fp, " [%s] %s", fdata[level], log);
		else
      		fprintf(fp, " %s", log);
		fflush(fp);
		fclose(fp);
		if(!strcmp(nvram_safe_get("asuslog_debug_test"), "1"))
			_dprintf("\n ----%s no unlock ------------\n", __func__);
		return 1;
	}else if(*asuslog_path && maxsize <= file_size){
		++fbss;
		fclose(fp);
		sprintf(logpath2, "%s-%d", logpath, fbss);
		while(!access(logpath2, F_OK))
			sprintf(logpath2, "%s-%d", logpath, ++fbss);
		rename(logpath, logpath2);
		fp = fopen(logpath, "a+");
		if ( fp )
		{
			fputs(log, fp);
			fflush(fp);
			fclose(fp);
			if(!strcmp(nvram_safe_get("asuslog_debug_test"), "1"))
				_dprintf("\n ----%s no unlock ------------\n", __func__);
			return 1;
		}
	}else{
		fclose(fp);
		sprintf(logpath2, "%s-1", logpath);
		unlink(logpath2);
		rename(logpath, logpath2);
		fp = fopen(logpath, "a+");
		if ( fp )
		{
			fputs(log, fp);
			fflush(fp);
			fclose(fp);
			if(!strcmp(nvram_safe_get("asuslog_debug_test"), "1"))
				_dprintf("\n ----%s no unlock ------------\n", __func__);
			return 1;
		}
	}
	return 0;
}

int rm_asusdebuglog(char *path)
{
	unlink(path);
	return 0;
}
