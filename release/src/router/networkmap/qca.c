struct get_stainfo_priv_s {
	int unit;
	int subunit;
	char *wlif_name;
};

static int handle_QCA_stainfo(const WLANCONFIG_LIST *src, void *arg)
{
	struct get_stainfo_priv_s *priv = arg;
	char buffer[18];
	unsigned char macaddr[6];
	STA_INFO_TABLE *sta_info_tab = NULL;

	if (!src || !arg)
		return -1;

	sta_info_tab = (STA_INFO_TABLE *)calloc(1, sizeof(STA_INFO_TABLE));
	if(sta_info_tab){
		strlcpy(sta_info_tab->txrate, src->txrate, sizeof(sta_info_tab->txrate));
		strlcpy(sta_info_tab->rxrate, src->rxrate, sizeof(sta_info_tab->rxrate));
		sta_info_tab->rssi = (unsigned int)src->rssi;
		strlcpy(sta_info_tab->conn_time, src->conn_time, sizeof(sta_info_tab->conn_time));
		strlcpy(buffer, src->addr, sizeof(buffer));
		ether_atoe(buffer, macaddr);
		memcpy(sta_info_tab->mac_addr, macaddr, sizeof(sta_info_tab->mac_addr));
		sta_info_tab->wireless = priv->unit + 1;
		printf("unit=%d,ifname=%s\n",priv->unit, priv->wlif_name);
		if(g_show_sta_info && f_exists("/tmp/conn_debug"))
			printf("%s[QCA] %02X%02X%02X%02X%02X%02X %s wl:%d %d, rx %s tx %s rssi %d conn_time %s\n", "[connection log]", sta_info_tab->mac_addr[0],
				sta_info_tab->mac_addr[1], sta_info_tab->mac_addr[2], sta_info_tab->mac_addr[3], sta_info_tab->mac_addr[4], sta_info_tab->mac_addr[5],
				priv->wlif_name, priv->unit, priv->subunit, sta_info_tab->txrate, sta_info_tab->rxrate, sta_info_tab->rssi, sta_info_tab->conn_time);
		sta_info_tab->next = NULL;
		if(g_sta_info_tab == NULL){
			g_sta_info_tab = sta_info_tab;
		}else{
			sta_info_tab->next = g_sta_info_tab;
			g_sta_info_tab = sta_info_tab;
		}
		return 0;
	}
	dbg("%s: malloc failed.\n", __func__);
	return -2;
}

int QCA_stainfo(char *ifname)
{
//only for ath0 & ath1
	int unit;
	struct get_stainfo_priv_s priv = { .unit = unit, .subunit = 0, .wlif_name = ifname };
	if(g_show_sta_info && f_exists("/tmp/conn_debug"))
		_dprintf("%s[QCA] scan interface %s\n", "[connection log]", ifname);
	for(unit = 0; unit < MAX_NR_WL_IF; unit++){
		if(!strcmp(ifname, get_wififname(unit))){
			priv.unit =unit;
			return __get_qca_sta_info_by_ifname(ifname, 0, handle_QCA_stainfo, &priv);;
		}
	}
	return -1;
}
