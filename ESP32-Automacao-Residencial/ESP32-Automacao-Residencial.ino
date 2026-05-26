/******************************************************************************************************************************************
 *  PROJETO: Smart Home Hub ESP32 - Automação Residencial de Alta Performance
 *  DESENVOLVEDOR: Robson Brasil
 *
 *  DESCRICAO TECNICA:
 *  Firmware industrial baseado em ESP32 WROOM-32 para controle de 8 zonas de
 * carga (Relés) e monitoramento ambiental avançado. Utiliza arquitetura
 * multi-core com FreeRTOS para processamento paralelo, garantindo latência zero
 * no acionamento de hardware.
 *
 *  ESTRUTURA MULTI-CORE (FreeRTOS):
 *  - Core 0 (TaskConexoes): WiFi Stack, Broker MQTT, Telegram Bot e Atualização
 * Remota (OTA).
 *  - Core 1 (TaskSensores): Varredura DHT22/BMP180, Lógica de Relés e Servidor
 * Web Local.
 *
 *  TECNOLOGIAS INTEGRADAS:
 *  - DASHBOARD: Interface Web responsiva integrada com atualização em tempo
 * real (JSON API).
 *  - MQTT: Protocolo IoT para integração com Node-RED, Home Assistant, Alexa e
 * Google Home.
 *  - TELEGRAM: Bot interativo com teclado dinâmico para controle remoto e
 * feedback de sensores.
 *  - PERSISTÊNCIA: Armazenamento de estado em memória não-volátil
 * (NVS/Preferences).
 *  - CONECTIVIDADE: IP Fixo, DNS customizado e sistema de auto-reconexão
 * inteligente.
 *
 *  DEPENDENCIAS DE HARDWARE/SOFTWARE:
 *  - Sensores: DHT22 (Temp/Hum) e BMP180 (Pressão/Altitude/Precisão).
 *  - Bibliotecas: PubSubClient, UniversalTelegramBot, Adafruit BMP085, DHT
 * Sensor Library.
 ******************************************************************************************************************************************/

// =============== BIBLIOTECAS ===============
#include <Preferences.h>

#include "Bibliotecas.h"
#include "ConfigMQTT.h"
#include "ConstantesTempo.h"
#include "DefinicoesProjeto.h"
#include "GPIOs.h"
#include "LoginsSenhas.h"
#include "MQTTParametros.h"
#include "Sensores.h"
#include "SinricProESP32.h"
#include "TaskCores.h"
#include "TelegramBotESP32.h"
#include "TopicosMQTT.h"
#include "VariaveisGlobais.h"
#include "WebServerESP32.h"
#include "WiFiUtils.h"
#include "HomeAssistantDiscovery.h"
#include "WiFiManagerConfig.h"

// Estados dos relés

// =============== INSTÂNCIAS DE OBJETOS ===============
// Instância do sensor DHT
DHT dht(DHTPIN, DHTTYPE);

// Instância do sensor BMP180
Adafruit_BMP085 bmp;
WiFiClient espClient;               // Cliente WiFi
PubSubClient mqttClient(espClient); // Cliente MQTT
Preferences preferences; // Memória não-volátil para persistência de estado

WiFiServer telnetServer(2323); // Servidor Telnet
WiFiClient telnetClient;     // Cliente Telnet
bool telnetAuthenticated = false;
DualSerialClass DualSerial;  // Espelho do Serial para WiFi

// Variáveis para controle de tempo de leitura dos sensores
// =============== VARIÁVEIS DE CONTROLE DE TEMPO ===============

// =============== VARIÁVEIS DE ESTADO ===============

// Tópicos MQTT definidos em TopicosMQTT.h

// Constantes para o sensor BMP180
const float pressaoNivelMar =
    1012.0; // Pressão ao nível do mar em sua localidad
const float altitudeNivelMar = 92.0; // Altitude de referência do seu local

void readSensors();
void publishRelayStates();
void logRelayAction(const char* source, int relayNum, bool state);

