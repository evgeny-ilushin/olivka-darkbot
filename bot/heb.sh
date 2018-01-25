#!/bin/bash

STR=$1
NOW=`date -R`

hebcal $(date +"%m %d %Y") | cut -d ' ' -f 2-
