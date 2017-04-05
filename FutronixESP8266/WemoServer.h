#ifndef __WEMO_SERVER_H__
#define __WEMO_SERVER_H__

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUDP.h>
#include <ESP8266WebServer.h>
#include <functional> 

#define SERVER_COUNT_LIMIT 20

class IWemoCallbackHandler
{
  public:
    virtual void handleCallback(int param);
};

class WemoServer
{
  public:
    WemoServer(char* deviceName, int localPort, IWemoCallbackHandler* callbackHandler);
    ~WemoServer();

    void respondToSearch(IPAddress& senderIP, unsigned int senderPort);
    void listen();

  private:
    char* _deviceName;
    int _localPort;
    IWemoCallbackHandler* _callbackHandler;
    String _uuid;
    WiFiUDP _udp; 
    String _serial;
    ESP8266WebServer* _server = NULL;
    
    void startWebServer();
    void handleEventservice(); 
    void handleUpnpControl(); 
    void handleRoot(); 
    void handleSetupXml();
};



WemoServer::WemoServer(char* deviceName, int localPort, IWemoCallbackHandler* callbackHandler)
{
  this->_deviceName = deviceName;
  this->_localPort = localPort;
  this->_callbackHandler = callbackHandler;
  
  uint32_t chipId = ESP.getChipId();
  char uuid[64];
  sprintf_P(uuid, PSTR("38323636-4558-4dda-9188-cda0e6%02x%02x%02x"),
          (uint16_t) ((chipId >> 16) & 0xff),
          (uint16_t) ((chipId >>  8) & 0xff),
          (uint16_t)   chipId        & 0xff);

  this->_serial = String(uuid);
  this->_uuid = "Socket-1_0-" + this->_serial+"-"+ String(localPort);

  this->startWebServer(); 
}

WemoServer::~WemoServer()
{
  delete this->_callbackHandler;
}

void WemoServer::respondToSearch(IPAddress& senderIP, unsigned int senderPort) {
  Serial.println("");
  Serial.print("Sending response to ");
  Serial.println(senderIP);
  Serial.print("Port : ");
  Serial.println(senderPort);

  IPAddress localIP = WiFi.localIP();
  char s[16];
  sprintf(s, "%d.%d.%d.%d", localIP[0], localIP[1], localIP[2], localIP[3]);

  String response =
    "HTTP/1.1 200 OK\r\n"
    "CACHE-CONTROL: max-age=86400\r\n"
    "DATE: Sat, 26 Nov 2016 04:56:29 GMT\r\n"
    "EXT:\r\n"
    "LOCATION: http://" + String(s) + ":" + String(this->_localPort) + "/setup.xml\r\n"
    "OPT: \"http://schemas.upnp.org/upnp/1/0/\"; ns=01\r\n"
    "01-NLS: b9200ebb-736d-4b93-bf03-835149d13983\r\n"
    "SERVER: Unspecified, UPnP/1.0, Unspecified\r\n"
    "ST: urn:Belkin:device:**\r\n"
    "USN: uuid:" + this->_uuid + "::urn:Belkin:device:**\r\n"
    "X-User-Agent: redsonic\r\n\r\n";

  this->_udp.beginPacket(senderIP, senderPort);
  this->_udp.write(response.c_str());
  this->_udp.endPacket();
}

void WemoServer::listen()
{
  if (this->_server != NULL) {
    this->_server->handleClient();
    delay(1);
  }  
}

void WemoServer::startWebServer()
{
  this->_server = new ESP8266WebServer(this->_localPort);

  this->_server->on("/", [&]() {
    this->handleRoot();
  });

  this->_server->on("/setup.xml", [&]() {
    this->handleSetupXml();
  });

  this->_server->on("/upnp/control/basicevent1", [&]() {
    this->handleUpnpControl();
  });

  this->_server->on("/eventservice.xml", [&]() {
    this->handleEventservice();
  });

  //this->_server->onNotFound(handleNotFound);
  this->_server->begin();

  Serial.println("WebServer started on port: ");
  Serial.println(_localPort);
}

