#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <inttypes.h>
#include <ctype.h>
#include <errno.h>
#include <shared.h>
#include <json.h>
#include <disk_initial.h>
#include "libswrt.h"


int GetPhyStatus(int verbose, phy_info_list *list)
{
#if defined(RTCONFIG_RALINK)
#if defined(RTCONFIG_NEW_PHYMAP) || defined(RTCONFIG_MT798X)
	ATE_port_status(verbose, list);
#elif defined(RTCONFIG_RALINK_MT7621) || defined(RTCONFIG_RALINK_EN7561)
	ATE_mt7621_esw_port_status();
#endif
#elif defined(RTCONFIG_QCA)
	ATE_port_status(verbose, list);
#elif defined(RTCONFIG_BCMARM)
#ifdef RTCONFIG_NEW_PHYMAP
	char cap_buf[64] = {0};
	if (list)
		list->count = 0;
	phy_port_mapping port_mapping;
	get_phy_port_mapping(&port_mapping);
#endif
#if defined(RTCONFIG_HND_ROUTER_AX_675X)
#elif defined(RTCONFIG_HND_ROUTER)
	int i, j = 1, ret = 0, model = get_model();
	unsigned int regv = 0, regv2 = 0, pmdv = 0, pmdv2 = 0;
	char out_buf[64], *speed;
    int extra_p0 = 0, ext_lret = 0, lret = 0, wanlan_staus = 0;
    int allports = 5;
#if defined(RTAX88U) || defined(GTAC5300)
	allports = 9;
#elif defined(GTAX11000)
	allports = 6;
#endif
	switch(model){
		case MODEL_RTAX88U:
			/*
				7 3 2 1 0 s3 s2 s1 s0	W0 L1 L2 L3 L4 L5 L6 L7 L8
	 		 */
			extra_p0 = S_53134;
			ports[0]=7; ports[1]=3; ports[2]=2; ports[3]=1; ports[4]=0;
			ports[5]=3+extra_p0; ports[6]=2+extra_p0; ports[7]=1+extra_p0; ports[8]=extra_p0;
			regv = hnd_ethswctl(REGACCESS, 0x0100, 2, 0, 0);
			regv2 = hnd_ethswctl(REGACCESS, 0x0104, 4, 0, 0);
			ext_lret = hnd_ethswctl(REGACCESS, 0x0108, 2, 0, 0);
			break;
		case MODEL_RTAC86U:
			/* WAN L4 L3 L2 L1 */
			ports[0]=7; ports[1]=3; ports[2]=2; ports[3]=1; ports[4]=0;
			regv = hnd_ethswctl(REGACCESS, 0x0100, 2, 0, 0);
			regv2 = hnd_ethswctl(REGACCESS, 0x0104, 4, 0, 0);
			ext_lret = hnd_ethswctl(REGACCESS, 0x0108, 2, 0, 0);
			break;
		case MODEL_GTAC5300:
			/*
				  1 0 s3 s2	   L1 L2 L3 L4
				7 3 2 s1 s0	W0 L5 L6 L7 L8
	 		 */
			extra_p0 = S_53134;
			ports[0]=7; ports[1]=1; ports[2]=0; ports[3]=3+extra_p0; ports[4]=2+extra_p0;
			ports[5]=3; ports[6]=2; ports[7]=1+extra_p0; ports[8]=extra_p0;
			regv = hnd_ethswctl(REGACCESS, 0x0100, 2, 0, 0);
			regv2 = hnd_ethswctl(REGACCESS, 0x0104, 4, 0, 0);
			ext_lret = hnd_ethswctl(REGACCESS, 0x0108, 2, 0, 0);
			break;
		case MODEL_GTAX11000:
			/*
				7 4 3 2 1 0 	L5(2.5G) W0 L1 L2 L3 L4
			*/
			ports[0]=4; ports[1]=3; ports[2]=2; ports[3]=1; ports[4]=0;
			ports[5]=7;
			regv = hnd_ethswctl(REGACCESS, 0x0100, 2, 0, 0);
			regv2 = hnd_ethswctl(REGACCESS, 0x0104, 4, 0, 0);
			ext_lret = hnd_ethswctl(REGACCESS, 0x0108, 2, 0, 0);
			break;
		case MODEL_RTAX92U:
			/* WAN L4 L3 L2 L1 */
			ports[0]=7; ports[1]=3; ports[2]=2; ports[3]=1; ports[4]=0;
			regv = hnd_ethswctl(REGACCESS, 0x0100, 2, 0, 0) & 0xf;
			regv2 = hnd_ethswctl(REGACCESS, 0x0104, 4, 0, 0);
			ext_lret = hnd_ethswctl(REGACCESS, 0x0108, 2, 0, 0) & 0xf;
			break;
	}
    memset(out_buf, 0, sizeof(out_buf));
	if(list) {
		list->count = 0;
	}
    for (i=0; i < allports; i++) {
		if(list) {
			++list->count;
			list->phy_info[i].phy_port_id = ports[i];
			if (i == 0) {
				snprintf(list->phy_info[i].label_name, sizeof(list->phy_info[i].label_name), "W0");
				snprintf(list->phy_info[i].cap_name, sizeof(list->phy_info[i].cap_name), "wan");
			} else {
				snprintf(list->phy_info[i].label_name, sizeof(list->phy_info[i].label_name), "L%d", i);
				snprintf(list->phy_info[i].cap_name, sizeof(list->phy_info[i].cap_name), "lan");
			}
		}
		if (hnd_get_phy_status(ports[i], extra_p0, regv, pmdv) == 0){ /*Disconnect*/
			if (i==0) {
				snprintf(out_buf, sizeof(out_buf), "W0=X;");
			} else {
				sprintf(out_buf, "%sL%d=X;", out_buf, i);
			}
			if(list) {
				snprintf(list->phy_info[i].state, sizeof(list->phy_info[i].state), "down");
				list->phy_info[i].link_rate = 0;
				snprintf(list->phy_info[i].duplex, sizeof(list->phy_info[i].duplex), "none");
			}
        }
		else { /*Connect, keep check speed*/
			ret = hnd_get_phy_speed(ports[i], extra_p0, regv2, pmdv2) &;
			if (i == 0) {
				sprintf(out_buf, "W0=%s;", ret ? "G":"M");
				wanlan_staus = 1;
			} else {
				sprintf(out_buf, "%sL%d=%s;", out_buf, i, ret ? "G":"M");
				wanlan_staus |= 1 << i;
			}
			if(list) {
				snprintf(list->phy_info[i].state, sizeof(list->phy_info[i].state), "up");
				list->phy_info[i].link_rate = ret ? 1000 : 100;
				if(hnd_get_phy_duplex(ports[i], extra_p0, ext_lret, pmdv2))
					snprintf(list->phy_info[i].duplex, sizeof(list->phy_info[i].duplex), "full");
				else
					snprintf(list->phy_info[i].duplex, sizeof(list->phy_info[i].duplex), "half");
				list->phy_info[i].tx_bytes = hnd_get_phy_mib(ports[i], extra_p0, "tx_bytes");
				list->phy_info[i].rx_bytes = hnd_get_phy_mib(ports[i], extra_p0, "rx_bytes");
				list->phy_info[i].tx_packets = hnd_get_phy_mib(ports[i], extra_p0, "tx_packets");
				list->phy_info[i].rx_packets = hnd_get_phy_mib(ports[i], extra_p0, "rx_packets");
				list->phy_info[i].crc_errors = hnd_get_phy_mib(ports[i], extra_p0, "rx_crc_errors");
			}
		}
	}
	if (verbose == 1)
		puts(out_buf);
	if (verbose == S_RTL8365MB || verbose == S_53134){
		if(extra_p0)
			return 1;
		else
			return 0;
	}

	return wanlan_staus;
#else
#ifdef RTCONFIG_NEW_PHYMAP
	int i, ret, lret = 0, mask, len = 0;
	char out_buf[30];
#if defined(RTCONFIG_EXT_RTL8365MB) || defined(RTCONFIG_EXT_RTL8370MB)
	char PStatus[5]="XXXXX";
#endif
	if(list) {
		list->count = 0;
	}
	memset(out_buf, 0, sizeof(out_buf));
	for (i = 0; i < port_mapping.count; i++) {
		// Only handle WAN/LAN ports
		if (((port_mapping.port[i].cap & PHY_PORT_CAP_WAN) == 0) && ((port_mapping.port[i].cap & PHY_PORT_CAP_LAN) == 0))
			continue;
		if(list) {
			++list->count;
			list->phy_info[i].phy_port_id = port_mapping.port[i].phy_port_id;
			snprintf(list->phy_info[i].label_name, sizeof(list->phy_info[i].label_name), "%s", 
				port_mapping.port[i].label_name);
			list->phy_info[i].cap = port_mapping.port[i].cap;
			snprintf(list->phy_info[i].cap_name, sizeof(list->phy_info[i].cap_name), "%s", 
				get_phy_port_cap_name(port_mapping.port[i].cap, cap_buf, sizeof(cap_buf)));
		}
		mask = 0;
		mask |= 0x0001 << list->phy_info[i].phy_port_id;
		if (get_phy_status(mask)==0) {/*Disconnect*/
			snprintf(list->phy_info[i].state, sizeof(list->phy_info[i].state), "%s", "down");
			len += sprintf(out_buf + len, "%s=X;", list->phy_info[i].label_name);
		} else { /*Connect, keep check speed*/
			lret |= 1 << i; 
			mask = 0;
			mask |= (0x0003 << (list->phy_info[i].phy_port_id * 2));
			ret = get_phy_speed(mask);
			ret >>= (list->phy_info[i].phy_port_id * 2);
			len += sprintf(out_buf + len, "%s=%s;", list->phy_info[i].label_name, (ret & 2)? "G":"M");
			if(list) {
				snprintf(list->phy_info[i].state, sizeof(list->phy_info[i].state), "up");
				list->phy_info[i].link_rate = (ret & 2) ? 1000 : 100;
				if(get_phy_duplex(mask))
					snprintf(list->phy_info[i].duplex, sizeof(list->phy_info[i].duplex), "full");
				else
					snprintf(list->phy_info[i].duplex, sizeof(list->phy_info[i].duplex), "half");
				list->phy_info[i].tx_bytes = get_phy_mib(port_mapping.port[i].phy_port_id, "tx_bytes");
				list->phy_info[i].rx_bytes = get_phy_mib(port_mapping.port[i].phy_port_id, "rx_bytes");
				list->phy_info[i].tx_packets = get_phy_mib(port_mapping.port[i].phy_port_id, "tx_packets");
				list->phy_info[i].rx_packets = get_phy_mib(port_mapping.port[i].phy_port_id, "rx_packets");
				list->phy_info[i].crc_errors = get_phy_mib(port_mapping.port[i].phy_port_id, "rx_crc_errors");
			}
		}
	}
	if (verbose)
#if defined(RTCONFIG_EXT_RTL8365MB) || defined(RTCONFIG_EXT_RTL8370MB)
		printf("%s", out_buf);
#else
		puts(out_buf);
#endif

#if defined(RTCONFIG_EXT_RTL8365MB) || defined(RTCONFIG_EXT_RTL8370MB)
	if (get_model() == MODEL_RTAC88U || get_model() == MODEL_RTAC5300)
		lret |= ext_rtk_phyState(verbose, PStatus);
#endif
	return lret;
#else
	int allports = 5;
	int ports[allports];
#if defined(RTCONFIG_EXT_RTL8365MB) || defined(RTCONFIG_EXT_RTL8370MB)
	int ext = 0;
#endif
	int i, ret, lret=0, model, mask;
	char out_buf[30], tmp[30];

	model = get_model();
	switch(model) {
	case MODEL_SBRAC3200P:
		/* WAN L1 L2 L3 L4 */
		ports[0]=0; ports[1]=1; ports[2]=2, ports[3]=3; ports[4]=4;
		break;
	case MODEL_RTAC3200:
		/* WAN L1 L2 L3 L4 */
		ports[0]=0; ports[1]=4; ports[2]=3, ports[3]=2; ports[4]=1;
		break;
	case MODEL_RTAC88U:
	case MODEL_RTAC3100:
		/* WAN L1 L2 L3 L4 */
		ports[0]=4; ports[1]=3; ports[2]=2; ports[3]=1; ports[4]=0;
#if defined(RTCONFIG_EXT_RTL8365MB) || defined(RTCONFIG_EXT_RTL8370MB)
		ext = 1;
#endif
		break;
	case MODEL_RTAC68U:
		/* WAN L1 L2 L3 L4 */
		ports[0]=0; ports[1]=1; ports[2]=2; ports[3]=3; ports[4]=4;
		break;
	case MODEL_RTAC5300:
		/* WAN L1 L2 L3 L4 */
		ports[0]=0; ports[1]=1; ports[2]=2; ports[3]=3; ports[4]=4;
#ifdef RTCONFIG_EXT_RTL8365MB
		ext = 1;
#endif
		break;
	case MODEL_R7000P:
		ports[0]=0; ports[1]=1; ports[2]=2; ports[3]=3; ports[4]=4;
		break;
	}

#if defined(RTCONFIG_EXT_RTL8365MB) || defined(RTCONFIG_EXT_RTL8370MB)
	char PStatus[5]="XXXXX";
#endif
	if(list) {
		list->count = 0;
	}
	memset(out_buf, 0, sizeof(out_buf));
	for (i=0; i < allports; i++) {
		if(list) {
			++list->count;
			list->phy_info[i].phy_port_id = ports[i];
			if (i==0) {
				snprintf(list->phy_info[i].label_name, sizeof(list->phy_info[i].label_name), "W0");
				snprintf(list->phy_info[i].cap_name, sizeof(list->phy_info[i].cap_name), "wan");
			} else {
				snprintf(list->phy_info[i].label_name, sizeof(list->phy_info[i].label_name), "L%d", i);
				snprintf(list->phy_info[i].cap_name, sizeof(list->phy_info[i].cap_name), "lan");
			}
		}
		mask = 0;
		mask |= 0x0001<<ports[i];
		if (get_phy_status(mask)==0) {/*Disconnect*/
			if (i==0)
				sprintf(out_buf, "W0=X;");
			else {
				sprintf(tmp, "%s", out_buf);//fix musl bug
				sprintf(out_buf, "%sL%d=X;", tmp, i);
			}
		}
		else { /*Connect, keep check speed*/
			mask = 0;
			mask |= (0x0003<<(ports[i]*2));
			ret=get_phy_speed(mask);
			ret>>=(ports[i]*2);
			if (i==0)
				sprintf(out_buf, "W0=%s;", (ret & 2)? "G":"M");
			else {
				lret = 1;
				sprintf(tmp, "%s", out_buf);
				sprintf(out_buf, "%sL%d=%s;", tmp, i, (ret & 2)? "G":"M");
			}
			if(list) {
				snprintf(list->phy_info[i].state, sizeof(list->phy_info[i].state), "up");
				list->phy_info[i].link_rate = (ret & 2) ? 1000 : 100;
				if(get_phy_duplex(mask))
					snprintf(list->phy_info[i].duplex, sizeof(list->phy_info[i].duplex), "full");
				else
					snprintf(list->phy_info[i].duplex, sizeof(list->phy_info[i].duplex), "half");
				list->phy_info[i].tx_bytes = get_phy_mib(ports[i], "tx_bytes");
				list->phy_info[i].rx_bytes = get_phy_mib(ports[i], "rx_bytes");
				list->phy_info[i].tx_packets = get_phy_mib(ports[i], "tx_packets");
				list->phy_info[i].rx_packets = get_phy_mib(ports[i], "rx_packets");
				list->phy_info[i].crc_errors = get_phy_mib(ports[i], "rx_crc_errors");
			}
		}
	}

	if (verbose)
#if defined(RTCONFIG_EXT_RTL8365MB) || defined(RTCONFIG_EXT_RTL8370MB)
		printf("%s", out_buf);
#else
		puts(out_buf);
#endif

#if defined(RTCONFIG_EXT_RTL8365MB) || defined(RTCONFIG_EXT_RTL8370MB)
	if (ext)
		lret |= ext_rtk_phyState(verbose, PStatus);
#endif
#endif
	return lret;
#endif
#endif
	return 1;
}


