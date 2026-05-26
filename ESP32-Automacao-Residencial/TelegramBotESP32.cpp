/**
 * @file TelegramBotESP32.cpp
 * @brief Bot do Telegram — Implementação.
 */
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <time.h>

#include "VariaveisGlobais.h"
#include "LoginsSenhas.h"
#include "TelegramBotESP32.h"
#include "RemoteDebug.h"

WiFiClientSecure telegramClient;
UniversalTelegramBot *telegramBot = nullptr;

unsigned long bot_lasttime = 0;
bool telegramInitialized = false;
String lastTelegramMessage = "";

const String relayNames[8] = {"Varanda", "Bancada", "Sala", "Cozinha", "Quintal", "Val", "Robson", "Kinha"};

void notifyTelegramStateChange(const String& source, int relayNum, bool relayState) {
  if (telegramQueue == NULL) return;
  TelegramNotification notif;
  strncpy(notif.source, source.c_str(), sizeof(notif.source) - 1);
  notif.source[sizeof(notif.source) - 1] = '\0';
  notif.relayNum = relayNum;
  notif.relayState = relayState;
  xQueueSend(telegramQueue, &notif, 0);
}

void processPendingTelegramNotification(const TelegramNotification& notif) {
  if (!telegramInitialized || telegramBot == nullptr) return;

  String msg = "\xF0\x9F\x94\x94 Estado atualizado via " + String(notif.source);

  if (notif.relayNum >= 0 && notif.relayNum < 8) {
    msg += "\n\xF0\x9F\x93\x8C " + relayNames[notif.relayNum] + ": " + String(notif.relayState ? "\xE2\x9A\xA1 LIGADO" : "\xE2\xAD\x95 DESLIGADO");
  } else if (notif.relayNum == -1) {
    msg += "\n\xF0\x9F\x93\x8C TODOS: " + String(notif.relayState ? "\xE2\x9A\xA1 LIGADOS" : "\xE2\xAD\x95 DESLIGADOS");
  }

  msg += "\n\nSelecione:";
  String keyboard = getControlKeyboard();

  telegramBot->sendMessageWithReplyKeyboard(CHAT_ID, msg, "", keyboard, true);
}

String getControlKeyboard() {
  String keyboard = "[";
  keyboard += "[\"" + String(RelayState1 ? "Varanda \xE2\xAD\x95 DESLIGAR" : "Varanda \xE2\x9A\xA1 LIGAR") + "\", \"" + String(RelayState5 ? "Quintal \xE2\xAD\x95 DESLIGAR" : "Quintal \xE2\x9A\xA1 LIGAR") + "\"],";
  keyboard += "[\"" + String(RelayState2 ? "Bancada \xE2\xAD\x95 DESLIGAR" : "Bancada \xE2\x9A\xA1 LIGAR") + "\", \"" + String(RelayState6 ? "Val \xE2\xAD\x95 DESLIGAR" : "Val \xE2\x9A\xA1 LIGAR") + "\"],";
  keyboard += "[\"" + String(RelayState3 ? "Sala \xE2\xAD\x95 DESLIGAR" : "Sala \xE2\x9A\xA1 LIGAR") + "\", \"" + String(RelayState7 ? "Robson \xE2\xAD\x95 DESLIGAR" : "Robson \xE2\x9A\xA1 LIGAR") + "\"],";
  keyboard += "[\"" + String(RelayState4 ? "Cozinha \xE2\xAD\x95 DESLIGAR" : "Cozinha \xE2\x9A\xA1 LIGAR") + "\", \"" + String(RelayState8 ? "Kinha \xE2\xAD\x95 DESLIGAR" : "Kinha \xE2\x9A\xA1 LIGAR") + "\"],";
  keyboard += "[\"\xF0\x9F\x92\xA1 LIGAR TUDO\", \"\xF0\x9F\x94\x8C DESLIGAR TUDO\"],";
  keyboard += "[\"\xF0\x9F\x93\x8A VER SENSORES\"]";
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

  String bootMsg = "\xF0\x9F\x8F\xA0 *Automacao Residencial*\nPainel de controle pronto!";
  String onRelays = "\n\n\xE2\x9A\xA0\xEF\xB8\x8F *Atencao! Os seguintes relies ja iniciaram LIGADOS:*";
  bool anyOn = false;

  if (RelayState1) { onRelays += "\n\xE2\x9A\xA1 Varanda"; anyOn = true; }
  if (RelayState2) { onRelays += "\n\xE2\x9A\xA1 Bancada"; anyOn = true; }
  if (RelayState3) { onRelays += "\n\xE2\x9A\xA1 Sala"; anyOn = true; }
  if (RelayState4) { onRelays += "\n\xE2\x9A\xA1 Cozinha"; anyOn = true; }
  if (RelayState5) { onRelays += "\n\xE2\x9A\xA1 Quintal"; anyOn = true; }
  if (RelayState6) { onRelays += "\n\xE2\x9A\xA1 Val"; anyOn = true; }
  if (RelayState7) { onRelays += "\n\xE2\x9A\xA1 Robson"; anyOn = true; }
  if (RelayState8) { onRelays += "\n\xE2\x9A\xA1 Kinha"; anyOn = true; }

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
      telegramBot->sendMessage(chat_id, "Nao autorizado.", "");
      continue;
    }

    if (text == "/start" || text == "Voltar") {
      telegramBot->sendMessageWithReplyKeyboard(chat_id, "\xF0\x9F\x8F\xA0 Selecione uma opcao:", "", getControlKeyboard(), true);
      continue;
    }

    bool handled = false;
    for (int j = 0; j < 8; j++) {
      if (text.startsWith(relayNames[j])) {
        setRelayByIndex(j, !(*relayStatePtrs[j]), "Telegram");
        handled = true;
        break;
      }
    }
    if (handled) continue;

    if (text == "\xF0\x9F\x92\xA1 LIGAR TUDO" || text == "/on") {
      setAllRelays(true, "Telegram");
    }
    else if (text == "\xF0\x9F\x94\x8C DESLIGAR TUDO" || text == "/off") {
      setAllRelays(false, "Telegram");
    }
    else if (text == "\xF0\x9F\x93\x8A VER SENSORES" || text == "/sensores") {
      String msg = "\xF0\x9F\x93\x8A *Leituras*\n";
      msg += "\xF0\x9F\x8C\xA1\xEF\xB8\x8F Temp: " + String(str_temp_data) + " C\n";
      msg += "\xF0\x9F\x8C\xA1\xEF\xB8\x8F Sensacao: " + String(str_tempterm_data) + " C\n";
      msg += "\xF0\x9F\x92\xA7 Umidade: " + String(str_hum_data) + "%\n";
      msg += "\xF0\x9F\x8C\xAB\xEF\xB8\x8F Ponto Orvalho: " + String(str_dewpoint_data) + " C\n";
      msg += "\xE2\x8F\xB2\xEF\xB8\x8F Pressao: " + String(pressure / 100.0, 1) + " hPa\n";
      msg += "\xF0\x9F\x8F\x94\xEF\xB8\x8F Altitude: " + String(altitude, 1) + " m";
      sendMainMenu(chat_id, msg);
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
