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
#include "LEDManager.h"
#include "IRManager.h"
#include "WifiManager.h"
#include "CommandInterface.h"
#include "WemoEmulator.h"
#include "SceneCallbackHandler.h"
#include "AwsManager.h"
#include "DatabaseManager.h"

/****************************************
 * FutronixESP8266
 * ---------------
 * The main interface for all functionality on this chip. 
 */
class FutronixESP8266
{
  private:
    /* controls the LED light */
    LEDManager _ledManager;

    /* interface to Futronix chip  */
    CommandInterface _command;

    /* interface to Wifi connection */
    WifiManager _wifiManager;

    /* WEMO emulation  */
    WemoEmulator _wemoEmulator;

    /* on-chip database */
    DatabaseManager _dbManager; 

    /* last set scene */
    int _currentScene = -1; 

  public: 
    FutronixESP8266();

    void begin();
    void loop();

    /***
     * gets entire contens of scene-names DB
     */
    DatabaseRecord* getAllSceneNames(); 

    /***
     * clears EEPROM DB of scene names 
     */
    void clearSceneNames();

    /***
     * renames the specified scene with given name 
     */
    void renameScene(int sceneNo, const char* sceneName); 

    /***
     * sets a scene on the Futronix chip 
     */
    void setScene(int sceneNo); 

    /***
     * gets the last-set scene 
     */
    int getCurrentScene();

    /***
     * restart wemo servers, or start them if not yet running
     */
    void restartWemoServers();    

  private:
    void startWemoServers();
};
/****************************************/

FutronixESP8266 futronix; 

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



/*---------------------------------------*/
FutronixESP8266::FutronixESP8266()
{
  
}

/*---------------------------------------*/
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

/*---------------------------------------*/
void FutronixESP8266::loop()
{
  
}

/*---------------------------------------*/
DatabaseRecord* FutronixESP8266::getAllSceneNames()
{
  return this->_dbManager.getAllRecords();
}

/*---------------------------------------*/
void FutronixESP8266::clearSceneNames()
{
  this->_dbManager.clear(false);
}

/*---------------------------------------*/
void FutronixESP8266::renameScene(int sceneNo, const char* sceneName)
{
  this->_dbManager.setRecord(sceneNo, sceneName);
} 

/*---------------------------------------*/
void FutronixESP8266::setScene(int sceneNo)
{
  this->_command.setScene(sceneNo);
  this->_currentScene = sceneNo;
} 

/*---------------------------------------*/
int FutronixESP8266::getCurrentScene()
{
  return this->_currentScene;
}

/*---------------------------------------*/
void FutronixESP8266::restartWemoServers()
{
  
}    

/*---------------------------------------*/
void FutronixESP8266::startWemoServers()
{
  int baseNumberPort = 80; 
  int baseNamePort = baseNumberPort + this->_dbManager.getRecordCount(); 

  //read all records from EEPROM
  DatabaseRecord* records = this->_dbManager.getAllRecords(); 

  //add wemo server/listeners for every scene & scene name 
  for(int n=0; n<this->_dbManager.getRecordCount(); n++)
  {
    char buffer[10]; 
    memcpy(buffer, "scene ", strlen("scene ")); 
    char* pBuf = buffer; 
    itoa((n+1), pBuf+strlen(buffer), 10); 

    //scene numbers 
    this->_wemoEmulator.addDevice(buffer, baseNumberPort+n, new SceneNumberCallbackHandler(&this->_command, n));

    //scene names 
    if (strlen(records[n].getData()) > 0)
      this->_wemoEmulator.addDevice(buffer, baseNamePort+n, new SceneNameCallbackHandler(&this->_command, &this->_dbManager, records[n].getData())); 
  }
}



