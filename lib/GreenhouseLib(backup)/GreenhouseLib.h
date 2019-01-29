/*
  GreenhouseLib.h
  Copyright (C)2017 Loup Hébert-Chartrand. All right reserved

  You can find the latest version of this code at :
  https://github.com/LoupHC/GreenhouseLib

  This library is free software; you can redistribute it and/or
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

#include "Arduino.h"
#include "TimeLord.h"
#include "elapsedMillis.h"
#include "Parameters.h"
#include "Defines.h"
#include "GreenhouseLib_rollups.h"
#include "GreenhouseLib_FF.h"
#include "GreenhouseLib_timing.h"



#ifndef GreenhouseLib_h
#define GreenhouseLib_h


struct Sequence{
  unsigned short onTime;
  unsigned short offTime;
};

class Alarm
{
  public :
    Alarm(boolean type, byte alarmPin);
    ~Alarm();
    void init();
    //Single alarm setup
    void above(float parameter, float value);
    void below(float parameter, float value);
    void offRange(float parameter, float minimum, float maximum);
    void conditionalTo(boolean value);
    //Multiple alarm setup
    void addSequence(byte number, unsigned short on, unsigned short off);
    void above(float parameter, float value, byte sequence);
    void below(float parameter, float value, byte sequence);
    void offRange(float parameter, float minimum, float maximum, byte sequence);
    void conditionalTo(boolean value, byte sequence);
    //Execute the alarm
    void checkAlarm();

  private :
    void activate(boolean state);
    elapsedMillis counter;
    Sequence sequence[MAX_ALARM_SEQUENCES];
    byte _sequencePlaying;
    byte _alarmType;
    byte _alarmPin;
};

struct OverrideProgram{
  unsigned short ID;
  byte priority;
  boolean active;
  boolean rollup[MAX_ROLLUPS];
  boolean fan[MAX_FANS];
  boolean heater[MAX_HEATERS];
};

class Greenhouse
{
  public:
    Greenhouse(int timezone, float latitude, float longitude, byte timepoints, byte rollups, byte stages, byte fans, byte heaters);
    ~Greenhouse();
    void setNow(byte rightNow[]);
    void setWeather(byte weather);
    void solarCalculations();
    void startingTime(byte rightNow[]);
    void startingParameters();
    void EEPROMUpdate();

    TimeLord myLord;
    #if MAX_TIMEPOINTS >= 1
      Timepoint timepoint[MAX_TIMEPOINTS];
    #endif
    #if MAX_ROLLUPS >= 1
      Rollup rollup[MAX_ROLLUPS];
    #endif
    #if MAX_FANS >= 1
      Fan fan[MAX_FANS];
    #endif
    #if MAX_HEATERS >= 1
      Heater heater[MAX_HEATERS];
    #endif

    OverrideProgram overrideProgram[MAX_OVERRIDE_PROGRAMS];

    //Confirm timepoint, cooling/heating temperatures, routine for each outputs
    void fullRoutine(byte rightNow[], float greenhouseTemperature);
    void fullRoutine(byte rightNow[6], float* coolingTemp, float* heatingTemp);

    void createOverrideProgram(unsigned short ID, byte priorityIndex);
    byte overridePriority(unsigned short ID);
    boolean overrideIsActive(unsigned short ID);

    void setOverrideProgram(unsigned short ID, boolean state);
    void setOutput(unsigned short ID, Rollup rollup, unsigned short target);
    void setOutput(unsigned short ID, Fan fan, boolean state);
    void setOutput(unsigned short ID, Heater heater, boolean state);
    void disableOutput(unsigned short ID, Rollup rollup);
    void disableOutput(unsigned short ID, Fan fan);
    void disableOutput(unsigned short ID, Heater heater);

    byte rightNow(byte index);
    byte weather();
    byte nowTimepoint();
    float heatingTemp();
    float coolingTemp();
    boolean isRamping();




  private:
    byte  _overrideProgramCounter;
    byte _timezone;         //time zone of your location (Most of Quebec : -5)
    float _longitude;
    float _latitude;
    byte _rightNow[6];      //actual time
    byte _sunTime[6];       //for sunrise, sunset calculation
    float _heatingTemp;     //reference temperature for heating devices
    float _coolingTemp;     //reference temperature for cooling devices
    float _newHeatingTemp;     //reference temperature for heating devices
    float _newCoolingTemp;     //reference temperature for cooling devices
    boolean _ramping;

    byte _timepoint;        //actual timepoint
    byte _weather;

    byte _timepoints;        //# of timepoints
    byte _rollups;           //# of rollups
    byte _stages;
    byte _fans;              //# of fans
    byte _heaters;           //# of heaters

    byte _alarmPin;
    boolean _alarmEnabled;
    boolean _alarmRelayType;
    boolean _alarmIsTriggered;
    float _alarmMin;
    float _alarmMax;

    void clearOverrides(byte overrideProgram);
    void establishPriority(byte overrideProgram);
    void initTimeLord(int timezone, float latitude, float longitude);
    void setSunrise();
    void setSunset();
    void selectActualProgram();
    void checkProgramSuccession();
    void setTempCible();
    void startRamping();

    elapsedMillis ramping;  //ramping timer
    elapsedMillis beep;


};

#endif
