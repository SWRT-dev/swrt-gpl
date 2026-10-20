int qca_getSTAInfo(int unit, struct stainfo **sta_info)
{
	int ret = 0, subunit, i, hr, min, sec;
	char *unit_name;
	char *p, *ifname;
	char *wl_ifnames;
	WIFI_STA_TABLE stainfo;
	struct stainfo *info, *last = NULL;
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
			if(last == NULL){
				last = *sta_info;
				while(last->next != NULL){
					info = last->next;
					last = info;
				}
			}
			last->next = (struct stainfo *)malloc(sizeof(struct stainfo));
			info = last->next;
			last = info;
		}
		if(info == NULL)
			break;
		memset(info, 0, sizeof(struct stainfo));
		info->next = NULL;
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

int get_wifi_noise(char *ifname, char *output, int len)
{
	int noise = 0;
	char cmd[64], tmp[256], *pt;
	FILE *fp = NULL;

	if(!output || !len)
		return -1;
	memset(output, 0, len);
	snprintf(cmd, sizeof(cmd), "iwconfig %s", ifname);
	fp = popen(cmd, "r");
	if(!fp)
		return -1;
	while(fgets(tmp, sizeof(tmp), fp)){
		pt = strstr(tmp, "Noise level:");
		if(pt){
			if(sscanf(pt, "Noise level: %ddBm", &noise) == 1)
				break;
		}
	}
	pclose(fp);
	snprintf(output, len, "%d", noise);
	return 0;
}

int get_wifi_mcs(char *ifname, char *output, int len)
{
	if(!output || !len)
		return -1;
	snprintf(output, len, "%d", -1);
	return 0;
}

int get_wifi_subif_info(int idx, char *output, int outputlen)
{
#if defined(RTCONFIG_HAS_5G_2) || defined(RTCONFIG_WIFI6E)
	if(idx > WL_5G_2_BAND)
#else
	if(idx > WL_5G_BAND)
#endif
		return -1;
	if(!output || !outputlen)
		return -1;
	snprintf(output, outputlen, "%d", -1);
	return 0;
}

int get_wifi_chanim(char *ifname, char *output, int outputlen)
{
	if(!output || !outputlen)
		return -1;
	snprintf(output, outputlen, "%d", -1);
	return 0;
}

int get_wifi_txop(char *ifname,int *txop, int outputlen)
{
	if(!txop)
		return -1;
	*txop = 0;
	return 0;
}

int get_wifi_glitch(char *ifname,int *glitch, int outputlen)
{
	if(!glitch)
		return -1;
	*glitch = 0;
	return 0;
}
