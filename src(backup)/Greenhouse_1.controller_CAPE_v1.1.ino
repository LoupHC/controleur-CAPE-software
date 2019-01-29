/*
  Greenhouse_prototype_CAPE_v.1.1.ino
  Copyright (C)2017 Loup Hébert-Chartrand. All right reserved

  This code has was made to interface with Arduino-like microcontrollers,
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

//***************************************************************************
//***************************************************************************
//***************************************************************************
/*
 *                                WARNING!!!
 *
 * CHANGING ANYTHING BELOW THIS LINE COULD HAVE UNSUSPECTED EFFECTS ON YOUR
 * CONTROLLER'S BEHAVIOUR.
 * CHECK "Greenhouse_parameters.h" TO CHANGE GREENHOUSE'S SETTINGS.
 *
 * HAVE A LOOK THO! IT'S WHERE THE REAL MAGIC TAKE PLACE!!
 * IF YOU MAKE ANY IMPROVEMENTS TO THIS CODE THAT COULD BENEFIT OTHER FARMERS, AND
 * WISH TO SHARE IT, YOU CAN CONTACT ME AT : loup.hebert.chartrand@gmail.com
 *
 *
*/
//***************************************************************************
//***************************************************************************
//***************************************************************************

#include "Arduino.h"

//********************LIBRARIES**************************
#include "Greenhouse_7.hardwareSettings.h"
#include "Greenhouse_2.parameters.h"
#include "GreenhouseLib.h"

Greenhouse greenhouse(TIMEZONE, LATITUDE, LONGITUDE, TIMEPOINTS, ROLLUPS, STAGES, FANS, HEATERS);
Alarm alarm(ACT_LOW, ALARM_PIN);

//********************POINTERS**************************

#if ROLLUPS >=1
  Rollup &R1 = greenhouse.rollup[0];
#endif
#if ROLLUPS == 2
  Rollup &R2 = greenhouse.rollup[1];
#endif
#if FANS >= 1
  Fan &F1 = greenhouse.fan[0];
#endif
#if FANS == 2
  Fan &F2 = greenhouse.fan[1];
#endif
#if HEATERS >= 1
  Heater &H1 = greenhouse.heater[0];
#endif
#if HEATERS == 2
  Heater &H2 = greenhouse.heater[1];
#endif
#if TIMEPOINTS >= 1
  Timepoint &T1 = greenhouse.timepoint[0];
#endif
#if TIMEPOINTS >= 2
  Timepoint &T2 = greenhouse.timepoint[1];
#endif
#if TIMEPOINTS >= 3
  Timepoint &T3 = greenhouse.timepoint[2];
#endif
#if TIMEPOINTS >= 4
  Timepoint &T4 = greenhouse.timepoint[3];
#endif
#if TIMEPOINTS == 5
  Timepoint &T5 = greenhouse.timepoint[4];
#endif

//********************VARIABLES**************************

//Time array
byteParameter rightNow[6];
byte rightNowValue[6];
//Temperature inside the greenhouse
floatParameter greenhouseTemperature;
floatParameter greenhouseHumidity;
boolParameter rain;
float coolingTemperature;
float heatingTemperature;

boolean sensorFailure = false;

//********************SENSORS**************************
//See "Greenhouse_sensors.h" for sensor functions
#include "Greenhouse_3.sensors.h"
//********************OVERRIDES**************************
//See "Greenhouse_overrides.h" for overrides functions
#include "Greenhouse_4.overrides.h"
//********************INTERFACE**************************
//See "Greenhouse_interface.h" for LCD display functions
#include "Greenhouse_5.interface.h"
//********************IMPORTS/EXPORTS**************************
//See "Greenhouse_importExport.h" for import/export functions
#include "Greenhouse_8.importExport.h"
//********************ERRORS**************************
#include "Preprocessor_error_codes.h"


//***************************************************
//********************SETUP**************************
//***************************************************

