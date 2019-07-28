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
//******************DEVICE FUNCTIONS************************
//****************************************************************/


unsigned short OnOffDevice::_EEPROMindex = 0;
unsigned short OnOffDevice::_counter = 0;

OnOffDevice::OnOffDevice(){
    _localIndex = DEVICE_INDEX + _EEPROMindex;
    _EEPROMindex += DEVICE_INDEX_SIZE;
    _localCounter = _counter;
    _counter++;

    mod.setLimits(-10, 10);
    mod.setAddress(_localIndex);
    _localIndex += sizeof(float);

    hyst.setLimits(0,5);
    hyst.setAddress(_localIndex);
    _localIndex += sizeof(float);

    type.setLimits(0,2);
    type.setAddress(_localIndex);
    _localIndex += sizeof(byte);

    enabled.setAddress(_localIndex);
    _localIndex += sizeof(boolean);

    lock.setAddress(_localIndex);
    _localIndex += sizeof(boolean);

    lockTarget.setAddress(_localIndex);
    _localIndex += sizeof(byte);

    initOverride(LOCK, 0,0);
}

void OnOffDevice::initOutput(byte outputType, boolean relayType, byte pin){
  //define opening/closing pins
  output.init(outputType, relayType, pin);
}

void OnOffDevice::unlock(){
  checkOverride(LOCK, false);
  lockedAndWaiting = false;
  lock.setValue(false);
}

void OnOffDevice::resetLockTimer(unsigned long seconds){
  lockedAndWaiting = true;
  overrideWaitingTime = seconds;
  overrideTimer = 0;
}

void OnOffDevice::checkOverrideTimer(){
  if(lockedAndWaiting == true){
    if(overrideTimer >= overrideWaitingTime*1000){
      checkOverride(LOCK, false);
      lockedAndWaiting = false;
    }
  }
}
void OnOffDevice::keepLockInMemory(byte increment){
  lock.setValue(true);
  lockTarget.setValue(increment);
}
void OnOffDevice::lockOn(){
  initOverride(LOCK, 0, true);
  checkOverride(LOCK, true);
}

void OnOffDevice::lockOff(){
  initOverride(LOCK, 0, false);
  checkOverride(LOCK, true);
}

void OnOffDevice::lockOnAndWait(unsigned long seconds){
  if(seconds != 0){
    resetLockTimer(seconds);
  }
  else{
    keepLockInMemory(1);
  }
  lockOn();
}

void OnOffDevice::lockOffAndWait(unsigned long seconds){
  if(seconds != 0){
    resetLockTimer(seconds);
  }
  else{
    keepLockInMemory(0);
  }
  lockOff();
}


//programmation functions
void OnOffDevice::adjustModLimits(){
  if(type.value() == HEATERTYPE){
    mod.setLimits(-10,0);
  }
  else if(type.value() == FANTYPE){
    mod.setLimits(0,10);
  }
}

void OnOffDevice::setParameters(byte typ, float modif, float hysteresis, bool enab){
  type.setValue(typ);
  adjustModLimits();
  hyst.setValue(hysteresis);
  mod.setValue(modif);
  enabled.setValue(enab);
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

boolean OnOffDevice::isOn(){
  if(output.isActive()){
    return true;
  }
  else{
    return false;
  }
}

void OnOffDevice::desactivateDevice(){
  enabled.setValue(false);
  output.stop();
}
void OnOffDevice::activateDevice(){
  enabled.setValue(true);
}
bool OnOffDevice::isActivated(){
  return enabled.value();
}


boolean OnOffDevice::TEST_parameterOffLimits(){
  if(hyst.isOffLimit()||mod.isOffLimit()){
    return true;
  }
  else{
    return false;
  }
}
void OnOffDevice::EEPROMGet(){
  #ifdef DEBUG_EEPROM
    Serial.println(F("-------------------"));
    Serial.print(F("--------DEVICE "));
    Serial.print(_localCounter);
    Serial.println(F("--------"));
    Serial.print(F("Address: "));
    Serial.print(enabled.address());
    Serial.print(F(" - Value :"));
    Serial.print(enabled.value());
    Serial.println(F(" - (Enabled)"));
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
    Serial.print(F("Address: "));
    Serial.print(type.address());
    if(type.value() == FANTYPE){    Serial.println(F(" - Type : Fan"));    }
    else if(type.value() == HEATERTYPE){    Serial.println(F(" - Type : Heater"));    }
  #endif
}

unsigned short OnOffDevice::nb(){
  return _localCounter;
}

unsigned short OnOffDevice::EEPROMIndexBegin(){
  return DEVICE_INDEX + (DEVICE_INDEX_SIZE*_localCounter);
}

unsigned short OnOffDevice::EEPROMIndexEnd(){
  return _localIndex;
}
/*
Start or stop the device when a certain temperature is reached
Adjust to an external target temperature (Mode VAR_TEMP)
*/

void OnOffDevice::routine(float target, float temp){
  adjustModLimits();
  if (isActivated()){
    if(type.value() == FANTYPE){
      fanRoutine(target, temp);
    }
    else if(type.value() == HEATERTYPE){
      heaterRoutine(target, temp);
    }
  }
}

void OnOffDevice::valvRoutine(){
  if (isActivated()){
    if(lock.value() == true){
      if(lockTarget.value() == 0){
        lockOff();
      }
      else{
        lockOn();
      }
    }
    checkOverrideTimer();
    _activeOverride = activeOverride();
    if(activeOverride() != OFF_VAL){
      if((bool)overrideTarget() == true){
        output.start();
      }
      else if((bool)overrideTarget() == false){
        output.stop();
      }
    }
    else{
      output.stop();
    }
  }
}

void OnOffDevice::fanRoutine(float target, float temp){
  if(lock.value() == true){
    if(lockTarget.value() == 0){
      lockOff();
    }
    else{
      lockOn();
    }
  }
  checkOverrideTimer();
  if(activeOverride() != OFF_VAL){
    if((bool)overrideTarget() == true){
      output.start();
    }
    else if((bool)overrideTarget() == false){
      output.stop();
    }
  }
  else{
      float activationTemp = target + mod.value();
      if (temp < (activationTemp - hyst.value())) {
        	output.stop();
      }
      else if (temp > activationTemp) {
        	output.start();
      }
  }
}

void OnOffDevice::heaterRoutine(float target, float temp){
  if(lock.value() == true){
    if(lockTarget.value() == 0){
      lockOff();
    }
    else{
      lockOn();
    }
  }
    checkOverrideTimer();
    if(activeOverride() != OFF_VAL){
      if((bool)overrideTarget() == true){
        output.start();
      }
      else if((bool)overrideTarget() == false){
        output.stop();
      }
    }
    else{
        float activationTemp = target + mod.value();
        if (temp > (activationTemp + hyst.value())) {
          	output.stop();
        }
        else if (temp < activationTemp) {
          	output.start();
        }
    }
  }
