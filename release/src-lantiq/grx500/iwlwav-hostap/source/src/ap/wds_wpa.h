#ifndef WDS_WPA_H
#define WDS_WPA_H

int ltq_wds_wpa_init(struct hostapd_data *hapd);
void ltq_wds_wpa_deinit(struct hostapd_data *hapd);
int ltq_wds_wpa_add(struct hostapd_data *hapd, const u8 *src);

#endif /* WDS_WPA_H */
