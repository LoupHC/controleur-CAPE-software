/*
  Greenhouse_prototype_CAPE_v.1.1.ino
  Copyright (C)2017 Loup Hébert-Chartrand. All right reserved

  This code has been made to interface with Arduino-like microcontrollers,
  for inclusion in greenhouse automation devices.

  Supported devices :
  - DS18B20 temperature sensor
  - SHT1X temperature/humidity sensor
  - DS3231 RTC module
  - 20x4 LCD Display with I2C backpack

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

//*******************************************************************
/*
(SEE Greenhouse_userManual.h FOR MORE DETAILS)

TIMEPOINTS PARAMETERS - SYNTAX RULES:
- type variable(TYPE) :   SR, CLOCK or SS (sunrise, manual, sunset)
- hour variable(HOUR) :   SR/SS types => 0 -24 to 24
                          CLOCK type => 0 to 24
- min variable(MN_MOD) :  SR/SS types => -60 to 60
                          CLOCK type => 0 to 60
- heating temperature variable(HEAT) : 0 to 50
- cooling temperature variable(COOL) : 0 to 50
- ramping (RAMP) : 0 to 60
*/
//*******************************************************Timepoint 1
#define TP1_TYPE            SR
#define TP1_HOUR            0
#define TP1_MN_MOD          0
#define TP1_HEAT_SUN        20
#define TP1_COOL_SUN        22
#define TP1_HEAT_CLOUD      16
#define TP1_COOL_CLOUD      18
#define TP1_RAMP            15
//*******************************************************Timepoint 2
#define TP2_TYPE            CLOCK
#define TP2_HOUR            10
#define TP2_MN_MOD          0
#define TP2_HEAT_SUN        22
#define TP2_COOL_SUN        25
#define TP2_HEAT_CLOUD      18
#define TP2_COOL_CLOUD      20
#define TP2_RAMP            15
//*******************************************************Timepoint 3
#define TP3_TYPE            SS
#define TP3_HOUR            -1
#define TP3_MN_MOD          0
#define TP3_HEAT_SUN        18
#define TP3_COOL_SUN        20
#define TP3_HEAT_CLOUD      16
#define TP3_COOL_CLOUD      18
#define TP3_RAMP            15
//*******************************************************Timepoint 4
#define TP4_TYPE            SS
#define TP4_HOUR            0
#define TP4_MN_MOD          0
#define TP4_HEAT_SUN        18
#define TP4_COOL_SUN        20
#define TP4_HEAT_CLOUD      16
#define TP4_COOL_CLOUD      18
#define TP4_RAMP            15
//*******************************************************Timepoint 5

#define TP5_TYPE            SS
#define TP5_HOUR            0
#define TP5_MN_MOD          30
#define TP5_HEAT_SUN        16
#define TP5_COOL_SUN        20
#define TP5_HEAT_CLOUD      16
#define TP5_COOL_CLOUD      18
#define TP5_RAMP            30
//*******************************************************************
/*
(SEE Greenhouse_userManual.h FOR MORE DETAILS)

ROLLUP PARAMETERS - SYNTAX RULES :
- hysteresis (HYST): 0 to 5
- rotation up(ROTUP): 0 to 300
- rotation down (ROTDOWN): 0 to 300
- pause time(PAUSE): 0 to 240
*/
//*******************************************************Rollup 1 (overral parameters)
#define R1_HYST             1
#define R1_ROTUP            25
#define R1_ROTDOWN          25
#define R1_PAUSE            5
//*******************************************************Rollup 2 (overral parameters)

#define R2_HYST             1
#define R2_ROTUP            25
#define R2_ROTDOWN          25
#define R2_PAUSE            5

//*******************************************************************
/*
(SEE Greenhouse_userManual.h FOR MORE DETAILS)

ROLLUP STAGES - SYNTAX RULES :
- temperature mod(MOD) : -5 to 10
- target increment(TARGET): 0 to 100
*/
//*******************************************************Rollup 1 (stages)
#define R1_S0_MOD           0
#define R1_S0_TARGET        0
#define R1_S1_MOD           0
#define R1_S1_TARGET        25
#define R1_S2_MOD           1
#define R1_S2_TARGET        50
#define R1_S3_MOD           2
#define R1_S3_TARGET        75
#define R1_S4_MOD           3
#define R1_S4_TARGET        100
//*******************************************************Rollup 2 (stages)

#define R2_S0_MOD           0
#define R2_S0_TARGET        0
#define R2_S1_MOD           1
#define R2_S1_TARGET        10
#define R2_S2_MOD           2
#define R2_S2_TARGET        30
#define R2_S3_MOD           3
#define R2_S3_TARGET        50
#define R2_S4_MOD           4
#define R2_S4_TARGET        70

//*************************************************************************
/*
(SEE Greenhouse_userManual.h FOR MORE DETAILS)

FAN PARAMETERS - SYNTAX RULES:
- hysteresis (HYST): 0 to 5
- temperature modificator (MOD): -5 to 10
*/
//*******************************************************Fan parameters

