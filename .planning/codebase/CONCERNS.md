# Codebase Concerns

**Analysis Date:** 2026-05-25

## Tech Debt

### Inter-Core Race Conditions (FreeRTOS Shared Mutable State)

**Issue:** Multiple boolean flags (`pendingMqttUpdate`, `pendingSinricProUpdate`, `pendingSensorMqttUpdate`, `pendingSensorSinricUpdate`, `pendingHADiscovery`) are read and written from both Core 0 (TaskConexoes) and Core 1 (TaskSensores/WebServer/loop) without any mutex, semaphore, or atomic operations. Same for relay state globals (`RelayState1`-`RelayState8`, `Todos`).

**Files:**
- `ESP32-Automacao-Residencial.ino` lines 834-870 (Core 0 consumes flags)
- `ESP32-Automacao-Residencial.ino` line 910 (Core 1 sets flag)
- `WebServerESP32.h` lines 86-87 (Core 1 sets flags)
- `SinricProESP32.h` lines 63, 77 (Core 0 sets flag)
- `TelegramBotESP32.h` line 182 (Core 1 sets flag)
- `VariaveisGlobais.cpp` lines 37-41 (definitions)
- `VariaveisGlobais.h` lines 59-63 (extern declarations)

**Impact:** Race conditions can cause missed state updates, duplicate MQTT publishes, or stale sensor data being published. In worst case, relay state feedback to SinricPro/Alexa could be incorrect. Under sustained load or if `mqttClient.loop()` blocks, the flags may be overwritten before being consumed.

**Fix approach:** Wrap all shared flag writes in `portMUX_TYPE` critical sections or use atomic intrinsics (`AtomicHandle`). Better yet, consolidate to a single inter-core command queue using `xQueueSend` (similar to the Telegram notification pattern) instead of raw boolean flags.

### Two Cores Writing to Same GPIO Pins Simultaneously

**Issue:** GPIO pins for relays are written to from both cores:
- Core 0 (TaskConexoes): MQTT callback writes to relay pins (`ESP32-Automacao-Residencial.ino` lines 285-289, 303-370)
- Core 1 (loop/WebServer): `handleApiRelay()` and `handleApiAll()` in `WebServerESP32.h` lines 71-123 also write to same pins
- Core 1 (Telegram): `handleTelegramMessages()` in `TelegramBotESP32.h` lines 140-147 writes to same pins
- Core 0 (SinricPro): `onPowerState()` in `SinricProESP32.h` lines 53-93 writes to same pins

**Impact:** Potential race conditions where a relay is toggled simultaneously from two sources, potentially leaving the physical output in an indeterminate state.

**Fix approach:** Route ALL hardware writes through a single service (Core 0 or a dedicated control queue). Core 1 should set pending flags and let Core 0 execute the physical writes.

### `delay(3000)` Blocking Core 0 in TaskConexoes

**Issue:** `ESP32-Automacao-Residencial.ino` lines 783-795 use a blocking `delay(3000)` inside the TaskConexoes loop when checking the config button. This completely halts Core 0 for 3 seconds — no MQTT processing, no OTA handling, no Telegram polling, no SinricPro.

```cpp
if (digitalRead(configButton) == LOW) {
  delay(3000);
  if (digitalRead(configButton) == LOW) {
```

**Impact:** All network services on Core 0 freeze for 3 seconds whenever the BOOT button is pressed. If the user holds the button, even longer.

**Fix approach:** Use non-blocking debounce with `millis()` timing:

```cpp
static unsigned long btnPressStart = 0;
static bool btnPressed = false;
if (digitalRead(configButton) == LOW && !btnPressed) {
  btnPressStart = millis();
  btnPressed = true;
} else if (digitalRead(configButton) == HIGH) {
  btnPressed = false;
} else if (btnPressed && (millis() - btnPressStart >= 3000)) {
  // execute portal logic
}
```

### NVS Preferences Leaks

