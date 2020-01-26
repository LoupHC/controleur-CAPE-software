


#define TEST_CLOCKF         201//RTC
#define TEST_CLOCKFF        202//RTC
#define TEST_TEMP           105


//#define DEBUG_CLOCK
  #include <OneWire.h>
  #include <DallasTemperature.h>
  #include <DS3231.h>
  #include <SHT1x.h>
  #include <GreenhouseLib_sensors.h>
  //Create DS18B20 objects
  OneWire oneWire1(TEMP1_CLOCK);
  DallasTemperature ds18b20_in(&oneWire1);
  OneWire oneWire2(TEMP2_CLOCK);
  DallasTemperature ds18b20_out(&oneWire2);
  //Create SHT1X objects
  SHT1x sht1x_in(TEMP1_DATA, TEMP1_CLOCK);
  SHT1x sht1x_out(TEMP2_DATA, TEMP2_CLOCK);
  //Create a RTC object
  DS3231  rtc(SDA, SCL);                // Init the DS3231 using the hardware interface
  Time  t;

  byte clockTest = 0;
  byte tempSensorTest = 0;
  byte humSensorTest = 0;

  //Time array
  byte rightNow[6];
  //sensors

  Temperature greenhouseTemperature;
  Temperature outsideTemperature;
  Humidity greenhouseHumidity;
  Humidity outsideHumidity;
  Sensor soilMoisture[3];

  Current motorOne;
  Current motorTwo;

  Lux radiation;

  boolean rain = false;
  float rainfall24h = 0;
  volatile int interruptWindSpeed = 0;
  unsigned short windSpeed = 0;


  //lux;
  unsigned long averageLux;
  unsigned long averageDailyLux;
  int mV;
  unsigned long luxReading[10];
  unsigned long hourlyLuxReading[12];
  byte luxSmoothingArray = 24;
  unsigned long cloudLux = 20000;
  unsigned long sunLux = 50000;

//rain bucket
#define Bucket_Size 0.26 // bucket size to trigger tip count
#define RG11_Pin 3 // digital pin RG11 connected to

volatile unsigned long tipCount = 0; // bucket tip counter used in interrupt routine
volatile unsigned long contactTime; // Timer to manage any contact bounce in interrupt routine
volatile float totalRainfall = 0; // total amount of rainfall detected
float rainSample = 0;
float rainSetpoint = 0.5;
unsigned long rainSampleDelay = 120000;
elapsedMillis rainSampleCounter;


boolean sensorFailure = false;


unsigned long counter = 1;

int startMin = 0;
int startHour = 3;
int startDay = 1;
int startMonth = 1;
int startYear = 0;

void setRollupsInTestMode(){
  if(clockTest != 0){
      greenhouse.testRollups(true);
  }
  else{
    greenhouse.testRollups(false);
  }
}
void sensorBackup(){
  if(EEPROM.read(1) == 111){
    float emergencyTemp = EEPROM.read(2);
    greenhouseTemperature.registerValue(emergencyTemp);
  }
  else{
    greenhouseTemperature.registerValue(20);
  }
  if(EEPROM.read(3) == 111){
    float emergencyHum = EEPROM.read(4);
    greenhouseHumidity.registerValue(emergencyHum);
  }
  else{
    greenhouseHumidity.registerValue(50);
  }
}
void resetLuxReadings(){
  for (int x = 0; x < 10; x++) {
    luxReading[x] = OFF_VAL;
  }

}
void resetHourlyLuxReadings(){
  for (int x = 0; x < 10; x++) {
    hourlyLuxReading[x] = OFF_VAL;
  }
}

// Interrupt handler routine that is triggered when the rg-11 detects rain
void isr_rg() {
  if((millis() - contactTime) > 15 ) { // debounce of sensor signal
  tipCount++;
  totalRainfall = tipCount * Bucket_Size;
  contactTime = millis();
  }
}

void getRain(){
  //Normally open contact rain sensor
  if(greenhouse.rainSensor.value() == NO_CONTACT){
    if(digitalRead(RAIN_SWITCH) == LOW){
      rain = true;
    }
    else{
      rain = false;
    }
  }
  //Normally close contact rain sensor
  else if(greenhouse.rainSensor.value() == NC_CONTACT){
    if(digitalRead(RAIN_SWITCH) == HIGH){
      rain = true;
    }
    else{
      rain = false;
    }
  }
  //RG11/Sparkfun - 0.01'' bucket rain sensor
  else if(greenhouse.rainSensor.value() == RG11_BUCKET){
    //check precipitations in the last 2 minuts sample
    //reset rainSample
    if(rainSampleCounter > rainSampleDelay){
      if(totalRainfall > rainSample + rainSetpoint){
        rain = true;
      }
      else{
        rain = false;
      }
      rainSample = totalRainfall;
      rainSampleCounter = 0;
    }
  }
  else{
    rain = false;
    totalRainfall = false;
  }
}

