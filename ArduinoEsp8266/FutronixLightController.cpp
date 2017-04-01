
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


  irSend->enableIROut(38); 
  
  int khz = 38; // 38kHz carrier frequency for the NEC protocol
  unsigned int irSignal[] = {35, params[0], params[1], params[2], params[3], params[4], params[5], 58 }; //AnalysIR Batch Export (IRremote) - RAW
  
  irSend->sendRaw(irSignal, sizeof(irSignal) / sizeof(irSignal[0]), khz); //Note the approach used to automatically calculate the size of the array.

  delay(5000); //In this example, the signal will be repeated every 5 seconds, approximately.

  
  uint64_t command = params[0]; 
  for(int n=1; n<6; n++)
  {
    command = command * 256 + params[n]; 
  }
  

  // 0x 05 00 00 03 0C EF

  Serial.println(params[0], HEX);
  Serial.println(params[1], HEX);
  Serial.println(params[2], HEX);
  Serial.println(params[3], HEX);
  Serial.println(params[4], HEX);
  Serial.println(params[5], HEX);
  //Serial.println(command);


  Serial.print("setting scene: "); 
  Serial.print(scene); 
  Serial.print(" in zone: "); 
  Serial.print(zone); 
  Serial.println(""); 

/*
  irSend->sendNEC(command, 64);
  delay(1000);
  irSend->sendNEC(command, 48);
  delay(1000);
  irSend->sendRC5(command, 64);
  delay(1000);
  irSend->sendRC5(command, 48);
  delay(1000);
  irSend->sendSAMSUNG(command, 64); 
  delay(1000);
  irSend->sendSAMSUNG(command, 48); 
  delay(1000);
  irSend->sendSharpRaw(command, 64); 
  delay(1000);
  irSend->sendSharpRaw(command, 48); 
  delay(1000);


  unsigned char stringMsg[] = "#0x0005000003F8:"; 
  irSend->sendDaikin(stringMsg);
  delay(1000);
  irSend->sendKelvinator(stringMsg);
  delay(1000);
  irSend->sendMitsubishiAC(stringMsg);
  */
}


