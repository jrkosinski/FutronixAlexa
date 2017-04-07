#ifndef __ADMIN_SERVER_H__
#define __ADMIN_SERVER_H__

#include <ESP8266WebServer.h>

//TODO: run more than one instance on the wifi network
#define ADMIN_SERVER_PORT 1001 

/****************************************
 * AdminServer
 * -----------
 */
class AdminServer
{
  private 
    int _localPort = ADMIN_SERVER_PORT;
    ESP8266WebServer* _server = NULL; 
    
  public: 
    AdminServer();

    begin(); 

  private: 
    //request handlers 
    void handleGetSceneNames(); 
    void handleClearSceneNames(); 
    void handleRenameScene(); 
    void handleSetScene(); 
    void handleGetCurrentScene(); 
    void handleRestartWemoServers(); 
};
/****************************************/

AdminServer::AdminServer()
{
  
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

  this->_server->begin();

  Serial.println("AdminServer started on port: ");
  Serial.println(_localPort);
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
   
}

/*---------------------------------------*/
void AdminServer::handleSetScene()
{
   
}

/*---------------------------------------*/
void AdminServer::handleGetCurrentScene()
{
   
}
 
/*---------------------------------------*/
void AdminServer::handleRestartWemoServers()
{
   
}


#endif
