
//**************************************SD CARD***************
#include <SPI.h>
#include <SD.h>
//Otomate V.2
const int chipSelect = 53;

File datalog;
File parameters;
int lastMinutRecord;
int lastHourRecord;

int minutCount = 0;
const int exportDelayForDailyRecord = 5;//minuts


//***************************************************
//*********************EXPORTS**************************
//***************************************************

void exportData(){
    String counter = String(exportCounter+1);
    Serial.print(F("ROW,SET,"));
    Serial.println(counter);
    Serial.print(F("DATA,DATE,TIME,"));
    Serial.print(greenhouseTemperature.value());
    Serial.print(F("),F("));
    Serial.println(greenhouseHumidity.value());
    Serial.print(F("CELL,SET,G2,"));
    Serial.println(exportCounter);
}

String dailyRecord(){

    t = rtc.getTime();

    String day_s = String(t.date);
    String fullday_s;

    if(t.date<10){
      fullday_s = String('0'+day_s);
    }
    else{
      fullday_s = String(day_s);
    }

    String month_s = String(t.mon);
    String fullmonth_s;

    if(t.mon<10){
      fullmonth_s = String('0'+month_s);
    }
    else{
      fullmonth_s = String(month_s);
    }

    String year_s = String(t.year);
    String fileName = year_s + fullmonth_s + fullday_s+".txt";

    return fileName;
}

String monthlyRecord(){

    t = rtc.getTime();

    String month_s = String(t.mon);
    String fullmonth_s;

    if(t.mon<10){
      fullmonth_s = String('0'+month_s);
    }
    else{
      fullmonth_s = String(month_s);
    }

    String year_s = String(t.year);
    String fileName = year_s + fullmonth_s +".txt";

    return fileName;
}


void exportData(String record){
  datalog = SD.open(record, FILE_WRITE);
  if (SD.exists(record)) {
    Serial.println(record);
  }
  else{
    Serial.println(F("Error writing to file"));
  }
  if (datalog) {
    datalog.print(greenhouse.hr());
    datalog.print(F(":"));
    datalog.print(greenhouse.mn());
    datalog.print(F(":00,"));
    datalog.print(greenhouseTemperature.value());
    datalog.print(F(","));
    datalog.print(greenhouseHumidity.value());
    datalog.print(F(","));
    datalog.print(greenhouseHumidity.absolute(greenhouseTemperature.value()));
    datalog.print(F(","));
    datalog.print(outsideTemperature.value());
    datalog.print(F(","));
    datalog.print(outsideHumidity.value());
    datalog.print(F(","));
    datalog.print(outsideHumidity.absolute(outsideTemperature.value()));
    datalog.print(F(","));
    datalog.print(rain);
    datalog.print(F(","));
    datalog.print(totalRainfall);
    datalog.print(F(","));
    datalog.print(windSpeed);
    datalog.print(F(","));
    datalog.print(radiation.lux());
    datalog.print(F(","));
    datalog.print(radiation.dayJoules());
    datalog.print(F(","));
    datalog.print(greenhouse.weather());
    datalog.print(F(","));
    datalog.print(greenhouse.heatingTemp());
    datalog.print(F(","));
    datalog.print(greenhouse.coolingTemp());
    datalog.print(F(","));
    datalog.print(R1.incrementCounter());
    datalog.print(F(","));
    datalog.print(R2.incrementCounter());
    datalog.print(F(","));
    datalog.print(R3.incrementCounter());
    datalog.print(F(","));
    datalog.print(D1.isOn());
    datalog.print(F(","));
    datalog.print(D2.isOn());
    datalog.print(F(","));
    datalog.print(D3.isOn());
    datalog.print(F(","));
    datalog.print(D4.isOn());
    datalog.print(F(","));
    datalog.print(D5.isOn());
    datalog.print(F(","));
    datalog.println(D6.isOn());
  }
  datalog.close(); // close the file

}

