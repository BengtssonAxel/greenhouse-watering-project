# ESP32 Smart Greenhouse Watering System — Architecture Plan

## 1. Overall system architecture

This project uses two ESP32 boards:

- ESP32 #1 — Outdoor controller
  - Reads four soil moisture sensors
  - Controls four watering valves
  - Reads indoor and outdoor temperature
  - Fetches weather forecasts from a free API
  - Runs the watering decision logic
  - Publishes status to the indoor unit

- ESP32 #2 — Indoor display unit
  - Receives the outdoor controller state over the wireless link
  - Shows the touchscreen UI
  - Allows manual override actions and threshold adjustment
  - Sends user commands back to the outdoor unit

### Recommended topology

- Use Wi-Fi + MQTT for the two-ESP32 link because:
  - the indoor unit needs a user interface and future extensibility
  - MQTT is simple to maintain and debug
  - both boards can stay on the same network with stable message routing
  - it supports future expansion such as cloud logging or OTA updates

- Use ESP-NOW only if the system must run without a local Wi-Fi router. It is lower latency but less flexible for UI/control messaging and harder to scale.

### Why MQTT is the better fit here

For this greenhouse project, MQTT is recommended because:

1. The outdoor controller is the main logic node and the indoor unit is mainly a UI/control node.
2. MQTT supports publish/subscribe messaging with clean state updates.
3. It is easy to add future features such as remote monitoring or diagnostics.
4. It avoids the complexity of peer-to-peer addressing that ESP-NOW requires for multi-node control.

## 2. Recommended communication strategy

### Link between the two ESP32 boards

- Use MQTT over Wi-Fi
- Broker: local MQTT broker on the same LAN, or a lightweight broker such as Mosquitto
- Outdoor unit publishes:
  - /greenhouse/status
  - /greenhouse/zone/+/reading
  - /greenhouse/weather
  - /greenhouse/valves
- Indoor unit publishes:
  - /greenhouse/commands
  - /greenhouse/ui/override

### Message model

- Status payload from outdoor unit:
  - soil moisture for all four zones
  - indoor temperature
  - outdoor temperature
  - valve on/off state
  - rain forecast and next watering decision
  - current system mode

- Commands from indoor unit:
  - manual valve open/close
  - skip next scheduled watering
  - adjust moisture thresholds for one zone
  - force watering for one zone

## 3. Weather API recommendation

Preferred option:

- SMHI Open Data API (Swedish Meteorological and Hydrological Institute)
  - free
  - no API key required
  - strong support for Swedish locations
  - metric units (Celsius, mm)

Alternative option:

- Open-Meteo API
  - free
  - no API key required
  - supports metric units
  - good for global locations

Recommendation:

- Start with SMHI for local compliance and Swedish weather relevance.
- Use Open-Meteo as a fallback if SMHI forecast endpoints become inconvenient for the ESP32 implementation.

## 4. Watering logic overview

The outdoor controller should combine:

- current soil moisture per zone
- current temperature
- indoor vs outdoor temperature difference
- upcoming precipitation forecast
- seasonal daylight/temperature adjustment for Scandinavian conditions

### Suggested logic rules

- If a zone is below the moisture threshold, start watering.
- If rain is expected within the next 6–12 hours, delay watering.
- If temperature is very low, reduce watering duration or skip watering to avoid stress.
- In early spring and late autumn, use a slightly more conservative threshold because plant demand is lower.
- In summer, increase attention to drying conditions and longer daylight hours.

This logic should be implemented in one central controller module so the display unit only shows the current state and user overrides.

## 5. Folder and file structure

The project should be organized as follows:

```text
greenhouse-watering-project/
  README.md
  platformio.ini
  src/
    main.cpp
    config.h
    config.cpp
    utils/
      types.h
      logging.h
    sensors/
      soil_moisture.h
      soil_moisture.cpp
      temperature.h
      temperature.cpp
    valves/
      valve_controller.h
      valve_controller.cpp
    wifi/
      wifi_manager.h
      wifi_manager.cpp
      mqtt_client.h
      mqtt_client.cpp
    weather/
      weather_client.h
      weather_client.cpp
      weather_model.h
    logic/
      watering_logic.h
      watering_logic.cpp
      scheduler.h
      scheduler.cpp
    display/
      display_ui.h
      display_ui.cpp
      touch_input.h
      touch_input.cpp
    comms/
      message_protocol.h
      message_protocol.cpp
  docs/
    architecture-plan.md
```

## 6. Suggested responsibilities by module

- main.cpp
  - initialize hardware, Wi-Fi, MQTT, display, and scheduler
  - run the main loop

- config.h / config.cpp
  - Wi-Fi credentials
  - MQTT broker address
  - zone names and thresholds
  - valve timing and watering duration

- sensors/\*
  - soil and temperature reading abstractions

- valves/\*
  - valve open/close timing and safety checks

- wifi/\*
  - Wi-Fi connection, reconnection, and MQTT publishing/subscribing

- weather/\*
  - fetch forecast data and parse the API response

- logic/\*
  - determine whether watering should happen
  - apply seasonal adjustment, rain skip, and threshold logic

- display/\*
  - touchscreen rendering and controls

## 7. Recommended implementation order

1. Create the basic PlatformIO project skeleton
2. Implement sensor and valve abstractions
3. Add Wi-Fi and MQTT communication
4. Add weather API fetch and parsing
5. Add watering decision logic
6. Add indoor touchscreen display and manual controls
7. Verify the two-ESP32 data flow end to end

## 8. Notes for Dalby, Sweden

- Use metric units only
- Temperature in Celsius
- Precipitation in millimeters
- Seasonal logic should reflect Scandinavian daylight variation and cooler spring/fall conditions
- The weather forecast should be treated as a key input to avoid unnecessary watering before rain
