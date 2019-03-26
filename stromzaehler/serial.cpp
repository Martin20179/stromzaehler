#include <iostream>
#include <stdio.h>      // standard input / output functions
#include <stdlib.h>
#include <string.h>     // string function definitions
#include <unistd.h>     // UNIX standard function definitions
#include <fcntl.h>      // File control definitions
#include <errno.h>      // Error number definitions
#include <termios.h>    // POSIX terminal control definitions
#include <iomanip>		// cout hex into string

#include "serial.h"		// eigener Header
#include "sml_edl21.h"	// Zähler Eigenschaften

using namespace std;
using std::string;
//using std::stringstream

//struct termios tty;
//struct termios tty_old;
//memset (&tty, 0, sizeof tty);

/*
        Öffne  Serielle USB Verbindung
		
		# Fixieren der USB-Devices
		# 1. mit "lusb" Vendor ID und Produkt ID herausfinden
		# 2. ggf. dann noch mit "udevadm info --query=all --attribute-walk --name=/dev/ttyUSB0" oder mit " udevadm info -a -n /dev/ttyUSB0 | grep '{serial}' | head -n1" Seriennummer rausfinden
		# 3. Regel erstellen
		#		sudo nano /etc/udev/rules.d/99-usb-serial-fernwaerme.rules
		#			SUBSYSTEM=="tty", ATTRS{idVendor}=="067b", ATTRS{idProduct}=="2303", SYMLINK+="fernwaerme"
		#		sudo nano /etc/udev/rules.d/99-usb-serial-strom.rules
		#			SUBSYSTEM=="tty", ATTRS{idVendor}=="10c4", ATTRS{idProduct}=="ea60", ATTRS{serial}=="001DE070", SYMLINK+="strom"
*/
int openConnection(int *USBCon)
{
	struct termios tty;
	string serialPort = "/dev/strom";
	
	*USBCon = open("/dev/strom", O_RDWR| O_NONBLOCK | O_NDELAY );
	if (*USBCon == -1)
	{
		perror("Fehler: Serieller Port </dev/strom>konnte nicht geöffnet werden\n");
		exit(EXIT_FAILURE);
	}
	fcntl(*USBCon, F_SETFL, 0);
	cout << "Serieller Port <" << serialPort << "> wurde erfolgreich geöffnet. File description ist <" << *USBCon << ">" << endl << endl;
	
	/* Set Baud Rate */
	cfsetospeed (&tty, (speed_t)B9600);
	cfsetispeed (&tty, (speed_t)B9600);

	/* Setting other Port Stuff */
	tty.c_cflag     &=  ~PARENB;		// Parity none
	tty.c_cflag     &=  ~CSTOPB;		// one StopBit
	tty.c_cflag     &=  ~CSIZE;			// clear out the current word size
	tty.c_cflag     |=  CS8;			// 8Bits (plus 1 Parity Bit)

	tty.c_cflag     &=  ~CRTSCTS;		// no flow control
	//tty.c_cflag     |=  CRTSCTS;		// flow control
	tty.c_cc[VMIN]	=   1;//1;			// read doesn't block
	tty.c_cc[VTIME]	=   100;			// Timeout (Einheit: 100ms)
	tty.c_cflag     |=  CREAD | CLOCAL;	// turn on READ & ignore ctrl lines

	/* Make raw */
	cfmakeraw(&tty);
	
	/* Flush Port, then applies attributes */
	tcflush( *USBCon, TCIFLUSH );
	if ( tcsetattr ( *USBCon, TCSANOW, &tty ) != 0)
	{
		cout << "Error " << errno << " from tcsetattr" << endl;
	}
	
	/* Flush Port, then applies attributes */
	tcflush( *USBCon, TCIFLUSH );
}