void WemoServer::handleEventservice()
{
  Serial.println(" ########## Responding to eventservice.xml ... ########\n");

  String eventServiceXml = "<?scpd xmlns=\"urn:Belkin:service-1-0\"?>"
        "<actionList>"
          "<action>"
            "<name>SetBinaryState</name>"
            "<argumentList>"
              "<argument>"
                "<retval/>"
                "<name>BinaryState</name>"
                "<relatedStateVariable>BinaryState</relatedStateVariable>"
                "<direction>in</direction>"
              "</argument>"
            "</argumentList>"
             "<serviceStateTable>"
              "<stateVariable sendEvents=\"yes\">"
                "<name>BinaryState</name>"
                "<dataType>Boolean</dataType>"
                "<defaultValue>0</defaultValue>"
              "</stateVariable>"
              "<stateVariable sendEvents=\"yes\">"
                "<name>level</name>"
                "<dataType>string</dataType>"
                "<defaultValue>0</defaultValue>"
              "</stateVariable>"
            "</serviceStateTable>"
          "</action>"
        "</scpd>\r\n"
        "\r\n";

  this->_server->send(200, "text/plain", eventServiceXml.c_str());
}

void WemoServer::handleUpnpControl(){
  Serial.println("########## Responding to  /upnp/control/basicevent1 ... ##########");

  //for (int x=0; x <= HTTP.args(); x++) {
  //  Serial.println(HTTP.arg(x));
  //}

  String request = this->_server->arg(0);
  Serial.print("request:");
  Serial.println(request);

  if(request.indexOf("<BinaryState>1</BinaryState>") > 0) {
      Serial.println("Got Turn on request");
      this->_callbackHandler->handleCallback(1);
  }

  if(request.indexOf("<BinaryState>0</BinaryState>") > 0) {
      Serial.println("Got Turn off request");
      this->_callbackHandler->handleCallback(0);
  }

  Serial.println("Responding to Control request");
  this->_server->send(200, "text/plain", "");
}

void WemoServer::handleRoot()
{
  this->_server->send(200, "text/plain", "You should tell Alexa to discover devices");
}

void WemoServer::handleSetupXml()
{
  Serial.println(" ########## Responding to setup.xml ... ########\n");

  IPAddress localIP = WiFi.localIP();
  char s[16];
  sprintf(s, "%d.%d.%d.%d", localIP[0], localIP[1], localIP[2], localIP[3]);

  String setupXml = "<?xml version=\"1.0\"?>"
        "<root>"
         "<device>"
            "<deviceType>urn:Belkin:device:controllee:1</deviceType>"
            "<friendlyName>"+ String(this->_deviceName) +"</friendlyName>"
            "<manufacturer>Belkin International Inc.</manufacturer>"
            "<modelName>Emulated Socket</modelName>"
            "<modelNumber>3.1415</modelNumber>"
            "<UDN>uuid:"+ this->_uuid +"</UDN>"
            "<serialNumber>221517K0101769</serialNumber>"
            "<binaryState>0</binaryState>"
            "<serviceList>"
              "<service>"
                  "<serviceType>urn:Belkin:service:basicevent:1</serviceType>"
                  "<serviceId>urn:Belkin:serviceId:basicevent1</serviceId>"
                  "<controlURL>/upnp/control/basicevent1</controlURL>"
                  "<eventSubURL>/upnp/event/basicevent1</eventSubURL>"
                  "<SCPDURL>/eventservice.xml</SCPDURL>"
              "</service>"
          "</serviceList>"
          "</device>"
        "</root>\r\n"
        "\r\n";

  this->_server->send(200, "text/xml", setupXml.c_str());

  Serial.print("Sending :");
  Serial.println(setupXml);
}

#endif
