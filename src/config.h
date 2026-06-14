#pragma once

#include <Arduino.h>

// Replace these with your actual Wi-Fi credentials before flashing the ESP32.
extern const char WIFI_SSID[];
extern const char WIFI_PASSWORD[];

// Optional connection tuning values for the startup routine.
extern const uint32_t WIFI_CONNECT_TIMEOUT_MS;
extern const uint32_t WIFI_RETRY_DELAY_MS;
