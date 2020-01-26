/*
  Greenhouse_prototype_CAPE_v.1.1.ino
  Copyright (C)2017 Loup Hébert-Chartrand. All right reserved

  This code has been made to interface with Arduino-like microcontrollers,
  for inclusion in greenhouse automation devices.

  Supported devices :
  - DS18B20 temperature sensor
  - DS3231 RTC module
  - 20x4 Serial LCD Display

  You can find the latest version of this code at :
  https://github.com/LoupHC/controleur-CAPE


  This code is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This code is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

//RTC_DS3231*********************************************
//********************GLOBAL**************************
//***************************************************
#define MODE_DISPLAY           1
#define MODE_PROGRAM           2
#define MODE_ACTION            3
#define SET_PARAMETERS     21
#define ACTION            3


#define ROLLUP_MENU_CONSTANT      1000
#define DEVICE_MENU_CONSTANT      2000
#define TIMEPOINT_MENU_CONSTANT   3000
#define GENERIC_MENU_CONSTANT     5000

//********************LCD parameters *******************
//Features :
// - 20x4 LCD display
// - I2C backpack
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

#define BACKLIGHT_PIN     3
LiquidCrystal_I2C  lcd(I2CADDR_LCD, 2, 1, 0, 4, 5, 6, 7);


//********************Keypad parameters *******************
//Features:
// - Matrix keypad 4x4
// - PCF8574 I/O expander

#define KEYPAD_DISPLAY

  #include <Keypad_I2C.h>
  #include <Keypad.h>
  #include <Wire.h>

#ifdef KEYPAD_DISPLAY
  const byte ROWS = 4; //four rows
  const byte COLS = 4; //four columns
  char keys[ROWS][COLS] = {
    {'1','2','3','A'},
    {'4','5','6','B'},
    {'7','8','9','C'},
    {'*','0','#','D'}
  };

  // bit numbers of PCF8574 i/o port
  byte rowPins[ROWS] = {7,6,5,4}; //connect to the row pinouts of the keypad
  byte colPins[COLS] = {3,2,1,0}; //connect to the column pinouts of the keypad

  Keypad_I2C keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS, I2CADDR_KEY);
#endif

#define I2C_OUTPUTS               //Comment this line if you don't use MCP23008 I/O Expander (Comment as well MCP_I2C_OUTPUTS on the Defines.h file of GreenhouseLib)

//********************Display variables*******************
unsigned int menuID = 5000;
unsigned int lastMenuID[30];
unsigned int menuPosition = 0;

short menu = MODE_DISPLAY;
boolean firstPrint = true;
boolean fastPlus = false;
boolean fastMinus = false;
const byte Code_lenght = 5; // Give enough room for 4 chars + NULL char
char Data[Code_lenght] = "0000"; // 4 is the number of chars it can hold + the null char = 5

elapsedMillis unpressedTimer;
elapsedMillis fastScrollTimer;

float fvariable;
unsigned short usvariable, usvariable1, usvariable2;
short svariable, svariable1, svariable2, svariable3, svariable4;
byte selectedElement;
unsigned short hourSet;
unsigned short minSet;

byte increment = 0;
short line = 0;
short maxLine;
short lastline = 0;
short smodif, smodif1, smodif2 = 0;
float fmodif = 0;

boolean confirm = false;
byte action = 0;


byte data_count = 0, master_count = 0;
bool Pass_is_good;
char keyPressed;

//Flashing display
byte flashingCounter = 0;
boolean codeWithNoDisplay = true;

boolean relayTest = false;
const byte RyDriverI2CAddr = B0100000;//device ID
byte GPIO = B11111111;
byte validAddress[6] = {0};

boolean reprint;

byte clocks[8] = {
  B01110,
  B10101,
  B10101,
  B10111,
  B10001,
  B10001,
  B01110,
};

byte rains[8] = {
  B00110,
  B11111,
  B11111,
  B01110,
  B10010,
  B00100,
  B01000,
};

byte wind1[8] ={
  B00000,
  B00010,
  B11010,
  B00100,
  B01011,
  B01000,
  B00000,
  B00000
};

byte wind2[8] = {
  B00000,
  B01000,
  B00101,
  B01110,
  B10100,
  B00010,
  B00000,
  B00000
};
byte humids[8] = {
  B00100,
  B01010,
  B01010,
  B10001,
  B10001,
  B10001,
  B01110,
};

byte locks[8] = {
  B00100,
  B01010,
  B01010,
  B11111,
  B11011,
  B11011,
  B11111,
  B00000,
};

byte up[8] = {
  B00100,
  B01110,
  B10101,
  B00100,
  B00100,
  B00100,
  B00100,
  B00100,
};
byte down[8] = {
  B00100,
  B00100,
  B00100,
  B00100,
  B00100,
  B10101,
  B01110,
  B00100,
};

byte target[] = {
  B00000,
  B01110,
  B10001,
  B10101,
  B10001,
  B01110,
  B00000,
  B00000
};

byte frosts[] = {
  B00100,
  B10101,
  B01110,
  B01110,
  B10101,
  B00100,
  B00000,
  B00000
};
const byte MAIN = 0;
const byte ROTUP = 1;
const byte ROTDOWN = 2;
const byte TYPE = 1;
const byte STARTTEMP = 2;
const byte STOPTEMP = 3;
const byte PAUSE= 3;
const byte HYST= 4;
const byte SETSTAGES= 5;
const byte S1TEMP= 10;
const byte S2TEMP= 20;
const byte S1TARGET= 11;
const byte S2TARGET= 21;
const byte HOME = 0;
const byte CONFIG = 1;
const byte HOURSET = 2;
const byte MINSET = 3;
const byte DAY = 4;
const byte MONTH = 5;
const byte YEAR = 6;
const byte SETLATITUDE = 7;
const byte SETLONGITUDE = 8;
const byte SETTIMEZONE = 9;
const byte ENABLEDAYNIGHT = 10;
const byte REF = 11;
const byte WEATHERADJUST = 12;
const byte WEATHER = 13;

const byte TEMPERATURES = 14;
const byte SELECTSENSORS = 15;
const byte INSIDETEMPSENSOR = 16;
const byte FACTORYSETTINGS = 17;
const byte ADJUST = 18;
const byte ADJUST24H = 19;
const byte ADJUSTVR = 20;
const byte ENTERCODE = 21;
const byte SETDIF = 22;
const byte SETPRENIGHT = 23;


const byte HUMIDITY6= 24;
const byte ANALOG1= 25;
const byte ANALOG2= 26;
const byte ANALOG3= 27;
const byte ANALOG4= 28;
const byte ANALOG5= 29;
const byte ANALOG6= 32;
const byte FIXOV8 = 33;
const byte FIXOV9 = 34;
const byte FIXOV10 = 35;
const byte PROGRAM = 36;
const byte PROTECTIONS = 37;
const byte FIXOV11 = 38;
const byte TENSIOM1 = 39;
const byte TENSIOM2 = 40;
const byte TENSIOM3 = 41;
const byte TENSIOMDATA = 42;
const byte ALARM = 43;

const byte AUTO= 50;
const byte FOPEN = 51;
const byte FCLOSE = 52;
const byte FINC1 = 53;
const byte FINC2 = 54;

const byte PULSE1= 55;
const byte PULSE2= 56;
const byte PULSE3= 57;

const byte FIXOV7= 58;
const byte ONTIME= 59;

const byte OVERRIDE = 60;
const byte FIXOV1 = 61;
const byte FIXOV2 = 62;
const byte FIXOV3 = 63;
const byte FIXOV4 = 64;
const byte HUMIDITY1= 65;
const byte HUMIDITY2= 66;
const byte RAIN1= 67;
const byte RAIN2= 68;
const byte WIND1= 69;
const byte WIND2= 70;
const byte BREAKER= 71;
const byte OUTSIDE1= 72;
const byte OUTSIDE2= 73;
const byte PERIODS = 74;
const byte ENABLE= 75;
const byte INSTRUCT= 76;
const byte PARAMETERS = 77;



const byte LOWTEMPALARM = 78;
const byte HIGHTEMPALARM = 94;

const byte MAXTEMP= 79;
const byte MINTEMP= 80;

const byte TESTS = 81;
const byte TESTTEMP = 82;

const byte WEATHERRATIO = 83;
const byte INSIDETEMP = 84;
const byte OUTSIDETEMP = 85;
const byte LUXMETER = 86;
const byte WINDSENSOR = 87;
const byte RAINSENSOR = 88;

const byte INSIDETEMPDATA = 89;
const byte OUTSIDETEMPDATA = 90;
const byte WEATHERDATA = 91;
const byte RADIATIONDATA = 92;
const byte CURRENTSENSOR = 93;


const byte HUMIDITY3= 95;
const byte HUMIDITY4= 96;
const byte HUMIDITY5= 97;

const byte FIXOV5= 98;
const byte FIXOV6= 99;

const byte STARTTIME = 1;
const byte HEATINGSUN = 3;
const byte COOLINGSUN = 4;
const byte HEATINGCLOUD = 5;
const byte COOLINGCLOUD = 6;
const byte RAMPING = 7;


unsigned long runTime;

//***************************************************
//********************MACROS**************************
//***************************************************
void clearData(){
for(int x = 0; x < Code_lenght-1; x++)
  {   // This can be used for any array size,
    Data[x] = 0; //clear array for new data
  }
  return;
}

void clearMenu(){
  firstPrint = true;
  codeWithNoDisplay = true;
  unpressedTimer = 0;
  data_count = 0;
  line = 0;
  increment = 0;
  lastline = 0;
  smodif = 0;
  smodif1 = 0;
  smodif2 = 0;
  fmodif = 0;
  lcd.clear();
  lcd.noBlink();
}

boolean nextIsConfirmed(){
  if((keyPressed == 'D')&&(unpressedTimer > 300)){
    return true;
  }
  else{
    return false;
  }
}
boolean backIsConfirmed(){
  if((keyPressed == 'C')&&(unpressedTimer > 300)){
    return true;
  }
  else{
    return false;
  }
}

boolean homeIsConfirmed(){
  if((keyPressed == 'A')&&(unpressedTimer > 300)){
    return true;
  }
  else{
    return false;
  }
}

boolean choiceIsConfirmed(){
  if(nextIsConfirmed() || backIsConfirmed()|| homeIsConfirmed()){
    return true;
  }
  else{
    return false;
  }
}

unsigned int genericId(unsigned int parameter){
  unsigned int fullId = GENERIC_MENU_CONSTANT+parameter;
  return fullId;
}

unsigned int rollupId(unsigned int nb, unsigned int parameter){
  unsigned int fullId = ROLLUP_MENU_CONSTANT+(nb*100)+parameter;
  return fullId;
}
unsigned int deviceId(unsigned int nb, unsigned int parameter){
  unsigned int fullId = DEVICE_MENU_CONSTANT+(nb*100)+parameter;
  return fullId;
}

unsigned int timepointId(unsigned int nb, unsigned int parameter){
  unsigned int fullId = TIMEPOINT_MENU_CONSTANT+(nb*100)+parameter;
  return fullId;
}
char buffer[30];
boolean enableMenuList[10] = {true, true, true, true, true, true, true, true, true, true};

void disableMenuElement(byte x){
  enableMenuList[x] = false;
}
void enableAllMenuItems(){
  for (int x = 0; x < sizeof(enableMenuList); x++){
    enableMenuList[x] = true;
  }
}

boolean menuIsEnabled(byte x){
  if(enableMenuList[x] == true){
    return true;
  }
  else{
    return false;
  }
}

void resetMenuCount(){
  reprint = true;
  menuPosition = 0; menuID = genericId(HOME);
}
void addPreviousMenu(unsigned int id){
  lastMenuID[menuPosition] = menuID;
  menuID = id;
  menuPosition ++;

}

void displayLastMenu(){
    reprint = true;
    clearMenu();
    menuPosition--;
    menuID = lastMenuID[menuPosition];
}

void displayHomeMenu(){
    reprint = true;
    clearMenu();
    resetMenuCount();
}

void displayNextMenu(unsigned short id){
  reprint = true;
  clearMenu();
  lastMenuID[menuPosition] = menuID;
  menuID = id;
  menuPosition++;
}

void display(unsigned short id){
  if(homeIsConfirmed()){
    enableAllMenuItems();
    displayHomeMenu();
  }
  else if(backIsConfirmed()){
    enableAllMenuItems();
    displayLastMenu();
  }
  else{
    enableAllMenuItems();
    displayNextMenu(id);
  }
}
void displayRollupMenuSequence(byte nb, byte prevmenu, byte menu){
    if(nextIsConfirmed()){
      resetMenuCount();
      addPreviousMenu(rollupId(nb,prevmenu));
    }
    display(rollupId(nb,menu));
}
void displayRollupMenuSequence(byte nb, byte menu){
    if(nextIsConfirmed()){
      resetMenuCount();
    }
    display(rollupId(nb,menu));
}

void displayDeviceMenuSequence(byte nb, byte prevmenu, byte menu){
    if(nextIsConfirmed()){
      resetMenuCount();
      addPreviousMenu(deviceId(nb,prevmenu));
    }
    display(deviceId(nb,menu));
}
void displayDeviceMenuSequence(byte nb, byte menu){
    if(nextIsConfirmed()){
      resetMenuCount();
    }
    display(deviceId(nb,menu));
}

void displayTimepointMenuSequence(byte nb, byte prevprevmenu, byte prevmenu, byte menu){
    if(nextIsConfirmed()){
      resetMenuCount();
      addPreviousMenu(genericId(prevprevmenu));
      addPreviousMenu(genericId(prevmenu));
    }
    display(timepointId(nb, menu));
}
void displayGenericMenuSequence(byte prevprevmenu, byte prevmenu, byte menu){
    if(nextIsConfirmed()){
      resetMenuCount();
      addPreviousMenu(genericId(prevprevmenu));
      addPreviousMenu(genericId(prevmenu));
    }
    display(genericId(menu));
}
void displayGenericMenuSequence(byte prevmenu, byte menu){
    if(nextIsConfirmed()){
      resetMenuCount();
      addPreviousMenu(genericId(prevmenu));
    }
    display(genericId(menu));
}
void displayGenericMenuSequence(byte menu){
    if(nextIsConfirmed()){
      resetMenuCount();
    }
    display(genericId(menu));
}


void serialPrintDigits(int digits){
  // utility function for digital clock display: prints preceding colon and leading 0
  if(digits < 10){
    Serial.print(F("0"));
  }
}

void lcdPrintDigits(int digits){
  // utility function for digital clock display: prints preceding colon and leading 0
  if((digits < 10)&&(digits >= 0)){
  lcd.print(F("0"));
  }
  lcd.print(digits);
}

const byte CLOCKS = 0;
const byte HUMIDS = 1;
const byte RAINS = 2;
const byte LOCKS = 3;
const byte TARGETS = 4;
const byte WINDS1 = 5;
const byte WINDS2 = 6;
const byte FROSTS = 7;


void initLCD(byte length, byte width){
  lcd.begin(length, width);
  lcd.setBacklightPin(BACKLIGHT_PIN, POSITIVE);
  lcd.setBacklight(HIGH);
  lcd.clear();
  lcd.createChar(FROSTS,frosts);
  lcd.createChar(CLOCKS,clocks);
  lcd.createChar(HUMIDS,humids);
  lcd.createChar(RAINS,rains);
  lcd.createChar(WINDS1,wind1);
  lcd.createChar(WINDS2,wind2);
  lcd.createChar(LOCKS,locks);
  lcd.createChar(TARGETS,target);
}

void printWeather(byte weather){
  if(greenhouse.weatheradjust.value() == true){
    if(weather >= 1 && weather <= 30){
     lcd.print(F("(CLD)"));
    }
    else if(weather > 30 && weather <= 70){
     lcd.print(F("(MIX)"));
    }
    else{
      lcd.print(F("(SUN)"));
    }
  }
}

void printWeather(){
  printWeather(greenhouse.weather());
}

String  printTimepoint(int nb){
  switch (nb){
    case 0 : return "DIF";break;
    case 1 : return "DAY";break;
    case 2 : return "PREN";break;
    case 3 : return "NIGHT";break;
  }
  return "";
}
void lcdPrintAlarm(byte _column, byte _row){

    //Device
  if(greenhouse.alarmEnabled.value()){
    lcd.setCursor(_column, _row); lcd.print(F("      "));
    lcd.setCursor(_column, _row); lcd.print(F("A:"));
    if(greenhouse.alarm.isActive() == true){
        lcd.print(F("ON "));
      }
    else{
      lcd.print(F("OFF"));
    }
  }
}
void lcdPrintOutput(String item, byte _column, byte _row, OnOffDevice &device){

    //Device
  if(device.isActivated()){
    lcd.setCursor(_column, _row); lcd.print(F("      "));
    lcd.setCursor(_column, _row); lcd.print(item);
    if(device.isOn() == true){
        lcd.print(F("ON "));
      }
    else{
      lcd.print(F("OFF"));
    }
    if(device.isLock()){
      lcd.setCursor(_column+5, _row);
      lcd.write((byte)LOCKS);
    }
    else if(device.isActive(ANALOG)){
      lcd.setCursor(_column+5, _row);
      lcd.write((byte)LOCKS);
    }
    else if(device.isActive(CLOCKOV1)){
      lcd.setCursor(_column+5, _row);
      if(device.ovType(CLOCKOV1) == HRCONDITIONAL || device.ovType(CLOCKOV1) == UNDERCONDITIONAL || device.ovType(CLOCKOV1) == SOILCONDITIONAL || device.ovType(CLOCKOV1) == UNDERDEFCONDITIONAL || device.ovType(CLOCKOV1) == HDEFCONDITIONAL){
        lcd.write((byte)HUMIDS);
      }
      else{
        lcd.write((byte)CLOCKS);
      }
    }
    else if(device.isActive(CLOCKOV2)){
      lcd.setCursor(_column+5, _row);

      if(device.ovType(CLOCKOV2) == HRCONDITIONAL || device.ovType(CLOCKOV2) == UNDERCONDITIONAL || device.ovType(CLOCKOV2) == SOILCONDITIONAL || device.ovType(CLOCKOV2) == UNDERDEFCONDITIONAL || device.ovType(CLOCKOV2) == HDEFCONDITIONAL){
          lcd.write((byte)HUMIDS);
      }
      else{
        lcd.write((byte)CLOCKS);
      }
    }
    else if(device.isActive(CLOCKOV3)){
      lcd.setCursor(_column+5, _row);
      if(device.ovType(CLOCKOV3) == HRCONDITIONAL || device.ovType(CLOCKOV3) == UNDERCONDITIONAL || device.ovType(CLOCKOV3) == SOILCONDITIONAL || device.ovType(CLOCKOV3) == UNDERDEFCONDITIONAL || device.ovType(CLOCKOV3) == HDEFCONDITIONAL){
        lcd.write((byte)HUMIDS);
      }
      else{
        lcd.write((byte)CLOCKS);
      }
    }
    else if(device.isActive(DESHUM)){
      lcd.setCursor(_column+5, _row);
      lcd.write((byte)HUMIDS);
    }
  }
}

void lcdPrintRollups(String side, byte _column, byte _row, Rollup & rollup){

  if (motorFailure[rollup.nb()] == true){
    if(flashingCounter == 0){
      lcd.setCursor(_column, _row);
      lcd.print(F("      "));
      lcd.setCursor(_column, _row);
      lcd.print(side);lcd.print(F("OVERL"));
    }
    else if(flashingCounter == 1){
      lcd.setCursor(_column, _row);
      lcd.print(F("      "));
    }
  }
  else if(rollup.isActivated()){
    lcd.setCursor(_column, _row);
    if(rollup.isStandby()){lcd.print(side);}
    else if (rollup.opening() == true){lcd.print(F("OPEN  "));}
    else if (rollup.closing() == true){lcd.print(F("CLOSE "));}
    else if(rollup.isWaiting()){
      if(flashingCounter == 0){
        lcd.setCursor(_column, _row);
        lcd.print(F("      "));
        lcd.setCursor(_column, _row);
        lcd.print(side);
        lcd.print(rollup.incrementCounter());
        lcd.print(F("%"));
      }
      else if(flashingCounter == 1){
        lcd.setCursor(_column, _row);
        lcd.print(F("      "));
      }
    }
    else{
      lcd.setCursor(_column, _row);
      lcd.print(F("      "));
      lcd.setCursor(_column, _row);
      lcd.print(side);
      lcd.print(rollup.incrementCounter());
      lcd.print(F("%"));
    }
    if(rollup.isLock()){
        lcd.setCursor(_column+5, _row);
        lcd.write((byte)LOCKS);
    }
    else if(rollup.isActive(WINDOV)){
      lcd.setCursor(_column+5, _row);
      if(flashingCounter == 0){
        lcd.write((byte)WINDS1);
      }
      else if(flashingCounter == 1){
        lcd.write((byte)WINDS2);
      }
    }
    else if(rollup.isEnabled(RAINOV) && rain == true){
      lcd.setCursor(_column+5, _row);
      lcd.write((byte)RAINS);
    }
    else if(rollup.isActive(OUTTEMP)){
      lcd.setCursor(_column+5, _row);
        lcd.write((byte)FROSTS);
    }
    else if(rollup.isActive(CLOCKOV1)){
      lcd.setCursor(_column+5, _row);
      if(rollup.ovType(CLOCKOV1) == HRCONDITIONAL || rollup.ovType(CLOCKOV1) == HDEFCONDITIONAL){
        lcd.write((byte)HUMIDS);
      }
      else{
        lcd.write((byte)CLOCKS);
      }
    }
    else if(rollup.isActive(CLOCKOV2)){
      lcd.setCursor(_column+5, _row);
      if(rollup.ovType(CLOCKOV2) == HRCONDITIONAL || rollup.ovType(CLOCKOV2) == HDEFCONDITIONAL ){
        lcd.write((byte)HUMIDS);
      }
      else{
        lcd.write((byte)CLOCKS);
      }
    }
    else if(rollup.isActive(CLOCKOV3)){
      lcd.setCursor(_column+5, _row);
      if(rollup.ovType(CLOCKOV3) == HRCONDITIONAL || rollup.ovType(CLOCKOV3) == HDEFCONDITIONAL){
        lcd.write((byte)HUMIDS);
      }
      else{
        lcd.write((byte)CLOCKS);
      }
    }

  }
}
void lcdPrintTemp(byte _row){

    lcd.setCursor(0,_row); lcd.print(F("         "));
    lcd.setCursor(0,_row);
    if(sensorFailure == false){lcd.print(greenhouseTemperature.value()); lcd.print(F("C"));}
    else if(sensorFailure == true){lcd.print(F("!!!"));}

    if(greenhouseHumidity.value() != 0){
      lcd.setCursor(4,_row);
      if(sensorFailure == false){ lcd.print(F("C-   "));lcd.setCursor(6,_row); lcd.print((int)greenhouseHumidity.value());lcd.print(F("%"));}
      else if(sensorFailure == true){lcd.print(F("!!!"));}
    }
}
boolean heaterIsActive(int nb){
  if((greenhouse.device[nb].type.value() == HEATERTYPE)&&(greenhouse.device[nb].isActivated())){
    return true;
  }
  else{
    return false;
  }
}

boolean fanIsActive(int nb){
  if((greenhouse.device[nb].type.value() == FANTYPE)&&(greenhouse.device[nb].isActivated())){
    return true;
  }
  else{
    return false;
  }
}
boolean closeFromCoolingTemp(){
  if((greenhouseTemperature.value()> greenhouse.coolingTemp())|| (greenhouseTemperature.value() > (greenhouse.coolingTemp() - ((greenhouse.coolingTemp()-greenhouse.heatingTemp())/2)) && (greenhouseTemperature.value() < greenhouse.coolingTemp()))){
    return true;
  }
  else{
    return false;
  }
}
boolean tensiometerDeviceIsActive(int nb){
  if(greenhouse.tensiometer[nb].value() == true){
    return true;
  }
  else{
    return false;
  }
}
boolean tensiometerDeviceIsActive(){
  if(greenhouse.tensiometer[0].value() == true || greenhouse.tensiometer[1].value() == true || greenhouse.tensiometer[1].value() == true ){
    return true;
  }
  else{
    return false;
  }
}

byte tensiometerDevices(){
  byte count = 0;
  for(int x = 0; x < MAX_TENSIOMETERS;x++){
    if(greenhouse.tensiometer[0].value() == true){
      count++;
    }
  }
  return count;
}
byte activeTensiometerDevice(){
  if(tensiometerDevices() == 1){
    for(int x = 0; x < MAX_TENSIOMETERS;x++){
      if(greenhouse.tensiometer[x].value() == true){
        return x;
      }
    }
  }
  return OFF_VAL;
}

boolean heatingDeviceIsActive(){
  byte count = 0;
  for(int x = 0; x < MAX_DEVICES;x++){
    if(heaterIsActive(x)){
      count++;
    }
  }
  if(count != 0){
    return true;
  }
  else{
    return false;
  }
}

boolean coolingDeviceIsActive(){
  byte count = 0;
  for(int x = 0; x < MAX_DEVICES;x++){
    if(fanIsActive(x)){
      count++;
    }
  }

  for(int x = 0; x < MAX_ROLLUPS;x++){
    if(greenhouse.rollup[x].isActivated()){
      count++;
    }
  }

  if(count != 0){
    return true;
  }
  else{
    return false;
  }

}
boolean someDeviceIsActive(){

    byte count = 0;
    for(int x = 0; x < MAX_DEVICES;x++){
      if(fanIsActive(x)){
        count++;
      }
    }
    for(int x = 0; x < MAX_DEVICES;x++){
      if(heaterIsActive(x)){
        count++;
      }
    }
    for(int x = 0; x < MAX_ROLLUPS;x++){
      if(greenhouse.rollup[x].isActivated()){
        count++;
      }
    }

    if(count != 0){
      return true;
    }
    else{
      return false;
    }

}

byte heatingDevices(){
  byte count = 0;
  for(int x = 0; x < MAX_DEVICES;x++){
    if(heaterIsActive(x)){
      count++;
    }
  }
  return count;
}

byte otherHeatingDevices(int nb){
  int x = 0;
  if(nb == 0){
    if(heaterIsActive(1)){x++;}
    if(heaterIsActive(2)){x++;}
    if(heaterIsActive(3)){x++;}
    if(heaterIsActive(4)){x++;}
    if(heaterIsActive(5)){x++;}
  }
  else if(nb == 1){
    if(heaterIsActive(0)){x++;}
    if(heaterIsActive(2)){x++;}
    if(heaterIsActive(3)){x++;}
    if(heaterIsActive(4)){x++;}
    if(heaterIsActive(5)){x++;}
  }
  else if(nb == 2){
    if(heaterIsActive(0)){x++;}
    if(heaterIsActive(1)){x++;}
    if(heaterIsActive(3)){x++;}
    if(heaterIsActive(4)){x++;}
    if(heaterIsActive(5)){x++;}
  }
  else if(nb == 3){
    if(heaterIsActive(0)){x++;}
    if(heaterIsActive(1)){x++;}
    if(heaterIsActive(2)){x++;}
    if(heaterIsActive(4)){x++;}
    if(heaterIsActive(5)){x++;}
  }
  else if(nb == 4){
    if(heaterIsActive(0)){x++;}
    if(heaterIsActive(1)){x++;}
    if(heaterIsActive(2)){x++;}
    if(heaterIsActive(3)){x++;}
    if(heaterIsActive(5)){x++;}
  }
  else if(nb == 5){
    if(heaterIsActive(0)){x++;}
    if(heaterIsActive(1)){x++;}
    if(heaterIsActive(2)){x++;}
    if(heaterIsActive(3)){x++;}
    if(heaterIsActive(4)){x++;}
  }
  return x;
}

byte coolingDevices(){
  int x = 0;
  if(fanIsActive(0)){x++;}
  if(fanIsActive(1)){x++;}
  if(fanIsActive(2)){x++;}
  if(fanIsActive(3)){x++;}
  if(fanIsActive(4)){x++;}
  if(fanIsActive(5)){x++;}
  if(greenhouse.rollup[0].isActivated()){x++;}
  if(greenhouse.rollup[1].isActivated()){x++;}
  if(greenhouse.rollup[2].isActivated()){x++;}
  return x;
}

void printClosestTarget(){
  if(coolingDeviceIsActive()){
    if(/*greenhouseTemperature > greenhouse.coolingTemp() ||*/ !heatingDeviceIsActive()){
      lcd.setCursor(10,0);
      lcd.write(TARGETS);
      lcd.print((int)greenhouse.coolingTemp());
      lcd.print(F("C"));
    }
  }
  if(heatingDeviceIsActive()){
    if(/*greenhouseTemperature < greenhouse.heatingTemp() ||*/ !coolingDeviceIsActive()){
        lcd.setCursor(10,0);
        lcd.write(TARGETS);
        lcd.print((int)greenhouse.heatingTemp());
        lcd.print(F("C"));
    }
    else/* if(greenhouseTemperature > greenhouse.heatingTemp() && greenhouseTemperature < greenhouse.coolingTemp())*/{
      lcd.setCursor(10,0);
      if(greenhouse.daynight.value() == false){
        lcd.write(TARGETS);
      }
      lcd.print((int)greenhouse.heatingTemp());
      lcd.print(F("-"));
      lcd.print((int)greenhouse.coolingTemp());
      lcd.print(F("C"));
    }
  }

}

