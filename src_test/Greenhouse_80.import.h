
elapsedMillis exportTimer;
unsigned short exportCounter = 0;
unsigned short maxLines = MAX_LINES_PER_FILE;
unsigned long exportDelay = EXPORT_DELAY;
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

  if(greenhouse.rollups.value() >= 1){
    R1.EEPROMGet();
  }
  if(greenhouse.rollups.value() == 2){
    R2.EEPROMGet();
  }
  if(greenhouse.devices.value() >= 1){
    D1.EEPROMGet();
  }
  if(greenhouse.devices.value() == 2){
    D2.EEPROMGet();
  }
  /*
  if(greenhouse.heaters.value() >= 1){
    H1.EEPROMGet();
  }
  if(greenhouse.heaters.value() == 2){
    H2.EEPROMGet();
  }*/

  if(greenhouse.timepoints.value() >= 1){
    T1.EEPROMGet();
  }
  if(greenhouse.timepoints.value() >= 2){
    T2.EEPROMGet();
  }
  if(greenhouse.timepoints.value() >= 3){
    T3.EEPROMGet();
  }
  if(greenhouse.timepoints.value() == 4){
    T4.EEPROMGet();
  }
}


void defaultSettings(){
    greenhouse.initGreenhouse(TIMEZONE, LATITUDE, LONGITUDE, TIMEPOINTS, ROLLUPS, DEVICES, false, false);
    greenhouse.startingTime(rightNow);

    #if ROLLUPS >= 1
      R1.setParameters(STAGES, R1_HYST, R1_ROTUP, R1_ROTDOWN, R1_PAUSE, false);
    #endif
    #if ROLLUPS >= 1 && STAGES >= 1
      R1.stage[0].mod.setValue(R1_S0_MOD);
      R1.stage[0].target.setValue(R1_S0_TARGET);
      R1.stage[1].mod.setValue(R1_S1_MOD);
      R1.stage[1].target.setValue(R1_S1_TARGET);
    #endif
    #if ROLLUPS >= 1 && STAGES >= 2
      R1.stage[2].mod.setValue(R1_S2_MOD);
      R1.stage[2].target.setValue(R1_S2_TARGET);
    #endif
    #if ROLLUPS >= 1 && STAGES >= 3
      R1.stage[3].mod.setValue(R1_S3_MOD);
      R1.stage[3].target.setValue(R1_S3_TARGET);
    #endif
    #if ROLLUPS >= 1 && STAGES >= 4
      R1.stage[4].mod.setValue(R1_S4_MOD);
      R1.stage[4].target.setValue(R1_S4_TARGET);
    #endif

    #if ROLLUPS == 2
      R2.setParameters(STAGES, R2_HYST, R2_ROTUP, R2_ROTDOWN, R2_PAUSE, false);
    #endif
    #if ROLLUPS == 2 && STAGES >= 1
      R2.stage[0].mod.setValue(R2_S0_MOD);
      R2.stage[0].target.setValue(R2_S0_TARGET);
      R2.stage[1].mod.setValue(R2_S1_MOD);
      R2.stage[1].target.setValue(R2_S1_TARGET);
    #endif
    #if ROLLUPS == 2 && STAGES >= 2
      R2.stage[2].mod.setValue(R2_S2_MOD);
      R2.stage[2].target.setValue(R2_S2_TARGET);
    #endif
    #if ROLLUPS == 2 && STAGES >= 3
      R2.stage[3].mod.setValue(R2_S3_MOD);
      R2.stage[3].target.setValue(R2_S3_TARGET);
    #endif
    #if ROLLUPS == 2 && STAGES >= 4
      R2.stage[4].mod.setValue(R2_S4_MOD);
      R2.stage[4].target.setValue(R2_S4_TARGET);
    #endif
    #if DEVICES >= 1
      D1.setParameters(D1_TYPE,D1_MOD, D1_HYST, false);
    #endif
    #if DEVICES == 2
      D2.setParameters(D2_TYPE,D2_MOD, D2_HYST, false);
    #endif
    #if TIMEPOINTS >= 1
      T1.setParameters(TP1_TYPE, TP1_HOUR, TP1_MN_MOD, TP1_HEAT_SUN, TP1_COOL_SUN, TP1_HEAT_CLOUD, TP1_COOL_CLOUD, TP1_RAMP);
    #endif
    #if TIMEPOINTS >= 2
      T2.setParameters(TP2_TYPE, TP2_HOUR, TP2_MN_MOD, TP2_HEAT_SUN, TP2_COOL_SUN, TP2_HEAT_CLOUD, TP2_COOL_CLOUD, TP2_RAMP);
    #endif
    #if TIMEPOINTS >= 3
      T3.setParameters(TP3_TYPE, TP3_HOUR, TP3_MN_MOD, TP3_HEAT_SUN, TP3_COOL_SUN, TP3_HEAT_CLOUD, TP3_COOL_CLOUD, TP3_RAMP);
    #endif
    #if TIMEPOINTS >= 4
      T4.setParameters(TP4_TYPE, TP4_HOUR, TP4_MN_MOD, TP4_HEAT_SUN, TP4_COOL_SUN, TP4_HEAT_CLOUD, TP4_COOL_CLOUD, TP4_RAMP);
    #endif
    #if TIMEPOINTS == 5
      T5.setParameters(TP5_TYPE, TP5_HOUR, TP5_MN_MOD, TP5_HEAT_SUN, TP5_COOL_SUN, TP5_HEAT_CLOUD, TP5_COOL_CLOUD, TP5_RAMP);
    #endif

    for(int x = 0; x < MAX_ROLLUPS;x++){
      greenhouse.rollup[x].initOverride(CLOCKOV1, FIX_R1_PRIORITY, R1_HRSTART, R1_MNSTART, R1_HRSTOP, R1_MNSTOP, R1_TARGET);
      greenhouse.rollup[x].disable(CLOCKOV1);
      greenhouse.rollup[x].initOverride(CLOCKOV2, FIX_R1_PRIORITY+1, R1_HRSTART+1, R1_MNSTART+1, R1_HRSTOP+1, R1_MNSTOP+1, R1_TARGET);
      greenhouse.rollup[x].disable(CLOCKOV2);
      greenhouse.rollup[x].initOverride(CLOCKOV3, FIX_R1_PRIORITY+2, R1_HRSTART+2, R1_MNSTART+2, R1_HRSTOP+2, R1_MNSTOP+2, R1_TARGET);
      greenhouse.rollup[x].disable(CLOCKOV3);
      greenhouse.rollup[x].initOverride(RAINOV, RAIN_PRIORITY, RAIN_TARGET);
      greenhouse.rollup[x].disable(RAINOV);

    }

    for(int x = 0; x < MAX_DEVICES;x++){
      greenhouse.device[x].initOverride(CLOCKOV1, FIX_D1_PRIORITY, D1_HRSTART, D1_MNSTART, D1_HRSTOP, D1_MNSTOP, D1_TARGET);
      greenhouse.device[x].disable(CLOCKOV1);
      greenhouse.device[x].initOverride(CLOCKOV2, FIX_D1_PRIORITY+1, D1_HRSTART+1, D1_MNSTART+1, D1_HRSTOP+1, D1_MNSTOP+1, D1_TARGET);
      greenhouse.device[x].disable(CLOCKOV2);
      greenhouse.device[x].initOverride(CLOCKOV3, FIX_D1_PRIORITY+2, D1_HRSTART+2, D1_MNSTART+2, D1_HRSTOP+2, D1_MNSTOP+2, D1_TARGET);
      greenhouse.device[x].disable(CLOCKOV3);
      greenhouse.device[x].initOverride(DESHUM, DESHUM_PRIORITY, ABOVE, DESHUM_HOT, 5, true);
      greenhouse.device[x].disable(DESHUM);
    }
}

