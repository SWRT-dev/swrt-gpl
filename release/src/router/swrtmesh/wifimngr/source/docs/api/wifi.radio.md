# wifi.radio.&lt;name&gt; Schema

```
https://www.iopsys.eu/wifi.radio.json
```

WiFi Radio Object

| Custom Properties | Additional Properties |
| ----------------- | --------------------- |
| Forbidden         | Forbidden             |

# wifi.radio.&lt;name&gt;

| List of Methods             |
| --------------------------- |
| [add_iface](#add_iface)     | Method | wifi.radio.&lt;name&gt; (this schema) |
| [autochannel](#autochannel) | Method | wifi.radio.&lt;name&gt; (this schema) |
| [del_iface](#del_iface)     | Method | wifi.radio.&lt;name&gt; (this schema) |
| [get](#get)                 | Method | wifi.radio.&lt;name&gt; (this schema) |
| [scan](#scan)               | Method | wifi.radio.&lt;name&gt; (this schema) |
| [scanresults](#scanresults) | Method | wifi.radio.&lt;name&gt; (this schema) |
| [stats](#stats)             | Method | wifi.radio.&lt;name&gt; (this schema) |
| [status](#status)           | Method | wifi.radio.&lt;name&gt; (this schema) |

## add_iface

`add_iface`

- type: `Method`

### add_iface Type

`object` with following properties:

| Property | Type   | Required |
| -------- | ------ | -------- |
| `input`  | object | Optional |
| `output` | object | Optional |

#### input

`input`

- is optional
- type: `object`

##### input Type

`object` with following properties:

| Property | Type    | Required |
| -------- | ------- | -------- |
| `args`   | object  | Optional |
| `config` | boolean | Optional |

#### args

`args`

- is optional
- type: `object`

##### args Type

`object` with following properties:

| Property | Type | Required |
| -------- | ---- | -------- |
| None     | None | None     |

#### config

`config`

- is optional
- type: `boolean`

##### config Type

`boolean`

### Ubus CLI Example

```
ubus call wifi.radio.<name> add_iface {"args":{},"config":true}
```

### JSONRPC Example

```json
{
  "jsonrpc": "2.0",
  "id": 0,
  "method": "call",
  "params": ["<SID>", "wifi.radio.<name>", "add_iface", { "args": {}, "config": true }]
}
```

#### output

`output`

- is optional
- type: `object`

##### output Type

`object` with following properties:

| Property | Type | Required |
| -------- | ---- | -------- |
| None     | None | None     |

### Output Example

```json
{}
```

## autochannel

`autochannel`

- type: `Method`

### autochannel Type

`object` with following properties:

| Property | Type   | Required |
| -------- | ------ | -------- |
| `input`  | object | Optional |
| `output` | object | Optional |

#### input

`input`

- is optional
- type: `object`

##### input Type

`object` with following properties:

| Property   | Type    | Required |
| ---------- | ------- | -------- |
| `algo`     | integer | Optional |
| `interval` | integer | Optional |
| `scans`    | integer | Optional |

#### algo

`algo`

- is optional
- type: `integer`

##### algo Type

`integer`

#### interval

`interval`

- is optional
- type: `integer`

##### interval Type

`integer`

#### scans

`scans`

- is optional
- type: `integer`

##### scans Type

`integer`

### Ubus CLI Example

```
ubus call wifi.radio.<name> autochannel {"interval":42230905,"algo":-63796592,"scans":80502357}
```

### JSONRPC Example

```json
{
  "jsonrpc": "2.0",
  "id": 0,
  "method": "call",
  "params": [
    "<SID>",
    "wifi.radio.<name>",
    "autochannel",
    { "interval": 42230905, "algo": -63796592, "scans": 80502357 }
  ]
}
```

#### output

##### wifi.radio.autochannel

Auto channel selection

`output`

- is optional
- type: `object`

##### output Type

`object` with following properties:

| Property      | Type    | Required     |
| ------------- | ------- | ------------ |
| `code`        | integer | **Required** |
| `new_channel` | oneOf   | **Required** |
| `status`      | string  | **Required** |

#### code

`code`

- is **required**
- type: `integer`

##### code Type

`integer`

#### new_channel

`new_channel`

- is **required**
- type: complex

##### new_channel Type

**One** of the following _conditions_ need to be fulfilled.

#### Condition 1

`integer`

- minimum value: `1`
- maximum value: `14`

#### Condition 2

`integer`

- minimum value: `32`
- maximum value: `200`

#### status

`status`

- is **required**
- type: `string`

##### status Type

`string`

### Output Example

```json
{ "code": 2357742, "new_channel": 132, "status": "est in ad ex culpa" }
```

## del_iface

`del_iface`

- type: `Method`

### del_iface Type

`object` with following properties:

| Property | Type   | Required |
| -------- | ------ | -------- |
| `input`  | object | Optional |
| `output` | object | Optional |

#### input

`input`

- is optional
- type: `object`

##### input Type

`object` with following properties:

| Property | Type    | Required     |
| -------- | ------- | ------------ |
| `config` | boolean | Optional     |
| `ifname` | string  | **Required** |

#### config

`config`

- is optional
- type: `boolean`

##### config Type

`boolean`

#### ifname

`ifname`

- is **required**
- type: `string`

##### ifname Type

`string`

- maximum length: 16 characters

### Ubus CLI Example

```
ubus call wifi.radio.<name> del_iface {"ifname":"adipi","config":true}
```

### JSONRPC Example

```json
{
  "jsonrpc": "2.0",
  "id": 0,
  "method": "call",
  "params": ["<SID>", "wifi.radio.<name>", "del_iface", { "ifname": "adipi", "config": true }]
}
```

#### output

`output`

- is optional
- type: `object`

##### output Type

`object` with following properties:

| Property | Type | Required |
| -------- | ---- | -------- |
| None     | None | None     |

### Output Example

```json
{}
```

## get

`get`

- type: `Method`

### get Type

`object` with following properties:

| Property | Type   | Required |
| -------- | ------ | -------- |
| `input`  | object | Optional |
| `output` | object | Optional |

#### input

`input`

- is optional
- type: `object`

##### input Type

`object` with following properties:

| Property | Type   | Required |
| -------- | ------ | -------- |
| `param`  | string | Optional |

#### param

`param`

- is optional
- type: `string`

##### param Type

`string`

### Ubus CLI Example

```
ubus call wifi.radio.<name> get {"param":"esse anim Duis minim laborum"}
```

### JSONRPC Example

```json
{
  "jsonrpc": "2.0",
  "id": 0,
  "method": "call",
  "params": ["<SID>", "wifi.radio.<name>", "get", { "param": "esse anim Duis minim laborum" }]
}
```

#### output

`output`

- is optional
- type: `object`

##### output Type

`object` with following properties:

| Property | Type    | Required |
| -------- | ------- | -------- |
| `status` | integer | Optional |
| `values` | integer | Optional |

#### status

`status`

- is optional
- type: `integer`

##### status Type

`integer`

#### values

`values`

- is optional
- type: `integer`

##### values Type

`integer`

### Output Example

```json
{ "status": 7971921, "values": -22899543 }
```

## scan

`scan`

- type: `Method`

### scan Type

`object` with following properties:

| Property | Type   | Required |
| -------- | ------ | -------- |
| `input`  | object | Optional |
| `output` | object | Optional |

#### input

`input`

- is optional
- type: `object`

##### input Type

`object` with following properties:

| Property  | Type   | Required |
| --------- | ------ | -------- |
| `bssid`   | string | Optional |
| `channel` | oneOf  | Optional |
| `ssid`    | string | Optional |

#### bssid

##### MAC Address

`bssid`

- is optional
- type: reference

##### bssid Type

`string`

- minimum length: 17 characters
- maximum length: 17 characters All instances must conform to this regular expression (test examples
  [here](<https://regexr.com/?expression=%5E(%5B0-9a-fA-F%5D%5B0-9a-fA-F%5D%3A)%7B5%7D%5B0-9a-fA-F%5D%5B0-9a-fA-F%5D%24>)):

```regex
^([0-9a-fA-F][0-9a-fA-F]:){5}[0-9a-fA-F][0-9a-fA-F]$
```

#### channel

`channel`

- is optional
- type: complex

##### channel Type

**One** of the following _conditions_ need to be fulfilled.

#### Condition 1

`integer`

- minimum value: `1`
- maximum value: `14`

#### Condition 2

`integer`

- minimum value: `32`
- maximum value: `200`

#### ssid

##### SSID

`ssid`

- is optional
- type: reference

##### ssid Type

`string`

- maximum length: 32 characters

### Ubus CLI Example

```
ubus call wifi.radio.<name> scan {"ssid":"non","bssid":"Fe:e5:56:E7:78:bb","channel":11}
```

### JSONRPC Example

```json
{
  "jsonrpc": "2.0",
  "id": 0,
  "method": "call",
  "params": ["<SID>", "wifi.radio.<name>", "scan", { "ssid": "non", "bssid": "Fe:e5:56:E7:78:bb", "channel": 11 }]
}
```

#### output

Trigger a scan

`output`

- is optional
- type: `object`

##### output Type

`object` with following properties:

| Property | Type | Required |
| -------- | ---- | -------- |
| None     | None | None     |

### Output Example

```json
{}
```

## scanresults

`scanresults`

- type: `Method`

### scanresults Type

`object` with following properties:

| Property | Type   | Required |
| -------- | ------ | -------- |
| `input`  | object | Optional |
| `output` | oneOf  | Optional |

#### input

`input`

- is optional
- type: `object`

##### input Type

`object` with following properties:

| Property | Type | Required |
| -------- | ---- | -------- |
| None     | None | None     |

### Ubus CLI Example

```
ubus call wifi.radio.<name> scanresults {}
```

### JSONRPC Example

```json
{ "jsonrpc": "2.0", "id": 0, "method": "call", "params": ["<SID>", "wifi.radio.<name>", "scanresults", {}] }
```

#### output

Show scan results

`output`

- is optional
- type: complex

##### output Type

**One** of the following _conditions_ need to be fulfilled.

#### Condition 1

#### Condition 2

`object` with following properties:

| Property           | Type    | Required     |
| ------------------ | ------- | ------------ |
| `band`             | string  | **Required** |
| `bssid`            | string  | **Required** |
| `encryption`       | string  | **Required** |
| `load_available`   | integer | **Required** |
| `load_stas`        | integer | **Required** |
| `load_utilization` | integer | **Required** |
| `rssi`             | integer | **Required** |
| `ssid`             | string  | **Required** |

#### band

##### WiFi Band

`band`

- is **required**
- type: reference

##### band Type

`string`

The value of this property **must** be equal to one of the [known values below](#-known-values).

##### band Known Values

| Value |
| ----- |
| 2GHz  |
| 5GHz  |

#### bssid

##### MAC Address

`bssid`

- is **required**
- type: reference

##### bssid Type

`string`

- minimum length: 17 characters
- maximum length: 17 characters All instances must conform to this regular expression (test examples
  [here](<https://regexr.com/?expression=%5E(%5B0-9a-fA-F%5D%5B0-9a-fA-F%5D%3A)%7B5%7D%5B0-9a-fA-F%5D%5B0-9a-fA-F%5D%24>)):

```regex
^([0-9a-fA-F][0-9a-fA-F]:){5}[0-9a-fA-F][0-9a-fA-F]$
```

#### encryption

`encryption`

- is **required**
- type: `string`

##### encryption Type

`string`

#### load_available

`load_available`

- is **required**
- type: `integer`

##### load_available Type

`integer`

- minimum value: `0`

#### load_stas

`load_stas`

- is **required**
- type: `integer`

##### load_stas Type

`integer`

- minimum value: `0`

#### load_utilization

`load_utilization`

- is **required**
- type: `integer`

##### load_utilization Type

`integer`

- minimum value: `0`
- maximum value: `100`

#### rssi

##### RSSI

`rssi`

- is **required**
- type: reference

##### rssi Type

`integer`

- minimum value: `-128`
- maximum value: `0`

#### ssid

##### SSID

`ssid`

- is **required**
- type: reference

##### ssid Type

`string`

- maximum length: 32 characters

### Output Example

```json
{
  "ssid": "sit mi",
  "bssid": "eF:96:af:66:13:4D",
  "encryption": "labore velit",
  "band": "2GHz",
  "rssi": -6,
  "load_stas": 17584500,
  "load_utilization": 65,
  "load_available": 75311380,
  "standard": 22195874,
  "bandwidth": "et ullamco",
  "channel": 48960927
}
```

## stats

`stats`

- type: `Method`

### stats Type

`object` with following properties:

| Property | Type   | Required |
| -------- | ------ | -------- |
| `input`  | object | Optional |
| `output` | object | Optional |

#### input

`input`

- is optional
- type: `object`

##### input Type

`object` with following properties:

| Property | Type | Required |
| -------- | ---- | -------- |
| None     | None | None     |

### Ubus CLI Example

```
ubus call wifi.radio.<name> stats {}
```

### JSONRPC Example

```json
{ "jsonrpc": "2.0", "id": 0, "method": "call", "params": ["<SID>", "wifi.radio.<name>", "stats", {}] }
```

#### output

##### wifi.radio.iface

Radio statistics

`output`

- is optional
- type: `object`

##### output Type

`object` with following properties:

| Property               | Type    | Required     |
| ---------------------- | ------- | ------------ |
| `rx_bytes`             | integer | **Required** |
| `rx_dropped_packets`   | integer | **Required** |
| `rx_error_packets`     | integer | **Required** |
| `rx_fcs_error_packets` | integer | **Required** |
| `rx_packets`           | integer | **Required** |
| `tx_bytes`             | integer | **Required** |
| `tx_dropped_packets`   | integer | **Required** |
| `tx_error_packets`     | integer | **Required** |
| `tx_packets`           | integer | **Required** |

#### rx_bytes

`rx_bytes`

- is **required**
- type: reference

##### rx_bytes Type

`integer`

- minimum value: `0`

#### rx_dropped_packets

`rx_dropped_packets`

- is **required**
- type: reference

##### rx_dropped_packets Type

`integer`

- minimum value: `0`

#### rx_error_packets

`rx_error_packets`

- is **required**
- type: reference

##### rx_error_packets Type

`integer`

- minimum value: `0`

#### rx_fcs_error_packets

`rx_fcs_error_packets`

- is **required**
- type: reference

##### rx_fcs_error_packets Type

`integer`

- minimum value: `0`

#### rx_packets

`rx_packets`

- is **required**
- type: reference

##### rx_packets Type

`integer`

- minimum value: `0`

#### tx_bytes

`tx_bytes`

- is **required**
- type: reference

##### tx_bytes Type

`integer`

- minimum value: `0`

#### tx_dropped_packets

`tx_dropped_packets`

- is **required**
- type: reference

##### tx_dropped_packets Type

`integer`

- minimum value: `0`

#### tx_error_packets

`tx_error_packets`

- is **required**
- type: reference

##### tx_error_packets Type

`integer`

- minimum value: `0`

#### tx_packets

`tx_packets`

- is **required**
- type: reference

##### tx_packets Type

`integer`

- minimum value: `0`

### Output Example

```json
{
  "tx_bytes": 52256666,
  "tx_packets": 56412375,
  "tx_error_packets": 92922995,
  "tx_dropped_packets": 12851167,
  "rx_bytes": 7731111,
  "rx_packets": 56612816,
  "rx_error_packets": 8626017,
  "rx_dropped_packets": 57740274,
  "rx_fcs_error_packets": 73453141
}
```

## status

`status`

- type: `Method`

### status Type

`object` with following properties:

| Property | Type   | Required |
| -------- | ------ | -------- |
| `input`  | object | Optional |
| `output` | object | Optional |

#### input

`input`

- is optional
- type: `object`

##### input Type

`object` with following properties:

| Property | Type | Required |
| -------- | ---- | -------- |
| None     | None | None     |

### Ubus CLI Example

```
ubus call wifi.radio.<name> status {}
```

### JSONRPC Example

```json
{ "jsonrpc": "2.0", "id": 0, "method": "call", "params": ["<SID>", "wifi.radio.<name>", "status", {}] }
```

#### output

##### wifi.radio.iface

Radio status

`output`

- is optional
- type: `object`

##### output Type

`object` with following properties:

| Property            | Type    | Required     | Default |
| ------------------- | ------- | ------------ | ------- |
| `band`              | string  | **Required** |         |
| `beacon_int`        | integer | **Required** | `100`   |
| `dtim_period`       | integer | **Required** | `1`     |
| `frag_threshold`    | integer | **Required** | `2346`  |
| `isup`              | boolean | **Required** |         |
| `long_retry_limit`  | integer | **Required** |         |
| `maxrate`           | integer | **Required** |         |
| `noise`             | integer | **Required** |         |
| `radio`             | string  | **Required** |         |
| `rts_threshold`     | integer | **Required** | `2347`  |
| `short_retry_limit` | integer | **Required** |         |

#### band

##### WiFi Band

`band`

- is **required**
- type: reference

##### band Type

`string`

The value of this property **must** be equal to one of the [known values below](#status-known-values).

##### band Known Values

| Value |
| ----- |
| 2GHz  |
| 5GHz  |

#### beacon_int

`beacon_int`

- is **required**
- type: `integer`
- default: `100`

##### beacon_int Type

`integer`

- minimum value: `1`
- maximum value: `65535`

#### dtim_period

`dtim_period`

- is **required**
- type: `integer`
- default: `1`

##### dtim_period Type

`integer`

- minimum value: `1`
- maximum value: `255`

#### frag_threshold

`frag_threshold`

- is **required**
- type: `integer`
- default: `2346`

##### frag_threshold Type

`integer`

- minimum value: `256`
- maximum value: `65535`

#### isup

`isup`

- is **required**
- type: `boolean`

##### isup Type

`boolean`

#### long_retry_limit

`long_retry_limit`

- is **required**
- type: `integer`

##### long_retry_limit Type

`integer`

- minimum value: `1`
- maximum value: `255`

#### maxrate

`maxrate`

- is **required**
- type: `integer`

##### maxrate Type

`integer`

- minimum value: `0`

#### noise

##### Noise

`noise`

- is **required**
- type: reference

##### noise Type

`integer`

- minimum value: `-127`
- maximum value: `0`

#### radio

##### Interface

`radio`

- is **required**
- type: reference

##### radio Type

`string`

- minimum length: 1 characters
- maximum length: 16 characters

#### rts_threshold

`rts_threshold`

- is **required**
- type: `integer`
- default: `2347`

##### rts_threshold Type

`integer`

- minimum value: `0`
- maximum value: `65535`

#### short_retry_limit

`short_retry_limit`

- is **required**
- type: `integer`

##### short_retry_limit Type

`integer`

- minimum value: `1`
- maximum value: `255`

### Output Example

```json
{
  "radio": "eu",
  "isup": false,
  "band": "5GHz",
  "noise": -68,
  "maxrate": 66569043,
  "beacon_int": 55864,
  "dtim_period": 104,
  "short_retry_limit": 244,
  "long_retry_limit": 188,
  "frag_threshold": 43689,
  "rts_threshold": 47804,
  "channel": "culpa",
  "bandwidth": false
}
```
