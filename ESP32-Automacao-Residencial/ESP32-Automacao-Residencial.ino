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
#include "Bibliotecas.h"
#include "ConstantesTempo.h"
#include "DefinicoesProjeto.h"
#include "GPIOs.h"
#include "LoginsSenhas.h"
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
#include "RelayManager.h"
#include "SensorManager.h"
#include "MQTTHandler.h"

// =============== INSTÂNCIAS DE OBJETOS ===============
WiFiServer telnetServer(2323);
WiFiClient telnetClient;
bool telnetAuthenticated = false;
DualSerialClass DualSerial;
extern WiFiClientSecure telegramClient;
extern UniversalTelegramBot *telegramBot;

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

/**
 * @brief Inicia conexão WiFi com IP estático e credenciais.
 * Não bloqueia — apenas inicia o processo de conexão.
 */
void setupWiFi() {
  if (WiFi.status() == WL_CONNECTED) return;

  #ifdef STATIC_IP
  IPAddress staticIP(STATIC_IP);
  IPAddress gateway(STATIC_GATEWAY);
  IPAddress subnet(STATIC_SUBNET);
  IPAddress dns(STATIC_DNS);
  WiFi.config(staticIP, gateway, subnet, dns);
  #endif

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.println("[WiFi] Conectando (" WIFI_SSID ")...");
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
