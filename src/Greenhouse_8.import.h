
unsigned short exportCounter = 0;
unsigned short maxLines = MAX_LINES_PER_FILE;
boolean connectionEstablished = false;
byte EEPROMReset = MEMORY_SETTINGS;



int get(const __FlashStringHelper* coordinate){
  int get;
  Serial.print(F("CELL,GET,"));
  Serial.println(coordinate);
  get = Serial.parseInt();
  return get;
}

float get(const __FlashStringHelper* coordinate1, const __FlashStringHelper* coordinate2){
  int unit;
  int dec;
  float floatVal;

  unit =  get(coordinate1);
  dec =  get(coordinate2);
  floatVal = (float)unit + (float)dec/10;
  return floatVal;
}

void checkFeedback(unsigned short ID){
  unsigned short spreadsheetTag =  get(F("O1"));
  Serial.println(spreadsheetTag);

  if (spreadsheetTag == ID){
    connectionEstablished = true;
  }
  else{
    connectionEstablished = false;
  }

}



//***************************************************
//*********************IMPORTS**************************
//***************************************************

void memorySettings(){

  greenhouse.EEPROMGet();
  greenhouse.startingTime(rightNow);

    R1.EEPROMGet();
    R2.EEPROMGet();
    R3.EEPROMGet();
    D1.EEPROMGet();
    D2.EEPROMGet();
    D3.EEPROMGet();
    D4.EEPROMGet();
    D5.EEPROMGet();

    T1.EEPROMGet();
    T2.EEPROMGet();
    T3.EEPROMGet();
    T4.EEPROMGet();
}



