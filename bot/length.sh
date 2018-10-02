#!/bin/bash

STR=$1
LOG=/home/olivka/bot/logs/wget.log
NOW=`date`

#wget --spider --tries=1 --timeout=5 $STR

export LC_ALL=ru_RU.cp1251

echo "[$NOW] \"$STR\":" >> $LOG
echo " * wget --spider --tries=1 --timeout=5 $STR 2>&1 | grep 'Length\:\|Длина\:\|ОШИБКА\ \|ERROR\ \|SIZE\ \|не\ существует\|Ошибка' | sed -e 's/[^ ]* \+//'" >> $LOG

#RES=`wget --spider --tries=1 --timeout=5 $STR 2>&1`

#echo " * > [$RES]" >> $LOG
#echo "" >> $LOG

wget --spider --tries=1 --timeout=5 $STR 2>&1 | grep 'Length\:\|Длина\:\|ОШИБКА\ \|ERROR\ \|SIZE\ \|не\ существует\|Ошибка' | sed -e 's/[^ ]* \+//'
#wget 2>&1 --spider --tries=1 --timeout=5 $STR | grep 'Length\:\|дМЙОБ\:\|ОШИБКА\ \|ERROR\ \|SIZE\ ' | sed -e 's/[^ ]* \+//'

#echo $STR

