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

#ifndef CONFIG_SUPPORT_OPENWRT
#ifndef CONFIG_GLIBC_2_20
#if !defined(RTCONFIG_RALINK_MT7629) && !defined(RTCONFIG_RALINK_MT7622) && !defined(RTCONFIG_WLMODULE_MT7915D_AP)
#include <linux/autoconf.h>
#endif
#endif
#endif

#include "ra_ioctl.h"

#if !defined(CONFIG_SUPPORT_OPENWRT) && !defined(RTCONFIG_RALINK_MT7629) && !defined(RTCONFIG_RALINK_MT7622) && !defined(RTCONFIG_WLMODULE_MT7915D_AP)
#define ETH_DEVNAME "eth2"
#else
#define ETH_DEVNAME "eth0"
#endif

void show_usage(void)
{
#ifndef CONFIG_RT2860V2_AP_MEMORY_OPTIMIZATION
	printf("mii_mgr -g -p [phy number] -r [register number]\n");
	printf("  Get: mii_mgr -g -p 3 -r 4\n\n");
	printf("mii_mgr -s -p [phy number] -r [register number] -v [0xvalue]\n");
	printf("  Set: mii_mgr -s -p 4 -r 1 -v 0xff11\n\n");
#endif
}

int main(int argc, char *argv[])
{
	int sk, opt, ret = 0;
	char options[] = "gsp:r:v:L:G:?t";
	int method = 0;
	struct ifreq ifr;
	struct ra_mii_ioctl_data mii;

#if defined (CONFIG_RALINK_MT7628)
	struct ifreq ifr2;
	struct ra_mii_ioctl_data mii2;
	int page_select = 0;
	int method2 = 0;
#endif
	if (argc < 6) {
		show_usage();
		return 0;
	}

	sk = socket(AF_INET, SOCK_DGRAM, 0);
	if (sk < 0) {
		printf("Open socket failed\n");
		return -1;
	}

	strncpy(ifr.ifr_name,ETH_DEVNAME, 5);
	ifr.ifr_data = &mii;
#if defined (CONFIG_RALINK_MT7628)
	strncpy(ifr2.ifr_name, ETH_DEVNAME , 5);
	ifr2.ifr_data = &mii2;
#endif

	while ((opt = getopt(argc, argv, options)) != -1) {
		switch (opt) {
			case 'g':
				method = RAETH_MII_READ;
				break;
			case 's':
				method = RAETH_MII_WRITE;
				break;
			case 'p':
				mii.phy_id = strtoul(optarg, NULL, 10);
#if defined (CONFIG_RALINK_MT7628)
				mii2.phy_id = strtoul(optarg, NULL, 10);
#endif
				break;
			case 'r':
#if defined (CONFIG_RALINK_MT7621) || defined(CONFIG_MACH_MT7623)
				if(mii.phy_id == 31) {
					mii.reg_num = strtol(optarg, NULL, 16);
				} else {
					mii.reg_num = strtol(optarg, NULL, 10);
				}
#else
				mii.reg_num = strtol(optarg, NULL, 10);
#endif
				break;
			case 'L':
#if defined (CONFIG_RALINK_MT7628)
				mii2.reg_num = 31;
				mii2.val_in = (strtol(optarg, NULL, 16) << 12);
				mii2.val_in |= 0x8000;
				page_select = 1;
#endif
				break;
			case 'G':
#if defined (CONFIG_RALINK_MT7628)
				mii2.reg_num = 31;
				mii2.val_in = (strtol(optarg, NULL, 16) << 12);
				page_select = 1;
#endif
				break;

			case 'v':
				mii.val_in = strtol(optarg, NULL, 16);
				break;
			case '?':
				show_usage();
				break;
		}
	}

#if defined (CONFIG_RALINK_MT7628)
	if(page_select){
	        method2 = RAETH_MII_WRITE;
		ret = ioctl(sk, method2, &ifr2);
		if (ret < 0) {
			printf("mii_mgr: ioctl error\n");
		}
		else{
			printf("Set: phy[%d].reg[%d] = %04x\n",
							mii2.phy_id, mii2.reg_num, mii2.val_in);

		}
	}
#endif
	if ((method == RAETH_MII_READ) || (method == RAETH_MII_WRITE)){
		ret = ioctl(sk, method, &ifr);
		if (ret < 0) {
			printf("mii_mgr: ioctl error\n");
		}
		else
			switch (method) {
				case RAETH_MII_READ:
					printf("Get: phy[%d].reg[%d] = %04x\n",
							mii.phy_id, mii.reg_num, mii.val_out);
					break;
				case RAETH_MII_WRITE:
					printf("Set: phy[%d].reg[%d] = %04x\n",
							mii.phy_id, mii.reg_num, mii.val_in);
					break;
			}
	}
	close(sk);
	return ret;
}