/**
 * @brief Atualiza o LED de status (GPIO 2) com base no WiFi e MQTT.
 * Aceso fixo = ambos conectados. Piscando = um ou ambos desconectados.
 */
void updateStatusLED() {
    static unsigned long lastLedBlink = 0;
    if (wifiConnected && mqttConnected) {
        digitalWrite(wifiLed, HIGH);
    } else {
        if (millis() - lastLedBlink >= WIFI_BLINK_INTERVAL) {
            lastLedBlink = millis();
            digitalWrite(wifiLed, !digitalRead(wifiLed));
        }
    }
}

void setupWiFi() {
  unsigned long currentMillis = millis();

  if (currentMillis - lastWifiRetryTime < WIFI_RETRY_INTERVAL) {
    return;
  }
  lastWifiRetryTime = currentMillis;

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("[Rede] Conectando ao WiFi...");

    IPAddress ip(STATIC_IP);
    IPAddress gateway(STATIC_GATEWAY);
    IPAddress subnet(STATIC_SUBNET);
    IPAddress dns(STATIC_DNS);
    WiFi.config(ip, gateway, subnet, dns);

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.println("[Rede] WiFi.begin() assíncrono iniciado.");
  }

  if (WiFi.status() == WL_CONNECTED && !wifiConnected) {
    wifiConnected = true;
  } else if (WiFi.status() != WL_CONNECTED) {
    wifiConnected = false;
  }
}

// Função para configurar o MQTT
void setupMQTT() {
  mqttClient.setServer(BrokerMQTT, MQTT_PORT);
  mqttClient.setCallback(mqttCallback);
  mqttClient.setKeepAlive(MQTT_KEEPALIVE);
  mqttClient.setBufferSize(2048);
}

// Função para reconectar ao MQTT
void reconnectMQTT() {
  if (!mqttClient.connected() && WiFi.status() == WL_CONNECTED) {
    Serial.println("Tentando conectar ao MQTT...");
    if (mqttClient.connect(clientID, LoginDoMQTT, SenhaMQTT)) {
      Serial.println("==============================");
      Serial.println("MQTT CONECTADO!");
      Serial.print("IP do ESP32: ");
      Serial.println(WiFi.localIP());
      Serial.println("==============================");
      mqttConnected = true;
      pendingHADiscovery = true;

      // Inscrição nos tópicos dos relés
      mqttClient.subscribe(sub0);
      mqttClient.subscribe(sub1);
      mqttClient.subscribe(sub2);
      mqttClient.subscribe(sub3);
      mqttClient.subscribe(sub4);
      mqttClient.subscribe(sub5);
      mqttClient.subscribe(sub6);
      mqttClient.subscribe(sub7);
      mqttClient.subscribe(sub8);
      mqttClient.subscribe(sub9); // Hermes commands
      // Publica estado inicial dos relés
      publishRelayStates();
    } else {
      Serial.print("Falha na conexão MQTT, rc=");
      Serial.println(mqttClient.state());
      mqttConnected = false;
    }
  }
}

const char* relayTopics[8] = {sub1, sub2, sub3, sub4, sub5, sub6, sub7, sub8};
const int relayPinNums[8] = {RelayPin1, RelayPin2, RelayPin3, RelayPin4, RelayPin5, RelayPin6, RelayPin7, RelayPin8};
bool* relayStatePtrs[8] = {&RelayState1, &RelayState2, &RelayState3, &RelayState4, &RelayState5, &RelayState6, &RelayState7, &RelayState8};

void setRelayByIndex(int idx, bool state, const char* source) {
  *relayStatePtrs[idx] = state;
  digitalWrite(relayPinNums[idx], !state);
  logRelayAction(source, idx, state);
  saveRelayState(idx + 1, state);
  notifyTelegramStateChange(source, idx, state);
}

void setAllRelays(bool state, const char* source) {
  Todos = state;
  for (int i = 0; i < 8; i++) {
    *relayStatePtrs[i] = state;
    digitalWrite(relayPinNums[i], !state);
    saveRelayState(i + 1, state);
  }
  logRelayAction(source, -1, state);
  notifyTelegramStateChange(source, -1, state);
}

