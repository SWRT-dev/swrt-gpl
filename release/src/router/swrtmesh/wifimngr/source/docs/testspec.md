# Test Specification

Most of the functionality in wifimngr can be tested via its ubus API. Each
API can be broken down into an individual test case to show full coverage is
achieved.

# Sections
* [Preqreuisites](#prerequisites)
* [Test Suites](#test-suites)
* [Functional API Tests](#functional-api-tests)
* [Unit Tests](#unit-tests)
* [Functional Tests](#functional-tests)
* [Writing New Tests](#writing-new-tests)


## Prerequisites

The only prerequisite for the wifimngr test suites is that libwifi has to be
built for the TEST platform, a versionprepared to publish dummy data for getter
API, and record setter API to a test logfile at `/tmp/test.log`.

## Test Suites

The wifimngr build pipe has three test suites, a functional-api suite, a unit
testi suite and functional test suite.

### Functional API Tests

The functional API tests consists of four individual test suites, one per object
Ubus under test. The functional API tests use the Ubus-API-validation
command-line interface tool to invoke a method, programmatically through
libubus, and validates it against the objects json-schema.

#### wifi

| Execution ID	| Method      	| Description 	| Function ID Coverage			|
| :---			| :---	   		| :---			| :---							|
| 1				| status     	| No argument  	| [1](./functionspec.md#status) |
| 2				| dummy     	| Negative Test	| 								|

#### wifi.ap.\<name\>

| Execution ID	| Method      	| Description 			| Function ID Coverage					|
| :---			| :--- 	  		| :---					| :---									|
| 1				| status     	| No argument	  		| [2](./functionspec.md#status-1)		|
| 2				| stats     	| No argument  			| [3](./functionspec.md#stats)			|
| 3				| assoclist  	| No argument	  		| [4](./functionspec.md#assoclist)		|
| 4				| stations   	| No argument  			| [5](./functionspec.md#stations)		|
| 5				| stations   	| With sta argument 	| [5](./functionspec.md#stations)		|
| 6				| list_neighbor	| No argument	  		| [14](./functionspec.md#list_neighbor)	|
| 7				| list_neighbor	| With client argument	| [14](./functionspec.md#list_neighbor)	|
| 8				| dump_beacon	| No argument	  		| [19](./functionspec.md#dump_beacon)	|
| 9				| monitor_get	| No argument	  		| [9](./functionspec.md#monitor_get)	|
| 10			| monitor_get	| With client argument	| [9](./functionspec.md#monitor_get)	|
| 11			| dummy     	| Negative Test			| 										|

#### wifi.radio.\<name\>

| Execution ID	| Method     	| Description 			| Function ID Coverage					|
| :---			| :---			| :---					| :---									|
| 1				| status    	| No argument	  		| [20](./functionspec.md#status-2)		|
| 2				| stats     	| No argument  			| [21](./functionspec.md#stats-1)		|
| 3				| scanresults	| No argument	  		| [24](./functionspec.md#scanresults)	|
| 4				| scanresults 	| With bssid argument	| [24](./functionspec.md#scanresults)	|
| 5				| get   		| No argument	  		| [22](./functionspec.md#get)			|
| 6				| autochannel	| No argument	  		| [25](./functionspec.md#autochannel)	|
| 7				| dummy			| Negative Test			| 										|

#### wifi.wps

| Execution ID	| Method     	| Description 					| Function ID Coverage					|
| :---			| :--- 			| :---							| :---									|
| 1				| status    	| No argument	  				| [30](./functionspec.md#status-3)		|
| 2				| status     	| With vif argument				| [30](./functionspec.md#status-3)		|
| 3				| generate_pin	| No argument	 				| [31](./functionspec.md#generate_pin)	|
| 4				| validate_pin	| With invalid pin argument		| [32](./functionspec.md#validate_pin)	|
| 5				| validate_pin	| With valid pin argument		| [32](./functionspec.md#validate_pin)	|
| 6				| showpin		| No argument	 				| [33](./functionspec.md#showpin)		|
| 7				| showpin		| With vif argument				| [33](./functionspec.md#showpin)		|
| 8				| dummy			| Negative Test					| 	 									|

### Unit Tests

The wifimngr unit tests are written in cmocka, invoking the ubus callbacks
directly from the source code, which is compiled into a shared library.
This means mocking the arguments of a cli or libubus invoke in a
`struct blob_attr *`. The results of the call will be logged to the logfile at
`/tmp/test.log`.

| Execution ID	| Method     			| Test Case Name												| Function ID Coverage							|
| :---			| :--- 					| :---															| :---											|
| 1				| disconnect    		| [test_api_radio_disconnect](#test_api_radio_disconnect)		| [6](./functionspec.md#disconnect)				|
| 2				| request_transition	| [test_api_req_bss_transition](#test_api_req_bss_transition)	| [16](./functionspec.md#request_transition)	|
| 3				| request_neighbor		| [test_api_req_beacon_report](#test_api_req_beacon_report) 	| [15](./functionspec.md#request_neighbor)		|
| 4				| add_neighbor 			| [test_api_add_nbr](#test_api_add_nbr)							| [12](./functionspec.md#add_neighbor)			|
| 5				| start					| [test_api_wps_start](#test_api_wps_start) 		   			| [28](./functionspec.md#start)					|
| 6				| stop					| [test_api_wps_stop](#test_api_wps_stop) 		   				| [29](./functionspec.md#stop)					|
| 7				| add_vendor_ie			| [test_api_add_vendor_ie](#test_api_add_vendor_ie) 			| [17](./functionspec.md#add_vendor_ie)			|
| 8				| del_vendor_ie 		| [test_api_del_vendor_ie](#test_api_del_vendor_ie) 			| [18](./functionspec.md#del_vendor_ie)			|
| 9				| del_neighbor 			| [test_api_del_neighbor](#test_api_del_neighbor)				| [13](./functionspec.md#del_neighbor)			|
| 11			| scan					| [test_api_scan](#test_api_scan) 		   						| [23](./functionspec.md#scan)					|
| 12			| setpin				| [test_set_wps_ap_pin](#test_set_wps_ap_pin) 				   	| [34](./functionspec.md#setpin)				|
| 13			| autochannel			| [test_api_acs](#test_api_acs) 		   						| [25](./functionspec.md#autochannel)			|
| 10			| subscribe_frame		| [test_subscribe_frame](#test_subscribe_frame) 				| [7](./functionspec.md#test_subscribe_frame)	|
| 10			| unsubscribe_frame		| [test_unsubscribe_frame](#test_unsubscribe_frame) 			| [8](./functionspec.md#test_unsubscribe_frame)	|
| 10			| add_iface  			| [test_add_iface](#test_add_iface) 							| [7](./functionspec.md#test_add_iface)			|
| 10			| del_iface  			| [test_del_iface](#test_del_iface) 							| [8](./functionspec.md#test_del_iface)			|
| 10			| monitor_add  			| [test_add_monitor](#test_add_monitor) 						| [7](./functionspec.md#monitor_add)			|
| 10			| monitor_del  			| [test_del_monitor](#test_del_monitor) 						| [8](./functionspec.md#monitor_del)			|

#### test_api_radio_disconnect

##### Description

Tests the wifimngr ubus API callback `sta_disconnect(5)`, publishing the method
[disconnect](./functionspec.md#disconnect).

##### Test Steps

Prepare the arguments as:
````bash
{ "sta": "50:31:32:33:34:35" }
````
Iussing a deauthenticate to a 5GHz client from the libwifi test platform.

Read the logfile and verify that the `sta` argument, and interface, was
accurately logged.

##### Test Expected Results

The expected result is for the log file to have recorded a call to the
disconnect function, through the interface `test5` and with the argument `sta`
as `50:31:32:33:34:35`.

````bash
[2020-02-04 13:06:12] disconnect: { "ifname": "test5", "sta": "50:31:32:33:34:35" }
````

#### test_api_req_bss_transition

##### Description

Tests the wifimngr ubus API callback `nbr_transition(5)`, publishing the method
[request_transition](./functionspec.md#request_transition).

##### Test Steps

Prepare the arguments as:

````bash
{ "client": "50:31:32:33:34:35", "bssid": ["22:22:22:22:22:22", "11:11:11:11:11:11" ] }
````

Requesting a 5GHz client, `50:31:32:33:34:35`, to transition from bssid
`22:22:22:22:22:22` to bssid `11:11:11:11:11:11`.

Read the logfile and verify that the interface and arguments were accurately
logged.

##### Test Expected Results

The expected result is for the log file to have recorded a call to the
req_bss_transition function, through the interface `test5` and with the argument `client`
as `50:31:32:33:34:35` and an array `bssid` with the entries
`"22:22:22:22:22:22","11:11:11:11:11:11"`.

````bash
[2020-02-04 13:06:12] req_bss_transition: { "ifname": "test5", "client": "50:31:32:33:34:35", "bssid": ["22:22:22:22:22:22","11:11:11:11:11:11",] }
````


#### test_api_req_beacon_report

##### Description

Tests the wifimngr ubus API callback `nbr_request(5)`, publishing the method
[request_neighbor](./functionspec.md#request_neighbor).

##### Test Steps

Prepare the arguments as:

````bash
{ "client": "50:31:32:33:34:35", "ssid": "Test SSID 5GHz" }
````

Requesting the client `50:31:32:33:34:35` with the ssid `Test SSID 5GHz` as a
neighbor.

Read the logfile and verify that the interface and arguments were accurately
logged.

##### Test Expected Results

The expected result is for the log file to have recorded a call to the
`nbr_request` function, through the interface `test5` and with the argument
`client` as `50:31:32:33:34:35`. Currently the `ssid` argument is ignored and
not expected to be observed from the log file.

````bash
[2020-02-04 13:06:12] req_beacon_report: { "ifname": "test5", "client": "50:31:32:33:34:35" }
````

#### test_api_add_nbr

##### Description

Tests the wifimngr ubus API callback `nbr_add(5)`, publishing the method
[add_neighbor](./functionspec.md#add_neighbor).

##### Test Steps

Prepare the arguments as:

````bash
{ "bssid": "00:11:12:13:14:15", "channel": 36, "bssid_info", "5", "reg": 5, "phy": 5 }
````

Requesting the neighbor bssid `00:11:12:13:14:15`, operating at channel 36 on
registry 5 and phy 5 to be added to the neighbor list.

Read the logfile and verify that the interface and arguments were accurately
logged.

##### Test Expected Results

The expected result is for the log file to have recorded a call to the
`add_neighbor` function, through the interface `test5` and with the arguments
`bssid` as `00:11:12:13:14:15`, channel as 36, phy as 5, reg as 5, bssid_info as
"5".

````bash
[2020-02-04 13:06:12] add_neighbor: { "ifname": "test5", "bssid_info": 5, "reg": 5, "channel": 36, "phy": 5, "bssid": "00:11:12:13:14:15" }
````

#### test_api_wps_start

##### Description

Tests the wifimngr ubus API callback `wps_start(5)`, publishing the method
[start](./functionspec.md#start) to the `wifi.wps` object.

##### Test Steps

The test case has three steps, trying different argument inputs to reach as high
coverage as possible.

The first part of the test case tests the role registrar, with a pin, allowing
others devices to connect via pin, over the 5GHz interface.

````bash
{ "vif": "test5", "pin": "24033848", "mode", "pin", "role": "registrar" }
````

The second part tests signing up with the role enrollee over the 2.4GHz
interface.

````bash
{ "vif": "test2", "role": "enrollee" }`
````

The third part of the test case tests the mode pbc, allowing other devices to
synchronize over via its own push button configuration, over the 2.4GHz
interface.

````bash
{ "vif": "test2", "mode": "pbc" }`
````

Read the logfile and verify that the interface and arguments were accurately
logged.

##### Test Expected Results

The expected result is for the log file to have recorded a call to the
`start_wps` function, showing the arguments accordingly, note that role
`registrar` is represented by 0, and `enrollee` by 1, whereas mode `pin` is
represented by 1 and `pbc` by 0.


````bash
[2020-02-04 13:06:12] start_wps: { "ifname": "test5", "pin": "24033848", "role": 4, "mode": 1 }
[2020-02-04 13:06:12] start_wps: { "ifname": "test2", "role": 1, "mode": 0 }
[2020-02-04 13:06:12] start_wps: { "ifname": "test2", "role": 0, "mode": 0 }
````

#### test_api_wps_stop

##### Description

Tests the wifimngr ubus API callback `wps_stop(5)`, publishing the method
[stop](./functionspec.md#stop) to the `wifi.wps` object.

##### Test Steps

Prepare the arguments as:

````bash
{ "ifname": "test5" }
````

Stopping any active WPS activity on the 5GHz interface.

Read the logfile and verify that the interface and arguments were accurately
logged.

##### Test Expected Results

As `wps_stop(5)` will call iterate all available interfaces and invoke the
libwifi API `wifi_stop_wps(1)` for each and every one, we expect to see all
(both) interfaces in the logs along with the function tag `stop_wps`.

````bash
[2020-02-04 13:06:12] stop_wps: { "ifname": "test2" }
[2020-02-04 13:06:12] stop_wps: { "ifname": "test5" }
````

#### test_api_add_vendor_ie

##### Description

Tests the wifimngr ubus API callback `vsie_add(5)`, publishing the method
[add_vendor_ie](./functionspec.md#add_vendor_ie) to the `wifi.ap.<name>` object.

##### Test Steps

Prepare the arguments:

````bash
{ "mgmt": 5, "oui": "112233", "data": "dd03efa567" }
````

Appending the vendor specific information to the management frame.

##### Test Expected Results

The expected result is to observe the function tag `add_vendor_ie` in the logs,
along with the prepared arguments, verifying that they have been parsed
correctly by wifimngr and libwifi.

````bash
[2020-02-04 13:06:12] add_vendor_ie: { "ifname": "test5", "mgmt": 5, "oui": "112233", "data": "dd03efa567" }
````

#### test_api_del_vendor_ie

##### Description

Tests the wifimngr ubus API callback `vsie_del(5)`, publishing the method
[del_vendor_ie](./functionspec.md#del_vendor_ie) to the `wifi.ap.<name>` object.

##### Test Steps

Prepare the arguments:
````bash
{ "mgmt": 5, "oui": "112233", "data": "dd03efa567" }
````

Deleting the vendor specific information from the management frame.

##### Test Expected Results
The expected result is to observe the function tag `del_vendor_ie` in the logs,
along with the prepared arguments, verifying that they have been parsed
correctly by wifimngr and libwifi.

````bash
[2020-02-04 13:06:12] del_vendor_ie: { "ifname": "test5", "mgmt": 5, "oui": "112233", "data": "dd03efa567" }
````

#### test_api_del_neighbor

##### Description

Tests the wifimngr ubus API callback `nbr_del(5)`, publishing the method
[del_neighbor](./functionspec.md#del_neighbor) to the `wifi.ap.<name>` object.

##### Test Steps

Prepare the arguments:

````bash
{ "bssid": "50:10:00:11:22:33" }
````

Removing the bssid from the neighbor list.

##### Test Expected Results

The expected result is to observe the function tag `del_neighbor` in the test
logs, along with the arguments, verifying that they have been parsed correctly
by wifimngr and libwifi.

````bash
[2020-02-04 13:06:12] del_neighbor: { "ifname": "test5", "bssid": "50:10:00:11:22:33" }
````

#### test_api_scan

##### Description

Tests the wifimngr ubus API callback `wl_scan(5)`, publishing the method
[scan](./functionspec.md#scan) to the `wifi.radio.<name>` object.

##### Test Steps

Prepare the arguments:

````bash
{ "ssid": "test", "bssid": "00:00:00:11:22:33", "channel": 36 }
````

Scanning the device using the ssid `test`.

##### Test Expected Results

The expected result is to observe the function tag `scan` in the test logs,
along with the 5GHz interface and the argument `test5` as
`ssid`, the remaining arguments are ignored as they are currently not
implemented or passed to libwifi.

````bash
[2020-02-04 13:06:12] scan: { "ifname": "test5", "ssid": "test", "bssid": "00:00:00:00:00:00", "channel": 0, "type": 0 }
````

#### test_set_wps_ap_pin

##### Description

Tests the wifimngr ubus API callback `wps_set_ap_pin(5)`, publishing the method
[setpin](./functionspec.md#setpin) to the `wifi.wps` object.

##### Test Steps

Prepare the arguments:

````bash
{ "ifname": "test5", "pin": "24033848" }
````

Setting the WPS pin of the 5GHz interface to 24033848.

##### Test Expected Results

The expected result is to observe the function tag `set_wps_ap_pin` in the test
logs, along with the 5GHz interface and the pin.

````bash
[2020-02-04 13:06:12] set_wps_ap_pin: { "ifname": "test5", "pin": 24033848 }
````

#### test_api_acs

##### Description

Tests the wifimngr ubus API callback `wl_autochannel(5)`, publishing the method
[autochannel](./functionspec.md#autochannel) to the `wifi.radio.<name>` object.

##### Test Steps

Prepare the arguments:

````bash
{ "ifname": "test5" }
````

Automatically selecting a channel to operate at. Currently no further arguments
are passed as they are ignored programmatically.

##### Test Expected Results

The expected result is to observe the function tag `acs` in the test
logs, along with the 5GHz interface.

````bash
[2020-02-04 13:06:12] acs: { "ifname": "test5" }
````

#### test_subscribe_frame

##### Description

Tests the wifimngr ubus API callback `subscribe_frame(5)`, publishing the method
[subscribe_frame](./functionspec.md#subscribe_frame) to the `wifi.ap.<name>`
object. Successfully calling the method will result in listening to the given
802.11 frames.

##### Test Steps

Prepare the arguments:

````bash
{ "type": 0, "stype": 8 }
````

Start subscribing to 802.11 frames of `stype` set to `WIFI_FRAME_BEACON` and
of `type` `WIFI_FRAME_MGMT`.

##### Test Expected Results

The expected result is to observe the function tag `subscribe_frame` in the test
logs, along with the 5GHz interface and the provided `type` and `stype`
arguments.

````bash
[2020-09-16 09:47:31] iface_subscribe_frame: { "ifname": "test5", "type": 0, "stype": 8 }
````

#### test_unsubscribe_frame

##### Description

Tests the wifimngr ubus API callback `unsubscribe_frame(5)`, publishing the
method [unsubscribe_frame](./functionspec.md#unsubscribe_frame) to the
`wifi.ap.<name>` object. Successfully calling the method will result in
no longer listening to the specified 802.11 frames.

##### Test Steps

Prepare the arguments:

````bash
{ "type": 0, "stype": 8 }
````

Stop subscribing to 802.11 frames of `stype` set to `WIFI_FRAME_PROBE_REQ` and
of `type` `WIFI_FRAME_CTRL`.

##### Test Expected Results

The expected result is to observe the function tag `unsubscribe_frame` in the
test logs, along with the 5GHz interface and the provided `type` and `stype`
arguments.

````bash
[2020-09-16 09:47:31] iface_unsubscribe_frame: { "ifname": "test5", "type": 1, "stype": 4 }
````

#### test_add_iface

##### Description

Tests the wifimngr ubus API callback `wl_add_iface(5)`, publishing the
method [add_iface](./functionspec.md#add_iface) to the `wifi.radio.<name>`
object. Successfully calling the method will generate another wireless
interface, and, if the `config` argument is set to true, a uci section
will be generated for the interface in `/etc/config/wireless`.

##### Test Steps

Prepare the arguments:

````bash
{ "config": 1, "args": {"mode": "monitor", "name": "test" }}
````

Creating a uci config section and creating an interface in monitor mode,
`WIFI_MODE_STA` with the name `test`.

##### Test Expected Results

The expected result is to observe the function tag `add_iface` in the
test logs, along with the 5GHz interface, with `wifi_mode` set to `4`, an `argv`
array with device, name and mode. Additional, an uci config section
attached to the test5 device for the interface `test`.

````bash
[2020-09-16 10:22:37] add_iface: { "ifname": "test5", "wifi_mode": "4", "argv": ["device", "test5", "mode", "monitor", "name", "test"]  }
````


#### test_del_iface

##### Description

Tests the wifimngr ubus API callback `wl_del_iface(5)`, publishing the
method [del_iface](./functionspec.md#del_iface) to the `wifi.radio.<name>`
object. Successfully calling the method will remove the wireless
interface, and, if the `config` argument is set to true, the corresponding
uci interface section in `/etc/config/wireless` will be deleted.

##### Test Steps

Prepare the arguments:

````bash
{ "config": 1, "iface": "test" }
````

Deleting the uci config section and the interface `test`.

##### Test Expected Results

The expected result is to observe the function tag `del_iface` in the
test logs, along with the 5GHz interface, with the `face`
set to `test`, `config` set to `1`, and the uci section for the `test`
interface to be removed.

````bash
[2020-09-16 10:22:37] del_iface: { "ifname": "test5", "iface": "test" }
````

#### test_monitor_add

##### Description

Tests the wifimngr ubus API callback `sta_monitor_add(5)`, publishing the method
[monitor_add](./functionspec.md#monitor_add) to the `wifi.ap.<name>` object.
Successfully calling the method will add an unassociated client to be monitored
for its rssi.

##### Test Steps

Prepare the arguments:

````bash
{ "sta": "11:22:33:44:55:66" }
````

Starting to monitor the unassociated wireless client with the mac address
`11:22:33:44:55:66`.

##### Test Expected Results

The expected result is to observe the function tag `monitor_sta` in the test
logs, along with the 5GHz interface and the argument `sta` as
`11:22:33:44:55:66`, with `enable` set to `1`.

````bash
[2020-09-16 11:20:12] monitor_sta: { "ifname": "test5", "sta": "11:22:33:44:55:66", "enable": 1 }
````

#### test_monitor_del

##### Description

Tests the wifimngr ubus API callback `sta_monitor_del(5)`, publishing the method
[monitor_del](./functionspec.md#monitor_del) to the `wifi.ap.<name>` object.
Successfully calling the method will stop monitoring the specified unassociated
client for its rssi.

##### Test Steps

Prepare the arguments:

````bash
{ "sta": "11:22:33:44:55:66" }
````

Stop monitoring the unassociated wireless client with the mac address
`11:22:33:44:55:66`.

##### Test Expected Results

The expected result is to observe the function tag `monitor_sta` in the test
logs, along with the 5GHz interface and the argument `sta` as
`11:22:33:44:55:66`, with `enable` set to `0`.

````bash
[2020-09-16 11:20:12] monitor_sta: { "ifname": "test5", "sta": "11:22:33:44:55:66", "enable": 0 }
````

### Functional Tests

| Serial ID		| Test Case Name	|
| :---			| :---				|
| 1				| test_api_events	|

#### test_api_events

##### Description

Tests the event registration and receive process. Wifimngr will read the file
`/etc/wifi.json` in order to register to netlink events, in this case the
netlink family `easysoc` and the group `notify`.


```
{
	"events" : [
		{
			"type": "wifi-event",
			"name": "cfg09ac03",
			"ifname": "test5",
			"family": "easysoc",
			"group": [
					"notify"
			]
		}
	]
}
```

##### Test Steps

As netlink events are not being published in the docker environment, the test
will verify that a listener thread is created, registrates a listener and calls
the libwifi API for receiving events.

As in practice the listener thread is intended to listen for events for the
entire lifetime of the process, there is no break condition for the listener
loop. However, in order to get a more accurate memory management report, the
event loop, `int wifimngr_recv_event(2)` is mocked to allow break on the return
code -1.

```
int wifimngr_recv_event(const char *ifname, void *handle)
{
	int err;

	for (;;) {
		err = wifi_recv_event((char *)ifname, handle);
		if (err < 0)
			return -1;
	}

	return 0;
}
```

##### Test Expected Results

The expected results are that wifimngr will have attempted to register a listner
with the family `easysoc`, the group `notify` under the interface `test5`, and
attempted to receive events under the same interface, read from the test log
file under the function tags `register_event` and `recv_event.`

````bash
[2020-02-05 09:38:40] register_event: { "ifname": "test5", "family": "easysoc", "group": "notify" }
[2020-02-05 09:38:40] recv_event: { "ifname": "test5" }
````

## Writing New Tests

On the addition of new APIs to wifimngr and libwifi, additional tests should be
written to maintain the coverage and quality of the code.

### Libwifi

On a new addition to the libwifi API, the test platform for libwifi has to be
extended. The test platform for libwifi consists of two files, `test.c` and
`test.h`, found under the subdirectory `./libwifi/modules/` under the
[easy-soc-libs](https://dev.iopsys.eu/iopsys/easy-soc-libs) repository.

The following examples assume the existence of a `struct test`.

```
struct test {
	unsigned char sta[6];
}
```

#### Getter

Assume the addition of the new API, `get_client(2)`, taking an interface,
`char *` as first argument, and a `struct test *`, filling the
struct with the mac address of the first client in the interface.

The first step is to prepare some dummy data in the header file, in this case
a dummy client of the type `struct test`. Two structs with static mac addresses
are to be prepared, one for each test interface, holding static data to
represent a mac address. Additionally, add a macro pointing to the address of
the structs with the dummy data.

```
/* 5GHz dummy data */
const struct test test5_test_data = {
	.sta = {"\x66\x55\x44\x33\x22\x11"}
};

/* 5GHz "pointer" to dummy data */
#define test5_test	&test5_test_data

/* 2.4GHz dummy data */
const struct test test2_test_data = {
	.sta = {"\x11\x22\x33\x44\x55\x66"}
};

/* 2.4GHz "pointer" to dummy data */
#define test2_test	&test2_test_data
```

With the dummy data prepared, the function can be implemented. The test platform
has a set of macros prepared to use when filling variables with dummy data,
ranging from primitive types, buffers, arrays and structs. For this example
the helper macro `GET_TEST_BUF_TYPE(4)` will be used:

```
#define TESTDATA(band, attr)	test ## band ## _ ## attr

...

#define GET_TEST_BUF_TYPE(o, iface, attr, type)				\
({									\
	if (ifname_to_band(iface) == 5)					\
		memcpy(o, TESTDATA(5, attr), sizeof(type));		\
	else								\
		memcpy(o, TESTDATA(2, attr), sizeof(type));		\
})
```

The macro will fill the first argument, `o`, wth the data pointed to by the
concatenated variable `test`, `5`, `_` and the `attr` argument, forming
`test5_test`.

Therefore, implementing our dummy getter, `test_get_client(2)`, becomes simple.

```
int get_client(char *ifname, struct test *t)
{
	GET_TEST_BUF_TYPE(t, name, test, struct test);

	return 0;
}
```

Lastly, it should be added to the driver, assuming the driver has been updated
with a `get_client` function pointer:

```
const struct wifi_driver test_driver = {
	.name = "test",
	.get_client = test_get_client,
	...
};
```

#### Setter

In the case of an addition of a new setter to the libwifi API. A log function
is prepared in the test driver, logging a `char *` string to a test log file,
`/tmp/test.log`, `log_test(2)`. The first argument to `log_test(2)` is the
`char *` representing the format, and the second is a `va_list`; all the
arguments of the format. It is important the the format follows the structure

```
<FUNCTION_IDENTIFIER>: { <RELEVANT_ARGS> }
```

Assume the addition of a `set_client(2)` API call, the function should be
implemented in the test driver, calling `log_test(2)` to be able to verify the
struct usage and library linkage. Let's assume that the exposed Ubus API
takes an argument in the form of a mac address, that would be provded as an
argument, `sta`.

```
int test_set_client(char *ifname, struct test *t)
{
	log_test("set_client: { \"ifname\": \"%s\", \"sta\": \"%02x%02x%02x%02x%02x%02x\" }",
			ifname, t->sta[0], t->sta[0], t->sta[0],
			t->sta[0], t->sta[0], t->sta[0]);

	return 0;
}
```

This will log the entry as:

```
[2020-02-04 13:06:12] set_client: { "ifname": "test5", "sta": "00:11:22:33:44:55" }
```

Which can then be read by the corresponding test case and verified.

### Writing Tests

When extending the test cases themselves, getters should be tested via the
Ubus-API-validation tool for in the functional-api-test suite.

Setters without any external dependencies (i.e. other processes) should be
included in the unit tests (`./test/api_test_wifi_setters.c`), and remaining
setters in the functional-test suite (`./test/function_test_wifi_event.c`).

For the setter tests, a utility function has been prepared to parse the logfile
for the latest occurrance of a function identifier tag, `poll_test_log(2)`,
utility function have been prepared. The function takes a `FILE *` as first
argument, found in the `struct test_env`, and the function identifier as
second argument, returning the json payload found in the log file, if any.

For more information on writing test cases see
[Continuous Integration at IOPSYS](https://dev.iopsys.eu/training/iopsyswrt-module-development/blob/master/testing/README.md)
