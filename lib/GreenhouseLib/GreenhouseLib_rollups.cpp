/*
  GreenhouseLib_rollups.cpp
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
#include "GreenhouseLib_rollups.h"

unsigned short Stage::_counter = 0;

Stage::Stage(){
  _localCounter = _counter;
  _counter++;
  if(_counter == MAX_STAGES){
    _counter = 0;
  }
}
Stage::~Stage(){}

unsigned short Stage::nb(){
  return _localCounter;
}
//****************************************************************
//*******************ROLLUP FUNCTIONS*****************************
//****************************************************************



/*
Constructor : Define Opening and Closing pins on the arduino, to connect to the relay box
_routine (normal logic for the rollup applies if true)
_closing (closing pin is active if true)
_opening (same with opening pin)
_closingCycle (a closing cycle is going on, including pause time)
_closingCycle (same with closing)
_incrementCounter (what increment the rollup has reached)
incrementCounter is set to OFF_VAL so the program recognize the first opening/closing run.
_stage (what cooling stage the rollup has reached)
_localIndex (starting point for EEPROM savings)
_index (starting point for next object of the same class)
EEPROMTimer (checks every 10 seconds if EEPROM has to be updated)

*/

unsigned short Rollup::_EEPROMindex = 0;
unsigned short Rollup::_counter = 0;

Rollup::Rollup(){
  _localCounter = _counter;
  _counter++;

  _activeOverride = OFF_VAL;
  _routineCycle = false;

//Addresses assignation

  _localIndex = ROLLUP_INDEX + _EEPROMindex;
  _EEPROMindex += ROLLUP_INDEX_SIZE;

  hyst.setLimits(0, 5);
  hyst.setAddress(_localIndex);
  _localIndex += sizeof(float);

  rotationUp.setLimits(0,500);
  rotationUp.setAddress(_localIndex);
  _localIndex += sizeof(unsigned short);

  rotationDown.setLimits(0,500);
  rotationDown.setAddress(_localIndex);
  _localIndex += sizeof(unsigned short);

  pause.setLimits(0,240);
  pause.setAddress(_localIndex);
  _localIndex += sizeof(unsigned short);
  _increments = 100;
  _test = false;

  for(int x = 0; x < MAX_STAGES; x++){
    stage[x].mod.setLimits(0,10);
    stage[x].mod.setAddress(_localIndex);
    _localIndex += sizeof(float);
    stage[x].target.setLimits(0,100);
    stage[x].target.setAddress(_localIndex);
    _localIndex += sizeof(unsigned short);
  }

  stages.setLimits(0, MAX_STAGES);
  stages.setAddress(_localIndex);
  _localIndex += sizeof(byte);
  enabled.setAddress(_localIndex);
  _localIndex += sizeof(boolean);

  #ifdef TEST_MODE
  _incrementCounter = 0;
  _stage = 0;
  #endif
  #ifndef TEST_MODE
  _incrementCounter = OFF_VAL;
  _stage = OFF_VAL;
  _reset = true;
  #endif
  initOverride(LOCK, 0,0);
  debugTimer = 0;
  rollupTimer = 0;
}

/*
Destructor
*/
Rollup::~Rollup(){}
/*
INIT OUTPUTS
- MAN_TEMP : target temperature is set outisde the class and differ for each cooling stage
relayType:
- ACT_HIGH : relay is active when pin is high
- ACT_LOW : relay is active when pin is low
rOpen (pin connected to opening relay)
rClose (pin connected to closing relay)
*/
void Rollup::initOutputs(byte outputType, boolean relayType, byte rOpen, byte rClose){
  //define opening/closing pins
  _openingPin.init(outputType, relayType, rOpen);
  _closingPin.init(outputType, relayType, rClose);
}

void Rollup::lockOpenAndWait(unsigned long seconds){
  if(seconds != 0){
    resetLockTimer(seconds);
  }
  lockOpen();
}

