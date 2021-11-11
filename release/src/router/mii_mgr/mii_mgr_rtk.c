#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <linux/if.h>
#include <linux/mii.h>
#include <linux/types.h>
#include <unistd.h>

#include "ra_ioctl.h"

#ifndef CONFIG_SUPPORT_OPENWRT
#define ETH_DEVNAME "eth2"
#else
#define ETH_DEVNAME "eth0"
#endif

void show_usage(void)
{
	printf("mii_mgr -g -p [phy number] -r [register number]\n");
	printf("  Get: mii_mgr -g -p 3 -r 4\n\n");
	printf("mii_mgr -s -p [phy number] -r [register number] -v [0xvalue]\n");
	printf("  Set: mii_mgr -s -p 4 -r 1 -v 0xff11\n\n");
}

int main(int argc, char *argv[])
{
	int sk, opt, ret = 0;
	char options[] = "gsp:r:v:L:G:?t";
	struct ifreq ifr;
	struct ra_switch_ioctl_data data;

	if (argc < 6) {
		show_usage();
		return 0;
	}

	sk = socket(AF_INET, SOCK_DGRAM, 0);
	if (sk < 0) {
		printf("Open socket failed\n");
		return -1;
	}

	strncpy(ifr.ifr_name, ETH_DEVNAME , 5);
	ifr.ifr_data = &data;

	while ((opt = getopt(argc, argv, options)) != -1) {
		switch (opt) {
			case 'g':
				data.cmd = SW_IOCTL_GET_PHY_REG;
				break;
			case 's':
				data.cmd = SW_IOCTL_SET_PHY_REG;
				break;
			case 'p':
				data.port = strtoul(optarg, NULL, 10);
				break;
			case 'r':
				data.reg_addr = strtol(optarg, NULL, 10);
				break;
			case 'v':
				data.reg_val = strtol(optarg, NULL, 16);
				break;
			case '?':
				show_usage();
				break;
		}
	}

	if ((data.cmd == SW_IOCTL_GET_PHY_REG) || (data.cmd == SW_IOCTL_SET_PHY_REG)){
		ret = ioctl(sk, RAETH_SW_IOCTL, &ifr);
		if (ret < 0) {
			printf("mii_mgr: ioctl error\n");
		}
	}
	close(sk);
	return ret;
}
