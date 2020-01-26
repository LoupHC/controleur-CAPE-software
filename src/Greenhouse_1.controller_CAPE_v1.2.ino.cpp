# 1 "/tmp/tmpfGIu0A"
#include <Arduino.h>
# 1 "/home/loup/github/controleur-CAPE-software/src/Greenhouse_1.controller_CAPE_v1.2.ino"
# 52 "/home/loup/github/controleur-CAPE-software/src/Greenhouse_1.controller_CAPE_v1.2.ino"
#include "Arduino.h"


#include "Greenhouse_7.hardwareSettings.h"
#include "Greenhouse_2.parameters.h"
#include "Greenhouse_10.debugLines.h"

#include "GreenhouseLib.h"

Greenhouse greenhouse;



  Rollup &R1 = greenhouse.rollup[0];
  Rollup &R2 = greenhouse.rollup[1];
  Rollup &R3 = greenhouse.rollup[2];
  OnOffDevice &D1 = greenhouse.device[0];
  OnOffDevice &D2 = greenhouse.device[1];
  OnOffDevice &D3 = greenhouse.device[2];
  OnOffDevice &D4 = greenhouse.device[3];
  OnOffDevice &D5 = greenhouse.device[4];
  OnOffDevice &D6 = greenhouse.device[5];
  Timepoint &T1 = greenhouse.timepoint[0];
  Timepoint &T2 = greenhouse.timepoint[1];
  Timepoint &T3 = greenhouse.timepoint[2];
  Timepoint &T4 = greenhouse.timepoint[3];





#include "Greenhouse_3.sensors.h"


#include "Greenhouse_4.overrides.h"


#include "Greenhouse_8.import.h"
#include "Greenhouse_9.export.h"


#include "Greenhouse_5.interface.h"

#include "Preprocessor_error_codes.h"

#ifdef UNIT_TEST
  #include "GreenhouseLib_Tests.h"
#endif
void setup();
void loop();
#line 105 "/home/loup/github/controleur-CAPE-software/src/Greenhouse_1.controller_CAPE_v1.2.ino"
void setup() {

  Serial.begin(9600);
  delay(100);


  Wire.begin();

  initLCD(20,4);

  #ifdef KEYPAD_DISPLAY
    keypad.begin( makeKeymap(keys) );
  #endif
  initSensors();
  delay(1000);
  initAlarms();


  #ifdef RTC_TIME_SET
    rtc.setTime(HOUR_SET, MINUT_SET, SECOND_SET);
  #endif
  #ifdef RTC_DATE_SET
    rtc.setDate(DAY_SET, MONTH_SET, YEAR_SET);
  #endif

  getDateAndTime();
  getGreenhouseTemp();
  getGreenhouseHum();
  greenhouse.startingTime(rightNow);
  greenhouse.startingParameters();
  Sensor::setLogHour(greenhouse.hr(), greenhouse.mn());
  loadParameters();







  pinMode(40, INPUT_PULLUP);
  pinMode(42, INPUT_PULLUP);
  pinMode(44, INPUT_PULLUP);
  pinMode(46, INPUT_PULLUP);


  #ifdef UNIT_TEST
    TestRunner::setTimeout(30);
  #endif
  startRecordingToSD();

}





int x = 0;

void loop() {
  x++;

  getDateAndTime();
  Sensor::setLogHour(greenhouse.hr(), greenhouse.mn());


  getGreenhouseTemp();
  getOutsideTemp();
  getGreenhouseHum();
  getOutsideHum();
  getRain();
  getWind();
  getLux();
  getOnTime();

  autoWeather();


  lcdDisplay();
  checkOverrides();


  greenhouse.fullRoutine(rightNow, greenhouseTemperature.value());

  if(!D6.enabled.value()){
    checkAlarms();
  }

  #ifdef EXCEL_DATALOGGER
    dataloggingToExcel();
  #endif
    dataloggingToSD();
    dataloggingToSD();
  #ifdef UNIT_TEST
    TestRunner::run();
  #endif

}