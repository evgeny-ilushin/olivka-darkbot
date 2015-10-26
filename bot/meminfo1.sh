#!/bin/bash

#mem_t=`ssh htpc cat /proc/meminfo | grep MemTotal | sed -e 's/[^ ]* \+//' -e 's/ [a-zA-Z ]\+//'`
#mem_a=`ssh htpc cat /proc/meminfo | grep 'Active\:' | sed -e 's/[^ ]* \+//' -e 's/ [a-zA-Z ]\+//'`
#mem_i=`ssh htpc cat /proc/meminfo | grep 'Inactive\:' | sed -e 's/[^ ]* \+//' -e 's/ [a-zA-Z ]\+//'`
#mem_b=`ssh htpc cat /proc/meminfo | grep 'Buffers\:' | sed -e 's/[^ ]* \+//' -e 's/ [a-zA-Z ]\+//'`
#mem_h=`ssh htpc cat /proc/meminfo | grep 'HighTotal\:' | sed -e 's/[^ ]* \+//' -e 's/ [a-zA-Z ]\+//'`

mem_t=`cat /proc/meminfo | grep MemTotal | sed -e 's/[^ ]* \+//' -e 's/ [a-zA-Z ]\+//'`
mem_a=`cat /proc/meminfo | grep 'Active\:' | sed -e 's/[^ ]* \+//' -e 's/ [a-zA-Z ]\+//'`
mem_i=`cat /proc/meminfo | grep 'Inactive\:' | sed -e 's/[^ ]* \+//' -e 's/ [a-zA-Z ]\+//'`
mem_b=`cat /proc/meminfo | grep 'Buffers\:' | sed -e 's/[^ ]* \+//' -e 's/ [a-zA-Z ]\+//'`
mem_h=`cat /proc/meminfo | grep 'HighTotal\:' | sed -e 's/[^ ]* \+//' -e 's/ [a-zA-Z ]\+//'`

#echo AIBH KB $mem_t\:$mem_a\:$mem_i\:$mem_b\:$mem_f
#echo AIBH MB $(($mem_t/1024))\:$(($mem_a/1024))\:$(($mem_i/1024))\:$(($mem_b/1024))\:$(($mem_h/1024))

mt=$(($mem_a+$mem_i+$mem_b+$mem_h))

scale=10
m1=$(($mem_t+1))
m2=$(($mem_a+1))
m3=$(($mem_i+1))
m4=$(($mem_b+1))
m5=$(($mem_h+1))

#echo $m2 + $m3 + $m4 + $m5 = $mt
#echo $(($m2*$scale/$m1)) + $(($m3*$scale/$m1)) + $(($m4*$scale/$m1)) + $(($m5*$scale/$m1)) = $scale

type_chart()
{
    clr=$1
    nlines=$2
    iCnt=$(($nlines*$scale/$m1+1))
    echo -ne '\x03'
    echo -n $clr,$clr
    while [ $iCnt -ne 0 ]; do
	echo -n l
        iCnt=$(($iCnt-1))
    done
}

# IRC colored
echo -ne '\x02Memory (\x0312Used\x034Kernel\x038Cache\x039Free\x0F\x03): '
echo -ne '\x0312\x02\x02'$(($mem_a/1024))'\x03:\x034\x02\x02'$(($mem_b/1024))'\x03:\x038\x02\x02'$(($mem_h/1024))'\x03:\x039\x02\x02'$(($mem_i/1024))
echo -ne '\x03 ['
type_chart '12' $m2
type_chart '4' $m4
type_chart '8' $m5
type_chart '9' $m3
echo -e '\x03]'


