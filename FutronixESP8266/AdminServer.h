#ifndef __ADMIN_SERVER_H__
#define __ADMIN_SERVER_H__

#include <ESP8266WebServer.h>
#include "FutronixESP8266.h"

//TODO: run more than one instance on the wifi network
#define ADMIN_SERVER_PORT 1001 

/****************************************
 * AdminServer
 * -----------
 * Web server that runs to receive commands from any client, for the purpose 
 * of admin tasks like monitoring or managing scene names. 
 */
class AdminServer
{
  private:
    int _localPort = ADMIN_SERVER_PORT;
    ESP8266WebServer* _server = NULL; 
    FutronixESP8266* _futronix = NULL; 
    
  public: 
    AdminServer(FutronixESP8266* futronix);

    void begin(); 

  private: 
    //request handlers 
    void handleGetSceneNames(); 
    void handleClearSceneNames(); 
    void handleRenameScene(); 
    void handleSetScene(); 
    void handleGetCurrentScene(); 
    void handleRestartWemoServers(); 
    void handleSetup(); 
};
/****************************************/

/*---------------------------------------*/
AdminServer::AdminServer(FutronixESP8266* futronix)
{
  this->_futronix = futronix; 
}

/*---------------------------------------*/
void AdminServer::begin()
{
  this->_server = new ESP8266WebServer(this->_localPort);

  this->_server->on("/admin/getSceneNames", [&]() {
    this->handleGetSceneNames();
  });
    
  this->_server->on("/admin/clearSceneNames", [&]() {
    this->handleClearSceneNames();
  });
    
  this->_server->on("/admin/renameScene", [&]() {
    this->handleRenameScene();
  });
    
  this->_server->on("/admin/setScene", [&]() {
    this->handleSetScene();
  });
    
  this->_server->on("/admin/getCurrentScene", [&]() {
    this->handleGetCurrentScene();
  });
    
  this->_server->on("/admin/restartWemoServers", [&]() {
    this->handleRestartWemoServers();
  });
    
  this->_server->on("/admin/setup", [&]() {
    this->handleSetup();
  });

  this->_server->begin();

  DEBUG_PRINTLN(String("AdminServer started on port: ") + _localPort);
}

/*---------------------------------------*/
void AdminServer::handleGetSceneNames()
{
   
}

/*---------------------------------------*/
void AdminServer::handleClearSceneNames()
{
   
}

/*---------------------------------------*/
void AdminServer::handleRenameScene()
{
  String scene = this->_server->arg("scene");   
  String name = this->_server->arg("name"); 

  this->_futronix->renameScene(scene.toInt(), name.c_str());

  this->_server->send(200, "text/plain", "");
}

/*---------------------------------------*/
void AdminServer::handleSetScene()
{
  String scene = this->_server->arg("scene");   

  this->_futronix->setScene(scene.toInt());

  this->_server->send(200, "text/plain", "");
}

/*---------------------------------------*/
void AdminServer::handleGetCurrentScene()
{
  int scene = this->_futronix->getCurrentScene();
  
  this->_server->send(200, "text/plain", String(scene));
}
 
/*---------------------------------------*/
void AdminServer::handleRestartWemoServers()
{
   
}
 
/*---------------------------------------*/
void AdminServer::handleSetup()
{
  //get arguments from querystring 
  String clearDbArg = this->_server->arg("clearDatabase");   
  String wifiSsid = this->_server->arg("wifiSsid");
  String wifiPasswd = this->_server->arg("wifiPasswd");
  
  bool clearDatabase = false;
  if (strcmp(clearDbArg.c_str(), "clearDb") == 0)
    clearDatabase = true;

  //run setup
  this->_futronix->setup(wifiSsid.c_str(), wifiPasswd.c_str(), clearDatabase);
}


#endif
