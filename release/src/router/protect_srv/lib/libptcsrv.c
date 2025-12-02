#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/mman.h>
#include <time.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <dirent.h>
#include <stdarg.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <syslog.h>
#include <protect_srv.h>

#ifndef RTCONFIG_NOTIFICATION_CENTER
int GetDebugValue(char *path)
{
	FILE *fp;
	char *np;
	char val[8];
	fp = fopen(path, "r");
	if(fp) {
		fgets(val,3,fp);
		np = strrchr(val, '\n');
		if (np) *np='\0';
		fclose(fp);
		return (int)strtol(val, NULL, 10);
	} else {
		return 0;
	}
}

void Debug2Console(const char * format, ...)
{
	FILE *f;
	int nfd;
	va_list args;
	
	if (((nfd = open("/dev/console", O_WRONLY | O_NONBLOCK)) > 0) &&
	    (f = fdopen(nfd, "w")))
	{
		va_start(args, format);
		vfprintf(f, format, args);
		va_end(args);
		fclose(f);
	}
	else
	{
		va_start(args, format);
		vfprintf(stderr, format, args);
		va_end(args);
	}
	
	if (nfd != -1) close(nfd);
}

int isFileExist(char *fname)
{
	struct stat fstat;
	
	if (lstat(fname,&fstat)==-1)
		return 0;
	if (S_ISREG(fstat.st_mode))
		return 1;
	
	return 0;
}
#endif

static void send_ptcsrv_event(int s_type, int status, const char *data, const char *msg)
{
	struct sockaddr_un addr;
	int sockfd;
	size_t n;
	PTCSRV_SOCK_DATA_T ptcsrv;
	memset(&ptcsrv, 0, sizeof(PTCSRV_SOCK_DATA_T));
	ptcsrv.d_type = PTCSRV_S_RPT;
	ptcsrv.report.s_type = s_type;
	ptcsrv.report.status = status;
	snprintf(ptcsrv.report.addr, sizeof(ptcsrv.report.addr), "%s", data);
	snprintf(ptcsrv.report.msg, sizeof(ptcsrv.report.msg), "%s", msg);
	
	if ( (sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
		syslog(LOG_ERR, "[%s:(%d)] ERROR socket.\n", __FUNCTION__, __LINE__);
		perror("socket error");
		return;
	}
	
	fcntl(sockfd, F_SETFL, O_NONBLOCK);
	
	memset(&addr, 0, sizeof(addr));
	addr.sun_family = AF_UNIX;
	strlcpy(addr.sun_path, PROTECT_SRV_SOCKET_PATH, sizeof(addr.sun_path));
	
	if (connect(sockfd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
		syslog(LOG_DEBUG, "[%s:(%d)] ERROR connect:%s.\n", __FUNCTION__, __LINE__, strerror(errno));
		perror("connect error");
		close(sockfd);
		return;
	}
	
	n = write(sockfd, &ptcsrv, sizeof(PTCSRV_SOCK_DATA_T));
	
	close(sockfd);
	
	if(n < 0) {
		syslog(LOG_DEBUG, "[%s:(%d)] ERROR write:%s.\n", __FUNCTION__, __LINE__, strerror(errno));
		perror("writing error");
	}
}

int ptcsrv_cli_create()
{
	struct sockaddr_un addr;
	int sockfd;
	if((sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0){
		syslog(LOG_ERR, "[%s:(%d)] ERROR socket.\n", __FUNCTION__, __LINE__);
		perror("socket error");
		return -1;
	}
	//fcntl(sockfd, F_SETFL, O_NONBLOCK);
	memset(&addr, 0, sizeof(addr));
	addr.sun_family = AF_UNIX;
	strlcpy(addr.sun_path, PROTECT_SRV_SOCKET_PATH, sizeof(addr.sun_path));
	if(connect(sockfd, (struct sockaddr*)&addr, sizeof(addr)) == 0)
		return sockfd;
	syslog(LOG_ERR, "[%s:(%d)] ERROR connect:%s.\n", __FUNCTION__, __LINE__, strerror(errno));
	perror("connect error");
	close(sockfd);
	return -1;
}

int ptcsrv_get_data(PTCSRV_SOCK_DATA_T *s_data, void* data, size_t len)
{
	int sockfd;
	size_t wlen, n;
	sockfd = ptcsrv_cli_create();
	if(sockfd < 0)
		return -1;
	wlen = sizeof(PTCSRV_SOCK_DATA_T);
	while(1){
		n = write(sockfd, s_data, wlen);
		if(n < 0){
			syslog(LOG_ERR, "[%s:(%d)] ERROR write:%s.\n", __FUNCTION__, __LINE__, strerror(errno));
			perror("write error");
			close(sockfd);
			return -1;
		}
		if(n == wlen)
			break;
		wlen -= n;
		s_data = (PTCSRV_SOCK_DATA_T *)((char *)s_data + n);
	}
	while(1){
		n = read(sockfd, data, len);
		if(n < 0){
			syslog(LOG_ERR, "[%s:(%d)] ERROR read:%s.\n", __FUNCTION__, __LINE__, strerror(errno));
			perror("read error");
			close(sockfd);
			return -1;
		}
		if(n == 0)
			break;
		len -= n;
		data = (char *)data + n;
	}
	close(sockfd);
	return 0;
}

void SEND_PTCSRV_EVENT(int s_type, int status, const char *addr, const char *msg)
{
	send_ptcsrv_event(s_type, status, addr, msg);
}

int IS_PTCSRV_LOCKED(int s_type, const char *addr)
{
	int ret = 0;
	PTCSRV_SOCK_DATA_T ptcsrv;
	memset(&ptcsrv, 0, sizeof(PTCSRV_SOCK_DATA_T));
	ptcsrv.d_type = PTCSRV_G_LOCK;
	ptcsrv.report.s_type = s_type;
	snprintf(ptcsrv.report.addr, sizeof(ptcsrv.report.addr), "%s", addr);
	ptcsrv_get_data(&ptcsrv, &ret, sizeof(ret));
	return ret;
}
