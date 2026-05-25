# Coding Conventions

**Analysis Date:** 2026-05-25

## Naming Patterns

**Files:**
- PascalCase for all source files: `WiFiUtils.h`, `TopicosMQTT.h`, `ConfigMQTT.h`, `OTAConfig.cpp`
- The main sketch file uses the project folder name: `ESP32-Automacao-Residencial.ino`
- Names are descriptive, in Portuguese for domain logic, English for technical files

**Functions:**
- **camelCase** for all functions throughout: `setupWiFi()`, `reconnectMQTT()`, `readSensors()`, `publishRelayStates()`, `handleApiRelay()`, `notifyTelegramStateChange()`
- Task functions (FreeRTOS) use PascalCase: `TaskConexoes()`, `TaskSensores()` — these are function-style C wrappers
- Lambda callbacks in C++ use PascalCase: `.onStart([](){...})`, `.onEnd([](){...})` in `OTAConfig.cpp`
- Web server route handlers use camelCase: `handleRoot()`, `handleApiRelays()`, `handleApiSensors()`

**Variables:**
- **camelCase** for local and member variables: `lastWifiRetryTime`, `currentMillis`, `newState`, `messageTemp`, `otaInitialized`
- **ALL_CAPS_SNAKE** for `#define` constants: `HA_DEVICE_ID`, `MQTT_KEEPALIVE`, `DEVICE_HOSTNAME`, `PROJETO_NOME`, `RelayPin1`, `DHTPIN`, `I2C_SDA`
- **ALL_CAPS_SNAKE** for `const` primitive globals with `PROGMEM`: `DASHBOARD_HTML`
- **hungarian-prefix** for global sensor string buffers: `str_temp_data`, `str_hum_data`, `str_tempterm_data`, `str_dewpoint_data` — the `str_` prefix indicates these are `char[]` arrays storing formatted sensor readings
- Boolean flags use descriptive names: `wifiConnected`, `mqttConnected`, `pendingMqttUpdate`, `otaInProgress`, `mdnsStarted`
- Relay state booleans use PascalCase as global externs: `RelayState1`–`RelayState8`, `Todos`

**Types:**
- **PascalCase** for `struct` types: `TelegramNotification` in `VariaveisGlobais.h`
- **PascalCase** for `enum` types: `RelayStates { RELAY_OFF, RELAY_ON }` in `VariaveisGlobais.h`
- **PascalCase** for `class` names: `DualSerialClass` in `RemoteDebug.h`
- OTA error enum constants: `OTA_AUTH_ERROR`, `OTA_BEGIN_ERROR` etc. (from ArduinoOTA library)

## Include Organization

**Order:**
1. Arduino/built-in includes: `<Preferences.h>`, `<WiFi.h>`, `<ArduinoOTA.h>`
2. Third-party library includes: `<UniversalTelegramBot.h>`, `<ArduinoJson.h>`, `<SinricPro.h>`, `<PubSubClient.h>`, `<DHT.h>`, `<Adafruit_BMP085.h>`
3. FreeRTOS includes: `<freertos/FreeRTOS.h>`, `<freertos/task.h>`
4. Project-local includes: `"Bibliotecas.h"`, `"ConfigMQTT.h"`, `"VariaveisGlobais.h"`

In `Bibliotecas.h`, all third-party includes are centralized as a single inclusion point.

**Include Guards:**
- All `.h` files use `#ifndef FILENAME_H` / `#define FILENAME_H` / `#endif` pattern consistently
- Example from `VariaveisGlobais.h`:
```cpp
#ifndef VARIAVEISGLOBAIS_H
#define VARIAVEISGLOBAIS_H
...
#endif // VARIAVEISGLOBAIS_H
```

**Include Paths:**
- Local headers are included with quoted paths: `#include "Bibliotecas.h"`
- Third-party headers use angle brackets: `#include <PubSubClient.h>`
- No subdirectory paths (all headers in root project directory)

## Code Style

**Formatting:**
- No automatic formatter detected (no `.clang-format`, `.prettierrc`, or `biome.json`)
- 2-space indentation throughout most files, with occasional 4-space blocks
- Opening braces on the same line for functions; there is some inconsistency in brace style across files

**Comment Style:**
- **Doxygen/JSDoc-style** for file headers and function documentation:
```cpp
/**
 * @file TelegramBotESP32.h
 * @brief Bot do Telegram com Teclado Interativo (Botões)
 */
```
```cpp
/**
 * @brief Adiciona uma notificação à fila para envio seguro no Core 0.
 */
void notifyTelegramStateChange(...);
```
- **Section separators** with `// =====` pattern for grouping related code:
```cpp
// =============== FUNÇÕES DE PERSISTÊNCIA ===============
// =============== INSTÂNCIAS DE OBJETOS ===============
```
- **Bullet-style** comments for inline explanations:
```cpp
// Nota: Os relés padrão de 5V costumam ativar em nível lógico BAIXO
```
- **Emoji notation** in debug serial output (Telegram-specific): `"✅ Hermes SENSORES: solicitado"`

**Line Length:**
- No enforced limit; some lines extend beyond 120 characters (e.g., MQTT topic strings, HA discovery payloads)

## Imports & Forward Declarations

**Forward Declarations:**
- Used extensively in the main `.ino` file before the `setup()` function:
```cpp
void setupWiFi();
void setupMQTT();
void reconnectMQTT();
void readSensors();
```
- Extern declarations for global variables/functions shared across files:
```cpp
extern void saveRelayState(int relayNum, bool state);
extern void publishRelayStates();
extern void logRelayAction(const char* source, int relayNum, bool state);
extern bool mqttConnected;
```

## Error Handling

**Strategy:** Serial logging-only; no exceptions, no error codes returned.

