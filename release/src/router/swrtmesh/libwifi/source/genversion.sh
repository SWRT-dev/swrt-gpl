#!/bin/sh

maj=$(cat ./VERSION.txt | cut -f1 -d.)
min=9999
rev=9999

min=$(git rev-list --count $(git log --follow -1 --pretty=%H VERSION.txt)..HEAD)
rev=$(git rev-parse --short=8 HEAD)

echo "$maj $min $rev"