**Issue:** The global `Preferences preferences` instance has `begin()` called once in `setup()` (line 716), but `end()` is NEVER called after individual `saveRelayState()` or `loadRelayState()` operations (lines 620-628). Additionally, `WebServerESP32.h`'s `getRelayName()` (lines 30-42) creates a SEPARATE local `Preferences p` instance and opens the same NVS namespace ("relay states") concurrently with the global instance — and opens it in readWrite mode (`false`) despite only reading.

**Files:**
- `ESP32-Automacao-Residencial.ino` lines 620-628
- `WebServerESP32.h` lines 30-42

**Impact:** Concurrent access to the same NVS namespace from two `Preferences` instances can cause NVS corruption or undefined behavior during simultaneous reads/writes.

**Fix approach:** Remove the local `Preferences` instance in `getRelayName()`. Pass the global `preferences` instance as parameter or read NVS values once at boot into a cached array.

### MQTT Credentials Logged to Serial

**Issue:** `WiFiManagerConfig.h` lines 44-47 log MQTT username and broker IP to Serial output:

```cpp
Serial.print("[NVS] Broker: "); Serial.println(mqtt_broker);
Serial.print("[NVS] User:   "); Serial.println(mqtt_user);
Serial.print("[NVS] Client: "); Serial.println(mqtt_client);
```

**Impact:** MQTT credentials are visible in serial monitor output and potentially in the Telnet mirror (via `DualSerialClass`). Anyone with physical USB access or Telnet access can see them.

**Fix approach:** Redact credentials in logs (show only first/last character, e.g. `ad***in`).

### Duplicate GPIO Pin Definitions

**Issue:** Relay pins are defined in BOTH `GPIOs.h` (lines 5-12) AND at the top of `ESP32-Automacao-Residencial.ino` (lines 40-47). The values are identical, but this is a maintenance risk — if someone changes pins in only one location, behavior breaks silently.

**Files:**
- `ESP32-Automacao-Residencial.ino` lines 40-47
- `GPIOs.h` lines 5-12

**Fix approach:** Remove the duplicate definitions from the `.ino` file. Let `GPIOs.h` be the single source of truth. The `GPIOs.h` is already included at line 59.

### `#define Serial DualSerial` Macro Substitution

**Issue:** `RemoteDebug.h` line 62 replaces all `Serial` identifiers with `DualSerial` via macro:

```cpp
#define Serial DualSerial
```

**Impact:** This macro replacement applies to ALL code including library headers included AFTER `RemoteDebug.h`. If any third-party library uses `Serial` internally for debug output, it gets redirected to the Telnet wrapper. This also breaks any C++ code that uses `Serial` in string concatenation or as a symbol name. It's a fragile pattern that can cause hard-to-diagnose compilation errors or runtime misbehavior.

**Fix approach:** Instead of a global `#define`, create a thin wrapper class and explicitly use `DualSerial` throughout the codebase, or use the platform's built-in Telnet/RemoteDebug support.

### Telegram Bot State Update Runs on Core 1 But Sets Core 0 Flags

**Issue:** `handleTelegramMessages()` in `TelegramBotESP32.h` (lines 123-186) runs on Core 1 (called from `telegramLoop()` which is called from `TaskConexoes` on Core 0 via line 856). Wait — actually, `telegramLoop()` IS called on Core 0 via `TaskConexoes`. So the Telegram handler runs on Core 0. But `handleWebServerClient()` runs on Core 1. Both cores write to the same relay pins and set the same `pendingSinricProUpdate` flag.

**Impact:** See race condition concerns above.

### `getRelayName()` Opens NVS Every Call

**Issue:** `WebServerESP32.h` function `getRelayName()` opens and closes the NVS Preferences namespace on every call. This function is called 8 times PER API request to `/api/relays` (line 58-60). This generates unnecessary NVS flash wear.

**Impact:** Accelerates NVS flash wear on the ESP32. Not critical for typical use but poor design.

**Fix approach:** Cache relay names in an array at boot time.

## Security Considerations

### Hardcoded API Credentials in Source Code

