# wifi.wps Schema

```
https://www.iopsys.eu/wifi.wps
```

| Custom Properties | Additional Properties |
| ----------------- | --------------------- |
| Forbidden         | Forbidden             |

# wifi.wps

| List of Methods               |
| ----------------------------- |
| [generate_pin](#generate_pin) | Method | wifi.wps (this schema) |
| [setpin](#setpin)             | Method | wifi.wps (this schema) |
| [showpin](#showpin)           | Method | wifi.wps (this schema) |
| [start](#start)               | Method | wifi.wps (this schema) |
| [status](#status)             | Method | wifi.wps (this schema) |
| [stop](#stop)                 | Method | wifi.wps (this schema) |
| [validate_pin](#validate_pin) | Method | wifi.wps (this schema) |

## generate_pin

`generate_pin`

- type: `Method`

### generate_pin Type

`object` with following properties:

| Property | Type   | Required     |
| -------- | ------ | ------------ |
| `input`  | object | Optional     |
| `output` | object | **Required** |

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
ubus call wifi.wps generate_pin {}
```

### JSONRPC Example

```json
{ "jsonrpc": "2.0", "id": 0, "method": "call", "params": ["<SID>", "wifi.wps", "generate_pin", {}] }
```

#### output

`output`

- is **required**
- type: `object`

##### output Type

`object` with following properties:

| Property | Type   | Required     |
| -------- | ------ | ------------ |
| `pin`    | string | **Required** |

#### pin

`pin`

- is **required**
- type: reference

##### pin Type

`string`

- minimum length: 8 characters
- maximum length: 8 characters

### Output Example

```json
{ "pin": "deserunt" }
```

## setpin

`setpin`

- type: `Method`

### setpin Type

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
| `pin`    | string | **Required** |
| `vif`    | string | Optional     |

#### pin

`pin`

- is **required**
- type: reference

##### pin Type

`string`

- minimum length: 8 characters
- maximum length: 8 characters

#### vif

##### Interface

`vif`

- is optional
- type: reference

##### vif Type

`string`

- minimum length: 1 characters
- maximum length: 16 characters

### Ubus CLI Example

```
ubus call wifi.wps setpin {"pin":"adelit e","vif":"in magn"}
```

### JSONRPC Example

```json
{
  "jsonrpc": "2.0",
  "id": 0,
  "method": "call",
  "params": ["<SID>", "wifi.wps", "setpin", { "pin": "adelit e", "vif": "in magn" }]
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

## showpin

`showpin`

- type: `Method`

### showpin Type

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
| `vif`    | string | Optional |

#### vif

##### Interface

`vif`

- is optional
- type: reference

##### vif Type

`string`

- minimum length: 1 characters
- maximum length: 16 characters

### Ubus CLI Example

```
ubus call wifi.wps showpin {"vif":"esse qu"}
```

### JSONRPC Example

```json
{ "jsonrpc": "2.0", "id": 0, "method": "call", "params": ["<SID>", "wifi.wps", "showpin", { "vif": "esse qu" }] }
```

#### output

`output`

- is optional
- type: `object`

##### output Type

`object` with following properties:

| Property | Type   | Required     |
| -------- | ------ | ------------ |
| `pin`    | string | **Required** |

#### pin

`pin`

- is **required**
- type: reference

##### pin Type

`string`

- minimum length: 8 characters
- maximum length: 8 characters

### Output Example

```json
{ "pin": "Duis qui" }
```

## start

`start`

- type: `Method`

### start Type

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
| `mode`   | string | Optional |
| `pin`    | string | Optional |
| `role`   | string | Optional |
| `vif`    | string | Optional |

#### mode

`mode`

- is optional
- type: `string`

##### mode Type

`string`

#### pin

`pin`

- is optional
- type: reference

##### pin Type

`string`

- minimum length: 8 characters
- maximum length: 8 characters

#### role

`role`

- is optional
- type: `enum`

##### role Type

`string`

The value of this property **must** be equal to one of the [known values below](#start-known-values).

##### role Known Values

| Value     |
| --------- |
| registrar |
| enrollee  |

#### vif

##### Interface

`vif`

- is optional
- type: reference

##### vif Type

`string`

- minimum length: 1 characters
- maximum length: 16 characters

### Ubus CLI Example

```
ubus call wifi.wps start {"vif":"ex commo","mode":"culpa","role":"enrollee","pin":"anim qui"}
```

### JSONRPC Example

```json
{
  "jsonrpc": "2.0",
  "id": 0,
  "method": "call",
  "params": [
    "<SID>",
    "wifi.wps",
    "start",
    { "vif": "ex commo", "mode": "culpa", "role": "enrollee", "pin": "anim qui" }
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

| Property | Type   | Required |
| -------- | ------ | -------- |
| `vif`    | string | Optional |

#### vif

##### Interface

`vif`

- is optional
- type: reference

##### vif Type

`string`

- minimum length: 1 characters
- maximum length: 16 characters

### Ubus CLI Example

```
ubus call wifi.wps status {"vif":"laboru"}
```

### JSONRPC Example

```json
{ "jsonrpc": "2.0", "id": 0, "method": "call", "params": ["<SID>", "wifi.wps", "status", { "vif": "laboru" }] }
```

#### output

`output`

- is optional
- type: `object`

##### output Type

`object` with following properties:

| Property | Type    | Required     |
| -------- | ------- | ------------ |
| `code`   | integer | **Required** |
| `status` | string  | **Required** |

#### code

`code`

- is **required**
- type: `integer`

##### code Type

`integer`

- minimum value: `0`

#### status

`status`

- is **required**
- type: `string`

##### status Type

`string`

### Output Example

```json
{ "code": 74811734, "status": "nisi Excepteur irure laboris ad" }
```

## stop

`stop`

- type: `Method`

### stop Type

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
ubus call wifi.wps stop {}
```

### JSONRPC Example

```json
{ "jsonrpc": "2.0", "id": 0, "method": "call", "params": ["<SID>", "wifi.wps", "stop", {}] }
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

## validate_pin

`validate_pin`

- type: `Method`

### validate_pin Type

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
| `pin`    | string | **Required** |

#### pin

`pin`

- is **required**
- type: reference

##### pin Type

`string`

- minimum length: 8 characters
- maximum length: 8 characters

### Ubus CLI Example

```
ubus call wifi.wps validate_pin {"pin":"ipsum am"}
```

### JSONRPC Example

```json
{ "jsonrpc": "2.0", "id": 0, "method": "call", "params": ["<SID>", "wifi.wps", "validate_pin", { "pin": "ipsum am" }] }
```

#### output

`output`

- is optional
- type: `object`

##### output Type

`object` with following properties:

| Property | Type    | Required     |
| -------- | ------- | ------------ |
| `valid`  | boolean | **Required** |

#### valid

`valid`

- is **required**
- type: `boolean`

##### valid Type

`boolean`

### Output Example

```json
{ "valid": true }
```
