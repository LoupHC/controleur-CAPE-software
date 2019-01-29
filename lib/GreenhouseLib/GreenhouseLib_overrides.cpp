#include "GreenhouseLib_overrides.h"



//Override manager

byte OverrideManager::_clockOvCount = 0;
byte OverrideManager::_floatOvCount = 0;
byte OverrideManager::_boolOvCount = 0;

bool OverrideManager::isActive(byte id){
  if((isEnabled(id)) && (isInitialized(id))){
    for(int x = 0; x < MAX_CLOCK_OV;x++){
      if(clockOv[x].ID.value() == id){
        return clockOv[x].isActive();
      }
    }
    for(int x = 0; x < MAX_FLOAT_OV;x++){
      if(floatOv[x].ID.value() == id){
        return floatOv[x].isActive();
      }
    }
    for(int x = 0; x < MAX_BOOL_OV;x++){
      if(boolOv[x].ID.value() == id){
        return boolOv[x].isActive();
      }
    }

  }
  return false;
}

bool OverrideManager::isEnabled(byte id){
  if(isInitialized(id)){
    for(int x = 0; x < MAX_CLOCK_OV;x++){
      if(clockOv[x].ID.value() == id){
        return clockOv[x].enabled.value();
      }
    }
    for(int x = 0; x < MAX_FLOAT_OV;x++){
      if(floatOv[x].ID.value() == id){
        return floatOv[x].enabled.value();
      }
    }
    for(int x = 0; x < MAX_BOOL_OV;x++){
      if(boolOv[x].ID.value() == id){
        return boolOv[x].enabled.value();
      }
    }
  }
  return false;
}

bool OverrideManager::isInitialized(byte id){
  for(int x = 0; x < MAX_CLOCK_OV;x++){
    if(clockOv[x].ID.value() == id){
      return true;
    }
  }
  for(int x = 0; x < MAX_FLOAT_OV;x++){
    if(floatOv[x].ID.value() == id){
      return true;
    }
  }
  for(int x = 0; x < MAX_BOOL_OV;x++){
    if(boolOv[x].ID.value() == id){
      return true;
    }
  }
  return false;
}


void OverrideManager::enable(byte id){
    for(int x = 0; x < MAX_CLOCK_OV;x++){
      if(clockOv[x].ID.value() == id){
        clockOv[x].enabled.setValue(true);
      }
    }
    for(int x = 0; x < MAX_FLOAT_OV;x++){
      if(floatOv[x].ID.value() == id){
        floatOv[x].enabled.setValue(true);
      }
    }
    for(int x = 0; x < MAX_BOOL_OV;x++){
      if(boolOv[x].ID.value() == id){
        boolOv[x].enabled.setValue(true);
      }
    }
}
void OverrideManager::disable(byte id){
    for(int x = 0; x < MAX_CLOCK_OV;x++){
      if(clockOv[x].ID.value() == id){
        clockOv[x].enabled.setValue(false);
        clockOv[x].desactivate();
      }
    }
    for(int x = 0; x < MAX_FLOAT_OV;x++){
      if(floatOv[x].ID.value() == id){
        floatOv[x].enabled.setValue(false);
        floatOv[x].desactivate();
      }
    }
    for(int x = 0; x < MAX_BOOL_OV;x++){
      if(boolOv[x].ID.value() == id){
        boolOv[x].enabled.setValue(false);
        boolOv[x].desactivate();
      }
    }
}

bool OverrideManager::idIsUnique(byte id){
    for(int x = 0; x < MAX_CLOCK_OV;x++){
      if(clockOv[x].ID.value() == id){
        return false;
      }
    }
    for(int x = 0; x < MAX_FLOAT_OV;x++){
      if(floatOv[x].ID.value() == id){
        return false;
      }
    }
    for(int x = 0; x < MAX_BOOL_OV;x++){
      if(boolOv[x].ID.value() == id){
        return false;
      }
    }
    return true;
}

byte OverrideManager::indexPosition(byte id){
  for(int x = 0; x < MAX_CLOCK_OV;x++){
    if(clockOv[x].ID.value() == id){
      return x;
    }
  }
  for(int x = 0; x < MAX_FLOAT_OV;x++){
    if(floatOv[x].ID.value() == id){
      return x;
    }
  }
  for(int x = 0; x < MAX_BOOL_OV;x++){
    if(boolOv[x].ID.value() == id){
      return x;
    }
  }
  return OFF_VAL;
}