#if defined(RTCONFIG_USB)
#if 1
void get_usb_devices_by_usb_port(usb_device_info_t device_list[], int max_devices, int usb_port)
{
	disk_info_t *disks_info = NULL, *follow_disk;
	usb_device_info_t *device_tmp;
	int i, j;
	char prefix[32], tmp[32];
	memset(device_list, 0, sizeof(usb_device_info_t)*max_devices);

	// list storage dievices first.
	disks_info = read_disk_data();
	if (disks_info) {
		for (follow_disk = disks_info; follow_disk != NULL; follow_disk = follow_disk->next) {
			partition_info_t *follow_partition;
			char ascii_tag[PATH_MAX];
			int scan_ret, port = 0, hub_port = 0, target_idx;


			if ((scan_ret = sscanf(follow_disk->port, "%d.%d", &port, &hub_port)) == 0)
				continue;

			if (port != usb_port)
				continue;

			if (hub_port > max_devices)
				continue;

			target_idx = (scan_ret == 1 ? 0 : hub_port);
			target_idx = target_idx > 0 ? target_idx - 1 : 0;
			device_tmp = &device_list[target_idx];

			/* usb path */
			snprintf(device_tmp->usb_path, sizeof(device_tmp->usb_path), "%d", port);

			/* node */
			snprintf(device_tmp->node, sizeof(device_tmp->node), "%s", follow_disk->port);

			/* type */
			snprintf(device_tmp->type, sizeof(device_tmp->type), "storage");

			snprintf(prefix, sizeof(prefix), "usb_path%s", device_tmp->node);

			/* manufacturer */
			snprintf(device_tmp->manufacturer, sizeof(device_tmp->manufacturer), "%s", 
				nvram_safe_get(strlcat_r(prefix, "_manufacturer", tmp, sizeof(tmp))));

			/* product */
			snprintf(device_tmp->product, sizeof(device_tmp->product), "%s", 
				nvram_safe_get(strlcat_r(prefix, "_product", tmp, sizeof(tmp))));

			/* serial */
			snprintf(device_tmp->serial, sizeof(device_tmp->serial), "%s", 
				nvram_safe_get(strlcat_r(prefix, "_serial", tmp, sizeof(tmp))));

			/* device name */
			memset(ascii_tag, 0, PATH_MAX);
			char_to_ascii_safe(ascii_tag, follow_disk->tag, PATH_MAX);
			snprintf(device_tmp->device_name, sizeof(device_tmp->device_name), "%s", follow_disk->tag);

			/* speed */
			device_tmp->speed = nvram_get_int(strlcat_r(prefix, "_speed", tmp, sizeof(tmp)));

			/* go through all partitions for caclulating size */
			for (follow_partition = follow_disk->partitions; follow_partition != NULL; follow_partition = follow_partition->next) {
				/* total size */
				device_tmp->storage_size_in_kilobytes += follow_partition->size_in_kilobytes;
				/* total used size */
				device_tmp->storage_used_in_kilobytes += follow_partition->used_kilobytes;
			}
		}

		free_disk_data(&disks_info);
	}

#if defined(RTCONFIG_USB_MODEM) || defined(RTCONFIG_USB_PRINTER)
	// list modem and printer dievices.
	for(i = 1; i <= MAX_USB_PORT; ++i) {
		if (i != usb_port)
			continue;
		snprintf(prefix, sizeof(prefix), "usb_path%d", i);
		if (nvram_get(prefix)) { // not usb hub
			device_tmp = &device_list[0];
			//memset(device_tmp, 0, sizeof(usb_device_info_t));

			/* usb path */
			snprintf(device_tmp->usb_path, sizeof(device_tmp->usb_path), "%d", i);
			/* node */
			snprintf(device_tmp->node, sizeof(device_tmp->node), "%d", i);

			if (
#if defined(RTCONFIG_USB_MODEM)
				nvram_match(prefix, "modem") ||
#endif
#if defined(RTCONFIG_USB_PRINTER)
				nvram_match(prefix, "printer") ||
#endif
				0) {
				/* type */
				snprintf(device_tmp->type, sizeof(device_tmp->type), "%s", nvram_safe_get(prefix));
				/* manufacturer */
				snprintf(device_tmp->manufacturer, sizeof(device_tmp->manufacturer), "%s", nvram_safe_get(strcat_r(prefix, "_manufacturer", tmp)));
				/* product */
				snprintf(device_tmp->product, sizeof(device_tmp->product), "%s", nvram_safe_get(strcat_r(prefix, "_product", tmp)));
				/* serial */
				snprintf(device_tmp->serial, sizeof(device_tmp->serial), "%s", nvram_safe_get(strcat_r(prefix, "_serial", tmp)));
				/* speed */
				device_tmp->speed = nvram_get_int(strlcat_r(prefix, "_speed", tmp, sizeof(tmp)));
			}

			for(j = 2; j <= MAX_USB_HUB_PORT; ++j) {  // fill rest reserved hub port.
				snprintf(prefix, sizeof(prefix), "usb_path%d.%d", i, j);
				device_tmp = &device_list[j-1];
				/* usb path */
				snprintf(device_tmp->usb_path, sizeof(device_tmp->usb_path), "%d", i);
				/* node */
				snprintf(device_tmp->node, sizeof(device_tmp->node), "%d.%d", i, j);
			}
		} else { // usb hub
			for(j = 1; j <= MAX_USB_HUB_PORT; ++j) {
				snprintf(prefix, sizeof(prefix), "usb_path%d.%d", i, j);
				device_tmp = &device_list[j-1];
				//memset(device_tmp, 0, sizeof(usb_device_info_t));

				/* usb path */
				snprintf(device_tmp->usb_path, sizeof(device_tmp->usb_path), "%d", i);
				/* node */
				snprintf(device_tmp->node, sizeof(device_tmp->node), "%d.%d", i, j);

				if (
#if defined(RTCONFIG_USB_MODEM)
					nvram_match(prefix, "modem") ||
#endif
#if defined(RTCONFIG_USB_PRINTER)
					nvram_match(prefix, "printer") ||
#endif
					0) {
					/* type */
					snprintf(device_tmp->type, sizeof(device_tmp->type), "%s", nvram_safe_get(prefix));
					/* manufacturer */
					snprintf(device_tmp->manufacturer, sizeof(device_tmp->manufacturer), "%s", nvram_safe_get(strcat_r(prefix, "_manufacturer", tmp)));
					/* product */
					snprintf(device_tmp->product, sizeof(device_tmp->product), "%s", nvram_safe_get(strcat_r(prefix, "_product", tmp)));
					/* serial */
					snprintf(device_tmp->serial, sizeof(device_tmp->serial), "%s", nvram_safe_get(strcat_r(prefix, "_serial", tmp)));
					/* speed */
					device_tmp->speed = nvram_get_int(strlcat_r(prefix, "_speed", tmp, sizeof(tmp)));
				}
			}
		}
	}
#endif
}
#else

