# 2.6.18 #2 SMP Sat Jan 20 16:56:37 MSK 2007 (up 15 days, 8:26, 3 users, load average: 0.42, 0.43, 0.44)

#!/bin/bash

uname_s=`uname -rv`
uptime_s=`uptime | sed -e 's/ *[0-9\:]* //'`

echo $uname_s \($uptime_s\)
