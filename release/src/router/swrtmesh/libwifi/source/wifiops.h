/**
 * @file wifiops.h
 * @brief WiFi radio and interfaces operations API header file.
 *
 * Copyright (C) 2020-2023 iopsys Software Solutions AB. All rights reserved.
 *
 * @author anjan.chanda@iopsys.eu
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */

#ifndef WIFIOPS_H
#define WIFIOPS_H

/**
 * @struct wifi_radio_ops
 * @brief wifi radio related operations.
 *
 * All radio/device specific operations are handled through this structure.
 * In order to support a new wifi chipset, struct wifi_radio_ops alongwith
 * struct wifi_iface_ops must be implemented by its driver module.
 *
 * Unless otherwise mentioned, the following functions return 0 on Success,
 * and -1 on Failure.
 *
 * <b>int (*list)(struct radio_entry *radio, int *num).</b>\n
 *	@brief            Get list of preset radios
 *	@param[out] radio  radio array
 *	@param[out] num   number of entries in radio array
 *
 * <b>int (*is_multiband)(const char *name, bool *res).</b>\n
 *	@brief            Check if multiband radio.
 *	@param[in] name   radio interface name
 *	@param[out] res   result
 *
 * <b>int (*info)(const char *name, struct wifi_radio *radio).</b>\n
 *	@brief            Get information about the radio interface.
 *	@param[in] name   radio interface name
 *	@param[out] radio struct wifi_radio
 *
 * <b>int (*info_band)(const char *name, enum wifi_band band, struct wifi_radio *radio).</b>\n
 *	@brief            Get information about the radio interface.
 *	@param[in] name   radio interface name
 *	@param[in] band   radio band - multiband case
 *	@param[out] radio struct wifi_radio
 *
 * <b>int (*get_supp_band)(const char *name, uint32_t *bands)</b>\n
 *	@brief            Get supported WiFi bands in bands param.
 *	@param[in] name   radio interface name
 *	@param[out] bands bitmap of bands from struct wifi_band
 *
 * <b>int (*get_oper_band)(const char *name, enum wifi_band *band)</b>\n
 *	@brief            Get current operating WiFi band.
 *	@param[in] name   radio interface name
 *	@param[out] band  band struct wifi_band type
 *
 * <b>int (*get_ifstatus)(const char *name, ifstatus_t *f)</b>\n
 *	@brief            Get WiFi radio device flags
 *	@param[in] name   radio interface name
 *	@param[out] f     IFF_* device flags
 *
 * <b>int (*get_caps)(const char *name, struct wifi_caps *caps)</b>\n
 *	@brief            Get WiFi radio capabilities.
 *	@param[in] name   radio interface name
 *	@param[out] caps  capabilities struct wifi_caps
 *
 * <b>int (*get_band_caps)(const char *name, enum wifi_band band, struct wifi_caps *caps)</b>\n
 *	@brief            Get WiFi radio capabilities.
 *	@param[in] name   radio interface name
 *	@param[in] band   radio band
 *	@param[out] caps  capabilities struct wifi_caps
 *
 * <b>int (*get_supp_stds)(const char *name, uint8_t *std)</b>\n
 *	@brief            Get WiFi supported standards.
 *	@param[in] name   radio interface name
 *	@param[out] std   bitmap of #enum wifi_std
 *
 * <b>int (*get_band_supp_stds)(const char *name, enum wifi_band band, uint8_t *std)</b>\n
 *	@brief            Get WiFi supported standards.
 *	@param[in] name   radio interface name
 *	@param[in] band   radio band
 *	@param[out] std   bitmap of #enum wifi_std
 *
 * <b>int (*get_oper_stds)(const char *name, uint8_t *std)</b>\n
 *	@brief            Get WiFi operational standards.
 *	@param[in] name   radio interface name
 *	@param[out] std   bitmap of enum wifi_std
 *
 * <b>int (*get_band_oper_stds)(const char *name, enum wifi_band band, uint8_t *std)</b>\n
 *	@brief            Get WiFi operational standards.
 *	@param[in] name   radio interface name
 *	@param[in] band   radio band
 *	@param[out] std   bitmap of enum wifi_std
 *
 * <b>int (*get_country)(const char *name, char *alpha2)</b>\n
 *	@brief             Get operating country information.
 *	@param[in] name    radio interface name
 *	@param[out] alpha2 country code
 *
 * <b>int (*get_countrylist)(const char *name, char **c, int *num)</b>\n
 *	@brief             Get supporting country list information.
 *	@param[in] name	   radio interface name
 *	@param[out] cc     country code of all supporting countries
 *	@param[out] num    count of countries in cc list
 *
 * <b>int (*get_channel)(const char *ifname, uint32_t *channel, enum wifi_bw *bw)</b>\n
 *	@brief              Get operating channel information.
 *	@param[in] ifname   radio interface name
 *	@param[out] channel primary control channel
 *	@param[out] bw      channel bandwidth
 *
 * <b>int (*get_band_channel)(const char *ifname, enum wifi_band band, uint32_t *channel, enum wifi_bw *bw)</b>\n
 *	@brief              Get operating channel information.
 *	@param[in] ifname   radio interface name
 *	@param[in] band     radio band
 *	@param[out] channel primary control channel
 *	@param[out] bw      channel bandwidth
 *
 * <b>int (*set_channel)(const char *ifname, uint32_t channel, enum wifi_bw bw)</b>\n
 *	@brief              Set operating channel with bandwidth.
 *	@param[in] ifname   radio interface name
 *	@param[out] channel primary control channel
 *	@param[out] bw      channel bandwidth
 *
 * <b>int (*get_supp_channels)(const char *name, uint32_t *chlist, int *num,
 *		const char *alpha2, enum wifi_band f, enum wifi_bw bw)</b>\n
 *	@brief              Get supported channels.
 *	@param[in] name     radio interface name
 *	@param[out] chlist  array of channels
 *	@param[out] num     number of channels in chlist array
 *	@param[in] alpha2   country code
 *	@param[in] f        frequency band #enum wifi_band
 *	@param[in] bw       channel bandwidth enum wifi_bw
 *
 * <b>int (*get_oper_channels)(const char *name, uint32_t *chlist, int *num,
 *		const char *alpha2, enum wifi_band f, enum wifi_bw b)</b>\n
 *	@brief              Get operating channels.
 *	@param[in] name     radio interface name
 *	@param[out] chlist  array of channels
 *	@param[out] num     number of channels in chlist array
 *	@param[in] alpha2   country code
 *	@param[in] f        frequency band #enum wifi_band
 *	@param[in] bw       channel bandwidth enum wifi_bw
 *
 * <b>int (*get_supp_opclass)(const char *name, int *num,
 *						struct wifi_opclass *o)</b>\n
 *	@brief              Get supported operating classes.
 *	@param[in] name     radio interface name
 *	@param[out] num     number of operating classes supported
 *	@param[out] o       array of struct wifi_opclass elements
 *
 * <b>int (*get_band_supp_opclass)(const char *name, enum wifi_band band,
 *				   int *num, struct wifi_opclass *o)</b>\n
 *	@brief              Get supported operating classes.
 *	@param[in] name     radio interface name
 *	@param[in] band     radio band
 *	@param[out] num     number of operating classes supported
 *	@param[out] o       array of struct wifi_opclass elements
 *
 * <b>int (*get_curr_opclass)(const char *name, int *num,
 *						struct wifi_opclass *o)</b>\n
 *	@brief              Get current operating class(es).
 *	@param[in] name     radio interface name
 *	@param[out] num     number of current operating classes
 *	@param[out] o       array of struct wifi_opclass elements
 *
 * <b>int (*get_band_curr_opclass)(const char *name, enum wifi_band band,
 *				   int *num, struct wifi_opclass *o)</b>\n
 *	@brief              Get current operating class(es).
 *	@param[in] name     radio interface name
 *	@param[in] band     radio band
 *	@param[out] num     number of current operating classes
 *	@param[out] o       array of struct wifi_opclass elements
 *
 * <b>int (*get_bandwidth)(const char *name, enum wifi_bw *bw)</b>\n
 *	@brief             Get operating channel bandwidth.
 *	@param[in] name    radio interface name
 *	@param[out] bw     bandwidth #enum wifi_bw
 *
 * <b>int (*get_supp_bandwidths)(const char *name, uint32_t *bws)</b>\n
 *	@brief             Get supported bandwidths
 *	@param[i] name     radio interface name
 *      @param[out] bws    bitmask of supported BWs
 *
 * <b>int (*get_band_supp_bandwidths)(const char *name, enum wifi_band band, uint32_t *bws)</b>\n
 *	@brief             Get supported bandwidths
 *	@param[i] name     radio interface name
 *	@param[i] band     radio band
 *      @param[out] bws    bitmask of supported BWs
 *
 * <b>int (*get_maxrate)(const char *name, unsigned long *rate)</b>\n
 *	@brief             Get maximum supported phy rate.
 *	@param[in] name    radio interface name
 *	@param[out] rate   rate in Mbps
 *
 * <b>int (*get_band_maxrate)(const char *name, enum wifi_band band, unsigned long *rate)</b>\n
 *	@brief             Get maximum supported phy rate.
 *	@param[in] name    radio interface name
 *	@param[in] band    radio band
 *	@param[out] rate   rate in Mbps
 *
 * <b>int (*get_basic_rates)(const char *name, int *num, uint32_t *rates)</b>\n
 *	@brief             Get basic phy rates.
 *	@param[in] name    radio interface name
 *	@param[out] num    number of elements in rates array
 *	@param[out] rates  array of rates in Mbps
 *
 * <b>int (*get_oper_rates)(const char *name, int *num, uint32_t *rates)</b>\n
 *	@brief             Get operational phy rates.
 *	@param[in] name    radio interface name
 *	@param[out] num    number of elements in rates array
 *	@param[out] rates  array of rates in Mbps
 *
 * <b>int (*get_supp_rates)(const char *name, int *num, uint32_t *rates)</b>\n
 *	@brief             Get supported phy rates.
 *	@param[in] name    radio interface name
 *	@param[out] num    number of elements in rates array
 *	@param[out] rates  array of rates in Mbps
 *
 * <b>int (*get_stats)(const char *ifname, struct wifi_radio_stats *s)</b>\n
 *	@brief             Get radio statistics.
 *	@param[in] ifname  radio interface name
 *	@param[out] s      radio stats and counters
 *
 * <b>int (*get_band_stats)(const char *ifname, enum wifi_band band, struct wifi_radio_stats *s)</b>\n
 *	@brief             Get radio statistics.
 *	@param[in] ifname  radio interface name
 *	@param[in] band    radio band
 *	@param[out] s      radio stats and counters
 *
 * <b>int (*scan)(const char *name, struct scan_param *p)</b>\n
 *	@brief             Trigger scanning.
 *	@param[in] name    radio interface name
 *	@param[in] p       scan request parameters
 *
 * <b>int (*scan_ex)(const char *ifname, struct scan_param_ex *sp)</b>\n
 *	@brief             Trigger scanning.
 *	@param[in] ifname  radio interface name
 *	@param[in] sp      scan request parameters
 *
 * <b>int (*get_scan_results)(const char *name, struct wifi_bss *bsss, int *num)</b>\n
 *	@brief             Get scan results.
 *	@param[in] name    radio interface name
 *	@param[out] bsss   array of scanned APs
 *	@param[out] num    number of scanned APs
 *
 * <b>int (*get_bss_scan_result)(const char *name, uint8_t *bssid,
 *				struct wifi_bss_detail *b)</b>\n
 *	@brief             Get scan result details of a specific AP.
 *	@param[in] name    radio interface name
 *	@param[in] bssid   bssid of a scanned AP
 *	@param[out] b      scan result including IE details
 *
 * <b>int (*get_noise)(const char *ifname, int *noise);
 *	@brief             Get current noise value.
 *	@param[in] name    radio interface name
 *	@param[out] noise  noise value in dBm
 *
 * <b>int (*get_band_noise)(const char *ifname, enum wifi_band band, int *noise);
 *	@brief             Get current noise value.
 *	@param[in] name    radio interface name
 *	@param[in] band    radio band
 *	@param[out] noise  noise value in dBm
 *
 * <b>int (*acs)(const char *name, struct acs_param *p)</b>\n
 *	@brief             Trigger ACS (auto channel selection).
 *	@param[in] name    radio interface name
 *	@param[in] p       ACS request parameters
 *
 * <b>int (*get_param)(const char *name, const char *param, int *len, void *val)</b>\n
 *	@brief             Get radio parameter value(s).
 *	@param[in] name    radio interface name
 *	@param[in] param   radio parameter name
 *	@param[out] len    length of the returned parameter value
 *	@param[out] val    parameter value
 *
 * <b>int (*set_param)(const char *name, const char *param, int len, void *val)</b>\n
 *	@brief             Set radio parameter value(s).
 *	@param[in] name    radio interface name
 *	@param[in] param   radio parameter name
 *	@param[in] len     length of the parameter
 *	@param[in] val     value of parameter
 *
 * <b>int (*get_hwaddr)(const char *name, uint8_t *hwaddr)</b>\n
 *	@brief             Get macaddress of the radio
 *	@param[in] name    radio interface name
 *	@param[out] hwaddr mac address as an hex array
 *
 * <b>int (*add_iface)(const char *name, enum wifi_mode m, char *argv[])</b>\n
 *	@brief             Create a WiFi interface on this radio.
 *	@param[in] name    radio interface name
 *	@param[in] m       wifi mode f.e. WIFI_MODE_AP, WIFI_MODE_STA etc.
 *	@param[in] argv    string arguments array of wifi attributes and values
 *
 * <b>int (*del_iface)(const char *name, const char *ifname)</b>\n
 *	@brief             Delete a WiFi interface on this radio.
 *	@param[in] name    radio interface name
 *	@param[in] ifname  wifi interface name to be deleted
 *
 * <b>int (*list_iface)(const char *name, struct iface_entry *iface, int *num)</b>\n
 *	@brief             List a WiFi interface on this radio.
 *	@param[in] name    radio interface name
 *	@param[out] iface  array of interfaces
 *	@param[out] num    number of entries in iface array
 *
 * <b>int (*channels_info)(const char *name, struct chan_entry *channel, int *num)</b>\n
 *	@brief             Get current channels info.
 *	@param[in] name    radio interface name
 *	@param[out] iface  array of channels
 *	@param[out] num    number of entries in channel array
 *
 * <b>int (*channels_info_band)(const char *name, enum wifi_band band, struct chan_entry *channel, int *num)</b>\n
 *	@brief             Get current channels info.
 *	@param[in] name    radio interface name
 *	@param[in] band    radio band
 *	@param[out] iface  array of channels
 *	@param[out] num    number of entries in channel array
 *
 * <b>int (*start_cac)(const char *name, int channel, enum wifi_bw bw,
 *					enum wifi_cac_method method)</b>\n
 *	@brief             Start CAC (channel availability check).
 *	@param[in] name    radio interface name
 *	@param[in] channel control channel on which CAC is requested
 *	@param[in] bw      bandwidth
 *	@param[in] method  CAC method requested
 *
 * <b>int (*stop_cac)(const char *name)</b>\n
 *	@brief             Stop CAC.
 *	@param[in] name    radio interface name
 *
 * <b>int (*get_opclass_preferences)(const char *name, struct wifi_opclass *opclass,
 *				     int *num);</b>\n
 *	@brief              Get prefered opclass/channels
 *	@param[in] name     radio interface name
 *	@param[out] opclass array of opclass/channels
 *	@param[in/out] num  number of entries in opclass array
 *
 * <b>int (*get_band_opclass_preferences)(const char *name, enum wifi_band band,
 *					  struct wifi_opclass *opclass, int *num);</b>\n
 *	@brief              Get prefered opclass/channels
 *	@param[in] name     radio interface name
 *	@param[in] band     radio band
 *	@param[out] opclass array of opclass/channels
 *	@param[in/out] num  number of entries in opclass array
 *
 * <b>int (*simulate_radar)(const char *name, struct wifi_radar_args *radar)</b>\n
 *	@brief             Trigger radar detection event.
 *	@param[in] name    radio interface name
 *	@param[in] radar   simulated radar parameters
 */
