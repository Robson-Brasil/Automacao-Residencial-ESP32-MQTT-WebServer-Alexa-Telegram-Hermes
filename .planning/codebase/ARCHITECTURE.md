<!-- refreshed: 2026-05-25 -->
# Architecture

**Analysis Date:** 2026-05-25

## System Overview

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                         USER INTERFACES                                     │
├──────────────────┬─────────────────────┬─────────────────┬─────────────────┤
│  Telegram Bot    │  Web Dashboard      │  Alexa/Google   │  MQTT Clients   │
│  (Push + Reply)  │  REST API + UI      │  (SinricPro)    │  (Node-RED, HA) │
│  Port: HTTPS     │  Port: 8181 HTTP    │  Port: WebSock  │  Port: 1883 TCP │
└─────────┬────────┴──────────┬──────────┴────────┬────────┴────────┬────────┘
          │                   │                    │                 │
          ▼                   ▼                    ▼                 ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│                     CORE 0 — TaskConexoes (Connectivity)                    │
│                     Pinned to PRO_CPU (Core 0)                              │
│                                                                             │
│  ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────┐                   │
│  │  WiFi    │  │  MQTT    │  │ Telegram │  │ SinricPro│                   │
│  │ Manager  │  │ PubSub   │  │  Bot     │  │ (Alexa/  │                   │
│  │ + mDNS   │  │ Client   │  │ Polling  │  │ Google)  │                   │
│  └──────────┘  └──────────┘  └──────────┘  └──────────┘                   │
│  ┌──────────┐  ┌──────────┐  ┌──────────┐                                  │
│  │   OTA    │  │  Telnet  │  │ HA       │                                  │
│  │ Updates  │  │  Debug   │  │ Discovery│                                  │
│  └──────────┘  └──────────┘  └──────────┘                                  │
└─────────────────────────────────────────────────────────────────────────────┘
          │                    ▲                    │
          │  IPC: Flags       │  IPC: Queue        │  IPC: Status LED
          │  (pendingMqtt*)   │  (telegramQueue)   │  (simple GPIO)
          ▼                    │                    ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│                     CORE 1 — TaskSensores + loop()                          │
│                     Pinned to APP_CPU (Core 1)                              │
│                                                                             │
│  ┌──────────────────┐  ┌──────────────────┐  ┌──────────────────┐          │
│  │   Sensor Reads   │  │   Relay Control  │  │   Web Server     │          │
│  │   DHT22 (60s)    │  │   8 GPIO Pins    │  │   Dashboard +    │          │
│  │   BMP180 (120s)  │  │   Active-Low     │  │   REST API       │          │
│  │   Dew Point calc │  │   NVS Persist    │  │   Port 8181      │          │
│  └──────────────────┘  └──────────────────┘  └──────────────────┘          │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
          │
          ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│                     HARDWARE LAYER                                          │
