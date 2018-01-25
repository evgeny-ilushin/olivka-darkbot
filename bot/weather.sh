#!/bin/bash

#STR=`cat /home/zloy/.wmWeatherReports/UUWW.TXT | grep 'Wind\|Visibility\|Sky\ conditions\|Weather\|Temperature'`

#echo $STR


#STR=`weather -i UUDD | tail -n +3 | tr -s ' ' | tr '\n' ',' | head -c -1`
#echo $STR


#curl http://wttr.in/tula | head -n 7

URL='http://www.accuweather.com/en/de/berlin/10178/weather-forecast/295982'

wget -q -O- "$URL" | awk -F\' '/acm_RecentLocationsCarousel\.push/{print $12" C, " }'| head -1


