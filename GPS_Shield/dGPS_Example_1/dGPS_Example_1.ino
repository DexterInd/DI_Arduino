// #include <dGPS.h>

/*
 This program demonstrates the dGPS library from Dexter Industries for Arduino
 
 For use with the Dexter Industries GPS Shield.  The dGPS can be found here:
   - http://dexterindustries.com/Arduino-GPS_Shield.html
 
 This code was originally based on the work of others.  You can see the original work here:
   - SoftwareSerial Library:  http://www.arduino.cc/en/Reference/SoftwareSerial
   - GPS Tutorial:  http://www.arduino.cc/playground/Tutorials/GPS
 
 How it works:
   - The dGPS requires the SoftwareSerial.h library.
   - The GPS is initialized in the setup function.
   - The GPS is updated: values for location, time, etc, are updated using the ".update" function.
   - The values are read using their respective calls.  These values stay the same until "update" is called again. 
   
 Note on Arduino MEGA and Leonardo:  You will need to change the pin on the board to 10/11/12.
 Change the corresponding #define SoftrxPin in dGPS.cpp to 10/11/12 as well.  

 IMPORTANT: Make sure that the pin selector on the dGPS is set to "10" before proceeding!
 
 */

#include "string.h"
#include "ctype.h"
#include "SoftwareSerial.h"
#include "dGPS.h"
#include "Arduino.h"

// Software serial TX & RX Pins for the GPS module
// Initiate the software serial connection

int ledPin = 13;                  // LED test pin
float Mumbai[]={18.9647,72.8258};
float London[]={51.5171,-0.1062};
float Washington[]={38.89,-77.03};
float BuenosA[]={-34.6036,-58.3817};
float Rome[]={41.9,12.5};
float Tokyo[]={35.6833,139.7667};

dGPS dgps = dGPS();               // Construct dGPS class

void setup() {
  pinMode(ledPin, OUTPUT);       // Initialize LED pin
  Serial.end();                  // Close any previously established connections
  Serial.begin(9600);            // Serial output back to computer.  On.
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  Serial.println("Initialize GPS.");
  dgps.init();                   // Run initialization routine for dGPS.
  delay(1000);  
  Serial.println("Initialized.");
}

void loop() {
  
  // dgps.update(Mumbai[0],Mumbai[1]);    // Calling this updates the GPS data.  The data in dGPS variables stays the same unless
                                   // this function is called.  When this function is called, the data is updated.
  dgps.update(Mumbai[0],Mumbai[1]);                                   
  Serial.print("Distance to Mumbai: ");
  Serial.print(dgps.Dist());      // Distance to the destination- in kilometers. Correct upto 2 decimal places. Radius of Earth taken as 6,378.1 kilometers
  Serial.println(" kilometers");
  Serial.print("Azimuth to Mumbai: "); 
  Serial.print(dgps.Azim());      //Azimuth of the destination coordinates from the current location- in degrees. Correct upto 2 decimal places
  Serial.println(" degrees");
  Serial.println("");   
  
  dgps.update(London[0],London[1]);   
  Serial.print("Distance to London: ");
  Serial.print(dgps.Dist());      
  Serial.println(" kilometers");
  Serial.print("Azimuth to London: "); 
  Serial.print(dgps.Azim());      
  Serial.println(" degrees");
  Serial.println(""); 
  
  dgps.update(Washington[0],Washington[1]); 
  Serial.print("Distance to Washington DC: ");
  Serial.print(dgps.Dist());      
  Serial.println(" kilometers");
  Serial.print("Azimuth to Washington DC: "); 
  Serial.print(dgps.Azim());      
  Serial.println(" degrees");
  Serial.println(""); 
  
  dgps.update(BuenosA[0],BuenosA[1]);
  Serial.print("Distance to Buenos Aires: ");
  Serial.print(dgps.Dist());      
  Serial.println(" kilometers");
  Serial.print("Azimuth to Buenos Aires: "); 
  Serial.print(dgps.Azim());      
  Serial.println(" degrees");
  Serial.println(""); 
  
  dgps.update(Rome[0],Rome[1]);
  Serial.print("Distance to Rome: ");
  Serial.print(dgps.Dist());      
  Serial.println(" kilometers");
  Serial.print("Azimuth to Rome: "); 
  Serial.print(dgps.Azim());      
  Serial.println(" degrees");
  Serial.println(""); 
  
  dgps.update(Tokyo[0],Tokyo[1]);
  Serial.print("Distance to Tokyo: ");
  Serial.print(dgps.Dist());      
  Serial.println(" kilometers");
  Serial.print("Azimuth to Tokyo: "); 
  Serial.print(dgps.Azim());      
  Serial.println(" degrees");
  Serial.println(""); 
}
