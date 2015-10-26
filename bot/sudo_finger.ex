#!/usr/bin/expect
set CMD $env(EXP_PARAM1)
spawn su -c "nmap -O -p 80 -P0 $CMD"
expect assword {send kindaWierda549yf\[\r}
expect eof
