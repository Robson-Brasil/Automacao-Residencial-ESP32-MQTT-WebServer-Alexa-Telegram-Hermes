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

// ========================================================================================
// MAPEAMENTO DE HARDWARE (GPIOs)
// ========================================================================================
// Nota: Os relés padrão de 5V costumam ativar em nível lógico BAIXO
// (active-low).
#define RelayPin1 26 // D26 Ligados ao Nora/MQTT
#define RelayPin2 13 // D13 Ligados ao SirincPro/Nora/MQTT
#define RelayPin3 14 // D14 Ligados ao Nora/MQTT
#define RelayPin4 32 // D32 Ligados ao Nora/MQTT
#define RelayPin5 16 // D16 Ligados ao Nora/MQTT
#define RelayPin6 17 // D17 Ligados ao MQTT
#define RelayPin7 18 // D18 Ligados ao MQTT
#define RelayPin8 19 // D19 Ligados ao SirincPro/MQTT

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
#include <WiFi.h>
#include "WiFiManagerConfig.h"


// LED indicador de WiFi
#define wifiLed 2 // D2

// Botão para forçar portal WiFiManager (GPIO0 = botão BOOT na maioria das placas)
#define configButton 0

// Sensor DHT
#define DHTPIN 25 // D25
#define DHTTYPE                                                                \
  DHT22 // DHT22 (AM2302) - Altere para DHT11 se estiver usando esse modelo

// Pinos I2C para BMP180
#define I2C_SDA 21 // Pino SDA
#define I2C_SCL 22 // Pino SCL

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
DualSerialClass DualSerial;  // Espelho do Serial para WiFi

// =============== VARIÁVEIS GLOBAIS ===============
// Variáveis para armazenar os dados dos sensores

// Variáveis para controle de tempo de leitura dos sensores
// =============== VARIÁVEIS DE CONTROLE DE TEMPO ===============

// =============== CONSTANTES DE TEMPO ===============
const unsigned long WIFI_RETRY_INTERVAL = 1000;
const unsigned long WIFI_BLINK_INTERVAL = 500;
const unsigned long RECONNECT_INTERVAL = 5000;
const unsigned long DHT_READ_INTERVAL = 60000;
const unsigned long BMP_READ_INTERVAL = 120000;
// =============== VARIÁVEIS DE ESTADO ===============

// Tópicos MQTT definidos em TopicosMQTT.h

// Constantes para o sensor BMP180
const float pressaoNivelMar =
    1012.0; // Pressão ao nível do mar em sua localidad
const float altitudeNivelMar = 92.0; // Altitude de referência do seu local

// Configurações do MQTT - definidas em LoginsSenhas.h (via WiFiManager)

// Configurações do Telegram
const char *BOT_TOKEN = "8444037202:AAGB4P8wjPtjiGzlBHInCYkLUqTa1OUzgHI";
const char *CHAT_ID = "5270818980";

// Configurações do SinricPro (Alexa / Google Home)
const char *SINRICPRO_APP_KEY = "4914a0d0-327e-4815-8128-822b7a80713d";
const char *SINRICPRO_APP_SECRET =
    "b73f409a-fcc5-4c60-8cf4-d86d9b4e2bae-e7fc2fe6-1b4a-4b71-aa38-de61ad019107";
const char *SINRICPRO_DEVICE_BANCADA =
    "69fd1b19baa50bf9bf2e4b8e"; // Bancada - RelayPin2 GPIO 13
const char *SINRICPRO_DEVICE_REFLETOR =
    "69fd1bc0977a0619a739f409"; // Refletor - RelayPin8 GPIO 19
const char *SINRICPRO_DEVICE_TEMP_SENSOR =
    "69fd1c4dbaa50bf9bf2e4c2a"; // Sensor de Temperatura DHT22

/**
 * @brief Tenta conectar ao WiFi se a conexão for perdida.
 * Utiliza IP fixo se configurado para evitar esperas de DHCP.
 */
void setupWiFi();

/**
 * @brief Configura o servidor e os callbacks do cliente MQTT.
 */
void setupMQTT();

/**
 * @brief Mantém a conexão com o Broker MQTT ativa.
 */
void reconnectMQTT();

