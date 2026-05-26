#include "MQTTHandler.h"
#include "RelayManager.h"
#include "VariaveisGlobais.h"
#include "GPIOs.h"
#include "TopicosMQTT.h"
#include "TelegramBotESP32.h"
#include "RemoteDebug.h"
#include "LoginsSenhas.h"
#include "ConfigMQTT.h"
#include "MQTTParametros.h"
#include "ConstantesTempo.h"
#include "HomeAssistantDiscovery.h"

WiFiClient espClient;
PubSubClient mqttClient(espClient);

void setupMQTT() {
  mqttClient.setServer(BrokerMQTT, MQTT_PORT);
  mqttClient.setCallback(mqttCallback);
  mqttClient.setKeepAlive(MQTT_KEEPALIVE);
  mqttClient.setBufferSize(2048);
}

void reconnectMQTT() {
  if (!mqttClient.connected() && WiFi.status() == WL_CONNECTED) {
    Serial.println("Tentando conectar ao MQTT...");
    if (mqttClient.connect(clientID, LoginDoMQTT, SenhaMQTT)) {
      Serial.println("==============================");
      Serial.println("MQTT CONECTADO!");
      Serial.print("IP do ESP32: ");
      Serial.println(WiFi.localIP());
      Serial.println("==============================");
      mqttConnected = true;
      pendingHADiscovery = true;

      mqttClient.subscribe(sub0);
      mqttClient.subscribe(sub1);
      mqttClient.subscribe(sub2);
      mqttClient.subscribe(sub3);
      mqttClient.subscribe(sub4);
      mqttClient.subscribe(sub5);
      mqttClient.subscribe(sub6);
      mqttClient.subscribe(sub7);
      mqttClient.subscribe(sub8);
      mqttClient.subscribe(sub9);
      publishRelayStates();
    } else {
      Serial.print("Falha na conexão MQTT, rc=");
      Serial.println(mqttClient.state());
      mqttConnected = false;
    }
  }
}

