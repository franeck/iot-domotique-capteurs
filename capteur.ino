#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "internet";
const char* password = "doudou69";

int inputPin = 13;

WebServer server(80);

void setup() {

  // Connexion wifi
  WiFi.begin(ssid, password);    
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    delay(3000);
    ESP.restart();
  }
  
  Serial.begin(9600);
  
  server.on("/", handle_OnConnect);
  server.onNotFound(handle_NotFound);
  pinMode(inputPin, INPUT);
  
  Serial.println(WiFi.localIP());
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
  Serial.println(digitalRead(inputPin));
}

void handle_OnConnect() {
  server.send(200, "text/html", "test");
}

void handle_NotFound(){
  server.send(404, "text/plain", "Not found");
}
