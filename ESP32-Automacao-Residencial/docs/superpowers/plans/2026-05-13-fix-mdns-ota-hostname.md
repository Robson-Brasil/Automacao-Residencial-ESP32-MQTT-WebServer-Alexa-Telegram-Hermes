# Fix mDNS and OTA Hostname Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Unify the device hostname across mDNS and OTA services, giving priority to Static IP and ensuring clean multi-core initialization.

**Architecture:** Centralize hostname definition in `DefinicoesProjeto.h`, propagate via `VariaveisGlobais`, and update `OTAConfig` and `WiFiManagerConfig` to use the unified name. Clean up `setupWiFi` to prevent concurrent execution between cores.

**Tech Stack:** ESP32, Arduino, FreeRTOS, WiFiManager, ESPmDNS, ArduinoOTA.

---

### Task 1: Centralização do Hostname e Limpeza de Variáveis

**Files:**
- Modify: `DefinicoesProjeto.h`
- Modify: `VariaveisGlobais.h`
- Modify: `VariaveisGlobais.cpp`

- [ ] **Step 1: Define the Global Hostname**
In `DefinicoesProjeto.h`, add the hostname definition.

```cpp
#define DEVICE_HOSTNAME "automacaoresidencial"
```

- [ ] **Step 2: Declare in VariaveisGlobais.h**

```cpp
extern const char* globalHostname;
```

- [ ] **Step 3: Initialize in VariaveisGlobais.cpp**

```cpp
#include "DefinicoesProjeto.h"
const char* globalHostname = DEVICE_HOSTNAME;
```

### Task 2: Unificação no OTA e mDNS

**Files:**
- Modify: `OTAConfig.cpp`
- Modify: `WiFiManagerConfig.h`

- [ ] **Step 1: Adjust OTA to use Global Hostname**

```cpp
// In OTAConfig.cpp
void setupOTA() {
  ArduinoOTA.setPort(3232);
  ArduinoOTA.setHostname(globalHostname);
  // ...
}
```

- [ ] **Step 2: Adjust mDNS and WiFiManager**

```cpp
// In WiFiManagerConfig.h
void setupMDNS() {
    if (WiFi.status() != WL_CONNECTED) return;
    WiFi.setHostname(globalHostname); 
    if (MDNS.begin(globalHostname)) {
        Serial.printf("[mDNS] Respondendo em: http://%s.local:8181\n", globalHostname);
        MDNS.addService("http", "tcp", 8181);
    }
}

bool WiFiManagerSetup() {
    // ...
    bool res = wm.autoConnect(globalHostname);
    // ...
}
```

### Task 3: Correção da Lógica de Inicialização (Core 0 vs Core 1)

**Files:**
- Modify: `Automacao-Residencial-ESP32-MQTT-WebServer-Alexa-Telegram.ino`

- [ ] **Step 1: Remove redundant setupWiFi call from setup()**
The `TaskConexoes` (Core 0) already handles WiFi reconnection and initialization.

- [ ] **Step 2: Ensure proper order in TaskConexoes**

```cpp
if (WiFi.status() == WL_CONNECTED && !otaInitialized) {
    setupMDNS();
    setupOTA();
    otaInitialized = true;
}
```
