# Graph Report - .  (2026-05-26)

## Corpus Check
- 40 files · ~17,619 words
- Verdict: corpus is large enough that graph structure adds value.

## Summary
- 237 nodes · 245 edges · 75 communities (39 shown, 36 thin omitted)
- Extraction: 91% EXTRACTED · 9% INFERRED · 0% AMBIGUOUS · INFERRED: 23 edges (avg confidence: 0.79)
- Token cost: 0 input · 0 output

## Community Hubs (Navigation)
- [[_COMMUNITY_Architecture & Config Docs|Architecture & Config Docs]]
- [[_COMMUNITY_Alexa & Telegram Control|Alexa & Telegram Control]]
- [[_COMMUNITY_Web Server API|Web Server API]]
- [[_COMMUNITY_Core Infrastructure|Core Infrastructure]]
- [[_COMMUNITY_Code Improvements & Tasks|Code Improvements & Tasks]]
- [[_COMMUNITY_Telegram Bot Functions|Telegram Bot Functions]]
- [[_COMMUNITY_WiFi & Network Config|WiFi & Network Config]]
- [[_COMMUNITY_Graphify Incremental Tracking|Graphify Incremental Tracking]]
- [[_COMMUNITY_Graphify Incremental Fix|Graphify Incremental Fix]]
- [[_COMMUNITY_Visualization Scripts|Visualization Scripts]]
- [[_COMMUNITY_Configuration Constants|Configuration Constants]]
- [[_COMMUNITY_Home Assistant MQTT Discovery|Home Assistant MQTT Discovery]]
- [[_COMMUNITY_Graph Finalization|Graph Finalization]]
- [[_COMMUNITY_Community 16|Community 16]]
- [[_COMMUNITY_Community 17|Community 17]]
- [[_COMMUNITY_Community 18|Community 18]]
- [[_COMMUNITY_Community 20|Community 20]]
- [[_COMMUNITY_Community 21|Community 21]]
- [[_COMMUNITY_Community 22|Community 22]]
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
- `RemoteDebug.h (Dual Serial/Telnet)` --references--> `VariaveisGlobais.h (Global State Header)`  [INFERRED]
  RemoteDebug.h → VariaveisGlobais.h
- `Progress Log` --references--> `ESP32 Project Analysis Task Plan`  [EXTRACTED]
  progress.md → task_plan.md
- `ESP32 Project Analysis Task Plan` --references--> `Telegram Bot Integration`  [EXTRACTED]
  task_plan.md → docs/INDICE.md
- `ESP32 Project Analysis Task Plan` --references--> `SinricPro Alexa/Google Home`  [EXTRACTED]
  task_plan.md → docs/INDICE.md
- `SinricPro Alexa Integration` --shares_data_with--> `MQTT Sync Pending Flag`  [INFERRED]
  SinricProESP32.h → VariaveisGlobais.h

## Hyperedges (group relationships)
- **Documentation Portal (INDICE)** — docsINDICEmd_DocIndex, PROJETO_DETALHADO_Guide, CHEATSHEET_Reference [EXTRACTED 1.00]
- **WiFi Initialization Stack Fix** — docsPlanFixMdnsOta, MDNSService, OTAUpdateService, WiFiManagerLibrary, TaskConexoesCore0, DeviceHostnameConstant [INFERRED 0.80]
- **Configuration Hardcoding Elimination Initiative** — docsPlanFixMdnsOta, docsPlanRemoveHardcodedWifi, docsSpecMqttWifiManager, docsSpecRemoveHardcodedWifi, HardcodedConfigRemoval [INFERRED 0.85]

## Communities (75 total, 36 thin omitted)

### Community 0 - "Architecture & Config Docs"
Cohesion: 0.15
Nodes (25): CHEATSHEET.md Quick Reference, DHCP-Only Networking, DEVICE_HOSTNAME Constant, FreeRTOS Multi-Core Architecture, Global Hostname Centralization Design, Hardcoded Configuration Removal Initiative, Hermes Natural Language Commands, Home Assistant MQTT Discovery (+17 more)

