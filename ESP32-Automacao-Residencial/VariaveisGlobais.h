/**
 * @file VariaveisGlobais.h
 * @brief Gerenciamento do estado global do sistema.
 * Utiliza o modificador 'extern' para compartilhamento de dados entre múltiplos arquivos.
 */
#ifndef VARIAVEISGLOBAIS_H
#define VARIAVEISGLOBAIS_H

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

// Estados dos relés
enum RelayStates
{
    RELAY_OFF = 0,
    RELAY_ON = 1
};

extern bool Todos;
extern bool RelayState1;
extern bool RelayState2;
extern bool RelayState3;
extern bool RelayState4;
extern bool RelayState5;
extern bool RelayState6;
extern bool RelayState7;
extern bool RelayState8;

extern char str_hum_data[7];
extern char str_temp_data[7];
extern char str_tempterm_data[7];
extern char str_dewpoint_data[7];
extern char str_tempF_data[7];
extern float temperature;
extern float pressure;
extern float altitude;
extern float altitudeTotal;
extern float seaLevelPressureCache;
extern float altitudeRealCache;
extern unsigned long lastMsgDHT;
extern unsigned long lastMsgBMP180;
extern unsigned long lastWifiRetryTime;
extern unsigned long lastWifiCheckTime;
extern unsigned long lastMqttReconnectAttempt;
extern unsigned long lastRelayUpdate;
extern bool wifiConnected;
extern bool mqttConnected;

// Estrutura para fila de notificações do Telegram (Thread-safe)
struct TelegramNotification {
    char source[32];
    int relayNum;
    bool relayState;
};

// Filas e Sinalizadores de sincronização entre núcleos (Thread-safe)
extern QueueHandle_t telegramQueue;
extern bool pendingMqttUpdate;
extern bool pendingSinricProUpdate;
extern bool pendingSensorMqttUpdate;
extern bool pendingSensorSinricUpdate;
extern bool pendingHADiscovery;


// Handles das Tasks FreeRTOS
extern TaskHandle_t hTaskConexoes;
extern TaskHandle_t hTaskSensores;
extern bool otaInProgress;

extern const char* globalHostname;

extern SemaphoreHandle_t relayMutex;

// Watchdog heartbeats
extern volatile uint32_t heartbeatConexoes;
extern volatile uint32_t heartbeatSensores;

#endif // VARIAVEISGLOBAIS_H
