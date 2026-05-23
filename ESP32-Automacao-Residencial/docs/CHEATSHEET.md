# ⚡ Cheatsheet - Referência Rápida

## 🔧 Configuração de Credenciais

Edite os seguintes arquivos para configurar o projeto:

### Arquivo .ino principal
```cpp
// Telegram (hardcoded)
const char *BOT_TOKEN = "SEU_TOKEN";
const char *CHAT_ID = "SEU_CHAT_ID";

// SinricPro (hardcoded)
const char *SINRICPRO_APP_KEY = "YOUR-APP-KEY";
const char *SINRICPRO_APP_SECRET = "YOUR-APP-SECRET";
const char *SINRICPRO_DEVICE_BANCADA = "device-id-bancada";
const char *SINRICPRO_DEVICE_REFLETOR = "device-id-refletor";
const char *SINRICPRO_DEVICE_TEMP_SENSOR = "device-id-temp";

// Constantes de referência BMP180
const float pressaoNivelMar = 1012.0;
const float altitudeNivelMar = 92.0;
```

### WiFiManager Portal (configuração via browser)
Ao abrir o portal WiFiManager, configure:
- SSID e Senha WiFi
- **IP do Broker MQTT**
- **Porta MQTT**
- **Usuário MQTT**
- **Senha MQTT**
- **Client ID MQTT**

> As configurações MQTT são salvas no NVS e persistem entre reinicializações.

---

## 🔌 Pinagem dos GPIOs

| GPIO | Função | Nome do Relé |
|------|--------|-------------|
| 0 | Botão BOOT (WiFiManager) | — |
| 2 | LED WiFi | — |
| 13 | Relé 2 | Bancada (SinricPro) |
| 14 | Relé 3 | Sala |
| 16 | Relé 5 | Quintal |
| 17 | Relé 6 | Varão (Val) |
| 18 | Relé 7 | Robson |
| 19 | Relé 8 | Kinha (SinricPro) |
| 21 | I2C SDA (BMP180) | — |
| 22 | I2C SCL (BMP180) | — |
| 25 | DHT22 | — |
| 26 | Relé 1 | Varanda |
| 32 | Relé 4 | Cozinha |

---

## 🌐 Endereços de Acesso

| Serviço | Endereço |
|---------|----------|
| Dashboard Web | `http://192.168.1.X:8181` |
| Dashboard (mDNS) | `http://automacaoresidencial.local:8181` |
| OTA | Porta 3232 (IDE Arduino) |
| Telnet (Debug) | Porta 2323 |

---

## 📡 Endpoints da API REST

| Endpoint | Descrição |
|----------|------------|
| `GET /` | Dashboard HTML |
| `GET /api/relays` | Lista todos os relés (estados + nomes) |
| `GET /api/sensors` | Lista todos os sensores |
| `GET /api/relay/0/1` | Ligar relé 1 (Varanda) |
| `GET /api/relay/0/0` | Desligar relé 1 (Varanda) |
| `GET /api/relay/7/1` | Ligar relé 8 (Kinha) |
| `GET /api/all/1` | Ligar todos |
| `GET /api/all/0` | Desligar todos |
| `GET /api/setname/0/Cozinha` | Nomear relé 1 |

**IDs dos relés na API:** 0=Varanda, 1=Bancada, 2=Sala, 3=Cozinha, 4=Quintal, 5=Varão, 6=Robson, 7=Kinha

---

## 📶 Comandos MQTT

### Ligar/Desligar Relé Individual
```bash
# Relé 1 (Varanda)
mosquitto_pub -t "ESP32/Interruptor1/Estado" -m "1"
mosquitto_pub -t "ESP32/Interruptor1/Estado" -m "0"

# Relé 2 a 8 (alterar número)
mosquitto_pub -t "ESP32/Interruptor2/Estado" -m "1"
```

### Ligar/Desligar Todos
```bash
mosquitto_pub -t "ESP32/Ligar-DesligarTudo/Estado" -m "1"
mosquitto_pub -t "ESP32/Ligar-DesligarTudo/Estado" -m "0"
```

### Comandos Hermes (Texto Natural)
```bash
# Ligar individual
mosquitto_pub -t "ESP32/Hermes/Comando" -m "LIGAR:VARANDA"
mosquitto_pub -t "ESP32/Hermes/Comando" -m "LIGAR:BANCADA"
mosquitto_pub -t "ESP32/Hermes/Comando" -m "LIGAR:SALA"
mosquitto_pub -t "ESP32/Hermes/Comando" -m "LIGAR:COZINHA"
mosquitto_pub -t "ESP32/Hermes/Comando" -m "LIGAR:QUINTAL"
mosquitto_pub -t "ESP32/Hermes/Comando" -m "LIGAR:VAL"
mosquitto_pub -t "ESP32/Hermes/Comando" -m "LIGAR:ROBSON"
mosquitto_pub -t "ESP32/Hermes/Comando" -m "LIGAR:KINHA"

# Desligar individual
mosquitto_pub -t "ESP32/Hermes/Comando" -m "DESLIGAR:SALA"

# Ligar/Desligar tudo
mosquitto_pub -t "ESP32/Hermes/Comando" -m "LIGAR:TUDO"
mosquitto_pub -t "ESP32/Hermes/Comando" -m "DESLIGAR:TUDO"

# Status e sensores
mosquitto_pub -t "ESP32/Hermes/Comando" -m "STATUS"
mosquitto_pub -t "ESP32/Hermes/Comando" -m "SENSORES"
```

