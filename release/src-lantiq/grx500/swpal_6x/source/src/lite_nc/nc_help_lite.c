#include "nc_help_lite.h"
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/un.h>

/* static function */

static int xsocket_type(len_and_sockaddr **lsap, int family, int sock_type)
{
	len_and_sockaddr *lsa;
	int fd;
	int len;

	if (family == AF_UNSPEC) {
	   family = AF_INET;
	}

	fd = socket(family, sock_type, 0);

	len = sizeof(struct sockaddr_in);
	if (family == AF_UNIX)
		len = sizeof(struct sockaddr_un);

	lsa = malloc(LSA_LEN_SIZE + len);
	if (lsa == NULL)
		return -1;

	memset((char*)lsa, '\0', LSA_LEN_SIZE + len);

	lsa->len = len;
	lsa->u.sa.sa_family = family;
	*lsap = lsa;
	return fd;
}

static void set_nport(struct sockaddr *sa, unsigned port)
{
	if (sa->sa_family == AF_INET) {
		struct sockaddr_in *sin = (void*) sa;
		sin->sin_port = port;
		return;
	}
}

static void setsockopt_reuseaddr(int fd)
{
	int optval = 1;	
	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int));
}

static int create_and_bind_or_die(int port, int sock_type)
{
	int fd;
	len_and_sockaddr *lsa;

	fd = xsocket_type(&lsa, AF_UNSPEC, sock_type);
	set_nport(&lsa->u.sa, htons(port));

	setsockopt_reuseaddr(fd);
	bind(fd, &lsa->u.sa, lsa->len);
	free(lsa);
	return fd;
}

static ssize_t safe_write(int fd, const void *buf, size_t count)
{
	ssize_t n;

	for (;;) {
		n = write(fd, buf, count);
		if (n >= 0 || errno != EINTR)
			break;
		errno = 0;
	}

	return n;
}

static ssize_t full_write(int fd, const void *buf, size_t len)
{
	ssize_t cc;
	ssize_t total;

	total = 0;

	while (len) {
		cc = safe_write(fd, buf, len);

		if (cc < 0) {
			if (total)
				return total;
			return cc;	/* write() returns -1 on failure. */
		}

		total += cc;
		buf = ((const char *)buf) + cc;
		len -= cc;
	}

	return total;
}


/* API */

int create_and_bind_stream_or_die(int port)
{
	return create_and_bind_or_die(port, SOCK_STREAM);
}

void close_on_exec_on(int fd)
{
	fcntl(fd, F_SETFD, FD_CLOEXEC);
}

int safe_poll(struct pollfd *ufds, nfds_t nfds, int timeout)
{
	while (1) {
		int n = poll(ufds, nfds, timeout);
		if (n >= 0)
			return n;

		if (timeout > 0)
			timeout--;

		if (errno == EINTR)
			continue;

		if (errno == ENOMEM)
			continue;
		return n;
	}
}

ssize_t safe_read(int fd, void *buf, size_t count)
{
	ssize_t n;

	do {
		n = read(fd, buf, count);
	} while (n < 0 && errno == EINTR);

	return n;
}

void xwrite(int fd, const void *buf, size_t count)
{
	if (count)
		full_write(fd, buf, count);
}
