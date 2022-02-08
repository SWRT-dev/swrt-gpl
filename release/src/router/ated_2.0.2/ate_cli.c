#include "precomp.h"

int ate_debug_level = MSG_MSGDUMP; /* default : ate_debug_level == 2 */
static int signup_flag = 1;
static struct HOST_IF *host_fd;
static char driver_ifname[IFNAMSIZ];	/* Backward competibilty */

#define CMD_BUF_LEN 4096
static char cmdbuf[CMD_BUF_LEN];
static int cmdbuf_pos = 0;
static const char *ps2 = NULL;

#ifdef DBG
/**
 * ate_printf - conditional printf
 * @level: priority level (MSG_*) of the message
 * @fmt: printf format string, followed by optional arguments
 *
 * This function is used to print conditional debugging and error messages.
 *
 * Note: New line '\n' is added to the end of the text when printing to stdout.
 */
void ate_printf(int level, char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);

	if (level >= ate_debug_level) {
		vprintf(fmt, ap);
		printf("\n");
	}

	va_end(ap);
}
#else /* DBG */
#define ate_printf(args...) do { } while (0)
#endif /* DBG */

void signup(int signum)
{
	int count;
	//printf("===>%s, interface (%s), signum: %d\n", __func__, driver_ifname, signum);

	/* It's time to terminate myself. */
	switch (signum) {
	case SIGTERM:
		if (signup_flag == 1) {
			/* Prepare Leave, free malloc */
			signup_flag = 0;

			if (host_fd) {
				close(host_fd->ctrl_sock);
				//printf("%d, free host_fd\n", getpid());
				free(host_fd);
				host_fd = NULL;
			}
		} else
			//printf("Signup_flag is already 0\n");

		break;

	default:
		//printf("Do nothing, %d\n", signum);
		break;
	}
}


void init_signals(void)
{
	struct sigaction sa;

	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask);
	sigaddset(&sa.sa_mask, SIGTERM);
	sa.sa_handler = signup;
}


int main(int argc, char *argv[])
{
	int n, count, max_fd;
	fd_set readfds;
	unsigned char cmd[30];
	socklen_t fromlen = sizeof(host_fd->local);

	os_memcpy(driver_ifname, "ra0", 4);	/*Act as old agent*/

	host_fd = malloc(sizeof(*host_fd));
	ate_printf(MSG_INFO, "%d, malloc host_fd\n", getpid());

	ate_ctrl_init(driver_ifname, host_fd);

	while(signup_flag) {
		FD_ZERO(&readfds);
		FD_SET(STDIN_FILENO, &readfds);

		if(STDIN_FILENO > max_fd)
			max_fd = STDIN_FILENO+1;

		count = select(max_fd, &readfds, NULL, NULL, NULL);

		if (count < 0) {
			ate_printf(MSG_WARNING, "select failed():");
			continue;
		} else if (count == 0) {
			continue;
			/* usleep(1000); */
		} else {
			if (FD_ISSET(STDIN_FILENO, &readfds)) {
				int c;
				unsigned char buf[1];
				int res;

				res = read(STDIN_FILENO, buf, 1);
				if (res < 0)
					perror("read");
				if (res <= 0) {
					perror("Unknown error");
					break;
				}
				c = buf[0];

				if (c == '\r' || c == '\n') {
					cmdbuf[cmdbuf_pos] = '\0';
					//edit_cmd_cb(edit_cb_ctx, cmdbuf);
					printf("%s> ", ps2 ? ps2 : "", cmdbuf);
					cmdbuf_pos = 0;
					fflush(stdout);

					if (strcmp(cmdbuf, "quit") == 0)
						signup(SIGTERM);
					else
						send(host_fd->ctrl_sock, cmdbuf, strlen(cmdbuf), 0);
					continue;
				}

				if (c >= 32 && c <= 255) {
					if (cmdbuf_pos < (int) sizeof(cmdbuf) - 1) {
						cmdbuf[cmdbuf_pos++] = c;
					}
				}
			}
		}
	}

	return 0;
}
