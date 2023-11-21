// #include <Arduino.h>
#include "FS.h"
#include <LittleFS.h>
#include "WiFi.h"  // <Wifi.h>
// #include "json.h"
#include <ESPAsyncWebServer.h>  // To run wifi in Soft Access Point (SAP) mode (standalone w/o router)
#include <ESPmDNS.h>
// #include <AsyncJson.h>
#define FORMAT_LITTLEFS_IF_FAILED true
const char *ap_ssid = "colorchess";
const char *ap_password = "checkmate";
// const char *ssid = "#SFO FREE WIFI";
// const char *password = "";
const char *ssid = "mywifi";
// uint8_t bssid[6] = {0x6e,0x06,0x53,0x3e,0xe6,0xbf};
const char *password = "mypwd";

IPAddress localip(192,168,1,69);
IPAddress gateway(192,168,1,5);
IPAddress subnet(255,255,255,0);
AsyncWebServer server(80);
String getRandomColor();
String processor();
void cleanLittleFS() {
    Serial.println("Cleaning LittleFS...");
    LittleFS.format();
    Serial.println("LittleFS cleaned.");
}
void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
    Serial.printf("Listing directory: %s\r\n", dirname);

    File root = fs.open(dirname);
    if(!root){
        Serial.println("- failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        Serial.println(" - not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if(levels){
                listDir(fs, file.path(), levels -1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("\tSIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
    Serial.println("- finished listing files");
}
void setup() {
    Serial.begin(115200);
    delay(1000);

    // cleanLittleFS();
    // delay (1000000);
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    
    Serial.print("Setting up Access Point ... ");
    Serial.println(WiFi.softAPConfig(localip, gateway, subnet) ? "Ready" : "Failed!");
    Serial.print("Starting Access Point ... ");
    Serial.println(WiFi.softAP(ap_ssid, ap_password) ? "Ready" : "Failed!");  // Set up ESP32 as an access point
    Serial.print("IP address = ");
    Serial.println(WiFi.softAPIP());  // Print the IP address of the ESP32

    if (!LittleFS.begin(FORMAT_LITTLEFS_IF_FAILED)) printf("Littlefs mount failed...\n");
    // WiFi.begin(ssid, password);  // Connect to existing Wi-Fi
    // while (WiFi.status() != WL_CONNECTED) { delay(1000); Serial.println("Connecting to WiFi..."); }
    // Serial.println("Connected to WiFi");

    listDir(LittleFS, "/", 3);

    // server.serveStatic("/chess", LittleFS, "/index.html");
    server.on("/tester", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(200, "text/plain", "Hello, tester");
    });  // This works!! when I connect to colorchess ssid wifi and browse to 192.168.1.69/tester .

    
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
// void scan(char* myssid) {
void scan() {
    printf("\nScanning.. ");
    int n = WiFi.scanNetworks();
    if (n != 0) {
        printf("found %d networks\n", n);
        Serial.println("Nr | SSID                             | RSSI | CH | Encryption");
        for (int i = 0; i < n; ++i) {
            // printf ("network %d: %s (%d) %s\n", i+1, WiFi.SSID(i), WiFi.RSSI(i), (WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? "open" : "***");
            // delay(50);
            Serial.print(n);
            // Print SSID and RSSI for each network found
            Serial.printf("%2d",i + 1);
            Serial.print(" | ");
            // char* newname = new String(WiFi.SSID(i));
            Serial.printf("%-32.32s", WiFi.SSID(i).c_str());
            Serial.printf("%-32.32s", WiFi.BSSIDstr(i).c_str());
            // if (!strcmp(newname.c_str(), myssid.))
            Serial.print(" | ");
            Serial.printf("%4d", WiFi.RSSI(i));
            Serial.print(" | ");
            Serial.printf("%2d", WiFi.channel(i));
            Serial.print(" | ");
            switch (WiFi.encryptionType(i))
            {
            case WIFI_AUTH_OPEN:
                Serial.print("open");
                break;
            case WIFI_AUTH_WEP:
                Serial.print("WEP");
                break;
            case WIFI_AUTH_WPA_PSK:
                Serial.print("WPA");
                break;
            case WIFI_AUTH_WPA2_PSK:
                Serial.print("WPA2");
                break;
            case WIFI_AUTH_WPA_WPA2_PSK:
                Serial.print("WPA+WPA2");
                break;
            case WIFI_AUTH_WPA2_ENTERPRISE:
                Serial.print("WPA2-EAP");
                break;
            case WIFI_AUTH_WPA3_PSK:
                Serial.print("WPA3");
                break;
            case WIFI_AUTH_WPA2_WPA3_PSK:
                Serial.print("WPA2+WPA3");
                break;
            case WIFI_AUTH_WAPI_PSK:
                Serial.print("WAPI");
                break;
            default:
                Serial.print("unknown");
            }
            Serial.println();
            delay(10);

        }
    }
    else printf("no networks found\n");
}
void connect() {
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}
String processor(const String &var) {
    if (var == "CHESSBOARD_COLOR") return getRandomColor();  // Replace placeholders in the HTML with dynamic content
    return String();
}
String getRandomColor() {
    return String("rgb(") + String(random(256)) + "," + String(random(256)) + "," + String(random(256)) + ")";  // Generate a random RGB color
}

void loop() {
//     scan();
//   //  delay(500);
//     connect();
    // if (scan(ssid)) {
    //     connect();
    //     while (true);
    // }
    // else delay(5000);
}
// void listFiles() {
//     Serial.println("Listing files in LittleFS:");
//     Dir dir = LittleFS.openDir("/");
//     while (dir.next()) {
//         Serial.print("FILE: ");
//         Serial.println(dir.fileName());
//     }
// }
