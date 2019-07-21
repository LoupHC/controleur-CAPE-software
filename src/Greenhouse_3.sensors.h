


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

  //Time array
  byte rightNow[6];
  //sensors

  Temperature greenhouseTemperature;
  Temperature outsideTemperature;
  Humidity greenhouseHumidity;
  Humidity outsideHumidity;

  Current motorOne;
  Current motorTwo;

  /*
  float greenhouseTemperature;
  float greenhouseTemperature24h = 0;
  float greenhouseTemperature72h = 0;

  float greenhouseHumidity = 0;
  float greenhouseAbsoluteHumidity = 0;
  float greenhouseHumidityDeficit = 0;

  float outsideTemperature = OFF_VAL;
  float outsideTemperatureMin24h = 0;
  float outsideTemperatureMax24h = 0;
  float outsideHumidity = 0;
  float outsideAbsoluteHumidity = 0;
  float outsideHumidityDeficit = 0;
*/
  float r1current;
  float r2current;

  boolean rain = false;
  float rainfall24h = 0;
  float windSpeed = 0;


  //lux;
  elapsedMillis minutCount;
  elapsedMillis hourCount;
  unsigned long averageLux;
  unsigned long averageDailyLux;
  int mV;
  unsigned long luxReading[10];
  unsigned long hourlyLuxReading[12];
  byte luxSmoothingArray = 24;
  unsigned long cloudLux = 20000;
  unsigned long sunLux = 50000;

//rain bucket
#define Bucket_Size 0.025 // bucket size to trigger tip count
#define RG11_Pin 3 // digital pin RG11 connected to

volatile unsigned long tipCount = 0; // bucket tip counter used in interrupt routine
volatile unsigned long contactTime; // Timer to manage any contact bounce in interrupt routine
volatile float totalRainfall = 0; // total amount of rainfall detected
float rainSample = 0;
float rainSetpoint = 0.1;
unsigned long rainSampleDelay = 60000;
elapsedMillis rainSampleCounter;


boolean sensorFailure = false;


unsigned long counter = 1;

int startMin = 0;
int startHour = 0;
int startDay = 1;
int startMonth = 1;
int startYear = 0;

void setRollupsInTestMode(){
  if(tempSensorTest != 0 || clockTest != 0){
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
      rain = false;
    }
    else{
      rain = true;
    }
  }
  //RG11 - 0.01'' bucket rain sensor
  else if(greenhouse.rainSensor.value() == RG11_BUCKET){
    //bucket mode
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
}

void getWind(){
  //0-5V (0-30 m/s) analog wind sensor
  int aRead = analogRead(WIND_ANALOG_SENSOR);
  windSpeed = (float)108/(float)1023*(float)aRead; //108km/h = 30m/s

}

