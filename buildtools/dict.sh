#!/bin/sh
dicts=`find release/src/router/www/ -name "*.dict"`
for dict in $dicts
do
sed -i 's/RT-AX53U/RT-AX53U][PGB-M1][JCG-Q10PRO][JCG-Q20][H3C-TX1801][CMCC-A9][XM-CR660X/' $dict
#sed -i 's/H3C-TX1801/H3C-TX1801][XM-CR660X/' $dict
sed -i 's/RT-AC3200]/RT-AC3200][SBRAC3200P]/' $dict
done
