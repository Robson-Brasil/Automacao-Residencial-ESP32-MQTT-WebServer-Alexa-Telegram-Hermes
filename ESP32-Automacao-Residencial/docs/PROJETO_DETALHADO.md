# 🏠 Projeto Detalhado - Smart Home Hub ESP32

## 1. Visão Geral do Projeto

Firmware industrial robusto para ESP32 WROOM-32, projetado para controlar **8 zonas de carga (relés)** e monitorar o ambiente com sensores de alta precisão. Contempla integração nativa com os principais assistentes de voz e plataformas IoT.

| Informação | Valor |
|------------|-------|
| **Autor** | Robson Brasil |
| **Data** | Maio/2026 |
| **Microcontrolador** | ESP32 WROOM-32 (38 pinos) |
| **Hostname** | automacaoresidencial |
| **Versão Firmware** | 1.0.0 |

---

## 2. Hardware e Pinagem

### 2.1 Mapeamento de GPIOs

| GPIO | Função | Descrição |
|------|--------|------------|
| GPIO 0 | Botão Config | Botão "BOOT" para forçar portal WiFiManager (pressionar 3s) |
| GPIO 2 | LED WiFi | LED indicador de status de conexão WiFi |
| GPIO 13 | Relé 2 - Bancada | Relé 2 (active-low) — Controlável via SinricPro/Alexa |
| GPIO 14 | Relé 3 - Sala | Relé 3 (active-low) |
| GPIO 16 | Relé 5 - Quintal | Relé 5 (active-low) |
| GPIO 17 | Relé 6 - Varão | Relé 6 (active-low) |
| GPIO 18 | Relé 7 - Robson | Relé 7 (active-low) |
| GPIO 19 | Relé 8 - Kinha | Relé 8 (active-low) — Controlável via SinricPro/Alexa |
| GPIO 21 | I2C SDA | Dados I2C para BMP180 |
| GPIO 22 | I2C SCL | Clock I2C para BMP180 |
| GPIO 25 | DHT22 | Sensor de temperatura e umidade |
| GPIO 26 | Relé 1 - Varanda | Relé 1 (active-low) |
| GPIO 32 | Relé 4 - Cozinha | Relé 4 (active-low) |

### 2.2 Nomes dos Relés

| Relé | GPIO | Nome | Integrado SinricPro |
|------|------|------|---------------------|
| 1 | 26 | Varanda | ❌ |
| 2 | 13 | Bancada | ✅ |
| 3 | 14 | Sala | ❌ |
| 4 | 32 | Cozinha | ❌ |
| 5 | 16 | Quintal | ❌ |
| 6 | 17 | Varão (Val) | ❌ |
| 7 | 18 | Robson | ❌ |
| 8 | 19 | Kinha | ✅ |

### 2.3 Sensores Suportados

| Sensor | GPIO | Protocolo | Dados |
|--------|------|------------|-------|
| **DHT22** | GPIO 25 | 1-Wire | Temperatura, Umidade, Sensação Térmica, Ponto de Orvalho |
| **BMP180** | GPIO 21/22 | I2C | Temperatura, Pressão Atmosférica, Altitude |

### 2.4 Módulo Relé

- **Tipo**: Módulo 8 canais 5V (Active-Low)
- **Conexão**: Relés ativam com nível lógico LOW (0V)
- **Cuidados**: Usar fonte externa 5V adequada para os relés

---

## 3. Arquitetura Multi-Core

O firmware utiliza **FreeRTOS** com processamento em dois núcleos:

### 3.1 Core 0 - TaskConexoes (WiFi/MQTT/Rede)

Responsável por:
- Gerenciamento de conexão WiFi
- Comunicação MQTT (publicação e subscrição)
- Bot Telegram (interativo com teclado dinâmico)
- Atualização Remota (OTA)
- mDNS
- SinricPro (Alexa/Google Home)
- Home Assistant Discovery (auto-descoberta MQTT)
- Servidor Telnet (Remote Debug porta 2323)
- Auto Restart (a cada 120 minutos)