int findRelayByName(const String& name) {
  for (int i = 0; i < 8; i++) {
    String stored = relayNames[i];
    stored.toUpperCase();
    if (name == stored) return i;
  }
  if (name == "VAL") return 5;
  return -1;
}

// Função de callback para mensagens MQTT recebidas
void mqttCallback(char *topic, byte *payload, unsigned int length) {
  if (length > 255) return;

  char messageBuf[256];
  memcpy(messageBuf, payload, length);
  messageBuf[length] = '\0';

  String messageTemp(messageBuf);
  messageTemp.trim();

  Serial.print("Mensagem recebida [");
  Serial.print(topic);
  Serial.print("] ");
  Serial.println(messageTemp);

  bool newState = messageTemp.startsWith("1");

  if (strcmp(topic, sub0) == 0) {
    if (Todos != newState) {
      Todos = newState;
      const int pins[] = {RelayPin1, RelayPin2, RelayPin3, RelayPin4,
                          RelayPin5, RelayPin6, RelayPin7, RelayPin8};
      for (int i = 0; i < 8; i++) digitalWrite(pins[i], !Todos);
      logRelayAction("MQTT", -1, Todos);
      RelayState1 = RelayState2 = RelayState3 = RelayState4 = RelayState5 =
          RelayState6 = RelayState7 = RelayState8 = Todos;
      for (int r = 1; r <= 8; r++) saveRelayState(r, Todos);
      notifyTelegramStateChange("MQTT", -1, Todos);
      pendingSinricProUpdate = true;
    }
    return;
  }

  for (int i = 0; i < 8; i++) {
    if (strcmp(topic, relayTopics[i]) == 0) {
      if (*relayStatePtrs[i] != newState) {
        *relayStatePtrs[i] = newState;
        digitalWrite(relayPinNums[i], !newState);
        logRelayAction("MQTT", i, newState);
        saveRelayState(i + 1, newState);
        notifyTelegramStateChange("MQTT", i, newState);
        pendingSinricProUpdate = true;
      }
      return;
    }
  }

  if (strcmp(topic, sub9) == 0) {
    // ========== HANDLE HERMES COMMANDS ==========
    // Formato: COMANDO:ITEM (ex: LIGAR:Varanda, DESLIGAR:Sala, SENSORES, STATUS)
    messageTemp.toUpperCase(); // normaliza pra maiúsculas
    String cmd = messageTemp;

    // SENSORES
    if (cmd == "SENSORES") {
      // Apenas publica o que já está em cache, sem forçar leitura
      Serial.println("✅ Hermes SENSORES");
      pendingSensorMqttUpdate = true;
    }
    // STATUS
    else if (cmd == "STATUS") {
      // Monta JSON com estado dos 8 relés + Toggle Geral
      char statusJson[512];
      snprintf(statusJson, sizeof(statusJson),
        "{\"estados\": {\"Varanda\": \"%s\", \"Bancada\": \"%s\", \"Sala\": \"%s\", \"Cozinha\": \"%s\", \"Quintal\": \"%s\", \"Val\": \"%s\", \"Robson\": \"%s\", \"Kinha\": \"%s\", \"TUDO\": \"%s\"}}",
        RelayState1 ? "ON" : "OFF", RelayState2 ? "ON" : "OFF", RelayState3 ? "ON" : "OFF", RelayState4 ? "ON" : "OFF",
        RelayState5 ? "ON" : "OFF", RelayState6 ? "ON" : "OFF", RelayState7 ? "ON" : "OFF", RelayState8 ? "ON" : "OFF",
        Todos ? "ON" : "OFF");
      mqttClient.publish(pub18, statusJson);
      Serial.println("✅ Hermes STATUS: " + String(statusJson));
    }
    // LIGAR individual ou TUDO
    
    // LIGAR TUDO
    if (cmd.startsWith("LIGAR") && cmd.indexOf("TUDO") != -1) {
      setAllRelays(true, "HERMES");
      pendingSinricProUpdate = true;
    }
    // DESLIGAR TUDO
    else if (cmd.startsWith("DESLIGAR") && cmd.indexOf("TUDO") != -1) {
      setAllRelays(false, "HERMES");
      pendingSinricProUpdate = true;
    }
    // LIGAR individual
    else if (cmd.startsWith("LIGAR:")) {
      String item = cmd.substring(6); item.trim();
      int idx = findRelayByName(item);
      if (idx >= 0) {
        setRelayByIndex(idx, true, "HERMES");
        pendingSinricProUpdate = true;
        Todos = true;
        for (int i = 0; i < 8; i++) if (!*relayStatePtrs[i]) { Todos = false; break; }
      }
    }
    // DESLIGAR individual
    else if (cmd.startsWith("DESLIGAR:")) {
      String item = cmd.substring(9); item.trim();
      int idx = findRelayByName(item);
      if (idx >= 0) {
        setRelayByIndex(idx, false, "Hermes");
        pendingSinricProUpdate = true;
        Todos = false;
        for (int i = 0; i < 8; i++) if (*relayStatePtrs[i]) { Todos = true; break; }
      }
    }
    pendingMqttUpdate = true;
  }
}

