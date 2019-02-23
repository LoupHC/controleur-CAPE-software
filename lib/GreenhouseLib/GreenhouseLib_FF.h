/*
  GreenhouseLib_FF.h
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

#ifndef GreenhouseLib_FF_h
#define GreenhouseLib_FF_h


class OnOffDevice : public Device
{
  public:

      //initialization functions
<<<<<<< HEAD
=======
      OnOffDevice();
      ~OnOffDevice();

>>>>>>> 8635b916547bc6428a90a9dd528a5a01848050a7
      void initOutput(byte outputType, boolean relayType, byte pin);

      void lockOn();
      void lockOff();
      void lockOnAndWait(int minuts);
      void lockOffAndWait(int minuts);
      void unlock();
      void resetLockTimer();
      boolean isLock();

      //programmation functions
      void setParameters(float temp, float hyst);

			//return private variables
      floatParameter hyst;
      floatParameter mod;
			boolean isActive();
      boolean override();

  		unsigned short EEPROMIndexBegin();
  		unsigned short EEPROMIndexEnd();

      boolean TEST_parameterOffLimits();

<<<<<<< HEAD
      boolean TEST_parameterOffLimits();

=======
>>>>>>> 8635b916547bc6428a90a9dd528a5a01848050a7

    protected:
      Output _output;
      byte _activeOverride;/*
      void stop();
      void start();
			//Parameters
      boolean _relayType;
      boolean _activate;
      boolean _desactivate;
			byte _pin;
<<<<<<< HEAD

			//Timer
      elapsedMillis EEPROMTimer;*/

=======
*/
			//Indexes
			unsigned short _localIndex;
      static unsigned short _EEPROMindex;
  		unsigned short _localCounter;
  		static unsigned short _counter;
			//Timer
      elapsedMillis EEPROMTimer;
>>>>>>> 8635b916547bc6428a90a9dd528a5a01848050a7


};


/*
OBJECT : Fan (cooling unit)

Parameters :
- activation temperature
- hysteresis
*/
class Fan : public OnOffDevice
{
public:
<<<<<<< HEAD
  Fan();
  void routine(float target, float temp);
  void EEPROMGet();
  unsigned short nb();
  unsigned short EEPROMIndexBegin();
  unsigned short EEPROMIndexEnd();

private:
  unsigned short _localCounter;
  static unsigned short _counter;
  unsigned short _localIndex;
  static unsigned short _EEPROMindex;

=======
  void routine(float target, float temp);
  void EEPROMGet();
>>>>>>> 8635b916547bc6428a90a9dd528a5a01848050a7
};

/*
OBJECT : Heater (heating unit)

Parameters :
- hysteresis
- activation temperature
*/

class Heater : public OnOffDevice
{
public:
<<<<<<< HEAD
  Heater();
  void routine(float target, float temp);
  void EEPROMGet();
  unsigned short nb();
  unsigned short EEPROMIndexBegin();
  unsigned short EEPROMIndexEnd();

private:
  unsigned short _localCounter;
  static unsigned short _counter;
  unsigned short _localIndex;
  static unsigned short _EEPROMindex;
=======
  void routine(float target, float temp);
  void EEPROMGet();
>>>>>>> 8635b916547bc6428a90a9dd528a5a01848050a7
};
#endif