struct wifi_radio_ops {
	int (*is_multiband)(const char *name, bool *res);
	int (*info)(const char *name, struct wifi_radio *radio);
	int (*info_band)(const char *name, enum wifi_band band, struct wifi_radio *radio);
	int (*get_supp_band)(const char *name, uint32_t *bands);
	int (*get_oper_band)(const char *name, enum wifi_band *band);
	int (*get_ifstatus)(const char *name, ifstatus_t *f);
	int (*get_caps)(const char *name, struct wifi_caps *caps);
	int (*get_band_caps)(const char *name, enum wifi_band band, struct wifi_caps *caps);
	int (*get_supp_stds)(const char *name, uint8_t *std);
	int (*get_band_supp_stds)(const char *name, enum wifi_band band, uint8_t *std);
	int (*get_oper_stds)(const char *name, uint8_t *std);
	int (*get_band_oper_stds)(const char *name, enum wifi_band band, uint8_t *std);

	int (*get_country)(const char *name, char *alpha2);
	int (*get_countrylist)(const char *name, char *cc, int *num);
	int (*get_channel)(const char *ifname, uint32_t *channel, enum wifi_bw *bw);
	int (*get_band_channel)(const char *ifname, enum wifi_band band, uint32_t *channel, enum wifi_bw *bw);
	int (*set_channel)(const char *ifname, uint32_t channel, enum wifi_bw bw);
	int (*get_supp_channels)(const char *name, uint32_t *chlist, int *num,
			const char *alpha2, enum wifi_band f, enum wifi_bw b);
	int (*get_oper_channels)(const char *name, uint32_t *chlist, int *num,
			const char *alpha2, enum wifi_band f, enum wifi_bw b);