#define switch_delay_to_wind_speed 2.4 // 2.4km/h = 1 trigger/second
volatile unsigned long contactTimeAne; // Timer to manage any contact bounce in interrupt routine for anemometer
elapsedMillis AneSampleCounter; //Anemometer switch counter; elapsed time in millisecond since last trigger

// Interrupt handler routine that is triggered when the anemometer make one rotation
void isr_ane() {
  if((millis() - contactTimeAne) > 15 ) { // debounce of sensor signal
    interruptWindSpeed = switch_delay_to_wind_speed*1000/AneSampleCounter;
  if(interruptWindSpeed < 1){
    interruptWindSpeed = 0; //Avoid low speed when no wind, so if windspeed lower than 1km/h than reset to 0km/h
  }
  AneSampleCounter = 0;
  contactTimeAne = millis();
  }
}

void getWind(){
  if(greenhouse.anemometer.value() == ANALOG_WIND){
    //0-5V (0-30 m/s) analog wind sensor
    int aRead = analogRead(WIND_ANALOG_SENSOR);
    float calculation = (float)108/(float)1023*(float)aRead; //108km/h = 30m/s
    windSpeed = calculation;
  }
  else if(greenhouse.anemometer.value() == SPARKFUN_WIND){
    windSpeed = (unsigned short)interruptWindSpeed;
  }
  else{
    windSpeed = 0;
  }

}



void testTime(){

    if(startMin > 59){
      startMin = 0;
      startHour++;
    }
    if(startHour > 23){
      startHour = 0;
      clockTest = 0;
    }
    if(startDay > 30){
      startDay = 1;
      startMonth++;
    }
    if(startMonth > 12){
      startMonth = 1;
      clockTest = 0;
    }
}

void getDateAndTime(){
  setRollupsInTestMode();
  if(clockTest == TEST_CLOCKF){
    t = rtc.getTime();

    rightNow[0] = 0;
    rightNow[1] = startMin;
    rightNow[2] = startHour;
    rightNow[3] = t.date;
    rightNow[4] = t.mon;
    rightNow[5] = t.year-2000;

    startMin+=2;
    testTime();

  }
  else if(clockTest == TEST_CLOCKFF){

    rightNow[0] = 0;
    rightNow[1] = startMin;
    rightNow[2] = startHour;
    rightNow[3] = startDay;
    rightNow[4] = startMonth;
    rightNow[5] = startYear;

    startDay++;
    testTime();

  }
  else{

      t = rtc.getTime();
      rightNow[5] = t.year-2000;
      rightNow[4] = t.mon;
      rightNow[3] = t.date;
      rightNow[2] = t.hour;
      rightNow[1] = t.min;
      rightNow[0] = t.sec;
  }


  #ifdef DEBUG_CLOCK
  for(int x = 0; x < 6; x++){
    Serial.print(rightNow[x]);
    Serial.print(":");
  }
  Serial.println("");
  #endif
}

boolean checkSensorFailure(float parameter,float measuredValue, float minValue, float maxValue, int recoveryAddress){
      if((measuredValue <= minValue)||(measuredValue >= maxValue)){
        byte lastRecordedValue;
        if(parameter < 0){
          lastRecordedValue = 0;
        }
        else{
          lastRecordedValue = parameter;
        }
        EEPROM.update(recoveryAddress, 111);
        EEPROM.update(recoveryAddress+1, lastRecordedValue);
        Serial.println(F("failure!"));
        Serial.println(measuredValue);
        return true;
      }
      else{
        return false;
      }
}


boolean droppingTest = false;
boolean risingTest = false;
float startTempTest = 10.0;
float mockGreenhouseTemp = 10.0;
float startHumTest = 80.0;
float mockGreenhouseHum = 80.0;


void testTemp(){
  greenhouseTemperature.registerValue(mockGreenhouseTemp);
}
void testHum(){
  greenhouseHumidity.registerValue(mockGreenhouseHum);
}


byte tempFailTest = 0;

void getGreenhouseTemp(){
/*//test T24h
  for (int x = 0; x < 24; x++){
    if(greenhouseTemperature.hourAverage(x) != OFF_VAL){
      Serial.print(F("["));
      Serial.print(greenhouseTemperature.hourAverage(x));
      Serial.print(F("]"));
    }
  }*/
  setRollupsInTestMode();
  if(tempSensorTest == TEST_TEMP){
    testTemp();
  }
  else{
    float temp;

    if(greenhouse.insideTempSensor.value() == DS18B20_TEMP){
      ds18b20_in.requestTemperatures();
      temp = ds18b20_in.getTempCByIndex(0);

      if(checkSensorFailure(greenhouseTemperature.value(), temp, -127.00,60.00,1)){
        tempFailTest++;
      }
      else{
        tempFailTest = 0;
      }

    }
    else if(greenhouse.insideTempSensor.value() == SHT1X_TEMP){
      temp = sht1x_in.readTemperatureC();
      if(checkSensorFailure(greenhouseTemperature.value(), temp, -40.00,60.00,1)){
        tempFailTest++;
      }
      else{
        tempFailTest = 0;
      }
    }
    if(tempFailTest == 0){
      greenhouseTemperature.registerValue(temp);
      sensorFailure = false;
    }
    else if (tempFailTest == 20){
      sensorFailure = true;
    }
  }
}

