#!/bin/sh

#we will stop all plugins when upgrading

ACTION=$1

if [ $# -lt 1 ]; then
    printf "Usage: $0 {start|stop|restart|reconfigure|check|kill}\n" >&2
    exit 1
fi

[ $ACTION = stop -o $ACTION = restart -o $ACTION = kill ] && ORDER="-r"

for i in $(find /jffs/softcenter/init.d/ -name '*.sh' | sort $ORDER ) ;
do
    case "$i" in
        *.sh )
            # Source shell script for speed.
            trap "" INT QUIT TSTP EXIT
            #set $1
            #echo "trying $i" >> /tmp/rc.log
            if [ -r "$i" ]; then
            . $i $ACTION
            fi
            ;;
        *)
            # No sh extension, so fork subprocess.
            $i $ACTION
            ;;
    esac
done
