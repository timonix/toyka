
#include "IPAddress.h"

#ifndef WEBSERVER_H
#define WEBSERVER_H

static const uint8_t NO_DATA_HEADER = 255;
static const uint8_t STICK_DIRECTION_HEADER = 50;
static const uint8_t TARGET_SPEED_HEADER = 51;
static const uint8_t ACCELEROMETER_HEADER = 80;
static const uint8_t GYROSCOPE_HEADER = 81;
static const uint8_t BATTERI_HEADER = 82;

class UdpTransceiver
{

    
    
public:
    
    UdpTransceiver(const char* ssid, const char* pass,const char* ssidAP, const char* passAP);
    void initServer();
    void update();

    void receiveData(int8_t* packet);

    void publishSensor(uint8_t sensorType, float* data, long timeStamp);
    //static int constructReplyFromSensorData(uint8_t* message, uint8_t sensorType, float* data, long time);


private:
    static const int NUM_PEERS = 9;
    IPAddress connectedPeers[NUM_PEERS];

    void addPeer(IPAddress ip);
    //void sendData();
    void reconnect();

    const char* password;
    const char* ssid;
    const char* passwordAP;
    const char* ssidAP;

};



#endif