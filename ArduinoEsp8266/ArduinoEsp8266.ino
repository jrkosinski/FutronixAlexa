
//ESP8266/Arduino base libs
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include <Stream.h>

//WEMO
#include "UpnpBroadcastResponder.h"
#include "UpnpBroadcastResponder.h"
#include "CallbackFunction.h"

//Futronix
#include "FutronixLightController.h"

//IR
#include <IRremoteESP8266.h>

//WEBSockets
#include <WebSockets.h>
#include <WebSocketsClient.h>
#include <Hash.h>

//AWS SDK
#include <sha256.h>
#include <Utils.h>
#include <AWSClient2.h>

//MQTT PAHO
#include <SPI.h>
#include <IPStack.h>
#include <Countdown.h>
#include <MQTTClient.h>

//AWS MQTT Websocket
#include <Client.h>
#include <AWSWebSocketClient.h>
#include <CircularByteBuffer.h>


/*
   try to connect to a service
   find out convention for pin
*/


//#define __BLANK_OUT_CODE__
//#define __BLINKERS_ONLY__


#ifdef __BLANK_OUT_CODE__
void setup()
{
}

void loop()
{
}

#else

//diagnostic blinkers
int _ledPin = 2;

void blinkLed(unsigned int);
void blinkLed();
void ledOn();
void ledOff();

#ifdef __BLINKERS_ONLY__

void setup()
{
  pinMode(_ledPin, OUTPUT);
}

void loop()
{
  blinkLed();
}
#else

// prototypes
boolean connectWifi();

//on/off callbacks
void officeLightsOn();
void officeLightsOff();

// Change this before you flash
const char* ssid = "mina";
const char* password = "HappyTime";

boolean wifiConnected = false;

UpnpBroadcastResponder upnpBroadcastResponder;

WemoSwitch *office = NULL;
IRsend irsend(0); //an IR led is connected to GPIO pin 0
FutronixLightController futronix;

/*****************
   setup
*/
void setup()
{
  //pinMode(_ledPin, OUTPUT);
  //ledOff();
  irsend.begin();

  Serial.begin(9600);
  // Initialise wifi connection
  wifiConnected = connectWifi();

  if (wifiConnected) {
    upnpBroadcastResponder.beginUdpMulticast();

    // Define your switches here. Max 14
    // Format: Alexa invocation name, local port no, on callback, off callback.
    office = new WemoSwitch("office lights", 80, officeLightsOn, officeLightsOff);

    Serial.println("Adding switches upnp broadcast responder");
    upnpBroadcastResponder.addDevice(*office);
  }
  else
  {
    //ledOff();
  }
}

/*****************
   loop
*/
void loop()
{
  if (wifiConnected) {
    upnpBroadcastResponder.serverLoop();

    office->serverLoop();

    futronix.setSceneInZone(0, 12, &irsend);

    delay(2000);
  }
  else
  {
    //ledOff();
    Serial.print("Wifi not connected ...");
  }
}

/*****************
   officeLightsOn
*/
void officeLightsOn() {
  Serial.print("Switch 1 turn on ...");
}

/*****************
   officeLightsOff
*/
void officeLightsOff() {
  Serial.print("Switch 1 turn off ...");
}

/*****************
   connectWifi
   connect to wifi – returns true if successful or false if not
*/
boolean connectWifi() {
  boolean state = true;
  int i = 0;

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
  Serial.println("Connecting to WiFi");

  // Wait for connection
  Serial.print("Connecting ...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (i > 10) {
      state = false;
      break;
    }
    i++;
  }

  if (state) {
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }
  else {
    Serial.println("");
    Serial.println("Connection failed.");
  }

  return state;
}

#endif

/*****************
   blinkLed
*/
void blinkLed()
{
  blinkLed(1);
}

/*****************
   blinkLed
*/
void blinkLed(unsigned int count)
{
  for (int n = 0; n < count; n++)
  {
    ledOn();
    delay(500);                // wait for a second
    ledOff();
    delay(500);                // wait for a second
  }
}

/*****************
   ledOn
*/
void ledOn()
{
  digitalWrite(_ledPin, LOW);
}

/*****************
   ledOff
*/
void ledOff()
{
  digitalWrite(_ledPin, HIGH);
}


#endif

