/*
 * # Database fixed len records 
 *  # read/write DB methods 
 *  
 * # Read in names at startup 
 * # Start appropriate servers 
 *  - Method to restart 
 * 
 * - Web server running 
 *  - gets requests
 *    - updates records 
 *    - restarts servers
 *    
 * - code safety/handling 
 * - code prettify 
 * 
 C:\Users\Home\AppData\Local\Arduino15\packages\esp8266\hardware\esp8266\2.1.0\tools\sdk\lib\libaxtls.a
 C:\Users\Home\AppData\Local\Arduino15\packages\esp8266\hardware\esp8266\2.1.0\libraries\ESP8266WiFi\src\include\DataSource.h
 C:\Users\Home\AppData\Local\Arduino15\packages\esp8266\hardware\esp8266\2.1.0\libraries\ESP8266WiFi\src\include\ssl.h
 C:\Users\Home\AppData\Local\Arduino15\packages\esp8266\hardware\esp8266\2.1.0\libraries\ESP8266WiFi\src\include\ClientContext.h
 C:\Users\Home\AppData\Local\Arduino15\packages\esp8266\hardware\esp8266\2.1.0\libraries\ESP8266WiFi\src\include\WiFiClientSecure.h / .cpp
 C:\Users\Home\AppData\Local\Arduino15\packages\esp8266\hardware\esp8266\2.1.0\libraries\ESP8266WiFi\src\include\WiFiClient.cpp 
 
 */

#include <Arduino.h>

int SCENE_TO_SET = -1; 

#include "debug.h"
#include "FutronixESP8266.h"
#include "AdminServer.h"

FutronixESP8266 futronix;
AdminServer adminServer(&futronix); 

void runOnce();
bool _ranOnce = false;

/*---------------------------------------*/
void setup()
{
  Serial.begin(9600);

  futronix.begin();
  //adminServer.begin();
}

IRTransmitter ir; 
  
/*---------------------------------------*/
void loop()
{
  if (!_ranOnce)
  {
    runOnce();
    _ranOnce = true;
  }
  
  futronix.loop();
  //adminServer.listen();

  if (SCENE_TO_SET >= 0)
  {
    int scene = SCENE_TO_SET;
    SCENE_TO_SET = -1; 
    futronix.setScene(scene); 
  }
}

/*---------------------------------------*/
void runOnce()
{
}



