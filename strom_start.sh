#!/bin/sh
# strom_start.sh
# --------------
# Startet das Stromzaehler-Programm zeitversetzt im Hintergrund.
# Gedacht als Autostart-Skript (z. B. ueber openHAB / Systemstart).

# Zeitversetzt starten (z. B. damit andere Dienste zuerst hochfahren)
sleep 300

# Programm im Hintergrund starten (losgeloest vom Terminal, Ein-/Ausgabe verworfen)
nohup /etc/openhab2/c/strom </dev/null >/dev/null 2>&1 & 
/bin/sleep 20
