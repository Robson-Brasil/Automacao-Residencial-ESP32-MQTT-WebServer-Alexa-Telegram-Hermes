/**
 * @file TelegramBotESP32.h
 * @brief Bot do Telegram com Teclado Interativo (Botões)
 */
#ifndef TELEGRAMBOTESP32_H
#define TELEGRAMBOTESP32_H

#include "Bibliotecas.h"
#include "VariaveisGlobais.h"
#include <time.h>

extern void saveRelayState(int relayNum, bool state);
extern void publishRelayStates();
extern void applyRelayStatesToPins();
extern String getControlKeyboard();

WiFiClientSecure telegramClient;
UniversalTelegramBot *telegramBot = nullptr;

const unsigned long BOT_MTBS = 1000;
unsigned long bot_lasttime = 0;
bool telegramInitialized = false;

String lastTelegramMessage = "";

const String relayNames[8] = {"Varanda", "Bancada", "Sala", "Cozinha", "Quintal", "Varão", "Robson", "Kinha"};

/**
 * @brief Adiciona uma notificação à fila para envio seguro no Core 0.
 */
/**
 * @brief Adiciona uma notificação à fila para envio seguro no Core 0.
 */
void notifyTelegramStateChange(const String& source, int relayNum = -1, bool relayState = false) {
  if (telegramQueue == NULL) return;
  TelegramNotification notif;
  strncpy(notif.source, source.c_str(), sizeof(notif.source) - 1);
  notif.source[sizeof(notif.source) - 1] = '\0';
  notif.relayNum = relayNum;
  notif.relayState = relayState;
  xQueueSend(telegramQueue, &notif, 0);
}

/**
 * @brief Envia a notificação pendente de fato (Chamado apenas pela TaskConexoes no Core 0).
 */
void processPendingTelegramNotification(const TelegramNotification& notif) {
  if (!telegramInitialized || telegramBot == nullptr) return;

  String msg = "🔔 Estado atualizado via " + String(notif.source);

  if (notif.relayNum >= 0 && notif.relayNum < 8) {
    msg += "\n📌 " + relayNames[notif.relayNum] + ": " + String(notif.relayState ? "⚡ LIGADO" : "⭕ DESLIGADO");
  } else if (notif.relayNum == -1) {
    msg += "\n📌 TODOS: " + String(notif.relayState ? "⚡ LIGADOS" : "⭕ DESLIGADOS");
  }

  msg += "\n\nSelecione:";
  String keyboard = getControlKeyboard();

  telegramBot->sendMessageWithReplyKeyboard(CHAT_ID, msg, "", keyboard, true);
}

// Função para gerar o JSON do teclado com o status atualizado
String getControlKeyboard() {
  String keyboard = "[";
  // Linha 1: R1 e R5
  keyboard += "[\"" + String(RelayState1 ? "Varanda ⭕ DESLIGAR" : "Varanda ⚡ LIGAR") + "\", \"" + String(RelayState5 ? "Quintal ⭕ DESLIGAR" : "Quintal ⚡ LIGAR") + "\"],";
  // Linha 2: R2 e R6
  keyboard += "[\"" + String(RelayState2 ? "Bancada ⭕ DESLIGAR" : "Bancada ⚡ LIGAR") + "\", \"" + String(RelayState6 ? "Val ⭕ DESLIGAR" : "Val ⚡ LIGAR") + "\"],";
  // Linha 3: R3 e R7
  keyboard += "[\"" + String(RelayState3 ? "Sala ⭕ DESLIGAR" : "Sala ⚡ LIGAR") + "\", \"" + String(RelayState7 ? "Robson ⭕ DESLIGAR" : "Robson ⚡ LIGAR") + "\"],";
  // Linha 4: R4 e R8
  keyboard += "[\"" + String(RelayState4 ? "Cozinha ⭕ DESLIGAR" : "Cozinha ⚡ LIGAR") + "\", \"" + String(RelayState8 ? "Kinha ⭕ DESLIGAR" : "Kinha ⚡ LIGAR") + "\"],";
  // Linha 5: Comandos Gerais
  keyboard += "[\"💡 LIGAR TUDO\", \"🔌 DESLIGAR TUDO\"],";
  // Linha 6: Sensores
  keyboard += "[\"📊 VER SENSORES\"]";
  keyboard += "]";
  return keyboard;
}

void sendMainMenu(String chat_id, String message) {
  String keyboard = getControlKeyboard();
  telegramBot->sendMessageWithReplyKeyboard(chat_id, message, "", keyboard, true);
}

