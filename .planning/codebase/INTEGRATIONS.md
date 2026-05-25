# External Integrations

**Analysis Date:** 2026-05-25

## APIs & External Services

### MQTT Broker
- **Purpose:** Core IoT messaging bus — all relay commands, state publications, sensor data, and Hermes commands flow through MQTT
- **SDK/Client:** `PubSubClient.h` (Nick O'Leary)
- **Config:** Broker IP, port (default 1883), username, password, client ID — all configurable via WiFiManager captive portal, persisted in NVS namespace `mqtt_config`
- **QoS:** QoS 2 for all 8 relays + master toggle; QoS 0 for all sensor data
- **Retain:** True for relay states, False for sensor readings
- **Keepalive:** 30 seconds (`MQTT_KEEPALIVE` in `MQTTParametros.h`)
- **Buffer:** 2048 bytes (`mqttClient.setBufferSize(2048)` in `ESP32-Automacao-Residencial.ino:228`)
- **Topic pattern:** `ESP32/<Function>[/<Subsensor>]/<Type>` (defined in `TopicosMQTT.h`)
  - Subscribe topics: `ESP32/Interruptor[1-8]/Comando`, `ESP32/Ligar-DesligarTudo/Comando`, `ESP32/Hermes/Comando`
  - Publish topics: `ESP32/Interruptor[1-8]/Estado`, `ESP32/Temperatura`, `ESP32/Umidade`, `ESP32/SensacaoTermica`, `ESP32/BMP180/*`, `ESP32/PontoOrvalho`

### SinricPro (Alexa / Google Home)
- **Purpose:** Voice assistant integration — 2 switches (Bancada, Refletor) + 1 temperature sensor
- **SDK/Client:** `SinricPro.h`, `SinricProSwitch.h`, `SinricProTemperaturesensor.h`
- **Auth:** App Key + App Secret (hardcoded in `ESP32-Automacao-Residencial.ino:130-138`)
- **Devices:**
  - `Bancada` (Relay 2, GPIO 13) — Device ID hardcoded, acts as SinricPro Switch
  - `Refletor` (Relay 8, GPIO 19) — Device ID hardcoded, acts as SinricPro Switch
  - `DHT22 Temp Sensor` — Device ID hardcoded, reports temperature events
- **Transport:** WebSocket (SinricPro SDK internal)
- **Throttling:** Temperature events at most every 60s, only on change >0.5°C or >1% humidity
- **Callback loop:** `SinricPro.handle()` called every ~10ms on Core 0 (`TaskConexoes`)
- **Cross-sync protection:** `sinricProOriginatedChange` flag prevents feedback loops between MQTT/Telegram/Dashboard and SinricPro

### Telegram Bot
- **Purpose:** Interactive remote control with dynamic inline keyboard + push notifications
- **SDK/Client:** `UniversalTelegramBot.h` (via `WiFiClientSecure`)
- **Auth:** BOT_TOKEN (hardcoded in `ESP32-Automacao-Residencial.ino:126`)
- **Chat ID:** Hardcoded (`ESP32-Automacao-Residencial.ino:127`) — unauthorized messages rejected
- **Transport:** HTTPS (WiFiClientSecure with `setInsecure()`)
- **Polling:** Every 1000ms (`BOT_MTBS`)
- **Features:**
  - Reply keyboard with per-relay toggle buttons + LIGAR TUDO / DESLIGAR TUDO / VER SENSORES
  - Boot notification with relay state summary
  - Real-time state change notifications (via inter-core queue)
  - Commands: `/start`, `/on`, `/off`, `/sensores`
- **Thread safety:** Notifications sent via FreeRTOS Queue (`telegramQueue`) from any core, processed only on Core 0

### Home Assistant (MQTT Discovery)
- **Purpose:** Zero-configuration entity registration in Home Assistant via MQTT Discovery protocol
- **SDK/Client:** Uses `PubSubClient` directly (no HA-specific library)
- **Protocol:** Publishes discovery payloads to `homeassistant/<component>/esp32_quarto_robson_v2/<entity>/config`
- **Entities registered (18 total):**
  - 1 master switch (`relay_todos`) — all relays
  - 8 individual switches (`relay_1` through `relay_8`)
  - 9 sensors: Temperature, Humidity, Sensação Térmica, Ponto de Orvalho, BMP180 Temperature, Pressure (real), Pressure (sea level), Altitude (real), Altitude (sea level)
- **Device identity:** Defined in `HomeAssistantDiscovery.h:10-14` (id: `esp32_quarto_robson_v2`, name: `ESP32 Quarto Robson`)
- **Trigger:** Published once on MQTT connect (`pendingHADiscovery` flag)

## Data Storage

**Persistent Storage (NVS):**
- **Relay states** — Namespace `relay states`: 8 boolean relay states + 8 custom name strings
- **MQTT config** — Namespace `mqtt_config`: broker, port, user, password, client ID
- **WiFi IP** — Namespace `wifi-ip`: static IP, gateway, subnet, DNS (DHCP-learned)

**No external database.** All state is stored locally on the ESP32 NVS partition.

## File Storage
- **Local filesystem only** — No SD card, SPIFFS, or LittleFS detected. Dashboard HTML is embedded in firmware as PROGMEM.

## Caching
- **None detected** — No caching layer. All state is in global RAM variables.

## Monitoring & Observability

**Error Tracking:**
- None (serial console only). All errors go to `Serial.println()` which is mirrored via Telnet.

**Logs:**
- Serial output at 115200 baud, mirrored to Telnet clients on port 2323 via `DualSerialClass` (`RemoteDebug.h`)
- Custom `logRelayAction()` function logs relay state changes with source identifier
- `Serial.printf()` scattered throughout for debugging

## CI/CD & Deployment

**Hosting:**
- N/A — firmware runs on ESP32 device

**CI Pipeline:**
- None detected

**Firmware Update:**
- OTA via `ArduinoOTA` on port 3232 with password `S3nh@S3gur@` (hardcoded in `OTAConfig.cpp:16`)
- Task suspension of Core 1 (sensors) during OTA flash to avoid SPI conflicts

## Environment Configuration

**Hardcoded credentials (in `ESP32-Automacao-Residencial.ino`):**
- `BOT_TOKEN` — Telegram bot token
- `CHAT_ID` — Authorized Telegram chat ID
- `SINRICPRO_APP_KEY` / `SINRICPRO_APP_SECRET` — SinricPro authentication
- `SINRICPRO_DEVICE_BANCADA` / `SINRICPRO_DEVICE_REFLETOR` / `SINRICPRO_DEVICE_TEMP_SENSOR` — Device IDs

**Configurable via WiFiManager (persisted in NVS):**
- `mqtt_broker`, `mqtt_port`, `mqtt_user`, `mqtt_pass`, `mqtt_client`

**Hardcoded reference values (in `.ino`):**
- `pressaoNivelMar` = 1012.0 — Sea level pressure reference for altitude calculation
- `altitudeNivelMar` = 92.0 — Location altitude reference

**Secrets location:**
- Embedded in source code (`.ino` file) — not ideal for security

## Webhooks & Callbacks

**Incoming:**
- HTTP REST API on port 8181 (WebServer):
  - `GET /api/relays` — JSON with 8 relay states + names
  - `GET /api/sensors` — JSON with all sensor readings
  - `GET /api/relay/{id}/{state}` — Control individual relay
  - `GET /api/all/{state}` — Control all relays
  - `GET /api/setname/{id}/{name}` — Set custom relay name

**Outgoing:**
- MQTT publish on state change (all relay topics + sensor topics)
- Telegram push notifications (on relay state changes)
- SinricPro events (switch state sync + temperature events)
- Home Assistant Discovery MQTT payloads (on initial connect)

## Networks & Protocols

| Port | Service | Protocol |
|------|---------|----------|
| 8181 | Web Dashboard / REST API | HTTP |
| 3232 | OTA Firmware Update | TCP (ArduinoOTA) |
| 2323 | Telnet Debug Mirror | TCP |
| 1883 | MQTT (configured in WiFiManager) | TCP |
| 53 | mDNS | UDP |

---

*Integration audit: 2026-05-25*
