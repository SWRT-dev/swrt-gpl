#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netlink/genl/genl.h>
#include <netlink/genl/ctrl.h>
#include <netlink/route/link.h>

#include "easy.h"

static int hex2num(char c)
{
	if (c >= '0' && c <= '9')
		return c - '0';
	if (c >= 'a' && c <= 'f')
		return c - 'a' + 10;
	if (c >= 'A' && c <= 'F')
		return c - 'A' + 10;
	return -1;
}

static int hex2byte(const char *hex)
{
	int a, b;
	if((a = hex2num(*hex++)) < 0)
		return -1;
	if((b = hex2num(*hex++)) < 0)
		return -1;

	return (a << 4) | b;
}

LIBEASY_API uint8_t *strtob(char *str, int len, uint8_t *bytes)
{
	size_t slen;
	int i;

	if (!str || !bytes)
		return NULL;

	slen = strlen(str);
	if (!slen || slen % 2 || str[slen] != '\0')
		return NULL;

	slen >>= 1;
	if (len > slen)
		len = (int)slen;

	for (i = 0; i < len; i++) {
		int a;

		if ((a = hex2byte(str)) < 0)
			return NULL;

		str += 2;
		bytes[i] = (uint8_t)a;
	}

	return bytes;
}

LIBEASY_API char *btostr(uint8_t *bytes, int len, char *str)
{
	size_t i;

	if (!str || !bytes)
		return NULL;

	for (i = 0; i < len; i++)
		sprintf(str + strlen(str), "%02x", bytes[i] & 0xff);

	return str;
}

LIBEASY_API uint8_t *hwaddr_aton(const char *macstr, uint8_t *mac)
{
	size_t i;

	for (i = 0; i < 6; i++) {
		int a;

		if((a = hex2byte(macstr)) < 0)
			return NULL;

		macstr += 2;
		mac[i] = (uint8_t)a;
		if (i < 6 - 1 && *macstr++ != ':')
			return NULL;
	}
	return mac;
}

LIBEASY_API char *hwaddr_ntoa(const uint8_t *mac, char *macstr)
{
	sprintf(macstr, "%02x:%02x:%02x:%02x:%02x:%02x",
			mac[0]&0xff, mac[1]&0xff,
			mac[2]&0xff, mac[3]&0xff,
			mac[4]&0xff, mac[5]&0xff);

	return macstr;
}

int LIBEASY_API uuid_strtob(char *uuidstr, uint8_t *uuid)
{
	uint8_t elen[] = {8, 4, 4, 4, 12};
	char *ptr, *pos, *end;
	uint8_t *ret;
	int idx = 0;
	int i = 0;


	if (!uuidstr || !uuid)
		return -1;

	ptr = uuidstr;
	end = uuidstr + strlen(uuidstr);

	while (ptr < end) {
		char tmp[32] = {0};

		pos = ptr;
		ptr = strchr(pos, '-');
		if (!ptr) {
			if (i == 4) {
				ptr = end;
				if (ptr - pos != elen[i])
					return -1;

				strncpy(tmp, pos, elen[i]);
				ret = strtob(tmp, elen[i] / 2, &uuid[idx]);
				return !ret ? -1 : 0;
			}

			return -1;
		}

		if (ptr - pos != elen[i])
			return -1;

		strncpy(tmp, pos, elen[i]);
		ret = strtob(tmp, elen[i] / 2, &uuid[idx]);
		if (!ret)
			return -1;

		ptr++;
		idx += elen[i] / 2;
		i++;
	}

	return 0;
}

int LIBEASY_API uuid_btostr(uint8_t *uuid, char *uuidstr)
{
	if (!uuidstr || !uuid)
		return -1;

	sprintf(uuidstr,
		"%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
		uuid[0], uuid[1], uuid[2], uuid[3],
		uuid[4], uuid[5], uuid[6], uuid[7],
		uuid[8], uuid[9], uuid[10], uuid[11],
		uuid[12], uuid[13], uuid[14], uuid[15]);

	return 0;
}

int LIBEASY_API get_ifstatus(const char *ifname, ifstatus_t *f)
{
	struct ifreq ifr;
	int ret = 0;
	int s;

	s = socket(AF_INET, SOCK_STREAM, 0);
	if (s < 0)
		return -errno;

	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_name, ifname, 16);
	if (ioctl(s, SIOCGIFFLAGS, &ifr) != 0) {
		ret = errno;
		close(s);
		return -ret;
	}

	*f = (ifstatus_t)ifr.ifr_flags;
	close(s);
	return 0;
}

