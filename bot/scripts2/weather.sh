#!/bin/bash

#STR=`cat /home/zloy/.wmWeatherReports/UUWW.TXT | grep 'Wind\|Visibility\|Sky\ conditions\|Weather\|Temperature'`

#echo $STR

#STR=`weather -i UUDD | tail -n +3 | tr -s ' ' | tr '\n' ',' | head -c -1`
#echo $STR

#./weather.py

function E {
    local T=`cat tula.sh | grep -a "$@" | cut -d '=' -f 2 | sed 's/[^[:print:]]//' | head -n 1`
#    echo -e "[$@] -> [$T]" > /dev/stderr
    echo -e "$T"
}

#wget "https://export.yandex.ru/bar/reginfo.xml?region=%D0%A5%D0%A5%D0" -O - | xml2 | tr '/, @-' '_____' | iconv -t "windows-1251" -f "UTF-8" > tula.sh
#wget --proxy=off --no-proxy "https://export.yandex.ru/bar/reginfo.xml?region=%D0%A5%D0%A5%D0" -O - | xml2 | iconv -t "windows-1251" -f "UTF-8" > tula.sh
curl -q "https://export.yandex.ru/bar/reginfo.xml?region=%D0%A5%D0%A5%D0" | xml2 | iconv -t "windows-1251" -f "UTF-8" > tula.sh
#C="$(curl -q 'https://w1.weather.gov/xml/current_obs/KMDW.rss' | xml2 | grep -E '/rss/channel/item/title=[A-Za-z0-9].' | cut -d '=' -f 2 | head -n 1)"

#exit

COND=$(E "/info/weather/day/day_part/weather_type")
TEMP=$(E "/info/weather/day/day_part/temperature=")
TOM=$(E "/info/weather/day/tomorrow/temperature=")
#echo $TOM
#echo ^^^^
TS=$(E "/info/weather/day/day_part/observation_time=")

HUM=$(E "/info/weather/day/day_part/dampness=")
PRE=$(E "/info/weather/day/day_part/pressure=")
WSP=$(E "/info/weather/day/day_part/wind_speed=")
WDR=$(E "/info/weather/day/day_part/wind_direction=")

#=$(E "")
#C="$(wget -q --no-proxy 'https://w1.weather.gov/xml/current_obs/KMDW.rss' -O - | xml2 | grep -E '/rss/channel/item/title=[A-Za-z0-9].' | cut -d '=' -f 2 | head -n 1)"

echo -e "$TS: $COND, $TEMP""C. Давление $PRE мм рт.ст., влажность $HUM""%, ветер $WSP м/с ($WDR). Завтра $TOM""C."
#echo -e "$TS: " - $TOM - ""C.
# $C


#E "/info/weather/day/day_part/temperature"