void OverrideManager::clearOverride(byte id){
  for(int x = 0; x < MAX_CLOCK_OV;x++){
    if(clockOv[x].ID.value() == id){
      clockOv[x].ID.setValue(OFF_VAL);
      clockOv[x].enabled.setValue(false);
    }
  }
  for(int x = 0; x < MAX_FLOAT_OV;x++){
    if(floatOv[x].ID.value() == id){
      floatOv[x].ID.setValue(OFF_VAL);
      floatOv[x].enabled.setValue(false);
    }
  }
  for(int x = 0; x < MAX_BOOL_OV;x++){
    if(boolOv[x].ID.value() == id){
      boolOv[x].ID.setValue(OFF_VAL);
      boolOv[x].enabled.setValue(false);
    }
  }
}

void OverrideManager::clearOverrides(){
  for(int x = 0; x < MAX_CLOCK_OV;x++){
    clockOv[x].ID.setValue(OFF_VAL);
    clockOv[x].enabled.setValue(false);
  }
  for(int x = 0; x < MAX_FLOAT_OV;x++){
    floatOv[x].ID.setValue(OFF_VAL);
    floatOv[x].enabled.setValue(false);
  }
  for(int x = 0; x < MAX_BOOL_OV;x++){
    boolOv[x].ID.setValue(OFF_VAL);
    boolOv[x].enabled.setValue(false);
  }
}


void OverrideManager::initOverride(byte id, byte prior, byte hrStar, byte mnStar, byte hrSto, byte mnSto, byte state){
  if(idIsUnique(id)){
    if(_clockOvCount < MAX_CLOCK_OV){
      clockOv[_clockOvCount].ID.setValue(id);
      clockOv[_clockOvCount].enabled.setValue(true);
      clockOv[_clockOvCount].priority.setValue(prior);
      clockOv[_clockOvCount].hrStart.setValue(hrStar);
      clockOv[_clockOvCount].mnStart.setValue(mnStar);
      clockOv[_clockOvCount].hrStop.setValue(hrSto);
      clockOv[_clockOvCount].mnStop.setValue(mnSto);
      clockOv[_clockOvCount].target.setValue(state);
      _clockOvCount +=1;
    }
    else{
      Serial.println("Clock override overflow - make some space!");
    }
  }
  else{
    clockOv[indexPosition(id)].enabled.setValue(true);
    clockOv[indexPosition(id)].priority.setValue(prior);
    clockOv[indexPosition(id)].hrStart.setValue(hrStar);
    clockOv[indexPosition(id)].mnStart.setValue(mnStar);
    clockOv[indexPosition(id)].hrStop.setValue(hrSto);
    clockOv[indexPosition(id)].mnStop.setValue(mnSto);
    clockOv[indexPosition(id)].target.setValue(state);
  }
}
void OverrideManager::checkOverride(byte id,  Rollup &rollup, byte hr, byte mn){
  for(int x = 0; x < MAX_CLOCK_OV;x++){
    if(clockOv[x].ID.value() == id){
      if(clockOv[x].enabled.value()){
        clockOv[x].checkIfActive(hr, mn);
      }
      else{
        clockOv[x].desactivate();
      }
      if(clockOv[x].isActive()){
        rollup.setOverride(clockOv[x].priority.value(), clockOv[x].target.value());
      }
      else if(!clockOv[x].isActive()){
        rollup.disableOverride(clockOv[x].priority.value());
      }
    }
  }
}
void OverrideManager::checkOverride(byte id,   Heater &heater, byte hr, byte mn){
  for(int x = 0; x < MAX_CLOCK_OV;x++){
    if(clockOv[x].ID.value() == id){
      if(clockOv[x].enabled.value()){
        clockOv[x].checkIfActive(hr, mn);
      }
      else{
        clockOv[x].desactivate();
      }
      if(clockOv[x].isActive()){
        heater.setOverride(clockOv[x].priority.value(), clockOv[x].target.value());
      }
      else if(!clockOv[x].isActive()){
        heater.disableOverride(clockOv[x].priority.value());
      }
    }
  }
}
void OverrideManager::checkOverride(byte id,  Fan &fan, byte hr, byte mn){
  for(int x = 0; x < MAX_CLOCK_OV;x++){
    if(clockOv[x].ID.value() == id){
      if(clockOv[x].enabled.value()){
        clockOv[x].checkIfActive(hr, mn);
      }
      else{
        clockOv[x].desactivate();
      }
      if(clockOv[x].isActive()){
        fan.setOverride(clockOv[x].priority.value(), clockOv[x].target.value());
      }
      else if(!clockOv[x].isActive()){
        fan.disableOverride(clockOv[x].priority.value());
      }
    }
  }
}

