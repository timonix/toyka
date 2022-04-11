
#ifndef WEBSERVER_H
#define WEBSERVER_H

class TcpServer
{

    
    
public:
    
    TcpServer(const char* ssid, const char* pass,const char* ssidAP, const char* passAP);
    void initServer();
    void update();

    void publishBat(uint8_t batData);
    void publishGyro(float *gyroData);
    void publishAcc(float* accData);
    void publishMag(float* magData);



private:
	
	uint8_t *createBuffer(float *data, uint8_t header);
	void send(uint8_t *buffer, uint8_t len);
	
    const char* password;
    const char* ssid;
    const char* passwordAP;
    const char* ssidAP;

};



#endif