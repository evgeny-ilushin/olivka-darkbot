#!/bin/bash

uname_s=`uname -rv`
uptime_s=`uptime | sed -e 's/ *[0-9\:]* //'`

echo $uname_s \($uptime_s\)
