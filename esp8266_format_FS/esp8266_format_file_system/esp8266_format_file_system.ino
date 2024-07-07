#include <FS.h>
#include <LittleFS.h>

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("Starting LittleFS formatting...");
  
  if (!LittleFS.begin()) {
    Serial.println("LittleFS mount failed");
    return;
  }
  
  Serial.println("Formatting LittleFS filesystem...");
  LittleFS.format();
  Serial.println("LittleFS formatted successfully");

  // Optionally, you can unmount LittleFS after formatting
  LittleFS.end();
  
  // End the sketch
  ESP.restart(); // Restart ESP8266 to apply formatting
}

void loop() {
  // This loop is not used, as setup() performs one-time actions
}
