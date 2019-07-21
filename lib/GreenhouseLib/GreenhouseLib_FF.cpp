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

    initOverride(LOCK, 0,0);
}

void OnOffDevice::initOutput(byte outputType, boolean relayType, byte pin){
  //define opening/closing pins
  output.init(outputType, relayType, pin);
}

void OnOffDevice::lockOn(){
  initOverride(LOCK, 0, true);
  checkOverride(LOCK, true);

}

void OnOffDevice::lockOff(){
  initOverride(LOCK, 0, false);
  checkOverride(LOCK, true);
}

void OnOffDevice::lockOnAndWait(int minuts){
  if(minuts != 0){
    resetLockTimer(minuts);
  }
  lockOn();
}

void OnOffDevice::lockOffAndWait(int minuts){
  if(minuts != 0){
    resetLockTimer(minuts);
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
    checkOverrideTimer();
    _activeOverride = activeOverride();
    if(_activeOverride != OFF_VAL){
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
  checkOverrideTimer();
  _activeOverride = activeOverride();
  if(_activeOverride != OFF_VAL){
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
    checkOverrideTimer();
    _activeOverride = activeOverride();
    if(_activeOverride != OFF_VAL){
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

/*
unsigned short Device::_EEPROMindex = 0;
unsigned short Device::_counter = 0;

Device::Device(){
    _localIndex = DEVICE_INDEX + _EEPROMindex;
    _EEPROMindex += DEVICE_INDEX_SIZE;
    _localCounter = _counter;
    _counter++;

    mod.setLimits(0, 10);
    mod.setAddress(_localIndex);
    _localIndex += sizeof(float);

    hyst.setLimits(0,5);
    hyst.setAddress(_localIndex);
    _localIndex += sizeof(float);

}

void Device::EEPROMGet(){
  #ifdef DEBUG_EEPROM
    Serial.println(F("-------------------"));
    Serial.print(F("--------DEVICE "));
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
Start or stop the device when a certain temperature is reached
Adjust to an external target temperature (Mode VAR_TEMP)
*/
/*
void Device::routine(float target, float temp){
    checkOverrideTimer();
    _activeOverride = activeOverride();
    if(_activeOverride != OFF_VAL){
      if((bool)overRide[_activeOverride].target == true){
        output.start();
      }
      else if((bool)overRide[_activeOverride].target == false){
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


  unsigned short Device::nb(){
    return _localCounter;
  }

  unsigned short Device::EEPROMIndexBegin(){
    return DEVICE_INDEX + (DEVICE_INDEX_SIZE*_localCounter);
  }

  unsigned short Device::EEPROMIndexEnd(){
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




void Heater::EEPROMGet(){
/*
  hyst.getInEEPROM();
  mod.getInEEPROM();
*/
/*
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
*/
