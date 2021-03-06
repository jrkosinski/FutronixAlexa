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

    /* for AWS MQTT */ 
    AwsManager* _aws; 

    /* last set scene */
    int _currentScene = -1; 
    
    char* _scenes[20] = {"scene one", "scene two", "scene three", "scene four", "scene five", "scene six", "scene seven", "scene eight", "scene nine", "scene ten", "scene eleven", "scene twelve", "scene thirteen", "scene fourteen", "scene fifteen", "scene sixteen", "scene seventeen", "scene eighteen", "scene nineteen", "scene twenty" };

  public: 
    FutronixESP8266();

    void begin();
    void loop();

    unsigned int getSceneCount();

    /***
     * returns a value indicating whether the DB on the chip 
     * has been initialized or not
     */
    bool hasBeenSetUp();

    /***
     * does initial setup of DB & chip 
     */
    void setup(const char* wifiSsid, const char* wifiPasswd, bool clearDatabase); 

    /***
     * gets entire contens of scene-names DB
     */
    void getAllSceneNames(char** buffer); 

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

    void queueSetScene(int scene); 

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
  this->_aws = new AwsManager(); 
}

/*---------------------------------------*/
bool FutronixESP8266::hasBeenSetUp()
{
  return this->_db->hasBeenSetUp();
}

/*---------------------------------------*/
void FutronixESP8266::begin()
{
  DEBUG_PRINTLN("Futronix:begin"); 
  
  //this->_led->begin();
  this->_wifi->begin();
  this->_command->begin();
  this->_db->begin(); 

  if (!this->_db->hasBeenSetUp())
    this->setup("mina", "HappyTime", true);

  if (this->_wifi->connect())
  {
    //start wemo
    this->_wemo->begin();
    this->startWemoServers();

    //start AWS service
    //this->_aws->begin();
    //this->_aws->connectAndListen();
  }
}

/*---------------------------------------*/
void FutronixESP8266::loop()
{
  this->_wemo->listen();
}

/*---------------------------------------*/
unsigned int FutronixESP8266::getSceneCount()
{
  return this->_db->getSceneCount();
}

/*---------------------------------------*/
void FutronixESP8266::setup(const char* wifiSsid, const char* wifiPasswd, bool clearDatabase)
{
  DEBUG_PRINTLN(String("Futronix:setup ") + wifiSsid + "  " + wifiPasswd); 
  //clear database 
  if (clearDatabase)
  {
    this->_db->clear(true);
  }

  //set wifi name & passwd
  this->_db->setWifiSsid(wifiSsid);
  this->_db->setWifiPasswd(wifiPasswd);

  this->_db->save();
}

/*---------------------------------------*/
void FutronixESP8266::getAllSceneNames(char** buffer)
{
  DEBUG_PRINTLN("Futronix:getAllSceneNames"); 
  int count = this->_db->getSceneCount();
  for(int n=0; n<count; n++)
  {
    buffer[n] = this->_db->getSceneName(n); 
  }
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
  this->_db->setSceneName(sceneNo-1, sceneName);
  this->_db->save();

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
  int baseNamePort = baseNumberPort + this->_db->getSceneCount(); 

  for(int n=0; n<this->_db->getSceneCount(); n++)
  {
    this->_wemo->addDevice(this->_scenes[n], 0, new SceneCallbackHandler(n)); 
  }
}

/*---------------------------------------*/
void FutronixESP8266::queueSetScene(int scene)
{
  SCENE_TO_SET = scene; 
}


#endif

