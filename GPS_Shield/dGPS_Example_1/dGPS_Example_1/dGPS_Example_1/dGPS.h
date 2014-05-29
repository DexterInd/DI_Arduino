// dGPS.h
// For use with the Dexter Industries / diCircuits.com dGPS
// The dGPS can be found here:  http://dexterindustries.com/Arduino-GPS_Shield.html


#include "SoftwareSerial.h"
#include "pins_arduino.h"
//#include "WProgram.h"
#include "Arduino.h"

#ifndef dGPS_h
#define dGPS_h

class dGPS
{
	public: 
		dGPS();
		void init();
		boolean update(float,float);
                boolean updategga();

		long Time();
		char *Status();
		float Lat();
		float Lon();
		float Vel();
		float Head();
		long Date();
                float Dist();
                float Azim();
                char *Mode();
                int SatView();
                float Hdop();
                float Alti();
                boolean Compare();
                void getflag(char*);
                float getdestcoord();
                long int Checked();
                
	private:
		// int _pin;
		char linea[300];
		//int _rxPin;
		//int _txPin;
		int cont;
		int bien;
		int conta;
		int indices[13];
		char comandoGPR[7]; // = "$GPRMC";
                char comandoGPG[7]; // = "$GPGGA";
		float conv_coords(float in_coords);
		char *subStr (char *str, char *delim, int index);
                int Csum(char*);
                char *Checks();
          	
};

#endif



