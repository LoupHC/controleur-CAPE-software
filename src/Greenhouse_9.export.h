
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
