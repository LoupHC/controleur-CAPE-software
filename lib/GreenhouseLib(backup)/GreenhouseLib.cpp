/*
  GreenhouseLib.cpp
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
#include "GreenhouseLib.h"


//Create an alarm instance
Alarm::Alarm(boolean type, byte alarmPin){
  _alarmType = type;
  _alarmPin = alarmPin;
  for(int x = 0; x < MAX_ALARM_SEQUENCES;x++){
    sequence[x].onTime = 0;
    sequence[x].offTime = 0;
  }
  _sequencePlaying = OFF;
  counter = 0;
}
Alarm::~Alarm(){}
//define alarm output (as IO output or MCP23008 output)
void Alarm::init(){
  #ifndef MCP_I2C_OUTPUTS
      pinMode(_alarmPin, OUTPUT);
      if(_alarmType == ACT_LOW){
        digitalWrite(_alarmPin, HIGH);
      }
      else{
        digitalWrite(_alarmPin, LOW);
      }
  #endif
    #ifdef MCP_I2C_OUTPUTS
        mcp.pinMode(_alarmPin, OUTPUT);
        if(_alarmType == ACT_LOW){
          mcp.digitalWrite(_alarmPin, HIGH);
        }
        else{
          mcp.digitalWrite(_alarmPin, LOW);
        }
  #endif
}
//create alarm sequence (# of seconds the alarm is on + # of secondes the alarm is off)
//For constant alarm, define "offTime" as 0
void Alarm::addSequence(byte number, unsigned short onTime, unsigned short offTime){
  if((number >= MAX_ALARM_SEQUENCES)||(number == 0)){
    return;
  }
  sequence[number].onTime = onTime;
  sequence[number].offTime = offTime;
}
//above a certain value, alarm will execute default sequence (constant alarm)
void Alarm::above(float parameter, float value){
    if(parameter > value){
      _sequencePlaying = 0;
    }
    else{
      if(_sequencePlaying == 0){
        _sequencePlaying = OFF;
      }
    }
}
//above a certain value, alarm will execute a specified sequence  (constant alarm)
void Alarm::above(float parameter, float value, byte sequence){
    if(parameter > value){
      _sequencePlaying = sequence;
    }
    else{
      if(_sequencePlaying == sequence){
        _sequencePlaying = OFF;
      }
    }
  }
//under a certain value, alarm will execute default sequence (constant alarm)
void Alarm::below(float parameter, float value){
    if(parameter < value){
      _sequencePlaying = 0;
    }
    else{
      if(_sequencePlaying == 0){
        _sequencePlaying = OFF;
      }
    }
}
//under a certain value, alarm will execute a specified sequence (constant alarm)
void Alarm::below(float parameter, float value, byte sequence){
    if(parameter < value){
      _sequencePlaying = sequence;
    }
    else{
      if(_sequencePlaying == sequence){
        _sequencePlaying = OFF;
      }
    }
}
//under or above a range of value, alarm will execute default sequence (constant alarm)

void Alarm::offRange(float parameter, float minimum, float maximum){
    if((parameter < minimum)||(parameter > maximum)){
      _sequencePlaying = 0;
    }
    else{
      if(_sequencePlaying == 0){
        _sequencePlaying = OFF;
      }
    }
}
//under or above a range of value, alarm will execute a specified sequence (constant alarm)
void Alarm::offRange(float parameter, float minimum, float maximum, byte sequence){
    if((parameter < minimum)||(parameter > maximum)){
      _sequencePlaying = sequence;
    }
    else{
      if(_sequencePlaying == sequence){
        _sequencePlaying = OFF;
      }
    }
}
//if argument "value" is true, alarm will execute default sequence  (constant alarm)
void Alarm::conditionalTo(boolean value){
    if(value == true){
      _sequencePlaying = 0;
    }
    else{
      if(_sequencePlaying == 0){
        _sequencePlaying = OFF;
      }
    }
}
//if argument "value" is true, alarm will execute a specified sequence
void Alarm::conditionalTo(boolean value, byte sequence){
  if(sequence >= MAX_ALARM_SEQUENCES){
    return;
  }
  if(value == true){
      _sequencePlaying = sequence;
  }
  else{
    if(_sequencePlaying == sequence){
      _sequencePlaying = OFF;
    }
  }
}
//activate or desactivate the alarm
void Alarm::activate(boolean state){
  if(state == true){
    #ifndef MCP_I2C_OUTPUTS
        if(_alarmType == ACT_LOW){
          digitalWrite(_alarmPin, LOW);
        }
        else{
          digitalWrite(_alarmPin, HIGH);
        }
    #endif
      #ifdef MCP_I2C_OUTPUTS
          if(_alarmType == ACT_LOW){
            mcp.digitalWrite(_alarmPin, LOW);
          }
          else{
            mcp.digitalWrite(_alarmPin, HIGH);
          }
    #endif
  }
  else if(state == false){
    #ifndef MCP_I2C_OUTPUTS
        if(_alarmType == ACT_LOW){
          digitalWrite(_alarmPin, HIGH);
        }
        else{
          digitalWrite(_alarmPin, LOW);
        }
    #endif
    #ifdef MCP_I2C_OUTPUTS
        if(_alarmType == ACT_LOW){
          mcp.digitalWrite(_alarmPin, HIGH);
        }
        else{
          mcp.digitalWrite(_alarmPin, LOW);
        }
    #endif
  }
}
//execute the selected sequence
void Alarm::checkAlarm(){
  //Serial.print("counter : ");
  //Serial.println(counter);
  //if(_sequencePlaying != OFF){
  //Serial.print("offtime : ");
  //Serial.println(sequence[_sequencePlaying].offTime);
  //Serial.print("ontime : ");
  //Serial.println(sequence[_sequencePlaying].onTime);
  //}
  if(_sequencePlaying == OFF){
    activate(false);
    counter = 0;
  }
  else if(sequence[_sequencePlaying].offTime == 0){
    activate(true);
    counter = 0;
  }
  else{
    if(counter < sequence[_sequencePlaying].onTime){
      activate(true);

    }
    else if(counter < (sequence[_sequencePlaying].onTime+sequence[_sequencePlaying].offTime)){
      activate(false);
    }
    else{
      counter = 0;
    }
  }
}


Greenhouse::Greenhouse(int timezone, float latitude, float longitude, byte timepoints, byte rollups, byte stages, byte fans, byte heaters){
  _timepoints = timepoints;
  _rollups = rollups;
  _stages = stages;
  _fans = fans;
  _heaters = heaters;
  _timezone = timezone;
  _latitude = latitude;
  _longitude = longitude;
  _weather = SUN;
  _ramping = false;
  _overrideProgramCounter = 0;
  for (byte x = 0; x < _rollups; x++){
    rollup[x].setStages(_stages);
  }
  for (byte y = 0; y < MAX_OVERRIDE_PROGRAMS; y++){
    overrideProgram[y].active = false;
    overrideProgram[y].ID = OFF_VAL;
    overrideProgram[y].priority = 0;
    for(int x = 0;x < MAX_ROLLUPS;x++){
        overrideProgram[y].rollup[x] = false;
    }
    for(int x = 0;x < MAX_FANS;x++){
        overrideProgram[y].fan[x] = false;
    }
    for(int x = 0;x < MAX_HEATERS;x++){
        overrideProgram[y].heater[x] = false;
    }
  }
}

Greenhouse::~Greenhouse(){
}

void Greenhouse::setNow(byte rightNow[6]){

  for(byte x = 0; x < 6 ; x++){
      _rightNow[x] = rightNow[x];
  }

    myLord.DST(_rightNow);
}


void Greenhouse::fullRoutine(byte rightNow[6], float greenhouseTemperature){
  setNow(rightNow);
  solarCalculations();
  checkProgramSuccession();
  selectActualProgram();
  startRamping();
  #if MAX_ROLLUPS >= 1
  for (byte x = 0; x < _rollups; x++){
    rollup[x].EEPROMPut();
    rollup[x].routine(_coolingTemp, greenhouseTemperature);
  }
  #endif
  #if MAX_FANS >= 1
  for (byte x = 0; x < _fans; x++){
    fan[x].EEPROMPut();
    fan[x].routine(_coolingTemp, greenhouseTemperature);
  }
  #endif
  #if MAX_HEATERS >= 1
  for (byte x = 0; x < _heaters; x++){
    heater[x].EEPROMPut();
    heater[x].routine(_heatingTemp, greenhouseTemperature);
  }
  #endif
  for (byte x = 0; x < _timepoints; x++){
    timepoint[x].EEPROMPut();
  }
}


void Greenhouse::fullRoutine(byte rightNow[6], float* coolingTemp, float* heatingTemp){
  setNow(rightNow);
  solarCalculations();
  checkProgramSuccession();
  selectActualProgram();
  startRamping();
  #if MAX_ROLLUPS >= 1
  for (byte x = 0; x < _rollups; x++){
    rollup[x].EEPROMPut();
  }
  #endif
  #if MAX_FANS >= 1
  for (byte x = 0; x < _fans; x++){
    fan[x].EEPROMPut();
  }
  #endif
  #if MAX_HEATERS >= 1
  for (byte x = 0; x < _heaters; x++){
    heater[x].EEPROMPut();
  }
  #endif
  for (byte x = 0; x < _timepoints; x++){
    timepoint[x].EEPROMPut();
  }
  *coolingTemp = _coolingTemp;
  *heatingTemp = _heatingTemp;
}

void Greenhouse::setOverrideProgram(unsigned short ID, boolean state){
  for(int x = 0; x < MAX_OVERRIDE_PROGRAMS; x++){
    if (overrideProgram[x].ID == ID){
      overrideProgram[x].active = state;

    }
    if(state == true){
      establishPriority(x);
    }
    if(state == false){
      clearOverrides(x);
    }
  }
}
void Greenhouse::establishPriority(byte index){
  for(int x = 0; x < MAX_OVERRIDE_PROGRAMS; x++){
    if((overrideProgram[x].ID != overrideProgram[index].ID)&&(overrideProgram[x].priority == overrideProgram[index].priority)){
      setOverrideProgram(overrideProgram[x].ID, false);
    }
  }
}

void Greenhouse::clearOverrides(byte index){
      for(int x = 0;x < MAX_ROLLUPS;x++){
        if(overrideProgram[index].rollup[x] == true){
          rollup[x].setOverride(overrideProgram[index].ID, false);
          overrideProgram[index].rollup[x] = false;
        }
      }
      for(int x = 0;x < MAX_FANS;x++){
        if(overrideProgram[index].fan[x] == true){
          fan[x].setOverride(overrideProgram[index].ID, false);
          overrideProgram[index].fan[x] = false;
        }
      }
      for(int x = 0;x < MAX_HEATERS;x++){
        if(overrideProgram[index].heater[x] == true){
          heater[x].setOverride(overrideProgram[index].ID, false);
          overrideProgram[index].heater[x] = false;
        }
      }
}

void Greenhouse::createOverrideProgram(unsigned short ID, byte priorityIndex){
  byte _localCounter = _overrideProgramCounter;
  _overrideProgramCounter++;
  if(_localCounter < MAX_OVERRIDE_PROGRAMS){
    overrideProgram[_localCounter].ID = ID;
    overrideProgram[_localCounter].priority = priorityIndex;
  }
}

byte Greenhouse::overridePriority(unsigned short ID){
  for(int x = 0; x < MAX_OVERRIDE_PROGRAMS; x++){
    if (overrideProgram[x].ID == ID){
      return overrideProgram[x].priority;
    }
  }
  return OFF_VAL;
}
boolean Greenhouse::overrideIsActive(unsigned short ID){
  for(int x = 0; x < MAX_OVERRIDE_PROGRAMS; x++){
    if (overrideProgram[x].ID == ID){
      return overrideProgram[x].active;
    }
  }
  return false;
}

void Greenhouse::setOutput(unsigned short ID, Rollup output, unsigned short target){
  for(int x = 0; x < MAX_OVERRIDE_PROGRAMS; x++){
    if ((overrideProgram[x].ID == ID)&&(overrideProgram[x].active == true)){
      overrideProgram[x].rollup[output.nb()] = true;
      if((rollup[output.nb()].overRide[overridePriority(ID)].target.value() != target)){
        rollup[output.nb()].initOverride(overridePriority(ID), target);
      }
      rollup[output.nb()].setOverride(overridePriority(ID), true);
    }
  }
}

void Greenhouse::setOutput(unsigned short ID, Fan output, boolean state){
  for(int x = 0; x < MAX_OVERRIDE_PROGRAMS; x++){
    if ((overrideProgram[x].ID == ID)&&(overrideProgram[x].active == true)){
      overrideProgram[x].fan[output.nb()] = true;
      if(fan[output.nb()].overRide[overridePriority(ID)].state != state){
        fan[output.nb()].initOverride(overridePriority(ID), state);
      }
      fan[output.nb()].setOverride(overridePriority(ID), true);
    }
  }
}

void Greenhouse::setOutput(unsigned short ID, Heater output, boolean state){
  for(int x = 0; x < MAX_OVERRIDE_PROGRAMS; x++){
    if ((overrideProgram[x].ID == ID)&&(overrideProgram[x].active == true)){
      overrideProgram[x].heater[output.nb()] = true;
      if(heater[output.nb()].overRide[overridePriority(ID)].state != state){
        heater[output.nb()].initOverride(overridePriority(ID), state);
      }
      heater[output.nb()].setOverride(overridePriority(ID), true);
    }
  }
}

void Greenhouse::disableOutput(unsigned short ID, Rollup output){
  for(int x = 0; x < MAX_OVERRIDE_PROGRAMS; x++){
    if ((overrideProgram[x].ID == ID)&&(overrideProgram[x].active == true)){
      overrideProgram[x].rollup[output.nb()] = false;
      rollup[output.nb()].setOverride(overridePriority(ID), false);
    }
  }
}
void Greenhouse::disableOutput(unsigned short ID, Fan output){
  for(int x = 0; x < MAX_OVERRIDE_PROGRAMS; x++){
    if ((overrideProgram[x].ID == ID)&&(overrideProgram[x].active == true)){
      overrideProgram[x].fan[output.nb()] = false;
      fan[output.nb()].setOverride(overridePriority(ID), false);
    }
  }
}
void Greenhouse::disableOutput(unsigned short ID, Heater output){
  for(int x = 0; x < MAX_OVERRIDE_PROGRAMS; x++){
    if ((overrideProgram[x].ID == ID)&&(overrideProgram[x].active == true)){
      overrideProgram[x].heater[output.nb()] = false;
      heater[output.nb()].setOverride(overridePriority(ID), false);
    }
  }
}



void Greenhouse::solarCalculations(){
  initTimeLord(_timezone, _latitude, _longitude);
  //Première lecture d'horloge pour définir le lever et coucher du soleil
  setSunrise();
  setSunset();
}

void Greenhouse::initTimeLord(int timezone, float latitude, float longitude){
  myLord.TimeZone(timezone * 60);
  myLord.Position(latitude, longitude);
  myLord.DstRules(3,2,11,1,60); // DST Rules for USA
}


void Greenhouse::setSunrise(){
  //Définit l'heure du lever et coucher du soleil
  for(byte x = 0; x < 6 ; x++){
      _sunTime[x] = _rightNow[x];
  }
  myLord.SunRise(_sunTime); ///On détermine l'heure du lever du soleil
  Timepoint::sunRise[HOUR] = (short)_sunTime[HOUR];
  Timepoint::sunRise[MINUT] = (short)_sunTime[MINUT];

  #ifdef DEBUG_SOLARCALC
    Serial.print("lever du soleil :");Serial.print(Timepoint::sunRise[HOUR]);  Serial.print(":");  Serial.println(Timepoint::sunRise[MINUT]);
    Serial.println("----");
  #endif
}

void Greenhouse::setSunset(){
  // Sunset:
  for(byte x = 0; x < 6 ; x++){
      _sunTime[x] = _rightNow[x];
  }
  myLord.SunSet(_sunTime); // Computes Sun Set. Prints:
  Timepoint::sunSet[HOUR] = (short)_sunTime[HOUR];
  Timepoint::sunSet[MINUT] = (short)_sunTime[MINUT];
  #ifdef DEBUG_SOLARCALC
    Serial.print("coucher du soleil :");  Serial.print(Timepoint::sunSet[HOUR]);  Serial.print(":");  Serial.println(Timepoint::sunSet[MINUT]);
    Serial.println("----");
  #endif
}
void Greenhouse::startingTime(byte rightNow[6]){
  setNow(rightNow);
  solarCalculations();
}
void Greenhouse::startingParameters(){
  //Exécution du programme
  selectActualProgram();
  setTempCible();

}
void Greenhouse::checkProgramSuccession(){
  if(_timepoints > 1){
    for(int x = 1; x < _timepoints;x++){
      if (((timepoint[x].hr() == timepoint[x-1].hr())  && (timepoint[x].mn() < timepoint[x-1].mn()))||(timepoint[x].hr() < timepoint[x-1].hr())){
        timepoint[x].type.setValue(timepoint[x-1].type.value());
        timepoint[x].setTimepoint(timepoint[x-1].hrMod.value(),timepoint[x-1].mnMod.value());
      }
    }
  }
}
void Greenhouse::selectActualProgram(){
  //Sélectionne le programme en cour

    #ifdef DEBUG_PROGRAM
      Serial.println("----");
      Serial.print ("Heure actuelle ");Serial.print(" : ");Serial.print(_rightNow[HOUR] );Serial.print(" : ");Serial.println(_rightNow[MINUT]);
    #endif
    for (byte y = 0; y < (_timepoints-1); y++){

    #ifdef DEBUG_PROGRAM
      Serial.print ("Programme "); Serial.print(y+1);Serial.print(" : ");Serial.print(timepoint[y].hr());Serial.print(" : ");Serial.println(timepoint[y].mn());
    #endif
      if (((_rightNow[HOUR] == timepoint[y].hr())  && (_rightNow[MINUT] >= timepoint[y].mn()))||((_rightNow[HOUR] > timepoint[y].hr()) && (_rightNow[HOUR] < timepoint[y+1].hr()))||((_rightNow[HOUR] == timepoint[y+1].hr())  && (_rightNow[MINUT] <timepoint[y+1].mn()))){
            _timepoint = y+1;
      }
    }

    #ifdef DEBUG_PROGRAM
      Serial.print ("Programme ");Serial.print(_timepoints);Serial.print(" : ");Serial.print(timepoint[_timepoints-1].hr());Serial.print(" : ");Serial.println(timepoint[_timepoints-1].mn());
    #endif

    if (((_rightNow[HOUR] == timepoint[_timepoints-1].hr())  && (_rightNow[MINUT] >= timepoint[_timepoints-1].mn()))||(_rightNow[HOUR] > timepoint[_timepoints-1].hr())||(_rightNow[HOUR] < timepoint[0].hr())||((_rightNow[HOUR] == timepoint[0].hr())  && (_rightNow[MINUT] < timepoint[0].mn()))){
      _timepoint = _timepoints;
    }
    #ifdef DEBUG_PROGRAM
      Serial.print ("Program is : ");
      Serial.println(_timepoint);
      Serial.println("----");
    #endif
}

void Greenhouse::setTempCible(){
  if(_weather == SUN){
    _coolingTemp = timepoint[_timepoint-1].coolingTemp.value();
    _heatingTemp = timepoint[_timepoint-1].heatingTemp.value();
  }
  else if(_weather == CLOUD){
      _coolingTemp = timepoint[_timepoint-1].coolingTempCloud.value();
      _heatingTemp = timepoint[_timepoint-1].heatingTempCloud.value();
  }
}

void Greenhouse::startRamping(){
  if(_weather == SUN){
    _newCoolingTemp = timepoint[_timepoint-1].coolingTemp.value();
    _newHeatingTemp = timepoint[_timepoint-1].heatingTemp.value();
  }
  else if(_weather == CLOUD){
      _newCoolingTemp = timepoint[_timepoint-1].coolingTempCloud.value();
      _newHeatingTemp = timepoint[_timepoint-1].heatingTempCloud.value();
  }

  unsigned long rampTime = (unsigned long)timepoint[_timepoint-1].ramping.value()*60000;

  if (ramping > rampTime){

    if (_newCoolingTemp > _coolingTemp){
      _coolingTemp += 0.5;
      if(_coolingTemp > _newCoolingTemp){
        _coolingTemp = _newCoolingTemp;
      }
    }
    else if (_newCoolingTemp < _coolingTemp){
      _coolingTemp -= 0.5;
      if(_coolingTemp < _newCoolingTemp){
        _coolingTemp = _newCoolingTemp;
      }
    }
    if (_newHeatingTemp > _heatingTemp){
      _heatingTemp += 0.5;
      if(_heatingTemp > _newHeatingTemp){
        _heatingTemp = _newHeatingTemp;
      }
    }
    else if (_newHeatingTemp < _heatingTemp){
      _heatingTemp -= 0.5;
      if(_heatingTemp < _newHeatingTemp){
        _heatingTemp = _newHeatingTemp;
      }
    }
    ramping = 0;
  }
  if((_newCoolingTemp == _coolingTemp)&&(_newHeatingTemp == _heatingTemp)){
    _ramping = false;
  }
  else{
    _ramping = true;
  }
}

void Greenhouse::setWeather(byte weather){
  _weather = weather;
}

byte Greenhouse::rightNow(byte index){
  return _rightNow[index];
}

byte Greenhouse::weather(){
  return _weather;
}
byte Greenhouse::nowTimepoint(){
  return _timepoint;
}
float Greenhouse::heatingTemp(){
  return _heatingTemp;
}
float Greenhouse::coolingTemp(){
  return _coolingTemp;
}
boolean Greenhouse::isRamping(){
  if (_ramping == true){
    return true;
  }
  else{
    return false;
  }
}
