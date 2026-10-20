/*
	serial.h
	--------
	Deklarationen fuer die serielle Kommunikation mit dem Stromzaehler
	und die Auswertung der eingelesenen SML-Frames.
*/

#ifndef SERIAL_HEADER
#define SERIAL_HEADER

// Oeffnet die serielle Verbindung (/dev/strom) und konfiguriert sie (9600 8N1)
int openConnection(int *USBCon);
// Schliesst die serielle Verbindung
int closeConnection(int *USBCon);
// Liest einen vollstaendigen SML-Frame (Start- bis Endmarker) und wertet ihn aus
char serialRead(int *USBCon, float *ptrfWirkarbeitBezugTotal_kWh, float *ptrfLeistung_W, float *ptrfLeistungMin_W, float *ptrfLeistungMax_W);
// Durchsucht einen Frame nach den gesuchten Messwerten und schreibt sie in die Zeiger
int frameAnalyse(char *frame, int size, float *ptrfWirkarbeitBezugTotal_kWh, float *ptrfLeistung_W, float *ptrfLeistungMin_W, float *ptrfLeistungMax_W);

#else
#endif