void Rollup::lockCloseAndWait(unsigned long seconds){
  if(seconds != 0){
    resetLockTimer(seconds);
  }
  lockClose();
}
void Rollup::lockOpen(){
  initOverride(LOCK, 0, 100);
  checkOverride(LOCK, true);
}
void Rollup::lockClose(){
  initOverride(LOCK, 0, 0);
  checkOverride(LOCK, true);
}
void Rollup::lockAtIncrement(byte increment){
  if(increment <= _increments){
    initOverride(LOCK, 0, increment);
    checkOverride(LOCK, true);
  }
}


void Rollup::lockAtIncrement(byte increment, unsigned long seconds){
  if(seconds != 0){
    resetLockTimer(seconds);
  }
  lockAtIncrement(increment);
}

/*
Open or close the rollups to specific increment, using a multiple cooling stages logic
*/
void Rollup::forceStop(){
  stopMove();
  _incrementCounter = OFF_VAL;
  _stage = OFF_VAL;
  _reset = true;
}

void Rollup::routine(float targetTemp, float temp){
  if(!isActivated()){
    forceStop();
  }
  if (isActivated() || isMoving()){
    if(isActivated() && _reset == true){
      setIncrementCounter(increments());
      lockCloseAndWait(rotationDown.value());
      _reset = false;
    }

    autoAdjustStages();
    checkOverrideTimer();
    byte checkOverride = activeOverride();
    if((_activeOverride == OFF_VAL)&&(checkOverride != OFF_VAL)&&(!isMoving())){
        updatePosition(overrideTarget());
        startMove();
    }
    else if(_activeOverride != OFF_VAL){
      watchOverride();
    }
    else{
      if(_routineCycle == false){
          updatePosition(temp, targetTemp);
          startMove();
      }
      else{
        watchRoutine();
      }
    }
  }
  debugPrints();
}




/*ROUTINE MOVING LOGIC
    INITIAL CONDITIONS:
    -increment counter is calibrated
    - theres a difference between actual increment and target increment
    ACTION:
    - If temp is higher than treshold : go to next stage up
    - If temp is lower than treshold : go to next stage down
    - calculate time of the move
    - start moving
    - set moving timer to 0
*/
void Rollup::updatePosition(float temp, float targetTemp){
  short targetIncrement;

  if (temp >= (targetTemp + stage[_upperStage].mod.value())){
      targetIncrement = stage[_upperStage].target.value();
      _incrementMove = targetIncrement - (short)_incrementCounter;
  }
  else if(temp < (targetTemp + stage[_stage].mod.value() - hyst.value())){
      targetIncrement = stage[_lowerStage].target.value();
      _incrementMove = targetIncrement - (short)_incrementCounter;
  }
  else{
    _incrementMove = 0;
  }

  if(_incrementMove != 0){
    _routineCycle = true;
  }
}
void Rollup::updatePosition(byte target){
  if(_routineCycle == true){
    _routineCycle = false;
  }
  _activeOverride = activeOverride();
  _incrementMove = target - (short)_incrementCounter;
}

void Rollup::startMove(){
  calibrateStages();
  updateTimings();
    //If motor goes up, calculate for how much time
  if (_incrementMove > 0){
    _moveTime = _upStep * (unsigned long)(abs(_incrementMove));
    _openingPin.start();
  }
    //If motor goes down, calculate for how much time
  else if (_incrementMove < 0){
    _moveTime = _downStep * (unsigned long)(abs(_incrementMove));
    _closingPin.start();
  }
  rollupTimer = 0;
}


  /*ROUTINE WATCH
  MOVE : until motor reaches target increment
  PAUSE : until end of pause time
  */
  void Rollup::watchRoutine(){
    if((rollupTimer > _moveTime)){
      stopMove();
    }
    if(rollupTimer > (_moveTime + _pauseTime)){
      resumeCycle("ROUTINE");
    }
  }


