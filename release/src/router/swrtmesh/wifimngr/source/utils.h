#ifndef UTILS_H
#define UTILS_H


unsigned char * hwaddr_aton(const char *macstr, unsigned char *mac);
char * hwaddr_ntoa(const unsigned char *mac, char *macstr);

static inline int is_zero_hwaddr(const unsigned char *mac)
{
	return (mac[0] | mac[1] | mac[2] | mac[3] | mac[4] | mac[5]) == 0;
}

int set_sighandler(int sig, void (*handler)(int));
int unset_sighandler(int sig);

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(_a)	sizeof(_a)/sizeof(_a[0])
#endif

#endif /* UTILS_H */
