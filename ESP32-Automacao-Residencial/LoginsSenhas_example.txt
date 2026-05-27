#include "LoginsSenhas.h"

char mqtt_broker[40]  = "SEU_BROKER_IP";
char mqtt_port[6]     = "1883";
char mqtt_user[32]    = "SEU_USUARIO_MQTT";
char mqtt_pass[32]    = "SUA_SENHA_MQTT";
char mqtt_client[24]  = "ESP32_Automacao";

const char* BrokerMQTT  = mqtt_broker;
const char* LoginDoMQTT = mqtt_user;
const char* SenhaMQTT   = mqtt_pass;
int MQTT_PORT           = 1883;
const char* clientID    = mqtt_client;

char telegram_token[64]            = DEFAULT_TELEGRAM_TOKEN;
char telegram_chatid[16]           = DEFAULT_CHAT_ID;
char sinric_app_key[40]            = DEFAULT_SINRICPRO_APP_KEY;
char sinric_app_secret[80]         = DEFAULT_SINRICPRO_APP_SECRET;
char sinric_device_bancada[30]     = DEFAULT_SINRICPRO_DEVICE_BANCADA;
char sinric_device_refletor[30]    = DEFAULT_SINRICPRO_DEVICE_REFLETOR;
char sinric_device_temp[30]        = DEFAULT_SINRICPRO_DEVICE_TEMP;

const char* BOT_TOKEN                  = telegram_token;
const char* CHAT_ID                    = telegram_chatid;
const char* SINRICPRO_APP_KEY          = sinric_app_key;
const char* SINRICPRO_APP_SECRET       = sinric_app_secret;
const char* SINRICPRO_DEVICE_BANCADA   = sinric_device_bancada;
const char* SINRICPRO_DEVICE_REFLETOR  = sinric_device_refletor;
const char* SINRICPRO_DEVICE_TEMP_SENSOR = sinric_device_temp;
