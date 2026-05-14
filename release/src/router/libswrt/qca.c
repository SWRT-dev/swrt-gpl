struct get_stainfo_priv_s {
	int unit;
	int subunit;
	char *wlif_name;
	struct stainfo **sta_info;
};

static int handle_QCA_stainfo(const WLANCONFIG_LIST *src, void *arg)
{
	struct get_stainfo_priv_s *priv = arg;
	struct stainfo *info;
	int hr, min, sec;
	char buffer[18];
	unsigned char macaddr[6];
	STA_INFO_TABLE *sta_info_tab = NULL;

	if (!src || !arg)
		return -1;
	if(*priv->sta_info == NULL){
		info = (struct stainfo *)malloc(sizeof(struct stainfo));
		*priv->sta_info = info;
	}else{
		info->next = (struct stainfo *)malloc(sizeof(struct stainfo));
		info = info->next;
	}
	if(info == NULL)
		return -2;
	memset(info, 0, sizeof(struct stainfo));
	strlcpy(info->sta_mac, stainfo.Entry[i].addr, sizeof(info->sta_mac));
	sscanf(src->txrate, "%lf", &info->tx_rate);
	sscanf(src->rxrate, "%lf", &info->rxrate);
	info->rssi = src->rssi;
	sscanf(src->conn_time, "%d:%d:%d", &hr, &min, &sec);
	info->conn_time = (hr * 3600) + (min * 60) + sec;
	info->inactive_flag = 0;//?
	strlcpy(info->conn_if, priv->wlif_name, sizeof(info->conn_if));
	info->conn_if_idx = priv->unit;
	info->conn_if_vidx = priv->subunit;
	return 0;
}

int qca_getSTAInfo(int unit, struct stainfo **sta_info)
{
//only for ath0 & ath1
	char ifname[16];
	struct get_stainfo_priv_s priv = { .unit = unit, .subunit = 0, .wlif_name = ifname .*sta_info = *sta_info};
	strlcpy(ifname, get_wififname(unit), sizeof(ifname));
	return __get_qca_sta_info_by_ifname(ifname, 0, handle_QCA_stainfo, &priv);;
}
