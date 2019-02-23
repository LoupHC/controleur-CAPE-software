
//#define DEBUG_CLOCK
  #include <OneWire.h>
  #include <DallasTemperature.h>
  #include <DS3231.h>
  #include <SHT1x.h>
  //Create DS18B20 object
  OneWire oneWire(DS18B20_DATA);
  DallasTemperature sensors(&oneWire);
  //Create SHT1X object
  SHT1x sht1x(SHT1X_DATA, SHT1X_CLOCK);
  //Create a RTC object
  DS3231  rtc(SDA, SCL);                // Init the DS3231 using the hardware interface
  Time  t;


unsigned long counter = 1;

<<<<<<< HEAD
int startMin = 0;
int startHour = 0;
int startDay = 1;
int startMonth = 1;
int startYear = 0;
=======
>>>>>>> 8635b916547bc6428a90a9dd528a5a01848050a7

void sensorBackup(){
  if(EEPROM.read(1) == 111){
    float emergencyTemp = EEPROM.read(2);
    greenhouseTemperature = emergencyTemp;
<<<<<<< HEAD
  }
  else{
    greenhouseTemperature = 20;
  }
  if(EEPROM.read(3) == 111){
    float emergencyHum = EEPROM.read(4);
    greenhouseHumidity = emergencyHum;
  }
  else{
=======
  }
  else{
    greenhouseTemperature = 20;
  }
  if(EEPROM.read(3) == 111){
    float emergencyHum = EEPROM.read(4);
    greenhouseHumidity = emergencyHum;
  }
  else{
>>>>>>> 8635b916547bc6428a90a9dd528a5a01848050a7
    greenhouseHumidity = 50;
  }
}

<<<<<<< HEAD
void testTime(){
    if(startMin > 59){
      startMin = 0;
      startHour++;
    }
    if(startHour > 23){
      startHour = 0;
      startDay++;
    }
    if(startDay > 30){
      startDay = 1;
      startMonth++;
    }
    if(startMonth > 12){
      startMonth = 1;
      startYear++;
    }
}

=======
>>>>>>> 8635b916547bc6428a90a9dd528a5a01848050a7
void getDateAndTime(){
  if(RTC == RTC_DS3231){

      t = rtc.getTime();
      rightNow[5] = t.year-2000;
      rightNow[4] = t.mon;
      rightNow[3] = t.date;
      rightNow[2] = t.hour;
      rightNow[1] = t.min;
      rightNow[0] = t.sec;
<<<<<<< HEAD
  }
  if(RTC == TEST_CLOCKF){

    rightNow[0] = 0;
    rightNow[1] = startMin;
    rightNow[2] = startHour;
    rightNow[3] = startDay;
    rightNow[4] = startMonth;
    rightNow[5] = startYear;

    startMin++;
    testTime();

  }
  if(RTC == TEST_CLOCKFF){

    rightNow[0] = 0;
    rightNow[1] = startMin;
    rightNow[2] = startHour;
    rightNow[3] = startDay;
    rightNow[4] = startMonth;
    rightNow[5] = startYear;

    startDay++;
    testTime();
=======
>>>>>>> 8635b916547bc6428a90a9dd528a5a01848050a7
  }


  #ifdef DEBUG_CLOCK
  for(int x = 0; x < 6; x++){
    Serial.print(rightNow[x]);
    Serial.print(":");
  }
  Serial.println("");
  #endif
}

void checkSensorFailure(float parameter,float measuredValue, float errorValue, int recoveryAddress, boolean * sensorFailure){
      if(measuredValue <= errorValue){
        byte lastRecordedValue;
        if(parameter < 0){
          lastRecordedValue = 0;
        }
        else{
          lastRecordedValue = parameter;
        }
        EEPROM.update(recoveryAddress, 111);
        EEPROM.update(recoveryAddress+1, lastRecordedValue);
        *sensorFailure = true;
      }
      else{
        *sensorFailure = false;
      }
}

void getGreenhouseTemp(){
  float temp;
  #if GH_TEMPERATURE == DS18B20
    sensors.requestTemperatures();
    temp = sensors.getTempCByIndex(0);

    checkSensorFailure(greenhouseTemperature, temp, -127.00,1,&sensorFailure);
  #endif
  #if GH_TEMPERATURE == SHT1X
    temp = sht1x.readTemperatureC();
    checkSensorFailure(greenhouseTemperature, temp, -40.00,1,&sensorFailure);
  #endif
  if(!sensorFailure){
    greenhouseTemperature = temp;
  }
}

void getGreenhouseHum(){
  float hum;
  #if GH_TEMPERATURE == SHT1X
    hum = sht1x.readHumidity();
    checkSensorFailure(greenhouseHumidity, hum, 0,3,&sensorFailure);
  #endif
  if(!sensorFailure){
    greenhouseHumidity = hum;
  }
}

void getRain(){
  #if RAIN_SENSOR == HYDREON_RG11
    if(digitalRead(RAIN_SWITCH) == LOW){
      rain = true;
    }
    else{
      rain = false;
    }
  #endif
}