	int (*get_supp_opclass)(const char *name, int *num_opclass,
						struct wifi_opclass *o);
	int (*get_band_supp_opclass)(const char *name, enum wifi_band band,
				     int *num_opclass, struct wifi_opclass *o);

	int (*get_curr_opclass)(const char *name, struct wifi_opclass *o);
	int (*get_band_curr_opclass)(const char *name, enum wifi_band band, struct wifi_opclass *o);

	int (*get_bandwidth)(const char *name, enum wifi_bw *bw);
	int (*get_supp_bandwidths)(const char *name, uint32_t *bws);
	int (*get_band_supp_bandwidths)(const char *name, enum wifi_band band, uint32_t *bws);
	int (*get_maxrate)(const char *name, unsigned long *rate_Mbps);
	int (*get_band_maxrate)(const char *name, enum wifi_band band, unsigned long *rate_Mbps);
	int (*get_basic_rates)(const char *name, int *num, uint32_t *rates_kbps);
	int (*get_oper_rates)(const char *name, int *num, uint32_t *rates_kbps);
	int (*get_supp_rates)(const char *name, int *num, uint32_t *rates);
	int (*get_stats)(const char *ifname, struct wifi_radio_stats *s);
	int (*get_band_stats)(const char *ifname, enum wifi_band band, struct wifi_radio_stats *s);

	int (*scan)(const char *name, struct scan_param *p);
	int (*scan_ex)(const char *ifname, struct scan_param_ex *sp);
	int (*get_scan_results)(const char *name, struct wifi_bss *bsss, int *num);
	int (*get_bss_scan_result)(const char *name, uint8_t *bssid,
						struct wifi_bss_detail *b);

	int (*get_noise)(const char *ifname, int *noise);
	int (*get_band_noise)(const char *ifname, enum wifi_band band, int *noise);

	int (*acs)(const char *name, struct acs_param *p);

	int (*get_param)(const char *name, const char *param, int *len, void *val);
	int (*set_param)(const char *name, const char *param, int len, void *val);

	int (*get_hwaddr)(const char *name, uint8_t *hwaddr);

	int (*add_iface)(const char *name, enum wifi_mode m, char *argv[]);
	int (*del_iface)(const char *name, const char *ifname);
	int (*list_iface)(const char *name, struct iface_entry *iface, int *num);

	int (*channels_info)(const char *name, struct chan_entry *channel, int *num);
	int (*channels_info_band)(const char *name, enum wifi_band band, struct chan_entry *channel, int *num);

	int (*start_cac)(const char *name, int channel, enum wifi_bw bw,
			 enum wifi_cac_method method);
	int (*stop_cac)(const char *name);
	int (*get_opclass_preferences)(const char *name, struct wifi_opclass *opclass,
				       int *num);
	int (*get_band_opclass_preferences)(const char *name, enum wifi_band band,
					    struct wifi_opclass *opclass, int *num);
	int (*simulate_radar)(const char *name, struct wifi_radar_args *radar);
};


