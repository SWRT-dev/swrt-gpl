/*
 * hostapd / Radio Measurement (RRM)
 * Copyright(c) 2013 - 2016 Intel Mobile Communications GmbH.
 * Copyright(c) 2011 - 2016 Intel Corporation. All rights reserved.
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */


#ifndef CSA_H
#define CSA_H

void hostapd_handle_spectrum_management(struct hostapd_data *hapd,
				      const u8 *buf, size_t len, int ssi_signal);

void hostapd_handle_action_extcsa(struct hostapd_data *hapd,
				      const u8 *buf, size_t len, int ssi_signal);

#endif /* CSA_H */
