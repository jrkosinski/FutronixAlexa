#ifndef __WEMO_EMULATOR_H__
#define __WEMO_EMULATOR_H__

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUDP.h>
#include <ESP8266WebServer.h>
#include <functional> 

#include "WemoServer.h"
#include "Fauxmo.h"
#include "CommandInterface.h"

#define SERVER_COUNT_LIMIT 40

//#define __OLD_WEMO_VERSION__
#ifndef __OLD_WEMO_VERSION__

/****************************************
 * WemoEmulator
 * ------------
 * Handles the wemo-emulating servers, and also the initial UDP request 
 * from the Alexa, scanning for devices on the network. 
 */
class WemoEmulator
{
  private:
    int _serverCount;
    bool _enabled = false; 
    Fauxmo _fauxmo; 
    
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
}

/*---------------------------------------*/
void WemoEmulator::begin()
{
  this->isRunning = false;
  this->_enabled = true; 
  
  DEBUG_PRINTLN("WemoEmulator:begin");

  this->_fauxmo.addDevice("scene one"); 
  this->_fauxmo.addDevice("scene two"); 
  this->_fauxmo.addDevice("scene three"); 
  this->_fauxmo.addDevice("scene four"); 
  this->_fauxmo.addDevice("scene five"); 
  this->_fauxmo.addDevice("scene six"); 
  this->_fauxmo.addDevice("scene seven"); 
  this->_fauxmo.addDevice("scene eight"); 
  this->_fauxmo.addDevice("scene nine"); 
  this->_fauxmo.addDevice("scene ten"); 
  this->_fauxmo.addDevice("scene eleven"); 
  this->_fauxmo.addDevice("scene twelve"); 
  this->_fauxmo.addDevice("scene thirteen"); 
  this->_fauxmo.addDevice("scene fourteen"); 
  this->_fauxmo.addDevice("scene fifteen"); 
  this->_fauxmo.addDevice("scene sixteen"); 
  this->_fauxmo.addDevice("scene seventeen"); 
  this->_fauxmo.addDevice("scene eighteen"); 
  this->_fauxmo.addDevice("scene nineteen"); 
 
  
  this->_fauxmo.onMessage([](unsigned char device_id, const char * device_name, bool state) {
    Serial.printf("[MAIN] Device #%d (%s) state: %s\n", device_id, device_name, state ? "ON" : "OFF");

        if (strcmp(device_name, "scene one") == 0)
        {
          DEBUG_PRINTLN("Alexa scene 1"); 
          CommandInterface::setSceneStatic(1);
        }
        if (strcmp(device_name, "scene two") == 0)
        {
          DEBUG_PRINTLN("Alexa scene 2"); 
          CommandInterface::setSceneStatic(2);
        }
        if (strcmp(device_name, "scene three") == 0)
        {
          DEBUG_PRINTLN("Alexa scene 3"); 
          CommandInterface::setSceneStatic(3);
        }
        if (strcmp(device_name, "scene four") == 0)
        {
          DEBUG_PRINTLN("Alexa scene 4"); 
          CommandInterface::setSceneStatic(4);
        }
        if (strcmp(device_name, "scene five") == 0)
        {
          DEBUG_PRINTLN("Alexa scene 5"); 
          CommandInterface::setSceneStatic(5);
        }
        if (strcmp(device_name, "scene six") == 0)
        {
          DEBUG_PRINTLN("Alexa scene 6"); 
          CommandInterface::setSceneStatic(6);
        }
        if (strcmp(device_name, "scene seven") == 0)
        {
          DEBUG_PRINTLN("Alexa scene 7"); 
          CommandInterface::setSceneStatic(7);
        }
    });
}

/*---------------------------------------*/
void WemoEmulator::listen()
{
  if (!this->_enabled)
    return; 

  this->_fauxmo.handle();
}

/*---------------------------------------*/
void WemoEmulator::stop()
{
}

/*---------------------------------------*/
bool WemoEmulator::addDevice(const char* deviceName, int localPort, IWemoCallbackHandler* callbackHandler)
{
  if (!this->_enabled)
    return false; 
}

/*---------------------------------------*/
bool WemoEmulator::replaceDevice(const char* deviceName, const char* newDeviceName, IWemoCallbackHandler* callbackHandler)
{
  if (!this->_enabled)
    return false; 
    
  return false;
}

/*---------------------------------------*/
int WemoEmulator::getDeviceIndexByName(const char* deviceName)
{
  return -1; 
}

#endif 


























#ifdef __OLD_VERSION__
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
  
  DEBUG_PRINTLN("WemoEmulator:begin");
  
  IPAddress ipMulti(239, 255, 255, 250);
  if (this->_udp.beginMulticast(WiFi.localIP(), ipMulti, this->_portMulti)) 
  {
    DEBUG_PRINTLN("Udp multicast server started at ");
    DEBUG_PRINTLN(ipMulti);
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
    
  //DEBUG_PRINTLN("WemoEmulator:listen");
  
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

  for(int n=0; n<this->_serverCount; n++)
  {
    if (this->_servers[n] != NULL)
      this->_servers[n]->listen();
  }
}

/*---------------------------------------*/
void WemoEmulator::stop()
{
  if (!this->_enabled)
    return; 
    
  DEBUG_PRINTLN("WemoEmulator:stop");
    
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
    
  DEBUG_PRINTLN(String("WemoEmulator:addDevice ") + deviceName + " port:" + String(localPort));
  
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
    
  DEBUG_PRINTLN(String("WemoEmulator:replaceDevice ") + deviceName + " to:" + newDeviceName);
    
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

#endif
