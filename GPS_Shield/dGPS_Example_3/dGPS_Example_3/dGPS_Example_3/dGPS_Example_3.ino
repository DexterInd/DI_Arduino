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
char fla2[2];                     //flag (Y/N) whether to print Altitude, number of satellites used and HDOP. Filled by user in Serial Monitor Box
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
  Serial.println("IMPORTANT: Make sure that the pin selector on the dGPS Shield");
  Serial.println("is set to 10 before proceeding!");
  delay(1000);  
  
  Serial.print("Do you want to display checksum (Y/N): "); 
  delay(3000);
  getflag(fla);
  Serial.println(*fla);
  
  *fla2=Serial.read();           // To clear rhe buffer before the actual flag value is read
  memset(fla2, 0, sizeof(fla2));
  Serial.print("Do you want to display Altitude, Satellites used and HDOP (Y/N): "); 
  delay(3000);
  getflag(fla2);
  Serial.println(*fla2);
  
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
  
  dgps.update(desLat, desLon);    // Calling this updates the GPS data.  The data in dGPS variables stays the same unless
                                  // this function is called.  When this function is called, the data is updated.
  Serial.print("UTC Time: ");
  Serial.println(dgps.Time());    // .Time returns the UTC time (GMT) in HHMMSS, 24 huor format (H-Hour; M-Minute; S-Second)
  
  Serial.print("Status: ");
  Serial.println(dgps.Status());  // A - Satellites acquired and a valid signal.  V - No sats and not a valid signal.
  
  Serial.print("Latitude: ");
  Serial.print(dgps.Lat(), 6);    // Lattitude - in DD.MMSSSS format (decimal-degrees format)  (D-Degree; M-Minute; S-Second)
  Serial.println(" degrees");
  
  Serial.print("Longitude: ");
  Serial.print(dgps.Lon(), 6);    // Longitude - in DD.MMSSSS format (decimal-degrees format)  (D-Degree; M-Minute; S-Second)
  Serial.println(" degrees");
  
  Serial.print("Velocity: ");
  Serial.print(dgps.Vel(), 6);    // Velocity, in knots.
  Serial.println(" knots");
  
  Serial.print("Heading: ");
  Serial.print(dgps.Head(), 6);   // Heading, in degrees
  Serial.println(" degrees");
  
  Serial.print("UTC Date(DDMMYY): ");
  Serial.println(dgps.Date());    // UTC date.  Date is in format:  DDMMYY (D - Day; M - Month; Y-Year)
  
  Serial.print("Distance to destination: ");
  Serial.print(dgps.Dist());      // The distance to the destination in kilometers. Correct upto 2 decimal points. Radius of Earth taken as 6,378.1 kilometers
  Serial.println(" kilometers");
  
  Serial.print("Azimuth to destination: "); 
  Serial.print(dgps.Azim());      //Azimuth of the destination coordinates from the current location in degrees. Correct upto 2 decimal points
  Serial.println(" degrees");
  
  Serial.print("Mode Indicator: "); 
  Serial.println(dgps.Mode());     //Mode Indicator (N-Data not valid,A-Autonomous mode,D-Differential mode,E-Estimated mode,M-Manual input mode,S-Simulator mode
    switch(*fla){                 //SWITCH CASE TO CHECK IF THE USER WANTS CHECKSUM OR NOT
     case 'Y':
     case 'y':
       Serial.print("Received CheckSum: ");
       Serial.println(dgps.Checks()); //Checksum received from packet
       Serial.print("Computed Checksum: ");
       Serial.println(dgps.Checked(),HEX); //Checksum computed
       break;
     case 'N':
     case 'n':
       Serial.print("");
       break;
     default: Serial.print(""); }
     
     switch(*fla2){               //SWITCH CASE TO CHECK IF THE USER WANTS Number Of Satellites, HDOP, and Altitude OR NOT
     case 'Y':
     case 'y':
       dgps.updategga();         //updates the values of Number of Satellites, HDOP and Altitude 
       Serial.print("Number of Satellites in use: ");
       Serial.println(dgps.SatView()); // Number of Satellites in use
       Serial.print("HDOP: ");
       Serial.println(dgps.Hdop());    // HDOP
       Serial.print("Altitude: ");
       Serial.print(dgps.Alti());      // Altitude (in meters) above sea-level
       Serial.println(" meters above sea level");
       break;
     case 'N':
     case 'n':
       Serial.print("");
       break;
     default: Serial.print(""); }
       
  Serial.println(""); 
}
