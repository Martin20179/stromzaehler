/*
	http.h
	------
	Konfiguration fuer den Versand der Messwerte an openHAB.

	Die Werte werden per curl als HTTP-POST an die openHAB-REST-API uebertragen.
	Der vollstaendige Befehl setzt sich zusammen aus:
	    HTTP_HEAD + <Wert> + HTTP_TARGET + <Item-Name>

	Bei Bedarf hier anpassen:
	  - HTTP_TARGET: IP-Adresse und Port des openHAB-Servers
	  - ITEM_*     : Namen der openHAB-Items, in die geschrieben wird
*/

#ifndef HTTP_HEADER
#define HTTP_HEADER

// Anfang des curl-Aufrufs inkl. Content-Type und Beginn der POST-Daten
#define HTTP_HEAD						"curl --header \"Content-Type: text/plain\" --request POST --data \""
// Abschluss der POST-Daten und Ziel-URL der openHAB-REST-API (Item-Name wird angehaengt)
#define HTTP_TARGET						"\" http://192.168.0.11:8080/rest/items/"
// openHAB-Item fuer den Zaehlerstand (Wirkarbeit Bezug gesamt) in kWh
#define ITEM_WIRKARBEIT_BEZUG_TOTAL		"UG_TECHNIK_Strom_Wirkarbeit_Gesamt_kWh"
// openHAB-Item fuer die aktuelle Wirkleistung in W
#define ITEM_LEISTUNG_TOTAL				"UG_TECHNIK_Strom_Wirkleistung_Gesamt_W"

// Sendet Zaehlerstand und Leistung an openHAB
int http(float *fWirkarbeitBezugTotal_kWh, float *fLeistung_W);

#else
#endif
