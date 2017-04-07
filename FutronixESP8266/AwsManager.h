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
#define AWS_USER              "XXXXXXXXXXXXXXX"
#define AWS_SECRET            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
#define AWS_REGION            "eu-east-1"
#define AWS_TOPIC             "$aws/things/Sigma/shadow/update"
#define AWS_PORT              443

const int maxMQTTpackageSize = 512;
const int maxMQTTMessageHandlers = 1;

AWSWebSocketClient _awsWSclient(1000);
IPStack _ipstack(_awsWSclient);
int _arrivedCount = 0;

/****************************************
 * AwsManager
 * ----------
 * Interface to AWS SDK. 
 */
class AwsManager
{
  private: 
    bool _enabled = false; 
    MQTT::Client<IPStack, Countdown, maxMQTTpackageSize, maxMQTTMessageHandlers>* _mqttClient = NULL;
    long _connection = 0;

  public: 
    AwsManager(); 

    void begin(); 
    void connectAndListen(); 

  private: 
    char* generateClientID(); 
    bool connect(); 
    void subscribe(); 
    void sendMessage();
    void messageArrived(MQTT::MessageData&);
}; 
/****************************************/

    
void onMessageArrivedCallback(MQTT::MessageData& md)
{
  MQTT::Message &message = md.message;

  DEBUG_PRINT("Message ");
  DEBUG_PRINT(++_arrivedCount);
  DEBUG_PRINT(" arrived: qos ");
  DEBUG_PRINT(message.qos);
  DEBUG_PRINT(", retained ");
  DEBUG_PRINT(message.retained);
  DEBUG_PRINT(", dup ");
  DEBUG_PRINT(message.dup);
  DEBUG_PRINT(", packetid ");
  DEBUG_PRINTLN(message.id);
  DEBUG_PRINT("Payload ");
  char* msg = new char[message.payloadlen+1]();
  memcpy (msg,message.payload,message.payloadlen);
  DEBUG_PRINTLN(msg);
  delete msg;
}

/*---------------------------------------*/
AwsManager::AwsManager()
{
  
}

/*---------------------------------------*/
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

/*---------------------------------------*/
void AwsManager::connectAndListen()
{
  if (this->connect ()){
    this->subscribe ();
    this->sendMessage ();
  }
}

/*---------------------------------------*/
char* AwsManager::generateClientID () 
{
  char* cID = new char[23]();
  for (int i=0; i<22; i+=1)
    cID[i]=(char)random(1, 256);
  return cID;
}

/*---------------------------------------*/
bool AwsManager::connect () 
{
  if (this->_mqttClient == NULL) {
    this->_mqttClient = new MQTT::Client<IPStack, Countdown, maxMQTTpackageSize, maxMQTTMessageHandlers>(_ipstack);
  } 
  else 
  {
    if (this->_mqttClient->isConnected ()) {    
      DEBUG_PRINTLN("Connected already; disconnecting..."); 
      this->_mqttClient->disconnect ();
  }  
      
  delete this->_mqttClient;
    this->_mqttClient = new MQTT::Client<IPStack, Countdown, maxMQTTpackageSize, maxMQTTMessageHandlers>(_ipstack);
  }

  //delay is not necessary... it just help us to get a "trustful" heap space value
  delay (1000);
  DEBUG_PRINTLN (millis ());
  DEBUG_PRINTLN (" - conn: ");
  DEBUG_PRINTLN (++_connection);
  DEBUG_PRINTLN (" - (");
  DEBUG_PRINTLN (ESP.getFreeHeap ());
  DEBUG_PRINTLN (")");

  int rc = _ipstack.connect(AWS_ENDPOINT, AWS_PORT);
  if (rc != 1)
  {
    DEBUG_PRINTLN("error connection to the websocket server");
    DEBUG_PRINTLN(rc);
    return false;
  } else {
    DEBUG_PRINTLN("websocket layer connected");
  }

  DEBUG_PRINTLN("MQTT connecting");
  MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
  data.MQTTVersion = 3;

  char* clientID = this->generateClientID ();
  data.clientID.cstring = clientID;
  rc = _mqttClient->connect(data);
  delete[] clientID;
  if (rc != 0) {
    DEBUG_PRINTLN("error connection to MQTT server");
    DEBUG_PRINTLN(rc);
    return false;
  }
  DEBUG_PRINTLN("MQTT connected");
  return true;
}

/*---------------------------------------*/
void AwsManager::subscribe () 
{
  //subscript to a topic
  int rc = _mqttClient->subscribe(AWS_TOPIC, MQTT::QOS0, onMessageArrivedCallback);
  if (rc != 0) {
    DEBUG_PRINTLN("rc from MQTT subscribe is ");
    DEBUG_PRINTLN(rc);
    return;
  }

  DEBUG_PRINTLN("MQTT subscribed");
}

/*---------------------------------------*/
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


