#!/bin/bash
K="$@"
if [ ! "$K" ]; then
    echo "(_O_)"
else
    K2="$(printf '%q' $K)"
    #echo "K=[$K]"
    #echo "======="
    R=`echo "$K" | base64 -d`

	if [ ! "$R" ]; then
	echo "(_O_)"
	else
	echo "$R"
	fi
fi




