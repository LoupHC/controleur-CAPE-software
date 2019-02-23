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


Greenhouse::Greenhouse(int timez, float lat, float longit, byte t, byte r, byte s, byte f, byte h){
  _localIndex = GREENHOUSE_INDEX;
  timezone.setLimits(-12, 14);
  timezone.setAddress(_localIndex);
  _localIndex += sizeof(short);
  latitude.setLimits(-90, 90);
  latitude.setAddress(_localIndex);
  _localIndex += sizeof(float);
  longitude.setLimits(-180, 180);
  longitude.setAddress(_localIndex);
  _localIndex += sizeof(float);
  timepoints.setLimits(1, MAX_TIMEPOINTS);
  timepoints.setAddress(_localIndex);
  _localIndex += sizeof(byte);
  rollups.setLimits(0, MAX_ROLLUPS);
  rollups.setAddress(_localIndex);
  _localIndex += sizeof(byte);
  stages.setLimits(0, MAX_ROLLUPS);
  stages.setAddress(_localIndex);
  _localIndex += sizeof(byte);
  fans.setLimits(0, MAX_FANS);
  fans.setAddress(_localIndex);
  _localIndex += sizeof(byte);
  heaters.setLimits(0, MAX_HEATERS);
  heaters.setAddress(_localIndex);
  _localIndex += sizeof(byte);

  timepoints.setValue(t);
  rollups.setValue(r);
  stages.setValue(s);
  fans.setValue(f);
  heaters.setValue(h);
  timezone.setValue(timez);
  latitude.setValue(lat);
  longitude.setValue(longit);

  _weather = SUN;
  _ramping = false;
  _overrideProgramCounter = 0;

  for (byte x = 0; x < rollups.value(); x++){
    rollup[x].setStages(s);
  }
}

Greenhouse::Greenhouse(){
  _localIndex = GREENHOUSE_INDEX;

  timezone.setValue(-5);
  latitude.setValue(46);
  longitude.setValue(-73);
  timepoints.setValue(MAX_TIMEPOINTS);
  rollups.setValue(MAX_ROLLUPS);
  stages.setValue(MAX_STAGES);
  fans.setValue(MAX_FANS);
  heaters.setValue(MAX_HEATERS);

  timezone.setLimits(-12, 14);
  timezone.setAddress(_localIndex);
  _localIndex += sizeof(short);

  latitude.setLimits(-90, 90);
  latitude.setAddress(_localIndex);
  _localIndex += sizeof(float);

  longitude.setLimits(-180, 180);
  longitude.setAddress(_localIndex);
  _localIndex += sizeof(float);

  timepoints.setLimits(1, MAX_TIMEPOINTS);
  timepoints.setAddress(_localIndex);
  _localIndex += sizeof(byte);

  rollups.setLimits(0, MAX_ROLLUPS);
  rollups.setAddress(_localIndex);
  _localIndex += sizeof(byte);

  stages.setLimits(0, MAX_STAGES);
  stages.setAddress(_localIndex);
  _localIndex += sizeof(byte);

  fans.setLimits(0, MAX_FANS);
  fans.setAddress(_localIndex);
  _localIndex += sizeof(byte);

  heaters.setLimits(0, MAX_HEATERS);
  heaters.setAddress(_localIndex);
  _localIndex += sizeof(byte);


  _weather = SUN;
  _ramping = false;
  _overrideProgramCounter = 0;
}

Greenhouse::~Greenhouse(){
}

void Greenhouse::initGreenhouse(short timez, float lat, float longit, byte t, byte r, byte s, byte f, byte h){

  timepoints.setValue(t);
  rollups.setValue(r);
  stages.setValue(s);
  fans.setValue(f);
  heaters.setValue(h);
  timezone.setValue(timez);
  latitude.setValue(lat);
  longitude.setValue(longit);

  for (byte x = 0; x < rollups.value(); x++){
    rollup[x].setStages(s);
  }
}

void Greenhouse::setNow(byte rightNow[6]){

  for(byte x = 0; x < 6 ; x++){
      _rightNow[x] = rightNow[x];
  }
  if(timezone.value() == -5){
    myLord.DST(_rightNow);
  }

  #ifdef DEBUG_SOLARCALC
    if(_rightNow[2]!=rightNow[2]){
        Serial.println("Heure avancée");
    }
    else{
        Serial.println("Heure normale");
    }
  #endif
}


