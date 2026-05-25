# Technology Stack

**Analysis Date:** 2026-05-25

## Languages

**Primary:**
- C++ (Arduino Framework variant) - All firmware logic in `.ino`, `.h`, and `.cpp` files

**Secondary:**
- HTML/CSS/JavaScript - Embedded dashboard UI inside `WebServerESP32.h` as PROGMEM raw string literal
- JSON - API payloads for REST endpoints and MQTT messages

## Platform / Hardware

**Microcontroller:**
- ESP32 WROOM-32 (38 pins) — Dual-core Xtensa LX6 @ 240MHz, 520KB SRAM, 4MB Flash

**Sensors/Actuators:**
- DHT22 (Temperature/Humidity) — GPIO 25
- BMP180 (Pressure/Altitude) — I2C (GPIO 21/22)
- 8-Channel Relay Module 5V (Active-Low) — GPIOs 26, 13, 14, 32, 16, 17, 18, 19
- LED Status Indicator — GPIO 2
- Configuration Button — GPIO 0 (BOOT button, 3-second press for WiFiManager portal)

**Framework:**
- Arduino for ESP32 (Arduino core for ESP32, built on top of FreeRTOS + ESP-IDF)

**Package Manager:**
- Not detected — project uses Arduino IDE library management (no PlatformIO.ini, no package.json)

## Core System Software

**RTOS:**
- FreeRTOS (ESP32 Arduino core built-in) — Dual-core task pinning via `xTaskCreatePinnedToCore()`

**mDNS:**
- ESPmDNS library — Hostname: `automacaoresidencial.local`

**Persistent Storage:**
- ESP32 NVS (Non-Volatile Storage) via `Preferences.h`
  - Namespace `relay states` — relay states and custom names
  - Namespace `mqtt_config` — MQTT broker, port, user, password, client ID
  - Namespace `wifi-ip` — DHCP-learned static IP configuration

## Key Libraries

**Critical (from `Bibliotecas.h` and source includes):**

| Library | Version | Purpose |
|---------|---------|---------|
| `WiFi.h` | ESP32 Arduino built-in | WiFi STA mode with static IP |
| `PubSubClient.h` | ~2.8+ (Nick O'Leary) | MQTT client (QoS 2 relays, QoS 0 sensors, 2048B buffer) |
| `UniversalTelegramBot.h` | ~1.x | Telegram bot with inline keyboards |
| `SinricPro.h` / `SinricProSwitch.h` / `SinricProTemperaturesensor.h` | ~3.x | Amazon Alexa & Google Home voice control |
| `ArduinoJson.h` | ~6.x | JSON serialization/deserialization |
| `DHT.h` | ~1.x (Adafruit) | DHT22 sensor driver |
| `Adafruit_BMP085.h` | ~1.x | BMP180 pressure sensor driver |
| `ArduinoOTA.h` | ESP32 Arduino built-in | Over-the-air firmware updates (port 3232) |
| `WiFiManager.h` | ~2.x (tzapu) | Captive portal for WiFi + MQTT configuration |
| `WebServer.h` | ESP32 Arduino built-in | HTTP REST API + dashboard (port 8181) |
| `ESPmDNS.h` | ESP32 Arduino built-in | mDNS responder |
| `Preferences.h` | ESP32 Arduino built-in | NVS key-value storage |
| `TimeLib.h` | Arduino Time library | NTP time sync |
| `Wire.h` | ESP32 Arduino built-in | I2C bus for BMP180 |

## Build/Dev Configuration

**Build System:**
- Arduino IDE (no `platformio.ini`, `CMakeLists.txt`, or `Makefile` found)

**Dev Dependencies:**
- Python scripts for graph analysis: `generate_visualization.py`, `finalize_graphify.py`, `finalize_fixed.py`
- No linter, formatter, or test framework detected

## Configuration

**Compile-time configuration files:**
- `DefinicoesProjeto.h` — Project name, author, version, hostname
- `GPIOs.h` — Pin mapping constants
- `MQTTParametros.h` — QoS, retain, keepalive settings
- `TopicosMQTT.h` — MQTT topic strings
- `ConstantesTempo.h` — Timing intervals (extern declarations)

**Runtime configuration files:**
- `LoginsSenhas.h` — MQTT credential buffers (populated via WiFiManager, defaults empty)
- `.ino` main file — Hardcoded Telegram BOT_TOKEN, CHAT_ID, SinricPro keys
- `HomeAssistantDiscovery.h` — HA device identity constants

**Serial console:**
- Baud rate: 115200
- Telnet mirror on port 2323 via `RemoteDebug.h`

## Platform Requirements

**Development:**
- Arduino IDE with ESP32 board support package
- Required libraries installed via Arduino Library Manager:
  - PubSubClient, ArduinoJson, UniversalTelegramBot, SinricPro, DHT sensor library, Adafruit BMP085, WiFiManager

**Production:**
- MQTT broker (e.g., Mosquitto, Home Assistant built-in)
- WiFi network (2.4 GHz)
- Optional: Telegram bot token (from BotFather), SinricPro account (for Alexa/Google Home)

---

*Stack analysis: 2026-05-25*
