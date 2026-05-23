#ifndef REMOTEDEBUG_H
#define REMOTEDEBUG_H

#include <Arduino.h>
#include <WiFi.h>

// Referencia o cliente Telnet global definido no arquivo principal .ino
extern WiFiClient telnetClient;

/**
 * @class DualSerialClass
 * @brief Classe que herda de Print para interceptar as saídas da porta Serial
 * e envia-las tanto para a UART física (cabo USB) quanto para o cliente Wi-Fi (Telnet/Raw).
 */
class DualSerialClass : public Print {
public:
  // Inicialização (repassa para o Serial físico)
  void begin(unsigned long baud) {
    Serial.begin(baud);
  }
  
  // Leitura (repassa para o Serial físico)
  int available() {
    return Serial.available();
  }
  
  int read() {
    return Serial.read();
  }
  
  int peek() {
    return Serial.peek();
  }

  void flush() {
    Serial.flush();
  }
  
  // Escrita de um único caractere
  size_t write(uint8_t c) override {
    // Se o cliente Telnet/Raw estiver conectado, envia para ele
    if (telnetClient && telnetClient.connected()) {
      telnetClient.write(c);
    }
    // Sempre envia para o Monitor Serial físico
    return Serial.write(c);
  }
  
  // Escrita de um buffer
  size_t write(const uint8_t *buffer, size_t size) override {
    if (telnetClient && telnetClient.connected()) {
      telnetClient.write(buffer, size);
    }
    return Serial.write(buffer, size);
  }
};

// Declara a instância global que será definida no arquivo .ino
extern DualSerialClass DualSerial;

// Este é o "pulo do gato": substitui todas as chamadas "Serial" por "DualSerial"
#define Serial DualSerial

#endif // REMOTEDEBUG_H
