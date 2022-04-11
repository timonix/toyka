
#include <ESP8266WiFi.h>

#include <algorithm> // std::min
#include "TcpServer.h"


#define STACK_PROTECTOR 512 // bytes

const int port = 59681;

    const uint8_t GYROSCOPE_HEADER = 50;
    const uint8_t ACCELEROMETER_HEADER = 51;
    const uint8_t MAGNETOMETER_HEADER = 52;
    const uint8_t BATTERY_HEADER = 53;

// how many clients should be able to telnet to this ESP8266
#define MAX_SRV_CLIENTS 2

WiFiServer server(port);
WiFiClient serverClients[MAX_SRV_CLIENTS];

TcpServer::TcpServer(const char *ssid, const char *pass, const char *ssidAP, const char *passAP)
{
    this->ssid = ssid;
    this->password = pass;
    this->ssidAP = ssidAP;
    this->passwordAP = passAP;
}

void TcpServer::initServer()
{

    WiFi.mode(WIFI_AP);
    //WiFi.begin(ssid, password);
    WiFi.softAP(ssid, password);
    Serial.println(ssid);
    Serial.println(password);
    /*while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }*/
    Serial.println();
    Serial.print("connected, address=");
    Serial.println(WiFi.localIP());

    // start server
    server.begin();
    server.setNoDelay(true);

    Serial.print("Ready! Use 'telnet ");
    Serial.print(WiFi.localIP());
    Serial.printf(" %d' to connect\n", port);
}

uint8_t *TcpServer::createBuffer(float *data, uint8_t header)
{
    uint8_t *sbuf = (uint8_t *)data;

    uint8_t *buffer_with_header = new uint8_t[1 * sizeof(uint8_t) + 4 * sizeof(float)];

    buffer_with_header[0] = GYROSCOPE_HEADER;
    for (uint8_t i = 1; i < 1 * sizeof(uint8_t) + 4 * sizeof(float); i++)
        buffer_with_header[i] = sbuf[i];

    return buffer_with_header;
}

void TcpServer::send(uint8_t *buffer, uint8_t len)
{

    // push UART data to all connected telnet clients
    for (uint8_t i = 0; i < MAX_SRV_CLIENTS; i++)
        // if client.availableForWrite() was 0 (congested)
        // and increased since then,
        // ensure write space is sufficient:
        if (serverClients[i].availableForWrite() >= len)
        {
            size_t tcp_sent = serverClients[i].write(buffer, len);
            if (tcp_sent != len)
            {
                // failed to send
                // logger->printf("len mismatch: available:%zd serial-read:%zd tcp-write:%zd\n", len, serial_got, tcp_sent);
            }
        }
}

void TcpServer::publishGyro(float *gyroData)
{
    uint8_t *buffer_with_header = createBuffer(gyroData, GYROSCOPE_HEADER);
    send(buffer_with_header, 1 * sizeof(uint8_t) + 4 * sizeof(float));
}

void TcpServer::publishAcc(float *accData)
{
    uint8_t *buffer_with_header = createBuffer(accData, ACCELEROMETER_HEADER);
    send(buffer_with_header, 1 * sizeof(uint8_t) + 4 * sizeof(float));
}

void TcpServer::publishMag(float *magData)
{
    uint8_t *buffer_with_header = createBuffer(magData, MAGNETOMETER_HEADER);
    send(buffer_with_header, 1 * sizeof(uint8_t) + 4 * sizeof(float));
}

void TcpServer::publishBat(uint8_t batteryLevel){
    uint8_t *buffer_with_header = new uint8_t[2];
    buffer_with_header[0] = BATTERY_HEADER;
    buffer_with_header[1] = batteryLevel;
    send(buffer_with_header,2);
}


void TcpServer::update()
{
    // check if there are any new clients
    if (server.hasClient())
    {
        // find free/disconnected spot
        int i;
        for (i = 0; i < MAX_SRV_CLIENTS; i++)
            if (!serverClients[i])
            { // equivalent to !serverClients[i].connected()
                serverClients[i] = server.available();
                Serial.print("New client: index ");
                Serial.print(i);
                break;
            }

        // no free/disconnected spot so reject
        if (i == MAX_SRV_CLIENTS)
        {
            server.available().println("busy");
            // hints: server.available() is a WiFiClient with short-term scope
            // when out of scope, a WiFiClient will
            // - flush() - all data will be sent
            // - stop() - automatically too
            Serial.printf("server is busy with %d active connections\n", MAX_SRV_CLIENTS);
        }
    }

    for (int i = 0; i < MAX_SRV_CLIENTS; i++)
        while (serverClients[i].available() && Serial.availableForWrite() > 0)
        {
            // working char by char is not very efficient
            Serial.write(serverClients[i].read());
        }

    // determine maximum output size "fair TCP use"
    // client.availableForWrite() returns 0 when !client.connected()
    int maxToTcp = 0;
    for (int i = 0; i < MAX_SRV_CLIENTS; i++)
        if (serverClients[i])
        {
            int afw = serverClients[i].availableForWrite();
            if (afw)
            {
                if (!maxToTcp)
                {
                    maxToTcp = afw;
                }
                else
                {
                    maxToTcp = std::min(maxToTcp, afw);
                }
            }
            else
            {
                // warn but ignore congested clients
                Serial.println("one client is congested");
            }
        }
}