void OverrideManager::initOverride(byte id, byte prior, byte comp, float var, float hys, byte state){
  if(idIsUnique(id)){
    if(_floatOvCount < MAX_FLOAT_OV){
      floatOv[_floatOvCount].ID.setValue(id);
      floatOv[_floatOvCount].enabled.setValue(true);
      floatOv[_floatOvCount].priority.setValue(prior);
      floatOv[_floatOvCount].comparator.setValue(comp);
      floatOv[_floatOvCount].floatVar.setValue(var);
      floatOv[_floatOvCount].hyst.setValue(hys);
      floatOv[_floatOvCount].target.setValue(state);
      _floatOvCount +=1;
    }
    else{
      Serial.println("Float override overflow - make some space!");
    }
  }
  else{
  floatOv[indexPosition(id)].enabled.setValue(true);
  floatOv[indexPosition(id)].priority.setValue(prior);
  floatOv[indexPosition(id)].comparator.setValue(comp);
  floatOv[indexPosition(id)].floatVar.setValue(var);
  floatOv[indexPosition(id)].hyst.setValue(hys);
  floatOv[indexPosition(id)].target.setValue(state);
  }
}


void OverrideManager::checkOverride(byte id,   Rollup &rollup, float variable){
  for(int x = 0; x < MAX_FLOAT_OV;x++){
    if(floatOv[x].ID.value() == id){
      if(floatOv[x].enabled.value()){
        floatOv[x].checkIfActive(variable);
      }
      else{
        floatOv[x].desactivate();
      }
      if(floatOv[x].isActive()){
        rollup.setOverride(floatOv[x].priority.value(), floatOv[x].target.value());
      }
      else if(!floatOv[x].isActive()){
        rollup.disableOverride(floatOv[x].priority.value());
      }
    }
  }
}
void OverrideManager::checkOverride(byte id,   Heater &heater, float variable){
  for(int x = 0; x < MAX_FLOAT_OV;x++){
    if(floatOv[x].ID.value() == id){
      if(floatOv[x].enabled.value()){
        floatOv[x].checkIfActive(variable);
      }
      else{
        floatOv[x].desactivate();
      }
      if(floatOv[x].isActive()){
        heater.setOverride(floatOv[x].priority.value(), floatOv[x].target.value());
      }
      else if(!floatOv[x].isActive()){
        heater.disableOverride(floatOv[x].priority.value());
      }
    }
  }
}
void OverrideManager::checkOverride(byte id,   Fan &fan, float variable){
  for(int x = 0; x < MAX_FLOAT_OV;x++){
    if(floatOv[x].ID.value() == id){
      if(floatOv[x].enabled.value()){
        floatOv[x].checkIfActive(variable);
      }
      else{
        floatOv[x].desactivate();
      }
      if(floatOv[x].isActive()){
        fan.setOverride(floatOv[x].priority.value(), floatOv[x].target.value());
      }
      else if(!floatOv[x].isActive()){
        fan.disableOverride(floatOv[x].priority.value());
      }
    }
  }
}

void OverrideManager::initOverride(byte id, byte prior, byte state){
  if(idIsUnique(id)){
    if(_boolOvCount < MAX_CLOCK_OV){
      boolOv[_boolOvCount].ID.setValue(id);
      boolOv[_boolOvCount].enabled.setValue(true);
      boolOv[_boolOvCount].priority.setValue(prior);
      boolOv[_boolOvCount].target.setValue(state);
      _boolOvCount +=1;
    }
    else{
      Serial.println("Bool override overflow - make some space!");
    }
  }
  else{
    boolOv[indexPosition(id)].enabled.setValue(true);
    boolOv[indexPosition(id)].priority.setValue(prior);
    boolOv[indexPosition(id)].target.setValue(state);
  }

}

void OverrideManager::checkOverride(byte id,   Rollup &rollup, bool variable){
  for(int x = 0; x < MAX_BOOL_OV;x++){
    if(boolOv[x].ID.value() == id){
        if(boolOv[x].enabled.value()){
          boolOv[x].checkIfActive(variable);
        }
        else{
          boolOv[x].desactivate();
        }
        if(boolOv[x].isActive()){
          rollup.setOverride(boolOv[x].priority.value(), boolOv[x].target.value());
        }
        else if(!boolOv[x].isActive()){
          rollup.disableOverride(boolOv[x].priority.value());
        }
    }
  }
}
void OverrideManager::checkOverride(byte id,   Heater &heater, bool variable){
  for(int x = 0; x < MAX_BOOL_OV;x++){
    if(boolOv[x].ID.value() == id){
      if(boolOv[x].enabled.value()){
        boolOv[x].checkIfActive(variable);
      }
      else{
        boolOv[x].desactivate();
      }
      if(boolOv[x].isActive()){
        heater.setOverride(boolOv[x].priority.value(), boolOv[x].target.value());
      }
      else if(!boolOv[x].isActive()){
        heater.disableOverride(boolOv[x].priority.value());
      }
    }
  }
}
void OverrideManager::checkOverride(byte id,   Fan &fan, bool variable){
  for(int x = 0; x < MAX_BOOL_OV;x++){
    if(boolOv[x].ID.value() == id){
      if(boolOv[x].enabled.value()){
        boolOv[x].checkIfActive(variable);
      }
      else{
        boolOv[x].desactivate();
      }
      if(boolOv[x].isActive()){
        fan.setOverride(boolOv[x].priority.value(), boolOv[x].target.value());
      }
      else if(!boolOv[x].isActive()){
        fan.disableOverride(boolOv[x].priority.value());
      }
    }
  }
}



