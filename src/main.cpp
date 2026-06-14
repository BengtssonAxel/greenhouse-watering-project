#include <Arduino.h>
#include <WiFi.h>

#include "config.h"

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("================================");
  Serial.println("ESP32 Greenhouse Watering System");
  Serial.println("Starting up...");
  Serial.println("================================");

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connecting to WiFi");
  unsigned long connectStart = millis();

  while (WiFi.status() != WL_CONNECTED &&
         (millis() - connectStart) < WIFI_CONNECT_TIMEOUT_MS) {
    delay(WIFI_RETRY_DELAY_MS);
    Serial.print(".");
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println();
    Serial.println("WiFi connection timed out.");
    return;
  }

  Serial.println();
  Serial.println("WiFi connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // Keep the program alive for now.
  delay(1000);
}