#!/bin/bash

cat mcclookup-mini.csv | while read l; do
A=$(echo $l | cut -d '|' -f 1);
B=$(echo $l | cut -d '|' -f 2);

echo -e "INSERT INTO carrier_list (code, name) VALUES ('$A', '$B');" >> mcclookup-mini-1.sql

done