//Clock RELATIVE
      unsigned short ClockRelativeOverride::_EEPROMindex = 0;

      ClockRelativeOverride::ClockRelativeOverride(){

        _localIndex = CLOCKOV_INDEX + _EEPROMindex;
        _EEPROMindex += CLOCKOV_INDEX_SIZE;

        _active = false;
        ID.setAddress(_localIndex);
        _localIndex += sizeof(byte);
        priority.setAddress(_localIndex);
        _localIndex += sizeof(byte);
        enabled.setAddress(_localIndex);
        _localIndex += sizeof(boolean);
        target.setAddress(_localIndex);
        _localIndex += sizeof(byte);
        hrStart.setLimits(0,23);
        hrStart.setAddress(_localIndex);
        _localIndex += sizeof(byte);
        mnStart.setLimits(0,59);
        mnStart.setAddress(_localIndex);
        _localIndex += sizeof(byte);
        hrStop.setLimits(0,23);
        hrStop.setAddress(_localIndex);
        _localIndex += sizeof(byte);
        mnStop.setLimits(0,59);
        mnStop.setAddress(_localIndex);

      }

      void ClockRelativeOverride::checkIfActive(byte hr, byte mn){
        if(isBetween(hrStart.value(), mnStart.value(), hr, mn, hrStop.value(), mnStop.value())){
          activate();
        }
        else{
          desactivate();
        }
      }

//FLOAT RELATIVE
      unsigned short FloatRelativeOverride::_EEPROMindex = 0;

      FloatRelativeOverride::FloatRelativeOverride(){
        _localIndex = FLOATOV_INDEX + _EEPROMindex;
        _EEPROMindex += FLOATOV_INDEX_SIZE;

        _active = false;
        ID.setAddress(_localIndex);
        _localIndex += sizeof(byte);
        priority.setAddress(_localIndex);
        _localIndex += sizeof(byte);
        enabled.setAddress(_localIndex);
        _localIndex += sizeof(boolean);
        target.setAddress(_localIndex);
        _localIndex += sizeof(byte);
        comparator.setAddress(_localIndex);
        _localIndex += sizeof(byte);
        floatVar.setAddress(_localIndex);
        _localIndex += sizeof(float);
        hyst.setAddress(_localIndex);
        _localIndex += sizeof(float);

      }

      void FloatRelativeOverride::checkIfActive(float variable){
        if (comparator.value() == ABOVE){
          if (variable > floatVar.value()){
            activate();
          }
          else if(variable <= (floatVar.value() - hyst.value())){
            desactivate();
          }
        }
        if (comparator.value() == BELOW){
          if (variable < floatVar.value()){
            activate();
          }
          else if(variable >= (floatVar.value() + hyst.value())){
            desactivate();
          }
        }
      }

//BOOL RELATIVE
      unsigned short BoolRelativeOverride::_EEPROMindex = 0;

      BoolRelativeOverride::BoolRelativeOverride(){

        _localIndex = BOOLOV_INDEX + _EEPROMindex;
        _EEPROMindex += BOOLOV_INDEX_SIZE;

        _active = false;
        ID.setAddress(_localIndex);
        _localIndex += sizeof(byte);
        priority.setAddress(_localIndex);
        _localIndex += sizeof(byte);
        enabled.setAddress(_localIndex);
        _localIndex += sizeof(boolean);
        target.setAddress(_localIndex);
        _localIndex += sizeof(boolean);

      }

      void BoolRelativeOverride::checkIfActive(bool variable){
        if(variable == true){
          activate();
        }
        else{
          desactivate();
        }
      }
//OVERRIDE
/*
      bool OverrideR::initialized(){
        if(ID.value() != OFF_VAL){
          return true;
        }
        else{
          return false;
        }
      }*/
      bool OverrideR::isActive(){
        return _active;
      }

      void OverrideR::activate(){
        _active = true;
      }

      void OverrideR::desactivate(){
        _active = false;
      }
