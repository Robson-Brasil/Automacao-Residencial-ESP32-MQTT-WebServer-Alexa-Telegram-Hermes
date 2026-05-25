# Testing Patterns

**Analysis Date:** 2026-05-25

## Test Framework

**Status:** No test framework detected.

The project is an ESP32/Arduino embedded firmware written in C++ (Arduino framework). There are no unit tests, integration tests, or end-to-end tests in the repository. No test runners, assertion libraries, or mocking frameworks are configured.

**No test files found:**
- No files matching `*.test.*` or `*.spec.*`
- No jest, vitest, catch2, unity test, or ArduinoUnit configurations
- No test runner configuration files (no `jest.config.*`, `platformio.ini` with test configs)

**Run Commands:** Not applicable — no testing infrastructure exists.

## Test File Organization

**Location:** Not applicable — no test directory exists.

**Naming:** Not applicable.

**Structure:** Not applicable.

## Test Strategy (Current State)

The project relies entirely on **manual testing** and **serial debug logging** for verification:

### Manual Testing Channels

1. **Serial Monitor (USB UART):**
   - Primary debug output via `Serial.println()` at 115200 baud
   - All state changes, sensor readings, and connection events logged to serial
   - Used during development to verify behavior

2. **Web Dashboard (Port 8181):**
   - REST API endpoints (`/api/relays`, `/api/sensors`, `/api/relay/{idx}/{state}`, `/api/all/{state}`)
   - Visual glassmorphism UI for relay control and sensor monitoring
   - Manual verification of relay toggling and sensor data accuracy

3. **MQTT Client:**
   - Subscribe to `ESP32/#` topics to observe all published states
   - Publish to `ESP32/InterruptorN/Comando` topics to trigger relays
   - Used to verify MQTT communication path

4. **Telegram Bot:**
   - Interactive keyboard for relay control and sensor queries
   - Push notifications for state changes
   - Manual verification of command routing and state propagation

5. **SinricPro (Alexa/Google Home):**
   - Voice command testing via Amazon Alexa or Google Home apps
   - Verifies SinricPro callback integration and state sync

### Verification Approach

- **State propagation testing:** Change relay state via one channel (e.g., Telegram) and verify all other channels reflect the change (Dashboard, MQTT, SinricPro)
- **Persistence testing:** Reboot the ESP32 and verify relay states are restored from NVS
- **Reconnection testing:** Disconnect WiFi/MQTT broker and verify auto-reconnect logic
- **Sensor accuracy:** Compare DHT22/BMP180 readings against known references

## Mocking

**Framework:** Not used — no mocking infrastructure exists.

**What to Mock:** Not applicable for current state.

## Fixtures and Factories

**Test Data:** Not applicable — no test fixtures exist.

Data sources used in development:
- **NVS/Preferences:** Stores relay states, MQTT config, relay names, static IP config — can be manually cleared via `preferences.clear()`
- **Sensor defaults:** Char arrays initialized to `"--"` placeholder values until first sensor reading

## Coverage

**Requirements:** None enforced. No coverage tooling exists.

**PlatformIO coverage tools** (gcov, lcov) would be applicable if PlatformIO test framework were adopted, but no `platformio.ini` is present.

## Test Types

**Unit Tests:** Not present. The codebase is not structured for unit testing — most modules are header-only with inline implementations, global mutable state (`extern bool` variables throughout), and hardware-dependent code tightly coupled with business logic.

**Integration Tests:** Not present. Integration testing would require actual ESP32 hardware or an emulator (e.g., Wokwi, QEMU for ESP32).

**E2E Tests:** Not present. The project has no automated E2E testing pipeline.

## Barriers to Testing

1. **Global mutable state:** All relay states (`RelayState1`–`RelayState8`, `Todos`) and flags (`pendingMqttUpdate`, `wifiConnected`, etc.) are global `extern` variables — hard to isolate for unit tests
2. **Hardware coupling:** `digitalWrite()`, `dht.readTemperature()`, `bmp.readPressure()`, `WiFi.status()` are called directly without abstraction layers
3. **Header-only modules:** Most logic lives in `.h` files with inline function definitions — no separation between interface and implementation
4. **Arduino framework dependency:** Code depends on Arduino.h, WiFi.h, PubSubClient, etc. — requires Arduino core or mock layer to compile in test environment
5. **Long tasks with side effects:** Core functions like `mqttCallback()` (~190 lines) and `TaskConexoes()` (~130 lines) handle multiple concerns (MQTT parsing, relay control, persistence, notifications, SinricPro sync), making focused testing difficult

## Recommended Testing Approach

For future testing adoption, the following patterns are recommended:

**Framework choice (if adding tests):**
- **PlatformIO Unit Testing** with `Unity` test framework (de facto standard for ESP32/Arduino)
- Or **Catch2** with mock Arduino layer

**Architecture improvements to enable testing:**
- Extract hardware abstractions (sensors, relays, WiFi) behind interface classes that can be mocked
- Move business logic out of header files into `.cpp` implementation files
- Replace global mutable state with dependency-injected state objects

**Test priorities:**
1. **MQTT callback parsing** — the `mqttCallback()` function has complex conditional logic for Hermes commands (LIGAR/DESLIGAR/SENSORES/STATUS)
2. **Relay state management** — `loadAllRelayStates()`, `saveRelayState()`, `applyRelayStatesToPins()`
3. **IPC synchronization** — the pending-flag pattern between Core 0 and Core 1
4. **SinricPro synchronization** — `sendSinricProSwitchEvents()` flag gating to avoid feedback loops

**Current debugging tools that function as test infrastructure:**
- `logRelayAction()` — traces all relay state changes with source attribution
- `updateStatusLED()` — visual indicator of WiFi/MQTT connectivity status
- Telnet mirror (`DualSerialClass`) — remote serial monitoring

---

*Testing analysis: 2026-05-25*
