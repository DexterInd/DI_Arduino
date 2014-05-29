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

long fTime;                       // UTC Time
char *cStatus;                    // Status
float fLat;                       // Current Latitude
float fLon;                       // Current Longitude
float fVel;                       // Velocity
float fHead;                      // Heading
long lDate;                       // UTC Date
float fdist;                      // Distance to destination
float fazimuth;                   // Azimuth of destination from current position
char *mode;                       // Mode
char *checkSum;                   // Received Checksum
int fSat;                         // Number of Satellites in use
float hDop;                       // HDOP
float fAlt;                       // Altitude above sea level
int computedSum;                  // Calculated Checksum

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
  gpsSerial.begin(9600);     //start serial for communication with GPS
  memset(linea, ' ', sizeof(linea)); }

long dGPS::Time(){          // returns UTC Time
	return fTime; }

char *dGPS::Status(){       // returns Status
	return cStatus; }

float dGPS::Lat(){          // returns Current Latitude
	return fLat; }

float dGPS::Lon(){          // returns Current Longitude
	return fLon; }

float dGPS::Vel(){          // returns Velocity
	return fVel; }

float dGPS::Head(){          // returns Heading
	return fHead; }

long dGPS::Date(){           // returns UTC Date
	return lDate; }

float dGPS::Dist(){          // returns Distance to destination
        return fdist; }

float dGPS::Azim(){          // returns Azimuth to destination
        return fazimuth; }

char *dGPS::Mode(){          // returns Mode
        return mode; }

char *dGPS::Checks(){        // returns Received Checksum
        return checkSum; }  

long int dGPS::Checked(){
  return computedSum;
}

// Compare the received checksum, and the calculated checksum. 
//If they are completely equal, return true. Otherwise return false.
boolean dGPS::Compare(){          
  char s[3];
  char *recCheck=Checks();                // received checksum
  long int x= Checked();                  // Computed checksum from the incoming $GPRMC string
  char *ccomCheck=itoa(x,s,16);           // convert long int to hexadecimal string 
  char comCheck[strlen(ccomCheck)];       
  for (int i=0;i<=strlen(ccomCheck);i++){ // convert ToUpper case, each character of hexadecimal string
    comCheck[i]= toupper(ccomCheck[i]);}

  boolean flag;
  // for each character in hexadecimal string, check if it matches with the received checksum value
  for (int i=0;i<min(strlen(recCheck),strlen(comCheck));i++){ 
    if (recCheck[i]==comCheck[i])
       { flag=true;  }                   // if characters in checksum match
    else { flag=false; }                 // if characters in checksum do not match
  }
  return flag;                // if all the characters in checksum match, then 'true' is returned. Otherwise, 'false' is returned.
}

int dGPS::SatView(){         // returns Number of Satellites in use
	return fSat; }

float dGPS::Hdop(){          // returns HDOP value
	return hDop; }

float dGPS::Alti(){          // returns Altitude above sea level
	return fAlt; }

// function to read the Y or N character from the user via serial monitor box   
void dGPS::getflag(char *str) {  
  /*  The following statements must precede the call to this function,
      to clear out any character data already in the serial buffer.
      Code: char flag[2];
            *flag = Serial.read();          
            memset(flag, 0, sizeof(flag));
      Call: object.getflag(flag); */
  while (Serial.available()==0){;}   // If nothing available in serial buffer, wait and do nothing.
  int index=0;
  if (Serial.available()>0){         // If data available in serial buffer, then read it
   if (index<2){
      str[index]=Serial.read();
      index++; }
   else str[index]='\0';
      delay(5);
   }
}

// function to get the latitudinal and longitudinal coordinates of the destination from user
float dGPS::getdestcoord(){  
  float result;
  while (Serial.available()==0) {;}      // do nothing until something comes into the serial buffer
  if (Serial.available()>0){
    result=Serial.parseFloat();          //read a float value from serial monitor box, upto 2 decimal places
    delay(5);                           
    }
  return result;
}