// Função para ler e publicar dados dos sensores
void readSensors() {
  unsigned long currentTimeDHT = millis();
  unsigned long currentTimeBMP180 = millis();

  // Leitura do DHT22 a cada 60 segundos
  if (currentTimeDHT - lastMsgDHT > DHT_READ_INTERVAL) {
    lastMsgDHT = currentTimeDHT;

    float temp_data = dht.readTemperature();
    float hum_data = dht.readHumidity();

    // Verifica se as leituras são válidas
    if (isnan(temp_data) || isnan(hum_data)) {
      Serial.println("Falha na leitura do sensor DHT22!");
      return;
    }

    // Processa dados de temperatura e umidade
    dtostrf(temp_data, 6, 2, str_temp_data);
    dtostrf(hum_data, 6, 2, str_hum_data);

    // Calcula Ponto de Orvalho (Dew Point)
    float a = 17.27;
    float b = 237.7;
    float gamma = (a * temp_data) / (b + temp_data) + log(hum_data / 100.0);
    float dewpoint = (b * gamma) / (a - gamma);
    dtostrf(dewpoint, 6, 2, str_dewpoint_data);

    // Processa temperatura em Fahrenheit e sensação térmica
    float tempF_data = dht.readTemperature(true);
    if (!isnan(tempF_data)) {
      dtostrf(tempF_data, 6, 2, str_tempF_data);

      float tempterm_data = dht.computeHeatIndex(tempF_data, hum_data);
      if (!isnan(tempterm_data)) {
        tempterm_data = dht.convertFtoC(tempterm_data);
        dtostrf(tempterm_data, 6, 2, str_tempterm_data);
      }
    }

    // Sinaliza para o Core 0 publicar os dados
    pendingSensorMqttUpdate = true;
    pendingSensorSinricUpdate = true;

    // Debug no serial
    Serial.println("\n=== Leitura DHT22 ===");
    Serial.print("Temperatura: ");
    Serial.print(str_temp_data);
    Serial.println(" °C");
    Serial.print("Umidade: ");
    Serial.print(str_hum_data);
    Serial.println(" %");
    Serial.print("Sensação Térmica: ");
    Serial.print(str_tempterm_data);
    Serial.println(" °C");
    Serial.print("Ponto de Orvalho: ");
    Serial.print(str_dewpoint_data);
    Serial.println(" °C");
  }

  // Leitura do BMP180 a cada 120 segundos
  if (currentTimeBMP180 - lastMsgBMP180 > BMP_READ_INTERVAL) {
    lastMsgBMP180 = currentTimeBMP180;

    pressure = bmp.readPressure();
    if (pressure != 0) {
      seaLevelPressureCache = bmp.readSealevelPressure(pressaoNivelMar);

      altitudeRealCache = bmp.readAltitude(pressaoNivelMar * 100);
      if (!isnan(altitudeRealCache)) {
        float tempBMP = bmp.readTemperature();
        temperature = tempBMP;

        altitude = altitudeRealCache + altitudeNivelMar;
        altitudeTotal = altitude;

        // Sinaliza para o Core 0 publicar os dados
        pendingSensorMqttUpdate = true;

        // Debug no serial
        Serial.println("\n=== Leitura BMP180 ===");
        Serial.print("Temperatura: ");
        Serial.print(temperature);
        Serial.println(" °C");
        Serial.print("Pressão: ");
        Serial.print(pressure / 100.0);
        Serial.println(" hPa");
        Serial.print("Pressão nível do mar: ");
        Serial.print(seaLevelPressureCache / 100.0);
        Serial.println(" hPa");
        Serial.print("Altitude real: ");
        Serial.print(altitudeRealCache);
        Serial.println(" m");
        Serial.print("Altitude total: ");
        Serial.print(altitude);
        Serial.println(" m");
      }
    } else {
      Serial.println("Erro na leitura do sensor BMP180");
    }
  }
}

