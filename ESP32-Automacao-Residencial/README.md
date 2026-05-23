# 🏠 Smart Home Hub ESP32 — Automação Residencial de Alta Performance

Firmware industrial robusto para ESP32 WROOM-32, projetado para controlar **8 zonas de carga (relés)** e monitorar o ambiente com sensores de alta precisão. Contempla integração nativa com os principais assistentes de voz e plataformas IoT.

## 🚀 Destaques do Projeto

- **Arquitetura Multi-Core (FreeRTOS):** Processamento paralelo para garantir latência zero.
- **Ecossistema Completo de Integração:**
  - 🌐 **MQTT** — Node-RED, Home Assistant, Homebridge
  - 🏠 **Home Assistant Discovery** — Auto-descoberta via MQTT (9 sensores + 9 switches)
  - 🔊 **Alexa / Google Home** — via SinricPro (2 switches + 1 sensor de temperatura)
  - 💬 **Telegram Bot** — teclado interativo com status em tempo real e notificações push
  - 📊 **Dashboard Web** — interface glassmorphism com API JSON (porta 8181)
  - 🗣️ **Hermes** — Comandos em texto natural via MQTT (`LIGAR:SALA`, `DESLIGAR:COZINHA`, etc.)
- **Persistência de Dados:** Estado dos relés, nomes personalizados e config MQTT salvos na memória não-volátil (NVS).
- **Atualização Remota (OTA):** Atualize o código sem cabos via WiFi (porta 3232).
- **Sensores Ambientais:** DHT22 (temp/umidade/orvalho) + BMP180 (pressão/altitude).
- **Remote Debug:** Espelhamento Serial via Telnet (porta 2323).
- **WiFiManager:** Portal de configuração com campos para WiFi + MQTT broker.
- **Auto Restart:** Reinicialização programada a cada 120 minutos para estabilidade.
- **Sincronização Cross-Platform:** Mudanças de qualquer plataforma (MQTT, voz, Telegram, Dashboard, Hermes) são propagadas para todas as outras.

## 📖 Documentação Completa

Toda a documentação detalhada, guias de montagem e configuração estão disponíveis na pasta `/docs`:

- [**📚 Índice Geral de Documentação**](./docs/INDICE.md)
- [**🛠️ Guia Detalhado do Projeto**](./docs/PROJETO_DETALHADO.md)
- [**⚡ Guia de Configuração Rápida**](./docs/CHEATSHEET.md)

---

## 🛠️ Tecnologias e Funcionalidades

### 🔌 Hardware Suportado
| Componente | Especificação |
|---|---|
| Microcontrolador | ESP32 WROOM-32 (38 pinos) |
| Relés | Módulo 8 canais 5V (Active-Low) |
| Sensor Temp/Umidade | DHT22 (GPIO 25) |
| Sensor Pressão/Altitude | BMP180 via I2C (GPIO 21/22) |
| LED Status WiFi | GPIO 2 |
| Botão Config | GPIO 0 (BOOT — 3s para portal) |

### 📡 Protocolos de Comunicação
| Protocolo | Descrição |
|---|---|
| **MQTT** | Broker configurável via portal, QoS 2 para relés, QoS 0 para sensores |
| **Home Assistant** | Auto-descoberta MQTT Discovery — 9 switches + 9 sensores |
| **SinricPro** | Alexa e Google Home — 2 switches + 1 sensor de temperatura |
| **Telegram** | Bot interativo com teclado dinâmico e notificações push |
| **Web Dashboard** | Interface glassmorphism + API REST JSON (porta 8181) |
| **Hermes** | Comandos em texto natural via tópico MQTT dedicado |
| **Telnet** | Debug remoto espelhando Serial (porta 2323) |

### 🌡️ Dados dos Sensores (publicados via MQTT)
**DHT22:**
- Temperatura (°C)
- Umidade (%)
- Sensação Térmica / Heat Index (°C)
- Ponto de Orvalho / Dew Point (°C)

**BMP180:**
- Temperatura (°C)
- Pressão Atmosférica Real (hPa)
- Pressão ao Nível do Mar (hPa)
- Altitude Real (m)
- Altitude Total acima do Nível do Mar (m)

### ⚙️ Características do Firmware
- **FreeRTOS Dual-Core:** Core 0 (WiFi/MQTT/Telegram/OTA/SinricPro/HA/Telnet) | Core 1 (Sensores/Relés/WebServer)
- **IPC Thread-Safe:** FreeRTOS Queue para notificações Telegram, flags para sincronização cross-core
- **WiFiManager:** Portal com configuração de WiFi + MQTT broker (persistido em NVS)
- **Auto-Reconexão:** WiFi e MQTT com intervalos configuráveis
- **OTA Seguro:** Senha de proteção e suspensão automática da task de sensores durante atualização
- **Auto Restart:** Reinicialização a cada 120 minutos
- **Persistência:** Nomes customizados, estados dos relés e config MQTT salvos em NVS

---

## 📁 Estrutura de Arquivos