/**
 * @struct wifi_iface_ops
 * @brief WiFi interface related operations.
 *
 * BSS/STA interface operations are handled through this structure.
 *
 * <b>int (*start_wps)(const char *ifname, struct wps_param wps)</b>\n
 *	@brief             Start WPS registration
 *	@param[in] ifname  interface name
 *	@param[in] wps     wps_param structure
 *
 * <b>int (*stop_wps)(const char *ifname)</b>\n
 *	@brief             Stop ongoing WPS registration
 *	@param[in] ifname  interface name
 *
 * <b>int (*get_wps_status)(const char *ifname, enum wps_status *s)</b>\n
 *	@brief             Get latest wps registration status
 *	@param[in] ifname  interface name
 *	@param[out] s      wps_param structure
 *
 * <b>int (*get_wps_pin)(const char *ifname, unsigned long *pin)</b>\n
 *	@brief             Get AP's (i.e. own) WPS pin
 *	@param[in] ifname  interface name
 *	@param[out] pin    wps pin value
 *
 * <b>int (*set_wps_pin)(const char *ifname, unsigned long pin)</b>\n
 *	@brief             Set AP's (i.e. own) WPS pin
 *	@param[in] ifname  interface name
 *	@param[in] pin     wps pin value
 *
 * <b>int (*get_wps_device_info)(const char *ifname, struct wps_device *s)</b>\n
 *	@brief             Get WPS device information
 *	@param[in] ifname  interface name
 *	@param[out] s      wps_device structure
 *
 * <b>int (*get_caps)(const char *ifname, struct wifi_caps *caps)</b>\n
 *	@brief             Get capabilities
 *	@param[in] ifname  interface name
 *	@param[out] caps   wifi_caps structure
 *
 * <b>int (*get_mode)(const char *ifname, enum wifi_mode *mode)</b>\n
 *	@brief             Get WiFi mode
 *	@param[in] ifname  interface name
 *	@param[out] mode   WiFi mode f.e. WIFI_MODE_AP or WIFI_MODE_STA etc.
 *
 * <b>int (*get_security)(const char *ifname, uint32_t *auth, uint32_t *enc)</b>\n
 *	@brief             Get security info
 *	@param[in] ifname  interface name
 *	@param[out] auth   authtication type
 *	@param[out] enc    encryption type
 *
 * <b>int (*add_vendor_ie)(const char *ifname, struct vendor_iereq *req)</b>\n
 *	@brief             Add vendor specific ie element
 *	@param[in] ifname  interface name
 *	@param[in] req     vendor_iereq structure
 *
 * <b>int (*del_vendor_ie)(const char *ifname, struct vendor_iereq *req)</b>\n
 *	@brief             Delete vendor specific ie element
 *	@param[in] ifname  interface name
 *	@param[in] req     vendor_iereq structure
 *
 * <b>int (*get_vendor_ies)(const char *ifname, struct vendor_ie *ies, int *num_ies)</b>\n
 *	@brief             Get list of vendor information elements
 *	@param[in] ifname  interface name
 *	@param[out] ies    array of struct vendor_ie
 *	@param[out] num    array size (number of elements)
 *
 * <b>int (*get_param)(const char *ifname, const char *param, int *len, void *val)</b>\n
 *	@brief             Get AP parameter value(s).
 *	@param[in] ifname  interface name
 *	@param[in] param   parameter name
 *	@param[out] len    length of the returned value
 *	@param[out] val    parameter value
 *
 * <b>int (*set_param)(const char *ifname, const char *param, int len, void *val)</b>\n
 *	@brief             Set AP parameter value(s).
 *	@param[in] ifname  interface name
 *	@param[in] param   parameter name
 *	@param[in] len     length of the parameter
 *	@param[in] val     value of parameter
 *
 * <b>int (*vendor_cmd)(const char *ifname, uint32_t vid, uint32_t subcmd,
 *			uint8_t *in, int inlen, uint8_t *out, int *outlen)</b>\n
 *	@brief             Vendor specific command
 *	@param[in] ifname  interface name
 *	@param[in] vid     vendor id (OUI)
 *	@param[in] subcmd  (sub)command
 *	@param[in] in      input parameter
 *	@param[in] inlen   length of the input parameter
 *	@param[out] out    output parameter
 *	@param[out] outlen length of the output parameter
 *
 * <b>int (*subscribe_frame)(const char *ifname, uint8_t type, uint8_t stype)</b>\n
 *	@brief             Subscribe for received frames
 *	@param[in] name    interface name
 *	@param[in] type    frame type as in IEEE802.11 Std.
 *	@param[in] stype   frame sub-type as in IEEE802.11 Std.
 *
 * <b>int (*unsubscribe_frame)(const char *ifname, uint8_t type, uint8_t stype)</b>\n
 *	@brief             Unsubscribe for received frames
 *	@param[in] name    interface name
 *	@param[in] type    frame type as in IEEE802.11 Std.
 *	@param[in] stype   frame sub-type as in IEEE802.11 Std.
 *
 * <b>int (*set_4addr)(const char *ifname, bool enable)</b>\n
 *	@brief             Enable or disable 4-address mode.
 *	@param[in] ifname  interface name
 *	@param[in] enable  enable = 1, else disable.
 *
 * <b>int (*get_4addr)(const char *ifname, bool *enabled)</b>\n
 *	@brief             Get status of 4-address mode.
 *	@param[in] ifname  interface name
 *	@param[out] enabled  enabled = true, else disabled.
 *
 * <b>int (*get_4addr_parent)(const char *ifname, char *parent)</b>\n
 *	@brief             Get parent interface of a 4-address mode interface.
 *	@param[in] ifname  interface name which is in 4-address mode
 *	@param[out] parent  parent interface name.
 *
 * <b>int (*set_vlan)(const char *ifname, struct vlan_param vlan)</b>\n
 *	@brief             Set VLAN link.
 *	@param[in] ifname  interface name
 *	@param[in] vlan    vlan parameters
 *
 * <b>int (*ap_info)(const char *ifname, struct wifi_ap *ap)</b>\n
 *	@brief             Get detailed AP information
 *	@param[in] ifname  interface name
 *	@param[out] ap     ap information
 *
 * <b>int (*get_bssid)(const char *ifname, uint8_t *bssid)</b>\n
 *	@brief             Get BSSID
 *	@param[in] ifname  interface name
 *	@param[out] bssid  BSSID buffer (6 bytes)
 *
 * <b>int (*get_ssid)(const char *ifname, char *ssid)</b>\n
 *	@brief             Get SSID
 *	@param[in] ifname  interface name
 *	@param[out] ssid   SSID buffer
 *
 * <b>int (*get_stats)(const char *ifname, struct wifi_ap_stats *s)</b>\n
 *	@brief             Get statistics
 *	@param[in] ifname  interface name
 *	@param[out] s      wifi_ap_stats structure
 *
 * <b>int (*get_beacon_ies)(const char *ifname, uint8_t *ies, int *len)</b>\n
 *	@brief             Get Beacon frame information elements
 *	@param[in] ifname  interface name
 *	@param[out] ies    information elements buffer
 *	@param[out] len    length of information elements buffer
 *
 * <b>int (*get_assoclist)(const char *ifname, uint8_t *stas, int *num_stas)</b>\n
 *	@brief             Get STA association list
 *	@param[in] ifname  interface name
 *	@param[out] stas   macaddresses of STAs
 *	@param[out] num_stas  number of STAs
 *
 * <b>int (*get_sta_info)(const char *ifname, uint8_t *addr, struct wifi_sta *info)</b>\n
 *	@brief             Get STA information
 *	@param[in] ifname  interface name
 *	@param[in] addr    macaddress of STA
 *	@param[out] info   STA information
 *
 * <b>int (*get_sta_stats)(const char *ifname, uint8_t *addr, struct wifi_sta_stats *s)</b>\n
 *	@brief             Get STA statistics
 *	@param[in] ifname  interface name
 *	@param[in] addr    macaddress of STA
 *	@param[out] s      STA counters
 *
 * <b>int (*disconnect_sta)(const char *ifname, uint8_t *sta, uint16_t reason)</b>\n
 *	@brief             Disconnect STA
 *	@param[in] ifname  interface name
 *	@param[in] sta     macaddress of STA
 *	@param[in] reason  disconnect reason code as in IEEE802.11 Std
 *
 * <b>int (*restrict_sta)(const char *ifname, uint8_t *sta, int enable)</b>\n
 *	@brief             Assoc-control STA
 *	@param[in] ifname  interface name
 *	@param[in] sta     macaddress of STA
 *	@param[in] enable  enable (= 1) or disable (= 0) assoc-control
 *
 * <b>int (*monitor_sta)(const char *ifname, uint8_t *sta, struct wifi_monsta_config *cfg)</b>\n
 *	@brief             Monitor STA frames
 *	@param[in] ifname  interface name
 *	@param[in] sta     macaddress of STA
 *	@param[in] cfg     monitor STA config
 *
 * <b>int (*get_monitor_sta)(const char *ifname, uint8_t *sta, struct wifi_monsta *sta)</b>\n
 *	@brief             Get monitored STA information
 *	@param[in] ifname  interface name
 *	@param[in] sta     macaddress of STA
 *	@param[out] mon    wifi_monsta structure
 *
 * <b>int (*get_monitor_stas)(const char *ifname, struct wifi_monsta *stas, int *num)</b>\n
 *	@brief             Get monitored STA information
 *	@param[in] ifname  interface name
 *	@param[out] stas   array of struct wifi_monsta
 *	@param[out] num    array size (number of wifi_monsta elements)
 *
 * <b>int (*probe_sta)(const char *ifname, uint8_t *sta)</b>\n
 *	@brief             Probe STA's connection status
 *	@param[in] ifname  interface name
 *	@param[in] sta     macaddress of STA
 *
 * <b>int (*add_neighbor)(const char *ifname, struct nbr nbr)</b>\n
 *	@brief             Add a 802.11k neighbor entry
 *	@param[in] ifname  interface name
 *	@param[in] nbr     nbr structure
 *
 * <b>int (*del_neighbor)(const char *ifname, unsigned char *bssid)</b>\n
 *	@brief             Delete a 802.11k neighbor entry
 *	@param[in] ifname  interface name
 *	@param[in] bssid   Bssid of the neighbor
 *
 * <b>int (*get_neighbor_list)(const char *ifname, struct nbr *nbr, int *nr)</b>\n
 *	@brief             Get 802.11k neighbor list
 *	@param[in] ifname  interface name
 *	@param[out] nbr    array of struct nbr
 *	@param[out] nr     number of array entries
 *
 * <b>int (*req_beacon_report)(const char *ifname, uint8_t *sta)</b>\n
 *	@brief             Request 802.11k Beacon Report from a STA
 *	@param[in] ifname  interface name
 *	@param[in] sta     macaddress of the STA
 *	@param[in] param   11k beacon request parameters
 *	@param[in] param_sz actual size of param
 *
 * <b>int (*get_beacon_report)(const char *ifname, uint8_t *sta,
 *					struct sta_nbr *snbr, int *nr)</b>\n
 *	@brief             Get 802.11k Beacon Report received from a STA
 *	@param[in] ifname  interface name
 *	@param[in] sta     macaddress of the STA
 *	@param[out] snbr   array of sta_nbr structures
 *	@param[out] nr     number of array entries
 *
 * <b>int (*req_bss_transition)(const char *ifname, unsigned char *sta,
 *		int bsss_nr, unsigned char *bsss, unsigned int tmo)</b>\n
 *	@brief             [Deprecated] Request 802.11v BSS transition to a STA
 *	@param[in] ifname  interface name
 *	@param[in] sta     macaddress of the STA
 *	@param[in] bsss_nr number of neighbor bssids
 *	@param[in] bsss    array of neighbor bssids
 *	@param[in] tmo     timeout (secs) until this request is valid
 *
 * <b>int (*req_btm)(const char *ifname, unsigned char *sta,
 *		int bsss_nr, unsigned char *bsss, struct wifi_btmreq *b)</b>\n
 *	@brief             Request 802.11v BSS transition to a STA
 *	@param[in] ifname  interface name
 *	@param[in] sta     macaddress of the STA
 *	@param[in] bsss_nr number of neighbor bssids
 *	@param[in] bsss    array of neighbors
 *	@param[in] b       additional request parameters
 *
 * <b>int (*get_11rkeys)(const char *ifname, unsigned char *sta, uint8_t *r1khid)</b>\n
 *	@brief             Get 802.11r keys
 *	@param[in] ifname  interface name
 *	@param[in] sta:    macaddress of the STA
 *	@param[out] rikhid:  R1KHID
 *
 * <b>int (*set_11rkeys)(const char *ifname, struct fbt_keys *fk)</b>\n
 *	@brief             Set 802.11r keys
 *	@param[in] ifname  interface name
 *	@param[in] fk      fbt_keys struct
 *
 * <b>int (*chan_switch)(const char *ifname, struct chan_switch_param *param)</b>\n
 *	@brief             Send CSA and attempt to switch channel
 *	@param[in] ifname  interface name
 *	@param[in] param   channel switch announcement parameters
 *
 * <b>int (*mbo_disallow_assoc)(const char *ifname, uint8_t reason)</b>\n
 *	@brief             Configure MBO assoc disallow
 *	@param[in] ifname  interface name
 *	@param[in] reason  reason of blocking, check wifi_mbo_disallow_assoc_reason
 *
 * <b>int (*ap_set_state)(const char *ifname, bool up)</b>\n
 *	@brief             Enable AP interface
 *	@param[in] ifname  interface name
 *	@param[in] up      interface up or down
 *
 * <b>int (*link_measure)(const char *ifname, uint8_t *sta)</b>\n
 *	@brief             Send a RRM Link Measurement Request to STA
 *	@param[in] ifname  interface name
 *	@param[in] sta     macaddress of the STA
 *
 * <b>int (*get_mlo_links)(const char *ifname, enum wifi_band band,
 *			   struct wifi_mlo_link *link, int *num)</b>\n
 *	@brief              Get MLO links we have inside netdev.
 *	@param[in] ifname   interface name
 *	@param[in] band     requested/used band
 *      @param[out] link    table of wifi_mlo_link
 *      @param[in|out] num  array size, number of mlo links
 *
 * <b>int (*sta_info)(const char *ifname, struct wifi_sta *sta)</b>\n
 *	@brief             Get detailed STA information
 *	@param[in] ifname  interface name
 *	@param[out] sta    STA information
 *
 * <b>int (*sta_get_stats)(const char *ifname, struct wifi_sta_stats *s)</b>\n
 *	@brief             Get STA interface statistics
 *	@param[in] ifname  interface name
 *	@param[out] s      STA interface statistics
 *
 * <b>int (*sta_get_ap_info)(const char *ifname, struct wifi_bss *info)</b>\n
 *	@brief             Get BSS information of the STA's AP
 *	@param[in] ifname  interface name
 *	@param[out] info   BSS information of STA's AP
 *
 * <b>int (*sta_disconnect_ap)(const char *ifname, uint32_t reason)</b>\n
 *	@brief             Disconnect from STA's AP
 *	@param[in] ifname  interface name
 *	@param[in] reason  disconnection reason code as in IEEE802.11 Std.
 *
 * <b>int (*sta_get_ifstats)(const char *ifname, struct wifi_sta_ifstats *s)</b>\n
 *	@brief             Get statistics of the interface in STA mode
 *	@param[in] ifname  interface name
 *	@param[out] s      wifi_sta_ifstats structure
 */