/**
 * @brief Realiza a leitura física dos sensores (DHT22 e BMP180).
 */
void readSensors();

/**
 * @brief Publica todos os estados dos relés via MQTT (Sync).
 */
void publishRelayStates();

/**
 * @brief Notifica o Telegram sobre mudança de estado (origem externa).
 * @param source Origem (MQTT, Dashboard, Telegram)
 * @param relayNum Número do relé (-1 para todos)
 * @param relayState Estado do relé (true/false)
 */
void notifyTelegramStateChange(const String &source, int relayNum,
                                bool relayState);

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

// Função para configurar o WiFi
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

  // Controle do LED unificado — tratado por updateStatusLED()

  // Verifica status da conexão para logs
  if (WiFi.status() == WL_CONNECTED && !wifiConnected) {
    wifiConnected = true;
    Serial.println("\n==============================");
    Serial.println("WiFi CONECTADO!");
    Serial.print("IP: "); Serial.println(WiFi.localIP());
    Serial.println("==============================");
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

// Função de callback para mensagens MQTT recebidas
void mqttCallback(char *topic, byte *payload, unsigned int length) {
  String messageTemp;

  for (unsigned int i = 0; i < length; i++) {
    messageTemp += (char)payload[i];
  }

  messageTemp.trim();

  Serial.print("Mensagem recebida [");
  Serial.print(topic);
  Serial.print("] ");
  Serial.println(messageTemp);
  // Controle dos relés
  bool newState = messageTemp.startsWith("1");

  if (strcmp(topic, sub0) == 0) {
    if (Todos != newState) {
      Todos = newState;

      const int pins[] = {RelayPin1, RelayPin2, RelayPin3, RelayPin4,
                          RelayPin5, RelayPin6, RelayPin7, RelayPin8};
      for (int i = 0; i < 8; i++) {
        digitalWrite(pins[i], !Todos);
      }

      RelayState1 = RelayState2 = RelayState3 = RelayState4 = RelayState5 =
          RelayState6 = RelayState7 = RelayState8 = Todos;

      for (int r = 1; r <= 8; r++)
        saveRelayState(r, Todos);
      notifyTelegramStateChange("MQTT", -1, Todos);
      pendingSinricProUpdate = true;
    }
  } else if (strcmp(topic, sub1) == 0) {
    if (RelayState1 != newState) {
      RelayState1 = newState;
      digitalWrite(RelayPin1, !RelayState1);
      saveRelayState(1, RelayState1);
      notifyTelegramStateChange("MQTT", 0, newState);
      pendingSinricProUpdate = true;
    }
  } else if (strcmp(topic, sub2) == 0) {
    if (RelayState2 != newState) {
      RelayState2 = newState;
      digitalWrite(RelayPin2, !RelayState2);
      saveRelayState(2, RelayState2);
      notifyTelegramStateChange("MQTT", 1, newState);
      pendingSinricProUpdate = true;
    }
  } else if (strcmp(topic, sub3) == 0) {
    if (RelayState3 != newState) {
      RelayState3 = newState;
      digitalWrite(RelayPin3, !RelayState3);
      saveRelayState(3, RelayState3);
      notifyTelegramStateChange("MQTT", 2, newState);
      pendingSinricProUpdate = true;
    }
  } else if (strcmp(topic, sub4) == 0) {
    if (RelayState4 != newState) {
      RelayState4 = newState;
      digitalWrite(RelayPin4, !RelayState4);
      saveRelayState(4, RelayState4);
      notifyTelegramStateChange("MQTT", 3, newState);
      pendingSinricProUpdate = true;
    }
  } else if (strcmp(topic, sub5) == 0) {
    if (RelayState5 != newState) {
      RelayState5 = newState;
      digitalWrite(RelayPin5, !RelayState5);
      saveRelayState(5, RelayState5);
      notifyTelegramStateChange("MQTT", 4, newState);
      pendingSinricProUpdate = true;
    }
  } else if (strcmp(topic, sub6) == 0) {
    if (RelayState6 != newState) {
      RelayState6 = newState;
      digitalWrite(RelayPin6, !RelayState6);
      saveRelayState(6, RelayState6);
      notifyTelegramStateChange("MQTT", 5, newState);
      pendingSinricProUpdate = true;
    }
  } else if (strcmp(topic, sub7) == 0) {
    if (RelayState7 != newState) {
      RelayState7 = newState;
      digitalWrite(RelayPin7, !RelayState7);
      saveRelayState(7, RelayState7);
      notifyTelegramStateChange("MQTT", 6, newState);
      pendingSinricProUpdate = true;
    }
  } else if (strcmp(topic, sub8) == 0) {
    if (RelayState8 != newState) {
      RelayState8 = newState;
      digitalWrite(RelayPin8, !RelayState8);
      saveRelayState(8, RelayState8);
      notifyTelegramStateChange("MQTT", 7, newState);
      pendingSinricProUpdate = true;
    }
  } else if (strcmp(topic, sub9) == 0) {
    // ========== HANDLE HERMES COMMANDS ==========
    // Formato: COMANDO:ITEM (ex: LIGAR:Varanda, DESLIGAR:Sala, SENSORES, STATUS)
    messageTemp.toUpperCase(); // normaliza pra maiúsculas
    String cmd = messageTemp;

    // SENSORES
    if (cmd == "SENSORES") {
      Serial.println("✅ Hermes SENSORES: solicitado, forçando leitura...");
      lastMsgDHT = 0;
      lastMsgBMP180 = 0;
      readSensors();
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
      Todos = true;
      RelayState1 = RelayState2 = RelayState3 = RelayState4 = true;
      RelayState5 = RelayState6 = RelayState7 = RelayState8 = true;
      const int pins[] = {RelayPin1, RelayPin2, RelayPin3, RelayPin4, RelayPin5, RelayPin6, RelayPin7, RelayPin8};
      for (int i = 0; i < 8; i++) digitalWrite(pins[i], LOW);
      for (int r = 1; r <= 8; r++) saveRelayState(r, true);
      notifyTelegramStateChange("HERMES", -1, true);
      pendingSinricProUpdate = true;
    }
    // DESLIGAR TUDO
    else if (cmd.startsWith("DESLIGAR") && cmd.indexOf("TUDO") != -1) {
      Todos = false;
      RelayState1 = RelayState2 = RelayState3 = RelayState4 = false;
      RelayState5 = RelayState6 = RelayState7 = RelayState8 = false;
      const int pins[] = {RelayPin1, RelayPin2, RelayPin3, RelayPin4, RelayPin5, RelayPin6, RelayPin7, RelayPin8};
      for (int i = 0; i < 8; i++) digitalWrite(pins[i], HIGH);
      for (int r = 1; r <= 8; r++) saveRelayState(r, false);
      notifyTelegramStateChange("HERMES", -1, false);
      pendingSinricProUpdate = true;
    }
    // LIGAR individual
    else if (cmd.startsWith("LIGAR:")) {
      String item = cmd.substring(6); item.trim();
      if (item == "VARANDA") { RelayState1 = true; digitalWrite(RelayPin1, LOW); saveRelayState(1, true); notifyTelegramStateChange("HERMES", 0, true); }
      else if (item == "BANCADA") { RelayState2 = true; digitalWrite(RelayPin2, LOW); saveRelayState(2, true); notifyTelegramStateChange("HERMES", 1, true); }
      else if (item == "SALA") { RelayState3 = true; digitalWrite(RelayPin3, LOW); saveRelayState(3, true); notifyTelegramStateChange("HERMES", 2, true); }
      else if (item == "COZINHA") { RelayState4 = true; digitalWrite(RelayPin4, LOW); saveRelayState(4, true); notifyTelegramStateChange("HERMES", 3, true); }
      else if (item == "QUINTAL") { RelayState5 = true; digitalWrite(RelayPin5, LOW); saveRelayState(5, true); notifyTelegramStateChange("HERMES", 4, true); }
      else if (item == "VAL" || item == "VARÃO") { RelayState6 = true; digitalWrite(RelayPin6, LOW); saveRelayState(6, true); notifyTelegramStateChange("HERMES", 5, true); }
      else if (item == "ROBSON") { RelayState7 = true; digitalWrite(RelayPin7, LOW); saveRelayState(7, true); notifyTelegramStateChange("HERMES", 6, true); }
      else if (item == "KINHA") { RelayState8 = true; digitalWrite(RelayPin8, LOW); saveRelayState(8, true); notifyTelegramStateChange("HERMES", 7, true); }
      pendingSinricProUpdate = true;
      Todos = RelayState1 && RelayState2 && RelayState3 && RelayState4 && RelayState5 && RelayState6 && RelayState7 && RelayState8;
    }
    // DESLIGAR individual
    else if (cmd.startsWith("DESLIGAR:")) {
      String item = cmd.substring(9); item.trim();
      if (item == "VARANDA") { RelayState1 = false; digitalWrite(RelayPin1, HIGH); saveRelayState(1, false); notifyTelegramStateChange("HERMES", 0, false); }
      else if (item == "BANCADA") { RelayState2 = false; digitalWrite(RelayPin2, HIGH); saveRelayState(2, false); notifyTelegramStateChange("HERMES", 1, false); }
      else if (item == "SALA") { RelayState3 = false; digitalWrite(RelayPin3, HIGH); saveRelayState(3, false); notifyTelegramStateChange("HERMES", 2, false); }
      else if (item == "COZINHA") { RelayState4 = false; digitalWrite(RelayPin4, HIGH); saveRelayState(4, false); notifyTelegramStateChange("HERMES", 3, false); }
      else if (item == "QUINTAL") { RelayState5 = false; digitalWrite(RelayPin5, HIGH); saveRelayState(5, false); notifyTelegramStateChange("HERMES", 4, false); }
      else if (item == "VAL" || item == "VARÃO") { RelayState6 = false; digitalWrite(RelayPin6, HIGH); saveRelayState(6, false); notifyTelegramStateChange("HERMES", 5, false); }
      else if (item == "ROBSON") { RelayState7 = false; digitalWrite(RelayPin7, HIGH); saveRelayState(7, false); notifyTelegramStateChange("HERMES", 6, false); }
      else if (item == "KINHA") { RelayState8 = false; digitalWrite(RelayPin8, HIGH); saveRelayState(8, false); notifyTelegramStateChange("HERMES", 7, false); }
      pendingSinricProUpdate = true;
      Todos = RelayState1 && RelayState2 && RelayState3 && RelayState4 && RelayState5 && RelayState6 && RelayState7 && RelayState8;
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
    char buffer[10];

    // NÃO chame bmp.begin() aqui!
    pressure = bmp.readPressure();
    if (pressure != 0) {
      // Pressão atual
      dtostrf(pressure / 100.0, 2, 2, buffer);

      // Pressão ao nível do mar
      float seaLevelPressure = bmp.readSealevelPressure(pressaoNivelMar);
      if (seaLevelPressure != 0) {
        dtostrf(seaLevelPressure / 100.0, 2, 2, buffer);
      }

      // Altitude
      float altitudeReal = bmp.readAltitude(pressaoNivelMar * 100);
      if (!isnan(altitudeReal)) {
        // Publica altitude real (acima da referência pressaoNivelMar)
        dtostrf(altitudeReal, 2, 2, buffer);

        // Calcula e publica altitude total (acima do nível do mar)
        altitude = altitudeReal + altitudeNivelMar;
        altitudeTotal = altitude; // Sincroniza variável global adicional
        dtostrf(altitude, 2, 2, buffer);

        // Sinaliza para o Core 0 publicar os dados
        pendingSensorMqttUpdate = true;

        // Lê a temperatura do BMP180
        float tempBMP = bmp.readTemperature();
        temperature = tempBMP; // atualiza global

        // Debug no serial
        Serial.println("\n=== Leitura BMP180 ===");
        Serial.print("Pressão: ");
        Serial.print(pressure / 100.0);
        Serial.println(" hPa");
        Serial.print("Pressão nível do mar: ");
        Serial.print(seaLevelPressure / 100.0);
        Serial.println(" hPa");
        Serial.print("Altitude real: ");
        Serial.print(altitudeReal);
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

  // Pressão ao nível do mar (correta)
  float seaLevelPressure = bmp.readSealevelPressure(pressaoNivelMar);
  dtostrf(seaLevelPressure / 100.0, 6, 2, tempStr);
  mqttClient.publish(pub14, tempStr, true); // Pressão ao nível do mar BMP180

  // Altitude real
  float altitudeReal = bmp.readAltitude(pressaoNivelMar * 100);
  dtostrf(altitudeReal, 6, 2, tempStr);
  mqttClient.publish(pub15, tempStr, true); // Altitude real BMP180

  // Altitude em relação ao nível do mar
  dtostrf(altitude, 6, 2, tempStr);
  mqttClient.publish(pub16, tempStr, true); // Altitude total BMP180
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
  String key = "RelayState" + String(relayNum);
  preferences.putBool(key.c_str(), state);
}

bool loadRelayState(int relayNum, bool defaultValue) {
  String key = "RelayState" + String(relayNum);
  return preferences.getBool(key.c_str(), defaultValue);
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
  digitalWrite(RelayPin1, !RelayState1);
  digitalWrite(RelayPin2, !RelayState2);
  digitalWrite(RelayPin3, !RelayState3);
  digitalWrite(RelayPin4, !RelayState4);
  digitalWrite(RelayPin5, !RelayState5);
  digitalWrite(RelayPin6, !RelayState6);
  digitalWrite(RelayPin7, !RelayState7);
  digitalWrite(RelayPin8, !RelayState8);
}

// =============== PROTÓTIPOS DE FUNÇÕES ===============
void setupWiFi();
void setupMQTT();
void handleMQTT();
void reconnectMQTT();
void checkWiFiConnection();
void readDHTSensor();
void readBMP180Sensor();
void readSensors();
void publishSensorData();
void publishRelayStates();

// =============== INÍCIO ===============
void setup() {
  // 1. Inicialização básica e hardware
  Serial.begin(115200);
  delay(100);
  Serial.println("\n[SISTEMA] Iniciando hardware...");

  // Carrega configurações MQTT salvas no NVS (antes do WiFiManager)
  loadMQTTConfig();

  pinMode(RelayPin1, OUTPUT);
  pinMode(RelayPin2, OUTPUT);
  pinMode(RelayPin3, OUTPUT);
  pinMode(RelayPin4, OUTPUT);
  pinMode(RelayPin5, OUTPUT);
  pinMode(RelayPin6, OUTPUT);
  pinMode(RelayPin7, OUTPUT);
  pinMode(RelayPin8, OUTPUT);
  pinMode(wifiLed, OUTPUT);
  pinMode(configButton, INPUT_PULLUP);

  // 2. Inicialização de Comunicação Inter-Core (IPC)
  // Criar a fila ANTES de qualquer serviço que possa usá-la
  telegramQueue = xQueueCreate(10, sizeof(TelegramNotification));

  // 3. Persistência e Estados Iniciais
  preferences.begin("relay states", false);
  bool needsReset = preferences.getBool("needsReset", true);
  if (needsReset) {
    for (int i = 1; i <= 8; i++) {
      String key = "RelayState" + String(i);
      preferences.remove(key.c_str());
    }
    preferences.putBool("needsReset", false);
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
      4096,
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
    // ── Botão de Configuração (pressionar por 3s para abrir portal) ─────
    if (digitalRead(configButton) == LOW) {
      delay(3000);
      if (digitalRead(configButton) == LOW) {
        Serial.println("[BOTAO] Abrindo portal WiFiManager...");
        WiFi.disconnect();
        wm.startConfigPortal(globalHostname);
        setupMDNS();
        // Recarrega config do NVS e reconfigura MQTT após fechar portal
        loadMQTTConfig();
        setupMQTT();
      }
    }

    unsigned long currentMillis = millis();

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
      } else {
        WiFiClient rejectedClient = telnetServer.available();
        rejectedClient.stop();
      }
    }

    if (telnetClient && telnetClient.connected()) {
      while (telnetClient.available()) {
        char c = telnetClient.read();
        Serial.write(c);
      }
    }

    // ── LED de Status ─────────────────────────────────────────────────
    updateStatusLED();

    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

// Task para sensores e relés - Core 1
void TaskSensores(void *pvParameters) {
  (void)pvParameters;
  for (;;) {
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
