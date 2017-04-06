
#ifndef __WIFI_MANAGER_H__
#define __WIFI_MANAGER_H__


#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <WiFiServer.h>
#include <WiFiUdp.h>


class WifiManager
{
  public:
    WifiManager(char* ssid, char* password); 

    bool isConnected; 

    void begin(); 
    bool connect(); 

  private: 
    ESP8266WiFiMulti _wiFiMulti;
    char* _wifiPasswd; 
    char* _wifiSsid; 
    bool _enabled = false; 
};

WifiManager::WifiManager(char* ssid, char* password)
{
  this->isConnected = false;
  this->_wifiSsid = ssid; 
  this->_wifiPasswd = password; 
}

void WifiManager::begin()
{
  this->_enabled = true; 
}

bool WifiManager::connect()
{
  this->_wiFiMulti.addAP(this->_wifiSsid, this->_wifiPasswd);
    
  Serial.println ("connecting to wifi");
  while(this->_wiFiMulti.run() != WL_CONNECTED) {
    delay(100);
    Serial.print (".");
  }
  Serial.println ("\nconnected");

  this->isConnected = true; 
  return true;
}



#endif
