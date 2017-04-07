
#ifndef __WIFI_MANAGER_H__
#define __WIFI_MANAGER_H__

#define WIFI_SSID "Mina";
#define WIFI_PASSWD "HappyTime";

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <WiFiServer.h>
#include <WiFiUdp.h>


/****************************************
 * WifiManager
 * -----------
 */
class WifiManager
{
  private: 
    ESP8266WiFiMulti _wiFiMulti;
    char* _wifiPasswd; 
    char* _wifiSsid; 
    bool _enabled = false; 

  public:
    bool isConnected; 
    
  public:
    WifiManager(); 

    void begin(); 
    bool connect(); 
};
/****************************************/

/*---------------------------------------*/
WifiManager::WifiManager()
{
  this->isConnected = false;
  this->_wifiSsid = WIFI_SSID; 
  this->_wifiPasswd = WIFI_PASSWD; 
}

/*---------------------------------------*/
void WifiManager::begin()
{
  this->_enabled = true; 
}

/*---------------------------------------*/
bool WifiManager::connect()
{
  this->_wiFiMulti.addAP(this->_wifiSsid, this->_wifiPasswd);
    
  DEBUG_PRINTLN ("connecting to wifi");
  while(this->_wiFiMulti.run() != WL_CONNECTED) {
    delay(100);
    DEBUG_PRINT (".");
  }
  DEBUG_PRINTLN ("\nconnected");

  this->isConnected = true; 
  return true;
}



#endif
