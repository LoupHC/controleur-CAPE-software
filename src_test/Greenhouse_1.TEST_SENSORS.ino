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
#include "Greenhouse_10.debugLines.h"


#include "GreenhouseLib.h"

Greenhouse greenhouse;

//********************POINTERS**************************
/*
  Rollup &R1 = greenhouse.rollup[0];
  Rollup &R2 = greenhouse.rollup[1];
  OnOffDevice &D1 = greenhouse.device[0];
  OnOffDevice &D2 = greenhouse.device[1];
  Timepoint &T1 = greenhouse.timepoint[0];
  Timepoint &T2 = greenhouse.timepoint[1];
  Timepoint &T3 = greenhouse.timepoint[2];
  Timepoint &T4 = greenhouse.timepoint[3];
  Timepoint &T5 = greenhouse.timepoint[4];
  Alarm &alarm = greenhouse.alarm;
*/


//********************SENSORS**************************
//See "Greenhouse_sensors.h" for sensor functions
#include "Greenhouse_3.sensors.h"
//********************OVERRIDES**************************
//See "Greenhouse_overrides.h" for overrides functions
//#include "Greenhouse_4.overrides.h"
//********************IMPORTS/EXPORTS**************************
//See "Greenhouse_importExport.h" for import/export functions
//#include "Greenhouse_8.import.h"
//#include "Greenhouse_9.export.h"
//********************INTERFACE**************************
//See "Greenhouse_interface.h" for LCD display functions
#include "Greenhouse_5.interface.h"
//********************ERRORS**************************
//#include "Preprocessor_error_codes.h"

#ifdef UNIT_TEST
  #include "GreenhouseLib_Tests.h"
#endif

//***************************************************
//********************SETUP**************************
//***************************************************

void setup() {
  //start communication with serial monitor
  Serial.begin(9600); // opens serial port, sets data rate to 9600 bps
  delay(100);

  //start communication internal temp/humididty sensor
  Wire.begin();
  //start communication with LCD
  initLCD(20,4);
  //start communication with keypad
  #ifdef KEYPAD_DISPLAY
    keypad.begin( makeKeymap(keys) );
  #endif
  initSensors();
  //Add safety alarm
  //alarm.init(MCP23008, ACT_LOW, ALARM_PIN);
  //alarm.addSequence(1, 1000, 1000);

  // change RTC settings
  #ifdef RTC_TIME_SET
    rtc.setTime(HOUR_SET, MINUT_SET, SECOND_SET);
  #endif
  #ifdef RTC_DATE_SET
    rtc.setDate(DAY_SET, MONTH_SET, YEAR_SET);
  #endif
  //get sensors values
  getDateAndTime();
  getGreenhouseTemp();
  getGreenhouseHum();
  getRain();

  //loadParameters();

  //Load parameters from EEPROM or Greenhouse_parameters.h

//********************Parameters*******************

  //actual time, timepoint and targetTemp
  greenhouse.startingParameters();



  #ifdef UNIT_TEST
    TestRunner::setTimeout(30);
  #endif
  //readDataFromSDFile(rtc.getDateStr());
}


//***************************************************
//*********************LOOP**************************
//***************************************************
int x = 0;

void loop() {
  Serial.print("average:");
  Serial.println(greenhouseTemperature.average24h());
  Serial.print("now:");
  Serial.println(greenhouseTemperature.value());

  x++;
  //actual time
  getDateAndTime();
  //get data from sensors
  getGreenhouseTemp();
  getOutsideTemp();
  getGreenhouseHum();
  getOutsideHum();
  getRain();
  getCurrent();
  //adjustWeatherSettings
  autoWeather();
  //diplay infos on LCD screen
  lcdDisplay();
  #if defined(ALARM_MAX_TEMP) && !defined(ALARM_MIN_TEMP)
    //alarm.above(greenhouseTemperature,greenhouse.maxtemp.value());
  #endif
  #if defined(ALARM_MIN_TEMP) && !defined(ALARM_MAX_TEMP)
    //alarm.below(greenhouseTemperature, greenhouse.mintemp.value();
  #endif
  //alarm.conditionalTo(sensorFailure, 1);
  //alarm.checkAlarm();
  //timepoint and target temperatures definitions, outputs routine
  greenhouse.fullRoutine(rightNow, greenhouseTemperature.value());
  //checkOverrides();

  #ifdef EXCEL_DATALOGGER
    //dataloggingToExcel();
  #endif
    //dataloggingToSD();
  #ifdef UNIT_TEST
    TestRunner::run();
  #endif
}