### 3.2 Core 1 - TaskSensores (Sensores/Web/Relés)

Responsável por:
- Leitura de sensores DHT22 e BMP180
- Lógica de controle dos relés
- Servidor Web (Dashboard + API REST)
- LED de status

### 3.3 Comunicação Inter-Core (IPC)

| Mecanismo | Tipo | Descrição |
|-----------|------|-----------|
| `telegramQueue` | FreeRTOS Queue | Fila thread-safe para notificações do Telegram (struct `TelegramNotification`) |
| `pendingMqttUpdate` | Flag bool | Sinaliza Core 0 para publicar estados dos relés |
| `pendingSensorMqttUpdate` | Flag bool | Sinaliza Core 0 para publicar dados dos sensores |
| `pendingSinricProUpdate` | Flag bool | Sinaliza Core 0 para sincronizar com SinricPro |
| `pendingSensorSinricUpdate` | Flag bool | Sinaliza Core 0 para enviar evento de temperatura ao SinricPro |
| `pendingHADiscovery` | Flag bool | Sinaliza Core 0 para publicar auto-descoberta Home Assistant |

### 3.4 Task Handles

```cpp
TaskHandle_t hTaskConexoes = NULL;  // Core 0 - Rede e protocolos
TaskHandle_t hTaskSensores = NULL;  // Core 1 - Sensores e Web
TaskHandle_t hTaskTelegram = NULL;  // Reservado
```

---

## 4. Conectividade e Rede

### 4.1 WiFiManager

O sistema utiliza o **WiFiManager** para configuração inicial e reconexão:
- Portal de configuração Web acessado automaticamente quando não há WiFi salvo
- Portal pode ser forçado pressionando o botão BOOT (GPIO0) por 3 segundos
- Credenciais WiFi armazenadas na memória flash do ESP32
- **Portal inclui campos configuráveis para MQTT**: Broker IP, Porta, Usuário, Senha, Client ID
- Timeout do portal: 180 segundos
- Timeout de conexão: 30 segundos

### 4.2 Persistência MQTT via NVS

As configurações MQTT são salvas no NVS (Preferences) no namespace `mqtt_config`:
- `broker` — IP/hostname do broker
- `port` — Porta do broker
- `user` — Usuário MQTT
- `pass` — Senha MQTT
- `client` — Client ID

### 4.3 mDNS

- **Hostname**: `automacaoresidencial`
- **Acesso**: `http://automacaoresidencial.local:8181`
- **Serviço**: HTTP na porta 8181

### 4.4 Auto-Reconexão

Sistema inteligente de reconexão:
- Verificação periódica do status WiFi
- Reconexão automática em caso de perda
- Reset de serviços (mDNS/OTA) após reconexão
- Reconfiguração automática do MQTT após reconexão

### 4.5 Auto Restart

- O sistema reinicia automaticamente a cada **120 minutos** para garantir estabilidade
- Previne memory leaks e degradação de performance em operação contínua

---

## 5. Sensores

### 5.1 DHT22 (Temperatura e Umidade)

| Dado | Unidade | Intervalo | Tópico MQTT |
|------|---------|-----------|-------------|
| Temperatura | °C | 60s | `.../Temperatura` |
| Umidade | % | 60s | `.../Umidade` |
| Sensação Térmica | °C | 60s | `.../SensacaoTermica` |
| Ponto de Orvalho | °C | 60s | `.../PontoOrvalho` |

**Fórmula do Ponto de Orvalho (Dew Point):**
```
a = 17.27, b = 237.7
gamma = (a * temp) / (b + temp) + log(hum / 100.0)
dewpoint = (b * gamma) / (a - gamma)
```

### 5.2 BMP180 (Pressão Atmosférica)

