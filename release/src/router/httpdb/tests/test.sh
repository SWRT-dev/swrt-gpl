#!/bin/sh

IP=127.0.0.1:8000
IPS=127.0.0.1:8443
curl -X POST -d '{"id":3434343, "method":"config_ss.sh", "params":[33,44],"fields":{"ss_server":"1.2.2.3","ss_password":"mypass","ss_port":"4433", "ss_method":"chacha20"}}' http://$IP/_api/
curl -X GET http://$IP/_result/3434343
curl -X GET http://$IP/_api/ss_
curl http://$IP/_root/a.txt
curl http://$IP/a.txt
curl --insecure  https://$IPS/a.txt

