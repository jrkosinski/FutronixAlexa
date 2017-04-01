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




#define __TEST_WEMO__
//#define __TEST_IR__
//#define __TEST_AWS__

#define   LED_PIN   2



// prototypes
void blinkLed(unsigned int);
void blinkLed();
void ledOn();
void ledOff();

boolean connectWifi();

//AWS MQTT
bool mqttConnect();
char* generateClientID (); 
void messageArrived(MQTT::MessageData&); 
void mqttSubscribe (); 
void mqttSendMessage ();

//on/off callbacks
void officeLightsOn();
void officeLightsOff();




// Constants
const char* wifi_ssid              = "mina";
const char* wifi_password          = "HappyTime";

char aws_endpoint[]           = "a21jd7gud1swyd.iot.us-east-1.amazonaws.com";
char aws_key[]                = "AKIAIS7JT5B2C5SIKVQQ"; 
char aws_secret[]             = "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";
char aws_region[]             = "eu-east-1";
const char* aws_topic         = "$aws/things/Sigma/shadow/update";
const int port                = 443;



//MQTT config
const int maxMQTTpackageSize = 512;
const int maxMQTTMessageHandlers = 1;

int _irCount = 0; 

//Globals 
boolean wifiConnected = false;
UpnpBroadcastResponder upnpBroadcastResponder;
WemoSwitch *office = NULL;
IRsend irsend(0); //an IR led is connected to GPIO pin 0
FutronixLightController futronix;



//AWS MQTT
ESP8266WiFiMulti wiFiMulti;
AWSWebSocketClient awsWSclient(1000);
IPStack ipstack(awsWSclient);
MQTT::Client<IPStack, Countdown, maxMQTTpackageSize, maxMQTTMessageHandlers> *mqttClient = NULL;
int arrivedcount = 0;
long connection = 0;



/*********************************************************************************************************************************************************/
/*********************************************************************************************************************************************************/


/*****************
   setup
*/
void setup()
{
  Serial.begin(9600);
  //Serial.setDebugOutput(1);
  //Serial.begin (115200);
  configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");

  pinMode(LED_PIN, OUTPUT); 
  ledOn();
  
  //IR 
  #ifdef __TEST_IR__
  Serial.println("testing IR"); 
  //irsend.begin();
  #endif 

  
  //WEMO 
  #ifdef __TEST_WEMO__
  Serial.println("testing WEMO"); 
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
  #endif
  

  //MQTT
  #ifdef __TEST_AWS__
  Serial.println("testing AWS"); 
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

    if (mqttConnect ()){
      blinkLed(3);
      mqttSubscribe ();
      mqttSendMessage ();
    }
  #endif
}

/*****************
   loop
*/
void loop()
{
  //IR 
  #ifdef __TEST_IR__

  if (_irCount < 10)
  {
    futronix.setSceneInZone(0, 2, &irsend);
    futronix.setSceneInZone(1, 2, &irsend);
    futronix.setSceneInZone(2, 2, &irsend);
    futronix.setSceneInZone(3, 2, &irsend);
    futronix.setSceneInZone(4, 2, &irsend);
    futronix.setSceneInZone(5, 2, &irsend);
    futronix.setSceneInZone(6, 2, &irsend);
    futronix.setSceneInZone(7, 2, &irsend);
    futronix.setSceneInZone(8, 2, &irsend);
    futronix.setSceneInZone(9, 2, &irsend);
    futronix.setSceneInZone(10, 2, &irsend);
    futronix.setSceneInZone(11, 2, &irsend);
    futronix.setSceneInZone(12, 2, &irsend);
    _irCount++; 
    blinkLed();
    delay(5000); 
  }
  #endif 


  #ifdef __TEST_WEMO__
  if (wifiConnected) {
    //Wemo
    upnpBroadcastResponder.serverLoop();

    office->serverLoop();

    //Futronix
    //futronix.setSceneInZone(0, 12, &irsend);
  }
  else
  {
    //ledOff();
    //Serial.println("Wifi not connected ...");
  }
  #endif
}


/*********************************************************************************************************************************************************/


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
   mqttConnect
   connects to websocket layer and mqtt layer
*/
bool mqttConnect () 
{
    if (mqttClient == NULL) {
      mqttClient = new MQTT::Client<IPStack, Countdown, maxMQTTpackageSize, maxMQTTMessageHandlers>(ipstack);
    } else {

      if (mqttClient->isConnected ()) {    
        Serial.println("Connected already; disconnecting..."); 
        mqttClient->disconnect ();
      }  
      delete mqttClient;
      mqttClient = new MQTT::Client<IPStack, Countdown, maxMQTTpackageSize, maxMQTTMessageHandlers>(ipstack);
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
      Serial.println(rc);
      return false;
    } else {
      Serial.println("websocket layer connected");
    }


    Serial.println("MQTT connecting");
    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
    data.MQTTVersion = 3;
    char* clientID = generateClientID ();
    data.clientID.cstring = clientID;
    rc = mqttClient->connect(data);
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
   mqttSubscribe
   subscribe to a mqtt topic
*/
void mqttSubscribe () {
   //subscript to a topic
    int rc = mqttClient->subscribe(aws_topic, MQTT::QOS0, messageArrived);
    if (rc != 0) {
      Serial.print("rc from MQTT subscribe is ");
      Serial.println(rc);
      return;
    }
    Serial.println("MQTT subscribed");
}

/*****************
   mqttSendMessage
   send a message to a mqtt topic
*/
void mqttSendMessage () {
    //send a message
    MQTT::Message message;
    char buf[100];
    strcpy(buf, "{\"state\":{\"reported\":{\"on\": false}, \"desired\":{\"on\": false}}}");
    message.qos = MQTT::QOS0;
    message.retained = false;
    message.dup = false;
    message.payload = (void*)buf;
    message.payloadlen = strlen(buf)+1;
    int rc = mqttClient->publish(aws_topic, message); 
}

/*****************
   officeLightsOn
*/
void officeLightsOn() {
  Serial.print("Switch 1 turn on ...");
  ledOn();
}

/*****************
   officeLightsOff
*/
void officeLightsOff() {
  Serial.print("Switch 1 turn off ...");
  ledOff();
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