> Comandos Hermes são normalizados para maiúsculas. Aceita `VAL` ou `VARÃO` para o relé 6.

---

## 📊 Comandos Telegram

| Comando | Ação |
|---------|------|
| `/start` | Menu principal |
| `/on` | Ligar todos os relés |
| `/off` | Desligar todos os relés |
| `/sensores` | Ver leituras dos sensores |
| `Voltar` | Retornar ao menu |
| Botões dos relés | Toggle individual (LIGAR/DESLIGAR) |

---

## 🏠 Home Assistant Discovery

Auto-descoberta via MQTT. Basta configurar o MQTT Broker no Home Assistant com o mesmo broker do ESP32.

**Prefixo de descoberta:** `homeassistant`
**Device ID:** `esp32_quarto_robson_v2`

Entidades descobertas automaticamente:
- 9 switches (8 relés + 1 "Todos")
- 9 sensores (temp, umidade, sensação térmica, orvalho, BMP temp, pressão x2, altitude x2)

---

## ⏱️ Intervalos de Execução

| Task | Intervalo |
|------|-----------|
| WiFi Check | 1000 ms |
| LED Blink | 500 ms |
| Telegram polling | 1000 ms |
| Reconnect MQTT | 5000 ms |
| Leitura DHT22 | 60000 ms (1 min) |
| Leitura BMP180 | 120000 ms (2 min) |
| Sync relés MQTT | 60000 ms |
| SinricPro temp event | 60000 ms |
| Auto Restart | 120 minutos |

---

## 🔐 Credenciais Padrão

| Serviço | Valor |
|---------|-------|
| OTA Senha | `S3nh@S3gur@` |
| OTA Porta | `3232` |
| Telnet Porta | `2323` |
| mDNS | `automacaoresidencial.local` |
| Dashboard | Porta `8181` |
| WiFiManager Portal Timeout | 180s |
| WiFiManager Connect Timeout | 30s |

---

## 📋 Mapeamento Relé ↔ Nome ↔ Hermes

| # | GPIO | Nome | Hermes | Telegram |
|---|------|------|--------|----------|
| 1 | 26 | Varanda | `VARANDA` | `Varanda` |
| 2 | 13 | Bancada | `BANCADA` | `Bancada` |
| 3 | 14 | Sala | `SALA` | `Sala` |
| 4 | 32 | Cozinha | `COZINHA` | `Cozinha` |
| 5 | 16 | Quintal | `QUINTAL` | `Quintal` |
| 6 | 17 | Varão | `VAL` / `VARÃO` | `Val` |
| 7 | 18 | Robson | `ROBSON` | `Robson` |
| 8 | 19 | Kinha | `KINHA` | `Kinha` |

---

## 🔧 Troubleshooting

| Problema | Solução |
|----------|---------|
| Não conecta WiFi | Verificar SSID/senha ou usar portal WiFiManager |
| OTA não aparece | Verificar se hostname está configurado corretamente |
| MQTT não conecta | Verificar IP do broker e credenciais no portal WiFiManager |
| Sensores não funcionam | Verificar conexões físicas DHT22/BMP180 (GPIOs 21/22/25) |
| Portal WiFiManager não abre | Pressionar botão BOOT por 3 segundos |
| Telnet não conecta | Verificar se WiFi está conectado, conectar na porta 2323 |
| Home Assistant não descobre | Verificar se MQTT broker está configurado no HA |
| Telegram não responde | Verificar BOT_TOKEN e CHAT_ID no .ino |

---

## 📋 Atalhos Úteis

```bash
# Verificar IP atribuído (Serial Monitor)
# Procure: [WM] IP: 192.168.1.XXX

# Acessar via mDNS
# http://automacaoresidencial.local:8181

# Atualizar OTA via Arduino IDE
# Ferramentas > Porta > 192.168.1.XXX (OTA)

# Conectar via Telnet para debug remoto
telnet <IP_DO_ESP32> 2323

# Subscrever todos os tópicos do ESP32
mosquitto_sub -t "ESP32/#" -v
```

---

*Última atualização: Maio de 2026*
