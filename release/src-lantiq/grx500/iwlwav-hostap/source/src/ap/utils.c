/*
 * AP mode helper functions
 * Copyright (c) 2009, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include "includes.h"

#include "common.h"
#include "common/ieee802_11_defs.h"
#include "fst/fst.h"
#include "sta_info.h"
#include "hostapd.h"


int hostapd_register_probereq_cb(struct hostapd_data *hapd,
				 int (*cb)(void *ctx, const u8 *sa,
					   const u8 *da, const u8 *bssid,
					   const u8 *ie, size_t ie_len,
					   int ssi_signal),
				 void *ctx)
{
	struct hostapd_probereq_cb *n;

	n = os_realloc_array(hapd->probereq_cb, hapd->num_probereq_cb + 1,
			     sizeof(struct hostapd_probereq_cb));
	if (n == NULL)
		return -1;

	hapd->probereq_cb = n;
	n = &hapd->probereq_cb[hapd->num_probereq_cb];
	hapd->num_probereq_cb++;

	n->cb = cb;
	n->ctx = ctx;

	return 0;
}


struct prune_data {
	struct hostapd_data *hapd;
	const u8 *addr;
};

static int prune_associations(struct hostapd_iface *iface, void *ctx)
{
	struct prune_data *data = ctx;
	struct sta_info *osta;
	struct hostapd_data *ohapd;
	size_t j;

	for (j = 0; j < iface->num_bss; j++) {
		ohapd = iface->bss[j];
		if (ohapd == data->hapd)
			continue;
#ifdef CONFIG_TESTING_OPTIONS
		if (ohapd->conf->skip_prune_assoc)
			continue;
#endif /* CONFIG_TESTING_OPTIONS */
#ifdef CONFIG_FST
		/* Don't prune STAs belong to same FST */
		if (ohapd->iface->fst &&
		    data->hapd->iface->fst &&
		    fst_are_ifaces_aggregated(ohapd->iface->fst,
					      data->hapd->iface->fst))
			continue;
#endif /* CONFIG_FST */
		osta = ap_get_sta(ohapd, data->addr);
		if (!osta)
			continue;

		wpa_printf(MSG_INFO, "%s: Prune association for " MACSTR,
			   ohapd->conf->iface, MAC2STR(osta->addr));
		ap_sta_disassociate(ohapd, osta, WLAN_REASON_UNSPECIFIED);
	}

	return 0;
}

/**
 * hostapd_prune_associations - Remove extraneous associations
 * @hapd: Pointer to BSS data for the most recent association
 * @addr: Associated STA address
 *
 * This function looks through all radios and BSS's for previous
 * (stale) associations of STA. If any are found they are removed.
 */
void hostapd_prune_associations(struct hostapd_data *hapd, const u8 *addr)
{
	struct prune_data data;
	data.hapd = hapd;
	data.addr = addr;
	if (hapd->iface->interfaces &&
	    hapd->iface->interfaces->for_each_interface)
		hapd->iface->interfaces->for_each_interface(
			hapd->iface->interfaces, prune_associations, &data);
}

#define hostapd_skip_space while(isspace(*s)) s++
#define get_number(x, s, e) (x = strtol(s, e, 10), *e != s)
static int get_rnge(const char *s, char **e, struct hostapd_iface *iface,
  int (*range_cb)(struct hostapd_iface*, int, int));
/**
 * hostapd_get_list - parses a list of numbers, in the form of "1 6 11-13".
 * Range may start with one or several spaces.
 * @s: String of numbers
 * @e: Upon exit contains pointer to place in s that caused parsing failure
 * @number_cb: function than will be called when number is found in s. Should
 *             return 0 on success
 * @range_cb: function than will be called when range is found in s. Should
 *            return 0 on success
 *
 * Returns 0 - success; -1 - failure
 */
int hostapd_get_list(const char *s, char **e, struct hostapd_iface *iface,
  int (*number_cb)(struct hostapd_iface*, int),
  int (*range_cb)(struct hostapd_iface*, int, int))
{
  int x, ret;
  int range, number = 0;

  hostapd_skip_space;
  /* empty string - OK */
  if ((*s) == '\0')
    return 0;
  while (1) {
    range = get_rnge(s, e, iface, range_cb);
    /* returns -1 on error */
    if (range == -1)
      break;
    /* returns 0 if range not found */
    if (!range) {
      number = get_number(x, s, e);
      if (number && number_cb) {
        ret = number_cb(iface, x);
        if (ret)
          break;
      }
    }
    if (!range && !number) break;
    s = *e;

    if ((*s) == '\0')
      return 0;
    if ((*s) == ' ') { s++; continue; }
    break;
  }
  *(const char **)e = s;
  return -1;
}

static int get_rnge(const char *s, char **e, struct hostapd_iface *iface,
  int (*range_cb)(struct hostapd_iface*, int, int))
{
  int x, y, ret;
  char *ee;
  if (!get_number(x, s, &ee))
    return 0;
  s = ee;

  if (*s != '-') {
    *(const char **)e = s;
    return 0;
  }
  s++;
  if (!get_number(y, s, e))
    return 0;
  if (y <= x)
    return 0;
  if (range_cb) {
    ret = range_cb(iface, x, y);
    if (ret)
      return ret;
  }
  return 1;
}