**Risk:** All third-party service credentials are stored as plaintext string literals in `ESP32-Automacao-Residencial.ino` lines 126-138:

- Telegram Bot Token: `"8444037202:AAGB4P8wjPtjiGzlBHInCYkLUqTa1OUzgHI"`
- Telegram Chat ID: `"5270818980"`
- SinricPro App Key: `"4914a0d0-327e-4815-8128-822b7a80713d"`
- SinricPro App Secret: `"b73f409a-fcc5-4c60-8cf4-d86d9b4e2bae-e7fc2fe6-1b4a-4b71-aa38-de61ad019107"`
- SinricPro Device IDs (3 values)
- OTA Password: `"S3nh@S3gur@"` in `OTAConfig.cpp` line 16

**Impact:** These credentials are committed to git and exposed to anyone with repository access. Anyone who obtains the BOT_TOKEN can control the Telegram bot. Anyone with SinricPro credentials can impersonate the device. Anyone with the OTA password can flash malicious firmware. The Telegram bot token alone allows sending messages as the bot and reading all messages sent to it.

**Files:**
- `ESP32-Automacao-Residencial.ino` lines 126-138
- `OTAConfig.cpp` line 16

**Current mitigation:** None. The `LoginsSenhas.h` file references these as `extern` (lines 21-29), suggesting awareness that they should be separated, but the actual values remain hardcoded in the `.ino` file.

**Recommendations:**
1. Move ALL credentials to a separate `secrets.h` file that is listed in `.gitignore` and never committed.
2. Use platform environment variables or a build-time configuration system (PlatformIO build flags, sdkconfig).
3. Alternatively, store credentials in NVS and load at boot (the MQTT config already uses this pattern — extend it to all credentials).
4. Rotate all currently exposed credentials immediately.

### Dashboard/API Has No Authentication

**Risk:** The web server on port 8181 (`WebServerESP32.h`) exposes complete relay control and sensor data with zero authentication. Endpoints:
- `GET /api/relays` — reads all relay states and names
- `GET /api/sensors` — reads all sensor data
- `POST /api/relay/{idx}/{state}` — controls any relay
- `POST /api/all/{state}` — controls all relays
- `POST /api/setname/{idx}` — renames relays

**Impact:** Anyone on the local network (including guests, compromised devices, or via Wi-Fi if WPA2 is cracked) can:
- Turn all lights/appliances on/off
- Monitor environmental data
- Rename relays (vandalism)

**Current mitigation:** Only accessible on the local network (no port forwarding assumed). Telnet on port 2323 also has no auth.

**Recommendations:**
1. Add HTTP Basic Authentication for all API endpoints.
2. Consider a session token approach for the web dashboard.
3. The Telnet server (`WiFiServer telnetServer(2323)`) should also require authentication.

### Telnet Server Exposed Without Authentication

**Risk:** `ESP32-Automacao-Residencial.ino` line 101 starts a Telnet server on port 2323 with no authentication. Anyone on the local network can connect and see all serial output (including MQTT credentials from logs).

**Files:**
- `ESP32-Automacao-Residencial.ino` lines 101-102
- `RemoteDebug.h` (DualSerialClass mirrors serial output to Telnet)

**Impact:** Full serial console access to anyone on the LAN, including credential exposure and system state monitoring.

### OTA Password Hardcoded

**Risk:** `OTAConfig.cpp` line 16: `ArduinoOTA.setPassword("S3nh@S3gur@");`

**Impact:** The same password is used on every compile and committed to git. Anyone who can reach the device on the network can flash arbitrary firmware.

### Hardcoded Reference Altitude and Sea-Level Pressure

**Risk:** `ESP32-Automacao-Residencial.ino` lines 119-121 hardcode `pressaoNivelMar = 1012.0` and `altitudeNivelMar = 92.0`. These values affect all BMP180 calculations (altitude, sea-level pressure). If the physical location changes (device moved to a different city), these values are wrong.

