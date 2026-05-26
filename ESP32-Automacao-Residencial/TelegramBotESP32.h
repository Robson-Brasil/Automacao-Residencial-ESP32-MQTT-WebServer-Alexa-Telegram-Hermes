/**
 * @file TelegramBotESP32.h
 * @brief Bot do Telegram com Teclado Interativo (Botões) — Declarações apenas.
 */
#ifndef TELEGRAMBOTESP32_H
#define TELEGRAMBOTESP32_H

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

#include "VariaveisGlobais.h"
#include "LoginsSenhas.h"

// Variáveis globais (definidas em TelegramBotESP32.cpp)
extern WiFiClientSecure telegramClient;
extern UniversalTelegramBot *telegramBot;
extern unsigned long bot_lasttime;
extern bool telegramInitialized;
extern String lastTelegramMessage;
extern const String relayNames[8];

const unsigned long BOT_MTBS = 1000;

// Protótipos das funções do RelayManager (definidas em RelayManager.cpp)
extern void saveRelayState(int relayNum, bool state);
extern void publishRelayStates();
extern void applyRelayStatesToPins();
extern void logRelayAction(const char* source, int relayNum, bool state);
extern bool* relayStatePtrs[8];
extern void setRelayByIndex(int idx, bool state, const char* source);
extern void setAllRelays(bool state, const char* source);

// Protótipos das funções do Telegram (definidas em TelegramBotESP32.cpp)
void notifyTelegramStateChange(const String& source, int relayNum = -1, bool relayState = false);
void processPendingTelegramNotification(const TelegramNotification& notif);
String getControlKeyboard();
void sendMainMenu(String chat_id, String message);
void initTelegram();
void handleTelegramMessages(int numNewMessages);
void telegramLoop();

#endif // TELEGRAMBOTESP32_H