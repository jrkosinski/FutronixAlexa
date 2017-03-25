
#include "FutronixLightController.h"
#include "IRremoteESP8266.h"


void FutronixLightController::setSceneInZone(int zone, int scene, IRsend* irSend)
{
  byte params[6];
  byte cardAddr = 0; 
  byte opCode = 5; 
  byte operatingMode = 0; 
  
  params[0] = cardAddr; 
  params[1] = opCode;
  params[2] = operatingMode; 
  params[3] = zone; 
  params[4] = scene; 

  byte checksum = 0; 
  for(int n=0; n<5; n++)
  {
    checksum = (checksum + params[n]) & 0xff; 
  }

  params[5] = (0x100 - checksum); 

  uint64_t command = params[0]; 
  for(int n=1; n<6; n++)
  {
    command = command * 256 + params[n]; 
  }

/*
  // 0x 05 00 00 00 0C EF

  Serial.println(params[0], HEX);
  Serial.println(params[1], HEX);
  Serial.println(params[2], HEX);
  Serial.println(params[3], HEX);
  Serial.println(params[4], HEX);
  Serial.println(params[5], HEX);
  //Serial.println(command);

  uint64_t target = 21474839790; 
  if (command == target)
    Serial.println("OK"); 
  Serial.print("\n");
  */
  irSend->sendNEC(command, 56);
}


