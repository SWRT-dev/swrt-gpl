# Function Specification

The scope of wifimngr is to expose the libwifi APIs over ubus.

```
root@dc667cfd1b01:/builds/iopsys/wifimngr# ubus -v list
'wifi' @0af621bb
	"status":{}
'wifi.ap.test5' @86000d35
	"status":{}
	"stats":{}
	"assoclist":{}
	"stations":{"sta":"String"}
	"disconnect":{"sta":"String","reason":"Integer"}
	"monitor_add":{"sta":"String"}
	"monitor_del":{"sta":"String"}
	"monitor_get":{"sta":"String"}
	"subscribe_frame":{"type":"Integer","stype":"Integer"}
	"unsubscribe_frame":{"type":"Integer","stype":"Integer"}
	"add_neighbor":{"bssid":"String","channel":"Integer","bssid_info":"String","reg":"Integer","phy":"Integer"}
	"del_neighbor":{"bssid":"String"}
	"list_neighbor":{"ssid":"String","client":"String"}
	"request_neighbor":{"ssid":"String","client":"String"}
	"request_transition":{"client":"String","bssid":"Array","timeout":"Integer"}
	"add_vendor_ie":{"mgmt":"Integer","oui":"String","data":"String"}
	"del_vendor_ie":{"mgmt":"Integer","oui":"String","data":"String"}
	"dump_beacon":{}
'wifi.radio.test5' @fac30612
	"status":{}
	"stats":{}
	"get":{"param":"String"}
	"scan":{"ssid":"String","bssid":"String","channel":"Integer"}
	"scanresults":{"bssid":"String"}
	"autochannel":{"interval":"Integer","algo":"Integer","scans":"Integer"}
	"add_iface":{"args":"Table","config":"Boolean"}
	"del_iface":{"ifname":"String","config":"Boolean"}
'wifi.wps' @76eb99e4
	"start":{"ifname":"String","mode":"String","role":"String","pin":"String"}
	"stop":{}
	"status":{"ifname":"String"}
	"generate_pin":{}
	"validate_pin":{"pin":"String"}
	"showpin":{"ifname":"String"}
	"setpin":{"ifname":"String","pin":"String"}
```

