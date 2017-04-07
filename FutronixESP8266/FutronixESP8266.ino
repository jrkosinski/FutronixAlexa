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

/*---------------------------------------*/
void setup()
{
  Serial.begin(9600);

  futronix.begin();
}

/*---------------------------------------*/
void loop()
{
  futronix.loop();
}