void defaultSettings(){
    greenhouse.initGreenhouse(TIMEZONE, LATITUDE, LONGITUDE, TIMEPOINTS, ROLLUPS, DEVICES, false, false);

    greenhouse.startingTime(rightNow);

      R1.setParameters(STAGES, R1_HYST, R1_ROTUP, R1_ROTDOWN, R1_PAUSE, false);
      R1.stage[0].mod.setValue(R1_S0_MOD);
      R1.stage[0].target.setValue(R1_S0_TARGET);
      R1.stage[1].mod.setValue(R1_S1_MOD);
      R1.stage[1].target.setValue(R1_S1_TARGET);
      R1.stage[2].mod.setValue(R1_S2_MOD);
      R1.stage[2].target.setValue(R1_S2_TARGET);
      R1.stage[3].mod.setValue(R1_S3_MOD);
      R1.stage[3].target.setValue(R1_S3_TARGET);
      R1.stage[4].mod.setValue(R1_S4_MOD);
      R1.stage[4].target.setValue(R1_S4_TARGET);

      R2.setParameters(STAGES, R2_HYST, R2_ROTUP, R2_ROTDOWN, R2_PAUSE, false);
      R2.stage[0].mod.setValue(R2_S0_MOD);
      R2.stage[0].target.setValue(R2_S0_TARGET);
      R2.stage[1].mod.setValue(R2_S1_MOD);
      R2.stage[1].target.setValue(R2_S1_TARGET);
      R2.stage[2].mod.setValue(R2_S2_MOD);
      R2.stage[2].target.setValue(R2_S2_TARGET);
      R2.stage[3].mod.setValue(R2_S3_MOD);
      R2.stage[3].target.setValue(R2_S3_TARGET);
      R2.stage[4].mod.setValue(R2_S4_MOD);
      R2.stage[4].target.setValue(R2_S4_TARGET);

      R3.setParameters(STAGES, R2_HYST, 100, 100, R2_PAUSE, false);
      R3.stage[0].mod.setValue(R2_S0_MOD);
      R3.stage[0].target.setValue(R2_S0_TARGET);
      R3.stage[1].mod.setValue(R2_S1_MOD);
      R3.stage[1].target.setValue(R2_S1_TARGET);
      R3.stage[2].mod.setValue(R2_S2_MOD);
      R3.stage[2].target.setValue(R2_S2_TARGET);
      R3.stage[3].mod.setValue(R2_S3_MOD);
      R3.stage[3].target.setValue(R2_S3_TARGET);
      R3.stage[4].mod.setValue(R2_S4_MOD);
      R3.stage[4].target.setValue(R2_S4_TARGET);

      D1.setParameters(D1_TYPE,D1_MOD, D1_HYST, false);
      D2.setParameters(D2_TYPE,D2_MOD, D2_HYST, false);
      D3.setParameters(D2_TYPE,D2_MOD, D2_HYST, false);
      D4.setParameters(D2_TYPE,D2_MOD, D2_HYST, false);
      D5.setParameters(D2_TYPE,D2_MOD, D2_HYST, false);
      D6.setParameters(D2_TYPE,D2_MOD, D2_HYST, false);

      T1.setParameters(TP1_TYPE, TP1_HOUR, TP1_MN_MOD, TP1_HEAT_SUN, TP1_COOL_SUN, TP1_HEAT_CLOUD, TP1_COOL_CLOUD, TP1_RAMP);
      T2.setParameters(TP2_TYPE, TP2_HOUR, TP2_MN_MOD, TP2_HEAT_SUN, TP2_COOL_SUN, TP2_HEAT_CLOUD, TP2_COOL_CLOUD, TP2_RAMP);
      T3.setParameters(TP3_TYPE, TP3_HOUR, TP3_MN_MOD, TP3_HEAT_SUN, TP3_COOL_SUN, TP3_HEAT_CLOUD, TP3_COOL_CLOUD, TP3_RAMP);
      T4.setParameters(TP4_TYPE, TP4_HOUR, TP4_MN_MOD, TP4_HEAT_SUN, TP4_COOL_SUN, TP4_HEAT_CLOUD, TP4_COOL_CLOUD, TP4_RAMP);

    for(int x = 0; x < MAX_ROLLUPS;x++){
      greenhouse.rollup[x].initOverride(CLOCKOV1, CLOCKOV1_PRIORITY, R1_HRSTART, R1_MNSTART, R1_HRSTOP, R1_MNSTOP, R1_TARGET, 0,0);
      greenhouse.rollup[x].disable(CLOCKOV1);
      greenhouse.rollup[x].initOverride(CLOCKOV2, CLOCKOV2_PRIORITY, R1_HRSTART+1, R1_MNSTART+1, R1_HRSTOP+1, R1_MNSTOP+1, R1_TARGET,0, 0);
      greenhouse.rollup[x].disable(CLOCKOV2);
      greenhouse.rollup[x].initOverride(CLOCKOV3, CLOCKOV3_PRIORITY, R1_HRSTART+2, R1_MNSTART+2, R1_HRSTOP+2, R1_MNSTOP+2, R1_TARGET,0, 0);
      greenhouse.rollup[x].disable(CLOCKOV3);
      greenhouse.rollup[x].initOverride(OUTTEMP, OUTT_PRIORITY, BELOW, 5, 1, 0, 0,0);
      greenhouse.rollup[x].disable(OUTTEMP);
      greenhouse.rollup[x].initOverride(RAINOV, RAINOV_PRIORITY, RAIN_TARGET, 0,0);
      greenhouse.rollup[x].disable(RAINOV);
      greenhouse.rollup[x].initOverride(WINDOV, WINDOV_PRIORITY, ABOVE, MAX_WIND_SPEED, 10, 0,0, 0);
      greenhouse.rollup[x].disable(WINDOV);
      greenhouse.rollup[x].currentLimit.setValue(0);
      greenhouse.device[x].initOverride(LOCK, 1, 0, 0, 0);
      greenhouse.device[x].initOverride(EXT_LOCK, 0, 0, 0, 0);
      greenhouse.device[x].disable(LOCK);
      greenhouse.device[x].disable(EXT_LOCK);
    }




    for(int x = 0; x < MAX_DEVICES;x++){
      greenhouse.device[x].initOverride(CLOCKOV1, CLOCKOV1_PRIORITY, D1_HRSTART, D1_MNSTART, D1_HRSTOP, D1_MNSTOP, D1_TARGET, 0,0);
      greenhouse.device[x].disable(CLOCKOV1);
      greenhouse.device[x].initOverride(CLOCKOV2, CLOCKOV2_PRIORITY, D1_HRSTART+1, D1_MNSTART+1, D1_HRSTOP+1, D1_MNSTOP+1, D1_TARGET, 0,0);
      greenhouse.device[x].disable(CLOCKOV2);
      greenhouse.device[x].initOverride(CLOCKOV3, CLOCKOV3_PRIORITY, D1_HRSTART+2, D1_MNSTART+2, D1_HRSTOP+2, D1_MNSTOP+2, D1_TARGET, 0,0);
      greenhouse.device[x].disable(CLOCKOV3);
      greenhouse.device[x].initOverride(ANALOG, ANALOG_PRIORITY, ABOVE, 2.5, 0.2, true, 0,0);
      greenhouse.device[x].disable(ANALOG);
      greenhouse.device[x].initOverride(LOCK, 1, true, 0, 0);
      greenhouse.device[x].initOverride(EXT_LOCK, 0, true, 0, 0);
      greenhouse.device[x].disable(LOCK);
      greenhouse.device[x].disable(EXT_LOCK);
    }


    Serial.print(F("Version 5: "));
    Serial.println(EEPROM[0]);
}

