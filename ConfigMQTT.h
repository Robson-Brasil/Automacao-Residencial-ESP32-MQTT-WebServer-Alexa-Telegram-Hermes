/**
 * @file ConfigMQTT.h
 * @brief Configuração e gerenciamento da comunicação MQTT.
 * Define protótipos para setup, reconexão e tratamento de mensagens recebidas.
 */
#ifndef CONFIGMQTT_H
#define CONFIGMQTT_H

#include "LoginsSenhas.h"

void setupMQTT();
void reconnectMQTT();
void mqttCallback(char* topic, byte* payload, unsigned int length);

#endif // CONFIGMQTT_H
