# Graph Report - .  (2026-05-23)

## Corpus Check
- Corpus is ~15,847 words - fits in a single context window. You may not need a graph.

## Summary
- 174 nodes · 176 edges · 59 communities (26 shown, 33 thin omitted)
- Extraction: 89% EXTRACTED · 11% INFERRED · 0% AMBIGUOUS · INFERRED: 19 edges (avg confidence: 0.78)
- Token cost: 0 input · 0 output

## Community Hubs (Navigation)
- [[_COMMUNITY_Cloud Integration Controllers|Cloud Integration Controllers]]
- [[_COMMUNITY_System Config & Networking|System Config & Networking]]
- [[_COMMUNITY_Library & Debug Infrastructure|Library & Debug Infrastructure]]
- [[_COMMUNITY_Web API Handlers|Web API Handlers]]
- [[_COMMUNITY_Credentials & WiFi Config|Credentials & WiFi Config]]
- [[_COMMUNITY_WiFiManager & MQTT Setup|WiFiManager & MQTT Setup]]
- [[_COMMUNITY_Community 6|Community 6]]
- [[_COMMUNITY_GPIOs & SinricPro Events|GPIOs & SinricPro Events]]
- [[_COMMUNITY_FreeRTOS Tasks & OTA|FreeRTOS Tasks & OTA]]
- [[_COMMUNITY_Community 9|Community 9]]
- [[_COMMUNITY_Community 12|Community 12]]
- [[_COMMUNITY_Community 13|Community 13]]
- [[_COMMUNITY_Community 15|Community 15]]
- [[_COMMUNITY_Community 16|Community 16]]
- [[_COMMUNITY_Community 17|Community 17]]
- [[_COMMUNITY_Community 18|Community 18]]
- [[_COMMUNITY_Project Documentation|Project Documentation]]
- [[_COMMUNITY_Community 30|Community 30]]
- [[_COMMUNITY_Community 31|Community 31]]
- [[_COMMUNITY_Community 32|Community 32]]
- [[_COMMUNITY_Community 33|Community 33]]
- [[_COMMUNITY_Community 34|Community 34]]
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
- [[_COMMUNITY_GitNexus Code Intelligence|GitNexus Code Intelligence]]
- [[_COMMUNITY_IoT Dashboard Interface|IoT Dashboard Interface]]
- [[_COMMUNITY_Project Analysis|Project Analysis]]
- [[_COMMUNITY_Quick Reference|Quick Reference]]
- [[_COMMUNITY_Community 55|Community 55]]
- [[_COMMUNITY_Community 56|Community 56]]
- [[_COMMUNITY_Community 57|Community 57]]
- [[_COMMUNITY_Community 58|Community 58]]

## God Nodes (most connected - your core abstractions)
1. `Documentation Index` - 10 edges
2. `Fix mDNS and OTA Hostname Plan` - 8 edges
3. `ESP32 Project Analysis Task Plan` - 7 edges
4. `SinricPro Sync Pending Flag` - 7 edges
5. `new_files` - 6 edges
6. `VariaveisGlobais.h (Global State Header)` - 6 edges
7. `MQTT WiFiManager Integration Design` - 6 edges
8. `Relay State Variables (R1-R8)` - 6 edges
9. `Remove Hardcoded WiFi/IP Plan` - 5 edges
10. `Remove Hardcoded WiFi/IP Design Spec` - 5 edges

## Surprising Connections (you probably didn't know these)
- `OTAConfig.cpp (OTA Implementation)` --references--> `VariaveisGlobais.h (Global State Header)`  [EXTRACTED]
  OTAConfig.cpp → VariaveisGlobais.h
- `RemoteDebug.h (Dual Serial/Telnet)` --references--> `VariaveisGlobais.h (Global State Header)`  [INFERRED]
  RemoteDebug.h → VariaveisGlobais.h
- `ESP32 Project Analysis Task Plan` --references--> `Progress Log`  [EXTRACTED]
  task_plan.md → progress.md
- `ESP32 Project Analysis Task Plan` --references--> `MQTT Broker Configuration`  [EXTRACTED]
  task_plan.md → docs/superpowers/specs/2026-05-14-mqtt-wifi-manager-design.md
- `ESP32 Project Analysis Task Plan` --references--> `Telegram Bot Integration`  [EXTRACTED]
  task_plan.md → docs/INDICE.md

## Hyperedges (group relationships)
- **Cross-Core Synchronization Mechanism** — CrossCoreSyncFlags, TelegramNotificationQueue, FreeRTOS_TaskHandles, VariaveisGlobais_h [INFERRED 0.90]
- **Documentation Portal (INDICE)** — docsINDICEmd_DocIndex, PROJETO_DETALHADO_Guide, CHEATSHEET_Reference [EXTRACTED 1.00]
- **WiFi Initialization Stack Fix** — docsPlanFixMdnsOta, MDNSService, OTAUpdateService, WiFiManagerLibrary, TaskConexoesCore0, DeviceHostnameConstant [INFERRED 0.80]
- **Configuration Hardcoding Elimination Initiative** — docsPlanFixMdnsOta, docsPlanRemoveHardcodedWifi, docsSpecMqttWifiManager, docsSpecRemoveHardcodedWifi, HardcodedConfigRemoval [INFERRED 0.85]

