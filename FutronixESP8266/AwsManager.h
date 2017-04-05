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

#define AWS_ENDPOINT          "a21jd7gud1swyd.iot.us-east-1.amazonaws.com";
#define AWS_USER              "AKIAIS7JT5B2C5SIKVQQ"; 
#define AWS_SECRET            "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";
#define AWS_REGION            "eu-east-1";
#define AWS_TOPIC             "$aws/things/Sigma/shadow/update";
#define AWS_PORT              = 443;

class AwsManager
{
  public: 
    AwsManager(); 

    void begin(); 
    void connectAndListen(); 

  private: 
    AWSWebSocketClient* _awsWSclient = new AWSWebSocketClient(1000);
    IPStack* _ipstack = new IPStack(_awsWSclient);
    MQTT::Client<IPStack, Countdown, maxMQTTpackageSize, maxMQTTMessageHandlers>* _mqttClient = NULL;
    int _arrivedcount = 0;
    long _connection = 0;

    char* generateClientId(); 
    bool connect(); 
    void subscribe(); 
    void sendMessage();
}; 


AwsManager::AwsManager()
{
  
}

void AwsManager::begin()
{
  //fill AWS parameters    
  this->_awsWSclient.setAWSRegion(AWS_REGION);
  this->_awsWSclient.setAWSDomain(AWS_ENDPOINT);
  this->_awsWSclient.setAWSKeyID(AWS_USER);
  this->_awsWSclient.setAWSSecretKey(AWS_SECRET);
  this->_awsWSclient.setUseSSL(true);
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

void AwsManager::messageArrived(MQTT::MessageData& md)
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

bool AwsManager::connect () 
{
  if (this->_mqttClient == NULL) {
    this->_mqttClient = new MQTT::Client<IPStack, Countdown, maxMQTTpackageSize, maxMQTTMessageHandlers>(ipstack);
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
  int rc = this->_mqttClient->subscribe(AWS_TOPIC, MQTT::QOS0, messageArrived);
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


