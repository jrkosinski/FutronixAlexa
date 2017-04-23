#define __MWWTTO__
#ifdef __MWWTTO__



//This example needs https://github.com/esp8266/arduino-esp8266fs-plugin

#include "FS.h"
#include <ESP8266WiFi.h>
#include "PubSubClient.h"

// Update these with values suitable for your network.

const char* ssid = "mina";
const char* password = "HappyTime";
#define PORT 443 
#define NEWUSER

const char* mqtt_server = "a21jd7gud1swyd.iot.us-east-1.amazonaws.com"; //MQTT broker ip 

void writeFiles(); 

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

}


WiFiClientSecure espClient;
PubSubClient client(mqtt_server,PORT,callback,espClient); //set  MQTT port number to 8883 as per //standard
long lastMsg = 0;
char msg[50];
int value = 0;



void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

#ifdef NEWUSER
const char* _user = "AKIAJHKIKQFLRQTDSFEA"; 
const char* _pass = "+LSScFDHsS64lvxF2uhADDlh42GXuulR7N4etLpy"; 
#else
const char* _user = "AKIAIMYWT5QLNV7VCJZQ"; 
const char* _pass = "a9LTk59IaadF4ObTq8s6BdRxk9xmPJHk5HS/3QQR"; 
#endif
const char* _topic = "a21jd7gud1swyd.iot.us-east-1.amazonaws.com/things/pything/shadow/update"; 
const char* _msg = "hi"; 
const char* _id = "ESP8266Client"; 

bool attempts = 0; 
void reconnect() {
  if (attempts > 0)
    return; 
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect

    attempts++; 
    Serial.println(_user);
    if (client.connect(_id, _user, _pass, _topic, 0, false, _msg)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
      return;
    }
  }
}

void setup() {

  Serial.begin(9600);
  Serial.setDebugOutput(true);
  setup_wifi();
  delay(1000);
  if (!SPIFFS.begin()) {
    Serial.println("Failed to mount file system");
    return;
  }
  else
  {
    writeFiles();
    Serial.println("Mounted FS");
  }

  Serial.print("Heap: "); Serial.println(ESP.getFreeHeap());


  // Load certificate file
  File cert = SPIFFS.open("/cert.der", "r"); //replace cert.crt eith your uploaded file name
  if (!cert) {
    Serial.println("Failed to open cert file");
  }
  else
  Serial.println("Success to open cert file");
  
  delay(1000);
  
  if(espClient.loadCertificate(cert))
    Serial.println("cert loaded");
  else
    Serial.println("cert not loaded");

  // Load private key file
  File private_key = SPIFFS.open("/private.der", "r"); //replace private eith your uploaded file name
  if (!private_key) {
    Serial.println("Failed to open private cert file");
  }
  else
  Serial.println("Success to open private cert file");
  
  delay(1000);
  
  if(espClient.loadPrivateKey(private_key))
    Serial.println("private key loaded");
  else
    Serial.println("private key not loaded");
  
  // Load CA file
  File ca = SPIFFS.open("/ca.der", "r"); //replace ca eith your uploaded file name
  if (!ca) {
    Serial.println("Failed to open ca ");
  }
  else
  Serial.println("Success to open ca");
  
  delay(1000);

  if(espClient.loadCACert(ca))
    Serial.println("ca loaded");
  else
    Serial.println("ca failed");
  
  Serial.print("Heap: "); Serial.println(ESP.getFreeHeap());
  
  
  client.setServer(mqtt_server, PORT);
  client.setCallback(callback);
}



void loop() {
  //Serial.println("LOOP......."); 
  if (!client.connected()) {
    reconnect();
  }
  else
    {
    client.loop();
  
    long now = millis();
    if (now - lastMsg > 2000) {
      lastMsg = now;
      ++value;
      snprintf (msg, 75, "hello world #%ld", value);
      Serial.print("Publish message: ");
      Serial.println(msg);
      client.publish("outTopic", msg);
    }
  }
  delay(20000); 
}


