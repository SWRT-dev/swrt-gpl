# Wifimngr

[wifimngr](https://dev.iopsys.eu/iopsys/wifimngr)

## Introduction

Wifimngr implements the wifi data model, publishing the wifi objects over ubus. In order to remain agnostic the vendor and underlying wifi modules present in the system, wifimngr uses the easy-soc-libs libwifi APIs to expose supported methods with its objects.

## Overview

Wifimngr manages three areas of wifi functionality:
1. WiFi Statistics
2. Access Points
3. Radios
4. WiFi Protected Setup (WPS)


### UCI Config

An example UCI wireless configuration file creating one access point interface, `test5`:

````bash
config wifi-device 'test5'
    option channel 'auto'
    option hwmode 'auto'
    option country 'DE'
    option band 'a'
    option bandwidth '80'

config wifi-iface
    option device 'test5'
    option ifname 'test5'
    option mode 'ap'
    option encryption 'psk2'
````

For more info on the UCI wireless configuration see [link](./docs/api/uci.wireless.md)

### Ubus API

This is a verbose print of all methods published to ubus on a device with one access point interface `test5`:

````bash
'wifi' @f2f72310
	"status":{}
'wifi.ap.test5' @57ac8570
	"status":{}
	"stats":{}
	"assoclist":{}
	"stations":{"sta":"String"}
	"disconnect":{"sta":"String","reason":"Integer"}
	"probe_sta":{"sta":"String"}
	"monitor_add":{"sta":"String"}
	"monitor_del":{"sta":"String"}
	"monitor_get":{"sta":"String"}
	"subscribe_frame":{"type":"Integer","stype":"Integer"}
	"unsubscribe_frame":{"type":"Integer","stype":"Integer"}
	"add_neighbor":{"bssid":"String","channel":"Integer","bssid_info":"String","reg":"Integer","phy":"Integer"}
	"del_neighbor":{"bssid":"String"}
	"list_neighbor":{"ssid":"String","client":"String"}
	"request_neighbor":{"client":"String","opclass":"Integer","channel":"Integer","duration":"Integer","mode":"String","bssid":"String","reporting_detail":"Integer","ssid":"String","channel_report":"Array","request_element":"Array"}
	"request_btm":{"client":"String","bssid":"Array","neighbor":"Array","mode":"Integer","disassoc_tmo":"Integer","validity_int":"Integer","bssterm_dur":"Integer","mbo_reason":"Integer","mbo_cell_pref":"Integer","mbo_reassoc_delay":"Integer"}
	"request_transition":{"client":"String","bssid":"Array","timeout":"Integer"}
	"assoc_control":{"client":"Array","enable":"Integer"}
	"add_vendor_ie":{"mgmt":"Integer","oui":"String","data":"String"}
	"del_vendor_ie":{"mgmt":"Integer","oui":"String","data":"String"}
	"dump_beacon":{}
	"chan_switch":{"count":"Integer","freq":"Integer","sec_chan_offset":"Integer","cf1":"Integer","cf2":"Integer","bw":"Integer","blocktx":"Boolean","ht":"Boolean","vht":"Boolean"}
	"measure_link":{"sta":"String"}
	"mbo_disallow_assoc":{"reason":"Integer"}
	"up":{}
	"down":{}
'wifi.radio.test5' @e662e933
	"status":{}
	"stats":{}
	"get":{"param":"String"}
	"scan":{"ssid":"String","bssid":"String","channel":"Integer"}
	"scanresults":{"bssid":"String"}
	"autochannel":{"interval":"Integer","algo":"Integer","scans":"Integer"}
'wifi.wps' @4eb8cbd8
	"start":{"ifname":"String","mode":"String","role":"String","pin":"String"}
	"stop":{}
	"status":{"ifname":"String"}
	"generate_pin":{}
	"validate_pin":{"pin":"String"}
	"showpin":{"ifname":"String"}
	"setpin":{"ifname":"String","pin":"String"}
````

For more info on the Ubus API see [function specification](./docs/functionspec.md) or go directly to the generated [docs](./docs/ubus.splash.md).

**Table 1. WiFi events over UBUS**

Event | Event Data
------|-----------
wifi.radio |_{"ifname":"wlan0","event":"ap-disabled", "data": {"bssid":"00:20:07:11:22:33}}_|
wifi.radio |_{"ifname":"wlan0","event":"ap-enabled", "data": {"bssid":"00:20:07:11:22:33"}}_|
wifi.radio |_{"ifname":"wlan1","event":"cac-start","data":{"channel":"52","bandwidth":80,"cac_time":60}}_|
wifi.radio |_{"ifname":"wlan1","event":"cac-end","data":{"channel":"52","bandwidth":80,"success":1}}_|
wifi.radio |_{"ifname":"wlan1","event":"channel","data":{"channel":"52", "bandwidth":80}}_|
wifi.radio |_{"ifname":"wlan1","event":"radar","data":{"channel":"52","bandwidth":80}}_|
wifi.radio |_{"ifname":"wlan1","event":"nop-end","data":{"channel":"52", "bandwidth":80}}_|
wifi.radio |_{"ifname":"wlan1","event":"acs-start"}_|
wifi.radio |_{"ifname":"wlan1","event":"acs-end","data":{"channel":"149", "bandwidth":80}}_|
wifi.radio |_{"ifname":"wlan1","event":"acs-failed"}_|
||
||
wifi.iface |_{"ifname":"wlan1","event":"up"}_|
wifi.iface |_{"ifname":"wlan1","event":"down"}_|
wifi.iface |_{"ifname":"wlan1","event":"created", "data":{"type":"sta|ap|monitor", "macaddr":"00:10:20:33:44:55"}}_|
wifi.iface |_{"ifname":"wlan1","event":"deleted" }_|
wifi.iface |_{"ifname":"wlan1","event":"wps-start", "data":{"method":"pbc"}}_|
wifi.iface |_{"ifname":"wlan1","event":"wps-stop", "data":{"method":"pbc"}}_|
wifi.iface |_{"ifname":"wlan1","event":"wps-success"}_|
wifi.iface |_{"ifname":"wlan1","event":"wps-success", "data": {"credentials": {"..."}}}_|
wifi.iface |_{"ifname":"wlan1","event":"wps-timeout"}_|
wifi.iface |_{"ifname":"wlan1","event":"wps-overlap"}_|
wifi.iface |_{"ifname":"wlan0","event":"frame-rx","data":{"hexstring":"00003a01...0100"}}_|
||
||
wifi.sta |_{"ifname":"wlan1","event":"connected","data":{"macaddr":"44:d4:37:42:47:bf", "bssid":"00:20:07:11:22:33"}}_|
wifi.sta |_{"ifname":"wlan1","event":"disconnected","data":{"macaddr":"44:d4:37:42:47:bf", "bssid":"00:20:07:11:22:33"}}_|
wifi.sta |_{"ifname":"wlan1","event":"wds-station-added","data":{"ifname":"wlan1.sta1","macaddr":"44:d4:37:42:47:bf"}}_|
wifi.sta |_{"ifname":"wlan1","event":"wds-station-removed","data":{"ifname":"wlan1.sta1","macaddr":"44:d4:37:42:47:bf"}}_|
wifi.sta |_{"ifname":"wlan1","event":"btm-resp","data":{"macaddr":"30:10:b3:6d:8d:ba","status":"0","target_bssid":"44:d4:37:42:1f:16"}}_|
wifi.sta |_{"ifname":"wlan0","event":"btm-resp","data":{"macaddr":"30:10:b3:6d:8d:ba","status":"1"}}_|
wifi.sta |_{"ifname":"wlan0","event":"monitor","data":{"macaddr":"44:d4:37:42:47:be","rssi":-35,"source":"mgmt"}}_|
||
wifi.sta |_{"ifname":"wlan0","event":"bcn-report","data":{"macaddr":"30:10:b3:6d:8d:ba","token":"1","mode":"00","nbr":[{"bssid":"44:d4:37:42:1f:16","channel":"11","op_class":"81","phy_type":"7","rcpi":"148","rsni":"255"}]}}_|
---------------


### Wifi Statistics

The `wifi` object publishes gathered radio and interface statistics.

```
'wifi' @f2f72310
	"status":{}
```

For info on the `wifi` API see [link](./docs/api/wifi.md#wifi)

### Access Point

An object will be published on ubus for each interface operating as an access point, managing operations and data for wifi clients, neighbor nodes and interface statistics.

To parse access point interfaces, wifimngr reads the wireless configuration file (`/etc/config/wireless`), looking for `wifi-iface` sections, with the option `mode` specified as `ap`.

```
config wifi-iface
	option device 'test5'
	option ifname 'test5'
	option mode 'ap'
```

Wifimngr will query libwifi to find available API calls for the driver, in order to dynamically publish available methods to ubus.

```
root@eagle:~# ubus -v list wifi.ap.test5
'wifi.ap.test5' @a939a75c
	"status":{}
	"stats":{}
	"assoclist":{}
	"stations":{"sta":"String"}
	"disconnect":{"sta":"String","reason":"Integer"}
	"probe_sta":{"sta":"String"}
	"monitor_add":{"sta":"String"}
	"monitor_del":{"sta":"String"}
	"monitor_get":{"sta":"String"}
	"subscribe_frame":{"type":"Integer","stype":"Integer"}
	"unsubscribe_frame":{"type":"Integer","stype":"Integer"}
	"add_neighbor":{"bssid":"String","channel":"Integer","bssid_info":"String","reg":"Integer","phy":"Integer"}
	"del_neighbor":{"bssid":"String"}
	"list_neighbor":{"ssid":"String","client":"String"}
	"request_neighbor":{"client":"String","opclass":"Integer","channel":"Integer","duration":"Integer","mode":"String","bssid":"String","reporting_detail":"Integer","ssid":"String","channel_report":"Array","request_element":"Array"}
	"request_transition":{"client":"String","bssid":"Array","timeout":"Integer"}
	"assoc_control":{"client":"Array","enable":"Integer"}
	"add_vendor_ie":{"mgmt":"Integer","oui":"String","data":"String"}
	"del_vendor_ie":{"mgmt":"Integer","oui":"String","data":"String"}
	"dump_beacon":{}
	"chan_switch":{"count":"Integer","freq":"Integer","sec_chan_offset":"Integer","cf1":"Integer","cf2":"Integer","bw":"Integer","blocktx":"Boolean","ht":"Boolean","vht":"Boolean"}
	"measure_link":{"sta":"String"}
	"mbo_disallow_assoc":{"reason":"Integer"}
	"up":{}
	"down":{}
```

For info on the access point API see [link](./docs/api/wifi.ap.md#wifiapname)

### Radio

Similarily to access point interfaces, wifimngr will publish one object per available radio, parsed from the wireless configuration, by `wifi-device` sections. The methods are added dynamically by quering for supported API calls from libwifi.

```
config wifi-device 'test5'
	option channel 'auto'
	option hwmode 'auto'
	option country 'DE'
	option band 'a'
	option bandwidth '80'
```

The radio object manages radio data and statistics, channel selection and wifi scan operations.

```
root@:/opt/work# ubus -v list wifi.radio.test5
'wifi.radio.test5' @fa7d185d
	"status":{}
	"stats":{}
	"get":{"param":"String"}
	"scan":{"ssid":"String","bssid":"String","channel":"Integer"}
	"scanresults":{"bssid":"String"}
	"autochannel":{"interval":"Integer","algo":"Integer","scans":"Integer"}
```
For info on the radio API see [link](./docs/api/wifi.radio.md#wifiradioname)

### Wifi Protected Setup

Lastly, wifimngr exposes WPS functionality to ubus.

The configuration is read from the `/etc/config/wireless`. To enable WPS the `wps` option has to be set to `1`.

```
config wifi-iface
	option wps '1'
```

To see more configuration options see [link](./docs/api/uci.wireless.md), under the `wifi-iface` section.


```
root@:/opt/work# ubus -v list wifi.wps
'wifi.wps' @78c52f3b
	"start":{"ifname":"String","mode":"String","role":"String","pin":"String"}
	"stop":{}
	"status":{"ifname":"String"}
	"generate_pin":{}
	"validate_pin":{"pin":"String"}
	"showpin":{"ifname":"String"}
	"setpin":{"ifname":"String","pin":"String"}
```

For info on the WPS API see [link](./docs/api/wifi.wps.md#wifiwps)


## Tests

This section will give a brief overview of the tests for wifimngr, for a more detailed report see [test specification](./docs/testspec.md)

To test wifimngr, the scope of the tests has to be clearly defined, as wifimngr is heavily dependent on libwifi:

1. Verify linkage between wifimngr and libwifi APIs
2. Verify that the wifi structures are correctly prepared and return data is used correctly by wifimngr
3. Verify that API calls successfully reach libwifi and if passed input is of correct format

As the test environment runs in a ubuntu 16.04 docker environment, with little possibility to prepare wifi drivers and kernel version, the easy-soc-libs had to be extended to support APIs to run on a test platform, returning dummy data for getters, and for setters, logging that the call made with timestamp and arguments.

To ensure full coverage, the getters are tested by the ubus-api-validation tool, using json-schemas to validate accurate output. The setters are tested by cmocka tests, creating a libwifimngr.so shared library, invoking its ubus API functions directly, verifying that the call went through, and that the arguments were passed correctly, by parsing the test log file found at `/tmp/test.log`.

Additionally, the cmocka tests verify that netlink event callbacks are
registered and invoked.

## Dependencies ##

To successfully build wifimngr, the following libraries are needed:

| Dependency  		| Link                                       						| License        |
| ----------------- | ---------------------------------------------------------------- 	| -------------- |
| libuci      		| https://git.openwrt.org/project/uci.git     					 	| LGPL 2.1       |
| libubox     		| https://git.openwrt.org/project/libubox.git 					 	| BSD            |
| libubus     		| https://git.openwrt.org/project/ubus.git    					 	| LGPL 2.1       |
| libjson-c   		| https://s3.amazonaws.com/json-c_releases    					 	| MIT            |
| libwifi	  		| https://dev.iopsys.eu/iopsys/easy-soc-libs/tree/devel/libwifi    	| GNU GPL2       |
| libnl3	  		| 											 					 	| 			     |
| libblobmsg_json	| 											 					 	| 			     |
| libnl-genl  		|                                             					 	|                |

Additionally, in order to build with the tests, the following libraries are needed:

| Dependency  				| Link                                       				| License		|
| ------------------------- | --------------------------------------------------------- | ------------- |
| libjson-schema-validator 	| https://github.com/pboettch/json-schema-validator     	| LGPL 2.1		|
| libjson-validator			| https://dev.iopsys.eu/iopsys/json-schema-validator		| 				|
| libjson-editor			| https://dev.iopsys.eu/iopsys/json-editor   				| 				|

