#ifndef dtoh16
#define dtoh16(i) (uint16)(i)
#endif

static sta_info_t *wl_sta_info(char *ifname, struct ether_addr *ea)
{
	static char buf[sizeof(sta_info_t)];
	sta_info_t *sta = NULL;

	strcpy(buf, "sta_info");
	memcpy(buf + strlen(buf) + 1, (void *)ea, ETHER_ADDR_LEN);

	if(wl_ioctl(ifname, WLC_GET_VAR, buf, sizeof(buf)))
		return NULL;
	sta = (sta_info_t *)buf;
	sta->ver = dtoh16(sta->ver);

	/* Report unrecognized version */
	if (sta->ver > WL_STA_VER) {
		dbg(" ERROR: unknown driver station info version %d\n", sta->ver);
		return NULL;
	}
	return sta;
}

int brcm_getSTAInfo(int unit, struct stainfo **sta_info)
{
	int unit = 0, count;
	char prefix[] = "wlXXXXXXXXXX_", subifname[] = "wlX.Y_XXXXXXXXXX", tmp[128];
	char *ifname;
	struct maclist *clientlist;
	struct stainfo *info;
	int max_sta_count, maclist_size;
	sta_info_t *sta = NULL;
	scb_val_t scb_val;
	int hr, min, sec;

	ifname = subifname;
	snprintf(prefix, sizeof(prefix), "wl%d_", unit);
	strlcpy(subifname, nvram_pf_safe_get(prefix, "ifname"), sizeof(subifname));
	if(nvram_get_int("sw_mode") == 2 && nvram_get_int("wlc_band") == unit)
		snprintf(subifname, sizeof(subifname), "wl%d.%d", unit, 1);

	if(*ifname){
		/* buffers and length */
		max_sta_count = 128;
		maclist_size = sizeof(clientlist->count) + max_sta_count * sizeof(struct ether_addr);
		clientlist = malloc(maclist_size);
		if(clientlist){
			memset(clientlist, 0, maclist_size);
			strcpy((char*)clientlist, "authe_sta_list");
			if (!wl_ioctl(ifname, WLC_GET_VAR, clientlist, maclist_size)){
				if(clientlist->count){
					for(count = 0; count < clientlist->count; count++){
						sta = wl_sta_info(ifname, &clientlist->ea[0]);
						if(sta && ((sta->flags & 0x10) != 0 || sta->in)){
							if(*sta_info == NULL){
								info = (struct stainfo *)malloc(sizeof(struct stainfo));
								*sta_info = info;
							}else{
								info->next = (struct stainfo *)malloc(sizeof(struct stainfo));
								info = info->next;
							}
							if(info == NULL)
								break;
							memset(info, 0, sizeof(struct stainfo));
							memset(tmp, 0, sizeof(tmp));
							ether_etoa(&clientlist->ea[0], tmp);
							strlcpy(info->sta_mac, tmp, sizeof(info->sta_mac));
							info->tx_rate = sta->tx_rate < 0 ? 0 : (double)sta->tx_rate / 1000;
							info->rx_rate = sta->rx_rate < 0 ? 0 : (double)sta->rx_rate / 1000;
							info->conn_time = sta->in;
							info->inactive_flag = 0;//?
							strlcpy(info->conn_if, ifname, sizeof(info->conn_if));
							info->conn_if_idx = unit;
							info->conn_if_vidx = 0;
							memcpy(&scb_val.ea, &clientlist->ea[0], ETHER_ADDR_LEN);
							if(wl_ioctl(ifname, WLC_GET_RSSI, &scb_val, sizeof(scb_val_t)))
								info->rssi = 0;
							else
								info->rssi = scb_val.val;
						}
					}
				}
			}
			free(clientlist);
		}
	}
	return 0;
}
