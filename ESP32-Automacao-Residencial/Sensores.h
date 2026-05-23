/**
 * @file Sensores.h
 * @brief Protótipos para leitura e inicialização de sensores DHT e BMP.
 */
#ifndef SENSORES_H
#define SENSORES_H

void readSensors();
void publishSensorData();
void publishRelayStates();
void readDHTSensor();
void readBMP180Sensor();

#endif // SENSORES_H