void exportParametersToSD(){
  parameters = SD.open("PARAMETERS", FILE_WRITE);
  if (SD.exists("PARAMETERS")) {
    Serial.println("PARAMETERS file exists");
  }
  else{
    Serial.println(F("Error writing to file"));
  }
  if (parameters) {
    for(int x = 0; x < MAX_ROLLUPS;x++){

      parameters.println(greenhouse.rollup[x].hyst.value());
      parameters.println(greenhouse.rollup[x].rotationUp.value());
      parameters.println(greenhouse.rollup[x].rotationDown.value());
      parameters.println(greenhouse.rollup[x].pause.value());

      for(int y = 0; y < MAX_STAGES;y++){
        parameters.println(greenhouse.rollup[x].stage[y].mod.value());
        parameters.println(greenhouse.rollup[x].stage[y].target.value());
      }
      parameters.println(greenhouse.rollup[x].stages.value());
      parameters.println(greenhouse.rollup[x].enabled.value());
      parameters.println(greenhouse.rollup[x].lock.value());
      parameters.println(greenhouse.rollup[x].lockTarget.value());
      parameters.println(greenhouse.rollup[x].currentLimit.value());

      for(int y = 0; y < MAX_CLOCK_OV;y++){
        parameters.println(greenhouse.rollup[x].clockOv[y].ID.value());
        parameters.println(greenhouse.rollup[x].clockOv[y].priority.value());
        parameters.println(greenhouse.rollup[x].clockOv[y].enabled.value());
        parameters.println(greenhouse.rollup[x].clockOv[y].target.value());
        parameters.println(greenhouse.rollup[x].clockOv[y].hrStart.value());
        parameters.println(greenhouse.rollup[x].clockOv[y].mnStart.value());
        parameters.println(greenhouse.rollup[x].clockOv[y].hrStop.value());
        parameters.println(greenhouse.rollup[x].clockOv[y].mnStop.value());
        parameters.println(greenhouse.rollup[x].clockOv[y].pulseOff.value());
        parameters.println(greenhouse.rollup[x].clockOv[y].pulseOn.value());
        parameters.println(greenhouse.rollup[x].clockOv[y].condition.value());
        parameters.println(greenhouse.rollup[x].clockOv[y].ovType.value());
      }
      for(int y = 0; y < MAX_BOOL_OV;y++){
        parameters.println(greenhouse.rollup[x].boolOv[y].ID.value());
        parameters.println(greenhouse.rollup[x].boolOv [y].priority.value());
        parameters.println(greenhouse.rollup[x].boolOv [y].enabled.value());
        parameters.println(greenhouse.rollup[x].boolOv [y].target.value());
        parameters.println(greenhouse.rollup[x].boolOv [y].pulseOff.value());
        parameters.println(greenhouse.rollup[x].boolOv [y].pulseOn.value());
      }
      for(int y = 0; y < MAX_FLOAT_OV;y++){
        parameters.println(greenhouse.rollup[x].floatOv[y].ID.value());
        parameters.println(greenhouse.rollup[x].floatOv [y].priority.value());
        parameters.println(greenhouse.rollup[x].floatOv [y].enabled.value());
        parameters.println(greenhouse.rollup[x].floatOv [y].target.value());
        parameters.println(greenhouse.rollup[x].floatOv [y].comparator.value());
        parameters.println(greenhouse.rollup[x].floatOv [y].floatVar.value());
        parameters.println(greenhouse.rollup[x].floatOv [y].hyst.value());
        parameters.println(greenhouse.rollup[x].floatOv [y].pulseOff.value());
        parameters.println(greenhouse.rollup[x].floatOv [y].pulseOn.value());
      }
    }

    for(int x = 0; x < MAX_DEVICES;x++){
      parameters.println(greenhouse.device[x].mod.value());
      parameters.println(greenhouse.device[x].hyst.value());
      parameters.println(greenhouse.device[x].type.value());
      parameters.println(greenhouse.device[x].enabled.value());
      parameters.println(greenhouse.device[x].lock.value());
      parameters.println(greenhouse.device[x].lockTarget.value());
      parameters.println(greenhouse.device[x].pulse.value());
      parameters.println(greenhouse.device[x].tensiometer.value());


      for(int y = 0; y < MAX_CLOCK_OV;y++){
        parameters.println(greenhouse.device[x].clockOv[y].ID.value());
        parameters.println(greenhouse.device[x].clockOv[y].priority.value());
        parameters.println(greenhouse.device[x].clockOv[y].enabled.value());
        parameters.println(greenhouse.device[x].clockOv[y].target.value());
        parameters.println(greenhouse.device[x].clockOv[y].hrStart.value());
        parameters.println(greenhouse.device[x].clockOv[y].mnStart.value());
        parameters.println(greenhouse.device[x].clockOv[y].hrStop.value());
        parameters.println(greenhouse.device[x].clockOv[y].mnStop.value());
        parameters.println(greenhouse.device[x].clockOv[y].pulseOff.value());
        parameters.println(greenhouse.device[x].clockOv[y].pulseOn.value());
        parameters.println(greenhouse.device[x].clockOv[y].condition.value());
        parameters.println(greenhouse.device[x].clockOv[y].ovType.value());
      }

      for(int y = 0; y < MAX_BOOL_OV;y++){
        parameters.println(greenhouse.device[x].boolOv[y].ID.value());
        parameters.println(greenhouse.device[x].boolOv [y].priority.value());
        parameters.println(greenhouse.device[x].boolOv [y].enabled.value());
        parameters.println(greenhouse.device[x].boolOv [y].target.value());
        parameters.println(greenhouse.device[x].boolOv [y].pulseOff.value());
        parameters.println(greenhouse.device[x].boolOv [y].pulseOn.value());
      }

      for(int y = 0; y < MAX_FLOAT_OV;y++){
        parameters.println(greenhouse.device[x].floatOv[y].ID.value());
        parameters.println(greenhouse.device[x].floatOv [y].priority.value());
        parameters.println(greenhouse.device[x].floatOv [y].enabled.value());
        parameters.println(greenhouse.device[x].floatOv [y].target.value());
        parameters.println(greenhouse.device[x].floatOv [y].comparator.value());
        parameters.println(greenhouse.device[x].floatOv [y].floatVar.value());
        parameters.println(greenhouse.device[x].floatOv [y].hyst.value());
        parameters.println(greenhouse.device[x].floatOv [y].pulseOff.value());
        parameters.println(greenhouse.device[x].floatOv [y].pulseOn.value());
      }
    }
    for(int x = 0; x < MAX_TIMEPOINTS;x++){
      parameters.println(greenhouse.timepoint[x].type.value());
      parameters.println(greenhouse.timepoint[x].mnMod.value());
      parameters.println(greenhouse.timepoint[x].hrMod.value());
      parameters.println(greenhouse.timepoint[x].coolingTemp.value());
      parameters.println(greenhouse.timepoint[x].heatingTemp.value());
      parameters.println(greenhouse.timepoint[x].coolingTempCloud.value());
      parameters.println(greenhouse.timepoint[x].heatingTempCloud.value());
      parameters.println(greenhouse.timepoint[x].ramping.value());
    }
    parameters.println(greenhouse.timezone.value());
    parameters.println(greenhouse.latitude.value());
    parameters.println(greenhouse.longitude.value());
    parameters.println(greenhouse.timepoints.value());
    parameters.println(greenhouse.rollups.value());
    parameters.println(greenhouse.devices.value());
    parameters.println(greenhouse.daynight.value());
    parameters.println(greenhouse.weatheradjust.value());
    parameters.println(greenhouse.dif.value());
    parameters.println(greenhouse.prenight.value());
    parameters.println(greenhouse.weatherP.value());
    parameters.println(greenhouse.insideTempSensor.value());
    parameters.println(greenhouse.outsideTempSensor.value());
    parameters.println(greenhouse.luxMeter.value());
    parameters.println(greenhouse.rainSensor.value());
    parameters.println(greenhouse.anemometer.value());
    for(int y = 0; y < MAX_TENSIOMETERS;y++){
      parameters.println(greenhouse.tensiometer[y].value());
    }
    parameters.println(greenhouse.autoWeather.value());
    parameters.println(greenhouse.lowTempAlarm.value());
    parameters.println(greenhouse.highTempAlarm.value());
    parameters.println(greenhouse.minTemp.value());
    parameters.println(greenhouse.maxTemp.value());
    parameters.println(greenhouse.autoWeather.value());
    parameters.println(greenhouse.alarmEnabled.value());
    parameters.println(greenhouse.energySavingMode.value());
  }
  parameters.close();
}

