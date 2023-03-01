#!/bin/sh
sed -i 's/Arial, MS UI Gothic, MS P Gothic, Microsoft Yahei UI/Microsoft Yahei UI, Arial, MS UI Gothic, MS P Gothic/' `grep -rl "Arial, MS UI Gothic, MS P Gothic, Microsoft Yahei UI" release/src/router/www/`
sed -i 's/Arial, Verdana, Helvetica, MS UI Gothic, MS P Gothic, Microsoft Yahei UI/Microsoft Yahei UI, Arial, Verdana, Helvetica, MS UI Gothic, MS P Gothic/'  `grep -rl "Arial, Verdana, Helvetica, MS UI Gothic, MS P Gothic, Microsoft Yahei UI" release/src/router/www/`
sed -i 's/Verdana, MS UI Gothic, MS P Gothic, Microsoft Yahei UI/Microsoft Yahei UI, Verdana, MS UI Gothic, MS P Gothic/' `grep -rl "Verdana, MS UI Gothic, MS P Gothic, Microsoft Yahei UI" release/src/router/www/`
sed -i 's/Arial, Helvetica, MS UI Gothic, MS P Gothic, Microsoft Yahei UI/Microsoft Yahei UI, Arial, Helvetica, MS UI Gothic, MS P Gothic/' `grep -rl "Arial, Helvetica, MS UI Gothic, MS P Gothic, Microsoft Yahei UI" release/src/router/www/`
sed -i 's/Courier, MS UI Gothic, MS P Gothic, Microsoft Yahei UI/Courier, Microsoft Yahei UI, MS UI Gothic, MS P Gothic/' `grep -rl "Courier, MS UI Gothic, MS P Gothic, Microsoft Yahei UI" release/src/router/www/`
sed -i 's/MS UI Gothic, MS P Gothic, Microsoft Yahei UI/Microsoft Yahei UI, MS UI Gothic, MS P Gothic/' `grep -rl "MS UI Gothic, MS P Gothic, Microsoft Yahei UI" release/src/router/www/`