│                                                                             │
│  ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────┐                   │
│  │ 8-Ch     │  │  DHT22   │  │  BMP180  │  │  Status  │                   │
│  │ Relay    │  │ Temp/Hum │  │ Pressure │  │  LED     │                   │
│  │ Module   │  │ GPIO 25  │  │ I2C:21/22│  │  GPIO 2  │                   │
│  │ GPIOs    │  │          │  │          │  │          │                   │
│  │ 26,13,14,│  │          │  │          │  │          │                   │
│  │ 32,16,18 │  │          │  │          │  │          │                   │
│  │ 17,19    │  │          │  │          │  │          │                   │
│  └──────────┘  └──────────┘  └──────────┘  └──────────┘                   │
└─────────────────────────────────────────────────────────────────────────────┘
```

## Component Responsibilities

| Component | Responsibility | File |
|-----------|----------------|------|
| Main Entry Point | System init, setup(), loop(), all core logic | `ESP32-Automacao-Residencial.ino` |
| Library Includes | Centralized dependency management | `Bibliotecas.h` |
| Global State | All shared variables, flags, handles, queue | `VariaveisGlobais.h` + `VariaveisGlobais.cpp` |
| GPIO Mapping | Pin number constants | `GPIOs.h` |
| Project Metadata | Name, version, author, hostname | `DefinicoesProjeto.h` |
| MQTT Topics | All subscribe/publish topic strings | `TopicosMQTT.h` |
| MQTT Parameters | QoS, retain, keepalive configs | `MQTTParametros.h` |
| WiFi Manager | Captive portal, NVS persistence for MQTT/IP | `WiFiManagerConfig.h` |
| WiFi Utils | WiFi connection prototypes | `WiFiUtils.h` |
| Telegram Bot | Bot polling, inline keyboard, notifications | `TelegramBotESP32.h` |
| SinricPro | Alexa/Google Home switches + temp sensor | `SinricProESP32.h` |
| Web Server | Dashboard HTML + REST API endpoints | `WebServerESP32.h` |
| Sensors | Sensor read prototypes | `Sensores.h` |
| HA Discovery | Home Assistant auto-discovery MQTT payloads | `HomeAssistantDiscovery.h` |
| OTA Config | Over-the-air update setup + callbacks | `OTAConfig.h` + `OTAConfig.cpp` |
| Remote Debug | DualSerial (Telnet mirror of Serial) | `RemoteDebug.h` |
| FreeRTOS Tasks | Task function prototypes | `TaskCores.h` |
| MQTT Config | ConfigMQTT prototypes | `ConfigMQTT.h` |
| Timing Constants | Time interval declarations | `ConstantesTempo.h` |
| Credentials | MQTT buffer defs + extern for Telegram/SinricPro | `LoginsSenhas.h` |

## Pattern Overview

**Overall:** Singleton FreeRTOS dual-core firmware with flat header-structured C++. All logic lives in headers and one `.ino` file, with minimal `.cpp` files.

**Key Characteristics:**
- **Polling architecture** — No interrupts for relay control. Callbacks attached for MQTT and SinricPro; all other loops poll on ~10ms intervals
- **Flat module organization** — 19 header files at root level, no subdirectories for source code
- **Inter-core communication** via volatile boolean flags (`pendingMqttUpdate`, `pendingSinricProUpdate`, `pendingSensorMqttUpdate`, `pendingSensorSinricUpdate`, `pendingHADiscovery`) and a FreeRTOS Queue (`telegramQueue`)
- **Centralized state** — All relay states, sensor buffers, and connection flags declared `extern` in `VariaveisGlobais.h`, defined in `VariaveisGlobais.cpp`

## Layers

### Connectivity Layer (Core 0 — TaskConexoes)
- **Purpose:** All network protocol handling
- **Location:** `ESP32-Automacao-Residencial.ino` lines 769-894
- **Contains:** WiFi connection, MQTT client loop, Telegram polling, SinricPro handle, OTA handle, Telnet server, HA Discovery publish, inter-core flag processing, status LED
- **Depends on:** WiFi hardware, NVS storage
- **Used by:** All external services (MQTT broker, Telegram API, SinricPro servers, HTTP clients)

### Sensor & Actuator Layer (Core 1 — TaskSensores + loop())
- **Purpose:** Physical I/O: sensor reads, relay state application, web server
- **Location:** `ESP32-Automacao-Residencial.ino` lines 763-766 (loop), 898-914 (TaskSensores)
- **Contains:** DHT22 reads (60s interval), BMP180 reads (120s interval), relay pin writes, dew point calculation, `webServer.handleClient()`
- **Depends on:** GPIO hardware, I2C bus
- **Used by:** System state (via inter-core flags)

### Hardware Abstraction Layer
- **GPIO constants** in `GPIOs.h`
- **Sensor libraries** via `Bibliotecas.h` (DHT.h, Adafruit_BMP085.h)
- **NVS persistence** via `Preferences.h` — `WiFiManagerConfig.h` and `WebServerESP32.h` read/write NVS directly

## Data Flow

### Primary Request Path (Web Dashboard → Relay)

1. HTTP GET to `http://<ESP32_IP>:8181/api/relay/0/1` (`WebServerESP32.h:71-94`)
2. `handleWebServerClient()` in `loop()` (Core 1) dispatches to `handleApiRelay()`
3. `digitalWrite()` sets GPIO pin, updates global `RelayState1`, calls `saveRelayState()` to NVS
4. `notifyTelegramStateChange()` enqueues notification via `xQueueSend()` to Core 0
5. Sets `pendingMqttUpdate = true` and `pendingSinricProUpdate = true` (volatile flags)
6. On next Core 0 tick (~10ms), `TaskConexoes` reads flags:
   - Calls `publishRelayStates()` → `mqttClient.publish()` to MQTT topics
   - Calls `sendSinricProSwitchEvents()` → SinricPro WebSocket event
