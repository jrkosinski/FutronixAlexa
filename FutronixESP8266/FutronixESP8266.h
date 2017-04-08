#ifndef __FUTRONIX_ESP8266_H__
#define __FUTRONIX_ESP8266_H__

#include <Arduino.h>

#include "LEDOutput.h"
#include "IRTransmitter.h"
#include "WifiConnection.h"
#include "CommandInterface.h"
#include "WemoEmulator.h"
#include "SceneCallbackHandler.h"
#include "AwsManager.h"
#include "Database.h"

/****************************************
 * FutronixESP8266
 * ---------------
 * The main interface for all functionality on this chip. 
 */
class FutronixESP8266
{
  private:
    /* controls the LED light */
    LEDOutput* _led;

    /* interface to Futronix chip  */
    CommandInterface* _command;

    /* interface to Wifi connection */
    WifiConnection* _wifi;

    /* WEMO emulation  */
    WemoEmulator* _wemoEmulator;

    /* on-chip database */
    Database* _db; 

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


/*---------------------------------------*/
FutronixESP8266::FutronixESP8266()
{
  this->_led = new LEDOutput();
  this->_command = new CommandInterface(); 
  this->_wifi = new WifiConnection(); 
  this->_wemoEmulator = new WemoEmulator(); 
  this->_db = new Database();
}

/*---------------------------------------*/
void FutronixESP8266::begin()
{
  //this->_led->begin();
  //this->_wifi->begin();
  //this->_command->begin();
  this->_db->begin(); 

/*
  if (this->_wifi.connect())
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
  return this->_db->getAllRecords();
}

/*---------------------------------------*/
void FutronixESP8266::clearSceneNames()
{
  this->_db->clear(false);
}

/*---------------------------------------*/
void FutronixESP8266::renameScene(int sceneNo, const char* sceneName)
{
  this->_db->setRecord(sceneNo, sceneName);

  //TODO: restart web server 
} 

/*---------------------------------------*/
void FutronixESP8266::setScene(int sceneNo)
{
  this->_command->setScene(sceneNo);
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
  int baseNamePort = baseNumberPort + this->_db->getRecordCount(); 

  //read all records from EEPROM
  DatabaseRecord* records = this->_db->getAllRecords(); 

  //add wemo server/listeners for every scene & scene name 
  for(int n=0; n<this->_db->getRecordCount(); n++)
  {
    char buffer[10]; 
    memcpy(buffer, "scene ", strlen("scene ")); 
    char* pBuf = buffer; 
    itoa((n+1), pBuf+strlen(buffer), 10); 

    //scene numbers 
    this->_wemoEmulator->addDevice(buffer, baseNumberPort+n, new SceneNumberCallbackHandler(this->_command, n));

    //scene names 
    if (strlen(records[n].getData()) > 0)
      this->_wemoEmulator->addDevice(buffer, baseNamePort+n, new SceneNameCallbackHandler(this->_command, this->_db, records[n].getData())); 
  }
}

#endif

