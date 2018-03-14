#!/bin/sh

JSON=$(wget -q -O - https://api.coindesk.com/v1/bpi/currentprice.json)
RT=$(echo $JSON | python3 -c "import sys, json; print(json.load(sys.stdin)['bpi']['USD']['rate'])")

echo "$RT американських доларів"
echo "$RT американських доларів" >> btc.sh.log

