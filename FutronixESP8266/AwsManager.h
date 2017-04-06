#ifndef __AWS_MANAGER_H__
#define __AWS_MANAGER_H__

#include <WebSockets.h>
#include <WebSocketsClient.h>
#include <Hash.h>
#include <sha256.h>
#include <Utils.h>
#include <AWSClient2.h>
#include <SPI.h>
#include <IPStack.h>
#include <Countdown.h>
#include <MQTTClient.h>
#include <Client.h> 
#include <AWSWebSocketClient.h>
#include <CircularByteBuffer.h>

#define AWS_ENDPOINT          "a21jd7gud1swyd.iot.us-east-1.amazonaws.com"
#define AWS_USER              "AKIAIS7JT5B2C5SIKVQQ"
#define AWS_SECRET            "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"
#define AWS_REGION            "eu-east-1"
#define AWS_TOPIC             "$aws/things/Sigma/shadow/update"
#define AWS_PORT              443

const int maxMQTTpackageSize = 512;
const int maxMQTTMessageHandlers = 1;

AWSWebSocketClient _awsWSclient(1000);
IPStack _ipstack(_awsWSclient);
int _arrivedCount = 0;

class AwsManager
{
  public: 
    AwsManager(); 

    void begin(); 
    void connectAndListen(); 

  private: 
    bool _enabled = false; 
    MQTT::Client<IPStack, Countdown, maxMQTTpackageSize, maxMQTTMessageHandlers>* _mqttClient = NULL;
    long _connection = 0;

    char* generateClientID(); 
    bool connect(); 
    void subscribe(); 
    void sendMessage();
    void messageArrived(MQTT::MessageData&);
}; 

    
void onMessageArrivedCallback(MQTT::MessageData& md)
{
  MQTT::Message &message = md.message;

  Serial.print("Message ");
  Serial.print(++_arrivedCount);
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

AwsManager::AwsManager()
{
  
}

void AwsManager::begin()
{
  //fill AWS parameters    
  _awsWSclient.setAWSRegion(AWS_REGION);
  _awsWSclient.setAWSDomain(AWS_ENDPOINT);
  _awsWSclient.setAWSKeyID(AWS_USER);
  _awsWSclient.setAWSSecretKey(AWS_SECRET);
  _awsWSclient.setUseSSL(true);

  this->_enabled = true; 
}

void AwsManager::connectAndListen()
{
  if (this->connect ()){
    this->subscribe ();
    this->sendMessage ();
  }
}

char* AwsManager::generateClientID () 
{
  char* cID = new char[23]();
  for (int i=0; i<22; i+=1)
    cID[i]=(char)random(1, 256);
  return cID;
}

bool AwsManager::connect () 
{
  if (this->_mqttClient == NULL) {
    this->_mqttClient = new MQTT::Client<IPStack, Countdown, maxMQTTpackageSize, maxMQTTMessageHandlers>(_ipstack);
  } 
  else 
  {
    if (this->_mqttClient->isConnected ()) {    
      Serial.println("Connected already; disconnecting..."); 
      this->_mqttClient->disconnect ();
  }  
      
  delete this->_mqttClient;
    this->_mqttClient = new MQTT::Client<IPStack, Countdown, maxMQTTpackageSize, maxMQTTMessageHandlers>(_ipstack);
  }

  //delay is not necessary... it just help us to get a "trustful" heap space value
  delay (1000);
  Serial.print (millis ());
  Serial.print (" - conn: ");
  Serial.print (++_connection);
  Serial.print (" - (");
  Serial.print (ESP.getFreeHeap ());
  Serial.println (")");


  int rc = _ipstack.connect(AWS_ENDPOINT, AWS_PORT);
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

  char* clientID = this->generateClientID ();
  data.clientID.cstring = clientID;
  rc = _mqttClient->connect(data);
  delete[] clientID;
  if (rc != 0) {
    Serial.print("error connection to MQTT server");
    Serial.println(rc);
    return false;
  }
  Serial.println("MQTT connected");
  return true;
}

void AwsManager::subscribe () 
{
  //subscript to a topic
  int rc = _mqttClient->subscribe(AWS_TOPIC, MQTT::QOS0, onMessageArrivedCallback);
  if (rc != 0) {
    Serial.print("rc from MQTT subscribe is ");
    Serial.println(rc);
    return;
  }

  Serial.println("MQTT subscribed");
}

void AwsManager::sendMessage () 
{
  //send a message
  MQTT::Message message;
  char buf[100];
  strcpy(buf, "{\"state\":{\"reported\":{\"on\": false}, \"desired\":{\"on\": false}}}");
  message.qos = MQTT::QOS0;
  message.retained = false;
  message.dup = false;
  message.payload = (void*)buf;
  message.payloadlen = strlen(buf)+1;
  int rc = this->_mqttClient->publish(AWS_TOPIC, message); 
}

#endif 