// update all the fields from the incoming $GPRMC string
boolean dGPS::update(float desLat, float desLon){
 while(true){
  int byteGPS=-1;
  byteGPS=gpsSerial.read();      //read a byte from the serial port
  if (byteGPS == -1) {           // See if the port is empty yet
    delay(5); 
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
        /*Serial.println(linea);   //Print out the incoming GPRMC string (just for reference)*/

          //------------------------------------------          
          // Get Time (UTC)
          char *data = "";
          memset(data, 0, sizeof(data));
          data = dGPS::subStr(linea, ",", 2);
          fTime = atol(data);
          delay(5);
        
          //------------------------------------------
          // Get Status
          memset(data, 0, sizeof(data));
          cStatus = dGPS::subStr(linea, ",", 3);
          delay(5);
          
          //------------------------------------------
          // Get Latitude (degrees)
          char *hemi;
          memset(data, 0, sizeof(data));
          char *cLat = "";
          cLat = dGPS::subStr(linea, ",", 4);
          fLat = strtod(cLat, NULL);
          fLat = conv_coords(fLat);
          hemi = subStr(linea, ",", 5);             // Gets the hemisphere.
          if(*hemi == 'S') {fLat = fLat*-1;}        // Setting the Latitude.
          delay(5);
          
          //------------------------------------------          
          // Get Longitude (degrees)
          memset(data, 0, sizeof(data));
          char *cLon = "";
          cLon = dGPS::subStr(linea, ",", 6);
          fLon = strtod(cLon, NULL);
          fLon = conv_coords(fLon);
          hemi = subStr(linea, ",", 7);            // Gets the hemisphere.
          if(*hemi == 'W') {fLon = fLon*-1;}       // Setting the Longitude.
          delay(5);          
          
          //------------------------------------------
          // Get Velocity (knots)
          memset(data, 0, sizeof(data));
          char *cVel = "";
          cVel = dGPS::subStr(linea, ",", 8);
          fVel = strtod(cVel, NULL);
          delay(5);
          
          //------------------------------------------
          // Get Heading (degrees)
          memset(data, 0, sizeof(data));
          char *cHead = "";
          cHead = dGPS::subStr(linea, ",", 9);
          fHead = strtod(cHead, NULL);
          delay(5);          

          //------------------------------------------
          // Get Date (UTC)
          memset(data, 0, sizeof(data));
          char *cDate = "";
          cDate = dGPS::subStr(linea, ",", 10);
          lDate = strtod(cDate, NULL);
          delay(5);
          
          //------------------------------------------
          // Get Distance between current location and destination coordinates (kilometers)         
          float t=((desLon-fLon)*22)/1260;           // difference between the longitudes in radians

          float cdist=acos(cos(((90-desLat)*22)/1260)*cos(((90-fLat)*22)/1260)+sin(((90-desLat)*22)/1260)*sin(((90-fLat)*22)/1260)*cos(t)); //angular distance
          fdist=6378.1*cdist;                       // distance= radius of earth * angular distance
          
          //------------------------------------------
          // Get Azimuth of the destination from current location
          float cazimuth=asin((sin(((90-desLat)*22)/1260)*sin(t))/sin(cdist)); // temporary azimuth(in radians)
          fazimuth=((cazimuth*1260)/22);           // azimuth in degrees
          
          //------------------------------------------
          // Get Mode Indicator 
          memset(data, 0, sizeof(data));
          char *cstr= subStr(linea,",",11);       // Takes the whole substring at the end of the GPRMC string (eg:A*0C)
          //Serial.println(cstr);
          mode=subStr(cstr, "*", 1);              // picks up mode indicator A from A*0C
          delay(5);          
          
          //------------------------------------------
          // Get Checksum value from packet and compute checksum from packet. 
          // used only if user entered Y or y for filtering out only valid data
          memset(data, 0, sizeof(data));
          checkSum = subStr(cstr, "*", 2);       // picks up checksum 0C from A*0C
          delay(5);
          computedSum=Csum(linea);               // compute checksum from the incoming string
                    
          conta=0;                               // Reset the buffer
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

// Function to update Number of Satellites used, HDOP, Altitude etc.
// from the incoming $GPGGA string
boolean dGPS::updategga(){  
  comandoGPG[0] = '$';          // initialization
  comandoGPG[1] = 'G';
  comandoGPG[2] = 'P';
  comandoGPG[3] = 'G';
  comandoGPG[4] = 'G';
  comandoGPG[5] = 'A';
  while(true){
  int byteGPS=-1;
  byteGPS=gpsSerial.read();      //read a byte from the serial port
  if (byteGPS == -1) {           // See if the port is empty yet
    delay(5); } 
  else {
    linea[conta]=byteGPS;        // If there is serial port data, it is put in the buffer
    conta++;                      
    if (byteGPS==13){            // If the received byte is = to 13, end of transmission 
      cont=0;
      bien=0;
      for (int i=1;i<7;i++){     // Verifies if the received command starts with $GPGGA
        if (linea[i]==comandoGPG[i-1]){
          bien++;
        }
      }
      if(bien==6){               // If yes, continue and process the data        
        Serial.print("");        // For some reason, this needs to be here or you won't populate variables.  If anyone solves this problem, let us know.
        Serial.print("");
        Serial.print("-");
        /*Serial.println(linea);   //Print out the GPGGA string (just for reference)*/
          
          //------------------------------------------          
          // Get Number of satellites in use
          char *data = "";
          memset(data, 0, sizeof(data));
          data = dGPS::subStr(linea, ",", 8);
          fSat = atoi(data);
          delay(5);
        
          //------------------------------------------
          // Get HDOP
          memset(data, 0, sizeof(data));
          data = dGPS::subStr(linea, ",", 9);
          hDop = atof(data);
          delay(5);
          
          //------------------------------------------
          // Get Altitude above sea-level (meters)
          memset(data, 0, sizeof(data));
          data = dGPS::subStr(linea, ",", 10);
          fAlt = atof(data);          
          delay(5);
          
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

// Function to return a substring defined by a delimiter at an index.
// Using it to parse out the GPS string.
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

// Calculate the checksum of the $GPRMC string received
int dGPS::Csum(char *line){ 
  int XOR = 0;                   
  int x;
  int lenline = strlen(line);    
  for (int i=0;i<lenline;i++){   //Find position of '*' character in string
    if(line[i]=='*') x=i;}
    
  for (int c = 2; c < x; c++) { //start after $ till just before * and xor all the characters 1-by-1
    XOR =XOR ^ (unsigned char)line[c];
  }
  return XOR;
}
