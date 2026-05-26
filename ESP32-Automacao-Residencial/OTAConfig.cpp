#include "OTAConfig.h"
#include <ArduinoOTA.h>
#include <WiFi.h>
#include "VariaveisGlobais.h"

uint32_t last_ota_time = 0;

void setupOTA() {
  // Configurações de Identificação
  ArduinoOTA.setPort(3232);
  ArduinoOTA.setHostname(globalHostname);
  ArduinoOTA.setPassword(OTA_PASSWORD);

  ArduinoOTA
    .onStart([]() {
      // Pausa a tarefa de sensores para evitar conflitos na Flash SPI
      if (hTaskSensores != NULL) {
        vTaskSuspend(hTaskSensores);
      }
      
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH) {
        type = "sketch";
      } else {
        type = "filesystem"; // Para SPIFFS ou LittleFS
      }
      Serial.println("\n[OTA] Iniciando atualizacao: " + type);
    })
    .onEnd([]() {
      // Retoma a tarefa (opcional, já que vai reiniciar)
      if (hTaskSensores != NULL) {
        vTaskResume(hTaskSensores);
      }
      
      Serial.println("\n[OTA] Sucesso! Gravacao concluida.");
      Serial.println("[OTA] Reiniciando hardware agora...");
      
      // COMANDOS ADICIONADOS PARA CORRIGIR O SEU ESTRANHAMENTO:
      Serial.flush(); // Força a saída de todos os dados do buffer serial
      delay(1000);    // Aguarda 1 segundo para o PC processar a mensagem antes do reset
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      // Atualiza o progresso no serial a cada 500ms
      if (millis() - last_ota_time > 500) {
        Serial.printf("Progresso: %u%%\r", (progress / (total / 100)));
        last_ota_time = millis();
      }
    })
    .onError([](ota_error_t error) {
      if (hTaskSensores != NULL) {
        vTaskResume(hTaskSensores);
      }
      
      Serial.printf("\n[OTA] Erro [%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Falha na Autenticacao");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Falha no Inicio (Begin)");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Falha de Conexao");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Falha na Recepcao");
      else if (error == OTA_END_ERROR) Serial.println("Falha no Final (End)");
    });

  ArduinoOTA.begin();
  Serial.println("[OTA] Servico pronto e aguardando...");
}