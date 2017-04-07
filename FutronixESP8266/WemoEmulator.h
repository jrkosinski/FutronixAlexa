#ifndef __WEMO_EMULATOR_H__
#define __WEMO_EMULATOR_H__

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUDP.h>
#include <ESP8266WebServer.h>
#include <functional> 

#include "WemoServer.h"

#define SERVER_COUNT_LIMIT 20

IPAddress _ipMulti(239, 255, 255, 250);

/****************************************
 * WemoEmulator
 * ------------
 * Handles the wemo-emulating servers, and also the initial UDP request 
 * from the Alexa, scanning for devices on the network. 
 */
class WemoEmulator
{
  private:
    WiFiUDP _udp;  
    WemoServer* _servers[SERVER_COUNT_LIMIT];
    const unsigned int _portMulti = 1900;
    char _packetBuffer[512];
    int _serverCount;
    bool _enabled = false; 
    
  public:
    bool isRunning; 
    
  public:
    WemoEmulator();
    ~WemoEmulator();

    void begin();
    void listen();
    void stop();
    bool addDevice(const char* deviceName, int localPort, IWemoCallbackHandler* callbackHandler);
    bool replaceDevice(const char* deviceName, const char* newDeviceName, IWemoCallbackHandler* callbackHandler);
    int getDeviceIndexByName(const char* deviceName);
};
/****************************************/

/*---------------------------------------*/
WemoEmulator::WemoEmulator()
{
  this->_serverCount = 0;
  this->isRunning = false;
}

/*---------------------------------------*/
WemoEmulator::~WemoEmulator()
{
  for (int n = 0; n < this->_serverCount; n++)
  {
    delete this->_servers[n];
  }
  this->_serverCount = 0;
}

/*---------------------------------------*/
void WemoEmulator::begin()
{
  this->isRunning = false;
  this->_enabled = true; 
  
  DEBUG_PRINTLN("Begin multicast ..");
  
  if (this->_udp.beginMulticast(WiFi.localIP(), _ipMulti, this->_portMulti)) 
  {
    DEBUG_PRINTLN("Udp multicast server started at ");
    DEBUG_PRINTLN(_ipMulti);
    DEBUG_PRINTLN(":");
    DEBUG_PRINTLN(this->_portMulti);

    this->isRunning = true;
  }
  else
  {
    DEBUG_PRINTLN("Connection failed");
  }
}

/*---------------------------------------*/
void WemoEmulator::listen()
{
  if (!this->_enabled)
    return; 
    
  int packetSize = this->_udp.parsePacket();
  if (packetSize <= 0)
    return;

  IPAddress senderIP = this->_udp.remoteIP();
  unsigned int senderPort = this->_udp.remotePort();

  // read the packet into the buffer
  this->_udp.read(_packetBuffer, packetSize);

  // check if this is a M-SEARCH for WeMo device
  String request = String((char*)_packetBuffer);

  if (request.indexOf('M-SEARCH') > 0) {
    if (request.indexOf("urn:Belkin:device:**") > 0) {
      DEBUG_PRINTLN("Got UDP Belkin Request..");

      for (int n = 0; n < this->_serverCount; n++) {
        WemoServer* server = _servers[n];

        if (server != NULL) {
          server->respondToSearch(senderIP, senderPort);
        }
      }
    }
  }

  for(int n = 0; n < this->_serverCount; n++) {
    WemoServer* server = _servers[n];

    if (server != NULL) {
      server->listen();
    }
  }
}

/*---------------------------------------*/
void WemoEmulator::stop()
{
  if (!this->_enabled)
    return; 
    
  for(int n = 0; n < this->_serverCount; n++) {
    WemoServer* server = _servers[n];

    if (server != NULL) {
      server->stop();
    }
  }
}

/*---------------------------------------*/
bool WemoEmulator::addDevice(const char* deviceName, int localPort, IWemoCallbackHandler* callbackHandler)
{
  if (!this->_enabled)
    return false; 
    
  if (this->_serverCount < SERVER_COUNT_LIMIT)
  {
    this->_servers[this->_serverCount] = new WemoServer(deviceName, localPort, callbackHandler);
    this->_serverCount++;
  }

  return false;
}

/*---------------------------------------*/
bool WemoEmulator::replaceDevice(const char* deviceName, const char* newDeviceName, IWemoCallbackHandler* callbackHandler)
{
  if (!this->_enabled)
    return false; 
    
  int index = this->getDeviceIndexByName(deviceName); 
  if (index >= 0)
  {
    WemoServer* server = this->_servers[index]; 
    server->stop(); 

    this->_servers[index] = new WemoServer(newDeviceName, server->getLocalPort(), callbackHandler); 
    delete server;
  }

  return false;
}

/*---------------------------------------*/
int WemoEmulator::getDeviceIndexByName(const char* deviceName)
{
  if (!this->_enabled)
    return -1; 
    
  for(int n = 0; n < this->_serverCount; n++) 
  {
    WemoServer* server = _servers[n];

    if (server != NULL)
    {
      if (strcmp(server->getDeviceName(), deviceName) == 0)
      {
        return n;
      }
    }
  }
  
  return -1; 
}

#endif