struct wifi_iface_ops {
	/*
	 * common ops
	 */
	int (*start_wps)(const char *ifname, struct wps_param wps);
	int (*stop_wps)(const char *ifname);
	int (*get_wps_status)(const char *ifname, enum wps_status *s);
	int (*get_wps_pin)(const char *ifname, unsigned long *pin);
	int (*set_wps_pin)(const char *ifname, unsigned long pin);
	int (*get_wps_device_info)(const char *ifname, struct wps_device *info);

	int (*get_caps)(const char *ifname, struct wifi_caps *caps);
	int (*get_mode)(const char *ifname, enum wifi_mode *mode);
	int (*get_security)(const char *ifname, uint32_t *auth, uint32_t *enc);

	int (*add_vendor_ie)(const char *ifname, struct vendor_iereq *req);
	int (*del_vendor_ie)(const char *ifname, struct vendor_iereq *req);
	int (*get_vendor_ies)(const char *ifname, struct vendor_ie *ies, int *num_ies);

	int (*get_param)(const char *ifname, const char *param, int *len, void *val);
	int (*set_param)(const char *ifname, const char *param, int len, void *val);
	int (*vendor_cmd)(const char *ifname, uint32_t vid, uint32_t subcmd,
			uint8_t *in, int inlen, uint8_t *out, int *outlen);

