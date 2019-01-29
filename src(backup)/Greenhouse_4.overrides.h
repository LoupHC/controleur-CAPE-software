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
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

/*ROLLUP CALIBRATION - FIX OVERRIDE

Conditions :
 - rollup has been fully open or fully close for a while (SAFETY_DELAY)
 - rollup is not moving
Action :
 - full opening or full closing cycle
*/

//OVERRIDES ID
#define FULL_VENTILATION    9000
#define DESHUM_CYCLE_AUTO   9001
#define DESHUM_CYCLE_FIX    9002
#define RAIN                9003
#define WIND_STRONG         9004
#define WIND_VERY_STRONG    9005
#define ROLLUP_CALIBRATION  9006

#ifdef FULL_VENTILATION
  #define TIME_IN_MINUTS    5
#endif

//OVERRIDES ATTRIBUTION

#define OVERRIDE1           FULL_VENTILATION
#define OVERRIDE1_PRIORITY  1

#define OVERRIDE2           DESHUM_CYCLE_AUTO
#define OVERRIDE2_PRIORITY  4

#define OVERRIDE3           DESHUM_CYCLE_FIX
#define OVERRIDE3_PRIORITY  4

#define OVERRIDE4           RAIN
#define OVERRIDE4_PRIORITY  3

boolean condition1 = false;
boolean condition2 = false;



boolean isBetween(unsigned short startHour, unsigned short startMin, unsigned short actualHour, unsigned short actualMin, unsigned short stopHour, unsigned short stopMin){
  unsigned short startTime = startHour*60 + startMin;
  unsigned short actualTime = actualHour*60 + actualMin;
  unsigned short stopTime = stopHour*60 + stopMin;

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


void initOverrides(){
    greenhouse.createOverrideProgram(FULL_VENTILATION,2);
    greenhouse.createOverrideProgram(DESHUM_CYCLE_AUTO,4);
    greenhouse.createOverrideProgram(DESHUM_CYCLE_FIX,4);
    greenhouse.createOverrideProgram(RAIN,3);
}
/*
void initOverride(unsigned short ID){
  if(ID == FULL_VENTILATION){
    R1.initOverride(greenhouse.overridePriority(ID), 100);
    R2.initOverride(greenhouse.overridePriority(ID), 100);
    F1.initOverride(greenhouse.overridePriority(ID),true);
    //F2.initOverride(greenhouse.overridePriority[ID],true);
  }
  if(ID == DESHUM_CYCLE_COLD){
    R1.initOverride(greenhouse.overridePriority(ID), 30);
    R2.initOverride(greenhouse.overridePriority(ID), 30);
    H1.initOverride(greenhouse.overridePriority(ID),true);
  }
  if(ID == RAIN){
    R1.initOverride(greenhouse.overridePriority(ID), 20);
    R2.initOverride(greenhouse.overridePriority(ID), 20);
  }
}

void setOverrideProgram(unsigned short ID, boolean state){
  for(int x = 0; x < MAX_OVERRIDE_PROGRAMS; x++){
    if(greenhouse.overrideProgram[x].ID == ID){
      if(state == true){
        greenhouse.overrideProgram[x].active = true;
        initOverride(greenhouse.overrideProgram[x].ID);
      }
      else{
        greenhouse.overrideProgram[x].active = false;
      }
      return;
    }
  }
}
*/

void checkOverrides(){
  for(int x; x < MAX_OVERRIDE_PROGRAMS;x++){
    if(greenhouse.overrideProgram[x].ID == FULL_VENTILATION){
      if(greenhouse.overrideIsActive(FULL_VENTILATION)){

        if(condition1 == true){
          greenhouse.setOutput(FULL_VENTILATION, R1, 100);
          greenhouse.setOutput(FULL_VENTILATION, R2, 100);
          greenhouse.setOutput(FULL_VENTILATION, F1, true);
        }
        else{
          greenhouse.disableOutput(FULL_VENTILATION, R1);
          greenhouse.disableOutput(FULL_VENTILATION, R2);
          greenhouse.disableOutput(FULL_VENTILATION, F1);
        }
      }
    }
    if(greenhouse.overrideProgram[x].ID == DESHUM_CYCLE_AUTO){
      if(greenhouse.overrideIsActive(DESHUM_CYCLE_AUTO)){
        byte maxHumidity = 85;
        byte hysteresis = 5;

        if(greenhouse.overrideIsActive(DESHUM_CYCLE_AUTO)){
          if(greenhouseHumidity.value() > maxHumidity){
            greenhouse.setOutput(DESHUM_CYCLE_AUTO, H1, true);
          }
          else if (greenhouseHumidity.value() < maxHumidity - hysteresis){
            greenhouse.disableOutput(DESHUM_CYCLE_AUTO, H1);
          }
        }
      }
    }
    if(greenhouse.overrideProgram[x].ID == DESHUM_CYCLE_FIX){
      if(greenhouse.overrideIsActive(DESHUM_CYCLE_FIX)){

        byte rollupIncrement = 20;
        byte rollupStartHour = 10;
        byte rollupStartMinut = 20;
        byte rollupStopHour = 10;
        byte rollupStopMinut = 35;
        byte heaterStartHour = 10;
        byte heaterStartMinut = 20;
        byte heaterStopHour = 10;
        byte heaterStopMinut = 30;

        if((isBetween(rollupStartHour, rollupStartMinut, greenhouse.rightNow(HOUR), greenhouse.rightNow(MINUT), rollupStopHour, rollupStopMinut))){
          greenhouse.setOutput(DESHUM_CYCLE_FIX, R1, rollupIncrement);
          greenhouse.setOutput(DESHUM_CYCLE_FIX, R2, rollupIncrement);
        }
        else{
          greenhouse.disableOutput(DESHUM_CYCLE_FIX, R1);
          greenhouse.disableOutput(DESHUM_CYCLE_FIX, R2);
        }
        if((isBetween(heaterStartHour, heaterStartMinut, greenhouse.rightNow(HOUR), greenhouse.rightNow(MINUT), heaterStopHour, heaterStopMinut))){
          greenhouse.setOutput(DESHUM_CYCLE_FIX, H1, true);
        }
        else{
          greenhouse.disableOutput(DESHUM_CYCLE_FIX, H1);
        }
      }
    }
    if(greenhouse.overrideProgram[x].ID == RAIN){
      if(greenhouse.overrideIsActive(RAIN)){
        if((rain.value() == true)&&(R1.incrementCounter() >= 20)){
          greenhouse.setOutput(RAIN, R1, 20);
        }
        else{
          greenhouse.disableOutput(RAIN, R1);
        }
        if((rain.value() == true)&&(R2.incrementCounter() >= 20)){
          greenhouse.setOutput(RAIN, R2, 20);
        }
        else{
          greenhouse.disableOutput(RAIN, R2);
        }
      }
    }
  }
}