float absoluteHumidity(float temperature, float humidity){
  //in g/m3
  return ((6.112 * pow(2.71828,((17.67 * temperature)/(temperature + 243.5))) * humidity * 2.1674) / (273.15 + temperature));
}

byte humFailTest = 0;

void getGreenhouseHum(){
  setRollupsInTestMode();

  if(humSensorTest == TEST_TEMP){
    testHum();
  }
  else{

    float hum;
    if(greenhouse.insideTempSensor.value() == SHT1X_TEMP){
      hum = sht1x_in.readHumidity();

      if(checkSensorFailure(greenhouseHumidity.value(), hum, -1.00,101.00,2)){
        humFailTest++;
      }
      else{
        humFailTest = 0;
      }
    }

    if(humFailTest == 0){
      greenhouseHumidity.registerValue(hum);
    }
  }
}

void getOutsideTemp(){
    float temp;

    if(greenhouse.outsideTempSensor.value() == DS18B20_TEMP){
      ds18b20_out.requestTemperatures();
      outsideTemperature.registerValue(ds18b20_out.getTempCByIndex(0));
    }
    else if(greenhouse.outsideTempSensor.value() == SHT1X_TEMP){
      outsideTemperature.registerValue(sht1x_out.readTemperatureC());
    }
    else{
      outsideTemperature.registerValue(OFF_VAL);
    }
}

void getOutsideHum(){
  if(greenhouse.outsideTempSensor.value() == SHT1X_TEMP){
    outsideHumidity.registerValue(sht1x_out.readHumidity());
  }
  else{
    outsideHumidity.registerValue(0);
  }
}

void getLux(){

  int aRead = analogRead(LIGHT_SENSOR);
  mV = 5000/1023*aRead;
  unsigned long nowLux = 200000/1023*aRead;

  radiation.registerLux(nowLux);/*
  Serial.println(F("New record!"));
  Serial.print(F("Average;"));
  Serial.print(radiation.luxHourAverage(greenhouse.hr()));
  Serial.print(F(";Average24h;"));
  Serial.print(radiation.averageDailyLux());
  Serial.print(F(";Average until now;"));
  Serial.print(radiation.averageLuxUntilNow());
  Serial.print(F(";Weather ratio;"));
  Serial.println(radiation.autoWeatherRatio());*/
}

void getSoilMoisture(){

  int aRead = analogRead(SOIL_MOISTURE1);
  unsigned long kPa = 80/1023*aRead;
  soilMoisture[0].registerValue(kPa);


  aRead = analogRead(SOIL_MOISTURE2);
  kPa = 80/1023*aRead;
  soilMoisture[1].registerValue(kPa);


  aRead = analogRead(SOIL_MOISTURE3);
  kPa = 80/1023*aRead;
  soilMoisture[2].registerValue(kPa);
}

void autoWeather(){
  if ((greenhouse.autoWeather.value() == true)&&(greenhouse.weatheradjust.value() == true)){
    greenhouse.weatherP.setValue(radiation.autoWeatherRatio());
  }
}

void getOnTime(){
  D1.onTime.record(greenhouse.hr(), greenhouse.mn(), D1.isOn());
  D2.onTime.record(greenhouse.hr(), greenhouse.mn(), D2.isOn());
  D3.onTime.record(greenhouse.hr(), greenhouse.mn(), D3.isOn());
  D4.onTime.record(greenhouse.hr(), greenhouse.mn(), D4.isOn());
  D5.onTime.record(greenhouse.hr(), greenhouse.mn(), D5.isOn());
}

void initSensors(){
  //last recorded value if probe doesnt reply back at first cycle
    sensorBackup();
  //start communication with temp probe
  if(greenhouse.insideTempSensor.value() == DS18B20_TEMP){
    ds18b20_in.begin();
    ds18b20_in.setResolution(12);
  }
  else if(greenhouse.insideTempSensor.value() == SHT1X_TEMP){
    sht1x_in.initSensor();
  }

  if(greenhouse.outsideTempSensor.value() == DS18B20_TEMP){
    ds18b20_out.begin();
    ds18b20_out.setResolution(12);
  }
  else if(greenhouse.outsideTempSensor.value() == SHT1X_TEMP){
    sht1x_out.initSensor();
  }
  //start communication with clock
    rtc.begin();
  //rain sensor
    pinMode(RAIN_SWITCH, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(RAIN_SWITCH), isr_rg, FALLING);
    pinMode(WIND_DIGITAL_SENSOR, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(WIND_DIGITAL_SENSOR), isr_ane, FALLING);
    sei();// Enable Interrupts

    greenhouseTemperature.clearRecords();


}
