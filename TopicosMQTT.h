#ifndef TOPICOSMQTT_H
#define TOPICOSMQTT_H

// =============== TÓPICOS MQTT CENTRALIZADOS ===============
// Padrão: ESP32/<Funcao>[/<Subsensor>]/<Tipo>

// --- Subscribe (ESP32 escuta comandos) ---
static const char *sub0  = "ESP32/Ligar-DesligarTudo/Estado";
static const char *sub1  = "ESP32/Interruptor1/Estado";
static const char *sub2  = "ESP32/Interruptor2/Estado";
static const char *sub3  = "ESP32/Interruptor3/Estado";
static const char *sub4  = "ESP32/Interruptor4/Estado";
static const char *sub5  = "ESP32/Interruptor5/Estado";
static const char *sub6  = "ESP32/Interruptor6/Estado";
static const char *sub7  = "ESP32/Interruptor7/Estado";
static const char *sub8  = "ESP32/Interruptor8/Estado";
static const char *sub9  = "ESP32/Hermes/Comando";

// --- Publish (ESP32 publica estado / sensores) ---
static const char *pub0  = "ESP32/Ligar-DesligarTudo/Comando";
static const char *pub1  = "ESP32/Interruptor1/Comando";
static const char *pub2  = "ESP32/Interruptor2/Comando";
static const char *pub3  = "ESP32/Interruptor3/Comando";
static const char *pub4  = "ESP32/Interruptor4/Comando";
static const char *pub5  = "ESP32/Interruptor5/Comando";
static const char *pub6  = "ESP32/Interruptor6/Comando";
static const char *pub7  = "ESP32/Interruptor7/Comando";
static const char *pub8  = "ESP32/Interruptor8/Comando";
static const char *pub9  = "ESP32/Temperatura";
static const char *pub10 = "ESP32/Umidade";
static const char *pub11 = "ESP32/SensacaoTermica";
static const char *pub12 = "ESP32/BMP180/Temperatura";
static const char *pub13 = "ESP32/BMP180/PressaoAtmosferica/Real";
static const char *pub14 = "ESP32/BMP180/PressaoAtmosferica/NivelMar";
static const char *pub15 = "ESP32/BMP180/AltitudeReal";
static const char *pub16 = "ESP32/BMP180/AltitudeNivelMar";
static const char *pub17 = "ESP32/PontoOrvalho";
static const char *pub18 = "ESP32/Hermes/StatusResposta";

#endif
