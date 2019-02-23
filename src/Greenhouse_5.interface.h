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
char key = '1';
short menu = MODE_DISPLAY;
boolean firstPrint = true;
boolean fastPlus = false;
boolean fastMinus = false;
const byte Code_lenght = 5; // Give enough room for 4 chars + NULL char
char Data[Code_lenght] = "0000"; // 4 is the number of chars it can hold + the null char = 5

const char SETDAY[Code_lenght]    = "0001";
const char SETMONTH[Code_lenght]  = "0002";
const char SETYEAR[Code_lenght]   = "0003";
const char SETHOUR[Code_lenght]   = "0004";
const char SETMIN[Code_lenght]    = "0005";

const char MAIN[Code_lenght]      = "0010";

const char R1HYST[Code_lenght]    = "1100";
const char R1ROTUP[Code_lenght]   = "1101";
const char R1ROTDOWN[Code_lenght] = "1102";
const char R1PAUSE[Code_lenght]   = "1103";
const char R1S1MOD[Code_lenght]   = "1110";
const char R1S1TARG[Code_lenght]  = "1111";
const char R1S2MOD[Code_lenght]   = "1120";
const char R1S2TARG[Code_lenght]  = "1121";
const char R1S3MOD[Code_lenght]   = "1130";
const char R1S3TARG[Code_lenght]  = "1131";
const char R1S4MOD[Code_lenght]   = "1140";
const char R1S4TARG[Code_lenght]  = "1141";
const char R1S5MOD[Code_lenght]   = "1150";
const char R1S5TARG[Code_lenght]  = "1151";
const char R2HYST[Code_lenght]    = "1200";
const char R2ROTUP[Code_lenght]   = "1201";
const char R2ROTDOWN[Code_lenght] = "1202";
const char R2PAUSE[Code_lenght]   = "1203";
const char R2S1MOD[Code_lenght]   = "1210";
const char R2S1TARG[Code_lenght]  = "1211";
const char R2S2MOD[Code_lenght]   = "1220";
const char R2S2TARG[Code_lenght]  = "1221";
const char R2S3MOD[Code_lenght]   = "1230";
const char R2S3TARG[Code_lenght]  = "1231";
const char R2S4MOD[Code_lenght]   = "1240";
const char R2S4TARG[Code_lenght]  = "1241";
const char R2S5MOD[Code_lenght]   = "1250";
const char R2S5TARG[Code_lenght]  = "1251";
const char F1HYST[Code_lenght]    = "2100";
const char F1MOD[Code_lenght]     = "2101";
const char F2HYST[Code_lenght]    = "2200";
const char F2MOD[Code_lenght]     = "2201";
const char H1HYST[Code_lenght]    = "3100";
const char H1MOD[Code_lenght]     = "3101";
const char H2HYST[Code_lenght]    = "3200";
const char H2MOD[Code_lenght]     = "3201";
const char T1TYPE[Code_lenght]    = "4100";
const char T1HOUR[Code_lenght]    = "4101";
const char T1MIN[Code_lenght]     = "4102";
const char T1HEATT[Code_lenght]   = "4103";
const char T1COOLT[Code_lenght]   = "4104";
const char T1RAMP[Code_lenght]    = "4105";
const char T2TYPE[Code_lenght]    = "4200";
const char T2HOUR[Code_lenght]    = "4201";
const char T2MIN[Code_lenght]     = "4202";
const char T2HEATT[Code_lenght]   = "4203";
const char T2COOLT[Code_lenght]   = "4204";
const char T2RAMP[Code_lenght]    = "4205";
const char T3TYPE[Code_lenght]    = "4300";
const char T3HOUR[Code_lenght]    = "4301";
const char T3MIN[Code_lenght]     = "4302";
const char T3HEATT[Code_lenght]   = "4303";
const char T3COOLT[Code_lenght]   = "4304";
const char T3RAMP[Code_lenght]    = "4305";
const char T4TYPE[Code_lenght]    = "4400";
const char T4HOUR[Code_lenght]    = "4401";
const char T4MIN[Code_lenght]     = "4402";
const char T4HEATT[Code_lenght]   = "4403";
const char T4COOLT[Code_lenght]   = "4404";
const char T4RAMP[Code_lenght]    = "4405";
const char T5TYPE[Code_lenght]    = "4500";
const char T5HOUR[Code_lenght]    = "4501";
const char T5MIN[Code_lenght]     = "4502";
const char T5HEATT[Code_lenght]   = "4503";
const char T5COOLT[Code_lenght]   = "4504";
const char T5RAMP[Code_lenght]    = "4505";
const char TESTRELAYS[Code_lenght]= "5555";
const char TESTIIC[Code_lenght]   = "6666";


const char ADJT24H[Code_lenght]   = "4001";
const char ADJTNIGHT[Code_lenght] = "4002";
const char ADJTDAY[Code_lenght]   = "4003";
const char DIF[Code_lenght]       = "4004";
const char DIFTEMP[Code_lenght]   = "4005";
const char PRENIGHT[Code_lenght]  = "4006";
const char PRENTEMP[Code_lenght]  = "4007";
const char PRENSPEED[Code_lenght] = "4008";
const char ADJTWEA[Code_lenght]   = "4009";


const char R1OV[Code_lenght]      = "5110";
const char R1FOPEN[Code_lenght]   = "5111";
const char R1FCLOSE[Code_lenght]  = "5112";
const char R1FINC1[Code_lenght]   = "5113";
const char R1FINC2[Code_lenght]   = "5114";

const char R2OV[Code_lenght]      = "5120";
const char R2FOPEN[Code_lenght]   = "5121";
const char R2FCLOSE[Code_lenght]  = "5122";
const char R2FINC1[Code_lenght]   = "5123";
const char R2FINC2[Code_lenght]   = "5124";

const char F1OV[Code_lenght]      = "5210";
const char F1FOPEN[Code_lenght]   = "5211";
const char F1FCLOSE[Code_lenght]  = "5212";

const char F2OV[Code_lenght]      = "5220";
const char F2FOPEN[Code_lenght]   = "5221";
const char F2FCLOSE[Code_lenght]  = "5222";

const char H1OV[Code_lenght]      = "5310";
const char H1FOPEN[Code_lenght]   = "5311";
const char H1FCLOSE[Code_lenght]  = "5312";

const char H2OV[Code_lenght]      = "5320";
const char H2FOPEN[Code_lenght]   = "5321";
const char H2FCLOSE[Code_lenght]  = "5322";

const char R1STAGES[Code_lenght]  = "6001";
const char R2STAGES[Code_lenght]  = "6002";

const char MENU1[Code_lenght]  = "1000";
const char MENU2[Code_lenght]  = "2000";
const char MENU3[Code_lenght]  = "3000";
const char MENU4[Code_lenght]  = "4000";
const char MENU5[Code_lenght]  = "5000";
const char MENU6[Code_lenght]  = "6000";
const char MENU7[Code_lenght]  = "7000";
const char MENU8[Code_lenght]  = "8000";
const char MENU9[Code_lenght]  = "9000";






elapsedMillis unpressedTimer;
elapsedMillis pressedTimer;

float fvariable;
unsigned short usvariable, usvariable1, usvariable2;
short svariable, svariable1, svariable2;
byte typeSet;
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
  lcd.noBlink();
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

void initLCD(byte length, byte width){

  lcd.begin(length, width);
  lcd.setBacklightPin(BACKLIGHT_PIN, POSITIVE);
  lcd.setBacklight(HIGH);
  lcd.clear();
  lcd.createChar(1,clocks);
  lcd.createChar(2,humids);
  lcd.createChar(3,rains);
  lcd.createChar(4,locks);
}

void lcdPrintOutput(String item, byte _column, byte _row, Fan fan){

    //Fan
    lcd.setCursor(_column, _row); lcd.print(F("          "));
    lcd.setCursor(_column, _row); lcd.print(item);
    if(fan.isActive() == true){
        lcd.print(F("ON "));
      }
    else{
      lcd.print(F("OFF"));
    }
    if(fan.override() == true){
      //lcd.setCursor(_column+8, _row);
      //lcd.print ("*");
    }
    else{
      //lcd.setCursor(_column+8, _row);
      //lcd.print (" ");
    }
}
void lcdPrintOutput(String item, byte _column, byte _row, Heater heater){
    lcd.setCursor(_column, _row); lcd.print(F("          "));
    lcd.setCursor(_column, _row); lcd.print(item);
    if(heater.isActive() == true){
        lcd.print(F("ON "));
      }
    else{
      lcd.print(F("OFF"));
    }
    if(heater.override() == true){
      //lcd.setCursor(_column+8, _row);
      //lcd.print ("*");
    }
    else{
      //  lcd.setCursor(_column+8, _row);
      //lcd.print (" ");
    }
}
void lcdPrintRollups(String side, String opening, String closing, byte _column, byte _row, Rollup rollup){
  //East rollup
  lcd.setCursor(_column, _row); lcd.print(F("          "));
  lcd.setCursor(_column, _row);
  if(rollup.incrementCounter() == OFF_VAL){lcd.print(F(""));}
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
  if(rollup.override() == true){
    //lcd.setCursor(_column+8, _row);
    //lcd.print ("*");
  }
  else{
      //lcd.setCursor(_column+8, _row);
      //lcd.print (" ");
    }
  }
