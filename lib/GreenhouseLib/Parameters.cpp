/*
  Parameters.cpp
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

#include "Parameters.h"

//Each  parameter have 3 associated values, a minimumimum value, a maximumimum value and an actual value
//float parameters

#define DEF_ADDRESS -111

floatParameter::floatParameter(){
  _address = DEF_ADDRESS;
}
floatParameter::~floatParameter(){}

void floatParameter::setAddress(int address){
  _address = address;
}
int floatParameter::address(){
  return _address;
}
void floatParameter::setValue(float value){
  if(_address != DEF_ADDRESS){
      EEPROM.put(_address, value);
  }
}

void floatParameter::setLimits(float minimum, float maximum){
  _minimum = minimum;
  _maximum = maximum;
}


float floatParameter::minimum(){
  return _minimum;
}
float floatParameter::maximum(){
  return _maximum;
}

float floatParameter::value(){
  if(_address != DEF_ADDRESS){
    float variable;
    EEPROM.get(_address, variable);

    return variable;
  }
  return OFF_VAL;
}

boolean floatParameter::isOffLimit(){
  if ((value() < _minimum) || (value() > _maximum)){
    return true;
  }
  else{
    return false;
  }
}

//short parameter

shortParameter::shortParameter(){
  _address = DEF_ADDRESS;
}
shortParameter::~shortParameter(){}

void shortParameter::setAddress(int address){
  _address = address;
}
int shortParameter::address(){
  return _address;
}
void shortParameter::setValue(short value){
  if(_address != DEF_ADDRESS){
      EEPROM.put(_address, value);

  }
}

void shortParameter::setLimits(short minimum, short maximum){
  _minimum = minimum;
  _maximum = maximum;
}


short shortParameter::minimum(){
  return _minimum;
}
short shortParameter::maximum(){
  return _maximum;
}

short shortParameter::value(){
  if(_address != DEF_ADDRESS){
    short variable;
    EEPROM.get(_address, variable);

    return variable;
  }
  return OFF_VAL;
}

boolean shortParameter::isOffLimit(){
  if ((value() < _minimum) || (value() > _maximum)){
    return true;
  }
  else{
    return false;
  }
}
//unsigned short Parameters

uShortParameter::uShortParameter(){
  _address = DEF_ADDRESS;
}
uShortParameter::~uShortParameter(){}

void uShortParameter::setAddress(int address){
  _address = address;
}
int uShortParameter::address(){
  return _address;
}
void uShortParameter::setValue(unsigned short value){
  if(_address != DEF_ADDRESS){
        EEPROM.put(_address, value);
  }
}


void uShortParameter::setLimits(unsigned short minimum, unsigned short maximum){
  _minimum = minimum;
  _maximum = maximum;
}


unsigned short uShortParameter::minimum(){
  return _minimum;
}
unsigned short uShortParameter::maximum(){
  return _maximum;
}

unsigned short uShortParameter::value(){
  if(_address != DEF_ADDRESS){
    unsigned short variable;
    EEPROM.get(_address, variable);

    return variable;
  }
}

boolean uShortParameter::isOffLimit(){
  if ((value() < _minimum) || (value() > _maximum)){
    return true;
  }
  else{
    return false;
  }
}

byteParameter::byteParameter(){
  _address = DEF_ADDRESS;
}
byteParameter::~byteParameter(){}

void byteParameter::setAddress(int address){
  _address = address;
}
int byteParameter::address(){
  return _address;
}
void byteParameter::setValue(byte value){
  if(_address != DEF_ADDRESS){
      EEPROM.put(_address, value);
  }
}


void byteParameter::setLimits(byte minimum, byte maximum){
  _minimum = minimum;
  _maximum = maximum;
}


byte byteParameter::minimum(){
  return _minimum;
}
byte byteParameter::maximum(){
  return _maximum;
}

byte byteParameter::value(){
  if(_address != DEF_ADDRESS){
    byte variable;
    EEPROM.get(_address, variable);

    return variable;
  }
  return OFF_VAL;
}

boolean byteParameter::isOffLimit(){
  if ((value() < _minimum) || (value() > _maximum)){
    return true;
  }
  else{
    return false;
  }
}
//boolean parameter

boolParameter::boolParameter(){
  _address = DEF_ADDRESS;
}
boolParameter::~boolParameter(){}

void boolParameter::setAddress(int address){
  _address = address;
}
int boolParameter::address(){
  return _address;
}
void boolParameter::setValue(bool value){
  if(_address != DEF_ADDRESS){
      EEPROM.put(_address, value);
  }
}

boolean boolParameter::value(){
  if(_address != DEF_ADDRESS){
    boolean variable;
    EEPROM.get(_address, variable);

    return variable;
  }
  return false;
}
//short parameters

/*
shortParameter::shortParameter(){
  _EEPROM = false;
  _watchLimits = false;
}
shortParameter::~shortParameter(){}

void shortParameter::setAddress(int address){
  _EEPROM = true;
  _address = address;
}
int shortParameter::address(){
  return _address;
}
void shortParameter::setValue(short value){
    if(_watchLimits){
      if(!isOffLimit()){
        _value = value;
      }
    }
    else{
      _value = value;
    }
    if(_EEPROM){
      loadInEEPROM();
    }
}
void shortParameter::setLimits(short minimum, short maximum){
  _watchLimits = true;
  _minimum = minimum;
  _maximum = maximum;
}

void shortParameter::loadInEEPROM(){
  if(_EEPROM){
    EEPROM.put(_address, _value);
  }
}
void shortParameter::getInEEPROM(){
  short variable;
  if(_EEPROM){
    EEPROM.get(_address, variable);
    setValue(variable);
  }
}
short shortParameter::minimum(){
  return _minimum;
}
short shortParameter::maximum(){
  return _maximum;
}

short shortParameter::value(){
  return _value;
}

boolean shortParameter::isOffLimit(){
  if ((_value < _minimum) || (_value > _maximum)){
    return true;
  }
  else{
    return false;
  }
}

//unsigned short parameters

uShortParameter::uShortParameter(){
  _EEPROM = false;
  _watchLimits = false;
}
uShortParameter::~uShortParameter(){}

void uShortParameter::setAddress(int address){
  _EEPROM = true;
  _address = address;
}
int uShortParameter::address(){
  return _address;
}
void uShortParameter::setValue(unsigned short value){
    if(_watchLimits){
      if(!isOffLimit()){
        _value = value;
      }
    }
    else{
      _value = value;
    }
    if(_EEPROM){
      loadInEEPROM();
    }
}
void uShortParameter::setLimits(unsigned short minimum, unsigned short maximum){
  _watchLimits = true;
  _minimum = minimum;
  _maximum = maximum;
}

void uShortParameter::loadInEEPROM(){
  EEPROM.put(_address, _value);
}
void uShortParameter::getInEEPROM(){
  unsigned short variable;
  if(_EEPROM){
    EEPROM.get(_address, variable);
    setValue(variable);
  }
}
unsigned short uShortParameter::minimum(){
  return _minimum;
}
unsigned short uShortParameter::maximum(){
  return _maximum;
}
unsigned short uShortParameter::value(){
  return _value;
}

boolean uShortParameter::isOffLimit(){
  if ((_value < _minimum) || (_value > _maximum)){
    return true;
  }
  else{
    return false;
  }
}

//Byte parameter

byteParameter::byteParameter(){
  _EEPROM = false;
  _watchLimits = false;
}
byteParameter::~byteParameter(){}

void byteParameter::setAddress(int address){
  _EEPROM = true;
  _address = address;
}
int byteParameter::address(){
  return _address;
}
void byteParameter::setValue(byte value){
    if(_watchLimits){
      if(!isOffLimit()){
        _value = value;
      }
    }
    else{
      _value = value;
    }
    if(_EEPROM){
      loadInEEPROM();
    }
}



void byteParameter::setLimits (byte minimum, byte maximum){
  _watchLimits = true;
  _minimum = minimum;
  _maximum = maximum;
}

void byteParameter::loadInEEPROM(){
  EEPROM.update(_address, _value);
}
void byteParameter::getInEEPROM(){
  byte variable;
  if(_EEPROM){
    variable = EEPROM.read(_address);
    setValue(variable);
  }
}
byte byteParameter::minimum(){
  return _minimum;
}
byte byteParameter::maximum(){
  return _maximum;
}
byte byteParameter::value(){
  return _value;
}
boolean byteParameter::isOffLimit(){
  if ((_value < _minimum) || (_value > _maximum)){
    return true;
  }
  else{
    return false;
  }
}
//boolean parameter

boolParameter::boolParameter(){
  _EEPROM = false;
}
boolParameter::~boolParameter(){}

void boolParameter::setAddress(int address){
  _EEPROM = true;
  _address = address;
}
int boolParameter::address(){
  return _address;
}
void boolParameter::setValue(bool value){
  _value = value;
  if(_EEPROM){
    loadInEEPROM();
  }
}

void boolParameter::loadInEEPROM(){
  EEPROM.update(_address, _value);
}

void boolParameter::getInEEPROM(){
  byte variable;
  if(_EEPROM){
    variable = EEPROM.read(_address);
    setValue(variable);
  }
}
boolean boolParameter::value(){
  return _value;
}
*/



