
elapsedMillis exportTimer;
unsigned short exportCounter = 0;
unsigned short maxLines = MAX_LINES_PER_FILE;
unsigned long exportDelay = EXPORT_DELAY;
boolean connectionEstablished = false;
byte EEPROMReset = MEMORY_SETTINGS;


void checkFeedback(unsigned short ID){
  Serial.println("CELL,GET,O1");
  unsigned short spreadsheetTag = Serial.parseInt();

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
  #if ROLLUPS >= 1
    R1.EEPROMGet();
  #endif
  #if ROLLUPS == 2
    R2.EEPROMGet();
  #endif
  #if FANS >=1
    F1.EEPROMGet();
  #endif
  #if FANS == 2
    F2.EEPROMGet();
  #endif
  #if HEATERS >=1
    H1.EEPROMGet();
  #endif
  #if HEATERS ==2
    H2.EEPROMGet();
  #endif

  #if TIMEPOINTS >= 1
  T1.EEPROMGet();
  #endif
  #if TIMEPOINTS >= 2
    T2.EEPROMGet();
  #endif
  #if TIMEPOINTS >= 3
    T3.EEPROMGet();
  #endif
  #if TIMEPOINTS >= 4
    T4.EEPROMGet();
  #endif
  #if TIMEPOINTS == 5
    T5.EEPROMGet();
  #endif
}

void defaultSettings(){
    #if ROLLUPS >= 1
      R1.setParameters(R1_HYST, R1_ROTUP, R1_ROTDOWN, R1_PAUSE);
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
      R2.setParameters(R2_HYST, R2_ROTUP, R2_ROTDOWN, R2_PAUSE);
    #endif
    #if ROLLUPS == 2 && STAGES >= 1
      R2.stage[0].mod.setValue(R1_S0_MOD);
      R2.stage[0].target.setValue(R1_S0_TARGET);
      R2.stage[1].mod.setValue(R1_S1_MOD);
      R2.stage[1].target.setValue(R1_S1_TARGET);
    #endif
    #if ROLLUPS == 2 && STAGES >= 2
      R2.stage[2].mod.setValue(R1_S2_MOD);
      R2.stage[2].target.setValue(R1_S2_TARGET);
    #endif
    #if ROLLUPS == 2 && STAGES >= 3
      R2.stage[3].mod.setValue(R1_S3_MOD);
      R2.stage[3].target.setValue(R1_S3_TARGET);
    #endif
    #if ROLLUPS == 2 && STAGES >= 4
      R2.stage[4].mod.setValue(R1_S4_MOD);
      R2.stage[4].target.setValue(R1_S4_TARGET);
    #endif
    #if FANS >= 1
      F1.setParameters(F1_MOD, F1_HYST);
    #endif
    #if FANS == 2
      F2.setParameters(F2_MOD, F2_HYST);
    #endif
    #if HEATERS >= 1
      H1.setParameters(H1_MOD, H1_HYST);
    #endif
    #if HEATERS == 2
      H2.setParameters(H2_MOD, H2_HYST);
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
}


