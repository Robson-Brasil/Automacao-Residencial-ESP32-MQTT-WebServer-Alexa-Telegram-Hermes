/**
 * @file HomeAssistantDiscovery.h
 * @brief MQTT Discovery para Home Assistant — auto-descoberta de relés e sensores.
 */
#ifndef HOMEASSISTANTDISCOVERY_H
#define HOMEASSISTANTDISCOVERY_H

#include "TopicosMQTT.h"

#define HA_DEVICE_ID          "esp32_quarto_robson_v2"
#define HA_DEVICE_NAME        "ESP32 Quarto Robson"
#define HA_DEVICE_MANUFACTURER "Robson Brasil"
#define HA_DEVICE_MODEL       "ESP32 WROOM-32"
#define HA_DEVICE_SW_VERSION  "1.0.0"
#define HA_DISCOVERY_PREFIX   "homeassistant"

static void _publishHA(PubSubClient &client, const String &topic, const String &payload) {
  Serial.print("[HA] Len=");
  Serial.print(payload.length());
  Serial.print(" -> ");
  bool ok = client.publish(topic.c_str(), payload.c_str(), true);
  Serial.println(ok ? "OK" : "FAIL");
}

void publishHADiscovery(PubSubClient &client) {
  const String dev = "\"device\":{\"identifiers\":[\"" HA_DEVICE_ID "\"],\"name\":\"" HA_DEVICE_NAME "\",\"manufacturer\":\"" HA_DEVICE_MANUFACTURER "\",\"model\":\"" HA_DEVICE_MODEL "\",\"sw_version\":\"" HA_DEVICE_SW_VERSION "\"}";

  Serial.println("[HA Discovery] Publicando auto-descoberta...");

  // ── SWITCHES (Relés 1-8) ──
  const char *relayNames[] = {
    "Interruptor 1", "Interruptor 2", "Interruptor 3", "Interruptor 4",
    "Interruptor 5", "Interruptor 6", "Interruptor 7", "Interruptor 8"
  };
  const char *relayIds[] = {
    "relay_1", "relay_2", "relay_3", "relay_4",
    "relay_5", "relay_6", "relay_7", "relay_8"
  };
  const char *relayCmdTopics[] = { sub1, sub2, sub3, sub4, sub5, sub6, sub7, sub8 };
  const char *relayStatTopics[] = { pub1, pub2, pub3, pub4, pub5, pub6, pub7, pub8 };

  // ── SWITCH GERAL (Todos) ──
  _publishHA(client,
    String(HA_DISCOVERY_PREFIX) + "/switch/" HA_DEVICE_ID "/relay_todos/config",
    "{\"name\":\"Todos os Interruptores\",\"state_topic\":\"" + String(pub0) + "\",\"command_topic\":\"" + String(sub0) + "\",\"payload_on\":\"1\",\"payload_off\":\"0\",\"state_on\":\"1\",\"state_off\":\"0\",\"unique_id\":\"" HA_DEVICE_ID "_relay_todos\"," + dev + "}"
  );
  delay(200);

  for (int i = 0; i < 8; i++) {
    String topic = String(HA_DISCOVERY_PREFIX) + "/switch/" HA_DEVICE_ID "/" + relayIds[i] + "/config";
    String payload = "{"
      "\"name\":\"" + String(relayNames[i]) + "\","
      "\"state_topic\":\"" + String(relayStatTopics[i]) + "\","
      "\"command_topic\":\"" + String(relayCmdTopics[i]) + "\","
      "\"payload_on\":\"1\","
      "\"payload_off\":\"0\","
      "\"state_on\":\"1\","
      "\"state_off\":\"0\","
      "\"unique_id\":\"" HA_DEVICE_ID "_" + String(relayIds[i]) + "\","
      + dev +
      "}";
    _publishHA(client, topic, payload);
    delay(200);
  }

  // ── SENSORES DHT22 ──

  // Temperatura
  _publishHA(client,
    String(HA_DISCOVERY_PREFIX) + "/sensor/" HA_DEVICE_ID "/temperatura/config",
    "{\"name\":\"Temperatura\",\"state_topic\":\"" + String(pub9) + "\",\"unit_of_measurement\":\"°C\",\"device_class\":\"temperature\",\"state_class\":\"measurement\",\"unique_id\":\"" HA_DEVICE_ID "_temperatura\"," + dev + "}"
  );
  delay(200);

  // Umidade
  _publishHA(client,
    String(HA_DISCOVERY_PREFIX) + "/sensor/" HA_DEVICE_ID "/umidade/config",
    "{\"name\":\"Umidade\",\"state_topic\":\"" + String(pub10) + "\",\"unit_of_measurement\":\"%\",\"device_class\":\"humidity\",\"state_class\":\"measurement\",\"unique_id\":\"" HA_DEVICE_ID "_umidade\"," + dev + "}"
  );
  delay(200);

  // Sensação Térmica
  _publishHA(client,
    String(HA_DISCOVERY_PREFIX) + "/sensor/" HA_DEVICE_ID "/sensacao_termica/config",
    "{\"name\":\"Sensacao Termica\",\"state_topic\":\"" + String(pub11) + "\",\"unit_of_measurement\":\"°C\",\"device_class\":\"temperature\",\"state_class\":\"measurement\",\"unique_id\":\"" HA_DEVICE_ID "_sensacao_termica\"," + dev + "}"
  );
  delay(200);

  // Ponto de Orvalho
  _publishHA(client,
    String(HA_DISCOVERY_PREFIX) + "/sensor/" HA_DEVICE_ID "/ponto_orvalho/config",
    "{\"name\":\"Ponto de Orvalho\",\"state_topic\":\"" + String(pub17) + "\",\"unit_of_measurement\":\"°C\",\"device_class\":\"temperature\",\"state_class\":\"measurement\",\"unique_id\":\"" HA_DEVICE_ID "_ponto_orvalho\"," + dev + "}"
  );
  delay(200);

  // ── SENSORES BMP180 ──

  // Temperatura BMP180
  _publishHA(client,
    String(HA_DISCOVERY_PREFIX) + "/sensor/" HA_DEVICE_ID "/bmp_temperatura/config",
    "{\"name\":\"Temperatura BMP180\",\"state_topic\":\"" + String(pub12) + "\",\"unit_of_measurement\":\"°C\",\"device_class\":\"temperature\",\"state_class\":\"measurement\",\"unique_id\":\"" HA_DEVICE_ID "_bmp_temperatura\"," + dev + "}"
  );
  delay(200);

  // Pressão Atmosférica Real
  _publishHA(client,
    String(HA_DISCOVERY_PREFIX) + "/sensor/" HA_DEVICE_ID "/bmp_pressao_real/config",
    "{\"name\":\"Pressao Atmosferica\",\"state_topic\":\"" + String(pub13) + "\",\"unit_of_measurement\":\"hPa\",\"device_class\":\"pressure\",\"state_class\":\"measurement\",\"unique_id\":\"" HA_DEVICE_ID "_bmp_pressao_real\"," + dev + "}"
  );
  delay(200);

  // Pressão Nível do Mar
  _publishHA(client,
    String(HA_DISCOVERY_PREFIX) + "/sensor/" HA_DEVICE_ID "/bmp_pressao_mar/config",
    "{\"name\":\"Pressao Nivel do Mar\",\"state_topic\":\"" + String(pub14) + "\",\"unit_of_measurement\":\"hPa\",\"device_class\":\"pressure\",\"state_class\":\"measurement\",\"unique_id\":\"" HA_DEVICE_ID "_bmp_pressao_mar\"," + dev + "}"
  );
  delay(200);

  // Altitude Real
  _publishHA(client,
    String(HA_DISCOVERY_PREFIX) + "/sensor/" HA_DEVICE_ID "/bmp_altitude_real/config",
    "{\"name\":\"Altitude Real\",\"state_topic\":\"" + String(pub15) + "\",\"unit_of_measurement\":\"m\",\"device_class\":\"distance\",\"state_class\":\"measurement\",\"unique_id\":\"" HA_DEVICE_ID "_bmp_altitude_real\"," + dev + "}"
  );
  delay(200);

  // Altitude Nível do Mar
  _publishHA(client,
    String(HA_DISCOVERY_PREFIX) + "/sensor/" HA_DEVICE_ID "/bmp_altitude_mar/config",
    "{\"name\":\"Altitude Nivel do Mar\",\"state_topic\":\"" + String(pub16) + "\",\"unit_of_measurement\":\"m\",\"device_class\":\"distance\",\"state_class\":\"measurement\",\"unique_id\":\"" HA_DEVICE_ID "_bmp_altitude_mar\"," + dev + "}"
  );
  delay(200);

  Serial.println("[HA Discovery] Auto-descoberta concluida.");
}

#endif // HOMEASSISTANTDISCOVERY_H