	int (*subscribe_frame)(const char *ifname, uint8_t type, uint8_t stype);
	int (*unsubscribe_frame)(const char *ifname, uint8_t type, uint8_t stype);
	int (*set_4addr)(const char *ifname, bool enable);
	int (*get_4addr)(const char *ifname, bool *enabled);
	int (*get_4addr_parent)(const char *ifname, char *parent);
	int (*set_vlan)(const char *ifname, struct vlan_param vlan);
	int (*link_measure)(const char *ifname, uint8_t *sta);
	int (*get_mlo_links)(const char *ifname, enum wifi_band band,
			     struct wifi_mlo_link *link, int *num);

	/*
	 * ap interface ops
	 */
	int (*ap_info)(const char *name, struct wifi_ap *ap);
	int (*get_bssid)(const char *ifname, uint8_t *bssid);
	int (*get_ssid)(const char *ifname, char *ssid);
	int (*get_stats)(const char *ifname, struct wifi_ap_stats *s);
	int (*get_beacon_ies)(const char *ifname, uint8_t *ies, int *len);

	int (*get_assoclist)(const char *ifname, uint8_t *stas, int *num_stas);
	int (*get_sta_info)(const char *ifname, uint8_t *addr, struct wifi_sta *info);
	int (*get_sta_stats)(const char *ifname, uint8_t *addr, struct wifi_sta_stats *s);
	int (*disconnect_sta)(const char *ifname, uint8_t *sta, uint16_t reason);
	int (*restrict_sta)(const char *ifname, uint8_t *sta, int enable);
	int (*monitor_sta)(const char *ifname, uint8_t *sta, struct wifi_monsta_config *cfg);
	int (*get_monitor_sta)(const char *ifname, uint8_t *sta, struct wifi_monsta *mon);
	int (*get_monitor_stas)(const char *ifname, struct wifi_monsta *stas, int *num);
	int (*probe_sta)(const char *ifname, uint8_t *sta);

	int (*add_neighbor)(const char *ifname, struct nbr nbr);
	int (*del_neighbor)(const char *ifname, unsigned char *bssid);
	int (*get_neighbor_list)(const char *ifname, struct nbr *nbr, int *nr);
	int (*req_beacon_report)(const char *ifname, uint8_t *sta,
					struct wifi_beacon_req *param, size_t param_sz);
	int (*get_beacon_report)(const char *ifname, uint8_t *sta,
					struct sta_nbr *snbr, int *nr);

	int (*req_bss_transition)(const char *ifname, unsigned char *sta,
			int bsss_nr, unsigned char *bsss, unsigned int tmo);

	int (*req_btm)(const char *ifname, unsigned char *sta,
			int bsss_nr, struct nbr *bsss, struct wifi_btmreq *b);

	int (*get_11rkeys)(const char *ifname, unsigned char *sta, uint8_t *r1khid);
	int (*set_11rkeys)(const char *ifname, struct fbt_keys *fk);

	int (*chan_switch)(const char *ifname, struct chan_switch_param *param);
	int (*mbo_disallow_assoc)(const char *ifname, uint8_t reason);

	int (*ap_set_state)(const char *ifname, bool up);

	/*
	 * sta interface ops
	 */
	int (*sta_info)(const char *name, struct wifi_sta *sta);
	int (*sta_get_stats)(const char *ifname, struct wifi_sta_stats *s);
	int (*sta_get_ap_info)(const char *ifname, struct wifi_bss *info);
	int (*sta_disconnect_ap)(const char *ifname, uint32_t reason);
	int (*sta_get_ifstats)(const char *ifname, struct wifi_sta_ifstats *s);
};

/** struct wifi_metainfo - meta information about wifi module */
struct wifi_metainfo {
	char vendor_id[8];		/**< 0xvvvv */
	char device_id[8];		/**< 0xdddd */
	char drv_data[128];		/**< driver name + version info */
	char fw_data[128];		/**< firmware name + version */
};


struct wifi_driver {
	const char *name;
	const char **(*get_apis)(const char *name);
	int (*info)(const char *name, struct wifi_metainfo *info);
	int (*radio_list)(struct radio_entry *radio, int *num);
	struct wifi_radio_ops radio;
	struct wifi_iface_ops iface;
	int (*register_event)(const char *ifname, struct event_struct *ev,
			      void **evhandle);
	int (*unregister_event)(const char *ifname, void *evhandle);
	int (*recv_event)(const char *ifname, void *evhandle);
	const char *(*get_version)(void);
	struct wifi_driver *fallback;
};

#define RADIO_OP(_p)	radio._p
#define IFACE_OP(_p)	iface._p

#define scan			RADIO_OP(scan)
#define scan_ex			RADIO_OP(scan_ex)
#define get_scan_results	RADIO_OP(get_scan_results)
#define get_bss_scan_result	RADIO_OP(get_bss_scan_result)
#define get_channel		RADIO_OP(get_channel)
#define set_channel		RADIO_OP(set_channel)
#define acs			RADIO_OP(acs)
#define get_bandwidth		RADIO_OP(get_bandwidth)
#define get_supp_bandwidths	RADIO_OP(get_supp_bandwidths)
#define get_noise		RADIO_OP(get_noise)
#define get_maxrate		RADIO_OP(get_maxrate)
#define get_country		RADIO_OP(get_country)
#define get_countrylist 	RADIO_OP(get_countrylist)
#define get_supp_channels	RADIO_OP(get_supp_channels)
#define get_oper_channels	RADIO_OP(get_oper_channels)
#define get_oper_stds		RADIO_OP(get_oper_stds)
#define get_supp_band		RADIO_OP(get_supp_band)
#define get_oper_band		RADIO_OP(get_oper_band)
#define get_supp_opclass	RADIO_OP(get_supp_opclass)
#define get_curr_opclass	RADIO_OP(get_curr_opclass)
#define add_iface		RADIO_OP(add_iface)
#define del_iface		RADIO_OP(del_iface)
#define list_iface		RADIO_OP(list_iface)
#define channels_info		RADIO_OP(channels_info)
#define start_cac		RADIO_OP(start_cac)
#define stop_cac		RADIO_OP(stop_cac)
#define get_opclass_preferences	RADIO_OP(get_opclass_preferences)
#define simulate_radar		RADIO_OP(simulate_radar)