### Community 1 - "Alexa & Telegram Control"
Cohesion: 0.18
Nodes (21): SinricPro Alexa Integration, Telegram Bot Controller, Web Dashboard Server, Generate Telegram Keyboard JSON, API All Relays Control, API Single Relay Control, Process Telegram Commands, Initialize Telegram Bot (+13 more)

### Community 2 - "Web Server API"
Cohesion: 0.15
Nodes (13): getRelayName(), handleApiAll(), handleApiRelay(), handleApiRelays(), handleApiSetName(), setupWebServer(), _ensureRelayNames(), getRelayName() (+5 more)

### Community 3 - "Core Infrastructure"
Cohesion: 0.15
Nodes (15): Bibliotecas.h (Library Aggregator), DualSerialClass (Telnet debug stream), FreeRTOS Task Handles (hTaskConexoes/Sensores/Telegram), GPIOs.h (Pin Definitions), OTAConfig.cpp (OTA Implementation), OTAConfig.h (OTA Header), RemoteDebug.h (Dual Serial/Telnet), VariaveisGlobais.cpp (Global Definitions) (+7 more)

### Community 4 - "Code Improvements & Tasks"
Cohesion: 0.17
Nodes (9): Code Duplication, ESP32 Automação Residencial, Mutex/Semaphore, NVS (Non-Volatile Storage), Race Condition, Telnet Remote Serial, TLS Certificate Validation, VariaveisGlobais (+1 more)

### Community 5 - "Telegram Bot Functions"
Cohesion: 0.33
Nodes (12): getControlKeyboard(), handleTelegramMessages(), initTelegram(), processPendingTelegramNotification(), sendMainMenu(), telegramLoop(), getControlKeyboard(), handleTelegramMessages() (+4 more)

### Community 6 - "WiFi & Network Config"
Cohesion: 0.19
Nodes (6): LoginsSenhas.h (Credentials Hub), loadStaticIPConfig(), saveMQTTConfigCallback(), saveMQTTConfigToNVS(), saveStaticIPConfig(), WiFiManagerSetup()

### Community 7 - "Graphify Incremental Tracking"
Cohesion: 0.22
Nodes (8): deleted_files, new_files, code, document, image, paper, video, new_total

### Community 9 - "Graphify Incremental Fix"
Cohesion: 0.40
Nodes (4): deleted_files, new_files, code, document

### Community 12 - "Configuration Constants"
Cohesion: 0.67
Nodes (3): Project Definition Constants, MQTT Topic Definitions, WiFi Manager Configuration

### Community 14 - "Graph Finalization"
Cohesion: 0.67
Nodes (3): Finalize Visualization (networkx), Finalize Visualization (graphify), Generate Visualization Standalone

### Community 18 - "Community 18"
Cohesion: 0.67
Nodes (3): WiFiManager Portal Setup, Save MQTT Config Callback, MQTT Topic String Constants

## Knowledge Gaps
- **70 isolated node(s):** `new_total`, `deleted_files`, `code`, `document`, `paper` (+65 more)
  These have ≤1 connection - possible missing edges or undocumented components.
- **36 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **Why does `VariaveisGlobais.h (Global State Header)` connect `Core Infrastructure` to `Web Server API`, `Telegram Bot Functions`, `WiFi & Network Config`?**
  _High betweenness centrality (0.049) - this node is a cross-community bridge._
- **Why does `Bibliotecas.h (Library Aggregator)` connect `Core Infrastructure` to `Telegram Bot Functions`?**
  _High betweenness centrality (0.005) - this node is a cross-community bridge._
- **What connects `new_total`, `deleted_files`, `code` to the rest of the system?**
  _70 weakly-connected nodes found - possible documentation gaps or missing edges._
- **Should `Architecture & Config Docs` be split into smaller, more focused modules?**
  _Cohesion score 0.14666666666666667 - nodes in this community are weakly interconnected._