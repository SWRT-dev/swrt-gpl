#!/bin/bash
set -euxo pipefail
echo "Functional API Tests"
pwd

make coverage -C ./

supervisorctl status all
cp -a gitlab-ci/supervisord-functional-api.conf /etc/supervisor/conf.d/
supervisorctl reload

i=0
while test $i -le 100
do
	if supervisorctl status wifimngr | grep RUNNING; then
		break
	fi
	echo "."
	sleep 2
	i=$((i+1))
done
supervisorctl status all

# run API validation
ubus-api-validator -d ./test/api/json/ > ./api-result.log

supervisorctl stop all
supervisorctl status || true

#report part
gcovr -r . --xml -o ./api-test-coverage.xml
gcovr -r .
tap-junit --input ./api-result.log --output report
date +%s > timestamp.log
geninfo . -b . -o ./coverage.info
genhtml coverage.info -o coverage
tar -zcvf coverage.tar.gz coverage test/memory-report.xml supervisord.log wifimngr.log /tmp/test.log
