# Stromzähler (SML / EDL21 → openHAB)

Kleines C++-Programm für Linux, das einen Stromzähler über eine serielle
USB-Schnittstelle ausliest und die Messwerte an einen [openHAB](https://www.openhab.org/)-Server
überträgt.

Der Zähler sendet seine Daten im **SML-Format (Smart Message Language, EDL21)**.
Das Programm liest die SML-Telegramme ein, filtert die relevanten Werte heraus
und schickt sie per `curl` an die openHAB-REST-API.

## Was das Programm macht

In einer Endlosschleife werden folgende Schritte durchlaufen:

1. **Serielle Verbindung öffnen** – zum Gerät `/dev/strom` (9600 Baud, 8N1, keine Flusssteuerung).
2. **SML-Frame einlesen** – es wird byteweise gelesen, bis ein kompletter Frame
   (vom Startmarker bis zum Endmarker inkl. Füllbytes und Prüfsumme) erkannt wurde.
3. **Frame auswerten** – im Telegramm wird nach zwei Werten gesucht:
   - **Wirkarbeit Bezug gesamt** (Zählerstand) in kWh
   - **aktuelle Wirkleistung** in W

   Zusätzlich werden intern die minimale und maximale Leistung mitgeführt.
4. **Werte senden** – sind beide Werte plausibel (nicht mehr der Startwert `-1.0`),
   werden sie per `curl` als HTTP-POST an openHAB übergeben.
5. **Warten** – kurze Pause (standardmäßig 30 Sekunden), dann Verbindung schließen
   und von vorne beginnen.

Die Messwerte werden auf der Konsole ausgegeben, z. B.:

```
fWirkarbeit (total): 12345.6789kWh	fLeistung (total): 450W		Leistung (min): 120W	Leistung (max): 2100W
```

## Projektstruktur

| Datei                         | Inhalt                                                                 |
| ----------------------------- | ---------------------------------------------------------------------- |
| `stromzaehler/main.cpp`       | Hauptprogramm mit der Endlosschleife                                   |
| `stromzaehler/main.h`         | Konfiguration (Zykluszeit `MIN_LOOP_TIME_SEC`)                         |
| `stromzaehler/serial.cpp`     | Serielle Kommunikation und Auswertung der SML-Frames                   |
| `stromzaehler/serial.h`       | Funktionsdeklarationen für die serielle Schnittstelle                  |
| `stromzaehler/http.cpp`       | Versand der Messwerte an openHAB (per `curl`)                          |
| `stromzaehler/http.h`         | Konfiguration: openHAB-URL und Item-Namen                              |
| `stromzaehler/sml_edl21.h`    | SML-Marker, Byte-Längen und Umrechnungsfaktoren des Zählers            |
| `Makefile`                    | Build-Konfiguration                                                    |
| `strom_start.sh`              | Autostart-Skript (startet das Programm zeitversetzt im Hintergrund)    |

## Voraussetzungen

- Linux-System (das Programm nutzt POSIX-Funktionen wie `termios`, `unistd` und `/dev/...`)
- `g++` zum Kompilieren
- `make`
- `curl` (wird zur Laufzeit zum Senden der Werte aufgerufen)
- Ein SML-fähiger Stromzähler mit einem USB-Infrarot-Lesekopf (oder vergleichbarer serieller Anbindung)
- Ein erreichbarer openHAB-Server

## Installation / Build

Im Projektverzeichnis:

```bash
make
```

Dabei entsteht das ausführbare Programm `strom`.

Zum Aufräumen (Objektdateien und Programm entfernen):

```bash
make clean
```

## Konfiguration

Vor dem Einsatz sind in der Regel ein paar Anpassungen nötig.

### 1. openHAB-Ziel und Item-Namen (`stromzaehler/http.h`)

```c
#define HTTP_TARGET                     "\" http://192.168.0.11:8080/rest/items/"
#define ITEM_WIRKARBEIT_BEZUG_TOTAL     "UG_TECHNIK_Strom_Wirkarbeit_Gesamt_kWh"
#define ITEM_LEISTUNG_TOTAL             "UG_TECHNIK_Strom_Wirkleistung_Gesamt_W"
```

- `HTTP_TARGET`: IP-Adresse und Port des eigenen openHAB-Servers eintragen.
- `ITEM_*`: Namen der openHAB-Items anpassen, in die geschrieben werden soll.

### 2. Zykluszeit (`stromzaehler/main.h`)

```c
#define MIN_LOOP_TIME_SEC 30
```

Pause in Sekunden zwischen zwei Ausleseversuchen. Größere Werte schonen die CPU.

### 3. Serielles Gerät fixieren (`/dev/strom`)

Das Programm öffnet fest das Gerät `/dev/strom`. Dieser Name wird über eine
**udev-Regel** an den konkreten USB-Lesekopf gebunden, damit er sich auch nach
einem Neustart oder beim Umstecken nicht ändert:

1. Vendor- und Produkt-ID des Lesekopfs ermitteln:

   ```bash
   lsusb
   ```

2. Bei Bedarf die Seriennummer herausfinden:

   ```bash
   udevadm info -a -n /dev/ttyUSB0 | grep '{serial}' | head -n1
   ```

3. Eine udev-Regel anlegen, z. B. `/etc/udev/rules.d/99-usb-serial-strom.rules`:

   ```
   SUBSYSTEM=="tty", ATTRS{idVendor}=="10c4", ATTRS{idProduct}=="ea60", ATTRS{serial}=="001DE070", SYMLINK+="strom"
   ```

   (Werte an den eigenen Lesekopf anpassen.)

Danach ist der Lesekopf dauerhaft als `/dev/strom` erreichbar.

## Ausführen

Direkt im Vordergrund (Ausgabe auf der Konsole):

```bash
./strom
```

Automatischer Start zeitversetzt im Hintergrund über das mitgelieferte Skript
`strom_start.sh`. Darin wird das Programm nach einer kurzen Wartezeit via `nohup`
losgelöst vom Terminal gestartet. Den Pfad zum Programm im Skript bei Bedarf anpassen:

```sh
nohup /etc/openhab2/c/strom </dev/null >/dev/null 2>&1 &
```

## Hinweise

- Die zum Zähler passenden SML-Marker, Byte-Längen und Umrechnungsfaktoren sind in
  `stromzaehler/sml_edl21.h` definiert. Für einen anderen Zähler müssen diese Werte
  ggf. angepasst werden. In der Datei sind zwei Beispiel-Telegramme als Referenz
  dokumentiert.
- Das Programm bricht ab, wenn `/dev/strom` nicht geöffnet werden kann. Es müssen
  außerdem ausreichende Rechte auf das serielle Gerät vorhanden sein
  (z. B. Benutzer in der Gruppe `dialout`).