```
├── Automacao-Residencial-ESP32-MQTT-WebServer-Alexa-Telegram.ino  # Arquivo principal
├── Bibliotecas.h                              # Includes centralizados
├── ConfigMQTT.h                               # Protótipos MQTT
├── ConstantesTempo.h                          # Intervalos de tarefas (extern)
├── DefinicoesProjeto.h                        # Meta-informações do projeto
├── GPIOs.h                                    # Mapeamento de pinos
├── HomeAssistantDiscovery.h                   # MQTT Discovery para Home Assistant
├── LoginsSenhas.h                             # Credenciais (extern buffers)
├── MQTTParametros.h                           # QoS, Retain, KeepAlive
├── OTAConfig.h / OTAConfig.cpp                # Configuração OTA
├── RemoteDebug.h                              # DualSerial (Telnet mirror)
├── Sensores.h                                 # Protótipos de leitura
├── SinricProESP32.h                           # Alexa/Google Home
├── TaskCores.h                                # Tasks FreeRTOS
├── TelegramBotESP32.h                         # Bot interativo
├── TopicosMQTT.h                              # Tópicos MQTT (extern)
├── VariaveisGlobais.h / VariaveisGlobais.cpp  # Estado global
├── WebServerESP32.h                           # Dashboard + API REST
├── WiFiManagerConfig.h                        # WiFiManager + mDNS + NVS MQTT
├── WiFiUtils.h                                # Protótipos WiFi
├── dashboard.html                             # Interface web (standalone)
└── docs/                                      # Documentação completa
```

---

## 🔧 Configuração Rápida

1. **WiFi e MQTT:** Abra o portal WiFiManager (automático ou botão BOOT 3s) e configure WiFi + Broker MQTT
2. **Credenciais Hardcoded:** Edite Telegram Token/Chat ID e SinricPro keys no arquivo `.ino`
3. **Tópicos MQTT:** Personalize o padrão `ESP32/...` em `TopicosMQTT.h`
4. **Referência BMP180:** Ajuste `pressaoNivelMar` e `altitudeNivelMar` para sua localização

---

## 📋 Requisitos de Hardware
- **Microcontrolador:** ESP32 (WROOM-32).
- **Sensores:** DHT22 (Umidade/Temp) e BMP180 (Pressão/Altitude).
- **Atuadores:** Módulo Relé de 8 Canais (Active-Low).

---

## 🧩 Publicação MQTT (Tópicos)

```
Base: ESP32/

PUBLISH (ESP32 envia):
  Temperatura                   → "24.50"  (DHT22, °C)
  Umidade                       → "65.20"  (DHT22, %)
  SensacaoTermica               → "25.10"  (DHT22, °C)
  PontoOrvalho                  → "18.30"  (DHT22, °C - calculado)
  BMP180/Temperatura            → "24.00"  (BMP180, °C)
  BMP180/PressaoAtmosferica/Real     → "1008.50" (hPa)
  BMP180/PressaoAtmosferica/NivelMar → "1012.00" (hPa)
  BMP180/AltitudeReal           → "32.10"  (m)
  BMP180/AltitudeNivelMar       → "124.10" (m)
  Interruptor[1-8]/Comando      → "0" ou "1" (estado atual)
  Ligar-DesligarTudo/Comando    → "0" ou "1"

SUBSCRIBE (ESP32 recebe):
  Interruptor[1-8]/Estado       → "1" (ligar) ou "0" (desligar)
  Ligar-DesligarTudo/Estado     → "1" (ligar todos) ou "0" (desligar todos)
  Hermes/Comando                → "LIGAR:SALA", "DESLIGAR:TUDO", "STATUS", etc.
```

---

## 🗣️ Comandos Hermes (Texto Natural)

Publique no tópico `ESP32/Hermes/Comando`:

| Comando | Ação |
|---------|------|
| `LIGAR:VARANDA` | Liga relé 1 |
| `LIGAR:BANCADA` | Liga relé 2 |
| `LIGAR:SALA` | Liga relé 3 |
| `LIGAR:COZINHA` | Liga relé 4 |
| `LIGAR:QUINTAL` | Liga relé 5 |
| `LIGAR:VAL` ou `LIGAR:VARÃO` | Liga relé 6 |
| `LIGAR:ROBSON` | Liga relé 7 |
| `LIGAR:KINHA` | Liga relé 8 |
| `LIGAR:TUDO` | Liga todos |
| `DESLIGAR:<ITEM>` | Desliga item específico |
| `DESLIGAR:TUDO` | Desliga todos |
| `STATUS` / `ESTADO` | Solicita status |
| `SENSORES` / `SENSOR` | Força leitura dos sensores |

---

## 🌐 Dashboard Web Local

Acesse `http://<IP_DO_ESP32>:8181` ou `http://automacaoresidencial.local:8181` para visualizar o dashboard e controlar os relés.

**Endpoints da API REST:**
| Endpoint | Descrição |
|---|---|
| `GET /api/relays` | Estados e nomes de todos os relés |
| `GET /api/sensors` | Leituras atuais de todos os sensores |
| `GET /api/relay/{id}/{state}` | Controla relé específico (id: 0-7, state: 0=off, 1=on) |
| `GET /api/all/{state}` | Liga/desliga todos os relés |
| `GET /api/setname/{id}/{name}` | Define nome customizado para relé |

---

## 🏠 Home Assistant

Auto-descoberta automática via MQTT Discovery. Configure o mesmo broker MQTT no Home Assistant e todas as entidades aparecerão automaticamente:

- **9 Switches:** Interruptor 1-8 + Todos os Interruptores
- **9 Sensores:** Temperatura, Umidade, Sensação Térmica, Ponto de Orvalho, Temp BMP180, Pressão (x2), Altitude (x2)

---

## 🔐 Pastas e Arquivos Não Versionados

Para segurança e organização, estas pastas **NÃO** devem ser commitadas:

- `.agents/`, `.gemini/`, `.opencode/`, `graphify-out/`
- `não usar como parte do código/`

---

*Desenvolvido por **Robson Brasil** — Atualizado em Maio de 2026*