| Dado | Unidade | Intervalo | Tópico MQTT |
|------|---------|-----------|-------------|
| Temperatura | °C | 120s | `.../BMP180/Temperatura` |
| Pressão Real | hPa | 120s | `.../BMP180/PressaoAtmosferica/Real` |
| Pressão Nível Mar | hPa | 120s | `.../BMP180/PressaoAtmosferica/NivelMar` |
| Altitude Real | m | 120s | `.../BMP180/AltitudeReal` |
| Altitude Total | m | 120s | `.../BMP180/AltitudeNivelMar` |

**Constantes de referência:**
- Pressão nível do mar: 1012.0 hPa
- Altitude de referência: 92.0 m

---

## 6. Relés e Atuação

### 6.1 Controle

- **8 relés** controláveis individualmente
- **Comando "Ligar Tudo"** — liga todos os relés
- **Comando "Desligar Tudo"** — desliga todos os relés

### 6.2 Métodos de Controle

| Método | Descrição |
|--------|------------|
| MQTT | Publicar em tópico de estado |
| Telegram | Teclado interativo com botões dinâmicos |
| Dashboard Web | Interface visual moderna (glassmorphism) |
| Alexa/Google Home | Comando de voz (via SinricPro — Relés 2 e 8) |
| API REST | Endpoints HTTP |
| Hermes | Comandos em texto natural via MQTT |
| Home Assistant | Auto-descoberta via MQTT Discovery |

### 6.3 Persistência

Estados e nomes personalizados dos relés são salvos em **NVS (Preferences)**:
- Persistem após reinicialização
- Nomes customizáveis via API ou Dashboard
- Namespace: `relay states`
- Chaves: `RelayState1` a `RelayState8`, `RelayName1` a `RelayName8`

---

## 7. Integrações

### 7.1 MQTT

| Parâmetro | Valor |
|-----------|-------|
| QoS Relés | 2 (configurado, PubSubClient suporta QoS 0/1) |
| QoS Sensores | 0 |
| Retain | Ligado para estados de relés |
| KeepAlive | 30 segundos |
| Buffer Size | 2048 bytes |

**Tópicos de Publicação (ESP32 → Broker):**
```
ESP32/{BaseTopic}/Temperatura
ESP32/{BaseTopic}/Umidade
ESP32/{BaseTopic}/SensacaoTermica
ESP32/{BaseTopic}/PontoOrvalho
ESP32/{BaseTopic}/BMP180/Temperatura
ESP32/{BaseTopic}/BMP180/PressaoAtmosferica/Real
ESP32/{BaseTopic}/BMP180/PressaoAtmosferica/NivelMar
ESP32/{BaseTopic}/BMP180/AltitudeReal
ESP32/{BaseTopic}/BMP180/AltitudeNivelMar
ESP32/{BaseTopic}/Interruptor[1-8]/Comando
ESP32/{BaseTopic}/Ligar-DesligarTudo/Comando
```

**Tópicos de Assinatura (Broker → ESP32):**
```
ESP32/{BaseTopic}/Interruptor[1-8]/Estado
ESP32/{BaseTopic}/Ligar-DesligarTudo/Estado
ESP32/{BaseTopic}/Hermes/Comando          ← NOVO: Comandos Hermes
```

### 7.2 Hermes (Comandos em Texto Natural)

Sistema de controle via comandos em texto natural publicados no tópico `Hermes/Comando`.

**Formato:** `COMANDO:ITEM` (normalizado para maiúsculas)

| Comando | Ação | Exemplo |
|---------|------|---------|
| `LIGAR:TUDO` | Liga todos os relés | `LIGAR:TUDO` |
| `DESLIGAR:TUDO` | Desliga todos os relés | `DESLIGAR:TUDO` |
| `LIGAR:<ITEM>` | Liga relé específico | `LIGAR:VARANDA` |
| `DESLIGAR:<ITEM>` | Desliga relé específico | `DESLIGAR:SALA` |
| `STATUS` / `ESTADO` | Solicita status atual | `STATUS` |
| `SENSORES` / `SENSOR` | Força leitura dos sensores | `SENSORES` |

**Itens disponíveis:**

