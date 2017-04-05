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

class WemoEmulator
{
  public:
    WemoEmulator();
    ~WemoEmulator();

    void begin();
    void listen();
    bool addDevice(char* deviceName, int localPort, IWemoCallbackHandler* callbackHandler);
    bool isRunning; 

  private:
    WemoServer* _servers[SERVER_COUNT_LIMIT];
    int _serverCount;
    const unsigned int _portMulti = 1900;
    char _packetBuffer[512];
    WiFiUDP _udp;  
};

WemoEmulator::WemoEmulator()
{
  this->_serverCount = 0;
  this->isRunning = false;
}

WemoEmulator::~WemoEmulator()
{
  for (int n = 0; n < this->_serverCount; n++)
  {
    delete this->_servers[n];
  }
  this->_serverCount = 0;
}

void WemoEmulator::begin()
{
  this->isRunning = false;
  
  Serial.println("Begin multicast ..");
  
  if(this->_udp.beginMulticast(WiFi.localIP(), _ipMulti, this->_portMulti)) {
    Serial.print("Udp multicast server started at ");
    Serial.print(_ipMulti);
    Serial.print(":");
    Serial.println(this->_portMulti);

    this->isRunning = true;
  }
  else{
    Serial.println("Connection failed");
  }
}

void WemoEmulator::listen()
{
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
      Serial.println("Got UDP Belkin Request..");

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

bool WemoEmulator::addDevice(char* deviceName, int localPort, IWemoCallbackHandler* callbackHandler)
{
  if (this->_serverCount < SERVER_COUNT_LIMIT)
  {
    this->_servers[this->_serverCount] = new WemoServer(deviceName, localPort, callbackHandler);
    this->_serverCount++;
  }

  return false;
}

#endif