void setup() {
  //start communication with serial monitor
  Serial.begin(9600); // opens serial port, sets data rate to 9600 bps
  //start communication internal temp/humididty sensor
  Wire.begin();
  //start communication with LCD
  initLCD(20,4);
  //start communication with keypad
  #ifdef KEYPAD_DISPLAY
    keypad.begin( makeKeymap(keys) );
  #endif
  //last recorded value if probe doesnt reply back at first cycle
  if(EEPROM.read(1) == 111){
    float emergencyTemp = EEPROM.read(2);
    greenhouseTemperature.setValue(emergencyTemp);
  }
  else{
    greenhouseTemperature.setValue(20);
  }
  if(EEPROM.read(3) == 111){
    float emergencyHum = EEPROM.read(4);
    greenhouseHumidity.setValue(emergencyHum);
  }
  else{
    greenhouseHumidity.setValue(50);
  }
  //start communication with temp probe
  #if GH_TEMPERATURE == DS18B20 || OUT_TEMPERATURE == DS18B20
    sensors.begin();
    sensors.setResolution(12);
  #endif
  #if GH_TEMPERATURE == SHT1X || OUT_TEMPERATURE == SHT1X || GH_HUMIDITY == SHT1X || OUT_HUMIDITY == SHT1X
    sht1x.initSensor();
  #endif
  //start communication with clock
  #if RTC == RTC_DS3231
    rtc.begin();
  #endif
  #if RAIN_SENSOR == HYDREON_RG11
    pinMode(RAIN_SWITCH, INPUT_PULLUP);
  #endif
  //start communication with relay driver
  #ifdef MCP_I2C_OUTPUTS
    mcp.begin();
  #endif
  //Add safety alarm
  alarm.init();
  alarm.addSequence(1, 200, 1000);

  // change RTC settings
  #ifdef RTC_TIME_SET
    rtc.setTime(HOUR_SET, MINUT_SET, SECOND_SET);
  #endif
  #ifdef RTC_DATE_SET
    rtc.setDate(DAY_SET, MONTH_SET, YEAR_SET);
  #endif
  //get sensors values
  getDateAndTime();
  greenhouse.startingTime(rightNowValue);
  getGreenhouseTemp();
  getGreenhouseHum();
  getRain();
  //Load parameters from EEPROM or Greenhouse_parameters.h
  loadParameters();

//********************Parameters*******************

  //actual time, timepoint and targetTemp
  greenhouse.startingParameters();
  initOverrides();

  greenhouse.setOverrideProgram(FULL_VENTILATION, true);
  greenhouse.setOverrideProgram(DESHUM_CYCLE_AUTO, true);
  greenhouse.setOverrideProgram(RAIN, true);

}


//***************************************************
//*********************LOOP**************************
//***************************************************
int x = 0;

void loop() {
  //Serial.println(greenhouseTemperature.value());
  //actual time
  getDateAndTime();
  //actual temperature
  getGreenhouseTemp();
  //actual humididty
  getGreenhouseHum();
  //rain
  getRain();
  //diplay infos on LCD screen
  lcdDisplay();

  #if defined(ALARM_MAX_TEMP) && !defined(ALARM_MIN_TEMP)
    alarm.above(greenhouseTemperature.value(),ALARM_MAX_TEMP);
  #endif
  #if defined(ALARM_MIN_TEMP) && !defined(ALARM_MAX_TEMP)
    alarm.below(greenhouseTemperature.value(), ALARM_MIN_TEMP);
  #endif
  #if defined(ALARM_MIN_TEMP) && defined(ALARM_MAX_TEMP)
    alarm.offRange(greenhouseTemperature.value(), ALARM_MIN_TEMP, ALARM_MAX_TEMP);
  #endif
  alarm.conditionalTo(sensorFailure, 1);
  alarm.checkAlarm();

  //timepoint and target temperatures definitions, outputs routine
  greenhouse.fullRoutine(rightNowValue, &coolingTemperature, &heatingTemperature);
  checkOverrides();
  #if ROLLUPS >= 1
    R1.routine(coolingTemperature, greenhouseTemperature.value());
  #endif

  #if ROLLUPS == 2
    R2.routine(coolingTemperature, greenhouseTemperature.value());
  #endif

  #if FANS >= 1
    F1.routine(coolingTemperature, greenhouseTemperature.value());

  #endif
  #if FANS == 2
    F2.routine(coolingTemperature, greenhouseTemperature.value());
  #endif

  #if HEATERS >= 1
    H1.routine(heatingTemperature, greenhouseTemperature.value());
  #endif

  #if HEATERS == 2
    H2.routine(heatingTemperature, greenhouseTemperature.value());
  #endif
  #ifdef EXCEL_DATALOGGER
    dataloggingToExcel();
  #endif
}
