#ifndef SERIAL_HEADER
#define SERIAL_HEADER

int openConnection(int *USBCon);
int closeConnection(int *USBCon);
char serialRead(int *USBCon, float *ptrfWirkarbeitBezugTotal_kWh, float *ptrfLeistung_W, float *ptrfLeistungMin_W, float *ptrfLeistungMax_W);
int frameAnalyse(char *frame, int size, float *ptrfWirkarbeitBezugTotal_kWh, float *ptrfLeistung_W, float *ptrfLeistungMin_W, float *ptrfLeistungMax_W);

#else
#endif