#ifndef HTTP_HEADER
#define HTTP_HEADER

#define HTTP_HEAD						"curl --header \"Content-Type: text/plain\" --request POST --data \""
#define HTTP_TARGET						"\" http://192.168.0.11:8080/rest/items/"
#define ITEM_WIRKARBEIT_BEZUG_TOTAL		"UG_TECHNIK_Strom_Wirkarbeit_Gesamt_kWh"
#define ITEM_LEISTUNG_TOTAL				"UG_TECHNIK_Strom_Wirkleistung_Gesamt_W"

int http(float *fWirkarbeitBezugTotal_kWh, float *fLeistung_W);

#else
#endif
