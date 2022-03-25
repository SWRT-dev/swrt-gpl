#!/bin/sh

wif2=`nvram get wl0_ifname`
wif5=`nvram get wl1_ifname`
start() {   
	ralinkiappd -wi $wif5 -wi $wif2 &
	echo 10000 > /proc/sys/net/ipv4/neigh/$wif5/base_reachable_time_ms
	echo 1 > /proc/sys/net/ipv4/neigh/$wif5/delay_first_probe_time
	echo 10000 > /proc/sys/net/ipv4/neigh/$wif2/base_reachable_time_ms
	echo 1 > /proc/sys/net/ipv4/neigh/$wif2/delay_first_probe_time
	echo 10000 > /proc/sys/net/ipv4/neigh/br0/base_reachable_time_ms
	echo 1 > /proc/sys/net/ipv4/neigh/br0/delay_first_probe_time
	echo 10000 > /proc/sys/net/ipv4/neigh/eth0/base_reachable_time_ms
	echo 1 > /proc/sys/net/ipv4/neigh/eth0/delay_first_probe_time
    iptables -A INPUT -i br0 -p tcp --dport 3517 -j ACCEPT
    iptables -A INPUT -i br0 -p udp --dport 3517 -j ACCEPT 
}

stop() {
    pid=`pidof ralinkiappd`
    if [ "$pid" != "" ]; then
        killall -q  ralinkiappd
		sleep 1
		killall -q  ralinkiappd
		sleep 1
    fi
    
}


case "$1" in
        start)
            start
            ;;

        stop)
            stop
            ;;

        restart)
            stop
            start
            ;;

        *)
            echo $"Usage: $0 {start|stop|restart}"
esac
