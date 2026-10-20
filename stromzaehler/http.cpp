/*
	http.cpp
	--------
	Uebertraegt die ermittelten Messwerte an openHAB.

	Fuer jeden Wert wird ein curl-Befehl zusammengesetzt und ueber system()
	ausgefuehrt. Die Ziel-URL und die Item-Namen sind in http.h definiert.
*/

#include <iostream>		// Standard-Ein-/Ausgabe
#include <stdio.h>      // Standard-Ein-/Ausgabe (sprintf)
#include <string.h>     // String-Funktionen
#include <stdlib.h>     // system()

#include "http.h"

using namespace std;
using std::string;

// Sendet Zaehlerstand (kWh) und aktuelle Leistung (W) jeweils per curl an openHAB
int http(float *fWirkarbeitBezugTotal_kWh, float *fLeistung_W)
{

	char strToSend[256];	// Puffer fuer den zusammengesetzten curl-Befehl

	// Zaehlerstand (Wirkarbeit Bezug gesamt) mit 4 Nachkommastellen senden
	sprintf(strToSend, HTTP_HEAD "%.04f" HTTP_TARGET ITEM_WIRKARBEIT_BEZUG_TOTAL"\n",*fWirkarbeitBezugTotal_kWh);
	system(strToSend);
	
	// Aktuelle Leistung ohne Nachkommastellen senden
	sprintf(strToSend, HTTP_HEAD "%.0f" HTTP_TARGET ITEM_LEISTUNG_TOTAL"\n",*fLeistung_W);
	system(strToSend);
	
	return 0;
}