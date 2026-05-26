#ifndef SENSORMANAGER_H
#define SENSORMANAGER_H

#include <Arduino.h>
#include <DHT.h>
#include <Adafruit_BMP085.h>

extern DHT dht;
extern Adafruit_BMP085 bmp;

void readSensors();

#endif