7. On next Core 0 tick, `telegramQueue` is drained → `sendMessageWithReplyKeyboard()` via HTTPS

### Primary Request Path (MQTT → Relay)

1. MQTT message arrives on `ESP32/Interruptor1/Comando` with payload `"1"`
2. `mqttCallback()` in `TaskConexoes` (Core 0) matches topic to `sub1`
3. Sets `RelayState1 = true`, calls `digitalWrite(RelayPin1, !RelayState1)`
4. Calls `saveRelayState(1, true)` (NVS), `notifyTelegramStateChange("MQTT", 0, true)` (queue)
5. Sets `pendingSinricProUpdate = true` for cross-sync
6. State change is published back to MQTT on next loop iteration

### Primary Request Path (Telegram → Relay)

1. `telegramLoop()` polls Telegram API every 1000ms (Core 0)
2. If a button like "Varanda ⚡ LIGAR" is pressed, `handleTelegramMessages()` toggles `RelayState1`
3. Same GPIO/NVS/publish sequence as above
4. Response sent back with updated keyboard showing new state

### Primary Request Path (Alexa/Google Home → Relay)

1. `SinricPro.handle()` (Core 0, every ~10ms) receives WebSocket command
2. `onPowerState()` callback fires, sets `sinricProOriginatedChange = true`
3. Updates relay state, GPIO, NVS, sets `pendingMqttUpdate = true`
4. Calls `notifyTelegramStateChange()` (queue)
5. After callback returns, `sinricProOriginatedChange` is cleared
6. `sendSinricProSwitchEvents()` short-circuits when `sinricProOriginatedChange` is true

### Sensor Data Flow

1. `TaskSensores` (Core 1) calls `readSensors()` every ~10ms
2. Internal timing logic reads DHT22 every 60s, BMP180 every 120s
3. Data stored in global char buffers (via `dtostrf()`)
4. Sets `pendingSensorMqttUpdate = true` flag
5. Core 0 reads flag and calls `publishSensorData()` → MQTT publish on 6 sensor topics
6. If DHT data changed, sets `pendingSensorSinricUpdate = true`
7. Core 0 sends temperature event to SinricPro (throttled to 60s, min 0.5°C delta)

### Hermes Command Flow

1. MQTT message on `ESP32/Hermes/Comando` (Core 0)
2. `mqttCallback()` parses natural-language commands: `LIGAR:SALA`, `DESLIGAR:TUDO`, `STATUS`, `SENSORES`
3. Same GPIO/NVS/notification chain as MQTT relay commands
4. STATUS command publishes JSON response to `ESP32/Hermes/StatusResposta`

