// =================== LOGINS E SENHAS ===================
#ifndef LOGINSSENHAS_H
#define LOGINSSENHAS_H

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

// Telegram - buffers carregados do NVS (fallback para valores padrão)
#define DEFAULT_TELEGRAM_TOKEN "8444037202:AAGB4P8wjPtjiGzlBHInCYkLUqTa1OUzgHI"
#define DEFAULT_CHAT_ID "5270818980"

// SinricPro (Alexa / Google Home)
#define DEFAULT_SINRICPRO_APP_KEY "4914a0d0-327e-4815-8128-822b7a80713d"
#define DEFAULT_SINRICPRO_APP_SECRET "b73f409a-fcc5-4c60-8cf4-d86d9b4e2bae-e7fc2fe6-1b4a-4b71-aa38-de61ad019107"
#define DEFAULT_SINRICPRO_DEVICE_BANCADA "69fd1b19baa50bf9bf2e4b8e"
#define DEFAULT_SINRICPRO_DEVICE_REFLETOR "69fd1bc0977a0619a739f409"
#define DEFAULT_SINRICPRO_DEVICE_TEMP "69fd1c4dbaa50bf9bf2e4c2a"

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

#endif // LOGINSSENHAS_H
