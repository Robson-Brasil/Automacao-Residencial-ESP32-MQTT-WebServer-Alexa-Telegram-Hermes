#ifndef OTACONFIG_H
#define OTACONFIG_H

#include <ArduinoOTA.h>

#define OTA_PASSWORD "S3nh@S3gur@"
#define TELNET_PASSWORD OTA_PASSWORD

void setupOTA();

#endif // OTACONFIG_H