# Contents
* [wifi](#wifi)
* [wifi.ap.\<name\>](#wifiapname)
* [wifi.radio.\<name\>](#wifiradioname)
* [wifi.wps](#wifiwps)

## APIs

Wifimngr publishes four different types objects, `wifi`, `wifi.ap.<name>`,
`wifi.radio.<name>` and `wifi.wps`.

### wifi

An object that publishes wifi radio and interface information.

| Method			| Function ID		|
| :---				| :---        		|
| [status](#status) | 1					|

#### Methods

Methods descriptions of the `wifi` object.

##### status

Exposes wifi radio statistics over ubus.

* [status documentation](./api/wifi.md#status)

### wifi.ap.\<name\>

Object for wifi access point interface functionality. One object per access
point interface will be published.
````bash
'wifi.ap.test5' @86000d35
	"status":{}
	"stats":{}
	"assoclist":{}
	"stations":{"sta":"String"}
	"disconnect":{"sta":"String","reason":"Integer"}
	"monitor_add":{"sta":"String"}
	"monitor_del":{"sta":"String"}
	"monitor_get":{"sta":"String"}
	"subscribe_frame":{"type":"Integer","stype":"Integer"}
	"unsubscribe_frame":{"type":"Integer","stype":"Integer"}
	"add_neighbor":{"bssid":"String","channel":"Integer","bssid_info":"String","reg":"Integer","phy":"Integer"}
	"del_neighbor":{"bssid":"String"}
	"list_neighbor":{"ssid":"String","client":"String"}
	"request_neighbor":{"ssid":"String","client":"String"}
	"request_transition":{"client":"String","bssid":"Array","timeout":"Integer"}
	"add_vendor_ie":{"mgmt":"Integer","oui":"String","data":"String"}
	"del_vendor_ie":{"mgmt":"Integer","oui":"String","data":"String"}
	"dump_beacon":{}
````

| Method      								|Function ID	|
| :--- 	  									| :---        	|
| [status](#status-1)						| 2				|
| [stats](#stats)							| 3				|
| [assoclist](#assoclist)					| 4				|
| [stations](#stations)						| 5				|
| [disconnect](#disconnect)					| 6				|
| [monitor_add](#monitor_add)				| 7				|
| [monitor_del](#monitor_del)				| 8				|
| [monitor_get](#monitor_get)			 	| 9				|
| [subscribe_frame](#subscribe_frame)		| 10			|
| [unsubscribe_frame](#unsubscribe_frame)	| 11			|
| [add_neighbor](#add_neighbor)		 		| 12			|
| [del_neighbor](#del_neighbor)		 		| 13			|
| [list_neighbor](#list_neighbor)			| 14			|
| [request_neighbor](#request_neighbor)		| 15			|
| [request_transition](#request_transition)	| 16			|
| [add_vendor_ie](#add_vendor_ie)			| 17			|
| [del_vendor_ie](#del_vendor_ie)			| 18			|
| [dump_beacon](#dump_beacon)				| 19			|

#### Methods

Method descriptions of the `wifi.ap.<name>` object.

##### status

Exposes access point interface statistics over ubus.

* [status documentation](./api/wifi.ap.md#status)

##### stats

Exposes access point interface rx/tx statistics over ubus.

* [stats documentation](./api/wifi.ap.md#stats)

##### assoclist

Exposes connected wireless clients over ubus.

* [assoclist documentation](./api/wifi.ap.md#assoclist)

##### stations

Exposes client wifi client statistics over ubus.

* [stations documentation](./api/wifi.ap.md#stations)

##### disconnect

Issue a deauthenticate to a wireles client.

* [disconnect documentation](./api/wifi.ap.md#disconnect)

##### monitor_add

Monitor signal strength of unassociated station.

* [monitor documentation](./api/wifi.ap.md#monitor_add)

##### monitor_del

Stop monitoring signal strength of unassociated station.

* [monitor documentation](./api/wifi.ap.md#monitor_del)

##### monitor_get

Monitor signal strength of unassociated station.

* [monitor documentation](./api/wifi.ap.md#monitor_get)

##### subscribe_frame

Subscribe to 802.11 frames, which the driver/firmware needs to pass up to ubus.

* [subscribe_frame documentation](./api/wifi.ap.md#subscribe_frame)


##### unsubscribe_frame

Unsubscribe to 802.11 frames.

* [unsubscribe_frame documentation](./api/wifi.ap.md#unsubscribe_frame)


##### add_neighbor

Add an access point to the neighbor list.

* [add_neighbor documentation](./api/wifi.ap.md#add_neighbor)

##### del_neighbor

Delete an access point from the neighbor list

* [del_neighbor documentation](./api/wifi.ap.md#del_neighbor)

##### list_neighbor

Prints neighbor access points in the network.

* [list_neighbor documentation](./api/wifi.ap.md#list_neighbor)

##### request_neighbor

Triggers an 11k beacon report from an associated station.

* [request_neighbor documentation](./api/wifi.ap.md#request_neighbor)

##### request_transition

Triggers an 11k beacon report from an associated station.

* [request_transition documentation](./api/wifi.ap.md#request_transition)

##### add_vendor_ie

Appends vendor specific information element to an 802.11 management frame.

* [add_vendor_ie documentation](./api/wifi.ap.md#add_vendor_ie)

##### del_vendor_ie

Remove vendor specific information element from an 802.11 management frame.

* [del_vendor_ie documentation](./api/wifi.ap.md#del_vendor_ie)

##### dump_beacon

Dump beacon frame contents.

* [dump_beacon documentation](./api/wifi.ap.md#dump_beacon)

### wifi.radio.\<name\>

Object for wifi device functionality. One object per device will be published to
ubus.

````bash
'wifi.radio.test5' @fac30612
	"status":{}
	"stats":{}
	"get":{"param":"String"}
	"scan":{"ssid":"String","bssid":"String","channel":"Integer"}
	"scanresults":{"bssid":"String"}
	"autochannel":{"interval":"Integer","algo":"Integer","scans":"Integer"}
	"add_iface":{"args":"Table","config":"Boolean"}
	"del_iface":{"ifname":"String","config":"Boolean"}
````
| Method     					| Function ID	|
| :---							| :---        	|
| [status](#status-2)			| 20			|
| [stats](#stats-1)				| 21			|
| [get](#get)					| 22			|
| [scan](#scan)					| 23			|
| [scanresults](#scanresults)	| 24			|
| [autochannel](#autochannel)	| 25			|
| [add_iface](#add_iface)		| 26			|
| [del_iface](#del_iface)		| 27			|

#### Methods

Method descriptions of the `wifi.radio.<name>` object.

##### status

Exposes radio statistics over ubus.

* [status documentation](./api/wifi.radio.md#status)

##### stats

Exposes radio stats over ubus.

* [stats documentation](./api/wifi.radio.md#stats)

##### get

Exposes temperature of the WiFi card.

* [get documentation](./api/wifi.radio.md#get)

##### scan

Scans the radio for broadcasted SSIDs.

* [scan documentation](./api/wifi.radio.md#scan)

##### scanresults

Exposes available SSIDs from a radio scan over ubus.

* [scanresults documentation](./api/wifi.radio.md#scanresults)

##### autochannel

Selects a channel for the radio to operate at.

* [autochannel documentation](./api/wifi.radio.md#autochannel)

### wifi.wps

Object for WiFi Protected Setup functionality.

````bash
'wifi.wps' @76eb99e4
	"start":{"vif":"String","mode":"String","role":"String","pin":"String"}
	"stop":{}
	"status":{"vif":"String"}
	"generate_pin":{}
	"validate_pin":{"pin":"String"}
	"showpin":{"vif":"String"}
	"setpin":{"vif":"String","pin":"String"}
````
| Method     						| Function ID	|
| :--- 								| :---       	|
| [start](#start)					| 28			|
| [stop](#stop)						| 29			|
| [status](#status-3)				| 30			|
| [generate_pin](#generate_pin)		| 31			|
| [validate_pin](#show_pin)			| 32			|
| [showpin](#validate_pin)			| 33			|
| [setpin](#setpin)					| 34			|

#### Methods

Descriptions of `wifi.wps` methods.


##### start

Initiates WPS functionality, default uses push button configuration (PBC).

* [start documentation](./api/wifi.wps.md#start)

##### stop

Stops any WPS functionality.

* [stop documentation](./api/wifi.wps.md#stop)

##### status

Gives current state of the WPS.

* [status documentation](./api/wifi.wps.md#status)

##### generate_pin

Method generating a valid pin for WPS.

* [generate_pin documentation](./api/wifi.wps.md#generate_pin)

##### show_pin

Shows currently set PIN, if any.

* [showpin documentation](./api/wifi.wps.md#showpin)


##### validate_pin

Takes a pin input and shows whether it is a valid PIN or not.

* [validate_pin documentation](./api/wifi.wps.md#validate_pin)

##### setpin

Takes a pin input and sets it as the active WPS pin for the device.

* [setpin documentation](./api/wifi.wps.md#setpin)
