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

// Telegram - hardcoded por segurança (não precisa ser configurável)
extern const char* BOT_TOKEN;
extern const char* CHAT_ID;

// SinricPro (Alexa / Google Home) - hardcoded (não precisa ser configurável)
extern const char* SINRICPRO_APP_KEY;
extern const char* SINRICPRO_APP_SECRET;
extern const char* SINRICPRO_DEVICE_BANCADA;
extern const char* SINRICPRO_DEVICE_REFLETOR;
extern const char* SINRICPRO_DEVICE_TEMP_SENSOR;

#endif // LOGINSSENHAS_H
