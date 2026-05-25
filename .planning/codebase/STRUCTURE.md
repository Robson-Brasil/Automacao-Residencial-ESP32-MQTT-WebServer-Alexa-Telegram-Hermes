# Codebase Structure

**Analysis Date:** 2026-05-25

## Directory Layout

```
ESP32-Automacao-Residencial/        # Root — firmware source
├── ESP32-Automacao-Residencial.ino # Main Arduino sketch (810 lines)
├── Bibliotecas.h                   # All #include directives (dependency hub)
├── ConfigMQTT.h                    # MQTT callback/setup/reconnect prototypes
├── ConstantesTempo.h               # Timing interval extern declarations
├── DefinicoesProjeto.h             # Project metadata (name, version, hostname)
├── GPIOs.h                         # GPIO pin mapping constants
├── HomeAssistantDiscovery.h        # HA MQTT Discovery payload builder
├── LoginsSenhas.h                  # MQTT credential buffers, SinricPro/Telegram externs
├── MQTTParametros.h                # QoS, retain, keepalive constants
├── OTAConfig.h                     # OTA setup prototype
├── OTAConfig.cpp                   # OTA configuration + callbacks (64 lines)
├── RemoteDebug.h                   # DualSerialClass (Telnet Serial mirror)
├── Sensores.h                      # Sensor read function prototypes
├── SinricProESP32.h                # Alexa/Google Home integration (196 lines)
├── TaskCores.h                     # FreeRTOS task function prototypes
├── TelegramBotESP32.h              # Telegram bot polling + keyboard (209 lines)
├── TopicosMQTT.h                   # All MQTT topic string constants
├── VariaveisGlobais.h              # extern declarations for all shared state
├── VariaveisGlobais.cpp            # Global variable definitions (49 lines)
├── WebServerESP32.h                # Dashboard + REST API (204 lines)
├── WiFiManagerConfig.h             # WiFiManager portal, NVS MQTT config, mDNS
├── WiFiUtils.h                     # WiFi setup/check prototypes
├── dashboard.html                  # Standalone web UI (embedded copy)
│
├── docs/                           # Project documentation
│   ├── CHEATSHEET.md               # Quick configuration guide
│   ├── INDICE.md                   # Documentation index
│   ├── PROJETO_DETALHADO.md        # Detailed project guide
│   └── superpowers/                # OpenSpec design docs (specs + plans)
│       ├── specs/
│       │   ├── 2026-05-14-mqtt-wifi-manager-design.md
│       │   └── 2026-05-14-remove-hardcoded-wifi-ip-design.md
│       └── plans/
│           ├── 2026-05-14-mqtt-wifi-manager-implementation.md
│           └── 2026-05-14-remove-hardcoded-wifi-ip.md
│
├── .opencode/                      # OpenCode agent configuration
├── graphify-out/                   # Code knowledge graph output
│   ├── GRAPH_REPORT.md
│   ├── graph.json
│   ├── cost.json
│   ├── manifest.json
│   └── cache/                      # AST + semantic analysis caches
│
├── .gitignore
├── CLAUDE.md                       # GitNexus code intelligence config
├── LICENSE
├── README.md
└── desktop.ini
```

## Directory Purposes

**Root (`ESP32-Automacao-Residencial/`):**
- Purpose: All firmware source — flat layout, no subdirectories for implementation
- Contains: 1 `.ino`, 2 `.cpp`, 19 `.h`, 1 `.html` files
- Key files: `ESP32-Automacao-Residencial.ino` (entry point + core logic)

**`docs/`:**
- Purpose: End-user and developer documentation (Portuguese)
- Contains: Markdown guides, OpenSpec design specs and implementation plans
- Not firmware source — documentation only

**`graphify-out/`:**
- Purpose: GitNexus code knowledge graph artifacts (AST, semantic caches, JSON graph)
- Generated: Yes (by GitNexus analysis tools)
- Committed: Yes

**`.opencode/`:**
- Purpose: OpenCode agent skills and configuration
- Generated: No (manually configured)
- Committed: Yes (per .gitignore — .opencode/ is NOT in the ignore list for this project)

## Key File Locations

**Entry Points:**
- `ESP32-Automacao-Residencial/ESP32-Automacao-Residencial.ino`: Arduino sketch entry — `setup()` and `loop()`

**Configuration:**
- `ESP32-Automacao-Residencial/DefinicoesProjeto.h`: Project metadata
- `ESP32-Automacao-Residencial/GPIOs.h`: Pin mapping
- `ESP32-Automacao-Residencial/TopicosMQTT.h`: MQTT topic strings
- `ESP32-Automacao-Residencial/MQTTParametros.h`: QoS/retain/keepalive
- `ESP32-Automacao-Residencial/ConstantesTempo.h`: Timing intervals
- `ESP32-Automacao-Residencial/LoginsSenhas.h`: Credential buffers
- `ESP32-Automacao-Residencial/HomeAssistantDiscovery.h`: HA device identity

**Core Logic:**
- `ESP32-Automacao-Residencial/ESP32-Automacao-Residencial.ino`: All main logic (810 lines)
- `ESP32-Automacao-Residencial/VariaveisGlobais.h` + `VariaveisGlobais.cpp`: Global state

