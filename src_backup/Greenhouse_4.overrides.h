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

/*ROLLUP CALIBRATION - FIX OVERRIDE

Conditions :
 - rollup has been fully open or fully close for a while (SAFETY_DELAY)
 - rollup is not moving
Action :
 - full opening or full closing cycle
*/

//OVERRIDES ID
#define FULL_VENTILATION    90
#define DESHUM_CYCLE_FIX    93
#define WIND_LOW            95
#define WIND_STRONG         96
#define ROLLUP_CALIBRATION  97
#define FIX_R1              98
#define FIX_R2              99
#define FIX_F1              100
#define FIX_F2              101
#define FIX_H1              102
#define FIX_H2              103
#define RAIN1                104
#define RAIN2                105
#define DESHUM_AUTO1         106
#define DESHUM_AUTO2         107
#define DESHUM_AUTO_COLD1    108
#define DESHUM_AUTO_COLD2    109

boolean condition1 = false;
boolean condition2 = false;

byte mnVent = 1;
float deshum_hot = 80;
float deshum_cold = 80;

boolean rainOverride(byte ID, Rollup rollup){
  float shuttingTemp = greenhouse.coolingTemp() + rollup.stage[0].target.value();
  byte targetIncrement = 20;
  if((rain == true)&&(rollup.incrementCounter() >= targetIncrement)&&(greenhouseTemperature >= shuttingTemp )){
    return true;
  }
  else{
    return false;
  }
}



void checkOverrides(){
    greenhouse.checkOverride(FIX_R1,R1,greenhouse.hr(), greenhouse.mn());
    greenhouse.checkOverride(FIX_R2,R2,greenhouse.hr(), greenhouse.mn());
    greenhouse.checkOverride(FIX_F1,F1,greenhouse.hr(), greenhouse.mn());
    greenhouse.checkOverride(FIX_F2,F2,greenhouse.hr(), greenhouse.mn());
    greenhouse.checkOverride(FIX_H1,H1,greenhouse.hr(), greenhouse.mn());
    greenhouse.checkOverride(FIX_H2,H2, greenhouse.hr(), greenhouse.mn());
    greenhouse.checkOverride(DESHUM_AUTO1,H1,greenhouseHumidity);
    greenhouse.checkOverride(DESHUM_AUTO2,H2,greenhouseHumidity);
    greenhouse.checkOverride(DESHUM_AUTO_COLD1,F1,greenhouseHumidity);
    greenhouse.checkOverride(DESHUM_AUTO_COLD2, F2, greenhouseHumidity);
    greenhouse.checkOverride(RAIN1, R1, rainOverride(RAIN1, R1));
    greenhouse.checkOverride(RAIN2, R2, rainOverride(RAIN2, R2));
}
