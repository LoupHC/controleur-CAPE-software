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

boolean condition1 = false;
boolean condition2 = false;

byte mnVent = 1;
float deshum_hot = 80;
float deshum_cold = 80;

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



void checkOverrides(){

  for (byte x = 0; x < greenhouse.rollups.value(); x++){
    greenhouse.rollup[x].checkOverride(CLOCKOV1,greenhouse.hr(), greenhouse.mn());
    greenhouse.rollup[x].checkOverride(CLOCKOV2,greenhouse.hr(), greenhouse.mn());
    greenhouse.rollup[x].checkOverride(CLOCKOV3,greenhouse.hr(), greenhouse.mn());
    greenhouse.rollup[x].checkOverride(RAINOV, rainOverride(RAINOV, R1));
    //greenhouse.rollup[x].checkOverride(WIND, windSpeed);
    //greenhouse.rollup[x].checkOverride(OUTTEMP, greenhouseTemperature);
  }
  for (byte x = 0; x < greenhouse.devices.value(); x++){
    greenhouse.device[x].checkOverride(CLOCKOV1,greenhouse.hr(), greenhouse.mn());
    greenhouse.device[x].checkOverride(CLOCKOV2,greenhouse.hr(), greenhouse.mn());
    greenhouse.device[x].checkOverride(CLOCKOV3,greenhouse.hr(), greenhouse.mn());
    greenhouse.device[x].checkOverride(DESHUM,greenhouseHumidity.value());
  }
}