void  excelSettings(){
}
void loadParameters(){
    #ifdef EXCEL_PROGRAMMER
      EEPROMReset = EXCEL_SETTINGS;
    #endif
    Serial.print(F("Version : "));
    Serial.println(EEPROM[0]);
    if(EEPROM[0] != FIRST_BOOT){   //flag indicates if EEPROM has been yet configured

      for (int i = 0 ; i < EEPROM.length() ; i++) {
        EEPROM.update(i, 0);
      }
      EEPROMReset = DEFAULT_SETTINGS;
    }
    EEPROM.update(0, FIRST_BOOT);
    Serial.print(F("Version : "));
    Serial.println(EEPROM[0]);
    #ifdef COMPUTER_INTERFACE
      EEPROMReset = DEFAULT_SETTINGS;
    #endif
    //If fresh start, set parameters from Greenhouse_parameters.h
    if(EEPROMReset == EXCEL_SETTINGS){
      excelSettings();
      memorySettings();
    }
    else if(EEPROMReset == DEFAULT_SETTINGS){
      defaultSettings();
      memorySettings();
    }
    else{
      memorySettings();
    }
    //Define pinout for each devices
      R1.initOutputs(MCP23008, ACT_HIGH, ROLLUP1_OPENING_PIN, ROLLUP1_CLOSING_PIN);
      R1.initCurrentPin(CURRENT_SENSOR1);
      R2.initOutputs(MCP23008, ACT_HIGH, ROLLUP2_OPENING_PIN, ROLLUP2_CLOSING_PIN);
      R2.initCurrentPin(CURRENT_SENSOR2);
      R3.initOutputs(MCP23008, ACT_HIGH, DEVICE1_PIN, DEVICE2_PIN);

      D1.initOutput(MCP23008, ACT_HIGH, ROLLUP1_OPENING_PIN);
      D2.initOutput(MCP23008, ACT_HIGH, ROLLUP2_OPENING_PIN);
      D3.initOutput(MCP23008, ACT_HIGH, DEVICE1_PIN);
      D4.initOutput(MCP23008, ACT_HIGH, DEVICE2_PIN);
      D5.initOutput(MCP23008, ACT_HIGH, DEVICE3_PIN);
      if(!greenhouse.alarmEnabled.value()){
        D6.initOutput(MCP23008, ACT_HIGH, ALARM_PIN);
      }

      Serial.print(F("Version : "));
      Serial.println(EEPROM[0]);

    EEPROMReset = MEMORY_SETTINGS;


}