void  excelSettings(){
    checkFeedback(EXCEL_PROGRAMMER_ID);
    if(connectionEstablished){
      int hysteresisU;
      int hysteresisD;
      float hysteresis;

      int heatSunU;
      int heatSunD;
      float heatSun;

      int coolSunU;
      int coolSunD;
      float coolSun;

      int heatCloudU;
      int heatCloudD;
      float heatCloud;

      int coolCloudU;
      int coolCloudD;
      float coolCloud;

      int modU;
      int modD;
      float mod;

      int rotUp;
      int rotDown;
      int pause;

      int target;
      byte type;
      int hrMod;
      int mnMod;

      int ramp;

        #if ROLLUPS >= 1
          Serial.println(F("CELL,GET,J40"));
          hysteresisU = Serial.parseInt();
          Serial.println(F("CELL,GET,K40"));
          hysteresisD = Serial.parseInt();
          hysteresis = (float)hysteresisU + (float)hysteresisD/10;

          Serial.println(F("CELL,GET,B38"));
          rotUp = Serial.parseInt();
          Serial.println(F("CELL,GET,B39"));
          rotDown = Serial.parseInt();
          Serial.println(F("CELL,GET,B40"));
          pause = Serial.parseInt();

          R1.setParameters(hysteresis, rotUp, rotDown, pause);
        #endif
        #if ROLLUPS >= 1 && STAGES >= 1
          Serial.println(F("CELL,GET,J43"));
          modU = Serial.parseInt();
          Serial.println(F("CELL,GET,K43"));
          modD = Serial.parseInt();
          mod = (float)modU + (float)modD/10;

          Serial.println(F("CELL,GET,E44"));
          target = Serial.parseInt();
          R1.stage[0].mod.setValue(0);
          R1.stage[0].target.setValue(0);
          R1.stage[1].mod.setValue(mod);
          R1.stage[1].target.setValue(target);
        #endif
        #if ROLLUPS >= 1 && STAGES >= 2
          Serial.println(F("CELL,GET,J45"));
          modU = Serial.parseInt();
          Serial.println(F("CELL,GET,K45"));
          modD = Serial.parseInt();
          mod = (float)modU + (float)modD/10;

          Serial.println(F("CELL,GET,E46"));
          target = Serial.parseInt();
          R1.stage[2].mod.setValue(mod);
          R1.stage[2].target.setValue(target);
        #endif
        #if ROLLUPS >= 1 && STAGES >= 3
          Serial.println(F("CELL,GET,J47"));
          modU = Serial.parseInt();
          Serial.println(F("CELL,GET,K47"));
          modD = Serial.parseInt();
          mod = (float)modU + (float)modD/10;

          Serial.println(F("CELL,GET,E48"));
          target = Serial.parseInt();
          R1.stage[3].mod.setValue(mod);
          R1.stage[3].target.setValue(target);
        #endif
        #if ROLLUPS >= 1 && STAGES >= 4
          Serial.println(F("CELL,GET,J49"));
          modU = Serial.parseInt();
          Serial.println(F("CELL,GET,K49"));
          modD = Serial.parseInt();
          mod = (float)modU + (float)modD/10;

          Serial.println(F("CELL,GET,E50"));
          target = Serial.parseInt();
          R1.stage[4].mod.setValue(mod);
          R1.stage[4].target.setValue(target);
        #endif
        #if ROLLUPS == 2
          Serial.println(F("CELL,GET,J55"));
          hysteresisU = Serial.parseInt();
          Serial.println(F("CELL,GET,K55"));
          hysteresisD = Serial.parseInt();
          hysteresis = (float)hysteresisU + (float)hysteresisD/10;

          Serial.println(F("CELL,GET,B53"));
          rotUp = Serial.parseInt();
          Serial.println(F("CELL,GET,B54"));
          rotDown = Serial.parseInt();
          Serial.println(F("CELL,GET,B55"));
          pause = Serial.parseInt();

          R2.setParameters(hysteresis, rotUp, rotDown, pause);
        #endif
        #if ROLLUPS == 2 && STAGES >= 1
          Serial.println(F("CELL,GET,J58"));
          modU = Serial.parseInt();
          Serial.println(F("CELL,GET,K58"));
          modD = Serial.parseInt();
          mod = (float)modU + (float)modD/10;

          Serial.println(F("CELL,GET,E58"));
          target = Serial.parseInt();
          R2.stage[0].mod.setValue(0);
          R2.stage[0].target.setValue(0);
          R2.stage[1].mod.setValue(mod);
          R2.stage[1].target.setValue(target);
        #endif
        #if ROLLUPS == 2 && STAGES >= 2
          Serial.println(F("CELL,GET,J59"));
          modU = Serial.parseInt();
          Serial.println(F("CELL,GET,K59"));
          modD = Serial.parseInt();
          mod = (float)modU + (float)modD/10;

          Serial.println(F("CELL,GET,E59"));
          target = Serial.parseInt();
          R2.stage[2].mod.setValue(mod);
          R2.stage[2].target.setValue(target);
        #endif
        #if ROLLUPS == 2 && STAGES >= 3
          Serial.println(F("CELL,GET,J60"));
          modU = Serial.parseInt();
          Serial.println(F("CELL,GET,K60"));
          modD = Serial.parseInt();
          mod = (float)modU + (float)modD/10;

          Serial.println(F("CELL,GET,E60"));
          target = Serial.parseInt();
          R2.stage[3].mod.setValue(mod);
          R2.stage[3].target.setValue(target);
        #endif
        #if ROLLUPS == 2 && STAGES >= 4
          Serial.println(F("CELL,GET,J61"));
          modU = Serial.parseInt();
          Serial.println(F("CELL,GET,K61"));
          modD = Serial.parseInt();
          mod = (float)modU + (float)modD/10;

          Serial.println(F("CELL,GET,E61"));
          target = Serial.parseInt();
          R2.stage[4].mod.setValue(mod);
          R2.stage[4].target.setValue(target);
        #endif
        #if FANS >= 1
          Serial.println(F("CELL,GET,J72"));
          modU = Serial.parseInt();
          Serial.println(F("CELL,GET,K72"));
          modD = Serial.parseInt();
          mod = (float)modU + (float)modD/10;

          Serial.println(F("CELL,GET,J73"));
          hysteresisU = Serial.parseInt();
          Serial.println(F("CELL,GET,K73"));
          hysteresisD = Serial.parseInt();
          hysteresis = (float)hysteresisU + (float)hysteresisD/10;

          F1.setParameters(mod, hysteresis);
        #endif
        #if FANS == 2
          Serial.println(F("CELL,GET,J75"));
          modU = Serial.parseInt();
          Serial.println(F("CELL,GET,K75"));
          modD = Serial.parseInt();
          mod = (float)modU + (float)modD/10;

          Serial.println(F("CELL,GET,J76"));
          hysteresisU = Serial.parseInt();
          Serial.println(F("CELL,GET,K76"));
          hysteresisD = Serial.parseInt();
          hysteresis = (float)hysteresisU + (float)hysteresisD/10;

          F2.setParameters(mod, hysteresis);
        #endif
        #if HEATERS >= 1
          Serial.println(F("CELL,GET,J78"));
          modU = Serial.parseInt();
          Serial.println(F("CELL,GET,K78"));
          modD = Serial.parseInt();
          mod = (float)modU + (float)modD/10;

          Serial.println(F("CELL,GET,J79"));
          hysteresisU = Serial.parseInt();
          Serial.println(F("CELL,GET,K79"));
          hysteresisD = Serial.parseInt();
          hysteresis = (float)hysteresisU + (float)hysteresisD/10;

          H1.setParameters(mod, hysteresis);
        #endif
        #if HEATERS == 2
          Serial.println(F("CELL,GET,J81"));
          modU = Serial.parseInt();
          Serial.println(F("CELL,GET,K81"));
          modD = Serial.parseInt();
          mod = (float)modU + (float)modD/10;
          Serial.println(F("CELL,GET,J82"));
          hysteresisU = Serial.parseInt();
          Serial.println(F("CELL,GET,K82"));
          hysteresisD = Serial.parseInt();
          hysteresis = (float)hysteresisU + (float)hysteresisD/10;

          H2.setParameters(mod, hysteresis);
        #endif
        #if TIMEPOINTS >= 1
          Serial.println(F("CELL,GET,C15"));
          type = (byte)Serial.parseInt();
          Serial.println(F("CELL,GET,C16"));
          hrMod = Serial.parseInt();
          Serial.println(F("CELL,GET,C17"));
          mnMod = Serial.parseInt();

          Serial.println(F("CELL,GET,J15"));
          heatSunU = Serial.parseInt();
          Serial.println(F("CELL,GET,K15"));
          heatSunD = Serial.parseInt();
          heatSun = (float)heatSunU + (float)heatSunD/10;

          Serial.println(F("CELL,GET,J16"));
          coolSunU = Serial.parseInt();
          Serial.println(F("CELL,GET,K16"));
          coolSunD = Serial.parseInt();
          coolSun = (float)coolSunU + (float)coolSunD/10;

          Serial.println(F("CELL,GET,J17"));
          heatCloudU = Serial.parseInt();
          Serial.println(F("CELL,GET,K17"));
          heatCloudD = Serial.parseInt();
          heatCloud = (float)heatCloudU + (float)heatCloudD/10;

          Serial.println(F("CELL,GET,J18"));
          coolCloudU = Serial.parseInt();
          Serial.println(F("CELL,GET,K18"));
          coolCloudD = Serial.parseInt();
          coolCloud = (float)coolCloudU + (float)coolCloudD/10;

          Serial.println(F("CELL,GET,B22"));
          ramp = Serial.parseInt();
          T1.setParameters(type, hrMod, mnMod, heatSun, coolSun, heatCloud, coolCloud, ramp);
        #endif
        #if TIMEPOINTS >= 2
          Serial.println(F("CELL,GET,E15"));
          type = (byte)Serial.parseInt();
          Serial.println(F("CELL,GET,E16"));
          hrMod = Serial.parseInt();
          Serial.println(F("CELL,GET,E17"));
          mnMod = Serial.parseInt();

          Serial.println(F("CELL,GET,L15"));
          heatSunU = Serial.parseInt();
          Serial.println(F("CELL,GET,M15"));
          heatSunD = Serial.parseInt();
          heatSun = (float)heatSunU + (float)heatSunD/10;

          Serial.println(F("CELL,GET,L16"));
          coolSunU = Serial.parseInt();
          Serial.println(F("CELL,GET,M16"));
          coolSunD = Serial.parseInt();
          coolSun = (float)coolSunU + (float)coolSunD/10;

          Serial.println(F("CELL,GET,L17"));
          heatCloudU = Serial.parseInt();
          Serial.println(F("CELL,GET,M17"));
          heatCloudD = Serial.parseInt();
          heatCloud = (float)heatCloudU + (float)heatCloudD/10;

          Serial.println(F("CELL,GET,L18"));
          coolCloudU = Serial.parseInt();
          Serial.println(F("CELL,GET,M18"));
          coolCloudD = Serial.parseInt();
          coolCloud = (float)coolCloudU + (float)coolCloudD/10;

          Serial.println(F("CELL,GET,D22"));
          ramp = Serial.parseInt();
          T2.setParameters(type, hrMod, mnMod, heatSun, coolSun, heatCloud, coolCloud, ramp);
        #endif
        #if TIMEPOINTS >= 3
          Serial.println(F("CELL,GET,G15"));
          type = (byte)Serial.parseInt();
          Serial.println(F("CELL,GET,G16"));
          hrMod = Serial.parseInt();
          Serial.println(F("CELL,GET,G17"));
          mnMod = Serial.parseInt();

          Serial.println(F("CELL,GET,J21"));
          heatSunU = Serial.parseInt();
          Serial.println(F("CELL,GET,K21"));
          heatSunD = Serial.parseInt();
          heatSun = (float)heatSunU + (float)heatSunD/10;

          Serial.println(F("CELL,GET,J22"));
          coolSunU = Serial.parseInt();
          Serial.println(F("CELL,GET,K22"));
          coolSunD = Serial.parseInt();
          coolSun = (float)coolSunU + (float)coolSunD/10;

          Serial.println(F("CELL,GET,J23"));
          heatCloudU = Serial.parseInt();
          Serial.println(F("CELL,GET,K23"));
          heatCloudD = Serial.parseInt();
          heatCloud = (float)heatCloudU + (float)heatCloudD/10;
          Serial.println(F("CELL,GET,J24"));
          coolCloudU = Serial.parseInt();
          Serial.println(F("CELL,GET,K24"));
          coolCloudD = Serial.parseInt();
          coolCloud = (float)coolCloudU + (float)coolCloudD/10;

          Serial.println(F("CELL,GET,F22"));
          ramp = Serial.parseInt();
          T3.setParameters(type, hrMod, mnMod, heatSun, coolSun, heatCloud, coolCloud, ramp);
        #endif
        #if TIMEPOINTS >= 4
          Serial.println(F("CELL,GET,I15"));
          type = (byte)Serial.parseInt();
          Serial.println(F("CELL,GET,I16"));
          hrMod = Serial.parseInt();
          Serial.println(F("CELL,GET,I17"));
          mnMod = Serial.parseInt();

          Serial.println(F("CELL,GET,L21"));
          heatSunU = Serial.parseInt();
          Serial.println(F("CELL,GET,M21"));
          heatSunD = Serial.parseInt();
          heatSun = (float)heatSunU + (float)heatSunD/10;

          Serial.println(F("CELL,GET,L22"));
          coolSunU = Serial.parseInt();
          Serial.println(F("CELL,GET,M22"));
          coolSunD = Serial.parseInt();
          coolSun = (float)coolSunU + (float)coolSunD/10;


          Serial.println(F("CELL,GET,L23"));
          heatCloudU = Serial.parseInt();
          Serial.println(F("CELL,GET,M23"));
          heatCloudD = Serial.parseInt();
          heatCloud = (float)heatCloudU + (float)heatCloudD/10;
          Serial.println(F("CELL,GET,L24"));
          coolCloudU = Serial.parseInt();
          Serial.println(F("CELL,GET,M24"));
          coolCloudD = Serial.parseInt();
          coolCloud = (float)coolCloudU + (float)coolCloudD/10;

          Serial.println(F("CELL,GET,H22"));
          ramp = Serial.parseInt();
          T4.setParameters(type, hrMod, mnMod, heatSun, coolSun, heatCloud, coolCloud, ramp);
        #endif
    }
    else{
      memorySettings();
    }
  }