void lcdPrintTarget(){
  lcd.setCursor(9,0); lcd.print(F("           "));
  lcd.setCursor(9,0);
    lcd.print(F("|"));
  if(greenhouse.isRamping()){
    if(flashingCounter == 0){
      printClosestTarget();
      if(greenhouse.daynight.value() == true){
        lcd.setCursor(15,0);
        switch(greenhouse.nowTimepoint()){
          case 1: lcd.print(F("(DIF)"));break;
          case 2: lcd.print(F("(DAY)"));break;
          case 3: lcd.print(F("(PRE)"));break;
          case 4: lcd.print(F("(NGT)"));break;
        }
      }
    }
    else if(flashingCounter == 1){
      lcd.print(F("          "));
    }
  }
  else{
    printClosestTarget();
    if(greenhouse.daynight.value() == true){

      lcd.setCursor(15,0);
      switch(greenhouse.nowTimepoint()){
        case 1: lcd.print(F("(DIF)"));break;
        case 2: lcd.print(F("(DAY)"));break;
        case 3: lcd.print(F("(PRE)"));break;
        case 4: lcd.print(F("(NGT)"));break;
      }
    }
  }



}

void lcdPrintTime(byte _row){
  lcd.setCursor(0,_row); lcdPrintDigits(greenhouse.rightNow(2)); lcd.print(F(":")); lcdPrintDigits(greenhouse.rightNow(1));lcd.print(F(":")); lcdPrintDigits(greenhouse.rightNow(0)); lcd.print(F(" |"));
  lcdPrintDigits(greenhouse.rightNow(3)); lcd.print(F("/")); lcdPrintDigits(greenhouse.rightNow(4));
  if(greenhouse.weatheradjust.value() == true){
    printWeather();
  }
  else{
     lcd.print(F("/")); lcdPrintDigits(2000+greenhouse.rightNow(5));
  }
}

void lcdPrintSeparators(){
  lcd.setCursor(6,2);
  lcd.print(F("|"));
  lcd.setCursor(13,2);
  lcd.print(F("|"));
  lcd.setCursor(6,3);
  lcd.print(F("|"));
  lcd.setCursor(13,3);
  lcd.print(F("|"));
}

void lcdPrintOutputs(){

  if(R1.isActivated()){
    lcdPrintRollups("R:",0, 2, greenhouse.rollup[0]);
  }
  if(R2.isActivated()){
    lcdPrintRollups("R:",7, 2, greenhouse.rollup[1]);
  }
  if(R3.isActivated()){
    lcdPrintRollups("R:", 14, 2, greenhouse.rollup[2]);
  }

  if(D1.isActivated()){
    if(D1.type.value() == FANTYPE){
      lcdPrintOutput("F:", 0, 2, greenhouse.device[0]);
    }
    else if(D1.type.value() == HEATERTYPE){
        lcdPrintOutput("H:", 0, 2, greenhouse.device[0]);
    }
    else if(D1.type.value() == VALVTYPE){
        lcdPrintOutput("V:", 0, 2, greenhouse.device[0]);
    }
  }
  if(D2.isActivated()){
    if(D2.type.value() == FANTYPE){
      lcdPrintOutput("F:", 7, 2, greenhouse.device[1]);
    }
    else if(D2.type.value() == HEATERTYPE){
      lcdPrintOutput("H:", 7, 2, greenhouse.device[1]);
    }
    else if(D2.type.value() == VALVTYPE){
      lcdPrintOutput("V:", 7, 2, greenhouse.device[1]);
    }
  }
  if(D3.isActivated()){
    if(D3.type.value() == FANTYPE){
      lcdPrintOutput("F:", 14, 2, greenhouse.device[2]);
    }
    else if(D3.type.value() == HEATERTYPE){
      lcdPrintOutput("H:", 14, 2, greenhouse.device[2]);
    }
    else if(D3.type.value() == VALVTYPE){
      lcdPrintOutput("V:", 14, 2, greenhouse.device[2]);
    }
  }

  if(D4.isActivated()){
    if(D4.type.value() == FANTYPE){
      lcdPrintOutput("F:", 0, 3, greenhouse.device[3]);
    }
    else if(D4.type.value() == HEATERTYPE){
      lcdPrintOutput("H:", 0, 3, greenhouse.device[3]);
    }
    else if(D4.type.value() == VALVTYPE){
      lcdPrintOutput("V:", 0, 3, greenhouse.device[3]);
    }
  }

  if(D5.isActivated()){
    if(D5.type.value() == FANTYPE){
      lcdPrintOutput("F:", 7, 3, greenhouse.device[4]);
    }
    else if(D5.type.value() == HEATERTYPE){
      lcdPrintOutput("H:", 7, 3, greenhouse.device[4]);
    }
    else if(D5.type.value() == VALVTYPE){
      lcdPrintOutput("V:", 7, 3, greenhouse.device[4]);
    }
  }

  if(D6.isActivated()){
    if(D6.type.value() == FANTYPE){
      lcdPrintOutput("F:", 14, 3, greenhouse.device[5]);
    }
    else if(D6.type.value() == HEATERTYPE){
      lcdPrintOutput("H:", 14, 3, greenhouse.device[5]);
    }
    else if(D6.type.value() == VALVTYPE){
      lcdPrintOutput("V:", 14, 3, greenhouse.device[5]);
    }
  }
  if(greenhouse.alarmEnabled.value()){
    lcdPrintAlarm(14,3);
  }
}

void yearRoundStartTime(){
  lcd.clear();
  lcd.setCursor(14,0); lcdPrintDigits(greenhouse.rightNow(3)); lcd.print(F("/")); lcdPrintDigits(greenhouse.rightNow(4));//lcd.print(F("/")); lcdPrintDigits(greenhouse.rightNow(5)); // lcd.print(F(" |")); lcdPrintDigits(greenhouse.rightNow(3)); lcd.print(F("/")); lcdPrintDigits(greenhouse.rightNow(4)); lcd.print(F("/")); lcdPrintDigits(2000+greenhouse.rightNow(5));
  if (greenhouse.dif.value() == true){
    lcd.setCursor(0,0);lcd.print(F("DIF: ")); lcdPrintDigits(T1.hr());lcd.print(F(":"));lcdPrintDigits(T1.mn());

    switch (T1.type.value()){
      case 0: lcd.print(F("(SR)"));break;
      case 1: lcd.print(F("(CL)"));break;
      case 2: lcd.print(F("(SS)"));break;
    }
  }
  lcd.setCursor(0,1);lcd.print(F("DAY: ")); lcdPrintDigits(T2.hr());lcd.print(F(":"));lcdPrintDigits(T2.mn());
  switch (T2.type.value()){
    case 0: lcd.print(F("(SR)"));break;
    case 1: lcd.print(F("(CL)"));break;
    case 2: lcd.print(F("(SS)"));break;
  }
  if (greenhouse.prenight.value() == true){
    lcd.setCursor(0,2);lcd.print(F("PRE: ")); lcdPrintDigits(T3.hr());lcd.print(F(":"));lcdPrintDigits(T3.mn());

    switch (T3.type.value()){
      case 0: lcd.print(F("(SR)"));break;
      case 1: lcd.print(F("(CL)"));break;
      case 2: lcd.print(F("(SS)"));break;
    }
  }
  lcd.setCursor(0,3);lcd.print(F("NGT: ")); lcdPrintDigits(T4.hr());lcd.print(F(":"));lcdPrintDigits(T4.mn());
  switch (T4.type.value()){
    case 0: lcd.print(F("(SR)"));break;
    case 1: lcd.print(F("(CL)"));break;
    case 2: lcd.print(F("(SS)"));break;
  }
}

void homeDisplay(){
  lcd.noBlink();
  if(clockTest == TEST_CLOCKFF){
    yearRoundStartTime();
  }
  else{
    if(firstPrint == true){
      lcd.clear();
      lcdPrintTemp(0);
      lcdPrintTime(1);
      lcdPrintOutputs();
      firstPrint = false;
    }

    lcdPrintTemp(0);
    lcdPrintTarget();
    lcdPrintTime(1);
    lcdPrintOutputs();
  }
  lcdPrintSeparators();
}

void adjustLine(){
  //adjust line
  if (line > maxLine - 3){
    line = maxLine-3;
  }
  if(line<0){
    line = 0;
  }
}
void select(){

  lcd.setCursor(0,2);
  lcd.print(F("YES"));
  lcd.setCursor(0,3);
  lcd.print(F("NO"));

  if(line > 1){
    line = 0;
  }
  if(line < 0){
    line = 1;
  }
  if(line == 0){
    lcd.setCursor(5, 3);
    lcd.blink();
    confirm = false;
  }
  if(line == 1){
    lcd.setCursor(5, 2);
    lcd.blink();
    confirm = true;
  }
}


void jumpIncrements(){
  increment++;
  if(increment > 1){
    increment = 0;
  }
}

short sincrement(){
  if (increment == 0){
    return 1;
  }
  else{
    return 10;
  }

}
short minincrement(){
  if (increment == 0){
    return 10;
  }
  else{
    return 60;
  }

}
float fincrement(){
  if (increment == 0){
    return 1;
  }
  else{
    return 0.1;
  }
}

void adjustusvariable(unsigned short min, unsigned short value, unsigned short max){

  if (line > lastline){
    smodif += sincrement();
  }
  else if (line < lastline){
    smodif -= sincrement();
  }

    svariable = value + smodif;
  if(svariable > (short)max){
    svariable = (short)max;
    line -= 1;
    smodif = max - value;
  }
  else if(svariable < (short)min){
    svariable = (short)min;
    line += 1;
    smodif = min - value;
  }
  usvariable = (unsigned short) svariable;
  lastline = line;

  svariable = value + smodif;


}
void adjustsvariable(short min, short value, short max){

  if (line > lastline){
    smodif += sincrement();
  }
  else if (line < lastline){
    smodif -= sincrement();
  }

  svariable = value + smodif;
  if(svariable > max){
    svariable = max;
    line -= 1;
    smodif = max - value;
  }
  else if(svariable < min){
    svariable = min;
    line += 1;
    smodif = min - value;
  }
  lastline = line;
  svariable = value + smodif;

}
void adjusttimevariable(short hour, short min){

  if (line > lastline){
    smodif2 += minincrement();
  }
  else if (line < lastline){
    smodif2 -= minincrement();
  }
  svariable2 = min + smodif2;

  if(svariable2 >= 60){
    smodif2 -= 60;
    smodif1 += 1;
  }
  if(svariable2 < 0){
    smodif2 += 60;
    smodif1 -= 1;
  }

  svariable2 = min + smodif2;
  svariable1 = hour + smodif1;

  if(svariable1 > 23){
    smodif2 -= minincrement();
    line -= 1;
  }
  else if(svariable1 < 0){
    smodif2 += minincrement();
    line += 1;
  }

  svariable2 = min + smodif2;

  if(svariable2 >= 60){
    smodif2 -= 60;
    smodif1 += 1;
  }
  if(svariable2 < 0){
    smodif2 += 60;
    smodif1 -= 1;
  }

  svariable2 = min + smodif2;
  svariable1 = hour + smodif1;
  lastline = line;
}



void adjustfvariable(float min, float value, float max){
  if (line > lastline){
    fmodif += fincrement();
  }
  else if (line < lastline){
    fmodif -= fincrement();
  }
  fvariable = value + fmodif;
  if(fvariable > max){
    fvariable = max;
    line -= 1;
    fmodif = max - value;
  }
  else if(fvariable < min){
    fvariable = min;
    line += 1;
    fmodif = min - value;
  }
  lastline = line;
  fvariable = value + fmodif;

}

void printHeader(const __FlashStringHelper* variableName){

  //if(firstPrint == true){
    lcd.setCursor(0,0);
    lcd.print(F("                   "));
  //}
  //firstPrint = false;
  //codeWithNoDisplay = false;

  lcd.noBlink();
  lcd.setCursor(0,0);
  lcd.print(variableName);
}

void printHeader(String header, const __FlashStringHelper* variableName){

  //if(firstPrint == true){
    lcd.setCursor(0,0);
    lcd.print(F("                   "));
  //}
  //firstPrint = false;
  //codeWithNoDisplay = false;

  lcd.setCursor(0,0);
  lcd.print(header);
  lcd.print(variableName);
}
void printHeader(const __FlashStringHelper* longName,const __FlashStringHelper* shortName,  byte weather, byte nb){

  //if(firstPrint == true){
    lcd.setCursor(0,0);
    lcd.print(F("                   "));
  //}
  //firstPrint = false;
  //codeWithNoDisplay = false;

  lcd.setCursor(0,0);
  if(greenhouse.daynight.value() == true){
    lcd.print(printTimepoint(nb));
    lcd.print(F(" - "));
    lcd.print(shortName);
    }
  else{
    lcd.print(longName);
  }
}

void printScrollingInstructions(){/*
  lcd.setCursor(0,1);
  lcd.print(F("*# : scroll"));
  lcd.setCursor(0,2);
  lcd.print(F("D  : select"));*/
}
void printScrollingInstructionsShort(){/*
  lcd.setCursor(0,1);
  lcd.print(F("*#:scroll | D:select"));
  lcd.setCursor(0,2);
  lcd.print(F("D  : select"));*/
}
void printAdjustInstructions(){/*
  lcd.setCursor(0,1);
  lcd.print(F("*# : adjust"));
  lcd.setCursor(0,2);
  lcd.print(F("D  : confirm/next"));*/
}
void printAdjustInstructionsShort(){/*
  lcd.setCursor(0,1);
  lcd.print(F("*#:adjust|D:conf/nxt"));*/
}
void printConfirmInstructions(){
  lcd.setCursor(0,1);
  lcd.print(F("Press 'N' to confirm"));
  lcd.setCursor(0,2);
  lcd.print(F("Press 'B' to cancel"));
}
void printReviewInstructions(){
  lcd.setCursor(0,1);
  lcd.print(F("Review of parameters"));
  lcd.setCursor(0,2);
  lcd.print(F("is required before "));
  lcd.setCursor(0,3);
  lcd.print(F("activation (press N)"));
}

void printWeatherAdjustExtremes(){
  lcd.setCursor(7,3);
  if(usvariable == 2){
    lcd.print(F("[CLD]"));
  }
  if(usvariable == 100){
    lcd.print(F("[SUN]"));
  }
}
void printSelfCalibration(){
  lcd.setCursor(0,3);
  if(usvariable == 0){
    lcd.print(F(" [SELF-CALIBRATION] "));
  }
}

void printDisabledIfZero(){
  lcd.setCursor(5,3);
  if(fvariable == 0){
    lcd.print(F("[DISABLED]"));
  }
}

void printSunrise(){
  lcd.setCursor(0,1);
  lcd.print(F("Sunrise :  ["));
  lcdPrintDigits(Timepoint::sunRise[2]);
  lcd.print(F("]:["));
  lcdPrintDigits(Timepoint::sunRise[1]);
  lcd.print(F("]"));
}
void printSunset(){
  lcd.setCursor(0,1);
  lcd.print(F("Sunset :   ["));
  lcdPrintDigits(Timepoint::sunSet[2]);
  lcd.print(F("]:["));
  lcdPrintDigits(Timepoint::sunSet[1]);
  lcd.print(F("]"));
}

void printCursor(float number, float increment, byte row, byte line){
  byte cursor = 0;
  if(number > 9){
    cursor +=1;
  }
  if(number > 99){
    cursor +=1;
  }
  if (increment == 10){
    cursor -=1;
  }
  if (increment == 0.1){
    cursor +=2;
  }
  lcd.setCursor((row + cursor), line);
  lcd.blink();
}
void printCursor(float increment, byte row, byte line){
  byte cursor = 0;
  if(increment == 10){
    cursor +=4;
  }
  lcd.setCursor((row + cursor), line);
  lcd.blink();
}
void confirmVariable(float min, float value, float max){
  adjustfvariable(min,value,max);
  lcd.setCursor(0,3);
  lcd.print(F("      ["));
  lcd.print(fvariable);
  lcd.print(F("]    "));
  printCursor(fvariable, fincrement(),7,3);
}

void confirmVariable(const __FlashStringHelper* header, float min, float value, float max, float ref, byte pos){
  adjustfvariable(min,value,max);

  if(pos == 1){
    lcd.setCursor(7,1);
    lcd.print(header);
    lcd.setCursor(6,2);
    lcd.print(F(">["));
    lcd.print(fvariable);
    lcd.print(F("]  "));
    if(greenhouse.weatheradjust.value() == true){
      lcd.setCursor(0,2);
      lcd.print(F("SUN"));
      lcd.setCursor(0,3);
      lcd.print(F("CLD"));
      lcd.setCursor(3,3);
      lcd.setCursor(6,3);
      lcd.print(F(" ["));
      lcd.print(ref);
      lcd.print(F("]  "));
    }
    printCursor(fvariable, fincrement(),8,2);
    lcd.blink();
  }
  else if(pos == 2 && greenhouse.weatheradjust.value() == true){
    lcd.setCursor(0,2);
    lcd.print(F("SUN"));
    lcd.setCursor(0,3);
    lcd.print(F("CLD"));
    lcd.setCursor(3,3);
    lcd.setCursor(7,1);
    lcd.print(header);
    lcd.setCursor(6,2);
    lcd.print(F(" ["));
    lcd.print(ref);
    lcd.print(F("]  "));
    lcd.setCursor(6,3);
    lcd.print(F(">["));
    lcd.print(fvariable);
    lcd.print(F("]  "));
    printCursor(fvariable, fincrement(),8,3);
  }
}
void confirmVariable(float min, float value, float max, float ref1,float ref2,float ref3, byte pos){
  adjustfvariable(min,value,max);/*
  lcd.setCursor(0,1);
  lcd.print("                    ");
  lcd.setCursor(0,2);
  lcd.print("                    ");*/

  if(pos == 1){
    lcd.setCursor(0,1);
    lcd.print(F("    Heating  Cooling"));
    lcd.setCursor(3,2);
    lcd.print(F(">["));
    lcd.print(fvariable);
    lcd.print(F("]  "));
    lcd.setCursor(13,2);
    lcd.print(F("["));
    lcd.print(ref1);
    lcd.print(F("]"));
    if(greenhouse.weatheradjust.value() == true){
      lcd.setCursor(0,2);
      lcd.print(F("SUN"));
      lcd.setCursor(0,3);
      lcd.print(F("CLD"));
      lcd.setCursor(3,3);
      lcd.print(F(" ["));
      lcd.print(ref2);
      lcd.print(F("]  "));
      lcd.setCursor(12,3);
      lcd.print(F(" ["));
      lcd.print(ref3);
      lcd.print(F("]"));
    }
    printCursor(fvariable, fincrement(),5,2);
    lcd.blink();
  }

  else if(pos == 2){
    lcd.setCursor(0,1);
    lcd.print(F("    Heating  Cooling"));
    lcd.setCursor(3,2);
    lcd.print(F(" ["));
    lcd.print(ref1);
    lcd.print(F("] "));
    lcd.setCursor(12,2);
    lcd.print(F(">["));
    lcd.print(fvariable);
    lcd.print(F("]"));
    if(greenhouse.weatheradjust.value() == true){
      lcd.setCursor(0,2);
      lcd.print(F("SUN"));
      lcd.setCursor(0,3);
      lcd.print(F("CLD"));
      lcd.setCursor(3,3);
      lcd.print(F(" ["));
      lcd.print(ref2);
      lcd.print(F("] "));
      lcd.setCursor(12,3);
      lcd.print(F(" ["));
      lcd.print(ref3);
      lcd.print(F("]"));
    }
    printCursor(fvariable, fincrement(),14,2);
    lcd.blink();
  }
  else if(pos == 3){
    lcd.setCursor(0,1);
    lcd.print(F("    Heating  Cooling"));
    lcd.setCursor(3,2);
    lcd.print(F(" ["));
    lcd.print(ref1);
    lcd.print(F("] "));
    lcd.setCursor(12,2);
    lcd.print(F(" ["));
    lcd.print(ref2);
    lcd.print(F("]"));
    if(greenhouse.weatheradjust.value() == true){
      lcd.setCursor(0,2);
      lcd.print(F("SUN"));
      lcd.setCursor(0,3);
      lcd.print(F("CLD"));
      lcd.setCursor(3,3);
      lcd.print(F(">["));
      lcd.print(fvariable);
      lcd.print(F("] "));
      lcd.setCursor(12,3);
      lcd.print(F(" ["));
      lcd.print(ref3);
      lcd.print(F("]"));
    }
    printCursor(fvariable, fincrement(),5,3);
  }
  else if(pos == 4){
    lcd.setCursor(0,1);
    lcd.print(F("    Heating  Cooling "));
    lcd.setCursor(3,2);
    lcd.print(F(" ["));
    lcd.print(ref1);
    lcd.print(F("] "));
    lcd.setCursor(12,2);
    lcd.print(F(" ["));
    lcd.print(ref2);
    lcd.print(F("]"));
    if(greenhouse.weatheradjust.value() == true){
      lcd.setCursor(0,2);
      lcd.print(F("SUN"));
      lcd.setCursor(0,3);
      lcd.print(F("CLD"));
      lcd.setCursor(3,3);
      lcd.print(F(" ["));
      lcd.print(ref3);
      lcd.print(F("] "));
      lcd.setCursor(12,3);
      lcd.print(F(">["));
      lcd.print(fvariable);
      lcd.print(F("]"));
    }
    printCursor(fvariable, fincrement(),14,3);
  }
}
void confirmVariable(const __FlashStringHelper* header,const __FlashStringHelper* secondLine,  float min, float value, float max, float reference){
  adjustfvariable(min,value,max);
  lcd.setCursor(0,1);
  lcd.print(header);
  lcd.print(reference);lcd.print(F("C"));
  lcd.setCursor(0,2);
  lcd.print(secondLine);lcd.print(reference + fvariable);lcd.print(F("C"));
  lcd.setCursor(0,3);
  lcd.print(F("MOD :       ["));
  if(fvariable >= 0){lcd.print(F("+"));}
  lcd.print(fvariable);
  lcd.print(F("]"));
  printCursor(fvariable, fincrement(),14,3);
}
void confirmVariableLESS(const __FlashStringHelper* header,const __FlashStringHelper* secondLine, float min, float value, float max, float reference){
  adjustfvariable(min,value,max);
  lcd.setCursor(0,1);
  lcd.print(header);
  lcd.print(reference);lcd.print(F("C"));
  lcd.setCursor(0,2);
  lcd.print(secondLine);lcd.print(reference - fvariable);lcd.print(F("C"));
  lcd.setCursor(0,3);
  lcd.print(F("MOD :       ["));
  if(fvariable >= 0){lcd.print(F("-"));}
  lcd.print(fvariable);
  lcd.print(F("]"));
  printCursor(fvariable, fincrement(),14,3);
}


