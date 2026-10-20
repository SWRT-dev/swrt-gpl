int lantiq_getSTAInfo(int unit, struct stainfo **sta_info)
{
	int ret = 0, subunit, i, hr, min, sec;
	char *ifname;
	char prefix[] = "wlXXXXXXXXXX_", subifname[] = "wlX.Y_XXXXXXXXXX";
	WIFI_STA_TABLE stainfo;
	struct stainfo *info, *last = NULL;

	memset(&stainfo, 0, sizeof(WIFI_STA_TABLE));
	ifname = subifname;
	if((repeater_mode() || wisp_mode()) && nvram_get_int("wlc_band") == unit)
		snprintf(prefix, sizeof(prefix), "wl%d.%d_", unit, 1);
	else
		snprintf(prefix, sizeof(prefix), "wl%d_", unit);
	strlcpy(subifname, nvram_pf_safe_get(prefix, "ifname"), sizeof(subifname));

	if(*ifname){
		subunit = get_wlsubnet(unit, ifname);
		if (subunit < 0)
			subunit = 0;
		ret = get_lantiq_sta_info_by_ifname(ifname, subunit, &stainfo);
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
			strlcpy(info->conn_if, ifname, sizeof(info->conn_if));
			info->conn_if_idx = unit;
			info->conn_if_vidx = subunit;
			info->rssi = stainfo.Entry[i].rssi;
		}
	}
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

/*
DirectConnect related statistic (per HW)

     64205 total packets received
     13340 total packets transmitted
  58564516 total bytes received
   5022960 total bytes transmitted
         0 bad packets received
         0 packet transmit problems
         0 total RX packets dropped
         0 total TX packets dropped
         0 multicast packets received
*/
int get_lantiqwifi_stats(const char *wif, uint64_t *tx, uint64_t *rx)
{
	char path[64],line[256];
	FILE *fp;
	snprintf(path, sizeof(path), "%s/%s/General", "/proc/net/mtlk", wif);
	fp = fopen(path, "r");
	if(!fp){
		_dprintf("Can't open the file: %s.\n", path);
		return -1;
	}
	//ret = fcntl(fileno(fp), F_GETFL, 0);
	//fcntl(fileno(fp), F_SETFL, ret | O_NONBLOCK);

	while(fgets(line, sizeof(line), fp)){
		if(!strstr(line, "per HW"))
			continue;
		while(fgets(line, sizeof(line), fp)){
			if(!strstr(line, "total bytes received"))
				continue;
			if(sscanf(line, "%llu", rx) != 1){
				fclose(fp);
				_dprintf("Can't read rx bytes in %s.\n", path);
				return -2;
			}
			if(!fgets(line, sizeof(line), fp) || sscanf(line, "%llu", tx) != 1){
				fclose(fp);
				_dprintf("Can't read tx bytes in %s.\n", path);
				return -2;
			}
			fclose(fp);
			return 0;
		}
	}
	fclose(fp);
	return -1;
}

