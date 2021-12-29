#!/bin/sh 

COUNTER=0

V="your are the best"
K="hello"
while [  $COUNTER -lt 50000 ]; do
    ../build/bin/dbus set $K$COUNTER=$V
    ../build/bin/dbus remove $K$COUNTER
    #sleep 1
    echo $K$COUNTER
    let COUNTER=COUNTER+1 
done
