#!/bin/sh

MSDELAY=10

# Input params
#NUMBER='+79101639015'
NUMBER=$1

errormsg ()
{
    echo "$*"
    exit 1
}

# Empry
if [ -z ${NUMBER} ]; then
    errormsg 'Invalid phone number (1)'
fi

# Remove "+" if any
while [ ${NUMBER:0:1} == "+" ]; do
    NUMBER=${NUMBER:1}
done

# Remove 7/8 prefix
if [ ${#NUMBER} -eq 11 ]; then
    NUMBER=${NUMBER:1}
fi

# Accept 10 digits only
if [ ${#NUMBER} -ne 10 ]; then
    errormsg 'Invalid phone number (2)'
fi

NUMBER_ND=`echo $NUMBER | sed 's/^[+-]//;s/[0-9]//g;s/\.//'`
#echo "[$NUMBER_ND] = ${#NUMBER_ND}"

# Accept numbers only
#if [ -n `echo $NUMBER | sed 's/^[+-]//;s/[0-9]//g;s/\.//'` ]; then
if [ ${#NUMBER_ND} -ne 0 ]; then
    errormsg 'Invalid phone number (3)'
fi

# Strip to DEF/CID
DEF=${NUMBER:0:3}
CID=${NUMBER:3}

#Check input

URL="http://mtt.ru/info/def/index.wbp?def=${DEF}&number=${CID}&region=&standard=&date=&operator="
#echo $URL

# Log
echo $NUMBER $URL >> def_info.sh.log

# Query DEF db
RETLINES=20
cnt=0

wget -O - -t 1 -T 5 -q $URL |

while read line
do
if [ "${line:11:22}" = "searchCodeResultsTable" ]; then
    # echo "${line}" 
    cnt=$((cnt+1))
fi
if [ $cnt -ne 0 -a $cnt -le $RETLINES ]; then
    echo "${line}"
    cnt=$((cnt+1))
fi
done