void loadParameters(){
    if(EEPROM[0] != FIRST_BOOT){   //flag indicates if EEPROM has been yet configured
      for (int i = 0 ; i < EEPROM.length() ; i++) {
        EEPROM.update(i, 0);
      }
      EEPROMReset = DEFAULT_SETTINGS;
      rtc.setTime(HOUR_SET, MINUT_SET, SECOND_SET);
      rtc.setDate(DAY_SET, MONTH_SET, YEAR_SET);
    }
    #ifdef EXCEL_PROGRAMMER
      EEPROMReset = EXCEL_SETTINGS;
    #endif
    #ifdef COMPUTER_INTERFACE
      EEPROMReset = DEFAULT_SETTINGS;
    #endif
    //Define pinout for each devices
  #if ROLLUPS >= 1
    R1.initOutputs(ACT_HIGH, ROLLUP1_OPENING_PIN, ROLLUP1_CLOSING_PIN);
  #endif
  #if ROLLUPS == 2
    R2.initOutputs(ACT_HIGH, ROLLUP2_OPENING_PIN, ROLLUP2_CLOSING_PIN);
  #endif
  #if FANS >= 1
    F1.initOutput(ACT_HIGH, FAN1_PIN);
  #endif
  #if FANS == 2
    F2.initOutput(ACT_HIGH, FAN2_PIN);
  #endif
  #if HEATERS >= 1
    H1.initOutput(ACT_HIGH, HEATER1_PIN);
  #endif
  #if HEATERS == 2
    H2.initOutput(ACT_HIGH, HEATER2_PIN);
  #endif

    //If fresh start, set parameters from Greenhouse_parameters.h
    if(EEPROMReset == DEFAULT_SETTINGS){
      Serial.println("check1");
      defaultSettings();
    }
    else if(EEPROMReset == EXCEL_SETTINGS){
      excelSettings();
    }
    else{
      memorySettings();
    }

    EEPROM[0] = FIRST_BOOT;
    EEPROMReset = MEMORY_SETTINGS;
}

