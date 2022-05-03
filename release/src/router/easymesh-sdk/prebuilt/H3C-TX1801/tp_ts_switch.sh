#!/bin/sh

echo "param1  90--> iptv vlan=90; only one valid vlan id; 1~4094"
echo "param2  20,30--> ts secondary vlan id combination; can be one or more, ',' is separator; 3~4094"
echo "param3  2-0,3-1--> the port(2) directly connected by iptv receiver;iptv receiver untag,the port(3) directly connected by iptv receiver;iptv receiver tag"
echo "param4  0,1--> the port directly connected by downstream MAP dev; can be one or more, ',' is separator 0~3"
echo "param3 and param4 can't have the same member"
echo "only set param4 when its downstream MAP dev has iptv receiver"

iptv_vlan="$1"
let iptv_vlan_10=$iptv_vlan
iptv_vlan_hex=$(printf %x $iptv_vlan_10)
ts_vlan="$2"
d_iptv_port_and_tag="$3"
d_map_dp="$4"

echo "param1 iptv_vlan=$iptv_vlan"
echo "param2 ts_vlan=$ts_vlan"
echo "param3 d_iptv_port_and_tag=$d_iptv_port_and_tag"
echo "param4 d_map_dp=$d_map_dp"

ifs_old=$IFS
echo "ifs_old=$ifs_old"
IFS=$','

for p_t in $d_iptv_port_and_tag
do
	if [ -z $d_iptv_port ]; then
		d_iptv_port=${p_t:0:1}
		if [ ${p_t:2:1} = '1' ]; then
			d_iptv_tag=${p_t:0:1}
		fi
	else
		d_iptv_port=$d_iptv_port,${p_t:0:1}
		if [ ${p_t:2:1} = '1' ]; then
			if [ -z $d_iptv_tag ]; then
				d_iptv_tag=${p_t:0:1}
			else
				d_iptv_tag=$d_iptv_tag,${p_t:0:1}
			fi 	
		fi
	fi 	
done

fstr='2'
lstr='10'
for p in $d_iptv_port
do
port=$fstr$p$lstr
echo "set user mode on port$p"
switch reg w $port 81000000
echo "set pvid $iptv_vlan on port$p"
switch vlan pvid $p $iptv_vlan
done
echo "set user mode on wan port4"
switch reg w 2410 81000000

port_all=0
tag_port=0

if [ "$d_map_dp" = "x" ]; then
	port_all=$d_iptv_port,4
	tag_port=4
else
	port_all=$d_iptv_port,4,$d_map_dp
	tag_port=$d_map_dp,4
fi
echo "############port_all=$port_all"
echo "############tag_port=$tag_port"

for t in $d_iptv_tag
do
	tag_port=$tag_port,$t	
done
echo "############tag_port=$tag_port"
for p in $port_all
do
value=$((1<<$p))
let "port_sum = $port_sum + $value"
done
echo "port_sum=$port_sum"

for p in $tag_port
do
value=$((1<<$p))
let "tag_sum = $tag_sum + $value"
done
echo "tag_sum=$tag_sum"

switch vlan vid 0 1 90 $port_sum $tag_sum 1 0 0

#ACL enable
switch acl enable 4 1

#ACL vlan 20: P4--> no action:
switch reg w 0x94 ffff8100
switch reg w 0x98 8100C   
switch reg w 0x90 8000500a

switch reg w 0x94 ffff00$iptv_vlan_hex
switch reg w 0x98 8100E   
switch reg w 0x90 8000500b                                                                                                 

switch reg w 0x94 c00
switch reg w 0x98 0
switch reg w 0x90 8000900a

switch reg w 0x94 19000000
switch reg w 0x98 0
switch reg w 0x90 8000b00a

#ACL match tpid 8100 : P4-->P5:(ingress tag --> egress tagged)
switch reg w 0x94 ffff8100
switch reg w 0x98 8100C   
switch reg w 0x90 8000500c                                                                                               

switch reg w 0x94 1000
switch reg w 0x98 0
switch reg w 0x90 8000900b

switch reg w 0x94 1A0E2080
switch reg w 0x98 0
switch reg w 0x90 8000b00b

#ACL match vlan 90 : P0-->P4:(ingress tag --> egress tagged)
if [ "$d_map_dp" != "x" ]; then
for p in $d_map_dp
do
value=$((1<<$p))
let "port_bitmap = $port_bitmap + $value"
done
echo "port_bitmap=$port_bitmap"
fi

fstr='80'
lstr='0C'
switch reg w 0x94 ffff8100 
#Pn ingress bitmap:8-15 
switch reg w 0x98 $fstr$port_bitmap$lstr
switch reg w 0x90 8000500d 

lstr='0E'
switch reg w 0x94 ffff00$iptv_vlan_hex
#Pn ingress bitmap:8-15 
switch reg w 0x98 $fstr$port_bitmap$lstr
switch reg w 0x90 8000500e 

switch reg w 0x94 6000 
switch reg w 0x98 0 
switch reg w 0x90 8000900c 

#P4 egress bitmap:8-15
switch reg w 0x94 1B081080 
switch reg w 0x98 0
switch reg w 0x90 8000b00c 

ramips_board_name() {
	local name

	[ -f /tmp/sysinfo/board_name ] && name=$(cat /tmp/sysinfo/board_name)
	[ -z "$name" ] && name="unknown"

	echo "${name%-[0-9]*M}"
}

board=$(ramips_board_name)
platform=${board:0:6}

if [ "$platform" == "mt7621" ]; then
for p in $ts_vlan
do
switch vlan set 0 $p 11111110
done
elif [ "$platform" == "mt7622" ]; then
for p in $ts_vlan
do
switch vlan set 0 $p 11111110
done
else
echo "platform=$platform"
fi

IFS=$ifs_old
