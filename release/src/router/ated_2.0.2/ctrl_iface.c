#include "stdio.h"
#include "stdlib.h"
#include "ate.h"
#include <sys/stat.h>


#define CONFIG_CTRL_IFACE_DIR "/var/run/ated"
char *ctrl_iface_dir = CONFIG_CTRL_IFACE_DIR;

int ate_ctrl_init(const char *ifname, struct HOST_IF *host_fd)
{
	char *cfile = NULL;
	int flen;

	if (ifname == NULL)
		goto error_out;

	flen = strlen(ctrl_iface_dir) + strlen(ifname) + 2;
	cfile = malloc(flen);
	if (cfile == NULL)
		goto error_out;

	snprintf(cfile, flen, "%s/%s", ctrl_iface_dir, ifname);

	if (mkdir(ctrl_iface_dir, S_IRWXU | S_IRWXG) < 0) {
		if (errno == EEXIST) {
			ate_printf(MSG_INFO ,"Using existing control interface directory.");
		} else {
			perror("mkdir[ctrl_interface]");
			goto error_out;
		}
	}

	if (os_strlen(ctrl_iface_dir) + 1 +
	    os_strlen(ifname) >= sizeof(host_fd->local.sun_path))
		goto error_out;


	if ( (host_fd->ctrl_sock = socket(PF_UNIX, SOCK_DGRAM, 0)) < 0 )
		printf("Create cli sock failed\n");

	host_fd->local.sun_family = AF_UNIX;
	sprintf(host_fd->local.sun_path, "%s", cfile);

	if (bind(host_fd->ctrl_sock, (struct sockaddr *) &host_fd->local, sizeof(host_fd->local)) < 0) {
		if (connect(host_fd->ctrl_sock, (struct sockaddr *) &host_fd->local, sizeof(host_fd->local)) < 0) {
			ate_printf(MSG_WARNING ,"ctrl_iface exists, but does not allow connections - assuming it was left"
				   "over from forced program termination.");
			if (unlink(cfile) < 0) {
				perror("unlink[ctrl_iface]");
				ate_printf(MSG_ERROR ,"Could not unlink existing ctrl_iface socket '%s'.", cfile);
				close(host_fd->ctrl_sock);
				goto error_out;
			}
			if (bind(host_fd->ctrl_sock, (struct sockaddr *) &host_fd->local, sizeof(host_fd->local)) < 0) {
				close(host_fd->ctrl_sock);
				perror("bind(PF_UNIX)");
				goto error_out;
			}
			ate_printf(MSG_INFO ,"Successfully replaced leftover ctrl_iface socket '%s'", cfile);
		}
	}

error_out:
	if(cfile)
		free(cfile);

	return host_fd->ctrl_sock;
}

