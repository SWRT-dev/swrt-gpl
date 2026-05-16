int qca_getSTAInfo(int unit, struct stainfo **sta_info)
{
	int ret = 0, subunit, i, hr, min, sec;
	char *unit_name;
	char *p, *ifname;
	char *wl_ifnames;
	WIFI_STA_TABLE stainfo;
	struct stainfo *info;
	memset(&stainfo, 0, sizeof(WIFI_STA_TABLE));

	unit_name = strdup(get_wififname(unit));
	if (!unit_name)
		return ret;
	wl_ifnames = strdup(nvram_safe_get("wl_ifnames"));
	if (!wl_ifnames) {
		free(unit_name);
		return ret;
	}
	p = wl_ifnames;
	while ((ifname = strsep(&p, " ")) != NULL) {
		while (*ifname == ' ') ++ifname;
		if (*ifname == 0) break;
		if(strncmp(ifname,unit_name,safe_strlen(unit_name)))
			continue;

		subunit = get_wlsubnet(unit, ifname);
		if (subunit < 0)
			subunit = 0;
		ret = get_qca_sta_info_by_ifname(ifname, subunit, &stainfo);

	}
	for(i = 0; i < stainfo.Num; i++){
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
		strlcpy(info->sta_mac, stainfo.Entry[i].addr, sizeof(info->sta_mac));
		sscanf(stainfo.Entry[i].txrate, "%lf", &info->tx_rate);
		sscanf(stainfo.Entry[i].rxrate, "%lf", &info->rx_rate);
		sscanf(stainfo.Entry[i].conn_time, "%d:%d:%d", &hr, &min, &sec);
		info->conn_time = (hr * 3600) + (min * 60) + sec;
		info->inactive_flag = 0;//?
		strlcpy(info->conn_if, unit_name, sizeof(info->conn_if));
		info->conn_if_idx = unit;
		info->conn_if_vidx = subunit;
		info->rssi = stainfo.Entry[i].rssi;
	}
	free(wl_ifnames);
	free(unit_name);
	return ret;
}