// Função para publicar dados dos sensores
void publishSensorData() {
  // Publicar dados do DHT22
  mqttClient.publish(pub9, str_temp_data, true);
  mqttClient.publish(pub10, str_hum_data, true);
  mqttClient.publish(pub11, str_tempterm_data, true);

  // Publicar dados do BMP180
  char tempStr[10];
  dtostrf(temperature, 6, 2, tempStr);
  mqttClient.publish(pub12, tempStr, true); // Temperatura BMP180

  dtostrf(pressure / 100.0, 6, 2, tempStr);
  mqttClient.publish(pub13, tempStr, true); // Pressão real BMP180

  // Pressão ao nível do mar (cache, evita I2C duplicado)
  dtostrf(seaLevelPressureCache / 100.0, 6, 2, tempStr);
  mqttClient.publish(pub14, tempStr, true); // Pressão ao nível do mar BMP180

  // Altitude real (cache, evita I2C duplicado)
  dtostrf(altitudeRealCache, 6, 2, tempStr);
  mqttClient.publish(pub15, tempStr, true); // Altitude real BMP180

  // Altitude em relação ao nível do mar
  dtostrf(altitude, 6, 2, tempStr);
  mqttClient.publish(pub16, tempStr, true);
  mqttClient.publish(pub17, str_dewpoint_data, true);
}

// Função para publicar estado dos relés
void publishRelayStates() {
  mqttClient.publish(pub0, Todos ? "1" : "0", true);
  mqttClient.publish(pub1, RelayState1 ? "1" : "0", true);
  mqttClient.publish(pub2, RelayState2 ? "1" : "0", true);
  mqttClient.publish(pub3, RelayState3 ? "1" : "0", true);
  mqttClient.publish(pub4, RelayState4 ? "1" : "0", true);
  mqttClient.publish(pub5, RelayState5 ? "1" : "0", true);
  mqttClient.publish(pub6, RelayState6 ? "1" : "0", true);
  mqttClient.publish(pub7, RelayState7 ? "1" : "0", true);
  mqttClient.publish(pub8, RelayState8 ? "1" : "0", true);
}

// =============== FUNÇÕES DE PERSISTÊNCIA ===============
void saveRelayState(int relayNum, bool state) {
  if (relayMutex == NULL) return;
  String key = "RelayState" + String(relayNum);
  if (xSemaphoreTake(relayMutex, portMAX_DELAY) == pdTRUE) {
    preferences.putBool(key.c_str(), state);
    xSemaphoreGive(relayMutex);
  }
}