//Time parameter which you can add and substract hours and minuts
timeParameter::timeParameter(){}
timeParameter::timeParameter(short hour, short minut){
  _hour = hour;
  _minut = minut;
}
timeParameter::~timeParameter(){}

void timeParameter::setTime(short hour, short minut){
    _hour = hour;
    _minut = minut;
}

void timeParameter::addTime(short hour, short minut){
  _hour += hour;
  _minut += minut;
  adjustTime();

}
void timeParameter::subTime(short hour, short minut){
    _hour -= hour;
    _minut -= minut;
    adjustTime();
}
unsigned short timeParameter::hour(){
  return _hour;
}
unsigned short timeParameter::minut(){
  return _minut;
}
void timeParameter::adjustTime(){
  while(_hour >= 24){
    _hour -= 24;
  }
  while(_minut >= 60){
    _hour += 1;
    _minut -= 60;
  }
  while(_hour < 0){
    _hour += 24;
  }
  while(_minut < 0){
    _hour -= 1;
    _minut += 60;
  }
}


  boolean isBetween(byte startHour, byte startMin, byte actualHour, byte actualMin, byte stopHour, byte stopMin){
    unsigned short startTime = (unsigned short)startHour*60 + (unsigned short)startMin;
    unsigned short actualTime = (unsigned short)actualHour*60 + (unsigned short)actualMin;
    unsigned short stopTime = (unsigned short)stopHour*60 + (unsigned short)stopMin;

    if (stopTime < startTime){
      stopTime += 24*60;
      if(actualTime < startTime){
        actualTime += 24*60;
      }
    }

    if ((actualTime >= startTime)&&(actualTime < stopTime)){
      return true;
    }
    else{
      return false;
    }
  }
