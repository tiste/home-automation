#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

const char* ssid = "........";
const char* password = "........";

ESP8266WebServer server(80);

// GPIO2, pin 4
const int relay = 2;

void handleRoot() {
  server.send(200, "text/plain", "this is home, call /open");
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void setup(void) {
  pinMode(relay, OUTPUT);
  digitalWrite(relay, 0);
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("garage-door")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);
  server.on("/open",[]() {
    digitalWrite(relay, 1);
    delay(2000);
    digitalWrite(relay, 0);

    server.send(200, "text/plain", "garage is opening");
  });
  server.onNotFound(handleNotFound);
  server.begin();

  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
}
