#!/bin/bash

set -euxo pipefail
echo "Functional Tests"
pwd

supervisorctl status all
cp -a gitlab-ci/supervisord-functional.conf /etc/supervisor/conf.d/
supervisorctl update
sleep 3
supervisorctl status all

make functional-test -C ./

supervisorctl stop all
supervisorctl status || true

#report part
#GitLab-CI output
gcovr -r .
# Artefact
gcovr -r . --xml -o ./functional-test-coverage.xml
date +%s > timestamp.log
geninfo . -b . -o ./coverage.info
genhtml coverage.info -o coverage
tar -zcvf coverage.tar.gz coverage