String recordHeader(){
  return "Time,InTemp,InHum,InAbs,OutTemp,OutHum,OutAbs,isRaining,Rainfall,Windspeed,Lux,W/m²,Weather ratio,CoolingT,HeatingT,R1,R2,R3,D1,D2,D3";
}

void startRecordingToSD(){


  Serial.println(F("Initializing SD card..."));

  if (!SD.begin(chipSelect)) {
    Serial.println(F("Card failed, or not present"));
    return;
  }

  //daily record file
  if (!SD.exists(dailyRecord())) {
    Serial.println(F("Creating daily record file"));
    datalog = SD.open(dailyRecord(), FILE_WRITE);
    datalog.println(recordHeader());
    datalog.close();
  }
  if (SD.exists(dailyRecord())) {
    Serial.println(F("Daily record file exists"));
  }
  else{
    Serial.println(F("error creating file"));
  }

  //monthly record file

  if (!SD.exists(monthlyRecord())) {
    Serial.println(F("Creating monthly record file"));
    datalog = SD.open(monthlyRecord(), FILE_WRITE);
    datalog.println(recordHeader());
    datalog.close();
  }
  if (SD.exists(monthlyRecord())) {
    Serial.println(F("Monthly record file exists"));
  }
  else{
    Serial.println(F("error creating file"));
  }
  /*//Read file
    if (datalog) {
    while (datalog.available()) {
      Serial.write(datalog.read());
    }
    datalog.close();
  }*/
}

void dataloggingToSD(){

  byte checkMin;
  //new record every X minuts
  if(greenhouse.mn() != lastMinutRecord){
    lastMinutRecord = greenhouse.mn();
    minutCount++;
  }
  if (minutCount >= exportDelayForDailyRecord){
    exportData(dailyRecord());
    minutCount = 0;
  }
  //new record every hour

  if (greenhouse.hr() != lastHourRecord){
      exportData(monthlyRecord());
      lastHourRecord = greenhouse.hr();
    }

  if(Parameter::updated()){
    exportParametersToSD();
    Parameter::acknowledgeUpdate();
  }
}
