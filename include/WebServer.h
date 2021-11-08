
#ifndef WEBSERVER_H
#define WEBSERVER_H

class WebServer
{
    
public:
    
    WebServer(const char* SSID, const char* pass);
    void initWebSocket();
    void updateWebSocket();


private:

    void notifyClients();
    void handleWebSocketMessage(void *arg, uint8_t *data, size_t len);
    //void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);

    const char* password;
    const char* ssid;

};




#endif