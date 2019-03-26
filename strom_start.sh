# Strom und Wasser Zeitversetzt (10 Minuten) starten
sleep 300

# Strom starten
nohup /etc/openhab2/c/strom </dev/null >/dev/null 2>&1 & 
/bin/sleep 20