**State Management:**
- Global volatile bools in RAM (8 relay states + connection flags)
- NVS persistence via `Preferences.h` for relay states (on change) and MQTT config (via WiFiManager)
- Sensor data in char buffers (`str_temp_data[7]`, etc.) updated on read
- Inter-core flags `pendingMqttUpdate`, `pendingSinricProUpdate`, `pendingSensorMqttUpdate`, `pendingSensorSinricUpdate`, `pendingHADiscovery` are volatile shared booleans (not mutex-protected, but single-writer pattern)
- Telegram notifications use FreeRTOS Queue (`telegramQueue`) for thread-safe IPC

## Key Abstractions

**DualSerialClass (`RemoteDebug.h`):**
- Purpose: Mirrors all `Serial.println()` output to Telnet clients
- Implementation: Inherits `Print`, overrides `write()` to send to both physical UART and any connected Telnet client
- Pattern: `#define Serial DualSerial` replaces all `Serial` references globally

**TelegramNotification (`VariaveisGlobais.h`):**
- Purpose: Thread-safe notification data structure for inter-core Telegram delivery
- Fields: `char source[32]` (origin string), `int relayNum`, `bool relayState`
- Transport: FreeRTOS `QueueHandle_t telegramQueue` (size 10)

**Global State Pattern:**
- All shared state: `VariaveisGlobais.h` (extern declarations) + `VariaveisGlobais.cpp` (definitions)
- Accessed by name across all translation units
- No encapsulation, no getter/setter abstraction

## Entry Points

**System Boot (`ESP32-Automacao-Residencial.ino:689-760`):**
1. Serial init (115200)
2. Load MQTT config from NVS
3. Configure GPIO pins for relays, LED, button
4. Create FreeRTOS Queue for Telegram notifications
5. Initialise NVS namespace `relay states` and load saved relay states
6. WiFi mode STA + hostname
7. Init sensors (DHT, I2C/BMP180)
8. Start Web Server on port 8181
9. Create two FreeRTOS tasks pinned to Core 0 and Core 1
10. `loop()` runs `handleWebServerClient()` + 10ms delay (Core 1)

**Core 0 Task (`TaskConexoes`, lines 769-894):**
- 10ms loop: WiFi check, MQTT reconnect/client loop, Telegram polling, SinricPro handle, OTA handle, inter-core flag processing, Telnet server, LED update

**Core 1 Task (`TaskSensores`, lines 898-914):**
- 10ms loop: `readSensors()` (with internal timing), periodic MQTT sync flag (every 60s)

## Cross-Cutting Concerns

**Logging:**
- All output via `Serial.println()` — the `#define Serial DualSerial` in `RemoteDebug.h` redirects to both USB UART and Telnet (port 2323)
- `logRelayAction(source, relayNum, state)` provides structured relay change logging
- No log levels, no filtering

**Validation:**
- DHT22 readings checked for `isnan()` before use
- BMP180 pressure checked for non-zero before use
- No input validation on MQTT messages beyond string prefix matching
- Telegram chat ID checked against hardcoded value (unauthorized messages rejected)
- WiFiManager sets/gets trusted input for MQTT credentials

**Authentication:**
- MQTT: username/password (configurable via WiFiManager)
- Telegram: hardcoded CHAT_ID whitelist
- OTA: hardcoded password `S3nh@S3gur@`
- SinricPro: App Key + App Secret (hardcoded)
- Web Dashboard: No authentication (open HTTP on port 8181)
- Telnet: No authentication (open TCP on port 2323)

**Timing Architecture:**
- Non-blocking pattern using `millis()` throughout
- WiFi retry: every 1s (`WIFI_RETRY_INTERVAL`)
- MQTT reconnect: every 5s (`RECONNECT_INTERVAL`)
- DHT22 read: every 60s
- BMP180 read: every 120s
- Telegram poll: every 1000ms (`BOT_MTBS`)
- Relay MQTT sync: every 60s
- SinricPro temp event: minimum 60s between events, only on significant change
- Main loop delay: 10ms (`vTaskDelay(10 / portTICK_PERIOD_MS)`)

---

*Architecture analysis: 2026-05-25*
