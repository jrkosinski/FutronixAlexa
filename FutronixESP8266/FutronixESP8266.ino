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
  if (!_ranOnce)
  {
    runOnce();
    _ranOnce = true;
  }
  
  futronix.loop();
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
}



