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
#include "GreenhouseLib_devices.h"

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

class Rollup  : public OverrideManager
{
  public:
    //initialization functions
    Rollup();
    ~Rollup();
    void initOutputs(byte outputType, boolean relayType, byte rOpen, byte rClose);
    void initStage(Stage stage, float mod, byte inc);

    void lockOpen();
    void lockClose();
    void lockOpenAndWait(unsigned long seconds);
    void lockCloseAndWait(unsigned long seconds);
    void lockAtIncrement(byte increment);
    void lockAtIncrement(byte increment, unsigned long seconds);
    void forceStop();
    //routihe functions
		void routine(float target, float temp);
    void checkCurrent(float current);
    //Parameters functions
    void setParameters(byte stages, float hyst, unsigned short rotationUp, unsigned short rotationDown, unsigned short pause, bool enabled);
    void setIncrementCounter(unsigned short increment);
    void EEPROMGet();
		//return private variables
    byteParameter stages;
    floatParameter hyst;
    uShortParameter rotationUp;
    uShortParameter rotationDown;
    uShortParameter pause;
    boolParameter enabled;
    floatParameter currentLimit;


    Stage stage[MAX_STAGES];

    unsigned short increments();

    //byte activeOverride();
    unsigned short incrementCounter();
    boolean isMoving();
    boolean isWaiting();
    boolean override();
    boolean opening();
    boolean closing();
    unsigned short nb();

		unsigned short EEPROMIndexBegin();
		unsigned short EEPROMIndexEnd();

    boolean TEST_parameterOffLimits();
    void setTest(boolean state);


    void desactivateDevice();
    void activateDevice();
    boolean isActivated();


    void checkOverrideTimer();
    void resetLockTimer(unsigned long seconds);
    void keepLockInMemory(byte increment);

    void unlock();


  private:

    boolParameter lock;
    byteParameter lockTarget;

    elapsedMillis overrideTimer;
    bool lockedAndWaiting;
    unsigned long overrideWaitingTime;

    boolean _test;
    byte _increments;
    //const parameters
    Output _openingPin;
    Output _closingPin;

		//Logic variables
		unsigned short _incrementCounter;
    unsigned long _upStep;
    unsigned long _downStep;
    unsigned long _pauseTime;
		unsigned long _moveTime;

    unsigned short _targetIncement;
    short _incrementMove;
    short _stageMove;
    boolean _routine;
    byte _activeOverride;
    boolean _routineCycle;
		unsigned short _stage;
    unsigned short _upperStage;
    unsigned short _lowerStage;
		unsigned short _stages;
    boolean _reset;

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

    void autoAdjustStages();
    void calibrateStages();
    void updateTimings();
    void checkStageSuccession();

		void printPause();
		void printEndPause();
    void debugPrints();

};


#endif