void mqttCallback(char *topic, byte *payload, unsigned int length) {
  if (length > 255) return;

  char messageBuf[256];
  memcpy(messageBuf, payload, length);
  messageBuf[length] = '\0';

  String messageTemp(messageBuf);
  messageTemp.trim();

  Serial.print("Mensagem recebida [");
  Serial.print(topic);
  Serial.print("] ");
  Serial.println(messageTemp);

  bool newState = messageTemp.startsWith("1");

  if (strcmp(topic, sub0) == 0) {
    if (Todos != newState) {
      Todos = newState;
      const int pins[] = {RelayPin1, RelayPin2, RelayPin3, RelayPin4,
                          RelayPin5, RelayPin6, RelayPin7, RelayPin8};
      for (int i = 0; i < 8; i++) digitalWrite(pins[i], !Todos);
      logRelayAction("MQTT", -1, Todos);
      RelayState1 = RelayState2 = RelayState3 = RelayState4 = RelayState5 =
          RelayState6 = RelayState7 = RelayState8 = Todos;
      for (int r = 1; r <= 8; r++) saveRelayState(r, Todos);
      notifyTelegramStateChange("MQTT", -1, Todos);
      pendingSinricProUpdate = true;
    }
    return;
  }

  for (int i = 0; i < 8; i++) {
    if (strcmp(topic, relayTopics[i]) == 0) {
      if (*relayStatePtrs[i] != newState) {
        *relayStatePtrs[i] = newState;
        digitalWrite(relayPinNums[i], !newState);
        logRelayAction("MQTT", i, newState);
        saveRelayState(i + 1, newState);
        notifyTelegramStateChange("MQTT", i, newState);
        pendingSinricProUpdate = true;
      }
      return;
    }
  }

  if (strcmp(topic, sub9) == 0) {
    messageTemp.toUpperCase();
    String cmd = messageTemp;

    if (cmd == "SENSORES") {
      Serial.println("✅ Hermes SENSORES");
      pendingSensorMqttUpdate = true;
    }
    else if (cmd == "STATUS") {
      char statusJson[512];
      snprintf(statusJson, sizeof(statusJson),
        "{\"estados\": {\"Varanda\": \"%s\", \"Bancada\": \"%s\", \"Sala\": \"%s\", \"Cozinha\": \"%s\", \"Quintal\": \"%s\", \"Val\": \"%s\", \"Robson\": \"%s\", \"Kinha\": \"%s\", \"TUDO\": \"%s\"}}",
        RelayState1 ? "ON" : "OFF", RelayState2 ? "ON" : "OFF", RelayState3 ? "ON" : "OFF", RelayState4 ? "ON" : "OFF",
        RelayState5 ? "ON" : "OFF", RelayState6 ? "ON" : "OFF", RelayState7 ? "ON" : "OFF", RelayState8 ? "ON" : "OFF",
        Todos ? "ON" : "OFF");
      mqttClient.publish(pub18, statusJson);
      Serial.println("✅ Hermes STATUS: " + String(statusJson));
    }

    if (cmd.startsWith("LIGAR") && cmd.indexOf("TUDO") != -1) {
      setAllRelays(true, "HERMES");
      pendingSinricProUpdate = true;
    }
    else if (cmd.startsWith("DESLIGAR") && cmd.indexOf("TUDO") != -1) {
      setAllRelays(false, "HERMES");
      pendingSinricProUpdate = true;
    }
    else if (cmd.startsWith("LIGAR:")) {
      String item = cmd.substring(6); item.trim();
      int idx = findRelayByName(item);
      if (idx >= 0) {
        setRelayByIndex(idx, true, "HERMES");
        pendingSinricProUpdate = true;
        Todos = true;
        for (int i = 0; i < 8; i++) if (!*relayStatePtrs[i]) { Todos = false; break; }
      }
    }
    else if (cmd.startsWith("DESLIGAR:")) {
      String item = cmd.substring(9); item.trim();
      int idx = findRelayByName(item);
      if (idx >= 0) {
        setRelayByIndex(idx, false, "Hermes");
        pendingSinricProUpdate = true;
        Todos = false;
        for (int i = 0; i < 8; i++) if (*relayStatePtrs[i]) { Todos = true; break; }
      }
    }
    pendingMqttUpdate = true;
  }
}

void publishRelayStates() {
  mqttClient.publish(pub0, Todos ? "1" : "0", true);
  mqttClient.publish(pub1, RelayState1 ? "1" : "0", true);
  mqttClient.publish(pub2, RelayState2 ? "1" : "0", true);
  mqttClient.publish(pub3, RelayState3 ? "1" : "0", true);
  mqttClient.publish(pub4, RelayState4 ? "1" : "0", true);
  mqttClient.publish(pub5, RelayState5 ? "1" : "0", true);
  mqttClient.publish(pub6, RelayState6 ? "1" : "0", true);
  mqttClient.publish(pub7, RelayState7 ? "1" : "0", true);
  mqttClient.publish(pub8, RelayState8 ? "1" : "0", true);
}

void publishSensorData() {
  mqttClient.publish(pub9, str_temp_data, true);
  mqttClient.publish(pub10, str_hum_data, true);
  mqttClient.publish(pub11, str_tempterm_data, true);

  char tempStr[10];
  dtostrf(temperature, 6, 2, tempStr);
  mqttClient.publish(pub12, tempStr, true);

  dtostrf(pressure / 100.0, 6, 2, tempStr);
  mqttClient.publish(pub13, tempStr, true);

  dtostrf(seaLevelPressureCache / 100.0, 6, 2, tempStr);
  mqttClient.publish(pub14, tempStr, true);

  dtostrf(altitudeRealCache, 6, 2, tempStr);
  mqttClient.publish(pub15, tempStr, true);

  dtostrf(altitude, 6, 2, tempStr);
  mqttClient.publish(pub16, tempStr, true);
  mqttClient.publish(pub17, str_dewpoint_data, true);
}