| Item | Relé | GPIO |
|------|------|------|
| VARANDA | 1 | 26 |
| BANCADA | 2 | 13 |
| SALA | 3 | 14 |
| COZINHA | 4 | 32 |
| QUINTAL | 5 | 16 |
| VAL / VARÃO | 6 | 17 |
| ROBSON | 7 | 18 |
| KINHA | 8 | 19 |
| TUDO | Todos | — |

### 7.3 Telegram Bot

Bot interativo com:
- **Teclado dinâmico** com botões para cada relé (mostra estado atual: LIGAR/DESLIGAR)
- **Status em tempo real** dos sensores
- **Comandos**: `/start`, `/sensores`, `/on`, `/off`, `Voltar`
- **Mensagem de boot** — informa quais relés já iniciaram ligados
- **Autorização** — apenas o CHAT_ID configurado pode controlar
- **Notificações push** — recebe alertas de mudanças de estado (MQTT, Dashboard, Alexa, Hermes)
- **Nomes dos relés**: Varanda, Bancada, Sala, Cozinha, Quintal, Val, Robson, Kinha

**Layout do teclado:**
```
[Varanda ⚡/⭕]     [Quintal ⚡/⭕]
[Bancada ⚡/⭕]     [Val ⚡/⭕]
[Sala ⚡/⭕]        [Robson ⚡/⭕]
[Cozinha ⚡/⭕]     [Kinha ⚡/⭕]
[💡 LIGAR TUDO]    [🔌 DESLIGAR TUDO]
[📊 VER SENSORES]
```

### 7.4 SinricPro (Alexa/Google Home)

Integração com 3 dispositivos:
- **Bancada** (Switch) — RelayPin2 (GPIO 13)
- **Refletor** (Switch) — RelayPin8 (GPIO 19)
- **Sensor de Temperatura** (DHT22) — GPIO 25

**Sincronização:**
- Rate limiting: 60s entre eventos de temperatura
- Só envia se variação > 0.5°C ou > 1% umidade
- Anti-loop: `sinricProOriginatedChange` evita eco de comandos
- Switch events enviados quando relés mudam por outras fontes

### 7.5 Home Assistant Discovery

Auto-descoberta via MQTT Discovery (prefixo `homeassistant`):

**Dispositivos registrados:**
- Device ID: `esp32_quarto_robson_v2`
- Nome: `ESP32 Quarto Robson`
- Fabricante: `Robson Brasil`
- Modelo: `ESP32 WROOM-32`

**Entidades auto-descobertas:**
| Tipo | Entidade | Device Class |
|------|----------|-------------|
| Switch | Todos os Interruptores | — |
| Switch | Interruptor 1-8 | — |
| Sensor | Temperatura | temperature |
| Sensor | Umidade | humidity |
| Sensor | Sensação Térmica | temperature |
| Sensor | Ponto de Orvalho | temperature |
| Sensor | Temperatura BMP180 | temperature |
| Sensor | Pressão Atmosférica | pressure |
| Sensor | Pressão Nível do Mar | pressure |
| Sensor | Altitude Real | distance |
| Sensor | Altitude Nível do Mar | distance |

### 7.6 Dashboard Web

Interface responsiva moderna acessível em `http://automacaoresidencial.local:8181` ou `http://<IP>:8181`

**Características:**
- Design glassmorphism com tema escuro
- Fonts: Outfit + JetBrains Mono
- Atualização em tempo real via polling JSON
- Controle individual de cada relé
- Visualização de sensores
- Nomes customizáveis dos relés
- Badges de status (WiFi, MQTT)

---

## 8. Atualização OTA

| Configuração | Valor |
|--------------|-------|
| Porta | 3232 |
| Hostname | automacaoresidencial |
| Senha | S3nh@S3gur@ |
| Segurança | Protegida por senha |

**Processo:**
1. A IDE Arduino detecta o dispositivo pelo hostname
2. Durante upload, a task de sensores é suspensa (`vTaskSuspend`)
3. Progresso exibido no Serial a cada 500ms
4. Após atualização, o ESP32 reinicia automaticamente
5. Task de sensores é retomada no `onEnd` e `onError`

