/*

FAUXMO ESP 2.0.0

Copyright (C) 2016 by Xose PÃŠrez <xose dot perez at gmail dot com>

The MIT License (MIT)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

*/

#ifndef FAUXMOESP_h
#define FAUXMOESP_h

#define DEFAULT_TCP_BASE_PORT   52000
#define UDP_MULTICAST_IP        IPAddress(239,255,255,250)
#define UDP_MULTICAST_PORT      1900
#define TCP_MAX_CLIENTS         10

#define UDP_SEARCH_PATTERN      "M-SEARCH"
#define UDP_DEVICE_PATTERN      "urn:Belkin:device:**"

#define UDP_RESPONSES_INTERVAL  100
#define UDP_RESPONSES_TRIES     5

const char UDP_TEMPLATE[] PROGMEM =
    "HTTP/1.1 200 OK\r\n"
    "CACHE-CONTROL: max-age=86400\r\n"
    "DATE: Sun, 20 Nov 2016 00:00:00 GMT\r\n"
    "EXT:\r\n"
    "LOCATION: http://%s:%d/setup.xml\r\n"
    "OPT: \"http://schemas.upnp.org/upnp/1/0/\"; ns=01\r\n"
    "01-NLS: %s\r\n"
    "SERVER: Unspecified, UPnP/1.0, Unspecified\r\n"
    "ST: urn:Belkin:device:**\r\n"
    "USN: uuid:Socket-1_0-%s::urn:Belkin:device:**\r\n\r\n";

const char SETUP_TEMPLATE[] PROGMEM =
    "<?xml version=\"1.0\"?>"
    "<root><device>"
        "<deviceType>urn:Belkin:device:controllee:1</deviceType>"
        "<friendlyName>%s</friendlyName>"
        "<manufacturer>Belkin International Inc.</manufacturer>"
        "<modelName>FauxmoESP</modelName>"
        "<modelNumber>2.0.0</modelNumber>"
        "<UDN>uuid:Socket-1_0-%s</UDN>"
    "</device></root>";

const char HEADERS[] PROGMEM =
    "HTTP/1.1 200 OK\r\n"
    "CONTENT-LENGTH: %d\r\n"
    "CONTENT-TYPE: text/xml\r\n"
    "DATE: Sun, 01 Jan 2017 00:00:00 GMT\r\n"
    "LAST-MODIFIED: Sat, 01 Jan 2017 00:00:00 GMT\r\n"
    "SERVER: Unspecified, UPnP/1.0, Unspecified\r\n"
    "X-USER-AGENT: redsonic\r\n"
    "CONNECTION: close\r\n\r\n"
    "%s\r\n";

#ifdef DEBUG_FAUXMO
    #define DEBUG_MSG_FAUXMO(...) DEBUG_FAUXMO.printf( __VA_ARGS__ )
#else
    #define DEBUG_MSG_FAUXMO(...)
#endif

#include <Arduino.h>
#include <ESPAsyncTCP.h>
#include <WiFiUdp.h>
#include <functional>
#include <vector>
#include <ESP8266WiFi.h>

typedef std::function<void(unsigned char, const char *, bool)> TStateFunction;

typedef struct {
    char * name;
    char * uuid;
    bool hit;
    AsyncServer * server;
} fauxmoesp_device_t;

class fauxmoESP {

    public:

        fauxmoESP(unsigned int port = DEFAULT_TCP_BASE_PORT);
        void addDevice(const char * device_name);
        void onMessage(TStateFunction fn) { _callback = fn; }
        void enable(bool enable) { _enabled = enable; }
        void handle();

    private:

        bool _enabled = true;
        unsigned int _base_port = DEFAULT_TCP_BASE_PORT;
        std::vector<fauxmoesp_device_t> _devices;
        WiFiUDP _udp;
        AsyncClient * _clients[TCP_MAX_CLIENTS];
        TStateFunction _callback = NULL;

        unsigned int _roundsLeft = 0;
        unsigned int _current = 0;
        unsigned long _lastTick;
        IPAddress _remoteIP;
        unsigned int _remotePort;

        void _sendUDPResponse(unsigned int device_id);
        void _nextUDPResponse();
        void _handleUDPPacket(const IPAddress remoteIP, unsigned int remotePort, uint8_t *data, size_t len);

        void _sendTCPPacket(AsyncClient *client, const char * response);
        AcConnectHandler _getTCPClientHandler(unsigned int device_id);
        void _handleTCPPacket(unsigned int device_id, AsyncClient *client, void *data, size_t len);

};



/*

FAUXMO ESP 2.0.0

Copyright (C) 2016 by Xose PÃŠrez <xose dot perez at gmail dot com>

The MIT License (MIT)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

*/


void fauxmoESP::_sendUDPResponse(unsigned int device_id) {

    fauxmoesp_device_t device = _devices[device_id];
    DEBUG_MSG_FAUXMO("[FAUXMO] UDP response for device #%d (%s)\n", _current, device.name);

    char response[strlen(UDP_TEMPLATE) + 40];
    sprintf_P(response, UDP_TEMPLATE,
        WiFi.localIP().toString().c_str(),
        _base_port + _current, device.uuid, device.uuid
    );

    WiFiUDP udpClient;
    udpClient.beginPacket(_remoteIP, _remotePort);
    udpClient.write(response);
    udpClient.endPacket();

}

void fauxmoESP::_nextUDPResponse() {

    while (_roundsLeft) {
        if (_devices[_current].hit == false) break;
        if (++_current == _devices.size()) {
            --_roundsLeft;
            _current = 0;
        }
    }

    if (_roundsLeft > 0) {
        _sendUDPResponse(_current);
        if (++_current == _devices.size()) {
            --_roundsLeft;
            _current = 0;
        }
    }
}

