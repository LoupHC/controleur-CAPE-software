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
unsigned short Fan::_EEPROMindex = 0;
unsigned short Fan::_counter = 0;

Fan::Fan(){
    _localIndex = FAN_INDEX + _EEPROMindex;
    _EEPROMindex += 8;
    _localCounter = _counter;
    _counter++;

    mod.setLimits(0, 10);
    mod.setAddress(_localIndex);
    _localIndex += sizeof(float);

    hyst.setLimits(0,5);
    hyst.setAddress(_localIndex);
    _localIndex += sizeof(float);

    for(int x = 0; x < MAX_OVERRIDES;x++){
      overRide[x].active = false;
      overRide[x].state = false;
    }

    EEPROMTimer = 0;
}


Fan::~Fan(){}

void Fan::initOutput(byte relayType, byte pin){

  _pin = pin;

  //set action linked to digitalWrite state, according to relay type
 _relayType = relayType;

 if (_relayType == ACT_HIGH){
   _activate = HIGH;
   _desactivate = LOW;
 }
 else if (_relayType == ACT_LOW){
   _activate = LOW;
   _desactivate = HIGH;
 }

 //initial state
 #ifndef MCP_I2C_OUTPUTS
    pinMode(pin, OUTPUT);
    digitalWrite(pin, _desactivate);
  #endif

  #ifdef MCP_I2C_OUTPUTS
    mcp.pinMode(pin, OUTPUT);
    mcp.digitalWrite(pin, _desactivate);
  #endif

 }
 void Fan::initOverride(byte number, boolean state){
   if((number > 0)&&(number < MAX_OVERRIDES)){
     overRide[number].state = state;
   }
 }

 void Fan::setOverride(byte number, boolean state){
   if((number > 0)&&(number < MAX_OVERRIDES)){
     if(state == true){
       overRide[number].active = true;
     }
     else{
       overRide[number].active = false;
     }
   }
 }
//action functions

/*
Start or stop the fan when a certain temperature is reached
Adjust to an external target temperature (Mode VAR_TEMP)
*/
void Fan::routine(float target, float temp){
    _activeOverride = activeOverride();
    if(_activeOverride != OFF_VAL){
      if(overRide[_activeOverride].state == true){
        start();
      }
      else if(overRide[_activeOverride].state == false){
        stop();
      }
    }
    else{
        float activationTemp = target + mod.value();
        if (temp < (activationTemp - hyst.value())) {
          	stop();
        }
        else if (temp > activationTemp) {
          	start();
        }
    }
  }

  byte Fan::activeOverride(){
    for(int x = 0; x < MAX_OVERRIDES;x++){
      if(overRide[x].active == true){
        return  x;
      }
    }
    return OFF_VAL;
  }

void Fan::lockOn(){
  _activeOverride = OFF_VAL;
  overRide[0].state = true;
  overRide[0].active = true;

}

void Fan::lockOff(){
  _activeOverride = OFF_VAL;
  overRide[0].state = false;
  overRide[0].active = true;
}

void Fan::unlock(){
  overRide[0].active = false;
}

void Fan::stop(){
  #ifndef MCP_I2C_OUTPUTS

  if(digitalRead(_pin) == _activate){
    digitalWrite(_pin, _desactivate);
  }
  #endif

  #ifdef MCP_I2C_OUTPUTS

  if(mcp.digitalRead(_pin) == _activate){
    mcp.digitalWrite(_pin, _desactivate);
  }
  #endif
  #ifdef DEBUG_FAN
    Serial.println(F("-------------"));
    Serial.println(F("Stop fan"));
    Serial.println(F("-------------"));
  #endif
}

void Fan::start(){
  #ifndef MCP_I2C_OUTPUTS

      if(digitalRead(_pin) == _desactivate){
        digitalWrite(_pin, _activate);
      }
    #endif

    #ifdef MCP_I2C_OUTPUTS

      if(mcp.digitalRead(_pin) == _desactivate){
        mcp.digitalWrite(_pin, _activate);
      }
    #endif
    #ifdef DEBUG_FAN
      Serial.println(F("-------------"));
      Serial.println(F("Start fan"));
      Serial.println(F("-------------"));
    #endif
  }

//programmation functions

void Fan::setParameters(float modif, float hysteresis){
  hyst.setValue(hysteresis);
  mod.setValue(modif);
}
/*
Or one by one...
*/

byte Fan::pin(){
  return _pin;
}
unsigned short Fan::nb(){
  return _localCounter;
}
boolean Fan::override(){
  if(_activeOverride == OFF_VAL){
    return false;
  }
  else{
    return true;
  }
}
boolean Fan::isActive(){
  #ifndef MCP_I2C_OUTPUTS
  if(digitalRead(_pin) == _activate){
    return true;
  }
  else{
    return false;
  }
  #endif

  #ifdef MCP_I2C_OUTPUTS

  if(mcp.digitalRead(_pin) == _activate){
    return true;
  }
  else{
    return false;
  }
  #endif
}

void Fan::EEPROMPut(){
  hyst.loadInEEPROM();
  mod.loadInEEPROM();
}

