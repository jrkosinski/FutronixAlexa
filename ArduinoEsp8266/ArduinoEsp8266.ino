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
void sceneOn(int);
void sceneOff(int); 
void scene1On();
void scene1Off();
void scene2On();
void scene2Off();
void scene3On();
void scene3Off();
void scene4On();
void scene4Off();
void scene5On();
void scene5Off();
void scene6On();
void scene6Off();
void scene7On();
void scene8Off();




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
WemoSwitch *scene1Server = NULL;
WemoSwitch *scene2Server = NULL;
WemoSwitch *scene3Server = NULL;
WemoSwitch *scene4Server = NULL;
WemoSwitch *scene5Server = NULL;
WemoSwitch *scene6Server = NULL;
WemoSwitch *scene7Server = NULL;
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
    scene1Server = new WemoSwitch("scene one",    81, scene1On, scene1Off);
    scene2Server = new WemoSwitch("scene two",    82, scene2On, scene2Off);
    scene3Server = new WemoSwitch("scene three",  83, scene3On, scene3Off);
    scene4Server = new WemoSwitch("scene four",   84, scene4On, scene4Off);
    scene5Server = new WemoSwitch("scene five",   85, scene5On, scene5Off);
    scene6Server = new WemoSwitch("scene six",    86, scene6On, scene6Off);
    scene7Server = new WemoSwitch("scene seven",  87, scene7On, scene7Off);

    Serial.println("Adding switches upnp broadcast responder");
    upnpBroadcastResponder.addDevice(*scene1Server);
    upnpBroadcastResponder.addDevice(*scene2Server);
    upnpBroadcastResponder.addDevice(*scene3Server);
    upnpBroadcastResponder.addDevice(*scene4Server);
    upnpBroadcastResponder.addDevice(*scene5Server);
    upnpBroadcastResponder.addDevice(*scene6Server);
    upnpBroadcastResponder.addDevice(*scene7Server);
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

    scene1Server->serverLoop();
    scene2Server->serverLoop();
    scene3Server->serverLoop();
    scene4Server->serverLoop();
    scene5Server->serverLoop();
    scene6Server->serverLoop();
    scene7Server->serverLoop();

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

void scene1On() {
  Serial.print("Scene 1 turn on ...");
  sceneOn(1); 
}
void scene1Off() {
  Serial.print("Scene 1 turn off ...");
  sceneOff(1); 
}
void scene2On() {
  Serial.print("Scene 2 turn on ...");
  sceneOn(2); 
}
void scene2Off() {
  Serial.print("Scene 2 turn off ...");
  sceneOff(2); 
}
void scene3On() {
  Serial.print("Scene 3 turn on ...");
  sceneOn(3); 
}
void scene3Off() {
  Serial.print("Scene 3 turn off ...");
  sceneOff(3); 
}
void scene4On() {
  Serial.print("Scene 4 turn on ...");
  sceneOn(4); 
}
void scene4Off() {
  Serial.print("Scene 4 turn off ...");
  sceneOff(4); 
}
void scene5On() {
  Serial.print("Scene 5 turn on ...");
  sceneOn(5); 
}
void scene5Off() {
  Serial.print("Scene 5 turn off ...");
  sceneOff(5); 
}
void scene6On() {
  Serial.print("Scene 6 turn on ...");
  sceneOn(6); 
}
void scene6Off() {
  Serial.print("Scene 6 turn off ...");
  sceneOff(6); 
}
void scene7On() {
  Serial.print("Scene 7 turn on ...");
  sceneOn(7); 
}
void scene7Off() {
  Serial.print("Scene 7 turn off ...");
  sceneOff(7); 
}

void sceneOn(int scene)  {
  //Serial.print("Scene %d turn on...", scene); 
  ledOn(); 
}

void sceneOff(int scene)  {
  //Serial.print("Scene %d turn off...", scene); 
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


