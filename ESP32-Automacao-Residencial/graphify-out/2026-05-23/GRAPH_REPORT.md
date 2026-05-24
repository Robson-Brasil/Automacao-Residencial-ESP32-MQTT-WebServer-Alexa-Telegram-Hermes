# Graph Report - .  (2026-05-23)

## Corpus Check
- Corpus is ~15,622 words - fits in a single context window. You may not need a graph.

## Summary
- 159 nodes · 184 edges · 37 communities (16 shown, 21 thin omitted)
- Extraction: 88% EXTRACTED · 12% INFERRED · 0% AMBIGUOUS · INFERRED: 22 edges (avg confidence: 0.78)
- Token cost: 0 input · 0 output

## Community Hubs (Navigation)
- [[_COMMUNITY_Core Infrastructure|Core Infrastructure]]
- [[_COMMUNITY_MQTT & Networking|MQTT & Networking]]
- [[_COMMUNITY_System Architecture & HA|System Architecture & HA]]
- [[_COMMUNITY_SinricPro & OTA|SinricPro & OTA]]
- [[_COMMUNITY_Telegram Bot & Debug|Telegram Bot & Debug]]
- [[_COMMUNITY_Library & Task Mgmt|Library & Task Mgmt]]
- [[_COMMUNITY_REST API Handlers|REST API Handlers]]
- [[_COMMUNITY_Telegram Notifications|Telegram Notifications]]
- [[_COMMUNITY_WiFi & MQTT Config|WiFi & MQTT Config]]
- [[_COMMUNITY_MQTT Config Persistence|MQTT Config Persistence]]
- [[_COMMUNITY_HA Discovery Topics|HA Discovery Topics]]
- [[_COMMUNITY_WiFi Manager Plans|WiFi Manager Plans]]
- [[_COMMUNITY_Project Docs|Project Docs]]
- [[_COMMUNITY_GitNexus|GitNexus]]
- [[_COMMUNITY_IoT Dashboard UI|IoT Dashboard UI]]
- [[_COMMUNITY_Project Analysis|Project Analysis]]
- [[_COMMUNITY_Cheatsheet|Cheatsheet]]
- [[_COMMUNITY_FreeRTOS Arch|FreeRTOS Arch]]
- [[_COMMUNITY_Credentials|Credentials]]
- [[_COMMUNITY_MQTT QoS Config|MQTT QoS Config]]
- [[_COMMUNITY_Task Prototypes|Task Prototypes]]
- [[_COMMUNITY_WiFi Setup|WiFi Setup]]
- [[_COMMUNITY_WiFi Check|WiFi Check]]
- [[_COMMUNITY_MQTT Reconnect|MQTT Reconnect]]
- [[_COMMUNITY_MQTT Callback|MQTT Callback]]
- [[_COMMUNITY_Sensor Read|Sensor Read]]
- [[_COMMUNITY_Publish Sensor|Publish Sensor]]
- [[_COMMUNITY_DHT Sensor|DHT Sensor]]
- [[_COMMUNITY_BMP180 Sensor|BMP180 Sensor]]

## God Nodes (most connected - your core abstractions)
1. `SinricProESP32.h (Alexa/Google Home)` - 13 edges
2. `WebServerESP32.h (Web Dashboard)` - 13 edges
3. `TelegramBotESP32.h (Telegram Bot)` - 12 edges
4. `WiFiManagerConfig.h (WiFi/MQTT Config Portal)` - 10 edges
5. `Documentation Index` - 10 edges
6. `Fix mDNS and OTA Hostname Plan` - 8 edges
7. `ESP32 Project Analysis Task Plan` - 7 edges
8. `VariaveisGlobais.h (Global State Header)` - 6 edges
9. `MQTT WiFiManager Integration Design` - 6 edges
10. `notifyTelegramStateChange (function)` - 5 edges

