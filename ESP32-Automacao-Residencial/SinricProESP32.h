/**
 * @file SinricProESP32.h
 * @brief Integração com SinricPro para controle via Amazon Alexa e Google Home.
 * 
 * Dispositivos configurados:
 * - Bancada (Switch) - RelayPin2 GPIO 13
 * - Refletor (Switch) - RelayPin8 GPIO 19
 * - Sensor de Temperatura (DHT22) - GPIO 25
 * 
 * Este módulo funciona como camada adicional ao sistema existente,
 * sincronizando estados com MQTT, Telegram e Dashboard Web.
 */
#ifndef SINRICPROESP32_H
#define SINRICPROESP32_H

#include "Bibliotecas.h"
#include "VariaveisGlobais.h"
#include "GPIOs.h"

// ── Protótipos externos (funções definidas no .ino principal) ──────────────
extern void saveRelayState(int relayNum, bool state);
extern void publishRelayStates();
extern void notifyTelegramStateChange(const String& source, int relayNum, bool relayState);
extern void logRelayAction(const char* source, int relayNum, bool state);

// ── Credenciais SinricPro (definidas no .ino principal) ────────────────────
extern const char* SINRICPRO_APP_KEY;
extern const char* SINRICPRO_APP_SECRET;
extern const char* SINRICPRO_DEVICE_BANCADA;
extern const char* SINRICPRO_DEVICE_REFLETOR;
extern const char* SINRICPRO_DEVICE_TEMP_SENSOR;

// ── Variáveis de controle do SinricPro ─────────────────────────────────────
bool sinricProConnected = false;
unsigned long lastSinricProTempEvent = 0;
const unsigned long SINRICPRO_TEMP_EVENT_INTERVAL = 60000; // 60 segundos entre eventos de temperatura
float lastSinricProTemp = -999.0;
float lastSinricProHum = -999.0;

// ── Flag para evitar loop infinito de sincronização ────────────────────────
// Quando o SinricPro recebe um comando, ele muda o relé e seta pendingMqttUpdate.
// Quando o MQTT/Telegram/Dashboard mudam um relé, eles setam pendingSinricProUpdate.
// Essa flag impede que a mudança vinda do SinricPro cause um evento de volta para ele.
bool sinricProOriginatedChange = false;

/**
 * @brief Callback chamado quando a Alexa/Google Home envia comando de ligar/desligar.
 * 
 * @param deviceId ID do dispositivo SinricPro que recebeu o comando
 * @param state Novo estado solicitado (true = ligar, false = desligar)
 * @return true se o comando foi executado com sucesso
 */
bool onPowerState(const String &deviceId, bool &state) {
  sinricProOriginatedChange = true; // Marca que a mudança veio do SinricPro

  if (deviceId == SINRICPRO_DEVICE_BANCADA) {
    // Bancada = RelayPin2 (GPIO 13) = RelayState2
    if (RelayState2 != state) {
      RelayState2 = state;
      digitalWrite(RelayPin2, !RelayState2); // active-low
      logRelayAction("Alexa/Google", 1, state);
      saveRelayState(2, RelayState2);
      pendingMqttUpdate = true;
      notifyTelegramStateChange("Alexa/Google", 1, state); // index 1 = Bancada
      Todos = RelayState1 && RelayState2 && RelayState3 && RelayState4 &&
              RelayState5 && RelayState6 && RelayState7 && RelayState8;
    }
    
    Serial.print("[SinricPro] Bancada: ");
    Serial.println(state ? "LIGADO" : "DESLIGADO");
  }
  else if (deviceId == SINRICPRO_DEVICE_REFLETOR) {
    // Refletor = RelayPin8 (GPIO 19) = RelayState8
    if (RelayState8 != state) {
      RelayState8 = state;
      digitalWrite(RelayPin8, !RelayState8); // active-low
      logRelayAction("Alexa/Google", 7, state);
      saveRelayState(8, RelayState8);
      pendingMqttUpdate = true;
      notifyTelegramStateChange("Alexa/Google", 7, state); // index 7 = Refletor/Kinha
      Todos = RelayState1 && RelayState2 && RelayState3 && RelayState4 &&
              RelayState5 && RelayState6 && RelayState7 && RelayState8;
    }
    
    Serial.print("[SinricPro] Refletor: ");
    Serial.println(state ? "LIGADO" : "DESLIGADO");
  }
  else {
    Serial.print("[SinricPro] Device ID desconhecido: ");
    Serial.println(deviceId);
    sinricProOriginatedChange = false;
    return false;
  }

  sinricProOriginatedChange = false;
  return true; // Indica ao SinricPro que o comando foi processado com sucesso
}

