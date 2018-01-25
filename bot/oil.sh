#!/bin/bash

XML="$(timeout 10 wget -q -O - https://www.quandl.com/api/v3/datasets/CHRIS/ICE_B1)"
TX=$(echo -e "$XML" | tr ',' '\n' | grep -A 1 "\"data\":" | tail -n 1)
#echo -n $(echo "scale=2; 10/$X2" | bc)

#echo -e $XML
echo -e $TX
