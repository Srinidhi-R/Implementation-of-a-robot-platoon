/*************************************************************
  Download latest Blynk library here:
    https://github.com/blynkkk/blynk-library/releases/latest

  Blynk is a platform with iOS and Android apps to control
  Arduino, Raspberry Pi and the likes over the Internet.
  You can easily build graphic interfaces for all your
  projects by simply dragging and dropping widgets.

    Downloads, docs, tutorials: http://www.blynk.cc
    Sketch generator:           http://examples.blynk.cc
    Blynk community:            http://community.blynk.cc
    Follow us:                  http://www.fb.com/blynkapp
                                http://twitter.com/blynk_app

  Blynk library is licensed under MIT license
  This example code is in public domain.

 *************************************************************
  This example runs directly on ESP32 chip.

  Note: This requires ESP32 support package:
    https://github.com/espressif/arduino-esp32

  Please be sure to select the right ESP32 module
  in the Tools -> Board menu!

  Change WiFi ssid, pass, and Blynk auth token to run :)
  Feel free to apply it to any other example. It's simple!
 *************************************************************/

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial

/* Fill-in your Template ID (only if using Blynk.Cloud) */
#define BLYNK_TEMPLATE_ID   "TMPLBspCPG20"


#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] =  "_AppDVTfgP-AMovQ3ZMEu5vQrUgiss7D";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "Ramachandran";
char pass[] = "Sri@2021";
WidgetBridge bridge1(V0);
unsigned int drn = 10;
unsigned long prev = 0;
BLYNK_WRITE(V0) // Executes when the value of virtual pin 0 changes
{
  if(param.asInt() == 1)
  {
    // execute this code if the switch widget is now ON
    // digitalWrite(2,HIGH);  // Set digital pin 2 HIGH
    drn = 1;
  }
  else
  {
    // execute this code if the switch widget is now OFF
    // digitalWrite(2,LOW);  // Set digital pin 2 LOW  
    drn = 0;  
  }
}


void setup()
{
  // Debug console
  Serial.begin(9600);

  Blynk.begin(auth, ssid, pass);
  pinMode(2, OUTPUT);
  //timer.setInterval(1000L, myTimer); 
}

void loop()
{
  Blynk.run();
  //timer.run();
  if(millis() - prev > 1000)
  {
    Serial.print("\nValue: ");
    Serial.print(drn);
    prev=millis();
  }
  
}