**Impact:** Inaccurate pressure and altitude readings. The sea-level pressure value is not auto-calibrated, so weather pressure readings are offset by the error in the reference value.

## Performance Bottlenecks

### Blocking 3-Second Delay in Core 0 Task

**Problem:** `delay(3000)` in `TaskConexoes` (line 784) halts ALL network processing on Core 0 for 3 seconds whenever the button is pressed.

**Files:**
- `ESP32-Automacao-Residencial.ino` lines 783-795

**Cause:** Blocking `delay()` in a real-time task loop.

**Improvement path:** Use non-blocking debounce with `millis()` timing (see fix in Tech Debt section).

### Redundant Sensor Read on Hermes SENSORES Command

**Problem:** `ESP32-Automacao-Residencial.ino` lines 379-384 call `readSensors()` synchronously on Core 0 inside the MQTT callback, which blocks MQTT processing. The function runs DHT22 reads (requires ~2 seconds) and BMP180 reads.

**Files:**
- `ESP32-Automacao-Residencial.ino` lines 379-384

**Cause:** Direct synchronous sensor read from MQTT callback context.

**Improvement path:** Just set `lastMsgDHT = 0` and `lastMsgBMP180 = 0` and let the Core 1 TaskSensores pick up the read naturally on its next cycle.

### 8 Individual MQTT Subscriptions for Relays

**Problem:** `ESP32-Automacao-Residencial.ino` lines 244-253 subscribe to 10 separate MQTT topics. Each `mqttClient.subscribe()` call generates a SUBSCRIBE packet. This is fine for a few topics, but the pattern could be consolidated with MQTT wildcards.

**Files:**
- `ESP32-Automacao-Residencial.ino` lines 244-253
- `TopicosMQTT.h` lines 7-17

**Improvement path:** Use a single wildcard subscription like `ESP32/+/Comando` and parse the topic in the callback. This reduces bandwidth and MQTT control packets.

## Fragile Areas

### MQTT Callback Chain (0.5KB function with massive duplicated code)

**Files:** `ESP32-Automacao-Residencial.ino` lines 265-453

**Why fragile:** The `mqttCallback` function is 188 lines with near-identical code blocks for each of 10 topics. Each relay handler is a copy-paste block with only the variable names changed. The Hermes command parser (lines 372-453) is another fragile if-else chain. Adding a new relay requires duplicating ~10 lines in the callback plus MQTT subscribes in `reconnectMQTT()` plus the `publishRelayStates()` function plus the `publishSensorData()` function.

**Safe modification:** 
- Refactor relays into an array of structs (pin, state, sub topic, pub topic, name index).
- Replace the MQTT command handler chain with a data-driven loop over the array.
- The Hermes command parser should use a lookup table, not sequential string comparisons.

**Test coverage:** No automated tests exist (Arduino embedded project).

### Telegram State Change Notification Queue

**Files:**
- `TelegramBotESP32.h` lines 35-43
- `ESP32-Automacao-Residencial.ino` lines 849-853

**Why fragile:** The `xQueueSend` call in `notifyTelegramStateChange()` uses 0 timeout (`xQueueSend(telegramQueue, &notif, 0)`). If the queue is full (max 10 items), the notification is silently dropped. Multiple rapid state changes (e.g., "LIGAR TUDO") can overflow the queue.

**Safe modification:** Use a small timeout (`pdMS_TO_TICKS(10)`) or increase queue depth. Monitor queue utilization via `uxQueueMessagesWaiting()` and log warnings when near capacity.

### Dual Core Timer Variable Synchronization

**Files:**
- `ESP32-Automacao-Residencial.ino` lines 800-827 (Core 0 timers)
- `ESP32-Automacao-Residencial.ino` lines 905-911 (Core 1 timers)

**Why fragile:** Timer variables like `lastWifiCheckTime`, `lastMqttReconnectAttempt`, `lastRelayUpdate` are declared as plain `unsigned long` and accessed from a single core each, so this is currently safe. However, the `pendingMqttUpdate` flag is set by Core 1 (line 910) and consumed by Core 0 (line 834) with no memory barrier. On multi-core ESP32, compiler optimizations or cache coherency issues could cause Core 0 to never see the flag change.

