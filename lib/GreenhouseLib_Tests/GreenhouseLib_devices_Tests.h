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
//#include "elapsedMillis.h"
//#include "Parameters_Tests.h"
//#include "GreenhouseLib_rollups_Tests.h"



#ifndef GreenhouseLib_devices_Tests_h
#define GreenhouseLib_devices_Tests_h

/*
class Override_Tests: public TestOnce {
  protected:

    void setup() override {
      int _localIndex = 3000;
      for(int x = 0; x < MAX_OVERRIDES;x++){
        test.overRide[x].active = false;
        test.overRide[x].type.setAddress(_localIndex);
        _localIndex += sizeof(byte);
        test.overRide[x].ID.setAddress(_localIndex);
        _localIndex += sizeof(byte);
        test.overRide[x].enabled.setAddress(_localIndex);
        _localIndex += sizeof(boolean);
        test.overRide[x].hrStart.setLimits(0,23);
        test.overRide[x].hrStart.setAddress(_localIndex);
        _localIndex += sizeof(byte);
        test.overRide[x].mnStart.setLimits(0,59);
        test.overRide[x].mnStart.setAddress(_localIndex);
        _localIndex += sizeof(byte);
        test.overRide[x].hrStop.setLimits(0,23);
        test.overRide[x].hrStop.setAddress(_localIndex);
        _localIndex += sizeof(byte);
        test.overRide[x].mnStop.setLimits(0,59);
        test.overRide[x].mnStop.setAddress(_localIndex);
        _localIndex += sizeof(byte);
        test.overRide[x].target.setLimits(0,100);
        test.overRide[x].target.setAddress(_localIndex);
        _localIndex += sizeof(byte);
        test.overRide[x].comparator.setLimits(0,5);
        test.overRide[x].comparator.setAddress(_localIndex);
        _localIndex += sizeof(byte);
        test.overRide[x].floatVar.setLimits(-100,100);
        test.overRide[x].floatVar.setAddress(_localIndex);
        _localIndex += sizeof(float);
        test.overRide[x].hyst.setLimits(0,5);
        test.overRide[x].hyst.setAddress(_localIndex);
        _localIndex += sizeof(float);
        test.overRide[x].boolVar.setAddress(_localIndex);
        _localIndex += sizeof(boolean);
      }
    }

    void teardown() override {
      for(int x = 3000; x < 4000; x++){
        EEPROM.update(x, 0);
      }
    }

  Device test;
};

testF(Override_Tests, Override_enabled){
  int TEST = 100;
  test.initOverride(TEST, 0, 10, 0, 12, 0, true);
  test.enableOverride(TEST, true);
  assertTrue(test.enabled(TEST));
}

testF(Override_Tests, Override_Fix_true){
  int TEST = 100;
  test.initOverride(TEST, 0, 10, 0, 12, 0, true);
  test.enableOverride(TEST, true);
  test.checkOverride(TEST, 10, 30);
  assertTrue(test.overRide[1].active);
}

testF(Override_Tests, Override_Fix_false){
  int TEST = 100;
  test.initOverride(TEST, 0, 10, 0, 12, 0, true);
  test.enableOverride(TEST, true);
  test.checkOverride(TEST, 9, 30);
  assertFalse(test.overRide[1].active);
}

testF(Override_Tests, Override_Floatrel_true){
  byte TEST = 100;
  int testPassed = 0;
  float target = 20.30;
  float hyst = 5.00;
  float temp = 20.31;
  test.initOverride(TEST, 0, ABOVE, target, hyst, true);
  test.enableOverride(TEST, true);
  test.checkOverride(TEST, temp);
  if(test.overRide[1].active ){
    testPassed++;
  };
  temp = 15.50;
  test.checkOverride(TEST, temp);
  if(test.overRide[1].active){
    testPassed++;
  };
  temp = 15.20;
  test.checkOverride(TEST, temp);
  if(!test.overRide[1].active){
    testPassed++;
  };
  assertEqual(testPassed, 3);
}

testF(Override_Tests, Override_Floatrel_false){
  byte TEST = 100;
  float target = 20.30;
  float hyst = 5.00;
  float temp = 20.29;
  test.initOverride(TEST, 0, ABOVE, target, hyst, true);
  test.enableOverride(TEST, true);
  test.checkOverride(TEST, temp);
  assertFalse(test.overRide[1].active);
}

testF(Override_Tests, Override_Boolrel_true1){
  byte TEST = 100;
  bool testBool = true;
  test.initOverride(TEST, 0, true, true);
  test.enableOverride(TEST, true);
  test.checkOverride(TEST, testBool);
  assertTrue(test.overRide[1].active);
}
testF(Override_Tests, Override_Boolrel_true2){
  byte TEST = 100;
  bool testBool = false;
  test.initOverride(TEST, 0, false, true);
  test.enableOverride(TEST, true);
  test.checkOverride(TEST, testBool);
  assertTrue(test.overRide[1].active);
}
*/
test (Alarm_above_test){
    alarm.above(30,20);
    alarm.checkAlarm();
    assertTrue(alarm.isActive());
}
test (Alarm_below_test){
    alarm.below(15,20);
    alarm.checkAlarm();
    assertTrue(alarm.isActive());
}
test (Alarm_offrange_test1){
    alarm.offRange(-10, 10, 20);
    alarm.checkAlarm();
    assertTrue(alarm.isActive());
}
test (Alarm_offrange_test2){
    alarm.offRange(30, 10, 20);
    alarm.checkAlarm();
    assertTrue(alarm.isActive());
}
test (Alarm_conditional_test){
    boolean test = true;
    alarm.conditionalTo(test);
    alarm.checkAlarm();
    assertTrue(alarm.isActive());
}

test(Output_test_MCP){
  int testPassed = 0;
  Output test;

  test.init(MCP23008, ACT_HIGH, 0);
  test.start();
  if(test.isActive()){
    testPassed++;
  }
  test.stop();
  if(!test.isActive()){
    testPassed++;
  }
  assertEqual(testPassed, 2);
}

test(Output_test_IO){
  int testPassed = 0;
  Output test;

  test.init(IO, ACT_HIGH, 11);
  test.start();
  if(test.isActive()){
    testPassed++;
  }
  test.stop();
  if(!test.isActive()){
    testPassed++;
  }
  assertEqual(testPassed, 2);
}
#endif