void confirmVariable(byte min, byte value, byte max){
  adjustusvariable(min,value,max);
  lcd.setCursor(0,3);
  lcd.print(F("       ["));
  lcd.print(usvariable);
  lcd.print(F("]   "));
  printCursor(usvariable, sincrement(),8,3);
}

void confirmVariable(unsigned short min, unsigned short value, unsigned short max){
  adjustusvariable(min,value,max);
  lcd.setCursor(0,3);
  lcd.print(F("       ["));
  lcd.print(usvariable);
  lcd.print(F("]   "));
  printCursor(usvariable, sincrement(),8,3);
}

void confirmPulseVariable(unsigned short min, unsigned short value, unsigned short max, unsigned short ref, byte pos){
  adjustusvariable(min,value,max);

  if(pos == 1){
    lcd.setCursor(0,2);
    lcd.print(F("On-time(min): >["));
    lcd.print(usvariable);
    lcd.print(F("]"));
    lcd.setCursor(0,3);
    lcd.print(F("Off-time(min): ["));
    lcd.print(ref);
    lcd.print(F("]"));
    printCursor(usvariable, sincrement(),16,2);
  }
  if(pos == 2){
    lcd.setCursor(0,2);
    lcd.print(F("On-time(min):  ["));
    lcd.print(ref);
    lcd.print(F("]"));
    lcd.setCursor(0,3);
    lcd.print(F("Off-time(min):>["));
    lcd.print(usvariable);
    lcd.print(F("]"));
    printCursor(usvariable, sincrement(),16,3);
  }
}
void confirmVariable(short min, short value, short max){
  adjustsvariable(min,value,max);
  lcd.setCursor(0,3);
  lcd.print(F("       ["));
  lcd.print(svariable);
  lcd.print(F("]   "));
  printCursor(svariable, sincrement(),8,3);
}
void confirmVariable(short min, short value, short max, float reference){
  adjustsvariable(min,value,max);
  lcd.setCursor(0,3);
  lcd.print(F("       ["));
  lcd.print(svariable);
  lcd.print(F("]"));
  printCursor(svariable, sincrement(),8,3);

}

void confirmVariable(short hour, short min){
  adjusttimevariable(hour, min);
  lcd.setCursor(0,3);
  lcd.print(F("     ["));
  lcdPrintDigits(svariable1);
  lcd.print(F("]:["));
  lcdPrintDigits(svariable2);
  lcd.print(F("]   "));
  printCursor(minincrement(),7,3);
}

void confirmVariable(short hour, short min, short hour2, short min2, byte pos){
  adjusttimevariable(hour, min);
  if(pos == 1){
    lcd.setCursor(0,2);
    lcd.print(F("Start at: >["));
    lcdPrintDigits(svariable1);
    lcd.print(F("]:["));
    lcdPrintDigits(svariable2);
    lcd.print(F("]"));
    lcd.setCursor(0,3);
    lcd.print(F("Stop at:   ["));
    lcdPrintDigits(hour2);
    lcd.print(F("]:["));
    lcdPrintDigits(min2);
    lcd.print(F("]"));
    printCursor(minincrement(),13,2);
  }
  if(pos == 2){
    lcd.setCursor(0,3);
    lcd.print(F("Stop at:  >["));
    lcdPrintDigits(svariable1);
    lcd.print(F("]:["));
    lcdPrintDigits(svariable2);
    lcd.print(F("]"));
    lcd.setCursor(0,2);
    lcd.print(F("Start at:  ["));
    lcdPrintDigits(hour2);
    lcd.print(F("]:["));
    lcdPrintDigits(min2);
    lcd.print(F("]"));
    printCursor(minincrement(),13,3);
  }

}
void confirmVariable(short min_hour, short hour, short max_hour, short min_min, short min, short max_min){
  adjusttimevariable(hour, min);
  lcd.setCursor(0,3);
  lcd.print(F("     ["));
  lcdPrintDigits(svariable1);
  lcd.print(F("]:["));
  lcdPrintDigits(svariable2);
  lcd.print(F("]   "));
  printCursor(minincrement(),7,3);
}

void printListElement(String element, byte line){
  lcd.setCursor(0,line);
  lcd.print(element);
}
void printBrackettedListElement(String element, byte line){
  //int cursor = (20-(element.length()+2))/2;
  lcd.setCursor(0,line);
  lcd.print(F("> "));
  lcd.setCursor(2,line);
  lcd.print(element);
  //lcd.print(F("]"));
}

void updateScreen(){
  reprint = true;
  for(int x = 1; x <= 3; x++  ){
    lcd.setCursor(0,x);
    lcd.print(F("                    "));
  }
}


void confirmVariable(unsigned short startPos, const char* const item[] PROGMEM, const int menuLenght){
  short select = (short)startPos + line;
  short lastElement = menuLenght-1;
  short firstElement = 0;

  while(enableMenuList[lastElement] == false){
    lastElement--;
  }
  while(enableMenuList[firstElement] == false){
    firstElement++;
  }

  while(enableMenuList[select] == false && select < lastElement && select > firstElement){
    if (line >= lastline){select++;line++;}
    else {select--;line--;}
  }
  if(select > lastElement){
    line -= 1;
    select = lastElement;
  }
  if(select < firstElement){
    line += 1;
    select = firstElement;
  }
  selectedElement = select;

  if(selectedElement != firstElement){
    byte prevElement = selectedElement -1;
    while(enableMenuList[prevElement] == false && prevElement > firstElement){
      prevElement--;
    }
    if(enableMenuList[prevElement] == true){
      strcpy_P(buffer, (char*)pgm_read_word(&(item[prevElement])));
      lcd.setCursor(1,3);
      lcd.print(buffer);
    }

  }
  if(selectedElement != lastElement){
    byte nextElement = selectedElement +1;
    while(enableMenuList[nextElement] == false && nextElement < lastElement){
      nextElement++;
    }
    if(enableMenuList[nextElement] == true){
      strcpy_P(buffer, (char*)pgm_read_word(&(item[nextElement])));
      lcd.setCursor(1,1);
      lcd.print(buffer);
    }
  }

  strcpy_P(buffer, (char*)pgm_read_word(&(item[selectedElement])));
  lcd.setCursor(0,2);
  lcd.print(F(">"));
  lcd.print(buffer);
  lastline = line;
}
/*
void confirmVariable(unsigned short startPos, const char* const item[] PROGMEM, const int menuLenght){
  short select = (short)startPos + line;

  if(select > menuLenght-1){
    line -= 1;
    select = menuLenght-1;
  }
  if(select < 0){
    line += 1;
    select = 0;
  }
  selectedElement = select;

  if(selectedElement != 0){
    byte prevElement = selectedElement -1;
    strcpy_P(buffer, (char*)pgm_read_word(&(item[prevElement])));
    lcd.setCursor(1,3);
    lcd.print(buffer);

  }
  if(selectedElement != menuLenght-1){
    byte nextElement = selectedElement +1;
    strcpy_P(buffer, (char*)pgm_read_word(&(item[nextElement])));
    lcd.setCursor(1,1);
    lcd.print(buffer);
  }

  strcpy_P(buffer, (char*)pgm_read_word(&(item[selectedElement])));
  lcd.setCursor(0,2);
  lcd.print(F(">"));
  lcd.print(buffer);

}
*/
void confirmVariable(unsigned short typeValue, const String item[], const int menuLenght){
  short type = (short)typeValue + line;

  for(int x = 1; x <= 3; x++  ){
    lcd.setCursor(0,x);
    lcd.print(F("                    "));
  }

  if(type > menuLenght-1){
    line -= 1;
    type = menuLenght-1;
  }
  if(type < 0){
    line += 1;
    type = 0;
  }
  selectedElement = type;

  if(selectedElement != 0){
    byte prevElement = selectedElement -1;
    printListElement(item[prevElement],3);
  }
  if(selectedElement != menuLenght-1){
    byte nextElement = selectedElement +1;
    printListElement(item[nextElement],1);
  }
  printBrackettedListElement(item[selectedElement],2);

}
void checkIIC(){
  lcd.noBlink();
  lcd.setCursor(0,0);
  lcd.print(F("----I2C DEVICES-----"));

  byte error, address;
  int nDevices = 0;

  for(address = 1; address < 127; address++ ) {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0)
    {
      validAddress[nDevices] = address;
      nDevices++;
    }
  }
  lcd.setCursor(0,1);
  if(validAddress[0] != 0){
    lcd.print(F("0x"));
    if (validAddress[0]<16)
      lcd.print(F("0"));
    lcd.print(validAddress[0],HEX);
  }
  lcd.setCursor(0,2);
  if(validAddress[1] != 0){
    lcd.print(F("0x"));
    if (validAddress[1]<16)
      lcd.print(F("0"));
    lcd.print(validAddress[1],HEX);
  }
  lcd.setCursor(0,3);
  if(validAddress[2] != 0){
    lcd.print(F("0x"));
    if (validAddress[2]<16)
      lcd.print(F("0"));
    lcd.print(validAddress[2],HEX);
  }
  lcd.setCursor(10,1);
  if(validAddress[3] != 0){
    lcd.print(F("0x"));
    if (validAddress[3]<16)
      lcd.print(F("0"));
    lcd.print(validAddress[3],HEX);
  }
  lcd.setCursor(10,2);
  if(validAddress[4] != 0){
    lcd.print(F("0x"));
    if (validAddress[4]<16)
      lcd.print(F("0"));
    lcd.print(validAddress[4],HEX);
  }
  lcd.setCursor(10,3);
  if(validAddress[5] != 0){
    lcd.print(F("0x"));
    if (validAddress[5]<16)
      lcd.print(F("0"));
    lcd.print(validAddress[5],HEX);
  }
}

void checkRelays(){
  GPIO = B00000000;
  if(relayTest == false){
   Wire.begin();
   Wire.beginTransmission(0x20); //begins talking to the slave device
   Wire.write(0x00); //selects the IODIRA register
   Wire.write(0x00); //this sets all port A pins to outputs
   Wire.write(0x09);//select GPIO register
   Wire.write(GPIO);//set register value-all low
   Wire.endTransmission(); //stops talking to device
  }
  relayTest = true;
  GPIO = B11111111;

  Wire.beginTransmission(RyDriverI2CAddr);
  Wire.write(0x09);//select GPIO register
  Wire.write(GPIO);//set register value-all high
  Wire.endTransmission();

  delay(5000);

  GPIO = B00000000;

  Wire.beginTransmission(RyDriverI2CAddr);
  Wire.write(0x09);//select GPIO register
  Wire.write(GPIO);//set register value-all high
  Wire.endTransmission();

  delay(1000);

  for(int x = 0; x < 8; x++){
    bitWrite(GPIO, x, 1);
    Wire.beginTransmission(RyDriverI2CAddr);
    Wire.write(0x09);//select GPIO register
    Wire.write(GPIO);//set register value-all high
    Wire.endTransmission();
    delay(1000);
  }

  for(int x = 0; x < 8; x++){
    bitWrite(GPIO, x, 0);
    Wire.beginTransmission(RyDriverI2CAddr);
    Wire.write(0x09);//select GPIO register
    Wire.write(GPIO);//set register value-all high
    Wire.endTransmission();
    delay(1000);
  }
}

boolean programSelected(const char* id){
  if(!strcmp(Data, id)){
    return true;
  }
  else{
    return false;
  }
}

void zeroEqualInfinity(){
  if(usvariable == 0){
    lcd.setCursor(3,3);
    lcd.print(F("INDEFINITELY"));
  }
}

void updateFlashCounter(){
  if(flashingCounter == 0){
    flashingCounter = 1;
  }
  else if(flashingCounter == 1){
    flashingCounter = 0;
  }
}

void refreshScreen(int seconds){
  if(unpressedTimer > (unsigned long) seconds*1000){
    displayHomeMenu();
  }
}

void resetTimerBeforeRefresh(){
 if(keyPressed != NO_KEY && keyPressed != '0' && keyPressed != 'A'&& keyPressed != 'D' && keyPressed != 'C'){
   unpressedTimer = 0;
 }
}




void disabled(int nb){
  lcd.clear(),  lcd.setCursor(0,0);lcd.print(F("DISABLED"));Serial.println(F("disabled menu"));
}
void disabled(){
  lcd.clear(),  lcd.setCursor(0,0);lcd.print(F("DISABLED"));Serial.println(F("disabled menu"));
}

String header(String text, int nb){
  int index = nb+1;
  return text + index;
}

const char unlock_s[] PROGMEM = "unlock";
const char activate_s[] PROGMEM = "activate";
const char cancel_s[] PROGMEM = "cancel";
const char alarm_s[] PROGMEM = "alarm";
const char rollup_s[] PROGMEM = "rollup";
const char fan_s[] PROGMEM = "fan";
const char heater_s[] PROGMEM = "heater";
const char valv_s[] PROGMEM = "valv";
const char confirm_s[] PROGMEM = "confirm";
const char enabled_s[] PROGMEM = "enable";
const char disabled_s[] PROGMEM = "disable";
const char manual_s[] PROGMEM = "manual";
const char sun_s[] PROGMEM = "sun";
const char disable_s[] PROGMEM = "disable";
const char parameters_s[] PROGMEM = "parameters";
const char programs_s[] PROGMEM = "programs";
const char protection_s[] PROGMEM = "protections";
const char overrides_s[] PROGMEM = "overrides";
const char rotationtime_s[] PROGMEM = "rotation time";
const char calibrate_s[] PROGMEM = "calibrate";
const char steps_s[] PROGMEM = "steps";
const char breaker_s[] PROGMEM = "breaker";
const char coldlock_s[] PROGMEM = "cold lock";
const char rain_s[] PROGMEM = "rain";
const char wind_s[] PROGMEM =  "wind";
const char humidity_s[] PROGMEM = "humidity";
const char timer3_s[] PROGMEM = "program 3";
const char timer2_s[] PROGMEM = "program 2";
const char timer1_s[] PROGMEM = "program 1";
const char btimer3_s[] PROGMEM = "(program 3)";
const char btimer2_s[] PROGMEM = "(program 2)";
const char btimer1_s[] PROGMEM = "(program 1)";
const char lockat_s[] PROGMEM = "lock at %";
const char lockclose_s[] PROGMEM = "lock close";
const char lockopen_s[] PROGMEM = "lock open";
const char ontime_s[] PROGMEM = "on-time";
const char analogsensor_s[] PROGMEM = "analog input";
const char pulse_s[] PROGMEM = "pulse";
const char lockon_s[] PROGMEM = "lock ON";
const char lockoff_s[] PROGMEM = "lock OFF";
const char above_s[] PROGMEM = "above";
const char under_s[] PROGMEM = "under";
const char deshumd_s[] PROGMEM = "deshum(deficit)";
const char deshumrh_s[] PROGMEM = "deshum(RH%)";
const char humidifyd_s[] PROGMEM = "humidify(deficit)";
const char humidifyrh_s[] PROGMEM = "humidify(RH%)";
const char soilmoisture_s[] PROGMEM = "humidify(soil)";
const char weatherc_s[] PROGMEM = "weather-specific";
const char auto_s[] PROGMEM = "auto";
const char ratio_s[] PROGMEM = "ratio";
const char cloud_s[] PROGMEM = "cloud";
const char mix_s[] PROGMEM = "mix";
const char tensiom3_s[] PROGMEM = "tensiometer3";
const char tensiom2_s[] PROGMEM = "tensiometer2";
const char tensiom1_s[] PROGMEM = "tensiometer1";
const char string_0[] PROGMEM = "motors";
const char string_1[] PROGMEM = "luxmeter";
const char anemometer_s[] PROGMEM = "anemometer";
const char string_2[] PROGMEM = "rain sensor";
const char string_3[] PROGMEM = "outside";
const char string_4[] PROGMEM = "inside";
const char string_5[] PROGMEM = "DS18B20";
const char string_6[] PROGMEM = "SHT1X";
const char string_7[] PROGMEM = "disabled";
const char string_8[] PROGMEM = "NC contact";
const char string_9[] PROGMEM = "NO contact";
const char string_10[] PROGMEM = "bucket mode";
const char bprenight_s[] PROGMEM = "(prenight)";
const char bmorning_s[] PROGMEM = "(morning)";
const char prenight_s[] PROGMEM = "prenight";
const char morning_s[] PROGMEM = "morning";
const char day_s[] PROGMEM = "day";
const char night_s[] PROGMEM = "night";
const char settings_s[] PROGMEM = "settings";
const char periods_s[] PROGMEM = "periods";
const char temperatures_s[] PROGMEM = "temperatures";
const char weatheradjust_s[] PROGMEM = "weather adjust";
const char daynightsettings_s[] PROGMEM = "day/night settings";
const char adjustt24h_s[] PROGMEM = "adjust T24h";
const char suntracking_s[] PROGMEM = "sun tracking";
const char factoryr_s[] PROGMEM = "factory reset";
const char htalarm_s[] PROGMEM = "high temp alarm";
const char ltalarm_s[] PROGMEM = "low temp alarm";
const char sensors_s[] PROGMEM = "sensors";
const char tests_s[] PROGMEM = "tests";
const char timecoord_s[] PROGMEM = "time/coord";
const char setpoint_s[] PROGMEM = "set temperatures";
const char analog_s[] PROGMEM = "0-5V analog";
const char sparkfun_s[] PROGMEM = "sparkfun kit";
const char testtemp_s[] PROGMEM = "test temp";
const char testrelays_s[] PROGMEM = "test relays";
const char ffw_s[] PROGMEM = "FFW";
const char fffw_s[] PROGMEM = "FFFW";

const char* const rainSensorList[] PROGMEM = {string_8, string_9, string_10, disabled_s};
const int sizeOfRainSensorList = sizeof(rainSensorList)/sizeof(rainSensorList[0]);

const char* const tempSensorList[] PROGMEM = {string_5, string_6, string_7};
const int sizeOfTempSensorList = sizeof(tempSensorList)/sizeof(tempSensorList[0]);

const char* const sensorList[] PROGMEM = {tensiom3_s, tensiom2_s, tensiom1_s,string_0, string_1,anemometer_s, string_2, string_3, string_4};
const int sizeOfSensorList = sizeof(sensorList)/sizeof(sensorList[0]);
byte lastSelectedElementInSensorList = sizeOfSensorList;

const char* const tensiometerList[] PROGMEM = {tensiom1_s, tensiom2_s, tensiom3_s};
const int sizeOfTensiometerList = sizeof(tensiometerList)/sizeof(tensiometerList[0]);
byte lastSelectedElementInTensiometerList = sizeOfTensiometerList;

const char* const	periodsListNMP[] PROGMEM= {bprenight_s, bmorning_s,night_s,day_s};
const char* const	periodsListNM[] PROGMEM= {prenight_s, bmorning_s,night_s,day_s};
const char* const	periodsListNP[] PROGMEM= {bprenight_s, morning_s,night_s,day_s};
const char* const	periodsList[] PROGMEM= {prenight_s, morning_s,night_s,day_s};
const int sizeOfPeriodsList = sizeof(periodsList)/sizeof(periodsList[0]);
byte lastSelectedElementInPeriodsList = sizeOfPeriodsList;

const char* const		mainTimepointBasicList[] PROGMEM= {settings_s,temperatures_s};
const int sizeOfMainTimepointBasicList = sizeof(mainTimepointBasicList)/sizeof(mainTimepointBasicList[0]);
byte lastSelectedElementInTimepointBasicList = sizeOfMainTimepointBasicList;

const char* const		mainTimepointList[] PROGMEM= {settings_s,periods_s,temperatures_s};
const int sizeOfMainTimepointList = sizeof(mainTimepointList)/sizeof(mainTimepointList[0]);
byte lastSelectedElementInTimepointList = sizeOfMainTimepointList;

const char* const		adjustTemperaturesList[] PROGMEM= {suntracking_s, weatheradjust_s,daynightsettings_s};
const int sizeOfAdjustTemperaturesList = sizeof(adjustTemperaturesList)/sizeof(adjustTemperaturesList[0]);
byte lastSelectedElementInAdjustList = sizeOfAdjustTemperaturesList;

const char* const	configList[] PROGMEM = {factoryr_s,tests_s, sensors_s,timecoord_s,setpoint_s};
//const int sizeOfMainRollupList = sizeof(mainRollupList)/sizeof(mainRollupList[0]);
const int sizeOfConfigList = sizeof(configList)/sizeof(configList[0]);
byte lastSelectedElementInConfigList = sizeOfConfigList;

const char* const anemometerList[] PROGMEM = {sparkfun_s, analog_s, disabled_s};
const int sizeOfAnemometerList = sizeof(anemometerList)/sizeof(anemometerList[0]);

const char* const testsList[] PROGMEM = {  testtemp_s,testrelays_s,ffw_s,fffw_s};
const int sizeOfTestsList = sizeof(testsList)/sizeof(testsList[0]);

const char* const unlockList[] PROGMEM = {unlock_s};
const int sizeOfUnlockList = sizeof(unlockList)/sizeof(unlockList[0]);
const char* const activateList[] PROGMEM = {activate_s};
const int sizeOfActivateList = sizeof(activateList)/sizeof(activateList[0]);
const char* const confirmList[] PROGMEM = {confirm_s, cancel_s};
const int sizeOfConfirmList = sizeof(confirmList)/sizeof(confirmList[0]);

