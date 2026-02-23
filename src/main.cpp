#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include "progmem_html.hpp"

#define EN_A 12
#define EN_B 13
#define IN_1 4
#define IN_2 5
#define IN_3 19
#define IN_4 18

#define SSID "MULTIBOT"
#define PASSWORD "MULTI123"

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

char in1_state = 0;
char in2_state = 0;
char in3_state = 0;
char in4_state = 0;
char ena_state = 0;
char enb_state = 0;

enum motor_state
{
  STOP = 0,
  FORWARD = 1,
  BACKWARD = 2
};

void motor_1_control(char state)
{
  switch(state)
  {
    case 0:
      in1_state = 0;
      in2_state = 0;
      break;
    case 1:
      in1_state = 0;
      in2_state = 1;
      break;
    case 2:
      in1_state = 1;
      in2_state = 0;
      break;
    default:
      break;
  }
}

void motor_2_control(char state)
{
  switch(state)
  {
    case 0:
      in3_state = 0;
      in4_state = 0;
      break;
    case 1:
      in3_state = 0;
      in4_state = 1;
      break;
    case 2:
      in3_state = 1;
      in4_state = 0;
      break;
    default:
      break;
  }
}

void wifi_init()
{
  WiFi.softAP(SSID, PASSWORD);
  WiFi.softAPConfig(IPAddress(192,168,4,1), IPAddress(192,168,4,1), IPAddress(255,255,255,0));
}

void motor_control_selectHandler(AsyncWebServerRequest* request)
{
  request->send(200, "text/html", motor_control_select_html);
}

void motor_1_controlHandler(AsyncWebServerRequest* request)
{
  request->send(200, "text/html", motor_control_1_html);
}

void motor_2_controlHandler(AsyncWebServerRequest* request)
{
  request->send(200, "text/html", motor_control_2_html);
}

void onWebSocketEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) 
{
  if (type == WS_EVT_CONNECT) {
    Serial.println("Client connected");
    client->text("Connected to WebSocket Server");
  }
  else if (type == WS_EVT_DISCONNECT) {
    Serial.println("Client disconnected");
  }
  else if (type == WS_EVT_DATA) {
    // Handle incoming WebSocket messages
    String command = "";
    for (size_t i = 0; i < len; i++) {
      command += (char)data[i];
    }
    Serial.print("Received: ");
    Serial.println(command);

    if(command == "forward_1")
    {
      motor_1_control(FORWARD);
    }
    else if(command == "backward_1")
    {
      motor_1_control(BACKWARD);
    }
    else if(command == "stop_1")
    {
      motor_1_control(STOP);
    }
    if(command == "forward_2")
    {
      motor_2_control(FORWARD);
    }
    else if(command == "backward_2")
    {
      motor_2_control(BACKWARD);
    }
    else if(command == "stop_2")
    {
      motor_2_control(STOP);
    }
  }
}

void setup() 
{
  pinMode(EN_A, OUTPUT);
  pinMode(EN_B, OUTPUT);
  pinMode(IN_1, OUTPUT);
  pinMode(IN_2, OUTPUT);
  pinMode(IN_3, OUTPUT);
  pinMode(IN_4, OUTPUT);

  analogWrite(EN_A, 255);
  analogWrite(EN_B, 255);

  Serial.begin(9600);

  wifi_init();

  ws.onEvent(onWebSocketEvent);
  server.addHandler(&ws);

  server.on("/", HTTP_GET, motor_control_selectHandler);
  server.on("/control/1", HTTP_GET, motor_1_controlHandler);
  server.on("/control/2", HTTP_GET, motor_2_controlHandler);
  server.begin();
}

void loop()
{
  ws.cleanupClients();
  digitalWrite(IN_1, in1_state);
  digitalWrite(IN_2, in2_state);
  digitalWrite(IN_3, in3_state);
  digitalWrite(IN_4, in4_state);
}


