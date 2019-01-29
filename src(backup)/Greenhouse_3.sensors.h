
//#define DEBUG_CLOCK

#if GH_TEMPERATURE == DS18B20 || OUT_TEMPERATURE == DS18B20
  #include <OneWire.h>
  #include <DallasTemperature.h>
  //Create DS18B20 object
  OneWire oneWire(DS18B20_PIN);
  DallasTemperature sensors(&oneWire);
#endif

#if GH_TEMPERATURE == SHT1X || OUT_TEMPERATURE == SHT1X || GH_HUMIDITY == SHT1X || OUT_HUMIDITY == SHT1X
  #include <SHT1x.h>
  SHT1x sht1x(SHT1X_DATA, SHT1X_CLOCK);
#endif

#if GH_TEMPERATURE == DHT11 || OUT_TEMPERATURE == DHT11 || GH_HUMIDITY == DHT11 || OUT_HUMIDITY == DHT11
  #include <DHT.h>
  //Create DHT object
  DHT dht(DHT11_PIN, DHT11);
#endif

#if GH_TEMPERATURE == DHT22 || OUT_TEMPERATURE == DHT22 || GH_HUMIDITY == DHT22 || OUT_HUMIDITY == DHT22
  #include <DHT.h>
  //Create DHT object
  DHT dht(DHT22_PIN, DHT22);
#endif

#if RTC == RTC_DS3231
  #include <DS3231.h>
  //Create a RTC object
  DS3231  rtc(SDA, SCL);                // Init the DS3231 using the hardware interface
  Time  t;
#endif


unsigned long counter = 1;


void getDateAndTime(){
  if(RTC == RTC_DS3231){

      t = rtc.getTime();
      rightNow[5].setValue(t.year-2000);
      rightNow[4].setValue(t.mon);
      rightNow[3].setValue(t.date);
      rightNow[2].setValue(t.hour);
      rightNow[1].setValue(t.min);
      rightNow[0].setValue(t.sec);
  }


  for(int x = 0; x < 6; x++){
    rightNowValue[x] = rightNow[x].value();
  }

  #ifdef DEBUG_CLOCK
  for(int x = 0; x < 6; x++){
    Serial.print(rightNow[x].value());
    Serial.print(":");
  }
  Serial.println("");
  #endif
}

void checkSensorFailure(floatParameter parameter,float measuredValue, float errorValue, int recoveryAddress, boolean * sensorFailure){
      if(measuredValue <= errorValue){
        byte lastRecordedValue;
        if(parameter.value() < 0){
          lastRecordedValue = 0;
        }
        else{
          lastRecordedValue = (byte)parameter.value();
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
    greenhouseTemperature.setValue(temp);
    greenhouseTemperature.updateLastValue();
  }
}

void getGreenhouseHum(){
  float hum;
  #if GH_TEMPERATURE == SHT1X
    hum = sht1x.readHumidity();
    checkSensorFailure(greenhouseHumidity, hum, 0,3,&sensorFailure);
  #endif
  if(!sensorFailure){
    greenhouseHumidity.setValue(hum);
  }
}

void getRain(){
  #if RAIN_SENSOR == HYDREON_RG11
    if(digitalRead(RAIN_SWITCH) == LOW){
      rain.setValue(true);
    }
    else{
      rain.setValue(false);
    }
  #endif
}