/**
 * @brief Configura e inicializa o SinricPro SDK.
 * Registra os dispositivos Switch e o sensor de temperatura.
 * Deve ser chamado após o WiFi estar configurado.
 */
void setupSinricPro() {
  // Registra os dispositivos Switch e seus callbacks
  SinricProSwitch &switchBancada = SinricPro[SINRICPRO_DEVICE_BANCADA];
  switchBancada.onPowerState(onPowerState);

  SinricProSwitch &switchRefletor = SinricPro[SINRICPRO_DEVICE_REFLETOR];
  switchRefletor.onPowerState(onPowerState);

  // Registra o sensor de temperatura (não precisa de callback, só envia eventos)
  SinricProTemperaturesensor &tempSensor = SinricPro[SINRICPRO_DEVICE_TEMP_SENSOR];

  // Callbacks de conexão/desconexão
  SinricPro.onConnected([]() {
    sinricProConnected = true;
    Serial.println("==============================");
    Serial.println("[SinricPro] CONECTADO!");
    Serial.println("  Alexa e Google Home prontos.");
    Serial.println("==============================");
  });

  SinricPro.onDisconnected([]() {
    sinricProConnected = false;
    Serial.println("[SinricPro] DESCONECTADO!");
  });

  // Inicia a conexão com o servidor SinricPro
  SinricPro.begin(SINRICPRO_APP_KEY, SINRICPRO_APP_SECRET);
  
  Serial.println("[SinricPro] Inicializado - Aguardando conexão...");
  Serial.println("[SinricPro] Dispositivos registrados:");
  Serial.println("  - Bancada (Switch) -> GPIO 13");
  Serial.println("  - Refletor (Switch) -> GPIO 19");
  Serial.println("  - Sensor Temperatura (DHT22) -> GPIO 25");
}

/**
 * @brief Loop principal do SinricPro.
 * Processa mensagens WebSocket recebidas do servidor.
 * Deve ser chamado frequentemente na TaskConexoes (Core 0).
 */
void sinricProLoop() {
  SinricPro.handle();
}

/**
 * @brief Envia evento de temperatura e umidade para o SinricPro.
 * Respeita o rate limiting (mínimo 60 segundos entre eventos).
 * Só envia se os valores mudaram desde o último envio.
 * 
 * @param temp Temperatura em graus Celsius
 * @param hum Umidade relativa em percentual
 */
void sendSinricProTemperatureEvent(float temp, float hum) {
  if (!sinricProConnected) return;
  
  unsigned long now = millis();
  if (now - lastSinricProTempEvent < SINRICPRO_TEMP_EVENT_INTERVAL) return;
  
  // Só envia se houve mudança significativa (0.5°C ou 1%)
  if (abs(temp - lastSinricProTemp) < 0.5 && abs(hum - lastSinricProHum) < 1.0) return;
  
  SinricProTemperaturesensor &mySensor = SinricPro[SINRICPRO_DEVICE_TEMP_SENSOR];
  bool success = mySensor.sendTemperatureEvent(temp, hum);
  
  if (success) {
    lastSinricProTemp = temp;
    lastSinricProHum = hum;
    lastSinricProTempEvent = now;
    Serial.printf("[SinricPro] Temp: %.1f°C | Hum: %.1f%%\r\n", temp, hum);
  } else {
    Serial.println("[SinricPro] Falha ao enviar evento de temperatura.");
  }
}

/**
 * @brief Envia eventos de estado dos switches para o SinricPro.
 * Chamado quando o estado dos relés muda por MQTT, Telegram ou Dashboard.
 * NÃO é chamado quando a mudança veio do próprio SinricPro (evita loop).
 */
void sendSinricProSwitchEvents() {
  if (!sinricProConnected) return;
  if (sinricProOriginatedChange) return; // Não reenvia se a mudança veio do SinricPro
  
  // Envia estado atual da Bancada (RelayState2)
  SinricProSwitch &switchBancada = SinricPro[SINRICPRO_DEVICE_BANCADA];
  switchBancada.sendPowerStateEvent(RelayState2);
  
  // Envia estado atual do Refletor (RelayState8)
  SinricProSwitch &switchRefletor = SinricPro[SINRICPRO_DEVICE_REFLETOR];
  switchRefletor.sendPowerStateEvent(RelayState8);
  
  Serial.printf("[SinricPro] Sync: Bancada=%s | Refletor=%s\r\n",
    RelayState2 ? "ON" : "OFF",
    RelayState8 ? "ON" : "OFF");
}

#endif // SINRICPROESP32_H