bool loadRelayState(int relayNum, bool defaultValue) {
  if (relayMutex == NULL) return defaultValue;
  String key = "RelayState" + String(relayNum);
  bool val = defaultValue;
  if (xSemaphoreTake(relayMutex, portMAX_DELAY) == pdTRUE) {
    val = preferences.getBool(key.c_str(), defaultValue);
    xSemaphoreGive(relayMutex);
  }
  return val;
}

void loadAllRelayStates() {
  RelayState1 = loadRelayState(1, false);
  RelayState2 = loadRelayState(2, false);
  RelayState3 = loadRelayState(3, false);
  RelayState4 = loadRelayState(4, false);
  RelayState5 = loadRelayState(5, false);
  RelayState6 = loadRelayState(6, false);
  RelayState7 = loadRelayState(7, false);
  RelayState8 = loadRelayState(8, false);
  Todos = RelayState1 && RelayState2 && RelayState3 && RelayState4 &&
          RelayState5 && RelayState6 && RelayState7 && RelayState8;
}

void applyRelayStatesToPins() {
  logRelayAction("Sistema", 0, RelayState1);
  logRelayAction("Sistema", 1, RelayState2);
  logRelayAction("Sistema", 2, RelayState3);
  logRelayAction("Sistema", 3, RelayState4);
  logRelayAction("Sistema", 4, RelayState5);
  logRelayAction("Sistema", 5, RelayState6);
  logRelayAction("Sistema", 6, RelayState7);
  logRelayAction("Sistema", 7, RelayState8);
  digitalWrite(RelayPin1, !RelayState1);
  digitalWrite(RelayPin2, !RelayState2);
  digitalWrite(RelayPin3, !RelayState3);
  digitalWrite(RelayPin4, !RelayState4);
  digitalWrite(RelayPin5, !RelayState5);
  digitalWrite(RelayPin6, !RelayState6);
  digitalWrite(RelayPin7, !RelayState7);
  digitalWrite(RelayPin8, !RelayState8);
}

// =============== FUNÇÃO DE LOG ===============
void logRelayAction(const char* source, int relayNum, bool state) {
  Serial.print('[');
  Serial.print(source);
  Serial.print("] ");
  if (relayNum >= 0 && relayNum < 8) {
    Serial.print(relayNames[relayNum]);
  } else {
    Serial.print("TODOS");
  }
  Serial.print(": ");
  Serial.println(state ? "LIGADO" : "DESLIGADO");
}

// =============== INÍCIO ===============
void setup() {
  // 1. Inicialização básica e hardware
  Serial.begin(115200);
  delay(100);
  Serial.println("\n[SISTEMA] " PROJETO_NOME " v" PROJETO_VERSAO);
  Serial.println("[SISTEMA] Autor: " PROJETO_AUTOR);
  Serial.println("[SISTEMA] Data: " PROJETO_DATA);

  migrateNVS();
  loadCredentials();

  // Relés começam como INPUT_PULLUP (sem driving ativo) durante boot
  pinMode(RelayPin1, INPUT_PULLUP);
  pinMode(RelayPin2, INPUT_PULLUP);
  pinMode(RelayPin3, INPUT_PULLUP);
  pinMode(RelayPin4, INPUT_PULLUP);
  pinMode(RelayPin5, INPUT_PULLUP);
  pinMode(RelayPin6, INPUT_PULLUP);
  pinMode(RelayPin7, INPUT_PULLUP);
  pinMode(RelayPin8, INPUT_PULLUP);
  pinMode(wifiLed, OUTPUT);
  pinMode(configButton, INPUT_PULLUP);

  // 2. Inicialização de Comunicação Inter-Core (IPC)
  // Criar a fila ANTES de qualquer serviço que possa usá-la
  telegramQueue = xQueueCreate(10, sizeof(TelegramNotification));
  relayMutex = xSemaphoreCreateMutex();

  // 3. Persistência e Estados Iniciais
  preferences.begin("app", false);
  bool needsReset = preferences.getBool("needsReset", true);
  if (needsReset) {
    for (int i = 1; i <= 8; i++) {
      String key = "RelayState" + String(i);
      preferences.remove(key.c_str());
    }
    preferences.putBool("needsReset", false);
  }
  // Só agora vira os relés pra OUTPUT — todo boot já passou
  const int relayPins[] = {RelayPin1, RelayPin2, RelayPin3, RelayPin4,
                           RelayPin5, RelayPin6, RelayPin7, RelayPin8};
  for (int i = 0; i < 8; i++) {
    digitalWrite(relayPins[i], HIGH);
    pinMode(relayPins[i], OUTPUT);
  }
  loadAllRelayStates();
  applyRelayStatesToPins();

  // 4. Inicialização do Stack de Rede (Apenas modo, sem conectar ainda)
  WiFi.mode(WIFI_STA);
  WiFi.setHostname(globalHostname);

  // 5. Inicialização de Sensores
  dht.begin();
  Wire.begin(I2C_SDA, I2C_SCL);
  if (!bmp.begin()) {
    Serial.println("[ERRO] BMP180 não encontrado!");
  }

  // 6. Inicialização de Servidores que rodam no Core 1 (loop principal)
  setupWebServer();

  // 7. Criação das Tasks (Multitarefa)
  xTaskCreatePinnedToCore(
      TaskConexoes,
      "TaskConexoes",
      8192,
      NULL, 1, &hTaskConexoes,
      0 // Core 0 (Protocolos de rede)
  );

  xTaskCreatePinnedToCore(
      TaskSensores,
      "TaskSensores",
      8192,
      NULL, 1, &hTaskSensores,
      1 // Core 1 (Lógica de sensores)
  );

  Serial.println("[SISTEMA] Setup Core 1 finalizado.");
}