**Safe modification:** Declare inter-core flags as `volatile bool` to prevent compiler optimizations from caching the value in a register. Currently none of the `pending*` flags are `volatile`.

### OTA Task Suspension

**Files:**
- `OTAConfig.cpp` lines 21-23, 35-37, 53-56
- `ESP32-Automacao-Residencial.ino` (TaskSensores runs on Core 1)

**Why fragile:** During OTA update, TaskSensores on Core 1 is suspended via `vTaskSuspend()`. If the OTA fails, the error handler calls `vTaskResume(hTaskSensores)` (lines 53-56). However, if `hTaskSensores` is NULL at the time of error (very early in boot), `vTaskResume(NULL)` behavior is undefined. Also, if the OTA succeeds and resets, the `vTaskResume` in `.onEnd()` (line 33-37) is unnecessary since the device resets immediately after.

### BMP180 No-Reconnect Strategy

**Files:**
- `ESP32-Automacao-Residencial.ino` lines 735-737

**Why fragile:** `bmp.begin()` is called once in `setup()`. If the BMP180 is disconnected from I2C or fails, `bmp.begin()` returns false with a serial message but is never retried. Sensors like DHT22 and BMP180 can have intermittent I2C/1-wire bus failures, and the device needs a full reboot to recover.

**Safe modification:** Implement periodic `bmp.begin()` retry in `readSensors()` when pressure reads are 0.

## Missing Critical Features

### No Over-The-Air Configuration Persistence for Credentials

**Files:**
- `LoginsSenhas.h` (header declares extern references for credentials)
- `ESP32-Automacao-Residencial.ino` lines 126-138 (hardcoded values)

**Problem:** Unlike MQTT credentials (which are configurable through WiFiManager and persisted in NVS), Telegram bot token/chat ID, SinricPro credentials, and OTA password are hardcoded in source. Changing them requires recompilation and re-flashing.

**Blocks:** Remote credential rotation. If a token is compromised (e.g., Telegram bot token leaked in git), the device must be physically reflashed.

### No Watchdog Timer

**Files:** `ESP32-Automacao-Residencial.ino` (setup function, lines 689-759)

**Problem:** Neither the Task Watchdog Timer (TWDT) nor the Interrupt Watchdog Timer (IWDT) is configured. If either FreeRTOS task hangs (e.g., MQTT callback blocks, sensor read stalls on I2C), the device will hang indefinitely with no automatic recovery.

**Risk:** A single sensor I2C lockup or MQTT library hang causes permanent loss of control until a manual power cycle.

## Test Coverage Gaps

**What's not tested:** The entire firmware has zero test infrastructure. There are no unit tests, integration tests, or hardware-in-the-loop tests. Every file is implementation-only with no test counterparts.

**Files:** All source files.

**Risk:** Any change to the MQTT relay handler, sensor reading logic, or inter-core communication can break functionality without detection. The complex chains in `mqttCallback`, `handleTelegramMessages`, and `readSensors()` are particularly risky.

**Priority:** Medium

## Dependencies at Risk

**Package:** `Adafruit BMP085` library (used via `Adafruit_BMP085.h` in `Bibliotecas.h` line 24)

**Risk:** The BMP085 library is used for the BMP180 sensor. While the BMP085 and BMP180 are register-compatible, the Adafruit_BMP085 library is older and has known limitations (no calibration data validation). The BMP180 is a discontinued sensor.

**Impact:** If the BMP180 sensor is replaced with a newer model (BMP280, BME280, BME680), the library and I2C communication will need rewriting.

**Package:** `SinricPro` SDK

**Risk:** The SinricPro SDK depends on WebSocket connections to their cloud servers. If SinricPro changes their API or discontinues the service, Alexa/Google Home integration breaks with no migration path. The device IDs and credentials are hardcoded.

---

*Concerns audit: 2026-05-25*
