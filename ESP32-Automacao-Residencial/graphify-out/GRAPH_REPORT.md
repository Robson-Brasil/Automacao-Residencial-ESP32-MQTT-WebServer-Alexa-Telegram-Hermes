# Graph Report - .  (2026-05-27)

## Corpus Check
- Corpus is ~15,905 words - fits in a single context window. You may not need a graph.

## Summary
- 310 nodes · 388 edges · 82 communities (43 shown, 39 thin omitted)
- Extraction: 80% EXTRACTED · 20% INFERRED · 0% AMBIGUOUS · INFERRED: 76 edges (avg confidence: 0.84)
- Token cost: 0 input · 0 output

## Community Hubs (Navigation)
- [[_COMMUNITY_HA MQTT Discovery|HA MQTT Discovery]]
- [[_COMMUNITY_Documentation & Architecture|Documentation & Architecture]]
- [[_COMMUNITY_External Integrations|External Integrations]]
- [[_COMMUNITY_Core Infrastructure|Core Infrastructure]]
- [[_COMMUNITY_HA Discovery Implementation|HA Discovery Implementation]]
- [[_COMMUNITY_Web API Handlers|Web API Handlers]]
- [[_COMMUNITY_Code Quality Concerns|Code Quality Concerns]]
- [[_COMMUNITY_Telegram Bot Control|Telegram Bot Control]]
- [[_COMMUNITY_Relay State Management|Relay State Management]]
- [[_COMMUNITY_WiFiMQTT Config|WiFi/MQTT Config]]
- [[_COMMUNITY_Library Aggregators|Library Aggregators]]
- [[_COMMUNITY_Community 11|Community 11]]
- [[_COMMUNITY_Community 12|Community 12]]
- [[_COMMUNITY_Community 13|Community 13]]
- [[_COMMUNITY_Community 14|Community 14]]
- [[_COMMUNITY_Community 15|Community 15]]
- [[_COMMUNITY_Community 16|Community 16]]
- [[_COMMUNITY_Community 17|Community 17]]
- [[_COMMUNITY_Community 19|Community 19]]
- [[_COMMUNITY_Community 20|Community 20]]
- [[_COMMUNITY_Community 21|Community 21]]
- [[_COMMUNITY_Community 22|Community 22]]
- [[_COMMUNITY_Community 24|Community 24]]
- [[_COMMUNITY_Community 25|Community 25]]
- [[_COMMUNITY_Community 35|Community 35]]
- [[_COMMUNITY_Community 36|Community 36]]
- [[_COMMUNITY_Community 37|Community 37]]
- [[_COMMUNITY_Community 38|Community 38]]
- [[_COMMUNITY_Community 39|Community 39]]
- [[_COMMUNITY_Community 40|Community 40]]
- [[_COMMUNITY_Community 41|Community 41]]
- [[_COMMUNITY_Community 42|Community 42]]
- [[_COMMUNITY_Community 43|Community 43]]
- [[_COMMUNITY_Community 44|Community 44]]
- [[_COMMUNITY_Community 45|Community 45]]
- [[_COMMUNITY_Community 46|Community 46]]
- [[_COMMUNITY_Community 47|Community 47]]
- [[_COMMUNITY_Community 48|Community 48]]
- [[_COMMUNITY_Community 49|Community 49]]
- [[_COMMUNITY_Community 50|Community 50]]
- [[_COMMUNITY_Community 51|Community 51]]
- [[_COMMUNITY_Community 52|Community 52]]
- [[_COMMUNITY_Community 53|Community 53]]
- [[_COMMUNITY_Community 54|Community 54]]
- [[_COMMUNITY_Community 55|Community 55]]
- [[_COMMUNITY_Community 56|Community 56]]
- [[_COMMUNITY_Community 57|Community 57]]
- [[_COMMUNITY_Community 58|Community 58]]
- [[_COMMUNITY_Community 59|Community 59]]
- [[_COMMUNITY_Community 60|Community 60]]
- [[_COMMUNITY_Community 61|Community 61]]
- [[_COMMUNITY_Community 62|Community 62]]
- [[_COMMUNITY_Community 63|Community 63]]
- [[_COMMUNITY_Community 75|Community 75]]

