/*
  GreenhouseLib_FF.cpp
  Copyright (C)2017 Loup Hébert-Chartrand. All right reserved

  You can find the latest version of this code at :
  https://github.com/LoupHC/GreenhouseLib

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This code is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/
#include "GreenhouseLib_FF.h"

//****************************************************
//******************FAN FUNCTIONS************************
//****************************************************************/

void OnOffDevice::initOutput(byte outputType, boolean relayType, byte pin){
  //define opening/closing pins
  _output.init(outputType, relayType, pin);
}

void OnOffDevice::lockOn(){
  _activeOverride = OFF_VAL;
  overRide[0].target = true;
  overRide[0].active = true;

}

void OnOffDevice::lockOff(){
  _activeOverride = OFF_VAL;
  overRide[0].target = false;
  overRide[0].active = true;
}

void OnOffDevice::unlock(){
  overRide[0].active = false;
}

void OnOffDevice::resetLockTimer(){
  overrideTimer = 0;
}
void OnOffDevice::lockOnAndWait(int minuts){
  overRide[0].target = true;
  overRide[0].active = true;
  lockedAndWaiting = true;
  overrideWaitingTime = minuts;
  resetLockTimer();
}

void OnOffDevice::lockOffAndWait(int minuts){
  overRide[0].target = false;
  overRide[0].active = true;
  lockedAndWaiting = true;
  overrideWaitingTime = minuts;
  resetLockTimer();
}

boolean OnOffDevice::isLock(){
  if(overRide[0].active == true){
    return true;
  }
  else{
    return false;
  }
}


//programmation functions

void OnOffDevice::setParameters(float modif, float hysteresis){
  hyst.setValue(hysteresis);
  mod.setValue(modif);
}
/*
Or one by one...
*/
boolean OnOffDevice::override(){
  if(_activeOverride == OFF_VAL){
    return false;
  }
  else{
    return true;
  }
}

boolean OnOffDevice::isActive(){
  if(_output.isActive()){
    return true;
  }
  else{
    return false;
  }
}


boolean OnOffDevice::TEST_parameterOffLimits(){
  if(hyst.isOffLimit()||mod.isOffLimit()){
    return true;
  }
  else{
    return false;
  }
}

unsigned short Fan::_EEPROMindex = 0;
unsigned short Fan::_counter = 0;

Fan::Fan(){
    _localIndex = FAN_INDEX + _EEPROMindex;
    _EEPROMindex += FAN_INDEX_SIZE;
    _localCounter = _counter;
    _counter++;

    mod.setLimits(0, 10);
    mod.setAddress(_localIndex);
    _localIndex += sizeof(float);

    hyst.setLimits(0,5);
    hyst.setAddress(_localIndex);
    _localIndex += sizeof(float);

}

void Fan::EEPROMGet(){
  #ifdef DEBUG_EEPROM
    Serial.println(F("-------------------"));
    Serial.print(F("--------FAN "));
    Serial.print(_localCounter);
    Serial.println(F("--------"));
    Serial.print(F("Address: "));
    Serial.print(hyst.address());
    Serial.print(F(" - Value :"));
    Serial.print(hyst.value());
    Serial.println(F(" - (Hysteresis)"));
    Serial.print(F("Address: "));
    Serial.print(mod.address());
    Serial.print(F(" - Value :"));
    Serial.print(mod.value());
    Serial.println(F("   - (Mod)"));
  #endif
}

/*
Start or stop the fan when a certain temperature is reached
Adjust to an external target temperature (Mode VAR_TEMP)
*/
void Fan::routine(float target, float temp){
    checkOverrideTimer();
    _activeOverride = activeOverride();
    if(_activeOverride != OFF_VAL){
      if((bool)overRide[_activeOverride].target == true){
        _output.start();
      }
      else if((bool)overRide[_activeOverride].target == false){
        _output.stop();
      }
    }
    else{
        float activationTemp = target + mod.value();
        if (temp < (activationTemp - hyst.value())) {
          	_output.stop();
        }
        else if (temp > activationTemp) {
          	_output.start();
        }
    }
  }

  unsigned short Fan::nb(){
    return _localCounter;
  }

  unsigned short Fan::EEPROMIndexBegin(){
    return FAN_INDEX + (FAN_INDEX_SIZE*_localCounter);
  }

  unsigned short Fan::EEPROMIndexEnd(){
    return _localIndex;
  }


  unsigned short Heater::_EEPROMindex = 0;
  unsigned short Heater::_counter = 0;

  Heater::Heater(){
      _localIndex = HEATER_INDEX + _EEPROMindex;
      _EEPROMindex += HEATER_INDEX_SIZE;
      _localCounter = _counter;
      _counter++;

      mod.setLimits(0, 10);
      mod.setAddress(_localIndex);
      _localIndex += sizeof(float);

      hyst.setLimits(0,5);
      hyst.setAddress(_localIndex);
      _localIndex += sizeof(float);

  }

void Heater::routine(float target, float temp){
    checkOverrideTimer();
    _activeOverride = activeOverride();
    if(_activeOverride != OFF_VAL){
      if((bool)overRide[_activeOverride].target == true){
        _output.start();
      }
      else if((bool)overRide[_activeOverride].target == false){
        _output.stop();
      }
    }
    else{
        float activationTemp = target + mod.value();
        if (temp > (activationTemp + hyst.value())) {
          	_output.stop();
        }
        else if (temp < activationTemp) {
          	_output.start();
        }
    }
  }

void Heater::EEPROMGet(){
/*
  hyst.getInEEPROM();
  mod.getInEEPROM();
*/
  #ifdef DEBUG_EEPROM
    Serial.println(F("-------------------"));
    Serial.print(F("--------HEATER "));
    Serial.print(_localCounter);
    Serial.println(F("--------"));
    Serial.print(F("Address: "));
    Serial.print(hyst.address());
    Serial.print(F(" - Value :"));
    Serial.print(hyst.value());
    Serial.println(F(" - (Hysteresis)"));
    Serial.print(F("Address: "));
    Serial.print(mod.address());
    Serial.print(F(" - Value :"));
    Serial.print(mod.value());
    Serial.println(F("   - (Mod)"));
  #endif
}

unsigned short Heater::nb(){
  return _localCounter;
}

unsigned short Heater::EEPROMIndexBegin(){
  return HEATER_INDEX + (HEATER_INDEX_SIZE*_localCounter);
}

unsigned short Heater::EEPROMIndexEnd(){
  return _localIndex;
}
