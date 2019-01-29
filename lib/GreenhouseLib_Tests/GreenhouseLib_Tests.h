/*
  GreenhouseLib.h
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

#include "Arduino.h"

#include <AUnit.h>
using namespace aunit;

//#include "elapsedMillis.h"
//#include "Parameters_Tests.h"
//#include "GreenhouseLib_rollups_Tests.h"
//#include "GreenhouseLib_FF_Tests.h
#include "GreenhouseLib_overrides_Tests.h"
#include "GreenhouseLib_devices_Tests.h"



#ifndef GreenhouseLib_Tests_h
#define GreenhouseLib_Tests_h




//test EEPROM address crossover between devices
test(Rollup_EEPROM_Overflow){
  int testPassed = 0;

  for(int x = 0; x < greenhouse.rollups.value();x++){
    if(greenhouse.rollups.value() > x+1){
      if(greenhouse.rollup[x].EEPROMIndexEnd() < greenhouse.rollup[x+1].EEPROMIndexBegin()){
          testPassed++;
      }
    }
    else {
      if(greenhouse.rollup[x].EEPROMIndexEnd() <= FAN_INDEX){
          testPassed++;
      }
    }
  }
  assertEqual(testPassed, greenhouse.rollups.value());
}

test(Fans_EEPROM_Overflow){
  int testPassed = 0;

  for(int x = 0; x < greenhouse.fans.value();x++){
    if(greenhouse.fans.value() > x+1){
      if(greenhouse.fan[x].EEPROMIndexEnd() < greenhouse.fan[x+1].EEPROMIndexBegin()){
          testPassed++;
      }
    }
    else if(greenhouse.heaters.value() >= 1){
      if(greenhouse.fan[x].EEPROMIndexEnd() < greenhouse.heater[0].EEPROMIndexBegin()){
          testPassed++;
      }
    }
    else{
      if(greenhouse.fan[x].EEPROMIndexEnd() < TIMEPOINT_INDEX){
          testPassed++;
      }
    }
  }
  assertEqual(testPassed, greenhouse.fans.value());
}
test(Heaters_EEPROM_Overflow){
  int testPassed = 0;

  for(int x = 0; x < greenhouse.fans.value();x++){
    if(greenhouse.heaters.value() > x+1){
      if(greenhouse.heater[x].EEPROMIndexEnd() < greenhouse.heater[x+1].EEPROMIndexBegin()){
          testPassed++;
      }
    }
    else{
      if(greenhouse.heater[x].EEPROMIndexEnd() < TIMEPOINT_INDEX){
          testPassed++;
      }
    }
  }
  assertEqual(testPassed, greenhouse.heaters.value());
}

test(Timepoints_EEPROM_Overflow){
  int testPassed = 0;

  for(int x = 0; x < greenhouse.timepoints.value();x++){
    if(greenhouse.timepoints.value() > x+1){
      if(greenhouse.timepoint[x].EEPROMIndexEnd() < greenhouse.timepoint[x+1].EEPROMIndexBegin()){
          testPassed++;
      }
    }
    else {
      testPassed++;
    }
  }
  assertEqual(testPassed, greenhouse.timepoints.value());
}


#endif
