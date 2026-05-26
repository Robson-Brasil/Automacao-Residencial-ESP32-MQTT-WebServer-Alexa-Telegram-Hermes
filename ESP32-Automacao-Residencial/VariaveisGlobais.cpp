#include "VariaveisGlobais.h"
#include "DefinicoesProjeto.h"

// Definições (sem "extern" aqui)
bool Todos = false;
bool RelayState1 = false;
bool RelayState2 = false;
bool RelayState3 = false;
bool RelayState4 = false;
bool RelayState5 = false;
bool RelayState6 = false;
bool RelayState7 = false;
bool RelayState8 = false;

char str_hum_data[7] = "--";
char str_temp_data[7] = "--";
char str_tempterm_data[7] = "--";
char str_dewpoint_data[7] = "--";
char str_tempF_data[7] = "--";
float temperature = 0.0f;
float pressure = 0.0f;
float altitude = 0.0f;
float altitudeTotal = 0.0f;
float seaLevelPressureCache = 0.0f;
float altitudeRealCache = 0.0f;
unsigned long lastMsgDHT = 0;
unsigned long lastMsgBMP180 = 0;
unsigned long lastWifiRetryTime = 0;
unsigned long lastWifiCheckTime = 0;
unsigned long lastMqttReconnectAttempt = 0;
unsigned long lastRelayUpdate = 0;
bool wifiConnected = false;
bool mqttConnected = false;

// Sinalizadores de sincronização
QueueHandle_t telegramQueue = NULL;
bool pendingMqttUpdate = false;
bool pendingSinricProUpdate = false;
bool pendingSensorMqttUpdate = false;
bool pendingSensorSinricUpdate = false;
bool pendingHADiscovery = false;


// Handles das Tasks FreeRTOS
TaskHandle_t hTaskConexoes = NULL;
TaskHandle_t hTaskSensores = NULL;
bool otaInProgress = false;

const char* globalHostname = DEVICE_HOSTNAME;

SemaphoreHandle_t relayMutex = NULL;

// Watchdog heartbeats
volatile uint32_t heartbeatConexoes = 0;
volatile uint32_t heartbeatSensores = 0;
