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
    WemoEmulator* _wemo;

    /* on-chip database */
    Database* _db; 

    /* last set scene */
    int _currentScene = -1; 

  public: 
    FutronixESP8266();

    void begin();
    void loop();

    /***
     * does initial setup of DB & chip 
     */
    void setup(const char* wifiSsid, const char* wifiPasswd, bool clearDatabase); 

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
  this->_db = new Database();
  this->_command = new CommandInterface(); 
  this->_wifi = new WifiConnection(this->_db->getWifiSsid(), this->_db->getWifiPasswd()); 
  this->_wemo = new WemoEmulator(); 
}

/*---------------------------------------*/
void FutronixESP8266::begin()
{
  DEBUG_PRINTLN("Futronix:begin"); 
  
  this->_led->begin();
  this->_wifi->begin();
  this->_command->begin();
  this->_db->begin(); 

  this->_db->setRecord(0, "");
  this->_db->setRecord(2, "");
  //this->_db->getAllRecords();

  if (this->_wifi->connect())
  {
    this->_wemo->begin();
    this->startWemoServers();
  }
}

/*---------------------------------------*/
void FutronixESP8266::loop()
{
  //this->_wemo->listen();
}

/*---------------------------------------*/
void FutronixESP8266::setup(const char* wifiSsid, const char* wifiPasswd, bool clearDatabase)
{
  //clear database 
  if (clearDatabase)
  {
    this->_db->clear(true);
  }

  //set wifi name & passwd
  this->_db->setWifiSsid(wifiSsid);
  this->_db->setWifiPasswd(wifiPasswd);
}

/*---------------------------------------*/
DatabaseRecord* FutronixESP8266::getAllSceneNames()
{
  DEBUG_PRINTLN("Futronix:getAllSceneNames"); 
  return this->_db->getAllRecords();
}

/*---------------------------------------*/
void FutronixESP8266::clearSceneNames()
{
  DEBUG_PRINTLN("Futronix:clearSceneNames"); 
  this->_db->clear(false);
}

/*---------------------------------------*/
void FutronixESP8266::renameScene(int sceneNo, const char* sceneName)
{
  DEBUG_PRINTLN(String("Futronix:renameScene ") + String(sceneNo) + " to " + sceneName); 
  this->_db->setRecord(sceneNo, sceneName);

  //TODO: restart web server 
} 

/*---------------------------------------*/
void FutronixESP8266::setScene(int sceneNo)
{
  DEBUG_PRINTLN(String("Futronix:setScene ") + String(sceneNo)); 
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
  DEBUG_PRINTLN("Futronix:restartWemoServers"); 
}    

/*---------------------------------------*/
void FutronixESP8266::startWemoServers()
{
  DEBUG_PRINTLN("Futronix:startWemoServers"); 
  
  int baseNumberPort = 80; 
  int baseNamePort = baseNumberPort + this->_db->getRecordCount(); 

  //read all records from EEPROM
  DatabaseRecord* records = this->_db->getAllRecords(); 

  //add wemo server/listeners for every scene & scene name 
  for(int n=0; n<this->_db->getRecordCount(); n++)
  {
    //scene numbers 
    this->_wemo->addDevice((String("scene ") + String(n+1)).c_str(), baseNumberPort+n, new SceneNumberCallbackHandler(this->_command, n));

    //scene names 
    if (strlen(records[n].getData()) > 0)
      this->_wemo->addDevice(records[n].getData(), baseNamePort+n, new SceneNameCallbackHandler(this->_command, this->_db, records[n].getData())); 
  }
}

#endif

