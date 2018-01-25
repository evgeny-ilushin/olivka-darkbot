#!/bin/bash

STR=$@

#if [ "$STR" == "" ]; then
#    exit 0
#fi

TOKEN=294103149:AAGPawepBdjAtu9z9aKDj2rLwwdNt0UDi9E
METHOD_NAME=sendMessage
#URL="https://api.telegram.org/bot$TOKEN/$METHOD_NAME?parse_mode=Markdown"
URL="https://api.telegram.org/bot$TOKEN/$METHOD_NAME?parse_mode=HTML"
#CHAT="-203638944"
CHAT="-1001082390874"
TEXT="$(echo $STR | iconv -f \"windows-1251\" -t \"UTF-8\")"

echo "$(date) $STR" >> /tmp/tgwebhook.log
#echo "$(date) $STR" >> /home/olivka/bot/logs/tgecho-io.log
#curl -X POST --data-urlencode "chat_id=$CHAT" --data-urlencode "text=$TEXT" "$URL"  >> /home/olivka/bot/logs/tgecho-io.log
#echo "" >> /home/olivka/bot/logs/tgecho-io.log
#echo $TEXT