void fauxmoESP::_handleUDPPacket(IPAddress remoteIP, unsigned int remotePort, uint8_t *data, size_t len) {

    if (!_enabled) return;

    //DEBUG_MSG_FAUXMO("[FAUXMO] Got UDP packet from %s\n", remoteIP.toString().c_str());

    data[len] = 0;
    String content = String((char *) data);

    if (content.indexOf(UDP_SEARCH_PATTERN) == 0) {
        if (content.indexOf(UDP_DEVICE_PATTERN) > 0) {

            DEBUG_MSG_FAUXMO("[FAUXMO] Search request from %s\n", remoteIP.toString().c_str());

            // Set hits to false
            for (unsigned int i = 0; i < _devices.size(); i++) {
                _devices[i].hit = false;
            }

            // Send responses
            _remoteIP = remoteIP;
            _remotePort = remotePort;
            _current = random(0, _devices.size());
            _roundsLeft = UDP_RESPONSES_TRIES;

        }
    }

}

void fauxmoESP::_sendTCPPacket(AsyncClient *client, const char * response) {
    char buffer[strlen(HEADERS) + strlen(response) + 10];
    sprintf_P(buffer, HEADERS, strlen(response), response);
    client->write(buffer);
}

void fauxmoESP::_handleTCPPacket(unsigned int device_id, AsyncClient *client, void *data, size_t len) {

    ((char * )data)[len] = 0;
    String content = String((char *) data);

    fauxmoesp_device_t device = _devices[device_id];

    if (content.indexOf("GET /setup.xml") == 0) {

        DEBUG_MSG_FAUXMO("[FAUXMO] /setup.xml response for device #%d (%s)\n", device_id, device.name);

        _devices[device_id].hit = true;
        char response[strlen(SETUP_TEMPLATE) + 50];
        sprintf_P(response, SETUP_TEMPLATE, device.name, device.uuid);
        _sendTCPPacket(client, response);
        client->close();

    }

    if (content.indexOf("POST /upnp/control/basicevent1") == 0) {

        if (content.indexOf("<BinaryState>0</BinaryState>") > 0) {
            if (_callback) _callback(device_id, device.name, false);
        }

        if (content.indexOf("<BinaryState>1</BinaryState>") > 0) {
            if (_callback) _callback(device_id, device.name, true);
        }

        _sendTCPPacket(client, "");
        client->close();

    }

}

AcConnectHandler fauxmoESP::_getTCPClientHandler(unsigned int device_id) {

    return [this, device_id](void *s, AsyncClient * client) {

        if (!_enabled) return;

        for (int i = 0; i < TCP_MAX_CLIENTS; i++) {
            if (!_clients[i] || !_clients[i]->connected()) {

                _clients[i] = client;

                client->onAck([this, i](void *s, AsyncClient *c, size_t len, uint32_t time) {
                    //DEBUG_MSG_FAUXMO("[FAUXMO] Got ack for client %i len=%u time=%u\n", i, len, time);
                }, 0);

                client->onData([this, i, device_id](void *s, AsyncClient *c, void *data, size_t len) {
                    //DEBUG_MSG_FAUXMO("[FAUXMO] Got data from client %i len=%i\n", i, len);
                    _handleTCPPacket(device_id, c, data, len);
                }, 0);

                client->onDisconnect([this, i](void *s, AsyncClient *c) {
                    //DEBUG_MSG_FAUXMO("[FAUXMO] Disconnect for client %i\n", i);
                    _clients[i]->free();
                }, 0);
                client->onError([this, i](void *s, AsyncClient *c, int8_t error) {
                    //DEBUG_MSG_FAUXMO("[FAUXMO] Error %s (%i) on client %i\n", c->errorToString(error), error, i);
                }, 0);
                client->onTimeout([this, i](void *s, AsyncClient *c, uint32_t time) {
                    //DEBUG_MSG_FAUXMO("[FAUXMO] Timeout on client %i at %i\n", i, time);
                    c->close();
                }, 0);

                return;

            }
        }

        DEBUG_MSG_FAUXMO("[FAUXMO] Rejecting client - Too many connections already.\n");

        // We cannot accept this connection at the moment
        client->onDisconnect([](void *s, AsyncClient *c) {
            delete(c);
        });
        client->stop();

    };

}

void fauxmoESP::addDevice(const char * device_name) {

    fauxmoesp_device_t new_device;
    unsigned int device_id = _devices.size();

    // Copy name
    new_device.name = strdup(device_name);

    // Create UUID
    char uuid[15];
    sprintf(uuid, "444556%06X%02X\0", ESP.getChipId(), device_id); // "DEV" + CHIPID + DEV_ID
    new_device.uuid = strdup(uuid);

    // TCP Server
    new_device.server = new AsyncServer(_base_port + device_id);
    new_device.server->onClient(_getTCPClientHandler(device_id), 0);
    new_device.server->begin();

    // Attach
    _devices.push_back(new_device);

}

void fauxmoESP::handle() {

    int len = _udp.parsePacket();
    if (len > 0) {
        IPAddress remoteIP = _udp.remoteIP();
        unsigned int remotePort = _udp.remotePort();
        uint8_t data[len];
        _udp.read(data, len);
        _handleUDPPacket(remoteIP, remotePort, data, len);
    }

    if (_roundsLeft > 0) {
        if (millis() - _lastTick > UDP_RESPONSES_INTERVAL) {
            _lastTick = millis();
            _nextUDPResponse();
        }
    }

}

fauxmoESP::fauxmoESP(unsigned int port) {

    _base_port = port;

    // UDP Server
    _udp.beginMulticast(WiFi.localIP(), UDP_MULTICAST_IP, UDP_MULTICAST_PORT);

}

#endif