//***************************************************
//*********************EXPORTS**************************
//***************************************************



void exportData(){
    String counter = String(exportCounter+1);
    Serial.print("ROW,SET,");
    Serial.println(counter);
    Serial.print("DATA,DATE,TIME,");
    Serial.print(greenhouseTemperature.value());
    Serial.print(",");
    Serial.println(greenhouseHumidity.value());
    Serial.print("CELL,SET,G2,");
    Serial.println(exportCounter);
}


void updateExportParameters(){

  Serial.println("CELL,GET,F2");
  int tableDelay = Serial.parseInt();
  exportDelay = tableDelay;

  Serial.println("CELL,GET,G2");
  int counter = Serial.parseInt();
  exportCounter = counter;


}

void saveExportAndClear(){
  String date = rtc.getDateStr();
  String now = rtc.getTimeStr();
  String fileName = date +"." + now + ".datalogging";
  Serial.print("SAVEWORKBOOKAS,");
  Serial.println(fileName);
  exportCounter = 0;

  Serial.println("CLEARRANGE,A,2,D,10000");
  String delay = String(exportDelay);
  Serial.print("CELL,SET,F2,");
  Serial.println(delay);
  Serial.print("CELL,SET,G2,0");

}

void dataloggingToExcel(){
  if (exportTimer >= (exportDelay*1000)){

    checkFeedback(EXCEL_DATALOGGER_ID);
    if(connectionEstablished){
      updateExportParameters();
    }
    //counter havent started yet

    if(exportCounter == 0){
      if (connectionEstablished){
        exportCounter++;
        exportData();
      }
      else{
        exportCounter = 0;
      }
    }
    //counter have started
    else{
      if(exportCounter < maxLines){
        if(connectionEstablished){
          exportCounter++;
          exportData();
        }
      }
      else{
        if(connectionEstablished){
          saveExportAndClear();
        }
      }
    }
    exportTimer = 0;
  }
}
