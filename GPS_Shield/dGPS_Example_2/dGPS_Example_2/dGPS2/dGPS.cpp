// dGPS.cpp
// For use with the Dexter Industries / diCircuits.com dGPS
// The dGPS can be found here:  http://dexterindustries.com/Arduino-GPS_Shield.html

#include "dGPS.h"
#include "SoftwareSerial.h"
#include "pins_arduino.h"
//#include "WProgram.h"
#include "Arduino.h"
#include "math.h"
#define MAX_STRING_LEN  300

#define SoftrxPin 10
#define SofttxPin 4

long fTime;
char *cStatus;
float fLat;
float fLon;
float fVel;
float fHead;
long lDate;
float fdist;
float fazimuth;
char *mode;
char *checkSum;

SoftwareSerial gpsSerial = SoftwareSerial(SoftrxPin, SofttxPin);

dGPS::dGPS()
{
  cont = 0;
  bien = 0;
  conta = 0;
  comandoGPR[0] = '$';
  comandoGPR[1] = 'G';
  comandoGPR[2] = 'P';
  comandoGPR[3] = 'R';
  comandoGPR[4] = 'M';
  comandoGPR[5] = 'C';
}


void dGPS::init(){
  gpsSerial.begin(9600); //start serial for communication with GPS
  memset(linea, ' ', sizeof(linea));
}

long dGPS::Time(){
	return fTime;
}

char *dGPS::Status(){
	return cStatus;
}

float dGPS::Lat(){
	return fLat;
}

float dGPS::Lon(){
	return fLon;
}

float dGPS::Vel(){
	return fVel;
}

float dGPS::Head(){
	return fHead;
}

long dGPS::Date(){
	return lDate;
}

float dGPS::Dist(){
        return fdist;
}

float dGPS::Azim(){
        return fazimuth;
}

char *dGPS::Mode(){
        return mode;
}

char *dGPS::Checks(){
        return checkSum;
}  

