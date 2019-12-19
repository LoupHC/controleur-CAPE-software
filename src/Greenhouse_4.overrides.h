/*
  Greenhouse_prototype_CAPE_v.1.1.ino
  Copyright (C)2017 Loup Hébert-Chartrand. All right reserved

  This code has been made to interface with Arduino-like microcontrollers,
  for inclusion in greenhouse automation devices.

  Supported devices :
  - DS18B20 temperature sensor
  - DS3231 RTC module
  - 20x4 Serial LCD Display

  You can find the latest version of this code at :
  https://github.com/LoupHC/controleur-CAPE


  This code is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This code is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.z

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

/*ROLLUP CALIBRATION - FIXOV OVERRIDE

Conditions :
 - rollup has been fully open or fully close for a while (SAFETY_DELAY)
 - rollup is not moving
Action :
 - full opening or full closing cycle
*/

//OVERRIDES ID
//#define FULL_VENTILATION    90
//#define DESHUM_CYCLE_FIXOV    93
//#define ROLLUP_CALIBRATION  97

#define WINDOV            95
#define CLOCKOV1                 96
#define CLOCKOV2                 97
#define CLOCKOV3                 98
#define RAINOV                99
#define OUTTEMP                100
#define DESHUM              106
#define PULSE              107
#define ANALOG              108

boolean condition1 = false;
boolean condition2 = false;

byte mnVent = 1;
float deshum_hot = 80;
float deshum_cold = 80;


Alarm &alarm = greenhouse.alarm;
boolean motorFailure[3] = {false,false,false};
boolean overcurrentAlarm = false;

elapsedMillis windTimer[3];

void initAlarms(){
  //Add safety alarm
  alarm.init(MCP23008, ACT_LOW, ALARM_PIN);
  alarm.addSequence(1, 1000, 59000);
  alarm.addSequence(2, 5000, 55000);
}

void checkAlarms(){
  if(greenhouse.lowTempAlarm.value() == true){
    alarm.below(greenhouseTemperature.value(),greenhouse.minTemp.value());
  }
  if(greenhouse.highTempAlarm.value() == true){
    alarm.above(greenhouseTemperature.value(), greenhouse.maxTemp.value());
  }
  for (byte x = 0; x < greenhouse.rollups.value(); x++){
    if(motorFailure[x] == true){
      overcurrentAlarm = true;
    }
  }
  if((motorFailure[0] == false)&&(motorFailure[1] == false)&&(motorFailure[2] == false)){
    overcurrentAlarm = false;
  }
  alarm.conditionalTo(sensorFailure, 1);
  alarm.conditionalTo(overcurrentAlarm, 2);
  alarm.checkAlarm();
}

boolean rainOverride(byte ID, Rollup rollup){
  float shuttingTemp = greenhouse.coolingTemp() + rollup.stage[0].target.value();
  byte targetIncrement = rollup.overrideTarget(RAINOV);
  if((rain == true)&&(rollup.incrementCounter() >= targetIncrement)&&(greenhouseTemperature.value() >= shuttingTemp )){
    return true;
  }
  else{
    return false;
  }
}

boolean override40 = false;
boolean override42 = false;
boolean override44 = false;
boolean override46 = false;

void externalOverrides(){
  if(digitalRead(40) == LOW){
    if(override40 == false){
      override40 = true;
      greenhouse.device[0].forceStart();
    }
  }
  else{
    if(override40 == true){
      override40 = false;
      greenhouse.device[0].unforce();
    }
  }
  if(digitalRead(42) == LOW){
    if(override42 == false){
      override42 = true;
      greenhouse.device[1].forceStart();
    }
  }
  else{
    if(override42 == true){
      override42 = false;
      greenhouse.device[1].unforce();
    }

  }
  if(digitalRead(44) == LOW){
    if(override44 == false){
      override44 = true;
      greenhouse.rollup[0].forceAt(100);
    }
  }
  else{
    if(override44 == true){
      override44 = false;
      greenhouse.rollup[0].unforce();
    }
  }
  if(digitalRead(46) == LOW){
    if(override46 == false){
      override46 = true;
      greenhouse.rollup[0].forceAt(0);
    }
  }
  else{
    if(override46 == true){
      override46 = false;
      greenhouse.rollup[0].unforce();
    }
  }
}