void  excelSettings(){

    checkFeedback(EXCEL_PROGRAMMER_ID);
    if(connectionEstablished){
      short timezone = get(F("P4"));
      float latitude = get(F("L2"),F("M2"));
      float longitude =  get(F("L3"),F("M3"));
      byte timepoints = get(F("B7"));
      byte rollups =  get(F("B8"));
      byte devices = get(F("B10"));

      greenhouse.initGreenhouse( timezone,  latitude,  longitude,  timepoints,  rollups,  devices, false, false);
      greenhouse.startingTime(rightNow);

        if(greenhouse.rollups.value() >= 1){
          float hysteresis = get(F("J40"),F("K40"));
          int rotUp =  get(F("B38"));
          int rotDown =  get(F("B39"));
          int pause =  get(F("B40"));
          byte stages = get(F("B9"));
          R1.setParameters(stages, hysteresis, rotUp, rotDown, pause, true);
        }
        if((greenhouse.rollups.value() >= 1)&&(R1.stages.value() >=1)){
          float mod = get(F("J43"),F("K43"));
          int target =  get(F("E44"));
          R1.stage[0].mod.setValue(0);
          R1.stage[0].target.setValue(0);
          R1.stage[1].mod.setValue(mod);
          R1.stage[1].target.setValue(target);
        }
        if((greenhouse.rollups.value() >= 1)&&(R1.stages.value() >=2)){
          float mod = get(F("J45"),F("K45"));
          int target =  get(F("E46"));
          R1.stage[2].mod.setValue(mod);
          R1.stage[2].target.setValue(target);
        }
        if((greenhouse.rollups.value() >= 1)&&(R1.stages.value() >=3)){
          float mod = get(F("J47"),F("K47"));
          int target =  get(F("E48"));
          R1.stage[3].mod.setValue(mod);
          R1.stage[3].target.setValue(target);
        }
        if((greenhouse.rollups.value() >= 1)&&(R1.stages.value() >=4)){
          float mod = get(F("J49"),F("K49"));
          int target =  get(F("E50"));
          R1.stage[4].mod.setValue(mod);
          R1.stage[4].target.setValue(target);
        }
        if(greenhouse.rollups.value() == 2){
          float hysteresis = get(F("J55"),F("K55"));
          int rotUp =  get(F("B53"));
          int rotDown =  get(F("B54"));
          int pause =  get(F("B55"));
          byte stages = get(F("B9"));

          R2.setParameters(stages, hysteresis, rotUp, rotDown, pause, true);
        }
        if((greenhouse.rollups.value() == 2)&&(R2.stages.value() >=1)){
          float mod = get(F("J58"),F("K58"));
          int target =  get(F("E58"));
          R2.stage[0].mod.setValue(0);
          R2.stage[0].target.setValue(0);
          R2.stage[1].mod.setValue(mod);
          R2.stage[1].target.setValue(target);
        }
        if((greenhouse.rollups.value() == 2)&&(R2.stages.value() >=2)){
          float mod = get(F("J59"),F("K59"));
          int target =  get(F("E59"));
          R2.stage[2].mod.setValue(mod);
          R2.stage[2].target.setValue(target);
        }
        if((greenhouse.rollups.value() == 2)&&(R2.stages.value() >=3)){
          float mod = get(F("J60"),F("K60"));
          int target =  get(F("E60"));
          R2.stage[3].mod.setValue(mod);
          R2.stage[3].target.setValue(target);
        }
        if((greenhouse.rollups.value() == 2)&&(R2.stages.value() >=4)){
          float  mod = get(F("J61"),F("K61"));
          int target =  get(F("E61"));
          R2.stage[4].mod.setValue(mod);
          R2.stage[4].target.setValue(target);
        }
        if(greenhouse.devices.value() >= 1){
          float  mod = get(F("J72"),F("K72"));
          float hysteresis = get(F("J73"),F("K73"));
          D1.setParameters(D1_TYPE, mod, hysteresis, true);
        }
        if(greenhouse.devices.value() == 2){
          float  mod = get(F("J75"),F("K75"));
          float hysteresis = get(F("J76"),F("K76"));
          D2.setParameters(D2_TYPE, mod, hysteresis, true);
        }
        if(greenhouse.timepoints.value() >= 1){
          byte type = (byte) get(F("C15"));
          int hrMod =  get(F("C16"));
          int mnMod =  get(F("C17"));
          float heatSun = get(F("J15"),F("K15"));
          float coolSun = get(F("J16"),F("K16"));
          float heatCloud = get(F("J17"),F("K17"));
          float coolCloud = get(F("J18"),F("K18"));
          int ramp =  get(F("B22"));
          T1.setParameters(type, hrMod, mnMod, heatSun, coolSun, heatCloud, coolCloud, ramp);
        }
        if(greenhouse.timepoints.value() >= 2){
          byte type = (byte) get(F("E15"));
          int hrMod =  get(F("E16"));
          int mnMod =  get(F("E17"));
          float heatSun = get(F("L15"),F("M15"));
          float coolSun = get(F("L16"),F("M16"));
          float heatCloud = get(F("L17"),F("M17"));
          float coolCloud = get(F("L18"),F("M18"));
          int ramp =  get(F("D22"));
          T2.setParameters(type, hrMod, mnMod, heatSun, coolSun, heatCloud, coolCloud, ramp);
        }
        if(greenhouse.timepoints.value() >= 3){
          byte type = (byte) get(F("G15"));
          int hrMod =  get(F("G16"));
          int mnMod =  get(F("G17"));
          float heatSun = get(F("J21"),F("K21"));
          float coolSun = get(F("J22"),F("K22"));
          float heatCloud = get(F("J23"),F("K23"));
          float coolCloud = get(F("J24"),F("K24"));
          int ramp =  get(F("D22"));

          T3.setParameters(type, hrMod, mnMod, heatSun, coolSun, heatCloud, coolCloud, ramp);
        }
        if(greenhouse.timepoints.value() >= 4){
          byte type = (byte) get(F("I15"));
          int hrMod =  get(F("I16"));
          int mnMod =  get(F("I17"));
          float heatSun = get(F("L21"),F("K21"));
          float coolSun = get(F("L22"),F("M22"));
          float heatCloud = get(F("L23"),F("M23"));
          float coolCloud = get(F("L24"),F("M24"));
          int ramp =  get(F("H22"));
          T4.setParameters(type, hrMod, mnMod, heatSun, coolSun, heatCloud, coolCloud, ramp);
      }
/*
        if(get(F("AA7")) == 1){
          int priority = 2;//get(F("AA7"));
          int hrStart = get(F("AA8"));
          int mnStart = get(F("AB8"));
          int hrStop = get(F("AA9"));
          int mnStop = get(F("AB9"));
          int target = get(F("AA10"));
          greenhouse.initOverride(FIX_R1, priority, hrStart, mnStart, hrStop, mnStop, target);
        }
        else{
          greenhouse.disable(FIX_R1);
        }

        if(get(F("AC7")) == 1){
          int priority = 2;//get(F("AC7"));
          int hrStart = get(F("AC8"));
          int mnStart = get(F("AD8"));
          int hrStop = get(F("AC9"));
          int mnStop = get(F("AD9"));
          int target = get(F("AC10"));
          greenhouse.initOverride(FIX_R2, priority, hrStart, mnStart, hrStop, mnStop, target);
        }
        else{
          greenhouse.disable(FIX_R2);
        }
        if(get(F("AE7")) == 1){
          int priority = 1;//get(F("AE7"));
          int hrStart = get(F("AE8"));
          int mnStart = get(F("AF8"));
          int hrStop = get(F("AE9"));
          int mnStop = get(F("AF9"));
          int target = get(F("AE10"));
          greenhouse.initOverride(FIX_D1, priority, hrStart, mnStart, hrStop, mnStop, target);
        }
        else{
          greenhouse.disable(FIX_D1);
        }

        if(get(F("AG7")) == 1){
          int priority = 1;//get(F("AG7"));
          int hrStart = get(F("AG8"));
          int mnStart = get(F("AH8"));
          int hrStop = get(F("AG9"));
          int mnStop = get(F("AH9"));
          int target = get(F("AG10"));
          greenhouse.initOverride(FIX_D2, priority, hrStart, mnStart, hrStop, mnStop, target);
        }
        else{
          greenhouse.disable(FIX_D2);
        }

        if(get(F("AE11")) == 1){
          int priority = 2;//get(F("AK7"));
          int treshold = get(F("AE12"));
          greenhouse.initOverride(DESHUM_AUTO_COLD1, priority, ABOVE, treshold, 5, true);
        }
        else{
          greenhouse.disable(DESHUM_AUTO_COLD1);
        }

        if(get(F("AG11")) == 1){
          int priority = 2;//get(F("AK7"));
          int treshold = get(F("AG12"));
          greenhouse.initOverride(DESHUM_AUTO_COLD2, priority, ABOVE, treshold, 5,  true);
        }
        else{
          greenhouse.disable(DESHUM_AUTO_COLD2);
        }

        if(get(F("AI11")) == 1){
          int priority = 2;//get(F("AK7"));
          int treshold = get(F("AI12"));
          greenhouse.initOverride(DESHUM_AUTO1, priority, ABOVE, treshold, 5, true);
        }
        else{
          greenhouse.disable(DESHUM_AUTO1);
        }

        if(get(F("AK11")) == 1){
          int priority = 2;//get(F("AK7"));
          int treshold = get(F("AK12"));
          greenhouse.initOverride(DESHUM_AUTO2, priority, ABOVE, treshold, 5, true);
        }
        else{
          greenhouse.disable(DESHUM_AUTO2);
        }


        if(get(F("AA14")) == 1){
          int priority = 1;//get(F("AK7"));
          int target = get(F("AA15"));
          greenhouse.initOverride(RAIN1, priority, target);
        }
        else{
          greenhouse.disable(RAIN1);
        }

        if(get(F("AC14")) == 1){
          int priority = 1;//get(F("AK7"));
          int target = get(F("AC15"));
          greenhouse.initOverride(RAIN2, priority, target);
        }
        else{
          greenhouse.disable(RAIN2);
        }*/
    }
  }
