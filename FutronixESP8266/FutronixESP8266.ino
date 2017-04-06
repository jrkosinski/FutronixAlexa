
#include <Arduino.h>

#include "LEDManager.h"
#include "IRManager.h"
#include "WifiManager.h"
#include "FutronixInterface.h"
#include "WemoEmulator.h"
#include "SceneCallbackHandler.h"
#include "AwsManager.h"

#define WIFI_SSID     "Mina"
#define WIFI_PASSWD   "HappyTime"

LEDManager _ledManager;
FutronixInterface _futronix;
WifiManager _wifiManager(WIFI_SSID, WIFI_PASSWD);
WemoEmulator _wemoEmulator;

void setup()
{
  Serial.begin(9600);
  pinMode(IR_PIN, OUTPUT);
  
  //_ledManager.begin();
  //_wifiManager.begin();
  //_futronix.begin();

/*
  if (_wifiManager.connect())
  {
    _wemoEmulator.begin();

    _wemoEmulator.addDevice("scene one", 80, new SceneCallbackHandler(&_futronix, 1));
    _wemoEmulator.addDevice("scene two", 81, new SceneCallbackHandler(&_futronix, 2));
    _wemoEmulator.addDevice("scene three", 82, new SceneCallbackHandler(&_futronix, 3));
  }
  */
}

void loop()
{
  digitalWrite(IR_PIN, LOW); 
  delay(1000);
  digitalWrite(IR_PIN, HIGH); 
  delay(1000);
  //if (_wemoEmulator.isRunning)
  //  _wemoEmulator.listen();

  //_irManager.test(1);
}

