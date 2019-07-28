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
#define SET_PARAMETER     21
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
unsigned int menuID;
unsigned int lastMenuID[30];
unsigned int menuPosition = 0;

char key = '1';
short menu = MODE_DISPLAY;
boolean firstPrint = true;
boolean fastPlus = false;
boolean fastMinus = false;
const byte Code_lenght = 5; // Give enough room for 4 chars + NULL char
char Data[Code_lenght] = "0000"; // 4 is the number of chars it can hold + the null char = 5

elapsedMillis unpressedTimer;
elapsedMillis pressedTimer;

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
const byte S3TEMP= 30;
const byte S4TEMP= 40;
const byte S1TARGET= 11;
const byte S2TARGET= 21;
const byte S3TARGET= 31;
const byte S4TARGET= 41;

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
const byte AUTO= 50;
const byte FOPEN = 51;
const byte FCLOSE = 52;
const byte FINC1 = 53;
const byte FINC2 = 54;

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
const byte PROGRAM = 77;




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

const byte LOWTEMPALARM = 78;
const byte HIGHTEMPALARM = 94;
const byte MAXTEMP= 79;
const byte MINTEMP= 80;

const byte STARTTIME = 1;
const byte HEATINGSUN = 3;
const byte COOLINGSUN = 4;
const byte HEATINGCLOUD = 5;
const byte COOLINGCLOUD = 6;
const byte RAMPING = 7;

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

boolean choiceIsConfirmed(){
  if(nextIsConfirmed() || backIsConfirmed()){
    return true;
  }
  else{
    return false;
  }
}

void resetMenuCount(){
  menuPosition = 0;menuID = 5000;
}

