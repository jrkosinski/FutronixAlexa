
#ifndef __WIFI_CONNECTION_H__
#define __WIFI_CONNECTION_H__

#define WIFI_SSID     "mina"
#define WIFI_PASSWD   "HappyTime"

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <WiFiServer.h>
#include <WiFiUdp.h>


/****************************************
 * WifiConnection
 * --------------
 * Connects to the Wifi. 
 */
class WifiConnection
{
  private: 
    ESP8266WiFiMulti _wiFiMulti;
    char* _wifiPasswd; 
    char* _wifiSsid; 
    bool _enabled = false; 

  public:
    bool isConnected; 
    
  public:
    WifiConnection(); 

    void begin(); 
    bool connect(); 
};
/****************************************/

/*---------------------------------------*/
WifiConnection::WifiConnection()
{
  this->isConnected = false;
  this->_wifiSsid = WIFI_SSID; 
  this->_wifiPasswd = WIFI_PASSWD; 
}

/*---------------------------------------*/
void WifiConnection::begin()
{
  DEBUG_PRINTLN("Wifi:begin"); 
  this->_enabled = true; 
}

/*---------------------------------------*/
bool WifiConnection::connect()
{
  DEBUG_PRINTLN(String("Wifi:connect ") + this->_wifiSsid + " / " + this->_wifiPasswd); 
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