## God Nodes (most connected - your core abstractions)
1. `Relay State Variables (RelayState1-8, Todos)` - 15 edges
2. `Documentation Index` - 10 edges
3. `Fix mDNS and OTA Hostname Plan` - 8 edges
4. `logRelayAction()` - 8 edges
5. `mqttCallback()` - 8 edges
6. `handleTelegramMessages()` - 7 edges
7. `ESP32 Project Analysis Task Plan` - 7 edges
8. `SinricPro Sync Pending Flag` - 7 edges
9. `mqttCallback()` - 7 edges
10. `saveRelayState()` - 7 edges

## Surprising Connections (you probably didn't know these)
- `initTelegram()` --shares_data_with--> `Relay State Variables (RelayState1-8, Todos)`  [INFERRED]
  TelegramBotESP32.cpp → VariaveisGlobais.h
- `initTelegram()` --references--> `Credentials Template (MQTT, Telegram, SinricPro)`  [INFERRED]
  TelegramBotESP32.cpp → LoginsSenhas_example.cpp.txt
- `onPowerState()` --calls--> `saveRelayState()`  [INFERRED]
  SinricProESP32.h → RelayManager.cpp
- `onPowerState()` --calls--> `logRelayAction()`  [INFERRED]
  SinricProESP32.h → RelayManager.cpp
- `onPowerState()` --calls--> `notifyTelegramStateChange()`  [INFERRED]
  SinricProESP32.h → TelegramBotESP32.cpp

## Hyperedges (group relationships)
- **Documentation Portal (INDICE)** — docsINDICEmd_DocIndex, PROJETO_DETALHADO_Guide, CHEATSHEET_Reference [EXTRACTED 1.00]
- **WiFi Initialization Stack Fix** — docsPlanFixMdnsOta, MDNSService, OTAUpdateService, WiFiManagerLibrary, TaskConexoesCore0, DeviceHostnameConstant [INFERRED 0.80]
- **Configuration Hardcoding Elimination Initiative** — docsPlanFixMdnsOta, docsPlanRemoveHardcodedWifi, docsSpecMqttWifiManager, docsSpecRemoveHardcodedWifi, HardcodedConfigRemoval [INFERRED 0.85]

## Communities (82 total, 39 thin omitted)

### Community 0 - "HA MQTT Discovery"
Cohesion: 0.11
Nodes (24): HA Device (esp32_quarto_robson_v2), publishHADiscovery(), mqttCallback(), reconnectMQTT(), loadAllRelayStates(), logRelayAction(), saveRelayState(), setAllRelays() (+16 more)

### Community 1 - "Documentation & Architecture"
Cohesion: 0.15
Nodes (25): CHEATSHEET.md Quick Reference, DHCP-Only Networking, DEVICE_HOSTNAME Constant, FreeRTOS Multi-Core Architecture, Global Hostname Centralization Design, Hardcoded Configuration Removal Initiative, Hermes Natural Language Commands, Home Assistant MQTT Discovery (+17 more)

### Community 2 - "External Integrations"
Cohesion: 0.18
Nodes (21): SinricPro Alexa Integration, Telegram Bot Controller, Web Dashboard Server, Generate Telegram Keyboard JSON, API All Relays Control, API Single Relay Control, Process Telegram Commands, Initialize Telegram Bot (+13 more)

### Community 3 - "Core Infrastructure"
Cohesion: 0.14
Nodes (15): Bibliotecas.h (Library Aggregator), DualSerialClass (Telnet debug stream), FreeRTOS Task Handles (hTaskConexoes/Sensores/Telegram), GPIOs.h (Pin Definitions), OTAConfig.cpp (OTA Implementation), OTAConfig.h (OTA Header), RemoteDebug.h (Dual Serial/Telnet), VariaveisGlobais.cpp (Global Definitions) (+7 more)

### Community 4 - "HA Discovery Implementation"
Cohesion: 0.13
Nodes (6): _publishHA(), publishHADiscovery(), publishRelayStates(), reconnectMQTT(), _publishHA(), publishHADiscovery()

