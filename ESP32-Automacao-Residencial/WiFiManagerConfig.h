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
// Migração: unifica namespaces legados em "app"
// ============================================================
void migrateNVS() {
    Preferences app;
    app.begin("app", false);
    if (app.getBool("_migrated", false)) { app.end(); return; }

    Serial.println("[NVS] Migrando dados legados para namespace 'app'...");

    Preferences old;

    // --- "relay states" → "app" (mesmas chaves) ---
    old.begin("relay states", true);
    for (int i = 1; i <= 8; i++) {
        String k = "RelayState" + String(i);
        bool v1 = old.getBool(k.c_str(), true);
        bool v2 = old.getBool(k.c_str(), false);
        if (v1 == v2) app.putBool(k.c_str(), v1);
    }
    {   bool v1 = old.getBool("needsReset", true);
        bool v2 = old.getBool("needsReset", false);
        if (v1 == v2) app.putBool("needsReset", v1);
    }
    for (int i = 1; i <= 8; i++) {
        String nk = "RelayName" + String(i);
        String nv = old.getString(nk.c_str(), "");
        if (nv.length() > 0) app.putString(nk.c_str(), nv);
    }
    old.end();

    // --- "mqtt_config" → "app" (chaves prefixadas com mqtt_) ---
    old.begin("mqtt_config", true);
    auto cp = [&](const char* k) {
        String v = old.getString(k, "");
        if (v.length()) {
            String nk = String("mqtt_") + k;
            app.putString(nk.c_str(), v);
        }
    };
    cp("broker"); cp("port"); cp("user"); cp("pass"); cp("client");
    old.end();

    // --- "wifi-ip" → "app" (chaves prefixadas com static_) ---
    old.begin("wifi-ip", true);
    IPAddress ip4, gw4, sn4, dns4;
    if (old.getBytes("ip", &ip4, 4) == 4 && ip4[0] != 0) {
        app.putBytes("static_ip", &ip4, 4);
        old.getBytes("gw", &gw4, 4); app.putBytes("static_gw", &gw4, 4);
        old.getBytes("sn", &sn4, 4); app.putBytes("static_sn", &sn4, 4);
        old.getBytes("dns", &dns4, 4); app.putBytes("static_dns", &dns4, 4);
    }
    old.end();

    // --- "credenciais" → "app" (mesmas chaves) ---
    old.begin("credenciais", true);
    auto cr = [&](const char* k) {
        String v = old.getString(k, "");
        if (v.length()) app.putString(k, v);
    };
    cr("telegram_token"); cr("chat_id"); cr("sinric_key");
    cr("sinric_secret"); cr("sinric_bancada"); cr("sinric_refletor"); cr("sinric_temp");
    old.end();

    app.putBool("_migrated", true);
    app.end();

    // Apaga namespaces legados
    auto eraseNS = [](const char* ns) { Preferences p; p.begin(ns, false); p.clear(); p.end(); };
    eraseNS("relay states"); eraseNS("mqtt_config"); eraseNS("wifi-ip"); eraseNS("credenciais");

    Serial.println("[NVS] Migracao concluida. Namespaces legados apagados.");
}

// ============================================================
// Persistência NVS (Preferences)
// ============================================================
void loadMQTTConfig() {
    Preferences prefs;
    prefs.begin("app", true);

    prefs.getString("mqtt_broker", mqtt_broker, sizeof(mqtt_broker));
    prefs.getString("mqtt_port", mqtt_port, sizeof(mqtt_port));
    prefs.getString("mqtt_user", mqtt_user, sizeof(mqtt_user));
    prefs.getString("mqtt_pass", mqtt_pass, sizeof(mqtt_pass));
    prefs.getString("mqtt_client", mqtt_client, sizeof(mqtt_client));

    prefs.end();

    MQTT_PORT = atoi(mqtt_port);

    Serial.println("[NVS] Config MQTT carregada:");
    Serial.print("[NVS] Broker: "); Serial.println(mqtt_broker);
    Serial.print("[NVS] Porta:  "); Serial.println(MQTT_PORT);
    Serial.print("[NVS] User:   "); Serial.println(mqtt_user);
    Serial.print("[NVS] Client: "); Serial.println(mqtt_client);
}

void saveCredentials() {
    Preferences prefs;
    prefs.begin("app", false);

    prefs.putString("telegram_token", telegram_token);
    prefs.putString("chat_id", telegram_chatid);
    prefs.putString("sinric_key", sinric_app_key);
    prefs.putString("sinric_secret", sinric_app_secret);
    prefs.putString("sinric_bancada", sinric_device_bancada);
    prefs.putString("sinric_refletor", sinric_device_refletor);
    prefs.putString("sinric_temp", sinric_device_temp);

    prefs.end();
    Serial.println("[NVS] Credenciais salvas com sucesso!");
}