// =============== EXECUÇÃO ===============
void loop() {
  handleWebServerClient();
  vTaskDelay(10 / portTICK_PERIOD_MS);
}

// Task para conexões (WiFi/MQTT/OTA/Telegram) - Core 0
void TaskConexoes(void *pvParameters) {
  (void)pvParameters;
  bool otaInitialized = false;
  bool servicesInitialized = false;

  // Inicializa serviços de rede específicos deste núcleo
  setupMQTT();
  setupSinricPro();
  telnetServer.begin();
  
  Serial.println("[CORE 0] Serviços de rede inicializados.");

  for (;;) {
    unsigned long currentMillis = millis();
    heartbeatConexoes++;

    // ── WiFi ──────────────────────────────────────────────────────────────
    if (currentMillis - lastWifiCheckTime >= WIFI_RETRY_INTERVAL) {
      lastWifiCheckTime = currentMillis;
      if (WiFi.status() != WL_CONNECTED) {
        if (wifiConnected) {
          Serial.println("[AVISO] WiFi perdido!");
          wifiConnected = false;
          otaInitialized = false;
        }
        setupWiFi();
      } else {
        if (!wifiConnected) wifiConnected = true; // Sincroniza flag
        
        if (!otaInitialized) {
          setupMDNS();
          setupOTA();
          otaInitialized = true;
          Serial.println("[Rede] mDNS e OTA ativos.");
        }
      }
    }

    // ── MQTT ──────────────────────────────────────────────────────────────
    if (WiFi.status() == WL_CONNECTED) {
      if (!mqttClient.connected()) {
        mqttConnected = false;
        if (currentMillis - lastMqttReconnectAttempt >= RECONNECT_INTERVAL) {
          lastMqttReconnectAttempt = currentMillis;
          reconnectMQTT();
        }
      }
      mqttClient.loop();
    }

    // ── Sinalizações entre Núcleos ────────────────────────────────────
    if (pendingMqttUpdate && mqttClient.connected()) {
      publishRelayStates();
      pendingMqttUpdate = false;
    }

    if (pendingSensorMqttUpdate && mqttClient.connected()) {
      publishSensorData();
      pendingSensorMqttUpdate = false;
    }

    if (pendingHADiscovery && mqttClient.connected()) {
      publishHADiscovery(mqttClient);
      pendingHADiscovery = false;
    }

    TelegramNotification notif;
    if (telegramQueue != NULL &&
        xQueueReceive(telegramQueue, &notif, 0) == pdPASS) {
      processPendingTelegramNotification(notif);
    }

    // ── Telegram / Sinric / OTA ───────────────────────────────────────
    telegramLoop();
    ArduinoOTA.handle();
    sinricProLoop();

    // Sincroniza estados dos relés com SinricPro
    if (pendingSinricProUpdate) {
      sendSinricProSwitchEvents();
      pendingSinricProUpdate = false;
    }

    if (pendingSensorSinricUpdate) {
      sendSinricProTemperatureEvent(String(str_temp_data).toFloat(),
                                    String(str_hum_data).toFloat());
      pendingSensorSinricUpdate = false;
    }

    // ── Servidor Telnet (Bridge WiFi <-> Serial) ─────────────────────
    if (telnetServer.hasClient()) {
      if (!telnetClient || !telnetClient.connected()) {
        if (telnetClient) telnetClient.stop();
        telnetClient = telnetServer.available();
        if (telnetClient && telnetClient.connected()) {
          telnetClient.print("ESP32 Automação Residencial\r\nPassword: ");
          telnetAuthenticated = false;
        }
      } else {
        WiFiClient rejectedClient = telnetServer.available();
        if (rejectedClient) rejectedClient.stop();
      }
    }

    if (telnetClient && telnetClient.connected()) {
      if (!telnetAuthenticated) {
        static String telnetInput;
        while (telnetClient.available()) {
          int b = telnetClient.read();
          if (b < 0) break;
          unsigned char c = (unsigned char)b;
          if (c >= 0xF0) continue;
          if (c == '\r' || c == '\n') {
            if (telnetInput.length() > 0) {
              if (telnetInput == TELNET_PASSWORD) {
                telnetClient.print("\r\nOK\r\n");
                telnetAuthenticated = true;
              } else {
                telnetClient.print("\r\nERROR\r\n");
                telnetClient.stop();
              }
              telnetInput = "";
            }
          } else if (c >= 32 && c < 127) {
            if (telnetInput.length() < 64) telnetInput += (char)c;
          }
        }
      } else {
        while (telnetClient.available()) {
          char c = telnetClient.read();
          Serial.write(c);
        }
      }
    }

    // ── LED de Status ─────────────────────────────────────────────────
    updateStatusLED();

    // ── Watchdog ─────────────────────────────────────────────────────
    static uint32_t lastHbConexoes = 0;
    static uint32_t lastHbSensores = 0;
    static unsigned long lastConexoesChange = 0;
    static unsigned long lastSensoresChange = 0;

    uint32_t hbC = heartbeatConexoes;
    if (hbC != lastHbConexoes) {
      lastHbConexoes = hbC;
      lastConexoesChange = currentMillis;
    }
    uint32_t hbS = heartbeatSensores;
    if (hbS != lastHbSensores) {
      lastHbSensores = hbS;
      lastSensoresChange = currentMillis;
    }

    if (currentMillis - lastConexoesChange >= 30000) {
      Serial.println("[WATCHDOG] TaskConexoes congelada! Reiniciando...");
      delay(100);
      ESP.restart();
    }
    if (currentMillis - lastSensoresChange >= 60000) {
      Serial.println("[WATCHDOG] TaskSensores congelada! Reiniciando...");
      delay(100);
      ESP.restart();
    }

    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

// Task para sensores e relés - Core 1
void TaskSensores(void *pvParameters) {
  (void)pvParameters;
  for (;;) {
    heartbeatSensores++;
    // Lê os sensores (a função readSensors já tem controle de tempo interno)
    readSensors();

    // Atualização periódica de sincronia (apenas se necessário e via Core 0)
    unsigned long currentMillis = millis();
    if (currentMillis - lastRelayUpdate >=
        60000) // Aumentado para 60s para evitar overhead
    {
      lastRelayUpdate = currentMillis;
      pendingMqttUpdate = true; // Solicita ao Core 0 que envie os estados
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}
