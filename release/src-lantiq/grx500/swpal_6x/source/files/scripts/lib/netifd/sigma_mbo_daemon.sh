#!/bin/sh

. /lib/netifd/sigma-ap.sh

kill_dwpal_cli()
{
        event1="AP-STA-WNM-NOTIF"
        event2="RRM-BEACON-REP-RECEIVED"
        all_event=`ps -w | grep -e $event1 -e $event2 | awk '{print $1}'`
        for i in $all_event
        do
                kill -9 $i
        done
}

is_running=`ps | grep sigma_mbo_d | wc -l`
if [ $is_running -ge 4 ]; then
	debug_print "mbo daemon is already running"
	exit 0
fi

debug_print "--------------------------------------------------- MBO DAEMON STARTED ---------------------------------------------------"

while :;
do
	vaps=`uci show wireless | grep ifname | cut -d"=" -f2 | grep "\." | tr '\r\n' ' ' | tr -d "'" | sed 's/w/-vw/g'`
	ap_tmp=`kill_dwpal_cli`
	event=`run_dwpal_cli_cmd -ihostap -mMain $vaps -dd -l"AP-STA-CONNECTED" -l"RRM-BEACON-REP-RECEIVED" -l"AP-STA-WNM-NOTIF"`
	debug_print "sigma_mbo_handler event received = $event"
	mbo_handler $event
done
