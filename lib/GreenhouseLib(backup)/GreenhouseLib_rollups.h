/*
  GreenhouseLib_rollups.h
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
#include "EEPROM.h"
#include "elapsedMillis.h"
#include "Parameters.h"
#include "Defines.h"
#include "GreenhouseLib_overrides.h"

#ifndef GreenhouseLib_rollups_h
#define GreenhouseLib_rollups_h




/*
//A counting template...

template <typename T>
struct counter{
    counter(){count++;}
    virtual ~counter(){--count;}
    int count;
};

template <typename T> int counter<T>::count( 0 );
*/


class Stage
{
  public:
    Stage();
    ~Stage();
    uShortParameter target;
    floatParameter mod;
    unsigned short nb();

  private:
		unsigned short _localCounter;
		static unsigned short _counter;
};

/*
OBJECT : Rollup
Parameters :
- activation temperature
- hysteresis
- time of rotation for full opening (max. 250 sec.)
- time of rotation for a full closing (max. 250 sec.)
- number of increments (max. 99)
- pause time between increments (max. 250 sec.)
*/

struct Override{
  byte type;
  byte ID;

  boolParameter active;
  byteParameter hrStart;
  byteParameter mnStart;
  byteParameter hrStop;
  byteParameter mnStop;
  byteParameter delay;
  byteParameter target;
  byteParameter comparator;
  floatParameter floatVar;
  boolParameter boolVar;
  elapsedMillis counter;
  //boolean active;
  //unsigned short target;
};

bool isBetween();

class Rollup
{
  public:
    //initialization functions
    Rollup();
    ~Rollup();
    void initOutputs(boolean relayType, byte rOpen, byte rClose);
    void initStage(Stage stage, float mod, byte inc);
    void initOverride(byte number, unsigned short targetIncrement);
    void setOverride(byte number, boolean state);
    /*void createOverride(byte ID, byte priority, byte hrStart, byte mnStart, byte hrStop, byte mnStop, byte target);
    void createOverride(byte ID, byte priority, byte mn, byte target);
    void createOverride(byte ID, byte priority, byte comparator, float value, byte target);
    void createOverride(byte ID, byte priority, bool value, byte target);



    void checkOverride(byte ID,byte hrStart, byte mnStart, byte hrStop, byte mnStop);
    void checkOverride(byte ID);
    void checOverride(byte ID,float variable);
    void checkOverride(byte ID,bool variable);
    */
    void lockOpen();
    void lockClose();
    void lockAtIncrement(byte increment);
    void unlock();



    //routihe functions
		void routine(float target, float temp);

    //Parameters functions
    void setParameters(float hyst, unsigned short rotationUp, unsigned short rotationDown, unsigned short pause);
    void setStages(byte stages);
    void setIncrementCounter(unsigned short increment);
    void EEPROMGet();
    void EEPROMPut();
		//return private variables
    Override overRide[MAX_OVERRIDES];
    Stage stage[MAX_STAGES];
    floatParameter hyst;
    uShortParameter rotationUp;
    uShortParameter rotationDown;
    uShortParameter pause;

    unsigned short increments();

    byte activeOverride();
    unsigned short incrementCounter();
    boolean isMoving();
    boolean isWaiting();
    boolean override();
    boolean opening();
    boolean closing();
    unsigned short nb();

    boolean TEST_parameterOffLimits();



    //Parameters
  private:
    byte _increments;
    //const parameters
    byte _openingPin;
    byte _closingPin;
    boolean _relayType;
    boolean _activate;
    boolean _desactivate;

		//Logic variables
		unsigned short _incrementCounter;
    unsigned long _upStep;
    unsigned long _downStep;
    unsigned long _pauseTime;
		unsigned long _moveTime;
    unsigned long _overrideDuration;
    short _incrementMove;
    short _stageMove;
    boolean _opening;
    boolean _closing;
    boolean _routine;
    byte _activeOverride;
    boolean _overrideCycle;
    boolean _routineCycle;
		unsigned short _stage;
    unsigned short _upperStage;
    unsigned short _lowerStage;
		unsigned short _stages;

		//Timers
		elapsedMillis rollupTimer;
		elapsedMillis debugTimer;

		//Indexes
    static unsigned short _EEPROMindex;
    unsigned short _localIndex;
		unsigned short _localCounter;
		static unsigned short _counter;

		//private functions
    //Actions
    void watchRoutine();
    void watchOverride();
    void updatePosition(float temp, float targetTemp);
    void updatePosition(byte overrideNumber);
    void startMove();
    void stopMove();
    void resumeCycle(String type);

    void startOpening();
    void stopOpening();
    void startClosing();
    void stopClosing();

    void action(byte pin, boolean state);

    void calibrateStages();
    void updateTimings();
    void checkStageSuccession();

		void printPause();
		void printEndPause();
    void debugPrints();

};


#endif
