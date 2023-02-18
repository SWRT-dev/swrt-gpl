#!/bin/sh

IP=127.0.0.1:8000
sleep 30
curl -X POST -d 'post-end' http://$IP/_resp/$1