void checkOverrides(){
  externalOverrides();

  for (byte x = 0; x < greenhouse.rollups.value(); x++){
    for (byte y = 0; y < 3; y++){
      if(greenhouse.rollup[x].ovType(CLOCKOV1+y) == HRCONDITIONAL){
        if(greenhouse.rollup[x].routinePosition( greenhouseTemperature.value(), greenhouse.coolingTemp()) <= greenhouse.rollup[x].overrideTarget(CLOCKOV1+y)){
          greenhouse.rollup[x].checkOverride(greenhouse.rollup[x].id(CLOCKOV1+y),greenhouse.hr(), greenhouse.mn(), greenhouseHumidity.value());
          Serial.print(greenhouse.rollup[0].routinePosition( greenhouseTemperature.value(), greenhouse.coolingTemp()));
          Serial.print(":");
          Serial.println(greenhouse.rollup[x].overrideTarget(CLOCKOV1+y));
        }
        else{
          //disable
          greenhouse.rollup[x].checkOverride(greenhouse.rollup[x].id(CLOCKOV1+y),greenhouse.hr(), greenhouse.mn(), 0);
        }
      }
      else{
        greenhouse.rollup[x].checkOverride(greenhouse.rollup[x].id(CLOCKOV1+y),greenhouse.hr(), greenhouse.mn());
      }
    }

    greenhouse.rollup[x].checkOverride(RAINOV, rainOverride(RAINOV, R1));

    //HUMIDITY OVERRIDE
    if(greenhouse.rollup[x].routinePosition( greenhouseTemperature.value(), greenhouse.coolingTemp()) <= greenhouse.rollup[x].overrideTarget(DESHUM)){
      greenhouse.rollup[x].checkOverride(DESHUM,greenhouseHumidity.value());
    }
    else{
      //disable
      greenhouse.rollup[x].checkOverride(DESHUM,(float)0);
    }
//WIND OVERRIDE
    if(!greenhouse.rollup[x].isActive(WINDOV)){
      greenhouse.rollup[x].checkOverride(WINDOV, (float)windSpeed);
      windTimer[x] = 0;
    }
    else{
      if(windTimer[x] >= 120000){
        greenhouse.rollup[x].checkOverride(WINDOV, (float)windSpeed);
        windTimer[x] = 0;
      }
    }
//OUTSIDE TEMP
    greenhouse.rollup[x].checkOverride(OUTTEMP, outsideTemperature.value());

//OVERCURRENT
    R1.checkCurrent(r1current);
    R2.checkCurrent(r2current);
    if(greenhouse.rollup[x].overCurrentWarning() == true){
      motorFailure[x] = true;
    }
    else{
      motorFailure[x] = false;
    }
  }

  for (byte x = 0; x < greenhouse.devices.value(); x++){
    for (byte y = 0; y < 3; y++){
      //PROGRAM 1
      if(greenhouse.device[x].ovType(CLOCKOV1+y) == WEATHERCONDITIONAL){
        greenhouse.device[x].checkOverride(greenhouse.device[x].id(CLOCKOV1+y),greenhouse.hr(), greenhouse.mn(), greenhouse.weather());
      }
      else if(greenhouse.device[x].ovType(CLOCKOV1+y) == SOILCONDITIONAL){
        greenhouse.device[x].checkOverride(greenhouse.device[x].id(CLOCKOV1+y),greenhouse.hr(), greenhouse.mn(), soilMoisture.value());
      }
      else if(greenhouse.device[x].ovType(CLOCKOV1+y) == UNDERCONDITIONAL){
        greenhouse.device[x].checkOverride(greenhouse.device[x].id(CLOCKOV1+y),greenhouse.hr(), greenhouse.mn(), greenhouseHumidity.value());
      }
      else if(greenhouse.device[x].ovType(CLOCKOV1+y) == HRCONDITIONAL){
        greenhouse.device[x].checkOverride(greenhouse.device[x].id(CLOCKOV1+y),greenhouse.hr(), greenhouse.mn(), greenhouseHumidity.value());
      }
      else{
        greenhouse.device[x].checkOverride(greenhouse.device[x].id(CLOCKOV1+y),greenhouse.hr(), greenhouse.mn());
      }
    }
    float analogInput = (float)mV/(float)1000;
    greenhouse.device[x].checkOverride(ANALOG, analogInput);
    greenhouse.device[x].checkOverride(PULSE,greenhouse.device[x].pulse.value());

  }
}
