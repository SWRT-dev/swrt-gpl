#!/bin/bash

set -e
echo "Unit Tests"
pwd

make unit-test -C ./

#report part
#GitLab-CI output
gcovr -r .
# Artefact
gcovr -r . --xml -o ./unit-test-coverage.xml
date +%s > timestamp.log
geninfo . -b . -o ./coverage.info
genhtml coverage.info -o coverage
tar -zcvf coverage.tar.gz coverage