void initTelegram() {
  if (telegramInitialized) return;
  
  if (WiFi.status() != WL_CONNECTED) return;

  configTime(0, 0, "pool.ntp.org", "time.nist.gov");
  telegramClient.setInsecure();
  
  if (telegramBot != nullptr) delete telegramBot;
  telegramBot = new UniversalTelegramBot(BOT_TOKEN, telegramClient);
  telegramInitialized = true;
  bot_lasttime = millis();
  
  Serial.println("[Telegram] Bot Online!");
  
  String bootMsg = "🏠 *Automação Residencial*\nPainel de controle pronto!";
  String onRelays = "\n\n⚠️ *Atenção! Os seguintes relés já iniciaram LIGADOS:*";
  bool anyOn = false;
  
  if (RelayState1) { onRelays += "\n⚡ Varanda"; anyOn = true; }
  if (RelayState2) { onRelays += "\n⚡ Bancada"; anyOn = true; }
  if (RelayState3) { onRelays += "\n⚡ Sala"; anyOn = true; }
  if (RelayState4) { onRelays += "\n⚡ Cozinha"; anyOn = true; }
  if (RelayState5) { onRelays += "\n⚡ Quintal"; anyOn = true; }
  if (RelayState6) { onRelays += "\n⚡ Val"; anyOn = true; }
  if (RelayState7) { onRelays += "\n⚡ Robson"; anyOn = true; }
  if (RelayState8) { onRelays += "\n⚡ Kinha"; anyOn = true; }
  
  if (anyOn) {
    bootMsg += onRelays;
  }
  
  sendMainMenu(String(CHAT_ID), bootMsg);
}
void handleTelegramMessages(int numNewMessages) {
  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = telegramBot->messages[i].chat_id;
    String text = telegramBot->messages[i].text;
    
    if (chat_id != String(CHAT_ID)) {
      telegramBot->sendMessage(chat_id, "Não autorizado.", "");
      continue;
    }

    bool updated = false;
    String feedback = "Comando executado!";

    if (text == "/start" || text == "Voltar") {
      feedback = "🏠 Selecione uma opção:";
      updated = true;
    }
    if (text.startsWith("Varanda")) { RelayState1 = !RelayState1; digitalWrite(RelayPin1, !RelayState1); saveRelayState(1, RelayState1); updated = true; }
    else if (text.startsWith("Bancada")) { RelayState2 = !RelayState2; digitalWrite(RelayPin2, !RelayState2); saveRelayState(2, RelayState2); updated = true; }
    else if (text.startsWith("Sala")) { RelayState3 = !RelayState3; digitalWrite(RelayPin3, !RelayState3); saveRelayState(3, RelayState3); updated = true; }
    else if (text.startsWith("Cozinha")) { RelayState4 = !RelayState4; digitalWrite(RelayPin4, !RelayState4); saveRelayState(4, RelayState4); updated = true; }
    else if (text.startsWith("Quintal")) { RelayState5 = !RelayState5; digitalWrite(RelayPin5, !RelayState5); saveRelayState(5, RelayState5); updated = true; }
    else if (text.startsWith("Val")) { RelayState6 = !RelayState6; digitalWrite(RelayPin6, !RelayState6); saveRelayState(6, RelayState6); updated = true; }
    else if (text.startsWith("Robson")) { RelayState7 = !RelayState7; digitalWrite(RelayPin7, !RelayState7); saveRelayState(7, RelayState7); updated = true; }
    else if (text.startsWith("Kinha")) { RelayState8 = !RelayState8; digitalWrite(RelayPin8, !RelayState8); saveRelayState(8, RelayState8); updated = true; }

    else if (text == "💡 LIGAR TUDO" || text == "/on") {
      RelayState1 = RelayState2 = RelayState3 = RelayState4 = RelayState5 = RelayState6 = RelayState7 = RelayState8 = true;
      digitalWrite(RelayPin1, LOW); digitalWrite(RelayPin2, LOW); digitalWrite(RelayPin3, LOW); digitalWrite(RelayPin4, LOW);
      digitalWrite(RelayPin5, LOW); digitalWrite(RelayPin6, LOW); digitalWrite(RelayPin7, LOW); digitalWrite(RelayPin8, LOW);
      for(int r=1; r<=8; r++) saveRelayState(r, true);
      feedback = "✅ Todos os relés ligada!";
      updated = true;
    }
    else if (text == "🔌 DESLIGAR TUDO" || text == "/off") {
      RelayState1 = RelayState2 = RelayState3 = RelayState4 = RelayState5 = RelayState6 = RelayState7 = RelayState8 = false;
      digitalWrite(RelayPin1, HIGH); digitalWrite(RelayPin2, HIGH); digitalWrite(RelayPin3, HIGH); digitalWrite(RelayPin4, HIGH);
      digitalWrite(RelayPin5, HIGH); digitalWrite(RelayPin6, HIGH); digitalWrite(RelayPin7, HIGH); digitalWrite(RelayPin8, HIGH);
      for(int r=1; r<=8; r++) saveRelayState(r, false);
      feedback = "⭕ Todos os relés desligados!";
      updated = true;
    }
    else if (text == "📊 VER SENSORES" || text == "/sensores") {
      String msg = "📊 *Leituras*\n";
      msg += "🌡️ Temp: " + String(str_temp_data) + " °C\n";
      msg += "🌡️ Sensação: " + String(str_tempterm_data) + " °C\n";
      msg += "💧 Umidade: " + String(str_hum_data) + "%\n";
      msg += "🌫️ Ponto Orvalho: " + String(str_dewpoint_data) + " °C\n";
      msg += "⏲️ Pressão: " + String(pressure/100.0, 1) + " hPa\n";
      msg += "🏔️ Altitude: " + String(altitude, 1) + " m";
      sendMainMenu(chat_id, msg);
      continue;
    }

    if (updated) {
      Todos = RelayState1 && RelayState2 && RelayState3 && RelayState4 && RelayState5 && RelayState6 && RelayState7 && RelayState8;
      publishRelayStates();
      pendingSinricProUpdate = true; // Sincroniza com SinricPro (Alexa/Google Home)
      telegramBot->sendMessageWithReplyKeyboard(chat_id, feedback, "", getControlKeyboard(), true);
    }
  }
}

void telegramLoop() {
  if (WiFi.status() != WL_CONNECTED) {
    telegramInitialized = false;
    return;
  }

  if (!telegramInitialized) {
    initTelegram();
    return;
  }
  
  if (millis() - bot_lasttime > BOT_MTBS) {
    int numNewMessages = telegramBot->getUpdates(telegramBot->last_message_received + 1);
    while (numNewMessages > 0) {
      handleTelegramMessages(numNewMessages);
      numNewMessages = telegramBot->getUpdates(telegramBot->last_message_received + 1);
    }
    bot_lasttime = millis();
  }
}

#endif // TELEGRAMBOTESP32_H