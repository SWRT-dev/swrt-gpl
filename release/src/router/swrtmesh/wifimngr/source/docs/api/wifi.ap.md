# wifi.ap.&lt;name&gt; Schema

```
https://www.iopsys.eu/wifi.ap.json
```

| Custom Properties | Additional Properties |
| ----------------- | --------------------- |
| Forbidden         | Forbidden             |

# wifi.ap.&lt;name&gt;

| List of Methods                           |
| ----------------------------------------- |
| [add_neighbor](#add_neighbor)             | Method | wifi.ap.&lt;name&gt; (this schema) |
| [add_vendor_ie](#add_vendor_ie)           | Method | wifi.ap.&lt;name&gt; (this schema) |
| [assoclist](#assoclist)                   | Method | wifi.ap.&lt;name&gt; (this schema) |
| [del_neighbor](#del_neighbor)             | Method | wifi.ap.&lt;name&gt; (this schema) |
| [del_vendor_ie](#del_vendor_ie)           | Method | wifi.ap.&lt;name&gt; (this schema) |
| [disconnect](#disconnect)                 | Method | wifi.ap.&lt;name&gt; (this schema) |
| [dump_beacon](#dump_beacon)               | Method | wifi.ap.&lt;name&gt; (this schema) |
| [list_neighbor](#list_neighbor)           | Method | wifi.ap.&lt;name&gt; (this schema) |
| [monitor_add](#monitor_add)               | Method | wifi.ap.&lt;name&gt; (this schema) |
| [monitor_del](#monitor_del)               | Method | wifi.ap.&lt;name&gt; (this schema) |
| [monitor_get](#monitor_get)               | Method | wifi.ap.&lt;name&gt; (this schema) |
| [request_neighbor](#request_neighbor)     | Method | wifi.ap.&lt;name&gt; (this schema) |
| [request_transition](#request_transition) | Method | wifi.ap.&lt;name&gt; (this schema) |
| [stations](#stations)                     | Method | wifi.ap.&lt;name&gt; (this schema) |
| [stats](#stats)                           | Method | wifi.ap.&lt;name&gt; (this schema) |
| [status](#status)                         | Method | wifi.ap.&lt;name&gt; (this schema) |

## add_neighbor

`add_neighbor`

- type: `Method`

### add_neighbor Type

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

| Property     | Type    | Required     |
| ------------ | ------- | ------------ |
| `bssid`      | string  | **Required** |
| `bssid_info` | string  | **Required** |
| `channel`    | oneOf   | **Required** |
| `phy`        | integer | **Required** |
| `reg`        | integer | **Required** |

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

#### bssid_info

`bssid_info`

- is **required**
- type: `string`

##### bssid_info Type

`string`

#### channel

`channel`

- is **required**
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

#### phy

`phy`

- is **required**
- type: `integer`

##### phy Type

`integer`

- minimum value: `0`

#### reg

`reg`

- is **required**
- type: `integer`

##### reg Type

`integer`

- minimum value: `0`

### Ubus CLI Example

```
ubus call wifi.ap.<name> add_neighbor {"bssid":"33:fa:Dc:E4:eB:00","channel":57,"bssid_info":"sed","reg":32891575,"phy":46674216}
```

### JSONRPC Example

```json
{
  "jsonrpc": "2.0",
  "id": 0,
  "method": "call",
  "params": [
    "<SID>",
    "wifi.ap.<name>",
    "add_neighbor",
    { "bssid": "33:fa:Dc:E4:eB:00", "channel": 57, "bssid_info": "sed", "reg": 32891575, "phy": 46674216 }
  ]
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

## add_vendor_ie

`add_vendor_ie`

- type: `Method`

### add_vendor_ie Type

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
| `data`   | string  | **Required** |
| `mgmt`   | integer | **Required** |
| `oui`    | string  | **Required** |

#### data

##### Hex String

`data`

- is **required**
- type: reference

##### data Type

`string`

All instances must conform to this regular expression (test examples
[here](https://regexr.com/?expression=%5E%5B0-9a-fA-F%5D%2B)):

```regex
^[0-9a-fA-F]+
```

#### mgmt

`mgmt`

- is **required**
- type: `integer`

##### mgmt Type

`integer`

#### oui

##### Three byte oui

`oui`

- is **required**
- type: reference

##### oui Type

`string`

- minimum length: 9 characters
- maximum length: 9 characters All instances must conform to this regular expression (test examples
  [here](<https://regexr.com/?expression=%5E(%5B0-9a-fA-F%5D%5B0-9a-fA-F%5D)%7B3%7D%24>)):

```regex
^([0-9a-fA-F][0-9a-fA-F]){3}$
```

### Ubus CLI Example

```
ubus call wifi.ap.<name> add_vendor_ie {"mgmt":47091704,"oui":"74C3a11Dc","data":"cC63FfB"}
```

### JSONRPC Example

```json
{
  "jsonrpc": "2.0",
  "id": 0,
  "method": "call",
  "params": ["<SID>", "wifi.ap.<name>", "add_vendor_ie", { "mgmt": 47091704, "oui": "74C3a11Dc", "data": "cC63FfB" }]
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

## assoclist

`assoclist`

- type: `Method`

### assoclist Type

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
ubus call wifi.ap.<name> assoclist {}
```

### JSONRPC Example

```json
{ "jsonrpc": "2.0", "id": 0, "method": "call", "params": ["<SID>", "wifi.ap.<name>", "assoclist", {}] }
```

#### output

`output`

- is optional
- type: `object`

##### output Type

`object` with following properties:

| Property    | Type  | Required     |
| ----------- | ----- | ------------ |
| `assoclist` | array | **Required** |

#### assoclist

`assoclist`

- is **required**
- type: `object[]`

##### assoclist Type

Array type: `object[]`

All items must be of the type: `object` with following properties:

| Property  | Type   | Required     |
| --------- | ------ | ------------ |
| `macaddr` | string | **Required** |
| `wdev`    | string | **Required** |

#### macaddr

##### MAC Address

`macaddr`

- is **required**
- type: reference

##### macaddr Type

`string`

- minimum length: 17 characters
- maximum length: 17 characters All instances must conform to this regular expression (test examples
  [here](<https://regexr.com/?expression=%5E(%5B0-9a-fA-F%5D%5B0-9a-fA-F%5D%3A)%7B5%7D%5B0-9a-fA-F%5D%5B0-9a-fA-F%5D%24>)):

```regex
^([0-9a-fA-F][0-9a-fA-F]:){5}[0-9a-fA-F][0-9a-fA-F]$
```

#### wdev

##### Interface

`wdev`

- is **required**
- type: reference

##### wdev Type

`string`

- minimum length: 1 characters
- maximum length: 16 characters

### Output Example

```json
{ "assoclist": [{ "wdev": "culpa", "macaddr": "Ee:21:45:20:43:2A" }] }
```

## del_neighbor

`del_neighbor`

- type: `Method`

### del_neighbor Type

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

| Property | Type   | Required     |
| -------- | ------ | ------------ |
| `bssid`  | string | **Required** |

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

### Ubus CLI Example

```
ubus call wifi.ap.<name> del_neighbor {"bssid":"ca:dC:0b:0a:c8:6c"}
```

### JSONRPC Example

```json
{
  "jsonrpc": "2.0",
  "id": 0,
  "method": "call",
  "params": ["<SID>", "wifi.ap.<name>", "del_neighbor", { "bssid": "ca:dC:0b:0a:c8:6c" }]
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

## del_vendor_ie

`del_vendor_ie`

- type: `Method`

### del_vendor_ie Type

`object` with following properties:

| Property | Type   | Required |
| -------- | ------ | -------- |
| `intput` | object | Optional |
| `output` | object | Optional |

#### intput

`intput`

- is optional
- type: `object`

##### intput Type

`object` with following properties:

| Property | Type    | Required     |
| -------- | ------- | ------------ |
| `data`   | string  | **Required** |
| `mgmt`   | integer | **Required** |
| `oui`    | string  | **Required** |

#### data

##### Hex String

`data`

- is **required**
- type: reference

##### data Type

`string`

All instances must conform to this regular expression (test examples
[here](https://regexr.com/?expression=%5E%5B0-9a-fA-F%5D%2B)):

```regex
^[0-9a-fA-F]+
```

#### mgmt

`mgmt`

- is **required**
- type: `integer`

##### mgmt Type

`integer`

#### oui

##### Three byte oui

`oui`

- is **required**
- type: reference

##### oui Type

`string`

- minimum length: 9 characters
- maximum length: 9 characters All instances must conform to this regular expression (test examples
  [here](<https://regexr.com/?expression=%5E(%5B0-9a-fA-F%5D%5B0-9a-fA-F%5D)%7B3%7D%24>)):

```regex
^([0-9a-fA-F][0-9a-fA-F]){3}$
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

## disconnect

`disconnect`

- type: `Method`

### disconnect Type

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

| Property | Type   | Required     |
| -------- | ------ | ------------ |
| `sta`    | string | **Required** |

#### sta

##### MAC Address

`sta`

- is **required**
- type: reference

##### sta Type

`string`

- minimum length: 17 characters
- maximum length: 17 characters All instances must conform to this regular expression (test examples
  [here](<https://regexr.com/?expression=%5E(%5B0-9a-fA-F%5D%5B0-9a-fA-F%5D%3A)%7B5%7D%5B0-9a-fA-F%5D%5B0-9a-fA-F%5D%24>)):

```regex
^([0-9a-fA-F][0-9a-fA-F]:){5}[0-9a-fA-F][0-9a-fA-F]$
```

### Ubus CLI Example

```
ubus call wifi.ap.<name> disconnect {"sta":"Ef:5C:15:AF:63:Fb"}
```

### JSONRPC Example

```json
{
  "jsonrpc": "2.0",
  "id": 0,
  "method": "call",
  "params": ["<SID>", "wifi.ap.<name>", "disconnect", { "sta": "Ef:5C:15:AF:63:Fb" }]
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

## dump_beacon

`dump_beacon`

- type: `Method`

### dump_beacon Type

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
ubus call wifi.ap.<name> dump_beacon {}
```

### JSONRPC Example

```json
{ "jsonrpc": "2.0", "id": 0, "method": "call", "params": ["<SID>", "wifi.ap.<name>", "dump_beacon", {}] }
```

#### output

`output`

- is optional
- type: `object`

##### output Type

`object` with following properties:

| Property     | Type  | Required     |
| ------------ | ----- | ------------ |
| `beacon-ies` | array | **Required** |

#### beacon-ies

`beacon-ies`

- is **required**
- type: `string[]`

##### beacon-ies Type

Array type: `string[]`

All items must be of the type: `string`

All instances must conform to this regular expression (test examples
[here](https://regexr.com/?expression=%5E%5B0-9a-fA-F%5D%2B%24)):

```regex
^[0-9a-fA-F]+$
```

### Output Example

```json
{ "beacon-ies": ["dBd", "2BC8d", "74dFac", "8Ee30fE186"] }
```

## list_neighbor

`list_neighbor`

- type: `Method`

### list_neighbor Type

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
| `client` | string | Optional |
| `ssid`   | string | Optional |

#### client

##### MAC Address

`client`

- is optional
- type: reference

##### client Type

`string`

- minimum length: 17 characters
- maximum length: 17 characters All instances must conform to this regular expression (test examples
  [here](<https://regexr.com/?expression=%5E(%5B0-9a-fA-F%5D%5B0-9a-fA-F%5D%3A)%7B5%7D%5B0-9a-fA-F%5D%5B0-9a-fA-F%5D%24>)):

```regex
^([0-9a-fA-F][0-9a-fA-F]:){5}[0-9a-fA-F][0-9a-fA-F]$
```

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
ubus call wifi.ap.<name> list_neighbor {"ssid":"ut","client":"3E:5A:Eb:a2:ED:ab"}
```

### JSONRPC Example

```json
{
  "jsonrpc": "2.0",
  "id": 0,
  "method": "call",
  "params": ["<SID>", "wifi.ap.<name>", "list_neighbor", { "ssid": "ut", "client": "3E:5A:Eb:a2:ED:ab" }]
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
{ "neighbors": 25886372 }
```

## monitor_add

`monitor_add`

- type: `Method`

### monitor_add Type

`object` with following properties:

| Property | Type   | Required |
| -------- | ------ | -------- |
| `intput` | object | Optional |
| `output` | object | Optional |

#### intput

`intput`

- is optional
- type: `object`

##### intput Type

`object` with following properties:

| Property | Type    | Required     |
| -------- | ------- | ------------ |
| `get`    | integer | Optional     |
| `sta`    | string  | **Required** |

#### get

`get`

- is optional
- type: `integer`

##### get Type

`integer`

#### sta

##### MAC Address

`sta`

- is **required**
- type: reference

##### sta Type

`string`

- minimum length: 17 characters
- maximum length: 17 characters All instances must conform to this regular expression (test examples
  [here](<https://regexr.com/?expression=%5E(%5B0-9a-fA-F%5D%5B0-9a-fA-F%5D%3A)%7B5%7D%5B0-9a-fA-F%5D%5B0-9a-fA-F%5D%24>)):

```regex
^([0-9a-fA-F][0-9a-fA-F]:){5}[0-9a-fA-F][0-9a-fA-F]$
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

## monitor_del

`monitor_del`

- type: `Method`

### monitor_del Type

`object` with following properties:

| Property | Type   | Required |
| -------- | ------ | -------- |
| `intput` | object | Optional |
| `output` | object | Optional |

#### intput

`intput`

- is optional
- type: `object`

##### intput Type

`object` with following properties:

| Property | Type    | Required     |
| -------- | ------- | ------------ |
| `get`    | integer | Optional     |
| `sta`    | string  | **Required** |

#### get

`get`

- is optional
- type: `integer`

##### get Type

`integer`

#### sta

##### MAC Address

`sta`

- is **required**
- type: reference

##### sta Type

`string`

- minimum length: 17 characters
- maximum length: 17 characters All instances must conform to this regular expression (test examples
  [here](<https://regexr.com/?expression=%5E(%5B0-9a-fA-F%5D%5B0-9a-fA-F%5D%3A)%7B5%7D%5B0-9a-fA-F%5D%5B0-9a-fA-F%5D%24>)):

```regex
^([0-9a-fA-F][0-9a-fA-F]:){5}[0-9a-fA-F][0-9a-fA-F]$
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

## monitor_get

`monitor_get`

- type: `Method`

### monitor_get Type

`object` with following properties:

| Property | Type   | Required |
| -------- | ------ | -------- |
| `intput` | object | Optional |
| `output` | object | Optional |

#### intput

`intput`

- is optional
- type: `object`

##### intput Type

`object` with following properties:

| Property | Type    | Required     |
| -------- | ------- | ------------ |
| `get`    | integer | Optional     |
| `sta`    | string  | **Required** |

#### get

`get`

- is optional
- type: `integer`

##### get Type

`integer`

#### sta

##### MAC Address

`sta`

- is **required**
- type: reference

##### sta Type

`string`

- minimum length: 17 characters
- maximum length: 17 characters All instances must conform to this regular expression (test examples
  [here](<https://regexr.com/?expression=%5E(%5B0-9a-fA-F%5D%5B0-9a-fA-F%5D%3A)%7B5%7D%5B0-9a-fA-F%5D%5B0-9a-fA-F%5D%24>)):

```regex
^([0-9a-fA-F][0-9a-fA-F]:){5}[0-9a-fA-F][0-9a-fA-F]$
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

## request_neighbor

`request_neighbor`

- type: `Method`

### request_neighbor Type

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

| Property | Type   | Required     |
| -------- | ------ | ------------ |
| `client` | string | **Required** |
| `ssid`   | string | Optional     |

#### client

##### MAC Address

`client`

- is **required**
- type: reference

##### client Type

`string`

- minimum length: 17 characters
- maximum length: 17 characters All instances must conform to this regular expression (test examples
  [here](<https://regexr.com/?expression=%5E(%5B0-9a-fA-F%5D%5B0-9a-fA-F%5D%3A)%7B5%7D%5B0-9a-fA-F%5D%5B0-9a-fA-F%5D%24>)):

```regex
^([0-9a-fA-F][0-9a-fA-F]:){5}[0-9a-fA-F][0-9a-fA-F]$
```

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
ubus call wifi.ap.<name> request_neighbor {"client":"0F:d3:c6:B5:2B:25","ssid":"laborum quis ut deserunt"}
```

### JSONRPC Example

```json
{
  "jsonrpc": "2.0",
  "id": 0,
  "method": "call",
  "params": [
    "<SID>",
    "wifi.ap.<name>",
    "request_neighbor",
    { "client": "0F:d3:c6:B5:2B:25", "ssid": "laborum quis ut deserunt" }
  ]
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

## request_transition

`request_transition`

- type: `Method`

### request_transition Type

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

| Property  | Type    | Required     |
| --------- | ------- | ------------ |
| `bssid`   | array   | **Required** |
| `client`  | string  | **Required** |
| `timeout` | integer | Optional     |

#### bssid

`bssid`

- is **required**
- type: reference

##### bssid Type

Array type: reference

All items must be of the type: `string`

- minimum length: 17 characters
- maximum length: 17 characters All instances must conform to this regular expression (test examples
  [here](<https://regexr.com/?expression=%5E(%5B0-9a-fA-F%5D%5B0-9a-fA-F%5D%3A)%7B5%7D%5B0-9a-fA-F%5D%5B0-9a-fA-F%5D%24>)):

```regex
^([0-9a-fA-F][0-9a-fA-F]:){5}[0-9a-fA-F][0-9a-fA-F]$
```

#### client

##### MAC Address

`client`

- is **required**
- type: reference

##### client Type

`string`

- minimum length: 17 characters
- maximum length: 17 characters All instances must conform to this regular expression (test examples
  [here](<https://regexr.com/?expression=%5E(%5B0-9a-fA-F%5D%5B0-9a-fA-F%5D%3A)%7B5%7D%5B0-9a-fA-F%5D%5B0-9a-fA-F%5D%24>)):

```regex
^([0-9a-fA-F][0-9a-fA-F]:){5}[0-9a-fA-F][0-9a-fA-F]$
```

#### timeout

`timeout`

- is optional
- type: `integer`

##### timeout Type

`integer`

### Ubus CLI Example

```
ubus call wifi.ap.<name> request_transition {"client":"BC:D5:a2:b8:12:40","bssid":["0B:DA:C1:FF:CF:c3","A4:b3:62:A7:Cb:09"],"timeout":-57832211}
```

### JSONRPC Example

```json
{
  "jsonrpc": "2.0",
  "id": 0,
  "method": "call",
  "params": [
    "<SID>",
    "wifi.ap.<name>",
    "request_transition",
    { "client": "BC:D5:a2:b8:12:40", "bssid": ["0B:DA:C1:FF:CF:c3", "A4:b3:62:A7:Cb:09"], "timeout": -57832211 }
  ]
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

## stations

`stations`

- type: `Method`

### stations Type

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
| `sta`    | string | Optional |

#### sta

##### MAC Address

`sta`

- is optional
- type: reference

##### sta Type

`string`

- minimum length: 17 characters
- maximum length: 17 characters All instances must conform to this regular expression (test examples
  [here](<https://regexr.com/?expression=%5E(%5B0-9a-fA-F%5D%5B0-9a-fA-F%5D%3A)%7B5%7D%5B0-9a-fA-F%5D%5B0-9a-fA-F%5D%24>)):

```regex
^([0-9a-fA-F][0-9a-fA-F]:){5}[0-9a-fA-F][0-9a-fA-F]$
```

### Ubus CLI Example

```
ubus call wifi.ap.<name> stations {"sta":"1A:b5:c6:cc:cF:84"}
```

### JSONRPC Example

```json
{
  "jsonrpc": "2.0",
  "id": 0,
  "method": "call",
  "params": ["<SID>", "wifi.ap.<name>", "stations", { "sta": "1A:b5:c6:cc:cF:84" }]
}
```

#### output

`output`

- is optional
- type: `object`

##### output Type

`object` with following properties:

| Property   | Type  | Required     |
| ---------- | ----- | ------------ |
| `stations` | array | **Required** |

#### stations

`stations`

- is **required**
- type: `object[]`

##### stations Type

Array type: `object[]`

All items must be of the type: `object` with following properties:

| Property           | Type    | Required     |
| ------------------ | ------- | ------------ |
| `airtime`          | integer | **Required** |
| `capabilities`     | object  | **Required** |
| `frequency`        | string  | **Required** |
| `idle`             | integer | **Required** |
| `in_network`       | integer | **Required** |
| `macaddr`          | string  | **Required** |
| `max_rate`         | integer | **Required** |
| `rssi`             | integer | **Required** |
| `rssi_per_antenna` | array   | **Required** |
| `rx_airtime`       | integer | **Required** |
| `snr`              | integer | **Required** |
| `stats`            | object  | **Required** |
| `status`           | object  | **Required** |
| `tx_airtime`       | integer | **Required** |
| `wdev`             | string  | **Required** |

#### airtime

`airtime`

- is **required**
- type: `integer`

##### airtime Type

`integer`

- minimum value: `0`
- maximum value: `100`

#### capabilities

`capabilities`

- is **required**
- type: `object`

##### capabilities Type

`object` with following properties:

| Property     | Type    | Required     |
| ------------ | ------- | ------------ |
| `2040coex`   | boolean | **Required** |
| `apsd`       | boolean | **Required** |
| `dot11ac`    | object  | Optional     |
| `dot11h`     | boolean | **Required** |
| `dot11k`     | object  | Optional     |
| `dot11n`     | object  | Optional     |
| `dot11v_btm` | boolean | **Required** |
| `proxy_arp`  | boolean | **Required** |
| `psmp`       | boolean | **Required** |
| `shortslot`  | boolean | **Required** |
| `wmm`        | boolean | **Required** |

#### 2040coex

`2040coex`

- is **required**
- type: `boolean`

##### 2040coex Type

`boolean`

#### apsd

`apsd`

- is **required**
- type: `boolean`

##### apsd Type

`boolean`

#### dot11ac

`dot11ac`

- is optional
- type: `object`

##### dot11ac Type

`object` with following properties:

| Property                | Type    | Required     |
| ----------------------- | ------- | ------------ |
| `dot11ac_160`           | boolean | **Required** |
| `dot11ac_8080`          | boolean | **Required** |
| `dot11ac_mpdu_max`      | integer | **Required** |
| `dot11ac_mu_beamformee` | boolean | **Required** |
| `dot11ac_mu_beamformer` | boolean | **Required** |
| `dot11ac_rx_ldpc`       | boolean | **Required** |
| `dot11ac_rx_stbc_1ss`   | boolean | **Required** |
| `dot11ac_rx_stbc_2ss`   | boolean | **Required** |
| `dot11ac_rx_stbc_3ss`   | boolean | **Required** |
| `dot11ac_rx_stbc_4ss`   | boolean | **Required** |
| `dot11ac_sgi160`        | boolean | **Required** |
| `dot11ac_sgi80`         | boolean | **Required** |
| `dot11ac_su_beamformee` | boolean | **Required** |
| `dot11ac_su_beamformer` | boolean | **Required** |
| `dot11ac_tx_stbc`       | boolean | **Required** |

#### dot11ac_160

`dot11ac_160`

- is **required**
- type: `boolean`

##### dot11ac_160 Type

`boolean`

#### dot11ac_8080

`dot11ac_8080`

- is **required**
- type: `boolean`

##### dot11ac_8080 Type

`boolean`

#### dot11ac_mpdu_max

`dot11ac_mpdu_max`

- is **required**
- type: `integer`

##### dot11ac_mpdu_max Type

`integer`

- minimum value: `0`
- maximum value: `65535`

#### dot11ac_mu_beamformee

`dot11ac_mu_beamformee`

- is **required**
- type: `boolean`

##### dot11ac_mu_beamformee Type

`boolean`

#### dot11ac_mu_beamformer

`dot11ac_mu_beamformer`

- is **required**
- type: `boolean`

##### dot11ac_mu_beamformer Type

`boolean`

#### dot11ac_rx_ldpc

`dot11ac_rx_ldpc`

- is **required**
- type: `boolean`

##### dot11ac_rx_ldpc Type

`boolean`

#### dot11ac_rx_stbc_1ss

`dot11ac_rx_stbc_1ss`

- is **required**
- type: `boolean`

##### dot11ac_rx_stbc_1ss Type

`boolean`

#### dot11ac_rx_stbc_2ss

`dot11ac_rx_stbc_2ss`

- is **required**
- type: `boolean`

##### dot11ac_rx_stbc_2ss Type

`boolean`

#### dot11ac_rx_stbc_3ss

`dot11ac_rx_stbc_3ss`

- is **required**
- type: `boolean`

##### dot11ac_rx_stbc_3ss Type

`boolean`

#### dot11ac_rx_stbc_4ss

`dot11ac_rx_stbc_4ss`

- is **required**
- type: `boolean`

##### dot11ac_rx_stbc_4ss Type

`boolean`

#### dot11ac_sgi160

`dot11ac_sgi160`

- is **required**
- type: `boolean`

##### dot11ac_sgi160 Type

`boolean`

#### dot11ac_sgi80

`dot11ac_sgi80`

- is **required**
- type: `boolean`

##### dot11ac_sgi80 Type

`boolean`

#### dot11ac_su_beamformee

`dot11ac_su_beamformee`

- is **required**
- type: `boolean`

##### dot11ac_su_beamformee Type

`boolean`

#### dot11ac_su_beamformer

`dot11ac_su_beamformer`

- is **required**
- type: `boolean`

##### dot11ac_su_beamformer Type

`boolean`

#### dot11ac_tx_stbc

`dot11ac_tx_stbc`

- is **required**
- type: `boolean`

##### dot11ac_tx_stbc Type

`boolean`

#### dot11h

`dot11h`

- is **required**
- type: `boolean`

##### dot11h Type

`boolean`

#### dot11k

`dot11k`

- is optional
- type: `object`

##### dot11k Type

`object` with following properties:

| Property             | Type    | Required     |
| -------------------- | ------- | ------------ |
| `dot11k_bcn_active`  | boolean | **Required** |
| `dot11k_bcn_passive` | boolean | **Required** |
| `dot11k_bcn_table`   | boolean | **Required** |
| `dot11k_link_meas`   | boolean | **Required** |
| `dot11k_nbr_report`  | boolean | **Required** |
| `dot11k_rcpi`        | boolean | **Required** |
| `dot11k_rsni`        | boolean | **Required** |

#### dot11k_bcn_active

`dot11k_bcn_active`

- is **required**
- type: `boolean`

##### dot11k_bcn_active Type

`boolean`

#### dot11k_bcn_passive

`dot11k_bcn_passive`

- is **required**
- type: `boolean`

##### dot11k_bcn_passive Type

`boolean`

#### dot11k_bcn_table

`dot11k_bcn_table`

- is **required**
- type: `boolean`

##### dot11k_bcn_table Type

`boolean`

#### dot11k_link_meas

`dot11k_link_meas`

- is **required**
- type: `boolean`

##### dot11k_link_meas Type

`boolean`

#### dot11k_nbr_report

`dot11k_nbr_report`

- is **required**
- type: `boolean`

##### dot11k_nbr_report Type

`boolean`

#### dot11k_rcpi

`dot11k_rcpi`

- is **required**
- type: `boolean`

##### dot11k_rcpi Type

`boolean`

#### dot11k_rsni

`dot11k_rsni`

- is **required**
- type: `boolean`

##### dot11k_rsni Type

`boolean`

#### dot11n

`dot11n`

- is optional
- type: `object`

##### dot11n Type

`object` with following properties:

| Property         | Type    | Required     |
| ---------------- | ------- | ------------ |
| `dot11n_40`      | boolean | **Required** |
| `dot11n_ldpc`    | boolean | **Required** |
| `dot11n_ps`      | boolean | **Required** |
| `dot11n_rx_stbc` | boolean | **Required** |
| `dot11n_sgi20`   | boolean | **Required** |
| `dot11n_sgi40`   | boolean | **Required** |
| `dot11n_tx_stbc` | boolean | **Required** |

#### dot11n_40

`dot11n_40`

- is **required**
- type: `boolean`

##### dot11n_40 Type

`boolean`

#### dot11n_ldpc

`dot11n_ldpc`

- is **required**
- type: `boolean`

##### dot11n_ldpc Type

`boolean`

#### dot11n_ps

`dot11n_ps`

- is **required**
- type: `boolean`

##### dot11n_ps Type

`boolean`

#### dot11n_rx_stbc

`dot11n_rx_stbc`

- is **required**
- type: `boolean`

##### dot11n_rx_stbc Type

`boolean`

#### dot11n_sgi20

`dot11n_sgi20`

- is **required**
- type: `boolean`

##### dot11n_sgi20 Type

`boolean`

#### dot11n_sgi40

`dot11n_sgi40`

- is **required**
- type: `boolean`

##### dot11n_sgi40 Type

`boolean`

#### dot11n_tx_stbc

`dot11n_tx_stbc`

- is **required**
- type: `boolean`

##### dot11n_tx_stbc Type

`boolean`

#### dot11v_btm

`dot11v_btm`

- is **required**
- type: `boolean`

##### dot11v_btm Type

`boolean`

#### proxy_arp

`proxy_arp`

- is **required**
- type: `boolean`

##### proxy_arp Type

`boolean`

#### psmp

`psmp`

- is **required**
- type: `boolean`

##### psmp Type

`boolean`

#### shortslot

`shortslot`

- is **required**
- type: `boolean`

##### shortslot Type

`boolean`

#### wmm

`wmm`

- is **required**
- type: `boolean`

##### wmm Type

`boolean`

#### frequency

##### WiFi Band

`frequency`

- is **required**
- type: reference

##### frequency Type

`string`

The value of this property **must** be equal to one of the [known values below](#stations-known-values).

##### frequency Known Values

| Value |
| ----- |
| 2GHz  |
| 5GHz  |

#### idle

`idle`

- is **required**
- type: `integer`

##### idle Type

`integer`

- minimum value: `0`

#### in_network

`in_network`

- is **required**
- type: `integer`

##### in_network Type

`integer`

- minimum value: `0`

#### macaddr

##### MAC Address

`macaddr`

- is **required**
- type: reference

##### macaddr Type

`string`

- minimum length: 17 characters
- maximum length: 17 characters All instances must conform to this regular expression (test examples
  [here](<https://regexr.com/?expression=%5E(%5B0-9a-fA-F%5D%5B0-9a-fA-F%5D%3A)%7B5%7D%5B0-9a-fA-F%5D%5B0-9a-fA-F%5D%24>)):

```regex
^([0-9a-fA-F][0-9a-fA-F]:){5}[0-9a-fA-F][0-9a-fA-F]$
```

#### max_rate

`max_rate`

- is **required**
- type: `integer`

##### max_rate Type

`integer`

- minimum value: `0`

#### rssi

##### RSSI

`rssi`

- is **required**
- type: reference

##### rssi Type

`integer`

- minimum value: `-128`
- maximum value: `0`

#### rssi_per_antenna

`rssi_per_antenna`

- is **required**
- type: reference

##### rssi_per_antenna Type

Array type: reference

All items must be of the type: `integer`

- minimum value: `-128`
- maximum value: `0`

#### rx_airtime

`rx_airtime`

- is **required**
- type: `integer`

##### rx_airtime Type

`integer`

- minimum value: `0`
- maximum value: `100`

#### snr

`snr`

- is **required**
- type: `integer`

##### snr Type

`integer`

- minimum value: `0`
- maximum value: `100`

#### stats

`stats`

- is **required**
- type: `object`

##### stats Type

`object` with following properties:

| Property              | Type    | Required     |
| --------------------- | ------- | ------------ |
| `rate_of_last_rx_pkt` | integer | **Required** |
| `rate_of_last_tx_pkt` | integer | **Required** |
| `rx_data_bytes`       | integer | **Required** |
| `rx_data_pkts`        | integer | **Required** |
| `rx_failures`         | integer | **Required** |
| `tx_failures`         | integer | **Required** |
| `tx_pkts_retries`     | integer | **Required** |
| `tx_total_bytes`      | integer | **Required** |
| `tx_total_pkts`       | integer | **Required** |

#### rate_of_last_rx_pkt

`rate_of_last_rx_pkt`

- is **required**
- type: reference

##### rate_of_last_rx_pkt Type

`integer`

- minimum value: `0`

#### rate_of_last_tx_pkt

`rate_of_last_tx_pkt`

- is **required**
- type: reference

##### rate_of_last_tx_pkt Type

`integer`

- minimum value: `0`

#### rx_data_bytes

`rx_data_bytes`

- is **required**
- type: reference

##### rx_data_bytes Type

`integer`

- minimum value: `0`

#### rx_data_pkts

`rx_data_pkts`

- is **required**
- type: reference

##### rx_data_pkts Type

`integer`

- minimum value: `0`

#### rx_failures

`rx_failures`

- is **required**
- type: reference

##### rx_failures Type

`integer`

- minimum value: `0`

#### tx_failures

`tx_failures`

- is **required**
- type: reference

##### tx_failures Type

`integer`

- minimum value: `0`

#### tx_pkts_retries

`tx_pkts_retries`

- is **required**
- type: reference

##### tx_pkts_retries Type

`integer`

- minimum value: `0`

#### tx_total_bytes

`tx_total_bytes`

- is **required**
- type: reference

##### tx_total_bytes Type

`integer`

- minimum value: `0`

#### tx_total_pkts

`tx_total_pkts`

- is **required**
- type: reference

##### tx_total_pkts Type

`integer`

- minimum value: `0`

#### status

`status`

- is **required**
- type: `object`

##### status Type

`object` with following properties:

| Property | Type    | Required     |
| -------- | ------- | ------------ |
| `ps`     | boolean | **Required** |
| `wmm`    | boolean | **Required** |

#### ps

`ps`

- is **required**
- type: `boolean`

##### ps Type

`boolean`

#### wmm

`wmm`

- is **required**
- type: `boolean`

##### wmm Type

`boolean`

#### tx_airtime

`tx_airtime`

- is **required**
- type: `integer`

##### tx_airtime Type

`integer`

- minimum value: `0`
- maximum value: `100`

#### wdev

##### Interface

`wdev`

- is **required**
- type: reference

##### wdev Type

`string`

- minimum length: 1 characters
- maximum length: 16 characters

### Output Example

```json
{
  "stations": [
    {
      "macaddr": "f9:AA:cb:4f:72:2b",
      "wdev": "labore",
      "frequency": "2GHz",
      "rssi": -43,
      "snr": 11,
      "idle": 46292766,
      "in_network": 87629928,
      "tx_airtime": 86,
      "rx_airtime": 86,
      "airtime": 91,
      "max_rate": 27575379,
      "status": { "wmm": true, "ps": false },
      "capabilities": {
        "wmm": false,
        "apsd": false,
        "shortslot": true,
        "dot11h": false,
        "2040coex": false,
        "psmp": true,
        "proxy_arp": false,
        "dot11v_btm": true,
        "dot11k": {
          "dot11k_link_meas": false,
          "dot11k_nbr_report": true,
          "dot11k_bcn_passive": false,
          "dot11k_bcn_active": true,
          "dot11k_bcn_table": true,
          "dot11k_rcpi": false,
          "dot11k_rsni": false
        },
        "dot11n": {
          "dot11n_ldpc": true,
          "dot11n_40": false,
          "dot11n_ps": true,
          "dot11n_sgi20": false,
          "dot11n_sgi40": false,
          "dot11n_tx_stbc": false,
          "dot11n_rx_stbc": true
        },
        "dot11ac": {
          "dot11ac_160": true,
          "dot11ac_8080": false,
          "dot11ac_mpdu_max": 12156,
          "dot11ac_sgi80": true,
          "dot11ac_sgi160": false,
          "dot11ac_rx_ldpc": false,
          "dot11ac_tx_stbc": false,
          "dot11ac_rx_stbc_1ss": true,
          "dot11ac_rx_stbc_2ss": true,
          "dot11ac_rx_stbc_3ss": true,
          "dot11ac_rx_stbc_4ss": false,
          "dot11ac_su_beamformer": false,
          "dot11ac_su_beamformee": true,
          "dot11ac_mu_beamformer": false,
          "dot11ac_mu_beamformee": false
        }
      },
      "stats": {
        "tx_total_pkts": 37714339,
        "tx_total_bytes": 21585078,
        "tx_failures": 22920021,
        "tx_pkts_retries": 89128137,
        "rx_data_pkts": 79427668,
        "rx_data_bytes": 37890537,
        "rx_failures": 26514072,
        "rate_of_last_tx_pkt": 17359622,
        "rate_of_last_rx_pkt": 15859057
      },
      "rssi_per_antenna": [-75, -87, -34, -33, -28]
    },
    {
      "macaddr": "bF:5E:ab:F1:6c:f4",
      "wdev": "e",
      "frequency": "5GHz",
      "rssi": -39,
      "snr": 79,
      "idle": 84221277,
      "in_network": 43375997,
      "tx_airtime": 43,
      "rx_airtime": 1,
      "airtime": 8,
      "max_rate": 86061640,
      "status": { "wmm": false, "ps": false },
      "capabilities": {
        "wmm": true,
        "apsd": true,
        "shortslot": false,
        "dot11h": true,
        "2040coex": true,
        "psmp": false,
        "proxy_arp": true,
        "dot11v_btm": true,
        "dot11k": {
          "dot11k_link_meas": true,
          "dot11k_nbr_report": true,
          "dot11k_bcn_passive": false,
          "dot11k_bcn_active": false,
          "dot11k_bcn_table": true,
          "dot11k_rcpi": false,
          "dot11k_rsni": true
        },
        "dot11n": {
          "dot11n_ldpc": true,
          "dot11n_40": true,
          "dot11n_ps": false,
          "dot11n_sgi20": true,
          "dot11n_sgi40": false,
          "dot11n_tx_stbc": false,
          "dot11n_rx_stbc": false
        },
        "dot11ac": {
          "dot11ac_160": false,
          "dot11ac_8080": true,
          "dot11ac_mpdu_max": 40789,
          "dot11ac_sgi80": false,
          "dot11ac_sgi160": true,
          "dot11ac_rx_ldpc": true,
          "dot11ac_tx_stbc": true,
          "dot11ac_rx_stbc_1ss": false,
          "dot11ac_rx_stbc_2ss": false,
          "dot11ac_rx_stbc_3ss": false,
          "dot11ac_rx_stbc_4ss": true,
          "dot11ac_su_beamformer": true,
          "dot11ac_su_beamformee": false,
          "dot11ac_mu_beamformer": false,
          "dot11ac_mu_beamformee": false
        }
      },
      "stats": {
        "tx_total_pkts": 35042834,
        "tx_total_bytes": 92577562,
        "tx_failures": 31914836,
        "tx_pkts_retries": 33277681,
        "rx_data_pkts": 74224435,
        "rx_data_bytes": 48693435,
        "rx_failures": 89031594,
        "rate_of_last_tx_pkt": 48623131,
        "rate_of_last_rx_pkt": 17272916
      },
      "rssi_per_antenna": [-63]
    },
    {
      "macaddr": "3b:d3:5b:62:ed:16",
      "wdev": "pr",
      "frequency": "5GHz",
      "rssi": -119,
      "snr": 54,
      "idle": 85478408,
      "in_network": 39941133,
      "tx_airtime": 65,
      "rx_airtime": 7,
      "airtime": 70,
      "max_rate": 39642724,
      "status": { "wmm": true, "ps": true },
      "capabilities": {
        "wmm": false,
        "apsd": false,
        "shortslot": false,
        "dot11h": true,
        "2040coex": false,
        "psmp": false,
        "proxy_arp": true,
        "dot11v_btm": true,
        "dot11k": {
          "dot11k_link_meas": false,
          "dot11k_nbr_report": false,
          "dot11k_bcn_passive": true,
          "dot11k_bcn_active": true,
          "dot11k_bcn_table": false,
          "dot11k_rcpi": false,
          "dot11k_rsni": true
        },
        "dot11n": {
          "dot11n_ldpc": true,
          "dot11n_40": false,
          "dot11n_ps": true,
          "dot11n_sgi20": false,
          "dot11n_sgi40": true,
          "dot11n_tx_stbc": false,
          "dot11n_rx_stbc": true
        },
        "dot11ac": {
          "dot11ac_160": false,
          "dot11ac_8080": true,
          "dot11ac_mpdu_max": 51094,
          "dot11ac_sgi80": false,
          "dot11ac_sgi160": false,
          "dot11ac_rx_ldpc": true,
          "dot11ac_tx_stbc": true,
          "dot11ac_rx_stbc_1ss": true,
          "dot11ac_rx_stbc_2ss": true,
          "dot11ac_rx_stbc_3ss": false,
          "dot11ac_rx_stbc_4ss": true,
          "dot11ac_su_beamformer": false,
          "dot11ac_su_beamformee": false,
          "dot11ac_mu_beamformer": false,
          "dot11ac_mu_beamformee": true
        }
      },
      "stats": {
        "tx_total_pkts": 68736896,
        "tx_total_bytes": 84047469,
        "tx_failures": 45303081,
        "tx_pkts_retries": 41481660,
        "rx_data_pkts": 82456157,
        "rx_data_bytes": 9742973,
        "rx_failures": 59708409,
        "rate_of_last_tx_pkt": 51320415,
        "rate_of_last_rx_pkt": 86185128
      },
      "rssi_per_antenna": [-47, -72]
    },
    {
      "macaddr": "74:82:19:A4:88:16",
      "wdev": "veli",
      "frequency": "5GHz",
      "rssi": -18,
      "snr": 8,
      "idle": 33866637,
      "in_network": 76787503,
      "tx_airtime": 72,
      "rx_airtime": 55,
      "airtime": 82,
      "max_rate": 93888260,
      "status": { "wmm": false, "ps": true },
      "capabilities": {
        "wmm": true,
        "apsd": false,
        "shortslot": true,
        "dot11h": true,
        "2040coex": true,
        "psmp": true,
        "proxy_arp": false,
        "dot11v_btm": false,
        "dot11k": {
          "dot11k_link_meas": true,
          "dot11k_nbr_report": false,
          "dot11k_bcn_passive": false,
          "dot11k_bcn_active": true,
          "dot11k_bcn_table": false,
          "dot11k_rcpi": true,
          "dot11k_rsni": false
        },
        "dot11n": {
          "dot11n_ldpc": false,
          "dot11n_40": false,
          "dot11n_ps": true,
          "dot11n_sgi20": false,
          "dot11n_sgi40": true,
          "dot11n_tx_stbc": false,
          "dot11n_rx_stbc": false
        },
        "dot11ac": {
          "dot11ac_160": true,
          "dot11ac_8080": false,
          "dot11ac_mpdu_max": 25026,
          "dot11ac_sgi80": true,
          "dot11ac_sgi160": true,
          "dot11ac_rx_ldpc": true,
          "dot11ac_tx_stbc": false,
          "dot11ac_rx_stbc_1ss": true,
          "dot11ac_rx_stbc_2ss": true,
          "dot11ac_rx_stbc_3ss": true,
          "dot11ac_rx_stbc_4ss": false,
          "dot11ac_su_beamformer": true,
          "dot11ac_su_beamformee": true,
          "dot11ac_mu_beamformer": false,
          "dot11ac_mu_beamformee": false
        }
      },
      "stats": {
        "tx_total_pkts": 84653504,
        "tx_total_bytes": 79348632,
        "tx_failures": 98031000,
        "tx_pkts_retries": 54619147,
        "rx_data_pkts": 8568864,
        "rx_data_bytes": 94837678,
        "rx_failures": 85984658,
        "rate_of_last_tx_pkt": 21836133,
        "rate_of_last_rx_pkt": 54960172
      },
      "rssi_per_antenna": [-20, -65, -118]
    },
    {
      "macaddr": "10:bD:7d:dA:65:DB",
      "wdev": "aute do",
      "frequency": "2GHz",
      "rssi": -64,
      "snr": 97,
      "idle": 40055134,
      "in_network": 78428034,
      "tx_airtime": 97,
      "rx_airtime": 6,
      "airtime": 83,
      "max_rate": 25561625,
      "status": { "wmm": true, "ps": true },
      "capabilities": {
        "wmm": true,
        "apsd": false,
        "shortslot": false,
        "dot11h": true,
        "2040coex": true,
        "psmp": true,
        "proxy_arp": false,
        "dot11v_btm": true,
        "dot11k": {
          "dot11k_link_meas": false,
          "dot11k_nbr_report": false,
          "dot11k_bcn_passive": true,
          "dot11k_bcn_active": false,
          "dot11k_bcn_table": false,
          "dot11k_rcpi": true,
          "dot11k_rsni": true
        },
        "dot11n": {
          "dot11n_ldpc": true,
          "dot11n_40": true,
          "dot11n_ps": false,
          "dot11n_sgi20": false,
          "dot11n_sgi40": false,
          "dot11n_tx_stbc": true,
          "dot11n_rx_stbc": true
        },
        "dot11ac": {
          "dot11ac_160": false,
          "dot11ac_8080": true,
          "dot11ac_mpdu_max": 13691,
          "dot11ac_sgi80": true,
          "dot11ac_sgi160": true,
          "dot11ac_rx_ldpc": false,
          "dot11ac_tx_stbc": true,
          "dot11ac_rx_stbc_1ss": true,
          "dot11ac_rx_stbc_2ss": false,
          "dot11ac_rx_stbc_3ss": true,
          "dot11ac_rx_stbc_4ss": false,
          "dot11ac_su_beamformer": true,
          "dot11ac_su_beamformee": false,
          "dot11ac_mu_beamformer": false,
          "dot11ac_mu_beamformee": true
        }
      },
      "stats": {
        "tx_total_pkts": 79292206,
        "tx_total_bytes": 65238825,
        "tx_failures": 29005909,
        "tx_pkts_retries": 18140633,
        "rx_data_pkts": 12268519,
        "rx_data_bytes": 6257629,
        "rx_failures": 63904908,
        "rate_of_last_tx_pkt": 38503623,
        "rate_of_last_rx_pkt": 89377560
      },
      "rssi_per_antenna": [-61]
    }
  ]
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
ubus call wifi.ap.<name> stats {}
```

### JSONRPC Example

```json
{ "jsonrpc": "2.0", "id": 0, "method": "call", "params": ["<SID>", "wifi.ap.<name>", "stats", {}] }
```

#### output

`output`

- is optional
- type: `object`

##### output Type

`object` with following properties:

| Property                  | Type    | Required     |
| ------------------------- | ------- | ------------ |
| `ack_fail_packets`        | integer | **Required** |
| `aggregate_packets`       | integer | **Required** |
| `rx_broadcast_packets`    | integer | **Required** |
| `rx_bytes`                | integer | **Required** |
| `rx_dropped_packets`      | integer | **Required** |
| `rx_error_packets`        | integer | **Required** |
| `rx_multicast_packets`    | integer | **Required** |
| `rx_packets`              | integer | **Required** |
| `rx_unicast_packets`      | integer | **Required** |
| `rx_unknown_packets`      | integer | **Required** |
| `tx_broadcast_packets`    | integer | **Required** |
| `tx_bytes`                | integer | **Required** |
| `tx_dropped_packets`      | integer | **Required** |
| `tx_error_packets`        | integer | **Required** |
| `tx_multi_retry_packets`  | integer | **Required** |
| `tx_multicast_packets`    | integer | **Required** |
| `tx_packets`              | integer | **Required** |
| `tx_retrans_fail_packets` | integer | **Required** |
| `tx_retrans_packets`      | integer | **Required** |
| `tx_retry_packets`        | integer | **Required** |
| `tx_unicast_packets`      | integer | **Required** |

#### ack_fail_packets

`ack_fail_packets`

- is **required**
- type: reference

##### ack_fail_packets Type

`integer`

- minimum value: `0`

#### aggregate_packets

`aggregate_packets`

- is **required**
- type: reference

##### aggregate_packets Type

`integer`

- minimum value: `0`

#### rx_broadcast_packets

`rx_broadcast_packets`

- is **required**
- type: reference

##### rx_broadcast_packets Type

`integer`

- minimum value: `0`

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

#### rx_multicast_packets

`rx_multicast_packets`

- is **required**
- type: reference

##### rx_multicast_packets Type

`integer`

- minimum value: `0`

#### rx_packets

`rx_packets`

- is **required**
- type: reference

##### rx_packets Type

`integer`

- minimum value: `0`

#### rx_unicast_packets

`rx_unicast_packets`

- is **required**
- type: reference

##### rx_unicast_packets Type

`integer`

- minimum value: `0`

#### rx_unknown_packets

`rx_unknown_packets`

- is **required**
- type: reference

##### rx_unknown_packets Type

`integer`

- minimum value: `0`

#### tx_broadcast_packets

`tx_broadcast_packets`

- is **required**
- type: reference

##### tx_broadcast_packets Type

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

#### tx_multi_retry_packets

`tx_multi_retry_packets`

- is **required**
- type: reference

##### tx_multi_retry_packets Type

`integer`

- minimum value: `0`

#### tx_multicast_packets

`tx_multicast_packets`

- is **required**
- type: reference

##### tx_multicast_packets Type

`integer`

- minimum value: `0`

#### tx_packets

`tx_packets`

- is **required**
- type: reference

##### tx_packets Type

`integer`

- minimum value: `0`

#### tx_retrans_fail_packets

`tx_retrans_fail_packets`

- is **required**
- type: reference

##### tx_retrans_fail_packets Type

`integer`

- minimum value: `0`

#### tx_retrans_packets

`tx_retrans_packets`

- is **required**
- type: reference

##### tx_retrans_packets Type

`integer`

- minimum value: `0`

#### tx_retry_packets

`tx_retry_packets`

- is **required**
- type: reference

##### tx_retry_packets Type

`integer`

- minimum value: `0`

#### tx_unicast_packets

`tx_unicast_packets`

- is **required**
- type: reference

##### tx_unicast_packets Type

`integer`

- minimum value: `0`

### Output Example

```json
{
  "tx_bytes": 66212541,
  "tx_packets": 12861595,
  "tx_unicast_packets": 71397817,
  "tx_multicast_packets": 10764381,
  "tx_broadcast_packets": 46642648,
  "tx_error_packets": 38117040,
  "tx_retrans_packets": 62550551,
  "tx_retrans_fail_packets": 38759943,
  "tx_retry_packets": 50072894,
  "tx_multi_retry_packets": 30414944,
  "tx_dropped_packets": 10287293,
  "ack_fail_packets": 13960170,
  "aggregate_packets": 79525634,
  "rx_bytes": 42443720,
  "rx_packets": 14208946,
  "rx_unicast_packets": 75112790,
  "rx_multicast_packets": 83538769,
  "rx_broadcast_packets": 8905287,
  "rx_error_packets": 51766474,
  "rx_dropped_packets": 39559619,
  "rx_unknown_packets": 41864015
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
ubus call wifi.ap.<name> status {}
```

### JSONRPC Example

```json
{ "jsonrpc": "2.0", "id": 0, "method": "call", "params": ["<SID>", "wifi.ap.<name>", "status", {}] }
```

#### output

`output`

- is optional
- type: `object`

##### output Type

`object` with following properties:

| Property       | Type    | Required     |
| -------------- | ------- | ------------ |
| `adm_capacity` | integer | **Required** |
| `bandwidth`    | oneOf   | **Required** |
| `bssid`        | string  | **Required** |
| `capabilities` | object  | **Required** |
| `encryption`   | string  | **Required** |
| `hidden`       | boolean | **Required** |
| `ifname`       | string  | **Required** |
| `max_stations` | integer | **Required** |
| `num_stations` | integer | **Required** |
| `ssid`         | string  | **Required** |
| `standard`     | string  | **Required** |
| `status`       | string  | **Required** |
| `utilization`  | integer | **Required** |

#### adm_capacity

`adm_capacity`

- is **required**
- type: `integer`

##### adm_capacity Type

`integer`

#### bandwidth

`bandwidth`

- is **required**
- type: complex

##### bandwidth Type

**One** of the following _conditions_ need to be fulfilled.

#### Condition 1

`integer`

#### Condition 2

`integer`

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

#### capabilities

`capabilities`

- is **required**
- type: `object`

##### capabilities Type

`object` with following properties:

| Property    | Type    | Required     |
| ----------- | ------- | ------------ |
| `apsd`      | boolean | **Required** |
| `dot11ac`   | object  | Optional     |
| `dot11h`    | boolean | **Required** |
| `dot11k`    | object  | Optional     |
| `dot11n`    | object  | Optional     |
| `shortslot` | boolean | **Required** |
| `wmm`       | boolean | **Required** |

#### apsd

`apsd`

- is **required**
- type: `boolean`

##### apsd Type

`boolean`

#### dot11ac

`dot11ac`

- is optional
- type: `object`

##### dot11ac Type

`object` with following properties:

| Property                | Type    | Required     |
| ----------------------- | ------- | ------------ |
| `dot11ac_160`           | boolean | **Required** |
| `dot11ac_8080`          | boolean | **Required** |
| `dot11ac_mpdu_max`      | integer | **Required** |
| `dot11ac_mu_beamformee` | boolean | **Required** |
| `dot11ac_mu_beamformer` | boolean | **Required** |
| `dot11ac_rx_ldpc`       | boolean | **Required** |
| `dot11ac_rx_stbc_1ss`   | boolean | **Required** |
| `dot11ac_rx_stbc_2ss`   | boolean | **Required** |
| `dot11ac_rx_stbc_3ss`   | boolean | **Required** |
| `dot11ac_rx_stbc_4ss`   | boolean | **Required** |
| `dot11ac_sgi160`        | boolean | **Required** |
| `dot11ac_sgi80`         | boolean | **Required** |
| `dot11ac_su_beamformee` | boolean | **Required** |
| `dot11ac_su_beamformer` | boolean | **Required** |
| `dot11ac_tx_stbc`       | boolean | **Required** |

#### dot11ac_160

`dot11ac_160`

- is **required**
- type: `boolean`

##### dot11ac_160 Type

`boolean`

#### dot11ac_8080

`dot11ac_8080`

- is **required**
- type: `boolean`

##### dot11ac_8080 Type

`boolean`

#### dot11ac_mpdu_max

`dot11ac_mpdu_max`

- is **required**
- type: `integer`

##### dot11ac_mpdu_max Type

`integer`

- minimum value: `0`
- maximum value: `65535`

#### dot11ac_mu_beamformee

`dot11ac_mu_beamformee`

- is **required**
- type: `boolean`

##### dot11ac_mu_beamformee Type

`boolean`

#### dot11ac_mu_beamformer

`dot11ac_mu_beamformer`

- is **required**
- type: `boolean`

##### dot11ac_mu_beamformer Type

`boolean`

#### dot11ac_rx_ldpc

`dot11ac_rx_ldpc`

- is **required**
- type: `boolean`

##### dot11ac_rx_ldpc Type

`boolean`

#### dot11ac_rx_stbc_1ss

`dot11ac_rx_stbc_1ss`

- is **required**
- type: `boolean`

##### dot11ac_rx_stbc_1ss Type

`boolean`

#### dot11ac_rx_stbc_2ss

`dot11ac_rx_stbc_2ss`

- is **required**
- type: `boolean`

##### dot11ac_rx_stbc_2ss Type

`boolean`

#### dot11ac_rx_stbc_3ss

`dot11ac_rx_stbc_3ss`

- is **required**
- type: `boolean`

##### dot11ac_rx_stbc_3ss Type

`boolean`

#### dot11ac_rx_stbc_4ss

`dot11ac_rx_stbc_4ss`

- is **required**
- type: `boolean`

##### dot11ac_rx_stbc_4ss Type

`boolean`

#### dot11ac_sgi160

`dot11ac_sgi160`

- is **required**
- type: `boolean`

##### dot11ac_sgi160 Type

`boolean`

#### dot11ac_sgi80

`dot11ac_sgi80`

- is **required**
- type: `boolean`

##### dot11ac_sgi80 Type

`boolean`

#### dot11ac_su_beamformee

`dot11ac_su_beamformee`

- is **required**
- type: `boolean`

##### dot11ac_su_beamformee Type

`boolean`

#### dot11ac_su_beamformer

`dot11ac_su_beamformer`

- is **required**
- type: `boolean`

##### dot11ac_su_beamformer Type

`boolean`

#### dot11ac_tx_stbc

`dot11ac_tx_stbc`

- is **required**
- type: `boolean`

##### dot11ac_tx_stbc Type

`boolean`

#### dot11h

`dot11h`

- is **required**
- type: `boolean`

##### dot11h Type

`boolean`

#### dot11k

`dot11k`

- is optional
- type: `object`

##### dot11k Type

`object` with following properties:

| Property             | Type    | Required     |
| -------------------- | ------- | ------------ |
| `dot11k_bcn_active`  | boolean | **Required** |
| `dot11k_bcn_passive` | boolean | **Required** |
| `dot11k_bcn_table`   | boolean | **Required** |
| `dot11k_link_meas`   | boolean | **Required** |
| `dot11k_nbr_report`  | boolean | **Required** |
| `dot11k_rcpi`        | boolean | **Required** |
| `dot11k_rsni`        | boolean | **Required** |

#### dot11k_bcn_active

`dot11k_bcn_active`

- is **required**
- type: `boolean`

##### dot11k_bcn_active Type

`boolean`

#### dot11k_bcn_passive

`dot11k_bcn_passive`

- is **required**
- type: `boolean`

##### dot11k_bcn_passive Type

`boolean`

#### dot11k_bcn_table

`dot11k_bcn_table`

- is **required**
- type: `boolean`

##### dot11k_bcn_table Type

`boolean`

#### dot11k_link_meas

`dot11k_link_meas`

- is **required**
- type: `boolean`

##### dot11k_link_meas Type

`boolean`

#### dot11k_nbr_report

`dot11k_nbr_report`

- is **required**
- type: `boolean`

##### dot11k_nbr_report Type

`boolean`

#### dot11k_rcpi

`dot11k_rcpi`

- is **required**
- type: `boolean`

##### dot11k_rcpi Type

`boolean`

#### dot11k_rsni

`dot11k_rsni`

- is **required**
- type: `boolean`

##### dot11k_rsni Type

`boolean`

#### dot11n

`dot11n`

- is optional
- type: `object`

##### dot11n Type

`object` with following properties:

| Property         | Type    | Required     |
| ---------------- | ------- | ------------ |
| `dot11n_40`      | boolean | **Required** |
| `dot11n_ldpc`    | boolean | **Required** |
| `dot11n_ps`      | boolean | **Required** |
| `dot11n_rx_stbc` | boolean | **Required** |
| `dot11n_sgi20`   | boolean | **Required** |
| `dot11n_sgi40`   | boolean | **Required** |
| `dot11n_tx_stbc` | boolean | **Required** |

#### dot11n_40

`dot11n_40`

- is **required**
- type: `boolean`

##### dot11n_40 Type

`boolean`

#### dot11n_ldpc

`dot11n_ldpc`

- is **required**
- type: `boolean`

##### dot11n_ldpc Type

`boolean`

#### dot11n_ps

`dot11n_ps`

- is **required**
- type: `boolean`

##### dot11n_ps Type

`boolean`

#### dot11n_rx_stbc

`dot11n_rx_stbc`

- is **required**
- type: `boolean`

##### dot11n_rx_stbc Type

`boolean`

#### dot11n_sgi20

`dot11n_sgi20`

- is **required**
- type: `boolean`

##### dot11n_sgi20 Type

`boolean`

#### dot11n_sgi40

`dot11n_sgi40`

- is **required**
- type: `boolean`

##### dot11n_sgi40 Type

`boolean`

#### dot11n_tx_stbc

`dot11n_tx_stbc`

- is **required**
- type: `boolean`

##### dot11n_tx_stbc Type

`boolean`

#### shortslot

`shortslot`

- is **required**
- type: `boolean`

##### shortslot Type

`boolean`

#### wmm

`wmm`

- is **required**
- type: `boolean`

##### wmm Type

`boolean`

#### encryption

`encryption`

- is **required**
- type: `string`

##### encryption Type

`string`

#### hidden

`hidden`

- is **required**
- type: `boolean`

##### hidden Type

`boolean`

#### ifname

##### Interface

`ifname`

- is **required**
- type: reference

##### ifname Type

`string`

- minimum length: 1 characters
- maximum length: 16 characters

#### max_stations

##### Number of stations

`max_stations`

- is **required**
- type: reference

##### max_stations Type

`integer`

- minimum value: `0`
- maximum value: `2007`

#### num_stations

##### Number of stations

`num_stations`

- is **required**
- type: reference

##### num_stations Type

`integer`

- minimum value: `0`
- maximum value: `2007`

#### ssid

##### SSID

`ssid`

- is **required**
- type: reference

##### ssid Type

`string`

- maximum length: 32 characters

#### standard

`standard`

- is **required**
- type: `string`

##### standard Type

`string`

#### status

`status`

- is **required**
- type: `string`

##### status Type

`string`

#### utilization

`utilization`

- is **required**
- type: `integer`

##### utilization Type

`integer`

- minimum value: `0`
- maximum value: `100`

### Output Example

```json
{
  "ifname": "Ut",
  "status": "aute ullamco",
  "ssid": "sed",
  "bssid": "F5:E8:Fe:1F:C5:A4",
  "encryption": "adipisicing id",
  "bandwidth": 40,
  "standard": "commodo aute veniam nulla dolore",
  "num_stations": 1967,
  "utilization": 80,
  "adm_capacity": 37990581,
  "hidden": true,
  "max_stations": 1718,
  "capabilities": {
    "wmm": true,
    "apsd": true,
    "shortslot": true,
    "dot11h": true,
    "dot11k": {
      "dot11k_link_meas": true,
      "dot11k_nbr_report": true,
      "dot11k_bcn_passive": false,
      "dot11k_bcn_active": false,
      "dot11k_bcn_table": false,
      "dot11k_rcpi": false,
      "dot11k_rsni": false
    },
    "dot11n": {
      "dot11n_ldpc": true,
      "dot11n_40": true,
      "dot11n_ps": true,
      "dot11n_sgi20": false,
      "dot11n_sgi40": true,
      "dot11n_tx_stbc": true,
      "dot11n_rx_stbc": false
    },
    "dot11ac": {
      "dot11ac_160": false,
      "dot11ac_8080": true,
      "dot11ac_mpdu_max": 29409,
      "dot11ac_sgi80": true,
      "dot11ac_sgi160": false,
      "dot11ac_rx_ldpc": false,
      "dot11ac_tx_stbc": true,
      "dot11ac_rx_stbc_1ss": true,
      "dot11ac_rx_stbc_2ss": false,
      "dot11ac_rx_stbc_3ss": true,
      "dot11ac_rx_stbc_4ss": false,
      "dot11ac_su_beamformer": false,
      "dot11ac_su_beamformee": false,
      "dot11ac_mu_beamformer": true,
      "dot11ac_mu_beamformee": true
    }
  }
}
```
