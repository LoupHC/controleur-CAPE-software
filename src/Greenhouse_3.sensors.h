
#ifdef DS18B20
  #include <OneWire.h>
  #include <DallasTemperature.h>
  //Create DS18B20 object
  OneWire oneWire(DS18B20_PIN);
  DallasTemperature sensors(&oneWire);
#endif

#ifdef SHT1X
  #include <SHT1x.h>
  SHT1x sht1x(SHT1X_DATA, SHT1X_CLOCK);
#endif

#ifdef DHT11
  #include <DHT.h>
  //Create DHT object
  DHT dht(DHT11_PIN, DHT11);
#endif

#ifdef DHT22
  #include <DHT.h>
  //Create DHT object
  DHT dht(DHT22_PIN, DHT22);
#endif

#ifdef CLOCK_DS3231
  #include <DS3231.h>
  //Create a RTC object
  DS3231  rtc(SDA, SCL);                // Init the DS3231 using the hardware interface
  Time  t;
#endif

#ifdef DHT12
  #include <DHT12.h>
  //Create DHT object
  DHT12 DHT;
#endif

unsigned long counter = 1;

void getDateAndTime(){

  for(int x = 0; x < 6; x++){
    rightNow[x].setLimits(0,60);
  }

  #ifdef CLOCK_DS3231
    t = rtc.getTime();
    rightNow[5].setValue(t.year-2000);
    rightNow[4].setValue(t.mon);
    rightNow[3].setValue(t.date);
    rightNow[2].setValue(t.hour);
    rightNow[1].setValue(t.min);
    rightNow[0].setValue(t.sec);
  #endif


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


void getGreenhouseTemp(){
  #ifdef DS18B20
    sensors.requestTemperatures();
    float temp = sensors.getTempCByIndex(0);

    if((temp <= -127.00)||(temp >= 85.00)){
      EEPROM.update(1, 111);
      EEPROM.update(2, (byte)greenhouseTemperature.value());
      greenhouseTemperature.setValue(greenhouseTemperature.value());
      sensorFailure = true;
        greenhouse.alarmBlast();
        delay(500);
        greenhouse.stopAlarm();
        delay(1000);
      Serial.println("sensor fault");
    }
    else{
      greenhouseTemperature.setValue(temp);
      greenhouseTemperature.updateLastValue();
      sensorFailure = false;
    }
  #endif

  #ifdef SHT1X
    float temp = sht1x.readTemperatureC();
    //Serial.println(temp);

    if(temp <= -30.00){
      EEPROM.update(1, 111);
      EEPROM.update(2, (byte)greenhouseTemperature.value());
      greenhouseTemperature.setValue(greenhouseTemperature.value());
      sensorFailure = true;
        greenhouse.alarmBlast();
        delay(500);
        greenhouse.stopAlarm();
        delay(1000);
      Serial.println("sensor fault");
    }
    else{
      greenhouseTemperature.setValue(temp);
      greenhouseTemperature.updateLastValue();
      sensorFailure = false;
    }
  #endif
  #ifdef DHT12
    DHT.read();
    greenhouseTemperature.setValue(DHT.temperature);
  #endif

}

void getGreenhouseHum(){

  #ifdef SHT1X
    float hum = sht1x.readHumidity();
    greenhouseHumidity.setValue(hum);
    //Serial.println(hum);
  #endif

  #ifdef DHT
    greenhouseHumidity.setValue((float)dht.readHumidity());
  #endif
  #ifdef DHT12
    DHT.read();
    greenhouseHumidity.setValue(DHT.humidity);
  #endif
}
