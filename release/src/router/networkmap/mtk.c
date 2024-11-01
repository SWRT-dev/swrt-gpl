
void MTK_stainfo(int unit)
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
	STA_INFO_TABLE *sta_info_tab = NULL;
	unsigned int wireless_type = 1;

	snprintf(prefix, sizeof(prefix), "wl%d_", unit);
	ifname = nvram_pf_safe_get(prefix, "ifname");

	/* query wl for authenticated sta list */
	memset(data, 0, sizeof(data));
	wrq.u.data.pointer = data;
	wrq.u.data.length = sizeof(data);
	wrq.u.data.flags = 0;
	if(wl_ioctl(ifname, RTPRIV_IOCTL_GET_MAC_TABLE_STRUCT, &wrq) >= 0){
		mp = (RT_802_11_MAC_TABLE_5G *)wrq.u.data.pointer;
		xTxR = nvram_pf_get_int(prefix, "HT_RxStream");
		if(!strcmp(ifname, WIF_2G))
			wireless_type = 1;
		else if(!strcmp(ifname, WIF_5G))
			wireless_type = 2;
//		else if(!strcmp(ifname, WIF_5G2))
//			wireless_type = 3;
		if(mp->Num > 0){
			int hr, min, sec;
			unsigned short tx_ratedata = 0;
			unsigned short rx_ratedata = 0;
			unsigned char phy, mcs, bw, vht_nss, sgi, stbc;
			unsigned char r_phy, r_mcs, r_bw, r_vht_nss, r_sgi, r_stbc;
			for(i = 0; i < mp->Num; i++){
				sta_info_tab = (STA_INFO_TABLE *)calloc(1, sizeof(STA_INFO_TABLE));
				if(sta_info_tab == NULL){
					printf("%s: calloc failed.\n", __func__);
					return;
				}
				rssi = cnt = 0;
				memcpy(sta_info_tab->mac_addr, mp->Entry[i].Addr, sizeof(sta_info_tab->mac_addr));
				sta_info_tab->wireless = wireless_type;
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
				if(cnt == 0){
					free(sta_info_tab);
					continue;	//skip this sta info
				}
				rssi = rssi / cnt;
				sta_info_tab->rssi = (unsigned int)rssi;
				tx_ratedata = mp->Entry[i].TxRate.word;
				rx_ratedata = (unsigned short)(mp->Entry[i].LastRxRate & 0xffff);
				mtk_parse_ratedata(tx_ratedata, &phy, &mcs, &bw, &vht_nss, &sgi, &stbc);
				mtk_parse_ratedata(rx_ratedata, &r_phy, &r_mcs, &r_bw, &r_vht_nss, &r_sgi, &r_stbc);
				hr = (mp->Entry[i].ConnectedTime) / 3600;
				min = (mp->Entry[i].ConnectedTime) % 3600 / 60;
				sec = mp->Entry[i].ConnectedTime - hr * 3600 - min * 60;
				snprintf(sta_info_tab->conn_time, sizeof(sta_info_tab->conn_time), "%02d:%02d:%02d", hr, min, sec);
				if(wireless_type == 1){
					snprintf(sta_info_tab->txrate, sizeof(sta_info_tab->txrate), "%5uM", mtk_mcs_to_rate(mcs, phy, bw, sgi, vht_nss, 0));
					if(rx_ratedata)
						snprintf(sta_info_tab->rxrate, sizeof(sta_info_tab->rxrate), "%5uM", mtk_mcs_to_rate(r_mcs, r_phy, r_bw, r_sgi, r_vht_nss, 0));
				}else{
					snprintf(sta_info_tab->txrate, sizeof(sta_info_tab->txrate), "%5uM", mtk_mcs_to_rate(mcs, phy, bw, sgi, vht_nss, 1));
					if(rx_ratedata)
						snprintf(sta_info_tab->rxrate, sizeof(sta_info_tab->rxrate), "%5uM", mtk_mcs_to_rate(r_mcs, r_phy, r_bw, r_sgi, r_vht_nss, 1));
				}
				if(g_show_sta_info && f_exists("/tmp/conn_debug"))
					printf("%s[%3d,MTK] %02X%02X%02X%02X%02X%02X, rx: %s tx: %s, rssi: %d\n", "[connection log]", i, sta_info_tab->mac_addr[0], sta_info_tab->mac_addr[1],
						sta_info_tab->mac_addr[2], sta_info_tab->mac_addr[3], sta_info_tab->mac_addr[4], sta_info_tab->mac_addr[5], 
						sta_info_tab->rxrate, sta_info_tab->txrate, sta_info_tab->rssi);
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
