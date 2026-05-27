#include "RelayManager.h"
#include "VariaveisGlobais.h"
#include "GPIOs.h"
#include "TopicosMQTT.h"
#include "TelegramBotESP32.h"
#include "RemoteDebug.h"

Preferences preferences;

const char* relayTopics[8] = {sub1, sub2, sub3, sub4, sub5, sub6, sub7, sub8};
const int relayPinNums[8] = {RelayPin1, RelayPin2, RelayPin3, RelayPin4, RelayPin5, RelayPin6, RelayPin7, RelayPin8};
bool* relayStatePtrs[8] = {&RelayState1, &RelayState2, &RelayState3, &RelayState4, &RelayState5, &RelayState6, &RelayState7, &RelayState8};

void setRelayByIndex(int idx, bool state, const char* source) {
  *relayStatePtrs[idx] = state;
  digitalWrite(relayPinNums[idx], !state);
  logRelayAction(source, idx, state);
  saveRelayState(idx + 1, state);
  notifyTelegramStateChange(source, idx, state);
  Todos = RelayState1 && RelayState2 && RelayState3 && RelayState4 &&
          RelayState5 && RelayState6 && RelayState7 && RelayState8;
}

void setAllRelays(bool state, const char* source) {
  Todos = state;
  for (int i = 0; i < 8; i++) {
    *relayStatePtrs[i] = state;
    digitalWrite(relayPinNums[i], !state);
    saveRelayState(i + 1, state);
  }
  logRelayAction(source, -1, state);
  notifyTelegramStateChange(source, -1, state);
}

int findRelayByName(const String& name) {
  for (int i = 0; i < 8; i++) {
    String stored = relayNames[i];
    stored.toUpperCase();
    if (name == stored) return i;
  }
  if (name == "VAL") return 5;
  return -1;
}

void saveRelayState(int relayNum, bool state) {
  if (relayMutex == NULL) return;
  String key = "RelayState" + String(relayNum);
  if (xSemaphoreTake(relayMutex, portMAX_DELAY) == pdTRUE) {
    preferences.putBool(key.c_str(), state);
    xSemaphoreGive(relayMutex);
  }
}

bool loadRelayState(int relayNum, bool defaultValue) {
  if (relayMutex == NULL) return defaultValue;
  String key = "RelayState" + String(relayNum);
  bool val = defaultValue;
  if (xSemaphoreTake(relayMutex, portMAX_DELAY) == pdTRUE) {
    val = preferences.getBool(key.c_str(), defaultValue);
    xSemaphoreGive(relayMutex);
  }
  return val;
}

void loadAllRelayStates() {
  RelayState1 = loadRelayState(1, false);
  RelayState2 = loadRelayState(2, false);
  RelayState3 = loadRelayState(3, false);
  RelayState4 = loadRelayState(4, false);
  RelayState5 = loadRelayState(5, false);
  RelayState6 = loadRelayState(6, false);
  RelayState7 = loadRelayState(7, false);
  RelayState8 = loadRelayState(8, false);
  Todos = RelayState1 && RelayState2 && RelayState3 && RelayState4 &&
          RelayState5 && RelayState6 && RelayState7 && RelayState8;
}

void applyRelayStatesToPins() {
  logRelayAction("Sistema", 0, RelayState1);
  logRelayAction("Sistema", 1, RelayState2);
  logRelayAction("Sistema", 2, RelayState3);
  logRelayAction("Sistema", 3, RelayState4);
  logRelayAction("Sistema", 4, RelayState5);
  logRelayAction("Sistema", 5, RelayState6);
  logRelayAction("Sistema", 6, RelayState7);
  logRelayAction("Sistema", 7, RelayState8);
  digitalWrite(RelayPin1, !RelayState1);
  digitalWrite(RelayPin2, !RelayState2);
  digitalWrite(RelayPin3, !RelayState3);
  digitalWrite(RelayPin4, !RelayState4);
  digitalWrite(RelayPin5, !RelayState5);
  digitalWrite(RelayPin6, !RelayState6);
  digitalWrite(RelayPin7, !RelayState7);
  digitalWrite(RelayPin8, !RelayState8);
}

void logRelayAction(const char* source, int relayNum, bool state) {
  Serial.print('[');
  Serial.print(source);
  Serial.print("] ");
  if (relayNum >= 0 && relayNum < 8) {
    Serial.print(relayNames[relayNum]);
  } else {
    Serial.print("TODOS");
  }
  Serial.print(": ");
  Serial.println(state ? "LIGADO" : "DESLIGADO");
}
