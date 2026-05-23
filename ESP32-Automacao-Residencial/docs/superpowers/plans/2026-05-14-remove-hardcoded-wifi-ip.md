# Limpeza de Wi-Fi e IP Fixo Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Remover credenciais de Wi-Fi e configurações de IP estático hardcoded, confiando 100% no WiFiManager e DHCP.

**Architecture:** O sistema deixará de tentar uma conexão inicial com dados fixos. A função `setupWiFi()` chamará diretamente o `WiFiManagerSetup()`, que gerencia a conexão com redes salvas ou abre o portal de configuração.

**Tech Stack:** Arduino/ESP32, WiFiManager.

---

### Task 1: Limpeza do Arquivo Principal (.ino)

**Files:**
- Modify: `Automacao-Residencial-ESP32-MQTT-WebServer-Alexa-Telegram.ino`

- [ ] **Step 1: Remover variáveis globais de rede**
Remover as definições de `local_IP`, `gateway`, `subnet`, `primaryDNS`, `secondaryDNS`, `ssid` e `password`.

- [ ] **Step 2: Refatorar a função `setupWiFi()`**
Remover a lógica de tentativa inicial e o fallback de 15 segundos.

```cpp
void setupWiFi() {
  unsigned long currentMillis = millis();

  // Controle de tentativas de conexão
  if (currentMillis - lastWifiRetryTime < WIFI_RETRY_INTERVAL) {
    return;
  }
  lastWifiRetryTime = currentMillis;

  // Se não estiver conectado, inicia o WiFiManager imediatamente
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("[Rede] Iniciando WiFiManager...");
    if (WiFiManagerSetup()) {
      setupMDNS();
    }
  }

  // Controle do LED de status
  if (currentMillis - lastWifiBlinkTime >= WIFI_BLINK_INTERVAL) {
    lastWifiBlinkTime = currentMillis;
    digitalWrite(wifiLed, !digitalRead(wifiLed));
  }

  // Verifica status da conexão para logs
  if (WiFi.status() == WL_CONNECTED && !wifiConnected) {
    digitalWrite(wifiLed, HIGH); 
    wifiConnected = true;
    Serial.println("\n==============================");
    Serial.println("WiFi CONECTADO!");
    Serial.print("IP: "); Serial.println(WiFi.localIP());
    Serial.println("==============================");
  } else if (WiFi.status() != WL_CONNECTED) {
    wifiConnected = false;
  }
}
```

---

### Task 2: Simplificação do WiFiManagerConfig.h

**Files:**
- Modify: `WiFiManagerConfig.h`

- [ ] **Step 1: Remover declarações `extern` de IP**
Remover as linhas `extern IPAddress local_IP;` etc.

- [ ] **Step 2: Remover parâmetro de IP Fixo no portal**
Remover a criação e adição do objeto `custom_ip`.

```cpp
bool WiFiManagerSetup() {
    wm.setConfigPortalTimeout(180);
    wm.setConnectTimeout(30);
    wm.setDebugOutput(false);

    Serial.println("[WM] Iniciando portal WiFiManager...");

    // Tenta conectar ou abre portal
    bool res = wm.autoConnect(globalHostname);

    if (!res) {
        Serial.println("[WM] Falha na conexao WiFi - modo AP ativo");
        return false;
    }

    Serial.println("[WM] WiFi conectado!");
    return true;
}
```

---

### Task 3: Limpeza de LoginsSenhas.h e OTAConfig.cpp

**Files:**
- Modify: `LoginsSenhas.h`
- Modify: `OTAConfig.cpp`

- [ ] **Step 1: Remover SSID/Password de `LoginsSenhas.h`**
Remover `extern const char* ssid;` e `extern const char* password;`.

- [ ] **Step 2: Remover SSID/Password de `OTAConfig.cpp`**
Remover as mesmas declarações `extern`.
