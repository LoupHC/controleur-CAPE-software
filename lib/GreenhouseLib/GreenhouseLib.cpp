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


Greenhouse::Greenhouse(int timez, float lat, float longit, byte t, byte r,  byte f){
  _localIndex = GREENHOUSE_INDEX;
  timezone.setAddress(_localIndex);
  _localIndex += sizeof(short);
  latitude.setAddress(_localIndex);
  _localIndex += sizeof(float);
  longitude.setAddress(_localIndex);
  _localIndex += sizeof(float);
  timepoints.setAddress(_localIndex);
  _localIndex += sizeof(byte);
  rollups.setAddress(_localIndex);
  _localIndex += sizeof(byte);
  devices.setAddress(_localIndex);
  _localIndex += sizeof(byte);
  daynight.setAddress(_localIndex);
  _localIndex += sizeof(boolean);
  weatheradjust.setAddress(_localIndex);
  _localIndex += sizeof(boolean);
  weatherP.setAddress(_localIndex);
  _localIndex += sizeof(byte);

  insideTemp.setAddress(_localIndex);
  _localIndex += sizeof(byte);
  outsideTemp.setAddress(_localIndex);
  _localIndex += sizeof(byte);
  luxMeter.setAddress(_localIndex);
  _localIndex += sizeof(byte);
  rainSensor.setAddress(_localIndex);
  _localIndex += sizeof(byte);
  anemometer.setAddress(_localIndex);
  _localIndex += sizeof(byte);


  lowTempAlarm.setAddress(_localIndex);
  _localIndex += sizeof(byte);
  highTempAlarm.setAddress(_localIndex);
  _localIndex += sizeof(byte);
  minTemp.setAddress(_localIndex);
  _localIndex += sizeof(float);
  maxTemp.setAddress(_localIndex);
  _localIndex += sizeof(float);

  timepoints.setValue(t);
  rollups.setValue(r);
  devices.setValue(f);
  timezone.setValue(timez);
  latitude.setValue(lat);
  longitude.setValue(longit);
  _ramping = false;
  _overrideProgramCounter = 0;
}

Greenhouse::Greenhouse(){
  _localIndex = GREENHOUSE_INDEX;

  timezone.setValue(-5);
  latitude.setValue(46);
  longitude.setValue(-73);
  timepoints.setValue(MAX_TIMEPOINTS);
  rollups.setValue(MAX_ROLLUPS);
  devices.setValue(MAX_DEVICES);

  timezone.setAddress(_localIndex);
  _localIndex += sizeof(short);

  latitude.setAddress(_localIndex);
  _localIndex += sizeof(float);

  longitude.setAddress(_localIndex);
  _localIndex += sizeof(float);

  timepoints.setAddress(_localIndex);
  _localIndex += sizeof(byte);

  rollups.setAddress(_localIndex);
  _localIndex += sizeof(byte);

  devices.setAddress(_localIndex);
  _localIndex += sizeof(byte);

  daynight.setAddress(_localIndex);
  _localIndex += sizeof(boolean);
  weatheradjust.setAddress(_localIndex);
  _localIndex += sizeof(boolean);

  dif.setAddress(_localIndex);
  _localIndex += sizeof(boolean);
  prenight.setAddress(_localIndex);
  _localIndex += sizeof(boolean);
  weatherP.setAddress(_localIndex);
  _localIndex += sizeof(byte);
  insideTemp.setAddress(_localIndex);
  _localIndex += sizeof(byte);
  outsideTemp.setAddress(_localIndex);
  _localIndex += sizeof(byte);
  luxMeter.setAddress(_localIndex);
  _localIndex += sizeof(byte);
  rainSensor.setAddress(_localIndex);
  _localIndex += sizeof(byte);
  anemometer.setAddress(_localIndex);
  _localIndex += sizeof(byte);
  autoWeather.setAddress(_localIndex);
  _localIndex += sizeof(bool);
  lowTempAlarm.setAddress(_localIndex);
  _localIndex += sizeof(byte);
  highTempAlarm.setAddress(_localIndex);
  _localIndex += sizeof(byte);
  minTemp.setAddress(_localIndex);
  _localIndex += sizeof(float);
  maxTemp.setAddress(_localIndex);
  _localIndex += sizeof(float);
  autoWeather.setAddress(_localIndex);
  _localIndex += sizeof(bool);

  _ramping = false;
  _overrideProgramCounter = 0;
}

Greenhouse::~Greenhouse(){
}