void getCurrent(){
  float currentRead1 = ((float)10/(float)1023*analogRead(CURRENT_SENSOR1)-5);
  float currentRead2 = ((float)10/(float)1023*analogRead(CURRENT_SENSOR2)-5);

  motorOne.registerValue(abs(currentRead1));
  motorTwo.registerValue(abs(currentRead2));
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


boolean droppingTest = false;
boolean risingTest = false;
float startTempTest = 10.0;
float stopTempTest = 30.0;
float testIncrement = 0.05;



void testTemp(){
  if(risingTest == false){
    greenhouseTemperature.registerValue(startTempTest + testIncrement);
    risingTest = true;
  }
  else if((risingTest == true)&&(droppingTest == false)){
    greenhouseTemperature.registerValue(greenhouseTemperature.value() + testIncrement);
  }
  else if((risingTest == true)&&(droppingTest == true)){
    greenhouseTemperature.registerValue(greenhouseTemperature.value() - testIncrement);
  }
  if(greenhouseTemperature.value() >= stopTempTest){
    droppingTest = true;
  }
  if(greenhouseTemperature.value() <= startTempTest){
    droppingTest = false;
    risingTest = false;
    tempSensorTest = 0;
  }
}

void getGreenhouseTemp(){
  setRollupsInTestMode();
  if(tempSensorTest == TEST_TEMP){
    testTemp();
  }
  else{
    float temp;
    if(greenhouse.insideTemp.value() == DS18B20_TEMP){
      ds18b20_in.requestTemperatures();
      temp = ds18b20_in.getTempCByIndex(0);

      checkSensorFailure(greenhouseTemperature.value(), temp, -127.00,1,&sensorFailure);
    }
    else if(greenhouse.insideTemp.value() == STH1X_TEMP){
      temp = sht1x_in.readTemperatureC();
      checkSensorFailure(greenhouseTemperature.value(), temp, -40.00,1,&sensorFailure);
    }
    if(!sensorFailure){
      greenhouseTemperature.registerValue(temp);
    }
  }
}

float absoluteHumidity(float temperature, float humidity){
  //in g/m3
  return ((6.112 * pow(2.71828,((17.67 * temperature)/(temperature + 243.5))) * humidity * 2.1674) / (273.15 + temperature));
}

void getGreenhouseHum(){
  setRollupsInTestMode();
  float hum;

  if(greenhouse.insideTemp.value() == STH1X_TEMP){
    hum = sht1x_in.readHumidity();
    checkSensorFailure(greenhouseHumidity.value(), hum, 0,3,&sensorFailure);
  }
  if(!sensorFailure){
    greenhouseHumidity.registerValue(hum);
    //greenhouseAbsoluteHumidity = absoluteHumidity(greenhouseTemperature, greenhouseHumidity);
    //greenhouseHumidityDeficit = absoluteHumidity(greenhouseTemperature, 100) - absoluteHumidity(greenhouseTemperature, greenhouseHumidity);
  }
}

void getOutsideTemp(){
    float temp;
    if(greenhouse.outsideTemp.value() == DS18B20_TEMP){
      ds18b20_out.requestTemperatures();
      outsideTemperature.registerValue(ds18b20_out.getTempCByIndex(0));
    }
    else if(greenhouse.outsideTemp.value() == STH1X_TEMP){
      outsideTemperature.registerValue(sht1x_out.readTemperatureC());
    }
    else{
      outsideTemperature.registerValue(OFF_VAL);
    }
}

void getOutsideHum(){
  if(greenhouse.outsideTemp.value() == STH1X_TEMP){
    outsideHumidity.registerValue(sht1x_out.readHumidity());
    //outsideAbsoluteHumidity = absoluteHumidity(outsideTemperature, outsideHumidity);
    //outsideHumidityDeficit = absoluteHumidity(outsideTemperature, 100) - absoluteHumidity(outsideTemperature, outsideHumidity);
  }
  else{
    outsideHumidity.registerValue(0);
  }
}



void recordNewLuxReading(){

  int aRead = analogRead(LIGHT_SENSOR);
  mV = 5000/1023*aRead;
  unsigned long nowLux = 200000/1023*aRead;
  for (int x = 9; x > 0; x--){
    luxReading[x] = luxReading[x-1];
  }
  luxReading[0] = nowLux;

}

void recordNewHourlyLuxReading(){
  for (int x = 11; x > 0; x--){
    hourlyLuxReading[x] = hourlyLuxReading[x-1];
  }
  hourlyLuxReading[0] = averageLux;
}


unsigned short luxToWeatherRatio(unsigned long lux){
  unsigned long luxIncrementForWeatherSettings = (sunLux-cloudLux)/100;
  for(unsigned long x = 100; x > 0; x--){
    if(lux > cloudLux+luxIncrementForWeatherSettings*x){
      return x;
    }
  }
}

boolean betweenSunriseAndSunset(){
  return isBetween(Timepoint::sunRise[HOUR]+2,Timepoint::sunRise[MINUT], greenhouse.hr(), greenhouse.mn(),Timepoint::sunSet[HOUR]-2,Timepoint::sunSet[MINUT] );

}

void getLux(){

    recordNewLuxReading();

    unsigned long sumOfReadings = 0;
    int numOfDataAcquisition = 0;

    for(int x = 0; x < luxSmoothingArray; x++){
      if(luxReading[x] != 255){
        sumOfReadings += luxReading[x];
        numOfDataAcquisition += 1;
      }
    }
    if(sumOfReadings != 0){
      averageLux = sumOfReadings/numOfDataAcquisition;
    }

}

void getDailyLux(){

    recordNewHourlyLuxReading();

    unsigned long sumOfReadings = 0;
    int numOfDataAcquisition = 0;

    for(int x = 0; x < 12; x++){
      if(hourlyLuxReading[x] != 255){
        sumOfReadings += hourlyLuxReading[x];
        numOfDataAcquisition += 1;
      }
    }
    if(sumOfReadings != 0){
      averageDailyLux = sumOfReadings/numOfDataAcquisition;
    }
}

void luxCalculations(){
  if(minutCount > 10000){
    getLux();
    //greenhouse.weatherP.setValue(luxToWeatherRatio(averageLux));
    minutCount = 0;
  }
  if(hourCount > 3600000){
    getDailyLux();
    hourCount = 0;
  }
}

void autoWeather(){
  luxCalculations();
  if ((greenhouse.luxMeter.value() == true)&&(greenhouse.weatheradjust.value() == true)){

    if(betweenSunriseAndSunset()){
        //greenhouse.weatherP.setValue(luxToWeatherRatio(averageLux));
    }
    else{
      //greenhouse.weatherP.setValue(luxToWeatherRatio(averageDailyLux));
    }
  }
}

void initSensors(){
  //last recorded value if probe doesnt reply back at first cycle
    sensorBackup();
  //start communication with temp probe
    ds18b20_in.begin();
    ds18b20_in.setResolution(12);
    //ds18b20_out.begin();
    //ds18b20_out.setResolution(12);
    sht1x_in.initSensor();
    sht1x_out.initSensor();
  //start communication with clock
    rtc.begin();
  //rain sensor
    pinMode(RAIN_SWITCH, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(RAIN_SWITCH), isr_rg, FALLING);
    sei();// Enable Interrupts
  //init luxArrays
    resetLuxReadings();
    resetHourlyLuxReadings();
    getLux();
    getDailyLux();

}
