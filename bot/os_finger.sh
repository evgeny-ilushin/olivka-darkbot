#!/bin/bash

export EXP_PARAM1=$1

if [ "$EXP_PARAM1" == '' ]; then
    echo missing destination address
    exit 1
fi

EXP=`/home/olivka/bot/sudo_finger.ex | grep -e 'No\ OS\ matches\ for\ host\|Aggressive\ OS\ guesses\|OS\ details\:\|Uptime\:' | tr -s ' ' | tr '\n' ' ' | tr '\r' ' '`
#echo \r\n

#EXP=`./sudo_finger.ex`


if [ "$EXP" == '' ]; then
    echo unknown destination
    exit 1
fi


echo $EXP 