## Surprising Connections (you probably didn't know these)
- `WiFiManager Configuration` --implements--> `MQTT WiFiManager Implementation Plan`  [EXTRACTED]
  WiFiManagerConfig.h → docs/superpowers/plans/2026-05-14-mqtt-wifi-manager-implementation.md
- `RemoteDebug.h (Dual Serial/Telnet)` --references--> `VariaveisGlobais.h (Global State Header)`  [INFERRED]
  RemoteDebug.h → VariaveisGlobais.h
- `SinricProESP32.h (Alexa/Google Home)` --references--> `sinricProCredentials (APP_KEY, APP_SECRET, DEVICE_*)`  [EXTRACTED]
  SinricProESP32.h → LoginsSenhas.h
- `SinricProESP32.h (Alexa/Google Home)` --references--> `VariaveisGlobais.h (Global State Header)`  [EXTRACTED]
  SinricProESP32.h → VariaveisGlobais.h
- `TelegramBotESP32.h (Telegram Bot)` --calls--> `publishRelayStates (function)`  [EXTRACTED]
  TelegramBotESP32.h → Sensores.h

## Hyperedges (group relationships)
- **Voice Assistant Integration (Alexa/Google Home)** — SinricProESP32_h, onPowerState, setupSinricPro, sendSinricProTemperatureEvent, sendSinricProSwitchEvents, sinricProCredentials [EXTRACTED 1.00]
- **Telegram Bot Control Flow** — TelegramBotESP32_h, initTelegram, handleTelegramMessages, telegramLoop, getControlKeyboard, sendMainMenu, telegramCredentials [EXTRACTED 1.00]
- **Cross-Core Synchronization Mechanism** — CrossCoreSyncFlags, TelegramNotificationQueue, FreeRTOS_TaskHandles, VariaveisGlobais_h [INFERRED 0.90]
- **Web Dashboard Control Flow** — WebServerESP32_h, setupWebServer, handleWebServerClient, handleApiRelay, handleApiAll, handleApiSetName [EXTRACTED 1.00]
- **WiFi & MQTT Configuration Portal** — WiFiManagerConfig_h, WiFiManagerSetup, loadMQTTConfig, saveMQTTConfigToNVS, saveMQTTConfigCallback, setupMDNS, setupMQTT, mqttCredentials, NVS_MQTTConfig [EXTRACTED 1.00]
- **Documentation Portal (INDICE)** — docsINDICEmd_DocIndex, PROJETO_DETALHADO_Guide, CHEATSHEET_Reference [EXTRACTED 1.00]
- **WiFi Initialization Stack Fix** — docsPlanFixMdnsOta, MDNSService, OTAUpdateService, WiFiManagerLibrary, TaskConexoesCore0, DeviceHostnameConstant [INFERRED 0.80]
- **Configuration Hardcoding Elimination Initiative** — docsPlanFixMdnsOta, docsPlanRemoveHardcodedWifi, docsSpecMqttWifiManager, docsSpecRemoveHardcodedWifi, HardcodedConfigRemoval [INFERRED 0.85]

## Communities (37 total, 21 thin omitted)

### Community 0 - "Core Infrastructure"
Cohesion: 0.12
Nodes (24): ConstantesTempo.h (Time Intervals), Core 1 - Sensor Reading Stack, Cross-Core Sync Flags (pending*), GPIOs.h (Pin Definitions), NVS Relay Name Persistence (Preferences), Sensores.h (Sensor Read Prototypes), SinricProESP32.h (Alexa/Google Home), TaskSensores (FreeRTOS task) (+16 more)

### Community 1 - "MQTT & Networking"
Cohesion: 0.13
Nodes (16): ConfigMQTT.h (MQTT Setup/Reconnect), Core 0 - Network/Connection Stack, HomeAssistantDiscovery.h (HA MQTT Discovery), NVS MQTT Config Persistence (Preferences), TaskConexoes (FreeRTOS task), TopicosMQTT.h (MQTT Topic Strings), WiFiManagerConfig.h (WiFi/MQTT Config Portal), WiFiManagerSetup (function) (+8 more)

