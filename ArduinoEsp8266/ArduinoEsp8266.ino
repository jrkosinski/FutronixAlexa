#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <WiFiServer.h>
#include <WiFiUdp.h>


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


//#define __BLANK_OUT_CODE__


#ifndef __BLANK_OUT_CODE__

//diagnostic blinkers
#define LED_PIN   2

void blinkLed(unsigned int);
void blinkLed();
void ledOn();
void ledOff();

// prototypes
boolean connectWifi();

//AWS MQQT
bool mqqtConnect();
char* generateClientID (); 
void messageArrived(MQTT::MessageData&); 
void mqqtSubscribe (); 
void mqqtSendMessage ();

//on/off callbacks
void officeLightsOn();
void officeLightsOff();


// Constants
const char* wifi_ssid              = "mina";
const char* wifi_password          = "HappyTime";
char aws_endpoint[]           = "your-endpoint.iot.eu-west-1.amazonaws.com";
char aws_key[]                = "your-iam-key";
char aws_secret[]             = "your-iam-secret-key";
char aws_region[]             = "eu-west-1";
const char* aws_topic         = "$aws/things/your-device/shadow/update";
const int port                = 443;

//MQTT config
const int maxMQTTpackageSize = 512;
const int maxMQTTMessageHandlers = 1;

//Globals 
boolean wifiConnected = false;
UpnpBroadcastResponder upnpBroadcastResponder;
WemoSwitch *office = NULL;
IRsend irsend(0); //an IR led is connected to GPIO pin 0
FutronixLightController futronix;

//AWS MQQT
ESP8266WiFiMulti wiFiMulti;
AWSWebSocketClient awsWSclient(1000);
IPStack ipstack(awsWSclient);
MQTT::Client<IPStack, Countdown, maxMQTTpackageSize, maxMQTTMessageHandlers> *mqqtClient = NULL;
int arrivedcount = 0;
long connection = 0;


/*****************
   generateClientID
   generate random mqtt clientID
*/
char* generateClientID () {
  char* cID = new char[23]();
  for (int i=0; i<22; i+=1)
    cID[i]=(char)random(1, 256);
  return cID;
}

/*****************
   messageArrived
   callback to handle mqtt messages
*/
void messageArrived(MQTT::MessageData& md)
{
  MQTT::Message &message = md.message;

  Serial.print("Message ");
  Serial.print(++arrivedcount);
  Serial.print(" arrived: qos ");
  Serial.print(message.qos);
  Serial.print(", retained ");
  Serial.print(message.retained);
  Serial.print(", dup ");
  Serial.print(message.dup);
  Serial.print(", packetid ");
  Serial.println(message.id);
  Serial.print("Payload ");
  char* msg = new char[message.payloadlen+1]();
  memcpy (msg,message.payload,message.payloadlen);
  Serial.println(msg);
  delete msg;
}

/*****************
   mqqtConnect
   connects to websocket layer and mqtt layer
*/
bool mqqtConnect () 
{
    if (mqqtClient == NULL) {
      mqqtClient = new MQTT::Client<IPStack, Countdown, maxMQTTpackageSize, maxMQTTMessageHandlers>(ipstack);
    } else {

      if (mqqtClient->isConnected ()) {    
        mqqtClient->disconnect ();
      }  
      delete mqqtClient;
      mqqtClient = new MQTT::Client<IPStack, Countdown, maxMQTTpackageSize, maxMQTTMessageHandlers>(ipstack);
    }

    //delay is not necessary... it just help us to get a "trustful" heap space value
    delay (1000);
    Serial.print (millis ());
    Serial.print (" - conn: ");
    Serial.print (++connection);
    Serial.print (" - (");
    Serial.print (ESP.getFreeHeap ());
    Serial.println (")");


   int rc = ipstack.connect(aws_endpoint, port);
    if (rc != 1)
    {
      Serial.println("error connection to the websocket server");
      return false;
    } else {
      Serial.println("websocket layer connected");
    }


    Serial.println("MQTT connecting");
    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
    data.MQTTVersion = 3;
    char* clientID = generateClientID ();
    data.clientID.cstring = clientID;
    rc = mqqtClient->connect(data);
    delete[] clientID;
    if (rc != 0)
    {
      Serial.print("error connection to MQTT server");
      Serial.println(rc);
      return false;
    }
    Serial.println("MQTT connected");
    return true;
}

/*****************
   mqqtSubscribe
   subscribe to a mqtt topic
*/
void mqqtSubscribe () {
   //subscript to a topic
    int rc = mqqtClient->subscribe(aws_topic, MQTT::QOS0, messageArrived);
    if (rc != 0) {
      Serial.print("rc from MQTT subscribe is ");
      Serial.println(rc);
      return;
    }
    Serial.println("MQTT subscribed");
}

/*****************
   mqqtSendMessage
   send a message to a mqtt topic
*/
void mqqtSendMessage () {
    //send a message
    MQTT::Message message;
    char buf[100];
    strcpy(buf, "{\"state\":{\"reported\":{\"on\": false}, \"desired\":{\"on\": false}}}");
    message.qos = MQTT::QOS0;
    message.retained = false;
    message.dup = false;
    message.payload = (void*)buf;
    message.payloadlen = strlen(buf)+1;
    int rc = mqqtClient->publish(aws_topic, message); 
}


/*****************
   setup
*/
void setup()
{
  Serial.begin(9600);

  //IR 
  irsend.begin();

  //Wemo 
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

  //MQQT
    //fill with ssid and wifi password
    wiFiMulti.addAP(wifi_ssid, wifi_password);
    Serial.println ("connecting to wifi");
    while(wiFiMulti.run() != WL_CONNECTED) {
        delay(100);
        Serial.print (".");
    }
    Serial.println ("\nconnected");

    //fill AWS parameters    
    awsWSclient.setAWSRegion(aws_region);
    awsWSclient.setAWSDomain(aws_endpoint);
    awsWSclient.setAWSKeyID(aws_key);
    awsWSclient.setAWSSecretKey(aws_secret);
    awsWSclient.setUseSSL(true);

    if (mqqtConnect ()){
      mqqtSubscribe ();
      mqqtSendMessage ();
    }
}

/*****************
   loop
*/
void loop()
{
  if (wifiConnected) {
    //Wemo
    upnpBroadcastResponder.serverLoop();

    office->serverLoop();

    //Futronix
    futronix.setSceneInZone(0, 12, &irsend);

    //MQQT
    //keep the mqtt up and running
    if (awsWSclient.connected ()) {    
        mqqtClient->yield();
    } else {
      //handle reconnection
      if (mqqtConnect ()){
        mqqtSubscribe ();      
      }
    }
    
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
  WiFi.begin(wifi_ssid, wifi_password);
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
    Serial.println(wifi_ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }
  else {
    Serial.println("");
    Serial.println("Connection failed.");
  }

  return state;
}


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
  digitalWrite(LED_PIN, LOW);
}

/*****************
   ledOff
*/
void ledOff()
{
  digitalWrite(LED_PIN, HIGH);
}


#else

void setup()
{
}

void loop()
{
}

#endif

