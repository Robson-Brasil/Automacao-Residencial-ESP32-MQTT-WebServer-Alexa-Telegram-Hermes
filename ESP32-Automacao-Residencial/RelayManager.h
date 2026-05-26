#ifndef RELAYMANAGER_H
#define RELAYMANAGER_H

#include <Arduino.h>
#include <Preferences.h>

extern Preferences preferences;

extern const char* relayTopics[8];
extern const int relayPinNums[8];
extern bool* relayStatePtrs[8];

void setRelayByIndex(int idx, bool state, const char* source);
void setAllRelays(bool state, const char* source);
int findRelayByName(const String& name);

void saveRelayState(int relayNum, bool state);
bool loadRelayState(int relayNum, bool defaultValue);
void loadAllRelayStates();
void applyRelayStatesToPins();

void logRelayAction(const char* source, int relayNum, bool state);

#endif