void Greenhouse::fullRoutine(byte rightNow[6], float greenhouseTemperature){
  setNow(rightNow);
  solarCalculations();
  checkProgramSuccession();
  selectActualProgram();
  startRamping();
  #if MAX_ROLLUPS >= 1
  for (byte x = 0; x < rollups.value(); x++){
      rollup[x].routine(_coolingTemp, greenhouseTemperature);
    //}
  }
  #endif
  #if MAX_FANS >= 1
  for (byte x = 0; x < fans.value(); x++){
      fan[x].routine(_coolingTemp, greenhouseTemperature);
    //}
  }
  #endif
  #if MAX_HEATERS >= 1
  for (byte x = 0; x < heaters.value(); x++){
      heater[x].routine(_heatingTemp, greenhouseTemperature);
    //}
  }
  #endif
}


void Greenhouse::fullRoutine(byte rightNow[6], float* coolingTemp, float* heatingTemp){
  setNow(rightNow);
  solarCalculations();
  checkProgramSuccession();
  selectActualProgram();
  startRamping();
  *coolingTemp = _coolingTemp;
  *heatingTemp = _heatingTemp;
}

void Greenhouse::solarCalculations(){
  initTimeLord(timezone.value(), latitude.value(), longitude.value());
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
  for(int x = 0; x < timepoints.value();x++){
    if(timepoint[x].type.value() == SR){
      timepoint[x].updateTimepoint();
    }
  }

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
  for(int x = 0; x < timepoints.value();x++){
    if(timepoint[x].type.value() == SS){
      timepoint[x].updateTimepoint();
    }
  }
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
  if(timepoints.value() > 1){
    for(int x = 1; x < timepoints.value();x++){
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
    for (byte y = 0; y < (timepoints.value()-1); y++){

    #ifdef DEBUG_PROGRAM
      Serial.print ("Programme "); Serial.print(y+1);Serial.print(" : ");Serial.print(timepoint[y].hr());Serial.print(" : ");Serial.println(timepoint[y].mn());
    #endif
      if (((_rightNow[HOUR] == timepoint[y].hr())  && (_rightNow[MINUT] >= timepoint[y].mn()))||((_rightNow[HOUR] > timepoint[y].hr()) && (_rightNow[HOUR] < timepoint[y+1].hr()))||((_rightNow[HOUR] == timepoint[y+1].hr())  && (_rightNow[MINUT] <timepoint[y+1].mn()))){
            _timepoint = y+1;
      }
    }

    #ifdef DEBUG_PROGRAM
      Serial.print ("Programme ");Serial.print(timepoints.value());Serial.print(" : ");Serial.print(timepoint[timepoints.value()-1].hr());Serial.print(" : ");Serial.println(timepoint[timepoints.value()-1].mn());
    #endif

    if (((_rightNow[HOUR] == timepoint[timepoints.value()-1].hr())  && (_rightNow[MINUT] >= timepoint[timepoints.value()-1].mn()))||(_rightNow[HOUR] > timepoint[timepoints.value()-1].hr())||(_rightNow[HOUR] < timepoint[0].hr())||((_rightNow[HOUR] == timepoint[0].hr())  && (_rightNow[MINUT] < timepoint[0].mn()))){
      _timepoint = timepoints.value();
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
      _coolingTemp += 1;
      if(_coolingTemp > _newCoolingTemp){
        _coolingTemp = _newCoolingTemp;
      }
    }
    else if (_newCoolingTemp < _coolingTemp){
      _coolingTemp -= 1;
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

byte Greenhouse::hr(){
  return _rightNow[2];
}

byte Greenhouse::mn(){
  return _rightNow[1];
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

void Greenhouse::EEPROMGet(){
  for (byte x = 0; x < rollups.value(); x++){
    rollup[x].setStages(stages.value());
  }

  #ifdef DEBUG_EEPROM
    Serial.println(F("-------------------"));
    Serial.print(F("Address: "));
    Serial.print(timezone.address());
    Serial.print(F(" - Value :"));
    Serial.print(timezone.value());
    Serial.println(F(" - (Timezone)"));
    Serial.print(F("Address: "));
    Serial.print(latitude.address());
    Serial.print(F(" - Value :"));
    Serial.print(latitude.value());
    Serial.println(F("   - (Latitude)"));
    Serial.print(F("Address: "));
    Serial.print(longitude.address());
    Serial.print(F(" - Value :"));
    Serial.print(longitude.value());
    Serial.println(F("   - (Longitude)"));
    Serial.print(F("Address: "));
    Serial.print(timepoints.address());
    Serial.print(F(" - Value :"));
    Serial.print(timepoints.value());
    Serial.println(F("   - (Timepoints)"));
    Serial.print(F("Address: "));
    Serial.print(rollups.address());
    Serial.print(F(" - Value :"));
    Serial.print(rollups.value());
    Serial.println(F("   - (Rollups)"));
    Serial.print(F("Address: "));
    Serial.print(stages.address());
    Serial.print(F(" - Value :"));
    Serial.print(stages.value());
    Serial.println(F("   - (Stages)"));
    Serial.print(F("Address: "));
    Serial.print(fans.address());
    Serial.print(F(" - Value :"));
    Serial.print(fans.value());
    Serial.println(F("   - (Fans)"));
    Serial.print(F("Address: "));
    Serial.print(heaters.address());
    Serial.print(F(" - Value :"));
    Serial.print(heaters.value());
    Serial.println(F("   - (Heaters)"));

  for (int x = 0; x < MAX_CLOCK_OV;x++){
    if(clockOv[x].enabled.value()){
      Serial.print(F("-------CLOCK OVERRIDE "));
      Serial.print(clockOv[x].ID.value());
      Serial.println(F("-------"));
      Serial.print(F("Address: "));
      Serial.print(clockOv[x].hrStart.address());
      Serial.print(F(" - Value :"));
      Serial.print(clockOv[x].hrStart.value());
      Serial.println(F(" - (Start hour)"));
      Serial.print(F("Address: "));
      Serial.print(clockOv[x].mnStart.address());
      Serial.print(F(" - Value :"));
      Serial.print(clockOv[x].mnStart.value());
      Serial.println(F("   - (Start minute) "));
      Serial.print(F("Address: "));
      Serial.print(clockOv[x].hrStop.address());
      Serial.print(F(" - Value :"));
      Serial.print(clockOv[x].hrStop.value());
      Serial.println(F("   - (Stop hour) "));
      Serial.print(F("Address: "));
      Serial.print(clockOv[x].mnStop.address());
      Serial.print(F(" - Value :"));
      Serial.print(clockOv[x].mnStop.value());
      Serial.println(F("   - (Stop minute) "));
      Serial.print(F("Address: "));
      Serial.print(clockOv[x].target.address());
      Serial.print(F(" - Value :"));
      Serial.print(clockOv[x].target.value());
      Serial.println(F("   - (Target increment) "));
    }
  }
  for (int x = 0; x < MAX_FLOAT_OV;x++){
    if(floatOv[x].enabled.value()){
      Serial.print(F("-------FLOAT OVERRIDE "));
      Serial.print(floatOv[x].ID.value());
      Serial.println(F("-------"));
      Serial.print(F("Address: "));
      Serial.print(floatOv[x].comparator.address());
      Serial.print(F(" - Value :"));
      if(floatOv[x].comparator.value() == ABOVE){
        Serial.print (" ABOVE ");
      }
      else if(floatOv[x].comparator.value() == BELOW){
        Serial.print (" BELOW ");
      }
      Serial.println(F("   - (Comparator) "));
      Serial.print(F("Address: "));
      Serial.print(floatOv[x].hyst.address());
      Serial.print(F(" - Value :"));
      Serial.print(floatOv[x].hyst.value());
      Serial.println(F("   - (Hysteresis) "));
      Serial.print(F("Address: "));
      Serial.print(floatOv[x].floatVar.address());
      Serial.print(F(" - Value :"));
      Serial.print(floatOv[x].floatVar.value());
      Serial.println(F("   - (Treshold) "));
      Serial.print(F("Address: "));
      Serial.print(floatOv[x].target.address());
      Serial.print(F(" - Value :"));
      Serial.print(floatOv[x].target.value());
      Serial.println(F("   - (Target increment) "));
    }
  }

  for (int x = 0; x < MAX_BOOL_OV;x++){
    if(boolOv[x].enabled.value()){
      Serial.print(F("-------BOOL OVERRIDE "));
      Serial.print(boolOv[x].ID.value());
      Serial.println(F("-------"));
      Serial.print(F("Address: "));
      Serial.print(boolOv[x].target.address());
      Serial.print(F(" - Value :"));
      Serial.print(boolOv[x].target.value());
      Serial.println(F("   - (Target increment) "));
    }
  }
  #endif
}
