#include <stdio.h>
#include <string.h>
#include <signal.h>

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

/* Convert "00:11:22:33:44:55" --> \x001122334455 */
unsigned char * hwaddr_aton(const char *macstr, unsigned char *mac)
{
	size_t i;

	for (i = 0; i < 6; i++) {
		int a;

		if((a = hex2byte(macstr)) < 0)
			return NULL;

		macstr += 2;
		mac[i] = a;
		if (i < 6 - 1 && *macstr++ != ':')
			return NULL;
	}
	return mac;
}

/* Convert \x001122334455 --> "00:11:22:33:44:55" */
char * hwaddr_ntoa(const unsigned char *mac, char *macstr)
{
	sprintf(macstr, "%02x:%02x:%02x:%02x:%02x:%02x",
			mac[0]&0xff, mac[1]&0xff,
			mac[2]&0xff, mac[3]&0xff,
			mac[4]&0xff, mac[5]&0xff);

	return macstr;
}

/* install a signal handler */
int set_sighandler(int sig, void (*handler)(int))
{
	struct sigaction sa;

	memset(&sa, 0, sizeof(sa));
	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask);
	sa.sa_handler = handler;
	if (sigaction(sig, &sa, NULL) < 0) {
		wifimngr_err("Error sigaction %d\n", sig);
		return -1;
	}

	return 0;
}

/* uninstall a signal handler */
int unset_sighandler(int sig)
{
	struct sigaction sa;

	memset(&sa, 0, sizeof(sa));
	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask);
	sa.sa_handler = SIG_DFL;

	return sigaction(sig, &sa, NULL);
}
