#!/bin/bash

#STR=`cat /home/zloy/.wmWeatherReports/UUWW.TXT | grep 'Wind\|Visibility\|Sky\ conditions\|Weather\|Temperature'`

#echo $STR

STR=`weather -i UUDD | tail -n +3 | tr -s ' ' | tr '\n' ',' | head -c -1`
echo $STR

