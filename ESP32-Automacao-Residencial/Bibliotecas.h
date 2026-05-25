/**
 * @file Bibliotecas.h
 * @brief Gerenciamento centralizado de dependências do projeto.
 * Inclui bibliotecas para periféricos (DHT, BMP), comunicação (WiFi, MQTT) e núcleo (FreeRTOS).
 * 
 * Este arquivo serve como ponto único de inclusão para todas as dependências externas
 * necessárias para o funcionamento do firmware, facilitando a manutenção e portabilidade.
 */
#ifndef BIBLIOTECAS_H
#define BIBLIOTECAS_H

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include <SinricPro.h>
#include <SinricProSwitch.h>
#include <SinricProTemperaturesensor.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085.h>
#include <Arduino.h>
#include <TimeLib.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <WebServer.h>

#include "OTAConfig.h"
#include "RemoteDebug.h"

#endif // BIBLIOTECAS_H
