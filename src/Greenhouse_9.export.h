
//**************************************SD CARD***************
#include <SPI.h>
#include <SD.h>
//Otomate V.2
const int chipSelect = 53;

File datalog;
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
    datalog.print(radiation.wattPerMeterSquare());
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
    datalog.println(D3.isOn());
  }
  datalog.close(); // close the file

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
}