char serialRead(int *USBCon, float *ptrfWirkarbeitBezugTotal_kWh, float *ptrfLeistung_W, float *ptrfLeistungMin_W, float *ptrfLeistungMax_W)
{
	int result = -1;
	int total_read = 0;
	int blockSize=1000;
	char inBuf[1];
	char blockBuffer[blockSize];
	const char marker_start[] = MARKER_START;
	const char marker_end[] = MARKER_END;
	
	int endMarkerCnt=0;
	
	bool foundStartMarker = false;
	bool total_read_reset = false;
	bool foundEndMarker = false;
	
	// buffer löschen
	for (int i=0; i<blockSize; i++)
		blockBuffer[i]=0;
	
	//printf("Size of buffer %d", sizeof buffer);
	//sleep(10);
    do
    {
        result = read(*USBCon, inBuf, 1);
		blockBuffer[total_read] = inBuf[0];
		total_read_reset=false;
		
		// erst StartMarker suchen
		if (!foundStartMarker)
		{
			// StartMarker überprüfen
			if (blockBuffer[total_read] != marker_start[total_read]) // falls keine Übereinstimmung einfach zurücksetzen
				total_read_reset=true;
			else
			{
				// prüfen ob bereits kompletter StartMarker erkannt wurde
				if (total_read == (sizeof marker_start - 1) ) // kompletter Start erkannt
				{
					//cout << "StartMarker erkannt :-)" << endl;
					foundStartMarker = true;
				}
			}
		}
		else
		{
			//printf("Result: %d Total_read: %d\n", result, total_read);
			
			if (!foundEndMarker)
			{
				// EndMarker überprüfen
				if (blockBuffer[total_read] == marker_end[endMarkerCnt]) // Zeichenübereinstimmung gefunden
					endMarkerCnt++;
				else
				{
					// Prüfen ob Marker komplett erkannt und Endbytes eingelesen werden können
					// oder des sich doch um keinen Endmarker handelt
					if ( (sizeof marker_end) == endMarkerCnt )
					{
						//printf("Anzahl der Füllbytes: %d\n", blockBuffer[total_read]);
						endMarkerCnt++;
					}
					else if ( (sizeof marker_end + 1) == endMarkerCnt )
						endMarkerCnt++;
					else if ( (sizeof marker_end + 2) == endMarkerCnt )
					{
						//printf("Prüfsumme: 0x%.04x\n", ((blockBuffer[total_read-1] & 0xFF) << 8) | blockBuffer[total_read] & 0xFF);
						// supi, Endmarker erkannt
						//cout << "EndMarker erkannt :-)" << endl;
						foundEndMarker = true;
					}
					else
						endMarkerCnt=0;
				}
			}
		}
		
		//printf("Result: 0x%.02x - StartMarker 0x%.02x - Total_read: %d\n", blockBuffer[total_read], marker_start[total_read], total_read);
		
		if (total_read_reset) // Positionszähler zurücksetzen
			total_read = 0;
		else	// Positionszähler kann nun eins hochgezählt werden;	
			total_read++;
			
		//printf("Total_read: %d - total_read_reset: %d - foundStartMarker: %d\n", total_read, total_read_reset, foundStartMarker);
    }
    while ( (total_read < blockSize) && (!foundEndMarker) );
	
	//printf("Gelesen: %dBytes\n", total_read);
	/*for (int i=0; i < total_read; i++)
	{
		printf("%02x", 0xFF & blockBuffer[i]);
	}
	cout << endl << endl;
	*/
	
	// gelesener Frame übergeben
	frameAnalyse(blockBuffer, total_read, ptrfWirkarbeitBezugTotal_kWh, ptrfLeistung_W, ptrfLeistungMin_W, ptrfLeistungMax_W);
	
	return 0;
}