#define get_bssid		IFACE_OP(get_bssid)
#define get_ssid		IFACE_OP(get_ssid)
#define disconnect_sta		IFACE_OP(disconnect_sta)
#define restrict_sta		IFACE_OP(restrict_sta)
#define monitor_sta		IFACE_OP(monitor_sta)
#define get_monitor_sta		IFACE_OP(get_monitor_sta)
#define get_monitor_stas	IFACE_OP(get_monitor_stas)
#define vendor_cmd		IFACE_OP(vendor_cmd)
#define get_assoclist		IFACE_OP(get_assoclist)
#define get_sta_info		IFACE_OP(get_sta_info)
#define get_sta_stats		IFACE_OP(get_sta_stats)
#define set_4addr		IFACE_OP(set_4addr)
#define get_4addr		IFACE_OP(get_4addr)
#define get_4addr_parent	IFACE_OP(get_4addr_parent)
#define set_vlan		IFACE_OP(set_vlan)
#define get_beacon_ies		IFACE_OP(get_beacon_ies)
#define chan_switch		IFACE_OP(chan_switch)
#define link_measure		IFACE_OP(link_measure)
#define mbo_disallow_assoc	IFACE_OP(mbo_disallow_assoc)
#define ap_set_state		IFACE_OP(ap_set_state)
#define get_mlo_links		IFACE_OP(get_mlo_links)


/* List of the APIs this library provides */

int wifi_driver_info(const char *name, struct wifi_metainfo *info);

/* WiFi radio APIs */
int wifi_radio_list(struct radio_entry *radio, int *num);
int wifi_radio_is_multiband(const char *ifname, bool *res);
int wifi_radio_info(const char *name, struct wifi_radio *radio);
int wifi_radio_info_band(const char *ifname, enum wifi_band band, struct wifi_radio *info);
int wifi_get_supp_band(const char *name, uint32_t *bands);
int wifi_get_oper_band(const char *name, enum wifi_band *band);
int wifi_radio_get_ifstatus(const char *ifname, ifstatus_t *f);
int wifi_radio_get_caps(const char *name, struct wifi_caps *caps);
int wifi_radio_get_band_caps(const char *name, enum wifi_band band, struct wifi_caps *caps);
int wifi_get_supp_stds(const char *name, uint8_t *std);
int wifi_get_band_supp_stds(const char *name, enum wifi_band band, uint8_t *std);
int wifi_get_oper_stds(const char *name, uint8_t *std);
int wifi_get_band_oper_stds(const char *name, enum wifi_band band, uint8_t *std);

int wifi_get_country(const char *name, char *alpha2);
int wifi_get_countrylist(const char *name, char *cc, int *num);
int wifi_get_channel(const char *ifname, uint32_t *channel, enum wifi_bw *bw);
int wifi_get_band_channel(const char *ifname, enum wifi_band band, uint32_t *channel, enum wifi_bw *bw);
int wifi_set_channel(const char *ifname, uint32_t channel, enum wifi_bw bw);
int wifi_get_supp_channels(const char *name, uint32_t *chlist, int *num,
			   const char *alpha2, enum wifi_band f, enum wifi_bw b);
int wifi_get_oper_channels(const char *name, uint32_t *chlist, int *num,
			   const char *alpha2, enum wifi_band f, enum wifi_bw b);

int wifi_get_supp_opclass(const char *name, int *num_opclass,
			  struct wifi_opclass *o);
int wifi_get_band_supp_opclass(const char *name, enum wifi_band band,
			       int *num_opclass, struct wifi_opclass *o);

int wifi_get_curr_opclass(const char *name, struct wifi_opclass *o);
int wifi_get_band_curr_opclass(const char *name, enum wifi_band band, struct wifi_opclass *o);

int wifi_get_bandwidth(const char *name, enum wifi_bw *bw);
int wifi_get_supp_bandwidths(const char *name, uint32_t *bws);
int wifi_get_band_supp_bandwidths(const char *name, enum wifi_band band, uint32_t *bws);
int wifi_get_maxrate(const char *name, unsigned long *rate);
int wifi_get_band_maxrate(const char *name, enum wifi_band band, unsigned long *rate);
int wifi_radio_get_basic_rates(const char *name, int *num, uint32_t *rates);
int wifi_radio_get_oper_rates(const char *name, int *num, uint32_t *rates);
int wifi_radio_get_supp_rates(const char *name, int *num, uint32_t *rates);
int wifi_radio_get_stats(const char *ifname, struct wifi_radio_stats *s);
int wifi_radio_get_band_stats(const char *ifname, enum wifi_band band, struct wifi_radio_stats *s);

int wifi_scan(const char *name, struct scan_param *p);
int wifi_scan_ex(const char *ifname, struct scan_param_ex *sp);
int wifi_get_scan_results(const char *name, struct wifi_bss *bsss, int *num);
int wifi_get_bss_scan_result(const char *name, uint8_t *bssid,
			     struct wifi_bss_detail *b);

int wifi_get_noise(const char *ifname, int *noise);
int wifi_get_band_noise(const char *ifname, enum wifi_band band, int *noise);

int wifi_acs(const char *name, struct acs_param *p);

int wifi_radio_get_param(const char *name, const char *param, int *len, void *val);
int wifi_radio_set_param(const char *name, const char *param, int len, void *val);

int wifi_radio_get_hwaddr(const char *name, uint8_t *hwaddr);

int wifi_add_iface(const char *name, enum wifi_mode m, char *argv[]);
int wifi_del_iface(const char *name, const char *ifname);
int wifi_list_iface(const char *name, struct iface_entry *iface, int *num);

int wifi_channels_info(const char *name, struct chan_entry *channel, int *num);
int wifi_channels_info_band(const char *name, enum wifi_band band, struct chan_entry *channel, int *num);

int wifi_start_cac(const char *name, int channel, enum wifi_bw bw,
		   enum wifi_cac_method method);
int wifi_stop_cac(const char *name);
int wifi_get_opclass_preferences(const char *name, struct wifi_opclass *opclass,
				 int *num);
int wifi_get_band_opclass_preferences(const char *name, enum wifi_band band,
				      struct wifi_opclass *opclass, int *num);
int wifi_simulate_radar(const char *name, struct wifi_radar_args *radar);

/** WiFi interface APIs */
int wifi_start_wps(const char *ifname, struct wps_param wps);
int wifi_stop_wps(const char *ifname);
int wifi_get_wps_status(const char *ifname, enum wps_status *s);
int wifi_get_wps_pin(const char *ifname, unsigned long *pin);
int wifi_set_wps_pin(const char *ifname, unsigned long pin);
int wifi_get_wps_device_info(const char *ifname, struct wps_device *info);

int wifi_get_caps(const char *ifname, struct wifi_caps *caps);
int wifi_get_mode(const char *ifname, enum wifi_mode *mode);
int wifi_get_security(const char *ifname, uint32_t *auth, uint32_t *enc);