---

## 9. Remote Debug (Telnet)

Sistema de debug remoto via Telnet:

| Configuração | Valor |
|--------------|-------|
| Porta Telnet | 2323 |
| Classe | `DualSerialClass` |
| Funcionamento | Espelha Serial UART para WiFi |

**DualSerialClass:**
- Herda de `Print` para interceptar saídas Serial
- Envia dados simultaneamente para UART (USB) e Telnet (WiFi)
- Macro `#define Serial DualSerial` substitui todas as chamadas
- Permite debug remoto sem cabo USB conectado
- Aceita apenas 1 cliente simultâneo

---

## 10. Persistência de Dados

O sistema utiliza **NVS (Preferences)** para armazenar:

| Namespace | Chaves | Descrição |
|-----------|--------|-----------|
| `relay states` | `RelayState1`-`RelayState8` | Estados dos relés |
| `relay states` | `RelayName1`-`RelayName8` | Nomes customizados |
| `relay states` | `needsReset` | Flag de reset (primeiro boot) |
| `mqtt_config` | `broker`, `port`, `user`, `pass`, `client` | Configurações MQTT |
| WiFiManager | — | Credenciais WiFi (gerenciado pela lib) |

---

## 11. Tabela de Tópicos MQTT

### Publicação (ESP32 → Broker)

| Tópico | Payload | Descrição |
|--------|---------|------------|
| `.../Temperatura` | "24.50" | Temperatura DHT22 (°C) |
| `.../Umidade` | "65.20" | Umidade DHT22 (%) |
| `.../SensacaoTermica` | "25.10" | Heat Index DHT22 (°C) |
| `.../PontoOrvalho` | "18.30" | Dew Point DHT22 (°C) |
| `.../BMP180/Temperatura` | "24.00" | Temperatura BMP180 (°C) |
| `.../BMP180/PressaoAtmosferica/Real` | "1008.50" | Pressão real (hPa) |
| `.../BMP180/PressaoAtmosferica/NivelMar` | "1012.00" | Pressão nível mar (hPa) |
| `.../BMP180/AltitudeReal` | "32.10" | Altitude real (m) |
| `.../BMP180/AltitudeNivelMar` | "124.10" | Altitude total (m) |
| `.../Interruptor[1-8]/Comando` | "0" ou "1" | Estado atual do relé |
| `.../Ligar-DesligarTudo/Comando` | "0" ou "1" | Estado geral |

### Subscrição (Broker → ESP32)

| Tópico | Payload | Ação |
|--------|---------|------|
| `.../Interruptor[1-8]/Estado` | "1" | Ligar relé 1-8 |
| `.../Interruptor[1-8]/Estado` | "0" | Desligar relé 1-8 |
| `.../Ligar-DesligarTudo/Estado` | "1" | Ligar todos |
| `.../Ligar-DesligarTudo/Estado` | "0" | Desligar todos |
| `.../Hermes/Comando` | Texto natural | Comandos Hermes |

---

## 12. API REST do Dashboard

| Endpoint | Método | Descrição |
|----------|--------|------------|
| `/` | GET | Dashboard HTML |
| `/api/relays` | GET | Estados e nomes de todos os relés (JSON) |
| `/api/sensors` | GET | Leituras atuais de todos os sensores (JSON) |
| `/api/relay/{id}/{state}` | GET | Controla relé específico (id: 0-7, state: 0=off, 1=on) |
| `/api/all/{state}` | GET | Liga/desliga todos os relés (state: 0=off, 1=on) |
| `/api/setname/{id}/{name}` | GET | Define nome customizado para relé |

**Exemplo JSON `/api/relays`:**
```json
{
  "states": [1,0,1,0,0,1,0,0],
  "names": ["Varanda","Bancada","Sala","Cozinha","Quintal","Varão","Robson","Kinha"]
}
```

