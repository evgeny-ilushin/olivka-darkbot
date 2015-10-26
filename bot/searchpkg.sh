#!/bin/bash

STR=$1
NOW=`date -R`

echo "Date: $NOW" >> /home/olivka/bot/logs/showpkg.log
echo "Command: [$STR]" >> /home/olivka/bot/logs/showpkg.log
RES=`env LC_ALL=C apt-cache show $STR 2>&1 | grep 'Version\:\ \|Description\:\ \|W\:\ '`
RES2=`echo $RES | tr '\n' ' '`
RES3=`echo $RES2 | wc -c`
echo -e "Result: $RES3 bytes [$RES2]\n" >> /home/olivka/bot/logs/showpkg.log
echo $RES