**Integrations (each in its own header):**
- `ESP32-Automacao-Residencial/TelegramBotESP32.h`: Telegram bot (209 lines)
- `ESP32-Automacao-Residencial/SinricProESP32.h`: Alexa/Google Home (196 lines)
- `ESP32-Automacao-Residencial/WebServerESP32.h`: Dashboard + API (204 lines)
- `ESP32-Automacao-Residencial/HomeAssistantDiscovery.h`: HA Discovery (136 lines)
- `ESP32-Automacao-Residencial/WiFiManagerConfig.h`: WiFi + MQTT config portal (209 lines)
- `ESP32-Automacao-Residencial/OTAConfig.h` + `OTAConfig.cpp`: OTA updates (70 lines total)
- `ESP32-Automacao-Residencial/RemoteDebug.h`: Telnet serial mirror (64 lines)

## Naming Conventions

**Files:**
- PascalCase for all `.h` and `.cpp` files: `ConfigMQTT.h`, `HomeAssistantDiscovery.h`, `VariaveisGlobais.cpp`
- Project naming with hyphens for `.ino`: `ESP32-Automacao-Residencial.ino`
- Lowercase for `.html`: `dashboard.html`
- UPPERCASE for `README.md`, `CLAUDE.md`, `LICENSE`

**Functions:**
- camelCase: `readSensors()`, `setupMQTT()`, `handleApiRelay()`, `publishRelayStates()`
- PascalCase for FreeRTOS task functions: `TaskConexoes()`, `TaskSensores()`
- Lowercase for some internal callbacks: `mqttCallback()`, `onPowerState()`

**Variables:**
- PascalCase for global boolean relay states: `RelayState1`, `RelayState8`, `Todos`
- camelCase for most other variables: `wifiConnected`, `mqttConnected`, `temperature`, `pressure`
- PascalCase for global string buffers: `mqtt_broker`, `mqtt_port`, `mqtt_user`, `mqtt_pass`
- Upper-snake-case for constants/defines: `MQTT_KEEPALIVE`, `DHT_READ_INTERVAL`, `SINRICPRO_APP_KEY`
- PascalCase for MQTT topic pointers: `sub0`..`sub9`, `pub0`..`pub18`

**Types:**
- PascalCase for structs and enums: `TelegramNotification`, `RelayStates` enum
- PascalCase for classes: `DualSerialClass`

**Directories:**
- All lowercase: `docs/`, `graphify-out/`, `.opencode/`

## Where to Add New Code

**New Feature (e.g., new integration):**
- Create a new header file at root: `NewIntegration.h`
- Add its includes to `Bibliotecas.h`
- Add any needed global state to `VariaveisGlobais.h` + `VariaveisGlobais.cpp`
- Integrate its main loop into `TaskConexoes()` (Core 0) or `TaskSensores()` (Core 1) depending on network vs I/O
- Add any needed MQTT topics to `TopicosMQTT.h`

**New Sensor:**
- Add sensor library include to `Bibliotecas.h`
- Create `NovoSensor.h` with read/publish functions
- Wire into `readSensors()` in the `.ino` file or `TaskSensores()`
- Add MQTT topics to `TopicosMQTT.h`
- Add HA Discovery payload in `HomeAssistantDiscovery.h`
- Add sensor data to `/api/sensors` endpoint in `WebServerESP32.h`

**New Web API Endpoint:**
- Add handler function in `WebServerESP32.h`
- Register route in `setupWebServer()` using `webServer.on()` or in the `onNotFound` fallback

**New MQTT Command:**
- Add subscribe/publish topic to `TopicosMQTT.h`
- Add handler branch in `mqttCallback()` in the `.ino` file

**Configuration Constants:**
- Add to the appropriate header: `GPIOs.h` (pins), `MQTTParametros.h` (QoS), `ConstantesTempo.h` (timing), `DefinicoesProjeto.h` (project metadata)

**Testing/Hardware Validation:**
- No test framework exists; add validation sketches to a `tests/` directory if created

## Special Directories

**`graphify-out/`:**
- Purpose: Code analysis graph data (GitNexus)
- Generated: Yes (by `npx gitnexus analyze`)
- Committed: Yes
- Contents: `graph.json`, `manifest.json`, `cost.json`, `GRAPH_REPORT.md`, `cache/` with AST and semantic analysis files

**`.opencode/`:**
- Purpose: OpenCode agent skills, configuration, and workspace state
- Generated: No
- Committed: Yes

**`docs/superpowers/`:**
- Purpose: OpenSpec design documents — specs (problem definition + solution design) and plans (implementation steps)
- Generated: Yes (by OpenSpec agent during explore/propose sessions)
- Committed: Yes

## File Organization Rules

1. **One module per `.h` file** — each integration or subsystem has its own header
2. **All `.h` files at root** — no source subdirectories
3. **Functional grouping** — headers are named by function (e.g., `TelegramBotESP32.h`, not `communication.h`)
4. **Global state in two files** — declaration in `VariaveisGlobais.h`, definition in `VariaveisGlobais.cpp`
5. **All dependencies in `Bibliotecas.h`** — single include point for external libraries
6. **Topic strings in one file** — all MQTT topics centralized in `TopicosMQTT.h`
7. **Configuration in separate files** — pin config, timing, MQTT params, project metadata each have their own header

---

*Structure analysis: 2026-05-25*
