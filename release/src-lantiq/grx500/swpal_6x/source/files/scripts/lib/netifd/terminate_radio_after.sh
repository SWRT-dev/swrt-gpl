#!/bin/sh
if [[ "$1" != "" && "$2" != "" && "$3" != "" ]]; then
        down_time_min=$3
        down_time_sec=$((down_time_min*60))
        sleep $2
        iw $1 iwlwav sEnableRadio 0
        sleep $down_time_sec
        iw $1 iwlwav sEnableRadio 1
else
        echo "ERROR: Bad input: wlan=$1; down after=$2 seconds; for=$3 mnutes"
fi