const char* const deviceList[] PROGMEM = {alarm_s,rollup_s,fan_s,heater_s,valv_s};
const int sizeOfDeviceList = sizeof(deviceList)/sizeof(deviceList[0]);
const char* const enableList[] PROGMEM = {disabled_s,enabled_s};
const int sizeOfEnableList = sizeof(enableList)/sizeof(enableList[0]);
const char* const timeRefList[] PROGMEM = {manual_s,sun_s};
const int sizeOfTimeRefList = sizeof(timeRefList)/sizeof(enableList[0]);

const char* const		stateList[] PROGMEM = {lockoff_s,lockon_s};
const int sizeOfStateList = sizeof(stateList)/sizeof(stateList[0]);

const char* const		valvConditionList[] PROGMEM = {lockon_s,pulse_s,weatherc_s,soilmoisture_s,humidifyrh_s, humidifyd_s};
const int sizeOfValvConditionList = sizeof(valvConditionList)/sizeof(valvConditionList[0]);
byte lastSelectedElementInValvConditionList = sizeOfValvConditionList;

const char* const		deviceConditionList[] PROGMEM = {lockon_s,pulse_s,deshumrh_s,deshumd_s};
const int sizeOfDeviceConditionList = sizeof(deviceConditionList)/sizeof(deviceConditionList[0]);
byte lastSelectedElementInDeviceConditionList = sizeOfDeviceConditionList;

const char* const		rollupConditionList[] PROGMEM = {lockat_s,pulse_s,deshumrh_s, deshumd_s};
const int sizeOfRollupConditionList = sizeof(rollupConditionList)/sizeof(rollupConditionList[0]);
byte lastSelectedElementInRollupConditionList = sizeOfRollupConditionList;

const char* const		mainValvList[] PROGMEM = {disable_s,ontime_s,analogsensor_s,programs_s, overrides_s};
const int sizeOfMainValvList = sizeof(mainValvList)/sizeof(mainValvList[0]);
byte lastSelectedElementInValvList = sizeOfMainValvList;

const char* const		mainDeviceList[] PROGMEM = {disable_s,ontime_s,analogsensor_s,programs_s, parameters_s,overrides_s};
const int sizeOfMainDeviceList = sizeof(mainDeviceList)/sizeof(mainDeviceList[0]);
byte lastSelectedElementInDeviceList = sizeOfMainDeviceList;

const char* const		overrideDeviceList[] PROGMEM = {pulse_s,lockoff_s,lockon_s};
const int sizeOfOverrideDeviceList = sizeof(overrideDeviceList)/sizeof(overrideDeviceList[0]);
byte lastSelectedElementInOverrideDeviceList = sizeOfOverrideDeviceList;

const char* const		overrideDeviceLockList[] PROGMEM = {unlock_s};
const int sizeOfOverrideDeviceLockList = sizeof(overrideDeviceLockList)/sizeof(overrideDeviceLockList[0]);
byte lastSelectedElementInOverrideDeviceLockList = sizeOfOverrideDeviceLockList;

const char* const		referenceList[] PROGMEM = {above_s,under_s};
const int sizeOfReferenceList = sizeof(referenceList)/sizeof(referenceList[0]);


const char* const weatherList[] PROGMEM  = { auto_s, ratio_s,cloud_s,mix_s,sun_s};
const int sizeOfWeatherList = sizeof(weatherList)/sizeof(weatherList[0]);

const char* const weather2List[] PROGMEM  = { cloud_s,mix_s, sun_s};
const int sizeOfWeather2List = sizeof(weather2List)/sizeof(weather2List[0]);

const char* const	programsList123[] PROGMEM= {timer3_s, timer2_s,timer1_s};
const char* const	programsList12[] PROGMEM= {btimer3_s, timer2_s,timer1_s};
const char* const	programsList13[] PROGMEM= {timer3_s, btimer2_s,timer1_s};
const char* const	programsList23[] PROGMEM= {timer3_s, timer2_s,btimer1_s};
const char* const	programsList1[] PROGMEM= {btimer3_s, btimer2_s,timer1_s};
const char* const	programsList2[] PROGMEM= {btimer3_s, timer2_s,btimer1_s};
const char* const	programsList3[] PROGMEM= {timer3_s, btimer2_s,btimer1_s};
const char* const	programsList[] PROGMEM= {btimer3_s, btimer2_s,btimer1_s};
const int sizeOfProgramsList = sizeof(programsList)/sizeof(programsList[0]);
byte lastSelectedElementInProgramsList = sizeOfProgramsList;


const char* const	mainAlarmList[] PROGMEM = {disable_s, htalarm_s,ltalarm_s,};
const int sizeOfMainAlarmList = sizeof(mainAlarmList)/sizeof(mainAlarmList[0]);
byte lastSelectedElementInAlarmList = sizeOfMainAlarmList;


const char* const	mainRollupList[] PROGMEM = {disable_s, protection_s, programs_s, parameters_s, overrides_s};
const int sizeOfMainRollupList = sizeof(mainRollupList)/sizeof(mainRollupList[0]);
byte lastSelectedElementInRollupList = sizeOfMainRollupList;

const char* const	rollupParametersList[] PROGMEM= {calibrate_s,rotationtime_s, steps_s};
const int sizeOfRollupParametersList = sizeof(rollupParametersList)/sizeof(rollupParametersList[0]);
byte lastSelectedElementInRollupParametersList = sizeOfRollupParametersList;

const char* const	protectionsRollupList[] PROGMEM= {breaker_s,coldlock_s,rain_s, wind_s};
const int sizeOfProtectionsRollupList = sizeof(protectionsRollupList)/sizeof(protectionsRollupList[0]);
byte lastSelectedElementInProtectionsRollupList = sizeOfProtectionsRollupList;

const char* const	overrideRollupList[] PROGMEM= {lockat_s,lockclose_s,lockopen_s};
const int sizeOfOverrideRollupList = sizeof(overrideRollupList)/sizeof(overrideRollupList[0]);
byte lastSelectedElementInOverrideRollupList = sizeOfOverrideRollupList;

const char* const	overrideRollupLockList[] PROGMEM= {unlock_s};
const int sizeOfOverrideRollupLockList = sizeof(overrideRollupLockList)/sizeof(overrideRollupLockList[0]);
byte lastSelectedElementInOverrideRollupLockList = sizeOfOverrideRollupLockList;

boolean clockOvSequence(Rollup &rollup, boolean index1, boolean index2,boolean index3){
  if((rollup.isEnabled(CLOCKOV1) == index1)&&(rollup.isEnabled(CLOCKOV2) == index2)&&(rollup.isEnabled(CLOCKOV3) == index3)){
    return true;
  }
  else{
    return false;
  }
}
boolean clockOvSequence(OnOffDevice &device, boolean index1, boolean index2,boolean index3){
  if((device.isEnabled(CLOCKOV1) == index1)&&(device.isEnabled(CLOCKOV2) == index2)&&(device.isEnabled(CLOCKOV3) == index3)){
    return true;
  }
  else{
    return false;
  }
}

byte clockOvIndex;

byte clockOv(byte index){
  switch(index){
    case 0 : return CLOCKOV1; break;
    case 1 : return CLOCKOV2; break;
    case 2 : return CLOCKOV3; break;
  }
  return OFF_VAL;
}

String deviceHeader(int nb){
  String header;
  String prefix;
  int index = nb+1;
  if(!greenhouse.device[nb].isActivated()){
    prefix = "D";
    header = prefix+index;
    return header;
  }
  else if(greenhouse.device[nb].type.value() == FANTYPE){
    prefix = "F";
    header = prefix+index;
    return header;
  }
  else if(greenhouse.device[nb].type.value() == HEATERTYPE){
    prefix = "H";
    header = prefix+index;
    return header;
  }
  else if(greenhouse.device[nb].type.value() == VALVTYPE){
    prefix = "V";
    header = prefix+index;
    return header;
  }
  else{
    prefix = "??";
    return prefix;
  }
}

String rollupHeader(int nb){
  String header;
  String prefix;
  int index = nb+1;
  prefix = "R";
  header = prefix+index;
  return header;
}

void mainRollup(int nb){
  if(greenhouse.rollup[nb].isActivated()){

    printHeader(rollupHeader(nb),F(" - MAIN MENU      "));
    confirmVariable(lastSelectedElementInRollupList, mainRollupList, sizeOfMainRollupList);

    if(choiceIsConfirmed()){
      switch (selectedElement) {
        case 4: lastSelectedElementInRollupList = 4;display(rollupId(nb,OVERRIDE));break;
        case 3: lastSelectedElementInRollupList = 3;display(rollupId(nb,PARAMETERS));break;
        case 2: lastSelectedElementInRollupList = 2;display(rollupId(nb,PROGRAM));break;
        case 1: lastSelectedElementInRollupList = 1;display(rollupId(nb,PROTECTIONS));break;
        case 0: lastSelectedElementInRollupList = sizeOfMainRollupList;display(rollupId(nb,ENABLE));break;
      }
    }
  }
  else{
    resetMenuCount();
    display(rollupId(nb,ENABLE));
  }


}


void programRollup(int nb){
  printHeader(rollupHeader(nb),F(" - PROGRAMS      "));

  if(clockOvSequence(greenhouse.rollup[nb], 1,1,1)){
    confirmVariable(lastSelectedElementInProgramsList, programsList123, sizeOfProgramsList);
  }
  else if(clockOvSequence(greenhouse.rollup[nb], 1,1,0)){
    confirmVariable(lastSelectedElementInProgramsList, programsList12, sizeOfProgramsList);
  }
  else if(clockOvSequence(greenhouse.rollup[nb], 1,0,1)){
    confirmVariable(lastSelectedElementInProgramsList, programsList13, sizeOfProgramsList);
  }
  else if(clockOvSequence(greenhouse.rollup[nb], 0,1,1)){
    confirmVariable(lastSelectedElementInProgramsList, programsList23, sizeOfProgramsList);
  }
  else if(clockOvSequence(greenhouse.rollup[nb], 1,0,0)){
    confirmVariable(lastSelectedElementInProgramsList, programsList1, sizeOfProgramsList);
  }
  else if(clockOvSequence(greenhouse.rollup[nb], 0,1,0)){
    confirmVariable(lastSelectedElementInProgramsList, programsList2, sizeOfProgramsList);
  }
  else if(clockOvSequence(greenhouse.rollup[nb], 0,0,1)){
    confirmVariable(lastSelectedElementInProgramsList, programsList3, sizeOfProgramsList);
  }
  else{
    confirmVariable(lastSelectedElementInProgramsList, programsList, sizeOfProgramsList);
  }

  if(choiceIsConfirmed()){
    switch (selectedElement) {
    case 2: clockOvIndex = 0; break;
    case 1: clockOvIndex = 1; break;
    case 0: clockOvIndex = 2; break;
    }
    lastSelectedElementInProgramsList = selectedElement;
    if(!greenhouse.rollup[nb].isEnabled(clockOv(clockOvIndex))){
      display(rollupId(nb, FIXOV2));
    }
    else{
      display(rollupId(nb, FIXOV1));
    }
  }
}

void setParametersRollup(int nb){
  printHeader(rollupHeader(nb),F("- SET PARAMETERS  "));

  confirmVariable(lastSelectedElementInRollupParametersList, rollupParametersList, sizeOfRollupParametersList);
  if(choiceIsConfirmed()){
    switch(selectedElement){
      case 2: lastSelectedElementInRollupParametersList = selectedElement; display(rollupId(nb, SETSTAGES));break;
      case 1: lastSelectedElementInRollupParametersList = selectedElement; display(rollupId(nb, ROTUP));break;
      case 0: lastSelectedElementInRollupParametersList = sizeOfRollupParametersList;
              greenhouse.rollup[nb].autoCalibration();
              display(genericId(HOME));break;
    }
  }
}

void enableRollup(int nb){
  if(greenhouse.rollup[nb].isActivated()){
    printHeader(rollupHeader(nb),F("- DISABLE(CONFIRM)"));
    confirmVariable(1, confirmList, sizeOfConfirmList);

    if(choiceIsConfirmed()){
      if(selectedElement == 0 && nextIsConfirmed()){
        greenhouse.rollup[nb].desactivateDevice();greenhouse.rollup[nb].disableOverrides();
      }
      display(genericId(HOME));
    }
  }
  else{
    printHeader(rollupHeader(nb),F("- ENABLE(CONFIRM)"));
    confirmVariable(1, confirmList, sizeOfConfirmList);

    if(choiceIsConfirmed()){
      if(selectedElement == 0 && nextIsConfirmed()){
        resetMenuCount();
        display(deviceId(nb,TYPE));
      }
      else{
        display(genericId(HOME));
      }
    }
  }
}

void protectionRollup(int nb){
    printHeader(rollupHeader(nb),F(" - PROTECTIONS"));
    if(greenhouse.anemometer.value() == OFF_WIND){disableMenuElement(3);}
    if(greenhouse.rainSensor.value() == OFF_RAIN){disableMenuElement(2);}
    if(greenhouse.outsideTempSensor.value() == OFF_TEMP){disableMenuElement(2);}
    confirmVariable(lastSelectedElementInProtectionsRollupList, protectionsRollupList, sizeOfProtectionsRollupList);
    if(choiceIsConfirmed()){
      switch (selectedElement) {
      case 3: display(rollupId(nb,WIND1));break;
      case 2: display(rollupId(nb,RAIN1));break;
      case 1: display(rollupId(nb,OUTSIDE1));break;
      case 0:display(rollupId(nb,BREAKER));break;
      }
      lastSelectedElementInProtectionsRollupList = selectedElement;
    }
}


void overridesRollup(int nb){
  printHeader(rollupHeader(nb),F(" - OVERRIDE       "));
  if(!greenhouse.rollup[nb].isLock()){
    confirmVariable(lastSelectedElementInOverrideRollupList, overrideRollupList, sizeOfOverrideRollupList);
    if(choiceIsConfirmed()){
      switch (selectedElement) {
        case 2: lastSelectedElementInOverrideRollupList = selectedElement; display(rollupId(nb,FOPEN));break;
        case 1: lastSelectedElementInOverrideRollupList = selectedElement; display(rollupId(nb,FCLOSE));break;
        case 0: lastSelectedElementInOverrideRollupList = selectedElement; display(rollupId(nb,FINC1));break;
      }
    }
  }
  else if(greenhouse.rollup[nb].isActive(EXT_LOCK)){
    lcd.setCursor(0,2);
    lcd.print(F(" [EXTERNAL CONTROL] "));
    if(choiceIsConfirmed()){
      if(nextIsConfirmed()){
        resetMenuCount();
      }
      display(rollupId(nb, MAIN));
    }
  }
  else{
    confirmVariable(lastSelectedElementInOverrideRollupLockList, overrideRollupLockList, sizeOfOverrideRollupLockList);
    if(choiceIsConfirmed()){
      switch (selectedElement) {
        case 0: lastSelectedElementInOverrideRollupLockList = selectedElement; display(rollupId(nb,AUTO));break;
      }
    }
  }
}




byte condition(byte target, byte type){
  if((target > 1 && target < 50)&&(type == VALVTYPE)){
    return 2;
  }
  else if (target == 1 || target == 0){
    return 0;
  }
  else{
    return 1;
  }
}

byte conditionType(byte target){
  if(target == ON){
    return 0;
  }
  else if(target == ON_PULSE){
    return 1;
  }
  else if(target == WEATHERCONDITIONAL){
    return 2;
  }
  else if(target == SOILCONDITIONAL){
    return 3;
  }
  else if(target == UNDERCONDITIONAL){
    return 4;
  }
  else if(target == UNDERDEFCONDITIONAL){
    return 5;
  }
  else if(target == HRCONDITIONAL){
    return 2;
  }
  else if(target == HDEFCONDITIONAL){
    return 3;
  }
  else{
    return 0;
  }
}

void addConditionRollup(int nb){
  printHeader(rollupHeader(nb),F(" - PROGRAM TYPE"));
  if(greenhouse.insideTempSensor.value() != SHT1X_TEMP){disableMenuElement(2);disableMenuElement(3);}
  confirmVariable(conditionType(greenhouse.rollup[nb].ovType(CLOCKOV1+clockOvIndex)), rollupConditionList, sizeOfRollupConditionList);
  if(choiceIsConfirmed()){
    switch (selectedElement) {
      case 0:    greenhouse.rollup[nb].setType(clockOv(clockOvIndex), ON);break;
      case 1:    greenhouse.rollup[nb].setType(clockOv(clockOvIndex), ON_PULSE);break;
      case 2:
        if(greenhouse.rollup[nb].ovType(clockOv(clockOvIndex)) != HRCONDITIONAL){
          greenhouse.rollup[nb].setCondition(clockOv(clockOvIndex), 85);
        }
        greenhouse.rollup[nb].setType(clockOv(clockOvIndex), HRCONDITIONAL);break;
      case 3:
        if(greenhouse.rollup[nb].ovType(clockOv(clockOvIndex)) != HDEFCONDITIONAL){
          greenhouse.rollup[nb].setCondition(clockOv(clockOvIndex), 7);
        }
        greenhouse.rollup[nb].setType(clockOv(clockOvIndex), HDEFCONDITIONAL);break;
      }
      lastSelectedElementInRollupConditionList = selectedElement;
      display(rollupId(nb, FIXOV3));
  }
}

void clockOverrideHumidityRollup(int nb){
  printHeader(rollupHeader(nb),F(" - DESHUM SETP(%)"));
  printAdjustInstructions();
  confirmVariable((unsigned short)1,(unsigned short) greenhouse.rollup[nb].condition(CLOCKOV1+clockOvIndex),(unsigned short)99);

  if(choiceIsConfirmed()){
    greenhouse.rollup[nb].setCondition(clockOv(clockOvIndex), usvariable);
    display(rollupId(nb,FIXOV5));
  }
}

void clockOverrideStartRollup(int nb){
  printHeader(rollupHeader(nb),F(" - PROGRAM PERIOD"));
  printAdjustInstructions();
  confirmVariable(greenhouse.rollup[nb].hourStart(clockOv(clockOvIndex)),greenhouse.rollup[nb].minStart(clockOv(clockOvIndex)), greenhouse.rollup[nb].hourStop(clockOv(clockOvIndex)),greenhouse.rollup[nb].minStop(clockOv(clockOvIndex)), (byte)1);
  if(choiceIsConfirmed()){
    greenhouse.rollup[nb].setHourStart(clockOv(clockOvIndex), svariable1);
    greenhouse.rollup[nb].setMinStart(clockOv(clockOvIndex), svariable2);
    display(rollupId(nb,FIXOV4));
  }
}
void clockOverrideStopRollup(int nb){
  printHeader(rollupHeader(nb),F(" - PROGRAM PERIOD"));
  printAdjustInstructions();
  confirmVariable(greenhouse.rollup[nb].hourStop(clockOv(clockOvIndex)),greenhouse.rollup[nb].minStop(clockOv(clockOvIndex)),greenhouse.rollup[nb].hourStart(clockOv(clockOvIndex)),greenhouse.rollup[nb].minStart(clockOv(clockOvIndex)), (byte) 2);
  if(choiceIsConfirmed()){

      greenhouse.rollup[nb].setHourStop(clockOv(clockOvIndex), svariable1);
      greenhouse.rollup[nb].setMinStop(clockOv(clockOvIndex), svariable2);

      if(greenhouse.rollup[nb].ovType(clockOv(clockOvIndex)) == ON){ display(rollupId(nb,FIXOV5));}
      else {display(rollupId(nb, FIXOV9));}


  }
}

void clockOverrideTargetRollup(int nb){
  printHeader(rollupHeader(nb),F(" - OPEN AT(%)"));
  confirmVariable((byte)0,greenhouse.rollup[nb].overrideTarget(clockOv(clockOvIndex)),(byte)100);
  if(choiceIsConfirmed()){
    greenhouse.rollup[nb].setOverrideTarget(clockOv(clockOvIndex), usvariable);
    if(!greenhouse.rollup[nb].isEnabled(clockOv(clockOvIndex))){
      greenhouse.rollup[nb].enable(clockOv(clockOvIndex));
    }
    displayRollupMenuSequence(nb, MAIN, PROGRAM);
  }
}
void clockOverrideRollup(int nb){
  printHeader(rollupHeader(nb),F(" - PROGRAM"));
  lcd.setCursor(13,0);
  lcd.print(clockOvIndex+1);
  confirmVariable(greenhouse.rollup[nb].isEnabled(clockOv(clockOvIndex)), enableList, sizeOfEnableList);
  if(choiceIsConfirmed()){
    switch (selectedElement) {
      case 0: greenhouse.rollup[nb].disable(clockOv(clockOvIndex));displayRollupMenuSequence(nb, MAIN, PROGRAM);break;
      case 1: greenhouse.rollup[nb].enable(clockOv(clockOvIndex));display(rollupId(nb,FIXOV3));break;
    }
  }

}
void clockOverrideHumDeficitRollup(int nb){
  printHeader(rollupHeader(nb),F(" - HUMIDITY(g/m3)"));
  printAdjustInstructions();
  confirmVariable((unsigned short)0,(unsigned short) greenhouse.rollup[nb].condition(CLOCKOV1+clockOvIndex),(unsigned short)30);

  if(choiceIsConfirmed()){
    greenhouse.rollup[nb].setCondition(clockOv(clockOvIndex), usvariable);
    display(rollupId(nb,FIXOV5));
  }
}

void clockOverridePulseOnRollup(int nb){
  printHeader(rollupHeader(nb),F(" - PULSE TIME  "));
  printAdjustInstructions();
  confirmPulseVariable(1,greenhouse.rollup[nb].pulseOnTime(CLOCKOV1+clockOvIndex),240, greenhouse.rollup[nb].pulseOffTime(CLOCKOV1+clockOvIndex),1);
  if(choiceIsConfirmed()){
    greenhouse.rollup[nb].setPulseOnTime(clockOv(clockOvIndex), usvariable);
    display(rollupId(nb, FIXOV10));
  }
}
void clockOverridePulseOffRollup(int nb){
  printHeader(rollupHeader(nb),F(" - PULSE TIME  "));
  printAdjustInstructions();
  confirmPulseVariable(0,greenhouse.rollup[nb].pulseOffTime(CLOCKOV1+clockOvIndex),240,greenhouse.rollup[nb].pulseOnTime(CLOCKOV1+clockOvIndex),2);
  if(choiceIsConfirmed()){
    greenhouse.rollup[nb].setPulseOffTime(clockOv(clockOvIndex), usvariable);
    if(greenhouse.rollup[nb].ovType(clockOv(clockOvIndex)) == HRCONDITIONAL){display(rollupId(nb, FIXOV11));}
    else if(greenhouse.rollup[nb].ovType(clockOv(clockOvIndex)) == HDEFCONDITIONAL){display(rollupId(nb, FIXOV8));}
    else{
      display(rollupId(nb,FIXOV5));
    }
  }
}
void rainRollup(int nb){
  printHeader(rollupHeader(nb),F(" - RAIN OVERRIDE"));
  confirmVariable(greenhouse.rollup[nb].isEnabled(RAINOV), enableList, sizeOfEnableList);
  if(choiceIsConfirmed()){
    switch (selectedElement) {
      case 0: greenhouse.rollup[nb].disable(RAINOV);displayRollupMenuSequence(nb, MAIN, PROTECTIONS);break;
      case 1: greenhouse.rollup[nb].enable(RAINOV);display(rollupId(nb, RAIN2));break;
    }
  }
}
void rainTargetRollup(int nb){
  printHeader(rollupHeader(nb),F(" - RAIN CLOSE(%)"));
  confirmVariable((byte)0,greenhouse.rollup[nb].overrideTarget(RAINOV),(byte)100);
  if(choiceIsConfirmed()){
    greenhouse.rollup[nb].setOverrideTarget(RAINOV, usvariable);
    displayRollupMenuSequence(nb, MAIN, PROTECTIONS);

  }
}

