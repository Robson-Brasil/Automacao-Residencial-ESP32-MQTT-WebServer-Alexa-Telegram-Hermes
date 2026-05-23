#ifndef WIFIMANAGERCONFIG_H
#define WIFIMANAGERCONFIG_H

#include <WiFiManager.h>
#include <ESPmDNS.h>
#include <Preferences.h>
#include <string.h>
#include "VariaveisGlobais.h"
#include "LoginsSenhas.h"

// Protótipo da função no .ino (reconfigura mqttClient com novos valores)
extern void setupMQTT();

WiFiManager wm;

// Guard para evitar addParameter duplicado
static bool wmParamsAdded = false;

// WiFiManager Parameters para MQTT
WiFiManagerParameter custom_mqtt_broker("mqtt_broker", "IP do Broker", mqtt_broker, 40);
WiFiManagerParameter custom_mqtt_port("mqtt_port", "Porta MQTT", mqtt_port, 6);
WiFiManagerParameter custom_mqtt_user("mqtt_user", "Usuario MQTT", mqtt_user, 32);
WiFiManagerParameter custom_mqtt_pass("mqtt_pass", "Senha MQTT", mqtt_pass, 32, "type=\"password\"");
WiFiManagerParameter custom_mqtt_client("mqtt_client", "Client ID", mqtt_client, 24);

// ============================================================
// Persistência NVS (Preferences)
// ============================================================
void loadMQTTConfig() {
    Preferences prefs;
    prefs.begin("mqtt_config", true); // readOnly

    prefs.getString("broker", mqtt_broker, sizeof(mqtt_broker));
    prefs.getString("port", mqtt_port, sizeof(mqtt_port));
    prefs.getString("user", mqtt_user, sizeof(mqtt_user));
    prefs.getString("pass", mqtt_pass, sizeof(mqtt_pass));
    prefs.getString("client", mqtt_client, sizeof(mqtt_client));

    prefs.end();

    MQTT_PORT = atoi(mqtt_port);

    Serial.println("[NVS] Config MQTT carregada:");
    Serial.print("[NVS] Broker: "); Serial.println(mqtt_broker);
    Serial.print("[NVS] Porta:  "); Serial.println(MQTT_PORT);
    Serial.print("[NVS] User:   "); Serial.println(mqtt_user);
    Serial.print("[NVS] Client: "); Serial.println(mqtt_client);
}

void saveMQTTConfigToNVS() {
    Preferences prefs;
    prefs.begin("mqtt_config", false); // write mode

    prefs.putString("broker", mqtt_broker);
    prefs.putString("port", mqtt_port);
    prefs.putString("user", mqtt_user);
    prefs.putString("pass", mqtt_pass);
    prefs.putString("client", mqtt_client);

    prefs.end();

    Serial.println("[NVS] Config MQTT salva com sucesso!");
}

// ============================================================
// Callback do WiFiManager — atualiza buffers + NVS + reconfigura MQTT
// ============================================================
void saveMQTTConfigCallback() {
    Serial.println("[WM] Salvando configuracoes MQTT...");

    strncpy(mqtt_broker, custom_mqtt_broker.getValue(), sizeof(mqtt_broker) - 1);
    mqtt_broker[sizeof(mqtt_broker) - 1] = '\0';

    strncpy(mqtt_port, custom_mqtt_port.getValue(), sizeof(mqtt_port) - 1);
    mqtt_port[sizeof(mqtt_port) - 1] = '\0';

    strncpy(mqtt_user, custom_mqtt_user.getValue(), sizeof(mqtt_user) - 1);
    mqtt_user[sizeof(mqtt_user) - 1] = '\0';

    strncpy(mqtt_pass, custom_mqtt_pass.getValue(), sizeof(mqtt_pass) - 1);
    mqtt_pass[sizeof(mqtt_pass) - 1] = '\0';

    strncpy(mqtt_client, custom_mqtt_client.getValue(), sizeof(mqtt_client) - 1);
    mqtt_client[sizeof(mqtt_client) - 1] = '\0';

    MQTT_PORT = atoi(mqtt_port);

    // Persiste no NVS
    saveMQTTConfigToNVS();

    // Reconfigura o cliente MQTT com os novos valores
    setupMQTT();

    Serial.print("[WM] Broker: "); Serial.println(mqtt_broker);
    Serial.print("[WM] Porta: "); Serial.println(MQTT_PORT);
    Serial.print("[WM] Usuario: "); Serial.println(mqtt_user);
    Serial.print("[WM] Client ID: "); Serial.println(mqtt_client);
}

// ============================================================
// Setup do WiFiManager (parâmetros adicionados apenas 1 vez)
// ============================================================
bool WiFiManagerSetup() {
    if (!wmParamsAdded) {
        wm.addParameter(&custom_mqtt_broker);
        wm.addParameter(&custom_mqtt_port);
        wm.addParameter(&custom_mqtt_user);
        wm.addParameter(&custom_mqtt_pass);
        wm.addParameter(&custom_mqtt_client);
        wmParamsAdded = true;
    }

    wm.setSaveConfigCallback(saveMQTTConfigCallback);
    wm.setConfigPortalTimeout(180);
    wm.setConnectTimeout(30);
    wm.setDebugOutput(false);

    Serial.println("[WM] Iniciando portal WiFiManager...");

    bool res = wm.autoConnect(globalHostname);

    if (!res) {
        Serial.println("[WM] Falha na conexao WiFi - modo AP ativo");
        return false;
    }

    Serial.println("[WM] WiFi conectado com sucesso!");
    Serial.print("[WM] IP: ");
    Serial.println(WiFi.localIP());

    return true;
}

void setupMDNS() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("[mDNS] WiFi nao conectado, abortando...");
        return;
    }

    MDNS.end();
    WiFi.setHostname(globalHostname);

    delay(500);
    Serial.println("[mDNS] Iniciando...");

    if (MDNS.begin(globalHostname)) {
        Serial.printf("[mDNS] Respondendo em: http://%s.local:8181\n", globalHostname);

        MDNS.addService("http", "tcp", 8181);
    } else {
        Serial.println("[mDNS] FALHA!");
    }
}

#endif
