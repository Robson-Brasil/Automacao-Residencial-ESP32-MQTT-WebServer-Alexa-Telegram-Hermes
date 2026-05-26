/**
 * @file HomeAssistantDiscovery.h
 * @brief MQTT Discovery para Home Assistant — Declarações apenas.
 */
#ifndef HOMEASSISTANTDISCOVERY_H
#define HOMEASSISTANTDISCOVERY_H

#include <PubSubClient.h>

#define HA_DEVICE_ID          "esp32_quarto_robson_v2"
#define HA_DEVICE_NAME        "ESP32 Quarto Robson"
#define HA_DEVICE_MANUFACTURER "Robson Brasil"
#define HA_DEVICE_MODEL       "ESP32 WROOM-32"
#define HA_DEVICE_SW_VERSION  "1.0.0"
#define HA_DISCOVERY_PREFIX   "homeassistant"

void publishHADiscovery(PubSubClient &client);

#endif // HOMEASSISTANTDISCOVERY_H