void alarm(){
  printHeader(F("D6 - ALARM MENU   "));
  confirmVariable(lastSelectedElementInAlarmList, mainAlarmList, sizeOfMainAlarmList);
  if(choiceIsConfirmed()){
    switch (selectedElement) {
      case 0: display(deviceId(5,ENABLE));
      lastSelectedElementInAlarmList = 2;break;
      case 1: display(genericId(HIGHTEMPALARM));
      lastSelectedElementInAlarmList = selectedElement;break;
      case 2: display(genericId(LOWTEMPALARM));
      lastSelectedElementInAlarmList = selectedElement;break;
    }
  }
}

  void alarmLow(){
      printHeader(F("   LOW TEMP ALARM   "));
      confirmVariable(greenhouse.lowTempAlarm.value(), enableList, sizeOfEnableList);
      if(choiceIsConfirmed()){
        switch (selectedElement) {
          case 0: greenhouse.lowTempAlarm.setValue(false);displayGenericMenuSequence(ALARM);break;
          case 1: display(genericId(MINTEMP));break;
        }
      }
  }
  void alarmHigh(){
    printHeader(F("  HIGH TEMP ALARM   "));
    confirmVariable(greenhouse.highTempAlarm.value(), enableList, sizeOfEnableList);
    if(choiceIsConfirmed()){
      switch (selectedElement) {
        case 0: greenhouse.highTempAlarm.setValue(false);displayGenericMenuSequence(ALARM);break;
        case 1: display(genericId(MAXTEMP));break;
      }
    }
  }
  void minTemp(){
    printHeader(F(" MIN. TEMPERATURE(C)"));
    confirmVariable((float)-30,greenhouse.minTemp.value(),(float)50);
    if(nextIsConfirmed()){
      greenhouse.lowTempAlarm.setValue(true);
    }
    if(choiceIsConfirmed()){
      greenhouse.minTemp.setValue(fvariable);
      displayGenericMenuSequence(ALARM);
    }
  }
  void maxTemp(){
    printHeader(F(" MAX. TEMPERATURE(C)"));
    confirmVariable((float)-30,greenhouse.maxTemp.value(),(float)50);
    if(nextIsConfirmed()){
      greenhouse.highTempAlarm.setValue(true);
    }
    if(choiceIsConfirmed()){
      greenhouse.maxTemp.setValue(fvariable);
      displayGenericMenuSequence(ALARM);
    }

  }

void windRollup(int nb){
  printHeader(rollupHeader(nb),F(" - WIND OVERRIDE"));
  confirmVariable(greenhouse.rollup[nb].isEnabled(WINDOV), enableList, sizeOfEnableList);
  if(choiceIsConfirmed()){
    switch (selectedElement) {
      case 0: greenhouse.rollup[nb].disable(WINDOV);displayRollupMenuSequence(nb, MAIN, PROTECTIONS);break;
      case 1: greenhouse.rollup[nb].enable(WINDOV);display(rollupId(nb, WIND2));break;
    }
  }
}
void windTargetRollup(int nb){
  printHeader(rollupHeader(nb),F(" - MAX SPEED(km/h)"));
  confirmVariable((unsigned short)11,(unsigned short)greenhouse.rollup[nb].floatVar(WINDOV),(unsigned short)100);
  if(choiceIsConfirmed()){
    greenhouse.rollup[nb].setFloatVar(WINDOV, (float)usvariable);
    displayRollupMenuSequence(nb, MAIN, PROTECTIONS);
  }
}
void coldLockRollup(int nb){
  printHeader(rollupHeader(nb),F(" - COLD LOCK"));
  confirmVariable(greenhouse.rollup[nb].isEnabled(OUTTEMP), enableList, sizeOfEnableList);
  if(choiceIsConfirmed()){
    switch (selectedElement) {
      case 0: greenhouse.rollup[nb].disable(OUTTEMP);displayRollupMenuSequence(nb, MAIN, PROTECTIONS);break;
      case 1: greenhouse.rollup[nb].enable(OUTTEMP);display(rollupId(nb, OUTSIDE2));break;
    }
  }
}

void minTempRollup(int nb){
  printHeader(rollupHeader(nb),F(" - CLOSE AT(C)"));
  confirmVariable((float)-50,greenhouse.rollup[nb].floatVar(OUTTEMP),(float)50);
  if(choiceIsConfirmed()){
    //greenhouse.rollup[nb].setFloatVar(DESHUM, fvariable);
    displayRollupMenuSequence(nb, MAIN, PROTECTIONS);
  }
}

void motorBreaker(int nb){
  printHeader(rollupHeader(nb),F(" - BREAK AT(Amp)"));
  confirmVariable((float)0,greenhouse.rollup[nb].currentLimit.value(),(float)10);
  printDisabledIfZero();
  if(choiceIsConfirmed()){
    greenhouse.rollup[nb].currentLimit.setValue(fvariable);
    displayRollupMenuSequence(nb, MAIN, PROTECTIONS);
  }
}


void autoModeRollup(int nb){
  greenhouse.rollup[nb].unlock();
  display(genericId(HOME));

}
void forceOpenRollup(int nb){
  printHeader(rollupHeader(nb),F(" - LOCK OPEN(min)"));
  printAdjustInstructions();
  confirmVariable((unsigned short)0, (unsigned short)0, (unsigned short)1440);
  zeroEqualInfinity();
  if(nextIsConfirmed()){
    greenhouse.rollup[nb].lockOpenAndWait((unsigned long)usvariable*60);
  }
  if(choiceIsConfirmed()){
    display(genericId(HOME));
  }
}
void forceCloseRollup(int nb){
  printHeader(rollupHeader(nb),F(" - LOCK CLOSE(min)"));
  printAdjustInstructions();
  confirmVariable((unsigned short)0, (unsigned short)0, (unsigned short)1440);
  zeroEqualInfinity();
  if(nextIsConfirmed()){
    greenhouse.rollup[nb].lockCloseAndWait((unsigned long)usvariable*60);
  }
  if(choiceIsConfirmed()){
    display(genericId(HOME));
  }
}
void forceIncRollup1(int nb){
  printHeader(rollupHeader(nb),F(" - LOCK TO %     "));
  printAdjustInstructions();
  confirmVariable( (unsigned short)0, greenhouse.rollup[nb].incrementCounter(), (unsigned short)100);
  if(choiceIsConfirmed()){
    usvariable1 = usvariable;
    display(rollupId(nb,FINC2));
  }
}
void forceIncRollup2(int nb){
  printHeader(rollupHeader(nb),F(" - DELAY (min)    "));
  printAdjustInstructions();
  confirmVariable((unsigned short)0, (unsigned short)0, (unsigned short)1440);
  zeroEqualInfinity();
  if(nextIsConfirmed()){
    greenhouse.rollup[nb].lockAtIncrement(usvariable1, (unsigned long)usvariable*60);
  }
  if(choiceIsConfirmed()){
    display(genericId(HOME));
  }
}

void rotationUpRollup(int nb){
  printHeader(rollupHeader(nb),F("- ROTATION UP(s)"));
  printAdjustInstructions();
  if(nb != 2){
    confirmVariable(0,greenhouse.rollup[nb].rotationUp.value(),MAX_ROTATION_TIME);
    printSelfCalibration();
  }
  else{
    confirmVariable(MIN_ROTATION_TIME,greenhouse.rollup[nb].rotationUp.value(),MAX_ROTATION_TIME);
  }
  if(nextIsConfirmed()){
    if(usvariable == 0){
      greenhouse.rollup[nb].activateDevice();
      greenhouse.rollup[nb].autoCalibration();
      display(genericId(HOME));
    }
    else{
      greenhouse.rollup[nb].rotationUp.setValue(usvariable);
      display(rollupId(nb,ROTDOWN));
    }
  }
  if(backIsConfirmed()||homeIsConfirmed()){
    display(genericId(HOME));
  }
}
void rotationDownRollup(int nb){
  printHeader(rollupHeader(nb),F("- ROTATION DOWN(s)"));
  printAdjustInstructions();
    confirmVariable(MIN_ROTATION_TIME,greenhouse.rollup[nb].rotationDown.value(),MAX_ROTATION_TIME);

    if(choiceIsConfirmed()){

      if(nextIsConfirmed()){
        if(!greenhouse.rollup[nb].isActivated()){
          greenhouse.rollup[nb].activateDevice();
        }
        resetMenuCount();
      }
      greenhouse.rollup[nb].rotationDown.setValue(usvariable);
      display(genericId(HOME));
    }
}

void setStagesRollup(int nb){
  printHeader(rollupHeader(nb),F("- OPENING STEPS   "));
  printAdjustInstructions();
  confirmVariable(MIN_STAGES,greenhouse.rollup[nb].stages.value(),MAX_STAGES-1);
  if(choiceIsConfirmed()){
    greenhouse.rollup[nb].stages.setValue(usvariable);
    if(greenhouse.rollup[nb].stages.value() > 1){
      display(rollupId(nb,S1TARGET));
    }
    else{
      display(rollupId(nb,S2TARGET));
    }
  }
}

void setStage1TargetRollup(int nb){
  printHeader(rollupHeader(nb),F("- MIN OPENING(%)  "));
  printAdjustInstructions();
  confirmVariable(STAGE_TARGET_MINIMUM,greenhouse.rollup[nb].stage[1].target.value(),STAGE_TARGET_MAXIMUM);
  if(choiceIsConfirmed()){
    greenhouse.rollup[nb].stage[1].target.setValue(usvariable);
    display(rollupId(nb,S2TARGET));
  }
}
void setStage2TargetRollup(int nb){
  printHeader(rollupHeader(nb),F("- MAX OPENING(%)  "));
  printAdjustInstructions();
  if(greenhouse.rollup[nb].stages.value() > 1){
    int x = greenhouse.rollup[nb].stages.value();
    confirmVariable(greenhouse.rollup[nb].stage[1].target.value(),greenhouse.rollup[nb].stage[x].target.value(),STAGE_TARGET_MAXIMUM);
    if(choiceIsConfirmed()){
      unsigned short incr = (greenhouse.rollup[nb].stage[x].target.value()-greenhouse.rollup[nb].stage[1].target.value())/(x-1);
      for(int y = 2; y <= greenhouse.rollup[nb].stages.value();y++){
        int incrementMultiplier = x-y;
        greenhouse.rollup[nb].stage[y].target.setValue(usvariable- (incr*incrementMultiplier));
      }
      display(rollupId(nb,S1TEMP));
    }
  }
  else{
    confirmVariable(greenhouse.rollup[nb].stage[0].target.value(),greenhouse.rollup[nb].stage[1].target.value(),STAGE_TARGET_MAXIMUM);
    if(choiceIsConfirmed()){
      greenhouse.rollup[nb].stage[1].target.setValue(usvariable);
      display(rollupId(nb,S2TEMP));
    }
  }
}

void setStage1TempRollup(int nb){
  unsigned int returnid;
  if(greenhouse.rollup[nb].stages.value() > 1){
    returnid = rollupId(nb,S2TEMP);
  }
  else{
    returnid = rollupId(nb,PAUSE);
  }
    printHeader(rollupHeader(nb),F("- START OPENING(C)"));
  confirmVariable(F("coolT:       "), F("start open:  "),MOD_MINIMUM,greenhouse.rollup[nb].stage[1].mod.value(),MOD_MAXIMUM, greenhouse.coolingTemp());
  if(choiceIsConfirmed()){
    greenhouse.rollup[nb].stage[1].mod.setValue(fvariable);
    display(returnid);
  }
}
void setStage2TempRollup(int nb){
  printHeader(rollupHeader(nb),F("- FULL OPENING(C) "));
  if(greenhouse.rollup[nb].stages.value() > 1){
    for(int x = 2; x <= greenhouse.rollup[nb].stages.value();x++){
      if(greenhouse.rollup[nb].stages.value() == x){
        confirmVariable(F("coolT:       "), F("full open:   "),greenhouse.rollup[nb].stage[1].mod.value(),greenhouse.rollup[nb].stage[x].mod.value(),MOD_MAXIMUM, greenhouse.coolingTemp());
        if(choiceIsConfirmed()){
          float incr = (greenhouse.rollup[nb].stage[x].mod.value()-greenhouse.rollup[nb].stage[1].mod.value())/(x-1);
          for(int y = 2; y <= greenhouse.rollup[nb].stages.value();y++){
            int incrementMultiplier = x-y;
            greenhouse.rollup[nb].stage[y].mod.setValue(fvariable- (incr*incrementMultiplier));
          }
          display(rollupId(nb,PAUSE));
        }
      }
    }
  }
  else{
    confirmVariable(F("coolT "), F("full open:   "),MOD_MINIMUM,greenhouse.rollup[nb].stage[1].mod.value(),MOD_MAXIMUM, greenhouse.coolingTemp());
    if(choiceIsConfirmed()){
      greenhouse.rollup[nb].stage[1].mod.setValue(fvariable);
      display(rollupId(nb,PAUSE));
    }
  }
}
void pauseTimeRollup(int nb){
  printHeader(rollupHeader(nb),F("- PAUSE TIME(s)   "));
  printAdjustInstructions();
    confirmVariable(MIN_PAUSE_TIME,greenhouse.rollup[nb].pause.value(),MAX_PAUSE_TIME);

    if(choiceIsConfirmed()){
      greenhouse.rollup[nb].pause.setValue(usvariable);
      display(rollupId(nb,HYST));
    }
}
void hystRollup(int nb){
  printHeader(rollupHeader(nb),F("- HYSTERESIS(C)   "));
  printAdjustInstructions();
  confirmVariable(HYST_MINIMUM,greenhouse.rollup[nb].hyst.value(),HYST_MAXIMUM);
    if(choiceIsConfirmed()){
      greenhouse.rollup[nb].hyst.setValue(fvariable);
      if(nextIsConfirmed()){
        resetMenuCount();
      }
      displayRollupMenuSequence(nb, MAIN, PARAMETERS);

    }
}
//********************DEVICE SPECIFIC MENUS********************

byte weatherElementInList(){
  if(greenhouse.autoWeather.value() == true){
    return 0;
  }
  else if(greenhouse.weatherP.value() == 50){
    return 3;
  }
  else if(greenhouse.weatherP.value() == 100){
    return 4;
  }
  else if(greenhouse.weatherP.value() == 1){
    return 2;
  }
  else{
    return 1;
  }
}




void mainDevice(int nb){
  if(nb < 3 && greenhouse.rollup[nb].isActivated()){
    resetMenuCount();
    display(rollupId(nb,MAIN));
  }
  else if(greenhouse.device[nb].isActivated()){
    printHeader(deviceHeader(nb),F(" - MAIN MENU"));

    if(greenhouse.device[nb].type.value() != VALVTYPE){
      confirmVariable(lastSelectedElementInDeviceList, mainDeviceList, sizeOfMainDeviceList);
      if(choiceIsConfirmed()){
        switch (selectedElement) {
          case 5: display(deviceId(nb,OVERRIDE));lastSelectedElementInDeviceList = selectedElement; break;
          case 4: display(deviceId(nb,STARTTEMP));lastSelectedElementInDeviceList = selectedElement; break;
          case 3: display(deviceId(nb,PROGRAM));lastSelectedElementInDeviceList = selectedElement; break;
          case 2: display(deviceId(nb,ANALOG1));lastSelectedElementInDeviceList = selectedElement; break;
          case 1: display(deviceId(nb,ONTIME));lastSelectedElementInDeviceList = selectedElement; break;
          case 0: display(deviceId(nb,ENABLE));lastSelectedElementInDeviceList = sizeOfMainDeviceList; break;
        }
      }
    }
    else{
      confirmVariable(lastSelectedElementInValvList, mainValvList, sizeOfMainValvList);
      if(choiceIsConfirmed()){
        switch (selectedElement) {
          case 4: display(deviceId(nb,OVERRIDE));lastSelectedElementInValvList = selectedElement; break;
          case 3: display(deviceId(nb,PROGRAM));lastSelectedElementInValvList = selectedElement; break;
          case 2: display(deviceId(nb,ANALOG1));lastSelectedElementInValvList = selectedElement; break;
          case 1: display(deviceId(nb,ONTIME));lastSelectedElementInValvList = selectedElement; break;
          case 0: display(deviceId(nb,ENABLE));lastSelectedElementInValvList = sizeOfMainValvList; break;
        }
      }
    }
  }
  else if(nb == 5 && greenhouse.alarmEnabled.value()== true){
    resetMenuCount();
    display(genericId(ALARM));
  }
  else{
    resetMenuCount();
    display(deviceId(nb,ENABLE));
  }
}

void enableDevice(int nb){

  if(nb == 5 && greenhouse.alarmEnabled.value() == true){
    printHeader(F("  DISABLE ALARM "));
    confirmVariable(1, confirmList, sizeOfConfirmList);

    if(choiceIsConfirmed()){
      if(selectedElement == 0 && nextIsConfirmed()){
        greenhouse.alarmEnabled.setValue(false);
        //release alarm relay
        greenhouse.alarm.start();
      }
      display(genericId(HOME));
    }
  }
  else if(nb < 3 && greenhouse.rollup[nb].isActivated()){
    printHeader(rollupHeader(nb),F("- DISABLE(CONFIRM)"));
    confirmVariable(1, confirmList, sizeOfConfirmList);

    if(choiceIsConfirmed()){
      if(selectedElement == 0 && nextIsConfirmed()){
        greenhouse.rollup[nb].desactivateDevice();greenhouse.rollup[nb].disableOverrides();
      }
      display(genericId(HOME));
    }
  }
  else if(greenhouse.device[nb].isActivated()){
    printHeader(deviceHeader(nb),F("- DISABLE(CONFIRM)"));
    confirmVariable(1, confirmList, sizeOfConfirmList);

    if(choiceIsConfirmed()){
      if(selectedElement == 0 && nextIsConfirmed()){
        greenhouse.device[nb].desactivateDevice();greenhouse.device[nb].disableOverrides();
      }
      display(genericId(HOME));
    }
  }
  else{
    printHeader(deviceHeader(nb),F("- ENABLE(CONFIRM)"));
    confirmVariable(1, confirmList, sizeOfConfirmList);

    if(choiceIsConfirmed()){
      if(selectedElement == 0 && nextIsConfirmed()){
        resetMenuCount();
        display(deviceId(nb,TYPE));
      }
      else{
        display(genericId(HOME));
      }
    }
  }
}

void overridesDevice(int nb){
  printHeader(deviceHeader(nb),F(" - OVERRIDES      "));
  if(!greenhouse.device[nb].isLock()){
    confirmVariable(lastSelectedElementInOverrideDeviceList, overrideDeviceList, sizeOfOverrideDeviceList);
    if(choiceIsConfirmed()){
      switch (selectedElement) {
        case 2: lastSelectedElementInOverrideDeviceList = selectedElement; display(deviceId(nb,FOPEN));break;
        case 1: lastSelectedElementInOverrideDeviceList = selectedElement; display(deviceId(nb,FCLOSE));break;
        case 0: lastSelectedElementInOverrideDeviceList = selectedElement; display(deviceId(nb,PULSE2));break;
      }
    }
  }
  else if(greenhouse.device[nb].isActive(EXT_LOCK)){
    lcd.setCursor(0,2);
    lcd.print(F(" [EXTERNAL CONTROL] "));
    if(choiceIsConfirmed()){
      if(nextIsConfirmed()){
        resetMenuCount();
      }
      display(deviceId(nb, MAIN));
    }
  }
  else{
    confirmVariable(lastSelectedElementInOverrideDeviceLockList, overrideDeviceLockList, sizeOfOverrideDeviceLockList);
    if(choiceIsConfirmed()){
      switch (selectedElement) {
        case 0: lastSelectedElementInOverrideDeviceLockList = selectedElement; display(deviceId(nb,AUTO));break;
      }
    }
  }
}
void printProgram(){

    lcd.setCursor(16,0);
    lcd.print(F("(P"));
    lcd.print(clockOvIndex+1);
    lcd.print(F(")"));
}

void programDevice(int nb){
  printHeader(deviceHeader(nb),F(" - PROGRAMS      "));

  if(clockOvSequence(greenhouse.device[nb], 1,1,1)){
    confirmVariable(lastSelectedElementInProgramsList, programsList123, sizeOfProgramsList);
  }
  else if(clockOvSequence(greenhouse.device[nb], 1,1,0)){
    confirmVariable(lastSelectedElementInProgramsList, programsList12, sizeOfProgramsList);
  }
  else if(clockOvSequence(greenhouse.device[nb], 1,0,1)){
    confirmVariable(lastSelectedElementInProgramsList, programsList13, sizeOfProgramsList);
  }
  else if(clockOvSequence(greenhouse.device[nb], 0,1,1)){
    confirmVariable(lastSelectedElementInProgramsList, programsList23, sizeOfProgramsList);
  }
  else if(clockOvSequence(greenhouse.device[nb], 1,0,0)){
    confirmVariable(lastSelectedElementInProgramsList, programsList1, sizeOfProgramsList);
  }
  else if(clockOvSequence(greenhouse.device[nb], 0,1,0)){
    confirmVariable(lastSelectedElementInProgramsList, programsList2, sizeOfProgramsList);
  }
  else if(clockOvSequence(greenhouse.device[nb], 0,0,1)){
    confirmVariable(lastSelectedElementInProgramsList, programsList3, sizeOfProgramsList);
  }
  else{
    confirmVariable(lastSelectedElementInProgramsList, programsList, sizeOfProgramsList);
  }

  if(choiceIsConfirmed()){
    switch (selectedElement) {
    case 2: clockOvIndex = 0; break;
    case 1: clockOvIndex = 1; break;
    case 0: clockOvIndex = 2; break;
    }
    if(greenhouse.device[nb].isEnabled(clockOv(clockOvIndex))){
      display(deviceId(nb, FIXOV1));
    }
    else{
      display(deviceId(nb, FIXOV2));
    }
  }
}
void pulseDevice(int nb){
  printHeader(deviceHeader(nb),F(" - LOCK PULSE(min)"));
  printAdjustInstructions();
  confirmVariable((unsigned short)0, (unsigned short)0, (unsigned short)1440);
  zeroEqualInfinity();
  if(nextIsConfirmed()){
    greenhouse.device[nb].lockOnAndWait((unsigned long)usvariable*60);
  }
  if(choiceIsConfirmed()){
    display(genericId(HOME));
  }
}
void pulseOnDevice(int nb){
  printHeader(deviceHeader(nb),F(" - PULSE TIME"));
  printAdjustInstructions();
  confirmPulseVariable(1,greenhouse.device[nb].pulseOnTime(LOCK),240,greenhouse.device[nb].pulseOffTime(LOCK),1);
  if(choiceIsConfirmed()){
    greenhouse.device[nb].setPulseOnTime(LOCK, usvariable);
    display(deviceId(nb, PULSE3));
  }
}
void pulseOffDevice(int nb){
  printHeader(deviceHeader(nb),F(" - PULSE TIME"));
  printAdjustInstructions();

  confirmPulseVariable(1,greenhouse.device[nb].pulseOffTime(LOCK),240,greenhouse.device[nb].pulseOnTime(LOCK),2);
  if(choiceIsConfirmed()){
    greenhouse.device[nb].setPulseOffTime(LOCK, usvariable);
    display(deviceId(nb, PULSE1));
  }
}

void clockOverrideDevice(int nb){
  printHeader(deviceHeader(nb),F(" - PROGRAM"));
  lcd.setCursor(13,0);
  lcd.print(clockOvIndex+1);
  confirmVariable(greenhouse.device[nb].isEnabled(clockOv(clockOvIndex)), enableList, sizeOfEnableList);
  if(choiceIsConfirmed()){
    switch (selectedElement) {
      case 0: greenhouse.device[nb].disable(clockOv(clockOvIndex)); displayDeviceMenuSequence(nb, MAIN, PROGRAM);break;
      case 1: greenhouse.device[nb].enable(clockOv(clockOvIndex)); display(deviceId(nb, FIXOV3));break;
    }
  }

}

