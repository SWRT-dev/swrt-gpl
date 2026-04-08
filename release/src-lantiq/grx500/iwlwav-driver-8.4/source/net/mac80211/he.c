/*
 * HE handling
 *
 * Copyright(c) 2017 Intel Deutschland GmbH
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include "ieee80211_i.h"

void
ieee80211_he_oper_ie_to_sta_he_oper(struct ieee80211_supported_band *sband,
				  const u8 *he_oper_ie, struct sta_info *sta)
{
	struct ieee80211_sta_he_oper *he_oper = &sta->sta.he_oper;
	struct ieee80211_he_operation *he_oper_ie_elem = (void *)he_oper_ie;
	u32 he_oper_params;
	u8 he_oper_idx, size;

	memset(he_oper, 0, sizeof(*he_oper));

	if (!he_oper_ie || !ieee80211_get_he_sta_cap(sband))
		return;

	he_oper_params = le32_to_cpu(he_oper_ie_elem->he_oper_params);
	size = sizeof(he_oper->he_oper_params);
	memcpy(&he_oper->he_oper_params, he_oper_ie, size);
	he_oper_idx = size;
	size = sizeof(he_oper->he_mcs_nss_set);
	memcpy(&he_oper->he_mcs_nss_set, &he_oper_ie_elem[he_oper_idx], size);
	he_oper_idx += size;

	/* Copy the option operation info and check before copy */
	if (he_oper_params & IEEE80211_HE_OPERATION_VHT_OPER_INFO) {
		size = sizeof(he_oper->vht_oper_info);
		memcpy(&he_oper->vht_oper_info, &he_oper_ie[he_oper_idx], size);
		he_oper_idx += size;
	}

	if (he_oper_params & IEEE80211_HE_OPERATION_CO_HOSTED_BSS) {
		size = sizeof(he_oper->co_hosted_bss);
		memcpy(&he_oper->co_hosted_bss, &he_oper_ie[he_oper_idx], size);
		he_oper_idx += size;
	}

	if (he_oper_params & IEEE80211_HE_OPERATION_6GHZ_INFO_PRESENT) {
		size = sizeof(he_oper->oper_6GHZ_info);
		memcpy(&he_oper->oper_6GHZ_info, &he_oper_ie[he_oper_idx], size);
	}
}

void
ieee80211_he_cap_ie_to_sta_he_cap(struct ieee80211_sub_if_data *sdata,
				  struct ieee80211_supported_band *sband,
				  const u8 *he_cap_ie, u8 he_cap_len,
				  struct sta_info *sta)
{
	struct ieee80211_sta_he_cap *he_cap = &sta->sta.he_cap;
	struct ieee80211_he_cap_elem *he_cap_ie_elem = (void *)he_cap_ie;
	u8 he_ppe_size;
	u8 mcs_nss_size;
	u8 he_total_size;

	memset(he_cap, 0, sizeof(*he_cap));

	if (!he_cap_ie || !ieee80211_get_he_sta_cap(sband))
		return;

	/* Make sure size is OK */
	mcs_nss_size = ieee80211_he_mcs_nss_size(he_cap_ie_elem);
	he_ppe_size =
		ieee80211_he_ppe_size(he_cap_ie[sizeof(he_cap->he_cap_elem) +
						mcs_nss_size],
				      he_cap_ie_elem->phy_cap_info);
	he_total_size = sizeof(he_cap->he_cap_elem) + mcs_nss_size +
			he_ppe_size;
	if (he_cap_len < he_total_size)
		return;

	memcpy(&he_cap->he_cap_elem, he_cap_ie, sizeof(he_cap->he_cap_elem));

	/* HE Tx/Rx HE MCS NSS Support Field */
	memcpy(&he_cap->he_mcs_nss_supp,
	       &he_cap_ie[sizeof(he_cap->he_cap_elem)], mcs_nss_size);

	/* Check if there are (optional) PPE Thresholds */
	if (he_cap->he_cap_elem.phy_cap_info[6] &
	    IEEE80211_HE_PHY_CAP6_PPE_THRESHOLD_PRESENT)
		memcpy(he_cap->ppe_thres,
		       &he_cap_ie[sizeof(he_cap->he_cap_elem) + mcs_nss_size],
		       he_ppe_size);

	he_cap->has_he = true;
}
