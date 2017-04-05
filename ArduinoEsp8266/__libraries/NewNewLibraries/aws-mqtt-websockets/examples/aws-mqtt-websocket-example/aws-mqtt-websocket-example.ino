#include <ESP8266WiFi.h>
#include <ESP8266WiFiAP.h>
#include <ESP8266WiFiGeneric.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WiFiScan.h>
#include <ESP8266WiFiSTA.h>
#include <ESP8266WiFiType.h>
#include "WiFiClient.h"
#include "WiFiClientSecure.h"
#include <WiFiServer.h>
#include <WiFiUdp.h>

#include <Arduino.h>
#include <Stream.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

//AWS
#include "sha256.h"
#include "Utils.h"
#include "AWSClient2.h"

//WEBSockets
#include <Hash.h>
#include <WebSocketsClient.h>

//MQTT PAHO
#include <SPI.h>
#include <IPStack.h>
#include <Countdown.h>
#include <MQTTClient.h>

#define LED_PIN 2
 
//AWS MQTT Websocket
#include "Client.h"
#include "AWSWebSocketClient.h"
#include "CircularByteBuffer.h"

//AWS IOT config, change these:
char wifi_ssid[]       = "mina";
char wifi_password[]   = "HappyTime";
char aws_endpoint[]    = "a21jd7gud1swyd.iot.us-east-1.amazonaws.com";


//#define __IAM_ROOT__

#ifndef __IAM_ROOT__
//IAM
char aws_key[]         = "AKIAIS7JT5B2C5SIKVQQ"; 
char aws_secret[]      = "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";  
#else
//ROOT
char aws_key[]         = "AKIAJ7JJ7FHWTUMT6LCA"; 
char aws_secret[]      = "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";
#endif 

char aws_region[]      = "us-east-1";
const char* aws_topic  = "$aws/things/pything/shadow/update";
int port = 443;


//MQTT config
const int maxMQTTpackageSize = 512;
const int maxMQTTMessageHandlers = 1;

ESP8266WiFiMulti WiFiMulti;

AWSWebSocketClient awsWSclient(1000);

IPStack ipstack(awsWSclient);
MQTT::Client<IPStack, Countdown, maxMQTTpackageSize, maxMQTTMessageHandlers> *client = NULL;

//# of connections
long connection = 0;

//generate random mqtt clientID
char* generateClientID () {
  char* cID = new char[23]();
  for (int i=0; i<22; i+=1)
    cID[i]=(char)random(1, 256);
  return cID;
}

//count messages arrived
int arrivedcount = 0;

void blinkLed(unsigned int);
void blinkLed();
void ledOn();
void ledOff();

//callback to handle mqtt messages
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