#define F1_HYST             1
#define F1_MOD              1
#define F2_HYST             1
#define F2_MOD              3

//**********************************************************************
/*
(SEE Greenhouse_userManual.h FOR MORE DETAILS)

HEATER PARAMETERS - SYNTAX RULES:
- hysteresis : 0 to 5
- temperature modificator : -10 to 5
*/
//*******************************************************Heater parameters

#define H1_HYST             2
#define H1_MOD              -1
#define H2_HYST             2
#define H2_MOD              -1

//************************************************************************

/*ALARM
  Fire alarm (ALARM_PIN) when temperature get over a maximum or under a minimum
*/
#define ALARM_MIN_TEMP    5.00
#define ALARM_MAX_TEMP    35.00


/*ROLLUP CALIBRATION - FIX OVERRIDE

Conditions :
 - rollup has been fully open or fully close for a while (SAFETY_DELAY(min))
 - rollup is not moving
Action :
 - full opening or full closing cycle
*/
//#define OVERRIDE8 "FULL_VENTILATION"
//#define OVERRIDE9 "FULL_VENTILATION"

/*FULL VENTILATION - FIX OVERRIDE

Conditions :
 -Full ventiliation action is called
Action :
 - open rollups and start all fans for a while (FULL_VENTILATION_DELAY(min))
*/

/*#define FULL_VENTILATION
#define FULL_VENTILATION_DELAY        5
*/

/*DESHUM CYCLE - RELATIVE OVERRIDE

Conditions :
  - deshum cycle has been activated
  -between start time and stop time
  -temperature stays over a minimum
Action :
  - open rollups at target increment
  AND/OR
  - activate heater
  AND/OR
  - activate fan(s)
*/
/*
#define HEATER1_DESHUM
#define HEATER1_DESHUM_START_HOUR      6
#define HEATER1_DESHUM_START_MIN       5
#define HEATER1_DESHUM_STOP_HOUR       6
#define HEATER1_DESHUM_STOP_MIN        10

#define FAN1_DESHUM
#define FAN1_DESHUM_START_HOUR         6
#define FAN1_DESHUM_START_MIN          5
#define FAN1_DESHUM_STOP_HOUR          6
#define FAN1_DESHUM_STOP_MIN           20

#define ROLLUP1_DESHUM
#define ROLLUP1_DESHUM_INCREMENT       50
#define ROLLUP1_DESHUM_START_HOUR      6
#define ROLLUP1_DESHUM_START_MIN       5
#define ROLLUP1_DESHUM_STOP_HOUR       6
#define ROLLUP1_DESHUM_STOP_MIN        20

#define ROLLUP2_DESHUM
#define ROLLUP2_DESHUM_INCREMENT       50
#define ROLLUP2_DESHUM_START_HOUR      6
#define ROLLUP2_DESHUM_START_MIN       5
#define ROLLUP2_DESHUM_STOP_HOUR       6
#define ROLLUP2_DESHUM_STOP_MIN        20

#define DESHUM_MININIM                 14
*/


#define R1_FIX
#define FIX_R1_PRIORITY 3
#define R1_HRSTART  21
#define R1_MNSTART  50
#define R1_HRSTOP 22
#define R1_MNSTOP 15
#define R1_TARGET 50

//#define R2_FIX
#define FIX_R2_PRIORITY 3
#define R2_HRSTART  5
#define R2_MNSTART  5
#define R2_HRSTOP 5
#define R2_MNSTOP 5
#define R2_TARGET 50

//#define F1_FIX
#define FIX_F1_PRIORITY 2
#define F1_HRSTART  5
#define F1_MNSTART  5
#define F1_HRSTOP 5
#define F1_MNSTOP 5
#define F1_TARGET  true

//#define F2_FIX
#define FIX_F2_PRIORITY 2
#define F2_HRSTART  5
#define F2_MNSTART  5
#define F2_HRSTOP 5
#define F2_MNSTOP 5
#define F2_TARGET  true

#define H1_FIX
#define FIX_H1_PRIORITY 2
#define H1_HRSTART  21
#define H1_MNSTART  50
#define H1_HRSTOP 22
#define H1_MNSTOP 5
#define H1_TARGET  true

//#define H2_FIX
#define FIX_H2_PRIORITY 2
#define H2_HRSTART  5
#define H2_MNSTART  5
#define H2_HRSTOP 5
#define H2_MNSTOP 5
#define H2_TARGET  true

//#define DESHUM_AUTO_HEAT
#define DESHUM_PRIORITY    3
#define DESHUM_HOT         80

#define DESHUM_AUTO_FAN
#define DESHUM_COLD_PRIORITY    3
#define DESHUM_COLD        80

#define RAIN_CLOSE
#define RAIN_PRIORITY      2
#define RAIN_TARGET        20