// reference to ej_get_usb_info of httpd/web.c
void get_usb_devices(usb_device_info_t **device_list)
{
	disk_info_t *disks_info = NULL, *follow_disk;
	usb_device_info_t **device_list_tmp;
	int i, j;
	char prefix[32], tmp[32], usb_path_tmp[16];

	*device_list = NULL;
	device_list_tmp = device_list;

	// list storage dievices first.
	disks_info = read_disk_data();
	if (disks_info) {
		for (follow_disk = disks_info; follow_disk != NULL; follow_disk = follow_disk->next) {
			partition_info_t *follow_partition;
			char ascii_tag[PATH_MAX];

			*device_list_tmp = (usb_device_info_t *)malloc(sizeof(usb_device_info_t));

			if(*device_list_tmp) {
				memset((*device_list_tmp), 0, sizeof(usb_device_info_t));

				/* usb path */
				memset(usb_path_tmp, 0, sizeof(usb_path_tmp));
				snprintf(usb_path_tmp, sizeof(usb_path_tmp), "%c", follow_disk->port[0]);
				(*device_list_tmp)->usb_path = strdup(usb_path_tmp);

				/* tmpdisk.node */
				(*device_list_tmp)->node = strdup(follow_disk->port);

				/* tmpdisk.deviceType */
				(*device_list_tmp)->type = strdup("storage");

				snprintf(prefix, sizeof(prefix), "usb_path%s", (*device_list_tmp)->node);
				/* manufacturer */
				(*device_list_tmp)->manufacturer = strdup(nvram_safe_get(strlcat_r(prefix, "_manufacturer", tmp, sizeof(tmp))));

				/* product */
				(*device_list_tmp)->product = strdup(nvram_safe_get(strlcat_r(prefix, "_product", tmp, sizeof(tmp))));

				/* serial */
				(*device_list_tmp)->serial = strdup(nvram_safe_get(strlcat_r(prefix, "_serial", tmp, sizeof(tmp))));

				/* device name */
				memset(ascii_tag, 0, PATH_MAX);
				char_to_ascii_safe(ascii_tag, follow_disk->tag, PATH_MAX);
				(*device_list_tmp)->device_name = strdup(ascii_tag);

				/* go through all partitions for caclulating size */
				for (follow_partition = follow_disk->partitions; follow_partition != NULL; follow_partition = follow_partition->next) {
					/* total size */
					(*device_list_tmp)->storage_size_in_kilobytes += follow_partition->size_in_kilobytes;
					/* total used size */
					(*device_list_tmp)->storage_used_in_kilobytes += follow_partition->used_kilobytes;
				}
				while ((*device_list_tmp)) {
					device_list_tmp = &((*device_list_tmp)->next);
				}
			}
		}

		free_disk_data(&disks_info);
	}

	// list modem and printer dievices.
	for(i = 1; i <= MAX_USB_PORT; ++i){
		snprintf(prefix, sizeof(prefix), "usb_path%d", i);
		if (nvram_get(prefix)) { // not usb hub
			if (nvram_match(prefix, "modem") || nvram_match(prefix, "printer")) {

				*device_list_tmp = (usb_device_info_t *)malloc(sizeof(usb_device_info_t));
				if (*device_list_tmp) {
					memset((*device_list_tmp), 0, sizeof(usb_device_info_t));

					/* usb path */
					snprintf(usb_path_tmp, sizeof(usb_path_tmp), "%d", i);
					(*device_list_tmp)->usb_path = strdup(usb_path_tmp);

					/* tmpdisk.node */
					(*device_list_tmp)->node = strdup(usb_path_tmp);

					/* tmpdisk.deviceType */
					(*device_list_tmp)->type = strdup(nvram_safe_get(prefix));
					(*device_list_tmp)->manufacturer = strdup(nvram_safe_get(strcat_r(prefix, "_manufacturer", tmp)));
					(*device_list_tmp)->product = strdup(nvram_safe_get(strcat_r(prefix, "_product", tmp)));
					(*device_list_tmp)->serial = strdup(nvram_safe_get(strcat_r(prefix, "_serial", tmp)));
					(*device_list_tmp)->device_name = strdup("");

					while ((*device_list_tmp))
						device_list_tmp = &((*device_list_tmp)->next);
				}
			}
		} else { // usb hub
			for(j = 1; j <= MAX_USB_HUB_PORT; ++j){
				snprintf(prefix, sizeof(prefix), "usb_path%d.%d", i, j);

				if (nvram_match(prefix, "modem") || nvram_match(prefix, "printer")) {

					*device_list_tmp = (usb_device_info_t *)malloc(sizeof(usb_device_info_t));
					if (*device_list_tmp) {
						memset((*device_list_tmp), 0, sizeof(usb_device_info_t));

						/* usb path */
						snprintf(usb_path_tmp, sizeof(usb_path_tmp), "%d", i);
						(*device_list_tmp)->usb_path = strdup(usb_path_tmp);

						/* tmpdisk.node */
						snprintf(usb_path_tmp, sizeof(usb_path_tmp), "%d.%d", i, j);
						(*device_list_tmp)->node = strdup(usb_path_tmp);

						/* tmpdisk.deviceType */
						(*device_list_tmp)->type = strdup(nvram_safe_get(prefix));
						(*device_list_tmp)->manufacturer = strdup(nvram_safe_get(strcat_r(prefix, "_manufacturer", tmp)));
						(*device_list_tmp)->product = strdup(nvram_safe_get(strcat_r(prefix, "_product", tmp)));
						(*device_list_tmp)->serial = strdup(nvram_safe_get(strcat_r(prefix, "_serial", tmp)));
						(*device_list_tmp)->device_name = strdup("");

						while ((*device_list_tmp))
							device_list_tmp = &((*device_list_tmp)->next);
					}
				}
			}
		}
	}
}

void free_usb_devices(usb_device_info_t **device_list) {
	usb_device_info_t *device_list_tmp, *device_list_old;
	device_list_tmp = *device_list;
	while (device_list_tmp) {
		if (device_list_tmp->usb_path)
			free(device_list_tmp->usb_path);
		if (device_list_tmp->node)
			free(device_list_tmp->node);
		if (device_list_tmp->type)
			free(device_list_tmp->type);
		if (device_list_tmp->manufacturer)
			free(device_list_tmp->manufacturer);
		if (device_list_tmp->product)
			free(device_list_tmp->product);
		if (device_list_tmp->serial)
			free(device_list_tmp->serial);
		if (device_list_tmp->device_name)
			free(device_list_tmp->device_name);

		device_list_old = device_list_tmp;
		device_list_tmp = device_list_tmp->next;
		free(device_list_old);
	}
}
#endif
#endif