//connects to websocket layer and mqtt layer
bool connect () {

    if (client == NULL) 
    {
      client = new MQTT::Client<IPStack, Countdown, maxMQTTpackageSize, maxMQTTMessageHandlers>(ipstack);
    } else 
    {
      if (client->isConnected ()) {    
        client->disconnect ();
      }  
      delete client;
      client = new MQTT::Client<IPStack, Countdown, maxMQTTpackageSize, maxMQTTMessageHandlers>(ipstack);
    }


    //delay is not necessary... it just help us to get a "trustful" heap space value
    delay (1000);
    Serial.print (millis ());
    Serial.print (" - conn: ");
    Serial.print (++connection);
    Serial.print (" - (");
    Serial.print (ESP.getFreeHeap ());
    Serial.println (")");




   Serial.println("connecting with ipstack...");
   int rc = ipstack.connect(aws_endpoint, port);
    if (rc != 1)
    {
      Serial.println("error connecting to the websocket server with ipstack");
      return false;
    } else {
      Serial.println("websocket layer connected");
    }


    Serial.println("MQTT connecting");
    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
    data.MQTTVersion = 3;
    char* clientID = generateClientID ();
    data.clientID.cstring = clientID;
    rc = client->connect(data);
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

//subscribe to a mqtt topic
void subscribe () {
   //subscript to a topic
    int rc = client->subscribe(aws_topic, MQTT::QOS0, messageArrived);
    if (rc != 0) {
      Serial.print("rc from MQTT subscribe is ");
      Serial.println(rc);
      return;
    }
    Serial.println("MQTT subscribed");
}

//send a message to a mqtt topic
void sendmessage () {
    //send a message
    MQTT::Message message;
    char buf[100];
    strcpy(buf, "{\"state\":{\"reported\":{\"on\": false}, \"desired\":{\"on\": false}}}");
    message.qos = MQTT::QOS0;
    message.retained = false;
    message.dup = false;
    message.payload = (void*)buf;
    message.payloadlen = strlen(buf)+1;
    int rc = client->publish(aws_topic, message); 
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



void setup() {
    Serial.begin (9600);
    //Serial.begin (9600);
    delay (2000);
    //Serial.setDebugOutput(1);
    configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");

    pinMode(LED_PIN, OUTPUT); 

    //fill with ssid and wifi password
    WiFiMulti.addAP(wifi_ssid, wifi_password);
    Serial.println ("connecting to wifi");
    while(WiFiMulti.run() != WL_CONNECTED) {
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

    if (connect ()){
      blinkLed(3);
      subscribe ();
      sendmessage ();
    }
    else
      blinkLed(1); 
}

void loop() {
  /*
  //keep the mqtt up and running
  if (awsWSclient.connected ()) {    
      client->yield();
  } else {
    //handle reconnection
    if (connect ()){
      subscribe ();      
    }
  }
*/
  delay(10000); 
}


/*

⸮@⸮c⸮⸮connecting to wifi
state: 5 -> 0 (0)
rm 0
f r0, .....................scandone
.f r0, scandone
state: 0 -> 2 (b0)
state: 2 -> 3 (0)
state: 3 -> 5 (10)
add 0
aid 4
cnt 

connected with mina, channel 8
dhcp client start...
ip:192.168.1.34,mask:255.255.255.0,gw:192.168.1.1

connected
Wed Mar 29 13:59:43 2017

10807 - conn: 1 - (30240)
:ref 1
:wr
:sent 25
:ww
:wr
:sent 2
:ww
:wr
:sent 17
:ww
:wr
:sent 2
:ww
:wr
:sent 2
:ww
:rn 115
:rcl
:abort
:c0 1, 115
:ur 1
:del
[WS-Client] connect wss...
:ref 1
:wr
:sent 105
:ww
:rn 1460
:rd 5, 1460, 0
:rdi 1460, 5
:rd 1455, 1460, 5
:rdi 1455, 1455
:c0 1455, 1460
:rn 1460
:rd 1460, 1460:rch 1460, 81
, 0
:rdi 1460, 1460
:c 1460, 1460, 1541
:rd 81, 81, 0
:rdi 81, 81
:c0 81, 81
:wr
:sent 267
:ww
:wr
:sent 6
:ww
:wr
:sent 69
:rn 75
:ww
:rd 5, 75, 0
:rdi 75, 5
:rd 1, 75, 5
:rdi 70, 1
:rd 5, 75, 6
:rdi 69, 5
:rd 64, 75, 11
:rdi 64, 64
:c0 64, 75
[WS-Client] connected to a21jd7gud1swyd.iot.us-east-1.amazonaws.com:443.
[WS-Client][sendHeader] sending header...
[WS-Client][sendHeader] handshake GET /mqtt?X-Amz-Algorithm=AWS4-HMAC-SHA256&X-Amz-Credential=AKIAIS7JT5B2C5SIKVQQ%2F19860618%2Fus-east-1%2Fiotdevicegateway%2Faws4_request&X-Amz-Date=19860618T123600Z&X-Amz-Expires=86400&X-Amz-SignedHeaders=host&X-Amz-Signature=eb48503a3bad2c66c79c97e9b142b46a586084a469ecb0e6c4bb30e4622fd34c HTTP/1.1
Host: a21jd7gud1swyd.iot.us-east-1.amazonaws.com:443
Connection: Upgrade
Upgrade: websocket
Sec-WebSocket-Version: 13
Sec-WebSocket-Key: TGZ2jE5GIy4YW7LLYH9vMw==
Sec-WebSocket-Protocol: mqtt
Origin: file://
User-Agent: arduino-WebSocket-Client

:wr
:sent 613
:ww
[WS-Client][sendHeader] sending header... Done (918283us).
:rn 1162
:rch 1162, 53
:rcl
:abort
:rd 5, 1215, 0
:rdi 1162, 5
:rd 944, 1215, 5
:rdi 1157, 944
:wcs ra 894[WS-Client][handleHeader] RX: HTTP/1.1 403 Forbidden
[WS-Client][handleHeader] RX: content-type: application/json
[WS-Client][handleHeader] RX: content-length: 163
[WS-Client][handleHeader] RX: date: Wed, 29 Mar 2017 10:59:47 GMT
[WS-Client][handleHeader] RX: x-amzn-RequestId: 27042526-2601-34a0-ded8-b5f6ac0e41a6
[WS-Client][handleHeader] RX: x-amzn-ErrorType: ForbiddenException:
[WS-Client][handleHeader] RX: access-control-allow-origin: *
[WS-Client][handleHeader] RX: access-control-allow-headers: Authorization
[WS-Client][handleHeader] RX: access-control-allow-headers: X-amz-security-token
[WS-Client][handleHeader] RX: access-control-allow-headers: Accept
[WS-Client][handleHeader] RX: access-control-allow-headers: X-amz-date
[WS-Client][handleHeader] RX: access-control-allow-headers: X-amz-user-agent
[WS-Client][handleHeader] RX: access-control-allow-headers: X-amz-content-sha256
[WS-Client][handleHeader] RX: access-control-allow-headers: Accept-Language
[WS-Client][handleHeader] RX: access-control-allow-headers: X-requested-with
[WS-Client][handleHeader] RX: access-control-allow-headers: Content-Language
[WS-Client][handleHeader] RX: access-control-allow-headers: Content-Type
[WS-Client][handleHeader] RX: access-control-expose-headers: x-amzn-ErrorMessage
[WS-Client][handleHeader] RX: access-control-expose-headers: x-amzn-RequestId
[WS-Client][handleHeader] RX: access-control-expose-headers: x-amzn-ErrorType
[WS-Client][handleHeader] RX: access-control-expose-headers: Date
[WS-Client][handleHeader] Header read fin.
[WS-Client][handleHeader] Client settings:
[WS-Client][handleHeader]  - cURL: /mqtt?X-Amz-Algorithm=AWS4-HMAC-SHA256&X-Amz-Credential=AKIAIS7JT5B2C5SIKVQQ%2F19860618%2Fus-east-1%2Fiotdevicegateway%2Faws4_request&X-Amz-Date=19860618T123600Z&X-Amz-Expires=86400&X-Amz-SignedHeaders=host&X-Amz-Signature=eb48503a3bad2c66c79c97e9b142b46a586084a469ecb0e6c4bb30e4622fd34c
[WS-Client][handleHeader]  - cKey: TGZ2jE5GIy4YW7LLYH9vMw==
[WS-Client][handleHeader] Server header:
[WS-Client][handleHeader]  - cCode: 403
[WS-Client][handleHeader]  - cIsUpgrade: 0
[WS-Client][handleHeader]  - cIsWebsocket: 1
[WS-Client][handleHeader]  - cAccept: 
[WS-Client][handleHeader]  - cProtocol: mqtt
[WS-Client][handleHeader]  - cExtensions: 
[WS-Client][handleHeader]  - cVersion: 0
[WS-Client][handleHeader]  - cSessionId: 
:rd 5, 1215, 949
:rdi 213, 5
:rd 208, 1215, 954
:rdi 208, 208
:c 208, 1162, 1215
:wcs ra 163[WS-Client][handleHeader] no Websocket connection close.
:ur 1
:del
[WS-Client] client disconnected.
error connection to the websocket server


 */
 
