# wifi Schema

```
https://www.iopsys.eu/wifi.json
```

| Custom Properties | Additional Properties |
| ----------------- | --------------------- |
| Forbidden         | Forbidden             |

# wifi

| List of Methods   |
| ----------------- |
| [status](#status) | Method | wifi (this schema) |

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
ubus call wifi status {}
```

### JSONRPC Example

```json
{ "jsonrpc": "2.0", "id": 0, "method": "call", "params": ["<SID>", "wifi", "status", {}] }
```

#### output

`output`

- is optional
- type: `object`

##### output Type

`object` with following properties:

| Property | Type  | Required     |
| -------- | ----- | ------------ |
| `radios` | array | **Required** |

#### radios

`radios`

- is **required**
- type: `object[]`

##### radios Type

Array type: `object[]`

All items must be of the type: `object` with following properties:

| Property       | Type    | Required     |
| -------------- | ------- | ------------ |
| `accesspoints` | array   | **Required** |
| `backhauls`    | array   | **Required** |
| `band`         | string  | **Required** |
| `bandwidth`    | integer | **Required** |
| `country`      | string  | **Required** |
| `isup`         | boolean | **Required** |
| `name`         | string  | **Required** |
| `noise`        | integer | **Required** |
| `rate`         | integer | **Required** |
| `standard`     | string  | **Required** |

#### accesspoints

`accesspoints`

- is **required**
- type: reference

##### accesspoints Type

Array type: reference

All items must be of the type: `object` with following properties:

| Property | Type   | Required     |
| -------- | ------ | ------------ |
| `bssid`  |        | **Required** |
| `ifname` | string | **Required** |
| `ssid`   | string | **Required** |
| `status` | string | **Required** |

#### bssid

`bssid`

- is **required**
- type: complex

##### bssid Type

Unknown type ``.

```json
{
  "bssid": {
    "$ref": "#/definitions/macaddr_t"
  },
  "simpletype": "complex"
}
```

#### ifname

`ifname`

- is **required**
- type: `string`

##### ifname Type

`string`

- minimum length: 1 characters
- maximum length: 16 characters

#### ssid

`ssid`

- is **required**
- type: `string`

##### ssid Type

`string`

- maximum length: 32 characters

#### status

`status`

- is **required**
- type: `string`

##### status Type

`string`

#### backhauls

`backhauls`

- is **required**
- type: reference

##### backhauls Type

Array type: reference

All items must be of the type: `object` with following properties:

| Property | Type   | Required     |
| -------- | ------ | ------------ |
| `bssid`  |        | **Required** |
| `ifname` | string | **Required** |
| `ssid`   | string | **Required** |
| `status` | string | **Required** |

#### bssid

`bssid`

- is **required**
- type: complex

##### bssid Type

Unknown type ``.

```json
{
  "bssid": {
    "$ref": "#/definitions/macaddr_t"
  },
  "simpletype": "complex"
}
```

#### ifname

`ifname`

- is **required**
- type: `string`

##### ifname Type

`string`

- minimum length: 1 characters
- maximum length: 16 characters

#### ssid

`ssid`

- is **required**
- type: `string`

##### ssid Type

`string`

- maximum length: 32 characters

#### status

`status`

- is **required**
- type: `string`

##### status Type

`string`

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

#### bandwidth

##### Bandwidth

`bandwidth`

- is **required**
- type: reference

##### bandwidth Type

`integer`

The value of this property **must** be equal to one of the [known values below](#status-known-values).

##### bandwidth Known Values

| Value |
| ----- |
| 20    |
| 40    |
| 80    |
| 160   |

#### country

`country`

- is **required**
- type: `string`

##### country Type

`string`

- minimum length: 2 characters
- maximum length: 3 characters

#### isup

`isup`

- is **required**
- type: `boolean`

##### isup Type

`boolean`

#### name

##### Interface

`name`

- is **required**
- type: reference

##### name Type

`string`

- minimum length: 1 characters
- maximum length: 16 characters

#### noise

##### Noise

`noise`

- is **required**
- type: reference

##### noise Type

`integer`

- minimum value: `-127`
- maximum value: `0`

#### rate

`rate`

- is **required**
- type: `integer`

##### rate Type

`integer`

- minimum value: `0`

#### standard

`standard`

- is **required**
- type: `string`

##### standard Type

`string`

### Output Example

```json
{
  "radios": [
    {
      "name": "voluptate rep",
      "isup": false,
      "standard": "sunt aliqua dolore laboris officia",
      "country": "qui",
      "band": "5GHz",
      "bandwidth": 160,
      "noise": -93,
      "rate": 52420370,
      "accesspoints": [
        { "ifname": "culpa", "status": "esse", "ssid": "Lorem id magna", "bssid": { "bssid": "45:F6:98:d8:E2:dc" } },
        {
          "ifname": "adipisicing",
          "status": "est id et ut irure",
          "ssid": "minim sit mollit",
          "bssid": { "bssid": "E9:eE:06:be:13:65" }
        },
        {
          "ifname": "sit dolo",
          "status": "laboris nulla qui",
          "ssid": "officia commodo",
          "bssid": { "bssid": "2D:F0:4F:EA:0D:eb" }
        },
        {
          "ifname": "nostru",
          "status": "officia dolor ea dolore occaecat",
          "ssid": "qui sit ",
          "bssid": { "bssid": "58:c8:97:d1:d1:9C" }
        },
        { "ifname": "es", "status": "ut sint ipsum Duis ex", "ssid": "ex ", "bssid": { "bssid": "63:Bc:eF:3d:c3:9d" } }
      ],
      "backhauls": [
        {
          "ifname": "labo",
          "status": "nostrud ex amet",
          "ssid": "dolore in eu",
          "bssid": { "bssid": "5e:43:a7:9f:6c:01" }
        },
        {
          "ifname": "id aliquip",
          "status": "esse",
          "ssid": "dolor consectetur",
          "bssid": { "bssid": "ce:FA:92:fe:df:b4" }
        },
        {
          "ifname": "i",
          "status": "dolore mollit commodo",
          "ssid": "minim",
          "bssid": { "bssid": "Cf:aC:0A:Ed:b3:A6" }
        },
        {
          "ifname": "inci",
          "status": "consectetur id pariatur",
          "ssid": "sed",
          "bssid": { "bssid": "d8:aa:D5:23:eC:FD" }
        },
        { "ifname": "nisi", "status": "voluptate fugiat", "ssid": "sunt ", "bssid": { "bssid": "DB:B1:Fe:50:Cb:Eb" } }
      ],
      "channel": 24753590,
      "channels": "ad minim eiusmod"
    },
    {
      "name": "occaecat",
      "isup": false,
      "standard": "labore in dolor ad",
      "country": "ni",
      "band": "2GHz",
      "bandwidth": 80,
      "noise": -106,
      "rate": 30910391,
      "accesspoints": [
        {
          "ifname": "si",
          "status": "aute occaecat laboris ut aliqua",
          "ssid": "incididunt",
          "bssid": { "bssid": "69:68:c5:6c:22:DA" }
        },
        {
          "ifname": "fugiat ",
          "status": "Lorem reprehenderit",
          "ssid": "dolore min",
          "bssid": { "bssid": "88:8f:87:F9:0C:B7" }
        }
      ],
      "backhauls": [
        {
          "ifname": "nisi magna ",
          "status": "officia in sed Lorem",
          "ssid": "sit dolore ",
          "bssid": { "bssid": "9f:F0:aD:3c:BF:af" }
        },
        { "ifname": "laborum", "status": "mollit", "ssid": "fugiat", "bssid": { "bssid": "D0:Ba:C5:83:D8:be" } }
      ],
      "channel": -99539458.64552729,
      "channels": -53586837
    },
    {
      "name": "dolore qu",
      "isup": false,
      "standard": "ut commodo",
      "country": "pr",
      "band": "2GHz",
      "bandwidth": 20,
      "noise": -87,
      "rate": 59230503,
      "accesspoints": [
        {
          "ifname": "cillum d",
          "status": "sit occaecat dolor",
          "ssid": "dolor",
          "bssid": { "bssid": "De:5b:52:b1:ba:5a" }
        },
        { "ifname": "paria", "status": "magna ex", "ssid": "cupidatat ", "bssid": { "bssid": "BA:De:8B:fF:Fc:fC" } }
      ],
      "backhauls": [
        {
          "ifname": "Lorem ut Ut ",
          "status": "ut cupidatat dolor in consequat",
          "ssid": "ad nostrud commodo in in",
          "bssid": { "bssid": "bA:21:C0:F4:d1:E7" }
        }
      ],
      "channel": "dolor do minim",
      "channels": 96257304.49025026
    },
    {
      "name": "in ut",
      "isup": false,
      "standard": "et enim in",
      "country": "vol",
      "band": "5GHz",
      "bandwidth": 80,
      "noise": -90,
      "rate": 66907094,
      "accesspoints": [
        {
          "ifname": "reprehenderi",
          "status": "commodo ex qui nostrud",
          "ssid": "",
          "bssid": { "bssid": "2E:cA:0b:ff:a3:3D" }
        },
        {
          "ifname": "aute Exce",
          "status": "cillum dolor",
          "ssid": "Ut pariatur sun",
          "bssid": { "bssid": "FD:06:B5:9c:Ea:39" }
        },
        {
          "ifname": "officia con",
          "status": "sint ullamco in nulla consequat",
          "ssid": "dolor dol",
          "bssid": { "bssid": "F0:2d:54:D2:b9:FE" }
        }
      ],
      "backhauls": [
        {
          "ifname": "est deserunt",
          "status": "tempor dolor dolor",
          "ssid": "Duis laborum",
          "bssid": { "bssid": "a1:Cb:F8:5f:F0:f4" }
        },
        {
          "ifname": "mo",
          "status": "dolor amet esse veniam",
          "ssid": "cupidatat",
          "bssid": { "bssid": "fa:9F:9a:C4:a2:cF" }
        },
        { "ifname": "d", "status": "", "ssid": "c", "bssid": { "bssid": "bc:88:7b:ab:cD:27" } },
        { "ifname": "consequ", "status": "Duis", "ssid": "cupidatat", "bssid": { "bssid": "3A:FF:A7:BA:9A:dc" } },
        {
          "ifname": "dolo",
          "status": "Lorem quis in aliqua occaecat",
          "ssid": "q",
          "bssid": { "bssid": "8D:6e:f7:fd:8C:e6" }
        }
      ],
      "channel": "do ea sit labore est",
      "channels": -76131712.30140434
    }
  ]
}
```
