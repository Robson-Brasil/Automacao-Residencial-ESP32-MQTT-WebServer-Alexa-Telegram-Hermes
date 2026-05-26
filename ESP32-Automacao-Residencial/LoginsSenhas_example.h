// =================== LOGINS E SENHAS ===================
// ATENÇÃO: Copie este arquivo para LoginsSenhas.h e preencha com suas credenciais.
// LoginsSenhas.h está no .gitignore — nunca commite credenciais reais.
#ifndef LOGINSSENHAS_EXAMPLE_H
#define LOGINSSENHAS_EXAMPLE_H

// Variáveis globais do MQTT - buffers para configuração via WiFiManager
// Valores padrão vazios — o cliente configura tudo pelo portal WiFiManager
char mqtt_broker[40] = "";
char mqtt_port[6] = "";
char mqtt_user[32] = "";
char mqtt_pass[32] = "";
char mqtt_client[24] = "";

// Ponteiros para compatibilidade com PubSubClient
const char* BrokerMQTT = mqtt_broker;
const char* LoginDoMQTT = mqtt_user;
const char* SenhaMQTT = mqtt_pass;
int MQTT_PORT = 1883;
const char* clientID = mqtt_client;

// OTA - senha para atualização over-the-air
#define OTA_PASSWORD "SUA_SENHA_OTA_AQUI"

// Telegram - substitua pelos seus valores reais
#define DEFAULT_TELEGRAM_TOKEN "SEU_TELEGRAM_BOT_TOKEN_AQUI"
#define DEFAULT_CHAT_ID "SEU_CHAT_ID_AQUI"

// SinricPro (Alexa / Google Home) - substitua pelos seus valores reais
#define DEFAULT_SINRICPRO_APP_KEY "SEU_SINRICPRO_APP_KEY_AQUI"
#define DEFAULT_SINRICPRO_APP_SECRET "SEU_SINRICPRO_APP_SECRET_AQUI"
#define DEFAULT_SINRICPRO_DEVICE_BANCADA "SEU_DEVICE_ID_BANCADA"
#define DEFAULT_SINRICPRO_DEVICE_REFLETOR "SEU_DEVICE_ID_REFLETOR"
#define DEFAULT_SINRICPRO_DEVICE_TEMP "SEU_DEVICE_ID_TEMPERATURA"

// Buffers mutáveis (populados do NVS via loadCredentials(), fallback para DEFAULT_*)
char telegram_token[64] = DEFAULT_TELEGRAM_TOKEN;
char telegram_chatid[16] = DEFAULT_CHAT_ID;
char sinric_app_key[40] = DEFAULT_SINRICPRO_APP_KEY;
char sinric_app_secret[80] = DEFAULT_SINRICPRO_APP_SECRET;
char sinric_device_bancada[30] = DEFAULT_SINRICPRO_DEVICE_BANCADA;
char sinric_device_refletor[30] = DEFAULT_SINRICPRO_DEVICE_REFLETOR;
char sinric_device_temp[30] = DEFAULT_SINRICPRO_DEVICE_TEMP;

// Aliases de compatibilidade (código existente usa BOT_TOKEN, SINRICPRO_APP_KEY etc.)
const char* BOT_TOKEN = telegram_token;
const char* CHAT_ID = telegram_chatid;
const char* SINRICPRO_APP_KEY = sinric_app_key;
const char* SINRICPRO_APP_SECRET = sinric_app_secret;
const char* SINRICPRO_DEVICE_BANCADA = sinric_device_bancada;
const char* SINRICPRO_DEVICE_REFLETOR = sinric_device_refletor;
const char* SINRICPRO_DEVICE_TEMP_SENSOR = sinric_device_temp;

#endif // LOGINSSENHAS_EXAMPLE_H
