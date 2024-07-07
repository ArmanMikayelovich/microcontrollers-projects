#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FS.h>
#include <LittleFS.h>
#include <time.h>
/* Установите здесь свои SSID и пароль */
const char* own_wifi_ssid = "Service_Config_Server";  // SSID
const char* own_wifi_password = "10203055";           // пароль

IPAddress local_ip(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
ESP8266WebServer server(80);


String ssid = "";
String password = "";
String mqtt_broker = "";
String mqtt_topic = "";
String mqtt_username = "";
String mqtt_password = "";



void setup() {
  delay(1000);  // Delay before serial setup

  Serial.begin(115200);
  if (!LittleFS.begin()) {
    Serial.println("LittleFS mount failed");
    return;
  }
  // Check if wifi_mqtt file exists
  if (LittleFS.exists("/wifi_mqtt")) {
    File file = LittleFS.open("/wifi_mqtt", "r");
    if (file) {
      ssid = file.readStringUntil('\n').c_str();
      password = file.readStringUntil('\n').c_str();  // Consider securely handling password
      mqtt_broker = file.readStringUntil('\n').c_str();
      mqtt_topic = file.readStringUntil('\n').c_str();
      mqtt_username = file.readStringUntil('\n').c_str();
      mqtt_password = file.readStringUntil('\n').c_str();  // Consider securely handling password
      file.close();
      Serial.println("successfully read data from file");
      // Print read data to verify
      Serial.print("SSID: ");
      Serial.println(ssid);
      Serial.print("Password: ");
      Serial.println(password);
      Serial.print("MQTT Broker: ");
      Serial.println(mqtt_broker);
      Serial.print("MQTT Topic: ");
      Serial.println(mqtt_topic);
      Serial.print("MQTT Username: ");
      Serial.println(mqtt_username);
      Serial.print("MQTT Password: ");
      Serial.println(mqtt_password);
    }
  } else {
    Serial.println("No data from File System...");
    WiFi.softAP(own_wifi_ssid, own_wifi_password);
    WiFi.softAPConfig(local_ip, gateway, subnet);
    delay(100);

    server.on("/", HTTP_GET, handleRoot);
    server.on("/save", HTTP_POST, handleSave);

    server.begin();
    Serial.println("HTTP server started");
  }
}

void handleRoot() {
  String html = "<html><body><h1>Node Control Panel</h1>";
  html += "<form method='post' action='/save'>";
  html += "WiFi SSID: <input type='text' name='ssid' value=''><br>";
  html += "WiFi Password: <input type='password' name='password'><br>";  // Password input should be blank for security
  html += "MQTT Broker: <input type='text' name='mqtt_broker' value=''><br>";
  html += "MQTT Topic: <input type='text' name='mqtt_topic' value=''><br>";
  html += "MQTT Username: <input type='text' name='mqtt_username' value=''><br>";
  html += "MQTT Password: <input type='password' name='mqtt_password'><br>";  // Password input should be blank for security
  html += "<input type='submit' value='Save'></form>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void handleSave() {
  bool isValidData = server.hasArg("ssid") && server.hasArg("mqtt_broker")
   && server.hasArg("mqtt_topic") && server.hasArg("mqtt_username");
  if (isValidData) {
    ssid = server.arg("ssid").c_str();  // Update ssid
    // Update other variables if necessary
    mqtt_broker = server.arg("mqtt_broker").c_str();
    mqtt_topic = server.arg("mqtt_topic").c_str();
    mqtt_username = server.arg("mqtt_username").c_str();
    mqtt_password = server.arg("mqtt_password").c_str();

   
    createFile();
    writeInFile();
    
    server.send(200, "text/plain", "Configuration saved. Restarting...");
    delay(1000);
    ESP.restart();
  } else {
    server.send(400, "text/plain", "Missing arguments");
  }
}

void createFile(){
 // Check if /wifi_mqtt file exists
    if (!LittleFS.exists("/wifi_mqtt")) {
      Serial.println("File /wifi_mqtt does not exist. Creating...");

      // Open file in write mode (create it)
      File file = LittleFS.open("/wifi_mqtt", "w");
      if (!file) {
        Serial.println("Failed to create file");
      } else {
        Serial.println("File created successfully");
      }
    }
}

void writeInFile(){
// Save to LittleFS
    File file = LittleFS.open("/wifi_mqtt", "w");
    if (!file) {
      Serial.println("Failed to create file");
    } else {
      file.println(ssid);
      file.println(password);  // Consider storing password securely if needed
      file.println(mqtt_broker);
      file.println(mqtt_topic);
      file.println(mqtt_username);
      file.println(mqtt_password);  // Consider storing password securely if needed
      file.close();
    }
}


void loop() {
  server.handleClient();
}

void handle_NotFound() {
  server.send(404, "text/plain", "Not found");
}