void lcdPrintTemp(byte _row){

    lcd.setCursor(0,_row); lcd.print(F("         "));
    lcd.setCursor(0,_row);
    if(sensorFailure == false){lcd.print(greenhouseTemperature); lcd.print(F("C"));}
    else if(sensorFailure == true){lcd.print(F("!!!"));}

    if(greenhouseHumidity != 0){
      lcd.setCursor(4,_row);
      if(sensorFailure == false){ lcd.print(F("C-   "));lcd.setCursor(6,_row); lcd.print((int)greenhouseHumidity);lcd.print("%");}
      else if(sensorFailure == true){lcd.print(F("!!!"));}
    }
}

void lcdPrintTarget(){
  lcd.setCursor(9,0); lcd.print(F("           "));
  lcd.setCursor(9,0);
  lcd.print(F("|"));
if(greenhouse.isRamping()){
  if(flashingCounter == 0){
    lcd.print((int)greenhouse.heatingTemp()); lcd.print(F("-"));lcd.print((int)greenhouse.coolingTemp());
    if(greenhouse.weather() == SUN){
     lcd.print("(SUN)");
    }
    else{
     lcd.print("(CLD)");
    }
  }
  else if(flashingCounter == 1){
    lcd.print(F("          "));
  }
}
else{
  lcd.print((int)greenhouse.heatingTemp()); lcd.print(F("-"));lcd.print((int)greenhouse.coolingTemp());
  if(greenhouse.weather() == SUN){
   lcd.print("(SUN)");
  }
  else{
   lcd.print("(CLD)");
  }
}



}

void lcdPrintTime(byte _row){
    lcd.setCursor(0,_row); lcdPrintDigits(greenhouse.rightNow(2)); lcd.print(F(":")); lcdPrintDigits(greenhouse.rightNow(1));lcd.print(F(":")); lcdPrintDigits(greenhouse.rightNow(0)); // lcd.print(F(" |")); lcdPrintDigits(greenhouse.rightNow(3)); lcd.print(F("/")); lcdPrintDigits(greenhouse.rightNow(4)); lcd.print(F("/")); lcdPrintDigits(2000+greenhouse.rightNow(5));
    lcd.setCursor(9,_row); lcd.print(F("|(TIMEP: ")); lcd.print(greenhouse.nowTimepoint()); lcd.print(F(") "));
}

void lcdPrintOutputs(){
  if(greenhouse.rollups.value() >= 1){
    lcdPrintRollups("R1:","OPENING", "CLOSING", 0, 2, R1);
  }

  if(greenhouse.rollups.value() == 2){
    lcdPrintRollups("|R2:","|OPENING", "|CLOSING", 9, 2, R2);
  }

  if((greenhouse.fans.value() == 1)&&(greenhouse.heaters.value() == 0)){
    lcdPrintOutput("F1: ", 0, 3, F1);
  }
  else if((greenhouse.fans.value() >= 1)&&(greenhouse.heaters.value() >= 1)){
    lcdPrintOutput("F1: ", 0, 3, F1);
    lcdPrintOutput("|H1: ", 9, 3, H1);
  }
  else if((greenhouse.fans.value() == 2)&&(greenhouse.heaters.value() == 0)){
    lcdPrintOutput("F1: ", 0, 3, F1);
    lcdPrintOutput("|F2: ", 9, 3, F2);
  }
  else if((greenhouse.fans.value() == 0)&&(greenhouse.heaters.value() == 1)){
    lcdPrintOutput("H1: ", 0, 3, H1);
  }
  else if((greenhouse.fans.value() == 0)&&(greenhouse.heaters.value() == 2)){
    lcdPrintOutput("H1: ", 0, 3, H1);
    lcdPrintOutput("|H2: ", 9, 3, H2);
  }
  else{
    lcd.setCursor(0, 3);
    lcd.print(F("                    "));
  }
  if(greenhouse.isActive(FIX_R1)){
      lcd.setCursor(8, 3);
      lcd.write((byte)1);
  }
  if(greenhouse.isActive(FIX_R2)){
      lcd.setCursor(19, 3);
      lcd.write((byte)1);
  }
  if(greenhouse.isActive(FIX_F1)){
      lcd.setCursor(8, 3);
      lcd.write((byte)1);
  }
  if(greenhouse.isActive(FIX_F2)){
      lcd.setCursor(19, 3);
      lcd.write((byte)1);
  }
  if(greenhouse.isActive(FIX_H1)&&greenhouse.fans.value() == 0){
      lcd.setCursor(8, 3);
      lcd.write((byte)1);
  }
  if(greenhouse.isActive(FIX_H1)&&greenhouse.fans.value() >= 1){
      lcd.setCursor(19, 3);
      lcd.write((byte)1);
  }
  if(greenhouse.isActive(FIX_H2)){
      lcd.setCursor(19, 3);
      lcd.write((byte)1);
  }

  if(greenhouse.isActive(DESHUM_AUTO1)&&greenhouse.fans.value() == 0){
      lcd.setCursor(8, 3);
      lcd.write((byte)2);
  }
  if(greenhouse.isActive(DESHUM_AUTO1)&&greenhouse.fans.value() >= 1){
      lcd.setCursor(19, 3);
      lcd.write((byte)2);
  }
  if(greenhouse.isActive(DESHUM_AUTO2)){
      lcd.setCursor(19, 3);
      lcd.write((byte)2);
  }
  if(greenhouse.isActive(DESHUM_AUTO_COLD1)){
      lcd.setCursor(8, 3);
      lcd.write((byte)2);
  }
  /*if(greenhouse.isActive(DESHUM_AUTO_COLD2)){
      lcd.setCursor(19, 3);
      lcd.write((byte)2);
  }*/
  if(rain == true){
      lcd.setCursor(8, 2);
      lcd.write((byte)3);
  }
  if((rain == true)&& (greenhouse.rollups.value() == 2)){
      lcd.setCursor(19, 2);
      lcd.write((byte)3);
  }
  if(R1.isLock()){
      lcd.setCursor(8, 2);
      lcd.write((byte)4);
  }
  if(R2.isLock()){
      lcd.setCursor(19, 2);
      lcd.write((byte)4);
  }
  if(F1.isLock()){
      lcd.setCursor(8, 3);
      lcd.write((byte)4);
  }
  if(F2.isLock()){
      lcd.setCursor(19, 3);
      lcd.write((byte)4);
  }
  if(H1.isLock()&&greenhouse.fans.value() == 0){
      lcd.setCursor(8, 3);
      lcd.write((byte)4);
  }
  if(H1.isLock()&&greenhouse.fans.value() >= 1){
      lcd.setCursor(19, 3);
      lcd.write((byte)4);
  }
  if(H2.isLock()){
      lcd.setCursor(19, 3);
      lcd.write((byte)4);
  }
}

