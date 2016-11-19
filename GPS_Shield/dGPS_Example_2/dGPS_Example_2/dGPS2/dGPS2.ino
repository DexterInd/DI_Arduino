/*
 This program demonstrates the dGPS library from Dexter Industries for Arduino
 
 For use with the Dexter Industries GPS Shield.  The dGPS can be found here:
   - http://dexterindustries.com/Arduino-GPS_Shield.html
 
 This code was originally based on the work of others.  You can see the original work here:
   - SoftwareSerial Library:  http://www.arduino.cc/en/Reference/SoftwareSerial
   - GPS Tutorial:  http://www.arduino.cc/playground/Tutorials/GPS
 
 See our Arduino GPS Converting Coordinates for information on converting coordinates
 from GPS Style Coordinates (ddmm.ssss) to Decimal Style Coordinates (dd.mmssss).
 
 How it works:
   - The dGPS requires the SoftwareSerial.h library.
   - The GPS is initialized in the setup function.
   - The GPS is updated: values for location, time, etc, are updated using the ".update" function.
   - The values are read using their respective calls.  These values stay the same until "update" is called again. 

IMPORTANT: Make sure that the pin selector on the dGPS Shield is set to "10" before proceeding!

*/

#include "string.h"
#include "ctype.h"
#include "SoftwareSerial.h"
#include "dGPS.h"

// Software serial TX & RX Pins for the GPS module
// Initiate the software serial connection

int ledPin = 13;                  // LED test pin
float desLat=0;                   //Destination Latitude filled by user in Serial Monitor Box
float desLon=0;                   //Destination Longitude filled by user in Serial Monitor Box
char fla[2];                      //flag (Y/N) whether to print checksum or not. Filled by user in Serial Monitor Box
dGPS dgps = dGPS();               // Construct dGPS class

float getdestcoord()
  // function to get the coordinates of the destination from user
  { float result;
    while (Serial.available()==0)
    {;}// do nothing until something comes into the serial buffer

    if (Serial.available()>0)
    {
     result=Serial.parseFloat(); //read a float value from serial monitor box, correct upto 2 decimal places
     delay(10); // the processor needs a moment to process
    }
    return result;
  }
  
 void getflag(char *str)
   // function to read the flag character from the user 
  {  
    while (Serial.available()==0)
    {;}
    int index=0;
    if (Serial.available()>0)
    { if (index<2){
      str[index]=Serial.read();
      index++;
     }
      else str[index]='\0';
      delay(10);
    }  
  }

void setup() {
  pinMode(ledPin, OUTPUT);       // Initialize LED pin
  Serial.end();                  // Close any previously established connections
  Serial.begin(9600);            // Serial output back to computer.  On.
  dgps.init();                   // Run initialization routine for dGPS.
  delay(1000);  

  Serial.println("IMPORTANT: Make sure that the pin selector on the dGPS Shield is set to 10 before proceeding!");
  delay(1000);  
  Serial.print("Do you want to display checksum (Y/N): "); 
  delay(3000);
  getflag(fla);
  Serial.println(*fla);
  Serial.print("Enter Destination Latitude (in degrees): ");
  delay(3000);
  desLat=getdestcoord();
  Serial.println(desLat);
  Serial.print("Enter Destination Longitude (in degrees): ");
  delay(3000);
  desLon=getdestcoord();
  Serial.println(desLon);
}

void loop() {
  
  dgps.update(desLat, desLon);                  // Calling this updates the GPS data.  The data in dGPS variables stays the same unless
                                  // this function is called.  When this function is called, the data is updated.
  Serial.print("UTC Time: ");
  Serial.println(dgps.Time());    // .Time returns the UTC time (GMT) in HHMMSS, 24 huor format (H-Hour; M-Minute; S-Second)
  Serial.print("Status: ");
  Serial.println(dgps.Status());  // A - Satellites acquired and a valid signal.  V - No sats and not a valid signal.
  Serial.print("Latitude: ");
  Serial.println(dgps.Lat(), 6);  // Lattitude - in DD.MMSSSS format (decimal-degrees format)  (D-Degree; M-Minute; S-Second)
  Serial.print("Longitude: ");
  Serial.println(dgps.Lon(), 6);  // Longitude - in DD.MMSSSS format (decimal-degrees format)  (D-Degree; M-Minute; S-Second)
  Serial.print("Velocity: ");
  Serial.println(dgps.Vel(), 6);  // Velocity, in knots.
  Serial.print("Heading: ");
  Serial.println(dgps.Head(), 6);  // Heading, in degrees
  Serial.print("UTC Date: ");
  Serial.println(dgps.Date());  // UTC date.  Date is in format:  DDMMYY (D - Day; M - Month; Y-Year)
  Serial.print("Distance to destination: ");
  Serial.println(dgps.Dist());    // The distance to the destination in kilometers. Correct upto 2 decimal points. Radius of Earth taken as 6,378.1 kilometers
  Serial.print("Azimuth to destination: "); 
  Serial.println(dgps.Azim());     //Azimuth of the destination coordinates from the current location in degrees. Correct upto 2 decimal points
  Serial.print("Mode Indicator: "); 
  Serial.println(dgps.Mode());     //Azimuth of the destination coordinates from the current location in degrees. Correct upto 2 decimal points
    switch(*fla){ //SWITCH CASE TO CHECK IF THE USER WANTS CHECKSUM OR NOT
     case 'Y':
     case 'y':
       Serial.print("CheckSum: ");
       Serial.println(dgps.Checks());
       break;
     case 'N':
     case 'n':
       Serial.print("");
       break;
     default: Serial.print(""); }
 Serial.println("");
}
