/*
	main.cpp
	--------
	Einstiegspunkt des Stromzaehler-Programms.

	Ablauf:
	  1. Serielle USB-Verbindung zum Stromzaehler oeffnen (/dev/strom)
	  2. Einen vollstaendigen SML-Frame einlesen und auswerten
	  3. Bei plausiblen Werten die Messwerte per HTTP (curl) an openHAB senden
	  4. Kurz warten (MIN_LOOP_TIME_SEC) und die Verbindung wieder schliessen
	  5. Endlos wiederholen

	Hinweis: Das Programm ist fuer Linux/POSIX ausgelegt (termios, unistd, /dev/...).
*/

#include <iostream>
#include <stdio.h>      // Standard-Ein-/Ausgabe (printf, ...)
#include <stdlib.h>     // Allgemeine Hilfsfunktionen (exit, ...)
#include <string.h>     // String-Funktionen (memset, memcpy, ...)
#include <unistd.h>     // UNIX-Standardfunktionen (read, close, sleep, ...)
#include <fcntl.h>      // Dateikontroll-Definitionen (open, fcntl, ...)
#include <errno.h>      // Fehlernummer-Definitionen (errno)
#include <termios.h>    // POSIX-Terminalsteuerung (serielle Schnittstelle)
#include <iomanip>		// Formatierung von cout (Hex-Ausgabe)

# include <getopt.h>		// Kommandozeilen-Optionen (aktuell nicht genutzt)
# include <sys/types.h>
# include <sys/stat.h>

#include "main.h"		// MIN_LOOP_TIME_SEC, Deklaration von main()
#include "serial.h"		// Serielle Schnittstelle und SML-Auswertung
#include "http.h"		// Versand der Messwerte an openHAB

using namespace std;
using std::string;

/*
	Hauptfunktion: Initialisiert die Messwerte und durchlaeuft die Endlosschleife,
	die den Zaehler ausliest und die Werte an openHAB uebergibt.
*/
int main()
{
	int USBCon = -1;							// Dateideskriptor der seriellen Verbindung (-1 = ungueltig)
	float fWirkarbeitBezugTotal_kWh = -1.0;		// Zaehlerstand Bezug gesamt in kWh (-1.0 = noch kein Wert)
	float fLeistung_W = -1.0;					// aktuelle Wirkleistung in W (-1.0 = noch kein Wert)
	float fLeistungMin_W = 9999.0;				// bisher kleinste gemessene Leistung in W
	float fLeistungMax_W = 0.0;					// bisher groesste gemessene Leistung in W
	
	
	
	cout << "*** Starte Stromablesung ***\n\n";
	
	// Verbindung wird innerhalb der Schleife geoeffnet (siehe unten)
	//openConnection(&USBCon);
		
	while(true)
	{
		// Serielle Verbindung in jedem Durchlauf neu oeffnen und am Ende wieder schliessen
		openConnection(&USBCon);
	
		// Einen SML-Frame einlesen und die Messwerte in die uebergebenen Variablen schreiben
		serialRead(&USBCon, &fWirkarbeitBezugTotal_kWh, &fLeistung_W, &fLeistungMin_W, &fLeistungMax_W);
		// Nur senden, wenn plausible Werte ermittelt wurden (also nicht mehr der Startwert -1.0)
		if ( (fWirkarbeitBezugTotal_kWh != -1.0) && (fLeistung_W != -1.0) )
			http(&fWirkarbeitBezugTotal_kWh, &fLeistung_W);

		sleep(MIN_LOOP_TIME_SEC);       // kurze Pause, um die CPU zu entlasten
		
		closeConnection(&USBCon);		// Verbindung wieder schliessen
	}
	
	//closeConnection(&USBCon);
	
}