/*OVERRIDE WATCH
MOVE : until motor reaches target increment
PAUSE :for all override's duration
*/
void Rollup::watchOverride(){
  if(rollupTimer > _moveTime){
      stopMove();
  }
  byte checkOverride = activeOverride();
  if((rollupTimer > _moveTime)&&(checkOverride != OFF_VAL)&&(checkOverride != _activeOverride)){
      updatePosition(checkOverride);
      startMove();
  }
  if((rollupTimer > _moveTime)&&(checkOverride == OFF_VAL)){
      resumeCycle("OVERRIDE");
  }
}
/*STOP MOVING
    INITIAL CONDITION:
    -increment counter is calibrated
    ACTION:
    - stop moving
    - set new value for incrementCounter
    - reset _incrementMove
*/
void Rollup::stopMove(){
    if(_incrementCounter == OFF_VAL){
      return;
    }
    if(_openingPin.isActive()){
      _openingPin.stop();
    }
    if(_closingPin.isActive()){
      _closingPin.stop();
    }
    _incrementCounter = _incrementCounter + _incrementMove;
    _incrementMove = 0;
    calibrateStages();

}
/*RESUME CYCLE
    INITIAL CONDITION:
    -override is active
    OR
    - routine cycle is active
    ACTION:

    ACTION:
    - desactivate override and activate routine
    OR
    - desactivate routine cycle
    - reset times, moving time and stage move
*/
void Rollup::resumeCycle(String type){
  if(type == "OVERRIDE"){
      _activeOverride = OFF_VAL;
  }
  else if(type == "ROUTINE"){
      _routineCycle = false;
      printEndPause();
  }
  _moveTime = 0;
  rollupTimer = 0;
}

/*CALIBRATE STAGE
    INITIAL CONDITION:
    - stage is uncalibrated
    ACTION:
    - if incrementCounter is uncalibrateddesactivate routine cycle
    - reset times, moving time and stage move
*/
void Rollup::calibrateStages(){
        //If youre at top increment, consider youre at top stage
        if(_incrementCounter >= stage[stages.value()].target.value()){
          _stage = stages.value();
        }
        //If youre at bottom increment, consider youre at lowest stage
        else if(_incrementCounter == stage[0].target.value()){
          _stage = 0;
        }
        //Anywhere between first and second stage increment, consider youre in the second stage
        else if((_incrementCounter > stage[0].target.value())&&(_incrementCounter <= stage[1].target.value())){
          _stage = 1;
        }
        //Between second stage and third, consider youre at second stage;
        //Between third stage and fourth, considier youre at third stage;
        //etc.
        else{
          for(byte x = 1; x < stages.value();x++){
            if((_incrementCounter >= stage[x].target.value())&& (_incrementCounter < stage[x+1].target.value())){
              _stage = x;
            }
          }
        }
        checkStageSuccession();
  }

  void Rollup::setTest(boolean state){
    _test = state;
  }

  void Rollup::updateTimings(){
    float upStep = (float)rotationUp.value()*1000/(float)_increments;
    float downStep = (float)rotationDown.value()*1000/(float)_increments;
    _upStep = upStep;
    _downStep = downStep;
    _pauseTime = pause.value()*1000;

      if(_test == true){
        _upStep = 0;
        _downStep = 0;
        _pauseTime = 0;
      }
  }

