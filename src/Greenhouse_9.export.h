
//**************************************SD CARD***************
#include <SPI.h>
#include <SD.h>

const int chipSelect = 10;

File datalog;

//***************************************************
//*********************EXPORTS**************************
//***************************************************

void exportData(){
    String counter = String(exportCounter+1);
    Serial.print(F("ROW,SET,"));
    Serial.println(counter);
    Serial.print(F("DATA,DATE,TIME,"));
    Serial.print(greenhouseTemperature);
    Serial.print(F("),F("));
    Serial.println(greenhouseHumidity);
    Serial.print(F("CELL,SET,G2,"));
    Serial.println(exportCounter);
}

void exportDataToSD(){

  t = rtc.getTime();
  String dayy = String(t.date);
  String monthh = String(t.mon);
  String yearr = String(t.year);
  String datalogFile = dayy + monthh + yearr + ".txt";
  Serial.println(datalogFile);

  datalog = SD.open(datalogFile, FILE_WRITE);

  if (datalog) {
    Serial.print(greenhouse.hr());
    Serial.print(":");
    Serial.print(greenhouse.mn());
    Serial.print(":00,T:,");
    Serial.print(greenhouseTemperature);
    Serial.print(F(",C,HR:,"));
    Serial.print(greenhouseHumidity);
    Serial.print(F(",%,Rain:,"));
    Serial.print(totalRainfall);
    Serial.print(F(",mm,"));
    Serial.print(luxReading[0]);
    Serial.print(F(",lux,Rollup1:,"));
    Serial.print(R1.incrementCounter());
    Serial.print(F(",%,Rollup2:"));
    Serial.print(R1.incrementCounter());
    Serial.println(F(",%,"));
    datalog.print(greenhouse.hr());
    datalog.print(":");
    datalog.print(greenhouse.mn());
    datalog.print(":00,T:,");
    datalog.print(greenhouseTemperature);
    datalog.print(F(",C,HR:,"));
    datalog.print(greenhouseHumidity);
    datalog.print(F(",%,Rain:,"));
    datalog.print(totalRainfall);
    datalog.print(F(",mm,"));
    datalog.print(luxReading[0]);
    datalog.print(F(",lux,Rollup1:,"));
    datalog.print(R1.incrementCounter());
    datalog.print(F(",%,Rollup2:"));
    datalog.print(R1.incrementCounter());
    datalog.println(F(",%,"));
    datalog.close(); // close the file
  }

}



void readDataFromSDFile(char	*string){

  t = rtc.getTime();
  String dayy = String(t.date);
  String monthh = String(t.mon);
  String yearr = String(t.year);
  String datalogFile = dayy + monthh + yearr + ".txt";

  Serial.print("Initializing SD card...");

  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  datalog = SD.open(datalogFile);

  if (SD.exists(datalogFile)) {
    Serial.println("file exists.");
  }
  if (datalog) {/*
    while (datalog.available()) {
      Serial.write(datalog.read());
    }*/
    datalog.close();
  }
}

void updateExportParameters(){

  Serial.println(F("CELL,GET,F2"));
  int tableDelay =  get(F("B7"));
  exportDelay = tableDelay;

  Serial.println(F("CELL,GET,G2"));
  int counter =  get(F("B7"));
  exportCounter = counter;


}

void saveExportAndClear(){
  String date = rtc.getDateStr();
  String now = rtc.getTimeStr();
  String fileName = date +"." + now + ".datalogging";
  Serial.print(F("SAVEWORKBOOKAS,"));
  Serial.println(fileName);
  exportCounter = 0;

  Serial.println(F("CLEARRANGE,A,2,D,10000"));
  String delay = String(exportDelay);
  Serial.print(F("CELL,SET,F2,"));
  Serial.println(delay);
  Serial.print(F("CELL,SET,G2,0"));

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
void dataloggingToSD(){
  if (exportTimer >= (exportDelay*1000)){
        exportDataToSD();
        exportTimer = 0;
    }
}