void addConditionDevice(int nb){
  printHeader(deviceHeader(nb),F(" - PROGRAM TYPE"));
  if(greenhouse.device[nb].type.value() == VALVTYPE){
    if(greenhouse.insideTempSensor.value() != SHT1X_TEMP){disableMenuElement(4);disableMenuElement(5);}
    if(greenhouse.weatheradjust.value() == false){disableMenuElement(2);}
    if(!tensiometerDeviceIsActive()){disableMenuElement(3);}
    confirmVariable(conditionType(greenhouse.device[nb].ovType(clockOv(clockOvIndex))),valvConditionList, sizeOfValvConditionList);
    if(choiceIsConfirmed()){
      switch (selectedElement) {
        case 0:    greenhouse.device[nb].setType(clockOv(clockOvIndex), ON);break;
        case 1:    greenhouse.device[nb].setType(clockOv(clockOvIndex), ON_PULSE);break;
        case 2:   if(greenhouse.device[nb].ovType(clockOv(clockOvIndex)) != WEATHERCONDITIONAL){
                    greenhouse.device[nb].setCondition(clockOv(clockOvIndex), 100);
                  }
                  greenhouse.device[nb].setType(clockOv(clockOvIndex), WEATHERCONDITIONAL);break;
        case 3:   if(greenhouse.device[nb].ovType(clockOv(clockOvIndex)) != SOILCONDITIONAL){
                    greenhouse.device[nb].setCondition(clockOv(clockOvIndex), 15);
                  }
                  greenhouse.device[nb].setType(clockOv(clockOvIndex), SOILCONDITIONAL);break;
        case 4:   if(greenhouse.device[nb].ovType(clockOv(clockOvIndex)) != UNDERCONDITIONAL){
                    greenhouse.device[nb].setCondition(clockOv(clockOvIndex), 50);
                  }
                  greenhouse.device[nb].setType(clockOv(clockOvIndex), UNDERCONDITIONAL);break;
        case 5:   if(greenhouse.device[nb].ovType(clockOv(clockOvIndex)) != UNDERDEFCONDITIONAL){
                    greenhouse.device[nb].setCondition(clockOv(clockOvIndex), 3);
                  }
                  greenhouse.device[nb].setType(clockOv(clockOvIndex), UNDERDEFCONDITIONAL);break;
      }
      lastSelectedElementInValvConditionList = selectedElement;
      display(deviceId(nb, FIXOV3));

    }
  }
  else{
    if(greenhouse.insideTempSensor.value() != SHT1X_TEMP){disableMenuElement(2);disableMenuElement(3);}
    confirmVariable(conditionType(greenhouse.device[nb].ovType(CLOCKOV1+clockOvIndex)), deviceConditionList, sizeOfDeviceConditionList);
    if(choiceIsConfirmed()){
      switch (selectedElement) {
        case 0:    greenhouse.device[nb].setType(clockOv(clockOvIndex), ON);break;
        case 1:    greenhouse.device[nb].setType(clockOv(clockOvIndex), ON_PULSE);break;
        case 2:    if(greenhouse.device[nb].ovType(clockOv(clockOvIndex)) != HRCONDITIONAL){
                    greenhouse.device[nb].setCondition(clockOv(clockOvIndex), 85);
                  }
                   greenhouse.device[nb].setType(clockOv(clockOvIndex), HRCONDITIONAL);break;
        case 3:    if(greenhouse.device[nb].ovType(clockOv(clockOvIndex)) != HDEFCONDITIONAL){
                    greenhouse.device[nb].setCondition(clockOv(clockOvIndex), 7);
                  }
                  greenhouse.device[nb].setType(clockOv(clockOvIndex), HDEFCONDITIONAL);break;
      }
      lastSelectedElementInDeviceConditionList = selectedElement;
      display(deviceId(nb, FIXOV3));
    }
  }
}


void clockOverrideStartDevice(int nb){
  printHeader(deviceHeader(nb),F(" - PROGRAM PERIOD"));
  printAdjustInstructions();
  confirmVariable(greenhouse.device[nb].hourStart(clockOv(clockOvIndex)),greenhouse.device[nb].minStart(clockOv(clockOvIndex)), greenhouse.device[nb].hourStop(clockOv(clockOvIndex)),greenhouse.device[nb].minStop(clockOv(clockOvIndex)), (byte)1);
  if(choiceIsConfirmed()){
    greenhouse.device[nb].setHourStart(clockOv(clockOvIndex), svariable1);
    greenhouse.device[nb].setMinStart(clockOv(clockOvIndex), svariable2);
    display(deviceId(nb,FIXOV4));
  }
}
void clockOverrideStopDevice(int nb){
  printHeader(deviceHeader(nb),F(" - PROGRAM PERIOD"));
  printAdjustInstructions();
  confirmVariable(greenhouse.device[nb].hourStop(clockOv(clockOvIndex)),greenhouse.device[nb].minStop(clockOv(clockOvIndex)),greenhouse.device[nb].hourStart(clockOv(clockOvIndex)),greenhouse.device[nb].minStart(clockOv(clockOvIndex)), (byte) 2);
  if(choiceIsConfirmed()){

      greenhouse.device[nb].setHourStop(clockOv(clockOvIndex), svariable1);
      greenhouse.device[nb].setMinStop(clockOv(clockOvIndex), svariable2);

      if(greenhouse.device[nb].ovType(clockOv(clockOvIndex)) == ON){
          if(!greenhouse.device[nb].isEnabled(clockOv(clockOvIndex))){
            greenhouse.device[nb].enable(clockOv(clockOvIndex));
          }
          displayDeviceMenuSequence(nb, MAIN, PROGRAM);
      }
      else {display(deviceId(nb, FIXOV8));}


  }
}

void clockOverridePulseOnDevice(int nb){
  printHeader(deviceHeader(nb),F(" - PULSE TIME  "));
  printAdjustInstructions();
  confirmPulseVariable(1,greenhouse.device[nb].pulseOnTime(CLOCKOV1+clockOvIndex),240, greenhouse.device[nb].pulseOffTime(CLOCKOV1+clockOvIndex),1);
  if(choiceIsConfirmed()){
    greenhouse.device[nb].setPulseOnTime(clockOv(clockOvIndex), usvariable);
    display(deviceId(nb, FIXOV9));
  }
}
void clockOverridePulseOffDevice(int nb){
  printHeader(deviceHeader(nb),F(" - PULSE TIME  "));
  printAdjustInstructions();
  confirmPulseVariable(0,greenhouse.device[nb].pulseOffTime(CLOCKOV1+clockOvIndex),240,greenhouse.device[nb].pulseOnTime(CLOCKOV1+clockOvIndex),2);
  if(choiceIsConfirmed()){
    greenhouse.device[nb].setPulseOffTime(clockOv(clockOvIndex), usvariable);
    if(greenhouse.device[nb].ovType(clockOv(clockOvIndex)) == WEATHERCONDITIONAL){display(deviceId(nb, FIXOV5));}
    else if(greenhouse.device[nb].ovType(clockOv(clockOvIndex)) == SOILCONDITIONAL){display(deviceId(nb, FIXOV6));}
    else if(greenhouse.device[nb].ovType(clockOv(clockOvIndex)) == HRCONDITIONAL){display(deviceId(nb, FIXOV10));}
    else if(greenhouse.device[nb].ovType(clockOv(clockOvIndex)) == HDEFCONDITIONAL){display(deviceId(nb, FIXOV7));}
    else if(greenhouse.device[nb].ovType(clockOv(clockOvIndex)) == UNDERDEFCONDITIONAL){display(deviceId(nb, FIXOV7));}
    else if(greenhouse.device[nb].ovType(clockOv(clockOvIndex)) == UNDERCONDITIONAL){display(deviceId(nb, FIXOV10));}
    else{
      if(!greenhouse.device[nb].isEnabled(clockOv(clockOvIndex))){
        greenhouse.device[nb].enable(clockOv(clockOvIndex));
      }
      displayDeviceMenuSequence(nb, MAIN, PROGRAM);
    }
  }
}

byte weatherCondition(byte condition){
  if(condition == 1){
    return 0;
  }
  else if(condition == 30){
    return 1;
  }
  else if(condition == 70){
    return 2;
  }
}

void weatherConditional(int nb){
  printHeader(deviceHeader(nb),F(" - WEATHER TYPE"));
  printAdjustInstructions();
  confirmVariable(weatherCondition(greenhouse.device[nb].condition(clockOv(clockOvIndex))), weather2List, sizeOfWeather2List);
  printWeatherAdjustExtremes();

  if(choiceIsConfirmed()){
      switch (selectedElement) {
        case 2: greenhouse.device[nb].setCondition(clockOv(clockOvIndex), 100);break;
        case 1: greenhouse.device[nb].setCondition(clockOv(clockOvIndex), 50);break;
        case 0: greenhouse.device[nb].setCondition(clockOv(clockOvIndex), 1);break;
      }

      if(!greenhouse.device[nb].isEnabled(clockOv(clockOvIndex))){
        greenhouse.device[nb].enable(clockOv(clockOvIndex));
      }
      displayDeviceMenuSequence(nb, MAIN, PROGRAM);
  }
}

void soilMoistureTarget(int nb){
  printHeader(deviceHeader(nb),F(" - SOIL MOIST(kPa)"));
  printAdjustInstructions();
  confirmVariable((unsigned short)1,(unsigned short) greenhouse.device[nb].condition(CLOCKOV1+clockOvIndex),(unsigned short)80);

  if(choiceIsConfirmed()){
    greenhouse.device[nb].setCondition(clockOv(clockOvIndex), usvariable);
    if(tensiometerDevices() > 1){
      display(deviceId(nb, FIXOV11));
    }
    else{
      if(activeTensiometerDevice() != OFF_VAL){
        greenhouse.device[nb].asignTensiometer(activeTensiometerDevice());
      }

      if(!greenhouse.device[nb].isEnabled(clockOv(clockOvIndex))){
        greenhouse.device[nb].enable(clockOv(clockOvIndex));
      }
      displayDeviceMenuSequence(nb, MAIN, PROGRAM);
    }
  }
}

void clockOverrideAssignTensiom(int nb){
  printHeader(deviceHeader(nb),F(" - ASSIGN TENSIOM"));
  printAdjustInstructions();
  for(int x = 0; x < MAX_TENSIOMETERS; x++){
    if(!tensiometerDeviceIsActive(x)){
      disableMenuElement(x);
    }
  }
  confirmVariable(greenhouse.device[nb].tensiometerIndex(), tensiometerList, sizeOfTensiometerList);    

  if(choiceIsConfirmed()){
    greenhouse.device[nb].asignTensiometer(selectedElement);
    if(nextIsConfirmed()){
      if(!greenhouse.device[nb].isEnabled(clockOv(clockOvIndex))){
        greenhouse.device[nb].enable(clockOv(clockOvIndex));
      }
    }
    displayDeviceMenuSequence(nb, MAIN, PROGRAM);
  }
}

void clockOverrideHumDeficitDevice(int nb){
  printHeader(deviceHeader(nb),F(" - HUMIDITY(g/m3)"));
  printAdjustInstructions();
  confirmVariable((unsigned short)0,(unsigned short) greenhouse.device[nb].condition(CLOCKOV1+clockOvIndex),(unsigned short)30);

  if(choiceIsConfirmed()){
    greenhouse.device[nb].setCondition(clockOv(clockOvIndex), usvariable);
    if(!greenhouse.device[nb].isEnabled(clockOv(clockOvIndex))){
      greenhouse.device[nb].enable(clockOv(clockOvIndex));
    }
    displayDeviceMenuSequence(nb, MAIN, PROGRAM);
  }
}

void clockOverrideHumidityDevice(int nb){
  printHeader(deviceHeader(nb),F(" - HUMIDTY SETP(%)"));
  printAdjustInstructions();
  confirmVariable((unsigned short)1,(unsigned short) greenhouse.device[nb].condition(CLOCKOV1+clockOvIndex),(unsigned short)99);

  if(choiceIsConfirmed()){
    greenhouse.device[nb].setCondition(clockOv(clockOvIndex), usvariable);
    displayDeviceMenuSequence(nb, MAIN, PROGRAM);
  }
}

void analogDevice(int nb){
  printHeader(deviceHeader(nb),F(" - ANALOG(V) OVER."));
  confirmVariable(greenhouse.device[nb].isEnabled(ANALOG), enableList, sizeOfEnableList);
  if(choiceIsConfirmed()){
    switch (selectedElement) {
      case 0: greenhouse.device[nb].disable(ANALOG);
      displayDeviceMenuSequence(nb, MAIN);

      break;
      case 1: greenhouse.device[nb].enable(ANALOG);display(deviceId(nb, ANALOG2));break;
    }
  }

}
void analogReferenceDevice(int nb){
  printHeader(deviceHeader(nb),F(" - REFERENCE"));
  confirmVariable(greenhouse.device[nb].isEnabled(ANALOG), referenceList, sizeOfReferenceList);
  if(choiceIsConfirmed()){
    greenhouse.device[nb].setComparator(ANALOG, selectedElement+1);
    display(deviceId(nb, ANALOG3));

  }

}
void analogTargetDevice(int nb){
  printHeader(deviceHeader(nb),F(" - TARGET (V) "));
  printAdjustInstructions();
  confirmVariable((float)0,greenhouse.device[nb].floatVar(ANALOG),(float)5);
  if(choiceIsConfirmed()){
    greenhouse.device[nb].setFloatVar(ANALOG, fvariable);
    display(deviceId(nb, ANALOG4));
  }
}

void analogStateDevice(int nb){
  printHeader(deviceHeader(nb),F(" - DEVICE ACTION "));
  printAdjustInstructions();
  confirmVariable((boolean)greenhouse.device[nb].overrideTarget(ANALOG), stateList, sizeOfStateList);
  if(choiceIsConfirmed()){
    greenhouse.device[nb].setOverrideTarget(ANALOG, selectedElement);
    display(deviceId(nb, ANALOG5));
  }
}

void analogHysteresisDevice(int nb){
  printHeader(deviceHeader(nb),F(" - HYSTERESIS (mV) "));
  printAdjustInstructions();
  confirmVariable((float)0,greenhouse.device[nb].hysteresis(DESHUM),(float)1);
  if(choiceIsConfirmed()){
    greenhouse.device[nb].setHysteresis(ANALOG, fvariable);
    displayDeviceMenuSequence(nb, MAIN);
  }
}

void autoModeDevice(int nb){
  greenhouse.device[nb].unlock();
  display(genericId(HOME));
}

void forceOpenDevice(int nb){
  printHeader(deviceHeader(nb),F(" - LOCK ON (min)  "));
  printAdjustInstructions();
  confirmVariable((unsigned short)0, (unsigned short)0, (unsigned short)1440);
  zeroEqualInfinity();
  if(nextIsConfirmed()){
    greenhouse.device[nb].lockOnAndWait((unsigned long)usvariable*60);
    greenhouse.device[nb].setPulseOffTime(LOCK, 0);
  }
  if(choiceIsConfirmed()){
    display(genericId(HOME));
  }
}

void forceCloseDevice(int nb){
  printHeader(deviceHeader(nb),F(" - LOCK OFF (min)"));
  printAdjustInstructions();
  confirmVariable((unsigned short)0, (unsigned short)0, (unsigned short)1440);
  zeroEqualInfinity();
  if(nextIsConfirmed()){
    greenhouse.device[nb].lockOffAndWait((unsigned long)usvariable*60);
    greenhouse.device[nb].setPulseOffTime(LOCK, 0);
  }
  if(choiceIsConfirmed()){
    display(genericId(HOME));
  }
}

void typeDevice(int nb){
  printHeader(deviceHeader(nb),F("- TYPE"));
  if(nb != 5){disableMenuElement(0);}
  if((nb > 2)||(nb == 2 && (D3.isActivated() || D4.isActivated()) )){disableMenuElement(1);}
  confirmVariable(0, deviceList, sizeOfDeviceList);
  if(choiceIsConfirmed()){
    switch(selectedElement){
      case 0:
        if(nextIsConfirmed()){
          greenhouse.alarmEnabled.setValue(true);
          resetMenuCount();
        }
        display(genericId(HOME));
      break;
      case 1:
        display(rollupId(nb, ROTUP));
      break;
      case 2:case 3:case 4:
        if(nextIsConfirmed()){
          greenhouse.device[nb].type.setValue(selectedElement-2);
          greenhouse.device[nb].activateDevice();
          resetMenuCount();
        }
        display(genericId(HOME));
      break;
    }
    if(selectedElement != 0){
    }
    else{
    }
  }
}

void startTempDevice(int nb){
  printHeader(deviceHeader(nb),F("- START TEMP(C)"));
  if(greenhouse.device[nb].type.value() == FANTYPE){
    confirmVariable(F("coolT:       "), F("start above: "),MOD_MINIMUM,greenhouse.device[nb].mod.value(),MOD_MAXIMUM, greenhouse.coolingTemp());
    if(choiceIsConfirmed()){
      greenhouse.device[nb].mod.setValue(fvariable);
      display(deviceId(nb,STOPTEMP));
    }
  }
  else if(greenhouse.device[nb].type.value() == HEATERTYPE){
    if((greenhouse.daynight.value() == false)&&(otherHeatingDevices(nb) == 0)){
      printAdjustInstructions();
      confirmVariable((float)MIN_TEMPERATURE, greenhouse.heatingTemp()+greenhouse.device[nb].mod.value(), (float)MAX_TEMPERATURE);
      if(choiceIsConfirmed()){
        greenhouse.timepoint[2].heatingTemp.setValue(fvariable);
        greenhouse.device[nb].mod.setValue(0);
        display(deviceId(nb,STOPTEMP));
      }
    }
    else{
      confirmVariable(F("heatT:       "), F("start under: "),HEATER_MOD_MINIMUM,greenhouse.device[nb].mod.value(),HEATER_MOD_MAXIMUM, greenhouse.heatingTemp());
      if(choiceIsConfirmed()){
        greenhouse.device[nb].mod.setValue(fvariable);
        display(deviceId(nb,STOPTEMP));
      }
    }
  }
}
void stopTempDevice(int nb){
  printHeader(deviceHeader(nb),F("- HYSTERESIS(C)   "));
  printAdjustInstructions();
  confirmVariable(HYST_MINIMUM,greenhouse.device[nb].hyst.value(),HYST_MAXIMUM);
  if(choiceIsConfirmed()){
    greenhouse.device[nb].hyst.setValue(fvariable);
    displayDeviceMenuSequence(nb, MAIN);
  }
}
void onTime(int nb){
  printHeader(deviceHeader(nb),F("- ON TIME(min)"));
  lcd.setCursor(0,1);
  lcd.print(F("ON time(24h) : "));
  lcd.print(greenhouse.device[nb].onTime.onTime24h());
  if(choiceIsConfirmed()){
    displayDeviceMenuSequence(nb, MAIN);
  }

}
//********************TIMEPOINTS SPECIFIC MENUS********************

void adjustAndSetTimepoint(Timepoint &timepoint){

    short hour;
    short minut;

    if(timepoint.type.value() == SR){
      hour = svariable1 - (short)Timepoint::sunRise[HOUR];
      minut =  svariable2 - (short)Timepoint::sunRise[MINUT];
    }
    else if(timepoint.type.value() == SS){
      hour = svariable1 - (short)Timepoint::sunSet[HOUR];
      minut = svariable2 - (short)Timepoint::sunSet[MINUT];
    }
    else{
      hour = svariable1;
      minut = svariable2;
    }

    timepoint.setTimepoint(hour, minut);
}

void adjustTimepointAfterTypeChange(Timepoint &timep){

    short hour;
    short minut;

    if(timep.type.value() == SR){
      hour = timep.hr() - (short)Timepoint::sunRise[HOUR];
      minut =  timep.mn() - (short)Timepoint::sunRise[MINUT];
    }
    else if(timep.type.value() == SS){
      hour = timep.hr() - (short)Timepoint::sunSet[HOUR];
      minut = timep.mn() - (short)Timepoint::sunSet[MINUT];
    }
    else{
      hour = timep.hr();
      minut = timep.mn();
    }

    timep.setTimepoint(hour, minut);
    if((timep.nb() == 1)&&(greenhouse.dif.value() == false)){
      greenhouse.timepoint[0].type.setValue(timep.type.value());
      greenhouse.timepoint[0].setTimepoint(hour, minut);
    }
    if((timep.nb() == 3)&&(greenhouse.prenight.value() == false)){
      greenhouse.timepoint[2].type.setValue(timep.type.value());
      greenhouse.timepoint[2].setTimepoint(hour, minut);
    }
}



boolean reference(byte ref){
  if(ref == CLOCK){
    return 0;
  }
  else{
    return 1;
  }
}

const byte BOTH = 0;
const byte FANONLY = 1;
const byte HEATERONLY = 2;

boolean setHeaterTempOnly(){
  if(!coolingDeviceIsActive() && heatingDeviceIsActive()){
    return true;
  }
  return false;
}

boolean setFanTempOnly(){
  if(!heatingDeviceIsActive() && coolingDeviceIsActive()){
    return true;
  }
  return false;
}

boolean setBothTemp(){
  if(!setHeaterTempOnly() && !setFanTempOnly()){
    return true;
  }
  return false;
}

/*Consider :
- which devices are enable
- weatheradjust
- daynight mode
*/
byte nextTimepoint(int nb){
  if(nb == 0){
    return 1;
  }
  else if(nb == 1){
    if(greenhouse.prenight.value() == true){
      return 2;
    }
    else{
      return 3;
    }
  }
  else if(nb == 2){
    return 3;
  }
  else{
    return 1;
  }
}
byte firstTimepoint(){
  if(greenhouse.dif.value() == true){
    return 0;
  }
  else{
    return 1;
  }
}
void adjustTemperature(int nb, byte menu){
  if (menu == HEATINGSUN){
    if(setBothTemp()){
      greenhouse.timepoint[nb].heatingTemp.setValue(fvariable);
    }
    else if(greenhouse.weatheradjust.value() == true){
      greenhouse.timepoint[nb].heatingTemp.setValue(fvariable);
      greenhouse.timepoint[nb].coolingTemp.setValue(fvariable+3);
    }
    else if (greenhouse.daynight.value() == true && nb < 3){
      greenhouse.timepoint[nb].heatingTemp.setValue(fvariable);
      greenhouse.timepoint[nb].coolingTemp.setValue(fvariable+3);
    }
    else{
      greenhouse.timepoint[nb].heatingTemp.setValue(fvariable);
    }
  }

  if (menu == COOLINGSUN){
    greenhouse.timepoint[nb].coolingTemp.setValue(fvariable);
    if(greenhouse.weatheradjust.value() == true && setFanTempOnly()){
      greenhouse.timepoint[nb].heatingTemp.setValue(fvariable-3);
    }
    else if (greenhouse.daynight.value() == true && setFanTempOnly() && nb < 3){
      greenhouse.timepoint[nb].heatingTemp.setValue(fvariable-3);
    }
  }

  if (menu == HEATINGCLOUD){
    if(setBothTemp()){
      greenhouse.timepoint[nb].heatingTempCloud.setValue(fvariable);
    }
    else if(greenhouse.daynight.value() == true && setHeaterTempOnly() && nb < 3){
      greenhouse.timepoint[nb].heatingTempCloud.setValue(fvariable);
      greenhouse.timepoint[nb].coolingTempCloud.setValue(fvariable+3);
    }
    else{
      greenhouse.timepoint[nb].heatingTempCloud.setValue(fvariable);
      greenhouse.timepoint[nb].coolingTempCloud.setValue(fvariable+3);
    }
  }
  if (menu == COOLINGCLOUD){
    greenhouse.timepoint[nb].coolingTempCloud.setValue(fvariable);
    if(greenhouse.daynight.value() == true && setFanTempOnly() && nb < 3){
      greenhouse.timepoint[nb].heatingTempCloud.setValue(fvariable-3);
    }
    else if (setFanTempOnly()){
      greenhouse.timepoint[nb].heatingTempCloud.setValue(fvariable-3);
    }
  }
}
void nextTemperatureMenu(int nb, byte menu){
  //premier menu
  if (menu == MAIN){
    if(setFanTempOnly()){
      display(timepointId(nb, COOLINGSUN));
    }
    else{
      display(timepointId(nb, HEATINGSUN));
    }
  }
  if (menu == HEATINGSUN){
    greenhouse.timepoint[nb].heatingTemp.setValue(fvariable);
    if(setBothTemp()){
      display(timepointId(nb, COOLINGSUN));
    }
    else if(greenhouse.weatheradjust.value() == true){
      greenhouse.timepoint[nb].coolingTemp.setValue(fvariable+3);
      display(timepointId(nb, HEATINGCLOUD));
    }
    else if (greenhouse.daynight.value() == true && nb < 3){
      greenhouse.timepoint[nb].coolingTemp.setValue(fvariable+3);
      display(timepointId(nextTimepoint(nb), HEATINGSUN));
    }
    else{
      displayGenericMenuSequence(CONFIG, TEMPERATURES);
    }
  }

  if (menu == COOLINGSUN){
    greenhouse.timepoint[nb].coolingTemp.setValue(fvariable);
    if(greenhouse.weatheradjust.value() == true && setFanTempOnly()){
      greenhouse.timepoint[nb].heatingTemp.setValue(fvariable-3);
      display(timepointId(nb, COOLINGCLOUD));
    }
    else if (greenhouse.weatheradjust.value() == true && setBothTemp()){
      display(timepointId(nb, HEATINGCLOUD));
    }
    else if (greenhouse.daynight.value() == true && setFanTempOnly() && nb < 3){
      greenhouse.timepoint[nb].heatingTemp.setValue(fvariable-3);
      display(timepointId(nextTimepoint(nb), COOLINGSUN));
    }
    else if (greenhouse.daynight.value() == true && setBothTemp() && nb < 3){
      display(timepointId(nextTimepoint(nb), HEATINGSUN));
    }
    else{
      displayGenericMenuSequence(CONFIG, TEMPERATURES);
    }
  }

  if (menu == HEATINGCLOUD){
    greenhouse.timepoint[nb].heatingTempCloud.setValue(fvariable);

    if(setBothTemp()){
      display(timepointId(nb, COOLINGCLOUD));
    }
    else if(greenhouse.daynight.value() == true && setHeaterTempOnly() && nb < 3){
      greenhouse.timepoint[nb].coolingTempCloud.setValue(fvariable+3);
      display(timepointId(nextTimepoint(nb), HEATINGSUN));
    }
    else{
      greenhouse.timepoint[nb].coolingTempCloud.setValue(fvariable+3);
      displayGenericMenuSequence(CONFIG, TEMPERATURES);
    }
  }
  if (menu == COOLINGCLOUD){
    greenhouse.timepoint[nb].coolingTempCloud.setValue(fvariable);
    if(greenhouse.daynight.value() == true && setFanTempOnly() && nb < 3){
      greenhouse.timepoint[nb].heatingTempCloud.setValue(fvariable-3);
      display(timepointId(nextTimepoint(nb), COOLINGSUN));
    }
    else if(greenhouse.daynight.value() == true && nb < 3){
      display(timepointId(nextTimepoint(nb), HEATINGSUN));
    }
    else if (setFanTempOnly()){
      greenhouse.timepoint[nb].heatingTempCloud.setValue(fvariable-3);
      displayGenericMenuSequence(CONFIG, TEMPERATURES);
    }
    else{
      displayGenericMenuSequence(CONFIG, TEMPERATURES);
    }
  }
}