void Fan::EEPROMGet(){

  float hysteresis;
  EEPROM.get(hyst.address(), hysteresis);
  hyst.setValue(hysteresis);

  float modif;
  EEPROM.get(mod.address(), modif);
  mod.setValue(modif);

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

boolean Fan::TEST_parameterOffLimits(){
  if(hyst.isOffLimit()||mod.isOffLimit()){
    return true;
  }
  else{
    return false;
  }
}
//****************************************************************
//******************HEATER FUNCTIONS************************
//****************************************************************
unsigned short Heater::_EEPROMindex = 0;
unsigned short Heater::_counter = 0;

Heater::Heater(){
    _localIndex = HEATER_INDEX + _EEPROMindex;
    _EEPROMindex += 8;
    _localCounter = _counter;
    _counter++;

    mod.setLimits(-10, 0);
    mod.setAddress(_localIndex);
    _localIndex += sizeof(float);

    hyst.setLimits(0,5);
    hyst.setAddress(_localIndex);
    _localIndex += sizeof(float);

    for(int x = 0; x < MAX_OVERRIDES;x++){
      overRide[x].active = false;
      overRide[x].state = false;
    }

    EEPROMTimer = 0;
}

Heater::~Heater(){}

void Heater::initOutput(byte relayType, byte pin){
  _pin = pin;

 //set action linked to digitalWrite state, according to relay type
 _relayType = relayType;
 if (_relayType == ACT_HIGH){
   _activate = HIGH;
   _desactivate = LOW;
 }
 else if (_relayType == ACT_LOW){
   _activate = LOW;
   _desactivate = HIGH;
 }

 //initial state
 #ifndef MCP_I2C_OUTPUTS
    pinMode(_pin, OUTPUT);
    digitalWrite(_pin, _desactivate);
  #endif

  #ifdef MCP_I2C_OUTPUTS
    mcp.pinMode(_pin, OUTPUT);
    mcp.digitalWrite(_pin, _desactivate);
  #endif

 }
 void Heater::initOverride(byte number, boolean state){
   if((number > 0)&&(number < MAX_OVERRIDES)){
     overRide[number].state = state;
   }
 }

 void Heater::setOverride(byte number, boolean state){
   if((number > 0)&&(number < MAX_OVERRIDES)){
     if(state == true){
       overRide[number].active = true;
     }
     else{
       overRide[number].active = false;
     }
   }
 }

//action functions
/*
Start or stop the heater when a certain temperature is reached
Adjust to an external target temperature (Mode VAR_TEMP)
*/
void Heater::routine(float target, float temp){
    _activeOverride = activeOverride();
    if(_activeOverride != OFF_VAL){
      if(overRide[_activeOverride].state == true){
        start();
      }
      else if(overRide[_activeOverride].state == false){
        stop();
      }
    }
  	else{
      float activationTemp = target + mod.value();
      if (temp > (activationTemp + hyst.value())) {
        	stop();
      } else if (temp < activationTemp) {
        	start();
      }
    }
  }

  byte Heater::activeOverride(){
    for(int x = 0; x < MAX_OVERRIDES;x++){
      if(overRide[x].active == true){
        return  x;
      }
    }
    return OFF_VAL;
  }
void Heater::lockOn(){
  _activeOverride = OFF_VAL;
  overRide[0].state = true;
  overRide[0].active = true;

}

void Heater::lockOff(){
  _activeOverride = OFF_VAL;
  overRide[0].state = false;
  overRide[0].active = true;
}

void Heater::unlock(){
  overRide[0].active = false;
}

void Heater::stop(){
  #ifndef MCP_I2C_OUTPUTS
    if(digitalRead(_pin) == HIGH){
      digitalWrite(_pin, _desactivate);
    }
  #endif
  #ifdef MCP_I2C_OUTPUTS
    if(mcp.digitalRead(_pin) == _activate){
      mcp.digitalWrite(_pin, _desactivate);
    }
  #endif

  #ifdef DEBUG_HEATER
    Serial.println(F("-------------"));
    Serial.println(F("Stop heater"));
    Serial.println(F("-------------"));
  #endif
  }

void Heater::start(){
  #ifndef MCP_I2C_OUTPUTS
    if(digitalRead(_pin) == _desactivate){
      digitalWrite(_pin, _activate);
    }
  #endif
  #ifdef MCP_I2C_OUTPUTS
    if(mcp.digitalRead(_pin) == _desactivate){
      mcp.digitalWrite(_pin, _activate);
    }
  #endif
  #ifdef DEBUG_HEATER
    Serial.println(F("-------------"));
    Serial.println(F("Start heater"));
    Serial.println(F("-------------"));
  #endif
  }

//programmation functions


void Heater::setParameters(float modif, float hysteresis){
  hyst.setValue(hysteresis);
  mod.setValue(modif);
}
/*
Or one by one...
*/

byte Heater::pin(){
  return _pin;
}
unsigned short Heater::nb(){
  return _localCounter;
}
boolean Heater::override(){
  if(_activeOverride == OFF_VAL){
    return false;
  }
  else{
    return true;
  }
}

boolean Heater::isActive(){
  #ifndef MCP_I2C_OUTPUTS
  if(digitalRead(_pin) == _activate){
    return true;
  }
  else{
    return false;
  }
  #endif

  #ifdef MCP_I2C_OUTPUTS

  if(mcp.digitalRead(_pin) == _activate){
    return true;
  }
  else{
    return false;
  }
  #endif
}
void Heater::EEPROMPut(){
  hyst.loadInEEPROM();
  mod.loadInEEPROM();
}
void Heater::EEPROMGet(){

  float hysteresis;
  EEPROM.get(hyst.address(), hysteresis);
  hyst.setValue(hysteresis);

  float modif;
  EEPROM.get(mod.address(), modif);
  mod.setValue(modif);

  #ifdef DEBUG_EEPROM
    Serial.println(F("-------------------"));
    Serial.print(F("-------HEATER "));
    Serial.print(_localCounter);
    Serial.println(F("------"));
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

boolean Heater::TEST_parameterOffLimits(){
  if(hyst.isOffLimit()||mod.isOffLimit()){
    return true;
  }
  else{
    return false;
  }
}
