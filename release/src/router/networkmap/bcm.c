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

static char *ratetostr(uint32_t rate, char *buf, int len)
{
	if(buf == NULL)
		return NULL;
	if(rate == -1){
		strcpy(buf, "        ");
		return buf;
	}
	snprintf(buf, len, "%dM", rate / 1000);
	return buf;
}

int BCM_stainfo(void)
{
	int unit = 0, count;
	char word[128], prefix[] = "wlXXXXXXXXXX_", subifname[] = "wlX.Y_XXXXXXXXXX", tmp[128];
	char *next, *ifname;
	struct maclist *clientlist;
	int max_sta_count, maclist_size;
	STA_INFO_TABLE *sta_info_tab = NULL;
	sta_info_t *sta = NULL;
	scb_val_t scb_val;
	int hr, min, sec;
	foreach(word, nvram_safe_get("wl_ifnames"), next){
		snprintf(prefix, sizeof(prefix), "wl%d_", unit);
		ifname = nvram_safe_get(strcat_r(prefix, "ifname", tmp));
		if(nvram_get_int("sw_mode") == 2 && nvram_get_int("wlc_band") == unit){
			snprintf(subifname, sizeof(subifname), "wl%d.%d", unit, 1);
			ifname = subifname;
		}
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
								sta_info_tab = (STA_INFO_TABLE *)calloc(1, sizeof(STA_INFO_TABLE));
								if(sta_info_tab == NULL){
									printf("%s: calloc failed.\n", __func__);
									free(clientlist);
									return 0;
								}
								memcpy(sta_info_tab->mac_addr, &clientlist->ea[0], ETHER_ADDR_LEN);
								sta_info_tab->wireless = unit + 1;
								memcpy(&scb_val.ea, &clientlist->ea[0], ETHER_ADDR_LEN);
								if(wl_ioctl(ifname, WLC_GET_RSSI, &scb_val, sizeof(scb_val_t)))
									sta_info_tab->rssi = 0;
								else
									sta_info_tab->rssi = scb_val.val;
								ratetostr(sta->tx_rate, sta_info_tab->txrate, sizeof(sta_info_tab->txrate));
								ratetostr(sta->rx_rate, sta_info_tab->rxrate, sizeof(sta_info_tab->rxrate));
								hr = (sta->in) / 3600;
								min = (sta->in) % 3600 / 60;
								sec = sta->in - hr * 3600 - min * 60;
								snprintf(sta_info_tab->conn_time, sizeof(sta_info_tab->conn_time), "%02d:%02d:%02d", hr, min, sec);
								if(g_show_sta_info && f_exists("/tmp/conn_debug"))
									printf("%s[%3d,BCM] %02X%02X%02X%02X%02X%02Xwl:%d %d, rx %s tx %s rssi %d conn_time %s\n", "[connection log]", count,
										sta_info_tab->mac_addr[0], sta_info_tab->mac_addr[1], sta_info_tab->mac_addr[2], sta_info_tab->mac_addr[3],
										sta_info_tab->mac_addr[4], sta_info_tab->mac_addr[5], sta_info_tab->wireless, unit, sta_info_tab->rxrate,
										sta_info_tab->txrate, sta_info_tab->rssi, sta_info_tab->conn_time);
								sta_info_tab->next = NULL;
								if(g_sta_info_tab == NULL){
									g_sta_info_tab = sta_info_tab;
								}else{
									sta_info_tab->next = g_sta_info_tab;
									g_sta_info_tab = sta_info_tab;
								}
							}
						}
					}
				}
				free(clientlist);
			}
		}
		unit++;
	}
	return 0;
}
