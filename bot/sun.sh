#!/bin/bash

JSON=$(curl -q "https://api.sunrise-sunset.org/json?lat=54.1809363&lng=-7.02109")

SR=$(echo $JSON | cut -d ',' -f 1 | cut -d \" -f 6)
SS=$(echo $JSON | cut -d ',' -f 2 | cut -d \" -f 4)

echo "Sunrise: $SR, sunset: $SS"