void importParametersFromSD(){

  boolean boolvalue;
  float floatvalue;
  unsigned short usvalue;
  short svalue;
  byte bytevalue;

  for(int x = 0; x < MAX_ROLLUPS;x++){

    greenhouse.rollup[x].hyst.setValue(floatvalue);
    greenhouse.rollup[x].rotationUp.setValue(usvalue);
    greenhouse.rollup[x].rotationDown.setValue(usvalue);
    greenhouse.rollup[x].pause.setValue(usvalue);

    for(int y = 0; y < MAX_STAGES;y++){
      greenhouse.rollup[x].stage[y].mod.setValue(floatvalue);
      greenhouse.rollup[x].stage[y].target.setValue(usvalue);
    }
    greenhouse.rollup[x].stages.setValue(bytevalue);
    greenhouse.rollup[x].enabled.setValue(boolvalue);
    greenhouse.rollup[x].lock.setValue(boolvalue);
    greenhouse.rollup[x].lockTarget.setValue(bytevalue);
    greenhouse.rollup[x].currentLimit.setValue(floatvalue);

    for(int y = 0; y < MAX_CLOCK_OV;y++){
      greenhouse.rollup[x].clockOv[y].ID.setValue(bytevalue);
      greenhouse.rollup[x].clockOv[y].priority.setValue(bytevalue);
      greenhouse.rollup[x].clockOv[y].enabled.setValue(boolvalue);
      greenhouse.rollup[x].clockOv[y].target.setValue(bytevalue);
      greenhouse.rollup[x].clockOv[y].hrStart.setValue(bytevalue);
      greenhouse.rollup[x].clockOv[y].mnStart.setValue(bytevalue);
      greenhouse.rollup[x].clockOv[y].hrStop.setValue(bytevalue);
      greenhouse.rollup[x].clockOv[y].mnStop.setValue(bytevalue);
      greenhouse.rollup[x].clockOv[y].pulseOff.setValue(bytevalue);
      greenhouse.rollup[x].clockOv[y].pulseOn.setValue(bytevalue);
      greenhouse.rollup[x].clockOv[y].condition.setValue(bytevalue);
      greenhouse.rollup[x].clockOv[y].ovType.setValue(bytevalue);
    }
    for(int y = 0; y < MAX_BOOL_OV;y++){
      greenhouse.rollup[x].boolOv[y].ID.setValue(bytevalue);
      greenhouse.rollup[x].boolOv [y].priority.setValue(bytevalue);
      greenhouse.rollup[x].boolOv [y].enabled.setValue(boolvalue);
      greenhouse.rollup[x].boolOv [y].target.setValue(boolvalue);
      greenhouse.rollup[x].boolOv [y].pulseOff.setValue(boolvalue);
      greenhouse.rollup[x].boolOv [y].pulseOn.setValue(boolvalue);
    }
    for(int y = 0; y < MAX_FLOAT_OV;y++){
      greenhouse.rollup[x].floatOv[y].ID.setValue(bytevalue);
      greenhouse.rollup[x].floatOv [y].priority.setValue(bytevalue);
      greenhouse.rollup[x].floatOv [y].enabled.setValue(boolvalue);
      greenhouse.rollup[x].floatOv [y].target.setValue(bytevalue);
      greenhouse.rollup[x].floatOv [y].comparator.setValue(bytevalue);
      greenhouse.rollup[x].floatOv [y].floatVar.setValue(floatvalue);
      greenhouse.rollup[x].floatOv [y].hyst.setValue(floatvalue);
      greenhouse.rollup[x].floatOv [y].pulseOff.setValue(bytevalue);
      greenhouse.rollup[x].floatOv [y].pulseOn.setValue(bytevalue);
    }
  }

  for(int x = 0; x < MAX_DEVICES;x++){
    greenhouse.device[x].mod.setValue(floatvalue);
    greenhouse.device[x].hyst.setValue(floatvalue);
    greenhouse.device[x].type.setValue(bytevalue);
    greenhouse.device[x].enabled.setValue(boolvalue);
    greenhouse.device[x].lock.setValue(boolvalue);
    greenhouse.device[x].lockTarget.setValue(bytevalue);
    greenhouse.device[x].pulse.setValue(boolvalue);
    greenhouse.device[x].tensiometer.setValue(bytevalue);


    for(int y = 0; y < MAX_CLOCK_OV;y++){
      greenhouse.device[x].clockOv[y].ID.setValue(bytevalue);
      greenhouse.device[x].clockOv[y].priority.setValue(bytevalue);
      greenhouse.device[x].clockOv[y].enabled.setValue(boolvalue);
      greenhouse.device[x].clockOv[y].target.setValue(bytevalue);
      greenhouse.device[x].clockOv[y].hrStart.setValue(bytevalue);
      greenhouse.device[x].clockOv[y].mnStart.setValue(bytevalue);
      greenhouse.device[x].clockOv[y].hrStop.setValue(bytevalue);
      greenhouse.device[x].clockOv[y].mnStop.setValue(bytevalue);
      greenhouse.device[x].clockOv[y].pulseOff.setValue(bytevalue);
      greenhouse.device[x].clockOv[y].pulseOn.setValue(bytevalue);
      greenhouse.device[x].clockOv[y].condition.setValue(bytevalue);
      greenhouse.device[x].clockOv[y].ovType.setValue(bytevalue);
    }

    for(int y = 0; y < MAX_BOOL_OV;y++){
      greenhouse.device[x].boolOv[y].ID.setValue(bytevalue);
      greenhouse.device[x].boolOv [y].priority.setValue(bytevalue);
      greenhouse.device[x].boolOv [y].enabled.setValue(boolvalue);
      greenhouse.device[x].boolOv [y].target.setValue(boolvalue);
      greenhouse.device[x].boolOv [y].pulseOff.setValue(boolvalue);
      greenhouse.device[x].boolOv [y].pulseOn.setValue(boolvalue);
    }

    for(int y = 0; y < MAX_FLOAT_OV;y++){
      greenhouse.device[x].floatOv[y].ID.setValue(bytevalue);
      greenhouse.device[x].floatOv [y].priority.setValue(bytevalue);
      greenhouse.device[x].floatOv [y].enabled.setValue(boolvalue);
      greenhouse.device[x].floatOv [y].target.setValue(bytevalue);
      greenhouse.device[x].floatOv [y].comparator.setValue(bytevalue);
      greenhouse.device[x].floatOv [y].floatVar.setValue(floatvalue);
      greenhouse.device[x].floatOv [y].hyst.setValue(floatvalue);
      greenhouse.device[x].floatOv [y].pulseOff.setValue(bytevalue);
      greenhouse.device[x].floatOv [y].pulseOn.setValue(bytevalue);
    }
  }
  for(int x = 0; x < MAX_TIMEPOINTS;x++){
    greenhouse.timepoint[x].type.setValue(bytevalue);
    greenhouse.timepoint[x].mnMod.setValue(svalue);
    greenhouse.timepoint[x].hrMod.setValue(svalue);
    greenhouse.timepoint[x].coolingTemp.setValue(floatvalue);
    greenhouse.timepoint[x].heatingTemp.setValue(floatvalue);
    greenhouse.timepoint[x].coolingTempCloud.setValue(floatvalue);
    greenhouse.timepoint[x].heatingTempCloud.setValue(floatvalue);
    greenhouse.timepoint[x].ramping.setValue(usvalue);
  }
  greenhouse.timezone.setValue(svalue);
  greenhouse.latitude.setValue(floatvalue);
  greenhouse.longitude.setValue(floatvalue);
  greenhouse.timepoints.setValue(bytevalue);
  greenhouse.rollups.setValue(bytevalue);
  greenhouse.devices.setValue(bytevalue);
  greenhouse.daynight.setValue(boolvalue);
  greenhouse.weatheradjust.setValue(boolvalue);
  greenhouse.dif.setValue(boolvalue);
  greenhouse.prenight.setValue(boolvalue);
  greenhouse.weatherP.setValue(bytevalue);
  greenhouse.insideTempSensor.setValue(bytevalue);
  greenhouse.outsideTempSensor.setValue(bytevalue);
  greenhouse.luxMeter.setValue(bytevalue);
  greenhouse.rainSensor.setValue(bytevalue);
  greenhouse.anemometer.setValue(bytevalue);
  for(int y = 0; y < MAX_TENSIOMETERS;y++){
    greenhouse.tensiometer[y].setValue(bytevalue);
  }
  greenhouse.autoWeather.setValue(boolvalue);
  greenhouse.lowTempAlarm.setValue(bytevalue);
  greenhouse.highTempAlarm.setValue(bytevalue);
  greenhouse.minTemp.setValue(floatvalue);
  greenhouse.maxTemp.setValue(floatvalue);
  greenhouse.autoWeather.setValue(boolvalue);
  greenhouse.alarmEnabled.setValue(bytevalue);
  greenhouse.energySavingMode.setValue(boolvalue);


}