int LIBEASY_API get_ifoperstatus(const char *ifname, ifopstatus_t *opstatus)
{
	struct rtnl_link *link;
	struct nl_sock *sk;
	int ret = 0;

	sk = nl_socket_alloc();
	if (sk == NULL) {
		ret = -errno;
		return ret;
	}

	nl_connect(sk, NETLINK_ROUTE);
	link = rtnl_link_alloc();
	if (link == NULL) {
		ret = -errno;
		nl_socket_free(sk);
		return ret;
	}

	if (rtnl_link_get_kernel(sk, 0, ifname, &link) < 0) {
		ret = -1;
		goto out;
	}

	*opstatus = rtnl_link_get_operstate(link);

out:
	rtnl_link_put(link);
	nl_socket_free(sk);
	return ret;
}

int LIBEASY_API set_sighandler(int sig, void (*handler)(int))
{
	struct sigaction sa;

	memset(&sa, 0, sizeof(sa));
	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask);
	sa.sa_handler = handler;
	if (sigaction(sig, &sa, NULL) < 0) {
		libeasy_err("Error sigaction %d\n", sig);
		return -1;
	}

	return 0;
}

int LIBEASY_API unset_sighandler(int sig)
{
	struct sigaction sa;

	memset(&sa, 0, sizeof(sa));
	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask);
	sa.sa_handler = SIG_DFL;

	return sigaction(sig, &sa, NULL);
}

#define QD_LINE_MAX 2048
LIBEASY_API char *trim(char *str)
{
	size_t start = 0, end = 0;

	if (!str)
		return NULL;

	if (strlen(str) == 0)
		return str;

	/* trim at the end */
	end = strlen(str);
	while (end > 0 && isspace(str[end - 1]))
		end--;
	str[end] = 0;

	/* trim at the start */
	start = 0;
	while (start < end && isspace(str[start]))
		start++;
	memmove(str, str + start, end - start + 1);

	return str;
}

void LIBEASY_API remove_newline(char *buf)
{
	size_t len;

	len = strlen(buf);
	if (len > 0 && buf[len - 1] == '\n')
		buf[len - 1] = 0;
}

int vsnsystemf(char *output, size_t output_size, const char *format, va_list ap)
{
	int n, rv = -1;
	size_t cmdline_size = 256;
	char *cmdline = NULL, *new_cmdline;

	cmdline = (char *)malloc(cmdline_size);
	if (!cmdline)
		goto out;

	while (1) {
		memset(cmdline, 0, cmdline_size);

		n = vsnprintf(cmdline, cmdline_size, format, ap);

		if (n < 0)
			goto out_cmdline;
		if (n < cmdline_size)
			break; /* good */

		/* else try again with more space */
		cmdline_size += 32;
		new_cmdline = (char *) realloc(cmdline,
						cmdline_size * sizeof(char));
		if (!new_cmdline)
			goto out_cmdline;
		cmdline = new_cmdline;
	}

	FILE *stream;
	char *line;

	stream = popen(cmdline, "r");	/* Flawfinder: ignore */
	if (!stream)
		goto out_stream;

	if (!output || !(output_size > 0))
		goto out_no_output;

	line = (char *) malloc(QD_LINE_MAX * sizeof(char));
	if (!line)
		goto out_line;

	memset(output, 0, output_size);
	while (fgets(line, QD_LINE_MAX, stream)) {
		int remaining = (int)output_size - (int)strlen(output) - 1;

		if (remaining <= 0)
			break;
		strncat(output, line, (size_t)remaining);
	}
	output[output_size - 1] = 0;

out_line:
	free(line);
out_no_output:
	rv = pclose(stream);
	/* Check pclose() status */
	if (WIFEXITED(rv))
		rv = WEXITSTATUS(rv);
	else if (WIFSIGNALED(rv))
		rv = WTERMSIG(rv);
	else if (WIFSTOPPED(rv))
		rv = WSTOPSIG(rv);
out_stream:
out_cmdline:
	free(cmdline);
out:
	return rv;
}

int vsystemf(const char *format, va_list ap)
{
	int rv;

	rv = vsnsystemf(NULL, 0, format, ap);

	return rv;
}


/* runCmd is an alias for systemf, calls directly vsystemf */
void LIBEASY_API runCmd(const char *format, ...)
{
	va_list ap;

	va_start(ap, format);
	vsystemf(format, ap);
	va_end(ap);
}

/* chrCmd is an alias for snsystemf, calls directly vsnsystemf */
LIBEASY_API char *chrCmd(char *output, size_t output_size, const char *format, ...)
{
	va_list ap;

	va_start(ap, format);
	vsnsystemf(output, output_size, format, ap);
	va_end(ap);

	trim(output);

	return output;
}

LIBEASY_API int Cmd(char *output, size_t output_size, const char *format, ...)
 {
	int rv;
	va_list ap;

	va_start(ap, format);

	rv = vsnsystemf(output, output_size, format, ap);
	va_end(ap);

	trim(output);

	return rv;
 }
