#include <iostream>		// Standard-Ein-/Ausgabe
#include <stdio.h>      // standard input / output functions
#include <string.h>     // string function definitions
#include <stdlib.h>

#include "http.h"

using namespace std;
using std::string;

int http(float *fWirkarbeitBezugTotal_kWh, float *fLeistung_W)
{

	char strToSend[256];
	// fWirkarbeitBezugTotal_kWh
	sprintf(strToSend, HTTP_HEAD "%.04f" HTTP_TARGET ITEM_WIRKARBEIT_BEZUG_TOTAL"\n",*fWirkarbeitBezugTotal_kWh);
	system(strToSend);
	
	//fLeistung_W
	sprintf(strToSend, HTTP_HEAD "%.0f" HTTP_TARGET ITEM_LEISTUNG_TOTAL"\n",*fLeistung_W);
	system(strToSend);
	
	return 0;
}