**Exemplo JSON `/api/sensors`:**
```json
{
  "temp": "24.50",
  "hum": "65.20",
  "press": "1008.5",
  "tempFeel": "25.10",
  "dew": "18.30",
  "bmpTemp": "24.00",
  "alt": "124.10",
  "mqtt": true
}
```

---

## 13. Estrutura de Arquivos

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
├── OTAConfig.h                                # Protótipo OTA
├── OTAConfig.cpp                              # Implementação OTA
├── RemoteDebug.h                              # DualSerial (Telnet mirror)
├── Sensores.h                                 # Protótipos de leitura
├── SinricProESP32.h                           # Alexa/Google Home
├── TaskCores.h                                # Tasks FreeRTOS
├── TelegramBotESP32.h                         # Bot interativo
├── TopicosMQTT.h                              # Tópicos MQTT (extern)
├── VariaveisGlobais.h                         # Estado global (extern)
├── VariaveisGlobais.cpp                       # Definições de variáveis
├── WebServerESP32.h                           # Dashboard + API REST
├── WiFiManagerConfig.h                        # WiFiManager + mDNS + NVS MQTT
├── WiFiUtils.h                                # Protótipos WiFi
├── dashboard.html                             # Interface web (standalone)
└── docs/                                      # Documentação
    ├── INDICE.md
    ├── PROJETO_DETALHADO.md
    ├── CHEATSHEET.md
    └── superpowers/                           # Planos e specs de desenvolvimento
```

---

## 14. Requisitos de Bibliotecas

| Biblioteca | Versão | Função |
|------------|--------|--------|
| PubSubClient | 2.8+ | Cliente MQTT |
| DHT sensor library | 1.4+ | Sensor DHT22 |
| Adafruit BMP085 | 1.2+ | Sensor BMP180 |
| Adafruit Unified Sensor | 1.0+ | Base para BMP180 |
| WiFiManager | 2.0+ | Portal de configuração WiFi |
| UniversalTelegramBot | 1.3+ | Bot Telegram |
| ArduinoOTA | Built-in | Atualização Remota |
| SinricPro | 2.9+ | Alexa/Google Home |
| ArduinoJson | 6.x+ | Parsing JSON |
| Preferences | Built-in | Persistência NVS |
| TimeLib | 1.0+ | Sincronização de tempo (NTP) |
| ESPmDNS | Built-in | mDNS/Bonjour |
| WebServer | Built-in | Servidor HTTP |
| WiFiClientSecure | Built-in | TLS para Telegram |

---

## 15. Intervalos de Execução

| Task | Intervalo | Descrição |
|------|-----------|-----------|
| WiFi Check | 1000ms | Verificação de conexão |
| LED Blink | 500ms | Piscar LED quando desconectado |
| MQTT Reconnect | 5000ms | Tentativa de reconexão MQTT |
| Telegram Polling | 1000ms | Verificação de mensagens |
| Leitura DHT22 | 60000ms | Temperatura e umidade |
| Leitura BMP180 | 120000ms | Pressão e altitude |
| Sync Relés MQTT | 60000ms | Publicação periódica de estados |
| SinricPro Temp Event | 60000ms | Evento de temperatura (com rate limit) |
| Auto Restart | 120 min | Reinicialização programada |
| Task Delay (ambas) | 10ms | Yield do FreeRTOS |

---

## 16. Fluxo de Sincronização Cross-Platform

Quando um relé muda de estado por qualquer fonte, a mudança é propagada para TODAS as plataformas:

```
[Fonte] → [ESP32] → [MQTT Broker] → [Home Assistant / Node-RED]
              ↓
         [SinricPro] → [Alexa / Google Home]
              ↓
         [Telegram] → [Notificação push]
              ↓
         [Dashboard] → [Atualização via polling]
```

**Fontes de entrada:** MQTT, Telegram, Dashboard Web, SinricPro (Alexa), Hermes

**Proteção anti-loop:** `sinricProOriginatedChange` impede que comandos do SinricPro gerem eventos de volta.

---

*Última atualização: Maio de 2026 - Robson Brasil*
