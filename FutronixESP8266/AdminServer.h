#ifndef __ADMIN_SERVER_H__
#define __ADMIN_SERVER_H__

#include <WiFiUDP.h>
#include <ESP8266WebServer.h>
#include "FutronixESP8266.h"
#include "CommandInterface.h"

//TODO: run more than one instance on the wifi network
#define ADMIN_SERVER_PORT 80 
#define ADMIN_UDP_PORT    2001 

/****************************************
 * AdminServer
 * -----------
 * Web server that runs to receive commands from any client, for the purpose 
 * of admin tasks like monitoring or managing scene names. 
 */
class AdminServer
{
  private:
    int _localTcpPort = ADMIN_SERVER_PORT;
    int _localUdpPort = ADMIN_UDP_PORT;
    ESP8266WebServer* _server = NULL; 
    FutronixESP8266* _futronix = NULL; 
    WiFiUDP _udp;  
    char _packetBuffer[512];
    
  public: 
    AdminServer(FutronixESP8266* futronix);

    void begin(); 
    void listen();

  private: 
    //request handlers 
    void handleGetSceneNames(); 
    void handleClearSceneNames(); 
    void handleRenameScene(); 
    void handleSetScene(); 
    void handleGetCurrentScene(); 
    void handleRestartWemoServers(); 
    void handleSetup(); 
    void handleStatus(); 
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
  this->_server = new ESP8266WebServer(this->_localTcpPort);

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
    
  this->_server->on("/admin/status", [&]() {
    this->handleStatus();
  });

  this->_server->begin();

  DEBUG_PRINTLN(String("AdminServer: started on port ") + _localTcpPort);

  //open UDP port
  this->_udp.begin(this->_localUdpPort); 
}

/*---------------------------------------*/
void AdminServer::listen()
{
  int packetSize = this->_udp.parsePacket();
  if (packetSize)
  {
    Serial.printf("AdminServer:listen Received %d bytes from %s, port %d\n", packetSize, this->_udp.remoteIP().toString().c_str(), this->_udp.remotePort());
    int len = this->_udp.read(this->_packetBuffer, 255);
    Serial.printf("AdminServer: UDP packet contents: %s\n", this->_packetBuffer);
    
    if (len > 0)
    {
      this->_packetBuffer[len] = 0;

      //if it's the right type of request...
      if (strcmp(this->_packetBuffer, MAGIC_WORD) == 0)
      {
        //...send a response 
        /*
        DEBUG_PRINTLN("AdminServer: send response"); 
        this->_udp.beginPacket(this->_udp.remoteIP(), 2003);
        this->_udp.write(MAGIC_WORD);
        this->_udp.endPacket();
        */
          
        WiFiClient client; 
        DEBUG_PRINTLN(String("AdminServer: connecting to ") + this->_udp.remoteIP().toString().c_str());
        //if (client.connect(this->_udp.remoteIP(), this->_udp.remotePort()))
        //{
        //  DEBUG_PRINTLN(String("AdminServer: connected to ") + this->_udp.remoteIP().toString().c_str());
        //  client.print("80");
        //}

        this->_udp.beginPacket(this->_udp.remoteIP(), this->_udp.remotePort()); 
        this->_udp.write("80"); 
        this->_udp.endPacket();
      }
    }
  }
    
  this->_server->handleClient();
}

/*---------------------------------------*/
void AdminServer::handleGetSceneNames()
{
  int sceneCount = this->_futronix->getSceneCount(); 
  char* scenes[sceneCount]; 
  this->_futronix->getAllSceneNames(scenes); 

  char responseBuffer[100 * sceneCount]; 
  char tempBuffer[100]; 
  int bufferIndex = 0; 
  for(int n=0; n<sceneCount; n++)
  {
    sprintf(tempBuffer, "%d:%s\n", (n+1), scenes[n]); 
    strcpy(responseBuffer+bufferIndex, tempBuffer); 
    bufferIndex+= strlen(tempBuffer); 
  }

  DEBUG_PRINTLN(String("AdminServer:handleGetSceneNames sending ") + responseBuffer); 
  this->_server->send(200, "text/plain", responseBuffer);
}

/*---------------------------------------*/
void AdminServer::handleClearSceneNames()
{
  this->_futronix->clearSceneNames();
  this->_server->send(200, "text/plain", "");
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

  //this->_futronix->setScene(scene.toInt());
  this->_futronix->queueSetScene(scene.toInt());

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
  if (strcmp(clearDbArg.c_str(), "true") == 0)
    clearDatabase = true;

  //run setup
  this->_futronix->setup(wifiSsid.c_str(), wifiPasswd.c_str(), clearDatabase);
  this->_server->send(200, "text/plain", "");
}
 
/*---------------------------------------*/
void AdminServer::handleStatus()
{
  DEBUG_PRINTLN("AdminServer:got status request"); 
  if (this->_futronix->hasBeenSetUp())
    this->_server->send(200, "text/plain", "");
  else 
    this->_server->send(404, "text/plain", ""); 
}


#endif
