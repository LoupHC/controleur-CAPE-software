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


floatParameter::floatParameter(){}
floatParameter::~floatParameter(){}

void floatParameter::setAddress(int address){
  _address = address;
}
int floatParameter::address(){
  return _address;
}
void floatParameter::setValue(float value){
  setLastValue(_value);
    _value = value;
}

void floatParameter::setLastValue(float lastValue){
    _lastValue = lastValue;
}

void floatParameter::updateLastValue(){
  setLastValue(_value);
}

boolean floatParameter::valueHasChanged(){
  if(_value == _lastValue){
    return false;
  }
  else{
    return true;
  }
}

void floatParameter::setLimits(float minimum, float maximum){
  _minimum = minimum;
  _maximum = maximum;
}
void floatParameter::loadInEEPROM(){
  EEPROM.put(_address, _value);
}

float floatParameter::minimum(){
  return _minimum;
}
float floatParameter::maximum(){
  return _maximum;
}

float floatParameter::value(){
  return _value;
}
float floatParameter::lastValue(){
  return _lastValue;
}
boolean floatParameter::isOffLimit(){
  if ((_value < _minimum) || (_value > _maximum)){
    return true;
  }
  else{
    return false;
  }
}
//short parameters

shortParameter::shortParameter(){}
shortParameter::~shortParameter(){}

void shortParameter::setAddress(int address){
  _address = address;
}
int shortParameter::address(){
  return _address;
}
void shortParameter::setValue(short value){
  setLastValue(_value);
    _value = value;
}
void shortParameter::setLastValue(short lastValue){
    _lastValue = lastValue;
}

void shortParameter::updateLastValue(){
  setLastValue(_value);
}

boolean shortParameter::valueHasChanged(){
  if(_value == _lastValue){
    return false;
  }
  else{
    return true;
  }
}
void shortParameter::setLimits(short minimum, short maximum){
  _minimum = minimum;
  _maximum = maximum;
}

void shortParameter::loadInEEPROM(){
  EEPROM.put(_address, _value);
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

short shortParameter::lastValue(){
  return _lastValue;
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

uShortParameter::uShortParameter(){}
uShortParameter::~uShortParameter(){}

void uShortParameter::setAddress(int address){
  _address = address;
}
int uShortParameter::address(){
  return _address;
}
void uShortParameter::setValue(unsigned short value){
  setLastValue(_value);
    _value = value;
}
void uShortParameter::setLastValue(unsigned short lastValue){
    _lastValue = lastValue;
}

void uShortParameter::updateLastValue(){
  setLastValue(_value);
}

boolean uShortParameter::valueHasChanged(){
  if(_value == _lastValue){
    return false;
  }
  else{
    return true;
  }
}

void uShortParameter::setLimits(unsigned short minimum, unsigned short maximum){
  _minimum = minimum;
  _maximum = maximum;
}

void uShortParameter::loadInEEPROM(){
  EEPROM.put(_address, _value);
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

unsigned short uShortParameter::lastValue(){
  return _lastValue;
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

byteParameter::byteParameter(){}
byteParameter::~byteParameter(){}

void byteParameter::setAddress(int address){
  _address = address;
}
int byteParameter::address(){
  return _address;
}
void byteParameter::setValue(byte value){
  setLastValue(_value);
  _value = value;
}

void byteParameter::setLastValue(byte lastValue){
    _lastValue = lastValue;
}

void byteParameter::updateLastValue(){
  setLastValue(_value);
}


void byteParameter::setLimits (byte minimum, byte maximum){
  _minimum = minimum;
  _maximum = maximum;
}

void byteParameter::loadInEEPROM(){
  EEPROM.update(_address, _value);
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

byte byteParameter::lastValue(){
  return _lastValue;
}

boolean byteParameter::valueHasChanged(){
  if(_value == _lastValue){
    return false;
  }
  else{
    return true;
  }
}
//boolean parameter

boolParameter::boolParameter(){}
boolParameter::~boolParameter(){}

void boolParameter::setAddress(int address){
  _address = address;
}
int boolParameter::address(){
  return _address;
}
void boolParameter::setValue(bool value){
  setLastValue(_value);
  _value = value;
}

void boolParameter::setLastValue(bool lastValue){
    _lastValue = lastValue;
}

void boolParameter::updateLastValue(){
  setLastValue(_value);
}

void boolParameter::loadInEEPROM(){
  EEPROM.update(_address, _value);
}
boolean boolParameter::value(){
  return _value;
}

boolean boolParameter::lastValue(){
  return _lastValue;
}

boolean boolParameter::valueHasChanged(){
  if(_value == _lastValue){
    return false;
  }
  else{
    return true;
  }
}

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
short timeParameter::hour(){
  return _hour;
}
short timeParameter::minut(){
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