void display(unsigned short id){
  if(menu == MODE_DISPLAY){
    clearMenu();
    resetMenuCount();
    lastMenuID[menuPosition] = menuID;
    menuID = id;
    menuPosition++;
  }
  else if(backIsConfirmed()){
    clearMenu();
    menuPosition--;
    menuID = lastMenuID[menuPosition];
  }
  else{
    clearMenu();
    lastMenuID[menuPosition] = menuID;
    menuID = id;
    menuPosition++;
  }
}
unsigned int rollupId(unsigned int nb, unsigned int parameter){
  unsigned int fullId = ROLLUP_MENU_CONSTANT+(nb*100)+parameter;
  return fullId;
}
unsigned int stageId(unsigned int rollupnb, unsigned int stagenb, unsigned int parameter){
  unsigned int fullId = ROLLUP_MENU_CONSTANT+(rollupnb*100)+(rollupnb*100)+parameter;
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

void serialPrintDigits(int digits){
  // utility function for digital clock display: prints preceding colon and leading 0
  if(digits < 10){
    Serial.print("0");
  }
}

void lcdPrintDigits(int digits){
  // utility function for digital clock display: prints preceding colon and leading 0
  if((digits < 10)&&(digits >= 0)){
  lcd.print(F("0"));
  }
  lcd.print(digits);
}

const byte CLOCKS = 1;
const byte HUMIDS = 2;
const byte RAINS = 3;
const byte LOCKS = 4;
const byte UPS = 5;
const byte DOWNS = 6;
const byte TARGETS = 7;
const byte WINDS1 = 8;
const byte WINDS2 = 9;
const byte FROSTS = 10;


void initLCD(byte length, byte width){
  lcd.begin(length, width);
  lcd.setBacklightPin(BACKLIGHT_PIN, POSITIVE);
  lcd.setBacklight(HIGH);
  lcd.clear();
  lcd.createChar(CLOCKS,clocks);
  lcd.createChar(HUMIDS,humids);
  lcd.createChar(RAINS,rains);
  lcd.createChar(WINDS1,wind1);
  lcd.createChar(WINDS2,wind2);
  lcd.createChar(FROSTS,frosts);
  lcd.createChar(LOCKS,locks);
  lcd.createChar(UPS,up);
  lcd.createChar(DOWNS,down);
  lcd.createChar(TARGETS,target);
}
void printWeather(byte weather){
  if(greenhouse.weatheradjust.value() == true){
    if(weather >= 1 && weather <= 30){
     lcd.print("(CLD)");
    }
    else if(weather > 30 && weather <= 70){
     lcd.print("(MIX)");
    }
    else{
      lcd.print("(SUN)");
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
void lcdPrintOutput(String item, byte _column, byte _row, OnOffDevice device){

    //Device
  lcd.setCursor(_column, _row); lcd.print(F("      "));
  if(device.isActivated()){
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
    else if(device.isActive(CLOCKOV1)||device.isActive(CLOCKOV2)||device.isActive(CLOCKOV3)){
      lcd.setCursor(_column+5, _row);
      lcd.write((byte)CLOCKS);
    }
    else if(device.isActive(DESHUM)){
      lcd.setCursor(_column+5, _row);
      lcd.write((byte)HUMIDS);
    }
  }
}

void lcdPrintRollups(String side, String opening, String closing, byte _column, byte _row, Rollup rollup){
  lcd.setCursor(_column, _row); lcd.print(F("         "));

  if(rollup.isActivated()){
    lcd.setCursor(_column, _row);
    if(rollup.incrementCounter() == OFF_VAL){lcd.print(side); }
    else if (rollup.opening() == true){lcd.print(opening);}
    else if (rollup.closing() == true){lcd.print(closing);}
    else if(rollup.isWaiting()){
      if(flashingCounter == 0){
        lcd.print(side); lcd.print(rollup.incrementCounter());lcd.print(F("%"));
      }
      else if(flashingCounter == 1){
        lcd.print(F("          "));
      }
    }
    else{lcd.print(side); lcd.print(rollup.incrementCounter());lcd.print(F("%"));}

    if(rollup.isLock()){
        lcd.setCursor(_column+8, _row);
        lcd.write((byte)LOCKS);
    }
    else if(rollup.isActive(CLOCKOV1)||rollup.isActive(CLOCKOV2)||rollup.isActive(CLOCKOV3)){
      lcd.setCursor(_column+8, _row);
      lcd.write((byte)CLOCKS);
    }
    else if(rollup.isEnabled(RAINOV) && rain == true){
      lcd.setCursor(_column+8, _row);
      lcd.write((byte)RAINS);
    }
    else if(rollup.isActive(WINDOV)){
      lcd.setCursor(_column+8, _row);
      if(flashingCounter == 0){
        lcd.write((byte)WINDS1);
      }
      else if(flashingCounter == 1){
        lcd.write((byte)WINDS2);
      }
    }
    else if(rollup.isActive(OUTTEMP)){
      lcd.setCursor(_column+8, _row);
        lcd.write((byte)FROSTS);
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
      if(sensorFailure == false){ lcd.print(F("C-   "));lcd.setCursor(6,_row); lcd.print((int)greenhouseHumidity.value());lcd.print("%");}
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

boolean heatingDeviceIsActive(){
  if(heaterIsActive(0) || heaterIsActive(1)|| heaterIsActive(2)){
    return true;
  }
  else{
    return false;
  }
}

boolean coolingDeviceIsActive(){
    if((fanIsActive(0)||fanIsActive(1)||fanIsActive(2)||fanIsActive(3)||fanIsActive(4)||greenhouse.rollup[0].isActivated()||greenhouse.rollup[1].isActivated()||greenhouse.rollup[2].isActivated())){
      return true;
    }
    else{
      return false;
    }
}
boolean someDeviceIsActive(){
  if(heaterIsActive(0) || heaterIsActive(1)|| heaterIsActive(2) || heaterIsActive(3)|| heaterIsActive(4)||fanIsActive(0)||fanIsActive(1)||fanIsActive(2)||fanIsActive(3)||fanIsActive(4)||greenhouse.rollup[0].isActivated()||greenhouse.rollup[1].isActivated()||greenhouse.rollup[2].isActivated()){
    return true;
  }
  else{
    return false;
  }
}

byte heatingDevices(){
  int x = 0;
  if(heaterIsActive(0)){x++;}
  if(heaterIsActive(1)){x++;}
  if(heaterIsActive(2)){x++;}
  if(heaterIsActive(3)){x++;}
  if(heaterIsActive(4)){x++;}
  return x;
}

byte otherHeatingDevices(int nb){
  int x = 0;
  if(nb == 0){
    if(heaterIsActive(1)){x++;}
    if(heaterIsActive(2)){x++;}
    if(heaterIsActive(3)){x++;}
    if(heaterIsActive(4)){x++;}
  }
  else if(nb == 1){
    if(heaterIsActive(0)){x++;}
    if(heaterIsActive(2)){x++;}
    if(heaterIsActive(3)){x++;}
    if(heaterIsActive(4)){x++;}
  }
  else if(nb == 2){
    if(heaterIsActive(0)){x++;}
    if(heaterIsActive(1)){x++;}
    if(heaterIsActive(3)){x++;}
    if(heaterIsActive(4)){x++;}
  }
  else if(nb == 3){
    if(heaterIsActive(0)){x++;}
    if(heaterIsActive(1)){x++;}
    if(heaterIsActive(2)){x++;}
    if(heaterIsActive(4)){x++;}
  }
  else if(nb == 4){
    if(heaterIsActive(0)){x++;}
    if(heaterIsActive(1)){x++;}
    if(heaterIsActive(2)){x++;}
    if(heaterIsActive(3)){x++;}
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
      lcd.print("C");
    }
  }
  if(heatingDeviceIsActive()){
    if(/*greenhouseTemperature < greenhouse.heatingTemp() ||*/ !coolingDeviceIsActive()){
        lcd.setCursor(10,0);
        lcd.write(TARGETS);
        lcd.print((int)greenhouse.heatingTemp());
        lcd.print("C");
    }
    else/* if(greenhouseTemperature > greenhouse.heatingTemp() && greenhouseTemperature < greenhouse.coolingTemp())*/{
      lcd.setCursor(10,0);
      if(greenhouse.daynight.value() == false){
        lcd.write(TARGETS);
      }
      lcd.print((int)greenhouse.heatingTemp());
      lcd.print("-");
      lcd.print((int)greenhouse.coolingTemp());
      lcd.print("C");
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
}

void lcdPrintOutputs(){

  if(!D4.isActivated()){lcdPrintRollups("R1: ","OPENING", "CLOSING", 0, 2, R1);}
  if(!D5.isActivated()){lcdPrintRollups("R2: ","OPENING", "CLOSING", 10, 2, R2);}
  if(!D2.isActivated()&&!D3.isActivated()){lcdPrintRollups("R3: ","OPENING", "CLOSING", 10, 3, R3);}

  if(D1.isActivated()){
    if(D1.type.value() == FANTYPE){
      lcdPrintOutput("F1:", 0, 3, D1);
    }
    else if(D1.type.value() == HEATERTYPE){
        lcdPrintOutput("H1:", 0, 3, D1);
    }
    else if(D1.type.value() == VALVTYPE){
        lcdPrintOutput("V1:", 0, 3, D1);
    }
  }

  if(!R3.isActivated()){
    if(D2.type.value() == FANTYPE){
      lcdPrintOutput("F2:", 7, 3, D2);
    }
    else if(D2.type.value() == HEATERTYPE){
      lcdPrintOutput("H2:", 7, 3, D2);
    }
    else if(D2.type.value() == VALVTYPE){
      lcdPrintOutput("V2:", 7, 3, D2);
    }
  }
  if(!R3.isActivated()){
    if(D3.type.value() == FANTYPE){
      lcdPrintOutput("F3:", 14, 3, D3);
    }
    else if(D3.type.value() == HEATERTYPE){
      lcdPrintOutput("H3:", 14, 3, D3);
    }
    else if(D3.type.value() == VALVTYPE){
      lcdPrintOutput("V3:", 14, 3, D3);
    }
  }
  if(!R1.isActivated()){
    if(D4.type.value() == FANTYPE){
      lcdPrintOutput("F4:", 0, 2, D4);
    }
    else if(D4.type.value() == HEATERTYPE){
      lcdPrintOutput("H4:", 0, 2, D4);
    }
    else if(D4.type.value() == VALVTYPE){
      lcdPrintOutput("V4:", 0, 2, D4);
    }
  }
  if(!R2.isActivated()){
    if(D5.type.value() == FANTYPE){
      lcdPrintOutput("F5:",14 , 2, D5);
    }
    else if(D5.type.value() == HEATERTYPE){
      lcdPrintOutput("H5:", 14, 2, D5);
    }
    else if(D5.type.value() == VALVTYPE){
      lcdPrintOutput("V5:", 14, 2, D5);
    }
  }
}

void yearRoundStartTime(){
  lcd.clear();
  lcd.setCursor(14,0); lcdPrintDigits(greenhouse.rightNow(3)); lcd.print(F("/")); lcdPrintDigits(greenhouse.rightNow(4));//lcd.print(F("/")); lcdPrintDigits(greenhouse.rightNow(5)); // lcd.print(F(" |")); lcdPrintDigits(greenhouse.rightNow(3)); lcd.print(F("/")); lcdPrintDigits(greenhouse.rightNow(4)); lcd.print(F("/")); lcdPrintDigits(2000+greenhouse.rightNow(5));
  if (greenhouse.dif.value() == true){
    lcd.setCursor(0,0);lcd.print(F("DIF: ")); lcdPrintDigits(T1.hr());lcd.print(F(":"));lcdPrintDigits(T1.mn());

    switch (T1.type.value()){
      case 0: lcd.print("(SR)");break;
      case 1: lcd.print("(CL)");break;
      case 2: lcd.print("(SS)");break;
    }
  }
  lcd.setCursor(0,1);lcd.print(F("DAY: ")); lcdPrintDigits(T2.hr());lcd.print(F(":"));lcdPrintDigits(T2.mn());
  switch (T2.type.value()){
    case 0: lcd.print("(SR)");break;
    case 1: lcd.print("(CL)");break;
    case 2: lcd.print("(SS)");break;
  }
  if (greenhouse.prenight.value() == true){
    lcd.setCursor(0,2);lcd.print(F("PRE: ")); lcdPrintDigits(T3.hr());lcd.print(F(":"));lcdPrintDigits(T3.mn());

    switch (T3.type.value()){
      case 0: lcd.print("(SR)");break;
      case 1: lcd.print("(CL)");break;
      case 2: lcd.print("(SS)");break;
    }
  }
  lcd.setCursor(0,3);lcd.print(F("NGT: ")); lcdPrintDigits(T4.hr());lcd.print(F(":"));lcdPrintDigits(T4.mn());
  switch (T4.type.value()){
    case 0: lcd.print("(SR)");break;
    case 1: lcd.print("(CL)");break;
    case 2: lcd.print("(SS)");break;
  }
}

void homeDisplay(){
  menu = MODE_DISPLAY;
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
  lcd.print("YES");
  lcd.setCursor(0,3);
  lcd.print("NO");

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


void displayMenu(char index){
  clearMenu(); menu = MODE_DISPLAY;key = index;firstPrint = true; unpressedTimer = 0; line = 0;
}

void displayNextProgram(const char* id){
  for(int x = 0; x < Code_lenght;x++){
      Data[x] = id[x];
  }
  clearMenu();
}


void menuProgram(){

    if(firstPrint == true){
      lcd.setCursor(0,0);
      unpressedTimer = 0;
      lcd.clear();
      lcd.print(F("--Enter #Parameter--"));
      firstPrint = false;
    }

    if ((keyPressed != NO_KEY) && (keyPressed != 'A')&&(keyPressed != 'B')&&(keyPressed != 'C')&&(keyPressed != 'D')&&(keyPressed != '*')&&(keyPressed != '#') ) // makes sure a key is actually pressed, equal to (customKey != NO_KEY)
    {
      Data[data_count] = keyPressed; // store char into data array
      lcd.setCursor(data_count+8,2); // move cursor to show each new char
      lcd.blink();
      lcd.print(Data[data_count]); // print char at said cursor
      data_count++; // increment data array by 1 to store new char, also keep track of the number of chars entered
      if(data_count == Code_lenght-1){
        data_count = 0;
        lcd.setCursor(data_count+8,2);
      }
    }

    if((data_count == 0) && (keyPressed == 'D')) // if the array index is equal to the number of expected chars, compare data to master
    {
      menu = SET_PARAMETER;
      clearMenu();
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
  svariable = value + smodif;

  if (line > lastline){
    smodif += sincrement();
  }
  else if (line < lastline){
    smodif -= sincrement();
  }

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

}
void adjustsvariable(short min, short value, short max){
  svariable = value + smodif;

  if (line > lastline){
    smodif += sincrement();
  }
  else if (line < lastline){
    smodif -= sincrement();
  }

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
}
void adjusttimevariable(short min_hour, short hour, short max_hour, short min_min, short min, short max_min){

  svariable1 = hour + smodif1;
  svariable2 = min + smodif2;

  if (line > lastline){
    smodif2 += minincrement();
  }
  else if (line < lastline){
    smodif2 -= minincrement();
  }

  if((svariable2 >= 60)&&(svariable1 >= 0)){
      smodif2 -= 60;
      smodif1 += 1;
  }
  if((svariable2 > 0)&&(svariable1 < 0)){
      smodif2 -= 60;
      smodif1 += 1;
  }
  if((svariable2 < 0)&&(svariable1 > 0)){
      smodif2 += 60;
      smodif1 -= 1;
  }
  if((svariable2 <= -60)&&(svariable1 <= 0)){
      smodif2 += 60;
      smodif1 -= 1;
  }

  if(svariable1 > max_hour){
    svariable1 = max_hour;
    line -= 1;
  }
  else if(svariable1 < min_hour){
    svariable1 = min_hour;
    line += 1;
  }
  if(svariable2 < min_min){
    svariable2 = min_min;
  }
  lastline = line;
}



void adjustfvariable(float min, float value, float max){
  fvariable = value + fmodif;
  if (line > lastline){
    fmodif += fincrement();
  }
  else if (line < lastline){
    fmodif -= fincrement();
  }
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
}



void printHeader(const __FlashStringHelper* variableName){

  if(firstPrint == true){
    lcd.setCursor(0,0);
    lcd.print(F("                   "));
  }
  firstPrint = false;
  codeWithNoDisplay = false;

  lcd.noBlink();
  lcd.setCursor(0,0);
  lcd.print(variableName);
}
void printHeader(String header, const __FlashStringHelper* variableName){

  if(firstPrint == true){
    lcd.setCursor(0,0);
    lcd.print(F("                   "));
  }
  firstPrint = false;
  codeWithNoDisplay = false;

  lcd.setCursor(0,0);
  lcd.print(header);
  lcd.print(variableName);
}
void printHeader(const __FlashStringHelper* longName,const __FlashStringHelper* shortName,  byte weather, byte nb){

  if(firstPrint == true){
    lcd.setCursor(0,0);
    lcd.print(F("                   "));
  }
  firstPrint = false;
  codeWithNoDisplay = false;

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

void printScrollingInstructions(){
  lcd.setCursor(0,1);
  lcd.print(F("*# : scroll"));
  lcd.setCursor(0,2);
  lcd.print(F("D  : select"));
}
void printScrollingInstructionsShort(){
  lcd.setCursor(0,1);
  lcd.print(F("*#:scroll | D:select"));
  lcd.setCursor(0,2);
  lcd.print(F("D  : select"));
}
void printAdjustInstructions(){
  lcd.setCursor(0,1);
  lcd.print(F("*# : adjust"));
  lcd.setCursor(0,2);
  lcd.print(F("D  : confirm/next"));
}
void printAdjustInstructionsShort(){
  lcd.setCursor(0,1);
  lcd.print(F("*#:adjust|D:conf/nxt"));
}
void printConfirmInstructions(){
  lcd.setCursor(0,1);
  lcd.print(F("Press 'D' to confirm"));
  lcd.setCursor(0,2);
  lcd.print(F("Press 'C' to cancel"));
}
void printReviewInstructions(){
  lcd.setCursor(0,1);
  lcd.print(F("Review of parameters"));
  lcd.setCursor(0,2);
  lcd.print(F("is required before "));
  lcd.setCursor(0,3);
  lcd.print(F("activation (press D)"));
}

void printWeatherAdjustExtremes(){
  lcd.setCursor(7,3);
  if(usvariable == 1){
    lcd.print("[CLD]");
  }
  if(usvariable == 100){
    lcd.print("[SUN]");
  }
}

void printDisabledIfZero(){
  lcd.setCursor(5,3);
  if(fvariable == 0){
    lcd.print("[DISABLED]");
  }
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
void confirmVariable(float min, float value, float max){
  adjustfvariable(min,value,max);
  lcd.setCursor(0,3);
  lcd.print(F("      ["));
  lcd.print(fvariable);
  lcd.print(F("]    "));
  printCursor(fvariable, fincrement(),7,3);
}

void confirmVariable(const __FlashStringHelper* header, float min, float value, float max, float ref, byte pos){
  adjustfvariable(min,value,max);/*
  lcd.setCursor(0,1);
  lcd.print("                    ");
  lcd.setCursor(0,2);
  lcd.print("                    ");*/


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
    lcd.setCursor(11,3);
    printCursor(fvariable, fincrement(),8,3);
    lcd.blink();
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
    lcd.print(F("]   "));
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
      lcd.print(F("]   "));
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
    lcd.print(F("]   "));
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
      lcd.print(F("]   "));
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
    lcd.print(F("]   "));
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
      lcd.print(F("]   "));
    }
    printCursor(fvariable, fincrement(),5,3);
    lcd.blink();
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
    lcd.print(F("]   "));
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
      lcd.print(F("]   "));
    }
    printCursor(fvariable, fincrement(),14,3);
    lcd.blink();
  }
}
void confirmVariable(const __FlashStringHelper* header,const __FlashStringHelper* secondLine,  float min, float value, float max, float reference){
  adjustfvariable(min,value,max);
  lcd.setCursor(0,1);
  lcd.print(header);
  lcd.print(reference);lcd.print("C");
  lcd.setCursor(0,2);
  lcd.print(secondLine);lcd.print(reference + fvariable);lcd.print("C");
  lcd.setCursor(0,3);
  lcd.print(F("MOD :       ["));
  if(fvariable >= 0){lcd.print("+");}
  lcd.print(fvariable);
  lcd.print(F("]"));
  printCursor(fvariable, fincrement(),14,3);
  lcd.blink();
}/*
void confirmVariable(const __FlashStringHelper* secondLine,const __FlashStringHelper* thirdLine,  float min, float value, float max, float reference1, float reference2){
  adjustfvariable(min,value,max);
  lcd.setCursor(0,2);
  lcd.print(secondLine);lcd.print(reference + fvariable);lcd.print("C");
  lcd.setCursor(0,3);
  lcd.print(thirdLine);lcd.print(reference + fvariable);lcd.print("C");
}*/
void confirmVariableLESS(const __FlashStringHelper* header,const __FlashStringHelper* secondLine, float min, float value, float max, float reference){
  adjustfvariable(min,value,max);
  lcd.setCursor(0,1);
  lcd.print(header);
  lcd.print(reference);lcd.print("C");
  lcd.setCursor(0,2);
  lcd.print(secondLine);lcd.print(reference - fvariable);lcd.print("C");
  lcd.setCursor(0,3);
  lcd.print(F("MOD :       ["));
  if(fvariable >= 0){lcd.print("-");}
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
void confirmVariable(short min_hour, short hour, short max_hour, short min_min, short min, short max_min){
  adjusttimevariable(min_hour, hour, max_hour, min_min, min, max_min);
  lcd.setCursor(0,3);
  lcd.print(F("     ["));
  lcdPrintDigits(svariable1);
  lcd.print(F("]:["));
  lcdPrintDigits(svariable2);
  lcd.print(F("]   "));
}

void printListElement(String element, byte line){
  //int cursor = (20-(element.length()))/2;
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

char buffer[30];

void confirmVariable(unsigned short typeValue, const char* const item[] PROGMEM, const int menuLenght){
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
    //printListElement(item[prevElement],3);
    strcpy_P(buffer, (char*)pgm_read_word(&(item[prevElement])));
    lcd.setCursor(0,3);
    lcd.print(buffer);

  }
  if(selectedElement != menuLenght-1){
    byte nextElement = selectedElement +1;
    //printListElement(item[nextElement],1);
    strcpy_P(buffer, (char*)pgm_read_word(&(item[nextElement])));
    lcd.setCursor(0,1);
    lcd.print(buffer);
  }

  strcpy_P(buffer, (char*)pgm_read_word(&(item[selectedElement])));
  lcd.setCursor(0,2);
  lcd.print(">");
  lcd.print(buffer);
  //printBrackettedListElement(item[selectedElement],2);

}

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

void setParameter(String header, const __FlashStringHelper* variableName, floatParameter &parameter, unsigned int returnid){
  printHeader(header, variableName);
  printAdjustInstructions();
    confirmVariable(parameter.minimum(),parameter.value(),parameter.maximum());
    if(choiceIsConfirmed()){
      parameter.setValue(fvariable);
      display(returnid);
    }
}
void setParameter(String header,const __FlashStringHelper* variableName, uShortParameter &parameter, unsigned int returnid){
  printHeader(header, variableName);
  printAdjustInstructions();
  confirmVariable(parameter.minimum(),parameter.value(),parameter.maximum());
    if(choiceIsConfirmed()){
      parameter.setValue(usvariable);
      display(returnid);
    }
}
void setParameter(String header,const __FlashStringHelper* variableName, shortParameter &parameter, unsigned int returnid){
  printHeader(header, variableName);
  printAdjustInstructions();
    confirmVariable(parameter.minimum(),parameter.value(),parameter.maximum());
    if(choiceIsConfirmed()){
      parameter.setValue(svariable);
      display(returnid);
    }
}

void setParameter(String header,const __FlashStringHelper* variableName, byteParameter &parameter, unsigned int returnid){
  printHeader(header, variableName);
  printAdjustInstructions();
    confirmVariable(parameter.minimum(),parameter.value(),parameter.maximum());
    if(choiceIsConfirmed()){
      parameter.setValue(svariable);
      display(returnid);
    }
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
    lcd.print("0x");
    if (validAddress[0]<16)
      lcd.print("0");
    lcd.print(validAddress[0],HEX);
  }
  lcd.setCursor(0,2);
  if(validAddress[1] != 0){
    lcd.print("0x");
    if (validAddress[1]<16)
      lcd.print("0");
    lcd.print(validAddress[1],HEX);
  }
  lcd.setCursor(0,3);
  if(validAddress[2] != 0){
    lcd.print("0x");
    if (validAddress[2]<16)
      lcd.print("0");
    lcd.print(validAddress[2],HEX);
  }
  lcd.setCursor(10,1);
  if(validAddress[3] != 0){
    lcd.print("0x");
    if (validAddress[3]<16)
      lcd.print("0");
    lcd.print(validAddress[3],HEX);
  }
  lcd.setCursor(10,2);
  if(validAddress[4] != 0){
    lcd.print("0x");
    if (validAddress[4]<16)
      lcd.print("0");
    lcd.print(validAddress[4],HEX);
  }
  lcd.setCursor(10,3);
  if(validAddress[5] != 0){
    lcd.print("0x");
    if (validAddress[5]<16)
      lcd.print("0");
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

void refreshScreen(byte seconds){
  if(unpressedTimer > (unsigned long) seconds*1000){
    menu = MODE_DISPLAY;
    key = '1';
    clearMenu();
    clearData();
  }
}

void resetTimerBeforeRefresh(){
 if(keyPressed != NO_KEY && keyPressed != 'D' && keyPressed != 'C'){
   unpressedTimer = 0;
 }
}


unsigned int genericId(unsigned int parameter){
  unsigned int fullId = GENERIC_MENU_CONSTANT+parameter;
  return fullId;
}


void disabled(int nb){
  lcd.clear(),  lcd.setCursor(0,0);lcd.print("DISABLED");Serial.println("disabled menu");
}
void disabled(){
  lcd.clear(),  lcd.setCursor(0,0);lcd.print("DISABLED");Serial.println("disabled menu");
}

String header(String text, int nb){
  int index = nb+1;
  return text + index;
}

const char unlock_s[] PROGMEM = "unlock";
const char activate_s[] PROGMEM = "activate";
const char cancel_s[] PROGMEM = "cancel";
const char fan_s[] PROGMEM = "fan";
const char heater_s[] PROGMEM = "heater";
const char valv_s[] PROGMEM = "valv";
const char confirm_s[] PROGMEM = "confirm";
const char enabled_s[] PROGMEM = "enabled";
const char disabled_s[] PROGMEM = "disabled";
const char manual_s[] PROGMEM = "manual";
const char sun_s[] PROGMEM = "sund";

const char* const unlockList[] PROGMEM = {unlock_s};
const int sizeOfUnlockList = sizeof(unlockList)/sizeof(unlockList[0]);
const char* const activateList[] PROGMEM = {activate_s};
const int sizeOfActivateList = sizeof(activateList)/sizeof(activateList[0]);
const char* const confirmList[] PROGMEM = {cancel_s, confirm_s};
const int sizeOfConfirmList = sizeof(confirmList)/sizeof(confirmList[0]);
const char* const deviceList[] PROGMEM = {fan_s,heater_s,valv_s};
const int sizeOfDeviceList = sizeof(deviceList)/sizeof(deviceList[0]);
const char* const enableList[] PROGMEM = {disabled_s,enabled_s};
const int sizeOfEnableList = sizeof(enableList)/sizeof(enableList[0]);
const char* const timeRefList[] PROGMEM = {manual_s,sun_s};
const int sizeOfTimeRefList = sizeof(timeRefList)/sizeof(enableList[0]);

//********************ROLLUP SPECIFIC MENUS********************

const char disable_s[] PROGMEM = "disable";
const char parameters_s[] PROGMEM = "parameters";
const char overrides_s[] PROGMEM = "overrides";
const char rotationtime_s[] PROGMEM = "rotation time";
const char steps_s[] PROGMEM = "steps";
const char breaker_s[] PROGMEM = "breaker";
const char coldlock_s[] PROGMEM = "outside temp";
const char rain_s[] PROGMEM = "rain";
const char wind_s[] PROGMEM =  "wind";
const char timer3_s[] PROGMEM = "timer 3";
const char timer2_s[] PROGMEM = "timer 2";
const char timer1_s[] PROGMEM = "timer 1";
const char lockat_s[] PROGMEM = "lock at %";
const char lockclose_s[] PROGMEM = "lock close";
const char lockopen_s[] PROGMEM = "lock open";

const char* const	mainRollupList[] PROGMEM = {disable_s, parameters_s, overrides_s};
const int sizeOfMainRollupList = sizeof(mainRollupList)/sizeof(mainRollupList[0]);
byte lastSelectedElementInRollupList = 3;

const char* const	rollupParametersList[] PROGMEM= {rotationtime_s, steps_s};
const int sizeOfRollupParametersList = sizeof(rollupParametersList)/sizeof(rollupParametersList[0]);

const char* const	overrideRollupList[] PROGMEM= {breaker_s,coldlock_s,rain_s, wind_s,timer3_s,timer2_s,timer1_s,lockat_s,lockclose_s,lockopen_s};
const int sizeOfOverrideRollupList = sizeof(overrideRollupList)/sizeof(overrideRollupList[0]);

const char* const	overrideRollupLockList[] PROGMEM= {breaker_s,coldlock_s,rain_s,wind_s,timer3_s,timer2_s,timer1_s,unlock_s};
const int sizeOfOverrideRollupLockList = sizeof(overrideRollupLockList)/sizeof(overrideRollupLockList[0]);

void mainRollup(int nb){
  if(greenhouse.rollup[nb].isActivated()){

    printHeader(header(("R"),nb),F(" - MAIN MENU      "));

    confirmVariable(lastSelectedElementInRollupList, mainRollupList, sizeOfMainRollupList);
    if(choiceIsConfirmed()){
      switch (selectedElement) {
        case 2: lastSelectedElementInRollupList = 2;display(rollupId(nb,OVERRIDE));break;
        case 1: lastSelectedElementInRollupList = 1;display(rollupId(nb,PROGRAM));break;
        case 0: lastSelectedElementInRollupList = 2;display(rollupId(nb,ENABLE));break;
      }
    }
  }
  else{
    resetMenuCount();
    display(rollupId(nb,ENABLE));
  }
}

void programRollup(int nb){
  printHeader(header(("R"),nb),F("- SET PARAMETERS  "));

  confirmVariable(1, rollupParametersList, sizeOfRollupParametersList);
  if(choiceIsConfirmed()){
    switch(selectedElement){
      case 1: display(rollupId(nb, SETSTAGES));break;
      case 0: display(rollupId(nb, ROTUP));break;
    }
  }
}

void enableRollup(int nb){
  if(greenhouse.rollup[nb].isActivated()){
    printHeader(header(("R"),nb),F("- DISABLE(CONFIRM)"));
    printConfirmInstructions();

    if(nextIsConfirmed()){
      greenhouse.rollup[nb].desactivateDevice();display(genericId(HOME));
    }
    if(backIsConfirmed()){
      display(genericId(HOME));
    }
  }
  else{
    printHeader(header(("R"),nb),F("- ACTIVATE"));
    printConfirmInstructions();

    if(nextIsConfirmed()){
      greenhouse.rollup[nb].activateDevice();
      resetMenuCount();
      display(rollupId(nb,ROTUP));
    }
    if(backIsConfirmed()){
      display(genericId(HOME));
    }
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
void overridesRollup(int nb){
  printHeader(header(("R"),nb),F(" - OVERRIDE       "));
  if(!greenhouse.rollup[nb].isLock()){
    confirmVariable(9, overrideRollupList, sizeOfOverrideRollupList);
    if(choiceIsConfirmed()){
      switch (selectedElement) {
        case 9: display(rollupId(nb,FOPEN));break;
        case 8: display(rollupId(nb,FCLOSE));break;
        case 7: display(rollupId(nb,FINC1));break;
        case 6: clockOvIndex = 0; display(rollupId(nb,FIXOV1));break;
        case 5: clockOvIndex = 1; display(rollupId(nb,FIXOV1));break;
        case 4: clockOvIndex = 2; display(rollupId(nb,FIXOV1));break;
        case 3: display(rollupId(nb,WIND1));break;
        case 2: display(rollupId(nb,RAIN1));break;
        case 1: display(rollupId(nb,OUTSIDE1));break;
        case 0: display(rollupId(nb,BREAKER));break;
      }
    }
  }
  else{
    confirmVariable(7, overrideRollupLockList, sizeOfOverrideRollupLockList);
    if(choiceIsConfirmed()){
      switch (selectedElement) {
        case 7: display(rollupId(nb,AUTO));break;
        case 6: clockOvIndex = 0; display(rollupId(nb,FIXOV1));break;
        case 5: clockOvIndex = 1; display(rollupId(nb,FIXOV1));break;
        case 4: clockOvIndex = 2; display(rollupId(nb,FIXOV1));break;
        case 3: display(rollupId(nb,WIND1));break;
        case 2: display(rollupId(nb,RAIN1));break;
        case 1: display(rollupId(nb,OUTSIDE1));break;
        case 0: display(rollupId(nb,BREAKER));break;
      }
    }
  }
}

void clockOverrideRollup(int nb){
  printHeader(header(("R"),nb),F(" - CLOCK OVERRIDE"));
  confirmVariable(greenhouse.rollup[nb].isEnabled(clockOv(clockOvIndex)), enableList, sizeOfEnableList);
  if(choiceIsConfirmed()){
    switch (selectedElement) {
      case 0: greenhouse.rollup[nb].disable(clockOv(clockOvIndex));display(genericId(HOME));break;
      case 1: display(rollupId(nb, FIXOV2));break;
    }
  }
}
void clockOverrideStartRollup(int nb){
  printHeader(header(("R"),nb),F(" - START TIME"));
  printAdjustInstructions();
  confirmVariable((short)0,greenhouse.rollup[nb].hourStart(clockOv(clockOvIndex)),(short)23, (short)0,greenhouse.rollup[nb].minStart(clockOv(clockOvIndex)),(short)59);
  if(choiceIsConfirmed()){
    svariable3 = svariable1;
    svariable4 = svariable2;
    display(rollupId(nb,FIXOV3));
  }
}
void clockOverrideStopRollup(int nb){
  printHeader(header(("R"),nb),F(" - STOP TIME"));
  printAdjustInstructions();
  confirmVariable((short)0, greenhouse.rollup[nb].hourStop(clockOv(clockOvIndex)),(short)23, (short)0, greenhouse.rollup[nb].minStop(clockOv(clockOvIndex)), (short)59);
  if(choiceIsConfirmed()){
    display(rollupId(nb,FIXOV4));
  }
}

void clockOverrideTargetRollup(int nb){
  printHeader(header(("R"),nb),F(" - TARGET(%)"));
  confirmVariable((byte)0,greenhouse.rollup[nb].overrideTarget(clockOv(clockOvIndex)),(byte)100);
  if(choiceIsConfirmed()){
    greenhouse.rollup[nb].initOverride(clockOv(clockOvIndex), FIX_R1_PRIORITY+clockOvIndex, svariable3, svariable4, svariable1, svariable2, usvariable);
    display(genericId(HOME));

  }
}
void rainRollup(int nb){
  printHeader(header(("R"),nb),F(" - RAIN OVERRIDE"));
  confirmVariable(greenhouse.rollup[nb].isEnabled(RAINOV), enableList, sizeOfEnableList);
  if(choiceIsConfirmed()){
    switch (selectedElement) {
      case 0: greenhouse.device[nb].disable(RAINOV);display(genericId(HOME));
break;
      case 1: display(rollupId(nb, RAIN2));break;
    }
  }
}
void rainTargetRollup(int nb){
  printHeader(header(("R"),nb),F(" - RAIN CLOSE(%)"));
  confirmVariable((byte)0,greenhouse.rollup[nb].overrideTarget(RAINOV),(byte)100);
  if(choiceIsConfirmed()){
    greenhouse.rollup[nb].initOverride(RAINOV, 2, usvariable);
    display(genericId(HOME));

  }
}


  void alarmLow(){
      printHeader(F("   LOW TEMP ALARM   "));
      confirmVariable(greenhouse.lowTempAlarm.value(), enableList, sizeOfEnableList);
      if(choiceIsConfirmed()){
        switch (selectedElement) {
          case 0: greenhouse.lowTempAlarm.setValue(false);display(genericId(HOME));break;
          case 1: display(genericId(MINTEMP));break;
        }
      }
  }
  void alarmHigh(){
        printHeader(F("  HIGH TEMP ALARM   "));
        confirmVariable(greenhouse.highTempAlarm.value(), enableList, sizeOfEnableList);
        if(choiceIsConfirmed()){
          switch (selectedElement) {
            case 0: greenhouse.highTempAlarm.setValue(false);display(genericId(HOME));break;
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
      display(genericId(HOME));
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
      display(genericId(HOME));
    }

  }

void windRollup(int nb){
  printHeader(header(("R"),nb),F(" - WIND OVERRIDE"));
  confirmVariable(greenhouse.rollup[nb].isEnabled(WINDOV), enableList, sizeOfEnableList);
  if(choiceIsConfirmed()){
    switch (selectedElement) {
      case 0: greenhouse.device[nb].disable(WINDOV);display(genericId(HOME));break;
      case 1: display(rollupId(nb, WIND2));break;
    }
  }
}
void windTargetRollup(int nb){
  printHeader(header(("R"),nb),F(" - MAX SPEED(km/h)"));
  confirmVariable((float)0,greenhouse.rollup[nb].floatVar(WINDOV),(float)100);
  if(choiceIsConfirmed()){
    greenhouse.rollup[nb].initOverride(WINDOV, 1, ABOVE, fvariable, 5, 0);
    display(genericId(HOME));
  }
}
void coldLockRollup(int nb){
  printHeader(header(("R"),nb),F(" - OUTSIDE TEMP OV"));
  confirmVariable(greenhouse.rollup[nb].isEnabled(OUTTEMP), enableList, sizeOfEnableList);
  if(choiceIsConfirmed()){
    switch (selectedElement) {
      case 0: greenhouse.device[nb].disable(OUTTEMP);display(genericId(HOME));break;
      case 1: display(rollupId(nb, OUTSIDE2));break;
    }
  }
}
void minTempRollup(int nb){
  printHeader(header(("R"),nb),F(" - CLOSE AT(C)"));
  confirmVariable((float)-50,greenhouse.rollup[nb].floatVar(OUTTEMP),(float)50);
  if(choiceIsConfirmed()){
    greenhouse.rollup[nb].initOverride(OUTTEMP, 1, BELOW, fvariable, 1, 0);
    display(genericId(HOME));
  }
}

void motorBreaker(int nb){
  printHeader(header(("R"),nb),F(" - CLOSE AT(C)"));
  confirmVariable((float)0,greenhouse.rollup[nb].currentLimit.value(),(float)10);
  printDisabledIfZero();
  if(choiceIsConfirmed()){
    greenhouse.rollup[nb].currentLimit.setValue(fvariable);
    display(genericId(HOME));
  }
}


void autoModeRollup(int nb){
  greenhouse.rollup[nb].unlock();
  display(genericId(HOME));

}
void forceOpenRollup(int nb){
  printHeader(header(("R"),nb),F(" - LOCK OPEN(min)"));
  printAdjustInstructions();
  confirmVariable((unsigned short)0, (unsigned short)0, (unsigned short)1440);
  zeroEqualInfinity();
  if(nextIsConfirmed()){
    greenhouse.rollup[nb].lockOpenAndWait((unsigned long)usvariable*60);
    display(genericId(HOME));
  }
  if(backIsConfirmed()){
    display(genericId(HOME));
  }
}
void forceCloseRollup(int nb){
  printHeader(header(("R"),nb),F(" - LOCK CLOSE(min)"));
  printAdjustInstructions();
  confirmVariable((unsigned short)0, (unsigned short)0, (unsigned short)1440);
  zeroEqualInfinity();
  if(nextIsConfirmed()){
    greenhouse.rollup[nb].lockCloseAndWait((unsigned long)usvariable*60);
    display(genericId(HOME));
  }
  if(backIsConfirmed()){
    display(genericId(HOME));
  }
}
void forceIncRollup1(int nb){
  printHeader(header(("R"),nb),F(" - LOCK TO %     "));
  printAdjustInstructions();
  confirmVariable( (unsigned short)0, greenhouse.rollup[nb].incrementCounter(), (unsigned short)100);
  if(choiceIsConfirmed()){
    usvariable1 = usvariable;
    display(rollupId(nb,FINC2));
  }
}
void forceIncRollup2(int nb){
  printHeader(header(("R"),nb),F(" - DELAY (min)    "));
  printAdjustInstructions();
  confirmVariable((unsigned short)0, (unsigned short)0, (unsigned short)1440);
  zeroEqualInfinity();
  if(nextIsConfirmed()){
    greenhouse.rollup[nb].lockAtIncrement(usvariable1, (unsigned long)usvariable*60);
    display(genericId(HOME));
  }
  if(backIsConfirmed()){
    display(genericId(HOME));
  }
}

void rotationUpRollup(int nb){
  lcd.setCursor(19,0);
  lcd.write((byte)UPS);
  setParameter(header(("R"),nb),F("-ROTATION TIME(s)"),greenhouse.rollup[nb].rotationUp,rollupId(nb,ROTDOWN));
}
void rotationDownRollup(int nb){
  lcd.setCursor(19,0);
  lcd.write((byte)DOWNS);
  if(nextIsConfirmed()){
    resetMenuCount();
  }
  setParameter(header(("R"),nb),F("-ROTATION TIME(s)"),greenhouse.rollup[nb].rotationDown,genericId(HOME));
}

void setStagesRollup(int nb){
  printHeader(header(("R"),nb),F("- OPENING STEPS   "));
  printAdjustInstructions();
  confirmVariable(greenhouse.rollup[nb].stages.minimum()+1,greenhouse.rollup[nb].stages.value(),greenhouse.rollup[nb].stages.maximum()-1);
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
  printHeader(header(("R"),nb),F("- MIN OPENING(%)  "));
  printAdjustInstructions();
  confirmVariable(greenhouse.rollup[nb].stage[1].target.minimum(),greenhouse.rollup[nb].stage[1].target.value(),greenhouse.rollup[nb].stage[1].target.maximum());
  if(choiceIsConfirmed()){
    greenhouse.rollup[nb].stage[1].target.setValue(usvariable);
    display(rollupId(nb,S2TARGET));
  }
}
void setStage2TargetRollup(int nb){
  printHeader(header(("R"),nb),F("- MAX OPENING(%)  "));
  printAdjustInstructions();
  if(greenhouse.rollup[nb].stages.value() > 1){
    int x = greenhouse.rollup[nb].stages.value();
    confirmVariable(greenhouse.rollup[nb].stage[1].target.value(),greenhouse.rollup[nb].stage[x].target.value(),greenhouse.rollup[nb].stage[x].target.maximum());
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
    confirmVariable(greenhouse.rollup[nb].stage[0].target.value(),greenhouse.rollup[nb].stage[1].target.value(),greenhouse.rollup[nb].stage[1].target.maximum());
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
    printHeader(header(("R"),nb),F("- START OPENING(C)"));
  confirmVariable(F("coolT:       "), F("start open:  "),greenhouse.rollup[nb].stage[1].mod.minimum(),greenhouse.rollup[nb].stage[1].mod.value(),greenhouse.rollup[nb].stage[1].mod.maximum(), greenhouse.coolingTemp());
  if(choiceIsConfirmed()){
    greenhouse.rollup[nb].stage[1].mod.setValue(fvariable);
    display(returnid);
  }
}
void setStage2TempRollup(int nb){
  printHeader(header(("R"),nb),F("- FULL OPENING(C) "));
  if(greenhouse.rollup[nb].stages.value() > 1){
    for(int x = 2; x <= greenhouse.rollup[nb].stages.value();x++){
      if(greenhouse.rollup[nb].stages.value() == x){
        confirmVariable(F("coolT:       "), F("full open:   "),greenhouse.rollup[nb].stage[1].mod.value(),greenhouse.rollup[nb].stage[x].mod.value(),greenhouse.rollup[nb].stage[x].mod.maximum(), greenhouse.coolingTemp());
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
    confirmVariable(F("coolT "), F("full open:   "),greenhouse.rollup[nb].stage[1].mod.minimum(),greenhouse.rollup[nb].stage[1].mod.value(),greenhouse.rollup[nb].stage[1].mod.maximum(), greenhouse.coolingTemp());
    if(choiceIsConfirmed()){
      greenhouse.rollup[nb].stage[1].mod.setValue(fvariable);
      display(rollupId(nb,PAUSE));
    }
  }
}
void pauseTimeRollup(int nb){
  setParameter(header(("R"),nb),F("- PAUSE TIME(s)   "),greenhouse.rollup[nb].pause,rollupId(nb,HYST));
}
void hystRollup(int nb){
  printHeader(header(("R"),nb),F("- HYSTERESIS(C)   "));
  printAdjustInstructions();
  confirmVariable(greenhouse.rollup[nb].hyst.minimum(),greenhouse.rollup[nb].hyst.value(),greenhouse.rollup[nb].hyst.maximum());
    if(choiceIsConfirmed()){
      greenhouse.rollup[nb].hyst.setValue(fvariable);
      if(nextIsConfirmed()){
        resetMenuCount();
      }
      display(rollupId(nb,MAIN));
    }

}
//********************DEVICE SPECIFIC MENUS********************
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

const char humidity_s[] PROGMEM = "humidity";
const char lockon_s[] PROGMEM = "lock ON";
const char lockoff_s[] PROGMEM = "lock OFF";

const char* const		mainDeviceList[] PROGMEM = {disable_s,parameters_s,overrides_s};
const int sizeOfMainDeviceList = sizeof(mainDeviceList)/sizeof(mainDeviceList[0]);
byte lastSelectedElementInDeviceList = 3;

const char* const		overrideDeviceList[] PROGMEM = {humidity_s,timer3_s,timer2_s,timer1_s,lockoff_s,lockon_s};
//const int sizeOfMainRollupList = sizeof(mainRollupList)/sizeof(mainRollupList[0]);
const int sizeOfOverrideDeviceList = sizeof(overrideDeviceList)/sizeof(overrideDeviceList[0]);
byte lastSelectedElementInOverrideDeviceList = 6;

const char* const		overrideDeviceLockList[] PROGMEM = {humidity_s,timer3_s,timer2_s,timer1_s,unlock_s};
//const int sizeOfMainRollupList = sizeof(mainRollupList)/sizeof(mainRollupList[0]);
const int sizeOfOverrideDeviceLockList = sizeof(overrideDeviceLockList)/sizeof(overrideDeviceLockList[0]);
byte lastSelectedElementInOverrideDeviceLockList = 5;

void mainDevice(int nb){
  if(greenhouse.device[nb].isActivated()){
    printHeader(deviceHeader(nb),F(" - MAIN MENU"));

    confirmVariable(lastSelectedElementInDeviceList, mainDeviceList, sizeOfMainDeviceList);
    if(choiceIsConfirmed()){
      switch (selectedElement) {
        case 2: display(deviceId(nb,OVERRIDE));lastSelectedElementInDeviceList = 2; break;
        case 1:
          if(greenhouse.device[nb].type.value() != VALVTYPE){
            if(greenhouse.device[nb].type.value() == HEATERTYPE && heatingDevices() < 2){
              display(deviceId(nb,STOPTEMP));
            }
            else if(greenhouse.device[nb].type.value() == FANTYPE && coolingDevices() < 2){
              display(deviceId(nb,STOPTEMP));
            }
            else{
              display(deviceId(nb,STARTTEMP));
            }
            lastSelectedElementInDeviceList = 1;
          }
        break;
        case 0: display(deviceId(nb,ENABLE));lastSelectedElementInDeviceList = 2; break;
      }

    }
  }
  else{
    resetMenuCount();
    display(deviceId(nb,ENABLE));
  }
}

void disableDevice(int nb){
  if(greenhouse.device[nb].isActivated()){
    printHeader(deviceHeader(nb),F("- DISABLE(CONFIRM)"));
    printConfirmInstructions();

    if(nextIsConfirmed()){
      greenhouse.device[nb].desactivateDevice();greenhouse.device[nb].disableOverrides();display(genericId(HOME));
      display(genericId(HOME));
    }
    if(backIsConfirmed()){
      display(genericId(HOME));
    }
  }
  else{
    printHeader(deviceHeader(nb),F("- ENABLE(CONFIRM)"));
    printConfirmInstructions();
    if(nextIsConfirmed()){
      resetMenuCount();
      display(deviceId(nb,TYPE));
    }
    if(backIsConfirmed()){
      display(genericId(HOME));
    }
  }
}

void overridesDevice(int nb){
  printHeader(deviceHeader(nb),F(" - OVERRIDES      "));
  if(greenhouse.device[nb].isLock()){
    confirmVariable(lastSelectedElementInOverrideDeviceLockList, overrideDeviceLockList, sizeOfOverrideDeviceLockList);
    if(choiceIsConfirmed()){
      switch (selectedElement) {
        case 4: display(deviceId(nb,AUTO));break;
        case 3: clockOvIndex = 0; display(deviceId(nb,FIXOV1));break;
        case 2: clockOvIndex = 1; display(deviceId(nb,FIXOV1));break;
        case 1: clockOvIndex = 2; display(deviceId(nb,FIXOV1));break;
        case 0: display(deviceId(nb,HUMIDITY1));break;
      }
    }
  }
  else{
    confirmVariable(lastSelectedElementInOverrideDeviceList, overrideDeviceList, sizeOfOverrideDeviceList);
    if(choiceIsConfirmed()){
      switch (selectedElement) {
        case 5: display(deviceId(nb,FOPEN));break;
        case 4: display(deviceId(nb,FCLOSE));break;
        case 3: clockOvIndex = 0; display(deviceId(nb,FIXOV1));break;
        case 2: clockOvIndex = 1; display(deviceId(nb,FIXOV1));break;
        case 1: clockOvIndex = 2; display(deviceId(nb,FIXOV1));break;
        case 0: display(deviceId(nb,HUMIDITY1));break;
      }
    }
  }
}
void clockOverrideDevice(int nb){
  printHeader(deviceHeader(nb),F(" - CLOCK OVERRIDE"));
  confirmVariable(greenhouse.device[nb].isEnabled(clockOv(clockOvIndex)), enableList, sizeOfEnableList);
  if(choiceIsConfirmed()){
    switch (selectedElement) {
      case 0: greenhouse.device[nb].disable(clockOv(clockOvIndex));display(genericId(HOME));break;
      case 1: display(deviceId(nb, FIXOV2));break;
    }
  }
}
void clockOverrideStartDevice(int nb){
  printHeader(deviceHeader(nb),F(" - START TIME"));
  printAdjustInstructions();
  confirmVariable((short)0,greenhouse.device[nb].hourStart(clockOv(clockOvIndex)),(short)23, (short)0,greenhouse.device[nb].minStart(clockOv(clockOvIndex)),(short)59);
  if(choiceIsConfirmed()){
    svariable3 = svariable1;
    svariable4 = svariable2;
    display(deviceId(nb,FIXOV3));
  }
}
void clockOverrideStopDevice(int nb){
  printHeader(deviceHeader(nb),F(" - STOP TIME"));
  printAdjustInstructions();
  confirmVariable((short)0, greenhouse.device[nb].hourStop(clockOv(clockOvIndex)),(short)23, (short)0, greenhouse.device[nb].minStop(clockOv(clockOvIndex)), (short)59);
  if(choiceIsConfirmed()){
    if(greenhouse.device[nb].type.value() == VALVTYPE && greenhouse.weatheradjust.value() == true){
      display(deviceId(nb, FIXOV4));
    }
    else{
      greenhouse.device[nb].initOverride(clockOv(clockOvIndex), FIX_D1_PRIORITY+clockOvIndex, svariable3, svariable4, svariable1, svariable2, true);
      display(genericId(HOME));
    }
  }
}

void weatherConditional(int nb){
  printHeader(deviceHeader(nb),F(" - WEATHER CONDITION"));
  printAdjustInstructions();
  confirmVariable((unsigned short)1,(unsigned short) greenhouse.device[nb].overrideTarget(CLOCKOV1+clockOvIndex),(unsigned short)100);
  printWeatherAdjustExtremes();

  if(choiceIsConfirmed()){
    greenhouse.device[nb].initOverride(clockOv(clockOvIndex), FIX_D1_PRIORITY+clockOvIndex, svariable3, svariable4, svariable1, svariable2, usvariable);
    display(genericId(HOME));
  }
}

void humidityDevice(int nb){
  printHeader(deviceHeader(nb),F(" - HR(%) OVERRIDE"));
  confirmVariable(greenhouse.device[nb].isEnabled(DESHUM), enableList, sizeOfEnableList);
  if(choiceIsConfirmed()){
    switch (selectedElement) {
      case 0: greenhouse.device[nb].disable(DESHUM);display(genericId(HOME));break;
      case 1: display(deviceId(nb, HUMIDITY2));break;
    }
  }
}


void humidityTargetDevice(int nb){
  printHeader(deviceHeader(nb),F(" - MAX HUMIDITY(%) "));
  printAdjustInstructions();
  confirmVariable((float)0,greenhouse.device[nb].floatVar(DESHUM),(float)100);
  if(choiceIsConfirmed()){
    greenhouse.device[nb].initOverride(DESHUM, 2, ABOVE, fvariable, 3, true);
    display(genericId(HOME));

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
    display(genericId(HOME));
  }
  if(backIsConfirmed()){
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
    display(genericId(HOME));
  }
  if(backIsConfirmed()){
    display(genericId(HOME));
  }
}

void typeDevice(int nb){
  printHeader(deviceHeader(nb),F("- TYPE"));

  confirmVariable(greenhouse.device[nb].type.value(), deviceList, sizeOfDeviceList);
  if(choiceIsConfirmed()){
    if(nextIsConfirmed()){
      greenhouse.device[nb].type.setValue(selectedElement);
      greenhouse.device[nb].activateDevice();
    }
    display(genericId(HOME));
  }
}

void startTempDevice(int nb){
  printHeader(deviceHeader(nb),F("- START TEMP(C)"));
  if(greenhouse.device[nb].type.value() == FANTYPE){
    confirmVariable(F("coolT:       "), F("start above: "),greenhouse.device[nb].mod.minimum(),greenhouse.device[nb].mod.value(),greenhouse.device[nb].mod.maximum(), greenhouse.coolingTemp());
    if(choiceIsConfirmed()){
      greenhouse.device[nb].mod.setValue(fvariable);
      display(deviceId(nb,STOPTEMP));
    }
  }
  else if(greenhouse.device[nb].type.value() == HEATERTYPE){
    if((greenhouse.daynight.value() == false)&&(otherHeatingDevices(nb) == 0)){
      printAdjustInstructions();
      confirmVariable((float)0, greenhouse.heatingTemp()+greenhouse.device[nb].mod.value(), (float)50);
      if(choiceIsConfirmed()){
        greenhouse.timepoint[2].heatingTemp.setValue(fvariable);
        greenhouse.device[nb].mod.setValue(0);
        display(deviceId(nb,STOPTEMP));
      }
    }
    else{
      confirmVariable(F("heatT:       "), F("start under: "),greenhouse.device[nb].mod.minimum(),greenhouse.device[nb].mod.value(),greenhouse.device[nb].mod.maximum(), greenhouse.heatingTemp());
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
  confirmVariable(greenhouse.device[nb].hyst.minimum(),greenhouse.device[nb].hyst.value(),greenhouse.device[nb].hyst.maximum());
  if(choiceIsConfirmed()){
    greenhouse.device[nb].hyst.setValue(fvariable);
    if(nextIsConfirmed()){
      resetMenuCount();
    }
    display(deviceId(nb,MAIN));
  }
}
//********************TIMEPOINTS SPECIFIC MENUS********************

const char adjustments_s[] PROGMEM = "adjustments";
const char periods_s[] PROGMEM = "periods";
const char temperatures_s[] PROGMEM = "temperatures";
const char weatheradjust_s[] PROGMEM = "weather adjust";
const char daynightsettings_s[] PROGMEM = "day/night settings";
const char adjustt24h_s[] PROGMEM = "adjust T24h";


const char* const		mainTimepointList[] PROGMEM= {adjustments_s,periods_s,temperatures_s};
const int sizeOfMainTimepointList = sizeof(mainTimepointList)/sizeof(mainTimepointList[0]);
byte lastSelectedElementInTimepointList = 3;

const char* const		adjustTemperaturesList[] PROGMEM= {weatheradjust_s,daynightsettings_s,adjustt24h_s};
const int sizeOfAdjustTemperaturesList = sizeof(adjustTemperaturesList)/sizeof(adjustTemperaturesList[0]);

const char* const		adjustTemperaturesBasicList[] PROGMEM= {adjustt24h_s};
const int sizeOfAdjustTemperaturesBasicList = sizeof(adjustTemperaturesBasicList)/sizeof(adjustTemperaturesBasicList[0]);

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
      if(nextIsConfirmed()){
        resetMenuCount();
      }
      display(genericId(TEMPERATURES));
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
      if(nextIsConfirmed()){
        resetMenuCount();
      }
      display(genericId(TEMPERATURES));
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
      if(nextIsConfirmed()){
        resetMenuCount();
      }
        display(genericId(TEMPERATURES));
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
        if(nextIsConfirmed()){
          resetMenuCount();
        }
        display(genericId(TEMPERATURES));
    }
    else{
      if(nextIsConfirmed()){
        resetMenuCount();
      }
      display(genericId(TEMPERATURES));
    }
  }
}

void mainTimepoint(){
  printHeader(F(" TEMPERATURES MENU  "));
  confirmVariable(lastSelectedElementInTimepointList, mainTimepointList, sizeOfMainTimepointList);
  if(choiceIsConfirmed()){
    switch(selectedElement){
      case 2: lastSelectedElementInTimepointList = 2; nextTemperatureMenu(firstTimepoint(), MAIN);break;

      case 1:
        if(greenhouse.daynight.value() == false){
          display(genericId(ENABLEDAYNIGHT));
        }
        else{
          display(genericId(PERIODS));}
        lastSelectedElementInTimepointList = 1;break;
      case 0: lastSelectedElementInTimepointList = 0; display(genericId(ADJUST));break;
    }
  }
}

const char bprenight_s[] PROGMEM = "(prenight)";
const char bmorning_s[] PROGMEM = "(morning)";
const char prenight_s[] PROGMEM = "prenight";
const char morning_s[] PROGMEM = "morning";
const char day_s[] PROGMEM = "DAY";
const char night_s[] PROGMEM = "NIGHT";

const char* const	periodsListNMP[] PROGMEM= {bprenight_s, bmorning_s,night_s,day_s};
const char* const	periodsListNM[] PROGMEM= {prenight_s, bmorning_s,night_s,day_s};
const char* const	periodsListNP[] PROGMEM= {bprenight_s, morning_s,night_s,day_s};
const char* const	periodsList[] PROGMEM= {prenight_s, morning_s,night_s,day_s};
const int sizeOfPeriodsList = sizeof(periodsList)/sizeof(periodsList[0]);
byte lastSelectedElementInPeriodsList = 3;

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
      case 0: lastSelectedElementInPeriodsList = 0; display(genericId(SETPRENIGHT)); break;
      case 1: lastSelectedElementInPeriodsList = 1; display(genericId(SETDIF)); break;
      case 2: lastSelectedElementInPeriodsList = 2; display(timepointId(3, STARTTIME));break;
      case 3: lastSelectedElementInPeriodsList = 3; display(timepointId(1, STARTTIME)); break;
    }
  }
}

void startTimepoint(int nb){
  printHeader(printTimepoint(nb),F(" - START TIME"));
  printAdjustInstructions();
  confirmVariable(0, greenhouse.timepoint[nb].hr(),23, 0,greenhouse.timepoint[nb].mn(),59);
  if(choiceIsConfirmed()){
    if((nb == 1 && greenhouse.dif.value() == false)||(nb == 3 && greenhouse.prenight.value() == false)){
      adjustAndSetTimepoint(greenhouse.timepoint[nb-1]);
    }
    adjustAndSetTimepoint(greenhouse.timepoint[nb]);
    display(timepointId(nb,RAMPING));
  }
}
void referenceTimepoint(int nb){
  printHeader(printTimepoint(nb),F(" - SUN TRACKING"));
  confirmVariable(reference(greenhouse.timepoint[nb].type.value()), enableList, sizeOfEnableList);
  if(choiceIsConfirmed()){
    switch (selectedElement) {
      case 0:
        greenhouse.timepoint[nb].type.setValue(CLOCK);
        break;
      case 1:
        if(nb < 2){
          greenhouse.timepoint[nb].type.setValue(SR);
        }
        else{
          greenhouse.timepoint[nb].type.setValue(SS);
        }
        break;
    }
    adjustTimepointAfterTypeChange(greenhouse.timepoint[nb]);
    if(nextIsConfirmed()){
      resetMenuCount();
    }
    display(genericId(TEMPERATURES));
  }
}

void ramping(int nb){
  printHeader(printTimepoint(nb),F(" - RAMPING"));
  printAdjustInstructions();
  confirmVariable(greenhouse.timepoint[nb].ramping.minimum(),greenhouse.timepoint[nb].ramping.value(),greenhouse.timepoint[nb].ramping.maximum());

  if(choiceIsConfirmed()){
    greenhouse.timepoint[nb].ramping.setValue(usvariable);
    display(timepointId(nb,REF));
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
        if(nextIsConfirmed()){
          resetMenuCount();
        }
        display(genericId(TEMPERATURES));break;
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
        if(nextIsConfirmed()){
          resetMenuCount();
        }
        display(genericId(TEMPERATURES));break;
      case 1: greenhouse.prenight.setValue(true); display(timepointId(2,STARTTIME));break;
    }
  }
}

void sunHeatTemperature(int nb){
  printHeader(F("  TEMPERATURES(*C)  "),F("TEMPERATURES"), SUN, nb);
  //printAdjustInstructionsShort();
  if((heatingDeviceIsActive()&&coolingDeviceIsActive())||!someDeviceIsActive()){
    confirmVariable(greenhouse.timepoint[nb].heatingTemp.minimum(),greenhouse.timepoint[nb].heatingTemp.value(),greenhouse.timepoint[nb].heatingTemp.maximum(), greenhouse.timepoint[nb].coolingTemp.value(), greenhouse.timepoint[nb].heatingTempCloud.value(),greenhouse.timepoint[nb].coolingTempCloud.value(), 1);
  }
  else{
    confirmVariable(F("Heating"),greenhouse.timepoint[nb].heatingTemp.minimum(),greenhouse.timepoint[nb].heatingTemp.value(),greenhouse.timepoint[nb].heatingTemp.maximum(), greenhouse.timepoint[nb].heatingTempCloud.value(), 1);
  }
  if(choiceIsConfirmed()){
    nextTemperatureMenu(nb, HEATINGSUN);
  }
}

void sunCoolTemperature(int nb ){
  printHeader(F("  TEMPERATURES(*C)  "),F("TEMPERATURES"), SUN, nb);
  //printAdjustInstructionsShort();
  if((heatingDeviceIsActive()&&coolingDeviceIsActive())||!someDeviceIsActive()){
    confirmVariable(greenhouse.timepoint[nb].coolingTemp.minimum(),greenhouse.timepoint[nb].coolingTemp.value(),greenhouse.timepoint[nb].coolingTemp.maximum(), greenhouse.timepoint[nb].heatingTemp.value(), greenhouse.timepoint[nb].heatingTempCloud.value(),greenhouse.timepoint[nb].coolingTempCloud.value(), 2);
  }
  else{
    confirmVariable(F("Cooling"),greenhouse.timepoint[nb].coolingTemp.minimum(),greenhouse.timepoint[nb].coolingTemp.value(),greenhouse.timepoint[nb].coolingTemp.maximum(), greenhouse.timepoint[nb].coolingTempCloud.value(), 1);
  }

  if(choiceIsConfirmed()){
    nextTemperatureMenu(nb, COOLINGSUN);
  }
}



void cloudHeatTemperature(int nb){
  printHeader(F("  TEMPERATURES(*C)  "),F("TEMPERATURES"), CLOUD, nb);
  //printAdjustInstructionsShort();

  if((heatingDeviceIsActive()&&coolingDeviceIsActive())||!someDeviceIsActive()){
  confirmVariable(greenhouse.timepoint[nb].heatingTempCloud.minimum(),greenhouse.timepoint[nb].heatingTempCloud.value(),greenhouse.timepoint[nb].heatingTempCloud.maximum(), greenhouse.timepoint[nb].heatingTemp.value(), greenhouse.timepoint[nb].coolingTemp.value(),greenhouse.timepoint[nb].coolingTempCloud.value(), 3);
  }
  else{
    confirmVariable(F("Heating"),greenhouse.timepoint[nb].heatingTempCloud.minimum(),greenhouse.timepoint[nb].heatingTempCloud.value(),greenhouse.timepoint[nb].heatingTempCloud.maximum(), greenhouse.timepoint[nb].heatingTemp.value(), 2);
  }
  if(choiceIsConfirmed()){
    nextTemperatureMenu(nb, HEATINGCLOUD);
  }
}

void cloudCoolTemperature(int nb){
  printHeader(F("  TEMPERATURES(*C)  "),F("TEMPERATURES"), CLOUD, nb);
  //printAdjustInstructionsShort();

  if((heatingDeviceIsActive()&&coolingDeviceIsActive())||!someDeviceIsActive()){
  confirmVariable(greenhouse.timepoint[nb].coolingTempCloud.minimum(),greenhouse.timepoint[nb].coolingTempCloud.value(),greenhouse.timepoint[nb].coolingTempCloud.maximum(), greenhouse.timepoint[nb].heatingTemp.value(), greenhouse.timepoint[nb].coolingTemp.value(),greenhouse.timepoint[nb].heatingTempCloud.value(), 4);
  }
  else{
    confirmVariable(F("Cooling"),greenhouse.timepoint[nb].coolingTempCloud.minimum(),greenhouse.timepoint[nb].coolingTempCloud.value(),greenhouse.timepoint[nb].coolingTempCloud.maximum(), greenhouse.timepoint[nb].coolingTemp.value(), 2);
  }
  if(choiceIsConfirmed()){
    nextTemperatureMenu(nb, COOLINGCLOUD);
  }
}

void adjustTimepoint(){
  printHeader(F("  TEMP ADJUSTMENTS  "));
  confirmVariable(2, adjustTemperaturesList, sizeOfAdjustTemperaturesList);
  if(choiceIsConfirmed()){
    switch (selectedElement) {
      case 2: display(genericId(ADJUST24H));break;
      case 1: display(genericId(ENABLEDAYNIGHT));break;
      case 0: display(genericId(WEATHERADJUST));break;
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
    if(nextIsConfirmed()){
      resetMenuCount();
    }
    display(genericId(TEMPERATURES));
  }
}
//********************GENERIC MENUS********************
const String	configList[] = {"factory reset","high temp alarm","low temp alarm","sensors","tests","time/date/coord"};
//const int sizeOfMainRollupList = sizeof(mainRollupList)/sizeof(mainRollupList[0]);
const int sizeOfConfigList = sizeof(configList)/sizeof(configList[0]);
byte lastSelectedElementInConfigList = 5;

void config(){
  printHeader(F("    CONFIG MENU     "));
  confirmVariable(lastSelectedElementInConfigList, configList, sizeOfConfigList);

  if(choiceIsConfirmed()){
    switch (selectedElement) {
      case 5  : display(genericId(HOURSET));lastSelectedElementInConfigList = 5;break;
      case 4: display(genericId(TESTS));lastSelectedElementInConfigList = 4;break;
      case 3: display(genericId(SELECTSENSORS));lastSelectedElementInConfigList = 3;break;
      case 2: display(genericId(LOWTEMPALARM));lastSelectedElementInConfigList = 2;break;
      case 1: display(genericId(HIGHTEMPALARM));lastSelectedElementInConfigList = 1;break;
      case 0: display(genericId(FACTORYSETTINGS));lastSelectedElementInConfigList = 0;break;
    }
  }
}

const char string_0[] PROGMEM = "motors";
const char string_1[] PROGMEM = "radiation";
const char string_2[] PROGMEM = "weather";
const char string_3[] PROGMEM = "outside";
const char string_4[] PROGMEM = "inside";
const char* const sensorList[] PROGMEM = {string_0, string_1, string_2, string_3, string_4};
const int sizeOfSensorList = sizeof(sensorList)/sizeof(sensorList[0]);

void selectSensor(){
  printHeader(F(" SELECT SENSOR "));
  confirmVariable(4, sensorList, sizeOfSensorList);

  if(choiceIsConfirmed()){
    switch (selectedElement) {
      case 0: display(genericId(CURRENTSENSOR));break;
      case 1: display(genericId(LUXMETER));break;
      case 2: display(genericId(RAINSENSOR));break;
      case 3: display(genericId(OUTSIDETEMP));break;
      case 4: display(genericId(INSIDETEMP));break;
    }
  }
}

const char string_5[] PROGMEM = "DS18B20";
const char string_6[] PROGMEM = "SHT1X";
const char string_7[] PROGMEM = "disabled";
const char* const tempSensorList[] PROGMEM = {string_5, string_6, string_7};
const int sizeOfTempSensorList = sizeof(tempSensorList)/sizeof(tempSensorList[0]);

void insideTemp(){
  printHeader(F(" INSIDE TEMP SENSOR "));
    confirmVariable(greenhouse.insideTemp.value(), tempSensorList, sizeOfTempSensorList-1);

  if(choiceIsConfirmed()){
    switch (selectedElement) {
      //0 = DS18B20
      //1 = SHT1X
      case 0: greenhouse.insideTemp.setValue(0);break;
      case 1: greenhouse.insideTemp.setValue(1);break;
    }
    display(genericId(INSIDETEMPDATA));
  }

}

void outsideTemp(){
  printHeader(F("OUTSIDE TEMP SENSOR"));
  confirmVariable(greenhouse.outsideTemp.value(), tempSensorList, sizeOfTempSensorList);

  if(choiceIsConfirmed()){
    switch (selectedElement) {
      //0 = DS18B20
      //1 = SHT1X
      //2 = off
      case 0: greenhouse.outsideTemp.setValue(0); display(genericId(OUTSIDETEMPDATA));break;
      case 1: greenhouse.outsideTemp.setValue(1); display(genericId(OUTSIDETEMPDATA));break;
      case 2: greenhouse.outsideTemp.setValue(2); display(genericId(HOME));break;
    }
  }
}
const char string_8[] PROGMEM = "NC contact";
const char string_9[] PROGMEM = "NO contact";
const char string_10[] PROGMEM = "bucket mode";
const char string_11[] PROGMEM = "disabled";
const char* const rainSensorList[] PROGMEM = {string_8, string_9, string_10, string_11};
const int sizeOfRainSensorList = sizeof(rainSensorList)/sizeof(rainSensorList[0]);

void rainsensor(){
  printHeader(F("   RAIN SENSOR   "));
  confirmVariable(greenhouse.rainSensor.value(), rainSensorList, sizeOfRainSensorList);

  if(choiceIsConfirmed()){
    switch (selectedElement) {
      //0 = bucket mode
      //1 = no contact
      //2 = nc contact
      //3 = off
      case 0: greenhouse.rainSensor.setValue(0);break;
      case 1: greenhouse.rainSensor.setValue(1); break;
      case 2: greenhouse.rainSensor.setValue(2); break;
      case 3: greenhouse.rainSensor.setValue(3);break;
    }
    display(genericId(WINDSENSOR));
  }
}

void windsensor(){
  printHeader(F("   ANEMOMETER   "));
  confirmVariable(greenhouse.anemometer.value(), enableList, sizeOfEnableList);

  if(choiceIsConfirmed()){
    switch (selectedElement) {
      case 0: greenhouse.anemometer.setValue(false); display(genericId(WEATHERDATA));break;
      case 1: greenhouse.anemometer.setValue(true); display(genericId(WEATHERDATA));break;
    }
  }
}
void luxmeter(){
  printHeader(F("     LUX METER    "));
  confirmVariable(greenhouse.luxMeter.value(), enableList, sizeOfEnableList);

  if(choiceIsConfirmed()){
    switch (selectedElement) {
      case 0: greenhouse.luxMeter.setValue(false); display(genericId(RADIATIONDATA));break;
      case 1: greenhouse.luxMeter.setValue(true); display(genericId(RADIATIONDATA));break;
    }
  }
}

void insidetempdata(){
  printHeader(F("   INSIDE TEMP/HUM  "));
  lcd.setCursor(0,1);
  lcd.print(F("T:"));
  lcd.print(greenhouseTemperature.value());
  lcd.print(F("C"));
  lcd.setCursor(0,2);
  lcd.print(F("T24h:"));
  lcd.print((int)greenhouseTemperature.average24h());
  lcd.print(F("C"));
  if(greenhouse.insideTemp.value() == STH1X_TEMP){
    lcd.setCursor(8,1);
    lcd.print(F("|HR:"));
    lcd.print(greenhouseHumidity.value());
    lcd.print(F("%"));
    lcd.setCursor(8,2);
    lcd.print(F("|HA:"));
    lcd.print((int)greenhouseHumidity.absolute(greenhouseTemperature.value()));
    lcd.print(F("g/m3"));
    lcd.setCursor(8,3);
    lcd.print(F("|HD:"));
    lcd.print((int)greenhouseHumidity.deficit(greenhouseTemperature.value()));
    lcd.print(F("g/m3"));
  }
  if(choiceIsConfirmed()){
    display(genericId(HOME));
  }

}
void outsidetempdata(){
  if(greenhouse.outsideTemp.value() != OFF_TEMP){
    printHeader(F("  OUTSIDE TEMP/HUM  "));
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
  if(greenhouse.outsideTemp.value() == STH1X_TEMP){
    lcd.setCursor(8,1);
    lcd.print(F("|HR:"));
    lcd.print(outsideHumidity.value());
    lcd.print(F("%"));
    lcd.setCursor(8,2);
    lcd.print(F("|HA:"));
    lcd.print((int)outsideHumidity.absolute(outsideTemperature.value()));
    lcd.print(F("g/m3"));
    lcd.setCursor(8,3);
    lcd.print(F("|HD:"));
    lcd.print((int)outsideHumidity.deficit(outsideTemperature.value()));
    lcd.print(F("g/m3"));
  }
  if(choiceIsConfirmed()){
    display(genericId(HOME));
  }
}
void weatherdata(){
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
    lcd.setCursor(0,2);
    lcd.print(F("Rain 24h: "));
    lcd.print(totalRainfall);
    lcd.print(F("mm"));
  }
  if(greenhouse.anemometer.value() != false){
    lcd.setCursor(0,3);
    lcd.print(F("Wind spd: "));
    lcd.print(windSpeed);
    lcd.print(F("km/h"));
  }
  if(choiceIsConfirmed()){
    display(genericId(HOME));
  }
}
void radiationdata(){
  printHeader(F("   RADIATION DATA   "));

  if(greenhouse.luxMeter.value() != false){
    lcd.setCursor(0,1);
    lcd.print(luxReading[0]);
    lcd.print(F(" lux"));
    lcd.setCursor(0,2);
    lcd.print((float)luxReading[0]*0.0079);
    lcd.print(F(" W/m2"));
  }
  if(choiceIsConfirmed()){
    display(genericId(HOME));
  }
}

void currentsensor(){
    printHeader(F("   MOTORS CURRENT   "));

    if(R1.isActivated()){
      lcd.setCursor(0,1);
      lcd.print(F("R1 : "));
      lcd.print(r1current);
      lcd.print(F(" A  "));
    }
    if(R2.isActivated()){
      lcd.setCursor(0,2);
      lcd.print(F("R2 : "));
      lcd.print(r2current);
      lcd.print(F(" A  "));
    }
    if(choiceIsConfirmed()){
      display(genericId(HOME));
    }
}

const String	testsList[] = { "test temp" ,"test relays",  "FFW","FFFW"};
//const int sizeOfMainRollupList = sizeof(mainRollupList)/sizeof(mainRollupList[0]);
const int sizeOfTestsList = sizeof(testsList)/sizeof(testsList[0]);

void tests(){
  printHeader(F(" TEST PROGRAMS "));
    confirmVariable(0, testsList, sizeOfTestsList);

  if(nextIsConfirmed()){
    switch (selectedElement) {
      case 0: display(genericId(TESTTEMP));break;
      case 1: checkRelays(); display(genericId(HOME));break;
      case 2: clockTest = TEST_CLOCKF; display(genericId(HOME));break;
      case 3: clockTest = TEST_CLOCKFF; display(genericId(HOME));break;
    }
  }
  if(backIsConfirmed()){
    display(genericId(CONFIG));
  }
}

void mockTemp(){;
  adjustfvariable(-10,20,50);
  greenhouseTemperature.registerValue(fvariable);
  homeDisplay();
  menu = MODE_PROGRAM;
  greenhouse.testRollups(true);
  if(backIsConfirmed()){
    greenhouse.testRollups(false);
    display(genericId(HOME));
  }
}

void factorySettings(){
  printHeader(F("  FACTORY SETTINGS  "));
  confirmVariable(0, confirmList, sizeOfConfirmList);

  if(choiceIsConfirmed()){
    switch (selectedElement) {
      case 0: display(genericId(HOME));break;
      case 1: EEPROMReset = DEFAULT_SETTINGS; loadParameters();display(genericId(HOME));break;
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
        case 0: greenhouse.daynight.setValue(false);greenhouse.dif.setValue(false);greenhouse.prenight.setValue(false); display(genericId(TEMPERATURES));break;
        case 1: greenhouse.daynight.setValue(true); display(genericId(PERIODS));break;
      }
    }
  }
  else{
    if(choiceIsConfirmed()){
      switch (selectedElement) {
        case 0: greenhouse.daynight.setValue(false);greenhouse.dif.setValue(false);greenhouse.prenight.setValue(false);break;
        case 1: greenhouse.daynight.setValue(true); break;
      }
      display(genericId(TEMPERATURES));
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
        if(nextIsConfirmed()){
          resetMenuCount();
        }
        display(genericId(TEMPERATURES));break;
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
    if(nextIsConfirmed()){
      resetMenuCount();
    }
    if(choiceIsConfirmed()){
      switch (selectedElement) {
        case 0: greenhouse.weatheradjust.setValue(false);display(genericId(TEMPERATURES));break;
        case 1: greenhouse.weatheradjust.setValue(true);display(genericId(TEMPERATURES));break;
      }
    }
  }
}

const String weatherList[]  = { "auto", "ratio","cloud","sun"};
const int sizeOfWeatherList = sizeof(weatherList)/sizeof(weatherList[0]);

byte weatherElementInList(){
  if(greenhouse.luxMeter.value() == true){
    return 0;
  }
  else if(greenhouse.weatherP.value() == 0){
    return 3;
  }
  else if(greenhouse.weatherP.value() == 100){
    return 3;
  }
  else if(greenhouse.weatherP.value() == 1){
    return 2;
  }
  else{
    return 1;
  }
}

void setWeather(){
  if(greenhouse.weatheradjust.value() == true){
    printHeader(F("    SET WEATHER     "));

    confirmVariable(weatherElementInList(), weatherList, sizeOfWeatherList);
    if(nextIsConfirmed()){
      switch (selectedElement) {
        case 3: greenhouse.luxMeter.setValue(false); greenhouse.setWeather(SUN);display(genericId(HOME));break;
        case 2: greenhouse.luxMeter.setValue(false); greenhouse.setWeather(CLOUD);display(genericId(HOME));break;
        case 1: greenhouse.luxMeter.setValue(false); display(genericId(WEATHERRATIO));break;
        case 0: greenhouse.luxMeter.setValue(true); display(genericId(HOME));break;
      }
    }
    if(backIsConfirmed()){
      display(genericId(HOME));
    }
  }
  else{
    if(nextIsConfirmed()){
      resetMenuCount();
    }
    display(genericId(WEATHERADJUST));
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
    confirmVariable(0,yearSet,99);
    if(choiceIsConfirmed()){
        unsigned short year = 2000 + usvariable;
        rtc.setDate( (byte)greenhouse.rightNow(3), (byte)greenhouse.rightNow(4), year);
        getDateAndTime();
        greenhouse.setNow(rightNow);
        greenhouse.solarCalculations();
      display(genericId(SETLATITUDE));
    }
  }

  void latitudeset(){
    lcd.setCursor(19,0);
    setParameter("",F("   SET LATITUDE    "),greenhouse.latitude,genericId(SETLONGITUDE));
  }
  void longitudeset(){
    lcd.setCursor(19,0);
    setParameter("",F("   SET LONGITUDE   "),greenhouse.longitude,genericId(SETTIMEZONE));
  }
  void timezoneset(){
    lcd.setCursor(19,0);
    if(nextIsConfirmed()){
      resetMenuCount();
    }
    setParameter("",F("   SET TIMEZONE    "),greenhouse.timezone,genericId(CONFIG));
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
    {FIXOV2, clockOverrideStartRollup},
    {FIXOV3, clockOverrideStopRollup},
    {FIXOV4, clockOverrideTargetRollup},
    {RAIN1, rainRollup},
    {RAIN2, rainTargetRollup},
    {WIND1, windRollup},
    {WIND2, windTargetRollup},
    {BREAKER, motorBreaker},
    {OUTSIDE1, coldLockRollup},
    {OUTSIDE2, minTempRollup},
    {PROGRAM, programRollup},
    {ENABLE, enableRollup},
    {INSTRUCT, disabled},
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
    {FIXOV2, clockOverrideStartDevice},
    {FIXOV3, clockOverrideStopDevice},
    {FIXOV4, weatherConditional},
    {HUMIDITY1, humidityDevice},
    {HUMIDITY2, humidityTargetDevice},
    {ENABLE, disableDevice},
};
const indexedMenu	timepointMenus[] = {
//  - name ----    - menu ----------
    {MAIN, disabled},
		{STARTTIME, startTimepoint},
    {REF, referenceTimepoint},
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
    {WEATHERADJUST, weatheradjust},
    {WEATHER, setWeather},
    {WEATHERRATIO, weatherRatio},
    {ENABLEDAYNIGHT, daynight},
    {PERIODS, periods},
    {TESTS, tests},
    {TESTTEMP, mockTemp},

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
    {INSIDETEMPDATA,insidetempdata},
    {OUTSIDETEMPDATA,outsidetempdata},
    {WEATHERDATA,weatherdata},
    {RADIATIONDATA,radiationdata},
    {CURRENTSENSOR,currentsensor},
    {LOWTEMPALARM,alarmLow},
    {MINTEMP,minTemp},
    {HIGHTEMPALARM,alarmHigh},
    {MAXTEMP,maxTemp}

};
//EXECUTE ACTIVE MENU PROGRAM
void printRollup( int nb, byte menuName){
  menu = MODE_PROGRAM;
  int	numMenus = sizeof(rollupMenus) / sizeof(indexedMenu);

  for (int i = 0; i < numMenus; i++) {
    if ( menuName == rollupMenus[i].name) {
      rollupMenus[i].Menu(nb);
    }
  }
}
void printDevice( int nb, byte menuName){
  menu = MODE_PROGRAM;

  int	numMenus = sizeof(deviceMenus) / sizeof(indexedMenu);

  for (int i = 0; i < numMenus; i++) {
    if ( menuName == deviceMenus[i].name) {
      deviceMenus[i].Menu(nb);
    }
  }
}

void printTimepoint( int nb, byte menuName){
  menu = MODE_PROGRAM;

  int	numMenus = sizeof(timepointMenus) / sizeof(indexedMenu);

  for (int i = 0; i < numMenus; i++) {
    if ( menuName == timepointMenus[i].name) {
      timepointMenus[i].Menu(nb);
    }
  }
}

void printGeneric(byte menuName){
  menu = MODE_PROGRAM;

  int	numMenus = sizeof(genericMenus) / sizeof(Menu);

  for (int i = 0; i < numMenus; i++) {
    if (menuName == genericMenus[i].name) {
      genericMenus[i].Menu();
    }
  }
}


void printMenu(){
  for(int x = 0;x < greenhouse.rollups.value();x++){
    if(menuID == rollupId(x,MAIN)){  printRollup(x, MAIN);}
    if(menuID == rollupId(x,AUTO)){  printRollup(x, AUTO);}
    if(menuID == rollupId(x,FOPEN)){  printRollup(x, FOPEN);}
    if(menuID == rollupId(x,FCLOSE)){  printRollup(x, FCLOSE);}
    if(menuID == rollupId(x,OVERRIDE)){  printRollup(x, OVERRIDE);}
    if(menuID == rollupId(x,FINC1)){  printRollup(x, FINC1);}
    if(menuID == rollupId(x,FINC2)){  printRollup(x, FINC2);}
    if(menuID == rollupId(x,FIXOV1)){  printRollup(x, FIXOV1);}
    if(menuID == rollupId(x,FIXOV2)){  printRollup(x, FIXOV2);}
    if(menuID == rollupId(x,FIXOV3)){  printRollup(x, FIXOV3);}
    if(menuID == rollupId(x,FIXOV4)){  printRollup(x, FIXOV4);}
    if(menuID == rollupId(x,HUMIDITY1)){  printRollup(x, HUMIDITY1);}
    if(menuID == rollupId(x,HUMIDITY2)){  printRollup(x, HUMIDITY2);}
    if(menuID == rollupId(x,RAIN1)){  printRollup(x, RAIN1);}
    if(menuID == rollupId(x,RAIN2)){  printRollup(x, RAIN2);}
    if(menuID == rollupId(x,WIND1)){  printRollup(x, WIND1);}
    if(menuID == rollupId(x,WIND2)){  printRollup(x, WIND2);}
    if(menuID == rollupId(x,BREAKER)){  printRollup(x, BREAKER);}
    if(menuID == rollupId(x,OUTSIDE1)){  printRollup(x, OUTSIDE1);}
    if(menuID == rollupId(x,OUTSIDE2)){  printRollup(x, OUTSIDE2);}
    if(menuID == rollupId(x,PROGRAM)){  printRollup(x, PROGRAM);}
    if(menuID == rollupId(x,ENABLE)){  printRollup(x, ENABLE);}
    if(menuID == rollupId(x,INSTRUCT)){  printRollup(x, INSTRUCT);}
    if(menuID == rollupId(x,HYST)){  printRollup(x, HYST);}
  	if(menuID == rollupId(x,ROTUP )){  printRollup(x, ROTUP );}
  	if(menuID == rollupId(x,ROTDOWN )){  printRollup(x, ROTDOWN );}
  	if(menuID == rollupId(x,PAUSE)){  printRollup(x, PAUSE);}
    if(menuID == rollupId(x,SETSTAGES)){  printRollup(x, SETSTAGES);}
  	if(menuID == rollupId(x,S1TEMP)){  printRollup(x, S1TEMP);}
  	if(menuID == rollupId(x,S2TEMP)){  printRollup(x, S2TEMP);}
  	if(menuID == rollupId(x,S3TEMP)){  printRollup(x, S3TEMP);}
  	if(menuID == rollupId(x,S4TEMP)){  printRollup(x, S4TEMP);}
  	if(menuID == rollupId(x,S1TARGET)){  printRollup(x, S1TARGET);}
  	if(menuID == rollupId(x,S2TARGET)){  printRollup(x, S2TARGET);}
  	if(menuID == rollupId(x,S3TARGET)){  printRollup(x, S3TARGET);}
  	if(menuID == rollupId(x,S4TARGET)){  printRollup(x, S4TARGET);}
  }

  for(int x = 0;x < greenhouse.devices.value();x++){
    if(menuID == deviceId(x,MAIN)){  printDevice(x, MAIN);}
    if(menuID == deviceId(x,TYPE)){  printDevice(x, TYPE);}
    if(menuID == deviceId(x,STARTTEMP)){  printDevice(x, STARTTEMP);}
    if(menuID == deviceId(x,STOPTEMP)){  printDevice(x, STOPTEMP);}
    if(menuID == deviceId(x,OVERRIDE)){  printDevice(x, OVERRIDE);}
    if(menuID == deviceId(x,AUTO)){  printDevice(x, AUTO);}
    if(menuID == deviceId(x,FOPEN)){  printDevice(x, FOPEN);}
    if(menuID == deviceId(x,FCLOSE)){  printDevice(x, FCLOSE);}
    if(menuID == deviceId(x,FIXOV1)){  printDevice(x, FIXOV1);}
    if(menuID == deviceId(x,FIXOV2)){  printDevice(x, FIXOV2);}
    if(menuID == deviceId(x,FIXOV3)){  printDevice(x, FIXOV3);}
    if(menuID == deviceId(x,FIXOV4)){  printDevice(x, FIXOV4);}
    if(menuID == deviceId(x,HUMIDITY1)){  printDevice(x, HUMIDITY1);}
    if(menuID == deviceId(x,HUMIDITY2)){  printDevice(x, HUMIDITY2);}
    if(menuID == deviceId(x,ENABLE)){  printDevice(x, ENABLE);}
  }

  for(int x = 0;x < greenhouse.timepoints.value();x++){

    if(menuID == timepointId(x,MAIN)){  printTimepoint(x, MAIN);}
    if(menuID == timepointId(x,STARTTIME)){  printTimepoint(x, STARTTIME);}
    if(menuID == timepointId(x,HEATINGSUN)){  printTimepoint(x, HEATINGSUN);}
    if(menuID == timepointId(x,COOLINGSUN)){  printTimepoint(x, COOLINGSUN);}
    if(menuID == timepointId(x,HEATINGCLOUD)){  printTimepoint(x, HEATINGCLOUD);}
    if(menuID == timepointId(x,COOLINGCLOUD)){  printTimepoint(x, COOLINGCLOUD);}
    if(menuID == timepointId(x,RAMPING)){  printTimepoint(x, RAMPING);}
    if(menuID == timepointId(x,REF)){  printTimepoint(x, REF);}
  	if(menuID == timepointId(x,ENABLE)){  printTimepoint(x, ENABLE);}
  }
  if(menuID == genericId(HOME)){  printGeneric(HOME);}
  if(menuID == genericId(CONFIG)){  printGeneric(CONFIG);}
  if(menuID == genericId(HOURSET)){  printGeneric(HOURSET);}
  if(menuID == genericId(MINSET)){  printGeneric(MINSET);}
  if(menuID == genericId(DAY)){  printGeneric(DAY);}
  if(menuID == genericId(MONTH)){  printGeneric(MONTH);}
  if(menuID == genericId(YEAR)){  printGeneric(YEAR);}
  if(menuID == genericId(SETLATITUDE)){  printGeneric(SETLATITUDE);}
  if(menuID == genericId(SETLONGITUDE)){  printGeneric(SETLONGITUDE);}
  if(menuID == genericId(SETTIMEZONE)){  printGeneric(SETTIMEZONE);}
  if(menuID == genericId(WEATHERRATIO)){  printGeneric(WEATHERRATIO);}
  if(menuID == genericId(WEATHERADJUST)){  printGeneric(WEATHERADJUST);}
  if(menuID == genericId(WEATHER)){  printGeneric(WEATHER);}
  if(menuID == genericId(ENABLEDAYNIGHT)){  printGeneric(ENABLEDAYNIGHT);}
  if(menuID == genericId(TEMPERATURES)){  printGeneric(TEMPERATURES);}
	if(menuID == genericId(SELECTSENSORS)){  printGeneric(SELECTSENSORS);}
	if(menuID == genericId(INSIDETEMPSENSOR)){  printGeneric(INSIDETEMPSENSOR);}
	if(menuID == genericId(FACTORYSETTINGS)){  printGeneric(FACTORYSETTINGS);}
	if(menuID == genericId(ADJUST24H)){  printGeneric(ADJUST24H);}
	if(menuID == genericId(ADJUSTVR)){  printGeneric(ADJUSTVR);}
	if(menuID == genericId(ADJUST)){  printGeneric(ADJUST);}
  if(menuID == genericId(SETDIF)){  printGeneric(SETDIF);}
  if(menuID == genericId(SETPRENIGHT)){  printGeneric(SETPRENIGHT);}
	if(menuID == genericId(ENTERCODE)){  printGeneric(ENTERCODE);}
  if(menuID == genericId(PERIODS)){  printGeneric(PERIODS);}
  if(menuID == genericId(TESTS)){  printGeneric(TESTS);}
  if(menuID == genericId(TESTTEMP)){  printGeneric(TESTTEMP);}
  if(menuID == genericId(INSIDETEMP)){  printGeneric(INSIDETEMP);}
  if(menuID == genericId(OUTSIDETEMP)){  printGeneric(OUTSIDETEMP);}
  if(menuID == genericId(RAINSENSOR)){  printGeneric(RAINSENSOR);}
  if(menuID == genericId(WINDSENSOR)){  printGeneric(WINDSENSOR);}
  if(menuID == genericId(LUXMETER)){  printGeneric(LUXMETER);}
  if(menuID == genericId(LOWTEMPALARM)){  printGeneric(LOWTEMPALARM);}
  if(menuID == genericId(HIGHTEMPALARM)){  printGeneric(HIGHTEMPALARM);}
  if(menuID == genericId(MINTEMP)){  printGeneric(MINTEMP);}
  if(menuID == genericId(MAXTEMP)){  printGeneric(MAXTEMP);}
  if(menuID == genericId(INSIDETEMPDATA)){  printGeneric(INSIDETEMPDATA);}
  if(menuID == genericId(OUTSIDETEMPDATA)){  printGeneric(OUTSIDETEMPDATA);}
  if(menuID == genericId(WEATHERDATA)){  printGeneric(WEATHERDATA);}
  if(menuID == genericId(RADIATIONDATA)){  printGeneric(RADIATIONDATA);}
  if(menuID == genericId(CURRENTSENSOR)){  printGeneric(CURRENTSENSOR);}
}



boolean pressSelected(){
 char keyPressed = keypad.getKey();

 if(keyPressed != '*'){
   return true;
 }
 else{
   return false;
 }
}

boolean pressIncrease(){
 char keyPressed = keypad.getKey();

 if(keyPressed != '#'){
   return true;
 }
 else{
   return false;
 }
}
boolean pressDecrease(){
 char keyPressed = keypad.getKey();

 if(keyPressed != '*'){
   return true;
 }
 else{
   return false;
 }
}

void selectActiveKey(){
 #ifdef KEYPAD_DISPLAY
 keyPressed = keypad.getKey();

 switch(keyPressed){
   case '1' : key = keyPressed;resetMenuCount();if(!D4.isActivated()){display(rollupId(0,MAIN));};menu = MODE_PROGRAM;break;
   case '2' : key = keyPressed;resetMenuCount();if(!D5.isActivated()){display(rollupId(1,MAIN));};menu = MODE_PROGRAM;break;
   case '3' : key = keyPressed;resetMenuCount();if((!D2.isActivated())&&(!D3.isActivated())){display(rollupId(2,MAIN));};menu = MODE_PROGRAM;break;
   case '4' : key = keyPressed;resetMenuCount();display(deviceId(0,MAIN));menu = MODE_PROGRAM;break;
   case '5' : key = keyPressed;resetMenuCount();if(!R3.isActivated()){display(deviceId(1,MAIN));};menu = MODE_PROGRAM;break;
   case '6' : key = keyPressed;resetMenuCount();if(!R3.isActivated()){display(deviceId(2,MAIN));};menu = MODE_PROGRAM;break;
   case '7' : key = keyPressed;resetMenuCount();if(!R1.isActivated()){display(deviceId(3,MAIN));};menu = MODE_PROGRAM;break;
   case '8' : key = keyPressed;resetMenuCount();if(!R2.isActivated()){display(deviceId(4,MAIN));};menu = MODE_PROGRAM;break;
   case '9' : key = keyPressed;resetMenuCount();display(genericId(WEATHER));menu = MODE_PROGRAM;break;
   case '*' : line--;break;
   case '0' : key = keyPressed;jumpIncrements();break;
   case '#' : line++;break;
   case 'A' : key = keyPressed;resetMenuCount();display(genericId(TEMPERATURES));menu = MODE_PROGRAM;break;
   case 'B' : key = keyPressed;resetMenuCount();display(genericId(CONFIG));menu = MODE_PROGRAM;break;
 }
 resetTimerBeforeRefresh();
 #endif
}

void lcdDisplay(){
    updateFlashCounter();

   #ifdef KEYPAD_DISPLAY
    selectActiveKey();
    if(menu == MODE_DISPLAY){

      resetMenuCount();
      homeDisplay();
      refreshScreen(60);
    }
    if(menu == MODE_PROGRAM){
      printMenu();
      refreshScreen(60);
    }
  #endif

  #ifndef KEYPAD_DISPLAY

    resetMenuCount();
    homeDisplay();
  #endif
}
