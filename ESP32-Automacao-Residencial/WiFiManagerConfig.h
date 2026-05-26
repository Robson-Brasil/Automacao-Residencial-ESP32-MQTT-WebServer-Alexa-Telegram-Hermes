#ifndef WIFIMANAGERCONFIG_H
#define WIFIMANAGERCONFIG_H

#include <ESPmDNS.h>
#include <Preferences.h>
#include "VariaveisGlobais.h"
#include "LoginsSenhas.h"

// ============================================================
// Migracao NVS: unifica namespaces legados em "app"
// ============================================================
void migrateNVS() {
    Preferences app;
    app.begin("app", false);
    if (app.getBool("_migrated", false)) { app.end(); return; }

    Serial.println("[NVS] Migrando dados legados para namespace 'app'...");

    Preferences old;

    old.begin("relay states", true);
    for (int i = 1; i <= 8; i++) {
        String k = "RelayState" + String(i);
        bool v1 = old.getBool(k.c_str(), true);
        bool v2 = old.getBool(k.c_str(), false);
        if (v1 == v2) app.putBool(k.c_str(), v1);
    }
    {
        bool v1 = old.getBool("needsReset", true);
        bool v2 = old.getBool("needsReset", false);
        if (v1 == v2) app.putBool("needsReset", v1);
    }
    for (int i = 1; i <= 8; i++) {
        String nk = "RelayName" + String(i);
        String nv = old.getString(nk.c_str(), "");
        if (nv.length() > 0) app.putString(nk.c_str(), nv);
    }
    old.end();

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

    auto eraseNS = [](const char* ns) { Preferences p; p.begin(ns, false); p.clear(); p.end(); };
    eraseNS("relay states"); eraseNS("mqtt_config"); eraseNS("wifi-ip"); eraseNS("credenciais");

    Serial.println("[NVS] Migracao concluida. Namespaces legados apagados.");
}

// ============================================================
// Credenciais Telegram/SinricPro via NVS
// ============================================================
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

// ============================================================
// mDNS
// ============================================================
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
