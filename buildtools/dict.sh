#!/bin/sh
dicts=`find release/src/router/www/ -name "*.dict"`
for dict in $dicts
do
sed -i 's/RT-AX53U/RT-AX53U][JCG-Q10PRO][H3C-TX1801/' $dict
done
