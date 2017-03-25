#ifndef __UPNPBROADCAST_RESPONDER_H__
#define __UPNPBROADCAST_RESPONDER_H__
 
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUDP.h>
#include "WemoSwitch.h"

class UpnpBroadcastResponder {
private:
        WiFiUDP UDP;  
public:
        UpnpBroadcastResponder();
        ~UpnpBroadcastResponder();
        bool beginUdpMulticast();
        void serverLoop();
        void addDevice(WemoSwitch& device);
};
 
#endif