void homeDisplay(){

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

void printHeader(String header, short maxLineOfMenu){

  //clear + print header
  if(firstPrint == true){
    unpressedTimer = 0;
    lcd.clear();
    lcd.setCursor(0,0);lcd.print(header);
    maxLine = maxLineOfMenu;
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

void rollupDisplay(Rollup rollup){

  printHeader("ROLLUP - PARAMET ",3);
  if(firstPrint == true){
    lcd.print("(");
    lcd.print(rollup.nb()+1);
    lcd.print(")");
    firstPrint = false;
  }
  adjustLine();

  //print content
  for(int x = 1; x < 4; x++){
    int writeLine = x+line;
    if (writeLine > maxLine){
      writeLine = maxLine;
    }
    switch(writeLine){
      case 1: lcd.setCursor(0,x);lcd.print(F("UP: ")); lcd.print(rollup.rotationUp.value());lcd.print(F("s")); lcd.setCursor(9,x);lcd.print(F("|DOWN: ")); lcd.print(rollup.rotationDown.value());lcd.print(F("s"));break;
      case 2: lcd.setCursor(0,x);lcd.print(F("Pause: ")); lcd.print(rollup.pause.value());lcd.print(F("s"));break;
      case 3:  lcd.setCursor(0,x);lcd.print(F("Hyst:  "));  lcd.print(rollup.hyst.value());lcd.print(F("C"));break;
    }
  }
}

void stageDisplay(Rollup rollup){

  float stageTemp1 = greenhouse.coolingTemp() + rollup.stage[1].mod.value();
  float stageTemp2 = greenhouse.coolingTemp() + rollup.stage[2].mod.value();
  float stageTemp3 = greenhouse.coolingTemp() + rollup.stage[3].mod.value();
  float stageTemp4 = greenhouse.coolingTemp() + rollup.stage[4].mod.value();

  printHeader("ROLLUP - STAGES ",greenhouse.stages.value());
  if(firstPrint == true){
    lcd.print("(");
    lcd.print(rollup.nb()+1);
    lcd.print(")");
    firstPrint = false;
  }
  adjustLine();

  //print content
  int maxLines = greenhouse.stages.value()+1;
  if(maxLines > 4){
    maxLines = 4;
  }
  for(int x = 1; x < maxLines; x++){
    int writeLine = x+line;
    if (writeLine > maxLine){
      writeLine = maxLine;
    }
    switch(writeLine){
      case 1:  lcd.setCursor(0,x);lcd.print(F("S1: ")); lcd.print(stageTemp1);lcd.print(F("-"));lcd.print(stageTemp2);lcd.print(F("-")); lcd.print(rollup.stage[1].target.value()); lcd.print(F("%"));break;
      case 2:  lcd.setCursor(0,x);lcd.print(F("S2: ")); lcd.print(stageTemp2);lcd.print(F("-"));lcd.print(stageTemp3);lcd.print(F("-")); lcd.print(rollup.stage[2].target.value()); lcd.print(F("%"));break;
      case 3:  lcd.setCursor(0,x);lcd.print(F("S3: ")); lcd.print(stageTemp3);lcd.print(F("-"));lcd.print(stageTemp4);lcd.print(F("-")); lcd.print(rollup.stage[3].target.value()); lcd.print(F("%"));break;
      case 4:  lcd.setCursor(0,x);lcd.print(F("S4: ")); lcd.print(stageTemp4);lcd.print(F("- ... "));lcd.print(F("-")); lcd.print(rollup.stage[4].target.value()); lcd.print(F("%"));break;
    }
  }
}

void fanDisplay(Fan fan){

  float onTemp = greenhouse.coolingTemp() + fan.mod.value();
  float offTemp = greenhouse.coolingTemp() + fan.mod.value() - fan.hyst.value();

  printHeader("FAN - PARAMETERS ",3);
  if(firstPrint == true){
    lcd.print("(");
    lcd.print(fan.nb()+1);
    lcd.print(")");
    firstPrint = false;
  }
  adjustLine();

  //print content
  for(int x = 1; x < 4; x++){
    int writeLine = x+line;
    if (writeLine > maxLine){
      writeLine = maxLine;
    }
    switch(writeLine){
      case 1: lcd.setCursor(0,x);lcd.print(F("Activ. temp: ")); lcd.print(onTemp);lcd.print(F("C"));break;
      case 2: lcd.setCursor(0,x);lcd.print(F("Shut. temp: ")); lcd.print(offTemp);lcd.print(F("C"));break;
      case 3: lcd.setCursor(0,x);lcd.print(F("MOD:")); if(fan.mod.value() >= 0){lcd.print(F(" "));}lcd.print(fan.mod.value()); lcd.setCursor(9,3);lcd.print(F("|HYST: ")); lcd.print(fan.hyst.value());break;
    }
  }
}

void heaterDisplay(Heater heater){

  float onTemp = greenhouse.heatingTemp() + heater.mod.value();
  float offTemp = greenhouse.heatingTemp() + heater.mod.value() + heater.hyst.value();

  printHeader("HEATER - PARAMET ",3);
  if(firstPrint == true){
    lcd.print("(");
    lcd.print(heater.nb()+1);
    lcd.print(")");
    firstPrint = false;
  }
  adjustLine();

  //print content
  for(int x = 1; x < 4; x++){
    int writeLine = x+line;
    if (writeLine > maxLine){
      writeLine = maxLine;
    }
    switch(writeLine){
      case 1: lcd.setCursor(0,x);lcd.print(F("Activ. temp: ")); lcd.print(onTemp);lcd.print(F("C"));break;
      case 2: lcd.setCursor(0,x);lcd.print(F("Shut. temp: ")); lcd.print(offTemp);lcd.print(F("C"));break;
      case 3: lcd.setCursor(0,x);lcd.print(F("MOD:")); if(heater.mod.value() >= 0){lcd.print(F(" "));}lcd.print(heater.mod.value()); lcd.setCursor(9,3);lcd.print(F("|HYST: ")); lcd.print(heater.hyst.value());break;
    }
  }
}

void timepointsDisplay(){
  printHeader("TIMEPOINTS - TIME",greenhouse.timepoints.value());
  if(firstPrint == true){
    firstPrint = false;
  }
  adjustLine();

  //print content
  int maxLines = greenhouse.timepoints.value()+1;
  if(maxLines > 4){
    maxLines = 4;
  }
  for(int x = 1; x < maxLines; x++){
    int writeLine = x+line;
    if (writeLine > maxLine){
      writeLine = maxLine;
    }

    switch(writeLine){
      case 1:
      if (greenhouse.timepoints.value() > 1){
        lcd.setCursor(0,x);lcd.print(F("TP1: ")); lcdPrintDigits(T1.hr());lcd.print(F(":"));lcdPrintDigits(T1.mn());lcd.print(F("-")); lcdPrintDigits(T2.hr());lcd.print(F(":"));lcdPrintDigits(T2.mn());

        switch (T1.type.value()){
          case 0: lcd.print("(SR)");break;
          case 1: lcd.print("(CL)");break;
          case 2: lcd.print("(SS)");break;
        }
      }
      break;
      case 2:
      if (greenhouse.timepoints.value() == 2){
        lcd.setCursor(0,x);lcd.print(F("TP2: ")); lcdPrintDigits(T2.hr());lcd.print(F(":"));lcdPrintDigits(T2.mn());lcd.print(F("-")); lcdPrintDigits(T1.hr());lcd.print(F(":"));lcdPrintDigits(T1.mn());
      }else if (greenhouse.timepoints.value() > 2){
        lcd.setCursor(0,x);lcd.print(F("TP2: ")); lcdPrintDigits(T2.hr());lcd.print(F(":"));lcdPrintDigits(T2.mn());lcd.print(F("-")); lcdPrintDigits(T3.hr());lcd.print(F(":"));lcdPrintDigits(T3.mn());
      }
      if (greenhouse.timepoints.value() >= 2){
        switch (T2.type.value()){
          case 0: lcd.print("(SR)");break;
          case 1: lcd.print("(CL)");break;
          case 2: lcd.print("(SS)");break;
        }
      }
      break;
      case 3:
      if (greenhouse.timepoints.value() == 3){
        lcd.setCursor(0,x);lcd.print(F("TP3: ")); lcdPrintDigits(T3.hr());lcd.print(F(":"));lcdPrintDigits(T3.mn());lcd.print(F("-")); lcdPrintDigits(T1.hr());lcd.print(F(":"));lcdPrintDigits(T1.mn());
      }else if (greenhouse.timepoints.value() > 3){
        lcd.setCursor(0,x);lcd.print(F("TP3: ")); lcdPrintDigits(T3.hr());lcd.print(F(":"));lcdPrintDigits(T3.mn());lcd.print(F("-")); lcdPrintDigits(T4.hr());lcd.print(F(":"));lcdPrintDigits(T4.mn());
      }
      if (greenhouse.timepoints.value() >= 3){
        switch (T3.type.value()){
          case 0: lcd.print("(SR)");break;
          case 1: lcd.print("(CL)");break;
          case 2: lcd.print("(SS)");break;
        }
      }
      break;
      case 4:
      if (greenhouse.timepoints.value() == 4){
        lcd.setCursor(0,x);lcd.print(F("TP4: ")); lcdPrintDigits(T4.hr());lcd.print(F(":"));lcdPrintDigits(T4.mn());lcd.print(F("-")); lcdPrintDigits(T1.hr());lcd.print(F(":"));lcdPrintDigits(T1.mn());
      }
      if (greenhouse.timepoints.value() == 4){
        switch (T4.type.value()){
          case 0: lcd.print("(SR)");break;
          case 1: lcd.print("(CL)");break;
          case 2: lcd.print("(SS)");break;
        }
      }
      break;
   }
  }
}

void temperaturesDisplay(){

  if(greenhouse.weather() == SUN){
    printHeader("TIMEP - TEMP(SUN)",greenhouse.timepoints.value());
  }
  else if(greenhouse.weather() == CLOUD){
    printHeader("TIMEP - TEMP(CLOUD)",greenhouse.timepoints.value());
  }
  if(firstPrint == true){
    firstPrint = false;
  }

  adjustLine();

  //print content
  int maxLines = greenhouse.timepoints.value()+1;
  if(maxLines > 4){
    maxLines = 4;
  }
  for(int x = 1; x < maxLines; x++){
    int writeLine = x+line;
    if (writeLine > maxLine){
      writeLine = maxLine;
    }
    if(greenhouse.weather() == SUN){
      switch(writeLine){
        if (greenhouse.timepoints.value() >= 1){
          case 1: lcd.setCursor(0,x);lcd.print(F("TP1: ")); lcd.print(T1.heatingTemp.value());lcd.print(F("-")); lcd.print(T1.coolingTemp.value());
            lcd.setCursor(16,x);lcd.print(F(" R  ")); lcd.setCursor(18,x);lcd.print(T1.ramping.value());break;
        }
        if (greenhouse.timepoints.value() >= 2){
          case 2: lcd.setCursor(0,x);lcd.print(F("TP2: ")); lcd.print(T2.heatingTemp.value());lcd.print(F("-")); lcd.print(T2.coolingTemp.value());
            lcd.setCursor(16,x);lcd.print(F(" R  ")); lcd.setCursor(18,x);lcd.print(T2.ramping.value());break;
        }
        if (greenhouse.timepoints.value() >= 3){
          case 3: lcd.setCursor(0,x);lcd.print(F("TP3: ")); lcd.print(T3.heatingTemp.value());lcd.print(F("-")); lcd.print(T3.coolingTemp.value());
            lcd.setCursor(16,x);lcd.print(F(" R  ")); lcd.setCursor(18,x); lcd.print(T3.ramping.value());break;
        }
        if (greenhouse.timepoints.value() == 4){
          case 4: lcd.setCursor(0,x);lcd.print(F("TP4: ")); lcd.print(T4.heatingTemp.value());lcd.print(F("-")); lcd.print(T4.coolingTemp.value());
            lcd.setCursor(16,x);lcd.print(F(" R  ")); lcd.setCursor(18,x); lcd.print(T4.ramping.value());break;
        }
      }
    }
    else if(greenhouse.weather() == CLOUD){
      switch(writeLine){
        if (greenhouse.timepoints.value() >= 1){
          case 1: lcd.setCursor(0,x);lcd.print(F("TP1: ")); lcd.print(T1.heatingTempCloud.value());lcd.print(F("-")); lcd.print(T1.coolingTempCloud.value());
            lcd.setCursor(16,x);lcd.print(F(" R  ")); lcd.setCursor(18,x); lcd.print(T1.ramping.value());break;
        }
        if (greenhouse.timepoints.value() >= 2){
          case 2: lcd.setCursor(0,x);lcd.print(F("TP2: ")); lcd.print(T2.heatingTempCloud.value());lcd.print(F("-")); lcd.print(T2.coolingTempCloud.value());
            lcd.setCursor(16,x);lcd.print(F(" R  ")); lcd.setCursor(18,x);  lcd.print(T2.ramping.value());break;
        }
        if (greenhouse.timepoints.value() >= 2){
          case 3: lcd.setCursor(0,x);lcd.print(F("TP3: ")); lcd.print(T3.heatingTempCloud.value());lcd.print(F("-")); lcd.print(T3.coolingTempCloud.value());
            lcd.setCursor(16,x);lcd.print(F(" R  ")); lcd.setCursor(18,x);  lcd.print(T3.ramping.value());break;
        }
        if (greenhouse.timepoints.value() >= 2){
          case 4: lcd.setCursor(0,x);lcd.print(F("TP4: ")); lcd.print(T4.heatingTempCloud.value());lcd.print(F("-")); lcd.print(T4.coolingTempCloud.value());
            lcd.setCursor(16,x);lcd.print(F(" R  ")); lcd.setCursor(18,x);  lcd.print(T4.ramping.value());break;
        }
      }
    }
  }
}
void geoDisplay(){

}

void sensorsDisplay(){

}

void menuDisplay(){

    if(key == '1'){
      homeDisplay();
    }
    else if (key == '2'){
      if (greenhouse.rollups.value() >= 1){
        rollupDisplay(R1);
      }
    }
    else if (key == '3'){
      if(greenhouse.rollups.value() == 2){
        rollupDisplay(R2);
      }
    }
    else if (key == '5'){
      if (greenhouse.rollups.value() >= 1){
        stageDisplay(R1);
      }
    }
    else if (key == '6'){
      if (greenhouse.rollups.value() == 2){
        stageDisplay(R2);
      }
    }
    else if (key == '4'){
      if (greenhouse.fans.value() >= 1){
        fanDisplay(F1);
      }
      else if ((greenhouse.fans.value() == 0)&&(greenhouse.heaters.value() >= 1)){
        heaterDisplay(H1);
      }
    }
    else if (key == '7'){
      if ((greenhouse.fans.value() == 2)){
        fanDisplay(F2);
      }
      else if ((greenhouse.fans.value() >= 1)&&(greenhouse.heaters.value() >= 1)){
        heaterDisplay(H1);
      }
      else if ((greenhouse.heaters.value() == 2)){
        heaterDisplay(H2);
      }
    }
    else if (key == '8'){
      timepointsDisplay();
    }
    else if (key == '9'){
      temperaturesDisplay();
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

boolean choiceIsConfirmed(){
  if((keyPressed == 'D')&&(unpressedTimer > 300)){
    return true;
  }
  else{
    return false;
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

void menuAction(){
    if(keyPressed == 'C'){
      lcd.clear();
      lcd.print("---SELECT ACTION----");
    }
    if(keyPressed == '2'){
      menu = SET_PARAMETER;
      displayNextProgram(R1OV);
    }
    if(keyPressed == '3'){
      menu = SET_PARAMETER;
      displayNextProgram(R2OV);
    }
    if(keyPressed == '4'){
      menu = SET_PARAMETER;
      displayNextProgram(F1OV);
    }
    if(keyPressed == '7'){
      menu = SET_PARAMETER;
      displayNextProgram(H1OV);
    }
    if(keyPressed == '9'){
      menu = SET_PARAMETER;
      displayNextProgram(ADJT24H);
    }
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

float fincrement(){
  if (increment == 0){
    return 0.5;
  }
  else{
    return 0.5;
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
    smodif2 += 10;
  }
  else if (line < lastline){
    smodif2 -= 10;
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

void confirmVariable(const __FlashStringHelper* header,const __FlashStringHelper* variableName, float min, float value, float max){
  adjustfvariable(min,value,max);
  firstPrint = false;
  codeWithNoDisplay = false;

  lcd.noBlink();
  lcd.setCursor(0,1);
  lcd.print(header);
  lcd.print(variableName);
  lcd.setCursor(0,2);
  lcd.print("(Press *# to adjust)");
  lcd.setCursor(0,3);
  lcd.print("      [");
  lcd.print(fvariable);
  lcd.print("]    ");


}
void confirmVariable(const __FlashStringHelper* header,const __FlashStringHelper* variableName, unsigned short min, unsigned short value, unsigned short max){
  adjustusvariable(min,value,max);
  firstPrint = false;
  codeWithNoDisplay = false;

  lcd.noBlink();
  lcd.setCursor(0,1);
  lcd.print(header);
  lcd.print(variableName);
  lcd.setCursor(0,2);
  lcd.print("(Press *# to adjust)");
  lcd.setCursor(0,3);
  lcd.print("       [");
  lcd.print(usvariable);
  lcd.print("]   ");
}
void confirmVariable(const __FlashStringHelper* header,const __FlashStringHelper* variableName, short min, short value, short max){
  adjustsvariable(min,value,max);
  firstPrint = false;
  codeWithNoDisplay = false;

  lcd.noBlink();
  lcd.setCursor(0,1);
  lcd.print(header);
  lcd.print(variableName);
  lcd.setCursor(0,2);
  lcd.print("(Press *# to adjust)");
  lcd.setCursor(0,3);
  lcd.print("       [");
  lcd.print(svariable);
  lcd.print("]   ");
}
void confirmVariable(const __FlashStringHelper* header,const __FlashStringHelper* variableName, short min_hour, short hour, short max_hour, short min_min, short min, short max_min){
  adjusttimevariable(min_hour, hour, max_hour, min_min, min, max_min);
  firstPrint = false;
  codeWithNoDisplay = false;

  lcd.noBlink();
  lcd.setCursor(0,1);
  lcd.print(header);
  lcd.print(variableName);
  lcd.setCursor(0,2);
  lcd.print("(Press *# to adjust)");
  lcd.setCursor(0,3);
  lcd.print("     [");
  lcdPrintDigits(svariable1);
  lcd.print("]:[");
  lcdPrintDigits(svariable2);
  lcd.print("]   ");
}

void confirmVariable(const __FlashStringHelper* header,const __FlashStringHelper* variableName, byte typeValue, String item1, String item2){
  short type = (short)typeValue + line;
  if(type > 1){
    type = 1;
    line -= 1;
  }
  if(type < 0){
    type = 0;
    line += 1;
  }
  typeSet = type;

  firstPrint = false;
  codeWithNoDisplay = false;
  lcd.noBlink();
  lcd.setCursor(0,1);
  lcd.print(header);
  lcd.print(variableName);
  lcd.setCursor(0,2);
  lcd.print("(Press *# to scroll)");
  lcd.setCursor(0,3);
  lcd.print("      [");
  switch (typeSet){
    case 0: lcd.print(item1);break;
    case 1: lcd.print(item2);break;
  }
  lcd.print("]   ");
}
void confirmVariable(const __FlashStringHelper* header,const __FlashStringHelper* variableName, byte typeValue, String item1, String item2,String item3){
  short type = (short)typeValue + line;
  if(type > 2){
    type = 2;
    line -= 1;
  }
  if(type < 0){
    type = 0;
    line += 1;
  }
  typeSet = type;

  firstPrint = false;
  codeWithNoDisplay = false;
  lcd.noBlink();
  lcd.setCursor(0,1);
  lcd.print(header);
  lcd.print(variableName);
  lcd.setCursor(0,2);
  lcd.print("(Press *# to scroll)");
  lcd.setCursor(0,3);
  lcd.print("      [");
  switch (typeSet){
    case 0: lcd.print(item1);break;
    case 1: lcd.print(item2);break;
    case 2: lcd.print(item3);break;
  }
  lcd.print("]   ");
}

void confirmVariable(const __FlashStringHelper* header,const __FlashStringHelper* variableName, byte typeValue, String item1, String item2,String item3,String item4 ){
  short type = (short)typeValue + line;
  if(type > 3){
    type = 3;
    line -= 1;
  }
  if(type < 0){
    type = 0;
    line += 1;
  }
  typeSet = type;

  firstPrint = false;
  codeWithNoDisplay = false;
  lcd.noBlink();
  lcd.setCursor(0,1);
  lcd.print(header);
  lcd.print(variableName);
  lcd.setCursor(0,2);
  lcd.print("(Press *# to scroll)");
  lcd.setCursor(0,3);
  lcd.print("      [");
  switch (typeSet){
    case 0: lcd.print(item1);break;
    case 1: lcd.print(item2);break;
    case 2: lcd.print(item3);break;
    case 3: lcd.print(item4);break;
  }
  lcd.print("]   ");
}

void confirmVariable(const __FlashStringHelper* header,const __FlashStringHelper* variableName, byte typeValue, String item[], byte menuLenght){
  short type = (short)typeValue + line;
  if(type > menuLenght-1){
    type = menuLenght-1;
    line -= 1;
  }
  if(type < 0){
    type = 0;
    line += 1;
  }
  typeSet = type;

  firstPrint = false;
  codeWithNoDisplay = false;
  lcd.noBlink();
  lcd.setCursor(0,1);
  lcd.print(header);
  lcd.print(variableName);
  lcd.setCursor(0,2);
  lcd.print("(Press *# to scroll)");
  lcd.setCursor(0,3);
  lcd.print("      [");
  lcd.print(item[typeSet]);
  lcd.print("]   ");
}

void setParameter(const __FlashStringHelper* header,const __FlashStringHelper* variableName, floatParameter &parameter, const char* returnid){
    confirmVariable(header,variableName,parameter.minimum(),parameter.value(),parameter.maximum());
    if(choiceIsConfirmed()){
      parameter.setValue(fvariable);
      displayNextProgram(returnid);
    }
}
void setParameter(const __FlashStringHelper* header,const __FlashStringHelper* variableName, uShortParameter &parameter, const char* returnid){
    confirmVariable(header,variableName,parameter.minimum(),parameter.value(),parameter.maximum());
    if(choiceIsConfirmed()){
      parameter.setValue(usvariable);
      displayNextProgram(returnid);
    }
}
void setParameter(const __FlashStringHelper* header,const __FlashStringHelper* variableName, shortParameter &parameter, const char* returnid){
    confirmVariable(header,variableName,parameter.minimum(),parameter.value(),parameter.maximum());
    if(choiceIsConfirmed()){
      parameter.setValue(svariable);
      displayNextProgram(returnid);
    }
}
void checkIIC(){
  lcd.noBlink();
  lcd.setCursor(0,0);
  lcd.print("----I2C DEVICES-----");

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
    lcd.setCursor(7,3);
    lcd.print(F("INF"));
  }
}


void rollupMenu(const __FlashStringHelper* header, Rollup &rollup){
  if(greenhouse.rollups.value() >= rollup.nb()+1){
    char index = (rollup.nb()+1) +'0';

    char RHYST[Code_lenght]    = {'1',index,'0','0'};
    char RROTUP[Code_lenght]   = {'1',index,'0','1'};
    char RROTDOWN[Code_lenght] = {'1',index,'0','2'};
    char RPAUSE[Code_lenght]   = {'1',index,'0','3'};
    char RS1MOD[Code_lenght]   = {'1',index,'1','1'};
    char RS1TARG[Code_lenght]  = {'1',index,'1','2'};
    char RS2MOD[Code_lenght]   = {'1',index,'2','1'};
    char RS2TARG[Code_lenght]  = {'1',index,'2','2'};
    char RS3MOD[Code_lenght]   = {'1',index,'3','1'};
    char RS3TARG[Code_lenght]  = {'1',index,'3','2'};
    char RS4MOD[Code_lenght]   = {'1',index,'4','1'};
    char RS4TARG[Code_lenght]  = {'1',index,'4','2'};

    if(programSelected(RHYST)){
      setParameter(header,F("HYSTERESIS"),rollup.hyst,RROTUP);
    }
    if(programSelected(RROTUP)){
      setParameter(header,F("ROTATION (UP)  "),rollup.rotationUp,RROTDOWN);
    }
    if(programSelected(RROTDOWN)){
      setParameter(header,F("ROTATION (DOWN)"),rollup.rotationDown,RPAUSE);
    }
    if(programSelected(RPAUSE)){
      setParameter(header,F("PAUSE TIME     "), rollup.pause,RS1MOD);
    }
    if(greenhouse.stages.value() >= 1){
      if(programSelected(RS1MOD)){
        setParameter(header,F("STAGE 1 MOD   "),rollup.stage[1].mod,RS1TARG);
      }
      if(greenhouse.stages.value() == 1){
        if(programSelected(RS1TARG)){
          setParameter(header,F("STAGE 1 TARGET"),rollup.stage[1].target,MENU1);
        }
      }
      else{
        if(programSelected(RS1TARG)){
          setParameter(header,F("STAGE 1 TARGET"),rollup.stage[1].target,RS2MOD);
        }
      }
    }
    if(greenhouse.stages.value() >= 2){
      if(programSelected(RS2MOD)){
        setParameter(header,F("STAGE 2 MOD   "),rollup.stage[2].mod,RS2TARG);
      }
      if(greenhouse.stages.value() == 2){
        if(programSelected(RS2TARG)){
          setParameter(header,F("STAGE 2 TARGET"),rollup.stage[2].target,MENU1);
        }
      }
      else{
        if(programSelected(RS2TARG)){
          setParameter(header,F("STAGE 2 TARGET"),rollup.stage[2].target,RS3MOD);
        }
      }
    }
    if(greenhouse.stages.value() >= 3){
      if(programSelected(RS3MOD)){
        setParameter(header,F("STAGE 3 MOD   "),rollup.stage[3].mod,RS3TARG);
      }
      if(greenhouse.stages.value() == 3){
        if(programSelected(RS3TARG)){
          setParameter(header,F("STAGE 3 TARGET"),rollup.stage[3].target,MENU1);
        }
      }
      else{
        if(programSelected(RS3TARG)){
          setParameter(header,F("STAGE 3 TARGET"),rollup.stage[3].target,RS4MOD);
        }
      }
    }
    if(greenhouse.stages.value() >= 4){
      if(programSelected(RS4MOD)){
        setParameter(header,F("STAGE 4 MOD   "),rollup.stage[4].mod,RS4TARG);
      }
      if(programSelected(RS4TARG)){
        setParameter(header,F("STAGE 4 TARGET"),rollup.stage[4].target,MENU1);
      }
    }
  }
}

void adjustAndSetTimepoint(Timepoint &timepoint){

    short hour;
    short minut;

    if(timepoint.type.value() == SR){
      hour = svariable1 - (short)Timepoint::sunRise[HOUR];
      minut = svariable2 - (short)Timepoint::sunRise[MINUT];
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

void timepointMenu(const __FlashStringHelper* header, Timepoint &timepoint){

  if(greenhouse.timepoints.value() >= timepoint.nb()+1){
    char index = (timepoint.nb()+1) +'0';

    char TTIME[Code_lenght] = {'4',index,'0','0'};
    char TTYPE[Code_lenght]    = {'4',index,'0','1'};
    char THEATTS[Code_lenght]   = {'4',index,'0','3'};
    char THEATTC[Code_lenght]   = {'4',index,'0','4'};
    char TCOOLTS[Code_lenght]   = {'4',index,'0','5'};
    char TCOOLTC[Code_lenght]   = {'4',index,'0','6'};
    char TRAMP[Code_lenght]   = {'4',index,'0','7'};

    if(programSelected(TTIME)){
      confirmVariable(header,F("SET TIME"), 0,timepoint.hr(),23, 0,timepoint.mn(),59);
      if(choiceIsConfirmed()){
        adjustAndSetTimepoint(timepoint);
        displayNextProgram(TTYPE);

      }
    }
    if(programSelected(TTYPE)){
      confirmVariable(header,F("TIME REF     "),timepoint.type.value(), "SUNRISE", "CLOCK", "SUNSET");
      if(choiceIsConfirmed()){

        timepoint.type.setValue(typeSet);
        adjustAndSetTimepoint(timepoint);

        displayNextProgram(THEATTS);
      }
    }
    if(programSelected(THEATTS)){
      setParameter(header,F("HEAT T*(SUN)   "),timepoint.heatingTemp,THEATTC);
    }
    if(programSelected(THEATTC)){
      setParameter(header,F("HEAT T*(CLOUD) "),timepoint.heatingTempCloud,TCOOLTS);
    }
    if(programSelected(TCOOLTS)){
      setParameter(header,F("COOL T*(SUN)   "),timepoint.coolingTemp,TCOOLTC);
    }
    if(programSelected(TCOOLTC)){
      setParameter(header,F("COOL T*(CLOUD) "),timepoint.coolingTemp,TRAMP);
    }
    if(programSelected(TRAMP)){
      setParameter(header,F("RAMPING       "),timepoint.ramping,MENU1);
    }
  }
}
void fanMenu(const __FlashStringHelper* header, Fan &fan){

  if(greenhouse.fans.value() >= fan.nb()+1){
    char index = (fan.nb()+1) +'0';

    char FHYST[Code_lenght]    = {'2',index,'0','0'};
    char FMOD[Code_lenght]   = {'2',index,'0','1'};

    if(programSelected(FHYST)){
      setParameter(header,F("HYSTERESIS"),fan.hyst,FMOD);
    }
    if(programSelected(FMOD)){
      setParameter(header,F("MODIFICATOR"),fan.mod,MENU1);
    }
  }
}
void heaterMenu(const __FlashStringHelper* header, Heater &heater){

  if(greenhouse.heaters.value() >= heater.nb()+1){
    char index = (heater.nb()+1) +'0';

    char HHYST[Code_lenght]    = {'3',index,'0','0'};
    char HMOD[Code_lenght]   = {'3',index,'0','1'};

    if(programSelected(HHYST)){
      setParameter(header,F("HYSTERESIS"),heater.hyst,HMOD);
    }
    if(programSelected(HMOD)){
      setParameter(header,F("MODIFICATOR"),heater.mod,MENU1);
    }
  }
}


void menuSetParameter(){
  if(firstPrint == true){
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("-Parameter Selected-"));
  }


//Général
  if(programSelected(SETDAY)){
    unsigned short daySet = (unsigned short)greenhouse.rightNow(3);
    confirmVariable(F(""),F("SET DAY"),0,daySet,31);
    if(choiceIsConfirmed()){
      #if  RTC == RTC_DS3231
        unsigned short year = 2000+(unsigned short)greenhouse.rightNow(5);
        rtc.setDate((byte)usvariable, (byte)greenhouse.rightNow(4), year);
        getDateAndTime();
        greenhouse.setNow(rightNow);
        greenhouse.solarCalculations();
      #endif
      displayMenu('1');
    }
  }
  if(programSelected(SETMONTH)){
    unsigned short monthSet = (unsigned short)greenhouse.rightNow(4);
    confirmVariable(F(""),F("SET MONTH"),1,monthSet,12);
    if(choiceIsConfirmed()){
        #if  RTC == RTC_DS3231
        unsigned short year = 2000+(unsigned short)greenhouse.rightNow(5);
        rtc.setDate( (byte)greenhouse.rightNow(3),(byte)usvariable, year);
        getDateAndTime();
        greenhouse.setNow(rightNow);
        greenhouse.solarCalculations();
      #endif
      displayMenu('1');
    }
  }
  if(programSelected(SETYEAR)){
    unsigned short yearSet = (unsigned short)greenhouse.rightNow(5);
    confirmVariable(F(""),F("SET YEAR"),0,yearSet,99);
    if(choiceIsConfirmed()){
        #if  RTC == RTC_DS3231
        unsigned short year = 2000 + usvariable;
        rtc.setDate( (byte)greenhouse.rightNow(3), (byte)greenhouse.rightNow(4), year);
        getDateAndTime();
        greenhouse.setNow(rightNow);
        greenhouse.solarCalculations();
      #endif
      displayMenu('1');
    }
  }
  if(programSelected(SETHOUR)){
    confirmVariable(F(""),F("SET HOUR"),0,(unsigned short)greenhouse.rightNow(2),23);
    if(choiceIsConfirmed()){
      #if  RTC == RTC_DS3231
        hourSet = usvariable;
        if(!(rightNow[2] == greenhouse.rightNow(2))){
          if(hourSet > 0){
            hourSet -= 1;
          }
          else{
            hourSet = 23;
          }
        }
        rtc.setTime(hourSet, rightNow[1], 0);
        displayNextProgram(SETMIN);
      #endif
    }
  }



  if(programSelected(SETMIN)){
    confirmVariable(F(""),F("SET MINUTS"),0,(unsigned short)greenhouse.rightNow(1),59);
    if(choiceIsConfirmed()){
      #if  RTC == RTC_DS3231
        minSet = usvariable;
        rtc.setTime(rightNow[2], minSet, 0);
        displayMenu('1');
      #endif
    }
  }
  if(programSelected(MAIN)){
    String item[15];
    byte counter = 0;
    item[counter] = "Adjust";
    counter++;
    item[counter] = "T day";
    counter++;
    item[counter] = "T night";
    counter++;

    for(int x = 1; x <= greenhouse.rollups.value();x++){
      String device = "Rollup";
      String title = device + x;
      item[counter] = title;
      counter++;
    }
    for(int x = 1; x <= greenhouse.fans.value();x++){
      String device = "Fan";
      String title = device + x;
      item[counter] = title;
      counter++;
    }
    for(int x = 1; x <= greenhouse.heaters.value();x++){
      String device = "Heater";
      String title = device + x;
      item[counter] = title;
      counter++;
    }
    item[counter] = "Sensors";
    counter++;
    item[counter] = "Config";
    counter++;

    confirmVariable(F(""),F("     MAIN MENU     "), 1, item, counter);
    if(choiceIsConfirmed()){
      switch(typeSet) {
        case 0 :break;
        case 1 :displayNextProgram("1100");break;
        case 2 :break;
        case 3 :break;
        case 4 :break;
      }
    }
  }

  //Rollup 1
rollupMenu(F("R1 - "), R1);
rollupMenu(F("R2 - "), R2);
fanMenu(F("F1 - "), F1);
fanMenu(F("F2 - "), F2);
heaterMenu(F("H1 - "), H1);
heaterMenu(F("H2 - "), H2);

timepointMenu(F("DIF: "), T1);
timepointMenu(F("DAY: "), T2);
timepointMenu(F("PREN.: "), T3);
timepointMenu(F("NIGHT: "), T4);

  if(programSelected(TESTRELAYS)){
    checkRelays();
  }

  if(programSelected(TESTIIC)){
    checkIIC();
    if(choiceIsConfirmed()){
      displayMenu('1');
    }
  }

  if(programSelected(R1OV)){
    confirmVariable(F(""),F("   R1 - OVERRIDE"), 0, "AUTO", "FOPEN", "FCLOSE", "FINCR");
    if(choiceIsConfirmed()){
      switch (typeSet) {
        case 0: R1.unlock();displayMenu('1');break;
        case 1: displayNextProgram(R1FOPEN);break;
        case 2: displayNextProgram(R1FCLOSE);break;
        case 3: displayNextProgram(R1FINC1);break;
      }
    }
  }

  if(programSelected(R1FOPEN)){
    confirmVariable(F(""),F("  R1 - DELAY (MIN)"), (unsigned short)0, (unsigned short)0, (unsigned short)1440);
    zeroEqualInfinity();
    if(choiceIsConfirmed()){
      if(usvariable != 0){
        R1.lockAtIncrement(100, usvariable);
      }
      else{
        R1.lockOpen();
      }
      displayMenu('1');
    }
  }

  if(programSelected(R1FCLOSE)){
    confirmVariable(F(""),F("  R1 -  DELAY (MIN)"), (unsigned short)0, (unsigned short)0, (unsigned short)1440);
    zeroEqualInfinity();
    if(choiceIsConfirmed()){
      if(usvariable != 0){
        R1.lockAtIncrement(0, usvariable);
      }
      else{
        R1.lockClose();
      }
      displayMenu('1');
    }
  }
  if(programSelected(R1FINC1)){
    confirmVariable(F(""),F("R1 - INCREMENT"), (unsigned short)0, R1.incrementCounter(), (unsigned short)100);
    if(choiceIsConfirmed()){
      usvariable1 = usvariable;
      displayNextProgram(R1FINC2);
    }
  }
  if(programSelected(R1FINC2)){
  confirmVariable(F(""),F("  R1 -  DELAY (MIN)"), (unsigned short)0, (unsigned short)0, (unsigned short)1440);
  zeroEqualInfinity();
    if(choiceIsConfirmed()){
      usvariable2 = usvariable;
      if(usvariable2 == 0){
        R1.lockAtIncrement(usvariable1);
      }
      else{
        R1.lockAtIncrement(usvariable1, usvariable2);
      }
      displayMenu('1');
    }
  }

  if(programSelected(R2OV)){
    confirmVariable(F(""),F("   R2 - OVERRIDE"), 0, "AUTO", "FOPEN", "FCLOSE", "FINCR");
    if(choiceIsConfirmed()){
      switch (typeSet) {
        case 0: R2.unlock();displayMenu('1');break;
        case 1: displayNextProgram(R2FOPEN);break;
        case 2: displayNextProgram(R2FCLOSE);break;
        case 3: displayNextProgram(R2FINC1);break;
      }
    }
  }

  if(programSelected(R2FOPEN)){
    confirmVariable(F(""),F("  R2 - DELAY (MIN)"), (unsigned short)0, (unsigned short)0, (unsigned short)1440);
    zeroEqualInfinity();
    if(choiceIsConfirmed()){
      if(usvariable != 0){
        R2.lockAtIncrement(100, usvariable);
      }
      else{
        R2.lockOpen();
      }
      displayMenu('1');
    }
  }

  if(programSelected(R2FCLOSE)){
    confirmVariable(F(""),F("  R2 -  DELAY (MIN)"), (unsigned short)0, (unsigned short)0, (unsigned short)1440);
    zeroEqualInfinity();
    if(choiceIsConfirmed()){
      if(usvariable != 0){
        R2.lockAtIncrement(0, usvariable);
      }
      else{
        R2.lockClose();
      }
      displayMenu('1');
    }
  }
  if(programSelected(R2FINC1)){
    confirmVariable(F(""),F("R2 - FORCE TO INC"), (unsigned short)0, R2.incrementCounter(), (unsigned short)100);
    if(choiceIsConfirmed()){
      usvariable1 = usvariable;
      displayNextProgram(R2FINC2);
    }
  }
  if(programSelected(R2FINC2)){
  confirmVariable(F(""),F("  R2 - DELAY (MIN)"), (unsigned short)0, (unsigned short)0, (unsigned short)1440);
  zeroEqualInfinity();
    if(choiceIsConfirmed()){
      usvariable2 = usvariable;
      if(usvariable2 == 0){
        R2.lockAtIncrement(usvariable1);
      }
      else{
        R2.lockAtIncrement(usvariable1, usvariable2);
      }
      displayMenu('1');
    }
  }

  if(programSelected(F1OV)){
    confirmVariable(F(""),F("   F1 - OVERRIDE"), 0, "AUTO", "FORCE ON", "FORCE OFF");
    if(choiceIsConfirmed()){
      switch (typeSet) {
        case 0: F1.unlock();displayMenu('1');break;
        case 1: displayNextProgram(F1FOPEN);break;
        case 2: displayNextProgram(F1FCLOSE);break;
      }
    }
  }

  if(programSelected(F1FOPEN)){
    confirmVariable(F(""),F("  F1 - DELAY (MIN)"), (unsigned short)0, (unsigned short)0, (unsigned short)1440);
    zeroEqualInfinity();
    if(choiceIsConfirmed()){
      if(usvariable != 0){
        F1.lockOnAndWait(usvariable);
      }
      else{
        F1.lockOn();
      }
      displayMenu('1');
    }
  }

  if(programSelected(F1FCLOSE)){
    confirmVariable(F(""),F("  F1 - DELAY (MIN)"), (unsigned short)0, (unsigned short)0, (unsigned short)1440);
    zeroEqualInfinity();
    if(choiceIsConfirmed()){
      if(usvariable != 0){
        F1.lockOffAndWait(usvariable);
      }
      else{
        F1.lockOff();
      }
      displayMenu('1');
    }
  }

  if(programSelected(F2OV)){
    confirmVariable(F(""),F("   F2 - OVERRIDE"), 0, "AUTO", "FORCE ON", "FORCE OFF");
    if(choiceIsConfirmed()){
      switch (typeSet) {
        case 0: F2.unlock();displayMenu('1');break;
        case 1: displayNextProgram(F2FOPEN);break;
        case 2: displayNextProgram(F2FCLOSE);break;
      }
    }
  }

  if(programSelected(F2FOPEN)){
    confirmVariable(F(""),F("  F2 -  DELAY (MIN)"), (unsigned short)0, (unsigned short)0, (unsigned short)1440);
    zeroEqualInfinity();
    if(choiceIsConfirmed()){
      if(usvariable != 0){
        F2.lockOnAndWait(usvariable);
      }
      else{
        F2.lockOn();
      }
      displayMenu('1');
    }
  }

  if(programSelected(F2FCLOSE)){
    confirmVariable(F(""),F("  F2 -  DELAY (MIN)"), (unsigned short)0, (unsigned short)0, (unsigned short)1440);
    zeroEqualInfinity();
    if(choiceIsConfirmed()){
      if(usvariable != 0){
        F2.lockOffAndWait(usvariable);
      }
      else{
        F2.lockOff();
      }
      displayMenu('1');
    }
  }

  if(programSelected(H1OV)){
    confirmVariable(F(""),F("   H1 - OVERRIDE"), 0, "AUTO", "FORCE ON", "FORCE OFF");
    if(choiceIsConfirmed()){
      switch (typeSet) {
        case 0: H1.unlock();displayMenu('1');break;
        case 1: displayNextProgram(H1FOPEN);break;
        case 2: displayNextProgram(H1FCLOSE);break;
      }
    }
  }

  if(programSelected(H1FOPEN)){
    confirmVariable(F(""),F("  H1 -  DELAY (MIN)"), (unsigned short)0, (unsigned short)0, (unsigned short)1440);
    zeroEqualInfinity();
    if(choiceIsConfirmed()){
      if(usvariable != 0){
        H1.lockOnAndWait(usvariable);
      }
      else{
        H1.lockOn();
      }
      displayMenu('1');
    }
  }

  if(programSelected(H1FCLOSE)){
    confirmVariable(F(""),F("  H1 -  DELAY (MIN)"), (unsigned short)0, (unsigned short)0, (unsigned short)1440);
    zeroEqualInfinity();
    if(choiceIsConfirmed()){
      if(usvariable != 0){
        H1.lockOffAndWait(usvariable);
      }
      else{
        H1.lockOff();
      }
      displayMenu('1');
    }
  }

  if(programSelected(H2OV)){
    confirmVariable(F(""),F("   H2 - OVERRIDE"), 0, "AUTO", "FORCE ON", "FORCE OFF");
    if(choiceIsConfirmed()){
      switch (typeSet) {
        case 0: H2.unlock();displayMenu('1');break;
        case 1: displayNextProgram(H2FOPEN);break;
        case 2: displayNextProgram(H2FCLOSE);break;
      }
    }
  }

  if(programSelected(H2FOPEN)){
    confirmVariable(F(""),F("  H2 -  DELAY (MIN)"), (unsigned short)0, (unsigned short)0, (unsigned short)1440);
    zeroEqualInfinity();
    if(choiceIsConfirmed()){
      if(usvariable != 0){
        H2.lockOnAndWait(usvariable);
      }
      else{
        H2.lockOn();
      }
      displayMenu('1');
    }
  }

  if(programSelected(H2FCLOSE)){
    confirmVariable(F(""),F("  H2 -  DELAY (MIN)"), (unsigned short)0, (unsigned short)0, (unsigned short)1440);
    zeroEqualInfinity();
    if(choiceIsConfirmed()){
      if(usvariable != 0){
        H2.lockOffAndWait(usvariable);
      }
      else{
        H2.lockOff();
      }
      displayMenu('1');
    }
  }

  if(programSelected(ADJT24H)){
    confirmVariable(F(""),F("    ADJUST T-24H"), (float)-10, (float)0, (float)10);
    if(choiceIsConfirmed()){
      for(int x = 0; x < greenhouse.timepoints.value(); x++){
        greenhouse.timepoint[x].heatingTemp.setValue(greenhouse.timepoint[x].heatingTemp.value()+fvariable);
        greenhouse.timepoint[x].heatingTempCloud.setValue(greenhouse.timepoint[x].heatingTempCloud.value()+fvariable);
        greenhouse.timepoint[x].coolingTemp.setValue(greenhouse.timepoint[x].coolingTemp.value()+fvariable);
        greenhouse.timepoint[x].coolingTempCloud.setValue(greenhouse.timepoint[x].coolingTempCloud.value()+fvariable);
      }
      displayMenu('9');
    }
  }

  if(programSelected(ADJTNIGHT)){
    confirmVariable(F(""),F("   ADJUST T-NIGHT"), (float)-10, (float)0, (float)10);
    if(choiceIsConfirmed()){
      for(int x = 0; x < greenhouse.timepoints.value(); x++){
        if(!isBetween(Timepoint::sunRise[2],Timepoint::sunRise[1],greenhouse.timepoint[x].hr(),greenhouse.timepoint[x].mn(),Timepoint::sunSet[2],Timepoint::sunSet[1])){
          greenhouse.timepoint[x].heatingTemp.setValue(greenhouse.timepoint[x].heatingTemp.value()+fvariable);
          greenhouse.timepoint[x].heatingTempCloud.setValue(greenhouse.timepoint[x].heatingTempCloud.value()+fvariable);
          greenhouse.timepoint[x].coolingTemp.setValue(greenhouse.timepoint[x].coolingTemp.value()+fvariable);
          greenhouse.timepoint[x].coolingTempCloud.setValue(greenhouse.timepoint[x].coolingTempCloud.value()+fvariable);
        }
      }
      displayMenu('9');
    }
  }
  if(programSelected(ADJTDAY)){
    confirmVariable(F(""),F("    ADJUST T-DAY"), (float)-10, (float)0, (float)10);
    if(choiceIsConfirmed()){
      for(int x = 0; x < greenhouse.timepoints.value(); x++){
        if(isBetween(Timepoint::sunRise[2],Timepoint::sunRise[1],greenhouse.timepoint[x].hr(),greenhouse.timepoint[x].mn(),Timepoint::sunSet[2],Timepoint::sunSet[1])){
          greenhouse.timepoint[x].heatingTemp.setValue(greenhouse.timepoint[x].heatingTemp.value()+fvariable);
          greenhouse.timepoint[x].heatingTempCloud.setValue(greenhouse.timepoint[x].heatingTempCloud.value()+fvariable);
          greenhouse.timepoint[x].coolingTemp.setValue(greenhouse.timepoint[x].coolingTemp.value()+fvariable);
          greenhouse.timepoint[x].coolingTempCloud.setValue(greenhouse.timepoint[x].coolingTempCloud.value()+fvariable);
        }
      }
      displayMenu('9');
    }
  }
  if(programSelected(DIF)){
    confirmVariable(F(""),F("   DIF-TIME"), 0, "ENABLE", "DISABLE");
    if(choiceIsConfirmed()){
      if(typeSet == 0){
        displayNextProgram(DIFTEMP);
      }
      if(typeSet == 1){
        T1.heatingTemp.setValue(T4.heatingTemp.value());
        T1.heatingTempCloud.setValue(T4.heatingTempCloud.value());
        T1.coolingTemp.setValue(T4.coolingTemp.value());
        T1.coolingTempCloud.setValue(T4.coolingTempCloud.value());
        displayMenu('9');
      }
    }
  }
  if(programSelected(DIFTEMP)){
    confirmVariable(F(""),F("DIF TEMPERATURE RISE"), (float)0, (float)0, (float) 10);
    if(choiceIsConfirmed()){
      T1.heatingTemp.setValue(T4.heatingTemp.value()+fvariable);
      T1.heatingTempCloud.setValue(T4.heatingTempCloud.value()+fvariable);
      T1.coolingTemp.setValue(T4.coolingTemp.value()+fvariable);
      T1.coolingTempCloud.setValue(T4.coolingTempCloud.value()+fvariable);
      displayMenu('9');
    }
  }
  if(programSelected(PRENIGHT)){
    confirmVariable(F(""),F("   PRENIGHT DROP"), 0, "ENABLE", "DISABLE");
    if(choiceIsConfirmed()){
      if(typeSet == 0){
        displayNextProgram(PRENTEMP);
      }
      if(typeSet == 1){
        T3.heatingTemp.setValue(T2.heatingTemp.value());
        T3.heatingTempCloud.setValue(T2.heatingTempCloud.value());
        T3.coolingTemp.setValue(T2.coolingTemp.value());
        T3.coolingTempCloud.setValue(T2.coolingTempCloud.value());
        T3.ramping.setValue(T2.ramping.value());
        displayMenu('9');
      }
    }
  }
  if(programSelected(PRENTEMP)){
    confirmVariable(F(""),F(" PRENIGHT TEMP DROP"), (float)-10, (float)0, (float) 0);
    if(choiceIsConfirmed()){
      T3.heatingTemp.setValue(T2.heatingTemp.value()+fvariable);
      T3.heatingTempCloud.setValue(T2.heatingTempCloud.value()+fvariable);
      T3.coolingTemp.setValue(T2.coolingTemp.value()+fvariable);
      T3.coolingTempCloud.setValue(T2.coolingTempCloud.value()+fvariable);
      displayNextProgram(PRENSPEED);
    }
  }
  if(programSelected(PRENSPEED)){
    confirmVariable(F(""),F("PREN SPEED(-1C/Xmin)"), (unsigned short)0, (unsigned short)0, (unsigned short) 15);
    if(choiceIsConfirmed()){
      T3.ramping.setValue(usvariable);
      displayMenu('9');
    }
  }
  if(programSelected(R1STAGES)){
    confirmVariable(F(""),F(" ADJ R1 STAGES MODS"), (float)-5, (float)0, (float) 5);
    if(choiceIsConfirmed()){
      for(int x = 0; x < greenhouse.stages.value();x++ ){
        R1.stage[x].mod.setValue(R1.stage[x].mod.value() + fvariable);
      }
      displayMenu('5');
    }
  }
  if(programSelected(R2STAGES)){
    confirmVariable(F(""),F(" ADJ R2 STAGES MODS"), (float)-5, (float)0, (float) 5);
    if(choiceIsConfirmed()){
      for(int x = 0; x < greenhouse.stages.value();x++ ){
        R2.stage[x].mod.setValue(R2.stage[x].mod.value() + fvariable);
      }
      displayMenu('6');
    }
  }
  if(programSelected(ADJTWEA)){
    confirmVariable(F(""),F("  ADJUST WEATHER"), greenhouse.weather(), "SUN", "CLOUD");
    if(choiceIsConfirmed()){
      greenhouse.setWeather(typeSet);
      displayMenu('9');
    }
  }
  if(programSelected(MENU1)){
    displayMenu('1');
  }
  if(programSelected(MENU2)){
    displayMenu('2');
  }
  if(programSelected(MENU3)){
    displayMenu('3');
  }
  if(programSelected(MENU4)){
    displayMenu('4');
  }
  if(programSelected(MENU5)){
    displayMenu('5');
  }
  if(programSelected(MENU6)){
    displayMenu('6');
  }
  if(programSelected(MENU7)){
    displayMenu('7');
  }
  if(programSelected(MENU8)){
    displayMenu('8');
  }
  if(programSelected(MENU9)){
    displayMenu('9');
  }

  if(codeWithNoDisplay && menu == SET_PARAMETER){
    menu = MODE_PROGRAM;clearMenu();clearData();
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
 if(keyPressed != NO_KEY && keyPressed != 'D'){
   unpressedTimer = 0;
 }
}


void selectActiveKey(){
 #ifdef KEYPAD_DISPLAY
 keyPressed = keypad.getKey();

 switch(keyPressed){
   case '1' : key = keyPressed;if(menu == MODE_DISPLAY){firstPrint = true;}break;
   case '2' : key = keyPressed;if(menu == MODE_DISPLAY){firstPrint = true;}break;
   case '3' : key = keyPressed;if(menu == MODE_DISPLAY){firstPrint = true;}break;
   case '4' : key = keyPressed;if(menu == MODE_DISPLAY){firstPrint = true;}break;
   case '5' : key = keyPressed;if(menu == MODE_DISPLAY){firstPrint = true;}break;
   case '6' : key = keyPressed;if(menu == MODE_DISPLAY){firstPrint = true;}break;
   case '7' : key = keyPressed;if(menu == MODE_DISPLAY){firstPrint = true;}break;
   case '8' : key = keyPressed;if(menu == MODE_DISPLAY){firstPrint = true;}break;
   case '9' : key = keyPressed;if(menu == MODE_DISPLAY){firstPrint = true;}break;
   case '*' : line--;break;
   case '0' : key = keyPressed;if(menu == MODE_DISPLAY){firstPrint = true;}if(menu == SET_PARAMETER){jumpIncrements();}break;
   case '#' : line++;break;
   case 'A' : menu = MODE_DISPLAY;key = '1';clearMenu();clearData();break;
   case 'B' : menu = MODE_PROGRAM;clearMenu();clearData();break;
   case 'C' : menu = MODE_ACTION;clearMenu();clearData();break;
   case 'D' : break;
 }
 resetTimerBeforeRefresh();
 #endif
}

void lcdDisplay(){
    updateFlashCounter();

   #ifdef KEYPAD_DISPLAY
    selectActiveKey();
    if(menu == MODE_DISPLAY){
      menuDisplay();
      refreshScreen(15);
    }
    if(menu == MODE_PROGRAM){
      menuProgram();
      refreshScreen(30);
    }
    if(menu == SET_PARAMETER){
      menuSetParameter();
      refreshScreen(30);
    }
    if(menu == MODE_ACTION){
      menuAction();
      refreshScreen(30);
    }
  #endif

  #ifndef KEYPAD_DISPLAY
    homeDisplay();
  #endif
}