### Community 5 - "Web API Handlers"
Cohesion: 0.17
Nodes (11): _ensureRelayNames(), getRelayName(), handleApiRelays(), handleApiSetName(), _ensureRelayNames(), getRelayName(), handleApiAll(), handleApiRelay() (+3 more)

### Community 6 - "Code Quality Concerns"
Cohesion: 0.17
Nodes (9): Code Duplication, ESP32 Automação Residencial, Mutex/Semaphore, NVS (Non-Volatile Storage), Race Condition, Telnet Remote Serial, TLS Certificate Validation, VariaveisGlobais (+1 more)

### Community 7 - "Telegram Bot Control"
Cohesion: 0.33
Nodes (12): getControlKeyboard(), handleTelegramMessages(), initTelegram(), processPendingTelegramNotification(), sendMainMenu(), telegramLoop(), getControlKeyboard(), handleTelegramMessages() (+4 more)

### Community 8 - "Relay State Management"
Cohesion: 0.33
Nodes (13): mqttCallback(), applyRelayStatesToPins(), findRelayByName(), loadAllRelayStates(), loadRelayState(), logRelayAction(), saveRelayState(), setAllRelays() (+5 more)

### Community 9 - "WiFi/MQTT Config"
Cohesion: 0.19
Nodes (6): LoginsSenhas.h (Credentials Hub), loadStaticIPConfig(), saveMQTTConfigCallback(), saveMQTTConfigToNVS(), saveStaticIPConfig(), WiFiManagerSetup()

### Community 11 - "Community 11"
Cohesion: 0.22
Nodes (8): deleted_files, new_files, code, document, image, paper, video, new_total

### Community 12 - "Community 12"
Cohesion: 0.40
Nodes (4): deleted_files, new_files, code, document

### Community 15 - "Community 15"
Cohesion: 0.67
Nodes (3): Project Definition Constants, MQTT Topic Definitions, WiFi Manager Configuration

### Community 16 - "Community 16"
Cohesion: 0.67
Nodes (3): WiFiManager Portal Setup, Save MQTT Config Callback, MQTT Topic String Constants

### Community 17 - "Community 17"
Cohesion: 0.67
Nodes (3): Finalize Visualization (networkx), Finalize Visualization (graphify), Generate Visualization Standalone

## Knowledge Gaps
- **76 isolated node(s):** `new_total`, `deleted_files`, `code`, `document`, `paper` (+71 more)
  These have ≤1 connection - possible missing edges or undocumented components.
- **39 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **Why does `VariaveisGlobais.h (Global State Header)` connect `Core Infrastructure` to `WiFi/MQTT Config`, `Web API Handlers`, `Telegram Bot Control`?**
  _High betweenness centrality (0.039) - this node is a cross-community bridge._
- **Why does `Relay State Variables (RelayState1-8, Todos)` connect `HA MQTT Discovery` to `Community 13`?**
  _High betweenness centrality (0.007) - this node is a cross-community bridge._
- **Are the 15 inferred relationships involving `Relay State Variables (RelayState1-8, Todos)` (e.g. with `mqttCallback()` and `publishRelayStates()`) actually correct?**
  _`Relay State Variables (RelayState1-8, Todos)` has 15 INFERRED edges - model-reasoned connections that need verification._
- **Are the 4 inferred relationships involving `logRelayAction()` (e.g. with `onPowerState()` and `handleApiRelay()`) actually correct?**
  _`logRelayAction()` has 4 INFERRED edges - model-reasoned connections that need verification._
- **Are the 3 inferred relationships involving `mqttCallback()` (e.g. with `Relay State Variables (RelayState1-8, Todos)` and `Cross-Core Sync Flags`) actually correct?**
  _`mqttCallback()` has 3 INFERRED edges - model-reasoned connections that need verification._
- **What connects `new_total`, `deleted_files`, `code` to the rest of the system?**
  _76 weakly-connected nodes found - possible documentation gaps or missing edges._
- **Should `HA MQTT Discovery` be split into smaller, more focused modules?**
  _Cohesion score 0.10984848484848485 - nodes in this community are weakly interconnected._