void loadCredentials() {
    Preferences prefs;
    prefs.begin("app", true);

    String val;

    val = prefs.getString("telegram_token", "");
    if (val.length() > 0) val.toCharArray(telegram_token, sizeof(telegram_token));

    val = prefs.getString("chat_id", "");
    if (val.length() > 0) val.toCharArray(telegram_chatid, sizeof(telegram_chatid));

    val = prefs.getString("sinric_key", "");
    if (val.length() > 0) val.toCharArray(sinric_app_key, sizeof(sinric_app_key));

    val = prefs.getString("sinric_secret", "");
    if (val.length() > 0) val.toCharArray(sinric_app_secret, sizeof(sinric_app_secret));

    val = prefs.getString("sinric_bancada", "");
    if (val.length() > 0) val.toCharArray(sinric_device_bancada, sizeof(sinric_device_bancada));

    val = prefs.getString("sinric_refletor", "");
    if (val.length() > 0) val.toCharArray(sinric_device_refletor, sizeof(sinric_device_refletor));

    val = prefs.getString("sinric_temp", "");
    if (val.length() > 0) val.toCharArray(sinric_device_temp, sizeof(sinric_device_temp));

    prefs.end();
    Serial.println("[NVS] Credenciais carregadas.");
}

void saveMQTTConfigToNVS() {
    Preferences prefs;
    prefs.begin("app", false);

    prefs.putString("mqtt_broker", mqtt_broker);
    prefs.putString("mqtt_port", mqtt_port);
    prefs.putString("mqtt_user", mqtt_user);
    prefs.putString("mqtt_pass", mqtt_pass);
    prefs.putString("mqtt_client", mqtt_client);

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
// DHCP Capture — salva IP automático no NVS
// ============================================================
IPAddress staticIP(0, 0, 0, 0);
IPAddress staticGateway(0, 0, 0, 0);
IPAddress staticSubnet(255, 255, 255, 0);
IPAddress staticDNS(8, 8, 8, 8);
bool staticIPConfigured = false;

void saveStaticIPConfig() {
    if (WiFi.status() != WL_CONNECTED) return;
    IPAddress ip = WiFi.localIP();
    IPAddress gw = WiFi.gatewayIP();
    IPAddress sn = WiFi.subnetMask();
    IPAddress dns = WiFi.dnsIP(0);
    Preferences prefs;
    prefs.begin("app", false);
    prefs.putBytes("static_ip", &ip, 4);
    prefs.putBytes("static_gw", &gw, 4);
    prefs.putBytes("static_sn", &sn, 4);
    prefs.putBytes("static_dns", &dns, 4);
    prefs.end();
    Serial.printf("[DHCP] IP fixo salvo: %s\n", ip.toString().c_str());
}

void loadStaticIPConfig() {
    Preferences prefs;
    prefs.begin("app", true);
    size_t len = prefs.getBytes("static_ip", &staticIP, 4);
    if (len == 4 && staticIP[0] != 0) {
        prefs.getBytes("static_gw", &staticGateway, 4);
        prefs.getBytes("static_sn", &staticSubnet, 4);
        prefs.getBytes("static_dns", &staticDNS, 4);
        staticIPConfigured = true;
        Serial.printf("[DHCP] IP fixo carregado: %s\n", staticIP.toString().c_str());
    }
    prefs.end();
}

void clearStaticIPConfig() {
    Preferences prefs;
    prefs.begin("app", false);
    prefs.remove("static_ip"); prefs.remove("static_gw");
    prefs.remove("static_sn"); prefs.remove("static_dns");
    prefs.end();
    staticIPConfigured = false;
    Serial.println("[DHCP] IP fixo removido do NVS");
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

    loadStaticIPConfig();
    if (staticIPConfigured) {
        WiFi.config(staticIP, staticGateway, staticSubnet, staticDNS);
        Serial.printf("[WM] IP fixo aplicado: %s\n", staticIP.toString().c_str());
    }

    Serial.println("[WM] Iniciando portal WiFiManager...");

    bool res = wm.autoConnect(globalHostname);

    if (!res) {
        Serial.println("[WM] Falha na conexao WiFi - modo AP ativo");
        return false;
    }

    Serial.println("[WM] WiFi conectado com sucesso!");
    Serial.print("[WM] IP: ");
    Serial.println(WiFi.localIP());

    saveStaticIPConfig();

    return true;
}

void setupMDNS() {
    static bool mdnsStarted = false;
    if (mdnsStarted) return;
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("[mDNS] WiFi nao conectado, abortando...");
        return;
    }

    if (MDNS.begin(globalHostname)) {
        mdnsStarted = true;
        Serial.printf("[mDNS] Respondendo em: http://%s.local:8181\n", globalHostname);
        MDNS.addService("http", "tcp", 8181);
    } else {
        Serial.println("[mDNS] FALHA!");
    }
}

#endif