void mainTimepoint(){
  printHeader(F(" TEMPERATURES MENU  "));
  if(greenhouse.daynight.value() == true){
    confirmVariable(lastSelectedElementInTimepointList, mainTimepointList, sizeOfMainTimepointList);
    if(choiceIsConfirmed()){
      switch(selectedElement){
        case 2:
          lastSelectedElementInTimepointList = 2;
          lastSelectedElementInTimepointBasicList = 1;
          nextTemperatureMenu(firstTimepoint(), MAIN);break;

        case 1:
          display(genericId(PERIODS));
          lastSelectedElementInTimepointList = 1;break;
        case 0:
          lastSelectedElementInTimepointList = 0;
          lastSelectedElementInTimepointBasicList = 0;
          display(genericId(ADJUST));break;
      }
    }
  }
  else{
    confirmVariable(lastSelectedElementInTimepointBasicList, mainTimepointBasicList, sizeOfMainTimepointBasicList);
    if(choiceIsConfirmed()){
      switch(selectedElement){
        case 1:
          lastSelectedElementInTimepointBasicList = 1;
          lastSelectedElementInTimepointList = 1;
          nextTemperatureMenu(firstTimepoint(), MAIN);break;
        case 0:
          lastSelectedElementInTimepointBasicList = 0;
          lastSelectedElementInTimepointList = 0;
          display(genericId(ADJUST));break;
      }
    }

  }
}

void periods(){
  printHeader(F(" PERIODS MENU  "));
  if(greenhouse.prenight.value() && greenhouse.dif.value()){
    confirmVariable(lastSelectedElementInPeriodsList, periodsList, sizeOfPeriodsList);
  }
  else if(!greenhouse.prenight.value() && greenhouse.dif.value()){
    confirmVariable(lastSelectedElementInPeriodsList, periodsListNP, sizeOfPeriodsList);
  }
  else if(greenhouse.prenight.value() && !greenhouse.dif.value()){
    confirmVariable(lastSelectedElementInPeriodsList, periodsListNM, sizeOfPeriodsList);
  }
  else{
    confirmVariable(lastSelectedElementInPeriodsList, periodsListNMP, sizeOfPeriodsList);
  }
  if(choiceIsConfirmed()){
    switch(selectedElement){
    case 3: lastSelectedElementInPeriodsList = 3; display(timepointId(1, STARTTIME)); break;
    case 2: lastSelectedElementInPeriodsList = 2; display(timepointId(3, STARTTIME));break;
    case 1: lastSelectedElementInPeriodsList = 1; display(genericId(SETDIF)); break;
    case 0: lastSelectedElementInPeriodsList = 0; display(genericId(SETPRENIGHT)); break;
    }
  }
}

void startTimepoint(int nb){
  printHeader(printTimepoint(nb),F(" - TEMP PERIOD"));
  printAdjustInstructions();
  if(greenhouse.timepoint[nb].type.value() != CLOCK){
    if(nb < 2){
      printSunrise();
    }
    else{
      printSunset();
    }
  }
  confirmVariable(greenhouse.timepoint[nb].hr(),greenhouse.timepoint[nb].mn(),greenhouse.timepoint[nextTimepoint(nb)].hr(),greenhouse.timepoint[nextTimepoint(nb)].mn(),1);
  if(choiceIsConfirmed()){
    if((nb == 1 && greenhouse.dif.value() == false)||(nb == 3 && greenhouse.prenight.value() == false)){
      adjustAndSetTimepoint(greenhouse.timepoint[nb-1]);
    }
    adjustAndSetTimepoint(greenhouse.timepoint[nb]);
    display(timepointId(nb,RAMPING));
  }
}
void referenceTimepoint(){
  printHeader(F("SUN TRACKING "));
  confirmVariable(reference(greenhouse.timepoint[0].type.value()), enableList, sizeOfEnableList);

  if(choiceIsConfirmed()){
    switch (selectedElement) {
      case 0:
        greenhouse.timepoint[0].type.setValue(CLOCK);
        greenhouse.timepoint[1].type.setValue(CLOCK);
        greenhouse.timepoint[2].type.setValue(CLOCK);
        greenhouse.timepoint[3].type.setValue(CLOCK);
        break;
      case 1:
          greenhouse.timepoint[0].type.setValue(SR);
          greenhouse.timepoint[1].type.setValue(SR);
          greenhouse.timepoint[2].type.setValue(SS);
          greenhouse.timepoint[3].type.setValue(SS);
        break;

    }
    adjustTimepointAfterTypeChange(greenhouse.timepoint[0]);
    adjustTimepointAfterTypeChange(greenhouse.timepoint[1]);
    adjustTimepointAfterTypeChange(greenhouse.timepoint[2]);
    adjustTimepointAfterTypeChange(greenhouse.timepoint[3]);
    displayGenericMenuSequence(CONFIG, TEMPERATURES, ADJUST);
  }
}

void ramping(int nb){
  printHeader(printTimepoint(nb),F(" - RAMPING"));
  printAdjustInstructions();
  confirmVariable(RAMPING_MINIMUM,greenhouse.timepoint[nb].ramping.value(),RAMPING_MAXIMUM);

  if(choiceIsConfirmed()){
    greenhouse.timepoint[nb].ramping.setValue(usvariable);
    displayGenericMenuSequence(CONFIG, TEMPERATURES, PERIODS);
  }
}

void setDif(){
  printHeader(F("DIF (MORNING)     "));

  confirmVariable(greenhouse.dif.value(), enableList, sizeOfEnableList);
  if(choiceIsConfirmed()){
    switch (selectedElement) {
      case 0:
        greenhouse.dif.setValue(false);
        greenhouse.timepoint[0].hrMod.setValue(greenhouse.timepoint[1].hrMod.value());
        greenhouse.timepoint[0].mnMod.setValue(greenhouse.timepoint[1].mnMod.value());
        greenhouse.timepoint[0].updateTimepoint();
        displayGenericMenuSequence(CONFIG, TEMPERATURES, PERIODS);break;
      case 1: greenhouse.dif.setValue(true); display(timepointId(0,STARTTIME));break;
    }
  }
}

void setPrenight(){
  printHeader(F("PRENIGHT           "));

  confirmVariable(greenhouse.prenight.value(), enableList, sizeOfEnableList);
  if(choiceIsConfirmed()){
    switch (selectedElement) {
      case 0:
        greenhouse.prenight.setValue(false);
        greenhouse.timepoint[2].hrMod.setValue(greenhouse.timepoint[3].hrMod.value());
        greenhouse.timepoint[2].mnMod.setValue(greenhouse.timepoint[3].mnMod.value());
        greenhouse.timepoint[2].updateTimepoint();
        displayGenericMenuSequence(CONFIG, TEMPERATURES, PERIODS);break;
      case 1: greenhouse.prenight.setValue(true); display(timepointId(2,STARTTIME));break;
    }
  }
}

void sunHeatTemperature(int nb){
  printHeader(F("  TEMPERATURES(*C)  "),F("TEMPERATURES"), SUN, nb);
  //printAdjustInstructionsShort();
  if((heatingDeviceIsActive()&&coolingDeviceIsActive())||!someDeviceIsActive()){
    confirmVariable(MIN_TEMPERATURE,greenhouse.timepoint[nb].heatingTemp.value(),MAX_TEMPERATURE, greenhouse.timepoint[nb].coolingTemp.value(), greenhouse.timepoint[nb].heatingTempCloud.value(),greenhouse.timepoint[nb].coolingTempCloud.value(), 1);
  }
  else{
    confirmVariable(F("Heating"),MIN_TEMPERATURE,greenhouse.timepoint[nb].heatingTemp.value(),MAX_TEMPERATURE, greenhouse.timepoint[nb].heatingTempCloud.value(), 1);
  }
  if(choiceIsConfirmed()){
    nextTemperatureMenu(nb, HEATINGSUN);
  }
}

void sunCoolTemperature(int nb ){
  printHeader(F("  TEMPERATURES(*C)  "),F("TEMPERATURES"), SUN, nb);
  //printAdjustInstructionsShort();
  if((heatingDeviceIsActive()&&coolingDeviceIsActive())||!someDeviceIsActive()){
    confirmVariable(MIN_TEMPERATURE,greenhouse.timepoint[nb].coolingTemp.value(),MAX_TEMPERATURE, greenhouse.timepoint[nb].heatingTemp.value(), greenhouse.timepoint[nb].heatingTempCloud.value(),greenhouse.timepoint[nb].coolingTempCloud.value(), 2);
  }
  else{
    confirmVariable(F("Cooling"),MIN_TEMPERATURE,greenhouse.timepoint[nb].coolingTemp.value(),MAX_TEMPERATURE, greenhouse.timepoint[nb].coolingTempCloud.value(), 1);
  }

  if(choiceIsConfirmed()){
    nextTemperatureMenu(nb, COOLINGSUN);
  }
}



void cloudHeatTemperature(int nb){
  printHeader(F("  TEMPERATURES(*C)  "),F("TEMPERATURES"), CLOUD, nb);
  //printAdjustInstructionsShort();

  if((heatingDeviceIsActive()&&coolingDeviceIsActive())||!someDeviceIsActive()){
  confirmVariable(MIN_TEMPERATURE,greenhouse.timepoint[nb].heatingTempCloud.value(),MAX_TEMPERATURE, greenhouse.timepoint[nb].heatingTemp.value(), greenhouse.timepoint[nb].coolingTemp.value(),greenhouse.timepoint[nb].coolingTempCloud.value(), 3);
  }
  else{
    confirmVariable(F("Heating"),MIN_TEMPERATURE,greenhouse.timepoint[nb].heatingTempCloud.value(),MAX_TEMPERATURE, greenhouse.timepoint[nb].heatingTemp.value(), 2);
  }
  if(choiceIsConfirmed()){
    nextTemperatureMenu(nb, HEATINGCLOUD);
  }
}

void cloudCoolTemperature(int nb){
  printHeader(F("  TEMPERATURES(*C)  "),F("TEMPERATURES"), CLOUD, nb);
  //printAdjustInstructionsShort();

  if((heatingDeviceIsActive()&&coolingDeviceIsActive())||!someDeviceIsActive()){
  confirmVariable(MIN_TEMPERATURE,greenhouse.timepoint[nb].coolingTempCloud.value(),MAX_TEMPERATURE, greenhouse.timepoint[nb].heatingTemp.value(), greenhouse.timepoint[nb].coolingTemp.value(),greenhouse.timepoint[nb].heatingTempCloud.value(), 4);
  }
  else{
    confirmVariable(F("Cooling"),MIN_TEMPERATURE,greenhouse.timepoint[nb].coolingTempCloud.value(),MAX_TEMPERATURE, greenhouse.timepoint[nb].coolingTemp.value(), 2);
  }
  if(choiceIsConfirmed()){
    nextTemperatureMenu(nb, COOLINGCLOUD);
  }
}

void adjustTimepoint(){
  printHeader(F("   TEMP SETTINGS   "));
  if(greenhouse.daynight.value() == true){
    disableMenuElement(0);
  }
  confirmVariable(lastSelectedElementInAdjustList, adjustTemperaturesList, sizeOfAdjustTemperaturesList);

  if(choiceIsConfirmed()){
    switch (selectedElement) {
      case 2: lastSelectedElementInAdjustList = selectedElement; display(genericId(ENABLEDAYNIGHT));break;
      case 1: lastSelectedElementInAdjustList = selectedElement; display(genericId(WEATHERADJUST));break;
      case 0: lastSelectedElementInAdjustList = selectedElement; display(genericId(REF));break;
    }
  }

}

void adjustTemperatures(){
  printHeader(F("  ADJUST TEMP 24H  "));
  lcd.setCursor(0,1);
  lcd.print(F("Increase/decr. all"));
  lcd.setCursor(0,2);
  lcd.print(F("temp parameters"));
  confirmVariable((float)-5, (float)0, (float)5);
  if(choiceIsConfirmed()){
    T1.coolingTemp.setValue(T1.coolingTemp.value()+ fvariable);
    T1.coolingTempCloud.setValue(T1.coolingTempCloud.value()+ fvariable);
    T1.heatingTemp.setValue(T1.heatingTemp.value()+ fvariable);
    T1.heatingTempCloud.setValue(T1.heatingTempCloud.value()+ fvariable);

    T2.coolingTemp.setValue(T2.coolingTemp.value()+ fvariable);
    T2.coolingTempCloud.setValue(T2.coolingTempCloud.value()+ fvariable);
    T2.heatingTemp.setValue(T2.heatingTemp.value()+ fvariable);
    T2.heatingTempCloud.setValue(T2.heatingTempCloud.value()+ fvariable);

    T3.coolingTemp.setValue(T3.coolingTemp.value()+ fvariable);
    T3.coolingTempCloud.setValue(T3.coolingTempCloud.value()+ fvariable);
    T3.heatingTemp.setValue(T3.heatingTemp.value()+ fvariable);
    T3.heatingTempCloud.setValue(T3.heatingTempCloud.value()+ fvariable);

    T4.coolingTemp.setValue(T4.coolingTemp.value()+ fvariable);
    T4.coolingTempCloud.setValue(T4.coolingTempCloud.value()+ fvariable);
    T4.heatingTemp.setValue(T4.heatingTemp.value()+ fvariable);
    T4.heatingTempCloud.setValue(T4.heatingTempCloud.value()+ fvariable);

    displayGenericMenuSequence(CONFIG, TEMPERATURES, ADJUST);
  }
}
//********************GENERIC MENUS********************

void config(){
  printHeader(F("    MAIN MENU     "));
  confirmVariable(lastSelectedElementInConfigList, configList, sizeOfConfigList);

  if(choiceIsConfirmed()){
    switch (selectedElement) {
      case 4  : display(genericId(TEMPERATURES));break;
      case 3  : display(genericId(HOURSET));break;
      case 2 : display(genericId(SELECTSENSORS));break;
      case 1: display(genericId(TESTS));break;
      case 0: display(genericId(FACTORYSETTINGS));break;
    }
    lastSelectedElementInConfigList = selectedElement;
  }
}

void selectSensor(){
  printHeader(F(" SELECT SENSOR "));
  confirmVariable(lastSelectedElementInSensorList, sensorList, sizeOfSensorList);

  if(choiceIsConfirmed()){
    switch (selectedElement) {
      case 0: display(genericId(TENSIOM3));break;
      case 1: display(genericId(TENSIOM2));break;
      case 2: display(genericId(TENSIOM1));break;
      case 3: display(genericId(CURRENTSENSOR));break;
      case 4: display(genericId(LUXMETER));break;
      case 5: display(genericId(WINDSENSOR));break;
      case 6: display(genericId(RAINSENSOR));break;
      case 7: display(genericId(OUTSIDETEMP));break;
      case 8: display(genericId(INSIDETEMP));break;
    }
    lastSelectedElementInSensorList = selectedElement;
  }
}

void insideTemp(){
  printHeader(F(" INSIDE TEMP SENSOR "));
    confirmVariable(greenhouse.insideTempSensor.value(), tempSensorList, sizeOfTempSensorList-1);

  if(choiceIsConfirmed()){
    switch (selectedElement) {
      //0 = DS18B20
      //1 = SHT1X
      case 0: greenhouse.insideTempSensor.setValue(DS18B20_TEMP);break;
      case 1: greenhouse.insideTempSensor.setValue(SHT1X_TEMP);break;
    }
    display(genericId(INSIDETEMPDATA));
  }
}


void outsideTemp(){
  printHeader(F("OUTSIDE TEMP SENSOR"));
  confirmVariable(greenhouse.outsideTempSensor.value(), tempSensorList, sizeOfTempSensorList);

  if(choiceIsConfirmed()){
    switch (selectedElement) {
      //0 = DS18B20
      //1 = SHT1X
      //2 = off
      case 0: greenhouse.outsideTempSensor.setValue(DS18B20_TEMP); display(genericId(OUTSIDETEMPDATA));break;
      case 1: greenhouse.outsideTempSensor.setValue(SHT1X_TEMP); display(genericId(OUTSIDETEMPDATA));break;
      case 2: greenhouse.outsideTempSensor.setValue(OFF_TEMP);
      displayGenericMenuSequence(CONFIG, SELECTSENSORS);break;
    }
  }
}

void rainsensor(){
  printHeader(F("   RAIN SENSOR   "));
  confirmVariable(greenhouse.rainSensor.value(), rainSensorList, sizeOfRainSensorList);

  if(choiceIsConfirmed()){
    switch (selectedElement) {
      case 0: greenhouse.rainSensor.setValue(NC_CONTACT); display(genericId(WEATHERDATA)); break;
      case 1: greenhouse.rainSensor.setValue(NO_CONTACT);  display(genericId(WEATHERDATA)); break;
      case 2: greenhouse.rainSensor.setValue(RG11_BUCKET); display(genericId(WEATHERDATA)); break;
      case 3: greenhouse.rainSensor.setValue(OFF_RAIN);displayGenericMenuSequence(CONFIG, SELECTSENSORS);break;
    }
  }
}

void windsensor(){
  printHeader(F("   ANEMOMETER   "));
  confirmVariable(greenhouse.anemometer.value(), anemometerList, sizeOfAnemometerList);

  if(choiceIsConfirmed()){
    switch (selectedElement) {
      case 0: greenhouse.anemometer.setValue(SPARKFUN_WIND); display(genericId(WEATHERDATA));break;
      case 1: greenhouse.anemometer.setValue(ANALOG_WIND); display(genericId(WEATHERDATA));break;
      case 2: greenhouse.anemometer.setValue(OFF_WIND); displayGenericMenuSequence(CONFIG, SELECTSENSORS);break;
    }
  }
}

void luxmeter(){
  printHeader(F("     LUX METER    "));
  confirmVariable(greenhouse.luxMeter.value(), enableList, sizeOfEnableList);

  if(choiceIsConfirmed()){
    switch (selectedElement) {
      case 0: greenhouse.luxMeter.setValue(false);  displayGenericMenuSequence(CONFIG, SELECTSENSORS);break;
      case 1: greenhouse.luxMeter.setValue(true); display(genericId(RADIATIONDATA));break;
    }
  }
}
void tensiometer1(){
  printHeader(F("   TENSIOMETER 1  "));
  confirmVariable(greenhouse.tensiometer[0].value(), enableList, sizeOfEnableList);

  if(choiceIsConfirmed()){
    switch (selectedElement) {
      case 0: greenhouse.tensiometer[0].setValue(false);  displayGenericMenuSequence(CONFIG, SELECTSENSORS);break;
      case 1: greenhouse.tensiometer[0].setValue(true); display(genericId(TENSIOMDATA));break;
    }
  }
}
void tensiometer2(){
  printHeader(F("   TENSIOMETER 2  "));
  confirmVariable(greenhouse.tensiometer[1].value(), enableList, sizeOfEnableList);

  if(choiceIsConfirmed()){
    switch (selectedElement) {
      case 0: greenhouse.tensiometer[1].setValue(false);  displayGenericMenuSequence(CONFIG, SELECTSENSORS);break;
      case 1: greenhouse.tensiometer[1].setValue(true); display(genericId(TENSIOMDATA));break;
    }
  }
}
void tensiometer3(){
  printHeader(F("   TENSIOMETER 3  "));
  confirmVariable(greenhouse.tensiometer[2].value(), enableList, sizeOfEnableList);

  if(choiceIsConfirmed()){
    switch (selectedElement) {
      case 0: greenhouse.tensiometer[2].setValue(false);  displayGenericMenuSequence(CONFIG, SELECTSENSORS);break;
      case 1: greenhouse.tensiometer[2].setValue(true); display(genericId(TENSIOMDATA));break;
    }
  }
}
void insidetempdata(){
  reprint = true;
  lcd.setCursor(0,0);
  lcd.print(F("INSIDE"));
  lcd.setCursor(0,1);
  lcd.print(F("T:"));
  if(sensorFailure == false){lcd.print(greenhouseTemperature.value()); lcd.print(F("C"));}
  else{lcd.print(F("!!!   "));}
  lcd.setCursor(0,2);
  lcd.print(F("T12h:"));
  lcd.print((int)greenhouseTemperature.average12h());
  lcd.print(F("C"));
  lcd.setCursor(0,3);
  lcd.print(F("T24h:"));
  lcd.print((int)greenhouseTemperature.average24h());
  lcd.print(F("C"));
  if(greenhouse.insideTempSensor.value() == SHT1X_TEMP){
    lcd.setCursor(8,0);
    lcd.print(F("|RH:"));
    if(sensorFailure == false){lcd.print(greenhouseHumidity.value()); lcd.print(F("%"));}
    else{lcd.print(F("!!!     "));}
    lcd.setCursor(8,1);
    lcd.print(F("|AH:"));
    if(sensorFailure == false){lcd.print((int)greenhouseHumidity.absolute(greenhouseTemperature.value())); lcd.print(F("g/m3"));}
    else{lcd.print(F("!!!     "));}
    lcd.setCursor(8,2);
    lcd.print(F("|HD:"));
    if(sensorFailure == false){lcd.print((int)greenhouseHumidity.deficit(greenhouseTemperature.value())); lcd.print(F("g/m3"));}
    else{lcd.print(F("!!!     "));}
    lcd.setCursor(8,3);
    lcd.print(F("|Dewp:"));
    if(sensorFailure == false){lcd.print(greenhouseHumidity.dew(greenhouseTemperature.value())); lcd.print(F("C"));}
    else{lcd.print(F("!!!     "));}
  }
  if(choiceIsConfirmed()){
    displayGenericMenuSequence(CONFIG, SELECTSENSORS);
  }

}
void outsidetempdata(){
  reprint = true;
  if(greenhouse.outsideTempSensor.value() != OFF_TEMP){
    lcd.setCursor(0,0);
    lcd.print(F("OUTSIDE"));
    lcd.setCursor(0,1);
    lcd.print(F("T:"));
    lcd.print(outsideTemperature.value());
    lcd.print(F("C"));
    lcd.setCursor(0,2);
    lcd.print(F("Min:"));
    lcd.print(outsideTemperature.min24h());
    lcd.print(F("C"));
    lcd.setCursor(0,3);
    lcd.print(F("Max:"));
    lcd.print(outsideTemperature.max24h());
    lcd.print(F("C"));
  }
  if(greenhouse.outsideTempSensor.value() == SHT1X_TEMP){
    lcd.setCursor(8,0);
    lcd.print(F("|RH:"));
    lcd.print(outsideHumidity.value());
    lcd.print(F("%"));
    lcd.setCursor(8,1);
    lcd.print(F("|AH:"));
    lcd.print((int)outsideHumidity.absolute(outsideTemperature.value()));
    lcd.print(F("g/m3"));
    lcd.setCursor(8,2);
    lcd.print(F("|HD:"));
    lcd.print((int)outsideHumidity.deficit(outsideTemperature.value()));
    lcd.print(F("g/m3"));
    lcd.setCursor(8,3);
    lcd.print(F("|Dewp:"));
    lcd.print(greenhouseHumidity.dew(outsideTemperature.value())); lcd.print(F("C"));
  }
  if(choiceIsConfirmed()){
    displayGenericMenuSequence(CONFIG, SELECTSENSORS);
  }
}
void weatherdata(){
  reprint = true;
  printHeader(F("   WEATHER STATION  "));
  lcd.setCursor(0,1);
  if(greenhouse.rainSensor.value() != OFF_RAIN){
    lcd.print(F("Rain: "));
    if(rain == true){
      lcd.print(F("yes"));
    }
    else{
      lcd.print(F("no "));
    }
  }
  if(greenhouse.rainSensor.value() == RG11_BUCKET){
    lcd.setCursor(10,1);
    lcd.print(F("(0:reset)"));
    lcd.setCursor(0,2);
    lcd.print(F("Rain 24h: "));
    lcd.print(totalRainfall);
    lcd.print(F("mm  "));
    if((keyPressed == '0')&&(unpressedTimer > 300)){
      totalRainfall = 0;
      rainSampleCounter = 0;
    }
  }
  if(greenhouse.anemometer.value() != OFF_WIND){
    lcd.setCursor(0,3);
    lcd.print(F("Wind spd: "));
    lcd.print(windSpeed);
    lcd.print(F("km/h  "));
  }
  if(choiceIsConfirmed()){
    displayGenericMenuSequence(CONFIG, SELECTSENSORS);
  }
}
void radiationdata(){
  reprint = true;
  printHeader(F("   RADIATION DATA   "));

  if(greenhouse.luxMeter.value() != false){
    lcd.setCursor(0,1);
    lcd.print(F("Lux : "));
    lcd.print(radiation.lux());
    lcd.setCursor(0,2);
    lcd.print(F("W/m2: "));
    lcd.print(radiation.wattPerMeterSquare());
    lcd.setCursor(0,3);
    lcd.print(F("Joules: "));
    lcd.print(radiation.dayJoules());
  }
  if(choiceIsConfirmed()){
    displayGenericMenuSequence(CONFIG, SELECTSENSORS);
  }
}
void tensiometerdata(){
  reprint = true;
  printHeader(F(" TENSIOMETERS DATA  "));
  for (int x = 0; x < 3;x++){
      if(greenhouse.tensiometer[x].value() == true){
        lcd.setCursor(0,x+1);
        lcd.print(F("T"));
        lcd.print(x+1);
        lcd.print(F(" : "));
        lcd.print(soilMoisture[x].value());
        lcd.print(F(" kPa "));
      }
  }
  if(choiceIsConfirmed()){
    displayGenericMenuSequence(CONFIG, SELECTSENSORS);
  }
}
void currentsensor(){
  reprint = true;
    printHeader(F("   MOTORS CURRENT   "));
      lcd.setCursor(0,1);
      lcd.print(F("R1 : "));
      lcd.print(R1.current());
      lcd.print(F(" A   "));

      lcd.setCursor(0,2);
      lcd.print(F("R2 : "));
      lcd.print(R2.current());
      lcd.print(F(" A   "));

    if(choiceIsConfirmed()){
      displayGenericMenuSequence(CONFIG, SELECTSENSORS);
    }
}

