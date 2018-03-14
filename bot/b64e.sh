#!/bin/bash
K="$@"
if [ ! "$K" ]; then
    echo "(_O_)"
else
    K2="$(printf '%q' $K)"
    #echo "K=[$K]"
    #echo "======="
    echo "$K" | base64
fi