### Community 2 - "System Architecture & HA"
Cohesion: 0.20
Nodes (16): CHEATSHEET.md Quick Reference, DEVICE_HOSTNAME Constant, FreeRTOS Multi-Core Architecture, Global Hostname Centralization Design, Hermes Natural Language Commands, Home Assistant MQTT Discovery, mDNS (ESPmDNS), OTA Update (ArduinoOTA) (+8 more)

### Community 4 - "Telegram Bot & Debug"
Cohesion: 0.31
Nodes (6): getControlKeyboard(), handleTelegramMessages(), initTelegram(), processPendingTelegramNotification(), sendMainMenu(), telegramLoop()

### Community 5 - "Library & Task Mgmt"
Cohesion: 0.20
Nodes (10): Bibliotecas.h (Library Aggregator), DefinicoesProjeto.h (Project Metadata), DualSerialClass (Telnet debug stream), FreeRTOS Task Handles (hTaskConexoes/Sensores/Telegram), OTAConfig.cpp (OTA Implementation), OTAConfig.h (OTA Header), RemoteDebug.h (Dual Serial/Telnet), VariaveisGlobais.cpp (Global Definitions) (+2 more)

### Community 6 - "REST API Handlers"
Cohesion: 0.29
Nodes (6): getRelayName(), handleApiAll(), handleApiRelay(), handleApiRelays(), handleApiSetName(), setupWebServer()

### Community 7 - "Telegram Notifications"
Cohesion: 0.22
Nodes (9): TelegramBotESP32.h (Telegram Bot), TelegramNotification (struct + QueueHandle), applyRelayStatesToPins (function), getControlKeyboard (function), initTelegram (function), processPendingTelegramNotification (function), sendMainMenu (function), telegramCredentials (BOT_TOKEN, CHAT_ID) (+1 more)

### Community 8 - "WiFi & MQTT Config"
Cohesion: 0.42
Nodes (9): DHCP-Only Networking, Hardcoded Configuration Removal Initiative, MQTT Broker Configuration, NVS Flash Persistence, WiFiManager Library, WiFiManager Custom MQTT Parameters, Remove Hardcoded WiFi/IP Plan, MQTT WiFiManager Integration Design (+1 more)

## Knowledge Gaps
- **58 isolated node(s):** `WiFiManager Configuration`, `GitNexus Code Intelligence`, `IoT Dashboard Interface`, `Project Analysis Findings`, `Smart Home Hub ESP32 README` (+53 more)
  These have ≤1 connection - possible missing edges or undocumented components.
- **21 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **Why does `VariaveisGlobais.h (Global State Header)` connect `Library & Task Mgmt` to `Core Infrastructure`, `MQTT & Networking`, `Telegram Notifications`?**
  _High betweenness centrality (0.058) - this node is a cross-community bridge._
- **Why does `SinricProESP32.h (Alexa/Google Home)` connect `Core Infrastructure` to `MQTT & Networking`, `Library & Task Mgmt`?**
  _High betweenness centrality (0.041) - this node is a cross-community bridge._
- **Why does `WebServerESP32.h (Web Dashboard)` connect `Core Infrastructure` to `MQTT & Networking`, `Library & Task Mgmt`?**
  _High betweenness centrality (0.040) - this node is a cross-community bridge._
- **What connects `WiFiManager Configuration`, `GitNexus Code Intelligence`, `IoT Dashboard Interface` to the rest of the system?**
  _59 weakly-connected nodes found - possible documentation gaps or missing edges._
- **Should `Core Infrastructure` be split into smaller, more focused modules?**
  _Cohesion score 0.12318840579710146 - nodes in this community are weakly interconnected._
- **Should `MQTT & Networking` be split into smaller, more focused modules?**
  _Cohesion score 0.13333333333333333 - nodes in this community are weakly interconnected._