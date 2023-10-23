#include <poll.h>
#include <stdlib.h>
#include <unistd.h>
#include <stddef.h>
#include <arpa/inet.h>

typedef struct len_and_sockaddr {
    socklen_t len;
    union {
        struct sockaddr sa;
        struct sockaddr_in sin;
    } u;
} len_and_sockaddr;

enum {
    LSA_LEN_SIZE = offsetof(len_and_sockaddr, u),
    LSA_SIZEOF_SA = sizeof(
        union {
            struct sockaddr sa;
            struct sockaddr_in sin;
        }
    )
};

int create_and_bind_stream_or_die(int port);

void close_on_exec_on(int fd);

int safe_poll(struct pollfd *ufds, nfds_t nfds, int timeout);

ssize_t safe_read(int fd, void *buf, size_t count);

void xwrite(int fd, const void *buf, size_t count);
