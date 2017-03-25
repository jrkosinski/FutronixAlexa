
#include "IRremoteESP8266.h"
#include <Arduino.h>

#ifndef __FUTRONIX_LIGHT_CONTROLLER_H__
#define __FUTRONIX_LIGHT_CONTROLLER_H__

class FutronixLightController
{
  public:
    void setSceneInZone(int zone, int scene, IRsend* irSend); 
};

#endif