## Communities (59 total, 33 thin omitted)

### Community 0 - "Cloud Integration Controllers"
Cohesion: 0.18
Nodes (21): SinricPro Alexa Integration, Telegram Bot Controller, Web Dashboard Server, Generate Telegram Keyboard JSON, API All Relays Control, API Single Relay Control, Process Telegram Commands, Initialize Telegram Bot (+13 more)

### Community 1 - "System Config & Networking"
Cohesion: 0.20
Nodes (16): CHEATSHEET.md Quick Reference, DEVICE_HOSTNAME Constant, FreeRTOS Multi-Core Architecture, Global Hostname Centralization Design, Hermes Natural Language Commands, Home Assistant MQTT Discovery, mDNS (ESPmDNS), OTA Update (ArduinoOTA) (+8 more)

### Community 2 - "Library & Debug Infrastructure"
Cohesion: 0.29
Nodes (11): Bibliotecas.h (Library Aggregator), DualSerialClass (Telnet debug stream), OTAConfig.h (OTA Header), RemoteDebug.h (Dual Serial/Telnet), VariaveisGlobais.h (Global State Header), getControlKeyboard(), handleTelegramMessages(), initTelegram() (+3 more)

### Community 3 - "Web API Handlers"
Cohesion: 0.29
Nodes (6): getRelayName(), handleApiAll(), handleApiRelay(), handleApiRelays(), handleApiSetName(), setupWebServer()

### Community 4 - "Credentials & WiFi Config"
Cohesion: 0.27
Nodes (6): LoginsSenhas.h (Credentials Hub), loadStaticIPConfig(), saveMQTTConfigCallback(), saveMQTTConfigToNVS(), saveStaticIPConfig(), WiFiManagerSetup()

### Community 5 - "WiFiManager & MQTT Setup"
Cohesion: 0.42
Nodes (9): DHCP-Only Networking, Hardcoded Configuration Removal Initiative, MQTT Broker Configuration, NVS Flash Persistence, WiFiManager Library, WiFiManager Custom MQTT Parameters, Remove Hardcoded WiFi/IP Plan, MQTT WiFiManager Integration Design (+1 more)

### Community 6 - "Community 6"
Cohesion: 0.22
Nodes (8): deleted_files, new_files, code, document, image, paper, video, new_total

### Community 7 - "GPIOs & SinricPro Events"
Cohesion: 0.33
Nodes (3): GPIOs.h (Pin Definitions), onPowerState(), setupSinricPro()

### Community 8 - "FreeRTOS Tasks & OTA"
Cohesion: 0.40
Nodes (4): FreeRTOS Task Handles (hTaskConexoes/Sensores/Telegram), OTAConfig.cpp (OTA Implementation), VariaveisGlobais.cpp (Global Definitions), setupOTA (function)

### Community 9 - "Community 9"
Cohesion: 0.40
Nodes (4): deleted_files, new_files, code, document

### Community 12 - "Community 12"
Cohesion: 0.67
Nodes (3): Project Definition Constants, MQTT Topic Definitions, WiFi Manager Configuration

### Community 15 - "Community 15"
Cohesion: 0.67
Nodes (3): Finalize Visualization (networkx), Finalize Visualization (graphify), Generate Visualization Standalone

### Community 17 - "Community 17"
Cohesion: 0.67
Nodes (3): WiFiManager Portal Setup, Save MQTT Config Callback, MQTT Topic String Constants

## Knowledge Gaps
- **66 isolated node(s):** `new_total`, `deleted_files`, `code`, `document`, `paper` (+61 more)
  These have ≤1 connection - possible missing edges or undocumented components.
- **33 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **Why does `VariaveisGlobais.h (Global State Header)` connect `Library & Debug Infrastructure` to `FreeRTOS Tasks & OTA`, `Web API Handlers`, `Credentials & WiFi Config`, `GPIOs & SinricPro Events`?**
  _High betweenness centrality (0.043) - this node is a cross-community bridge._
- **Why does `OTAConfig.cpp (OTA Implementation)` connect `FreeRTOS Tasks & OTA` to `Library & Debug Infrastructure`?**
  _High betweenness centrality (0.011) - this node is a cross-community bridge._
- **Why does `Documentation Index` connect `System Config & Networking` to `WiFiManager & MQTT Setup`?**
  _High betweenness centrality (0.007) - this node is a cross-community bridge._
- **What connects `new_total`, `deleted_files`, `code` to the rest of the system?**
  _66 weakly-connected nodes found - possible documentation gaps or missing edges._