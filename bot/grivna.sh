#!/bin/bash

XML="$(timeout 10 wget -q -O - http://bank-ua.com/export/currrate.xml)"
X1=$(echo "$XML" | grep -A 3 "RUB")
X2=$(echo "$X1" | tail -n 1 | sed 's/rate>/\ /g' | sed 's/<//g' | sed 's/\///g' | tr -d ' ')

#echo -n "$X2"
echo -n $(echo "scale=2; 10/$X2" | bc)
#echo -n $(echo "scale=2; 10/$X2" | bc) "(не забудь запитати про ковток :)"
