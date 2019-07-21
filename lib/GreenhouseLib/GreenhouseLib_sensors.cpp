/*
  GreenhouseLib.cpp
  Copyright (C)2017 Loup Hébert-Chartrand. All right reserved

  You can find the latest version of this code at :
  https://github.com/LoupHC/GreenhouseLib

  This library is free software{} you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation{} either
  version 2.1 of the License, or (at your option) any later version.

  This code is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY{} without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library{} if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/
#include "GreenhouseLib_sensors.h"



Sensor::Sensor(){
  clearRecords();
}

Sensor::~Sensor(){}

float Sensor::value(){
  return _value;
}

//get the average ofevery values recorded in average24h
float Sensor::average24h(){
  float sumOfData = 0;
  int dataCount = 0;
  float average = 0;

  for (int x = 0; x < 24; x++){
    if(_average24h[x] != OFF_VAL){
      sumOfData += _average24h[x];
      dataCount++;
    }
  }
//if theres no records in _average24h array, check _average1h instead
  if(dataCount == 0){
    for (int x = 0; x < 6; x++){
      if(_average1h[x] != OFF_VAL){
        sumOfData += _average1h[x];
        dataCount++;
      }
      sumOfData += _value;
      dataCount++;
    }
  }
  average = sumOfData/dataCount;

  return average;
}

//check if some data is missing for 24h average
boolean Sensor::allDataAcquired(){
  for (int x = 0; x <24; x++){
    if(_average24h[x] == OFF_VAL){
      return true;
    }
  }
  return false;
}

//get the lowest value of every values recorded in average24h
float Sensor::min24h(){

  float minimum = _value;

  for (int x = 0; x < 6; x++){
    if((_average1h[x] < minimum)&&(_average1h[x] != OFF_VAL)){
      minimum = _average1h[x];
    }
  }
  for (int x = 0; x <24; x++){
    if((_average24h[x] < minimum)&&(_average24h[x] != OFF_VAL)){
      minimum = _average24h[x];
    }
  }

  return minimum;
}

//get the highest value of every values recorded in average24h
float Sensor::max24h(){

  float maximum = _value;

  for (int x = 0; x < 6; x++){
    if((_average1h[x] > maximum)&&(_average1h[x] != OFF_VAL)){
      maximum = _average1h[x];
    }
  }for (int x = 0; x < 24; x++){
    if((_average24h[x] > maximum)&&(_average24h[x] != OFF_VAL)){
      maximum = _average24h[x];
    }
  }

  return maximum;
}


void Sensor::clearRecords(){
    for(int x = 0; x < 24; x++){
      _average24h[x] = OFF_VAL;
    }
    for(int x = 0; x < 6; x++){
      _average1h[x] = OFF_VAL;
    }
    timer = 0;
    _recordCount = 0;
}

//record new value every 10 minuts
void Sensor::registerValue(float value){
  _value = value;
  if(timer >= 600000){
    recordNewValueInAverage1h();
    timer = 0;
  }
}

void Sensor::recordNewValueInAverage1h(){
  for (int x = 5; x > 0; x--){
    _average1h[x] = _average1h[x-1];
  }
  _average1h[0] = _value;
  _recordCount++;

  if(_recordCount == 6){
    recordNewValueInAverage24h();
    _recordCount = 0;
  }
}

// record last hour average every 60 minuts to get average24h
void Sensor::recordNewValueInAverage24h(){

  float sumOfData = 0;
  int dataCount = 0;
  float newRecordToAverage24h;

  //calculate last hour average
  for (int x = 0; x < 6; x++){
    if(_average1h[x] != OFF_VAL){
      sumOfData += _average1h[x];
      dataCount++;
    }
  }

  if(dataCount == 0){
    newRecordToAverage24h = 0;
  }
  else{
    newRecordToAverage24h = sumOfData/dataCount;
  }

  //record new average to average24h
  for (int x = 23; x > 0; x--){
    _average24h[x] = _average24h[x-1];
  }
  _average24h[0] = newRecordToAverage24h;
}



float Humidity::relativeToAbsoluteConversion(float humidity, float temperature){
  return ((6.112 * pow(2.71828,((17.67 * temperature)/(temperature + 243.5))) * humidity * 2.1674) / (273.15 + temperature));
}
//absolute humidity in g/m3
float Humidity::absolute(float temp){
  float absoluteH = relativeToAbsoluteConversion(_value, temp);
  return absoluteH;
}
//humidity deficit in g/m3
float Humidity::deficit(float temp){
  float Hdeficit = relativeToAbsoluteConversion(100, temp)-relativeToAbsoluteConversion(_value, temp);
  return Hdeficit;
}

unsigned long Lux::wattPerMeterSquare(){
  return value()*0.0079;//conversion constant from lux to w/m2 (for direct sunlight)
}

//joules/cm2/24h
unsigned long Lux::Joules24h(){
  unsigned long joules = 0;

  for (int x = 0; x < 24; x--){
    if(_average24h[x] != OFF_VAL){
      joules += _average24h[x]*0.0079/10000*3600000;
    }
  }
}

Current::Current(){
  for(int x = 0; x < 5; x++){
    _currentRead[x] = 0;
  }
  _timer = 0;
  _averageTimer = 0;
  _recordCount = 0;
}

Current::~Current(){
}


void Current::registerValue(float value){
  if(_timer >= 1000){
    for (int x = 5; x > 0; x--){
      _currentRead[x] = _currentRead[x-1];
    }
    _currentRead[0] = value;
    _recordCount++;
    _timer = 0;
  }
  if(_averageTimer >= 5000){
    getAverage();
    _averageTimer = 0;
  }
}

void Current::getAverage(){
  float sumOfData = 0;

  for (int x = 0; x < 5; x++){
      sumOfData += _currentRead[x];
  }
  //_value = sumOfData/(float)5;
}

float Current::average(){
  return _value;
}
