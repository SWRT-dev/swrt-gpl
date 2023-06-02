/*
 * qca.c - for mac80211 based Qualcomm drivers.
 *
 * Copyright (C) 2020-2023 iopsys Software Solutions AB. All rights reserved.
 *
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

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <sys/types.h>
#include <fcntl.h>
#include <net/if.h>
#include <time.h>
#include <syslog.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>

#include <easy/easy.h>
#include "wifi.h"
#include "wifiutils.h"
#include "nlwifi.h"
#include "debug.h"
#include "drivers.h"


static int qca_simulate_radar_debugfs(const char *phyname, struct wifi_radar_args *radar)
{
	char path[512] = {};
	int fd;
	int char_num;

	char_num = snprintf(path, sizeof(path), "/sys/kernel/debug/ieee80211/%s/ath12k/dfs_simulate_radar", phyname);
	if (WARN_ON(char_num < 0))
		return -1;

	if (WARN_ON(char_num >= (int)sizeof(path)))
		return -1;

	fd = open(path, O_WRONLY);
	if (WARN_ON(fd < 0))
		return -1;

	/* Trigger radar
	 * echo 1 > /sys/kernel/debug/ieee80211/phy2/ath12k/dfs_simulate_radar
	 */
	if (!write(fd, "1", 1)) {
		close(fd);
		return -1;
	}

	close(fd);
	return 0;
}

int radio_simulate_radar(const char *name, struct wifi_radar_args *radar)
{
	if (WARN_ON(!radar))
		return -1;

	libwifi_dbg("[%s] %s called ch:%d, bandwidth:%d, type:0x%x, subband:0x%x\n",
		    name, __func__, radar->channel, radar->bandwidth, radar->type,
		    radar->subband_mask);

	return qca_simulate_radar_debugfs(name, radar);
}

const struct wifi_driver qca_driver = {
	.name = "wlan,phy",

	/* Radio/phy callbacks */
	.simulate_radar = radio_simulate_radar,

	/* Interface/vif common callbacks */

	/* Interface/vif ap callbacks */

	/* Interface/vif sta callbacks */

	/* use fallback driver ops */
	.fallback = &mac80211_driver,
};
