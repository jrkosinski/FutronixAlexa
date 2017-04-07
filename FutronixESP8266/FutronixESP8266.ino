
#include <Arduino.h>

#include "LEDManager.h"
#include "IRManager.h"
#include "WifiManager.h"
#include "FutronixInterface.h"
#include "WemoEmulator.h"
#include "SceneCallbackHandler.h"
#include "AwsManager.h"
#include "DatabaseManager.h"

#define WIFI_SSID     "Mina"
#define WIFI_PASSWD   "HappyTime"

LEDManager _ledManager;
FutronixInterface _futronix;
WifiManager _wifiManager(WIFI_SSID, WIFI_PASSWD);
WemoEmulator _wemoEmulator;
DatabaseManager _dbManager; 


void addWemoServers(); 

void setup()
{
  Serial.begin(9600);
  //pinMode(IR_PIN, OUTPUT);
  
  //_ledManager.begin();
  //_wifiManager.begin();
  //_futronix.begin();
  _dbManager.begin(); 

/*
  if (_wifiManager.connect())
  {
    _wemoEmulator.begin();

    addWemoServers();
  }
  */
}

void loop()
{
  //if (_wemoEmulator.isRunning)
  //  _wemoEmulator.listen();

  //_futronix.test();
  _dbManager.test(); 
  delay(3000); 
}


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
 */


 void addWemoServers()
 {
  int baseNumberPort = 80; 
  int baseNamePort = baseNumberPort + _dbManager.getRecordCount(); 

  DatabaseRecord* records = _dbManager.getAllRecords(); 
  
  for(int n=0; n<_dbManager.getRecordCount(); n++)
  {
    char buffer[10]; 
    memcpy(buffer, "scene ", strlen("scene ")); 
    char* pBuf = buffer; 
    itoa((n+1), pBuf+strlen(buffer), 10); 
    
    _wemoEmulator.addDevice(buffer, baseNumberPort+n, new SceneNumberCallbackHandler(&_futronix, n));
    _wemoEmulator.addDevice(buffer, baseNamePort+n, new SceneNameCallbackHandler(&_futronix, &_dbManager, records[n].getData())); 
  }
 }