**Patterns:**
- Sensor read failures are checked with `isnan()`:
```cpp
if (isnan(temp_data) || isnan(hum_data)) {
    Serial.println("Falha na leitura do sensor DHT22!");
    return;
}
```
- Hardware init failures logged but not fatal:
```cpp
if (!bmp.begin()) {
    Serial.println("[ERRO] BMP180 não encontrado!");
}
```
- MQTT connection failures logged with return code:
```cpp
if (mqttClient.connect(clientID, LoginDoMQTT, SenhaMQTT)) {
    // success path
} else {
    Serial.print("Falha na conexão MQTT, rc=");
    Serial.println(mqttClient.state());
}
```
- No retry limits or circuit-breaker patterns beyond the `if(connected)` checks

## Logging

**Framework:** Direct `Serial.println()` calls throughout; no logging library.

**Patterns:**
- Tags in square brackets for log categorization: `[SISTEMA]`, `[Rede]`, `[AVISO]`, `[ERRO]`, `[CORE 0]`, `[OTA]`, `[WM]`, `[NVS]`, `[DHCP]`, `[mDNS]`, `[HA]`, `[SinricPro]`, `[Telegram]`, `[WEB]`, `[DHCP]`
- Boot/initialization logging in `setup()`:
```cpp
Serial.println("\n[SISTEMA] " PROJETO_NOME " v" PROJETO_VERSAO);
```
- State change logging via `logRelayAction()` helper:
```cpp
void logRelayAction(const char* source, int relayNum, bool state) {
    Serial.print('[');
    Serial.print(source);
    Serial.print("] ");
    // prints relay name + state
}
```
- Connection boundary markers:
```cpp
Serial.println("==============================");
Serial.println("MQTT CONECTADO!");
Serial.println("==============================");
```
- Dual output via `DualSerialClass` (Telnet) — all Serial output is mirrored to Telnet clients via the `#define Serial DualSerial` macro in `RemoteDebug.h`

## Function Design

**Size:** Highly variable:
- Short utility functions: 3–10 lines (e.g., `saveRelayState()`, `sinricProLoop()`)
- Medium handler functions: 20–50 lines (e.g., `reconnectMQTT()`, `handleTelegramMessages()`)
- Large orchestrator functions: 100+ lines (e.g., `TaskConexoes()` ~130 lines, `mqttCallback()` ~190 lines, `readSensors()` ~115 lines)

**Parameters:**
- Simple types (`int`, `bool`, `float`) passed by value
- Strings passed as `const String&` (Arduino String type by reference):
```cpp
void notifyTelegramStateChange(const String& source, int relayNum, bool relayState);
```
- C-strings passed as `const char*` for compatibility with PubSubClient:
```cpp
void logRelayAction(const char* source, int relayNum, bool state);
void mqttCallback(char* topic, byte* payload, unsigned int length);
```
- FreeRTOS task functions take `void *pvParameters`:
```cpp
void TaskConexoes(void *pvParameters);
void TaskSensores(void *pvParameters);
```

**Return Values:**
- Most functions return `void` (fire-and-forget / side-effect pattern)
- Boolean-returning functions: `WiFiManagerSetup()`, `bool onPowerState(...)` callback
- No error code returns; failures are logged to Serial

## Module Design

**Exports:**
- Public API in `.h` files: function prototypes, extern variables
- Implementation details and static globals kept in `.cpp` or `.h` (single-file modules)

**Barrel/Facade Files:**
- `Bibliotecas.h` acts as a centralized include manifest for all third-party libraries

**Module Structure Pattern:**
- Each functional area gets a single `.h` header file containing both declarations and inline implementations
- Only `OTAConfig` and `VariaveisGlobais` have separate `.cpp` files
- Most "modules" are header-only: `WebServerESP32.h`, `TelegramBotESP32.h`, `SinricProESP32.h`, `WiFiManagerConfig.h`, `HomeAssistantDiscovery.h`
- The main `.ino` file is the orchestrator: includes headers, defines globals, implements `setup()` and `loop()`, and contains the two FreeRTOS task functions

## Memory Management

- No dynamic allocation after setup (except `telegramBot = new UniversalTelegramBot(...)` in `initTelegram()`)
- `PROGMEM` used for large static data (dashboard HTML in `WebServerESP32.h`):
```cpp
const char DASHBOARD_HTML[] PROGMEM = R"rawl(...)rawl";
```
- Fixed-size `char[]` buffers for sensor data: `char str_temp_data[7]`
- NVS (Preferences) for persistent state storage

## File Structure Conventions

- All source files in project root (flat structure)
- Documentation in `docs/` subdirectory
- Graph/analysis output in `graphify-out/` subdirectory
- `.ino` file must match parent directory name (Arduino IDE requirement)

## Defensive Coding Patterns

- **Active-low relay convention:** `digitalWrite(pin, !RelayState)` — all relays are active-low, inverted
- **Inter-core synchronization:** Pending flags (`pendingMqttUpdate`, `pendingSinricProUpdate`) and FreeRTOS Queue (`telegramQueue`) for thread-safe IPC between Core 0 and Core 1
- **Guard flags:** `wmParamsAdded` prevents duplicate WiFiManager parameter registration; `otaInitialized` prevents OTA double-init; `mdnsStarted` prevents mDNS double-start; `sinricProOriginatedChange` prevents feedback loop between SinricPro and MQTT
- **Static IP caching:** DHCP-obtained IP is saved to NVS for reuse on reboot
- **Rate limiting:** Sensor reads capped at DHT_READ_INTERVAL (60s) and BMP_READ_INTERVAL (120s); SinricPro temperature events capped at 60s with change threshold of 0.5°C / 1% humidity

---

*Convention analysis: 2026-05-25*
