#!/bin/bash

export LC_ALL=ru_RU.cp1251

#echo "12312312312312312312"

/home/olivka/bot/scripts/mlookup-bin.sh $@ 2>/dev/null | tee -a /tmp/lookup.log

