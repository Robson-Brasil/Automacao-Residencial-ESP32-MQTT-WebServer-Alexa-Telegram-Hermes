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
extern void logRelayAction(const char* source, int relayNum, bool state);
extern bool* relayStatePtrs[8];
extern void setRelayByIndex(int idx, bool state, const char* source);
extern void setAllRelays(bool state, const char* source);
extern String getControlKeyboard();

WiFiClientSecure telegramClient;
UniversalTelegramBot *telegramBot = nullptr;

const unsigned long BOT_MTBS = 1000;
unsigned long bot_lasttime = 0;
bool telegramInitialized = false;

String lastTelegramMessage = "";

const String relayNames[8] = {"Varanda", "Bancada", "Sala", "Cozinha", "Quintal", "Varão", "Robson", "Kinha"};

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
  keyboard += "[\"" + String(RelayState2 ? "Bancada ⭕ DESLIGAR" : "Bancada ⚡ LIGAR") + "\", \"" + String(RelayState6 ? "Varão ⭕ DESLIGAR" : "Varão ⚡ LIGAR") + "\"],";
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
  static const char* TELEGRAM_ROOT_CA = \
    "-----BEGIN CERTIFICATE-----\n" \
    "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\n" \
    "-----END CERTIFICATE-----\n";
  telegramClient.setCACert(TELEGRAM_ROOT_CA);
  
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
  if (RelayState6) { onRelays += "\n⚡ Varão"; anyOn = true; }
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
    for (int i = 0; i < 8; i++) {
      if (text.startsWith(relayNames[i])) {
        setRelayByIndex(i, !(*relayStatePtrs[i]), "Telegram");
        updated = true;
        break;
      }
    }
    if (!updated) {
      if (text == "💡 LIGAR TUDO" || text == "/on") {
        setAllRelays(true, "Telegram");
        feedback = "✅ Todos os relés ligados!";
        updated = true;
      }
      else if (text == "🔌 DESLIGAR TUDO" || text == "/off") {
        setAllRelays(false, "Telegram");
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