void Rollup::autoAdjustStages(){
  for(byte x = 1; x <= stages.value();x++){
    if(stage[x].mod.value() < stage[x-1].mod.value()){
      stage[x].mod.setValue(stage[x-1].mod.value());
    }
    if(stage[x].target.value() < stage[x-1].target.value()){
      stage[x].target.setValue(stage[x-1].target.value());
    }

    if(stage[x].mod.value() > stage[x-1].mod.maximum()){
      stage[x].mod.setValue(stage[x-1].mod.maximum());
    }
    if(stage[x].target.value() > stage[x-1].target.maximum()){
      stage[x].target.setValue(stage[x-1].target.maximum());
    }
  }
}

  void Rollup::checkStageSuccession(){
    if(_stage != stages.value()){
      if(stage[_stage].target.value() != stage[_stage+1].target.value()){
        _upperStage = _stage+1;
      }
      else if(stage[_stage].target.value() != stage[_stage+2].target.value()){
        _upperStage = _stage+2;
      }
      else if(stage[_stage].target.value() != stage[_stage+3].target.value()){
        _upperStage = _stage+3;
      }
      else if(stage[_stage].target.value() != stage[_stage+4].target.value()){
        _upperStage = _stage+4;
      }
      else{
        _upperStage = stages.value();
      }
    }
    else{
      _upperStage = stages.value();
    }
    if(_stage != 0){
      if(stage[_stage].target.value() != stage[_stage-1].target.value()){
        _lowerStage = _stage-1;
      }
      else if(stage[_stage].target.value() != stage[_stage-2].target.value()){
        _lowerStage = _stage-2;
      }
      else if(stage[_stage].target.value() != stage[_stage-3].target.value()){
        _lowerStage = _stage-3;
      }
      else if(stage[_stage].target.value() != stage[_stage-4].target.value()){
        _lowerStage = _stage-4;
      }
      else{
        _lowerStage = 0;
      }
    }
    else{
      _lowerStage = 0;
    }
  }
  void Rollup::desactivateDevice(){
    enabled.setValue(false);
    _openingPin.stop();
    _closingPin.stop();
  }
  void Rollup::activateDevice(){
    enabled.setValue(true);
  }
  bool Rollup::isActivated(){
    return enabled.value();
  }
void Rollup::printPause(){
  #ifdef DEBUG_ROLLUP1_STATE
  if(_localCounter == 0){
    Serial.println(F("-------------"));
    Serial.println(F("Rollup1 : start pause"));
  }
  #endif
  #ifdef DEBUG_ROLLUP2_STATE
  if(_localCounter == 1){
    Serial.println(F("-------------"));
    Serial.println(F("Rollup2 : start pause"));
  }
  #endif
}

void Rollup::printEndPause(){

  #ifdef DEBUG_ROLLUP1_STATE
  if(_localCounter == 0){
    Serial.println(F("-------------"));
    Serial.println(F("Rollup1 : stop pause"));
  }
  #endif
  #ifdef DEBUG_ROLLUP2_STATE
  if(_localCounter == 1){
    Serial.println(F("-------------"));
    Serial.println(F("Rollup2 : stop pause"));
  }
  #endif
}
/*
Program all parameters all at once...
*/
void Rollup::setParameters(byte st, float rHyst, unsigned short rotUp, unsigned short rotDown, unsigned short paus, bool enab){
  stages.setValue(st);
  hyst.setValue(rHyst);
  rotationUp.setValue(rotUp);
  rotationDown.setValue(rotDown);
  pause.setValue(paus);
  enabled.setValue(enab);
}


void Rollup::setIncrementCounter(unsigned short increment){
  _incrementCounter = increment;
}

