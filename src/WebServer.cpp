
#include <sstream>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "WebServer.h"

static char html_page[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <title>
        Mousebot
    </title>
    <meta name="viewport" content="user-scalable=no">
</head>
<body  style="position: fixed; font-family: 'Gill Sans', 'Gill Sans MT', Calibri, 'Trebuchet MS', sans-serif ;
color:rgb(128, 128, 128);
font-size: xx-large;">
    <h1 style="text-align:center">
        MOUSEBOT </h1>
    <p style="text-align: center;">
        X: <span id="x_coordinate"> </span>
        Y: <span id="y_coordinate"> </span>
        Speed: <span id="speed"> </span> %
        Angle: <span id="angle"> </span>
    </p>
    <canvas id="canvas" name="game"></canvas>
    <script>
        var gateway = `ws://${window.location.hostname}/ws`;
        var connection = new WebSocket(gateway);
        connection.onopen = function () {
            connection.send('Connect ' + new Date());
        };
        connection.onerror = function (error) {
            console.log('WebSocket Error ', error);
            alert('WebSocket Error ', error);
        };
        connection.onmessage = function (e) {
            console.log('Server: ', e.data);
        };

        function send(x,y,speed,angle){
            var data = {"x":x,"y":y,"s":speed,"a":angle};
            data = JSON.stringify(data);
            console.log(data);
            connection.send(data);
        }
        function send_speed(speed){
            connection.send("s"+speed);
        }
        function send_angle(angle){
            connection.send("a"+angle);
        }


    </script>
    <script>
        var canvas, ctx;

        window.addEventListener('load', () => {

            canvas = document.getElementById('canvas');
            ctx = canvas.getContext('2d');          
            resize(); 

            document.addEventListener('mousedown', startDrawing);
            document.addEventListener('mouseup', stopDrawing);
            document.addEventListener('mousemove', Draw);

            document.addEventListener('touchstart', startDrawing);
            document.addEventListener('touchend', stopDrawing);
            document.addEventListener('touchcancel', stopDrawing);
            document.addEventListener('touchmove', Draw);
            window.addEventListener('resize', resize);

            document.getElementById("x_coordinate").innerText = 0;
            document.getElementById("y_coordinate").innerText = 0;
            document.getElementById("speed").innerText = 0;
            document.getElementById("angle").innerText = 0;

        });

        var width, height, radius, x_orig, y_orig;
        function resize() {
            width = window.innerWidth;
            radius = 200;
            height = radius * 6.5;
            ctx.canvas.width = width;
            ctx.canvas.height = height;
            background();
            joystick(width / 2, height / 3);
        }

        function background() {
            x_orig = width / 2;
            y_orig = height / 3;

            ctx.beginPath();
            ctx.arc(x_orig, y_orig, radius + 20, 0, Math.PI * 2, true);
            ctx.fillStyle = '#ECE5E5';
            ctx.fill();
        }

        function joystick(width, height) {
            ctx.beginPath();
            ctx.arc(width, height, radius, 0, Math.PI * 2, true);
            ctx.fillStyle = '#F08080';
            ctx.fill();
            ctx.strokeStyle = '#F6ABAB';
            ctx.lineWidth = 8;
            ctx.stroke();
        }

        let coord = { x: 0, y: 0 };
        let paint = false;

        function getPosition(event) {
            var mouse_x = event.clientX || event.touches[0].clientX;
            var mouse_y = event.clientY || event.touches[0].clientY;
            coord.x = mouse_x - canvas.offsetLeft;
            coord.y = mouse_y - canvas.offsetTop;
        }

        function is_it_in_the_circle() {
            var current_radius = Math.sqrt(Math.pow(coord.x - x_orig, 2) + Math.pow(coord.y - y_orig, 2));
            if (radius >= current_radius) return true
            else return false
        }


        function startDrawing(event) {
            paint = true;
            getPosition(event);
            if (is_it_in_the_circle()) {
                ctx.clearRect(0, 0, canvas.width, canvas.height);
                background();
                joystick(coord.x, coord.y);
                Draw();
            }
        }


        function stopDrawing() {
            paint = false;
            ctx.clearRect(0, 0, canvas.width, canvas.height);
            background();
            joystick(width / 2, height / 3);
            document.getElementById("x_coordinate").innerText = 0;
            document.getElementById("y_coordinate").innerText = 0;
            document.getElementById("speed").innerText = 0;
            document.getElementById("angle").innerText = 0;

            send_speed(0);
            send_angle(0);

        }

        function Draw(event) {

            if (paint) {
                ctx.clearRect(0, 0, canvas.width, canvas.height);
                background();
                var angle_in_degrees,x, y, speed;
                var angle = Math.atan2((coord.y - y_orig), (coord.x - x_orig));

                if (Math.sign(angle) == -1) {
                    angle_in_degrees = Math.round(-angle * 180 / Math.PI);
                }
                else {
                    angle_in_degrees =Math.round( 360 - angle * 180 / Math.PI);
                }


                if (is_it_in_the_circle()) {
                    joystick(coord.x, coord.y);
                    x = coord.x;
                    y = coord.y;
                }
                else {
                    x = radius * Math.cos(angle) + x_orig;
                    y = radius * Math.sin(angle) + y_orig;
                    joystick(x, y);
                }

            
                getPosition(event);

                var speed =  Math.round(100 * Math.sqrt(Math.pow(x - x_orig, 2) + Math.pow(y - y_orig, 2)) / radius);

                var x_relative = Math.round(x - x_orig);
                var y_relative = Math.round(y - y_orig);
                

                document.getElementById("x_coordinate").innerText =  x_relative;
                document.getElementById("y_coordinate").innerText =y_relative ;
                document.getElementById("speed").innerText = speed;
                document.getElementById("angle").innerText = angle_in_degrees;

                //send( x_relative,y_relative,speed,angle_in_degrees);
                send_speed(speed);
                send_angle(angle_in_degrees);
            }
        } 
    </script>
</body>
</html><br>
)rawliteral";

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

int speed;
int angle;

WebServer::WebServer(const char* ssid, const char* pass)
{
    this->ssid = ssid;
    this->password = pass;

}

int WebServer::getAngle(){
    return angle;
}

int WebServer::getSpeed(){
    return speed;
}

void WebServer::updateWebSocket() {
    ws.cleanupClients();
}

void notifyClients() {
    ws.textAll("NOTIFY STRING");
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) { 
    AwsFrameInfo *info = (AwsFrameInfo*)arg;
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
        data[len] = 0;
        std::string data_as_string((char*)data);
        std::string newString(data_as_string.substr(1));
        std::stringstream ss(newString);
        if (data[0] == 's') {
            ss >> speed;
        }
        if (data[0] == 'a') {
            ss >> angle;
        }

      notifyClients();
    }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len){
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

String processor(const String& var){
    Serial.println(var);
    return "processor return";
}

void WebServer::initWebSocket() {
    WiFi.begin(ssid);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi..");
    }

    // Print ESP Local IP Address
    Serial.println(WiFi.localIP());

    ws.onEvent(onEvent);
    server.addHandler(&ws);

      // Route for root / web page
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send_P(200, "text/html", html_page, processor);
    });

    // Start server
    server.begin();
}