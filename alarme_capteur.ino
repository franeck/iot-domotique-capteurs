#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>

/* ================================= Wifi ================================= */
const char* ssid = "";  // Enter SSID here
const char* password = "";  //Enter Password here

/* ================================= Web Server ================================= */

WebServer server(80);
String baseIP = "192.168.1.62";
int sensorID = 1;

/* ================================= Sensor ================================= */

#define SENSOR_PIN 35
boolean entraceLastOpen = false;
boolean entranceOpen = false;
int sensorNextCheck = 0;
int entranceNextCheck = 0;
boolean sensorState = true;
boolean sensorLastState[4] = {false, false, false, false};
int lastStatePointer = 0;

/* ================================= Setup ================================= */

void setup() {
  Serial.begin(115200);

  // Wifi connection
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) 
  {
     delay(500);
  }

  // Http server routing
  server.on("/state", handleState);
  server.onNotFound(handleNotFound);
  server.begin();

  // Sensor
  pinMode(SENSOR_PIN, INPUT);
}

/* ================================= LOOP ================================= */

void loop() {
  server.handleClient();
  checkSensor(150);
  checkEntrance(700);
  if(entranceOpen != entraceLastOpen) {
    sendEvent();
  }
}

/* ================================= HTTP ================================= */

void handleState() {
  DynamicJsonDocument doc(1024);
  doc["open"] = entranceOpen;
  String body;
  serializeJson(doc, body);
  server.send(200, "application/json", body); 
}

void handleNotFound(){
  server.send(404, "text/plain", "Not found");
}

/* ================================= Sensor ================================= */

void checkSensor(int checkDelay) {
  if(millis() > sensorNextCheck) {
    if(lastStatePointer > 3) {
      lastStatePointer = 0;
    }
    sensorLastState[lastStatePointer] = !digitalRead(SENSOR_PIN);
    sensorNextCheck = millis() + checkDelay;
    lastStatePointer += 1;
  } 
}

void checkEntrance(int checkDelay) {
  if(millis() > entranceNextCheck) {
    entraceLastOpen = entranceOpen;
    int stateSum = 0;
    for(int i=0; i < 4; i++) {
      stateSum += sensorLastState[i];
    }
    if(stateSum == 4) {
      entranceOpen = true;
    } else if (stateSum == 0) {
      entranceOpen = false;
    } else {
      // Change nothing
    }
    entranceNextCheck = millis() + checkDelay;
  }
}

/* ================================= Contact Base ================================= */

void sendEvent() {
  Serial.println("Sending event, opening: "+String(entranceOpen));
  HTTPClient http;
  http.begin("http://"+baseIP+"/event");
  http.addHeader("Content-Type", "text/json");
  DynamicJsonDocument doc(1024);
  JsonObject root = doc.to<JsonObject>();
  JsonObject event = root.createNestedObject("event");
  event["sensorId"] = sensorID; 
  event["opening"] = entranceOpen;
  String body;
  serializeJson(root, body);
  int httpCode = http.POST(body);
  http.end();
  entraceLastOpen = entranceOpen;
}
