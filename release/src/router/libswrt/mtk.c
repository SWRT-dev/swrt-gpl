
int mtk_getSTAInfo(int unit, struct stainfo **sta_info)
{
	struct iwreq wrq;
	int i, firstRow;
#if defined(RTCONFIG_MT798X)
	char data[sizeof(RT_802_11_MAC_TABLE_5G)]; // 2G, 5G use same structure
#else
	char data[16384];
#endif
	char mac[18];
	RT_802_11_MAC_TABLE_5G *mp;
	char *value;
	int rssi, cnt, xTxR;
	char *ifname = NULL;
	char prefix[] = "wlXXXXXXXXXX_";
	struct stainfo *info, *last = NULL;
	int iocmd = RTPRIV_IOCTL_GET_MAC_TABLE;
#if defined(RTCONFIG_MT798X)
	iocmd = RTPRIV_IOCTL_GET_MAC_TABLE_STRUCT;
#endif
	snprintf(prefix, sizeof(prefix), "wl%d_", unit);
	ifname = nvram_pf_safe_get(prefix, "ifname");

	/* query wl for authenticated sta list */
	memset(data, 0, sizeof(data));
	wrq.u.data.pointer = data;
	wrq.u.data.length = sizeof(data);
	wrq.u.data.flags = 0;
	if(wl_ioctl(ifname, iocmd, &wrq) >= 0){
		mp = (RT_802_11_MAC_TABLE_5G *)wrq.u.data.pointer;
		xTxR = nvram_pf_get_int(prefix, "HT_RxStream");
		if(mp->Num > 0){
			unsigned short tx_ratedata = 0;
			unsigned short rx_ratedata = 0;
			unsigned char phy, mcs, bw, vht_nss, sgi, stbc;
			unsigned char r_phy, r_mcs, r_bw, r_vht_nss, r_sgi, r_stbc;
			for(i = 0; i < mp->Num; i++){
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
				rssi = cnt = 0;
				memset(info, 0, sizeof(struct stainfo));
				info->next = NULL;
				memset(mac, 0, sizeof(mac));
				ether_etoa(&mp->Entry[i].Addr[0], mac);
				strlcpy(info->sta_mac, mac, sizeof(info->sta_mac));
				if(mp->Entry[i].AvgRssi0 && cnt < xTxR){
					rssi += (signed int)(signed char)mp->Entry[i].AvgRssi0;//fix char = unsigned char on arm64
					cnt++;
				}
				if(mp->Entry[i].AvgRssi1 && cnt < xTxR){
					rssi += (signed int)(signed char)mp->Entry[i].AvgRssi1;
					cnt++;
				}
				if(mp->Entry[i].AvgRssi2 && cnt < xTxR){
					rssi += (signed int)(signed char)mp->Entry[i].AvgRssi2;
					cnt++;
				}
				if(cnt){
					rssi = rssi / cnt;
					info->rssi = (unsigned int)rssi;
				}else
					info->rssi = 0;
				tx_ratedata = mp->Entry[i].TxRate.word;
				rx_ratedata = (unsigned short)(mp->Entry[i].LastRxRate & 0xffff);
				mtk_parse_ratedata(tx_ratedata, &phy, &mcs, &bw, &vht_nss, &sgi, &stbc);
				mtk_parse_ratedata(rx_ratedata, &r_phy, &r_mcs, &r_bw, &r_vht_nss, &r_sgi, &r_stbc);
				info->conn_time = mp->Entry[i].ConnectedTime;
				if(unit == 0){
					info->tx_rate = (double)mtk_mcs_to_rate(mcs, phy, bw, sgi, vht_nss, 0);
					if(rx_ratedata)
						info->rx_rate = (double)mtk_mcs_to_rate(r_mcs, r_phy, r_bw, r_sgi, r_vht_nss, 0);
				}else{
					info->tx_rate = (double)mtk_mcs_to_rate(mcs, phy, bw, sgi, vht_nss, 1);
					if(rx_ratedata)
						info->rx_rate = (double)mtk_mcs_to_rate(r_mcs, r_phy, r_bw, r_sgi, r_vht_nss, 1);
				}
				info->inactive_flag = 0;//?
				strlcpy(info->conn_if, ifname, sizeof(info->conn_if));
				info->conn_if_idx = unit;
				info->conn_if_vidx = 0;
			}
		}
	}
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
