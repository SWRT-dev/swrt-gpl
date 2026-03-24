/* Licensed under GPLv2 or later, see file LICENSE in this source tree.*/

#include "nc_help_lite.h"
#define BUFF_SIZE 1024
#define PORT_NUM 9000
#define BACKLOG 128

int main(int argc, char **argv)
{
	int sfd, cfd = 0;
	struct pollfd pfds[2];
	char buff[BUFF_SIZE] = { '\0' };

	sfd = create_and_bind_stream_or_die(PORT_NUM);
	listen(sfd, BACKLOG);
	close_on_exec_on(sfd);

	cfd = accept(sfd, NULL, 0);
	pfds[0].fd = STDIN_FILENO;
	pfds[0].events = POLLIN;
	pfds[1].fd = cfd;
	pfds[1].events = POLLIN;

	for (;;) {
		int fdidx;
		int ofd;
		int nread;

		safe_poll(pfds, 2, -1);

		fdidx = 0;
		while (1) {
			if (pfds[fdidx].revents) {
				nread = safe_read(pfds[fdidx].fd, buff, BUFF_SIZE);
				if (fdidx != 0) {
					if (nread < 1)
						exit(EXIT_SUCCESS);
					ofd = STDOUT_FILENO;
				} else {
					if (nread < 1) {
						shutdown(cfd, SHUT_WR);
						pfds[0].fd = -1;
					}
					ofd = cfd;
				}
				xwrite(ofd, buff, nread);
			}
			if (fdidx == 1)
				break;
			fdidx++;
		}
	}
}
