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

#include "LEDManager.h"
#include "IRManager.h"
#include "WifiManager.h"
#include "CommandInterface.h"
#include "WemoEmulator.h"
#include "SceneCallbackHandler.h"
#include "AwsManager.h"
#include "DatabaseManager.h"

class FutronixESP8266
{
  private:
    LEDManager _ledManager;
    CommandInterface _command;
    WifiManager _wifiManager;
    WemoEmulator _wemoEmulator;
    DatabaseManager _dbManager; 

  public: 
    FutronixESP8266();

    void begin();
    void loop();

    DatabaseRecord* getAllSceneNames(); 
    void clearSceneNames();
    void renameScene(int sceneNo, char* sceneName); 
    void setScene(int sceneNo); 
    int getCurrentScene();
    void restartWemoServers();    

  private:
    void startWemoServers();
};

FutronixESP8266 futronix; 

void setup()
{
  Serial.begin(9600);

  futronix.begin();
}

void loop()
{
  futronix.loop();
}



FutronixESP8266::FutronixESP8266()
{
  
}

void FutronixESP8266::begin()
{
  //this->_ledManager.begin();
  //this->_wifiManager.begin();
  //this->_command.begin();
  this->_dbManager.begin(); 

/*
  if (this->_wifiManager.connect())
  {
    this->_wemoEmulator.begin();

    this->startWemoServers();
  }
  */
}

void FutronixESP8266::loop()
{
  
}

void FutronixESP8266::startWemoServers()
{
  int baseNumberPort = 80; 
  int baseNamePort = baseNumberPort + this->_dbManager.getRecordCount(); 

  DatabaseRecord* records = this->_dbManager.getAllRecords(); 
  
  for(int n=0; n<this->_dbManager.getRecordCount(); n++)
  {
    char buffer[10]; 
    memcpy(buffer, "scene ", strlen("scene ")); 
    char* pBuf = buffer; 
    itoa((n+1), pBuf+strlen(buffer), 10); 
    
    this->_wemoEmulator.addDevice(buffer, baseNumberPort+n, new SceneNumberCallbackHandler(&this->_command, n));
    this->_wemoEmulator.addDevice(buffer, baseNamePort+n, new SceneNameCallbackHandler(&this->_command, &this->_dbManager, records[n].getData())); 
  }
}



