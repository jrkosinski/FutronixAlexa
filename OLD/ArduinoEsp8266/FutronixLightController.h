
#include "IRremoteESP8266.h"
#include <Arduino.h>

#ifndef __FUTRONIX_LIGHT_CONTROLLER_H__
#define __FUTRONIX_LIGHT_CONTROLLER_H__

class FutronixLightController
{
  private: 
    IRsend* _irSend; //an IR led is connected to GPIO pin 0
    
  public:
    FutronixLightController(); 
    ~FutronixLightController();

    void begin();
    void setSceneInZone(int zone, int scene); 
    void tv(); 
};

#endif

