
#include "FutronixLightController.h"
#include "IRremoteESP8266.h"


FutronixLightController::FutronixLightController()
{
  this->_irSend = new IRsend(0); 
}
FutronixLightController::~FutronixLightController()
{
  delete this->_irSend;
}

void FutronixLightController::begin()
{
  this->_irSend->begin();
}

void FutronixLightController::setSceneInZone(int zone, int scene)
{
  /*
   4 address bits (0000) 
   6 command bits (19, or 0x13, or 010011, for scene 3) 
   1 parity bit: 1 if the number of 1s is even 
   bin: 0000 010011 0
   dec: 38
   hex: 
   */

  //this->_irSend->ledOff();
  //return;
   
   this->_irSend->sendFutronix(17); 
   //delay(5000); 

/*
   this->_irSend->sendNEC(0x20DF10EF, 32); 
   delay(5000); 
   this->_irSend->sendLG(0x20DF10EF, 32); 
   */
}

void FutronixLightController::tv()
{
  this->_irSend->sendLG(0x20DF10EF, 32); 
  delay(1000);
   this->_irSend->sendNEC(0x20DF10EF, 32); 
  delay(1000);
}