void Greenhouse::initGreenhouse(short timez, float lat, float longit, byte t, byte r, byte f, bool dn, bool wa){

  timepoints.setValue(t);
  rollups.setValue(r);
  devices.setValue(f);
  timezone.setValue(timez);
  latitude.setValue(lat);
  longitude.setValue(longit);
  daynight.setValue(dn);
  dif.setValue(false);
  prenight.setValue(false);
  weatheradjust.setValue(wa);
  weatherP.setValue(SUN);
  insideTemp.setValue(STH1X_TEMP);
  outsideTemp.setValue(OFF_TEMP);
  luxMeter.setValue(false);
  anemometer.setValue(false);
  rainSensor.setValue(OFF_RAIN);
  lowTempAlarm.setValue(false);
  highTempAlarm.setValue(false);
  minTemp.setValue(-10);
  maxTemp.setValue(40);
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
        Serial.println(F("Heure avancée"));
    }
    else{
        Serial.println(F("Heure normale"));
    }
  #endif
}

boolean Greenhouse::otherRollupsAreMoving(byte exception){
  for (byte x = 0; x < rollups.value(); x++){
    if(x != exception){
      if (rollup[x].isMoving() && rollup[x].isActivated()){
        return true;
      }
    }
  }
  return false;
}

void Greenhouse::fullRoutine(byte rightNow[6], float greenhouseTemperature){
  setNow(rightNow);
  solarCalculations();
  checkProgramSuccession();
  selectActualProgram();
  startRamping();

    #if MAX_ROLLUPS >= 1
    for (byte x = 0; x < rollups.value(); x++){
      if((!otherRollupsAreMoving(x))){
          rollup[x].routine(_coolingTemp, greenhouseTemperature);
      }
    }
    #endif

    #if MAX_DEVICES >= 1
    for (byte x = 0; x < devices.value(); x++){
      device[x].onTime.record(hr(), mn(), device[x].isOn());
      if((device[x].type.value() == FANTYPE)){
        device[x].routine(_coolingTemp, greenhouseTemperature);
      }
      else if((device[x].type.value() == HEATERTYPE)){
        device[x].routine(_heatingTemp, greenhouseTemperature);
      }
      else if((device[x].type.value() == VALVTYPE)){
        device[x].valvRoutine();
      }
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
    Serial.print(F("lever du soleil :"));Serial.print(Timepoint::sunRise[HOUR]);  Serial.print(F(":"));  Serial.println(Timepoint::sunRise[MINUT]);
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
    Serial.print(F("coucher du soleil :"));  Serial.print(Timepoint::sunSet[HOUR]);  Serial.print(F(":"));  Serial.println(Timepoint::sunSet[MINUT]);
    Serial.println(F("----"));
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
  timeParameter  startTime(timepoint[_timepoint-1].hr(), timepoint[_timepoint-1].mn());
  timeParameter  firstStep(timepoint[_timepoint-1].hr(), timepoint[_timepoint-1].mn());
  timeParameter  secondStep(timepoint[_timepoint-1].hr(), timepoint[_timepoint-1].mn());
  timeParameter  thirdStep(timepoint[_timepoint-1].hr(), timepoint[_timepoint-1].mn());

  firstStep.addTime(0, (unsigned long)timepoint[_timepoint-1].ramping.value()/3*1);
  secondStep.addTime(0, (unsigned long)timepoint[_timepoint-1].ramping.value()/3*2);
  thirdStep.addTime(0, (unsigned long)timepoint[_timepoint-1].ramping.value()/3*3);


    if(daynight.value() == false){
      _lastTimepoint = 2;
      _timepoint = 2;
    }
    else{
    #ifdef DEBUG_PROGRAM
      Serial.println(F("----"));
      Serial.print (F("Heure actuelle "));Serial.print(F(" : "));Serial.print(_rightNow[HOUR] );Serial.print(F(" : "));Serial.println(_rightNow[MINUT]);
    #endif
    for (byte y = 0; y < (timepoints.value()-1); y++){
      timeParameter  startTime(timepoint[y].hr(), timepoint[y].mn());
      timeParameter  stopTime(timepoint[y+1].hr(), timepoint[y+1].mn());

    #ifdef DEBUG_PROGRAM
      Serial.print (F("Programme ")); Serial.print(y+1);Serial.print(F(" : "));Serial.print(timepoint[y].hr());Serial.print(F(" : "));Serial.println(timepoint[y].mn());
    #endif

      if (isBetween(startTime.hour(), startTime.minut(), _rightNow[HOUR], _rightNow[MINUT], stopTime.hour(), stopTime.minut())){
        _timepoint = y+1;
      }
    }

    #ifdef DEBUG_PROGRAM
      Serial.print (F("Programme "));Serial.print(timepoints.value());Serial.print(F(" : "));Serial.print(timepoint[timepoints.value()-1].hr());Serial.print(F(" : "));Serial.println(timepoint[timepoints.value()-1].mn());
    #endif

    if (isBetween(timepoint[timepoints.value()-1].hr(), timepoint[timepoints.value()-1].mn(), _rightNow[HOUR], _rightNow[MINUT], timepoint[0].hr(), timepoint[0].mn())){
      _timepoint = timepoints.value();
    }
    #ifdef DEBUG_PROGRAM
      Serial.print (F("Program is : "));
      Serial.println(_timepoint);
      Serial.println(F("----"));
    #endif
    }
}

void Greenhouse::setTempCible(){
  if((weatherP.value() == SUN)||(weatheradjust.value() == false)){
    _coolingTemp = timepoint[_timepoint-1].coolingTemp.value();
    _heatingTemp = timepoint[_timepoint-1].heatingTemp.value();
  }
  else if(weatherP.value() == CLOUD){
      _coolingTemp = timepoint[_timepoint-1].coolingTempCloud.value();
      _heatingTemp = timepoint[_timepoint-1].heatingTempCloud.value();
  }
  else if((weatherP.value() > 1)||(weatherP.value() <= 100)){
    _coolingTemp = timepoint[_timepoint-1].coolingTempCloud.value() + ((((float)weatherP.value())/100)*(timepoint[_timepoint-1].coolingTemp.value() - timepoint[_timepoint-1].coolingTempCloud.value()));
    _heatingTemp = timepoint[_timepoint-1].heatingTempCloud.value() + ((((float)weatherP.value())/100)*(timepoint[_timepoint-1].heatingTemp.value() - timepoint[_timepoint-1].heatingTempCloud.value()));
  }
}


void Greenhouse::updateTempTargets(){
  float lastCoolingTemp;
  float lastHeatingTemp;

  if((weatherP.value() == SUN)||(weatheradjust.value() == false)){
    _newCoolingTemp = timepoint[_timepoint-1].coolingTemp.value();
    _newHeatingTemp = timepoint[_timepoint-1].heatingTemp.value();
    lastCoolingTemp = timepoint[_lastTimepoint-1].coolingTemp.value();
    lastHeatingTemp = timepoint[_lastTimepoint-1].heatingTemp.value();

  }

  else if(weatherP.value() == CLOUD){
      _newCoolingTemp = timepoint[_timepoint-1].coolingTempCloud.value();
      _newHeatingTemp = timepoint[_timepoint-1].heatingTempCloud.value();
      lastCoolingTemp = timepoint[_lastTimepoint-1].coolingTempCloud.value();
      lastHeatingTemp = timepoint[_lastTimepoint-1].heatingTempCloud.value();
  }
  else if((weatherP.value() > 1)||(weatherP.value() <= 100)){
    _newCoolingTemp = timepoint[_timepoint-1].coolingTempCloud.value() + ((((float)weatherP.value())/100)*(timepoint[_timepoint-1].coolingTemp.value() - timepoint[_timepoint-1].coolingTempCloud.value()));
    _newHeatingTemp = timepoint[_timepoint-1].heatingTempCloud.value() + ((((float)weatherP.value())/100)*(timepoint[_timepoint-1].heatingTemp.value() - timepoint[_timepoint-1].heatingTempCloud.value()));
    lastCoolingTemp = timepoint[_lastTimepoint-1].coolingTempCloud.value() + ((((float)weatherP.value())/100)*(timepoint[_lastTimepoint-1].coolingTemp.value() - timepoint[_lastTimepoint-1].coolingTempCloud.value()));
    lastHeatingTemp = timepoint[_lastTimepoint-1].heatingTempCloud.value() + ((((float)weatherP.value())/100)*(timepoint[_lastTimepoint-1].heatingTemp.value() - timepoint[_lastTimepoint-1].heatingTempCloud.value()));
  }

  _coolingTempStep = (_newCoolingTemp - lastCoolingTemp)/3;
  _heatingTempStep = (_newHeatingTemp - lastHeatingTemp)/3;
}


void Greenhouse::startRamping(){



  switch(_timepoint){
    case 1: _lastTimepoint = 4;break;
    case 2: if(dif.value() == true){_lastTimepoint = 1;}
            else{_lastTimepoint = 4;} break;
    case 3: _lastTimepoint = 2;break;
    case 4: if(prenight.value() == true){_lastTimepoint = 3;}
            else{_lastTimepoint = 2;} break;break;
  }

  updateTempTargets();

  timeParameter  startTime(timepoint[_timepoint-1].hr(), timepoint[_timepoint-1].mn());
  timeParameter  firstStep(timepoint[_timepoint-1].hr(), timepoint[_timepoint-1].mn());
  timeParameter  secondStep(timepoint[_timepoint-1].hr(), timepoint[_timepoint-1].mn());
  timeParameter  thirdStep(timepoint[_timepoint-1].hr(), timepoint[_timepoint-1].mn());

  firstStep.addTime(0, (unsigned long)timepoint[_timepoint-1].ramping.value()/3*1);
  secondStep.addTime(0, (unsigned long)timepoint[_timepoint-1].ramping.value()/3*2);
  thirdStep.addTime(0, (unsigned long)timepoint[_timepoint-1].ramping.value()/3*3);

  #ifdef DEBUG_RAMPING
    Serial.println(F("----");
    Serial.print (F("Now "));SerialM.print(F(" : "));Serial.print(_rightNow[HOUR] );Serial.print(F(" : "));Serial.println(_rightNow[MINUT]);
    Serial.print (F("Heat temp"));Serial.print(F(" : "));Serial.println(_heatingTemp);

    if(_ramping == true){
      Serial.print (F("New heat temp"));Serial.print(" : ");Serial.println(_newHeatingTemp);
      Serial.print (F("temp step"));Serial.print(" : ");Serial.println(_heatingTempStep);

      Serial.print (F("step1")); Serial.print(" : ");Serial.print(firstStep.hour());Serial.print(" : ");Serial.println(firstStep.minut());
      Serial.print (F("step2")); Serial.print(" : ");Serial.print(secondStep.hour());Serial.print(" : ");Serial.println(secondStep.minut());
      Serial.print (F("step3")); Serial.print(" : ");Serial.print(thirdStep.hour());Serial.print(" : ");Serial.println(thirdStep.minut());
    }
  #endif

  if(isBetween(timepoint[_timepoint-1].hr(), timepoint[_timepoint-1].mn(), hr(), mn(), firstStep.hour(), firstStep.minut())){
    //keep cooling/heatingTemp as it is
    _ramping = true;
    #ifdef DEBUG_RAMPING

    Serial.println(F("step1 check"));
    #endif

  }
  else if(isBetween(timepoint[_timepoint-1].hr(), timepoint[_timepoint-1].mn(), hr(), mn(), secondStep.hour(), secondStep.minut())){
    _coolingTemp = _newCoolingTemp - _coolingTempStep*2;
    _heatingTemp = _newHeatingTemp - _heatingTempStep*2;
    _ramping = true;
    #ifdef DEBUG_RAMPING

    Serial.println(F("step2 check"));
    #endif

  }
  else if(isBetween(timepoint[_timepoint-1].hr(), timepoint[_timepoint-1].mn(), hr(), mn(), thirdStep.hour(), thirdStep.minut())){
    _coolingTemp = _newCoolingTemp - _coolingTempStep*1;
    _heatingTemp = _newHeatingTemp - _heatingTempStep*1;
    _ramping = true;
    #ifdef DEBUG_RAMPING

    Serial.println(F("step3 check"));
    #endif

  }
  else{
    _coolingTemp = _newCoolingTemp;
    _heatingTemp = _newHeatingTemp;
    _ramping = false;
    #ifdef DEBUG_RAMPING

    Serial.println(F("stop ramping"));
    #endif

  }


/*
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
      _heatingTemp += 1;
      if(_heatingTemp > _newHeatingTemp){
        _heatingTemp = _newHeatingTemp;
      }
    }
    else if (_newHeatingTemp < _heatingTemp){
      _heatingTemp -= 1;
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
  }*/
}

void Greenhouse::setWeather(byte weather){
  weatherP.setValue(weather);
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
  return weatherP.value();
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

//display instantaneous moves for better testing
void Greenhouse::testRollups(boolean state){
  for (byte x = 0; x < rollups.value(); x++){
    rollup[x].setTest(state);
    //if(state == true){
      //rollup[x].unlock();
    //}
  }
}

void Greenhouse::EEPROMGet(){

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
    Serial.print(devices.address());
    Serial.print(F(" - Value :"));
    Serial.print(devices.value());
    Serial.println(F("   - (Devices)"));
/*
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
  */
  #endif
}
