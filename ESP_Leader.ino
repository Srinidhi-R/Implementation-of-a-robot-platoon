
#define BLYNK_PRINT Serial

/* Fill-in your Template ID (only if using Blynk.Cloud) */
#define BLYNK_TEMPLATE_ID   "TMPLN6wWGVaw"


#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "f4397UM-S26IzOVto0iRLmW7dhMWdIrM";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "Ramachandran";
char pass[] = "Sri@2021";
WidgetBridge bridge1(V0);
BlynkTimer timer; 
bool sensorVal = 0;
unsigned int i = 0;

BLYNK_CONNECTED()
{
  bridge1.setAuthToken("_AppDVTfgP-AMovQ3ZMEu5vQrUgiss7D");
}

void myTimer() 
{
  // This function describes what will happen with each timer tick
  // e.g. writing sensor value to datastream V5
  Blynk.virtualWrite(V0, sensorVal);
  bridge1.digitalWrite(2, sensorVal);  
  
}


void setup()
{
  // Debug console
  Serial.begin(9600);

  Blynk.begin(auth, ssid, pass);
  timer.setInterval(1000L, myTimer); 
}

void loop()
{
  Blynk.run();
  timer.run();
  if(i%2 == 0)
    sensorVal = 1;
  else
    sensorVal = 0;
  i++;
}
