# Findings - ESP32 Project Analysis

## Project Overview
- **Project Name:** ESP32 - Sistema IoT
- **Platform:** ESP32 (Multi-core FreeRTOS)
- **Status:** Functional and robust. Critical logic bugs and FreeRTOS concurrency risks have been successfully resolved.

## Phase 1: Configuration & Structure
- **Hardcoded Credentials:** WiFi, MQTT, Telegram, and SinricPro credentials are hardcoded in the `.ino` file, ignoring the `extern` declarations in `LoginsSenhas.h`.
- **Multiple Definitions Risk:** Several headers (`TelegramBotESP32.h`, `SinricProESP32.h`, `WebServerESP32.h`) define functions and variables directly. This will cause "multiple definition" errors if the project grows and these headers are included in more than one `.cpp`.
- **Redundant Definitions:** GPIO pins and constants are defined both in `.h` files and the `.ino` file.
- **Include-All Anti-pattern:** `Bibliotecas.h` includes everything, which creates high coupling and slow compilation.

## Phase 2: Core Infrastructure (WiFi, OTA, Time)
- **WiFi Resilience:** `setupWiFi` is called in both `setup()` and `TaskConexoes`. While it works, it's redundant.
- **OTA File Naming:** `OTAConfig.cpp` is included like a header and contains guards. It should be a proper `.h` and `.cpp` pair.
- **System Blocking:** If `bmp.begin()` fails, the system enters an infinite `while(1)`, preventing WiFi, OTA, and all other services from starting.

## Phase 3: Communication Services
- **MQTT QoS:** `MQTTParametros.h` sets QoS 2, but `PubSubClient` only supports QoS 0. The settings are currently ignored.
- **Telegram (CRITICAL BUG):** Logic inversion in `handleTelegramMessages`. Sending "ON" sets the pin `HIGH` (which is OFF for active-low relays) and sets `RelayState` to `false`. This is the opposite of the MQTT and Web Dashboard logic.
- **SinricPro Integration:** Limited to only 2 relays (Bancada and Refletor). The other 6 cannot be controlled via Alexa/Google Home.
- **Naming Inconsistency:** Relay names in Telegram (`relayNames`) differ from names in WebServer (`DEFAULT_NAMES`).

## Phase 4: Application Logic & Resource Management
- **Thread Safety:** Global flags like `pendingMqttUpdate` are used for inter-core communication but are not marked `volatile` or protected by semaphores, which could lead to race conditions.
- **WebServer Buffers:** The 512-byte JSON buffer in `WebServerESP32.h` is risky and might overflow as relay names grow.

## Phase 5: Architecture Refactoring (Resolved)
- **Thread Safety & Race Conditions (FIXED):** The previous direct calls to `mqttClient.publish` and SinricPro from Core 1 were removed. The system now safely routes all network updates to Core 0 using `pendingSensorMqttUpdate` and `pendingSensorSinricUpdate` flags.
- **Telegram Memory Corruption (FIXED):** The global `pendingTelegramNotify` `String` variable was causing a race condition between Cores. It was replaced by a native FreeRTOS `QueueHandle_t` (`telegramQueue`) transmitting a thread-safe `struct` with a `char[]` buffer.
- **Telegram Logic Inversion (FIXED):** The inverted active-low logic for individual relays in `TelegramBotESP32.h` (e.g., `Varanda ON` writing `HIGH`) was corrected to properly set `LOW` and `true`.

## Specific Issues & Improvements
1.  **DONE:** Inverted Telegram logic to match active-low hardware and MQTT state.
2.  **DONE:** Implemented FreeRTOS xQueue and safe flags for Inter-Core Communication (IPC).
3.  **FIX:** Remove infinite loop in `bmp.begin()` to allow system to run even without one sensor.
4.  **IMPROVE:** Move all definitions from `.h` to `.cpp` files.
5.  **IMPROVE:** Properly use `LoginsSenhas.h` and a corresponding `.cpp` for credentials.
6.  **IMPROVE:** Consolidate relay names in a single configuration file.