void tests(){
  printHeader(F(" TEST PROGRAMS "));
    confirmVariable(0, testsList, sizeOfTestsList);

  if(nextIsConfirmed()){
    switch (selectedElement) {
      case 0: startTempTest = round(greenhouseTemperature.value()); startHumTest = round(greenhouseHumidity.value()); display(genericId(TESTTEMP));break;
      case 1: checkRelays(); display(genericId(HOME));break;
      case 2: clockTest = TEST_CLOCKF; display(genericId(HOME));break;
      case 3: clockTest = TEST_CLOCKFF; display(genericId(HOME));break;
    }
  }
  if(backIsConfirmed()||homeIsConfirmed()){
    displayGenericMenuSequence(CONFIG, TESTS);
  }
}
byte sensorVariable = 0;

void mockInsideTemp(){
  reprint = true;
  homeDisplay();
  tempSensorTest = TEST_TEMP;
  humSensorTest = TEST_TEMP;

  if(sensorVariable == 0){
    adjustfvariable(-10,startTempTest,50);
    mockGreenhouseTemp =  fvariable;
    printCursor(greenhouseTemperature.value(), fincrement(), 0, 0);
  }
  else if(sensorVariable == 1){
    adjustusvariable(1,(unsigned short)startHumTest,100);
    mockGreenhouseHum =  (float)usvariable;
    printCursor((int)greenhouseHumidity.value(), sincrement(), 6, 0);
  }
  //greenhouse.testRollups(true);
  if(nextIsConfirmed() || backIsConfirmed()){
    if(sensorVariable == 1){
      startTempTest = fvariable;
      line = 0;
      sensorVariable = 0;
    }
    else{
      startHumTest = fvariable;
      line = 0;
      sensorVariable = 1;
    }
  }
  if(homeIsConfirmed()){
    tempSensorTest = OFF;
    humSensorTest = OFF;
    //greenhouse.testRollups(false);
    display(genericId(HOME));
  }
}
void mockInsideHum(){
  reprint = true;
  homeDisplay();
  Serial.println(unpressedTimer);
  adjustfvariable(1,startHumTest,100);
  mockGreenhouseHum =  fvariable;
  printCursor((int)greenhouseHumidity.value(), sincrement(), 6, 0);
  menu = MODE_PROGRAM;
  humSensorTest = TEST_TEMP;
  //greenhouse.testRollups(true);
  if(choiceIsConfirmed()){
    humSensorTest = OFF;
    //greenhouse.testRollups(false);
    display(genericId(HOME));
  }
}


void factorySettings(){
  printHeader(F("  FACTORY SETTINGS  "));
  confirmVariable(1, confirmList, sizeOfConfirmList);

  if(choiceIsConfirmed()){
    switch (selectedElement) {
      case 1: displayGenericMenuSequence(CONFIG);break;
      case 0: EEPROMReset = DEFAULT_SETTINGS; loadParameters();initSensors();display(genericId(HOME));break;
    }
  }

}


void daynight(){
  printHeader(F(" DAY/NIGHT SETTINGS "));

  confirmVariable(greenhouse.daynight.value(), enableList, sizeOfEnableList);
  if(nextIsConfirmed()){
    resetMenuCount();
  }
  if(greenhouse.daynight.value() == false){

    if(choiceIsConfirmed()){

      switch (selectedElement) {
        case 0: greenhouse.daynight.setValue(false);greenhouse.dif.setValue(false);greenhouse.prenight.setValue(false); displayGenericMenuSequence(CONFIG, TEMPERATURES, ADJUST);break;
        case 1: greenhouse.daynight.setValue(true); resetMenuCount();lastSelectedElementInTimepointList = 1;display(genericId(TEMPERATURES));break;
      }
    }
  }
  else{
    if(choiceIsConfirmed()){
      switch (selectedElement) {
        case 0: greenhouse.daynight.setValue(false);greenhouse.dif.setValue(false);greenhouse.prenight.setValue(false);break;
        case 1: greenhouse.daynight.setValue(true); break;
      }
      displayGenericMenuSequence(CONFIG, TEMPERATURES, ADJUST);
    }
  }
}


void weatheradjust(){
  printHeader(F("  WEATHER-SETTINGS  "));

  confirmVariable(greenhouse.weatheradjust.value(), enableList, sizeOfEnableList);
  if(greenhouse.weatheradjust.value() == false){
    if(choiceIsConfirmed()){
      switch (selectedElement) {
        case 0: greenhouse.weatheradjust.setValue(false);
        displayGenericMenuSequence(CONFIG, TEMPERATURES, ADJUST);
        break;
        case 1:
          greenhouse.weatheradjust.setValue(true);
          greenhouse.timepoint[0].coolingTempCloud.setValue(greenhouse.timepoint[0].coolingTemp.value());
          greenhouse.timepoint[1].coolingTempCloud.setValue(greenhouse.timepoint[1].coolingTemp.value());
          greenhouse.timepoint[2].coolingTempCloud.setValue(greenhouse.timepoint[2].coolingTemp.value());
          greenhouse.timepoint[3].coolingTempCloud.setValue(greenhouse.timepoint[3].coolingTemp.value());
          greenhouse.timepoint[0].heatingTempCloud.setValue(greenhouse.timepoint[0].heatingTemp.value());
          greenhouse.timepoint[1].heatingTempCloud.setValue(greenhouse.timepoint[1].heatingTemp.value());
          greenhouse.timepoint[2].heatingTempCloud.setValue(greenhouse.timepoint[2].heatingTemp.value());
          greenhouse.timepoint[3].heatingTempCloud.setValue(greenhouse.timepoint[3].heatingTemp.value());
          if(nextIsConfirmed()){
            resetMenuCount();
          }
          nextTemperatureMenu(firstTimepoint(), MAIN);
          break;
      }
    }
  }
  else{
    if(choiceIsConfirmed()){
      switch (selectedElement) {
        case 0: greenhouse.weatheradjust.setValue(false);break;
        case 1: greenhouse.weatheradjust.setValue(true);break;
      }
      displayGenericMenuSequence(CONFIG, TEMPERATURES, ADJUST);
    }
  }
}



void setWeather(){
  if(greenhouse.weatheradjust.value() == true){
    printHeader(F("    SET WEATHER     "));
    if(greenhouse.luxMeter.value() == false){disableMenuElement(0);}
    confirmVariable(weatherElementInList(), weatherList, sizeOfWeatherList);
    if(nextIsConfirmed()){
      switch (selectedElement) {
        case 4: greenhouse.autoWeather.setValue(false); greenhouse.setWeather(SUN);display(genericId(HOME));break;
        case 3: greenhouse.autoWeather.setValue(false); greenhouse.setWeather(50);display(genericId(HOME));break;
        case 2: greenhouse.autoWeather.setValue(false); greenhouse.setWeather(CLOUD);display(genericId(HOME));break;
        case 1: greenhouse.autoWeather.setValue(false); display(genericId(WEATHERRATIO));break;
        case 0: greenhouse.autoWeather.setValue(true); display(genericId(HOME));break;
      }
    }
    if(backIsConfirmed()||homeIsConfirmed()){
      display(genericId(HOME));
    }
  }
  else{
    resetMenuCount();
    display(genericId(HOME));
  }

}

void weatherRatio(){

    printHeader(F("  WEATHER-ADJUST(%) "));
    printAdjustInstructions();

    confirmVariable(1,greenhouse.weather(),100);
    printWeatherAdjustExtremes();

    if(nextIsConfirmed()){
      greenhouse.setWeather(usvariable);
    }
    if(choiceIsConfirmed()){
      display(genericId(HOME));
    }
}

void hourset(){
  printHeader(F("     SET HOUR      "));
  printAdjustInstructions();
  confirmVariable(0,(unsigned short)greenhouse.rightNow(2),23);
  if(choiceIsConfirmed()){
      hourSet = usvariable;
      if((rightNow[2] != greenhouse.rightNow(2))){
        if(hourSet > 0){
          hourSet -= 1;
        }
        else{
          hourSet = 23;
        }
      }
      rtc.setTime(hourSet, rightNow[1], 0);
    display(genericId(MINSET));
  }
}

void minset(){
  printHeader(F("    SET MINUTS     "));
  printAdjustInstructions();
  confirmVariable(0,(unsigned short)greenhouse.rightNow(1),59);
  if(choiceIsConfirmed()){
      minSet = usvariable;
      rtc.setTime(rightNow[2], minSet, 0);
    display(genericId(DAY));
  }
}
void dayset(){
    printHeader(F("      SET DAY      "));
    printAdjustInstructions();
    unsigned short daySet = (unsigned short)greenhouse.rightNow(3);
    confirmVariable(0,daySet,31);
    if(choiceIsConfirmed()){
        unsigned short year = 2000+(unsigned short)greenhouse.rightNow(5);
        rtc.setDate((byte)usvariable, (byte)greenhouse.rightNow(4), year);
        getDateAndTime();
        greenhouse.setNow(rightNow);
        greenhouse.solarCalculations();
      display(genericId(MONTH));
    }
  }

  void monthset(){
      printHeader(F("     SET MONTH     "));
      printAdjustInstructions();
    unsigned short monthSet = (unsigned short)greenhouse.rightNow(4);
    confirmVariable(1,monthSet,12);
    if(choiceIsConfirmed()){
        unsigned short year = 2000+(unsigned short)greenhouse.rightNow(5);
        rtc.setDate( (byte)greenhouse.rightNow(3),(byte)usvariable, year);
        getDateAndTime();
        greenhouse.setNow(rightNow);
        greenhouse.solarCalculations();
      display(genericId(YEAR));
    }
  }
  void yearset(){
    printHeader(F("     SET YEAR      "));
    printAdjustInstructions();
    unsigned short yearSet = (unsigned short)greenhouse.rightNow(5);
    confirmVariable((unsigned short)2000,2000+yearSet,(unsigned short)2099);
    if(choiceIsConfirmed()){
        unsigned short year = usvariable-2000;
        rtc.setDate( (byte)greenhouse.rightNow(3), (byte)greenhouse.rightNow(4), year);
        getDateAndTime();
        greenhouse.setNow(rightNow);
        greenhouse.solarCalculations();
      display(genericId(SETLATITUDE));
    }
  }

  void latitudeset(){
      printHeader("",F("   SET LATITUDE    "));
      printAdjustInstructions();
        confirmVariable(MIN_LATITUDE,greenhouse.latitude.value(),MAX_LATITUDE);

        if(choiceIsConfirmed()){
          greenhouse.latitude.setValue(fvariable);
          display(genericId(SETLONGITUDE));
        }
  }
  void longitudeset(){
      printHeader("",F("   SET LONGITUDE    "));
      printAdjustInstructions();
        confirmVariable(MIN_LONGITUDE,greenhouse.longitude.value(),MAX_LONGITUDE);

        if(choiceIsConfirmed()){
          greenhouse.longitude.setValue(fvariable);
          display(genericId(SETTIMEZONE));
        }
  }
  void timezoneset(){
      printHeader("",F("  SET TIMEZONE(UTC) "));
      printAdjustInstructions();
        confirmVariable(MIN_TIMEZONE,greenhouse.timezone.value(),MAX_TIMEZONE);
        if(choiceIsConfirmed()){
          greenhouse.timezone.setValue(svariable);
          displayGenericMenuSequence(CONFIG);
        }
  }

struct indexedMenu {
  const byte name;
  void	(*Menu)(int nb);
  };

struct Menu {
  const byte name;
  void	(*Menu)();
};
const indexedMenu	rollupMenus[] = {
//  - name ----    - menu ----------
    {MAIN, mainRollup},
		{ROTUP , rotationUpRollup},
		{ROTDOWN , rotationDownRollup},
		{PAUSE, pauseTimeRollup},
    {HYST, hystRollup},
    {SETSTAGES, setStagesRollup},
		{S1TEMP, setStage1TempRollup},
		{S2TEMP, setStage2TempRollup},
		{S1TARGET, setStage1TargetRollup},
		{S2TARGET, setStage2TargetRollup},

    {AUTO, autoModeRollup},
    {FOPEN, forceOpenRollup},
    {FCLOSE, forceCloseRollup},
    {FINC1, forceIncRollup1},
    {FINC2, forceIncRollup2},
    {OVERRIDE, overridesRollup},
    {FIXOV1, clockOverrideRollup},
    {FIXOV2, addConditionRollup},
    {FIXOV3, clockOverrideStartRollup},
    {FIXOV4, clockOverrideStopRollup},
    {FIXOV5, clockOverrideTargetRollup},
    {FIXOV8, clockOverrideHumDeficitRollup},
    {FIXOV9, clockOverridePulseOnRollup},
    {FIXOV10, clockOverridePulseOffRollup},
    {FIXOV11, clockOverrideHumidityRollup},
    {RAIN1, rainRollup},
    {RAIN2, rainTargetRollup},
    {WIND1, windRollup},
    {WIND2, windTargetRollup},
    {BREAKER, motorBreaker},
    {OUTSIDE1, coldLockRollup},
    {OUTSIDE2, minTempRollup},
    {PARAMETERS, setParametersRollup},
    {PROGRAM, programRollup},
    {PROTECTIONS, protectionRollup},
    {ENABLE, enableRollup},
};

const indexedMenu	deviceMenus[] = {
//  - name ----    - menu ----------
		{MAIN, mainDevice},
		{TYPE, typeDevice},
		{STARTTEMP, startTempDevice},
		{STOPTEMP, stopTempDevice},

		{AUTO, autoModeDevice},
    {FOPEN, forceOpenDevice},
    {FCLOSE, forceCloseDevice},

    {OVERRIDE, overridesDevice},
    {FIXOV1, clockOverrideDevice},
    {FIXOV2, addConditionDevice},
    {FIXOV3, clockOverrideStartDevice},
    {FIXOV4, clockOverrideStopDevice},
    {FIXOV5, weatherConditional},
    {FIXOV6, soilMoistureTarget},
    {FIXOV7, clockOverrideHumDeficitDevice},
    {FIXOV8, clockOverridePulseOnDevice},
    {FIXOV9, clockOverridePulseOffDevice},
    {FIXOV10, clockOverrideHumidityDevice},
    {FIXOV11, clockOverrideAssignTensiom},
    {ANALOG1, analogDevice},
    {ANALOG2, analogReferenceDevice},
    {ANALOG3, analogTargetDevice},
    {ANALOG4, analogStateDevice},
    {ANALOG5, analogHysteresisDevice},
    {PULSE1, pulseDevice},
    {PULSE2, pulseOnDevice},
    {PULSE3, pulseOffDevice},
    {ENABLE, enableDevice},
    {ONTIME, onTime},
    {PROGRAM, programDevice},
};
const indexedMenu	timepointMenus[] = {
//  - name ----    - menu ----------
    {MAIN, disabled},
		{STARTTIME, startTimepoint},
    {HEATINGSUN, sunHeatTemperature},
    {COOLINGSUN, sunCoolTemperature},
    {HEATINGCLOUD, cloudHeatTemperature},
    {COOLINGCLOUD, cloudCoolTemperature},
    {RAMPING, ramping},
    {ENABLE, disabled},
};

const Menu	genericMenus[] = {
//  - name ----    - menu ----------
    {HOME, homeDisplay},
    {CONFIG, config},
    {HOURSET, hourset},
    {MINSET, minset},
    {DAY, dayset},
    {MONTH, monthset},
    {YEAR, yearset},
    {SETLATITUDE, latitudeset},
    {SETLONGITUDE, longitudeset},
    {SETTIMEZONE, timezoneset},
    {REF, referenceTimepoint},
    {WEATHERADJUST, weatheradjust},
    {WEATHER, setWeather},
    {WEATHERRATIO, weatherRatio},
    {ENABLEDAYNIGHT, daynight},
    {PERIODS, periods},
    {TESTS, tests},
    {TESTTEMP, mockInsideTemp},

    {TEMPERATURES, mainTimepoint},
    {SELECTSENSORS, selectSensor},
    {INSIDETEMPSENSOR, disabled},
    {FACTORYSETTINGS, factorySettings},
    {ADJUST, adjustTimepoint},
    {ADJUST24H, adjustTemperatures},
    {ADJUSTVR, disabled},
    {ENTERCODE, disabled},
    {SETDIF, setDif},
    {SETPRENIGHT, setPrenight},
    {INSIDETEMP,insideTemp},
    {OUTSIDETEMP,outsideTemp},
    {RAINSENSOR,rainsensor},
    {WINDSENSOR,windsensor},
    {LUXMETER,luxmeter},
    {TENSIOM1,tensiometer1},
    {TENSIOM2,tensiometer2},
    {TENSIOM3,tensiometer3},
    {INSIDETEMPDATA,insidetempdata},
    {OUTSIDETEMPDATA,outsidetempdata},
    {WEATHERDATA,weatherdata},
    {RADIATIONDATA,radiationdata},
    {RADIATIONDATA,radiationdata},
    {CURRENTSENSOR,currentsensor},
    {TENSIOMDATA,tensiometerdata},
    {ALARM,alarm},
    {LOWTEMPALARM,alarmLow},
    {MINTEMP,minTemp},
    {HIGHTEMPALARM,alarmHigh},
    {MAXTEMP,maxTemp},

};
//EXECUTE ACTIVE MENU PROGRAM
void printRollup( int nb, byte menuName){
  menu = MODE_PROGRAM;
  int	numMenus = sizeof(rollupMenus) / sizeof(indexedMenu);

  for (int i = 0; i < numMenus; i++) {
    if ( menuName == rollupMenus[i].name) {
      if(reprint == true){
        reprint = false;
        rollupMenus[i].Menu(nb);
      }
    }
  }
}
void printDevice( int nb, byte menuName){
  menu = MODE_PROGRAM;

  int	numMenus = sizeof(deviceMenus) / sizeof(indexedMenu);

  for (int i = 0; i < numMenus; i++) {
    if ( menuName == deviceMenus[i].name) {
      if(reprint == true){
        reprint =false;
        deviceMenus[i].Menu(nb);
      }
    }
  }
}

void printTimepoint( int nb, byte menuName){
  menu = MODE_PROGRAM;

  int	numMenus = sizeof(timepointMenus) / sizeof(indexedMenu);

  for (int i = 0; i < numMenus; i++) {
    if ( menuName == timepointMenus[i].name) {
      if(reprint == true){
        reprint = false;
        timepointMenus[i].Menu(nb);
      }
    }
  }
}

void printGeneric(byte menuName){
  menu = MODE_PROGRAM;

  int	numMenus = sizeof(genericMenus) / sizeof(Menu);

  for (int i = 0; i < numMenus; i++) {
    if (menuName == genericMenus[i].name) {
      if(reprint == true){
        reprint = false;
        genericMenus[i].Menu();
      }
    }
  }
}

void printMenu(unsigned short id){
  	int th,h,t,u;
    u=id%100;
    h=(id/100)%10;
    th=id/1000;
    switch(th){
      case (ROLLUP_MENU_CONSTANT/1000) :printRollup(h,(u));break;
      case (DEVICE_MENU_CONSTANT/1000) :printDevice(h,(u));break;
      case (GENERIC_MENU_CONSTANT/1000) :printGeneric(u);break;
      case (TIMEPOINT_MENU_CONSTANT/1000) :printTimepoint(h,(u));break;
    }

}





void selectActiveKey(){
 #ifdef KEYPAD_DISPLAY
 keyPressed = keypad.getKey();

 if(keypad.getState() == HOLD){
    lcd.noBlink();
   if((fastScrollTimer < 300)|| (keyPressed != '*' && keyPressed != '#')){
     return;
   }
   else{
     fastScrollTimer = 0;
   }
 }
 switch(keyPressed){
   case '1' : updateScreen();resetMenuCount();display(deviceId(0,MAIN));break;
   case '2' : updateScreen();resetMenuCount();display(deviceId(1,MAIN));break;
   case '3' : updateScreen();resetMenuCount();display(deviceId(2,MAIN));break;
   case '4' : updateScreen();resetMenuCount();if(!R3.isActivated()){display(deviceId(3,MAIN));}break;
   case '5' : updateScreen();resetMenuCount();display(deviceId(4,MAIN));break;
   case '6' : updateScreen();resetMenuCount();display(deviceId(5,MAIN));break;
   case '9' : updateScreen();resetMenuCount();display(genericId(WEATHER));break;
   case '*' : updateScreen(); line--;break;
   case '0' : updateScreen(); jumpIncrements();break;
   case '#' : updateScreen(); line++;break;
   case 'A' : updateScreen();break;
   case 'B' : updateScreen();resetMenuCount();display(genericId(CONFIG));break;
   case 'C' : updateScreen();break;
   case 'D' : updateScreen();break;
 }
 resetTimerBeforeRefresh();
 #endif
}

void lcdDisplay(){
    updateFlashCounter();

   #ifdef KEYPAD_DISPLAY
    selectActiveKey();
    if(menuID == genericId(HOME)){
      resetMenuCount();
      homeDisplay();
      refreshScreen(60);
    }
    else{
      printMenu(menuID);
      if(keypad.getState() == HOLD){
         lcd.noBlink();
       }
      refreshScreen(300);
    }
  #endif

  #ifndef KEYPAD_DISPLAY

    resetMenuCount();
    homeDisplay();
  #endif
}
