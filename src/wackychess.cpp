#include <ESPAsyncWebServer.h>
// #include "FS.h"
#include <LittleFS.h>
#include <WiFi.h>
#include "json.h"
String getRandomColor();
String processor();


const char *ap_ssid = "colorchess";
const char *ap_password = "checkmate";

IPAddress localip(192,168,1,69);
IPAddress gateway(192,168,1,5);
IPAddress subnet(255,255,255,0);

AsyncWebServer server(80);

void setup() {
  Serial.begin(115200);

  Serial.print("Setting up Access Point ... ");
  Serial.println(WiFi.softAPConfig(localip, gateway, subnet) ? "Ready" : "Failed!");

  Serial.print("Starting Access Point ... ");
  Serial.println(WiFi.softAP(ap_ssid, ap_password) ? "Ready" : "Failed!");  // Set up ESP32 as an access point

  Serial.print("IP address = ");
  Serial.println(WiFi.softAPIP());  // Print the IP address of the ESP32

  // LittleFS.begin();
  // WiFi.begin(ssid, password);  // Connect to existing Wi-Fi
  // while (WiFi.status() != WL_CONNECTED) { delay(1000); Serial.println("Connecting to WiFi..."); }
  // Serial.println("Connected to WiFi");

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    AsyncWebServerResponse *response = request->beginResponse(LittleFS, "/index.html", "text/html");  // Serve the HTML page
    request->send(response);
  });

  server.on("/chessboard.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    AsyncWebServerResponse *response = request->beginResponse(LittleFS, "/chessboard.js", "application/javascript");  // Serve the JavaScript file
    request->send(response);
  });

  server.begin();  // Start server
}

void loop() {
}
String processor(const String &var) {
  if (var == "CHESSBOARD_COLOR") return getRandomColor();  // Replace placeholders in the HTML with dynamic content
  return String();
}
String getRandomColor() {
  return String("rgb(") + String(random(256)) + "," + String(random(256)) + "," + String(random(256)) + ")";  // Generate a random RGB color
}