void writeFiles()
{
  File cert = SPIFFS.open("/cert.der", "w"); //replace cert.crt eith your uploaded file name
  if (!cert)
    Serial.println("Unable to open file for write"); 
  else
  {
    unsigned char buf[862]= {48,130,3,90,48,130,2,66,160,3,2,1,2,2,21,0,188,23,22,6,244,26,182,131,36,124,226,211,233,109,189,128,47,148,125,137,48,13,6,9,42,134,72,134,247,13,1,1,11,5,0,48,77,49,75,48,73,6,3,85,4,11,12,66,65,109,97,122,111,110,32,87,101,98,32,83,101,114,118,105,99,101,115,32,79,61,65,109,97,122,111,110,46,99,111,109,32,73,110,99,46,32,76,61,83,101,97,116,116,108,101,32,83,84,61,87,97,115,104,105,110,103,116,111,110,32,67,61,85,83,48,30,23,13,49,55,48,51,51,48,49,50,53,53,48,52,90,23,13,52,57,49,50,51,49,50,51,53,57,53,57,90,48,30,49,28,48,26,6,3,85,4,3,12,19,65,87,83,32,73,111,84,32,67,101,114,116,105,102,105,99,97,116,101,48,130,1,34,48,13,6,9,42,134,72,134,247,13,1,1,1,5,0,3,130,1,15,0,48,130,1,10,2,130,1,1,0,129,114,38,8,248,110,54,140,247,110,113,136,54,112,205,63,252,210,90,132,95,234,136,227,111,97,216,144,173,206,132,46,181,100,119,132,40,54,37,114,104,42,122,70,49,153,173,241,115,104,114,213,162,196,122,70,204,28,220,187,86,183,87,134,183,173,64,90,31,30,178,45,238,209,172,60,144,18,103,217,68,244,169,74,215,93,226,86,179,161,57,164,117,10,30,129,224,208,16,214,252,253,220,176,169,219,255,245,155,61,176,151,6,28,25,153,179,27,39,88,248,102,179,35,59,175,138,206,44,247,120,100,152,234,59,45,131,27,222,78,60,230,170,122,125,56,189,150,171,38,222,200,237,132,1,71,104,161,147,237,60,34,247,236,56,206,89,197,50,225,66,177,220,215,96,239,251,246,213,85,151,192,81,62,240,82,184,220,213,77,205,60,146,140,32,111,43,148,141,206,201,224,123,14,162,54,96,234,18,46,242,219,220,172,206,135,206,30,0,144,17,196,151,205,109,55,50,154,163,31,136,166,50,243,201,85,60,143,99,203,216,74,206,245,16,114,5,244,186,215,37,209,113,224,226,33,2,3,1,0,1,163,96,48,94,48,31,6,3,85,29,35,4,24,48,22,128,20,209,205,54,168,79,152,192,14,136,238,29,192,128,80,230,118,68,105,223,17,48,29,6,3,85,29,14,4,22,4,20,172,27,207,123,194,167,28,71,11,50,156,15,225,112,122,211,144,219,231,89,48,12,6,3,85,29,19,1,1,255,4,2,48,0,48,14,6,3,85,29,15,1,1,255,4,4,3,2,7,128,48,13,6,9,42,134,72,134,247,13,1,1,11,5,0,3,130,1,1,0,7,89,61,84,129,186,10,193,166,148,86,183,58,251,100,250,144,12,185,193,168,195,109,190,212,130,219,36,129,93,84,18,30,17,65,142,131,248,146,189,35,49,103,112,138,58,84,69,87,166,68,146,239,124,162,10,36,182,254,223,34,165,159,205,103,211,139,98,121,62,174,130,227,80,27,234,220,111,118,96,101,33,176,1,8,39,191,111,0,96,212,234,84,154,161,206,206,65,181,196,177,56,83,18,23,152,215,131,165,30,65,26,101,188,249,203,80,216,93,19,90,114,231,238,57,221,143,165,132,103,248,210,50,218,249,6,179,221,235,68,243,246,145,3,88,119,114,143,124,34,168,190,76,163,157,136,140,1,157,200,131,43,184,110,212,138,38,52,59,92,35,138,161,124,44,35,46,14,188,70,83,230,158,132,58,10,38,194,189,10,209,144,83,140,250,210,113,147,75,154,232,86,32,72,182,97,81,191,202,54,246,166,102,60,31,129,123,143,146,71,100,140,28,50,227,235,113,155,148,185,23,87,205,162,29,85,146,47,123,207,23,36,36,232,103,122,44,171,21,78,242,110,162,221,234,199}; 
    cert.write(buf, 862); 
    cert.close();
  }
  
  File cert2 = SPIFFS.open("/private.der", "w"); //replace cert.crt eith your uploaded file name
  if (!cert2)
    Serial.println("Unable to open file for write"); 
  else
  {
    unsigned char buf[1191]= {48,130,4,163,2,1,0,2,130,1,1,0,129,114,38,8,248,110,54,140,247,110,113,136,54,112,205,63,252,210,90,132,95,234,136,227,111,97,216,144,173,206,132,46,181,100,119,132,40,54,37,114,104,42,122,70,49,153,173,241,115,104,114,213,162,196,122,70,204,28,220,187,86,183,87,134,183,173,64,90,31,30,178,45,238,209,172,60,144,18,103,217,68,244,169,74,215,93,226,86,179,161,57,164,117,10,30,129,224,208,16,214,252,253,220,176,169,219,255,245,155,61,176,151,6,28,25,153,179,27,39,88,248,102,179,35,59,175,138,206,44,247,120,100,152,234,59,45,131,27,222,78,60,230,170,122,125,56,189,150,171,38,222,200,237,132,1,71,104,161,147,237,60,34,247,236,56,206,89,197,50,225,66,177,220,215,96,239,251,246,213,85,151,192,81,62,240,82,184,220,213,77,205,60,146,140,32,111,43,148,141,206,201,224,123,14,162,54,96,234,18,46,242,219,220,172,206,135,206,30,0,144,17,196,151,205,109,55,50,154,163,31,136,166,50,243,201,85,60,143,99,203,216,74,206,245,16,114,5,244,186,215,37,209,113,224,226,33,2,3,1,0,1,2,130,1,0,104,123,92,91,237,57,248,105,251,229,119,89,252,167,59,228,36,11,29,232,175,10,189,134,230,6,82,41,37,65,5,204,53,185,108,6,141,246,118,111,75,200,205,125,14,215,254,96,95,72,181,55,222,243,59,199,29,69,70,157,39,183,64,188,138,81,27,26,34,248,65,213,117,104,99,223,86,98,164,211,232,38,154,230,191,195,21,199,233,161,126,199,81,120,227,122,237,156,195,154,228,5,124,54,84,68,48,235,107,108,117,182,208,197,169,156,55,198,249,17,133,30,80,5,102,77,160,120,246,120,243,197,40,88,217,56,189,161,142,246,142,73,166,186,222,135,198,78,239,194,190,150,127,67,114,63,148,115,248,241,25,49,0,77,234,252,118,242,191,48,90,57,128,40,233,5,116,83,135,38,14,63,113,46,143,21,26,255,56,237,125,19,81,110,21,185,141,66,198,204,206,163,187,240,241,248,46,16,58,139,249,197,132,48,248,78,27,98,145,179,75,201,199,37,48,227,140,103,228,154,13,37,159,131,78,229,164,105,173,84,37,53,19,114,217,225,163,161,72,76,79,221,201,212,174,169,2,129,129,0,203,150,73,134,194,128,61,32,177,37,10,238,35,21,86,237,118,33,180,128,76,219,33,249,48,200,170,167,110,110,172,132,229,172,129,247,162,215,56,248,209,118,119,52,111,252,163,115,172,49,24,60,236,202,145,93,105,239,207,80,43,222,14,128,49,149,137,156,135,10,7,45,22,222,12,165,233,1,31,152,43,211,196,140,62,36,247,191,175,44,10,225,226,51,242,98,175,182,202,21,173,32,248,42,18,72,86,77,128,219,183,221,177,91,56,188,180,19,187,234,112,151,14,47,16,4,47,227,2,129,129,0,162,197,121,202,139,246,141,144,11,67,46,51,223,21,160,77,137,121,5,141,20,188,169,42,177,10,50,18,112,23,94,190,152,173,35,208,159,62,227,121,219,79,42,6,134,197,216,224,26,245,129,208,129,10,233,29,15,41,122,9,59,36,11,169,34,107,159,65,71,87,35,200,47,137,117,212,95,176,238,238,37,34,26,46,95,63,55,11,60,145,190,192,177,204,238,162,73,178,202,121,62,147,216,75,132,54,186,109,120,250,99,61,231,141,70,34,236,208,190,237,216,2,210,203,246,249,125,43,2,129,129,0,172,60,160,69,223,69,136,241,160,140,187,59,142,248,221,203,22,23,27,211,51,178,108,130,154,176,26,24,126,136,41,114,97,176,217,95,189,39,162,46,119,95,103,40,56,218,31,26,156,130,8,132,122,188,65,209,42,239,212,143,122,29,106,93,48,21,216,15,81,251,188,232,123,161,46,72,42,225,101,110,99,29,204,249,237,21,111,120,126,229,158,235,77,105,47,86,40,92,87,137,145,40,4,107,235,125,193,109,222,103,132,157,141,15,164,231,152,221,221,44,81,70,170,156,76,75,142,139,2,129,128,115,101,235,173,180,195,12,164,5,200,16,54,136,63,109,209,234,215,217,30,167,136,5,171,21,174,243,198,236,159,52,245,141,138,154,172,170,105,204,158,52,228,144,23,230,110,216,14,50,46,194,101,15,71,171,243,133,47,83,172,189,184,21,104,72,213,226,39,70,171,185,219,144,16,53,112,102,147,43,33,189,67,203,41,251,46,247,24,73,77,206,222,16,112,8,243,23,170,225,47,74,53,73,16,104,224,224,21,2,195,9,158,246,205,202,222,177,228,96,168,129,15,167,97,33,108,201,215,2,129,128,76,209,60,50,117,235,137,0,39,16,30,249,122,50,208,207,149,213,26,246,146,246,173,120,30,177,180,22,204,224,239,52,239,200,205,113,73,172,178,7,1,23,21,153,124,76,224,149,141,238,77,72,109,8,101,116,81,69,30,132,97,7,174,55,0,17,104,250,158,238,180,164,179,209,249,119,111,180,90,160,79,35,17,235,2,37,227,219,29,142,208,134,169,172,210,240,72,95,60,48,65,73,114,245,30,39,228,115,216,255,253,155,153,48,5,206,174,225,24,187,144,156,116,48,113,230,213,121}; 
    cert2.write(buf, 1197); 
    cert2.close();
  }
  
  File cert3 = SPIFFS.open("/ca.der", "w"); //replace cert.crt eith your uploaded file name
  if (!cert3)
    Serial.println("Unable to open file for write"); 
  else
  {
    unsigned char buf[1239]= {48,130,4,211,48,130,3,187,160,3,2,1,2,2,16,24,218,209,158,38,125,232,187,74,33,88,205,204,107,59,74,48,13,6,9,42,134,72,134,247,13,1,1,5,5,0,48,129,202,49,11,48,9,6,3,85,4,6,19,2,85,83,49,23,48,21,6,3,85,4,10,19,14,86,101,114,105,83,105,103,110,44,32,73,110,99,46,49,31,48,29,6,3,85,4,11,19,22,86,101,114,105,83,105,103,110,32,84,114,117,115,116,32,78,101,116,119,111,114,107,49,58,48,56,6,3,85,4,11,19,49,40,99,41,32,50,48,48,54,32,86,101,114,105,83,105,103,110,44,32,73,110,99,46,32,45,32,70,111,114,32,97,117,116,104,111,114,105,122,101,100,32,117,115,101,32,111,110,108,121,49,69,48,67,6,3,85,4,3,19,60,86,101,114,105,83,105,103,110,32,67,108,97,115,115,32,51,32,80,117,98,108,105,99,32,80,114,105,109,97,114,121,32,67,101,114,116,105,102,105,99,97,116,105,111,110,32,65,117,116,104,111,114,105,116,121,32,45,32,71,53,48,30,23,13,48,54,49,49,48,56,48,48,48,48,48,48,90,23,13,51,54,48,55,49,54,50,51,53,57,53,57,90,48,129,202,49,11,48,9,6,3,85,4,6,19,2,85,83,49,23,48,21,6,3,85,4,10,19,14,86,101,114,105,83,105,103,110,44,32,73,110,99,46,49,31,48,29,6,3,85,4,11,19,22,86,101,114,105,83,105,103,110,32,84,114,117,115,116,32,78,101,116,119,111,114,107,49,58,48,56,6,3,85,4,11,19,49,40,99,41,32,50,48,48,54,32,86,101,114,105,83,105,103,110,44,32,73,110,99,46,32,45,32,70,111,114,32,97,117,116,104,111,114,105,122,101,100,32,117,115,101,32,111,110,108,121,49,69,48,67,6,3,85,4,3,19,60,86,101,114,105,83,105,103,110,32,67,108,97,115,115,32,51,32,80,117,98,108,105,99,32,80,114,105,109,97,114,121,32,67,101,114,116,105,102,105,99,97,116,105,111,110,32,65,117,116,104,111,114,105,116,121,32,45,32,71,53,48,130,1,34,48,13,6,9,42,134,72,134,247,13,1,1,1,5,0,3,130,1,15,0,48,130,1,10,2,130,1,1,0,175,36,8,8,41,122,53,158,96,12,170,231,75,59,78,220,124,188,60,69,28,187,43,224,254,41,2,249,87,8,163,100,133,21,39,245,241,173,200,49,137,93,34,232,42,170,166,66,179,143,248,185,85,183,177,183,75,179,254,143,126,7,87,236,239,67,219,102,98,21,97,207,96,13,164,216,222,248,224,195,98,8,61,84,19,235,73,202,89,84,133,38,229,43,143,27,159,235,245,161,145,194,51,73,216,67,99,106,82,75,210,143,232,112,81,77,209,137,105,123,199,112,246,179,220,18,116,219,123,93,75,86,211,150,191,21,119,161,176,244,162,37,242,175,28,146,103,24,229,244,6,4,239,144,185,228,0,228,221,58,181,25,255,2,186,244,60,238,224,139,235,55,139,236,244,215,172,242,246,240,61,175,221,117,145,51,25,29,28,64,203,116,36,25,33,147,217,20,254,172,42,82,199,143,213,4,73,228,141,99,71,136,60,105,131,203,254,71,189,43,126,79,197,149,174,14,157,212,209,67,192,103,115,227,20,8,126,229,63,159,115,184,51,10,207,93,63,52,135,150,138,238,83,232,37,21,2,3,1,0,1,163,129,178,48,129,175,48,15,6,3,85,29,19,1,1,255,4,5,48,3,1,1,255,48,14,6,3,85,29,15,1,1,255,4,4,3,2,1,6,48,109,6,8,43,6,1,5,5,7,1,12,4,97,48,95,161,93,160,91,48,89,48,87,48,85,22,9,105,109,97,103,101,47,103,105,102,48,33,48,31,48,7,6,5,43,14,3,2,26,4,20,143,229,211,26,134,172,141,142,107,195,207,128,106,212,72,24,44,123,25,46,48,37,22,35,104,116,116,112,58,47,47,108,111,103,111,46,118,101,114,105,115,105,103,110,46,99,111,109,47,118,115,108,111,103,111,46,103,105,102,48,29,6,3,85,29,14,4,22,4,20,127,211,101,167,194,221,236,187,240,48,9,243,67,57,250,2,175,51,49,51,48,13,6,9,42,134,72,134,247,13,1,1,5,5,0,3,130,1,1,0,147,36,74,48,95,98,207,216,26,152,47,61,234,220,153,45,189,119,246,165,121,34,56,236,196,167,160,120,18,173,98,14,69,112,100,197,231,151,102,45,152,9,126,95,175,214,204,40,101,242,1,170,8,26,71,222,249,249,124,146,90,8,105,32,13,217,62,109,110,60,13,110,216,230,6,145,64,24,185,248,193,237,223,219,65,170,224,150,32,201,205,100,21,56,129,201,148,238,162,132,41,11,19,111,142,219,12,221,37,2,219,164,139,25,68,210,65,122,5,105,74,88,79,96,202,126,130,106,11,2,170,37,23,57,181,219,127,231,132,101,42,149,138,189,134,222,94,129,22,131,45,16,204,222,253,168,130,42,109,40,31,13,11,196,229,231,26,38,25,225,244,17,111,16,181,149,252,231,66,5,50,219,206,157,81,94,40,182,158,133,211,91,239,165,125,69,64,114,142,183,14,107,14,6,251,51,53,72,113,184,157,39,139,196,101,95,13,134,118,156,68,122,246,149,92,246,93,50,8,51,164,84,182,24,63,104,92,242,66,74,133,56,84,131,95,209,232,44,242,172,17,214,168,237,99,106};                               
    cert3.write(buf, 297); 
    cert3.close();
  }
}

































#else
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


char aws_key[]         = "AKIAJ7JJ7FHWTUMT6LCA"; 
char aws_secret[]      = "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";

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


#endif

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
 
