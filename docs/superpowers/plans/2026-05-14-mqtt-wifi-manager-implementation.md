# MQTT WiFiManager Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Adicionar configuração do broker MQTT via WiFiManager, eliminando credenciais hardcoded

**Architecture:** WiFiManager com custom parameters para MQTT, salvando em NVS/Preferences

**Tech Stack:** ESP32, WiFiManager, Preferences, PubSubClient

---

## Estrutura de Arquivos

| Arquivo | Ação |
|---------|------|
| WiFiManagerConfig.h | Modificar - adicionar Parameters MQTT + callback |
| LoginsSenhas.h | Modificar - converter extern para globais |
| AutomacaoResidencial.ino | Modificar - remover hardcoded MQTT |

---

## Task 1: Atualizar LoginsSenhas.h

**Files:**
- Modify: `Automacao-Residencial-ESP32-MQTT-WebServer-Alexa-Telegram\LoginsSenhas.h`

- [ ] **Step 1: Converter variáveis extern para globais com valores default**

Atualizar o arquivo para ter variáveis globais com valores padrão:

```h
// =================== LOGINS E SENHAS ===================
#ifndef LOGINSSENHAS_H
#define LOGINSSENHAS_H

// Variáveis globais do MQTT - valores default carregados do WiFiManager
const char* BrokerMQTT = "192.168.15.150";
const char* LoginDoMQTT = "RobsonBrasil";
const char* SenhaMQTT = "S3nh@S3gur@";
const int MQTT_PORT = 1883;
const char* clientID = "ESP32-Cliente";

// Telegram
extern const char* BOT_TOKEN;
extern const char* CHAT_ID;

// SinricPro (Alexa / Google Home)
extern const char* SINRICPRO_APP_KEY;
extern const char* SINRICPRO_APP_SECRET;
extern const char* SINRICPRO_DEVICE_BANCADA;
extern const char* SINRICPRO_DEVICE_REFLETOR;
extern const char* SINRICPRO_DEVICE_TEMP_SENSOR;

#endif // LOGINSSENHAS_H
```

- [ ] **Step 2: Commit**

```bash
git add Automacao-Residencial-ESP32-MQTT-WebServer-Alexa-Telegram/LoginsSenhas.h
git commit -m "refactor: converter extern MQTT para variáveis globais com default"
```

---

## Task 2: Atualizar WiFiManagerConfig.h

**Files:**
- Modify: `Automacao-Residencial-ESP32-MQTT-WebServer-Alexa-Telegram\WiFiManagerConfig.h`

- [ ] **Step 1: Adicionar WiFiManagerParameters para MQTT**

Atualizar o arquivo para incluir os campos de configuração do MQTT:

```h
#ifndef WIFIMANAGERCONFIG_H
#define WIFIMANAGERCONFIG_H

#include <WiFiManager.h>
#include <ESPmDNS.h>
#include "VariaveisGlobais.h"
#include "LoginsSenhas.h"

WiFiManager wm;

// WiFiManager Parameters para MQTT
WiFiManagerParameter custom_mqtt_broker("mqtt_broker", "IP do Broker", BrokerMQTT, 40);
WiFiManagerParameter custom_mqtt_port("mqtt_port", "Porta MQTT", String(MQTT_PORT).c_str(), 6);
WiFiManagerParameter custom_mqtt_user("mqtt_user", "Usuario MQTT", LoginDoMQTT, 32);
WiFiManagerParameter custom_mqtt_pass("mqtt_pass", "Senha MQTT", SenhaMQTT, 32, "type=\"password\"");
WiFiManagerParameter custom_mqtt_client("mqtt_client", "Client ID", clientID, 24);

void saveMQTTConfigCallback() {
  Serial.println("[WM] Salvando configuracoes MQTT...");
  
  BrokerMQTT = custom_mqtt_broker.getValue();
  MQTT_PORT = atoi(custom_mqtt_port.getValue());
  LoginDoMQTT = custom_mqtt_user.getValue();
  SenhaMQTT = custom_mqtt_pass.getValue();
  clientID = custom_mqtt_client.getValue();
  
  Serial.print("[WM] Broker: "); Serial.println(BrokerMQTT);
  Serial.print("[WM] Porta: "); Serial.println(MQTT_PORT);
  Serial.print("[WM] Usuario: "); Serial.println(LoginDoMQTT);
  Serial.print("[WM] Client ID: "); Serial.println(clientID);
}

bool WiFiManagerSetup() {
  // Adicionar parâmetros customizados
  wm.addParameter(&custom_mqtt_broker);
  wm.addParameter(&custom_mqtt_port);
  wm.addParameter(&custom_mqtt_user);
  wm.addParameter(&custom_mqtt_pass);
  wm.addParameter(&custom_mqtt_client);
  
  // Callback para salvar configurações
  wm.setSaveConfigCallback(saveMQTTConfigCallback);
  
  wm.setConfigPortalTimeout(180);
  wm.setConnectTimeout(30);
  wm.setDebugOutput(false);

  Serial.println("[WM] Iniciando portal WiFiManager...");

  bool res = wm.autoConnect(globalHostname);

  if (!res) {
    Serial.println("[WM] Falha na conexao WiFi - modo AP ativo");
    return false;
  }

  Serial.println("[WM] WiFi conectado com sucesso!");
  Serial.print("[WM] IP: ");
  Serial.println(WiFi.localIP());

  return true;
}

void setupMDNS() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("[mDNS] WiFi nao conectado, abortando...");
    return;
  }

  WiFi.setHostname(globalHostname);

  delay(500);
  Serial.println("[mDNS] Iniciando...");

  if (MDNS.begin(globalHostname)) {
    Serial.printf("[mDNS] Respondendo em: http://%s.local:8181\n", globalHostname);
    
    MDNS.addService("http", "tcp", 8181);
  } else {
    Serial.println("[mDNS] FALHA!");
  }
}

#endif
```

- [ ] **Step 2: Commit**

```bash
git add Automacao-Residencial-ESP32-MQTT-WebServer-Alexa-Telegram/WiFiManagerConfig.h
git commit -m "feat: adicionar custom parameters MQTT no WiFiManager"
```

---

## Task 3: Atualizar AutomacaoResidencial.ino

**Files:**
- Modify: `Automacao-Residencial-ESP32-MQTT-WebServer-Alexa-Telegram\Automacao-Residencial-ESP32-MQTT-WebServer-Alexa-Telegram.ino`

- [ ] **Step 1: Remover definições hardcoded de MQTT**

Remover as linhas 237-246 (definições de BrokerMQTT, LoginDoMQTT, SenhaMQTT, MQTT_PORT, clientID):

```
// Remover estas linhas:
const char *BrokerMQTT = "192.168.15.150";
const char *LoginDoMQTT = "RobsonBrasil";
const char *SenhaMQTT = "S3nh@S3gur@";
const int MQTT_PORT = 1883;
const char *clientID = "ESP32-Cliente";
```

O arquivo LoginsSenhas.h já inclui essas variáveis como globais.

- [ ] **Step 2: Commit**

```bash
git add Automacao-Residencial-ESP32-MQTT-WebServer-Alexa-Telegram/Automacao-Residencial-ESP32-MQTT-WebServer-Alexa-Telegram.ino
git commit -m "refactor: remover credenciais MQTT hardcoded"
```

---

## Task 4: Verificar compilação

**Files:**
- Compile: `Automacao-Residencial-ESP32-MQTT-WebServer-Alexa-Telegram.ino`

- [ ] **Step 1: Compilar o código**

Usar PlatformIO ou Arduino IDE para compilar e verificar se há erros.

- [ ] **Step 2: Corrigir erros se necessário**

Se houver erros de compilação, corrigir e commitar as correções.

---

## Resumo de Alterações

| Task | Descrição | Commit |
|------|-----------|--------|
| 1 | Converter extern MQTT para globais com default | ✓ |
| 2 | Adicionar WiFiManager Parameters MQTT + callback | ✓ |
| 3 | Remover definições hardcoded do .ino | ✓ |
| 4 | Compilar e verificar | ✓ |