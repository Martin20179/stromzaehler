#include <iostream>
#include <stdio.h>      // standard input / output functions
#include <stdlib.h>
#include <string.h>     // string function definitions
#include <unistd.h>     // UNIX standard function definitions
#include <fcntl.h>      // File control definitions
#include <errno.h>      // Error number definitions
#include <termios.h>    // POSIX terminal control definitions
#include <iomanip>		// cout hex into string

# include <getopt.h>
# include <sys/types.h>
# include <sys/stat.h>

#include "main.h"
#include "serial.h"
#include "http.h"

using namespace std;
using std::string;

/*
	Hauptfunktion
*/
int main()
{
	int USBCon = -1;
	float fWirkarbeitBezugTotal_kWh = -1.0;
	float fLeistung_W = -1.0;
	float fLeistungMin_W = 9999.0;
	float fLeistungMax_W = 0.0;
	
	
	
	cout << "*** Starte Stromablesung ***\n\n";
	
	// Probiere USB/Seriell Verbindung zu öffnen
	//openConnection(&USBCon);
		
	while(true)
	{
		// Probiere USB/Seriell Verbindung zu öffnen
		// Test in der Loop jedes mal öffnen und schließen
		openConnection(&USBCon);
	
		serialRead(&USBCon, &fWirkarbeitBezugTotal_kWh, &fLeistung_W, &fLeistungMin_W, &fLeistungMax_W);
		// wurden plausible Werte geschrieben?
		if ( (fWirkarbeitBezugTotal_kWh != -1.0) && (fLeistung_W != -1.0) )
			http(&fWirkarbeitBezugTotal_kWh, &fLeistung_W);

		sleep(MIN_LOOP_TIME_SEC);       // damit die CPU etwas entlasstet ist
		
		closeConnection(&USBCon);
	}
	
	//closeConnection(&USBCon);
	
}
