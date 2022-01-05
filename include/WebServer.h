
#ifndef WEBSERVER_H
#define WEBSERVER_H

class WebServer
{
    
public:
    
    WebServer(const char* ssid, const char* pass,const char* ssidAP, const char* passAP);
    void initWebSocket();
    void updateWebSocket();
    int getSpeed();
    int getAngle();


private:

    void notifyClients();
    void handleWebSocketMessage(void *arg, uint8_t *data, size_t len);
    //void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);

    const char* password;
    const char* ssid;
    const char* passwordAP;
    const char* ssidAP;

};




#endif