void Rollup::EEPROMGet(){
/*
  hyst.getInEEPROM();
  rotationUp.getInEEPROM();
  rotationDown.getInEEPROM();
  pause.getInEEPROM();
*/
  #ifdef DEBUG_EEPROM
    Serial.println(F("-------------------"));
    Serial.print(F("-------ROLLUP "));
    Serial.print(_localCounter);
    Serial.println(F("------"));
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
    Serial.print(rotationUp.address());
    Serial.print(F(" - Value :"));
    Serial.print(rotationUp.value());
    Serial.println(F("   - (Rotation up)"));
    Serial.print(F("Address: "));
    Serial.print(rotationDown.address());
    Serial.print(F(" - Value :"));
    Serial.print(rotationDown.value());
    Serial.println(F("   - (Rotation down)"));
    Serial.print(F("Address: "));
    Serial.print(pause.address());
    Serial.print(F(" - Value :"));
    Serial.print(pause.value());
    Serial.println(F("   - (Pause)"));
  #endif

  for(unsigned short x = 0; x < stages.value()+1; x++){/*
    stage[x].mod.getInEEPROM();
    stage[x].target.getInEEPROM();*/

    #ifdef DEBUG_EEPROM
      Serial.print(F("-------STAGE  "));
      Serial.print(stage[x].nb());
      Serial.println(F("------"));
      Serial.print(F("Address: "));
      Serial.print(stage[x].mod.address());
      Serial.print(F(" - Value :"));
      Serial.print(stage[x].mod.value());
      Serial.print(F(" - (Stage mod "));
      Serial.print(x);
      Serial.println(F(")"));
      Serial.print(F("Address: "));
      Serial.print(stage[x].target.address());
      Serial.print(F(" - Value :"));
      Serial.print(stage[x].target.value());
      Serial.print(F("   - (Stage target "));
      Serial.print(x);
      Serial.println(F(")"));
    #endif
  }
}
//Return private variables

unsigned short Rollup::increments(){
  return _increments;
}

unsigned short Rollup::incrementCounter(){
  return _incrementCounter;
}
boolean Rollup::opening(){
  return _openingPin.isActive();
}
boolean Rollup::closing(){
  return _closingPin.isActive();
}
boolean Rollup::override(){
  if(_activeOverride != OFF_VAL){
    return true;
  }
  else{
    return false;
  }
}
boolean Rollup::isMoving(){
  if ((_openingPin.isActive())||(_closingPin.isActive())){
    return true;
  }
  else{
    return false;
  }
}
boolean Rollup::isWaiting(){
    if (((!_openingPin.isActive())||(!_closingPin.isActive()))&&(_routineCycle == true)){
      return true;
    }
    else{
      return false;
    }
}

unsigned short Rollup::nb(){
  return _localCounter;
}

unsigned short Rollup::EEPROMIndexBegin(){
  return ROLLUP_INDEX + (ROLLUP_INDEX_SIZE*_localCounter);
}

unsigned short Rollup::EEPROMIndexEnd(){
  return _localIndex;
}

boolean Rollup::TEST_parameterOffLimits(){
  for(int x = 0; x < MAX_STAGES;x++){
    if(stage[x].target.isOffLimit()||stage[x].mod.isOffLimit()){
      return true;
    }
  }
  if(hyst.isOffLimit()||rotationUp.isOffLimit()||rotationDown.isOffLimit()||pause.isOffLimit()){
    return true;
  }
  else{
    return false;
  }
}