bool dGPS::update(float desLat, float desLon){

 while(true){
  int byteGPS=-1;
  byteGPS=gpsSerial.read();      //read a byte from the serial port
  if (byteGPS == -1) {           // See if the port is empty yet
    delay(100); 
  } 
  else {
    linea[conta]=byteGPS;        // If there is serial port data, it is put in the buffer
    conta++;                      
    if (byteGPS==13){            // If the received byte is = to 13, end of transmission 
      cont=0;
      bien=0;
      for (int i=1;i<7;i++){     // Verifies if the received command starts with $GPR
        if (linea[i]==comandoGPR[i-1]){
          bien++;
        }
      }
      if(bien==6){               // If yes, continue and process the data
        
        Serial.print("");        // For some reason, this needs to be here or you won't populate variables.  If anyone solves this problem, let us know.
        Serial.print("");
        Serial.print("-");
        Serial.println(linea);   //Print out the GPRMC string (just for reference)
          //------------------------------------------          
          // Get time
          char *data = "";
          memset(data, 0, sizeof(data));
          data = dGPS::subStr(linea, ",", 2);
          fTime = atol(data);
        
          //------------------------------------------
          // Status
          memset(data, 0, sizeof(data));
          cStatus = subStr(linea, ",", 3);
          
          //------------------------------------------
          // Latitude
          char *hemi;
          memset(data, 0, sizeof(data));
          char *cLat = "";
          cLat = dGPS::subStr(linea, ",", 4);
          fLat = strtod(cLat, NULL);
          fLat = conv_coords(fLat);
          hemi = subStr(linea, ",", 5);    // Sets the hemisphere.
          if(*hemi == 'S') {fLat = fLat*-1;}        // Setting the hemisphere.
          //Serial.println(hemi);

          //------------------------------------------          
          // Longitude
          memset(data, 0, sizeof(data));
          char *cLon = "";
          cLon = dGPS::subStr(linea, ",", 6);
          fLon = strtod(cLon, NULL);
          fLon = conv_coords(fLon);
          hemi = subStr(linea, ",", 7);    // Sets the hemisphere.
          if(*hemi == 'W') {fLon = fLon*-1;}        // Setting the hemisphere.
          //Serial.println(*hemi);
          
          //------------------------------------------
          // Velocity (knots)
          memset(data, 0, sizeof(data));
          char *cVel = "";
          cVel = dGPS::subStr(linea, ",", 8);
          fVel = strtod(cVel, NULL);

          //------------------------------------------
          // Heading (Degrees)
          memset(data, 0, sizeof(data));
          char *cHead = "";
          cHead = dGPS::subStr(linea, ",", 9);
          fHead = strtod(cHead, NULL);
          //Serial.print("Heading: ");
          //Serial.println(fHead, 6);
          
          //------------------------------------------
          // Date (UTC)
          memset(data, 0, sizeof(data));
          char *cDate = "";
          cDate = dGPS::subStr(linea, ",", 10);
          lDate = strtod(cDate, NULL);
          
          //------------------------------------------
          //Distance between current location and destination coordinates          
          float t=((desLon-fLon)*22)/1260; //difference between the longitudes in radians

          float cdist=acos(cos(((90-desLat)*22)/1260)*cos(((90-fLat)*22)/1260)+sin(((90-desLat)*22)/1260)*sin(((90-fLat)*22)/1260)*cos(t)); //angular distance
          fdist=6378.1*cdist; //distance= radius of earth * angular distance
          
          //------------------------------------------
          //Azimuth of the destination from current location
          float cazimuth=asin((sin(((90-desLat)*22)/1260)*sin(t))/sin(cdist)); //temporary azimuth(in radians)
          fazimuth=((cazimuth*1260)/22); //azimuth in degrees
          
          //------------------------------------------
          //Mode Indicator 
          int numcom=0;  
          for (int i=0;i<MAX_STRING_LEN;i++)   //find the number of "," separators
          {   if (linea[i]==',')
              { numcom++; }
          }
          //Serial.print("number of commas: ");
          //Serial.println (numcom);

          memset(data, 0, sizeof(data));
          char *cstr= subStr(linea,",",numcom); //Takes the whole substring at the end of the GPRMC string (eg:A*0C)
          //Serial.println(cstr);
          mode=subStr(cstr, "*", 1); //picks up mode indicator A from A*0C
          
          //------------------------------------------
          //Checksum printed only if flag is Y
          memset(data, 0, sizeof(data));
          checkSum = subStr(cstr, "*", 2); // picks up checksum 0C from A*0C
          
          conta=0;                    // Reset the buffer
          memset(linea, 0, sizeof(linea));
	  break;
          
      }
      conta=0;                    // Reset the buffer
      memset(linea, 0, sizeof(linea));  
    }
  }
 } // END WHILE STATEMENT
 return true;  
}


// Function to return a substring defined by a delimiter at an index
// Using it to parse out the GPS string
char *dGPS::subStr (char *str, char *delim, int index) {
  char *act, *sub, *ptr;
  static char copy[MAX_STRING_LEN];
  int i;

  // Since strtok consumes the first arg, make a copy
  strcpy(copy, str);

  for (i = 1, act = copy; i <= index; i++, act = NULL) {
    //Serial.print(".");
    sub = strtok_r(act, delim, &ptr);
    if (sub == NULL) break;
  }

  return sub;
}

// This function converts data from GPS-style (ddmm.ssss) to decimal style (dd.mmssss)
float dGPS::conv_coords(float in_coords)
{
  //Initialize the location.
  float f = in_coords;
  // Get the first two digits by turning f into an integer, then doing an integer divide by 100;
  // firsttowdigits should be 77 at this point.
  int firsttwodigits = ((int)f)/100;                               //This assumes that f < 10000.
  float nexttwodigits = f - (float)(firsttwodigits*100);
  float theFinalAnswer = (float)(firsttwodigits + nexttwodigits/60.0);
  return theFinalAnswer;
}
