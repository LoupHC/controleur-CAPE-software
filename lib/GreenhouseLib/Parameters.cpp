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

void Parameter::setAddress(int address){
  _address = address;
}
int Parameter::address(){
  return _address;
}



floatParameter::floatParameter(){
  setAddress(DEF_ADDRESS);
}
floatParameter::~floatParameter(){}


void floatParameter::setValue(float value){
  if(address() != DEF_ADDRESS){
      EEPROM.put(address(), value);
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
  if(address() != DEF_ADDRESS){
    float variable;
    EEPROM.get(address(), variable);

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
  setAddress(DEF_ADDRESS);
}
shortParameter::~shortParameter(){}

void shortParameter::setValue(short value){
  if(address() != DEF_ADDRESS){
      EEPROM.put(address(), value);

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
  if(address() != DEF_ADDRESS){
    short variable;
    EEPROM.get(address(), variable);

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
  setAddress(DEF_ADDRESS);
}
uShortParameter::~uShortParameter(){}


void uShortParameter::setValue(unsigned short value){
  if(address() != DEF_ADDRESS){
        EEPROM.put(address(), value);
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
  if(address() != DEF_ADDRESS){
    unsigned short variable;
    EEPROM.get(address(), variable);

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
  setAddress(DEF_ADDRESS);
}
byteParameter::~byteParameter(){}

void byteParameter::setValue(byte value){
  if(address() != DEF_ADDRESS){
      EEPROM.put(address(), value);
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
  if(address() != DEF_ADDRESS){
    byte variable;
    EEPROM.get(address(), variable);

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
  setAddress(DEF_ADDRESS);
}
boolParameter::~boolParameter(){}

void boolParameter::setValue(bool value){
  if(address() != DEF_ADDRESS){
      EEPROM.put(address(), value);
  }
}

boolean boolParameter::value(){
  if(address() != DEF_ADDRESS){
    boolean variable;
    EEPROM.get(address(), variable);

    return variable;
  }
  return false;
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