void loadParameters(){
    #ifdef EXCEL_PROGRAMMER
      EEPROMReset = EXCEL_SETTINGS;
    #endif

    if(EEPROM[0] != FIRST_BOOT){   //flag indicates if EEPROM has been yet configured

      for (int i = 0 ; i < EEPROM.length() ; i++) {
        EEPROM.update(i, 0);
      }
      EEPROMReset = DEFAULT_SETTINGS;
    }
    EEPROM.update(0, FIRST_BOOT);
    Serial.println(EEPROM.read(0));

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
    if(greenhouse.rollups.value() >= 1){
      R1.initOutputs(MCP23008, ACT_HIGH, ROLLUP1_OPENING_PIN, ROLLUP1_CLOSING_PIN);
    }
    if(greenhouse.rollups.value() == 2){
      R2.initOutputs(MCP23008, ACT_HIGH, ROLLUP2_OPENING_PIN, ROLLUP2_CLOSING_PIN);
    }
    if(greenhouse.devices.value() == 1){
      D1.initOutput(MCP23008, ACT_HIGH, DEVICE1_PIN);
    }
    if(greenhouse.devices.value() == 2){
      D1.initOutput(MCP23008, ACT_HIGH, DEVICE1_PIN);
      D2.initOutput(MCP23008, ACT_HIGH, DEVICE2_PIN);
    }

    EEPROMReset = MEMORY_SETTINGS;


}
