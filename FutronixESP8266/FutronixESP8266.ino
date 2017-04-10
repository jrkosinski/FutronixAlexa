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
 */

#include <Arduino.h>

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
  //futronix.loop();
  if (!_ranOnce)
  {
    runOnce();
    _ranOnce = true;
  }
  
  //adminServer.listen();

/*
  LEDOutput led; 
  led.begin(); 
  led.blink(10); 
  led.off(); 
  delay(3000); 
  */
}

/*---------------------------------------*/
void runOnce()
{
  //WiFiClient client; 
  //if (client.connect("192.168.1.38", 1000))
  //  Serial.println("CONNECTED");
  
  //delay(1000); 
  //ir.test(3);

/*
  delay(3000); 
  for(int n=-1; n<20; n++)
  {
    futronix.setScene(n); 
    delay(3000); 
  }
  futronix.setScene(255); 
  */
}



