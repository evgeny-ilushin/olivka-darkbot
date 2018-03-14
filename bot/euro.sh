#!/bin/bash

XML="$(env LC_ALL=C timeout 10 wget -q -O - 'http://www.cbr.ru/scripts/XML_daily.asp' | sed 's/<Value>/\n<Value>/g' | sed 's/<\/Value>/<\/Value>\n/g')"
#XML="$(env LC_ALL=C timeout 10 wget -q -O - 'http://www.cbr.ru/scripts/XML_daily.asp' | sed 's/\>/\>\n/g')"
#XML="$(env LC_ALL=C timeout 10 wget -q -O - 'http://www.cbr.ru/scripts/XML_daily.asp')"
echo "$XML" > '/tmp/111.log'
#X1=$(cat /tmp/111.log | grep -A 3 "EUR")
X1=$(echo "$XML" | grep -A 1 "EUR")
echo "$X1" > /tmp/1
X2=$(echo "$X1" | tail -n 1 | sed 's/Value>//g' | sed 's/<//g' | sed 's/\///g' | tr -d ' ' | tr -d '\t' | tr -d \r | tr -d \n)
X3=$(echo -n "$X2" | tr -d '\n' | tr -d '\r')
echo -n "$X3"

