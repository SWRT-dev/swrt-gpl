## IMProxy

IMProxy is an IGMP/MLD Proxy daemon written in c for Linux router. 

**Features**:
- Very lightweight and do not depend on any thirdparty library.
- Supports source filtering completely. 
- Implement IGMP/MLD proxy standard (RFC 4605) IGMPv3(rfc 3376) MLDv2(rfc 3810).
- Supports one upstream interface and multiple downstream interfaces.
- Supports group quick leave


It support one upstream and multiple downstream.

### How to Test
You can use VLC Player, or other mcast test tools. I will show you have to test with mcast_test.py in this repository.
On downstream side PC, run mulitcast recevier.
```shell
mcast_test.py -m 224.8.8.8 -r ## Join 224.8.8.8 and start receive packets 
```
On upstream side PC, run mulitcast sender
```shell
mcast_test.py -m 224.8.8.8 -s ## send packet to 224.8.8.8 
```
Check if Recevier start to print time.
### How to Debug
Take sample.cfg as sample and run test above.

IMProxy create Virtual Network Devices when it start, every interface have a unique index,you can find this through '`cat /proc/net/ip_mr_vif`
```shell
/var # cat /proc/net/ip_mr_vif
Interface      BytesIn  PktsIn  BytesOut PktsOut Flags Local    Remote
15 wan1          15804      50         0       0 00000 C0A8164C 00000000
24 eth0.1            0       0     15804      50 00000 C0A81501 00000000
26 eth0.3            0       0         0       0 00000 C0A81301 00000000
27 eth0.4            0       0         0       0 00000 C0A81401 00000000
28 eth0.5            0       0      9264      24 00000 C0A81B01 00000000
```

When IMProxy receive igmp report on downstream interface(eth0.1), improxy will parse it and join related group on upstream interface(wan1)
`cat /proc/net/igmp`
```shell
/var # cat /proc/net/igmp
Idx     Device    : Count Querier       Group    Users Timer    Reporter
...................
15      wan1      :     9      V3
                                E0080808     1 0:00000000               0
                                E00000FB     1 0:00000000               0
                                E00000FC     1 0:00000000               0
                                E0000009     1 0:00000000               0
                                EFFFFFFA     1 0:00000000               0
                                E0000002     1 0:00000000               0
                                E0000016     1 0:00000000               0
                                E0000001     1 0:00000000               0
24      eth0.1    :     7      V3
                                E0000009     1 0:00000000               0
                                E0000016     1 0:00000000               0
                                E0000002     1 0:00000000               0
                                EFFFFFFA     1 0:00000000               0
                                E0000001     1 0:00000000               0
.........................
                                
```

When mulitcast data arrive at upstream interface(wan1), linux kernel send a packet to improxy, this packet contain source address,group address and interface, according to these information,IMProxy will create a Multicast Forwarding Cache,you can find this through `cat /proc/net/ip_mr_cache`
```shell
/var # cat /proc/net/ip_mr_cache
Group    Origin   Iif     Pkts    Bytes    Wrong Oifs
E0080808 C0A80799 15         8     8480        0 24:1           /*froward data from 15(wan1) to 24(eth0.1) */
EFFFFFFA C0A80799 15       100    31608        0 24:1   28:1
EFFFFFFA C0A80199 65535 -2073669404        4 -2073669404
EFFFFFFA C0A81501 65535 -2073667868        4 -2073667868
```

Kernel forward multicast data on the basis of multicast forwarding cache.