int frameAnalyse(char *frame, int size, float *ptrfWirkarbeitBezugTotal_kWh, float *ptrfLeistung_W, float *ptrfLeistungMin_W, float *ptrfLeistungMax_W)
{
	//cout << "Frame-Analyse starten..." << endl;
	
	//char test[] = memcpy();
	char searchWirkarbeitBezugTotal[] = WIRKARBEIT_BEZUG_TOTAL_MARKER;
	int posWirkarbeitBezugTotal = 0;
	int iWirkarbeitBezugTotal = 0;
	
	char searchLeistungTotal[] = LEISTUNG_TOTAL_MARKER;
	int posLeistungTotal = 0;
	int iLeistungTotal = 0;
	
	/*
	cout << "searchLeistungTotal: ";
	for (int i=0; i < sizeof searchLeistungTotal; i++)
	{
		printf("%02x", 0xFF & searchLeistungTotal[i]);
	}
	cout << endl;
	*/
	
	// die Suche geht los...
	// kompletter Frame durchlaufen lassen
	for (int i=0; i<size; i++)
	{
		// suche nach searchWirkarbeitBezugTotal
		for (int j=0; j<sizeof searchWirkarbeitBezugTotal; j++)
		{
			if(frame[i+j] != searchWirkarbeitBezugTotal[j])
				break;
			
			if (j == (sizeof searchWirkarbeitBezugTotal -1) )
			{
				//printf("Treffer <searchWirkarbeitBezugTotal> :-) an Position i: %d j: %d\n\n", i, j );
				posWirkarbeitBezugTotal = i + j + 1;
			}
		}
		
		// suche nach searchLeistungTotal
		for (int j=0; j<sizeof searchLeistungTotal; j++)
		{
			if(frame[i+j] != searchLeistungTotal[j])
				break;
			
			if (j == (sizeof searchLeistungTotal -1) )
			{
				//printf("Treffer <searchLeistungTotal> :-) an Position i: %d j: %d\n\n", i, j );
				posLeistungTotal = i + j + 1;
			}
		}
	}
	
	// kann WirkarbeitBezugTotal geholt werden?
	if (posWirkarbeitBezugTotal != 0)
	{	
		// ja, dann holen
		for (int i=0; i<WIRKARBEIT_BEZUG_TOTAL_LENGTH; i++)
		{
			iWirkarbeitBezugTotal |= ((0xFF & frame[posWirkarbeitBezugTotal+i]) << ((WIRKARBEIT_BEZUG_TOTAL_LENGTH-1-i) * 8));
		}
	}
	
	// kann LeistungTotal geholt werden?
	if (posLeistungTotal != 0)
	{	
		// ja, dann holen
		for (int i=0; i<LEISTUNG_TOTAL_LENGTH; i++)
		{
			iLeistungTotal |= ((0xFF & frame[posLeistungTotal+i]) << ((LEISTUNG_TOTAL_LENGTH-1-i) * 8));
		}
	}
	
	// Ausgabe
	if ( (posWirkarbeitBezugTotal != 0) && (posLeistungTotal != 0) )
	{
		//printf("iWirkarbeitBezugTotal: %d\n", iWirkarbeitBezugTotal);
		*ptrfWirkarbeitBezugTotal_kWh = (float) iWirkarbeitBezugTotal/WIRKARBEIT_BEZUG_TOTAL_FAKTOR;
		printf("fWirkarbeit (total): %.4fkWh", *ptrfWirkarbeitBezugTotal_kWh);
		
		//printf("iLeistungTotal: %d\n", iLeistungTotal);
		*ptrfLeistung_W = (float) iLeistungTotal/LEISTUNG_TOTAL_FAKTOR;
		printf("\tfLeistung (total): %.0fW", *ptrfLeistung_W);
		
		// Min Wert überprüfen und ggf. neu setzen
		if (*ptrfLeistungMin_W > *ptrfLeistung_W)
			*ptrfLeistungMin_W = *ptrfLeistung_W;
			
		if (*ptrfLeistungMax_W < *ptrfLeistung_W)
			*ptrfLeistungMax_W = *ptrfLeistung_W;
		
		printf("\t\tLeistung (min): %.0fW", *ptrfLeistungMin_W);
		printf("\tLeistung (max): %.0fW\n", *ptrfLeistungMax_W);
		
		return 0;
	}
	else
		return -1;
}

int closeConnection(int * USBCon)
{
	close(*USBCon);
	return 0;
}