void Rollup::debugPrints(){
  if(debugTimer > 1000){
    if(_localCounter == 0 ){
      #ifdef DEBUG_ROLLUP1_CYCLE
        Serial.println(F("---------"));
        if(_activeOverride != OFF_VAL){
          Serial.println(F("OVERRIDE : ON"));
        }
        if(_routineCycle == true){
          Serial.println(F("ROUTINE CYCLE : ON"));
        }
      #endif
      #ifdef DEBUG_ROLLUP1_POSITION
          Serial.println(F("---------"));
          Serial.print(F("up stage : "));
          Serial.println(_upperStage);
          Serial.print(F("up target : "));
          Serial.println(stage[_upperStage].target.value());
          Serial.print(F("Stage : "));
          Serial.println(_stage);
          Serial.print(F("low stage : "));
          Serial.println(_lowerStage);
          Serial.print(F("low target : "));
          Serial.println(stage[_lowerStage].target.value());
      #endif
      #ifdef DEBUG_ROLLUP1_TIMING
        Serial.println(F("---------"));
        if(_routineCycle == true){
          Serial.println(F("ROUTINE1"));
          Serial.print(F("rollupTimer : "));
          Serial.println(rollupTimer);
          Serial.print(F("moveTime : "));
          Serial.println(_moveTime);
          Serial.print(F("pauseTime : "));
          Serial.println(_moveTime + _pauseTime);
        }
        else if(_activeOverride != OFF_VAL){
          Serial.print(F("OVERRIDE"));
          Serial.println(_activeOverride);
          Serial.print(F("rollupTimer : "));
          Serial.println(rollupTimer);
          Serial.print(F("moveTime : "));
          Serial.println(_moveTime);
        }
      #endif
    }
  if(_localCounter == 1 ){
    #ifdef DEBUG_ROLLUP2_CYCLE
      Serial.println(F("---------"));
      if(_activeOverride != OFF_VAL){
        Serial.println(F("OVERRIDE : ON"));
      }
      if(_routineCycle == true){
        Serial.println(F("ROUTINE CYCLE : ON"));
      }
    #endif

    #ifdef DEBUG_ROLLUP2_POSITION
      Serial.println(F("---------"));
      Serial.print(F("up stage : "));
      Serial.println(_upperStage);
      Serial.print(F("up target : "));
      Serial.println(stage[_upperStage].target.value());
      Serial.print(F("Stage : "));
      Serial.println(_stage);
      Serial.print(F("low stage : "));
      Serial.println(_lowerStage);
      Serial.print(F("low target : "));
      Serial.println(stage[_lowerStage].target.value());
    #endif
    #ifdef DEBUG_ROLLUP2_TIMING
      Serial.println(F("---------"));
      if(_routineCycle == true){
        Serial.println(F("ROUTINE1"));
        Serial.print(F("rollupTimer : "));
        Serial.println(rollupTimer);
        Serial.print(F("moveTime : "));
        Serial.println(_moveTime);
        Serial.print(F("pauseTime : "));
        Serial.println(_moveTime + _pauseTime);
      }
      else if(_activeOverride != OFF_VAL){
        Serial.println(F("FIX OVERRIDE1"));
        Serial.print(F("rollupTimer : "));
        Serial.println(rollupTimer);
        Serial.print(F("moveTime : "));
        Serial.println(_moveTime);
      }
    #endif
    }
    if(_localCounter == 2 ){
      #ifdef DEBUG_ROLLUP3_CYCLE
        Serial.println(F("---------"));
        if(_activeOverride != OFF_VAL){
          Serial.println(F("OVERRIDE : ON"));
        }
        if(_routineCycle == true){
          Serial.println(F("ROUTINE CYCLE : ON"));
        }
      #endif

      #ifdef DEBUG_ROLLUP3_POSITION
        Serial.println(F("---------"));
        Serial.print(F("up stage : "));
        Serial.println(_upperStage);
        Serial.print(F("up target : "));
        Serial.println(stage[_upperStage].target.value());
        Serial.print(F("Stage : "));
        Serial.println(_stage);
        Serial.print(F("low stage : "));
        Serial.println(_lowerStage);
        Serial.print(F("low target : "));
        Serial.println(stage[_lowerStage].target.value());
      #endif
      #ifdef DEBUG_ROLLUP3_TIMING
        Serial.println(F("---------"));
        if(_routineCycle == true){
          Serial.println(F("ROUTINE3"));
          Serial.print(F("rollupTimer : "));
          Serial.println(rollupTimer);
          Serial.print(F("moveTime : "));
          Serial.println(_moveTime);
          Serial.print(F("pauseTime : "));
          Serial.println(_moveTime + _pauseTime);
        }
        else if(_activeOverride != OFF_VAL){
          Serial.println(F("FIX OVERRIDE3"));
          Serial.print(F("rollupTimer : "));
          Serial.println(rollupTimer);
          Serial.print(F("moveTime : "));
          Serial.println(_moveTime);
        }
      #endif
      }
    debugTimer = 0;
  }
}
