
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "UdpTransceiver.h"

#define PORT 42100

WiFiUDP UDP;

unsigned long previousMillis;
boolean reconnected = false;


UdpTransceiver::UdpTransceiver(const char *ssid, const char *pass, const char *ssidAP, const char *passAP)
{

    for(int i = 0; i<NUM_PEERS;i++){
        connectedPeers[i] = INADDR_ANY;
    }
    
    this->ssid = ssid;
    this->password = pass;
    this->ssidAP = ssidAP;
    this->passwordAP = passAP;
}

void UdpTransceiver::initServer()
{
    IPAddress local_IP(192,168,4,1);
    IPAddress gateway(192,168,4,1);
    IPAddress subnet(255,255,255,0);
    
    WiFi.softAPConfig(local_IP, gateway, subnet);
    WiFi.softAP(ssidAP, passwordAP);


      // Connecting to WiFi...
    Serial.print("Connecting to ");
    Serial.println(ssid);

      // Connected to WiFi
    Serial.print("IP address for network ");
    Serial.print(ssidAP);
    Serial.print(" : ");
    Serial.println(WiFi.softAPIP());

    UDP.begin(PORT);
    Serial.print("Listening on UDP port ");
    Serial.println(PORT);

}


static int hashIP(IPAddress ip, uint8_t modulo){
    return (ip[2]+ip[3])%modulo;
}

void UdpTransceiver::addPeer(IPAddress ip){
    connectedPeers[hashIP(ip,NUM_PEERS-1)] = ip;
}

void UdpTransceiver::receiveData(int8_t* packet){

    int packetSize = UDP.parsePacket();
    if (packetSize) {
        UDP.read((byte*)packet, 255);
        addPeer(UDP.remoteIP());
        return;
    }
    packet[0] = NO_DATA_HEADER;
}

static int constructReplyFromSensorData(uint8_t* message, uint8_t sensorType, float* data, long time){
    message[0] = sensorType;
    uint32_t index = 1;
    for (uint32_t i = 0; i<sizeof(long);i++){
        message[index] = data[i];
        index++;
    }

    for (uint32_t i = 0; i<sizeof(float)*3+1;i++){
        message[index] = data[i];
        index++;
    }
    
    return index;
}


void UdpTransceiver::publishSensor(uint8_t sensorType, float* data, long timeStamp){
    uint8_t msg[10];
    int message_len = constructReplyFromSensorData(msg, sensorType, data, timeStamp);
    for(auto & peer : connectedPeers){
        UDP.beginPacket(peer, PORT);
        UDP.write(msg,message_len);
        UDP.endPacket();
    }
}

void UdpTransceiver::reconnect(){
    unsigned long currentMillis = millis();
    if ((WiFi.status() != WL_CONNECTED) && (currentMillis - previousMillis >=10000)) {
        Serial.print("Connecting to ");
        Serial.print(ssid);
        Serial.println();
        WiFi.disconnect();
        WiFi.begin(ssid, password);
        previousMillis = currentMillis;
        reconnected = true;
    }
    if (WiFi.status() == WL_CONNECTED && reconnected) {
        reconnected = false;
        Serial.println();
        Serial.println("Connected!");
        Serial.print("IP address for network ");
        Serial.print(ssid);
        Serial.print(" : ");
        Serial.println(WiFi.localIP());
    }
}

void UdpTransceiver::update()
{
    reconnect();

}