
#include <Arduino.h>

#include "LEDManager.h"
#include "IRManager.h"
#include "WifiManager.h"
#include "FutronixInterface.h"
#include "WemoEmulator.h"
#include "SceneCallbackHandler.h"

#define WIFI_SSID     "Mina"
#define WIFI_PASSWD   "HappyTime"

LEDManager _ledManager;
IRManager _irManager;
FutronixInterface _futronix(&_irManager);
WifiManager _wifiManager(WIFI_SSID, WIFI_PASSWD);
WemoEmulator _wemoEmulator;

void setup()
{
  _ledManager.begin();
  _wifiManager.begin();
  _irManager.begin();
  _futronix.begin();

  if (_wifiManager.connect())
  {
    _wemoEmulator.begin();

    _wemoEmulator.addDevice("scene one", 80, new SceneCallbackHandler(&_futronix, 1));
  }
}

void loop()
{
  _wemoEmulator.listen();
}

