#!/bin/bash

PORT=$2
HOST=$1

AN="$HOST"
AN1="$HOST:$PORT"

if [ "$HOST" == '' ]; then
    echo $AN missing host name
    exit 1
fi

if [ "$PORT" == '' ]; then
    echo $AN: missing destination port
    exit 1
fi
                
NMAP=`nmap -P0 --max-retries 2 --host-timeout 10s --reason -p $PORT $HOST 2>/dev/null | grep $PORT | grep -e 'open\|closed'`

if [ "$NMAP" == '' ]; then
    echo $AN1 destination unreachable
    exit 1
fi

echo $AN:$NMAP