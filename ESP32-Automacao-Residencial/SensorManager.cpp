#include "SensorManager.h"
#include "VariaveisGlobais.h"
#include "GPIOs.h"
#include "ConstantesTempo.h"
#include "RemoteDebug.h"

DHT dht(DHTPIN, DHTTYPE);
Adafruit_BMP085 bmp;

const float pressaoNivelMar = 1012.0;
const float altitudeNivelMar = 92.0;

void readSensors() {
  unsigned long currentTimeDHT = millis();
  unsigned long currentTimeBMP180 = millis();

  if (currentTimeDHT - lastMsgDHT > DHT_READ_INTERVAL) {
    lastMsgDHT = currentTimeDHT;

    float temp_data = dht.readTemperature();
    float hum_data = dht.readHumidity();

    if (isnan(temp_data) || isnan(hum_data)) {
      Serial.println("Falha na leitura do sensor DHT22!");
      return;
    }

    dtostrf(temp_data, 6, 2, str_temp_data);
    dtostrf(hum_data, 6, 2, str_hum_data);

    float a = 17.27;
    float b = 237.7;
    float gamma = (a * temp_data) / (b + temp_data) + log(hum_data / 100.0);
    float dewpoint = (b * gamma) / (a - gamma);
    dtostrf(dewpoint, 6, 2, str_dewpoint_data);

    float tempF_data = dht.readTemperature(true);
    if (!isnan(tempF_data)) {
      dtostrf(tempF_data, 6, 2, str_tempF_data);

      float tempterm_data = dht.computeHeatIndex(tempF_data, hum_data);
      if (!isnan(tempterm_data)) {
        tempterm_data = dht.convertFtoC(tempterm_data);
        dtostrf(tempterm_data, 6, 2, str_tempterm_data);
      }
    }

    pendingSensorMqttUpdate = true;
    pendingSensorSinricUpdate = true;

    Serial.println("\n=== Leitura DHT22 ===");
    Serial.print("Temperatura: ");
    Serial.print(str_temp_data);
    Serial.println(" °C");
    Serial.print("Umidade: ");
    Serial.print(str_hum_data);
    Serial.println(" %");
    Serial.print("Sensação Térmica: ");
    Serial.print(str_tempterm_data);
    Serial.println(" °C");
    Serial.print("Ponto de Orvalho: ");
    Serial.print(str_dewpoint_data);
    Serial.println(" °C");
  }

  if (currentTimeBMP180 - lastMsgBMP180 > BMP_READ_INTERVAL) {
    lastMsgBMP180 = currentTimeBMP180;

    pressure = bmp.readPressure();
    if (pressure != 0) {
      seaLevelPressureCache = bmp.readSealevelPressure(pressaoNivelMar);

      altitudeRealCache = bmp.readAltitude(pressaoNivelMar * 100);
      if (!isnan(altitudeRealCache)) {
        float tempBMP = bmp.readTemperature();
        temperature = tempBMP;

        altitude = altitudeRealCache + altitudeNivelMar;
        altitudeTotal = altitude;

        pendingSensorMqttUpdate = true;

        Serial.println("\n=== Leitura BMP180 ===");
        Serial.print("Temperatura: ");
        Serial.print(temperature);
        Serial.println(" °C");
        Serial.print("Pressão: ");
        Serial.print(pressure / 100.0);
        Serial.println(" hPa");
        Serial.print("Pressão nível do mar: ");
        Serial.print(seaLevelPressureCache / 100.0);
        Serial.println(" hPa");
        Serial.print("Altitude real: ");
        Serial.print(altitudeRealCache);
        Serial.println(" m");
        Serial.print("Altitude total: ");
        Serial.print(altitude);
        Serial.println(" m");
      }
    } else {
      Serial.println("Erro na leitura do sensor BMP180");
    }
  }
}
