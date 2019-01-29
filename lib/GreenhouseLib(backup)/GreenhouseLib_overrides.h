
#include "Arduino.h"
#include "EEPROM.h"
#include "elapsedMillis.h"
#include "Parameters.h"
#include "Defines.h"

#ifndef GreenhouseLib_overrides_h
#define GreenhouseLib_overrides_h

void createOverride(byte ID, byte hrStart, byte mnStart, byte hrStop, byte mnStop, byte target);
void createOverride(byte ID, byte mn, byte target);
void createOverride(byte ID, float value, byte target);
void createOverride(byte ID, bool value, byte target);

void checkOverride(byte hrStart, byte mnStart, byte hrStop, byte mnStop);
void startOverride();
void checOverride(float variable);
void checOverride(int variable);
void checkOverride(bool variable);

void setOverride(byte ID, bool state);

#endif