int wifi_add_vendor_ie(const char *ifname, struct vendor_iereq *req);
int wifi_del_vendor_ie(const char *ifname, struct vendor_iereq *req);
int wifi_get_vendor_ies(const char *ifname, struct vendor_ie *ies, int *num_ies);

int wifi_get_param(const char *name, const char *param, int *len, void *val);
int wifi_set_param(const char *name, const char *param, int len, void *val);
int wifi_vendor_cmd(const char *ifname, uint32_t vid, uint32_t subcmd,
		    uint8_t *in, int ilen, uint8_t *out, int *olen);

int wifi_subscribe_frame(const char *ifname, uint8_t type, uint8_t stype);
int wifi_unsubscribe_frame(const char *ifname, uint8_t type, uint8_t stype);
int wifi_set_4addr(const char *ifname, bool enable);
int wifi_get_4addr(const char *ifname, bool *enabled);
int wifi_get_4addr_parent(const char *ifname, char *parent);
int wifi_set_vlan(const char *ifname, struct vlan_param vlan);
int wifi_link_measure(const char *ifname, uint8_t *sta);
int wifi_get_mlo_links(const char *ifname, enum wifi_band band, struct wifi_mlo_link *link, int *num);


/* WiFi AP interface specific APIs */
int wifi_ap_info(const char *name, struct wifi_ap *ap);
int wifi_get_bssid(const char *ifname, uint8_t *bssid);
int wifi_get_ssid(const char *ifname, char *ssid);
int wifi_ap_get_stats(const char *ifname, struct wifi_ap_stats *s);
int wifi_get_beacon_ies(const char *ifname, uint8_t *ies, int *len);

int wifi_get_assoclist(const char *ifname, uint8_t *stas, int *num_stas);
int wifi_get_sta_info(const char *ifname, uint8_t *addr, struct wifi_sta *info);
int wifi_get_sta_stats(const char *ifname, uint8_t *addr, struct wifi_sta_stats *s);
int wifi_disconnect_sta(const char *ifname, uint8_t *sta, uint16_t reason);
int wifi_restrict_sta(const char *ifname, uint8_t *sta, int enable);
int wifi_monitor_sta(const char *ifname, uint8_t *sta, struct wifi_monsta_config *cfg);
int wifi_get_monitor_sta(const char *ifname, uint8_t *sta, struct wifi_monsta *mon);
int wifi_get_monitor_stas(const char *ifname, struct wifi_monsta *stas, int *num);
int wifi_probe_sta(const char *ifname, uint8_t *sta);

int wifi_add_neighbor(const char *ifname, struct nbr nbr);
int wifi_del_neighbor(const char *ifname, unsigned char *bssid);
int wifi_get_neighbor_list(const char *ifname, struct nbr *nbrs, int *nr);
int wifi_req_beacon_report(const char *ifname, uint8_t *sta,
			   struct wifi_beacon_req *param, size_t param_sz);
int wifi_get_beacon_report(const char *ifname, uint8_t *sta,
			   struct sta_nbr *snbrs, int *nr);
int wifi_req_bss_transition(const char *ifname, unsigned char *sta,
			    int bsss_nr, unsigned char *bsss,
			    unsigned int tmo);

int wifi_req_btm(const char *ifname, uint8_t *sta, int bsss_nr, struct nbr *bsss,
		 struct wifi_btmreq *b);

int wifi_get_11rkeys(const char *ifname, unsigned char *sta, uint8_t *r1khid);
int wifi_set_11rkeys(const char *ifname, struct fbt_keys *fk);
int wifi_chan_switch(const char *ifname, struct chan_switch_param *param);
int wifi_mbo_disallow_assoc(const char *ifname, uint8_t reason);
int wifi_ap_set_state(const char *ifname, bool up);


/* WiFi STA interface APIs */
int wifi_sta_info(const char *ifname, struct wifi_sta *sta);
int wifi_sta_get_stats(const char *ifname, struct wifi_sta_stats *s);
int wifi_sta_get_ap_info(const char *ifname, struct wifi_bss *info);
int wifi_sta_disconnect_ap(const char *ifname, uint32_t reason);
int wifi_sta_get_stats_ex(const char *ifname, struct wifi_sta_ifstats *s);


/* WiFi events */
enum wifi_event_type {
	WIFI_EVENT_UNDEFINED,
	WIFI_EVENT_VENDOR = 1,
	WIFI_EVENT_SCAN_START,
	WIFI_EVENT_SCAN_END,
	WIFI_EVENT_SCAN_ABORT,
	WIFI_EVENT_STA_ASSOC,
	WIFI_EVENT_STA_DISASSOC,
	WIFI_EVENT_CAC_START,
	WIFI_EVENT_CAC_END,
	WIFI_EVENT_RADAR_DETECT,
	WIFI_EVENT_CHANNEL,
	WIFI_EVENT_WPS_OVERLAP,
	WIFI_EVENT_WPS_SUCCESS,
	WIFI_EVENT_BTM_RESPONSE,
	WIFI_EVENT_FRAME,

	/* add new types here */

	WIFI_EVENT_DEFAULT = 99,   /* last resort */
};


int wifi_register_event(const char *ifname, struct event_struct *ev,
			void **evhandle);
int wifi_unregister_event(const char *ifname, void *evhandle);
int wifi_recv_event(const char *ifname, void *evhandle);


/** vendor agnostic wifi APIs */

/** Get interface state */
int wifi_get_ifstatus(const char *ifname, ifstatus_t *f);

/** Get interface operational state */
int wifi_get_ifoperstatus(const char *ifname, ifopstatus_t *opstatus);

/** Get valid channels for given country, band and bandwidth */
int wifi_get_valid_channels(const char *ifname, enum wifi_band b,
					enum wifi_bw bw, const char *cc,
					uint32_t *chlist, int *n);


/** Get current operating class of the wifi radio */
int wifi_get_opclass(const char *name, struct wifi_opclass *o);
int wifi_get_band_opclass(const char *name, enum wifi_band band, struct wifi_opclass *o);

/** Get list of channels for a global opclass */
int wifi_opclass_to_channels(uint32_t opclass, int *num, uint32_t *channels);

/** Get operating class for current channel and HT20 */
int wifi_get_opclass_ht20(const char *name, struct wifi_opclass *o);

/** Get list of operating classes supported by the wifi radio */
int wifi_get_supported_opclass(const char *name, int *num_opclass,
						struct wifi_opclass *o);

/** Get list of preferred operating classes */
int wifi_get_opclass_pref(const char *name, int *num_opclass,
			  struct wifi_opclass *o);
int wifi_get_band_opclass_pref(const char *name, enum wifi_band band,
			       int *num_opclass, struct wifi_opclass *o);

/** Get sideband base on ctrl channel and bw */
int wifi_get_sideband(const char *name, enum wifi_chan_ext *sideband);

/** Check if given channel is DFS */
bool wifi_is_dfs_channel(const char *name, int channel, int bandwidth);

/* Get list of APIs that this library supports */
const char **libwifi_get_apis(char *name);

/* Get library version */
const char *libwifi_get_version(void);

/* Helper function to check if an API is defined by a driver implementation */
extern int libwifi_supports(const char *ifname, char *api_name);

#endif /* WIFIOPS_H */
